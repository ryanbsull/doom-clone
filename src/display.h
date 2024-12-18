#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdint>

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

int draw_ray(uint32_t* pixels, int x, int tex_x, int start, int end, int idx, int side) {
	int y, tex_y, tex_idx = idx * TEXTURE_HEIGHT * TEXTURE_WIDTH;
	uint32_t color;
	float step = TEXTURE_HEIGHT / (float)(end - start);
	float tex_pos = (float)(start) - ((float)SCREEN_HEIGHT / 2) + ((float)(end - start) / 2) * step;

	for (int i = start; i < end; i++) {
		tex_y = (int) tex_pos & (TEXTURE_HEIGHT - 1);
		tex_pos += step;
		color = textures->pixels[tex_idx + TEXTURE_HEIGHT * tex_y + tex_x];
		color = (side == x_side) ? (color >> 1) & 8355711 : color;
		pixels[y * SCREEN_WIDTH + x] = color;
	}

	return 0;
}

#endif
