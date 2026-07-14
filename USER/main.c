#include "stm32f10x.h"
#include "app_tasks.h"
#include "delay_us.h"
#include "led.h"
#include "beep.h"
#include "uart.h"
#include "tim3.h"
#include "adc.h"
#include "lcd1602.h"
#include "ds18b20.h"
#include "dht11_driver.h"
#include "key.h"
#include "keyint.h"
#include "motor.h"
#include "sg90.h"

/* --------------------------------------------------------- */
/* IPC 对象定义                                                */
/* --------------------------------------------------------- */
QueueHandle_t xUARTCommandQueue = NULL;
TaskHandle_t  xControlTaskHandle = NULL;

/* --------------------------------------------------------- */
/* FreeRTOS 钩子函数                                          */
/* --------------------------------------------------------- */
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    while (1);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    while (1);
}

/* --------------------------------------------------------- */
/* main                                                       */
/* --------------------------------------------------------- */
int main(void)
{
    /* ---- 1. 中断优先级分组（必须在任何外设中断使能前设置） ---- */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* ---- 2. 硬件初始化 ---- */
    DelayUs_Init();             /* TIM4：微秒延时（DHT11/DS18B20 时序） */
    LedInit();
    BeepInit();
    UART1_Config();             /* USART1 中断优先级已在内部设为 6 */
    TIM3_PWM_Init();
    ADC1_Init();                /* ADC+DMA 连续扫描 */
    Lcd1602Init();
    DS18B20_Init();
    DHT11_GPIO_Config();
    KeyInit();
    KeyIntConfig();             /* EXTI4 中断优先级已在内部设为 6 */
    MotorInit();
    SG90_Init();
    Servo_SetAngle(0);

    /* ---- 3. 创建 IPC 对象 ---- */
    xUARTCommandQueue = xQueueCreate(4, UART_CMD_MAX_LEN);
    UART_SetCommandQueue(xUARTCommandQueue);

    /* ---- 4. 创建任务 ---- */
    xTaskCreate(vSensorTask,   "Sensor",   384, NULL, 2, NULL);
    xTaskCreate(vControlTask,  "Control",  512, NULL, 3, &xControlTaskHandle);
    xTaskCreate(vDisplayTask,  "Display",  256, NULL, 1, NULL);
    xTaskCreate(vKeyTask,      "KeyScan",  192, NULL, 1, NULL);
    xTaskCreate(vLogTask,      "Log",      256, NULL, 0, NULL);

    /* 将 ControlTask 句柄注册到 DK1 中断驱动 */
    KeyInt_SetNotifyTask(xControlTaskHandle);

    /* ---- 5. 启动调度器 ---- */
    vTaskStartScheduler();

    for (;;);
}
