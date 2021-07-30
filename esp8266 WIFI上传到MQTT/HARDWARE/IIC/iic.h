#ifndef __IIC_H
#define __IIC_H
#include "sys.h"

//IO方向设置
#define SDA_IN()  {GPIOA->CRL&=0X0FFFFFFF;GPIOA->CRL|=8<<12;}
#define SDA_OUT() {GPIOA->CRL&=0X0FFFFFFF;GPIOA->CRL|=3<<12;}

//IO操作函数
#define IIC_SCL   PAout(5)//SCL
#define IIC_SDA   PAout(7)//SDA
#define READ_SDA  PAin(7)//输入SDA
#include "stdlib.h"
 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	


void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_WriteByte(u8 data);
u8 IIC_ReadByte(void);

void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y, u8 *p,u8 Char_Size);	
void OLED_CLS(void);
void OLED_Fill(unsigned char fill_Data);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_OFF(void);
void OLED_ON(void);
void OLED_Init(void);
void WriteDat(u8 data);
void WriteCmd(u8 command);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode);
void OLED_Update_Diagrams(void);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 flag);
void OLED_ShowText(u8 x,u8 y,u8* str,u8 flag);
void OLED_WR_Byte(unsigned dat,unsigned cmd);
#endif


