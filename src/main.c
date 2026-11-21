// TODO 0: Map Editor

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
#include "actor-funcs/actor-basic.h"
#include "console.h"

// there is one image holder per instance of the executable file
ImageHolder image_holder;

App app;
Player player = {.vel={0,0}, .a=PI/2, .vela=0.0, .speed=2.0, .turn_speed=2, .height=0.55, .size=0.1};

Map map;
GameSettings game_settings;
Console console;

// in ms
Uint64 TIME_NOW = 0;
Uint64 TIME_LAST = 0;
// in seconds / frame
double dt = 1;
char debug_text[128];

#define FPS_BUCKET_SIZE 32
float fps_bucket[FPS_BUCKET_SIZE];
size_t fps_bucket_count = 0;
float fps_avg = 0.0;
GraphData fps_graph_data = {0};

RayData ray_data[SCREEN_WIDTH];

char render_map = false;

#define REND_THREAD_AMM 4
RenderThread render_threads[REND_THREAD_AMM];

static void renderBackground() {
    double focal_length = SCREEN_WIDTH / (2.0 * tan(game_settings.fov / 2.0));

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
        pa1[2] = {player.pos[X]*MAP_REP_SCALE+cos(player.a-(game_settings.fov/2))*MAP_REP_PLAYER_SIZE*3, 
                 player.pos[Y]*MAP_REP_SCALE+sin(player.a-(game_settings.fov/2))*MAP_REP_PLAYER_SIZE*3},
        pa2[2] = {player.pos[X]*MAP_REP_SCALE+cos(player.a+(game_settings.fov/2))*MAP_REP_PLAYER_SIZE*3, 
                 player.pos[Y]*MAP_REP_SCALE+sin(player.a+(game_settings.fov/2))*MAP_REP_PLAYER_SIZE*3};
    
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

static void renderRepMapData() { // TODO
    renderRect(&app, (Rect){.color = C_WHITE, .w = 34, .h = 3, .x = game_settings.face_place_idx*34 - 1, .y = 33});

    for (int i=0;i<MAP_MAX_FACETEX;i++) {
        ImageData* data = map.face_tex_data[i];

        if (data == NULL) continue;

        renderImage(&app, data, (Rect){.x = i*34, .y = 2, .w = 32, .h = 32}, 9999999);
    }
}

static void handleInput() {
        player.inpa = 0;
        player.inpap = 0;
    
        SDL_Event input;
        while(SDL_PollEvent(&input)) {
            // console input //
            Console_ReadTextInput(&console, input);

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

                    if (input.key.keysym.scancode == SDL_SCANCODE_TAB) {
                        render_map = true;
                    }

                    if (input.key.keysym.scancode == SDL_SCANCODE_E && game_settings.b_map_editor && !console.b_visible) {
                        if (player.a < PI/4 || player.a > 7*PI/4)
                            map.mapV[coordToIndex((int)player.pos[X]+1, (int)player.pos[Y], MAP_MAX_X+1)] = game_settings.face_place_idx;
                        else if (player.a > PI/4 && player.a < 3*PI/4)
                            map.mapH[coordToIndex((int)player.pos[X], (int)player.pos[Y]+1, MAP_MAX_X)] = game_settings.face_place_idx;
                        else if (player.a > 3*PI/4 && player.a < 5*PI/4)
                            map.mapV[coordToIndex((int)player.pos[X], (int)player.pos[Y], MAP_MAX_X+1)] = game_settings.face_place_idx;
                        else if (player.a > 5*PI/4 && player.a < 7*PI/4)
                            map.mapH[coordToIndex((int)player.pos[X], (int)player.pos[Y], MAP_MAX_X)] = game_settings.face_place_idx;
                    }
                    if (input.key.keysym.scancode == SDL_SCANCODE_R && game_settings.b_map_editor && !console.b_visible) {
                        map.mapT[coordToIndex((int)player.pos[X], (int)player.pos[Y], MAP_MAX_X)].clng_type = game_settings.face_place_idx;
                    }
                    if (input.key.keysym.scancode == SDL_SCANCODE_F && game_settings.b_map_editor && !console.b_visible) {
                        map.mapT[coordToIndex((int)player.pos[X], (int)player.pos[Y], MAP_MAX_X)].flor_type = game_settings.face_place_idx;
                    }
                    if (!console.b_visible) Player_handleKeyboardInput(&player, &input.key, 1);

                    break;
                case SDL_KEYUP:
                    if (input.key.keysym.scancode == SDL_SCANCODE_TAB) {
                        render_map = false;
                    }

                    if (!console.b_visible) Player_handleKeyboardInput(&player, &input.key, -1);
                    break;
                case SDL_MOUSEMOTION:
                    Player_handleMouseMotion(&player, &input.motion, &app);
                    break;
                case SDL_MOUSEWHEEL:
                    if (game_settings.b_map_editor) {
                        game_settings.face_place_idx -= input.wheel.y;

                        if (game_settings.face_place_idx < 0) game_settings.face_place_idx = 0;
                    }
                    break;
                default:
                    break;
            }
        }
}

static void raycast() {
    const int amm = SCREEN_WIDTH;

    for (int i=0; i<amm; i++) {
        double deg_amm = i*game_settings.fov/amm;

        ray_data[i] = Player_castRay(&player, &map, player.a+deg_amm-(game_settings.fov/2.0));

        renderStrip(&app, &ray_data[i], &player, &map, i, deg_amm-(game_settings.fov/2.0));
    }

    //submit jobs
    for (int i=0;i<REND_THREAD_AMM;i++) RenderThreadSubmitJob(&render_threads[i]);

    // wait
    for (int i=0;i<REND_THREAD_AMM;i++) RenderThreadAwaitCompletion(&render_threads[i]);
}

static void initPlayer() {
    player.pos[X] = map.player_init_pos[X];
    player.pos[Y] = map.player_init_pos[Y];
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
    uint64_t bytes;
    currentRssBytes(&bytes);

    snprintf(debug_text, sizeof(debug_text), "FPS: %0.2f\nPOS: (%0.2f,%0.2f)\nANG: (%0.2f,%0.2f)\nRAM: %.2f MB", 
            fps_avg, 
            player.pos[X], player.pos[Y], 
            player.a, player.ap,
            bytes/(1000.0*1000.0));

    renderText(&app, getImageData("font1.png"), 8, 8, debug_text, 0, 0, 1, C_WHITE);
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
        .w = 96,
        .h = 32,
        .y = SCREEN_HEIGHT-33,
        .x = 1
    };
    renderGraph(&fps_graph_data, "FPS AVG", background, C_GREEN, &app);
}

static void cleanup() {
    freeImageHolder();
    destroyThreads();
    Map_free(&map);
    App_free(&app);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    App_init(&app);

    GameSettings_SetDefault(game_settings);
    Console_SetDefault(console);
    ConsoleCommands_Init();
    
    game_settings.b_map_editor = (argc == 2 && !strcmp(argv[1], "map-editor"));

    image_holder = initImageHolder();
    
    initImages();
    initThreads();
    initPlayer();

    map = Map_init();
    Map_readFromFile(&map, "maps/test.map");

    atexit(cleanup);

    for(;;) {
        TIME_LAST = TIME_NOW;
        TIME_NOW = SDL_GetTicks();
        dt = (TIME_NOW-TIME_LAST)/1000.0;

        // console update //
        Console_Update(&console);

        // input //
        handleInput();

        // velocity input conversion //
        Player_inputVelocity(&player);

        // player velocity //
        Player_handleVelocity(&player, dt);

        // player collision //
        Player_handleCollision(&player, &map, dt, &app);

        // player position //
        Player_handlePosition(&player, dt);

        // player anim //
        Player_walkAnimation(&player, dt);

        // map actor funcs //
        Map_callActorFuncs(&map);

        // render frame //
        App_clearBuffer(&app);

        renderBackground();

        raycast();

        renderSprites();

        if (render_map) {
            renderRepFloor();
            renderRepWalls();
            renderRepSprites();
            renderRepPlayer();
        }

        if (game_settings.b_debug && !render_map) {
            renderDebugText();

            renderDebugFPSGraph();
        }

        if (game_settings.b_map_editor && !render_map) {
            renderRepMapData();
        }

        Console_Render(&app, &console);

        presentFrame(&app);

        // delays every frame if over hz; helps free CPU time
        if (game_settings.b_limit_framerate) limitFPS();
    }
}