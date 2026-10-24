#include "hashmap.h"
#include <string.h>
#include <stdlib.h>

static void HashMap_initItem(HashMapItem* item) {
    item->key = NULL;
    item->value.value = NULL;
    item->value.value_size = 0;
    item->next_idx = HASHMAP_NO_IDX;
}

static size_t hash(const char* key) {
    size_t len = strlen(key);
    uint32_t hash = 0;

    for (int i=0;i<len;i++) {
        hash += key[i];
    }

    return hash % HASHMAP_ITEMARR_SIZE;
}

HashMap* HashMap_init() {
    HashMap* m = malloc(sizeof(*m));
    if (m==NULL) {
        free(m);
        return NULL;
    };

    m->collision_arr_size = HASHMAP_COLLARR_SIZE;
    m->collision_arr = malloc(HASHMAP_COLLARR_SIZE * sizeof(*m->collision_arr)); // !!
    if (m->collision_arr == NULL) {
        free(m);
        return NULL;
    }

    for (size_t i=0;i<HASHMAP_ITEMARR_SIZE;i++) {
        HashMap_initItem(&m->item_arr[i]);
    }

    for (size_t i=0;i<HASHMAP_COLLARR_SIZE;i++) {
        HashMap_initItem(&m->collision_arr[i]);
    }

    m->collision_arr_size = HASHMAP_COLLARR_SIZE;
    m->collision_arr_count = 0;
    m->last_deleted_col_idx = 0;

    return m;
}

void HashMap_free(HashMap* m) {
    if (m==NULL) return;

    free(m->collision_arr);
    free(m);
}

static int HashMap_resize(HashMap* m) {
    if (m == NULL || HASHMAP_RESIZE_FACTOR < 2) return 1;

    size_t old_size = m->collision_arr_size;

    if (old_size > SIZE_MAX / HASHMAP_RESIZE_FACTOR) return 1;
    size_t new_size = old_size * HASHMAP_RESIZE_FACTOR;

    if (new_size > SIZE_MAX / sizeof *m->collision_arr) return 1;

    HashMapItem* new_arr = realloc(m->collision_arr, new_size * sizeof *new_arr);
    if (new_arr == NULL) return 1;

    m->collision_arr = new_arr;
    m->collision_arr_size = new_size;

    for (size_t i = old_size; i < new_size; ++i) {
        HashMap_initItem(&m->collision_arr[i]);
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

static void HashMap_setNxtCol(HashMap* m, HashMapItem item, uint32_t hsh) {
    HashMapItem* current_item = &m->item_arr[hsh];

    while (current_item->next_idx >= 0) {
        current_item = &m->collision_arr[current_item->next_idx];
    }

    size_t idx = HashMap_findNxtCol(m);

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
        .next_idx = -1
    };

    if (m->item_arr[hsh].key == NULL) {
        m->item_arr[hsh] = new_item;
    } else {
        HashMap_setNxtCol(m, new_item, hsh);
    }

    if (m->collision_arr_count > m->collision_arr_size * HASHMAP_MAX_LOAD)
        HashMap_resize(m);
}

HashMapItem* HashMap_get(HashMap* m, const char* key) {
    uint32_t hsh = hash(key);

    HashMapItem* item = &m->item_arr[hsh];
    
    while (item != NULL) {
        if (item->key == NULL) return NULL;
        if (strcmp(item->key, key) == 0) return item;
        if (item->next_idx < 0) return NULL;

        item = &m->collision_arr[item->next_idx];
    }

    return NULL;
}