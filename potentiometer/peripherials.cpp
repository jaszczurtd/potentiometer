
#include "peripherials.h"

void encoder(void);
void encInit(void);

void initPeripherials(void) {
  pinMode(PIN_POWER_IN, INPUT_PULLUP);
  pinMode(PIN_POWER_OUT, OUTPUT);

  pinMode(PIN_MUTE_IN, INPUT_PULLUP);
  pinMode(PIN_MUTE_OUT, OUTPUT);

  encInit();
}

void power(bool state) {
  digitalWrite(PIN_POWER_OUT, state);
}

bool isPowerON(void) {
  return digitalRead(PIN_POWER_OUT);
}

bool isPowerPressed(void) {
  return !digitalRead(PIN_POWER_IN);
}

void mute(bool state) {
  digitalWrite(PIN_MUTE_OUT, state);
}

bool isMuteON(void) {
  return digitalRead(PIN_MUTE_OUT);
}

bool isMutePressed(void) {
  return !digitalRead(PIN_MUTE_IN);
}

static int lastVal = -1;
void encInit(void) {
  pinMode(PIN_ENC_L, INPUT_PULLUP);
  pinMode(PIN_ENC_R, INPUT_PULLUP);
  attachInterrupt(PIN_ENC_R, encoder, CHANGE);
}

void encoder(void) {
  int val = digitalRead(PIN_ENC_L) | digitalRead(PIN_ENC_R) << 1;
  if(val != lastVal) {
    lastVal = val;

    deb("%d", val);
  }
}
