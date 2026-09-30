#include "app.h"
#include "defs.h"

void App_init(App* app) {
    int renderer_flags, window_flags;

    renderer_flags = SDL_RENDERER_ACCELERATED;
    window_flags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't initialize SDL2: %s\n", SDL_GetError());
        exit(1);
    }

    app->window = SDL_CreateWindow(SCREEN_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);

    if (!app->window) {
        printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    app->renderer = SDL_CreateRenderer(app->window, -1, renderer_flags);

    if (!app->renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }
    app->screen_pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));

    app->screen_texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void App_clearBuffer(App* app) {
    memset(app->screen_pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
}

void App_free(App* app) {
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    free(app->screen_pixels);
}