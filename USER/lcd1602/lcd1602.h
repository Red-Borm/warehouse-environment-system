#ifndef __LCD1602_H_
#define __LCD1602_H_
#include "stdint.h"
#include "sys.h"

//LCD1602控制引脚定义
#define LCD1602_EN   PAout(1) //EN->PA1   使能信号   H：使能  L：失能
#define LCD1602_RW   PAout(2) //R/W->PA2  读/写选择  H：读   L：写
#define LCD1602_RS   PAout(3) //RS->PA3   数据/命令选择  H：数据寄存器  L：指令寄存器

void Lcd1602Init(void);
void Lcd1602WriteBusData(uint8_t dat);
void Lcd1602WaitBusy(void);
void Lcd1602WriteCommand(uint8_t Command);
void Lcd1602WriteData(uint8_t data);
void Lcd1602ClearFull(void);
void Lcd1602ShowString(uint8_t line,uint8_t list,uint8_t *s,uint8_t len);
void Lcd1602ShowInt(uint8_t line,uint8_t list,uint8_t dat,uint8_t mode);

#endif

