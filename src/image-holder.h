#ifndef IMAGE_HOLDER_H
#define IMAGE_HOLDER_H

#include <stdint.h>
#include <png.h>
#include "defs.h"
#include "hashmap.h"

typedef struct {
    png_image image;
    uint32_t* pixels;
    size_t size;
} ImageData;

typedef struct {
    HashMap* image_map;
} ImageHolder;

/// @brief Compiles images using raw byte data and gets added to the ImageHolder struct
/// @param png_data Raw byte data 
/// @return Index of ImageData in ImageHolder
void setImageHolderIndex(unsigned char png_data[], size_t png_size, const char* key);

/// @brief Gets a reference to a ImageData variable from the ImageHolder
/// @param index Index of ImageData variable
/// @return A reference to a ImageData variable
ImageData* getImageData(const char* key);
ImageHolder initImageHolder();

/// @brief Frees the ImageHolder
void freeImageHolder();

#endif