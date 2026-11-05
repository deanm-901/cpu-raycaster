#include "map.h"
#include <string.h>
#include <stdio.h>

static void checkMark(SpriteImageList* img_list) {
    char pattern[MAX_SPRITE_IMG_KEY_CHAR];
    memcpy(pattern, img_list->img_keys[0], sizeof pattern);
    pattern[sizeof pattern - 1] = '\0';

    char *mark = strstr(pattern, "%n");
    if (mark != NULL) {

        size_t prefix_len = (int)(mark-pattern);
        const char *suffix = mark+2;

        for (int i=0;i<img_list->img_amm;i++) {
            snprintf(img_list->img_keys[i], MAX_SPRITE_IMG_KEY_CHAR, "%.*s%d%s", (int)prefix_len, pattern, i, suffix);
            
            printf("[checkMark Debug] Generated Key [%d]: '%s' at address %p\n", 
                   i, img_list->img_keys[i], (void*)img_list->img_keys[i]);
        }
    }
}
void SpriteImageList_init(SpriteImageList* list) {
    checkMark(list);
}

Map Map_init() {
    Map map = {0};

    map.player_init_pos[X] = 1.5;
    map.player_init_pos[Y] = 1.5;

    for (int i=0;i<MAP_MAX_X*MAP_MAX_Y;i++) {
        MapTileAttr_INIT(def);
        map.mapT[i] = def;
    }

    return map;
}

int Map_setFaceTexIdx(Map* map, const char* str, int idx) {
    if (idx >= MAP_MAX_FACETEX || idx < 1) return 1;

    snprintf(map->face_tex_idx[idx], sizeof(map->face_tex_idx[idx]), str);

    return 0;
}

int Map_initFaceTexData(Map* map) {
    for (int i=0;i<MAP_MAX_FACETEX;i++) {
        map->face_tex_data[i] = NULL;

        char str[MAP_MAX_FACETEX_CHAR];
        strncpy(str, map->face_tex_idx[i], MAP_MAX_FACETEX_CHAR - 1);
        str[MAP_MAX_FACETEX_CHAR-1] = '\0';

        if (str[0] == '\0') continue;

        ImageData* ptr = getImageData(str);

        if (ptr == NULL) return 1;
        
        map->face_tex_data[i] = ptr;
    }

    return 0;
}

size_t Map_addSprite(Map* map, SpriteAttr sprite) {
    size_t idx = map->sprites_amm;

    checkMark(&sprite.rotation_imgs);

    map->sprites[idx] = sprite;

    map->sprites_amm++;

    return idx;
}

size_t Map_addActor(Map* map, Actor actor){
    size_t idx = map->actors_amm;
    
    map->actors[idx] = actor;

    (*map->actors[idx].Func_Init)(&map->actors[idx].attr);

    map->actors_amm++;

    return idx;
}

int Map_callActorFuncs(Map* map) {
    for (int i=0;i<map->actors_amm; i++) {
        (*map->actors[i].Func_Frame)(&map->actors[i].attr);
    }
}