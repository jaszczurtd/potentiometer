
#include "start.h"

void powerSequence(bool state);
bool displayValues(void *v);

unsigned char values[MAX_VALUES];

void setupTimers(void) {
  int time = SECOND;
  createTimerObject();
  setupTimerWith(UNSYNCHRONIZE_TIME, time, callAtEverySecond);
  setupTimerWith(UNSYNCHRONIZE_TIME, time / 8, displayValues);
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
      mute(!isMuteON());
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