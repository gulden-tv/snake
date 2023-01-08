
#ifndef __SNAKE_AND_FOOD_H_
#define __SNAKE_AND_FOOD_H_

    #include "snake.h"
    #include "food.h"

    // вычисляет количество ходов до еды
    int distance(const snake snake, const food food); 

    void autoChangeDirection(snake *snake, food food[], int foodSize); 

    void repairSeed(food f[], size_t nfood, snake *head); 

    _Bool haveEat(snake *head, food f[]); 


#endif

