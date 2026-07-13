/**
 * @file		iq_dpc_multipipe_tbl.h
 * @brief		Badpix Detection/Compensation control API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_MULTIPIPE_TBL_H__
#define __IQ_DPC_MULTIPIPE_TBL_H__

#include "fj_std.h"

#include "iq_dpc_tbl.h"
#include "iq_dpc_custom.h"

#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

// T_BF_IQ_DPC_AREA_INFO
typedef struct {
	USHORT		pos_x;						/**< Start position x				*/
	USHORT		pos_y;						/**< Start position y				*/
	USHORT		width;						/**< Width of macro processing area	*/
	USHORT		lines;						/**< Line of macro processing area	*/
} T_BF_IQ_DPC_AREA_INFO;

// T_BF_IQ_DPC_MULTIPIPE_TBL
typedef struct {
	UINT32							tbl_idx;
	BOOL							valid;
	U_BF_IQ_DPC_TBL_DATA*			top_addr;
	ULLONG							maxbytelen;
	UINT32							x_num;
	UINT32							y_num;
	T_BF_IQ_DPC_TBL_INFO			tbl_info[D_BF_IQ_DPC_MULTIPIPE_TBL_X_NUM][D_BF_IQ_DPC_MULTIPIPE_TBL_Y_NUM];
	T_BF_IQ_DPC_AREA_INFO			crop_win[D_BF_IQ_DPC_MULTIPIPE_TBL_X_NUM][D_BF_IQ_DPC_MULTIPIPE_TBL_Y_NUM];
} T_BF_IQ_DPC_MULTIPIPE_TBL;



/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Function																		*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


extern FJ_ERR_CODE BF_IQ_Dpc_MultiPipeTbl_Init( VOID );
extern T_BF_IQ_DPC_MULTIPIPE_TBL* BF_IQ_Dpc_MultiPipeTbl_Alloc( VOID );
extern VOID BF_IQ_Dpc_MultiPipeTbl_Free( T_BF_IQ_DPC_MULTIPIPE_TBL* const multipipe_tbl );
extern VOID BF_IQ_Dpc_MultiPipeTbl_Stat( const T_BF_IQ_DPC_MULTIPIPE_TBL* const multipipe_tbl );
extern VOID BF_IQ_Dpc_MultiPipeTbl_Print_SizeDbg( VOID );


#ifdef __cplusplus
}
#endif

#endif // __IQ_DPC_MULTIPIPE_TBL_H__

