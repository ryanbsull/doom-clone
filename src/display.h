#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include <stdint.h>

#define SCREEN_WIDTH		640
#define SCREEN_HEIGHT		400

#define TEXTURE_WIDTH		64
#define TEXTURE_HEIGHT	64
#define TEXTURE_BORDER	5
#define TEX_FILE_WIDTH	626
#define TEX_FILE_HEIGHT	833

enum side {
	x_side,
	y_side
};

SDL_Surface* textures;

void display_textures(uint32_t* pixels) {
	for (int i = 0; i < SCREEN_WIDTH; i++)
    for(int j = 0; j < SCREEN_HEIGHT;  j++)
			pixels[j*SCREEN_WIDTH + i] = ((uint32_t*)textures->pixels)[j*SCREEN_WIDTH + i];
}

int init_textures() {
	IMG_Init(IMG_INIT_PNG);

	// textures.png is a 9 x 12 grid of 64 x 64 pixel textures separated by 5 pixel buffers
	// with texture[8][11] being blank 
	textures = IMG_Load("textures/environment.png");
	printf("FLAGS: %x\n", textures->flags);
	return 0;
}

void get_texture_idx(int idx, int* tex_x, int* tex_y) {
	*tex_x = TEXTURE_BORDER * ((idx % 9) + 1);
	*tex_x += (idx % 9) * TEXTURE_WIDTH;
	*tex_y = TEXTURE_BORDER * (idx / 9 + 1);
	*tex_y += (idx / 9) * TEXTURE_WIDTH;
}

int draw_ray(uint32_t* pixels, int x, int hit_point, int start, int end, int idx, int side) {
	int y, tex_y, tex_x, tex_y_base; 
	uint32_t color;
	float step = TEXTURE_HEIGHT / (float)(end - start);
	get_texture_idx((side == x_side) ? idx : idx-1, &tex_x, &tex_y_base);
	tex_x += hit_point;

	for (y = start; y < end; y++) {
		tex_y = (y - start) * step;
		color = ((uint32_t*)textures->pixels)[(tex_y + tex_y_base) * TEX_FILE_WIDTH + tex_x];
		pixels[y * SCREEN_WIDTH + x] = color;
	}

	return 0;
}

int clear_screen(uint32_t* pixels) {
	for (int x = 0; x < SCREEN_WIDTH; x++)
		for(int y = 0; y < SCREEN_HEIGHT; y++)
			pixels[x*SCREEN_HEIGHT + y] = 0;

	return 0;
}

int render_screen(SDL_Texture* texture, SDL_Renderer* renderer, uint32_t* pixels) {
	SDL_UpdateTexture(
		texture,
		NULL,
		pixels, SCREEN_WIDTH * sizeof(uint32_t)
	);
	SDL_RenderCopyEx(
		renderer,
		texture,
		NULL, NULL,
		0.0, NULL,
		SDL_FLIP_NONE
	);
	SDL_RenderPresent(renderer);

	return 0;
}

#endif
