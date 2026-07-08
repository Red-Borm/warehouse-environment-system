#ifndef __TIM6_H
#define __TIM6_H

#include "stm32f10x.h"

void TIM6_Config(void);
void TIM6_IRQHandler(void);


extern uint16_t timeCount500ms;
extern uint8_t timeFlag500ms;

extern uint16_t timeCount5s;
extern uint8_t timeFlag5s;

#endif