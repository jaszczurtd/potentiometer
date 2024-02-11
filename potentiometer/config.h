#ifndef T_CONFIG
#define T_CONFIG

//#define I2C_SCANNER

//those defined only for debug
#define SKIP_INTRO
#define SKIP_OFF
#define SKIP_OFF_SECONDS_TO_START 2

#define SECOND 1000

//general system constants
#define WATCHDOG_TIME (4 * SECOND)

//miloseconds: time to show error 
#define ERROR_TIME 800

//in seconds
#define TIME_DELAY_SPEAKERS 3
#define TIME_DELAY_SOFT_POWER 10
#define TIME_INTRO_TIME_KERNEL_ACOUSTIC 2
#define TIME_LOAD 30
#define MUTE_TIME_DISPLAY 2

#define LOADING_STEPS 42
//loading bar steps optimizaion
#define AMOUNT_BARS_UNTIL_LOGO 15 

#define TIME_DELAY_VOLUME_RC5_CONTROL 200

#endif

