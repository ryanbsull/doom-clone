#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include <stdint.h>
#include "map.h"
#include "player.h"

#define SCREEN_WIDTH		640
#define SCREEN_HEIGHT		400

#define TEXTURE_WIDTH		64
#define TEXTURE_HEIGHT	64
#define TEXTURE_BORDER	5
#define TEX_FILE_WIDTH	626
#define TEX_FILE_HEIGHT	833

#define SHTGN_WIDTH					230
#define SHTGN_HEIGHT				162
#define SHTGN_FILE_WIDTH		230
#define SHTGN_FILE_HEIGHT		1300

enum side {
	x_side,
	y_side
};

void display_textures(uint32_t* pixels);
int init_textures();
void get_texture_idx(int idx, int* tex_x, int* tex_y);
void get_shotgun_idx(int idx, int* tex_x, int* tex_y);
int draw_shotgun(uint32_t* pixels, int idx);
int draw_ray(uint32_t* pixels, int x, int hit_point, int start, int end, int idx, int side);
int draw_point(uint32_t* pixels, player* p, vec2* pt);
int draw_wall(player* p, wall* w);
int clear_screen(uint32_t* pixels);
int render_screen(SDL_Texture* texture, SDL_Renderer* renderer, uint32_t* pixels);

#endif
