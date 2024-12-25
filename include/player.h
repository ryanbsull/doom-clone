#ifndef PLAYER_H
#define PLAYER_H

#include "util.h"
#include <math.h>

#define MVMT_SPEED    0.2
#define ANGULAR_SPEED 0.1

typedef struct {
	vec2 pos;
	vec2 dir;
	vec2 cam;
} player;

extern int map[];
void move(player* p, int dir);
void rotate(player* p, int dir);

#endif
