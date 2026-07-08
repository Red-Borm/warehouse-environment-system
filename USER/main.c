#include "stm32f10x.h"
#include "key.h"
#include "keyint.h"
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
#include "dht11_driver.h"
#include "stdio.h"
#include "sg90.h" 


/* --------------------------------------------------------- */
/* 模式定义 */
/* --------------------------------------------------------- */
typedef enum
{
    MODE_REMOTE = 0, // 远程模式
    MODE_AUTO = 1,   // 自动模式
    MODE_MANUL = 2   // 手动模式
}   ModeType;

/* 全局模式变量和执行机构状态变量 */
ModeType CurrentMode = MODE_AUTO; // 初始化为自动模式

uint8_t Fan_Status = 0;           // DC 直流电机/风扇状态 (0:关, 1:开) - 仅用于显示状态

uint8_t Vent_Position = 0;        // 排风口门阀位置状态 (0:关闭, 1:开启)

uint8_t curtain_Status = 0; 

/* 外部变量引用声明 */
extern uint8_t strBuff[32];                 // 串口接收缓冲区
extern uint8_t rxFlag;                      // 串口接收标志位
extern volatile uint16_t ADC_ConvertedValue[2]; // DMA搬运的ADC值 [0]:光照 [1]:烟雾
extern uint8_t timeFlag500ms;               // 0.5s定时标志位
extern uint8_t timeFlag5s;                  // 5s定时标志位
extern uint8_t  keyFlag;                    // DK1按键中断标志位

uint8_t keyValue;
// 传感器数据
float lightpercent = 0.0;
float MQ2_ConvertedValue = 0;
short temp;
float temperature = 0.0; 
DHT11_Data_TypeDef humi_temp;
ErrorStatus dhtStatus = ERROR; 

// 显示缓冲区
char Lcd_Line1[17] = "Mode: AUTO      "; // LCD行1
char Lcd_Line2[17] = "System Normal   "; // LCD行2


// 【已删除】Set_DC_Motor 函数，直接使用 DCColor 宏控制


int main(void)
{
    // ---------------------------------------------------------
    // 1. 硬件外设初始化
    // ---------------------------------------------------------
    delay_init();           
    LedInit();              
    BeepInit();             
    UART1_Config();         
    TIM6_Config();          
    TIM3_PWM_Init();        
    ADC1_Init();            
    Lcd1602Init();          
    OLED_Init();            
    OLED_Clear();           
    DS18B20_Init();         
    DHT11_GPIO_Config();    
    KeyInit();              
    KeyIntConfig();         
    MotorInit();            // 【新增】确保 DCColor 对应的 PA4 被配置为输出
    
    // 【舵机初始化】
    SG90_Init();            // 新增：舵机初始化
    Servo_SetAngle(0);      // 初始状态：门阀关闭
    
    // 设置中断优先级分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
    
    while(1)
    {
        
        // -----------------------------------------------------
        // 2. 传感器数据采集与计算
        // -----------------------------------------------------
        // ... (传感器采集代码不变) ...
        
        // A. 心跳灯
        if(timeFlag500ms == 1)  
        {
            GPIOA->ODR ^= GPIO_Pin_7; 
            timeFlag500ms = 0;        
        }
        
        // B. 光照计算
        lightpercent = (float)ADC_ConvertedValue[0] / 4095.0 * 100.0;
        
        // C. 烟雾计算 (MQ2)
        MQ2_ConvertedValue = 100.0 * (float)ADC_ConvertedValue[1] / 4095.0; 
        
        // D. 温度采集 (DS18B20)
        if(timeFlag5s) 
        {
            temp = DS18B20_Get_Temp();
            if(temp < 0) temperature = (float)(-temp * 0.1);
            else temperature = (float)(temp * 0.1);
            timeFlag5s = 0; 
        }
				
		// E. 读取DHT11温湿度
        dhtStatus = Read_DHT11(&humi_temp);
		
		// -----------------------------------------------------
        // 3. 模式切换逻辑 (DK1 中断处理)
        // -----------------------------------------------------
        if (keyFlag == 1) 
        {
            delay_ms(20); 
            if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)==0) 
            {
                // 切换模式：REMOTE (0) -> AUTO (1) -> MANUL (2) -> REMOTE (0)
                CurrentMode = (ModeType)((CurrentMode + 1) % 3); 
                
                // 模式切换后，关闭所有执行机构 (确保安全和状态清零)
                DCColor = 0;    // 关闭 DC 风扇
                Fan_Status = 0; // 更新状态变量
                
                BEEP_OFF;       // 关闭蜂鸣器
                
                curtain_Status = 0; 
            }
            keyFlag = 0; // 清除标志位
        }
        
        // -----------------------------------------------------
        // 4. 工作模式控制逻辑
        // -----------------------------------------------------
        switch((int)CurrentMode)
		{
			case MODE_REMOTE: // 远程控制模式
                
                sprintf((char*)Lcd_Line1, "Mode: REMOTE    ");
                sprintf((char*)Lcd_Line2, "Wait for Cmd    ");
                
                if(rxFlag == 1)
                {
                    if (strstr((char*)strBuff, "FAN_ON")) 
                    {
                        DCColor = 1; 
                        Fan_Status = 1;
                    }
                    else if (strstr((char*)strBuff, "FAN_OFF")) 
                    {
                        DCColor = 0; 
                        Fan_Status = 0;
                    }
                    // 远程控制门阀
                    else if (strstr((char*)strBuff, "VENT_OPEN")) Servo_SetAngle(0);
                    else if (strstr((char*)strBuff, "VENT_CLOSE")) Servo_SetAngle(180); // 注意：SG90一般是0-180度，这里可能是笔误保留原样
                    
                    rxFlag = 0; 
                    memset(strBuff, 0, 32);
                }
                
                break;
       
            case MODE_AUTO: // 自动控制模式 (新联动逻辑)
                
                // --- 1. 报警与光敏液位报警 ---
                if (temperature > 40.0 || MQ2_ConvertedValue > 75.0 || lightpercent > 60.0)
                {
                    // 简化报警：T>40 OR CH4>75 OR Liquid Low(Light>60)
                    BEEP_ON; 
                    if (temperature > 40.0)
                        sprintf((char*)Lcd_Line2, "Alarm: TEMP HIGH");
                    else if (MQ2_ConvertedValue > 75.0)
                        sprintf((char*)Lcd_Line2, "Alarm: GAS HIGH ");
                    else if (lightpercent > 60.0)
                        sprintf((char*)Lcd_Line2, "Alarm: LIQ LOW  ");
                }
                else
                {
                    BEEP_OFF; // 解除报警
                    sprintf((char*)Lcd_Line2, "System Normal   ");
                }

                // --- 2. DC风扇控制 ---
                // 开启条件：温度 > 25C OR 甲烷 > 60%
                if (temperature >= 30.0 || MQ2_ConvertedValue >= 60.0)
                {
                    DCColor = 1;
                    Fan_Status = 1;
                }
                // 关闭条件 (需同时满足)：温度 < 28C AND 甲烷 < 55%
                else if (temperature < 28.0 && MQ2_ConvertedValue < 55.0) 
                {
                    DCColor = 0;
                    Fan_Status = 0;
                }

                // --- 3. 门阀/舵机控制 (温度/甲烷) ---
                if (temperature >= 25.0|| MQ2_ConvertedValue >= 50.0) 
                {
                    Servo_SetAngle(0); // 温度高/气高，开启门阀散热/排气
                    Vent_Position = 1;
                }
                else if (temperature < 25.0|| MQ2_ConvertedValue < 50.0) 
                {
                    Servo_SetAngle(180); // 温度低/气低，关闭门阀
                    Vent_Position = 0;
                }
								
				//--- 3. 门阀/舵机/风扇控制 (湿度) ---
				if (humi_temp.humi_int >= 50.0)
				{
					Servo_SetAngle(0); 
                    Vent_Position = 1;
				}
				else if (humi_temp.humi_int >= 90.0)
				{
					DCColor = 1;
                    Fan_Status = 1;
				}
				else
				{
					Servo_SetAngle(180);
                    Vent_Position = 0;
          DCColor = 0;
                    Fan_Status = 0;
				}
				
                
                sprintf((char*)Lcd_Line1, "Mode: AUTO      ");
                // 如果没有报警，LCD2显示执行机构状态
                if(strstr((char*)Lcd_Line2, "Alarm") == NULL)
                    sprintf((char*)Lcd_Line2, "FAN:%s VENT:%s", Fan_Status ? "ON " : "OFF", Vent_Position ? "OPEN" : "CLSD");


                break;
			
            case MODE_MANUL: // 手动控制模式
                
                sprintf((char*)Lcd_Line1, "Mode: MANUL     ");
                
                // 读取按键 (KEY2, KEY3, KEY4)
                keyValue = KeyScan();
                
                if(keyValue == KEY2) // DK2 (PC5) - 控制风扇开关
                {
                    if(Fan_Status == 0)
                    {
                        DCColor = 1;
                        Fan_Status = 1;
                    }
                    else
                    {
                        DCColor = 0;
                        Fan_Status = 0;
                    }
                    delay_ms(50); 
                }
                else if(keyValue == KEY3) // DK3 (PC6) - 门阀开启
                {
                    Servo_SetAngle(0);
                    Vent_Position = 1;
                }
                else if(keyValue == KEY4) // DK4 (PC7) - 门阀关闭
                {
                    Servo_SetAngle(180);
                    Vent_Position = 0;
                }
                
                // LCD第二行显示执行机构状态
                sprintf((char*)Lcd_Line2, "FAN:%s VENT:%s", Fan_Status ? "ON " : "OFF", Vent_Position ? "OPEN" : "CLSD");
                
                break;
        }
				
        // -----------------------------------------------------
        // 5. 显示更新 
        // -----------------------------------------------------
        
        // 刷新 LCD
        Lcd1602ShowString(1, 0, (uint8_t*)Lcd_Line1, 16); // 第一行显示模式
        Lcd1602ShowString(2, 0, (uint8_t*)Lcd_Line2, 16); // 第二行显示状态

        // 刷新 OLED
        sprintf((char*)strBuff, "Light:%.1f%%   ", lightpercent);
        OLED_ShowCH(0, 0, (uint8_t*)strBuff); 
        
        sprintf((char*)strBuff, "CH4  :%.1f%%   ", MQ2_ConvertedValue);
        OLED_ShowCH(0, 2, (uint8_t*)strBuff);
        
        sprintf((char*)strBuff,"Temp :%d.%dC  ",humi_temp.temp_int,humi_temp.temp_deci);
        OLED_ShowCH(0, 4, (uint8_t*)strBuff);
        
        if(dhtStatus == SUCCESS)
        {
			sprintf((char*)strBuff,"Humi :%d%%  ",humi_temp.humi_int);
        }
        else
        {
            sprintf((char*)strBuff, "Humi :Error    "); 
        }
        OLED_ShowCH(0, 6, (uint8_t*)strBuff);


        // -----------------------------------------------------
        // 6. 串口统一输出 
        // -----------------------------------------------------
        
        printf("\r\n--- Sensor Status ---\r\n");
        printf("Mode        : %s\r\n", (CurrentMode == MODE_REMOTE) ? "REMOTE" : ((CurrentMode == MODE_AUTO) ? "AUTO" : "MANUAL"));
        printf("Vent Status : %s\\r\\n", Vent_Position ? "OPEN" : "CLOSE");
        printf("Fan Status  : %s\r\n", Fan_Status ? "ON" : "OFF"); 
        printf("Light Level : %.1f %%\r\n", lightpercent);
        printf("Smoke Level : %.1f %%\r\n", MQ2_ConvertedValue);
        printf("Temperature : %.1f C\r\n", temperature);
        if(dhtStatus == SUCCESS)
        {
            printf("Humidity    : %d %%\r\n", humi_temp.humi_int);
        }
        else
        {
            printf("Humidity    : Read Error\r\n");
        }
        printf("Curtain     : %s\r\n", curtain_Status ? "OPEN" : "CLOSE");


        // -----------------------------------------------------
        // 7. 定时与延时
        // -----------------------------------------------------
        // 【删除】StepMotor_Timer_Run() 的调用
        
        delay_ms(1000); 
    } 
}