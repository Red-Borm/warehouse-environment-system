#ifndef __TIM3_H
#define __TIM3_H

#include "stm32f10x.h"

/* 初始化 TIM3 通道2（PB5）为 PWM 输出，实现呼吸灯效果 */
void TIM3_PWM_Init(void);

#endif
