#pragma once

/* ============ TEXT SETTINGS ============ */
#define HEADER_TEXT "MEGUMIN CLOCK"
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
#define CHIISANA_BOKENSHA_JINGLE 0
#define FNAF_JINGLE 1
#define WESTMINISTER_CHIME_JINGLE 2
#define NIGHT_JINGLE 3
#define SAMSUNG 4
  
/* ============ VARIABLES ============ */
const char monthNames[12][5] PROGMEM = {
  "JAN ","FEB ","MAR ","APR ","MAY ","JUN ",
  "JUL ","AUG ","SEP ","OCT ","NOV ","DEC "
};

const char daysFull[7][9] PROGMEM = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

const char bottomMessages[][20] PROGMEM = {
  "Compiling reality",
  "Explosion magic!",  
  "Debugging time...",
  "System stable(?)",
  "Darkness, lend me", 
  "Running on coffee",
  "Trust the clock",
  "Awaiting interrupt",
  "Logic > emotions",
  "Prepare to blast!", 
  "Firmware mode",
  "Still not working",
  "One bug at a time",
  "I cast my spell!",  
  "Delay ruins dreams",
  "Measure twice",
  "Stay calibrated",
  "One shot, one kill",
  "Voltage is sacred",
  "Nanoseconds pass",
  "Nothing but boom!", 
  "Looping forever",
  "Stack overflow",
  "No semicolon??",
  "Magic is supreme",  
  "Undefined life",
  "Clock drift again",
  "Resistor missing",
  "Cache invalidated",
  "Reflash maybe?",
  "Chaos reigns now",  
  "RTC never lies",
  "Bits are honest",
  "Signal detected",
  "Silence = error",
  "Try another pin",
  "Final explosion!",  
  "Timing is tricky",
  "Read the datasheet",
  "Floating inputs!",
  "Check your GND",
  "Witness my power",  
  "Baud mismatch?",
  "Calibrating luck",
  "Works on my PC",
  "Hardware issue",
  "Final explosion!"
};

enum Field { FIELD_HOUR, FIELD_MIN, FIELD_AMPM, FIELD_MONTH, FIELD_DAY, FIELD_YEAR };
Field selected = FIELD_HOUR;

bool editMode = false,
     adjustHeld = false,
     h_edited = false,
     m_edited = false;

DateTime now, lastTime;
uint8_t bottomIndex = 0;
static uint8_t lastBottomIndex = -1;

unsigned long lastBottomUpdate = 0,
              borderLastUpdate = 0,
              lastUpdate = 0,
              lastCheck = 0;

const char bottomTextBuffer[20]; // for fucken bottom function on seperate 2 methods