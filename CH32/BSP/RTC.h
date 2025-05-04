/* RTC.h */
#ifndef __RTC_H
#define __RTC_H

#include "bsp.h" // 硬件依赖头文件

/* 错误状态枚举 */
typedef enum {
    RTC_OK = 0,       // 操作成功
    RTC_INVALID_TIME  // 时间参数无效
} RTC_Status;


/* 函数声明 */
RTC_Status RTC_Init(uint32_t prescaler);
RTC_Status RTC_SetTime(const struct tm *time);
RTC_Status RTC_GetTime(struct tm *timeptr);
void RTC_proc(void);

#endif
