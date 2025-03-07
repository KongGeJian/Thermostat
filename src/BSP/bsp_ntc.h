/*
*********************************************************************************************************
*                                        BOARD SUPPORT PACKAGE
*
* Name    : NTC温度传感器
* Version : V1.0
* Author  : 空格键
*********************************************************************************************************
*/

#ifndef __BSP_NTC_H__
#define __BSP_NTC_H__

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "bsp.h"

#include <MATH.H>


/*
*********************************************************************************************************
*                                               DEFINE
*********************************************************************************************************
*/

//ADC通道枚举
typedef enum ADC_CH_E
{
    CH_0 = 0,
    CH_1,
    CH_2,
    CH_3,
    CH_4,
    CH_5,
    CH_6,
    CH_7,
    CH_8,
    CH_9,
    CH_10,
    CH_11,
    CH_12,
    CH_13,
    CH_14,
    CH_15,  
} ADC_CH_E_TYP;



/*
*********************************************************************************************************
*                                             FUNCTION
*********************************************************************************************************
*/

extern void BSP_NTC_Init(void) large;

extern s16  BSP_NTC_GetTemp() large;


#endif
/***********************************************END*****************************************************/