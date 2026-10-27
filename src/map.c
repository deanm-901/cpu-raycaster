#include "map.h"
#include <string.h>
#include <stdio.h>

size_t Map_addSprite(Map* map, SpriteAttr sprite) {
    size_t idx = map->sprites_amm;

    char pattern[MAX_SPRITE_IMG_KEY_CHAR];
    memcpy(pattern, sprite.img_keys[0], sizeof pattern);
    pattern[sizeof pattern - 1] = '\0';

    char *mark = strstr(pattern, "%n");
    if (mark != NULL) {

        size_t prefix_len = (int)(mark-pattern);
        const char *suffix = mark+2;

        for (int i=0;i<sprite.img_amm;i++) {
            int len = snprintf(sprite.img_keys[i], sizeof sprite.img_keys[i], "%.*s%zu%s", (int)prefix_len, pattern, i, suffix);
        }
    }

    map->sprites[idx] = sprite;

    map->sprites_amm++;

    return idx;
}