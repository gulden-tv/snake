#ifndef __CONFIG_H_
#define __CONFIG_H_

    enum 
    {
        LEFT = 1, UP, RIGHT, DOWN, STOP_GAME = 'q'
    };

    enum 
    {
        MAX_TAIL_SIZE = 1000,
        START_TAIL_SIZE = 3,
        MAX_FOOD_SIZE = 20,
        FOOD_EXPIRE_SECONDS = 10,
        SPEED = 20000,
        SEED_NUMBER = 3
    };

    enum
    {
        SNAKE1 = 1,
        SNAKE2 = 2,
        FOOD = 3
    };

#endif
