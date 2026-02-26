#pragma once

extern DrawUI Draw;

const uint16_t melody[][22] PROGMEM = {
    { 659, 622, 659, 0  , 523, 0  , 523, 659, 784 }, // Startup
    { 880, 698, 784, 523, 0  , 523, 784, 880, 698 }, // FNAF
    { 330, 415, 370, 247, 0  , 0  , 330, 370, 415, 330, 0  , 0  , 415, 330, 370, 247, 0  , 0  , 247, 370, 415, 330 }, // Westminster Chime
    { 262, 262, 392, 392, 440, 440, 392, 0  , 349, 349, 330, 330, 294, 294, 262}, // night
    { 1040, 1040 , 1310 } // samsung
};

const uint16_t noteDurations[][22] PROGMEM = {
    { 150, 150, 150, 100, 150, 100, 150, 150, 300 },  
    { 820, 820, 820, 820, 300, 820, 820, 820, 1000 }, 
    { 420, 420, 420, 735, 200, 200, 420, 420, 420, 735, 200, 200, 420, 420, 420, 735, 200, 200, 420, 420, 420, 735 },
    { 400, 400, 400, 400, 400, 400, 200, 400, 400, 400, 400, 400, 400, 200, 200},
    {  70, 110, 90 }
};

const uint8_t melodyLengths[] PROGMEM = { 9, 9, 22, 15, 3 };
    
void quickBeepStart(int duration = 50) {
    tone(BUZZER, 1000, duration);
}

void Jingle(uint8_t jingleNumber) {
    uint8_t melodyLength = pgm_read_byte_near(&melodyLengths[jingleNumber]);

    for (int i = 0; i < melodyLength; i++) {
        uint16_t note = pgm_read_word_near(&melody[jingleNumber][i]);
        uint16_t dur  = pgm_read_word_near(&noteDurations[jingleNumber][i]);
        if(note > 0) {
            tone(BUZZER, note);
            if(jingleNumber == FNAF_JINGLE) Draw.TextColorChange();
            delay(dur);
            noTone(BUZZER);
        } else {
            delay(dur);
        }
        delay(50);
    }

    if(jingleNumber != FNAF_JINGLE) return;
    loadColors();
    now = rtc.now();
    Draw.Time(now);
    Draw.Date(now);
}