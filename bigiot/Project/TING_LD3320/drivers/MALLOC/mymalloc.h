#ifndef _MY_MALLOC_
#define _MY_MALLOC_

typedef struct           //定义一个内存块管理结构体
{
	uint8_t  mem_state;  //当前对应内存卡的状态  0 可用  1被占用
	uint8_t  num;        //连续占用的内存块数量 
}MEM_BCT;

void mymen_Init(void);
void *mymalloc(uint32_t size);
void *myfree(void *p);

#endif
