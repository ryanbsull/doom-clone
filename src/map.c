#include "../include/map.h"

map_data current_map;

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
