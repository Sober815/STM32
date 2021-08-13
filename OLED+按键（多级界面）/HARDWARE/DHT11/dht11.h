#ifndef __DHT11_H
#define __DHT11_H
#include "sys.h"

#define DATA_IO_IN()  {GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=8<<0;}
#define DATA_IO_OUT() {GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=3<<0;}


#define DATA_IO      PAout(0)
#define DATA_IO_READ PAin(0)
typedef struct   
{
	u8 temperature[2];  //temperature
	u8 humidity[2];     //humidity
	u16 checksum;       //checksum
}DHT11_TypeDef;
#define DHT11 dht11;


void DHT11_Init(void);
	;
 void DHT11_Ack(void);
u8 DHT11_Check(void) ;
u8 DHT11_Read_Bit(void) 	;
 u8 DHT11_Read_Byte(void)  ;
 u8 DHT11_Read_Data(u8 *temp,u8 *humi);
#endif
