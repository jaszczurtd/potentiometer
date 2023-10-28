
#include "start.h"

void powerSequence(bool state);

void setupTimers(void) {
  int time = SECOND;
  createTimerObject();
  setupTimerWith(UNSYNCHRONIZE_TIME, time, callAtEverySecond);
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

  tft->fillScreen(COLOR(ORANGE));//ICONS_BG_COLOR);

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
  softInitDisplay();
  return true; 
}

void looper(void) {
  timerTick();

  if(isPowerPressed()) {
    while(isPowerPressed()) {
      timerTick();
      delay(CORE_OPERATION_DELAY);  
    }
    powerSequence(!isPowerON());
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


}