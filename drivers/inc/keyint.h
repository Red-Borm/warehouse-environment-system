#ifndef __KEYINT_H
#define __KEYINT_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"

/* DK1（PC4）外部中断触发标志位 */
extern uint8_t keyFlag;

/* 配置 PC4 外部中断（下降沿触发） */
void KeyIntConfig(void);
void KeyInt_SetNotifyTask(TaskHandle_t task);

#endif
