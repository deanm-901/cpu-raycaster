#ifndef MAP_H
#define MAP_H

#include "defs.h"
#include <stdlib.h>

typedef enum __attribute__((__packed__)) _MapWallType {
    NONE,
    SOME
} MapWallType;

typedef struct _Map {
    int mapH[MAP_MAX_H];
    int mapV[MAP_MAX_V];
} Map;

typedef struct _WallAttr {
    double height;
    int textureID;
    int isCollidable;
} WallAttr;

typedef struct _WallTypeHolder {
    int wall_id;
    WallAttr walls[MAX_WALLTYPE_COUNT];
} WallTypeHolder;

#endif