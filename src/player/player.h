#ifndef PLAYER_H
#define PLAYER_H

#define MVMT_SPEED    10
#define ANGULAR_SPEED 5 

enum dir {
	LEFT = -1,
	RIGHT = 1,
	FWD = 1,
	BACK = -1,
};

typedef struct {
	float x,y;
} vec2;

typedef struct {
	int x,y;
} int_vec2;

typedef struct {
	vec2 pos;
	vec2 dir;
	vec2 cam;
} player;

int move();
int turn();

#endif
