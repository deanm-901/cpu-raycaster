#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>

typedef struct {
    double x,y,a;
    int vx,vy;
    float speed;
} Player;

void Player_handleKeyboardInput(Player* player, SDL_KeyboardEvent* input, int down);
void Player_inputVelocity(Player* player, double dt);

#endif