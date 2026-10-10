#include "images.h"
#include <stdlib.h>
#include <png.h>

const unsigned char txt_test[]  = {
    #embed "../img/test.txt"
};

extern ImageHolder image_holder;

int setImageHolderIndex(unsigned char png_data[], size_t png_size) {
    if (image_holder.nextFreeIndex >= MAX_IMAGE_COUNT) {
        fprintf(stderr, "Max Image Ammount Reached!\n");
        return -1;
    }

    ImageData data;

    data.image.version = PNG_IMAGE_VERSION;

    if (!png_image_begin_read_from_memory(&data.image, png_data, png_size)) {
        fprintf(stderr, "PNG read error: %s\n", data.image.message);
        exit(1);
    }

    data.image.format = PNG_FORMAT_RGBA;
    size_t count = (size_t)data.image.width * data.image.height;
    size_t bytes = PNG_IMAGE_SIZE(data.image);

    unsigned char *rgba = malloc(bytes);                // !!
    uint32_t *pixels = malloc(count * sizeof *pixels);  // !!

    if (!rgba || !pixels) {
        free(rgba);
        free(pixels);
        png_image_free(&data.image);
        exit(1);
    }

    if (!png_image_finish_read(&data.image, NULL, pixels, 0, NULL)) {
        fprintf(stderr, "PNG decode error: %s\n", data.image.message);
        free(rgba);
        free(pixels);
        png_image_free(&data.image);
        exit(1);
    }

    for (size_t i=0; i<count; ++i) {
        uint32_t r = rgba[4*i+0];
        uint32_t g = rgba[4*i+1];
        uint32_t b = rgba[4*i+2];
        uint32_t a = rgba[4*i+3];

        pixels[i] = (a<<24)|(r<<16)|(g<<8)|b;
    }

    free(rgba);

    data.pixels = pixels;

    int idx = image_holder.nextFreeIndex;
    image_holder.data[idx] = data;
    image_holder.nextFreeIndex++;
    
    return idx;
}

ImageData* getImageData(int index) {
    return &image_holder.data[index];
}

void freeImageHolder() {
    for (int i=0; i<MAX_IMAGE_COUNT; i++) {
        free(&image_holder.data[i].pixels);
        png_image_free(&image_holder.data[i].image);
    }
}