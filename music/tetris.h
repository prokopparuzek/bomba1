#pragma once
#include "tones.h"
// tetris song
// 140 bpm
#define CTVRT 428 // v ms
#define POSUN 3

const uint16_t song [] PROGMEM = {
    e1, CTVRT, h, CTVRT/2, c1, CTVRT/2, d1, CTVRT/2, e1, CTVRT/4, d1, CTVRT/4, c1, CTVRT/2, h, CTVRT/2,
    a, CTVRT, a, CTVRT/2, c1, CTVRT/2, e1, CTVRT, d1, CTVRT/2, c1, CTVRT/2,
    h, CTVRT, h, CTVRT/2, c1, CTVRT/2, d1, CTVRT, e1, CTVRT,
    c1, CTVRT, a, CTVRT, a, CTVRT, H, CTVRT/2, c, CTVRT/2,
    POMLKA, CTVRT/2, d1, CTVRT, f1, CTVRT/2, a1, CTVRT, g1, CTVRT/2, f1, CTVRT/2,
    e1, CTVRT + CTVRT/2, c1, CTVRT/2, e1, CTVRT, d1, CTVRT/2, c1, CTVRT/2,
    h, CTVRT, h, CTVRT/2, c1, CTVRT/2, d1, CTVRT, e1, CTVRT,
    c1, CTVRT, a, CTVRT, a, CTVRT, POMLKA, CTVRT,
    e1, CTVRT, h, CTVRT/2, c1, CTVRT/2, d1, CTVRT/2, e1, CTVRT/4, d1, CTVRT/4, c1, CTVRT/2, h, CTVRT/2,
    a, CTVRT, a, CTVRT/2, c1, CTVRT/2, e1, CTVRT, d1, CTVRT/2, c1, CTVRT/2,
    h, CTVRT, h, CTVRT/2, c1, CTVRT/2, d1, CTVRT, e1, CTVRT,
    c1, CTVRT, a, CTVRT, a, CTVRT, H, CTVRT/2, c, CTVRT/2,
    POMLKA, CTVRT/2, d1, CTVRT, f1, CTVRT/2, a1, CTVRT, g1, CTVRT/2, f1, CTVRT/2,
    e1, CTVRT + CTVRT/2, c1, CTVRT/2, e1, CTVRT, d1, CTVRT/2, c1, CTVRT/2,
    h, CTVRT, h, CTVRT/2, c1, CTVRT/2, d1, CTVRT, e1, CTVRT,
    c1, CTVRT, a, CTVRT, a, CTVRT, POMLKA, CTVRT,
    e, CTVRT*2, c, CTVRT*2,
    d, CTVRT*2, H, CTVRT*2,
    c, CTVRT*2, A, CTVRT*2,
    Ab, CTVRT*2, H, CTVRT*2,
    e, CTVRT*2, c, CTVRT*2,
    d, CTVRT*2, H, CTVRT, H, CTVRT,
    c, CTVRT, e, CTVRT, a, CTVRT, a, CTVRT,
    ab, CTVRT*3, h, CTVRT
};
