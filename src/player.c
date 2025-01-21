#include "../include/player.h"

void move(player* p, int dir) {
  if (dir == FWD || dir == BACK) {
    p->pos.x += cos((float)p->angle * M_PI / 180) * dir * MVMT_SPEED;
    p->pos.z += sin((float)p->angle * M_PI / 180) * dir * MVMT_SPEED;
  } else {
    p->pos.x +=
        cos((float)(p->angle + 90 * (dir / 2)) * M_PI / 180) * MVMT_SPEED;
    p->pos.z +=
        sin((float)(p->angle + 90 * (dir / 2)) * M_PI / 180) * MVMT_SPEED;
  }
}

void rotate(player* p, int dir) {
  p->angle += ANGULAR_SPEED * (-dir / 2);
  if (p->angle > 359) p->angle -= 360;
  if (p->angle < 0) p->angle += 360;
}
