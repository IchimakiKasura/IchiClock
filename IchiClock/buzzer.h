#pragma once

extern DrawUI Draw;

const uint16_t melody[5][43] PROGMEM = {
    { 523, 523, 587, 659, 659, 784, 880,1047, 880, 784, 659, 659, 587, 523, 880, 784, 659, 587, 392, 523, 523, 587, 523 }, // konosuba ed
    { 880, 698, 784, 523, 0  , 523, 784, 880, 698 }, // FNAF
    { 330, 415, 370, 247, 0  , 0  , 330, 370, 415, 330, 0  , 0  , 415, 330, 370, 247, 0  , 0  , 247, 370, 415, 330 }, // Westminster Chime
    { 262, 262, 392, 392, 440, 440, 392, 0  , 349, 349, 330, 330, 294, 294, 262}, // night
    { 392, 330, 262, 440, 392, 330, 392, 349, 392, 349, 294, 330, 659, 523, 392, 523, 392, 330, 440, 392, 330, 523, 392, 196, 440, 494, 392, 349, 294, 262, 262, 294, 330, 349, 392, 440, 494, 523, 587, 659, 698, 784 }
};

const uint16_t noteDurations[5][43] PROGMEM = {
    { 600, 350, 100, 600, 350, 100, 350, 100, 350, 100, 600, 350, 100, 600, 600, 350, 100, 350, 100, 600, 350, 100, 1225 },
    { 820, 820, 820, 820, 300, 820, 820, 820, 1000 }, 
    { 420, 420, 420, 735, 200, 200, 420, 420, 420, 735, 200, 200, 420, 420, 420, 735, 200, 200, 420, 420, 420, 735 },
    { 400, 400, 400, 400, 400, 400, 200, 400, 400, 400, 400, 400, 400, 200, 200},
    { 444, 223, 444, 445, 1111, 444, 445, 444, 222, 445, 444, 445, 222, 222, 222, 222, 223, 444, 445, 222, 444, 445, 889, 666, 445, 444, 222, 445, 444, 445, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35 }
};

const uint8_t melodyLengths[5] PROGMEM = { 23, 9, 22, 15, 43 };
    
void quickBeepStart(int duration = 50) {
    tone(BUZZER, 1000, duration);
}

struct JingleState {
    uint8_t jingle, index = 0, delays = 0;
    unsigned long nextTime = 0;
    bool playing = false, borders;
} jingleState;

void Jingle(uint8_t jingleNumber, bool blocking = false, bool changeBorders = false, uint8_t delays = 0) {
    uint8_t len = pgm_read_byte_near(&melodyLengths[jingleNumber]);
    
    if(blocking) {
        for(uint8_t i = 0; i < len; i++) {
            uint16_t note = pgm_read_word_near(&melody[jingleNumber][i]);
            uint16_t dur  = pgm_read_word_near(&noteDurations[jingleNumber][i]);
            if(note) { tone(BUZZER, note); delay(dur+delays); noTone(BUZZER); }
            else delay(dur);
            delay(30);
        }
    } else {
        jingleState.jingle   = jingleNumber;
        jingleState.index    = 0;
        jingleState.delays   = delays;
        jingleState.nextTime = 0;
        jingleState.playing  = true;
        jingleState.borders  = changeBorders;
    }
}

void updateJingle() {
    static bool playing = false;
    static unsigned long end = 0, lastBorder = 0;
    if(!jingleState.playing) return;
    unsigned long now = millis();
    uint8_t len = pgm_read_byte_near(&melodyLengths[jingleState.jingle]);
    if(jingleState.borders && now - lastBorder >= 500) { Draw.TextColorChange(); lastBorder = now; }
    if(jingleState.index >= len) { jingleState.playing = false; noTone(BUZZER); loadColors(); Draw.Time(); Draw.Date(); return; }
    uint16_t note = pgm_read_word_near(&melody[jingleState.jingle][jingleState.index]);
    uint16_t dur  = pgm_read_word_near(&noteDurations[jingleState.jingle][jingleState.index]);
    if(!playing) { if(note) tone(BUZZER, note); end = now + dur + jingleState.delays; playing = true; }
    if(playing && now >= end) { if(note) noTone(BUZZER); playing = false; jingleState.nextTime = now + 30; jingleState.index++; }
}