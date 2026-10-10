#ifndef IMAGES_H
#define IMAGES_H

#include <stdint.h>
#include <png.h>
#include "defs.h"

extern const unsigned char txt_test[];

typedef struct {
    png_image image;
    uint32_t* pixels;
} ImageData;

typedef struct {
    int nextFreeIndex;
    ImageData data[MAX_IMAGE_COUNT];
} ImageHolder;

/// @brief Compiles images using raw byte data and gets added to the ImageHolder struct
/// @param png_data Raw byte data 
/// @return Index of ImageData in ImageHolder
int setImageHolderIndex(unsigned char png_data[], size_t png_size);

/// @brief Gets a reference to a ImageData variable from the ImageHolder
/// @param index Index of ImageData variable
/// @return A reference to a ImageData variable
ImageData* getImageData(int index);

/// @brief Frees the ImageHolder
void freeImageHolder();

#endif