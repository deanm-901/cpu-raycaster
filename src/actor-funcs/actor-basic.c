#include "actor-basic.h"
#include "../player.h"
#include "../defs.h"

extern Player player;

void func_InitBasic(ActorAttr *attr) {
    //attr->map_sprite->special_anim.b_running = 1;
}

void func_FrameBasic(ActorAttr *attr) {
    attr->map_sprite->pos[X] = attr->pos[X];
    attr->map_sprite->pos[Y] = attr->pos[Y];

    float dx = player.pos[X]-attr->pos[X];
    float dy = player.pos[Y]-attr->pos[Y];
    float i_dh = Q_rsqrt(dx*dx+dy*dy);

    attr->pos[X] += dx*i_dh * attr->speed;
    attr->pos[Y] += dy*i_dh * attr->speed;
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
        .speed = 0.005,
    };

    Actor a = {
        .Func_Frame = &func_FrameBasic,
        .Func_Init = &func_InitBasic,
        .attr = attr,
    };

    Map_addActor(map, a);
}