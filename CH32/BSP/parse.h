#ifndef __UART_PARSER_H
#define __UART_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp.h"

/*--------------------- ���ó��� ---------------------*/
#define MAX_DATA_LEN 16  // �����Ч���ݳ���

/*--------------------- ���Ͷ��� ---------------------*/


// ָ���Ԫ
typedef struct {
    uint8_t cmd;         // ָ����
    void (*handler)(uint8_t* data,uint8_t data_len); // ������ָ��
} UART_CmdHandler;

// ������ʵ���ṹ��
typedef struct {
    uint8_t head;        // ��ͷ��ʶ
    uint8_t tail;        // ��β��ʶ
    const UART_CmdHandler* cmds;   // ָ�ָ��
    uint8_t cmd_count;             // ָ������
    uint8_t state;                 // ״̬��״̬
} UART_Parser;
enum {
        PARSE_WAIT_HEADER,
        PARSE_RECEIVING_DATA,
};

/*--------------------- �������� ---------------------*/


#define MAX_DATA_LEN 16  // ������ݳ���

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
