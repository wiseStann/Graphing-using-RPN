#include "RPN.h"
#include "token.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

token **convert_to_RPN(token **tokens, int tokens_size,
                       int *result_tokens_size) {
  token **output = (token **)malloc(tokens_size * sizeof(struct Token));
  int idx = 0;
  op_stack *stack = new_op_stack();
  for (int i = 0; i < tokens_size; i++) {
    token *curr = tokens[i];
    if (curr->kind == TOKEN_NUMBER || curr->kind == TOKEN_VARIABLE_NAME) {
      output[idx++] = new_token(curr->str_token, curr->text_pos, curr->kind);
    } else if (curr->kind == TOKEN_FUNCTION_NAME && curr->str_token) {
      convert_to_RPN_function(output, tokens, &i, &idx);
    } else {
      convert_to_RPN_operator(output, curr, stack, &idx);
    }
  }
  while (stack->size > 0) {
    op_node *tail_node = op_stack_pop(stack);
    token *tail = tail_node->operator;
    free(tail_node);
    output[idx++] = new_token(tail->str_token, tail->text_pos, tail->kind);
  }
  op_stack_free(stack);
  *result_tokens_size = idx;
  return output;
}

void convert_to_RPN_function(token **output, token **tokens, int *i, int *idx) {
  // if we have function like sin(cos(x + 1) - tan(x*x)), we must place all the
  // tokens to the output in that given order Example: function -> sin(x),
  // output: 'sin', '(', 'x', ')'

  // sin(cos(x + 1) - tan(x * x)) -> sin ( cos ( x 1 + ) tan ( x x * ) - )

  op_stack *stack = new_op_stack();

  // skip function name
  token *curr = tokens[*i];
  token *function =
      new_token(curr->str_token, curr->text_pos, TOKEN_FUNCTION_NAME);
  output[(*idx)++] = function;
  (*i)++;

  op_stack *brackets = new_op_stack();
  do {
    curr = tokens[*i];
    token *next_token = new_token(curr->str_token, curr->text_pos, curr->kind);
    if (curr->kind == TOKEN_OPEN_BRACKET) {
      op_stack_push(brackets, curr);
      output[(*idx)++] = next_token;
      convert_to_RPN_operator(output, curr, stack, idx);
    } else if (curr->kind == TOKEN_CLOSE_BRACKET) {
      op_stack_pop(brackets);
      convert_to_RPN_operator(output, curr, stack, idx);
      output[(*idx)++] = next_token;
    } else if (curr->kind == TOKEN_NUMBER ||
               curr->kind == TOKEN_VARIABLE_NAME ||
               curr->kind == TOKEN_FUNCTION_NAME) {
      output[(*idx)++] = next_token;
    } else {
      convert_to_RPN_operator(output, curr, stack, idx);
    }
    (*i)++;
  } while (curr->kind != TOKEN_CLOSE_BRACKET ||
           stack_get_tail(brackets) != NULL);
  (*i)--;
  op_stack_free(stack);
}

void convert_to_RPN_operator(token **output, token *curr, op_stack *stack,
                             int *idx) {
  if (curr->kind == TOKEN_OPEN_BRACKET) {
    op_stack_push(stack, curr);
  } else if (curr->kind == TOKEN_CLOSE_BRACKET) {
    op_node *tail = NULL;
    while (stack->size > 0 &&
           (tail = op_stack_pop(stack))->operator->kind != TOKEN_OPEN_BRACKET) {
      output[*idx] = new_token(tail->operator->str_token,
                               tail->operator->text_pos, tail->operator->kind);
      (*idx)++;
      free(tail);
    }
  } else {
    int precedence = curr->operator.precedence;
    token *tmp_tail = stack_get_tail(stack);
    if (tmp_tail && tmp_tail->kind != TOKEN_OPEN_BRACKET && stack->size > 0) {
      if ((tmp_tail->kind == TOKEN_UN_MINUS ||
           precedence <= tmp_tail->operator.precedence) &&
          (tmp_tail->kind != TOKEN_BIN_MINUS ||
           tmp_tail->kind != TOKEN_BIN_MINUS) &&
          curr->kind != TOKEN_UN_MINUS) {
        op_node *tail = op_stack_pop(stack); // pop stack
        output[*idx] =
            new_token(tail->operator->str_token, tail->operator->text_pos,
                      tail->operator->kind);
        free(tail);
        (*idx)++;
      }
    }
    op_stack_push(stack, curr); // push curr to stack
  }
}

double evaluate_RPN(token **tokens, int tokens_size, int *i, double x,
                    int *flag, int single_func, int base_func) {
  op_stack *stack = new_op_stack();
  for (; *i < tokens_size && *flag; (*i)++) {
    token *curr = tokens[*i];
    if (curr->kind == TOKEN_NUMBER) {
      op_stack_push(stack, curr);
    } else if (curr->kind == TOKEN_VARIABLE_NAME) {
      curr->number = x;
      op_stack_push(stack, curr);
    } else if (curr->kind == TOKEN_FUNCTION_NAME) {
      curr->number = evaluate_RPN_complex_function(tokens, tokens_size, i, x,
                                                   flag, base_func);
      op_stack_push(stack, curr);
      if (single_func)
        break;
    } else if (curr->kind == TOKEN_CLOSE_BRACKET) {  // for function
      if (!base_func)
        *flag = 0;
      (*i) -= 2;
    } else {
      evaluate_RPN_operator(curr, stack, flag);
    }
  }
  return get_evaluated_RPN(stack);
}

double get_evaluated_RPN(op_stack *stack) {
  op_node *tail = op_stack_pop(stack);
  double result;
  if (tail == NULL)
    result = 0;
  else
    result = tail->operator->number;
  free(tail);
  op_stack_free(stack);
  return result;
}

double evaluate_RPN_complex_function(token **tokens, int tokens_size, int *i,
                                     double x, int *flag, int base_func) {
  token *base_function = tokens[(*i)++];
  token *curr = NULL;
  int _flag;
  op_stack *stack = new_op_stack();
  op_stack *brackets = new_op_stack();
  do {
    _flag = 1;
    curr = tokens[*i];
    if (curr->kind == TOKEN_OPEN_BRACKET) {
      op_stack_push(brackets, curr);
    } else if (curr->kind == TOKEN_CLOSE_BRACKET) {
      op_stack_pop(brackets);
    } else if (curr->kind == TOKEN_NUMBER) {
      op_stack_push(stack, curr);
    } else if (curr->kind == TOKEN_FUNCTION_NAME ||
               curr->kind == TOKEN_VARIABLE_NAME) {
      double func_expr = evaluate_RPN(tokens, tokens_size, i, x, &_flag, 1, 0);
      if (curr->kind == TOKEN_FUNCTION_NAME) {
        if (base_func != 1)
          evaluate_RPN_function(curr, func_expr);
        (*i)--;
      } else {
        curr->number = func_expr;
      }
      op_stack_push(stack, curr);
    } else {
      evaluate_RPN_operator(curr, stack, flag);
    }
    (*i)++;
  } while (stack_get_tail(brackets) != NULL && *i < tokens_size);
    
  if (base_func)
    (*i)--;

  double entire_func_expr = get_evaluated_RPN(stack);
  evaluate_RPN_function(base_function, entire_func_expr);
  return base_function->number;
}

void evaluate_RPN_function(token *curr, double x) {
  switch (curr->func_id) {
  case 1:
    curr->number = sin(x);
    break;
  case 2:
    curr->number = cos(x);
    break;
  case 3:
    curr->number = tan(x);
    break;
  case 4:
    curr->number = cos(x) / sin(x);
    break;
  case 5:
    curr->number = log(x);
    break;
  case 6:
    curr->number = sqrt(x);
    break;
  default:
    fprintf(stderr, "Unknown function given in [evaluate_RPN_function]\n");
    break;
  }
}

void evaluate_RPN_operator(token *curr, op_stack *stack, int *flag) {
  double result = 0;
  op_node *operand2_node = NULL, *operand1_node = NULL;
  if (curr->kind == TOKEN_UN_MINUS) {
    operand2_node = op_stack_pop(stack);
  } else {
    operand2_node = op_stack_pop(stack);
    operand1_node = op_stack_pop(stack);
  }
  // operand2_node is always non-NULL
  token *operand2 = operand2_node->operator;
  token *operand1 = (operand1_node) ? operand1_node->operator: NULL;

  free(operand2_node);
  free(operand1_node);
  switch (curr->kind) {
  case TOKEN_BIN_PLUS:
    result = operand1->number + operand2->number;
    break;
  case TOKEN_UN_MINUS:
    result = -operand2->number;
    break;
  case TOKEN_BIN_MINUS:
    result = operand1->number - operand2->number;
    break;
  case TOKEN_RSLASH: {
    if (fabs((double)operand2->number) < EPS) {
      *flag = 0;
    } else {
      result = operand1->number / operand2->number;
    }
    break;
  }
  case TOKEN_STAR:
    result = operand1->number * operand2->number;
    break;
  default:
    break;
  }
  if (*flag) {
    curr->number = result;
    op_stack_push(stack, curr);
  }
}
