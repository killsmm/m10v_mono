/**
 * @file		jpgepc_wrap.h
 * @brief		wrapper for JPEG fixed-length calc and Encode Ctrl
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _JPGEPC_WRAP_H_
#define _JPGEPC_WRAP_H_

#include "ddim_typedef.h"

// JPGEPC lib include files
#include "jpgepc.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

typedef struct {
	UCHAR					downsp_type;			// Downsample type (Parameter of Im_JPEG_Set_Down_Sampling_Rate)
	UCHAR					max_retry;				// Maximum retry count. (1~D_JPGEPC_RETRY_MAX_COUNT)
	UINT32					shift;					// Shift value.
	UINT32					ratio;					// Target ratio.
	ULONG					target_bytes;			// Target bytes.
} T_BF_JPGEPC_WRAP_TUNING_PARAM;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function													  	*/
/*----------------------------------------------------------------------*/
extern VOID		Jpgepc_Wrap_Set_Tuning_Param( T_BF_JPGEPC_WRAP_TUNING_PARAM* const tuning_param, const T_BF_JPGENC_PARAM* const jpgenc_param );
extern VOID		Jpgepc_Wrap_Update_Qtable( const UINT32 quality, const T_IM_JPEG_QUAT_TBL_PACK* const quant_tbl );
extern INT32	Jpgepc_Wrap_Print_Measure_Time( const CHAR* const comment );
extern INT32	Jpgepc_Wrap_Start_Measure_Time( VOID );
extern VOID		Jpgepc_Wrap_Err_Printer( const CHAR* const format, ... );

#endif

