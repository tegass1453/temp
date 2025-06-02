#ifndef __AS608_H
#define __AS608_H

#include "bsp.h"

#ifdef __cplusplus
extern "C" {
#endif


// AS608 ��־λ
#define CMD_NONE      0x00    // �޲���
#define CMD_GENIMG    0x01    // �ɼ�ͼ��
#define CMD_IMG2TZ1   0x02    // ת������������1
#define CMD_IMG2TZ2   0x22    // ת������������2
#define CMD_REGMODEL  0x05    // �ϳ�ģ��
#define CMD_STORE     0x06    // �洢ģ��
#define CMD_SEARCH    0x04    // ����ָ��
#define CMD_MATCH     0x03    // �ȶ�
#define CMD_DELETE    0x0C    // ɾ��


// ======== ״̬��־λ���壬�����س����ȡ�����̿��� ========
#define AS608_FLAG_IDLE           0x0000  // ����״̬
#define AS608_FLAG_GENIMG_OK      0x0001  // �ɼ�ָ��ͼ��ɹ�
#define AS608_FLAG_IMG2TZ1_OK     0x0002  // ����1��ȡ�ɹ�
#define AS608_FLAG_IMG2TZ2_OK     0x0004  // ����2��ȡ�ɹ�
#define AS608_FLAG_REGMODEL_OK    0x0008  // ģ�ͺϳɳɹ�
#define AS608_FLAG_STORE_OK       0x0010  // ģ��洢�ɹ�
#define AS608_FLAG_SEARCH_OK      0x0020  // �����ɹ�
#define AS608_FLAG_MATCH_OK       0x0040  // �ȶԳɹ�
#define AS608_FLAG_DELETE_OK      0x0080  // ɾ��ģ��ɹ�
#define AS608_FLAG_FAIL           0x8000  // ͨ��ʧ��
#define AS608_FLAG_NO_FINGER      0x0100  // ����ָ
#define AS608_FLAG_NOT_FOUND      0x0200  // δ�ҵ�
#define AS608_FLAG_MATCH_FAIL     0x0400  // �ȶ�ʧ��



// AS608 ���ݰ���ʶ��
#define AS608_PKT_CMD      0x01  // �������MCU -> AS608��
#define AS608_PKT_DATA     0x02  // ���ݰ���MCU -> AS608��
#define AS608_PKT_ACK      0x07  // Ӧ�����AS608 -> MCU��
#define AS608_PKT_END      0x08  // �������ݰ�
#define AS608_PKT_IMAGE    0x0A  // ͼ��/ģ�����ݰ���AS608 -> MCU��

// AS608 ָ�
#define AS608_CMD_COLLECT_IMAGE   0x01  // �ɼ�ͼ��
#define AS608_CMD_GEN_CHAR        0x02  // ��������
#define AS608_CMD_MATCH           0x03  // �ȶ�����
#define AS608_CMD_SEARCH          0x04  // ����ָ�ƿ�
#define AS608_CMD_REG_MODEL       0x05  // ע��ģ��
#define AS608_CMD_STORE           0x06  // �洢ģ��
#define AS608_CMD_LOAD_CHAR       0x07  // ��ȡģ��
#define AS608_CMD_UP_CHAR         0x08  // �ϴ�ģ��
#define AS608_CMD_DOWN_CHAR       0x09  // ����ģ��
#define AS608_CMD_UP_IMAGE        0x0A  // �ϴ�ͼ��
#define AS608_CMD_DOWN_IMAGE      0x0B  // ����ͼ��
#define AS608_CMD_DELETE_CHAR     0x0C  // ɾ��ģ��
#define AS608_CMD_EMPTY           0x0D  // ���ָ�ƿ�
#define AS608_CMD_SET_SYS_PARAM   0x0E  // ����ϵͳ����
#define AS608_CMD_READ_SYS_PARAM  0x0F  // ��ȡϵͳ����
#define AS608_CMD_GET_TEMPLATE_CNT 0x1D // ��ȡģ����
#define AS608_CMD_WRITE_NOTEPAD   0x18  // д��ǩ��
#define AS608_CMD_READ_NOTEPAD    0x19  // ����ǩ��
#define AS608_CMD_HIGH_SPEED_SEARCH 0x1B // ��������
#define AS608_CMD_VERIFYPWD       0x13  // ��֤����



uint8_t  as608_send(uint8_t* data, uint8_t size);
uint8_t  as608_sendpackhead(void);
uint8_t  as608_sendaddr(void);
uint16_t as608_sum(uint8_t* data, uint8_t size);

void     as608_input(void);                                 // �ɼ�ͼ��
void     as608_feature(uint8_t buff_name);                  // ��������
void     as608_match(void);                                 // �ȶ�
void     as608_merge_feature(void);                         // �ϳ�ģ��
void     as608_store_template(uint8_t buffer_id, uint16_t page_id); // �洢ģ��
void     as608_search(uint8_t buffer_id, uint16_t start_page, uint16_t page_num); // ����
void     as608_delete_template(uint16_t page_id, uint16_t count);    // ɾ��ģ��

int8_t   as608_Process(uint8_t* buf, uint16_t len);         // ��������
void     as608_proc(void);                                 // ������������
void as608_movedata(uint16_t addr_old, uint16_t addr);







#ifdef __cplusplus
}
#endif

#endif /* __AS608_UART6_H */
