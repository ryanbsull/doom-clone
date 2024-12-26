#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>

#include "include/util.h"
#include "include/map.h"
#include "include/player.h"
#include "include/display.h"

struct {
	SDL_Window* win;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
	player player;
} state;

int init();
int game_loop();
int cleanup();

int main() {
	printf("DOOM\n");
	if (init())
		return 1;
	while (!game_loop()) {
		SDL_Delay(5);
	}
	return cleanup();
}

int init() {
	state.win = SDL_CreateWindow(
		"doom",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
		);
	if (!state.win) {
		printf("Failed to create window:\n\tError: %s", SDL_GetError());
		return 1;
	}

	state.renderer = SDL_CreateRenderer(state.win, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!state.renderer) {
		printf("Failed to create renderer:\n\tError: %s", SDL_GetError());
		return 1;
	}

	state.texture = SDL_CreateTexture(
		state.renderer, 
		SDL_PIXELFORMAT_RGB888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT
		);
	if (!state.texture) {
		printf("Failed to create texture:\n\tError: %s", SDL_GetError());
		return 1;
	}

	state.player.pos.x = 10;
	state.player.pos.y = 10;
	state.player.dir.x = 1;
	state.player.dir.y = 0;
	// define camera plane where the view will be projected onto
	state.player.cam.x = 0;
	state.player.cam.y = 0.66;

	init_textures();
	return 0;
}

int intercepts(player* p, wall* w, float dist) {
	return (p->pos.x + dist * p->dir.x >= fmin(w->start.x, w->end.x) && p->pos.x + dist * p->dir.x <= fmax(w->start.x, w->end.x) &&
		p->pos.y + dist * p->dir.y >= fmin(w->start.y, w->end.y) && p->pos.y + dist * p->dir.y <= fmax(w->start.y, w->end.y));
}

float get_intercept_dist(player* p, wall* w) {
	vec2 intc_pt; 
	float dist = -(((p->pos.x - w->start.x)*(w->start.y - w->end.y) - (p->pos.y - w->start.y)*(w->start.x - w->end.x)) / 
		((p->dir.x)*(w->start.y - w->end.y) - (p->dir.y)*(w->start.x - w->end.x)));
	if (!intercepts(p, w , dist) || dist  < 0.66)
		return -1;
	return dist;
}

void raycast(player* p, int slice) {
	float cam_x = 2*(float)slice / SCREEN_WIDTH - 1;
	vec2 slice_start, slice_end, // vectors that will be used for drawing our vertical slices across the screen
	side_dist, // tracks the length of the ray from the current positiojn to the next x or y side
	ray_dir = {p->dir.x + p->cam.x * cam_x, p->dir.y + p->cam.y * cam_x}, // calculate ray direction from the camera plane
	delta_dist = {(ray_dir.x == 0) ? 1e30 : fabs(1 / ray_dir.x), (ray_dir.y == 0) ? 1e30 : fabs(1 / ray_dir.y)};
	int_vec2 map_pos = {(int)p->pos.x, (int)p->pos.y}; // track integer map position
	float dist; // track distance until a wall is encountered for this ray
	int hit = map[map_pos.x * 16 + map_pos.y]; // track if the ray has hit a wall
	enum side s;
	int step_x, step_y;

	if (ray_dir.x < 0) {
		step_x = -1;
		side_dist.x = (p->pos.x - (float) map_pos.x) * delta_dist.x;
	} else {
		step_x = 1;
		side_dist.x = ((float) map_pos.x + 1 - p->pos.x) * delta_dist.x;
	}
	if (ray_dir.y < 0) {
		step_y = -1;
		side_dist.y = (p->pos.y - (float) map_pos.y) * delta_dist.y;
	} else {
		step_y = 1;
		side_dist.y = ((float) map_pos.y + 1 - p->pos.y) * delta_dist.y;
	}

	while (hit == 0) {
		if (side_dist.x < side_dist.y) {
			side_dist.x += delta_dist.x;
			map_pos.x += step_x;
			s = x_side;
		} else {
			side_dist.y += delta_dist.y;
			map_pos.y += step_y;
			s = y_side;
		}
		hit = map[map_pos.x * 16 + map_pos.y];
	}
	switch(s) {
		case x_side:
			dist = (side_dist.x - delta_dist.x);
			break;
		case y_side:
			dist = (side_dist.y - delta_dist.y);
			break;
	}
	int tex_idx = hit;
	double wall_hit_x = 
			(s == x_side) ? 
			      p->pos.y + dist * ray_dir.y : 
			      p->pos.x + dist * ray_dir.x;
	wall_hit_x -= trunc(wall_hit_x);
	int tex_slice = (int)(wall_hit_x * (double)TEXTURE_WIDTH);
	tex_slice = TEXTURE_WIDTH - tex_slice - 1;

	int line_height = (int)(SCREEN_HEIGHT / dist);
	int draw_start = (SCREEN_HEIGHT - line_height) / 2;
	int draw_end = (SCREEN_HEIGHT + line_height) / 2;

	slice_start.x = slice;
	slice_end.x = slice;

	slice_start.y = draw_start;
	slice_end.y = draw_end;
	draw_ray(state.pixels, slice_start.x, tex_slice, slice_start.y, slice_end.y, tex_idx, s);
}

int game_loop() {
	clear_screen(state.pixels);
	static int shotgun_idx = 0, time = 0, dt = 0;
	int prev_time = time;
	time = SDL_GetTicks();
	dt += time - prev_time;
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				return 1;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
					case SDLK_w:
						move(&state.player, FWD);
						break;
					case SDLK_s:
						move(&state.player, BACK);
						break;
					case SDLK_a:
						rotate(&state.player, LEFT);
						break;
					case SDLK_d:
						rotate(&state.player, RIGHT);
						break;
					case SDLK_p:
						printf("Player Info:\n\tPosition: [%f,%f]\n\tDirection: [%f,%f]\n", state.player.pos.x, state.player.pos.y, state.player.dir.x, state.player.dir.y);
						break;
					case SDLK_e:
						shotgun_idx = 1;
						break;
				}
				break;
		}
	}

	for (int i = 0; i < SCREEN_WIDTH; i++)
		raycast(&state.player, i);

	draw_shotgun(state.pixels, shotgun_idx);
	if (shotgun_idx != 0 && dt > 100) {
		shotgun_idx = (shotgun_idx + 1) % 8;
		dt = 0;
	}
	render_screen(state.texture, state.renderer, state.pixels);
	return 0;
}

int cleanup() {
	SDL_DestroyTexture(state.texture);
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.win);
	SDL_Quit();
	return 0;
}