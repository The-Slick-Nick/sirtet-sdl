

CC = gcc
CFLAGS = -Wall --std=gnu99

TEST_SUITES = test_block.c
TEST_OBJS = $(patsubst %.c, %.o, $(TEST_SUITES))
TEST_BINS = $(patsubst %.c, %_bin, $(TEST_SUITES))
INCL = ../src


all: $(TEST_BINS) clean


$(TEST_OBJS): %.o: %.c
	@$(CC) $(CFLAGS) -I $(INCL) -c $< -o $@

# Make, run, remove
$(TEST_BINS): %_bin: %.o
	@$(CC) $(CFLAGS) -o $@ $< -l:libEWENIT.a
	@./$@


clean:
	@rm *.o
	@rm *_bin

