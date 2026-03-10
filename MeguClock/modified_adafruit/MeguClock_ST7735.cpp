/**************************************************************************
    This library is a modified version of the Adafruit_ST7735_and_ST7789_Library
    combined with Adafruit_GFX_Library.

    https://github.com/adafruit/Adafruit-GFX-Library
    https://github.com/adafruit/Adafruit-ST7735-Library
    
    If you wish to use this feel free though it is hard coded for
      - Arduino boards (AVR)
      - 1.8" GREEN TAB 128x160
      - Screen is Flipped

    It is specifically designed for my simple digital clock as the library
    is flash intensive, I've scrap the unused ones to barebones of the code.
**************************************************************************/
#include "MeguClock_ST7735.h"
#include "glcdfont.c"
#include <avr/pgmspace.h>

#define AVR_WRITESPI(x) for (SPDR = (x); (!(SPSR & _BV(SPIF)));)

static const uint8_t PROGMEM
  Rcmd1[] = {
       15,                             
     0x01,  0x80, 
      150,                          
     0x11,  0x80, 
      255,                          
     0xB1,     3,              
     0x01,  0x2C,  0x2D,            
     0xB2,     3,             
     0x01,  0x2C,  0x2D,             
     0xB3,     6,              
     0x01,  0x2C,  0x2D,             
     0x01,  0x2C,  0x2D,             
     0xB4,     1,              
     0x07,                         
     0xC0,     3,              
     0xA2,
     0x02,                         
     0x84,                         
     0xC1,     1,              
     0xC5,                         
     0xC2,     2,              
     0x0A,                        
     0x00,                        
     0xC3,     2,             
     0x8A,                         
     0x2A,                         
     0xC4,     2,             
     0x8A,  0xEE,
     0xC5,     1,             
     0x0E,
     0x20,     0,              
     0x36,     1,              
     0xC8,                        
     0x3A,     1,             
     0x05
},
  Rcmd2green[] = {              // This is where the part
        2,                      // where you most likely change
     0x2A,     4,               // to RED or other tab
     0x00,  0x02,               // if you want to use this minified version
     0x00,  0x7F+0x02,          // 
     0x2B,     4,               // seek to Adafruit_ST77xx to see the red tab code
     0x00,  0x01,                  
     0x00,  0x9F+0x01
}, 
  Rcmd3[] = { 
        4,                             
     0xE0,    16,       
     0x02,  0x1c,  0x07,  0x12,      
     0x37,  0x32,  0x29,  0x2d,       
     0x29,  0x25,  0x2B,  0x39,
     0x00,  0x01,  0x03,  0x10,
     0xE1,    16,     
     0x03,  0x1d,  0x07,  0x06,       
     0x2E,  0x2C,  0x29,  0x2D,       
     0x2E,  0x2E,  0x37,  0x3F,
     0x00,  0x00,  0x02,  0x10,
     0x13,  0x80, 
       10,
     0x29,  0x80,
      100
};

MeguClock_ST7735::MeguClock_ST7735(int8_t cs, int8_t dc, int8_t rst) : _rst(rst), _cs(cs), _dc(dc)  {
    hwspi._spi = &SPI;
    _width = TFT_WIDTH;
    _height = TFT_HEIGHT;
    cursor_y = cursor_x = 0;
    textsize_x = textsize_y = 1;
    textcolor = textbgcolor = 0xFFFF;
}
void MeguClock_ST7735::initR() {
    DDRB = (1 << _rst)|(1 << _dc)|(1 << _cs);
    PORTB |= (1 << _rst)|(1 << _dc)|(1 << _cs);
    hwspi.settings = SPISettings(16000000L, MSBFIRST, SPI_MODE0);
    if(hwspi._spi) hwspi._spi->begin();
    displayInit(Rcmd1);
    displayInit(Rcmd2green);
    displayInit(Rcmd3);
    startWrite();
    writeAVRSPI(0x36);
    AVR_WRITESPI(0xC0);    // Flips the screen and also fixes BGR
    endWrite();
}
void MeguClock_ST7735::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) { _swap_int16_t(x0, y0); _swap_int16_t(x1, y1); }
    if (x0 > x1) { _swap_int16_t(x0, x1); _swap_int16_t(y0, y1); }
    int16_t dx = x1 - x0, dy = abs(y1 - y0), err = dx >> 1, ystep = (y0 < y1) ? 1 : -1;
    startWrite();
    for (; x0 <= x1; x0++) {
        int16_t px = steep ? y0 : x0, py = steep ? x0 : y0;
        setAddrWindow(px, py, 1, 1);
        writeColor(color, 1);
        if ((err -= dy) < 0) { y0 += ystep; err += dx; }
    }
}
void MeguClock_ST7735::displayInit(const uint8_t *addr) {
  uint8_t numCommands = pgm_read_byte(addr++);
  while (numCommands--) {
    uint8_t cmd = pgm_read_byte(addr++);
    uint8_t numArgs = pgm_read_byte(addr++);
    uint16_t ms = numArgs & 0x80;
    numArgs &= ~0x80;
    startWrite();
    writeAVRSPI(cmd);
    while (numArgs--) AVR_WRITESPI(pgm_read_byte(addr++));
    endWrite();
    if (ms) {
      ms = pgm_read_byte(addr++);
      delay(ms == 255 ? 500 : ms);
    }
  }
}
void MeguClock_ST7735::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    x += 2; y += 1;
    startWrite();
    writeAVRSPI(0x2A);
    AVR_WRITESPI(x >> 8); AVR_WRITESPI(x & 0xFF);
    AVR_WRITESPI((x + w - 1) >> 8); AVR_WRITESPI((x + w - 1) & 0xFF);
    writeAVRSPI(0x2B); 
    AVR_WRITESPI(y >> 8); AVR_WRITESPI(y & 0xFF);
    AVR_WRITESPI((y + h - 1) >> 8); AVR_WRITESPI((y + h - 1) & 0xFF);
    writeAVRSPI(0x2C);
}
void MeguClock_ST7735::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if(x<0 ||x>=_width || y<0 || y>=_height) return;
  setAddrWindow(x, y, 1, 1);
  startWrite();
  writeColor(color, 1);
  endWrite();
}
void MeguClock_ST7735::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  writeLine(x, y, x, y + h - 1, color);
}
void MeguClock_ST7735::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  writeLine(x, y, x + w - 1, y, color);
}
void MeguClock_ST7735::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if(x>=_width || y>=_height || w<=0 || h<=0) return;
  if(x+w-1>=_width)  w=_width -x;
  if(y+h-1>=_height) h=_height-y;
  startWrite();
  setAddrWindow(x, y, w, h);
  writeColor(color, (uint32_t)w * h);
  endWrite();
}
void MeguClock_ST7735::fillScreen(uint16_t color) {
  startWrite();
  setAddrWindow(0, 0, _width, _height);
  writeColor(color, _width * _height);
  endWrite();
}
void MeguClock_ST7735::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y) {
  if ((x >= _width) || (y >= _height) || ((x + 6 * size_x - 1) < 0) || ((y + 8 * size_y - 1) < 0)) return;
  if ((c >= 176)) c++; 
  for (int8_t i = 0; i < 5; i++) { 
    uint8_t line = pgm_read_byte(&font[c * 5 + i]);
    for (int8_t j = 0; j < 8; j++, line >>= 1) {
      if (line & 1) {
        if (size_x == 1 && size_y == 1)
          drawPixel(x + i, y + j, color);
        else
          fillRect(x + i * size_x, y + j * size_y, size_x, size_y, color);
      } else if (bg != color) {
        if (size_x == 1 && size_y == 1)
          drawPixel(x + i, y + j, bg);
        else
          fillRect(x + i * size_x, y + j * size_y, size_x, size_y, bg);
      }
    }
  }
  if (bg != color) { 
    if (size_x == 1 && size_y == 1)
      drawFastVLine(x + 5, y, 8, bg);
    else
      fillRect(x + 5 * size_x, y, size_x, 8 * size_y, bg);
  }
}
size_t MeguClock_ST7735::write(uint8_t c) {
  if (c == '\n') {              
    cursor_x = 0;               
    cursor_y += textsize_y * 8; 
  } else if (c != '\r') {       
    if ((cursor_x + textsize_x * 6) > _width) { 
      cursor_x = 0;                                       
      cursor_y += textsize_y * 8; 
    }
    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
              textsize_y);
    cursor_x += textsize_x * 6; 
  }
  return 1;
}
void MeguClock_ST7735::getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    uint8_t c;
    int16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1;
    *x1 = x; *y1 = y; *w = *h = 0;

    while((c = *str++)) charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);

    if(maxx >= minx) *w = maxx - minx + 1;
    if(maxy >= miny) *h = maxy - miny + 1;
    *x1 = minx; *y1 = miny;
}
void MeguClock_ST7735::getTextBounds(const __FlashStringHelper *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    uint8_t *s = (uint8_t *)str, c;
    int16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1;
    *x1 = x; *y1 = y; *w = *h = 0;
    while ((c = pgm_read_byte(s++))) charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
    if(maxx >= minx) *w = maxx - minx + 1;
    if(maxy >= miny) *h = maxy - miny + 1;
    *x1 = minx; *y1 = miny;
}
void MeguClock_ST7735::charBounds(unsigned char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy) {
  if(c == '\n') { *x = 0; *y += textsize_y * 8; }
    else if(c != '\r') {
      if(*x + textsize_x * 6 > _width) {
          *x = 0;
          *y += textsize_y * 8;
      }
      int16_t x2 = *x + textsize_x * 5 - 1;
      int16_t y2 = *y + textsize_y * 8 - 1;
      if(x2 > *maxx) *maxx = x2;
      if(y2 > *maxy) *maxy = y2;
      if(*x < *minx) *minx = *x;
      if(*y < *miny) *miny = *y;
      *x += textsize_x * 6;
  }
}
void MeguClock_ST7735::startWrite() {
  hwspi._spi->beginTransaction(hwspi.settings);
  PORTB &= ~(1 << 2);
}
void MeguClock_ST7735::endWrite() {
  PORTB &= ~(1 << 2);
  hwspi._spi->endTransaction();
}
void MeguClock_ST7735::writeAVRSPI(uint8_t addr) {
  SPI_DC_LOW();
  AVR_WRITESPI(addr);
  SPI_DC_HIGH();
}
inline void MeguClock_ST7735::writeColor(uint16_t color, uint32_t len) {
    uint8_t hi = color >> 8, lo = color & 0xFF;
    while (len--) {
        AVR_WRITESPI(hi);
        AVR_WRITESPI(lo);
    }
}