#ifndef __TIM6_H
#define __TIM6_H

#include "stm32f10x.h"

/* 配置 TIM6 为 1ms 周期定时器 */
void TIM6_Config(void);

/* TIM6 中断服务函数 */
void TIM6_IRQHandler(void);

/* 500ms 定时计数器和标志位 */
extern uint16_t timeCount500ms;
extern uint8_t timeFlag500ms;

/* 5s 定时计数器和标志位 */
extern uint16_t timeCount5s;
extern uint8_t timeFlag5s;

#endif
