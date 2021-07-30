#include "esp8266.h"
#include "string.h"
#include "usart.h"
#include "usart3.h"
#include "stm32f10x.h"
#include "sys.h" 
#include "delay.h"

//����wifi
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
//���ӷ�����
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
    sprintf((char*)p,"[{\"Name\":\"P1\",\"Value\":%0.1f}]\r\n",value);//��������
    u3_printf("POST /api/V1/gateway/UpdateSensors/02 HTTP/1.1\r\n");
    u3_printf("userkey: f06750bdd7db418f82ab2f0661cdc670\r\n");
    u3_printf("Host: open.lewei50.com\r\n");
    u3_printf("Content-Length: %d\r\n",strlen(p)-2); //-2����Ϊ�����/r/n
    u3_printf("Connection: close\r\n");
    u3_printf("\r\n");
    if(atk_8266_send_cmd(p,"true",20)){
        printf("���ݷ���ʧ��\r\n"); 
        lewei_send(value);//���ݷ���ʧ��һֱ��
    };
}


//ESP8266����
void ESP8266_Init(void)
{

	WIFI_connect();	
	link_server();

}




//ESP8266�˳�͸��ģʽ   ����ֵ:0,�˳��ɹ�;1,�˳�ʧ��
//����wifiģ�飬ͨ����wifiģ����������3��+��ÿ��+��֮�� ����10ms,������Ϊ���������η���+��
u8 esp8266_quit_trans(void)
{
	u8 result=1;
	u3_printf("+++");
	delay_ms(1000);					//�ȴ�500ms̫�� Ҫ1000ms�ſ����˳�
	result=atk_8266_send_cmd("AT","OK",20);//�˳�͸���ж�.
	if(result)
		printf("quit_trans failed!");
	else
		printf("quit_trans success!");
	return result;
}






//��ESP8266��������
//cmd:���͵������ַ���;ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��;waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����);1,����ʧ��
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{ 	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(esp8266_check_cmd(ack))
				{
					printf("ack:%s\r\n",(u8*)ack);
					break;//�õ���Ч���� 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 


//ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����;����,�ڴ�Ӧ������λ��(str��λ��)
u8* esp8266_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//��ESP8266��������
//cmd:���͵������ַ���;waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:�������ݺ󣬷������ķ�����֤��
u8* esp8266_send_data(u8 *cmd,u16 waittime)
{
	char temp[5];
	char *ack=temp;
	USART3_RX_STA=0;
	u3_printf("%s",cmd);	//��������
	if(waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
				ack=(char*)USART3_RX_BUF;
				printf("ack:%s\r\n",(u8*)ack);
				USART3_RX_STA=0;
				break;//�õ���Ч���� 
			} 
		}
	}
	return (u8*)ack;
} 
