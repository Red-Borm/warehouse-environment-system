#include "led.h"

void LedInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;  
    
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
  
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | 
                               GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | 
                               GPIO_Pin_6 | GPIO_Pin_7; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;      
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    
    

    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
  
    LED1_OFF; LED2_OFF; LED3_OFF; 
    LED4_OFF; LED5_OFF; LED6_OFF; 
    LED7_OFF; LED8_OFF;
}