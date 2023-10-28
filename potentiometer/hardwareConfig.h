#ifndef T_HARDWARECONFIG
#define T_HARDWARECONFIG

#define PWM_FREQUENCY_HZ 300
#define I2C_SPEED_HZ 50000

#define PWM_WRITE_RESOLUTION 11
#define PWM_RESOLUTION 2047

#define PIN_SDA 0
#define PIN_SCL 1

#define PIN_MISO 16
#define PIN_MOSI 19
#define PIN_SCK 18

//PCF8574 i2c addr
#define PCF8574_AMOUNT 6
#define PCF8574_ADDR_1 0x38
#define PCF8574_ADDR_2 0x39
#define PCF8574_ADDR_3 0x3b
#define PCF8574_ADDR_4 0x3c
#define PCF8574_ADDR_5 0x3e
#define PCF8574_ADDR_6 0x3f

#define EEPROM_I2C_ADDRESS 0x50

//LCD / display
#define TFT_CS     17 //CS
#define TFT_RST    -1 //reset
#define TFT_DC     15 //A0

//peripherials

#define PIN_POWER_IN 21
#define PIN_POWER_OUT 2


#endif
