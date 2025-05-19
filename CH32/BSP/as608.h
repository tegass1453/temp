#ifndef __UART_PARSER_H
#define __UART_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp.h"




/**
  * @brief  初始化AS608指纹模块通信接口
  * @note   配置UART7通信参数（波特率57600, 8N1）
  *         需在系统初始化阶段调用
  *         对应指令：UART7硬件初始化
  */
void as608_init(void);

/**
  * @brief  从传感器采集指纹图像
  */


void PS_GetImage(void);

/**
  * @brief  生成指纹特征文件
  */
void PS_GenChar(u8 buffer);

/**
  * @brief  生成指纹模板
  * @note   合并两个特征缓冲区生成完整指纹模板（指令码0x05）
  *         指令结构：
  *         [EF01][FFFFFFFF][01][0003][05][00][0009]
  *         校验和：0x01+0x00+0x03+0x05=0x09（低字节）
  *         模板暂存于模块内部缓冲区，需调用PS_StoreChar存储
  */

void PS_RegModel(void);

/**
  * @brief  存储指纹模板到指定位置
  * @param  addr 存储位置（1-1000）
  * @note   发送模板存储指令（指令码0x06）
  *         指令结构：
  *         [EF01][FFFFFFFF][01][0006][06][0200][addr][校验和]
  *         示例：addr=5时校验和=0x01+0x00+0x06+0x06+0x02+0x00+0x05=0x14
  *         每个位置只能存储一个模板，重复写入会覆盖
  */
void PS_StoreChar(u8 addr);

/**
  * @brief  指纹特征搜索
  * @note   在指纹库中搜索匹配模板（指令码0x04）
  *         指令结构：
  *         [EF01][FFFFFFFF][01][0008][04][02000000FF][校验和]
  *         参数说明：
  *           - 搜索范围：0x0000-0xFFFF（此处从0开始搜索全部）
  *           - 最大匹配阈值：0xFF（500分）
  *         返回匹配得分和位置（通过uart7_rec_string[13]获取得分）
  */
void PS_Search(void);

/**
  * @brief  初始化指纹模块状态检测GPIO
  * @note   配置PA1引脚为浮空输入模式
  *         用于检测模块的Touch信号（手指按压状态）
  *         需配合中断或轮询方式检测引脚状态变化
  */
void As608_Gpio_Init();

/**
  * @brief  清空指纹数据库
  * @note   发送清库指令（指令码0x0D）
  *         指令结构：
  *         [EF01][FFFFFFFF][01][0003][0D][00][0011]
  *         校验和：0x01+0x00+0x03+0x0D=0x11（低字节）
  *         将删除所有存储的指纹模板（不可恢复）
  */
void PS_Empty(void);
void as608_proc(void);


#ifdef __cplusplus
}
#endif

#endif /* __UART_PARSER_H */





