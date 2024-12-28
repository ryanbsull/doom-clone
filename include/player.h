#ifndef PLAYER_H
#define PLAYER_H

#include "util.h"
#include <math.h>

#define MVMT_SPEED    4
#define ANGULAR_SPEED 4

typedef struct {
	vec2 pos;
	int angle; // [0-360] degrees
} player;

extern int map[];
void move(player* p, int dir);
void rotate(player* p, int dir);

#endif
