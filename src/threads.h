#ifndef THREADS_H
#include "render.h"
#include <pthread.h>

typedef struct _RenderFrameThreadCtx {
    App* app;
    RayData* data;
    Player* player;
    Map* map;
    int posX, clng_start, flor_start;
    double focal_length, horizon;
    float deg_amm;

    int is_ready;
    int is_done;
    int should_exit;

    pthread_mutex_t mutex;
    pthread_cond_t cond_start;
    pthread_cond_t cond_end;

} RenderFrameThreadCtx;

#endif