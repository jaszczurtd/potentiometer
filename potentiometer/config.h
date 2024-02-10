#ifndef T_CONFIG
#define T_CONFIG

//#define I2C_SCANNER

//those defined only for debug
#define SKIP_INTRO
#define SKIP_OFF
#define SKIP_OFF_SECONDS_TO_START 2

//general system constants
#define WATCHDOG_TIME 4000
#define UNSYNCHRONIZE_TIME 15
#define CORE_OPERATION_DELAY 1

#define SECOND 1000

//in seconds
#define TIME_DELAY_SPEAKERS 3
#define TIME_DELAY_SOFT_POWER 10
#define TIME_INTRO_TIME_KERNEL_ACOUSTIC 2
#define TIME_LOADING_TIME_KERNEL_ACOUSTIC 0.25
#define TIME_LOAD 30
#define MUTE_TIME_DISPLAY 2

#define LOADING_STEPS 42
#define AMOUNT_BARS_UNTIL_LOGO 15 //loading bar steps
#define TIME_DELAY_VOLUME_RC5_CONTROL 200

//in ms
#define RC5_PROBE_TIME 150

//delay time right after start, before first serious alerts will show up (in seconds)
#define SERIOUS_ALERTS_DELAY_TIME 1

#endif

