#include <string.h>
#include "io.h"
#include "graph.h"
#include "token.h"
#include "parse.h"
#include "RPN.h"

void start_plotting() {
  int expr_size, tokens_size, flag = 1;
  while (flag) {
    (void)tokens_size;
    printf("%s ", INPUT_PREF);
    char *input = read_expression(&expr_size);
    if (strcmp(input, "exit") == 0) {
      flag = 0;
    } else {
      token **tokens = tokenize_input(input, expr_size, &tokens_size);;
      if (parse_input(tokens, tokens_size)) {
        int RPN_tokens_size = 0;
        token **RPN_tokens = convert_to_RPN((const token**)tokens, tokens_size, &RPN_tokens_size);
        point points[FIELD_WIDTH];
        generate_graph(points, RPN_tokens, RPN_tokens_size, &flag);
        if (flag) {
          show_graph(points);
        }
        for (int i = 0; i < RPN_tokens_size; i++) {
          if (RPN_tokens[i]->func_id > 0)
            free(RPN_tokens[i]);
        }
        free(RPN_tokens);
      }

      for (int i = 0; i < tokens_size; i++)
        free(tokens[i]);
      free(tokens);
    }
    free(input);
  }
}

char *read_expression(int *expr_size) {
  char *input = (char *)malloc(EXPRESSION_LEN * sizeof(char));
  int ch = 0, idx = 0;
  while ((ch = getchar()) != '\n' || idx == 0) {
    if (idx > EXPRESSION_LEN) {
      void* tmp = realloc(input, idx + EXPRESSION_EXP_VAL);
      if (tmp == NULL) {
        fprintf(stderr, "Realloc returned NULL, aborted\n");
        free(input);
        return NULL;
      } else {
        input = tmp;
      }
    }
    input[idx] = ch;
    idx++;
  }
  input[idx] = '\0';
  *expr_size = idx;
  return input;
}

void print_field(int field[FIELD_HEIGHT][FIELD_WIDTH]) {
  for (int i = -1; i < GAME_HEIGHT - 1; i++) {
    for (int j = -1; j < GAME_WIDTH - 1; j++) {
      if ((i > -1 || i < GAME_HEIGHT - 1) && (j == -1 || j == GAME_WIDTH - 2)) {
        printf("|");
      } else if (i == -1 || i == GAME_HEIGHT - 2) {
        printf("-");
      } else if (i < FIELD_HEIGHT && j < FIELD_WIDTH && j > -1) {
        if (field[i][j] == 1)
          printf("*");
        else
          printf(".");
      }
    }
    printf("\n");
  }
  printf("\n");
}
