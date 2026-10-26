#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "app.h"
#include "map.h"
#include "player.h"
#include "image-holder.h"
#include "threads.h"

typedef struct {
    int x;
    int y;
    int w;
    int h;
    uint32_t color;
} Rect;

void prepareFrame(App* app);
void presentFrame(App* app);

int pixelBackground(int x, int y, double ticks);

void renderRect(App* app, Rect rect);
void renderLine(App* app, int p1[2], int p2[2], uint32_t color);
void renderStrip(App* app, RayData* data, Player* player, Map* map, int posX, float deg_amm);
void renderImage(App* app, ImageData* data, Rect rect, float zbuffer);
void renderText(App* app, ImageData* font, uint8_t tile_w, uint8_t tile_h, const char* text, uint32_t pos_x, uint32_t pos_y, int scale, uint32_t color);
void renderSprite(SpriteAttr* sprite, App* app, Player* player);

void* asyncRenderStripChunk(void* ctx);

#endif