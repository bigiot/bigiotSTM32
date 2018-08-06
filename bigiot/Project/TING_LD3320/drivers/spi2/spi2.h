/*-------------------------------------------------*/
/*               超纬电子开发板                    */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef __SPI2_H
#define __SPI2_H

#include "main.h"
				  	    													  
void SPI2_Init(void);			    //初始化SPI2口
void SPI2_SetSpeed(u8 SpeedSet);    //设置SPI2速度   
u8   SPI2_ReadWriteByte(u8 TxData); //SPI2总线读写一个字节
		 
#endif

