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
* Description: 操控结构体
*
* Note(s)    : none
********************************************************************************
*/

//菜单
typedef enum MENU_E
{
    M_A,    //自动挡
    M_H,    //手动挡
    M_U,    //电压
    M_L,    //日志
    M_R,    //重置
    NR_M,   //结尾
} MENU_E_TYP;
//子菜单：A
typedef enum MENU_A_E
{
    M_A_0,
    M_A_1,  //档位
    NR_M_A, //结尾
} MENU_A_E_TYP;
//子菜单：H
typedef enum MENU_H_E
{
    M_H_0,
    M_H_1,  //正脉宽
    M_H_2,  //负脉宽
    M_H_3,  //脉冲次数
    NR_M_H, //结尾
} MENU_H_E_TYP;
//子菜单：U
typedef enum MENU_U_E
{
    M_U_0,
    M_U_1,  //电压
    NR_M_U, //结尾
} MENU_U_E_TYP;
//子菜单：L
typedef enum MENU_L_E
{
    M_L_0,
    M_L_1,  //日志
    NR_M_L, //结尾
} MENU_L_E_TYP;
//子菜单：├
typedef enum MENU_R_E
{
    M_R_0,
    M_R_1,  //重置
    NR_M_R, //结尾
} MENU_R_E_TYP;

//控制
typedef struct CTR
{
    MENU_E_TYP menu;    //菜单
    u8 sub_menu;        //子菜单
    u8 auto_gear;       //自动-档位
    PD_TYP hand_pd;     //手动-脉冲驱动
    u16 weld_count;     //日志-焊接计数
    float voltage;      //电压
    u8 reset;           //重置标识
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