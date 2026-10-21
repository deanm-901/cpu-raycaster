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
    T_TILES,
} MapFaceType;

// --WIP--
//typedef struct _MapTileAttr {
//    uint8_t light_level;
//    uint8_t height;
//} MapTileAttr

typedef struct _Map {
    float wall_height;
    MapFaceType mapH[MAP_MAX_H];           // horizontal wall face type
    MapFaceType mapV[MAP_MAX_V];           // vertical wall face type
    MapFaceType mapF[MAP_MAX_X*MAP_MAX_Y]; // floor face type
    MapFaceType mapC[MAP_MAX_X*MAP_MAX_Y]; // ceiling face type
    uint8_t mapLight[MAP_MAX_X*MAP_MAX_Y]; // lighting
} Map;

typedef struct _FaceAttr {
    int atlas_pos[2];
    int isCollidable;
} FaceAttr;

#endif