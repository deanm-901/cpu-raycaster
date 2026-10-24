#include "colors.h"
#include <stdint.h>


uint32_t ColorMult_component(uint32_t c1, float Rm, float Gm, float Bm, float Am) {
    uint32_t A = ((c1>>24)&0xFF) * Am;
    uint32_t R = ((c1>>16)&0xFF) * Rm;
    uint32_t G = ((c1>> 8)&0xFF) * Gm;
    uint32_t B = (c1&0xFF) * Bm;

    return (A<<24)|(R<<16)|(G<<8)|B;
}

uint32_t ColorMult_2colors(uint32_t c1, uint32_t c2) {
    if (c2==C_WHITE) return c1;

    uint32_t A = ((c1>>24)&0xFF) * (float)((c2>>24)&0xFF)/0xFF;
    uint32_t R = ((c1>>16)&0xFF) * (float)((c2>>16)&0xFF)/0xFF;
    uint32_t G = ((c1>> 8)&0xFF) * (float)((c2>> 8)&0xFF)/0xFF;
    uint32_t B = (c1&0xFF) * (float)(c2&0xFF)/0xFF;

    return (A<<24)|(R<<16)|(G<<8)|B;
}