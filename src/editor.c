#include "../include/editor.h"

void move_editor(int_vec2 *editor, int dir) {
	if (dir == FWD || dir == BACK) {
		editor->y += dir * EDIT_MVMT;
	} else if (dir == LEFT || dir == RIGHT) {
		editor->x -= (dir / 2) * EDIT_MVMT;
	}
}

void add_wall(int_vec2 *editor, map_data *map, int_vec2* start, int_vec2* end) {
	printf("WALL VALUES: \n\tSTART: [%d,%d]\n\tEND: [%d,%d]\n", start->x, start->y, end->x, end->y);
	return;
}

void translate_to_editor(int x, int y, int_vec2* coords, int_vec2* editor) {
	coords->x = ((x - (SCREEN_WIDTH / 2)) / 20) + editor->x;
	coords->y = ((-y + (SCREEN_HEIGHT / 2)) / 20) + editor->y;
}
