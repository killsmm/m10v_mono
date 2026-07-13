/**
 * @file		NF_driver_wrapper.h
 * @brief		Nand Flash driver wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _NF_DRIVER_WRAPPER_H_
#define _NF_DRIVER_WRAPPER_H_

#include "../nf/include/mwnf.h"
#include "fs_if.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_NF_DRIVE_1 = 0,
	E_NF_DRIVE_2 = 1,
	E_NF_DRIVE_3 = 2,
	E_NF_DRIVE_4 = 3,
	E_NF_DRIVE_5 = 4,
	E_NF_DRIVE_6 = 5,
	E_NF_DRIVE_7 = 6,
} E_NF_DRIVE_NO;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//extern volatile NF_Device_Info NFdevInfo[];
//extern volatile FS_Nformat_Info NFformatInfo[];
extern volatile char gNF_Drive_Name[NF_MAX_DRV_NUM][2];
extern volatile unsigned long gNF_Drive_Protect[NF_MAX_DRV_NUM];
//extern volatile unsigned char gMW_Exs_IRAM_NF[];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
FW_INT32 NF_InitDataDrive(FW_VOID);

#endif	// _NF_DRIVER_WRAPPER_H_
