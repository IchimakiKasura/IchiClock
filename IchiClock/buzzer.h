#pragma once

extern DrawUI Draw;

const uint16_t melody[][23] PROGMEM = {
    { 523, 523, 587, 659, 659, 784, 880,1047, 880, 784, 659, 659, 587, 523, 880, 784, 659, 587, 392, 523, 523, 587, 523 }, // konosuba ed
    { 880, 698, 784, 523, 0  , 523, 784, 880, 698 }, // FNAF
    { 330, 415, 370, 247, 0  , 0  , 330, 370, 415, 330, 0  , 0  , 415, 330, 370, 247, 0  , 0  , 247, 370, 415, 330 }, // Westminster Chime
    { 262, 262, 392, 392, 440, 440, 392, 0  , 349, 349, 330, 330, 294, 294, 262}, // night
    { 1040, 1040 , 1310 } // samsung
};

const uint16_t noteDurations[][23] PROGMEM = {
    { 600, 350, 100, 600, 350, 100, 350, 100, 350, 100, 600, 350, 100, 600, 600, 350, 100, 350, 100, 600, 350, 100, 1225 },
    { 820, 820, 820, 820, 300, 820, 820, 820, 1000 }, 
    { 420, 420, 420, 735, 200, 200, 420, 420, 420, 735, 200, 200, 420, 420, 420, 735, 200, 200, 420, 420, 420, 735 },
    { 400, 400, 400, 400, 400, 400, 200, 400, 400, 400, 400, 400, 400, 200, 200},
    {  70, 110, 90 }
};

const uint8_t melodyLengths[] PROGMEM = { 23, 9, 22, 15, 3 };
    
void quickBeepStart(int duration = 50) {
    tone(BUZZER, 1000, duration);
}

void Jingle(uint8_t jingleNumber, uint8_t delays = 0) {
    uint8_t melodyLength = pgm_read_byte_near(&melodyLengths[jingleNumber]);

    for (int i = 0; i < melodyLength; i++) {
        uint16_t note = pgm_read_word_near(&melody[jingleNumber][i]);
        uint16_t dur  = pgm_read_word_near(&noteDurations[jingleNumber][i]);
        if(note > 0) {
            tone(BUZZER, note);
            if(jingleNumber == CHIISANA_BOKENSHA_JINGLE && delays >= 1) Draw.TextColorChange();
            if(jingleNumber == FNAF_JINGLE) Draw.TextColorChange();
            delay(dur);
            noTone(BUZZER);
        } else {
            delay(dur);
        }
        delay(50);
    }

    if(jingleNumber != FNAF_JINGLE || delays >= 1) return;
    loadColors();
    Draw.Time();
    Draw.Date();
}