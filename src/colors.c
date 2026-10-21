#include "colors.h"
#include <stdint.h>

inline uint32_t Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
    return (A<<24)|(R<<16)|(G<<8)|B;
}

uint32_t ColorMult(uint32_t c1, float Rm, float Gm, float Bm, float Am) {
    uint8_t A = ((c1>>24)&0xFF) * Am;
    uint8_t R = ((c1>>16)&0xFF) * Rm;
    uint8_t G = ((c1>> 8)&0xFF) * Gm;
    uint8_t B = (c1&0xFF) * Bm;

    return (A<<24)|(R<<16)|(G<<8)|B;
}