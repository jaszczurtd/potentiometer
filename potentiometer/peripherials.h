#ifndef T_PERIPHERIALS
#define T_PERIPHERIALS

#include "config.h"

#include <Arduino.h>
#include "hardwareConfig.h"
#include "start.h"

extern unsigned char values[];

#define MAX_VOLUME 42

#define P_UNDETERMINED -1
#define MUTE_FOR_CHANGE_VOLUME 200

#define V_VOLUME 0
#define V_MUTE 1
#define V_SELECTED_INPUT 2
#define MAX_VALUES 3

void initPeripherials(void);
void power(bool state);
bool isPowerON(void);
bool isPowerPressed(void);
void mute(bool state);
bool isMuteON(void);
bool isMutePressed(void);
void muteWithEncoderSupport(void);
int checkAndApplyInputs(void);
void selectInput(int input);
void storeValuesToEEPROM(void);
void restoreValuesFromEEPROM(void);

#endif

