
#include "tools.h"

static Timer generalTimer;

NOINIT static char deb_buffer[128];
void deb(const char *format, ...) {

  rp2040.idleOtherCore();

  va_list valist;
  va_start(valist, format);

  memset (deb_buffer, 0, sizeof(deb_buffer));
  vsnprintf(deb_buffer, sizeof(deb_buffer) - 1, format, valist);
  Serial.println(deb_buffer);

  va_end(valist);

  rp2040.resumeOtherCore();
}

NOINIT static char derr_buffer[128];
void derr(const char *format, ...) {

  rp2040.idleOtherCore();

  va_list valist;
  va_start(valist, format);

  memset (derr_buffer, 0, sizeof(derr_buffer));

  const char *error = "ERROR! ";
  int len = strlen(error);

  strcpy(derr_buffer, error);

  vsnprintf(derr_buffer + len, sizeof(derr_buffer) - 1 - len, format, valist);
  Serial.println(derr_buffer);

  va_end(valist);

  rp2040.resumeOtherCore();
}

void initBasicPIO(void) {
  pinMode(LED_BUILTIN, OUTPUT);
}

void initI2C(void) {
  Wire.setSDA(PIN_SDA);
  Wire.setSCL(PIN_SCL);
  Wire.setClock(I2C_SPEED_HZ);
  Wire.begin();
}

void initSPI(void) {
  SPI.setRX(PIN_MISO); //MISO
  SPI.setTX(PIN_MOSI); //MOSI
  SPI.setSCK(PIN_SCK); //SCK
}

void createTimerObject(void) {
  generalTimer = timer_create_default();
}

void timerTick(void) {
  generalTimer.tick();
}

void setupTimerWith(unsigned long ut, unsigned long time, bool(*function)(void *argument)) {
  watchdog_update();
  generalTimer.every(time, function);
  delay(ut);
  watchdog_update();
}

static unsigned char pcf8574State = 0;
static int pcf_addrs[] = {
  PCF8574_ADDR_1, PCF8574_ADDR_2, PCF8574_ADDR_3, PCF8574_ADDR_4, PCF8574_ADDR_5, PCF8574_ADDR_6
};

void pcf8574_init(void) {
  pcf8574State = 0xff;

  for(int a = 0; a < PCF8574_AMOUNT; a++) {
    Wire.beginTransmission(pcf_addrs[a]);
    Wire.write(pcf8574State);
    Wire.endTransmission();
  }
}

void pcf8574_write(int pcf_addr, unsigned char pin, bool value) {
  if(value) {
    bitSet(pcf8574State, pin);
  }  else {
    bitClear(pcf8574State, pin);
  }

  Wire.beginTransmission(pcf_addr);
  bool success = Wire.write(pcf8574State);
  bool notFound = Wire.endTransmission();

  if(!success) {
    derr("error writting byte to pcf8574");
  }

  if(notFound) {
    derr("pcf8574 not found");
  }
}

bool isWireBusy(unsigned int dataAddress) {
  Wire.beginTransmission(dataAddress);
  return Wire.endTransmission();
}

bool pcf8574_read(int pcf_addr, unsigned char pin) {
  Wire.beginTransmission(pcf_addr);
  bool retVal = Wire.read();
  bool notFound = Wire.endTransmission();

  if(notFound) {
    derr("pcf8574 not found");
  }

  return retVal;
}

void writeAT24(unsigned int dataAddress, byte dataVal) {
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write(dataAddress >> 8);
  Wire.write(dataAddress & 0xff);
  Wire.write(dataVal);
  Wire.endTransmission();
  while(isWireBusy(EEPROM_I2C_ADDRESS)){   
    delayMicroseconds(100);
    watchdog_update();
  }
  delay(5);
}

// Function to read from EEPROM
byte readAT24(unsigned int dataAddress) {
  byte readData = 0;
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write(dataAddress >> 8);
  Wire.write(dataAddress & 0xff);
  Wire.endTransmission();
  Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);
  if (Wire.available()) {
    readData = Wire.read();
  }
  return readData;
}

void writeAT24Int(unsigned int dataAddress, int dataVal) {
  writeAT24(dataAddress + 0, (dataVal >> 24) & 0xFF);
  writeAT24(dataAddress + 1, (dataVal >> 16) & 0xFF);
  writeAT24(dataAddress + 2, (dataVal >> 8) & 0xFF);
  writeAT24(dataAddress + 3, (dataVal & 0xFF));
}

int readAT24Int(unsigned int dataAddress) {
  return  (readAT24(dataAddress + 0) << 24) |
          (readAT24(dataAddress + 1) << 16) |
          (readAT24(dataAddress + 2) << 8) |
          (readAT24(dataAddress + 3));
}

#ifdef I2C_SCANNER
unsigned int loopCounter = 0;
static bool t = false;
void i2cScanner(void) {
  byte error, address;
  int nDevices;
 
  while(true) {
    Serial.print("Scanning ");
    Serial.print(loopCounter++);
    Serial.print("...\n");
  
    nDevices = 0;
    for(address = 1; address < 127; address++ ) {
      watchdog_update();
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
  
      if (error == 0) {
        Serial.print("I2C device found at address 0x");
        if (address < 16)
          Serial.print("0");
        Serial.print(address, HEX);
        Serial.println("  !");
  
        nDevices++;
      }
      else if (error == 4) {
        Serial.print("Unknown error at address 0x");
        if (address < 16)
          Serial.print("0");
        Serial.println(address, HEX);
      }    
    }
    if (nDevices == 0)
      Serial.println("No I2C devices found\n");
    else
      Serial.println("done\n");
  
    delay(500);           // wait 500 mseconds for next scan
  }

}
#endif
