#include "actor-basic.h"
#include "../player.h"
#include "../defs.h"

extern Player player;
extern double dt;

static void func_InitBasic(ActorAttr *attr) {
    //attr->map_sprite->special_anim.b_running = 1;
    attr->data[0] = malloc(sizeof(float));

    *((float*)attr->data[0]) = 1.0f; // speed
}

static void func_FrameBasic(ActorAttr *attr) {
    attr->map_sprite->pos[X] = attr->pos[X];
    attr->map_sprite->pos[Y] = attr->pos[Y];

    float dx = player.pos[X]-attr->pos[X];
    float dy = player.pos[Y]-attr->pos[Y];
    float i_dh = Q_rsqrt(dx*dx+dy*dy);

    float speed = *((float*)attr->data[0]);

    attr->pos[X] += dx*i_dh * dt * speed;
    attr->pos[Y] += dy*i_dh * dt * speed;
}

static void func_DestroyBasic(ActorAttr *attr) {
    free(attr->data[0]);
}

void InitBasicActor(Map* map) {
    SpriteImageList rot = {
        .img_keys = {
            "starman%n.png"
        },
        .img_amm = 12
    };

    SpriteAttr s = {
        .rotation_imgs = rot,
        .size = 0.8,
        .a = 0,
    };
    size_t s_idx = Map_addSprite(map, s);

    ActorAttr attr = {
        .map_sprite = &map->sprites[s_idx],
        .pos = {1.5,2.5},
    };

    Actor a = {
        .Func_Frame = &func_FrameBasic,
        .Func_Init = &func_InitBasic,
        .Func_Destroy = &func_DestroyBasic,
        .attr = attr,
    };

    Map_addActor(map, a);
}