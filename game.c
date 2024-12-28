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

	default_map(); // initialize default map
	state.player.pos.x = 10;
	state.player.pos.y = 10;
	state.player.angle = 0;

	init_textures();
	return 0;
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
						printf("Player Info:\n\tPosition: [%f,%f]\n\tDirection: %u degrees\n", state.player.pos.x, state.player.pos.y, state.player.angle);
						break;
					case SDLK_e:
						shotgun_idx = 1;
						break;
				}
				break;
		}
	}

	vec2 test1;
	vec2 test2;
	test1.x = 20;
	test1.y = 11;
	test2.x = 25;
	test2.y = 15;
	draw_point(state.pixels, &state.player, &test1);
	draw_point(state.pixels, &state.player, &test2);

	/*
	draw_shotgun(state.pixels, shotgun_idx);
	if (shotgun_idx != 0 && dt > 100) {
		shotgun_idx = (shotgun_idx + 1) % 8;
		dt = 0;
	}
	*/
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