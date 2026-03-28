#pragma once
#include <MeguClock_ST7735.h>
#include <MeguClock_DS3231.h>
#include "assets/meguminLogo.h"
#include "config/ColorConfig.hpp"

enum Field
{
    FIELD_HOUR,
    FIELD_MIN,
    FIELD_AMPM,
    FIELD_MONTH,
    FIELD_DAY,
    FIELD_YEAR
};

Field selected = FIELD_HOUR;

class DrawUI
{
private:
    MeguClock_ST7735 *_tft;
    uint8_t _clockX;
    bool _clockBoundsCached = false,
         _timeDrawnErr = false,
         _dateDrawnErr = false;

    void _cacheClockBounds();
    inline bool _hideField(const Field &f);
    inline void _clearLine(const uint8_t &y, const uint8_t &h);
    template <typename T>
    void _CenteredText(const T str, const uint8_t &y, const uint8_t &size, const uint16_t &color);
    void _Logo(uint8_t x, uint8_t y, uint8_t scale);

public:
    bool blinkState = true,
         editMode = false;
    uint16_t lastBlink = 0;
    DrawUI() = default;
    void init(uint8_t cs, uint8_t dc, uint8_t rst);
    void Header(boolean t);
    void Time(const DateTime &t);
    void Date(const DateTime &t);
    void Bottom(const char *str);
    void SystemBoot();
    void FakeLoading();
    void CheckeredBorders(uint16_t fill, uint16_t dash);
    void TextColorChange(boolean saveColor);
    void ReDraw(const DateTime &t);
    void bg() { _tft->fillScreen(0x40A3); }
} Draw;

void DrawUI::_cacheClockBounds()
{
    if (_clockBoundsCached)
        return;

    uint8_t _clockW, _clockH;
    int16_t x1, y1;

    _tft->getTextBounds(F("00:00"), 0, 0, &x1, &y1, &_clockW, &_clockH);

    _clockX = (128 - _clockW) / 2;
    _clockBoundsCached = true;
}

inline bool DrawUI::_hideField(const Field &f)
{
    return Draw.editMode && selected == f && !adjustHeld && !blinkState;
}

inline void DrawUI::_clearLine(const uint8_t &y, const uint8_t &h)
{
    _tft->fillRect(3, y, 122, h, 0x40A3);
}

template <typename T>
void DrawUI::_CenteredText(const T t, const uint8_t &y, const uint8_t &s, const uint16_t &c)
{
    int16_t x1, y1;
    uint8_t w, h;

    _tft->setTextSize(s);
    _tft->setTextColor(c);
    _tft->getTextBounds(t, 0, 0, &x1, &y1, &w, &h);
    _tft->setCursor((128 - w) / 2, y);
    _tft->print(t);
}

void DrawUI::_Logo(uint8_t x, uint8_t y, uint8_t scale)
{
    for (uint8_t i = 0; i < 62; i++)
    {
        Rect r;

        memcpy_P(&r, &meguLogo[i], sizeof r);

        uint16_t color = pgm_read_word(&megumin_colors[r.colorId]);

        _tft->fillRect(
            x + ((r.x - 14) + r.x_repeat) * scale,
            y + (r.y - 13) * scale,
            r.w * scale,
            r.h * scale,
            color);
        _tft->fillRect(
            x + (r.x - 14) * scale,
            y + ((r.y - 13) + r.y_repeat) * scale,
            r.w * scale,
            r.h * scale,
            color);
    }
}

void DrawUI::init(uint8_t cs, uint8_t dc, uint8_t rst)
{
    _tft = new MeguClock_ST7735(cs, dc, rst);
    _tft->init();
    bg();
}

void DrawUI::Header(boolean t)
{
    static bool _l = false;
    _clearLine(10, 20);

    if (t == 0)
    {
        _CenteredText(F("MEGUMIN"), 10, HEADER_SIZE, WHITE);
        _CenteredText(F("CLOCK"), 23, HEADER_SIZE, WHITE);
    }
    else
    {
        _CenteredText(F("CONFIGURE"), 10, HEADER_SIZE, WHITE);
        _CenteredText(F("TIME/DATE"), 23, HEADER_SIZE, WHITE);
    }

    _Logo(19, 22, 1);
    _Logo(109, 22, 1);
}

void DrawUI::Time(const DateTime &t)
{
    _clearLine(50, 30);

    char Buf[3];
    const char *ampm = rtc.rtcConnected() ? _hideField(FIELD_AMPM) ? "--" : t.midday() : "??";

    _tft->setTextSize(CLOCK_SIZE);
    _cacheClockBounds();
    _tft->setCursor(_clockX, 50);

    if (rtc.rtcConnected())
    {
        sprintf(Buf, "%02d", t.twelveHour());
        _tft->setTextColor(_hideField(FIELD_HOUR) ? RED : M_COLORS::ClockColor());
        _tft->print(_hideField(FIELD_HOUR) ? "--" : Buf);

        _tft->setTextColor(WHITE);
        _tft->print(":");

        sprintf(Buf, "%02d", t.minute());
        _tft->setTextColor(_hideField(FIELD_MIN) ? RED : M_COLORS::ClockColor());
        _tft->print(_hideField(FIELD_MIN) ? "--" : Buf);
    }
    else
        _tft->print(F("??:??"));

    _clearLine(80, 20);
    _CenteredText(ampm, 80, AMPM_SIZE, _hideField(FIELD_AMPM) ? RED : M_COLORS::ClockColor());
    // _Logo(64,80,3);
}

void DrawUI::Date(const DateTime &t)
{
    _clearLine(105, 22);

    char Buf[10];
    bool hideMo = _hideField(FIELD_MONTH);
    bool hideDay = _hideField(FIELD_DAY);
    bool hideYr = _hideField(FIELD_YEAR);
    uint16_t c = ((hideMo || hideDay || hideYr) ? RED : M_COLORS::DateColor());

    _tft->setTextSize(DATE_SIZE);
    _tft->setTextColor(c);
    _tft->setCursor(30, 105);

    if (rtc.rtcConnected())
    {
        sprintf(Buf, "%03s ", t.monthName());
        _tft->print(hideMo ? "--- " : Buf);

        if (!hideDay)
            sprintf(Buf, "%02d, ", t.day());
        _tft->print(hideDay ? "--, " : Buf);

        if (!hideYr)
            sprintf(Buf, "%04d", t.year());
        _tft->print(hideYr ? "----" : Buf);

        sprintf(Buf, "%s", t.dayOfTheWeekName());
        _CenteredText(Buf, 105 + 10, DATE_SIZE, c);
    }
    else
        _tft->print(F("  RtcError"));
}

void DrawUI::Bottom(const char *t)
{
    _clearLine(140, 12);
    _CenteredText(t, 140, BOTTOM_SIZE, GREEN);
}

void DrawUI::SystemBoot()
{
    _CenteredText(F("MEGU-CLOCK"), 90, 2, WHITE);
    _Logo(64, 55, 3);
}

void DrawUI::FakeLoading()
{
    static const uint8_t barSteps[] = {0, 25, 50, 55, 60, 65, 85, 100};
    static const uint16_t delays[] = {250, 250, 250, 150, 150, 150, 250, 500};

    _CenteredText(F("system loading"), 125, 1, YELLOW);

    _tft->fillRect(40, 145, 42, 6, WHITE);
    _tft->fillRect(41, 146, 40, 4, 0x40A3);

    for (byte i = 0; i < 8; i++)
    {
        _tft->fillRect(41, 146, (40 * barSteps[i]) / 100, 4, YELLOW);
        delay(delays[i]);
    }
}

void DrawUI::CheckeredBorders(uint16_t fillColor = M_COLORS::ClockColor(), uint16_t dashColor = M_COLORS::DateColor())
{
    uint16_t hY[] = {1, 38, 128, 158};
    static bool swapColors = false;

    if (swapColors)
    {
        uint16_t temp = fillColor;
        fillColor = dashColor;
        dashColor = temp;
    }

    swapColors = !swapColors;

    for (int i = 0; i < 4; i++)
        for (int x = 1; x < 127; x += 8)
            _tft->fillRect(x, hY[i], 4, 2, dashColor),
            _tft->fillRect(x + 4, hY[i], 4, 2, fillColor);

    for (int y = 1; y < 160; y += 2)
        for (int i = 0, x = 1; i < 2; i++, x = 126)
            _tft->fillRect(x, y, 2, 4, (y % 8 < 4) ? dashColor : fillColor);
}

void DrawUI::ReDraw(const DateTime &t = rtc.s_now)
{
    Time(t);
    Date(t);
}

void DrawUI::TextColorChange(boolean saveColor = false)
{
    M_COLORS::ClockColor_index = (M_COLORS::ClockColor_index + 1) % 8;
    M_COLORS::DateColor_index = (M_COLORS::ClockColor_index + 1 + rand(7)) % 8;

    if (M_COLORS::DateColor_index == M_COLORS::ClockColor_index)
        M_COLORS::DateColor_index = (M_COLORS::DateColor_index + 1) % 8;

    CheckeredBorders(M_COLORS::ClockColor(), M_COLORS::DateColor());

    ReDraw();

    if (saveColor)
        M_COLORS::Save();
}