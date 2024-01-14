/*
*********************************************************************************************************
*                                        BOARD SUPPORT PACKAGE
*
* Name    : bsp.c
* Version : V1.0
* Author  : 空格键
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "bsp.h"



/*
*********************************************************************************************************
* Description : BSP初始化方法。
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_Init(void) large
{
    P_SW2 |= 0x80;      //使能访问 扩展RAM区特殊功能寄存器（XFR）
    AUXR |= 0x80;       //定时器T0设置为1T（为兼容传统8051，默认12T）
    EA = 1;             //使能总中断。

    //初始化 LIB
    delay_init();
    mem_init();

    //初始化 BSP
    BSP_UART_Init();
    BSP_SEG_Init();
    BSP_KBD_Init();
    BSP_BUZZER_Init();
    BSP_EEPROM_Init();
    BSP_NTC_Init();
    BSP_RELAY_INIT();
}

/**********************************************END******************************************************/
