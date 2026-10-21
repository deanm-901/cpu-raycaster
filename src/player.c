#include "player.h"
#include "defs.h"
#include "app.h"
#include "render.h"
#include "colors.h"
#include <math.h>

extern double dt;

void Player_handleMouseMotion(Player* player, SDL_MouseMotionEvent* input) {
    //if ((float)input->xrel * MOUSE_SENS == 0) return;

    player->inpa  = (float)input->xrel * MOUSE_SENS;
    if (ENABLE_PITCH) player->inpap = -(float)input->yrel * MOUSE_SENS * 0.75;
}
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
    case SDL_SCANCODE_A:
        player->inp[Y] += -down;
        break;
    case SDL_SCANCODE_D:
        player->inp[Y] += down;
        break;
    default:
        break;
    }
}
void Player_inputVelocity(Player* player) {

    player->vela  = player->inpa *player->turn_speed;
    player->velap = player->inpap*player->turn_speed;

    if (player->a < 0)    player->a += 2*PI;
    if (player->a > 2*PI) player->a -= 2*PI;

    player->vel[X] = player->inp[X]*cos(player->a) + player->inp[Y]*cos(player->a+PI/2);
    player->vel[Y] = player->inp[X]*sin(player->a) + player->inp[Y]*sin(player->a+PI/2);

    float normal_dem = sqrt(player->vel[X]*player->vel[X] + player->vel[Y]*player->vel[Y]);
    if (normal_dem == 0) return;

    player->vel[X] = player->vel[X]/normal_dem;
    player->vel[Y] = player->vel[Y]/normal_dem;
}

void Player_handleCollision(Player* player, Map* map, double dt, App* app) {
    float minX = player->pos[X]-player->size, maxX = player->pos[X]+player->size;
    float minY = player->pos[Y]-player->size, maxY = player->pos[Y]+player->size;

    int detection_radius = 2*PLAYER_COLLISION_RADIUS + 1;
    int detection_area = detection_radius*detection_radius;

    int wall_det_h_idx = 0;
    int wall_det_h[detection_area];

    int wall_det_v_idx = 0;
    int wall_det_v[detection_area];

    int hDetected = 0, vDetected = 0;

    for (int x=-PLAYER_COLLISION_RADIUS; x<=PLAYER_COLLISION_RADIUS; x++) {
        for (int y=-PLAYER_COLLISION_RADIUS; y<=PLAYER_COLLISION_RADIUS; y++) {
            int index_h = coordToIndex(floor(player->pos[X]+x), floor(player->pos[Y]+y), MAP_MAX_X);
            int index_v = coordToIndex(floor(player->pos[X]+x), floor(player->pos[Y]+y), MAP_MAX_X+1);

            if (index_h >= 0 && index_h < MAP_MAX_H &&
                map->mapH[index_h] != T_NONE) {
                wall_det_h[wall_det_h_idx++] = index_h;
            }

            if (index_v >= 0 && index_v < MAP_MAX_V &&
                map->mapV[index_v] != T_NONE) {
                wall_det_v[wall_det_v_idx++] = index_v;
            }
        }
    }

    double wallY;
    double wallX;

    for (int i=0; i<wall_det_h_idx; i++) {
        int idx = wall_det_h[i];
        int wx = idx % MAP_MAX_X;
        int wy = idx / MAP_MAX_X;

        if (wx <= maxX && wx+1 >= minX &&
            wy <= maxY && wy >= minY) {
            hDetected = 1;
            wallY = wy;
            break;
        }
    }

    for (int i=0; i<wall_det_v_idx; i++) {
        int idx = wall_det_v[i];
        int wx = idx % (MAP_MAX_X+1);
        int wy = idx / (MAP_MAX_X+1);

        if (wx <= maxX && wx >= minX &&
            wy <= maxY && wy+1 >= minY) {
            vDetected = 1;
            wallX = wx;
            break;
        }
    }

    int normal[2];
    normal[X] = (player->pos[X] < wallX)? -vDetected:vDetected;
    normal[Y] = (player->pos[Y] < wallY)? -hDetected:hDetected;

    if (vDetected && normal[X]*player->vel[X] < 0) player->vel[X] = 0;
    if (hDetected && normal[Y]*player->vel[Y] < 0) player->vel[Y] = 0;
}

void Player_handleVelocity(Player* player, double dt) {
    player->pos[X] += player->vel[X]*dt*player->speed;
    player->pos[Y] += player->vel[Y]*dt*player->speed;
    player->a += player->vela*dt;
    player->ap += player->velap*dt;

    if (LIMIT_PITCH) {
        if (player->ap < -PI/4) player->ap = -PI/4;
        if (player->ap > PI/4) player->ap = PI/4;
    }
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

    int hit_tile[2];

    MapFaceType type = T_NONE;
    RayHitSide side = VERTICAL;
    double distance = 0.0;
    while (type == T_NONE && distance < RAYCAST_MAX_DIST) {
        if (lengths[VERT] < lengths[HORZ]) {
            tile_pos[X] += tile_step[X];
            distance = lengths[VERT];
            side = VERTICAL;
            lengths[VERT] += len_step[VERT];

            hit_tile[X] = tile_pos[X]+tile_correct[X]-(ray_dir[X] > 0);
            hit_tile[Y] = tile_pos[Y];

            type = map->mapV[coordToIndex(tile_pos[X]+tile_correct[X], tile_pos[Y], MAP_MAX_X+1)];
        } else {
            tile_pos[Y] += tile_step[Y];
            distance = lengths[HORZ];
            side = HORIZONTAL;
            lengths[HORZ] += len_step[HORZ];

            hit_tile[X] = tile_pos[X];
            hit_tile[Y] = tile_pos[Y]+tile_correct[Y]-(ray_dir[Y] > 0);

            type = map->mapH[coordToIndex(tile_pos[X], tile_pos[Y]+tile_correct[Y], MAP_MAX_X)];
        }

        if (tile_pos[X] < 0 || tile_pos[X] >= MAP_MAX_X || tile_pos[Y] < 0 || tile_pos[Y] >= MAP_MAX_Y) break;
    }

    #undef HORZ
    #undef VERT

    double end_pos[2] = {pos[X]+ray_dir[X]*distance, pos[Y]+ray_dir[Y]*distance};

    double u;
    if (side==VERTICAL) {
        u = (ray_dir[X] > 0) ? end_pos[Y]-tile_pos[Y] : 1-(end_pos[Y]-tile_pos[Y]);
    } else {
        u = (ray_dir[Y] > 0) ? 1-(end_pos[X]-tile_pos[X]) : end_pos[X]-tile_pos[X];
    }

    RayData data = {.dist = distance, 
                    .type = type, 
                    .end_pos = {end_pos[X], end_pos[Y]}, 
                    .hit_tile = {hit_tile[X], hit_tile[Y]},
                    .side = side, 
                    .u = u};

    return data;
}