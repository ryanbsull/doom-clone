#ifndef PLAYER_H
#define PLAYER_H

#include <math.h>

#include "util.h"

#define MVMT_SPEED 1       // in squares per call to move()
#define ANGULAR_SPEED 0.5  // in degrees per call to rotate()
#define JUMP_HEIGHT 5      // in squares per call to jump()
#define P_HEIGHT 5         // height of the player

typedef struct {
  int_vec3 pos;
  int angle;  // [0-360] degrees
  vec3 vel;
} player;

extern int map[];
void move(player* p, int dir);
void rotate(player* p, int dir);
void jump(player* p);
void update_player(player* p);

#endif
