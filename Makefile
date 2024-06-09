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

build: $(DEST)/$(TARGET)

$(DEST)/$(TARGET): $(OBJS)
	@ar rvs $@ $^


$(OBJS): $(DEST)/%.o: $(INCL)/%.c
	@echo This task to build $@ from $<
	$(CC) $(CFLAGS) -I$(INCL) -c $< -o $@


# find .c files, make .o files



