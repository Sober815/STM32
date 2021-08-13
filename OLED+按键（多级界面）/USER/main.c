#include "led.h"
#include "delay.h"
#include "sys.h"
#include "iic.h"
#include "usart.h"
#include "key.h"
#include "timer.h"
#include <string.h>
#include "menu.h"




extern unsigned int HZ;
int main(void)
{	
    HZ= GB16_NUM();

	KEY_Init();
    uart_init(115200);
	LED_Init();
	delay_init();	 //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	OLED_Init();
	OLED_Update_Diagrams();
	OLED_CLS();
    
    
   printf( "init over!\n" );
    
   OLED_ShowCH(0,0,"123");
		
	


	while(1)
	{	
        
        Menu_key_set();
       
		
	}
	
	  
	  
}	  
	
