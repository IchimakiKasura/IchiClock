#include "custom_ST7735.h"
#include <limits.h>
#if !defined(ARDUINO_STM32_FEATHER) && !defined(ARDUINO_UNOR4_WIFI)
#if !defined(ARDUINO_UNOR4_MINIMA)
#include "pins_arduino.h"
#include "wiring_private.h"
#endif
#endif
#include <SPI.h>

Adafruit_ST7735::Adafruit_ST7735(int8_t cs, int8_t dc, int8_t rst)
    : Min_Adafruit_SPITFT(128, 160, cs, dc, rst) {}
void Adafruit_ST7735::displayInit(const uint8_t *addr) {
  uint8_t numCommands, cmd, numArgs;
  uint16_t ms;
  numCommands = pgm_read_byte(addr++); 
  while (numCommands--) {              
    cmd = pgm_read_byte(addr++);       
    numArgs = pgm_read_byte(addr++);   
    ms = numArgs & 0x80;       
    numArgs &= ~0x80;          
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
void Adafruit_ST7735::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  x += _xstart;
  y += _ystart;
  uint32_t xa = ((uint32_t)x << 16) | (x + w - 1);
  uint32_t ya = ((uint32_t)y << 16) | (y + h - 1);
  writeCommand(0x2A); 
  SPI_WRITE32(xa);
  writeCommand(0x2B); 
  SPI_WRITE32(ya);
  writeCommand(0x2C); 
}
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
  Rcmd2green[] = {
        2,                             
     0x2A,     4,            
     0x00,  0x02,                   
     0x00,  0x7F+0x02,            
     0x2B,     4,             
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

void Adafruit_ST7735::initR() {
  initSPI(32000000, spiMode);
  displayInit(Rcmd1);
  displayInit(Rcmd2green);
  displayInit(Rcmd3);
  _xstart = 2;
  _ystart = 1;

  // flips the screen, Also freaking fixes the BGR? huh
  MeguClockRotate(0x36, 0xC0);
}