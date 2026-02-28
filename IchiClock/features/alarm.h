#pragma once

struct Alarms {
    const uint8_t Hours[5];
    const char Messages[5][19];
    const uint8_t Melodies[5];
};

const Alarms alarm PROGMEM = {
    { 6, 10, 12, 15, 22 },
    {
        "FNAF YAY!",
        "good morning :>",
        "good after noon :>",
        "its 3 O'CLOCK :>",
        "good night :>"
    },
    { FNAF_JINGLE, SAMSUNG, WESTMINISTER_CHIME_JINGLE,
      WESTMINISTER_CHIME_JINGLE, NIGHT_JINGLE }
};

void CheckAlarm(int h, int m, int s, bool test = false) {
    if(!test) {
        if (m || s) return;
        static int8_t lastHour = -1;
        if (lastHour == h) return;
        lastHour = h;
    }
    for (uint8_t i = 0; i < sizeof(alarm.Hours) / sizeof(alarm.Hours[0]); i++) {
        if (pgm_read_byte(&alarm.Hours[i]) == h) {
            char buffer[sizeof(alarm.Messages[i])];
            strcpy_P(buffer, alarm.Messages[i]);
            Draw.Bottom(buffer);
            Jingle(pgm_read_byte(&alarm.Melodies[i]), false, i==0);
            break;
        }
    }
}