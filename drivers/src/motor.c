#include "motor.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "FreeRTOS.h"
#include "task.h"

/* 步进电机 8 拍励磁表：正转 */
uint8_t FFW[8] = {0xF1, 0xF3, 0xF2, 0xF6, 0xF4, 0xFC, 0xF8, 0xF9};

/* 步进电机 8 拍励磁表：反转 */
uint8_t FFZ[8] = {0xF9, 0xF8, 0xFC, 0xF4, 0xF6, 0xF2, 0xF3, 0xF1};

/* 初始化电机 GPIO */
void MotorInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能 GPIOA 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* 配置 PA0~PA4 为推挽输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |
                                  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 初始状态：全部停止 */
    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 |
                          GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4);
}

/* 设置步进电机励磁位（取 tobit 的低 4 位对应 PA0~PA3） */
void SetBit(uint8_t tobit)
{
    uint8_t i, temp;
    temp = tobit;
    for (i = 0; i < 4; i++)
    {
        if (temp & 0x01)
            PAout(i) = 1;
        else
            PAout(i) = 0;
        temp >>= 1;
    }
}

/* 步进电机控制：1=正转，2=反转，其他=停止 */
void StepMotorControl(uint8_t Status)
{
    uint8_t i, j;
    switch (Status)
    {
        case 1:   /* 正转 */
            for (j = 0; j < 12; j++)
            {
                for (i = 0; i < 8; i++)
                {
                    SetBit(FFW[i] & 0x1F);
                    vTaskDelay(pdMS_TO_TICKS(2));
                }
            }
            break;

        case 2:   /* 反转 */
            for (j = 0; j < 12; j++)
            {
                for (i = 0; i < 8; i++)
                {
                    SetBit(FFZ[i] & 0x1F);
                    vTaskDelay(pdMS_TO_TICKS(2));
                }
            }
            break;

        default:  /* 停止 */
            SetBit(0);
            break;
    }
}
