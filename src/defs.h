#ifndef DEFS_H
#define DEFS_H

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_NAME "Raycaster"

#define MAP_REP_SCALE 40
#define MAP_REP_PLAYER_SIZE 8

#define MAP_MAX_X 16
#define MAP_MAX_Y 16

#define MAP_MAX_H MAP_MAX_X*(MAP_MAX_Y+1)
#define MAP_MAX_V MAP_MAX_Y*(MAP_MAX_X+1)

#define X 0
#define Y 1
#define Z 2

#define PI 3.14159

#define RAYCAST_MAX_DIST 100.0

#define DEG_TO_RAD 0.0174532925
#define RAD_TO_DEG 57.29578

#define FOV 75.0 * DEG_TO_RAD
#define RAYCAST_RES 1.0

#endif