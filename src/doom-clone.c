#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "player/player.h"

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	400

#define TEXTURE_WIDTH	64
#define TEXTURE_HEIGHT	64

enum side {
	x_side,
	y_side
};

typedef uint32_t 	u32;
typedef uint8_t 	u8;

struct {
	SDL_Window* win;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
	player player;
} state;

SDL_Surface* textures;

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
		SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT
		);
	if (!state.texture) {
		printf("Failed to create texture:\n\tError: %s", SDL_GetError());
		return 1;
	}

	IMG_Init(IMG_INIT_PNG);
	// load textures from single PNG
	// get individual textures by indexing into subset of textures->pixels
	textures = IMG_Load("textures.png");
	return 0;
}

int game_loop() {
	return 1;
}

int cleanup() {
	SDL_DestroyTexture(state.texture);
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.win);
	SDL_Quit();
	return 0;
}