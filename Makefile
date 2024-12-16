CC = gcc
CFLAGS = -lm -lSDL2 -g
SRC_FILES := $(wildcard src/*.c)
OUTPUT_FILES := $(patsubst src/%.c,build/%,$(SRC_FILES))

all: $(OUTPUT_FILES)

build/%: src/%.c
	@if [ ! -d "build" ]; then mkdir build; fi
	$(CC) $(CFLAGS) $^ -o $@ 

clean:
	rm -rf build/

