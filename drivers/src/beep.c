#include "beep.h"

/* 初始化蜂鸣器 IO 口 */
void BeepInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 使能 GPIOD 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    /* 配置 PD14 为推挽输出，速度 50MHz */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* 初始状态：蜂鸣器关闭 */
    GPIO_ResetBits(GPIOD, GPIO_Pin_14);
}
