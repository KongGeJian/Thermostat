/*
*********************************************************************************************************
*                                        BOARD SUPPORT PACKAGE
*
* Name    : 数码管
* Version : V1.0
* Author  : 空格键
* ----------------------------
* Note(s) : 驱动IC:TM1620
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "bsp_seg.h"



/*
*********************************************************************************************************
*                                               DEFINE
*********************************************************************************************************
*/

#define STB P17     // 片选
#define CLK P54     // 时钟
#define DIO P55     // 数据，在时钟上升沿输入串行数据，从低位开始

#define GRID1_ADDR 0xC0 // 00H 第1段
#define GRID2_ADDR 0xC2 // 02H 第2段
#define GRID3_ADDR 0xC4 // 04H 第3段
#define GRID4_ADDR 0xC6 // 06H 第4段
#define GRID5_ADDR 0xC8 // 08H 第5段
#define GRID6_ADDR 0xCA // 0AH 第6段

#define CMD_DISPLAY_CONTROL 0x8A // 显示控制命令：显示开、消光脉冲14/16（最亮）10001111


byte xdata seg_code[6] = {0};


/*
*********************************************************************************************************
*                                         PRIVATE FUNCTION
*********************************************************************************************************
*/

/*写数据*/
static void _TM1620_Write(byte wr_data) compact
{
    byte i;
    for (i = 0; i < 8; i++)
    {
        CLK = 0;
        if (wr_data & 0x01)
            DIO = 1;
        else
            DIO = 0;
        delay_us(1);
        CLK = 1;
        wr_data >>= 1;
    }
}

/*发送命令字*/
static void _Write_CMD(byte cmd) compact
{
    STB = 0;
    _TM1620_Write(cmd);
    STB = 1;
}

/*写一格数据*/
static void _Weite_Grid(byte grid_addr, byte seg_code) compact
{
    STB = 0;
    _TM1620_Write(grid_addr);   // 地址命令：1100 0000 设置显示地址从0开始
    _TM1620_Write(seg_code);    // 设置段
    STB = 1;
}

/*
*********************************************************************************************************
* Description : 数码管初始化
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_SEG_Init(void) large
{
    // 设置IO模式：标准双向口
    P1M1 &= ~0x80;
    P1M0 &= ~0x80;
    P5M1 &= ~0x30;
    P5M0 &= ~0x30;

    CLK = 1; // 上拉TM1620通信时钟引脚
    STB = 1; // 上拉TM1620通信片选引脚
    DIO = 1; // 上拉TM1620通信数据引脚

    // 初始化LED控制芯片
    _Write_CMD(0x02);   // 设置显示模式：6位8段
    _Write_CMD(0x44);   // 设置数据命令：写数据到显示寄存器、固定地址、普通模式
    
    _Weite_Grid(GRID1_ADDR, SEG_SYMBOL[4]);
    _Weite_Grid(GRID2_ADDR, SEG_SYMBOL[3]);
    _Weite_Grid(GRID3_ADDR, SEG_SYMBOL[3]);
    _Weite_Grid(GRID4_ADDR, SEG_SYMBOL[3]);
    _Weite_Grid(GRID5_ADDR, SEG_SYMBOL[3]);
    _Weite_Grid(GRID6_ADDR, SEG_SYMBOL[5]);

    _Write_CMD(CMD_DISPLAY_CONTROL);    // 显示控制命令：
}

/*
*********************************************************************************************************
* Description : 显示
*
* Argument(s) : seg_code - 显示字符码
*********************************************************************************************************
*/
void BSP_SEG_Show(byte seg_code[]) large
{
    _Weite_Grid(GRID1_ADDR, seg_code[0]);
    _Weite_Grid(GRID2_ADDR, seg_code[1]);
    _Weite_Grid(GRID3_ADDR, seg_code[2]);
    _Weite_Grid(GRID4_ADDR, seg_code[3]);
    _Weite_Grid(GRID5_ADDR, seg_code[4]);
    _Weite_Grid(GRID6_ADDR, seg_code[5]);
    _Write_CMD(CMD_DISPLAY_CONTROL);
}

/*
*********************************************************************************************************
* Description : 熄灭
*********************************************************************************************************
*/
void BSP_SEG_Black() large
{
    _Weite_Grid(GRID1_ADDR, 0x00);
    _Weite_Grid(GRID2_ADDR, 0x00);
    _Weite_Grid(GRID3_ADDR, 0x00);
    _Weite_Grid(GRID4_ADDR, 0x00);
    _Weite_Grid(GRID5_ADDR, 0x00);
    _Weite_Grid(GRID6_ADDR, 0x00);
    _Write_CMD(CMD_DISPLAY_CONTROL);
}

/*
*********************************************************************************************************
* Description : 显示菜单
*
* Argument(s) : menu - 菜单编号
*********************************************************************************************************
*/
void BSP_SEG_Show_Menu(u8 menu) large
{
    if (menu >= 5)
        return;

    seg_code[0] = SEG_SYMBOL[4];
    seg_code[1] = SEG_SYMBOL_MENU[menu];
    seg_code[2] = SEG_SYMBOL[5];
    seg_code[3] = SEG_SYMBOL[0];
    seg_code[4] = SEG_SYMBOL[0];
    seg_code[5] = SEG_SYMBOL[0];
    BSP_SEG_Show(seg_code);
}

/*
*********************************************************************************************************
* Description : 显示子菜单-A H
*
* Argument(s) : menu - 一级菜单
*               sub_symbol - 二级菜单符号
*               i - 整数
*
* Note(s)     : 显示范围[0,99]
*********************************************************************************************************
*/
void BSP_SEG_Show_SubMenu_AH(u8 menu, byte sub_symbol, byte seg5, byte seg6) large
{
    seg_code[0] = SEG_SYMBOL_MENU[menu];
    seg_code[1] = sub_symbol;
    seg_code[2] = SEG_SYMBOL[2];
    seg_code[3] = SEG_SYMBOL[0];
    seg_code[4] = seg5;
    seg_code[5] = seg6;
    BSP_SEG_Show(seg_code);
}

/*
*********************************************************************************************************
* Description : 显示子菜单-L
*
* Argument(s) : menu - 一级菜单
*               sub_symbol - 二级菜单符号
*               i - 整数
*
* Note(s)     : 显示范围[0,999]
*********************************************************************************************************
*/
void BSP_SEG_Show_SubMenu_L(u8 menu, byte sub_symbol, u16 i) large
{
    i = math_imin(i, 999);

    seg_code[0] = SEG_SYMBOL_MENU[menu];
    seg_code[1] = sub_symbol;
    seg_code[2] = SEG_SYMBOL[2];
    seg_code[3] = SEG_DIGIT[i / 100];
    seg_code[4] = SEG_DIGIT[i % 100 / 10];
    seg_code[5] = SEG_DIGIT[i % 10];
    BSP_SEG_Show(seg_code);
}

/*
*********************************************************************************************************
* Description : 显示子菜单-U
*
* Argument(s) : menu - 一级菜单
*               sub_symbol - 二级菜单符号
*               f - 浮点数
*
* Note(s)     : <10时精度0.01，<100时精度0.1，>=100时精度1，显示范围[0.0,999]
*********************************************************************************************************
*/
void BSP_SEG_Show_SubMenu_U(u8 menu, byte sub_symbol, float f) large
{
    int tmp;
    if (f > 999)
        f = 999;

    seg_code[0] = SEG_SYMBOL_MENU[menu];
    seg_code[1] = sub_symbol;
    seg_code[2] = SEG_SYMBOL[2];
    if (f >= 99.95)
    {
        tmp = (int) (f + 0.05);
        seg_code[3] = SEG_DIGIT[tmp / 100];
        seg_code[4] = SEG_DIGIT[tmp % 100 / 10];
        seg_code[5] = SEG_DIGIT[tmp % 10];
    }
    else if (f >= 9.995)
    {
        tmp =  (int) ((f + 0.005) * 10);
        seg_code[3] = SEG_DIGIT[tmp / 100];
        seg_code[4] = SEG_DIGIT[tmp % 100 / 10] | SEG_SYMBOL[1];
        seg_code[5] = SEG_DIGIT[tmp % 10];
    }
    else
    {
        tmp =  (int) ((f + 0.0005) * 100);
        seg_code[3] = SEG_DIGIT[tmp / 100] | SEG_SYMBOL[1];
        seg_code[4] = SEG_DIGIT[tmp % 100 / 10];
        seg_code[5] = SEG_DIGIT[tmp % 10];
    }
    BSP_SEG_Show(seg_code);
}

/*
*********************************************************************************************************
* Description : 显示子菜单-R
*
* Argument(s) : menu - 一级菜单
*               i - 整数
*
* Note(s)     : 显示范围[0,999]
*********************************************************************************************************
*/
void BSP_SEG_Show_SubMenu_R(u8 menu, byte seg4, byte seg5, byte seg6) large
{
    seg_code[0] = SEG_SYMBOL_MENU[menu];
    seg_code[1] = SEG_SYMBOL[0];
    seg_code[2] = SEG_SYMBOL[2];
    seg_code[3] = seg4;
    seg_code[4] = seg5;
    seg_code[5] = seg6;
    BSP_SEG_Show(seg_code);
}

/**********************************************RND******************************************************/
