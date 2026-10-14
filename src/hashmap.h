#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>
#include <stddef.h>

#define HASHMAP_ITEMARR_SIZE 1000
#define HASHMAP_COLLARR_SIZE 1000
#define HASHMAP_RESIZE_FACTOR 2
#define HASHMAP_MAX_LOAD 0.5

typedef struct _HashMapValue {
    uint8_t* value;
    size_t value_size;
} HashMapValue;

typedef struct _HashMapItem {
    size_t next_idx;
    const char* key;
    HashMapValue value;
} HashMapItem;

typedef struct _HashMap {
    HashMapItem item_arr[HASHMAP_ITEMARR_SIZE];
    HashMapItem* collision_arr;
    size_t collision_arr_size;
    size_t collision_arr_count;
} HashMap;

static uint32_t hash(const char* key);
static int HashMap_resize(HashMap* m);

HashMap* HashMap_init();
void HashMap_free(HashMap* m);
int HashMap_set(HashMap* m, const char* key, uint8_t* value, size_t value_size);

#endif