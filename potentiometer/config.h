#ifndef T_CONFIG
#define T_CONFIG

//#define I2C_SCANNER

//those defined only for debug
//#define SKIP_INTRO
//#define SKIP_OFF
//#define SKIP_OFF_SECONDS_TO_START 2
//#define EEPROM_SUPPORTED 

#define SECOND 1000

//general system constants
#define WATCHDOG_TIME (4 * SECOND)

//miloseconds: time to show error 
#define ERROR_TIME 800

//in seconds
#define TIME_DELAY_SPEAKERS 3
#define TIME_DELAY_SOFT_POWER 10
#define TIME_INTRO_TIME_KERNEL_ACOUSTIC 2
#define TIME_LOAD 35
#define MUTE_TIME_DISPLAY 2
#define SCREEN_DIMMER 8
#define TIME_ENCODER_PRESS_TO_OFF 3
#define TIME_MUTE_BBM 2
#define TIME_VOLUME_MBB 30

#define LOADING_STEPS 35

#define TIME_DELAY_VOLUME_RC5_CONTROL 200

//0 to 5
#define LIMITED_INPUTS 3

//0 to 255
#define BRIGHTNESS_MAX 255
#define BRIGHTNESS_AMBIENT 26

//ms
#define BRIGHTNESS_FADER_STEP_TIME 15
#endif

