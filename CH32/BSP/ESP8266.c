#include "ESP8266.h"


uint8_t ESP8266_Init(void)
{

    Delay_Ms(2000);
    uart4_printf("AT+CWMODE=1");
    Delay_Ms(3000);
    uart4_printf("AT+CWDHCP=1,1");
    Delay_Ms(3000);
    uart4_printf("AT+CWJAP=\"%s\",\"%s\"",WIFIName,WIFIPassword);
    Delay_Ms(3000);
    uart4_printf("AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"",ClientId,UserName,OneNetPassword);
    Delay_Ms(5000);
    uart4_printf("AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1");
    Delay_Ms(5000);
    uart4_printf("AT+MQTTSUB=0,\"$sys/%s/%s/thing/property/set\",1",UserName,ClientId);
    Delay_Ms(5000);



    return 1;
}
