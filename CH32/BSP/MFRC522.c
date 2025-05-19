#include "MFRC522.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	MFRC522射频模块c文件                   
 * 版本			: V1.0
 * 日期			: 2024.9.2
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

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

    // 1. 开启GPIO和SPI1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_SPI1, ENABLE);

    // 2. 配置 SPI引脚 (SCK, MISO, MOSI)
    // SCK -> PA5, MOSI -> PA7，推挽复用输出
    GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_SCK_PIN | MFRC522_GPIO_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(MFRC522_GPIO_SCK_PORT, &GPIO_InitStructure);

    // MISO -> PA6，浮空输入
    GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(MFRC522_GPIO_MISO_PORT, &GPIO_InitStructure);

    // 3. 配置SDA（片选）引脚为普通推挽输出
    GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MFRC522_GPIO_SDA_PORT, &GPIO_InitStructure);

    // 配置RST（复位）引脚为普通推挽输出
    GPIO_InitStructure.GPIO_Pin = MFRC522_GPIO_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MFRC522_GPIO_RST_PORT, &GPIO_InitStructure);

    // 4. 配置SPI1参数
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      // 主机模式
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  // 8位数据
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                         // 空闲时低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                       // 第一个边沿采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          // 软件管理NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;// 分频16（可以根据需要调整）
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 // 高位在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    // 5. 使能SPI1
    SPI_Cmd(SPI1, ENABLE);

    // 6. 片选默认拉高
    MFRC522_SDA_H;




}

// 向MFRC522读一个寄存器
unsigned char Read_MFRC522(unsigned char Address)
{
    unsigned char ucAddr;
    unsigned char ucResult;
    uint32_t time=TIM2_GetTick();

    ucAddr = ((Address << 1) & 0x7E) | 0x80; // 构造读命令

    MFRC522_SDA_L; // 拉低片选，开始通信

    // 发送寄存器地址
    SPI_I2S_SendData(SPI1, ucAddr);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET&&Timeout(time,1000));
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET&&Timeout(time,1000));
    (void)SPI_I2S_ReceiveData(SPI1); // 读出无效数据，清除接收缓冲

    // 发送空数据读取真正返回值
    SPI_I2S_SendData(SPI1, 0x00);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET&&Timeout(time,1000));
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET&&Timeout(time,1000));
    ucResult = SPI_I2S_ReceiveData(SPI1);

    MFRC522_SDA_H; // 拉高片选，结束通信

    return ucResult;
}

// 向MFRC522写一个寄存器
void Write_MFRC522(unsigned char Address, unsigned char value)
{
    unsigned char ucAddr;
    uint32_t time=TIM2_GetTick();
    ucAddr = (Address << 1) & 0x7E; // 构造写命令

    MFRC522_SDA_L; // 拉低片选，开始通信

    // 发送寄存器地址
    SPI_I2S_SendData(SPI1, ucAddr);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET&&Timeout(time,1000));
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET&&Timeout(time,1000));
    (void)SPI_I2S_ReceiveData(SPI1);

    // 发送要写的数据
    SPI_I2S_SendData(SPI1, value);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET&&Timeout(time,1000));
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET&&Timeout(time,1000));
    (void)SPI_I2S_ReceiveData(SPI1);

    MFRC522_SDA_H; // 拉高片选，结束通信
}


/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
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

    
    Write_MFRC522(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    Write_MFRC522(TReloadRegL,30);           
    Write_MFRC522(TReloadRegH,0);
    Write_MFRC522(TModeReg,0x8D);
    Write_MFRC522(TPrescalerReg,0x3E);
   Write_MFRC522(TxAutoReg,0x40);
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg,tmp | mask);  // set bit mask
}



/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & ~mask);  // clear bit mask
} 


/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
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
   
    Write_MFRC522(ComIEnReg,irqEn|0x80);	//PCD_TRANSCEIVE模式 无命令改变
    ClearBitMask(ComIrqReg,0x80);			//IRQ开漏输出
    Write_MFRC522(CommandReg,PCD_IDLE);  //取消当前命令
    SetBitMask(FIFOLevelReg,0x80);		//清除FIFO Flash 内ErrReg  BufferOvfl标志
    
    for (i=0; i<InLenByte; i++)
    {   
				Write_MFRC522(FIFODataReg, pInData[i]);    //把数据存到FIFO
		}
    Write_MFRC522(CommandReg, Command);   //发送FIFO内容
   
    
    if (Command == PCD_TRANSCEIVE)
    {    
				SetBitMask(BitFramingReg,0x80);  //立即停止定时器
		}
    
		n = Read_MFRC522(ComIrqReg);
    i = 1500;//根据时钟频率调整，操作M1卡最大等待时间25ms
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



//开启天线  
//每次启动或关闭天线发射之间应至少有1ms的间隔
void MFRC522_AntennaOn(void)
{
    unsigned char i;
    i = Read_MFRC522(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


//关闭天线
void MFRC522_AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}


//功能描述用MF522计算CRC
//输入参数pIndata--要读数CRC的数据len--数据长度pOutData--计算的CRC结果
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
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
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
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
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



//功    能：防冲突检测读取选中卡片的卡序列号
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK 
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
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
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
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
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
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
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
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
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
    unsigned char status;       //返回符


    ic_card_t temp_card={0};
    status = MFRC522_Request(PICC_REQALL, temp_card.type);//寻卡
    if (status != MI_OK)
    {

           MFRC522_Reset();
           MFRC522_AntennaOff();
           MFRC522_AntennaOn();
           return;
     }


     status = MFRC522_Anticoll(temp_card.id);//防冲突
     if (status != MI_OK)
     {
           return;
     }

     int8_t num=MFRC522_find_card(&temp_card);
     /*读取卡片*/
     if(num!=-1)//识别到卡片
     {
         /*此处增加解析*/




     }
     else //未保存此卡片
    {
         if(MFRC522_flag&MFRC522_FLAG_ADD)
         {
             MFRC522_flag&=~MFRC522_FLAG_ADD;
             MFRC522_add_card(&temp_card);
         }
    }





      MFRC522_Halt();

}

