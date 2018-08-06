/*-------------------------------------------------*/
/*               超纬电子开发板                    */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef __LD3320_H
#define __LD3320_H

#include "main.h"

extern u8 nLD_Mode;

/*----------------------------------------*/
/*  注意修改LD_AsrAddFixed函数中的关键词  */
/*----------------------------------------*/

#define LD_RST_H()   PBout(8)=1
#define LD_RST_L()   PBout(8)=0

#define LD_CS_H()	 PAout(12)=1
#define LD_CS_L()	 PAout(12)=0

#define LD_SPIS_H()  PAout(11)=1
#define LD_SPIS_L()  PAout(11)=0

//以下三个状态定义用来记录程序是在运行ASR识别还是在运行MP3播放
#define LD_MODE_IDLE		0x00
#define LD_MODE_ASR		    0x08
#define LD_MODE_MP3		 	0x40

//以下是LD3320的时钟配置
#define CLK_IN   	        8.00 	    //  开发板上用的12M的有源晶振
#define LD_PLL_11			(u8)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19		0x0f
#define LD_PLL_MP3_1B		0x18
#define LD_PLL_MP3_1D   	(u8)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

#define LD_PLL_ASR_19 		(u8)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 		0x48
#define LD_PLL_ASR_1D 		0x1f

void LD_reset(void);
void LD_Init_Common(void);
void LD3320_Init(void);
void LD3320_GPIO_init(void);
void LD3320_EXTI_Init(void);
u8 spi_send_byte(u8 byte);
void LD_WriteReg(u8 data1,u8 data2);
u8 LD_ReadReg(u8 reg_add);
void ProcessInt0(void);
void CLK_Configuration(void);
void EXTI9_5_IRQHandler(void);
#endif

