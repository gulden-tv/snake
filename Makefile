# Makefile
LDFLAGS = -Llib .\pdcurses.dll -s -Wl,--subsystem,console

snake: main.c
	gcc -o snake main.c $(LDFLAGS)

all: snake