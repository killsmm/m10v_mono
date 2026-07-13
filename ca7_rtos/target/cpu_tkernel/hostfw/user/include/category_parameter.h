/**
 * @file		category_parameter.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _CATEGORY_PARAMETER_H_
#define _CATEGORY_PARAMETER_H_

#include "share.h"
#include "share_data.h"
#include "awb_task.h"
#include "ae_task.h"
#include "af_task.h"
#include "IQ_bin_para.h"
#include "user_utility_calib.h"
#include "os_if.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_CONFIG_MODE_DEBUG			0
#define D_CONFIG_MODE_VIEW			1
#define D_CONFIG_MODE_CAP			2
#define D_CONFIG_MODE_YUVIN_VIEW	3

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

VOID Cate_Init(VOID);
VOID Cate_Get_Size( E_SHARE_YUV_OUT yuv_out, ULONG * width, ULONG * lines );
VOID Cate_Get_Config( T_SHARE_SYSTEM_CONFIG* const config, UINT32 user_param );
VOID Cate_Get_MovieRec_Config( T_SHARE_MOVIE_REC_CONFIG* const config );
BOOL Cate_Get_MovieRec_H265_DynamicConfig( E_SHARE_YUV_OUT stream_id, T_FJ_MOVIE_H265_DYNAMIC_CONFIG_PARAM* h265_config );
BOOL Cate_Get_MovieRec_H264_DynamicConfig( E_SHARE_YUV_OUT stream_id, T_FJ_MOVIE_H264_DYNAMIC_CONFIG_PARAM* h264_config );
VOID Cate_Get_System_Param( T_SHARE_SYSTEM_PARAM* const param, UINT32 user_param );
VOID Cate_Get_AE_Param( T_AE_PARAM* const param, UINT32 user_param );
VOID Cate_Get_AWB_Param( T_AWB_PARAM* const param );
VOID Cate_Get_AF_Param( T_AF_PARAM* const param );
VOID Cate_Get_IQ_Param(T_IQ_PARAM* const param);
ULONG Cate_Get_IQ_LDC_TBL_IDX(VOID);
E_SHARE_DISP_IF Cate_Get_Disp_If( VOID );
VOID Cate_Get_Dcf_File_Name( CHAR * obj_name );
VOID Cate_Get_Dcf_Dir_Name( CHAR * obj_name );
VOID Cate_Get_Exif_Maker_Note( CHAR * maker_note );
void Cate_Get_Exif_focal_length ( float *focal_length);
VOID Cate_Get_DCF_UserDrive( CHAR* drive );
VOID Cate_Get_Thumbnail_Size( ULONG * width, ULONG * lines );
INT32 Cate_Decode_String( INT32 argc, CHAR** argv );
INT32 Cate_Set( UCHAR cate, UCHAR index, UCHAR size, const ULONG* data );
INT32 Cate_Get( UCHAR cate, UCHAR index, UCHAR size, ULONG* data );
VOID Cate_Get_SHDCAG_EN(BOOL *shd_en, BOOL *cag_en);
UINT32 Cate_Get_HDMI_VID_SEL(void);
void Cate_Set_WB_Result(void);
void Cate_Set_AE_Result(void);
VOID Cate_Set_AutoFraming_Roi_Pos(UINT32 roiX, UINT32 roiY, UINT32 roiW, UINT32 roiH);
VOID Cate_Get_AutoFraming_Win_Pos(UINT32 *winSize, UINT32 *winPos);
VOID Cate_Set_Latest_Image_Index( UINT32 index );
UINT32 Cate_Get_Latest_Image_Index( VOID );
VOID Cate_Set_Latest_Movie_Index( UINT32 index );
UINT32 Cate_Get_Latest_Movie_Index( VOID );
VOID Cate_Get_Calib_Badpix_Param( T_CALIB_BADPIX_PARAM* const param );
ULONG Cate_Get_Bridge_Num(VOID);
ULONG Cate_Get_HDMI_SEN_USED(VOID);
VOID Cate_Get_Stream_Info(T_SHARE_STREAM_INFO *info);
ULONG Cate_Get_Drop_Frame_Half(VOID);
VOID Cate_Get_UVC_Param( T_UVC_PARAM* param );
VOID Cate_Set_Audio_SampleRate( UINT32 data );
BOOL Cate_Get_IPCU_Streaming(VOID);
VOID Cate_Update_FD_Param( UCHAR en );
VOID Cate_Update_PD_Param( UCHAR en );
VOID Cate_Update_Roi0_PTZ_Info( FLOAT trim_ratio, FLOAT zoom_pos_x, FLOAT zoom_pos_y );
UINT32 Cate_Get_Line_Transfer_Cycle(VOID);
T_IM_PRO_AEAWB_LV_GATE Cate_Get_AWB_RGB_LVL_Gate(VOID);

#ifdef __cplusplus
}
#endif

#endif	// _CATEGORY_PARAMETER_H_

