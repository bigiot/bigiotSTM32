/*-------------------------------------------------*/
/*               超纬电子开发板                    */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#include "mp3.h"
#include "ld3320.h"
#include "delay.h"
#include "ff.h"   
#include "usart1.h"

FIL   MP3_file;	  		//文件变量
UINT  Read_Bytes;       //变量，用于保存读取操作，实际读取的个数
UINT  Write_Bytes ;     //变量，用于保存写取操作，实际写入的个数

u32 nMp3Size=0;			//mp3文件的大小
u32 nMp3Pos=0;			//mp3文件的偏移(最后的偏移就是文件大小)
u8  bMp3Play=0;			//用来记录播放MP3的状态  0：空闲  1：播放

/*-------------------------------------------------*/
/*函数名：播放MP3                                  */
/*参  数：path：MP3文件路径                        */
/*返回值：0：正确  其他：错误                      */
/*-------------------------------------------------*/
u8 PlayDemoSound_mp3(char *path)
{
	u8 res;
	
	res=f_open(&MP3_file,path,FA_OPEN_EXISTING | FA_READ );    //打开一个mp3文件
	if(res != FR_OK)                                           //如果不存在
	{
		printf("该文件不存在\r\n");                            //串口显示信息
		f_close(&MP3_file);                                    //关闭文件
		return res;		                                       //返回错误值
	}

	bMp3Play = 1;                              //1：播放状态
	nMp3Size = f_size(&MP3_file)-1;            //读取文件大小	
	printf("文件大小=%d字节\r\n",nMp3Size);    //串口显示信息
	
	LD_Init_MP3();                             //将LD3320初始化为播放MP3模式		
	LD_play();                                 //开始播放

    return 0;
}

/*-------------------------------------------------*/
/*函数名：初始化MP3模式                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/ 
void LD_Init_MP3(void)	
{
	nLD_Mode = LD_MODE_MP3;	   //当前进行MP3播放
	LD_Init_Common();		   //通用初始化

	LD_WriteReg(0xBD,0x02);	   //内部增益控制 初始化时写入FFH
	LD_WriteReg(0x17, 0x48);   //写48H可以激活DSP
	delay_ms(100);

	LD_WriteReg(0x85, 0x52); 	//内部反馈设置 初始化时写入52H
	LD_WriteReg(0x8F, 0x00);  	//LineOut(线路输出)选择 初始化时写入00H
	LD_WriteReg(0x81, 0x00);	//耳机左音量 设置为00H为最大音量
	LD_WriteReg(0x83, 0x00);	//耳机右音量 设置为00H为最大音量
	LD_WriteReg(0x8E, 0x05);	//喇叭输出音量  本寄存器设置为00H为最大音量	此处声音关闭
	LD_WriteReg(0x8D, 0xff);	//内部增益控制 初始化时写入FFH
	delay_ms(100);
	LD_WriteReg(0x87, 0xff);	//模拟电路控制 MP3播放初始化时写 FFH
	LD_WriteReg(0x89, 0xff);    //模拟电路控制 MP3播放时写 FFH
	delay_ms(100);
	LD_WriteReg(0x22, 0x00);   //FIFO_DATA下限低8位
	LD_WriteReg(0x23, 0x00);	//FIFO_DATA下限高8位
	LD_WriteReg(0x20, 0xef);   //FIFO_DATA上限低8位
	LD_WriteReg(0x21, 0x07);	//FIFO_DATA上限高8位
	LD_WriteReg(0x24, 0x77);    
	LD_WriteReg(0x25, 0x03);	
	LD_WriteReg(0x26, 0xbb);    
	LD_WriteReg(0x27, 0x01); 	
}

/*-------------------------------------------------*/
/*函数名：准备播放mp3                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/ 
void LD_play(void)	 
{
	nMp3Pos=0;         
	bMp3Play=1;

	if (nMp3Pos >=  nMp3Size)
		return ; 

	fill_the_fifo();

    LD_WriteReg(0xBA, 0x00);
	LD_WriteReg(0x17, 0x48);
	LD_WriteReg(0x33, 0x01);
	LD_WriteReg(0x29, 0x04);
	
	LD_WriteReg(0x02, 0x01); 
	LD_WriteReg(0x85, 0x5A);

}

/*-------------------------------------------------*/
/*函数名：填满fifo为播放mp3做准备                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/ 
void fill_the_fifo(void)
{
    u8 ucStatus;
	
	ucStatus = LD_ReadReg(0x06);
	while (!(ucStatus&MASK_FIFO_STATUS_AFULL))  //fifo是否满了
	{
		LD_WriteReg(0x01,0xff);
		ucStatus = LD_ReadReg(0x06);
	}	
}

/*-------------------------------------------------*/
/*函数名：继续读取mp3文件数据到fifo                */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/ 
void LD_ReloadMp3Data_Again(void)
{
	u8 val;
    u8 ucStatus;

	ucStatus = LD_ReadReg(0x06);
	while (!(ucStatus&MASK_FIFO_STATUS_AFULL) && nMp3Pos<=nMp3Size)	//fifo是否满了
	{
		nMp3Pos++;
		f_read(&MP3_file,&val,1,&Read_Bytes);	
		LD_WriteReg(0x01,val);
		ucStatus = LD_ReadReg(0x06);
	}

	if(nMp3Pos>=nMp3Size)
	{
	    LD_WriteReg(0xBC, 0x01);
		LD_WriteReg(0x29, 0x10);
		
		while(!(LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END));  //等待MP3播放完毕

		LD_WriteReg(0x2B,  0);
      	LD_WriteReg(0xBA, 0);	
		LD_WriteReg(0xBC,0x0);	
		bMp3Play=0;					// 声音数据全部播放完后，修改bMp3Play的变量
		LD_WriteReg(0x08,1);

      	LD_WriteReg(0x08,0);
		LD_WriteReg(0x33, 0);

		f_close(&MP3_file);                                    //关闭文件

	}		
}

/*-------------------------------------------------*/
/*函数名：中断处理函数                             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
u8 ucRegVal;   //寄存器备份变量
u8 ucHighInt;  //寄存器备份变量
u8 ucLowInt;   //寄存器备份变量

void ProcessInt_MP3(void)	 
{
	ucRegVal = LD_ReadReg(0x2B);

	// 声音播放产生的中断，有三种：
	// A. 声音数据已全部播放完。
	// B. 声音数据已发送完毕。
	// C. 声音数据暂时将要用完，需要放入新的数据。	
	ucHighInt = LD_ReadReg(0x29); 
	ucLowInt=LD_ReadReg(0x02); 
	LD_WriteReg(0x29,0) ;
	LD_WriteReg(0x02,0) ;
	
	// A. 声音数据已全部播放完。
    if(LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END)
    {
		LD_WriteReg(0x2B,  0);
      	LD_WriteReg(0xBA, 0);	
		LD_WriteReg(0xBC,0x0);	
		bMp3Play=0;					// 声音数据全部播放完后，修改bMp3Play的变量
		LD_WriteReg(0x08,1);

      	LD_WriteReg(0x08,0);
		LD_WriteReg(0x33, 0);
		f_close(&MP3_file);         //关闭文件
		return ;
     }
	
	// B. 声音数据已发送完毕。
	if(nMp3Pos>=nMp3Size)
	{	
		LD_WriteReg(0xBC, 0x01);
		LD_WriteReg(0x29, 0x10);
		f_close(&MP3_file);                                    //关闭文件
		return;	
	}

	// C. 声音数据暂时将要用完，需要放入新的数据。	
	LD_ReloadMp3Data_Again();			
	LD_WriteReg(0x29,ucHighInt); 
	LD_WriteReg(0x02,ucLowInt) ;
}
