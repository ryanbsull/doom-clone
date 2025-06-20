#ifndef ENEMY_H
#define ENEMY_H

#include "util.h"

typedef struct {
  int_vec3 pos;
  int angle;  // [0-360] degrees
  vec3 vel;
} enemy;

extern int map[];
void move_enemy(enemy* p);
void change_dir_enemy(enemy* p);
void update_enemy(enemy* p);

#endif
