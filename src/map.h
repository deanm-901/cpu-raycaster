#ifndef MAP_H
#define MAP_H

#include "defs.h"
#include <stdlib.h>

typedef enum __attribute__((__packed__)) _MapFaceType {
    T_NONE,
    T_HOMER,
    T_PIZZA,
    T_GRASS,
    T_BRICK,
} MapFaceType;

typedef struct _Map {
    float wall_height;
    MapFaceType mapH[MAP_MAX_H];
    MapFaceType mapV[MAP_MAX_V];
    MapFaceType mapF[MAP_MAX_X*MAP_MAX_Y];
    MapFaceType mapC[MAP_MAX_X*MAP_MAX_Y];
} Map;

typedef struct _FaceAttr {
    int atlas_pos[2];
    int isCollidable;
} FaceAttr;

#endif