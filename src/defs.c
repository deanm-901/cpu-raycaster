#include "defs.h"

inline int coordToIndex(int x, int y, int width) {
    return y*width + x;
}
inline void indexToCoord(int index, int width, int* x, int* y) {
    *x = index%width;
    *y = index/width;
}
