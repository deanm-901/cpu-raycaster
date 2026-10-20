#include "colors.h"
#include <stdint.h>

inline uint32_t Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
    return (A<<24)|(R<<16)|(G<<8)|B;
}