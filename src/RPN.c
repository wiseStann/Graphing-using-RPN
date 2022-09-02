#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "RPN.h"
#include "token.h"


token **convert_to_RPN(const token **tokens, int tokens_size, int *result_tokens_size) {
    token **output = (token**)malloc(tokens_size * sizeof(struct Token));
    int idx = 0;
    op_stack *stack = new_op_stack();
    for (int i = 0; i < tokens_size; i++) {
        token *curr = ((token**)tokens)[i];
        if (curr->kind == TOKEN_NUMBER || curr->kind == TOKEN_VARIABLE_NAME) {
            output[idx] = curr;
            idx++;
        } else if (curr->kind == TOKEN_FUNCTION_NAME && curr->str_token) {
            // function -> sin(x), 'sin', '(', 'x', ')'
            char function[128] = {};
            i++;  // skip function name
            strcat(function, curr->str_token);
            token *open_bracket = ((token**)tokens)[i++];
            strcat(function, open_bracket->str_token);
            token *next = NULL;
            while ((next = ((token**)tokens)[i]) != NULL && next->kind != TOKEN_CLOSE_BRACKET) {
                strcat(function, next->str_token);
                i++;
            }
            if (next)
                strcat(function, next->str_token);
            i++;  // skip close bracket
            token *function_token = new_token(function, TOKEN_FUNCTION_NAME);
            output[idx] = function_token;
        } else {
            convert_to_RPN_operator(output, curr, stack, &idx);
        }
    }
    while (stack->size > 0) {
        token *tail = op_stack_pop(stack);
        output[idx] = tail;
        idx++;
    }
    op_stack_free(stack);
    *result_tokens_size = idx;
    return output;
}

void convert_to_RPN_operator(token **output, token *curr, op_stack *stack, int *idx) {
    if (curr->kind == TOKEN_OPEN_BRACKET) {
        op_stack_push(stack, curr);
    } else if (curr->kind == TOKEN_CLOSE_BRACKET) {
        token *tail = NULL;
        while (stack->size > 0 && (tail = op_stack_pop(stack))->kind != TOKEN_OPEN_BRACKET) {
            output[*idx] = tail;
            (*idx)++;
        }
    } else {
        int precedence = curr->operator.precedence;
        token *tmp_tail = stack_get_tail(stack);
        if (tmp_tail && tmp_tail->kind != TOKEN_OPEN_BRACKET && stack->size > 0) {
            if (tmp_tail->kind == TOKEN_UN_MINUS || precedence <= tmp_tail->operator.precedence) {
                token *tail = op_stack_pop(stack);  // pop stack
                output[*idx] = tail;
                (*idx)++;
            }
        }
        op_stack_push(stack, curr);  // push curr to stack
    }
}

double evaluate_RPN(token **tokens, int tokens_size, double x, int *flag) {
    op_stack *stack = new_op_stack();
    for (int i = 0; i < tokens_size && *flag; i++) {
        token *current = tokens[i];
        if (current->kind == TOKEN_NUMBER) {
            op_stack_push(stack, current);
        } else if (current->kind == TOKEN_VARIABLE_NAME) {
            current->number = x;
            op_stack_push(stack, current);
        } else if (current->kind == TOKEN_FUNCTION_NAME) {
            evaluate_RPN_function(current, x);
        } else {
            evaluate_RPN_operator(current, stack, flag);
        }
    }
    token *tail = op_stack_pop(stack);
    double result;
    if (tail == NULL)
        result = 0;
    else
        result = tail->number;
    op_stack_free(stack);
    return result;
}

void evaluate_RPN_function(token *current, double x) {
    switch (current->func_id) {
        case 1:
            current->number = sin(x);
            break;
        case 2:
            current->number = cos(x);
            break;
        case 3:
            current->number = tan(x);
            break;
        case 4:
            current->number = cos(x) / sin(x);
            break;
        case 5:
            current->number = log(x);
            break;
        case 6:
            current->number = sqrt(x);
            break;
    }
}

void evaluate_RPN_operator(token *current, op_stack *stack, int *flag) {
    double result = 0;
    double operand2 = op_stack_pop(stack)->number;
    double operand1 = op_stack_pop(stack)->number;
    switch (current->kind) {
        case TOKEN_BIN_PLUS:
            result = operand1 + operand2;
            break;
        case TOKEN_BIN_MINUS:
            result = operand1 - operand2;
            break;
        case TOKEN_RSLASH: {
            if (fabs(operand2) < EPS) {
                fprintf(stderr, "Division by zero, exited\n");
                *flag = 0;
            } else {
                result = operand1 / operand2;
            }
            break;
        }
        case TOKEN_STAR:
            result = operand1 * operand2;
            break;
        default:
            break;
    }
    if (*flag) {
        char str_double[64];
        sprintf(str_double, "%lf", result);
        token *result_token = new_token(str_double, TOKEN_NUMBER);
        op_stack_push(stack, result_token);
    }
}
