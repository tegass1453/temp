#include  "parse.h"



/*--------------------- UART2 ���� ---------------------*/
void set_password(uint8_t* data,uint8_t data_len){
    while(data_len--)
    {
        uart2_printf("%C",*data++);
    }
}

const UART_CmdHandler uart2_cmd[] = {
        {0x01,set_password},
};
UART_Parser uart2_parser={
        .head=0XAA,
        .tail=0XA7,
        .cmds=uart2_cmd,
        .cmd_count=sizeof(uart2_cmd)/sizeof(uart2_cmd[0]),
        .state=PARSE_WAIT_HEADER

};

/*--------------------- UART3 ���� ---------------------*/
void set_password3(uint8_t* data, uint8_t data_len) {
    while(data_len--) {
        uart3_printf("%C", *data++);  // ʹ�� UART3 �Ĵ�ӡ����
    }
}

const UART_CmdHandler uart3_cmd[] = {
    {0x01, set_password3},  // ���� ID �Ͷ�Ӧ�Ĵ�����
};

UART_Parser uart3_parser = {
    .head = 0XAA,  // ֡ͷ���� UART2 ��ͬ���ɸ��������޸ģ�
    .tail = 0XA7,  // ֡β
    .cmds = uart3_cmd,
    .cmd_count = sizeof(uart3_cmd)/sizeof(uart3_cmd[0]),
    .state = PARSE_WAIT_HEADER
};

/*--------------------- UART4 ���� ---------------------*/
void set_password4(uint8_t* data, uint8_t data_len) {
    while(data_len--) {
        uart4_printf("%C", *data++);  // ʹ�� UART4 �Ĵ�ӡ����
    }
}

const UART_CmdHandler uart4_cmd[] = {
    {0x01, set_password4},  // ���� ID ���Զ���
};

UART_Parser uart4_parser = {
    .head = 0XAA,
    .tail = 0XA7,
    .cmds = uart4_cmd,
    .cmd_count = sizeof(uart4_cmd)/sizeof(uart4_cmd[0]),
    .state = PARSE_WAIT_HEADER
};

/*--------------------- UART6 ���� ---------------------*/
void set_password6(uint8_t* data, uint8_t data_len) {
    while(data_len--) {
        uart6_printf("%C", *data++);  // ʹ�� UART6 �Ĵ�ӡ����
    }
}

const UART_CmdHandler uart6_cmd[] = {
    {0x01, set_password6},  // ���� ID ���Զ���
};

UART_Parser uart6_parser = {
    .head = 0XAA,
    .tail = 0XA7,
    .cmds = uart6_cmd,
    .cmd_count = sizeof(uart6_cmd)/sizeof(uart6_cmd[0]),
    .state = PARSE_WAIT_HEADER
};












// ��ʼ��������
void Parser_Set(UART_Parser* p,
                uint8_t head, 
                uint8_t tail,
                const UART_CmdHandler* cmds,
                uint8_t count) 
{
    p->head = head;
    p->tail = tail;
    p->cmds = cmds;
    p->cmd_count = count;

}


/**
  * @brief  ����������յ��Ĵ�������
  * @param  p     ������ʵ��ָ��
  * @param  buff  �������ݻ�����
  * @param  len   ���ݳ���
  * @note   ʵ�ְ�ͷ��β��⡢ָ�����������������
  */
void Parser_Process(UART_Parser* p, uint8_t* buff, uint8_t len)
{

    uint8_t head_index=0;
    uint8_t tail_index=0;
    uint8_t cmd=0;
    /* �����������е�ÿ���ֽ� */
    for (uint8_t i = 0; i < len; i++) {
        uint8_t current_byte = buff[i];  // ��ǰ������ֽ�

        /* ״̬������ */
        switch (p->state) {
            /* ״̬0���ȴ���ͷ */
            case PARSE_WAIT_HEADER:
                if (current_byte == p->head) {
                head_index = i;
                cmd=buff[i+1];
                //uart2_printf("%d\n",head_index);
                    p->state = PARSE_RECEIVING_DATA;       // �л������ݽ���״̬
                }
                break;

            /* ״̬1���������ݣ�ָ��+���ݣ� */
            case PARSE_RECEIVING_DATA:
                /* ��黺������� */
                /* ����β */
                if (current_byte == p->tail) {
                    tail_index=i;
                    //uart2_printf("%d\n",tail_index);
                    }
                break;

             }
    }





    if(p->state == PARSE_WAIT_HEADER)
    {
        uart2_printf("ERROR!\n");
    }
    else
        for (int var = 0; var < p->cmd_count; ++var) {
             if(p->cmds[var].cmd==cmd)
             {
                 //uart2_printf("%d,%d\n",head_index+2,tail_index-head_index-2);
                 p->cmds[var].handler(buff+head_index+2,tail_index-head_index-2);
             }
        }
    p->state = PARSE_WAIT_HEADER;

}




