#pragma once
#include "rtc.h"

unsigned long selectPressStart = 0,
              adjustPressTime  = 0,
              adjustHoldStart  = 0;
bool          adjustWaitingSecondTap = false,
              selectIgnoreRelease = false;
unsigned long lastSelect = 0,
              lastAdjust = 0;

void handleSelectButton() {
    bool state = digitalRead(BTN_SELECT);
    static bool lastState = HIGH,
                longHandled = false;
    unsigned long nowMs = millis();

    if (lastState && !state) {
        selectPressStart = nowMs;
        longHandled = false;
    }

    if (!state && !longHandled && (nowMs - selectPressStart >= 500)) {
        longHandled =
        selectIgnoreRelease = true;
        if (editMode) M_RTC::Save();
    }

    if (!lastState && state) {
        if (selectIgnoreRelease) {
            selectIgnoreRelease = false;
        } 
        else if (!longHandled) {
            if (editMode) {
                Draw.blinkState = !(Draw.lastBlink = 0);
                if (selected == FIELD_YEAR) M_RTC::Save();
                else {
                    selected = (Field)((selected + 1) % 6);
                    quickBeepStart();
                }
            } 
            else {
                M_RTC::Load();
                Draw.Header(1);
                Draw.blinkState = editMode = true;
                quickBeepStart();
            }
        }
    }
    lastState = state;
}

void applyAdjustment(int dir) {
    int dim;
    switch (selected) {
        case FIELD_HOUR:  M_RTC::h = (M_RTC::h + dir + 24) % 24; h_edited = true; break;
        case FIELD_MIN:   M_RTC::m = (M_RTC::m + dir + 60) % 60; m_edited = true; break;
        case FIELD_AMPM:  M_RTC::h = (M_RTC::h + 12) % 24; break;
        case FIELD_MONTH:
            M_RTC::mo = (M_RTC::mo + dir - 1 + 12) % 12 + 1;
            dim = M_RTC::getDay();
            if (M_RTC::d > dim)
                M_RTC::d = dim;
            break;
        case FIELD_DAY:
            dim = M_RTC::getDay();
            M_RTC::d += dir;
            if (M_RTC::d < 1)
                M_RTC::d = dim;
            else if (M_RTC::d > dim)
                M_RTC::d = 1;
            break;
        case FIELD_YEAR:
            M_RTC::y += dir;
            dim = M_RTC::getDay();
            if (M_RTC::d > dim)
                M_RTC::d = dim;
            break;
    }
    quickBeepStart();
}

void handleAdjustButton() {
    static bool lastState = HIGH;
    static unsigned long lastRepeat = 0;
    bool state = digitalRead(BTN_ADJUST);
    unsigned long nowMs = millis();

    if (!editMode) {
        if (lastState && !state) {
            Draw.TextColorChange(true);
            quickBeepStart();
            adjustHoldStart = nowMs;
        }

        if (!state && (nowMs - adjustHoldStart > 500)) {
            if (nowMs - lastRepeat >= 120) {
                Draw.TextColorChange(true);
                quickBeepStart();
                lastRepeat = nowMs;
            }
        }
        lastState = state;
        return;
    }

    if (lastState && !state) {
        if (nowMs - adjustPressTime < 300) {
            adjustWaitingSecondTap = false;
            applyAdjustment(-1);
        } else {
            adjustWaitingSecondTap = true;
            adjustPressTime = nowMs;
        }
        
        adjustHoldStart = nowMs;
        lastRepeat = nowMs;
    }

    if (!state && (nowMs - adjustHoldStart > 500)) {
        if (nowMs - lastRepeat >= 120) {
            applyAdjustment(1);
            lastRepeat = nowMs;
        }
    }

    if (adjustWaitingSecondTap && state && (nowMs - adjustPressTime >= 300)) {
        adjustWaitingSecondTap = false;
        applyAdjustment(1);
    }

    lastState = state;
}
void handleBothButtons() {
    if(jingleState.playing) return;

    unsigned long now = millis();
    bool sel = digitalRead(BTN_SELECT) == LOW;
    bool adj = digitalRead(BTN_ADJUST) == LOW;

    if (sel) lastSelect = now;
    if (adj) lastAdjust = now;

    if (sel && adj && abs((long)(lastSelect - lastAdjust)) < 200 && !editMode) {
        Draw.Bottom("KONOSUBA!!");
        Jingle(CHIISANA_BOKENSHA_JINGLE, false, true, 150);
        lastSelect = lastAdjust = 0;
        return;
    }
    handleSelectButton();
    handleAdjustButton();
}