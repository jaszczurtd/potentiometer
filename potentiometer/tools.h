#ifndef T_TOOLS
#define T_TOOLS

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <arduino-timer.h>

#include "hardwareConfig.h"
#include "config.h"
#include "start.h"

#define MAX_DEB_BUFFER 128

#define INIT_PCF_STATE 0xff

#ifndef NOINIT
#define NOINIT __attribute__((section(".noinit"))) 
#endif

extern unsigned char values[];

void deb(const char *format, ...);
void derr(const char *format, ...);
void initBasicPIO(void);
void initI2C(void);
void initSPI(void);
void createTimerObject(void);
void timerTick(void);
void setupTimerWith(unsigned long ut, unsigned long time, bool(*function)(void *argument));
void launchTaskAt(unsigned long time, bool(*function)(void *argument));
void cancelTimerTasks(void);
bool isWireBusy(unsigned int dataAddress);
void pcf8574_init(void);
int getPCF8574at(int addr);
void pcf8574_write(int pcf_addr, unsigned char pin, bool value);
unsigned char pcf8574_read(int pcf_addr);
void writeAT24(unsigned int dataAddress, byte dataVal);
byte readAT24(unsigned int dataAddress);
void writeAT24Int(unsigned int dataAddress, int dataVal);
int readAT24Int(unsigned int dataAddress);
#ifdef I2C_SCANNER
void i2cScanner(void);
#endif

#endif
