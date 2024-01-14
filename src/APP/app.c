/*
*******************************************************************************
* Name    : app.c
* Version : V1.0
* Author  : 空格键
* Date    : 2022-04-23
* ----------------------------
* Note(s) : -
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

CTR_TYP idata ctr; //控制

CTR_TYP xdata eep_param; //eep保存参数

//睡眠计时
#define SLEEP_TIME 60   //60s
u8 xdata sleep_countdown = SLEEP_TIME;

volatile bit data flag_refresh_seg = true;  //刷新数码管标识，上电默认刷新一次
volatile bit data flag_save_params = false; //保存参数标识
volatile bit data flag_sleep = false;       //保存参数标识


/*
*******************************************************************************
*                                 函数声明
*******************************************************************************
*/

//默认值
void defaultVariable() compact
{
    ctr.targetTemp = M_RUN_SETTING_DEFAULT;
    ctr.setP0 = M_SET_P0_C;
    ctr.setP1 = M_SET_P1_DEFAULT;
    ctr.setP2 = M_SET_P2_DEFAULT;
    ctr.setP3 = M_SET_P3_DEFAULT;
    ctr.setP4 = M_SET_P4_DEFAULT;
    ctr.setP5 = M_SET_P5_DEFAULT;
    ctr.setP6 = M_SET_P6_OFF;
    ctr.setP7 = 0;
    ctr.setP8 = 0;
    ctr.setP9 = false;
}

//初始化变量
void initVariable() compact
{
    u16 len;

    ctr.menu = M_RUN;
    ctr.runMenu = M_RUN_MEASURE;
    ctr.setMenu = M_SET_P0;
    defaultVariable();

    //从EEP初始化配置
    len = sizeof(eep_param);
    if (BSP_EEPROM_Read_Params(&eep_param, len))
    {
        ctr.targetTemp = eep_param.targetTemp;
        ctr.setP0 = eep_param.setP0;
        ctr.setP1 = eep_param.setP1; 
        ctr.setP2 = eep_param.setP2; 
        ctr.setP3 = eep_param.setP3; 
        ctr.setP4 = eep_param.setP4; 
        ctr.setP5 = eep_param.setP5; 
        ctr.setP6 = eep_param.setP6; 
        ctr.setP7 = eep_param.setP7; 
        ctr.setP8 = eep_param.setP8;
    }
}

//获取子菜单NR
u8 getSubMenuNr(u8 menu)
{
    switch (menu)
    {
    case M_A:
        return NR_M_A;
    case M_H:
        return NR_M_H;
    case M_U:
        return NR_M_U;
    case M_L:
        return NR_M_L;
    case M_R:
        return NR_M_R;
    default:
        return 1;
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

//刷新数码管显示
void refreshSegShow() large
{
    u8 cnt;

    if (!flag_refresh_seg)
        return;

    //显示主菜单
    if (ctr.sub_menu == 0)
    {
        BSP_SEG_Show_Menu(ctr.menu);
        flag_refresh_seg = false;
        return;
    }
    //显示子菜单
    switch (ctr.menu)
    {
    case M_A:
        if (ctr.sub_menu == M_A_1)
            BSP_SEG_Show_SubMenu_AH(ctr.menu, SEG_SYMBOL[0], SEG_DIGIT[ctr.auto_gear / 10], SEG_DIGIT[ctr.auto_gear % 10]);
        break;
    case M_H:
        if (ctr.sub_menu == M_H_1)
            BSP_SEG_Show_SubMenu_AH(ctr.menu, SEG_SYMBOL[6], SEG_DIGIT[ctr.hand_pd.pwidth / 10], SEG_DIGIT[ctr.hand_pd.pwidth % 10]);
        else if (ctr.sub_menu == M_H_2)
            BSP_SEG_Show_SubMenu_AH(ctr.menu, SEG_SYMBOL[7], SEG_DIGIT[ctr.hand_pd.nwidth / 10] | SEG_SYMBOL[1], SEG_DIGIT[ctr.hand_pd.nwidth % 10]);
        else if (ctr.sub_menu == M_H_3)
            BSP_SEG_Show_SubMenu_AH(ctr.menu, SEG_SYMBOL[8], SEG_DIGIT[ctr.hand_pd.num / 10], SEG_DIGIT[ctr.hand_pd.num % 10]);
        break;
    case M_U:
        if (ctr.sub_menu == M_U_1)
            BSP_SEG_Show_SubMenu_U(ctr.menu, SEG_SYMBOL[0], ctr.voltage);
        break;
    case M_L:
        if (ctr.sub_menu == M_L_1)
            BSP_SEG_Show_SubMenu_L(ctr.menu, SEG_SYMBOL[0], ctr.weld_count);
        break;
    case M_R:
        if (ctr.sub_menu == M_R_1)
        {
            BSP_SEG_Show_SubMenu_R(ctr.menu, 0x58, 0x18, 0x50); //clr
            if (ctr.reset == 1)
            {
                cnt = 3;
                while (cnt--)
                {
                    BSP_SEG_Show_SubMenu_R(ctr.menu, SEG_SYMBOL[0], SEG_SYMBOL[0], SEG_SYMBOL[0]);
                    delay_ms(200);
                    BSP_SEG_Show_SubMenu_R(ctr.menu, 0x58, 0x18, 0x50);
                    delay_ms(500);
                }
                BSP_BUZZER_Sound();
            }
        }
        break;
    default:
        break;
    }
    flag_refresh_seg = false;
}

//按键控制处理
void keyCtrProcess() large
{
    boolean tmp; 
    if (flag_sleep) //唤醒不做响应
        return;

    if (!BSP_KBD_Scan()) 
        return;
    //BSP_UART_Println(UART1, "key_code: %02bX,%02bX,%02bX,%02bX", BSP_KBD_KeyCode(K_SET), BSP_KBD_KeyCode(K_ADD), BSP_KBD_KeyCode(K_SUB), BSP_KBD_KeyCode(K_CFM));

    tmp = keyAction();
    flag_refresh_seg |= tmp;
    flag_save_params |= tmp;
    sleep_countdown = SLEEP_TIME;   //重新倒计时
}

//焊接
void welder() large
{
    WELDER_STATE_E_TYP chk_state;

    if (ctr.menu != M_A && ctr.menu != M_H) // A H 才可焊接
        return;
    if (ctr.sub_menu == 0) // 进入子菜单 才可焊接
        return;

    chk_state = BSP_WELDER_State();
    if (BSP_WELDER_StateIsChange(chk_state))
        BSP_UART_Println(UART1, "chk_state: %bX", chk_state);
    if (chk_state == JUST_TOUCHED)
    {
        sleep_countdown = SLEEP_TIME;   //重新倒计时
        // 嘀~   嘀嘀~
        BSP_BUZZER_Sound();
        delay_ms(500);
        BSP_BUZZER_Sound();
        delay_ms(50);
        BSP_BUZZER_Sound();
        // 启动焊接
        BSP_WELDER_Start(getPd());
    }
    else if (chk_state == JUST_FINISHED)
    {
        ctr.weld_count++;   //焊接次数++
        delay_ms(500);
        // 嘀嘀嘀~
        BSP_BUZZER_Sound();
        delay_ms(50);
        BSP_BUZZER_Sound();
        delay_ms(50);
        BSP_BUZZER_Sound();
        // 工作完毕，一个大延时
        BSP_WELDER_End();
        delay_ms(500);
    }
}

//保存参数
void saveParams() large
{
    if (!flag_save_params)
        return;
    if (!BSP_KBD_IsAllNone()) //有按键动作，延迟保存
        return;

    eep_params[0] = ctr.menu;
    eep_params[1] = ctr.sub_menu;
    eep_params[2] = ctr.auto_gear;
    eep_params[3] = ctr.hand_pd.pwidth;
    eep_params[4] = ctr.hand_pd.nwidth;
    eep_params[5] = ctr.hand_pd.num;
    eep_params[6] = ctr.weld_count;
    BSP_EEPROM_Write_Params(eep_params, EEP_PARAMS_LEN);

    flag_save_params = false;
}

//睡眠检测
void mcuSleepWatch() large
{
    if (flag_sleep) //睡眠中醒来
    {
        flag_sleep = false;
        sleep_countdown = SLEEP_TIME;   //重新倒计时
        flag_refresh_seg = true;
    }
    else
    {
        sleep_countdown--;
        // BSP_UART_Println(UART1, "sleep_countdown: %bu", sleep_countdown);
        if (sleep_countdown == 0)
        {
            flag_sleep = true;
            BSP_SEG_Black();    //关数码管
            BSP_POWER_Off();    //MCU掉电模式
        }
    }
}

/*
*******************************************************************************
*                                   函数
*******************************************************************************
*/
void setup()
{
    BSP_Init();

    initVariable();

    delay_ms(500);
    BSP_BUZZER_Sound(); // 自检发声
    BSP_UART_Println(UART1, "setup complete!");
}

void createTask()
{
    OS_CreateTask(0, 100, refreshSegShow);  //数码管刷新，立即运行，后每100ms运行一次
    OS_CreateTask(11, 10, keyCtrProcess);   //按键控制扫描，每10ms扫描一次
    OS_CreateTask(12, 200, welder);         //焊接
    OS_CreateTask(13, 1000, saveParams);    //保存参数
    OS_CreateTask(16, 1000, mcuSleepWatch); //睡眠检测
}
