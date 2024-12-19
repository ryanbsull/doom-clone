#ifndef MAP_H
#define MAP_H

#include "util.h"

typedef struct {
  vec2 start;
  vec2 end;
  int height;
} wall;

typedef struct {
  wall* bounds;
  int num_walls;
  int floor;
  int ceiling;
} map_section;

#endif
