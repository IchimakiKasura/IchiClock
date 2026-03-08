#include "Min_Adafruit_SPITFT.h"

#if defined(__AVR__)
#if defined(__AVR_XMEGA__) 
#define AVR_WRITESPI(x)                                                        \
  for (SPI0_DATA = (x); (!(SPI0_INTFLAGS & _BV(SPI_IF_bp)));)
#elif defined(__LGT8F__)
#define AVR_WRITESPI(x)                                                        \
  SPDR = (x);                                                                  \
  asm volatile("nop");                                                         \
  while ((SPFR & _BV(RDEMPT)))                                                 \
    ;                                                                          \
  SPFR = _BV(RDEMPT) | _BV(WREMPT)
#else
#define AVR_WRITESPI(x) for (SPDR = (x); (!(SPSR & _BV(SPIF)));)
#endif
#endif

#if defined(PORT_IOBUS)
#undef digitalPinToPort
#define digitalPinToPort(P) (&(PORT_IOBUS->Group[g_APinDescription[P].ulPort]))
#endif

#if defined(USE_SPI_DMA) && (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))

#include "wiring_private.h" 
#include <Adafruit_ZeroDMA.h>
#include <malloc.h>          
#define tcNum 2              
#define wrPeripheral PIO_CCL

static volatile bool dma_busy = false;
static void dma_callback(Adafruit_ZeroDMA *dma) { dma_busy = false; }

#if defined(__SAMD51__)
static const struct {
  Tc *tc;   
  int gclk; 
  int evu;  
} tcList[] = {{TC0, TC0_GCLK_ID, EVSYS_ID_USER_TC0_EVU},
              {TC1, TC1_GCLK_ID, EVSYS_ID_USER_TC1_EVU},
              {TC2, TC2_GCLK_ID, EVSYS_ID_USER_TC2_EVU},
              {TC3, TC3_GCLK_ID, EVSYS_ID_USER_TC3_EVU},
#if defined(TC4)
              {TC4, TC4_GCLK_ID, EVSYS_ID_USER_TC4_EVU},
#endif
#if defined(TC5)
              {TC5, TC5_GCLK_ID, EVSYS_ID_USER_TC5_EVU},
#endif
#if defined(TC6)
              {TC6, TC6_GCLK_ID, EVSYS_ID_USER_TC6_EVU},
#endif
#if defined(TC7)
              {TC7, TC7_GCLK_ID, EVSYS_ID_USER_TC7_EVU}
#endif
};
#define NUM_TIMERS (sizeof tcList / sizeof tcList[0]) 
#endif                                                
#endif

#define TFT_HARD_SPI 0
#define TFT_SOFT_SPI 1
#define TFT_PARALLEL 2

Min_Adafruit_SPITFT::Min_Adafruit_SPITFT(uint16_t w, uint16_t h, int8_t cs, int8_t dc,
                                 int8_t mosi, int8_t sck, int8_t rst,
                                 int8_t miso)
    : Min_Adafruit_GFX(w, h), connection(TFT_SOFT_SPI), _rst(rst), _cs(cs),
      _dc(dc) {
  swspi._sck = sck;
  swspi._mosi = mosi;
  swspi._miso = miso;
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(CORE_TEENSY)
#if !defined(KINETISK)
  dcPinMask = digitalPinToBitMask(dc);
  swspi.sckPinMask = digitalPinToBitMask(sck);
  swspi.mosiPinMask = digitalPinToBitMask(mosi);
#endif
  dcPortSet = portSetRegister(dc);
  dcPortClr = portClearRegister(dc);
  swspi.sckPortSet = portSetRegister(sck);
  swspi.sckPortClr = portClearRegister(sck);
  swspi.mosiPortSet = portSetRegister(mosi);
  swspi.mosiPortClr = portClearRegister(mosi);
  if (cs >= 0) {
#if !defined(KINETISK)
    csPinMask = digitalPinToBitMask(cs);
#endif
    csPortSet = portSetRegister(cs);
    csPortClr = portClearRegister(cs);
  } else {
#if !defined(KINETISK)
    csPinMask = 0;
#endif
    csPortSet = dcPortSet;
    csPortClr = dcPortClr;
  }
  if (miso >= 0) {
    swspi.misoPort = portInputRegister(miso);
#if !defined(KINETISK)
    swspi.misoPinMask = digitalPinToBitMask(miso);
#endif
  } else {
    swspi.misoPort = portInputRegister(dc);
  }
#else  
  dcPinMask = digitalPinToBitMask(dc);
  swspi.sckPinMask = digitalPinToBitMask(sck);
  swspi.mosiPinMask = digitalPinToBitMask(mosi);
  dcPortSet = &(PORT->Group[g_APinDescription[dc].ulPort].OUTSET.reg);
  dcPortClr = &(PORT->Group[g_APinDescription[dc].ulPort].OUTCLR.reg);
  swspi.sckPortSet = &(PORT->Group[g_APinDescription[sck].ulPort].OUTSET.reg);
  swspi.sckPortClr = &(PORT->Group[g_APinDescription[sck].ulPort].OUTCLR.reg);
  swspi.mosiPortSet = &(PORT->Group[g_APinDescription[mosi].ulPort].OUTSET.reg);
  swspi.mosiPortClr = &(PORT->Group[g_APinDescription[mosi].ulPort].OUTCLR.reg);
  if (cs >= 0) {
    csPinMask = digitalPinToBitMask(cs);
    csPortSet = &(PORT->Group[g_APinDescription[cs].ulPort].OUTSET.reg);
    csPortClr = &(PORT->Group[g_APinDescription[cs].ulPort].OUTCLR.reg);
  } else {
    csPortSet = dcPortSet;
    csPortClr = dcPortClr;
    csPinMask = 0;
  }
  if (miso >= 0) {
    swspi.misoPinMask = digitalPinToBitMask(miso);
    swspi.misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(miso));
  } else {
    swspi.misoPinMask = 0;
    swspi.misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(dc));
  }
#endif 
#else  
  dcPort = (PORTreg_t)portOutputRegister(digitalPinToPort(dc));
  dcPinMaskSet = digitalPinToBitMask(dc);
  swspi.sckPort = (PORTreg_t)portOutputRegister(digitalPinToPort(sck));
  swspi.sckPinMaskSet = digitalPinToBitMask(sck);
  swspi.mosiPort = (PORTreg_t)portOutputRegister(digitalPinToPort(mosi));
  swspi.mosiPinMaskSet = digitalPinToBitMask(mosi);
  if (cs >= 0) {
    csPort = (PORTreg_t)portOutputRegister(digitalPinToPort(cs));
    csPinMaskSet = digitalPinToBitMask(cs);
  } else {
    csPort = dcPort;
    csPinMaskSet = 0;
  }
  if (miso >= 0) {
    swspi.misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(miso));
    swspi.misoPinMask = digitalPinToBitMask(miso);
  } else {
    swspi.misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(dc));
    swspi.misoPinMask = 0;
  }
  csPinMaskClr = ~csPinMaskSet;
  dcPinMaskClr = ~dcPinMaskSet;
  swspi.sckPinMaskClr = ~swspi.sckPinMaskSet;
  swspi.mosiPinMaskClr = ~swspi.mosiPinMaskSet;
#endif 
#endif 
}
Min_Adafruit_SPITFT::Min_Adafruit_SPITFT(uint16_t w, uint16_t h, int8_t cs, int8_t dc,
                                 int8_t rst)
    : Min_Adafruit_SPITFT(w, h, &SPI, cs, dc, rst) {}
Min_Adafruit_SPITFT::Min_Adafruit_SPITFT(uint16_t w, uint16_t h, SPIClass *spiClass,
                                 int8_t cs, int8_t dc, int8_t rst)
    : Min_Adafruit_GFX(w, h), connection(TFT_HARD_SPI), _rst(rst), _cs(cs),
      _dc(dc) {
  hwspi._spi = spiClass;
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(CORE_TEENSY)
#if !defined(KINETISK)
  dcPinMask = digitalPinToBitMask(dc);
#endif
  dcPortSet = portSetRegister(dc);
  dcPortClr = portClearRegister(dc);
  if (cs >= 0) {
#if !defined(KINETISK)
    csPinMask = digitalPinToBitMask(cs);
#endif
    csPortSet = portSetRegister(cs);
    csPortClr = portClearRegister(cs);
  } else { 
#if !defined(KINETISK)
    csPinMask = 0;
#endif
    csPortSet = dcPortSet;
    csPortClr = dcPortClr;
  }
#else  
  dcPinMask = digitalPinToBitMask(dc);
  dcPortSet = &(PORT->Group[g_APinDescription[dc].ulPort].OUTSET.reg);
  dcPortClr = &(PORT->Group[g_APinDescription[dc].ulPort].OUTCLR.reg);
  if (cs >= 0) {
    csPinMask = digitalPinToBitMask(cs);
    csPortSet = &(PORT->Group[g_APinDescription[cs].ulPort].OUTSET.reg);
    csPortClr = &(PORT->Group[g_APinDescription[cs].ulPort].OUTCLR.reg);
  } else {
    csPortSet = dcPortSet;
    csPortClr = dcPortClr;
    csPinMask = 0;
  }
#endif 
#else  
  dcPort = (PORTreg_t)portOutputRegister(digitalPinToPort(dc));
  dcPinMaskSet = digitalPinToBitMask(dc);
  if (cs >= 0) {
    csPort = (PORTreg_t)portOutputRegister(digitalPinToPort(cs));
    csPinMaskSet = digitalPinToBitMask(cs);
  } else {
    csPort = dcPort;
    csPinMaskSet = 0;
  }
  csPinMaskClr = ~csPinMaskSet;
  dcPinMaskClr = ~dcPinMaskSet;
#endif 
#endif 
}
void Min_Adafruit_SPITFT::initSPI(uint32_t freq, uint8_t spiMode) {

  if (!freq)
    freq = DEFAULT_SPI_FREQ; 

  
  if (_cs >= 0) {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); 
  }
  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH); 

  if (connection == TFT_HARD_SPI) {

#if defined(SPI_HAS_TRANSACTION)
    hwspi.settings = SPISettings(freq, MSBFIRST, spiMode);
#else
    hwspi._freq = freq; 
#endif
    hwspi._mode = spiMode;
    if (
#if !defined(SPI_INTERFACES_COUNT)
        1
#else
#if SPI_INTERFACES_COUNT > 0
        (hwspi._spi == &SPI)
#endif
#if SPI_INTERFACES_COUNT > 1
        || (hwspi._spi == &SPI1)
#endif
#if SPI_INTERFACES_COUNT > 2
        || (hwspi._spi == &SPI2)
#endif
#if SPI_INTERFACES_COUNT > 3
        || (hwspi._spi == &SPI3)
#endif
#if SPI_INTERFACES_COUNT > 4
        || (hwspi._spi == &SPI4)
#endif
#if SPI_INTERFACES_COUNT > 5
        || (hwspi._spi == &SPI5)
#endif
#endif 
    ) {
      hwspi._spi->begin();
    }
  } else if (connection == TFT_SOFT_SPI) {

    pinMode(swspi._mosi, OUTPUT);
    digitalWrite(swspi._mosi, LOW);
    pinMode(swspi._sck, OUTPUT);
    digitalWrite(swspi._sck, LOW);
    if (swspi._miso >= 0) {
      pinMode(swspi._miso, INPUT);
    }

  } else {
#if defined(__AVR__)
    for (uint8_t i = 0; i < NUM_DIGITAL_PINS; i++) {
      if ((PORTreg_t)portOutputRegister(digitalPinToPort(i)) ==
          tft8.writePort) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
      }
    }
#elif defined(USE_FAST_PINIO)
#if defined(CORE_TEENSY)
    if (!tft8.wide) {
      *tft8.dirSet = 0xFF;    
      *tft8.writePort = 0x00; 
    } else {
      *(volatile uint16_t *)tft8.dirSet = 0xFFFF;
      *(volatile uint16_t *)tft8.writePort = 0x0000;
    }
#else  
    uint8_t portNum = g_APinDescription[tft8._d0].ulPort, 
        dBit = g_APinDescription[tft8._d0].ulPin,         
        lastBit = dBit + (tft8.wide ? 15 : 7);
    for (uint8_t i = 0; i < PINS_COUNT; i++) {
      if ((g_APinDescription[i].ulPort == portNum) &&
          (g_APinDescription[i].ulPin >= dBit) &&
          (g_APinDescription[i].ulPin <= (uint32_t)lastBit)) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
      }
    }
#endif 
#endif
    pinMode(tft8._wr, OUTPUT);
    digitalWrite(tft8._wr, HIGH);
    if (tft8._rd >= 0) {
      pinMode(tft8._rd, OUTPUT);
      digitalWrite(tft8._rd, HIGH);
    }
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

#if defined(USE_SPI_DMA) && (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
  if (((connection == TFT_HARD_SPI) || (connection == TFT_PARALLEL)) &&
      (dma.allocate() == DMA_STATUS_OK)) {
    if (dptr = dma.addDescriptor(NULL, NULL, 42, DMA_BEAT_SIZE_BYTE, false,
                                 false)) {
      int major = (WIDTH > HEIGHT) ? WIDTH : HEIGHT;
      major += (major & 1);   
      maxFillLen = major * 2;
      if ((pixelBuf[0] = (uint16_t *)malloc(maxFillLen * sizeof(uint16_t)))) {
        pixelBuf[1] = &pixelBuf[0][major];
        int numDescriptors = (WIDTH * HEIGHT + (maxFillLen - 1)) / maxFillLen;
        if ((descriptor = (DmacDescriptor *)memalign(
                 16, numDescriptors * sizeof(DmacDescriptor)))) {
          int dmac_id;
          volatile uint32_t *data_reg;
          if (connection == TFT_HARD_SPI) {
            if (*(SERCOM **)hwspi._spi == &sercom0) {
              dmac_id = SERCOM0_DMAC_ID_TX;
              data_reg = &SERCOM0->SPI.DATA.reg;
#if defined SERCOM1
            } else if (*(SERCOM **)hwspi._spi == &sercom1) {
              dmac_id = SERCOM1_DMAC_ID_TX;
              data_reg = &SERCOM1->SPI.DATA.reg;
#endif
#if defined SERCOM2
            } else if (*(SERCOM **)hwspi._spi == &sercom2) {
              dmac_id = SERCOM2_DMAC_ID_TX;
              data_reg = &SERCOM2->SPI.DATA.reg;
#endif
#if defined SERCOM3
            } else if (*(SERCOM **)hwspi._spi == &sercom3) {
              dmac_id = SERCOM3_DMAC_ID_TX;
              data_reg = &SERCOM3->SPI.DATA.reg;
#endif
#if defined SERCOM4
            } else if (*(SERCOM **)hwspi._spi == &sercom4) {
              dmac_id = SERCOM4_DMAC_ID_TX;
              data_reg = &SERCOM4->SPI.DATA.reg;
#endif
#if defined SERCOM5
            } else if (*(SERCOM **)hwspi._spi == &sercom5) {
              dmac_id = SERCOM5_DMAC_ID_TX;
              data_reg = &SERCOM5->SPI.DATA.reg;
#endif
#if defined SERCOM6
            } else if (*(SERCOM **)hwspi._spi == &sercom6) {
              dmac_id = SERCOM6_DMAC_ID_TX;
              data_reg = &SERCOM6->SPI.DATA.reg;
#endif
#if defined SERCOM7
            } else if (*(SERCOM **)hwspi._spi == &sercom7) {
              dmac_id = SERCOM7_DMAC_ID_TX;
              data_reg = &SERCOM7->SPI.DATA.reg;
#endif
            }
            dma.setPriority(DMA_PRIORITY_3);
            dma.setTrigger(dmac_id);
            dma.setAction(DMA_TRIGGER_ACTON_BEAT);
            for (int d = 0; d < numDescriptors; d++) {
              descriptor[d].BTCTRL.bit.VALID = true;
              descriptor[d].BTCTRL.bit.EVOSEL = DMA_EVENT_OUTPUT_DISABLE;
              descriptor[d].BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_NOACT;
              descriptor[d].BTCTRL.bit.BEATSIZE = DMA_BEAT_SIZE_BYTE;
              descriptor[d].BTCTRL.bit.DSTINC = 0;
              descriptor[d].BTCTRL.bit.STEPSEL = DMA_STEPSEL_SRC;
              descriptor[d].BTCTRL.bit.STEPSIZE =
                  DMA_ADDRESS_INCREMENT_STEP_SIZE_1;
              descriptor[d].DSTADDR.reg = (uint32_t)data_reg;
            }
          } else {
#if defined(__SAMD51__)
            int dmaChannel = dma.getChannel();
            DMAC->Channel[dmaChannel].CHEVCTRL.bit.EVOE = 1;
            DMAC->Channel[dmaChannel].CHEVCTRL.bit.EVOMODE = 0;
            Tc *timer = tcList[tcNum].tc; 
            int id = tcList[tcNum].gclk;  
            GCLK_PCHCTRL_Type pchctrl;
            GCLK->PCHCTRL[id].bit.CHEN = 0; 
            while (GCLK->PCHCTRL[id].bit.CHEN); 
            pchctrl.bit.GEN = GCLK_PCHCTRL_GEN_GCLK0_Val;
            pchctrl.bit.CHEN = 1; 
            GCLK->PCHCTRL[id].reg = pchctrl.reg;
            while (!GCLK->PCHCTRL[id].bit.CHEN);
            timer->COUNT8.CTRLA.bit.ENABLE = 0;
            while (timer->COUNT8.SYNCBUSY.bit.STATUS);
            timer->COUNT8.WAVE.bit.WAVEGEN = 2;    
            timer->COUNT8.CTRLA.bit.MODE = 1;      
            timer->COUNT8.CTRLA.bit.PRESCALER = 0; 
            while (timer->COUNT8.SYNCBUSY.bit.STATUS);
            timer->COUNT8.CTRLBCLR.bit.DIR = 1; 
            while (timer->COUNT8.SYNCBUSY.bit.CTRLB);
            timer->COUNT8.CTRLBSET.bit.ONESHOT = 1; 
            while (timer->COUNT8.SYNCBUSY.bit.CTRLB);
            timer->COUNT8.PER.reg = 6; 
            while (timer->COUNT8.SYNCBUSY.bit.PER);
            timer->COUNT8.CC[0].reg = 2; 
            while (timer->COUNT8.SYNCBUSY.bit.CC0);
            timer->COUNT8.EVCTRL.bit.TCEI = 1;
            timer->COUNT8.EVCTRL.bit.EVACT = 1;
            timer->COUNT8.CTRLA.reg |= TC_CTRLA_ENABLE;
            while (timer->COUNT8.SYNCBUSY.bit.STATUS);
#if (wrPeripheral == PIO_CCL)
            MCLK->APBCMASK.bit.CCL_ = 1;         
            CCL->CTRL.bit.ENABLE = 0;            
            CCL->CTRL.bit.SWRST = 1;             
            CCL->LUTCTRL[tcNum].bit.ENABLE = 0;  
            CCL->LUTCTRL[tcNum].bit.FILTSEL = 0; 
            CCL->LUTCTRL[tcNum].bit.INSEL0 = 6;  
            CCL->LUTCTRL[tcNum].bit.INSEL1 = 0;  
            CCL->LUTCTRL[tcNum].bit.INSEL2 = 0;  
            CCL->LUTCTRL[tcNum].bit.TRUTH = 1;   
            CCL->LUTCTRL[tcNum].bit.ENABLE = 1;  
            CCL->CTRL.bit.ENABLE = 1;            
#endif

            

            
            
            GCLK->PCHCTRL[EVSYS_GCLK_ID_0].bit.CHEN = 0;
            while (GCLK->PCHCTRL[EVSYS_GCLK_ID_0].bit.CHEN)
              ;
            pchctrl.bit.GEN = GCLK_PCHCTRL_GEN_GCLK0_Val;
            pchctrl.bit.CHEN = 1; 
            GCLK->PCHCTRL[EVSYS_GCLK_ID_0].reg = pchctrl.reg;
            
            while (!GCLK->PCHCTRL[EVSYS_GCLK_ID_0].bit.CHEN)
              ;
            MCLK->APBBMASK.bit.EVSYS_ = 1;

            
            EVSYS->USER[tcList[tcNum].evu].reg = 1;
            
            
            
            EVSYS_CHANNEL_Type ev;
            ev.reg = 0;
            ev.bit.PATH = 2;                  
            ev.bit.EVGEN = 0x22 + dmaChannel; 
            EVSYS->Channel[0].CHANNEL.reg = ev.reg;

            
            for (int d = 0; d < numDescriptors; d++) {
              
              
              descriptor[d].BTCTRL.bit.VALID = true;
              
              descriptor[d].BTCTRL.bit.EVOSEL = 0x3;
              descriptor[d].BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_NOACT;
              descriptor[d].BTCTRL.bit.BEATSIZE =
                  tft8.wide ? DMA_BEAT_SIZE_HWORD : DMA_BEAT_SIZE_BYTE;
              descriptor[d].BTCTRL.bit.SRCINC = 1;
              descriptor[d].BTCTRL.bit.DSTINC = 0;
              descriptor[d].BTCTRL.bit.STEPSEL = DMA_STEPSEL_SRC;
              descriptor[d].BTCTRL.bit.STEPSIZE =
                  DMA_ADDRESS_INCREMENT_STEP_SIZE_1;
              descriptor[d].DSTADDR.reg = (uint32_t)tft8.writePort;
            }
#endif 
          } 

          lastFillColor = 0x0000;
          lastFillLen = 0;
          dma.setCallback(dma_callback);
          return; 
                  
        } 
        free(pixelBuf[0]);
        pixelBuf[0] = pixelBuf[1] = NULL;
      } 
        
        
    } 
    dma.free(); 
  }
#endif 
}
void Min_Adafruit_SPITFT::setSPISpeed(uint32_t freq) {
#if defined(SPI_HAS_TRANSACTION)
  hwspi.settings = SPISettings(freq, MSBFIRST, hwspi._mode);
#else
  hwspi._freq = freq; 
#endif
}
void Min_Adafruit_SPITFT::startWrite(void) {
  SPI_BEGIN_TRANSACTION();
  if (_cs >= 0)
    SPI_CS_LOW();
}
void Min_Adafruit_SPITFT::endWrite(void) {
  if (_cs >= 0)
    SPI_CS_HIGH();
  SPI_END_TRANSACTION();
}

void Min_Adafruit_SPITFT::writePixel(int16_t x, int16_t y, uint16_t color) {
  if ((x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
    setAddrWindow(x, y, 1, 1);
    SPI_WRITE16(color);
  }
}
void Min_Adafruit_SPITFT::swapBytes(uint16_t *src, uint32_t len, uint16_t *dest) {
  if (!dest)
    dest = src; 
  for (uint32_t i = 0; i < len; i++) {
    dest[i] = __builtin_bswap16(src[i]);
  }
}
void Min_Adafruit_SPITFT::writePixels(uint16_t *colors, uint32_t len, bool block,
                                  bool bigEndian) {

  if (!len)
    return; 

  
  (void)block;
  (void)bigEndian;

#if defined(ESP32)
  if (connection == TFT_HARD_SPI) {
    if (!bigEndian) {
      hwspi._spi->writePixels(colors, len * 2); 
    } else {
      hwspi._spi->writeBytes((uint8_t *)colors, len * 2); 
    }
    return;
  }
#elif defined(ARDUINO_NRF52_ADAFRUIT) &&                                       \
    defined(NRF52840_XXAA) 
  if (!bigEndian) {
    swapBytes(colors, len); 
  }
  hwspi._spi->transfer(colors, NULL, 2 * len); 
  if (!bigEndian) {
    swapBytes(colors, len); 
  }

  return;
#elif defined(ARDUINO_ARCH_RP2040)
  spi_inst_t *pi_spi = hwspi._spi == &SPI ? __SPI0_DEVICE : __SPI1_DEVICE;

  if (!bigEndian) {
    
    hw_write_masked(&spi_get_hw(pi_spi)->cr0, 15 << SPI_SSPCR0_DSS_LSB,
                    SPI_SSPCR0_DSS_BITS);
    spi_write16_blocking(pi_spi, colors, len);
    
    hw_write_masked(&spi_get_hw(pi_spi)->cr0, 7 << SPI_SSPCR0_DSS_LSB,
                    SPI_SSPCR0_DSS_BITS);
  } else {
    spi_write_blocking(pi_spi, (uint8_t *)colors, len * 2);
  }
  return;
#elif defined(ARDUINO_ARCH_RTTHREAD)
  if (!bigEndian) {
    swapBytes(colors, len); 
  }
  hwspi._spi->transfer(colors, 2 * len);
  if (!bigEndian) {
    swapBytes(colors, len); 
  }
  return;
#elif defined(USE_SPI_DMA) &&                                                  \
    (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
  if ((connection == TFT_HARD_SPI) || (connection == TFT_PARALLEL)) {
    int maxSpan = maxFillLen / 2; 
    uint8_t pixelBufIdx = 0;      
#if defined(__SAMD51__)
    if (connection == TFT_PARALLEL) {
      
      pinPeripheral(tft8._wr, wrPeripheral);
    }
#endif 
    if (!bigEndian) { 
      while (len) {
        int count = (len < maxSpan) ? len : maxSpan;

        
        
        
        
        swapBytes(colors, count, pixelBuf[pixelBufIdx]);
        colors += count;

        
        
        
        descriptor[pixelBufIdx].SRCADDR.reg =
            (uint32_t)pixelBuf[pixelBufIdx] + count * 2;
        descriptor[pixelBufIdx].BTCTRL.bit.SRCINC = 1;
        descriptor[pixelBufIdx].BTCNT.reg = count * 2;
        descriptor[pixelBufIdx].DESCADDR.reg = 0;

        while (dma_busy)
          ; 

        
        memcpy(dptr, &descriptor[pixelBufIdx], sizeof(DmacDescriptor));
        dma_busy = true;
        dma.startJob(); 
        if (connection == TFT_PARALLEL)
          dma.trigger();
        pixelBufIdx = 1 - pixelBufIdx; 

        len -= count;
      }
    } else { 
      
      
      
      
      
      
      int d, numDescriptors = (len + 32766) / 32767;
      for (d = 0; d < numDescriptors; d++) {
        int count = (len < 32767) ? len : 32767;
        descriptor[d].SRCADDR.reg = (uint32_t)colors + count * 2;
        descriptor[d].BTCTRL.bit.SRCINC = 1;
        descriptor[d].BTCNT.reg = count * 2;
        descriptor[d].DESCADDR.reg = (uint32_t)&descriptor[d + 1];
        len -= count;
        colors += count;
      }
      descriptor[d - 1].DESCADDR.reg = 0;

      while (dma_busy)
        ; 

      
      memcpy(dptr, &descriptor[0], sizeof(DmacDescriptor));
      dma_busy = true;
      dma.startJob(); 
      if (connection == TFT_PARALLEL)
        dma.trigger();
    } 

    lastFillColor = 0x0000; 
    lastFillLen = 0;
    if (block) {
      while (dma_busy)
        ; 
#if defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO)
      if (connection == TFT_HARD_SPI) {
        
        hwspi._spi->setDataMode(hwspi._mode);
      } else {
        pinPeripheral(tft8._wr, PIO_OUTPUT); 
      }
#endif 
    }
    return;
  }
#endif 

  
  

  if (!bigEndian) {
    while (len--) {
      SPI_WRITE16(*colors++);
    }
  } else {
    
    
    
    
    
    
    
    while (len--) {
      SPI_WRITE16(__builtin_bswap16(*colors++));
    }
  }
}
void Min_Adafruit_SPITFT::dmaWait(void) {
#if defined(USE_SPI_DMA) && (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
  while (dma_busy)
    ;
#if defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO)
  if (connection == TFT_HARD_SPI) {
    
    hwspi._spi->setDataMode(hwspi._mode);
  } else {
    pinPeripheral(tft8._wr, PIO_OUTPUT); 
  }
#endif 
#endif
}
bool Min_Adafruit_SPITFT::dmaBusy(void) const {
#if defined(USE_SPI_DMA) && (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
  return dma_busy;
#else
  return false;
#endif
}
void Min_Adafruit_SPITFT::writeColor(uint16_t color, uint32_t len) {

  if (!len)
    return; 

  uint8_t hi = color >> 8, lo = color;

#if defined(ESP32) 
  if (connection == TFT_HARD_SPI) {
#define SPI_MAX_PIXELS_AT_ONCE 32
#define TMPBUF_LONGWORDS (SPI_MAX_PIXELS_AT_ONCE + 1) / 2
#define TMPBUF_PIXELS (TMPBUF_LONGWORDS * 2)
    static uint32_t temp[TMPBUF_LONGWORDS];
    uint32_t c32 = color * 0x00010001;
    uint16_t bufLen = (len < TMPBUF_PIXELS) ? len : TMPBUF_PIXELS, xferLen,
             fillLen;
    
    fillLen = (bufLen + 1) / 2; 
    for (uint32_t t = 0; t < fillLen; t++) {
      temp[t] = c32;
    }
    
    while (len) {                              
      xferLen = (bufLen < len) ? bufLen : len; 
      writePixels((uint16_t *)temp, xferLen);
      len -= xferLen;
    }
    return;
  }
#elif defined(ARDUINO_NRF52_ADAFRUIT) &&                                       \
    defined(NRF52840_XXAA) 
  
  uint32_t const pixbufcount = min(len, ((uint32_t)2 * width()));
  uint16_t *pixbuf = (uint16_t *)rtos_malloc(2 * pixbufcount);

  
  
  if (pixbuf) {
    uint16_t const swap_color = __builtin_bswap16(color);

    
    for (uint32_t i = 0; i < pixbufcount; i++) {
      pixbuf[i] = swap_color;
    }

    while (len) {
      uint32_t const count = min(len, pixbufcount);
      writePixels(pixbuf, count, true, true);
      len -= count;
    }

    rtos_free(pixbuf);
    return;
  }
#elif defined(ARDUINO_ARCH_RTTHREAD)
  uint16_t pixbufcount;
  uint16_t *pixbuf;
  int16_t lines = height() / 4;
#define QUICKPATH_MAX_LEN 16
  uint16_t quickpath_buffer[QUICKPATH_MAX_LEN];

  do {
    pixbufcount = min(len, (lines * width()));
    if (pixbufcount > QUICKPATH_MAX_LEN) {
      pixbuf = (uint16_t *)rt_malloc(2 * pixbufcount);
    } else {
      pixbuf = quickpath_buffer;
    }
    lines -= 2;
  } while (!pixbuf && lines > 0);

  if (pixbuf) {
    uint16_t const swap_color = __builtin_bswap16(color);

    while (len) {
      uint16_t count = min(len, pixbufcount);
      
      for (uint16_t i = 0; i < count; i++) {
        pixbuf[i] = swap_color;
      }
      
      
      writePixels(pixbuf, count, true, true);
      len -= count;
    }
    if (pixbufcount > QUICKPATH_MAX_LEN) {
      rt_free(pixbuf);
    }
#undef QUICKPATH_MAX_LEN
    return;
  }
#else 
#if defined(USE_SPI_DMA) && (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
  if (((connection == TFT_HARD_SPI) || (connection == TFT_PARALLEL)) &&
      (len >= 16)) { 
    int i, d, numDescriptors;
    if (hi == lo) { 
      onePixelBuf = color;
      
      
      
      
      numDescriptors = (len + 32766) / 32767;
      for (d = 0; d < numDescriptors; d++) {
        int count = (len < 32767) ? len : 32767;
        descriptor[d].SRCADDR.reg = (uint32_t)&onePixelBuf;
        descriptor[d].BTCTRL.bit.SRCINC = 0;
        descriptor[d].BTCNT.reg = count * 2;
        descriptor[d].DESCADDR.reg = (uint32_t)&descriptor[d + 1];
        len -= count;
      }
      descriptor[d - 1].DESCADDR.reg = 0;
    } else {
      
      
      
      
      
      uint32_t *pixelPtr = (uint32_t *)pixelBuf[0],
               twoPixels = __builtin_bswap16(color) * 0x00010001;
      
      
      if (color == lastFillColor) {
        
        
        if (len > lastFillLen) {
          int fillStart = lastFillLen / 2,
              fillEnd = (((len < maxFillLen) ? len : maxFillLen) + 1) / 2;
          for (i = fillStart; i < fillEnd; i++)
            pixelPtr[i] = twoPixels;
          lastFillLen = fillEnd * 2;
        } 
      } else {
        int fillEnd = (((len < maxFillLen) ? len : maxFillLen) + 1) / 2;
        for (i = 0; i < fillEnd; i++)
          pixelPtr[i] = twoPixels;
        lastFillLen = fillEnd * 2;
        lastFillColor = color;
      }

      numDescriptors = (len + maxFillLen - 1) / maxFillLen;
      for (d = 0; d < numDescriptors; d++) {
        int pixels = (len < maxFillLen) ? len : maxFillLen, bytes = pixels * 2;
        descriptor[d].SRCADDR.reg = (uint32_t)pixelPtr + bytes;
        descriptor[d].BTCTRL.bit.SRCINC = 1;
        descriptor[d].BTCNT.reg = bytes;
        descriptor[d].DESCADDR.reg = (uint32_t)&descriptor[d + 1];
        len -= pixels;
      }
      descriptor[d - 1].DESCADDR.reg = 0;
    }
    memcpy(dptr, &descriptor[0], sizeof(DmacDescriptor));
#if defined(__SAMD51__)
    if (connection == TFT_PARALLEL) {
      
      pinPeripheral(tft8._wr, wrPeripheral);
    }
#endif 

    dma_busy = true;
    dma.startJob();
    if (connection == TFT_PARALLEL)
      dma.trigger();
    while (dma_busy)
      ; 
#if defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO)
    if (connection == TFT_HARD_SPI) {
      hwspi._spi->setDataMode(hwspi._mode);
    } else {
      pinPeripheral(tft8._wr, PIO_OUTPUT); 
    }
#endif 
    return;
  }
#endif 
#endif 
  if (connection == TFT_HARD_SPI) {
#if defined(ESP8266)
    do {
      uint32_t pixelsThisPass = len;
      if (pixelsThisPass > 50000)
        pixelsThisPass = 50000;
      len -= pixelsThisPass;
      delay(1); 
      while (pixelsThisPass--) {
        hwspi._spi->write(hi);
        hwspi._spi->write(lo);
      }
    } while (len);
#elif defined(ARDUINO_ARCH_RP2040)
    spi_inst_t *pi_spi = hwspi._spi == &SPI ? __SPI0_DEVICE : __SPI1_DEVICE;
    color = __builtin_bswap16(color);

    while (len--)
      spi_write_blocking(pi_spi, (uint8_t *)&color, 2);
#else 
    while (len--) {
#if defined(__AVR__)
      AVR_WRITESPI(hi);
      AVR_WRITESPI(lo);
#elif defined(ESP32)
      hwspi._spi->write(hi);
      hwspi._spi->write(lo);
#else
      hwspi._spi->transfer(hi);
      hwspi._spi->transfer(lo);
#endif
    }
#endif 
  } else if (connection == TFT_SOFT_SPI) {
#if defined(ESP8266)
    do {
      uint32_t pixelsThisPass = len;
      if (pixelsThisPass > 20000)
        pixelsThisPass = 20000;
      len -= pixelsThisPass;
      yield(); 
      while (pixelsThisPass--) {
        for (uint16_t bit = 0, x = color; bit < 16; bit++) {
          if (x & 0x8000)
            SPI_MOSI_HIGH();
          else
            SPI_MOSI_LOW();
          SPI_SCK_HIGH();
          SPI_SCK_LOW();
          x <<= 1;
        }
      }
    } while (len);
#else 
    while (len--) {
#if defined(__AVR__)
      for (uint8_t bit = 0, x = hi; bit < 8; bit++) {
        if (x & 0x80)
          SPI_MOSI_HIGH();
        else
          SPI_MOSI_LOW();
        SPI_SCK_HIGH();
        SPI_SCK_LOW();
        x <<= 1;
      }
      for (uint8_t bit = 0, x = lo; bit < 8; bit++) {
        if (x & 0x80)
          SPI_MOSI_HIGH();
        else
          SPI_MOSI_LOW();
        SPI_SCK_HIGH();
        SPI_SCK_LOW();
        x <<= 1;
      }
#else      
      for (uint16_t bit = 0, x = color; bit < 16; bit++) {
        if (x & 0x8000)
          SPI_MOSI_HIGH();
        else
          SPI_MOSI_LOW();
        SPI_SCK_HIGH();
        x <<= 1;
        SPI_SCK_LOW();
      }
#endif     
    }
#endif     
  } else { 
    if (hi == lo) {
#if defined(__AVR__)
      len *= 2;
      *tft8.writePort = hi;
      while (len--) {
        TFT_WR_STROBE();
      }
#elif defined(USE_FAST_PINIO)
      if (!tft8.wide) {
        len *= 2;
        *tft8.writePort = hi;
      } else {
        *(volatile uint16_t *)tft8.writePort = color;
      }
      while (len--) {
        TFT_WR_STROBE();
      }
#endif
    } else {
      while (len--) {
#if defined(__AVR__)
        *tft8.writePort = hi;
        TFT_WR_STROBE();
        *tft8.writePort = lo;
#elif defined(USE_FAST_PINIO)
        if (!tft8.wide) {
          *tft8.writePort = hi;
          TFT_WR_STROBE();
          *tft8.writePort = lo;
        } else {
          *(volatile uint16_t *)tft8.writePort = color;
        }
#endif
        TFT_WR_STROBE();
      }
    }
  }
}
void Min_Adafruit_SPITFT::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                    uint16_t color) {
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
            writeFillRectPreclipped(x, y, w, h, color);
          }
        }
      }
    }
  }
}
void inline Min_Adafruit_SPITFT::writeFastHLine(int16_t x, int16_t y, int16_t w,
                                            uint16_t color) {
  if ((y >= 0) && (y < _height) && w) { 
    if (w < 0) {                        
      x += w + 1;                       
      w = -w;                           
    }
    if (x < _width) { 
      int16_t x2 = x + w - 1;
      if (x2 >= 0) { 
        
        if (x < 0) {
          x = 0;
          w = x2 + 1;
        } 
        if (x2 >= _width) {
          w = _width - x;
        } 
        writeFillRectPreclipped(x, y, w, 1, color);
      }
    }
  }
}
void inline Min_Adafruit_SPITFT::writeFastVLine(int16_t x, int16_t y, int16_t h,
                                            uint16_t color) {
  if ((x >= 0) && (x < _width) && h) { 
    if (h < 0) {                       
      y += h + 1;                      
      h = -h;                          
    }
    if (y < _height) { 
      int16_t y2 = y + h - 1;
      if (y2 >= 0) { 
        
        if (y < 0) {
          y = 0;
          h = y2 + 1;
        } 
        if (y2 >= _height) {
          h = _height - y;
        } 
        writeFillRectPreclipped(x, y, 1, h, color);
      }
    }
  }
}
inline void Min_Adafruit_SPITFT::writeFillRectPreclipped(int16_t x, int16_t y,
                                                     int16_t w, int16_t h,
                                                     uint16_t color) {
  setAddrWindow(x, y, w, h);
  writeColor(color, (uint32_t)w * h);
}

void Min_Adafruit_SPITFT::drawPixel(int16_t x, int16_t y, uint16_t color) {
  
  if ((x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
    
    startWrite();
    setAddrWindow(x, y, 1, 1);
    SPI_WRITE16(color);
    endWrite();
  }
}
void Min_Adafruit_SPITFT::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                               uint16_t color) {
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
void Min_Adafruit_SPITFT::drawFastHLine(int16_t x, int16_t y, int16_t w,
                                    uint16_t color) {
  if ((y >= 0) && (y < _height) && w) { 
    if (w < 0) {                        
      x += w + 1;                       
      w = -w;                           
    }
    if (x < _width) { 
      int16_t x2 = x + w - 1;
      if (x2 >= 0) { 
        
        if (x < 0) {
          x = 0;
          w = x2 + 1;
        } 
        if (x2 >= _width) {
          w = _width - x;
        } 
        startWrite();
        writeFillRectPreclipped(x, y, w, 1, color);
        endWrite();
      }
    }
  }
}
void Min_Adafruit_SPITFT::drawFastVLine(int16_t x, int16_t y, int16_t h,
                                    uint16_t color) {
  if ((x >= 0) && (x < _width) && h) { 
    if (h < 0) {                       
      y += h + 1;                      
      h = -h;                          
    }
    if (y < _height) { 
      int16_t y2 = y + h - 1;
      if (y2 >= 0) { 
        
        if (y < 0) {
          y = 0;
          h = y2 + 1;
        } 
        if (y2 >= _height) {
          h = _height - y;
        } 
        startWrite();
        writeFillRectPreclipped(x, y, 1, h, color);
        endWrite();
      }
    }
  }
}
void Min_Adafruit_SPITFT::pushColor(uint16_t color) {
  startWrite();
  SPI_WRITE16(color);
  endWrite();
}

void Min_Adafruit_SPITFT::invertDisplay(bool i) {
  startWrite();
  writeCommand(i ? invertOnCommand : invertOffCommand);
  endWrite();
}
uint16_t Min_Adafruit_SPITFT::color565(uint8_t red, uint8_t green, uint8_t blue) {
  return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}
void Min_Adafruit_SPITFT::sendCommand(uint8_t commandByte, uint8_t *dataBytes,
                                  uint8_t numDataBytes) {
  SPI_BEGIN_TRANSACTION();
  if (_cs >= 0)
    SPI_CS_LOW();

  SPI_DC_LOW();          
  spiWrite(commandByte); 

  SPI_DC_HIGH();
  for (int i = 0; i < numDataBytes; i++) {
    if ((connection == TFT_PARALLEL) && tft8.wide) {
      SPI_WRITE16(*(uint16_t *)dataBytes);
      dataBytes += 2;
    } else {
      spiWrite(*dataBytes); 
      dataBytes++;
    }
  }

  if (_cs >= 0)
    SPI_CS_HIGH();
  SPI_END_TRANSACTION();
}
void Min_Adafruit_SPITFT::sendCommand(uint8_t commandByte, const uint8_t *dataBytes,
                                  uint8_t numDataBytes) {
  SPI_BEGIN_TRANSACTION();
  if (_cs >= 0)
    SPI_CS_LOW();

  SPI_DC_LOW();          
  spiWrite(commandByte); 

  SPI_DC_HIGH();
  for (int i = 0; i < numDataBytes; i++) {
    if ((connection == TFT_PARALLEL) && tft8.wide) {
      SPI_WRITE16(*(uint16_t *)dataBytes);
      dataBytes += 2;
    } else {
      spiWrite(pgm_read_byte(dataBytes++));
    }
  }

  if (_cs >= 0)
    SPI_CS_HIGH();
  SPI_END_TRANSACTION();
}
void Min_Adafruit_SPITFT::sendCommand16(uint16_t commandWord,
                                    const uint8_t *dataBytes,
                                    uint8_t numDataBytes) {
  SPI_BEGIN_TRANSACTION();
  if (_cs >= 0)
    SPI_CS_LOW();

  if (numDataBytes == 0) {
    SPI_DC_LOW();             
    SPI_WRITE16(commandWord); 
    SPI_DC_HIGH();            
  }
  for (int i = 0; i < numDataBytes; i++) {
    SPI_DC_LOW();             
    SPI_WRITE16(commandWord); 
    SPI_DC_HIGH();            
    commandWord++;
    SPI_WRITE16((uint16_t)pgm_read_byte(dataBytes++));
  }

  if (_cs >= 0)
    SPI_CS_HIGH();
  SPI_END_TRANSACTION();
}

uint8_t Min_Adafruit_SPITFT::readcommand8(uint8_t commandByte, uint8_t index) {
  uint8_t result;
  startWrite();
  SPI_DC_LOW(); 
  spiWrite(commandByte);
  SPI_DC_HIGH(); 
  do {
    result = spiRead();
  } while (index--); 
  endWrite();
  return result;
}
uint16_t Min_Adafruit_SPITFT::readcommand16(uint16_t addr) {
#if defined(USE_FAST_PINIO) 
  uint16_t result = 0;
  if ((connection == TFT_PARALLEL) && tft8.wide) {
    startWrite();
    SPI_DC_LOW(); 
    SPI_WRITE16(addr);
    SPI_DC_HIGH(); 
    TFT_RD_LOW();  
#if defined(HAS_PORT_SET_CLR)
    *(volatile uint16_t *)tft8.dirClr = 0xFFFF;   
    result = *(volatile uint16_t *)tft8.readPort; 
    *(volatile uint16_t *)tft8.dirSet = 0xFFFF;   
#else                                             
    *(volatile uint16_t *)tft8.portDir = 0x0000;  
    result = *(volatile uint16_t *)tft8.readPort; 
    *(volatile uint16_t *)tft8.portDir = 0xFFFF;  
#endif                                            
    TFT_RD_HIGH();                                
    endWrite();
  }
  return result;
#else
  (void)addr; 
  return 0;
#endif 
}
inline void Min_Adafruit_SPITFT::SPI_BEGIN_TRANSACTION(void) {
  if (connection == TFT_HARD_SPI) {
#if defined(SPI_HAS_TRANSACTION)
    hwspi._spi->beginTransaction(hwspi.settings);
#else 
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ARDUINO_ARCH_STM32F1)
    hwspi._spi->setClockDivider(SPI_CLOCK_DIV2);
#elif defined(__arm__)
    hwspi._spi->setClockDivider(11);
#elif defined(ESP8266) || defined(ESP32)
    hwspi._spi->setFrequency(hwspi._freq);
#elif defined(RASPI) || defined(ARDUINO_ARCH_STM32F1)
    hwspi._spi->setClock(hwspi._freq);
#endif
    hwspi._spi->setBitOrder(MSBFIRST);
    hwspi._spi->setDataMode(hwspi._mode);
#endif 
  }
}
inline void Min_Adafruit_SPITFT::SPI_END_TRANSACTION(void) {
#if defined(SPI_HAS_TRANSACTION)
  if (connection == TFT_HARD_SPI) {
    hwspi._spi->endTransaction();
  }
#endif
}
void Min_Adafruit_SPITFT::spiWrite(uint8_t b) {
  if (connection == TFT_HARD_SPI) {
#if defined(__AVR__)
    AVR_WRITESPI(b);
#elif defined(ESP8266) || defined(ESP32)
    hwspi._spi->write(b);
#elif defined(ARDUINO_ARCH_RP2040)
    spi_inst_t *pi_spi = hwspi._spi == &SPI ? __SPI0_DEVICE : __SPI1_DEVICE;
    spi_write_blocking(pi_spi, &b, 1);
#else
    hwspi._spi->transfer(b);
#endif
  } else if (connection == TFT_SOFT_SPI) {
    for (uint8_t bit = 0; bit < 8; bit++) {
      if (b & 0x80)
        SPI_MOSI_HIGH();
      else
        SPI_MOSI_LOW();
      SPI_SCK_HIGH();
      b <<= 1;
      SPI_SCK_LOW();
    }
  } else { 
#if defined(__AVR__)
    *tft8.writePort = b;
#elif defined(USE_FAST_PINIO)
    if (!tft8.wide)
      *tft8.writePort = b;
    else
      *(volatile uint16_t *)tft8.writePort = b;
#endif
    TFT_WR_STROBE();
  }
}
void Min_Adafruit_SPITFT::writeCommand(uint8_t cmd) {
  SPI_DC_LOW();
  spiWrite(cmd);
  SPI_DC_HIGH();
}
uint8_t Min_Adafruit_SPITFT::spiRead(void) {
  uint8_t b = 0;
  uint16_t w = 0;
  if (connection == TFT_HARD_SPI) {
    return hwspi._spi->transfer((uint8_t)0);
  } else if (connection == TFT_SOFT_SPI) {
    if (swspi._miso >= 0) {
      for (uint8_t i = 0; i < 8; i++) {
        SPI_SCK_HIGH();
        b <<= 1;
        if (SPI_MISO_READ())
          b++;
        SPI_SCK_LOW();
      }
    }
    return b;
  } else { 
    if (tft8._rd >= 0) {
#if defined(USE_FAST_PINIO)
      TFT_RD_LOW(); 
#if defined(__AVR__)
      *tft8.portDir = 0x00; 
      w = *tft8.readPort;   
      *tft8.portDir = 0xFF; 
#else                       
      if (!tft8.wide) { 
#if defined(HAS_PORT_SET_CLR)
        *tft8.dirClr = 0xFF; 
        w = *tft8.readPort;  
        *tft8.dirSet = 0xFF; 
#else  
        *tft8.portDir = 0x00; 
        w = *tft8.readPort;   
        *tft8.portDir = 0xFF; 
#endif 
      } else { 
#if defined(HAS_PORT_SET_CLR)
        *(volatile uint16_t *)tft8.dirClr = 0xFFFF; 
        w = *(volatile uint16_t *)tft8.readPort;    
        *(volatile uint16_t *)tft8.dirSet = 0xFFFF; 
#else  
        *(volatile uint16_t *)tft8.portDir = 0x0000; 
        w = *(volatile uint16_t *)tft8.readPort;     
        *(volatile uint16_t *)tft8.portDir = 0xFFFF; 
#endif 
      }
      TFT_RD_HIGH(); 
#endif 
#else  
      w = 0; 
#endif 
    }
    return w;
  }
}
void Min_Adafruit_SPITFT::write16(uint16_t w) {
  if (connection == TFT_PARALLEL) {
#if defined(USE_FAST_PINIO)
    if (tft8.wide)
      *(volatile uint16_t *)tft8.writePort = w;
#else
    (void)w; 
#endif
    TFT_WR_STROBE();
  }
}
void Min_Adafruit_SPITFT::writeCommand16(uint16_t cmd) {
  SPI_DC_LOW();
  write16(cmd);
  SPI_DC_HIGH();
}
uint16_t Min_Adafruit_SPITFT::read16(void) {
  uint16_t w = 0;
  if (connection == TFT_PARALLEL) {
    if (tft8._rd >= 0) {
#if defined(USE_FAST_PINIO)
      TFT_RD_LOW();    
      if (tft8.wide) { 
#if defined(HAS_PORT_SET_CLR)
        *(volatile uint16_t *)tft8.dirClr = 0xFFFF; 
        w = *(volatile uint16_t *)tft8.readPort;    
        *(volatile uint16_t *)tft8.dirSet = 0xFFFF; 
#else                                               
        *(volatile uint16_t *)tft8.portDir = 0x0000; 
        w = *(volatile uint16_t *)tft8.readPort;     
        *(volatile uint16_t *)tft8.portDir = 0xFFFF; 
#endif                                              
      }
      TFT_RD_HIGH(); 
#else                
      w = 0; 
#endif               
    }
  }
  return w;
}
inline void Min_Adafruit_SPITFT::SPI_MOSI_HIGH(void) {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *swspi.mosiPortSet = 1;
#else 
  *swspi.mosiPortSet = swspi.mosiPinMask;
#endif
#else  
  *swspi.mosiPort |= swspi.mosiPinMaskSet;
#endif 
#else  
  digitalWrite(swspi._mosi, HIGH);
#endif 
}
inline void Min_Adafruit_SPITFT::SPI_MOSI_LOW(void) {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *swspi.mosiPortClr = 1;
#else 
  *swspi.mosiPortClr = swspi.mosiPinMask;
#endif
#else  
  *swspi.mosiPort &= swspi.mosiPinMaskClr;
#endif 
#else  
  digitalWrite(swspi._mosi, LOW);
#endif 
}
inline void Min_Adafruit_SPITFT::SPI_SCK_HIGH(void) {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *swspi.sckPortSet = 1;
#else 
  *swspi.sckPortSet = swspi.sckPinMask;
#endif
#else  
  *swspi.sckPort |= swspi.sckPinMaskSet;
#endif 
#else  
  digitalWrite(swspi._sck, HIGH);
#endif 
}
inline void Min_Adafruit_SPITFT::SPI_SCK_LOW(void) {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *swspi.sckPortClr = 1;
#else 
  *swspi.sckPortClr = swspi.sckPinMask;
#endif
#else  
  *swspi.sckPort &= swspi.sckPinMaskClr;
#endif 
#else  
  digitalWrite(swspi._sck, LOW);
#endif 
}
inline bool Min_Adafruit_SPITFT::SPI_MISO_READ(void) {
#if defined(USE_FAST_PINIO)
#if defined(KINETISK)
  return *swspi.misoPort;
#else  
  return *swspi.misoPort & swspi.misoPinMask;
#endif 
#else  
  return digitalRead(swspi._miso);
#endif 
}
void Min_Adafruit_SPITFT::SPI_WRITE16(uint16_t w) {
  if (connection == TFT_HARD_SPI) {
#if defined(__AVR__)
    AVR_WRITESPI(w >> 8);
    AVR_WRITESPI(w);
#elif defined(ESP8266) || defined(ESP32)
    hwspi._spi->write16(w);
#elif defined(ARDUINO_ARCH_RP2040)
    spi_inst_t *pi_spi = hwspi._spi == &SPI ? __SPI0_DEVICE : __SPI1_DEVICE;
    w = __builtin_bswap16(w);
    spi_write_blocking(pi_spi, (uint8_t *)&w, 2);
#elif defined(ARDUINO_ARCH_RTTHREAD)
    hwspi._spi->transfer16(w);
#else
    
    hwspi._spi->transfer(w >> 8);
    hwspi._spi->transfer(w);
#endif
  } else if (connection == TFT_SOFT_SPI) {
    for (uint8_t bit = 0; bit < 16; bit++) {
      if (w & 0x8000)
        SPI_MOSI_HIGH();
      else
        SPI_MOSI_LOW();
      SPI_SCK_HIGH();
      SPI_SCK_LOW();
      w <<= 1;
    }
  } else { 
#if defined(__AVR__)
    *tft8.writePort = w >> 8;
    TFT_WR_STROBE();
    *tft8.writePort = w;
#elif defined(USE_FAST_PINIO)
    if (!tft8.wide) {
      *tft8.writePort = w >> 8;
      TFT_WR_STROBE();
      *tft8.writePort = w;
    } else {
      *(volatile uint16_t *)tft8.writePort = w;
    }
#endif
    TFT_WR_STROBE();
  }
}
void Min_Adafruit_SPITFT::SPI_WRITE32(uint32_t l) {
  if (connection == TFT_HARD_SPI) {
#if defined(__AVR__)
    AVR_WRITESPI(l >> 24);
    AVR_WRITESPI(l >> 16);
    AVR_WRITESPI(l >> 8);
    AVR_WRITESPI(l);
#elif defined(ESP8266) || defined(ESP32)
    hwspi._spi->write32(l);
#elif defined(ARDUINO_ARCH_RP2040)
    spi_inst_t *pi_spi = hwspi._spi == &SPI ? __SPI0_DEVICE : __SPI1_DEVICE;
    l = __builtin_bswap32(l);
    spi_write_blocking(pi_spi, (uint8_t *)&l, 4);
#elif defined(ARDUINO_ARCH_RTTHREAD)
    hwspi._spi->transfer16(l >> 16);
    hwspi._spi->transfer16(l);
#else
    hwspi._spi->transfer(l >> 24);
    hwspi._spi->transfer(l >> 16);
    hwspi._spi->transfer(l >> 8);
    hwspi._spi->transfer(l);
#endif
  } else if (connection == TFT_SOFT_SPI) {
    for (uint8_t bit = 0; bit < 32; bit++) {
      if (l & 0x80000000)
        SPI_MOSI_HIGH();
      else
        SPI_MOSI_LOW();
      SPI_SCK_HIGH();
      SPI_SCK_LOW();
      l <<= 1;
    }
  } else { 
#if defined(__AVR__)
    *tft8.writePort = l >> 24;
    TFT_WR_STROBE();
    *tft8.writePort = l >> 16;
    TFT_WR_STROBE();
    *tft8.writePort = l >> 8;
    TFT_WR_STROBE();
    *tft8.writePort = l;
#elif defined(USE_FAST_PINIO)
    if (!tft8.wide) {
      *tft8.writePort = l >> 24;
      TFT_WR_STROBE();
      *tft8.writePort = l >> 16;
      TFT_WR_STROBE();
      *tft8.writePort = l >> 8;
      TFT_WR_STROBE();
      *tft8.writePort = l;
    } else {
      *(volatile uint16_t *)tft8.writePort = l >> 16;
      TFT_WR_STROBE();
      *(volatile uint16_t *)tft8.writePort = l;
    }
#endif
    TFT_WR_STROBE();
  }
}
inline void Min_Adafruit_SPITFT::TFT_WR_STROBE(void) {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *tft8.wrPortClr = 1;
  *tft8.wrPortSet = 1;
#else  
  *tft8.wrPortClr = tft8.wrPinMask;
  *tft8.wrPortSet = tft8.wrPinMask;
#endif 
#else  
  *tft8.wrPort &= tft8.wrPinMaskClr;
  *tft8.wrPort |= tft8.wrPinMaskSet;
#endif 
#else  
  digitalWrite(tft8._wr, LOW);
  digitalWrite(tft8._wr, HIGH);
#endif 
}
inline void Min_Adafruit_SPITFT::TFT_RD_HIGH(void) {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
  *tft8.rdPortSet = tft8.rdPinMask;
#else  
  *tft8.rdPort |= tft8.rdPinMaskSet;
#endif 
#else  
  digitalWrite(tft8._rd, HIGH);
#endif 
}
inline void Min_Adafruit_SPITFT::TFT_RD_LOW(void) {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
  *tft8.rdPortClr = tft8.rdPinMask;
#else  
  *tft8.rdPort &= tft8.rdPinMaskClr;
#endif 
#else  
  digitalWrite(tft8._rd, LOW);
#endif 
}