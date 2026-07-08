#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

extern volatile uint16_t ADC_ConvertedValue[2];

void ADC1_Init(void);

#endif