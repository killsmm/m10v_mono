/**
 * @file		user_utility_jpeg.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")
#include "im_jpeg.h"
#include "user_utility_jpeg.h"
#include "share.h"
#include "user.h"
#include "os_if.h"
#include "category_parameter.h"
#include "sdram_map_common.h"
#include "user_sync_task.h"
#ifdef CO_DCF_EXIF_ON
#include <math.h>
#include "fj_arm.h"
#include "fj_exif.h"
#endif

#include "l1l2cache.h"
#include "dcf_if.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_JPEG_PASS1_DWSP D_IM_JPEG_DOWNSP_1_4
#ifdef CO_DCF_EXIF_ON
#define RN_FIX_VAL(n) ( (float)( (n) / 65536.0 ) )
#endif

#define D_JPEG_ENC_EN_MAIN		(0x00)
#define D_JPEG_ENC_EN_THUMB		(0x01)
#define D_JPEG_ENC_EN_MPF		(0x02)

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// Zigzag scan order for Y.
static volatile T_IM_JPEG_QUAT_TBL gStill_Utility_JPEG_ENC_Qtbl_Base_Y = {
	{
		0x10,	0x0B,	0x0A,	0x10,	0x18,	0x28,	0x33,	0x3D,
		0x0C,	0x0C,	0x0E,	0x13,	0x1A,	0x3A,	0x3C,	0x37,
		0x0E,	0x0D,	0x10,	0x18,	0x28,	0x39,	0x45,	0x38,
		0x0E,	0x11,	0x16,	0x1D,	0x33,	0x57,	0x50,	0x3E,
		0x12,	0x16,	0x25,	0x38,	0x44,	0x6D,	0x67,	0x4D,
		0x18,	0x23,	0x37,	0x40,	0x51,	0x68,	0x71,	0x5C,
		0x31,	0x40,	0x4E,	0x57,	0x67,	0x79,	0x78,	0x65,
		0x48,	0x5C,	0x5F,	0x62,	0x70,	0x64,	0x67,	0x63,
		}
};

static volatile const T_IM_JPEG_QUAT_TBL gStill_Utility_JPEG_ENC_Qtbl_Base_Y_Const = {
	{	
		0x10,	0x0B,	0x0A,	0x10,	0x18,	0x28,	0x33,	0x3D,
		0x0C,	0x0C,	0x0E,	0x13,	0x1A,	0x3A,	0x3C,	0x37,
		0x0E,	0x0D,	0x10,	0x18,	0x28,	0x39,	0x45,	0x38,
		0x0E,	0x11,	0x16,	0x1D,	0x33,	0x57,	0x50,	0x3E,
		0x12,	0x16,	0x25,	0x38,	0x44,	0x6D,	0x67,	0x4D,
		0x18,	0x23,	0x37,	0x40,	0x51,	0x68,	0x71,	0x5C,
		0x31,	0x40,	0x4E,	0x57,	0x67,	0x79,	0x78,	0x65,
		0x48,	0x5C,	0x5F,	0x62,	0x70,	0x64,	0x67,	0x63,
		}
};

// Zigzag scan order for Cb/Cr.
static volatile T_IM_JPEG_QUAT_TBL gStill_Utility_JPEG_ENC_Qtbl_Base_C = {
	{
		0x11,	0x12,	0x18,	0x2F,	0x63,	0x63,	0x63,	0x63,
		0x12,	0x15,	0x1A,	0x42,	0x63,	0x63,	0x63,	0x63,
		0x18,	0x1A,	0x38,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x2F,	0x42,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		}
};

static volatile const T_IM_JPEG_QUAT_TBL gStill_Utility_JPEG_ENC_Qtbl_Base_C_Const = {
	{	
		0x11,	0x12,	0x18,	0x2F,	0x63,	0x63,	0x63,	0x63,
		0x12,	0x15,	0x1A,	0x42,	0x63,	0x63,	0x63,	0x63,
		0x18,	0x1A,	0x38,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x2F,	0x42,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		}
};


// JPEG compression ratio control table(LOW)
static const ULONG index_size_tbl_l[100] = {
	0x00000C19, 0x00000C19, 0x00000C49, 0x00000C9D, 0x00000D01, 0x00000D54, 0x00000DAF, 0x00000E07, 0x00000E5D, 0x00000EAA,
	0x00000EFC, 0x00000F52, 0x00000F9E, 0x00000FED, 0x0000103C,	0x00001087, 0x000010D2, 0x00001116, 0x00001163, 0x000011A1,
	0x000011EE, 0x00001237, 0x0000127E, 0x000012C2, 0x00001300,	0x00001343, 0x0000138C, 0x000013D8, 0x0000140F, 0x00001454,
	0x00001490, 0x000014CF, 0x00001525, 0x0000154C, 0x000015A4,	0x000015E3, 0x00001619, 0x00001671, 0x000016A3, 0x000016CB,
	0x00001711, 0x0000177D, 0x00001797, 0x000017F6, 0x0000182F,	0x00001872, 0x0000188A, 0x000018E6, 0x00001952, 0x00001953,
	0x00001956, 0x000019E8, 0x00001A4D, 0x00001A65, 0x00001AAD,	0x00001AF1, 0x00001B7C, 0x00001BF6, 0x00001C24, 0x00001C8B,
	0x00001D23, 0x00001D5E, 0x00001DDF, 0x00001E33, 0x00001EAE,	0x00001F30, 0x00001F76, 0x00002063, 0x00002121, 0x000021BF,
	0x000022A4, 0x00002311, 0x0000242F, 0x0000252C, 0x0000253A,	0x0000261D, 0x00002797, 0x00002901, 0x00002992, 0x00002B2D,
	0x00002C74, 0x00002DE5, 0x00003037, 0x000031F3, 0x000033AD,	0x00003601, 0x00003829, 0x00003BB6, 0x00003D62, 0x000041E9,
	0x000045C2, 0x000049D9, 0x00005031, 0x0000586A, 0x00006223,	0x00007091, 0x00008024, 0x000090CF, 0x0000C0D0, 0x00010000
};

// JPEG compression ratio control table(HIGH)
static const ULONG index_size_tbl_h[100] = {
	0x00000769, 0x00000769, 0x0000081E, 0x00000939, 0x00000A88,	0x00000BA1, 0x00000CC1, 0x00000DC5, 0x00000EC6, 0x00000FC0,
	0x000010A1, 0x00001198, 0x00001272, 0x00001348, 0x00001412,	0x000014E2, 0x000015B0, 0x00001668, 0x0000172F, 0x000017C9,
	0x00001899, 0x00001935, 0x000019F4, 0x00001AA4, 0x00001B33,	0x00001BCB, 0x00001C7D, 0x00001D16, 0x00001DA2, 0x00001E36,
	0x00001EB6, 0x00001F3C, 0x00001FFD, 0x0000205E, 0x000020FC,	0x00002188, 0x000021FC, 0x00002296, 0x000022FF, 0x00002359,
	0x000023E8, 0x00002488, 0x000024D3, 0x0000257B, 0x000025EE,	0x0000266F, 0x000026A9, 0x00002732, 0x00002801, 0x00002814,
	0x00002835, 0x00002917, 0x0000298B, 0x000029D8, 0x00002A54,	0x00002ADF, 0x00002B6C, 0x00002C21, 0x00002C7E, 0x00002CFB,
	0x00002DD1, 0x00002E37, 0x00002F07, 0x00002F88, 0x0000303E,	0x000030D8, 0x0000317B, 0x00003287, 0x00003332, 0x00003439,
	0x0000352D, 0x000035D1, 0x000036FB, 0x00003856, 0x00003893,	0x000039C1, 0x00003B89, 0x00003CF1, 0x00003DF1, 0x00003F99,
	0x00004148, 0x000042C5, 0x0000451A, 0x000046FF, 0x000048E4,	0x00004BA4, 0x00004DEE, 0x00005184, 0x000053F4, 0x000058AB,
	0x00005C7F, 0x0000606B, 0x00006791, 0x00007083, 0x00007BA8,	0x000089CC, 0x00009A10, 0x0000AE8E, 0x0000DE63, 0x00010000
};

static volatile ULONG g_code_size = 0;
static T_JPEG_ENC g_jpeg_enc;
static BOOL	g_ipcu_jpeg_is_enabled = 0;
static UCHAR g_jpeg_encode_en = D_JPEG_ENC_EN_MAIN;
static OS_USER_ID g_jpeg_sem = 0;

// Pass-1
static T_IM_JPEG_ENC_MNG		g_enc_mng;
static T_IM_JPEG_ENC_FRAME_MNG	g_enc_frame_mng;
static UCHAR  g_quality; // 1 to 100
static ULONG* g_index_size_tbl;
static UCHAR  g_jpg_retry_cnt;

static ULONG					g_jpeg_size = 0;
static ULONG					g_jpeg_addr = 0;
static ULLONG					g_jpeg_cnt = 0;
#ifdef CO_DCF_EXIF_ON
static T_BF_DCF_IF_EXIF_INFO	g_jpeg_exif = {0};
static ULONG					g_jpeg_exif_thumbnail_addr = 0;
static ULONG					g_jpeg_exif_thumbnail_size = 0;
static CHAR						g_jpeg_exif_maker_note[32];
#endif

static T_BF_DCF_IF_JPEG_DATA g_dcf_jpg_data;
#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
static T_JPEG_MPF_DATA g_mpf_data;
#endif
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 jpeg_enc_start( const T_JPEG_ENC* param );
static VOID jpeg_enc_callback( T_IM_JPEG_ENC_MNG* jpeg_mng );
static VOID jpeg_codecount_callback( T_IM_JPEG_ENC_MNG* enc_mng );
static VOID jpeg_calc_qtbl(UCHAR quality, T_IM_JPEG_QUAT_TBL_PACK quant_tbl);
static UCHAR jpeg_clip_qtbl( UCHAR object, UCHAR min, UCHAR max );
static UCHAR jpeg_enc_compress_ctrl( T_IM_JPEG_ENC_MNG* enc_mng, UCHAR current_q, UINT32 limit );
#ifdef CO_DCF_EXIF_ON
static VOID jpeg_ipcu_callback_dummy( UCHAR id, UINT32 pdata, UINT32 length, UINT32 cont, UINT32 total_length );
static INT32 jpeg_get_calendar( T_FJ_ARM_IPCU_CALENDAR * calendar );
static INT32 jpeg_set_exif_info( const ULLONG frame_no, FW_USHORT width, FW_USHORT lines, T_BF_DCF_IF_EXIF_INFO * exif_info );
static void jpeg_save_end( FW_INT32 ercd );
static void jpeg_save_end2( FW_INT32 ercd );
#endif

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
INT32 User_Utility_JPEG_Enc( ULLONG frame_no, const T_JPEG_ENC *in, ULONG thumb_addr, ULONG thumb_size )
{
	INT32 rval = 0;

	g_jpeg_enc.format		= in->format;
	g_jpeg_enc.width		= in->width;
	g_jpeg_enc.lines		= in->lines;
	g_jpeg_enc.g_y_width	= in->g_y_width;
	g_jpeg_enc.g_c_width	= in->g_c_width;
	g_jpeg_enc.addr_y		= in->addr_y;
	g_jpeg_enc.addr_c		= in->addr_c;
	g_jpeg_enc.addr_code	= in->addr_code;
	g_jpeg_enc.limit_size	= in->limit_size;
	g_jpeg_enc.quality		= in->quality;
	g_jpeg_encode_en 		= D_JPEG_ENC_EN_MAIN;

#ifdef CO_DCF_EXIF_ON
	g_jpeg_exif_thumbnail_addr = thumb_addr;
	g_jpeg_exif_thumbnail_size = thumb_size;
#endif

	g_jpeg_cnt = frame_no;
	
	rval = jpeg_enc_start(&g_jpeg_enc);
	UPRINTF_ERR(rval);
	
	return rval;
}

INT32 User_Utility_JPEG_Enc_Thumb( ULLONG frame_no, const T_SHARE_YUV* yuv, E_SHARE_YUV_OUT index, ULONG addr_code, ULONG limit_size, ULONG thumb_yuv_addr, UCHAR quality )
{
	FJ_IMG_CONV_CTRL conv_info;
	T_OS_USER_CSEM user_csem;

	memset( &conv_info, 0, sizeof( FJ_IMG_CONV_CTRL ) );

	conv_info.src_img.addr.Y_G				= yuv->layout[index].addr_y;
	conv_info.src_img.addr.Cb_B				= yuv->layout[index].addr_c;
	conv_info.src_img.addr.Cr_R				= conv_info.src_img.addr.Cb_B;
	conv_info.src_img.addr.Alpha			= 0;

	conv_info.src_img.g_y_g_width			= yuv->layout[index].h_size;
	conv_info.src_img.g_cb_b_width			= yuv->layout[index].h_size;
	conv_info.src_img.g_cr_r_width			= conv_info.src_img.g_cb_b_width;
	conv_info.src_img.g_alpha_width			= 0;
	conv_info.src_img.width					= yuv->layout[index].h_size;
	conv_info.src_img.lines					= yuv->layout[index].v_size;
	conv_info.src_img.line_bytes.Y_G		= conv_info.src_img.width;
	conv_info.src_img.line_bytes.Cb_B		= conv_info.src_img.line_bytes.Y_G;
	conv_info.src_img.line_bytes.Cr_R		= conv_info.src_img.line_bytes.Y_G;
	conv_info.src_img.line_bytes.Alpha		= 0;
	conv_info.src_img.pfmt					= yuv->layout[index].format == E_SHARE_YUV_FORMAT_422_SEMI ? FJ_IMG_PFMT_YCC422 : FJ_IMG_PFMT_YCC420;

	conv_info.dst_img.addr.Y_G				= thumb_yuv_addr;
	conv_info.dst_img.addr.Cb_B				= thumb_yuv_addr + WIDTH_THUMB * LINES_THUMB;
	conv_info.dst_img.addr.Cr_R				= conv_info.dst_img.addr.Cb_B;
	conv_info.dst_img.addr.Alpha			= 0;
	conv_info.dst_img.g_y_g_width			= WIDTH_THUMB;
	conv_info.dst_img.g_cb_b_width			= WIDTH_THUMB;
	conv_info.dst_img.g_cr_r_width			= conv_info.dst_img.g_cb_b_width;
	conv_info.dst_img.g_alpha_width			= 0;
	conv_info.dst_img.width					= WIDTH_THUMB;
	conv_info.dst_img.lines					= LINES_THUMB;
	conv_info.dst_img.line_bytes.Y_G		= conv_info.dst_img.width;
	conv_info.dst_img.line_bytes.Cb_B		= conv_info.dst_img.line_bytes.Y_G;
	conv_info.dst_img.line_bytes.Cr_R		= conv_info.dst_img.line_bytes.Y_G;
	conv_info.dst_img.line_bytes.Alpha		= 0;
	conv_info.dst_img.pfmt					= FJ_IMG_PFMT_YCC422; // To fix YUV422 in Thumbnail

	conv_info.resize_mode					= FJ_IMG_RESIZE_MODE_BILINEAR;
	conv_info.rotate_deg					= FJ_DISP_ROTATE_DEGREE_0;
	conv_info.fill_enable					= 1;
	conv_info.timeout						= -1;

	FJ_ImageConvert( &conv_info );

	if( g_jpeg_sem <= 0 ) {
		user_csem.sematr = (D_OS_USER_TA_TFIFO || D_OS_USER_TA_TPRI);
		user_csem.isemcnt = 0;
		user_csem.maxsem = 1;
		g_jpeg_sem = OS_User_Acre_Sem(&user_csem);
	}

	g_jpeg_enc.format = conv_info.dst_img.pfmt == FJ_IMG_PFMT_YCC422 ? E_SHARE_YUV_FORMAT_422_SEMI : E_SHARE_YUV_FORMAT_420_SEMI;
	g_jpeg_enc.width = conv_info.dst_img.width;
	g_jpeg_enc.lines = conv_info.dst_img.lines;
	g_jpeg_enc.g_y_width = conv_info.dst_img.g_y_g_width;
	g_jpeg_enc.g_c_width = conv_info.dst_img.g_cb_b_width;
	g_jpeg_enc.addr_y = conv_info.dst_img.addr.Y_G;
	g_jpeg_enc.addr_c = conv_info.dst_img.addr.Cb_B;
	g_jpeg_enc.addr_code = addr_code;
	g_jpeg_enc.limit_size = limit_size;
	g_jpeg_enc.quality = quality;
	g_jpeg_encode_en = D_JPEG_ENC_EN_THUMB;
	g_jpeg_cnt = frame_no;
	jpeg_enc_start(&g_jpeg_enc);
	OS_User_Wai_Sem(g_jpeg_sem);

	return OK;
}

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
INT32 User_Utility_JPEG_Enc_MPF( ULLONG frame_no, const T_SHARE_YUV* yuv, E_SHARE_YUV_OUT index, ULONG addr_code, ULONG limit_size, UCHAR quality )
{
	T_OS_USER_CSEM user_csem;

	if( g_jpeg_sem <= 0 ) {
		user_csem.sematr = (D_OS_USER_TA_TFIFO || D_OS_USER_TA_TPRI);
		user_csem.isemcnt = 0;
		user_csem.maxsem = 1;
		g_jpeg_sem = OS_User_Acre_Sem(&user_csem);
	}

	g_jpeg_enc.format = yuv->layout[index].format;
	g_jpeg_enc.width = yuv->layout[index].h_size;
	g_jpeg_enc.lines = yuv->layout[index].v_size;
	g_jpeg_enc.g_y_width = yuv->layout[index].h_byte;
	g_jpeg_enc.g_c_width = yuv->layout[index].h_byte;
	g_jpeg_enc.addr_y = yuv->layout[index].addr_y;
	g_jpeg_enc.addr_c = yuv->layout[index].addr_c;
	g_jpeg_enc.addr_code = addr_code;
	g_jpeg_enc.limit_size = limit_size;
	g_jpeg_enc.quality = quality;
	g_jpeg_encode_en = D_JPEG_ENC_EN_MPF;
	g_jpeg_cnt = frame_no;
	jpeg_enc_start(&g_jpeg_enc);

	OS_User_Wai_Sem(g_jpeg_sem);

	return OK;
}

VOID User_Utility_JPEG_Set_MPF( const T_JPEG_MPF_DATA* mpf_data )
{
	memcpy(&g_mpf_data, mpf_data, sizeof(T_JPEG_MPF_DATA));
}
#endif

INT32 User_Utility_JPEG_Get_Result( ULONG* addr, ULONG* size )
{
	*addr = g_jpeg_enc.addr_code;
	*size = g_code_size;
	return OK;
}

/*************************************************************
* JPEG encode callback function, this function will check 
* UVC status to save jpeg file or not.
*************************************************************/
INT32 User_Utility_JPEG_ENC( FW_USHORT width, FW_USHORT lines, const ULLONG frame_no ){
	INT32 ret = 0;
	CHAR filepath[32];
	ULONG fid = 0;
	ULONG ret_wr_size;

	if(User_Utility_is_IPCU_JPEG_Enable() == 0){
		if((g_jpeg_exif_thumbnail_addr == 0) || (g_jpeg_exif_thumbnail_size == 0)){
			User_Utility_JPEG_Get_Result( &g_jpeg_addr, &g_jpeg_size );
			
			FJ_Dcf_Make_New_File_Path_By_Ext("JPG", filepath);
			fid = FJ_Fs_Open(filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC));
			FJ_Dcf_Update_DcfDb_Add_File(filepath);
			FJ_Fs_Write(fid, (UINT32 *)g_jpeg_addr, g_jpeg_size, &ret_wr_size);
			FJ_Fs_Close( fid );
			jpeg_save_end(0);
		}else{
#ifdef CO_DCF_EXIF_ON
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_SAVE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "JPEG Save"));
			User_Utility_JPEG_Get_Result( &g_jpeg_addr, &g_jpeg_size );
			jpeg_set_exif_info( frame_no, width, lines, &g_jpeg_exif );
			ret = User_Utility_JPEG_Save_File( g_jpeg_addr, g_jpeg_size, &g_jpeg_exif );
#endif
		}
	}else if (User_Utility_is_IPCU_JPEG_Enable() == 1){
		if( OS_IF_Rtos_UVC_Is_Header() ){
			OS_IF_Rtos_UVC_MJPEG_Stream_Send(width, lines, frame_no);
		}else{
			OS_IF_Rtos_MJPEG_Stream_Send(width, lines, frame_no);
		}
	}

	return ret;
}


#ifdef CO_DCF_EXIF_ON
INT32 User_Utility_JPEG_Save_File( ULONG addr, ULONG size, const T_BF_DCF_IF_EXIF_INFO* exif )
{
	INT32 ret;
	UPRINTF("User_Utility_JPEG_Save_File\n");
	memset(&g_dcf_jpg_data, 0, sizeof(T_BF_DCF_IF_JPEG_DATA));
	g_dcf_jpg_data.jpg_idx = 0;
	g_dcf_jpg_data.main_addr = addr;
	g_dcf_jpg_data.main_size = size;
	g_dcf_jpg_data.thumb_addr = g_jpeg_exif_thumbnail_addr;
	g_dcf_jpg_data.thumb_size = g_jpeg_exif_thumbnail_size;
#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	g_dcf_jpg_data.screen_data1.screen_addr = g_mpf_data.mpf_enc[0].addr;
	g_dcf_jpg_data.screen_data1.screen_size = g_mpf_data.mpf_enc[0].size;
	g_dcf_jpg_data.screen_data1.screen_width = g_mpf_data.mpf_enc[0].width;
	g_dcf_jpg_data.screen_data1.screen_lines = g_mpf_data.mpf_enc[0].lines;
	g_dcf_jpg_data.screen_data2.screen_addr = g_mpf_data.mpf_enc[1].addr;
	g_dcf_jpg_data.screen_data2.screen_size = g_mpf_data.mpf_enc[1].size;
	g_dcf_jpg_data.screen_data2.screen_width = g_mpf_data.mpf_enc[1].width;
	g_dcf_jpg_data.screen_data2.screen_lines = g_mpf_data.mpf_enc[1].lines;
#endif
	g_dcf_jpg_data.exif_info = *exif;
	if (addr != 0 && size != 0) BF_L1l2cache_Flush_Addr(addr, size);
	if (g_jpeg_exif_thumbnail_addr != 0 && g_jpeg_exif_thumbnail_size != 0) BF_L1l2cache_Flush_Addr(g_jpeg_exif_thumbnail_addr, g_jpeg_exif_thumbnail_size);
#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	if (g_mpf_data.mpf_enc[0].addr != 0 && g_mpf_data.mpf_enc[0].size != 0) BF_L1l2cache_Flush_Addr(g_mpf_data.mpf_enc[0].addr, g_mpf_data.mpf_enc[0].size);
	if (g_mpf_data.mpf_enc[1].addr != 0 && g_mpf_data.mpf_enc[1].size != 0) BF_L1l2cache_Flush_Addr(g_mpf_data.mpf_enc[1].addr, g_mpf_data.mpf_enc[1].size);
#endif
	ret = BF_Dcf_If_Save_Jpeg_As(jpeg_save_end, &g_dcf_jpg_data);
	UPRINTF_ERR(ret);

	return ret;
}

INT32 User_Utility_JPEG_Get_Exif( ULLONG frame_no, T_JPEG_SAVE_FILE2* save )
{
	save->exif = &g_jpeg_exif;
	jpeg_set_exif_info( frame_no, save->main_width, save->main_lines, save->exif );
	return OK;
}

INT32 User_Utility_JPEG_Save_File2( const T_JPEG_SAVE_FILE2* save )
{
	INT32 ret;
	static T_BF_DCF_IF_JPEG_DATA g_dcf_jpg_data2;
	
	memset(&g_dcf_jpg_data2, 0, sizeof(T_BF_DCF_IF_JPEG_DATA));
	
	g_dcf_jpg_data2.jpg_idx = 0;
	g_dcf_jpg_data2.exif_info	= *save->exif;
	g_dcf_jpg_data2.main_addr	= save->main_addr;
	g_dcf_jpg_data2.main_size	= save->main_size;
	g_dcf_jpg_data2.thumb_addr	= save->thumb_addr;
	g_dcf_jpg_data2.thumb_size	= save->thumb_size;
#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	g_dcf_jpg_data2.screen_data1.screen_addr	= save->mpf.mpf_enc[0].addr;
	g_dcf_jpg_data2.screen_data1.screen_size	= save->mpf.mpf_enc[0].size;
	g_dcf_jpg_data2.screen_data1.screen_width	= save->mpf.mpf_enc[0].width;
	g_dcf_jpg_data2.screen_data1.screen_lines	= save->mpf.mpf_enc[0].lines;
	g_dcf_jpg_data2.screen_data2.screen_addr	= save->mpf.mpf_enc[1].addr;
	g_dcf_jpg_data2.screen_data2.screen_size	= save->mpf.mpf_enc[1].size;
	g_dcf_jpg_data2.screen_data2.screen_width	= save->mpf.mpf_enc[1].width;
	g_dcf_jpg_data2.screen_data2.screen_lines	= save->mpf.mpf_enc[1].lines;
#endif
	
	if (g_dcf_jpg_data2.main_addr != 0 && g_dcf_jpg_data2.main_size != 0){
		BF_L1l2cache_Flush_Addr(g_dcf_jpg_data2.main_addr, g_dcf_jpg_data2.main_size);
	}
	if (g_dcf_jpg_data2.thumb_addr != 0 && g_dcf_jpg_data2.thumb_size != 0){
		BF_L1l2cache_Flush_Addr(g_dcf_jpg_data2.thumb_addr, g_dcf_jpg_data2.thumb_size);
	}
#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	if (g_dcf_jpg_data2.screen_data1.screen_addr != 0 && g_dcf_jpg_data2.screen_data1.screen_size != 0){
		BF_L1l2cache_Flush_Addr(g_dcf_jpg_data2.screen_data1.screen_addr, g_dcf_jpg_data2.screen_data1.screen_size);
	}
	if (g_dcf_jpg_data2.screen_data2.screen_addr != 0 && g_dcf_jpg_data2.screen_data2.screen_size != 0){
		BF_L1l2cache_Flush_Addr(g_dcf_jpg_data2.screen_data2.screen_addr, g_dcf_jpg_data2.screen_data2.screen_size);
	}
#endif



	UCHAR detect;
	OS_IF_Rtos_MJPEG_Stream_Send3(save->main_addr, save->main_size, save->exif, save->main_width, save->main_lines, 0);
	FJ_Get_MediaDetect(1, &detect);
	if (detect == 1){
		ret = BF_Dcf_If_Save_Jpeg_As(jpeg_save_end2, &g_dcf_jpg_data2);
		UPRINTF("CARD DETECTED\n");
		UPRINTF_ERR(ret);
	}else{
		jpeg_save_end2(0);
	}
	return OK;
}

BOOL User_Utility_ExifUpdateCB( VOID )
{
	CHAR						date_string[20];
	T_EXIF_RATIONAL				write_buffer;
	T_EXIF_SRATIONAL			write_buffer_s;
	SHORT						write_value_s;

	//DateTime
	sprintf( date_string, "%04d:%02d:%02d %02d:%02d:%02d",
			g_jpeg_exif.date.ad_year, g_jpeg_exif.date.month, g_jpeg_exif.date.day,
			g_jpeg_exif.date.hour, g_jpeg_exif.date.min, g_jpeg_exif.date.sec );
	FJ_SetImageEXIF( FJ_IFD_0, 0x0132, FJ_TYPE_ASCII, 20, 0, ( BYTE * )date_string );

	//ExposureTime
	write_buffer.Num = g_jpeg_exif.exposure_time.nume;
	write_buffer.Den = g_jpeg_exif.exposure_time.denomi;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x829A, FJ_TYPE_RATIONAL, 1, 0, ( BYTE * )&write_buffer );

	//FNumber
	write_buffer.Num = g_jpeg_exif.f_value.nume;
	write_buffer.Den = g_jpeg_exif.f_value.denomi;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x829D, FJ_TYPE_RATIONAL, 1, 0, ( BYTE * )&write_buffer );

	//ExposureProgram
	write_value_s = g_jpeg_exif.exposure_prog;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x8822, FJ_TYPE_SHORT, 1, 0, ( BYTE * )&write_value_s );

	//ISOSpeedRatings
	write_value_s = g_jpeg_exif.iso_value;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x8827, FJ_TYPE_SHORT, 1, 0, ( BYTE * )&write_value_s );

	//ShutterSpeedValue
	write_buffer.Num = g_jpeg_exif.shutter_speed.nume;
	write_buffer.Den = g_jpeg_exif.shutter_speed.denomi;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x9201, FJ_TYPE_SRATIONAL, 1, 0, ( BYTE * )&write_buffer );

	//ApertureValue
	write_buffer.Num = g_jpeg_exif.aperture.nume;
	write_buffer.Den = g_jpeg_exif.aperture.denomi;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x9202, FJ_TYPE_RATIONAL, 1, 0, ( BYTE * )&write_buffer );

	//ExposureBiasValue
	write_buffer_s.Num = ( LONG )round( g_jpeg_exif.ev * 10 );
	write_buffer_s.Den = 10;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x9204, FJ_TYPE_SRATIONAL, 1, 0, ( BYTE * )&write_buffer_s );

	//MaxApertureValue
	write_buffer.Num = g_jpeg_exif.max_aperture.nume;
	write_buffer.Den = g_jpeg_exif.max_aperture.denomi;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x9205, FJ_TYPE_RATIONAL, 1, 0, ( BYTE * )&write_buffer );

	//MeteringMode
	write_value_s = g_jpeg_exif.metering;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x9207, FJ_TYPE_SHORT, 1, 0, ( BYTE * )&write_value_s );

	//LightSource
	write_value_s = g_jpeg_exif.light_source;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x9208, FJ_TYPE_SHORT, 1, 0, ( BYTE * )&write_value_s );

	//Flash
	write_value_s = g_jpeg_exif.flash;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x9209, FJ_TYPE_SHORT, 1, 0, ( BYTE * )&write_value_s );

	//FocalLength
	write_buffer.Num = g_jpeg_exif.focal_length.nume;
	write_buffer.Den = g_jpeg_exif.focal_length.denomi;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0x920A, FJ_TYPE_RATIONAL, 1, 0, ( BYTE * )&write_buffer );

	//PixelXDimension
	write_value_s = g_jpeg_exif.width;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0xA002, FJ_TYPE_SHORT, 1, 0, ( BYTE * )&write_value_s );

	//PixelYDimension
	write_value_s = g_jpeg_exif.lines;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0xA003, FJ_TYPE_SHORT, 1, 0, ( BYTE * )&write_value_s );

	//DigitalZommRatio
	write_buffer.Num = g_jpeg_exif.dzoom_ratio.nume;
	write_buffer.Den = g_jpeg_exif.dzoom_ratio.denomi;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0xA404, FJ_TYPE_RATIONAL, 1, 0, ( BYTE * )&write_buffer );

	//Sharpness
	write_value_s = g_jpeg_exif.sharpness;
	FJ_SetImageEXIF( FJ_IFD_EXIF, 0xA40A, FJ_TYPE_SHORT, 1, 0, ( BYTE * )&write_value_s );

	return TRUE;
}

INT32 User_Utility_CreateMakerNoteCB(CHAR * byBuffer, INT32 Size, INT32 Offset, INT32 * ParamArray, INT32 ParamNum)
{
	INT32 ret;

	if( Size != ( strlen( g_jpeg_exif_maker_note ) + 1 ) ) {
		return -1;
	}

	ret = snprintf( byBuffer, Size, "%s", g_jpeg_exif_maker_note );

	if( ret > 0 ) {
		return 1;
	}

	return -1;
}
#endif // CO_DCF_EXIF_ON

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static INT32 jpeg_enc_start( const T_JPEG_ENC* param )
{
	T_IM_JPEG_QUAT_TBL local_qtbl0, local_qtbl1;
	T_IM_JPEG_QUAT_TBL_PACK		quant_tbl;
	T_IM_JPEG_ENC_MNG			enc_mng = {
		.mem_format					= E_IM_JPEG_MEM_FORM_PLANE_DOT,
		.component[0].quant_tbl_no	= 0,	// Y
		.component[0].huf_dc_tbl_no	= 0,	// Y
		.component[0].huf_ac_tbl_no	= 0,	// Y
		.component[1].quant_tbl_no	= 1,	// CB
		.component[1].huf_dc_tbl_no	= 1,	// CB
		.component[1].huf_ac_tbl_no	= 1,	// CB
		.component[2].quant_tbl_no	= 1,	// CR
		.component[2].huf_dc_tbl_no	= 1,	// CR
		.component[2].huf_ac_tbl_no	= 1,	// CR
		.skip_mk_flg				= D_IM_JPEG_ENABLE_OFF,
		.dri_mk_num					= 0,
		.exif_fmt_flg				= D_IM_JPEG_ENABLE_OFF,
		.pint_line					= 0,
		.pint_sect					= 0,
		.pbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE,
		.pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_4,
		.pbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF,
		.pbuf_ctrl.prot_type		= D_IM_JPEG_D_SEC_NORMAL_ACCESS,
		.pbuf_ctrl.err_state		= D_IM_JPEG_AXI_OKAY,
		.jbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE,
		.jbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_4,
		.jbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF,
		.jbuf_ctrl.prot_type		= D_IM_JPEG_D_SEC_NORMAL_ACCESS,
		.jbuf_ctrl.err_state		= D_IM_JPEG_AXI_OKAY,	// output parameter.
		.pburst_length				= E_IM_JPEG_BURST_INCR_8,
		.jburst_length				= E_IM_JPEG_BURST_INCR_8,
		.bit_depth					= E_IM_JPEG_BIT_DEPTH_8BIT,
		.burst_alignment			= E_IM_JPEG_BURST_AL_ON,
		.port_mode					= E_IM_JPEG_IIP_PORT_MODE_OFF,
		.ring_on					= D_IM_JPEG_ENABLE_OFF,
		.block_size					= E_IM_JPEG_IIP_BLOCK_64x8,
		.ring_size					= 1,
		.code_size					= 0,	// output parameter.
		.result						= 0,	// output parameter.
	};
	T_IM_JPEG_ENC_FRAME_MNG		enc_frame_mng = {
 		.code_count_flg				= D_IM_JPEG_ENABLE_ON, // Pass-1 ON
		.code_over_count_flg		= D_IM_JPEG_ENABLE_OFF,
		.limit_size					= 0,
	};
	INT32 ret;
	
	// global
	g_code_size = 0;
	g_quality = param->quality;//95;		 // default quality (1-100)
	g_jpg_retry_cnt = 0; // Pass-1
	g_index_size_tbl = (ULONG*)&index_size_tbl_l; // Pass-1
	
	// clock
	Dd_Top_Set_CLKSTOP3_JPGAH( 0 );
	Dd_Top_Set_CLKSTOP3_JPGAX( 0 );
	Dd_Top_Set_CLKSTOP3_JPGCK( 0 );
	
 	quant_tbl.quantization_tbl_0 = &local_qtbl0;
	quant_tbl.quantization_tbl_1 = &local_qtbl1;
	quant_tbl.quantization_tbl_2 = &local_qtbl0;
	quant_tbl.quantization_tbl_3 = &local_qtbl1;
	jpeg_calc_qtbl( g_quality, quant_tbl);
	Im_JPEG_Set_QTbl( &quant_tbl, 0 );
	// Ctrl
	if( enc_frame_mng.code_count_flg == D_IM_JPEG_ENABLE_ON ){
	// Pass-1 ON
		enc_mng.pcallback = jpeg_codecount_callback;
	}else{
		enc_mng.pcallback = jpeg_enc_callback;
	}
	switch( param->format ){
	case E_SHARE_YUV_FORMAT_422_SEMI:
		enc_mng.mem_format = E_IM_JPEG_MEM_FORM_PLANE_DOT;
		enc_mng.ycc_smpl = E_IM_JPEG_SMPL_TYPE_YCC422;
		break;
	case E_SHARE_YUV_FORMAT_420_SEMI:
		enc_mng.mem_format = E_IM_JPEG_MEM_FORM_PLANE_DOT;
		enc_mng.ycc_smpl = E_IM_JPEG_SMPL_TYPE_YCC420;
		break;
	default:
		return -1;
	}
	enc_mng.width = param->width;
	enc_mng.lines = param->lines;
	ret = Im_JPEG_Ctrl_Enc( &enc_mng );
	g_enc_mng = enc_mng; // save param
	if( ret ){
		return ret;
	}
	
	if( enc_frame_mng.code_count_flg == D_IM_JPEG_ENABLE_ON ){
	// Pass-1 ON
		Im_JPEG_Set_Down_Sampling_Rate( D_JPEG_PASS1_DWSP );
	}
	
	// Frame
	enc_frame_mng.global_y_width	= param->g_y_width;
	enc_frame_mng.global_c_width	= param->g_c_width;
	enc_frame_mng.ycc_addr.y		= param->addr_y;
	enc_frame_mng.ycc_addr.c		= param->addr_c;
	enc_frame_mng.code_addr			= param->addr_code;
	enc_frame_mng.limit_size		= param->limit_size;
	//BF_L1l2cache_Clean_Flush_Addr( param->addr_code, (((UINT32)param->width) * param->lines) );
	ret = Im_JPEG_Ctrl_Enc_Frame( &enc_frame_mng );
	g_enc_frame_mng = enc_frame_mng; // save param
	if( ret ){
		return ret;
	}
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC_ACTUAL, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "JPEG Enc %d", (INT32)g_jpeg_cnt));
	ret = Im_JPEG_Start_Enc();
	if( ret ){
		return ret;
	}
	return ret;
}

static VOID jpeg_codecount_callback( T_IM_JPEG_ENC_MNG* enc_mng )
{
	T_IM_JPEG_QUAT_TBL_PACK quant_tbl;
	T_IM_JPEG_QUAT_TBL local_qtbl0, local_qtbl1;
	ULONG code_size = enc_mng->code_size << ( D_JPEG_PASS1_DWSP * 2 ); // without down sampling
	
	if( code_size > g_enc_frame_mng.limit_size && g_quality != 1 ){
		// retry code count
		g_quality = jpeg_enc_compress_ctrl( enc_mng, g_quality, g_enc_frame_mng.limit_size * 0.9); // 10% margin
	 	quant_tbl.quantization_tbl_0 = &local_qtbl0;
		quant_tbl.quantization_tbl_1 = &local_qtbl1;
		quant_tbl.quantization_tbl_2 = &local_qtbl0;
		quant_tbl.quantization_tbl_3 = &local_qtbl1;
		jpeg_calc_qtbl( g_quality, quant_tbl );
		Im_JPEG_Set_QTbl( &quant_tbl, 0 );
		Im_JPEG_Start_Enc();
	}else{
		// encode & save JPEG
		g_enc_mng.pcallback = jpeg_enc_callback;
		g_enc_frame_mng.code_count_flg = D_IM_JPEG_ENABLE_OFF;
		Im_JPEG_Ctrl_Enc( &g_enc_mng );
		Im_JPEG_Set_Down_Sampling_Rate( D_IM_JPEG_DOWNSP_NONE );
		Im_JPEG_Ctrl_Enc_Frame( &g_enc_frame_mng );
		Im_JPEG_Start_Enc();
	}
}

static VOID jpeg_enc_callback( T_IM_JPEG_ENC_MNG* jpeg_mng )
{
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC_ACTUAL, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "JPEG Enc"));
	// UPRINTF("jpeg_enc_callback\n");
	g_code_size = jpeg_mng->code_size;

	if( g_jpeg_encode_en == D_JPEG_ENC_EN_THUMB ) {
		OS_User_Sig_Sem( g_jpeg_sem );
	}else if( g_jpeg_encode_en == D_JPEG_ENC_EN_MPF ) {
		OS_User_Sig_Sem( g_jpeg_sem );
	}else {
		Snd_Event_to_User( E_USER_EVENT_JPG_ENC_END, g_jpeg_cnt );
	}

	g_jpeg_encode_en = D_JPEG_ENC_EN_MAIN;
}


#ifdef CO_DCF_EXIF_ON
static VOID jpeg_ipcu_callback_dummy( UCHAR id, UINT32 pdata, UINT32 length, UINT32 cont, UINT32 total_length )
{
	UPRINTF_ERR( id );
}

static INT32 jpeg_get_calendar( T_FJ_ARM_IPCU_CALENDAR * calendar )
{
#ifdef CO_TKERNEL_ONLY
	return NG;
#else
	INT32						ret = NG;
	UCHAR						req_id;

	if( calendar == NULL ) {
		return ret;
	}

	ret = FJ_IPCU_Open( E_FJ_IPCU_MAILBOX_TYPE_14, &req_id );

	if (FJ_ERR_OK != ret) {
		return ret;
	}
	else{
		ret = FJ_IPCU_SetReceiverCB( req_id, jpeg_ipcu_callback_dummy );

		if ( FJ_ERR_OK == ret ) {
			ret = FJ_IPCU_Get_Calendar_CM0( calendar );

			if ( FJ_ERR_OK == ret ) {
				ret = OK;
			}
		}
	}

	FJ_IPCU_Close( req_id );

	return ret;
#endif
}

static INT32 jpeg_set_exif_info( const ULLONG frame_no, FW_USHORT width, FW_USHORT lines, T_BF_DCF_IF_EXIF_INFO * exif_info )
{
	INT32						ret;
	T_AE_PARAM					ae_param;
	T_AWB_PARAM					awb_param;
	T_SHARE_APEX				apex_param;
	T_FJ_ARM_IPCU_CALENDAR		calendar;
	DOUBLE						double_tmp;

	Cate_Get_AE_Param( &ae_param, D_CONFIG_MODE_CAP );
	Cate_Get_AWB_Param( &awb_param );
	Get_Frame_Share_APEX( &apex_param, frame_no );
	memset( exif_info, 0, sizeof( T_BF_DCF_IF_EXIF_INFO ) );

	exif_info->width					= width;
	exif_info->lines					= lines;
	exif_info->ev						= (double)( (double)apex_param.sensor[0].bias_EV / (double)65536.0);

	ret = jpeg_get_calendar( &calendar );

	if( ret == OK ) {
		exif_info->date.ad_year			= calendar.ipcu_year + 2000;
		exif_info->date.month			= calendar.ipcu_month;
		exif_info->date.day				= calendar.ipcu_day;
		exif_info->date.hour			= calendar.ipcu_hour;
		exif_info->date.min				= calendar.ipcu_minute;
		exif_info->date.sec				= calendar.ipcu_second;
	}
	else {
		exif_info->date.ad_year			= 2000;
		exif_info->date.month			= 1;
		exif_info->date.day				= 1;
		exif_info->date.hour			= 0;
		exif_info->date.min				= 0;
		exif_info->date.sec				= 0;
	}

	double_tmp = pow(2.0 ,RN_FIX_VAL(apex_param.sensor[0].TV));
	exif_info->exposure_time.nume	= 10;
	exif_info->exposure_time.denomi = (ULONG)(double_tmp * 10);

	exif_info->f_value.nume				= round(pow(2, RN_FIX_VAL( apex_param.sensor[0].AV) / 2) * 10.0);
	exif_info->f_value.denomi			= 10;

	UPRINTF("AV = %d f value = %d\n", apex_param.sensor[0].AV, exif_info->f_value.nume);

	exif_info->exposure_prog			= E_BF_DCF_IF_EXPOSURE_NORMAL;

	exif_info->shutter_speed.nume		= (UINT32)(RN_FIX_VAL(apex_param.sensor[0].TV) * 10.0);
	exif_info->shutter_speed.denomi		= 10;

	exif_info->aperture.nume			= round(pow(2, RN_FIX_VAL( apex_param.sensor[0].AV) / 2) * 10.0);
	exif_info->aperture.denomi			= 10;

	exif_info->max_aperture.nume		= exif_info->aperture.nume;
	exif_info->max_aperture.denomi		= exif_info->aperture.denomi;

	switch( ae_param.mode ) {
		case 0x01:
			exif_info->metering			= E_BF_DCF_IF_METERING_CENTER_WEIGHTED;
			break;

		case 0x02:
			exif_info->metering			= E_BF_DCF_IF_METERING_AVERAGE;
			break;

		case 0x03:
			exif_info->metering			= E_BF_DCF_IF_METERING_CENTER_WEIGHTED;
			break;

		case 0x04:
			exif_info->metering			= E_BF_DCF_IF_METERING_CENTER_WEIGHTED;
			break;

		case 0x05:
			exif_info->metering			= E_BF_DCF_IF_METERING_SPOT;
			break;

		case 0x07:
			exif_info->metering			= E_BF_DCF_IF_METERING_OTHER;
			break;

		default:
			exif_info->metering			= E_BF_DCF_IF_METERING_UNKNOWN;
			break;
	}

	switch( awb_param.mode ) {
		case 0x01:
			exif_info->light_source		= 0x0a;
			break;

		case 0x02:
			exif_info->light_source		= 0x09;
			break;

		case 0x03:
			exif_info->light_source		= 0x04;
			break;

		case 0x04:
			exif_info->light_source		= 0x02;
			break;

		case 0x07:
			exif_info->light_source		= 0x03;
			break;

		default:
			exif_info->light_source		= 0x00;
			break;
	}

	exif_info->flash					= 0;

	exif_info->dzoom_ratio.nume			= 1;
	exif_info->dzoom_ratio.denomi		= 1;

	double_tmp = pow(2.0 ,RN_FIX_VAL(apex_param.sensor[0].SV));
	UPRINTF("double_tmp = %f\n", double_tmp);
	exif_info->iso_value				= (USHORT)(double_tmp * 3.125);
	UPRINTF("exif_info->iso_value = %d\n", exif_info->iso_value);

	float focal_length = 0;
	Cate_Get_Exif_focal_length(&focal_length);

	exif_info->focal_length.nume		= (UINT32)(focal_length * 10);
	exif_info->focal_length.denomi		= 10;
	UPRINTF("exif focal length = %f\n", focal_length);
	exif_info->sharpness				= 0;
	Cate_Get_Exif_Maker_Note( g_jpeg_exif_maker_note );
	exif_info->maker_note_length		= strlen( g_jpeg_exif_maker_note ) + 1;
	exif_info->qvga_enable				= 0;

	return OK;
}

static void jpeg_save_end( FW_INT32 ercd )
{
	UPRINTF_ERR(ercd);
	Snd_Event_to_User( E_USER_EVENT_JPG_SAVE_END, 0 );
}

static void jpeg_save_end2( FW_INT32 ercd )
{
	T_USER_SYNC_MSG user_sync_msg;
	OS_USER_ER os_err = 0;
	
	UPRINTF_ERR(ercd);
	Snd_Event_to_User( E_USER_EVENT_JPG_SAVE_END, 0 );
	
	user_sync_msg.event		= E_USER_SYNC_EVENT_JPG_SAVE_END;
	user_sync_msg.frame_no	= -1;
	os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
	UPRINTF_ERR(os_err);
}
#endif

static UCHAR jpeg_enc_compress_ctrl( T_IM_JPEG_ENC_MNG* enc_mng, UCHAR current_q, UINT32 limit )
{
	ULONG			next_size_index = 0;
	UCHAR			next_q = 100;
	UINT32			yuv_size;	  // original size(byte)
	UINT32			pix_size;     // original size(pix)
 	UINT32			current_size; // code size
	UINT32			qmax_size;	  // size of jpeg file (q table values are all 1)
	UCHAR			comp;		  // compression ratio (percent)
	E_IM_JPEG_SMPL_TYPE ycc_smpl;

	ycc_smpl	 = enc_mng->ycc_smpl;
	pix_size	 = enc_mng->width * enc_mng->lines;
	current_size = enc_mng->code_size << ( D_JPEG_PASS1_DWSP * 2 ); // without down sampling
	
	if( g_jpg_retry_cnt == 0 ){
		if( ycc_smpl == E_IM_JPEG_SMPL_TYPE_YCC422 ){
			yuv_size  = pix_size << 1;			 // Size of YUV (422)
		}else{
			yuv_size  = (pix_size * 3 ) >> 1;	 // Size of YUV (420)
		}
		qmax_size = (( current_size << 8 ) / index_size_tbl_l[current_q - 1] ) << 8;
		comp 	  = (( qmax_size * 100 ) / yuv_size );
		if( comp > 30 ){
			g_index_size_tbl = (ULONG*)&index_size_tbl_h;
		}
		else{
			g_index_size_tbl = (ULONG*)&index_size_tbl_l;
		}
	}
 	next_size_index = ( g_index_size_tbl[current_q - 1] * (( limit << 8 ) / current_size )) >> 8;
	while( g_index_size_tbl[next_q -1] > next_size_index ){
		if( next_q == 1 ){
			break;
		}
		next_q--;
	}
	g_jpg_retry_cnt++;

	return next_q;
}

static VOID jpeg_calc_qtbl(UCHAR quality, T_IM_JPEG_QUAT_TBL_PACK quant_tbl)
{
	UINT32 cnt;
	float qscale;

	if( quality < 50 ){
		qscale = 50.0 / quality;
	}else{
		qscale = (100.0 - quality ) / 50.0;
	}
 	for( cnt=0;cnt<64;cnt++){
  		quant_tbl.quantization_tbl_0->quant_value[cnt] = jpeg_clip_qtbl( qscale * gStill_Utility_JPEG_ENC_Qtbl_Base_Y.quant_value[cnt] + 0.5, 1, 255 );
 	}
 	for( cnt=0;cnt<64;cnt++){
 		quant_tbl.quantization_tbl_1->quant_value[cnt] = jpeg_clip_qtbl( qscale * gStill_Utility_JPEG_ENC_Qtbl_Base_C.quant_value[cnt] + 0.5, 1, 255 );
 	}
 	for( cnt=0;cnt<64;cnt++){
 		quant_tbl.quantization_tbl_2->quant_value[cnt] = jpeg_clip_qtbl( qscale * gStill_Utility_JPEG_ENC_Qtbl_Base_Y.quant_value[cnt] + 0.5, 1, 255 );
	}
 	for( cnt=0;cnt<64;cnt++){
 		quant_tbl.quantization_tbl_3->quant_value[cnt] = jpeg_clip_qtbl( qscale * gStill_Utility_JPEG_ENC_Qtbl_Base_C.quant_value[cnt] + 0.5, 1, 255 );
	}
}

static UCHAR jpeg_clip_qtbl( UCHAR object, UCHAR min, UCHAR max )
{
	if( object > max ){
		return max;
	}
	else if( object < min ){
		return min;
	}
	else{
		return object;
	}
}

void User_Utility_IPCU_JPEG_Enable(BOOL en)
{
	g_ipcu_jpeg_is_enabled = en;
}

BOOL User_Utility_is_IPCU_JPEG_Enable(void)
{
	return g_ipcu_jpeg_is_enabled;
}



