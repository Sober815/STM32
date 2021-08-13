#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"


#define KEY0 	 PAin(5)   	
#define KEY1	 PAin(6)	 
#define KEY2 	 PAin(7)	 
 
 
 
 
#define KEY0_PRES	1		//KEY0  
#define KEY1_PRES	2		//KEY1 
#define KEY2_PRES	3		//WK_UP  


void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);  	//按键扫描函数					    
#endif



