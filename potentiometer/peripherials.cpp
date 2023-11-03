
#include "peripherials.h"

void encoder(void);
void encoderSupport(void);
void encInit(void);

unsigned char inputsTab[] = {
  PIN_IMPUT_6, PIN_IMPUT_5, PIN_IMPUT_4, PIN_IMPUT_3, PIN_IMPUT_1, PIN_IMPUT_2
};

void initPeripherials(void) {
  pinMode(PIN_POWER_IN, INPUT_PULLUP);
  pinMode(PIN_POWER_OUT, OUTPUT);

  pinMode(PIN_MUTE_IN, INPUT_PULLUP);
  pinMode(PIN_MUTE_OUT, OUTPUT);

  for(int a = 0; a < sizeof(inputsTab); a++) {
    pinMode(inputsTab[a], INPUT_PULLUP);
  }

  encInit();
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

void mute(bool state) {
  digitalWrite(PIN_MUTE_OUT, state);
}

bool isMuteON(void) {
  return digitalRead(PIN_MUTE_OUT);
}

bool isMutePressed(void) {
  return !digitalRead(PIN_MUTE_IN);
}

static volatile int lastVal = P_UNDETERMINED;
static volatile int support = P_UNDETERMINED;
static unsigned long previousMillis = 0; 
static unsigned long currentMillis;

void encInit(void) {
  pinMode(PIN_ENC_L, INPUT_PULLUP);
  pinMode(PIN_ENC_R, INPUT_PULLUP);
  attachInterrupt(PIN_ENC_R, encoder, CHANGE);
  attachInterrupt(PIN_ENC_L, encoderSupport, CHANGE);
  currentMillis = millis();
}

void encoderSupport(void) {
  if(isPowerON()) {
    support = digitalRead(PIN_ENC_L) | digitalRead(PIN_ENC_R) << 1;
  }
}

void encoder(void) {
  if(isPowerON()) {
    int val = digitalRead(PIN_ENC_L) | digitalRead(PIN_ENC_R) << 1;
    if(val != lastVal) {
      int volume = values[V_VOLUME];
      int movement = P_UNDETERMINED;

      if(!values[V_MUTE]) {
        currentMillis = millis();
      }

      if((lastVal == P_UNDETERMINED && val == 2 && support == P_UNDETERMINED) || 
        (lastVal == P_UNDETERMINED && val == 0 && support == 0) ||
        (lastVal == 2 && val == 0 && support == 0) || 
        (lastVal == 0 && val == 2 && support == 0) ||
        (lastVal == 3 && val == 0 && support == 0) ||
        (lastVal == 2 && val == 0 && support == 3) ||
        (lastVal == 0 && val == 2 && support == 2)) {

        if(volume < MAX_VOLUME) {
          if(!values[V_MUTE]) {
            mute(true);
          }
          volume++;
        }
      } else

      if((lastVal == P_UNDETERMINED && val == 3 && support == P_UNDETERMINED) ||
        (lastVal == P_UNDETERMINED && val == 3 && support == 3) ||
        (lastVal == P_UNDETERMINED && val == 0 && support == 2) ||
        (lastVal == 2 && val == 0 && support == 2) ||
        (lastVal == 0 && val == 3 && support == 3) || 
        (lastVal == 0 && val == 3 && support == 1) || 
        (lastVal == 3 && val == 0 && support == 2)) {
        
        if(volume > 0) {
          if(!values[V_MUTE]) {
            mute(true);
          }
          volume--;
        }      
      }

      deb("%d %d %d", lastVal, val, support);

      values[V_VOLUME] = volume;
      redraw();

      lastVal = val;
    }
  }
}

void muteWithEncoderSupport(void) {
  if(isPowerON()) {
    if (currentMillis - previousMillis >= MUTE_FOR_CHANGE_VOLUME) {
      mute(false);
    }
  }
}

int checkAndApplyInputs(void) {
  if(isPowerON()) {
    for(int a = 0; a < sizeof(inputsTab); a++) {
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

void selectInput(int input) {
  for(int a = 0; a < PCF8574_AMOUNT; a++) {
    bool val = true;
    if(input == a) {
      val = false;
    }
    pcf8574_write(getPCF8574at(PCF_INPUTS), a + 2, val);
  }
}

void storeValuesToEEPROM(void) {
  for(int a = 0; a < MAX_VALUES; a++) {
    writeAT24(a, values[a]);
  }
}

void restoreValuesFromEEPROM(void) {
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
  if(input > sizeof(inputsTab) - 1) {
    input = 0;
  }
  selectInput(input);
}