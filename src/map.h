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
#define MAX_SPRITE_IMG_KEY_CHAR 16

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

#define ACTOR_MAX_DATA_PTRS 16

typedef struct _ActorAttr {
    SpriteAttr* map_sprite;
    float pos[2];
    void* data[ACTOR_MAX_DATA_PTRS];
} ActorAttr;

typedef struct _Actor {
    void (*Func_Init)(ActorAttr*);
    void (*Func_Frame)(ActorAttr*);
    void (*Func_Destroy)(ActorAttr*);

    ActorAttr attr;
} Actor;

#define MapTileAttr_INIT(X) MapTileAttr X = {.light_lvl = 255, .flor_type = 3}

#define MAP_MAX_FACETEX 32
#define MAP_MAX_FACETEX_CHAR 32
#define MAP_MAX_SPRITES 64
#define MAP_MAX_ACTORS  64

typedef struct _Map {
    // texture data
    char* sky_texture;
    char face_tex_idx[MAP_MAX_FACETEX][MAP_MAX_FACETEX_CHAR];
    ImageData* face_tex_data[MAP_MAX_FACETEX];

    // wall/ceiling/floor data
    float wall_height;
    uint8_t mapH[MAP_MAX_H];               // horizontal wall face type
    uint8_t mapV[MAP_MAX_V];               // vertical wall face type
    MapTileAttr mapT[MAP_MAX_X*MAP_MAX_Y]; // map tile attributes

    // sprite/actor data
    SpriteAttr sprites[MAP_MAX_SPRITES];
    size_t sprites_amm;
    Actor actors[MAP_MAX_ACTORS];
    size_t actors_amm;

    // player data
    float player_init_pos[2];
} Map;

Map Map_init();
size_t Map_addSprite(Map* map, SpriteAttr sprite);
size_t Map_addActor(Map* map, Actor actor);
int Map_initFaceTexData(Map* map);
int Map_setFaceTexIdx(Map* map, const char* str, int idx);
int Map_callActorFuncs(Map* map);
void Map_destroyActors(Map* map);

void SpriteImageList_init(SpriteImageList* list);

#endif