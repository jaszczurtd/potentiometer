#ifndef T_TOOLS
#define T_TOOLS

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <arduino-timer.h>

#include "hardwareConfig.h"
#include "config.h"
#include "start.h"

#ifndef NOINIT
#define NOINIT __attribute__((section(".noinit"))) 
#endif

void deb(const char *format, ...);
void derr(const char *format, ...);
void initBasicPIO(void);
void initI2C(void);
void initSPI(void);
void createTimerObject(void);
void timerTick(void);
void setupTimerWith(unsigned long ut, unsigned long time, bool(*function)(void *argument));
bool isWireBusy(unsigned int dataAddress);
void pcf8574_init(void);
void pcf8574_write(int pcf_addr, unsigned char pin, bool value);
bool pcf8574_read(int pcf_addr, unsigned char pin);
void writeAT24(unsigned int dataAddress, byte dataVal);
byte readAT24(unsigned int dataAddress);
void writeAT24Int(unsigned int dataAddress, int dataVal);
int readAT24Int(unsigned int dataAddress);
#ifdef I2C_SCANNER
void i2cScanner(void);
#endif

#endif
