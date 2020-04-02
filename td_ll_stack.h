#ifndef TD_LL_STACK_H
#define TD_LL_STACK_H

#include <stddef.h>

#pragma mark - Data Types

typedef struct _td_ll_stack_node
{
    void *element;
    struct _td_ll_stack_node *previous_node;
} td_ll_stack_node;

typedef struct _td_ll_stack
{
    td_ll_stack_node *top;
    size_t length;
} td_ll_stack;

typedef void (*td_ll_stack_free_element_handler)(void *element);

#pragma mark - Creating a Stack

td_ll_stack* td_ll_stack_alloc();
td_ll_stack* td_ll_stack_init(td_ll_stack *stack);

#pragma mark - Deallocating a Stack

void td_ll_stack_free(td_ll_stack *stack);
void td_ll_stack_free_with_elements(td_ll_stack *stack);
void td_ll_stack_free_with_element_handler(
         td_ll_stack *stack,
         td_ll_stack_free_element_handler handler
     );

#pragma mark - Checking Stack Size

int td_ll_stack_is_empty(td_ll_stack *stack);
size_t td_ll_stack_length(td_ll_stack *stack);

#pragma mark - Adding/Accessing/Removing Elements

void td_ll_stack_push(td_ll_stack *stack, void *element);
void* td_ll_stack_top(td_ll_stack *stack);
void* td_ll_stack_pop(td_ll_stack *stack);

#endif

