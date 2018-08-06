/*-------------------------------------------------*/
/*               超纬电子开发板                    */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef __SPI1_H
#define __SPI1_H

#include "main.h"
				  	    													  
void SPI1_Init(void);			    //初始化SPI1口
void SPI1_SetSpeed(u8 SpeedSet);    //设置SPI1速度   
u8   SPI1_ReadWriteByte(u8 TxData); //SPI1总线读写一个字节		 
#endif

