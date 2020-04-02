# flags

CFLAGS=-g

# default

.PHONY: all
all : td_array_stack_maze td_ll_stack_maze \
      td_array_stack_test td_ll_stack_test

td_array_stack.o : td_array_stack.c

td_ll_stack.o : td_ll_stack.c

# maze

maze : td_array_stack_maze, td_ll_stack_maze

# td_array_stack_maze

td_array_stack_maze : td_array_stack_maze.o td_array_stack.o

td_array_stack_maze.o : td_array_stack_maze.c

# td_ll_stack_maze

td_ll_stack_maze : td_ll_stack_maze.o td_ll_stack.o

td_ll_stack_maze.o : td_ll_stack_maze.c

# test

test : td_array_stack_test td_ll_stack_test

# td_array_stack_test

td_array_stack_test : td_array_stack_test.o td_array_stack.o

td_array_stack_test.o : td_array_stack_test.c

# td_ll_stack_test

td_ll_stack_test : td_ll_stack_test.o td_ll_stack.o

td_ll_stack_test.o : td_ll_stack_test.c

# clean

.PHONY: clean
clean :
	rm -rf td_array_stack.o td_ll_stack.o            \
	       td_array_stack_maze td_array_stack_maze.o \
	       td_ll_stack_maze td_ll_stack_maze.o       \
	       td_array_stack_test td_array_stack_test.o \
	       td_ll_stack_test td_ll_stack_test.o

