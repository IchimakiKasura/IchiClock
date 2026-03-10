#pragma once
#include "Arduino.h"
#include "Print.h"
#include "gfxfont.h"
#include <Adafruit_SPIDevice.h>
#include <SPI.h>

#define TFT_HEIGHT 160
#define TFT_WIDTH 128

#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define ORANGE 0xFC00

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

class MeguClock_ST7735 : public Print {
private:
    void startWrite();
    void endWrite();
    void setAddrWindow(uint16_t, uint16_t, uint16_t, uint16_t);
    void SPI_DC_HIGH() { PORTB |= (1 << _dc); }
    void SPI_DC_LOW() { PORTB &= ~(1 << _dc); }
    void writeAVRSPI(uint8_t);
public:
    MeguClock_ST7735(int8_t, int8_t, int8_t);
    ~MeguClock_ST7735() {};
    void initR();
    void drawPixel(int16_t, int16_t, uint16_t);
    void writeLine(int16_t, int16_t, int16_t, int16_t, uint16_t);
    void drawFastVLine(int16_t, int16_t, int16_t, uint16_t);
    void drawFastHLine(int16_t, int16_t, int16_t, uint16_t);
    void fillRect(int16_t, int16_t, int16_t, int16_t, uint16_t);
    void fillScreen(uint16_t);
    inline void writeColor(uint16_t, uint32_t);
    void drawChar(int16_t, int16_t, unsigned char, uint16_t, uint16_t, uint8_t, uint8_t);
    void getTextBounds(const char*, int16_t, int16_t, int16_t*, int16_t*, uint16_t*, uint16_t*);
    void getTextBounds(const __FlashStringHelper*, int16_t, int16_t, int16_t*, int16_t*, uint16_t*, uint16_t*);
    using Print::write;
    size_t write(uint8_t);
    void setTextSize(uint8_t s) { textsize_x = textsize_y = (s > 0) ? s : 1; }
    void setCursor(int16_t x, int16_t y) { cursor_x = x; cursor_y = y; }
    void setTextColor(uint16_t c) { textcolor = textbgcolor = c; }
    uint8_t width() const { return _width; }
    uint8_t height() const { return _height; }
protected:
    void displayInit(const uint8_t*);
    void charBounds(unsigned char, int16_t*, int16_t*, int16_t*, int16_t*, int16_t*, int16_t*);
    inline void SPI_BEGIN_TRANSACTION();
    inline void SPI_END_TRANSACTION();
    struct {          
        SPIClass *_spi; 
        SPISettings settings;
    } hwspi;
    int8_t _rst;
    int8_t _cs;
    int8_t _dc;
    int16_t _width;
    int16_t _height;
    int16_t cursor_x;    
    int16_t cursor_y;
    uint16_t textcolor;
    uint16_t textbgcolor;
    uint8_t textsize_x;
    uint8_t textsize_y;
};