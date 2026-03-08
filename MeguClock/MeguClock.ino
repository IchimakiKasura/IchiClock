#include "utils/rand.h"
#include "utils/vars.h"
#include "config/ClockConfig.h"
#include "hardware/display.h"
#include "hardware/buzzer.h"
#include "hardware/buttons.h"
#include "features/alarm.h"

void setup() {
  initialize();
  Draw.Header(0);
  Draw.Bottom(strcpy_P(bottomTextBuffer, bottomMessages[rand(45)]));
}

void loop() {
  updateJingle();

  mRTC.isRTC = mRTC.rtcConnected();

  if (mRTC.isRTC && !lastRTCState) {
      Draw.ReDraw();
  }
  else if (!mRTC.isRTC && lastRTCState) {
      DateTime zero(0,0,0,0,0,0);
      Draw.ReDraw(zero);
  }

  if (mRTC.isRTC) {
      now = mRTC.rtc.now();
      adjustHeld = digitalRead(BTN_ADJUST) == LOW;

      timeUpdate();

      if (millis() - lastCheck >= 20) {
          lastCheck = millis();
          handleBothButtons();
      }

      CheckAlarm(now.hour(), now.minute(), now.second());
  }

  lastRTCState = mRTC.isRTC;

  bottomTextUpdate();
  borderUpdate();
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

    if (millis() - Draw.lastBlink < 700) return;

    Draw.lastBlink = millis();
    Draw.blinkState = !Draw.blinkState;

    if (selected != FIELD_HOUR && !h_edited && now.hour() != mRTC.h)
        mRTC.h = now.hour();

    if (selected != FIELD_MIN && !m_edited && now.minute() != mRTC.m)
        mRTC.m = now.minute();

    DateTime temp(mRTC.y, mRTC.mo, mRTC.d, mRTC.h, mRTC.m, now.second());
    Draw.ReDraw(temp);
}

void bottomTextUpdate() {
    if (jingleState.playing) return;
    if (millis() - lastBottomUpdate < 10000) return;

    lastBottomUpdate = millis();

    uint8_t newIndex;
    do {
        newIndex = rand(46);
    } while (newIndex == lastBottomIndex && 46 > 1);

    lastBottomIndex = newIndex;
    bottomIndex = newIndex;

    Draw.Bottom(strcpy_P(bottomTextBuffer, bottomMessages[newIndex]));
}

void borderUpdate() {
    if(millis() - borderLastUpdate < 500) return;
    borderLastUpdate = millis();
    Draw.CheckeredBorders();
}

void initialize() {
    Draw.init(TFT_CS, TFT_DC, TFT_RST);
    M_COLORS::Load();
    Draw.SystemBoot();

    Jingle(CHIISANA_BOKENSHA_JINGLE, true);
    delay(100);

    Draw.FakeLoading();

    mRTC.init();

    pinMode(BTN_SELECT, INPUT_PULLUP);
    pinMode(BTN_ADJUST, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);

    Draw.bg();

    // mRTC.sync();
}