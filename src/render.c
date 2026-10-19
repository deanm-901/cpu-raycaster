#include "render.h"
#include "defs.h"
#include "colors.h"
#include <math.h>

void prepareFrame(App* app) {
    // SDL_SetRenderDrawColor(app->renderer, 50,50,50,255);
    // SDL_RenderClear(app->renderer);
}

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

            App_setPixel(app, rect.x+x,rect.y+y, rect.color);
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
        App_setPixel(app, round(pos[X]), round(pos[Y]), color);
        pos[X] += diff[X];
        pos[Y] += diff[Y];
        i++;
    }
}

static void renderStripFloor(App* app, Player* player, double horizon, int start, int posX, float deg_amm, double focal_length) {
    if (start>=SCREEN_HEIGHT) return;

    if (start<0) start = 0;

    ImageData* img = getImageData("homer");
    const float ray_ang = player->a+deg_amm;
    const float ray_cos = cos(ray_ang);
    const float ray_sin = sin(ray_ang);
    const float cos_deg_amm = cos(deg_amm);
    const int tex_height = img->image.height, tex_width = img->image.width;

    const double dist_scale = focal_length * player->height / cos_deg_amm;

    const double px = player->pos[X], py = player->pos[Y];

    for (int y=start; y<SCREEN_HEIGHT; y++) {
        const float dz = dist_scale/(y-horizon),
              dx = dz*ray_cos + px,
              dy = dz*ray_sin + py;
        
        const float u = dx-floor(dx), 
              v = dy-floor(dy);
        
        const uint32_t pixel = img->pixels[coordToIndex(tex_width*u, tex_height*v, tex_width)];

        App_setPixel(app, posX, y, pixel);
    }
}

void renderStrip(App* app, RayData* data, Player* player, int posX, float deg_amm) {

    double focal_length = SCREEN_WIDTH / (2.0 * tan(FOV / 2.0));
    double horizon = SCREEN_HEIGHT/2 + focal_length*tan(player->ap);

    float dist = data->dist * cos(deg_amm);

    int startY = horizon - focal_length * (1.0-player->height) / dist;
    int endY   = horizon + focal_length * player->height / dist;
    int c_startY = (startY<0)?0:startY;
    int c_endY   = (endY>SCREEN_HEIGHT)?SCREEN_HEIGHT:endY;
    
    int lineH  = endY-startY;
    int c_lineH = c_endY-c_startY;

    renderStripFloor(app, player, horizon, c_endY, posX, deg_amm, focal_length);
    if (data->type == NONE) return;

    ImageData* img = getImageData("pizza");

    //if (dist < 1) dist = 1;
    for (int i = 0; i < c_lineH; i++) {
        float u = data->u;
        if (u<0.0f)  u = 0.0f;
        if (u>=1.0f) u = 0.99999f;

        float v = (float)(i+c_startY-startY)/lineH;
        if (v<0.0f)  v = 0.0f;
        if (v>=1.0f) v = 0.99999f;

        uint32_t pixel = img->pixels[coordToIndex(img->image.width*u, v * img->image.height, img->image.width)];
        uint8_t s = 0xFF*data->u;

        App_setPixel(app, posX, i+c_startY, pixel);
    }
}

void renderImage(App* app, ImageData* data, Rect rect) {
    float scale[2] = {(float)rect.w/data->image.width, (float)rect.h/data->image.height};

    for (int y = 0; y < data->image.height*scale[Y]; y++) {
        for (int x = 0; x < data->image.width*scale[X]; x++) {

            int index = ((int)(y/scale[Y])*data->image.width)+(x/scale[X]);
            uint32_t pixel = data->pixels[index];

            App_setPixel(app, x+rect.x, y+rect.y, pixel);
        }
    }
}