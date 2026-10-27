#include "defs.h"

inline int coordToIndex(int x, int y, int width) {
    return y*width + x;
}
inline void indexToCoord(int index, int width, int* x, int* y) {
    *x = index%width;
    *y = index/width;
}

inline float lerp(float v0, float v1, float t) {
    return ((1-t)*v0+t*v1);
}