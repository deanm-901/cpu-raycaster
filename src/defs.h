#ifndef DEFS_H
#define DEFS_H

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 400
#define SCREEN_RESOLUTION_SCALE 2
#define SCREEN_NAME "Raycaster"

#define MAP_REP_SCALE 8
#define MAP_REP_PLAYER_SIZE 2

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

#define FOV (60.0 * DEG_TO_RAD)
#define RAYCAST_RES 1

#define MAX_IMAGE_COUNT 128
#define MAX_FACETYPE_COUNT 128

#define PLAYER_COLLISION_RADIUS 1

#define TEXTURE_ATLAS_W 32
#define TEXTURE_ATLAS_H 32

#define FOG_MAX_DIST 20.0f
#define FOG_SUB_AMM 0.4f

// settings
#define DEBUG 1
#define MOUSE_SENS 0.1
#define LIMIT_PITCH 1
#define ENABLE_PITCH 1

int coordToIndex(int x, int y, int width);
void indexToCoord(int index, int width, int* x, int* y);

#endif