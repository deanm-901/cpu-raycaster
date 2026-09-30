#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "app.h"
#include "pos.h"

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
void renderLine(App* app, Pos2 p1, Pos2 p2);

#endif