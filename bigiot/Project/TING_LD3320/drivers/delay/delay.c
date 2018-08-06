/*-------------------------------------------------*/
/*               超纬电子开发板                    */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#include "delay.h"

/*-------------------------------------------------*/
/*函数名：初始化延迟函数                           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void delay_init()	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	//SysTick（滴答时钟）的频率是主频率的1/8
	//通常主频率为最大的72M，那么SysTick的频率是9M
	//那么SysTick计数器1个数，代表(1/9)us	
}								    

/*-------------------------------------------------*/
/*函数名：延迟微秒函数                             */
/*参  数：us：延时多少微秒                         */
/*返回值：无                                       */
/*-------------------------------------------------*/	    								   
void delay_us(u32 us)
{		
	u32 temp;	    	 
	SysTick->LOAD=us*9;                        //SysTick加载值，delay_init函数中我们知道， SysTick计数器一个数是1/9微秒，所以乘以9	  		 
	SysTick->VAL=0x00;                         //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;   //开始倒数	 
	do
	{
		temp=SysTick->CTRL;					   //读取SysTick状态寄存器
	}
	while(temp&0x01&&!(temp&(1<<16)));         //等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;   //关闭计数器
	SysTick->VAL =0X00;        				   //清空计数器	 
}

/*-------------------------------------------------*/
/*函数名：延迟毫秒函数                             */
/*参  数：ms：延时多少毫秒                         */
/*返回值：无                                       */
/*-------------------------------------------------*/
void delay_ms(u32 ms)
{
	//我们首先注意一个问题SysTick时钟计数器是24位的，9M频率下，总共能延时1864.135ms
	//所有我们以1800为界限，小于1800的延时一次计数就行，大于1800的多次计数	
	u32 i;
	u32 temp;
	
	if(ms<1800)   //小于1800
	{
		SysTick->LOAD=(u32)ms*9*1000;             //时间加载,SysTick一个数使1/9微秒，换算成ms的话，乘以9再乘以1000
		SysTick->VAL =0x00;                       //清空计数器
		SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  //开始倒数  
		do
		{
			temp=SysTick->CTRL;                        //读取SysTick状态寄存器
		}
		while((temp&0x01)&&!(temp&(1<<16)));           //等待时间到达，标志位置位   
		SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
		SysTick->VAL =0X00;                            //清空计数器	 
	}
	else   //大于1800
	{
		for(i=0;i<(ms/1800);i++)    //除以1800整数部分，for循环的延时
		{
			SysTick->LOAD=1800*9*1000;                //时间加载,700ms延时
			SysTick->VAL =0x00;                       //清空计数器
			SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  //开始倒数  
			do
			{
				temp=SysTick->CTRL;                        //读取SysTick状态寄存器
			}
			while((temp&0x01)&&!(temp&(1<<16)));           //等待时间到达，标志位置位   
			SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
			SysTick->VAL =0X00;                            //清空计数器	 
		}
		//接下来余数部分的延时
		SysTick->LOAD=(u32)(ms%1800)*9*1000;      //时间加载,余数部分
		SysTick->VAL =0x00;                       //清空计数器
		SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  //开始倒数  
		do
		{
		temp=SysTick->CTRL;                       //读取SysTick状态寄存器
		}
		while((temp&0x01)&&!(temp&(1<<16)));           //等待时间到达，标志位置位   
		SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
		SysTick->VAL =0X00;                            //清空计数器	
	}
}     

/*-------------------------------------------------*/
/*函数名：延迟秒函数                               */
/*参  数：ms：延时多少秒                           */
/*返回值：无                                       */
/*-------------------------------------------------*/
void delay_s(u32 s)
{
	u32 i;
	
	for(i=0;i<s;i++)
		delay_ms(1000);
}





























