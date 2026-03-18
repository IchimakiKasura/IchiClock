#pragma once
#include "../config/ColorConfig.h"

extern inline void updateFunction(void (*func)(), uint32_t &ms, int16_t t);

const uint16_t melodies[5][86] PROGMEM = {
    // 0 - Konosuba ED
    {
        24, // total notes
    /*  note:      duration:  */
        523,      600,
        523,      350,
        587,      100,
        659,      600,
        659,      350,
        784,      100,
        880,      350,
        1047,     100,
        880,      350,
        784,      100,
        659,      600,
        659,      350,
        587,      100,
        523,      600,
        880,      600,
        784,      350,
        659,      100,
        587,      350,
        392,      100,
        523,      600,
        523,      350,
        587,      100,
        523,      1225
    },
    // 1 - FNAF
    {
        10,
        880,820, 698,820, 784,820, 523,820, 0,300,
        523,820, 784,820, 880,820, 698,1000
    },
    // 2 - Westminster Chime
    {
        23,
        330,420, 415,420, 370,420, 247,735, 0,200, 0,200,
        330,420, 370,420, 415,420, 330,735, 0,200, 0,200,
        415,420, 330,420, 370,420, 247,735, 0,200, 0,200,
        247,420, 370,420, 415,420, 330,735
    },
    // 3 - Night
    {
        16,
        262,400, 262,400, 392,400, 392,400,
        440,400, 440,400, 392,200, 0,400,
        349,400, 349,400, 330,400, 330,400,
        294,400, 294,400, 262,200
    },
    // 4 - Custom
    {
        44,
        392,444, 330,223, 262,444, 440,445, 392,1111,
        330,444, 392,445, 349,444, 392,222, 349,445,
        294,444, 330,445, 659,222, 523,222, 392,222,
        523,222, 392,223, 330,444, 440,445, 392,222,
        330,444, 523,445, 392,889, 196,666,
        440,445, 494,444, 392,222, 349,445, 294,444, 262,445,
        262,35, 294,35, 330,35, 349,35, 392,35, 440,35,
        494,35, 523,35, 587,35, 659,35, 698,35, 784,35
    }
};

void quickBeepStart(int duration = 50) {
    tone(
        #ifdef CUSTOM_PINS
        BUZZER
        #else
        5
        #endif
        , 1000, duration
    );
}

struct JingleState {
    uint8_t jingle;
    uint8_t index  = 0;
    uint8_t delays = 0;
    uint32_t nextTime = 0;
    bool playing = false;
    bool borders = false;
} jingleState;

void Jingle(uint8_t jingleNumber,
            bool blocking      = false,
            bool changeBorders = false,
            uint8_t delays     = 0)
{
    uint8_t len = pgm_read_byte_near(&melodies[jingleNumber][0]);

    if (blocking) {

        for (uint8_t i = 0; i < len; i++) {
            uint8_t n = 1 + i * 2;
            uint16_t note = pgm_read_word_near(&melodies[jingleNumber][n]);
            uint16_t dur = pgm_read_word_near(&melodies[jingleNumber][n + 1]);

            if (note) {
                tone(
                    #ifdef CUSTOM_PINS
                    BUZZER
                    #else
                    5
                    #endif
                    , note
                );
                delay(dur + delays);
                noTone(
                    #ifdef CUSTOM_PINS
                    BUZZER
                    #else
                    5
                    #endif
                );
            } else {
                delay(dur);
            }

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

    static bool playing        = false;
    static uint32_t end        = 0;
    static uint32_t lastBorder = 0;

    if (!jingleState.playing) return;

    uint8_t len = pgm_read_byte_near(&melodies[jingleState.jingle][0]);

    if (jingleState.borders)
        updateFunction([](){Draw.TextColorChange();}, lastBorder, 500);

    if (jingleState.index >= len) {
        jingleState.playing = false;

        noTone(
        #ifdef CUSTOM_PINS
            BUZZER
        #else
            5
        #endif
        );

        M_COLORS::Load();
        Draw.ReDraw();
        return;
    }

    uint8_t n = 1 + jingleState.index * 2;

    uint16_t note = pgm_read_word_near(&melodies[jingleState.jingle][n]);

    uint16_t dur = pgm_read_word_near(&melodies[jingleState.jingle][n + 1]);

    if (!playing) {

        if (note)
            tone(
            #ifdef CUSTOM_PINS
                BUZZER
            #else
                5
            #endif
            , note);

        end = systemTime + dur + jingleState.delays;
        playing = true;
    }

    if (playing && systemTime >= end) {

        if (note)
            noTone(
            #ifdef CUSTOM_PINS
                BUZZER
            #else
                5
            #endif
            );

        playing = false;
        jingleState.nextTime = systemTime + 30;
        jingleState.index++;
    }
}