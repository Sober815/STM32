#include "rtc.h"


//����RTC��׼�ṹ��
struct RTC_CLOCK rtc_clock; 
 
/*
��������: RTC��ʼ������
*/

static void RTC_NVIC_Config(void)
{	
    NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTCȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�1λ,�����ȼ�3λ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ�ܸ�ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);		//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}
void RTC_Init(void)
{
	 if(BKP->DR1!=0xAB) //��ʾRTC��һ�γ�ʼ��
	 {
			//1. ���ݼĴ���ʱ��
			RCC->APB1ENR|=1<<27; //����ʱ�ӽӿ�
			RCC->APB1ENR|=1<<28; //��Դʱ�ӽӿ�
			PWR->CR|=1<<8; 			 //����д��RTC�ͺ󱸼Ĵ���
		  
		  //2. ����RTCʱ��Դ
			RCC->BDCR|=1<<0;     //�����ⲿ32.768Kʱ��
		  while(!(RCC->BDCR&1<<1)){} //�ȴ�ʱ�Ӿ���
			RCC->BDCR&=~(0x3<<8);  //���ʱ������
			RCC->BDCR|=0x1<<8;     //ѡ���ⲿ32.768Kʱ��
			
			//3. ����RTC���ļĴ���
			RCC->BDCR|=1<<15;     //����RTCʱ��
      while(!(RTC->CRL&1<<5)){} //�ж���һ�μĴ����Ƿ�д���
			RTC->CRL|=1<<4;  //��������ģʽ
			RTC->PRLH=0;      //Ԥ��Ƶ��λ
			RTC->PRLL=0x7FFF; //32767	Ԥ��Ƶ��λ
			RTC->CNTH=0;      //��������λ
			RTC->CNTL=0;      //��������λ
			RTC->ALRH=0;      //���ӼĴ�����λ
			RTC->ALRL=60;      //���ӼĴ�����λ
			RTC->CRL&=~(1<<4);//�˳�����ģʽ	
			while(!(RTC->CRL&1<<5)){} //�ж���һ�μĴ����Ƿ�д���
			BKP->DR1=0xAB; 		//��ʾ���óɹ���	
		}
 
		RTC->CRH|=1<<0; //���ж�
		RTC->CRH|=1<<1; //�����ж�

		RTC_NVIC_Config();//RCT�жϷ�������		 
}
 
extern void Update_FrameShow(void);
/*
��������: RTC�����жϷ�����
*/
void RTC_IRQHandler(void)
{
	  u32 SecCnt;
		if(RTC->CRL&1<<0)
		{
				SecCnt=RTC->CNTH<<16;//��ȡ��λ
				SecCnt|=RTC->CNTL;   //��ȡ��λ
				RTC_GetTime(SecCnt); //ת����׼ʱ��
				RTC_GetWeek(SecCnt);
			 // printf("%d-%d-%d %d:%d:%d week:%d\n",rtc_clock.year,rtc_clock.mon,rtc_clock.day,rtc_clock.hour,rtc_clock.min,rtc_clock.sec,rtc_clock.week);
				Update_FrameShow(); //������ʾ
				RTC->CRL&=~(1<<0); //������жϱ�־λ
		}
		
		if(RTC->CRL&1<<1)
		{
//				printf("����ʱ�䵽��!....\n");
//			  BEEP=1;
//			  DelayMs(500);
//				BEEP=0;
				RTC->CRL&=~(1<<1); //��������жϱ�־λ
		}
}
 
 
 
//������·�
static int mon_r[12]={31,29,31,30,31,30,31,31,30,31,30,31};
//ƽ����·�
static int mon_p[12]={31,28,31,30,31,30,31,31,30,31,30,31};
 
 
/*
��������: ����RTCʱ��
�����β�:
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
	  u32 SecCnt=0; //������
		/*1. �ۼ��Ѿ���ȥ�����*/
		for(i=2017;i<year;i++)  //��׼���:20170101000000
	  {
				if(RTC_GetYearState(i))
				{
						SecCnt+=366*24*60*60; //����һ�������
				}
				else
				{
						SecCnt+=365*24*60*60; //ƽ��һ�������
				}
		}
		/*2. �ۼӹ�ȥ���·�*/
		for(i=0;i<mon-1;i++)
		{
			  if(RTC_GetYearState(year))
				{
						SecCnt+=mon_r[i]*24*60*60; //����һ�µ�����
				}
				else
				{
						SecCnt+=mon_p[i]*24*60*60; //ƽ��һ�µ�����
				}	
		}
		
		/*3. �ۼӹ�ȥ������*/
		SecCnt+=(day-1)*24*60*60;
		
		/*4. �ۼӹ�ȥСʱ*/
		SecCnt+=hour*60*60;
		
		/*5. �ۼӹ�ȥ�ķ���*/
		SecCnt+=min*60;
		
		/*6. �ۼӹ�ȥ����*/
		SecCnt+=sec;
		
		/*7. ����RTCʱ��*/
		RCC->APB1ENR|=1<<27; //����ʱ�ӽӿ�
		RCC->APB1ENR|=1<<28; //��Դʱ�ӽӿ�
		PWR->CR|=1<<8; 			 //����д��RTC�ͺ󱸼Ĵ���
		while(!(RTC->CRL&1<<5)){} //�ж���һ�μĴ����Ƿ�д���
		RTC->CRL|=1<<4;           //��������ģʽ
		RTC->CNTH=SecCnt>>16;     //��������λ
		RTC->CNTL=SecCnt&0xFFFF;  //��������λ
		RTC->CRL&=~(1<<4);//�˳�����ģʽ	
		while(!(RTC->CRL&1<<5)){} //�ж���һ�μĴ����Ƿ�д���
}
 
 
/*
��������: ��ȡRTCʱ��
��������: u32 sec �뵥λʱ��
*/
void RTC_GetTime(u32 sec)
{
		u32 i;
		rtc_clock.year=2017; //��׼���
		
	  /*1. ���㵱ǰ�����*/
	  while(1)
		{
			  if(RTC_GetYearState(rtc_clock.year))
				{
						if(sec>=366*24*60*60) //��һ��
						{
								sec-=366*24*60*60;
								rtc_clock.year++;
						}
						else break;
				}
				else
				{
						if(sec>=365*24*60*60) //��һ��
						{
								sec-=365*24*60*60;
								rtc_clock.year++;
						}
						else break;
				}
		}
		
		/*2. ���㵱ǰ���·�*/
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
		
		/*3. ���㵱ǰ������*/
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
		
		/*4. ���㵱ǰ��Сʱ*/
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
		
		/*5. ���㵱ǰ�ķ���*/
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
		
		/*6. ���㵱ǰ����*/
		rtc_clock.sec=sec;
}
 
 
/*
��������: �ж�����Ƿ���ƽ�ꡢ����
����ֵ  : 0��ʾƽ�� 1��ʾ����
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
��������: ��ȡ����
*/
void RTC_GetWeek(u32 sec)
{
	u32 day1=sec/(60*60*24); //���뵥λʱ��תΪ����
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
 



