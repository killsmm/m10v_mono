/**
 * @file		still_proc_subr.c
 * @brief		capture image create procedure
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// FJ Header
#include "fj_std.h"

// Still Header
#include "still_proc_subr.h"
#include "still_iip.h"
#include "still_debug.h"
#include "still_common.h"
#include "still_dcache.h"

// IIP Resource Header
#include "bf_iip_resource.h"

// FileSystem Header
#include "fs_if.h"

// Board Header
#include "sdram_map_cache_define.h"

// Utility Header
#include "measure_time.h"
#include "mba_if.h"
#include "debug.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_BF_STILL_IIP_CSC_MDSEL				(1)
#define D_BF_STILL_IIP_CSCK_X1_0				(0x1000L)	// when MDSEL=1.

#define M_BF_STILL_CALC_IIP_CSCK( f_val )		((LONG)((f_val) * D_BF_STILL_IIP_CSCK_X1_0))

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Movie info															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

// for AFN debugging
static T_IM_IIP_PARAM_AFN* g_afn_unit_inf_convimg;
static T_IM_IIP_PARAM_STS_FILL* g_stx_unit_inf_convimg;
static T_IM_IIP_PARAM_AFN_OPCOL_0* g_afn_tbl_convimg;

#if 0	// Warning Delete (tentative)
static __align(32) UCHAR	gFrect_Unit_0_Buffer[M_BF_STILL_ROUNDUP_32(sizeof(T_IM_IIP_PARAM_FRECT))];			// Frect-0ch
static __align(32) UCHAR	gFrect_Stx_Unit_0_Buffer[M_BF_STILL_ROUNDUP_32(sizeof(T_IM_IIP_PARAM_STS_FILL))];	// Frect-0ch
#endif	// Warning Delete (tentative)
//static __align(32) UCHAR	gFrect_Unit_1_Buffer[M_BF_STILL_ROUNDUP_32(sizeof(T_IM_IIP_PARAM_FRECT))];			// Frect-1ch
//static __align(32) UCHAR	gFrect_Stx_Unit_1_Buffer[M_BF_STILL_ROUNDUP_32(sizeof(T_IM_IIP_PARAM_STS_FILL))];	// Frect-1ch

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static VOID bf_still_set_iip_res2_to_res( const T_BF_STILL_IIP_RESOURCE2* const res2, T_BF_STILL_IIP_RESOURCE* const res )
{
	res->wait_factor		= res2->wait_factor;
	res->unitid_bitmask64	= res2->unitid_bitmask64;
	res->src_cache_sel		= res2->src_cache_sel[0];
	res->src_unitid			= res2->src_unitid[0];
	res->dst_unitid			= res2->dst_unitid;
	res->src_portid			= res2->src_portid[0];
	res->dst_portid			= res2->dst_portid;
	res->src_pixid			= res2->src_pixid[0];
	res->dst_pixid			= res2->dst_pixid;
	res->src_master_if		= res2->src_master_if[0];
	res->dst_master_if		= res2->dst_master_if;
	res->wait_time			= res2->wait_time;

	// for Not use Im_IIP_Open_SWTRG() and Im_IIP_Close_SWTRG().
	res->open_res_bitmask	= 0;
	res->pixid_bitmask		= 0;
}

static VOID bf_still_calc_csck( const T_BF_STILL_IIP_YCC2RGB_MATRIX* const ycc2rgb_matrix, T_BF_STILL_IIP_CSC_PARAM* const csc, const T_BF_STILL_IMG_CONV* const src_img, const T_BF_STILL_IMG_CONV* const dst_img )
{
	UINT32	r_idx;
	UINT32	g_idx;
	UINT32	b_idx;
	UINT32	a_idx;
	const CHAR errmsg[] = "bf_still_calc_csck: error. no=%u\n";

	csc->mdsel			= D_BF_STILL_IIP_CSC_MDSEL;
	csc->alpha_in_sel	= E_IM_IIP_PARAM_ALPSEL_ALPVAL_REG;

	if ( src_img->addr.alpha != 0 ) {
		csc->alpha_out_sel = E_IM_IIP_PARAM_ALOSEL_THROUGH;
	}
	else {
		csc->alpha_out_sel = E_IM_IIP_PARAM_ALOSEL_CALCULATED;
	}

	switch ( src_img->depth ) {
		case E_BF_STILL_IMG_DEPTH_U8:
			csc->alpha_val = 0xFF;
			break;
		case E_BF_STILL_IMG_DEPTH_U12:
			csc->alpha_val = 0xFFF;
			break;
		case E_BF_STILL_IMG_DEPTH_U16:
			csc->alpha_val = 0xFFFF;
			break;
		default:
			// Unsupported depth.
			BF_Debug_Print_Error(( errmsg, 1 ));
			csc->alpha_val = 0xFF;	// fail safe
			break;
	}

	switch ( dst_img->type ) {
		case E_BF_STILL_IMG_TYPE_RGB888:
			// Support both E_BF_STILL_PLANE_TYPE_PLANER and E_BF_STILL_PLANE_TYPE_CHUNKY.
			r_idx = 0;
			g_idx = 1;
			b_idx = 2;
			a_idx = 3;
			break;
		case E_BF_STILL_IMG_TYPE_RGB444:
			// for E_BF_STILL_PLANE_TYPE_CHUNKY.
			r_idx = 2;
			g_idx = 0;
			b_idx = 1;
			a_idx = 3;
			break;
		default:
			// Unsupported image type.
//			BF_Debug_Print_Error(( errmsg, 2 ));
			r_idx = 0;		// fail safe
			g_idx = 1;		// fail safe
			b_idx = 2;		// fail safe
			a_idx = 3;		// fail safe
			break;
	}

	// R.
	csc->csck[r_idx].bit.CSCK_0 = M_BF_STILL_CALC_IIP_CSCK( ycc2rgb_matrix->r[0] );	// Y.
	csc->csck[r_idx].bit.CSCK_1 = M_BF_STILL_CALC_IIP_CSCK( ycc2rgb_matrix->r[1] );	// Cb.
	csc->csck[r_idx].bit.CSCK_2 = M_BF_STILL_CALC_IIP_CSCK( ycc2rgb_matrix->r[2] );	// Cr.
	csc->csck[r_idx].bit.CSCK_3 = (csc->csck[r_idx].bit.CSCK_1 + csc->csck[r_idx].bit.CSCK_2)
								* (-((csc->alpha_val +1) /2)) / csc->alpha_val;		// A ... decrease CbCr offset. (e.g. 128).
	// G.
	csc->csck[g_idx].bit.CSCK_0 = M_BF_STILL_CALC_IIP_CSCK( ycc2rgb_matrix->g[0] );	// Y.
	csc->csck[g_idx].bit.CSCK_1 = M_BF_STILL_CALC_IIP_CSCK( ycc2rgb_matrix->g[1] );	// Cb.
	csc->csck[g_idx].bit.CSCK_2 = M_BF_STILL_CALC_IIP_CSCK( ycc2rgb_matrix->g[2] );	// Cr.
	csc->csck[g_idx].bit.CSCK_3 = (csc->csck[g_idx].bit.CSCK_1 + csc->csck[g_idx].bit.CSCK_2)
								* (-((csc->alpha_val +1) /2)) / csc->alpha_val;		// A ... decrease CbCr offset. (e.g. 128).
	// B.
	csc->csck[b_idx].bit.CSCK_0 = M_BF_STILL_CALC_IIP_CSCK( ycc2rgb_matrix->b[0] );	// Y.
	csc->csck[b_idx].bit.CSCK_1 = M_BF_STILL_CALC_IIP_CSCK( ycc2rgb_matrix->b[1] );	// Cb.
	csc->csck[b_idx].bit.CSCK_2 = M_BF_STILL_CALC_IIP_CSCK( ycc2rgb_matrix->b[2] );	// Cr.
	csc->csck[b_idx].bit.CSCK_3 = (csc->csck[b_idx].bit.CSCK_1 + csc->csck[b_idx].bit.CSCK_2)
								* (-((csc->alpha_val +1) /2)) / csc->alpha_val;		// A ... decrease CbCr offset. (e.g. 128).
	// A.
	csc->csck[a_idx].bit.CSCK_0 = M_BF_STILL_CALC_IIP_CSCK( 0.0f );					// Y.
	csc->csck[a_idx].bit.CSCK_1 = M_BF_STILL_CALC_IIP_CSCK( 0.0f );					// Cb.
	csc->csck[a_idx].bit.CSCK_2 = M_BF_STILL_CALC_IIP_CSCK( 0.0f );					// Cr.
	csc->csck[a_idx].bit.CSCK_3 = M_BF_STILL_CALC_IIP_CSCK( 1.0f );					// A.
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ IIP Resource
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

static FJ_ERR_CODE bf_still_proc_subr_alloc_iip_res( const T_FJ_IIP_RES_CONFIG_ARG* const iip_arg_conf, T_BF_STILL_IIP_RESOURCE* const iip_res )
{
	T_FJ_IIP_RES_CONFIG_OUT	res_config;
	INT32					res_ercd;
	FJ_ERR_CODE				fj_ercd;
	const CHAR				errmsg[] = "bf_still_proc_subr_alloc_iip_res(): ercd=%d err=%u\n";
	INT32					wait_time;

	if( iip_res->wait_time != -1 ){
		wait_time = iip_res->wait_time *3;
	}
	else {
		wait_time = iip_res->wait_time;
	}

#ifdef DEBUG_ON_PC
	if( (0 < wait_time) && (wait_time < 20) ) {
		wait_time = 20;
	}
#endif

	res_ercd = BF_IIP_Res_Config_Lock_BaseFW( &res_config, iip_arg_conf, 1, wait_time );
	if( res_ercd != D_FJ_IIP_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, res_ercd, 1 ));
		BF_IIP_Res_DebugPrint();
		return FJ_ERR_NG;
	}

	fj_ercd = BF_Still_Iip_Set_Config( &res_config, iip_res );
	if( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, fj_ercd, 2 ));
		return fj_ercd;
	}

	return FJ_ERR_OK;
}

static FJ_ERR_CODE bf_still_proc_subr_alloc_iip_res2( const T_FJ_IIP_RES_CONFIG_ARG* const iip_arg_conf, T_BF_STILL_IIP_RESOURCE2* const iip_res )
{
	T_FJ_IIP_RES_CONFIG_OUT	res_config;
	INT32					res_ercd;
	FJ_ERR_CODE				fj_ercd;
	const CHAR				errmsg[] = "bf_still_proc_subr_alloc_iip_res2(): ercd=%d err=%u\n";
	INT32					wait_time;

	if( iip_res->wait_time != -1 ){
		wait_time = iip_res->wait_time *3;
	}
	else {
		wait_time = iip_res->wait_time;
	}

#ifdef DEBUG_ON_PC
	if( (0 < wait_time) && (wait_time < 20) ) {
		wait_time = 20;
	}
#endif

	res_ercd = BF_IIP_Res_Config_Lock_BaseFW( &res_config, iip_arg_conf, 1, wait_time );
	if( res_ercd != D_FJ_IIP_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, res_ercd, 1 ));
		BF_IIP_Res_DebugPrint();
		return FJ_ERR_NG;
	}

	fj_ercd = BF_Still_Iip_Set_Config2( &res_config, iip_res );
	if( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, fj_ercd, 2 ));
		return fj_ercd;
	}

	return FJ_ERR_OK;
}



static FJ_ERR_CODE bf_still_proc_subr_free_iip_res( const T_BF_STILL_IIP_RESOURCE* const iip_res )
{
	T_FJ_IIP_RES_CONFIG_OUT	res_config;
	INT32					res_ercd;
	INT32					wait_time;
	const CHAR				errmsg[] = "bf_still_proc_subr_free_iip_res(): ercd=%d err=%u\n";

	res_config.unitid_bitmask64 = iip_res->unitid_bitmask64;
	res_config.pixid_bitmask = iip_res->pixid_bitmask;
	res_config.other_bitmask = iip_res->open_res_bitmask;
	
	if( iip_res->wait_time != -1 ){
		wait_time = iip_res->wait_time *3;
	}
	else {
		wait_time = iip_res->wait_time;
	}

	res_ercd = BF_IIP_Res_Config_Free_BaseFW( &res_config, 1, wait_time );
	if( res_ercd != D_FJ_IIP_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, res_ercd, 1 ));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

static FJ_ERR_CODE bf_still_proc_subr_free_iip_res2( const T_BF_STILL_IIP_RESOURCE2* const iip_res )
{
	T_FJ_IIP_RES_CONFIG_OUT	res_config;
	INT32					res_ercd;
	INT32					wait_time;
	const CHAR				errmsg[] = "bf_still_proc_subr_free_iip_res(): ercd=%d err=%u\n";

	res_config.unitid_bitmask64 = iip_res->unitid_bitmask64;
	res_config.pixid_bitmask = iip_res->pixid_bitmask;
	res_config.other_bitmask = 0;
	
	if( iip_res->wait_time != -1 ){
		wait_time = iip_res->wait_time *3;
	}
	else {
		wait_time = iip_res->wait_time;
	}

	res_ercd = BF_IIP_Res_Config_Free_BaseFW( &res_config, 1, wait_time );
	if( res_ercd != D_FJ_IIP_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, res_ercd, 1 ));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

VOID BF_Still_Proc_Subr_Convert_ImgInfo( const FJ_IMG_INFO* const org_img, T_BF_STILL_IMG* conv_img )
{
	switch( org_img->pfmt ) {
		case FJ_IMG_PFMT_YCC422:
			conv_img->type	= E_BF_STILL_IMG_TYPE_YCC422;
			conv_img->plane	= E_BF_STILL_PLANE_TYPE_C_INTL;
			break;

		case FJ_IMG_PFMT_YCC420:
			conv_img->type	= E_BF_STILL_IMG_TYPE_YCC420;
			conv_img->plane	= E_BF_STILL_PLANE_TYPE_C_INTL;
			break;

		case FJ_IMG_PFMT_VIDEO:
			conv_img->type	= E_BF_STILL_IMG_TYPE_YCC420;
			conv_img->plane	= E_BF_STILL_PLANE_TYPE_VIDEO;
			break;

		case FJ_IMG_PFMT_YCC444:
			conv_img->type	= E_BF_STILL_IMG_TYPE_YCC444;
			conv_img->plane	= E_BF_STILL_PLANE_TYPE_C_INTL;
			break;

		case FJ_IMG_PFMT_YCC400:
			conv_img->type	= E_BF_STILL_IMG_TYPE_YCC400;
			conv_img->plane	= E_BF_STILL_PLANE_TYPE_1PLANE;
			break;

		case FJ_IMG_PFMT_RGBA8888:
			conv_img->type	= E_BF_STILL_IMG_TYPE_RGB888;
			conv_img->plane	= E_BF_STILL_PLANE_TYPE_CHUNKY;
			break;

		case FJ_IMG_PFMT_RGBA4444:
			conv_img->type	= E_BF_STILL_IMG_TYPE_RGB444;
			conv_img->plane	= E_BF_STILL_PLANE_TYPE_CHUNKY;
			break;

		default:
			return;
	}


	conv_img->depth			= E_BF_STILL_IMG_DEPTH_U8;
	conv_img->addr.y		= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( org_img->addr.AddressY );
	conv_img->addr.cb		= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( org_img->addr.AddressCb );
	conv_img->addr.cr		= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( org_img->addr.AddressCr );
	conv_img->pixs.width	= org_img->width;
	conv_img->pixs.lines	= org_img->lines;
	conv_img->g_width.y		= org_img->g_y_width;
	conv_img->g_width.cb	= org_img->g_c_width;
	conv_img->g_width.cr	= org_img->g_c_width;

	return;
}


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ 
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *	@brief			Start AFN Process for IIP Scaler
 *	@param[in]		T_BF_STILL_IIP_IMG_CONVERT* img_info
 *	@param[in]		UCHAR fill_en
 *	@return			FJ_ERR_CODE
 *	@note			None
 *	@attention		None
 */
FJ_ERR_CODE BF_Still_Proc_Execute_AfnConvertImage( const T_BF_STILL_IIP_IMG_CONVERT* const img_info, const UCHAR fill_en, const INT32 timeout )	/* pgr0339 */
{
	INT32								ercd = 0;
	FJ_ERR_CODE							fj_ercd;
	T_FJ_IIP_RES_CONFIG_ARG				iip_arg_conf = {
		.unit = {	// Store Unit set later.
			.oned	= 0,	.flt	= 0,	.afn_0	= 0,	.afn_1	= 0,	.afn_2	= 0,	.afn	= 1,
			.frect	= 0,	.mon	= 0,	.csc	= 0,	.lut	= 0,	.gpc	= 0,	.blend	= 0,
			.cfl	= 0,	.mft	= 0,	.sl		= 0,	.sl_0_2	= 0,	.sl_3_8	= 0,
			.sl0	= 0,	.sl1	= 0,	.sl2	= 0,	.sl3	= 0,	.sl4	= 0,	.sl5	= 0,
			.sl6	= 0,	.sl7	= 0,	.sl8	= 0,	.sl_mon = 0,
			.sl_master_if_0 = 0,	.sl_master_if_1 = 0,	.sl_master_if_2 = 0,
		},
		.pixid = 2,			// request 2 pixids.
		.other = D_FJ_IIP_RES_OTHER_NONE
	};
	T_BF_STILL_IIP_RESOURCE				res;
	T_BF_STILL_IIP_RESOURCE2			res2 = {
		// Auto allocation by bf_still_proc_subr_alloc_iip_res().
		.wait_factor		= 0,
		.unitid_bitmask64	= 0,
		.pixid_bitmask		= 0,
		.src_unitid[0]		= E_IM_IIP_UNIT_ID_MAX,
		.src_unitid[1]		= E_IM_IIP_UNIT_ID_MAX,
		.dst_unitid			= E_IM_IIP_UNIT_ID_MAX,
		.filter_unitid		= E_IM_IIP_UNIT_ID_MAX,
		.src_portid[0]		= E_IM_IIP_PARAM_PORTID_NONE,
		.src_portid[1]		= E_IM_IIP_PARAM_PORTID_NONE,
		.dst_portid			= E_IM_IIP_PARAM_PORTID_NONE,
		.filter_portid[0]	= E_IM_IIP_PARAM_PORTID_NONE,
		.filter_portid[1]	= E_IM_IIP_PARAM_PORTID_NONE,
		.src_pixid[0]		= 0xff,
		.src_pixid[1]		= 0xff,
		.dst_pixid			= 0xff,
		.src_cache_sel[0]	= E_IM_IIP_PARAM_CSEL_0,
		.src_cache_sel[1]	= E_IM_IIP_PARAM_CSEL_0,
		.open_res_bitmask	= E_IM_IIP_OPEN_RES_CACHE1,
		.wait_time			= timeout,
	};
	T_BF_STILL_IIP_AFN_PARAM			afn = {
		.dst_pos = {
			.sta_x = 0,
			.sta_y = 0,
			.width = 0,
			.lines = 0,
		},
		.grain_pixs.width	= 64,
		.grain_pixs.lines	= 8,
		.prefetch_off		= E_IM_IIP_PARAM_PFOFF_RUN,
		.alignment.width	= 1,
		.alignment.lines	= 1,
		.fil_md				= 0,
		.mask_enable		= FALSE,
		.rotate_deg			= FJ_IMG_ROTATE_DEGREE_0,
		.nearest_neighbor	= FALSE,
		.mirror				= FALSE,
	};
	T_BF_STILL_IIP_CSC_PARAM			csc;
	T_BF_STILL_IIP_YCC2RGB_MATRIX		ycc2rgb_matrix = {
				.r = { 1.0,  0.0,    1.402 },	// R coefficient. (0:Y, 1:Cb, 2:Cr).
				.g = { 1.0, -0.344, -0.714 },	// G coefficient. (0:Y, 1:Cb, 2:Cr).
				.b = { 1.0,  1.772,  0.0   },	// B coefficient. (0:Y, 1:Cb, 2:Cr).
	};
	UCHAR afn_unit_inf_buffer_convimg[M_BF_STILL_ROUNDUP_32(sizeof(T_IM_IIP_PARAM_AFN)) + D_BF_STILL_IIP_TBL_ALIGN];
	UCHAR csc_unit_inf_buffer_convimg[M_BF_STILL_ROUNDUP_32(sizeof(T_IM_IIP_PARAM_CSC)) + D_BF_STILL_IIP_TBL_ALIGN];
	UCHAR stx_unit_inf_buffer_convimg[M_BF_STILL_ROUNDUP_32(sizeof(T_IM_IIP_PARAM_STS_FILL)) + D_BF_STILL_IIP_TBL_ALIGN];
	UCHAR afn_tbl_buffer_convimg[M_BF_STILL_ROUNDUP_32(sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0)) + D_BF_STILL_IIP_TBL_ALIGN];
	const CHAR							errmsg[] = "BF_Still_Proc_Execute_AfnConvertImage: no=%u ercd=%d\n";

	afn.afn_unit_inf	= (T_IM_IIP_PARAM_AFN*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)afn_unit_inf_buffer_convimg );
	afn.sl_unit_inf		= (T_IM_IIP_PARAM_STS_FILL*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)stx_unit_inf_buffer_convimg );
	afn.afn_tbl			= (T_IM_IIP_PARAM_AFN_OPCOL_0*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)afn_tbl_buffer_convimg );

	csc.csc_unit_inf	= (T_IM_IIP_PARAM_CSC*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)csc_unit_inf_buffer_convimg );

	// for Debug.
	g_afn_unit_inf_convimg	= afn.afn_unit_inf;
	g_stx_unit_inf_convimg	= afn.sl_unit_inf;
	g_afn_tbl_convimg		= afn.afn_tbl;

	// Set D-Zoom parameter.
	afn.crop_rect.sta_x = M_BF_STILL_AFN_CONV_TBL_VAL( img_info->crop_rect.sta_x );
	afn.crop_rect.sta_y = M_BF_STILL_AFN_CONV_TBL_VAL( img_info->crop_rect.sta_y );
	afn.crop_rect.width = M_BF_STILL_AFN_CONV_TBL_VAL( img_info->crop_rect.width );
	afn.crop_rect.lines = M_BF_STILL_AFN_CONV_TBL_VAL( img_info->crop_rect.lines );
	afn.rotate_deg      = img_info->rotate_deg;
	afn.mirror          = img_info->mirror;

	// Select Fill function.
	if ( fill_en ) {
		afn.fil_md = 1;
	}

	if ( img_info->fill_videofmt_alignment != FALSE ) {
		if ( img_info->dst.plane == E_BF_STILL_PLANE_TYPE_VIDEO ) {
			afn.alignment.width	= D_BF_STILL_VIDEOFMT_YC_WIDTH;
			afn.alignment.lines	= D_BF_STILL_VIDEOFMT_YC_LINES;
			afn.fil_md			= 1;
		}
	}

	// Bi-Cubic.
	switch( img_info->itmd ) {
		case E_BF_STILL_IIP_ITMD_BICUBIC:
			iip_arg_conf.other		= D_FJ_IIP_RES_OTHER_BICUBIC_0_OR_1;
			afn.nearest_neighbor	= FALSE;
			break;
		case E_BF_STILL_IIP_ITMD_NEAREST:
			afn.nearest_neighbor	= TRUE;
			break;
		default:
			afn.nearest_neighbor	= FALSE;
			break;
	}

	if ( ( img_info->src.type == E_BF_STILL_IMG_TYPE_RGB888 ) ||
		 ( img_info->src.type == E_BF_STILL_IMG_TYPE_RGB444 ) ||
		 ( img_info->dst.type == E_BF_STILL_IMG_TYPE_RGB888 ) ||
		 ( img_info->dst.type == E_BF_STILL_IMG_TYPE_RGB444 ) ) {

		iip_arg_conf.unit.csc = 1;

		// Select Master IF.
		fj_ercd = BF_Still_Iip_Set_SrcCacheSel( &res2.src_cache_sel[0], img_info->src_mif );
		if ( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, 1, fj_ercd ));
			return fj_ercd;
		}

		fj_ercd = BF_Still_Iip_Set_SrcCacheSel( &res2.src_cache_sel[1], img_info->src_mif );
		if ( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, 2, fj_ercd ));
			return fj_ercd;
		}

		fj_ercd = BF_Still_Iip_Set_SrcMasterIf( &res2.src_master_if[0], img_info->src_mif );
		if ( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, 3, fj_ercd ));
			return fj_ercd;
		}

		fj_ercd = BF_Still_Iip_Set_SrcMasterIf( &res2.src_master_if[1], img_info->src_mif );
		if ( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, 4, fj_ercd ));
			return fj_ercd;
		}

		fj_ercd = BF_Still_Iip_Set_DstMasterIf( &res2.dst_master_if, &iip_arg_conf, img_info->dst_mif, FALSE );
		if ( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, 5, fj_ercd ));
			return fj_ercd;
		}

		// start AFN process.
		fj_ercd = bf_still_proc_subr_alloc_iip_res2( &iip_arg_conf, &res2 );
		if ( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, 6, fj_ercd ));
			fj_ercd = FJ_ERR_NG;
		}

		while ( fj_ercd == FJ_ERR_OK ) {

			ercd = BF_Still_Iip_Open2( &res2 );
			if ( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 7, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			while ( 1 ) {

				bf_still_set_iip_res2_to_res( &res2, &res );

				fj_ercd = BF_Still_Iip_Set_Afn( &img_info->src, &img_info->dst, &res, &afn );
				if ( fj_ercd != FJ_ERR_OK ) {
					BF_Debug_Print_Error(( errmsg, 8, fj_ercd ));
					fj_ercd = FJ_ERR_NG;
					break;
				}

				bf_still_calc_csck( &ycc2rgb_matrix, &csc, &img_info->src, &img_info->dst );

				fj_ercd = BF_Still_Iip_Append_AfnCsc( &res2, &afn, &csc );
				if( fj_ercd != FJ_ERR_OK ) {
					BF_Debug_Print_Error(( errmsg, 9, fj_ercd ));
					break;
				}

				ercd = BF_Still_Iip_Start2( &res2 );
				if ( ercd != D_IM_IIP_OK ) {
					BF_Debug_Print_Error(( errmsg, 10, ercd ));
					fj_ercd = FJ_ERR_NG;
					break;
				}

				// delete basefw\fj\imgproc\still
				ercd = BF_Still_Iip_WaitEnd2( &res2 );
				if ( ercd != D_IM_IIP_OK ) {
					BF_Debug_Print_Error(( errmsg, 11, ercd ));
					fj_ercd = FJ_ERR_NG;
					break;
				}

				fj_ercd = FJ_ERR_OK;
				break;
			}

			ercd = BF_Still_Iip_Close2( &res2 );
			if ( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 12, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			break;
		}

		(VOID)bf_still_proc_subr_free_iip_res2( &res2 );
	}
	else {

		bf_still_set_iip_res2_to_res( &res2, &res );

		// Select Master IF.
		fj_ercd = BF_Still_Iip_Set_SrcCacheSel( &res.src_cache_sel, img_info->src_mif );
		if ( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, 13, fj_ercd ));
			return fj_ercd;
		}

		fj_ercd = BF_Still_Iip_Set_SrcMasterIf( &res.src_master_if, img_info->src_mif );
		if ( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, 14, fj_ercd ));
			return fj_ercd;
		}

		fj_ercd = BF_Still_Iip_Set_DstMasterIf( &res.dst_master_if, &iip_arg_conf, img_info->dst_mif, FALSE );
		if ( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, 15, fj_ercd ));
			return fj_ercd;
		}

		// start AFN process.
		fj_ercd = bf_still_proc_subr_alloc_iip_res( &iip_arg_conf, &res );
		if ( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, 16, fj_ercd ));
			fj_ercd = FJ_ERR_NG;
		}

		while ( fj_ercd == FJ_ERR_OK ) {

			ercd = BF_Still_Iip_Open( &res );
			if ( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 17, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			while ( 1 ) {

				fj_ercd = BF_Still_Iip_Set_Afn( &img_info->src, &img_info->dst, &res, &afn );
				if ( fj_ercd != FJ_ERR_OK ) {
					BF_Debug_Print_Error(( errmsg,18, fj_ercd ));
					fj_ercd = FJ_ERR_NG;
					break;
				}

				ercd = BF_Still_Iip_Start( &res, E_BF_STILL_IIP_LINK_OFF );
				if ( ercd != D_IM_IIP_OK ) {
					BF_Debug_Print_Error(( errmsg, 19, ercd ));
					fj_ercd = FJ_ERR_NG;
					break;
				}

				ercd = BF_Still_Iip_WaitEnd( &res );
				if ( ercd != D_IM_IIP_OK ) {
					BF_Debug_Print_Error(( errmsg, 20, ercd ));
					fj_ercd = FJ_ERR_NG;
					break;
				}

				fj_ercd = FJ_ERR_OK;
				break;
			}

			ercd = BF_Still_Iip_Close( &res );
			if ( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 21, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			break;
		}

		(VOID)bf_still_proc_subr_free_iip_res( &res );
	}

	return fj_ercd;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ 
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
static T_BF_STILL_IIP_RESOURCE				gRes_IIP_Resouce;
static T_FJ_IIP_RES_CONFIG_ARG				gIip_Arg_conf = {
	.unit = {	// Store Unit set later.
		.oned	= 0,	.flt	= 0,	.afn_0	= 0,	.afn_1	= 0,	.afn_2	= 0,	.afn	= 1,
		.frect	= 0,	.mon	= 0,	.csc	= 0,	.lut	= 0,	.gpc	= 0,	.blend	= 0,
		.cfl	= 0,	.mft	= 0,	.sl		= 0,	.sl_0_2	= 0,	.sl_3_8	= 0,
		.sl0	= 0,	.sl1	= 0,	.sl2	= 0,	.sl3	= 0,	.sl4	= 0,	.sl5	= 0,
		.sl6	= 0,	.sl7	= 0,	.sl8	= 0,	.sl_mon = 0,
		.sl_master_if_0 = 0,	.sl_master_if_1 = 0,	.sl_master_if_2 = 0,
	},
	.pixid = 2,			// request 2 pixids.
	.other = D_FJ_IIP_RES_OTHER_NONE
};
T_BF_STILL_IIP_AFN_PARAM			gAfn_Param = {
	.dst_pos = {
		.sta_x = 0,
		.sta_y = 0,
		.width = 0,
		.lines = 0,
	},
	.grain_pixs.width	= 64,
	.grain_pixs.lines	= 8,
	.prefetch_off		= E_IM_IIP_PARAM_PFOFF_RUN,
	.fil_md				= 0,
	.mask_enable		= FALSE,
	.alignment.width	= 1,
	.alignment.lines	= 1,
	.rotate_deg = FJ_IMG_ROTATE_DEGREE_0,
	.nearest_neighbor	= FALSE,
	.mirror				= FALSE,
};

static UCHAR afn_unit_inf_buffer_convimg[M_BF_STILL_ROUNDUP_32(sizeof(T_IM_IIP_PARAM_AFN)) + D_BF_STILL_IIP_TBL_ALIGN];
static UCHAR stx_unit_inf_buffer_convimg[M_BF_STILL_ROUNDUP_32(sizeof(T_IM_IIP_PARAM_STS_FILL)) + D_BF_STILL_IIP_TBL_ALIGN];
static UCHAR afn_tbl_buffer_convimg[M_BF_STILL_ROUNDUP_32(sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0)) + D_BF_STILL_IIP_TBL_ALIGN];

/**
 *	@brief			Start AFN Process for IIP Scaler
 *	@param[in]		T_BF_STILL_IIP_IMG_CONVERT* img_info
 *	@param[in]		UCHAR fill_en
 *	@return			FJ_ERR_CODE
 *	@note			None
 *	@attention		None
 */
FJ_ERR_CODE BF_Still_Proc_Execute_AfnConvertImage_2_Open_SetData( const T_BF_STILL_IIP_IMG_CONVERT* const img_info, const UCHAR fill_en, const INT32 timeout )	/* pgr0339 */
{
	FJ_ERR_CODE							fj_ercd;
	T_BF_STILL_IIP_RESOURCE2			res2 = {
		// Auto allocation by bf_still_proc_subr_alloc_iip_res().
		.wait_factor		= 0,
		.unitid_bitmask64	= 0,
		.pixid_bitmask		= 0,
		.src_unitid[0]		= E_IM_IIP_UNIT_ID_MAX,
		.src_unitid[1]		= E_IM_IIP_UNIT_ID_MAX,
		.dst_unitid			= E_IM_IIP_UNIT_ID_MAX,
		.filter_unitid		= E_IM_IIP_UNIT_ID_MAX,
		.src_portid[0]		= E_IM_IIP_PARAM_PORTID_NONE,
		.src_portid[1]		= E_IM_IIP_PARAM_PORTID_NONE,
		.dst_portid			= E_IM_IIP_PARAM_PORTID_NONE,
		.filter_portid[0]	= E_IM_IIP_PARAM_PORTID_NONE,
		.filter_portid[1]	= E_IM_IIP_PARAM_PORTID_NONE,
		.src_pixid[0]		= 0xff,
		.src_pixid[1]		= 0xff,
		.dst_pixid			= 0xff,
		.src_cache_sel[0]	= E_IM_IIP_PARAM_CSEL_0,
		.src_cache_sel[1]	= E_IM_IIP_PARAM_CSEL_0,
		.open_res_bitmask	= E_IM_IIP_OPEN_RES_CACHE1,
		.wait_time			= timeout,
	};
	const CHAR							errmsg[] = "BF_Still_Proc_Execute_AfnConvertImage: no=%u ercd=%d\n";

	FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_AFN, FJ_MBALOG_TYPE_POINT, "AFN-Set"));

	gAfn_Param.afn_unit_inf	= (T_IM_IIP_PARAM_AFN*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)afn_unit_inf_buffer_convimg );
	gAfn_Param.sl_unit_inf		= (T_IM_IIP_PARAM_STS_FILL*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)stx_unit_inf_buffer_convimg );
	gAfn_Param.afn_tbl			= (T_IM_IIP_PARAM_AFN_OPCOL_0*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)afn_tbl_buffer_convimg );

	// for Debug.
	g_afn_unit_inf_convimg	= gAfn_Param.afn_unit_inf;
	g_stx_unit_inf_convimg	= gAfn_Param.sl_unit_inf;
	g_afn_tbl_convimg		= gAfn_Param.afn_tbl;

	// Set D-Zoom parameter.
	gAfn_Param.crop_rect.sta_x = M_BF_STILL_AFN_CONV_TBL_VAL( img_info->crop_rect.sta_x );
	gAfn_Param.crop_rect.sta_y = M_BF_STILL_AFN_CONV_TBL_VAL( img_info->crop_rect.sta_y );
	gAfn_Param.crop_rect.width = M_BF_STILL_AFN_CONV_TBL_VAL( img_info->crop_rect.width );
	gAfn_Param.crop_rect.lines = M_BF_STILL_AFN_CONV_TBL_VAL( img_info->crop_rect.lines );
	gAfn_Param.rotate_deg      = img_info->rotate_deg;

	// Select Fill function.
	if ( fill_en ) {
		gAfn_Param.fil_md = 1;
	}

	if ( img_info->fill_videofmt_alignment != FALSE ) {
		if ( img_info->dst.plane == E_BF_STILL_PLANE_TYPE_VIDEO ) {
			gAfn_Param.alignment.width	= D_BF_STILL_VIDEOFMT_YC_WIDTH;
			gAfn_Param.alignment.lines	= D_BF_STILL_VIDEOFMT_YC_LINES;
			gAfn_Param.fil_md			= 1;
		}
	}

	// Bi-Cubic.
	if ( img_info->itmd == E_BF_STILL_IIP_ITMD_BICUBIC ) {
		gIip_Arg_conf.other = D_FJ_IIP_RES_OTHER_BICUBIC_0_OR_1;
	}

	bf_still_set_iip_res2_to_res( &res2, &gRes_IIP_Resouce );

	// Select Master IF.
	fj_ercd = BF_Still_Iip_Set_SrcCacheSel( &gRes_IIP_Resouce.src_cache_sel, img_info->src_mif );
	if ( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, 13, fj_ercd ));
		return fj_ercd;
	}

	fj_ercd = BF_Still_Iip_Set_SrcMasterIf( &gRes_IIP_Resouce.src_master_if, img_info->src_mif );
	if ( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, 14, fj_ercd ));
		return fj_ercd;
	}

	fj_ercd = BF_Still_Iip_Set_DstMasterIf( &gRes_IIP_Resouce.dst_master_if, &gIip_Arg_conf, img_info->dst_mif, FALSE );
	if ( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, 15, fj_ercd ));
		return fj_ercd;
	}

	fj_ercd = bf_still_proc_subr_alloc_iip_res( &gIip_Arg_conf, &gRes_IIP_Resouce );
	if ( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, 16, fj_ercd ));
		fj_ercd = FJ_ERR_NG;
	}

	return fj_ercd;
}

/**
 *	@brief			Start AFN Process for IIP Scaler
 *	@param[in]		T_BF_STILL_IIP_IMG_CONVERT* img_info
 *	@param[in]		UCHAR fill_en
 *	@return			FJ_ERR_CODE
 *	@note			None
 *	@attention		None
 */
FJ_ERR_CODE BF_Still_Proc_Execute_AfnConvertImage_2_Execute( const T_BF_STILL_IIP_IMG_CONVERT* const img_info )
{
	INT32			ercd;
	FJ_ERR_CODE		fj_ercd;

	// start AFN process.
	ercd = BF_Still_Iip_Open( &gRes_IIP_Resouce );
	if ( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( "BF_Still_Proc_Execute_AfnConvertImage_2_Execute: no=%u ercd=%d\n", 17, ercd ));
		return FJ_ERR_NG;
	}

	fj_ercd = BF_Still_Iip_Set_Afn( &img_info->src, &img_info->dst, &gRes_IIP_Resouce, &gAfn_Param );
	if ( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( "BF_Still_Proc_Execute_AfnConvertImage_2_Execute: no=%u ercd=%d\n",18, fj_ercd ));
		return FJ_ERR_NG;
	}

	// IIP start
	ercd = BF_Still_Iip_Start( &gRes_IIP_Resouce, E_BF_STILL_IIP_LINK_OFF );
	if ( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( "BF_Still_Proc_Execute_AfnConvertImage_2_Execute: no=%u ercd=%d\n", 20, ercd ));
		return FJ_ERR_NG;
	}

	// IIP wait end
	ercd = BF_Still_Iip_WaitEnd( &gRes_IIP_Resouce );
	if ( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( "BF_Still_Proc_Execute_AfnConvertImage_2_Execute: no=%u ercd=%d\n", 21, ercd ));
		fj_ercd = FJ_ERR_NG;
	}

	// IIP close
	ercd = BF_Still_Iip_Close( &gRes_IIP_Resouce );
	if ( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( "BF_Still_Proc_Execute_AfnConvertImage_2_Execute: no=%u ercd=%d\n", 22, ercd ));
		fj_ercd = FJ_ERR_NG;
	}

	return fj_ercd;
}

/**
 *	@brief			Close AFN Process for IIP Scaler
 *	@param[in]		VOID
 *	@return			FJ_ERR_CODE
 *	@note			None
 *	@attention		None
 */
FJ_ERR_CODE BF_Still_Proc_Execute_AfnConvertImage_2_Close( VOID )
{
	// free IIP Resource
	(VOID)bf_still_proc_subr_free_iip_res( &gRes_IIP_Resouce );

	return FJ_ERR_OK;
}


