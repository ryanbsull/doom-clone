#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

enum dir {
	LEFT = 2,
	RIGHT = -2,
	FWD = 1,
	BACK = -1,
};

typedef uint32_t 	u32;
typedef uint8_t 	u8;

typedef struct {
	float x,y;
} vec2;

typedef struct {
	int x,y;
} int_vec2;

typedef struct {
	float x,y,z;
} vec3;

typedef struct {
	int x,y,z;
} int_vec3;

#endif
