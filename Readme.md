td\_stack
=========

Array and linked-list implementations of the stack data structure in the C
programming language

## Required Software

* GCC or Clang compilers to build the code
* GNU Make to automate the compilation process

## Building Tests and Sample Programs

Run the following command to build all the programs:

```bash
make
```

## Running Tests

```bash
./td_array_stack_test # Run unit tests for the array implementation of the stack.
./td_ll_stack_test # Run unit tests for the linked-list implementation of the stack.
```

## Running Sample Programs

```bash
# Run a sample program for each stack implementation that uses the stack to
# generate a 2-D maze that is 20 by 10 cells in size.
./td_array_stack_maze 20 10
./td_ll_stack_maze 20 10
```

## Cleanup

Run `make clean` to remove the generated object files and executables.

## Credits

**td\_stack** was created by Dmitrii Toskaitov <dmitrii@toksaitov.com>

