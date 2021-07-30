#include "iic.h"
#include "delay.h"
#include "codetab.h"
uint8_t OLED_Diagrams[8][128];
extern const unsigned char LOGO[16][27];

void IIC_Init(void)
{
GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ; //�������
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	IIC_SCL=1;
	IIC_SDA=1;


}

void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA=1;
	IIC_SCL=1;
	delay_us(4);
	IIC_SDA=0;
	delay_us(4);
	IIC_SCL=0;
	
}


void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SDA=0;
	IIC_SCL=0;
	delay_us(4);
	IIC_SCL=1;
	IIC_SDA=1;
	delay_us(4);

}

u8 IIC_Wait_Ack(void)
{
u8 ucErrTime=0;
SDA_IN(); //SDA ����Ϊ����
IIC_SDA=1;delay_us(1);
IIC_SCL=1;delay_us(1);
while(READ_SDA)
{
	ucErrTime++;
	if(ucErrTime>250)
	
	IIC_Stop();
	return 1;
	}

	IIC_SCL=0;//ʱ����� 0
	return 0;
} 
void IIC_Ack(void)
{	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
void IIC_WriteByte(u8 data)
{
    u8 i;
    SDA_OUT();
    for(i=0;i<8;i++)
    {
        IIC_SCL=0;
        delay_us(2);
        if(data & 0x80)     //MSB(�����Чλ),
            IIC_SDA=1;		//�Ӹ�λ��ʼһλһλ����
        else
			IIC_SDA=0;
        
		
		IIC_SCL=1;
        delay_us(2);
        IIC_SCL=0;
        data<<=1;

    }
}

u8 IIC_ReadByte(void)
{
    u8 data,i;
    IIC_SDA=1;
    delay_us(2);
    for(i=0;i<8;i++)
    {
        data<<=1;
        IIC_SCL=0;
        delay_us(2);
        IIC_SCL=1;
        delay_us(2);
		if(READ_SDA)	//if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))
            data=data | 0x01;
        else 
            data=data & 0xFE;

    }
    IIC_SCL=0;
    delay_us(2);
    return data;

}
void WriteCmd(u8 command)
{
    IIC_Start();
    IIC_WriteByte(0x78);//OLED��ַ
    IIC_Wait_Ack();
    IIC_WriteByte(0x00);//�Ĵ�����ַ
    IIC_Wait_Ack();
    IIC_WriteByte(command);
    IIC_Wait_Ack();
    IIC_Stop();
}
void WriteDat(u8 data)
{
    IIC_Start();
    IIC_WriteByte(0x78);//OLED��ַ
    IIC_Wait_Ack();
    IIC_WriteByte(0x40);//�Ĵ�����ַ
    IIC_Wait_Ack();
    IIC_WriteByte(data);
    IIC_Wait_Ack();
    IIC_Stop();
}
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
			{

   WriteDat(dat);
   
		}
	else {
   WriteCmd(dat);
		
	}


}
void OLED_Init(void)
{
	IIC_Init();
    delay_ms(100); //�������ʱ����Ҫ

    WriteCmd(0xAE); //display off
    WriteCmd(0x20); //Set Memory Addressing Mode    
    WriteCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    WriteCmd(0xc8); //Set COM Output Scan Direction
    WriteCmd(0x00); //---set low column address
    WriteCmd(0x10); //---set high column address
    WriteCmd(0x40); //--set start line address
    WriteCmd(0x81); //--set contrast control register
    WriteCmd(0xff); //���ȵ��� 0x00~0xff
    WriteCmd(0xa1); //--set segment re-map 0 to 127
    WriteCmd(0xa6); //--set normal display
    WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
    WriteCmd(0x3F); //
    WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    WriteCmd(0xd3); //-set display offset
    WriteCmd(0x00); //-not offset
    WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    WriteCmd(0xf0); //--set divide ratio
    WriteCmd(0xd9); //--set pre-charge period
    WriteCmd(0x22); //
    WriteCmd(0xda); //--set com pins hardware configuration
    WriteCmd(0x12);
    WriteCmd(0xdb); //--set vcomh
    WriteCmd(0x20); //0x20,0.77xVcc
    WriteCmd(0x8d); //--set DC-DC enable
    WriteCmd(0x14); //
    WriteCmd(0xaf); //--turn on oled panel
}

void OLED_ON(void)
{
    WriteCmd(0X8D);  //���õ�ɱ�
    WriteCmd(0X14);  //������ɱ�
    WriteCmd(0XAF);  //OLED����
}

/**
  * @brief  OLED_SetPos�����ù��
  * @param  x,���xλ��
    *                   y�����yλ��
  * @retval ��

  */
void OLED_OFF(void)
{
	WriteCmd(0x8D);	//��ɱ�����������
	WriteCmd(0x10);	//��ɱ�����ֵ-�رյ�ɱ�
	WriteCmd(0xAE);	//�ر���ʾ
}
void OLED_SetPos(unsigned char x, unsigned char y) //������ʼ������
{ 
    WriteCmd(0xb0+y);
    WriteCmd(((x&0xf0)>>4)|0x10);
    WriteCmd((x&0x0f)|0x01);
}

 /**
  * @brief  OLED_Fill�����������Ļ
  * @param  fill_Data:Ҫ��������
    * @retval ��
  */
void OLED_Fill(unsigned char fill_Data)//ȫ�����
{
    unsigned char m,n;
    for(m=0;m<8;m++)
    {
        WriteCmd(0xb0+m);       //page0-page1
        WriteCmd(0x00);     //low column start address
        WriteCmd(0x10);     //high column start address
        for(n=0;n<128;n++)
            {
                WriteDat(fill_Data);
            }
    }
}

void OLED_CLS(void)//����
{
    OLED_Fill(0x00);
}


u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}		
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//�õ�ƫ�ƺ��ֵ			
		if(x>Max_Column-1){x=0;y=y+2;} 
		if(Char_Size ==16)
			{
			OLED_SetPos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_SetPos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
				
			}
}
void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
//��ʾһ���ַ��Ŵ�
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
{
	while (*chr!='\0')
	{		OLED_ShowChar(x,y,*chr,Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			chr++;
	}
}
//��ʾ����
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_SetPos(x,y);	
    for(t=0;t<16;t++) 
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
				adder+=1;
     }	
		OLED_SetPos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
				adder+=1;
      }					
}
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 
void OLED_Update_Diagrams(void)//����OLED����Ķ���
{
	uint8_t page, row;
	for(page = 0; page < 8; page++)	//8ҳforѭ��
	{
		WriteCmd(0xb0+page);	//8ҳ��ַ��0~7
		WriteCmd(0x00);	//����8λ��ʼ�е�ַ�ĵ���λ
		WriteCmd(0x10);	//����8λ��ʼ�е�ַ�ĸ���λ
		for(row = 0; row < 128; row++)	//128��forѭ��
			WriteDat(OLED_Diagrams[page][row]);	//����OLEDͼ��
	}
}
/*void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode)
{
	uint8_t page, temp, line;
	if(x>127 || y>63)	return;	//������Χ���
	page = y/8;	//�õ�����ҳ��
	temp = y%8;	//�õ�����ҳ���ڵڼ���
	line = 1<<temp;	//ת��Ϊʮ������
	mode ? (OLED_Diagrams[page][x] |= line) : (OLED_Diagrams[page][x] &= ~line);	////modeΪ1��д1����֮д0
}
*/
/*void OLED_TRUTH_LOGO(uint8_t mode)
{
	uint8_t x, y, t1, temp, j;
	mode ? OLED_Fill(0) : OLED_Fill(1);
	
	for(y = 0; y <= 80; y++)	//y��Χ
		for(x = 0,j = 8; x <= 128;x = x+8,j++)	//x��Χ
		{
			temp = LOGO[y-18][j-8];
			for(t1=0;t1<8;t1++)
			{
				temp&0x80 ? OLED_DrawPoint(x+t1,y,mode) : OLED_DrawPoint(x+t1,y,!mode);
				temp<<=1;
			}
		}
	OLED_Update_Diagrams();	//OLED����ͼ��
}*/
