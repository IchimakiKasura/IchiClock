#pragma once
#include "Arduino.h"
#include "Print.h"
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

class MeguClock_ST7735 : public Print {
private:
    inline void startWrite();
    inline void endWrite();
    void setAddrWindow(uint16_t x, uint16_t y, uint8_t w, uint8_t h);
    inline void writeAVRSPI(uint8_t addr);
public:
    MeguClock_ST7735(int8_t cs, int8_t dc, int8_t rst);
    ~MeguClock_ST7735() {};
    void init();
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint16_t color);
    void fillScreen(uint16_t color);
    inline void writeColor(uint16_t color, uint32_t length);
    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_tx);
    void getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint8_t *w, uint8_t *h);
    void getTextBounds(const __FlashStringHelper *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint8_t *w, uint8_t *h);
    using Print::write;
    size_t write(uint8_t c);
    inline void setTextSize(uint8_t s) { textsize = (s > 0) ? s : 1; }
    inline void setCursor(int16_t x, int16_t y) { cursor_x = x; cursor_y = y; }
    inline void setTextColor(uint16_t c) { textcolor = textbgcolor = c; }
protected:
    void displayInit(const uint8_t *addr);
    void charBounds(unsigned char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
    struct {          
        SPIClass *_spi = &SPI; 
        SPISettings settings = SPISettings(16000000, MSBFIRST, SPI_MODE0);
    } hwspi;
    uint8_t _rst;
    uint8_t _cs;
    uint8_t _dc;
    uint8_t _width;
    uint8_t _height;
    int16_t cursor_x;    
    int16_t cursor_y;
    uint16_t textcolor;
    uint16_t textbgcolor;
    uint8_t textsize;
};