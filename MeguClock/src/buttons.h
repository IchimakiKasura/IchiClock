#pragma once

uint16_t selectPressStart = 0,
         adjustPressTime = 0,
         adjustHoldStart = 0,
         lastSelect = 0,
         lastAdjust = 0;
bool adjustWaitingSecondTap = false,
     selectIgnoreRelease = false;

extern inline void updateFunction(void (*func)(), uint16_t &ms, uint16_t t);

void handleSelectButton()
{
#ifndef CUSTOM_PINS
    bool state = (PIND & (1 << 2)) != 0;
#else
    bool state = digitalRead(BTN_SELECT);
#endif

    static bool lastState = HIGH,
                longHandled = false;

    if (lastState && !state)
    {
        selectPressStart = systemTime;
        longHandled = false;
    }

    if (!state && !longHandled && (systemTime - selectPressStart >= 500))
    {
        longHandled =
            selectIgnoreRelease = true;
        if (Draw.editMode)
        {
            rtc.Save();
            Draw.editMode = false;
            selected = FIELD_HOUR;
            Draw.Header(0);
            Draw.ReDraw();
        }
    }

    if (!lastState && state)
    {
        if (selectIgnoreRelease)
        {
            selectIgnoreRelease = false;
        }
        else if (!longHandled)
        {
            if (Draw.editMode)
            {
                Draw.blinkState = !(Draw.lastBlink = 0);
                if (selected == FIELD_YEAR)
                {
                    rtc.Save();
                    Draw.editMode = false;
                    selected = FIELD_HOUR;
                    Draw.Header(0);
                    Draw.ReDraw();
                }
                else
                {
                    selected = (Field)((selected + 1) % 6);
                    quickBeepStart();
                }
            }
            else
            {
                rtc.Load();
                Draw.Header(1);
                Draw.blinkState = Draw.editMode = true;
                quickBeepStart();
            }
        }
    }
    lastState = state;
}

void applyAdjustment(int dir)
{
    int dim;
    switch (selected)
    {
    case FIELD_HOUR:
        rtc.h = (rtc.h + dir + 24) % 24;
        h_edited = true;
        break;
    case FIELD_MIN:
        rtc.m = (rtc.m + dir + 60) % 60;
        m_edited = true;
        break;
    case FIELD_AMPM:
        rtc.h = (rtc.h + 12) % 24;
        break;
    case FIELD_MONTH:
        rtc.mo = (rtc.mo + dir - 1 + 12) % 12 + 1;
        dim = rtc.s_now.month();
        if (rtc.d > dim)
            rtc.d = dim;
        break;
    case FIELD_DAY:
        dim = rtc.s_now.day();
        rtc.d += dir;
        if (rtc.d < 1)
            rtc.d = dim;
        else if (rtc.d > dim)
            rtc.d = 1;
        break;
    case FIELD_YEAR:
        rtc.y += dir;
        dim = rtc.s_now.year();
        if (rtc.d > dim)
            rtc.d = dim;
        break;
    }
    quickBeepStart();
}

void handleAdjustButton()
{
    static bool lastState = HIGH;
    static uint16_t lastRepeat = 0;

#ifndef CUSTOM_PINS
    bool state = (PIND & (1 << 3));
#else
    bool state = digitalRead(BTN_ADJUST);
#endif

    if (!Draw.editMode)
    {
        if (lastState && !state)
        {
            Draw.TextColorChange(true);
            quickBeepStart();
            adjustHoldStart = systemTime;
        }

        if (!state && (systemTime - adjustHoldStart > 500))
            if (systemTime - lastRepeat >= 120)
            {
                Draw.TextColorChange(true);
                quickBeepStart();
                lastRepeat = systemTime;
            }
        lastState = state;
        return;
    }

    if (lastState && !state)
    {
        if (systemTime - adjustPressTime < 300)
        {
            adjustWaitingSecondTap = false;
            applyAdjustment(-1);
        }
        else
        {
            adjustWaitingSecondTap = true;
            adjustPressTime = systemTime;
        }

        adjustHoldStart = systemTime;
        lastRepeat = systemTime;
    }

    if (!state && (systemTime - adjustHoldStart > 500))
        if (systemTime - lastRepeat >= 120)
        {
            applyAdjustment(1);
            lastRepeat = systemTime;
        }
    

    if (adjustWaitingSecondTap && state && (systemTime - adjustPressTime >= 300))
    {
        adjustWaitingSecondTap = false;
        applyAdjustment(1);
    }

    lastState = state;
}
void handleBothButtons()
{
    if (jingleState.playing)
        return;

#ifndef CUSTOM_PINS
    bool sel = !(PIND & (1 << 2));
    bool adj = !(PIND & (1 << 3));
#else
    bool sel = digitalRead(BTN_SELECT) == LOW;
    bool adj = digitalRead(BTN_ADJUST) == LOW;
#endif

    if (sel)
        lastSelect = systemTime;
    if (adj)
        lastAdjust = systemTime;

    if (sel && adj && abs((long)(lastSelect - lastAdjust)) < 200 && !Draw.editMode)
    {
        Draw.Bottom("KONOSUBA!!");
        Jingle(CHIISANA_BOKENSHA_JINGLE, false, true, 150);
        lastSelect = lastAdjust = 0;
        return;
    }
    handleSelectButton();
    handleAdjustButton();
}