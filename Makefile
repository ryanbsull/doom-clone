CC = gcc
CFLAGS = -lm -lSDL2_image -lSDL2 -g
SRC_FILES := $(wildcard src/*.c)
OUTPUT_FILES := $(patsubst src/%.c,build/%.o,$(SRC_FILES))

all: doom

doom: $(OUTPUT_FILES) game.c
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: src/%.c
	@if [ ! -d "build" ]; then mkdir build; fi
	$(CC) -g -c $^ -o $@ 

clean:
	rm -rf build/ doom*

