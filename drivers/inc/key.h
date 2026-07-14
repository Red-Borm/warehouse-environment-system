#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

/* 按键编号定义（PC4~PC7，低电平有效） */
#define KEY1  1   /* PC4 模式切换键 */
#define KEY2  2   /* PC5 手动控制-风扇 */
#define KEY3  3   /* PC6 手动控制-开排气扇 */
#define KEY4  4   /* PC7 手动控制-关排气扇 */
#define NO_KEYPRESS 0  /* 无按键按下 */

/* 初始化按键 GPIO（PC4~PC7，上拉输入模式） */
void KeyInit(void);

/* 按键扫描函数，返回 KEY1~KEY4 或 NO_KEYPRESS */
uint8_t KeyScan(void);

#endif
