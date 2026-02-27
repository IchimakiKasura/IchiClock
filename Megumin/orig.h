#include <Wire.h>
#include <Adafruit_ST7735.h>

#define TFT_CS   10
#define TFT_RST  8
#define TFT_DC   9

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

// Color definitions BGR
#define M_HAT 0x41EB
#define M_HAT_SHADOW 0x2907
#define M_RED 0x4A18
#define M_YELLOW 0x65BE
#define M_HAIR 0x632F
#define M_HAIR_SHADOW 0x3188
#define M_SKIN 0xBEFF
#define M_SKIN_SHADOW 0x959B
#define M_LOW_EYE 0x4A76
#define M_HIGH_EYE 0x28CA
#define OUTLINE ST77XX_BLACK
#define BG ST7735_WHITE


void setup() {
  tft.initR(INITR_GREENTAB);
  tft.setRotation(0);
  tft.fillScreen(BG);

  // x, y, w, h, c

  // outlines
  tft.fillRect( 0, 16, 30,  1, OUTLINE);
  tft.fillRect( 1, 15, 28,  1, OUTLINE);
  tft.fillRect( 2, 14, 26,  1, OUTLINE);
  tft.fillRect( 3, 13, 24,  4, OUTLINE);
  tft.fillRect( 4, 13, 22,  7, OUTLINE);
  tft.fillRect( 5, 10, 20, 14, OUTLINE);
  tft.fillRect( 6,  8, 18,  2, OUTLINE);
  tft.fillRect( 6, 24,  3,  2, OUTLINE);
  tft.fillRect( 7,  7, 21,  1, OUTLINE);
  tft.fillRect( 7, 26,  1,  2, OUTLINE);
  tft.fillRect( 8,  6, 18,  1, OUTLINE);
  tft.fillRect( 9,  5, 16,  1, OUTLINE);
  tft.fillRect( 9, 24,  1,  1, OUTLINE);
  tft.fillRect(10,  4, 14,  1, OUTLINE);
  tft.fillRect(11,  3, 12,  1, OUTLINE);
  tft.fillRect(12,  2, 10,  1, OUTLINE);
  tft.fillRect(13,  1,  8,  1, OUTLINE);
  tft.fillRect(14,  0,  6,  1, OUTLINE);
  tft.fillRect(20, 24,  1,  1, OUTLINE);
  tft.fillRect(21, 24,  3,  2, OUTLINE);
  tft.fillRect(22, 26,  1,  2, OUTLINE);
  tft.fillRect(26,  8,  1,  1, OUTLINE);
  tft.fillRect(27,  8,  2,  2, OUTLINE);

  // HAT COLOR
  tft.fillRect( 5, 12, 4, 1, M_HAT);
  tft.fillRect( 5, 13, 1, 1, M_HAT);
  tft.fillRect( 7, 12, 4, 1, M_HAT);
  tft.fillRect( 8,  7, 3, 1, M_HAT);
  tft.fillRect( 9,  6, 1, 1, M_HAT);
  tft.fillRect(11, 11, 8, 1, M_HAT);
  tft.fillRect(13,  2, 5, 2, M_HAT);
  tft.fillRect(13,  6, 1, 1, M_HAT);
  tft.fillRect(14,  1, 3, 4, M_HAT);
  tft.fillRect(14,  5, 2, 2, M_HAT);
  tft.fillRect(16,  6, 1, 1, M_HAT);
  tft.fillRect(19,  7, 1, 1, M_HAT);
  tft.fillRect(19, 12, 4, 1, M_HAT);
  tft.fillRect(20,  6, 2, 2, M_HAT);
  tft.fillRect(24, 13, 1, 1, M_HAT);
  tft.fillRect(18,  1, 2, 1, M_HAT_SHADOW);
  tft.fillRect(19,  2, 2, 2, M_HAT_SHADOW);
  tft.fillRect(20,  3, 2, 2, M_HAT_SHADOW);
  tft.fillRect(22,  4, 1, 2, M_HAT_SHADOW);
  tft.fillRect(23,  5, 1, 2, M_HAT_SHADOW);
  tft.fillRect(24,  6, 1, 1, M_HAT_SHADOW);

  // HAT RED
  tft.fillRect( 7,  9, 4, 2, M_RED);
  tft.fillRect(10,  5, 3, 1, M_RED);
  tft.fillRect(11,  4, 1, 1, M_RED);
  tft.fillRect(11,  6, 1, 1, M_RED);
  tft.fillRect(11,  8, 8, 2, M_RED);
  tft.fillRect(17,  5, 3, 1, M_RED);
  tft.fillRect(18,  4, 1, 1, M_RED);
  tft.fillRect(18,  6, 1, 1, M_RED);
  tft.fillRect(19,  9, 4, 2, M_RED);
  tft.fillRect(23, 10, 1, 2, M_RED);

  // HAT YELLOW
  tft.fillRect( 2, 15, 1, 1, M_YELLOW);
  tft.fillRect( 3, 14, 3, 1, M_YELLOW);
  tft.fillRect( 6, 10, 1, 2, M_YELLOW);
  tft.fillRect( 6, 13, 5, 1, M_YELLOW);
  tft.fillRect( 7,  9, 1, 1, M_YELLOW);
  tft.fillRect(10,  9, 3, 1, M_YELLOW);
  tft.fillRect(11,  5, 1, 1, M_YELLOW);
  tft.fillRect(11,  8, 1, 1, M_YELLOW);
  tft.fillRect(11, 12, 8, 1, M_YELLOW);
  tft.fillRect(15,  9, 3, 1, M_YELLOW);
  tft.fillRect(16,  8, 1, 1, M_YELLOW);
  tft.fillRect(18,  5, 1, 1, M_YELLOW);
  tft.fillRect(19, 10, 3, 1, M_YELLOW);
  tft.fillRect(19, 13, 5, 1, M_YELLOW);
  tft.fillRect(20,  9, 1, 1, M_YELLOW);
  tft.fillRect(24, 14, 3, 1, M_YELLOW);
  tft.fillRect(27,  8, 1, 1, M_YELLOW);
  tft.fillRect(27, 15, 1, 1, M_YELLOW);

  // SKIN
  tft.fillRect( 8, 17, 1, 4, M_SKIN);
  tft.fillRect( 9, 16, 4, 6, M_SKIN);
  tft.fillRect(10, 15, 2, 1, M_SKIN_SHADOW);
  tft.fillRect(10, 22, 10, 1, M_SKIN);
  tft.fillRect(13, 17, 1, 6, M_SKIN);
  tft.fillRect(14, 18, 1, 5, M_SKIN);
  tft.fillRect(15, 19, 2, 4, M_SKIN);
  tft.fillRect(17, 17, 1, 6, M_SKIN);
  tft.fillRect(18, 15, 2, 1, M_SKIN_SHADOW);
  tft.fillRect(18, 16, 3, 6, M_SKIN);
  tft.fillRect(21, 17, 1, 4, M_SKIN);

  // EYE
  tft.fillRect(10, 17, 2, 1, OUTLINE);
  tft.fillRect(10, 18, 2, 1, M_HIGH_EYE);
  tft.fillRect(10, 19, 2, 1, M_LOW_EYE);
  tft.fillRect(18, 17, 2, 1, OUTLINE);
  tft.fillRect(18, 18, 2, 1, M_HIGH_EYE);
  tft.fillRect(18, 19, 2, 1, M_LOW_EYE);

  // HAIR
  tft.fillRect( 5, 17, 2, 3, M_HAIR);
  tft.fillRect( 6, 16, 2, 1, M_HAIR);
  tft.fillRect( 6, 20, 1, 2, M_HAIR);
  tft.fillRect( 6, 22, 2, 2, M_HAIR);
  tft.fillRect( 7, 23, 2, 2, M_HAIR);
  tft.fillRect( 7, 25, 1, 1, M_HAIR);
  tft.fillRect( 9, 23, 1, 1, M_HAIR);
  tft.fillRect(13, 15, 1, 1, M_HAIR);
  tft.fillRect(14, 15, 3, 2, M_HAIR);
  tft.fillRect(15, 17, 1, 1, M_HAIR);
  tft.fillRect(20, 23, 1, 1, M_HAIR);
  tft.fillRect(21, 23, 2, 2, M_HAIR);
  tft.fillRect(22, 16, 2, 1, M_HAIR);
  tft.fillRect(22, 22, 2, 2, M_HAIR);
  tft.fillRect(22, 25, 1, 1, M_HAIR);
  tft.fillRect(23, 17, 2, 3, M_HAIR);
  tft.fillRect(23, 20, 1, 2, M_HAIR);
  tft.fillRect( 5, 16, 1, 1, M_HAIR_SHADOW);
  tft.fillRect(13, 14, 4, 1, M_HAIR_SHADOW);
  tft.fillRect(16, 15, 3, 1, M_HAIR_SHADOW);
  tft.fillRect(21, 15, 3, 1, M_HAIR_SHADOW);
  tft.fillRect(24, 16, 1, 1, M_HAIR_SHADOW);
}

void loop() {}