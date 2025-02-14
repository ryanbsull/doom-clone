#include <stdio.h>

#include "../include/player.h"

void move(player* p, int dir) {
  if (dir == FWD || dir == BACK) {
    p->vel.x = cos((float)p->angle * M_PI / 180) * dir * MVMT_SPEED;
    p->vel.z = sin((float)p->angle * M_PI / 180) * dir * MVMT_SPEED;
  } else {
    p->vel.x =
        cos((float)(p->angle + 90 * (dir / 2)) * M_PI / 180) * MVMT_SPEED;
    p->vel.z =
        sin((float)(p->angle + 90 * (dir / 2)) * M_PI / 180) * MVMT_SPEED;
  }
}

void rotate(player* p, int dir) {
  p->angle += ANGULAR_SPEED * (-dir / 2);
  if (p->angle > 359) p->angle -= 360;
  if (p->angle < 0) p->angle += 360;
}

void jump(player* p) { p->vel.y = JUMP_HEIGHT; }

void update_player(player* p) {
  printf("VELOCITY VECTOR: <%f,%f,%f>\n", p->vel.x, p->vel.y, p->vel.z);
  p->pos.x += round(p->vel.x);
  p->pos.z += round(p->vel.z);
  p->pos.y += p->vel.y;
  if (p->pos.y - P_HEIGHT < 0) {
    p->pos.y = P_HEIGHT;
    p->vel.y = 0;
  }

  p->vel.x = 0;
  p->vel.z = 0;
}
