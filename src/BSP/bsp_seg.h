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
    0x7C, // 0B 01111100, B
    0x39, // 0B 00111001, C
    0x5E, // 0B 01011110, D
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
};

//定义数码管符号：菜单
static const unsigned char code SEG_SYMBOL_MENU[] = {
    0x77, // 0, M_A
    0x76, // 1, M_H
    0x3E, // 2, M_U
    0x38, // 3, M_L
    0x70, // 4, M_R
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

extern void BSP_SEG_Show_Menu(u8 menu) large;
extern void BSP_SEG_Show_SubMenu_AH(u8 menu, byte sub_symbol, byte seg5, byte seg6) large;
extern void BSP_SEG_Show_SubMenu_L(u8 menu, byte sub_symbol, u16 i) large;
extern void BSP_SEG_Show_SubMenu_U(u8 menu, byte sub_symbol, float f) large;
extern void BSP_SEG_Show_SubMenu_R(u8 menu, byte seg4, byte seg5, byte seg6) large;


#endif
/***********************************************END*****************************************************/
