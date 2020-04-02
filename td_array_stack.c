#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "td_array_stack.h"

#pragma mark - Stack Constants

static const size_t Stack_Capacity_Scale = 2;

#pragma mark - Creating a Stack

td_array_stack* td_array_stack_alloc()
{
    td_array_stack *stack =
        (td_array_stack *) malloc(sizeof(td_array_stack));

    return stack;
}

td_array_stack* td_array_stack_init(td_array_stack *stack)
{
    if (stack) {
        stack->elements = NULL;
        stack->capacity = 0;
        stack->length   = 0;
    }

    return stack;
}

#pragma mark - Deallocating a Stack

void td_array_stack_free(td_array_stack *stack)
{
    if (stack) {
        free(stack);
    }
}

void td_array_stack_free_with_elements(td_array_stack *stack)
{
    if (stack) {
        if (stack->elements) {
            for (size_t i = 0; i < stack->length; ++i) {
                void *element = stack->elements[i];
                if (element) {
                    free(element);
                }
            }
        }

        free(stack);
    }
}

void td_array_stack_free_with_element_handler(
         td_array_stack *stack,
         td_array_stack_free_element_handler handler
     )
{
    if (stack) {
        if (stack->elements) {
            for (size_t i = 0; i < stack->length; ++i) {
                handler(stack->elements[i]);
            }
        }

        free(stack);
    }
}

#pragma mark - Checking Stack Size

int td_array_stack_is_empty(td_array_stack *stack)
{
    assert(stack);

    return stack->length == 0;
}

size_t td_array_stack_length(td_array_stack *stack)
{
    assert(stack);

    return stack->length;
}

#pragma mark - Adding/Accessing/Removing Elements

void td_array_stack_push(td_array_stack *stack, void *element)
{
    assert(stack);

    if (stack->length + 1 > stack->capacity) {
        size_t current_capacity =
            stack->capacity;

        size_t new_capacity =
            current_capacity == 0 ?
                1 : current_capacity;

        new_capacity *= Stack_Capacity_Scale;
        if (new_capacity <= current_capacity) {
            new_capacity = SIZE_MAX;
        }

        assert(new_capacity != current_capacity);

        void *elements =
            malloc(sizeof(td_array_stack) * new_capacity);
        assert(elements);

        if (stack->elements) {
            for (size_t i = 0; i < stack->length; ++i) {
                ((char **) elements)[i] =
                    stack->elements[i];
            }

            free(stack->elements);
        }

        stack->elements =
            elements;
        stack->capacity =
            new_capacity;
    }

    stack->elements[stack->length] =
        element;
    ++stack->length;
}

void* td_array_stack_top(td_array_stack *stack)
{
    assert(stack);

    void *element = NULL;
    if (stack->length > 0) {
        assert(stack->elements);
        element =
            stack->elements[stack->length - 1];
    }

    return element;
}

void* td_array_stack_pop(td_array_stack *stack)
{
    assert(stack);

    void *element = NULL;
    if (stack->length > 0) {
        assert(stack->elements);
        element =
            stack->elements[--stack->length];
    }

    return element;
}

#pragma mark - Managing Stack Memory

void td_array_stack_compact(td_array_stack *stack)
{
    assert(stack);

    size_t length =
        stack->length;
    length =
        length == 0 ?
            1 : length;

    size_t new_capacity = length * Stack_Capacity_Scale;
    if (new_capacity > length &&
            new_capacity < stack->capacity) {
        void *elements =
            malloc(sizeof(td_array_stack) * new_capacity);
        assert(elements);

        if (stack->elements) {
            for (size_t i = 0; i < stack->length; ++i) {
                ((char **) elements)[i] =
                    stack->elements[i];
            }

            free(stack->elements);
        }

        stack->elements =
            elements;
        stack->capacity =
            new_capacity;
    }
}

