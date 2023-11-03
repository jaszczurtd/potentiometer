
#include "start.h"

void powerSequence(bool state);
bool displayValues(void *v);
bool volumeSave(void *v);

unsigned char values[MAX_VALUES];
static int lastStoredVolume = P_UNDETERMINED;

void setupTimers(void) {
  int time = SECOND;
  createTimerObject();
  setupTimerWith(UNSYNCHRONIZE_TIME, time, callAtEverySecond);
  setupTimerWith(UNSYNCHRONIZE_TIME, time / 8, displayValues);
  setupTimerWith(UNSYNCHRONIZE_TIME, time / 4, volumeSave);
}

void initialization(void) {

  Serial.begin(9600);
 
  TFT *tft = initTFT();

  initI2C();
  pcf8574_init();
  Wire.end();

  initSPI();
  initI2C();

  #ifdef RESET_EEPROM
  resetEEPROM();
  #endif

  initBasicPIO();
  initPeripherials();

  //powerSequence(LOW);

  #ifdef I2C_SCANNER
  i2cScanner();
  #endif

  tft->fillScreen(ICONS_BG_COLOR);

  setupTimers();

  callAtEverySecond(NULL);
}

static bool alertBlink = false;
bool alertSwitch(void) {
  return alertBlink;
}

bool callAtEverySecond(void *argument) {
  alertBlink = (alertBlink) ? false : true;
  digitalWrite(LED_BUILTIN, alertBlink);

  //regular draw - low importance values
//  drawLowImportanceValues();
  return true; 
}

void looper(void) {
  timerTick();

  if(isPowerPressed()) {
    while(isPowerPressed()) {
      timerTick();
    }
    powerSequence(!isPowerON());
  }

  if(isPowerON()) {
    if(isMutePressed()) {
      while(isMutePressed()) {
        timerTick();
      }
      values[V_MUTE] = !isMuteON();
      mute(values[V_MUTE]);
      storeValuesToEEPROM();
    }

    if(!values[V_MUTE]) {
      muteWithEncoderSupport();
    }

    int input = checkAndApplyInputs();
    if(input != -1) {
      selectInput(input);
      values[V_SELECTED_INPUT] = input;
      storeValuesToEEPROM();
    }

    if(lastStoredVolume != values[V_VOLUME]) {
      lastStoredVolume = values[V_VOLUME];
      setVol(lastStoredVolume);
    }
  }

  delay(CORE_OPERATION_DELAY);  
}

void initialization1(void) {
  deb("Second core initialized");
}

void looper1(void) {

  delay(CORE_OPERATION_DELAY);  
}

void powerSequence(bool state) {
  power(state);
  softInitDisplay();
  redraw();
  if(!state) {
    pcf8574_init();
    mute(false);
  } else {
    restoreValuesFromEEPROM();
    lastStoredVolume = values[V_VOLUME];
    setVol(values[V_VOLUME]);    
  }
}

static bool redrawScreen = false;
void redraw(void) {
  redrawScreen = true;
}

bool displayValues(void *v) {

  if(!isPowerON()){
    return true;
  }

  if(redrawScreen) {
    TFT *tft = returnTFTReference();

    tft->prepareText("volume: %d", values[V_VOLUME]);
    tft->fillRect(0, 0, 160, 100, ICONS_BG_COLOR);
    tft->sansBoldWithPosAndColor(10, 30, 0xffffff);
    tft->printlnFromPreparedText();
    redrawScreen = false;
  }

  return true;
}

static int lastVolume = 0;
void storeControlLocalVolume(void) {
  lastVolume = values[V_VOLUME];
}
bool volumeSave(void *v) {
  if(lastVolume != values[V_VOLUME]) {
    storeControlLocalVolume();
    storeValuesToEEPROM();
  }
  return true;
}