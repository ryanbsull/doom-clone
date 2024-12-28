#include "../include/player.h"

void move(player* p, int dir) {
	p->pos.x += cos((float)p->angle * M_PI / 180) * dir * MVMT_SPEED;
	p->pos.y += sin((float)p->angle * M_PI / 180) * dir * MVMT_SPEED;
}

void rotate(player* p, int dir) {
	p->angle += ANGULAR_SPEED * dir;
	if (p->angle > 359)
		p->angle -= 360;
	if (p->angle < 0)
		p->angle += 360;
}
