# Makefile
LDFLAGS=-lncurses

snake: main.c Makefile
	gcc -o snake main.c $(LDFLAGS) -g

all: snake
