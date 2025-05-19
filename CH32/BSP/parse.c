#include  "parse.h"



/*--------------------- UART2 配置 ---------------------*/
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

/*--------------------- UART3 配置 ---------------------*/
void set_password3(uint8_t* data, uint8_t data_len) {
    while(data_len--) {
        uart3_printf("%C", *data++);  // 使用 UART3 的打印函数
    }
}

const UART_CmdHandler uart3_cmd[] = {
    {0x01, set_password3},  // 命令 ID 和对应的处理函数
};

UART_Parser uart3_parser = {
    .head = 0XAA,  // 帧头（与 UART2 相同，可根据需求修改）
    .tail = 0XA7,  // 帧尾
    .cmds = uart3_cmd,
    .cmd_count = sizeof(uart3_cmd)/sizeof(uart3_cmd[0]),
    .state = PARSE_WAIT_HEADER
};

/*--------------------- UART4 配置 ---------------------*/
void set_password4(uint8_t* data, uint8_t data_len) {
    while(data_len--) {
        uart4_printf("%C", *data++);  // 使用 UART4 的打印函数
    }
}

const UART_CmdHandler uart4_cmd[] = {
    {0x01, set_password4},  // 命令 ID 可自定义
};

UART_Parser uart4_parser = {
    .head = 0XAA,
    .tail = 0XA7,
    .cmds = uart4_cmd,
    .cmd_count = sizeof(uart4_cmd)/sizeof(uart4_cmd[0]),
    .state = PARSE_WAIT_HEADER
};

/*--------------------- UART6 配置 ---------------------*/
void set_password6(uint8_t* data, uint8_t data_len) {
    while(data_len--) {
        uart6_printf("%C", *data++);  // 使用 UART6 的打印函数
    }
}

const UART_CmdHandler uart6_cmd[] = {
    {0x01, set_password6},  // 命令 ID 可自定义
};

UART_Parser uart6_parser = {
    .head = 0XAA,
    .tail = 0XA7,
    .cmds = uart6_cmd,
    .cmd_count = sizeof(uart6_cmd)/sizeof(uart6_cmd[0]),
    .state = PARSE_WAIT_HEADER
};












// 初始化解析器
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
  * @brief  解析处理接收到的串口数据
  * @param  p     解析器实例指针
  * @param  buff  输入数据缓冲区
  * @param  len   数据长度
  * @note   实现包头包尾检测、指令解析及处理函数调用
  */
void Parser_Process(UART_Parser* p, uint8_t* buff, uint8_t len)
{

    uint8_t head_index=0;
    uint8_t tail_index=0;
    uint8_t cmd=0;
    /* 遍历缓冲区中的每个字节 */
    for (uint8_t i = 0; i < len; i++) {
        uint8_t current_byte = buff[i];  // 当前处理的字节

        /* 状态机处理 */
        switch (p->state) {
            /* 状态0：等待包头 */
            case PARSE_WAIT_HEADER:
                if (current_byte == p->head) {
                head_index = i;
                cmd=buff[i+1];
                //uart2_printf("%d\n",head_index);
                    p->state = PARSE_RECEIVING_DATA;       // 切换到数据接收状态
                }
                break;

            /* 状态1：接收数据（指令+数据） */
            case PARSE_RECEIVING_DATA:
                /* 检查缓冲区溢出 */
                /* 检测包尾 */
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




