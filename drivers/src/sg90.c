#include "sg90.h"

/* 初始化 SG90 舵机（TIM2_CH1, PA0, 50Hz PWM） */
void SG90_Init(void)
{
    /* 1. 打开 TIM2 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* 2. 打开 GPIOA 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* 3. 配置 PA0 为复用推挽输出（TIM2_CH1） */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 4. 配置 TIM2 时基：20ms 周期（50Hz） */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 20000 - 1;     /* 20ms 周期 */
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;      /* 1MHz 计数频率 */
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* 5. 配置 PWM 输出通道（CH1） */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 1500;   /* 初始 1500us，对应 90 度 */
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    /* 6. 启动定时器和 PWM 输出 */
    TIM_Cmd(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
}

/* 设置舵机角度：0~180 度 */
void Servo_SetAngle(uint16_t angle)
{
    /* 将角度映射到 1ms~2ms 的脉冲宽度 */
    /* 0 度对应 1000us，180 度对应 2000us */
    uint16_t pulse_width = (uint16_t)(1000 + (angle * 1000 / 180));

    /* 设置 PWM 脉冲宽度 */
    TIM_SetCompare1(TIM2, pulse_width);
}
