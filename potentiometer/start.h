
#ifndef T_START
#define T_START

#include "config.h"

#include <Arduino.h>
#include <Wire.h>
#include <arduino-timer.h>
#include <SPI.h>

#include "tools.h"
#include "TFTExtension.h"
#include "hardwareConfig.h"

#define MEDIUM_TIME_ONE_SECOND_DIVIDER 12
#define FREQUENT_TIME_ONE_SECOND_DIVIDER 16

void drawMediumImportanceValues(void);
void drawHighImportanceValues(void);
void drawLowImportanceValues(void);
void triggerDrawHighImportanceValue(bool state);

bool callAtEverySecond(void *argument);
bool callAtEveryHalfSecond(void *argument);
bool callAtEveryHalfHalfSecond(void *argument);

void initialization(void);
void initialization1(void);
void looper(void);
void looper1(void);
bool seriousAlertSwitch(void);
bool alertSwitch(void);

#endif
