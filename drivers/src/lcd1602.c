#include "lcd1602.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
/* delay functions are no longer needed (no blocking delays in LCD init) */

/* 初始化 LCD1602：8 位数据模式，双行显示，5x7 点阵 */
void Lcd1602Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能 GPIOA 和 GPIOE 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE, ENABLE);

    /* 配置控制引脚：PA1(EN), PA2(RW), PA3(RS) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置数据引脚：PE7~PE15 为开漏输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
                                  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* 初始电平设置 */
    GPIO_ResetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
    GPIO_SetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                       GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
                       GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

    /* LCD1602 初始化时序 */
    Lcd1602WriteCommand(0x38);   /* 8 位数据总线，双行显示，5x7 点阵 */
    Lcd1602WriteCommand(0x01);   /* 清屏 */
    Lcd1602WriteCommand(0x06);   /* 地址自动加 1，画面不移动 */
    Lcd1602WriteCommand(0x0c);   /* 显示开，光标关，闪烁关 */
}

/* 向 LCD 数据总线（PE7~PE15）写入一个字节 */
void Lcd1602WriteBusData(uint8_t dat)
{
    uint16_t temp;
    temp = dat;
    temp <<= 8;
    temp |= GPIO_ReadOutputData(GPIOE) & 0x00FF;
    GPIO_Write(GPIOE, temp);
}

/* 等待 LCD 空闲（读取忙标志，直到 BF=0） */
void Lcd1602WaitBusy(void)
{
    uint8_t temp;
    do
    {
        Lcd1602WriteBusData(0xff);
        LCD1602_EN = 0;
        LCD1602_RS = 0;       /* 选择指令寄存器 */
        LCD1602_RW = 1;       /* 读模式 */
        LCD1602_EN = 1;
        temp = (uint8_t)(GPIO_ReadInputData(GPIOE) >> 8);
        LCD1602_EN = 0;
    } while (temp & 0x80);    /* 检查忙标志位 BF */
}

/* 向 LCD 写入命令 */
void Lcd1602WriteCommand(uint8_t Command)
{
    Lcd1602WaitBusy();
    LCD1602_EN = 0;
    LCD1602_RS = 0;             /* 指令寄存器 */
    LCD1602_RW = 0;             /* 写模式 */
    Lcd1602WriteBusData(Command);
    LCD1602_EN = 1;             /* 产生使能脉冲 */
    LCD1602_EN = 0;
}

/* 向 LCD 写入数据 */
void Lcd1602WriteData(uint8_t data)
{
    Lcd1602WaitBusy();
    LCD1602_EN = 0;
    LCD1602_RS = 1;             /* 数据寄存器 */
    LCD1602_RW = 0;             /* 写模式 */
    Lcd1602WriteBusData(data);
    LCD1602_EN = 1;             /* 产生使能脉冲 */
    LCD1602_EN = 0;
}

/* 清屏 */
void Lcd1602ClearFull(void)
{
    Lcd1602WriteCommand(0x01);
}

/* 在指定位置显示字符串 */
void Lcd1602ShowString(uint8_t line, uint8_t list, uint8_t *s, uint8_t len)
{
    uint8_t temp, i;

    /* 计算 DDRAM 地址：第 1 行从 0x80 开始，第 2 行从 0xC0 开始 */
    if (line == 2)
        line = 0xC0;
    else
        line = 0x80;

    /* 限制列范围 */
    if (list > 16)
        list = 16;

    /* 设置显示起始地址 */
    Lcd1602WriteCommand(line | list);

    /* 逐个输出字符 */
    for (i = 0; i < len; i++)
    {
        temp = *(s + i);
        Lcd1602WriteData(temp);
    }
}

/* 在指定位置显示整数（3 位） */
void Lcd1602ShowInt(uint8_t line, uint8_t list, uint8_t dat, uint8_t mode)
{
    uint8_t i, y = 1, t = 1, temp;
    uint8_t p[3];

    /* 计算数字位数 */
    for (i = 0; i < 2; i++)
    {
        t *= 10;
        if (dat >= t)
            y += 1;
    }

    /* 分离各位数字 */
    p[2] = (dat % 10) + 0x30;     /* 个位 */
    t = 1;
    for (i = 2; i > 0; i--)       /* 十位、百位 */
    {
        t *= 10;
        temp = dat / t;
        if (temp >= 10)
            temp = temp % 10;
        if (temp)
            p[i - 1] = temp + 0x30;
        else
            p[i - 1] = 0x30;
    }

    /* 前导零/空格处理 */
    if (y < 3)
    {
        t = 3 - y;
        for (i = 0; i < t; i++)
        {
            if (mode)
                p[i] = 0x30;      /* 补零 */
            else
                p[i] = ' ';       /* 补空格 */
        }
    }

    /* 显示数字 */
    Lcd1602ShowString(line, list, p, 3);
}
