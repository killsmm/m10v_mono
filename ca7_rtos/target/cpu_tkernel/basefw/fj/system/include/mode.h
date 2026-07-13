/**
 * @file		mode.h
 * @brief		mode process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _MODE_H_
#define _MODE_H_

#include "fj_mode.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_BF_MODE_SECOND_CODE			(0x80000000)
#define D_BF_MODE_THIRD_CODE			(0x40000000)
#define D_BF_MODE_FOURTH_CODE			(0x20000000)

#define D_BF_MODE_BASE_CODE				(0x00000000)
#define D_BF_MODE_VIEW_CODE				(0x80000000)
#define D_BF_MODE_PB_CODE				(0x40000000)
#define D_BF_MODE_OTHER_CODE			(0x10000000)

#define D_BF_MODE_CATEGORY_POWER_DOWN	(0x08000000)
#define D_BF_MODE_CATEGORY_MENU			(0x04000000)
#define D_BF_MODE_CATEGORY_PB			(0x02000000)
#define D_BF_MODE_CATEGORY_RESTORE		(0x01000000)
#define D_BF_MODE_CATEGORY_STORE_IMAGE	(0x00800000)
#define D_BF_MODE_CATEGORY_CAPTURE_AVI	(0x00400000)
#define D_BF_MODE_CATEGORY_CAPTURE_SEQ	(0x00200000)
#define D_BF_MODE_CATEGORY_CAPTURE		(0x00100000)
#define D_BF_MODE_CATEGORY_CALIBRATION	(0x00080000)
#define D_BF_MODE_CATEGORY_VIEW			(0x00040000)
#define D_BF_MODE_CATEGORY_IDLE			(0x00020000)
#define D_BF_MODE_CATEGORY_BOOT			(0x00010000)
#define D_BF_MODE_CATEGORY_ALL			(0xFFFF0000)

#define D_BF_MODE_WAIT_SYSTEM_MODE		(0x00000001)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UINT32	prev_mode;				/* please refer to FJ_MODE */
	UINT32	current_mode;			/* please refer to FJ_MODE */
	UINT32	next_mode;				/* please refer to FJ_MODE */
	UINT32	req_mode;				/* please refer to FJ_MODE */
	UCHAR	sr1;
	USHORT	sr2;
} T_MODE_ATTR;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern T_MODE_ATTR gBF_Mode_Attr;

// active Base FW API counter
extern int gBF_Mode_BaseFW_API_Cnt;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

FJ_MODE fj_get_act_sysmode(VOID);
FJ_ERR_CODE	fj_querystatus( UINT32* pwUSR1, USHORT* pwUSR2 );
FJ_ERR_CODE	fj_getstatusregister( FJ_STATUS_REGISTER bRegister, USHORT* pwValue );
FJ_ERR_CODE	fj_setmode( FJ_MODE bNewMode );
FJ_ERR_CODE	fj_waitmodecomp( FJ_MODE wait_mode, UINT32 sleepTime, UINT32 repeatNum );
FJ_ERR_CODE	fj_startmode( VOID );
FJ_ERR_CODE	fj_stopmode( VOID );
BOOL fj_is_modestatus_stop( VOID );
FJ_MODE	BF_Mode_Get_Prev_Sysmode( VOID );
FJ_MODE	BF_Mode_Get_Next_Sysmode( VOID );
VOID	BF_Mode_Set_Next_Sysmode( FJ_MODE next_mode );
VOID	BF_Mode_Set_Request_Sysmode( FJ_MODE req_mode );
VOID	BF_Mode_Update_Sysmode_Change( VOID );
VOID	BF_Mode_Cancel_Sysmode_Change(VOID);
FJ_ERR_CODE BF_Mode_Check( UINT32 validModeCategory);
VOID	BF_Mode_Put_CheckPoint(UINT32 category, USHORT checkpoint);
INT32	BF_Mode_Ref_Checkpoint(UINT32 category, USHORT checkpoint);

#ifdef __cplusplus
}
#endif

#endif	// _MODE_H_
