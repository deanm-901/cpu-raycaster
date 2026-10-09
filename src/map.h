#ifndef MAP_H
#define MAP_H

#include "defs.h"
#include <stdlib.h>

typedef enum __attribute__((__packed__)) _MapWallType {
    NONE,
    SOME
} MapWallType;

typedef struct {
    MapWallType mapH[MAP_MAX_H];
    MapWallType mapV[MAP_MAX_V];
} Map;

#endif