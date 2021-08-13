#include "dht11.h"
#include "delay.h"
#include "usart.h"


DHT11_TypeDef dht11={0,0,0,0,0};


void DHT11_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能 GPIOA 时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PA0 清除之前设置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //PA0 输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	DATA_IO=1;
}

 void DHT11_Ack(void)
{
	DATA_IO_OUT(); 
	DATA_IO=0;
	delay_ms(20); 
	DATA_IO=1;
	delay_us(30);
	

}

u8 DHT11_Check(void) 	   
{  
	u8 retry=0;
	DATA_IO_IN();//SET INPUT	 
    while (DATA_IO_READ&&retry<100)//DHT11会拉低40~80us
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DATA_IO_READ&&retry<100)//DHT11拉低后会再次拉高40~80us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}





 u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DATA_IO_READ&&retry<100)//等待变为低电平
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DATA_IO_READ&&retry<100)//等待变高电平
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//等待40us
	if(DATA_IO_READ)return 1;
	else return 0;		   
}

 u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}

u8 DHT11_Read_Data(u8 *temp,u8 *humi)
{
	
	u8 i;
	u8 buf[5];
	DHT11_Ack();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)
		{
			buf[i]=DHT11_Read_Byte();
		
		}
		if((buf[1]+buf[2]+buf[3]+buf[0])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		
		}
		
	
		
	
	}else return 1;
	

return 0;



}



/*
}*/

