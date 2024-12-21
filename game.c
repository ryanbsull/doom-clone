#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
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

	default_map(); // initialize default map
	state.player.pos.x = 10;
	state.player.pos.y = 10;
	state.player.dir.x = 1;
	state.player.dir.y = 0;
	// define camera plane where the view will be projected onto
	state.player.cam.x = 0;
	state.player.cam.y = -1;

	init_textures();
	return 0;
}

int intercepts(player* p, wall* w, float dist) {
	return (p->pos.x + dist * p->dir.x >= fmin(w->start.x, w->end.x) && p->pos.x + dist * p->dir.x <= fmax(w->start.x, w->end.x) &&
		p->pos.y + dist * p->dir.y >= fmin(w->start.y, w->end.y) && p->pos.y + dist * p->dir.y <= fmax(w->start.y, w->end.y));
}

float get_intercept_dist(player* p, wall* w, float* hit_pt) {
	vec2 intc_pt; 
	float dist = -(((p->pos.x - w->start.x)*(w->start.y - w->end.y) - (p->pos.y - w->start.y)*(w->start.x - w->end.x)) / 
			((p->dir.x)*(w->start.y - w->end.y) - (p->dir.y)*(w->start.x - w->end.x)));
	if (!intercepts(p, w , dist) || dist  < 0.66)
		return -1;
	*hit_pt = fabs(((p->dir.x)*(p->pos.y - w->start.y) - (p->dir.y)*(p->pos.x - w->start.x)) / 
			((p->dir.x)*(w->start.y - w->end.y) - (p->dir.y)*(w->start.x - w->end.x)));
	*hit_pt = *hit_pt * (sqrtf(pow(w->end.x - w->start.x, 2) + pow(w->end.y - w->start.y, 2)));
	*hit_pt = *hit_pt - floorf(*hit_pt);
	return fabs(dist);
}

void raycast(player* p, int slice) {
	player temp_player;
	float dist = INFINITY, current_dist;
	float offset = ((float) slice / SCREEN_WIDTH) - 0.5;
	temp_player.pos.x = p->pos.x + offset * (p->cam.x);
	temp_player.pos.y = p->pos.y + offset * (p->cam.y);
	temp_player.dir.x = p->dir.x;
	temp_player.dir.y = p->dir.y;
	int tex_idx = 32, wall_idx = -1;
	float hit_pt, current_hit_pt;
	for (int i = 0; i < current_map.num_sections; i++)
		for (int j = 0; j < current_map.sections[i].num_walls; j++) {
			current_dist = get_intercept_dist(&temp_player, 
				&current_map.sections[i].walls[j], &current_hit_pt);
			if (current_dist > 0 && current_dist < dist) {
				wall_idx = j;
				dist = current_dist;
				hit_pt = current_hit_pt;
			}
		}
	int tex_slice = (int)((float) TEXTURE_WIDTH * hit_pt);
	int line_height = (int)(SCREEN_HEIGHT / dist);
	int draw_start = (SCREEN_HEIGHT - line_height) / 2;
	int draw_end = (SCREEN_HEIGHT + line_height) / 2;

	draw_ray(state.pixels, slice, tex_slice, draw_start, draw_end, tex_idx, wall_idx % 2);
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