#include "sg90.h"

void SG90_Init(void) {
    // 1. 打开TIM2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // 2. 打开GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 3. 配置PC12为复用推挽输出 (TIM2_CH1)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 4. 配置定时器2生成PWM信号
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 20000 - 1;  // 频率20ms (50Hz)
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;   // 1MHz计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // 5. 配置PWM输出 - 使用TIM2_CH1
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 1500;  // 初始占空比，1500us对应90°位置
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    
    // 6. 启动定时器
    TIM_Cmd(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);  // 启用PWM输出
}

void Servo_SetAngle(uint16_t angle) {
    // 将角度映射到1ms到2ms的脉冲宽度
    // 0°对应1000us，180°对应2000us
    uint16_t pulse_width = (uint16_t)(1000 + (angle * 1000 / 180));  // 范围1ms - 2ms
    
    // 设置PWM脉冲宽度
    TIM_SetCompare1(TIM2, pulse_width);
}