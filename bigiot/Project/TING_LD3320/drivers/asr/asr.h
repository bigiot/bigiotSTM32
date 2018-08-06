/*-------------------------------------------------*/
/*               超纬电子开发板                    */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef __ASR_H
#define __ASR_H

#include "main.h"

extern u8 nAsrStatus;
extern u8 ucRegVal;

/*----------------------------------------*/
/*             关键词识别码               */
/*           根据需要自行修改             */
/*----------------------------------------*/

#define CODE_CQXT	1	 //重启系统
#define CODE_JGXH	2	 //讲个笑话
#define CODE_DK	    3	 //打开
#define CODE_GB	    4	 //关闭

#define DATE_A 4         //几个关键词
#define DATE_B 20		 //关键词的大小


//以下五个状态定义用来记录程序是在运行ASR识别过程中的哪个状态
#define LD_ASR_NONE				0x00	//	表示没有在作ASR识别
#define LD_ASR_RUNING			0x01	//	表示LD3320正在作ASR识别中
#define LD_ASR_FOUNDOK			0x10	//	表示一次识别流程结束后，有一个识别结果
#define LD_ASR_FOUNDZERO 		0x11	//	表示一次识别流程结束后，没有识别结果
#define LD_ASR_ERROR	 		0x31	//	表示一次识别流程中LD3320芯片内部出现不正确的状态

//MIC设置
#define MIC_VOL 0X4c

u8 RunASR(void);
void LD_Init_ASR(void);
u8 LD_Check_ASRBusyFlag_b2(void);
u8 LD_AsrRun(void);
u8 LD_GetResult(void);
u8 LD_AsrAddFixed(void);
void ProcessInt_ASR(void);

#endif
