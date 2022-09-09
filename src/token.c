#include "token.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char *functions[FUNCTIONS_SIZE] = {
    "sin", "cos", "tan", "ctg", "ln", "sqrt", NULL,
};

int get_operator_kind(char operator) {
  int kind = -1;
  switch (operator) {
  case '-':
    kind = TOKEN_BIN_MINUS;
    break;
  case '+':
    kind = TOKEN_BIN_PLUS;
    break;
  case '/':
    kind = TOKEN_RSLASH;
    break;
  case '*':
    kind = TOKEN_STAR;
    break;
  case '(':
    kind = TOKEN_OPEN_BRACKET;
    break;
  case ')':
    kind = TOKEN_CLOSE_BRACKET;
    break;
  }
  return kind;
}

int get_operator_precedence(token_kind_e token_kind) {
  int precedence = 0;
  switch (token_kind) {
  case TOKEN_BIN_PLUS:
  case TOKEN_BIN_MINUS:
    precedence = 1;
    break;
  case TOKEN_UN_PLUS:
  case TOKEN_UN_MINUS:
    precedence = 1;
    break;
  case TOKEN_RSLASH:
  case TOKEN_STAR:
    precedence = 2;
    break;
  case TOKEN_OPEN_BRACKET:
  case TOKEN_CLOSE_BRACKET:
    precedence = 4;
    break;
  default:
    break;
  }
  return precedence;
}

tokenizer *new_tokenizer(char *expression, int expression_size) {
  tokenizer *_tokenizer = (tokenizer *)malloc(sizeof(struct Tokenizer));
  _tokenizer->expression = expression; // must not be freed
  _tokenizer->expr_size = expression_size;
  _tokenizer->curr_pos = 0;
  _tokenizer->tokens_size = 0;
  _tokenizer->prev = NULL;
  return _tokenizer;
}

int get_function_id(char str_token[LEXEME_MAX_LEN]) {
  int id = -1, flag = 1;
  for (int i = 0; functions[i] != NULL && flag; i++) {
    if (strcmp(str_token, functions[i]) == 0) {
      id = i;
      flag = 0;
    }
  }
  return id + 1;
}

token *new_token(char str_token[LEXEME_MAX_LEN], int text_pos,
                 token_kind_e kind) {
  token *_token = NULL;
  if (str_token == NULL) {
    fprintf(stderr, "Token of kind id '%d' is NULL, aborted\n", kind);
    return NULL;
  } else {
    _token = (token *)malloc(sizeof(struct Token));
    switch (kind) {
    case TOKEN_NUMBER:
      _token->number = strtod(str_token, NULL);
      break;
    case TOKEN_FUNCTION_NAME:
      strcpy(_token->func, str_token);
      _token->func_id = get_function_id(str_token);
      break;
    // operator
    default:
      _token->operator.text = str_token[0];
      _token->operator.precedence = get_operator_precedence(kind);
      break;
    }
    _token->kind = kind;
  }
  strcpy(_token->str_token, str_token);
  _token->text_pos = text_pos;
  return _token;
}

token **tokenize_input(char *expression, int expr_size, int *tokens_size) {
  tokenizer *_tokenizer = new_tokenizer(expression, expr_size);
  token **tokens = tokenize_expression(_tokenizer);
  *tokens_size = _tokenizer->tokens_size;
  free(_tokenizer);
  return tokens;
}

token **tokenize_expression(tokenizer *_tokenizer) {
  token **tokens =
      (token **)malloc(_tokenizer->expr_size * sizeof(struct Token));
  int token_idx = 0;
  for (; _tokenizer->curr_pos < _tokenizer->expr_size;) {
    char symbol = get_curr_symbol(_tokenizer);
    token *next_token = NULL;
    if (isdigit(symbol)) {
      next_token = next_number_token(_tokenizer);
    } else if (isalpha(symbol)) {
      next_token = next_word_token(_tokenizer);
    } else if (isspace(symbol)) {
      _tokenizer->curr_pos++;
      continue;
    } else {
      token_kind_e kind = get_operator_kind(symbol);
      if (curr_symbol_is_unary_operator(_tokenizer)) {
        if (symbol == '-') {
          kind = TOKEN_UN_MINUS;
        } else if (symbol == '+') {
          kind = TOKEN_UN_PLUS;
        }
      }
      char str_symbol[LEXEME_MAX_LEN] = {symbol};
      next_token = new_token(str_symbol, _tokenizer->curr_pos, kind);
      _tokenizer->curr_pos++;
    }
    tokens[token_idx] = next_token;
    token_idx++;
    _tokenizer->prev = next_token;
  }

  _tokenizer->tokens_size = token_idx;
  return tokens;
}

char get_curr_symbol(tokenizer *_tokenizer) {
  char next;
  if (_tokenizer->curr_pos > _tokenizer->expr_size)
    next = _tokenizer->expression[_tokenizer->expr_size - 1];
  else
    next = _tokenizer->expression[_tokenizer->curr_pos];
  return next;
}

int curr_symbol_is_unary_operator(tokenizer *_tokenizer) {
  // +10
  // 10 - +10
  // () + 10
  int result;
  if (_tokenizer->prev)
    result = (_tokenizer->prev->kind != TOKEN_NUMBER &&
              _tokenizer->prev->kind != TOKEN_FUNCTION_NAME &&
              _tokenizer->prev->kind != TOKEN_VARIABLE_NAME &&
              _tokenizer->prev->kind != TOKEN_OPEN_BRACKET &&
              _tokenizer->prev->kind != TOKEN_CLOSE_BRACKET);
  else
    result = 1;
  return result;
}

token *next_word_token(tokenizer *_tokenizer) {
  token *word = NULL;
  char str_word[LEXEME_MAX_LEN];
  int idx = 0, start_pos = _tokenizer->curr_pos;
  char curr_symbol;
  while ((curr_symbol = get_curr_symbol(_tokenizer)) != '\0' &&
         isalpha(curr_symbol)) {
    str_word[idx] = curr_symbol;
    idx++;
    _tokenizer->curr_pos++;
  }
  str_word[idx] = '\0';
  if (strcmp(str_word, "x") == 0)
    word = new_token(str_word, start_pos, TOKEN_VARIABLE_NAME);
  else
    word = new_token(str_word, start_pos, TOKEN_FUNCTION_NAME);
  return word;
}

token *next_number_token(tokenizer *_tokenizer) {
  token *number = NULL;
  char str_number[LEXEME_MAX_LEN];
  int idx = 0, start_pos = _tokenizer->curr_pos;
  char curr_symbol;
  while ((curr_symbol = get_curr_symbol(_tokenizer)) != '\0' &&
         isdigit(curr_symbol)) {
    str_number[idx] = curr_symbol;
    idx++;
    _tokenizer->curr_pos++;
  }
  // if float point number is given
  if (get_curr_symbol(_tokenizer) == '.') {
    str_number[idx++] = get_curr_symbol(_tokenizer);
    _tokenizer->curr_pos++;
    while ((curr_symbol = get_curr_symbol(_tokenizer)) != '\0' &&
           isdigit(curr_symbol)) {
      str_number[idx] = curr_symbol;
      idx++;
      _tokenizer->curr_pos++;
    }
  }
  str_number[idx] = '\0';
  number = new_token(str_number, start_pos, TOKEN_NUMBER);
  return number;
}
