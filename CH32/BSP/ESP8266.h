#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "bsp.h"

#define UserName "tpC3iQF4IC"
#define ClientId "tegass"
#define OneNetPassword "version=2018-10-31&res=products%2FtpC3iQF4IC%2Fdevices%2Ftegass&et=1748574239&method=md5&sign=PHnA%2FdY0dP6XgZCxASOqGg%3D%3D"
#define SubscribeTopics "$sys/tpC3iQF4IC/tegass/#"
#define PublishTopic "$sys/tpC3iQF4IC/tegass/thing/property/post"
#define WIFIName "tegass"
#define WIFIPassword "00000000"



uint8_t ESP8266_Init(void);



#endif
