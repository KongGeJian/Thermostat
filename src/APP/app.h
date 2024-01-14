/*
*******************************************************************************
* Name    : app.h
* Version : V1.0
* Author  : 空格键
* Date    : 2022-04-23
*******************************************************************************
*/

#ifndef _APP_H
#define _APP_H

#include "os.h"
#include "bsp.h"



/*
********************************************************************************
* Description: 菜单定义
********************************************************************************
*/

//菜单: 模式
typedef enum MENU_E
{
    M_RUN,  //运行模式
    M_SET,  //设置模式
    NR_M,   //结尾
} MENU_E_TYP;

//子菜单: M_RUN
typedef enum MENU_RUN_E
{
    M_RUN_MEASURE,  //测量
    M_RUN_SETTING,  //设定 
    NR_M_RUN,
} MENU_RUN_E_TYP;

//子菜单: M_SET_P1
#define M_RUN_SETTING_DEFAULT  28

//子菜单: M_SET
typedef enum MENU_SET_E
{
    M_SET_P0,    //P0-P9
    M_SET_P1,
    M_SET_P2,
    M_SET_P3,
    M_SET_P4,
    M_SET_P5,
    M_SET_P6,
    M_SET_P7,
    M_SET_P8,
    M_SET_P9,
    NR_M_SET,
} MENU_SET_E_TYP;

//子菜单: M_SET_P0
typedef enum MENU_SET_P0_E
{
    M_SET_P0_C, //制冷
    M_SET_P0_H, //制热
    NR_M_SET_P0,
} MENU_SET_P0_E_TYP;

//子菜单: M_SET_P1
#define M_SET_P1_DEFAULT 1
#define M_SET_P1_MIN  1
#define M_SET_P1_MAX  150

//子菜单: M_SET_P2
#define M_SET_P2_DEFAULT  250
#define M_SET_P2_MAX  300

//子菜单: M_SET_P3
#define M_SET_P3_DEFAULT  -20
#define M_SET_P3_MIN  -40

//子菜单: M_SET_P4
#define M_SET_P4_DEFAULT  0
#define M_SET_P4_MIN  0
#define M_SET_P4_MAX  30

//子菜单: M_SET_P5
#define M_SET_P5_DEFAULT  0
#define M_SET_P5_MIN  -99
#define M_SET_P5_MAX  99

//子菜单: M_SET_P6
#define M_SET_P6_OFF  -1
#define M_SET_P6_MIN  0
#define M_SET_P6_MAX  110

//控制
typedef struct CTR
{
    MENU_E_TYP menu;         // 菜单
    MENU_RUN_E_TYP runMenu;  // 菜单
    MENU_SET_E_TYP setMenu;  // 菜单
    s16 targetTemp;          // 设定目标温度
    MENU_SET_P0_E_TYP setP0; // P0
    u8 setP1;                // P1，精度0.1，真实值 ÷10
    s16 setP2;               // P2
    s8 setP3;                // P3
    u8 setP4;                // P4
    s8 setP5;                // P5, 精度0.1，真实值 ÷10
    s8 setP6;                // P6
    u8 setP7;                // P7，预留
    u8 setP8;                // P8，预留
    boolean setP9;           // 重置标识
} CTR_TYP;


/*
*******************************************************************************
*                                 函数声明
*******************************************************************************
*/

void setup();
void createTask();

/*main*/
void main()
{
    OS_Init();

    setup();
    createTask();

    OS_Start();
    while (1)
    {
        OS_TaskProcess();
    }
}

#endif