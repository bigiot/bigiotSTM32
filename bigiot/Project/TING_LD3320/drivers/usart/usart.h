/*-------------------------------------------------*/
/*               超纬电子开发板                    */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "main.h"

#define USART1_RXBUFF_SIZE  			1024  	  //定义最大接收字节数 256
	  	
extern u8  USART1_RX_BUF[USART1_RXBUFF_SIZE];     //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART1_RX_STA;         		          //接收状态标记	

void Usart1_init(u32 bound);
void USART1_IRQHandler(void);
#endif


