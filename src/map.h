#ifndef MAP_H
#define MAP_H

#include "defs.h"
#include "image-holder.h"
#include <stdlib.h>
#include <stdint.h>

typedef struct __attribute__((__packed__)) _MapTileAttr {
    uint8_t light_lvl;
    uint8_t clng_type;
    uint8_t flor_type;
} MapTileAttr;


#define MAX_SPRITE_IMAGES 12
#define MAX_SPRITE_IMG_KEY_CHAR 24

typedef struct _SpriteImageList {
    char img_keys[MAX_SPRITE_IMAGES][MAX_SPRITE_IMG_KEY_CHAR];
    size_t img_amm;
} SpriteImageList;

typedef struct _SpriteAnim {
    SpriteImageList img_list;
    size_t current_frame;
    int anim_fps;
    float dt; // summation of time per frame
    uint8_t b_running;
    uint8_t b_loop;
    uint8_t b_rtrn_aft_end; // return to the base rotation img / animaton after the animation has concluded
} SpriteAnim;

typedef struct _SpriteAttr {
    float pos[2];

    SpriteImageList rotation_imgs;

    SpriteAnim special_anim;

    float size;
    float a;
    uint8_t b_spin;
    uint8_t b_frontfacing;
    int spin_spd; // in frames per second
} SpriteAttr;

#define MapTileAttr_INIT(X) MapTileAttr X = {.light_lvl = 255}

#define MAP_MAX_FACETEX 32
#define MAP_MAX_FACETEX_CHAR 32

typedef struct _Map {
    float wall_height;
    char face_tex_idx[MAP_MAX_FACETEX][MAP_MAX_FACETEX_CHAR];
    ImageData* face_tex_data[MAP_MAX_FACETEX];
    uint8_t mapH[MAP_MAX_H];               // horizontal wall face type
    uint8_t mapV[MAP_MAX_V];               // vertical wall face type
    MapTileAttr mapT[MAP_MAX_X*MAP_MAX_Y]; // map tile attributes
    SpriteAttr sprites[MAX_MAP_SPRITES];
    size_t sprites_amm;
    char* sky_texture;
} Map;

size_t Map_addSprite(Map* map, SpriteAttr sprite);
Map Map_init();
int Map_initFaceTexData(Map* map);
int Map_setFaceTexIdx(Map* map, const char* str, int idx);

void SpriteImageList_init(SpriteImageList* list);

#endif