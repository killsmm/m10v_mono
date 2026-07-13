/**
 * @file		ct_im_jpeg.c
 * @brief		JPEG ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "dd_top.h"
#include "ct_im_jpeg.h"
#include "ddim_user_custom.h"
// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#include "palladium_test.h"
#include "palladium_test_common.h"
#endif
// ### REMOVE_RELEASE END
#include "jdsjp7.h"
#include <string.h>
#include <stdlib.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define CO_CT_IM_JPEG_REG_DUMP	// Debug dump option.

#ifdef CO_ACT_CLOCK
#define CO_ACT_JPEG_CLOCK
#endif //CO_ACT_CLOCK
#ifdef CO_ACT_HCLOCK
#define CO_ACT_JPEG_HCLOCK
#endif //CO_ACT_HCLOCK
#ifdef CO_ACT_ICLOCK
#define CO_ACT_JPEG_ICLOCK
#endif //CO_ACT_ICLOCK

#if 1	// Clock control force ON.
#ifndef CO_ACT_CLOCK
#define CO_ACT_JPEG_CLOCK
#endif //CO_ACT_CLOCK
#ifndef CO_ACT_HCLOCK
#define CO_ACT_JPEG_HCLOCK
#endif //CO_ACT_HCLOCK
#ifndef CO_ACT_ICLOCK
#define CO_ACT_JPEG_ICLOCK
#endif //CO_ACT_ICLOCK
#endif

#define D_CT_JPEG_HCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP3())
#define D_CT_JPEG_HCLK_BIT			D_DD_TOP_JPGAH_BIT


// #define CO_DEBUG_ON_PC
// #define CO_DEBUG_MEASURE_MS
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_CT_JPEG_FORMAT_444P			= 0,	// 444 plane
	E_CT_JPEG_FORMAT_422P,			 		// 422 plane
	E_CT_JPEG_FORMAT_422PD,			 		// 422 plane & dot
	E_CT_JPEG_FORMAT_420P,			 		// 400 plane
	E_CT_JPEG_FORMAT_420PD,			 		// 400 plane & dot
	E_CT_JPEG_FORMAT_400P,			 		// 400 plane
	E_CT_JPEG_FORMAT_RGBP					// RGB
} E_CT_JPEG_FORMAT;

typedef enum {
	E_PRINT_CTRL_ENC_BASE			= 0,
	E_PRINT_CTRL_ENC_FRAME,			 	
	E_PRINT_CTRL_DEC_BASE,			 	
	E_PRINT_CTRL_DEC_FRAME			 	
} E_PRINT_CTRL;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UCHAR					sync;
	E_CT_JPEG_FORMAT		format;
	ULONG					g_y_width;
	ULONG					g_y_lines;
	ULONG					g_c_width;
	USHORT					h_size;
	USHORT					v_size;
	ULLONG					limit_size;
	UCHAR					pause_flg;
	UCHAR					count_flg;
	UCHAR					quantup_flg;
	UCHAR					skip_mk_flg;
	UCHAR					dri;
	UCHAR					dsp_rate;
} T_CT_JPEG_ENC_PARAM;

typedef struct {
	UCHAR					sync;
	E_CT_JPEG_FORMAT		format;
	ULONG					g_y_width;
	ULONG					g_y_lines;
	ULONG					g_c_width;
	USHORT					h_size;
	USHORT					v_size;
	UCHAR					cutout_flg;
	USHORT					cut_h_size;
	USHORT					cut_v_size;
	ULLONG					limit_size;
	E_IM_JPEG_RESIZE_EXT	ext;
	UCHAR					corr_flg;
	UCHAR					pause_flg;
} T_CT_JPEG_DEC_PARAM;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#ifdef CO_ACT_JPEG_CLOCK
static UCHAR gCt_IM_Jpg_Clk_Ctrl_Cnt = 0;
#endif // CO_ACT_JPEG_CLOCK
#ifdef CO_ACT_JPEG_HCLOCK
static UCHAR gCt_IM_Jpg_Hclk_Ctrl_Cnt = 0;
#endif // CO_ACT_JPEG_HCLOCK
#ifdef CO_ACT_JPEG_ICLOCK
static UCHAR gCt_IM_Jpg_Iclk_Ctrl_Cnt = 0;
#endif // CO_ACT_JPEG_ICLOCK 

//static DDIM_USER_SYSTIM		gJpeg_Measure_Time;

static UCHAR				gJpeg_Enc_End_Flg=0;
static UCHAR				gJpeg_Dec_End_Flg=0;
static T_IM_JPEG_ENC_MNG 	gJpeg_Enc_Mng;
static T_IM_JPEG_DEC_MNG 	gJpeg_Dec_Mng;
static ULONG				gJpeg_Encode_Size = 0;
static UCHAR				gJpeg_AXI_Err = 0;
static ULONG				gJpeg_OutputBytes = 0ul;


static T_IM_JPEG_QUAT_TBL gJpeg_Qtbl = {
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
		0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
		0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
		0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0xFF
	}
};

static T_IM_JPEG_QUAT_TBL gJpeg_Qtbl2 = {
	{
		0x40, 0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39,
		0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31,
		0x30, 0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x2A, 0x29,
		0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21,
		0x20, 0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19,
		0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11,
		0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09,
		0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01
	}
};

// for Y
static T_IM_JPEG_QUAT_TBL gJpeg_Qtbl_Y = {
	{	0x10, 0x0B, 0x0A, 0x10, 0x18, 0x28, 0x33, 0x3D,
		0x0C, 0x0C, 0x0E, 0x13, 0x1A, 0x3A, 0x3C, 0x37,
		0x0E, 0x0D, 0x10, 0x18, 0x28, 0x39, 0x45, 0x38,
		0x0E, 0x11, 0x16, 0x1D, 0x33, 0x57, 0x50, 0x3E,
		0x12, 0x16, 0x25, 0x38, 0x44, 0x6D, 0x67, 0x4D,
		0x18, 0x23, 0x37, 0x40, 0x51, 0x68, 0x71, 0x5C,
		0x31, 0x40, 0x4E, 0x57, 0x67, 0x79, 0x78, 0x65,
		0x48, 0x5C, 0x5F, 0x62, 0x70, 0x64, 0x67, 0x63	}
};

// for Cb/Cr
static T_IM_JPEG_QUAT_TBL gJpeg_Qtbl_C = {
	{	0x11, 0x12, 0x18, 0x2F, 0x63, 0x63, 0x63, 0x63,
		0x12, 0x15, 0x1A, 0x42, 0x63, 0x63, 0x63, 0x63,
		0x18, 0x1A, 0x38, 0x63, 0x63, 0x63, 0x63, 0x63,
		0x2F, 0x42, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
		0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
		0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
		0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
		0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63	}
};


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
#if 0	// measure_time disable.
#ifndef CO_DEBUG_MEASURE_MS
static ULONG ct_jpeg_get_time_us( VOID )
{
	UINT64 peri_clk;
	UINT64 gtimer_counter;
	
	peri_clk = Dd_Top_Get_PERICLK();
	Dd_GTMR_Get_Counter( &gtimer_counter );
	
	return (ULONG)(((gtimer_counter * 1000) / (peri_clk / 100000)) / 100);
}
#else
static ULONG ct_jpeg_get_time_ms( VOID )
{
	return ct_jpeg_get_time_us() / 1000;
}
#endif

INT32 ct_jpeg_measure_time_start( VOID )
{
	DDIM_USER_SYSTIM sys_time;

#ifndef CO_DEBUG_MEASURE_MS
	sys_time = ct_jpeg_get_time_us();
#else
	sys_time = ct_jpeg_get_time_ms();
#endif
	gJpeg_Measure_Time = sys_time;
	return 0;
}

INT32 ct_jpeg_measure_time_print( VOID )
{
	DDIM_USER_SYSTIM sys_time;
	DDIM_USER_SYSTIM m_time;

#ifndef CO_DEBUG_MEASURE_MS
	sys_time = ct_jpeg_get_time_us();
	m_time = sys_time - gJpeg_Measure_Time;
	Ddim_Print(("***** Measure Time = %d(us) *****\n", (ULONG)m_time));
#else
	sys_time = ct_jpeg_get_time_ms();
	m_time = sys_time - gJpeg_Measure_Time;
	Ddim_Print(("***** Measure Time = %d(ms) *****\n", (ULONG)m_time));
#endif

	return 0;
}
#endif	// measure_time disable.

static VOID ct_jpeg_start_hclock( VOID )
{
#ifdef CO_ACT_JPEG_HCLOCK
	DDIM_User_AhbReg_SpinLock();

	Dd_Top_Start_Clock( (UCHAR*)&gCt_IM_Jpg_Hclk_Ctrl_Cnt, D_CT_JPEG_HCLK_REG_ADDR, ~D_CT_JPEG_HCLK_BIT );
#endif // CO_ACT_JPEG_HCLOCK
}

static VOID ct_jpeg_stop_hclock( VOID )
{
#ifdef CO_ACT_JPEG_HCLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gCt_IM_Jpg_Hclk_Ctrl_Cnt, D_CT_JPEG_HCLK_REG_ADDR, D_CT_JPEG_HCLK_BIT );
	DDIM_User_AhbReg_SpinUnLock();
#endif // CO_ACT_JPEG_HCLOCK
}

static VOID ct_jpeg_ctrl_print( E_PRINT_CTRL ptn, VOID* ctrl )
{
#if 0
	T_IM_JPEG_DEC_MNG dec_mng;
	T_IM_JPEG_DEC_FRAME_MNG dec_frm_mng;
	T_IM_JPEG_ENC_MNG enc_mng;
	T_IM_JPEG_ENC_FRAME_MNG enc_frm_mng;

	switch(ptn) {
		case E_PRINT_CTRL_ENC_BASE:
			memcpy( &enc_mng, ctrl, sizeof(T_IM_JPEG_ENC_MNG) );
			Ddim_Print(("Print Enc Base Setting start ------------- \n"));
			Ddim_Print(("enc_mng.ycc_smpl                   =[0x%X]\n", enc_mng.ycc_smpl));
			Ddim_Print(("enc_mng.mem_format                 =[0x%X]\n", enc_mng.mem_format));
			Ddim_Print(("enc_mng.width                      =[%d]\n", enc_mng.width));
			Ddim_Print(("enc_mng.lines                      =[%d]\n", enc_mng.lines));
			Ddim_Print(("enc_mng.component[0].quant_tbl_no  =[0x%X]\n", enc_mng.component[0].quant_tbl_no ));
			Ddim_Print(("enc_mng.component[0].huf_dc_tbl_no =[0x%X]\n", enc_mng.component[0].huf_dc_tbl_no));
			Ddim_Print(("enc_mng.component[0].huf_ac_tbl_no =[0x%X]\n", enc_mng.component[0].huf_ac_tbl_no));
			Ddim_Print(("enc_mng.component[1].quant_tbl_no  =[0x%X]\n", enc_mng.component[1].quant_tbl_no ));
			Ddim_Print(("enc_mng.component[1].huf_dc_tbl_no =[0x%X]\n", enc_mng.component[1].huf_dc_tbl_no));
			Ddim_Print(("enc_mng.component[1].huf_ac_tbl_no =[0x%X]\n", enc_mng.component[1].huf_ac_tbl_no));
			Ddim_Print(("enc_mng.component[2].quant_tbl_no  =[0x%X]\n", enc_mng.component[2].quant_tbl_no ));
			Ddim_Print(("enc_mng.component[2].huf_dc_tbl_no =[0x%X]\n", enc_mng.component[2].huf_dc_tbl_no));
			Ddim_Print(("enc_mng.component[2].huf_ac_tbl_no =[0x%X]\n", enc_mng.component[2].huf_ac_tbl_no));
			Ddim_Print(("enc_mng.skip_mk_flg                =[0x%X]\n", enc_mng.skip_mk_flg));
			Ddim_Print(("enc_mng.dri_mk_num                 =[0x%X]\n", enc_mng.dri_mk_num));
			Ddim_Print(("enc_mng.exif_fmt_flg               =[0x%X]\n", enc_mng.exif_fmt_flg));
			Ddim_Print(("enc_mng.pint_line                  =[0x%X]\n", enc_mng.pint_line));
			Ddim_Print(("enc_mng.pint_sect                  =[0x%lX]\n", enc_mng.pint_sect));
			Ddim_Print(("enc_mng.pbuf_ctrl.endian           =[0x%X]\n", enc_mng.pbuf_ctrl.endian));
			Ddim_Print(("enc_mng.pbuf_ctrl.issue_tran_num   =[0x%X]\n", enc_mng.pbuf_ctrl.issue_tran_num));
			Ddim_Print(("enc_mng.pbuf_ctrl.cache_type       =[0x%X]\n", enc_mng.pbuf_ctrl.cache_type));
			Ddim_Print(("enc_mng.pbuf_ctrl.prot_type        =[0x%X]\n", enc_mng.pbuf_ctrl.prot_type));
			Ddim_Print(("enc_mng.jbuf_ctrl.endian           =[0x%X]\n", enc_mng.jbuf_ctrl.endian));
			Ddim_Print(("enc_mng.jbuf_ctrl.issue_tran_num   =[0x%X]\n", enc_mng.jbuf_ctrl.issue_tran_num));
			Ddim_Print(("enc_mng.jbuf_ctrl.cache_type       =[0x%X]\n", enc_mng.jbuf_ctrl.cache_type));
			Ddim_Print(("enc_mng.jbuf_ctrl.prot_type        =[0x%X]\n", enc_mng.jbuf_ctrl.prot_type));
			Ddim_Print(("enc_mng.jburst_length              =[0x%X]\n", enc_mng.jburst_length));
			Ddim_Print(("enc_mng.pburst_length              =[0x%X]\n", enc_mng.pburst_length));
			Ddim_Print(("Print Enc Base Setting end ------------- \n"));
			break;
		case E_PRINT_CTRL_ENC_FRAME:
			memcpy( &enc_frm_mng, ctrl, sizeof(T_IM_JPEG_ENC_FRAME_MNG) );
			Ddim_Print(("Print Enc Frame Setting start ------------- \n"));
			Ddim_Print(("enc_frm_mng.global_y_width =[%ld]\n", enc_frm_mng.global_y_width));
			Ddim_Print(("enc_frm_mng.global_c_width =[%ld]\n", enc_frm_mng.global_c_width));
			Ddim_Print(("enc_frm_mng.ycc_addr.y     =[0x%08lX]\n", enc_frm_mng.ycc_addr.y));
			Ddim_Print(("enc_frm_mng.ycc_addr.c     =[0x%08lX]\n", enc_frm_mng.ycc_addr.c));
			Ddim_Print(("enc_frm_mng.code_addr      =[0x%08lX]\n", enc_frm_mng.code_addr));
			Ddim_Print(("enc_frm_mng.code_count_flg =[0x%X]\n", enc_frm_mng.code_count_flg));
#ifdef CO_DEBUG_ON_PC
			Ddim_Print(("enc_frm_mng.limit_size     =[%I64X]\n", enc_frm_mng.limit_size));
#else
			Ddim_Print(("enc_frm_mng.limit_size     =[0x%llX]\n", enc_frm_mng.limit_size));
#endif	// CO_DEBUG_ON_PC
			Ddim_Print(("Print Enc Frame Setting end------------- \n"));
			break;
		case E_PRINT_CTRL_DEC_BASE:
			memcpy( &dec_mng, ctrl, sizeof(T_IM_JPEG_DEC_MNG) );
			Ddim_Print(("Print Dec Base Setting start ------------- \n"));
			Ddim_Print(("dec_mng.smpl_type                =[0x%X]\n", dec_mng.smpl_type));
			Ddim_Print(("dec_mng.mem_format               =[0x%X]\n", dec_mng.mem_format));
			Ddim_Print(("dec_mng.ext_mode                 =[0x%X]\n", dec_mng.ext_mode));
			Ddim_Print(("dec_mng.skip_mk_flg              =[0x%X]\n", dec_mng.skip_mk_flg));
			Ddim_Print(("dec_mng.pint_line                =[0x%X]\n", dec_mng.pint_line));
			Ddim_Print(("dec_mng.pint_sect                =[0x%lX]\n", dec_mng.pint_sect));
			Ddim_Print(("dec_mng.corr_mode                =[0x%X]\n", dec_mng.corr_mode));
			Ddim_Print(("dec_mng.color_band.y_band        =[0x%X]\n", dec_mng.color_band.y_band));
			Ddim_Print(("dec_mng.color_band.cb_band       =[0x%X]\n", dec_mng.color_band.cb_band));
			Ddim_Print(("dec_mng.color_band.cr_band       =[0x%X]\n", dec_mng.color_band.cr_band));
			Ddim_Print(("dec_mng.pbuf_ctrl.endian         =[0x%X]\n", dec_mng.pbuf_ctrl.endian));
			Ddim_Print(("dec_mng.pbuf_ctrl.issue_tran_num =[0x%X]\n", dec_mng.pbuf_ctrl.issue_tran_num));
			Ddim_Print(("dec_mng.pbuf_ctrl.cache_type     =[0x%X]\n", dec_mng.pbuf_ctrl.cache_type));
			Ddim_Print(("dec_mng.pbuf_ctrl.prot_type      =[0x%X]\n", dec_mng.pbuf_ctrl.prot_type));
			Ddim_Print(("dec_mng.jbuf_ctrl.endian         =[0x%X]\n", dec_mng.jbuf_ctrl.endian));
			Ddim_Print(("dec_mng.jbuf_ctrl.issue_tran_num =[0x%X]\n", dec_mng.jbuf_ctrl.issue_tran_num));
			Ddim_Print(("dec_mng.jbuf_ctrl.cache_type     =[0x%X]\n", dec_mng.jbuf_ctrl.cache_type));
			Ddim_Print(("dec_mng.jbuf_ctrl.prot_type      =[0x%X]\n", dec_mng.jbuf_ctrl.prot_type));
			Ddim_Print(("dec_mng.jburst_length            =[0x%X]\n", dec_mng.jburst_length));
			Ddim_Print(("dec_mng.pburst_length            =[0x%X]\n", dec_mng.pburst_length));
			Ddim_Print(("Print Dec Base Setting end ------------- \n"));
			break;
		case E_PRINT_CTRL_DEC_FRAME:
			memcpy( &dec_frm_mng, ctrl, sizeof(T_IM_JPEG_DEC_FRAME_MNG) );
			Ddim_Print(("Print Dec Frame Setting start ------------- \n"));
			Ddim_Print(("dec_frm_mng.global_y_width  =[%ld]\n", dec_frm_mng.global_y_width));
			Ddim_Print(("dec_frm_mng.global_c_width  =[%ld]\n", dec_frm_mng.global_c_width));
			Ddim_Print(("dec_frm_mng.cutout_flg	     =[0x%X]\n", dec_frm_mng.cutout_flg	));
			Ddim_Print(("dec_frm_mng.cutout_width    =[%d]\n", dec_frm_mng.cutout_width));
			Ddim_Print(("dec_frm_mng.cutout_lines    =[%d]\n", dec_frm_mng.cutout_lines));
			Ddim_Print(("dec_frm_mng.cutout_offset_h =[%d]\n", dec_frm_mng.cutout_offset_h));
			Ddim_Print(("dec_frm_mng.cutout_offset_v =[%d]\n", dec_frm_mng.cutout_offset_v));
			Ddim_Print(("dec_frm_mng.ycc_addr.y      =[0x%08lX]\n", dec_frm_mng.ycc_addr.y));
			Ddim_Print(("dec_frm_mng.ycc_addr.c      =[0x%08lX]\n", dec_frm_mng.ycc_addr.c));
			Ddim_Print(("dec_frm_mng.code_addr       =[0x%08lX]\n", dec_frm_mng.code_addr));
#ifdef CO_DEBUG_ON_PC
			Ddim_Print(("dec_frm_mng.code_size   =[%I64X]\n", dec_frm_mng.code_size));
#else
			Ddim_Print(("dec_frm_mng.code_size   =[0x%llX]\n", dec_frm_mng.code_size));
#endif	// CO_DEBUG_ON_PC
			Ddim_Print(("Print Dec Frame Setting end ------------- \n"));
			break;
		default:
			break;
	}
#endif
}

static VOID ct_jpeg_encode_cb( T_IM_JPEG_ENC_MNG* pjpg_enc_mng )
{
	Ddim_Print(("***** Encode Callback function *****\n"));
	memcpy(&gJpeg_Enc_Mng, pjpg_enc_mng, sizeof(T_IM_JPEG_ENC_MNG));
	gJpeg_Enc_End_Flg = 1;
//	ct_jpeg_measure_time_print();
	Ddim_Print(("***** Process Result = 0x%lX *****\n", (ULONG)(pjpg_enc_mng->result)));
	Ddim_Print(("***** Code Size      = 0x%lX *****\n", (ULONG)(pjpg_enc_mng->code_size)));
	return;
}

static VOID ct_jpeg_decode_cb( T_IM_JPEG_DEC_MNG* pjpg_dec_mng )
{
	Ddim_Print(("***** Decode Callback function *****\n"));
	memcpy(&gJpeg_Dec_Mng, pjpg_dec_mng, sizeof(T_IM_JPEG_DEC_MNG));
	gJpeg_Dec_End_Flg = 1;
//	ct_jpeg_measure_time_print();
	return;
}

#ifdef CO_DEBUG_ON_PC
static VOID ct_jpeg_encode_int_handler( UCHAR ptn )
{
	IO_JPG7.JSTATE.bit.JALLRSTP = 1;
	switch(ptn) {
		case 1:
		// Normal
			IO_JPG7.JINT.word		= 0x0C00C000;
			IO_JPG7.JPINTSTA.word	= 0x80000000;
			IO_JPG7.JPBYTCNT.word	= 0x005DC900;
			break;

		case 2:
		// Pause
			IO_JPG7.JINT.word		= 0x80001000;
			IO_JPG7.JPINTSTA.word	= 0x00000000;
			IO_JPG7.JPBYTCNT.word	= 0xFFFFFFFF;
			break;

		case 3:
		// Quant update
			IO_JPG7.JINT.word		= 0x0C000000;
			IO_JPG7.JPINTSTA.word	= 0x00000010;
			IO_JPG7.JPBYTCNT.word	= 0xFFFFFFFF;
			break;
		case 4:
		// JPBINT
			IO_JPG7.JINT.word		= 0x20400000;
			IO_JPG7.JPINTSTA.word	= 0x00000000;
			break;

		case 5:
		// JJBINT
			IO_JPG7.JINT.word		= 0x20200000;
			IO_JPG7.JPINTSTA.word	= 0x00000000;
			break;

		default:
			
			break;

	}
	IO_JPG7.JPSTATUS.bit.JPSTATUS = 1;
	Im_JPEG_Int_Handler();
	return;
}

static VOID ct_jpeg_decode_int_handler( UCHAR ptn )
{
	IO_JPG7.JSTATE.bit.JALLRSTP = 1;

	switch(ptn) {
		case 1:
		// Normal
			IO_JPG7.JINT.word		= 0x0A000000;
			IO_JPG7.JPINTSTA.word	= 0x00000008;
			IO_JPG7.JPSUSPEND.bit.JPSUSPEND = 1;

			Im_JPEG_Int_Handler();
			// JIF
			IO_JPG7.JINT.word		= 0x8000;
			Im_JPEG_Int_Handler();
			// JCF
			IO_JPG7.JINT.word		= 0x4000;
			Im_JPEG_Int_Handler();

			IO_JPG7.JINT.word		= 0x0C000000;
			IO_JPG7.JPINTSTA.word	= 0x80000000;
			break;

		case 2:
		// Pause 1
			IO_JPG7.JINT.word		= 0x0A000000;
			IO_JPG7.JPINTSTA.word	= 0x00000008;
			IO_JPG7.JPSUSPEND.bit.JPSUSPEND = 1;

			Im_JPEG_Int_Handler();

			IO_JPG7.JINT.word		= 0x00001000;
			IO_JPG7.JPINTSTA.word	= 0x00000000;
			break;

		case 3:
		// Pause 2
			IO_JPG7.JINT.word		= 0x8000;
			Im_JPEG_Int_Handler();

			IO_JPG7.JINT.word		= 0x4000;
			Im_JPEG_Int_Handler();

			IO_JPG7.JINT.word		= 0x0C000000;
			IO_JPG7.JPINTSTA.word	= 0x80000000;
			break;

		default:
			
			break;

	}
	IO_JPG7.JPSTATUS.bit.JPSTATUS = 1;
	Im_JPEG_Int_Handler();
	return;
}
#endif	// CO_DEBUG_ON_PC

static VOID ct_jpeg_stop( VOID )
{
#ifdef CO_DEBUG_ON_PC
	IO_JPG7.JSTATE.bit.JALLRSTP = 1;
#endif	// CO_DEBUG_ON_PC
	Im_JPEG_Stop();
	return;
}

static VOID ct_jpeg_set_ycc_out_bytes( const E_IM_JPEG_SMPL_TYPE smpl_type, const E_IM_JPEG_MEM_FORM mem_format, const ULONG global_y_width, const ULONG global_c_width, const ULONG lines )
{
	unsigned long out_data_siz_y;
	unsigned long out_data_siz_c;

	// Calc Y bytes.
	out_data_siz_y = global_y_width * lines;

	switch( smpl_type ) {
		case E_IM_JPEG_SMPL_TYPE_YCC444:	// FALLTHROUGH
		case E_IM_JPEG_SMPL_TYPE_YCC422:
			out_data_siz_c = global_c_width * lines;
			break;
//		case E_IM_JPEG_SMPL_TYPE_YCC420:
		default:
			out_data_siz_c = global_c_width * (lines /2);
			break;
		case E_IM_JPEG_SMPL_TYPE_YCC400:
			out_data_siz_c = 0ul;
	}
	if( mem_format == E_IM_JPEG_MEM_FORM_PLANE ) {
		out_data_siz_c *= 2;
	}

	gJpeg_OutputBytes = out_data_siz_y + out_data_siz_c;
}

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
static VOID ct_jpeg_encode_palladium_in( const T_IM_JPEG_ENC_MNG* const enc_mng, const T_IM_JPEG_ENC_FRAME_MNG* const enc_frm_mng, const T_CT_JPEG_ENC_PARAM* const enc_param )
{
	unsigned long in_data_siz_y;
	unsigned long in_data_siz_c;

	// Calc Y bytes.
	in_data_siz_y = ((unsigned long)enc_frm_mng->global_y_width) * enc_param->g_y_lines;

	switch( enc_mng->ycc_smpl ) {
		case E_IM_JPEG_SMPL_TYPE_YCC444:	// FALLTHROUGH
		case E_IM_JPEG_SMPL_TYPE_YCC422:
			in_data_siz_c = ((unsigned long)enc_frm_mng->global_c_width) * enc_param->g_y_lines;
			break;
//		case E_IM_JPEG_SMPL_TYPE_YCC420:
		default:
			in_data_siz_c = ((unsigned long)enc_frm_mng->global_c_width) * (enc_param->g_y_lines /2);
			break;
		case E_IM_JPEG_SMPL_TYPE_YCC400:
			in_data_siz_c = 0ul;
	}
	if( enc_mng->mem_format == E_IM_JPEG_MEM_FORM_PLANE ) {
		in_data_siz_c *= 2;
	}


	Ddim_Print(("%s(): Palladium_Set_In_Localstack( 0x%lx, %lu )\n", __func__, ((unsigned long)enc_frm_mng->ycc_addr.y), in_data_siz_y + in_data_siz_c));
	Palladium_Set_In_Localstack( ((unsigned long)enc_frm_mng->ycc_addr.y), in_data_siz_y + in_data_siz_c );
}
#endif

// ### REMOVE_RELEASE END
static VOID ct_jpeg_encode( T_CT_JPEG_ENC_PARAM* enc_param, UCHAR set_qualv )
{
	T_IM_JPEG_ENC_MNG enc_mng;
	T_IM_JPEG_ENC_FRAME_MNG enc_frm_mng;
	INT32 ret = 0;
//	E_IM_JPEG_AXI_ST		buf_status;
	INT32	i;
	INT32	roop_cnt;
	ULLONG	pause_size;
	ULONG	sector_cnt;
	USHORT	err_ratio;
	T_IM_JPEG_ENC_MNG out_enc_mng;
	T_IM_JPEG_ENC_FRAME_MNG out_enc_frm_mng;
	T_IM_JPEG_QUAT_TBL_PACK quant_tbl_pack;
// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
	ULONG	code_addr;
#endif
// ### REMOVE_RELEASE END

	memset( &enc_mng, 0, sizeof( T_IM_JPEG_ENC_MNG ) );
	memset( &enc_frm_mng, 0, sizeof( T_IM_JPEG_ENC_FRAME_MNG ) );
	memset( &out_enc_mng, 0, sizeof( T_IM_JPEG_ENC_MNG ) );
	memset( &out_enc_frm_mng, 0, sizeof( T_IM_JPEG_ENC_FRAME_MNG ) );


	// Base
	switch( enc_param->format ) {
		case E_CT_JPEG_FORMAT_444P:
		case E_CT_JPEG_FORMAT_RGBP:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC444;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_422P:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC422;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_422PD:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC422;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE_DOT;
			break;
		case E_CT_JPEG_FORMAT_420P:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC420;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_420PD:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC420;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE_DOT;
			break;
		case E_CT_JPEG_FORMAT_400P:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC400;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		default:
			Ddim_Print(("input parameter error\n"));
			return;
	}

	enc_mng.width = enc_param->h_size;
	enc_mng.lines = enc_param->v_size;

	enc_mng.component[0].quant_tbl_no  = 0;	// Y
	enc_mng.component[0].huf_dc_tbl_no = 0;	// Y
	enc_mng.component[0].huf_ac_tbl_no = 0;	// Y
	enc_mng.component[1].quant_tbl_no  = 1;	// CB
	enc_mng.component[1].huf_dc_tbl_no = 1;	// CB
	enc_mng.component[1].huf_ac_tbl_no = 1;	// CB
	enc_mng.component[2].quant_tbl_no  = 1;	// CR
	enc_mng.component[2].huf_dc_tbl_no = 1;	// CR
	enc_mng.component[2].huf_ac_tbl_no = 1;	// CR

	enc_mng.skip_mk_flg = enc_param->skip_mk_flg;

	if ( enc_param->dri ) {
		enc_mng.dri_mk_num = 0x10;
	}
	else {
		enc_mng.dri_mk_num = 0;
	}

	enc_mng.exif_fmt_flg				= D_IM_JPEG_ENABLE_ON;
	enc_mng.pint_line					= 0;
	enc_mng.pint_sect					= 0;
	enc_mng.pbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	enc_mng.pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	enc_mng.pbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF;
	enc_mng.pbuf_ctrl.prot_type			= D_IM_JPEG_D_SEC_NORMAL_ACCESS;
	enc_mng.jbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	enc_mng.jbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	enc_mng.jbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF;
	enc_mng.jbuf_ctrl.prot_type			= D_IM_JPEG_D_SEC_NORMAL_ACCESS;
	enc_mng.jburst_length				= E_IM_JPEG_BURST_INCR_8;
	enc_mng.pburst_length				= E_IM_JPEG_BURST_INCR_8;

	if ( enc_param->sync ) {
		enc_mng.pcallback = NULL;
	}
	else {
		enc_mng.pcallback = (VP_CALLBACK)ct_jpeg_encode_cb;
	}

	// Frame
	enc_frm_mng.global_y_width = enc_param->g_y_width;

	// Y addr
	enc_frm_mng.ycc_addr.y = D_CT_JPEG_YCC_ADDR;

	enc_frm_mng.global_c_width = enc_param->g_c_width;
	// Cb addr
	enc_frm_mng.ycc_addr.c = D_CT_JPEG_YCC_ADDR + (enc_param->g_y_width * enc_param->g_y_lines);

	// Code addr
	if( gJpeg_AXI_Err ) {
		enc_frm_mng.code_addr		= D_CT_JPEG_CODE_ADDR_ERR;
	}
	else {
		enc_frm_mng.code_addr		= D_CT_JPEG_CODE_ADDR;
	}
	enc_frm_mng.code_count_flg	= enc_param->count_flg;

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
	ct_jpeg_encode_palladium_in( &enc_mng, &enc_frm_mng, enc_param );
	code_addr = enc_frm_mng.code_addr;
#endif

// ### REMOVE_RELEASE END
//	ct_jpeg_measure_time_start();

	ret = Im_JPEG_Open( 100 );
	Ddim_Print(("Im_JPEG_Open ret=0x%X\n", ret));
	if ( ret != D_IM_JPEG_OK ) {
		return;
	}

	if ( set_qualv ) {
		ret = Im_JPEG_Set_Quality( 51200, 0 );
		Ddim_Print(("Im_JPEG_Set_Quality ret=0x%X\n", ret));
		if ( ret != D_IM_JPEG_OK ) {
			Im_JPEG_Close();
			return;
		}
	}
	else {
#if 1
		quant_tbl_pack.quantization_tbl_0 = &gJpeg_Qtbl;
		quant_tbl_pack.quantization_tbl_1 = &gJpeg_Qtbl;
		quant_tbl_pack.quantization_tbl_2 = &gJpeg_Qtbl;
		quant_tbl_pack.quantization_tbl_3 = &gJpeg_Qtbl;
#else
		quant_tbl_pack.quantization_tbl_0 = NULL;
		quant_tbl_pack.quantization_tbl_1 = NULL;
		quant_tbl_pack.quantization_tbl_2 = NULL;
		quant_tbl_pack.quantization_tbl_3 = NULL;
#endif
		Im_JPEG_Set_QTbl( &quant_tbl_pack, 0 );
		Ddim_Print(("Im_JPEG_Set_QTbl ret=0x%X\n", ret));
		if ( ret != D_IM_JPEG_OK ) {
			Im_JPEG_Close();
			return;
		}
	}

	ret = Im_JPEG_Ctrl_Enc( &enc_mng );
	Ddim_Print(("Im_JPEG_Ctrl_Enc ret=0x%X\n", ret));
	// Ctrl Print
	ct_jpeg_ctrl_print( E_PRINT_CTRL_ENC_BASE, (VOID*)&enc_mng );
	if ( ret != D_IM_JPEG_OK ) {
		Im_JPEG_Close();
		return;
	}

	if ( !enc_param->pause_flg ) {

		// limit size
		enc_frm_mng.limit_size		= enc_param->limit_size;

		if ( enc_param->count_flg  ) {
			err_ratio = Im_JPEG_Set_Down_Sampling_Rate( enc_param->dsp_rate );
			Ddim_Print(("Im_JPEG_Set_Down_Sampling_Rate err_ratio=0x%X\n", err_ratio));
			roop_cnt = 0;
		}
		else if ( enc_param->quantup_flg ) {
	
			if ( set_qualv ) {
				// For next frame
				ret = Im_JPEG_Set_Quality( 0x40, 1 );
				Ddim_Print(("Im_JPEG_Set_Quality ret=0x%X\n", ret));
				if ( ret != D_IM_JPEG_OK ) {
					Im_JPEG_Close();
					return;
				}
			}
			else {
				quant_tbl_pack.quantization_tbl_0 = &gJpeg_Qtbl2;
				quant_tbl_pack.quantization_tbl_1 = &gJpeg_Qtbl2;
				quant_tbl_pack.quantization_tbl_2 = &gJpeg_Qtbl2;
				quant_tbl_pack.quantization_tbl_3 = &gJpeg_Qtbl2;

				Im_JPEG_Set_QTbl( &quant_tbl_pack, 1 );
				Ddim_Print(("Im_JPEG_Set_QTbl ret=0x%X\n", ret));
				if ( ret != D_IM_JPEG_OK ) {
					Im_JPEG_Close();
					return;
				}
			}
			roop_cnt = 1;
		}
		else {
			roop_cnt = 0;
		}

		for ( i=0; i<=roop_cnt; i++ ) {

			ret = Im_JPEG_Ctrl_Enc_Frame( &enc_frm_mng );
			Ddim_Print(("Im_JPEG_Ctrl_Enc_Frame ret=0x%X\n", ret));
			// Ctrl Print
			ct_jpeg_ctrl_print( E_PRINT_CTRL_ENC_FRAME, (VOID*)&enc_frm_mng );
			if ( ret != D_IM_JPEG_OK ) {
				Im_JPEG_Close();
				return;
			}

			Ddim_Print(("Im_JPEG_Get_Ctrl_Enc\n"));
			Im_JPEG_Get_Ctrl_Enc( &out_enc_mng );
			ct_jpeg_ctrl_print( E_PRINT_CTRL_ENC_BASE, (VOID*)&out_enc_mng );
			Ddim_Print(("Im_JPEG_Get_Ctrl_Enc_Frame\n"));
			Im_JPEG_Get_Ctrl_Enc_Frame( &out_enc_frm_mng );
			ct_jpeg_ctrl_print( E_PRINT_CTRL_ENC_FRAME, (VOID*)&out_enc_frm_mng );

			Ddim_Print(("Im_JPEG_Start_Enc\n"));
			ret = Im_JPEG_Start_Enc();
			Ddim_Print(("Im_JPEG_Start_Enc ret=0x%X\n", ret));
			if ( ret != D_IM_JPEG_OK ) {
				Im_JPEG_Close();
				return;
			}

#ifdef CO_CT_IM_JPEG_REG_DUMP
			ct_jpeg_start_hclock();
			Ddim_Print(("IO_JPG7.JINTEN   =0x%lX\n", IO_JPG7.JINTEN.word));
			Ddim_Print(("IO_JPG7.JPSTATUS.bit.JPSTATUS =0x%X\n", IO_JPG7.JPSTATUS.bit.JPSTATUS));
			Ddim_Print(("IO_JPG7.JPCMD    =0x%lX\n", IO_JPG7.JPCMD.word));
			Ddim_Print(("IO_JPG7.JPWIDTH  =0x%lX\n", IO_JPG7.JPWIDTH.word));
			Ddim_Print(("IO_JPG7.JPHEIGHT =0x%lX\n", IO_JPG7.JPHEIGHT.word));
			Ddim_Print(("IO_JPG7.JIMGYGH  =0x%lX\n", IO_JPG7.JIMGYGH.word));
			Ddim_Print(("IO_JPG7.JIMGCGH  =0x%lX\n", IO_JPG7.JIMGCGH.word));
			Ddim_Print(("IO_JPG7.JIMGSH   =0x%lX\n", IO_JPG7.JIMGSH.word));
			Ddim_Print(("IO_JPG7.JIMGSV   =0x%lX\n", IO_JPG7.JIMGSV.word));
			Ddim_Print(("IO_JPG7.JIMGSHDG =0x%lX\n", IO_JPG7.JIMGSHDG.word));
			Ddim_Print(("IO_JPG7.JPFORMAT =0x%lX\n", IO_JPG7.JPFORMAT.word));
			Ddim_Print(("IO_JPG7.JMODE    =0x%lX\n", IO_JPG7.JMODE.word));
			Ddim_Print(("IO_JPG7.JCCTL    =0x%lX\n", IO_JPG7.JCCTL.word));
			Ddim_Print(("IO_JPG7.JICTL    =0x%lX\n", IO_JPG7.JICTL.word));
			Ddim_Print(("IO_JPG7.JJBSTA   =0x%lX\n", IO_JPG7.JJBSTA.word));
			Ddim_Print(("IO_JPG7.JPBSTA   =0x%lX\n", IO_JPG7.JPBSTA.word));
			Ddim_Print(("IO_JPG7.JPMODE   =0x%lX\n", IO_JPG7.JPMODE.word));
			Ddim_Print(("IO_JPG7.JIMGAY.bit.JIMGAY   =0x%lX\n", IO_JPG7.JIMGAY.bit.JIMGAY));
			Ddim_Print(("IO_JPG7.JIMGACB.bit.JIMGACB =0x%lX\n", IO_JPG7.JIMGACB.bit.JIMGACB));
			Ddim_Print(("IO_JPG7.JCODA.bit.JCODA     =0x%lX\n", IO_JPG7.JCODA.bit.JCODA));

			ct_jpeg_stop_hclock();
#endif

/*
			while( 1 ) {
				Im_JPEG_Get_Axi_State( &buf_status );
				Ddim_Print(("Im_JPEG_Get_Axi_State status=0x%X\n", buf_status));
				if ( buf_status == E_IM_JPEG_AXI_ST_ACTIVE_BOTH ) {
					break;
				}
#ifdef CO_DEBUG_ON_PC
				IO_JPG7.JSTATE.bit.JALLACTP = 1;
				IO_JPG7.JSTATE.bit.JJBACTP = 1;
				IO_JPG7.JSTATE.bit.JPBACTP = 1;
#endif	// CO_DEBUG_ON_PC
			}
*/
			if ( roop_cnt != 0 ) {
				enc_frm_mng.code_addr		= 0x52000000;
				// For next frame
				ret = Im_JPEG_Ctrl_Enc_Frame( &enc_frm_mng );
				Ddim_Print(("Im_JPEG_Ctrl_Enc_Frame ret=0x%X\n", ret));
				// Ctrl Print
				ct_jpeg_ctrl_print( E_PRINT_CTRL_ENC_FRAME, (VOID*)&enc_frm_mng );
				if ( ret != D_IM_JPEG_OK ) {
					Im_JPEG_Close();
					return;
				}
			}

			if ( enc_param->sync ) {
#ifdef CO_DEBUG_ON_PC
				ct_jpeg_encode_int_handler(1);
#endif	// CO_DEBUG_ON_PC
				Ddim_Print(("Im_JPEG_Wait_End_Enc\n"));
				ret = Im_JPEG_Wait_End_Enc( &enc_mng, D_DDIM_WAIT_END_TIME );

				Ddim_Print(("Im_JPEG_Wait_End_Enc ret=0x%X\n", ret));
				memcpy(&gJpeg_Enc_Mng, &enc_mng, sizeof(T_IM_JPEG_ENC_MNG));

				if ( ret != D_IM_JPEG_OK ) {
					ct_jpeg_stop();
					Im_JPEG_Close();
					Ddim_Print(("***** Encode Error result=[0x%X] *****\n", gJpeg_Enc_Mng.result));
					return;
				}
			}
			else {
#ifdef CO_DEBUG_ON_PC
				ct_jpeg_encode_int_handler(1);
#endif	// CO_DEBUG_ON_PC

				while ( !gJpeg_Enc_End_Flg ) {
					DDIM_User_Dly_Tsk(1);
				}

				Ddim_Print(("***** Callback Wait End *****\n"));

				gJpeg_Enc_End_Flg = 0;

				if ( (gJpeg_Enc_Mng.result & D_IM_JPEG_PROC_END) != D_IM_JPEG_PROC_END ) {
					Ddim_Print(("***** Encode Error result=[0x%X] *****\n", gJpeg_Enc_Mng.result));
					ct_jpeg_stop();
					Ddim_Print(("***** close *****\n"));
					Im_JPEG_Close();
					return;
				}
			}

			gJpeg_Encode_Size = ( gJpeg_Enc_Mng.code_size << (enc_param->dsp_rate * 2) );
			gJpeg_OutputBytes = gJpeg_Encode_Size;

//			ct_jpeg_measure_time_print();
			Ddim_Print(("***** Encode %d result                  =[0x%X] *****\n", i, gJpeg_Enc_Mng.result));
			Ddim_Print(("***** Encode %d gJpeg_Enc_Mng.code_size =[0x%lX] *****\n", i, gJpeg_Enc_Mng.code_size));
			Ddim_Print(("***** Encode %d gJpeg_Encode_Size       =[0x%lX] *****\n", i, gJpeg_Encode_Size));
		}
	}
	else {
	// pause

		// limit size
		pause_size = enc_param->limit_size / 2;
		sector_cnt = (ULONG)( pause_size / 512 );
		enc_frm_mng.limit_size = pause_size;

		for ( i=0; i<=1; i++ ) {

			if ( i == 0 ) {

				ret = Im_JPEG_Ctrl_Enc_Frame( &enc_frm_mng );
				Ddim_Print(("Im_JPEG_Ctrl_Enc_Frame ret=0x%X\n", ret));
				ct_jpeg_ctrl_print( E_PRINT_CTRL_ENC_FRAME, (VOID*)&enc_frm_mng );
				if ( ret != D_IM_JPEG_OK ) {
					Im_JPEG_Close();
					return;
				}

				Im_JPEG_Get_Ctrl_Enc( &out_enc_mng );
				ct_jpeg_ctrl_print( E_PRINT_CTRL_ENC_BASE, (VOID*)&out_enc_mng );
				Im_JPEG_Get_Ctrl_Enc_Frame( &out_enc_frm_mng );
				ct_jpeg_ctrl_print( E_PRINT_CTRL_ENC_FRAME, (VOID*)&out_enc_frm_mng );

				Ddim_Print(("Im_JPEG_Start_Enc\n"));
				ret = Im_JPEG_Start_Enc();
				Ddim_Print(("Im_JPEG_Start_Enc ret=0x%X\n", ret));
				if ( ret != D_IM_JPEG_OK ) {
					Im_JPEG_Close();
					return;
				}

				ct_jpeg_start_hclock();
				Ddim_Print(("IO_JPG7.JINTEN =0x%lX\n", IO_JPG7.JINTEN.word));
				Ddim_Print(("IO_JPG7.JPSTATUS.bit.JPSTATUS =0x%X\n", IO_JPG7.JPSTATUS.bit.JPSTATUS));
				Ddim_Print(("IO_JPG7.JPCMD  =0x%lX\n", IO_JPG7.JPCMD.word));
				Ddim_Print(("IO_JPG7.JICTL  =0x%lX\n", IO_JPG7.JICTL.word));
				Ddim_Print(("IO_JPG7.JCCTL  =0x%lX\n", IO_JPG7.JCCTL.word));
				ct_jpeg_stop_hclock();

			}
			else {
				// none limit
				enc_frm_mng.limit_size = 0;
				enc_frm_mng.code_addr = D_CT_JPEG_CODE_ADDR + (sector_cnt * 512);

				ret = Im_JPEG_Restart_Enc(&enc_frm_mng);
				Ddim_Print(("Im_JPEG_Restart_Enc ret=0x%X\n", ret));
				ct_jpeg_ctrl_print( E_PRINT_CTRL_ENC_FRAME, (VOID*)&enc_frm_mng );
				if ( ret != D_IM_JPEG_OK ) {
					Im_JPEG_Close();
					return;
				}
			}

			if ( enc_param->sync ) {
#ifdef CO_DEBUG_ON_PC
				if ( i == 0 ) {
					ct_jpeg_encode_int_handler(2);
				}
				else {
					ct_jpeg_encode_int_handler(1);
				}
#endif	// CO_DEBUG_ON_PC
				ret = Im_JPEG_Wait_End_Enc( &enc_mng, D_DDIM_WAIT_END_TIME );

				Ddim_Print(("Im_JPEG_Wait_End_Enc ret=0x%X\n", ret));
				memcpy(&gJpeg_Enc_Mng, &enc_mng, sizeof(T_IM_JPEG_ENC_MNG));

				if ( ret != D_IM_JPEG_OK ) {
					if ( i == 0 && ret == D_IM_JPEG_ENCODE_PAUSE ) {
						Ddim_Print(("Encode Pause\n"));
#ifdef CO_DEBUG_ON_PC
						IO_JPG7.JCCTL.bit.JCTRG = 2;
#endif	// CO_DEBUG_ON_PC
					}
					else {
						ct_jpeg_stop();
						Im_JPEG_Close();
						Ddim_Print(("Encode Error result=0x%X\n", gJpeg_Enc_Mng.result));
						return;
					}
				}
			}
			else {
#ifdef CO_DEBUG_ON_PC
				if ( i == 0 ) {
					ct_jpeg_encode_int_handler(2);
				}
				else {
					ct_jpeg_encode_int_handler(1);
				}
#endif	// CO_DEBUG_ON_PC
				while ( !gJpeg_Enc_End_Flg ) {
					;
				}
				gJpeg_Enc_End_Flg = 0;

				if ( (gJpeg_Enc_Mng.result & D_IM_JPEG_PROC_END) != D_IM_JPEG_PROC_END ) {
					if ( i == 0 && ret == D_IM_JPEG_ENCODE_PAUSE ) {
						Ddim_Print(("Encode Pause\n"));
					}
					else {
						ct_jpeg_stop();
						Im_JPEG_Close();
						Ddim_Print(("***** Encode Error result=[0x%X] *****\n", gJpeg_Enc_Mng.result));
						return;
					}
				}
			}

			Ddim_Print(("***** Encode %d result                 =[0x%X] *****\n", i, gJpeg_Enc_Mng.result));
			Ddim_Print(("***** Encode %d gJpeg_Enc_Mng.code_size=[0x%lX] *****\n", i, gJpeg_Enc_Mng.code_size));
		}

		gJpeg_Encode_Size = gJpeg_Enc_Mng.code_size;
		gJpeg_OutputBytes = gJpeg_Encode_Size;

		Ddim_Print(("***** Encode gJpeg_Encode_Size=[0x%lX] *****\n", gJpeg_Encode_Size));
	}
// ### REMOVE_RELEASE BEGIN

#ifdef CO_PT_ENABLE
	if( gJpeg_Encode_Size != 0 ) {
		Ddim_Print(("%s() Palladium_Set_Out_Localstack( 0x%lx, %lu )\n", __func__, code_addr, gJpeg_Encode_Size));
		Palladium_Set_Out_Localstack( code_addr, gJpeg_Encode_Size );
	}
#endif
// ### REMOVE_RELEASE END

	ret = Im_JPEG_Close();
	Ddim_Print(("Im_JPEG_Close ret=0x%X\n", ret));
	if ( ret != D_IM_JPEG_OK ) {
		return;
	}

	return;
}

static VOID ct_jpeg_encode_force_stop( T_CT_JPEG_ENC_PARAM* enc_param )
{
	T_IM_JPEG_ENC_MNG enc_mng;
	T_IM_JPEG_ENC_FRAME_MNG enc_frm_mng;
	INT32 ret = 0;
	E_IM_JPEG_AXI_ST		buf_status;

	memset( &enc_mng, 0, sizeof( T_IM_JPEG_ENC_MNG ) );
	memset( &enc_frm_mng, 0, sizeof( T_IM_JPEG_ENC_FRAME_MNG ) );

	// Base
	switch( enc_param->format ) {
		case E_CT_JPEG_FORMAT_444P:
		case E_CT_JPEG_FORMAT_RGBP:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC444;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_422P:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC422;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_422PD:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC422;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE_DOT;
			break;
		case E_CT_JPEG_FORMAT_420P:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC420;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_420PD:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC420;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE_DOT;
			break;
		case E_CT_JPEG_FORMAT_400P:
			enc_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC400;
			enc_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		default:
			Ddim_Print(("input parameter error\n"));
			return;
	}

	enc_mng.width = enc_param->h_size;
	enc_mng.lines = enc_param->v_size;

	enc_mng.component[0].quant_tbl_no  = 0;	// Y
	enc_mng.component[0].huf_dc_tbl_no = 0;	// Y
	enc_mng.component[0].huf_ac_tbl_no = 0;	// Y
	enc_mng.component[1].quant_tbl_no  = 1;	// CB
	enc_mng.component[1].huf_dc_tbl_no = 1;	// CB
	enc_mng.component[1].huf_ac_tbl_no = 1;	// CB
	enc_mng.component[2].quant_tbl_no  = 1;	// CR
	enc_mng.component[2].huf_dc_tbl_no = 1;	// CR
	enc_mng.component[2].huf_ac_tbl_no = 1;	// CR

	enc_mng.skip_mk_flg = D_IM_JPEG_ENABLE_OFF;

	if ( enc_param->dri ) {
		enc_mng.dri_mk_num = 0x10;
	}
	else {
		enc_mng.dri_mk_num = 0;
	}

	enc_mng.exif_fmt_flg				= D_IM_JPEG_ENABLE_ON;
	enc_mng.pint_line					= 0;
	enc_mng.pint_sect					= 0;
	enc_mng.pbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	enc_mng.pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	enc_mng.pbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF;
	enc_mng.pbuf_ctrl.prot_type			= D_IM_JPEG_D_SEC_NORMAL_ACCESS;
	enc_mng.jbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	enc_mng.jbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	enc_mng.jbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF;
	enc_mng.jbuf_ctrl.prot_type			= D_IM_JPEG_D_SEC_NORMAL_ACCESS;
	enc_mng.jburst_length				= E_IM_JPEG_BURST_INCR_8;
	enc_mng.pburst_length				= E_IM_JPEG_BURST_INCR_8;

	if ( enc_param->sync ) {
		enc_mng.pcallback = NULL;
	}
	else {
		enc_mng.pcallback = (VP_CALLBACK)ct_jpeg_encode_cb;
	}

	// Frame
	enc_frm_mng.global_y_width = enc_param->g_y_width;
	enc_frm_mng.global_c_width = enc_param->g_c_width;

	// Y addr
	enc_frm_mng.ycc_addr.y = D_CT_JPEG_YCC_ADDR;

	if ( enc_mng.ycc_smpl != E_IM_JPEG_SMPL_TYPE_YCC400 ) {

		// Cb addr
		enc_frm_mng.ycc_addr.c = D_CT_JPEG_YCC_ADDR + (enc_param->g_y_width * enc_param->g_y_lines);
	}

	// Code addr
	enc_frm_mng.code_addr		= D_CT_JPEG_CODE_ADDR;
	enc_frm_mng.code_count_flg	= enc_param->count_flg;

	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	Ddim_Print(("Im_JPEG_Open ret=0x%X\n", ret));
	if ( ret != D_IM_JPEG_OK ) {
		return;
	}

	Im_JPEG_Set_Quality( 1024, 0 );

	ret = Im_JPEG_Ctrl_Enc( &enc_mng );
	Ddim_Print(("Im_JPEG_Ctrl_Enc ret=0x%X\n", ret));
	if ( ret != D_IM_JPEG_OK ) {
		Im_JPEG_Close();
		return;
	}

	// limit size
	enc_frm_mng.limit_size		= enc_param->limit_size;

	ret = Im_JPEG_Ctrl_Enc_Frame( &enc_frm_mng );
	Ddim_Print(("Im_JPEG_Ctrl_Enc_Frame ret=0x%X\n", ret));
	if ( ret != D_IM_JPEG_OK ) {
		Im_JPEG_Close();
		return;
	}

	Ddim_Print(("Im_JPEG_Start_Enc\n"));
	ret = Im_JPEG_Start_Enc();
	Ddim_Print(("Im_JPEG_Start_Enc ret=0x%X\n", ret));
	if ( ret != D_IM_JPEG_OK ) {
		Im_JPEG_Close();
		return;
	}

	while( 1 ) {
		Im_JPEG_Get_Axi_State( &buf_status );
		if ( buf_status == E_IM_JPEG_AXI_ST_ACTIVE_BOTH ) {
			break;
		}
#ifdef CO_DEBUG_ON_PC
		IO_JPG7.JSTATE.bit.JALLACTP = 1;
		IO_JPG7.JSTATE.bit.JJBACTP = 1;
		IO_JPG7.JSTATE.bit.JPBACTP = 1;
#endif	// CO_DEBUG_ON_PC
	}

	// Force stop
	ct_jpeg_stop();

	Im_JPEG_Get_Axi_State( &buf_status );
	Ddim_Print(("Im_JPEG_Get_Axi_State status=0x%X\n", buf_status));

	ct_jpeg_start_hclock();
	Ddim_Print(("IO_JPG7.JPSTATUS.bit.JPSTATUS=0x%X\n", IO_JPG7.JPSTATUS.bit.JPSTATUS));
	ct_jpeg_stop_hclock();

	ret = Im_JPEG_Close();
	Ddim_Print(("Im_JPEG_Close ret=0x%X\n", ret));
	if ( ret != D_IM_JPEG_OK ) {
		return;
	}

	return;
}

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
static VOID ct_jpeg_decode_palladium_in( const T_IM_JPEG_DEC_MNG* const dec_mng, const T_IM_JPEG_DEC_FRAME_MNG* const dec_frm_mng, const T_CT_JPEG_DEC_PARAM* const dec_param )
{
	unsigned long in_data_siz_y;	// Maximum data bytes of Y. (Compression ratio=x1.0)
	unsigned long in_data_siz_c;	// Maximum data bytes of C. (Compression ratio=x1.0)

	// Calc Y bytes.
	in_data_siz_y = ((unsigned long)dec_frm_mng->global_y_width) * dec_param->g_y_lines;

	switch( dec_mng->smpl_type ) {
		case E_IM_JPEG_SMPL_TYPE_YCC444:	// FALLTHROUGH
		case E_IM_JPEG_SMPL_TYPE_YCC422:
			in_data_siz_c = ((unsigned long)dec_frm_mng->global_c_width) * dec_param->g_y_lines;
			break;
//		case E_IM_JPEG_SMPL_TYPE_YCC420:
		default:
			in_data_siz_c = ((unsigned long)dec_frm_mng->global_c_width) * (dec_param->g_y_lines /2);
			break;
		case E_IM_JPEG_SMPL_TYPE_YCC400:
			in_data_siz_c = 0ul;
	}
	if( dec_mng->mem_format == E_IM_JPEG_MEM_FORM_PLANE ) {
		in_data_siz_c *= 2;
	}


	Ddim_Print(("%s() Palladium_Set_In_Localstack( 0x%lx, %lu )\n", __func__, ((unsigned long)dec_frm_mng->code_addr), in_data_siz_y + in_data_siz_c));
	Palladium_Set_In_Localstack( ((unsigned long)dec_frm_mng->code_addr), in_data_siz_y + in_data_siz_c );
	Palladium_Set_Out_Localstack( ((unsigned long)dec_frm_mng->code_addr), in_data_siz_y + in_data_siz_c );	// for Input file check.
}

static VOID ct_jpeg_decode_palladium_out( const T_IM_JPEG_DEC_MNG* const dec_mng, const T_IM_JPEG_DEC_FRAME_MNG* const dec_frm_mng, const T_CT_JPEG_DEC_PARAM* const dec_param )
{
	unsigned long out_data_siz_y;
	unsigned long out_data_siz_c;

	// Calc Y bytes.
	out_data_siz_y = ((unsigned long)dec_frm_mng->global_y_width) * dec_param->g_y_lines;

	switch( dec_mng->smpl_type ) {
		case E_IM_JPEG_SMPL_TYPE_YCC444:	// FALLTHROUGH
		case E_IM_JPEG_SMPL_TYPE_YCC422:
			out_data_siz_c = ((unsigned long)dec_frm_mng->global_c_width) * dec_param->g_y_lines;
			break;
//		case E_IM_JPEG_SMPL_TYPE_YCC420:
		default:
			out_data_siz_c = ((unsigned long)dec_frm_mng->global_c_width) * (dec_param->g_y_lines /2);
			break;
		case E_IM_JPEG_SMPL_TYPE_YCC400:
			out_data_siz_c = 0ul;
	}
	if( dec_mng->mem_format == E_IM_JPEG_MEM_FORM_PLANE ) {
		out_data_siz_c *= 2;
	}


	Ddim_Print(("%s() Palladium_Set_Out_Localstack( 0x%lx, %lu )\n", __func__, ((unsigned long)dec_frm_mng->ycc_addr.y), out_data_siz_y + out_data_siz_c));
	Palladium_Set_Out_Localstack( ((unsigned long)dec_frm_mng->ycc_addr.y), out_data_siz_y + out_data_siz_c );
}
#endif

// ### REMOVE_RELEASE END

static VOID ct_jpeg_decode( T_CT_JPEG_DEC_PARAM* dec_param )
{

	T_IM_JPEG_DEC_MNG dec_mng;
	T_IM_JPEG_DEC_FRAME_MNG dec_frm_mng;
	INT32	ret=0;
	ULLONG	pause_size;
	ULONG	sector_cnt;
	INT32	i;

	memset( &dec_mng, 0, sizeof( T_IM_JPEG_DEC_MNG ) );
	memset( &dec_frm_mng, 0, sizeof( T_IM_JPEG_DEC_FRAME_MNG ) );

	switch( dec_param->format ) {
		case E_CT_JPEG_FORMAT_444P:
		case E_CT_JPEG_FORMAT_RGBP:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC444;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_422P:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC422;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_422PD:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC422;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE_DOT;
			break;
		case E_CT_JPEG_FORMAT_420P:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC420;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_420PD:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC420;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE_DOT;
			break;
		case E_CT_JPEG_FORMAT_400P:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC400;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		default:
			Ddim_Print(("input parameter error\n"));
			return;
	}

	dec_mng.ext_mode 	= dec_param->ext;
	dec_mng.skip_mk_flg	= 0;
	dec_mng.pint_line	= 0;
	dec_mng.pint_sect	= 0;
	dec_mng.corr_mode	= dec_param->corr_flg;

	dec_mng.color_band.y_band	= 0x01;
	dec_mng.color_band.cb_band	= 0x01;
	dec_mng.color_band.cr_band	= 0x01;

	dec_mng.pbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	dec_mng.pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	dec_mng.pbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF;
	dec_mng.pbuf_ctrl.prot_type			= D_IM_JPEG_D_SEC_NORMAL_ACCESS;
	dec_mng.jbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	dec_mng.jbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	dec_mng.jbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF;
	dec_mng.jbuf_ctrl.prot_type			= D_IM_JPEG_D_SEC_NORMAL_ACCESS;
	dec_mng.jburst_length				= E_IM_JPEG_BURST_INCR_8;
	dec_mng.pburst_length				= E_IM_JPEG_BURST_INCR_8;

	if ( dec_param->sync ) {
		dec_mng.pcallback = NULL;
	}
	else {
		dec_mng.pcallback = (VP_CALLBACK)ct_jpeg_decode_cb;
	}

	// Frame memory
	// Y
	dec_frm_mng.global_y_width = dec_param->g_y_width;

	if ( dec_param->cutout_flg ) {
		dec_frm_mng.cutout_flg		= D_IM_JPEG_ENABLE_ON;
		dec_frm_mng.cutout_width	= dec_param->cut_h_size;
		dec_frm_mng.cutout_lines	= dec_param->cut_v_size;
		dec_frm_mng.cutout_offset_h = ((dec_param->h_size - dec_param->cut_h_size) / 2 );
		dec_frm_mng.cutout_offset_v = ((dec_param->v_size - dec_param->cut_v_size) / 2 );

		if ( (dec_frm_mng.cutout_offset_h & 0xF) != 0 ) {
			dec_frm_mng.cutout_offset_h = ((dec_frm_mng.cutout_offset_h >> 4) << 4) + 16;
		}
		if ( (dec_frm_mng.cutout_offset_v & 0xF) != 0 ) {
			dec_frm_mng.cutout_offset_v = ((dec_frm_mng.cutout_offset_v >> 4) << 4) + 16;
		}

		if( gJpeg_AXI_Err ) {
			dec_frm_mng.ycc_addr.y		= D_CT_JPEG_YCC_ADDR_ERR;
		}
		else {
			dec_frm_mng.ycc_addr.y		= D_CT_JPEG_YCC_ADDR;
		}

		// CbCr
		dec_frm_mng.global_c_width = dec_param->g_c_width;
		dec_frm_mng.ycc_addr.c		= D_CT_JPEG_YCC_ADDR + (dec_param->g_y_width * dec_param->g_y_lines);
	}
	else {
		dec_frm_mng.cutout_flg		= D_IM_JPEG_ENABLE_OFF;
		dec_frm_mng.cutout_width	= 0;
		dec_frm_mng.cutout_lines	= 0;
		dec_frm_mng.cutout_offset_h	= 0;
		dec_frm_mng.cutout_offset_v	= 0;

		if( gJpeg_AXI_Err ) {
			dec_frm_mng.ycc_addr.y		= D_CT_JPEG_YCC_ADDR_ERR;
		}
		else {
			dec_frm_mng.ycc_addr.y		= D_CT_JPEG_YCC_ADDR;
		}

		// CbCr
		dec_frm_mng.global_c_width = dec_param->g_c_width;
		dec_frm_mng.ycc_addr.c		= D_CT_JPEG_YCC_ADDR + (dec_param->g_y_width * dec_param->g_y_lines);
	}
	dec_frm_mng.code_addr		= D_CT_JPEG_CODE_ADDR;

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
	ct_jpeg_decode_palladium_in( &dec_mng, &dec_frm_mng, dec_param );
#endif

// ### REMOVE_RELEASE END
//	ct_jpeg_measure_time_start();

	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	Ddim_Print(("Im_JPEG_Open ret=0x%X\n", ret));
	if ( ret != D_IM_JPEG_OK ) {
		return;
	}

	ret = Im_JPEG_Ctrl_Dec( &dec_mng );
	Ddim_Print(("Im_JPEG_Ctrl_Dec ret=0x%X\n", ret));
	ct_jpeg_ctrl_print( E_PRINT_CTRL_DEC_BASE, (VOID*)&dec_mng );
	if ( ret != D_IM_JPEG_OK ) {
		Im_JPEG_Close();
		return;
	}

	if ( !dec_param->pause_flg ) {

		// limit size
		dec_frm_mng.code_size		= dec_param->limit_size;

		ret = Im_JPEG_Ctrl_Dec_Frame( &dec_frm_mng );
		Ddim_Print(("Im_JPEG_Ctrl_Dec_Frame ret=0x%X\n", ret));
		// Ctrl Print
		ct_jpeg_ctrl_print( E_PRINT_CTRL_DEC_FRAME, (VOID*)&dec_frm_mng );
		if ( ret != D_IM_JPEG_OK ) {
			Im_JPEG_Close();
			return;
		}

		ret = Im_JPEG_Start_Dec();
		Ddim_Print(("Im_JPEG_Start_Dec ret=0x%X\n", ret));
		if ( ret != D_IM_JPEG_OK ) {
			Im_JPEG_Close();
			return;
		}

		if ( dec_param->sync ) {
#ifdef CO_DEBUG_ON_PC
			IO_JPG7.JPWIDTH.bit.JPWIDTH		= dec_param->h_size;
			IO_JPG7.JPHEIGHT.bit.JPHEIGHT	= dec_param->v_size;
			IO_JPG7.JPMODE.bit.MMODE		= (UCHAR)dec_mng.smpl_type;

			ct_jpeg_decode_int_handler(1);

#endif	// CO_DEBUG_ON_PC

			ret = Im_JPEG_Wait_End_Dec( &dec_mng, 5000 );
			Ddim_Print(("Im_JPEG_Wait_End_Dec ret=0x%X\n", ret));

			memcpy(&gJpeg_Dec_Mng, &dec_mng, sizeof(T_IM_JPEG_DEC_MNG));

			if ( ret != D_IM_JPEG_OK ) {
				ct_jpeg_stop();
				Im_JPEG_Close();
				Ddim_Print(("***** Decode Error result=[0x%X] *****\n", gJpeg_Dec_Mng.result));
				Ddim_Print(("***** Decode Error Code=[0x%X] *****\n", gJpeg_Dec_Mng.err_code));
				Ddim_Print(("***** Decode Sampling type[%d] *****\n", gJpeg_Dec_Mng.smpl_type));
				return;
			}

		}
		else {
#ifdef CO_DEBUG_ON_PC
			IO_JPG7.JPWIDTH.bit.JPWIDTH		= dec_param->h_size;
			IO_JPG7.JPHEIGHT.bit.JPHEIGHT	= dec_param->v_size;
			IO_JPG7.JPMODE.bit.MMODE		= (UCHAR)dec_mng.smpl_type;

			ct_jpeg_decode_int_handler(1);

#endif	// CO_DEBUG_ON_PC

			while ( !gJpeg_Dec_End_Flg ) {
				DDIM_User_Dly_Tsk(1);
			}
			gJpeg_Dec_End_Flg = 0;

			if ( (gJpeg_Dec_Mng.result & D_IM_JPEG_PROC_END) != D_IM_JPEG_PROC_END ) {
					ct_jpeg_stop();
					Im_JPEG_Close();
					Ddim_Print(("***** Decode Error result=[0x%X] *****\n", gJpeg_Dec_Mng.result));
					Ddim_Print(("***** Decode Error Code=[0x%X] *****\n", gJpeg_Dec_Mng.err_code));
					Ddim_Print(("***** Decode Sampling type[%d] *****\n", gJpeg_Dec_Mng.smpl_type));
					return;
			}
		}

//		ct_jpeg_measure_time_print();
		Ddim_Print(("***** Decode result=[0x%X] *****\n", gJpeg_Dec_Mng.result));
		Ddim_Print(("***** Decode Sampling type[%d] *****\n", gJpeg_Dec_Mng.smpl_type));
		Ddim_Print(("***** Decode Width[%d], Height[%d] *****\n", gJpeg_Dec_Mng.org_width, gJpeg_Dec_Mng.org_lines));

	}
	else {
		pause_size = dec_param->limit_size / 2;
		sector_cnt = (ULONG)( pause_size / 512 ) + 1;
		dec_frm_mng.code_size = pause_size;

		Ddim_Print(("***** pause_size=[0x%llX] *****\n", pause_size));

		for ( i=0; i <= 1; i++ ) {

			if (i == 1) {
				// none limit
				dec_frm_mng.code_size = 0;
				dec_frm_mng.code_addr = D_CT_JPEG_CODE_ADDR + (sector_cnt * 512);

#ifdef CO_DEBUG_ON_PC
				IO_JPG7.JCCTL.bit.JCTRG = 2;
#endif	// CO_DEBUG_ON_PC
				ret = Im_JPEG_Restart_Dec(&dec_frm_mng);
				Ddim_Print(("Im_JPEG_Restart_Dec ret=0x%X\n", ret));
				ct_jpeg_ctrl_print( E_PRINT_CTRL_DEC_FRAME, (VOID*)&dec_frm_mng );
				if ( ret != D_IM_JPEG_OK ) {
					ct_jpeg_stop();
					Im_JPEG_Close();
					return;
				}

			}
			else {

				ret = Im_JPEG_Ctrl_Dec_Frame( &dec_frm_mng );
				Ddim_Print(("Im_JPEG_Ctrl_Dec_Frame ret=0x%X\n", ret));
				ct_jpeg_ctrl_print( E_PRINT_CTRL_DEC_FRAME, (VOID*)&dec_frm_mng );
				if ( ret != D_IM_JPEG_OK ) {
					Im_JPEG_Close();
					return;
				}

				ret = Im_JPEG_Start_Dec();
				Ddim_Print(("Im_JPEG_Start_Dec ret=0x%X\n", ret));
				if ( ret != D_IM_JPEG_OK ) {
					Im_JPEG_Close();
					return;
				}
			}

			if ( dec_param->sync ) {

#ifdef CO_DEBUG_ON_PC
				if ( i == 0 ) {
					IO_JPG7.JPWIDTH.bit.JPWIDTH		= dec_param->h_size;
					IO_JPG7.JPHEIGHT.bit.JPHEIGHT	= dec_param->v_size;
					IO_JPG7.JPMODE.bit.MMODE		= (UCHAR)dec_mng.smpl_type;

					ct_jpeg_decode_int_handler(2);

					Ddim_Print(("IO_JPG7.JPWIDTH.bit.JPWIDTH   =%d\n", IO_JPG7.JPWIDTH.bit.JPWIDTH));
					Ddim_Print(("IO_JPG7.JPHEIGHT.bit.JPHEIGHT =%d\n", IO_JPG7.JPHEIGHT.bit.JPHEIGHT));
					Ddim_Print(("IO_JPG7.JIMGSH.bit.JIMGSH     =%d\n", IO_JPG7.JIMGSH.bit.JIMGSH));
					Ddim_Print(("IO_JPG7.JIMGSV.bit.JIMGSV     =%d\n", IO_JPG7.JIMGSV.bit.JIMGSV));
					Ddim_Print(("IO_JPG7.JIMGSHDG.bit.JIMGSHDG =%d\n", IO_JPG7.JIMGSHDG.bit.JIMGSHDG));
					Ddim_Print(("IO_JPG7.JHOFFSET.bit.JHOFFSET =%d\n", IO_JPG7.JHOFFSET.bit.JHOFFSET));
					Ddim_Print(("IO_JPG7.JVOFFSET.bit.JVOFFSET =%d\n", IO_JPG7.JVOFFSET.bit.JVOFFSET));
				}
				else {
					ct_jpeg_decode_int_handler(3);
				}
#endif	// CO_DEBUG_ON_PC
				ret = Im_JPEG_Wait_End_Dec( &dec_mng, D_DDIM_WAIT_END_TIME );
				Ddim_Print(("Im_JPEG_Wait_End_Dec ret=0x%X\n", ret));

				memcpy(&gJpeg_Dec_Mng, &dec_mng, sizeof(T_IM_JPEG_DEC_MNG));

				if ( ret != D_IM_JPEG_OK ) {
					if ( i == 0 && ret == D_IM_JPEG_DECODE_PAUSE ) {
						Ddim_Print(("Decode Pause \n"));
					}
					else {
						ct_jpeg_stop();
						Im_JPEG_Close();
						Ddim_Print(("***** Decode Error result=[0x%X] *****\n", gJpeg_Dec_Mng.result));
						Ddim_Print(("***** Decode Error Code=[0x%X] *****\n", gJpeg_Dec_Mng.err_code));
						Ddim_Print(("***** Decode Sampling type[%d] *****\n", gJpeg_Dec_Mng.smpl_type));
						return;
					}
				}
			}
			else {

#ifdef CO_DEBUG_ON_PC
				if ( i == 0 ) {
					IO_JPG7.JPWIDTH.bit.JPWIDTH		= dec_param->h_size;
					IO_JPG7.JPHEIGHT.bit.JPHEIGHT	= dec_param->v_size;
					IO_JPG7.JPMODE.bit.MMODE			= (UCHAR)dec_mng.smpl_type;

					ct_jpeg_decode_int_handler(2);

					Ddim_Print(("IO_JPG7.JPWIDTH.bit.JPWIDTH=%d\n", IO_JPG7.JPWIDTH.bit.JPWIDTH));
					Ddim_Print(("IO_JPG7.JPHEIGHT.bit.JPHEIGHT=%d\n", IO_JPG7.JPHEIGHT.bit.JPHEIGHT));
					Ddim_Print(("IO_JPG7.JIMGSH.bit.JIMGSH=%d\n", IO_JPG7.JIMGSH.bit.JIMGSH));
					Ddim_Print(("IO_JPG7.JIMGSV.bit.JIMGSV=%d\n", IO_JPG7.JIMGSV.bit.JIMGSV));
					Ddim_Print(("IO_JPG7.JIMGSHDG.bit.JIMGSHDG=%d\n", IO_JPG7.JIMGSHDG.bit.JIMGSHDG));
					Ddim_Print(("IO_JPG7.JHOFFSET.bit.JHOFFSET=%d\n", IO_JPG7.JHOFFSET.bit.JHOFFSET));
					Ddim_Print(("IO_JPG7.JVOFFSET.bit.JVOFFSET=%d\n", IO_JPG7.JVOFFSET.bit.JVOFFSET));
				}
				else {
					ct_jpeg_decode_int_handler(3);
				}
#endif	// CO_DEBUG_ON_PC
				while ( !gJpeg_Dec_End_Flg ) {
					DDIM_User_Dly_Tsk(1);
				}
				gJpeg_Dec_End_Flg = 0;

				if ( (gJpeg_Dec_Mng.result & D_IM_JPEG_PROC_END) != D_IM_JPEG_PROC_END ) {
					if ( i == 0 && (gJpeg_Dec_Mng.result & D_IM_JPEG_PAUSE) == D_IM_JPEG_PAUSE ) {
						Ddim_Print(("Decode Pause\n"));
					}
					else {
						ct_jpeg_stop();
						Im_JPEG_Close();
						Ddim_Print(("***** Decode Error result=[0x%X] *****\n", gJpeg_Dec_Mng.result));
						Ddim_Print(("***** Decode Error Code=[0x%X] *****\n", gJpeg_Dec_Mng.err_code));
						Ddim_Print(("***** Decode Sampling type[%d] *****\n", gJpeg_Dec_Mng.smpl_type));
						return;
					}
				}
			}
		}

		Ddim_Print(("***** Decode result=[0x%X] *****\n", gJpeg_Dec_Mng.result));
		Ddim_Print(("***** Decode Sampling type[%d] *****\n", gJpeg_Dec_Mng.smpl_type));
		Ddim_Print(("***** Decode Width[%d], Height[%d] *****\n", gJpeg_Dec_Mng.org_width, gJpeg_Dec_Mng.org_lines));

	}

	ct_jpeg_set_ycc_out_bytes( dec_mng.smpl_type, dec_mng.mem_format, dec_frm_mng.global_y_width, dec_frm_mng.global_c_width, gJpeg_Dec_Mng.org_lines );

// ### REMOVE_RELEASE BEGIN

#ifdef CO_PT_ENABLE
	ct_jpeg_decode_palladium_out( &dec_mng, &dec_frm_mng, dec_param );
#endif
// ### REMOVE_RELEASE END
	ret = Im_JPEG_Close();
	Ddim_Print(("Im_JPEG_Close ret=0x%X\n", ret));
	if ( ret != D_IM_JPEG_OK ) {
		return;
	}

}


static VOID ct_jpeg_decode_mk_skip( T_CT_JPEG_DEC_PARAM* dec_param )
{
	T_IM_JPEG_DEC_MNG dec_mng;
	T_IM_JPEG_DEC_FRAME_MNG dec_frm_mng;
	INT32	ret=0;
	INT32	i;

	memset( &dec_mng, 0, sizeof( T_IM_JPEG_DEC_MNG ) );
	memset( &dec_frm_mng, 0, sizeof( T_IM_JPEG_DEC_FRAME_MNG ) );

	switch( dec_param->format ) {
		case E_CT_JPEG_FORMAT_444P:
		case E_CT_JPEG_FORMAT_RGBP:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC444;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_422P:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC422;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_422PD:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC422;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE_DOT;
			break;
		case E_CT_JPEG_FORMAT_420P:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC420;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		case E_CT_JPEG_FORMAT_420PD:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC420;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE_DOT;
			break;
		case E_CT_JPEG_FORMAT_400P:
			dec_mng.smpl_type	= E_IM_JPEG_SMPL_TYPE_YCC400;
			dec_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
			break;
		default:
			Ddim_Print(("input parameter error\n"));
			return;
	}

	dec_mng.ext_mode 	= dec_param->ext;
	dec_mng.skip_mk_flg	= 0;
	dec_mng.pint_line	= 0;
	dec_mng.pint_sect	= 0;
	dec_mng.corr_mode	= dec_param->corr_flg;

	dec_mng.color_band.y_band	= 0x01;
	dec_mng.color_band.cb_band	= 0x01;
	dec_mng.color_band.cr_band	= 0x01;

	dec_mng.pbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	dec_mng.pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	dec_mng.pbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF;
	dec_mng.pbuf_ctrl.prot_type			= D_IM_JPEG_D_SEC_NORMAL_ACCESS;
	dec_mng.jbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	dec_mng.jbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	dec_mng.jbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF;
	dec_mng.jbuf_ctrl.prot_type			= D_IM_JPEG_D_SEC_NORMAL_ACCESS;
	dec_mng.jburst_length				= E_IM_JPEG_BURST_INCR_8;
	dec_mng.pburst_length				= E_IM_JPEG_BURST_INCR_8;

	if ( dec_param->sync ) {
		dec_mng.pcallback = NULL;
	}
	else {
		dec_mng.pcallback = (VP_CALLBACK)ct_jpeg_decode_cb;
	}

	// Frame memory
	// Y
	dec_frm_mng.global_y_width = dec_param->g_y_width;

	if ( dec_param->cutout_flg ) {
		dec_frm_mng.cutout_flg		= D_IM_JPEG_ENABLE_ON;
		dec_frm_mng.cutout_width	= dec_param->cut_h_size;
		dec_frm_mng.cutout_lines	= dec_param->cut_v_size;
		dec_frm_mng.cutout_offset_h = ((dec_param->h_size - dec_param->cut_h_size) / 2 );
		dec_frm_mng.cutout_offset_v = ((dec_param->v_size - dec_param->cut_v_size) / 2 );

		if ( (dec_frm_mng.cutout_offset_h & 0xF) != 0 ) {
			dec_frm_mng.cutout_offset_h = ((dec_frm_mng.cutout_offset_h >> 4) << 4) + 16;
		}
		if ( (dec_frm_mng.cutout_offset_v & 0xF) != 0 ) {
			dec_frm_mng.cutout_offset_v = ((dec_frm_mng.cutout_offset_v >> 4) << 4) + 16;
		}

		if( gJpeg_AXI_Err ) {
			dec_frm_mng.ycc_addr.y		= D_CT_JPEG_YCC_ADDR_ERR;
		}
		else {
			dec_frm_mng.ycc_addr.y		= D_CT_JPEG_YCC_ADDR;
		}

		// CbCr
		dec_frm_mng.global_c_width = dec_param->g_c_width;
		dec_frm_mng.ycc_addr.c	= D_CT_JPEG_YCC_ADDR + (dec_param->g_y_width * dec_param->g_y_lines);
	}
	else {
		dec_frm_mng.cutout_flg		= D_IM_JPEG_ENABLE_OFF;
		dec_frm_mng.cutout_width	= 0;
		dec_frm_mng.cutout_lines	= 0;
		dec_frm_mng.cutout_offset_h	= 0;
		dec_frm_mng.cutout_offset_v	= 0;

		if( gJpeg_AXI_Err ) {
			dec_frm_mng.ycc_addr.y		= D_CT_JPEG_YCC_ADDR_ERR;
		}
		else {
			dec_frm_mng.ycc_addr.y		= D_CT_JPEG_YCC_ADDR;
		}

		// CbCr
		dec_frm_mng.global_c_width = dec_param->g_c_width;
		dec_frm_mng.ycc_addr.c	= D_CT_JPEG_YCC_ADDR + (dec_param->g_y_width * dec_param->g_y_lines);
	}
	dec_frm_mng.code_addr = D_CT_JPEG_CODE_ADDR;

//	ct_jpeg_measure_time_start();

	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	Ddim_Print(("Im_JPEG_Open ret=0x%X\n", ret));
	if ( ret != D_IM_JPEG_OK ) {
		return;
	}

	for( i=0; i < 10; i++ ) {

		if ( i == 0 ) {
			ret = Im_JPEG_Ctrl_Dec( &dec_mng );
			Ddim_Print(("Im_JPEG_Ctrl_Dec ret=0x%X\n", ret));
			ct_jpeg_ctrl_print( E_PRINT_CTRL_DEC_BASE, (VOID*)&dec_mng );
		}
		else {
			ret = Im_JPEG_Set_Skip_Marker_Dec();
			// target address change
			dec_frm_mng.code_addr = D_CT_JPEG_CODE_ADDR2;
		}
		if ( ret != D_IM_JPEG_OK ) {
			Im_JPEG_Close();
			return;
		}

		// limit size
		dec_frm_mng.code_size		= dec_param->limit_size;
		ret = Im_JPEG_Ctrl_Dec_Frame( &dec_frm_mng );
		Ddim_Print(("Im_JPEG_Ctrl_Dec_Frame ret=0x%X\n", ret));
		// Ctrl Print
		ct_jpeg_ctrl_print( E_PRINT_CTRL_DEC_FRAME, (VOID*)&dec_frm_mng );
		if ( ret != D_IM_JPEG_OK ) {
			Im_JPEG_Close();
			return;
		}

		if ( i == 0 ) {
			ret = Im_JPEG_Start_Dec();
			Ddim_Print(("Im_JPEG_Start_Dec ret=0x%X\n", ret));
		}
		else {
			ret = Im_JPEG_Start_Skip_Marker_Dec();
			Ddim_Print(("Im_JPEG_Start_Skip_Marker_Dec ret=0x%X\n", ret));
#if 0	// error root
			ret = Im_JPEG_Start_Skip_Marker_Dec();
			Ddim_Print(("Im_JPEG_Start_Skip_Marker_Dec ret=0x%X\n", ret));
#endif
		}
		if ( ret != D_IM_JPEG_OK ) {
			ct_jpeg_stop();
			Im_JPEG_Close();
			return;
		}
		ret = Im_JPEG_Wait_End_Dec( &dec_mng, 5000 );
		Ddim_Print(("Im_JPEG_Wait_End_Dec ret=0x%X\n", ret));

		memcpy(&gJpeg_Dec_Mng, &dec_mng, sizeof(T_IM_JPEG_DEC_MNG));

		if ( ret != D_IM_JPEG_OK ) {
			ct_jpeg_stop();
			Im_JPEG_Close();
			Ddim_Print(("***** Decode Error result=[0x%X] *****\n", gJpeg_Dec_Mng.result));
			Ddim_Print(("***** Decode Error Code=[0x%X] *****\n", gJpeg_Dec_Mng.err_code));
			Ddim_Print(("***** Decode Sampling type[%d] *****\n", gJpeg_Dec_Mng.smpl_type));
			return;
		}
//		ct_jpeg_measure_time_print();
		Ddim_Print(("***** Decode result=[0x%X] *****\n", gJpeg_Dec_Mng.result));
		Ddim_Print(("***** Decode Sampling type[%d] *****\n", gJpeg_Dec_Mng.smpl_type));
		Ddim_Print(("***** Decode Width[%d], Height[%d] *****\n", gJpeg_Dec_Mng.org_width, gJpeg_Dec_Mng.org_lines));
	}

	ret = Im_JPEG_Close();
	Ddim_Print(("Im_JPEG_Close ret=0x%X\n", ret));
	if ( ret != D_IM_JPEG_OK ) {
		return;
	}

	ct_jpeg_set_ycc_out_bytes( dec_mng.smpl_type, dec_mng.mem_format, dec_frm_mng.global_y_width, dec_frm_mng.global_c_width, gJpeg_Dec_Mng.org_lines );
}

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
static VOID ct_jpeg_decode_sync_palladium_in( const T_IM_JPEG_DEC_INPUT* const input, const ULONG lines )
{
	unsigned long in_data_siz_y;
	unsigned long in_data_siz_c;

	// Calc Y bytes.
	in_data_siz_y = ((unsigned long)input->global_y_width) * lines;

	switch( input->smpl_type ) {
		case E_IM_JPEG_SMPL_TYPE_YCC444:	// FALLTHROUGH
		case E_IM_JPEG_SMPL_TYPE_YCC422:
			in_data_siz_c = ((unsigned long)input->global_c_width) * lines;
			break;
//		case E_IM_JPEG_SMPL_TYPE_YCC420:
		default:
			in_data_siz_c = ((unsigned long)input->global_c_width) * (lines /2);
			break;
		case E_IM_JPEG_SMPL_TYPE_YCC400:
			in_data_siz_c = 0ul;
	}
	if( input->mem_format == E_IM_JPEG_MEM_FORM_PLANE ) {
		in_data_siz_c *= 2;
	}


	Ddim_Print(("%s(): Palladium_Set_In_Localstack( 0x%lx, %lu )\n", __func__, ((unsigned long)input->code_addr), in_data_siz_y + in_data_siz_c));
	Palladium_Set_In_Localstack( ((unsigned long)input->code_addr), in_data_siz_y + in_data_siz_c );
}

static VOID ct_jpeg_decode_sync_palladium_out( const T_IM_JPEG_DEC_INPUT* const input, const T_IM_JPEG_DEC_OUTPUT* const output )
{
	unsigned long out_data_siz_y;
	unsigned long out_data_siz_c;

	// Calc Y bytes.
	out_data_siz_y = ((unsigned long)input->global_y_width) * output->org_lines;

	switch( input->smpl_type ) {
		case E_IM_JPEG_SMPL_TYPE_YCC444:	// FALLTHROUGH
		case E_IM_JPEG_SMPL_TYPE_YCC422:
			out_data_siz_c = ((unsigned long)input->global_c_width) * output->org_lines;
			break;
//		case E_IM_JPEG_SMPL_TYPE_YCC420:
		default:
			out_data_siz_c = ((unsigned long)input->global_c_width) * (output->org_lines /2);
			break;
		case E_IM_JPEG_SMPL_TYPE_YCC400:
			out_data_siz_c = 0ul;
	}
	if( input->mem_format == E_IM_JPEG_MEM_FORM_PLANE ) {
		out_data_siz_c *= 2;
	}


	Ddim_Print(("%s(): Palladium_Set_Out_Localstack( 0x%lx, %lu )\n", __func__, ((unsigned long)input->dst_ycc_addr.y), out_data_siz_y + out_data_siz_c));
	Palladium_Set_Out_Localstack( ((unsigned long)input->dst_ycc_addr.y), out_data_siz_y + out_data_siz_c );
}
#endif

// ### REMOVE_RELEASE END
static VOID ct_im_jpeg_set_qtbl( UCHAR next )
{
	T_IM_JPEG_QUAT_TBL_PACK quant_tbl_pack;
	INT32 i;

#if 1
	quant_tbl_pack.quantization_tbl_0 = &gJpeg_Qtbl_Y;
	quant_tbl_pack.quantization_tbl_1 = &gJpeg_Qtbl_C;
	quant_tbl_pack.quantization_tbl_2 = &gJpeg_Qtbl_Y;
	quant_tbl_pack.quantization_tbl_3 = &gJpeg_Qtbl_C;
#else
	quant_tbl_pack.quantization_tbl_0 = NULL;
	quant_tbl_pack.quantization_tbl_1 = NULL;
	quant_tbl_pack.quantization_tbl_2 = NULL;
	quant_tbl_pack.quantization_tbl_3 = NULL;
#endif

	Im_JPEG_Set_QTbl( &quant_tbl_pack, next );

	if ( !next ) {
		ct_jpeg_start_hclock();
		for ( i=0; i < 16; i++ ) {
			Ddim_Print(("IO_JPG7.JPQT0[%d]=0x%lX\n", i, IO_JPG7.JPQT0.word[i]));
		}
		for ( i=0; i < 16; i++ ) {
			Ddim_Print(("IO_JPG7.JPQT1[%d]=0x%lX\n", i, IO_JPG7.JPQT1.word[i]));
		}
		for ( i=0; i < 16; i++ ) {
			Ddim_Print(("IO_JPG7.JPQT2[%d]=0x%lX\n", i, IO_JPG7.JPQT2.word[i]));
		}
		for ( i=0; i < 16; i++ ) {
			Ddim_Print(("IO_JPG7.JPQT3[%d]=0x%lX\n", i, IO_JPG7.JPQT3.word[i]));
		}
		ct_jpeg_stop_hclock();
	}
}

static VOID ct_im_jpeg_set_quality( UCHAR next )
{
	INT32 i;

	Im_JPEG_Set_Quality( 0, next );

	ct_jpeg_start_hclock();
		for ( i=0; i < 16; i++ ) {
			Ddim_Print(("IO_JPG7.JPQT0[%d]=0x%lX\n", i, IO_JPG7.JPQT0.word[i]));
		}
		for ( i=0; i < 16; i++ ) {
			Ddim_Print(("IO_JPG7.JPQT1[%d]=0x%lX\n", i, IO_JPG7.JPQT1.word[i]));
		}
		for ( i=0; i < 16; i++ ) {
			Ddim_Print(("IO_JPG7.JPQT2[%d]=0x%lX\n", i, IO_JPG7.JPQT2.word[i]));
		}
		for ( i=0; i < 16; i++ ) {
			Ddim_Print(("IO_JPG7.JPQT3[%d]=0x%lX\n", i, IO_JPG7.JPQT3.word[i]));
		}
	ct_jpeg_stop_hclock();
}

static VOID ct_im_jpeg_1_1_1( VOID )
{
#ifdef CO_DEBUG_ON_PC
	IO_JPG7.JSTATE.bit.JALLRSTP = 1;
#endif	// CO_DEBUG_ON_PC

	Im_JPEG_Init();

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("IO_JPG7.JPCMD.bit.JRESET=%d\n", IO_JPG7.JPCMD.bit.JRESET));
#endif	// CO_DEBUG_ON_PC
	return;
}

static VOID ct_im_jpeg_1_2_1( VOID )
{
	INT32 ret;
	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	Ddim_Print(("Im_JPEG_Open ret=0x%X\n", ret));
	return;
}

static VOID ct_im_jpeg_1_2_2( VOID )
{
	INT32 ret;

	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	Ddim_Print(("Im_JPEG_Open ret=0x%X\n", ret));

	ret = Im_JPEG_Close();
	Ddim_Print(("Im_JPEG_Close ret=0x%X\n", ret));

	return;
}

static VOID ct_im_jpeg_1_3_1( VOID )
{
	INT32 ret;
	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Open error ret=0x%X\n", ret));
		return;
	}

	ct_im_jpeg_set_qtbl( 0 );

	ret = Im_JPEG_Close();
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Close error ret=0x%X\n", ret));
		return;
	}
	return;
}

static VOID ct_im_jpeg_1_3_2( VOID )
{
	INT32 ret;
	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Open error ret=0x%X\n", ret));
		return;
	}

	ct_im_jpeg_set_qtbl( 1 );

	ret = Im_JPEG_Close();
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Close error ret=0x%X\n", ret));
		return;
	}
	return;
}

static VOID ct_im_jpeg_1_3_3( VOID )
{
	INT32 ret;
	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Open error ret=0x%X\n", ret));
		return;
	}

	ct_im_jpeg_set_quality( 0 );

	ret = Im_JPEG_Close();
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Close error ret=0x%X\n", ret));
		return;
	}
	return;
}

static VOID ct_im_jpeg_1_3_4( VOID )
{
	INT32 ret;
	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Open error ret=0x%X\n", ret));
		return;
	}

	ct_im_jpeg_set_quality( 1 );

	ret = Im_JPEG_Close();
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Close error ret=0x%X\n", ret));
		return;
	}
	return;
}

static VOID ct_im_jpeg_1_3_5( VOID )
{
	T_IM_JPEG_ENC_MNG jpeg_mng;
	USHORT ret_ratio;
	INT32 ret;

	jpeg_mng.ycc_smpl					= E_IM_JPEG_SMPL_TYPE_YCC422;
	jpeg_mng.mem_format					= E_IM_JPEG_MEM_FORM_PLANE_DOT;
	jpeg_mng.width						= 4608;
	jpeg_mng.lines						= 3456;
	jpeg_mng.component[0].quant_tbl_no	= 0;
	jpeg_mng.component[0].huf_dc_tbl_no	= 0;
	jpeg_mng.component[0].huf_ac_tbl_no	= 0;
	jpeg_mng.component[1].quant_tbl_no	= 1;
	jpeg_mng.component[1].huf_dc_tbl_no	= 1;
	jpeg_mng.component[1].huf_ac_tbl_no	= 1;
	jpeg_mng.component[2].quant_tbl_no	= 2;
	jpeg_mng.component[2].huf_dc_tbl_no	= 1;
	jpeg_mng.component[2].huf_ac_tbl_no	= 1;
	jpeg_mng.skip_mk_flg				= D_IM_JPEG_ENABLE_OFF;
	jpeg_mng.dri_mk_num					= 8;
	jpeg_mng.exif_fmt_flg				= D_IM_JPEG_ENABLE_OFF;
	jpeg_mng.pint_line					= 0;
	jpeg_mng.pint_sect					= 0;
	jpeg_mng.pbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	jpeg_mng.pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	jpeg_mng.pbuf_ctrl.cache_type		= 0;
	jpeg_mng.pbuf_ctrl.prot_type		= 0;
	jpeg_mng.jbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	jpeg_mng.jbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	jpeg_mng.jbuf_ctrl.cache_type		= 0;
	jpeg_mng.jbuf_ctrl.prot_type		= 0;
	jpeg_mng.jburst_length				= E_IM_JPEG_BURST_INCR_8;
	jpeg_mng.pburst_length				= E_IM_JPEG_BURST_INCR_8;
	jpeg_mng.code_size					= 0;
	jpeg_mng.result						= 0;
	jpeg_mng.pcallback 					= (VP_CALLBACK)ct_jpeg_encode_cb;

	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Open error ret=0x%X\n", ret));
		return;
	}

	Im_JPEG_Ctrl_Enc(&jpeg_mng);

	ret_ratio = Im_JPEG_Set_Down_Sampling_Rate( D_IM_JPEG_DOWNSP_1_2 );
	Ddim_Print(("Im_JPEG_Set_Down_Sampling_Rate=0x%X\n", ret_ratio));

	ct_jpeg_start_hclock();
	Ddim_Print(("IO_JPG7.JICTL.bit.JIXP=0x%X\n", IO_JPG7.JICTL.bit.JIXP));
	Ddim_Print(("IO_JPG7.JIMGSH.bit.JIMGSH=%d\n", IO_JPG7.JIMGSH.bit.JIMGSH));
	Ddim_Print(("IO_JPG7.JPWIDTH.bit.JPWIDTH=%d\n", IO_JPG7.JPWIDTH.bit.JPWIDTH));
	Ddim_Print(("IO_JPG7.JIMGSV.bit.JIMGSV=%d\n", IO_JPG7.JIMGSV.bit.JIMGSV));
	Ddim_Print(("IO_JPG7.JPHEIGHT.bit.JPHEIGHT=%d\n", IO_JPG7.JPHEIGHT.bit.JPHEIGHT));
	ct_jpeg_stop_hclock();

	ret = Im_JPEG_Close();
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Close error ret=0x%X\n", ret));
		return;
	}

	return;
}

static VOID ct_im_jpeg_1_3_6( VOID )
{
	T_IM_JPEG_ENC_MNG jpeg_mng;
	USHORT ret_ratio;
	INT32 ret;

	jpeg_mng.ycc_smpl					= E_IM_JPEG_SMPL_TYPE_YCC422;
	jpeg_mng.mem_format					= E_IM_JPEG_MEM_FORM_PLANE;
	jpeg_mng.width						= 4616;
	jpeg_mng.lines						= 3464;
	jpeg_mng.component[0].quant_tbl_no	= 0;
	jpeg_mng.component[0].huf_dc_tbl_no	= 0;
	jpeg_mng.component[0].huf_ac_tbl_no	= 0;
	jpeg_mng.component[1].quant_tbl_no	= 1;
	jpeg_mng.component[1].huf_dc_tbl_no	= 1;
	jpeg_mng.component[1].huf_ac_tbl_no	= 1;
	jpeg_mng.component[2].quant_tbl_no	= 2;
	jpeg_mng.component[2].huf_dc_tbl_no	= 1;
	jpeg_mng.component[2].huf_ac_tbl_no	= 1;
	jpeg_mng.skip_mk_flg				= D_IM_JPEG_ENABLE_OFF;
	jpeg_mng.dri_mk_num					= 8;
	jpeg_mng.exif_fmt_flg				= D_IM_JPEG_ENABLE_OFF;
	jpeg_mng.pint_line					= 0;
	jpeg_mng.pint_sect					= 0;
	jpeg_mng.pbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	jpeg_mng.pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	jpeg_mng.pbuf_ctrl.cache_type		= 0;
	jpeg_mng.pbuf_ctrl.prot_type		= 0;
	jpeg_mng.jbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
	jpeg_mng.jbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
	jpeg_mng.jbuf_ctrl.cache_type		= 0;
	jpeg_mng.jbuf_ctrl.prot_type		= 0;
	jpeg_mng.jburst_length				= E_IM_JPEG_BURST_INCR_8;
	jpeg_mng.pburst_length				= E_IM_JPEG_BURST_INCR_8;
	jpeg_mng.code_size					= 0;
	jpeg_mng.result						= 0;
	jpeg_mng.pcallback					= (VP_CALLBACK)ct_jpeg_decode_cb;

	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Open error ret=0x%X\n", ret));
		return;
	}

	Im_JPEG_Ctrl_Enc(&jpeg_mng);

	ret_ratio = Im_JPEG_Set_Down_Sampling_Rate( D_IM_JPEG_DOWNSP_1_2 );
	Ddim_Print(("Im_JPEG_Set_Down_Sampling_Rate=0x%X\n", ret_ratio));

	ct_jpeg_start_hclock();
	Ddim_Print(("IO_JPG7.JICTL.bit.JIXP=0x%X\n", IO_JPG7.JICTL.bit.JIXP));
	Ddim_Print(("IO_JPG7.JIMGSH.bit.JIMGSH=%d\n", IO_JPG7.JIMGSH.bit.JIMGSH));
	Ddim_Print(("IO_JPG7.JPWIDTH.bit.JPWIDTH=%d\n", IO_JPG7.JPWIDTH.bit.JPWIDTH));
	Ddim_Print(("IO_JPG7.JIMGSV.bit.JIMGSV=%d\n", IO_JPG7.JIMGSV.bit.JIMGSV));
	Ddim_Print(("IO_JPG7.JPHEIGHT.bit.JPHEIGHT=%d\n", IO_JPG7.JPHEIGHT.bit.JPHEIGHT));
	ct_jpeg_stop_hclock();

	ret = Im_JPEG_Close();
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Close error ret=0x%X\n", ret));
		return;
	}

	return;
}

static VOID ct_im_jpeg_1_3_7( VOID )
{
	T_IM_JPEG_ENC_MNG in_jpeg_mng;
	T_IM_JPEG_ENC_MNG out_jpeg_mng;
	T_IM_JPEG_ENC_FRAME_MNG in_jpeg_frm_mng;
	T_IM_JPEG_ENC_FRAME_MNG out_jpeg_frm_mng;
	INT32 ret;

	memset( &in_jpeg_mng, 0, sizeof(T_IM_JPEG_ENC_MNG) );
	memset( &out_jpeg_mng, 0, sizeof(T_IM_JPEG_ENC_MNG) );
	memset( &in_jpeg_frm_mng, 0, sizeof(T_IM_JPEG_ENC_FRAME_MNG) );
	memset( &out_jpeg_frm_mng, 0, sizeof(T_IM_JPEG_ENC_FRAME_MNG) );

	in_jpeg_mng.ycc_smpl					=E_IM_JPEG_SMPL_TYPE_YCC400;
	in_jpeg_mng.mem_format					=E_IM_JPEG_MEM_FORM_PLANE_DOT;
	in_jpeg_mng.width						=0xFFFF;
	in_jpeg_mng.lines						=0xFFFF;
	in_jpeg_mng.component[0].quant_tbl_no	=0;
	in_jpeg_mng.component[0].huf_dc_tbl_no	=0;
	in_jpeg_mng.component[0].huf_ac_tbl_no	=0;
	in_jpeg_mng.component[1].quant_tbl_no	=1;
	in_jpeg_mng.component[1].huf_dc_tbl_no	=1;
	in_jpeg_mng.component[1].huf_ac_tbl_no	=1;
	in_jpeg_mng.component[2].quant_tbl_no	=2;
	in_jpeg_mng.component[2].huf_dc_tbl_no	=0;
	in_jpeg_mng.component[2].huf_ac_tbl_no	=1;
	in_jpeg_mng.skip_mk_flg					=D_IM_JPEG_ENABLE_ON;
	in_jpeg_mng.dri_mk_num					=0xFFFF;
	in_jpeg_mng.exif_fmt_flg				=D_IM_JPEG_ENABLE_ON;
	in_jpeg_mng.pint_line					=0xFFFF;
	in_jpeg_mng.pint_sect					=0xFFFFFFFF;
	in_jpeg_mng.pbuf_ctrl.endian			=E_IM_JPEG_ENDIAN_BIG;
	in_jpeg_mng.pbuf_ctrl.issue_tran_num	=E_IM_JPEG_ISSUE_TRAN_8;
	in_jpeg_mng.pbuf_ctrl.cache_type		=D_IM_JPEG_ON_CACHE_RW_WRITE_BACK;
	in_jpeg_mng.pbuf_ctrl.prot_type			=D_IM_JPEG_I_NSEC_PRIVILEGED_ACCESS;
	in_jpeg_mng.jbuf_ctrl.endian			=E_IM_JPEG_ENDIAN_BIG;
	in_jpeg_mng.jbuf_ctrl.issue_tran_num	=E_IM_JPEG_ISSUE_TRAN_8;
	in_jpeg_mng.jbuf_ctrl.cache_type		=D_IM_JPEG_ON_CACHE_RW_WRITE_BACK;
	in_jpeg_mng.jbuf_ctrl.prot_type			=D_IM_JPEG_I_NSEC_PRIVILEGED_ACCESS;
	in_jpeg_mng.jburst_length				=E_IM_JPEG_BURST_INCR_8;
	in_jpeg_mng.pburst_length				=E_IM_JPEG_BURST_INCR_16;
	in_jpeg_mng.code_size					=0;
	in_jpeg_mng.result						=0;
	in_jpeg_mng.pcallback					=NULL;

	in_jpeg_frm_mng.global_y_width	= 0xFFFFF;
	in_jpeg_frm_mng.global_c_width	= 0xFFFFF;
	in_jpeg_frm_mng.ycc_addr.y		= 0xFFFFFFFF;
	in_jpeg_frm_mng.ycc_addr.c		= 0xFFFFFFFF;
	in_jpeg_frm_mng.code_addr		= 0xFFFFFFFF;
	in_jpeg_frm_mng.code_count_flg	= D_IM_JPEG_ENABLE_OFF;
	in_jpeg_frm_mng.limit_size		= 0x1FFFFFFFC00;

	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Open error ret=0x%X\n", ret));
		return;
	}

	ret = Im_JPEG_Ctrl_Enc(&in_jpeg_mng);
	Ddim_Print(("Im_JPEG_Ctrl_Enc ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Ctrl_Enc_Frame( &in_jpeg_frm_mng );
	Ddim_Print(("Im_JPEG_Ctrl_Enc_Frame ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ct_jpeg_start_hclock();
	Ddim_Print(("IO_JPG7.JPMODE.bit.S_ENDIAN    =0x%X\n", IO_JPG7.JPMODE.bit.S_ENDIAN));
	Ddim_Print(("IO_JPG7.JPMODE.bit.P_ENDIAN    =0x%X\n", IO_JPG7.JPMODE.bit.P_ENDIAN));
	Ddim_Print(("IO_JPG7.JPMODE.bit.DISPOSAL    =0x%X\n", IO_JPG7.JPMODE.bit.DISPOSAL));
	Ddim_Print(("IO_JPG7.JPMODE.bit.MMODE       =0x%X\n", IO_JPG7.JPMODE.bit.MMODE));
	Ddim_Print(("IO_JPG7.JPQTABLE.bit.QT0L      =0x%X\n", IO_JPG7.JPQTABLE.bit.QT0L));
	Ddim_Print(("IO_JPG7.JPQTABLE.bit.QT0U      =0x%X\n", IO_JPG7.JPQTABLE.bit.QT0U));
	Ddim_Print(("IO_JPG7.JPQTABLE.bit.QT1L      =0x%X\n", IO_JPG7.JPQTABLE.bit.QT1L));
	Ddim_Print(("IO_JPG7.JPQTABLE.bit.QT1U      =0x%X\n", IO_JPG7.JPQTABLE.bit.QT1U));
	Ddim_Print(("IO_JPG7.JPQTABLE.bit.QT2L      =0x%X\n", IO_JPG7.JPQTABLE.bit.QT2L));
	Ddim_Print(("IO_JPG7.JPQTABLE.bit.QT2U      =0x%X\n", IO_JPG7.JPQTABLE.bit.QT2U));
	Ddim_Print(("IO_JPG7.JPHTABLE.bit.HT0DC     =0x%X\n", IO_JPG7.JPHTABLE.bit.HT0DC));
	Ddim_Print(("IO_JPG7.JPHTABLE.bit.HT0AC     =0x%X\n", IO_JPG7.JPHTABLE.bit.HT0AC));
	Ddim_Print(("IO_JPG7.JPHTABLE.bit.HT1DC     =0x%X\n", IO_JPG7.JPHTABLE.bit.HT1DC));
	Ddim_Print(("IO_JPG7.JPHTABLE.bit.HT1AC     =0x%X\n", IO_JPG7.JPHTABLE.bit.HT1AC));
	Ddim_Print(("IO_JPG7.JPHTABLE.bit.HT2DC     =0x%X\n", IO_JPG7.JPHTABLE.bit.HT2DC));
	Ddim_Print(("IO_JPG7.JPHTABLE.bit.HT2AC     =0x%X\n", IO_JPG7.JPHTABLE.bit.HT2AC));
	Ddim_Print(("IO_JPG7.JPDRISET.word          =0x%lX\n", IO_JPG7.JPDRISET.word));
	Ddim_Print(("IO_JPG7.JPWIDTH.word           =0x%lX\n", IO_JPG7.JPWIDTH.word));
	Ddim_Print(("IO_JPG7.JPHEIGHT.word          =0x%lX\n", IO_JPG7.JPHEIGHT.word));
	Ddim_Print(("IO_JPG7.JPFORMAT.bit.CODECNT   =0x%X\n", IO_JPG7.JPFORMAT.bit.CODECNT));
	Ddim_Print(("IO_JPG7.JPFORMAT.bit.MKSKIP    =0x%X\n", IO_JPG7.JPFORMAT.bit.MKSKIP));
	Ddim_Print(("IO_JPG7.JPFORMAT.bit.EXIFMOD   =0x%X\n", IO_JPG7.JPFORMAT.bit.EXIFMOD));
	Ddim_Print(("IO_JPG7.JMODE.bit.JCSCCCEN     =0x%X\n", IO_JPG7.JMODE.bit.JCSCCCEN));
	Ddim_Print(("IO_JPG7.JMODE.bit.JPBDEXT      =0x%X\n", IO_JPG7.JMODE.bit.JPBDEXT));
	Ddim_Print(("IO_JPG7.JMODE.bit.JMEMFORM     =0x%X\n", IO_JPG7.JMODE.bit.JMEMFORM));
	Ddim_Print(("IO_JPG7.JMODE.bit.JPBENDIAN    =0x%X\n", IO_JPG7.JMODE.bit.JPBENDIAN));
	Ddim_Print(("IO_JPG7.JMODE.bit.JJBENDIAN    =0x%X\n", IO_JPG7.JMODE.bit.JJBENDIAN));
	Ddim_Print(("IO_JPG7.JPBSTA.bit.JPISSUESET  =0x%X\n", IO_JPG7.JPBSTA.bit.JPISSUESET));
	Ddim_Print(("IO_JPG7.JPBSTA.bit.JPACACHE    =0x%X\n", IO_JPG7.JPBSTA.bit.JPACACHE));
	Ddim_Print(("IO_JPG7.JPBSTA.bit.JPAPROT     =0x%X\n", IO_JPG7.JPBSTA.bit.JPAPROT));
	Ddim_Print(("IO_JPG7.JJBSTA.bit.JJISSUESET  =0x%X\n", IO_JPG7.JJBSTA.bit.JJISSUESET));
	Ddim_Print(("IO_JPG7.JJBSTA.bit.JJACACHE    =0x%X\n", IO_JPG7.JJBSTA.bit.JJACACHE));
	Ddim_Print(("IO_JPG7.JJBSTA.bit.JJAPROT     =0x%X\n", IO_JPG7.JJBSTA.bit.JJAPROT));
	Ddim_Print(("IO_JPG7.JCCTL.bit.JCRP         =0x%X\n", IO_JPG7.JCCTL.bit.JCRP));
	Ddim_Print(("IO_JPG7.JICTL.bit.JIRP         =0x%X\n", IO_JPG7.JICTL.bit.JIRP));
	Ddim_Print(("IO_JPG7.JINTEN.bit.JLINEEN     =0x%X\n", IO_JPG7.JINTEN.bit.JLINEEN));
	Ddim_Print(("IO_JPG7.JLINEVAL.word          =0x%lX\n", IO_JPG7.JLINEVAL.word));
	Ddim_Print(("IO_JPG7.JINTEN.bit.JSECTEN     =0x%X\n", IO_JPG7.JINTEN.bit.JSECTEN));
	Ddim_Print(("IO_JPG7.JSECTVAL.word          =0x%lX\n", IO_JPG7.JSECTVAL.word));
	Ddim_Print(("\n"));

	Ddim_Print(("IO_JPG7.JIMGYGH.bit.JIMGYGH    =0x%X\n", IO_JPG7.JIMGYGH.bit.JIMGYGH));
	Ddim_Print(("IO_JPG7.JIMGCGH.bit.JIMGCGH    =0x%X\n", IO_JPG7.JIMGCGH.bit.JIMGCGH));
	Ddim_Print(("IO_JPG7.JIMGSH.bit.JIMGSH      =0x%X\n", IO_JPG7.JIMGSH.bit.JIMGSH));
	Ddim_Print(("IO_JPG7.JIMGSV.bit.JIMGSV      =0x%X\n", IO_JPG7.JIMGSV.bit.JIMGSV));
	Ddim_Print(("IO_JPG7.JIMGAY.bit.JIMGAY      =0x%lX\n", IO_JPG7.JIMGAY.bit.JIMGAY));
	Ddim_Print(("IO_JPG7.JIMGACB.bit.JIMGACB    =0x%lX\n", IO_JPG7.JIMGACB.bit.JIMGACB));
	Ddim_Print(("IO_JPG7.JCCTL.bit.JCC          =0x%X\n", IO_JPG7.JCCTL.bit.JCC));
	Ddim_Print(("IO_JPG7.JCODA.bit.JCODA        =0x%lX\n", IO_JPG7.JCODA.bit.JCODA));
	Ddim_Print(("IO_JPG7.JCODSV                 =0x%lX\n", IO_JPG7.JCODSV.word));
	Ddim_Print(("\n"));
	ct_jpeg_stop_hclock();

	ret = Im_JPEG_Get_Ctrl_Enc(&out_jpeg_mng);
	Ddim_Print(("Im_JPEG_Get_Ctrl_Enc ret=0x%X\n", ret));

	Ddim_Print(("out_jpeg_mng.ycc_smpl=0x%X\n", out_jpeg_mng.ycc_smpl));
	Ddim_Print(("out_jpeg_mng.component[0].quant_tbl_no   =0x%X\n", out_jpeg_mng.component[0].quant_tbl_no));
	Ddim_Print(("out_jpeg_mng.component[1].quant_tbl_no   =0x%X\n", out_jpeg_mng.component[1].quant_tbl_no));
	Ddim_Print(("out_jpeg_mng.component[2].quant_tbl_no   =0x%X\n", out_jpeg_mng.component[2].quant_tbl_no));
	Ddim_Print(("out_jpeg_mng.component[0].huf_dc_tbl_no  =0x%X\n", out_jpeg_mng.component[0].huf_dc_tbl_no));
	Ddim_Print(("out_jpeg_mng.component[0].huf_ac_tbl_no  =0x%X\n", out_jpeg_mng.component[0].huf_ac_tbl_no));
	Ddim_Print(("out_jpeg_mng.component[1].huf_dc_tbl_no  =0x%X\n", out_jpeg_mng.component[1].huf_dc_tbl_no));
	Ddim_Print(("out_jpeg_mng.component[1].huf_ac_tbl_no  =0x%X\n", out_jpeg_mng.component[1].huf_ac_tbl_no));
	Ddim_Print(("out_jpeg_mng.component[2].huf_dc_tbl_no  =0x%X\n", out_jpeg_mng.component[2].huf_dc_tbl_no));
	Ddim_Print(("out_jpeg_mng.component[2].huf_ac_tbl_no  =0x%X\n", out_jpeg_mng.component[2].huf_ac_tbl_no));
	Ddim_Print(("out_jpeg_mng.dri_mk_num                  =0x%X\n", out_jpeg_mng.dri_mk_num));
	Ddim_Print(("out_jpeg_mng.width                       =0x%X\n", out_jpeg_mng.width));
	Ddim_Print(("out_jpeg_mng.lines                       =0x%X\n", out_jpeg_mng.lines));
	Ddim_Print(("out_jpeg_mng.skip_mk_flg                 =0x%X\n", out_jpeg_mng.skip_mk_flg));
	Ddim_Print(("out_jpeg_mng.exif_fmt_flg                =0x%X\n", out_jpeg_mng.exif_fmt_flg));
	Ddim_Print(("out_jpeg_mng.mem_format                  =0x%X\n", out_jpeg_mng.mem_format));
	Ddim_Print(("out_jpeg_mng.pbuf_ctrl.endian            =0x%X\n", out_jpeg_mng.pbuf_ctrl.endian));
	Ddim_Print(("out_jpeg_mng.jbuf_ctrl.endian            =0x%X\n", out_jpeg_mng.jbuf_ctrl.endian));
	Ddim_Print(("out_jpeg_mng.pbuf_ctrl.issue_tran_num    =0x%X\n", out_jpeg_mng.pbuf_ctrl.issue_tran_num));
	Ddim_Print(("out_jpeg_mng.pbuf_ctrl.cache_type        =0x%X\n", out_jpeg_mng.pbuf_ctrl.cache_type));
	Ddim_Print(("out_jpeg_mng.pbuf_ctrl.prot_type         =0x%X\n", out_jpeg_mng.pbuf_ctrl.prot_type));
	Ddim_Print(("out_jpeg_mng.jbuf_ctrl.issue_tran_num    =0x%X\n", out_jpeg_mng.jbuf_ctrl.issue_tran_num));
	Ddim_Print(("out_jpeg_mng.jbuf_ctrl.cache_type        =0x%X\n", out_jpeg_mng.jbuf_ctrl.cache_type));
	Ddim_Print(("out_jpeg_mng.jbuf_ctrl.prot_type         =0x%X\n", out_jpeg_mng.jbuf_ctrl.prot_type));
	Ddim_Print(("out_jpeg_mng.jburst_length               =0x%X\n", out_jpeg_mng.jburst_length));
	Ddim_Print(("out_jpeg_mng.pburst_length               =0x%X\n", out_jpeg_mng.pburst_length));
	Ddim_Print(("out_jpeg_mng.bit_depth                   =0x%X\n", out_jpeg_mng.bit_depth));
	Ddim_Print(("out_jpeg_mng.pint_line                   =0x%X\n", out_jpeg_mng.pint_line));
	Ddim_Print(("out_jpeg_mng.pint_sect                   =0x%lX\n", out_jpeg_mng.pint_sect));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Get_Ctrl_Enc_Frame(&out_jpeg_frm_mng);
	Ddim_Print(("Im_JPEG_Get_Ctrl_Enc_Frame ret=0x%X\n", ret));

	Ddim_Print(("out_jpeg_frm_mng.global_y_width          =0x%lX\n", out_jpeg_frm_mng.global_y_width));
	Ddim_Print(("out_jpeg_frm_mng.global_c_width          =0x%lX\n", out_jpeg_frm_mng.global_c_width));
	Ddim_Print(("out_jpeg_frm_mng.ycc_addr.y              =0x%lX\n", out_jpeg_frm_mng.ycc_addr.y));
	Ddim_Print(("out_jpeg_frm_mng.ycc_addr.c              =0x%lX\n", out_jpeg_frm_mng.ycc_addr.c));
	Ddim_Print(("out_jpeg_frm_mng.code_count_flg          =0x%X\n", out_jpeg_frm_mng.code_count_flg));
	Ddim_Print(("out_jpeg_frm_mng.code_addr               =0x%lX\n", out_jpeg_frm_mng.code_addr));
#if defined(CO_DEBUG_ON_PC) && defined(__MINGW32__)
	Ddim_Print(("out_jpeg_frm_mng.limit_size              =%I64X\n", out_jpeg_frm_mng.limit_size));
#else
	Ddim_Print(("out_jpeg_frm_mng.limit_size              =%llX\n", out_jpeg_frm_mng.limit_size));
#endif	// CO_DEBUG_ON_PC
	Ddim_Print(("\n"));

	ret = Im_JPEG_Close();
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Close error ret=0x%X\n", ret));
		return;
	}

	return;
}

static VOID ct_im_jpeg_1_3_8( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_444P;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_9( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_422P;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= (ULONG)(D_CT_JPEG_TEST_SIZE_H / 2);
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_10( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_422PD;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}


static VOID ct_im_jpeg_1_3_11( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_422P;
	enc_param.g_y_width		= (ULONG)((D_CT_JPEG_TEST_SIZE_H << 1) << 4);
	enc_param.g_y_lines		= 0;
	enc_param.g_c_width		= 0;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_12( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_420P;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= (ULONG)(D_CT_JPEG_TEST_SIZE_H / 2);
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_13( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 0;
	enc_param.format		= E_CT_JPEG_FORMAT_420PD;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_14( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 0;
	enc_param.format		= E_CT_JPEG_FORMAT_400P;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= 0;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_15( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_RGBP;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_16( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_422PD;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 0;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_17( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_420PD;
	enc_param.g_y_width		= D_CT_JPEG_SIZE_3M_H;
	enc_param.g_y_lines		= D_CT_JPEG_SIZE_3M_V;
	enc_param.g_c_width		= D_CT_JPEG_SIZE_3M_H;
	enc_param.h_size		= D_CT_JPEG_SIZE_VGA_H;
	enc_param.v_size		= D_CT_JPEG_SIZE_VGA_V;
	enc_param.limit_size	= D_CT_JPEG_SIZE_VGA_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 0;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_18( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_422PD;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 1;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_19( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_422PD;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 1;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_1_2;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_20( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_422PD;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_CODE;
	enc_param.pause_flg		= 1;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );
	return;
}

static VOID ct_im_jpeg_1_3_21( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;
	ULONG	sect_cnt;
	USHORT	line_cnt;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_422PD;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode( &enc_param, 1 );

	line_cnt = Im_JPEG_Get_Line_Cnt();
	Ddim_Print(("Im_JPEG_Get_Line_Cnt ret=0x%X\n", line_cnt));

	sect_cnt = Im_JPEG_Get_Sect_Cnt();
	Ddim_Print(("Im_JPEG_Get_Sect_Cnt ret=0x%lX\n", sect_cnt));
	
	return;
}

static VOID ct_im_jpeg_1_4_2( VOID )
{
	T_IM_JPEG_DEC_MNG in_jpeg_mng;
	T_IM_JPEG_DEC_MNG out_jpeg_mng;
	T_IM_JPEG_DEC_FRAME_MNG in_jpeg_frm_mng;
	T_IM_JPEG_DEC_FRAME_MNG out_jpeg_frm_mng;
	INT32 ret;

	memset( &in_jpeg_mng, 0, sizeof(T_IM_JPEG_DEC_MNG) );
	memset( &out_jpeg_mng, 0, sizeof(T_IM_JPEG_DEC_MNG) );
	memset( &in_jpeg_frm_mng, 0, sizeof(T_IM_JPEG_DEC_FRAME_MNG) );
	memset( &out_jpeg_frm_mng, 0, sizeof(T_IM_JPEG_DEC_FRAME_MNG) );

	in_jpeg_mng.mem_format					=E_IM_JPEG_MEM_FORM_PLANE_DOT;
	in_jpeg_mng.ext_mode					=E_IM_JPEG_RESIZE_EXT_BAND;
	in_jpeg_mng.corr_mode					=D_IM_JPEG_ENABLE_ON;
	in_jpeg_mng.skip_mk_flg					=D_IM_JPEG_ENABLE_ON;
	in_jpeg_mng.pint_line					=0xFFFF;
	in_jpeg_mng.pint_sect					=0x11111111;
	in_jpeg_mng.color_band.y_band			=0xFF;
	in_jpeg_mng.color_band.cb_band			=0x11;
	in_jpeg_mng.color_band.cr_band			=0x22;
	in_jpeg_mng.pbuf_ctrl.endian			=E_IM_JPEG_ENDIAN_BIG;
	in_jpeg_mng.pbuf_ctrl.issue_tran_num	=E_IM_JPEG_ISSUE_TRAN_8;
	in_jpeg_mng.pbuf_ctrl.cache_type		=D_IM_JPEG_ON_CACHE_RW_WRITE_BACK;
	in_jpeg_mng.pbuf_ctrl.prot_type			=D_IM_JPEG_I_NSEC_PRIVILEGED_ACCESS;
	in_jpeg_mng.jbuf_ctrl.endian			=E_IM_JPEG_ENDIAN_BIG;
	in_jpeg_mng.jbuf_ctrl.issue_tran_num	=E_IM_JPEG_ISSUE_TRAN_8;
	in_jpeg_mng.jbuf_ctrl.cache_type		=D_IM_JPEG_ON_CACHE_RW_WRITE_BACK;
	in_jpeg_mng.jbuf_ctrl.prot_type			=D_IM_JPEG_I_NSEC_PRIVILEGED_ACCESS;
	in_jpeg_mng.jburst_length				=E_IM_JPEG_BURST_INCR_8;
	in_jpeg_mng.pburst_length				=E_IM_JPEG_BURST_INCR_16;
	in_jpeg_mng.smpl_type					=E_IM_JPEG_SMPL_TYPE_YCC422;
	in_jpeg_mng.org_width					=0;
	in_jpeg_mng.org_lines					=0;
	in_jpeg_mng.result						=0;
	in_jpeg_mng.err_code					=0;
	in_jpeg_mng.pcallback					=NULL;

	in_jpeg_frm_mng.global_y_width	= 0xFFFFF;
	in_jpeg_frm_mng.global_c_width	= 0x99999;
	in_jpeg_frm_mng.cutout_flg		= D_IM_JPEG_ENABLE_ON;
	in_jpeg_frm_mng.cutout_width	= 0xFFFF;
	in_jpeg_frm_mng.cutout_lines	= 0x0001;
	in_jpeg_frm_mng.cutout_offset_h	= 0x1234;
	in_jpeg_frm_mng.cutout_offset_v	= 0x4321;
	in_jpeg_frm_mng.ycc_addr.y		= 0xFFFFFFFF;
	in_jpeg_frm_mng.ycc_addr.c		= 0x00000001;
	in_jpeg_frm_mng.code_addr		= 0x22222222;
	in_jpeg_frm_mng.code_size		= 0x1FFFFFFFC00;

	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Open error ret=0x%X\n", ret));
		return;
	}

	ret = Im_JPEG_Ctrl_Dec(&in_jpeg_mng);
	Ddim_Print(("Im_JPEG_Ctrl_Dec ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Ctrl_Dec_Frame( &in_jpeg_frm_mng );
	Ddim_Print(("Im_JPEG_Ctrl_Dec_Frame ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ct_jpeg_start_hclock();
	Ddim_Print(("IO_JPG7.JPMODE.bit.S_ENDIAN       =0x%X\n", IO_JPG7.JPMODE.bit.S_ENDIAN));
	Ddim_Print(("IO_JPG7.JPMODE.bit.P_ENDIAN       =0x%X\n", IO_JPG7.JPMODE.bit.P_ENDIAN));
	Ddim_Print(("IO_JPG7.JPMODE.bit.DISPOSAL       =0x%X\n", IO_JPG7.JPMODE.bit.DISPOSAL));
	Ddim_Print(("IO_JPG7.JPMODE.bit.MMODE          =0x%X\n", IO_JPG7.JPMODE.bit.MMODE));
	Ddim_Print(("IO_JPG7.JPFORMAT.bit.CODECNT      =0x%X\n", IO_JPG7.JPFORMAT.bit.CODECNT));
	Ddim_Print(("IO_JPG7.JPFORMAT.bit.MKSKIP       =0x%X\n", IO_JPG7.JPFORMAT.bit.MKSKIP));
	Ddim_Print(("IO_JPG7.JPFORMAT.bit.EXIFMOD      =0x%X\n", IO_JPG7.JPFORMAT.bit.EXIFMOD));
	Ddim_Print(("IO_JPG7.JPWIDTH.bit.JPWIDTH       =%d\n", IO_JPG7.JPWIDTH.bit.JPWIDTH));
	Ddim_Print(("IO_JPG7.JPHEIGHT.bit.JPHEIGHT     =%d\n", IO_JPG7.JPHEIGHT.bit.JPHEIGHT));
	Ddim_Print(("IO_JPG7.JMODE.bit.JPBDEXT         =0x%X\n", IO_JPG7.JMODE.bit.JPBDEXT));
	Ddim_Print(("IO_JPG7.JMODE.bit.JMEMFORM        =0x%X\n", IO_JPG7.JMODE.bit.JMEMFORM));
	Ddim_Print(("IO_JPG7.JMODE.bit.JPBENDIAN       =0x%X\n", IO_JPG7.JMODE.bit.JPBENDIAN));
	Ddim_Print(("IO_JPG7.JMODE.bit.JJBENDIAN       =0x%X\n", IO_JPG7.JMODE.bit.JJBENDIAN));
	Ddim_Print(("IO_JPG7.JPBSTA.bit.JPISSUESET     =0x%X\n", IO_JPG7.JPBSTA.bit.JPISSUESET));
	Ddim_Print(("IO_JPG7.JPBSTA.bit.JPACACHE       =0x%X\n", IO_JPG7.JPBSTA.bit.JPACACHE));
	Ddim_Print(("IO_JPG7.JPBSTA.bit.JPAPROT        =0x%X\n", IO_JPG7.JPBSTA.bit.JPAPROT));
	Ddim_Print(("IO_JPG7.JJBSTA.bit.JJISSUESET     =0x%X\n", IO_JPG7.JJBSTA.bit.JJISSUESET));
	Ddim_Print(("IO_JPG7.JJBSTA.bit.JJACACHE       =0x%X\n", IO_JPG7.JJBSTA.bit.JJACACHE));
	Ddim_Print(("IO_JPG7.JJBSTA.bit.JJAPROT        =0x%X\n", IO_JPG7.JJBSTA.bit.JJAPROT));
	Ddim_Print(("IO_JPG7.JCCTL.bit.JCRP            =0x%X\n", IO_JPG7.JCCTL.bit.JCRP));
	Ddim_Print(("IO_JPG7.JICTL.bit.JIRP            =0x%X\n", IO_JPG7.JICTL.bit.JIRP));
	Ddim_Print(("IO_JPG7.JINTEN.bit.JLINEEN        =0x%X\n", IO_JPG7.JINTEN.bit.JLINEEN));
	Ddim_Print(("IO_JPG7.JLINEVAL.word             =0x%lX\n", IO_JPG7.JLINEVAL.word));
	Ddim_Print(("IO_JPG7.JINTEN.bit.JSECTEN        =0x%X\n", IO_JPG7.JINTEN.bit.JSECTEN));
	Ddim_Print(("IO_JPG7.JSECTVAL.word             =0x%lX\n", IO_JPG7.JSECTVAL.word));
	Ddim_Print(("IO_JPG7.JBANDCOLOR.bit.JBANDY     =0x%X\n", IO_JPG7.JBANDCOLOR.bit.JBANDY));
	Ddim_Print(("IO_JPG7.JBANDCOLOR.bit.JBANDCB    =0x%X\n", IO_JPG7.JBANDCOLOR.bit.JBANDCB));
	Ddim_Print(("IO_JPG7.JBANDCOLOR.bit.JBANDCR    =0x%X\n", IO_JPG7.JBANDCOLOR.bit.JBANDCR));
	Ddim_Print(("\n"));

	Ddim_Print(("IO_JPG7.JICTL.bit.JIXP            =0x%X\n", IO_JPG7.JICTL.bit.JIXP));
	Ddim_Print(("IO_JPG7.JCCTL.bit.JCC             =0x%X\n", IO_JPG7.JCCTL.bit.JCC));
	Ddim_Print(("IO_JPG7.JIMGAY.bit.JIMGAY         =0x%lX\n", IO_JPG7.JIMGAY.bit.JIMGAY));
	Ddim_Print(("IO_JPG7.JIMGACB.bit.JIMGACB       =0x%lX\n", IO_JPG7.JIMGACB.bit.JIMGACB));
	Ddim_Print(("IO_JPG7.JCODA.bit.JCODA           =0x%lX\n", IO_JPG7.JCODA.bit.JCODA));
	Ddim_Print(("IO_JPG7.JCODSV.word               =0x%lX\n", IO_JPG7.JCODSV.word));
	Ddim_Print(("\n"));
	ct_jpeg_stop_hclock();

	ret = Im_JPEG_Get_Ctrl_Dec(&out_jpeg_mng);
	Ddim_Print(("Im_JPEG_Get_Ctrl_Dec ret=0x%X\n", ret));

	Ddim_Print(("out_jpeg_mng.skip_mk_flg              =0x%X\n", out_jpeg_mng.skip_mk_flg));
	Ddim_Print(("out_jpeg_mng.mem_format               =0x%X\n", out_jpeg_mng.mem_format));
	Ddim_Print(("out_jpeg_mng.ext_mode                 =0x%X\n", out_jpeg_mng.ext_mode));
	Ddim_Print(("out_jpeg_mng.corr_mode                =0x%X\n", out_jpeg_mng.corr_mode));
	Ddim_Print(("out_jpeg_mng.color_band.y_band        =0x%X\n", out_jpeg_mng.color_band.y_band));
	Ddim_Print(("out_jpeg_mng.color_band.cb_band       =0x%X\n", out_jpeg_mng.color_band.cb_band));
	Ddim_Print(("out_jpeg_mng.color_band.cr_band       =0x%X\n", out_jpeg_mng.color_band.cr_band));
	Ddim_Print(("out_jpeg_mng.pbuf_ctrl.endian         =0x%X\n", out_jpeg_mng.pbuf_ctrl.endian));
	Ddim_Print(("out_jpeg_mng.jbuf_ctrl.endian         =0x%X\n", out_jpeg_mng.jbuf_ctrl.endian));
	Ddim_Print(("out_jpeg_mng.pbuf_ctrl.issue_tran_num =0x%X\n", out_jpeg_mng.pbuf_ctrl.issue_tran_num));
	Ddim_Print(("out_jpeg_mng.pbuf_ctrl.cache_type     =0x%X\n", out_jpeg_mng.pbuf_ctrl.cache_type));
	Ddim_Print(("out_jpeg_mng.pbuf_ctrl.prot_type      =0x%X\n", out_jpeg_mng.pbuf_ctrl.prot_type));
	Ddim_Print(("out_jpeg_mng.jbuf_ctrl.issue_tran_num =0x%X\n", out_jpeg_mng.jbuf_ctrl.issue_tran_num));
	Ddim_Print(("out_jpeg_mng.jbuf_ctrl.cache_type     =0x%X\n", out_jpeg_mng.jbuf_ctrl.cache_type));
	Ddim_Print(("out_jpeg_mng.jbuf_ctrl.prot_type      =0x%X\n", out_jpeg_mng.jbuf_ctrl.prot_type));
	Ddim_Print(("out_jpeg_mng.jburst_length            =0x%X\n", out_jpeg_mng.jburst_length));
	Ddim_Print(("out_jpeg_mng.pburst_length            =0x%X\n", out_jpeg_mng.pburst_length));
	Ddim_Print(("out_jpeg_mng.pint_line                =0x%X\n", out_jpeg_mng.pint_line));
	Ddim_Print(("out_jpeg_mng.pint_sect                =0x%lX\n", out_jpeg_mng.pint_sect));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Get_Ctrl_Dec_Frame(&out_jpeg_frm_mng);
	Ddim_Print(("Im_JPEG_Get_Ctrl_Dec_Frame ret=0x%X\n", ret));

	Ddim_Print(("out_jpeg_frm_mng.ycc_addr.y          =0x%lX\n", out_jpeg_frm_mng.ycc_addr.y));
	Ddim_Print(("out_jpeg_frm_mng.ycc_addr.c          =0x%lX\n", out_jpeg_frm_mng.ycc_addr.c));
	Ddim_Print(("out_jpeg_frm_mng.code_addr           =0x%lX\n", out_jpeg_frm_mng.code_addr));
#if defined(CO_DEBUG_ON_PC) && defined(__MINGW__)
	Ddim_Print(("out_jpeg_frm_mng.code_size           =0x%I64X\n", out_jpeg_frm_mng.code_size));
#else
	Ddim_Print(("out_jpeg_frm_mng.code_size           =0x%llX\n", out_jpeg_frm_mng.code_size));
#endif	// CO_DEBUG_ON_PC
	Ddim_Print(("\n"));

	ret = Im_JPEG_Close();
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Close error ret=0x%X\n", ret));
		return;
	}

	return;
}

static VOID ct_im_jpeg_1_4_3( VOID )
{
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 1;
	dec_param.format		= E_CT_JPEG_FORMAT_444P;
	dec_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.cutout_flg	= 0;
	dec_param.cut_h_size	= 0;
	dec_param.cut_v_size	= 0;
	dec_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 0;
	dec_param.pause_flg		= 0;

	ct_jpeg_decode( &dec_param );
	return;
}

static VOID ct_im_jpeg_1_4_4( VOID )
{
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 1;
	dec_param.format		= E_CT_JPEG_FORMAT_422P;
	dec_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.g_c_width		= (ULONG)(D_CT_JPEG_TEST_SIZE_H / 2);
	dec_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.cutout_flg	= 0;
	dec_param.cut_h_size	= 0;
	dec_param.cut_v_size	= 0;
	dec_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 0;
	dec_param.pause_flg		= 0;

	ct_jpeg_decode( &dec_param );
	return;
}

static VOID ct_im_jpeg_1_4_5( VOID )
{
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 1;
	dec_param.format		= E_CT_JPEG_FORMAT_422PD;
	dec_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.cutout_flg	= 0;
	dec_param.cut_h_size	= 0;
	dec_param.cut_v_size	= 0;
	dec_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 0;
	dec_param.pause_flg		= 0;

	ct_jpeg_decode( &dec_param );
	return;
}

static VOID ct_im_jpeg_1_4_6( VOID )
{
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 0;
	dec_param.format		= E_CT_JPEG_FORMAT_420P;
	dec_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.g_c_width		= (ULONG)(D_CT_JPEG_TEST_SIZE_H / 2);
	dec_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.cutout_flg	= 0;
	dec_param.cut_h_size	= 0;
	dec_param.cut_v_size	= 0;
	dec_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 0;
	dec_param.pause_flg		= 0;

	ct_jpeg_decode( &dec_param );
	return;
}

static VOID ct_im_jpeg_1_4_7( VOID )
{
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 0;
	dec_param.format		= E_CT_JPEG_FORMAT_420PD;
	dec_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.cutout_flg	= 0;
	dec_param.cut_h_size	= 0;
	dec_param.cut_v_size	= 0;
	dec_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 0;
	dec_param.pause_flg		= 0;

	ct_jpeg_decode( &dec_param );
	return;
}

static VOID ct_im_jpeg_1_4_8( VOID )
{
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 1;
	dec_param.format		= E_CT_JPEG_FORMAT_400P;
	dec_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.g_c_width		= 0;
	dec_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.cutout_flg	= 0;
	dec_param.cut_h_size	= 0;
	dec_param.cut_v_size	= 0;
	dec_param.limit_size	= 0;
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 0;
	dec_param.pause_flg		= 0;

	ct_jpeg_decode( &dec_param );
	return;
}

static VOID ct_im_jpeg_1_4_9( VOID )
{
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 1;
	dec_param.format		= E_CT_JPEG_FORMAT_422PD;
	dec_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.cutout_flg	= 0;
	dec_param.cut_h_size	= 0;
	dec_param.cut_v_size	= 0;
	dec_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 1;
	dec_param.pause_flg		= 0;

	ct_jpeg_decode( &dec_param );
	return;
}

static VOID ct_im_jpeg_1_4_10( VOID )
{
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 1;
	dec_param.format		= E_CT_JPEG_FORMAT_422PD;
	dec_param.g_y_width		= D_CT_JPEG_SIZE_VGA_H;
	dec_param.g_y_lines		= D_CT_JPEG_SIZE_VGA_V;
	dec_param.g_c_width		= D_CT_JPEG_SIZE_VGA_H;
	dec_param.h_size		= D_CT_JPEG_SIZE_3M_H;
	dec_param.v_size		= D_CT_JPEG_SIZE_3M_V;
	dec_param.cutout_flg	= 1;
	dec_param.cut_h_size	= D_CT_JPEG_SIZE_VGA_H;
	dec_param.cut_v_size	= D_CT_JPEG_SIZE_VGA_V;
	dec_param.limit_size	= D_CT_JPEG_SIZE_3M_LIMIT;
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 0;
	dec_param.pause_flg		= 0;

	ct_jpeg_decode( &dec_param );
	return;
}

static VOID ct_im_jpeg_1_4_11( VOID )
{
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 1;
	dec_param.format		= E_CT_JPEG_FORMAT_422PD;
	dec_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.cutout_flg	= 0;
	dec_param.cut_h_size	= 0;
	dec_param.cut_v_size	= 0;
	dec_param.limit_size	= D_CT_JPEG_TEST_CODE;
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 0;
	dec_param.pause_flg		= 1;

	ct_jpeg_decode( &dec_param );
	return;
}

static VOID ct_im_jpeg_1_4_13( VOID )
{
	ULONG	sect_cnt;
	USHORT	line_cnt;
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 1;
	dec_param.format		= E_CT_JPEG_FORMAT_422PD;
	dec_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.cutout_flg	= 0;
	dec_param.cut_h_size	= 0;
	dec_param.cut_v_size	= 0;
	dec_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 0;
	dec_param.pause_flg		= 0;

	ct_jpeg_decode( &dec_param );

	line_cnt = Im_JPEG_Get_Line_Cnt();
	Ddim_Print(("Im_JPEG_Get_Line_Cnt ret=0x%X\n", line_cnt));

	sect_cnt = Im_JPEG_Get_Sect_Cnt();
	Ddim_Print(("Im_JPEG_Get_Sect_Cnt ret=0x%lX\n", sect_cnt));

	return;
}

static VOID ct_im_jpeg_1_4_14( VOID )
{
	T_IM_JPEG_DEC_INPUT		input;
	T_IM_JPEG_DEC_OUTPUT	output;
	INT32	ret;

	memset(&input, 0, sizeof(T_IM_JPEG_DEC_INPUT));
	memset(&output, 0, sizeof(T_IM_JPEG_DEC_OUTPUT));

	input.global_y_width	= D_CT_JPEG_TEST_SIZE_H;
	input.global_c_width	= D_CT_JPEG_TEST_SIZE_H;
	input.mem_format		= E_IM_JPEG_MEM_FORM_PLANE_DOT;
	input.smpl_type			= E_IM_JPEG_SMPL_TYPE_YCC422;
	input.code_size			= D_CT_JPEG_TEST_SIZE_LIMIT;
	input.code_addr			= D_CT_JPEG_CODE_ADDR;
	input.dst_ycc_addr.y	= D_CT_JPEG_YCC_ADDR;
	input.dst_ycc_addr.c	= D_CT_JPEG_YCC_ADDR + (D_CT_JPEG_TEST_SIZE_H * D_CT_JPEG_TEST_SIZE_V);

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
	ct_jpeg_decode_sync_palladium_in( &input, D_CT_JPEG_TEST_SIZE_V );
	IO_JPG7.JPWIDTH.bit.JPWIDTH		= input.global_y_width;
	IO_JPG7.JPHEIGHT.bit.JPHEIGHT	= D_CT_JPEG_TEST_SIZE_V;
#endif

// ### REMOVE_RELEASE END
	ret = Im_JPEG_Decode_Sync( &input, &output );
	Ddim_Print(("Im_JPEG_Decode_Sync ret=0x%X\n", ret));

	Ddim_Print(("output.smpl_type =0x%X\n", output.smpl_type));
	Ddim_Print(("output.org_width =0x%X\n", output.org_width));
	Ddim_Print(("output.org_lines =0x%X\n", output.org_lines));
	Ddim_Print(("output.err_code  =0x%X\n", output.err_code));

	
	ct_jpeg_set_ycc_out_bytes( output.smpl_type, input.mem_format, input.global_y_width, input.global_c_width, output.org_lines );

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
	if( ret == D_IM_JPEG_OK ) {
		ct_jpeg_decode_sync_palladium_out( &input, &output );
	}
#endif

// ### REMOVE_RELEASE END
	return;
}

static VOID ct_im_jpeg_1_5_1( VOID )
{
	T_CT_JPEG_ENC_PARAM enc_param;

	enc_param.sync			= 1;
	enc_param.format		= E_CT_JPEG_FORMAT_422PD;
	enc_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	enc_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	enc_param.limit_size	= D_CT_JPEG_TEST_SIZE_LIMIT;
	enc_param.pause_flg		= 0;
	enc_param.count_flg		= 0;
	enc_param.quantup_flg	= 0;
	enc_param.skip_mk_flg	= D_IM_JPEG_ENABLE_OFF;
	enc_param.dri			= 1;
	enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;

	ct_jpeg_encode_force_stop( &enc_param );
	return;
}

static VOID ct_im_jpeg_1_5_2( VOID )
{
	INT32 ret;
	E_IM_JPEG_AXI_ST status;

	ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Open error ret=0x%X\n", ret));
		return;
	}

#ifdef CO_DEBUG_ON_PC
	IO_JPG7.JSTATE.bit.JALLACTP = 1;
	IO_JPG7.JSTATE.bit.JPBACTP = 1;
#endif	// CO_DEBUG_ON_PC

	ret = Im_JPEG_Get_Axi_State( &status );
	Ddim_Print(("Im_JPEG_Get_Axi_State ret=0x%X\n", ret));
	Ddim_Print(("status=0x%X\n", status));

	ct_jpeg_start_hclock();
	Ddim_Print(("IO_JPG7.JPSTATUS.bit.JPSTATUS=0x%X\n", IO_JPG7.JPSTATUS.bit.JPSTATUS));
	ct_jpeg_stop_hclock();

	ret = Im_JPEG_Close();
	if ( ret != D_IM_JPEG_OK ) {
		Ddim_Print(("Im_JPEG_Close error ret=0x%X\n", ret));
		return;
	}

	return;
}

static VOID ct_im_jpeg_2_1_1( VOID )
{
	INT32 ret;
	ret = Im_JPEG_Open( 100 );
	Ddim_Print(("Im_JPEG_Open ret=0x%X\n", ret));

	ret = Im_JPEG_Open( 20 );
	Ddim_Print(("Im_JPEG_Open ret=0x%X\n", ret));

	return;
}

static VOID ct_im_jpeg_2_2_1( VOID )
{
	INT32 ret;

	ret = Im_JPEG_Open( -2 );
	Ddim_Print(("Im_JPEG_Open ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Set_Quality( 0x40000, 0 );
	Ddim_Print(("Im_JPEG_Set_Quality ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Set_Down_Sampling_Rate( 4 );
	Ddim_Print(("Im_JPEG_Set_Down_Sampling_Rate ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Ctrl_Enc( NULL );
	Ddim_Print(("Im_JPEG_Ctrl_Enc ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Ctrl_Enc_Frame( NULL );
	Ddim_Print(("Im_JPEG_Ctrl_Enc_Frame ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Get_Ctrl_Enc( NULL );
	Ddim_Print(("Im_JPEG_Get_Ctrl_Enc ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Get_Ctrl_Enc_Frame( NULL );
	Ddim_Print(("Im_JPEG_Get_Ctrl_Enc_Frame ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Wait_End_Enc( NULL , 100 );
	Ddim_Print(("Im_JPEG_Wait_End_Enc ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Restart_Enc( NULL );
	Ddim_Print(("Im_JPEG_Restart_Enc ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Ctrl_Dec( NULL );
	Ddim_Print(("Im_JPEG_Ctrl_Dec ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Ctrl_Dec_Frame( NULL );
	Ddim_Print(("Im_JPEG_Ctrl_Dec_Frame ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Get_Ctrl_Dec( NULL );
	Ddim_Print(("Im_JPEG_Get_Ctrl_Dec ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Get_Ctrl_Dec_Frame( NULL );
	Ddim_Print(("Im_JPEG_Get_Ctrl_Dec_Frame ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Wait_End_Dec( NULL , 100 );
	Ddim_Print(("Im_JPEG_Wait_End_Dec ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Get_Axi_State( NULL );
	Ddim_Print(("Im_JPEG_Get_Axi_State ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Restart_Dec( NULL );
	Ddim_Print(("Im_JPEG_Restart_Dec ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	ret = Im_JPEG_Decode_Sync( NULL, NULL );
	Ddim_Print(("Im_JPEG_Decode_Sync ret=0x%X\n", ret));
	Ddim_Print(("\n"));

	return;
}

static VOID ct_im_jpeg_2_3_1( VOID )
{
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 1;
	dec_param.format		= E_CT_JPEG_FORMAT_420PD;
	dec_param.g_y_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.g_y_lines		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.g_c_width		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.h_size		= D_CT_JPEG_TEST_SIZE_H;
	dec_param.v_size		= D_CT_JPEG_TEST_SIZE_V;
	dec_param.cutout_flg	= 0;
	dec_param.cut_h_size	= 0;
	dec_param.cut_v_size	= 0;
	dec_param.limit_size	= D_CT_JPEG_TEST_SIZE_H * D_CT_JPEG_TEST_SIZE_V * 2;	// Input jpeg file compression ratio 200% are supposed.
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 0;
	dec_param.pause_flg		= 0;

	ct_jpeg_decode( &dec_param );
	return;
}

static VOID ct_im_jpeg_2_3_2( VOID )
{
	T_CT_JPEG_DEC_PARAM dec_param;

	dec_param.sync			= 1;
	dec_param.format		= E_CT_JPEG_FORMAT_420PD;
	dec_param.g_y_width		= D_CT_JPEG_SIZE_3M_H;
	dec_param.g_y_lines		= D_CT_JPEG_SIZE_3M_V;
	dec_param.g_c_width		= D_CT_JPEG_SIZE_3M_H;
	dec_param.h_size		= D_CT_JPEG_SIZE_3M_H;
	dec_param.v_size		= D_CT_JPEG_SIZE_3M_V;
	dec_param.cutout_flg	= 1;
	dec_param.cut_h_size	= D_CT_JPEG_SIZE_VGA_H;
	dec_param.cut_v_size	= D_CT_JPEG_SIZE_VGA_V;
	dec_param.limit_size	= D_CT_JPEG_SIZE_3M_H * D_CT_JPEG_SIZE_3M_V * 2;	// Input jpeg file compression ratio 200% are supposed.
	dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
	dec_param.corr_flg		= 0;
	dec_param.pause_flg		= 0;

	ct_jpeg_decode( &dec_param );
	return;
}

static VOID ct_im_jpeg_check_handler( INT32 ptn )
{
	IO_JPG7.JSTATE.bit.JALLRSTP = 1;
	IO_JPG7.JINTEN.word = 0xFFFFFFFF;

//	IO_JPG7.JPMODE.bit.DISPOSAL = 0;
	IO_JPG7.JPMODE.bit.DISPOSAL = 1;

	switch ( ptn ) {
		case 1:
		//	JCLRFMER
			IO_JPG7.JINT.word		= 0x20040000;
			IO_JPG7.JPINTSTA.word	= 0x00000000;
			break;
		case 2:
		//	JJBINT
			IO_JPG7.JJBSTA.bit.JJRES = 0x2;
			IO_JPG7.JINT.word		= 0x20400000;
			IO_JPG7.JPINTSTA.word	= 0x00000000;
			break;
		case 3:
		//	JPBINT
			IO_JPG7.JPBSTA.bit.JPRES = 0x3;
			IO_JPG7.JINT.word		= 0x20800000;
			IO_JPG7.JPINTSTA.word	= 0x00000000;
			break;
		case 4:
		//	JLINEINT
			IO_JPG7.JINT.word		= 0x20000;
			IO_JPG7.JPINTSTA.word	= 0x00000000;
			break;
		case 5:
		//	JSECTINT
			IO_JPG7.JINT.word		= 0x10000;
			IO_JPG7.JPINTSTA.word	= 0x00000000;
			break;
		case 6:
		//	ERR_FLG
			IO_JPG7.JINT.word		= 0x09000000;
			IO_JPG7.JPINTSTA.word	= 0x00000400;
			break;
		case 7:
		//	SIZERR
			IO_JPG7.JINT.word		= 0x09000000;
			IO_JPG7.JPINTSTA.word	= 0x00000200;
			break;
		case 8:
		//	RSTERR
			IO_JPG7.JINT.word		= 0x09000000;
			IO_JPG7.JPINTSTA.word	= 0x00000100;
			break;
		case 9:
		//	UMKINFO
			IO_JPG7.JPSUSPEND.bit.JPSUSPEND = 1;
			IO_JPG7.JINT.word		= 0x8000000;
			IO_JPG7.JPINTSTA.word	= 0x00000004;
			break;
		case 10:
		//	COMMKONFO
			IO_JPG7.JPSUSPEND.bit.JPSUSPEND = 1;
			IO_JPG7.JINT.word		= 0x8000000;
			IO_JPG7.JPINTSTA.word	= 0x00000002;
			break;
		case 11:
		//	APPINFO
			IO_JPG7.JPSUSPEND.bit.JPSUSPEND = 1;
			IO_JPG7.JINT.word		= 0x8000000;
			IO_JPG7.JPINTSTA.word	= 0x00000001;
			break;
		case 12:
		//	COREND
			IO_JPG7.JINT.word		= 0x08000000;
			IO_JPG7.JPINTSTA.word	= 0x40000000;
			IO_JPG7.JPCORSTA.word	= 0x00000003;
			break;
		case 13:
		// Normal
			IO_JPG7.JINT.word		= 0x0C00C000;
			IO_JPG7.JPINTSTA.word	= 0x80000000;
			IO_JPG7.JPBYTCNT.word	= 0xFFFFFFFF;
			break;
		default:
			Ddim_Print(("input parameter error\n"));
			return;
	}

	IO_JPG7.JPSTATUS.bit.JPSTATUS = 1;

	Im_JPEG_Int_Handler();

	Ddim_Print(("IO_JPG7.JINT     = [0x%lX]\n", IO_JPG7.JINT.word));
	Ddim_Print(("IO_JPG7.JPINTSTA = [0x%lX]\n", IO_JPG7.JPINTSTA.word));
	Ddim_Print(("IO_JPG7.JPSUSPEND = [0x%X]\n", IO_JPG7.JPSUSPEND.bit.JPSUSPEND));

	return;
}

// for palladium
ULONG Ct_Im_Jpeg_Get_Enc_Size( VOID )
{
	return gJpeg_Encode_Size;
}


/**************************************************************************************
*
*	JPEG EPC test
*
***************************************************************************************/
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_CT_IM_JPG_SPENC_MULTI_ON
#define D_CT_IM_JPG_SPENC_RETRY_MAX_COUNT		(30)

#define M_MIN( val1, val2 )			(( (val1) < (val2) )?(val1):(val2))

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	T_CT_IM_JPG_SPENC_MODE_CAPTURE = 0,
	T_CT_IM_JPG_SPENC_MODE_PLAY,
	T_CT_IM_JPG_SPENC_MODE_MOVIE,
	T_CT_IM_JPG_SPENC_MODE_MAX,
} T_CT_IM_JPG_SPENC_MODE;


typedef enum {
	CR_4_0                       = (short )0x0000,	/**< CR 4.0	*/
	CR_4_5                       = (short )0x0001,	/**< CR 4.5	*/
	CR_5_0                       = (short )0x0002,	/**< CR 5.0	*/
	CR_5_5                       = (short )0x0003,	/**< CR 5.5	*/
	CR_6_0                       = (short )0x0004,	/**< CR 6.0	*/
	CR_6_5                       = (short )0x0005,	/**< CR 6.5	*/
	CR_7_0                       = (short )0x0006,	/**< CR 7.0	*/
	CR_7_5                       = (short )0x0007,	/**< CR 7.5	*/
	CR_8_0                       = (short )0x0008,	/**< CR 8.0	*/
	CR_8_5                       = (short )0x0009,	/**< CR 8.5	*/
	CR_9_0                       = (short )0x000A,	/**< CR 9.0	*/
	CR_9_5                       = (short )0x000B,	/**< CR 9.5	*/
	CR_10_0                      = (short )0x000C,	/**< CR 10.0	*/
	CR_12_0                      = (short )0x000D,	/**< CR 12.0	*/
	CR_14_0                      = (short )0x000E,	/**< CR 14.0	*/
	CR_16_0                      = (short )0x000F,	/**< CR 16.0	*/
	CR_17_0                      = (short )0x0010,	/**< CR 17.0	*/
	CR_18_0                      = (short )0x0011,	/**< CR 18.0	*/
	CR_19_0                      = (short )0x0012,	/**< CR 19.0	*/
	CR_20_0                      = (short )0x0013,	/**< CR 20.0	*/
	CR_21_0                      = (short )0x0014,	/**< CR 21.0	*/
	CR_22_0                      = (short )0x0015,	/**< CR 22.0	*/
	CR_23_0                      = (short )0x0016,	/**< CR 23.0	*/
	CR_24_0                      = (short )0x0017,	/**< CR 24.0	*/
	CR_25_0                      = (short )0x0018,	/**< CR 25.0	*/
	CR_26_0                      = (short )0x0019,	/**< CR 26.0	*/
	CR_27_0                      = (short )0x001A,	/**< CR 27.0	*/
	CR_28_0                      = (short )0x001B,	/**< CR 28.0	*/
	CR_29_0                      = (short )0x001C,	/**< CR 29.0	*/
	CR_30_0                      = (short )0x001D,	/**< CR 30.0	*/
	CR_31_0                      = (short )0x001E,	/**< CR 31.0	*/
	CR_32_0                      = (short )0x001F,	/**< CR 32.0	*/
	CR_33_0                      = (short )0x0020,	/**< CR 33.0	*/
	CR_34_0                      = (short )0x0021,	/**< CR 34.0	*/
	CR_35_0                      = (short )0x0022,	/**< CR 35.0	*/
	CR_36_0                      = (short )0x0023,	/**< CR 36.0	*/
	CR_37_0                      = (short )0x0024,	/**< CR 37.0	*/
	CR_38_0                      = (short )0x0025,	/**< CR 38.0	*/
	CR_39_0                      = (short )0x0026,	/**< CR 39.0	*/
	CR_40_0                      = (short )0x0027,	/**< CR 40.0	*/
	CR_3_0                       = (short )0x0028,	/**< CR 3.0	*/
	CR_3_5                       = (short )0x0029,	/**< CR 3.5	*/
	CR_2_0                       = (short )0x002A,	/**< CR 2.0	*/
	CR_2_5                       = (short )0x002B,	/**< CR 2.5	*/
	CR_2_7                       = (short )0x002C	/**< CR 2.7	*/
} COMPRESSION_RATIO;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	T_IM_JPEG_ENC_MNG			jpeg_enc_tbl;		// Parameter of Im_JPEG_Ctrl_Enc().
	T_IM_JPEG_ENC_FRAME_MNG		jpeg_enc_frame_tbl;	// Parameter of Im_JPEG_Ctrl_Enc_Frame().
	T_CT_IM_JPG_SPENC_MODE		mode;				// Encode mode (parameter of Jpgepc_Wrap_Set_Tuning_Param()).
	ULONG						limit_size;			// Maximum bytes of JPEG code.
	USHORT						quality;			// Quality value of JPEG code when encode is successful. (In/Out)
} T_CT_IM_JPG_SPENC_PARAM;


typedef struct {
	UCHAR	downsp_type;			// Downsample type (Parameter of Im_JPEG_Set_Down_Sampling_Rate())
	UCHAR	max_retry;				// Maximum retry count. (1~D_JPGEPC_RETRY_MAX_COUNT)
	UINT32	shift;					// Shift value.
	UINT32	ratio;					// Target ratio.
	ULONG	target_bytes;			// Target bytes.
} T_CT_IM_JPG_SPENC_TUNING_PARAM;

/*----------------------------------------------------------------------*/
/* Global															*/
/*----------------------------------------------------------------------*/

static const T_IM_JPEG_AXI_CTRL gCT_Im_Jpeg_Pbuf_Ctrl = {				// PBUF(AXI) controll.
		.endian = E_IM_JPEG_ENDIAN_LITTLE,
		.issue_tran_num = E_IM_JPEG_ISSUE_TRAN_8,
		.cache_type = D_IM_JPEG_NON_CACHE_NON_BUF,
		.prot_type = D_IM_JPEG_D_SEC_NORMAL_ACCESS,
		.err_state = 0,
};
static const T_IM_JPEG_AXI_CTRL gCT_Im_Jpeg_Jbuf_Ctrl = {				// JBUF(AXI) controll.
		.endian = E_IM_JPEG_ENDIAN_LITTLE,
		.issue_tran_num = E_IM_JPEG_ISSUE_TRAN_8,
		.cache_type = D_IM_JPEG_NON_CACHE_NON_BUF,
		.prot_type = D_IM_JPEG_D_SEC_NORMAL_ACCESS,
		.err_state = 0,
};

static USHORT gCT_Im_Jpeg_Cr;

/*----------------------------------------------------------------------*/
/* Function															*/
/*----------------------------------------------------------------------*/
static SHORT ct_im_jpeg_get_encode_quality( USHORT cr )
{
	gCT_Im_Jpeg_Cr = cr;
	SHORT quality = 1;

	switch ( cr ) {
		case CR_2_0:
		case CR_2_5:
		case CR_2_7:
		case CR_3_0:
		case CR_3_5:
		case CR_4_0:
		case CR_4_5:
			quality = 40;
			break;
		case CR_5_0:
		case CR_5_5:
		case CR_6_0:
		case CR_6_5:
			break;
		case CR_7_0:
			quality = 8;
			break;
		case CR_7_5:
			break;
		case CR_8_0:
			quality = 7;
			break;
		case CR_8_5:
		case CR_9_0:
		case CR_9_5:
			break;
		case CR_10_0:
			quality = 3;
			break;
		case CR_12_0:
			break;
		case CR_14_0:
			quality = 2;
			break;
		case CR_16_0:
		case CR_17_0:
		case CR_18_0:
			break;
		case CR_19_0:
			quality = 1;
			break;
		case CR_20_0:
		case CR_21_0:
		case CR_22_0:
		case CR_23_0:
		case CR_24_0:
		case CR_25_0:
		case CR_26_0:
		case CR_27_0:
		case CR_28_0:
		case CR_29_0:
		case CR_30_0:
		case CR_31_0:
		case CR_32_0:
		case CR_33_0:
		case CR_34_0:
		case CR_35_0:
		case CR_36_0:
		case CR_37_0:
		case CR_38_0:
		case CR_39_0:
		case CR_40_0:
			break;
		default :
			break;
	}
	return quality;
}

static ULONG ct_im_jpeg_calc_target_size( USHORT width, USHORT lines, T_CT_IM_JPG_SPENC_MODE mode )
{
	ULONG result;
	USHORT cr;

	result= width * lines * 2;
	cr = gCT_Im_Jpeg_Cr;

	if (width == 160) {
		if ( (cr >= 40) || (cr == 0) ) {
			cr = 1;		// Max 4_5
		}
		else if (cr > 19) {
			cr = 19;	// min 20_0
		}
	}
	else {
		if ( (mode == T_CT_IM_JPG_SPENC_MODE_CAPTURE) || (mode == T_CT_IM_JPG_SPENC_MODE_PLAY) ) {
			if (cr >= 41) {
				cr = 0;		// Max 4_0
			}
		}
	}

	// cr to div value
	switch ( cr ) {
		case CR_4_0:
		case CR_5_0:
		case CR_6_0:
		case CR_7_0:
		case CR_8_0:
		case CR_9_0:
		case CR_10_0:
			result /= (ULONG)cr / 2 + 4;
			break;
		case CR_4_5:
		case CR_5_5:
		case CR_6_5:
		case CR_7_5:
		case CR_8_5:
		case CR_9_5:
			result /= ((DOUBLE)cr + 8) / 2;
			break;
		case CR_12_0:
			result /= 12;
			break;
		case CR_14_0:
			result /= 14;
			break;
		case CR_16_0:
		case CR_17_0:
		case CR_18_0:
		case CR_19_0:
		case CR_20_0:
		case CR_21_0:
		case CR_22_0:
		case CR_23_0:
		case CR_24_0:
		case CR_25_0:
		case CR_26_0:
		case CR_27_0:
		case CR_28_0:
		case CR_29_0:
		case CR_30_0:
		case CR_31_0:
		case CR_32_0:
		case CR_33_0:
		case CR_34_0:
		case CR_35_0:
		case CR_36_0:
		case CR_37_0:
		case CR_38_0:
		case CR_39_0:
		case CR_40_0:
			result /= (ULONG)cr + 1;
			break;
		case CR_3_0:
			result /= 3;
			break;
		case CR_3_5:
			result /= (DOUBLE)7 / 2;
			break;
		case CR_2_0:
			result /= 2;
			break;
		case CR_2_5:
			result /= (DOUBLE)5 / 2;
			break;
		case CR_2_7:
			result /= (DOUBLE)27 / 10;
			break;
		default:
			break;
	}
	return result;
}

static VOID ct_im_jpeg_special_set_tuning_param( T_CT_IM_JPG_SPENC_TUNING_PARAM* tuning_param, T_CT_IM_JPG_SPENC_PARAM* jpgenc_param )
{
	UINT32 pixs;

	pixs = jpgenc_param->jpeg_enc_tbl.width * jpgenc_param->jpeg_enc_tbl.lines;

	tuning_param->ratio = 0x00007999;
	tuning_param->max_retry = 30;

	if( pixs <= (160 * 120) ) {
		// ThumbNail
		tuning_param->shift = 0;
		tuning_param->downsp_type = D_IM_JPEG_DOWNSP_NONE;
	}
	else if( pixs <= (640 *480) ) {
		// Main VGA
		tuning_param->shift = 5;
		tuning_param->downsp_type = D_IM_JPEG_DOWNSP_NONE;
	}
	else if( pixs <= (1700 * (1700 *3 /4)) ) {
		// Main 2M
		tuning_param->downsp_type = D_IM_JPEG_DOWNSP_1_2;
		tuning_param->shift = 10;
	}
	else {
		tuning_param->downsp_type = D_IM_JPEG_DOWNSP_1_8;
		tuning_param->shift = 10;
	}

	tuning_param->target_bytes = ct_im_jpeg_calc_target_size( jpgenc_param->jpeg_enc_tbl.width, jpgenc_param->jpeg_enc_tbl.lines, jpgenc_param->mode );

}

static VOID ct_im_jpeg_special_init_param( T_CT_IM_JPG_SPENC_PARAM* jpgenc_param, T_CT_IM_JPG_SPENC_TUNING_PARAM* tuning_param, UCHAR* realenc )
{
	//---- Set downsp_typeation Ratio & Target Size ----
	ct_im_jpeg_special_set_tuning_param( tuning_param, jpgenc_param );

	tuning_param->max_retry = M_MIN( tuning_param->max_retry, D_CT_IM_JPG_SPENC_RETRY_MAX_COUNT );

	jpgenc_param->jpeg_enc_frame_tbl.limit_size = jpgenc_param->limit_size;

	//---- Set SDRAM Limit ----
	Ddim_Print(( "TargetSize=%lu\n", tuning_param->target_bytes ));
	Ddim_Print(( "limit Size=%lu\n", jpgenc_param->limit_size ));

	if( tuning_param->target_bytes > jpgenc_param->limit_size ) {
		tuning_param->target_bytes = jpgenc_param->limit_size;
	}

	Ddim_Print(( "==== Target(limited)=%lu ====\n", tuning_param->target_bytes ));

	tuning_param->target_bytes >>= tuning_param->shift;
	tuning_param->target_bytes = (tuning_param->target_bytes * tuning_param->ratio) / 0x8000;

	Ddim_Print(( "==== Target * %.2f ====\n", (((FLOAT)tuning_param->ratio) / 0x8000) ));
	Ddim_Print(( "==== TargetSize=%lu ====\n", tuning_param->target_bytes ));

	//---- JPEG fixed Size ----
#ifdef D_CT_IM_JPG_SPENC_MULTI_ON
	if( tuning_param->downsp_type == D_IM_JPEG_DOWNSP_NONE ) {
		jpgenc_param->jpeg_enc_frame_tbl.code_count_flg = D_IM_JPEG_ENABLE_OFF;
		*realenc = 0;
	}
	else {
		jpgenc_param->jpeg_enc_frame_tbl.code_count_flg = D_IM_JPEG_ENABLE_ON;	// Pass_1
		*realenc = 1;
	}
#else
	jpgenc_param->jpeg_enc_frame_tbl.code_count_flg = D_IM_JPEG_ENABLE_OFF;
	*realenc = 0;
#endif
}

static INT32 ct_im_jpeg_special_init_macro( T_CT_IM_JPG_SPENC_PARAM* jpgenc_param, T_CT_IM_JPG_SPENC_TUNING_PARAM* tuning_param )
{
	INT32 ret;

#ifdef CO_DEBUG_ON_PC
	IO_JPG7.JSTATE.bit.JALLRSTP = 0;
#endif	// CO_DEBUG_ON_PC

	Im_JPEG_Init();
	Im_JPEG_Set_QTbl( NULL, 0 );
	ret = Im_JPEG_Ctrl_Enc( &jpgenc_param->jpeg_enc_tbl );
	if( ret != D_IM_JPEG_OK ) {
		return ret;
	}
	ret = Im_JPEG_Ctrl_Enc_Frame( &jpgenc_param->jpeg_enc_frame_tbl );
	if( ret != D_IM_JPEG_OK ) {
		return ret;
	}
	Im_JPEG_Set_Down_Sampling_Rate( tuning_param->downsp_type );

	return D_IM_JPEG_OK;
}


static INT32 ct_im_jpeg_special_run_encode( T_CT_IM_JPG_SPENC_PARAM* jpgenc_param, T_CT_IM_JPG_SPENC_TUNING_PARAM* tuning_param, UINT32 quality, USHORT* err_ratio )
{
	INT32 ret;

	ret = Im_JPEG_Set_Quality( quality, 0 );
	if( ret != D_IM_JPEG_OK ) {
		return ret;
	}
	ret = Im_JPEG_Ctrl_Enc_Frame( &jpgenc_param->jpeg_enc_frame_tbl );
	if( ret != D_IM_JPEG_OK ) {
		return ret;
	}
	*err_ratio = Im_JPEG_Set_Down_Sampling_Rate( tuning_param->downsp_type );
	ret = Im_JPEG_Start_Enc();
	if( ret != D_IM_JPEG_OK ) {
		return ret;
	}

	ct_jpeg_start_hclock();
	Ddim_Print(("IO_JPG7.JINTEN   =0x%lX\n", IO_JPG7.JINTEN.word));
	Ddim_Print(("IO_JPG7.JPSTATUS.bit.JPSTATUS =0x%X\n", IO_JPG7.JPSTATUS.bit.JPSTATUS));
	Ddim_Print(("IO_JPG7.JPCMD    =0x%lX\n", IO_JPG7.JPCMD.word));
	Ddim_Print(("IO_JPG7.JPWIDTH  =0x%lX\n", IO_JPG7.JPWIDTH.word));
	Ddim_Print(("IO_JPG7.JPHEIGHT =0x%lX\n", IO_JPG7.JPHEIGHT.word));
	Ddim_Print(("IO_JPG7.JIMGYGH  =0x%lX\n", IO_JPG7.JIMGYGH.word));
	Ddim_Print(("IO_JPG7.JIMGCGH  =0x%lX\n", IO_JPG7.JIMGCGH.word));
	Ddim_Print(("IO_JPG7.JIMGSH   =0x%lX\n", IO_JPG7.JIMGSH.word));
	Ddim_Print(("IO_JPG7.JIMGSV   =0x%lX\n", IO_JPG7.JIMGSV.word));
	Ddim_Print(("IO_JPG7.JIMGSHDG =0x%lX\n", IO_JPG7.JIMGSHDG.word));
	Ddim_Print(("IO_JPG7.JPFORMAT =0x%lX\n", IO_JPG7.JPFORMAT.word));
	Ddim_Print(("IO_JPG7.JMODE    =0x%lX\n", IO_JPG7.JMODE.word));
	Ddim_Print(("IO_JPG7.JCCTL    =0x%lX\n", IO_JPG7.JCCTL.word));
	Ddim_Print(("IO_JPG7.JICTL    =0x%lX\n", IO_JPG7.JICTL.word));
	Ddim_Print(("IO_JPG7.JJBSTA   =0x%lX\n", IO_JPG7.JJBSTA.word));
	Ddim_Print(("IO_JPG7.JPBSTA   =0x%lX\n", IO_JPG7.JPBSTA.word));
	Ddim_Print(("IO_JPG7.JPMODE   =0x%lX\n", IO_JPG7.JPMODE.word));
	ct_jpeg_stop_hclock();


#ifdef CO_DEBUG_ON_PC
	ct_jpeg_encode_int_handler(1);
#endif	// CO_DEBUG_ON_PC

	ret = Im_JPEG_Wait_End_Enc( &jpgenc_param->jpeg_enc_tbl, 1000 );
	Ddim_Print(( "I:Im_JPEG_Wait_End_Enc ret=%d\n", ret ));
	if( ret != D_IM_JPEG_OK ) {
		if( ret != D_IM_JPEG_ENCODE_PAUSE ) {
			Ddim_Print(( "E:jpeg encode error ret=%d\n", ret ));
			jpgenc_param->jpeg_enc_tbl.code_size = 0;
			return ret;
		}
	}
	if( (jpgenc_param->jpeg_enc_tbl.code_size < jpgenc_param->limit_size) && (ret == D_IM_JPEG_OK) ) {
		return D_IM_JPEG_OK;
	}
	else {
		return D_IM_JPEG_ENCODE_PAUSE;
	}
}


static INT32 ct_im_jpeg_special_encode_pass1( T_CT_IM_JPG_SPENC_PARAM* jpgenc_param, T_CT_IM_JPG_SPENC_TUNING_PARAM* tuning_param, UINT32* quality )
{
	UCHAR	flg_Hretry = 0;
	UCHAR	retry_cnt;
	UINT32	quality_0 = 0;
	UINT32	quality_1 = 0;
	UINT32	quality_best = 0;
	USHORT	err_ratio;
	INT32	size_0 = 0;
	INT32	size_1 = 0;
	INT32	sizediff_best = 0x00FFFFFF;
	INT32	jpeg_size_try[D_CT_IM_JPG_SPENC_RETRY_MAX_COUNT];
	INT32	ret;

	for( retry_cnt = 0; retry_cnt < tuning_param->max_retry; retry_cnt++ ) {
		if( 0 == retry_cnt ) {
			quality_0 = *quality = 0x40;
		}
		else if( 1 == retry_cnt ) {
			quality_1 = *quality = 50<<10;
		}
		//---- try ---
		ret = ct_im_jpeg_special_run_encode( jpgenc_param, tuning_param, *quality, &err_ratio );
		if( (ret != D_IM_JPEG_ENCODE_PAUSE) && (ret != D_IM_JPEG_OK) ) {
			// Encode error
			return ret;
		}

		jpeg_size_try[retry_cnt] = ( jpgenc_param->jpeg_enc_tbl.code_size >> (tuning_param->shift - (tuning_param->downsp_type * 2)) );
		Ddim_Print(( "err_ratio [0x%X] :\n", err_ratio ));
		if( err_ratio != 0x100 ) {
			jpeg_size_try[retry_cnt] = (jpeg_size_try[retry_cnt] * err_ratio) >> 8;
		}
		Ddim_Print(( "Count [%2u] : quality=%2d.%d  size=%4u [%lu] /tgtsize=%4lu\n",
				retry_cnt, (*quality) >> 10, (*quality) & 0x3FF, jpeg_size_try[retry_cnt], jpgenc_param->jpeg_enc_tbl.code_size, tuning_param->target_bytes ));


		//==== Check CodeSize ====
		//---- 1st ,2nd ----
		if( 0 == retry_cnt ) {
			if( tuning_param->target_bytes < jpeg_size_try[retry_cnt] ) {
				Ddim_Print(( "Abort0 : Lowest SF\n" ));
				break;
			}
			size_0 = jpeg_size_try[retry_cnt];
		}
		else if( 1 == retry_cnt ) {
			if ( tuning_param->target_bytes > jpeg_size_try[retry_cnt] ) {
				if( flg_Hretry != 0 ) {
					Ddim_Print(( "Abort1 : High Side SF\n" ));
					break;
				}
				else {
					*quality = 70 << 10;
					flg_Hretry++;
				}
			}
			else {
				size_1 = jpeg_size_try[retry_cnt];
				*quality = ((tuning_param->target_bytes - size_0) * (quality_1 - quality_0)) / (size_1 - size_0) + quality_0;
			}
		}
		//---- Check & Next Setting ----
		else {
			//---- save the best value ----
			if( tuning_param->target_bytes > jpeg_size_try[retry_cnt] ) {
				Ddim_Print(( "---> Update1\n" ));
				if( sizediff_best > tuning_param->target_bytes - jpeg_size_try[retry_cnt] ) {
					sizediff_best = tuning_param->target_bytes - jpeg_size_try[retry_cnt];
					quality_best = *quality;
					Ddim_Print(( "---> Update2 Best quality!!\n" ));
				}
			}
			if( tuning_param->target_bytes > jpeg_size_try[retry_cnt] ) {
				quality_0 = *quality;
				size_0 = jpeg_size_try[retry_cnt];
			}
			else {
				quality_1 = *quality;
				size_1 = jpeg_size_try[retry_cnt];
			}
			//---- OK : Size （Code size is target in - 0%～+1.56%
			if( (jpeg_size_try[retry_cnt] >= tuning_param->target_bytes) && 												// tgt-0
				(jpeg_size_try[retry_cnt] < (tuning_param->target_bytes + (tuning_param->target_bytes >> 6))) ) {			// tgt+1.56%
				Ddim_Print(( "OK : JPEG Size in Target Range!!\n" ));
				break;
			}
			//---- white---
			if( retry_cnt > 0 ) {
				if( (jpeg_size_try[retry_cnt-1] - (jpeg_size_try[retry_cnt-1] >> 7) < jpeg_size_try[retry_cnt] ) &&		// -0.78%
					(jpeg_size_try[retry_cnt-1] + (jpeg_size_try[retry_cnt-1] >> 7) > jpeg_size_try[retry_cnt] ) ) {		// +0.78%
					if( jpeg_size_try[retry_cnt] < tuning_param->target_bytes ) {
						Ddim_Print(( "Abort2 : Saturation1 Lower Size [like White Image]\n" ));
						break;
					}
				}
				if( jpeg_size_try[retry_cnt-1] == jpeg_size_try[retry_cnt] ) {
					if( jpeg_size_try[retry_cnt] < tuning_param->target_bytes ) {
						Ddim_Print(( "Abort3 : Saturation2\n" ));
						break;
					}
				}
			}
			//---- Zero Div ----
			if( size_1 == size_0 ) {
				Ddim_Print(( "Abort4 : Zero Div\n" ));
				break;
			}

			//---- calc quarity ----
			*quality = ((tuning_param->target_bytes - size_0) * (quality_1 - quality_0)) / (size_1 - size_0) + quality_0;

			//---- HOKEN ----
			if( retry_cnt >= (tuning_param->max_retry -2) ) {
				if( quality_best != 0 ) {
					*quality = quality_best;
					Ddim_Print(( "**** HOKEN quality BEST ****\n" ));
					break;
				}
				else if( jpeg_size_try[retry_cnt] < tuning_param->target_bytes ) {
					Ddim_Print(( "**** HOKEN break ****\n" ));
					break;
				}
				else {
					*quality = 10;
					Ddim_Print(( "**** HOKEN Force set High-CompRatio ****\n" ));
				}
			}
		}
	}
	return D_IM_JPEG_OK;
}


static INT32 ct_im_jpeg_special_encode_real( T_CT_IM_JPG_SPENC_PARAM* jpgenc_param, T_CT_IM_JPG_SPENC_TUNING_PARAM* tuning_param, UINT32* quality )
{
	USHORT	err_ratio;
	INT32	ret;

	Ddim_Print(( "**** Real Encode ****\n" ));

	jpgenc_param->jpeg_enc_frame_tbl.code_count_flg = D_IM_JPEG_ENABLE_OFF;
	tuning_param->downsp_type = D_IM_JPEG_DOWNSP_NONE;

	while( 1 ) {
		ret = ct_im_jpeg_special_run_encode( jpgenc_param, tuning_param, *quality, &err_ratio );

		Ddim_Print(( "**** MainEnc : quality=%2u.%u  size=%4lu [%lu] /tgtsize=%4lu ****\n", (*quality) >> 10, (*quality) & 0x3FF, (jpgenc_param->jpeg_enc_tbl.code_size >> tuning_param->shift), jpgenc_param->jpeg_enc_tbl.code_size, tuning_param->target_bytes ));

		if( ret == D_IM_JPEG_OK ) {
			// Encode success
			return ret;
		}
		if( ret != D_IM_JPEG_ENCODE_PAUSE ) {
			jpgenc_param->jpeg_enc_tbl.code_size = 0;
			// Encode error
			return ret;
		}

		// Code size over
		*quality -= (1<<10);

		Ddim_Print(( "RETRY at MainEncode!!\n" ));

	}
}

// Capture_Jpeg_Encode
ULONG CT_Im_Jpeg_Special_Encode( T_CT_IM_JPG_SPENC_PARAM* jpgenc_param )
{
	T_CT_IM_JPG_SPENC_TUNING_PARAM tuning_param = {
		.downsp_type = D_IM_JPEG_DOWNSP_1_8,
		.max_retry = D_CT_IM_JPG_SPENC_RETRY_MAX_COUNT,
		.shift = 10,
		.ratio = 0x00007999,					// 0x8000 => 1.0
	};
	UCHAR	realenc = 0;
	UINT32	quality = 0;
	INT32	ret;
	ULONG	jpeg_size = 0;

	// parameter initialize
	ct_im_jpeg_special_init_param( jpgenc_param, &tuning_param, &realenc );

	//---- JPEG Open ----
	ret = Im_JPEG_Open( 1000 );
	if( ret != D_IM_JPEG_OK ) {
		Ddim_Print(( "E:jpeg encode open error ret=%d\n", ret ));
		return 0;
	}

	while( 1 ) {	// for Error return
		ret = ct_im_jpeg_special_init_macro( jpgenc_param, &tuning_param );
		if (ret != D_DDIM_OK) {
			Ddim_Print(( "E:jpeg encode set error width=%d, line=%d, ret=%d\n", jpgenc_param->jpeg_enc_tbl.width, jpgenc_param->jpeg_enc_tbl.lines, ret ));
			jpeg_size = 0UL;
			break;
		}


		//==== Retry Loop ====
		ret = ct_im_jpeg_special_encode_pass1( jpgenc_param, &tuning_param, &quality );
		if( ret != D_IM_JPEG_OK ) {
			jpeg_size = 0UL;
			break;
		}

		// ==== JPEG Encode ====
		if( (tuning_param.downsp_type != D_IM_JPEG_DOWNSP_NONE) || (realenc == 1) ) {
			ret = ct_im_jpeg_special_encode_real( jpgenc_param, &tuning_param, &quality );
			if( ret != D_IM_JPEG_OK ) {
				jpeg_size = 0UL;
				break;
			}
		}

		//---- return val ----
		jpgenc_param->quality = (USHORT)(quality >> 10);		// return 10bit
		jpeg_size = jpgenc_param->jpeg_enc_tbl.code_size;

		break;
	}

	ret = Im_JPEG_Close();
	if( ret != D_IM_JPEG_OK ) {
		Ddim_Print(( "E:jpeg encode close error ret=%d\n", ret));
		jpeg_size = 0UL;
	}

	Ddim_Print(( "#### Return : size=%lu  quality=%u ####\n", jpeg_size, jpgenc_param->quality ));

	return jpeg_size;

}

VOID Ct_Im_Jpeg_Run_1( INT32 ct_param_1, INT32 ct_param_2 )
{
	T_CT_IM_JPG_SPENC_PARAM	jpgenc_param;

#ifdef CO_DEBUG_ON_PC
	memset((USHORT*)&IO_JPG7, 0, sizeof(IO_JPG7));
#endif	// CO_DEBUG_ON_PC

	if ( ct_param_1 != 1 && ct_param_1 != 6 ) {

#ifdef CO_DEBUG_ON_PC
		IO_JPG7.JSTATE.bit.JALLRSTP = 0;
#endif	// CO_DEBUG_ON_PC

		Ddim_Print(( "*** Jpeg begin Im_Jpeg_Init\n" ));
		Im_JPEG_Init();
		Ddim_Print(( "*** Jpeg end Im_Jpeg_Init\n" ));
	}

	switch ( ct_param_1 ) {
		case 1:
			if (ct_param_2 == 1) {
				Ddim_Print(("TEST 1_1_1 Start\n"));
				ct_im_jpeg_1_1_1();
				Ddim_Print(("TEST 1_1_1 End\n"));
			}
			else {
				Ddim_Print(("input parameter error\n"));
			}
			break;
		case 2:
			if (ct_param_2 == 1) {
				Ddim_Print(("TEST 1_2_1 Start\n"));
				ct_im_jpeg_1_2_1();
				Ddim_Print(("TEST 1_2_1 End\n"));
			}
			else if (ct_param_2 == 2) {
				Ddim_Print(("TEST 1_2_2 Start\n"));
				ct_im_jpeg_1_2_2();
				Ddim_Print(("TEST 1_2_2 End\n"));
			}
			else {
				Ddim_Print(("input parameter error\n"));
			}
			break;

		case 3:
			switch ( ct_param_2 ) {
				case 1:
					Ddim_Print(("TEST 1_3_1 Start\n"));
					ct_im_jpeg_1_3_1();
					Ddim_Print(("TEST 1_3_1 End\n"));
					break;
				case 2:
					Ddim_Print(("TEST 1_3_2 Start\n"));
					ct_im_jpeg_1_3_2();
					Ddim_Print(("TEST 1_3_2 End\n"));
					break;
				case 3:
					Ddim_Print(("TEST 1_3_3 Start\n"));
					ct_im_jpeg_1_3_3();
					Ddim_Print(("TEST 1_3_3 End\n"));
					break;
				case 4:
					Ddim_Print(("TEST 1_3_4 Start\n"));
					ct_im_jpeg_1_3_4();
					Ddim_Print(("TEST 1_3_4 End\n"));
					break;
				case 5:
					Ddim_Print(("TEST 1_3_5 Start\n"));
					ct_im_jpeg_1_3_5();
					Ddim_Print(("TEST 1_3_5 End\n"));
					break;
				case 6:
					Ddim_Print(("TEST 1_3_6 Start\n"));
					ct_im_jpeg_1_3_6();
					Ddim_Print(("TEST 1_3_6 End\n"));
					break;
				case 7:
					Ddim_Print(("TEST 1_3_7 Start\n"));
					ct_im_jpeg_1_3_7();
					Ddim_Print(("TEST 1_3_7 End\n"));
					break;
				case 8:
					Ddim_Print(("TEST 1_3_8 Start\n"));
					ct_im_jpeg_1_3_8();
					Ddim_Print(("TEST 1_3_8 End\n"));
					break;
				case 9:
					Ddim_Print(("TEST 1_3_9 Start\n"));
					ct_im_jpeg_1_3_9();
					Ddim_Print(("TEST 1_3_9 End\n"));
					break;
				case 10:
					Ddim_Print(("TEST 1_3_10 Start\n"));
					ct_im_jpeg_1_3_10();
					Ddim_Print(("TEST 1_3_10 End\n"));
					break;
				case 11:
					Ddim_Print(("TEST 1_3_11 Start\n"));
					ct_im_jpeg_1_3_11();
					Ddim_Print(("TEST 1_3_11 End\n"));
					break;
				case 12:
					Ddim_Print(("TEST 1_3_12 Start\n"));
					ct_im_jpeg_1_3_12();
					Ddim_Print(("TEST 1_3_12 End\n"));
					break;
				case 13:
					Ddim_Print(("TEST 1_3_13 Start\n"));
					ct_im_jpeg_1_3_13();
					Ddim_Print(("TEST 1_3_13 End\n"));
					break;
				case 14:
					Ddim_Print(("TEST 1_3_14 Start\n"));
					ct_im_jpeg_1_3_14();
					Ddim_Print(("TEST 1_3_14 End\n"));
					break;
				case 15:
					Ddim_Print(("TEST 1_3_15 Start\n"));
					ct_im_jpeg_1_3_15();
					Ddim_Print(("TEST 1_3_15 End\n"));
					break;
				case 16:
					Ddim_Print(("TEST 1_3_16 Start\n"));
					ct_im_jpeg_1_3_16();
					Ddim_Print(("TEST 1_3_16 End\n"));
					break;
				case 17:
					Ddim_Print(("TEST 1_3_17 Start\n"));
					ct_im_jpeg_1_3_17();
					Ddim_Print(("TEST 1_3_17 End\n"));
					break;
				case 18:
					Ddim_Print(("TEST 1_3_18 Start\n"));
					ct_im_jpeg_1_3_18();
					Ddim_Print(("TEST 1_3_18 End\n"));
					break;
				case 19:
					Ddim_Print(("TEST 1_3_19 Start\n"));
					ct_im_jpeg_1_3_19();
					Ddim_Print(("TEST 1_3_19 End\n"));
					break;
				case 20:
					Ddim_Print(("TEST 1_3_20 Start\n"));
					ct_im_jpeg_1_3_20();
					Ddim_Print(("TEST 1_3_20 End\n"));
					break;
				case 21:
					Ddim_Print(("TEST 1_3_21 Start\n"));
					ct_im_jpeg_1_3_21();
					Ddim_Print(("TEST 1_3_21 End\n"));
					break;
				default:
					Ddim_Print(("input parameter error\n"));
					break;
			}
			break;

		case 4:
			switch ( ct_param_2 ) {
				case 1:
					Ddim_Print(("TEST 1_4_1 empty number\n"));
					break;
				case 2:
					Ddim_Print(("TEST 1_4_2 Start\n"));
					ct_im_jpeg_1_4_2();
					Ddim_Print(("TEST 1_4_2 End\n"));
					break;
				case 3:
					Ddim_Print(("TEST 1_4_3 Start\n"));
					ct_im_jpeg_1_4_3();
					Ddim_Print(("TEST 1_4_3 End\n"));
					break;
				case 4:
					Ddim_Print(("TEST 1_4_4 Start\n"));
					ct_im_jpeg_1_4_4();
					Ddim_Print(("TEST 1_4_4 End\n"));
					break;
				case 5:
					Ddim_Print(("TEST 1_4_5 Start\n"));
					ct_im_jpeg_1_4_5();
					Ddim_Print(("TEST 1_4_5 End\n"));
					break;
				case 6:
					Ddim_Print(("TEST 1_4_6 Start\n"));
					ct_im_jpeg_1_4_6();
					Ddim_Print(("TEST 1_4_6 End\n"));
					break;
				case 7:
					Ddim_Print(("TEST 1_4_7 Start\n"));
					ct_im_jpeg_1_4_7();
					Ddim_Print(("TEST 1_4_7 End\n"));
					break;
				case 8:
					Ddim_Print(("TEST 1_4_8 Start\n"));
					ct_im_jpeg_1_4_8();
					Ddim_Print(("TEST 1_4_8 End\n"));
					break;
				case 9:
					Ddim_Print(("TEST 1_4_9 Start\n"));
					ct_im_jpeg_1_4_9();
					Ddim_Print(("TEST 1_4_9 End\n"));
					break;
				case 10:
					Ddim_Print(("TEST 1_4_10 Start\n"));
					ct_im_jpeg_1_4_10();
					Ddim_Print(("TEST 1_4_10 End\n"));
					break;
				case 11:
					Ddim_Print(("TEST 1_4_11 Start\n"));
					ct_im_jpeg_1_4_11();
					Ddim_Print(("TEST 1_4_11 End\n"));
					break;
				case 12:
					Ddim_Print(("TEST 1_4_12 empty number\n"));
					break;
				case 13:
					Ddim_Print(("TEST 1_4_13 Start\n"));
					ct_im_jpeg_1_4_13();
					Ddim_Print(("TEST 1_4_13 End\n"));
					break;
				case 14:
					Ddim_Print(("TEST 1_4_14 Start\n"));
					ct_im_jpeg_1_4_14();
					Ddim_Print(("TEST 1_4_14 End\n"));
					break;
				default:
					Ddim_Print(("input parameter error\n"));
					break;
			}
			break;

		case 5:
			if (ct_param_2 == 1) {
				Ddim_Print(("TEST 1_5_1 Start\n"));
				ct_im_jpeg_1_5_1();
				Ddim_Print(("TEST 1_5_1 End\n"));
			}
			else if (ct_param_2 == 2) {
				Ddim_Print(("TEST 1_5_2 Start\n"));
				ct_im_jpeg_1_5_2();
				Ddim_Print(("TEST 1_5_2 End\n"));
			}
			else {
				Ddim_Print(("input parameter error\n"));
			}
			break;

		case 6:	// Special

			Ddim_Print(("TEST 6 Start\n"));

			memset( &jpgenc_param, 0, sizeof(T_CT_IM_JPG_SPENC_PARAM) );

			jpgenc_param.jpeg_enc_tbl.ycc_smpl			= E_IM_JPEG_SMPL_TYPE_YCC422;
			jpgenc_param.jpeg_enc_tbl.mem_format		= E_IM_JPEG_MEM_FORM_PLANE_DOT;
			jpgenc_param.jpeg_enc_tbl.skip_mk_flg		= D_IM_JPEG_ENABLE_OFF;
			jpgenc_param.jpeg_enc_tbl.dri_mk_num		= 0;
			jpgenc_param.jpeg_enc_tbl.exif_fmt_flg		= D_IM_JPEG_ENABLE_OFF;
			jpgenc_param.jpeg_enc_tbl.pint_line			= 0;
			jpgenc_param.jpeg_enc_tbl.pint_sect			= 0;
			jpgenc_param.jpeg_enc_tbl.pbuf_ctrl			= gCT_Im_Jpeg_Pbuf_Ctrl;
			jpgenc_param.jpeg_enc_tbl.jbuf_ctrl			= gCT_Im_Jpeg_Jbuf_Ctrl;
			jpgenc_param.jpeg_enc_tbl.bit_depth			= E_IM_JPEG_BIT_DEPTH_8BIT;
			jpgenc_param.jpeg_enc_tbl.pburst_length		= E_IM_JPEG_BURST_INCR_8;
			jpgenc_param.jpeg_enc_tbl.jburst_length		= E_IM_JPEG_BURST_INCR_8;
			jpgenc_param.jpeg_enc_tbl.width				= D_CT_JPEG_SIZE_16M_H;
			jpgenc_param.jpeg_enc_tbl.lines				= D_CT_JPEG_SIZE_16M_V;
			// Y Component parameter
			jpgenc_param.jpeg_enc_tbl.component[0].quant_tbl_no  = 0;	// Select Qunatization table No. for Y	
			jpgenc_param.jpeg_enc_tbl.component[0].huf_dc_tbl_no = 0;	// Select DC Huffman table No. for Y	
			jpgenc_param.jpeg_enc_tbl.component[0].huf_ac_tbl_no = 0;	// Select AC Huffman table No. for Y	
			// Cb Component parameter
			jpgenc_param.jpeg_enc_tbl.component[1].quant_tbl_no  = 1;	// Select Qunatization table No. for Cb	
			jpgenc_param.jpeg_enc_tbl.component[1].huf_dc_tbl_no = 1;	// Select DC Huffman table No. for Cb	
			jpgenc_param.jpeg_enc_tbl.component[1].huf_ac_tbl_no = 1;	// Select AC Huffman table No. for Cb	
			// Cr Component parameter
			jpgenc_param.jpeg_enc_tbl.component[2].quant_tbl_no  = 1;	// Select Qunatization table No. for Cr	
			jpgenc_param.jpeg_enc_tbl.component[2].huf_dc_tbl_no = 1;	// Select DC Huffman table No. for Cr	
			jpgenc_param.jpeg_enc_tbl.component[2].huf_ac_tbl_no = 1;	// Select AC Huffman table No. for Cr	

			// Address
			jpgenc_param.jpeg_enc_frame_tbl.ycc_addr.y		= D_CT_JPEG_YCC_ADDR;
			jpgenc_param.jpeg_enc_frame_tbl.ycc_addr.c		= D_CT_JPEG_YCC_ADDR + (jpgenc_param.jpeg_enc_tbl.width * jpgenc_param.jpeg_enc_tbl.lines);
			jpgenc_param.jpeg_enc_frame_tbl.code_addr		= D_CT_JPEG_CODE_ADDR;
			// Size
			jpgenc_param.jpeg_enc_frame_tbl.global_y_width	= D_CT_JPEG_SIZE_16M_H;
			jpgenc_param.jpeg_enc_frame_tbl.global_c_width	= D_CT_JPEG_SIZE_16M_H;
			jpgenc_param.jpeg_enc_frame_tbl.code_count_flg	= D_IM_JPEG_ENABLE_OFF;	// dummy value
			jpgenc_param.jpeg_enc_frame_tbl.code_over_count_flg	= D_IM_JPEG_ENABLE_OFF;
			jpgenc_param.limit_size							= ((D_CT_JPEG_SIZE_16M_H + 16) * D_CT_JPEG_SIZE_16M_V) / 2.6;
			jpgenc_param.mode								= T_CT_IM_JPG_SPENC_MODE_CAPTURE;

			// Quality
			jpgenc_param.quality							= ct_im_jpeg_get_encode_quality( CR_4_0 );

			gJpeg_Encode_Size =  CT_Im_Jpeg_Special_Encode( &jpgenc_param );
			gJpeg_OutputBytes = gJpeg_Encode_Size;

			Ddim_Print(("JPEG encode size [0x%lX]\n", gJpeg_Encode_Size));
			Ddim_Print(("TEST 6 End\n"));

			break;

		default:
			Ddim_Print(("input parameter error\n"));
			break;
	}

	return;
}

VOID Ct_Im_Jpeg_Run_2( INT32 ct_param_1, INT32 ct_param_2 )
{

	Ddim_Print(( "*** Jpeg begin Im_Jpeg_Init\n" ));
	Im_JPEG_Init();
	Ddim_Print(( "*** Jpeg end Im_Jpeg_Init\n" ));

	switch (ct_param_1) {
		case 1:
			if (ct_param_2 == 1) {
				Ddim_Print(("TEST 2_1_1 Start\n"));
				ct_im_jpeg_2_1_1();
				Ddim_Print(("TEST 2_1_1 End\n"));
			}
			else {
				Ddim_Print(("input parameter error\n"));
			}
			break;
		case 2:
			if (ct_param_2 == 1) {
				Ddim_Print(("TEST 2_2_1 Start\n"));
				ct_im_jpeg_2_2_1();
				Ddim_Print(("TEST 2_2_1 End\n"));
			}
			else {
				Ddim_Print(("input parameter error\n"));
			}
			break;
		case 3:
			if (ct_param_2 == 1) {
				Ddim_Print(("TEST 2_3_1 Start\n"));
				ct_im_jpeg_2_3_1();
				Ddim_Print(("TEST 2_3_1 End\n"));
			}
			else if (ct_param_2 == 2) {
				Ddim_Print(("TEST 2_3_2 Start\n"));
				ct_im_jpeg_2_3_2();
				Ddim_Print(("TEST 2_3_2 End\n"));
			}
			else {
				Ddim_Print(("input parameter error\n"));
			}
			break;
		case 4:
			if (ct_param_2 <= 12) {
				Ddim_Print(("TEST 2_4_1 Start\n"));
				ct_im_jpeg_check_handler( ct_param_2 );
				Ddim_Print(("TEST 2_4_1 End\n"));
			}
			else {
				Ddim_Print(("input parameter error\n"));
			}
			break;
		default:
			Ddim_Print(("input parameter error\n"));
			break;
	}
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Command main function for JPEG test.
 * @param  INT32 argc	:The number of parameters
 *		   CGAR** argv	:The value of parameters
 * @return VOID
 */
VOID Ct_Im_Jpeg_Main( INT32 argc, CHAR** argv )
{
	INT32	ct_param1 = 0;
	INT32	ct_param2 = 0;
	INT32	ct_param3 = 0;
	INT32	ct_param4 = 0;
	INT32	ct_param5 = 0;
	INT32	ct_param6 = 0;

	if ( argc > 8 ) {
		Ddim_Print(("input parameter error. argc=%d\n", argc));
		return;
	}

	if( argc >= 2 ) {
		ct_param1 = atoi((const char *)argv[1]);
	}
	if( argc >= 3 ) {
		ct_param2 = atoi((const char *)argv[2]);
	}
	if( argc >= 4 ) {
		ct_param3 = atoi((const char *)argv[3]);
	}
	if( argc >= 5 ) {
		ct_param4 = atoi((const char *)argv[4]);
	}

	gJpeg_AXI_Err = 0;
	gJpeg_OutputBytes = 0;

	if ( ct_param1 == 1 ) {
		Ct_Im_Jpeg_Run_1( ct_param2, ct_param3 );
	}
	else if ( ct_param1 == 2 ) {
		Ct_Im_Jpeg_Run_2( ct_param2, ct_param3 );
	}
	else if ( ct_param1 == 3 ) {
	// Encode
		T_CT_JPEG_ENC_PARAM enc_param;

		enc_param.sync			= 1;
		enc_param.g_y_width		= ct_param3;
		enc_param.g_y_lines		= ct_param4;
		enc_param.h_size		= ct_param3;
		enc_param.v_size		= ct_param4;
		// AXI_Err happen
		gJpeg_AXI_Err			= atoi((const char *)argv[5]);
		// Quality value use or Qtable set
		ct_param5	 			= atoi((const char *)argv[6]);
		// skip marker mode use
		ct_param6	 			= atoi((const char *)argv[7]);

		if ( ct_param2 == 0 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_444P;
			enc_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 1 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_422P;
			enc_param.g_c_width	= (ULONG)(ct_param3 >> 1);
		}
		else if( ct_param2 == 2 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_422PD;
			enc_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 3 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_420P;
			enc_param.g_c_width	= (ULONG)(ct_param3 >> 1);
		}
		else if( ct_param2 == 4 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_420PD;
			enc_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 5 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_400P;
			enc_param.g_c_width	= 0;
		}
		else {
			Ddim_Print(("input parameter error. argc=%d\n", argc));
			return;
		}

		enc_param.limit_size	= 0;
		enc_param.pause_flg		= 0;
		enc_param.count_flg		= 0;
		enc_param.quantup_flg	= 0;
		enc_param.dri			= 0;
		enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;
		enc_param.skip_mk_flg	= ct_param6;

		Im_JPEG_Init();

		ct_jpeg_encode( &enc_param, ct_param5 );

	}
	else if ( ct_param1 == 4 ) {
	// Decode
		T_CT_JPEG_DEC_PARAM dec_param;

//		memset( (void *)0x49000000 ,0, 0x10000000 );

//		Im_JPEG_Init();

		memset( &dec_param, 0, sizeof(dec_param) );

		dec_param.sync			= 1;
		dec_param.g_y_width		= ct_param3;
		dec_param.g_y_lines		= ct_param4;
		dec_param.h_size		= ct_param3;
		dec_param.v_size		= ct_param4;
		// AXI_Err happen
		gJpeg_AXI_Err			= atoi((const char *)argv[5]);
		// skip marker mode use
		ct_param5				= atoi((const char *)argv[6]);

		if ( ct_param2 == 0 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_444P;
			dec_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 1 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_422P;
			dec_param.g_c_width	= (ULONG)(ct_param3 >> 1);
		}
		else if( ct_param2 == 2 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_422PD;
			dec_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 3 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_420P;
			dec_param.g_c_width	= (ULONG)(ct_param3 >> 1);
		}
		else if( ct_param2 == 4 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_420PD;
			dec_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 5 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_400P;
			dec_param.g_c_width	= 0;
		}
		else {
			Ddim_Print(("input parameter error. argc=%d\n", argc));
			return;
		}

		dec_param.cutout_flg	= 0;
		dec_param.cut_h_size	= 0;
		dec_param.cut_v_size	= 0;
		dec_param.limit_size	= 0;
		dec_param.ext			= E_IM_JPEG_RESIZE_EXT_DIRECT;
		dec_param.corr_flg		= 1;
		dec_param.pause_flg		= 0;

		if ( ct_param5 > 0 ) {
		// marker skip mode test
			ct_jpeg_decode_mk_skip( &dec_param );
		}
		else {
		// normal test
			ct_jpeg_decode( &dec_param );
		}
	}
	else if ( ct_param1 == 5 ) {
	// cont Decode
		T_CT_JPEG_DEC_PARAM dec_param;
		INT32 i;
		INT32 cnt_max = 1;

		dec_param.sync			= 1;
		dec_param.g_y_width		= ct_param3;
		dec_param.g_y_lines		= ct_param4;
		dec_param.h_size		= ct_param3;
		dec_param.v_size		= ct_param4;
		gJpeg_AXI_Err = atoi((const char *)argv[5]);
		cnt_max = atoi((const char *)argv[6]);

		if ( ct_param2 == 0 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_444P;
			dec_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 1 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_422P;
			dec_param.g_c_width	= (ULONG)(ct_param3 >> 1);
		}
		else if( ct_param2 == 2 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_422PD;
			dec_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 3 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_420P;
			dec_param.g_c_width	= (ULONG)(ct_param3 >> 1);
		}
		else if( ct_param2 == 4 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_420PD;
			dec_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 5 ) {
			dec_param.format	= E_CT_JPEG_FORMAT_400P;
			dec_param.g_c_width	= 0;
		}
		else {
			Ddim_Print(("input parameter error. argc=%d\n", argc));
			return;
		}

		dec_param.cutout_flg	= 0;
		dec_param.cut_h_size	= 0;
		dec_param.cut_v_size	= 0;
		dec_param.limit_size	= 0;
		dec_param.ext			= E_IM_JPEG_RESIZE_EXT_BAND;
		dec_param.corr_flg		= 1;
		dec_param.pause_flg		= 0;

		for( i=0; i < cnt_max; i++ ) {
//			Im_JPEG_Init();
			ct_jpeg_decode( &dec_param );
		}
	}
	else if ( ct_param1 == 6 ) {

		T_IM_JPEG_ENC_MNG jpeg_mng;
		USHORT ret_ratio;
		INT32 ret;
		UINT32 dsp;

		if ( ct_param2 == 0 ) {
			jpeg_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC420;
		}
		else if( ct_param2 == 1 ) {
			jpeg_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC422;
		}
		else if( ct_param2 == 2 ) {
			jpeg_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC444;
		}
		else if( ct_param2 == 3 ) {
			jpeg_mng.ycc_smpl	= E_IM_JPEG_SMPL_TYPE_YCC400;
		}
		else {
			Ddim_Print(("input parameter error. argc=%d\n", argc));
			return;
		}

		if ( ct_param3 == 0 ) {
			jpeg_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE;
		}
		else if( ct_param3 == 1 ) {
			jpeg_mng.mem_format	= E_IM_JPEG_MEM_FORM_PLANE_DOT;
		}
		else {
			Ddim_Print(("input parameter error. argc=%d\n", argc));
			return;
		}

		if ( ct_param4 == 0 ) {
			dsp	= D_IM_JPEG_DOWNSP_NONE;
		}
		else if( ct_param4 == 1 ) {
			dsp	= D_IM_JPEG_DOWNSP_1_2;
		}
		else if( ct_param4 == 2 ) {
			dsp	= D_IM_JPEG_DOWNSP_1_4;
		}
		else if( ct_param4 == 3 ) {
			dsp	= D_IM_JPEG_DOWNSP_1_8;
		}
		else {
			Ddim_Print(("input parameter error. argc=%d\n", argc));
			return;
		}

		jpeg_mng.width						= 0xFFFF;
		jpeg_mng.lines						= 0xFFFF;
		jpeg_mng.component[0].quant_tbl_no	= 0;
		jpeg_mng.component[0].huf_dc_tbl_no	= 0;
		jpeg_mng.component[0].huf_ac_tbl_no	= 0;
		jpeg_mng.component[1].quant_tbl_no	= 1;
		jpeg_mng.component[1].huf_dc_tbl_no	= 1;
		jpeg_mng.component[1].huf_ac_tbl_no	= 1;
		jpeg_mng.component[2].quant_tbl_no	= 2;
		jpeg_mng.component[2].huf_dc_tbl_no	= 1;
		jpeg_mng.component[2].huf_ac_tbl_no	= 1;
		jpeg_mng.skip_mk_flg				= D_IM_JPEG_ENABLE_OFF;
		jpeg_mng.dri_mk_num					= 8;
		jpeg_mng.exif_fmt_flg				= D_IM_JPEG_ENABLE_OFF;
		jpeg_mng.pint_line					= 0;
		jpeg_mng.pint_sect					= 0;
		jpeg_mng.pbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
		jpeg_mng.pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
		jpeg_mng.pbuf_ctrl.cache_type		= 0;
		jpeg_mng.pbuf_ctrl.prot_type		= 0;
		jpeg_mng.jbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE;
		jpeg_mng.jbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_8;
		jpeg_mng.jbuf_ctrl.cache_type		= 0;
		jpeg_mng.jbuf_ctrl.prot_type		= 0;
		jpeg_mng.jburst_length				= E_IM_JPEG_BURST_INCR_8;
		jpeg_mng.pburst_length				= E_IM_JPEG_BURST_INCR_8;
		jpeg_mng.bit_depth					= E_IM_JPEG_BIT_DEPTH_8BIT;
		jpeg_mng.code_size					= 0;
		jpeg_mng.result						= 0;
		jpeg_mng.pcallback 					= (VP_CALLBACK)ct_jpeg_encode_cb;

		ret = Im_JPEG_Open( D_DDIM_WAIT_END_TIME );
		if ( ret != D_IM_JPEG_OK ) {
			Ddim_Print(("Im_JPEG_Open error ret=0x%X\n", ret));
			return;
		}

		Im_JPEG_Ctrl_Enc(&jpeg_mng);

		ret_ratio = Im_JPEG_Set_Down_Sampling_Rate( dsp );
		Ddim_Print(("Im_JPEG_Set_Down_Sampling_Rate=0x%X\n", ret_ratio));

		ct_jpeg_start_hclock();
		Ddim_Print(("IO_JPG7.JICTL.bit.JIXP=0x%X\n", IO_JPG7.JICTL.bit.JIXP));
		Ddim_Print(("IO_JPG7.JIMGSH.bit.JIMGSH=%d\n", IO_JPG7.JIMGSH.bit.JIMGSH));
		Ddim_Print(("IO_JPG7.JPWIDTH.bit.JPWIDTH=%d\n", IO_JPG7.JPWIDTH.bit.JPWIDTH));
		Ddim_Print(("IO_JPG7.JIMGSV.bit.JIMGSV=%d\n", IO_JPG7.JIMGSV.bit.JIMGSV));
		Ddim_Print(("IO_JPG7.JPHEIGHT.bit.JPHEIGHT=%d\n", IO_JPG7.JPHEIGHT.bit.JPHEIGHT));
		ct_jpeg_stop_hclock();

		ret = Im_JPEG_Close();
		if ( ret != D_IM_JPEG_OK ) {
			Ddim_Print(("Im_JPEG_Close error ret=0x%X\n", ret));
			return;
		}
	}
	else if ( ct_param1 == 7 ) {
	// Encode
		T_CT_JPEG_ENC_PARAM enc_param;
		INT32	i;

		enc_param.sync			= 1;
		enc_param.g_y_width		= ct_param3;
		enc_param.g_y_lines		= ct_param4;
		enc_param.h_size		= ct_param3;
		enc_param.v_size		= ct_param4;
		// AXI_Err happen
		gJpeg_AXI_Err			= atoi((const char *)argv[5]);
		// Quality value use or Qtable set
		ct_param5	 			= atoi((const char *)argv[6]);
		// skip marker mode use
		ct_param6	 			= atoi((const char *)argv[7]);

		if ( ct_param2 == 0 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_444P;
			enc_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 1 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_422P;
			enc_param.g_c_width	= (ULONG)(ct_param3 >> 1);
		}
		else if( ct_param2 == 2 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_422PD;
			enc_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 3 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_420P;
			enc_param.g_c_width	= (ULONG)(ct_param3 >> 1);
		}
		else if( ct_param2 == 4 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_420PD;
			enc_param.g_c_width	= (ULONG)ct_param3;
		}
		else if( ct_param2 == 5 ) {
			enc_param.format	= E_CT_JPEG_FORMAT_400P;
			enc_param.g_c_width	= 0;
		}
		else {
			Ddim_Print(("input parameter error. argc=%d\n", argc));
			return;
		}

		enc_param.limit_size	= 0;
		enc_param.pause_flg		= 0;
		enc_param.count_flg		= 0;
		enc_param.quantup_flg	= 0;
		enc_param.dri			= 0;
		enc_param.dsp_rate		= D_IM_JPEG_DOWNSP_NONE;
		enc_param.skip_mk_flg	= ct_param6;

		Im_JPEG_Init();

		for ( i=0; i<1000; i++ ) {
			ct_jpeg_encode( &enc_param, ct_param5 );
		}
	}
	else if ( ct_param1 == 8 ) {
		Ct_Im_Jpeg_Run_1( 1, 1 );
		Ct_Im_Jpeg_Run_1( 2, 1 );
		Ct_Im_Jpeg_Run_1( 2, 2 );
		Ct_Im_Jpeg_Run_1( 3, 1 );
		Ct_Im_Jpeg_Run_1( 3, 2 );
		Ct_Im_Jpeg_Run_1( 3, 3 );
		Ct_Im_Jpeg_Run_1( 3, 4 );
		Ct_Im_Jpeg_Run_1( 3, 5 );
		Ct_Im_Jpeg_Run_1( 3, 6 );
		Ct_Im_Jpeg_Run_1( 3, 7 );
		Ct_Im_Jpeg_Run_1( 3, 8 );
		Ct_Im_Jpeg_Run_1( 3, 9 );
		Ct_Im_Jpeg_Run_1( 3, 10 );
		Ct_Im_Jpeg_Run_1( 3, 11 );
		Ct_Im_Jpeg_Run_1( 3, 12 );
		Ct_Im_Jpeg_Run_1( 3, 13 );
		Ct_Im_Jpeg_Run_1( 3, 14 );
		Ct_Im_Jpeg_Run_1( 3, 15 );
		Ct_Im_Jpeg_Run_1( 3, 16 );
		Ct_Im_Jpeg_Run_1( 3, 17 );
		Ct_Im_Jpeg_Run_1( 3, 18 );
		Ct_Im_Jpeg_Run_1( 3, 19 );
		Ct_Im_Jpeg_Run_1( 3, 20 );
		Ct_Im_Jpeg_Run_1( 3, 21 );
		Ct_Im_Jpeg_Run_1( 4, 1 );
		Ct_Im_Jpeg_Run_1( 4, 2 );
		Ct_Im_Jpeg_Run_1( 4, 3 );
		Ct_Im_Jpeg_Run_1( 4, 4 );
		Ct_Im_Jpeg_Run_1( 4, 5 );
		Ct_Im_Jpeg_Run_1( 4, 6 );
		Ct_Im_Jpeg_Run_1( 4, 7 );
		Ct_Im_Jpeg_Run_1( 4, 8 );
		Ct_Im_Jpeg_Run_1( 4, 9 );
		Ct_Im_Jpeg_Run_1( 4, 10 );
		Ct_Im_Jpeg_Run_1( 4, 11 );
		Ct_Im_Jpeg_Run_1( 4, 12 );
		Ct_Im_Jpeg_Run_1( 4, 13 );
		Ct_Im_Jpeg_Run_1( 4, 14 );
		Ct_Im_Jpeg_Run_1( 4, 15 );
		Ct_Im_Jpeg_Run_1( 4, 16 );
		Ct_Im_Jpeg_Run_1( 4, 17 );
		Ct_Im_Jpeg_Run_1( 4, 18 );
		Ct_Im_Jpeg_Run_1( 5, 1 );
		Ct_Im_Jpeg_Run_1( 5, 2 );

		// Special Encode for HIIRE
		Ct_Im_Jpeg_Run_1( 6, 1 );

		Ct_Im_Jpeg_Run_2( 1, 1 );
		Ct_Im_Jpeg_Run_2( 2, 1 );
		Ct_Im_Jpeg_Run_2( 3, 1 );
		Ct_Im_Jpeg_Run_2( 3, 2 );
		Ct_Im_Jpeg_Run_2( 4, 1 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 2 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 3 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 4 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 5 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 6 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 7 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 8 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 9 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 10 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 11 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 12 );	// Int_Handler PC check
		Ct_Im_Jpeg_Run_2( 4, 13 );	// Int_Handler PC check
	}
	else {
		Ddim_Print(("input parameter error\n"));
	}

#ifdef CO_ACT_JPEG_CLOCK
	if (gCt_IM_Jpg_Clk_Ctrl_Cnt != 0) {
		Ddim_Print(("Jpeg clock Error gCt_IM_Jpg_Clk_Ctrl_Cnt=%d\n", gCt_IM_Jpg_Clk_Ctrl_Cnt));
	}
	else {
		Ddim_Print(("Jpeg clock OK\n"));
	}
#endif // CO_ACT_JPEG_CLOCK

#ifdef CO_ACT_JPEG_HCLOCK
	if (gCt_IM_Jpg_Hclk_Ctrl_Cnt != 0) {
		Ddim_Print(("Jpeg Hclock Error gCt_IM_Jpg_Hclk_Ctrl_Cnt=%d\n", gCt_IM_Jpg_Hclk_Ctrl_Cnt));
	}
	else {
		Ddim_Print(("Jpeg Hclock OK\n"));
	}
#endif // CO_ACT_JPEG_HCLOCK

#ifdef CO_ACT_JPEG_ICLOCK
	if (gCt_IM_Jpg_Iclk_Ctrl_Cnt != 0) {
		Ddim_Print(("Jpeg Iclock Error gCt_IM_Jpg_Iclk_Ctrl_Cnt=%d\n", gCt_IM_Jpg_Iclk_Ctrl_Cnt));
	}
	else {
		Ddim_Print(("Jpeg Iclock OK\n"));
	}
#endif // CO_ACT_JPEG_ICLOCK

	return;

}

ULONG Ct_Im_Jpeg_Get_OutputBytes( VOID )
{
	return gJpeg_OutputBytes;
}

ULONG Ct_Im_Jpeg_Get_YccAddr( VOID )
{
	return D_CT_JPEG_YCC_ADDR;
}

ULONG Ct_Im_Jpeg_Get_CodeAddr( VOID )
{
	return D_CT_JPEG_CODE_ADDR;
}

