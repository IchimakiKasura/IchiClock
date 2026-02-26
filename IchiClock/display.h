#pragma once
bool blinkState = true;
unsigned long lastBlink = 0;

class DrawUI {
    private:
        Adafruit_ST7735* _tft;
        int16_t _clockX;
        bool _clockBoundsCached = false;
        inline void _cacheClockBounds() {
            uint16_t _clockW, _clockH;
            if (_clockBoundsCached) return;
            int16_t x1, y1;
            _tft->getTextBounds("00:00", 0, 0, &x1, &y1, &_clockW, &_clockH);
            _clockX = (_tft->width() - _clockW) / 2;
            _clockBoundsCached = true;
        }
        inline bool _hideField(const Field &f)
            { return editMode && selected==f && !adjustHeld && !blinkState; }
        inline void _clearLine(const int16_t &y,const int16_t &h)
            { _tft->fillRect(3, y, _tft->width() - 8, h, BLACK); }
        inline void _CenteredText(const char* t,const int16_t &y,const int16_t &s,const uint16_t &c) {
            _tft->setTextSize(s);
            _tft->setTextColor(c);
            int16_t x1, y1; uint16_t w, h;
            _tft->getTextBounds(t, 0, 0, &x1, &y1, &w, &h);
            _tft->setCursor((_tft->width() - w) / 2, y);
            _tft->print(t);
        }
        inline void _ProgressBar(int percent) {
            byte x = 40, y = 150;
            _tft->fillRect(x,     y,     42,                   1, WHITE);        // top
            _tft->fillRect(x,     y+1,   1,                    4, WHITE);        // left
            _tft->fillRect(x+41,  y+1,   1,                    4, WHITE);        // right
            _tft->fillRect(x,     y+5,   42,                   1, WHITE);        // bottom
            _tft->fillRect(x+1,   y+1,   (40 * percent) / 100, 4, ST77XX_GREEN); // fill
        }
    public:
        DrawUI() = default;
        void init(Adafruit_ST7735& tft) { _tft = &tft; }
        void Header(const char* t = HEADER_TEXT) {
            _clearLine(HEADER_Y - 5, 15);
            _CenteredText(t, HEADER_Y, HEADER_SIZE, WHITE);
        }
        void Time(const DateTime &t = now) {
            uint8_t h12 = t.hour() % 12;
            if (!h12) h12 = 12;
            char hBuf[3], mBuf[3];
            if (!_hideField(FIELD_HOUR)) sprintf(hBuf, "%02d", h12);
            if (!_hideField(FIELD_MIN))  sprintf(mBuf, "%02d", t.minute());
            const char* ampm = _hideField(FIELD_AMPM) ? "--" : (t.hour() >= 12 ? "PM" : "AM");
            _tft->setTextSize(CLOCK_SIZE);
            _cacheClockBounds();
            _clearLine(CLOCK_Y, 30);
            _tft->setCursor(_clockX, CLOCK_Y);
            _tft->setTextColor(_hideField(FIELD_HOUR) ? RED : clock_color());
            _tft->print(_hideField(FIELD_HOUR) ? "--" : hBuf);
            _tft->setTextColor(WHITE);
            _tft->print(":");
            _tft->setTextColor(_hideField(FIELD_MIN) ? RED : clock_color());
            _tft->print(_hideField(FIELD_MIN) ? "--" : mBuf);
            _clearLine(AMPM_Y, 20);
            _CenteredText(ampm, AMPM_Y, AMPM_SIZE, _hideField(FIELD_AMPM) ? RED : clock_color());
        }
        void Date(const DateTime &t = now) {
            _clearLine(DATE_Y, 22);
            bool hideMo  = _hideField(FIELD_MONTH);
            bool hideDay = _hideField(FIELD_DAY);
            bool hideYr  = _hideField(FIELD_YEAR);
            uint16_t c = ((hideMo || hideDay || hideYr) ? RED : date_color());
            char buf[13];
            char dayBuf[3];
            char yrBuf[5];
            _tft->setTextSize(DATE_SIZE);
            _tft->setCursor(0, DATE_Y);
            if (!hideDay) sprintf(dayBuf, "%02d", t.day());
            if (!hideYr)  sprintf(yrBuf, "%04d", t.year());
            sprintf(buf, "%s %s, %s",
                hideMo ? "---" : pgm_read_ptr(&monthNames[t.month()-1]),
                hideDay ? "--" : dayBuf,
                hideYr ? "----" : yrBuf);
            _CenteredText(buf, DATE_Y, DATE_SIZE, c);
            _CenteredText(pgm_read_ptr(&daysFull[now.dayOfTheWeek()]), DATE_Y + 10, DATE_SIZE, c);
        }
        void Bottom(const char* t) {
            _clearLine(BOTTOM_Y, 12);
            _CenteredText(t, BOTTOM_Y, BOTTOM_SIZE, ST77XX_GREEN);
        }
        void SystemBoot() {
            _CenteredText("Welcome", TITLE_Y, TITLE_SIZE, WHITE);
            _CenteredText("To", TITLE_Y + 18, TITLE_SIZE, WHITE);
            _CenteredText("My Klok", TITLE_Y + 35, TITLE_SIZE, WHITE);
        }
        void FakeLoading() {
            _CenteredText("system loading", 125, 1, RED); 
            const uint8_t barSteps[] = {0, 25, 50, 55, 60, 65, 85, 100};
            const uint16_t delays[]   = {250, 250, 250, 150, 150, 150, 250, 500};
            for (uint16_t i = 0; i < 8; i++) {
                _ProgressBar(barSteps[i]);
                delay(delays[i]);
            }
        }
        void CheckeredBorders(uint16_t fillColor = 0, uint16_t dashColor = 0) {
            static bool swapColors = false;
            if(fillColor == 0) fillColor = pgm_read_word(&colors[clock_color_index]);
            if(dashColor == 0) dashColor = pgm_read_word(&colors[date_color_index]);
            if (swapColors) {
                uint16_t temp = fillColor;
                fillColor = dashColor;
                dashColor = temp;
            }
            swapColors = !swapColors;
            for (int x = 1; x < _tft->width(); x += 4 * 2) {                // top border
                _tft->fillRect(x, 1, 4, 2, dashColor);
                _tft->fillRect(x + 4, 1, 4, 2, fillColor);
            }
            for (int x = 3; x < _tft->width() - 2; x += 4 * 2) {            // header border
                _tft->fillRect(x, 40, 4, 2, dashColor);
                _tft->fillRect(x + 4, 40, 4, 2, fillColor);
            }
            for (int x = 3; x < _tft->width() - 2; x += 4 * 2) {            // bottom text border
                _tft->fillRect(x, 130, 4, 2, dashColor);
                _tft->fillRect(x + 4, 130, 4, 2, fillColor);
            }
            for (int x = 3; x < _tft->width() - 2; x += 4 * 2) {            // bottom border
                _tft->fillRect(x, _tft->height() - 2, 4, 2, dashColor);
                _tft->fillRect(x + 4, _tft->height() - 2, 4, 2, fillColor);
            }
            for (int y = 1; y < _tft->height(); y += 4 * 2) {               // left border
                _tft->fillRect(1, y, 2, 4, dashColor);
                _tft->fillRect(1, y + 4, 2, 4, fillColor);
            }
            for (int y = 3; y < _tft->height(); y += 4 * 2) {               // rightborder
                _tft->fillRect(_tft->width() - 2, y, 2, 4, dashColor);
                _tft->fillRect(_tft->width() - 2, y + 4, 2, 4, fillColor);
            }
        }
        void TextColorChange(bool saveColor = false) {
            clock_color_index = (clock_color_index + 1) % 8;
            date_color_index = (clock_color_index + 1 + random(7)) % 8;
            if(date_color_index == clock_color_index) date_color_index = (date_color_index + 1) % 8;
            CheckeredBorders(pgm_read_word(&colors[clock_color_index]), pgm_read_word(&colors[date_color_index]));
            Time();
            Date();
            if (saveColor) saveColors();
        }
};

DrawUI Draw;