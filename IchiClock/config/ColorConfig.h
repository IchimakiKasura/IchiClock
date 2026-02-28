#pragma once

// BGR FIX
#define RED    ST77XX_BLUE
#define BLUE   ST77XX_RED
#define CYAN   ST77XX_YELLOW
#define YELLOW ST77XX_CYAN
// shortcut
#define WHITE   ST77XX_WHITE
#define BLACK   ST77XX_BLACK
#define ORANGE  ST77XX_ORANGE
#define MAGENTA ST77XX_MAGENTA
#define GREEN   ST77XX_GREEN

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
            EEPROM.update(0, ClockColor_index & 0xFF);
            EEPROM.update(0 + 1, ClockColor_index >> 8);
            EEPROM.update(2, DateColor_index & 0xFF);
            EEPROM.update(2 + 1, DateColor_index >> 8);
        }

        static void Load() {
            byte c = EEPROM.read(0) | (EEPROM.read(0 + 1) << 8);
            byte d = EEPROM.read(2)  | (EEPROM.read(2 + 1) << 8);
        
            if (c != 0xFFFF) ClockColor_index = c;
            if (d != 0xFFFF) DateColor_index  = d;
        }

};