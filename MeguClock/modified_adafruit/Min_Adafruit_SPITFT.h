#pragma once
#include "Min_Adafruit_GFX.h"
#include <SPI.h>
typedef uint8_t ADAGFX_PORT_t;
typedef volatile ADAGFX_PORT_t* PORTreg_t;
enum tftBusWidth { tft8bitbus, tft16bitbus };
class Min_Adafruit_SPITFT : public Min_Adafruit_GFX {
public:
  Min_Adafruit_SPITFT(uint16_t w, uint16_t h, int8_t cs, int8_t dc, int8_t rst = -1);
  ~Min_Adafruit_SPITFT() {};
  virtual void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
  void initSPI(uint32_t freq = 0, uint8_t spiMode = SPI_MODE0);
  void startWrite(void);
  void endWrite(void);
  void MeguClockRotate(uint8_t commandByte, uint8_t *dataBytes);
  void sendCommand(uint8_t commandByte, const uint8_t *dataBytes = NULL, uint8_t numDataBytes = 0);
  inline void writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void writeCommand(uint8_t cmd);
  void SPI_WRITE16(uint16_t w);
  void SPI_WRITE32(uint32_t l);
  void SPI_DC_HIGH(void) { digitalWrite(_dc, HIGH); }
  void SPI_DC_LOW(void) { digitalWrite(_dc, LOW); }
protected:
  inline void SPI_BEGIN_TRANSACTION(void);
  inline void SPI_END_TRANSACTION(void); 
  struct {          
    SPIClass *_spi; 
    SPISettings settings;
    uint32_t _mode; 
  } hwspi;    
  int8_t _rst;             
  int8_t _cs;              
  int8_t _dc;              
  int16_t _xstart = 0;          
  int16_t _ystart = 0;          
  uint8_t invertOnCommand = 0;  
  uint8_t invertOffCommand = 0; 
  uint32_t _freq = 0; 
};