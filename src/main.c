// TODO 0: Actors
// TODO 1: Map Editor

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

App app;
Player player = {.pos={1.5,1.5}, .vel={0,0}, .a=PI/2, .vela=0.0, .speed=2.0, .turn_speed=2, .height=0.55, .size=0.1};

Map map;

// in ms
Uint64 TIME_NOW = 0;
Uint64 TIME_LAST = 0;
// in seconds / frame
double dt = 1;
char debug_text[128];

#define FPS_BUCKET_SIZE 16
float fps_bucket[FPS_BUCKET_SIZE];
size_t fps_bucket_count = 0;
float fps_avg = 0.0;
GraphData fps_graph_data = {0};

RayData ray_data[SCREEN_WIDTH];

#define REND_THREAD_AMM 4
RenderThread render_threads[REND_THREAD_AMM];

static void renderBackground() {
    double focal_length = SCREEN_WIDTH / (2.0 * tan(FOV / 2.0));

    ImageData* sky = getImageData(map.sky_texture);

    int y_pos = (player.ap/(2*PI))*SCREEN_HEIGHT;
    int fill = SCREEN_HEIGHT/4;

    Rect rect1 = {.x=(-player.a/(2*PI))*SCREEN_WIDTH,                .y=y_pos-fill/2, .w=SCREEN_WIDTH, .h=SCREEN_HEIGHT+fill};
    Rect rect2 = {.x=(-player.a/(2*PI))*SCREEN_WIDTH + SCREEN_WIDTH, .y=y_pos-fill/2, .w=SCREEN_WIDTH, .h=SCREEN_HEIGHT+fill};

    renderImage(&app, sky, rect1, 0.0);
    renderImage(&app, sky, rect2, 0.0);
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
        if (map.mapH[wh] == 0) continue;

        int x = MAP_REP_SCALE*(wh % MAP_MAX_X), y = MAP_REP_SCALE*(wh / MAP_MAX_X);

        int p1[2] = {x,y}, p2[2] = {x+MAP_REP_SCALE, y};

        renderLine(&app, p1, p2, C_WHITE);
    } 
    for (int wv=0;wv<MAP_MAX_V;wv++) { // vertical
        if (map.mapV[wv] == 0) continue;

        int x = MAP_REP_SCALE*(wv % (MAP_MAX_X+1)), y = MAP_REP_SCALE*(wv / (MAP_MAX_X+1));

        int p1[2] = {x,y}, p2[2] = {x, y+MAP_REP_SCALE};

        renderLine(&app, p1, p2, C_WHITE);
    } 
}

static void renderRepPlayer() {
    //angle line
    int p1[2] = {player.pos[X]*MAP_REP_SCALE, 
                 player.pos[Y]*MAP_REP_SCALE}, 
        pa1[2] = {player.pos[X]*MAP_REP_SCALE+cos(player.a-(FOV/2))*MAP_REP_PLAYER_SIZE*3, 
                 player.pos[Y]*MAP_REP_SCALE+sin(player.a-(FOV/2))*MAP_REP_PLAYER_SIZE*3},
        pa2[2] = {player.pos[X]*MAP_REP_SCALE+cos(player.a+(FOV/2))*MAP_REP_PLAYER_SIZE*3, 
                 player.pos[Y]*MAP_REP_SCALE+sin(player.a+(FOV/2))*MAP_REP_PLAYER_SIZE*3};
    
    //velocity line
    int vp[2] = {
        player.pos[X]*MAP_REP_SCALE+player.vel[X]*MAP_REP_PLAYER_SIZE*2,
        player.pos[Y]*MAP_REP_SCALE+player.vel[Y]*MAP_REP_PLAYER_SIZE*2
    };
    
    // want velocity line
    int wvp[2] = {
        player.pos[X]*MAP_REP_SCALE+player.wvel[X]*MAP_REP_PLAYER_SIZE*2, 
        player.pos[Y]*MAP_REP_SCALE+player.wvel[Y]*MAP_REP_PLAYER_SIZE*2, 
    };

    //raycasting line test
    //for (int i=0; i<sizeof(ray_data)/sizeof(RayData); i++) {
    //    int r2[2] = {
    //        ray_data[i].end_pos[X]*MAP_REP_SCALE,
    //        ray_data[i].end_pos[Y]*MAP_REP_SCALE
    //    };
    //
    //    renderLine(&app, p1, r2, C_GREEN);
    //}
    
    renderLine(&app, p1, wvp, C_BLUE);
    renderLine(&app, p1, vp, C_YELLOW);
    renderLine(&app, p1, pa1, C_RED);
    renderLine(&app, p1, pa2, C_RED);
    renderLine(&app, pa1, pa2, C_RED);
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

                    if (input.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                        map.sprites[0].special_anim.b_running = !map.sprites[0].special_anim.b_running;
                    }

                    if (input.key.keysym.scancode == SDL_SCANCODE_E) {
                        map.sprites[0].b_spin = !map.sprites[0].b_spin;
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

static void initImages() {
    InitAssetData();
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

        graphDataAdd(&fps_graph_data, fps_avg);
    }
    // memory allocated
    unsigned long bytes;
    currentRssBytes(&bytes);

    snprintf(debug_text, sizeof(debug_text), "FPS: %0.2f\nPOS: (%0.2f,%0.2f)\nANG: (%0.2f,%0.2f)\nRAM: %.2f MB", 
            fps_avg, 
            player.pos[X], player.pos[Y], 
            player.a, player.ap,
            bytes/(1000.0*1000.0));

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

static void renderRepSprites() {
    for (int i=0;i<map.sprites_amm;i++) {
        SpriteAttr* sprite = &map.sprites[i];

        int p1[2] = {sprite->pos[X]*MAP_REP_SCALE, sprite->pos[Y]*MAP_REP_SCALE};
        int p2[2] = {sprite->pos[X]*MAP_REP_SCALE+cos(sprite->a)*MAP_REP_PLAYER_SIZE*2, 
                      sprite->pos[Y]*MAP_REP_SCALE+sin(sprite->a)*MAP_REP_PLAYER_SIZE*2};
        
        Rect sprite_rect = {.x=(sprite->pos[X]*MAP_REP_SCALE)-(MAP_REP_PLAYER_SIZE/2), .y=(sprite->pos[Y]*MAP_REP_SCALE)-(MAP_REP_PLAYER_SIZE/2),
                        .w=MAP_REP_PLAYER_SIZE, .h=MAP_REP_PLAYER_SIZE,
                        .color=C_CYAN};

        renderRect(&app, sprite_rect);
        renderLine(&app, p1, p2, C_CYAN);
    }
}

static void renderSprites() {

    for (int i=0;i<map.sprites_amm;i++) {
        SpriteAttr* sprite = &map.sprites[i];

        renderSprite(sprite, &app, &player, &map);
    }
}

static void initSprites() {
    for(int i=0;i<1;i++) {
        SpriteImageList anim_img = {
            .img_keys = {
                "starman0.png",
                "starman0.png",
                "starman11.png",
                "starman1.png",
                "starman11.png",
                "starman1.png",
                "starman11.png",
                "starman1.png",
                "starman0.png",
                "starman0.png",
            },
            .img_amm = 10
        };

        SpriteAnim anim = {
            .img_list=anim_img,
            .anim_fps=12,
            .b_loop=false,
            .b_rtrn_aft_end=true,
        };
        
        SpriteImageList rot = {
            .img_keys = {
                "starman%n.png"
            },
            .img_amm = 12
        };

        SpriteAttr sprite1 = {
            .rotation_imgs = rot,
            .pos = {1.5+i,2.5},
            .size = 0.8,
            .b_spin = false,
            .b_frontfacing = false,
            .special_anim = anim,
            .spin_spd = 12,
            .a = 0,
        };
        
        Map_addSprite(&map, sprite1);
    }
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

static void renderDebugFPSGraph() {
    Rect background = {
        .color = C_BLACK,
        .w = 64,
        .h = 32,
        .y = MAP_MAX_Y*MAP_REP_SCALE + 8
    };
    renderGraph(&fps_graph_data, "FPS AVG", background, C_GREEN, &app);
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

    map = Map_init();

    map.wall_height = 1.0;
    map.sky_texture = "sky.png";

    Map_setFaceTexIdx(&map, "f_brick.png", 1);
    Map_setFaceTexIdx(&map, "f_floor.png", 2);
    Map_setFaceTexIdx(&map, "f_grass.png", 3);
    Map_setFaceTexIdx(&map, "f_homer.png", 4);
    Map_setFaceTexIdx(&map, "f_pizza.png", 5);
    Map_setFaceTexIdx(&map, "f_bkrms.png", 6);

    for(int i=0;i<8;i++){ 
        map.mapH[i] = 1;
        map.mapH[i+128] = 1;

        map.mapV[17*i] = 1;
        map.mapV[17*i+8] = 1;

        for (int j=0;j<8;j++) {
            map.mapT[i*MAP_MAX_X+j].clng_type = 1;
            map.mapT[i*MAP_MAX_X+j].flor_type = 2;
            map.mapT[i*MAP_MAX_X+j].light_lvl = 255*pow(0.85,j+i);
        }
    }
    map.mapV[17*3+8] = 0;
    map.mapH[20] = 4;
    map.mapH[21] = 5;
    map.mapV[21] = 1;
    map.mapV[38] = 1;
    map.mapV[39] = 1;
    
    image_holder = initImageHolder();
    Map_initFaceTexData(&map);
    
    initImages();
    initThreads();
    initSprites();

    atexit(cleanup);

    for(;;) {
        TIME_LAST = TIME_NOW;
        TIME_NOW = SDL_GetTicks();
        dt = (TIME_NOW-TIME_LAST)/1000.0;

        handleInput();

        // velocity input conversion //
        Player_inputVelocity(&player);

        map.sprites[0].pos[Y] = 3.8*sin(TIME_NOW/2000.0) + 4;

        // player velocity //
        Player_handleVelocity(&player, dt);

        // player collision //
        Player_handleCollision(&player, &map, dt, &app);

        // player position //
        Player_handlePosition(&player, dt);

        // player anim //
        Player_walkAnimation(&player, dt);

        // render frame //
        App_clearBuffer(&app);

        renderBackground();

        raycast();

        renderSprites();

        if (debug) {
            renderRepFloor();

            renderRepWalls();

            renderRepSprites();

            renderRepPlayer();

            renderDebugText();

            renderDebugFPSGraph();
        }

        presentFrame(&app);

        // delays every frame if over hz; helps free CPU time
        if (LIMIT_FRAMERATE) limitFPS();
    }
}