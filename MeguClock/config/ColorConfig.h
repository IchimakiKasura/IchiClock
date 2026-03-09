#pragma once
#include <EEPROM.h>

class M_COLORS {
public:
    inline static byte ClockColor_index = 1;
    inline static byte DateColor_index = 2;

    inline static const uint16_t colors[8] PROGMEM = {
        BLUE,
        RED,
        YELLOW,
        CYAN,
        GREEN,
        WHITE,
        MAGENTA,
        ORANGE
    };

    static uint16_t ClockColor() {
        return pgm_read_word(&colors[ClockColor_index]);
    }

    static uint16_t DateColor() {
        return pgm_read_word(&colors[DateColor_index]);
    }

    static void Save() {
        EEPROM.update(0, ClockColor_index);
        EEPROM.update(1, DateColor_index);
    }

    static void Load() {
        ClockColor_index = EEPROM.read(0);
        DateColor_index  = EEPROM.read(1);
    }
};