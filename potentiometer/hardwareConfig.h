#ifndef T_HARDWARECONFIG
#define T_HARDWARECONFIG

#include "config.h"

#define I2C_SPEED_HZ 150000

#define PIN_SDA 0
#define PIN_SCL 1

#define PIN_MISO 16
#define PIN_MOSI 19
#define PIN_SCK 18

//6 means indexes from 0 to 5
#define PCF8574_AMOUNT 6

//PCF8574 i2c addr
#define PCF8574_ADDR_1 0x38
#define PCF8574_ADDR_2 0x3c
#define PCF8574_ADDR_3 0x3e
#define PCF8574_ADDR_4 0x3f
#define PCF8574_ADDR_5 0x39
#define PCF8574_ADDR_6 0x3b

//whichone PCF8574 chip (index) is used as input selector?
#define PCF_INPUTS 5

#ifdef EEPROM_SUPPORTED
#define EEPROM_I2C_ADDRESS 0x50
#endif

//raspberry pi pico PIO assignemnts

//LCD / display
#define TFT_CS     17 //CS
#define TFT_RST    -1 //reset (PIO is not used here)
#define TFT_DC     15 //DC

#define PIN_POWER_IN 21
#define PIN_POWER_OUT 2
#define PIN_ADD_POWER_IN 6
#define PIN_MUTE_OUT 14

#define PIN_SPEAKERS 3
#define PIN_SOFTPOWER 28

#define PIN_ENC_L 4
#define PIN_ENC_R 5

#define PIN_INPUT_1 7
#define PIN_INPUT_2 9
#define PIN_INPUT_3 10
#define PIN_INPUT_4 11
#define PIN_INPUT_5 12
#define PIN_INPUT_6 13

#define PIN_RC5 20

#define PIN_ERROR 8

#define PIN_INPUTS_AMOUNT 26
#define PIN_LCD_BRIGHTNESS 22

#endif
