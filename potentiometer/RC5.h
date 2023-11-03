/*
 *  RC5 Arduino Library
 *  Guy Carpenter, Clearwater Software - 2013
 *
 *  Licensed under the BSD2 license, see LICENSE for details.
 *
 *  All text above must be included in any redistribution.
 */

#ifndef RC5_h
#define RC5_h

#include <Arduino.h>
#include "tools.h"

#define RC5_POWER 12
#define RC5_VOLUME_UP 16
#define RC5_VOLUME_DOWN 17
#define RC5_MUTE 13
#define RC5_1 1
#define RC5_2 2
#define RC5_3 3
#define RC5_4 4
#define RC5_5 5
#define RC5_6 6

class RC5
{
 public:
    unsigned char pin;
    unsigned char state;
    unsigned long time0;
    unsigned long lastValue;
    unsigned int bits;
    unsigned int command;
    void (*targetCallback)(unsigned char toggle, unsigned char address, unsigned char command);

    RC5(unsigned char pin);
    void setCallback(void(*function)(unsigned char toggle, unsigned char address, unsigned char command));
    void reset();
    bool read(unsigned int *message);
    bool read(unsigned char *toggle, unsigned char *address, unsigned char *command);
    void decodeEvent(unsigned char event);
    void decodePulse(unsigned char signal, unsigned long period);
};

RC5 *initRC5(int pin);

#endif // RC5_h
