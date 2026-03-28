/**************************************************************************
    This library is a modified version of the Adafruit RTCLib's DateTime

    https://github.com/adafruit/RTClib

    If you wish to use this feel free

    It is specifically* designed for my simple digital clock as the library
    is flash intensive, I've scrap the unused ones to barebones of the code.

    Author: Ichimaki Kasura
**************************************************************************/
#include "MeguClock_DateTime.h"

const char __months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const uint8_t __daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30};
const char __weeks[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

inline uint8_t conv2d(const char *p)
{
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

/*!
    @brief  Constructor from (year, month, day, hour, minute, second).

    This code was from the RTCLib's DateTime
    @warning If the provided parameters are not valid (e.g. 31 February),
             the constructed DateTime will be invalid.
    @param year Either the full year (range: 2000--2099) or the offset from
                year 2000 (range: 0--99).
    @param month Month number (1--12).
    @param day Day of the month (1--31).
    @param hour,min,sec Hour (0--23), minute (0--59) and second (0--59).
*/
DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t seconds)
{
    if (year >= 2000U)
        year -= 2000U;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = minute;
    ss = seconds;
}

/*!
    @brief  Memory friendly constructor for generating the build time.

    This version is intended to save RAM by keeping the date and time
    strings in program memory.

    This code was from the RTCLib's DateTime

    @param date Date PROGMEM string, e.g. F("Apr 16 2020").
    @param time Time PROGMEM string, e.g. F("18:34:56").
*/
DateTime::DateTime(const __FlashStringHelper *date, const __FlashStringHelper *time)
{
    char buff[11];
    memcpy_P(buff, date, sizeof(buff));
    m = 0;
    for (uint8_t i = 0; i < 12; i++)
        if (buff[0] == __months[i][0] && buff[1] == __months[i][1] && buff[2] == __months[i][2])
        {
            m = i + 1; break;
        }
    d = conv2d(buff + 4);
    yOff = conv2d(buff + 9);
    memcpy_P(buff, time, 8);
    hh = conv2d(buff);
    mm = conv2d(buff + 3);
    ss = conv2d(buff + 6);
}

/*!
    @brief  Gives weekdays as numerics

    `0 = Sunday`, `1 = Monday`, etc...

    This code was from the RTCLib's DateTime
*/
uint8_t DateTime::dayOfTheWeek() const
{
    uint16_t y = yOff,
             days = d;
    if (y >= 2000U)
        y -= 2000U;
    for (uint8_t i = 1; i < m; ++i)
        days += __daysInMonth[i - 1];
    if (m > 2 && y % 4 == 0)
        ++days;
    uint16_t day = days + 365 * y + (y + 3) / 4 - 1;
    return (day + 6) % 7;
}

/*!
    @brief  Gives weekdays as const chars*

    `Sunday`, `Monday`, etc...
*/
char *DateTime::dayOfTheWeekName() const
{
    return __weeks[dayOfTheWeek()];
}

/*!
    @brief  Gives month as const chars*

    `JAN `, `FEB `, etc...

    @note It has trailing space every end of the word.
*/
char *DateTime::monthName() const
{
    static char buff[4];
    const char *src = __months[m - 1];
    for (uint8_t i = 0; i < 3; i++)
        buff[i] = toupper(src[i]);
    buff[3] = '\0';
    return buff;
}