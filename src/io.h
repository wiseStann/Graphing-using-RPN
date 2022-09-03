#ifndef SRC_IO_H_
#define SRC_IO_H_

#include <stdio.h>
#include "graph.h"

#define INPUT_PREF "[expression]:"

void start_plotting();

char *read_expression(int *expr_size);

void print_field(int field[FIELD_HEIGHT][FIELD_WIDTH]);


#endif  // SRC_IO_H_
