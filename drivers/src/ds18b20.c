#include "ds18b20.h"
#include "delay_us.h"

/* 配置 DS18B20 数据引脚为输入模式 */
void DS18B20_IO_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/* 配置 DS18B20 数据引脚为输出模式 */
void DS18B20_IO_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/* 复位 DS18B20 */
void DS18B20_Rst(void)
{
    DS18B20_IO_OUT();         /* 切换为输出模式 */
    DS18B20_DQ_OUT = 0;       /* 拉低总线 750us */
    delay_us(750);
    DS18B20_DQ_OUT = 1;       /* 释放总线 */
    delay_us(15);
}

/* 检测 DS18B20 是否存在：返回 0=存在，1=不存在 */
uint8_t DS18B20_Check(void)
{
    uint8_t retry = 0;
    DS18B20_IO_IN();          /* 切换为输入模式 */

    /* 等待 DS18B20 拉低总线（存在脉冲） */
    while (DS18B20_DQ_IN && retry < 200)
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 200)
        return 1;             /* 无应答 */

    retry = 0;
    /* 等待 DS18B20 释放总线（拉高） */
    while (!DS18B20_DQ_IN && retry < 240)
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 240)
        return 1;             /* 超时 */

    return 0;                 /* 检测成功 */
}

/* 从 DS18B20 读取一个位 */
uint8_t DS18B20_Read_Bit(void)
{
    uint8_t data;
    DS18B20_IO_OUT();
    DS18B20_DQ_OUT = 0;
    delay_us(2);
    DS18B20_DQ_OUT = 1;
    DS18B20_IO_IN();
    delay_us(12);

    if (DS18B20_DQ_IN)
        data = 1;
    else
        data = 0;

    delay_us(50);
    return data;
}

/* 从 DS18B20 读取一个字节 */
uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i, j, dat = 0;
    for (i = 1; i <= 8; i++)
    {
        j = DS18B20_Read_Bit();
        dat = (j << 7) | (dat >> 1);
    }
    return dat;
}

/* 向 DS18B20 写入一个字节 */
void DS18B20_Write_Byte(uint8_t dat)
{
    uint8_t j;
    uint8_t testb;
    DS18B20_IO_OUT();

    for (j = 1; j <= 8; j++)
    {
        testb = dat & 0x01;
        dat = dat >> 1;

        if (testb)
        {
            /* 写 1：拉低 2us 后释放 */
            DS18B20_DQ_OUT = 0;
            delay_us(2);
            DS18B20_DQ_OUT = 1;
            delay_us(60);
        }
        else
        {
            /* 写 0：拉低 60us 后释放 */
            DS18B20_DQ_OUT = 0;
            delay_us(60);
            DS18B20_DQ_OUT = 1;
            delay_us(2);
        }
    }
}

/* 启动 DS18B20 温度转换 */
void DS18B20_Start(void)
{
    DS18B20_Rst();
    DS18B20_Check();
    DS18B20_Write_Byte(0xCC);   /* 跳过 ROM */
    DS18B20_Write_Byte(0x44);   /* 启动温度转换 */
}

/* 初始化 DS18B20：返回 0=成功，1=失败 */
uint8_t DS18B20_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_11);

    DS18B20_Rst();
    return DS18B20_Check();
}

/* 读取 DS18B20 温度值，返回温度 x10（如 25.5 度返回 255） */
short DS18B20_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL, TH;
    short tem;

    DS18B20_Start();            /* 启动转换 */
    DS18B20_Rst();
    DS18B20_Check();
    DS18B20_Write_Byte(0xCC);   /* 跳过 ROM */
    DS18B20_Write_Byte(0xBE);   /* 读取暂存器 */

    TL = DS18B20_Read_Byte();   /* 温度低字节 */
    TH = DS18B20_Read_Byte();   /* 温度高字节 */

    if (TH > 7)
    {
        /* 温度为负，取补码 */
        TH = ~TH;
        TL = ~TL;
        temp = 0;                /* 负温度标志 */
    }
    else
    {
        temp = 1;                /* 正温度标志 */
    }

    tem = TH;
    tem <<= 8;
    tem += TL;

    if (temp)
    {
        /* 正温度：直接乘以 0.0625 再乘 10，得到 0.1 度分辨率 */
        tem = (float)tem * 0.0625 * 10;
    }
    else
    {
        /* 负温度：先取补码再处理 */
        tem = (float)(~(tem + 1)) * 0.0625 * 10;
    }

    return tem;
}
