#include "image-holder.h"
#include <stdlib.h>
#include <png.h>
#include "hashmap.h"

extern ImageHolder image_holder;

void setImageHolderIndex(unsigned char png_data[], size_t png_size, const char* key) {
    ImageData* data = calloc(1, sizeof *data);
    if (data == NULL) exit(1);

    data->image.version = PNG_IMAGE_VERSION;

    if (!png_image_begin_read_from_memory(&data->image, png_data, png_size)) {
        fprintf(stderr, "PNG read error: %s\n", data->image.message);
        exit(1);
    }

    data->image.format = PNG_FORMAT_RGBA;
    size_t count = (size_t)data->image.width * data->image.height;
    size_t bytes = PNG_IMAGE_SIZE(data->image);

    unsigned char *rgba = malloc(bytes);                // !!
    uint32_t *pixels = malloc(count * sizeof *pixels);  // !!

    if (!rgba || !pixels) {
        free(rgba);
        free(pixels);
        png_image_free(&data->image);
        exit(1);
    }

    if (!png_image_finish_read(&data->image, NULL, rgba, 0, NULL)) {
        fprintf(stderr, "PNG decode error: %s\n", data->image.message);
        png_image_free(&data->image);
        free(rgba);
        free(pixels);
        free(data);
        exit(1);
    }
    
    png_image_free(&data->image);

    for (size_t i=0; i<count; ++i) {
        uint32_t r = rgba[4*i+0];
        uint32_t g = rgba[4*i+1];
        uint32_t b = rgba[4*i+2];
        uint32_t a = rgba[4*i+3];

        pixels[i] = (a<<24)|(r<<16)|(g<<8)|b;
    }

    free(rgba);

    data->pixels = pixels;
    data->size = count;

    HashMap_set(image_holder.image_map, key, data, sizeof *data);
}

ImageData* getImageData(const char* key) {
    HashMapItem* item = HashMap_get(image_holder.image_map, key);
    
    return (ImageData*)item->value.value;
}

ImageHolder initImageHolder() {
    ImageHolder holder;

    holder.image_map = HashMap_init();

    return holder;
}

static void freeImageItem(HashMapItem* item) {
    if (item->key == NULL || item->value.value == NULL) return;

    ImageData* data = item->value.value;

    free(data->pixels);
    free(data);
}

void freeImageHolder() {
    HashMap *map = image_holder.image_map;
    if (map == NULL) return;

    for (size_t i = 0; i<HASHMAP_ITEMARR_SIZE; ++i) {
        freeImageItem(&map->item_arr[i]);
    }

    for (size_t i = 0; i<HASHMAP_ITEMARR_SIZE; ++i) {
        freeImageItem(&map->collision_arr[i]);
    }
    HashMap_free(image_holder.image_map);
}