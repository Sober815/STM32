#include "menu.h"

void (*current_operation_index)(); //执行当前操作函数

key_table  table[56]=
{
    {0,0,1,(*fun0)},//一级界面
    
    {1,2,0,(*fun1)},//二级界面第一行
    {2,3,4,(*fun2)},//二级界面第二行
    {3,1,5,(*fun3)},//二级界面第三行
    
    {4,4,1,(*fun4)},//三级界面
    {5,5,1,(*fun5)},//三级界面
};

u8  func_index = 0;//主程序此时所在界面的索引值     
     
void  Menu_key_set(void)
{
        u8 t;
        t=KEY_Scan(1);
        delay_ms(50);
        switch(t)
        {
            
            case KEY0_PRES:
                printf("1\r\n");
                func_index=table[func_index].next;
                OLED_CLS();
                break;
            case KEY1_PRES:
                 printf("2\r\n");
                 func_index=table[func_index].enter;
                 OLED_CLS();
            break;
            case KEY2_PRES:
                 printf("3\r\n");break;
            
            default:
                delay_ms(10);
            
                
        
        
        }
   current_operation_index=table[func_index].current_operation;
  (*current_operation_index)();//执行当前操作函数
}

void fun0(void)
{

    meun0_func();


}

void fun1(void)
{
    meun1_func();
    OLED_ShowCH(0, 0, "-> " );
}

void fun2(void)
{
    meun1_func();
    OLED_ShowCH(0, 2, "-> " );
}

void fun3(void)
{
    meun1_func();
    OLED_ShowCH(0, 4, "-> " );
}

void fun4(void)
{
    meun2_1_func();
    OLED_ShowCH(0, 0, "-> " );
}

void fun5(void)
{
    meun2_2_func();
    OLED_ShowCH(0, 0, "-> " );
}




void meun0_func(void)
{
    
    OLED_ShowCH(0,0,"    MAIN MENU    ");

}

void meun1_func(void)
{
    //OLED_CLS();
    OLED_ShowCH(0,0,"   2-1 fanhui");
    OLED_ShowCH(0,2,"   2-2");
    OLED_ShowCH(0,4,"   2-3");
   
}

void meun2_1_func(void)
{
    //OLED_CLS();   
    OLED_ShowCH(0,0,"   3-1-fanhui");


}
void meun2_2_func(void)
{
   // OLED_CLS();   
    OLED_ShowCH(0,0,"   3-2-fanhui");

}


