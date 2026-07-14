#ifndef __DHT11_DRIVER_H
#define __DHT11_DRIVER_H

#include "stm32f10x.h"
#include "delay_us.h"

#define HIGH  1   /* GPIO 输出高电平 */
#define LOW   0   /* GPIO 输出低电平 */

/* 设置 DHT11 数据引脚（PA8）电平 */
#define DHT11_DATA_OUT(a)  if(a) GPIO_SetBits(GPIOA, GPIO_Pin_8); \
                           else GPIO_ResetBits(GPIOA, GPIO_Pin_8)

/* 读取 DHT11 数据引脚（PA8）电平 */
#define DHT11_DATA_IN()    GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)

/* DHT11 温湿度数据结构体 */
typedef struct
{
    uint8_t humi_int;      /* 湿度整数部分 */
    uint8_t humi_deci;     /* 湿度小数部分 */
    uint8_t temp_int;      /* 温度整数部分 */
    uint8_t temp_deci;     /* 温度小数部分 */
    uint8_t check_sum;     /* 校验和 */
} DHT11_Data_TypeDef;

/* 初始化 DHT11 GPIO（PA8，推挽输出） */
void DHT11_GPIO_Config(void);

/* 读取 DHT11 温湿度数据，返回 SUCCESS 或 ERROR */
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);

#endif
