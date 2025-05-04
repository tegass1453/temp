#ifndef __BSP_H__
#define __BSP_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>

#include "debug.h"
#include  "tim.h"
#include "MFRC522.h"
#include "schedule.h"
#include "ringbuffer.h"
#include  "usart.h"
#include "Relay.h"
#include "ESP8266.h"
#include "RTC.h"

extern struct tm rtc_time;

#endif
