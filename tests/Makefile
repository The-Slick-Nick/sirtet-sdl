

CC = gcc
CFLAGS = -Wall --std=gnu99

### Edit this to manually add testing .c files ###
### NOTE: I could just search for any ".c" file in this directory,
### but something about this feels nicer
TEST_SUITES = test_block.c test_coordinates.c
######


TEST_OBJS = $(patsubst %.c, %.o, $(TEST_SUITES))
TEST_BINS = $(patsubst %.c, %.bin, $(TEST_SUITES))

INCL = ../src  # Where to find headers
LIB = ../build # Where to find compiled library

all: run_tests clean


run_tests: build_tests  
	for x in *.bin; do ./$$x; done
	@echo "Tests Complete"

build_tests: $(TEST_BINS)

$(TEST_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -I $(INCL) -c $< -o $@

# Make and run
$(TEST_BINS): %.bin: %.o $(LIB)/lib.a
	$(CC) $(CFLAGS) -o $@ $< -L$(LIB) -l:libEWENIT.a -l:lib.a


$(LIB)/lib.a:
	@make build_lib -C ..



clean:
	@rm -f *.o
	@rm -f *.bin
	@rm -f *.a

