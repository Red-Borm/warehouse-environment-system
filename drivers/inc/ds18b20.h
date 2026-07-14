#ifndef __DS18B20_H
#define __DS18B20_H

#include "sys.h"

/* DS18B20 数据引脚定义（PB11） */
#define DS18B20_DQ_OUT PBout(11)  /* 输出 */
#define DS18B20_DQ_IN  PBin(11)   /* 输入 */

/* 初始化 DS18B20 并检测是否存在（返回 0=正常，1=失败） */
uint8_t DS18B20_Init(void);

/* 获取温度值，分辨率 0.1 摄氏度 */
short DS18B20_Get_Temp(void);

/* 启动温度转换 */
void DS18B20_Start(void);

/* 向 DS18B20 写入一个字节 */
void DS18B20_Write_Byte(uint8_t dat);

/* 从 DS18B20 读取一个字节 */
uint8_t DS18B20_Read_Byte(void);

/* 从 DS18B20 读取一个位 */
uint8_t DS18B20_Read_Bit(void);

/* 检测 DS18B20 是否存在（返回 0=存在） */
uint8_t DS18B20_Check(void);

/* 复位 DS18B20 */
void DS18B20_Rst(void);

#endif
