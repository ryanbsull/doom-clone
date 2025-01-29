#ifndef MAP_H
#define MAP_H

// arbitrary maximum value of a point on the map
#define MAX_MAP_VAL 1024
#define MAX_LVL_NAME 100

static int lvl_num = 1;

#include <stdio.h>
#include <stdlib.h>

#include "util.h"

typedef struct _wall {
  int_vec2 start;
  int_vec2 end;
  int height;
  int texture;
  struct _wall* next;
} wall;

typedef struct {
  wall* walls;
  int num_walls;
  int floor;
  int floor_tex;
  int ceiling;
  int ceiling_tex;
} map_section;

typedef struct {
  map_section* sections;
  int num_sections;
} map_data;

extern map_data current_map;

int default_map();
int load_map(char* map_file);
int save_map();
void new_lvl();
void add_wall(map_data* map, int_vec2* start, int_vec2* end, int section);
void pop_wall(map_data* map, int section);

#endif
