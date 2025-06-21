#include "../include/enemy.h"

void move_enemy(enemy* p) {
  p->vel.x = ENEMY_SPEED * cos((double)p->angle / 360);
  p->vel.y = ENEMY_SPEED * sin((double)p->angle / 360);
}

void change_dir_enemy(enemy* p) {}

void update_enemy(enemy* p) {
  p->pos.x += round(p->vel.x);
  p->pos.z += round(p->vel.z);
  p->pos.y += p->vel.y;
  if (p->pos.y - p->height < 0) {
    p->pos.y = p->height;
    p->vel.y = 0;
  }

  p->vel.x = 0;
  p->vel.z = 0;
}
