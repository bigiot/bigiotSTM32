#ifndef __USART2_H
#define	__USART2_H

#include "stdio.h"	
#include "main.h"

#define USART2_RXBUFF_SIZE  			2048  	  //定义最大接收字节数 256
	  	
extern u8  USART2_RX_BUF[USART2_RXBUFF_SIZE];     //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART2_RX_STA;         		          //接收状态标记	

void USART2_init(u32 bound);
void UART2Test(void);
void UART2SendByte(unsigned char SendData);
unsigned char UART2GetByte(unsigned char* GetData);
void USART2_IRQHandler(void);
void USART2_printf(char *fmt, ...);

#endif
