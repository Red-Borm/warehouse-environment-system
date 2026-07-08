/*利用定时器3的通道(PB5)输出PWM波控制PB5引脚上的LED灯的亮度2*/

#include "tim3.h"

 int8_t direction =0; //方向
 uint8_t  dutyCycle = 0; //占空比值


void TIM3_PWM_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    // 时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 配置部分重映射（TIM3_CH2->PB5）
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);

    // GPIO配置（复用推挽输出）
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 时基配置
    TIM_TimeBaseStruct.TIM_Prescaler = 7200 - 1;   // 72MHz/7200 = 10kHz
    TIM_TimeBaseStruct.TIM_Period = 100 - 1;       // 100Hz PWM频率
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);

    // PWM通道配置
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = dutyCycle;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC2Init(TIM3, &TIM_OCInitStruct);
	

    // 中断配置
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//开TIM3的更新中断
		
    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // 预装载使能
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    
    TIM_Cmd(TIM3, ENABLE);
}

//中断处理函数
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        
        dutyCycle += direction; // 调整占空比
        if(dutyCycle >= 100)    // 峰值检测
        {
            direction = -1;
            dutyCycle = 100;
        }
        else if(dutyCycle <= 0) // 谷值检测
        {
            direction = 1;
            dutyCycle = 0;
        }
        TIM_SetCompare2(TIM3, dutyCycle); // 更新PWM
    }
}		
