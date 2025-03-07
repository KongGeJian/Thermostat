
/*
*********************************************************************************************************
*                                        BOARD SUPPORT PACKAGE
*
* Name    : bsp.h
* Version : V1.0
* Author  : 空格键
* ----------------------------
* Note(s) : (1) 这里是BSP预处理总头文件
*********************************************************************************************************
*/

#ifndef __BSP_H__
#define __BSP_H__

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include "cfg.h"

#include "lib_stdint.h"
#include "lib_delay.h"
#include "lib_mem.h"
// #include "lib_str.h"
#include "lib_math.h"



/*
*********************************************************************************************************
*                                        INCLUDE FILES - Sub module
*********************************************************************************************************
*/

#include "bsp_uart.h"
#include "bsp_seg.h"
#include "bsp_kbd.h"
#include "bsp_buzzer.h"
#include "bsp_eeprom.h"
#include "bsp_ntc.h"
#include "bsp_relay.h"



/*
*********************************************************************************************************
*                                              FUNCTION
*********************************************************************************************************
*/

extern void  BSP_Init(void) large;



#endif
/***********************************************END*****************************************************/
