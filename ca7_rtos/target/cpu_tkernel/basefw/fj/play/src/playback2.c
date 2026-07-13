/**
 * @file		playback2.c
 * @brief		public playback function for host side
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_PB_INIT"
//#pragma arm section rodata="BASEFW_PB_CONST"
//#pragma arm section code="BASEFW_PB_CODE"

#include "fj_playback.h"
#include "sdram_map_play.h"
#include "sdram_map_cache_define.h"
#include "still_utility.h"
#include "play_macro_wrap.h"
#include "play_common.h"
#include "disp.h"
#include "l1l2cache.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
// Nothing Special

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
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
//#pragma arm section code="BASEFW_COMM_CODE"

//#pragma arm section code="BASEFW_PB_CODE"

//#pragma arm section code="BASEFW_COMM_CODE"

//#pragma arm section code="BASEFW_PB_CODE"

//#pragma arm section code="BASEFW_COMM_CODE"

/**
 * @brief		get memory information of playback
 * @param		i:T_FJ_PLAYBACK_MEMINFO* mem_info
 * @return		FJ_ERR_CODE
 * @note		This API returns top address and size of each buffer.
 * @attention	None
 */
FJ_ERR_CODE fj_stillplayback_getmeminfo( T_FJ_PLAYBACK_MEMINFO* mem_info )
{
	// Set memory information
	mem_info->decoded_output_yuv_main.top_addr	= BF_Sdram_Map_Play_Get_Decoded_YUV_Main_Addr();
	mem_info->decoded_output_yuv_main.buff_size	= BF_Sdram_Map_Play_Get_Decoded_YUV_Main_Size();
	mem_info->decoded_output_yuv_thm.top_addr	= BF_Sdram_Map_Play_Get_Decoded_YUV_Thumb_Addr();
	mem_info->decoded_output_yuv_thm.buff_size	= BF_Sdram_Map_Play_Get_Decoded_YUV_Thumb_Size();
	mem_info->decoded_resize_yuv.top_addr		= BF_Sdram_Map_Play_Get_Resize_YUV_Addr();
	mem_info->decoded_resize_yuv.buff_size		= BF_Sdram_Map_Play_Get_Resize_YUV_Size();
	mem_info->decoded_rgb.top_addr				= BF_Sdram_Map_Play_Get_Screen_RGB_Addr();
	mem_info->decoded_rgb.buff_size				= BF_Sdram_Map_Play_Get_Screen_RGB_Size();
	mem_info->thm_rgb.top_addr					= BF_Sdram_Map_Play_Get_Thumb_RGB_Addr();
	mem_info->thm_rgb.buff_size					= BF_Sdram_Map_Play_Get_Thumb_RGB_Size();
	mem_info->disp_yuv_lcd.top_addr				= BF_Sdram_Map_Play_Get_Disp_YUV_LCD_Addr();
	mem_info->disp_yuv_lcd.buff_size			= BF_Sdram_Map_Play_Get_Disp_YUV_LCD_Size();
	mem_info->disp_yuv_hdmi.top_addr			= BF_Sdram_Map_Play_Get_Disp_YUV_HDMI_Addr();
	mem_info->disp_yuv_hdmi.buff_size			= BF_Sdram_Map_Play_Get_Disp_YUV_HDMI_Size();
	
	return FJ_ERR_OK;
}

/**
 * @brief		Jpeg Decode
 * @param		i:T_FJ_PLAYBACK_DECODE* dec_info
 * @param		o:T_FJ_PLAYBACK_YUV_INFO* yuv_info
 * @return		FJ_ERR_CODE
 * @note		Decode jpeg data to YCC422 or YCC420.
 * @attention	None
 */
FJ_ERR_CODE fj_stillplayback_imagedecode( T_FJ_PLAYBACK_DECODE* dec_info, T_FJ_PLAYBACK_YUV_INFO* yuv_info )
{
	T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG	jpeg_dec_mng;
	FJ_ERR_CODE							ret;
	ULONG								global_width, global_lines;
	ULONG								offset_cb;
//	ULONG								offset_cr;
	UINT32								addr;
	UINT32								size;
	
	// Initialize
	memset( &jpeg_dec_mng, 0, sizeof(T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG) );
	memset( yuv_info, 0, sizeof(T_FJ_PLAYBACK_YUV_INFO) );
	
	// Global Size
	global_width	= (USHORT)(BF_Play_Common_RoundUp_Bit_16((ULONG)dec_info->src_width));
	if (dec_info->dst_fmt == E_FJ_PLAYBACK_FMT_YCC420)
		global_lines	= (USHORT)(BF_Play_Common_RoundUp_Bit_16((ULONG)dec_info->src_lines));
	else
		global_lines	= (USHORT)(BF_Play_Common_RoundUp_Bit_8((ULONG)dec_info->src_lines));
	
	// Set Parameters
	jpeg_dec_mng.mem_format		= E_BF_PLAY_MACRO_WRAP_JPG_MFMT_SEMI_PLANAR;
	switch ( dec_info->dst_fmt ) {
		case E_FJ_PLAYBACK_FMT_YCC422:
			jpeg_dec_mng.ycbcr_smpl	= E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422;
			offset_cb = global_width * global_lines;
//			offset_cr = offset_cb / 2;
			size = (UINT32)global_width * global_lines * 2;
			break;
		case E_FJ_PLAYBACK_FMT_YCC420:
			jpeg_dec_mng.ycbcr_smpl	= E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420;
			offset_cb = global_width * global_lines;
//			offset_cr = offset_cb / 4;
			size = (UINT32)global_width * global_lines * 3/2;
			break;
		default:
			return FJ_ERR_NG_INPUT_PARAM;
	}
	jpeg_dec_mng.gl_width		= global_width;
	jpeg_dec_mng.gl_lines		= global_lines;
	jpeg_dec_mng.ycbcr_addr[0]	= (ULONG)dec_info->dst_addr;
	jpeg_dec_mng.ycbcr_addr[1]	= (ULONG)(jpeg_dec_mng.ycbcr_addr[0] + offset_cb);
	jpeg_dec_mng.code_addr		= M_SDRAM_MAP_COMMON_ADR_LOG2PHY((ULONG)dec_info->src_addr);
	jpeg_dec_mng.code_size		= (ULONG)dec_info->src_size;
	jpeg_dec_mng.width			= (USHORT)dec_info->src_width;
	jpeg_dec_mng.lines			= (USHORT)dec_info->src_lines;
	
#if 0	// for Debug
	BF_Debug_Print_Debug(("----\n"));
	BF_Debug_Print_Debug(("mem_format	= %d (0: planar, 1: semi-planar)\n", jpeg_dec_mng.mem_format));
	BF_Debug_Print_Debug(("ycbcr_smpl	= %d (1: YCC422, 2: YCC420)\n", jpeg_dec_mng.ycbcr_smpl));
	BF_Debug_Print_Debug(("gl_width		= %d\n", jpeg_dec_mng.gl_width));
	BF_Debug_Print_Debug(("gl_lines		= %d\n", jpeg_dec_mng.gl_lines));
	BF_Debug_Print_Debug(("ycbcr_addr[0]	= 0x%08X\n", jpeg_dec_mng.ycbcr_addr[0]));
	BF_Debug_Print_Debug(("ycbcr_addr[1]	= 0x%08X\n", jpeg_dec_mng.ycbcr_addr[1]));
	BF_Debug_Print_Debug(("ycbcr_addr[2]	= 0x%08X\n", jpeg_dec_mng.ycbcr_addr[2]));
	BF_Debug_Print_Debug(("code_addr	= 0x%08X\n", jpeg_dec_mng.code_addr));
	BF_Debug_Print_Debug(("code_size	= %d\n", jpeg_dec_mng.code_size));
	BF_Debug_Print_Debug(("width		= %d\n", jpeg_dec_mng.width));
	BF_Debug_Print_Debug(("lines		= %d\n", jpeg_dec_mng.lines));
#endif
	
	// Cache Clean & Flash
	addr = (UINT32)(M_SDRAM_MAP_COMMON_ADR_LOG2PHY(jpeg_dec_mng.ycbcr_addr[0]));
	BF_L1l2cache_Clean_Flush_Addr( addr, size );
	
	// Execute Jpeg Decode Process
	ret = BF_Play_Macro_Wrap_Decode_Jpeg( &jpeg_dec_mng, -1 );
	if( ret != FJ_ERR_OK ){
		return FJ_ERR_NG;
	}
	
	// Set Output Parameters
	yuv_info->y_addr		= jpeg_dec_mng.ycbcr_addr[0];
	yuv_info->cbcr_addr		= jpeg_dec_mng.ycbcr_addr[1];
	yuv_info->g_width		= global_width;
	yuv_info->g_lines		= global_lines;
	yuv_info->width			= jpeg_dec_mng.width;
	yuv_info->lines			= jpeg_dec_mng.lines;
	switch( jpeg_dec_mng.ycbcr_smpl ){
		case E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422:
			yuv_info->yuv_format	= E_FJ_PLAYBACK_FMT_YCC422;
			break;
		case E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420:
			yuv_info->yuv_format	= E_FJ_PLAYBACK_FMT_YCC420;
			break;
		default:
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

/**
 * @brief		Image Convert
 * @param		i:T_FJ_PLAYBACK_CONVERT* conv_info
 * @return		FJ_ERR_CODE
 * @note		This function achieves the following image conversion.
   - YUV data resize
   - Rotate
   - Convert image format YUV to RGBA
 * @attention	None
 */
FJ_ERR_CODE fj_stillplayback_imageconvert( T_FJ_PLAYBACK_CONVERT* conv_info )
{
	FJ_ERR_CODE			ret;
	FJ_IMG_CONV_CTRL	conv_ctrl;
	UINT32				addr;
	UINT32				size;
	
	// Initialize
	memset( &conv_ctrl, 0, sizeof(conv_ctrl) );
	
	// src settings
	conv_ctrl.src_img.addr.Y_G			= conv_info->src_img.addr.AddressY;
	conv_ctrl.src_img.addr.Cb_B			= conv_info->src_img.addr.AddressCb;
	conv_ctrl.src_img.addr.Cr_R			= conv_info->src_img.addr.AddressCr;
	conv_ctrl.src_img.addr.Alpha		= 0;
	conv_ctrl.src_img.g_y_g_width		= conv_info->src_img.g_y_width;
	conv_ctrl.src_img.g_cb_b_width		= conv_info->src_img.g_c_width;
	conv_ctrl.src_img.g_cr_r_width		= conv_ctrl.src_img.g_cb_b_width;
	conv_ctrl.src_img.g_alpha_width		= 0;
	conv_ctrl.src_img.width				= conv_info->src_img.width;
	conv_ctrl.src_img.lines				= conv_info->src_img.lines;
	conv_ctrl.src_img.line_bytes.Y_G	= conv_ctrl.src_img.width;
	conv_ctrl.src_img.line_bytes.Cb_B	= conv_ctrl.src_img.line_bytes.Y_G;
	conv_ctrl.src_img.line_bytes.Cr_R	= conv_ctrl.src_img.line_bytes.Y_G;
	conv_ctrl.src_img.line_bytes.Alpha	= 0;
	conv_ctrl.src_img.pfmt				= conv_info->src_img.pfmt;
	
	// dst settings
	conv_ctrl.dst_img.addr.Y_G			= conv_info->dst_img.addr.AddressY;
	conv_ctrl.dst_img.addr.Cb_B			= conv_info->dst_img.addr.AddressCb;
	conv_ctrl.dst_img.addr.Cr_R			= conv_info->dst_img.addr.AddressCr;
	conv_ctrl.dst_img.addr.Alpha 		= 0;
	if( conv_info->dst_img.pfmt != FJ_IMG_PFMT_RGBA8888 ){
		conv_ctrl.dst_img.g_y_g_width		= conv_info->dst_img.g_y_width;
		conv_ctrl.dst_img.g_cb_b_width		= conv_info->dst_img.g_c_width;
		conv_ctrl.dst_img.g_cr_r_width		= conv_ctrl.dst_img.g_cb_b_width;
		conv_ctrl.dst_img.g_alpha_width		= 0;
	}
	else {
		conv_ctrl.dst_img.g_y_g_width		= conv_info->dst_img.g_y_width  * 4;
		conv_ctrl.dst_img.g_cb_b_width		= conv_ctrl.dst_img.g_y_g_width;
		conv_ctrl.dst_img.g_cr_r_width		= conv_ctrl.dst_img.g_y_g_width;
		conv_ctrl.dst_img.g_alpha_width		= conv_ctrl.dst_img.g_y_g_width;
	}
	conv_ctrl.dst_img.width				= conv_info->dst_img.width;
	conv_ctrl.dst_img.lines				= conv_info->dst_img.lines;
	if( conv_info->dst_img.pfmt != FJ_IMG_PFMT_RGBA8888 ){
		conv_ctrl.dst_img.line_bytes.Y_G	= conv_ctrl.dst_img.width;
		conv_ctrl.dst_img.line_bytes.Cb_B	= conv_ctrl.dst_img.line_bytes.Y_G;
		conv_ctrl.dst_img.line_bytes.Cr_R	= conv_ctrl.dst_img.line_bytes.Y_G;
		conv_ctrl.dst_img.line_bytes.Alpha	= 0;
	}
	else {
		conv_ctrl.dst_img.line_bytes.Y_G	= conv_ctrl.dst_img.width * 4;
		conv_ctrl.dst_img.line_bytes.Cb_B	= conv_ctrl.dst_img.line_bytes.Y_G;
		conv_ctrl.dst_img.line_bytes.Cr_R	= conv_ctrl.dst_img.line_bytes.Y_G;
		conv_ctrl.dst_img.line_bytes.Alpha	= conv_ctrl.dst_img.line_bytes.Y_G;
	}
	conv_ctrl.dst_img.pfmt				= conv_info->dst_img.pfmt;
	// common settings
	conv_ctrl.resize_mode				= conv_info->resize_mode;
	conv_ctrl.rotate_deg				= conv_info->rotate_deg;
	conv_ctrl.fill_enable				= 0;	// disable (fixed value)
	conv_ctrl.timeout					= -1;	// wait forever (fixed value)
	
#if 0	// for Debug
	BF_Debug_Print_Debug(("[fj_imageconvert() : input parameters]\n"));
	BF_Debug_Print_Debug(("---- src info ----\n"));
	BF_Debug_Print_Debug((" addr.Y_G		= 0x%08X\n", conv_ctrl.src_img.addr.Y_G ));
	BF_Debug_Print_Debug((" addr.Cb_B		= 0x%08X\n", conv_ctrl.src_img.addr.Cb_B ));
	BF_Debug_Print_Debug((" addr.Cr_R		= 0x%08X\n", conv_ctrl.src_img.addr.Cr_R ));
	BF_Debug_Print_Debug((" addr.Alpha		= 0x%08X\n", conv_ctrl.src_img.addr.Alpha ));
	BF_Debug_Print_Debug((" g_y_g_width		= %d\n", conv_ctrl.src_img.g_y_g_width ));
	BF_Debug_Print_Debug((" g_cb_b_width		= %d\n", conv_ctrl.src_img.g_cb_b_width ));
	BF_Debug_Print_Debug((" g_cr_r_width		= %d\n", conv_ctrl.src_img.g_cr_r_width ));
	BF_Debug_Print_Debug((" g_alpha_width		= %d\n", conv_ctrl.src_img.g_alpha_width ));
	BF_Debug_Print_Debug((" width			= %d\n", conv_ctrl.src_img.width ));
	BF_Debug_Print_Debug((" lines			= %d\n", conv_ctrl.src_img.lines ));
	BF_Debug_Print_Debug((" line_bytes.Y_G		= %d\n", conv_ctrl.src_img.line_bytes.Y_G ));
	BF_Debug_Print_Debug((" line_bytes.Cb_B		= %d\n", conv_ctrl.src_img.line_bytes.Cb_B ));
	BF_Debug_Print_Debug((" line_bytes.Cr_R		= %d\n", conv_ctrl.src_img.line_bytes.Cr_R ));
	BF_Debug_Print_Debug((" line_bytes.Alpha	= %d\n", conv_ctrl.src_img.line_bytes.Alpha ));
	BF_Debug_Print_Debug((" pfmt			= %d\n", conv_ctrl.src_img.pfmt ));
	BF_Debug_Print_Debug(("---- dst info ----\n"));
	BF_Debug_Print_Debug((" addr.Y_G		= 0x%08X\n", conv_ctrl.dst_img.addr.Y_G ));
	BF_Debug_Print_Debug((" addr.Cb_B		= 0x%08X\n", conv_ctrl.dst_img.addr.Cb_B ));
	BF_Debug_Print_Debug((" addr.Cr_R		= 0x%08X\n", conv_ctrl.dst_img.addr.Cr_R ));
	BF_Debug_Print_Debug((" addr.Alpha		= 0x%08X\n", conv_ctrl.dst_img.addr.Alpha ));
	BF_Debug_Print_Debug((" g_y_g_width		= %d\n", conv_ctrl.dst_img.g_y_g_width ));
	BF_Debug_Print_Debug((" g_cb_b_width		= %d\n", conv_ctrl.dst_img.g_cb_b_width ));
	BF_Debug_Print_Debug((" g_cr_r_width		= %d\n", conv_ctrl.dst_img.g_cr_r_width ));
	BF_Debug_Print_Debug((" g_alpha_width		= %d\n", conv_ctrl.dst_img.g_alpha_width ));
	BF_Debug_Print_Debug((" width			= %d\n", conv_ctrl.dst_img.width ));
	BF_Debug_Print_Debug((" lines			= %d\n", conv_ctrl.dst_img.lines ));
	BF_Debug_Print_Debug((" line_bytes.Y_G		= %d\n", conv_ctrl.dst_img.line_bytes.Y_G ));
	BF_Debug_Print_Debug((" line_bytes.Cb_B		= %d\n", conv_ctrl.dst_img.line_bytes.Cb_B ));
	BF_Debug_Print_Debug((" line_bytes.Cr_R		= %d\n", conv_ctrl.dst_img.line_bytes.Cr_R ));
	BF_Debug_Print_Debug((" line_bytes.Alpha	= %d\n", conv_ctrl.dst_img.line_bytes.Alpha ));
	BF_Debug_Print_Debug((" pfmt			= %d\n", conv_ctrl.dst_img.pfmt ));
	BF_Debug_Print_Debug(("---- common info ----\n"));
	BF_Debug_Print_Debug((" resize_mode		= %d\n", conv_ctrl.resize_mode ));
	BF_Debug_Print_Debug((" rotate_deg		= %d\n", conv_ctrl.rotate_deg ));
	BF_Debug_Print_Debug((" fill_enable		= %d\n", conv_ctrl.fill_enable ));
	BF_Debug_Print_Debug((" timeout			= %d\n", conv_ctrl.timeout ));
	BF_Debug_Print_Debug(("====\n"));
#endif
	
	// Cache Clean & Flash
	addr = (UINT32)( M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)conv_ctrl.dst_img.addr.Y_G ) );
	switch( conv_ctrl.dst_img.pfmt ){
		case FJ_IMG_PFMT_YCC420:
			size = (UINT32)( conv_ctrl.dst_img.g_y_g_width * conv_ctrl.dst_img.lines * 3/2 );
			break;
		
		case FJ_IMG_PFMT_YCC422:
			size = (UINT32)( conv_ctrl.dst_img.g_y_g_width * conv_ctrl.dst_img.lines * 2 );
			break;

		case FJ_IMG_PFMT_RGBA8888:
		default:	// for safety
			size = (UINT32)( conv_ctrl.dst_img.g_y_g_width * conv_ctrl.dst_img.lines );
			break;
	}
	BF_L1l2cache_Clean_Flush_Addr( addr, size );
	
	ret = fj_imageconvert( &conv_ctrl );
	if( ret != FJ_ERR_OK ){
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

