#include "../inc/update_time.h"

int32_t upd_time;


void speedInit(int32_t * upd_time, int32_t upd_time_init_val)
{
    *upd_time=upd_time_init_val;
}

void speedUpdate(int32_t * upd_time, int32_t inkrement, int32_t min, int32_t max, uint8_t cmd )
{
    if(cmd)
    {
        *upd_time+=inkrement;
        lim(upd_time,min,max);
    }
}