#include "defs.h"

int coordToIndex(int x, int y, int width) {
    return y*width + x;
}