#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	400

enum dir {
	LEFT = -1,
	RIGHT = 1,
	FWD = 1,
	BACK = -1,
};

enum side {
	x_side,
	y_side
};

typedef uint32_t 	u32;
typedef uint8_t 	u8;

typedef struct {
	float x,y;
} vec2;

typedef struct {
	int x,y;
} int_vec2;

typedef struct {
	vec2 pos;
	vec2 dir;
	vec2 cam;
} player;

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
		SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT
		);
	if (!state.texture) {
		printf("Failed to create texture:\n\tError: %s", SDL_GetError());
		return 1;
	}
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