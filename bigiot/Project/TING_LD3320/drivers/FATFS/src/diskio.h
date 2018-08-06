/*-------------------------------------------------*/
/*         超纬电子STM32F407ZGT6开发板             */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef _DISKIO_DEFINED
/*-------------------------------------------------*/
/*         超纬电子STM32F407ZGT6开发板             */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#define _USE_WRITE	1	//读权限
#define _USE_IOCTL	1	//控制权限

#include "integer.h"

typedef BYTE	DSTATUS;   //保存结构代码

//各种错误状态
typedef enum {
	RES_OK = 0,		//正确
	RES_ERROR,		//读写错误
	RES_WRPRT,		//读保护
	RES_NOTRDY,		//未就绪
	RES_PARERR		//无效的参数
} DRESULT;


DSTATUS disk_initialize (BYTE pdrv);                                        //初始化硬件磁盘
DSTATUS disk_status (BYTE pdrv);                                            //获得磁盘状态 
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);        //读磁盘扇区  
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count); //写磁盘扇区  
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);                       //控制函数
DWORD get_fattime (void);       //获得时间
void *ff_memalloc (UINT size);  //动态分配内存 
void ff_memfree (void* mf);     //释放内存

//状态表示
#define STA_NOINIT		0x01	//未初始化
#define STA_NODISK		0x02	//未发现磁盘
#define STA_PROTECT		0x04	//写保护


//以下不常用，未作翻译
/* Command code for disk_ioctrl fucntion */
/* Generic command (used by FatFs) */
#define CTRL_SYNC			0	/* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT	1	/* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE		2	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR	4	/* Force erased a block of sectors (for only _USE_ERASE) */

/* Generic command (not used by FatFs) */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */
#define CTRL_FORMAT			8	/* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */

#ifdef __cplusplus
}
#endif

#endif
