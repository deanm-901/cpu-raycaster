#include "player.h"

void Player_handleKeyboardInput(Player* player, SDL_KeyboardEvent* input, int down) {
    if (input->repeat != 0) return;

    switch (input->keysym.scancode)
    {
    case SDL_SCANCODE_W:
        player->vy = -down;
        break;
    case SDL_SCANCODE_S:
        player->vy = down;
        break;
    case SDL_SCANCODE_A:
        player->vx = -down;
        break;
    case SDL_SCANCODE_D:
        player->vx = down;
        break;
    default:
        break;
    }
}
void Player_inputVelocity(Player* player, double dt) {
    player->x += player->vx*dt*player->speed;
    player->y += player->vy*dt*player->speed;
}

