#ifndef RTC_H
#define RTC_H
#include "stm32f10x.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"


 #include "stm32f10x_rtc.h"
//定时RTC时钟结构体
struct RTC_CLOCK
{
		u32 year;
	  u32 mon;
	  u32 day;
		u32 hour;
		u32 min;
	  u32 sec;
		u32 week;
};
extern struct RTC_CLOCK rtc_clock; 
 
//函数声明
void RTC_Init(void);
u8 RTC_GetYearState(u32 year);
void RTC_GetTime(u32 sec);
void RTC_GetWeek(u32 sec);
void RTC_SetTime(u32 year,u32 mon,u32 day,u32 hour,u32 min,u32 sec);



#endif

