#include "key.h"
#include "FreeRTOS.h"
#include "task.h"

/* 初始化按键 GPIO */
void KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 使能 GPIOC 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* 配置 PC4~PC7 为上拉输入，速度 50MHz */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/* 按键扫描函数，带消抖处理 */
uint8_t KeyScan(void)
{
    /* 检测 KEY1（PC4） */
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(10));    /* 延时消抖 */
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == 0)
        {
            return KEY1;
        }
    }

    /* 检测 KEY2（PC5） */
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(10));    /* 延时消抖 */
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 0)
        {
            return KEY2;
        }
    }

    /* 检测 KEY3（PC6） */
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(10));    /* 延时消抖 */
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) == 0)
        {
            return KEY3;
        }
    }

    /* 检测 KEY4（PC7） */
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(10));    /* 延时消抖 */
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 0)
        {
            return KEY4;
        }
    }

    return NO_KEYPRESS;   /* 无按键按下 */
}
