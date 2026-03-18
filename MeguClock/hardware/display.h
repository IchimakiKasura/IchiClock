#pragma once
#include <MeguClock_ST7735.h>
#include "rtc.h"
#include "../assets/meguminLogo.h"
#include "../config/ColorConfig.h"

class DrawUI {
private:
    MeguClock_ST7735* _tft;
    int16_t _clockX;
    bool _clockBoundsCached = false,
         _timeDrawnErr = false,
         _dateDrawnErr = false;

    void _cacheClockBounds();
    inline bool _hideField(const Field &f);
    inline void _clearLine(const int16_t &y, const int16_t &h);
    template<typename args>
    void _CenteredText(const args t,const int16_t &y,const int16_t &s,const uint16_t &c);
    void _Logo(int x, int y, uint8_t scale);

public:
    bool blinkState = true;
    uint32_t lastBlink = 0;
    DrawUI() = default;
    void init(uint8_t cs, uint8_t dc, uint8_t rst);
    void Header(byte t);
    void Time(const DateTime &t = now);
    void Date(const DateTime &t = now);
    void Bottom(const char* t);
    void SystemBoot();
    void FakeLoading();
    void CheckeredBorders(uint16_t fillColor = 0, uint16_t dashColor = 0);
    void TextColorChange(bool saveColor = false);
    void ReDraw(const DateTime &t = now);
    void bg() { _tft->fillScreen(0x40A3); }
} Draw;

void DrawUI::_cacheClockBounds() {
    if (_clockBoundsCached) return;

    uint16_t _clockW, _clockH;
    int16_t x1, y1;

    _tft->getTextBounds(F("00:00"), 0, 0, &x1, &y1, &_clockW, &_clockH);

    _clockX = (128 - _clockW) / 2;
    _clockBoundsCached = true;
}

inline bool DrawUI::_hideField(const Field &f) {
    return editMode && selected==f && !adjustHeld && !blinkState;
}

inline void DrawUI::_clearLine(const int16_t &y, const int16_t &h) {
    _tft->fillRect(3, y, 122, h, 0x40A3);
}

template<typename args>
void DrawUI::_CenteredText(const args t,const int16_t &y,const int16_t &s,const uint16_t &c) {
    int16_t x1, y1;
    uint16_t w, h;

    _tft->setTextSize(s);
    _tft->setTextColor(c);
    _tft->getTextBounds(t, 0, 0, &x1, &y1, &w, &h);
    _tft->setCursor((128 - w) / 2, y);
    _tft->print(t);
}

void DrawUI::_Logo(int x, int y, uint8_t scale) {
    for (uint16_t i = 0; i < sizeof(rects)/sizeof(Rect); i++) {
        Rect r;

        memcpy_P(&r, &rects[i], sizeof r);

        uint16_t color = pgm_read_word(&megumin_colors[r.colorId]);

        _tft->fillRect(x+((r.x-14)+r.x_repeat)*scale,y+(r.y-13)*scale,r.w*scale,r.h*scale,color);
        _tft->fillRect(x+(r.x-14)*scale,y+((r.y-13)+r.y_repeat)*scale,r.w*scale,r.h*scale,color);
    }
}

void DrawUI::init(uint8_t cs, uint8_t dc, uint8_t rst) {
    _tft = new MeguClock_ST7735(cs, dc, rst);
    _tft->initR();
    _tft->fillScreen(0x40A3);
}

void DrawUI::Header(byte t) {
    static bool _l = false;
    _clearLine(10, 20);

    if(t == 0) {
        _CenteredText(F("MEGUMIN"), 10, HEADER_SIZE, WHITE);
        _CenteredText(F("CLOCK"),   23, HEADER_SIZE, WHITE);
    } else {
        _CenteredText(F("CONFIGURE"), 10, HEADER_SIZE, WHITE);
        _CenteredText(F("TIME/DATE"), 23, HEADER_SIZE, WHITE);
    }
    
    _Logo(19,22,1); _Logo(109,22,1);
}

void DrawUI::Time(const DateTime &t = now) {
    _clearLine(50, 30);

    char Buf[3];
    uint8_t h12 = mRTC.getCurrentHour(t);
    const char* ampm = mRTC.isRTC ? _hideField(FIELD_AMPM) ? "--" : mRTC.getCurrentMidday(t) : "??";

    if (!h12) h12 = 12;
    
    _tft->setTextSize(CLOCK_SIZE);
    _cacheClockBounds();
    _tft->setCursor(_clockX, 50);

    if(mRTC.isRTC)
    {
        if (!_hideField(FIELD_HOUR)) sprintf(Buf, "%02d", h12);
        _tft->setTextColor(_hideField(FIELD_HOUR) ? RED : M_COLORS::ClockColor());
        _tft->print(_hideField(FIELD_HOUR) ? "--" : Buf);

        _tft->setTextColor(WHITE);
        _tft->print(":");
        
        if (!_hideField(FIELD_MIN))  sprintf(Buf, "%02d", t.minute());
        _tft->setTextColor(_hideField(FIELD_MIN) ? RED : M_COLORS::ClockColor());
        _tft->print(_hideField(FIELD_MIN) ? "--" : Buf);

    } else _tft->print(F("??:??"));
    
    _clearLine(80, 20);
    _CenteredText(ampm, 80, AMPM_SIZE, _hideField(FIELD_AMPM) ? RED : M_COLORS::ClockColor());
    // _Logo(64,80,3);
}

void DrawUI::Date(const DateTime &t = now) {
    _clearLine(105, 22);

    char Buf[10];
    bool hideMo  = _hideField(FIELD_MONTH);
    bool hideDay = _hideField(FIELD_DAY);
    bool hideYr  = _hideField(FIELD_YEAR);
    uint16_t c = ((hideMo || hideDay || hideYr) ? RED : M_COLORS::DateColor());

    _tft->setTextSize(DATE_SIZE);
    _tft->setTextColor(c);
    _tft->setCursor(30, 105);

    if(mRTC.isRTC)
    {
        _tft->print(hideMo ? "--- " : strcpy_P(Buf, monthNames[t.month()-1]));

        if (!hideDay) sprintf(Buf, "%02d, ", t.day());
        _tft->print(hideDay ? "--, " : Buf);

        if (!hideYr) sprintf(Buf, "%04d", t.year());
        _tft->print(hideYr ? "----" : Buf);

        _CenteredText(strcpy_P(Buf, daysFull[now.dayOfTheWeek()]), 105 + 10, DATE_SIZE, c);
    } else _tft->print(F("  RtcError"));
}

void DrawUI::Bottom(const char* t) {
    _clearLine(140, 12);
    _CenteredText(t, 140, BOTTOM_SIZE, GREEN);
}

void DrawUI::SystemBoot() {
    _CenteredText(F("MEGU-CLOCK"), 90, 2, WHITE);
    _Logo(64,55,3);
}

void DrawUI::FakeLoading() {
    const uint8_t barSteps[] = {0, 25, 50, 55, 60, 65, 85, 100};
    const uint16_t delays[] = {250, 250, 250, 150, 150, 150, 250, 500};

    _CenteredText(F("system loading"), 125, 1, YELLOW); 

    _tft->fillRect(40, 145, 42, 6, WHITE);
    _tft->fillRect(41, 146, 40, 4, 0x40A3);

    for (byte i = 0; i < 8; i++) {
        _tft->fillRect(41, 146, (40*barSteps[i])/100, 4, YELLOW);
        delay(delays[i]);
    }
}

void DrawUI::CheckeredBorders(uint16_t fillColor = 0, uint16_t dashColor = 0) {
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
        for (int x = 1; x < 128 - 1; x += 8) {
            _tft->fillRect(x, hY[i], 4, 2, dashColor);
            _tft->fillRect(x + 4, hY[i], 4, 2, fillColor);
        }
    }

    for (int y = 1; y < 160; y += 8) {
        _tft->fillRect(1, y, 2, 4, dashColor);
        _tft->fillRect(1, y + 4, 2, 4, fillColor);
        _tft->fillRect(126, y+2, 2, 4, dashColor);
        _tft->fillRect(126, y+6, 2, 4, fillColor);
    }
}

void DrawUI::TextColorChange(bool saveColor = false) {
    M_COLORS::ClockColor_index = (M_COLORS::ClockColor_index + 1) % 8;
    M_COLORS::DateColor_index = (M_COLORS::ClockColor_index + 1 + rand(7)) % 8;

    if(M_COLORS::DateColor_index == M_COLORS::ClockColor_index)  
        M_COLORS::DateColor_index = (M_COLORS::DateColor_index + 1) % 8;

    CheckeredBorders(M_COLORS::ClockColor(), M_COLORS::DateColor());
    
    Time();
    Date();

    if (saveColor) M_COLORS::Save();
}

void DrawUI::ReDraw(const DateTime &t = now) {
    Time(t);
    Date(t);
}