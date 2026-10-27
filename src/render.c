#include "render.h"
#include "defs.h"
#include "colors.h"
#include <math.h>
#include <string.h>
#include <pthread.h>

extern FaceAttr face_types[MAX_FACETYPE_COUNT];
extern RayData ray_data[SCREEN_WIDTH];

extern Uint64 TIME_NOW;

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

            App_setPixel(app, rect.x+x,rect.y+y, rect.color, 9999);
        }
    }
}

void renderLine(App* app, int p1[2], int p2[2], uint32_t color) {
    float diff[2] = {p2[X]-p1[X], p2[Y]-p1[Y]};
    float step;

    if (abs(diff[X]) >= abs(diff[Y]))
        step = abs(diff[X]);
    else
        step = abs(diff[Y]);
    
    diff[X] = diff[X]/step;
    diff[Y] = diff[Y]/step;

    float pos[2] = {p1[X], p1[Y]};
    int i = 0;

    while (i <= step) {
        App_setPixel(app, round(pos[X]), round(pos[Y]), color, 9999);
        pos[X] += diff[X];
        pos[Y] += diff[Y];
        i++;
    }
}

static float getFogSubtraction(float dist) {
    if (dist > FOG_MAX_DIST) return -1.0f;

    return 0.5*cos(dist*PI / FOG_MAX_DIST) - 0.5;
}

static void renderStripCeiling(App* app, Player* player, Map* map, double horizon, int start, int posX, float deg_amm, double focal_length) {
    if (start<=0) return;

    if (start>=SCREEN_HEIGHT) start = SCREEN_HEIGHT-1;

    if (player->height > map->wall_height) return;

    const ImageData* img = getImageData("atlas.png");
    const float ray_ang = player->a+deg_amm;
    const float ray_cos = cos(ray_ang);
    const float ray_sin = sin(ray_ang);
    const float cos_deg_amm = cos(deg_amm);

    const double dist_scale = focal_length * (map->wall_height-player->height-player->height_disp) / cos_deg_amm;

    const double px = player->pos[X], py = player->pos[Y];

    for (int y=start; y>=0; y--) {
        const float dz = dist_scale/(horizon-y),
                    dx = dz*ray_cos + px,
                    dy = dz*ray_sin + py;
        
        const int ix = (int)dx, iy = (int)dy;
        
        if (ix >= MAP_MAX_X || ix < 0 || iy >= MAP_MAX_Y || iy < 0) continue;

        const MapFaceType type = map->mapT[coordToIndex(ix, iy, MAP_MAX_X)].clng_type;
        const FaceAttr* attr = &face_types[type];

        if (type == T_NONE) continue;
        
        float texX = TEXTURE_ATLAS_W*(dx-floor(dx)), 
              texY = TEXTURE_ATLAS_H*(dy-floor(dy));
        
        const int atlasX = attr->atlas_pos[X] * TEXTURE_ATLAS_W;
        const int atlasY = attr->atlas_pos[Y] * TEXTURE_ATLAS_H;

        if (texX >= TEXTURE_ATLAS_W) texX = TEXTURE_ATLAS_W - 1;
        if (texY >= TEXTURE_ATLAS_H) texY = TEXTURE_ATLAS_H - 1;
        
        float mult = (float)map->mapT[coordToIndex(ix, iy, MAP_MAX_X)].light_lvl/255.0;
        mult *= getFogSubtraction(dz)*FOG_SUB_AMM+1;

        uint32_t pixel = img->pixels[coordToIndex(texX+atlasX, texY+atlasY, img->image.width)];
        pixel = ColorMult(pixel, mult, mult, mult, 1.0);

        App_setPixel(app, posX, y, pixel, 1.0/dz);
    }
}

static void renderStripFloor(App* app, Player* player, Map* map, double horizon, int start, int posX, float deg_amm, double focal_length) {
    if (start>=SCREEN_HEIGHT) return;

    if (start<0) start = 0;

    const ImageData* img = getImageData("atlas.png");
    const float ray_ang = player->a+deg_amm;
    const float ray_cos = cos(ray_ang);
    const float ray_sin = sin(ray_ang);
    const float cos_deg_amm = cos(deg_amm);

    const double dist_scale = focal_length * (player->height+player->height_disp) / cos_deg_amm;

    const double px = player->pos[X], py = player->pos[Y];

    for (int y=start; y<SCREEN_HEIGHT; y++) {
        const float dz = dist_scale/(y-horizon+1), // +1 for correction
              dx = dz*ray_cos + px,
              dy = dz*ray_sin + py;
        
        const int ix = (int)dx, iy = (int)dy;

        if (ix >= MAP_MAX_X || ix < 0 || iy >= MAP_MAX_Y || iy < 0) continue;

        const MapFaceType type = map->mapT[coordToIndex(ix, iy, MAP_MAX_X)].flor_type;
        const FaceAttr* attr = &face_types[type];

        if (type == T_NONE) continue;
        
        float texX = TEXTURE_ATLAS_W*(dx-floor(dx)), 
              texY = TEXTURE_ATLAS_H*(dy-floor(dy));
        
        const int atlasX = attr->atlas_pos[X] * TEXTURE_ATLAS_W;
        const int atlasY = attr->atlas_pos[Y] * TEXTURE_ATLAS_H;

        if (texX >= TEXTURE_ATLAS_W) texX = TEXTURE_ATLAS_W - 1;
        if (texY >= TEXTURE_ATLAS_H) texY = TEXTURE_ATLAS_H - 1;
        
        float mult = (float)map->mapT[coordToIndex(ix, iy, MAP_MAX_X)].light_lvl/255.0;
        mult *= getFogSubtraction(dz)*FOG_SUB_AMM+1;
        if (mult<0) mult=0;

        uint32_t pixel = img->pixels[coordToIndex(texX+atlasX, texY+atlasY, img->image.width)];
        pixel = ColorMult(pixel, mult, mult, mult, 1.0);

        App_setPixel(app, posX, y, pixel, 1.0/dz);
    }
}

extern pthread_t rend_thread_chunk1, rend_thread_chunk2;
extern RenderFrameThreadCtx render_thread_context1, render_thread_context2;

AsyncRenderCtx render_strips[SCREEN_WIDTH];

void renderStrip(App* app, RayData* data, Player* player, Map* map, int posX, float deg_amm) {

    const double focal_length = SCREEN_WIDTH / (2.0 * tan(FOV / 2.0));
    const double horizon = SCREEN_HEIGHT/2 + focal_length*tan(player->ap);

    const float dist = data->dist * cos(deg_amm);

    const int startY = horizon - focal_length * (map->wall_height-player->height-player->height_disp) / dist;
    const int endY   = horizon + focal_length * (player->height+player->height_disp) / dist;
    const int c_startY = (startY<0)?0:startY;
    const int c_endY   = (endY>SCREEN_HEIGHT)?SCREEN_HEIGHT:endY;
    
    const int lineH  = endY-startY;
    const int c_lineH = c_endY-c_startY;

    AsyncRenderCtx ctx = {
        .app = app,
        .data = data,
        .player = player,
        .map = map,
        .horizon = horizon,
        .clng_start = c_startY,
        .flor_start = c_endY,
        .deg_amm = deg_amm,
        .focal_length = focal_length,
        .posX = posX,
    };
    render_strips[posX] = ctx;

    if (data->type == T_NONE) return;

    const FaceAttr* attr = &face_types[data->type];

    float mult = (float)map->mapT[coordToIndex(data->hit_tile[X], data->hit_tile[Y], MAP_MAX_X)].light_lvl/255.0;
    mult *= getFogSubtraction(dist)*FOG_SUB_AMM+1;

    ImageData* img = getImageData("atlas.png");

    int atlasX = attr->atlas_pos[X] * TEXTURE_ATLAS_W;
    int atlasY = attr->atlas_pos[Y] * TEXTURE_ATLAS_H;

    //if (dist < 1) dist = 1;
    for (int i = 0; i < c_lineH; i++) {
        float u = data->u;
        if (u<0.0f)  u = 0.0f;
        if (u>=1.0f) u = 0.99999f;

        float v = (float)(i+c_startY-startY)/lineH * map->wall_height;
        v -= (int)v;
        if (v<0.0f)  v = 0.0f;
        if (v>=1.0f) v = 0.99999f;

        uint32_t pixel = img->pixels[coordToIndex(u*TEXTURE_ATLAS_W + atlasX, v*TEXTURE_ATLAS_H + atlasY, img->image.width)];
        pixel = ColorMult(pixel, mult, mult, mult, 1.0);

        App_setPixel(app, posX, i+c_startY, pixel, 1.0/dist);
    }
}

void renderImage(App* app, ImageData* data, Rect rect, float zbuffer) {
    float scale[2] = {(float)rect.w/data->image.width, (float)rect.h/data->image.height};

    for (int y = 0; y < data->image.height*scale[Y]; y++) {
        for (int x = 0; x < data->image.width*scale[X]; x++) {

            int index = ((int)(y/scale[Y])*data->image.width)+(x/scale[X]);
            uint32_t pixel = data->pixels[index];

            App_setPixel(app, x+rect.x, y+rect.y, pixel, zbuffer);
        }
    }
}

void renderText(App* app, ImageData* font, uint8_t tile_w, uint8_t tile_h, const char* text, uint32_t pos_x, uint32_t pos_y, int scale, uint32_t color) {
    const size_t text_len = strlen(text);
    const size_t font_wdth = font->image.width/tile_w;

    int char_pos_x = 0;
    int char_pos_y = 0;

    for (int i=0;i<text_len;i++) { // each letter
        const uint8_t idx = (uint8_t)text[i] - 32;

        const uint16_t x = (idx%font_wdth) * tile_w;
        const uint16_t y = (idx/font_wdth) * tile_h;

        if (text[i] == '\n') {
            char_pos_y++;
            char_pos_x=0;
            continue;
        }

        for (int px=0;px<tile_w;px++) {
            for (int py=0;py<tile_h;py++) { // each internal pixel
                uint32_t pixel = font->pixels[coordToIndex(x+px,y+py,font->image.width)];

                for (int s=0;s<scale*scale;s++) { // each displayed pixel
                    const uint32_t s_x = s%scale;
                    const uint32_t s_y = s/scale;

                    const uint32_t pix_pos_x = pos_x+s_x + px*scale + char_pos_x*tile_w*scale;
                    const uint32_t pix_pos_y = pos_y+s_y + py*scale + char_pos_y*tile_h*scale;

                    pixel = ColorMult(pixel, color);

                    App_setPixel(app, pix_pos_x, pix_pos_y, pixel, 9999);
                }
            }
        }
        char_pos_x++;
    }
}

void renderSprite(SpriteAttr* sprite, App* app, Player* player) {
    const float dx = sprite->pos[X]-player->pos[X], dy = sprite->pos[Y]-player->pos[Y];
    const float pa = atan2f(dy, dx);
    
    const float pdist = hypotf(dx,dy);

    const float sizea = atan2f(sprite->size/2, pdist);
    const float rel = remainderf(pa-player->a, 2*PI);
    float amin  = rel-sizea, amax = rel+sizea;

    const float aperpix = FOV/SCREEN_WIDTH;

    const float pixsize = sizea*2/aperpix;

    const int pixstrt = (amin+FOV/2)/aperpix;
    const int pix_end = (amax+FOV/2)/aperpix;

    if (pix_end < 0 || pixstrt > SCREEN_WIDTH) return;

    const int c_pixstrt = (pixstrt<0)? 0:pixstrt;
    const int c_pix_end = (pix_end>SCREEN_WIDTH)? SCREEN_WIDTH:pix_end;

    const int pixstrt_diff = c_pixstrt-pixstrt;
    const int pix_end_diff = c_pix_end-pix_end;

    const double focal_length = SCREEN_WIDTH / (2.0 * tan(FOV / 2.0));
    const double horizon = SCREEN_HEIGHT/2 + focal_length*tan(player->ap);

    ImageData* img;

    float img_ang = pa-sprite->a+PI+(PI/sprite->img_amm);
    if (img_ang > 2*PI) img_ang -= 2*PI;
    if (img_ang < 0)    img_ang += 2*PI;
    const int frame = ((img_ang)/(2*PI))*sprite->img_amm;

    if (sprite->b_spin) {
        img = getImageData(sprite->img_keys[(TIME_NOW*sprite->spin_spd/1000) % sprite->img_amm]);
    } else {
        img = getImageData(sprite->img_keys[frame]);
    }

    const float depth = pdist * cos(pa-player->a); 

    for (int x=c_pixstrt;x<c_pix_end;x++) {
        if (ray_data[x].dist < depth) continue;

        const int tx=x+pixstrt_diff;

        float u = (float)(tx-c_pixstrt)/pixsize;

        const int startY = horizon - focal_length * (sprite->size-player->height-player->height_disp) / depth;
        const int endY   = horizon + focal_length * (player->height+player->height_disp) / depth;
        const int c_startY = (startY<0)?0:startY;
        const int c_endY   = (endY>SCREEN_HEIGHT)?SCREEN_HEIGHT:endY;

        if (endY < 0 || startY > SCREEN_HEIGHT) continue;
        
        const int lineH  = endY-startY;
        const int c_lineH = c_endY-c_startY;

        for (int i=0;i<c_lineH;i++) {
            float v = (float)(i+c_startY-startY)/lineH;

            uint32_t pixel = img->pixels[coordToIndex(u*img->image.width, v*img->image.height, img->image.width)];

            App_setPixel(app, x, i+c_startY, pixel, 1.0/depth);
        }
    }
}

void* asyncRenderStripChunk(void* ctx) {
    RenderFrameThreadCtx* fctx = (RenderFrameThreadCtx*)ctx;

    for(;;) {
        pthread_mutex_lock(&fctx->mutex);

        while(!fctx->is_ready && !fctx->should_exit) {
            pthread_cond_wait(&fctx->cond_start, &fctx->mutex);
        }

        if (fctx->should_exit) break;

        // clear start flag, unlock mutex to work in parallel
        fctx->is_ready = false;
        pthread_mutex_unlock(&fctx->mutex);

        for (int i=fctx->startX;i<fctx->endX;i++) {
            renderStripFloor(
                render_strips[i].app, 
                render_strips[i].player, 
                render_strips[i].map, 
                render_strips[i].horizon, 
                render_strips[i].flor_start, 
                render_strips[i].posX, 
                render_strips[i].deg_amm, 
                render_strips[i].focal_length
            );
            renderStripCeiling(
                render_strips[i].app, 
                render_strips[i].player, 
                render_strips[i].map, 
                render_strips[i].horizon, 
                render_strips[i].clng_start, 
                render_strips[i].posX, 
                render_strips[i].deg_amm, 
                render_strips[i].focal_length
            );
        }

        // signal we are done
        pthread_mutex_lock(&fctx->mutex);
        fctx->is_done = true;
        pthread_cond_signal(&fctx->cond_end);
        pthread_mutex_unlock(&fctx->mutex);
    }

    return NULL;
}