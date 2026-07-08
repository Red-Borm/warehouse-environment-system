#include "keyint.h"

uint8_t  keyFlag;

void KeyIntConfig(void)
{
    
     //定义初始化结构体变量
    GPIO_InitTypeDef  GPIO_InitStruct;
    EXTI_InitTypeDef  EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    
    
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    //外部中断
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    
    //配置GPIOC4的引脚
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;   
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;  
    
    
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
    GPIO_Init(GPIOC, &GPIO_InitStruct);
  
    //配置外部中断线
    EXTI_InitStruct.EXTI_Line = EXTI_Line4;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    
		EXTI_Init(&EXTI_InitStruct);
		
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
   
    NVIC_InitStruct.NVIC_IRQChannel=EXTI4_IRQn;
   //配置中断控制器，针对EXIT4
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
}


void EXTI4_IRQHandler(void)
{
    //检查是否触发中断
    if(EXTI_GetITStatus(EXTI_Line4) !=RESET)
    {
        keyFlag=1;
        
    }
    EXTI_ClearFlag( EXTI_Line4);
}