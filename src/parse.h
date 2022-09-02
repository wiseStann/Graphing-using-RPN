#ifndef SRC_PARSE_H_
#define SRC_PARSE_H_

#include "op_stack.h"
#include "token.h"


#define ERR_MESSAGE_MAX_LEN 128

#define _THROW_ERROR(fmt, ...) {         \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
}

// Parser structure
typedef struct Parser {
    token **tokens;
    op_stack *brackets;
    int tokens_size;
    int curr_pos;
    int state;
} parser;


parser *new_parser(token **tokens, int tokens_size);

int is_valid_operator(int operator);

int expect_token(parser *_parser, token_kind_e expected);

token *get_curr_token(parser *_parser);

void eat_token(parser *_parser);

int parse_input(token **tokens, int tokens_size);

int parse_expression(parser *_parser);

void parse_single_expression(parser *_parser);

void parse_parent_expression(parser *_parser);

void parse_operator(parser *_parser);

void parse_function(parser *_parser);

void parse_number(parser *_parser);


#endif  // SRC_PARSE_H_
