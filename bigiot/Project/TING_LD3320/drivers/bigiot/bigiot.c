#include "stm32f10x.h"
#include "usart2.h"
#include "bigiot.h"

/*发送登录信息*/
void checkin(char *DEVICEID,char *APIKEY)
{
    USART2_printf("{\"M\":\"checkin\",\"ID\":\"%s\",\"K\":\"%s\"}\n", DEVICEID, APIKEY);
}
/*退出登录*/
void checkout(char *DEVICEID,char *APIKEY)
{
    USART2_printf("{\"M\":\"checkout\",\"ID\":\"%s\",\"K\":\"%s\"}\n", DEVICEID, APIKEY);
}
/*检查当前登录状态*/
void check_status(void)
{
    USART2_printf("{\"M\":\"status\"}\n");
}
/*发送指令到目标单位*/
void say(char *toID, char *content)
{
    USART2_printf("{\"M\":\"say\",\"ID\":\"%s\",\"C\":\"%s\"}\n", toID, content);
}
/*上传一个接口的实时数据*/
void update1(char *did, char *inputid, float value) {
    USART2_printf("{\"M\":\"update\",\"ID\":\"%s\",\"V\":{\"%s\":\"%f\"}}\n", did, inputid, value);
}
