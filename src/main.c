// TODO 0: Add Sprites

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#include <SDL2/SDL.h>

#include "colors.h"
#include "defs.h"
#include "app.h"
#include "render.h"
#include "player.h"
#include "image-holder.h"
#include "map.h"
#include "hashmap.h"
#include "threads.h"
#include "assets.h"

// there is one image holder per instance of the executable file
ImageHolder image_holder;

FaceAttr face_types[MAX_FACETYPE_COUNT];

App app;
Player player = {.pos={1.5,1.5}, .vel={0,0}, .a=PI/2, .vela=0.0, .speed=1.5, .turn_speed=2, .height=0.55, .size=0.1};

Map map = {.wall_height = 1.0};

// in ms
Uint64 TIME_NOW = 0;
Uint64 TIME_LAST = 0;
// in seconds / frame
double dt = 1;
char debug_text[64];

#define FPS_BUCKET_SIZE 16
float fps_bucket[FPS_BUCKET_SIZE];
size_t fps_bucket_count = 0;
float fps_avg = 0.0;

RayData ray_data[SCREEN_WIDTH];

#define REND_THREAD_AMM 4
RenderThread render_threads[REND_THREAD_AMM];

static void renderBackground() {
    double focal_length = SCREEN_WIDTH / (2.0 * tan(FOV / 2.0));

    ImageData* sky = getImageData("sky.png");

    int y_pos = (player.ap/(2*PI))*SCREEN_HEIGHT;
    int fill = SCREEN_HEIGHT/4;

    Rect rect1 = {.x=(-player.a/(2*PI))*SCREEN_WIDTH,                .y=y_pos-fill/2, .w=SCREEN_WIDTH, .h=SCREEN_HEIGHT+fill};
    Rect rect2 = {.x=(-player.a/(2*PI))*SCREEN_WIDTH + SCREEN_WIDTH, .y=y_pos-fill/2, .w=SCREEN_WIDTH, .h=SCREEN_HEIGHT+fill};

    renderImage(&app, sky, rect1);
    renderImage(&app, sky, rect2);
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
                    if (input.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        int w,h;
                        SDL_GetWindowSize(app.window, &w, &h);
                        SDL_WarpMouseInWindow(app.window, w/2, h/2);
                    
                        SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());
                    }

                    if (input.key.keysym.scancode == SDL_SCANCODE_F11) {
                        SDL_SetWindowFullscreen(app.window, 
                            (SDL_GetWindowFlags(app.window)&SDL_WINDOW_FULLSCREEN_DESKTOP)?
                                0:SDL_WINDOW_FULLSCREEN_DESKTOP
                        );
                    }

                    Player_handleKeyboardInput(&player, &input.key, 1);
                    break;
                case SDL_KEYUP:
                    Player_handleKeyboardInput(&player, &input.key, -1);
                    break;
                case SDL_MOUSEMOTION:
                    Player_handleMouseMotion(&player, &input.motion, &app);
                    break;
                default:
                    break;
            }
        }
}

static void raycast() {
    const int amm = SCREEN_WIDTH;

    for (int i=0; i<amm; i++) {
        double deg_amm = i*FOV/amm;

        ray_data[i] = Player_castRay(&player, &map, player.a+deg_amm-(FOV/2.0));

        renderStrip(&app, &ray_data[i], &player, &map, i, deg_amm-(FOV/2.0));
    }

    //submit jobs
    for (int i=0;i<REND_THREAD_AMM;i++) RenderThreadSubmitJob(&render_threads[i]);

    // wait
    for (int i=0;i<REND_THREAD_AMM;i++) RenderThreadAwaitCompletion(&render_threads[i]);
}

static void renderRaycast() {
    int amm = sizeof(ray_data)/sizeof(RayData);

    for (int i=0; i<amm; i++) {
        double deg_amm = i*FOV/amm;
    }
}

static void initFaceTypes() {
    FaceAttr homer = {.isCollidable=1,.atlas_pos={1,0}};
    face_types[T_HOMER] = homer;
    FaceAttr pizza = {.isCollidable=1,.atlas_pos={2,0}};
    face_types[T_PIZZA] = pizza;
    FaceAttr grass = {.isCollidable=1,.atlas_pos={0,0}};
    face_types[T_GRASS] = grass;
    FaceAttr bricks = {.isCollidable=1,.atlas_pos={0,1}};
    face_types[T_BRICK] = bricks;
    FaceAttr tiles = {.isCollidable=1,.atlas_pos={1,1}};
    face_types[T_TILES] = tiles;
}

static void initImages() {
    InitAssetData();
}

static void initMapDefaults() {
    for (int i=0;i<MAP_MAX_X*MAP_MAX_Y;i++) {
        MapTileAttr_INIT(def);
        map.mapT[i] = def;
    }
}

static void renderDebugText() {
    // fps
    float current_fps = 1.0/dt;
    fps_bucket[fps_bucket_count++] = current_fps;

    if (fps_bucket_count >= FPS_BUCKET_SIZE) { // print average
        float fps_sum = 0.0f;
        for (int i=0;i<FPS_BUCKET_SIZE;i++) fps_sum += fps_bucket[i];

        fps_avg = fps_sum/FPS_BUCKET_SIZE;
        fps_bucket_count = 0;
    }

    snprintf(debug_text, sizeof(debug_text), "FPS: %0.2f\nPOS: (%0.2f,%0.2f)", fps_avg, player.pos[X], player.pos[Y]);

    renderText(&app, getImageData("font1.png"), 8, 8, debug_text, MAP_MAX_X*MAP_REP_SCALE, 0, 1, C_WHITE);
}

static void initThreads() {
    static int pixAmm = SCREEN_WIDTH/REND_THREAD_AMM;

    for (int i=0;i<REND_THREAD_AMM;i++)
        InitRenderThread(&render_threads[i], pixAmm*i, pixAmm*(i+1), asyncRenderStripChunk);
}

static void destroyThreads() {
    for (int i=0;i<REND_THREAD_AMM;i++)
        DestroyRenderThread(&render_threads[i]);
}


static void limitFPS() {
    const int display_num = SDL_GetWindowDisplayIndex(app.window);
    SDL_DisplayMode display;
    SDL_GetDisplayMode(display_num, 0, &display);
    const int hz = display.refresh_rate;
    
    const int ticks_per_frame = 1000/hz;
    const int frame_ticks = SDL_GetTicks() - TIME_NOW;

    const int diff = ticks_per_frame - frame_ticks;

    if (diff > 0) {
        SDL_Delay(diff);
    }
}

static void cleanup() {
    freeImageHolder();
    destroyThreads();
    App_free(&app);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    int debug = DEBUG;

    App_init(&app);

    initMapDefaults();

    for(int i=0;i<8;i++){ 
        map.mapH[i] = T_BRICK;
        map.mapH[i+128] = T_BRICK;

        map.mapV[17*i] = T_BRICK;
        map.mapV[17*i+8] = T_BRICK;

        for (int j=0;j<8;j++) {
            map.mapT[i*MAP_MAX_X+j].clng_type = T_BRICK;
            map.mapT[i*MAP_MAX_X+j].flor_type = T_TILES;
            map.mapT[i*MAP_MAX_X+j].light_lvl = 255*pow(0.85,j+i);
        }
    }
    map.mapV[17*3+8] = T_NONE;
    map.mapH[20] = T_HOMER;
    map.mapH[21] = T_PIZZA;
    map.mapV[21] = T_BRICK;
    map.mapV[38] = T_BRICK;
    map.mapV[39] = T_BRICK;
    
    image_holder = initImageHolder();
    
    initImages();
    initThreads();
    initFaceTypes();

    atexit(cleanup);

    for(;;) {
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

        // player anim //
        Player_walkAnimation(&player, dt);

        // render frame //
        App_clearBuffer(&app);

        renderBackground();

        raycast();

        if (debug) {
            renderRepFloor();

            renderRepWalls();

            renderRepPlayer();

            renderDebugText();
        }

        presentFrame(&app);

        // delays every frame if over hz; helps free CPU time
        limitFPS();
    }
}