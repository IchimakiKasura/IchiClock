/**************************************************************************
    MEGUMIN CLOCK (MEGUCLOCK / MEGU-CLOCK)

    A custom digital clock built on Arduino, inspired by Megumin.

    Peak optimization shit.

    Features:
        - Fake loading because god knows its just a simple mcu.
        - Customizable uh colors
        - clock can be adjusted using 2 buttons
        - added shitty buzzer to uh for alarm idfk.
        - alarms are preloaded/hardcoded? idfk lmao
            - 6:00 AM `FNAF`
            - 10:00 AM `Samsung Morning Flower`
            - 12:00 PM `Winchester Chime`
            - 3:00 PM `Winchester Chime`
            - 10:00 PM `Short ahh Twinkle star smh`
        - has random ahh messages every 10 seconds at the bottom. *(can be customized through code ofc)*
        - has an epic border.
        - Uses only 65%? of program storage space whaaat? idfk.

    - Includes modified libraries (Reduced Flash Storage)
        - MeguClock_DS3231 (Originally from Adafruit RTCLib)
        - MeguClock_ST7735 (Originally from Adafruit ST7735 & ST7789)

    Author: Ichimaki Kasura
**************************************************************************/
#include <Arduino.h>
#include "src/includes.h"

inline void updateFunction(void (*func)(), uint16_t &ms, uint16_t t)
{
    if (systemTime - ms < t) return;
    func();
    ms = systemTime;
}

void timeUpdate()
{
    if (!Draw.editMode)
    {
        if (rtc.s_now.minute() != lastTime.minute() ||
            rtc.s_now.hour() != lastTime.hour() ||
            rtc.s_now.day() != lastTime.day())
        {

            Draw.ReDraw();
            lastTime = rtc.s_now;
            quickBeepStart();
        }
        return;
    }

    updateFunction([]()
    {
        Draw.blinkState = !Draw.blinkState;

        if (selected != FIELD_HOUR && !h_edited && rtc.s_now.hour() != rtc.h)
            rtc.h = rtc.s_now.hour();

        if (selected != FIELD_MIN && !m_edited && rtc.s_now.minute() != rtc.m)
            rtc.m = rtc.s_now.minute();

        Draw.ReDraw({rtc.y, rtc.mo, rtc.d, rtc.h, rtc.m, rtc.s_now.second()});
    }, Draw.lastBlink, 700);
}

void bottomTextUpdate()
{
    if (jingleState.playing)
        return;

    uint8_t newIndex;
    do
    {
        newIndex = rand(46);
    } while (newIndex == lastBottomIndex && 46 > 1);

    lastBottomIndex = newIndex;
    bottomIndex = newIndex;

    Draw.Bottom(strcpy_P(bottomTextBuffer, bottomMessages[newIndex]));
}

void initialize()
{
    init();

    rtc.init();

    // rtc.sync();

// #define RESET_COLORS
#ifdef RESET_COLORS
    EEPROM.update(0, ClockColor_index);
    EEPROM.update(1, DateColor_index);
#endif
       
#ifndef CUSTOM_PINS
    Draw.init(2, 1, 0);
#else
    Draw.init(TFT_CS, TFT_DC, TFT_RST);
#endif
    M_COLORS::Load();
    Draw.SystemBoot();

    Jingle(CHIISANA_BOKENSHA_JINGLE, true);
    delay(100);

    Draw.FakeLoading();

#ifndef CUSTOM_PINS
    DDRD = 0x20;
    PORTD = 0x0C;
#else
    pinMode(BTN_SELECT, INPUT_PULLUP);
    pinMode(BTN_ADJUST, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);
#endif

    Draw.bg();
}

int main()
{
    initialize();

    Draw.Header(0);
    Draw.Bottom(strcpy_P(bottomTextBuffer, bottomMessages[rand(4)]));

    while (true)
    {
        systemTime = millis();

        updateJingle();
        updateFunction([](){Draw.CheckeredBorders();}, borderLastUpdate, 500);
        updateFunction(bottomTextUpdate, lastBottomUpdate, 10000);

        if (rtc.rtcConnected())
        {
            rtc.updateRTC();

#ifndef CUSTOM_PINS
            adjustHeld = !(PIND & (1 << 3));
#else
            adjustHeld = digitalRead(BTN_ADJUST);
#endif

            timeUpdate();

            updateFunction(handleBothButtons, lastCheck, 15);

            CheckAlarm(rtc.s_now.hour(), rtc.s_now.minute(), rtc.s_now.second());
        }

        if (rtc.rtcConnected() && !lastRTCState)
            Draw.ReDraw();

        lastRTCState = rtc.rtcConnected();
    }

    return 0;
}