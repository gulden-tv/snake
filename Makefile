# Makefile
LDFLAGS=-lncurses
SRCS += main.c
SRCS += rec_table.c

snake:
	gcc $(SRCS) -o snake $(LDFLAGS)

all: snake