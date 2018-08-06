/*-------------------------------------------------*/
/*         超纬电子STM32F407ZGT6开发板             */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/
/*-------------------------------------------------*/
/*            FAT文件系统移植的关键文件            */
/*        实现文件系统和硬件磁盘的连接函数         */
/*-------------------------------------------------*/

#include "diskio.h"		
#include "sd.h"
#include "mymalloc.h"		

#define SD_CARD	 0                  //SD卡,编号为0
#define FLASH_SECTOR_SIZE 	512	    //扇区大小		  
#define FLASH_BLOCK_SIZE   	8     	//每个BLOCK有8个扇区

/*-------------------------------------------------*/
/*函数名：初始化磁盘接口函数                       */
/*参  数：pdrv：磁盘编号                           */
/*返回值：错误状态值                               */
/*-------------------------------------------------*/
DSTATUS disk_initialize (BYTE pdrv)
{
	u8 res=0;	    
	switch(pdrv)          //判断哪个磁盘
	{
		case SD_CARD:   res=SD_Init();//SD卡初始化
						if(res)       
						{
							SD_SPI_SpeedLow();
							SD_SPI_ReadWriteByte(0xff);//提供额外的8个时钟
							SD_SPI_SpeedHigh();
						}
						break;		
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;  //初始化失败返回STA_NOINIT：1
	else return 0;              //初始化成功返回0
}

/*-------------------------------------------------*/
/*函数名：获得磁盘状态接口函数                     */
/*参  数：pdrv：磁盘编号                           */
/*返回值：错误状态值                               */
/*-------------------------------------------------*/
DSTATUS disk_status (BYTE pdrv)
{ 
	return 0;   //直接返回0，表示成功，没有具体的操作
} 

/*-------------------------------------------------*/
/*函数名：读扇区接口函数                           */
/*参  数：pdrv：磁盘编号                           */
/*参  数：buff: 接收地址                           */
/*参  数：count:需要读取的扇区数                   */
/*返回值：错误状态值                               */
/*-------------------------------------------------*/
DRESULT disk_read (BYTE pdrv,BYTE *buff,DWORD sector,UINT count)
{
	u8 res=0; 
    if (!count)return RES_PARERR;     //需要读取的扇区数不能等于0，否则返回参数错误		 	 
	switch(pdrv)                      //判断哪个磁盘
	{
		case SD_CARD:   res=SD_ReadDisk(buff,sector,count);	 
						if(res)
						{
							SD_SPI_SpeedLow();
							SD_SPI_ReadWriteByte(0xff);//提供额外的8个时钟
							SD_SPI_SpeedHigh();
						}
						break;
		default:
			res=1; 
	}
    if(res==0x00)return RES_OK;	   //正确返回RES_OK
    else return RES_ERROR;	       //错误返回RES_ERROR
}

/*-------------------------------------------------*/
/*函数名：写扇区接口函数                           */
/*参  数：pdrv：磁盘编号                           */
/*参  数：buff: 写入缓冲区                         */
/*参  数：count:需要写入的扇区数                   */
/*返回值：错误状态值                               */
/*-------------------------------------------------*/

#if _USE_WRITE   //开启读权限，"diskio.h"中设置

DRESULT disk_write (BYTE pdrv,const BYTE *buff,DWORD sector,UINT count)
{
	u8 res=0;  
    if (!count)return RES_PARERR;         //需要写入的扇区数不能等于0，否则返回参数错误		 	 
	switch(pdrv)                                        //判断哪个磁盘
	{ 
		case SD_CARD:    res=SD_WriteDisk((u8*)buff,sector,count);   //写扇区 
						 break;
		default:
			res=1; 
	}
    if(res==0x00)return RES_OK;	   //正确返回RES_OK
    else return RES_ERROR;	       //错误返回RES_ERROR	
}
#endif

/*-------------------------------------------------*/
/*函数名：指令控制                                 */
/*参  数：pdrv：磁盘编号                           */
/*参  数：ctrl:控制代码                            */
/*参  数：buff:发送/接收缓冲区指针                 */
/*返回值：错误状态值                               */
/*-------------------------------------------------*/

#if _USE_IOCTL     //开启控制权限，"diskio.h"中设置

DRESULT disk_ioctl (BYTE pdrv,BYTE cmd,void *buff)
{
	DRESULT res;						  			     
	if(pdrv==SD_CARD) //判断是SD卡
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				SD_CS=0;
		        if(SD_WaitReady()==0)res = RES_OK; 
		        else res = RES_ERROR;	  
				SD_CS=1;
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SD_GetSectorCount();
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}
	else res=RES_ERROR;//其他的不支持
    return res;
}
#endif

/*-------------------------------------------------*/
/*函数名：获得时间                                 */
/*参  数：无                                       */
/*返回值：                                         */
/*-------------------------------------------------*/                                                                                                                                                                                                                                               
DWORD get_fattime (void)
{				 
	return 0;   //没实现这个功能
}

/*-------------------------------------------------*/
/*函数名：动态分配内存                             */
/*参  数：size：需要的空间大小                     */
/*返回值：分配的内存指针                           */
/*-------------------------------------------------*/    
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(size);
}

/*-------------------------------------------------*/
/*函数名：释放内存                                 */
/*参  数：需要释放的内存的指针                     */
/*返回值：无                                       */
/*-------------------------------------------------*/  
void ff_memfree (void* mf)		 
{
	myfree(mf);
}

















