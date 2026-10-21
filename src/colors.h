#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>

#define C_RED    0xFF0000
#define C_GREEN  0x00FF00
#define C_BLUE   0x0000FF
#define C_YELLOW 0xFFFF00
#define C_CYAN   0x00FFFF
#define C_MAGENT 0xFF00FF
#define C_GRAY   0x808080
#define C_BLACK  0x000000
#define C_WHITE  0xFFFFFF

#define C_DRKRED 0x800000

inline uint32_t Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A);
uint32_t ColorMult(uint32_t c1, float Rm, float Gm, float Bm, float Am);
// --WIP--
uint32_t ColorMix(uint32_t c1, uint32_t c2, uint8_t w);

#endif