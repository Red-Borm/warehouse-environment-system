#ifndef __DELAY_US_H
#define __DELAY_US_H

#include "stm32f10x.h"

/* 初始化 TIM4 为微秒延时提供硬件计时（1MHz 计数频率） */
void DelayUs_Init(void);

/* 微秒级阻塞延时（用于对时序敏感的传感器协议，如 DHT11 / DS18B20） */
void delay_us(uint32_t us);

#endif
