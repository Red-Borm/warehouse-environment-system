/******************************************************************************

 ******************************************************************************
*  文 件 名   : ds18b20.c
ds18b20驱动程序 测温范围-55℃-125
******************************************************************************/
#include "ds18b20.h"
#include "delay.h"	


/************************************************************************
* 函数: void DS18B20_IO_IN(void)
* 描述: 设置DQ引脚为输入模式
* 参数: none.
* 返回: none.
* 备注:    
************************************************************************/
void DS18B20_IO_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能PORTB口时钟 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				//推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

}
/************************************************************************
* 函数: void DS18B20_IO_OUT(void) 
* 描述: 设置DQ引脚为输出模式
* 参数: none.
* 返回: none.
* 备注:    
************************************************************************/
void DS18B20_IO_OUT(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能口时钟 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				//推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/************************************************************************
* 函数: void DS18B20_Rst(void)
* 描述: 复位DS18B20
* 参数: none.
* 返回: none.
* 备注:    
************************************************************************/
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); 	//设置为输出模式
    DS18B20_DQ_OUT=0; 	//拉低DQ
    delay_us(750);    	//拉低480-960us之间都可以
    DS18B20_DQ_OUT=1; 	//拉高DQ
	delay_us(15);     	//延时一会
}
/************************************************************************
* 函数: unsigned char DS18B20_Check(void) 
* 描述: 等待DS18B20的回应
* 参数: none.
* 返回: 返回1:未检测到DS18B20的存在  返回0:存在
* 备注:    
************************************************************************/
unsigned char DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();					//设置为输入模式	 
    while (DS18B20_DQ_IN&&retry<200)	//等待一段时间，如果初始化成功 DS18B20会在15~60ms内产生一个低电平 根据这个可以判断DS18B20是否存在
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;				//如果没有等到则返回1 表示不存在
	else retry=0;						//如果存在，再次判断一次 防止上次是失误
    while (!DS18B20_DQ_IN&&retry<240)	//DS18B20 输出电平后会输出高电平信号表示结束初始化 再次等待直到 出现高电平信号出现才能认为是DS18B20存在 然后返回
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    		//返回1 表示失败 DS18B20不存在   
	return 0;							//返回0 表示正确 DS18B20存在
}
/************************************************************************
* 函数: unsigned char DS18B20_Read_Bit(void) 
* 描述: 从DS18B20读取一个位的数据
* 参数: none.
* 返回: 返回:读到的数据 0 和 1
* 备注:    
************************************************************************/
unsigned char DS18B20_Read_Bit(void) 	 
{
    u8 data;
	DS18B20_IO_OUT();	//设置为输出模式
    DS18B20_DQ_OUT=0; 
	delay_us(2);
    DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();	//设置为输入模式
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    delay_us(50);           
    return data;
}
/************************************************************************
* 函数: unsigned char DS18B20_Read_Byte(void)  
* 描述: 从DS18B20读取一个字节
* 参数: none.
* 返回: 读到的数据
* 备注:    
************************************************************************/
unsigned char DS18B20_Read_Byte(void)     
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
/************************************************************************
* 函数: void DS18B20_Write_Byte(unsigned char dat) 
* 描述: 写一个字节到DS18B20
* 参数: dat：要写入的字节
* 返回: none.
* 备注:    
************************************************************************/ 
void DS18B20_Write_Byte(unsigned char dat)     
 {             
    unsigned char j;
    unsigned char testb;
	DS18B20_IO_OUT();			//设置输出模式
    for (j=1;j<=8;j++) 			//八位数据
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT=0;	// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT=1;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT=0;	// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}
/************************************************************************
* 函数: void DS18B20_Start(void) 
* 描述: 发送开始温度转换指令
* 参数: none.
* 返回: none.
* 备注:    
************************************************************************/
void DS18B20_Start(void) 
{   						               
    DS18B20_Rst();	   
	DS18B20_Check();			//初始化DS18B20	 
    DS18B20_Write_Byte(0xcc);	//跳过ROM命令 忽略64位ROM地址，直接向DS18B20发温度变换命令，适用于一个从机工作
    DS18B20_Write_Byte(0x44);	//启动DS18B20进行温度转换，12位转换时最长为750ms（9位为93.75ms），结果存入内部9字节的RAM中
} 
/************************************************************************
* 函数: unsigned char DS18B20_Init(void) 
* 描述: 初始化DS18B20的IO口 DQ 同时检测DS18B20的存在
* 参数: none.
* 返回: 返回1:不存在  返回0:存在  
* 备注:    
************************************************************************/	 
unsigned char DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	 //使能口时钟 	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 // 推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_11);    					 //输出1
	DS18B20_Rst();
	return DS18B20_Check();
}  
/************************************************************************
* 函数: short DS18B20_Get_Temp(void)
* 描述: 从ds18b20得到温度值
* 参数: none.
* 返回: 温度值 （-550~1250） 扩大了十倍的温度 分辨率：0.1C  精度0.5℃
* 备注:    
************************************************************************/ 
short DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
	short tem;
    DS18B20_Start ();  			//DS18B20开始工作
    DS18B20_Rst();				//初始化DS18B20	
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);	//跳过ROM命令 忽略64位ROM地址，直接向DS18B20发温度变换命令，适用于一个从机工作
    DS18B20_Write_Byte(0xbe);	//读温度命令 ，读暂存器。读内部RAM中9字节的温度数据
    TL=DS18B20_Read_Byte(); 	//温度值低位LSB   高5位是温度的正负号，低3位为温度的高位  
    TH=DS18B20_Read_Byte(); 	//温度值高位MSB   高4位为温度的低位，低4位为温度小数部分 
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;					//温度为负  
    }else temp=1;				//温度为正	  	  
    tem=TH; 					//获得高八位
    tem<<=8;    
    tem+=TL;					//获得低八位
	if(temp)					//如果为正的 直接转换	 
	{
		tem = (float)tem*0.0625*10;		   	//乘以0.0625 这里方便使用 扩大了十倍保留一位小数点
	}
	else 
	{
		tem = (float)(~(tem+1))*0.0625*10;	//按照手册要求 先加一 后取反乘以0.0625 这里方便使用 扩大了十倍保留一位小数点		
	}
	return tem;   
}



 
