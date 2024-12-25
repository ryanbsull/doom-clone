#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

enum dir {
	LEFT = -1,
	RIGHT = 1,
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

#endif
