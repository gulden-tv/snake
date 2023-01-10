#ifndef __FOOD_H_
#define __FOOD_H_
    
    #include <stdlib.h>
    #include <time.h>
 
    #include "config.h"
    #include "color.h"
    
    /*
        Еда массив точек
        x, y - координата где установлена точка
        put_time - время когда данная точка была установлена
        point - внешний вид точки ('$','E'...)
        enable - была ли точка съедена
    */
    typedef struct  
    {
        int x;
        int y;
        time_t put_time;
        char point;
        uint8_t enable;
    }food;

    extern food corn[MAX_FOOD_SIZE];

    //Инициализация еды
    void initFood(food f[], size_t size); 
    //Обновить/разместить текущее зерно на поле
    void putFoodSeed(food *fp); 
    // Мигаем зерном, перед тем как оно исчезнет
    void blinkFood(food fp[], size_t nfood); 
    //Разместить еду на поле
    void putFood(food f[], size_t number_seeds); 

    void refreshFood(food f[], int nfood);

#endif