#ifndef __IIC_H
#define __IIC_H
#include "sys.h"
#include "stdlib.h"

//IO方向设置
#define SDA_IN()  {GPIOB->CRH&=0XFFFFFFF0;GPIOB->CRH|=8<<0;}   // B9是CRH的第2个//所以左移4个到第二个
#define SDA_OUT() {GPIOB->CRH&=0XFFFFFFF0;GPIOB->CRH|=3<<0;} 	// 1*4

//IO操作函数
#define IIC_SCL   PBout(9)//SCL
#define IIC_SDA   PBout(8)//SDA
#define READ_SDA  PBin(8)//输入SDA

 		     
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

void OLED_CLS(void);
void OLED_Fill(unsigned char fill_Data);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_OFF(void);
void OLED_ON(void);
void OLED_Init(void);
void WriteDat(u8 data);
void WriteCmd(u8 command);
void OLED_Update_Diagrams(void);
void OLED_WR_Byte(unsigned dat,unsigned cmd);

void OLED_ShowCH(u8 x, u8 y,u8 *chs);
unsigned int GB16_NUM(void);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len,u8 mode);
void OLED_ShowCH(u8 x, u8 y,u8 *chs);

#endif


