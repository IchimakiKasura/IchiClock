#pragma once
#include "Arduino.h"

/*!
    @brief  Modified DateTime from RTCLib.
*/
class DateTime
{
public:
    DateTime() = default;
    DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t seconds);
    DateTime(const __FlashStringHelper *date, const __FlashStringHelper *time);

    /*! @brief Year haha */
    uint16_t year() const { return 2000U + yOff; }
    /*! @brief Month haha */
    uint8_t month() const { return m; }
    char *monthName() const;
    /*! @brief Day haha */
    uint8_t day() const { return d; }
    /*! @brief Hour haha */
    uint8_t hour() const { return hh; }

    /*!
        @brief  Returns 12 hour clock.

        If the clock is more than 12, e.g. 13, it will return as 1.

        This code was from the RTCLib's DateTime
    */
    uint8_t twelveHour() const { return (hh == 0 || hh == 12) ? 12 : (hh > 12 ? hh - 12 : hh); }

    /*! @brief AM or PM */
    char *midday() const { return hh >= 12 ? "PM" : "AM"; }
    /*! @brief Minute haha */
    uint8_t minute() const { return mm >= 59 ? 59 : mm; };
    /*! @brief Second haha */
    uint8_t second() const { return ss; };
    uint8_t dayOfTheWeek() const;
    char *dayOfTheWeekName() const;

protected:
    uint8_t yOff;
    uint8_t m;
    uint8_t d;
    uint8_t hh;
    uint8_t mm;
    uint8_t ss;
};