#include "as608.h"

uint8_t pack_head[]={0xEF,0X01};
uint8_t as608_addr[]={0xFF,0xFF,0xFF,0xFF};

uint16_t as608_flag = AS608_FLAG_IDLE;      // 当前指令解析状态
uint8_t  as608_last_cmd = CMD_NONE;         // 记录上一条下发的指令
uint16_t as608_matched_page_id = 0xFFFF;    // 搜索成功时匹配到的模板ID
uint16_t as608_matched_score   = 0;         // 搜索成功时的匹配分数

uint8_t as608_send(uint8_t* data,uint8_t size)
{
    UART6_DMA_SendBuff(data, size);
    return size;
}
uint8_t as608_sendpackhead(void)
{
    as608_send(pack_head,2);
    return 2;
}
uint8_t as608_sendaddr(void)
{
    as608_send(as608_addr,4);
    return 4;
}
uint16_t as608_sum(uint8_t* data,uint8_t size)
{
    uint16_t sum=0;
    for (int var = 0; var < size; ++var)
    {
        sum+=*data++;
    }

    return sum;
}

/*录入指纹*/
void as608_input(void)
{
    static uint8_t buff[]={
            0xEF,0X01,//包头
            0xFF,0XFF,0XFF,0XFF,//地址
            AS608_PKT_CMD,//标识：命令
            0x00,0x03,//包长度
            AS608_CMD_COLLECT_IMAGE,//命令：采集图形
            0x00,0x05,//校验和
    };
    as608_last_cmd=CMD_GENIMG;
    as608_send(buff,sizeof(buff));

}


/*生成特征*/
void as608_feature(uint8_t buff_name)
{
    static uint8_t buff[]={
            0xEF,0X01,//包头
            0xFF,0XFF,0XFF,0XFF,//地址
            AS608_PKT_CMD,//标识：命令
            0x00,0x04,//包长度
            AS608_CMD_GEN_CHAR,//命令：采集图形
            0x00,
            0x00,0x07,//校验和
    };

    buff[10]=buff_name;
    buff[12]+=buff[10];
    as608_send(buff,sizeof(buff));
    as608_last_cmd=buff_name==1?CMD_IMG2TZ1:CMD_IMG2TZ2;
}

/* 精确比对 CharBuffer1 和 CharBuffer2 */
void as608_match(void)
{
    static uint8_t buff[] = {
        0xEF, 0x01,                 // 包头
        0xFF, 0xFF, 0xFF, 0xFF,     // 地址
        AS608_PKT_CMD,             // 包标识（命令包）
        0x00, 0x03,                // 包长度（命令字节数+2）
        AS608_CMD_MATCH,           // 指令码（0x03 比对）
        0x00, 0x07                 // 校验和（0x01+0x03+0x03=0x07）
    };

    as608_send(buff, sizeof(buff));
    as608_last_cmd=CMD_MATCH;
}

// 合并 CharBuffer1 与 CharBuffer2 生成模板
void as608_merge_feature(void)
{
    static uint8_t buff[] = {
        0xEF, 0x01,                 // 包头
        0xFF, 0xFF, 0xFF, 0xFF,     // 地址
        AS608_PKT_CMD,             // 包标识（命令包）
        0x00, 0x03,                // 包长度
        AS608_CMD_REG_MODEL,       // 指令码（0x05 合并特征）
        0x00, 0x09                 // 校验和（0x01 + 0x03 + 0x05 = 0x09）
    };

    as608_send(buff, sizeof(buff));
    as608_last_cmd=CMD_REGMODEL;
}

// 将 CharBuffer1/2 中的模板存储到 page_id 位置
/*id最大为299*/
void as608_store_template(uint8_t buffer_id, uint16_t page_id)
{
    uint8_t buff[] = {
        0xEF, 0x01,                         // 包头
        0xFF, 0xFF, 0xFF, 0xFF,             // 地址
        AS608_PKT_CMD,                     // 包标识符
        0x00, 0x06,                         // 包长度
        AS608_CMD_STORE,                   // 指令码：存储模板
        buffer_id,                         // 缓存区号（1或2）
        (page_id >> 8) & 0xFF,             // 存储位置高字节
        page_id,                    // 存储位置低字节
        0x00, 0x00                          // 校验和（后续计算）
    };

    // 计算校验和（从包标识符 0x01 开始到 page_id 低字节为止）
    uint16_t sum = 0;
    for (int i = 6; i <= 11; i++) {
        sum += buff[i];
    }

    buff[12] = (sum >> 8) & 0xFF;  // 校验和高字节
    buff[13] = sum & 0xFF;         // 校验和低字节

    as608_send(buff, sizeof(buff));
    as608_last_cmd=CMD_STORE;
}


/*搜索指纹*/
void as608_search(uint8_t buffer_id, uint16_t start_page, uint16_t page_num)
{
    uint8_t buff[] = {
        0xEF, 0x01,                       // 包头
        0xFF, 0xFF, 0xFF, 0xFF,           // 地址
        AS608_PKT_CMD,                   // 包标识符（命令）
        0x00, 0x08,                       // 包长度（指令 + 参数 + 校验）
        AS608_CMD_SEARCH,                // 指令码 0x04
        buffer_id,                       // 缓冲区编号（1 或 2）
        start_page >> 8,        // 起始页高
        start_page,               // 起始页低
        page_num >> 8,          // 搜索页数高
        page_num,                 // 搜索页数低
        0x00, 0x00                        // 校验和
    };

    // 计算校验和：从包标识（0x01）开始累加到 page_num 低位
    uint16_t sum = 0;
    for (int i = 6; i <= 13; i++) {
        sum += buff[i];
    }

    buff[14] = sum >> 8;
    buff[15] = sum ;

    as608_send(buff, sizeof(buff));
    as608_last_cmd=CMD_SEARCH;
}

/* 删除指纹 */
void as608_delete_template(uint16_t page_id, uint16_t count)
{
    uint8_t buff[] = {
        0xEF, 0x01,                     // 包头
        0xFF, 0xFF, 0xFF, 0xFF,         // 模块地址
        AS608_PKT_CMD,                 // 命令包标识
        0x00, 0x07,                     // 包长度
        AS608_CMD_DELETE_CHAR,              // 指令码 0x0C
        (page_id >> 8) & 0xFF,         // PageID 高字节
        page_id & 0xFF,                // PageID 低字节
        (count >> 8) & 0xFF,           // 删除数量 高
        count & 0xFF,                  // 删除数量 低
        0x00, 0x00                      // 校验和（后续计算）
    };

    // 校验和计算：从标识符开始，到 count 低字节为止
    uint16_t sum = 0;
    for (int i = 6; i <= 13; i++) {
        sum += buff[i];
    }

    buff[14] = (sum >> 8) & 0xFF;
    buff[15] = sum & 0xFF;

    as608_send(buff, sizeof(buff));
    as608_last_cmd=CMD_DELETE;
}




// ===================== 主解析器函数 ===================== //
int8_t as608_Process(uint8_t* buf, uint16_t len)
{
    uint16_t pack_len = 0;        // 数据包声明长度
    uint8_t buff[32] = {0};       // 有效数据缓冲
    uint16_t sum = 0;             // 校验和

    // ----------- 校验包头（0xEF 0x01）-----------
    for (int i = 0; i < 2; ++i) {
        if (buf[i] != pack_head[i]) return -1;
        if (--len == 0) return -1;
    }

    // ----------- 校验模块地址（默认全FF）-----------
    for (int i = 0; i < 4; ++i) {
        if (buf[i+2] != as608_addr[i]) return -1;
        if (--len == 0) return -1;
    }

    // ----------- 校验包标识（响应包=0x07）-----------
    if (buf[6] != 0x07) return -1;
    if (--len == 0) return -1;

    // ----------- 读取数据包长度 -----------
    pack_len = ((uint16_t)buf[7] << 8) | buf[8];
    len -= 2;
    if (len == 0) return -1;
    if (len < pack_len) return -1;

    // ----------- 拷贝有效数据段（确认码+内容+校验和）-----------
    for (int i = 9; i < pack_len + 9; ++i) {
        buff[i-9] = buf[i];
    }

    // ----------- 校验和校验 -----------
    sum = 0;
    for (int i = 6; i < 6 + pack_len; ++i) {
        sum += buf[i];
    }
    uint16_t recv_sum = ((uint16_t)buf[9 + pack_len - 2] << 8) | buf[9 + pack_len - 1];
    if (sum != recv_sum) return -2;

    // ----------- 联合“上一次指令”与本次确认码解析操作 -----------

    switch(as608_last_cmd)
    {
        case CMD_GENIMG: // 采集指纹
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_GENIMG_OK;
            else if(buff[0]==0x02) as608_flag = AS608_FLAG_NO_FINGER;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_IMG2TZ1: // 特征1提取
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_IMG2TZ1_OK;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_IMG2TZ2: // 特征2提取
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_IMG2TZ2_OK;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_REGMODEL: // 合成模板
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_REGMODEL_OK;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_STORE: // 存储模板
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_STORE_OK;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_SEARCH: // 搜索指纹
            if(buff[0]==0x00 && pack_len>=5) {
                as608_flag = AS608_FLAG_SEARCH_OK;
                as608_matched_page_id = ((uint16_t)buff[1]<<8)|buff[2];
                as608_matched_score   = ((uint16_t)buff[3]<<8)|buff[4];
            } else if(buff[0]==0x09) {
                as608_flag = AS608_FLAG_NOT_FOUND;
            } else {
                as608_flag = AS608_FLAG_FAIL;
            }
            break;
        case CMD_MATCH: // 比对
            if(buff[0]==0x00 || buff[0]==0x08)
                as608_flag = AS608_FLAG_MATCH_OK;
            else if(buff[0]==0x0C)
                as608_flag = AS608_FLAG_MATCH_FAIL;
            else
                as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_DELETE: // 删除模板
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_DELETE_OK;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        default: // 未知/未跟踪指令
            as608_flag = AS608_FLAG_FAIL;
            break;
    }
    // as608_flag、as608_matched_page_id、as608_matched_score供主控逻辑随时读取
    return 0;
}

void as608_proc(void)
{
    // 采集图像成功
    if (as608_flag & AS608_FLAG_GENIMG_OK)
    {
        // TODO: 图像采集成功业务处理
        as608_flag &= ~AS608_FLAG_GENIMG_OK; // 只清本位
    }

    // 特征1提取成功
    if (as608_flag & AS608_FLAG_IMG2TZ1_OK)
    {
        // TODO: 特征1提取成功处理
        as608_flag &= ~AS608_FLAG_IMG2TZ1_OK;
    }

    // 特征2提取成功
    if (as608_flag & AS608_FLAG_IMG2TZ2_OK)
    {
        // TODO: 特征2提取成功处理
        as608_flag &= ~AS608_FLAG_IMG2TZ2_OK;
    }

    // 合成模板成功
    if (as608_flag & AS608_FLAG_REGMODEL_OK)
    {
        // TODO: 合成模板成功处理
        as608_flag &= ~AS608_FLAG_REGMODEL_OK;
    }

    // 存储模板成功
    if (as608_flag & AS608_FLAG_STORE_OK)
    {
        // TODO: 存储成功后续
        as608_flag &= ~AS608_FLAG_STORE_OK;
    }

    // 搜索（比对）成功
    if (as608_flag & AS608_FLAG_SEARCH_OK)
    {
        // TODO: 匹配成功处理
        // 可使用 as608_matched_page_id, as608_matched_score
        as608_flag &= ~AS608_FLAG_SEARCH_OK;
    }

    // 搜索未找到
    if (as608_flag & AS608_FLAG_NOT_FOUND)
    {
        // TODO: 未找到处理
        as608_flag &= ~AS608_FLAG_NOT_FOUND;
    }

    // 比对成功
    if (as608_flag & AS608_FLAG_MATCH_OK)
    {
        // TODO: 比对成功处理
        as608_flag &= ~AS608_FLAG_MATCH_OK;
    }

    // 比对失败
    if (as608_flag & AS608_FLAG_MATCH_FAIL)
    {
        // TODO: 比对失败处理
        as608_flag &= ~AS608_FLAG_MATCH_FAIL;
    }

    // 删除模板成功
    if (as608_flag & AS608_FLAG_DELETE_OK)
    {
        // TODO: 删除模板后业务
        as608_flag &= ~AS608_FLAG_DELETE_OK;
    }

    // 没有检测到手指
    if (as608_flag & AS608_FLAG_NO_FINGER)
    {
        // TODO: 无手指业务提示
        as608_flag &= ~AS608_FLAG_NO_FINGER;
    }

    // 通用失败（错误等）
    if (as608_flag & AS608_FLAG_FAIL)
    {
        // TODO: 错误处理或日志
        as608_flag &= ~AS608_FLAG_FAIL;
    }
}




