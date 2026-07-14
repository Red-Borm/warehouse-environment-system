#include "delay_us.h"

/* 初始化 TIM4 用于微秒延时，72MHz / 72 = 1MHz，每 tick = 1us */
void DelayUs_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM4->PSC = 72 - 1;           /* 72MHz / 72 = 1MHz */
    TIM4->ARR = 0xFFFF;           /* 最大计数值，避免溢出 */
}

/* 微秒级阻塞延时（查询方式） */
void delay_us(uint32_t us)
{
    TIM4->CNT = 0;                /* 清零计数器 */
    TIM4->CR1 |= TIM_CR1_CEN;    /* 启动定时器 */
    while (TIM4->CNT < us);      /* 等待计数到达指定微秒数 */
    TIM4->CR1 &= ~TIM_CR1_CEN;   /* 停止定时器 */
}
