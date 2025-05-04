/* RTC.h */
#ifndef __RTC_H
#define __RTC_H

#include "bsp.h" // Ӳ������ͷ�ļ�

/* ����״̬ö�� */
typedef enum {
    RTC_OK = 0,       // �����ɹ�
    RTC_INVALID_TIME  // ʱ�������Ч
} RTC_Status;


/* �������� */
RTC_Status RTC_Init(uint32_t prescaler);
RTC_Status RTC_SetTime(const struct tm *time);
RTC_Status RTC_GetTime(struct tm *timeptr);
void RTC_proc(void);

#endif
