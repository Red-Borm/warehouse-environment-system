#include "tim6.h"

uint16_t timeCount500ms = 0;   /* 500ms 计数 */
uint8_t timeFlag500ms = 0;     /* 500ms 时间到标志 */

uint16_t timeCount5s = 0;      /* 5s 计数 */
uint8_t timeFlag5s = 0;        /* 5s 时间到标志 */

/* 配置 TIM6 为 1ms 周期定时器 */
void TIM6_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    /* 72MHz / 72 = 1MHz, 1000 计数 = 1ms */
    TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM6, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

/* TIM6 中断服务函数：每 1ms 触发一次 */
void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        timeCount500ms++;
        timeCount5s++;

        if (timeCount500ms >= 500)    /* 500ms 到 */
        {
            timeFlag500ms = 1;
            timeCount500ms = 0;
        }

        if (timeCount5s >= 5000)      /* 5s 到 */
        {
            timeFlag5s = 1;
            timeCount5s = 0;
        }

        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}
