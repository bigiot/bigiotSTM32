/*
file   : *.cpp
author : shentq
version: V1.0
date   : 2015/7/5

Copyright 2015 shentq. All Rights Reserved.
*/

//STM32 RUN IN eBox


#include "ebox.h"
#include "cJSON.h"

//================================
String WiFiSSID = "FAST_SXM";//=======
String WiFiPASSWORD = "lcx123456";//==
String DEVICEID = "112";
String APIKEY = "c88036f9c";
//====================================

char uart1_rx_str[1024];
int i=0;
char uart2_rx_str[1024];
int j=0;
cJSON * pJsonRoot = NULL;

void uart2_rx_event()
{
    uint16_t c;
    c = uart2.read();
    uart1.write(c);
    uart2_rx_str[j]=c;
    if (c == '\n')
    {
        uart2_rx_str[j+1] = '\0';
        //uart2.printf(uart2_rx_str);
        i=0;
    }else{
        i++;
    }
}
void uart1_rx_event()
{
    uint16_t c;
    c = uart1.read();
    uart1_rx_str[i]=c;
    if (c == '\n')
    {
        uart1_rx_str[i+1] = '\0';
        uart2.printf(uart1_rx_str);
        i=0;
    }else{
        i++;
    }
}

void setup()
{
    ebox_init();
    uart1.begin(115200);
    uart2.begin(115200);
    uart1.attach(uart1_rx_event,RxIrq);
    uart2.attach(uart2_rx_event,RxIrq);
    
    delay_ms(5000);

    uart2.printf("+++");
    delay_ms(1000);
    
    uart2.printf("AT\r\n");
    delay_ms(1000);
    
    uart2.printf("AT+RESTORE\r\n");
    delay_ms(5000);
    
    uart2.printf("AT+CWMODE=1\r\n");
    delay_ms(1000);
    
    uart2.print("AT+CWJAP=\""+WiFiSSID+"\",\""+WiFiPASSWORD+"\"\r\n");
    delay_ms(10000);
    
    uart2.printf("AT+CIPMUX=0\r\n");
    delay_ms(1000);

    uart2.printf("AT+CIPMODE=1\r\n");
    delay_ms(1000);
    
    uart2.printf("AT+SAVETRANSLINK=1,\"121.42.180.30\",8282,\"TCP\"\r\n");
    delay_ms(5000);
    
    uart2.printf("AT+RST\r\n");
    delay_ms(10000);
    
    uart2.print("{\"M\":\"checkin\",\"ID\":\""+DEVICEID+"\",\"K\":\""+APIKEY+"\"}\r\n");

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




