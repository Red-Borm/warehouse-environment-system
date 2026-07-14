#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

/* ADC 转换结果数组 [0]=光照传感器, [1]=MQ2 烟雾传感器 */
extern volatile uint16_t ADC_ConvertedValue[2];

/* 初始化 ADC1 + DMA（双通道连续扫描：CH11 光照，CH12 气体） */
void ADC1_Init(void);

#endif
