
#include "start.h"

void setupTimers(void) {
  int time = SECOND;
  createTimerObject();
  setupTimerWith(UNSYNCHRONIZE_TIME, time, callAtEverySecond);
}

void initialization(void) {

  Serial.begin(9600);
 
  TFT *tft = initTFT();

  //adafruit LCD driver is messing up something with i2c on rpi pin 0 & 1
  //this has to be invoked as soon as possible, and twice
  initI2C();
  pcf8574_init();
  Wire.end();

  initSPI();

  initI2C();

  #ifdef RESET_EEPROM
  resetEEPROM();
  #endif

  initBasicPIO();

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

  delay(CORE_OPERATION_DELAY);  
}

void initialization1(void) {
  deb("Second core initialized");
}

void looper1(void) {

  delay(CORE_OPERATION_DELAY);  
}

