############################################################
# Edit this to manually add testing .c files ###
# NOTE: I could just search for any ".c" file in this directory,
# but something about this feels nicer

TEST_SUITES = $(shell find . -name 'test*.c')
############################################################


# .o file equivalents of defined test suites
TEST_OBJS = $(patsubst %.c, %.o, $(TEST_SUITES))

# .bin binary file equivalents of defined test suites
TEST_BINS = $(patsubst %.c, %.bin, $(TEST_SUITES))

# Root folder to recursively find headers from
INCL = ../src

# Folder for compiled/linked results
LIB = ../build

SRC_DIRS = ../src

# .d "dependency" type file compiled from source files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)

# compiler include flags to use for compilation
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Compiler to use
CC = gcc

# Compile with -MMD and -MP flags to identify header
# dependencies and generate them as -d files
CFLAGS = -Wall -g --std=gnu99 $(INC_FLAGS) -MMD -MP

all: run_tests clean

run_tests: build_src build_tests  
	mkdir -p logs
	for x in *.bin; do \
		valgrind \
		-s --tool=memcheck --leak-check=full --track-origins=yes --show-reachable=yes \
		--log-file=logs/$$x.log \
		./$$x; \
	done
	@echo "Tests Complete"

build_src:
	@make build_lib -C ..

build_tests: $(TEST_BINS)


# For each test suite, .c -> .o
$(TEST_OBJS): %.o: %.c
	@echo $(TEST_SUITES)
	$(CC) $(CFLAGS) -c $< -o $@

# Link each suite's .o file to lib.a (compiled from project root)
# to create each suite's binary
$(TEST_BINS): %.bin: %.o $(LIB)/lib.a
	@echo $(TEST_SUITES)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIB) -l:libEWENIT.a -l:lib.a -lSDL2 -l SDL2_ttf


# NOTE: Relies on the makefile in project root having a "make build_lib"
# task that generates "lib.a" in a build folder (lib.a should link all
# object files needed by project)
$(LIB)/lib.a:
	@make build_lib -C ..

clean:
	@rm -f *.o
	@rm -f *.bin
	@rm -f *.a



# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)

