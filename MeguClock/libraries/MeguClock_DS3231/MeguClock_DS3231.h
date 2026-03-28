#pragma once
#include <Wire.h>
#include "Arduino.h"
#include "MeguClock_DateTime.h"

#define W_START Wire.beginTransmission(0x68)
#define W_STOP Wire.endTransmission()

class rtc_i2c
{
protected:
    inline uint8_t bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }
    inline uint8_t bin2bcd(uint8_t val) { return val + 6 * (val / 10); }
};

/*!
    @brief  RTC based on the DS3231 chip connected via I2C and the Wire library
*/
class MeguClock_DS3231 : rtc_i2c
{
public:
    // temporary placeholder for editables
    uint8_t h, m, mo, d; uint16_t y;

    void init();
    bool rtcConnected();
    DateTime now();
    void adjust(const DateTime &dt);
    void Load();
    void Save();
    // static now
    static DateTime s_now;
    void updateRTC();
    void sync();
};