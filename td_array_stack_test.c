#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#include "td_array_stack.h"

void td_array_stack_test_alloc();
void td_array_stack_test_init();
void td_array_stack_test_free();
void td_array_stack_test_free_with_elements();
void td_array_stack_test_free_with_element_handler();
void td_array_stack_test_is_empty();
void td_array_stack_test_length();
void td_array_stack_test_push();
void td_array_stack_test_top();
void td_array_stack_test_pop();
void td_array_stack_test_compact();

int main(int argc, char **argv)
{
#define TD_MAIN_TEST(TEST_FUNCTION)      \
    do {                                 \
        puts(#TEST_FUNCTION);            \
        TEST_FUNCTION();                 \
        puts(#TEST_FUNCTION ": Passed"); \
    } while(0)

    TD_MAIN_TEST(td_array_stack_test_alloc);
    TD_MAIN_TEST(td_array_stack_test_init);
    TD_MAIN_TEST(td_array_stack_test_free);
    TD_MAIN_TEST(td_array_stack_test_free_with_elements);
    TD_MAIN_TEST(td_array_stack_test_free_with_element_handler);
    TD_MAIN_TEST(td_array_stack_test_is_empty);
    TD_MAIN_TEST(td_array_stack_test_length);
    TD_MAIN_TEST(td_array_stack_test_push);
    TD_MAIN_TEST(td_array_stack_test_top);
    TD_MAIN_TEST(td_array_stack_test_pop);
    TD_MAIN_TEST(td_array_stack_test_compact);

#undef TD_MAIN_TEST
}

void td_array_stack_test_alloc()
{
    td_array_stack *stack =
        td_array_stack_alloc();

    assert(stack);

    td_array_stack_free(stack);
}

void td_array_stack_test_init()
{
    td_array_stack *stack =
        td_array_stack_alloc();

    assert(stack);

    td_array_stack_init(stack);

    assert(stack);
    assert(stack->elements == NULL);
    assert(stack->capacity == 0);
    assert(stack->length   == 0);

    td_array_stack_free(stack);
}

void td_array_stack_test_free()
{
    td_array_stack *stack =
        td_array_stack_init(td_array_stack_alloc());

    assert(stack);

    td_array_stack_free(stack);
}

void td_array_stack_test_free_with_elements()
{
    td_array_stack *stack =
        td_array_stack_init(td_array_stack_alloc());

    assert(stack);

    int *element = (int *) malloc(sizeof(int));
    *element = 42;

    td_array_stack_push(stack, element);
    td_array_stack_free_with_elements(stack);
}

void td_array_stack_test_free_int_element_handler(
         void *element
     )
{
    if (element) {
        free(element);
    }
}

void td_array_stack_test_free_with_element_handler()
{
    td_array_stack *stack =
        td_array_stack_init(td_array_stack_alloc());

    assert(stack);

    int *element = (int *) malloc(sizeof(int));
    *element = 42;
    td_array_stack_push(stack, element);

    element = (int *) malloc(sizeof(int));
    *element = 8;
    td_array_stack_push(stack, element);

    element = (int *) malloc(sizeof(int));
    *element = 3;
    td_array_stack_push(stack, element);

    td_array_stack_free_with_element_handler(
        stack, td_array_stack_test_free_int_element_handler
    );
}

void td_array_stack_test_is_empty()
{
    td_array_stack *stack =
        td_array_stack_init(td_array_stack_alloc());

    assert(stack);

    int isEmpty = td_array_stack_is_empty(stack);
    assert(isEmpty);

    int *element = (int *) malloc(sizeof(int));
    *element = 42;
    td_array_stack_push(stack, element);

    element = (int *) malloc(sizeof(int));
    *element = 8;
    td_array_stack_push(stack, element);

    isEmpty = td_array_stack_is_empty(stack);
    assert(!isEmpty);

    element = (int *) td_array_stack_pop(stack);
    free(element);

    isEmpty = td_array_stack_is_empty(stack);
    assert(!isEmpty);

    element = (int *) td_array_stack_pop(stack);
    free(element);

    isEmpty = td_array_stack_is_empty(stack);
    assert(isEmpty);

    td_array_stack_free(stack);
}

void td_array_stack_test_length()
{
    td_array_stack *stack =
        td_array_stack_init(td_array_stack_alloc());

    assert(stack);

    size_t stack_length = td_array_stack_length(stack);
    assert(stack_length == 0);

    int *element = (int *) malloc(sizeof(int));
    *element = 42;
    td_array_stack_push(stack, element);

    stack_length = td_array_stack_length(stack);
    assert(stack_length == 1);

    element = (int *) malloc(sizeof(int));
    *element = 8;
    td_array_stack_push(stack, element);

    stack_length = td_array_stack_length(stack);
    assert(stack_length == 2);

    element = (int *) td_array_stack_pop(stack);
    free(element);

    stack_length = td_array_stack_length(stack);
    assert(stack_length == 1);

    element = (int *) td_array_stack_pop(stack);
    free(element);

    stack_length = td_array_stack_length(stack);
    assert(stack_length == 0);

    td_array_stack_free(stack);
}

void td_array_stack_test_push()
{
    const size_t test_range = 1000;

    td_array_stack *stack =
        td_array_stack_init(td_array_stack_alloc());

    assert(stack);

    for (size_t i = 0; i < test_range; ++i) {
        int *element =
            (int *) malloc(sizeof(int));

        *element =
            rand();

        td_array_stack_push(stack, element);
        assert(stack->length == i + 1);
    }

    td_array_stack_free_with_elements(stack);
}

void td_array_stack_test_top()
{
    td_array_stack *stack =
        td_array_stack_init(td_array_stack_alloc());

    assert(stack);

    int *element = (int *) td_array_stack_top(stack);
    assert(stack->length == 0);
    assert(!element);

    element = (int *) malloc(sizeof(int));
    *element = 42;
    td_array_stack_push(stack, element);

    element = (int *) td_array_stack_top(stack);
    assert(stack->length == 1);
    assert(element);

    element = (int *) malloc(sizeof(int));
    *element = 8;
    td_array_stack_push(stack, element);

    element = (int *) td_array_stack_top(stack);
    assert(stack->length == 2);
    assert(element);

    element = (int *) td_array_stack_pop(stack);
    free(element);

    element = (int *) td_array_stack_top(stack);
    assert(stack->length == 1);
    assert(element);

    element = (int *) td_array_stack_pop(stack);
    free(element);

    element = (int *) td_array_stack_top(stack);
    assert(stack->length == 0);
    assert(!element);

    td_array_stack_free(stack);
}

void td_array_stack_test_pop()
{
    const size_t test_range = 1000;

    td_array_stack *stack =
        td_array_stack_init(td_array_stack_alloc());

    assert(stack);

    for (size_t i = 0; i < test_range; ++i) {
        int *element =
            (int *) malloc(sizeof(int));

        *element =
            rand();

        td_array_stack_push(stack, element);
        assert(stack->length == i + 1);
    }

    for (size_t i = 0; i < test_range; ++i) {
        int *element =
            (int *) td_array_stack_pop(stack);

        assert(element);
        free(element);

        assert(stack->length == test_range - (i + 1));
    }

    td_array_stack_free(stack);
}

void td_array_stack_test_compact()
{
    const size_t test_range = 1000;

    td_array_stack *stack =
        td_array_stack_init(td_array_stack_alloc());

    assert(stack);

    assert(stack->capacity == 0);

    int *element = (int *) malloc(sizeof(int));
    *element = 42;
    td_array_stack_push(stack, element);

    assert(stack->capacity == 2);

    element = (int *) malloc(sizeof(int));
    *element = 8;
    td_array_stack_push(stack, element);

    assert(stack->capacity == 2);

    element = (int *) malloc(sizeof(int));
    *element = 3;
    td_array_stack_push(stack, element);

    assert(stack->capacity == 4);

    element = (int *) td_array_stack_pop(stack);
    free(element);

    assert(stack->capacity == 4);

    element = (int *) td_array_stack_pop(stack);
    free(element);

    assert(stack->capacity == 4);

    td_array_stack_compact(stack);

    assert(stack->capacity == 2);

    td_array_stack_free_with_elements(stack);
}

