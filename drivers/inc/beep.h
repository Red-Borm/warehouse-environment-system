#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h"

/* 蜂鸣器控制宏（PD14，高电平蜂鸣器响） */
#define BEEP_ON    GPIO_SetBits(GPIOD, GPIO_Pin_14)
#define BEEP_OFF   GPIO_ResetBits(GPIOD, GPIO_Pin_14)

/* 初始化蜂鸣器 IO 口（PD14，推挽输出） */
void BeepInit(void);

#endif
