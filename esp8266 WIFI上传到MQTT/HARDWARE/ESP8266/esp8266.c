#include "esp8266.h"
#include "string.h"
#include "usart.h"
#include "usart3.h"
#include "stm32f10x.h"
#include "sys.h" 
#include "delay.h"

//连接wifi
static void WIFI_connect(void)
{	
	delay_ms(1000);
	delay_ms(1000);
	char cmd[120];
	atk_8266_send_cmd("AT+RST","OK",600);
	atk_8266_send_cmd("AT","OK",600);
	atk_8266_send_cmd("AT+CWMODE=3","OK",200);
	atk_8266_send_cmd("AT+RST","OK",1000);
	//sprintf((char *)cmd,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);
	//atk_8266_send_cmd(cmd,"OK",1000);
	atk_8266_send_cmd("AT+CWJAP=\"mi9\",\"99999999\,1,4","WIFI GOT IP",1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	printf("connect to wifi sucessful\r\n");



}
//连接服务器
static void link_server(void)
{
	
	//"AT+CIPSTART=\"TCP\",\"192.168.1.102\",8080\r\n"
	u3_printf("AT+CIPSTART=\"TCP\",\"api.lewei50.com\",80\r\n");
	
	printf("connect to server\r\n");
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	u3_printf("AT+CIPMODE=1\r\n");
	delay_ms(1000);
	delay_ms(1000);
	u3_printf("AT+CIPSEND\r\n");
	delay_ms(1000);
	delay_ms(1000);

}
void lewei_send(float value)
{	

    char p[100]="";
    sprintf((char*)p,"[{\"Name\":\"P1\",\"Value\":%0.1f}]\r\n",value);//测试数据
    u3_printf("POST /api/V1/gateway/UpdateSensors/02 HTTP/1.1\r\n");
    u3_printf("userkey: f06750bdd7db418f82ab2f0661cdc670\r\n");
    u3_printf("Host: open.lewei50.com\r\n");
    u3_printf("Content-Length: %d\r\n",strlen(p)-2); //-2是因为后面的/r/n
    u3_printf("Connection: close\r\n");
    u3_printf("\r\n");
    if(atk_8266_send_cmd(p,"true",20)){
        printf("数据发送失败\r\n"); 
        lewei_send(value);//数据发送失败一直发
    };
}


//ESP8266配置
void ESP8266_Init(void)
{

	WIFI_connect();	
	link_server();

}




//ESP8266退出透传模式   返回值:0,退出成功;1,退出失败
//配置wifi模块，通过想wifi模块连续发送3个+（每个+号之间 超过10ms,这样认为是连续三次发送+）
u8 esp8266_quit_trans(void)
{
	u8 result=1;
	u3_printf("+++");
	delay_ms(1000);					//等待500ms太少 要1000ms才可以退出
	result=atk_8266_send_cmd("AT","OK",20);//退出透传判断.
	if(result)
		printf("quit_trans failed!");
	else
		printf("quit_trans success!");
	return result;
}






//向ESP8266发送命令
//cmd:发送的命令字符串;ack:期待的应答结果,如果为空,则表示不需要等待应答;waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果);1,发送失败
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{ 	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp8266_check_cmd(ack))
				{
					printf("ack:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 


//ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果;其他,期待应答结果的位置(str的位置)
u8* esp8266_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//向ESP8266发送数据
//cmd:发送的命令字符串;waittime:等待时间(单位:10ms)
//返回值:发送数据后，服务器的返回验证码
u8* esp8266_send_data(u8 *cmd,u16 waittime)
{
	char temp[5];
	char *ack=temp;
	USART3_RX_STA=0;
	u3_printf("%s",cmd);	//发送命令
	if(waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
				ack=(char*)USART3_RX_BUF;
				printf("ack:%s\r\n",(u8*)ack);
				USART3_RX_STA=0;
				break;//得到有效数据 
			} 
		}
	}
	return (u8*)ack;
} 
