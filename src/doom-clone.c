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

int main() {
	printf("DOOM\n");
	return 0;
}