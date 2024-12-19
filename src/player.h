#ifndef PLAYER_H
#define PLAYER_H

#include "util.h"
#include <math.h>

#define MVMT_SPEED    0.2
#define ANGULAR_SPEED 0.1

int map[] = {
		32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0,32,32,32,32, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0,32, 0,32,32, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,
		32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
};

typedef struct {
	vec2 pos;
	vec2 dir;
	vec2 cam;
} player;


void move(player* p, int dir) {
	float angle = atan2(p->dir.y, p->dir.x);

	if (map[(int)(p->pos.x + cos(angle) * dir) * 16 + (int)(p->pos.y + sin(angle) * dir)] == 1)
		return;
	
	if (p->pos.x + cos(angle) * dir < 15 && p->pos.x + cos(angle) * dir >= 0.66) {
		p->pos.x += cos(angle) * dir * MVMT_SPEED;
	}
	if (p->pos.y + sin(angle) * dir < 15 && p->pos.y + sin(angle) * dir >= 0.66) {
		p->pos.y += sin(angle) * dir * MVMT_SPEED;
	}
}

void rotate(player* p, int dir) {
	float angle = atan2(p->dir.y, p->dir.x);
	angle += ANGULAR_SPEED * dir;
	p->dir.x = cos(angle);
	p->dir.y = sin(angle);
	// ensure the camera plane is always perpendicular to our direction vector
	p->cam.x = -p->dir.y;
	p->cam.y = p->dir.x;
}

#endif
