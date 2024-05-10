#include "peripherials.h"

void encoder(void);
void encoderSupport(void);
void encInit(void);
void errorInt(void);

static unsigned char inputsTab[] = {
  PIN_INPUT_6, PIN_INPUT_5, PIN_INPUT_4, PIN_INPUT_3, PIN_INPUT_1, PIN_INPUT_2
};

int getAmountOfHardwareInputs(void) {
  return sizeof(inputsTab);
} 

void initMainPIO(void) {
  analogWriteResolution(ANALOG_WRITE_RESOLUTION);
  lcdBrightness(0);
}

void initPeripherials(void) {
  pinMode(PIN_POWER_IN, INPUT_PULLUP);
  pinMode(PIN_POWER_OUT, OUTPUT);

  pinMode(PIN_ADD_POWER_IN, INPUT_PULLUP);
  pinMode(PIN_MUTE_OUT, OUTPUT);

  for(int a = 0; a < (int)sizeof(inputsTab); a++) {
    pinMode(inputsTab[a], INPUT_PULLUP);
  }

  pinMode(PIN_INPUTS_AMOUNT, INPUT_PULLUP);

  pinMode(PIN_SPEAKERS, OUTPUT);
  pinMode(PIN_SOFTPOWER, OUTPUT);

  encInit();
}

void lcdBrightness(int val) {
  analogWrite(PIN_LCD_BRIGHTNESS, ((1 << ANALOG_WRITE_RESOLUTION) - 1) - val);
}

void power(bool state) {
  digitalWrite(PIN_POWER_OUT, state);
}

bool isPowerON(void) {
  return digitalRead(PIN_POWER_OUT);
}

bool isPowerPressed(void) {
  return !digitalRead(PIN_POWER_IN);
}

bool isAdditionalPowerPresed(void) {
  return !digitalRead(PIN_ADD_POWER_IN);
}

void mute(bool state) {
  digitalWrite(PIN_MUTE_OUT, state);
}

bool isMuteON(void) {
  return digitalRead(PIN_MUTE_OUT);
}

void speakers(bool state) {
  digitalWrite(PIN_SPEAKERS, state);
}

void softPower(bool state) {
  digitalWrite(PIN_SOFTPOWER, state);
}

static volatile int lastVal = P_UNDETERMINED;
static volatile int support = P_UNDETERMINED;
static volatile unsigned long previousEncoderMillis = 0; 
static volatile unsigned long currentEncoderMillis;

void encInit(void) {
  pinMode(PIN_ENC_L, INPUT_PULLUP);
  pinMode(PIN_ENC_R, INPUT_PULLUP);
  attachInterrupt(PIN_ENC_R, encoder, CHANGE);
  attachInterrupt(PIN_ENC_L, encoderSupport, CHANGE);
  currentEncoderMillis = millis();
}

void volumeUp(void) {
  if(values[V_VOLUME] < MAX_VOLUME) {
    values[V_VOLUME]++;
    if(isMuteON()) {
      muteSequence(false);
    }
    redrawVolume();
  }
}

void volumeDown(void) {
  if(values[V_VOLUME] > 0) {
    values[V_VOLUME]--;
    if(isMuteON()) {
      muteSequence(false);
    }
    redrawVolume();
  }      
}

void encoderSupport(void) {
  if(isPowerON() && isSystemLoaded()) {
    support = digitalRead(PIN_ENC_L) | digitalRead(PIN_ENC_R) << 1;
  }
}

void encoder(void) {
  if(isPowerON() && isSystemLoaded()) {
    int val = digitalRead(PIN_ENC_L) | digitalRead(PIN_ENC_R) << 1;
    if(val != lastVal) {
      if(!values[V_MUTE]) {
        currentEncoderMillis = millis();
      }

      if((lastVal == P_UNDETERMINED && val == 2 && support == P_UNDETERMINED) || 
        (lastVal == P_UNDETERMINED && val == 0 && support == 0) ||
        (lastVal == 2 && val == 0 && support == 0) || 
        (lastVal == 0 && val == 2 && support == 0) ||
        (lastVal == 2 && val == 0 && support == 3) ||
        (lastVal == 3 && val == 0 && support == 0) ||
        (lastVal == 0 && val == 2 && support == 2)) {

        volumeUp();
      } else

      if((lastVal == P_UNDETERMINED && val == 3 && support == P_UNDETERMINED) ||
        (lastVal == P_UNDETERMINED && val == 3 && support == 3) ||
        (lastVal == P_UNDETERMINED && val == 0 && support == 2) ||
        (lastVal == 2 && val == 0 && support == 2) ||
        (lastVal == 0 && val == 3 && support == 3) || 
        (lastVal == 0 && val == 3 && support == 1) || 
        (lastVal == 3 && val == 0 && support == 2)) {
        
        volumeDown();
      }

      lastVal = val;
    }
  }
}

void setVol(int value) {

  if(isErrorActive()) {
    return;
  }

  if (value >= 0 && value < MAX_VOLUME) {
  
    int bit = value % 8;
    int expanderIndex = value / 8;

    pcf8574_write(expanderIndex, bit, false);

    delay(TIME_VOLUME_MBB);

    for (int i = 0; i < PCF8574_AMOUNT; i++) {
      for (int j = 0; j < 8; j++) {
        if(i == PCF_INPUTS && j > INPUT_SELECTOR_START_BIT - 1) {
          break;
        }
        if(j != bit) {
          pcf8574_write(i, j, true);
        }
      }
    }

  }
}

int readInputsKeyboardState(void) {
  if(isPowerON()) {
    for(int a = 0; a < (int)sizeof(inputsTab); a++) {
      if(!digitalRead(inputsTab[a])) {
        while(!digitalRead(inputsTab[a])) {
          timerTick();
        }
        return a;
      }
    }
  }
  return -1;
}

int translateRC5ToInputState(int command) {
  int selectedInput = -1;

  if(command >= RC5_1 && command <= RC5_6) {
    selectedInput = command - RC5_1;
  }

  return selectedInput;
}

int limitInputs(int input) {
  if(!digitalRead(PIN_INPUTS_AMOUNT)) {
    int a = LIMITED_INPUTS;
    if(a > 8 - INPUT_SELECTOR_START_BIT) {
      a = 8 - INPUT_SELECTOR_START_BIT;
    }
    if(input > a) {
      input = -1;
    }
  }
  return input;
}

void selectInput(int input) {
  for(int a = 0; a < PCF8574_AMOUNT; a++) {
    bool val = true;
    if(input == a) {
      val = false;
    }
    pcf8574_write(PCF_INPUTS, a + INPUT_SELECTOR_START_BIT, val);
  }
}

void storeValuesToEEPROM(void) {
#ifdef EEPROM_SUPPORTED
  for(int a = 0; a < MAX_VALUES; a++) {
    writeAT24(a, values[a]);
  }
#endif
}

void restoreValuesFromEEPROM(void) {
#ifdef EEPROM_SUPPORTED
  for(int a = 0; a < MAX_VALUES; a++) {
    values[a] = readAT24(a);
  }

  if(values[V_VOLUME] > MAX_VOLUME) {
    values[V_VOLUME] = 0;
  }
  storeControlLocalVolume();

  if(values[V_MUTE] > 1) {
    values[V_MUTE] = false;
  }
  mute(values[V_MUTE]);

  int input = values[V_SELECTED_INPUT];
  if(input > getAmountOfHardwareInputs() - 1) {
    input = 0;
  }
  selectInput(input);
#endif
}

bool setupErrorDetection(void) {
  detachInterrupt(PIN_ERROR);
  pinMode(PIN_ERROR, INPUT_PULLUP);
  attachInterrupt(PIN_ERROR, errorInt, FALLING);
  if(digitalRead(PIN_ERROR) == LOW) {
    errorInt();
    return true;
  }
  return false;
}

void errorLoopHandle(void) {
  if(isErrorActive()) {
    if(digitalRead(PIN_ERROR) == HIGH) {
      clearError();
      redrawScreen();
    }
  }
}

void errorInt(void) {
  detachInterrupt(PIN_ERROR);

  unsigned long endTime = millis() + ERROR_TIME; 
  while (digitalRead(PIN_ERROR) == LOW) { 
    if (millis() >= endTime) { 
      speakers(false);
      softPower(false);
      drawError();
      break;
    }
  }

  attachInterrupt(digitalPinToInterrupt(PIN_ERROR), errorInt, FALLING); 
}

