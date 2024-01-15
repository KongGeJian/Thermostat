/*
*********************************************************************************************************
*                                        BOARD SUPPORT PACKAGE
*
* Name    : 数码管
* Version : V1.0
* Author  : 空格键
*********************************************************************************************************
*/

#ifndef __BSP_SEG_H__
#define __BSP_SEG_H__

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

//定义数码管0~9数字码表
static const unsigned char code SEG_DIGIT[16] = {
    0x3F, // 0B 00111111, 0
    0x06, // 0B 00000110, 1
    0x5B, // 0B 01011011, 2
    0x4F, // 0B 01001111, 3
    0x66, // 0B 01100110, 4
    0x6D, // 0B 01101101, 5
    0x7D, // 0B 01111101, 6
    0x07, // 0B 00000111, 7
    0x7F, // 0B 01111111, 8
    0x6F, // 0B 01101111, 9
    0x77, // 0B 01110111, A
    0x7C, // 0B 01111100, B b
    0x39, // 0B 00111001, C
    0x5E, // 0B 01011110, D d
    0x79, // 0B 01111001, E
    0x71, // 0B 01110001, F
};

//定义数码管符号
static const unsigned char code SEG_SYMBOL[] = {
    0x00, // 0, BLACK  熄灭
    0x80, // 1, DP
    0x40, // 2, -
    0x09, // 3, 上下2横
    0x39, // 4, [
    0x0F, // 5, ]
    0x23, // 6, 冖
    0x1C, // 7, 凵
    0x64, // 8, ㄣ
    0x08, // 9, _
    0x00, // 10, 预留
    0x39, // 11, C
    0x71, // 12, F
    0x76, // 13, H
    0x38, // 14, L
    0x5C, // 15, o
    0x73, // 16, P
    0x70, // 17, r
};


extern byte xdata seg_code[];


/*
*********************************************************************************************************
*                                             FUNCTION
*********************************************************************************************************
*/

extern void BSP_SEG_Init(void) large;

extern void BSP_SEG_Show(byte seg_code[]) large;
extern void BSP_SEG_Black() large;

extern void BSP_SEG_Show_Custom(byte seg1, byte seg2, byte seg3) large;
extern void BSP_SEG_Show_SetMenu(u8 n) large;
extern void BSP_SEG_Show_SensorOpen() large;
extern void BSP_SEG_Show_OutOfRange() large;
extern void BSP_SEG_Show_Alarm() large;
extern void BSP_SEG_Show_Clear() large;
extern void BSP_SEG_Show_IntVal(u16 val) large;
extern void BSP_SEG_Show_Temp(s16 temp) large;


#endif
/***********************************************END*****************************************************/
