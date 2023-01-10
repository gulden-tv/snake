# Makefile
LDFLAGS=-lncurses

run:
	./debug/program

build:
	gcc -c src/auxilary.c -o debug/auxilary.o
	gcc -c src/color.c -o debug/color.o
	gcc -c src/update_time.c -o debug/update_time.o
	gcc -c src/config.c -o debug/config.o
	gcc -c src/control.c -o debug/control.o
	gcc -c src/help.c -o debug/help.o
	gcc -c src/food.c -o debug/food.o
	gcc -c src/snake.c -o debug/snake.o
	gcc -c src/snake_and_food.c -o debug/snake_and_food.o
	gcc -c main.c -o debug/main.o

	gcc -o debug/program debug/auxilary.o debug/update_time.o debug/color.o debug/config.o debug/control.o debug/help.o debug/food.o debug/snake.o debug/snake_and_food.o debug/main.o $(LDFLAGS)  

clean:
	rm -r debug/**

download:
	git add .
	git commit -m "Сохранение"
	git push origin lesson3

upload:
	git stash 
	git pull origin lesson3




	 