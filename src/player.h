#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>

typedef struct {
    double pos[2],a;
    double vel[2],vela;
    double inp[2],inpa;
    float speed, turn_speed;
} Player;

void Player_handleKeyboardInput(Player* player, SDL_KeyboardEvent* input, int down);
void Player_inputVelocity(Player* player, double dt);

#endif