#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <string.h>

#include "map.h"
#include "player.h"
#include "util.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 400

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64
#define TEXTURE_BORDER 5
#define TEX_FILE_WIDTH 626
#define TEX_FILE_HEIGHT 833

#define SHTGN_WIDTH 230
#define SHTGN_HEIGHT 162
#define SHTGN_FILE_WIDTH 230
#define SHTGN_FILE_HEIGHT 1300

#define PAUSE_LOGO_W 1126
#define PAUSE_LOGO_H 710

#define FONT_H 1150
#define FONT_W 1000

#define LETTER_W 110
#define LETTER_H 120

#define YELLOW_TEXT 0xFFFFD300

enum side { x_side, y_side };

typedef struct text_area {
  int display;
  int clickable;
  int font_size;
  int len;
  int_vec2 pos;
  u32 color;
  char* msg;
  struct text_area* next;
} text;

void display_textures(uint32_t* pixels);
int init_textures();
void get_texture_idx(int idx, int* tex_x, int* tex_y);
void get_shotgun_idx(int idx, int* tex_x, int* tex_y);
int draw_shotgun(uint32_t* pixels, int idx);
int draw_ray(uint32_t* pixels, int x, int hit_point, int start, int end,
             int idx, int side);
int draw_point(uint32_t* pixels, player* p, int_vec3* pt);
int draw_wall(uint32_t* pixels, player* p, wall* w);
void draw_ceiling(u32* pixels, int ceiling_tex);
void draw_floor(u32* pixels, int floor_tex);
int clear_screen(uint32_t* pixels);
void draw_level_edit(u32* pixels, map_data* level, player* p, int_vec2* editor);
void draw_temp_wall(u32* pixels, int_vec2* s, int_vec2* e, int_vec2* editor);
void draw_text(u32* pixels, int_vec2* pos, int size, char* str, int len,
               u32 color);
int render_screen(SDL_Texture* texture, SDL_Renderer* renderer,
                  uint32_t* pixels);
int pause_screen(uint32_t* pixels);

#endif
