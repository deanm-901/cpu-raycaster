#include "hashmap.h"
#include <string.h>
#include <stdlib.h>

static uint32_t hash(const char* key) {
    size_t len = strlen(key);
    uint32_t hash = 0;

    for (int i=0;i<len;i++) {
        hash += key[i];
    }

    return hash % HASHMAP_ITEMARR_SIZE;
}

HashMap* HashMap_init() {
    HashMap* m = malloc(sizeof(HashMap));
    if (m==NULL) return NULL;

    m->collision_arr_size = HASHMAP_COLLARR_SIZE;
    m->collision_arr = calloc(HASHMAP_COLLARR_SIZE, sizeof(HashMapItem));

    memset(m->item_arr, 0, sizeof(m->item_arr));

    m->last_deleted_col_idx = 0;

    return m;
}

void HashMap_free(HashMap* m) {
    free(m->collision_arr);
    free(m);
}

static int HashMap_resize(HashMap* m) {
    size_t old_collarr_size  = m->collision_arr_size;
    HashMapItem* old_collarr = m->collision_arr;

    m->collision_arr_size *= HASHMAP_RESIZE_FACTOR;
    m->collision_arr = calloc(m->collision_arr_size, sizeof(HashMapItem));

    if (m->collision_arr == NULL) {
        m->collision_arr = old_collarr;
        m->collision_arr_size = old_collarr_size;
        return 1;
    }

    return 0;
}

static size_t HashMap_findNxtCol(HashMap* m) {
    size_t idx;
    
    if (m->collision_arr[m->last_deleted_col_idx].key == NULL) {
        idx = m->last_deleted_col_idx;
    } else {
        if (m->collision_arr[m->collision_arr_count].key != NULL) m->collision_arr_count++;

        idx = m->collision_arr_count;
    }

    return idx;
}

static int HashMap_setNxtCol(HashMap* m, HashMapItem item, uint32_t hsh) {
    size_t idx;
    HashMapItem* current_item = &m->item_arr[hsh];

    while (current_item->next_idx != NULL) {
        idx = current_item->next_idx;

        current_item = &m->collision_arr[idx];
    }

    idx = HashMap_findNxtCol(m);

    current_item->next_idx = idx;
    m->collision_arr[idx] = item;
}

int HashMap_set(HashMap* m, const char* key, void* value, size_t value_size) {
    uint32_t hsh = hash(key);

    HashMapValue new_value = {
        .value = value,
        .value_size = value_size,
    };

    HashMapItem new_item = {
        .key = key,
        .value = new_value,
    };

    if (m->item_arr[hsh].key == NULL) {
        m->item_arr[hsh] = new_item;
    } else {
        HashMap_setNxtCol(m, new_item, hsh);
    }

    if (m->collision_arr_count > m->collision_arr_size * HASHMAP_MAX_LOAD)
        HashMap_resize(m);
}