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
#include "map.h"

App app;
Player player = {.pos={1.5,1.5}, .vel={0,0}, .a=0.0, .vela=0.0, .speed=1.5, .turn_speed=3};

Map map;

// in ms
Uint64 TIME_NOW = 0;
Uint64 TIME_LAST = 0;
// in seconds / frame
double dt = 1;

static void renderRepBackground() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            App_setPixel(&app, x,y, pixelBackground(x,y, TIME_NOW));
        }
    }
}

static void renderRepFloor() {
    for (int ry=0;ry<MAP_MAX_Y;ry++) {
        for (int rx=0;rx<MAP_MAX_X;rx++) {
            Rect rect = {.x=MAP_REP_SCALE*rx+1, .y=MAP_REP_SCALE*ry+1,
                         .w=MAP_REP_SCALE-1, .h=MAP_REP_SCALE-1,
                         .color=C_BLACK};
            renderRect(&app, rect);
        }
    }
}

static void renderRepWalls() {
    for (int wh=0;wh<MAP_MAX_H;wh++) { // horizontal
        if (map.mapH[wh] == NONE) continue;

        int x = MAP_REP_SCALE*(wh % MAP_MAX_X), y = MAP_REP_SCALE*(wh / MAP_MAX_X);

        int p1[2] = {x,y}, p2[2] = {x+MAP_REP_SCALE, y};

        renderLine(&app, p1, p2, C_WHITE);
    } 
    for (int wv=0;wv<MAP_MAX_V;wv++) { // vertical
        if (map.mapV[wv] == NONE) continue;

        int x = MAP_REP_SCALE*(wv % (MAP_MAX_X+1)), y = MAP_REP_SCALE*(wv / (MAP_MAX_X+1));

        int p1[2] = {x,y}, p2[2] = {x, y+MAP_REP_SCALE};

        renderLine(&app, p1, p2, C_WHITE);
    } 
}

static void renderRepPlayer() {
    //angle line
    int p1[2] = {player.pos[X]*MAP_REP_SCALE, 
                 player.pos[Y]*MAP_REP_SCALE}, 
        p2[2] = {player.pos[X]*MAP_REP_SCALE+cos(player.a)*MAP_REP_PLAYER_SIZE*2, 
                 player.pos[Y]*MAP_REP_SCALE+sin(player.a)*MAP_REP_PLAYER_SIZE*2};

    //raycasting line test
    RayData data = Player_castRay(&player, &map, player.a);
    
    int r2[2] = {
        data.end_pos[X]*MAP_REP_SCALE,
        data.end_pos[Y]*MAP_REP_SCALE
    };

    renderLine(&app, p1, r2, C_GREEN);
    renderLine(&app, p1, p2, C_RED);
    
    //actual player rect
    Rect player_rect = {.x=(player.pos[X]*MAP_REP_SCALE)-(MAP_REP_PLAYER_SIZE/2), .y=(player.pos[Y]*MAP_REP_SCALE)-(MAP_REP_PLAYER_SIZE/2),
                        .w=MAP_REP_PLAYER_SIZE, .h=MAP_REP_PLAYER_SIZE,
                        .color=C_RED};
    renderRect(&app, player_rect);
}

static void handleInput() {
        SDL_Event input;
        while(SDL_PollEvent(&input)) {
            switch (input.type) {
                case SDL_QUIT:
                    exit(0);
                    break;
                case SDL_KEYDOWN:
                    Player_handleKeyboardInput(&player, &input.key, 1);
                    break;
                case SDL_KEYUP:
                    Player_handleKeyboardInput(&player, &input.key, -1);
                    break;
                default:
                    break;
            }
        }
}

static void cleanup() {
    App_free(&app);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    App_init(&app);

    for(int i=0;i<8;i++){ 
        map.mapH[i] = SOME;
        map.mapH[i+128] = SOME;

        map.mapV[17*i] = SOME;
        map.mapV[17*i+8] = SOME;
    }
    map.mapH[20] = SOME;
    map.mapH[21] = SOME;
    map.mapV[21] = SOME;
    map.mapV[38] = SOME;
    map.mapV[39] = SOME;

    atexit(cleanup);

    while(1) {
        TIME_LAST = TIME_NOW;
        TIME_NOW = SDL_GetTicks();
        dt = (TIME_NOW-TIME_LAST)/1000.0;

        //printf("FPS: %f\n", (double)1/dt);

        handleInput();

        // velocity //
        Player_inputVelocity(&player, dt);

        // render frame //
        App_clearBuffer(&app);

        renderRepBackground();

        renderRepFloor();

        renderRepWalls();

        renderRepPlayer();

        presentFrame(&app);

        // delays every frame by ms; helps free CPU time
        SDL_Delay(4);
    }

}