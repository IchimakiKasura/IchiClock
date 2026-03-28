/**************************************************************************
    This library is a modified version of the Adafruit RTCLib's DS3231

    https://github.com/adafruit/RTClib

    If you wish to use this feel free

    It is specifically* designed for my simple digital clock as the library
    is flash intensive, I've scrap the unused ones to barebones of the code.

    Author: Ichimaki Kasura
**************************************************************************/
#include "MeguClock_DS3231.h"

// Offsets the time to fix the delay after upload.
#define OFFSET 9

DateTime MeguClock_DS3231::s_now;

void MeguClock_DS3231::init()
{
    Wire.begin();
    Wire.setWireTimeout(3000, true);
    h = m = mo = d = y = 0;
}

/*!
    @brief  Checks rtc connection.
    @return ```Boolean```
*/
bool MeguClock_DS3231::rtcConnected()
{
    W_START;
    return (W_STOP == 0);
}

/*!
    @brief  Get the current date/time
    @return DateTime object with the current date/time
*/
DateTime MeguClock_DS3231::now()
{
    uint8_t buffer[7];

    W_START;
    Wire.write(0);
    W_STOP;

    Wire.requestFrom(0x68, 7);

    for (uint8_t i = 0; i < 7; i++)
        buffer[i] = Wire.read();

    return DateTime(
        bcd2bin(buffer[6]) + 2000U,
        bcd2bin(buffer[5] & 0x7F),
        bcd2bin(buffer[4]),
        bcd2bin(buffer[2]),
        bcd2bin(buffer[1]),
        bcd2bin(buffer[0] & 0x7F));
}

/*!
    @brief  Set the date and flip the Oscillator Stop Flag
    @param dt DateTime object containing the date/time to set
*/
void MeguClock_DS3231::adjust(const DateTime &dt)
{
    uint8_t buffer[8] = {
        0x00,
        bin2bcd(dt.second()),
        bin2bcd(dt.minute()),
        bin2bcd(dt.hour()),
        bin2bcd(dt.dayOfTheWeek() == 0 ? 7 : dt.dayOfTheWeek()),
        bin2bcd(dt.day()),
        bin2bcd(dt.month()),
        bin2bcd(dt.year() - 2000U)};

    W_START;
    for (uint8_t i = 0; i < 8; i++)
        Wire.write(buffer[i]);
    W_STOP;
    W_START;
    Wire.write(0x0F);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 1);
    uint8_t statreg = Wire.read();
    statreg &= ~0x80;
    W_START;
    Wire.write(0x0F);
    Wire.write(statreg);
    W_STOP;
}

/*!
    @brief  Loads current time to temporary
*/
void MeguClock_DS3231::Load()
{
    h = s_now.hour();
    m = s_now.minute();
    d = s_now.day();
    mo = s_now.month();
    y = s_now.year();
}

/*!
    @brief  Saves temporary to current time.
    @note Will overwrite the current time.
*/
void MeguClock_DS3231::Save()
{
    adjust(DateTime(y, mo, d, h, m, s_now.second()));
}

/*!
   @brief Updates the static DateTime to current.
*/
void MeguClock_DS3231::updateRTC()
{
    s_now = now();
}

/*!
   @brief It syncs.
*/
void MeguClock_DS3231::sync()
{
    DateTime t(F(__DATE__), F(__TIME__));
    adjust(DateTime(t.year(), t.month(), t.day(), t.hour(), t.minute(), t.second() + OFFSET));
}