#include "rtc.h"


//定义RTC标准结构体
struct RTC_CLOCK rtc_clock; 
 
/*
函数功能: RTC初始化函数
*/

static void RTC_NVIC_Config(void)
{	
    NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//先占优先级1位,从优先级3位
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//使能该通道中断
	NVIC_Init(&NVIC_InitStructure);		//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}
void RTC_Init(void)
{
	 if(BKP->DR1!=0xAB) //表示RTC第一次初始化
	 {
			//1. 备份寄存器时钟
			RCC->APB1ENR|=1<<27; //备份时钟接口
			RCC->APB1ENR|=1<<28; //电源时钟接口
			PWR->CR|=1<<8; 			 //允许写入RTC和后备寄存器
		  
		  //2. 配置RTC时钟源
			RCC->BDCR|=1<<0;     //开启外部32.768K时钟
		  while(!(RCC->BDCR&1<<1)){} //等待时钟就绪
			RCC->BDCR&=~(0x3<<8);  //清空时钟配置
			RCC->BDCR|=0x1<<8;     //选择外部32.768K时钟
			
			//3. 配置RTC核心寄存器
			RCC->BDCR|=1<<15;     //开启RTC时钟
      while(!(RTC->CRL&1<<5)){} //判断上一次寄存器是否写完成
			RTC->CRL|=1<<4;  //进入配置模式
			RTC->PRLH=0;      //预分频高位
			RTC->PRLL=0x7FFF; //32767	预分频低位
			RTC->CNTH=0;      //计数器高位
			RTC->CNTL=0;      //计数器低位
			RTC->ALRH=0;      //闹钟寄存器高位
			RTC->ALRL=60;      //闹钟寄存器低位
			RTC->CRL&=~(1<<4);//退出配置模式	
			while(!(RTC->CRL&1<<5)){} //判断上一次寄存器是否写完成
			BKP->DR1=0xAB; 		//表示配置成功了	
		}
 
		RTC->CRH|=1<<0; //秒中断
		RTC->CRH|=1<<1; //闹钟中断

		RTC_NVIC_Config();//RCT中断分组设置		 
}
 
extern void Update_FrameShow(void);
/*
函数功能: RTC闹钟中断服务函数
*/
void RTC_IRQHandler(void)
{
	  u32 SecCnt;
		if(RTC->CRL&1<<0)
		{
				SecCnt=RTC->CNTH<<16;//获取高位
				SecCnt|=RTC->CNTL;   //获取低位
				RTC_GetTime(SecCnt); //转换标准时间
				RTC_GetWeek(SecCnt);
			 // printf("%d-%d-%d %d:%d:%d week:%d\n",rtc_clock.year,rtc_clock.mon,rtc_clock.day,rtc_clock.hour,rtc_clock.min,rtc_clock.sec,rtc_clock.week);
				Update_FrameShow(); //更新显示
				RTC->CRL&=~(1<<0); //清除秒中断标志位
		}
		
		if(RTC->CRL&1<<1)
		{
//				printf("闹钟时间到达!....\n");
//			  BEEP=1;
//			  DelayMs(500);
//				BEEP=0;
				RTC->CRL&=~(1<<1); //清除闹钟中断标志位
		}
}
 
 
 
//闰年的月份
static int mon_r[12]={31,29,31,30,31,30,31,31,30,31,30,31};
//平年的月份
static int mon_p[12]={31,28,31,30,31,30,31,31,30,31,30,31};
 
 
/*
函数功能: 设置RTC时间
函数形参:
    u32 year;   2018
	  u32 mon;     8
	  u32 day;
		u32 hour;
		u32 min;
	  u32 sec;
*/
void RTC_SetTime(u32 year,u32 mon,u32 day,u32 hour,u32 min,u32 sec)
{
		u32 i;
	  u32 SecCnt=0; //总秒数
		/*1. 累加已经过去的年份*/
		for(i=2017;i<year;i++)  //基准年份:20170101000000
	  {
				if(RTC_GetYearState(i))
				{
						SecCnt+=366*24*60*60; //闰年一年的秒数
				}
				else
				{
						SecCnt+=365*24*60*60; //平年一年的秒数
				}
		}
		/*2. 累加过去的月份*/
		for(i=0;i<mon-1;i++)
		{
			  if(RTC_GetYearState(year))
				{
						SecCnt+=mon_r[i]*24*60*60; //闰年一月的秒数
				}
				else
				{
						SecCnt+=mon_p[i]*24*60*60; //平年一月的秒数
				}	
		}
		
		/*3. 累加过去的天数*/
		SecCnt+=(day-1)*24*60*60;
		
		/*4. 累加过去小时*/
		SecCnt+=hour*60*60;
		
		/*5. 累加过去的分钟*/
		SecCnt+=min*60;
		
		/*6. 累加过去的秒*/
		SecCnt+=sec;
		
		/*7. 设置RTC时间*/
		RCC->APB1ENR|=1<<27; //备份时钟接口
		RCC->APB1ENR|=1<<28; //电源时钟接口
		PWR->CR|=1<<8; 			 //允许写入RTC和后备寄存器
		while(!(RTC->CRL&1<<5)){} //判断上一次寄存器是否写完成
		RTC->CRL|=1<<4;           //进入配置模式
		RTC->CNTH=SecCnt>>16;     //计数器高位
		RTC->CNTL=SecCnt&0xFFFF;  //计数器低位
		RTC->CRL&=~(1<<4);//退出配置模式	
		while(!(RTC->CRL&1<<5)){} //判断上一次寄存器是否写完成
}
 
 
/*
函数功能: 获取RTC时间
函数参数: u32 sec 秒单位时间
*/
void RTC_GetTime(u32 sec)
{
		u32 i;
		rtc_clock.year=2017; //基准年份
		
	  /*1. 计算当前的年份*/
	  while(1)
		{
			  if(RTC_GetYearState(rtc_clock.year))
				{
						if(sec>=366*24*60*60) //够一年
						{
								sec-=366*24*60*60;
								rtc_clock.year++;
						}
						else break;
				}
				else
				{
						if(sec>=365*24*60*60) //够一年
						{
								sec-=365*24*60*60;
								rtc_clock.year++;
						}
						else break;
				}
		}
		
		/*2. 计算当前的月份*/
		rtc_clock.mon=1;
		for(i=0;i<12;i++)
		{
				if(RTC_GetYearState(rtc_clock.year))
				{
					 	if(sec>=mon_r[i]*24*60*60)
						{
							 sec-=mon_r[i]*24*60*60;
							 rtc_clock.mon++;
						}
						else break;		
				}
				else
				{
						if(sec>=mon_p[i]*24*60*60)
						{
							 sec-=mon_p[i]*24*60*60;
							 rtc_clock.mon++;
						}
						else break;	
				}
		}
		
		/*3. 计算当前的天数*/
		rtc_clock.day=1;
		while(1)
		{
				if(sec>=24*60*60)
				{
						sec-=24*60*60;
						rtc_clock.day++;
				}
				else break;
		}
		
		/*4. 计算当前的小时*/
		rtc_clock.hour=0;
		while(1)
		{
				if(sec>=60*60)
				{
						sec-=60*60;
						rtc_clock.hour++;
				}
				else break;
		}
		
		/*5. 计算当前的分钟*/
		rtc_clock.min=0;
		while(1)
		{
				if(sec>=60)
				{
						sec-=60;
						rtc_clock.min++;
				}
				else break;
		}
		
		/*6. 计算当前的秒*/
		rtc_clock.sec=sec;
}
 
 
/*
函数功能: 判断年份是否是平年、闰年
返回值  : 0表示平年 1表示闰年
*/
u8 RTC_GetYearState(u32 year)
{
	 if((year%4==0&&year%100!=0)||year%400==0)
	 {
		 return 1;
	 }
	 return 0;
}
 
 
/*
函数功能: 获取星期
*/
void RTC_GetWeek(u32 sec)
{
	u32 day1=sec/(60*60*24); //将秒单位时间转为天数
	switch(day1%7)
	{
		case 0:
			rtc_clock.week=0;
			break;
	 	case 1:
			rtc_clock.week=1;
			break;
		case 2:
			rtc_clock.week=2;
			break;
		case 3:
			rtc_clock.week=3;
			break;
		case 4:
			rtc_clock.week=4;
			break;
		case 5:
			rtc_clock.week=5;
			break;
		case 6:
			rtc_clock.week=6;
			break;
	}
}
 



