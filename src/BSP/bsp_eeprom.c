/*
*********************************************************************************************************
*                                        BOARD SUPPORT PACKAGE
*
* Name    : EEPROM
* Version : V1.0
* Author  : 空格键
* ----------------------------
* Note(s) : STM8G1K08 EEPROM 大小、扇区、起始地址、结束地址 需要用户自定义
*             ----------------------------------------------------------------
*             大小  扇区  起始地址  结束地址
*             4K    8    0000h    0FFFh
*             ----------------------------------------------------------------
*             第一扇区：0000h - 01FFh
*             第二扇区：0200h - 03FFh
* Tip(s)  : 概念：
*             + 字节编程：是将“1”写成“1”或“0”，如果是扇区字节是“0”，只能写成“0”，不能由0变1。
*             + 扇区擦除：是将整个扇区所有字节变成“FFH”，由0变1也只有扇区擦除可以做到。
*             + 扇区：每个扇区512字节，擦除是以扇区为单位。
*             所以写数据前先要对扇区擦除，再字节编程写数据。
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "bsp_eeprom.h"



/*
*********************************************************************************************************
*                                               DEFINE
*********************************************************************************************************
*/
/*声明与IAP关联的SFR*/
// sfr IAP_DATA    =   0xC2;           //IAP data register
// sfr IAP_ADDRH   =   0xC3;           //IAP address HIGH
// sfr IAP_ADDRL   =   0xC4;           //IAP address LOW
// sfr IAP_CMD     =   0xC5;           //IAP command register
// sfr IAP_TRIG    =   0xC6;           //IAP trigger register
// sfr IAP_CONTR   =   0xC7;           //IAP control register
// sfr IAP_TPS     =   0xF5;           //IAP wait time control register

/*定义ISP/IAP/EEPROM命令*/
#define CMD_IDLE    0x00            //Stand-By
#define CMD_READ    0x01            //Byte-Read
#define CMD_PROGRAM 0x02            //Byte-Program/Byte-Write
#define CMD_ERASE   0x03            //Sector-Erase

/*定义IAP控制的ISP/IAP/EEPROM操作常数*/
#define ENABLE_IAP 0x80           //使能IAP操作

#define TPS_VAL ((u8)(FOSC/1000000ul + 0.5)) //等待时间

//EEPROM的起始地址
#define IAP_ADDR_SECTOR_1 0x0000
#define IAP_ADDR_SECTOR_2 0x0200

//扇区1版本标识 ====注意：当数据格式改变时，更换以下标志，否则程序可能会跑乱
#define VERSION_SECTOR_1 0x01


/*
*********************************************************************************************************
*                                         PRIVATE FUNCTION
*********************************************************************************************************
*/

/*禁用ISP/IAP/EEPROM功能，使MCU处于安全状态*/
static void _IapIdle()
{
    IAP_CONTR = 0;      //关闭IAP功能
    IAP_CMD = 0;        //清除命令寄存器
    IAP_TRIG = 0;       //清除触发寄存器
    IAP_ADDRH = 0x80;   //将地址设置到非IAP区域
    IAP_ADDRL = 0;
}

/*
*********************************************************************************************************
* Description : EEPROM初始化。
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_EEPROM_Init(void) large
{
    _nop_();    // EMPTY
}

/*
*********************************************************************************************************
* Description : 从ISP/IAP/EEPROM区域读取一个字节
*
* Argument(s) : addr - ISP/IAP/EEPROM 地址
*
* Return(s)   : Flash data
*
* Note(s)     : none.
*********************************************************************************************************
*/
byte BSP_EEPROM_ReadByte(word addr) compact
{
    byte dat;

    IAP_CONTR = ENABLE_IAP; //使能IAP
    IAP_TPS = TPS_VAL;      //设置等待时间参数
    IAP_CMD = CMD_READ;     //设置IAP读命令
    IAP_ADDRL = addr;       //设置IAP低地址
    IAP_ADDRH = addr >> 8;  //设置IAP高地址
    IAP_TRIG = 0x5a;        //写触发命令(0x5a)
    IAP_TRIG = 0xa5;        //写触发命令(0xa5)
    _nop_();                //一个空时钟操作，CPU等待IAP操作完成
    dat = IAP_DATA;         //读IAP数据
    _IapIdle();             //关闭IAP功能

    return dat;
}

/*
*********************************************************************************************************
* Description : 将一个字节编程/写到ISP/IAP/EEPROM区域
*
* Argument(s) : addr - ISP/IAP/EEPROM 地址
*               dat  - 要写入的数据
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_EEPROM_WriteByte(word addr, byte dat) compact
{
    IAP_CONTR = ENABLE_IAP; //使能IAP
    IAP_TPS = TPS_VAL;      //设置等待时间参数
    IAP_CMD = CMD_PROGRAM;  //设置IAP写命令
    IAP_ADDRL = addr;       //设置IAP低地址
    IAP_ADDRH = addr >> 8;  //设置IAP高地址
    IAP_DATA = dat;         //写IAP数据
    IAP_TRIG = 0x5a;        //写触发命令(0x5a)
    IAP_TRIG = 0xa5;        //写触发命令(0xa5)
    _nop_();                //一个空时钟操作，CPU等待IAP操作完成
    _IapIdle();             //关闭IAP功能
}

/*
*********************************************************************************************************
* Description : 擦除一个扇区区域
*
* Argument(s) : addr - ISP/IAP/EEPROM 地址
*
* Note(s)     : 擦除后，扇区字节都是 0xFF
*********************************************************************************************************
*/
void BSP_EEPROM_EraseSector(word addr) compact
{
    IAP_CONTR = ENABLE_IAP; //使能IAP
    IAP_TPS = TPS_VAL;      //设置等待参数12MHz
    IAP_CMD = CMD_ERASE;    //设置IAP擦除命令
    IAP_ADDRL = addr;       //设置IAP低地址
    IAP_ADDRH = addr >> 8;  //设置IAP高地址
    IAP_TRIG = 0x5a;        //写触发命令(0x5a)
    IAP_TRIG = 0xa5;        //写触发命令(0xa5)
    _nop_();                //一个空时钟操作，CPU等待IAP操作完成
    _IapIdle();             //关闭IAP功能
}

/*
*********************************************************************************************************
* Description : 从*eep_params写配置参数到eep中
*
* Argument(s) : *eep_params - 配置参数指针，通用无类型指针
*               len - 参数占用字节长度
*
* Return(s)   : true-成功，false-失败
*
* Note(s)     : 这里限制在一个扇区，len长度不要超过500.
*********************************************************************************************************
*/
boolean BSP_EEPROM_Write_Params(void *eep_params, u16 len) large
{
    u16 i;
    byte *eep_p = (byte *)eep_params;
    word addr = IAP_ADDR_SECTOR_1;

    if (len > 500)
        return false;

    BSP_EEPROM_EraseSector(addr);           //擦扇区
    BSP_EEPROM_WriteByte(addr, VERSION_SECTOR_1);   //第一个字节为版本标识
    addr++;

    for (i = 0; i < len; i++)
        BSP_EEPROM_WriteByte(addr++, *eep_p++);

    return true;
}

/*
*********************************************************************************************************
* Description : 从eep中读配置参数到*eep_params中
*
* Argument(s) : *eep_params - 配置参数指针，通用无类型指针
*               len - 参数占用字节长度
*
* Return(s)   : true-成功，false-失败
*
* Note(s)     : 这里限制在一个扇区，len长度不要超过500.
*********************************************************************************************************
*/
boolean BSP_EEPROM_Read_Params(void *eep_params, u16 len) large
{
    u8 i;
    byte *eep_p = (byte *)eep_params;
    word addr = IAP_ADDR_SECTOR_1;
    
    if (len > 500)
        return false;

    if (BSP_EEPROM_ReadByte(addr) != VERSION_SECTOR_1)  //第一个字节为版本标识
        return false;
    addr++;

    for (i = 0; i < len; i++)
        *eep_p++ = BSP_EEPROM_ReadByte(addr++);

    return true;
}


/**********************************************RND******************************************************/
