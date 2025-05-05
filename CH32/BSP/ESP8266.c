#include "ESP8266.h"


uint8_t ESP8266_Init(void)
{

    Delay_Ms(2000);
    uart4_printf("AT+CWMODE=1\r\n");
    Delay_Ms(3000);
    uart4_printf("AT+CWDHCP=1,1\r\n");
    Delay_Ms(3000);
    uart4_printf("AT+CWJAP=\"%s\",\"%s\"\r\n",WIFIName,WIFIPassword);
    Delay_Ms(3000);
    uart4_printf("AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",ClientId,UserName,OneNetPassword);
    Delay_Ms(5000);
    uart4_printf("AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n");
    Delay_Ms(5000);
    uart4_printf("AT+MQTTSUB=0,\"$sys/%s/%s/thing/property/set\",1\r\n",UserName,ClientId);
    Delay_Ms(5000);
    uart4_printf("AT+MQTTSUB=0,\"$sys/%s/%s/thing/property/post\",1\r\n",UserName,ClientId);
    Delay_Ms(5000);

    return 1;
}

/*AT+MQTTPUB=0,\"$sys/产品ID/设备ID/thing/property/post","{\"id\":\"123\"\,\"params\":{\"属性标识符\":{\"value\":上传的数据}}}\",0,0*/

void ESP8266_setpassword(uint8_t* string,uint8_t size)
{
    uint8_t temp[10]={0};
    for (int i = 0; i < size; ++i) {
        temp[i]=string[i];
    }


    uart4_printf("AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\"id\":\"123\",\"params\":{\"%s\":{\"value\":%s}}}\",0,0\r\n",UserName,ClientId,"password",temp);
    Delay_Ms(100);

}

void ESP8266_setenable(uint8_t flag)
{

    uart4_printf("AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"%s\\\":{\\\"value\\\":%s}}}\",0,0\r\n",UserName,ClientId,"enable",flag?"true":"false");
    Delay_Ms(100);
}


void ESP8266_settime(void)
{

    uart4_printf("AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"%s\\\":{\\\"value\\\":%ld}}}\",0,0\r\n",UserName,ClientId,"timestamp",time_count);
    Delay_Ms(100);
}

