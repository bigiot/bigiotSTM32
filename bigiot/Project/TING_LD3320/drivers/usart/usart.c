/*-------------------------------------------------*/
/*               超纬电子开发板                    */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#include "usart.h"	  
	
u8  USART1_RX_BUF[USART1_RXBUFF_SIZE]; //接收缓冲
u16 USART1_RX_STA=0;                   //接收状态标记	  

/*-------------------------------------------------*/
/*函数名：初始化串口1                              */
/*参  数：bound：波特率                            */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Usart1_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOA	
	USART_DeInit(USART1);                                   //复位串口1
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;              
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //初始化PA9
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //初始化PA10

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;       //串口1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	                        //根据指定的参数初始化VIC寄存器
  
	USART_InitStructure.USART_BaudRate = bound;                //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;        //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //收发模式

    USART_Init(USART1, &USART_InitStructure);     //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口 
}

/*加入以下代码,支持printf函数,而不需要选择use MicroLIB*/	  
#if 1
#pragma import(__use_no_semihosting)                             
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
_sys_exit(int x) 
{ 
	x = x; 
} 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
/*-------------------------------------------------*/
/*函数名：串口1中断服务函数                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART1_IRQHandler(void)   //串口1中断服务程序，数据以\r\n作为结束标志
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //进入接收中断
	{
		Res =USART_ReceiveData(USART1); //读取接收到的数据		
		if((USART1_RX_STA&0x8000)==0)    //==0，接收没完成：位15代表置位与否，代表接收是否完成
		{			
			if(USART1_RX_STA&0x4000)     //位14置位代表，接收到了\r
			{
				if(Res=='\n')           //如果接收到\n，如此一来\r\n都收到了，表示结束
				{				
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res; //记录下\n数据
					USART1_RX_STA++;	                     //角标+1
					USART1_RX_STA|=1<<15;	                 //位15置位，接收完成了 
				}
				else 
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res; //记录下数据
					USART1_RX_STA++;	                        //角标+1
					USART1_RX_STA&=~(1<<14);                    //清除位14，接收到的\r不是代表结束，是正常数据	
				}
			}
			else
			{
				if(Res=='\r') //如果接收到了\r
				{
					USART1_RX_STA|=(1<<14);                  //位14置位代表，接收到了\r 
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res; //记录下\r
					USART1_RX_STA++;	                     //数组角标+1
				}
				else         //如果不是\r
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res; //记录下数据
					USART1_RX_STA++;	                     //数组角标+1
				}				
			}
		} 
  } 
} 

