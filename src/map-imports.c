#include "map-imports.h"
#include "hashmap.h"
#include "map.h"
#include <string.h>
static unsigned char map_0[] = {
#embed "../maps/test.map"
};

static HashMap* map_map;

static void setIndex(unsigned char data[], size_t data_size, const char* key) {
    Map* map = malloc(sizeof(Map));
    memcpy(map, data, data_size);

    HashMap_set(map_map, key, map, sizeof(Map));
}

void assignMapData() {
    map_map = HashMap_init();
    setIndex(map_0, sizeof(map_0), "test.map");
}

Map* getMapData(const char* key) {
    HashMapItem* item = HashMap_get(map_map, key);
    return (Map*)item->value.value;
}
