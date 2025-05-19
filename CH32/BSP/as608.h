#ifndef __UART_PARSER_H
#define __UART_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp.h"




/**
  * @brief  ��ʼ��AS608ָ��ģ��ͨ�Žӿ�
  * @note   ����UART7ͨ�Ų�����������57600, 8N1��
  *         ����ϵͳ��ʼ���׶ε���
  *         ��Ӧָ�UART7Ӳ����ʼ��
  */
void as608_init(void);

/**
  * @brief  �Ӵ������ɼ�ָ��ͼ��
  */


void PS_GetImage(void);

/**
  * @brief  ����ָ�������ļ�
  */
void PS_GenChar(u8 buffer);

/**
  * @brief  ����ָ��ģ��
  * @note   �ϲ�����������������������ָ��ģ�壨ָ����0x05��
  *         ָ��ṹ��
  *         [EF01][FFFFFFFF][01][0003][05][00][0009]
  *         У��ͣ�0x01+0x00+0x03+0x05=0x09�����ֽڣ�
  *         ģ���ݴ���ģ���ڲ��������������PS_StoreChar�洢
  */

void PS_RegModel(void);

/**
  * @brief  �洢ָ��ģ�嵽ָ��λ��
  * @param  addr �洢λ�ã�1-1000��
  * @note   ����ģ��洢ָ�ָ����0x06��
  *         ָ��ṹ��
  *         [EF01][FFFFFFFF][01][0006][06][0200][addr][У���]
  *         ʾ����addr=5ʱУ���=0x01+0x00+0x06+0x06+0x02+0x00+0x05=0x14
  *         ÿ��λ��ֻ�ܴ洢һ��ģ�壬�ظ�д��Ḳ��
  */
void PS_StoreChar(u8 addr);

/**
  * @brief  ָ����������
  * @note   ��ָ�ƿ�������ƥ��ģ�壨ָ����0x04��
  *         ָ��ṹ��
  *         [EF01][FFFFFFFF][01][0008][04][02000000FF][У���]
  *         ����˵����
  *           - ������Χ��0x0000-0xFFFF���˴���0��ʼ����ȫ����
  *           - ���ƥ����ֵ��0xFF��500�֣�
  *         ����ƥ��÷ֺ�λ�ã�ͨ��uart7_rec_string[13]��ȡ�÷֣�
  */
void PS_Search(void);

/**
  * @brief  ��ʼ��ָ��ģ��״̬���GPIO
  * @note   ����PA1����Ϊ��������ģʽ
  *         ���ڼ��ģ���Touch�źţ���ָ��ѹ״̬��
  *         ������жϻ���ѯ��ʽ�������״̬�仯
  */
void As608_Gpio_Init();

/**
  * @brief  ���ָ�����ݿ�
  * @note   �������ָ�ָ����0x0D��
  *         ָ��ṹ��
  *         [EF01][FFFFFFFF][01][0003][0D][00][0011]
  *         У��ͣ�0x01+0x00+0x03+0x0D=0x11�����ֽڣ�
  *         ��ɾ�����д洢��ָ��ģ�壨���ɻָ���
  */
void PS_Empty(void);
void as608_proc(void);


#ifdef __cplusplus
}
#endif

#endif /* __UART_PARSER_H */





