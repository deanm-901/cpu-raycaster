#include "app.h"
#include "defs.h"

void App_init(App* app) {
    int renderer_flags, window_flags;

    renderer_flags = SDL_RENDERER_ACCELERATED;
    window_flags = SDL_WINDOW_RESIZABLE;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't initialize SDL2: %s\n", SDL_GetError());
        exit(1);
    }

    app->window = SDL_CreateWindow(SCREEN_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH*SCREEN_RESOLUTION_SCALE, SCREEN_HEIGHT*SCREEN_RESOLUTION_SCALE, window_flags);

    if (!app->window) {
        printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        exit(1);
    }

    SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_SCALING, "0");

    app->renderer = SDL_CreateRenderer(app->window, -1, renderer_flags);

    if (!app->renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }
    app->screen_pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t)); // !!
    app->screen_zbuffer = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(float)); // !!

    app->screen_texture = SDL_CreateTexture(app->renderer, 
                                            SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 
                                            SCREEN_WIDTH, 
                                            SCREEN_HEIGHT);

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_RenderSetLogicalSize(app->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void App_clearBuffer(App* app) {
    memset(app->screen_pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
    memset(app->screen_zbuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(float));
}

void App_free(App* app) {
    SDL_DestroyTexture(app->screen_texture);
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    free(app->screen_pixels);
    free(app->screen_zbuffer);
}

void App_setPixel(App* app, int x, int y, uint32_t color, float zbuffer) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;

    if ((color>>24 & 0xFF) != 0xFF) return;

    size_t idx = (size_t)y * SCREEN_WIDTH + (size_t)x;
    
    if (app->screen_zbuffer[idx] > zbuffer) return;

    app->screen_pixels[idx] = color;
    app->screen_zbuffer[idx] = zbuffer;
}