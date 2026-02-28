#include <Wire.h>
#include <Adafruit_ST7735.h>
// #include "song.h"

#define TFT_CS   10
#define TFT_RST  8
#define TFT_DC   9

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

const uint16_t colorTable[] PROGMEM = {
  0x0000,0x41EB,0x2907,0x4A18,0x65BE,0x632F,0x3188,0xBEFF,0x959B,0x28CA,0x4A76
};

struct Rect {
  uint8_t x, y, w, h, colorId;
  const int8_t *xOffs; // optional X offsets
  const int8_t *yOffs; // optional Y offsets
  uint8_t nX;           // number of X offsets
  uint8_t nY;           // number of Y offsets
};

#define NO_OFFS nullptr, nullptr, 0, 0

// Predefined offsets
const int8_t off6[] PROGMEM = {6};
const int8_t off10[] PROGMEM = {10};
const int8_t off9[] PROGMEM = {9};
const int8_t off16[] PROGMEM = {16};

// ================= Master Sprite =================
const Rect sprite[] PROGMEM = {
  // Outline
  {11,0,8,1,0, NO_OFFS},{10,1,10,1,0, NO_OFFS},{9,2,12,1,0, NO_OFFS},{8,3,14,1,0, NO_OFFS},
  {7,4,16,1,0, NO_OFFS},{7,5,18,1,0, NO_OFFS},{6,6,15,14,0, NO_OFFS},{5,8,17,12,0, NO_OFFS},
  {3,11,21,2,0, NO_OFFS},{2,12,23,2,0, NO_OFFS},{1,13,25,1,0, NO_OFFS},{0,14,3,1,0, NO_OFFS},
  {4,11,19,6,0, NO_OFFS},{24,14,3,1,0, NO_OFFS},{6,20,3,2,0, NO_OFFS},{18,20,3,2,0, NO_OFFS},
  {7,22,1,1,0, NO_OFFS},{19,22,1,1,0, NO_OFFS},{23,6,1,1,0, NO_OFFS},{25,6,1,1,0, NO_OFFS},{24,7,2,1,0, NO_OFFS},

  // Hat
  {11,1,5,1,1, NO_OFFS},{12,2,3,1,1, NO_OFFS},{13,3,1,1,1, NO_OFFS},{12,4,3,1,1, NO_OFFS},
  {8,5,2,1,1, off9, nullptr, 1,0},{8,4,1,1,1, off10, nullptr, 1,0},{7,10,3,1,1, off10, nullptr, 1,0},
  {5,11,1,1,1, off16, nullptr,1,0},

  // Hat shadow
  {17,1,2,1,2, NO_OFFS},{18,2,2,1,2, NO_OFFS},{19,3,2,1,2, NO_OFFS},{20,4,2,1,2, NO_OFFS},

  // Red
  {9,3,3,1,3, off6, nullptr,1,0},{10,2,1,3,3, off6, nullptr,1,0},{7,7,3,2,3, off10, nullptr,1,0},
  {10,6,7,2,3, NO_OFFS},{20,8,1,2,3, NO_OFFS},

  // Yellow
  {10,3,1,1,4, NO_OFFS},{16,3,1,1,4, NO_OFFS},{7,7,1,1,4, NO_OFFS},{18,7,1,1,4, NO_OFFS},
  {10,6,1,1,4, NO_OFFS},{14,6,1,1,4, NO_OFFS},{24,6,1,1,4, NO_OFFS},{2,13,1,1,4, NO_OFFS},
  {24,13,1,1,4, NO_OFFS},{6,8,1,2,4, NO_OFFS},{9,7,3,1,4, NO_OFFS},{13,7,3,1,4, NO_OFFS},
  {17,8,3,1,4, NO_OFFS},{3,12,3,1,4, NO_OFFS},{21,12,3,1,4, NO_OFFS},{6,11,4,1,4, NO_OFFS},
  {17,11,4,1,4, NO_OFFS},{10,10,7,1,4, NO_OFFS},

  // Hair
  {5,14,2,3,5, NO_OFFS},{20,14,2,3,5, NO_OFFS},{6,17,1,3,5, NO_OFFS},{20,17,1,3,5, NO_OFFS},
  {7,20,1,2,5, NO_OFFS},{19,20,1,2,5, NO_OFFS},{12,13,3,1,5, NO_OFFS},{13,14,1,1,5, NO_OFFS},

  // Hair shadow
  {12,12,3,1,6, NO_OFFS},{6,13,2,1,6, NO_OFFS},{19,13,2,1,6, NO_OFFS},{5,14,1,1,6, NO_OFFS},
  {21,14,1,1,6, NO_OFFS},{8,19,1,1,6, NO_OFFS},{18,19,1,1,6, NO_OFFS},

  // Skin
  {8,14,4,4,7, NO_OFFS},{15,14,4,4,7, NO_OFFS},{9,16,9,3,7, NO_OFFS},{12,15,1,1,7, NO_OFFS},
  {9,13,2,1,8, NO_OFFS},{16,13,2,1,8, NO_OFFS},

  // Eyes
  {9,14,2,1,0, NO_OFFS},{9,15,2,1,9, NO_OFFS},{9,16,2,1,10, NO_OFFS},
  {16,14,2,1,0, NO_OFFS},{16,15,2,1,9, NO_OFFS},{16,16,2,1,10, NO_OFFS},

  // Mouth
  {12,17,3,1,0, NO_OFFS}
};
void drawSprite(int16_t ox, int16_t oy, uint8_t scale) {
  for (uint16_t i = 0; i < sizeof(sprite)/sizeof(Rect); i++) {
    Rect r;
    memcpy_P(&r, &sprite[i], sizeof r);
    uint16_t color = pgm_read_word(&colorTable[r.colorId]);

    // original
    tft.fillRect(ox + r.x*scale, oy + r.y*scale, r.w*scale, r.h*scale, color);

    // X offsets
    for (uint8_t j=0;j<r.nX;j++){
      int8_t xOff = pgm_read_byte(&r.xOffs[j]);
      tft.fillRect(ox + (r.x+xOff)*scale, oy + r.y*scale, r.w*scale, r.h*scale, color);
    }

    // Y offsets (currently unused)
    for (uint8_t j=0;j<r.nY;j++){
      int8_t yOff = pgm_read_byte(&r.yOffs[j]);
      tft.fillRect(ox + r.x*scale, oy + (r.y+yOff)*scale, r.w*scale, r.h*scale, color);
    }
  }
}

void setup() {
  tft.initR(INITR_GREENTAB);
  tft.setRotation(0);
  tft.fillScreen(0x18A8);
  pinMode(5, OUTPUT);

  drawFullSprite(0,0,3);
  // Logo(69,70,3);
  // playSong(5);
}

void loop() {}