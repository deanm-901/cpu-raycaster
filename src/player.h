#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "map.h"

typedef struct {
    double pos[2],a;
    double vel[2],vela;
    double inp[2],inpa;
    float speed, turn_speed;
} Player;

typedef enum {
    VERTICAL,
    HORIZONTAL,
} RayHitSide;

typedef struct {
    double dist;
    double end_pos[2];
    MapWallType type;
    RayHitSide side;
    double u;
} RayData;


void Player_handleKeyboardInput(Player* player, SDL_KeyboardEvent* input, int down);
void Player_inputVelocity(Player* player, double dt);

RayData Player_castRay(Player* player, Map* map, double angle);

#endif