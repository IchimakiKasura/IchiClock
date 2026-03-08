#ifndef _ADAFRUIT_SPITFT_H_
#define _ADAFRUIT_SPITFT_H_

#if !defined(__AVR_ATtiny85__) && !defined(__AVR_ATtiny84__)

#include "Min_Adafruit_GFX.h"
#include <SPI.h>

#if defined(__AVR__)
typedef uint8_t ADAGFX_PORT_t;       
#define USE_FAST_PINIO               
#elif defined(ARDUINO_STM32_FEATHER) 
typedef class HardwareSPI SPIClass; 
typedef uint32_t ADAGFX_PORT_t;     
#elif defined(__arm__)
#if defined(ARDUINO_ARCH_SAMD)
typedef uint32_t ADAGFX_PORT_t; 
#define USE_FAST_PINIO   
#define HAS_PORT_SET_CLR 
#elif defined(CORE_TEENSY)
#if defined(__IMXRT1052__) || defined(__IMXRT1062__) 
typedef uint32_t ADAGFX_PORT_t; 
#else
typedef uint8_t ADAGFX_PORT_t; 
#endif
#define USE_FAST_PINIO   
#define HAS_PORT_SET_CLR 
#else
typedef uint32_t ADAGFX_PORT_t; 
#endif
#else
typedef uint32_t ADAGFX_PORT_t; 
#endif                                     
typedef volatile ADAGFX_PORT_t *PORTreg_t; 
#if defined(__AVR__) && !defined(__LGT8F__)
#define DEFAULT_SPI_FREQ 8000000L 
#else
#define DEFAULT_SPI_FREQ 16000000L 
#endif
#if defined(ADAFRUIT_PYPORTAL) || defined(ADAFRUIT_PYPORTAL_M4_TITANO) ||      \
    defined(ADAFRUIT_PYBADGE_M4_EXPRESS) ||                                    \
    defined(ADAFRUIT_PYGAMER_M4_EXPRESS) ||                                    \
    defined(ADAFRUIT_MONSTER_M4SK_EXPRESS) || defined(NRF52_SERIES) ||         \
    defined(ADAFRUIT_CIRCUITPLAYGROUND_M0)
#define USE_SPI_DMA 
#else                           
#endif
#if defined(USE_SPI_DMA) && (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
#include <Adafruit_ZeroDMA.h>
#endif
enum tftBusWidth { tft8bitbus, tft16bitbus };
#if defined(ARDUINO_ARCH_RP2040)
#ifndef __SPI0_DEVICE
#define __SPI0_DEVICE spi0
#endif
#ifndef __SPI1_DEVICE
#define __SPI1_DEVICE spi1
#endif
#endif
class Min_Adafruit_SPITFT : public Min_Adafruit_GFX {

public:
  Min_Adafruit_SPITFT(uint16_t w, uint16_t h, int8_t cs, int8_t dc, int8_t mosi,
                  int8_t sck, int8_t rst = -1, int8_t miso = -1);
  Min_Adafruit_SPITFT(uint16_t w, uint16_t h, int8_t cs, int8_t dc,
                  int8_t rst = -1);
  Min_Adafruit_SPITFT(uint16_t w, uint16_t h, SPIClass *spiClass, int8_t cs,
                  int8_t dc, int8_t rst = -1);
  ~Min_Adafruit_SPITFT(){};
  virtual void begin(uint32_t freq) = 0;
  virtual void setAddrWindow(uint16_t x, uint16_t y, uint16_t w,
                             uint16_t h) = 0;
  void initSPI(uint32_t freq = 0, uint8_t spiMode = SPI_MODE0);
  void setSPISpeed(uint32_t freq);
  void startWrite(void);
  void endWrite(void);
  void sendCommand(uint8_t commandByte, uint8_t *dataBytes,
                   uint8_t numDataBytes);
  void sendCommand(uint8_t commandByte, const uint8_t *dataBytes = NULL,
                   uint8_t numDataBytes = 0);
  void sendCommand16(uint16_t commandWord, const uint8_t *dataBytes = NULL,
                     uint8_t numDataBytes = 0);
  uint8_t readcommand8(uint8_t commandByte, uint8_t index = 0);
  uint16_t readcommand16(uint16_t addr);
  void writePixel(int16_t x, int16_t y, uint16_t color);
  void writePixels(uint16_t *colors, uint32_t len, bool block = true,
                   bool bigEndian = false);
  void writeColor(uint16_t color, uint32_t len);
  void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                     uint16_t color);
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  inline void writeFillRectPreclipped(int16_t x, int16_t y, int16_t w,
                                      int16_t h, uint16_t color);
  void dmaWait(void);
  bool dmaBusy(void) const; 
  void swapBytes(uint16_t *src, uint32_t len, uint16_t *dest = NULL);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void pushColor(uint16_t color);
  void invertDisplay(bool i);
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
  void spiWrite(uint8_t b);          
  void writeCommand(uint8_t cmd);    
  uint8_t spiRead(void);             
  void write16(uint16_t w);          
  void writeCommand16(uint16_t cmd); 
  uint16_t read16(void);
  void SPI_WRITE16(uint16_t w); 
  void SPI_WRITE32(uint32_t l);
  void SPI_CS_HIGH(void) {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
    *csPortSet = 1;
#else  
    *csPortSet = csPinMask;
#endif 
#else  
    *csPort |= csPinMaskSet;
#endif 
#else  
    digitalWrite(_cs, HIGH);
#endif 
  }
  void SPI_CS_LOW(void) {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
    *csPortClr = 1;
#else  
    *csPortClr = csPinMask;
#endif 
#else  
    *csPort &= csPinMaskClr;
#endif 
#else  
    digitalWrite(_cs, LOW);
#endif 
  }
  void SPI_DC_HIGH(void) {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
    *dcPortSet = 1;
#else  
    *dcPortSet = dcPinMask;
#endif 
#else  
    *dcPort |= dcPinMaskSet;
#endif 
#else  
    digitalWrite(_dc, HIGH);
#endif 
  }
  void SPI_DC_LOW(void) {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
    *dcPortClr = 1;
#else  
    *dcPortClr = dcPinMask;
#endif 
#else  
    *dcPort &= dcPinMaskClr;
#endif 
#else  
    digitalWrite(_dc, LOW);
#endif 
  }

protected:
  inline void SPI_MOSI_HIGH(void);
  inline void SPI_MOSI_LOW(void);
  inline void SPI_SCK_HIGH(void);
  inline void SPI_SCK_LOW(void);
  inline bool SPI_MISO_READ(void);
  inline void SPI_BEGIN_TRANSACTION(void);
  inline void SPI_END_TRANSACTION(void);
  inline void TFT_WR_STROBE(void); 
  inline void TFT_RD_HIGH(void);   
  inline void TFT_RD_LOW(void);    

#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
  PORTreg_t csPortSet; 
  PORTreg_t csPortClr; 
  PORTreg_t dcPortSet; 
  PORTreg_t dcPortClr; 
#else                  
  PORTreg_t csPort; 
  PORTreg_t dcPort; 
#endif                 
#endif                 
#if defined(__cplusplus) && (__cplusplus >= 201100)
  union {
#endif
    struct {          
      SPIClass *_spi; 
#if defined(SPI_HAS_TRANSACTION)
      SPISettings settings; 
#else
    uint32_t _freq; 
#endif
      uint32_t _mode; 
    } hwspi;          
    struct {          
#if defined(USE_FAST_PINIO)
      PORTreg_t misoPort; 
#if defined(HAS_PORT_SET_CLR)
      PORTreg_t mosiPortSet; 
      PORTreg_t mosiPortClr; 
      PORTreg_t sckPortSet;  
      PORTreg_t sckPortClr;  
#if !defined(KINETISK)
      ADAGFX_PORT_t mosiPinMask; 
      ADAGFX_PORT_t sckPinMask;  
#endif                           
#else                            
      PORTreg_t mosiPort;           
      PORTreg_t sckPort;            
      ADAGFX_PORT_t mosiPinMaskSet; 
      ADAGFX_PORT_t mosiPinMaskClr; 
      ADAGFX_PORT_t sckPinMaskSet;  
      ADAGFX_PORT_t sckPinMaskClr;  
#endif                           
#if !defined(KINETISK)
      ADAGFX_PORT_t misoPinMask; 
#endif                           
#endif                           
      int8_t _mosi;              
      int8_t _miso;              
      int8_t _sck;               
    } swspi;                     
    struct {                     
#if defined(USE_FAST_PINIO)

#if defined(__IMXRT1052__) || defined(__IMXRT1062__) 
      volatile uint32_t *writePort; 
      volatile uint32_t *readPort;  
#else
      volatile uint8_t *writePort; 
      volatile uint8_t *readPort;  
#endif
#if defined(HAS_PORT_SET_CLR)
      
      
#if defined(__IMXRT1052__) || defined(__IMXRT1062__) 
      volatile uint32_t *dirSet; 
      volatile uint32_t *dirClr; 
#else
      volatile uint8_t *dirSet; 
      volatile uint8_t *dirClr; 
#endif
      PORTreg_t wrPortSet; 
      PORTreg_t wrPortClr; 
      PORTreg_t rdPortSet; 
      PORTreg_t rdPortClr; 
#if !defined(KINETISK)
      ADAGFX_PORT_t wrPinMask; 
#endif                         
      ADAGFX_PORT_t rdPinMask; 
#else
      volatile uint8_t *portDir;  
      PORTreg_t wrPort;           
      PORTreg_t rdPort;           
      ADAGFX_PORT_t wrPinMaskSet; 
      ADAGFX_PORT_t wrPinMaskClr; 
      ADAGFX_PORT_t rdPinMaskSet; 
      ADAGFX_PORT_t rdPinMaskClr; 
#endif                         
#endif                         
      int8_t _d0;              
      int8_t _wr;              
      int8_t _rd;              
      bool wide = 0;           
    } tft8;                    
#if defined(__cplusplus) && (__cplusplus >= 201100)
  }; 
#endif
#if defined(USE_SPI_DMA) &&                                                    \
    (defined(__SAMD51__) ||                                                    \
     defined(ARDUINO_SAMD_ZERO))     
  Adafruit_ZeroDMA dma;              
  DmacDescriptor *dptr = NULL;       
  DmacDescriptor *descriptor = NULL; 
  uint16_t *pixelBuf[2];             
  uint16_t maxFillLen;               
  uint16_t lastFillColor = 0;        
  uint32_t lastFillLen = 0;          
  uint8_t onePixelBuf;               
#endif
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if !defined(KINETISK)
  ADAGFX_PORT_t csPinMask; 
  ADAGFX_PORT_t dcPinMask; 
#endif                     
#else                      
  ADAGFX_PORT_t csPinMaskSet; 
  ADAGFX_PORT_t csPinMaskClr; 
  ADAGFX_PORT_t dcPinMaskSet; 
  ADAGFX_PORT_t dcPinMaskClr; 
#endif                     
#endif                     
  uint8_t connection;      
  int8_t _rst;             
  int8_t _cs;              
  int8_t _dc;              
  int16_t _xstart = 0;          
  int16_t _ystart = 0;          
  uint8_t invertOnCommand = 0;  
  uint8_t invertOffCommand = 0; 
  uint32_t _freq = 0; 
};
#endif 
#endif 