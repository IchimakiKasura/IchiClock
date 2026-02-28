#include <EEPROM.h>
#include <RTClib.h>
#include <Adafruit_ST7735.h>

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

#include "customColor.h"
#include "ClockConfig.h"
#include "display.h"
#include "buzzer.h"
#include "buttons.h"
#include "utils.h"
#include "alarm.h"

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
            if(now.hour() != h && selected != FIELD_HOUR && !h_edited) h = now.hour();
            if(now.minute() != m && selected != FIELD_MIN && !m_edited) m = now.minute();
            DateTime temp(y, mo, d, h, m, now.second());
            refreshScreen(temp);
        }
        return;
    }
    if (now.minute() != lastTime.minute() || now.hour() != lastTime.hour() || now.day() != lastTime.day()) {
        refreshScreen();
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