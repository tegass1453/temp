#include "schedule.h"

uint64_t system_time;
uint32_t Task_num;

Task system_array[]={
   {MFRC522_Proc,0,500},
   {uart_proc,0,10},
   {RTC_proc,0,2000},
   {as608_proc,0,200},

};

uint32_t schedule_get_time(void) {
    return system_time;
}
void schedule_init(void) {
    Task_num = sizeof(system_array)/sizeof(system_array[0]);
}

void schedule_proc(void) {
	
	system_time = TIM2_GetTick();
	
    for (int i=0;i<Task_num;i++){
        if(system_time>=system_array[i].period+system_array[i].last_time){
            
			system_array[i].task_func();
			system_array[i].last_time=system_time;
			
        }
    }
}
