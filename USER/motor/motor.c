#include "motor.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "delay.h"


uint8_t FFW[8]={0xf1,0xf3,0xf2,0xf6,0xf4,0xfc,0xf8,0xf9}; //反转
uint8_t FFZ[8]={0xf9,0xf8,0xfc,0xf4,0xf6,0xf2,0xf3,0xf1}; //正转

/*********************************************************************
 @Function  : 电机驱动IO初始化
 @Parameter : N/A 
 @Return    : N/A
**********************************************************************/
void MotorInit(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 /*时钟使能*/	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 /*引脚配置*/	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |  //PA0->A PA1->B
															 GPIO_Pin_2 | GPIO_Pin_3 |  //PA2->C PA3->D
															 GPIO_Pin_4;  //PA4->DC  
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
 GPIO_Init(GPIOA, &GPIO_InitStructure);				 
 GPIO_ResetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_1 | 
										GPIO_Pin_2 | GPIO_Pin_3 | 
										GPIO_Pin_4);	 //拉低，停转	
}

/*********************************************************************
 @Function  : 步进电机驱动处理
 @Parameter : tobit : 项极性值
 @Return    : N/A
**********************************************************************/
void SetBit(uint8_t tobit)
{
	uint8_t i,temp;
	temp = tobit;
	for(i=0;i<4;i++)
	{
		if(temp&0x01)
			PAout(i)=1;
		else
			PAout(i)=0;
		temp>>=1;
	}
}

/*********************************************************************
 @Function  : 步进电机转向控制
 @Parameter : Status : 运转控制
										 1 : 顺转
                     2 : 反转
 @Return    : N/A
**********************************************************************/
void StepMotorControl(uint8_t Status)
{
	 uint8_t i=0,j=0;
	switch(Status)
	{
		case 1:	 //顺转
		  for(j=0;j<12;j++)
      {
		    for(i=0;i<8;i++)
        {
		      SetBit(FFW[i]&0x1f);
          delay_ms(2);
        }
		  }		
			break;
		case 2:	 //反转	
		  for(j=0;j<12;j++)
      {
		    for(i=0;i<8;i++)
        {
		      SetBit(FFZ[i]&0x1f);
          delay_ms(2);
        }
	    }		
			break;
		default: SetBit(0); break; //停止
	}	
}

