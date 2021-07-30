#ifndef __ESP8266_H
#define __ESP8266_H
#include "sys.h"
#include "stdio.h"	

//用户配置AT指令
extern char restart[];
extern char cwmode[];
extern char cwlap[];
extern char cwjap[];
extern char cifsr[];
extern char cipmux[];
extern char cipstart[];
extern char cipsend[];
extern char cipserver[];
extern char cwlif[];
extern char cipstatus[];
extern char cipsto[];
extern char cipmode[];
extern char test[];


#define wifista_ssid   "mi9"                     //路由器SSID名称
#define wifista_password   "99999999"   

//函数

u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8* esp8266_send_data(u8 *cmd,u16 waittime);
u8* esp8266_check_cmd(u8 *str);
u8 esp8266_quit_trans(void);

void ESP8266_Init(void);
void atk_8266_wifista_TCP_client(void);
void lewei_send(float value);


void esp8266_Config(void);
#endif
