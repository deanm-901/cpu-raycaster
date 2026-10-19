#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "map.h"
#include "app.h"

typedef struct {
    double pos[2],a,ap;
    double vel[2],vela,velap;
    double inp[2],inpa,inpap;
    double height, size;
    float speed, turn_speed;
} Player;

typedef enum {
    VERTICAL,
    HORIZONTAL,
} RayHitSide;

typedef struct {
    double dist;
    double end_pos[2];
    MapFaceType type;
    RayHitSide side;
    double u;
} RayData;


void Player_handleKeyboardInput(Player* player, SDL_KeyboardEvent* input, int down);
void Player_inputVelocity(Player* player);
void Player_handleMouseMotion(Player* player, SDL_MouseMotionEvent* input);
void Player_handleCollision(Player* player, Map* map, double dt, App* app);
void Player_handleVelocity(Player* player, double dt);

RayData Player_castRay(Player* player, Map* map, double angle);

#endif