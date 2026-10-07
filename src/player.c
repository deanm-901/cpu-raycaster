#include "player.h"
#include "defs.h"
#include <math.h>

void Player_handleKeyboardInput(Player* player, SDL_KeyboardEvent* input, int down) {
    if (input->repeat != 0) return;

    switch (input->keysym.scancode)
    {
    case SDL_SCANCODE_W:
        player->inp[X] = down;
        break;
    case SDL_SCANCODE_S:
        player->inp[X] = -down;
        break;
    case SDL_SCANCODE_A:
        player->inpa = -down;
        break;
    case SDL_SCANCODE_D:
        player->inpa = down;
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

    player->vel[X] = player->inp[X]*cos(player->a) + player->inp[Y]*sin(player->a);
    player->vel[Y] = player->inp[X]*sin(player->a) + player->inp[Y]*cos(player->a);

    float normal_dem = sqrt(player->vel[X]*player->vel[X] + player->vel[Y]*player->vel[Y]);
    if (normal_dem == 0) return;

    player->vel[X] = player->vel[X]/normal_dem;
    player->vel[Y] = player->vel[Y]/normal_dem;

    player->pos[X] += player->vel[X]*dt*player->speed;
    player->pos[Y] += player->vel[Y]*dt*player->speed;
}

