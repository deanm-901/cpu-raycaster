#include "map.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

void Map_free(Map* map) {
    for (int i=0;i<map->actors_amm;i++) {
        (*map->actors[i].Func_Destroy)(&map->actors[i].attr);
    }

    free(map->sky_texture);
}

static MapSaveData Map_getSaveData(Map* map) {
    MapSaveData data;

    strncpy(data.sky_texture, map->sky_texture, sizeof(data.sky_texture) - 1);
    data.sky_texture[sizeof(data.sky_texture) - 1] = '\0';

    memcpy(data.face_tex_idx, map->face_tex_idx, sizeof(data.face_tex_idx));

    data.wall_height = map->wall_height;
    memcpy(data.mapH, map->mapH, sizeof(data.mapH));
    memcpy(data.mapV, map->mapV, sizeof(data.mapV));
    memcpy(data.mapT, map->mapT, sizeof(data.mapT));

    memcpy(data.sprites, map->sprites, sizeof(data.sprites));
    data.sprites_amm = map->sprites_amm;
    memcpy(data.actors, map->actors, sizeof(data.actors));
    data.actors_amm = map->actors_amm;

    memcpy(data.player_init_pos, map->player_init_pos, sizeof(data.player_init_pos));

    return data;
}

static void Map_setFromSaveData(Map* map, MapSaveData* data) {
    map->sky_texture = malloc(sizeof(data->sky_texture));

    if (map->sky_texture != NULL) {
        memcpy(map->sky_texture, data->sky_texture, sizeof(data->sky_texture));

        map->sky_texture[sizeof(data->sky_texture)-1] = '\0';
    }

    memcpy(map->face_tex_idx, data->face_tex_idx, sizeof(map->face_tex_idx));

    map->wall_height = data->wall_height;
    memcpy(map->mapH, data->mapH, sizeof(map->mapH));
    memcpy(map->mapV, data->mapV, sizeof(map->mapV));
    memcpy(map->mapT, data->mapT, sizeof(map->mapT));

    memcpy(map->sprites, data->sprites, sizeof(map->sprites));
    map->sprites_amm = data->sprites_amm;
    memcpy(map->actors, data->actors, sizeof(map->actors));
    map->actors_amm = data->actors_amm;

    memcpy(map->player_init_pos, data->player_init_pos, sizeof(map->player_init_pos));

    Map_initFaceTexData(map);
}

void Map_writeToFile(Map* map, const char* name) {
    FILE *file;

    char nameArr[64];
    strncpy(nameArr, name, sizeof(nameArr)-1);
    nameArr[sizeof(nameArr)-1] = '\0';

    snprintf(nameArr, sizeof(nameArr), "%s.map", name);

    file = fopen(nameArr, "wb");
    if (file == NULL) {
        fprintf(stderr, "File %s failed to open for writing\n", name);
        return;
    }

    MapSaveData data = Map_getSaveData(map);

    size_t written = fwrite(&data, sizeof(data), 1, file);

    fclose(file);

    if (written != 1) {
        fprintf(stderr, "Error writing Map to file %s\n", name);
    }
}

void Map_readFromFile(Map* map, const char* name) {
    // get the data
    FILE *file = fopen(name, "rb");

    if (file == NULL) {
        fprintf(stderr, "File %s failed to open for reading\n", name);
        return;
    }

    MapSaveData* data = malloc(sizeof(MapSaveData));

    size_t read = fread(data, sizeof(MapSaveData), 1, file);

    fclose(file);

    if (read != 1) {
        fprintf(stderr, "Error reading from file %s. Map write aborted.\n", name);
        return;
    }

    // convert the data
    Map_setFromSaveData(map, data);

    free(data);
}