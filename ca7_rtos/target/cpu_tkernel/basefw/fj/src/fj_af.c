/**
 * @file		fj_af.c
 * @brief		Auto Focus Setting API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_af.h"
#include "mode.h"
// delete_basefw\fj\3a
//#include "3a.h"

/******************************************************************************
 Define
******************************************************************************/
#define MODE_CHECK_BOOT			(D_BF_MODE_BASE_CODE|D_BF_MODE_CATEGORY_ALL)
#define MODE_CHECK_VIEW			(D_BF_MODE_VIEW_CODE|D_BF_MODE_CATEGORY_VIEW|D_BF_MODE_CATEGORY_CALIBRATION|D_BF_MODE_CATEGORY_CAPTURE)
#define MODE_CHECK_ALL			(D_BF_MODE_VIEW_CODE|D_BF_MODE_OTHER_CODE|D_BF_MODE_CATEGORY_ALL)
#define MODE_CHECK_VIEW_MOVIE	(MODE_CHECK_VIEW|D_BF_MODE_CATEGORY_CAPTURE_AVI)

/******************************************************************************
 Program
******************************************************************************/
FJ_ERR_CODE FJ_AFSetWindow( UCHAR af_id, UCHAR af_win_no, AFWinInfo* af_wininfo )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afsetwindow( af_id, af_win_no, af_wininfo );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFSetGlobalWindow( UCHAR af_id, AFWinRect* afg_window )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afsetglobalwindow( af_id, afg_window );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFSetLuminanceCalc( UCHAR af_id, UCHAR tap_no, AFLumiInfo* af_lumiinfo )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afsetluminancecalc( af_id, tap_no, af_lumiinfo );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFSetVerticalWindow( UCHAR af_id, AFVWinInfo* afv_wininfo )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afsetverticalwindow( af_id, afv_wininfo );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFSetHorizontalCalc( UCHAR af_id, UCHAR tap_no, AFHInfo* af_horizoninfo )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afsethorizontalcalc( af_id, tap_no, af_horizoninfo );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFSetPowerEnable( UCHAR af_id, AFPowerState* af_pow_state )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afsetpowerenable( af_id, af_pow_state );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetLuminanceData( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgetluminancedata( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetLuminanceBrightCount( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgetluminancebrightcount( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetLuminanceDarkCount( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgetluminancedarkcount( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetHorizontalData0( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgethorizontaldata0( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetHorizontalData1( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgethorizontaldata1( af_id, af_win_no, data );
	//}

	return ret;
}


FJ_ERR_CODE FJ_AFGetHorizontalData2( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgethorizontaldata2( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetVerticalData( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgetverticaldata( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetLuminanceDataMaxValue( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgetluminancedatamaxvalue( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetLuminanceDataMinValue( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgetluminancedataminvalue( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetDataMaxValue( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgetdatamaxvalue( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetDataMinValue( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgetdataminvalue( af_id, af_win_no, data );
	//}

	return ret;
}


FJ_ERR_CODE FJ_AFGetHorizontalLineAccumMaxData0( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgethorizontallineaccummaxdata0( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetHorizontalLineAccumMaxData1( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgethorizontallineaccummaxdata1( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFGetHorizontalLineAccumMaxData2( UCHAR af_id, UCHAR af_win_no, ULONG* data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afgethorizontallineaccummaxdata2( af_id, af_win_no, data );
	//}

	return ret;
}

FJ_ERR_CODE FJ_AFSetCalcDataReady( UCHAR af_id, UCHAR on_off )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//ret = BF_Mode_Check(MODE_CHECK_VIEW_MOVIE);
	//if (ret == FJ_ERR_OK) {
	//	ret = fj_afsetcalcdataready(af_id, on_off);
	//}

	return ret;
}

