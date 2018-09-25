#include <stdarg.h>
#include "usart2.h"

u8  USART2_RX_BUF[USART2_RXBUFF_SIZE]; //接收缓冲
u16 USART2_RX_STA=0;                   //接收状态标记	  

void USART2_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE );
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //?????,4??????,4??????;
    
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //???;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; //?????;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4; //?????;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //USART2 TX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //??????;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); //??A;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //USART2 RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //????;
    GPIO_Init(GPIOA, &GPIO_InitStructure); //??A;

    USART_InitStructure.USART_BaudRate = bound; //???;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //???8?;
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //???1?;
    USART_InitStructure.USART_Parity = USART_Parity_No ; //????;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);//??????;

    //USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE); //????;
}

/*发送一个字节数据*/
void UART2SendByte(unsigned char SendData)
{
    USART_SendData(USART2, SendData);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/*接收一个字节数据*/
unsigned char UART2GetByte(unsigned char* GetData)
{
    if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
    {
        return 0;//没有收到数据
    }
    *GetData = USART_ReceiveData(USART2);
    return 1;//收到数据
}
/*接收一个数据，马上返回接收到的这个数据*/
void UART2Test(void)
{
    unsigned char i = 0;

    while(1)
    {
        while(UART2GetByte(&i))
        {
            USART_SendData(USART2, i);
        }
    }
}

/*-------------------------------------------------*/
/*函数名：串口2中断服务函数                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)   //串口1中断服务程序，数据以\r\n作为结束标志
{
    u8 Res;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //进入接收中断
    {
        Res = USART_ReceiveData(USART2); //读取接收到的数据
        if((USART2_RX_STA & 0x8000) == 0) //==0，接收没完成：位15代表置位与否，代表接收是否完成
        {
            if(Res == '\n')         //如果接收到\n，如此一来\r\n都收到了，表示结束
            {
                USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = Res; //记录下\n数据
                USART2_RX_STA++;	                     //角标+1
                USART2_RX_STA |= 1 << 15;	             //位15置位，接收完成了
            }
            else         //如果不是\n
            {
                USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = Res; //记录下数据
                USART2_RX_STA++;	                     //数组角标+1
            }
        }
    }
}

void  USART2_printf(char *fmt, ...)
{
    char buffer[USART2_RXBUFF_SIZE - 1];
    u16 i = 0;
    u8 len;

    va_list arg_ptr; //Define convert parameters variable
    va_start(arg_ptr, fmt); //Init variable
    len = vsnprintf(buffer, USART2_RXBUFF_SIZE+1, fmt, arg_ptr); //parameters list format to buffer
    
    while ((i < USART2_RXBUFF_SIZE) && (i < len) && (len > 0))
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        USART_SendData(USART2, (u8) buffer[i++]);
    }
    va_end(arg_ptr);
}

