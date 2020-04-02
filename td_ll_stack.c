#include <stdlib.h>
#include <assert.h>

#include "td_ll_stack.h"

#pragma mark - Creating a Stack

td_ll_stack* td_ll_stack_alloc()
{
    td_ll_stack *stack =
        (td_ll_stack *) malloc(sizeof(td_ll_stack));

    if (stack) {
        stack->top = NULL;
    }

    return stack;
}

td_ll_stack* td_ll_stack_init(td_ll_stack *stack)
{
    if (stack) {
        stack->top = NULL;
        stack->length = 0;
    }

    return stack;
}

#pragma mark - Deallocating a Stack

void td_ll_stack_free(td_ll_stack *stack)
{
    if (stack) {
        td_ll_stack_node *node = stack->top;
        while (node) {
            td_ll_stack_node *current_node =
                node;
            node =
                current_node->previous_node;

            free(current_node);
        }

        free(stack);
    }
}

void td_ll_stack_free_with_elements(td_ll_stack *stack)
{
    if (stack) {
        td_ll_stack_node *node = stack->top;
        while (node) {
            td_ll_stack_node *current_node =
                node;

            void *element = current_node->element;
            if (element) {
                free(element);
            }

            node =
                current_node->previous_node;

            free(current_node);
        }

        free(stack);
    }
}

void td_ll_stack_free_with_element_handler(
         td_ll_stack *stack,
         td_ll_stack_free_element_handler handler
     )
{
    if (stack) {
        td_ll_stack_node *node = stack->top;
        while (node) {
            td_ll_stack_node *current_node =
                node;

            handler(current_node->element);

            node =
                current_node->previous_node;

            free(current_node);
        }

        free(stack);
    }
}

#pragma mark - Checking Stack Size

int td_ll_stack_is_empty(td_ll_stack *stack)
{
    assert(stack);

    return stack->length == 0;
}

size_t td_ll_stack_length(td_ll_stack *stack)
{
    assert(stack);

    return stack->length;
}

#pragma mark - Adding/Accessing/Removing Elements

void td_ll_stack_push(td_ll_stack *stack, void *element)
{
    assert(stack);

    td_ll_stack_node *node =
        (td_ll_stack_node *) malloc(sizeof(*node));

    assert(node);

    node->element =
        element;
    node->previous_node =
        NULL;

    td_ll_stack_node *top = stack->top;
    if (top) {
        node->previous_node =
            top;
    }

    stack->top =
        node;
    ++stack->length;
}

void* td_ll_stack_top(td_ll_stack *stack)
{
    assert(stack);

    void *element = NULL;

    td_ll_stack_node *top;
    if ((top = stack->top)) {
        element =
            top->element;
    }

    return element;
}

void* td_ll_stack_pop(td_ll_stack *stack)
{
    assert(stack);

    void *element = NULL;

    td_ll_stack_node *top;
    if ((top = stack->top)) {
        element =
            top->element;

        stack->top =
            top->previous_node;
        --stack->length;

        free(top);
    }

    return element;
}

