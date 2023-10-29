
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
#include "peripherials.h"

#define MAX_VALUES 16

extern unsigned char values[];

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
void redraw(void);

#endif
