#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>

#define C_RED    0xFFFF0000
#define C_GREEN  0xFF00FF00
#define C_BLUE   0xFF0000FF
#define C_YELLOW 0xFFFFFF00
#define C_CYAN   0xFF00FFFF
#define C_MAGENT 0xFFFF00FF
#define C_GRAY   0xFF808080
#define C_BLACK  0xFF000000
#define C_WHITE  0xFFFFFFFF

#define C_DRKRED 0xFF800000

// --WIP--
uint32_t ColorMix(uint32_t c1, uint32_t c2, uint8_t w);

uint32_t ColorMult_component(uint32_t c1, float Rm, float Gm, float Bm, float Am);
uint32_t ColorMult_2colors(uint32_t c1, uint32_t c2);

#define COLOR_MULT_SELECT(_1,_2,_3,_4,_5,NAME,...) NAME

#define ColorMult(...) \
    COLOR_MULT_SELECT(__VA_ARGS__, \
        ColorMult_component, \
        bad_4_args, \
        bad_3_args, \
        ColorMult_2colors, \
        bad_1_args, \
    )(__VA_ARGS__)

#endif