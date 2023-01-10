
#include "../inc/snake_and_food.h"



// вычисляет количество ходов до еды
int distance(const snake snake, const food food) 
{   
    return (abs(snake.x - food.x) + abs(snake.y - food.y));
}


void autoChangeDirection(snake *snake, food food[], int foodSize) 
{
    int pointer = 0;
    for (int i = 1; i < foodSize; i++) {   // ищем ближайшую еду
        pointer = (distance(*snake, food[i]) < distance(*snake, food[pointer])) ? i : pointer;
    }
    if ((snake->direction == RIGHT || snake->direction == LEFT) &&
        (snake->y != food[pointer].y)) {  // горизонтальное движение
        snake->direction = (food[pointer].y > snake->y) ? DOWN : UP;
    } else if ((snake->direction == DOWN || snake->direction == UP) &&
               (snake->x != food[pointer].x)) {  // вертикальное движение
        snake->direction = (food[pointer].x > snake->x) ? RIGHT : LEFT;
    }
}

void repairSeed(food f[], size_t nfood, snake *head) 
{
    for (size_t i = 0; i < head->tsize; i++)
        for (size_t j = 0; j < nfood; j++) {
            /* Если хвост совпадает с зерном */
            if (f[j].x == head->tail[i].x && f[j].y == head->tail[i].y && f[i].enable) {
                mvprintw(0, 0, "Repair tail seed %d", j);
                putFoodSeed(&f[j]);
            }
        }
    for (size_t i = 0; i < nfood; i++)
        for (size_t j = 0; j < nfood; j++) {
            /* Если два зерна на одной точке */
            if (i != j && f[i].enable && f[j].enable && f[j].x == f[i].x && f[j].y == f[i].y && f[i].enable) {
                mvprintw(0, 0, "Repair same seed %d", j);
                putFoodSeed(&f[j]);
            }
        }
}

_Bool haveEat(snake *head, food f[]) 
{
    for (size_t i = 0; i < MAX_FOOD_SIZE; i++)
        if (f[i].enable && head->x == f[i].x && head->y == f[i].y) {
            f[i].enable = 0;
            return 1;
        }
    return 0;
}