#pragma once
#include <Wire.h>
#include "display.h"

class M_RTC {
    public:
        inline static uint16_t h = 0, m = 0, mo = 0, d = 0, y = 0;
        static bool rtcConnected() {
            Wire.beginTransmission(0x68);
            return (Wire.endTransmission() == 0);
        }
        static int getDay() {
            if (m == 2) return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 29 : 28;
            if (m == 4 || m == 6 || m == 9 || m == 11) return 30;
            return 31;
        }
        static void Load() {
            h = now.hour(); m = now.minute();
            d = now.day();  mo = now.month(); y = now.year();
        }
        static void Save() {
            rtc.adjust(DateTime(y, mo, d, h, m, now.second()));
            editMode = false;
            selected = FIELD_HOUR;
            Draw.Header(0);
            Draw.ReDraw();
            quickBeepStart();
        }
};