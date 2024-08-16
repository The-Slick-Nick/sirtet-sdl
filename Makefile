# Thanks to http://makefiletutorial.com/#makefile-cookbook
# (and, by extension,
#  Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
# ) for this template
#
# NOTE: To make proper use, use one of commands:
# > make all
#	Builds library file, binary file, runs binary, then deletes
# > make_exe
#	Builds library and binary file
# > make_lib
#	Builds library file


###############################################################################
# Files and directories
###############################################################################
BUILD_DIR := ./build
SRC_DIR := ./src

TARGET_EXEC := final_program


SRCS := $(wildcard $(SRC_DIR)/**/*.c)
SRCS += $(wildcard $(SRC_DIR)/*.c)

OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

INC_DIRS := $(sort $(dir $(wildcard $(SRC_DIR)/*/)))
INC_DIRS += $(SRC_DIR)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

SDL_MODULES := SDL2 SDL2_ttf SDL2_mixer
SDL_FLAGS := $(addprefix -l,$(SDL_MODULES))

EXE_FILE := main.bin
LIB_FILE := $(BUILD_DIR)/libsirtet.a

COMPILER := gcc
COMP_FLAGS := -Wall -Werror -g

###############################################################################
# Makefile entry points
###############################################################################


run_exe: build_exe
	./main.bin


build_exe: ./main.bin

build_lib: $(LIB_FILE)

run_profile_summary: build_exe
	mkdir -p logs
	valgrind -s --tool=memcheck --leak-check=summary --track-origins=yes --show-reachable=yes --log-file=logs/valgrind.log ./main.bin


run_profile_full: build_exe
	mkdir -p logs
	valgrind -s --tool=memcheck --leak-check=full --track-origins=yes --show-reachable=yes --log-file=logs/valgrind.log ./main.bin

clean:
	rm main.bin
	rm -rf $(BUILD_DIR)/*

###############################################################################
# Builds
###############################################################################

$(EXE_FILE): main.c $(LIB_FILE)
	$(COMPILER) $(COMP_FLAGS) main.c -o $@ $(INC_FLAGS) -L$(BUILD_DIR) -lsirtet $(SDL_FLAGS)


# static library
$(LIB_FILE): $(OBJS)
	ar rcs $@ $^

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(COMPILER) $(COMP_FLAGS) -c $< -o $@ $(INC_FLAGS) -L$(BUILD_DIR) $(SDL_FLAGS)


# $(BUILD_DIR)/lib.a: $(OBJS)
# 	ar rvs $@ $^
#
# ./main.bin: $(BUILD_DIR)/lib.a
# 	gcc -Wall main.c -g -o main.bin $(INC_FLAGS) -L $(BUILD_DIR) -l:lib.a $(SDL_FLAGS)
#
