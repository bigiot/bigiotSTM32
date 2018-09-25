#include <string.h>
#include "stm32f10x.h"
#include "usart1.h"
#include "spk.h"
#include "sd.h"
#include "ff.h"
/*
   字符数组发声
*/
void char2voice(char * str, int char_set)
{
  int H = 0;
  int L = 0;
  int length = strlen(str);
  if(length+2 > 255){
	  H = (length+2) / 256;
	  L = (length+2) % 256;
  } else {
	  L = length + 2;
  }
  UART1SendByte(0xFD);
  UART1SendByte(H);
  UART1SendByte(L);
  UART1SendByte(0x01);
  UART1SendByte(char_set);
  printf("%s",str);
}

/*
   二进制字符串转16进制，并颠倒大小端，发挥字节数
*/
int change(const char s[], char bits[]) {
  int i, n = 0;
  for (i = 0; s[i]; i += 2) {
    if (s[i] >= 'a' && s[i] <= 'f')
      bits[n] = s[i] - 'a' + 10;
    else bits[n] = s[i] - '0';
    if (s[i + 1] >= 'a' && s[i + 1] <= 'f')
      bits[n] = (bits[n] << 4) | (s[i + 1] - 'a' + 10);
    else bits[n] = (bits[n] << 4) | (s[i + 1] - '0');
    ++n;
  }
  bits[n] = '\0';
  return n;
}
/*
   缓存播放数据
   str:缓存内容,index:缓存索引[0,15]
*/
void saveCache(char * str, int index)
{
  int H = 0;
  int L = 0;
  int length = strlen(str);
  if(length+2 > 255){
	  H = (length+2) / 256;
	  L = (length+2) % 256;
  } else {
	  L = length + 2;
  }
  UART1SendByte(0xFD);
  UART1SendByte(H);
  UART1SendByte(L);
  UART1SendByte(0x31);
  UART1SendByte(index);
  printf("%s",str);
}
/*
    播放缓存
    times 播放缓存次数
    char_set 0:GB2312 1:GBK 2:BIG5 3:Unicode
*/
void playCache(int times, int char_set)
{
  char five = (times<<4)|char_set;
  UART1SendByte(0xFD);
  UART1SendByte(0x00);
  UART1SendByte(0x02);
  UART1SendByte(0x32);
  UART1SendByte(five);
}
/*
    播放文件内容
    fileName 文件名称，不含后缀 .txt
    char_set 0:GB2312 1:GBK 2:BIG5 3:Unicode
*/
int file2voice(char * path, int char_set) {
    FIL config_file2;
    UINT br;
    u8 res;
    size_t fileSize;
    char config_buf[512];
    res=f_open(&config_file2,path,FA_OPEN_EXISTING | FA_READ );
    if(res != FR_OK)                                           //如果不存在
	{
		char2voice("该文件不存在",0);                            //串口显示信息
		f_close(&config_file2);                                    //关闭文件
		return 0;		                                       //返回错误值
        //TODO...
	}
    f_read (&config_file2, config_buf,f_size(&config_file2),&br);			//读取文件到buf
    f_close(&config_file2);	

    fileSize = f_size(&config_file2);
    if (fileSize > 2048) {
      //Serial1.println("Config file size is too large");
      return 0;
    }
    UART1SendByte(0xFD);
    UART1SendByte(0x00);
    UART1SendByte(fileSize + 2);
    UART1SendByte(0x01);
    UART1SendByte(char_set);
    printf("%s",config_buf);
    return 1;
}
