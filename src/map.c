#include "map.h"

size_t Map_addSprite(Map* map, SpriteAttr sprite) {
    size_t idx = map->sprites_amm;

    map->sprites[idx] = sprite;

    map->sprites_amm++;

    return idx;
}