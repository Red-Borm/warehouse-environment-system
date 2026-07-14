#include "led.h"

/* 初始化 LED 指示灯 GPIO */
void LedInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 使能 GPIOA 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* 配置 PA0~PA7 为推挽输出，速度 50MHz */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                               GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |
                               GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 初始状态：所有 LED 熄灭 */
    LED1_OFF; LED2_OFF; LED3_OFF;
    LED4_OFF; LED5_OFF; LED6_OFF;
    LED7_OFF; LED8_OFF;
}
