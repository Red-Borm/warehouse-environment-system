//串口一用于开发板进行通信(PA9,PA10)
#include "uart.h"


uint8_t strBuff[32]={0};
uint8_t idx=0;
uint8_t  rxFlag=0;   //接收完成标志


void UART1_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;//定义结构体变量
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//开启时钟
    
    //串口一的GPIO初始化
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;  //发送线
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);//初始化PA端口
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;  //接收线
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);//初始化PA端口
    
    //初始化串口的通信参数
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART1, &USART_InitStruct);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);   //使能串口
    
    //中断控制器初始化
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

//串口接收中断处理函数
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        strBuff[idx] = USART_ReceiveData(USART1);  //接收数据
        
        if(strBuff[idx] != '#')
        {
            idx++;
        }
        else
        {  
            //接收完成
            strBuff[idx] = '\0';
            printf("%s", strBuff);  //将接收到的字符串再发回去
            rxFlag = 1;
            idx = 0;
        }
    }
    
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}

//重定向printf()的串口
int fputc(int ch, FILE* f)
{
    USART_SendData(USART1, (uint16_t)ch);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    return ch;
}
