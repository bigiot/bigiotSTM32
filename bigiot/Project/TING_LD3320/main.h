/*-------------------------------------------------*/
/*               3??3¦Ì?¡Á¨®?a¡¤¡é¡ã?                    */
/*    ¨¬?¡À|¦Ì??¡¤¡êo http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef __MAIN_H
#define __MAIN_H	

#include "stm32f10x.h"
																	    
//??¡ä?2¨´¡Á¡Â,¨º¦Ì??51¨¤¨¤??¦Ì?GPIO????1|?¨¹
//??¨¬?¨º¦Ì??????,2???<<CM3¨¨¡§¨ªt????>>¦Ì¨²????(87¨°3~92¨°3).
//IO?¨²2¨´¡Á¡Âo¨º?¡§¨°?
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO?¨²¦Ì??¡¤¨®3¨¦?
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO?¨²2¨´¡Á¡Â,????¦Ì£¤¨°?¦Ì?IO?¨²!
//¨¨¡¤¡À¡ên¦Ì??¦ÌD?¨®¨²16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //¨º?3? 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //¨º?¨¨? 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //¨º?3? 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //¨º?¨¨? 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //¨º?3? 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //¨º?¨¨? 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //¨º?3? 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //¨º?¨¨? 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //¨º?3? 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //¨º?¨¨?

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //¨º?3? 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //¨º?¨¨?

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //¨º?3? 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //¨º?¨¨?
typedef enum {
    OFF_LINE = 0,
    CONNECTED,
    CHECKED,
    FORCED_OFF_LINE
}bigiot_state;
#endif
