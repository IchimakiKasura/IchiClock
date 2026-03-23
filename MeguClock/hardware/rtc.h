#pragma once
#include <RTClib.h>
#include <Wire.h>

// uh Modify the RTClib too? by breaking it apart and only taking the DS3231?
class M_RTC {
public:
    RTC_DS3231 rtc;
    uint16_t h, m, mo, d, y;
    bool isRTC;
    void init();
    void sync();
    bool rtcConnected();
    int getCurrentDay(const DateTime &t);
    int getDay();
    inline const char* getCurrentMonth(const DateTime &t);
    inline int getCurrentHour(const DateTime &t);
    inline int getHour();
    inline const char* getCurrentMidday(const DateTime &t);
    inline const char* getMidday();
    void Load();
    void Save();
    DateTime now() { return rtc.now(); };
} mRTC;

void M_RTC::init() {
    Wire.begin();
    Wire.setWireTimeout(3000, true);
    rtc.begin();
    h = 0, m = 0, mo = 0, d = 0, y = 0;
}

void M_RTC::sync() {
    DateTime t(F(__DATE__), F(__TIME__));
    t = DateTime(t.year(), t.month(), t.day(), t.hour(), t.minute(), t.second() + 7);
    rtc.adjust(t);
}

bool M_RTC::rtcConnected() {
    Wire.beginTransmission(0x68);
    return (Wire.endTransmission() == 0);
}

void M_RTC::Load() {
    h = g_now.hour(); m = g_now.minute();
    d = g_now.day();  mo = g_now.month(); y = g_now.year();
}

void M_RTC::Save() {
    rtc.adjust(DateTime(y, mo, d, h, m, g_now.second()));
    editMode = false;
    selected = FIELD_HOUR;
}

int M_RTC::getCurrentDay(const DateTime &t) {
    if (t.month()==2) return (t.year()%4==0 && (t.year()%100!=0 || t.year()%400==0)) ? 29 : 28;
    if (t.month()==4 || t.month()==6 || t.month()==9 || t.month()==11) return 30;
    return 31;
}
int M_RTC::getDay() { return getCurrentDay({y, mo, d, h, m, 0}); }

inline const char* M_RTC::getCurrentMonth(const DateTime &t) {
    switch(t.month()) {
        case 1:  return "JAN ";
        case 2:  return "FEB ";
        case 3:  return "MAR ";
        case 4:  return "APR ";
        case 5:  return "MAY ";
        case 6:  return "JUN ";
        case 7:  return "JUL ";
        case 8:  return "AUG ";
        case 9:  return "SEP ";
        case 10: return "OCT ";
        case 11: return "NOV ";
        case 12: return "DEC ";
    }
}
inline int M_RTC::getCurrentHour(const DateTime &t) { return t.hour() % 12; }
inline int M_RTC::getHour() { return h % 12; }
inline const char* M_RTC::getCurrentMidday(const DateTime &t) { return t.hour() >= 12 ? "PM" : "AM"; }
inline const char* M_RTC::getMidday() { return h >= 12 ? "PM" : "AM"; }