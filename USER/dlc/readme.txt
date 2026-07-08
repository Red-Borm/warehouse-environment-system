

****************实验一 LED灯的控制***************




#include "stm32f10x.h"
#include "led.h"
#include "delay.h"

 int main(void)
 {		
 	delay_init();//初始化SYSTick
	LedInit();//初始化LED的IO端口
	 
	while(1)
		{
			LED1_ON;
			LED2_OFF;
			LED3_OFF;	
			delay_ms(1000);
			
			LED1_OFF;
			LED2_ON;
			LED3_OFF;		
			delay_ms(1000);
			
			
			LED1_OFF;
			LED2_OFF;
			LED3_ON;
			delay_ms(1000);
		}
 }
 
 
 *****************************************************按键轮序****************************
 
 

#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "key.h"

 int main(void)
 {		
 	delay_init();//初始化SYSTick
	LedInit();//初始化LED的IO端口
	KeyInit();//初始化按键
	
	while (1)
	{
		if(KeyScan()==1)//如果按键1按下
		{
			GPIOA->ODR ^=GPIO_Pin_0; //LED反转状态
		}
		if(KeyScan()==2)//如果按键2按下
		{
			GPIOA->ODR ^=GPIO_Pin_1; //LED反转状态
		}
		if(KeyScan()==3)//如果按键3按下
		{
			GPIOA->ODR ^=GPIO_Pin_2; //LED反转状态
		}
		if(KeyScan()==4)//如果按键4按下
		{
			GPIOA->ODR ^=GPIO_Pin_3; //LED反转状态
		}
		delay_ms(200);
	}
	
 }
 ****************************************实验2按键中断*****************************************
 
 
 
#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "beep.h"
#include "keyint.h"


extern uint8_t keyFlag;

 int main(void)
 {		
 	delay_init();//初始化SYSTick
	LedInit();//初始化LED的IO端口
	BeepInit();
	KeyIntConfig();
	
	 while(1)
	 {
		 if(keyFlag==1)
		 {
			 keyFlag=0;
			 
			 GPIOD->ODR ^=GPIO_Pin_14;//蜂鸣器状态反转
			 
		 }
		 
		 delay_ms(100);
	 }
	 
	 
	 
	 
 }
 

************************************************************************************************8

#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "beep.h"
#include "keyint.h"
#include "uart.h"


extern uint8_t strBuff[32];
extern uint8_t rxFlag;

 int main(void)
 {		
 	delay_init();//初始化SYSTick
	LedInit();//初始化LED的IO端口
	BeepInit();
	UART1_Config();//串口初始化
	 
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
			 
			 rxFlag=0; //标志位清零
			 memset(strBuff,0,32);//字符串数组清零
		 }
		 delay_ms(1000);
	 }
	 
	 
	 
	 
 }
 
 ********************************************************************
 
 #include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "beep.h"
#include "uart.h"

extern uint8_t strBuff[32];
extern uint8_t rxFlag;   //接收完成标志

int main()
{
    delay_init();
    LedInit();
    BeepInit();
    UART1_Config();
    
    printf("USART1 is OK!\n");
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
    while(1)
    {
        if(rxFlag == 1)
        {
            if (strcmp((const char*)strBuff, "LED1_ON") == 0)
            {
                LED1_ON;
                printf("LED1 is ON!\n");
            }
            
            if (strcmp((const char*)strBuff, "LED1_OFF") == 0)
            {
                LED1_OFF;
                printf("LED1 is OFF!\n");
            }
            
            if (strcmp((const char*)strBuff, "BEEP_ON") == 0)
            {
                BEEP_ON;
                printf("BEEP is ON!\n");
            }
            
            if (strcmp((const char*)strBuff, "BEEP_OFF") == 0)
            {
                BEEP_OFF;
                printf("BEEP is OFF!\n");
            }
            
            rxFlag = 0;    //标志位置0
            memset(strBuff, 0, 32);   //清空缓冲区
        }
        
        delay_ms(500);
    }
}

*******************************************************************

#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "beep.h"
#include "uart.h"

extern uint8_t strBuff[32];
extern uint8_t rxFlag;   //接收完成标志

int main()
{
    delay_init();
    LedInit();
    BeepInit();
    UART1_Config();
    TIM6_Config();
	
	
    printf("USART1 is OK!\n");
     
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
     while(1)
     {
			 if(timeFlag==1)
			 {
					GPIOA->ODR ^=GPIO_Pin_7;
					timeFlag=0;
			 }
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

 ****************************************************************************
#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "beep.h"
#include "uart.h"
#include "tim6.h"
#include "tim3.h"
#include "adc.h"

extern uint8_t strBuff[32]; //存储接受到的字符串
extern uint8_t rxFlag;   // 接收完成标志
extern uint8_t timeFlag;
extern volatile uint16_t ADC_ConvertedValue; //存储转化结果


int main()
{
    delay_init();
    LedInit();
    BeepInit();
    UART1_Config();
    TIM6_Config();
    TIM3_PWM_Init(); // 初始化TIM3为PWM1模式
    
    printf("USART1 is OK!\n");
     
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
    while(1)
    {
        if(timeFlag == 1)  
        {
            GPIOA->ODR ^= GPIO_Pin_7;
            timeFlag = 0;
        }
        
        if(rxFlag == 1)
        {
            if(strcmp((const char*)strBuff, "LED1_ON") == 0)  
            {
                LED1_ON;
                printf("LED1 is ON!\n");
            }
            if(strcmp((const char*)strBuff, "LED1_OFF") == 0)
            {
                LED1_OFF;
                printf("LED1 is OFF!\n");
            }
            if(strcmp((const char*)strBuff, "BEEP_ON") == 0)
            {
                BEEP_ON;
                printf("BEEP is ON!\n");
            }
            if(strcmp((const char*)strBuff, "BEEP_OFF") == 0)
            {
                BEEP_OFF;
                printf("BEEP is OFF!\n");
            }
            
            rxFlag = 0; 
            memset(strBuff, 0, 32);
        }
        delay_ms(1000);
    }
}
***********************************************************************************

#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "beep.h"
#include "uart.h"
#include "tim6.h"
#include "tim3.h"
#include "adc.h"

extern uint8_t strBuff[32]; //存储接受到的字符串
extern uint8_t rxFlag;   // 接收完成标志
extern uint8_t timeFlag;
extern volatile uint16_t ADC_ConvertedValue; //存储转化结果
float lightpercent=0.0;


int main()
{
    uint16_t ADC_Value=0;//存放从内存取来的转换结果
	
	
		delay_init();
    LedInit();
    BeepInit();
    UART1_Config();
    TIM6_Config();
    TIM3_PWM_Init(); // 初始化TIM3为PWM1模式
		ADC1_Init();
		
		
     
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
    while(1)
    {
      //精确定时控制LED灯的亮灭  
			if(timeFlag == 1)  
        {
            GPIOA->ODR ^= GPIO_Pin_7;
            timeFlag = 0;
        }
        
				//上位机通过串口控制LED
        if(rxFlag == 1)
        {
            if(strcmp((const char*)strBuff, "LED1_ON") == 0)  
            {
                LED1_ON;
                printf("LED1 is ON!\n");
            }
            if(strcmp((const char*)strBuff, "LED1_OFF") == 0)
            {
                LED1_OFF;
                printf("LED1 is OFF!\n");
            }
            if(strcmp((const char*)strBuff, "BEEP_ON") == 0)
            {
                BEEP_ON;
                printf("BEEP is ON!\n");
            }
            if(strcmp((const char*)strBuff, "BEEP_OFF") == 0)
            {
                BEEP_OFF;
                printf("BEEP is OFF!\n");
            }
            
            rxFlag = 0; 
            memset(strBuff, 0, 32);
        }
				
				//光照强度传感器数据ADC采集
				
				lightpercent = 100-(float)ADC_ConvertedValue / 4095.0 * 100.0;
        printf("Light=%0.1f%%\n", lightpercent);
				
        delay_ms(1000);
    }
}
*****************************************************************************
改stm32f10x.h 119 800000 ->12000000
.c 1056 9 -> 6
***************************************************************************
 #include "stm32f10x.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "beep.h"
#include "uart.h"
#include "tim6.h"
#include "tim3.h"
#include "adc.h"
#include "lcd1602.h"
#include "string.h"
#include "oled_iic.h"
#include "ds18b20.h"
#include "motor.h"
#include <stdio.h>

extern uint8_t strBuff[32];
extern uint8_t rxFlag;
extern volatile uint16_t ADC_ConvertedValue[2];
extern uint8_t timeFlag500ms; //定时时间到0.5s的标志位
extern uint8_t timeFlag5s; //定时时间到5s的标志位
//光照
float lightpercent = 0.0;
uint8_t LightStr[32] = {0};
//烟雾
uint16_t MQ2_Value=0;
float MQ2_ConvertedValue=0;
uint8_t MQ2Str[32]={0};
//处理烟温度的变量定义
short temp;
float temperature;
uint8_t tempStr[32]={0};

uint8_t ADCRawStr[32] = {0};
uint8_t StatusStr[32] = {0};
uint8_t curtain_Status;


int main(void)
{
    
    
    
    
    delay_init();
    LedInit();
    BeepInit();
    UART1_Config();
    TIM6_Config();
    TIM3_PWM_Init();
    ADC1_Init();
    Lcd1602Init();
    OLED_Init();
    OLED_Clear();       //清OLED屏
    DS18B20_Init();			//初始化DS18B20
    
        
    printf("System Starting...\n");
    printf("Initial ADC Value: %d\n", ADC_ConvertedValue);
    
    // OLED初始显示
    OLED_ShowCH(0, 0, (uint8_t*)"Light Sensor");
    OLED_ShowCH(2, 0, (uint8_t*)"Initializing...");
    
    // LCD1602初始显示
    Lcd1602ShowString(1, 1, (uint8_t*)"Light Sensor Test", 16);
    Lcd1602ShowString(2, 1, (uint8_t*)"Initializing...", 15);
    
    delay_ms(2000); // 显示初始化信息2秒
    
    // 清屏并显示初始界面
    OLED_Clear();
    OLED_ShowCH(0, 0, (uint8_t*)"Light:");
    
    Lcd1602ClearFull();
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
    while(1)
    {
        
			
				if(timeFlag500ms == 1)  
        {
            GPIOA->ODR ^= GPIO_Pin_7;
            timeFlag500ms = 0;
        }
        
        if(rxFlag == 1)
        {
            // 串口命令处理
            if(strcmp((const char*)strBuff, "LED1_ON") == 0)  
            {
                LED1_ON;
                printf("LED1 is ON!\n");
            }
            if(strcmp((const char*)strBuff, "LED1_OFF") == 0)
            {
                LED1_OFF;
                printf("LED1 is OFF!\n");
            }
            if(strcmp((const char*)strBuff, "BEEP_ON") == 0)
            {
                BEEP_ON;
                printf("BEEP is ON!\n");
            }
            if(strcmp((const char*)strBuff, "BEEP_OFF") == 0)
            {
                BEEP_OFF;
                printf("BEEP is OFF!\n");
            }
            
            rxFlag = 0; 
            memset(strBuff, 0, 32);
        }
        
        // ADC光照强度/烟雾浓度/温度采集并发送上位机
        lightpercent =(float)ADC_ConvertedValue[0] / 4095.0 * 1+00.0;
        printf("Raw ADC: %4d, Light: %0.1f%%\n", ADC_ConvertedValue[0], lightpercent);
        sprintf((char*)LightStr, "Light:%6.1f%%", lightpercent);

        MQ2_Value =ADC_ConvertedValue[1];
        MQ2_ConvertedValue=100*MQ2_Value/4950.0;
        printf("MQ:%6.1f%%", MQ2_ConvertedValue);
				
				if(timeFlag5s)//5s钟采集一次温度
				{
					temp=DS18B20_Get_Temp();
					if(temp<0)
					{
						temperature=(float)(-temp*0.1);
						printf("temp=%2.1f\n",temperature);
					}
					else
					{
						temperature=(float)(temp*0.1);
						printf("temp=%2.1f\n",temperature);
					}
					
				}
                
        //光照数据显示在OLED上
        sprintf((char*)ADCRawStr, "ADC Raw:%5d", ADC_ConvertedValue[0]);
        sprintf((char*)LightStr, "%.1f%%", lightpercent);
        OLED_ShowCH(0, 0, LightStr); 
                
				//烟雾数据显示在OLED上
				sprintf((char*)MQ2Str,"MQ2=%0.1f%%",MQ2_ConvertedValue);
				OLED_ShowCH(1,2,MQ2Str);
				
				//温度数据显示在OLED上
				sprintf((char*)tempStr,"T=%0.1f",temperature);
				OLED_ShowCH(2,4,tempStr);
        
                // 光照控制窗帘自动控制逻辑
        if(lightpercent < 30&&curtain_Status==0) // 光照弱
        {
            GPIOA->ODR &=~GPIO_Pin_3; 
						StepMotorControl(2);//开窗帘
						delay_ms(100);
						StepMotorControl(0);//停
						curtain_Status=1;//窗帘开的状态
        }
        else if(lightpercent > 60&&curtain_Status==1) // 光照强
        {
            GPIOA->ODR=GPIO_Pin_3; //灯灭
						StepMotorControl(1);//关窗帘
						delay_ms(100);
						StepMotorControl(0);//停
						curtain_Status=0;//窗帘关的状态
        }
                
				uint8_t keyValue=KeyScan();
				if(keyValue==1) GPIOA->ODR ^=GPIO_Pin_4;//DCMotor转or停
				
				//烟雾控制蜂鸣器报警自动控制逻辑
				if(MQ2_ConvertedValue>20)
				{
						BEEP_ON;}
				else
				{
						BEEP_OFF;}
				
				//温度控制风扇自动控制逻辑
				if(temperature>28)
				{
						DCColor=1;}//风扇开
				else
				{
						DCColor=0;}//风扇关
                
        delay_ms(1000); // 缩短刷新间隔，显示更流畅
    }
}
 
 
 