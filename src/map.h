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


#define MAX_SPRITE_IMAGES 12
#define MAX_SPRITE_IMG_KEY_CHAR 24

typedef struct _SpriteImageList {
    char img_keys[MAX_SPRITE_IMAGES][MAX_SPRITE_IMG_KEY_CHAR];
    size_t img_amm;
} SpriteImageList;

typedef struct _SpriteAttr {
    float pos[2];

    SpriteImageList rotation_imgs;

    SpriteImageList special_anim;
    size_t anim_frame_num;
    uint8_t b_start_anim;

    float size;
    float a;
    uint8_t b_spin;
    uint8_t b_frontfacing;
    int spin_spd; // in frames per second
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

void SpriteImageList_init(SpriteImageList* list);

#endif