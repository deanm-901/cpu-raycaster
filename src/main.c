// TODO: 

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "defs.h"
#include "app.h"
#include "render.h"
#include "colors.h"
#include "player.h"

App app;
Player player = {.x=0.0, .y=0.0, .a=0.0, .speed=60};

// temp
int map_width  = 9;
int map_height = 7;
int map[] = {
    1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,1,0,1,
    1,0,0,1,1,0,1,0,1,
    1,0,0,1,0,0,1,0,1,
    1,0,0,1,0,0,1,0,1,
    1,1,1,1,1,1,1,1,1,
};

static void cleanup() {
    App_free(&app);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    App_init(&app);

    atexit(cleanup);

    // in ms
    Uint64 TIME_NOW = SDL_GetTicks();
    Uint64 TIME_LAST = 0;
    // in seconds / frame
    double dt = 1;

    while(1) {
        TIME_LAST = TIME_NOW;
        TIME_NOW = SDL_GetTicks();
        dt = (TIME_NOW-TIME_LAST)/1000.0;

        printf("FPS: %f\n", (double)1/dt);

        // input //
        SDL_Event input;
        while(SDL_PollEvent(&input)) {
            if (input.type == SDL_QUIT) exit(0);

            if (input.type == SDL_KEYDOWN) {
                Player_handleKeyboardInput(&player, &input.key, 1);
            }
            if (input.type == SDL_KEYUP) {
                Player_handleKeyboardInput(&player, &input.key, 0);
            }
        }

        // velocity //
        Player_inputVelocity(&player, dt);

        // render frame //
        App_clearBuffer(&app);

        // background
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                app.screen_pixels[y*SCREEN_WIDTH+x] = pixelBackground(x,y, TIME_NOW);
            }
        }

        // 2d map representation
        for (int ry=0;ry<map_height;ry++) {
            for (int rx=0;rx<map_width;rx++) {
                Rect rect = {.x=60*rx+1, .y=60*ry+1,
                             .w=59, .h=59,
                             .color=map[ry*map_width+rx]==1?C_WHITE:C_BLACK};
                renderRect(&app, rect);
            }
        }

        // 2d player representation
        Rect player_rect = {.x=player.x-4, .y=player.y-4,
                            .w=8, .h=8,
                            .color=C_RED};
        renderRect(&app, player_rect);

        presentFrame(&app);

        // delays every frame by ms; helps free CPU time
        SDL_Delay(4);
    }

}