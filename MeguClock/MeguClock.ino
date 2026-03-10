#include <Arduino.h>
#include "utils/vars.h"

int main() {
    initialize();

    Draw.Header(0);
    Draw.Bottom(strcpy_P(bottomTextBuffer, bottomMessages[rand(45)]));

    while(true) {
        systemTime = millis();
        
        updateJingle();

        updateFunction([](){Draw.CheckeredBorders();}, borderLastUpdate, 500);
        updateFunction(bottomTextUpdate, lastBottomUpdate, 10000);

        mRTC.isRTC = mRTC.rtcConnected();

        if (mRTC.isRTC) {
            now = mRTC.rtc.now();
            
            #ifndef CUSTOM_PINS
                adjustHeld = !(PIND & (1 << 3));
            #else
                adjustHeld = digitalRead(BTN_ADJUST);
            #endif

            timeUpdate();

            updateFunction(handleBothButtons, lastCheck, 20);

            CheckAlarm(now.hour(), now.minute(), now.second());
        }

        if (mRTC.isRTC && !lastRTCState) {
            Draw.ReDraw();
        }
        else if (!mRTC.isRTC && lastRTCState) {
            DateTime zero(0,0,0,0,0,0);
            Draw.ReDraw(zero);
        }

        lastRTCState = mRTC.isRTC;
    }
}

void timeUpdate() {
    if (!editMode) {
        if (now.minute() != lastTime.minute() ||
            now.hour()   != lastTime.hour()   ||
            now.day()    != lastTime.day()) {

            Draw.ReDraw();
            lastTime = now;
            quickBeepStart();
        }
        return;
    }

    updateFunction([](){
        Draw.blinkState = !Draw.blinkState;

        if (selected != FIELD_HOUR && !h_edited && now.hour() != mRTC.h)
            mRTC.h = now.hour();

        if (selected != FIELD_MIN && !m_edited && now.minute() != mRTC.m)
            mRTC.m = now.minute();

        DateTime temp(mRTC.y, mRTC.mo, mRTC.d, mRTC.h, mRTC.m, now.second());
        Draw.ReDraw(temp);
    }, Draw.lastBlink, 700);
}

void bottomTextUpdate() {
    if (jingleState.playing) return;

    uint8_t newIndex;
    do {
        newIndex = rand(46);
    } while (newIndex == lastBottomIndex && 46 > 1);

    lastBottomIndex = newIndex;
    bottomIndex = newIndex;

    Draw.Bottom(strcpy_P(bottomTextBuffer, bottomMessages[newIndex]));
}

void initialize() {
    init();

#ifndef CUSTOM_PINS
    Draw.init(2, 1, 0);
#else
    Draw.init(TFT_CS, TFT_DC, TFT_RST);
    digitalWrite(TFT_RST, HIGH);
#endif
    M_COLORS::Load();
    Draw.SystemBoot();


    Jingle(CHIISANA_BOKENSHA_JINGLE, true);
    delay(100);

    Draw.FakeLoading();

    mRTC.init();

#ifndef CUSTOM_PINS
    // some brainfuck shit idfk, it saves like 40bytes?
    // rest pins are input | D5 output
    DDRD = 0x20;    // (1<<5) b00100000
    PORTD = 0x0C;   // (1<<2)|(1<<3) b00001100
#else
    pinMode(BTN_SELECT, INPUT_PULLUP);
    pinMode(BTN_ADJUST, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);
#endif

    Draw.bg();

    // mRTC.sync();
}

inline void updateFunction(void (*func)(), unsigned long &ms, int16_t t) {
    if (systemTime - ms < t) return;
    func();
    ms = systemTime;
}