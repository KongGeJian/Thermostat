/*
*********************************************************************************************************
*                                        BOARD SUPPORT PACKAGE
*
* Name    : 蜂鸣器
* Version : V1.0
* Author  : 空格键
* ----------------------------
* Note(s) : 有源蜂鸣器。S8050 NPN三极管驱动。高电平发声
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "bsp_buzzer.h"



/*
*********************************************************************************************************
*                                               DEFINE
*********************************************************************************************************
*/

#define BZ P10



/*
*********************************************************************************************************
* Description : 蜂鸣器初始化。
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_BUZZER_Init(void) large
{
    // 设置IO模式：强推挽输出
    P1M1 &= ~0x01;
    P1M0 |= 0x01;

    BZ = 0;     //初始化蜂鸣器引脚低电位
}

/*
*********************************************************************************************************
* Description : 发声
*
* Note(s)     : 延迟 20 毫秒
*********************************************************************************************************
*/
void BSP_BUZZER_Sound() large
{
    BZ = 1;
    delay_ms(20);
    BZ = 0;
}

/**********************************************RND******************************************************/
