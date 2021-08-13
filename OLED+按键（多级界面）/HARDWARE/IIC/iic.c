#include "iic.h"
#include "delay.h"
#include "codetab.h"
#include "string.h"


u8 OLED_Diagrams[8][128];



unsigned int HZ=0;
//返回GB16字库里汉字个数
unsigned int GB16_NUM(void)
{
  unsigned int HZ_NUM;
  unsigned char *PT;
  PT = hz_index;
  while(*PT++ != '\0')
  {
  	 HZ_NUM++;
  }

  return HZ_NUM/2;
} 


void IIC_Init(void)
{
GPIO_InitTypeDef  GPIO_InitStructure;
 	
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ; //推挽输出
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	
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
SDA_IN(); //SDA 设置为输入
IIC_SDA=1;delay_us(1);
IIC_SCL=1;delay_us(1);
while(READ_SDA)
{
	ucErrTime++;
	if(ucErrTime>250)
	
	IIC_Stop();
	return 1;
	}

	IIC_SCL=0;//时钟输出 0
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
        if(data & 0x80)     //MSB(最高有效位),
            IIC_SDA=1;		//从高位开始一位一位传输
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
    IIC_WriteByte(0x78);//OLED地址
    IIC_Wait_Ack();
    IIC_WriteByte(0x00);//寄存器地址
    IIC_Wait_Ack();
    IIC_WriteByte(command);
    IIC_Wait_Ack();
    IIC_Stop();
}
void WriteDat(u8 data)
{
    IIC_Start();
    IIC_WriteByte(0x78);//OLED地址
    IIC_Wait_Ack();
    IIC_WriteByte(0x40);//寄存器地址
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
    delay_ms(100); //这里的延时很重要

    WriteCmd(0xAE); //display off
    WriteCmd(0x20); //Set Memory Addressing Mode    
    WriteCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    WriteCmd(0xc8); //Set COM Output Scan Direction
    WriteCmd(0x00); //---set low column address
    WriteCmd(0x10); //---set high column address
    WriteCmd(0x40); //--set start line address
    WriteCmd(0x81); //--set contrast control register
    WriteCmd(0xff); //亮度调节 0x00~0xff
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
    WriteCmd(0X8D);  //设置电荷泵
    WriteCmd(0X14);  //开启电荷泵
    WriteCmd(0XAF);  //OLED唤醒
}

/**
  * @brief  OLED_SetPos，设置光标
  * @param  x,光标x位置
    *                   y，光标y位置
  * @retval 无

  */
void OLED_OFF(void)
{
	WriteCmd(0x8D);	//电荷泵设置命令字
	WriteCmd(0x10);	//电荷泵设置值-关闭电荷泵
	WriteCmd(0xAE);	//关闭显示
}
void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
    WriteCmd(0xb0+y);
    WriteCmd(((x&0xf0)>>4)|0x10);
    WriteCmd((x&0x0f)|0x01);
}

 /**
  * @brief  OLED_Fill，填充整个屏幕
  * @param  fill_Data:要填充的数据
    * @retval 无
  */
void OLED_Fill(unsigned char fill_Data)//全屏填充
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

void OLED_CLS(void)//清屏
{
    OLED_Fill(0x00);
}


u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}		



void OLED_Update_Diagrams(void)//更新OLED上面的东西
{
	uint8_t page, row;
	for(page = 0; page < 8; page++)	//8页for循环
	{
		WriteCmd(0xb0+page);	//8页地址由0~7
		WriteCmd(0x00);	//设置8位起始列地址的低四位
		WriteCmd(0x10);	//设置8位起始列地址的高四位
		for(row = 0; row < 128; row++)	//128列for循环
			WriteDat(OLED_Diagrams[page][row]);	//更新OLED图表
	}
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
void OLED_ShowChar(u8 x, u8 y, u8 chr)
{
  u8 c = 0, i = 0;
  c = chr - ' '; //得到偏移后的值
  if(x > 130 - 1)
  {
    x = 0;
    y = y + 2;
  }
	OLED_SetPos(x, y);
	for(i = 0; i < 8; i++)
		WriteDat(zf[c * 16 + i]);
	OLED_SetPos(x, y + 1);
	for(i = 0; i < 8; i++)
		WriteDat(zf[c * 16 + i + 8]);

}
//显示个数字
//x,y :起点坐标
//len :数字的位数
//num:数值(0~4294967295);
//mode:   为1:显示0   为0:显示空格
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len,u8 mode)
{
  u8 t, temp;
  for(t = 0; t < len; t++)
  {
    temp = (num / oled_pow(10, len - t - 1)) % 10;
		if(temp == 0)
		{
			if(mode)
				OLED_ShowChar(x + 8*t, y, '0');
			else
				OLED_ShowChar(x + 8*t, y, ' ');
			continue;
		}
		else
			OLED_ShowChar(x + 8*t, y, temp + '0');
  }
}


//显示中英文字符
void OLED_ShowCH(u8 x, u8 y,u8 *chs)
{
  u32 i=0;
	u32 j;
	char* m;
	while (*chs != '\0')
	{
		if (*chs > 0xa0)				//汉字内码都是大于0xa0
		{
			for (i=0 ;i < HZ;i++)
			{	
				if(x>112)
				{
					x=0;
					y=y+2;
				}
				if ((*chs == hz_index[i*2]) && (*(chs+1) == hz_index[i*2+1]))
				{
					OLED_SetPos(x, y);
					for(j=0;j<16;j++)
						WriteDat(hz[i*32+j]);
					OLED_SetPos(x,y+1);
					for(j=0;j<16;j++)
						WriteDat(hz[i*32+j+16]);
					x +=16;
					break;
				}
			}
			chs+=2;
		}
		else
		{
			if(x>122)
			{
				x=0;
				y=y+2;
			}
			m=strchr(zf_index,*chs);
			if (m!=NULL)
			{
				OLED_SetPos(x, y);
				for(j = 0; j < 8; j++)
					WriteDat(zf[((u8)*m-' ') * 16 + j]);
				OLED_SetPos(x, y + 1);
				for(j = 0; j < 8; j++)
					WriteDat(zf[((u8)*m-' ') * 16 + j + 8]);
				x += 8;
			}
			chs++;
		}
	}
}









