#pragma once

const uint8_t alarmHours[5] PROGMEM = {
    6,  // 6 AM     - FNAF YAY!
    10, // 10 AM    - "good morning :>"
    12, // 12 PM    - "good after noon :>"
    15, // 3 PM     - "its 3 O'CLOCK :>"
    22  // 10 PM    - "good night :>"
};
const char alarmMsgs[5][19] PROGMEM = {
  "FNAF YAY!",
  "good morning :>",
  "good after noon :>",
  "its 3 O'CLOCK :>",
  "good night :>"
};
const uint8_t alarmMelodies[5] PROGMEM = {
  FNAF_JINGLE,
  SAMSUNG,
  WESTMINISTER_CHIME_JINGLE,
  WESTMINISTER_CHIME_JINGLE,
  NIGHT_JINGLE
};
void CheckAlarm(int h, int m, int s, bool test = false) {
    if(!test) {
        if (m || s) return;
        static int8_t lastHour = -1;
        if (lastHour == h) return;
        lastHour = h;
    }
    for (uint8_t i = 0; i < sizeof(alarmHours) / sizeof(alarmHours[0]); i++) {
        if (pgm_read_byte(&alarmHours[i]) == h) {
            char buffer[sizeof(alarmMsgs[i])];
            strcpy_P(buffer, alarmMsgs[i]);
            Draw.Bottom(buffer);
            Jingle(pgm_read_byte(&alarmMelodies[i]), false, i==0);
            break;
        }
    }
}