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

volatile bit data flag_seg_refresh = true;  //数码管刷新标识，上电默认刷新一次
volatile bit data flag_save_params = false; //保存参数标识


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
}

//初始化参数
void initParam() compact
{
    // u16 len;

    app_menu_ctr.menu = M_RUN;
    app_menu_ctr.runMenu = M_RUN_MEASURE;
    app_menu_ctr.setMenu = M_SET_P0;
    app_menu_ctr.setMenuVal = false;

    defaultParam();
    app_cfg_param.setP9 = false;

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
        //eep中P9不设置
    }
}

//判断加减操作：-1减，1加，0无操作
s8 decideAddSub() large
{
    if ((BSP_KBD_IsShort(K_ADD) || BSP_KBD_IsRepeat(K_ADD)) && BSP_KBD_IsNone(K_SUB)) //+
        return 1;
    else if ((BSP_KBD_IsShort(K_SUB) || BSP_KBD_IsRepeat(K_SUB)) && BSP_KBD_IsNone(K_ADD)) //-
        return -1;
    return 0;
}

//数码管刷新显示
void segRefreshShow() large
{
    u8 cnt;

    if (!flag_seg_refresh)
        return;

    switch (app_menu_ctr.menu)
    {
    case M_RUN: // 运行模式
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
            BSP_SEG_Show_Temp(app_cfg_param.targetTemp);
            break;
        }
        break;
    case M_SET: // 设置模式
        if (!app_menu_ctr.setMenuVal) // 显示菜单Px
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
                BSP_SEG_Show_IntVal(app_cfg_param.setP2 / 10);
                break;
            case M_SET_P3:
                BSP_SEG_Show_IntVal(app_cfg_param.setP3 / 10);
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
                    BSP_SEG_Show_IntVal(app_cfg_param.setP6 / 10);
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
        break;
    }
    flag_seg_refresh = false;
}

//按键处理
void keyProcess() large
{
    bit action = false;
    s8 addSub;

    if (!BSP_KBD_Scan())
        return;
    //BSP_UART_Println(UART1, "key_code: %02bX,%02bX,%02bX,%02bX", BSP_KBD_KeyCode(K_SET), BSP_KBD_KeyCode(K_ADD), BSP_KBD_KeyCode(K_SUB), BSP_KBD_KeyCode(K_CFM));

    addSub = decideAddSub();
    switch (app_menu_ctr.menu)
    {
    case M_RUN: // 运行模式
        if (BSP_KBD_IsLong(K_SET) && app_menu_ctr.runMenu == M_RUN_MEASURE) // 设置-长按
        {
            app_menu_ctr.menu = M_SET;
        }
        else if (BSP_KBD_IsShort(K_SET)) //设置-短按
        {
            app_menu_ctr.runMenu = (app_menu_ctr.runMenu + 1) % NR_M_RUN; //切换
        }
        else if (addSub != 0 && app_menu_ctr.runMenu == M_RUN_SETTING) // 加减操作
        {
            if (addSub == 1) //+
                app_cfg_param.targetTemp = math_imin(app_cfg_param.targetTemp + 10, app_cfg_param.setP2);
            else //-
                app_cfg_param.targetTemp = math_imax(app_cfg_param.targetTemp - 10, app_cfg_param.setP3);
        }
        break;
    case M_SET: // 设置模式
        if (BSP_KBD_IsLong(K_SET)) // 设置-长按
        {
            app_menu_ctr.menu = M_RUN;
            flag_save_params = true;
        }
        else if (BSP_KBD_IsShort(K_SET)) //设置-短按
        {
            app_menu_ctr.setMenuVal = !app_menu_ctr.setMenuVal;
        }
        else if (addSub != 0) //加减操作
        {
            if (!app_menu_ctr.setMenuVal) //显示菜单
            {
                if (addSub == 1) //+
                    app_menu_ctr.setMenu = (app_menu_ctr.setMenu + 1) % NR_M_SET; // 切换true;
                else //-
                    app_menu_ctr.setMenu = app_menu_ctr.setMenu == M_SET_P0 ? M_SET_P9 : app_menu_ctr.setMenu - 1;
            }
            else //显示值
            {
                switch (app_menu_ctr.setMenu)
                {
                case M_SET_P0:
                    app_cfg_param.setP0 = app_cfg_param.setP0 == M_SET_P0_C ? M_SET_P0_H : M_SET_P0_C;
                    break;
                case M_SET_P1:
                    if (addSub == 1) //+
                        app_cfg_param.setP1 = math_imin(app_cfg_param.setP1 + 1, M_SET_P1_MAX);
                    else //-
                        app_cfg_param.setP1 = math_imax(app_cfg_param.setP1 - 1, M_SET_P1_MIN);
                    break;
                case M_SET_P2:
                    if (addSub == 1) //+
                        app_cfg_param.setP2 = math_imin(app_cfg_param.setP2 + 10, M_SET_P2_MAX);
                    else //-
                        app_cfg_param.setP2 = math_imax(app_cfg_param.setP2 - 10, app_cfg_param.setP3);
                    break;
                case M_SET_P3:
                    if (addSub == 1) //+
                        app_cfg_param.setP3 = math_imin(app_cfg_param.setP3 + 10, app_cfg_param.setP2);
                    else //-
                        app_cfg_param.setP3 = math_imax(app_cfg_param.setP3 - 10, M_SET_P3_MIN);
                    break;
                case M_SET_P4:
                    if (addSub == 1) //+
                        app_cfg_param.setP4 = math_imin(app_cfg_param.setP4 + 1, M_SET_P4_MAX);
                    else //-
                        app_cfg_param.setP4 = math_imax(app_cfg_param.setP4 - 1, M_SET_P4_MIN);
                    break;
                case M_SET_P5:
                    if (addSub == 1) //+
                        app_cfg_param.setP5 = math_imin(app_cfg_param.setP5 + 1, M_SET_P5_MAX);
                    else //-
                        app_cfg_param.setP5 = math_imax(app_cfg_param.setP5 - 1, M_SET_P5_MIN);
                    break;
                case M_SET_P6:
                    if (addSub == 1) //+
                        if (app_cfg_param.setP6 == M_SET_P6_OFF)
                            app_cfg_param.setP6 = M_SET_P6_MIN;
                        else
                            app_cfg_param.setP6 = math_imin(app_cfg_param.setP6 + 10, M_SET_P6_MAX);
                    else //-
                        if (app_cfg_param.setP6 == M_SET_P6_MIN)
                            app_cfg_param.setP6 = M_SET_P6_OFF;
                        else
                            app_cfg_param.setP6 = math_imax(app_cfg_param.setP6 - 10, M_SET_P6_MIN);
                    break;
                case M_SET_P7:
                    // no code
                    break;
                case M_SET_P8:
                    // no code
                    break;
                case M_SET_P9:
                    if (app_cfg_param.setP9 == false)
                    {
                        if (BSP_KBD_GetPressTime(K_ADD) >= 500 && BSP_KBD_GetPressTime(K_SUB) >= 500) // 加减同时按下超过6s
                        {
                            app_cfg_param.setP9 = true;
                            defaultParam();
                        }
                    }
                    else
                    {
                        if (BSP_KBD_IsRelease(K_ADD) && BSP_KBD_IsRelease(K_SUB))
                            app_cfg_param.setP9 = 0;
                    }
                    break;
                }
            }
        }
        break;
    }

    flag_seg_refresh = true;
    auto_return_countdown = AUTO_RETURN_TIME;   //重新倒计时
}

//获取温度值
void getTemperature() large
{
    u16 sampleTemp = BSP_NTC_GetTemp(); // 温度采样值
    BSP_UART_Println(UART1, "sampleTemp: %hd", sampleTemp);

    if (sampleTemp == INT16_MIN) // 传感器未接入
        currTemp = sampleTemp;
    else
        currTemp = sampleTemp / 10 + app_cfg_param.setP5; // 采样值转显示值，含校准值

    if (app_cfg_param.setP6 != M_SET_P6_OFF && currTemp >= app_cfg_param.setP6) // 高温报警
        BSP_BUZZER_Sound();

    flag_seg_refresh = true;
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
    eep_param.setP9 = false;
    BSP_EEPROM_Write_Params(&eep_param, sizeof(eep_param));

    flag_save_params = false;
}

//倒计时
void countdown() large
{
    if (auto_return_countdown == 0)
    {
        if (!(app_menu_ctr.menu == M_RUN && app_menu_ctr.runMenu == M_RUN_MEASURE)) // 不是运行模式中的测量
        {
            app_menu_ctr.menu = M_RUN;
            app_menu_ctr.runMenu = M_RUN_MEASURE;
            flag_seg_refresh = true;
            flag_save_params = true; // 自动保存
        }
    }
    else
    {
        auto_return_countdown--;
    }
}

/*
*******************************************************************************
*                                   主函数
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
    OS_CreateTask(14, 500, getTemperature);// 获取温度值
    OS_CreateTask(15, 200, relayWork);     // 继电器工作
    OS_CreateTask(18, 1000, saveParams);   // 保存参数
    OS_CreateTask(19, 1000, countdown);    // 倒计时
}
