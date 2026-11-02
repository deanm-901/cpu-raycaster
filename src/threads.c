#include "threads.h"

void DestroyRenderThread(RenderThread* rthread) {
    pthread_mutex_lock(&rthread->ctx.mutex);
    rthread->ctx.should_exit = 1;
    pthread_cond_signal(&rthread->ctx.cond_start);
    pthread_mutex_unlock(&rthread->ctx.mutex);

    pthread_join(rthread->thread, NULL);

    pthread_mutex_destroy(&rthread->ctx.mutex);
    pthread_cond_destroy(&rthread->ctx.cond_end);
    pthread_cond_destroy(&rthread->ctx.cond_start);
}

void InitRenderThread(RenderThread* rthread, int startX, int endX, void *(*func)(void *)) {
    pthread_mutex_init(&rthread->ctx.mutex, NULL);
    pthread_cond_init(&rthread->ctx.cond_end, NULL);
    pthread_cond_init(&rthread->ctx.cond_start, NULL);

    pthread_mutex_lock(&rthread->ctx.mutex);

    rthread->ctx.startX = startX;
    rthread->ctx.endX = endX;
    rthread->ctx.should_exit = 0;
    rthread->ctx.is_done = true;
    rthread->ctx.is_ready = false;
    
    pthread_mutex_unlock(&rthread->ctx.mutex);

    pthread_create(&rthread->thread, NULL, func, &rthread->ctx);
}

void RenderThreadSubmitJob(RenderThread* rthread) {
    pthread_mutex_lock(&rthread->ctx.mutex);

    rthread->ctx.is_done = false;
    rthread->ctx.is_ready = true;
    pthread_cond_signal(&rthread->ctx.cond_start);

    pthread_mutex_unlock(&rthread->ctx.mutex);
}

void RenderThreadAwaitCompletion(RenderThread* rthread) {
    pthread_mutex_lock(&rthread->ctx.mutex);
    while(!rthread->ctx.is_done) {
        pthread_cond_wait(&rthread->ctx.cond_end, &rthread->ctx.mutex);
    }
    pthread_mutex_unlock(&rthread->ctx.mutex);
}