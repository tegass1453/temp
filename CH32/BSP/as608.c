#include "as608.h"

uint8_t pack_head[]={0xEF,0X01};
uint8_t as608_addr[]={0xFF,0xFF,0xFF,0xFF};

uint16_t as608_flag = AS608_FLAG_IDLE;      // ��ǰָ�����״̬
uint8_t  as608_last_cmd = CMD_NONE;         // ��¼��һ���·���ָ��
uint16_t as608_matched_page_id = 0xFFFF;    // �����ɹ�ʱƥ�䵽��ģ��ID
uint16_t as608_matched_score   = 0;         // �����ɹ�ʱ��ƥ�����

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

/*¼��ָ��*/
void as608_input(void)
{
    static uint8_t buff[]={
            0xEF,0X01,//��ͷ
            0xFF,0XFF,0XFF,0XFF,//��ַ
            AS608_PKT_CMD,//��ʶ������
            0x00,0x03,//������
            AS608_CMD_COLLECT_IMAGE,//����ɼ�ͼ��
            0x00,0x05,//У���
    };
    as608_last_cmd=CMD_GENIMG;
    as608_send(buff,sizeof(buff));

}


/*��������*/
void as608_feature(uint8_t buff_name)
{
    static uint8_t buff[]={
            0xEF,0X01,//��ͷ
            0xFF,0XFF,0XFF,0XFF,//��ַ
            AS608_PKT_CMD,//��ʶ������
            0x00,0x04,//������
            AS608_CMD_GEN_CHAR,//����ɼ�ͼ��
            0x00,
            0x00,0x07,//У���
    };

    buff[10]=buff_name;
    buff[12]+=buff[10];
    as608_send(buff,sizeof(buff));
    as608_last_cmd=buff_name==1?CMD_IMG2TZ1:CMD_IMG2TZ2;
}

/* ��ȷ�ȶ� CharBuffer1 �� CharBuffer2 */
void as608_match(void)
{
    static uint8_t buff[] = {
        0xEF, 0x01,                 // ��ͷ
        0xFF, 0xFF, 0xFF, 0xFF,     // ��ַ
        AS608_PKT_CMD,             // ����ʶ���������
        0x00, 0x03,                // �����ȣ������ֽ���+2��
        AS608_CMD_MATCH,           // ָ���루0x03 �ȶԣ�
        0x00, 0x07                 // У��ͣ�0x01+0x03+0x03=0x07��
    };

    as608_send(buff, sizeof(buff));
    as608_last_cmd=CMD_MATCH;
}

// �ϲ� CharBuffer1 �� CharBuffer2 ����ģ��
void as608_merge_feature(void)
{
    static uint8_t buff[] = {
        0xEF, 0x01,                 // ��ͷ
        0xFF, 0xFF, 0xFF, 0xFF,     // ��ַ
        AS608_PKT_CMD,             // ����ʶ���������
        0x00, 0x03,                // ������
        AS608_CMD_REG_MODEL,       // ָ���루0x05 �ϲ�������
        0x00, 0x09                 // У��ͣ�0x01 + 0x03 + 0x05 = 0x09��
    };

    as608_send(buff, sizeof(buff));
    as608_last_cmd=CMD_REGMODEL;
}

// �� CharBuffer1/2 �е�ģ��洢�� page_id λ��
/*id���Ϊ299*/
void as608_store_template(uint8_t buffer_id, uint16_t page_id)
{
    uint8_t buff[] = {
        0xEF, 0x01,                         // ��ͷ
        0xFF, 0xFF, 0xFF, 0xFF,             // ��ַ
        AS608_PKT_CMD,                     // ����ʶ��
        0x00, 0x06,                         // ������
        AS608_CMD_STORE,                   // ָ���룺�洢ģ��
        buffer_id,                         // �������ţ�1��2��
        (page_id >> 8) & 0xFF,             // �洢λ�ø��ֽ�
        page_id,                    // �洢λ�õ��ֽ�
        0x00, 0x00                          // У��ͣ��������㣩
    };

    // ����У��ͣ��Ӱ���ʶ�� 0x01 ��ʼ�� page_id ���ֽ�Ϊֹ��
    uint16_t sum = 0;
    for (int i = 6; i <= 11; i++) {
        sum += buff[i];
    }

    buff[12] = (sum >> 8) & 0xFF;  // У��͸��ֽ�
    buff[13] = sum & 0xFF;         // У��͵��ֽ�

    as608_send(buff, sizeof(buff));
    as608_last_cmd=CMD_STORE;
}


/*����ָ��*/
void as608_search(uint8_t buffer_id, uint16_t start_page, uint16_t page_num)
{
    uint8_t buff[] = {
        0xEF, 0x01,                       // ��ͷ
        0xFF, 0xFF, 0xFF, 0xFF,           // ��ַ
        AS608_PKT_CMD,                   // ����ʶ�������
        0x00, 0x08,                       // �����ȣ�ָ�� + ���� + У�飩
        AS608_CMD_SEARCH,                // ָ���� 0x04
        buffer_id,                       // ��������ţ�1 �� 2��
        start_page >> 8,        // ��ʼҳ��
        start_page,               // ��ʼҳ��
        page_num >> 8,          // ����ҳ����
        page_num,                 // ����ҳ����
        0x00, 0x00                        // У���
    };

    // ����У��ͣ��Ӱ���ʶ��0x01����ʼ�ۼӵ� page_num ��λ
    uint16_t sum = 0;
    for (int i = 6; i <= 13; i++) {
        sum += buff[i];
    }

    buff[14] = sum >> 8;
    buff[15] = sum ;

    as608_send(buff, sizeof(buff));
    as608_last_cmd=CMD_SEARCH;
}

/* ɾ��ָ�� */
void as608_delete_template(uint16_t page_id, uint16_t count)
{
    uint8_t buff[] = {
        0xEF, 0x01,                     // ��ͷ
        0xFF, 0xFF, 0xFF, 0xFF,         // ģ���ַ
        AS608_PKT_CMD,                 // �������ʶ
        0x00, 0x07,                     // ������
        AS608_CMD_DELETE_CHAR,              // ָ���� 0x0C
        (page_id >> 8) & 0xFF,         // PageID ���ֽ�
        page_id & 0xFF,                // PageID ���ֽ�
        (count >> 8) & 0xFF,           // ɾ������ ��
        count & 0xFF,                  // ɾ������ ��
        0x00, 0x00                      // У��ͣ��������㣩
    };

    // У��ͼ��㣺�ӱ�ʶ����ʼ���� count ���ֽ�Ϊֹ
    uint16_t sum = 0;
    for (int i = 6; i <= 13; i++) {
        sum += buff[i];
    }

    buff[14] = (sum >> 8) & 0xFF;
    buff[15] = sum & 0xFF;

    as608_send(buff, sizeof(buff));
    as608_last_cmd=CMD_DELETE;
}




// ===================== ������������ ===================== //
int8_t as608_Process(uint8_t* buf, uint16_t len)
{
    uint16_t pack_len = 0;        // ���ݰ���������
    uint8_t buff[32] = {0};       // ��Ч���ݻ���
    uint16_t sum = 0;             // У���

    // ----------- У���ͷ��0xEF 0x01��-----------
    for (int i = 0; i < 2; ++i) {
        if (buf[i] != pack_head[i]) return -1;
        if (--len == 0) return -1;
    }

    // ----------- У��ģ���ַ��Ĭ��ȫFF��-----------
    for (int i = 0; i < 4; ++i) {
        if (buf[i+2] != as608_addr[i]) return -1;
        if (--len == 0) return -1;
    }

    // ----------- У�����ʶ����Ӧ��=0x07��-----------
    if (buf[6] != 0x07) return -1;
    if (--len == 0) return -1;

    // ----------- ��ȡ���ݰ����� -----------
    pack_len = ((uint16_t)buf[7] << 8) | buf[8];
    len -= 2;
    if (len == 0) return -1;
    if (len < pack_len) return -1;

    // ----------- ������Ч���ݶΣ�ȷ����+����+У��ͣ�-----------
    for (int i = 9; i < pack_len + 9; ++i) {
        buff[i-9] = buf[i];
    }

    // ----------- У���У�� -----------
    sum = 0;
    for (int i = 6; i < 6 + pack_len; ++i) {
        sum += buf[i];
    }
    uint16_t recv_sum = ((uint16_t)buf[9 + pack_len - 2] << 8) | buf[9 + pack_len - 1];
    if (sum != recv_sum) return -2;

    // ----------- ���ϡ���һ��ָ��뱾��ȷ����������� -----------

    switch(as608_last_cmd)
    {
        case CMD_GENIMG: // �ɼ�ָ��
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_GENIMG_OK;
            else if(buff[0]==0x02) as608_flag = AS608_FLAG_NO_FINGER;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_IMG2TZ1: // ����1��ȡ
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_IMG2TZ1_OK;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_IMG2TZ2: // ����2��ȡ
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_IMG2TZ2_OK;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_REGMODEL: // �ϳ�ģ��
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_REGMODEL_OK;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_STORE: // �洢ģ��
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_STORE_OK;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_SEARCH: // ����ָ��
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
        case CMD_MATCH: // �ȶ�
            if(buff[0]==0x00 || buff[0]==0x08)
                as608_flag = AS608_FLAG_MATCH_OK;
            else if(buff[0]==0x0C)
                as608_flag = AS608_FLAG_MATCH_FAIL;
            else
                as608_flag = AS608_FLAG_FAIL;
            break;
        case CMD_DELETE: // ɾ��ģ��
            if(buff[0]==0x00)      as608_flag = AS608_FLAG_DELETE_OK;
            else                   as608_flag = AS608_FLAG_FAIL;
            break;
        default: // δ֪/δ����ָ��
            as608_flag = AS608_FLAG_FAIL;
            break;
    }
    // as608_flag��as608_matched_page_id��as608_matched_score�������߼���ʱ��ȡ
    return 0;
}

void as608_proc(void)
{
    // �ɼ�ͼ��ɹ�
    if (as608_flag & AS608_FLAG_GENIMG_OK)
    {
        // TODO: ͼ��ɼ��ɹ�ҵ����
        as608_flag &= ~AS608_FLAG_GENIMG_OK; // ֻ�屾λ
    }

    // ����1��ȡ�ɹ�
    if (as608_flag & AS608_FLAG_IMG2TZ1_OK)
    {
        // TODO: ����1��ȡ�ɹ�����
        as608_flag &= ~AS608_FLAG_IMG2TZ1_OK;
    }

    // ����2��ȡ�ɹ�
    if (as608_flag & AS608_FLAG_IMG2TZ2_OK)
    {
        // TODO: ����2��ȡ�ɹ�����
        as608_flag &= ~AS608_FLAG_IMG2TZ2_OK;
    }

    // �ϳ�ģ��ɹ�
    if (as608_flag & AS608_FLAG_REGMODEL_OK)
    {
        // TODO: �ϳ�ģ��ɹ�����
        as608_flag &= ~AS608_FLAG_REGMODEL_OK;
    }

    // �洢ģ��ɹ�
    if (as608_flag & AS608_FLAG_STORE_OK)
    {
        // TODO: �洢�ɹ�����
        as608_flag &= ~AS608_FLAG_STORE_OK;
    }

    // �������ȶԣ��ɹ�
    if (as608_flag & AS608_FLAG_SEARCH_OK)
    {
        // TODO: ƥ��ɹ�����
        // ��ʹ�� as608_matched_page_id, as608_matched_score
        as608_flag &= ~AS608_FLAG_SEARCH_OK;
    }

    // ����δ�ҵ�
    if (as608_flag & AS608_FLAG_NOT_FOUND)
    {
        // TODO: δ�ҵ�����
        as608_flag &= ~AS608_FLAG_NOT_FOUND;
    }

    // �ȶԳɹ�
    if (as608_flag & AS608_FLAG_MATCH_OK)
    {
        // TODO: �ȶԳɹ�����
        as608_flag &= ~AS608_FLAG_MATCH_OK;
    }

    // �ȶ�ʧ��
    if (as608_flag & AS608_FLAG_MATCH_FAIL)
    {
        // TODO: �ȶ�ʧ�ܴ���
        as608_flag &= ~AS608_FLAG_MATCH_FAIL;
    }

    // ɾ��ģ��ɹ�
    if (as608_flag & AS608_FLAG_DELETE_OK)
    {
        // TODO: ɾ��ģ���ҵ��
        as608_flag &= ~AS608_FLAG_DELETE_OK;
    }

    // û�м�⵽��ָ
    if (as608_flag & AS608_FLAG_NO_FINGER)
    {
        // TODO: ����ָҵ����ʾ
        as608_flag &= ~AS608_FLAG_NO_FINGER;
    }

    // ͨ��ʧ�ܣ�����ȣ�
    if (as608_flag & AS608_FLAG_FAIL)
    {
        // TODO: ���������־
        as608_flag &= ~AS608_FLAG_FAIL;
    }
}




