#ifndef MAP_H
#define MAP_H

#include "defs.h"
#include <stdlib.h>
#include <stdint.h>

typedef enum __attribute__((__packed__)) _MapFaceType {
    T_NONE,
    T_HOMER,
    T_PIZZA,
    T_GRASS,
    T_BRICK,
    T_TILES,
    T_BCKRM,
} MapFaceType;

typedef struct __attribute__((__packed__)) _MapTileAttr {
    uint8_t light_lvl;
    MapFaceType clng_type;
    MapFaceType flor_type;
} MapTileAttr;

typedef struct _SpriteAttr {
    float pos[2];
    char* img_key;
    float size;
    float a;
} SpriteAttr;

#define MapTileAttr_INIT(X) MapTileAttr X = {.light_lvl = 255, .flor_type = T_GRASS}

typedef struct _Map {
    float wall_height;
    MapFaceType mapH[MAP_MAX_H];           // horizontal wall face type
    MapFaceType mapV[MAP_MAX_V];           // vertical wall face type
    MapTileAttr mapT[MAP_MAX_X*MAP_MAX_Y]; // map tile attributes
    SpriteAttr sprites[MAX_MAP_SPRITES];
    size_t sprites_amm;
    char* sky_texture;
} Map;

typedef struct _FaceAttr {
    int atlas_pos[2];
    int isCollidable;
} FaceAttr;

size_t Map_addSprite(Map* map, SpriteAttr sprite);

#endif