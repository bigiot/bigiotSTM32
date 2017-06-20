/*
file   : *.cpp
author : shentq
version: V1.0
date   : 2015/7/5

Copyright 2015 shentq. All Rights Reserved.
*/

//STM32 RUN IN eBox


#include "ebox.h"

char inputString[1024];
int i=0;

void uart2_rx_event()
{
    uint16_t c;
    c = uart2.read();
    uart1.write(c);
}
void uart1_rx_event()
{
    uint16_t c;
    c = uart1.read();
    inputString[i]=c;
	inputString[i+1] = '\0';
    if (c == '\n')
    {
        uart2.printf(inputString);
        i=0;
    }else if(strncmp(inputString,"+++",3) == 0)
	{
        uart2.printf(inputString);
        i=0;
    }else
	{
		i++;
	}
}
void setup()
{
    ebox_init();
    uart1.begin(115200);
    uart2.begin(115200);
    uart2.attach(uart2_rx_event,RxIrq);
    uart1.attach(uart1_rx_event,RxIrq);
}
int main(void)
{
    setup();
    uart1.printf("uart1 to uart2 OK!");
    while(1)
    {
        ;
    }
}
