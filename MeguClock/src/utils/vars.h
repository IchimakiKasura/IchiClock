#pragma once

bool adjustHeld = false,
     h_edited = false,
     m_edited = false,
     lastRTCState = false;

MeguClock_DS3231 rtc;
DateTime lastTime;

uint8_t bottomIndex,
        lastBottomIndex = -1;

uint16_t systemTime,
         lastBottomUpdate,
         borderLastUpdate,
         lastUpdate ,
         lastCheck;

const char bottomTextBuffer[20], bottomMessages[][20] PROGMEM = {
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