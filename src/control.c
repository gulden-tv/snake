#include "../inc/control.h"


void changeDirection(int32_t *new_direction, int32_t key) 
{
    switch (key) {
        case KEY_DOWN: // стрелка вниз
            *new_direction = DOWN;
            break;
        case KEY_UP: // стрелка вверх
            *new_direction = UP;
            break;
        case KEY_LEFT: // стрелка влево
            *new_direction = LEFT;
            break;
        case KEY_RIGHT: // стрелка вправо
            *new_direction = RIGHT;
            break;
        default:
            break;
    }
}

int checkDirection(int32_t dir, int32_t key) 
{
    if (KEY_DOWN == key && dir == UP || KEY_UP == key && dir == DOWN || KEY_LEFT == key && dir == RIGHT ||
        KEY_RIGHT == key && dir == LEFT) {
        return 0;
    } else {
        return 1;
    }
}


