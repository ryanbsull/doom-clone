#ifndef PLAYER_H
#define PLAYER_H

#include <math.h>

#include "util.h"

#define MVMT_SPEED 8       // in squares per call to move()
#define ANGULAR_SPEED 0.5  // in degrees per call to rotate()

typedef struct {
  int_vec3 pos;
  int angle;  // [0-360] degrees
} player;

extern int map[];
void move(player* p, int dir);
void rotate(player* p, int dir);

#endif
