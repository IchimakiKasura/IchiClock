/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http:
  The 1.8" TFT shield
    ----> https:
  The 1.44" TFT breakout
    ----> https:
  as well as Adafruit raw 1.8" TFT display
    ----> http:

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#ifndef _ADAFRUIT_ST77XXH_
#define _ADAFRUIT_ST77XXH_

#include "Arduino.h"
#include "Print.h"
#include "Min_Adafruit_GFX.h"
#include "Min_Adafruit_SPITFT.h"

#define ST7735_TFTWIDTH_128 128  
#define ST7735_TFTWIDTH_80 80    
#define ST7735_TFTHEIGHT_128 128 
#define ST7735_TFTHEIGHT_160 160 


#define ST_CMD_DELAY 0x80 

#define ST77XX_SWRESET 0x01

#define ST77XX_SLPIN 0x10
#define ST77XX_SLPOUT 0x11
#define ST77XX_PTLON 0x12
#define ST77XX_NORON 0x13
#define ST77XX_INVOFF 0x20
#define ST77XX_INVON 0x21
#define ST77XX_DISPOFF 0x28
#define ST77XX_DISPON 0x29
#define ST77XX_CASET 0x2A
#define ST77XX_RASET 0x2B
#define ST77XX_RAMWR 0x2C
#define ST77XX_RAMRD 0x2E
#define ST77XX_PTLAR 0x30
#define ST77XX_TEOFF 0x34
#define ST77XX_TEON 0x35
#define ST77XX_MADCTL 0x36
#define ST77XX_COLMOD 0x3A
#define ST77XX_MADCTL_MY 0x80
#define ST77XX_MADCTL_MX 0x40
#define ST77XX_MADCTL_MV 0x20
#define ST77XX_MADCTL_ML 0x10
#define ST77XX_MADCTL_RGB 0x00
#define ST77XX_RDID1 0xDA
#define ST77XX_RDID2 0xDB
#define ST77XX_RDID3 0xDC
#define ST77XX_RDID4 0xDD

#define ST77XX_BLACK 0x0000
#define ST77XX_WHITE 0xFFFF
#define ST77XX_RED 0xF800
#define ST77XX_GREEN 0x07E0
#define ST77XX_BLUE 0x001F
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW 0xFFE0
#define ST77XX_ORANGE 0xFC00

class Adafruit_ST77xx : public Min_Adafruit_SPITFT {
public:
  Adafruit_ST77xx(uint16_t w, uint16_t h, int8_t _CS, int8_t _DC, int8_t _MOSI,
                  int8_t _SCLK, int8_t _RST = -1, int8_t _MISO = -1);
  Adafruit_ST77xx(uint16_t w, uint16_t h, int8_t CS, int8_t RS,
                  int8_t RST = -1);
  void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  void setRotation(uint8_t r);
  void enableDisplay(boolean enable);
  void enableTearing(boolean enable);
  void enableSleep(boolean enable);

protected:
  uint8_t _colstart = 0,   
      _rowstart = 0,       
      spiMode = SPI_MODE0; 

  void begin(uint32_t freq = 0);
  void commonInit(const uint8_t *cmdList);
  void displayInit(const uint8_t *addr);
  void setColRowStart(int8_t col, int8_t row);
};

#endif 
