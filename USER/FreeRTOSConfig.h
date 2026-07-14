#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stm32f10x.h"

/* --------------------------------------------------------- */
/* MCU 参数配置：STM32F103C8T6                                */
/* - 主频：72MHz                                              */
/* - Flash：64KB                                              */
/* - SRAM：20KB                                               */
/* - 内核：Cortex-M3                                          */
/* --------------------------------------------------------- */

/* CPU 主频（单位 Hz），务必与 SystemInit 配置一致 */
#define configCPU_CLOCK_HZ                  72000000

/* 系统时钟节拍频率：1000 = 1ms 一个 tick */
#define configTICK_RATE_HZ                  1000

/* --------------------------------------------------------- */
/* 调度配置                                                                    */
/* --------------------------------------------------------- */
#define configUSE_PREEMPTION                1               /* 抢占式调度 */
#define configUSE_TIME_SLICING              1               /* 时间片轮转 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1           /* 使用硬件指令优化任务选择 */

/* --------------------------------------------------------- */
/* 钩子函数                                                                    */
/* --------------------------------------------------------- */
#define configUSE_IDLE_HOOK                 0               /* 空闲任务钩子 */
#define configUSE_TICK_HOOK                 0               /* Tick 钩子 */
#define configCHECK_FOR_STACK_OVERFLOW      2               /* 栈溢出检测（方法2） */
#define configUSE_MALLOC_FAILED_HOOK        1               /* 内存分配失败钩子 */

/* --------------------------------------------------------- */
/* 任务与堆栈                                                                  */
/* --------------------------------------------------------- */
#define configMAX_PRIORITIES                5               /* 最大优先级数（0~4） */
#define configMINIMAL_STACK_SIZE            ((unsigned short)128)    /* 最小栈大小（字） */
#define configMAX_TASK_NAME_LEN             16              /* 任务名最大长度 */

/* --------------------------------------------------------- */
/* 堆大小：C8T6 只有 20KB SRAM，谨慎分配                          */
/* heap_4 的 8KB 堆 ≈ 可创建 4~6 个简单任务 + 队列/信号量          */
/* 如果编译后 RAM 不够，调小此值或关掉 unused 功能                   */
/* --------------------------------------------------------- */
#define configTOTAL_HEAP_SIZE               ((size_t)(8 * 1024))

/* 16 位 tick 计数器（Cortex-M3 是 32 位，设为 0） */
#define configUSE_16_BIT_TICKS              0

/* 空闲任务让出 CPU 给同优先级任务 */
#define configIDLE_SHOULD_YIELD             1

/* --------------------------------------------------------- */
/* IPC 功能（按需开启，默认开常用项）                                            */
/* --------------------------------------------------------- */
#define configUSE_MUTEXES                   1
#define configUSE_RECURSIVE_MUTEXES         1
#define configUSE_COUNTING_SEMAPHORES       1
#define configUSE_QUEUE_SETS               0
#define configUSE_TASK_NOTIFICATIONS       1

/* --------------------------------------------------------- */
/* 软件定时器                                                                    */
/* --------------------------------------------------------- */
#define configUSE_TIMERS                    1
#define configTIMER_TASK_PRIORITY           2               /* 定时器任务优先级 */
#define configTIMER_QUEUE_LENGTH            10              /* 定时器命令队列长度 */
#define configTIMER_TASK_STACK_DEPTH        256             /* 定时器任务栈大小 */

/* --------------------------------------------------------- */
/* 中断优先级配置（关键！）                                                     */
/* STM32F103 使用 4 位优先级（NVIC_PriorityGroup_4）                              */
/* 0 = 最高优先级，15 = 最低优先级                                                */
/* --------------------------------------------------------- */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

/* 内核中断优先级（最低优先级，用于 SysTick/PendSV/SVC） */
#define configKERNEL_INTERRUPT_PRIORITY                 (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << 4)

/* FreeRTOS API 可安全调用的最高中断优先级 */
/* 优先级数字 >= 5 的中断才能调用 FromISR 结尾的 API */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << 4)

/* --------------------------------------------------------- */
/* INCLUDE 开关                                                                    */
/* --------------------------------------------------------- */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xTaskResumeFromISR              1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xSemaphoreGetMutexClient        1
#define INCLUDE_xTimerPendFunctionCall          1

/* --------------------------------------------------------- */
/* 断言宏（用于调试）                                                */
/* --------------------------------------------------------- */
#define configASSERT_DEFINED                   0
#if (configASSERT_DEFINED == 1)
extern void vAssertCalled(const char *pcFile, unsigned long ulLine);
#define configASSERT(x) if((x) == 0) vAssertCalled(__FILE__, __LINE__)
#endif

#endif /* FREERTOS_CONFIG_H */
