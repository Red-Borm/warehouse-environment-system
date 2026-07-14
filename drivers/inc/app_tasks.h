#ifndef __APP_TASKS_H
#define __APP_TASKS_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* --------------------------------------------------------- */
/* IPC 对象声明（在 main.c 中创建）                            */
/* --------------------------------------------------------- */
extern QueueHandle_t xUARTCommandQueue;   /* 串口命令队列 */
extern TaskHandle_t  xControlTaskHandle;  /* ControlTask 句柄（用于 DK1 通知） */

/* --------------------------------------------------------- */
/* 任务函数声明                                                */
/* --------------------------------------------------------- */
void vSensorTask(void *pvParameters);     /* 传感器采集任务 */
void vControlTask(void *pvParameters);    /* 主控制逻辑任务 */
void vDisplayTask(void *pvParameters);    /* LCD 显示任务 */
void vKeyTask(void *pvParameters);        /* 按键扫描任务 */
void vLogTask(void *pvParameters);        /* 串口日志输出任务 */

/* --------------------------------------------------------- */
/* 应用公共类型定义                                            */
/* --------------------------------------------------------- */
typedef enum {
    MODE_REMOTE = 0,
    MODE_AUTO   = 1,
    MODE_MANUAL = 2
} ModeType;

typedef struct {
    float       temperature_c;
    int8_t      humidity_pct;          /* -1 表示读取失败 */
    uint8_t     light_pct;
    uint8_t     gas_pct;
    uint8_t     valid_mask;            /* bit0=温度, bit1=湿度, bit2=光照, bit3=气体 */
    TickType_t  sample_tick;
} SensorSnapshot;

#endif /* __APP_TASKS_H */
