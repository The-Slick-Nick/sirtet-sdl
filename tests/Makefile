

CC = gcc
CFLAGS = -Wall --std=gnu99

TEST_SUITES = test_block.c
TEST_OBJS = $(patsubst %.c, %.o, $(TEST_SUITES))
TEST_BINS = $(patsubst %.c, %_bin, $(TEST_SUITES))

INCL = ../src  # Where to find headers
LIB = ../build # Where to find compiled library

all: run_tests clean


run_tests: $(TEST_BINS)


$(TEST_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -I $(INCL) -c $< -o $@

# Make and run
$(TEST_BINS): %_bin: %.o $(LIB)/lib.a
	$(CC) $(CFLAGS) -o $@ $< -L$(LIB) -l:libEWENIT.a -l:lib.a
	./$@


$(LIB)/lib.a:
	@make -C ..



clean:
	@rm -f *.o
	@rm -f *_bin
	@rm -f *.a

