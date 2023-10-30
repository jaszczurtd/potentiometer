#ifndef T_PERIPHERIALS
#define T_PERIPHERIALS

#include "config.h"

#include <Arduino.h>
#include "hardwareConfig.h"
#include "start.h"

extern unsigned char values[];

#define MAX_VOLUME 64

#define V_VOLUME 0
#define V_MUTE 1

void initPeripherials(void);
void power(bool state);
bool isPowerON(void);
bool isPowerPressed(void);
void mute(bool state);
bool isMuteON(void);
bool isMutePressed(void);

#endif

