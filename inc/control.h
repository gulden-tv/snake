#ifndef __CONTROL_H_
#define __CONTROL_H_

    #include <inttypes.h>
    #include <ncurses.h>
   

    #include "config.h"

    void changeDirection(int32_t *new_direction, int32_t key);
    int checkDirection(int32_t dir, int32_t key); 

#endif