#include "dht11_driver.h"
#include "FreeRTOS.h"
#include "task.h"

/* 初始化 DHT11 数据引脚 GPIO（PA8） */
void DHT11_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_8);   /* 初始拉高 */
}

/* 将 DHT11 数据引脚切换为输入模式（带上拉） */
static void DHT11_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* 将 DHT11 数据引脚切换为输出模式（推挽） */
static void DHT11_Mode_Out_PP(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* 从 DHT11 读取一个字节数据（高位先出 MSB） */
static uint8_t Read_Byte(void)
{
    uint8_t i, temp = 0;

    for (i = 0; i < 8; i++)
    {
        /* 等待 50us 低电平结束 */
        while (DHT11_DATA_IN() == Bit_RESET);

        /* 延时 40us 后采样：26~28us 高电平 = 0，70us 高电平 = 1 */
        delay_us(40);

        if (DHT11_DATA_IN() == Bit_SET)
        {
            /* 数据位为 1：等待高电平结束 */
            while (DHT11_DATA_IN() == Bit_SET);
            temp |= (uint8_t)(0x01 << (7 - i));   /* MSB 格式 */
        }
        else
        {
            /* 数据位为 0 */
            temp &= (uint8_t)~(0x01 << (7 - i));
        }
    }
    return temp;
}

/* 读取 DHT11 温湿度数据（40bit 数据帧） */
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{
    /* 主机拉低 18ms 启动信号 */
    DHT11_Mode_Out_PP();
    DHT11_DATA_OUT(LOW);
    vTaskDelay(pdMS_TO_TICKS(18));

    /* 拉高 30us 后切换为输入 */
    DHT11_DATA_OUT(HIGH);
    delay_us(30);
    DHT11_Mode_IPU();

    /* 检测从机响应信号 */
    if (DHT11_DATA_IN() == Bit_RESET)
    {
        /* 等待 80us 低电平响应 */
        while (DHT11_DATA_IN() == Bit_RESET);

        /* 等待 80us 高电平准备 */
        while (DHT11_DATA_IN() == Bit_SET);

        /* 读取 40bit 数据：湿度整数+湿度小数+温度整数+温度小数+校验 */
        DHT11_Data->humi_int = Read_Byte();
        DHT11_Data->humi_deci = Read_Byte();
        DHT11_Data->temp_int = Read_Byte();
        DHT11_Data->temp_deci = Read_Byte();
        DHT11_Data->check_sum = Read_Byte();

        /* 恢复输出模式，拉高总线 */
        DHT11_Mode_Out_PP();
        DHT11_DATA_OUT(HIGH);

        /* 校验数据是否正确 */
        if (DHT11_Data->check_sum ==
            DHT11_Data->humi_int + DHT11_Data->humi_deci +
            DHT11_Data->temp_int + DHT11_Data->temp_deci)
            return SUCCESS;
        else
            return ERROR;
    }
    else
    {
        return ERROR;   /* 无响应 */
    }
}
