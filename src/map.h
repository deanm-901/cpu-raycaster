#ifndef MAP_H
#define MAP_H

#include "defs.h"
#include <stdlib.h>

typedef enum __attribute__((__packed__)) _MapFaceType {
    T_NONE,
    T_SOME
} MapFaceType;

typedef struct _Map {
    float wall_height;
    MapFaceType mapH[MAP_MAX_H];
    MapFaceType mapV[MAP_MAX_V];
    MapFaceType mapF[MAP_MAX_X*MAP_MAX_Y];
    MapFaceType mapC[MAP_MAX_X*MAP_MAX_Y];
} Map;

typedef struct _WallAttr {
    int textureID;
    int isCollidable;
} WallAttr;

typedef struct _WallTypeHolder {
    int wall_id;
    WallAttr walls[MAX_WALLTYPE_COUNT];
} WallTypeHolder;

#endif