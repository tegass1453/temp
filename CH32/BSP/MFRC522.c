#include "MFRC522.h"

/*****************���絥Ƭ�����******************
											STM32
 * �ļ�			:	MFRC522��Ƶģ��c�ļ�                   
 * �汾			: V1.0
 * ����			: 2024.9.2
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	������							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/

#define 	MAXRLEN 18
#define   RC522_DELAY()  Delay_Us(2)

#define timeout(time_old,freq) (((TIM2_GetTick()>=time_old+freq)?1:0)&&(time_old=TIM2_GetTick()||1))
#define Timeout(time_old,freq) (!timeout(time_old,freq))


#define  MFRC522_FLAG_ADD 0X01
uint8_t MFRC522_flag;

void MFRC522_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    // 1. ����GPIO��SPI1ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_SPI1, ENABLE);

    // 2. ���� SPI���� (SCK, MISO, MOSI)
    // SCK -> PA5, MOSI -> PA7�����츴�����
    GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_SCK_PIN | MFRC522_GPIO_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(MFRC522_GPIO_SCK_PORT, &GPIO_InitStructure);

    // MISO -> PA6����������
    GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(MFRC522_GPIO_MISO_PORT, &GPIO_InitStructure);

    // 3. ����SDA��Ƭѡ������Ϊ��ͨ�������
    GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MFRC522_GPIO_SDA_PORT, &GPIO_InitStructure);

    // ����RST����λ������Ϊ��ͨ�������
    GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MFRC522_GPIO_RST_PORT, &GPIO_InitStructure);

    // 4. ����SPI1����
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      // ����ģʽ
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  // 8λ����
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                         // ����ʱ�͵�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                       // ��һ�����ز���
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          // �������NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;// ��Ƶ16�����Ը�����Ҫ������
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 // ��λ��ǰ
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    // 5. ʹ��SPI1
    SPI_Cmd(SPI1, ENABLE);

    // 6. ƬѡĬ������
    MFRC522_SDA_H;




}

// ��MFRC522��һ���Ĵ���
unsigned char Read_MFRC522(unsigned char Address)
{
    unsigned char ucAddr;
    unsigned char ucResult;
    uint32_t time=TIM2_GetTick();

    ucAddr = ((Address << 1) & 0x7E) | 0x80; // ���������

    MFRC522_SDA_L; // ����Ƭѡ����ʼͨ��

    // ���ͼĴ�����ַ
    SPI_I2S_SendData(SPI1, ucAddr);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET&&Timeout(time,1000));
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET&&Timeout(time,1000));
    (void)SPI_I2S_ReceiveData(SPI1); // ������Ч���ݣ�������ջ���

    // ���Ϳ����ݶ�ȡ��������ֵ
    SPI_I2S_SendData(SPI1, 0x00);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET&&Timeout(time,1000));
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET&&Timeout(time,1000));
    ucResult = SPI_I2S_ReceiveData(SPI1);

    MFRC522_SDA_H; // ����Ƭѡ������ͨ��

    return ucResult;
}

// ��MFRC522дһ���Ĵ���
void Write_MFRC522(unsigned char Address, unsigned char value)
{
    unsigned char ucAddr;
    uint32_t time=TIM2_GetTick();
    ucAddr = (Address << 1) & 0x7E; // ����д����

    MFRC522_SDA_L; // ����Ƭѡ����ʼͨ��

    // ���ͼĴ�����ַ
    SPI_I2S_SendData(SPI1, ucAddr);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET&&Timeout(time,1000));
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET&&Timeout(time,1000));
    (void)SPI_I2S_ReceiveData(SPI1);

    // ����Ҫд������
    SPI_I2S_SendData(SPI1, value);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET&&Timeout(time,1000));
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET&&Timeout(time,1000));
    (void)SPI_I2S_ReceiveData(SPI1);

    MFRC522_SDA_H; // ����Ƭѡ������ͨ��
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_Reset(void) 
{
    uint32_t time=TIM2_GetTick();
	//unsigned char i;
    MFRC522_RST_H;
        Delay_Us (1);
    MFRC522_RST_L;
        Delay_Us (1);
    MFRC522_RST_H;
        Delay_Us (1);

    //MFRC522_RST_H;
    Write_MFRC522(CommandReg,0x0F); //soft reset
    while((Read_MFRC522(CommandReg) & 0x10)&&Timeout(time,1000)); //wait chip start ok

    Delay_Us(1);

    
    Write_MFRC522(ModeReg,0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
    Write_MFRC522(TReloadRegL,30);           
    Write_MFRC522(TReloadRegH,0);
    Write_MFRC522(TModeReg,0x8D);
    Write_MFRC522(TPrescalerReg,0x3E);
   Write_MFRC522(TxAutoReg,0x40);
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg,tmp | mask);  // set bit mask
}



/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & ~mask);  // clear bit mask
} 


/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pInData[IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOutData[OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
/////////////////////////////////////////////////////////////////////
char MFRC522_ToCard(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
   
    Write_MFRC522(ComIEnReg,irqEn|0x80);	//PCD_TRANSCEIVEģʽ ������ı�
    ClearBitMask(ComIrqReg,0x80);			//IRQ��©���
    Write_MFRC522(CommandReg,PCD_IDLE);  //ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);		//���FIFO Flash ��ErrReg  BufferOvfl��־
    
    for (i=0; i<InLenByte; i++)
    {   
				Write_MFRC522(FIFODataReg, pInData[i]);    //�����ݴ浽FIFO
		}
    Write_MFRC522(CommandReg, Command);   //����FIFO����
   
    
    if (Command == PCD_TRANSCEIVE)
    {    
				SetBitMask(BitFramingReg,0x80);  //����ֹͣ��ʱ��
		}
    
		n = Read_MFRC522(ComIrqReg);
    i = 1500;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    do 
    {
         n = Read_MFRC522(ComIrqReg);
			
         i--;
			
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);
	      
    if (i!=0)
    {    
         if(!(Read_MFRC522(ErrorReg)&0x1B))
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = Read_MFRC522(FIFOLevelReg);
              	lastBits = Read_MFRC522(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = Read_MFRC522(FIFODataReg);    }
            }
         }
         else
         {   
					status = MI_ERR;   
				 }
        
   }
   

   SetBitMask(ControlReg,0x80);           // stop timer now
   Write_MFRC522(CommandReg,PCD_IDLE); 
   return status;
}



//��������  
//ÿ��������ر����߷���֮��Ӧ������1ms�ļ��
void MFRC522_AntennaOn(void)
{
    unsigned char i;
    i = Read_MFRC522(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


//�ر�����
void MFRC522_AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}


//������������MF522����CRC
//���������pIndata--Ҫ����CRC�����ݪ�len--���ݳ��Ȫ�pOutData--�����CRC���
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    Write_MFRC522(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   Write_MFRC522(FIFODataReg, *(pIndata+i));   }
    Write_MFRC522(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_Halt(void)
{
    
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
	char status;
    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    unLen=status;
    return MI_OK;
}




/////////////////////////////////////////////////////////////////////
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_Request(unsigned char req_code,unsigned char *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 

   ClearBitMask(Status2Reg,0x08);
   Write_MFRC522(BitFramingReg,0x07);
   SetBitMask(TxControlReg,0x03);
 
   ucComMF522Buf[0] = req_code;

   status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
   
   if ((status == MI_OK) && (unLen == 0x10))
   {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;  

	 }
   
   return status;
}



//��    �ܣ�����ͻ��⪡��ȡѡ�п�Ƭ�Ŀ����к�
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK 
char MFRC522_Anticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    Write_MFRC522(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];

         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char MFRC522_SelectTag(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}



/////////////////////////////////////////////////////////////////////
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////               
char MFRC522_AuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
 //   memcpy(&ucComMF522Buf[2], pKey, 6); 
 //   memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = MFRC522_ToCard(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
///////////////////////////////////////////////////////////////////// 
char MFRC522_Read(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          pData[IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                  
char MFRC522_Write(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = MFRC522_ToCard(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}

#define card_size 64
ic_card_t card_arr[card_size]={0};

int8_t MFRC522_find_card(ic_card_t* card)
{
    for (int i = 0; i < card_size; ++i) {
        if(strcmp(card->id,card_arr[i].id)==0&&strcmp(card->type,card_arr[i].type)==0)
        {
            return i;
        }
    }
    return -1;
}
int8_t MFRC522_add_card(ic_card_t* card)
{
    int i=0;
    for ( i= 0; i < card_size; ++i)
    {
        if(card_arr[i].used_flag==0)
        {
            break;
        }
    }

    if(i==card_size)return -1;

    memset(card_arr[i].id,0,20);
    memset(card_arr[i].type,0,2);
    memcpy(card_arr[i].id,card->id,20);
    memcpy(card_arr[i].type,card->type,2);
    card_arr[i].used_flag=1;
    return i;
}

uint32_t uchar_to_uint(uint8_t* buffer,uint8_t size)
{
    uint32_t temp=0;
    while(size--)
    {
        temp|=*buffer++;
        temp<<=8;
    }

    return temp;


}

void MFRC522_Proc(void)
{
    unsigned char status;       //���ط�


    ic_card_t temp_card={0};
    status = MFRC522_Request(PICC_REQALL, temp_card.type);//Ѱ��
    if (status != MI_OK)
    {

           MFRC522_Reset();
           MFRC522_AntennaOff();
           MFRC522_AntennaOn();
           return;
     }


     status = MFRC522_Anticoll(temp_card.id);//����ͻ
     if (status != MI_OK)
     {
           return;
     }

     int8_t num=MFRC522_find_card(&temp_card);
     /*��ȡ��Ƭ*/
     if(num!=-1)//ʶ�𵽿�Ƭ
     {
         /*�˴����ӽ���*/




     }
     else //δ����˿�Ƭ
    {
         if(MFRC522_flag&MFRC522_FLAG_ADD)
         {
             MFRC522_flag&=~MFRC522_FLAG_ADD;
             MFRC522_add_card(&temp_card);
         }
    }





      MFRC522_Halt();

}

