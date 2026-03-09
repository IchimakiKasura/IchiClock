#include "Min_Adafruit_SPITFT.h"
#define AVR_WRITESPI(x) for (SPDR = (x); (!(SPSR & _BV(SPIF)));)
Min_Adafruit_SPITFT::Min_Adafruit_SPITFT(uint16_t w, uint16_t h, int8_t cs, int8_t dc, int8_t rst) 
  : Min_Adafruit_GFX(w, h), _rst(rst), _cs(cs), _dc(dc) {
  hwspi._spi = &SPI;
}
void Min_Adafruit_SPITFT::initSPI(uint32_t freq, uint8_t spiMode) {
  if (!freq) freq = 16000000L; 
  if (_cs >= 0) {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
  }
  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH); 

  hwspi.settings = SPISettings(freq, MSBFIRST, spiMode);
  hwspi._mode = spiMode;
  if (hwspi._spi) {
    hwspi._spi->begin();
  }

  if (_rst >= 0) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(100);
    digitalWrite(_rst, HIGH);
    delay(200);
  }
}
void Min_Adafruit_SPITFT::startWrite(void) {
  SPI_BEGIN_TRANSACTION();
  if (_cs >= 0) digitalWrite(_cs, LOW);
}
void Min_Adafruit_SPITFT::endWrite(void) {
  if (_cs >= 0) digitalWrite(_cs, HIGH);
  SPI_END_TRANSACTION();
}
inline void Min_Adafruit_SPITFT::writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  setAddrWindow(x, y, w, h);
  uint32_t len = (uint32_t)w * h;
  uint16_t buf[16];
  for (uint8_t i = 0; i < 16; i++) buf[i] = color;
  while (len) {
    uint8_t n = (len > 16) ? 16 : len;
    for (uint8_t j = 0; j < n; j++) SPI_WRITE16(buf[j]);
    len -= n;
  }
}
void Min_Adafruit_SPITFT::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
    startWrite();
    setAddrWindow(x, y, 1, 1);
    SPI_WRITE16(color);
    endWrite();
  }
}
void Min_Adafruit_SPITFT::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (w && h) {   
    if (w < 0) {  
      x += w + 1; 
      w = -w;     
    }
    if (x < _width) { 
      if (h < 0) {    
        y += h + 1;   
        h = -h;       
      }
      if (y < _height) { 
        int16_t x2 = x + w - 1;
        if (x2 >= 0) { 
          int16_t y2 = y + h - 1;
          if (y2 >= 0) { 
            
            if (x < 0) {
              x = 0;
              w = x2 + 1;
            } 
            if (y < 0) {
              y = 0;
              h = y2 + 1;
            } 
            if (x2 >= _width) {
              w = _width - x;
            } 
            if (y2 >= _height) {
              h = _height - y;
            } 
            startWrite();
            writeFillRectPreclipped(x, y, w, h, color);
            endWrite();
          }
        }
      }
    }
  }
}
void Min_Adafruit_SPITFT::MeguClockRotate(uint8_t commandByte, uint8_t *dataBytes) {
  startWrite();
  SPI_DC_LOW();
  AVR_WRITESPI(0x36);
  SPI_DC_HIGH();
  AVR_WRITESPI(0xC0);
  endWrite();
}
void Min_Adafruit_SPITFT::sendCommand(uint8_t commandByte, const uint8_t *dataBytes, uint8_t numDataBytes) {
  startWrite();
  SPI_DC_LOW();
  AVR_WRITESPI(commandByte); 
  SPI_DC_HIGH();
  for (int i = 0; i < numDataBytes; i++) {
      AVR_WRITESPI(pgm_read_byte(dataBytes++));
  }
  endWrite();
}
inline void Min_Adafruit_SPITFT::SPI_BEGIN_TRANSACTION(void) {
  hwspi._spi->beginTransaction(hwspi.settings);
}
inline void Min_Adafruit_SPITFT::SPI_END_TRANSACTION(void) {
  hwspi._spi->endTransaction();
}
void Min_Adafruit_SPITFT::writeCommand(uint8_t cmd) {
  SPI_DC_LOW();
  AVR_WRITESPI(cmd);
  SPI_DC_HIGH();
}
void Min_Adafruit_SPITFT::SPI_WRITE16(uint16_t w) {
  AVR_WRITESPI(w >> 8);
  AVR_WRITESPI(w);
}
void Min_Adafruit_SPITFT::SPI_WRITE32(uint32_t l) {
  AVR_WRITESPI(l >> 24);
  AVR_WRITESPI(l >> 16);
  AVR_WRITESPI(l >> 8);
  AVR_WRITESPI(l);
}