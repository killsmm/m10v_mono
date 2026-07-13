/**
 * @file		fj_still.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "os_user_custom.h"
#include "fj_still.h"
#include "mode.h"
#include "still_basic.h"
// delete basefw\fj\imgproc\still
//#include "still_effect.h"
//#include "still_preview.h"
#include "still_utility.h"
// delete basefw\fj\imgproc\still
//#include "still_pro_ctrl.h"
// delete_basefw\fj\3a
//#include "3a.h"


/******************************************************************************
 Define
******************************************************************************/
#define MODE_CHECK_BOOT		(D_BF_MODE_BASE_CODE|D_BF_MODE_CATEGORY_ALL)
#define MODE_CHECK_VIEW		(D_BF_MODE_VIEW_CODE|D_BF_MODE_CATEGORY_VIEW)
#define MODE_CHECK_ALL		(D_BF_MODE_VIEW_CODE|D_BF_MODE_OTHER_CODE|D_BF_MODE_CATEGORY_ALL)
#define MODE_CHECK_MOVIE	(D_BF_MODE_VIEW_CODE|(D_BF_MODE_CATEGORY_ALL & (~D_BF_MODE_CATEGORY_CAPTURE_AVI)))
#define MODE_CHECK_CAPTURE	(D_BF_MODE_VIEW_CODE|D_BF_MODE_CATEGORY_CAPTURE_SEQ|D_BF_MODE_CATEGORY_CAPTURE)

//#define CO_FJ_STILL_API_CHECK_ENABLE
#ifdef CO_FJ_STILL_API_CHECK_ENABLE
#include "mba_if.h"
#define M_STILL_MBA_PRINTF(arg)						(M_MBA_PRINTF (arg))
#else
#define M_STILL_MBA_PRINTF(arg)
#endif
/******************************************************************************
 Program
******************************************************************************/
/**********************************
 Basic APIs
**********************************/
FJ_ERR_CODE FJ_ConfigCapture( UCHAR bConfig, USHORT wOperand )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if(ret == FJ_ERR_OK) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ConfigCapture" ));
	//	ret = fj_configcapture(bConfig, wOperand);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SaveRawVideo( UCHAR output_part, UCHAR instance_id, UCHAR enable )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//ret = BF_Mode_Check(MODE_CHECK_VIEW);
	//if( ret == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SaveRawVideo" ));
	//	fj_saverawvideo(instance_id, enable);
	//}

	return ret;
}

FJ_ERR_CODE FJ_SetSg( UCHAR sen_id, FJ_SG_CTRL* sg_ctrl )
{
	FJ_ERR_CODE	ret = FJ_ERR_NG;

	if( (sg_ctrl != NULL) && (BF_Mode_Check(MODE_CHECK_ALL) == FJ_ERR_OK) ){
		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetSg" ));
		fj_set_sg( sen_id, sg_ctrl );
		ret = FJ_ERR_OK;
	}

	return ret;
}

FJ_ERR_CODE FJ_SgStart( UCHAR sen_id )
{
	FJ_ERR_CODE	ret = FJ_ERR_NG;

	if(BF_Mode_Check(MODE_CHECK_ALL) == FJ_ERR_OK) {
		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SgStart" ));
		fj_sg_start(sen_id);
		ret = FJ_ERR_OK;
	}

	return ret;
}

FJ_ERR_CODE FJ_SgStop( UCHAR sen_id, UCHAR stop_mode )
{
	FJ_ERR_CODE	ret = FJ_ERR_NG;

	if(BF_Mode_Check(MODE_CHECK_ALL) == FJ_ERR_OK) {
		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SgStop" ));
		fj_sg_stop( sen_id, stop_mode );
		ret = FJ_ERR_OK;
	}

	return ret;
}

FJ_ERR_CODE FJ_GetNonLinerHistogram( UCHAR demosaic_id, USHORT* pHistogramBuff )
{
	// delete basefw\fj\imgproc\still
	//M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetNonLinerHistogram" ));
	//return fj_getnonlinerhistogram(demosaic_id, pHistogramBuff);
	return FJ_ERR_OK;
}

FJ_ERR_CODE FJ_Still_Set_Adaptive_Ae_Param( const FJ_STILL_ADAPTIVE_AE_PARAM* const param )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_still_set_adaptive_ae_param(param);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Sensor_Open( FJ_SENSOR_ENUM sensor_enum, UCHAR* sensor_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Sensor_Open" ));
	//	ret = fj_sensor_open(sensor_enum, sensor_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Sen_Open( FJ_SEN_ENUM sen_enum, UCHAR* sen_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Sen_Open" ));
	//	ret = fj_sen_open(sen_enum, sen_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Sen_Close( UCHAR sen_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Sen_Close" ));
	//	ret = fj_sen_close(sen_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Sen_Init( FJ_SEN_ENUM sen_enum )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Sen_Init" ));
	//	ret = fj_sen_init(sen_enum);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Sen_Suspend( UCHAR sen_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = fj_sen_control_sensorclk(0);

	return ret;
}

FJ_ERR_CODE FJ_Sen_Resume( UCHAR sen_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = fj_sen_control_sensorclk(1);

	return ret;
}

FJ_ERR_CODE FJ_Demosaic_Open( FJ_DEMOSAIC_ENUM demosaic_enum, UCHAR* demosaic_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Demosaic_Open" ));
	//	ret = fj_demosaic_open(demosaic_enum, demosaic_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Demosaic_Close( UCHAR demosaic_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Demosaic_Close" ));
	//	ret = fj_demosaic_close(demosaic_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Demosaic_Init( FJ_DEMOSAIC_ENUM demosaic_enum )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Demosaic_Init" ));
	//	ret = fj_demosaic_init(demosaic_enum);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Demosaic_Suspend( UCHAR demosaic_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Demosaic_Suspend" ));
	//	ret = fj_demosaic_suspend(demosaic_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Demosaic_Resume( UCHAR demosaic_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Demosaic_Resume" ));
	//	ret = fj_demosaic_resume(demosaic_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Stitch_Open( FJ_STITCH_ENUM stitch_enum, UCHAR* stitch_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Stitch_Open" ));
	//	ret = fj_stitch_open(stitch_enum, stitch_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Stitch_Close( UCHAR stitch_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Stitch_Close" ));
	//	ret = fj_stitch_close(stitch_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Stitch_Init( FJ_STITCH_ENUM stitch_enum )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Stitch_Init" ));
	//	ret = fj_stitch_init(stitch_enum);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Stitch_Suspend( UCHAR stitch_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Stitch_Suspend" ));
	//	ret = fj_stitch_suspend(stitch_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Stitch_Resume( UCHAR stitch_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Stitch_Resume" ));
	//	ret = fj_stitch_resume(stitch_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_SetImageMacroMode( FJ_STILL_IMAGE_MACRO_MODE macro_mode )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetImageMacroMode" ));
	//	ret = fj_setimagemacromode(macro_mode);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Sensor_Connect( UCHAR sensor_id, UCHAR sen_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Sensor_Connect" ));
	//	ret = fj_sensor_connect(sensor_id, sen_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Sen_Connect( UCHAR sen_id, UCHAR demosaic_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Sen_Connect" ));
	//	ret = fj_sen_connect(sen_id, demosaic_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Demosaic_Connect_Stitch( UCHAR demosaic_id, UCHAR output_id, UCHAR stitch_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Demosaic_Connect_Stitch" ));
	//	ret = fj_demosaic_connect_stitch(demosaic_id, output_id, stitch_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Demosaic_Connect_Encode( UCHAR demosaic_id, UCHAR output_id, UCHAR encode_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Demosaic_Connect_Encode" ));
	//	ret = fj_demosaic_connect_encode(demosaic_id, output_id, encode_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Demosaic_Connect_Display( UCHAR demosaic_id, UCHAR output_id, UCHAR disp_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Demosaic_Connect_Display" ));
	//	ret = fj_demosaic_connect_display(demosaic_id, output_id, disp_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Stitch_Connect_Encode( UCHAR stitch_id, UCHAR video_encode_id1, UCHAR video_encode_id2, UCHAR video_encode_id3, UCHAR still_encode_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Stitch_Connect_Encode" ));
	//	ret = fj_stitch_connect_encode(stitch_id, video_encode_id1, video_encode_id2, video_encode_id3, still_encode_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Stitch_Connect_Display( UCHAR stitch_id, UCHAR disp_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Stitch_Connect_Display" ));
	//	ret = fj_stitch_connect_display(stitch_id, disp_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Sensor_Disconnect( UCHAR sensor_id, UCHAR sen_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Sensor_Disconnect" ));
	//	ret = fj_sensor_disconnect(sensor_id, sen_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Sen_Disconnect( UCHAR sen_id, UCHAR demosaic_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Sen_Disconnect" ));
	//	ret = fj_sen_disconnect(sen_id, demosaic_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Demosaic_Disconnect_Stitch( UCHAR demosaic_id, UCHAR output_id, UCHAR stitch_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Demosaic_Disconnect_Stitch" ));
	//	ret = fj_demosaic_disconnect_stitch(demosaic_id, output_id, stitch_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Sen_Disconnect_Display( UCHAR sen_id, UCHAR disp_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Sen_Disconnect_Display" ));
	//	ret = fj_sen_disconnect_display(sen_id, disp_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Demosaic_Disconnect_Encode( UCHAR demosaic_id, UCHAR output_id, UCHAR encode_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Demosaic_Disconnect_Encode" ));
	//	ret = fj_demosaic_disconnect_encode(demosaic_id, output_id, encode_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Demosaic_Disconnect_Display( UCHAR demosaic_id, UCHAR output_id, UCHAR disp_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Demosaic_Disconnect_Display" ));
	//	ret = fj_demosaic_disconnect_display(demosaic_id, output_id, disp_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Stitch_Disconnect_Encode( UCHAR stitch_id, UCHAR video_encode_id1, UCHAR video_encode_id2, UCHAR video_encode_id3, UCHAR still_encode_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Stitch_Disconnect_Encode" ));
	//	ret = fj_stitch_disconnect_encode(stitch_id, video_encode_id1, video_encode_id2, video_encode_id3, still_encode_id);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Stitch_Disconnect_Display( UCHAR stitch_id, UCHAR disp_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Stitch_Disconnect_Display" ));
	//	ret = fj_stitch_disconnect_display(stitch_id, disp_id);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Set_Sensor_Window( UCHAR sensor_id, FJ_SENSOR_WINDOW_CONFIG* sensor_info )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Set_Sensor_Window" ));
	//	ret = fj_set_sensor_window(sensor_id, sensor_info);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Get_Sensor_Window( UCHAR sensor_id, FJ_SENSOR_WINDOW_CONFIG* sensor_info )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Get_Sensor_Window" ));
	//	ret = fj_get_sensor_window(sensor_id, sensor_info);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Set_Sen_Window( UCHAR sen_id, FJ_SEN_WINDOW_CONFIG* window_config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Set_Sen_Window" ));
	//	ret = fj_set_sen_window(sen_id, window_config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Get_Sen_Window( UCHAR sen_id, FJ_SEN_WINDOW_CONFIG* window_config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Get_Sen_Window" ));
	//	ret = fj_get_sen_window(sen_id, window_config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Set_Demosaic_Window( UCHAR demosaic_id, FJ_DEMOSAIC_WINDOW_CONFIG* window_config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Set_Demosaic_Window" ));
	//	ret = fj_set_demosaic_window(demosaic_id, window_config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Get_Demosaic_Window( UCHAR demosaic_id, FJ_DEMOSAIC_WINDOW_CONFIG* window_config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Get_Demosaic_Window" ));
	//	ret = fj_get_demosaic_window(demosaic_id, window_config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Set_Stitch_Window( UCHAR stitch_id, FJ_STITCH_WINDOW_CONFIG* window_config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Set_Stitch_Window" ));
	//	ret = fj_set_stitch_window(stitch_id, window_config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Get_Stitch_Window( UCHAR stitch_id, FJ_STITCH_WINDOW_CONFIG* window_config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Get_Stitch_Window" ));
	//	ret = fj_get_stitch_window(stitch_id, window_config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Set_Encode_Window( UCHAR encode_id, FJ_ENCODE_WINDOW_CONFIG* window_config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Set_Encode_Window" ));
	//	ret = fj_set_encode_window(encode_id, window_config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Get_Encode_Window( UCHAR encode_id, FJ_ENCODE_WINDOW_CONFIG* window_config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Get_Encode_Window" ));
	//	ret = fj_get_encode_window(encode_id, window_config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaicInputControl( UCHAR demosaic_id, UCHAR row_division_num, UCHAR column_division_num )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetDemosaicInputControl" ));
	//	ret = fj_setdemosaicinputcontrol(demosaic_id, row_division_num, column_division_num);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetDemosaicInputControl( UCHAR demosaic_id, UCHAR* row_division_num, UCHAR* column_division_num )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetDemosaicInputControl" ));
	//	ret = fj_getdemosaicinputcontrol(demosaic_id, row_division_num, column_division_num);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaicOutputControl( UCHAR demosaic_id, UCHAR output_id, FJ_DEMOSAIC_OUTPUT initiate_cycle, UINT32 sampling_count, FJ_DEMOSAIC_OUTPUT_IMG_PFMT image_format )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetDemosaicOutputControl" ));
	//	ret = fj_setdemosaicoutputcontrol(demosaic_id, output_id, initiate_cycle, sampling_count, image_format);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetDemosaicOutputControl( UCHAR demosaic_id, UCHAR output_id, FJ_DEMOSAIC_OUTPUT* initiate_cycle, UINT32* sampling_count, FJ_DEMOSAIC_OUTPUT_IMG_PFMT* image_format )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetDemosaicOutputControl" ));
	//	ret = fj_getdemosaicoutputcontrol(demosaic_id, output_id, initiate_cycle, sampling_count, image_format);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaic3dnrEnabled( UCHAR demosaic_id, BOOL enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetDemosaic3dnrEnabled" ));
	//	ret = fj_setdemosaic3dnrenabled(demosaic_id, enabled);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetDemosaic3dnrEnabled( UCHAR demosaic_id, BOOL* enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetDemosaic3dnrEnabled" ));
	//	ret = fj_getdemosaic3dnrenabled(demosaic_id, enabled);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaic3dnrConfig( UCHAR demosaic_id, FJ_STILL_3DNR_CONFIG* config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetDemosaic3dnrConfig" ));
	//	ret = fj_setdemosaic3dnrconfig(demosaic_id, config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetDemosaic3dnrConfig( UCHAR demosaic_id, FJ_STILL_3DNR_CONFIG* config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetDemosaic3dnrConfig" ));
	//	ret = fj_getdemosaic3dnrconfig(demosaic_id, config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaicLtmEnabled( UCHAR demosaic_id, BOOL enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetDemosaicLtmEnabled" ));
	//	ret = fj_setdemosaicltmenabled(demosaic_id, enabled);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetDemosaicLtmEnabled( UCHAR demosaic_id, BOOL* enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetDemosaicLtmEnabled" ));
	//	ret = fj_getdemosaicltmenabled(demosaic_id, enabled);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaicShdrEnabled( UCHAR demosaic_id, BOOL enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetDemosaicShdrEnabled" ));
	//	ret = fj_setdemosaicshdrenabled(demosaic_id, enabled);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetDemosaicShdrEnabled( UCHAR demosaic_id, BOOL* enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetDemosaicShdrEnabled" ));
	//	ret = fj_getdemosaicshdrenabled(demosaic_id, enabled);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaicShdrConfig( UCHAR demosaic_id, FJ_STILL_SHDR_CONFIG* config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetDemosaicShdrConfig" ));
	//	ret = fj_setdemosaicshdrconfig(demosaic_id, config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetDemosaicShdrConfig( UCHAR demosaic_id, FJ_STILL_SHDR_CONFIG* config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetDemosaicShdrConfig" ));
	//	ret = fj_getdemosaicshdrconfig(demosaic_id, config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaicAhipEnabled( UCHAR demosaic_id, BOOL enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetDemosaicAhipEnabled" ));
	//	ret = fj_setdemosaicahipenabled(demosaic_id, enabled);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetDemosaicAhipEnabled( UCHAR demosaic_id, BOOL* enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetDemosaicAhipEnabled" ));
	//	ret = fj_getdemosaicahipenabled(demosaic_id, enabled);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaicWarpEnabled( UCHAR demosaic_id, BOOL enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetDemosaicWarpEnabled" ));
	//	ret = fj_setdemosaicwarpenabled(demosaic_id, enabled);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetDemosaicWarpEnabled( UCHAR demosaic_id, BOOL* enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetDemosaicWarpEnabled" ));
	//	ret = fj_getdemosaicwarpenabled(demosaic_id, enabled);
	//}

	return ret;
}

FJ_ERR_CODE FJ_Notify_Loading_WarpTable_Complete( UINT32 tbl_idx, const BYTE* addr, ULONG bytes )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "WarpLoad %u", tbl_idx ));
	//	ret = fj_notify_loading_warptable_complete( tbl_idx, addr, bytes );
	//}

	return ret;
}

FJ_ERR_CODE FJ_Warp_Calc_GeTable( UINT32 tbl_idx, FJ_GE_TBL_INFO* ge_tbl_info, const FJ_WARP_CALC_PARAM* calc_param )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "WarpLoad %u", tbl_idx ));
	//	ret = fj_warp_calc_getable( tbl_idx, ge_tbl_info, calc_param );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaicRawOutPart( UCHAR demosaic_id, UCHAR raw_out_part )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "RawOutPart %u", tbl_idx ));
		// TODO
	}

	return ret;
}

FJ_ERR_CODE	FJ_GetDemosaicRawOutPart( UCHAR demosaic_id, UCHAR* raw_out_part )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "RawOutPart %u", tbl_idx ));
		// TODO
	}

	return ret;
}

FJ_ERR_CODE	FJ_SetSenConfig( UCHAR sen_id, FJ_STILL_SEN_CONFIG* sen_config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetSenConfig" ));
	//	ret = fj_setsenconfig(sen_id, sen_config);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetSenConfig( UCHAR sen_id, FJ_STILL_SEN_CONFIG* sen_config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetSenConfig" ));
	//	ret = fj_getsenconfig(sen_id, sen_config);
	//}

	return ret;
}

FJ_ERR_CODE FJ_SetJpegEncodeQuality( UCHAR encode_id, FJ_JPEG_QUALITY_KIND bQuality, USHORT wOperand )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if(ret == FJ_ERR_OK) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetJpegEncodeQuality" ));
	//	ret = fj_setjpegencodequality(encode_id, bQuality, wOperand);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetJpegEncodeQuality( UCHAR encode_id, FJ_JPEG_QUALITY_KIND* bQuality, USHORT* wOperand )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetJpegEncodeQuality" ));
	//	ret = fj_getjpegencodequality(encode_id, bQuality, wOperand);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_LoadLtmMap( UCHAR demosaic_id, FJ_LTM_MAP_CTRL map_ctrl, UINT32 map_addr )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "LoadLtmMap" ));
	//	ret = fj_loadltmmap(demosaic_id, map_ctrl, map_addr);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GenerateJpegShdr( UCHAR demosaic_id, BOOL normal_img_jpeg_onoff )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GenerateJpegShdr" ));
	//	ret = fj_generatejpegshdr(demosaic_id, normal_img_jpeg_onoff);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Reconfigure_Sensor( UCHAR sen_id, UINT32 pre_frame_rate, UINT32 post_frame_rate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "Reconfigure_Sensor" ));
	//	ret = fj_reconfigure_sensor(sen_id, pre_frame_rate, post_frame_rate);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaicFlipMirrorEnabled( UCHAR demosaic_id, BOOL flip_enabled, BOOL mirror_enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "SetDemosaicFlipMirrorEnabled" ));
	//	ret = fj_setdemosaicflipmirrorenabled(demosaic_id, flip_enabled, mirror_enabled);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_GetDemosaicFlipMirrorEnabled( UCHAR demosaic_id, BOOL* flip_enabled, BOOL* mirror_enabled )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "GetDemosaicFlipMirrorEnabled" ));
		ret = fj_getdemosaicflipmirrorenabled(demosaic_id, flip_enabled, mirror_enabled);
	}

	return ret;
}

FJ_ERR_CODE	FJ_Still_GetBeforeTCHistogram( UCHAR demosaic_id, E_FJ_STILL_BTC_HIST_TBL data_type, USHORT* hist_buffer, UCHAR* overflow_flg )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_still_getbeforetchistogram( demosaic_id, data_type, hist_buffer, overflow_flg );
	//}
	return ret;
}

FJ_ERR_CODE	FJ_Still_GetBeforeTCLuminanceEvaluation( UCHAR demosaic_id, UINT32* luminance_buffer )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_still_getbeforetcluminanceevaluation( demosaic_id, luminance_buffer );
	//}
	return ret;
}

FJ_ERR_CODE	FJ_Still_SetToneTable( UCHAR demosaic_id, USHORT* tone_tbl, E_FJ_STILL_TC_REF_RESOLUTION table_resol, USHORT table_end_point )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_still_settonetable( demosaic_id, tone_tbl, table_resol, table_end_point );
	//}
	return ret;
}

FJ_ERR_CODE	FJ_Still_SetRgbOffset( UCHAR demosaic_id, T_FJ_STILL_RGB_OFFSET* rgb_offset )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_still_setrgboffset( demosaic_id, rgb_offset );
	//}
	return ret;
}

FJ_ERR_CODE	FJ_Still_SetTctParam( UCHAR demosaic_id, T_FJ_STILL_TCT_PARAM* tct_param )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_still_settctparam( demosaic_id, tct_param );
	//}
	return ret;
}

FJ_ERR_CODE	FJ_Still_SetTchsParam( UCHAR demosaic_id, T_FJ_STILL_TCHS_PARAM* tchs_param )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_still_settchsparam( demosaic_id, tchs_param );
	//}
	return ret;
}


FJ_ERR_CODE	FJ_Still_SetToneParam( UCHAR demosaic_id, T_FJ_STILL_TONE_PARAM* tone_param )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_still_settoneparam( demosaic_id, tone_param );
	//}
	return ret;
}

FJ_ERR_CODE	FJ_Still_GetTctTchsGlobalWindowSize( UCHAR demosaic_id, USHORT* global_width, USHORT* global_lines )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_still_gettcttchsglobalwindowsize( demosaic_id, global_width, global_lines );
	//}
	return ret;
}

FJ_ERR_CODE	FJ_SetDemosaicRawScalerEnabled( UCHAR demosaic_id, FJ_STILL_RAW_SCALER_SEL scaler_sel )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_setdemosaicrawscalerenabled( demosaic_id, scaler_sel );
	//}
	return ret;
}


FJ_ERR_CODE FJ_GetDemosaicRawScalerEnabled( UCHAR demosaic_id, FJ_STILL_RAW_SCALER_SEL* scaler_sel )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_getdemosaicrawscalerenabled(demosaic_id, scaler_sel);
	//}

	return ret;
}

FJ_ERR_CODE FJ_GetRawWindowConfig ( UCHAR sen_id, FJ_STILL_RAW_WINDOW_CONFIG* raw_window_cfg )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_getrawwindowconfig(sen_id, raw_window_cfg);
	//}

	return ret;
}

FJ_ERR_CODE	FJ_Still_GetReadoutTimestamp( UCHAR sen_id, UINT64 frame_no, FJ_STILL_READOUT_TIMESTAMP* readout_timestamp )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_still_getreadouttimestamp(sen_id, frame_no, readout_timestamp);
	//}

	return ret;
}

FJ_ERR_CODE FJ_SetStatisticPeriod( UCHAR inst_id, FJ_STAT_LOCATION_TYPE stat_locate, Statistic_Info SchData )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_setstatisticperiod(inst_id, stat_locate, SchData);
	//}

	return ret;
}

FJ_ERR_CODE FJ_SetStatAeAwbConfig( UCHAR aeawb_id, UCHAR inst_id, FJ_STAT_LOCATION_TYPE stat_locate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_setstataeawbconfig( aeawb_id, inst_id, stat_locate );
	//}
	return ret;
}

FJ_ERR_CODE FJ_GetStatAeAwbConfig( UCHAR aeawb_id, UCHAR* inst_id, FJ_STAT_LOCATION_TYPE* stat_locate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_getstataeawbconfig( aeawb_id, inst_id, stat_locate );
	//}
	return ret;
}

FJ_ERR_CODE FJ_SetStatAfConfig( UCHAR af_id, UCHAR inst_id, FJ_STAT_LOCATION_TYPE stat_locate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_setstatafconfig( af_id, inst_id, stat_locate );
	//}
	return ret;
}

FJ_ERR_CODE FJ_GetStatAfConfig( UCHAR af_id, UCHAR* inst_id, FJ_STAT_LOCATION_TYPE* stat_locate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_getstatafconfig( af_id, inst_id, stat_locate );
	//}
	return ret;
}

FJ_ERR_CODE FJ_SetStatFlckConfig( UCHAR flck_id, UCHAR inst_id, FJ_STAT_LOCATION_TYPE stat_locate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_setstatflckconfig( flck_id, inst_id, stat_locate );
	//}
	return ret;
}

FJ_ERR_CODE FJ_GetStatFlckConfig( UCHAR flck_id, FJ_STAT_LOCATION_TYPE* stat_locate, UCHAR* inst_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_getstatflckconfig( flck_id, stat_locate, inst_id );
	//}
	return ret;
}

FJ_ERR_CODE FJ_SetStatHistConfig( UCHAR hist_id, UCHAR inst_id, FJ_STAT_LOCATION_TYPE stat_locate )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_setstathistconfig( hist_id, inst_id, stat_locate );
	//}
	return ret;
}

FJ_ERR_CODE FJ_GetStatHistConfig( UCHAR hist_id, FJ_STAT_LOCATION_TYPE* stat_locate, UCHAR* inst_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\3a
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_getstathistconfig( hist_id, stat_locate, inst_id );
	//}
	return ret;
}


/**********************************
 Utility APIs
**********************************/
FJ_ERR_CODE FJ_ImageSPF( FJ_IMG_SPF_CTRL* spf_ctrl_info )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if(ret == FJ_ERR_OK) {
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageSPF" ));
	//	ret = fj_imagespf( spf_ctrl_info );
	//}

	return ret;
}

FJ_ERR_CODE FJ_ImageCopy( UINT32 src_addr, UINT32 dst_addr, UINT32 width, UINT32 lines )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if( ret == FJ_ERR_OK ){
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageCopy" ));
	//	ret = fj_imagecopy( src_addr, dst_addr, width, lines );
	//}

	return ret;
}

FJ_ERR_CODE FJ_ImageCopyYcc( FJ_IMG_INFO* src, FJ_IMG_INFO* dst )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if( ret == FJ_ERR_OK ){
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageCopyYcc" ));
	//	ret = fj_imagecopyycc( src, dst );
	//}

	return ret;
}

FJ_ERR_CODE FJ_ImageConvert( FJ_IMG_CONV_CTRL* conv_info )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check(MODE_CHECK_ALL);
	if( ret == FJ_ERR_OK ){
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageConvert" ));
		ret = fj_imageconvert( conv_info );
	}

	return ret;
}

FJ_ERR_CODE FJ_ImageFormatConvert( FJ_IMG_INFO* src, FJ_IMG_INFO* dst, UCHAR planer_select )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if( ret == FJ_ERR_OK ){
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageFmtConvert" ));
	//	ret = fj_imageformatconvert( src, dst, planer_select );
	//}

	return ret;
}

FJ_ERR_CODE FJ_ImageCrop( FJ_IMG_CROP_CTRL* crop_info )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check(MODE_CHECK_ALL);
	if( ret == FJ_ERR_OK ){
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageCrop" ));
		ret = fj_imagecrop( crop_info );
	}

	return ret;
}

FJ_ERR_CODE FJ_ImageCropSplitWithDisplay( FJ_IMG_CROP_CTRL_SPLIT* crop_info )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if( ret == FJ_ERR_OK ){
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageCropSplitWithDisplay" ));
	//	ret = fj_imagecropsplitwithdisplay( crop_info );
	//}

	return ret;
}

FJ_ERR_CODE FJ_ImageFillVideo( FJ_IMG_FILL_CTRL* fill_info )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if(ret == FJ_ERR_OK) {
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageFillH264" ));
	//	ret = fj_imagefillvideo( fill_info );
	//}

	return ret;
}

FJ_ERR_CODE FJ_ImageFreeRect( FJ_IMG_FRECT_CTRL* frect_ctrl )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//ret = BF_Mode_Check(MODE_CHECK_ALL);
	//if(ret == FJ_ERR_OK) {
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageFreeRect" ));
	//	ret = fj_imagefreerect( frect_ctrl );
	//}

	return ret;
}

/**
 * @brief 		H/W support OSD Blend function.
*/
FJ_ERR_CODE FJ_ImageBlend( FJ_IMG_BLEND_CTRL* osd_blend_info )
{
	FJ_ERR_CODE fj_ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check(MODE_CHECK_ALL) == FJ_ERR_OK ){
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageBlend" ));
	//	fj_ret = fj_imageblend( osd_blend_info );
	//}
	return fj_ret;
}

/**
 * @brief 		Draw "Face-Frame" function.
*/
FJ_ERR_CODE FJ_ImageDrawFaceFrame( FJ_IMG_INFO* src_frame, T_FACE_FRAME_EACH_SET* face_pos_array, T_FACE_FRAME_SET* frame_set )
{
	FJ_ERR_CODE fj_ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check(MODE_CHECK_ALL) == FJ_ERR_OK ){
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageDrawFaceFrame" ));
	//	fj_ret = fj_imagedrawfaceframe( src_frame, face_pos_array, frame_set );
	//}
	return fj_ret;
}

/**
 * @brief 		Draw "Face-Rect" function.
*/
FJ_ERR_CODE FJ_ImageDrawRect( FJ_IMG_INFO* src_frame, FJ_RECT_FRAME_SET* frame_set )
{
	FJ_ERR_CODE fj_ret = FJ_ERR_NG;

	// delete basefw\fj\imgproc\still
	//if( BF_Mode_Check(MODE_CHECK_ALL) == FJ_ERR_OK ){
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageDrawRect" ));
	//	fj_ret = fj_imagedrawrect( src_frame, frame_set );
	//}
	return fj_ret;
}

FJ_ERR_CODE FJ_ImageJpegEncode( FJ_IMG_JPEG_CTRL* enc_info )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
//		M_STILL_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "ImageJpegEncode" ));
		ret = fj_imagejpegencode(enc_info);
	}

	return ret;
}

FJ_ERR_CODE FJ_Still_Get_ObWaveDetectValue( const UCHAR sen_id, FJ_STILL_OB_VALUE* const obdata )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_still_get_obwavedetectvalue( sen_id, obdata );
	//}
	return ret;
}

FJ_ERR_CODE FJ_ImageFill( FJ_IMG_FILL_INFO* fill_info )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_imagefill( fill_info );
	//}
	return ret;
}

FJ_ERR_CODE FJ_IsBufferFillAvailable( VOID )
{
	FJ_ERR_CODE ret = FJ_ERR_NG_CONDITION;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_isbufferfillavailable();
	//}
	return ret;
}

FJ_ERR_CODE FJ_ConfigGyro( FJ_GYRO_CONFIG* gyro_config )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
		ret = fj_configgyro( gyro_config );
	}
	return ret;
}

FJ_ERR_CODE FJ_AddGyroData( ULLONG frame_no, FJ_GYRO_DATA* gyro_data )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
		ret = fj_addgyrodata( frame_no, gyro_data );
	}
	return ret;
}

FJ_ERR_CODE FJ_Set_Capture_Stitch_Status( FJ_STILL_STITCH_TYPE stitch_type )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete\basefw\fj\imgproc\still
	//if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
	//	ret = fj_set_capture_stitch_status( stitch_type );
	//}
	return ret;
}

FJ_ERR_CODE FJ_Get_Capture_Stitch_Status( FJ_STILL_STITCH_TYPE * stitch_type )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	if( BF_Mode_Check( MODE_CHECK_ALL ) == FJ_ERR_OK ) {
		ret = fj_get_capture_stitch_status( stitch_type );
	}
	return ret;
}

