#include <Wire.h>
#include <Adafruit_ST7735.h>
#include <avr/pgmspace.h>
#include "song.h"

#define TFT_CS   10
#define TFT_RST  8
#define TFT_DC   9

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

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
  tft.fillScreen(0x18A8);
  pinMode(5, OUTPUT);

  //outline
      tft.fillRect(11,  0,  8,  1, OUTLINE);
      tft.fillRect(10,  1, 10,  1, OUTLINE);
      tft.fillRect( 9,  2, 12,  1, OUTLINE);
      tft.fillRect( 8,  3, 14,  1, OUTLINE);
      tft.fillRect( 7,  4, 16,  1, OUTLINE);
      tft.fillRect( 7,  5, 18,  1, OUTLINE);
      tft.fillRect( 6,  6, 15, 14, OUTLINE);
      tft.fillRect( 5,  8, 17, 12, OUTLINE);
      tft.fillRect( 3, 11, 21,  2, OUTLINE);
      tft.fillRect( 2, 12, 23,  2, OUTLINE);
      tft.fillRect( 1, 13, 25,  1, OUTLINE);
      tft.fillRect( 0, 14,  3,  1, OUTLINE);
      tft.fillRect( 4, 11, 19,  6, OUTLINE);
      tft.fillRect(24, 14,  3,  1, OUTLINE);
      tft.fillRect( 6, 20,  3,  2, OUTLINE);
      tft.fillRect(18, 20,  3,  2, OUTLINE);
      tft.fillRect( 7, 22,  1,  1, OUTLINE);
      tft.fillRect(19, 22,  1,  1, OUTLINE);
      tft.fillRect(23,  6,  1,  1, OUTLINE);
      tft.fillRect(25,  6,  1,  1, OUTLINE);
      tft.fillRect(24,  7,  2,  1, OUTLINE);

  // HAT
      tft.fillRect(11,  1,  5,  1, M_HAT);
      tft.fillRect(12,  2,  3,  1, M_HAT);
      tft.fillRect(13,  3,  1,  1, M_HAT);
      tft.fillRect(12,  4,  3,  1, M_HAT);
      
      tft.fillRect( 8,  5,  2,  1, M_HAT);
      tft.fillRect(17,  5,  2,  1, M_HAT);
      
      tft.fillRect( 8,  4,  1,  1, M_HAT);
      tft.fillRect(18,  4,  1,  1, M_HAT);

      tft.fillRect( 7, 10,  3,  1, M_HAT);
      tft.fillRect(10,  9,  7,  1, M_HAT);
      tft.fillRect(17, 10,  3,  1, M_HAT);

      tft.fillRect( 5, 11,  1,  1, M_HAT);
      tft.fillRect(21, 11,  1,  1, M_HAT);
      
      // SHADOW
      tft.fillRect(17,  1,  2,  1, M_HAT_SHADOW);
      tft.fillRect(18,  2,  2,  1, M_HAT_SHADOW);
      tft.fillRect(19,  3,  2,  1, M_HAT_SHADOW);
      tft.fillRect(20,  4,  2,  1, M_HAT_SHADOW);

  // RED
  tft.fillRect( 9,  3,  3,  1, M_RED);
  tft.fillRect(15,  3,  3,  1, M_RED);

  tft.fillRect(10,  2,  1,  3, M_RED);
  tft.fillRect(16,  2,  1,  3, M_RED);

  tft.fillRect( 7,  7,  3,  2, M_RED);
  tft.fillRect(17,  7,  3,  2, M_RED);

  tft.fillRect(10,  6,  7,  2, M_RED);
  tft.fillRect(20,  8,  1,  2, M_RED);

  //YELLOW
      tft.fillRect(10,  3,  1,  1, M_YELLOW);
      tft.fillRect(16,  3,  1,  1, M_YELLOW);

      tft.fillRect( 7,  7,  1,  1, M_YELLOW);
      tft.fillRect(18,  7,  1,  1, M_YELLOW);

      tft.fillRect(10,  6,  1,  1, M_YELLOW);
      tft.fillRect(14,  6,  1,  1, M_YELLOW);
      tft.fillRect(24,  6,  1,  1, M_YELLOW);


      tft.fillRect( 2, 13,  1,  1, M_YELLOW);
      tft.fillRect(24, 13,  1,  1, M_YELLOW);

      tft.fillRect( 6,  8,  1,  2, M_YELLOW);
      tft.fillRect( 9,  7,  3,  1, M_YELLOW);

      tft.fillRect(13,  7,  3,  1, M_YELLOW);
      tft.fillRect(17,  8,  3,  1, M_YELLOW);

      tft.fillRect( 3, 12,  3,  1, M_YELLOW);
      tft.fillRect(21, 12,  3,  1, M_YELLOW);

      tft.fillRect( 6, 11,  4,  1, M_YELLOW);
      tft.fillRect(17, 11,  4,  1, M_YELLOW);

      tft.fillRect(10, 10,  7,  1, M_YELLOW);
      tft.fillRect(10, 10,  7,  1, M_YELLOW);

  //HAIR
      tft.fillRect( 5, 14,  2,  3, M_HAIR);
      tft.fillRect(20, 14,  2,  3, M_HAIR);
      
      tft.fillRect( 6, 17,  1,  3, M_HAIR);
      tft.fillRect(20, 17,  1,  3, M_HAIR);
      
      tft.fillRect( 7, 20,  1,  2, M_HAIR);
      tft.fillRect(19, 20,  1,  2, M_HAIR);
      
      tft.fillRect(12, 13,  3,  1, M_HAIR);
      tft.fillRect(13, 14,  1,  1, M_HAIR);
  
      // SHADOW
      tft.fillRect(12, 12,  3,  1, M_HAIR_SHADOW);

      tft.fillRect( 6, 13,  2,  1, M_HAIR_SHADOW);
      tft.fillRect(19, 13,  2,  1, M_HAIR_SHADOW);
      
      tft.fillRect( 5, 14,  1,  1, M_HAIR_SHADOW);
      tft.fillRect(21, 14,  1,  1, M_HAIR_SHADOW);
      
      tft.fillRect( 8, 19,  1,  1, M_HAIR_SHADOW);
      tft.fillRect(18, 19,  1,  1, M_HAIR_SHADOW);

  // SKIN
      tft.fillRect( 8, 14,  4,  4, M_SKIN);
      tft.fillRect(15, 14,  4,  4, M_SKIN);

      tft.fillRect( 9, 16,  9,  3, M_SKIN);

      tft.fillRect(12, 15,  1,  1, M_SKIN);

      // SHADOW
      tft.fillRect( 9, 13,  2,  1, M_SKIN_SHADOW);
      tft.fillRect(16, 13,  2,  1, M_SKIN_SHADOW);

  // LEFT EYE
      tft.fillRect( 9, 14,  2,  1, OUTLINE);
      tft.fillRect( 9, 15,  2,  1, M_HIGH_EYE);
      tft.fillRect( 9, 16,  2,  1, M_LOW_EYE);
  // RIGHT EYE
      tft.fillRect(16, 14,  2,  1, OUTLINE);
      tft.fillRect(16, 15,  2,  1, M_HIGH_EYE);
      tft.fillRect(16, 16,  2,  1, M_LOW_EYE);

  // mouth
  tft.fillRect(12, 17,  3,  1, OUTLINE);

  // Logo(69,70,3);
  // playSong(5);
}

void loop() {}