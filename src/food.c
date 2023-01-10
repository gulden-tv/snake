#include "../inc/food.h"

food corn[MAX_FOOD_SIZE];

void initFood(food f[], size_t size) 
{
    food init = {0, 0, 0, 0, 0};
    int max_y = 0, max_x = 0;
    getmaxyx(stdscr, max_y, max_x);
    for (size_t i = 0; i < size; i++) {
        f[i] = init;
    }
}


/*
 Обновить/разместить текущее зерно на поле
*/
void putFoodSeed(food *fp) 
{
    int max_x = 0, max_y = 0;
    char spoint[2] = {0};
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(fp->y, fp->x, " ");
    fp->x = rand() % (max_x - 1);
    fp->y = rand() % (max_y - 2) + 1; //Не занимаем верхнюю строку
    fp->put_time = time(NULL);
    fp->point = '$';
    fp->enable = 1;
    spoint[0] = fp->point;
    setColor(FOOD_NUMBER);
    mvprintw(fp->y, fp->x, spoint);
}

// Мигаем зерном, перед тем как оно исчезнет
void blinkFood(food fp[], size_t nfood) 
{
    time_t current_time = time(NULL);
    char spoint[2] = {0}; // как выглядит зерно '$','\0'
    for (size_t i = 0; i < nfood; i++) {
        if (fp[i].enable && (current_time - fp[i].put_time) > 6) {
            spoint[0] = (current_time % 2) ? 'S' : 's';
            setColor(FOOD_NUMBER);
            mvprintw(fp[i].y, fp[i].x, spoint);
        }
    }
}

/*
  Разместить еду на поле
*/
void putFood(food f[], size_t number_seeds) 
{
    for (size_t i = 0; i < number_seeds; i++) {
        putFoodSeed(&f[i]);
    }
}

void refreshFood(food f[], int nfood) {
    int max_x = 0, max_y = 0;
    char spoint[2] = {0};
    getmaxyx(stdscr, max_y, max_x);
    for (size_t i = 0; i < nfood; i++) {
        if (f[i].put_time) {
            if (!f[i].enable || (time(NULL) - f[i].put_time) > FOOD_EXPIRE_SECONDS) {
                putFoodSeed(&f[i]);
            }
        }
    }
}