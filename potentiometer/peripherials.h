#ifndef T_PERIPHERIALS
#define T_PERIPHERIALS

#include "config.h"

#include <Arduino.h>
#include "hardwareConfig.h"

void initPeripherials(void);
void power(bool state);
bool isPowerON(void);
bool isPowerPressed(void);

#endif

