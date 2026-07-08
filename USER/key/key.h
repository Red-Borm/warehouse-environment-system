/*KEY1,KEY2,KEY3,KEY4分别接在PC4-PC7,低电平有效*/

#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY1  1
#define KEY2  2
#define KEY3  3
#define KEY4  4

#define NO_KEYPRESS 0

void KeyInit(void);//按键初始化
uint8_t KeyScan(void);//按键扫描函数

#endif/*__KEY_H*/