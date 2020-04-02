#ifndef TD_ARRAY_STACK_H
#define TD_ARRAY_STACK_H

#include <stddef.h>

#pragma mark - Data Types

typedef struct _td_array_stack
{
    void **elements;
    size_t capacity;
    size_t length;
} td_array_stack;

typedef void (*td_array_stack_free_element_handler)(void *element);

#pragma mark - Creating a Stack

td_array_stack* td_array_stack_alloc();
td_array_stack* td_array_stack_init(td_array_stack *stack);

#pragma mark - Deallocating a Stack

void td_array_stack_free(td_array_stack *stack);
void td_array_stack_free_with_elements(td_array_stack *stack);
void td_array_stack_free_with_element_handler(
         td_array_stack *stack,
         td_array_stack_free_element_handler handler
     );

#pragma mark - Checking Stack Size

int td_array_stack_is_empty(td_array_stack *stack);
size_t td_array_stack_length(td_array_stack *stack);

#pragma mark - Adding/Accessing/Removing Elements

void td_array_stack_push(td_array_stack *stack, void *element);
void* td_array_stack_top(td_array_stack *stack);
void* td_array_stack_pop(td_array_stack *stack);

#pragma mark - Managing Stack Memory

void td_array_stack_compact(td_array_stack *stack);

#endif

