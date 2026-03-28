#pragma once
#include <EEPROM.h>

class M_COLORS
{
public:
    static uint8_t ClockColor_index, DateColor_index;

    static const uint16_t colors[8];

    inline static uint16_t ClockColor(), DateColor();
    inline static void Save(), Load();
};

uint8_t M_COLORS::ClockColor_index = 1,
        M_COLORS::DateColor_index = 2;

const uint16_t M_COLORS::colors[8] PROGMEM = {
    BLUE,
    RED,
    YELLOW,
    CYAN,
    GREEN,
    WHITE,
    MAGENTA,
    ORANGE
};

inline static uint16_t M_COLORS::ClockColor()
{
    return pgm_read_word(&colors[ClockColor_index]);
}

inline static uint16_t M_COLORS::DateColor()
{
    return pgm_read_word(&colors[DateColor_index]);
}

inline static void M_COLORS::Save()
{
    EEPROM.update(0, ClockColor_index);
    EEPROM.update(1, DateColor_index);
}

inline static void M_COLORS::Load()
{
    ClockColor_index = EEPROM.read(0);
    DateColor_index = EEPROM.read(1);
}