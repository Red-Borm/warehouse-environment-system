#include "uart.h"

static QueueHandle_t s_commandQueue = NULL;
static char s_rxFrame[UART_CMD_MAX_LEN];
static uint8_t s_rxIndex = 0;

volatile uint16_t uartRxOverflowCount = 0;
volatile uint16_t uartQueueFullCount = 0;

void UART_SetCommandQueue(QueueHandle_t queue)
{
    s_commandQueue = queue;
}

void UART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

void USART1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t ch;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        ch = (uint8_t)USART_ReceiveData(USART1);

        if (ch == '#')
        {
            s_rxFrame[s_rxIndex] = '\0';
            if (s_commandQueue != NULL)
            {
                if (xQueueSendFromISR(s_commandQueue, s_rxFrame, &xHigherPriorityTaskWoken) != pdPASS)
                {
                    uartQueueFullCount++;
                }
            }
            s_rxIndex = 0;
        }
        else if (s_rxIndex < (UART_CMD_MAX_LEN - 1))
        {
            s_rxFrame[s_rxIndex++] = (char)ch;
        }
        else
        {
            uartRxOverflowCount++;
            s_rxIndex = 0;
        }
    }

    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int fputc(int ch, FILE *f)
{
    (void)f;
    USART_SendData(USART1, (uint16_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {
    }
    return ch;
}
