#ifndef __MENU_H
#define __MENU_H	 
#include "sys.h"
#include "key.h"
#include "iic.h"
#include "usart.h"
#include "delay.h"
typedef struct
   {
     u8 current;
     u8 next; //����һ��
     u8 enter; //ȷ��
    void (*current_operation)(void); //��ǰ״̬Ӧ��ִ�еĲ���
     } key_table;

void  Menu_key_set(void);
     
     
     
void fun0(void);     
void fun1(void);
void fun2(void);
void fun3(void);
void fun4(void);
     
void fun5(void);	


void meun0_func(void); 
void meun1_func(void);
void meun2_1_func(void);
void meun2_2_func(void);     
#endif



