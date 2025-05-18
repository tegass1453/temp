#ifndef __UART_PARSER_H
#define __UART_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp.h"

/*--------------------- 配置常量 ---------------------*/
#define MAX_DATA_LEN 16  // 最大有效数据长度

/*--------------------- 类型定义 ---------------------*/


// 指令处理单元
typedef struct {
    uint8_t cmd;         // 指令码
    void (*handler)(uint8_t* data,uint8_t data_len); // 处理函数指针
} UART_CmdHandler;

// 解析器实例结构体
typedef struct {
    uint8_t head;        // 包头标识
    uint8_t tail;        // 包尾标识
    const UART_CmdHandler* cmds;   // 指令集指针
    uint8_t cmd_count;             // 指令数量
    uint8_t state;                 // 状态机状态
} UART_Parser;
enum {
        PARSE_WAIT_HEADER,
        PARSE_RECEIVING_DATA,
};

/*--------------------- 函数声明 ---------------------*/


#define MAX_DATA_LEN 16  // 最大数据长度

void Parser_Set(UART_Parser* p,
                const uint8_t head,
                const uint8_t tail,
                const UART_CmdHandler* cmds,
                uint8_t count) ;
void Parser_Process(UART_Parser* p, uint8_t* buff, uint8_t len);


#ifdef __cplusplus
}
#endif

#endif /* __UART_PARSER_H */
