#include "player.h"
#include "defs.h"
#include <math.h>

void Player_handleKeyboardInput(Player* player, SDL_KeyboardEvent* input, int down) {
    if (input->repeat != 0) return;

    switch (input->keysym.scancode)
    {
    case SDL_SCANCODE_W:
        player->inp[X] += down;
        break;
    case SDL_SCANCODE_S:
        player->inp[X] += -down;
        break;
    case SDL_SCANCODE_Q:
        player->inp[Y] += -down;
        break;
    case SDL_SCANCODE_E:
        player->inp[Y] += down;
        break;
    case SDL_SCANCODE_A:
        player->inpa += -down;
        break;
    case SDL_SCANCODE_D:
        player->inpa += down;
        break;
    default:
        break;
    }
}
void Player_inputVelocity(Player* player, double dt) {

    player->vela = player->inpa*player->turn_speed;
    player->a += player->vela*dt;

    if (player->a < 0)    player->a += 2*PI;
    if (player->a > 2*PI) player->a -= 2*PI;

    player->vel[X] = player->inp[X]*cos(player->a) + player->inp[Y]*cos(player->a+PI/2);
    player->vel[Y] = player->inp[X]*sin(player->a) + player->inp[Y]*sin(player->a+PI/2);

    float normal_dem = sqrt(player->vel[X]*player->vel[X] + player->vel[Y]*player->vel[Y]);
    if (normal_dem == 0) return;

    player->vel[X] = player->vel[X]/normal_dem;
    player->vel[Y] = player->vel[Y]/normal_dem;

    player->pos[X] += player->vel[X]*dt*player->speed;
    player->pos[Y] += player->vel[Y]*dt*player->speed;
}

RayData Player_castRay(Player* player, Map* map, double angle) {
    #define HORZ 0
    #define VERT 1
    
    double pos[2] = {player->pos[X], player->pos[Y]};
    double ray_dir[2] = {cos(angle), sin(angle)};
    int tile_pos[2] = {(int)pos[X], (int)pos[Y]};

    double p_offset[2] = {pos[X]-tile_pos[X], pos[Y]-tile_pos[Y]};

    double tile_step[2];
    tile_step[X] = (ray_dir[X] > 0) ? 1:-1;
    tile_step[Y] = (ray_dir[Y] > 0) ? 1:-1;

    int tile_correct[2];
    tile_correct[X] = (ray_dir[X] > 0) ? 0:1;
    tile_correct[Y] = (ray_dir[Y] > 0) ? 0:1;

    double len_step[2];
    len_step[HORZ] = fabs(1 / ray_dir[Y]);
    len_step[VERT] = fabs(1 / ray_dir[X]);

    double lengths[2];
    lengths[HORZ] = (ray_dir[Y] > 0) ? (1-p_offset[Y])/ray_dir[Y] : -p_offset[Y]/ray_dir[Y];
    lengths[VERT] = (ray_dir[X] > 0) ? (1-p_offset[X])/ray_dir[X] : -p_offset[X]/ray_dir[X];

    MapWallType type = NONE;
    double distance = 0.0;
    while (type == NONE && distance < RAYCAST_MAX_DIST) {
        if (lengths[VERT] < lengths[HORZ]) {
            tile_pos[X] += tile_step[X];
            distance = lengths[VERT];
            lengths[VERT] += len_step[VERT];

            type = map->mapV[tile_pos[Y]*(MAP_MAX_X+1)+(tile_pos[X]+tile_correct[X])];
        } else {
            tile_pos[Y] += tile_step[Y];
            distance = lengths[HORZ];
            lengths[HORZ] += len_step[HORZ];

            type = map->mapH[(tile_pos[Y]+tile_correct[Y])*MAP_MAX_X+tile_pos[X]];
        }

        if (tile_pos[X] < 0 || tile_pos[X] > MAP_MAX_X || tile_pos[Y] < 0 || tile_pos[Y] > MAP_MAX_Y) break;
    }

    #undef HORZ
    #undef VERT

    RayData data = {.dist = distance, .type = type, .end_pos = {pos[X]+ray_dir[X]*distance, pos[Y]+ray_dir[Y]*distance}};

    return data;
}