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

//清seg_code
static void _Clear_SegCode() large
{
    s8 i;
    for (i = sizeof(seg_code) / sizeof(seg_code[0]) - 1; i >= 0; i--)
        seg_code[i] = SEG_SYMBOL[0];
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
    _Weite_Grid(GRID4_ADDR, SEG_SYMBOL[5]);

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
    // _Weite_Grid(GRID5_ADDR, seg_code[4]);
    // _Weite_Grid(GRID6_ADDR, seg_code[5]);
    _Write_CMD(CMD_DISPLAY_CONTROL);
}

/*
*********************************************************************************************************
* Description : 熄灭
*********************************************************************************************************
*/
void BSP_SEG_Black() large
{
    _Clear_SegCode();
    BSP_SEG_Show(seg_code);
}

/*
*********************************************************************************************************
* Description : 显示xx
*
* Note(s)     : none.
*********************************************************************************************************
*/
//自定义
void BSP_SEG_Show_Custom(byte seg1, byte seg2, byte seg3, byte seg4) large
{
    seg_code[0] = seg1;
    seg_code[1] = seg2;
    seg_code[2] = seg3;
    seg_code[3] = seg4;
    BSP_SEG_Show(seg_code);
}

//设置模式菜单
void BSP_SEG_Show_SetMenu(u8 n) large
{
    _Clear_SegCode();
    seg_code[0] = SEG_SYMBOL[16];
    seg_code[1] = SEG_DIGIT[n];
    BSP_SEG_Show(seg_code);
}

//传感器开路
void BSP_SEG_Show_SensorOpen() large
{
    _Clear_SegCode();
    seg_code[1] = SEG_SYMBOL[14];
    seg_code[2] = SEG_SYMBOL[14];
    BSP_SEG_Show(seg_code);
}

//越界，超出范围
void BSP_SEG_Show_OutOfRange() large
{
    _Clear_SegCode();
    seg_code[1] = SEG_SYMBOL[13];
    seg_code[2] = SEG_SYMBOL[13];
    BSP_SEG_Show(seg_code);
}

//告警
void BSP_SEG_Show_Alarm() large
{
    _Clear_SegCode();
    seg_code[1] = SEG_SYMBOL[2];
    seg_code[2] = SEG_SYMBOL[2];
    BSP_SEG_Show(seg_code);
}

//显示clr
void BSP_SEG_Show_Reset() large
{
    _Clear_SegCode();
    seg_code[1] = 0x58;
    seg_code[2] = 0x18;
    seg_code[3] = 0x50;
    BSP_SEG_Show(seg_code);
}

//显示整型值
void BSP_SEG_Show_IntVal(s16 val) large
{
    u8 sign;
    sign = val >= 0 ? 0 : 1; // 符号：0正数，1负数
    val = abs(val);

    _Clear_SegCode();

    if (val >= 100)
        seg_code[1] = SEG_DIGIT[val / 100];
    if (val >= 10)
        seg_code[2] = SEG_DIGIT[val % 100 / 10];
    seg_code[3] = SEG_DIGIT[val % 10];

    if (sign)
    {
        if (val >= 100)
            seg_code[0] = SEG_SYMBOL[2];
        else if (val >= 10)
            seg_code[1] = SEG_SYMBOL[2];
        else
            seg_code[2] = SEG_SYMBOL[2];
    }   
    BSP_SEG_Show(seg_code);
}

//显示小数值。范围[-400,3000], 精度0.1
void BSP_SEG_Show_DecVal(s16 val) large
{
    u8 sign, decimal;
    u16 integer;

    sign = val >= 0 ? 0 : 1; // 符号：0正数，1负数
    val = abs(val);
    integer = val / 10; // 整数部分
    decimal = val % 10; // 小数部分

    _Clear_SegCode();

    if (integer >= 100)
    {
        seg_code[0] = SEG_DIGIT[integer / 100];
        seg_code[1] = SEG_DIGIT[integer % 100 / 10];
        seg_code[2] = SEG_DIGIT[integer % 10] | SEG_SYMBOL[1];
        seg_code[3] = SEG_DIGIT[decimal];
    }
    else if (integer >= 10)
    {
        if (sign)
            seg_code[0] = SEG_SYMBOL[2];
        seg_code[1] = SEG_DIGIT[integer / 10];
        seg_code[2] = SEG_DIGIT[integer % 10] | SEG_SYMBOL[1];
        seg_code[3] = SEG_DIGIT[decimal];
    }
    else
    {
        if (sign)
            seg_code[1] = SEG_SYMBOL[2];
        seg_code[2] = SEG_DIGIT[integer] | SEG_SYMBOL[1];
        seg_code[3] = SEG_DIGIT[decimal];
    }
    BSP_SEG_Show(seg_code);
}

/**********************************************RND******************************************************/
