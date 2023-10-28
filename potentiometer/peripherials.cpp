
#include "peripherials.h"

void initPeripherials(void) {
  pinMode(PIN_POWER_IN, INPUT_PULLUP);
  pinMode(PIN_POWER_OUT, OUTPUT);
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