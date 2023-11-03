
#include "RC5.h"

#define MIN_SHORT  444
#define MAX_SHORT 1333
#define MIN_LONG  1334
#define MAX_LONG  2222

/* 
 * These step by two because it makes it
 * possible to use the values as bit-shift counters
 * when making state-machine transitions.  States
 * are encoded as 2 bits, so we step by 2.
 */
#define EVENT_SHORTSPACE  0
#define EVENT_SHORTPULSE  2
#define EVENT_LONGSPACE   4
#define EVENT_LONGPULSE   6
 
#define STATE_START1 0
#define STATE_MID1   1
#define STATE_MID0   2
#define STATE_START0 3

/*
 * definitions for parsing the bitstream into 
 * discrete parts.  14 bits are parsed as:
 * [S1][S2][T][A A A A A][C C C C C C]
 * Bits are transmitted MSbit first.
 */
#define S2_MASK       0x1000  // 1 bit
#define S2_SHIFT      12
#define TOGGLE_MASK   0x0800  // 1 bit 
#define TOGGLE_SHIFT  11
#define ADDRESS_MASK  0x7C0  //  5 bits
#define ADDRESS_SHIFT 6
#define COMMAND_MASK  0x003F //  low 6 bits
#define COMMAND_SHIFT 0


/* trans[] is a table of transitions, indexed by
 * the current state.  Each byte in the table 
 * represents a set of 4 possible next states,
 * packed as 4 x 2-bit values: 8 bits DDCCBBAA,
 * where AA are the low two bits, and 
 *   AA = short space transition
 *   BB = short pulse transition
 *   CC = long space transition
 *   DD = long pulse transition
 *
 * If a transition does not change the state, 
 * an error has occured and the state machine should
 * reset.
 *
 * The transition table is:
 * 00 00 00 01  from state 0: short space->1
 * 10 01 00 01  from state 1: short pulse->0, long pulse->2
 * 10 01 10 11  from state 2: short space->3, long space->1
 * 11 11 10 11  from state 3: short pulse->2
 */
const unsigned char trans[] = {0x01,
                               0x91,  
                               0x9B,  
                               0xFB};

static RC5 *rc5 = NULL;
RC5 *initRC5(int pin) {
  rc5 = new RC5(pin);
  return rc5;
}

static void callback(void) {
  unsigned char toggle, address, command;
  if(rc5->read(&toggle, &address, &command)) {
    if(rc5->targetCallback != NULL) {
      rc5->targetCallback(toggle, address, command);
    }
  }
}

RC5::RC5(unsigned char p) {
  pin = p;
  targetCallback = NULL;
  pinMode(pin, INPUT_PULLUP);
  attachInterrupt(pin, callback, CHANGE);
  reset();
}

void RC5::setCallback(void(*function)(unsigned char toggle, unsigned char address, unsigned char command)) {
  targetCallback = function;
}

void RC5::reset() {
  state = STATE_MID1;
  bits = 1;  // emit a 1 at start - see state machine graph
  command = 1;
  time0 = micros();
}

void RC5::decodePulse(unsigned char signal, unsigned long period) {
  if (period >= MIN_SHORT && period <= MAX_SHORT) {
    decodeEvent(signal ? EVENT_SHORTPULSE : EVENT_SHORTSPACE);
  } else if (period >= MIN_LONG && period <= MAX_LONG) {
    decodeEvent(signal ? EVENT_LONGPULSE : EVENT_LONGSPACE);
  } else {
    // time period out of range, reset
    reset();
  }
}

void RC5::decodeEvent(unsigned char event) {
  // find next state, 2 bits
  unsigned char newState = (trans[state]>>event) & 0x3;
  if (newState==state) {
    // no state change indicates error, reset
    reset();
  } else {
    state = newState;
    if (newState == STATE_MID0) {
      // always emit 0 when entering mid0 state
      command = (command<<1)+0;
      bits++;
    } else if (newState == STATE_MID1) {
      // always emit 1 when entering mid1 state
      command = (command<<1)+1;
      bits++;
    }
  }
}

bool RC5::read(unsigned int *message) {
  /* Note that the input value read is inverted from the theoretical signal,
      ie we get 1 while no signal present, pulled to 0 when a signal is detected.
      So when the value changes, the inverted value that we get from reading the pin
      is equal to the theoretical (uninverted) signal value of the time period that
      has just ended.
  */
  int value = digitalRead(pin);

  if (value != lastValue) {
    unsigned long time1 = micros();
    unsigned long elapsed = time1-time0;
    time0 = time1;
    lastValue = value;
    decodePulse(value, elapsed);
  }
  
  if (bits == 14) {
    *message = command;
    command = 0;
    bits = 0;
    return true;
  } else {
    return false;
  }
}

bool RC5::read(unsigned char *toggle, unsigned char *address, unsigned char *command) {
  unsigned int message;
  if (read(&message)) {
    *toggle  = (message & TOGGLE_MASK ) >> TOGGLE_SHIFT;
    *address = (message & ADDRESS_MASK) >> ADDRESS_SHIFT;
      
    // Support for extended RC5:
    // to get extended command, invert S2 and shift into command's 7th bit
    unsigned char extended = (~message & S2_MASK) >> (S2_SHIFT - 6);
    *command = ((message & COMMAND_MASK) >> COMMAND_SHIFT) | extended;
    
    return true;
  } else {
    return false;
  }
}
