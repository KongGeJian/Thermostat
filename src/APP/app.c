/*
*******************************************************************************
* Name    : app.c
* Version : V1.0
* Author  : 空格键
* Date    : 2024-01-06
* ----------------------------
* Note(s) : 整型类型，精度0.1，表示含义：÷10才是真实温度值
*******************************************************************************
*/

/*
*******************************************************************************
*                                INCLUDE FILE
*******************************************************************************
*/

#include "app.h"

/*
*******************************************************************************
*                                 全局参数
*******************************************************************************
*/

MENU_CTR_TYP  idata app_menu_ctr;   //应用菜单控制
CFG_PARAM_TYP idata app_cfg_param;  //应用配置参数
CFG_PARAM_TYP xdata eep_param;      //eep保存参数
s16 xdata currTemp;                 //当前温度，精度0.1。INT16_MIN表示NTC未接入
boolean xdata work;                 //继电器吸合工作

#define AUTO_RETURN_TIME 60   //字段返回时间，单位：秒
u8 xdata auto_return_countdown = AUTO_RETURN_TIME;

volatile bit data flag_refresh_seg = true;  //刷新数码管标识，上电默认刷新一次
volatile bit data flag_save_params = false; //保存参数标识
volatile bit data flag_sleep = false;       //保存参数标识


/*
*******************************************************************************
*                                 函数声明
*******************************************************************************
*/

//默认值
void defaultParam() compact
{
    app_cfg_param.targetTemp = M_RUN_SETTING_DEFAULT;
    app_cfg_param.setP0 = M_SET_P0_C;
    app_cfg_param.setP1 = M_SET_P1_DEFAULT;
    app_cfg_param.setP2 = M_SET_P2_DEFAULT;
    app_cfg_param.setP3 = M_SET_P3_DEFAULT;
    app_cfg_param.setP4 = M_SET_P4_DEFAULT;
    app_cfg_param.setP5 = M_SET_P5_DEFAULT;
    app_cfg_param.setP6 = M_SET_P6_OFF;
    app_cfg_param.setP7 = 0;
    app_cfg_param.setP8 = 0;
    app_cfg_param.setP9 = false;
}

//初始化参数
void initParam() compact
{
    // u16 len;

    app_menu_ctr.menu = M_RUN;
    app_menu_ctr.runMenu = M_RUN_MEASURE;
    app_menu_ctr.setMenu = M_SET_P0;
    app_menu_ctr.setMenuVal = false;
    defaultVariable();

    currTemp = INT16_MIN;
    work = false;

    //从EEP初始化配置
    // len = sizeof(eep_param);
    // if (BSP_EEPROM_Read_Params(&eep_param, len))
    if (BSP_EEPROM_Read_Params(&eep_param, sizeof(eep_param)))
    {
        app_cfg_param.targetTemp = eep_param.targetTemp;
        app_cfg_param.setP0 = eep_param.setP0;
        app_cfg_param.setP1 = eep_param.setP1; 
        app_cfg_param.setP2 = eep_param.setP2; 
        app_cfg_param.setP3 = eep_param.setP3; 
        app_cfg_param.setP4 = eep_param.setP4; 
        app_cfg_param.setP5 = eep_param.setP5; 
        app_cfg_param.setP6 = eep_param.setP6; 
        app_cfg_param.setP7 = eep_param.setP7; 
        app_cfg_param.setP8 = eep_param.setP8;
    }
}

//加减计算
boolean addOrSub(u8 *p, u8 max, u8 min)
{
    if ((BSP_KBD_IsShort(K_ADD) || BSP_KBD_IsRepeat(K_ADD)) && BSP_KBD_IsNone(K_SUB)) //+
    {
        *p = math_imin(*p + 1, max);
        return true;
    }
    else if ((BSP_KBD_IsShort(K_SUB) || BSP_KBD_IsRepeat(K_SUB)) && BSP_KBD_IsNone(K_ADD)) //-
    {
        *p = math_imax(*p - 1, min);
        return true;
    }
    return false;
}

//按键动作响应
boolean keyAction() large
{
    bit action = false;

    if (BSP_KBD_IsShort(K_SET)) //设置-短按
    {
        if (ctr.sub_menu != 0) //退出子菜单
        {
            ctr.sub_menu = 0;
            action = true;
        }
    }
    else if (BSP_KBD_IsShort(K_CFM)) //确认-短按
    {
        ctr.sub_menu = (ctr.sub_menu + 1) % getSubMenuNr(ctr.menu); //子菜单切换
        ctr.sub_menu += (ctr.sub_menu == 0) ? 1 : 0;
        action = true;
    }
    else if (BSP_KBD_IsNone(K_SET) || BSP_KBD_IsNone(K_CFM)) //加减-的按键动作
    {
        if (ctr.sub_menu == 0) //主菜单切换
        {
            action = addOrSub(&ctr.menu, M_R, M_A);
        }
        else //子菜单设置
        {
            switch (ctr.menu)
            {
            case M_A:
                if (ctr.sub_menu == M_A_1)
                    action = addOrSub(&ctr.auto_gear, PD_GEAR_MAX, PD_GEAR_MIN);
                break;
            case M_H:
                if (ctr.sub_menu == M_H_1)
                    action = addOrSub(&ctr.hand_pd.pwidth, PD_WIDTH_MAX, PD_P_WIDTH_MIN);
                else if (ctr.sub_menu == M_H_2)
                    action = addOrSub(&ctr.hand_pd.nwidth, PD_WIDTH_MAX, PD_N_WIDTH_MIN);
                else if (ctr.sub_menu == M_H_3)
                    action = addOrSub(&ctr.hand_pd.num, PD_NUM_MAX, PD_NUM_MIN);
                break;
            case M_U:
                if (ctr.sub_menu == M_U_1)
                {
                    ctr.voltage = BSP_POWER_GetVoltage() / 1000.0;
                    action = true;
                }
                break;
            case M_R:
                if (ctr.sub_menu == M_R_1)
                {
                    if (ctr.reset == 0)
                    {
                        if (BSP_KBD_GetPressTime(K_SUB) >= 500) //长按5s
                        {
                            ctr.reset = 1;
                            defaultVariable();
                            action = true;
                        }
                    }
                    else
                    {
                        if (BSP_KBD_IsRelease(K_SUB))
                            ctr.reset = 0;
                    }
                }
                break;
            default:
                break;
            }
        }
    }

    return action;
}

//数码管刷新显示
void segRefreshShow() large
{
    u8 cnt;

    if (!flag_refresh_seg)
        return;

    //显示子菜单
    switch (app_menu_ctr.menu)
    {
    case M_RUN:  //运行模式
        switch (app_menu_ctr.runMenu)
        {
        case M_RUN_MEASURE: // 测量值
            if (currTemp == INT16_MIN)
                BSP_SEG_Show_SensorOpen();
            else if (currTemp < M_SET_P3_MIN || currTemp > M_SET_P2_MAX)
                BSP_SEG_Show_OutOfRange();
            else
                BSP_SEG_Show_Temp(currTemp);
            break;
        case M_RUN_SETTING: // 设定值
            BSP_SEG_Show_Temp(app_cfg_param.targetTemp * 10);
            break;
        }
        break;
    case M_SET: // 设置模式
        if (!app_menu_ctr.setMenuVal) //显示菜单Px
        {
            BSP_SEG_Show_SetMenu(app_menu_ctr.setMenu);
        }
        else // 显示数值
        {
            switch (app_menu_ctr.setMenu)
            {
            case M_SET_P0:
                if (app_cfg_param.setP0 == M_SET_P0_C)
                    BSP_SEG_Show_Custom(SEG_SYMBOL[0], SEG_SYMBOL[0], SEG_SYMBOL[11]);
                else
                    BSP_SEG_Show_Custom(SEG_SYMBOL[0], SEG_SYMBOL[0], SEG_SYMBOL[13]);
                break;
            case M_SET_P1:
                BSP_SEG_Show_Temp(app_cfg_param.setP1);
                break;
            case M_SET_P2:
                BSP_SEG_Show_Temp(app_cfg_param.setP2 * 10);
                break;
            case M_SET_P3:
                BSP_SEG_Show_Temp(app_cfg_param.setP3 * 10);
                break;
            case M_SET_P4:
                BSP_SEG_Show_IntVal(app_cfg_param.setP4);
                break;
            case M_SET_P5:
                BSP_SEG_Show_Temp(app_cfg_param.setP5);
                break;
            case M_SET_P6:
                if (app_cfg_param.setP6 == M_SET_P6_OFF)
                    BSP_SEG_Show_Custom(SEG_SYMBOL[15], SEG_SYMBOL[12], SEG_SYMBOL[12]);
                else
                    BSP_SEG_Show_IntVal(app_cfg_param.setP6);
                break;
            case M_SET_P7:
                BSP_SEG_Show_IntVal(0);
                break;
            case M_SET_P8:
                BSP_SEG_Show_IntVal(0);
                break;
            case M_SET_P9:
                BSP_SEG_Show_Clear(); // clr
                if (app_cfg_param.setP9)
                {
                    cnt = 3;
                    while (cnt--)
                    {
                        BSP_SEG_Black();
                        delay_ms(200);
                        BSP_SEG_Show_Clear();
                        delay_ms(500);
                    }
                    BSP_BUZZER_Sound();
                }
                break;
            }
        }
    }
    flag_refresh_seg = false;
}

//按键处理
void keyProcess() large
{
    boolean tmp; 

    if (!BSP_KBD_Scan()) 
        return;
    //BSP_UART_Println(UART1, "key_code: %02bX,%02bX,%02bX,%02bX", BSP_KBD_KeyCode(K_SET), BSP_KBD_KeyCode(K_ADD), BSP_KBD_KeyCode(K_SUB), BSP_KBD_KeyCode(K_CFM));

    tmp = keyAction();
    flag_refresh_seg |= tmp;
    flag_save_params |= tmp;
    auto_return_countdown = AUTO_RETURN_TIME;   //重新倒计时
}

//继电器工作
void relayWork() large
{
    if (app_menu_ctr.menu != M_RUN || app_menu_ctr.runMenu != M_RUN_MEASURE) // 不在运行模式+测量，不工作
        return;
    if (currTemp < (M_SET_P3_MIN * 10) || currTemp > (M_SET_P2_MAX * 10)) // 温度不在有效范围，不工作
        return;

    if (app_cfg_param.setP0 == M_SET_P0_C) // 制冷模式
    {
        if (work == false && currTemp >= app_cfg_param.targetTemp)
            work = true;
        else if (work == true && currTemp <= (app_cfg_param.targetTemp - app_cfg_param.setP1))
            work = false;
    }
    else if (app_cfg_param.setP0 == M_SET_P0_H) // 制热模式
    {
        if (work == false && currTemp <= app_cfg_param.targetTemp)
            work = true;
        else if (work == true && currTemp >= (app_cfg_param.targetTemp + app_cfg_param.setP1))
            work = false;
    }

    if (work)
        BSP_RELAY_Start();
    else
        BSP_RELAY_Stop();
}

//保存参数
void saveParams() large
{
    if (!flag_save_params)
        return;
    if (!BSP_KBD_IsAllNone()) //有按键动作，延迟保存
        return;

    eep_param.targetTemp = app_cfg_param.targetTemp;
    eep_param.setP0 = app_cfg_param.setP0;
    eep_param.setP1 = app_cfg_param.setP1;
    eep_param.setP2 = app_cfg_param.setP2;
    eep_param.setP3 = app_cfg_param.setP3;
    eep_param.setP4 = app_cfg_param.setP4;
    eep_param.setP5 = app_cfg_param.setP5;
    eep_param.setP6 = app_cfg_param.setP6;
    eep_param.setP7 = app_cfg_param.setP7;
    eep_param.setP8 = app_cfg_param.setP8;
    eep_param.setP9 = app_cfg_param.setP9;
    BSP_EEPROM_Write_Params(&eep_param, sizeof(eep_param));

    flag_save_params = false;
}

//倒计时
void countdown() large
{
    if (auto_return_countdown > 0)
        auto_return_countdown--;
}

/*
*******************************************************************************
*                                   函数
*******************************************************************************
*/
void setup()
{
    BSP_Init();
    initParam();

    delay_ms(500);
    BSP_BUZZER_Sound(); // 自检发声
    BSP_UART_Println(UART1, "setup complete!");
}

void createTask()
{
    OS_CreateTask(0, 100, segRefreshShow); // 数码管刷新，立即运行，后每100ms运行一次
    OS_CreateTask(11, 10, keyProcess);     // 按键处理，每10ms扫描一次
    OS_CreateTask(12, 200, relayWork);     // 继电器工作
    OS_CreateTask(13, 1000, saveParams);   // 保存参数
    OS_CreateTask(16, 1000, countdown);    // 倒计时
}
