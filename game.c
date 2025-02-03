#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>

#include "SDL2/SDL_events.h"
#include "SDL2/SDL_mouse.h"
#include "SDL2/SDL_render.h"
#include "include/display.h"
#include "include/editor.h"
#include "include/map.h"
#include "include/player.h"

struct {
  SDL_Window* win;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
  player player;
  int_vec2 editor;
} state;

int init();
int game_loop();
int cleanup();

int main() {
  printf("DOOM\n");
  if (init()) return 1;
  while (!game_loop()) {
    SDL_Delay(5);
  }
  return cleanup();
}

int init() {
  state.win =
      SDL_CreateWindow("doom", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!state.win) {
    printf("Failed to create window:\n\tError: %s", SDL_GetError());
    return 1;
  }

  state.renderer = SDL_CreateRenderer(state.win, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!state.renderer) {
    printf("Failed to create renderer:\n\tError: %s", SDL_GetError());
    return 1;
  }

#if defined(__APPLE__)
  printf("APPLE\n");
  state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_RGB888,
                                    SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                                    SCREEN_HEIGHT);
#elif defined(__linux__)
  printf("LINUX\n");
  state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888,
                                    SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                                    SCREEN_HEIGHT);
#endif
  if (!state.texture) {
    printf("Failed to create texture:\n\tError: %s", SDL_GetError());
    return 1;
  }

  default_map();  // initialize default map
  state.player.pos.x = 0;
  state.player.pos.y = 5;
  state.player.pos.z = 0;  // player will have a height of 1
  state.player.angle = 0;

  // the center the editor at [0,0]
  state.editor.x = 0;
  state.editor.y = 0;

  init_textures();
  SDL_SetRelativeMouseMode(SDL_FALSE);
  return 0;
}

void draw_to_minimap(u32* pixels, int x, int y, u32 color) {
  if (y < 0 || (((SCREEN_HEIGHT * 3) / 4) + y) > SCREEN_HEIGHT ||
      ((SCREEN_WIDTH * 3) / 4) + x > SCREEN_WIDTH || x < 0)
    return;
  pixels[(((SCREEN_HEIGHT * 3) / 4) + y) * SCREEN_WIDTH +
         (x + (SCREEN_WIDTH * 3) / 4)] = color;
}

int show_minimap(u32* pixels, player* p, vec3* pt) {
  for (int i = (SCREEN_WIDTH * 3 / 4); i < SCREEN_WIDTH; i += 3)
    pixels[(SCREEN_HEIGHT * 3 / 4) * SCREEN_WIDTH + i] = 0xFFFFFFFF;
  for (int i = (SCREEN_HEIGHT * 3 / 4); i < SCREEN_HEIGHT; i += 3)
    pixels[i * SCREEN_WIDTH + (SCREEN_WIDTH * 3 / 4)] = 0xFFFFFFFF;

  int dx = pt->x - p->pos.x, dy = pt->z - p->pos.z, sign = 1;
  float cs = cos(p->angle * M_PI / 180), sn = sin(p->angle * M_PI / 180);

  // draw player
  draw_to_minimap(pixels, (int)p->pos.x, (int)p->pos.z, 0xFF00FFFF);

  // draw point
  draw_to_minimap(pixels, (int)pt->x, (int)pt->z, 0xFFFF00FF);

  // draw direction vector
  for (int i = 2; i < 10; i++)
    draw_to_minimap(pixels, (int)(p->pos.x + i * cs), ((int)p->pos.z + i * sn),
                    0x00FFFFFF);

  // draw dx, dy lines from player to point
  if (dx < 0) sign = -1;
  for (int i = 0; i < dx * sign; i += 2)
    draw_to_minimap(pixels, (int)(p->pos.x + i * sign), (int)p->pos.z,
                    0xFFFFFF00);
  sign = 1;
  if (dy < 0) sign = -1;
  for (int i = 0; i < dy * sign; i += 2)
    draw_to_minimap(pixels, (int)(p->pos.x + dx), ((int)p->pos.z + i * sign),
                    0xFFFFFF00);
  sign = 1;

  // show relative x and y directions
  int world_x = dy * cs - dx * sn;
  int world_y = dx * cs - dy * sn;
  world_y *=
      (sqrt(pow(dx, 2) + pow(dy, 2)) / sqrt(pow(world_x, 2) + pow(world_y, 2)));
  if (world_y < 0) sign = -1;
  for (int i = 0; i < world_y * sign; i++)
    draw_to_minimap(pixels, (int)(p->pos.x + i * sign * cs),
                    ((int)p->pos.z + i * sign * sn), 0xF0F8F088);
  sign = 1;
  if (world_x < 0) sign = -1;
  for (int i = 0; i < world_x * sign; i++)
    draw_to_minimap(pixels, (int)(p->pos.x + i * sign * (-sn) + (world_y * cs)),
                    ((int)p->pos.z + i * sign * cs + (world_y * sn)),
                    0xF0F8F088);

  return 0;
}

int game_loop() {
  clear_screen(state.pixels);
  static int shotgun_idx = 0, minimap = 0, time = 0, dt = 0, pause = 1,
             level_edit = 0;
  static int_vec2 wall_s = {MAX_MAP_VAL, MAX_MAP_VAL},
                  wall_e = {MAX_MAP_VAL, MAX_MAP_VAL};
  int print = 0;
  int prev_time = time;
  time = SDL_GetTicks();
  dt += time - prev_time;
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT:
        return 1;
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
          case SDLK_ESCAPE:
            pause = (pause + 1) % 2;
            if (!pause) level_edit = 0;
            SDL_SetRelativeMouseMode(!pause);
            break;
          case SDLK_q:
            if (level_edit) pop_wall(&current_map, 0);
            break;
          case SDLK_w:
            if (!pause)
              move(&state.player, FWD);
            else
              move_editor(&state.editor, FWD);
            break;
          case SDLK_s:
            if (!pause)
              move(&state.player, BACK);
            else
              move_editor(&state.editor, BACK);
            break;
          case SDLK_a:
            if (!pause)
              move(&state.player, LEFT);
            else
              move_editor(&state.editor, LEFT);
            break;
          case SDLK_d:
            if (!pause)
              move(&state.player, RIGHT);
            else
              move_editor(&state.editor, RIGHT);
            break;
          case SDLK_p:
            printf(
                "Player Info:\n\tPosition: [%d,%d,%d]\n\tDirection: %u "
                "degrees\n",
                state.player.pos.x, state.player.pos.y, state.player.pos.z,
                state.player.angle);
            print = 1;
            break;
          case SDLK_e:
            if (!pause) shotgun_idx = 1;
            break;
          case SDLK_t:
            if (!pause) minimap = (minimap + 1) % 2;
            break;
          case SDLK_l:
            if (pause) level_edit = 1;
            break;
          case SDLK_z:
            save_map();
            break;
          case SDLK_r:
            load_map("levels/one.lvl");
            break;
          case SDLK_n:
            if (level_edit) new_lvl();
            break;
        }
        break;
      case SDL_MOUSEMOTION:
        if (level_edit) {
          translate_to_editor(e.button.x, e.button.y, &wall_e, &state.editor);
        } else
          rotate(&state.player, e.motion.xrel);
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (level_edit) {
          translate_to_editor(e.button.x, e.button.y, &wall_s, &state.editor);
        }
        break;
      case SDL_MOUSEBUTTONUP:
        if (level_edit) {
          translate_to_editor(e.button.x, e.button.y, &wall_e, &state.editor);
          add_wall(&current_map, &wall_s, &wall_e, 0);
          // reset wall values
          wall_s.x = MAX_MAP_VAL;
          wall_s.y = MAX_MAP_VAL;
          wall_e.x = MAX_MAP_VAL;
          wall_e.y = MAX_MAP_VAL;
        }
        break;
    }
  }

  if (!pause) {
    draw_ceiling(state.pixels, current_map.sections[0].ceiling_tex);
    draw_floor(state.pixels, current_map.sections[0].floor_tex);
    current_map.sections[0].walls =
        reorder_walls(current_map.sections[0].walls, &state.player.pos);
    wall* tmp = current_map.sections[0].walls;
    while (tmp != NULL) {
      draw_wall(state.pixels, &state.player, tmp);
      tmp = tmp->next;
    }

    draw_shotgun(state.pixels, shotgun_idx);
    if (shotgun_idx != 0 && dt > 150) {
      shotgun_idx = (shotgun_idx + 1) % 7;
      dt = 0;
    }
  } else {
    if (level_edit) {
      draw_level_edit(state.pixels, &current_map, &state.player, &state.editor);
      if (wall_s.x != MAX_MAP_VAL && wall_s.y != MAX_MAP_VAL &&
          wall_e.x != MAX_MAP_VAL && wall_e.y != MAX_MAP_VAL)
        draw_temp_wall(state.pixels, &wall_s, &wall_e, &state.editor);
    } else {
      clear_screen(state.pixels);
      pause_screen(state.pixels);
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
