# For my own reference: a makefile cheat sheet
# https://devhints.io/makefile
#
#
# NOTE: A standard to follow here (that my tests and sandbox folder rely on)
# is making "build_lib" a standardized task that generates a lib.a file in
# 'build' folder.
# (These other makefiles I use call "make build" on the root project directory
#  and link the resulting library file (lib.a))
#
CC = gcc
CFLAGS = -Wall --std=gnu99
INCL = src
DEST = build
TARGET = lib.a

SOURCES ?= $(wildcard $(INCL)/*.c)
OBJS ?= $(patsubst $(INCL)%.c, $(DEST)%.o, $(SOURCES))

all: build

run: build
	./main.bin

clean:
	rm -f *.bin

build_lib: $(DEST)/$(TARGET)

build: build_lib main.c
	$(CC) $(CFLAGS) -I$(INCL) main.c -o main.bin -L $(DEST) -l:lib.a -lSDL2

$(DEST)/$(TARGET): $(OBJS)
	@ar rvs $@ $^


$(OBJS): $(DEST)/%.o: $(INCL)/%.c
	@echo This task to build $@ from $<
	$(CC) $(CFLAGS) -I$(INCL) -c $< -o $@


# find .c files, make .o files



