// TODO 0: Cool sky
// TODO 1: Multiple types of walls, wall attributes

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "defs.h"
#include "app.h"
#include "render.h"
#include "colors.h"
#include "player.h"
#include "images.h"
#include "map.h"
#include "hashmap.h"

// there is one image holder per instance of the executable file
ImageHolder image_holder;

App app;
Player player = {.pos={1.5,1.5}, .vel={0,0}, .a=PI/2, .vela=0.0, .speed=1.5, .turn_speed=2, .height=0.5, .size=0.1};

Map map = {.wall_height = 3.0};

char *title = SCREEN_NAME;

// in ms
Uint64 TIME_NOW = 0;
Uint64 TIME_LAST = 0;
// in seconds / frame
double dt = 1;

RayData ray_data[(int)(SCREEN_WIDTH/RAYCAST_RES)];

static void renderBackground() {
    Rect rect = {.x=0, .y=0, .w=SCREEN_WIDTH, .h=SCREEN_HEIGHT};

    renderImage(&app, getImageData("sky"), rect);
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
        if (map.mapH[wh] == T_NONE) continue;

        int x = MAP_REP_SCALE*(wh % MAP_MAX_X), y = MAP_REP_SCALE*(wh / MAP_MAX_X);

        int p1[2] = {x,y}, p2[2] = {x+MAP_REP_SCALE, y};

        renderLine(&app, p1, p2, C_WHITE);
    } 
    for (int wv=0;wv<MAP_MAX_V;wv++) { // vertical
        if (map.mapV[wv] == T_NONE) continue;

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
    //for (int i=0; i<sizeof(ray_data)/sizeof(RayData); i++) {
    //    int r2[2] = {
    //        ray_data[i].end_pos[X]*MAP_REP_SCALE,
    //        ray_data[i].end_pos[Y]*MAP_REP_SCALE
    //    };
    //
    //    renderLine(&app, p1, r2, C_GREEN);
    //}
    
    renderLine(&app, p1, p2, C_RED);
    //actual player rect
    Rect player_rect = {.x=(player.pos[X]*MAP_REP_SCALE)-(MAP_REP_PLAYER_SIZE/2), .y=(player.pos[Y]*MAP_REP_SCALE)-(MAP_REP_PLAYER_SIZE/2),
                        .w=MAP_REP_PLAYER_SIZE, .h=MAP_REP_PLAYER_SIZE,
                        .color=C_RED};

    renderRect(&app, player_rect);
}

static void handleInput() {
        player.inpa = 0;
        player.inpap = 0;
    
        SDL_Event input;
        while(SDL_PollEvent(&input)) {
            switch (input.type) {
                case SDL_QUIT:
                    exit(0);
                    break;
                case SDL_KEYDOWN:
                    if (input.key.keysym.scancode == SDL_SCANCODE_ESCAPE) exit(0);

                    Player_handleKeyboardInput(&player, &input.key, 1);
                    break;
                case SDL_KEYUP:
                    Player_handleKeyboardInput(&player, &input.key, -1);
                    break;
                case SDL_MOUSEMOTION:
                    Player_handleMouseMotion(&player, &input.motion);
                    break;
                default:
                    break;
            }
        }
}

static void raycast() {
    int amm = sizeof(ray_data)/sizeof(RayData);

    for (int i=0; i<amm; i++) {
        double deg_amm = i*FOV/amm;

        ray_data[i] = Player_castRay(&player, &map, player.a+deg_amm-(FOV/2.0));

        renderStrip(&app, &ray_data[i], &player, &map, i, deg_amm-(FOV/2.0));
    }
}

static void renderRaycast() {
    int amm = sizeof(ray_data)/sizeof(RayData);

    for (int i=0; i<amm; i++) {
        double deg_amm = i*FOV/amm;

    }
}

static void cleanup() {
    App_free(&app);
    SDL_Quit();
    freeImageHolder();
}

static void initImages() {
    setImageHolderIndex(png_homer, png_homer_size, "homer");
    setImageHolderIndex(png_pizza, png_pizza_size, "pizza");
    setImageHolderIndex(png_sky, png_sky_size, "sky");
}

int main(int argc, char* argv[]) {
    int debug = DEBUG;

    App_init(&app);

    for(int i=0;i<8;i++){ 
        map.mapH[i] = T_SOME;
        map.mapH[i+128] = T_SOME;

        map.mapV[17*i] = T_SOME;
        map.mapV[17*i+8] = T_SOME;

        for (int j=0;j<8;j++)
            map.mapC[i*MAP_MAX_X+j] = T_SOME;
    }
    map.mapV[17*3+8] = T_NONE;
    map.mapH[20] = T_SOME;
    map.mapH[21] = T_SOME;
    map.mapV[21] = T_SOME;
    map.mapV[38] = T_SOME;
    map.mapV[39] = T_SOME;

    memset(map.mapF, T_SOME, sizeof(map.mapF));
    
    image_holder = initImageHolder();
    
    initImages();

    atexit(cleanup);

    while(1) {
        TIME_LAST = TIME_NOW;
        TIME_NOW = SDL_GetTicks();
        dt = (TIME_NOW-TIME_LAST)/1000.0;

        handleInput();


        // velocity input conversion //
        Player_inputVelocity(&player);

        // collision //
        Player_handleCollision(&player, &map, dt, &app);

        // player velocity //
        Player_handleVelocity(&player, dt);

        // render frame //
        App_clearBuffer(&app);

        renderBackground();

        raycast();

        if (debug) {
            renderRepFloor();

            renderRepWalls();

            renderRepPlayer();
        }

        presentFrame(&app);

        // delays every frame by ms; helps free CPU time
        SDL_Delay(4);
    }

}