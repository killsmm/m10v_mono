/**
 * @file		fj_flck.c
 * @brief		Flicker Setting API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_flck.h"
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

/******************************************************************************
 Program
******************************************************************************/
FJ_ERR_CODE FJ_SetFLCKWindow( UCHAR flck_id, FLCKWinInfo* wininfo )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_setflckwindow( flck_id, wininfo );
	//}

	return ret;
}

FJ_ERR_CODE FJ_GetFLCKWindow( UCHAR flck_id, FLCKWinInfo* wininfo )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_getflckwindow( flck_id, wininfo );
	//}

	return ret;
}

FJ_ERR_CODE FJ_SetFLCKDataformat( UCHAR flck_id, UCHAR format_type )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_setflckdataformat( flck_id, format_type );
	//}

	return ret;
}

FJ_ERR_CODE FJ_GetFLCKDataformat( UCHAR flck_id, UCHAR* format_type )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_getflckdataformat( flck_id, format_type );
	//}

	return ret;
}

FJ_ERR_CODE FJ_SetFLCKWDMode( UCHAR flck_id, UCHAR wd_mode )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_setflckwdmode( flck_id, wd_mode );
	//}

	return ret;
}

FJ_ERR_CODE FJ_GetFLCKWDMode( UCHAR flck_id, UCHAR* wd_mode )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_getflckwdmode( flck_id, wd_mode );
	//}

	return ret;
}

FJ_ERR_CODE FJ_SetFLCKLevelGate( UCHAR flck_id, FLCKLevelGate* gate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_setflcklevelgate( flck_id, gate );
	//}

	return ret;
}

FJ_ERR_CODE FJ_GetFLCKLevelGate( UCHAR flck_id, FLCKLevelGate* gate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_getflcklevelgate( flck_id, gate );
	//}

	return ret;
}

FJ_ERR_CODE FJ_SetFLCKMatrixLevelGate( UCHAR flck_id, FLCKMtxLevelGate* mtx_gate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_setflckmatrixlevelgate( flck_id, mtx_gate );
	//}

	return ret;
}

FJ_ERR_CODE FJ_GetFLCKMatrixLevelGate( UCHAR flck_id, FLCKMtxLevelGate* mtx_gate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_getflckmatrixlevelgate( flck_id, mtx_gate );
	//}

	return ret;
}

FJ_ERR_CODE FJ_SetFLCKComputeValueRatio( UCHAR flck_id, FLCKComputeRationInfo* flckcmp )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_setflckcomputevalueratio( flck_id, flckcmp );
	//}

	return ret;
}

FJ_ERR_CODE FJ_GetFLCKComputeValueRatio( UCHAR flck_id, FLCKComputeRationInfo* flckcmp )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_getflckcomputevalueratio( flck_id, flckcmp );
	//}

	return ret;
}

