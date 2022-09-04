CFLAGS = -Wall -Werror -Wextra
TARGET = ../build/graph

all: graph

graph: graph.c token.c op_stack.c io.c parse.c RPN.c
	gcc $(CFLAGS) graph.c token.c op_stack.c io.c parse.c RPN.c -o $(TARGET) -lm

rebuild: clean all

clean:
	rm -rf ../build/*