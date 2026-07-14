#include "tim3.h"

int8_t direction = 0;    /* PWM 呼吸灯方向：1=增加，-1=减少 */
uint8_t dutyCycle = 0;   /* PWM 当前占空比 */

/* 初始化 TIM3 通道2（PB5）为 PWM 输出，实现呼吸灯效果 */
void TIM3_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* 使能时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* 配置引脚重映射：TIM3_CH2 -> PB5 */
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);

    /* 配置 PB5 为复用推挽输出 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* 配置 TIM3 时基：分频 7200，周期 100，PWM 频率 = 72MHz/7200/100 = 100Hz */
    TIM_TimeBaseStruct.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseStruct.TIM_Period = 100 - 1;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);

    /* 配置 PWM 通道2 */
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = dutyCycle;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC2Init(TIM3, &TIM_OCInitStruct);

    /* 配置更新中断，用于呼吸灯渐变 */
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* 使能预装载 */
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    TIM_Cmd(TIM3, ENABLE);
}

/* TIM3 更新中断：PWM 占空比渐变（呼吸灯效果） */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

        dutyCycle += direction;         /* 步进调整占空比 */
        if (dutyCycle >= 100)           /* 达到上限，调转方向 */
        {
            direction = -1;
            dutyCycle = 100;
        }
        else if (dutyCycle <= 0)        /* 达到下限，调转方向 */
        {
            direction = 1;
            dutyCycle = 0;
        }
        TIM_SetCompare2(TIM3, dutyCycle);
    }
}
