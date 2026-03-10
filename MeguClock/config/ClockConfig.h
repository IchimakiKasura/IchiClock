#pragma once

// #define CUSTOM_PINS
// for TFT pinouts, it accepts registers only specifically B type (pins 8-13)
// because 10,9 are the mosi and clk usually (SPI pins) for arduino uno/nano
// #define TFT_CS   2 //10
// #define TFT_RST  0 //8
// #define TFT_DC   1 //9
// #define BTN_SELECT 2
// #define BTN_ADJUST 3
// #define BUZZER     5

// I only did the pins to save small bytes, rest are uhh hard

/* ============ TEXT SETTINGS ============ */
#define HEADER_TEXT "MEGUMIN CLOCK"
// Boot title is hardcoded no.

#define TITLE_SIZE   2
#define HEADER_SIZE  1
#define CLOCK_SIZE   3
#define AMPM_SIZE    2
#define DATE_SIZE    1
#define BOTTOM_SIZE  1

/* ============ idk better readability ============ */
#define CHIISANA_BOKENSHA_JINGLE 0
#define FNAF_JINGLE 1
#define WESTMINISTER_CHIME_JINGLE 2
#define NIGHT_JINGLE 3
#define SAMSUNG 4