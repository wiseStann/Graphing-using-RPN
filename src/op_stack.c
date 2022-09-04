#include "op_stack.h"
#include <stdlib.h>

op_node *new_op_stack_node(token *operator) {
  op_node *node = (op_node *)malloc(sizeof(struct OperatorNode));
  node->next = NULL;
  node->operator= operator;
  return node;
}

op_stack *new_op_stack() {
  op_stack *stack = (op_stack *)malloc(sizeof(struct OperatorStack));
  stack->head = NULL;
  stack->size = 0;
  return stack;
}

void op_stack_push(op_stack *stack, token *operator) {
  if (stack->head == NULL) {
    stack->head = new_op_stack_node(operator);
  } else {
    op_node *curr = stack->head;
    while (curr->next) {
      curr = curr->next;
    }
    curr->next = new_op_stack_node(operator);
  }
  stack->size++;
}

op_node *op_stack_pop(op_stack *stack) {
  op_node *popped = NULL;
  if (stack->head != NULL) {
    op_node *curr = stack->head;
    if (stack->size == 1) {
      op_node *tmp = curr;
      stack->head = tmp->next;
      popped = tmp;
    } else {
      while (curr->next->next) {
        curr = curr->next;
      }
      op_node *tmp = curr->next;
      curr->next = NULL;
      popped = tmp;
    }
    stack->size--;
  }
  return popped;
}

token *stack_get_tail(op_stack *stack) {
  token *popped = NULL;
  if (stack->head == NULL) {
    return NULL;
  } else {
    op_node *curr = stack->head;
    while (curr->next) {
      curr = curr->next;
    }
    return curr->operator;
  }
  return popped;
}

void op_stack_free(op_stack *stack) {
  while (stack->head) {
    op_node *tmp = stack->head->next;
    free(stack->head);
    stack->head = tmp;
  }
  free(stack);
}
