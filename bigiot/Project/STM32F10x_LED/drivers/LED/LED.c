#include "stm32f10x.h"
#include "LED.h"


/* LED1端口控制 */
#define Led1SetHigh()       GPIO_SetBits(GPIOC, GPIO_Pin_13)
#define Led1SetLow()        GPIO_ResetBits(GPIOC, GPIO_Pin_13)
#define Led1Status()        GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)

/* LED2端口控制 */
#define Led2SetHigh()       GPIO_SetBits(GPIOC, GPIO_Pin_14)
#define Led2SetLow()        GPIO_ResetBits(GPIOC, GPIO_Pin_14)
#define Led2Status()        GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_14)

/* LED_BL端口控制 */
#define LedBLSetHigh()      GPIO_SetBits(GPIOC, GPIO_Pin_15)
#define LedBLSetLow()       GPIO_ResetBits(GPIOC, GPIO_Pin_15)
#define LedBLStatus()       GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_15)


/**********************************************/
/*                                            */
/*                 LED初始化                  */
/*                                            */
/**********************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /* GPIO时钟使能 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    /* GPIO配置 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_InitStructure.GPIO_Pin);
}


/**********************************************/
/*                                            */
/*                 点亮LED                    */
/*                                            */
/**********************************************/
void Led_On(uint8_t leds)
{
    if(leds == LED1)
    {
        Led1SetLow();
    }
    else if(leds == LED2)
    {
        Led2SetLow();
    }
    else if(leds == LED_BL)
    {
        LedBLSetLow();
    }
}


/**********************************************/
/*                                            */
/*                 熄灭LED                    */
/*                                            */
/**********************************************/
void Led_Off(uint8_t leds)
{
    if(leds == LED1)
    {
        Led1SetHigh();
    }
    else if(leds == LED2)
    {
        Led2SetHigh();
    }
    else if(leds == LED_BL)
    {
        LedBLSetHigh();
    }
}


/**********************************************/
/*                                            */
/*               LED状态反转                  */
/*                                            */
/**********************************************/
void Led_Toggle(uint8_t leds)
{
    if(leds == LED1)
    {
        if(Led1Status())
        {
            Led1SetLow();
        }
        else
        {
            Led1SetHigh();
        }
    }
    else if(leds == LED2)
    {
        if(Led2Status())
        {
            Led2SetLow();
        }
        else
        {
            Led2SetHigh();
        }
    }
    else if(leds == LED_BL)
    {
        if(LedBLStatus())
        {
            LedBLSetLow();
        }
        else
        {
            LedBLSetHigh();
        }
    }
}
