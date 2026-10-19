#include "images.h"
#include <stdlib.h>
#include <png.h>
#include "hashmap.h"

unsigned char png_homer[]  = {
    #embed "../images/homer.png"
};
size_t png_homer_size = sizeof png_homer;

unsigned char png_pizza[]  = {
    #embed "../images/pizza.png"
};
size_t png_pizza_size = sizeof png_pizza;

unsigned char png_sky[]  = {
    #embed "../images/sky.png"
};
size_t png_sky_size = sizeof png_sky;



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
        free(rgba);
        free(pixels);
        png_image_free(&data->image);
        exit(1);
    }

    for (size_t i=0; i<count; ++i) {
        uint8_t r = rgba[4*i+0];
        uint8_t g = rgba[4*i+1];
        uint8_t b = rgba[4*i+2];
        uint8_t a = rgba[4*i+3];

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

void freeImageHolder() {
    HashMap_free(image_holder.image_map);
}