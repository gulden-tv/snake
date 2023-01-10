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
        EXIT_TIME_OUT = 20000,
        UPDATE_TIME_INIT_VAL = 500,
        UPDATE_TIME_MIN_VAL = 50,
        UPDATE_TIME_MAX_VAL =1000,
        UPDATE_TIME_INKREMENT = 50,
        UPDATE_TIME_DECREMENT = -50,
        UPDATE_TIME_CHANGE_CMD = 1,
        SEED_NUMBER = 3
    };

    enum
    {
        SNAKE_USER_NUMBER = 1,
        SNAKE_ROBOT_NUMBER = 2,
        FOOD_NUMBER = 3
    };

#endif
