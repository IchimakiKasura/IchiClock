#include "Min_Adafruit_ST77xx.h"
#include <limits.h>
#if !defined(ARDUINO_STM32_FEATHER) && !defined(ARDUINO_UNOR4_WIFI)
#if !defined(ARDUINO_UNOR4_MINIMA)
#include "pins_arduino.h"
#include "wiring_private.h"
#endif
#endif
#include <SPI.h>

#define SPI_DEFAULT_FREQ 32000000 

Adafruit_ST77xx::Adafruit_ST77xx(uint16_t w, uint16_t h, int8_t cs, int8_t dc,
                                 int8_t mosi, int8_t sclk, int8_t rst,
                                 int8_t miso)
    : Min_Adafruit_SPITFT(w, h, cs, dc, mosi, sclk, rst, miso) {}

Adafruit_ST77xx::Adafruit_ST77xx(uint16_t w, uint16_t h, int8_t cs, int8_t dc,
                                 int8_t rst)
    : Min_Adafruit_SPITFT(w, h, cs, dc, rst) {}

void Adafruit_ST77xx::displayInit(const uint8_t *addr) {

  uint8_t numCommands, cmd, numArgs;
  uint16_t ms;

  numCommands = pgm_read_byte(addr++); 
  while (numCommands--) {              
    cmd = pgm_read_byte(addr++);       
    numArgs = pgm_read_byte(addr++);   
    ms = numArgs & ST_CMD_DELAY;       
    numArgs &= ~ST_CMD_DELAY;          
    sendCommand(cmd, addr, numArgs);
    addr += numArgs;

    if (ms) {
      ms = pgm_read_byte(addr++); 
      if (ms == 255)
        ms = 500; 
      delay(ms);
    }
  }
}

void Adafruit_ST77xx::begin(uint32_t freq) {
  if (!freq) {
    freq = SPI_DEFAULT_FREQ;
  }
  _freq = freq;

  invertOnCommand = ST77XX_INVON;
  invertOffCommand = ST77XX_INVOFF;

  initSPI(freq, spiMode);
}

void Adafruit_ST77xx::commonInit(const uint8_t *cmdList) {
  begin();

  if (cmdList) {
    displayInit(cmdList);
  }
}

void Adafruit_ST77xx::setAddrWindow(uint16_t x, uint16_t y, uint16_t w,
                                    uint16_t h) {
  x += _xstart;
  y += _ystart;
  uint32_t xa = ((uint32_t)x << 16) | (x + w - 1);
  uint32_t ya = ((uint32_t)y << 16) | (y + h - 1);

  writeCommand(ST77XX_CASET); 
  SPI_WRITE32(xa);

  writeCommand(ST77XX_RASET); 
  SPI_WRITE32(ya);

  writeCommand(ST77XX_RAMWR); 
}

void Adafruit_ST77xx::setRotation(uint8_t m) {
  uint8_t madctl = 0;

  rotation = m % 4; 

  switch (rotation) {
  case 0:
    madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
    _xstart = _colstart;
    _ystart = _rowstart;
    break;
  case 1:
    madctl = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
    _ystart = _colstart;
    _xstart = _rowstart;
    break;
  case 2:
    madctl = ST77XX_MADCTL_RGB;
    _xstart = _colstart;
    _ystart = _rowstart;
    break;
  case 3:
    madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
    _ystart = _colstart;
    _xstart = _rowstart;
    break;
  }

  sendCommand(ST77XX_MADCTL, &madctl, 1);
}

void Adafruit_ST77xx::setColRowStart(int8_t col, int8_t row) {
  _colstart = col;
  _rowstart = row;
}

void Adafruit_ST77xx::enableDisplay(boolean enable) {
  sendCommand(enable ? ST77XX_DISPON : ST77XX_DISPOFF);
}

void Adafruit_ST77xx::enableTearing(boolean enable) {
  sendCommand(enable ? ST77XX_TEON : ST77XX_TEOFF);
}

void Adafruit_ST77xx::enableSleep(boolean enable) {
  sendCommand(enable ? ST77XX_SLPIN : ST77XX_SLPOUT);
}