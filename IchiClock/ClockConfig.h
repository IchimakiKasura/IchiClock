#pragma once

/* ============ TEXT SETTINGS ============ */
#define HEADER_TEXT "ICHI'S CLOCK"
// Boot title is hardcoded no.

#define TITLE_Y   55
#define HEADER_Y  18
#define CLOCK_Y   50
#define AMPM_Y    80
#define DATE_Y    105
#define BOTTOM_Y  140

#define TITLE_SIZE   2
#define HEADER_SIZE  1
#define CLOCK_SIZE   3
#define AMPM_SIZE    2
#define DATE_SIZE    1
#define BOTTOM_SIZE  1

/* ============ none ============ */
#define STARTUP_JINGLE 0
#define FNAF_JINGLE 1
#define WESTMINISTER_CHIME_JINGLE 2
#define NIGHT_JINGLE 3
#define SAMSUNG 4
  
/* ============ VARIABLES ============ */
const char* const monthNames[] PROGMEM = {
  "JAN","FEB","MAR","APR","MAY","JUN",
  "JUL","AUG","SEP","OCT","NOV","DEC"
};
const char* const daysFull[] PROGMEM = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
const char* const bottomMessages[] PROGMEM = {
  "Compiling reality",
  "Debugging time...",
  "System stable(?)",
  "Running on coffee",
  "Trust the clock",
  "Code never sleeps",
  "Awaiting interrupt",
  "Logic > emotions",
  "Firmware mode",
  "Still not working",
  "Try rebooting :)",
  "One bug at a time",
  "Delay ruins dreams",
  "Probably wiring",
  "Measure twice",
  "Stay calibrated",
  "Sensors watching",
  "Voltage is sacred",
  "Ground is truth",
  "Magic smoke = bad",
  "Nanoseconds pass",
  "Looping forever",
  "Hello, universe",
  "Stack overflow :)",
  "No semicolon??",
  "Undefined life",
  "Clock drift again",
  "Resistor missing",
  "Did you save?",
  "Cache invalidated",
  "Reflash maybe?",
  "RTC never lies",
  "Bits are honest",
  "Signal detected",
  "Silence = error",
  "Try another pin",
  "Timing is tricky",
  "Read the datasheet",
  "Floating inputs!",
  "Check your GND",
  "Baud mismatch?",
  "Calibrating luck",
  "Works on my PC",
  "Hardware issue :)",
  "Still alive!"
};
const uint8_t totalMessages = 44;

enum Field { FIELD_HOUR, FIELD_MIN, FIELD_AMPM, FIELD_MONTH, FIELD_DAY, FIELD_YEAR };
const uint8_t NUM_FIELDS = 6;
Field selected = FIELD_HOUR;

bool editMode = false,
     adjustHeld = false;

DateTime now, lastTime;
uint8_t bottomIndex = 0;
static uint8_t lastBottomIndex = -1;

unsigned long lastBottomUpdate = 0,
              borderLastUpdate = 0,
              lastUpdate = 0;