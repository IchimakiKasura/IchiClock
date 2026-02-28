#pragma once
#include "../assets/meguminLogo.h"
#include "../config/ColorConfig.h"

class DrawUI {
    private:
        Adafruit_ST7735* _tft;
        int16_t _clockX;
        bool _clockBoundsCached = false;

        void _cacheClockBounds() {
            if (_clockBoundsCached) return;

            uint16_t _clockW, _clockH;
            int16_t x1, y1;

            _tft->getTextBounds(F("00:00"), 0, 0, &x1, &y1, &_clockW, &_clockH);
            _clockX = (_tft->width() - _clockW) / 2;
            _clockBoundsCached = true;
        }

        bool _hideField(const Field &f) {
            return editMode && selected==f && !adjustHeld && !blinkState;
        }

        void _clearLine(const int16_t &y,const int16_t &h) {
            _tft->fillRect(3, y, _tft->width() - 8, h, 0x18A8); 
        }

        void _CenteredText(const char* t,const int16_t &y,const int16_t &s,const uint16_t &c) {
            int16_t x1, y1;
            uint16_t w, h;
            _tft->setTextSize(s);
            _tft->setTextColor(c);
            _tft->getTextBounds(t, 0, 0, &x1, &y1, &w, &h);
            _tft->setCursor((_tft->width() - w) / 2, y);
            _tft->print(t);
        }

        void _ProgressBar(int percent) {
            byte x = 40, y = 145;
            _tft->fillRect(x,     y,     42,               1, WHITE);        // top
            _tft->fillRect(x,     y+1,   1,                4, WHITE);        // left
            _tft->fillRect(x+41,  y+1,   1,                4, WHITE);        // right
            _tft->fillRect(x,     y+5,   42,               1, WHITE);        // bottom
            _tft->fillRect(x+1,   y+1,   (40*percent)/100, 4, YELLOW);       // fill
        }

        void _Logo(int x, int y, uint8_t scale) {
            for (uint16_t i = 0; i < sizeof(rects)/sizeof(rects[0]); i++) {
                Rect r; memcpy_P(&r, &rects[i], sizeof(Rect));
                tft.fillRect(
                    x + round((r.x-17)*scale),
                    y + round((r.y-15)*scale),
                    max(1, round(r.w*scale)),
                    max(1, round(r.h*scale)),
                    pgm_read_word(&megumin_colors[r.c])
                );
            }
        }
    public:
        bool blinkState = true;
        unsigned long lastBlink = 0;

        DrawUI() = default;

        void init(Adafruit_ST7735& tft) {
            _tft = &tft;
        }

        void Header(byte t) {
            static bool _l = false;
            _clearLine(HEADER_Y-8, 20);

            if(t == 0) {
                _CenteredText("MEGUMIN", HEADER_Y-8, HEADER_SIZE, WHITE);
                _CenteredText("CLOCK", HEADER_Y+5, HEADER_SIZE, WHITE);
            } else {
                _CenteredText("CONFIGURE", HEADER_Y-8, HEADER_SIZE, WHITE);
                _CenteredText("TIME/DATE", HEADER_Y+5, HEADER_SIZE, WHITE);
            }
            
            _Logo(19,22,1); _Logo(109,22,1);
        }

        void Time(const DateTime &t = now) {
            _clearLine(CLOCK_Y, 30);

            char Buf[3];
            uint8_t h12 = t.hour() % 12;
            const char* ampm = _hideField(FIELD_AMPM) ? "--" : (t.hour() >= 12 ? "PM" : "AM");

            if (!h12) h12 = 12;
            
            _tft->setTextSize(CLOCK_SIZE);
            _cacheClockBounds();
            _tft->setCursor(_clockX, CLOCK_Y);

            if (!_hideField(FIELD_HOUR)) sprintf(Buf, "%02d", h12);
            _tft->setTextColor(_hideField(FIELD_HOUR) ? RED : M_COLORS::ClockColor());
            _tft->print(_hideField(FIELD_HOUR) ? "--" : Buf);

            _tft->setTextColor(WHITE);
            _tft->print(":");

            if (!_hideField(FIELD_MIN))  sprintf(Buf, "%02d", t.minute());
            _tft->setTextColor(_hideField(FIELD_MIN) ? RED : M_COLORS::ClockColor());
            _tft->print(_hideField(FIELD_MIN) ? "--" : Buf);

            _clearLine(AMPM_Y, 20);
            _CenteredText(ampm, AMPM_Y, AMPM_SIZE, _hideField(FIELD_AMPM) ? RED : M_COLORS::ClockColor());
        }

        void Date(const DateTime &t = now) {
            _clearLine(DATE_Y, 22);

            char Buf[9];
            bool hideMo  = _hideField(FIELD_MONTH);
            bool hideDay = _hideField(FIELD_DAY);
            bool hideYr  = _hideField(FIELD_YEAR);
            uint16_t c = ((hideMo || hideDay || hideYr) ? RED : M_COLORS::DateColor());

            _tft->setTextSize(DATE_SIZE);
            _tft->setTextColor(c);
            _tft->setCursor(30, DATE_Y);

            _tft->print(hideMo ? "--- " : strcpy_P(Buf, monthNames[t.month()-1]));

            if (!hideDay) sprintf(Buf, "%02d, ", t.day());
            _tft->print(hideDay ? "--, " : Buf);

            if (!hideYr) sprintf(Buf, "%04d", t.year());
            _tft->print(hideYr ? "----" : Buf);

            _CenteredText(strcpy_P(Buf, daysFull[now.dayOfTheWeek()]), DATE_Y + 10, DATE_SIZE, c);
        }

        void Bottom(const char* t) {
            _clearLine(BOTTOM_Y, 12);
            _CenteredText(t, BOTTOM_Y, BOTTOM_SIZE, GREEN);
        }

        void SystemBoot() {
            _CenteredText("MEGU-CLOCK", 100, 2, WHITE);
            _Logo(69,55,3.0);
        }

        void FakeLoading() {
            const uint8_t barSteps[] = {0, 25, 50, 55, 60, 65, 85, 100};
            const uint16_t delays[] = {250, 250, 250, 150, 150, 150, 250, 500};

            _CenteredText("system loading", 125, 1, YELLOW); 

            for (byte i = 0; i < 8; i++) {
                _ProgressBar(barSteps[i]);
                delay(delays[i]);
            }
        }

        void CheckeredBorders(uint16_t fillColor = 0, uint16_t dashColor = 0) {

            uint16_t hY[] = {1, 38, 128, 158};
            static bool swapColors = false;

            if(fillColor == 0) fillColor = M_COLORS::ClockColor();
            if(dashColor == 0) dashColor = M_COLORS::DateColor();

            if (swapColors) {
                uint16_t temp = fillColor;
                fillColor = dashColor;
                dashColor = temp;
            }

            swapColors = !swapColors;
            
            for (int i = 0; i < 4; i++) {
                for (int x = 1; x < _tft->width() - 1; x += 8) {
                    _tft->fillRect(x      , hY[i], 4, 2, dashColor);
                    _tft->fillRect(x + 4  , hY[i], 4, 2, fillColor);
                }
            }

            for (int y = 1; y < _tft->height(); y += 8) {
                _tft->fillRect(1        , y    , 2, 4, dashColor);
                _tft->fillRect(1        , y + 4, 2, 4, fillColor);
                _tft->fillRect(_tft->width()-2, y+2  , 2, 4, dashColor);
                _tft->fillRect(_tft->width()-2, y+6  , 2, 4, fillColor);
            }
        }

        void TextColorChange(bool saveColor = false) {
            M_COLORS::ClockColor_index = (M_COLORS::ClockColor_index + 1) % 8;
            M_COLORS::DateColor_index = (M_COLORS::ClockColor_index + 1 + rand(7)) % 8;

            if(M_COLORS::DateColor_index == M_COLORS::ClockColor_index)  {
                M_COLORS::DateColor_index = (M_COLORS::DateColor_index + 1) % 8;
            }

            CheckeredBorders(M_COLORS::ClockColor(), M_COLORS::DateColor());
            
            Time();
            Date();

            if (saveColor) M_COLORS::Save();
        }

        void ReDraw(const DateTime &t = now) {
            Time(t);
            Date(t);
        }
};

DrawUI Draw;