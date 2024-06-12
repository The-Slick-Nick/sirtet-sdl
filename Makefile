# For my own reference: a makefile cheat sheet
# https://devhints.io/makefile
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

build: $(DEST)/$(TARGET) main.c
	$(CC) $(CFLAGS) -I$(INCL) main.c -o main.bin -L $(DEST) -l:lib.a

$(DEST)/$(TARGET): $(OBJS)
	@ar rvs $@ $^


$(OBJS): $(DEST)/%.o: $(INCL)/%.c
	@echo This task to build $@ from $<
	$(CC) $(CFLAGS) -I$(INCL) -c $< -o $@


# find .c files, make .o files



