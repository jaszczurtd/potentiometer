
#include "peripherials.h"

void encoder(void);
int encoderRead(void);

int val = 0;
void initPeripherials(void) {
  pinMode(PIN_POWER_IN, INPUT_PULLUP);
  pinMode(PIN_POWER_OUT, OUTPUT);

  pinMode(PIN_MUTE_IN, INPUT_PULLUP);
  pinMode(PIN_MUTE_OUT, OUTPUT);

  pinMode(PIN_ENC_L, INPUT_PULLUP);
  pinMode(PIN_ENC_R, INPUT_PULLUP);
  attachInterrupt(PIN_ENC_L, encoder, CHANGE);
  attachInterrupt(PIN_ENC_R, encoder, CHANGE);
  val = encoderRead();

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

int encoderRead(void) {
  int v = 0;

  if(!digitalRead(PIN_ENC_L)) v |= (1 << 1);
  if(!digitalRead(PIN_ENC_R)) v |= (1 << 2);

  return v;
}

void encoder(void) {

  int valt = encoderRead();

  values[V_VOLUME] = valt;

//  if(valt != val) {

//    if((val == 3 && valt == 1) || (val == 0 && valt == 2)) values[V_VOLUME]--;
//    else
//    if((val == 2 && valt == 0) || (val == 1 && valt == 3)) values[V_VOLUME]++;

//   val = valt;

    redraw();
//  }

}