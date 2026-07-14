#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "stdio.h"

#define UART_CMD_MAX_LEN 16

void UART1_Config(void);
void UART_SetCommandQueue(QueueHandle_t queue);

#endif
