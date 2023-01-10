#ifndef __UPDATE_TIME_H_
#define __UPDATE_TIME_H_

    #include <inttypes.h>

    #include "auxilary.h"
    
    void speedInit(int32_t * upd_time, int32_t upd_time_init_val);
    
    void speedUpdate(int32_t * upd_time, int32_t inkrement, int32_t min, int32_t max, uint8_t cmd );

    extern int32_t upd_time;

#endif


