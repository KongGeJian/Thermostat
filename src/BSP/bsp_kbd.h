/*
*********************************************************************************************************
*                                        BOARD SUPPORT PACKAGE
*
* Name    : 按键
* Version : V1.0
* Author  : 空格键
*********************************************************************************************************
*/

#ifndef __BSP_KBD_H__
#define __BSP_KBD_H__

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "bsp.h"


/*
*********************************************************************************************************
*                                               DEFINE
*********************************************************************************************************
*/

//按键
typedef enum
{
    K_SET,  //设置
    K_ADD,  //加
    K_SUB,  //减
    K_CFM,  //确认
    NR_K,   //结尾
} KEY_E_TYP;

/**按键动作标识**/
#define K_NONE    0    //无按键按下
#define K_MAKE    0x80 //通码，表示一直按着
#define K_PRESS   0x20 //刚按下
#define K_RELEASE 0x10 //刚释放
#define K_REPEAT  0x08 //重复
#define K_LONG    0x02 //长按
#define K_SHORT   0x01 //短按



/*
*********************************************************************************************************
*                                             FUNCTION
*********************************************************************************************************
*/

extern void    BSP_KBD_Init(void) large;

extern boolean BSP_KBD_Scan() large;
extern u8      BSP_KBD_KeyCode(KEY_E_TYP kx) large;

extern boolean BSP_KBD_IsNone(KEY_E_TYP kx) large;
extern boolean BSP_KBD_IsPress(KEY_E_TYP kx) large;
extern boolean BSP_KBD_IsRelease(KEY_E_TYP kx) large;
extern boolean BSP_KBD_IsRepeat(KEY_E_TYP kx) large;
extern boolean BSP_KBD_IsLong(KEY_E_TYP kx) large;
extern boolean BSP_KBD_IsShort(KEY_E_TYP kx) large;
extern boolean BSP_KBD_IsAllNone() large;

extern s32     BSP_KBD_GetPressTime(KEY_E_TYP kx) large;


#endif
/***********************************************END*****************************************************/
