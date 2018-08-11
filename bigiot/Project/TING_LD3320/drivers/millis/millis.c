#include "stm32f10x.h"
#include "millis.h"

void MILLIS_Init(void)
{
#ifdef USE_LSE
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); //使能电源时钟和备份区域时钟

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);     //取消备份区写保护

    /* Reset Backup Domain */

    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)   //判断是否被唤醒
    {
        PWR_ClearFlag(PWR_FLAG_WU); //清除唤醒标志
    } else
    {
        BKP_DeInit();    //复位备份区域
    }

    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}

    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC ALARM */
    RTC_ITConfig(RTC_IT_ALR, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();


    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

#else
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    //中断时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);





    //中断优先级配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    //设置RTC闹钟中断
    NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //闹钟中断接到第17线外部中断
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    //PWR_WakeUpPinCmd(DISABLE);

    //电源管理部分时钟开启
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    //使能后备寄存器访问
    PWR_BackupAccessCmd(ENABLE);


    /* Reset Backup Domain */

    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)   //判断是否被唤醒
    {
        PWR_ClearFlag(PWR_FLAG_WU); // 清除唤醒标志
    } else
    {
        BKP_DeInit();    //复位备份区域
    }

    //BKP_ClearFlag();
    //    BKP_DeInit();
    //??LSI
    RCC_LSICmd(ENABLE);
    //??????
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {}
    //?????????
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    RCC_RTCCLKCmd(ENABLE);
    //??RTC_CTL?????RSF?(???????)????1
    RTC_WaitForSynchro();
    RTC_WaitForLastTask();
    //??????
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    RTC_WaitForLastTask();
    //????
    RTC_SetPrescaler(32); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32+1) */
    RTC_WaitForLastTask();
    //?????
    RTC_SetCounter(0);
    RTC_WaitForLastTask();
#endif
}

void RTCAlarm_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line17);
        RTC_ClearITPendingBit(RTC_IT_ALR);
    }

    return;
}
void RTC_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_ALR) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line17);   //???????17?????
        RTC_ClearITPendingBit(RTC_IT_ALR);
        RTC_WaitForLastTask();
    }
    else if(RTC_GetITStatus(RTC_IT_SEC) == SET)
    {
        RTC_ClearITPendingBit(RTC_IT_SEC);
    }
}
unsigned long millis(void)//???????????????(ms)
{
    return RTC_CURRENT_VAL;
}
