#include "key.h"
#include "delay.h"

//按键初始化
void KeyInit(void)
{
	  GPIO_InitTypeDef  GPIO_InitStruct;  
    
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
  
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;      
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    
    

    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
	
}
//按键扫描函数
uint8_t KeyScan(void)
{
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)==0)//判断是否有键按下
	{
		delay_ms(10);//演延时消抖动
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)==0)
		{
			//while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)==0);//等待按键松开
			
			return KEY1;
		}
	}
	
	
if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)==0)//判断是否有键按下
	{
		delay_ms(10);//演延时消抖动
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)==0)
		{
			//while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)==0);//等待按键松开
			
			return KEY2;
		}
	}
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==0)//判断是否有键按下
	{
		delay_ms(10);//演延时消抖动
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==0)
		{
			//while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==0);//等待按键松开
			
			return KEY3;
		}
	}
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)==0)//判断是否有键按下
	{
		delay_ms(10);//演延时消抖动
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)==0)
		{
			//while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)==0);//等待按键松开
			
			return KEY4;
		}
	}
      	return   NO_KEYPRESS;//没有键按下,返回0
}