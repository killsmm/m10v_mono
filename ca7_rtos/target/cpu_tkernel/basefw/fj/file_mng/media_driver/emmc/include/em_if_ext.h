// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
 * @file em_if_ext.h
 * @brief external interface of functions group
 * @note None
 * @attention None
 *
 */
/*****************************************************************************/

// include -------------------------------------

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UINT32 partType;
	UINT32 startLBA;
	UINT32 partSize;
} T_EM_MBR_INFO;

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// drive no
typedef enum {
	E_MWEM_DRIVE_0 = 0,
	E_MWEM_DRIVE_1 = 1,
	E_MWEM_DRIVE_2 = 2,
	E_MWEM_DRIVE_3 = 3,
	E_MWEM_DRIVE_4 = 4,
	E_MWEM_DRIVE_5 = 5,
	E_MWEM_DRIVE_6 = 6 
} E_MWEM_DRIVE;

// function -------------------------------------
extern VOID MWEM_IntHandler(VOID);
extern INT32 MWEM_Init(VOID);
extern INT32 MWEM_Set_Config(UINT32 mode);
extern INT32 MWEM_Set_AccessPartition(UINT32 partition);
extern INT32 MWEM_Get_Config(UINT32* mode);
extern INT32 MWEM_DeviceInit(VOID);
extern INT32 MWEM_Bread(VOID *devinfo, ULONG start, ULONG cnt, CHAR* data);
extern INT32 MWEM_DirectRead(ULONG start, ULONG cnt, CHAR* data);
extern INT32 MWEM_Bwrite(VOID *devinfo, ULONG start, ULONG cnt, CHAR* data);
extern INT32 MWEM_DirectWrite(ULONG start, ULONG cnt, CHAR* data);
extern INT32 MWEM_Format(VOID);
extern INT32 MWEM_Format_Drive(E_MWEM_DRIVE drive_no);
extern INT32 MWEM_Read_MBR(UCHAR* d_buf);
extern INT32 MWEM_Read_MBR_Info(T_EM_MBR_INFO* mbrinfo, ULONG* par_num);
extern INT32 MWEM_Write_MBR(VOID);
extern INT32 MWEM_Check_MBR(VOID);
extern INT32 MWEM_Get_Drive_Num(VOID);
extern INT32 MWEM_Get_ParamBlk_Info(ULONG* param_blk_top, ULONG* param_blk_size);
extern INT32 MWEM_Get_ExceptionLog_Info(ULONG* exception_log_top);
extern VOID MWEM_Set_Interrupt_Enable(ULONG enable);
extern INT32 MWEM_End(VOID);
