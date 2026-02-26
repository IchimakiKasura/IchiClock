#pragma once

const uint8_t alarmHours[] PROGMEM = {
    6,  // 6 AM     - FNAF YAY!
    10, // 10 AM    - "good morning :>"
    12, // 12 PM    - "good after noon :>"
    15, // 3 PM     - "its 3 O'CLOCK :>"
    22  // 10 PM    - "good night :>"
};
const char* const alarmMsgs[] PROGMEM = {
  "FNAF YAY!",
  "good morning :>",
  "good after noon :>",
  "its 3 O'CLOCK :>",
  "good night :>"
};
const uint16_t* const alarmMelodies[] PROGMEM = {
  FNAF_JINGLE,
  SAMSUNG,
  WESTMINISTER_CHIME_JINGLE,
  WESTMINISTER_CHIME_JINGLE,
  NIGHT_JINGLE
};
void CheckAlarm(int h, int m, int s) {
    if (m || s) return;
    static int8_t lastHour = -1;
    if (lastHour == h) return;
    lastHour = h;
    for (uint8_t i = 0; i < sizeof(alarmHours); i++) {
        if (pgm_read_byte(&alarmHours[i]) == h) {
            Draw.Bottom(pgm_read_word_near(&alarmMsgs[i]));
            Jingle(pgm_read_word_near(&alarmMelodies[i]));
            break;
        }
    }
}