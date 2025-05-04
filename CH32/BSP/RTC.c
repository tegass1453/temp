/* RTC.c */
#include "RTC.h"


struct tm rtc_time={0};
/**
  * @brief  RTC初始化
  * @param  prescaler: 预分频值（根据时钟源计算）
  * @note   假设时钟源已配置为32.768kHz LSE
  *         预分频值 = 32768 - 1 = 0x7FFF
  */
RTC_Status RTC_Init(uint32_t prescaler)
{
    /* 1. 启用时钟相关外设 */
    // 启用PWR和BKP时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    // 允许访问备份域
    PWR_BackupAccessCmd(ENABLE);

    /* 2. 配置LSI时钟源 */
    // 开启LSI时钟（约40KHz）
    RCC_LSICmd(ENABLE);
    // 等待LSI就绪（重要！）
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

    /* 3. 配置RTC时钟源 */
    // 选择LSI作为RTC时钟
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    // 启用RTC时钟
    RCC_RTCCLKCmd(ENABLE);

    /* 4. 配置预分频器 */
    RTC_EnterConfigMode();
    // 注意：LSI频率40kHz时，典型预分频值示例：
    // 分频到1Hz需要设置39999（40000-1）
    RTC_SetPrescaler(prescaler);
    RTC_WaitForLastTask();
    RTC_ExitConfigMode();

    /* 5. 同步操作 */
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
  * @brief  设置UTC时间
  * @param  time: tm结构体指针（时区无关）
  * @retval 操作状态
  */
RTC_Status RTC_SetTime(const struct tm *time)
{
    /* 基础参数检查 */
    if(time->tm_year < 70)   return RTC_INVALID_TIME; // 年份必须 >= 1970
    if(time->tm_mon > 11)    return RTC_INVALID_TIME;
    if(time->tm_mday < 1)    return RTC_INVALID_TIME;
    if(time->tm_hour > 23)   return RTC_INVALID_TIME;
    if(time->tm_min > 59)    return RTC_INVALID_TIME;
    if(time->tm_sec > 59)    return RTC_INVALID_TIME;

    /* 生成UNIX时间戳 */
    struct tm tmp = *time;
    time_t timestamp = mktime(&tmp);

    /* 检查日期是否被自动修正（如2月30日） */
    if(tmp.tm_mday != time->tm_mday ||
       tmp.tm_mon != time->tm_mon ||
       tmp.tm_year != time->tm_year){
        return RTC_INVALID_TIME;
    }

    /* 写入硬件 */
    RTC_EnterConfigMode();
    RTC_SetCounter((uint32_t)timestamp);
    RTC_WaitForLastTask();
    RTC_ExitConfigMode();

    return RTC_OK;
}

/**
  * @brief  获取当前UTC时间
  * @retval tm结构体（直接映射硬件时间）
  */
RTC_Status RTC_GetTime(struct tm *timeptr)
{
    /* 参数有效性检查 */
    if(timeptr == NULL) return RTC_INVALID_TIME;

    /* 读取硬件计数器值 */
    uint32_t counter = RTC_GetCounter();

    /* 转换为UNIX时间戳（注意CH32V307的RTC起始时间）*/
    time_t timestamp = (time_t)counter;

    /* 转换为UTC时间结构体 */
    struct tm *tmp = gmtime(&timestamp);
    if(tmp == NULL) return RTC_INVALID_TIME;

    /* 复制数据到输出参数 */
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
