#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h>

#include "util.h"

typedef struct {
  int_vec2 start;
  int_vec2 end;
  int height;
  int texture;
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
int save_map(char* map_file);

#endif
