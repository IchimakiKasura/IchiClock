#pragma once
#include "Arduino.h"
#include "Print.h"
#include "Min_Adafruit_GFX.h"
#include "Min_Adafruit_SPITFT.h"

#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define ORANGE 0xFC00

class Adafruit_ST7735 : public Min_Adafruit_SPITFT {
public:
  Adafruit_ST7735(int8_t cs, int8_t dc, int8_t rst);
  void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  void initR();
protected:
  uint8_t _colstart = 0,   
      _rowstart = 0,       
      spiMode = SPI_MODE0;
  void displayInit(const uint8_t *addr);
};