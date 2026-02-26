#pragma once

#define ADDR_CLOCK_COLOR 0
#define ADDR_DATE_COLOR  2

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

const uint16_t colors[] PROGMEM = {
    BLUE,
    RED,
    YELLOW,
    CYAN,
    GREEN,
    WHITE,
    MAGENTA,
    ORANGE
};

byte clock_color_index = 2;
byte date_color_index = 3;

uint16_t clock_color() { return pgm_read_word(&colors[clock_color_index]); }
uint16_t date_color()  { return pgm_read_word(&colors[date_color_index]); }

void saveColors() {
    EEPROM.update(ADDR_CLOCK_COLOR, clock_color_index & 0xFF);
    EEPROM.update(ADDR_CLOCK_COLOR + 1, clock_color_index >> 8);
    EEPROM.update(ADDR_DATE_COLOR, date_color_index & 0xFF);
    EEPROM.update(ADDR_DATE_COLOR + 1, date_color_index >> 8);
}

void loadColors() {
    byte c = EEPROM.read(ADDR_CLOCK_COLOR) | (EEPROM.read(ADDR_CLOCK_COLOR + 1) << 8);
    byte d = EEPROM.read(ADDR_DATE_COLOR)  | (EEPROM.read(ADDR_DATE_COLOR + 1) << 8);

    if (c != 0xFFFF) clock_color_index = c;
    if (d != 0xFFFF) date_color_index  = d;
}