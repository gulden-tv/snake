#ifndef __SNAKE_H_
#define __SNAKE_H_

    #include <stdio.h>
    #include <stdlib.h>
    #include <inttypes.h>
    
    #include "config.h"
    #include "color.h"


    /*
        Хвост этто массив состоящий из координат x,y
    */
    typedef struct 
    {
        int x;
        int y;
    }tail; 

    /*
        Голова змейки содержит в себе
        x,y - координаты текущей позиции
        direction - направление движения
        tsize - размер хвоста
        *tail -  ссылка на хвост
    */
    typedef struct  
    {
        int number;
        int x;
        int y;
        int direction;
        size_t tsize;
        tail *tail;
    }snake;


    //Инициализация хвоста змеи
    void initTail(tail t[], size_t size); 
    //Инициализация головы змеи
    void initHead(snake *head); 
    //Инициализация змеи
    void init(snake *head, int number, tail *tail, size_t size); 
    //Движение головы с учетом текущего направления движения 
    void go(snake *head);
    //Движение хвоста с учетом движения головы
    void goTail(snake *head); 
    //Увеличение хвоста на 1 элемент
    void addTail(snake *head); 
    //Выводит на экран уровни игрока и змеи бота 
    void printLevel(snake *head); 
    //Выводит в конце игры уровень игрока
    void printExit(snake *head); 
    //Определяет врезалась ли голова змеи в хвост змеи
    _Bool isCrash(snake *head); 

    extern tail snake_user_tail[MAX_TAIL_SIZE], snake_robot_tail[MAX_TAIL_SIZE];
    extern snake snake_user, snake_robot;

#endif