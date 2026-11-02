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

size_t Map_addSprite(Map* map, SpriteAttr sprite) {
    size_t idx = map->sprites_amm;

    checkMark(&sprite.rotation_imgs);

    map->sprites[idx] = sprite;

    map->sprites_amm++;

    return idx;
}