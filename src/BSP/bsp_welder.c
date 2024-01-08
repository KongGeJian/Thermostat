/*
*********************************************************************************************************
*                                        BOARD SUPPORT PACKAGE
*
* Name    : 焊接
* Version : V1.0
* Author  : 空格键
* ----------------------------
* Note(s) : 参考 [README.md - 工作状态机]
*             T1 16位自动重载模式，精确控制时间。驱动、检测 均使用光耦隔离。
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "bsp_welder.h"



/*
*********************************************************************************************************
*                                               DEFINE
*********************************************************************************************************
*/

#define CHK P16     //焊笔触碰检测：初始拉高，1-未触碰，0-触碰
#define DD  P11     //驱动：初始拉高，0-驱动MOS管导通。输出脉冲信号

#define DD_Start()   DD=1  //驱动启动
#define DD_Stop()    DD=0  //驱动停止
#define DD_IsStart() DD==1 //驱动是否启动中

#define T_100US (65536-SYSclk/10000)     //0.1ms timer 重载值


u16 idata cycle;    //脉冲周期
u16 idata ptime;    //正脉冲时间

volatile u8 idata  counter;     //脉冲计数器
volatile u16 idata time_count;  //时间计数

volatile WELDER_STATE_E_TYP xdata state; //焊接状态



/*
*********************************************************************************************************
*                                             INTERRUPT
*********************************************************************************************************
*/

/*T1中断服务程序*/
void T1_ISR() interrupt 3 using 2
{
    time_count++;
    if (time_count == cycle)
    {
        counter--;
        if (counter == 0)
        {
            DD_Stop(); //驱动停止
            TR1= 0;    //T1停止运行
            counter = 0;
            time_count = 0;
        } 
        else
        {
            DD_Start(); //驱动启动
            time_count = 0;
        }
    }
    else if (time_count == ptime)
    {
        DD_Stop(); //驱动停止
    }   
}

/*
*********************************************************************************************************
* Description : 焊接初始化
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_WELDER_Init(void) large
{
    P1M0 &= ~0x40;      //CHK设置为准双向口
    P1M1 &= ~0x40;
    // P1PU |= 0x40;       //使能内部4.1K上拉电阻
    P1M0 &= ~0x02;      //DD设置为准双向口
    P1M1 &= ~0x02;

    CHK = 1;            //引脚拉高
    DD_Stop();

    AUXR |= 0x40;       //定时器1为1T模式
    TMOD &= ~0xF0;      //定时器1为16位自动重装模式
    TR1= 0;             //T1停止运行
    TL1 = T_100US;      //初始化T1自动重装低字节
    TH1 = T_100US >> 8; //初始化T1自动重装高字节

    ET1 = 1;    //启用定时器1中断
    EA = 1;     //打开全局中断开关

    counter = 0;
    state = UNTOUCHED;
}

/*
*********************************************************************************************************
* Description : 获取焊接状态
*
* Argument(s) : none.
*
* Return(s)   : [WELDER_STATE_E_TYP]
*
* Note(s)     : 注意，焊接工作期间，会将检测信号CHK恢复成高（未触碰），所以这里引入counter=0焊接完毕判断
*********************************************************************************************************
*/
WELDER_STATE_E_TYP BSP_WELDER_State() large
{
    switch (state)
    {
    case UNTOUCHED:
        if (CHK == 0)  // 刚按下
        {
            delay_ms(10);
            if (CHK == 0)
            {
                state = JUST_TOUCHED;
            }
        }
        break;
    case WORKING:
        if (counter == 0) // counter!=0表示焊接中，counter=0表示焊接完毕
        {
            state = JUST_FINISHED;
        }
        break;
    case LIFTING:
        if (CHK == 1) // 刚抬起
        {
            delay_ms(100); // 工作完毕，如果还未抬起，CHK会恢复到0
            if (CHK == 1)
            {
                state = JUST_LIFTED;
            }
        }
        break;
    case JUST_LIFTED:
        state = UNTOUCHED;
        break;
    default:
        state = UNTOUCHED;
        break;
    }
    return state;
}

/*
*********************************************************************************************************
* Description : 焊接启动
*
* Argument(s) : *pd - 脉冲驱动指针
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_WELDER_Start(PD_TYP *pd) large
{
    // 校验
    if (pd->pwidth > PD_WIDTH_MAX || pd->pwidth < PD_P_WIDTH_MIN)
        return;
    if (pd->nwidth > PD_WIDTH_MAX || pd->nwidth < PD_N_WIDTH_MIN)
        return;
    if (pd->num > PD_NUM_MAX || pd->num < PD_NUM_MIN)
        return;
    
    if (counter != 0)   //counter不为0，表示上次工作还未完成
        return;

    cycle = pd->pwidth * 10 + pd->nwidth;
    ptime = pd->pwidth * 10;
    counter = pd->num + 1;  //+1是第0.1ms，不做驱动
    time_count = cycle - 1; //同上

    TL1 = T_100US;      //设置T1低字节
    TH1 = T_100US >> 8; //设置T1高字节
    TR1=1;              //T1开始运行

    state = WORKING;    //工作中
}

/*
*********************************************************************************************************
* Description : 焊接结束
*********************************************************************************************************
*/
void BSP_WELDER_End() large
{
    state = LIFTING; //待抬起
}

/*
*********************************************************************************************************
* Description : 焊接装填是否改变
*
* Argument(s) : curr - 当前装填
*
* Return(s)   : true-状态改变，否则false
*********************************************************************************************************
*/
boolean BSP_WELDER_StateIsChange(WELDER_STATE_E_TYP curr) large
{
    static WELDER_STATE_E_TYP prev = UNTOUCHED;
    if (curr != prev)
    {
        prev = curr;
        return true;
    }
    return false;
}

/**********************************************RND******************************************************/
