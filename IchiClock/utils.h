#pragma once

uint16_t h, m, mo, d, y;

void initialize() {             // goofy ahh init
    tft.initR(INITR_GREENTAB);
    tft.setRotation(0);
    Draw.init(tft);
    tft.fillScreen(BLACK);
    loadColors();
    Draw.SystemBoot();
    Jingle(STARTUP_JINGLE);
    delay(100);
    Draw.FakeLoading();
    Wire.begin();
    rtc.begin();
    pinMode(BTN_SELECT, INPUT_PULLUP);
    pinMode(BTN_ADJUST, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);
    tft.fillScreen(BLACK);

    if(SETUP_TIME) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}
void refreshScreen(const DateTime &t = now) {
    Draw.Time(t);
    Draw.Date(t);
}
int daysInMonth(int m, int y) {
    if (m == 2) return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 29 : 28;
    if (m == 4 || m == 6 || m == 9 || m == 11) return 30;
    return 31;
}
inline void loadFromRTC() {
    h = now.hour(); m = now.minute();
    d = now.day();  mo = now.month(); y = now.year();
}
inline void saveToRTC() {
    rtc.adjust(DateTime(y, mo, d, h, m, now.second()));
    editMode = false;
    selected = FIELD_HOUR;
    Draw.Header();
    refreshScreen();
    quickBeepStart();
}