/**
 * @file		iq_calib.c
 * @brief		Algorithm of Calibration API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_iq.h"

#include "fj_filesystem.h"

#include "iq_dpc_detect_info.h"
#include "iq_calib.h"

// System header
#include "instance.h"

#include "debug.h"


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
// nothing special.


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
/** Calibration - Current farget */
static volatile E_IQ_CALIB_TGT			gCalibration_CurrentTarget = E_IQ_CALIB_TGT_INIT;
/** Calibration - Current pixel format */
static volatile E_IQ_CALIB_FMT_PIX		gCalibration_CurrentFormatPix = E_IQ_CALIB_FMT_PIX_16NONPACK;
/** Calibration - Raw Save execute flag */
static volatile BOOL					gCalibration_RawSaveToMedia = FALSE;
/** Calibration - Jpeg Save execute flag */
static volatile BOOL					gCalibration_JpegSaveToMedia = FALSE;
/** Calibration - common parameter */
static T_BF_IQ_CALIB_INFO	gCalibration_Info[D_BF_INSTANCE_ID_MAX_SEN] = {
	{
		.pv_bayer16_addr = NULL,
		.ui_bayer_bytelen = 0,
		.us_global_h_pixels = 0,
		.us_global_w_pixels = 0,
		.us_vd_ofs = 0,
		.us_hd_ofs = 0,
		.t_top_color = E_IQ_BAYER_COLOR_R,
		.still_mode = E_IQ_STILL_MODE_IDLE,
		.us_field_num = 0,
	},
};
/** Calibration - Current operate */
static volatile E_IQ_CALIB_OP			gCalibration_CurrentOperate = E_IQ_CALIB_OP_MAX;


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

/** @brief		FJ_IQ_CalibrationSetFormatPix
 * @param[in]	e_format_pix	pixel format
 * @return		None
 * @note		None
 * @attention	None
 */
VOID fj_iq_calibrationsetformatpix( const E_IQ_CALIB_FMT_PIX e_format_pix )
{
	if( e_format_pix >= E_IQ_CALIB_FMT_PIX_MAX ) {
		return;
	}

	gCalibration_CurrentFormatPix = e_format_pix;
}


/** @brief		FJ_IQ_CalibrationGetFormatPix
 * @return		pixel format
 * @note		None
 * @attention	None
 */
E_IQ_CALIB_FMT_PIX fj_iq_calibrationgetformatpix( VOID )
{
	return gCalibration_CurrentFormatPix;
}


/** @brief		FJ_IQ_CalibrationSetRawSaveToMedia
 * @param[in]	e_save	save execute flag
 * @return		None
 * @note		None
 * @attention	None
 */
VOID fj_iq_calibrationsetrawsavetomedia( const BOOL e_save )
{
	gCalibration_RawSaveToMedia = ((e_save != FALSE)?(TRUE):(FALSE));
}


/** @brief		FJ_IQ_CalibrationGetRawSaveToMedia
 * @return		save execute flag
 * @note		None
 * @attention	None
 */
BOOL fj_iq_calibrationgetrawsavetomedia( VOID )
{
	return gCalibration_RawSaveToMedia;
}


/** @brief		FJ_IQ_CalibrationSetJpegSaveToMedia
 * @param[in]	e_save	save execute flag
 * @return		None
 * @note		None
 * @attention	None
 */
VOID fj_iq_calibrationsetjpegsavetomedia( const BOOL e_save )
{
	gCalibration_JpegSaveToMedia = ((e_save != FALSE)?(TRUE):(FALSE));
}


/** @brief		FJ_IQ_CalibrationGetJpegSaveToMedia
 * @return		save execute flag
 * @note		None
 * @attention	None
 */
BOOL fj_iq_calibrationgetjpegsavetomedia( VOID )
{
	return gCalibration_JpegSaveToMedia;
}


/** @brief		FJ_IQ_CalibrationSetTarget
 * @param[in]	e_target	Calibration target(view/movie/capture)
 * @return		None
 * @note		None
 * @attention	None
 */
VOID fj_iq_calibrationsettarget( const E_IQ_CALIB_TGT e_target )
{
	if( (e_target <= E_IQ_CALIB_TGT_INIT)
	 || (e_target >= E_IQ_CALIB_TGT_MAX)
	 ) {
		return;
	}

	gCalibration_CurrentTarget = e_target;
}


/** @brief		FJ_IQ_CalibrationGetTarget
 * @return		Calibration target(view/movie/capture)
 * @return		None
 * @note		None
 * @attention	None
 */
E_IQ_CALIB_TGT fj_iq_calibrationgettarget( VOID )
{
	return gCalibration_CurrentTarget;
}


/** @brief		FJ_IQ_CalibrationSetOperate
 * @param[in]	e_operate	Calibration operate
 * @return		None
 * @note		None
 * @attention	None
 */
VOID fj_iq_calibrationsetoperate( const E_IQ_CALIB_OP e_operate )
{
 	if(e_operate >= E_IQ_CALIB_OP_MAX) {
		return;
	}

	gCalibration_CurrentOperate = e_operate;
}


/** @brief		FJ_IQ_CalibrationGetOperate
 * @return		Calibration Operate
 * @return		None
 * @note		None
 * @attention	None
 */
E_IQ_CALIB_OP fj_iq_calibrationgetoperate( VOID )
{
	return gCalibration_CurrentOperate;
}


/** @brief		FJ_IQ_CalibrationGetBayerInfo<br>
 * 				return bayer information for Calibration
 * @param[out]	ppv_top_addr			bayer address(16bit unpack/12bit pack)
 * @param[out]	pus_global_height_pixs	bayer height(pixel num)
 * @param[out]	pus_global_width_pixs	bayer width(pixel num)
 * @retval		TRUE	success
 * @retval		FALSE	parameter error
 * @note		None
 * @attention	None
 */
BOOL fj_iq_calibrationgetbayerinfo( VOID** const ppv_top_addr, USHORT* const pus_global_height_pixs, USHORT* const pus_global_width_pixs )
{
	if( ppv_top_addr == NULL ) {
		return FALSE;
	}
	if( pus_global_height_pixs == NULL ) {
		return FALSE;
	}
	if( pus_global_width_pixs == NULL ) {
		return FALSE;
	}

	*ppv_top_addr = (VOID*)gCalibration_Info[0].pv_bayer16_addr;
	*pus_global_height_pixs = gCalibration_Info[0].us_global_h_pixels;
	*pus_global_width_pixs = gCalibration_Info[0].us_global_w_pixels;

	return TRUE;
}


/** @brief		FJ_IQ_CalibrationSetBayer<br>
 * @param[in]	bay_addr			bayer address(16bit unpack/12bit pack)
 * @param[in]	bay_bytes	bayer bytes(pixel num)
 * @param[in]	width	bayer width(pixel num)
 * @param[in]	lines	bayer lines(pixel num)
 * @retval		None
 * @note		None
 * @attention	None
 */
VOID fj_iq_calib_set_bay( const VOID* const bay_addr, const UINT32 bay_bytes, const UINT32 width, const UINT32 lines )
{
	BF_IQ_Calib_Set_Bay(0, bay_addr, bay_bytes, width, lines);
}



/** @brief		IQ_LoadFile_BayerData
 *	@param[in]	puc_filepath	BayerData file path
 *	@retval		FJ_ERR_OK  success of file loading.
 *	@retval		FJ_ERR_NG  parameter errror.
 *	@note		None
 *	@attention	for FJ debug only
 */
FJ_ERR_CODE BF_IQ_Calib_Load_BayerFile( const UCHAR sen_id, const UCHAR* const puc_filepath )
{
	FJ_ERR_CODE fj_ercd;
	ULONG ul_fileId;
	ULONG ul_read_size;


	if( puc_filepath == NULL ) {
		return FJ_ERR_NG;
	}


	ul_fileId = FJ_Fs_Open( (CHAR*)puc_filepath, FJ_FS_OPEN_RDONLY );
	if( ul_fileId == 0 ) {
		return FJ_ERR_NG;
	}

	while( 1 ) {
		fj_ercd = FJ_Fs_Read( ul_fileId, (UINT32*)gCalibration_Info[sen_id].pv_bayer16_addr, ((ULONG)gCalibration_Info[sen_id].ui_bayer_bytelen), &ul_read_size );

		if((( fj_ercd != FJ_ERR_OK ) && ( fj_ercd != FJ_ERR_READ_EOF)) ||
			( ul_read_size != (ULONG)gCalibration_Info[sen_id].ui_bayer_bytelen )) {
			fj_ercd = FJ_ERR_NG;
#ifdef FJ_DEBUG_PRINT_ON
			BF_Debug_Print_Warning(( "%s: reading size != requested size. %s, %u\n",
				__func__, puc_filepath, (UINT32)ul_read_size ));
#endif
		}

		fj_ercd = FJ_ERR_OK;

		break;
	}

	FJ_Fs_Close( ul_fileId );

	return fj_ercd;
}


/** @brief			IQ_Calibration_SetParam
 *					setting calibration parameter
 *	@param[in]		pt_calib_param		detect parameters structure
 *	@retval			FJ_ERR_OK  success.
 *	@retval			FJ_ERR_NG  parameter errror.
 *	@note			None
 *	@attention		None
 */
FJ_ERR_CODE BF_IQ_Calibration_Set_Param( const UCHAR sen_id, const T_BF_IQ_CALIB_PARAM* const pt_calib_param )
{
	if( pt_calib_param == NULL ) {
		return FJ_ERR_NG;
	}

	gCalibration_Info[sen_id].pv_bayer16_addr = (VOID*)(pt_calib_param->pus_bayer16_addr);
	gCalibration_Info[sen_id].ui_bayer_bytelen = pt_calib_param->ui_bayer_bytelen;
	gCalibration_Info[sen_id].us_global_h_pixels = pt_calib_param->us_global_rows;
	gCalibration_Info[sen_id].us_global_w_pixels = pt_calib_param->us_global_columns;
	gCalibration_Info[sen_id].us_field_num = pt_calib_param->us_field_num;
	gCalibration_Info[sen_id].us_vd_ofs = pt_calib_param->us_vd_ofs;
	gCalibration_Info[sen_id].us_hd_ofs = pt_calib_param->us_hd_ofs;
	gCalibration_Info[sen_id].t_top_color = pt_calib_param->t_top_color;
	gCalibration_Info[sen_id].still_mode = pt_calib_param->still_mode;
	gCalibration_Info[sen_id].us_upper_trim = pt_calib_param->us_upper_trim;
	gCalibration_Info[sen_id].us_left_trim = pt_calib_param->us_left_trim;

	BF_IQ_Dpc_Detect_Info_Set_CalibParam( sen_id, pt_calib_param );

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_IQ_Calibration_Set_Ob( const UCHAR sen_id, const T_BF_IQ_CALIB_INFO_OB* const pt_ob )
{
	gCalibration_Info[sen_id].ob = *pt_ob;			// OB information.

	return FJ_ERR_OK;
}


T_BF_IQ_CALIB_INFO_OB* BF_IQ_Calibration_Get_Ob( const UCHAR sen_id )
{
	return &gCalibration_Info[sen_id].ob;
}


FJ_ERR_CODE BF_IQ_Calib_Set_Bay( const UCHAR sen_id, const VOID* const bay_addr, const UINT32 bay_bytes, const UINT32 width, const UINT32 lines )
{
	gCalibration_Info[sen_id].pv_bayer16_addr = bay_addr;
	gCalibration_Info[sen_id].ui_bayer_bytelen = bay_bytes;
	gCalibration_Info[sen_id].us_global_w_pixels = width;
	gCalibration_Info[sen_id].us_global_h_pixels = lines;

	BF_IQ_Dpc_Detect_Info_Set_Bay( sen_id, bay_addr, bay_bytes, width, lines );

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_IQ_Calib_Set_BayAddr( const UCHAR sen_id, const VOID* const bay_addr )
{
	gCalibration_Info[sen_id].pv_bayer16_addr = bay_addr;

	BF_IQ_Dpc_Detect_Info_Set_BayAddr( sen_id, bay_addr );

	return FJ_ERR_OK;
}


const T_BF_IQ_CALIB_INFO* BF_IQ_Calib_Get_Param( const UCHAR sen_id )
{
	return &gCalibration_Info[sen_id];
}


const CHAR* BF_IQ_Calib_Get_CalibTgtName( const E_IQ_CALIB_TGT e_target )
{
	static const CHAR* const calib_name[E_IQ_CALIB_TGT_MAX] = {
		[E_IQ_CALIB_TGT_INIT]			= "INIT",
		[E_IQ_CALIB_TGT_0]	= "TGT_0",
		[E_IQ_CALIB_TGT_1]	= "TGT_1",
		[E_IQ_CALIB_TGT_2]	= "TGT_2",
		[E_IQ_CALIB_TGT_3]	= "TGT_3",
		[E_IQ_CALIB_TGT_4]	= "TGT_4",
	};
	static const CHAR calib_name_unknown[] = "UNKNOWN";

	if( e_target == E_IQ_CALIB_TGT_ERR ) {
		return "ERR";
	}
	if( e_target >= E_IQ_CALIB_TGT_MAX ) {
		return calib_name_unknown;
	}

	if( calib_name[e_target] == NULL ) {
		return calib_name_unknown;
	}

	return calib_name[e_target];
}

