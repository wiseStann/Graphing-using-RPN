#ifndef SRC_RPN_H_
#define SRC_RPN_H_

#include "op_stack.h"
#include "token.h"

#define EPS 1e-6

token **convert_to_RPN(const token **tokens, int tokens_size,
                       int *result_tokens_size);

void convert_to_RPN_operator(token **output, token *curr, op_stack *stack,
                             int *idx);

double evaluate_RPN(token **tokens, int tokens_size, double x, int *flag);

void evaluate_RPN_function(token *current, double x);

void evaluate_RPN_operator(token *current, op_stack *stack, int *flag);

#endif // SRC_RPN_H_
