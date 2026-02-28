#include <EEPROM.h>
#include <RTClib.h>
#include <Adafruit_ST7735.h>
#include "utils/rand.h"
#include "config/ClockConfig.h"
#include "config/ColorConfig.h"

#define TFT_CS   10
#define TFT_RST  8
#define TFT_DC   9

#define BTN_SELECT 2
#define BTN_ADJUST 3
#define BUZZER     5

// ONLY SET TO TRUE IF NEEDS TO SYNC TO THE TIME
#define SETUP_TIME false

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);
RTC_DS3231 rtc;

#include "hardware/display.h"
#include "hardware/buzzer.h"
#include "hardware/buttons.h"
#include "hardware/rtc.h"
#include "features/alarm.h"

bool dones = false;

void setup() {
    initialize();
    Draw.Header(0);
    Draw.Bottom(strcpy_P(bottomTextBuffer, bottomMessages[rand(46)]));
}

void loop() {
    updateJingle();
    now = rtc.now();
    
    adjustHeld = digitalRead(BTN_ADJUST) == LOW;

    timeUpdate();
    bottomTextUpdate();
    borderUpdate();

    if(millis() - lastCheck >= 20) {
        lastCheck = millis();
        handleBothButtons();
    };

    CheckAlarm(now.hour(), now.minute(), now.second());
}

void timeUpdate() {
    if(editMode){
        if(millis() - Draw.lastBlink >= 700){
            Draw.lastBlink = millis();
            Draw.blinkState = !Draw.blinkState;
            if(now.hour() != M_RTC::h && selected != FIELD_HOUR && !h_edited) M_RTC::h = now.hour();
            if(now.minute() != M_RTC::m && selected != FIELD_MIN && !m_edited) M_RTC::m = now.minute();
            DateTime temp(M_RTC::y, M_RTC::mo, M_RTC::d, M_RTC::h, M_RTC::m, now.second());
            Draw.ReDraw(temp);
        }
        return;
    }
    if (now.minute() != lastTime.minute() || now.hour() != lastTime.hour() || now.day() != lastTime.day()) {
        Draw.ReDraw();
        lastTime = now;
        quickBeepStart();
    }
}
void bottomTextUpdate() {
    if(jingleState.playing) return;
    if (millis() - lastBottomUpdate >= 10000) {
        lastBottomUpdate = millis();
        uint8_t newIndex;
        do {
            newIndex = rand(46);
        } while (newIndex == lastBottomIndex && 46 > 1);
        lastBottomIndex = newIndex;
        bottomIndex = newIndex;
        Draw.Bottom(strcpy_P(bottomTextBuffer, bottomMessages[rand(46)]));
    }
}
void borderUpdate() {
    if(millis() - borderLastUpdate >= 500) {
        borderLastUpdate = millis();
        Draw.CheckeredBorders();
    }
}
void initialize() {             // goofy ahh init
    tft.initR(INITR_GREENTAB);
    tft.setRotation(0);
    tft.fillScreen(0x18A8);
    Draw.init(tft);
    M_COLORS::  Load();
    Draw.SystemBoot();
    Jingle(CHIISANA_BOKENSHA_JINGLE, true);
    delay(100);
    Draw.FakeLoading();
    Wire.begin();
    rtc.begin();
    pinMode(BTN_SELECT, INPUT_PULLUP);
    pinMode(BTN_ADJUST, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);
    tft.fillScreen(0x18A8);

    if(SETUP_TIME) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}