/*-------------------------------------------------*/
/*               超纬电子开发板                    */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/
#include <string.h>
#include "asr.h"
#include "ld3320.h"
#include "delay.h"
#include "ff.h"
#include "usart1.h"
#include "cJSON.h"

FIL file;	    //文件变量

u8 nAsrStatus = 0;              //序运行的状态，意义如下
//	LD_ASR_NONE:		表示没有在作ASR识别
//	LD_ASR_RUNING：		表示LD3320正在作ASR识别中
//	LD_ASR_FOUNDOK:		表示一次识别流程结束后，有一个识别结果
//	LD_ASR_FOUNDZERO:	表示一次识别流程结束后，没有识别结果
//	LD_ASR_ERROR:		表示一次识别流程中LD3320芯片内部出现不正确的状态

/*-------------------------------------------------*/
/*函数名：ld3320启动识别                           */
/*参  数：b8 语音识别超时时长（秒）                                      */
/*返回值：1：成功  0：失败                         */
/*-------------------------------------------------*/
u8 RunASR(u8 b8, char* path)
{
    u8 i = 0;                         //用于for循环
    u8 asrflag = 0;                   //标志位

    for (i = 0; i < 5; i++)			  //防止由于硬件原因导致LD3320芯片工作不正常，所以一共尝试5次启动ASR识别流程
    {
        LD_Init_ASR(b8);			      //初始化ASR
        delay_ms(5);                  //延时5ms
        if (LD_AsrAddFixed(path) == 0)	 //添加关键词语到LD3320芯片中
        {
            LD_reset();			      //添加失败，立即重启LD3320芯片
            delay_ms(5);		      //延时5ms
            continue;                 //重来一次
        }
        delay_ms(10);                 //添加成功，延时5ms

        if (LD_AsrRun() == 0)         //执行语音识别的流程
        {
            LD_reset();			      //失败，立即重启LD3320芯片
            delay_ms(5);		      //延时5ms
            continue;                 //重来一次
        }

        asrflag = 1;                  //标志位=1，表示成功
        break;					      //ASR流程启动成功，退出当前for循环。
    }

    return asrflag;                   //返回标志位 如果5次都不成功返回0  成功返回1
}

/*-------------------------------------------------*/
/*函数名：初始化识别流程                           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LD_Init_ASR(u8 b8)
{
    nLD_Mode = LD_MODE_ASR;
    LD_Init_Common();

    LD_WriteReg(0xBD, 0x00);
    LD_WriteReg(0x17, 0x48);
    delay_ms(5);
    LD_WriteReg(0x3C, 0x80);
    LD_WriteReg(0x3E, 0x07);
    LD_WriteReg(0x38, 0xff);
    LD_WriteReg(0x3A, 0x07);
    LD_WriteReg(0x40, 0);
    LD_WriteReg(0x42, 8);
    LD_WriteReg(0x44, 0);
    LD_WriteReg(0x46, 8);
    LD_WriteReg(0xB8, b8);
    delay_ms(1);
}

/*-------------------------------------------------*/
/*函数名：检测是否识别 忙                          */
/*参  数：无                                       */
/*返回值：0：忙 1：空闲                            */
/*-------------------------------------------------*/
u8 LD_Check_ASRBusyFlag_b2(void)
{
    u8 j;
    u8 flag = 0;

    for (j = 0; j < 10; j++)
    {
        if (LD_ReadReg(0xb2) == 0x21)
        {
            flag = 1;
            break;
        }
        delay_ms(10);
    }
    return flag;
}

/*-------------------------------------------------*/
/*函数名：执行识别流程函数                         */
/*参  数：无                                       */
/*返回值：0：识别忙 1：正确                        */
/*-------------------------------------------------*/
u8 LD_AsrRun(void)
{
    LD_WriteReg(0x35, MIC_VOL);
    LD_WriteReg(0x1C, 0x09);
    LD_WriteReg(0xBD, 0x20);
    LD_WriteReg(0x08, 0x01);
    delay_ms(5);
    LD_WriteReg(0x08, 0x00);
    delay_ms(5);

    if(LD_Check_ASRBusyFlag_b2() == 0)
    {
        return 0;
    }

    LD_WriteReg(0xB2, 0xff);
    LD_WriteReg(0x37, 0x06);
    LD_WriteReg(0x37, 0x06);
    delay_ms(5);
    LD_WriteReg(0x1C, 0x0b);
    LD_WriteReg(0x29, 0x10);

    LD_WriteReg(0xBD, 0x00);
    return 1;
}

/*-------------------------------------------------*/
/*函数名：获取识别结果                             */
/*参  数：无                                       */
/*返回值：结果                                     */
/*-------------------------------------------------*/
u8 LD_GetResult(void)
{
    return LD_ReadReg(0xc5);
}

/*-------------------------------------------------*/
/*函数名：添加关键词                               */
/*参  数：无                                       */
/*返回值：1：成功                                  */
/*-------------------------------------------------*/
u8 LD_AsrAddFixed(char* path)
{
    u8 i, k, j, flag;
    u8 nAsrAddLength;
    u8 pCode[DATE_A];
    u8 sRecog[DATE_A][DATE_B];
    cJSON *root;
    char voice_buf[2048];
    //if(sizeof(voice_buf)/sizeof(voice_buf[0]) == 1)
    //if(voice_buf[0] == 1)
    //{
    UINT br;
    u8 res;
    res = f_open(&file, path, FA_OPEN_EXISTING | FA_READ ); //打开一个mp3文件
    if(res != FR_OK)                                           //如果不存在
    {
        printf("1该文件不存在:%d\r\n", res);                           //串口显示信息
        f_close(&file);                                    //关闭文件
        return res;		                                       //返回错误值
        //TODO...
    }
    f_read (&file, voice_buf, f_size(&file), &br);			//读取文件到buf
    f_close(&file);
    //}
    printf("buf:%s\r\n",voice_buf);
    root = cJSON_Parse(voice_buf);
    for(i = 0; i < cJSON_GetArraySize(root); i++)
    {
        cJSON * item = cJSON_GetArrayItem(root, i);
        cJSON * N = cJSON_GetObjectItem(item, "N");
        cJSON * C = cJSON_GetObjectItem(item, "C");
        char *c = C->valuestring;
        pCode[i] = N->valueint;
        for(j = 0; j < strlen(c); j++)
        {
            sRecog[i][j] = c[j];
        }
        sRecog[i][j] = NULL;
        //printf("%d->", N->valueint);
        //printf("%s\r\n", c);
    }
    pCode[i]=NULL;
    if(root)cJSON_Delete(root);
    /*
    u8 sRecog[DATE_A][DATE_B] =    //关键词列表，汉语拼音。用户自己修改
    {
    	"chong qi xi tong",\
    	"jiang ge xiao hua",\
    	"xiao bei xiao bei",\
    	"bei ke wu lian",               };

    u8  pCode[DATE_A] =            //关键词，对应的识别码。用户自己修改
    {
    	CODE_CQXT,\
    	CODE_JGXH,\
    	CODE_DK,\
    	CODE_GB,                };*/

    flag = 1;
    for (k = 0; k < DATE_A; k++)
    {
        if(LD_Check_ASRBusyFlag_b2() == 0)
        {
            flag = 0;
            break;
        }
        if (pCode[k] == NULL)
            break;
        printf("%d:", pCode[k]);
        LD_WriteReg(0xc1, pCode[k] );
        LD_WriteReg(0xc3, 0 );
        LD_WriteReg(0x08, 0x04);
        delay_ms(1);
        LD_WriteReg(0x08, 0x00);
        delay_ms(1);

        for (nAsrAddLength = 0; nAsrAddLength < DATE_B; nAsrAddLength++)
        {
            if (sRecog[k][nAsrAddLength] == NULL)
                break;
            LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
            printf("%c", sRecog[k][nAsrAddLength]);
        }
        printf("\r\n");
        LD_WriteReg(0xb9, nAsrAddLength);
        LD_WriteReg(0xb2, 0xff);
        LD_WriteReg(0x37, 0x04);
    }

    return flag;
}

/*-------------------------------------------------*/
/*函数名：识别处理函数                             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void ProcessInt_ASR(void)
{
    u8 nAsrResCount = 0;

    ucRegVal = LD_ReadReg(0x2B);

    // 语音识别产生的中断
    // 有声音输入，不论识别成功或失败都有中断
    LD_WriteReg(0x29, 0) ;
    LD_WriteReg(0x02, 0) ;

    if((ucRegVal & 0x10) && LD_ReadReg(0xb2) == 0x21 && LD_ReadReg(0xbf) == 0x35)
    {
        nAsrResCount = LD_ReadReg(0xba);

        if(nAsrResCount > 0 && nAsrResCount <= 4)
        {
            nAsrStatus = LD_ASR_FOUNDOK;
        }
        else
        {
            nAsrStatus = LD_ASR_FOUNDZERO;
        }
    }
    else
    {
        nAsrStatus = LD_ASR_FOUNDZERO;	//执行没有识别
    }

    LD_WriteReg(0x2b, 0);
    LD_WriteReg(0x1C, 0); /*写0:ADC不可用*/

    LD_WriteReg(0x29, 0) ;
    LD_WriteReg(0x02, 0) ;
    LD_WriteReg(0x2B, 0);
    LD_WriteReg(0xBA, 0);
    LD_WriteReg(0xBC, 0);
    LD_WriteReg(0x08, 1);	/*清除FIFO_DATA*/
    LD_WriteReg(0x08, 0);	/*清除FIFO_DATA后 再次写0*/

    delay_ms(1);
}
void Stop_ASR(void)
{
    LD_WriteReg(0xBC, 0x08);
}
