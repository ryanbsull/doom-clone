#include "../include/map.h"

map_data current_map;

int default_map() {
  load_map("levels/one.lvl");
  return 0;
}

// initialize map from a map data file
int load_map(char* map_file) {
  int map_size;
  FILE* file;

  file = fopen(map_file, "rb");
  if (file == NULL) goto err;
  if (!fread(&current_map.num_sections, sizeof(int), 1, file)) goto err;
  current_map.sections = (map_section*)malloc(map_size * sizeof(map_section));
  for (int i = 0; i < current_map.num_sections; i++) {
    fread(&current_map.sections[i].ceiling, sizeof(int), 1, file);
    fread(&current_map.sections[i].ceiling_tex, sizeof(int), 1, file);
    fread(&current_map.sections[i].floor, sizeof(int), 1, file);
    fread(&current_map.sections[i].floor_tex, sizeof(int), 1, file);
    fread(&current_map.sections[i].num_walls, sizeof(int), 1, file);
    current_map.sections[i].walls = (wall*)malloc(sizeof(wall));
    fread(current_map.sections[i].walls, sizeof(wall), 1, file);
    current_map.sections[i].walls->next = (wall*)malloc(sizeof(wall));
    wall* tmp = current_map.sections[i].walls->next;
    for (int j = 1; j < current_map.sections[i].num_walls; j++) {
      fread(tmp, sizeof(wall), 1, file);
      tmp->next = (wall*)malloc(sizeof(wall));
      tmp = tmp->next;
    }
  }

  fclose(file);
  return 0;
cleanup_map:
  for (int i = 0; i < current_map.num_sections; i++)
    free(current_map.sections[i].walls);
  free(current_map.sections);
err:
  current_map.num_sections = 0;
  fclose(file);
  return 1;
}

// save the current map to a file
int save_map(char* map_file) {
  FILE* file;
  file = fopen(map_file, "w+");
  fwrite(&current_map.num_sections, sizeof(int), 1, file);
  for (int i = 0; i < current_map.num_sections; i++) {
    fwrite(&current_map.sections[i].ceiling, sizeof(int), 1, file);
    fwrite(&current_map.sections[i].ceiling_tex, sizeof(int), 1, file);
    fwrite(&current_map.sections[i].floor, sizeof(int), 1, file);
    fwrite(&current_map.sections[i].floor_tex, sizeof(int), 1, file);
    fwrite(&current_map.sections[i].num_walls, sizeof(int), 1, file);
    fwrite(current_map.sections[i].walls, sizeof(wall), 1, file);
    wall* tmp = current_map.sections[i].walls->next;
    while (tmp != NULL) {
      fwrite(tmp, sizeof(wall), 1, file);
      tmp = tmp->next;
    }
  }
  return 0;
}

void add_wall(map_data* map, int_vec2* start, int_vec2* end, int section) {
  if (section > map->num_sections) return;
  map->sections[section].num_walls++;
  wall* w = map->sections[section].walls;
  while (w->next != NULL) w = w->next;

  w->next = (wall*)malloc(sizeof(wall));
  w = w->next;

  w->start.x = start->x;
  w->start.y = start->y;
  w->end.x = end->x;
  w->end.y = end->y;
  w->height = 10;
  w->texture = 30;
}

void pop_wall(map_data* map, int section) {
  if (section > map->num_sections) return;
  map->sections[section].num_walls++;
  wall* w = map->sections[section].walls;
  while (w->next != NULL && w->next->next != NULL) w = w->next;
  free(w->next);
  w->next = NULL;
}
