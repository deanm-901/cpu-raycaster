#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "app.h"
#include "player.h"

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
void renderStrip(App* app, RayData* data, int posX, int addY, float deg_amm);

#endif