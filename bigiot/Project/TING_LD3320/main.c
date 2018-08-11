/**
  ******************************************************************************
  * @file    main.c
  * @author  www.bigiot.net
  * @version V0.1
  * @date    2017-6-9
  * @brief   STM32F103C8T6_ESP01_LED，设备登录为不加密模式。
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h"
#include "sd.h"
#include "ff.h"
#include "ld3320.h"
#include "mp3.h"
#include "led.h"
#include "asr.h"
#include "cJSON.h"
#include "millis.h"
#include "bigiot.h"

//FATFS fs[_VOLUMES];

FATFS fs;
//FIL config_file2;

//char buf[2048];

unsigned long lastCheckStatusTime = 0;
unsigned long lastCheckOutTime = 0;
unsigned long lastSayTime = 0;
unsigned long lastWakeTime = 0;
const unsigned long postingInterval = 40000;
const unsigned long statusInterval = 60000;
bigiot_state BGT_STA = OFF_LINE;
unsigned int waked = 0;
/*以下两个变量必须修改为自己的设备ID及APIKEY，在贝壳物联用户中心添加设备即可获得*/
char *DEVICEID = "458";
char *APIKEY = "9cb787949";

/*用CJSON处理接收到的信息*/
int processMessage(char *msg) {
    cJSON *jsonObj = cJSON_Parse(msg);
    cJSON *method;
    char *m;
    //json字符串解析失败，直接退出
    printf("网络接收：%s", msg);
    if(!jsonObj)
    {
        //uart1.USART2_printf("json string wrong!");
        return 0;
    }
    method = cJSON_GetObjectItem(jsonObj, "M");
    m = method->valuestring;
    if(strncmp(m, "WELCOME", 7) == 0)
    {
        BGT_STA = CONNECTED;
        //防止设备在线状态未消除，先登出
        checkout(DEVICEID, APIKEY);
        lastCheckOutTime = millis();
    }
    if(strncmp(m, "connected", 9) == 0)
    {
        BGT_STA = CONNECTED;
        checkin(DEVICEID, APIKEY);
    }
    //{"M":"checkout","IP":"xx1","T":"xx2"}\n
    if(strncmp(m, "checkout", 8) == 0)
    {
        BGT_STA = FORCED_OFF_LINE;
    }
    //{"M":"checkinok","ID":"xx1","NAME":"xx2","T":"xx3"}\n
    if(strncmp(m, "checkinok", 9) == 0)
    {
        BGT_STA = CHECKED;
    }
    if(strncmp(m, "checked", 7) == 0)
    {
        BGT_STA = CHECKED;
    }
    //有设备或用户登录，发送欢迎信息
    if(strncmp(m, "login", 5) == 0)
    {
        char *from_id = cJSON_GetObjectItem(jsonObj, "ID")->valuestring;
        char new_content[] = "Dear friend, welcome to BIGIOT !";
        say(from_id, new_content);
    }
    //收到say指令，执行相应动作，并进行相应回复
    if(strncmp(m, "say", 3) == 0 && millis() - lastSayTime > 10)
    {
        char *content = cJSON_GetObjectItem(jsonObj, "C")->valuestring;
        char *from_id = cJSON_GetObjectItem(jsonObj, "ID")->valuestring;
        lastSayTime = millis();
        if(strncmp(content, "play", 4) == 0)
        {
            char new_content[] = "led played";
            //do something here....
            LED_D3 = 0;
            say(from_id, new_content);
        }
        else if(strncmp(content, "stop", 4) == 0)
        {
            char new_content[] = "led stoped";
            //do something here....
            LED_D3 = 1;
            say(from_id, new_content);
        }
    }
    if(jsonObj)cJSON_Delete(jsonObj);
    return 1;
}


void setup(void)
{
    delay_init();//延时初始化
    USART1_init(115200);
    USART2_init(115200);
    LED_Init(); 	                   //LED初始化
    while(SD_Init())                   //SD卡初始化
    {
        printf("SD卡初始化错误\r\n");  //串口提示SD卡初始化错误
        delay_ms(2000);                //延时2s
    }
    f_mount(&fs, "0:", 1); 	     //挂载SD卡
    LD3320_Init();	                   //初始化LD3320
    MILLIS_Init();
}

u8 write_file(char *path)
{
    u8 res;
    UINT br;
    FIL file;
    char wakeup[] = "[{\"N\":1,\"C\":\"xiao bei xiao bei\"},{\"N\":2,\"C\":\"bei ke wu lian\"},{\"N\":3,\"C\":\"xiao zhu xiao zhu\"},{\"N\":4,\"C\":\"bei ke kei kei\"}]";
    res = f_open (&file, path, FA_OPEN_ALWAYS | FA_WRITE);			//打开文件,不存在则新建.
    if(res != FR_OK)
    {
        printf("打开失败:%s\r\n", path);                           //串口显示信息
        f_close(&file);                                    //关闭文件
        return res;
    }
    //res=f_lseek(&file,f_size(&file)); 										    //移动读写指针(移动到文件的结尾添加数据)
    res = f_lseek(&file, 0);                                                         //移动读写指针(移动到文件的开头)
    f_write (&file, wakeup, sizeof(wakeup) - 1, &br);	//写入数据
    f_close(&file);		//关闭文件
    printf("write file OK!%d\r\n", res);
    return res;
}
u8 write_file2(char *path)
{
    u8 res;
    UINT br;
    FIL file;
    char wakeup[] = "{\"C0\":{\"ID\":\"6589\",\"K\":\"658bb0ee3\",\"BR\":9600,\"CI\":10,\"CC\":1,\"SPK\":1},\"C1\":{\"M\":\"WAKE\",\"W\":\"wake\"},\"C2\":{\"M\":\"LED\",\"V\":1,\"W\":\"ledon\"}}";
    res = f_open (&file, path, FA_OPEN_ALWAYS | FA_WRITE);			//打开文件,不存在则新建.
    if(res != FR_OK)
    {
        printf("打开失败:%s\r\n", path);                           //串口显示信息
        f_close(&file);                                    //关闭文件
        return res;
    }
    //res=f_lseek(&file,f_size(&file)); 										    //移动读写指针(移动到文件的结尾添加数据)
    res = f_lseek(&file, 0);                                                         //移动读写指针(移动到文件的开头)
    f_write (&file, wakeup, sizeof(wakeup) - 1, &br);	//写入数据
    f_close(&file);		//关闭文件
    printf("write file2 OK!%d\r\n", res);
    return res;
}

void voice_command(u8 num,char * path)
{
    FIL file;
    UINT br;
    u8 res;
    char config_buf[2048];
    cJSON *root;
    cJSON *C_n;
    cJSON *C_n_m;
    char *c_n_str;
    res=f_open(&file,path,FA_OPEN_EXISTING | FA_READ );    //打开一个mp3文件
	if(res != FR_OK)                                           //如果不存在
	{
		printf("2该文件不存在:%d\r\n",res);                            //串口显示信息
		f_close(&file);                                    //关闭文件
		return;		                                       //返回错误值
        //TODO...
	}
    f_read (&file, config_buf,f_size(&file),&br);			//读取文件到buf
    f_close(&file);	
    printf("file:%s\r\n",config_buf);
    root = cJSON_Parse(config_buf);
    //printf("%s\r\n", cJSON_Print(root));
    sprintf(c_n_str,"%s%d","C",num);
    printf("c_n_str:%s\r\n",c_n_str);
    C_n = cJSON_GetObjectItem(root, "C1");
    C_n_m = cJSON_GetObjectItem(C_n, "M");
    printf("c_n_m:%s\r\n",C_n_m->valuestring);
    if(strncmp(C_n_m->valuestring, "WAKE", 4) == 0)
    {
        printf("waked\r\n");
    }
    if(strncmp(C_n_m->valuestring, "LED", 3) == 0)
    {
        printf("LEDed\r\n");
    }
    if(root)cJSON_Delete(root);
    return;
}
int main(void)
{
    u8 nAsrRes = 0;
    u16 len;
    setup();
    write_file("asr.txt");
    write_file2("config.txt");
    //read_wakeup_file("0:/message.txt");
    printf(" 口令1：重启系统\r\n ");
    printf(" 口令2：讲个笑话\r\n ");
    printf(" 口令3：打开\r\n ");
    printf(" 口令4：关闭\r\n ");
    nAsrStatus = LD_ASR_NONE;		     //初始状态：没有在作ASR
    PlayDemoSound_mp3("系统准备.mp3");   //播放文件
    //while(bMp3Play == 0);                //语音数据发送完毕
    //delay_ms(5000);

    while (1)
    {
        if ( BGT_STA == CONNECTED && lastCheckOutTime != 0 && millis() - lastCheckOutTime > 200) {
            checkin(DEVICEID, APIKEY);
            lastCheckOutTime = 0;
        }
        if (millis() - lastCheckStatusTime > statusInterval || (lastCheckStatusTime == 0 && millis() > 5000)) {
            check_status();
            lastCheckStatusTime = millis();
        }

        if(USART2_RX_STA & 0x8000)
        {
            len = USART2_RX_STA & 0x3fff;                   //得到此次接收到的数据长度
            printf("收到长度：%d\r\n", len);
            USART2_RX_BUF[len] = '\0';
            //printf("usart2 收到数据：%s\r\n",USART2_RX_BUF);
            processMessage((char*)USART2_RX_BUF);
            USART2_RX_STA = 0;
        }
        //printf("bmp3play\r\n");
        if(bMp3Play == 1)continue;
        if (millis() - lastWakeTime > 11000 && lastWakeTime != 0 )
        {
            waked = 0;
            lastWakeTime = 0;
            Stop_ASR();
        }
        switch(nAsrStatus)
        {
        case LD_ASR_RUNING:
            break;

        case LD_ASR_ERROR:
            printf("LD3320芯片内部出现不正确。\r\n");
            break;

        case LD_ASR_NONE:
            nAsrStatus = LD_ASR_RUNING;            //启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算
            if (RunASR(0xff, "asr.txt") == 0)
            {
                printf("ASR_ERROR\r\n");
                nAsrStatus = LD_ASR_ERROR;
            }
            break;

        case LD_ASR_FOUNDOK:
            nAsrRes = LD_ReadReg(0xc5);	            //一次ASR识别成功结束，取ASR识别结果
            voice_command(nAsrRes,"config.txt");
            if(nAsrRes == 1)
            {
                waked = 1;
                lastWakeTime = millis();
                printf("接收到口令：重启系统\r\n");
                nAsrStatus = LD_ASR_NONE;
                break;
            }
            if(waked != 1)
            {
                printf("未唤醒。\r\n");
                nAsrStatus = LD_ASR_NONE;
                break;
            }
            if(nAsrRes == 2)
            {
                printf("接收到口令：讲个笑话\r\n");
            }
            if(nAsrRes == 3)
            {
                printf("接收到口令：打开\r\n");
            }
            if(nAsrRes == 4)
            {
                printf("接收到口令：关闭\r\n");
            }
            
            printf("不在口令集之内\r\n");
            nAsrStatus = LD_ASR_NONE;
            break;

        case LD_ASR_FOUNDZERO:
            printf("未知口令\r\n");
            nAsrStatus = LD_ASR_NONE;
            break;

        default:
            nAsrStatus = LD_ASR_NONE;
            break;
        }
    }
}

