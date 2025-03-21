#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>

#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_mouse.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_scancode.h"
#include "include/display.h"
#include "include/editor.h"
#include "include/map.h"
#include "include/player.h"
#include "include/util.h"

struct {
  SDL_Window* win;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
  player player;
  int_vec2 editor;
  const unsigned char* keys;
  int key_size;
  text* screen_text;
} state;

int init();
int game_loop();
int cleanup();
int handle_keys(int* pause, int* level_edit, int* shotgun_idx, int* minimap,
                int* print, int* stats, int dt);

int main() {
  printf("DOOM\n");
  if (init()) return 1;
  while (!game_loop()) {
    SDL_Delay(10);
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
  state.player.pos.y = P_HEIGHT;
  state.player.pos.z = 0;  // player will have a height of 1
  state.player.vel.x = 0;
  state.player.vel.y = 0;
  state.player.vel.z = 0;
  state.player.angle = 0;

  // the center the editor at [0,0]
  state.editor.x = 0;
  state.editor.y = 0;

  state.keys = SDL_GetKeyboardState(&state.key_size);
  init_textures();
  SDL_SetRelativeMouseMode(SDL_FALSE);

  // init blank text object
  int_vec2 pos = {0, 0};
  init_text(&pos, 0, NULL, 0, 0x00000000, 0, 0, &state.screen_text);
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
             level_edit = 0, stats = 0;
  static int_vec2 wall_s = {MAX_MAP_VAL, MAX_MAP_VAL},
                  wall_e = {MAX_MAP_VAL, MAX_MAP_VAL};
  int print = 0;
  int prev_time = time;
  time = SDL_GetTicks();
  int elapsed = time - prev_time;
  dt += time - prev_time;
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT:
        return 1;
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
          if (sqrt((wall_s.x - wall_e.x) * (wall_s.x - wall_e.x) +
                   (wall_s.y - wall_e.y) * (wall_s.y - wall_e.y)) > 5)
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
  handle_keys(&pause, &level_edit, &shotgun_idx, &minimap, &print, &stats, dt);

  if (!pause) {
    draw_ceiling(state.pixels, current_map.sections[0].ceiling_tex);
    draw_floor(state.pixels, current_map.sections[0].floor_tex);
    current_map.sections[0].walls =
        reorder_walls(current_map.sections[0].walls, &state.player);
    wall* tmp = current_map.sections[0].walls;
    while (tmp != NULL) {
      draw_wall(state.pixels, &state.player, tmp);
      tmp = tmp->next;
    }

    draw_shotgun(state.pixels, shotgun_idx / 10);

    if (!state.screen_text->next) {
      int_vec2 text_pos = {20, SCREEN_HEIGHT - 25};
      char* init = (char*)malloc(20 * sizeof(char));
      init_text(&text_pos, 20, init, 20, YELLOW_TEXT, 0, 0,
                &state.screen_text->next);
    }
    text* stat_txt = state.screen_text->next;
    if (stats) {
      // display the FPS in the top left
      memset(stat_txt->msg, 0, stat_txt->len);
      int fps = 1000 / dt;
      sprintf(stat_txt->msg, "%d\n(%d %d %d)", fps, state.player.pos.x,
              state.player.pos.y, state.player.pos.z);
      stat_txt->display = 1;
    } else {
      if (stat_txt) stat_txt->display = 0;
    }
    if (stat_txt->pos.y != SCREEN_HEIGHT - 25) printf("WHAT HAPPENED?\n");
    draw_text(state.pixels, stat_txt);

    if (dt > 20) {
      if (shotgun_idx != 0) {
        shotgun_idx = (shotgun_idx + 3) % 70;
      }
      if (state.player.pos.y - P_HEIGHT >= 0) {
        if (state.player.vel.y > -15)
          state.player.vel.y -= (((float)dt) / 1000) * 10;
      }
      update_player(&state.player);

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

int handle_keys(int* pause, int* level_edit, int* shotgun_idx, int* minimap,
                int* print, int* stats, int dt) {
  static int num_esc = 0;
  state.keys = SDL_GetKeyboardState(NULL);
  if (num_esc <= 4) num_esc += state.keys[SDL_SCANCODE_ESCAPE];
  if (num_esc > 4) {
    *pause = !*pause;
    if (!*pause) *level_edit = 0;
    SDL_SetRelativeMouseMode(!*pause);
    num_esc = 0;
  }
  if (state.keys[SDL_SCANCODE_SPACE]) {
    if (!*pause &&
        state.player.pos.y - P_HEIGHT == current_map.sections[0].floor)
      jump(&state.player);
  }
  if (state.keys[SDL_SCANCODE_Q]) {
    if (*level_edit) pop_wall(&current_map, 0);
  }
  if (state.keys[SDL_SCANCODE_W]) {
    if (!*pause)
      move(&state.player, FWD);
    else
      move_editor(&state.editor, FWD);
  }
  if (state.keys[SDL_SCANCODE_S]) {
    if (!*pause)
      move(&state.player, BACK);
    else
      move_editor(&state.editor, BACK);
  }
  if (state.keys[SDL_SCANCODE_A]) {
    if (!*pause)
      move(&state.player, LEFT);
    else
      move_editor(&state.editor, LEFT);
  }
  if (state.keys[SDL_SCANCODE_D]) {
    if (!*pause)
      move(&state.player, RIGHT);
    else
      move_editor(&state.editor, RIGHT);
  }
  if (state.keys[SDL_SCANCODE_T]) {
    if (!*pause) *stats = 1;
  }
  if (state.keys[SDL_SCANCODE_P]) {
    printf(
        "Player Info:\n\tPosition: [%d,%d,%d]\n\tDirection: %u "
        "degrees\n",
        state.player.pos.x, state.player.pos.y, state.player.pos.z,
        state.player.angle);
    *print = 1;
  }
  if (state.keys[SDL_SCANCODE_E]) {
    if (!*pause) *shotgun_idx = 1;
  }
  if (state.keys[SDL_SCANCODE_T]) {
    if (!*pause) *minimap = (*minimap + 1) % 2;
  }
  if (state.keys[SDL_SCANCODE_L]) {
    if (*pause) *level_edit = 1;
  }
  if (state.keys[SDL_SCANCODE_Z]) {
    save_map();
  }
  if (state.keys[SDL_SCANCODE_R]) {
    load_map("levels/one.lvl");
  }
  if (state.keys[SDL_SCANCODE_N]) {
    if (*level_edit) new_lvl();
  }
  return 0;
}
