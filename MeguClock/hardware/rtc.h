#pragma once
#include <RTClib.h>
#include <Wire.h>

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
    char getCurrentMonth(const char *months[]);
    int getCurrentHour(const DateTime &t);
    int getHour();
    char* getCurrentMidday(const DateTime &t);
    char* getMidday();
    void Load();
    void Save();
};

inline void M_RTC::init() {
    Wire.begin();
    Wire.setWireTimeout(3000, true);
    rtc.begin();
    h = 0, m = 0, mo = 0, d = 0, y = 0;
}

inline void M_RTC::sync() {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

inline bool M_RTC::rtcConnected() {
    Wire.beginTransmission(0x68);
    return (Wire.endTransmission() == 0);
}

inline int M_RTC::getCurrentDay(const DateTime &t) {
    if (t.month() == 2) return (t.year() % 4 == 0 && (t.year() % 100 != 0 || t.year() % 400 == 0)) ? 29 : 28;
    if (t.month() == 4 || t.month() == 6 || t.month() == 9 || t.month() == 11) return 30;
    return 31;
}

inline int M_RTC::getDay() {
    if (mo == 2) return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 29 : 28;
    if (mo == 4 || mo == 6 || mo == 9 || mo == 11) return 30;
    return 31;
}

inline char M_RTC::getCurrentMonth(const char *months[]) {
    return months[now.month()-1];
}

inline int M_RTC::getCurrentHour(const DateTime &t) {
    return t.hour() % 12;
}

inline int M_RTC::getHour() {
    return h % 12;
}

inline char* M_RTC::getCurrentMidday(const DateTime &t) {
    return t.hour() >= 12 ? "AM" : "PM";
}

inline char* M_RTC::getMidday() {
    return h >= 12 ? "AM" : "PM";
}

inline void M_RTC::Load() {
    h = now.hour(); m = now.minute();
    d = now.day();  mo = now.month(); y = now.year();
}

inline void M_RTC::Save() {
    rtc.adjust(DateTime(y, mo, d, h, m, now.second()));
    editMode = false;
    selected = FIELD_HOUR;
}

inline M_RTC mRTC;