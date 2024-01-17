/*
*********************************************************************************************************
*                                        BOARD SUPPORT PACKAGE
*
* Name    : EEPROM
* Version : V1.0
* Author  : 空格键
*********************************************************************************************************
*/

#ifndef __BSP_EEPROM_H__
#define __BSP_EEPROM_H__

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




/*
*********************************************************************************************************
*                                             FUNCTION
*********************************************************************************************************
*/

extern void BSP_EEPROM_Init(void) large;

extern byte BSP_EEPROM_ReadByte(word addr) compact;
extern void BSP_EEPROM_WriteByte(word addr, byte dat) compact;
extern void BSP_EEPROM_EraseSector(word addr) compact;

extern boolean BSP_EEPROM_Write_Params(void *eep_params, u16 len) large;
extern boolean BSP_EEPROM_Read_Params(void *eep_params, u16 len) large;


#endif
/***********************************************END*****************************************************/
