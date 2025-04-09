#include "../include/display.h"

SDL_Surface* textures;
SDL_Surface* shotgun;
SDL_Surface* pause_logo;
SDL_Surface* font;

void display_textures(u32* pixels) {
  for (int i = 0; i < SCREEN_WIDTH; i++)
    for (int j = 0; j < SCREEN_HEIGHT; j++)
      pixels[j * SCREEN_WIDTH + i] =
          ((u32*)textures->pixels)[j * TEX_FILE_WIDTH + i];
}

int init_textures() {
  IMG_Init(IMG_INIT_PNG);

  // environment.png is a 9 x 12 grid of 64 x 64 pixel textures separated by 5
  // pixel buffers with texture[8][11] being blank
  textures = IMG_Load("textures/environment.png");
  shotgun = IMG_Load("textures/shotgun.png");
  pause_logo = IMG_Load("textures/pause_logo.png");
  font = IMG_Load("textures/font.png");
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

int draw_shotgun(u32* pixels, int idx) {
  int tex_x, tex_y;
  u32 color;
  get_shotgun_idx(idx, &tex_x, &tex_y);
  for (int i = 0; i < SHTGN_WIDTH * 2; i++) {
    for (int j = 0; j < SHTGN_HEIGHT * 2; j++) {
      color =
          ((u32*)shotgun
               ->pixels)[(tex_y + (SHTGN_HEIGHT - (j / 2))) * SHTGN_FILE_WIDTH +
                         (tex_x + (i / 2))];
      if (color != 0xff0fffff && color != 0xffffff0f)
        pixels[j * SCREEN_WIDTH +
               (i + (SCREEN_WIDTH - (2 * SHTGN_WIDTH + 190)) / 2)] = color;
    }
  }
  return 0;
}

int draw_point(u32* pixels, player* p, int_vec3* pt) {
  int dx = pt->x - p->pos.x, dy = p->pos.y - pt->y, dz = pt->z - p->pos.z;
  // add 90 degrees since we want to project onto the camera plane which is
  // perpendicular to the player
  float cs = cos((p->angle + 90) * M_PI / 180),
        sn = sin((p->angle + 90) * M_PI / 180);
  float rotated_x, rotated_z;
  int_vec2 screen;

  rotated_x = dx * cs + dz * sn;
  rotated_z = dz * cs - dx * sn;

  if (rotated_z >= 0) return 1;

  screen.x = SCREEN_WIDTH / 2 + (200 * rotated_x / rotated_z);
  screen.y = SCREEN_HEIGHT / 2 + (200 * dy / rotated_z);

  if (screen.x >= 0 && screen.x < SCREEN_WIDTH && screen.y >= 0 &&
      screen.y < SCREEN_HEIGHT)
    pixels[(screen.y * SCREEN_WIDTH) + screen.x] = 0xFFFFFFFF;

  return 0;
}

float clip_wall(int* x0, int* y0, int* z0, int x1, int y1, int z1) {
  float da = *z0;
  float db = z1;
  float d = da - db;
  if (d <= 0.0001) {
    d = 1;
  }
  float s = *z0 / d;
  *x0 = *x0 + s * (x1 - (*x0));
  *y0 = *y0 + s * (y1 - (*y0));
  *z0 = *z0 + s * (z1 - (*z0));
  if (*z0 == 0) {
    *z0 = -1;
  }
  return s;
}

void draw_line(u32* pixels, int_vec2* start, int_vec2* end) {
  int dx = end->x - start->x, dy = end->y - start->y;
  float angle = atan2(dy, dx);
  float len = sqrt(dx * dx + dy * dy);
  int_vec2 current;

  for (float i = 0; i < len; i++) {
    current.x = start->x + i * cos(angle) + 1;
    current.y = start->y + i * sin(angle) + 1;
    if (current.x >= 0 && current.x < SCREEN_WIDTH && current.y >= 0 &&
        current.y < SCREEN_HEIGHT)
      pixels[(current.y * SCREEN_WIDTH) + current.x] = 0xFFFFFFFF;
  }
}

void fill_wall(u32* pixels, int_vec2* start_t, int_vec2* end_t,
               int_vec2* start_b, int_vec2* end_b) {
  int dx = end_t->x - start_t->x, dy_t = end_t->y - start_t->y,
      dy_b = end_b->y - start_b->y;
  int x, y_b, y_t;
  float slope_t = (float)dy_t / dx;
  float slope_b = (float)dy_b / dx;

  for (float i = 0; i < dx; i++) {
    x = start_b->x + i;
    y_b = start_b->y + (i * slope_b);
    if (y_b < 0) {
      y_b = 0;
    }
    y_t = start_t->y + (i * slope_t);
    if (y_t >= SCREEN_HEIGHT - 1) {
      y_t = SCREEN_HEIGHT;
    }
    for (int j = y_b; j < y_t; j++)
      if (x >= 0 && x < SCREEN_WIDTH)
        pixels[(j * SCREEN_WIDTH) + x] = 0xFFFFFFFF;
  }
}

int clip_diff(int end, int start, int clip) {
  int diff = 0, screen_end = end, screen_start = start;
  if (start < 0) screen_start = 0;
  if (screen_start >= clip) screen_start = clip - 1;

  if (end < 0) screen_end = 0;
  if (screen_end >= clip) screen_end = clip - 1;

  diff = screen_end - screen_start;

  return abs(diff);
}

void fill_wall_textured(u32* pixels, int_vec2* start_t, int_vec2* end_t,
                        int_vec2* start_b, int_vec2* end_b, int tex_idx,
                        float wall_len) {
  int dx = end_t->x - start_t->x, dy_t = end_t->y - start_t->y,
      dy_b = end_b->y - start_b->y;
  float step_x = (TEXTURE_WIDTH / (float)dx), step_y;
  int tex_x_base, tex_y_base, tex_x, tex_y;
  int x, y_b, y_t;
  get_texture_idx(tex_idx, &tex_x_base, &tex_y_base);

  float slope_t = (float)dy_t / dx;
  float slope_b = (float)dy_b / dx;

  for (float i = 0; i < dx; i++) {
    x = start_b->x + i;
    y_b = start_b->y + (i * slope_b);
    y_t = start_t->y + (i * slope_t);
    step_y = TEXTURE_HEIGHT / (float)(y_t - y_b);
    if (y_t >= SCREEN_HEIGHT - 1) {
      y_t = SCREEN_HEIGHT;
    }
    if (y_b < 0) {
      y_b = 0;
    }
    tex_x = ((int)(i * step_x) % TEXTURE_WIDTH) + tex_x_base;
    for (int j = y_b; j < y_t; j++)
      if (x >= 0 && x < SCREEN_WIDTH) {
        tex_y = ((j - y_b) * step_y) + tex_y_base;
        pixels[(j * SCREEN_WIDTH) + x] =
            ((u32*)textures->pixels)[tex_y * TEX_FILE_WIDTH + tex_x];
      }
  }
}

int draw_wall(u32* pixels, player* p, wall* w, map_section* s) {
  int dx_w = w->end.x - w->start.x, dy_w = w->end.y - w->start.y;
  int dx_s = w->start.x - p->pos.x, dx_e = w->end.x - p->pos.x,
      dy_t = p->pos.y - w->height, dy_b = p->pos.y - 0,
      dz_s = w->start.y - p->pos.z, dz_e = w->end.y - p->pos.z;
  // add 90 degrees since we want to project onto the camera plane which is
  // perpendicular to the player
  float cs = cos((p->angle + 90) * M_PI / 180),
        sn = sin((p->angle + 90) * M_PI / 180);
  int rot_xs, rot_xe, rot_zs, rot_ze;
  int_vec2 pt_s, pt_e;
  int_vec2 pb_s, pb_e;

  rot_xs = dx_s * cs + dz_s * sn;
  rot_xe = dx_e * cs + dz_e * sn;
  rot_zs = dz_s * cs - dx_s * sn;
  rot_ze = dz_e * cs - dx_e * sn;
  if (rot_zs >= 0 && rot_ze >= 0) return 1;
  if (rot_zs >= 0) {
    clip_wall(&rot_xs, &dy_t, &rot_zs, rot_xe, dy_t, rot_ze);
    clip_wall(&rot_xs, &dy_b, &rot_zs, rot_xe, dy_b, rot_ze);
  }
  if (rot_ze >= 0) {
    clip_wall(&rot_xe, &dy_b, &rot_ze, rot_xs, dy_b, rot_zs);
    clip_wall(&rot_xe, &dy_t, &rot_ze, rot_xs, dy_t, rot_zs);
  }

  pt_s.x = SCREEN_WIDTH / 2 + (200 * rot_xs / rot_zs);
  pt_s.y = SCREEN_HEIGHT / 2 + (200 * dy_t / rot_zs);
  pt_e.x = SCREEN_WIDTH / 2 + (200 * rot_xe / rot_ze);
  pt_e.y = SCREEN_HEIGHT / 2 + (200 * dy_t / rot_ze);

  pb_s.x = SCREEN_WIDTH / 2 + (200 * rot_xs / rot_zs);
  pb_s.y = SCREEN_HEIGHT / 2 + (200 * dy_b / rot_zs);
  pb_e.x = SCREEN_WIDTH / 2 + (200 * rot_xe / rot_ze);
  pb_e.y = SCREEN_HEIGHT / 2 + (200 * dy_b / rot_ze);
  w->dist = (rot_ze + rot_zs) / 2;

  fill_wall_textured(pixels, &pt_s, &pt_e, &pb_s, &pb_e, w->texture,
                     sqrt(dx_w * dx_w + dy_w * dy_w));
  fill_wall_textured(pixels, &pt_e, &pt_s, &pb_e, &pb_s, w->texture,
                     sqrt(dx_w * dx_w + dy_w * dy_w));

  return 0;
}

int draw_section(u32* pixels, player* p, map_section* s) {
  s->walls = reorder_walls(s->walls, p);
  wall* tmp = s->walls;
  if (p->pos.y > tmp->height)
    s->surface_type = 1;
  else if (p->pos.y < tmp->height)
    s->surface_type = 2;
  else
    s->surface_type = 0;

  for (int i = 0; i < SCREEN_WIDTH - 1; i++) {
    s->roof_pts[2 * i] = 0;
    s->roof_pts[2 * i + 1] = SCREEN_HEIGHT;
  }

  while (tmp != NULL) {
    draw_wall(pixels, p, tmp, s);
    tmp = tmp->next;
  }
  if (s->surface_type != 0) draw_surface(s->roof_pts, s->roof_color);
  return 0;
}

int pause_screen(u32* pixels) {
  float step_x = (float)2 * PAUSE_LOGO_W / (SCREEN_WIDTH);
  float step_y = (float)2 * PAUSE_LOGO_H / (SCREEN_HEIGHT);
  int tex_x, tex_y;
  for (int x = SCREEN_WIDTH / 4; x < 3 * SCREEN_WIDTH / 4; x++) {
    for (int y = SCREEN_HEIGHT / 4; y < 3 * SCREEN_HEIGHT / 4; y++) {
      tex_x = (x - (SCREEN_WIDTH / 4)) * step_x;
      tex_y = PAUSE_LOGO_H - ((y - (SCREEN_HEIGHT / 4)) * step_y) - 1;
      pixels[y * SCREEN_WIDTH + x] =
          ((u32*)pause_logo->pixels)[tex_y * PAUSE_LOGO_W + tex_x];
    }
  }

  static text* start_button;
  if (!start_button) {
    int_vec2 pos = {SCREEN_WIDTH / 2 - 60, 50};
    init_text(&pos, 25, "START", 5, 0xFFFFFFFF, 0, 1, &start_button);
    /*
    start_button = (text*)malloc(sizeof(text));
    memcpy(&start_button->pos, &loc, sizeof(int_vec2));
    start_button->len = 5;
    start_button->font_size = 25;
    start_button->clickable = 0;
    start_button->display = 1;
    start_button->msg = (char*)malloc(start_button->len * sizeof(char));
    start_button->color = 0xFFFFFFFF;
    strcpy(start_button->msg, "START");*/
  }
  // draw_text(pixels, start_button);
  return 0;
}

void draw_sky(u32* pixels, int ceiling_tex) {
  int tex_x, tex_y;
  get_texture_idx(ceiling_tex, &tex_x, &tex_y);
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    for (int y = SCREEN_HEIGHT / 2; y < SCREEN_HEIGHT; y++) {
      pixels[y * SCREEN_WIDTH + x] =
          ((u32*)
               textures->pixels)[(y % TEXTURE_HEIGHT + tex_y) * TEX_FILE_WIDTH +
                                 (x % TEXTURE_WIDTH + tex_x)];
    }
  }
}

void draw_floor(u32* pixels, int floor_tex) {
  int tex_x, tex_y;
  get_texture_idx(floor_tex, &tex_x, &tex_y);
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    for (int y = 0; y < SCREEN_HEIGHT / 2; y++) {
      pixels[y * SCREEN_WIDTH + x] =
          ((u32*)
               textures->pixels)[(y % TEXTURE_HEIGHT + tex_y) * TEX_FILE_WIDTH +
                                 (x % TEXTURE_WIDTH + tex_x)];
    }
  }
}

void draw_grid(u32* pixels) {
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
      if (y % 20 == 0 || x % 20 == 0) pixels[y * SCREEN_WIDTH + x] = 0x80808080;
    }
  }
}

void draw_wall_to_grid(u32* pixels, wall* w, int_vec2* editor) {
  int dx = w->end.x - w->start.x, dy = w->end.y - w->start.y;
  float len = 20 * sqrt(dx * dx + dy * dy);
  float slope = atan2(w->end.y - w->start.y, w->end.x - w->start.x);
  int y_val, x_val;

  for (int i = 0; i < (int)len + 1; i++) {
    y_val =
        ((w->start.y - editor->y) * 20 + i * sin(slope)) + (SCREEN_HEIGHT / 2);
    x_val =
        ((w->start.x - editor->x) * 20 + i * cos(slope)) + (SCREEN_WIDTH / 2);
    if (y_val < SCREEN_HEIGHT && y_val >= 0 && x_val < SCREEN_WIDTH &&
        x_val >= 0)
      pixels[y_val * SCREEN_WIDTH + x_val] = 0x00FFFF00;
  }
}

void draw_temp_wall(u32* pixels, int_vec2* s, int_vec2* e, int_vec2* editor) {
  wall temp;
  temp.start.x = s->x;
  temp.start.y = s->y;
  temp.end.x = e->x;
  temp.end.y = e->y;
  draw_wall_to_grid(pixels, &temp, editor);
}

void draw_player_to_grid(u32* pixels, player* p, int_vec2* editor) {
  int x_val = 20 * (p->pos.x - editor->x) + (SCREEN_WIDTH / 2),
      y_val = 20 * (p->pos.z - editor->y) + (SCREEN_HEIGHT / 2);
  for (int x = x_val - 5; x < x_val + 5; x++) {
    for (int y = y_val - 5; y < y_val + 5; y++) {
      if (y < SCREEN_HEIGHT && y >= 0 && x < SCREEN_WIDTH && x >= 0)
        pixels[y * SCREEN_WIDTH + x] = 0x000000FF;
    }
  }
}

void get_letter_offset(int_vec2* offset, char letter) {
  if (letter >= 65 && letter <= 90) {
    int l_idx = (letter - 65);
    offset->x = (l_idx % 9) * LETTER_W + 12;
    offset->y = -(l_idx / 9) * LETTER_H + 10;
  } else if (letter >= 97 && letter <= 122) {
    int l_idx = (letter - 97);
    offset->x = (l_idx % 9) * LETTER_W + 12;
    offset->y = -(l_idx / 9) * LETTER_H - 10 - 3 * LETTER_H;
  } else if (letter >= 48 && letter <= 57) {
    int l_idx = (letter - 48);
    offset->x = (l_idx % 9) * LETTER_W + 12;
    offset->y = -(l_idx / 9) * LETTER_H - 25 - 6 * LETTER_H;
  } else if (letter == '(' || letter == ')') {
    int l_idx = (letter - '(');
    offset->x = (l_idx + 5) * LETTER_W + 12;
    offset->y = -8 * LETTER_H - 20;
  }
}

void draw_text(u32* pixels, text* txt) {
  if (!txt->display) return;

  float step_x = (float)(LETTER_W - 10) / txt->font_size;
  float step_y = (float)(LETTER_H - 5) / txt->font_size;
  int tex_x, tex_y;
  int_vec2 offset = {0, 0};
  int screen_offset = -txt->font_size;
  int y_pos = txt->pos.y;
  for (int i = 0; i < txt->len; i++) {
    screen_offset += txt->font_size;
    if (txt->msg[i] && txt->msg[i] == '\n') {
      y_pos -= txt->font_size + 10;
      screen_offset = -txt->font_size;
      continue;
    }
    if (txt->msg[i] && txt->msg[i] != ' ') {
      get_letter_offset(&offset, txt->msg[i]);
      for (int x = 0; x < txt->font_size; x++) {
        for (int y = 0; y < txt->font_size - 5; y++) {
          tex_x = x * step_x + offset.x;
          tex_y = LETTER_H - (y * step_y + offset.y);
          u32 pixel_val = ((u32*)font->pixels)[tex_y * FONT_W + tex_x];
          if (pixel_val != 4291611852 && pixel_val != 0)
            pixels[(y + y_pos) * SCREEN_WIDTH +
                   (x + txt->pos.x + screen_offset)] = txt->color;
        }
      }
    }
  }
}

void init_text(int_vec2* pos, int size, char* str, int len, u32 color,
               int clickable, int display, text** ptr) {
  *ptr = (text*)(malloc(sizeof(text)));
  (*ptr)->clickable = clickable;
  (*ptr)->display = display;
  (*ptr)->font_size = size;
  (*ptr)->len = len;
  memcpy(&(*ptr)->pos, pos, sizeof(int_vec2));
  (*ptr)->color = color;
  (*ptr)->msg = (char*)malloc(len * sizeof(char));
  memcpy((*ptr)->msg, str, len);
  (*ptr)->next = NULL;
}

void draw_level_edit(u32* pixels, map_data* level, player* p,
                     int_vec2* editor) {
  draw_grid(pixels);
  int num_wall;
  wall* w;
  draw_player_to_grid(pixels, p, editor);
  for (int i = 0; i < level->num_sections; i++) {
    w = level->sections[i].walls;
    while (w != NULL) {
      draw_wall_to_grid(pixels, w, editor);
      w = w->next;
    }
  }
}

int clear_screen(u32* pixels) {
  for (int x = 0; x < SCREEN_WIDTH; x++)
    for (int y = 0; y < SCREEN_HEIGHT; y++) pixels[x * SCREEN_HEIGHT + y] = 0;

  return 0;
}

int render_screen(SDL_Texture* texture, SDL_Renderer* renderer, u32* pixels) {
  SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(u32));
  SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
  SDL_RenderPresent(renderer);

  return 0;
}
