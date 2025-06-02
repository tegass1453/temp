#ifndef __AS608_H
#define __AS608_H

#include "bsp.h"

#ifdef __cplusplus
extern "C" {
#endif


// AS608 标志位
#define CMD_NONE      0x00    // 无操作
#define CMD_GENIMG    0x01    // 采集图像
#define CMD_IMG2TZ1   0x02    // 转特征到缓冲区1
#define CMD_IMG2TZ2   0x22    // 转特征到缓冲区2
#define CMD_REGMODEL  0x05    // 合成模板
#define CMD_STORE     0x06    // 存储模板
#define CMD_SEARCH    0x04    // 搜索指纹
#define CMD_MATCH     0x03    // 比对
#define CMD_DELETE    0x0C    // 删除


// ======== 状态标志位定义，供主控程序读取和流程控制 ========
#define AS608_FLAG_IDLE           0x0000  // 空闲状态
#define AS608_FLAG_GENIMG_OK      0x0001  // 采集指纹图像成功
#define AS608_FLAG_IMG2TZ1_OK     0x0002  // 特征1提取成功
#define AS608_FLAG_IMG2TZ2_OK     0x0004  // 特征2提取成功
#define AS608_FLAG_REGMODEL_OK    0x0008  // 模型合成成功
#define AS608_FLAG_STORE_OK       0x0010  // 模板存储成功
#define AS608_FLAG_SEARCH_OK      0x0020  // 搜索成功
#define AS608_FLAG_MATCH_OK       0x0040  // 比对成功
#define AS608_FLAG_DELETE_OK      0x0080  // 删除模板成功
#define AS608_FLAG_FAIL           0x8000  // 通用失败
#define AS608_FLAG_NO_FINGER      0x0100  // 无手指
#define AS608_FLAG_NOT_FOUND      0x0200  // 未找到
#define AS608_FLAG_MATCH_FAIL     0x0400  // 比对失败



// AS608 数据包标识符
#define AS608_PKT_CMD      0x01  // 命令包（MCU -> AS608）
#define AS608_PKT_DATA     0x02  // 数据包（MCU -> AS608）
#define AS608_PKT_ACK      0x07  // 应答包（AS608 -> MCU）
#define AS608_PKT_END      0x08  // 结束数据包
#define AS608_PKT_IMAGE    0x0A  // 图像/模板数据包（AS608 -> MCU）

// AS608 指令集
#define AS608_CMD_COLLECT_IMAGE   0x01  // 采集图像
#define AS608_CMD_GEN_CHAR        0x02  // 生成特征
#define AS608_CMD_MATCH           0x03  // 比对特征
#define AS608_CMD_SEARCH          0x04  // 搜索指纹库
#define AS608_CMD_REG_MODEL       0x05  // 注册模板
#define AS608_CMD_STORE           0x06  // 存储模板
#define AS608_CMD_LOAD_CHAR       0x07  // 读取模板
#define AS608_CMD_UP_CHAR         0x08  // 上传模板
#define AS608_CMD_DOWN_CHAR       0x09  // 下载模板
#define AS608_CMD_UP_IMAGE        0x0A  // 上传图像
#define AS608_CMD_DOWN_IMAGE      0x0B  // 下载图像
#define AS608_CMD_DELETE_CHAR     0x0C  // 删除模板
#define AS608_CMD_EMPTY           0x0D  // 清空指纹库
#define AS608_CMD_SET_SYS_PARAM   0x0E  // 设置系统参数
#define AS608_CMD_READ_SYS_PARAM  0x0F  // 读取系统参数
#define AS608_CMD_GET_TEMPLATE_CNT 0x1D // 获取模板数
#define AS608_CMD_WRITE_NOTEPAD   0x18  // 写便签区
#define AS608_CMD_READ_NOTEPAD    0x19  // 读便签区
#define AS608_CMD_HIGH_SPEED_SEARCH 0x1B // 高速搜索
#define AS608_CMD_VERIFYPWD       0x13  // 验证密码



uint8_t  as608_send(uint8_t* data, uint8_t size);
uint8_t  as608_sendpackhead(void);
uint8_t  as608_sendaddr(void);
uint16_t as608_sum(uint8_t* data, uint8_t size);

void     as608_input(void);                                 // 采集图像
void     as608_feature(uint8_t buff_name);                  // 生成特征
void     as608_match(void);                                 // 比对
void     as608_merge_feature(void);                         // 合成模板
void     as608_store_template(uint8_t buffer_id, uint16_t page_id); // 存储模板
void     as608_search(uint8_t buffer_id, uint16_t start_page, uint16_t page_num); // 搜索
void     as608_delete_template(uint16_t page_id, uint16_t count);    // 删除模板

int8_t   as608_Process(uint8_t* buf, uint16_t len);         // 主解析器
void     as608_proc(void);                                 // 结果处理调度器
void as608_movedata(uint16_t addr_old, uint16_t addr);







#ifdef __cplusplus
}
#endif

#endif /* __AS608_UART6_H */
