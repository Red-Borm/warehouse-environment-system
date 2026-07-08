#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h"

#define BEEP_ON    GPIO_SetBits(GPIOD,GPIO_Pin_14)
#define BEEP_OFF   GPIO_ResetBits(GPIOD,GPIO_Pin_14)

void BeepInit(void);
    

#endif