/**
 * @file		fj_awb.c
 * @brief		Auto White Balance Setting API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_awb.h"
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
FJ_ERR_CODE FJ_AWBSetVideoGain( UCHAR demosaic_id, WB_Info* wb, SHORT* cc_matrix, UCHAR set_timming )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbsetvideogain(demosaic_id, wb, cc_matrix, set_timming);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBGetVideoGain( UCHAR demosaic_id, WB_Info* wb, SHORT* cc_matrix )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbgetvideogain(demosaic_id, wb, cc_matrix);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBSetStillGain( UCHAR demosaic_id, WB_Info* wb, SHORT* cc_matrix )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbsetstillgain(demosaic_id, wb, cc_matrix);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBGetStillGain( UCHAR demosaic_id, WB_Info* wb, SHORT* cc_matrix )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbgetstillgain(demosaic_id, wb, cc_matrix);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBSetVideoClipLevel( UCHAR demosaic_id, WB_Info* wb )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbsetvideocliplevel(demosaic_id, wb);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBSetStillClipLevel( UCHAR demosaic_id, WB_Info* wb )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbsetstillcliplevel(demosaic_id, wb);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBGetVideoClipLevel( UCHAR demosaic_id, WB_Info* wb )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbgetvideocliplevel(demosaic_id, wb);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBGetStillClipLevel( UCHAR demosaic_id, WB_Info* wb )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbgetstillcliplevel(demosaic_id, wb);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBSetLevelGate( UCHAR sro_id, AWBLevelGate* gate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbsetlevelgate(sro_id, gate);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBGetLevelGate( UCHAR sro_id, AWBLevelGate* gate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbgetlevelgate(sro_id, gate);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBSetMatrixLevelGate( UCHAR sro_id, AWBMtxLevelGate* mtx_gate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbsetmatrixlevelgate(sro_id, mtx_gate);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBGetMatrixLevelGate( UCHAR sro_id, AWBMtxLevelGate* mtx_gate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbgetmatrixlevelgate(sro_id, mtx_gate);
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBSetPGAIN( UCHAR demosaic_id, UCHAR block_num, BayAdjustInfo* bay_adjust )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbsetpgain( demosaic_id, block_num, bay_adjust );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBGetPGAIN( UCHAR demosaic_id, UCHAR block_num, BayAdjustInfo* bay_adjust )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbgetpgain( demosaic_id, block_num, bay_adjust );
	//}

	return ret;
}

/**
Set PGAIN decimal point position.
*/
FJ_ERR_CODE FJ_AWBSetPGAIN_Decimal_Pos( UCHAR demosaic_id, UCHAR block_num, E_FJ_AWB_PGAIN_DECI_POS decimal_point_position )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);	// TODO
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbsetpgain_decimal_pos( demosaic_id, block_num, decimal_point_position );
	//}

	return ret;
}

/**
Get PGAIN decimal point position.
*/
FJ_ERR_CODE FJ_AWBGetPGAIN_Decimal_Pos( UCHAR demosaic_id, UCHAR block_num, E_FJ_AWB_PGAIN_DECI_POS* decimal_point_position )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);	// TODO
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbgetpgain_decimal_pos( demosaic_id, block_num, decimal_point_position );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBSetDataformat( UCHAR sro_id, UCHAR format_type )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbsetdataformat( sro_id, format_type );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBGetDataformat( UCHAR sro_id, UCHAR* format_type )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbgetdataformat( sro_id, format_type );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBSetSlopeGain( UCHAR demosaic_id, const T_FJ_AWB_SLP_GAIN* slp_gain )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbsetslopegain( demosaic_id, slp_gain );
	//}

	return ret;
}

// --- REMOVE_CAMERAIF BEGIN (simple tool) ---
FJ_ERR_CODE FJ_AWB_Loading_Complete( E_BF_AWB_BIN_IDX type, BYTE* addr, ULONG bytes )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret =fj_awb_loading_complete( type, addr, bytes );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWB_Tbl_Get_Sdram_Common( VOID* awb_param_tbl )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret =fj_awb_tbl_get_sdram_common( awb_param_tbl );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWB_Tbl_Set_Force_Reload( VOID )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret =fj_awb_tbl_set_force_reload();
	//}

	return ret;
}
// --- REMOVE_CAMERAIF END (simple tool) ---

// Calibration
FJ_ERR_CODE FJ_AWBSetCalibPgain( UCHAR demosaic_id, BayAdjustInfo* bay_adjust )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbsetcalibpgain( demosaic_id, bay_adjust );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AWBGetCalibPgain( UCHAR demosaic_id, BayAdjustInfo* bay_adjust )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if(ret == FJ_ERR_OK) {
	//	ret = fj_awbgetcalibpgain( demosaic_id, bay_adjust );
	//}

	return ret;
}