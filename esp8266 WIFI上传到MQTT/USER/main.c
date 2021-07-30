#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "stdio.h"
#include "esp8266.h"
#include "string.h"
#include "timer.h"
#include "led.h"
#include "dht11.h"
#include <string.h>
#include "onenet.h"


	char buff[256];//上传数据
	u8 temperture, humidity;
	int main(void)
{
		 
	
		const char *topics[] = {"/mysmarthome/sub"};
		unsigned short timeCount = 0;	//发送间隔变量
		unsigned char *dataPtr = NULL;
		delay_init();	    	 			//延时函数初始化	  
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 			//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
		Usart1_Init(115200);	 				//串口1初始化为115200
		Usart2_Init(115200);	 				//串口2初始化为115200
		LED_Init();
		DHT11_Init();
		UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
		 
		ESP8266_Init();
		while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);
		UsartPrintf(USART_DEBUG, " onenet  OK\r\n");
		 
		
		OneNet_Subscribe(topics, 1);
		
		 
		while(1)
		{
			
		
			
						if(++timeCount >= 500)									//发送间隔5s
					{
					
						DHT11_Read_Data(&temperture,&humidity);
						UsartPrintf(USART_DEBUG,"温度%d ℃，湿度%d %%\r\n",temperture,humidity);	
							
						UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
						sprintf(buff,"{\"HUM\":%d,\"TEMP\":%d}",humidity,temperture);
						OneNet_Publish("/mysmarthome/sub", buff);
						
						timeCount = 0;
						ESP8266_Clear();
					}
						
					dataPtr = ESP8266_GetIPD(3);
					if(dataPtr != NULL)
						OneNet_RevPro(dataPtr);
					
					delay_ms(10);
				
			

		}
}


