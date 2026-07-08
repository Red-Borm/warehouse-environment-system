/*±¸·Ý3
#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "beep.h"
#include "keyint.h"
#include "uart.h"
#include "tim6.h"

extern uint8_t strBuff[32];
extern uint8_t rxFlag;

 int main(void)
 {        
    delay_init();
    LedInit();
    BeepInit();
    UART1_Coinfig();
     
    printf("USART1 is OK!\n");
     
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
     while(1)
     {
         if(rxFlag==1)
         {
             if(strcmp((const char*)strBuff,"LED1_ON")==1)
             {
                 LED1_ON;
                 printf("LED1 is ON!\n");
             }
             if(strcmp((const char*)strBuff,"LED1_OFF")==1)
             {
                 LED1_OFF;
                 printf("LED1 is OFF!\n");
             }
             if(strcmp((const char*)strBuff,"BEEP_ON")==1)
             {
                 BEEP_ON;
                 printf("BEEP is ON!\n");
             }
             if(strcmp((const char*)strBuff,"BEEP_OFF")==1)
             {
                 BEEP_OFF;
                 printf("BEEP is OFF!\n");
             }
             
             rxFlag=0; 
             memset(strBuff,0,32);
         }
         delay_ms(1000);
     }   
 }
 
 */
 