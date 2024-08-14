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

all: build_exe run_exe clean

#############################
### Files and directories ###
#############################
TARGET_EXEC := final_program

BUILD_DIR := ./build
SRC_DIRS := ./src

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Prepends BUILD_DIR and appends .o to every src file
# As an example, ./your_dir/hello.cpp turns into ./build/./your_dir/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# All SDL libraries/modules used
SDL_MODULES := SDL2 SDL2_ttf SDL2_mixer
# Lib flags for each SDL module
SDL_FLAGS := $(addprefix -l,$(SDL_MODULES))



# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
# Note to self: look in /usr/include for proper lib names
CPPFLAGS := -Wall $(INC_FLAGS) -g -MMD -MP $(SDL_FLAGS)

######################
### Build Commands ###
######################

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


$(BUILD_DIR)/lib.a: $(OBJS)
	ar rvs $@ $^	

./main.bin: $(BUILD_DIR)/lib.a
	gcc -Wall main.c -g -o main.bin $(INC_FLAGS) -L $(BUILD_DIR) -l:lib.a $(SDL_FLAGS)

##################
### HIGH LEVEL ###
##################

build_exe: ./main.bin

build_lib: $(BUILD_DIR)/lib.a

run_exe: build_exe
	./main.bin

run_profile_summary: build_exe
	mkdir -p logs
	valgrind -s --tool=memcheck --leak-check=summary --track-origins=yes --show-reachable=yes --log-file=logs/valgrind.log ./main.bin


run_profile_full: build_exe
	mkdir -p logs
	valgrind -s --tool=memcheck --leak-check=full --track-origins=yes --show-reachable=yes --log-file=logs/valgrind.log ./main.bin

clean:
	rm main.bin


# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
