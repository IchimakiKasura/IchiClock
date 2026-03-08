#include "Min_Adafruit_ST7735.h"
#include "Min_Adafruit_ST77xx.h"

Adafruit_ST7735::Adafruit_ST7735(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk,
                                 int8_t rst)
    : Adafruit_ST77xx(ST7735_TFTWIDTH_128, ST7735_TFTHEIGHT_160, cs, dc, mosi,
                      sclk, rst) {}

Adafruit_ST7735::Adafruit_ST7735(int8_t cs, int8_t dc, int8_t rst)
    : Adafruit_ST77xx(ST7735_TFTWIDTH_128, ST7735_TFTHEIGHT_160, cs, dc, rst) {}

static const uint8_t PROGMEM
  Rcmd1[] = {                       
    15,                             
    ST77XX_SWRESET,   ST_CMD_DELAY, 
      150,                          
    ST77XX_SLPOUT,    ST_CMD_DELAY, 
      255,                          
    ST7735_FRMCTR1, 3,              
      0x01, 0x2C, 0x2D,            
    ST7735_FRMCTR2, 3,             
      0x01, 0x2C, 0x2D,             
    ST7735_FRMCTR3, 6,              
      0x01, 0x2C, 0x2D,             
      0x01, 0x2C, 0x2D,             
    ST7735_INVCTR,  1,              
      0x07,                         
    ST7735_PWCTR1,  3,              
      0xA2,
      0x02,                         
      0x84,                         
    ST7735_PWCTR2,  1,              
      0xC5,                         
    ST7735_PWCTR3,  2,              
      0x0A,                        
      0x00,                        
    ST7735_PWCTR4,  2,             
      0x8A,                         
      0x2A,                         
    ST7735_PWCTR5,  2,             
      0x8A, 0xEE,
    ST7735_VMCTR1,  1,             
      0x0E,
    ST77XX_INVOFF,  0,              
    ST77XX_MADCTL,  1,              
      0xC8,                        
    ST77XX_COLMOD,  1,             
      0x05 },                      

  Rcmd2green[] = {                 
    2,                             
    ST77XX_CASET,   4,            
      0x00, 0x02,                   
      0x00, 0x7F+0x02,            
    ST77XX_RASET,   4,             
      0x00, 0x01,                  
      0x00, 0x9F+0x01 },           
  Rcmd3[] = {                       
    4,                             
    ST7735_GMCTRP1, 16      ,       
      0x02, 0x1c, 0x07, 0x12,      
      0x37, 0x32, 0x29, 0x2d,       
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      ,     
      0x03, 0x1d, 0x07, 0x06,       
      0x2E, 0x2C, 0x29, 0x2D,       
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST77XX_NORON,     ST_CMD_DELAY, 
      10,
    ST77XX_DISPON,    ST_CMD_DELAY,
      100 };

void Adafruit_ST7735::initR() {
  uint8_t madctl = 0;
  commonInit(Rcmd1);
  displayInit(Rcmd2green);
  _colstart = 2;
  _rowstart = 1;
  displayInit(Rcmd3);
  setRotation(0);
}