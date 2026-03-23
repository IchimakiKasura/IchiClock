#pragma once
#include <RTClib.h>
#include "rand.h"

enum Field { FIELD_HOUR, FIELD_MIN, FIELD_AMPM, FIELD_MONTH, FIELD_DAY, FIELD_YEAR };
Field selected = FIELD_HOUR;

bool editMode = false,
     adjustHeld = false,
     h_edited = false,
     m_edited = false,
     lastRTCState = false;

DateTime g_now, lastTime;
uint8_t bottomIndex = 0;
static uint8_t lastBottomIndex = -1;

uint32_t systemTime,
         lastBottomUpdate = 0,
         borderLastUpdate = 0,
         lastUpdate = 0,
         lastCheck = 0;

const char daysFull[][10] PROGMEM = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

const char bottomTextBuffer[20];
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
  "Hardware issue"
};

#include "../config/ClockConfig.h"
#include "../hardware/display.h"
#include "../hardware/buzzer.h"
#include "../hardware/buttons.h"
#include "../features/alarm.h"