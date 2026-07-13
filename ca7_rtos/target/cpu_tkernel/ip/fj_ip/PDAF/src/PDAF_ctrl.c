/**
 * @file		PDAF_ctrl.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */


#include "PDAF_ctrl.h"
#include "wrap_pdaflib.h"
#include "share_data.h"
#include "fj_filesystem.h"
#include "sdram_map_define.h"
#include "l1l2cache.h"
#include "fj_host_debug.h"
#include "fj_memory.h"

#include <string.h>

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define PD_H_SIZE	224		// temporarily
#define PD_V_SIZE	72		// temporarily
#define PD_MAX_SIZE	16128	// 224 x 72 8bit

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
//#define D_PD_DEBUG_SAVE_PD_DATA

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static UCHAR gPdL[PD_MAX_SIZE];
static UCHAR gPdR[PD_MAX_SIZE];
static UCHAR gPdW[0x20000];

static ImageData	g_pdaf_in = {
	.width				= 224,				// user set
	.height				= 72,				// user set
	.pixL				= (IMG_BYTES*)gPdL,	// user set
	.pixR				= (IMG_BYTES*)gPdR,	// user set
	.flg_defect			= 0,
	.bayer_ptn			= 2,
	.flg_inputPattern	= IMGDATA_INPUT_PHASE_LR,
	.flg_pixArrange		= PIX_ARRANGE_DUAL,
	.proc_x				= 0,
	.proc_y				= 0,
	.proc_w				= 224,				// user set
	.proc_h				= 72,				// user set
	.window_size		= 112,
	.window_interval	= 64,
//	.shift_len_max		= 16,
	.shift_len_max		= 24,
	.pix_ptn_shift_x	= 0,
	.pix_ptn_shift_y	= 0,
	.window_size_v		= 4,				// unused
	.window_interval_v	= 16,				// unused
	.pdaf_interval_x	= 2,				// unused
	.flg_normalize		= 1,
	.calcShiftAmount_ver= CSA_V6_1,
	.CSA_detectLine		= 0,
	.flg_detectDefPixs	= 0,
	.thrL_detectDefPixs	= 0,
	.thrR_detectDefPixs	= 0,
	.coef_detectDefPixs	= 0.0,
	.std_detectDefPixs	= 0.0,
	.PTN_w				= 2,
	.PTN_h				= 4,
	.PTN				= NULL,
	.shift_bits_SAD		= 0,
	.p_mem				= (VOID*)0,		// user set
	.size_mem			= (UINT32)(0),	// user set ( need >= 0x20000 )
};

static ResultData	g_pdaf_out;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 pdaf_make_lr( T_SHARE_AF_DATA *in_data );
static VOID pdaf_calc_target_position( FLOAT now_pos, T_PDAF_RESULT_DETAILS *result );
#ifdef D_PD_DEBUG_SAVE_PD_DATA
static void debug_save_file( CHAR* fname, ULONG addr, ULONG size );
#endif

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
INT32 PDAF_Ctrl_Test( VOID )
{
	INT32	ret;
	CHAR*	memL;
	CHAR*	memR;
	CHAR*	memW;
	
	// Read Sample Image from SD Card
	{
		INT32 ercd;
		ULONG fid;
		ULONG size = g_pdaf_in.width * g_pdaf_in.height;
		ULONG length;
		
		// L image
		memL = (CHAR*)SDRAM_ADR_CH1_RTOS_IMAGE_WORK;
		fid = FJ_Fs_Open( "I:\\PDAF_Sample0_L.raw", FJ_FS_OPEN_RDONLY );
		ercd = FJ_Fs_Read( fid, (UINT32*)memL, size, &length);
		UPRINTF_ERR(ercd);
		FJ_Fs_Close( fid );
		BF_L1l2cache_Flush_Addr( (UINT32)memL, length );	// for safety
		// R image
		memR = (CHAR*)( ((ULONG)memL + length + 3) & 0xFFFFFFFC );
		fid = FJ_Fs_Open( "I:\\PDAF_Sample0_R.raw", FJ_FS_OPEN_RDONLY );
		ercd = FJ_Fs_Read( fid, (UINT32*)memR, size, &length);
		UPRINTF_ERR(ercd);
		FJ_Fs_Close( fid );
		BF_L1l2cache_Flush_Addr( (UINT32)memR, length );	// for safety
		// Work Memory
		memW = (CHAR*)( ((ULONG)memR + length + 3) & 0xFFFFFFFC );
	}
	
	// set init
	g_pdaf_in.pixL					= (IMG_BYTES*)memL;
	g_pdaf_in.pixR					= (IMG_BYTES*)memR;
	g_pdaf_in.p_mem					= (VOID*)memW;
	g_pdaf_in.size_mem				= (UINT32)(0x20000); // 0x20000
	
	// clear output
	memset( &g_pdaf_out, 0, sizeof( ResultData ) );
	
	// init
	ret = wrap_EstimateShiftAmounts( &g_pdaf_in, &g_pdaf_out,  ESA_INIT);
	UPRINTF_ERR( ret );
	
	// exec
	ret = wrap_EstimateShiftAmounts( &g_pdaf_in, &g_pdaf_out,  ESA);
	UPRINTF_ERR( ret );
	
	// close
	ret = wrap_EstimateShiftAmounts( &g_pdaf_in, &g_pdaf_out,  ESA_CLOSE);
	UPRINTF_ERR( ret );
	
	printf( "PDAF Result \n" );
	printf( "  shift_amount  = %f\n", (float)g_pdaf_out.shift_amount );
	printf( "  reliability   = %f\n", (float)g_pdaf_out.reliability );
	printf( "  square_rate   = %f\n", (float)g_pdaf_out.square_rate );
	printf( "  shift_amount2 = %f\n", (float)g_pdaf_out.shift_amount2 );
	printf( "  reliability2  = %f\n", (float)g_pdaf_out.reliability2 );
	printf( "  square_rate2  = %f\n", (float)g_pdaf_out.square_rate2 );
	printf( "  HF_rate      = %f\n", (float)g_pdaf_out.HF_rate );
	
	return ret;
}


INT32 PDAF_Ctrl( T_SHARE_AF_DATA *in_data, FLOAT now_pos, T_PDAF_RESULT_DETAILS *result )
{
	// Temporary solution
	//  - Now, this function calculates PDAF shift value by only CPU without BMH macro.
	// ToDo
	//  1. Please use BMH macro for fast processing.
	//  2. For HYBRID AF, please set absolutely same area with contrast WD area.
	
	INT32	ret = OK;
//	printf( "PDAF_Ctrl is called (now:%fum)\n", now_pos );
	ret = pdaf_make_lr( in_data );
	
	if( ret != OK)
	{
		return ret;
	}
	
	// set init
	g_pdaf_in.p_mem					= (VOID*)gPdW;
	g_pdaf_in.size_mem				= (UINT32)(0x20000); // 0x20000
	
	// clear output
	memset( &g_pdaf_out, 0, sizeof( ResultData ) );
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AF, FJ_MBALOG_FUNC_ID_AF, FJ_MBALOG_TYPE_START, "PDAF Lib Init"));
	// init
	ret = wrap_EstimateShiftAmounts( &g_pdaf_in, &g_pdaf_out,  ESA_INIT);
	UPRINTF_ERR( ret );
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AF, FJ_MBALOG_FUNC_ID_AF, FJ_MBALOG_TYPE_END, "PDAF Lib Init"));
	
	// exec
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AF, FJ_MBALOG_FUNC_ID_AF, FJ_MBALOG_TYPE_START, "PDAF Lib Exec"));
	ret = wrap_EstimateShiftAmounts( &g_pdaf_in, &g_pdaf_out,  ESA);
	UPRINTF_ERR( ret );
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AF, FJ_MBALOG_FUNC_ID_AF, FJ_MBALOG_TYPE_END, "PDAF Lib Exec"));
	
	// close
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AF, FJ_MBALOG_FUNC_ID_AF, FJ_MBALOG_TYPE_START, "PDAF Lib Close"));
	ret = wrap_EstimateShiftAmounts( &g_pdaf_in, &g_pdaf_out,  ESA_CLOSE);
	UPRINTF_ERR( ret );
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AF, FJ_MBALOG_FUNC_ID_AF, FJ_MBALOG_TYPE_END, "PDAF Lib Close"));
	
/*	printf( "PDAF Result \n" );
	printf( "  call result   = %x\n", ret );
	printf( "  shift_amount  = %f\n", (float)g_pdaf_out.shift_amount );
	printf( "  reliability   = %f\n", (float)g_pdaf_out.reliability );
	printf( "  square_rate   = %f\n", (float)g_pdaf_out.square_rate );
	printf( "  shift_amount2 = %f\n", (float)g_pdaf_out.shift_amount2 );
	printf( "  reliability2  = %f\n", (float)g_pdaf_out.reliability2 );
	printf( "  square_rate2  = %f\n", (float)g_pdaf_out.square_rate2 );
	printf( "  HF_rate      = %f\n", (float)g_pdaf_out.HF_rate );
*/	
	pdaf_calc_target_position( now_pos, result );
	
	return ret;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/

INT32 pdaf_make_lr( T_SHARE_AF_DATA *in_data )
{
	// Temporary solution
	//  - Now, this function picks up PD pixels of center area by using CPU without SHD conpensation.
	// ToDo
	//  1. Please Pick up PD pixels by using HW Macro for fast operation(now, using CPU).
	//  2. If necessary, please pick up PD pixels of ROI area(now, only center pixels).
	//  3. SHD conpensation for PD pixel by using calibration data.
	
	USHORT	*src;
	UCHAR	*dst;
	UINT32	hcnt, vcnt;
	UINT32	cut_x = PD_H_SIZE;
	UINT32	cut_y = PD_V_SIZE;
	UINT32	offset_x = (( (in_data->pdaf.DUAL_PD_DATA.width>>1) - cut_x ) >> 1);
	UINT32	offset_y = (( in_data->pdaf.DUAL_PD_DATA.height - cut_y ) >> 1);
#ifdef D_PD_DEBUG_SAVE_PD_DATA
	static UINT32 f_cnt = 0;
	CHAR fname[32];
#endif
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AF, FJ_MBALOG_FUNC_ID_AF, FJ_MBALOG_TYPE_START, "PDAF make PD data"));
	
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_FLUSH,
		  (UINT32)(in_data->pdaf.DUAL_PD_DATA.addr), in_data->pdaf.DUAL_PD_DATA.g_width * in_data->pdaf.DUAL_PD_DATA.height);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_FLUSH,
		  (UINT32)(gPdL), PD_H_SIZE * PD_V_SIZE);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_FLUSH,
		  (UINT32)(gPdR), PD_H_SIZE * PD_V_SIZE);
	
	src = (USHORT*)in_data->pdaf.DUAL_PD_DATA.addr + in_data->pdaf.DUAL_PD_DATA.width * offset_y + offset_x * 2 + 1;
	dst = gPdL;
	
	if( (in_data->pdaf.DUAL_PD_DATA.addr * in_data->pdaf.DUAL_PD_DATA.width * in_data->pdaf.DUAL_PD_DATA.height) == 0 ){
		return NG;
	}
	
	for( vcnt=0; vcnt<cut_y; vcnt++ ){
		for( hcnt=0; hcnt<cut_x; hcnt++ ){
			*dst = (UCHAR)(*src>>2);
			src+=2;
			dst++;
		}
		src += ( in_data->pdaf.DUAL_PD_DATA.width - cut_x*2 );
	}
	
#ifdef D_PD_DEBUG_SAVE_PD_DATA
	sprintf( fname, "I:\\PD_DATA_L_center_%d.raw", f_cnt );
	debug_save_file(fname, (ULONG)gPdL, cut_x*cut_y);
	UPRINTF("Size: %d x %d (Ldata)\n", cut_x, cut_y);
#endif

	src = (USHORT*)in_data->pdaf.DUAL_PD_DATA.addr + in_data->pdaf.DUAL_PD_DATA.width * offset_y + offset_x*2;
	dst = gPdR;
	
	for( vcnt=0; vcnt<cut_y; vcnt++ ){
		for( hcnt=0; hcnt<cut_x; hcnt++ ){
			*dst = (UCHAR)(*src>>2);
			src+=2;
			dst++;
		}
		src += ( in_data->pdaf.DUAL_PD_DATA.width - cut_x*2 );
	}
	
#ifdef D_PD_DEBUG_SAVE_PD_DATA
	sprintf( fname, "I:\\PD_DATA_R_center_%d.raw", f_cnt );
	debug_save_file(fname, (ULONG)gPdR, cut_x*cut_y);
	UPRINTF("Size: %d x %d (Ldata)\n", cut_x, cut_y);
	f_cnt++;
#endif
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AF, FJ_MBALOG_FUNC_ID_AF, FJ_MBALOG_TYPE_END, "PDAF make PD data"));
	return OK;
}

static VOID pdaf_calc_target_position( FLOAT now_pos, T_PDAF_RESULT_DETAILS *result )
{
	// Temporary solution
	//  - This function use the simplest method for estimation of target position now.
	// ToDo
	//  1. Please use calibration data for estimation of target position.
	//     Please refer to MeituV6 for implementation.
	
	
	// ƒ¢Shift *0.935 = ƒ¢LensLength
	
	float lens_shift = (float)g_pdaf_out.shift_amount / 0.0935;
	
	// Temporary measure for click noise 1
	if( now_pos + lens_shift > 200 ){
		if( lens_shift > 20.0 ){
			lens_shift = 20.0;
		}
	}
	
	result -> PdafPos = lens_shift + now_pos;
	result -> ConfidenceLevel = (ULONG)(g_pdaf_out.reliability * 1024);
	
	// Temporary measure for click noise 2
	if( (result -> PdafPos < 10) && (now_pos > 100.0) ){
		result -> PdafPos = 10.0;
	}
	if( (result -> PdafPos > 180) && (now_pos < 150.0) ){
		result -> PdafPos = 180.0;
	}
	
//	printf(" PDAF result now(%f) -> target(%f)\n", now_pos, result -> PdafPos );
}

#ifdef D_PD_DEBUG_SAVE_PD_DATA
static void debug_save_file( CHAR* fname, ULONG addr, ULONG size )
{
	ULONG fid;
	ULONG write_size = 0;

	fid = FJ_Fs_Open ( fname, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	FJ_Fs_Write ( fid, (VOID*)addr, size, &write_size );
	FJ_Fs_Close( fid );
}
#endif
