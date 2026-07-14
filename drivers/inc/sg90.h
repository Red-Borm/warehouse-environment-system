#ifndef __SG90_H
#define __SG90_H

#include "stm32f10x.h"

/* 初始化 SG90 舵机（TIM2_CH1, PA0, 50Hz PWM） */
void SG90_Init(void);

/* 设置舵机角度：0~180 度 */
void Servo_SetAngle(uint16_t angle);

#endif
