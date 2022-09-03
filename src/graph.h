#ifndef SRC_GRAPH_H_
#define SRC_GRAPH_H_

#include <stdio.h>
#include <stdlib.h>
#include "token.h"

#define FIELD_WIDTH        80
#define FIELD_HEIGHT       25

#define GAME_WIDTH         (FIELD_WIDTH + 2)
#define GAME_HEIGHT        (FIELD_HEIGHT + 2)

#define EXPRESSION_LEN     64
#define EXPRESSION_EXP_VAL 16

typedef struct Point {
    int x;
    double y;
} point;

void generate_graph(point *points, token **RPN_tokens, int tokens_size, int *flag);

void show_graph(const point *points);

#endif  // SRC_GRAPH_H_
