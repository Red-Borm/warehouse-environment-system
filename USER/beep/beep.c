#include "beep.h"

void BeepInit(void)//蜂鸣器io接口初始化函数

{
GPIO_InitTypeDef  GPIO_InitStruct;//定义结构体变量
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//开时钟
    
    GPIO_InitStruct.GPIO_Pin =GPIO_Pin_14;
    GPIO_InitStruct.GPIO_Mode =GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
    

    GPIO_Init(GPIOD, &GPIO_InitStruct);//初始化PA端口

    
    GPIO_ResetBits(GPIOD,GPIO_Pin_14);
}