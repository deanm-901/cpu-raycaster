#include "render.h"
#include "defs.h"
#include <math.h>

void prepareFrame(App* app) {
    // SDL_SetRenderDrawColor(app->renderer, 50,50,50,255);
    // SDL_RenderClear(app->renderer);
}

void presentFrame(App* app) {
    SDL_UpdateTexture(app->screen_texture, NULL, app->screen_pixels, SCREEN_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(app->renderer);
    SDL_RenderCopy(app->renderer, app->screen_texture, NULL, NULL);

    SDL_RenderPresent(app->renderer);
}

int pixelBackground(int x, int y, double ticks) {
    uint32_t pixel;
    
    uint8_t r = x/(float) SCREEN_WIDTH*0XFF;
    uint8_t g = y/(float)SCREEN_HEIGHT*0XFF;
    uint8_t b = 0xFF;

    pixel = (r<<16)|(g<<8)|(b);
    return pixel;
}

void renderRect(App* app, Rect rect) {
    for (int y = 0; y < rect.h; y++) {
        for (int x = 0; x < rect.w; x++) {
            int px = rect.x+x, py = rect.y+y;

            if (px < 0 || px >  SCREEN_WIDTH) continue;
            if (py < 0 || py > SCREEN_HEIGHT) continue;

            app->screen_pixels[(SCREEN_WIDTH*rect.y+rect.x)+(SCREEN_WIDTH*y+x)] = rect.color;
        }
    }
}

void renderLine(App* app, Pos2 p1, Pos2 p2) {
    
}