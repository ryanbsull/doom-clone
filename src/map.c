#include "../include/map.h"

map_data current_map;

int default_map() {
  current_map.num_sections = 1;
  current_map.sections = (map_section*)malloc(sizeof(map_section));
  current_map.sky_tex = 16;
  current_map.floor_tex = 15;
  current_map.sections[0].ceiling = 10;
  current_map.sections[0].ceiling_color = 0xFFFFFFFF;
  current_map.sections[0].floor = 0;
  current_map.sections[0].floor_color = 0x00000000;
  current_map.sections[0].num_walls = 0;
  current_map.sections[0].walls = NULL;
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
    fread(&current_map.sections[i].ceiling_color, sizeof(u32), 1, file);
    fread(&current_map.sections[i].floor, sizeof(int), 1, file);
    fread(&current_map.sections[i].floor_color, sizeof(u32), 1, file);
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
int save_map() {
  FILE* file;
  char lvl_name[MAX_LVL_NAME];
  snprintf(lvl_name, MAX_LVL_NAME, "levels/%d.lvl", lvl_num);
  file = fopen(lvl_name, "w+");
  fwrite(&current_map.num_sections, sizeof(int), 1, file);
  for (int i = 0; i < current_map.num_sections; i++) {
    fwrite(&current_map.sections[i].ceiling, sizeof(int), 1, file);
    fwrite(&current_map.sections[i].ceiling_color, sizeof(u32), 1, file);
    fwrite(&current_map.sections[i].floor, sizeof(int), 1, file);
    fwrite(&current_map.sections[i].floor_color, sizeof(u32), 1, file);
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

void new_lvl() {
  lvl_num++;
  current_map.num_sections = 1;
  free(current_map.sections);
  current_map.sections = (map_section*)malloc(sizeof(map_section));
}

void add_wall(map_data* map, int_vec2* start, int_vec2* end, int section,
              int height, int tex) {
  if (section > map->num_sections) return;
  map->sections[section].num_walls++;
  wall* w = map->sections[section].walls;
  while (w != NULL && w->next != NULL) w = w->next;

  if (w != NULL) {
    w->next = (wall*)malloc(sizeof(wall));
    w = w->next;
  } else {
    map->sections[section].walls = (wall*)malloc(sizeof(wall));
    w = map->sections[section].walls;
  }

  w->start.x = start->x;
  w->start.y = start->y;
  w->end.x = end->x;
  w->end.y = end->y;
  w->height = height;
  w->texture = tex;
}

void pop_wall(map_data* map, int section) {
  if (section > map->num_sections) return;
  map->sections[section].num_walls--;
  wall* w = map->sections[section].walls;
  while (w->next != NULL && w->next->next != NULL) w = w->next;
  free(w->next);
  w->next = NULL;
}

int get_len(wall* w) {
  int len = 0;
  wall* tmp = w;
  while (tmp != NULL) {
    tmp = tmp->next;
    len++;
  }
  return len;
}

void print_walls(wall* walls, player* p) {
  int count = 0;
  wall* w = walls;
  while (w != NULL) {
    printf("WALL %d: %d\n", count, w->dist);
    w = w->next;
    count++;
  }
}

// simple bubble sort to test
// TODO: implement quicker sorting algo
wall* reorder_walls(wall* walls, player* p) {
  int len = get_len(walls), itr = 0, swap;
  wall* w = walls;

  while (itr < len) {
    wall* traverse = w;
    wall* prev = w;
    swap = 0;

    while (traverse->next != NULL) {
      wall* ptr = traverse->next;
      if (traverse->dist > ptr->dist) {
        swap = 1;
        if (traverse == w) {
          traverse->next = ptr->next;
          ptr->next = traverse;
          prev = ptr;
          w = prev;
        } else {
          traverse->next = ptr->next;
          ptr->next = traverse;
          prev->next = ptr;
          prev = ptr;
        }
        continue;
      }
      prev = traverse;
      traverse = traverse->next;
    }

    if (!swap) break;

    ++itr;
  }

  return w;
}
