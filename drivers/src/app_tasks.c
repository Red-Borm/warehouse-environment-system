#include "app_tasks.h"
#include "adc.h"
#include "dht11_driver.h"
#include "ds18b20.h"
#include "led.h"
#include "beep.h"
#include "motor.h"
#include "sg90.h"
#include "key.h"
#include "lcd1602.h"
#include "uart.h"
#include "string.h"
#include "stdio.h"

/* --------------------------------------------------------- */
/* 全局执行器状态（仅 ControlTask 写入，其余任务只读）           */
/* --------------------------------------------------------- */
ModeType  CurrentMode  = MODE_AUTO;
uint8_t   Fan_Status   = 0;
uint8_t   Vent_Position= 0;
uint8_t   curtain_Status = 0;

/* LCD 显示缓冲区（ControlTask 写入，DisplayTask 读取） */
char Lcd_Line1[17] = "Mode: AUTO      ";
char Lcd_Line2[17] = "System Normal   ";

/* 最新传感器快照（SensorTask 写入，ControlTask 读取） */
static SensorSnapshot s_latestSensor = {0};

/* --------------------------------------------------------- */
/* 传感器快照读写接口                                          */
/* --------------------------------------------------------- */
void SensorPublish(const SensorSnapshot *snap)
{
    taskENTER_CRITICAL();
    s_latestSensor = *snap;
    taskEXIT_CRITICAL();
}

void SensorReadLatest(SensorSnapshot *snap)
{
    taskENTER_CRITICAL();
    *snap = s_latestSensor;
    taskEXIT_CRITICAL();
}

/* --------------------------------------------------------- */
/* SensorTask：每 2s 采集一次所有传感器                          */
/* --------------------------------------------------------- */
void vSensorTask(void *pvParameters)
{
    TickType_t xLastWakeTime;
    SensorSnapshot snap;
    DHT11_Data_TypeDef dht;

    (void)pvParameters;
    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        /* 2s 固定周期 */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(2000));

        /* ---- A. ADC 光照 + 气体（DMA 已自动写入） ---- */
        snap.light_pct = (uint8_t)((float)ADC_ConvertedValue[0] / 4095.0f * 100.0f);
        snap.gas_pct   = (uint8_t)(100.0f * (float)ADC_ConvertedValue[1] / 4095.0f);
        snap.valid_mask = (1 << 2) | (1 << 3);  /* 光照+气体有效 */

        /* ---- B. DHT11 温湿度 ---- */
        if (Read_DHT11(&dht) == SUCCESS)
        {
            snap.temperature_c = (float)dht.temp_int + (float)dht.temp_deci * 0.1f;
            snap.humidity_pct  = dht.humi_int;
            snap.valid_mask |= (1 << 0) | (1 << 1);  /* 温度+湿度有效 */
        }
        else
        {
            snap.humidity_pct = -1;
            /* 保留上次有效温度 */
        }

        /* ---- C. DS18B20 温度（每 4 次周期 ≈ 8s 读一次） ---- */
        {
            static uint8_t ds18b20_count = 0;
            ds18b20_count++;
            if (ds18b20_count >= 4)
            {
                short raw = DS18B20_Get_Temp();
                if (raw < 0)
                    snap.temperature_c = (float)(-raw * 0.1f);
                else
                    snap.temperature_c = (float)(raw * 0.1f);
                snap.valid_mask |= (1 << 0);
                ds18b20_count = 0;
            }
        }

        snap.sample_tick = xTaskGetTickCount();
        SensorPublish(&snap);
    }
}

/* --------------------------------------------------------- */
/* ControlTask：模式切换 + 执行器控制（最高优先级）             */
/* 由 DK1 中断通知或 UART 命令队列唤醒                          */
/* --------------------------------------------------------- */
void vControlTask(void *pvParameters)
{
    SensorSnapshot snap;
    char cmdBuf[UART_CMD_MAX_LEN];
    uint32_t notifyVal;

    (void)pvParameters;

    /* 初始状态 */
    DCColor = 0;
    Servo_SetAngle(0);
    BEEP_OFF;

    for (;;)
    {
        /* ---- A. 等待外部事件（DK1 通知或 UART 命令） ---- */
        /* 先检查 DK1 通知 */
        notifyVal = ulTaskNotifyTake(pdTRUE, 0);
        if (notifyVal > 0)
        {
            vTaskDelay(pdMS_TO_TICKS(20));   /* 消抖 */
            if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == 0)
            {
                CurrentMode = (ModeType)((CurrentMode + 1) % 3);
                /* 切换模式时关闭所有执行器 */
                DCColor = 0;
                Fan_Status = 0;
                BEEP_OFF;
                curtain_Status = 0;
            }
        }

        /* 检查 UART 命令队列 */
        if (xQueueReceive(xUARTCommandQueue, cmdBuf, 0) == pdTRUE)
        {
            if (CurrentMode == MODE_REMOTE)
            {
                if (strcmp(cmdBuf, "FAN_ON") == 0)
                {
                    DCColor = 1;
                    Fan_Status = 1;
                }
                else if (strcmp(cmdBuf, "FAN_OFF") == 0)
                {
                    DCColor = 0;
                    Fan_Status = 0;
                }
                else if (strcmp(cmdBuf, "VENT_OPEN") == 0)
                    Servo_SetAngle(0);
                else if (strcmp(cmdBuf, "VENT_CLOSE") == 0)
                    Servo_SetAngle(180);
            }
        }

        /* ---- B. 读取最新传感器快照 ---- */
        SensorReadLatest(&snap);

        /* ---- C. 各模式控制逻辑 ---- */
        switch (CurrentMode)
        {
            case MODE_REMOTE:
                sprintf(Lcd_Line1, "Mode: REMOTE    ");
                sprintf(Lcd_Line2, "Wait for Cmd    ");
                break;

            case MODE_AUTO:
            {
                /* 报警检测 */
                uint8_t alarm = 0;
                if (snap.valid_mask & (1 << 0))
                {
                    if (snap.temperature_c > 40.0f)
                    { BEEP_ON; alarm = 1; sprintf(Lcd_Line2, "Alarm: TEMP HIGH"); }
                }
                if (!alarm && (snap.valid_mask & (1 << 3)) && snap.gas_pct > 75)
                { BEEP_ON; alarm = 1; sprintf(Lcd_Line2, "Alarm: GAS HIGH "); }
                if (!alarm && (snap.valid_mask & (1 << 2)) && snap.light_pct > 60)
                { BEEP_ON; alarm = 1; sprintf(Lcd_Line2, "Alarm: LIQ LOW  "); }
                if (!alarm)
                { BEEP_OFF; sprintf(Lcd_Line2, "System Normal   "); }

                /* 排风扇（温度/气体滞回） */
                if ((snap.valid_mask & (1 << 0)) && snap.temperature_c >= 30.0f)
                    { DCColor = 1; Fan_Status = 1; }
                else if ((snap.valid_mask & (1 << 3)) && snap.gas_pct >= 60)
                    { DCColor = 1; Fan_Status = 1; }
                else if (snap.temperature_c < 28.0f && snap.gas_pct < 55)
                    { DCColor = 0; Fan_Status = 0; }

                /* 排气阀 */
                if (snap.temperature_c >= 25.0f || snap.gas_pct >= 50)
                    { Servo_SetAngle(0); Vent_Position = 1; }
                else
                    { Servo_SetAngle(180); Vent_Position = 0; }

                /* 湿度联动 */
                if (snap.humidity_pct >= 50)
                    { Servo_SetAngle(0); Vent_Position = 1; }
                if (snap.humidity_pct >= 90)
                    { DCColor = 1; Fan_Status = 1; }

                sprintf(Lcd_Line1, "Mode: AUTO      ");
                if (!alarm)
                    sprintf(Lcd_Line2, "FAN:%s VENT:%s",
                            Fan_Status ? "ON " : "OFF",
                            Vent_Position ? "OPEN" : "CLSD");
                break;
            }

            case MODE_MANUAL:
                sprintf(Lcd_Line1, "Mode: MANUAL    ");
                /* 手动模式的按键由 KeyTask 处理，此处不额外扫描 */
                sprintf(Lcd_Line2, "FAN:%s VENT:%s",
                        Fan_Status ? "ON " : "OFF",
                        Vent_Position ? "OPEN" : "CLSD");
                break;
        }

        /* ---- D. 短延时等待下一事件 ---- */
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/* --------------------------------------------------------- */
/* KeyTask：DK2~DK4 按键扫描（20ms 周期）                      */
/* 仅在 MANUAL 模式下向 ControlTask 投递命令                   */
/* --------------------------------------------------------- */
void vKeyTask(void *pvParameters)
{
    uint8_t keyVal;
    TickType_t xLastWakeTime;

    (void)pvParameters;
    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(20));

        if (CurrentMode != MODE_MANUAL)
            continue;

        keyVal = KeyScan();
        if (keyVal == KEY2)       /* DK2 切换风扇 */
        {
            if (Fan_Status == 0) { DCColor = 1; Fan_Status = 1; }
            else                 { DCColor = 0; Fan_Status = 0; }
        }
        else if (keyVal == KEY3)  /* DK3 打开排气阀 */
        {
            Servo_SetAngle(0);
            Vent_Position = 1;
        }
        else if (keyVal == KEY4)  /* DK4 关闭排气阀 */
        {
            Servo_SetAngle(180);
            Vent_Position = 0;
        }
    }
}

/* --------------------------------------------------------- */
/* DisplayTask：每 500ms 刷新 LCD                              */
/* --------------------------------------------------------- */
void vDisplayTask(void *pvParameters)
{
    TickType_t xLastWakeTime;

    (void)pvParameters;
    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));

        Lcd1602ShowString(1, 0, (uint8_t*)Lcd_Line1, 16);
        Lcd1602ShowString(2, 0, (uint8_t*)Lcd_Line2, 16);
    }
}

/* --------------------------------------------------------- */
/* LogTask：每 3s 通过串口输出当前状态                           */
/* --------------------------------------------------------- */
void vLogTask(void *pvParameters)
{
    SensorSnapshot snap;
    TickType_t xLastWakeTime;

    (void)pvParameters;
    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(3000));
        SensorReadLatest(&snap);

        printf("\r\n--- System Status ---\r\n");
        printf("Mode    : %s\r\n", CurrentMode == MODE_REMOTE ? "REMOTE" :
                                   CurrentMode == MODE_AUTO   ? "AUTO"   : "MANUAL");
        printf("Fan     : %s\r\n", Fan_Status ? "ON" : "OFF");
        printf("Vent    : %s\r\n", Vent_Position ? "OPEN" : "CLOSE");
        if (snap.valid_mask & (1 << 0))
            printf("Temp    : %.1f C\r\n", snap.temperature_c);
        if (snap.valid_mask & (1 << 1))
            printf("Humi    : %d %%\r\n", snap.humidity_pct);
        if (snap.valid_mask & (1 << 2))
            printf("Light   : %d %%\r\n", snap.light_pct);
        if (snap.valid_mask & (1 << 3))
            printf("Gas     : %d %%\r\n", snap.gas_pct);
    }
}
