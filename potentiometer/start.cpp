
#include "start.h"

void powerSequence(bool state);
void inputSequence(int input);
bool volumeSave(void *v);
bool enableSpeakers(void *v);
bool enableSoftPower(void *v);
bool enableLoading(void *v);
bool loadingProgress(void *v);
bool lastStepLoading(void *v);
bool enableOn(void *v);
bool goOff(void *v);

unsigned char values[MAX_VALUES];
static int lastStoredVolume = P_UNDETERMINED;
static unsigned long volumeRC5Millis = 0;
static RC5 *rc5 = NULL;
static bool isLoaded = false;
static int loadingCounter = 0;
static bool isOnPowerOffState = false;

bool isSystemLoaded(void) {
  return isLoaded;
}

bool isPowerDownSequenceActive(void) {
  return isOnPowerOffState;
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
 
  initMainPIO();
  initBasicPIO();

  initSPI();
  TFT *tft = initTFT();
  tft->fillScreen(ICONS_BG_COLOR);

  initI2C();
  pcf8574_init();
  Wire.end();

  initI2C();

  #ifdef RESET_EEPROM
  resetEEPROM();
  #endif

  initPeripherials();
  rc5 = initRC5(PIN_RC5);
  rc5->setDefaultCallback();

  #ifdef I2C_SCANNER
  i2cScanner();
  #endif

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

static int dimmerValue;
bool fadeDimmer(void *v) {

  if(!isErrorActive()) {
    lcdBrightness(dimmerValue);
    if(dimmerValue == BRIGHTNESS_AMBIENT) {
      return false;
    }
    dimmerValue--;
    return true;
  } else {
    lcdBrightness(BRIGHTNESS_MAX);
    return false;
  }
}

bool mainScreenDimmer(void *v) {
  dimmerValue = BRIGHTNESS_MAX;
  setupDimmerSequence(BRIGHTNESS_FADER_STEP_TIME, fadeDimmer);
  return false;
}

void launchDimmer(void) {
  lcdBrightness(BRIGHTNESS_MAX);
  if(!isErrorActive()) {
    launchDimmerAt(SCREEN_DIMMER * SECOND, mainScreenDimmer);
  } else {
    cancelDimmerTask();
  }
}

void looper(void) {

  if(isPowerDownSequenceActive()) {
    looperSequence();
    return;
  }

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

  additionalPowerHandle();

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
    if(command == RC5_MUTE) {
      muteSequence(!isMuteON());
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
    lcdBrightness(BRIGHTNESS_MAX);
    clearError();
    if(!setupErrorDetection()) {
      launchTaskAt(TIME_DELAY_SPEAKERS * SECOND, enableSpeakers);
      launchTaskAt(TIME_DELAY_SOFT_POWER * SECOND, enableSoftPower);
  #ifdef SKIP_INTRO
      lastStepLoading(NULL);
  #else
      launchTaskAt(TIME_INTRO_TIME_KERNEL_ACOUSTIC * SECOND, enableLoading);
      showKernelAcousticLogo();
    } else {
      isLoaded = true;      
    }
#endif
  } else {
    isOnPowerOffState = true;
    speakers(false);
    softPower(false);
    mute(false);
    cancelTimerTasks();
    pcf8574_init();
    drawBye();
    launchTaskAt(TIME_DELAY_BYE * SECOND, goOff);
  }
}

bool goOff(void *v) {
  lcdBrightness(0);
  clearScreen();
  isOnPowerOffState = false;
  setupTimers();
  callAtEverySecond(NULL);
  return false;
}

void muteSequence(bool state) {
  if(isSystemLoaded()) {
    values[V_MUTE] = state;
    mute(values[V_MUTE]);
    storeValuesToEEPROM();
    drawMute(isMuteON());
  }
}

void inputSequence(int input) {
  if(isSystemLoaded()) {
    input = limitInputs(input);
    if(input < 0) {
      return;
    }
    if(values[V_SELECTED_INPUT] != input) {
      values[V_SELECTED_INPUT] = input;
      if(!values[V_MUTE]) {
        mute(true);
      }
      selectInput(input);
      storeValuesToEEPROM();
      redrawInput();
      delay(TIME_MUTE_BBM);
      mute(values[V_MUTE]);
    }
  }
}

void increaseInput(void) {
  if(isErrorActive()) {
    return;
  }
  int input = values[V_SELECTED_INPUT];
  input++;
  if(input > getAmountOfHardwareInputs() - 1) {
    input = 0;
  }
  input = limitInputs(input);
  if(input < 0) {
    input = 0;
  }
  inputSequence(input);
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
  if(isErrorActive()) {
    isLoaded = true;
    return false;
  }

  drawSmallKernelAcousticLogo();
  drawLoadingSequence(loadingCounter++);
  setupTimerWith(UNSYNCHRONIZE_TIME, int(float(float(TIME_LOAD) / float(LOADING_STEPS)) * SECOND),
                        loadingProgress);
  return false;
}

bool loadingProgress(void *v) {
  if(isErrorActive()) {
    isLoaded = true;
    return false;
  }
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
  selectInput(values[V_SELECTED_INPUT]);

  isLoaded = true;
  redrawScreen();

  return false;
}

void additionalPowerHandle(void) {

  if(!isPowerON()) {
    if(isAdditionalPowerPresed()) {
      while(isAdditionalPowerPresed()) {
        looperSequence();
      }
      powerSequence(!isPowerON());
    }
  } else {
    if(isAdditionalPowerPresed()) {
      unsigned long timeStart = millis() + (TIME_ENCODER_PRESS_TO_OFF * SECOND);
      while(isAdditionalPowerPresed()) {
        looperSequence();
        if(millis() > timeStart) {
          powerSequence(!isPowerON());
          while(isAdditionalPowerPresed()) {
            looperSequence();
          }
          return;
        }
      }
      increaseInput(); 
    }
  }

}
