#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>

typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *window;
    uint32_t* screen_pixels;
    SDL_Texture* screen_texture;
} App;

void App_init(App* app);
void App_clearBuffer(App* app);
void App_free(App* app);
void App_setPixel(App* app, int x, int y, uint32_t color);

#endif