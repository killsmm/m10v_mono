/**
 * @file		fj_usbfunc.h
 * @brief		USB Func header file.
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef _FJ_USBFUNC_H
#define _FJ_USBFUNC_H

/*--------------------------------------------------------------------------
Include Files
--------------------------------------------------------------------------*/
#include "fj_memory.h"

/*--------------------------------------------------------------------------
Types/Constants
--------------------------------------------------------------------------*/
#define CHIP_TEST_RESERVED_SIZE (2*1024)

/*  REALOS Flag value of FID_USB_FUNC */
#define	FLG_USB_FUNC_CFG		(0x00000001)	/* USB Configured */

/* Transfer command definition */
#define CMD_SETPARAM            0x0010
#define CMD_GETPARAM            0x0011
#define CMD_GETPARAMLONG        0x0012
#define CMD_MONITORCMD          0x0013
#define CMD_SETMEMLOCATIONPTR   0x0014
#define CMD_CONFIGMEMACCESS     0x0015
#define CMD_GETLONG             0x0016
#define CMD_SETLONG             0x0017

#define CMD_POLL_STATUS         0x0031
//#define CMD_PARTITION	0x0018
//#define CMD_PROGRAM	0x0020
//#define CMD_CRE_DIR	0x0021
//#define CMD_CRE_FILE	0x0022
//#define CMD_POWER_OFF	0x0030
#define CMD_EXIT	0x00EE

#define OP_FAILED 0x03
#define CMD_NG    0xFF

#define MAX_PACKET_LEN_BYBYTE 64
#define MAX_CMD_LEN_BYBYTE    8


/* Transfer command structure */
typedef struct SetParamInfoTag
{
    UINT16 wParamID;
    UINT16 wValue;
}SetParamInfo;

typedef struct GetParamInfoTag
{
    USHORT wParamID;
    USHORT wValue;
}GetParamInfo;

typedef struct GetParamLongTag
{
    INT32 ParamID;
    LONG  value;
}GetParamLongInfo;

typedef struct SetMemLocationPtrTag
{
    FJ_MEMORY_LOCATION  wLocation;
    LONG                bufferAddr;
    UINT32              size;
}SetMemLocationPtrInfo;

//typedef struct ConfigMemAccessTag
//{
//    FJ_MEM_TYPE byMemType;
//    UINT32      ulStartAddr;
//    UINT32      ulSize;
//}ConfigMemAccessInfo;

typedef struct GetLongTag
{
    UINT32 sizeByByte;
}GetLongInfo;

typedef struct SetLongTag
{
    UINT32 sizeByByte;
}SetLongInfo;

/*------------------------------------------------------------------------------
    Macros
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Variable
------------------------------------------------------------------------------*/
extern volatile BYTE gCustomerArea[CHIP_TEST_RESERVED_SIZE];  /* FJ Samuel Lin, 2008/4/7, ChipTest reserved memory */

/*------------------------------------------------------------------------------
    extern functions
------------------------------------------------------------------------------*/
FJ_ERR_CODE FJ_MonitorCmd(PBYTE pCmd);
FJ_ERR_CODE FJ_GetLong(PBYTE pBuffer, UINT32 sizeByByte);
FJ_ERR_CODE FJ_SetLong(PBYTE pBuffer, UINT32 sizeByByte);


#endif //_FJ_USBFUNC_H
