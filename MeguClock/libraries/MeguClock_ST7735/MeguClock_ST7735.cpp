/**************************************************************************
    This library is a modified version of the Adafruit_ST7735_and_ST7789_Library
    combined with Adafruit_GFX_Library.

    https://github.com/adafruit/Adafruit-GFX-Library
    https://github.com/adafruit/Adafruit-ST7735-Library
    
    If you wish to use this feel free though it is hard coded for
      - Arduino boards (AVR)
      - 1.8" GREEN TAB 128x160
      - Screen is Flipped
    
    If you wish to use higher resolution than 128x160, change the Width and
    Height arguments from uint8_t to uint16_t:

      setAddrWindow(uint16_t x, uint16_t y, uint8_t w, uint8_t h);
      fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint16_t color);
      getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint8_t *w, uint8_t *h);
      getTextBounds(const __FlashStringHelper *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint8_t *w, uint8_t *h);
      uint8_t _width;
      uint8_t _height;

    It is specifically designed for my simple digital clock as the library
    is flash intensive, I've scrap the unused ones to barebones of the code.

    Author: Ichimaki Kasura
**************************************************************************/
#include "MeguClock_ST7735.h"
#include "glcdfont.c"
#include <avr/pgmspace.h>

#define AVR_WRITESPI(x) for (SPDR = (x); (!(SPSR & _BV(SPIF)));)

static const uint8_t PROGMEM
  Rcmd1[] = {
       15,              // Number of commands in this list
     // --- Reset and wake display ---
     0x01, 0x80,        // SWRESET (Software reset)
       50,              // wait 50ms for controller reset
     0x11, 0x80,        // SLPOUT (Sleep out)
      255,              // wait 255ms for internal voltages to stabilize
     // --- Frame rate configuration ---
     0xB1, 3,           // FRMCTR1 - Frame rate control (normal mode)
     0x01,              // RTNA: internal oscillator division
     0x2C,              // FPA: front porch timing
     0x2D,              // BPA: back porch timing
     0xB2, 3,           // FRMCTR2 - Frame rate control (idle mode)
     0x01,              // same meaning as above
     0x2C,
     0x2D,
     0xB3, 6,           // FRMCTR3 - Frame rate control (partial mode)
     0x01,              // RTNA
     0x2C,              // FPA
     0x2D,              // BPA
     0x01,              // second set (used in partial mode)
     0x2C,
     0x2D,
     // --- Pixel inversion ---
     0xB4, 1,           // INVCTR - Display inversion control
     0x07,              // 2-dot inversion (reduces flicker)
     // --- Power control ---
     0xC0, 3,           // PWCTR1 - Power control
     0xA2,              // GVDD = LCD voltage level
     0x02,              // VGH/VGL voltage multiplier
     0x84,              // operation amplifier current
     0xC1, 1,           // PWCTR2
     0xC5,              // VGH/VGL voltage level
     0xC2, 2,           // PWCTR3
     0x0A,              // amplifier current adjustment
     0x00,              // boost frequency
     0xC3, 2,           // PWCTR4
     0x8A,              // VCOM amplitude control
     0x2A,              // VCOM amplitude fine adjustment
     0xC4, 2,           // PWCTR5
     0x8A,              // VCOM offset
     0xEE,              // additional voltage adjustment
     0xC5, 1,           // VMCTR1 - VCOM control
     0x0E,              // VCOM voltage (affects contrast)
     // --- Display mode ---
     0x20, 0,           // INVOFF - disable display inversion
     // --- Memory orientation ---
     0x36, 1,          // MADCTL - Memory data access control
     0xC0,             // bit settings: [0xC8 original]
                       // MY  = row address order
                       // MX  = column address order
                       // MV  = row/column swap
                       // BGR = color order
    // --- Pixel format ---
     0x3A, 1,           // COLMOD - Color mode
     0x05               // 16-bit color (RGB565)
},
  Rcmd2green[] = {              // This is where the part
        2,                      // where you most likely change
     0x2A,     4,               // to RED or other tab
     0x00,  0x00,               // if you want to use this minified version
     0x00,  0x80,               // 
     0x2B,     4,               // seek to Adafruit_ST77xx to see the red tab code
     0x00,  0x00,                  
     0x00,  0xA0
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
        5,
     0x29,  0x80,
      100
};

/*!
    @brief Instantiate MeguClock ST7735 driver with software SPI
    @param cs Chip select pin #
    @param dc Data/Command pin #
    @param rst reset pin #
*/
MeguClock_ST7735::MeguClock_ST7735(int8_t cs, int8_t dc, int8_t rst) : _rst(rst), _cs(cs), _dc(dc)  {
    _width = TFT_WIDTH;
    _height = TFT_HEIGHT;
    cursor_y = cursor_x = 0;
    textsize = 1;
    textcolor = textbgcolor = 0xFFFF;
}
/*!
    @brief  Initialization code
*/
void MeguClock_ST7735::init() {
    DDRB = (1 << _rst)|(1 << _dc)|(1 << _cs);
    PORTB |= (1 << _rst)|(1 << _dc)|(1 << _cs);
    if(hwspi._spi) hwspi._spi->begin();
    startWrite();
    displayInit(Rcmd1);
    displayInit(Rcmd2green);
    displayInit(Rcmd3);
}
/*!
    @brief  Companion code to the initiliazation tables. Reads and issues
            a series of LCD commands stored in PROGMEM byte array.
    @param  addr  Flash memory array with commands and data to send
*/
void MeguClock_ST7735::displayInit(const uint8_t *addr) {
  uint8_t numCommands, numArgs;
  uint16_t ms;
  numCommands = pgm_read_byte(addr++);
  while (numCommands--) {
    writeAVRSPI(pgm_read_byte(addr++));
    numArgs  = pgm_read_byte(addr++);
    ms       = numArgs & 0x80;
    numArgs &= ~0x80;
    while (numArgs--) AVR_WRITESPI(pgm_read_byte(addr++));
    if (ms) {
      ms = pgm_read_byte(addr++);
      delay(ms == 255 ? 500 : ms);
    }
  }
}
/*!
  @brief  SPI displays set an address window rectangle for blitting pixels
  @param  x  Top left corner x coordinate
  @param  y  Top left corner y coordinate
  @param  w  Width of window
  @param  h  Height of window
*/
void MeguClock_ST7735::setAddrWindow(uint16_t x, uint16_t y, uint8_t w, uint8_t h) {
    x += 2; y += 1;
    startWrite();
    writeAVRSPI(0x2A);
    AVR_WRITESPI(x >> 8); AVR_WRITESPI(x & 0xFF);
    AVR_WRITESPI((x + w - 1) >> 8); AVR_WRITESPI((x + w - 1) & 0xFF);
    writeAVRSPI(0x2B); 
    AVR_WRITESPI(y >> 8); AVR_WRITESPI(y & 0xFF);
    AVR_WRITESPI((y + h - 1) >> 8); AVR_WRITESPI((y + h - 1) & 0xFF);
    writeAVRSPI(0x2C);
}
/*!
  @brief  Draw a pixel to the canvas framebuffer
  @param  x  Top left corner x coordinate
  @param  y  Top left corner y coordinate
  @param  color 16-bit 5-6-5 Color to fill with
*/
void MeguClock_ST7735::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if(x<0 ||x>=_width || y<0 || y>=_height) return;
  setAddrWindow(x, y, 1, 1);
  writeColor(color, 1);
  endWrite();
}
/*!
  @brief  Fill a rectangle completely with one color.
  @param  x  Top left corner x coordinate
  @param  y  Top left corner y coordinate
  @param  w  Width of window
  @param  h  Height of window
  @param  color 16-bit 5-6-5 Color to fill with
*/
void MeguClock_ST7735::fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint16_t color) {
  if(x>=_width || y>=_height || w<=0 || h<=0) return;
  if(x+w-1>=_width)  w=_width -x;
if(y+h-1>=_height) h=_height-y;
  setAddrWindow(x, y, w, h);
  writeColor(color, (uint32_t)w * h);
  endWrite();
}
/*!
    @brief  Fill the framebuffer completely with one color
    @param  color Binary (on or off) color to fill with
*/
void MeguClock_ST7735::fillScreen(uint16_t color) {
  setAddrWindow(0, 0, _width, _height);
  writeColor(color, _width * _height);
  endWrite();
}
/*!
   @brief   Draw a single character
    @param    x   Bottom left corner x coordinate
    @param    y   Bottom left corner y coordinate
    @param    c   The 8-bit font-indexed character (likely ascii)
    @param    color 16-bit 5-6-5 Color to draw chraracter with
    @param    bg 16-bit 5-6-5 Color to fill background with (if same as color, no background)
    @param    size_tx  Font magnification level, 1 is 'original' size
*/
void MeguClock_ST7735::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_tx) {
  if ((x >= _width) || (y >= _height) || ((x + 6 * size_tx - 1) < 0) || ((y + 8 * size_tx - 1) < 0)) return;
  if ((c >= 176)) c++;
  for (int8_t i = 0; i < 5; i++) {
    uint8_t line = pgm_read_byte(&font[c * 5 + i]);
    for (int8_t j = 0; j < 8; j++, line >>= 1) {
      if (line & 1) {
        if (size_tx == 1) drawPixel(x + i, y + j, color);
        else fillRect(x + i * size_tx, y + j * size_tx, size_tx, size_tx, color);
      } else if (bg != color) {
        if (size_tx == 1) drawPixel(x + i, y + j, bg);
        else fillRect(x + i * size_tx, y + j * size_tx, size_tx, size_tx, bg);
      }
    }
  }
  if (bg != color) {
    if (size_tx == 1) {
      setAddrWindow(x, y, 1, 8);
      for (int i = 8; i--;) writeColor(bg, 1);
    }
    else
      fillRect(x + 5 * size_tx, y, size_tx, 8 * size_tx, bg);
  }
}
/*!
    @brief  Print one byte/character of data, used to support print()
    @param  c  The 8-bit ascii character to write
*/
size_t MeguClock_ST7735::write(uint8_t c) {
  if (c == '\n') {              
    cursor_x = 0;               
    cursor_y += textsize * 8; 
  } else if (c != '\r') {       
    if ((cursor_x + textsize * 6) > _width) { 
      cursor_x = 0;                                       
      cursor_y += textsize * 8; 
    }
    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize * 6; 
  }
  return 1;
}
/*!
    @brief  Helper to determine size of a string with current font/size.
            Pass string and a cursor position, returns UL corner and W,H.
    @param  str  The ASCII string to measure
    @param  x    The current cursor X
    @param  y    The current cursor Y
    @param  x1   The boundary X coordinate, returned by function
    @param  y1   The boundary Y coordinate, returned by function
    @param  w    The boundary width, returned by function
    @param  h    The boundary height, returned by function
*/
void MeguClock_ST7735::getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint8_t *w, uint8_t *h) {
    uint8_t c;
    int16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1;
    *x1 = x; *y1 = y; *w = *h = 0;
    while((c = *str++)) charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
    if(maxx >= minx) *w = maxx - minx + 1;
    if(maxy >= miny) *h = maxy - miny + 1;
    *x1 = minx; *y1 = miny;
}
/*!
    @brief    Helper to determine size of a PROGMEM string with current
   font/size. Pass string and a cursor position, returns UL corner and W,H.
    @param    str     The flash-memory ascii string to measure
    @param    x       The current cursor X
    @param    y       The current cursor Y
    @param    x1      The boundary X coordinate, set by function
    @param    y1      The boundary Y coordinate, set by function
    @param    w      The boundary width, set by function
    @param    h      The boundary height, set by function
*/
void MeguClock_ST7735::getTextBounds(const __FlashStringHelper *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint8_t *w, uint8_t *h) {
    uint8_t *s = (uint8_t *)str, c;
    int16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1;
    *x1 = x; *y1 = y; *w = *h = 0;
    while ((c = pgm_read_byte(s++))) charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
    if(maxx >= minx) *w = maxx - minx + 1;
    if(maxy >= miny) *h = maxy - miny + 1;
    *x1 = minx; *y1 = miny;
}
/*!
    @brief  Helper to determine size of a character with current font/size.
            Broke this out as it's used by both the PROGMEM- and RAM-resident
            getTextBounds() functions.
    @param  c     The ASCII character in question
    @param  x     Pointer to x location of character. Value is modified by
                  this function to advance to next character.
    @param  y     Pointer to y location of character. Value is modified by
                  this function to advance to next character.
    @param  minx  Pointer to minimum X coordinate, passed in to AND returned
                  by this function -- this is used to incrementally build a
                  bounding rectangle for a string.
    @param  miny  Pointer to minimum Y coord, passed in AND returned.
    @param  maxx  Pointer to maximum X coord, passed in AND returned.
    @param  maxy  Pointer to maximum Y coord, passed in AND returned.
*/
void MeguClock_ST7735::charBounds(unsigned char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy) {
  if(c == '\n') { *x = 0; *y += textsize * 8; }
    else if(c != '\r') {
      if(*x + textsize * 6 > _width) {
          *x = 0;
          *y += textsize * 8;
      }
      int16_t x2 = *x + textsize * 5 - 1;
      int16_t y2 = *y + textsize * 8 - 1;
      if(x2 > *maxx) *maxx = x2;
      if(y2 > *maxy) *maxy = y2;
      if(*x < *minx) *minx = *x;
      if(*y < *miny) *miny = *y;
      *x += textsize * 6;
  }
}
/*!
   @brief Start a display-writing routine.
*/
inline void MeguClock_ST7735::startWrite() {
  hwspi._spi->beginTransaction(hwspi.settings);
  PORTB &= ~(1 << _cs);
}
/*!
   @brief Ends a display-writing routine.
*/
inline void MeguClock_ST7735::endWrite() {
  PORTB |= ~(1 << _cs);
  hwspi._spi->endTransaction();
}
/*!
   @brief Writes to AVR's SPI connection.
*/
inline void MeguClock_ST7735::writeAVRSPI(uint8_t addr) {
  PORTB &= ~(1 << _dc);
  AVR_WRITESPI(addr);
  PORTB |= (1 << _dc);
}
/*!
   @brief Color, just color.
*/
inline void MeguClock_ST7735::writeColor(uint16_t color, uint32_t length) {
    uint8_t hi = color >> 8, lo = color & 0xFF;
    while (length--) {
      AVR_WRITESPI(hi);
      AVR_WRITESPI(lo);
    }
}