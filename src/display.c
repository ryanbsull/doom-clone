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
			color = ((uint32_t*)shotgun->pixels)[(tex_y + (SHTGN_HEIGHT - (j / 2))) * SHTGN_FILE_WIDTH + (tex_x + (i / 2))];
			if(color != 0xff0fffff && color != 0xffffff0f)
				pixels[j * SCREEN_WIDTH + (i + (SCREEN_WIDTH - (2 * SHTGN_WIDTH + 190)) / 2)] = color;
		}
	}
	return 0;
}

int draw_point(uint32_t* pixels, player* p, vec3* pt) {
	int dx = pt->x - p->pos.x, dy = p->pos.y - pt->y, dz = pt->z - p->pos.z;
	// add 90 degrees since we want to project onto the camera plane which is perpendicular to the player
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

void clip_wall(float* clip_x, int* clip_y, float* clip_z, vec3* start) {
	float d = start->z - *clip_z; if (d == 0) {d = 1;}
	float s = *clip_z / d;
	*clip_x += s*(start->x - *clip_x);
	*clip_y += s*(start->y - *clip_y);
	*clip_z += s*(start->z - *clip_z); if (*clip_z >= -0.0001) {*clip_z = -1;}
}

void draw_line(u32* pixels, int_vec2* start, int_vec2* end) {
	int dx = end->x - start->x, dy = end->y - start->y;
	float angle = atan2(dy, dx);
	float len = sqrt(dx * dx + dy * dy);
	int_vec2 current;

	for (float i = 0; i < len; i++) {
		current.x = start->x + i * cos(angle) + 1;
		current.y = start->y + i * sin(angle) + 1;
		if (current.x >= 0 && current.x < SCREEN_WIDTH && current.y >= 0 && current.y < SCREEN_HEIGHT)
			pixels[(current.y * SCREEN_WIDTH) + current.x] = 0xFFFFFFFF;
	}
}

void fill_wall(u32* pixels, int_vec2* start_t, int_vec2* end_t, int_vec2* start_b, int_vec2* end_b) {
	int dx = end_t->x - start_t->x, dy_t = end_t->y - start_t->y, dy_b = end_b->y - start_b->y;
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
		for(int j = y_b; j < y_t; j++)
			if (x >= 0 && x < SCREEN_WIDTH)
				pixels[(j * SCREEN_WIDTH) + x] = 0xFFFFFFFF;
	}
}

int draw_wall(u32* pixels, player* p, wall* w) {
	int dx_s = w->start.x - p->pos.x, dx_e = w->end.x - p->pos.x,
	dy_t = p->pos.y - w->height, dy_b = p->pos.y - 0, dz_s = w->start.y - p->pos.z, dz_e = w->end.y - p->pos.z;
	// add 90 degrees since we want to project onto the camera plane which is perpendicular to the player
	float cs = cos((p->angle + 90) * M_PI / 180), sn = sin((p->angle + 90) * M_PI / 180);
	float rot_xs, rot_xe, rot_zs, rot_ze;
	int_vec2 pt_s, pt_e;
	int_vec2 pb_s, pb_e;

	rot_xs = dx_s * cs + dz_s * sn;
	rot_xe = dx_e * cs + dz_e * sn;
	rot_zs = dz_s * cs - dx_s * sn;
	rot_ze = dz_e * cs - dx_e * sn;
	if (rot_zs >= 0 && rot_ze >= 0)
		return 1;
	if (rot_zs >= 0) {
		clip_wall(&rot_xs, &dy_t, &rot_zs, &(vec3){rot_xe, dy_t, rot_ze});
		clip_wall(&rot_xs, &dy_b, &rot_zs, &(vec3){rot_xe, dy_b, rot_ze});
	}
	if (rot_ze >= 0) {
		clip_wall(&rot_xe, &dy_b, &rot_ze, &(vec3){rot_xs, dy_b, rot_zs});
		clip_wall(&rot_xe, &dy_t, &rot_ze, &(vec3){rot_xs, dy_t, rot_zs});
	}

	pt_s.x = SCREEN_WIDTH / 2 + (200 * rot_xs / rot_zs);
	pt_s.y = SCREEN_HEIGHT / 2 + (200 * dy_t / rot_zs);
	pt_e.x = SCREEN_WIDTH / 2 + (200 * rot_xe / rot_ze);
	pt_e.y = SCREEN_HEIGHT / 2 + (200 * dy_t / rot_ze);

	pb_s.x = SCREEN_WIDTH / 2 + (200 * rot_xs / rot_zs);
	pb_s.y = SCREEN_HEIGHT / 2 + (200 * dy_b / rot_zs);
	pb_e.x = SCREEN_WIDTH / 2 + (200 * rot_xe / rot_ze);
	pb_e.y = SCREEN_HEIGHT / 2 + (200 * dy_b / rot_ze);


	fill_wall(pixels, &pt_s, &pt_e, &pb_s, &pb_e);
	
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
