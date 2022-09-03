#ifndef SRC_TOKEN_H_
#define SRC_TOKEN_H_

#include <stdio.h>

#define FUNCTIONS_SIZE 7

extern char *functions[FUNCTIONS_SIZE];

#define LEXEME_MAX_LEN 64

typedef enum TokenKind {
    TOKEN_NUMBER,
    TOKEN_FUNCTION_NAME,
    TOKEN_VARIABLE_NAME,

    // operators
    TOKEN_UN_PLUS,
    TOKEN_BIN_PLUS,
    TOKEN_UN_MINUS,
    TOKEN_BIN_MINUS,
    TOKEN_RSLASH,
    TOKEN_STAR,
    TOKEN_OPEN_BRACKET,
    TOKEN_CLOSE_BRACKET,
} token_kind_e;

typedef struct OperatorToken {
    int  precedence;
    char text;
} op_token;

op_token *new_op_token(int precedence, char text);

int get_operator_precedence(token_kind_e token_kind);

typedef struct Token {
    union {
        long double number;
        op_token    operator;
        char        func[LEXEME_MAX_LEN];
    };
    char str_token[LEXEME_MAX_LEN];
    int func_id;
    int text_pos;
    token_kind_e kind;
} token;

typedef struct Tokenizer {
    char *expression;
    int expr_size;
    token *prev;
    int tokens_size;
    int curr_pos;
} tokenizer;

tokenizer *new_tokenizer(char *expression, int expression_size);

token *new_token(char *str_token, int text_pos, token_kind_e type);

token **tokenize_input(char *expression, int expr_size, int *tokens_size);

token **tokenize_expression(tokenizer *_tokenizer);

char get_curr_symbol(tokenizer *_tokenizer);

int curr_symbol_is_unary_operator(tokenizer *_tokenizer);

token *next_word_token(tokenizer *_tokenizer);

token *next_number_token(tokenizer *_tokenizer);


#endif  // SRC_TOKEN_H_
