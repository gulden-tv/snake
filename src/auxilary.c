#include "../inc/auxilary.h"

//Ограничение входного значения
void lim(int32_t * val, int32_t min, int32_t max)
{
    if (*val<min)
    {
        *val = min;
    }
    else if (*val>max)
    {
        *val = max;
    }
}