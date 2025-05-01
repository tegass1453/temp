#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "bsp.h"

typedef struct Task{
    void (*task_func)(void);
    uint32_t last_time;
    uint32_t period;
}Task;
void schedule_proc(void) ;
void schedule_init(void);
uint32_t schedule_get_time(void);

#endif
