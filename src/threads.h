#ifndef THREADS_H
#include "render.h"
#include <pthread.h>

typedef struct _AsyncRenderCtx {
    App* app;
    RayData* data;
    Player* player;
    Map* map;
    int posX, clng_start, flor_start;
    double focal_length, horizon;
    float deg_amm;
} AsyncRenderCtx;

typedef struct _RenderFrameThreadCtx {
    int is_ready;
    int is_done;
    int should_exit;

    int startX, endX;

    pthread_mutex_t mutex;
    pthread_cond_t cond_start;
    pthread_cond_t cond_end;
} RenderFrameThreadCtx;

typedef struct _RenderThread {
    RenderFrameThreadCtx ctx;
    pthread_t thread;
} RenderThread;

void DestroyRenderThread(RenderThread* rthread);
void InitRenderThread(RenderThread* rthread, int startX, int endX, void *(*func)(void *));

void RenderThreadSubmitJob(RenderThread* rthread);
void RenderThreadAwaitCompletion(RenderThread* rthread);

#endif