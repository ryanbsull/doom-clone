#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH		640
#define SCREEN_HEIGHT		400

#define TEXTURE_WIDTH		64
#define TEXTURE_HEIGHT	64

enum side {
	x_side,
	y_side
};

SDL_Surface* textures;

int init_textures() {
	IMG_Init(IMG_INIT_PNG);
	textures = IMG_Load("textures.png");
	return 0;
}

int draw_ray(int x, int start, int end, void* pixels, void* texture, int tex_idx) {
	for (int i = start; i < end; i++);
	return 0;
}

#endif
