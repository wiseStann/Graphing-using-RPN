#ifndef SRC_OP_STACK_H_
#define SRC_OP_STACK_H_

#include "token.h"

typedef struct OperatorNode {
  struct OperatorNode *next;
  token *operator;
} op_node;

op_node *new_op_stack_node(token *operator);

typedef struct OperatorStack {
  op_node *head;
  unsigned size;
} op_stack;

op_stack *new_op_stack();

void op_stack_push(op_stack *stack, token *operator);

op_node *op_stack_pop(op_stack *stack);

token *stack_get_tail(op_stack *stack);

void op_stack_free(op_stack *stack);

#endif // SRC_OP_STACK_H_
