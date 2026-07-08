#include "lcd1602.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "delay.h"


/*********************************************************************
 @Function  : LCD1602初始化
 @Parameter : N/A
 @Return    : N/A
**********************************************************************/
void Lcd1602Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  /*时钟使能*/	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE, ENABLE);
  /*引脚配置*/	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | //EN->PA1
															  GPIO_Pin_2 | //R/W->PA2
															  GPIO_Pin_3;  //RS->PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
  GPIO_Init(GPIOA, &GPIO_InitStructure);					   
	//D0~D7->PE7~PE15
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
															  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
															  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);	//拉低	
  GPIO_SetBits(GPIOE,GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
										 GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
										 GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);	//拉高
	/*初始化LCD1602*/
	Lcd1602WriteCommand(0x38);	//功能设置，8位，双行，5*7
	Lcd1602WriteCommand(0x01);	//清屏
	Lcd1602WriteCommand(0x06);	//字符进入设置，字符递增，屏幕固定
	Lcd1602WriteCommand(0x0c);	//开显示，光标不显示
}

/*********************************************************************
 @Function  : LCD1602写总线数据
 @Parameter : N/A
 @Return    : N/A
**********************************************************************/
void Lcd1602WriteBusData(uint8_t dat)
{
	uint16_t temp;
	
	temp = dat;
	temp <<=8;
	temp |= GPIO_ReadOutputData(GPIOE)&0x00FF;	
	GPIO_Write(GPIOE,temp);
}


/*********************************************************************
 @Function  : 等待LCD1602忙结束
 @Parameter : N/A
 @Return    : N/A
**********************************************************************/
void Lcd1602WaitBusy(void)
{
	uint8_t temp;
  do
	{
		/*端口拉高*/
		Lcd1602WriteBusData(0xff);
		LCD1602_EN=0;
		/*选择指令寄存器*/
		LCD1602_RS=0;
		/*使能读*/
		LCD1602_RW=1;
		LCD1602_EN=1;				
		/*读数据*/
		temp = (uint8_t)(GPIO_ReadInputData(GPIOE)>>8);	
		LCD1602_EN=0;	
	}while(temp&0x80); /*最高位为1，液晶忙*/
}

/*********************************************************************
 @Function  : 向LCD1602写命令
 @Parameter : Command : 要写的命令
 @Return    : N/A
**********************************************************************/
void Lcd1602WriteCommand(uint8_t Command)
{
	/*等待忙结束*/
	Lcd1602WaitBusy();
	LCD1602_EN=0;					
	/*选择指令寄存器*/
	LCD1602_RS=0;					
	/*使能写*/
	LCD1602_RW=0;					
  /*写命令*/
	Lcd1602WriteBusData(Command);	
	/*执行命令*/
	LCD1602_EN=1;					
	LCD1602_EN=0;	
}

/*********************************************************************
 @Function  : 向LCD1602写数据
 @Parameter : data : 要写的数据
 @Return    : N/A
**********************************************************************/
void Lcd1602WriteData(uint8_t data)
{
	/*等待忙结束*/
	Lcd1602WaitBusy();
	LCD1602_EN=0;					
	/*选择数据寄存器*/
	LCD1602_RS=1;					
	/*使能写*/
	LCD1602_RW=0;					
  /*写数据*/
	Lcd1602WriteBusData(data);	
	/*执行命令*/
	LCD1602_EN=1;					
	LCD1602_EN=0;	
}

/*********************************************************************
 @Function  : LCD1602清全屏显示
 @Parameter : N/A
 @Return    : N/A
**********************************************************************/
void Lcd1602ClearFull(void)
{
 	Lcd1602WriteCommand(0x01);	
}

/*********************************************************************
 @Function  : LCD1602显示字符数据
 @Parameter : line : 起始行 1~2
							list ：起始列 0~16
							*s   ：显示的数据
							len  : 显示长度
 @Return    : N/A
**********************************************************************/
void Lcd1602ShowString(uint8_t line,uint8_t list,uint8_t *s,uint8_t len)
{
	uint8_t temp,i;
  /*取行*/
	if(line==2) line=0xc0;
	else line=0x80;
	/*限制在一列*/
	if(list>16) list=16;
	/*设置显示起始地址*/
	Lcd1602WriteCommand(line|list);
	/*输出字符串*/
	for(i=0;i<len;i++) 
	{
		temp = *(s+i);
	  Lcd1602WriteData(temp);
	}	
}

/*********************************************************************
 @Function  : LCD1602显示正整数
 @Parameter : line : 起始行 1~2
							list ：起始列 0~16
							*s   ：显示的数据
							mode : 空余填充 
									 0 ：不填充 
									 1 ：填充0
 @Return    : N/A
**********************************************************************/
void Lcd1602ShowInt(uint8_t line,uint8_t list,uint8_t dat,uint8_t mode)
{
	uint8_t i,y=1,t=1,temp;
	uint8_t p[3];	
  /*范围判断*/
	for(i=0;i<2;i++)
	{
	  t*=10;
		if(dat>=t) 
			y+=1;
	}
	/*取数据*/
	p[2]=(dat%10)+0x30; //个位
	t=1;
	for(i=2;i>0;i--) //百、十位
	{
		t*=10;
		temp = dat/t;
		if(temp>=10)
			temp=temp%10;
		if(temp)
			p[i-1]=temp+0x30; 
		else	
			p[i-1]=0x30;
	}
  /*空余填充*/
  if(y<3)
	{
    t=3-y;
		for(i=0;i<t;i++)
		{
			if(mode) 
				p[i]=0x30;
			else 
				p[i]=' ';
		}
	}	
	/*送显示*/
	Lcd1602ShowString(line,list,p,3);
}

