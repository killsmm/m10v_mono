/**
 * @file		jpgepc_wrap.c
 * @brief		wrapper for JPEG fixed-length calc and Encode Ctrl
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// JPGEPC header
#include "jpgepc_wrap.h"

// BaseFW header
#include "still.h"
#include "still_utility.h"

#include "measure_time.h"
#include "debug.h"

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
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
VOID Jpgepc_Wrap_Set_Tuning_Param( T_BF_JPGEPC_WRAP_TUNING_PARAM* const tuning_param, const T_BF_JPGENC_PARAM* const jpgenc_param )
{
	UINT32 pixs;

	pixs = jpgenc_param->jpg_enc_mng.width * jpgenc_param->jpg_enc_mng.lines;

	tuning_param->ratio		= 0x00007999; // 0.95
	tuning_param->max_retry	= 30;

	if( pixs <= (160 * 120) ) {
		// ThumbNail
		tuning_param->downsp_type = D_IM_JPEG_DOWNSP_NONE;
		tuning_param->shift = 0;
	}
	else if( pixs <= (640 * 480) ) {
		// Main VGA
		tuning_param->downsp_type = D_IM_JPEG_DOWNSP_1_2;
		tuning_param->shift = 2;
	}
	else if( pixs <= (1700 * (1700 * 3 / 4)) ) {
		// Main 2M
		tuning_param->downsp_type = D_IM_JPEG_DOWNSP_1_4;
		tuning_param->shift = 4;
	}
	else {
		tuning_param->downsp_type = D_IM_JPEG_DOWNSP_1_8;
		tuning_param->shift = 6;
	}

	tuning_param->target_bytes = BF_Still_Calc_Target_Size( jpgenc_param->jpg_enc_mng.width, jpgenc_param->jpg_enc_mng.lines, jpgenc_param->mode, jpgenc_param->cr );
}

VOID Jpgepc_Wrap_Update_Qtable( const UINT32 quality, const T_IM_JPEG_QUAT_TBL_PACK* const quant_tbl )
{
	// delete basefw\fj\imgproc\still
	//BF_Still_Utility_Get_Jpeg_Encode_Qtable( quality, (T_IM_JPEG_QUAT_TBL_PACK* const)quant_tbl );
}

INT32 Jpgepc_Wrap_Print_Measure_Time( const CHAR* const comment )
{
	return BF_Measure_Time_Print( E_BF_MEASURE_TIME_ID_DEBUG_1, comment );
}

INT32 Jpgepc_Wrap_Start_Measure_Time( VOID )
{
	return BF_Measure_Time_Start( E_BF_MEASURE_TIME_ID_DEBUG_1 );
}

VOID Jpgepc_Wrap_Err_Printer( const CHAR* const format, ... )
{
	BF_Debug_Print_Error(( format ));
}

