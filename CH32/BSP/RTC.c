/* RTC.c */
#include "RTC.h"


struct tm rtc_time={0};
/**
  * @brief  RTC��ʼ��
  * @param  prescaler: Ԥ��Ƶֵ������ʱ��Դ���㣩
  * @note   ����ʱ��Դ������Ϊ32.768kHz LSE
  *         Ԥ��Ƶֵ = 32768 - 1 = 0x7FFF
  */
RTC_Status RTC_Init(uint32_t prescaler)
{
    /* 1. ����ʱ��������� */
    // ����PWR��BKPʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    // ������ʱ�����
    PWR_BackupAccessCmd(ENABLE);

    /* 2. ����LSIʱ��Դ */
    // ����LSIʱ�ӣ�Լ40KHz��
    RCC_LSICmd(ENABLE);
    // �ȴ�LSI��������Ҫ����
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

    /* 3. ����RTCʱ��Դ */
    // ѡ��LSI��ΪRTCʱ��
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    // ����RTCʱ��
    RCC_RTCCLKCmd(ENABLE);

    /* 4. ����Ԥ��Ƶ�� */
    RTC_EnterConfigMode();
    // ע�⣺LSIƵ��40kHzʱ������Ԥ��Ƶֵʾ����
    // ��Ƶ��1Hz��Ҫ����39999��40000-1��
    RTC_SetPrescaler(prescaler);
    RTC_WaitForLastTask();
    RTC_ExitConfigMode();

    /* 5. ͬ������ */
    RTC_WaitForSynchro();

    rtc_time.tm_year=2025-1900;
    rtc_time.tm_mon=5-1;
    rtc_time.tm_mday=4;
    rtc_time.tm_hour=19;
    rtc_time.tm_min=59;
    rtc_time.tm_sec=59;



    return RTC_OK;
}

/**
  * @brief  ����UTCʱ��
  * @param  time: tm�ṹ��ָ�루ʱ���޹أ�
  * @retval ����״̬
  */
RTC_Status RTC_SetTime(const struct tm *time)
{
    /* ����������� */
    if(time->tm_year < 70)   return RTC_INVALID_TIME; // ��ݱ��� >= 1970
    if(time->tm_mon > 11)    return RTC_INVALID_TIME;
    if(time->tm_mday < 1)    return RTC_INVALID_TIME;
    if(time->tm_hour > 23)   return RTC_INVALID_TIME;
    if(time->tm_min > 59)    return RTC_INVALID_TIME;
    if(time->tm_sec > 59)    return RTC_INVALID_TIME;

    /* ����UNIXʱ��� */
    struct tm tmp = *time;
    time_t timestamp = mktime(&tmp);

    /* ��������Ƿ��Զ���������2��30�գ� */
    if(tmp.tm_mday != time->tm_mday ||
       tmp.tm_mon != time->tm_mon ||
       tmp.tm_year != time->tm_year){
        return RTC_INVALID_TIME;
    }

    /* д��Ӳ�� */
    RTC_EnterConfigMode();
    RTC_SetCounter((uint32_t)timestamp);
    RTC_WaitForLastTask();
    RTC_ExitConfigMode();

    return RTC_OK;
}

/**
  * @brief  ��ȡ��ǰUTCʱ��
  * @retval tm�ṹ�壨ֱ��ӳ��Ӳ��ʱ�䣩
  */
RTC_Status RTC_GetTime(struct tm *timeptr)
{
    /* ������Ч�Լ�� */
    if(timeptr == NULL) return RTC_INVALID_TIME;

    /* ��ȡӲ��������ֵ */
    uint32_t counter = RTC_GetCounter();

    /* ת��ΪUNIXʱ�����ע��CH32V307��RTC��ʼʱ�䣩*/
    time_t timestamp = (time_t)counter;

    /* ת��ΪUTCʱ��ṹ�� */
    struct tm *tmp = gmtime(&timestamp);
    if(tmp == NULL) return RTC_INVALID_TIME;

    /* �������ݵ�������� */
    memcpy(timeptr, tmp, sizeof(struct tm));

    return RTC_OK;
}
void RTC_proc(void)
{

    RTC_GetTime(&rtc_time);
    uart4_printf("Date: %04d-%02d-%02d %02d:%02d:%02d",
           rtc_time.tm_year,
           rtc_time.tm_mon,
           rtc_time.tm_mday,
           rtc_time.tm_hour,
           rtc_time.tm_min,
           rtc_time.tm_sec);


}
