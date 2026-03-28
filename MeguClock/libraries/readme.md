# ST7735 and DS3231 Modified libraries


## ST7735 1.8 (GREEN TAB)
- Customized GFX for ST7735 1.8" 128x160 GREEN TAB

- Functions that only works:
    - `drawPixel()`
    - `writeLine()`
    - `fillRect()`
    - `fillScreen()`
    - `drawChar()`
    - `getTextBounds()`
    - `print()` / `write()`
    - `setTextSize()`
    - `setCursor()`
    - `setTextColor()`
    - `writeColor()`

## DS3231
- Customized RTC library for DS3231

- Functions available:
    - DateTime
        - `year()`
        - `month()`
        - `monthName()`
        - `day()`
        - `hour()`
        - `twelveHour()`
        - `midday()`
        - `minute()`
        - `second()`
        - `dayOfTheWeek()`
        - `dayofTheWeekName()`
    - MeguClock_DS3231
        - `h`, `m`, `mo`, `d`, `y` -> _these are only for placeholders if you have edit mode function_
        - `init()`
        - `rtcConnected()`
        - `adjust()`
        - `Load()`
        - `Save()`
        - `now()`
        - `sync()`
        - `s_now` -> _built-in static variable for 'now' named 's_now'_

# Usage
- Put the folders at the arduino libraries or add `--libraries ./libraries` _(if you're using arduino cli)_ when uploading.