#ifndef __LCD1602_H
#define __LCD1602_H

#include "stdint.h"
#include "sys.h"

/* LCD1602 控制引脚定义 */
#define LCD1602_EN   PAout(1)  /* EN 使能信号（PA1），高电平有效 */
#define LCD1602_RW   PAout(2)  /* RW 读写选择（PA2），高读低写 */
#define LCD1602_RS   PAout(3)  /* RS 寄存器选择（PA3），高数据低指令 */

/* 初始化 LCD1602（8位数据模式，通过 PE7~PE15 连接） */
void Lcd1602Init(void);

/* 向 LCD 数据总线写入一个字节 */
void Lcd1602WriteBusData(uint8_t dat);

/* 等待 LCD 空闲（检测忙标志） */
void Lcd1602WaitBusy(void);

/* 向 LCD 写入命令 */
void Lcd1602WriteCommand(uint8_t Command);

/* 向 LCD 写入数据 */
void Lcd1602WriteData(uint8_t data);

/* 清屏 */
void Lcd1602ClearFull(void);

/* 在指定位置显示字符串（line=1~2, list=0~16） */
void Lcd1602ShowString(uint8_t line, uint8_t list, uint8_t *s, uint8_t len);

/* 在指定位置显示整数（mode=0 补空格, mode=1 补0） */
void Lcd1602ShowInt(uint8_t line, uint8_t list, uint8_t dat, uint8_t mode);

#endif
