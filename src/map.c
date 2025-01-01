#include "../include/map.h"

map_data current_map;

int default_map() {
  current_map.num_sections = 1;
  current_map.sections = (map_section*) malloc(sizeof(map_section) * current_map.num_sections);
  current_map.sections[0].num_walls = 7;
  current_map.sections[0].walls = (wall*) malloc(sizeof(wall) * current_map.sections[0].num_walls);
  // box that the player starts in
  current_map.sections[0].walls[0].start.x = 0;
  current_map.sections[0].walls[0].start.y = 0;
  current_map.sections[0].walls[0].end.x = 15;
  current_map.sections[0].walls[0].end.y = 0;
  current_map.sections[0].walls[0].height = 10;
  current_map.sections[0].walls[1].start.x = 15;
  current_map.sections[0].walls[1].start.y = 0;
  current_map.sections[0].walls[1].end.x = 15;
  current_map.sections[0].walls[1].end.y = 15;
  current_map.sections[0].walls[1].height = 10;
  current_map.sections[0].walls[2].start.x = 15;
  current_map.sections[0].walls[2].start.y = 15;
  current_map.sections[0].walls[2].end.x = 0;
  current_map.sections[0].walls[2].end.y = 15;
  current_map.sections[0].walls[2].height = 110;
  current_map.sections[0].walls[3].start.x = 0;
  current_map.sections[0].walls[3].start.y = 15;
  current_map.sections[0].walls[3].end.x = 0;
  current_map.sections[0].walls[3].end.y = 0;
  current_map.sections[0].walls[3].height = 10;
  // triangle within the room
  current_map.sections[0].walls[4].start.x = 2;
  current_map.sections[0].walls[4].start.y = 2;
  current_map.sections[0].walls[4].end.x = 1;
  current_map.sections[0].walls[4].end.y = 3;
  current_map.sections[0].walls[4].height = 10;
  current_map.sections[0].walls[5].start.x = 1;
  current_map.sections[0].walls[5].start.y = 3;
  current_map.sections[0].walls[5].end.x = 3;
  current_map.sections[0].walls[5].end.y = 3;
  current_map.sections[0].walls[5].height = 10;
  current_map.sections[0].walls[6].start.x = 3;
  current_map.sections[0].walls[6].start.y = 3;
  current_map.sections[0].walls[6].end.x = 2;
  current_map.sections[0].walls[6].end.y = 2;
  current_map.sections[0].walls[6].height = 10;
  return 0;
}

// initialize map from a map data file
int load_map(char* map_file) {
  int map_size;
  FILE* file;

  file = fopen(map_file, "rb");
  if (file == NULL)
    goto err;
  if (!fread(&map_size, sizeof(int), 1, file))
    goto err;
  current_map.num_sections = map_size;
  current_map.sections = (map_section *)malloc(map_size * sizeof(map_section));
  if (!fread(&current_map.sections, sizeof(map_section), map_size, file))
    goto cleanup_map;

  return 0;
cleanup_map:
  free(current_map.sections);
err:
  current_map.num_sections = 0;
  return 1;
}

// save the current map to a file
int save_map(char* map_file) {
  return 0;
}
