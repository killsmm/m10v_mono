/**
 * @file		stm_stitch.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "mode_stm.h"

#include "l1l2cache.h"
#include "fj_memory.h"
#include "ipc_blend.h"
#include "im_xch.h"

#include "ctrl_ive_qr_marker.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_STITCH_MEM_BANK_YC	(2)

#define FLG_QR_MARKER_SYNC		(0x00000001)
#define FLG_META_CLOSE			(0x00000010)
#define FLG_QR_MARKER_STOP		(0x00000100)
/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define M_MAKE_ALIGNMENT( val, align )	(((UINT32)(val) + (align -1)) & (~(align -1)))

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	ULONG					mem_addr_blend_y[D_STITCH_MEM_BANK_YC];
	ULONG					mem_addr_blend_c[D_STITCH_MEM_BANK_YC];
	ULONG					mem_addr_alpha;
	UCHAR					mem_bank_yc;

	ULLONG					qr_marker_task_frame_no;
} T_STITCH_MNG;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_STM_STITCH_IN		g_Stitch_In;
static T_STITCH_MNG			g_Stitch_mng;
static T_SHARE_YUV			g_share_sgde_yuv;
static T_SHARE_YUV			g_share_stitch_yuv;
static T_SHARE_STITCH_INFO	g_share_stitch_info;

#if CO_IVE_QR_MARKER_ON
static T_IVE_QR_MARKER_CONFIG	g_IVE_Para_Config = {
	.qr_start							= {'Q','R','S'}, 
	.qr_senid_of_imindex0				= 0x01,
	.qr_senid_of_imindex1				= 0x03,
	.qr_senid_of_imindex2				= 0x02,
	.qr_senid_of_imindex3				= 0x00,
	.qr_set_primary 					= 0x00,
	.qr_layout_columns					= 0x02,
	.qr_layout_lines					= 0x01,
	.qr_color							= 203,									//E_1BYTE_BW_STD
	.qr_correction_level				= 4,									//E_LEVEL_H
	.qr_camera_orientation				= 3,									//eHorizon
	.qr_rpl 							= {'a','b','c','d','e'},
	.qr_source_width					= {1920, 1920, 1920, 1920}, 			// FHD
	.qr_source_height					= {1080, 1080, 1080, 1080}, 			// FHD
	.qr_rotation						= {0.0, 0.0, 0.0, 0.0},
	.qr_center_x						= {960.0, 960.0, 960.0, 960.0}, 		// FHD
	.qr_center_y						= {540.0, 540.0, 540.0, 540.0}, 		// FHD
	.qr_width							= {1920.0, 1920.0, 1920.0, 1920.0}, 	// FHD
	.qr_height							= {1080.0, 1080.0, 1080.0, 1080.0}, 	// FHD
	.qr_calib_pan						= {0.0, 90.0, 180.0, 270.0},
	.qr_calib_tilt						= {0.0, 0.0, 0.0, 0.0},
	.qr_calib_roll						= {0.0, 0.0, 0.0, 0.0},
	.qr_calib_x 						= {0.0, 0.0, 0.0, 0.0},
	.qr_calib_y 						= {0.0, 0.0, 0.0, 0.0},
	.qr_calib_z 						= {0.0, 0.0, 0.0, 0.0},
	.qr_end 							= {'Q','R','E'}, 
	.qr_used_sen_num					= 0x02,
	.qr_sen_idx							= {0, 1, 2, 3},
};
#endif
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void stitch_proc(ULLONG frame_no);
#if CO_IVE_QR_MARKER_ON
static void marker_dual_draw_proc(T_SHARE_YUV* img_yuv);
#endif

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
INT32 STM_Stitch_Config( const T_STM_STITCH_IN* in, T_STM_STITCH_OUT* const out )
{
	UINT32	mem_addr_ch0, mem_addr_ch1;
	UCHAR	*ptr;
	USHORT	sx = 0, dx = 0, ofs = 0;
	ULONG	x,y;

	// ------------------------------
	// Save
	// ------------------------------
	g_Stitch_In = *in;

	mem_addr_ch0 = g_Stitch_In.available_memory_addr_ch0;
	mem_addr_ch1 = g_Stitch_In.available_memory_addr_ch1;
	
	if((g_Stitch_In.op_ipu_mode != E_SHARE_OP_IPU_MODE_DUAL_STITCH) && (g_Stitch_In.op_ipu_mode != E_SHARE_OP_IPU_MODE_DUAL_QR))
	{
		out->available_memory_addr_ch0 = mem_addr_ch0;
		out->available_memory_addr_ch1 = mem_addr_ch1;
		return D_STM_OK;
	}

	// ------------------------------
	// Global Variable
	// ------------------------------
	if(g_Stitch_In.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_STITCH){
		UPRINTF( "Ipu Mode: Dual Stitch\n");
		UPRINTF( "Stitch Info. => H:%d V:%d Margin:%d\n"
			, g_Stitch_In.yuv_in_h_size
			, g_Stitch_In.yuv_in_v_size 
			, g_Stitch_In.margin_h_size);

		memset( (VOID*)&g_Stitch_mng, 0, sizeof(g_Stitch_mng) );
		memset( (VOID*)&g_share_stitch_info, 0, sizeof(g_share_stitch_info) );
	}else if(g_Stitch_In.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_QR){
		UPRINTF( "Ipu Mode: QR Marker\n");

	}

	memset( (VOID*)&g_share_sgde_yuv, 0, sizeof(g_share_sgde_yuv) );
	memset( (VOID*)&g_share_stitch_yuv, 0, sizeof(g_share_stitch_yuv) );

	// ------------------------------
	// Make Memory Buffer
	// ------------------------------
	if(g_Stitch_In.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_STITCH){
		// blending
		for( x = 0; x < D_STITCH_MEM_BANK_YC; x++ ){
			mem_addr_ch0 = M_MAKE_ALIGNMENT(mem_addr_ch0, 128);
			g_Stitch_mng.mem_addr_blend_y[x] = mem_addr_ch0;
			g_Stitch_mng.mem_addr_blend_c[x] = g_Stitch_mng.mem_addr_blend_y[x] + g_Stitch_In.margin_h_size * g_Stitch_In.yuv_in_v_size;
			mem_addr_ch0 = g_Stitch_mng.mem_addr_blend_c[x] + g_Stitch_In.margin_h_size * g_Stitch_In.yuv_in_v_size;
		}
			
		// alpha
		mem_addr_ch0 = M_MAKE_ALIGNMENT(mem_addr_ch0, 128);
		g_Stitch_mng.mem_addr_alpha = mem_addr_ch0;
		mem_addr_ch0 = g_Stitch_mng.mem_addr_alpha + g_Stitch_In.margin_h_size * g_Stitch_In.yuv_in_v_size;
		
		// ------------------------------
		// Make Alpha Table
		// ------------------------------
		if(g_Stitch_In.margin_h_size > 255)
		{
			// 0 .... 0 1 2 .... 127 128 129 .... 253 254 255 .... 255
			ofs = (g_Stitch_In.margin_h_size - 255) / 2;
			dx = 1;
		}
		else
		{
			// normalize from 0 ~ 255
			ofs = 0;
			dx = 256 / g_Stitch_In.margin_h_size;
		}
			
		ptr = (UCHAR *)g_Stitch_mng.mem_addr_alpha;
		for( y = 0; y < g_Stitch_In.yuv_in_v_size; y++ ) {
			sx = 0;
			for( x = 0; x < g_Stitch_In.margin_h_size; x++ ) {
				*(ptr + (y * g_Stitch_In.margin_h_size) + x) = (UCHAR)sx;
				if((x >= ofs) && (sx < 255)){
					sx = sx + dx;
				}
				if(sx > 255) sx = 255;
			}
		}
		BF_L1l2cache_Flush_Addr((UINT32)g_Stitch_mng.mem_addr_alpha, (UINT32)g_Stitch_In.margin_h_size * g_Stitch_In.yuv_in_v_size);
	}else if(g_Stitch_In.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_QR){

	}
	
	// ------------------------------
	// Output
	// ------------------------------
	out->available_memory_addr_ch0 = mem_addr_ch0;
	out->available_memory_addr_ch1 = mem_addr_ch1;
	
	return D_STM_OK;
}


INT32 STM_Stitch_Start( void)
{
	
	if((g_Stitch_In.op_ipu_mode != E_SHARE_OP_IPU_MODE_DUAL_STITCH) && (g_Stitch_In.op_ipu_mode != E_SHARE_OP_IPU_MODE_DUAL_QR))
	{
		return D_STM_OK;
	}

	if(g_Stitch_In.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_QR){
		OS_User_Clr_Flg( FID_QR_MARKER_TASK, 0 );
		OS_User_Sta_Tsk( TID_QR_MARKER, 0 );
	}

	return D_STM_OK;
}


INT32 STM_Stitch_Event( E_SYSTEM_EVENT event, ULLONG frame_no )
{
	
	if((g_Stitch_In.op_ipu_mode != E_SHARE_OP_IPU_MODE_DUAL_STITCH) && (g_Stitch_In.op_ipu_mode != E_SHARE_OP_IPU_MODE_DUAL_QR))
	{
		return D_STM_OK;
	}

	switch( event )
	{
		case E_SYSTEM_EVENT_SGDE_YUV:
			if(g_Stitch_In.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_STITCH){
				Get_Frame_Share_SGDE_YUV(&g_share_sgde_yuv, frame_no);
				Get_Frame_Share_Stitch_Info(&g_share_stitch_info, frame_no);
				g_share_stitch_yuv = g_share_sgde_yuv;
				Set_Frame_Share_Stitch_YUV(&g_share_stitch_yuv, frame_no);
				stitch_proc(frame_no);
				Snd_Event_to_System( E_SYSTEM_EVENT_STITCH_YUV, frame_no );
			}else if(g_Stitch_In.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_QR){
				Get_Frame_Share_SGDE_YUV(&g_share_sgde_yuv, frame_no);
				Get_Frame_Share_Stitch_Info(&g_share_stitch_info, frame_no);
				g_share_stitch_yuv = g_share_sgde_yuv;
				Set_Frame_Share_Stitch_YUV(&g_share_stitch_yuv, frame_no);
				g_Stitch_mng.qr_marker_task_frame_no = frame_no;
				OS_User_Set_Flg( FID_QR_MARKER_TASK, FLG_QR_MARKER_SYNC );
			}
		break;
		
		default:
		break;
	}
	
	return D_STM_IDLE;
}


INT32 STM_Stitch_Stop( void)
{
	T_OS_USER_RTSK pk_rtsk;
	
	if((g_Stitch_In.op_ipu_mode != E_SHARE_OP_IPU_MODE_DUAL_STITCH) && (g_Stitch_In.op_ipu_mode != E_SHARE_OP_IPU_MODE_DUAL_QR))
	{
		return D_STM_OK;
	}

	if(g_Stitch_In.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_QR){
		OS_User_Set_Flg( FID_QR_MARKER_TASK, FLG_QR_MARKER_STOP );
		OS_User_Ref_Tsk( TID_QR_MARKER, &pk_rtsk );
		while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
			OS_User_Dly_Tsk( 1 );
			OS_User_Ref_Tsk( TID_QR_MARKER, &pk_rtsk );
		}
	}

	return D_STM_IDLE;
}

VOID Tsk_QR_Marker_Main( INT32 stacd )
{
	ULLONG	frame_no;
	OS_USER_FLGPTN	flgptn = 0;
#if CO_IVE_QR_MARKER_ON
	CHAR* MarkerID = NULL;
#endif
	
	UPRINTF( "Tsk_QR_Marker_Main Start\n");

#if CO_IVE_QR_MARKER_ON
	MarkerID = Ctrl_IVE_QR_Marker_ParaInit(&g_IVE_Para_Config);
#endif
	
	while(1){
		OS_User_Wai_Flg( FID_QR_MARKER_TASK, (FLG_QR_MARKER_SYNC | FLG_QR_MARKER_STOP), D_OS_USER_TWF_ORW, &flgptn);

		if( flgptn & FLG_QR_MARKER_STOP ) {
			OS_User_Clr_Flg( FID_QR_MARKER_TASK, ~FLG_QR_MARKER_STOP );
			break;
		}

		
		if( flgptn & FLG_QR_MARKER_SYNC ) {
			OS_User_Clr_Flg( FID_QR_MARKER_TASK, ~FLG_QR_MARKER_SYNC );

			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STITCH, FJ_MBALOG_FUNC_ID_BLEND, FJ_MBALOG_TYPE_START, "QR Marker"));
			frame_no = g_Stitch_mng.qr_marker_task_frame_no;

#if CO_IVE_QR_MARKER_ON
			Ctrl_IVE_QR_Marker_Run(MarkerID);
			marker_dual_draw_proc(&g_share_stitch_yuv);
#endif
	
			Snd_Event_to_System( E_SYSTEM_EVENT_STITCH_YUV, frame_no );
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STITCH, FJ_MBALOG_FUNC_ID_BLEND, FJ_MBALOG_TYPE_END, "QR Marker"));
		}
	}

#if CO_IVE_QR_MARKER_ON
	Ctrl_IVE_QR_Marker_Stop(MarkerID);
#endif

	OS_User_Ext_Tsk();
}


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/

static void stitch_proc(ULLONG frame_no)
{
	T_IPC_IMAGE		in0img;
	T_IPC_IMAGE		in1img;
	T_IPC_IMAGE		outimg;
	T_IPC_IMAGE		alpimg;

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STITCH, FJ_MBALOG_FUNC_ID_BLEND, FJ_MBALOG_TYPE_START, "Stitch"));
	
	g_Stitch_mng.mem_bank_yc	= ( g_Stitch_mng.mem_bank_yc + 1 ) % D_STITCH_MEM_BANK_YC;
	
	// alpha info
	alpimg.imgForm				= E_IPC_IMAGE_FORM_GRAY;
	alpimg.bitDepth				= 8;
	alpimg.dataForm				= E_IPC_IMAGE_DATA_FORM_UNPACK;
	alpimg.imgInfo[0].pImgAdrs	= (VOID*)(g_Stitch_mng.mem_addr_alpha);
	alpimg.imgInfo[0].gImgSize	= g_Stitch_In.margin_h_size;
	alpimg.imgInfo[0].wImgSize	= g_Stitch_In.margin_h_size;
	alpimg.imgInfo[0].hImgSize	= g_Stitch_In.yuv_in_v_size;
	
	/*************************Step 1*************************/
	// blending process for center parts
	if( g_share_stitch_yuv.layout[E_SHARE_YUV_OUT_USER_0].format == E_SHARE_YUV_FORMAT_420_SEMI ){
		in0img.imgForm = E_IPC_IMAGE_FORM_420SP;
		in1img.imgForm = E_IPC_IMAGE_FORM_420SP;
		outimg.imgForm = E_IPC_IMAGE_FORM_420SP;
	}else{
		in0img.imgForm = E_IPC_IMAGE_FORM_422SP;
		in1img.imgForm = E_IPC_IMAGE_FORM_422SP;
		outimg.imgForm = E_IPC_IMAGE_FORM_422SP;
	}
	// image0 info (left)
	in0img.bitDepth = 8;
	in0img.dataForm = E_IPC_IMAGE_DATA_FORM_UNPACK;
	in0img.imgInfo[0].pImgAdrs = (VOID*)(g_share_stitch_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y + g_share_stitch_info.blend_0.dst_ofs);
	in0img.imgInfo[0].gImgSize = g_share_stitch_info.blend_0.g_width;
	in0img.imgInfo[0].wImgSize = g_share_stitch_info.blend_0.width;
	in0img.imgInfo[0].hImgSize = g_share_stitch_info.blend_0.height;
	in0img.imgInfo[1].pImgAdrs = (VOID*)(g_share_stitch_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c + g_share_stitch_info.blend_0.dst_ofs);
	in0img.imgInfo[1].gImgSize = g_share_stitch_info.blend_0.g_width;
	in0img.imgInfo[1].wImgSize = g_share_stitch_info.blend_0.width;
	in0img.imgInfo[1].hImgSize = g_share_stitch_info.blend_0.height;
	// image1 info (right)
	in1img.bitDepth = 8;
	in1img.dataForm = E_IPC_IMAGE_DATA_FORM_UNPACK;
	in1img.imgInfo[0].pImgAdrs = (VOID*)(g_share_stitch_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y + g_share_stitch_info.blend_0.src_ofs);
	in1img.imgInfo[0].gImgSize = g_share_stitch_info.blend_0.g_width;
	in1img.imgInfo[0].wImgSize = g_share_stitch_info.blend_0.width;
	in1img.imgInfo[0].hImgSize = g_share_stitch_info.blend_0.height;
	in1img.imgInfo[1].pImgAdrs = (VOID*)(g_share_stitch_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c + g_share_stitch_info.blend_0.src_ofs);
	in1img.imgInfo[1].gImgSize = g_share_stitch_info.blend_0.g_width;
	in1img.imgInfo[1].wImgSize = g_share_stitch_info.blend_0.width;
	in1img.imgInfo[1].hImgSize = g_share_stitch_info.blend_0.height;
	// output image info
 	outimg = in0img;
	// blending
	IPC_BLEND_Blend(&outimg, &in0img, &in1img, &alpimg);
	
	/*************************Step 2*************************/
	// blending process for left parts
	if( g_share_stitch_yuv.layout[E_SHARE_YUV_OUT_USER_0].format == E_SHARE_YUV_FORMAT_420_SEMI ){
		in0img.imgForm = E_IPC_IMAGE_FORM_420SP;
		in1img.imgForm = E_IPC_IMAGE_FORM_420SP;
		outimg.imgForm = E_IPC_IMAGE_FORM_420SP;
	}else{
		in0img.imgForm = E_IPC_IMAGE_FORM_422SP;
		in1img.imgForm = E_IPC_IMAGE_FORM_422SP;
		outimg.imgForm = E_IPC_IMAGE_FORM_422SP;
	}
	// image0 info (left)
	in0img.bitDepth = 8;
	in0img.dataForm = E_IPC_IMAGE_DATA_FORM_UNPACK;
	in0img.imgInfo[0].pImgAdrs = (VOID*)(g_share_stitch_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y + g_share_stitch_info.blend_1.dst_ofs);
	in0img.imgInfo[0].gImgSize = g_share_stitch_info.blend_1.g_width;
	in0img.imgInfo[0].wImgSize = g_share_stitch_info.blend_1.width;
	in0img.imgInfo[0].hImgSize = g_share_stitch_info.blend_1.height;
	in0img.imgInfo[1].pImgAdrs = (VOID*)(g_share_stitch_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c + g_share_stitch_info.blend_1.dst_ofs);
	in0img.imgInfo[1].gImgSize = g_share_stitch_info.blend_1.g_width;
	in0img.imgInfo[1].wImgSize = g_share_stitch_info.blend_1.width;
	in0img.imgInfo[1].hImgSize = g_share_stitch_info.blend_1.height;
	// image1 info (right)
	in1img.bitDepth = 8;
	in1img.dataForm = E_IPC_IMAGE_DATA_FORM_UNPACK;
	in1img.imgInfo[0].pImgAdrs = (VOID*)(g_share_stitch_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y + g_share_stitch_info.blend_1.src_ofs);
	in1img.imgInfo[0].gImgSize = g_share_stitch_info.blend_1.g_width;
	in1img.imgInfo[0].wImgSize = g_share_stitch_info.blend_1.width;
	in1img.imgInfo[0].hImgSize = g_share_stitch_info.blend_1.height;
	in1img.imgInfo[1].pImgAdrs = (VOID*)(g_share_stitch_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c + g_share_stitch_info.blend_1.src_ofs);
	in1img.imgInfo[1].gImgSize = g_share_stitch_info.blend_1.g_width;
	in1img.imgInfo[1].wImgSize = g_share_stitch_info.blend_1.width;
	in1img.imgInfo[1].hImgSize = g_share_stitch_info.blend_1.height;
	// output image info
 	outimg = in0img;
	// blending
	IPC_BLEND_Blend(&outimg, &in0img, &in1img, &alpimg);

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STITCH, FJ_MBALOG_FUNC_ID_BLEND, FJ_MBALOG_TYPE_END, "Stitch"));

}

#if CO_IVE_QR_MARKER_ON
static void marker_dual_draw_proc(T_SHARE_YUV* img_yuv)
{
	INT32					ercd;
	T_IVE_QR_MARKER_OUT*	marker = NULL;
	T_IM_XCH_COPY_EX		xch_ctrl_copy;
    UINT32					offsetX, shiftOfsX = 0;
    UINT32					offsetY, shiftOfsY = 0;

	marker = Ctrl_IVE_Get_QRMarkerCB();

	xch_ctrl_copy.xch				= E_IM_XCH_CH_SEL_0;
	xch_ctrl_copy.copy_type			= E_IM_XCH_COPY_SEL_NORMAL;
	xch_ctrl_copy.size.width		= marker->width;
	xch_ctrl_copy.size.lines		= marker->lines;
	xch_ctrl_copy.size.src_gl_width	= marker->g_width;
	xch_ctrl_copy.size.dst_gl_width	= img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_byte;

	// For Sensor 0
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)marker->mark_img[0], marker->g_width*marker->g_width);
	xch_ctrl_copy.addr.src = (ULONG)marker->mark_img[0];
	offsetX = 0 + shiftOfsX;
	offsetY = 0 + shiftOfsY;
    //left top
    xch_ctrl_copy.addr.dst = img_yuv->layout[E_SHARE_YUV_OUT_USER_0].addr_y + offsetX + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_byte * offsetY;
	ercd = Im_Xch_Copy_Ex_Sync(&xch_ctrl_copy);
	if( ercd != D_IM_XCH_OK) {
		UPRINTF_ERR(1);
		UPRINTF( "Im_Xch_Copy_Ex_Sync: Error 0-1\n" );
		return;
	}

    //right top
    xch_ctrl_copy.addr.dst = img_yuv->layout[E_SHARE_YUV_OUT_USER_0].addr_y + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_size/2 - (offsetX + marker->width) + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_byte * offsetY;
	ercd = Im_Xch_Copy_Ex_Sync(&xch_ctrl_copy);
	if( ercd != D_IM_XCH_OK) {
		UPRINTF_ERR(1);
		UPRINTF( "Im_Xch_Copy_Ex_Sync: Error 0-2\n" );
		return;
	}

    //left bottom
    xch_ctrl_copy.addr.dst = img_yuv->layout[E_SHARE_YUV_OUT_USER_0].addr_y + offsetX + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_byte * (img_yuv->layout[E_SHARE_YUV_OUT_USER_0].v_size - (offsetY + marker->lines));
	ercd = Im_Xch_Copy_Ex_Sync(&xch_ctrl_copy);
	if( ercd != D_IM_XCH_OK) {
		UPRINTF_ERR(1);
		UPRINTF( "Im_Xch_Copy_Ex_Sync: Error 0-3\n" );
		return;
	}

    //right bottom
    xch_ctrl_copy.addr.dst = img_yuv->layout[E_SHARE_YUV_OUT_USER_0].addr_y + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_size/2 - (offsetX + marker->width) + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_byte * (img_yuv->layout[E_SHARE_YUV_OUT_USER_0].v_size - (offsetY + marker->lines));
	ercd = Im_Xch_Copy_Ex_Sync(&xch_ctrl_copy);
	if( ercd != D_IM_XCH_OK) {
		UPRINTF_ERR(1);
		UPRINTF( "Im_Xch_Copy_Ex_Sync: Error 0-4\n" );
		return;
	}

	// For Sensor 1
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)marker->mark_img[1], marker->g_width*marker->g_width);
	xch_ctrl_copy.addr.src = (ULONG)marker->mark_img[1];
	offsetX = img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_size/2 + shiftOfsX;
	offsetY = 0 + shiftOfsY;
    //left top
    xch_ctrl_copy.addr.dst = img_yuv->layout[E_SHARE_YUV_OUT_USER_0].addr_y + offsetX + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_byte * offsetY;
	ercd = Im_Xch_Copy_Ex_Sync(&xch_ctrl_copy);
	if( ercd != D_IM_XCH_OK) {
		UPRINTF_ERR(1);
		UPRINTF( "Im_Xch_Copy_Ex_Sync: Error 1-1\n" );
		return;
	}

    //right top
    xch_ctrl_copy.addr.dst = img_yuv->layout[E_SHARE_YUV_OUT_USER_0].addr_y + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_size/2 - (offsetX + marker->width) + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_byte * offsetY;
	ercd = Im_Xch_Copy_Ex_Sync(&xch_ctrl_copy);
	if( ercd != D_IM_XCH_OK) {
		UPRINTF_ERR(1);
		UPRINTF( "Im_Xch_Copy_Ex_Sync: Error 1-2\n" );
		return;
	}

    //left bottom
    xch_ctrl_copy.addr.dst = img_yuv->layout[E_SHARE_YUV_OUT_USER_0].addr_y + offsetX + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_byte * (img_yuv->layout[E_SHARE_YUV_OUT_USER_0].v_size - (offsetY + marker->lines));
	ercd = Im_Xch_Copy_Ex_Sync(&xch_ctrl_copy);
	if( ercd != D_IM_XCH_OK) {
		UPRINTF_ERR(1);
		UPRINTF( "Im_Xch_Copy_Ex_Sync: Error 1-3\n" );
		return;
	}

    //right bottom
    xch_ctrl_copy.addr.dst = img_yuv->layout[E_SHARE_YUV_OUT_USER_0].addr_y + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_size/2 - (offsetX + marker->width) + img_yuv->layout[E_SHARE_YUV_OUT_USER_0].h_byte * (img_yuv->layout[E_SHARE_YUV_OUT_USER_0].v_size - (offsetY + marker->lines));
	ercd = Im_Xch_Copy_Ex_Sync(&xch_ctrl_copy);
	if( ercd != D_IM_XCH_OK) {
		UPRINTF_ERR(1);
		UPRINTF( "Im_Xch_Copy_Ex_Sync: Error 1-4\n" );
		return;
	}

}
#endif


