CFLAGS = -Wall -Werror -Wextra
TARGET = graph

all: graph

graph: graph.c token.c op_stack.c c_type.c io.c parse.c RPN.c
	gcc $(CFLAGS) graph.c token.c op_stack.c c_type.c io.c parse.c RPN.c -o $(TARGET)

rebuild: clean all

clean:
	rm *.o