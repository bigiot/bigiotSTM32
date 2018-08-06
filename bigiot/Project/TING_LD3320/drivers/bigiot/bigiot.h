#ifndef __BIGIOT_H
#define	__BIGIOT_H

/*发送登录信息*/
void checkin(char *DEVICEID,char *APIKEY);
/*退出登录*/
void checkout(char *DEVICEID,char *APIKEY);
/*检查当前登录状态*/
void check_status(void);
/*发送指令到目标单位*/
void say(char *toID, char *content);
/*上传一个接口的实时数据*/
void update1(char *did, char *inputid, float value);
#endif
