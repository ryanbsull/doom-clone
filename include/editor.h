#ifndef EDITOR_H
#define EDITOR_H

#define EDIT_MVMT  3  // let editor move at n squares per unit time

#include "util.h"
#include "map.h"
#include "display.h"

void move_editor(int_vec2* editor, int dir);
void add_wall(int_vec2* editor, map_data* map, int_vec2* start, int_vec2* end);
void translate_to_editor(int x, int y, int_vec2* coords, int_vec2* editor);
#endif
