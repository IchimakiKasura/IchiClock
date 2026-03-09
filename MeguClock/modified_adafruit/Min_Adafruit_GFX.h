#pragma once
#include "Arduino.h"
#include "Print.h"
#include "gfxfont.h"
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SPIDevice.h>

class Min_Adafruit_GFX : public Print {

public:
  Min_Adafruit_GFX(int16_t w, int16_t h);
  virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
  virtual void startWrite(void);
  virtual void writePixel(int16_t x, int16_t y, uint16_t color);
  virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  virtual void endWrite(void);
  virtual void MeguClockRotate(uint8_t commandByte, uint8_t *dataBytes);
  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  virtual void fillScreen(uint16_t color);
  virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y);
  void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
  void getTextBounds(const __FlashStringHelper *s, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
  void setTextSize(uint8_t s);
  void setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
  }
  void setTextColor(uint16_t c) { textcolor = textbgcolor = c; }
  void setTextColor(uint16_t c, uint16_t bg) {
    textcolor = c;
    textbgcolor = bg;
  }
  using Print::write;
  virtual size_t write(uint8_t);
  int16_t width(void) const { return _width; };
  int16_t height(void) const { return _height; }
  int16_t getCursorX(void) const { return cursor_x; };
  int16_t getCursorY(void) const { return cursor_y; };
protected:
  void charBounds(unsigned char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
  int16_t WIDTH;
  int16_t HEIGHT;
  int16_t _width;
  int16_t _height;
  int16_t cursor_x;    
  int16_t cursor_y;
  uint16_t textcolor;
  uint16_t textbgcolor;
  uint8_t textsize_x;
  uint8_t textsize_y;
};