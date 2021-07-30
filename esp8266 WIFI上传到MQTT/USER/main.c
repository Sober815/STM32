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


	char buff[256];//�ϴ�����
	u8 temperture, humidity;
	int main(void)
{
		 
	
		const char *topics[] = {"/mysmarthome/sub"};
		unsigned short timeCount = 0;	//���ͼ������
		unsigned char *dataPtr = NULL;
		delay_init();	    	 			//��ʱ������ʼ��	  
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 			//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
		Usart1_Init(115200);	 				//����1��ʼ��Ϊ115200
		Usart2_Init(115200);	 				//����2��ʼ��Ϊ115200
		LED_Init();
		DHT11_Init();
		UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
		 
		ESP8266_Init();
		while(OneNet_DevLink())			//����OneNET
		delay_ms(500);
		UsartPrintf(USART_DEBUG, " onenet  OK\r\n");
		 
		
		OneNet_Subscribe(topics, 1);
		
		 
		while(1)
		{
			
		
			
						if(++timeCount >= 500)									//���ͼ��5s
					{
					
						DHT11_Read_Data(&temperture,&humidity);
						UsartPrintf(USART_DEBUG,"�¶�%d �棬ʪ��%d %%\r\n",temperture,humidity);	
							
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


