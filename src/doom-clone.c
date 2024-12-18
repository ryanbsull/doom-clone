#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdint.h>
#include "player.h"
#include "display.h"

typedef uint32_t 	u32;
typedef uint8_t 	u8;

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
		"raycast_test",
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

	init_textures();
	return 0;
}

int game_loop() {
	clear_screen(state.pixels);
	// render 4 texture skull - pentagram using draw_ray() function
	for (int i = 0; i < TEXTURE_WIDTH; i++)
		draw_ray(state.pixels, i + 100, i, 100, 200, 23, y_side);
	for (int i = 0; i < TEXTURE_WIDTH; i++)
		draw_ray(state.pixels, i + 164, i, 100, 200, 24, y_side);
	for (int i = 0; i < TEXTURE_WIDTH; i++)
		draw_ray(state.pixels, i + 100, i, 200, 300, 25, y_side);
	for (int i = 0; i < TEXTURE_WIDTH; i++)
		draw_ray(state.pixels, i + 164, i, 200, 300, 26, y_side);

	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				return 1;
		}
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