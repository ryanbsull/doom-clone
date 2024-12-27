#include "../include/player.h"

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

void move(player* p, int dir) {
	float angle = atan2(p->dir.y, p->dir.x);

	p->pos.x += cos(angle) * dir * MVMT_SPEED;
	p->pos.y += sin(angle) * dir * MVMT_SPEED;
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
