/**
 * @file		ctrl_cnr_ofl.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */

#include "mode_stm.h"
#include "ctrl_cnr_ofl.h"
#include "im_cnr.h"
#include "im_xch.h"
#include "still_img_info.h"
#include "still.h"
#include "sdram_map_cache_define.h"
#include "iqpara_api.h"
#include "iq_bin_dbg.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
//#define DBG_CNR_OFL_DUMP_YUV
//#define DBG_CNR_OFL_DUMP_REG

#define D_CNR_OFL_EXMODE_X1		(0)
#define D_CNR_OFL_EXMODE_X2		(1)
#define D_CNR_OFL_EXMODE_X4		(2)


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define M_ROUNDUP_8( val )	(((UINT32)(val)  +7) & 0xFFFFFFF8)
#define M_ROUNDUP_16( val )	(((UINT32)(val) +15) & 0xFFFFFFF0)


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	ULONG				addr_y;
	ULONG				addr_c;
	USHORT				g_y_width;
	USHORT				g_c_width;
	USHORT				width;
	USHORT				lines;
	E_SHARE_YUV_FORMAT	format;
} T_CNR_OFL_EX_INFO;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static ULONG gCnr_Ofl_Ex_Addr = 0;
static ULONG gCnr_Ofl_Work_Addr = 0;


/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static VOID cnr_ofl_padding16lines( const T_CNR_OFL_EX_INFO* const src_img, const USHORT lines );
static VOID cnr_ofl_reduce( const T_SHARE_LAYOUT* const src_img, const T_CNR_OFL_EX_INFO* const dst_img );
static VOID cnr_ofl_expand_c_only( const T_BF_STILL_IMG* const src_img, const T_BF_STILL_IMG* const dst_img );
static VOID cnr_ofl_callback( UCHAR ch, ULONG interrupt_flag_mask, UINT32 user_param );

/*==============================================================================*/
/* Debug Function																*/
/*==============================================================================*/
#ifdef DBG_CNR_OFL_DUMP_YUV
#include "fj_filesystem.h"
static void debug_save_file( CHAR* fname, ULONG addr, ULONG size )
{
	ULONG fid;
	ULONG write_size = 0;

	fid = FJ_Fs_Open ( fname, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	FJ_Fs_Write ( fid, (VOID*)addr, size, &write_size );
	FJ_Fs_Close( fid );
}
#endif

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
VOID Ctrl_CNR_OFL_Init(ULONG ex_addr, ULONG work_addr)
{
	gCnr_Ofl_Ex_Addr = ex_addr;
	gCnr_Ofl_Work_Addr = work_addr;
}

BOOL Ctrl_CNR_OFL_Process(const T_SHARE_LAYOUT* const yuv)
{
	UCHAR				ch = 0, ex_mode = 0, div = 1;
	T_BF_STILL_IMG		reduce_img;
	T_BF_STILL_IMG		expand_img;
	T_IM_CNR_OFL_CTRL	spr_ctrl;
	T_CNR_OFL_EX_INFO 	cnr_ex_info;
	INT32				im_ercd;
	T_IQ_PARAM_CNR_CTRL ctrl_0, ctrl_1;

	// judge if cnr-ofl process is needed by iq parameter setting
	IQPara_Get_IQ_CNR_Ctrl(&ctrl_0, &ctrl_1);
	if(ctrl_0.CNRMODE == 1){
		// cnr-otf mode processed, no need cnr-ofl process
		UPRINTF("CNR-OFL Disabled!\n");
		return TRUE;
	}
	ex_mode = ctrl_0.EXSPRMODE;

	// reduce image for ex mode
	if(ex_mode > D_CNR_OFL_EXMODE_X1){
		if(ex_mode == D_CNR_OFL_EXMODE_X2){
			div = 2;
			UPRINTF("CNR-OFL ExMode(x2)\n");
		}else{
			div = 4;
			UPRINTF("CNR-OFL ExMode(x4)\n");
		}
		cnr_ex_info.g_y_width = M_ROUNDUP_16(yuv->h_byte / div);
		cnr_ex_info.g_c_width = cnr_ex_info.g_y_width;
		cnr_ex_info.width = yuv->h_size / div;
		cnr_ex_info.lines = yuv->v_size / div;
		cnr_ex_info.format = yuv->format;
		cnr_ex_info.addr_y = gCnr_Ofl_Ex_Addr;
		cnr_ex_info.addr_c = cnr_ex_info.addr_y + cnr_ex_info.g_y_width * M_ROUNDUP_16(cnr_ex_info.lines);

		// reduce
		cnr_ofl_reduce(yuv, &cnr_ex_info);
	}else{
		UPRINTF("CNR-OFL ExMode(x1)\n");
		cnr_ex_info.g_y_width = M_ROUNDUP_16(yuv->h_byte);
		cnr_ex_info.g_c_width = cnr_ex_info.g_y_width;
		cnr_ex_info.width = yuv->h_size;
		cnr_ex_info.lines = yuv->v_size;
		cnr_ex_info.format = yuv->format;
		cnr_ex_info.addr_y = yuv->addr_y;
		cnr_ex_info.addr_c = yuv->addr_c;
	}
	
	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		UPRINTF_ERR(im_ercd);
		return FALSE;
	}
	
	// image size
	spr_ctrl.width		= M_ROUNDUP_16(cnr_ex_info.width);
	spr_ctrl.lines		= M_ROUNDUP_16(cnr_ex_info.lines);
	
	// The src image line padding, multiple of 16 lines.
	cnr_ofl_padding16lines( &cnr_ex_info, spr_ctrl.lines );
	
	// Setting YC output Mode
	if( cnr_ex_info.format == E_SHARE_YUV_FORMAT_420_SEMI ) {
		spr_ctrl.mode	= E_IM_CNR_OFL_YCC_MODE_YCC420_SIMPLE;
	}
	else {
		spr_ctrl.mode	= E_IM_CNR_OFL_YCC_MODE_YCC422_SIMPLE;
	}
	
	spr_ctrl.divide_top = E_IM_CNR_VPROCESS_NORM;		// not divide
	spr_ctrl.divide_bot = E_IM_CNR_VPROCESS_NORM;		// not divide
	spr_ctrl.pCallBack	= cnr_ofl_callback;				// for Error handling.
	spr_ctrl.user_param = ch;
	
	// src
	spr_ctrl.r_y_width	= cnr_ex_info.g_y_width;
	spr_ctrl.r_c_width	= cnr_ex_info.g_c_width;
	spr_ctrl.r_y_addr	= cnr_ex_info.addr_y;
	spr_ctrl.r_cb_addr	= cnr_ex_info.addr_c;
	spr_ctrl.r_cr_addr	= cnr_ex_info.addr_c;
	
	// dst
	spr_ctrl.w_y_width	= cnr_ex_info.g_y_width;
	spr_ctrl.w_c_width	= cnr_ex_info.g_c_width;
	spr_ctrl.w_y_addr	= cnr_ex_info.addr_y;
	spr_ctrl.w_cb_addr	= cnr_ex_info.addr_c;
	spr_ctrl.w_cr_addr	= cnr_ex_info.addr_c;
	
	// spr work address
	spr_ctrl.work_addr	= gCnr_Ofl_Work_Addr;
	
	// set Ctrl
	im_ercd = Im_CNR_OFL_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		(VOID)Im_CNR_OFL_Close( ch );
		UPRINTF_ERR(im_ercd);
		return FALSE;
	}
	
	//IQ update by PIPE process

	// start cnr ofl
	im_ercd = Im_CNR_OFL_Start_Sync( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		(VOID)Im_CNR_OFL_Close( ch );
		UPRINTF_ERR(im_ercd);
		return FALSE;
	}
	
	#ifdef DBG_CNR_OFL_DUMP_REG
	IQDbg_Dump_IQ_CNR_OFL(); // For debug
	#endif
	
	(VOID)Im_CNR_OFL_Close( ch );
	
	if(ex_mode > D_CNR_OFL_EXMODE_X1){
		if( cnr_ex_info.format == E_SHARE_YUV_FORMAT_420_SEMI ) {
			reduce_img.type 	= E_BF_STILL_IMG_TYPE_YCC420;
			expand_img.type 	= E_BF_STILL_IMG_TYPE_YCC420;
		}
		else {
			reduce_img.type 	= E_BF_STILL_IMG_TYPE_YCC422;
			expand_img.type 	= E_BF_STILL_IMG_TYPE_YCC422;
		}

		reduce_img.plane		= E_BF_STILL_PLANE_TYPE_C_INTL;
		reduce_img.depth		= E_BF_STILL_IMG_DEPTH_U8;
		reduce_img.addr.y		= cnr_ex_info.addr_y;
		reduce_img.addr.cb		= cnr_ex_info.addr_c;
		reduce_img.addr.cr		= cnr_ex_info.addr_c;
		reduce_img.pixs.width	= cnr_ex_info.width;
		reduce_img.pixs.lines	= cnr_ex_info.lines;
		reduce_img.g_width.y	= cnr_ex_info.g_y_width;
		reduce_img.g_width.cb	= cnr_ex_info.g_c_width;
		reduce_img.g_width.cr	= cnr_ex_info.g_c_width;
		
		expand_img.plane		= E_BF_STILL_PLANE_TYPE_C_INTL;
		expand_img.depth		= E_BF_STILL_IMG_DEPTH_U8;
		expand_img.addr.y		= yuv->addr_y;
		expand_img.addr.cb		= yuv->addr_c;
		expand_img.addr.cr		= yuv->addr_c;
		expand_img.pixs.width	= yuv->h_size;
		expand_img.pixs.lines	= yuv->v_size;
		expand_img.g_width.y	= yuv->h_byte;
		expand_img.g_width.cb	= yuv->h_byte;
		expand_img.g_width.cr	= yuv->h_byte;
	
		// expand
		cnr_ofl_expand_c_only( &reduce_img, &expand_img );
	}
	
	return TRUE;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static VOID cnr_ofl_padding16lines( const T_CNR_OFL_EX_INFO* const src_img, const USHORT lines )
{
	T_IM_XCH_COPY_EX copy_param = {
		.xch = E_IM_XCH_CH_SEL_0,
		.copy_type = E_IM_XCH_COPY_SEL_NORMAL,
		.size = {
			.lines = 1,
		},
	};
	USHORT loopcnt;
	USHORT global_lines_c;
	USHORT lines_c;

	// Set lines total is multiple of 16.
	// Copy bottom line from lines to lines multiple of 16.

	// Y channel
	copy_param.size.width			= src_img->width;
	copy_param.size.src_gl_width	= src_img->g_y_width;
	copy_param.size.dst_gl_width	= src_img->g_y_width;
	copy_param.addr.src				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( src_img->addr_y + src_img->g_y_width * ( src_img->lines - 1 ) );
	for( loopcnt = 0; loopcnt < (USHORT)( ( lines - src_img->lines ) & 0xF ); loopcnt++ ) {
		copy_param.addr.dst = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( src_img->addr_y + src_img->g_y_width * ( src_img->lines + loopcnt ) );
		Im_Xch_Copy_Ex_Sync( &copy_param );
	}

	if( src_img->format == E_SHARE_YUV_FORMAT_420_SEMI ) {
		lines_c			= src_img->lines / 2;
		global_lines_c	= M_ROUNDUP_8( lines_c );
	}
	else {
		lines_c			= src_img->lines;
		global_lines_c	= M_ROUNDUP_16( lines_c );
	}

	// Cb channel
	copy_param.size.width			= src_img->width;
	copy_param.size.src_gl_width	= src_img->g_c_width;
	copy_param.size.dst_gl_width	= src_img->g_c_width;
	copy_param.addr.src				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( src_img->addr_c + src_img->g_c_width * ( lines_c - 1 ) );
	for( loopcnt = 0; loopcnt < (USHORT)( ( global_lines_c - lines_c ) & 0xF ); loopcnt++ ) {
		copy_param.addr.dst = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( src_img->addr_c + src_img->g_c_width * ( lines_c + loopcnt ) );
		Im_Xch_Copy_Ex_Sync( &copy_param );
	}
	
	#ifdef DBG_CNR_OFL_DUMP_YUV
	{
		ULONG size = 0;
		
		size = src_img->g_y_width * lines;
		debug_save_file((CHAR *)"I:\\DST_Y1.YUV", src_img->addr_y, size);
		UPRINTF("Size: %d(%d) x %d, Addr:0x%08lx\n", src_img->g_y_width, src_img->width, lines, src_img->addr_y);
		
		size = src_img->g_c_width * lines;
		debug_save_file((CHAR *)"I:\\DST_C1.YUV", src_img->addr_c, size);
		UPRINTF("Size: %d(%d) x %d, Addr:0x%08lx\n", src_img->g_c_width, src_img->width, lines, src_img->addr_c);
	}
	#endif

}

static VOID cnr_ofl_reduce( const T_SHARE_LAYOUT* const src_img, const T_CNR_OFL_EX_INFO* const dst_img )
{
	FJ_IMG_CONV_CTRL	ctl;
	
	ctl.src_img.addr.Y_G			= src_img->addr_y;
	ctl.src_img.addr.Cb_B			= src_img->addr_c;
	ctl.src_img.addr.Cr_R			= 0;
	ctl.src_img.addr.Alpha			= 0;
	ctl.src_img.g_y_g_width 		= src_img->h_byte;
	ctl.src_img.g_cb_b_width		= src_img->h_byte;
	ctl.src_img.g_cr_r_width		= 0;
	ctl.src_img.g_alpha_width		= 0;
	ctl.src_img.width				= src_img->h_size;
	ctl.src_img.lines				= src_img->v_size;
	ctl.src_img.line_bytes.Y_G		= src_img->h_byte;
	ctl.src_img.line_bytes.Cb_B 	= src_img->h_byte;
	ctl.src_img.line_bytes.Cr_R 	= 0;
	ctl.src_img.line_bytes.Alpha	= 0;
	ctl.src_img.pfmt				= src_img->format == E_SHARE_YUV_FORMAT_420_SEMI ? FJ_IMG_PFMT_YCC420 : FJ_IMG_PFMT_YCC422;
	
	ctl.dst_img.addr.Y_G			= dst_img->addr_y;
	ctl.dst_img.addr.Cb_B			= dst_img->addr_c;
	ctl.dst_img.addr.Cr_R			= 0;
	ctl.dst_img.addr.Alpha			= 0;
	ctl.dst_img.g_y_g_width 		= dst_img->g_y_width;
	ctl.dst_img.g_cb_b_width		= dst_img->g_c_width;
	ctl.dst_img.g_cr_r_width		= 0;
	ctl.dst_img.g_alpha_width		= 0;
	ctl.dst_img.width				= dst_img->width;
	ctl.dst_img.lines				= dst_img->lines;
	ctl.dst_img.line_bytes.Y_G		= dst_img->g_y_width;
	ctl.dst_img.line_bytes.Cb_B 	= dst_img->g_c_width;
	ctl.dst_img.line_bytes.Cr_R 	= 0;
	ctl.dst_img.line_bytes.Alpha	= 0;
	ctl.dst_img.pfmt				= dst_img->format == E_SHARE_YUV_FORMAT_420_SEMI ? FJ_IMG_PFMT_YCC420 : FJ_IMG_PFMT_YCC422;
	
	ctl.fill_enable 	= 0;
	ctl.resize_mode 	= FJ_IMG_RESIZE_MODE_BILINEAR;
	ctl.timeout 		= 1000;

	FJ_ImageConvert( &ctl );

	#ifdef DBG_CNR_OFL_DUMP_YUV
	{
		ULONG size = 0;

		size = src_img->h_byte * src_img->v_size;
		debug_save_file((CHAR *)"I:\\SRC_Y0.YUV", src_img->addr_y, size);
		UPRINTF("Size: %d(%d) x %d, Addr:0x%08lx\n", src_img->h_byte, src_img->h_size, src_img->v_size, src_img->addr_y);

		size = src_img->h_byte * src_img->v_size;
		debug_save_file((CHAR *)"I:\\SRC_C0.YUV", src_img->addr_c, size);
		UPRINTF("Size: %d(%d) x %d, Addr:0x%08lx\n", src_img->h_byte, src_img->h_size, src_img->v_size, src_img->addr_c);

		size = dst_img->g_y_width * dst_img->lines;
		debug_save_file((CHAR *)"I:\\DST_Y0.YUV", dst_img->addr_y, size);
		UPRINTF("Size: %d(%d) x %d, Addr:0x%08lx\n", dst_img->g_y_width, dst_img->width, dst_img->lines, dst_img->addr_y);

		size = dst_img->g_c_width * dst_img->lines;
		debug_save_file((CHAR *)"I:\\DST_C0.YUV", dst_img->addr_c, size);
		UPRINTF("Size: %d(%d) x %d, Addr:0x%08lx\n", dst_img->g_c_width, dst_img->width, dst_img->lines, dst_img->addr_c);
	}
	#endif
	
}

static VOID cnr_ofl_expand_c_only( const T_BF_STILL_IMG* const src_img, const T_BF_STILL_IMG* const dst_img )
{
	T_RESIZE_IMG	src;
	T_RESIZE_IMG	dst;

	// src
	src.img			= *src_img;
	src.rect.left	= 0.0;
	src.rect.top	= 0.0;
	src.rect.width	= src_img->pixs.width;
	src.rect.lines	= src_img->pixs.lines;
	// dst
	dst.img			= *dst_img;
	dst.rect.left	= 0.0;
	dst.rect.top	= 0.0;
	dst.rect.width	= dst_img->pixs.width;
	dst.rect.lines	= dst_img->pixs.lines;

	BF_Still_Resize_Image_CbCrOnly( &src, &dst, E_RESIZE_MODE_BI_LINEAR, 100, 0, 0, NULL );

	#ifdef DBG_CNR_OFL_DUMP_YUV
	{
		ULONG size = 0;

		size = dst_img->g_width.y * dst_img->pixs.lines;
		debug_save_file((CHAR *)"I:\\DST_Y2.YUV", dst_img->addr.y, size);
		UPRINTF("Size: %d(%d) x %d, Addr:0x%08lx\n", dst_img->g_width.y, dst_img->pixs.width, dst_img->pixs.lines, dst_img->addr.y);

		size = dst_img->g_width.cb * dst_img->pixs.lines;
		debug_save_file((CHAR *)"I:\\DST_C2.YUV", dst_img->addr.cb, size);
		UPRINTF("Size: %d(%d) x %d, Addr:0x%08lx\n", dst_img->g_width.cb, dst_img->pixs.width, dst_img->pixs.lines, dst_img->addr.cb);
	}
	#endif
	
}

static VOID cnr_ofl_callback( UCHAR ch, ULONG interrupt_flag_mask, UINT32 user_param )
{
	if( interrupt_flag_mask == D_IM_CNR_FLG_0_OFL_SPR_AXI_ERR ) {
		UPRINTF( "cnr_ofl_callback(): CNR0 OFL SPR AXI error.\n" );
	}

	if( interrupt_flag_mask == D_IM_CNR_FLG_1_OFL_SPR_AXI_ERR ) {
		UPRINTF( "cnr_ofl_callback(): CNR1 OFL SPR AXI error.\n" );
	}
}


