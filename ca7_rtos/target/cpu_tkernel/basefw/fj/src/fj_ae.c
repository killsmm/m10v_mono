/**
 * @file		fj_ae.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_ae.h"
#include "mode.h"
// delete_basefw\fj\3a
//#include "3a.h"

/******************************************************************************
 Define
******************************************************************************/
#define MODE_CHECK_BOOT			(D_BF_MODE_BASE_CODE|D_BF_MODE_CATEGORY_ALL)
#define MODE_CHECK_VIEW			(D_BF_MODE_VIEW_CODE|D_BF_MODE_CATEGORY_VIEW|D_BF_MODE_CATEGORY_CALIBRATION|D_BF_MODE_CATEGORY_CAPTURE|D_BF_MODE_CATEGORY_CAPTURE_SEQ)
#define MODE_CHECK_ALL			(D_BF_MODE_VIEW_CODE|D_BF_MODE_OTHER_CODE|D_BF_MODE_CATEGORY_ALL)
#define MODE_CHECK_VIEW_MOVIE	(MODE_CHECK_VIEW|D_BF_MODE_CATEGORY_CAPTURE_AVI)
#define MODE_CHECK_CAPTURE		(MODE_CHECK_VIEW_MOVIE|D_BF_MODE_CATEGORY_CALIBRATION|D_BF_MODE_CATEGORY_CAPTURE|D_BF_MODE_CATEGORY_CAPTURE_SEQ)

/******************************************************************************
 Program
******************************************************************************/
FJ_ERR_CODE FJ_SetAEAWBWindow( UCHAR aeawb_id, AEAWBWinInfo* wininfo )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_CAPTURE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_setaeawbwindow(aeawb_id, wininfo);
	//}

	return ret;
}

FJ_ERR_CODE FJ_GetAEAWBWindow( UCHAR aeawb_id, AEAWBWinInfo* wininfo )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_CAPTURE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_getaeawbwindow(aeawb_id, wininfo);
	//}

	return ret;
}

FJ_ERR_CODE FJ_3ASetComputeValueRatio( UCHAR aeawb_id, AAAComputeRatioInfo* aaacmp )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_CAPTURE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_3asetcomputevalueratio(aeawb_id, aaacmp);
	//}

	return ret;
}

FJ_ERR_CODE FJ_3AGetComputeValueRatio( UCHAR aeawb_id, AAAComputeRatioInfo* aaacmp )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//if(BF_Mode_Check(MODE_CHECK_CAPTURE) == FJ_ERR_OK) {
	//	ret = fj_3agetcomputevalueratio(aeawb_id, aaacmp);
	//}
	return ret;
}

FJ_ERR_CODE FJ_SetAEAWBWDWindowMode( UCHAR aeawb_id, UCHAR wb_win_mode )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check(MODE_CHECK_CAPTURE);

	// delete_basefw\fj\3a
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_setaeawbwdwindowmode(aeawb_id, wb_win_mode);
	//}

	return ret;
}

FJ_ERR_CODE FJ_GetAEAWBWDWindowMode( UCHAR aeawb_id, UCHAR* wb_win_mode )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//if(BF_Mode_Check(MODE_CHECK_CAPTURE) == FJ_ERR_OK) {
	//	ret = fj_getaeawbwdwindowmode(aeawb_id, wb_win_mode);
	//}

	return ret;
}

// --- REMOVE_CAMERAIF BEGIN (simple tool) ---
FJ_ERR_CODE FJ_AE_Loading_Complete( E_BF_AE_BIN_IDX type, BYTE* addr, ULONG bytes )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret =fj_ae_loading_complete( type, addr, bytes );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AE_Tbl_Get_Sdram_Common( VOID* ae_param_tbl )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret =fj_ae_tbl_get_sdram_common( ae_param_tbl );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AE_Tbl_Set_Force_Reload( VOID )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret =fj_ae_tbl_set_force_reload();
	//}

	return ret;
}

// --- REMOVE_CAMERAIF END (simple tool) ---

