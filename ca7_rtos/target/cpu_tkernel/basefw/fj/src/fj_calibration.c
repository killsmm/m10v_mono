/**
 * @file		fj_calibration.c
 * @brief		Algorithm for Calibration API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "os_user_custom.h"
#include "ddim_typedef.h"
#include "fj_calibration.h"
// delete_basefw\fj\iq
//#include "iq_calib.h"
#include "mode.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define MODE_CHECK_ALL		(D_BF_MODE_VIEW_CODE|D_BF_MODE_OTHER_CODE|D_BF_MODE_CATEGORY_ALL)
#define MODE_CHECK_CALIB	(D_BF_MODE_VIEW_CODE|D_BF_MODE_OTHER_CODE|D_BF_MODE_CATEGORY_CALIBRATION)


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
// nothing special.


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

VOID FJ_IQ_CalibrationSetFormatPix( E_IQ_CALIB_FMT_PIX e_format_pix )
{
	// delete_basefw\fj\iq
	//FJ_ERR_CODE ret;

	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	fj_iq_calibrationsetformatpix( e_format_pix );
	//}
}

E_IQ_CALIB_FMT_PIX FJ_IQ_CalibrationGetFormatPix( VOID )
{
	// delete_basefw\fj\iq
	//FJ_ERR_CODE ret;

	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	return fj_iq_calibrationgetformatpix();
	//} else {
		return E_IQ_CALIB_FMT_PIX_ERR;
	//}
}

VOID FJ_IQ_CalibrationSetRawSaveToMedia( BOOL save )
{
	// delete_basefw\fj\iq
	//FJ_ERR_CODE ret;

	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	fj_iq_calibrationsetrawsavetomedia( save );
	//}
}

BOOL FJ_IQ_CalibrationGetRawSaveToMedia( VOID )
{
	BOOL ret = FALSE;

	// delete_basefw\fj\iq
	//if( BF_Mode_Check( MODE_CHECK_ALL ) ==  FJ_ERR_OK ) {
	//	ret = fj_iq_calibrationgetrawsavetomedia();
	//} else {
	//	ret = FALSE;
	//}

	return ret;
}

VOID FJ_IQ_CalibrationSetJpegSaveToMedia( BOOL save )
{
	// delete_basefw\fj\iq
	//FJ_ERR_CODE ret;

	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	fj_iq_calibrationsetjpegsavetomedia( save );
	//}
}

BOOL FJ_IQ_CalibrationGetJpegSaveToMedia( VOID )
{
	BOOL ret = FALSE;

	// delete_basefw\fj\iq
	//if( BF_Mode_Check( MODE_CHECK_ALL ) ==  FJ_ERR_OK ) {
	//	ret = fj_iq_calibrationgetjpegsavetomedia();
	//} else {
	//	ret = FALSE;
	//}

	return ret;
}

VOID FJ_IQ_CalibrationSetTarget( E_IQ_CALIB_TGT target )
{
	// delete_basefw\fj\iq
	//FJ_ERR_CODE ret;

	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	fj_iq_calibrationsettarget( target );
	//}
}

VOID FJ_IQ_CalibrationGetTarget( E_IQ_CALIB_TGT* target )
{
	// delete_basefw\fj\iq
	//FJ_ERR_CODE ret;

	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	*target = fj_iq_calibrationgettarget();
	//} else {
	//	*target = E_IQ_CALIB_TGT_ERR;
	//}
}

VOID FJ_IQ_CalibrationSetOperate( E_IQ_CALIB_OP operate )
{
	// delete_basefw\fj\iq
	//FJ_ERR_CODE ret;

	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	fj_iq_calibrationsetoperate( operate );
	//}
}

E_IQ_CALIB_OP FJ_IQ_CalibrationGetOperate( VOID )
{
	// delete_basefw\fj\iq
	//FJ_ERR_CODE ret;

	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	return fj_iq_calibrationgetoperate();
	//} else {
		return E_IQ_CALIB_OP_MAX;
	//}
}

VOID FJ_IQ_CalibrationSetBayer( const VOID* const bay_addr, const UINT32 bay_bytes, const UINT32 width, const UINT32 lines )
{
	// delete_basefw\fj\iq
	//FJ_ERR_CODE ret;

	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	fj_iq_calib_set_bay( bay_addr, bay_bytes, width, lines );
	//}
}

