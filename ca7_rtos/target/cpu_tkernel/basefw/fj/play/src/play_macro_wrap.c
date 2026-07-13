/**
 * @file		play_macro_wrap.c
 * @brief		playback macro wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_PB_INIT"
//#pragma arm section rodata="BASEFW_PB_CONST"
//#pragma arm section code="BASEFW_PB_CODE"

#include "play_attr.h"
#include "play_macro_wrap.h"
#include "play_common.h"
#include "still_attr.h"
#include "still_utility.h"
#include "jpgepc.h"
// --- REMOVE_DCF_EXIF BEGIN ---
#include "exif_controller.h"
// --- REMOVE_DCF_EXIF END ---
#include "sdram_map_play.h"
#include "sdram_map_cache_define.h"
#include "osd_attr.h"
#include "play_jpeg.h"
#include "bf_iip_resource.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
//#define CO_BF_PLAY_MACRO_WRAP_ENDIAN_BIG				/**< CPU Endian = BIG */
//#define	CO_BF_PLAY_MACRO_WRAP_JPEG_ASYNC

// [IIP]
#define D_BF_PLAY_MACRO_WRAP_IIP_PIXID_MAX			(12)

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
#ifdef CO_BF_PLAY_MACRO_WRAP_JPEG_ASYNC
static INT32							gPlay_JPEG_Decode_Wait;
static T_IM_JPEG_DEC_INFO				gPlay_JPEG_Decode_Mng;
#endif
T_BF_PLAY_MACRO_WRAP_DISP_PARAM			g_image_addr_lcd;
T_BF_PLAY_MACRO_WRAP_DISP_PARAM			g_image_addr_hdmi;

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
#define M_BF_PLAY_MACRO_WRAP_ROUNDDOWN_8( val )		(((val) >> 3) << 3)
#define M_BF_PLAY_MACRO_WRAP_ROUNDUP_8( val )		(((UINT32)(val) +7) & 0xFFFFFFF8)
#define M_BF_PLAY_MACRO_WRAP_ROUNDUP_16( val )		(((UINT32)(val) +15) & 0xFFFFFFF0)

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
/******************************/
/** Common Section(FastBoot) **/
/******************************/
//#pragma arm section code="BASEFW_BOOT_CODE"

// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief		size convert & roate for IIP macro
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src		source convert info
 * @param		i:E_BF_PLAY_MACRO_WRAP_JPG_SMPL src_ycc_type			source ycc type
 * @param		i:E_IM_IIP_FTYPE src_frame_type				source frame type
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst		destination convert info
 * @param		i:E_BF_PLAY_MACRO_WRAP_JPG_SMPL dst_ycc_type			destination ycc type
 * @param		i:E_IM_IIP_FTYPE dst_frame_type				destination frame type
 * @param		i:E_BF_DCF_IF_ROTATE rotate					rotate type
 * @param		i:E_BF_PLAY_MACRO_WRAP_ADJUST aspect		aspect adjust flag
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	None
 */
static INT32 play_macro_wrap_rotate_convert_image( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src, E_BF_PLAY_MACRO_WRAP_PFMT src_ycc_type, E_IM_IIP_FTYPE src_frame_type,
													T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst, E_BF_PLAY_MACRO_WRAP_PFMT dst_ycc_type, E_IM_IIP_FTYPE dst_frame_type,
											   		E_BF_DCF_IF_ROTATE rotate, E_BF_PLAY_MACRO_WRAP_ADJUST aspect )
{
	INT32	retval = 0;
	INT32	set_loop;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	dummy_src;

	
	T_IM_IIP_UTIL_RR			iip_util_cfg;
	T_FJ_IIP_RES_CONFIG_ARG		iip_res_config_arg;
	T_FJ_IIP_RES_CONFIG_OUT		iip_res_config_out;
	T_FJ_IIP_RES_CONFIG_OUT		iip_res_get_resource;
	INT32						wait_time = D_BF_PLAY_MACRO_WRAP_WAIT_TIME;
	UCHAR						param_master_IF	= D_IM_IIP_MASTER_IF0;
	UCHAR						param_buffer[(D_IM_IIP_UTIL_RR_BUF_BYTES * 2) + 8];
	ULONG						param_buffer_addr = (((ULONG)param_buffer) + 7UL) & 0xFFFFFFF8UL;

	E_IM_IIP_PFMT				src_pfmt;
	E_IM_IIP_PFMT				dst_pfmt;
	E_IM_IIP_UTIL_ROTATE		afn_rotate;
	T_IMAGE_SIZE				src_rect_part = {0};
	T_IMAGE_SIZE				dst_rect_part = {0};
	ULONG						src_add_offset_part = 0;
	ULONG						dst_add_offset_part = 0;
	
	// Initialize
	memcpy(&dummy_src, &src, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR));

	if ((rotate == E_BF_DCF_IF_ROTATE_90) || (rotate == E_BF_DCF_IF_ROTATE_270)) {
		dummy_src.size.width = src.size.lines;
		dummy_src.size.lines = src.size.width;
	}

	// Adjust destination info
	BF_Play_Macro_Wrap_Set_Adjust_Param( dummy_src, dst, dst_ycc_type, aspect );
	
	// Select pixel format
	switch (src_ycc_type) {
		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC422:
			src_pfmt		= E_IM_IIP_PFMT_YCC422;
			break;

		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC420:
			src_pfmt		= E_IM_IIP_PFMT_YCC420;
			break;

		default:
			BF_Debug_Print_Error(("play_macro_wrap_rotate_convert_image(): Unsupported sub-sampling type\n"));
			return -1;
	}

	// Select pixel format
	switch (dst_ycc_type) {
		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC422:
			dst_pfmt		= E_IM_IIP_PFMT_YCC422;
			break;

		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC420:
			dst_pfmt		= E_IM_IIP_PFMT_YCC420;
			break;

		default:
			BF_Debug_Print_Error(("play_macro_wrap_rotate_convert_image(): Unsupported sub-sampling type\n"));
			return -1;
	}

	// Set Cr address
	if (src_frame_type == E_IM_IIP_FTYPE_Y_PACKED_CBCR) {
		src.addrs.addr_c = src.addrs.addr_b;
	}
	if (dst_frame_type == E_IM_IIP_FTYPE_Y_PACKED_CBCR) {
		dst->addrs.addr_c = dst->addrs.addr_b;
	}

	// Set rotate type
	switch (rotate) {
		case E_BF_DCF_IF_ROTATE_90:
			afn_rotate = E_IM_IIP_UTIL_ROTATE_090;
			
			src_rect_part.width = src.size.width;
			src_rect_part.lines = src.size.lines;
			src_add_offset_part = 0;
			
			dst_rect_part.width = dst->size.width;
			dst_rect_part.lines = dst->size.lines;
			dst_add_offset_part = 0;
			
			break;
		case E_BF_DCF_IF_ROTATE_180:
			afn_rotate = E_IM_IIP_UTIL_ROTATE_180;
			
			src_rect_part.width = src.size.width;
			src_rect_part.lines = src.size.lines;
			src_add_offset_part = 0;
			
			dst_rect_part.width = dst->size.width;
			dst_rect_part.lines = dst->size.lines;
			dst_add_offset_part = 0;
			
			break;
		case E_BF_DCF_IF_ROTATE_270:
			afn_rotate = E_IM_IIP_UTIL_ROTATE_270;
			
			src_rect_part.width = src.size.width;
			src_rect_part.lines = src.size.lines;
			src_add_offset_part = 0;
			
			dst_rect_part.width = dst->size.width;
			dst_rect_part.lines = dst->size.lines;
			dst_add_offset_part = 0;
			
			break;
		default:
			afn_rotate = E_IM_IIP_UTIL_ROTATE_000;
			
			src_rect_part.width = src.size.width;
			src_rect_part.lines = src.size.lines;
			src_add_offset_part = 0;
			
			dst_rect_part.width = dst->size.width;
			dst_rect_part.lines = dst->size.lines;
			dst_add_offset_part = 0;
			
			break;
	}
	
	memset( &iip_res_config_arg,   0, sizeof( T_FJ_IIP_RES_CONFIG_ARG ));
	memset( &iip_res_config_out, 0, sizeof( T_FJ_IIP_RES_CONFIG_OUT ));
	iip_res_config_arg.unit.afn				= 1;
	iip_res_config_arg.unit.sl_master_if_0	= 1;
	iip_res_config_arg.pixid				= 2;
	
	// IIP Resource Get
	retval = BF_IIP_Res_Config_Lock_BaseFW( &iip_res_config_out, &iip_res_config_arg, 1, wait_time );
	if( retval != D_FJ_IIP_ERR_OK ){
		BF_Debug_Print_Error(("play_macro_wrap_rotate_convert_image(): BF_IIP_Res_Config_Lock_BaseFW Error=%d\n", retval));
		return -1;
	}
	
	// saves local
	iip_res_get_resource = iip_res_config_out;
	
	// afn unit id get
	if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN0 ){
		iip_util_cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN0;
		iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN0;
	}
	if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN1 ){
		iip_util_cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN1;
		iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN1;
	}
	if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN2 ){
		iip_util_cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN2;
		iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN2;
	}
	if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN3 ){
		iip_util_cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN3;
		iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN3;
	}
	
	// sl unit(memory) id get
	if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL0 ){
		iip_util_cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL0;
		iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL0;
	}
	if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL3 ){
		iip_util_cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL3;
		iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL3;
	}
	if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL5 ){
		iip_util_cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL5;
		iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL5;
	}
	if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL6 ){
		iip_util_cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL6;
		iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL6;
	}
	
	// pixel id get
	set_loop = 0;
	for( ; set_loop < D_BF_PLAY_MACRO_WRAP_IIP_PIXID_MAX; set_loop++ ){
		if( iip_res_get_resource.pixid_bitmask & (1 << set_loop) ){
			iip_util_cfg.src_pixid = (E_IM_IIP_PIXID)(1 << set_loop);
			iip_res_get_resource.pixid_bitmask &= ~(1 << set_loop);
			break;
		}
	}
	for( ; set_loop < D_BF_PLAY_MACRO_WRAP_IIP_PIXID_MAX; set_loop++ ){
		if( iip_res_get_resource.pixid_bitmask & (1 << set_loop) ){
			iip_util_cfg.dst_pixid = (E_IM_IIP_PIXID)(1 << set_loop);
			iip_res_get_resource.pixid_bitmask &= ~(1 << set_loop);
			break;
		}
	}
	if( set_loop == D_BF_PLAY_MACRO_WRAP_IIP_PIXID_MAX ){
		BF_Debug_Print_Error(("play_macro_wrap_rotate_convert_image(): IIP Resource PIXID Get NG!!\n"));
		BF_IIP_Res_Config_Free_BaseFW( &iip_res_config_out, 1, wait_time );
		return -1;
	}
	if( (iip_res_get_resource.unitid_bitmask64 != 0) || (iip_res_get_resource.pixid_bitmask != 0) ){
		BF_Debug_Print_Error(("play_macro_wrap_rotate_convert_image(): IIP Resource Abnormal!! \n"));
		BF_IIP_Res_Config_Free_BaseFW( &iip_res_config_out, 1, wait_time );
		return -1;
	}
	
	// common setting
	iip_util_cfg.bi_cubic_unitid					= E_IM_IIP_PARAM_SCUB_0;
	iip_util_cfg.afn_cache_select					= E_IM_IIP_PARAM_CSEL_0;
	iip_util_cfg.pix_depth							= D_IM_IIP_PDEPTH_8BITS;
	iip_util_cfg.rotation							= afn_rotate;
	iip_util_cfg.flip								= E_IM_IIP_UTIL_FLIP_OFF;
	iip_util_cfg.resize_mode						= E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;

	// source sen
	iip_util_cfg.src.rect.top						= 0.0;
	iip_util_cfg.src.rect.left						= 0.0;
	iip_util_cfg.src.rect.width					= src_rect_part.width;
	iip_util_cfg.src.rect.lines					= src_rect_part.lines;

	iip_util_cfg.src.gbl.alpha						= D_IM_IIP_ALPHA_FALSE;
	iip_util_cfg.src.gbl.frame_type				= src_frame_type;
	iip_util_cfg.src.gbl.pix_format				= src_pfmt;

	iip_util_cfg.src.gbl.chunky_color.component0	= D_IM_IIP_CHUNKY_COLOR_Y0_G;
	iip_util_cfg.src.gbl.chunky_color.component1	= D_IM_IIP_CHUNKY_COLOR_CB_B;
	iip_util_cfg.src.gbl.chunky_color.component2	= D_IM_IIP_CHUNKY_COLOR_CR_R;
	iip_util_cfg.src.gbl.chunky_color.component3	= D_IM_IIP_CHUNKY_COLOR_Y1_A;
	iip_util_cfg.src.gbl.sign_Y_G					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_cfg.src.gbl.sign_Cb_B					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_cfg.src.gbl.sign_Cr_R					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_cfg.src.gbl.sign_D3					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_cfg.src.gbl.width						= src_rect_part.width;
	iip_util_cfg.src.gbl.lines						= src_rect_part.lines;
	iip_util_cfg.src.gbl.line_bytes.Y_G			= src.size.global_width;
	if(src_frame_type == E_IM_IIP_FTYPE_PLANE){
		iip_util_cfg.src.gbl.line_bytes.Cb_B 		= (src.size.global_width >> 1);
		iip_util_cfg.src.gbl.line_bytes.Cr_R 		= (src.size.global_width >> 1);
	}
	else{
		iip_util_cfg.src.gbl.line_bytes.Cb_B 		= src.size.global_width;
		iip_util_cfg.src.gbl.line_bytes.Cr_R 		= src.size.global_width;
	}

	iip_util_cfg.src.gbl.line_bytes.Alpha			= 0;
	iip_util_cfg.src.gbl.addr.Y_G					= src.addrs.addr_a + src_add_offset_part;
	iip_util_cfg.src.gbl.addr.Cb_B 				= src.addrs.addr_b + src_add_offset_part;
	iip_util_cfg.src.gbl.addr.Cr_R 				= src.addrs.addr_c + src_add_offset_part;
	iip_util_cfg.src.gbl.addr.Alpha				= 0;

	iip_util_cfg.src.gbl.masterIF_Y_G 				= D_IM_IIP_MASTER_IF0;
	iip_util_cfg.src.gbl.masterIF_Cb_B 			= D_IM_IIP_MASTER_IF0;
	iip_util_cfg.src.gbl.masterIF_Cr_R				= D_IM_IIP_MASTER_IF0;
	iip_util_cfg.src.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF0;
	
	// destinatisting
	iip_util_cfg.dst.rect.top						= 0;
	iip_util_cfg.dst.rect.left						= 0;
	iip_util_cfg.dst.rect.width					= dst_rect_part.width;
	iip_util_cfg.dst.rect.lines					= dst_rect_part.lines;

	iip_util_cfg.dst.gbl.alpha						= D_IM_IIP_ALPHA_FALSE;
	iip_util_cfg.dst.gbl.frame_type				= dst_frame_type;
	iip_util_cfg.dst.gbl.pix_format				= dst_pfmt;

	iip_util_cfg.dst.gbl.chunky_color.component0	= D_IM_IIP_CHUNKY_COLOR_Y0_G;
	iip_util_cfg.dst.gbl.chunky_color.component1	= D_IM_IIP_CHUNKY_COLOR_CB_B;
	iip_util_cfg.dst.gbl.chunky_color.component2	= D_IM_IIP_CHUNKY_COLOR_CR_R;
	iip_util_cfg.dst.gbl.chunky_color.component3	= D_IM_IIP_CHUNKY_COLOR_Y1_A;
	iip_util_cfg.dst.gbl.sign_Y_G					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_cfg.dst.gbl.sign_Cb_B					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_cfg.dst.gbl.sign_Cr_R					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_cfg.dst.gbl.sign_D3					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_cfg.dst.gbl.width						= dst_rect_part.width;
	iip_util_cfg.dst.gbl.lines						= dst_rect_part.lines;
	iip_util_cfg.dst.gbl.line_bytes.Y_G			= dst->size.global_width;
	if(dst_frame_type == E_IM_IIP_FTYPE_PLANE){
		iip_util_cfg.dst.gbl.line_bytes.Cb_B 		= (dst->size.global_width >> 1);
		iip_util_cfg.dst.gbl.line_bytes.Cr_R 		= (dst->size.global_width >> 1);
	}
	else{
		iip_util_cfg.dst.gbl.line_bytes.Cb_B 		= dst->size.global_width;
		iip_util_cfg.dst.gbl.line_bytes.Cr_R 		= dst->size.global_width;
	}

	iip_util_cfg.dst.gbl.line_bytes.Alpha			= 0;
	iip_util_cfg.dst.gbl.addr.Y_G					= dst->addrs.addr_a + dst_add_offset_part;
	iip_util_cfg.dst.gbl.addr.Cb_B 				= dst->addrs.addr_b + dst_add_offset_part;
	iip_util_cfg.dst.gbl.addr.Cr_R 				= dst->addrs.addr_c + dst_add_offset_part;
	iip_util_cfg.dst.gbl.addr.Alpha				= 0;

	iip_util_cfg.dst.gbl.masterIF_Y_G 				= D_IM_IIP_MASTER_IF1;
	iip_util_cfg.dst.gbl.masterIF_Cb_B 			= D_IM_IIP_MASTER_IF1;
	iip_util_cfg.dst.gbl.masterIF_Cr_R				= D_IM_IIP_MASTER_IF1;
	iip_util_cfg.dst.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF1;
	
	
	retval = IM_IIP_Utility_ResizeRotate( &iip_util_cfg, NULL, param_master_IF, param_buffer_addr, wait_time );
	if( retval != D_IM_IIP_OK ){
		BF_Debug_Print_Error(("play_macro_wrap_rotate_convert_image(): IM_IIP_Utility_ResizeRotate error ret=%d\n", retval));
		BF_IIP_Res_Config_Free_BaseFW( &iip_res_config_out, 1, wait_time );
		return retval;
	}

	// IIP Resource Release
	retval = BF_IIP_Res_Config_Free_BaseFW( &iip_res_config_out, 1, wait_time );
	if( retval != D_FJ_IIP_ERR_OK ){
		BF_Debug_Print_Error(("play_macro_wrap_rotate_convert_image(): BF_IIP_Res_Config_Free_BaseFW Error=%d\n", retval));
		return -1;
	}

	return 0;
}
// --- REMOVE_DCF_EXIF END ---

/******************************/
/**  PB Section(FastBoot)    **/
/******************************/
//#pragma arm section code="BASEFW_PB_CODE"
/**
 * @brief		Specified JPEG is displayed
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR*	src_param		: src parameter.
 * @param		i:T_BF_PLAY_MACRO_WRAP_DISP_PARAM*	dst_param		: dst parameter.
 * @param		i:ULONG								half_rec_addr	: half rec addr.
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
static FJ_ERR_CODE play_macro_wrap_convert_display_window( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* src_param, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst_param, ULONG half_rec_addr )
{
	INT32 ret = 0;
	BOOL resized_flag = FALSE;

	if( (src_param->size.width == dst_param->size.width) && (src_param->size.lines == dst_param->size.lines) ) {
		BF_Play_Macro_Wrap_Copy_Image(src_param->addrs.addr_a, src_param->addrs.addr_b, src_param->size.global_width, 
										dst_param->addrs.addr_a, dst_param->addrs.addr_b, dst_param->size.global_width, 
										dst_param->size.width, dst_param->size.lines, 
										E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
										E_BF_PLAY_MACRO_WRAP_USED_2CH);
	}
	else {
		// Half Rec
		ret = BF_Play_Common_Half_Convert(src_param, dst_param, half_rec_addr, E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420, &resized_flag);
		if(ret != 0) {
			BF_Debug_Print_Error(("play_macro_wrap_convert_display_window():BF_Play_Common_Half_Convert error!\n"));
			return FJ_ERR_NG;
		}
		
// --- REMOVE_DCF_EXIF BEGIN ---
		ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( *src_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, dst_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420,
											   E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_ON );
// --- REMOVE_DCF_EXIF END ---
		if(ret != 0) {
			BF_Debug_Print_Error(("play_macro_wrap_convert_display_window():BF_Play_Macro_Wrap_Rotate_Convert_Image error!\n"));
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

/**
 * @brief		Display Playback Image to HDMI
 * @param		i:hd_size_addr	Size and Address of display image.
 * @param		i:display_type	Function type.(PB/OSD)
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID play_macro_wrap_display_hdmi(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR hd_size_addr, E_BF_PLAY_MACRO_WRAP_DISPLAY_TYPE display_type)
{
	FJ_ERR_CODE						ercd;
	BOOL							disp_center_flg;
	ULONG							vram_addr;
	ULONG							half_rec_addr;
	T_BF_PLAY_MACRO_WRAP_DISP_ADDR	image_addr;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	disp_dst;
	T_BF_PLAY_MACRO_WRAP_DISP_PARAM	disp_info;
	T_BF_PLAY_MACRO_WRAP_SIZE		output_size;
// --- REMOVE_DCF_EXIF BEGIN ---
	E_BF_DCF_IF_FILE_TYPE			file_type;
// --- REMOVE_DCF_EXIF END ---
	T_BF_PLAY_MACRO_WRAP_SIZE		image_size;

	memset( &disp_info, 0, sizeof(T_BF_PLAY_MACRO_WRAP_DISP_PARAM) );

	// Get output image size.
	ercd = BF_Play_Macro_Wrap_Get_Hdmi_Output_Size(&output_size);
	if (ercd != FJ_ERR_OK) {
		return;
	}

	disp_info.gl_width = output_size.global_width;
	disp_info.gl_lines = output_size.lines;
	
	if ((BF_Play_Attr_Get_Screen_Mode() == FJ_MODE_DISP_IMAGE_THUMB) && (gBF_Play_Common_Display_Data.play_file_info == E_BF_PLAY_COMMON_FILE_INFO_SINGLE)) {
// --- REMOVE_DCF_EXIF BEGIN ---
		file_type = BF_Dcf_If_Convert_File_Type( BF_Play_Attr_Get_File_Type() );
		switch (file_type) {
			case E_BF_DCF_IF_FILE_TYPE_AVI :		// Fallthrough
			case E_BF_DCF_IF_FILE_TYPE_MP4 :		// Fallthrough
			case E_BF_DCF_IF_FILE_TYPE_MOV :
				disp_info.pos_x  = 0;
				disp_info.pos_y  = 0;
				disp_info.width  = output_size.width;
				disp_info.lines  = output_size.lines;
				break;
			default :
				// Get display window parameters specified by HOST.
				disp_info.pos_x  = BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Pos_X();
				disp_info.pos_y  = BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Pos_Y();
				disp_info.width  = BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Width();
				disp_info.lines  = BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Lines();
				break;
		}
// --- REMOVE_DCF_EXIF END ---
		disp_center_flg  = BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Arflg();
	}
	else {
		// Get display window parameters specified by HOST.
		disp_info.pos_x  = BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Pos_X();
		disp_info.pos_y  = BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Pos_Y();
		disp_info.width  = BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Width();
		disp_info.lines  = BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Lines();
		disp_center_flg  = BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Arflg();
	}

	if( (disp_info.pos_x + disp_info.width) > disp_info.gl_width || (disp_info.pos_y + disp_info.lines) > disp_info.gl_lines ) {
		BF_Debug_Print_Error(("play_macro_wrap_display_hdmi():disp param Error! pos (x:%d,y:%d) size(width:%d,hight:%d) global size(gl_width:%d,gl_hight:%d)\n", disp_info.pos_x, disp_info.pos_y, disp_info.width, disp_info.lines, disp_info.gl_width, disp_info.gl_lines));
		return;
	}

	if ((disp_info.pos_x != 0) || (disp_info.pos_y != 0) || (disp_info.width != output_size.width) || (disp_info.lines != output_size.lines)) {
		// Display window processing.

		if (disp_center_flg) {
			// Display position is carried out in the center.
			disp_info.pos_x = ((disp_info.gl_width - disp_info.width) >> 1);
			disp_info.pos_y = ((disp_info.gl_lines - disp_info.lines) >> 1);
		}

		// Coordinates are made into even number. 
		if( (disp_info.pos_x & 0x1) != 0) {
			disp_info.pos_x--;
		}
		if( (disp_info.pos_y & 0x1) != 0) {
			disp_info.pos_y--;
		}
		
		// Get next VRAM address.
		half_rec_addr = BF_Sdram_Map_Play_Get_Half_Rec_Addr();
		vram_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(SDRAM_ADR_PLAY_DISP_WIND_DATA_HD);		
		BF_Play_Macro_Wrap_Set_Size_Addr(&disp_dst, &vram_addr, disp_info.width, disp_info.lines, disp_info.gl_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG);
		
		ercd = play_macro_wrap_convert_display_window(&hd_size_addr, &disp_dst, half_rec_addr);
		if (ercd != FJ_ERR_OK) {
			// DisplayWindow processing error.
			BF_Debug_Print_Error(("play_macro_wrap_display_hdmi(): play_macro_wrap_convert_display_window error. ercd=%d\n", ercd));
			return;
		}

		image_addr.addrs.y_addr = disp_dst.addrs.addr_a;
		image_addr.addrs.c_addr = disp_dst.addrs.addr_b;
		image_size.global_width = disp_dst.size.global_width;
		image_size.width        = disp_dst.size.width;
		image_size.lines        = disp_dst.size.lines;
	}
	else {
		image_addr.addrs.y_addr = hd_size_addr.addrs.addr_a;
		image_addr.addrs.c_addr = hd_size_addr.addrs.addr_b;
		image_size.global_width = hd_size_addr.size.global_width;
		image_size.width        = hd_size_addr.size.width;
		image_size.lines        = hd_size_addr.size.lines;
	}

	if (BF_Get_Pb_AutoDisplay() == E_PB_AUTODISP_OFF) {
		// Save next image addres for FJ_Show_Pb_DispImage().
		g_image_addr_hdmi.gl_width    = image_size.global_width;
		g_image_addr_hdmi.gl_lines    = 0;
		g_image_addr_hdmi.width       = image_size.width;
		g_image_addr_hdmi.lines       = image_size.lines;
		g_image_addr_hdmi.pos_x       = disp_info.pos_x;
		g_image_addr_hdmi.pos_y       = disp_info.pos_y;
		g_image_addr_hdmi.addr.addr_a = image_addr.addrs.y_addr;		
		g_image_addr_hdmi.addr.addr_b = image_addr.addrs.c_addr;
	}
	else {
		if (display_type == E_BF_PLAY_MACRO_WRAP_DISPLAY_TYPE_PB) {
			U_IM_DISP_SIZE input_disp_size = {0};
			ULONG y_hga, c_hga;
			U_IM_DISP_DSTA position = {0};
			T_IM_DISP_RESIZE resize = {0};

			Im_DISP_Get_Input_Size(E_IM_DISP_HDMI, &input_disp_size);
			Im_DISP_Get_Lfd(E_IM_DISP_HDMI, &y_hga, &c_hga);

			if ((input_disp_size.size.width != image_size.width) || (input_disp_size.size.lines != image_size.lines) || (y_hga != image_size.global_width) || (c_hga != image_size.global_width)){
				//Set DISP size
				input_disp_size.size.width = image_size.width;
				input_disp_size.size.lines = image_size.lines;

				BF_Play_Common_Disp_Set_Int_Callback(TRUE, 1, E_IM_DISP_WAIT_1_TIME);
				//wait VD
				BF_Play_Common_DISP_Wait_Vsync();

				Im_DISP_Set_Input_Size(E_IM_DISP_HDMI, input_disp_size);
				Im_DISP_Set_Lfd(E_IM_DISP_HDMI, image_size.global_width, image_size.global_width);

				//Im_DISP_Get_Resize(E_IM_DISP_HDMI, &resize);

				//position.bit.DSH = BF_Play_Common_RoundDown_Bit_2(((disp_info.pos_x + ((disp_info.width - image_size.width) >> 1)) * resize.rsz1.bit.HRSZM) / resize.rsz1.bit.HRSZN);
				//position.bit.DSV = BF_Play_Common_RoundDown_Bit_2(((disp_info.pos_y + ((disp_info.lines - image_size.lines) >> 1)) * resize.rsz2.bit.VRSZM) / resize.rsz2.bit.VRSZN);
				position.bit.DSH = 0;
				position.bit.DSV = 0;
				Im_DISP_Set_Display_Position(E_IM_DISP_HDMI, position);

				resize.rsz0 = E_IM_DISP_RSZSL_BILINEAR;
				resize.rsz1.bit.HRSZM = 1;
				resize.rsz1.bit.HRSZN = 1;
				resize.rsz1.bit.HRSZOF = 0;
				resize.rsz2.bit.VRSZM = 1;
				resize.rsz2.bit.VRSZN = 1;
				resize.rsz2.bit.VRSZOF = 0;
				resize.rsz3.bit.HCSTA = 0;
				resize.rsz3.bit.VCSTA = 0;
				Im_DISP_Set_Resize(E_IM_DISP_HDMI, &resize);

				BF_Play_Common_Disp_Set_Int_Callback(FALSE, 0, E_IM_DISP_WAIT_1_TIME);
			}
			// Display next image.
			BF_Play_Macro_Wrap_Display( E_IM_DISP_HDMI, &image_addr );
		}
	}

	return;
}

/**
 * @brief		Display Playback Image to DLCD
 * @param		i:vga_size_addr	Size and Address of display image.
 * @param		i:display_type	Function type.(PB/OSD)
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID play_macro_wrap_display_dlcd( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR vga_size_addr, E_BF_PLAY_MACRO_WRAP_DISPLAY_TYPE display_type )
{
	// LCD VGA_size only
	FJ_ERR_CODE						ret;
	BOOL							disp_center_flg;
	ULONG							vram_addr;
	ULONG							half_rec_addr;
	T_BF_PLAY_MACRO_WRAP_DISP_ADDR	image_addr;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	disp_dst;
	T_BF_PLAY_MACRO_WRAP_DISP_PARAM	disp_info;
// --- REMOVE_DCF_EXIF BEGIN ---
	E_BF_DCF_IF_FILE_TYPE			file_type;
// --- REMOVE_DCF_EXIF END ---
	T_BF_PLAY_MACRO_WRAP_SIZE 		image_size;

	memset( &disp_info, 0, sizeof(T_BF_PLAY_MACRO_WRAP_DISP_PARAM) );

	disp_info.gl_width = WIDTH_VGA;
	disp_info.gl_lines = LINES_VGA;
	
	if ((BF_Play_Attr_Get_Screen_Mode() == FJ_MODE_DISP_IMAGE_THUMB) && (gBF_Play_Common_Display_Data.play_file_info == E_BF_PLAY_COMMON_FILE_INFO_SINGLE)) {
// --- REMOVE_DCF_EXIF BEGIN ---
		file_type = BF_Dcf_If_Convert_File_Type( BF_Play_Attr_Get_File_Type() );
		switch (file_type) {
			case E_BF_DCF_IF_FILE_TYPE_AVI :		// Fallthrough
			case E_BF_DCF_IF_FILE_TYPE_MP4 :		// Fallthrough
			case E_BF_DCF_IF_FILE_TYPE_MOV :
				disp_info.pos_x = 0;
				disp_info.pos_y = 0;
				disp_info.width = WIDTH_VGA;
				disp_info.lines = LINES_VGA;
				break;
			default :
				disp_info.pos_x = BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Pos_X();
				disp_info.pos_y = BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Pos_Y();
				disp_info.width = BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Width();
				disp_info.lines = BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Lines();
				break;
		}
// --- REMOVE_DCF_EXIF END ---
		disp_center_flg  = BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Arflg();
	}
	else {
		disp_info.pos_x = BF_Play_Attr_Get_Disp_Win_Play_Lcd_Pos_X();
		disp_info.pos_y = BF_Play_Attr_Get_Disp_Win_Play_Lcd_Pos_Y();
		disp_info.width = BF_Play_Attr_Get_Disp_Win_Play_Lcd_Width();
		disp_info.lines = BF_Play_Attr_Get_Disp_Win_Play_Lcd_Lines();
		disp_center_flg = BF_Play_Attr_Get_Disp_Win_Play_Lcd_Arflg();
	}

	if( (disp_info.pos_x + disp_info.width) > disp_info.gl_width || (disp_info.pos_y + disp_info.lines) > disp_info.gl_lines ) {
		BF_Debug_Print_Error(("play_macro_wrap_display_dlcd():disp param Error! pos (x:%d,y:%d) size(width:%d,hight:%d)\n", disp_info.pos_x, disp_info.pos_y, disp_info.width, disp_info.lines));
		return;
	}

	if( (disp_info.pos_x != 0) || (disp_info.pos_y != 0) || (disp_info.width != WIDTH_VGA) || (disp_info.lines != LINES_VGA) ) {
		// Display window processing.		
		
		if (disp_center_flg) {
			// Display position is carried out in the center.
			disp_info.pos_x = ((disp_info.gl_width - disp_info.width) >> 1);
			disp_info.pos_y = ((disp_info.gl_lines - disp_info.lines) >> 1);
		}

		// Coordinates are made into even number. 
		if( (disp_info.pos_x & 0x1) != 0) {
			disp_info.pos_x--;
		}
		if( (disp_info.pos_y & 0x1) != 0) {
			disp_info.pos_y--;
		}
		
		// Get next VRAM address.
		half_rec_addr = BF_Sdram_Map_Play_Get_Half_Rec_Addr();
		vram_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( SDRAM_ADR_PLAY_DISP_WIND_DATA_VGA );
		BF_Play_Macro_Wrap_Set_Size_Addr(&disp_dst, &vram_addr, disp_info.width, disp_info.lines, disp_info.gl_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG);
				
		ret = play_macro_wrap_convert_display_window(&vga_size_addr, &disp_dst, half_rec_addr);
		if(ret != FJ_ERR_OK) {
			// DisplayWindow processing error.
			BF_Debug_Print_Error(("play_macro_wrap_display_dlcd(): play_macro_wrap_convert_display_window error. ercd=%d\n", ret));
			return;
		}
		
		image_addr.addrs.y_addr = disp_dst.addrs.addr_a;
		image_addr.addrs.c_addr = disp_dst.addrs.addr_b;
		image_size.global_width = disp_dst.size.global_width;
		image_size.width        = disp_dst.size.width;
		image_size.lines        = disp_dst.size.lines;
	}
	else {
		image_addr.addrs.y_addr = vga_size_addr.addrs.addr_a;
		image_addr.addrs.c_addr = vga_size_addr.addrs.addr_b;
		image_size.global_width = vga_size_addr.size.global_width;
		image_size.width        = vga_size_addr.size.width;
		image_size.lines        = vga_size_addr.size.lines;
	}

	if (BF_Get_Pb_AutoDisplay() == E_PB_AUTODISP_OFF) {
		// Save next image addres for FJ_Show_Pb_DispImage().	
		g_image_addr_lcd.gl_width    = image_size.global_width;
		g_image_addr_lcd.gl_lines    = 0;
		g_image_addr_lcd.width       = image_size.width;
		g_image_addr_lcd.lines       = image_size.lines;
		g_image_addr_lcd.pos_x       = disp_info.pos_x;
		g_image_addr_lcd.pos_y       = disp_info.pos_y;
		g_image_addr_lcd.addr.addr_a = image_addr.addrs.y_addr;		
		g_image_addr_lcd.addr.addr_b = image_addr.addrs.c_addr;
	}
	else {
		if (display_type == E_BF_PLAY_MACRO_WRAP_DISPLAY_TYPE_PB) {
			U_IM_DISP_SIZE input_disp_size = {0};
			ULONG y_hga, c_hga;
			U_IM_DISP_DSTA position = {0};
			T_IM_DISP_RESIZE resize;

			Im_DISP_Get_Input_Size(E_IM_DISP_LCD_MIPI, &input_disp_size);
			Im_DISP_Get_Lfd(E_IM_DISP_LCD_MIPI, &y_hga, &c_hga);
			if ((input_disp_size.size.width != image_size.width) || (input_disp_size.size.lines != image_size.lines) || (y_hga != image_size.global_width) || (c_hga != image_size.global_width)){
				//Set DISP size
				input_disp_size.size.width = image_size.width;
				input_disp_size.size.lines = image_size.lines;

				BF_Play_Common_Disp_Set_Int_Callback(TRUE, 1, E_IM_DISP_WAIT_1_TIME);
				//wait VD
				BF_Play_Common_DISP_Wait_Vsync();

				Im_DISP_Set_Input_Size(E_IM_DISP_LCD_MIPI, input_disp_size);
				Im_DISP_Set_Lfd(E_IM_DISP_LCD_MIPI, image_size.global_width, image_size.global_width);
				Im_DISP_Get_Resize(E_IM_DISP_LCD_MIPI, &resize);

				position.bit.DSH = BF_Play_Common_RoundDown_Bit_2(((disp_info.pos_x + ((disp_info.width - image_size.width) >> 1)) * resize.rsz1.bit.HRSZM) / resize.rsz1.bit.HRSZN);
				position.bit.DSV = BF_Play_Common_RoundDown_Bit_2(((disp_info.pos_y + ((disp_info.lines - image_size.lines) >> 1)) * resize.rsz2.bit.VRSZM) / resize.rsz2.bit.VRSZN);
				Im_DISP_Set_Display_Position(E_IM_DISP_LCD_MIPI, position);

				BF_Play_Common_Disp_Set_Int_Callback(FALSE, 0, E_IM_DISP_WAIT_1_TIME);
			}
			// Display next image.
			BF_Play_Macro_Wrap_Display( E_IM_DISP_LCD_MIPI, &image_addr );
		}
	}
}

/**
 * @brief		set xch parameter
 * @param		i:T_BF_PLAY_MACRO_WRAP_YCC_COPY_PARAM* copy_param
 * @param		o:T_IM_XCH_COPY_EX* xch_param_0
 * @param		o:T_IM_XCH_COPY_EX* xch_param_1
 * @param		i:E_BF_PLAY_MACRO_WRAP_USED_CH ch_mode
 * @return		VOID
 * @note		None
 * @attention	None
 */
static VOID play_macro_wrap_set_xch_param( T_BF_PLAY_MACRO_WRAP_YCC_COPY_PARAM* copy_param, T_IM_XCH_COPY_EX* xch_param_0, T_IM_XCH_COPY_EX* xch_param_1, E_BF_PLAY_MACRO_WRAP_USED_CH ch_mode )
{
	if ( ((copy_param->y.src_addr & 0xF) == 0) && ((copy_param->y.dst_addr & 0xF) == 0) && ((copy_param->y.width & 0xF) == 0) ) {
		xch_param_0->copy_type		= E_IM_XCH_COPY_SEL_HI;
	}
	else {
		xch_param_0->copy_type		= E_IM_XCH_COPY_SEL_NORMAL;
	}
	
	xch_param_0->xch				= E_IM_XCH_CH_SEL_0;
	xch_param_0->size.width			= copy_param->y.width;
	xch_param_0->size.lines			= copy_param->y.lines;
	xch_param_0->size.src_gl_width	= copy_param->y.src_gl_width;
	xch_param_0->size.dst_gl_width	= copy_param->y.dst_gl_width;
	xch_param_0->addr.src			= copy_param->y.src_addr;
	xch_param_0->addr.dst			= copy_param->y.dst_addr;

	if ( ((copy_param->c.src_addr & 0xF) == 0) && ((copy_param->c.dst_addr & 0xF) == 0) && ((copy_param->c.width & 0xF) == 0) ) {
		xch_param_1->copy_type		= E_IM_XCH_COPY_SEL_HI;
	}
	else {
		xch_param_1->copy_type		= E_IM_XCH_COPY_SEL_NORMAL;
	}

	if (ch_mode == E_BF_PLAY_MACRO_WRAP_USED_1CH) {
		xch_param_1->xch			= E_IM_XCH_CH_SEL_0;
	}
	else {
		xch_param_1->xch			= E_IM_XCH_CH_SEL_1;
	}
	xch_param_1->size.width			= copy_param->c.width;
	xch_param_1->size.lines			= copy_param->c.lines;
	xch_param_1->size.src_gl_width	= copy_param->c.src_gl_width;
	xch_param_1->size.dst_gl_width	= copy_param->c.dst_gl_width;
	xch_param_1->addr.src			= copy_param->c.src_addr;
	xch_param_1->addr.dst			= copy_param->c.dst_addr;

	return;
}

// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief		Get JPEG Main data
 * @param		i:ULONG image_index								image index
 * @param		i:UCHAR* load_addr								load image address
 * @param		i:UCHAR* decode_addr							decode image address
 * @param		o:T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng		jpeg decode information
 * @param		o:T_BF_DCF_IF_IMAGE_DETAIL* img_detail			file detail information
 * @param		o:T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data	load image information
 * @param		i:BOOL dec_flag									decode flag
 * @param		i:BOOL bDetect_mode_change						Detect mode change
 * @return		FW_INT32
 * @note		None
 * @attention	None
 */
static FW_INT32 play_macro_wrap_get_jpeg_main(ULONG image_index, UCHAR* load_addr, UCHAR* decode_addr, T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data, BOOL dec_flag, BOOL bDetect_mode_change)
{
	INT32		ret;
	ULONG		load_size;
	FW_USHORT	decode_width;
	FW_USHORT	decode_lines;

	memset( jpgmng,          0, sizeof(T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG) );
	memset( img_detail,      0, sizeof(T_BF_DCF_IF_IMAGE_DETAIL) );
	memset( load_image_data, 0, sizeof(T_BF_DCF_IF_LOAD_IMAGE_DATA) );

	/* get current exif information */
	ret = BF_Dcf_If_Get_Exif_Info_Jpeg( image_index, img_detail, load_image_data );
	if (ret != 0) {
		if (ret != FSDCF_ERRNO_NOT_APP1) {	// exif format check
			BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_main(): BF_Dcf_If_Get_Exif_Info_Jpeg err\n"));
			return -1;
		}
	}
	
	/****************************************/
	/* Check jpeg sub-sampling (YCC format) */
	/****************************************/
	if (img_detail->rotate >= E_BF_DCF_IF_ROTATE_0) {	// Not JFIF format
		if (img_detail->jpeg_format == E_BF_DCF_IF_JPEG_FORMAT_EXIF) {	// Exif format exist
			if ((img_detail->ycc_format != 0x21) && (img_detail->ycc_format != 0x22)) {	// 0x21=YCC422, 0x22=YCC420
				BF_Debug_Print_Information(("play_macro_wrap_get_jpeg_main(): jpeg sub-sampling err\n"));
				return -1;
			}
		}
	}

	/***************************/
	/* JPEG Image Load Process */
	/***************************/
	/* event flg clear */
	OS_User_Clr_Flg( FID_IMAGE_PLAY, ~D_BF_PLAY_COMMON_MAIN_LOADENG );

	if (!dec_flag) {
		load_size = SDRAM_SIZ_PLAY_IMAGE_LOAD_AREA;
	}
	else {
#ifdef CO_BF_DCF_IF_MPF_ENABLE
		if (img_detail->baseline_mp_flag == 0) {
#endif
			load_size = SDRAM_SIZ_PLAY_IMAGE_LOAD_AREA;
#ifdef CO_BF_DCF_IF_MPF_ENABLE
		}
		else {
			load_size = load_image_data->main_size;
		}
#endif
	}

	ret = BF_Dcf_If_Start_Div_Load_Jpeg( BF_Play_Common_Load_Result_Entry, image_index, (ULONG)load_addr, load_size);
	if (ret != D_OS_USER_E_OK) {
		BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_main(): BF_Dcf_If_Start_Div_Load_Jpeg err.\n"));
		return -1;
	}

	/* wait loading end */
	for (;;) {
		OS_User_Tslp_Tsk(10);

		if (BF_Play_Common_Load_End(&ret, &(jpgmng->code_size)) != D_BF_PLAY_COMMON_DIVLOAD_CONTINUE) {		// check loading end
			break;
		}
		if (bDetect_mode_change){
//			if (BF_Control_Is_Iif_Request() == 1) {		// check next event
//				BF_Dcf_If_Stop_Div_Load_Image();
//				return D_BF_PLAY_JPEG_LOAD_STOP;
//			}
		}
	}

	/* complete jpeg main data loading */
	ret = BF_Dcf_If_Get_Div_Load_Image_Result( &(jpgmng->code_size) );
	if (ret != 0) {
		BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_main(): BF_Dcf_If_Get_Div_Load_Image_Result err. errno=%d\n", ret));
		return -1;
	}

	/* not decode, return */
	if (!dec_flag) {
		return D_OS_USER_E_OK;
	}

	/****************************/
	/* jpeg data decode process */
	/****************************/
	decode_width          = BF_Play_Common_RoundUp_Bit_32(img_detail->main_width);
	decode_lines          = BF_Play_Common_RoundUp_Bit_16(img_detail->main_lines);
	jpgmng->code_addr     = (ULONG)load_addr;	
	jpgmng->ycbcr_addr[0] = (ULONG)decode_addr;
	jpgmng->ycbcr_addr[1] = jpgmng->ycbcr_addr[0] + decode_width * decode_lines;
	jpgmng->gl_width      = decode_width;
	jpgmng->lines         = decode_lines;
	jpgmng->ycbcr_smpl    = E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422;
	jpgmng->mem_format	  = E_BF_PLAY_MACRO_WRAP_JPG_MFMT_SEMI_PLANAR;

	/* JPEG Image Convert */
	// decompression JPEG data to YCbCr
	if (BF_Play_Macro_Wrap_Decode_Jpeg( jpgmng, D_BF_PLAY_MACRO_WRAP_WAIT_TIME ) != 0) {
		return -1;
	}

	return D_OS_USER_E_OK;
}

/**
 * @brief		Get JPEG Thumbnail data
 * @param		i:ULONG image_index								image index
 * @param		i:UCHAR* load_addr								load image address
 * @param		i:UCHAR* decode_addr							decode image address
 * @param		o:T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng		jpeg decode information
 * @param		o:T_BF_DCF_IF_IMAGE_DETAIL* img_detail			file detail information
 * @param		o:T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data	load image information
 * @param		i:BOOL dec_flag									decode flag
 * @return		FW_INT32
 * @note		None
 * @attention	None
 */
static FW_INT32 play_macro_wrap_get_jpeg_thumbnail(ULONG image_index, UCHAR* load_addr, UCHAR* decode_addr, T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data, BOOL dec_flag)
{	
	INT32	ret;

	memset( jpgmng,          0, sizeof(T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG) );
	memset( img_detail,      0, sizeof(T_BF_DCF_IF_IMAGE_DETAIL) );
	memset( load_image_data, 0, sizeof(T_BF_DCF_IF_LOAD_IMAGE_DATA) );

	/* get current exif information */
	ret = BF_Dcf_If_Get_Exif_Info_Jpeg( image_index, img_detail, load_image_data );
	if (ret != 0) {
		if (ret != FSDCF_ERRNO_NOT_APP1) {	// exif format check
			BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_thumbnail(): BF_Dcf_If_Get_Exif_Info_Jpeg err\n"));
			return -1;
		}
	}

	/****************************************/
	/* Check jpeg sub-sampling (YCC format) */
	/****************************************/
	if( img_detail->ycc_format_thum != 0x21 && img_detail->ycc_format_thum != 0x22 ){	// 0x21=YCC422
		BF_Debug_Print_Information(("play_macro_wrap_get_jpeg_thumbnail(): jpeg sub-sampling err\n"));
		return -1;
	}
	
	/*******************************/
	/* JPEG Thumbnail Load Process */
	/*******************************/
	if (load_image_data->thumb_offset != 0) {
		ret = BF_Dcf_If_Load_Thumb_Jpeg( image_index, (ULONG)load_addr, load_image_data->thumb_size, &(jpgmng->code_size), img_detail );
		if (ret != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_thumbnail(): BF_Dcf_If_Load_Thumb_Jpeg err.\n"));
			return -1;
		}
	}
	else {
		BF_Debug_Print_Information(("play_macro_wrap_get_jpeg_thumbnail(): Thumbnail Image Not Exist.\n"));
		return -1;
	}

	/* not decode, return */
	if (!dec_flag) {
		return D_OS_USER_E_OK;
	}

	/****************************/
	/* jpeg data decode process */
	/****************************/
	
	jpgmng->gl_width		= (USHORT)(BF_Play_Common_RoundUp_Bit_16((ULONG)WIDTH_THUMB));
	jpgmng->gl_lines		= (USHORT)(BF_Play_Common_RoundUp_Bit_16((ULONG)LINES_THUMB));
	jpgmng->code_addr		= M_SDRAM_MAP_COMMON_ADR_LOG2PHY((ULONG)load_addr);
	jpgmng->ycbcr_addr[0]   = (ULONG)decode_addr;
	jpgmng->ycbcr_addr[1]   = (ULONG)(jpgmng->ycbcr_addr[0] + jpgmng->gl_width * jpgmng->gl_lines);
	jpgmng->width			= (USHORT)WIDTH_THUMB;
	jpgmng->lines           = (USHORT)LINES_THUMB;

	if( img_detail->ycc_format_thum == 0x21 ) {
		jpgmng->ycbcr_smpl		= E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422;
	}
	else if( img_detail->ycc_format_thum == 0x22 ) {
		jpgmng->ycbcr_smpl		= E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420;
	}

	jpgmng->mem_format		= E_BF_PLAY_MACRO_WRAP_JPG_MFMT_SEMI_PLANAR;

	/* JPEG Thumbnail Convert Process */
	/* decode JPEG data */
	if (BF_Play_Macro_Wrap_Decode_Jpeg( jpgmng, 5 ) != 0){
		return -1;
	}

	return D_OS_USER_E_OK;
}

/**
 * @brief		Get JPEG Class1/Class2 data
 * @param		i:UCHAR cls_mode								class mode
 * @param		i:ULONG image_index								image index
 * @param		i:UCHAR* load_addr								load image address
 * @param		i:UCHAR* decode_addr							decode image address
 * @param		o:T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng		jpeg decode information
 * @param		o:T_BF_DCF_IF_IMAGE_DETAIL* img_detail			file detail information
 * @param		o:T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data	load image information
 * @param		i:BOOL dec_flag									decode flag
 * @param		i:BOOL bDetect_mode_change						Detect mode change
 * @return		FW_INT32
 * @note		None
 * @attention	None
 */
static FW_INT32 play_macro_wrap_get_jpeg_class(UCHAR cls_mode, ULONG image_index, UCHAR* load_addr, UCHAR* decode_addr, T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data, BOOL dec_flag, BOOL bDetect_mode_change)
{
	INT32	ret;

	memset( jpgmng,          0, sizeof(T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG) );
	memset( img_detail,      0, sizeof(T_BF_DCF_IF_IMAGE_DETAIL) );
	memset( load_image_data, 0, sizeof(T_BF_DCF_IF_LOAD_IMAGE_DATA) );

	/* get current exif information */
	ret = BF_Dcf_If_Get_Exif_Info_Jpeg( image_index, img_detail, load_image_data );
	if (ret != 0) {
		if (ret != FSDCF_ERRNO_NOT_APP1) {	// exif format check
			BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_class(): BF_Dcf_If_Get_Exif_Info_Jpeg err\n"));
			return -1;
		}
	}
	
	/****************************************/
	/* Check jpeg sub-sampling (YCC format) */
	/****************************************/
	if (cls_mode == FJ_PLAYBACK_IMAGE_SEL_CLS1) {
		if( (img_detail->class1_ycc_format != 0x21) && (img_detail->class1_ycc_format != 0x22) ){	// 0x21=YCC422 0x22=YCC420
			BF_Debug_Print_Information(("play_macro_wrap_get_jpeg_class(): jpeg sub-sampling err\n"));
			return -1;
		}
	}
	else {
		if( (img_detail->class2_ycc_format != 0x21) && (img_detail->class2_ycc_format != 0x22) ){	// 0x21=YCC422 0x22=YCC420
			BF_Debug_Print_Information(("play_macro_wrap_get_jpeg_class(): jpeg sub-sampling err\n"));
			return -1;
		}
	}

	/***************************/
	/* JPEG Image Load Process */
	/***************************/
	switch (cls_mode) {
#ifdef CO_BF_DCF_IF_MPF_ENABLE
		case FJ_PLAYBACK_IMAGE_SEL_CLS1:
			if (load_image_data->class1_offset != 0) {
				ret = BF_Dcf_If_Start_Div_Load_Monitor( BF_Play_Common_Load_Result_Entry, image_index,
												(ULONG)load_addr, load_image_data->class1_size, load_image_data->class1_offset );
				if (ret != D_OS_USER_E_OK) {
					BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_class(): BF_Dcf_If_Start_Div_Load_Monitor err.\n"));
					return -1;
				}
			}
			else {
				BF_Debug_Print_Information(("play_macro_wrap_get_jpeg_class(): Monitor Class1 Image Not Exist.\n"));
				return -1;
			}
			break;
			
		case FJ_PLAYBACK_IMAGE_SEL_CLS2:
			if (load_image_data->class2_offset != 0) {
				ret = BF_Dcf_If_Start_Div_Load_Monitor( BF_Play_Common_Load_Result_Entry, image_index,
												(ULONG)load_addr, load_image_data->class2_size, load_image_data->class2_offset );
 				if (ret != D_OS_USER_E_OK) {
					BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_class(): BF_Dcf_If_Start_Div_Load_Monitor err.\n"));
					return -1;
				}
			}
			else {
				BF_Debug_Print_Information(("play_macro_wrap_get_jpeg_class(): Monitor Class2 Image Not Exist.\n"));
				return -1;
			}
			break;

#endif
		default:
			BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_class(): system mode err\n"));
			return -1;
	}

	/* wait loading end */
	for (;;) {
		OS_User_Tslp_Tsk(10);

		if (BF_Play_Common_Load_End(&ret, &(jpgmng->code_size)) != D_BF_PLAY_COMMON_DIVLOAD_CONTINUE) {		// check loading end
			break;
		}
		if (bDetect_mode_change){
//			if (BF_Control_Is_Iif_Request() == 1) {		// check next event
//				BF_Dcf_If_Stop_Div_Load_Image();
//				return D_BF_PLAY_JPEG_LOAD_STOP;
//			}
		}
	}

	/* complete jpeg class data loading */
	ret = BF_Dcf_If_Get_Div_Load_Image_Result( &(jpgmng->code_size) );
	if (ret != 0) {
		BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_class(): BF_Dcf_If_Get_Div_Load_Image_Result err. errno=%d\n", ret));
		return -1;
	}

	/* not decode, return */
	if (!dec_flag) {
		return D_OS_USER_E_OK;
	}

	/****************************/
	/* jpeg data decode process */
	/****************************/
	jpgmng->code_addr		= (ULONG)load_addr;
	jpgmng->ycbcr_addr[0]	= (ULONG)decode_addr;
	jpgmng->ycbcr_addr[1]	= jpgmng->ycbcr_addr[0] + SDRAM_SIZ_PLAY_IMAGE_YCC_Y;
	jpgmng->gl_width		= PIX_SIZ_PLAY_DEFAULT_WIDTH;
	jpgmng->lines		    = PIX_SIZ_PLAY_DEFAULT_LINES;
	jpgmng->ycbcr_smpl		= E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422;
	jpgmng->mem_format		= E_BF_PLAY_MACRO_WRAP_JPG_MFMT_SEMI_PLANAR;

	/* JPEG Image Convert */
	/* decompression JPEG data to YCbCr */
	if (BF_Play_Macro_Wrap_Decode_Jpeg( jpgmng, D_BF_PLAY_MACRO_WRAP_WAIT_TIME ) != 0) {
		return -1;
	}

	return D_OS_USER_E_OK;
}

/**
 * @brief		Get JPEG Thumbnail Class1/Class2 data
 * @param		i:UCHAR cls_mode								class mode
 * @param		i:ULONG image_index								image index
 * @param		i:UCHAR* load_addr								load image address
 * @param		i:UCHAR* decode_addr							decode image address
 * @param		o:T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng		jpeg decode information
 * @param		o:T_BF_DCF_IF_IMAGE_DETAIL* img_detail			file detail information
 * @param		o:T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data	load image information
 * @param		i:BOOL dec_flag									decode flag
 * @return		error code
 * @note		None
 * @attention	None
 */
static FW_INT32 play_macro_wrap_get_jpeg_thm_class(UCHAR cls_mode, ULONG image_index, UCHAR* load_addr, UCHAR* decode_addr, T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data, BOOL dec_flag)
{
	INT32	ret;

	memset( jpgmng,          0, sizeof(T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG) );
	memset( img_detail,      0, sizeof(T_BF_DCF_IF_IMAGE_DETAIL) );
	memset( load_image_data, 0, sizeof(T_BF_DCF_IF_LOAD_IMAGE_DATA) );

	/* get current exif information */
	ret = BF_Dcf_If_Get_Exif_Info_Jpeg( image_index, img_detail, load_image_data );
	if (ret != 0) {
		if (ret != FSDCF_ERRNO_NOT_APP1) {	// exif format check
			BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_thm_class(): BF_Dcf_If_Get_Exif_Info_Jpeg err\n"));
			return -1;
		}
	}

	/****************************************/
	/* Check jpeg sub-sampling (YCC format) */
	/****************************************/
	if( img_detail->ycc_format_thum != 0x21 ){	// 0x21=YCC422
		BF_Debug_Print_Information(("play_macro_wrap_get_jpeg_thm_class(): jpeg sub-sampling err\n"));
		return -1;
	}

	/***************************/
	/* JPEG Image Load Process */
	/***************************/
	switch (cls_mode) {
#ifdef CO_BF_DCF_IF_MPF_ENABLE
		case FJ_PLAYBACK_IMAGE_SEL_CLS1:
			if (load_image_data->class1_thumb_offset != 0) {
				ret = BF_Dcf_If_Load_Monitor_Thumb( image_index, (ULONG)load_addr, load_image_data->class1_thumb_size, 
												load_image_data->class1_thumb_offset, &(jpgmng->code_size) );
				if (ret != D_OS_USER_E_OK) {
					BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_thm_class(): BF_Dcf_If_Load_Monitor_Thumb err.\n"));
					return -1;
				}
			}
			else {
				BF_Debug_Print_Information(("play_macro_wrap_get_jpeg_thm_class(): Monitor Class1 Thumbnail Image Not Exist.\n"));
				return -1;
			}
			break;
			
		case FJ_PLAYBACK_IMAGE_SEL_CLS2:
			if (load_image_data->class2_thumb_offset != 0) {
				ret = BF_Dcf_If_Load_Monitor_Thumb( image_index, (ULONG)load_addr, load_image_data->class2_thumb_size, 
												load_image_data->class2_thumb_offset, &(jpgmng->code_size) );
 				if (ret != D_OS_USER_E_OK) {
					BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_thm_class(): BF_Dcf_If_Load_Monitor_Thumb err.\n"));
					return -1;
				}
			}
			else {
				BF_Debug_Print_Information(("play_macro_wrap_get_jpeg_thm_class(): Monitor Class2 Thumbnail Image Not Exist.\n"));
				return -1;
			}
			break;

#endif
		default:
			BF_Debug_Print_Error(("play_macro_wrap_get_jpeg_thm_class(): system mode err\n"));
			return -1;
	}

	/* not decode, return */
	if (!dec_flag) {
		return D_OS_USER_E_OK;
	}

	/****************************/
	/* jpeg data decode process */
	/****************************/
	jpgmng->code_addr		= (ULONG)load_addr;
	jpgmng->ycbcr_addr[0]	= (ULONG)decode_addr;
	jpgmng->ycbcr_addr[1]	= jpgmng->ycbcr_addr[0] + PIX_SIZ_THUMB;
	jpgmng->gl_width		= WIDTH_THUMB;
	jpgmng->lines			= LINES_THUMB;
	jpgmng->ycbcr_smpl		= E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422;
	jpgmng->mem_format		= E_BF_PLAY_MACRO_WRAP_JPG_MFMT_SEMI_PLANAR;

	/* JPEG Thumbnail Convert Process */
	/* decode JPEG data */
	if (BF_Play_Macro_Wrap_Decode_Jpeg( jpgmng, 5 ) != 0){
		return -1;
	}

	return D_OS_USER_E_OK;
}

static BOOL play_macro_wrap_check_jpeg_thumbnail(ULONG image_index)
{	
	INT32	ret;
	T_BF_DCF_IF_IMAGE_DETAIL img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA load_image_data;

	memset( &img_detail,      0, sizeof(T_BF_DCF_IF_IMAGE_DETAIL) );
	memset( &load_image_data, 0, sizeof(T_BF_DCF_IF_LOAD_IMAGE_DATA) );

	ret = BF_Dcf_If_Get_Exif_Info_Jpeg( image_index, &img_detail, &load_image_data );

	if (ret != 0) {
		return FALSE;
	}

	if (load_image_data.thumb_offset == 0) {
		return FALSE;
	}

	return TRUE;
}
// --- REMOVE_DCF_EXIF END ---

//#pragma arm section code="BASEFW_COMM_CODE"

/**
 * @brief		set xch fill parameter
 * @param		UCHAR fill_data_y,  USHORT gl_width_y,  UINT32 dst_addr_y,  USHORT width_y,  USHORT lines_y,
 *				UCHAR fill_data_cb, USHORT gl_width_cb, UINT32 dst_addr_cb, USHORT width_cb, USHORT lines_cb,
 *				UCHAR fill_data_cr, USHORT gl_width_cr, UINT32 dst_addr_cr, USHORT width_cr, USHORT lines_cr,
 *				T_BF_PLAY_MACRO_WRAP_YCC_FILL_PARAM* fill_param
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID play_macro_wrap_set_fill_param( UCHAR fill_data_y, USHORT gl_width_y, UINT32 dst_addr_y, USHORT width_y, USHORT lines_y,
										UCHAR fill_data_cb, UCHAR fill_data_cr, USHORT gl_width_c, UINT32 dst_addr_c, USHORT width_c, USHORT lines_c,
										E_BF_PLAY_MACRO_WRAP_FILL fill_type, T_BF_PLAY_MACRO_WRAP_YCC_FILL_PARAM* fill_param )
{
	fill_param->fill_type		= fill_type;
	fill_param->fill_data_y		= fill_data_y;
	fill_param->y.gl_width		= gl_width_y;
	fill_param->y.dst_addr		= dst_addr_y;
	fill_param->y.width			= width_y;
	fill_param->y.lines			= lines_y;

	fill_param->fill_data_cb	= fill_data_cb;
	fill_param->fill_data_cr	= fill_data_cr;
	fill_param->c.gl_width		= gl_width_c;
	fill_param->c.dst_addr		= dst_addr_c;
	fill_param->c.width			= width_c;
	fill_param->c.lines			= lines_c;
	
	return;
}

/**
 * @brief		fill ycc image using one channel 
 * @param		i:T_BF_PLAY_MACRO_WRAP_YCC_FILL_PARAM* fill_param
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID play_macro_wrap_fill_ycc_1ch( T_BF_PLAY_MACRO_WRAP_YCC_FILL_PARAM* fill_param )
{
	INT32				ret;
	USHORT				cnt;
	T_IM_XCH_FILL_EX	xch_param;

	// Y
	if ((fill_param->fill_type == E_BF_PLAY_MACRO_WRAP_FILL_ALL) || (fill_param->fill_type == E_BF_PLAY_MACRO_WRAP_FILL_Y)) {

		xch_param.xch			= E_IM_XCH_CH_SEL_0;
		xch_param.fill_data		= fill_param->fill_data_y;
		xch_param.fill_addr		= fill_param->y.dst_addr;
		xch_param.size.width	= fill_param->y.width;
		xch_param.size.lines	= fill_param->y.lines;
		xch_param.size.gl_width	= fill_param->y.gl_width;

		ret = Im_Xch_Fill_Ex_Sync( &xch_param );
		if (ret != D_DDIM_OK) {
			BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Fill_Ycc: Im_Xch_Fill_Ex_Sync error. ret = %d\n", ret));
			return;
		}
	}

	// C
	if ((fill_param->fill_type == E_BF_PLAY_MACRO_WRAP_FILL_ALL) || (fill_param->fill_type == E_BF_PLAY_MACRO_WRAP_FILL_CC)) {

		if (fill_param->fill_data_cb == fill_param->fill_data_cr) {
			// Fill by the same data
			xch_param.xch			= E_IM_XCH_CH_SEL_0;
			xch_param.fill_data		= fill_param->fill_data_cb;
			xch_param.fill_addr		= fill_param->c.dst_addr;
			xch_param.size.width	= fill_param->c.width;
			xch_param.size.lines	= fill_param->c.lines;
			xch_param.size.gl_width	= fill_param->c.gl_width;

			ret = Im_Xch_Fill_Ex_Sync( &xch_param );
			if (ret != D_DDIM_OK) {
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Fill_Ycc: Im_Xch_Fill_Ex_Sync error. ret = %d\n", ret));
				return;
			}
		}
		else {
			// Fill by the same data
			xch_param.xch	= E_IM_XCH_CH_SEL_0;

			for (cnt = 0; cnt < fill_param->c.width; cnt += 2) {
				xch_param.fill_data		= fill_param->fill_data_cb;
				xch_param.fill_addr		= fill_param->c.dst_addr + cnt;
				xch_param.size.width	= 1;
				xch_param.size.lines	= fill_param->c.lines;
				xch_param.size.gl_width	= fill_param->c.gl_width;

				ret = Im_Xch_Fill_Ex_Sync( &xch_param );
				if (ret != D_DDIM_OK) {
					BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Fill_Ycc: Im_Xch_Fill_Ex_Sync error. ret = %d\n", ret));
					return;
				}

				xch_param.fill_data		= fill_param->fill_data_cr;
				xch_param.fill_addr		= fill_param->c.dst_addr + (cnt + 1);
				xch_param.size.width	= 1;
				xch_param.size.lines	= fill_param->c.lines;
				xch_param.size.gl_width	= fill_param->c.gl_width;

				ret = Im_Xch_Fill_Ex_Sync( &xch_param );
				if (ret != D_DDIM_OK) {
					BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Fill_Ycc: Im_Xch_Fill_Ex_Sync error. ret = %d\n", ret));
					return;
				}
			}
		}
	}
}

/**
 * @brief		fill ycc image using double channel 
 * @param		i:T_BF_PLAY_MACRO_WRAP_YCC_FILL_PARAM* fill_param
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID play_macro_wrap_fill_ycc_2ch( T_BF_PLAY_MACRO_WRAP_YCC_FILL_PARAM* fill_param )
{
	INT32				ret;
	USHORT				cnt;
	T_IM_XCH_FILL_EX	xch_param;

	// Y
	if ((fill_param->fill_type == E_BF_PLAY_MACRO_WRAP_FILL_ALL) || (fill_param->fill_type == E_BF_PLAY_MACRO_WRAP_FILL_Y)) {
		xch_param.xch			= E_IM_XCH_CH_SEL_0;
		xch_param.fill_data		= fill_param->fill_data_y;
		xch_param.fill_addr		= fill_param->y.dst_addr;
		xch_param.size.width	= fill_param->y.width;
		xch_param.size.lines	= fill_param->y.lines;
		xch_param.size.gl_width	= fill_param->y.gl_width;

		ret = Im_Xch_Open(E_IM_XCH_CH_SEL_0, D_DDIM_WAIT_END_FOREVER);
		if (ret != D_DDIM_OK) {
			// Im_Xch_Open error
			BF_Debug_Print_Error(("play_macro_wrap_fill_ycc_2ch: Im_Xch_Open error. ret = %d\n", ret));
			return;
		}

		ret = Im_Xch_Fill_Ex_Async( &xch_param, NULL );
		if (ret != D_DDIM_OK) {
			// Im_Xch_Open error
			Im_Xch_Close(E_IM_XCH_CH_SEL_0);
			BF_Debug_Print_Error(("play_macro_wrap_fill_ycc_2ch: Im_Xch_Fill_Ex_Async error. ret = %d\n", ret));
			return;
		}
	}

	// C
	if ((fill_param->fill_type == E_BF_PLAY_MACRO_WRAP_FILL_ALL) || (fill_param->fill_type == E_BF_PLAY_MACRO_WRAP_FILL_CC)) {

		while (1) {

			if (fill_param->fill_data_cb == fill_param->fill_data_cr) {
				xch_param.xch			= E_IM_XCH_CH_SEL_1;
				xch_param.fill_data		= fill_param->fill_data_cb;
				xch_param.fill_addr		= fill_param->c.dst_addr;
				xch_param.size.width	= fill_param->c.width;
				xch_param.size.lines	= fill_param->c.lines;
				xch_param.size.gl_width	= fill_param->c.gl_width;

				ret = Im_Xch_Fill_Ex_Sync( &xch_param );
				if (ret != D_DDIM_OK) {
					BF_Debug_Print_Error(("play_macro_wrap_fill_ycc_2ch: Im_Xch_Fill_Ex_Sync error. ret = %d\n", ret));
					break;
				}
			}
			else {
				xch_param.xch	= E_IM_XCH_CH_SEL_1;

				for (cnt = 0; cnt < fill_param->c.width; cnt += 2) {
					xch_param.fill_data		= fill_param->fill_data_cb;
					xch_param.fill_addr		= fill_param->c.dst_addr + cnt;
					xch_param.size.width	= 1;
					xch_param.size.lines	= fill_param->c.lines;
					xch_param.size.gl_width	= fill_param->c.gl_width;

					ret = Im_Xch_Fill_Ex_Sync( &xch_param );
					if (ret != D_DDIM_OK) {
						BF_Debug_Print_Error(("play_macro_wrap_fill_ycc_2ch: Im_Xch_Fill_Ex_Sync error. ret = %d\n", ret));
						break;
					}

					xch_param.fill_data		= fill_param->fill_data_cr;
					xch_param.fill_addr		= fill_param->c.dst_addr + (cnt + 1);
					xch_param.size.width	= 1;
					xch_param.size.lines	= fill_param->c.lines;
					xch_param.size.gl_width	= fill_param->c.gl_width;

					ret = Im_Xch_Fill_Ex_Sync( &xch_param );
					if (ret != D_DDIM_OK) {
						BF_Debug_Print_Error(("play_macro_wrap_fill_ycc_2ch: Im_Xch_Fill_Ex_Sync error. ret = %d\n", ret));
						break;
					}
				}
			}
			break;
		}
	}

	// for Y
	if ((fill_param->fill_type == E_BF_PLAY_MACRO_WRAP_FILL_ALL) || (fill_param->fill_type == E_BF_PLAY_MACRO_WRAP_FILL_Y)) {
		ret = Im_Xch_Wait_End( E_IM_XCH_CH_SEL_0 );
		if (ret != D_DDIM_OK) {
			BF_Debug_Print_Error(("play_macro_wrap_fill_ycc_2ch: Im_Xch_Wait_End error. ret = %d\n", ret));
		}
		Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
	}
	return;
}

/**
 * @brief		set xch copy parameter
 * @param		UINT32 src_addr_y, USHORT src_gl_width_y, USHORT dst_gl_width_y, UINT32 dst_addr_y, USHORT width_y, USHORT lines_y,
 *				UINT32 src_addr_c, USHORT src_gl_width_c, USHORT dst_gl_width_c, UINT32 dst_addr_c, USHORT width_c, USHORT lines_c,
 *				T_BF_PLAY_MACRO_WRAP_YCC_COPY_PARAM* copy_param
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID play_macro_wrap_set_copy_param( UINT32 src_addr_y, USHORT src_gl_width_y, USHORT dst_gl_width_y, UINT32 dst_addr_y, USHORT width_y, USHORT lines_y,
											UINT32 src_addr_c, USHORT src_gl_width_c, USHORT dst_gl_width_c, UINT32 dst_addr_c, USHORT width_c, USHORT lines_c,
											T_BF_PLAY_MACRO_WRAP_YCC_COPY_PARAM* copy_param )
{
	copy_param->y.src_addr		= src_addr_y;
	copy_param->y.src_gl_width	= src_gl_width_y;
	copy_param->y.dst_gl_width	= dst_gl_width_y;
	copy_param->y.dst_addr		= dst_addr_y;
	copy_param->y.width			= width_y;
	copy_param->y.lines			= lines_y;

	copy_param->c.src_addr		= src_addr_c;
	copy_param->c.src_gl_width	= src_gl_width_c;
	copy_param->c.dst_gl_width	= dst_gl_width_c;
	copy_param->c.dst_addr		= dst_addr_c;
	copy_param->c.width			= width_c;
	copy_param->c.lines			= lines_c;

	return;
}

/**
 * @brief		copy ycc image
 * @param		i:T_BF_PLAY_MACRO_WRAP_YCC_COPY_PARAM* copy_param
 * @param		i:E_BF_PLAY_MACRO_WRAP_USED_CH ch_mode
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID play_macro_wrap_copy_ycc( T_BF_PLAY_MACRO_WRAP_YCC_COPY_PARAM* copy_param, E_BF_PLAY_MACRO_WRAP_USED_CH ch_mode )
{
	INT32 ret;
	T_IM_XCH_COPY_EX xch_param_0;
	T_IM_XCH_COPY_EX xch_param_1;

	play_macro_wrap_set_xch_param( copy_param, &xch_param_0, &xch_param_1, ch_mode );
	
	if (ch_mode == E_BF_PLAY_MACRO_WRAP_USED_1CH) {
		if ((xch_param_0.size.width != 0) && (xch_param_0.size.lines != 0)) {
			ret = Im_Xch_Open(E_IM_XCH_CH_SEL_0, D_DDIM_WAIT_END_FOREVER);
			if (ret != D_DDIM_OK) {
				// Im_Xch_Open error
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Copy_Ycc: Im_Xch_Open error. ret = %d\n", ret));
				return;
			}

			ret = Im_Xch_Copy_Ex_Async( &xch_param_0, NULL );
			if (ret != D_DDIM_OK) {
				// Im_Xch_Open error
				Im_Xch_Wait_End(E_IM_XCH_CH_SEL_0);
				Im_Xch_Close(E_IM_XCH_CH_SEL_0);
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Copy_Ycc: Im_Xch_Copy_Ex_Async error. ret = %d\n", ret));
				return;
			}

			Im_Xch_Wait_End(E_IM_XCH_CH_SEL_0);
			Im_Xch_Close(E_IM_XCH_CH_SEL_0);

			if ((xch_param_1.size.width != 0) && (xch_param_1.size.lines != 0)) {
				ret = Im_Xch_Open(E_IM_XCH_CH_SEL_0, D_DDIM_WAIT_END_FOREVER);
				if (ret != D_DDIM_OK) {
					// Im_Xch_Open error
					BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Copy_Ycc: Im_Xch_Open error. ret = %d\n", ret));
					return;
				}

				ret = Im_Xch_Copy_Ex_Async( &xch_param_1, NULL );
				if (ret != D_DDIM_OK) {
					// Im_Xch_Open error
					Im_Xch_Wait_End(E_IM_XCH_CH_SEL_0);
					Im_Xch_Close(E_IM_XCH_CH_SEL_0);
					BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Copy_Ycc: Im_Xch_Copy_Ex_Async error. ret = %d\n", ret));
					return;
				}

				Im_Xch_Wait_End(E_IM_XCH_CH_SEL_0);
				Im_Xch_Close(E_IM_XCH_CH_SEL_0);
			}
		}
	} 
	else { //E_BF_PLAY_MACRO_WRAP_USED_2CH
		if ((xch_param_0.size.width != 0) && (xch_param_0.size.lines != 0)) {
			ret = Im_Xch_Open(E_IM_XCH_CH_SEL_0, D_DDIM_WAIT_END_FOREVER);
			if (ret != D_DDIM_OK) {
				// Im_Xch_Open error
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Copy_Ycc: Im_Xch_Open error. ret = %d\n", ret));
				return;
			}

			ret = Im_Xch_Copy_Ex_Async( &xch_param_0, NULL );
			if (ret != D_DDIM_OK) {
				// Im_Xch_Open error
				Im_Xch_Wait_End(E_IM_XCH_CH_SEL_0);
				Im_Xch_Close(E_IM_XCH_CH_SEL_0);
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Copy_Ycc: Im_Xch_Copy_Ex_Async error. ret = %d\n", ret));
				return;
			}

			if ((xch_param_1.size.width != 0) && (xch_param_1.size.lines != 0)) {
				ret = Im_Xch_Open(E_IM_XCH_CH_SEL_1, D_DDIM_WAIT_END_FOREVER);
				if (ret != D_DDIM_OK) {
					// Im_Xch_Open error
					Im_Xch_Wait_End(E_IM_XCH_CH_SEL_0);
					Im_Xch_Close(E_IM_XCH_CH_SEL_0);
					BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Copy_Ycc: Im_Xch_Open error. ret = %d\n", ret));
					return;
				}

				ret = Im_Xch_Copy_Ex_Async( &xch_param_1, NULL );
				if (ret != D_DDIM_OK) {
					// Im_Xch_Open error
					Im_Xch_Wait_End(E_IM_XCH_CH_SEL_0);
					Im_Xch_Close(E_IM_XCH_CH_SEL_0);
					Im_Xch_Wait_End(E_IM_XCH_CH_SEL_1);
					Im_Xch_Close(E_IM_XCH_CH_SEL_1);
					BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Copy_Ycc: Im_Xch_Copy_Ex_Async error. ret = %d\n", ret));
					return;
				}
				
				Im_Xch_Wait_End(E_IM_XCH_CH_SEL_1);
				Im_Xch_Close(E_IM_XCH_CH_SEL_1);
			}

			Im_Xch_Wait_End(E_IM_XCH_CH_SEL_0);
			Im_Xch_Close(E_IM_XCH_CH_SEL_0);
		}
	}
}

/******************************/
/**  Last Section(FastBoot)  **/
/******************************/
//#pragma arm section code="BASEFW_LAST_CODE"
#ifdef CO_BF_PLAY_MACRO_WRAP_JPEG_ASYNC
/**
 * @brief		JPEG Decode Callback function(asynchronous)
 * @param		i:T_IM_JPEG_DEC_INFO* jpg_dec_mng
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID play_macro_wrap_decode_jpeg_callback(T_IM_JPEG_DEC_INFO* jpg_dec_mng)
{
	// The processing result of JPEG Decode is succeeded from IntHandler.
	memcpy(&gPlay_JPEG_Decode_Mng, jpg_dec_mng, sizeof(T_IM_JPEG_DEC_INFO));

	// End of callback function waiting
	gPlay_JPEG_Decode_Wait = 0;
}
#endif

// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief 		playback save SOF0 width change process
 * @param		i:ULONG		jpg_addr	: jpeg addr
 * @param		i:ULONG		jpg_size	: jpeg code size
 * @param		i:USHORT	width		: original width
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
#if 0
static FJ_ERR_CODE play_macro_wrap_change_sof_width( ULONG jpg_addr, ULONG jpg_size, USHORT width )
{
	FJ_ERR_CODE	ercd = FJ_ERR_OK;
	USHORT		tag_2_byte;
	UCHAR*		main_jpg_addr_1_byte;
	ULONG		jpg_tag_count = 0;
	USHORT		tmpWidth;

	main_jpg_addr_1_byte = (UCHAR*)jpg_addr;
#ifdef CO_BF_PLAY_MACRO_WRAP_ENDIAN_BIG
	memcpy(&tag_2_byte, main_jpg_addr_1_byte, 2);
#else
	tag_2_byte = (((USHORT)main_jpg_addr_1_byte[0]) << 8) | (main_jpg_addr_1_byte[1]);
	BF_Debug_Print_Information(( "tag_2_byte = 0x%x\nmain_jpg_addr_1_byte[0] = 0x%x, main_jpg_addr_1_byte[1] = 0x%x\n", tag_2_byte, main_jpg_addr_1_byte[0], main_jpg_addr_1_byte[1] ));
#endif

	if(tag_2_byte == 0xFFD8) {
		main_jpg_addr_1_byte += 2;
		jpg_tag_count += 2;
		while(1) {
#ifdef CO_BF_PLAY_MACRO_WRAP_ENDIAN_BIG
			memcpy(&tag_2_byte, main_jpg_addr_1_byte, 2);
#else
			tag_2_byte = (((USHORT)main_jpg_addr_1_byte[0]) << 8) | (main_jpg_addr_1_byte[1]);
			BF_Debug_Print_Information(( "tag_2_byte = 0x%x\n", tag_2_byte ));
#endif
			jpg_tag_count += 2;
			if(tag_2_byte == 0xFFC0) {
				main_jpg_addr_1_byte += 7;
#ifdef CO_BF_PLAY_MACRO_WRAP_ENDIAN_BIG
				tmpWidth = width;
#else
				tmpWidth = ((width & 0x00FF) << 8) + (width >> 8);
				BF_Debug_Print_Information(( "tmpWidth = 0x%x\n", tmpWidth ));
#endif
				memcpy(main_jpg_addr_1_byte, &tmpWidth, 2);
				break;
			}
			else {
				main_jpg_addr_1_byte += 2;
				jpg_tag_count += 2;
#ifdef CO_BF_PLAY_MACRO_WRAP_ENDIAN_BIG
				memcpy(&tag_2_byte, main_jpg_addr_1_byte, 2);
#else
				tag_2_byte = (((USHORT)main_jpg_addr_1_byte[0]) << 8) | (main_jpg_addr_1_byte[1]);
				BF_Debug_Print_Information(( "tag_2_byte = 0x%x\n", tag_2_byte ));
#endif

				main_jpg_addr_1_byte += tag_2_byte;
				jpg_tag_count += tag_2_byte;
			}
			if(jpg_tag_count >  jpg_size) {
				ercd = FJ_ERR_NG;
				break;
			}
		}
	}
	else {
		BF_Debug_Print_Error(( "Jpeg create Error!\n" ));
		ercd = FJ_ERR_NG;
	}

	return ercd;

}
#endif
// --- REMOVE_DCF_EXIF END ---

/**
 * @brief		JPEG Image Information setting
 * @note		None
 * @attention	None
 */
#if 0
static FJ_ERR_CODE play_macro_wrap_set_decode_jpeg_size_info( T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG *jpg_dec_mng, T_IM_JPEG_DEC_ADDR_IMG* jpg_dec_addr_img )
{

	jpg_dec_addr_img->bar_p1		= jpg_dec_mng->ycbcr_addr[0];
	jpg_dec_addr_img->bar_p2		= jpg_dec_mng->ycbcr_addr[1];
	jpg_dec_addr_img->bar_p3		= jpg_dec_mng->ycbcr_addr[2];

	// Check the 8 aligned
	if ( (jpg_dec_mng->gl_width & 0x7) != 0 ) {
		jpg_dec_mng->gl_width = ( jpg_dec_mng->gl_width & 0x7 ) + 0x8;
	}
	if ( (jpg_dec_mng->gl_lines & 0x7) != 0 ) {
		jpg_dec_mng->gl_lines = ( jpg_dec_mng->gl_lines & 0x7 ) + 0x8;
	}

	switch ( jpg_dec_mng->mem_format ) {
		case E_BF_PLAY_MACRO_WRAP_JPG_MFMT_PLANAR:

			jpg_dec_addr_img->line_ofs_p1	= jpg_dec_mng->gl_width;

			if ( jpg_dec_mng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC444 ) {
				jpg_dec_addr_img->line_ofs_p2	= jpg_dec_mng->gl_width;
				jpg_dec_addr_img->line_ofs_p3	= jpg_dec_mng->gl_width;
				jpg_dec_addr_img->siz_p1		= jpg_dec_addr_img->line_ofs_p1 * jpg_dec_mng->gl_lines;
				jpg_dec_addr_img->siz_p2		= jpg_dec_addr_img->line_ofs_p2 * jpg_dec_mng->gl_lines;
				jpg_dec_addr_img->siz_p3		= jpg_dec_addr_img->line_ofs_p3 * jpg_dec_mng->gl_lines;
			}
			else if ( jpg_dec_mng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422 ) {
				jpg_dec_addr_img->line_ofs_p2	= (jpg_dec_mng->gl_width >> 1);
				jpg_dec_addr_img->line_ofs_p3	= (jpg_dec_mng->gl_width >> 1);
				jpg_dec_addr_img->siz_p1		= jpg_dec_addr_img->line_ofs_p1 * jpg_dec_mng->gl_lines;
				jpg_dec_addr_img->siz_p2		= jpg_dec_addr_img->line_ofs_p2 * jpg_dec_mng->gl_lines;
				jpg_dec_addr_img->siz_p3		= jpg_dec_addr_img->line_ofs_p3 * jpg_dec_mng->gl_lines;

			}
			else if ( jpg_dec_mng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420 ) {
				jpg_dec_addr_img->line_ofs_p2	= (jpg_dec_mng->gl_width >> 1);
				jpg_dec_addr_img->line_ofs_p3	= (jpg_dec_mng->gl_width >> 1);
				jpg_dec_addr_img->siz_p1		= jpg_dec_addr_img->line_ofs_p1 * (jpg_dec_mng->gl_lines >> 1);
				jpg_dec_addr_img->siz_p2		= jpg_dec_addr_img->line_ofs_p2 * (jpg_dec_mng->gl_lines >> 1);
				jpg_dec_addr_img->siz_p3		= jpg_dec_addr_img->line_ofs_p3 * (jpg_dec_mng->gl_lines >> 1);
			}
			else {	// 400
				jpg_dec_addr_img->line_ofs_p2	= 0;
				jpg_dec_addr_img->line_ofs_p3	= 0;
				jpg_dec_addr_img->siz_p1		= jpg_dec_addr_img->line_ofs_p1 * jpg_dec_mng->gl_lines;
				jpg_dec_addr_img->siz_p2		= 0;
				jpg_dec_addr_img->siz_p3		= 0;
			}
			break;

		case E_BF_PLAY_MACRO_WRAP_JPG_MFMT_SEMI_PLANAR:
			jpg_dec_addr_img->line_ofs_p1	= jpg_dec_mng->gl_width;

			if ( jpg_dec_mng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC444 ) {
				jpg_dec_addr_img->line_ofs_p2	= (jpg_dec_mng->gl_width << 1);
				jpg_dec_addr_img->line_ofs_p3	= 0;
				jpg_dec_addr_img->siz_p1		= jpg_dec_addr_img->line_ofs_p1 * jpg_dec_mng->gl_lines;
				jpg_dec_addr_img->siz_p2		= jpg_dec_addr_img->line_ofs_p2 * jpg_dec_mng->gl_lines;
				jpg_dec_addr_img->siz_p3		= 0;
			}
			else if ( jpg_dec_mng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422 ) {
				jpg_dec_addr_img->line_ofs_p2	= jpg_dec_mng->gl_width;
				jpg_dec_addr_img->line_ofs_p3	= 0;
				jpg_dec_addr_img->siz_p1		= jpg_dec_addr_img->line_ofs_p1 * jpg_dec_mng->gl_lines;
				jpg_dec_addr_img->siz_p2		= jpg_dec_addr_img->line_ofs_p2 * jpg_dec_mng->gl_lines;
				jpg_dec_addr_img->siz_p3		= 0;

			}
			else if ( jpg_dec_mng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420 ) {
				jpg_dec_addr_img->line_ofs_p2	= jpg_dec_mng->gl_width;
				jpg_dec_addr_img->line_ofs_p3	= 0;
				jpg_dec_addr_img->siz_p1		= jpg_dec_addr_img->line_ofs_p1 * (jpg_dec_mng->gl_lines >> 1);
				jpg_dec_addr_img->siz_p2		= jpg_dec_addr_img->line_ofs_p2 * (jpg_dec_mng->gl_lines >> 1);
				jpg_dec_addr_img->siz_p3		= 0;
			}
			else {	// 400
				jpg_dec_addr_img->line_ofs_p2	= 0;
				jpg_dec_addr_img->line_ofs_p3	= 0;
				jpg_dec_addr_img->siz_p1		= jpg_dec_addr_img->line_ofs_p1 * jpg_dec_mng->gl_lines;
				jpg_dec_addr_img->siz_p2		= 0;
				jpg_dec_addr_img->siz_p3		= 0;
			}
			break;

		case E_BF_PLAY_MACRO_WRAP_JPG_MFMT_TILE:
			if ( jpg_dec_mng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422 ) {
				jpg_dec_addr_img->siz_p1		= ( (jpg_dec_mng->gl_width + 15) / 16 * 512 ) * ( (jpg_dec_mng->gl_lines + 15) / 16 );
			}
			else {
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Decode_Jpeg(): Memory Format error.\n"));
				return FJ_ERR_NG;
			}
			break;

		default:
			BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Decode_Jpeg(): Memory Format error.\n"));
			return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}
#endif


/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
/******************************/
/** Common Section(FastBoot) **/
/******************************/
//#pragma arm section code="BASEFW_BOOT_CODE"
/**
 * @brief		decode Jpeg file function
 * @param		i/o: T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG*	jpg_dec_mng
 * @param		i: INT32 tout
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE	BF_Play_Macro_Wrap_Decode_Jpeg( T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpg_dec_mng, INT32 tout )
{
	INT32					drv_ret	= 0;
	FJ_ERR_CODE				result	= FJ_ERR_NG;
    T_IM_JPEG_DEC_MNG       jpgmng;
    T_IM_JPEG_DEC_FRAME_MNG jpg_frm_mng;
//	UCHAR 					loop_cnt;

	Dd_Top_Set_CLKSTOP3_JPGAH( 0 );
	Dd_Top_Set_CLKSTOP3_JPGAX( 0 );
	Dd_Top_Set_CLKSTOP3_JPGCK( 0 );

	// Check parameter
	if(jpg_dec_mng == NULL) {
		return result;
	}

	// Initialize variable
    memset( &jpgmng, 0, sizeof(T_IM_JPEG_DEC_MNG) );
    memset( &jpg_frm_mng, 0, sizeof(T_IM_JPEG_DEC_FRAME_MNG) );


	// Set the parameter
	// Size overwrite disable
    jpg_frm_mng.global_y_width  = jpg_dec_mng->gl_width;
    jpg_frm_mng.global_c_width  = jpg_dec_mng->gl_width;
    jpg_frm_mng.cutout_flg      = D_IM_JPEG_ENABLE_OFF;
    jpg_frm_mng.ycc_addr.y      = jpg_dec_mng->ycbcr_addr[0];   // Y
    jpg_frm_mng.ycc_addr.c      = jpg_dec_mng->ycbcr_addr[1];   // C
    jpg_frm_mng.code_addr       = jpg_dec_mng->code_addr;
    jpg_frm_mng.code_size       = 0;

	// for Mode Manage
    jpgmng.corr_mode        	= D_IM_JPEG_ENABLE_OFF;
	jpgmng.mem_format			= E_IM_JPEG_MEM_FORM_PLANE_DOT;
    jpgmng.ext_mode         	= E_IM_JPEG_RESIZE_EXT_DIRECT;
    jpgmng.skip_mk_flg      	= D_IM_JPEG_ENABLE_OFF;
    jpgmng.pint_line    		= 0;
    jpgmng.pint_sect    		= 0;
 	
	jpgmng.color_band.y_band	= 0x01;
	jpgmng.color_band.cb_band	= 0x01;
	jpgmng.color_band.cr_band	= 0x01;
	
	jpgmng.pbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	jpgmng.pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	jpgmng.pbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF;
	jpgmng.pbuf_ctrl.prot_type		= D_IM_JPEG_D_SEC_NORMAL_ACCESS;
	jpgmng.pbuf_ctrl.err_state		= D_IM_JPEG_AXI_OKAY;
	jpgmng.jbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	jpgmng.jbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	jpgmng.jbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF;
	jpgmng.jbuf_ctrl.prot_type		= D_IM_JPEG_D_SEC_NORMAL_ACCESS;
	jpgmng.jbuf_ctrl.err_state		= D_IM_JPEG_AXI_OKAY;	// output parameter.
	jpgmng.pburst_length		= E_IM_JPEG_BURST_INCR_8;
	jpgmng.jburst_length		= E_IM_JPEG_BURST_INCR_8;

	jpgmng.burst_alignment		= E_IM_JPEG_BURST_AL_ON;
	jpgmng.port_mode			= E_IM_JPEG_IIP_PORT_MODE_OFF;
	jpgmng.ring_on				= D_IM_JPEG_ENABLE_OFF;
	jpgmng.block_size			= E_IM_JPEG_IIP_BLOCK_64x8;
	jpgmng.ring_size			= 0;
	jpgmng.org_width			= 0;
	jpgmng.org_lines			= 0;
    jpgmng.smpl_type       		= jpg_dec_mng->ycbcr_smpl;
    jpgmng.pcallback        	= NULL;
	
	// Get the semaphore
	drv_ret = Im_JPEG_Open( D_DDIM_SEM_WAIT_FEVR );
	if (drv_ret != D_IM_JPEG_OK) {
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_DEC_Open error. ret=%d\n", drv_ret));
		return result;
	}
	
	drv_ret = Im_JPEG_Ctrl_Dec( &jpgmng );
	if (drv_ret != D_IM_JPEG_OK) {
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_DEC_Open error. ret=%d\n", drv_ret));
		return result;
	}
	
    drv_ret = Im_JPEG_Ctrl_Dec_Frame( &jpg_frm_mng );
    if ( drv_ret != D_IM_JPEG_OK ) {
        Im_JPEG_Close();
 		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_Ctrl_Dec_Frame error. ret=%d\n", drv_ret));
        return result;
    }

    drv_ret = Im_JPEG_Start_Dec();
    if ( drv_ret != D_IM_JPEG_OK ) {
        Im_JPEG_Close();
 		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_Start_Dec error. ret=%d\n", drv_ret));
        return result;
    }
    
	drv_ret = Im_JPEG_Wait_End_Dec(&jpgmng, D_DDIM_WAIT_END_TIME);
	if (drv_ret != D_IM_JPEG_OK) {
		if ( drv_ret == D_IM_JPEG_AXI_ERR ) {
			Ddim_Print(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_Wait_End_Dec AXI error! ercd = %d\n", drv_ret));
		}
		else if ( drv_ret == D_IM_JPEG_DECODE_PAUSE ) {
			Ddim_Print(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_Wait_End_Dec Code size over error! ercd = %d\n", drv_ret));
		}
		else {
			Ddim_Print(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_Wait_End_Dec Decode error! ercd = %d\n", drv_ret));
		}
		Im_JPEG_Stop();
	}
	
#if 0
	for ( loop_cnt = 0; loop_cnt <= 1; loop_cnt++ ) {
		drv_ret = Im_JPEG_Wait_End_Dec(&jpgmng, D_DDIM_WAIT_END_TIME);
	 	/* Result check */
		if( drv_ret != D_IM_JPEG_OK ) {
		    if ( drv_ret == D_IM_JPEG_AXI_ERR ) {
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_Wait_End_Dec() AXI error! ercd = %d\n", drv_ret));
			}
		    else if ( drv_ret == D_IM_JPEG_DECODE_ERR ) {
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_Wait_End_Dec() Decode error! ercd = %d\n", drv_ret));
			}
		    else if ( drv_ret == D_IM_JPEG_DECODE_PAUSE ) {
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_Wait_End_Dec() Jpeg decode pause! ercd = %d\n", drv_ret));
				
			    drv_ret = Im_JPEG_Restart_Dec( &jpg_frm_mng );
			    if ( drv_ret != D_IM_JPEG_OK ) {
			        Im_JPEG_Close();
			 		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_Restart_Dec() error. ret=%d\n", drv_ret));
			        return result;
			    }
			}
		}
	}
#endif	
	// Set output paramater
	jpg_dec_mng->ycbcr_smpl	= (E_BF_PLAY_MACRO_WRAP_JPG_SMPL)jpgmng.smpl_type;
	jpg_dec_mng->width		= jpgmng.org_width;
	jpg_dec_mng->lines		= jpgmng.org_lines;

	// Return the semaphore
	drv_ret = Im_JPEG_Close();
	if (drv_ret != D_IM_JPEG_OK) {
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Decode_Jpeg(): Im_JPEG_DEC_Close error. ret=%d\n", drv_ret));
		return result;
	}

	// OK
	result = FJ_ERR_OK;

	return result;
}

/**
 * @brief 		Get output image size of HDMI/TV display device for Base F/W APL.
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE* output_size		Output image size of HDMI/TV display device for Base F/W APL.
 * @return		Error code.
 * @note		The size of the image that the BaseF/W application should prepare for display device is returned.
 *				The value is different from a set value of the output size of the DISP macro. 
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Macro_Wrap_Get_Hdmi_Output_Size(T_BF_PLAY_MACRO_WRAP_SIZE* output_size)
{
	INT32 ret;
	ULONG ovsize;
	ULONG ohsize;

	ret = Im_DISP_Get_Output_Size(E_IM_DISP_HDMI, &ovsize, &ohsize);
	if ( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Disp_Output_Size() : Im_DISP_Get_Output_Size error.\n"));
		return FJ_ERR_NG;
	}

	output_size->width			= ohsize;
	output_size->lines			= ovsize;
	output_size->global_width	= BF_Play_Common_RoundUp_Bit_32(output_size->width);
	return FJ_ERR_OK;
}

/**
 * @brief		Get effective display device
 * @param		None
 * @retval		UCHAR
 * @note		None 
 * @attention	None
 */
UCHAR BF_Play_Macro_Wrap_Get_Effective_Device( VOID )
{
	UCHAR effective_device = 0x0;
#if 0	// (To change to the appropriate value from later)
	E_FJ_DISP_IFS show_enable = BF_Osd_Attr_Get_Display_Interface();	// LCD/HD connect status get
#else	// (To change to the appropriate value from later)
	E_FJ_DISP_IFS show_enable = E_FJ_DISP_IFS_HDMI;
#endif	// (To change to the appropriate value from later)

	switch(show_enable) {
		case E_FJ_DISP_IFS_LCD:
			effective_device |= E_BF_PLAY_MACRO_WRAP_EFFECTIVE_LCD;
			break;
			
		case D_IM_DISP_IFS_HDMI:
			effective_device |= E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI;
			break;
			
		default:
			effective_device |= E_BF_PLAY_MACRO_WRAP_EFFECTIVE_NONE;
			break;
	}
	
	return effective_device;
}

//#pragma arm section code="BASEFW_COMM_CODE"
/**
 * @brief		Make Back Image
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src_image
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR dst_image
 * @param		i:E_BF_PLAY_MACRO_WRAP_PFMT dst_ycc_type
 * @return		None
 * @attentio	None
 */
VOID BF_Play_Macro_Wrap_Make_Image_With_Back(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src_image, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR dst_image, E_BF_PLAY_MACRO_WRAP_PFMT dst_ycc_type)
{
	USHORT	lines_div;
	USHORT	left_fill_width;
	USHORT	up_fill_lines;
	T_BF_PLAY_MACRO_WRAP_ADDR dst_copy_addrs;

	switch(dst_ycc_type) {
		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC420:
			lines_div = 2;
			break;

		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC422:
			lines_div = 1;
			break;

		default:
			lines_div = 1;
			break;
	}

	left_fill_width = BF_Play_Common_RoundDown_Bit_2((dst_image.size.width - src_image.size.width) >> 1);
	up_fill_lines   = BF_Play_Common_RoundDown_Bit_2((dst_image.size.lines - src_image.size.lines) >> 1);
	
	/* Use Xch transfer to fill black color */
	BF_Play_Macro_Wrap_Fill_Image(&(dst_image.addrs), 
								dst_image.size.global_width, dst_image.size.width, dst_image.size.lines,
								D_BF_PLAY_MACRO_WRAP_FILL_BLACK_Y, D_BF_PLAY_MACRO_WRAP_FILL_BLACK_CB, D_BF_PLAY_MACRO_WRAP_FILL_BLACK_CR,
								dst_ycc_type,
								E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR,
								E_BF_PLAY_MACRO_WRAP_FILL_ALL,
								E_BF_PLAY_MACRO_WRAP_USED_2CH);

	dst_copy_addrs.addr_a = dst_image.addrs.addr_a + (dst_image.size.global_width * up_fill_lines) + left_fill_width;
	dst_copy_addrs.addr_b = dst_image.addrs.addr_b + ((dst_image.size.global_width * up_fill_lines) / lines_div) + left_fill_width;
	BF_Play_Macro_Wrap_Copy_Image(src_image.addrs.addr_a, src_image.addrs.addr_b, src_image.size.global_width, 
								dst_copy_addrs.addr_a, dst_copy_addrs.addr_b, dst_image.size.global_width, 
								src_image.size.width, src_image.size.lines,
								dst_ycc_type,
								E_BF_PLAY_MACRO_WRAP_USED_2CH);

	return;
}

/**
 * @brief		Fill Image by Xch
 * @param		i:VOID* fill_addrs
 * @param		i:USHORT fill_gl_width
 * @param		i:USHORT fill_width
 * @param		i:USHORT fill_lines, 
 * @param		i:UCHAR fill_y_color
 * @param		i:UCHAR fill_cb_color
 * @param		i:UCHAR fill_cr_color, 
 * @param		i:E_BF_PLAY_MACRO_WRAP_PFMT ycc_type
 * @param		i:E_BF_PLAY_MACRO_WRAP_SET_FORM fill_form
 * @param		i:E_BF_PLAY_MACRO_WRAP_FILL fill_type
 * @param		i:E_BF_PLAY_MACRO_WRAP_USED_CH ch_mode
 * @return		None
 * @note		None
 * @attentio	None
 */
VOID BF_Play_Macro_Wrap_Fill_Image( VOID* fill_addrs, 
									USHORT fill_gl_width, USHORT fill_width, USHORT fill_lines, 
									UCHAR fill_y_color, UCHAR fill_cb_color, UCHAR fill_cr_color, 
									E_BF_PLAY_MACRO_WRAP_PFMT ycc_type, 
									E_BF_PLAY_MACRO_WRAP_SET_FORM fill_form, 
									E_BF_PLAY_MACRO_WRAP_FILL fill_type,
									E_BF_PLAY_MACRO_WRAP_USED_CH ch_mode)
{
	USHORT	lines_div;
	ULONG	fill_addr_y;
	ULONG	fill_addr_c;
	T_BF_PLAY_MACRO_WRAP_YCC_FILL_PARAM fill_param;

	switch(ycc_type) {
		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC420:
			lines_div = 2;
			break;

		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC422:
			lines_div = 1;
			break;

		//case E_BF_PLAY_MACRO_WRAP_PFMT_YCC444:  // FALL THROUGH
		default:
			lines_div = 1;
			break;
	}
	
	// set param area address
	if( fill_form == E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG ) {
		ULONG* param;

		param = (ULONG*)fill_addrs;

		fill_addr_y = *param;										// set param area address (Y)
		fill_addr_c = fill_addr_y + (fill_gl_width * fill_lines);	// set param area address (CbCr)
	}
	else if( fill_form == E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR ) {
		T_BF_PLAY_MACRO_WRAP_ADDR* param;

		param = (T_BF_PLAY_MACRO_WRAP_ADDR*)fill_addrs;

		fill_addr_y = param->addr_a;								// set param area address (Y)
		fill_addr_c = param->addr_b;								// set param area address (CbCr)
	}
	else if( fill_form == E_BF_PLAY_MACRO_WRAP_SET_FORM_JPEGMNG ) {
		T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* param;

		param = (T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG*)fill_addrs;

		fill_addr_y = param->ycbcr_addr[0];							// set param area address (Y)
		fill_addr_c = param->ycbcr_addr[1];							// set param area address (CbCr)
	}
	else {
		return;
	}

	// Use Xch transfer to fill color
	play_macro_wrap_set_fill_param( fill_y_color,	fill_gl_width,	fill_addr_y,	fill_width,		fill_lines,
									fill_cb_color,	fill_cr_color,	fill_gl_width,	fill_addr_c,	fill_width,	(fill_lines / lines_div),
									fill_type,		&fill_param);


	if (ch_mode == E_BF_PLAY_MACRO_WRAP_USED_1CH) {
		play_macro_wrap_fill_ycc_1ch( &fill_param );
	}
	else {
		play_macro_wrap_fill_ycc_2ch( &fill_param );
	}

	return;
}

/**
 * @brief		Copy Image by Xch
 * @param		i:ULONG							src_addr_y			Y address of source image
 * @param		i:ULONG							src_addr_c			CC address of source image
 * @param		i:USHORT						src_gl_width		Global width of source image
 * @param		i:ULONG							dst_addr_y			Y address of destination image
 * @param		i:ULONG							dst_addr_c			CC address of destination image
 * @param		i:USHORT						dst_gl_width		Global width of destination image
 * @param		i:USHORT						copy_width			Copy width
 * @param		i:USHORT						copy_lines			Copy lines
 * @param		i:E_BF_PLAY_MACRO_WRAP_PFMT		ycc_type			YCC type
 * @param		i:E_BF_PLAY_MACRO_WRAP_USED_CH	ch_mode				Channel mode
 * @return		None
 * @note		None
 * @attentio	None
 */
VOID BF_Play_Macro_Wrap_Copy_Image(ULONG src_addr_y, ULONG src_addr_c, USHORT src_gl_width,
									ULONG dst_addr_y, ULONG dst_addr_c, USHORT dst_gl_width,
									USHORT copy_width, USHORT copy_lines,
									E_BF_PLAY_MACRO_WRAP_PFMT ycc_type,
									E_BF_PLAY_MACRO_WRAP_USED_CH ch_mode)
{
	USHORT	lines_div;
	T_BF_PLAY_MACRO_WRAP_YCC_COPY_PARAM copy_param;

	switch(ycc_type) {
		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC420:
			lines_div = 2;
			break;

		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC422:
			lines_div = 1;
			break;

		//case E_BF_PLAY_MACRO_WRAP_PFMT_YCC444:  // FALL THROUGH
		default:
			return;
	}
	
	/* Use Xch transfer to copy image */
	play_macro_wrap_set_copy_param( src_addr_y, src_gl_width, dst_gl_width, dst_addr_y, copy_width, copy_lines,
										src_addr_c, src_gl_width, dst_gl_width, dst_addr_c, copy_width, (copy_lines / lines_div),
										&copy_param );
	play_macro_wrap_copy_ycc( &copy_param, ch_mode );

	return;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

/**
 * @brief		Set Param Size and Address
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* set_size_addrs
 * @param		i:VOID*  set_addrs
 * @param		i:USHORT set_width
 * @param		i:USHORT set_lines
 * @param		i:USHORT set_global_width
 * @param		i:E_BF_PLAY_MACRO_WRAP_SET_FORM set_form
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Macro_Wrap_Set_Size_Addr( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* set_size_addrs, VOID*  set_addrs, USHORT set_width, USHORT set_lines, USHORT set_global_width,
                                       E_BF_PLAY_MACRO_WRAP_SET_FORM set_form )
{
	/* set param area size */
	set_size_addrs->size.width			= set_width;
	set_size_addrs->size.lines			= set_lines;
	set_size_addrs->size.global_width	= set_global_width;
	set_size_addrs->size.enc_width		= BF_Play_Common_RoundUp_Bit_16(set_width);

	/* set param area address */
	if( set_form == E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG ) {
		ULONG* param;
		param = (ULONG*)set_addrs;

		set_size_addrs->addrs.addr_a = *param;															/* set param area address A (Y) */
		set_size_addrs->addrs.addr_b = set_size_addrs->addrs.addr_a + ( set_global_width * set_lines );	/* set param area address B (CbCr) */
	}
	else if( set_form == E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR ) {
		T_BF_PLAY_MACRO_WRAP_ADDR* param;
		param = (T_BF_PLAY_MACRO_WRAP_ADDR*)set_addrs;

		set_size_addrs->addrs.addr_a = param->addr_a;	/* set param area address A (Y) */
		set_size_addrs->addrs.addr_b = param->addr_b;	/* set param area address B (CbCr) */
	}
	else if( set_form == E_BF_PLAY_MACRO_WRAP_SET_FORM_JPEGMNG ) {
		T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* param;
		param = (T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG*)set_addrs;

		set_size_addrs->addrs.addr_a = param->ycbcr_addr[0];	/* set param area address A (Y) */
		set_size_addrs->addrs.addr_b = param->ycbcr_addr[1];	/* set param area address B (CbCr) */
	}

	return;
}

// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief		size convert & roate for IIP macro
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src		source convert info
 * @param		i:E_BF_PLAY_MACRO_WRAP_JPG_SMPL src_ycc_type			source ycc type
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst		destination convert info
 * @param		i:E_BF_PLAY_MACRO_WRAP_JPG_SMPL dst_ycc_type			destination ycc type
 * @param		i:E_BF_DCF_IF_ROTATE rotate					rotate type
 * @param		i:E_BF_PLAY_MACRO_WRAP_ADJUST aspect		aspect adjust flag
 * @return		INT32
 * @note		None
 * @attention	None
 */
INT32 BF_Play_Macro_Wrap_Rotate_Convert_Image( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src, E_BF_PLAY_MACRO_WRAP_PFMT src_ycc_type,
												T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst, E_BF_PLAY_MACRO_WRAP_PFMT dst_ycc_type,
												E_BF_DCF_IF_ROTATE rotate, E_BF_PLAY_MACRO_WRAP_ADJUST aspect )
{
	return play_macro_wrap_rotate_convert_image( src, src_ycc_type, E_IM_IIP_FTYPE_Y_PACKED_CBCR,
												dst, dst_ycc_type, E_IM_IIP_FTYPE_Y_PACKED_CBCR,
								   				rotate, aspect );
}
// --- REMOVE_DCF_EXIF END ---

/**
 * @brief		size convert param set
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src		src convert info
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst		dst convert info
 * @param		i:E_BF_PLAY_MACRO_WRAP_JPG_SMPL dst_ycc_type			dst ycc type
 * @param		i:E_BF_PLAY_MACRO_WRAP_ADJUST adjust_onoff	aspect adjust flag
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Macro_Wrap_Set_Adjust_Param( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst, E_BF_PLAY_MACRO_WRAP_PFMT dst_ycc_type, E_BF_PLAY_MACRO_WRAP_ADJUST adjust_onoff )
{
	UINT32					asp_width = dst->size.lines;
	UINT32					asp_lines = dst->size.width;
	DOUBLE					d_img_ratio;
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE	dst_asp_type;

	if (adjust_onoff == E_BF_PLAY_MACRO_WRAP_ADJUST_ON) {	// check aspect and adjust size convert value
		BF_Debug_Print_Information(("BF_Play_Macro_Wrap_Set_Adjust_Param() : Aspect adjustment:ON\n"));

		// get src/dst aspect type
		BF_Play_Common_Get_Aspect_Type( dst->size.width, dst->size.lines, &dst_asp_type );

		if (dst_asp_type == E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_4_3) {
			asp_width = 3;
			asp_lines = 4;
		}
		else if(dst_asp_type == E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_16_9) {
			asp_width = 9;
			asp_lines = 16;
		}
		else if(dst_asp_type == E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_3_2) {  //HD: 720x480p 60Hz
			asp_width = 2;
			asp_lines = 3;
		}
		else if (dst_asp_type == E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_3_4) {
			asp_width = 4;
			asp_lines = 3;
		}
		else if (dst_asp_type == E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_9_16) {
			asp_width = 16;
			asp_lines = 9;
		}
		else if (dst_asp_type == E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_2_3) {
			asp_width = 3;
			asp_lines = 2;
		}
		else {//if (dst_asp_type == E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_UNDEFINE)
			if (dst->size.width == dst->size.lines){	// 1:1
				asp_width = 1;
				asp_lines = 1;	
			}
			else if (dst->size.width * 4 == dst->size.lines * 5){  //HD: 720x576p 50Hz
				asp_width = 4;
				asp_lines = 5;
			}
		}
		BF_Debug_Print_Information(("BF_Play_Macro_Wrap_Set_Adjust_Param() : Aspect %d:%d\n", asp_width, asp_lines));

		if ((src.size.width * asp_width) == (src.size.lines * asp_lines)) {
			BF_Debug_Print_Information(("BF_Play_Macro_Wrap_Set_Adjust_Param() : src aspect = dst aspect (src.width = dst.width; src.lines = dst.lines)\n"));
		}
		else if ((src.size.width * asp_width) > (src.size.lines * asp_lines)) {
			/* SRC Aspect > DST Aspect */
			BF_Debug_Print_Information(("BF_Play_Macro_Wrap_Set_Adjust_Param() : src aspect > dst aspect (src.width = dst.width; src.lines > dst.lines)\n"));
			/* image:
			**************************************
			*                                    *
			*                                    *
			*             MAIN PICTURE           * after_rec_lines
			*                                    *
			*                                    *
			**************************************
			*                                    *
			*                BLACK               *
			*                                    *
			**************************************
			*/	
			if (dst->size.width == WIDTH_VGA) {	// LCD display
				d_img_ratio = (DOUBLE)src.size.lines / (DOUBLE)src.size.width;
				dst->size.lines = (USHORT)((DOUBLE)WIDTH_VGA * d_img_ratio);
				if (dst->size.lines != LINES_VGA_3_2) {
					if (dst->size.lines + 1 == LINES_VGA_3_2) {
						dst->size.lines++;
					}
					else if (dst->size.lines - 1 == LINES_VGA_3_2) {
						dst->size.lines--;
					}
				}
				if (dst->size.lines != LINES_VGA_16_9) {
					if (dst->size.lines + 1 == LINES_VGA_16_9) {
						dst->size.lines++;
					}
					else if (dst->size.lines - 1 == LINES_VGA_16_9) {
						dst->size.lines--;
					}
				}
				if (dst->size.lines % 2) {
					dst->size.lines--;
				}
			}
			else if (dst->size.width == WIDTH_FHD) {	// HD display
				d_img_ratio = (DOUBLE)src.size.lines / (DOUBLE)src.size.width;
				dst->size.lines = (USHORT)((DOUBLE)WIDTH_FHD * d_img_ratio);
				if (dst->size.lines != LINES_FHD) {
					if (dst->size.lines + 1 == LINES_FHD) {
						dst->size.lines++;
					}
					else if (dst->size.lines - 1 == LINES_FHD) {
						dst->size.lines--;
					}
				}
				if (dst->size.lines != LINES_FHD) {
					if (dst->size.lines + 1 == LINES_FHD) {
						dst->size.lines++;
					}
					else if (dst->size.lines - 1 == LINES_FHD) {
						dst->size.lines--;
					}
				}				
				if (dst->size.lines % 2) {
					dst->size.lines--;
				}
			}
			else if (dst->size.width == WIDTH_4K2K_QFHD) {	// 4K display
				d_img_ratio = (DOUBLE)src.size.lines / (DOUBLE)src.size.width;
				dst->size.lines = (USHORT)((DOUBLE)WIDTH_4K2K_QFHD * d_img_ratio);
				if (dst->size.lines != LINES_4K2K_QFHD) {
					if (dst->size.lines + 1 == LINES_4K2K_QFHD) {
						dst->size.lines++;
					}
					else if (dst->size.lines - 1 == LINES_4K2K_QFHD) {
						dst->size.lines--;
					}
				}
				if (dst->size.lines != LINES_4K2K_QFHD) {
					if (dst->size.lines + 1 == LINES_4K2K_QFHD) {
						dst->size.lines++;
					}
					else if (dst->size.lines - 1 == LINES_4K2K_QFHD) {
						dst->size.lines--;
					}
				}				
				if (dst->size.lines % 2) {
					dst->size.lines--;
				}
			}
			else {
				d_img_ratio = (DOUBLE)src.size.lines / (DOUBLE)src.size.width;
				dst->size.lines = (USHORT)((DOUBLE)(dst->size.width) * d_img_ratio);
				if (dst->size.lines % 2) {
					dst->size.lines--;
				}
			}
		}
		else{
			/* SRC Aspect < DST Aspect */
			BF_Debug_Print_Information(("BF_Play_Macro_Wrap_Convert_Size() : src aspect < dst aspect (src.width > dst.width; src.lines = dst.lines)\n"));
			/* image:
			**************************************
			*                           *        *
			*                           *        *
			*                           *    B   *
			*                           *    L   *
			*       MAIN PICTURE        *    A   *
			*                           *    C   *
			*                           *    K   *
			*                           *        *
			*                           *        *
			**************************************
			<----- after_rec_width ----->
			*/

			/* calculate REC parameter */
			dst->size.width = BF_Play_Common_RoundDown_Bit_2((src.size.width * dst->size.lines) / src.size.lines);
		}
	}
	else {
		BF_Debug_Print_Information(("BF_Play_Macro_Wrap_Set_Adjust_Param() : Aspect adjustment:OFF\n"));
	}
	return;
}

/******************************/
/**  PB Section(FastBoot)    **/
/******************************/
//#pragma arm section code="BASEFW_PB_CODE"
// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief		Get JPEG data
 * @param		i:FJ_MODE										mode
 * @param		i:ULONG image_index								image index
 * @param		i:UCHAR* load_addr								load image address
 * @param		i:UCHAR* decode_addr							decode image address
 * @param		o:T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng		jpeg decode information
 * @param		o:T_BF_DCF_IF_IMAGE_DETAIL* img_detail			file detail information
 * @param		o:T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data	load image information
 * @param		i:BOOL dec_flag									decode flag
 * @param		i:BOOL bDetect_mode_change						Detect mode change
 * @return		FW_INT32
 * @note		None
 * @attention	None
 */
FW_INT32 BF_Play_Macro_Wrap_Get_Jpeg( FJ_MODE mode, ULONG image_index, UCHAR* load_addr, UCHAR* decode_addr, T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data, BOOL dec_flag, BOOL bDetect_mode_change )
{
	INT32	ret = 0;

	switch ( mode ) {
		case FJ_MODE_PLAYBACK:	//FALLTHROUGH
		case FJ_MODE_DISP_IMAGE_MAIN:       // main load
			ret = play_macro_wrap_get_jpeg_main(image_index, load_addr, decode_addr, jpgmng, img_detail, load_image_data, dec_flag, bDetect_mode_change);
			if (ret != FJ_ERR_OK) {
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Jpeg(): BF_Play_Macro_Wrap_Get_Jpeg_Main ERR. err=%d, current no=0x%lx\n", ret, image_index));
				return -1;
			}

			break;

#ifdef CO_BF_DCF_IF_MPF_ENABLE
		case FJ_MODE_DISP_MONCLS1_MAIN:       // class1 load
			ret = play_macro_wrap_get_jpeg_class(FJ_PLAYBACK_IMAGE_SEL_CLS1, image_index, load_addr, decode_addr, jpgmng, img_detail, load_image_data, dec_flag, bDetect_mode_change);
			if (ret != FJ_ERR_OK) {
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Jpeg(): BF_Play_Macro_Wrap_Get_Jpeg_Class ERR. err=%d, current no=0x%lx\n", ret, image_index));
				return -1;
			}

			break;
			
		case FJ_MODE_DISP_MONCLS2_MAIN:       // class2 load
			ret = play_macro_wrap_get_jpeg_class(FJ_PLAYBACK_IMAGE_SEL_CLS2, image_index, load_addr, decode_addr, jpgmng, img_detail, load_image_data, dec_flag, bDetect_mode_change);
			if (ret != FJ_ERR_OK) {
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Jpeg(): BF_Play_Macro_Wrap_Get_Jpeg_Class ERR. err=%d, current no=0x%lx\n", ret, image_index));
				return -1;
			}

			break;
#endif

		case FJ_MODE_DISP_IMAGE_THUMB:       // thumbnail load
			ret = play_macro_wrap_get_jpeg_thumbnail(image_index, load_addr, decode_addr, jpgmng, img_detail, load_image_data, dec_flag);
			if (ret != 0){
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Jpeg(): BF_Play_Macro_Wrap_Get_Jpeg_Thumbnail ERR. err=%d, current no=0x%lx\n", ret, image_index));
				return -1;
			}
			
			break;

#ifdef CO_BF_DCF_IF_MPF_ENABLE
		case FJ_MODE_DISP_MONCLS1_THUMB:
			ret = play_macro_wrap_get_jpeg_thm_class(FJ_PLAYBACK_IMAGE_SEL_CLS1, image_index, load_addr, decode_addr, jpgmng, img_detail, load_image_data, dec_flag);
			if (ret != 0){
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Jpeg(): BF_Play_Macro_Wrap_Get_Jpeg_Thm_Class ERR. err=%d, current no=0x%lx\n", ret, image_index));
				return -1;
			}
			
			break;
			
		case FJ_MODE_DISP_MONCLS2_THUMB:
			ret = play_macro_wrap_get_jpeg_thm_class(FJ_PLAYBACK_IMAGE_SEL_CLS1, image_index, load_addr, decode_addr, jpgmng, img_detail, load_image_data, dec_flag);
			if (ret != 0){
				BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Jpeg(): BF_Play_Macro_Wrap_Get_Jpeg_Thm_Class ERR. err=%d, current no=0x%lx\n", ret, image_index));
				return -1;
			}
			
			break;
#endif
		default:
			return -1;
	}

	return 0;
}
// --- REMOVE_DCF_EXIF END ---

/**
 * @brief:  	This function displays a given image regardless of size to the LCD display
 * @param:		E_IM_DISP_SEL disp_sel		common block selection
 * @param:	 	ULONG image_addr			address of image to be displayed
 * @return: 	None
 * @notes:	 	None
 * @attention	None
 */
VOID  BF_Play_Macro_Wrap_Display( E_IM_DISP_SEL disp_sel, const T_BF_PLAY_MACRO_WRAP_DISP_ADDR* src_addr )
{
	E_IM_DISP_BANK switch_bank;
	U_IM_DISP_LIDT lidt = { .word[0] = 0, .word[1] = 0 };
	INT32 ret;

	ret = Im_DISP_Get_Input_Data_Transfer( disp_sel, &lidt );
	if( ret != D_DDIM_OK) {
		BF_Debug_Print_Error(( "BF_Play_Macro_Wrap_Display: Im_DISP_Get_Input_Data_Transfer error.ret=%x\n",ret));
	}

	lidt.bit.IFMT	= D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT;
	lidt.bit.NBT	= D_IM_DISP_NBT_512BYTE;
	lidt.bit.PKGDV	= 0;
	lidt.bit.LVFM	= 0;
	ret = Im_DISP_Set_Input_Data_Transfer( disp_sel, lidt );
	if( ret != D_DDIM_OK) {
		BF_Debug_Print_Error(( "BF_Play_Macro_Wrap_Display: Im_DISP_Set_Input_Data_Transfer error.ret=%x\n",ret));
	}

	ret = Im_DISP_Get_Main_Bank(disp_sel, &switch_bank);
	if( ret != D_DDIM_OK) {
		BF_Debug_Print_Error(( "BF_Play_Macro_Wrap_Display: Im_DISP_Get_Main_Bank error.ret=%x\n",ret));
	}
	switch(switch_bank) {
		case E_IM_DISP_BANK_00:
		case E_IM_DISP_BANK_01:
		case E_IM_DISP_BANK_10:
			switch_bank = (E_IM_DISP_BANK)(switch_bank +1);
			break;
		default://E_IM_DISP_BANK_11
			switch_bank = E_IM_DISP_BANK_00;
			break;
	}

	//Set bank address
	ret = Im_DISP_Set_Addr(disp_sel, switch_bank, &src_addr->addrs);
	if( ret != D_DDIM_OK) {
		BF_Debug_Print_Error(( "BF_Play_Macro_Wrap_Display: Im_DISP_Set_Addr error.ret=%x\n",ret));
	}

	Im_DISP_Set_Main_Bank(disp_sel, switch_bank);
}

/**
 * @brief		Display Playback Image to LCD
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR vga_size_addr		display output size and address
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Macro_Wrap_Display_Lcd( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR vga_size_addr )
{
	U_IM_DISP_LALP lalp;

	// D-LCD Out
	play_macro_wrap_display_dlcd( vga_size_addr, E_BF_PLAY_MACRO_WRAP_DISPLAY_TYPE_PB );
//	Im_DISP_Change_BB_Color_Enable( FALSE );
	lalp.word = 0;
	lalp.bit.ALP = 0xFF;
	Im_DISP_Set_Alpha_Value(E_IM_DISP_LCD_MIPI, lalp);
	return;
}

/**
 * @brief		Display Playback Image to HDMI
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR hd_size_addr		display output size and address
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Macro_Wrap_Display_Hdmi( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR hd_size_addr )
{
	U_IM_DISP_LALP lalp;

	// HDMI Out
	play_macro_wrap_display_hdmi( hd_size_addr, E_BF_PLAY_MACRO_WRAP_DISPLAY_TYPE_PB );
//	Im_DISP_Change_BB_Color_Enable( FALSE );
	lalp.word = 0;
	lalp.bit.ALP = 0xFF;
	Im_DISP_Set_Alpha_Value(E_IM_DISP_HDMI, lalp);
	return;
}

/**
 * @brief		Show display playback image to LCD
 * @param		None
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Macro_Wrap_Show_Lcd( VOID )
{
#if 0
	T_BF_PLAY_MACRO_WRAP_DISP_ADDR		image_addr;
	U_IM_DISP_SIZE						input_disp_size = {0};
	U_IM_DISP_DSTA						position = {0};
	T_IM_DISP_RESIZE					resize;
	U_IM_DISP_LALP						lalp;

	//Set DISP size
	input_disp_size.size.width = g_image_addr_lcd.width;
	input_disp_size.size.lines = g_image_addr_lcd.lines;

	BF_Play_Common_Disp_Set_Int_Callback(TRUE, 1, E_IM_DISP_WAIT_1_TIME);
	//wait VD
	BF_Play_Common_DISP_Wait_Vsync();

	Im_DISP_Set_Input_Size(E_IM_DISP_LCD_MIPI, input_disp_size);

	Im_DISP_Set_Lfd(E_IM_DISP_LCD_MIPI, g_image_addr_lcd.gl_lines, g_image_addr_lcd.gl_lines);
	
	Im_DISP_Get_Resize(E_IM_DISP_LCD_MIPI, &resize);
	
	position.bit.DSH = ((g_image_addr_lcd.pos_x + ((WIDTH_VGA - g_image_addr_lcd.width) >> 1)) * resize.rsz1.bit.HRSZM) / resize.rsz1.bit.HRSZN;
	position.bit.DSV = ((g_image_addr_lcd.pos_y + ((LINES_VGA - g_image_addr_lcd.lines) >> 1)) * resize.rsz2.bit.VRSZM) / resize.rsz2.bit.VRSZN;
	Im_DISP_Set_Display_Position(E_IM_DISP_LCD_MIPI, position);

	// Display next image.
	image_addr.addrs.y_addr = g_image_addr_lcd.addr.addr_a;
	image_addr.addrs.c_addr = g_image_addr_lcd.addr.addr_b;

	BF_Play_Macro_Wrap_Display( E_IM_DISP_LCD_MIPI, &image_addr );

	BF_Play_Common_Disp_Set_Int_Callback(FALSE, 0, E_IM_DISP_WAIT_1_TIME);
//	Im_DISP_Change_BB_Color_Enable( FALSE );
	lalp.word = 0;
	lalp.bit.ALP = 0xFF;
	Im_DISP_Set_Alpha_Value(E_IM_DISP_LCD_MIPI, lalp);
#endif
	return;
}

/**
 * @brief		Show display playback image to HDMI/TV
 * @param		None
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Macro_Wrap_Show_Hdmi( VOID )
{
	FJ_ERR_CODE							ercd;
	T_BF_PLAY_MACRO_WRAP_SIZE			output_size;
	T_BF_PLAY_MACRO_WRAP_DISP_ADDR		image_addr;
	U_IM_DISP_SIZE						input_disp_size = {0};
	U_IM_DISP_DSTA						position = {0};
	T_IM_DISP_RESIZE					resize;
	U_IM_DISP_LALP						lalp;

	// Get output image size.
	ercd = BF_Play_Macro_Wrap_Get_Hdmi_Output_Size(&output_size);
	if (ercd != FJ_ERR_OK) {
		return;
	}
		
	//Set DISP size
	input_disp_size.size.width = g_image_addr_hdmi.width;
	input_disp_size.size.lines = g_image_addr_hdmi.lines;

	BF_Play_Common_Disp_Set_Int_Callback(TRUE, 1, E_IM_DISP_WAIT_1_TIME);
	//wait VD
	BF_Play_Common_DISP_Wait_Vsync();

	Im_DISP_Set_Input_Size(E_IM_DISP_HDMI, input_disp_size);

	Im_DISP_Set_Lfd(E_IM_DISP_HDMI, g_image_addr_hdmi.gl_lines, g_image_addr_hdmi.gl_lines);
	
	Im_DISP_Get_Resize(E_IM_DISP_HDMI, &resize);
	
	position.bit.DSH = ((g_image_addr_hdmi.pos_x + ((output_size.width - g_image_addr_hdmi.width) >> 1)) * resize.rsz1.bit.HRSZM) / resize.rsz1.bit.HRSZN;
	position.bit.DSV = ((g_image_addr_hdmi.pos_y + ((output_size.lines - g_image_addr_hdmi.lines) >> 1)) * resize.rsz2.bit.VRSZM) / resize.rsz2.bit.VRSZN;
	Im_DISP_Set_Display_Position(E_IM_DISP_HDMI, position);

	// Display next image.
	image_addr.addrs.y_addr = g_image_addr_hdmi.addr.addr_a;
	image_addr.addrs.c_addr = g_image_addr_hdmi.addr.addr_b;

	BF_Play_Macro_Wrap_Display( E_IM_DISP_HDMI, &image_addr );

	BF_Play_Common_Disp_Set_Int_Callback(FALSE, 0, E_IM_DISP_WAIT_1_TIME);
//	Im_DISP_Change_BB_Color_Enable( FALSE );
	lalp.word = 0;
	lalp.bit.ALP = 0xFF;
	Im_DISP_Set_Alpha_Value(E_IM_DISP_HDMI, lalp);
	return;
}

/**
 * @brief		Get histogram data (Asynchronous)
 * @param		i: USHORT	width
 * @param		i: USHORT	lines
 * @param		i: USHORT	gl_width
 * @param		i: ULONG	hist_addr
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Macro_Wrap_Get_Histogram_Data( USHORT width, USHORT lines, USHORT gl_width, ULONG hist_addr )
{
	T_IM_XCH_CTRL_CMN	xch_ctrl_cmn;
	T_IM_XCH_CTRL_HIST	xch_ctrl_hist;
	INT32				ret;

	// Set parameter
	xch_ctrl_cmn.mode		= D_IM_XCH_MD_HISTGRAM;
	xch_ctrl_hist.xdtype	= D_IM_XCH_XDTYPE_8;
	xch_ctrl_hist.hcyc		= 0x00;
	xch_ctrl_hist.vcyc		= 0x00;
	xch_ctrl_hist.hen		= 0xFFFFFFFF;
	xch_ctrl_hist.ven		= 0xFFFFFFFF;
	xch_ctrl_hist.s_hsize	= gl_width;
	xch_ctrl_cmn.g_hsize	= gl_width;
	xch_ctrl_cmn.d_hsize	= width;
	xch_ctrl_cmn.d_vsize	= lines;
	xch_ctrl_hist.src_addr	= hist_addr;
	xch_ctrl_cmn.dst_addr	= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Image_Work_Addr() );
	xch_ctrl_hist.hcnt		= 0;
	xch_ctrl_cmn.pCallBack	= 0;

	// get the semaphore
	ret = Im_Xch_Open( E_IM_XCH_CH_SEL_0, D_DDIM_SEM_WAIT_FEVR );
	if (ret != D_IM_XCH_OK) {
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Histogram_Data(): Im_Xch_Open error. ret=%d\n", ret));
		return FJ_ERR_NG;
	}

	ret = Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_0, &xch_ctrl_cmn );
	if (ret != D_IM_XCH_OK) {
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Histogram_Data(): Im_Xch_Ctrl_Common error. ret=%d\n", ret));
		return FJ_ERR_NG;
	}

	ret = Im_Xch_Ctrl_Hist( E_IM_XCH_CH_SEL_0, &xch_ctrl_hist );
	if (ret != D_IM_XCH_OK) {
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Histogram_Data(): Im_Xch_Ctrl_Hist error. ret=%d\n", ret));
		return FJ_ERR_NG;
	}

	ret = Im_Xch_Start_Async( E_IM_XCH_CH_SEL_0 );
	if (ret != D_IM_XCH_OK) {
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Histogram_Data(): Im_Xch_Start_Async error. ret=%d\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/******************************/
/**  Last Section(FastBoot)  **/
/******************************/
//#pragma arm section code="BASEFW_LAST_CODE"
// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief		decode encode jpeg422
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src_size_addrs src size and address
 * @param		i:ULONG dst_addr dst address
 * @param		i:E_BF_PLAY_MACRO_WRAP_JPEG_QT qt keep quantization table flag
 * @param		o:T_BF_DCF_IF_JPEG_QT* quant
 * @param		o:ULONG* dst_size dst size
 * @param		i:E_BF_PLAY_MACRO_WRAP_ENC_TYPE enc
 * @param		i:ULONG max_size 
 * @param		i:E_BF_PLAY_MACRO_WRAP_JPG_SMPL ycc_smpl 
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	Supported format is semi planar format only.
 */
INT32 BF_Play_Macro_Wrap_Encoder_Jpeg(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src_size_addrs, ULONG dst_addr, E_BF_PLAY_MACRO_WRAP_JPEG_QT qt, T_BF_DCF_IF_JPEG_QT* quant, ULONG* dst_size, E_BF_PLAY_MACRO_WRAP_ENC_TYPE enc, ULONG max_size, E_BF_PLAY_MACRO_WRAP_JPG_SMPL ycc_smpl)
{
#if 0
	INT32					ret = 0;
	FJ_ERR_CODE				sof_ret;
	T_BF_JPGENC_PARAM		jpegenc_param;
	ULONG					limit_size = 0;				// Encode limit size.
	T_IM_JPEG_ENC_AXI_MNG	jpg_enc_axi_mng = {			// AXI controll.
			.rlenmax2	= E_IM_JPEG_ENC_BURST_INCR_8,
			.rlenmax	= E_IM_JPEG_ENC_BURST_INCR_8,
			.wlenmax	= E_IM_JPEG_ENC_BURST_INCR_8,
			.wtmax		= E_IM_JPEG_ENC_TRAN_4,
			.m2arprot	= D_IM_JPEG_ENC_D_SEC_NORMAL_ACCESS,
			.m2arcache	= D_IM_JPEG_ENC_NON_CACHE_NON_BUF,
			.awprot		= D_IM_JPEG_ENC_D_SEC_NORMAL_ACCESS,
			.awcache	= D_IM_JPEG_ENC_NON_CACHE_NON_BUF,
			.arprot		= D_IM_JPEG_ENC_D_SEC_NORMAL_ACCESS,
			.arcache	= D_IM_JPEG_ENC_NON_CACHE_NON_BUF,
	};

	memset( &jpegenc_param, 0, sizeof(T_BF_JPGENC_PARAM) );

	jpegenc_param.smpl_type						= (E_IM_JPEG_ENC_SMPL_TYPE)ycc_smpl;
	jpegenc_param.mem_fmt						= E_IM_JPEG_ENC_MEM_FORM_PLANE_DOT;

	jpegenc_param.jpg_enc_axi_tbl				= jpg_enc_axi_mng;
	jpegenc_param.mode							= E_BF_JPGEPC_MODE_PLAY;
	jpegenc_param.rstm							= 0;
	jpegenc_param.jpg_enc_result.result			= 0;
	jpegenc_param.jpg_enc_result.encsize		= 0;

	jpegenc_param.jpg_enc_frame_tbl.qty			= 0;
	jpegenc_param.jpg_enc_frame_tbl.qtcb		= 1;
	jpegenc_param.jpg_enc_frame_tbl.qtcr		= 1;
	jpegenc_param.jpg_enc_frame_tbl.line_ofs_p1	= src_size_addrs.size.global_width;
	jpegenc_param.jpg_enc_frame_tbl.line_ofs_p2	= src_size_addrs.size.global_width;
	jpegenc_param.jpg_enc_frame_tbl.line_ofs_p3	= 0;
	jpegenc_param.jpg_enc_frame_tbl.xsize		= src_size_addrs.size.enc_width;
	jpegenc_param.jpg_enc_frame_tbl.ysize		= src_size_addrs.size.lines;

	jpegenc_param.jpg_enc_frame_tbl.bsbar		= dst_addr;
	jpegenc_param.jpg_enc_frame_tbl.penc		= E_IM_JPEG_ENC_PRE_ENC_OFF;	// dummy value

	/* Address */
	jpegenc_param.jpg_enc_frame_tbl.img.bar_p1	= src_size_addrs.addrs.addr_a;
	jpegenc_param.jpg_enc_frame_tbl.img.bar_p2	= src_size_addrs.addrs.addr_b;
	jpegenc_param.jpg_enc_frame_tbl.img.bar_p3	= 0;

	/* Size */
	if ( jpegenc_param.smpl_type == E_IM_JPEG_ENC_SMPL_TYPE_YCC420 ) {
		jpegenc_param.jpg_enc_frame_tbl.img.siz_p1	= M_BF_PLAY_MACRO_WRAP_ROUNDUP_16( src_size_addrs.size.global_width * src_size_addrs.size.lines );			// 16 byte unit
		jpegenc_param.jpg_enc_frame_tbl.img.siz_p2	= M_BF_PLAY_MACRO_WRAP_ROUNDUP_8( src_size_addrs.size.global_width * (src_size_addrs.size.lines >> 1) );	// 8 byte unit
		jpegenc_param.jpg_enc_frame_tbl.img.siz_p3	= 0;
	}
	else {
		jpegenc_param.jpg_enc_frame_tbl.img.siz_p1	= M_BF_PLAY_MACRO_WRAP_ROUNDUP_8( src_size_addrs.size.global_width * src_size_addrs.size.lines );			// 8 byte unit
		jpegenc_param.jpg_enc_frame_tbl.img.siz_p2	= M_BF_PLAY_MACRO_WRAP_ROUNDUP_8( src_size_addrs.size.global_width * src_size_addrs.size.lines );			// 8 byte unit
		jpegenc_param.jpg_enc_frame_tbl.img.siz_p3	= 0;
	}

	limit_size	= M_BF_PLAY_MACRO_WRAP_ROUNDDOWN_8( max_size );	// 8 byte unit

	switch(enc) {
		case E_BF_PLAY_MACRO_WRAP_ENC_TYPE_MAIN:
			if ( qt != E_BF_PLAY_MACRO_WRAP_JPEG_QT_OFF ) {
				jpegenc_param.quality = BF_Still_Utility_Get_Jpeg_Encode_Quality( (USHORT)BF_Still_Attr_Get_Comp_Main(), 0 );
			}
			break;

		case E_BF_PLAY_MACRO_WRAP_ENC_TYPE_SCREEN:
			// APP2 area limit check
			if ( limit_size > D_BF_DCF_EXIF_MAKER_SEGMENT_LIMIT_SIZE ){
				// APP2 segment limit size
				limit_size = D_BF_DCF_EXIF_MAKER_SEGMENT_LIMIT_SIZE;
			}

			if ( qt != E_BF_PLAY_MACRO_WRAP_JPEG_QT_OFF ) {
				jpegenc_param.quality = BF_Still_Utility_Get_Jpeg_Encode_Quality( (USHORT)BF_Still_Attr_Get_Comp_Screen_Vga(), 0 );
			}
			break;

		case E_BF_PLAY_MACRO_WRAP_ENC_TYPE_THUMB:
			if ( qt != E_BF_PLAY_MACRO_WRAP_JPEG_QT_OFF ) {
				jpegenc_param.quality = BF_Still_Utility_Get_Jpeg_Encode_Quality( 0, 1 );
			}
			break;

		default:
			break;
	}

	jpegenc_param.limit_size					= limit_size;
	jpegenc_param.jpg_enc_frame_tbl.bsmsiz		= limit_size;

	*dst_size = Jpgepc_Encode_Jpeg( &jpegenc_param );

	if ( src_size_addrs.size.width != src_size_addrs.size.enc_width ) {
		sof_ret = play_macro_wrap_change_sof_width( dst_addr, jpegenc_param.jpg_enc_result.encsize, src_size_addrs.size.width );
		if (sof_ret != FJ_ERR_OK){
			ret = -1;
		}
	}
	return ret;
#endif
	return 0;
}
// --- REMOVE_DCF_EXIF END ---

/**
 * @brief		set IIP AlphaBlend common parameter
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src1
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src2
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE dst_size
 * @param		i:T_IM_IIP_UTIL_ALPHABLEND* alpha_blend_param 
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Macro_Wrap_Set_AlphaBlend_Common_Param( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src1, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src2,
													T_BF_PLAY_MACRO_WRAP_SIZE_ADDR dst,
													T_IM_IIP_UTIL_ALPHABLEND* alpha_blend_param )
{
	alpha_blend_param->src[0].rect.top						= 0;
	alpha_blend_param->src[0].rect.left						= 0;
	alpha_blend_param->src[0].rect.width					= src1.size.width;
	alpha_blend_param->src[0].rect.lines					= src1.size.lines;
	alpha_blend_param->src[0].gbl.alpha						= D_IM_IIP_ALPHA_FALSE;
	alpha_blend_param->src[0].gbl.frame_type				= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	alpha_blend_param->src[0].gbl.pix_format				= E_IM_IIP_PFMT_YCC420;
	alpha_blend_param->src[0].gbl.chunky_color.component0	= D_IM_IIP_CHUNKY_COLOR_Y0_G;
	alpha_blend_param->src[0].gbl.chunky_color.component1	= D_IM_IIP_CHUNKY_COLOR_CB_B;
	alpha_blend_param->src[0].gbl.chunky_color.component2	= D_IM_IIP_CHUNKY_COLOR_CR_R;
	alpha_blend_param->src[0].gbl.chunky_color.component3	= D_IM_IIP_CHUNKY_COLOR_Y1_A;
	alpha_blend_param->src[0].gbl.sign_Y_G					= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->src[0].gbl.sign_Cb_B					= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->src[0].gbl.sign_Cr_R					= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->src[0].gbl.sign_D3					= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->src[0].gbl.width						= src1.size.width;
	alpha_blend_param->src[0].gbl.lines						= src1.size.lines;
	alpha_blend_param->src[0].gbl.line_bytes.Y_G			= src1.size.global_width;
	alpha_blend_param->src[0].gbl.line_bytes.Cb_B			= src1.size.global_width;
	alpha_blend_param->src[0].gbl.line_bytes.Cr_R			= 0;
	alpha_blend_param->src[0].gbl.line_bytes.Alpha			= 0;
	alpha_blend_param->src[0].gbl.addr.Y_G					= src1.addrs.addr_a;
	alpha_blend_param->src[0].gbl.addr.Cb_B					= src1.addrs.addr_b;
	alpha_blend_param->src[0].gbl.addr.Cr_R					= 0;
	alpha_blend_param->src[0].gbl.addr.Alpha				= 0;
	alpha_blend_param->src[0].gbl.masterIF_Y_G				= D_IM_IIP_MASTER_IF0;
	alpha_blend_param->src[0].gbl.masterIF_Cb_B				= D_IM_IIP_MASTER_IF0;
	alpha_blend_param->src[0].gbl.masterIF_Cr_R				= D_IM_IIP_MASTER_IF0;
	alpha_blend_param->src[0].gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF0;

	alpha_blend_param->src[1].rect.top						= 0;
	alpha_blend_param->src[1].rect.left						= 0;
	alpha_blend_param->src[1].rect.width					= src2.size.width;
	alpha_blend_param->src[1].rect.lines					= src2.size.lines;
	alpha_blend_param->src[1].gbl.alpha						= D_IM_IIP_ALPHA_FALSE;
	alpha_blend_param->src[1].gbl.frame_type				= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	alpha_blend_param->src[1].gbl.pix_format				= E_IM_IIP_PFMT_YCC420;
	alpha_blend_param->src[1].gbl.chunky_color.component0	= D_IM_IIP_CHUNKY_COLOR_Y0_G;
	alpha_blend_param->src[1].gbl.chunky_color.component1	= D_IM_IIP_CHUNKY_COLOR_CB_B;
	alpha_blend_param->src[1].gbl.chunky_color.component2	= D_IM_IIP_CHUNKY_COLOR_CR_R;
	alpha_blend_param->src[1].gbl.chunky_color.component3	= D_IM_IIP_CHUNKY_COLOR_Y1_A;
	alpha_blend_param->src[1].gbl.sign_Y_G					= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->src[1].gbl.sign_Cb_B					= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->src[1].gbl.sign_Cr_R					= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->src[1].gbl.sign_D3					= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->src[1].gbl.width						= src2.size.width;
	alpha_blend_param->src[1].gbl.lines						= src2.size.lines;
	alpha_blend_param->src[1].gbl.line_bytes.Y_G			= src2.size.global_width;
	alpha_blend_param->src[1].gbl.line_bytes.Cb_B			= src2.size.global_width;
	alpha_blend_param->src[1].gbl.line_bytes.Cr_R			= 0;
	alpha_blend_param->src[1].gbl.line_bytes.Alpha			= 0;
	alpha_blend_param->src[1].gbl.addr.Y_G					= src2.addrs.addr_a;
	alpha_blend_param->src[1].gbl.addr.Cb_B					= src2.addrs.addr_b;
	alpha_blend_param->src[1].gbl.addr.Cr_R					= 0;
	alpha_blend_param->src[1].gbl.addr.Alpha				= 0;
	alpha_blend_param->src[1].gbl.masterIF_Y_G				= D_IM_IIP_MASTER_IF0;
	alpha_blend_param->src[1].gbl.masterIF_Cb_B				= D_IM_IIP_MASTER_IF0;
	alpha_blend_param->src[1].gbl.masterIF_Cr_R				= D_IM_IIP_MASTER_IF0;
	alpha_blend_param->src[1].gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF0;
		
	alpha_blend_param->dst.rect.top							= 0;
	alpha_blend_param->dst.rect.left						= 0;
	alpha_blend_param->dst.rect.width						= dst.size.width;
	alpha_blend_param->dst.rect.lines						= dst.size.lines;
	alpha_blend_param->dst.gbl.alpha						= D_IM_IIP_ALPHA_FALSE;
	alpha_blend_param->dst.gbl.frame_type					= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	alpha_blend_param->dst.gbl.pix_format					= E_IM_IIP_PFMT_YCC420;
	alpha_blend_param->dst.gbl.chunky_color.component0		= D_IM_IIP_CHUNKY_COLOR_Y0_G;
	alpha_blend_param->dst.gbl.chunky_color.component1		= D_IM_IIP_CHUNKY_COLOR_CB_B;
	alpha_blend_param->dst.gbl.chunky_color.component2		= D_IM_IIP_CHUNKY_COLOR_CR_R;
	alpha_blend_param->dst.gbl.chunky_color.component3		= D_IM_IIP_CHUNKY_COLOR_Y1_A;
	alpha_blend_param->dst.gbl.sign_Y_G						= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->dst.gbl.sign_Cb_B					= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->dst.gbl.sign_Cr_R					= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->dst.gbl.sign_D3						= D_IM_IIP_UNSIGNED_DATA;
	alpha_blend_param->dst.gbl.width						= dst.size.width;
	alpha_blend_param->dst.gbl.lines						= dst.size.lines;
	alpha_blend_param->dst.gbl.line_bytes.Y_G				= dst.size.global_width;
	alpha_blend_param->dst.gbl.line_bytes.Cb_B				= dst.size.global_width;
	alpha_blend_param->dst.gbl.line_bytes.Cr_R				= 0;
	alpha_blend_param->dst.gbl.line_bytes.Alpha				= 0;
	alpha_blend_param->dst.gbl.addr.Y_G						= dst.addrs.addr_a;
	alpha_blend_param->dst.gbl.addr.Cb_B					= dst.addrs.addr_b;
	alpha_blend_param->dst.gbl.addr.Cr_R					= 0;
	alpha_blend_param->dst.gbl.addr.Alpha					= 0;
	alpha_blend_param->dst.gbl.masterIF_Y_G					= D_IM_IIP_MASTER_IF1;
	alpha_blend_param->dst.gbl.masterIF_Cb_B				= D_IM_IIP_MASTER_IF1;
	alpha_blend_param->dst.gbl.masterIF_Cr_R				= D_IM_IIP_MASTER_IF1;
	alpha_blend_param->dst.gbl.masterIF_Alpha				= D_IM_IIP_MASTER_IF1;

	alpha_blend_param->ld_cache_select[0]					= E_IM_IIP_PARAM_CSEL_0;
	alpha_blend_param->ld_cache_select[1]					= E_IM_IIP_PARAM_CSEL_1;
	alpha_blend_param->pix_depth							= D_IM_IIP_PDEPTH_8BITS;
	alpha_blend_param->alpha_depth							= E_IM_IIP_ADEPTH_8BITS;
	alpha_blend_param->alpha_subsampling					= D_IM_IIP_ALPHA_NO_SUBSAMP;
	alpha_blend_param->alpha.step							= E_IM_IIP_PARAM_ALPHA_STEP_8BITS;
	alpha_blend_param->alpha.sel							= E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL;
	alpha_blend_param->alpha.out							= E_IM_IIP_PARAM_ALPHA_OUT_NONE;

	return;
}

/**
 * @brief		AlphaBlend Image by IIP Macro
 * @param		i:T_IM_IIP_UTIL_ALPHABLEND iip_util_blend
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	None
 */
INT32 BF_Play_Macro_Wrap_AlphaBlend_Image( T_IM_IIP_UTIL_ALPHABLEND* iip_util_blend )
{

	INT32							retval = 0;
	INT32							set_loop;
	INT32							wait_time = D_BF_PLAY_MACRO_WRAP_WAIT_TIME;
	T_FJ_IIP_RES_CONFIG_ARG			iip_blend_config_arg;
	T_FJ_IIP_RES_CONFIG_OUT			iip_blend_config_out;
	T_FJ_IIP_RES_CONFIG_OUT			iip_blend_get_resource;
	UCHAR							param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR							param_buffer[D_IM_IIP_UTIL_ALPHABLEND_BUF_BYTES + 8];
	ULONG							param_buffer_addr = (((ULONG)param_buffer) + 7UL) & 0xFFFFFFF8UL;	// Convet 8bytes align

	memset( &iip_blend_config_arg, 0, sizeof( T_FJ_IIP_RES_CONFIG_ARG ));
	memset( &iip_blend_config_out, 0, sizeof( T_FJ_IIP_RES_CONFIG_OUT ));
	iip_blend_config_arg.unit.oned				= 2;
	iip_blend_config_arg.unit.blend				= 1;
	iip_blend_config_arg.unit.sl_master_if_0	= 1;
	iip_blend_config_arg.pixid					= 3;
	
	// IIP Resource Get
	retval = BF_IIP_Res_Config_Lock_BaseFW( &iip_blend_config_out, &iip_blend_config_arg, 1, wait_time );
	if( retval != D_FJ_IIP_ERR_OK ){
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_AlphaBlend_Image(): BF_IIP_Res_Config_Lock_BaseFW Error=%d\n", retval));
		return -1;
	}
	
	// saves local
	iip_blend_get_resource = iip_blend_config_out;
	
	// oned unit id get
	for( set_loop = 0; set_loop < 2; set_loop++ ){
		if( iip_blend_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_LD0 ){
			iip_util_blend->ld_unitid[set_loop] = E_IM_IIP_UNIT_ID_LD0;
			iip_blend_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_LD0;
			continue;
		}
		if( iip_blend_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_LD1 ){
			iip_util_blend->ld_unitid[set_loop] = E_IM_IIP_UNIT_ID_LD1;
			iip_blend_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_LD1;
			continue;
		}
		if( iip_blend_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_LD2 ){
			iip_util_blend->ld_unitid[set_loop] = E_IM_IIP_UNIT_ID_LD2;
			iip_blend_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_LD2;
			continue;
		}
	}
	
	// blend unit id get
	if( iip_blend_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_BLEND0 ){
		iip_util_blend->blend_unitid = E_IM_IIP_UNIT_ID_BLEND0;
		iip_blend_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_BLEND0;
	}
	if( iip_blend_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_BLEND1 ){
		iip_util_blend->blend_unitid = E_IM_IIP_UNIT_ID_BLEND1;
		iip_blend_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_BLEND1;
	}
	
	// sl unit(memory) id get
	if( iip_blend_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL2 ){
		iip_util_blend->sl_unitid = E_IM_IIP_UNIT_ID_SL2;
		iip_blend_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL2;
	}
	if( iip_blend_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL3 ){
		iip_util_blend->sl_unitid = E_IM_IIP_UNIT_ID_SL3;
		iip_blend_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL3;
	}
	if( iip_blend_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL4 ){
		iip_util_blend->sl_unitid = E_IM_IIP_UNIT_ID_SL4;
		iip_blend_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL4;
	}
	if( iip_blend_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL5 ){
		iip_util_blend->sl_unitid = E_IM_IIP_UNIT_ID_SL5;
		iip_blend_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL5;
	}
	if( iip_blend_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL6 ){
		iip_util_blend->sl_unitid = E_IM_IIP_UNIT_ID_SL6;
		iip_blend_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL6;
	}
	
	// pixel id get
	set_loop = 0;
	for( ; set_loop < D_BF_PLAY_MACRO_WRAP_IIP_PIXID_MAX; set_loop++ ){
		if( iip_blend_get_resource.pixid_bitmask & (1 << set_loop) ){
			iip_util_blend->src_pixid[0] = (E_IM_IIP_PIXID)(1 << set_loop);
			iip_blend_get_resource.pixid_bitmask &= ~(1 << set_loop);
			break;
		}
	}
	for( ; set_loop < D_BF_PLAY_MACRO_WRAP_IIP_PIXID_MAX; set_loop++ ){
		if( iip_blend_get_resource.pixid_bitmask & (1 << set_loop) ){
			iip_util_blend->src_pixid[1] = (E_IM_IIP_PIXID)(1 << set_loop);
			iip_blend_get_resource.pixid_bitmask &= ~(1 << set_loop);
			break;
		}
	}
	for( ; set_loop < D_BF_PLAY_MACRO_WRAP_IIP_PIXID_MAX; set_loop++ ){
		if( iip_blend_get_resource.pixid_bitmask & (1 << set_loop) ){
			iip_util_blend->dst_pixid = (E_IM_IIP_PIXID)(1 << set_loop);
			iip_blend_get_resource.pixid_bitmask &= ~(1 << set_loop);
			break;
		}
	}
	
	if( set_loop == D_BF_PLAY_MACRO_WRAP_IIP_PIXID_MAX ){
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_AlphaBlend_Image(): IIP Resource PIXID Get NG!!\n"));
		BF_IIP_Res_Config_Free_BaseFW( &iip_blend_config_out, 1, wait_time );
		return -1;
	}
	if( (iip_blend_get_resource.unitid_bitmask64 != 0) || (iip_blend_get_resource.pixid_bitmask != 0) ){
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_AlphaBlend_Image(): IIP Resource Abnormal!! \n"));
		BF_IIP_Res_Config_Free_BaseFW( &iip_blend_config_out, 1, wait_time );
		return -1;
	}

	retval = IM_IIP_Utility_AlphaBlend( iip_util_blend, param_master_IF, param_buffer_addr, wait_time );
	if( retval != D_DDIM_OK ){
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_AlphaBlend_Image(): IM_IIP_Utility_AlphaBlend error. (%d)\n", retval));
		BF_IIP_Res_Config_Free_BaseFW( &iip_blend_config_out, 1, wait_time );
		return -1;
	}
	
	// IIP Resource Release
	retval = BF_IIP_Res_Config_Free_BaseFW( &iip_blend_config_out, 1, wait_time );
	if( retval != D_FJ_IIP_ERR_OK ){
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_AlphaBlend_Image(): BF_IIP_Res_Config_Free_BaseFW Error=%d\n", retval));
		return -1;
	}
	
	return 0;
}

/**
 * @brief		set IIP LUT parameter
 * @param		i:T_BF_PLAY_MACRO_WRAP_ADDR src_addr
 * @param		i:USHORT src_gl_width
 * @param		i:USHORT src_width
 * @param		i:USHORT src_lines,
 * @param		i:T_BF_PLAY_MACRO_WRAP_ADDR dst_addr
 * @param		i:USHORT dst_gl_width
 * @param		i:USHORT dst_width
 * @param		i:USHORT dst_lines
 * @param		i:E_IM_IIP_PFMT iip_pfmt
 * @param		i:T_IM_IIP_LUTRAM iip_lutram
 * @param		i:T_IM_IIP_UTIL_LUT* iip_util_lut
 * @return		VOID
 * @note		None
 * @attention	None
 */
VOID BF_Play_Macro_Wrap_Set_Lut_Param( T_BF_PLAY_MACRO_WRAP_ADDR src_addr, 
										USHORT src_gl_width, USHORT src_width, USHORT src_lines,
										T_BF_PLAY_MACRO_WRAP_ADDR dst_addr,
										USHORT dst_gl_width, USHORT dst_width, USHORT dst_lines,
										E_IM_IIP_PFMT iip_pfmt, T_IM_IIP_LUTRAM iip_lutram,
										T_IM_IIP_UTIL_LUT* iip_util_lut )
{
	// src setting
	iip_util_lut->src.rect.top						= 0;
	iip_util_lut->src.rect.left						= 0;
	iip_util_lut->src.rect.width					= src_width;
	iip_util_lut->src.rect.lines					= src_lines;

	iip_util_lut->src.gbl.alpha						= D_IM_IIP_ALPHA_FALSE;
	iip_util_lut->src.gbl.frame_type				= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	iip_util_lut->src.gbl.pix_format				= iip_pfmt;
	iip_util_lut->src.gbl.chunky_color.component0	= D_IM_IIP_CHUNKY_COLOR_Y0_G;
	iip_util_lut->src.gbl.chunky_color.component1	= D_IM_IIP_CHUNKY_COLOR_CB_B;
	iip_util_lut->src.gbl.chunky_color.component2	= D_IM_IIP_CHUNKY_COLOR_CR_R;
	iip_util_lut->src.gbl.chunky_color.component3	= D_IM_IIP_CHUNKY_COLOR_Y1_A;
	iip_util_lut->src.gbl.sign_Y_G					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_lut->src.gbl.sign_Cb_B					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_lut->src.gbl.sign_Cr_R					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_lut->src.gbl.sign_D3					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_lut->src.gbl.width						= src_width;
	iip_util_lut->src.gbl.lines						= src_lines;
	iip_util_lut->src.gbl.line_bytes.Y_G			= src_gl_width;
	iip_util_lut->src.gbl.line_bytes.Cb_B 			= src_gl_width;
	iip_util_lut->src.gbl.line_bytes.Cr_R 			= src_gl_width;
	iip_util_lut->src.gbl.line_bytes.Alpha			= src_gl_width;
	iip_util_lut->src.gbl.addr.Y_G					= src_addr.addr_a;
	iip_util_lut->src.gbl.addr.Cb_B 				= src_addr.addr_b;
	iip_util_lut->src.gbl.addr.Cr_R 				= src_addr.addr_b;
	iip_util_lut->src.gbl.addr.Alpha				= 0;

	iip_util_lut->src.gbl.masterIF_Y_G 				= D_IM_IIP_MASTER_IF0;
	iip_util_lut->src.gbl.masterIF_Cb_B 			= D_IM_IIP_MASTER_IF0;
	iip_util_lut->src.gbl.masterIF_Cr_R				= D_IM_IIP_MASTER_IF0;
	iip_util_lut->src.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF0;

	// destination setting
	iip_util_lut->dst.rect.top						= 0;
	iip_util_lut->dst.rect.left						= 0;
	iip_util_lut->dst.rect.width					= dst_width;
	iip_util_lut->dst.rect.lines					= dst_lines;

	iip_util_lut->dst.gbl.alpha						= D_IM_IIP_ALPHA_FALSE;
	iip_util_lut->dst.gbl.frame_type				= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	iip_util_lut->dst.gbl.pix_format				= iip_pfmt;
	iip_util_lut->dst.gbl.chunky_color.component0	= D_IM_IIP_CHUNKY_COLOR_Y0_G;
	iip_util_lut->dst.gbl.chunky_color.component1	= D_IM_IIP_CHUNKY_COLOR_CB_B;
	iip_util_lut->dst.gbl.chunky_color.component2	= D_IM_IIP_CHUNKY_COLOR_CR_R;
	iip_util_lut->dst.gbl.chunky_color.component3	= D_IM_IIP_CHUNKY_COLOR_Y1_A;
	iip_util_lut->dst.gbl.sign_Y_G					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_lut->dst.gbl.sign_Cb_B					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_lut->dst.gbl.sign_Cr_R					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_lut->dst.gbl.sign_D3					= D_IM_IIP_UNSIGNED_DATA;
	iip_util_lut->dst.gbl.width						= dst_width;
	iip_util_lut->dst.gbl.lines						= dst_lines;
	iip_util_lut->dst.gbl.line_bytes.Y_G			= dst_gl_width;
	iip_util_lut->dst.gbl.line_bytes.Cb_B 			= dst_gl_width;
	iip_util_lut->dst.gbl.line_bytes.Cr_R 			= dst_gl_width;
	iip_util_lut->dst.gbl.line_bytes.Alpha			= dst_gl_width;
	iip_util_lut->dst.gbl.addr.Y_G					= dst_addr.addr_a;
	iip_util_lut->dst.gbl.addr.Cb_B 				= dst_addr.addr_b;
	iip_util_lut->dst.gbl.addr.Cr_R 				= dst_addr.addr_b;
	iip_util_lut->dst.gbl.addr.Alpha				= 0;

	iip_util_lut->dst.gbl.masterIF_Y_G 				= D_IM_IIP_MASTER_IF1;
	iip_util_lut->dst.gbl.masterIF_Cb_B 			= D_IM_IIP_MASTER_IF1;
	iip_util_lut->dst.gbl.masterIF_Cr_R				= D_IM_IIP_MASTER_IF1;
	iip_util_lut->dst.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF1;

	iip_util_lut->ld_cache_select					= E_IM_IIP_PARAM_CSEL_0;
	iip_util_lut->pix_depth							= D_IM_IIP_PDEPTH_8BITS;
	iip_util_lut->alpha_depth						= E_IM_IIP_ADEPTH_8BITS;
	iip_util_lut->alpha_subsampling					= D_IM_IIP_ALPHA_NO_SUBSAMP;

	iip_util_lut->lut_pix_depth[0]					= 8;
	iip_util_lut->lut_pix_depth[1]	 				= 8;
	iip_util_lut->lut_pix_depth[2]					= 8;
	iip_util_lut->lut_pix_depth[3]					= 8;
	iip_util_lut->splbit[0]							= 0;
	iip_util_lut->splbit[1]			 				= 0;
	iip_util_lut->splbit[2]	 		 				= 0;
	iip_util_lut->splbit[3]	 		 				= 0;
	iip_util_lut->lutram				 			= iip_lutram;

	return;
}

/**
 * @brief		Use IIP Macro to Lut Image Color
 * @param		i:T_IM_IIP_UTIL_ALPHABLEND iip_util_blend
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	None
 */
INT32 BF_Play_Macro_Wrap_Lut_Image( T_IM_IIP_UTIL_LUT* iip_util_lut )
{
	INT32							retval = 0;
	INT32							set_loop;
	T_FJ_IIP_RES_CONFIG_ARG			iip_lut_config_arg;
	T_FJ_IIP_RES_CONFIG_OUT			iip_lut_config_out;
	T_FJ_IIP_RES_CONFIG_OUT			iip_lut_get_resource;
	INT32							wait_time			= D_BF_PLAY_MACRO_WRAP_WAIT_TIME;
	UCHAR							param_master_IF		= D_IM_IIP_MASTER_IF0;
	UCHAR							param_buffer[D_IM_IIP_UTIL_LUT_BUF_BYTES + 8];
	ULONG							param_buffer_addr	= (((ULONG)param_buffer) + 7UL) & 0xFFFFFFF8UL;

	memset( &iip_lut_config_arg, 0, sizeof( T_FJ_IIP_RES_CONFIG_ARG ));
	memset( &iip_lut_config_out, 0, sizeof( T_FJ_IIP_RES_CONFIG_OUT ));
	iip_lut_config_arg.unit.oned			= 1;
	iip_lut_config_arg.unit.lut				= 1;
	iip_lut_config_arg.unit.sl_master_if_0	= 1;
	iip_lut_config_arg.pixid				= 2;
	
	// IIP Resource Get
	retval = BF_IIP_Res_Config_Lock_BaseFW( &iip_lut_config_out, &iip_lut_config_arg, 1, wait_time );
	if( retval != D_FJ_IIP_ERR_OK ){
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Lut_Image(): BF_IIP_Res_Config_Lock_BaseFW Error=%d\n", retval));
		return -1;
	}
	
	// saves local
	iip_lut_get_resource = iip_lut_config_out;
	
	// oned unit id get	
	if( iip_lut_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_LD0 ){
		iip_util_lut->ld_unitid = E_IM_IIP_UNIT_ID_LD0;
		iip_lut_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_LD0;
	}
	if( iip_lut_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_LD1 ){
		iip_util_lut->ld_unitid = E_IM_IIP_UNIT_ID_LD1;
		iip_lut_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_LD1;
	}
	if( iip_lut_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_LD2 ){
		iip_util_lut->ld_unitid = E_IM_IIP_UNIT_ID_LD2;
		iip_lut_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_LD2;
	}
	
	// lut unit id get
	if( iip_lut_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_LUT ){
		iip_util_lut->lut_unitid = E_IM_IIP_UNIT_ID_LUT;
		iip_lut_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_LUT;
	}
	
	// sl unit(memory) id get
	if( iip_lut_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL0 ){
		iip_util_lut->sl_unitid = E_IM_IIP_UNIT_ID_SL0;
		iip_lut_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL0;
	}
	if( iip_lut_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL3 ){
		iip_util_lut->sl_unitid = E_IM_IIP_UNIT_ID_SL3;
		iip_lut_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL3;
	}
	if( iip_lut_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL5 ){
		iip_util_lut->sl_unitid = E_IM_IIP_UNIT_ID_SL5;
		iip_lut_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL5;
	}
	if( iip_lut_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL6 ){
		iip_util_lut->sl_unitid = E_IM_IIP_UNIT_ID_SL6;
		iip_lut_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL6;
	}
	
	// pixel id get	
	set_loop = 0;
	for( ; set_loop < D_BF_PLAY_MACRO_WRAP_IIP_PIXID_MAX; set_loop++ ){
		if( iip_lut_get_resource.pixid_bitmask & (1 << set_loop) ){
			iip_util_lut->src_pixid = (E_IM_IIP_PIXID)(1 << set_loop);
			iip_lut_get_resource.pixid_bitmask &= ~(1 << set_loop);
			break;
		}
	}
	for( ; set_loop < D_BF_PLAY_MACRO_WRAP_IIP_PIXID_MAX; set_loop++ ){
		if( iip_lut_get_resource.pixid_bitmask & (1 << set_loop) ){
			iip_util_lut->dst_pixid = (E_IM_IIP_PIXID)(1 << set_loop);
			iip_lut_get_resource.pixid_bitmask &= ~(1 << set_loop);
			break;
		}
	}
	
	if( set_loop == D_BF_PLAY_MACRO_WRAP_IIP_PIXID_MAX ){
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Lut_Image(): IIP Resource PIXID Get NG!!\n"));
		BF_IIP_Res_Config_Free_BaseFW( &iip_lut_config_out, 1, wait_time );
		return -1;
	}
	if( (iip_lut_get_resource.unitid_bitmask64 != 0) || (iip_lut_get_resource.pixid_bitmask != 0) ){
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Lut_Image(): IIP Resource Abnormal!! \n"));
		BF_IIP_Res_Config_Free_BaseFW( &iip_lut_config_out, 1, wait_time );
		return -1;
	}

	retval = IM_IIP_Utility_LUT( iip_util_lut, param_master_IF, param_buffer_addr, wait_time );
	if( retval != D_DDIM_OK ){
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Lut_Image(): IM_IIP_Utility_LUT error. (%d)\n", retval));
		BF_IIP_Res_Config_Free_BaseFW( &iip_lut_config_out, 1, wait_time );
		return -1;
	}
	
	// IIP Resource Release
	retval = BF_IIP_Res_Config_Free_BaseFW( &iip_lut_config_out, 1, wait_time );
	if( retval != D_FJ_IIP_ERR_OK ){
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Lut_Image(): BF_IIP_Res_Config_Free_BaseFW Error=%d\n", retval));
		return -1;
	}
	
	return 0;
}

BOOL BF_Play_Macro_Wrap_Check_Jpeg_Thumbnail(ULONG image_index)
{
	return play_macro_wrap_check_jpeg_thumbnail( image_index );
}

