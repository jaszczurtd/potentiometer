#ifndef T_PERIPHERIALS
#define T_PERIPHERIALS

#include "config.h"

#include <Arduino.h>
#include "hardwareConfig.h"
#include "start.h"
#include "RC5.h"

extern unsigned char values[];

#define ANALOG_WRITE_RESOLUTION 8

#define UNSYNCHRONIZE_TIME 15
#define CORE_OPERATION_DELAY 1

#define MAX_VOLUME 42
#define INPUT_SELECTOR_START_BIT 2 

#define P_UNDETERMINED -1
#define MUTE_FOR_CHANGE_VOLUME 200 //ms
#define ENC_DEBOUNCE 4 //us

#define V_VOLUME 0
#define V_MUTE 1
#define V_SELECTED_INPUT 2
#define MAX_VALUES 3

int getAmountOfHardwareInputs(void);
void initMainPIO(void);
void initPeripherials(void);
void lcdBrightness(int val);
void power(bool state);
bool isPowerON(void);
bool isPowerPressed(void);
bool isAdditionalPowerPresed(void);
void mute(bool state);
void volumeUp(void);
void volumeDown(void);
void speakers(bool state);
void softPower(bool state);
bool isMuteON(void);
void setVol(int value);
int readInputsKeyboardState(void);
int translateRC5ToInputState(int command);
int limitInputs(int input);
void selectInput(int input);
void storeValuesToEEPROM(void);
void restoreValuesFromEEPROM(void);
bool setupErrorDetection(void);
void errorLoopHandle(void);

#endif

