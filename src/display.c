#include "../include/display.h"

SDL_Surface* textures;
SDL_Surface* shotgun;
uint8_t firing = 0;

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
	shotgun = IMG_Load("textures/shotgun.png");
	return 0;
}

void get_texture_idx(int idx, int* tex_x, int* tex_y) {
	*tex_x = TEXTURE_BORDER * ((idx % 9) + 1) + (idx % 9) * TEXTURE_WIDTH;
	*tex_y = TEXTURE_BORDER * (idx / 9 + 1) + (idx / 9) * TEXTURE_WIDTH;
}

void get_shotgun_idx(int idx, int* tex_x, int* tex_y) {
	*tex_x = 0;
	*tex_y = idx * SHTGN_HEIGHT + idx;
}

int draw_shotgun(uint32_t* pixels, int idx) {
	int tex_x, tex_y;
	uint32_t color;
	get_shotgun_idx(idx, &tex_x, &tex_y);
	for (int i = 0; i < SHTGN_WIDTH * 2; i++) {
		for (int j = 0; j < SHTGN_HEIGHT * 2; j++) {
			color = ((uint32_t*)shotgun->pixels)[(tex_y + (j / 2)) * SHTGN_FILE_WIDTH + (tex_x + (i / 2))];
			if(color != 0xff0fffff)
				pixels[(j + SCREEN_HEIGHT - 2 * SHTGN_HEIGHT) * SCREEN_WIDTH + (i + (SCREEN_WIDTH - (2 * SHTGN_WIDTH + 190)) / 2)] = color;
		}
	}
	return 0;
}

int draw_point(uint32_t* pixels, player* p, vec3* pt, int* print) {
	int dx = pt->x - p->pos.x, dy = p->pos.y - pt->y, dz = pt->z - p->pos.z;
	float cs = cos((p->angle + 90) * M_PI / 180), sn = sin((p->angle + 90) * M_PI / 180);
	float rotated_x, rotated_z;
	int_vec2 screen;

	rotated_x = dx * cs + dz * sn;
	rotated_z = dz * cs - dx * sn;

	if (rotated_z >= 0)
		return 1;
	
	screen.x = SCREEN_WIDTH / 2 + (200 * rotated_x / rotated_z);
	screen.y = SCREEN_HEIGHT / 2 + (200 * dy / rotated_z);

	if (screen.x >= 0 && screen.x < SCREEN_WIDTH && screen.y >= 0 && screen.y < SCREEN_HEIGHT)
		pixels[(screen.y * SCREEN_WIDTH) + screen.x] = 0xFFFFFFFF;

	return 0;	
}

int draw_wall(player* p, wall* w) {
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
		SDL_FLIP_VERTICAL
	);
	SDL_RenderPresent(renderer);

	return 0;
}
