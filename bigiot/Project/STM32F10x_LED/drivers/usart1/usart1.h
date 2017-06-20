#ifndef __USART1_H
#define	__USART1_H

void USART1_Config(void);
void UART1Test(void);
void UART1SendByte(unsigned char SendData);
unsigned char UART1GetByte(unsigned char* GetData);
void USART1_IRQHandler(void);

#endif
