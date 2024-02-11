
#include "start.h"

void powerSequence(bool state);
void muteSequence(bool state);
void inputSequence(int input);
bool volumeSave(void *v);
bool enableSpeakers(void *v);
bool enableSoftPower(void *v);
bool enableLoading(void *v);
bool loadingProgress(void *v);
bool lastStepLoading(void *v);
bool enableOn(void *v);

unsigned char values[MAX_VALUES];
static int lastStoredVolume = P_UNDETERMINED;
static unsigned long volumeRC5Millis = 0;
static RC5 *rc5 = NULL;
static bool isLoaded = false;
static int loadingCounter = 0;

bool isSystemLoaded(void) {
  return isLoaded;
}

void setupTimers(void) {
  int time = SECOND;
  createTimerObject();
  setupTimerWith(UNSYNCHRONIZE_TIME, time, callAtEverySecond);
  setupTimerWith(UNSYNCHRONIZE_TIME, time / 10, displayValues);
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
  rc5 = initRC5(PIN_RC5);
  rc5->setDefaultCallback();

  #ifdef I2C_SCANNER
  i2cScanner();
  #endif

  tft->fillScreen(ICONS_BG_COLOR);

  generateGradient(loadingGradient, LOADING_COLOR_1, LOADING_COLOR_2, LOADING_STEPS);
  setupTimers();

  callAtEverySecond(NULL);

#ifdef SKIP_OFF
  launchTaskAt(SKIP_OFF_SECONDS_TO_START * SECOND, enableOn);
#endif
}

static bool alertBlink = false;
bool alertSwitch(void) {
  return alertBlink;
}

bool callAtEverySecond(void *argument) {
  alertBlink = (alertBlink) ? false : true;
  digitalWrite(LED_BUILTIN, alertBlink);
  return true; 
}

void looperSequence(void) {
  timerTick();
  delay(CORE_OPERATION_DELAY);  
}

void looper(void) {

  bool received = false;
  int c = rc5Loop(), command = RC5_NONE;
  if(c != RC5_NONE) {
    received = true;
    command = c;
  }

  bool loopControl = true;
  while(received && loopControl) {
    c = rc5Loop();
    if(c == RC5_NONE) {
      break;
    }

    switch(c) {
      case RC5_VOLUME_DOWN:
      case RC5_VOLUME_UP:
        if(volumeRC5Millis == 0) {
          volumeRC5Millis = millis() + TIME_DELAY_VOLUME_RC5_CONTROL;
        }
        loopControl = false;
        break;
      default:
        break;
    }

    looperSequence();
  }

  if(isPowerPressed()) {
    while(isPowerPressed()) {
      looperSequence();
    }
    powerSequence(!isPowerON());
  }
  if(command == RC5_POWER) {
    powerSequence(!isPowerON());
  }

  if(isPowerON()) {
    if(isMutePressed()) {
      while(isMutePressed()) {
        looperSequence();
      }
      muteSequence(!isMuteON());
      drawMute(isMuteON());
    }
    if(command == RC5_MUTE) {
      muteSequence(!isMuteON());
      drawMute(isMuteON());
    }

    if(!values[V_MUTE]) {
      muteWithEncoderSupport();
    }

    int input = readInputsKeyboardState();
    if(input != -1) {
      inputSequence(input);
    }

    input = translateRC5ToInputState(command);
    if(input != -1) {
      inputSequence(input);
    }

    if(volumeRC5Millis > 0 &&
      (command == RC5_VOLUME_UP || 
      command == RC5_VOLUME_DOWN)) {

      if(volumeRC5Millis < millis()) {
        if(command == RC5_VOLUME_UP) {
          volumeUp();
        }
        if(command == RC5_VOLUME_DOWN) {
          volumeDown();
        }
        volumeRC5Millis = 0;
      }
    }

    if(lastStoredVolume != values[V_VOLUME]) {
      lastStoredVolume = values[V_VOLUME];
      setVol(lastStoredVolume);
    }
  }

  looperSequence();
}

void initialization1(void) {
  deb("Second core initialized");
}

void looper1(void) {

  delay(CORE_OPERATION_DELAY);  
}

void powerSequence(bool state) {
  power(state);
  loadingCounter = 0;
  isLoaded = false;

  if(state) {
    softInitDisplay();
    launchTaskAt(TIME_DELAY_SPEAKERS * SECOND, enableSpeakers);
    launchTaskAt(TIME_DELAY_SOFT_POWER * SECOND, enableSoftPower);
    clearError();
#ifdef SKIP_INTRO
    lastStepLoading(NULL);
#else
    launchTaskAt(TIME_INTRO_TIME_KERNEL_ACOUSTIC * SECOND, enableLoading);
    showKernelAcousticLogo();
#endif
  } else {
    cancelTimerTasks();
    clearScreen();
    speakers(false);
    softPower(false);
    pcf8574_init();
    mute(false);
    setupTimers();
  }
}

void muteSequence(bool state) {
  values[V_MUTE] = state;
  mute(values[V_MUTE]);
  storeValuesToEEPROM();
}

void inputSequence(int input) {
  selectInput(input);
  values[V_SELECTED_INPUT] = input;
  storeValuesToEEPROM();
  redrawInput();
}

bool enableOn(void *v) {
  powerSequence(true);
  return false;
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

bool enableSpeakers(void *v) {
  speakers(!isErrorActive());
  return false;
}

bool enableSoftPower(void *v) {
  softPower(!isErrorActive());
  return false;
}

bool enableLoading(void *v) {
  drawSmallKernelAcousticLogo();
  drawLoadingSequence(loadingCounter++);
  setupTimerWith(UNSYNCHRONIZE_TIME, int(float(float(TIME_LOAD) / float(LOADING_STEPS)) * SECOND),
                        loadingProgress);
  return false;
}

bool loadingProgress(void *v) {
  drawLoadingSequence(loadingCounter++);
  if(loadingCounter > LOADING_STEPS) {
    launchTaskAt(TIME_INTRO_TIME_KERNEL_ACOUSTIC * SECOND, lastStepLoading);
    showKernelAcousticLogo();
    return false;
  }
  return true;
}

bool lastStepLoading(void *v) {
  restoreValuesFromEEPROM();
  lastStoredVolume = values[V_VOLUME];
  setVol(values[V_VOLUME]); 

  isLoaded = true;
  redrawScreen();

  return false;
}

