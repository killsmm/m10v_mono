/**
 * @file		im_iip.h
 * @brief		Header file of JDSIIP4B driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@weakgroup im_iip
@{

@section im_iip_overview Overview
IIP Driver section.<br>
<br>
Rotation&Resize, CSC, LUT and AlphaBlend are prepared as Utility functions.<br>
We does not optimize 4 utility functions and
does not prepare other utility function at all.<br>

@}
*/
/**
@weakgroup im_iip
@{

<hr>

@section im_iip_sequence Sequence Chart
- @ref IIP_Init
- @ref IIP_Single_Using_Hwtrg
- @ref IIP_Single_Using_Swtrg


@section IIP_Init						Initialize driver
@image html im_iip_init.png

@section IIP_Single_Using_Hwtrg	Single frame processing using HWTRG
@image html im_iip_single_hwtrg.png

@section IIP_Single_Using_Swtrg	Single frame processing using SWTRG
@image html im_iip_single_swtrg.png


@}
*/
/**
@weakgroup im_iip
@{

<hr>

@section im_iip_sample Sample code
- @ref im_iip_swtrg
- @ref im_iip_hwtrg
- @ref im_iip_util

@}
*/
/**
@weakgroup im_iip
@{

@section im_iip_swtrg	Sample code of Single frame reducing to 1/2 using SWTRG.
@code
#include "im_iip.h"
#include "l1l2cache.h"
#include "ddim_typedef.h"

#include <stdio.h>


typedef struct {
	E_IM_IIP_FTYPE		frame_type;
	E_IM_IIP_PFMT		pix_format;
	T_IM_IIP_IMG_ADDR	addr;
	T_IM_IIP_LINE_BYTES	g_width;
	USHORT				width;
	USHORT				lines;
} T_SAMPLE_IMGINFO;

static __align(8) T_IM_IIP_PARAM_AFN			g_sample_unit_param_afn;	// Needs 8bytes align
static __align(8) T_IM_IIP_PARAM_STX			g_sample_unit_param_sl;		// Needs 8bytes align
static __align(8) T_IM_IIP_PARAM_AFN_OPCOL_0	g_sample_afntbl;			// Needs 8bytes align

#define D_SAMPLE_AFN_UNITID				E_IM_IIP_UNIT_ID_AFN0
#define D_SAMPLE_AFN_PLDUNIT			D_IM_IIP_PARAM_PLDUNIT_AFN0
#define D_SAMPLE_AFN_PORTID				E_IM_IIP_PARAM_PORTID_AFN0
#define D_SAMPLE_AFN_AXIID				E_IM_IIP_AXI_ID_AFN0
#define D_SAMPLE_AFN_RESIZE_MODE		E_IM_IIP_PARAM_ITMD_BI_CUBIC
#define D_SAMPLE_AFN_CSEL				E_IM_IIP_PARAM_CSEL_0
#define D_SAMPLE_AFN_CUBSEL				E_IM_IIP_PARAM_CUBSEL_AFN0
#define D_SAMPLE_AFN_NO					0

#define D_SAMPLE_SL_UNITID				E_IM_IIP_UNIT_ID_SL2
#define D_SAMPLE_SL_PLDUNIT				D_IM_IIP_PARAM_PLDUNIT_SL2
#define D_SAMPLE_SL_PORTID				E_IM_IIP_PARAM_PORTID_SL2
#define D_SAMPLE_SL_AXIID				E_IM_IIP_AXI_ID_SL2
#define D_SAMPLE_INT_FACTOR				D_IM_IIP_INT_FACTOR_SL2END
#define D_SAMPLE_SL_NO					2

#define D_SAMPLE_SRC_PIXID_MASK			E_IM_IIP_PIXID_4
#define D_SAMPLE_SRC_PIXID_VAL			4

#define D_SAMPLE_DST_PIXID_MASK			E_IM_IIP_PIXID_5
#define D_SAMPLE_DST_PIXID_VAL			5

#define D_SAMPLE_SRC_MASTER_IF			D_IM_IIP_MASTER_IF0
#define D_SAMPLE_DST_MASTER_IF			D_IM_IIP_MASTER_IF1


#define D_SAMPLE_AFNTBL_1_0				(0x10000UL)
#define sample_calc_afntbl_pos( f_val )	( (ULONG)((f_val) * D_SAMPLE_AFNTBL_1_0) )


static VOID sample_create_afntbl( T_IM_IIP_PARAM_AFN_OPCOL_0* const afntbl, const T_SAMPLE_IMGINFO* const dst_img )
{
	afntbl->STAXY0.bit.STAX = sample_calc_afntbl_pos( 0.0 );
	afntbl->STAXY0.bit.STAY = sample_calc_afntbl_pos( 0.0 );

	afntbl->D_OUT_IN_0.bit.DXX = sample_calc_afntbl_pos( 0.5 );		// reduce to 1/2
	afntbl->D_OUT_IN_0.bit.DXY = sample_calc_afntbl_pos( 0.0 );
	afntbl->D_OUT_IN_0.bit.DYX = sample_calc_afntbl_pos( 0.0 );
	afntbl->D_OUT_IN_0.bit.DYY = sample_calc_afntbl_pos( 0.5 );		// reduce to 1/2
	afntbl->SZ.bit.AFNDVSZ = dst_img->lines;
	afntbl->SZ.bit.AFNDHSZ = dst_img->width;
}

static VOID sample_create_unitparam_afn( T_IM_IIP_PARAM_AFN* const afn, const T_IM_IIP_PARAM_AFN_OPCOL_0* const afntbl )
{
	afn->header0.bit.BeginningAddress = 0x0000;
	afn->header0.bit.WordLength = 10;
	afn->header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY;
	afn->AFNTOPCNF.bit.DATACONF = 0;
//	afn->AFNTOPCNF.bit.WAITCONF;							// set later
	afn->AFNLDCNF.bit.UPSU = E_IM_IIP_PARAM_UPSU_NONE;
	afn->AFNLDCNF.bit.DWSU = E_IM_IIP_PARAM_DWSU_NONE;
	afn->AFNLDCNF.bit.DTHD = E_IM_IIP_PARAM_DTHD_NORMAL;
	afn->AFNLDCNF.bit.CBC = E_IM_IIP_PARAM_CBC_DISABLE;
	afn->AFNCTL.bit.LPLVL = E_IM_IIP_PARAM_LPLVL_L_LEVEL;
	afn->AFNCTL.bit.HDBL = E_IM_IIP_PARAM_HDBL_SAME_AS_PICHSZ;
	afn->AFNCTL.bit.VDBL = E_IM_IIP_PARAM_VDBL_SAME_AS_PICVSZ;
	afn->AFNCTL.bit.GHSZ = E_IM_IIP_PARAM_GHSZ_32PIX;		// Needs tuning
	afn->AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_8PIX;		// Needs tuning
	afn->AFNCTL.bit.HSMD = E_IM_IIP_PARAM_HSMD_NORMAL;
	afn->PFCTL.bit.PFEXTMC = 4;
	afn->PFCTL.bit.PFEXTADC = 64;							// Needs tuning
	afn->PFCTL.bit.PFSP1 = E_IM_IIP_PARAM_PFSP_1;			// Needs tuning
	afn->PFCTL.bit.PFSP2 = E_IM_IIP_PARAM_PFSP_16;			// Needs tuning
	afn->PFCTL.bit.PFVSP = E_IM_IIP_PARAM_PFVSP_VERTICAL_0;	// Needs tuning
	afn->PFCTL.bit.PFOFF = E_IM_IIP_PARAM_PFOFF_RUN;
	afn->PIXIDEF.bit.EXA = E_IM_IIP_PARAM_EXA_NO_ALPHA_TO_NEXT_UNIT;
	afn->PIXIDEF.bit.CSEL = D_SAMPLE_AFN_CSEL;
//	afn->PIXIDEF.bit.IPIXID;								// set later
	afn->OUTSIZE.bit.OUTHSZ = 0;
	afn->OUTSIZE.bit.OUTVSZ = 0;
	afn->OUTSTARTPOS.bit.STARTX = 0;
	afn->OUTSTARTPOS.bit.STARTY = 0;
	afn->CALMETHOD.bit.OPCOL = 0;
	afn->CALMETHOD.bit.ITMD = D_SAMPLE_AFN_RESIZE_MODE;
	afn->CALMETHOD.bit.FILMD = 0;
	afn->CALMETHOD.bit.DANTI = 1;
	afn->CALMETHOD.bit.AANTI = 1;
	afn->CALMETHOD.bit.CUBSEL = D_SAMPLE_AFN_CUBSEL;
	afn->FILVAL0.bit.FILVALYG = 0x0;
	afn->FILVAL0.bit.FILVALB = 0x0;
	afn->FILVAL1.bit.FILVALR = 0x0;
	afn->FILVAL1.bit.FILVALA = 0x0;
	afn->OPMD.bit.OPMD0 = E_IM_IIP_PARAM_OPMD_ADD;
	afn->header1.bit.BeginningAddress = 0x0030;
	afn->header1.bit.WordLength = 14;
	afn->header1.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET;
	afn->OPY.bit.OPBY = 0;
	afn->OPY.bit.OPAY = 0x10;	// 1.0
	afn->OPB.bit.OPBB = 0;
	afn->OPB.bit.OPAB = 0x10;	// 1.0
	afn->OPR.bit.OPBR = 0;
	afn->OPR.bit.OPAR = 0x10;	// 1.0
	afn->OPA.bit.OPBA = 0;
	afn->OPA.bit.OPAA = 0x10;	// 1.0
	afn->CLIPLVLY.bit.CLIPLVLYH = 0xFFFF;
	afn->CLIPLVLY.bit.CLIPLVLYL = 0;
	afn->CLIPLVLB.bit.CLIPLVLBH = 0xFFFF;
	afn->CLIPLVLB.bit.CLIPLVLBL = 0;
	afn->CLIPLVLR.bit.CLIPLVLRH = 0xFFFF;
	afn->CLIPLVLR.bit.CLIPLVLRL = 0;
	afn->CLIPLVLA.bit.CLIPLVLAH = 0xFFFF;
	afn->CLIPLVLA.bit.CLIPLVLAL = 0;
	afn->AFNPCNT.bit.AFNPHCNT = 1;
	afn->AFNPCNT.bit.AFNPVCNT = 1;
	afn->AFNPA.word = (ULONG)afntbl;
}

static VOID sample_create_unitparam_sl( T_IM_IIP_PARAM_STX* const sl, const T_SAMPLE_IMGINFO* const dst_img,
	const T_IM_IIP_PARAM_AFN* const afn, const T_IM_IIP_PARAM_AFN_OPCOL_0* const afntbl )
{
	sl->header0.bit.BeginningAddress = 0x0000;
	sl->header0.bit.WordLength = 6;
	sl->header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY;
	sl->SL_TOPCNF0.bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE;
//	sl->SL_TOPCNF0.bit.DATACONF;		// set later
//	sl->PIXIDDEF.bit.OPIXID;			// set later
	sl->PIXIDDEF.bit.CSEL = E_IM_IIP_PARAM_CSEL_0;	// dummy
	sl->PIXIDDEF.bit.DIV1ST = E_IM_IIP_PARAM_DIV1ST_1ST;
	sl->PIXIDDEF.bit.LKDIV = 0;

	sl->PLDUNIT_SL.bit.PLDUNIT_SL = D_IM_IIP_PARAM_PLDUNIT_NONE;

	sl->SL_CTL.bit.SL_MODE = 0;
	sl->SL_CTL.bit.LOADMD = 0;
	sl->SL_CTL.bit.MASKEN = E_IM_IIP_PARAM_MASKEN_NO_MASK;

	sl->header1.bit.BeginningAddress = 0x0020;
	sl->header1.bit.WordLength = 10;
	sl->header1.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY;
	sl->SL_HSIZE.bit.HSTART = 0;
	sl->SL_HSIZE.bit.HEND = dst_img->width - 1;
	sl->SL_VSIZE.bit.VSTART = 0;
	sl->SL_VSIZE.bit.VEND = dst_img->lines - 1;
	sl->SL_PHSZ.bit.PHSZ1 = ( E_IM_IIP_PARAM_GHSZ_64PIX == afn->AFNCTL.bit.GHSZ ) ? 64 : 32;
	sl->SL_PVSZ.bit.PVSZ1 = ( E_IM_IIP_PARAM_GVSZ_SAME_AS_VERTICAL_PIX == afn->AFNCTL.bit.GVSZ ) ? afntbl->SZ.bit.AFNDVSZ :
		                    ( E_IM_IIP_PARAM_GVSZ_256PIX == afn->AFNCTL.bit.GVSZ ) ? 256 :
		                    ( E_IM_IIP_PARAM_GVSZ_128PIX == afn->AFNCTL.bit.GVSZ ) ? 128 :
		                    ( E_IM_IIP_PARAM_GVSZ_64PIX  == afn->AFNCTL.bit.GVSZ ) ? 64 :
		                    ( E_IM_IIP_PARAM_GVSZ_32PIX  == afn->AFNCTL.bit.GVSZ ) ? 32 :
		                    ( E_IM_IIP_PARAM_GVSZ_16PIX  == afn->AFNCTL.bit.GVSZ ) ? 16 :
		                    ( E_IM_IIP_PARAM_GVSZ_8PIX   == afn->AFNCTL.bit.GVSZ ) ? 8 :
				            ( E_IM_IIP_PARAM_GVSZ_4PIX   == afn->AFNCTL.bit.GVSZ ) ? 4 : 2;
	sl->SL_PHSZ.bit.PHSZ0 = sl->SL_PHSZ.bit.PHSZ1;
	sl->SL_PVSZ.bit.PVSZ0 = sl->SL_PVSZ.bit.PVSZ1;

	sl->SEL_OPARA.bit.SEL_OPARA = 0;

	sl->header2.bit.BeginningAddress = 0x0048;
	sl->header2.bit.WordLength = 8;	// only output option 0
	sl->header2.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET;
	sl->OFSET_0.bit.OFSET = 0;
	sl->CLPTH_U_0.bit.CLPTH_U = 0x3FFF;
	sl->CLPTH_L_0.bit.CLPTH_L = -0x3FFF;
	sl->CLPVAL_U_0.bit.CLPVAL_U = sl->CLPTH_U_0.bit.CLPTH_U;
	sl->CLPVAL_L_0.bit.CLPVAL_L = sl->CLPTH_L_0.bit.CLPTH_L;
	sl->SFT_0.bit.SFTVAL = 0;
	sl->SFT_0.bit.SFTM = 0;
	sl->W14MD_0.bit.W14LSFT = 0;
	sl->W14MD_0.bit.W14SAT = 0;

	// note : Followings are not used.
//	sl->OFSET_1.bit.OFSET;
//	sl->CLPTH_U_1.bit.CLPTH_U;
//	sl->CLPTH_L_1.bit.CLPTH_L;
//	sl->CLPVAL_U_1.bit.CLPVAL_U;
//	sl->CLPVAL_L_1.bit.CLPVAL_L;
//	sl->SFT1_0.bit.SFTVAL;
//	sl->SFT1_1.bit.SFTMD;
//	sl->W14MD1.bit.W14LSFT;
//	sl->W14MD1.bit.W14SAT;
}

static INT32 sample_set_pixfmttbl( const UINT32 pixid, T_IM_IIP_PIXFMTTBL* const pixfmttbl, const T_SAMPLE_IMGINFO* const img, const UCHAR master_if )
{
	pixfmttbl->pix_depth = D_IM_IIP_PDEPTH_8BITS;
	pixfmttbl->alpha = D_IM_IIP_ALPHA_FALSE;
	pixfmttbl->alpha_depth = E_IM_IIP_ADEPTH_8BITS;
	pixfmttbl->alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP;
	pixfmttbl->frame_type = img->frame_type;
	pixfmttbl->pix_format = img->pix_format;
	pixfmttbl->chunky_color.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G;
	pixfmttbl->chunky_color.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B;
	pixfmttbl->chunky_color.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R;
	pixfmttbl->chunky_color.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A;
	pixfmttbl->sign_Y_G = D_IM_IIP_UNSIGNED_DATA;
	pixfmttbl->sign_Cb_B = D_IM_IIP_UNSIGNED_DATA;
	pixfmttbl->sign_Cr_R = D_IM_IIP_UNSIGNED_DATA;
	pixfmttbl->sign_D3 = D_IM_IIP_UNSIGNED_DATA;
	pixfmttbl->masterIF_Y_G = master_if;
	pixfmttbl->masterIF_Cb_B = master_if;
	pixfmttbl->masterIF_Cr_R = master_if;
	pixfmttbl->masterIF_Alpha = master_if;
	pixfmttbl->pix_outside_img = D_IM_IIP_OUTSIDE_IMG_EDGE;
//	pixfmttbl->Y_G_val_outside_img;		// no use
//	pixfmttbl->Cb_B_val_outside_img;	// no use
//	pixfmttbl->Cr_R_val_outside_img;	// no use
//	pixfmttbl->Alpha_val_outside_img;	// no use
	pixfmttbl->width = img->width;
	pixfmttbl->lines = img->lines;
	pixfmttbl->line_bytes.Y_G = img->g_width.Y_G;
	pixfmttbl->line_bytes.Cb_B = img->g_width.Cb_B;
	pixfmttbl->line_bytes.Cr_R = img->g_width.Cr_R;
	pixfmttbl->line_bytes.Alpha = img->g_width.Alpha;
	pixfmttbl->addr.Y_G = img->addr.Y_G;
	pixfmttbl->addr.Cb_B = img->addr.Cb_B;
	pixfmttbl->addr.Cr_R = img->addr.Cr_R;
	pixfmttbl->addr.Alpha = img->addr.Alpha;

	return Im_IIP_Ctrl_PIXFMTTBL( pixid, pixfmttbl );
}

static INT32 sample_set_unitparam_swtrg( const E_IM_IIP_UNIT_ID unit_id, const VOID* const unit_param, const UINT32 unitid_bitmask )
{
	T_IM_IIP_UNIT_CFG unitcfg = {
		.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG,
		.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE,
		.unit_param_addr = (ULONG)unit_param,
		.load_unit_param_flag = unitid_bitmask,
	};

	return Im_IIP_Ctrl_SWTRG_Unit( unit_id, &unitcfg );
}

static INT32 sample_set_axi( const E_IM_IIP_AXI_ID axi_id, const UCHAR master_if )
{
	T_IM_IIP_CTRL_AXI			axi_cfg = {
		.trans_req_ctrl = 0,	// register default
		.masterIF = master_if,
		.cache_type = 1,		// register default
		.protection_type = 0,	// register default
	};

	return Im_IIP_Ctrl_AXI( axi_id, &axi_cfg );
}

VOID sample_reduce_half_swtrg( const T_SAMPLE_IMGINFO* const src_img, const T_SAMPLE_IMGINFO* const dst_img )
{
	UINT32	unitid_bitmask = D_SAMPLE_AFN_PLDUNIT | D_SAMPLE_SL_PLDUNIT;
	UINT32	pixid_bitmask = D_SAMPLE_SRC_PIXID_MASK | D_SAMPLE_DST_PIXID_MASK;
	UINT32	open_res_bitmask = E_IM_IIP_OPEN_RES_NONE;
	INT32	wait_time = 1000;
	UINT32	wait_factor_res;
	UINT32	wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_SAMPLE_INT_FACTOR;
	INT32	ercd;
	T_IM_IIP_PIXFMTTBL			pixfmttbl_src;
	T_IM_IIP_PIXFMTTBL			pixfmttbl_dst;

	if( D_SAMPLE_AFN_RESIZE_MODE == E_IM_IIP_PARAM_ITMD_BI_CUBIC ) {
		open_res_bitmask |= E_IM_IIP_OPEN_RES_BICUBIC0;
	}
	if( D_SAMPLE_AFN_CSEL == E_IM_IIP_PARAM_CSEL_0 ) {
		open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE0;
	}
	else {
		open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE1;
	}

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, wait_time );
	printf( "Im_IIP_Open_SWTRG result = 0x%x.\n", ercd );
	if( ercd != D_IM_IIP_OK ) {
		return;
	}

	while( 1 ) {
		sample_create_afntbl( &g_sample_afntbl, dst_img );
		sample_create_unitparam_afn( &g_sample_unit_param_afn, &g_sample_afntbl );
		sample_create_unitparam_sl( &g_sample_unit_param_sl, dst_img, &g_sample_unit_param_afn, &g_sample_afntbl );

		g_sample_unit_param_afn.AFNTOPCNF.bit.WAITCONF = D_SAMPLE_SL_PORTID;
		g_sample_unit_param_afn.PIXIDEF.bit.IPIXID = D_SAMPLE_SRC_PIXID_VAL;
		g_sample_unit_param_sl.SL_TOPCNF0.bit.DATACONF = D_SAMPLE_AFN_PORTID;
		g_sample_unit_param_sl.PIXIDDEF.bit.OPIXID = D_SAMPLE_DST_PIXID_VAL;

		ercd = sample_set_unitparam_swtrg( D_SAMPLE_AFN_UNITID, &g_sample_unit_param_afn, unitid_bitmask );
		printf( "sample_set_unitparam afn result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		ercd = sample_set_unitparam_swtrg( D_SAMPLE_SL_UNITID, &g_sample_unit_param_sl, unitid_bitmask );
		printf( "sample_set_unitparam sl result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		ercd = sample_set_pixfmttbl( D_SAMPLE_SRC_PIXID_VAL, &pixfmttbl_src, src_img, D_SAMPLE_SRC_MASTER_IF );
		printf( "sample_set_pixfmttbl result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}
		ercd = sample_set_pixfmttbl( D_SAMPLE_DST_PIXID_VAL, &pixfmttbl_dst, dst_img, D_SAMPLE_DST_MASTER_IF );
		printf( "sample_set_pixfmttbl result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		ercd = sample_set_axi( D_SAMPLE_AFN_AXIID, D_SAMPLE_SRC_MASTER_IF );
		printf( "sample_set_axi result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}
		ercd = sample_set_axi( D_SAMPLE_SL_AXIID, D_SAMPLE_DST_MASTER_IF );
		printf( "sample_set_axi result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		ercd = Im_IIP_Set_Interrupt( wait_factor, 1 );
		printf( "Im_IIP_Set_Interrupt result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		BF_L1l2cache_Clean_Addr( (ULONG)&g_sample_afntbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
		BF_L1l2cache_Clean_Addr( (ULONG)&g_sample_unit_param_afn, sizeof(T_IM_IIP_PARAM_AFN) );
		BF_L1l2cache_Clean_Addr( (ULONG)&g_sample_unit_param_sl, sizeof(T_IM_IIP_PARAM_STX) );

		ercd = Im_IIP_Start_SWTRG( D_SAMPLE_SL_UNITID, D_IM_IIP_SWTRG_ON );
		printf( "Im_IIP_Start_SWTRG sl result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
				break;
		}

		ercd = Im_IIP_Start_SWTRG( D_SAMPLE_AFN_UNITID, D_IM_IIP_SWTRG_ON );
		printf( "Im_IIP_Start_SWTRG afn result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
				break;
		}

		ercd = Im_IIP_Wait_End( &wait_factor_res,
								  wait_factor,
								  D_IM_IIP_OR_WAIT,
								  wait_time );
		printf( "Im_IIP_Wait_End AXI result = 0x%x. wait=0x%x\n", ercd, wait_factor_res );
		if( ercd == D_IM_IIP_OK ) {
			if( wait_factor_res & D_IM_IIP_INT_FACTOR_AXIERR ) {
				printf( "Im_IIP_Wait_End AXI Error\n" );
				ercd = D_IM_IIP_ERR;
			}
		}

		if( D_SAMPLE_AFN_RESIZE_MODE == E_IM_IIP_PARAM_ITMD_BI_CUBIC ) {
			ercd = Im_IIP_Free_CubicUnit( D_SAMPLE_AFN_UNITID );
			printf( "Im_IIP_Free_CubicUnit result = 0x%x.\n", ercd );
			if( ercd != D_IM_IIP_OK ) {
				break;
			}
		}

		break;
	}

	ercd = Im_IIP_Close_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask );
	printf( "Im_IIP_Close_SWTRG result = 0x%x.\n", ercd );
}

VOID sample_main_swtrg( VOID )
{
	T_SAMPLE_IMGINFO				src_img = {
		.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,
		.pix_format = E_IM_IIP_PFMT_YCC422,
		.addr = {
			.Y_G   = 0x20000000,
			.Cb_B  = 0x21000000,
			.Cr_R  = 0x22000000,
			.Alpha = 0x20000000,	// dummy
		},
		.g_width = {
			.Y_G   = 1280,
			.Cb_B  = 1280 /2,
			.Cr_R  = 1280 /2,
			.Alpha = 1280,			// dummy
		},
		.width = 1280,
		.lines = 720,
	};
	T_SAMPLE_IMGINFO				dst_img = {
		.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,
		.pix_format = E_IM_IIP_PFMT_YCC422,
		.addr = {
			.Y_G   = 0x23000000,
			.Cb_B  = 0x24000000,
			.Cr_R  = 0x25000000,
			.Alpha = 0x23000000,	// dummy
		},
		.g_width = {
			.Y_G   = 640,
			.Cb_B  = 640 /2,
			.Cr_R  = 640 /2,
			.Alpha = 640,			// dummy
		},
		.width = 640,
		.lines = 360,
	};

	sample_reduce_half_swtrg( &src_img, &dst_img );
}

@endcode
<br><br>

@}
*/
/**
@weakgroup im_iip
@{
@section im_iip_hwtrg	Sample code of Single frame reducing to 1/2 using HWTRG.
@code
#include "im_iip.h"
#include "l1l2cache.h"
#include "ddim_typedef.h"

#include <stdio.h>
#include <string.h>


typedef struct {
	E_IM_IIP_FTYPE		frame_type;
	E_IM_IIP_PFMT		pix_format;
	T_IM_IIP_IMG_ADDR	addr;
	T_IM_IIP_LINE_BYTES	g_width;
	USHORT				width;
	USHORT				lines;
} T_SAMPLE_IMGINFO;

static __align(8) T_IM_IIP_PARAM_AFN			g_sample_unit_param_afn;	// Needs 8bytes align
static __align(8) T_IM_IIP_PARAM_STX			g_sample_unit_param_sl;		// Needs 8bytes align
static __align(8) T_IM_IIP_PARAM_AFN_OPCOL_0	g_sample_afntbl;			// Needs 8bytes align

#define D_SAMPLE_AFN_PORTID				E_IM_IIP_PARAM_PORTID_AFN0
#define D_SAMPLE_AFN_AXI_MEMB			afn0
#define D_SAMPLE_AFN_RESIZE_MODE		E_IM_IIP_PARAM_ITMD_BI_CUBIC
#define D_SAMPLE_AFN_CSEL				E_IM_IIP_PARAM_CSEL_0
#define D_SAMPLE_AFN_CUBSEL				E_IM_IIP_PARAM_CUBSEL_AFN0
#define D_SAMPLE_AFN_NO					0

#define D_SAMPLE_SL_PORTID				E_IM_IIP_PARAM_PORTID_SL2
#define D_SAMPLE_SL_AXI_MEMB			sl2
#define D_SAMPLE_INT_FACTOR				D_IM_IIP_INT_FACTOR_SL2END
#define D_SAMPLE_SL_NO					2

#define D_SAMPLE_SRC_PIXID_VAL			4

#define D_SAMPLE_DST_PIXID_VAL			5

#define D_SAMPLE_SRC_MASTER_IF			D_IM_IIP_MASTER_IF0
#define D_SAMPLE_DST_MASTER_IF			D_IM_IIP_MASTER_IF1


#define D_SAMPLE_AFNTBL_1_0				(0x10000UL)
#define sample_calc_afntbl_pos( f_val )	( (ULONG)((f_val) * D_SAMPLE_AFNTBL_1_0) )


static VOID sample_create_afntbl( T_IM_IIP_PARAM_AFN_OPCOL_0* const afntbl, const T_SAMPLE_IMGINFO* const dst_img )
{
	afntbl->STAXY0.bit.STAX = sample_calc_afntbl_pos( 0.0 );
	afntbl->STAXY0.bit.STAY = sample_calc_afntbl_pos( 0.0 );

	afntbl->D_OUT_IN_0.bit.DXX = sample_calc_afntbl_pos( 0.5 );		// reduce to 1/2
	afntbl->D_OUT_IN_0.bit.DXY = sample_calc_afntbl_pos( 0.0 );
	afntbl->D_OUT_IN_0.bit.DYX = sample_calc_afntbl_pos( 0.0 );
	afntbl->D_OUT_IN_0.bit.DYY = sample_calc_afntbl_pos( 0.5 );		// reduce to 1/2
	afntbl->SZ.bit.AFNDVSZ = dst_img->lines;
	afntbl->SZ.bit.AFNDHSZ = dst_img->width;
}

static VOID sample_create_unitparam_afn( T_IM_IIP_PARAM_AFN* const afn, const T_IM_IIP_PARAM_AFN_OPCOL_0* const afntbl )
{
	afn->header0.bit.BeginningAddress = 0x0000;
	afn->header0.bit.WordLength = 10;
	afn->header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY;
	afn->AFNTOPCNF.bit.DATACONF = 0;
//	afn->AFNTOPCNF.bit.WAITCONF;							// set later
	afn->AFNLDCNF.bit.UPSU = E_IM_IIP_PARAM_UPSU_NONE;
	afn->AFNLDCNF.bit.DWSU = E_IM_IIP_PARAM_DWSU_NONE;
	afn->AFNLDCNF.bit.DTHD = E_IM_IIP_PARAM_DTHD_NORMAL;
	afn->AFNLDCNF.bit.CBC = E_IM_IIP_PARAM_CBC_DISABLE;
	afn->AFNCTL.bit.LPLVL = E_IM_IIP_PARAM_LPLVL_L_LEVEL;
	afn->AFNCTL.bit.HDBL = E_IM_IIP_PARAM_HDBL_SAME_AS_PICHSZ;
	afn->AFNCTL.bit.VDBL = E_IM_IIP_PARAM_VDBL_SAME_AS_PICVSZ;
	afn->AFNCTL.bit.GHSZ = E_IM_IIP_PARAM_GHSZ_32PIX;		// Needs tuning
	afn->AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_8PIX;		// Needs tuning
	afn->AFNCTL.bit.HSMD = E_IM_IIP_PARAM_HSMD_NORMAL;
	afn->PFCTL.bit.PFEXTMC = 4;
	afn->PFCTL.bit.PFEXTADC = 64;							// Needs tuning
	afn->PFCTL.bit.PFSP1 = E_IM_IIP_PARAM_PFSP_1;			// Needs tuning
	afn->PFCTL.bit.PFSP2 = E_IM_IIP_PARAM_PFSP_16;			// Needs tuning
	afn->PFCTL.bit.PFVSP = E_IM_IIP_PARAM_PFVSP_VERTICAL_0;	// Needs tuning
	afn->PFCTL.bit.PFOFF = E_IM_IIP_PARAM_PFOFF_RUN;
	afn->PIXIDEF.bit.EXA = E_IM_IIP_PARAM_EXA_NO_ALPHA_TO_NEXT_UNIT;
	afn->PIXIDEF.bit.CSEL = D_SAMPLE_AFN_CSEL;
//	afn->PIXIDEF.bit.IPIXID;								// set later
	afn->OUTSIZE.bit.OUTHSZ = 0;
	afn->OUTSIZE.bit.OUTVSZ = 0;
	afn->OUTSTARTPOS.bit.STARTX = 0;
	afn->OUTSTARTPOS.bit.STARTY = 0;
	afn->CALMETHOD.bit.OPCOL = 0;
	afn->CALMETHOD.bit.ITMD = D_SAMPLE_AFN_RESIZE_MODE;
	afn->CALMETHOD.bit.FILMD = 0;
	afn->CALMETHOD.bit.DANTI = 1;
	afn->CALMETHOD.bit.AANTI = 1;
	afn->CALMETHOD.bit.CUBSEL = D_SAMPLE_AFN_CUBSEL;
	afn->FILVAL0.bit.FILVALYG = 0x0;
	afn->FILVAL0.bit.FILVALB = 0x0;
	afn->FILVAL1.bit.FILVALR = 0x0;
	afn->FILVAL1.bit.FILVALA = 0x0;
	afn->OPMD.bit.OPMD0 = E_IM_IIP_PARAM_OPMD_ADD;
	afn->header1.bit.BeginningAddress = 0x0030;
	afn->header1.bit.WordLength = 14;
	afn->header1.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET;
	afn->OPY.bit.OPBY = 0;
	afn->OPY.bit.OPAY = 0x10;	// 1.0
	afn->OPB.bit.OPBB = 0;
	afn->OPB.bit.OPAB = 0x10;	// 1.0
	afn->OPR.bit.OPBR = 0;
	afn->OPR.bit.OPAR = 0x10;	// 1.0
	afn->OPA.bit.OPBA = 0;
	afn->OPA.bit.OPAA = 0x10;	// 1.0
	afn->CLIPLVLY.bit.CLIPLVLYH = 0xFFFF;
	afn->CLIPLVLY.bit.CLIPLVLYL = 0;
	afn->CLIPLVLB.bit.CLIPLVLBH = 0xFFFF;
	afn->CLIPLVLB.bit.CLIPLVLBL = 0;
	afn->CLIPLVLR.bit.CLIPLVLRH = 0xFFFF;
	afn->CLIPLVLR.bit.CLIPLVLRL = 0;
	afn->CLIPLVLA.bit.CLIPLVLAH = 0xFFFF;
	afn->CLIPLVLA.bit.CLIPLVLAL = 0;
	afn->AFNPCNT.bit.AFNPHCNT = 1;
	afn->AFNPCNT.bit.AFNPVCNT = 1;
	afn->AFNPA.word = (ULONG)afntbl;
}

static VOID sample_create_unitparam_sl( T_IM_IIP_PARAM_STX* const sl, const T_SAMPLE_IMGINFO* const dst_img,
	const T_IM_IIP_PARAM_AFN* const afn, const T_IM_IIP_PARAM_AFN_OPCOL_0* const afntbl )
{
	sl->header0.bit.BeginningAddress = 0x0000;
	sl->header0.bit.WordLength = 6;
	sl->header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY;
	sl->SL_TOPCNF0.bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE;
//	sl->SL_TOPCNF0.bit.DATACONF;		// set later
//	sl->PIXIDDEF.bit.OPIXID;			// set later
	sl->PIXIDDEF.bit.CSEL = E_IM_IIP_PARAM_CSEL_0;	// dummy
	sl->PIXIDDEF.bit.DIV1ST = E_IM_IIP_PARAM_DIV1ST_1ST;
	sl->PIXIDDEF.bit.LKDIV = 0;

	sl->PLDUNIT_SL.bit.PLDUNIT_SL = D_IM_IIP_PARAM_PLDUNIT_NONE;

	sl->SL_CTL.bit.SL_MODE = 0;
	sl->SL_CTL.bit.LOADMD = 0;
	sl->SL_CTL.bit.MASKEN = E_IM_IIP_PARAM_MASKEN_NO_MASK;

	sl->header1.bit.BeginningAddress = 0x0020;
	sl->header1.bit.WordLength = 10;
	sl->header1.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY;
	sl->SL_HSIZE.bit.HSTART = 0;
	sl->SL_HSIZE.bit.HEND = dst_img->width - 1;
	sl->SL_VSIZE.bit.VSTART = 0;
	sl->SL_VSIZE.bit.VEND = dst_img->lines - 1;
	sl->SL_PHSZ.bit.PHSZ1 = ( E_IM_IIP_PARAM_GHSZ_64PIX == afn->AFNCTL.bit.GHSZ ) ? 64 : 32;
	sl->SL_PVSZ.bit.PVSZ1 = ( E_IM_IIP_PARAM_GVSZ_SAME_AS_VERTICAL_PIX == afn->AFNCTL.bit.GVSZ ) ? afntbl->SZ.bit.AFNDVSZ :
		                    ( E_IM_IIP_PARAM_GVSZ_256PIX == afn->AFNCTL.bit.GVSZ ) ? 256 :
		                    ( E_IM_IIP_PARAM_GVSZ_128PIX == afn->AFNCTL.bit.GVSZ ) ? 128 :
		                    ( E_IM_IIP_PARAM_GVSZ_64PIX  == afn->AFNCTL.bit.GVSZ ) ? 64 :
		                    ( E_IM_IIP_PARAM_GVSZ_32PIX  == afn->AFNCTL.bit.GVSZ ) ? 32 :
		                    ( E_IM_IIP_PARAM_GVSZ_16PIX  == afn->AFNCTL.bit.GVSZ ) ? 16 :
		                    ( E_IM_IIP_PARAM_GVSZ_8PIX   == afn->AFNCTL.bit.GVSZ ) ? 8 :
				            ( E_IM_IIP_PARAM_GVSZ_4PIX   == afn->AFNCTL.bit.GVSZ ) ? 4 : 2;
	sl->SL_PHSZ.bit.PHSZ0 = sl->SL_PHSZ.bit.PHSZ1;
	sl->SL_PVSZ.bit.PVSZ0 = sl->SL_PVSZ.bit.PVSZ1;

	sl->SEL_OPARA.bit.SEL_OPARA = 0;

	sl->header2.bit.BeginningAddress = 0x0048;
	sl->header2.bit.WordLength = 8;	// only output option 0
	sl->header2.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET;
	sl->OFSET_0.bit.OFSET = 0;
	sl->CLPTH_U_0.bit.CLPTH_U = 0x3FFF;
	sl->CLPTH_L_0.bit.CLPTH_L = -0x3FFF;
	sl->CLPVAL_U_0.bit.CLPVAL_U = sl->CLPTH_U_0.bit.CLPTH_U;
	sl->CLPVAL_L_0.bit.CLPVAL_L = sl->CLPTH_L_0.bit.CLPTH_L;
	sl->SFT_0.bit.SFTVAL = 0;
	sl->SFT_0.bit.SFTM = 0;
	sl->W14MD_0.bit.W14LSFT = 0;
	sl->W14MD_0.bit.W14SAT = 0;

	// note : Followings are not used.
//	sl->OFSET_1.bit.OFSET;
//	sl->CLPTH_U_1.bit.CLPTH_U;
//	sl->CLPTH_L_1.bit.CLPTH_L;
//	sl->CLPVAL_U_1.bit.CLPVAL_U;
//	sl->CLPVAL_L_1.bit.CLPVAL_L;
//	sl->SFT1_0.bit.SFTVAL;
//	sl->SFT1_1.bit.SFTMD;
//	sl->W14MD1.bit.W14LSFT;
//	sl->W14MD1.bit.W14SAT;
}

static INT32 sample_set_pixfmttbl( const UINT32 pixid, T_IM_IIP_PIXFMTTBL* const pixfmttbl, const T_SAMPLE_IMGINFO* const img, const UCHAR master_if )
{
	pixfmttbl->pix_depth = D_IM_IIP_PDEPTH_8BITS;
	pixfmttbl->alpha = D_IM_IIP_ALPHA_FALSE;
	pixfmttbl->alpha_depth = E_IM_IIP_ADEPTH_8BITS;
	pixfmttbl->alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP;
	pixfmttbl->frame_type = img->frame_type;
	pixfmttbl->pix_format = img->pix_format;
	pixfmttbl->chunky_color.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G;
	pixfmttbl->chunky_color.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B;
	pixfmttbl->chunky_color.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R;
	pixfmttbl->chunky_color.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A;
	pixfmttbl->sign_Y_G = D_IM_IIP_UNSIGNED_DATA;
	pixfmttbl->sign_Cb_B = D_IM_IIP_UNSIGNED_DATA;
	pixfmttbl->sign_Cr_R = D_IM_IIP_UNSIGNED_DATA;
	pixfmttbl->sign_D3 = D_IM_IIP_UNSIGNED_DATA;
	pixfmttbl->masterIF_Y_G = master_if;
	pixfmttbl->masterIF_Cb_B = master_if;
	pixfmttbl->masterIF_Cr_R = master_if;
	pixfmttbl->masterIF_Alpha = master_if;
	pixfmttbl->pix_outside_img = D_IM_IIP_OUTSIDE_IMG_EDGE;
//	pixfmttbl->Y_G_val_outside_img;		// no use
//	pixfmttbl->Cb_B_val_outside_img;	// no use
//	pixfmttbl->Cr_R_val_outside_img;	// no use
//	pixfmttbl->Alpha_val_outside_img;	// no use
	pixfmttbl->width = img->width;
	pixfmttbl->lines = img->lines;
	pixfmttbl->line_bytes.Y_G = img->g_width.Y_G;
	pixfmttbl->line_bytes.Cb_B = img->g_width.Cb_B;
	pixfmttbl->line_bytes.Cr_R = img->g_width.Cr_R;
	pixfmttbl->line_bytes.Alpha = img->g_width.Alpha;
	pixfmttbl->addr.Y_G = img->addr.Y_G;
	pixfmttbl->addr.Cb_B = img->addr.Cb_B;
	pixfmttbl->addr.Cr_R = img->addr.Cr_R;
	pixfmttbl->addr.Alpha = img->addr.Alpha;

	return Im_IIP_Ctrl_PIXFMTTBL( pixid, pixfmttbl );
}

static INT32 sample_set_unitparam_hwtrg( const T_IM_IIP_PARAM_AFN* const afn, const T_IM_IIP_PARAM_STX* const sl )
{
	T_IM_IIP_UNITINFTBL unittbl;

	T_IM_IIP_UNIT_CFG unitcfg_afn = {
		.unit_ctrl = D_IM_IIP_HW_CTRL_AUTO,
		.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE,
//		.unit_param_addr									// set later
		.load_unit_param_flag = 0,
	};
	T_IM_IIP_UNIT_CFG unitcfg_sl = {
		.unit_ctrl = D_IM_IIP_HW_CTRL_AUTO,
		.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE,
//		.unit_param_addr									// set later
		.load_unit_param_flag = 0,
	};


	unitcfg_afn.unit_param_addr = (ULONG)afn;
	unitcfg_sl.unit_param_addr = (ULONG)sl;

	memset( &unittbl, '\0', sizeof(unittbl) );
	unittbl.pAFN[D_SAMPLE_AFN_NO] = &unitcfg_afn;
	unittbl.pSL[D_SAMPLE_SL_NO] = &unitcfg_sl;

	return Im_IIP_Ctrl_UNITINFTBL( &unittbl );
}

static INT32 sample_set_base( UINT32 wait_factor )
{
	T_IM_IIP_CTRL_BASE base = {
		.b2y_link = D_IM_IIP_LINK_B2Y_OFF,
		.b2y_link = D_IM_IIP_LINK_B2Y_OFF,
		.sl2_interrupt_lines = 0,
		.sl3_interrupt_lines = 0,
		.sl4_interrupt_lines = 0,
		.sl5_interrupt_lines = 0,
		.sl6_interrupt_lines = 0,
		.int_handler = NULL,
		.cache_flush_ctrl[0] = D_IM_IIP_FLUSH_IMGCACHE_START,
		.cache_flush_ctrl[1] = D_IM_IIP_FLUSH_IMGCACHE_START,
		.ext_bit_ctrl_less_8bits[0] = D_IM_IIP_EXTBIT_LOWER_ALIGNED,
		.ext_bit_ctrl_less_8bits[1] = D_IM_IIP_EXTBIT_LOWER_ALIGNED,
	};

	base.enable_interrupt_flag = wait_factor;

	return Im_IIP_Ctrl_Base( &base );
}

static INT32 sample_set_access_enable( const UCHAR permission_flg )
{
	UINT32 res_bitmask = 0;

	if( D_SAMPLE_AFN_CSEL == E_IM_IIP_PARAM_CSEL_0 ) {
		return D_IM_IIP_OK;
	}
	else {
		res_bitmask |= E_IM_IIP_OPEN_RES_CACHE1;
	}
	return Im_IIP_Set_AccessEnable( res_bitmask, permission_flg );
}

static INT32 sample_set_axi_all( const UCHAR master_if_afn, const UCHAR master_if_sl )
{
	T_IM_IIP_CTRL_AXI_ALL axi_all;
	T_IM_IIP_CTRL_AXI			axi_cfg_afn = {
		.trans_req_ctrl = 0,	// register default
		.masterIF = master_if_afn,
		.cache_type = 1,		// register default
		.protection_type = 0,	// register default
	};
	T_IM_IIP_CTRL_AXI			axi_cfg_sl = {
		.trans_req_ctrl = 0,	// register default
		.masterIF = master_if_sl,
		.cache_type = 1,		// register default
		.protection_type = 0,	// register default
	};

	memset( &axi_all, '\0', sizeof(axi_all) );
	axi_all.D_SAMPLE_AFN_AXI_MEMB = &axi_cfg_afn;
	axi_all.D_SAMPLE_SL_AXI_MEMB = &axi_cfg_sl;

	return Im_IIP_Ctrl_AXI_ALL( &axi_all );
}

VOID sample_reduce_half_hwtrg( const T_SAMPLE_IMGINFO* const src_img, const T_SAMPLE_IMGINFO* const dst_img )
{
	INT32	wait_time = 1000;
	UINT32	wait_factor_res;
	UINT32	wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_SAMPLE_INT_FACTOR;
	INT32	ercd;
	INT32	ercd2;
	T_IM_IIP_PIXFMTTBL			pixfmttbl_src;
	T_IM_IIP_PIXFMTTBL			pixfmttbl_dst;

	ercd = Im_IIP_Open( wait_time );
	printf( "Im_IIP_Open result = 0x%x.\n", ercd );
	if( ercd != D_IM_IIP_OK ) {
		return;
	}

	while( 1 ) {
		sample_create_afntbl( &g_sample_afntbl, dst_img );
		sample_create_unitparam_afn( &g_sample_unit_param_afn, &g_sample_afntbl );
		sample_create_unitparam_sl( &g_sample_unit_param_sl, dst_img, &g_sample_unit_param_afn, &g_sample_afntbl );

		g_sample_unit_param_afn.AFNTOPCNF.bit.WAITCONF = D_SAMPLE_SL_PORTID;
		g_sample_unit_param_afn.PIXIDEF.bit.IPIXID = D_SAMPLE_SRC_PIXID_VAL;
		g_sample_unit_param_sl.SL_TOPCNF0.bit.DATACONF = D_SAMPLE_AFN_PORTID;
		g_sample_unit_param_sl.PIXIDDEF.bit.OPIXID = D_SAMPLE_DST_PIXID_VAL;

		ercd = sample_set_base( wait_factor );
		printf( "sample_set_base result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		ercd = sample_set_unitparam_hwtrg( &g_sample_unit_param_afn, &g_sample_unit_param_sl );
		printf( "sample_set_unitparam_hwtrg sl result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		ercd = sample_set_pixfmttbl( D_SAMPLE_SRC_PIXID_VAL, &pixfmttbl_src, src_img, D_SAMPLE_SRC_MASTER_IF );
		printf( "sample_set_pixfmttbl result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}
		ercd = sample_set_pixfmttbl( D_SAMPLE_DST_PIXID_VAL, &pixfmttbl_dst, dst_img, D_SAMPLE_DST_MASTER_IF );
		printf( "sample_set_pixfmttbl result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		ercd = sample_set_axi_all( D_SAMPLE_SRC_MASTER_IF, D_SAMPLE_DST_MASTER_IF );
		printf( "sample_set_axi_all result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		ercd = sample_set_access_enable( 1 );
		printf( "sample_set_access_enable result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		BF_L1l2cache_Clean_Addr( (ULONG)&g_sample_afntbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
		BF_L1l2cache_Clean_Addr( (ULONG)&g_sample_unit_param_afn, sizeof(T_IM_IIP_PARAM_AFN) );
		BF_L1l2cache_Clean_Addr( (ULONG)&g_sample_unit_param_sl, sizeof(T_IM_IIP_PARAM_STX) );

		ercd = Im_IIP_Start();
		printf( "Im_IIP_Start result = 0x%x\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
				break;
		}

		ercd = Im_IIP_Wait_End( &wait_factor_res,
								  wait_factor,
								  D_IM_IIP_OR_WAIT,
								  wait_time );
		printf( "Im_IIP_Wait_End AXI result = 0x%x. wait=0x%x\n", ercd, wait_factor_res );
		if( ercd == D_IM_IIP_OK ) {
			if( wait_factor_res & D_IM_IIP_INT_FACTOR_AXIERR ) {
				printf( "Im_IIP_Wait_End AXI Error\n" );
				ercd = D_IM_IIP_ERR;
			}
		}

		ercd = Im_IIP_Stop( D_IM_IIP_STOP );
		printf( "Im_IIP_Stop result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		ercd = sample_set_access_enable( 0 );
		printf( "sample_set_access_enable result = 0x%x.\n", ercd );
		if( ercd != D_IM_IIP_OK ) {
			break;
		}

		break;
	}

	if( ercd != D_IM_IIP_OK ) {
		ercd2 = Im_IIP_Stop( D_IM_IIP_ABORT );
		printf( "Im_IIP_Stop result = 0x%x.\n", ercd2 );

		ercd2 = sample_set_access_enable( 0 );
		printf( "sample_set_access_enable result = 0x%x.\n", ercd2 );
	}

	ercd2 = Im_IIP_Close();
	printf( "Im_IIP_Close result = 0x%x.\n", ercd2 );
	if( ercd == D_IM_IIP_OK ) {
		ercd = ercd2;
	}
}

VOID sample_main_hwtrg( VOID )
{
	T_SAMPLE_IMGINFO				src_img = {
		.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,
		.pix_format = E_IM_IIP_PFMT_YCC422,
		.addr = {
			.Y_G   = 0x20000000,
			.Cb_B  = 0x21000000,
			.Cr_R  = 0x22000000,
			.Alpha = 0x20000000,	// dummy
		},
		.g_width = {
			.Y_G   = 1280,
			.Cb_B  = 1280 /2,
			.Cr_R  = 1280 /2,
			.Alpha = 1280,			// dummy
		},
		.width = 1280,
		.lines = 720,
	};
	T_SAMPLE_IMGINFO				dst_img = {
		.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,
		.pix_format = E_IM_IIP_PFMT_YCC422,
		.addr = {
			.Y_G   = 0x23000000,
			.Cb_B  = 0x24000000,
			.Cr_R  = 0x25000000,
			.Alpha = 0x23000000,	// dummy
		},
		.g_width = {
			.Y_G   = 640,
			.Cb_B  = 640 /2,
			.Cr_R  = 640 /2,
			.Alpha = 640,			// dummy
		},
		.width = 640,
		.lines = 360,
	};

	sample_reduce_half_hwtrg( &src_img, &dst_img );
}

@endcode
<br><br>

@}
*/
/**
@weakgroup im_iip
@{
@section im_iip_util	Sample code of Single frame reducing to 1/2 using Utility.
@code
#include "im_iip.h"
#include "ddim_typedef.h"

#include <stdio.h>


typedef struct {
	E_IM_IIP_FTYPE		frame_type;
	E_IM_IIP_PFMT		pix_format;
	T_IM_IIP_IMG_ADDR	addr;
	T_IM_IIP_LINE_BYTES	g_width;
	USHORT				width;
	USHORT				lines;
} T_SAMPLE_IMGINFO;


#define D_SAMPLE_AFN_UNITID				E_IM_IIP_UNIT_ID_AFN0
#define D_SAMPLE_AFN_RESIZE_MODE		E_IM_IIP_UTIL_RESIZE_MODE_BICUBIC
#define D_SAMPLE_AFN_CSEL				E_IM_IIP_PARAM_CSEL_0

#define D_SAMPLE_SL_UNITID				E_IM_IIP_UNIT_ID_SL2

#define D_SAMPLE_SRC_PIXID_MASK			E_IM_IIP_PIXID_4

#define D_SAMPLE_DST_PIXID_MASK			E_IM_IIP_PIXID_5

#define D_SAMPLE_SRC_MASTER_IF			D_IM_IIP_MASTER_IF0
#define D_SAMPLE_DST_MASTER_IF			D_IM_IIP_MASTER_IF1


VOID sample_reduce_half_util( const T_SAMPLE_IMGINFO* const src_img, const T_SAMPLE_IMGINFO* const dst_img )
{
	T_IM_IIP_UTIL_RR iip_util_rr = {
		.afn_unitid = D_SAMPLE_AFN_UNITID,		// UNITID of AFN
		.sl_unitid = D_SAMPLE_SL_UNITID,		// UNITID of SL
		.src_pixid = D_SAMPLE_SRC_PIXID_MASK,	// Source image PIXID
		.dst_pixid = D_SAMPLE_DST_PIXID_MASK,	// Destination image PIXID
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
//				.width = 1280,							// Width
//				.lines = 720,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
//				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
//				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
//				.width = 1280,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
//				.lines = 720,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
//				.line_bytes = {								// The size of byte of Global image 1 line.
//					.Y_G   = 1280,
//					.Cb_B  = 1280,
//					.Cr_R  = 1280,
//					.Alpha = 1280,			// dummy
//				},
//				.addr = {									// Beginning address of Global image.
//					.Y_G   = 0x20000000,
//					.Cb_B  = 0x21000000,
//					.Cr_R  = 0x22000000,
//					.Alpha = 0x20000000,	// dummy
//				},
				.masterIF_Y_G = D_SAMPLE_SRC_MASTER_IF,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_SAMPLE_SRC_MASTER_IF,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_SAMPLE_SRC_MASTER_IF,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_SAMPLE_SRC_MASTER_IF,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
//				.width = 640,							// Width
//				.lines = 360,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
//				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
//				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
//				.width = 640,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
//				.lines = 360,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
//				.line_bytes = {								// The size of byte of Global image 1 line.
//					.Y_G   = 640,
//					.Cb_B  = 640,
//					.Cr_R  = 640,
//					.Alpha = 640,			// dummy
//				},
//				.addr = {									// Beginning address of Global image.
//					.Y_G   = 0x23000000,
//					.Cb_B  = 0x24000000,
//					.Cr_R  = 0x25000000,
//					.Alpha = 0x23000000,	// dummy
//				},
				.masterIF_Y_G = D_SAMPLE_DST_MASTER_IF,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_SAMPLE_DST_MASTER_IF,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_SAMPLE_DST_MASTER_IF,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_SAMPLE_DST_MASTER_IF,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.afn_cache_select = D_SAMPLE_AFN_CSEL,	// AFN cache select.
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,			// PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
		.rotation = E_IM_IIP_UTIL_ROTATE_000,		// Angle of rotation
		.resize_mode = D_SAMPLE_AFN_RESIZE_MODE,// Resize mode
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[272 + 8];
	ULONG param_buffer_addr = (((ULONG)param_buffer) + 7UL) & 0xFFFFFFF8UL;	// Convet 8bytes align
	INT32 wait_time = 1000;
	INT32 ercd;

	iip_util_rr.src.rect.width = src_img->width;
	iip_util_rr.src.rect.lines = src_img->lines;
	iip_util_rr.src.gbl.frame_type = src_img->frame_type;
	iip_util_rr.src.gbl.pix_format = src_img->pix_format;
	iip_util_rr.src.gbl.width = src_img->width;
	iip_util_rr.src.gbl.lines = src_img->lines;
	iip_util_rr.src.gbl.line_bytes = src_img->g_width;
	iip_util_rr.src.gbl.addr = src_img->addr;

	iip_util_rr.dst.rect.width = dst_img->width;
	iip_util_rr.dst.rect.lines = dst_img->lines;
	iip_util_rr.dst.gbl.frame_type = dst_img->frame_type;
	iip_util_rr.dst.gbl.pix_format = dst_img->pix_format;
	iip_util_rr.dst.gbl.width = dst_img->width;
	iip_util_rr.dst.gbl.lines = dst_img->lines;
	iip_util_rr.dst.gbl.line_bytes = dst_img->g_width;
	iip_util_rr.dst.gbl.addr = dst_img->addr;

	ercd = IM_IIP_Utility_ResizeRotate( &iip_util_rr,
										NULL,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	printf( "IM_IIP_Utility_ResizeRotate result = 0x%x.\n", ercd );
	if( ercd != D_IM_IIP_OK ) {
		return;
	}
}

VOID sample_main_util( VOID )
{
	T_SAMPLE_IMGINFO				src_img = {
		.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,
		.pix_format = E_IM_IIP_PFMT_YCC422,
		.addr = {
			.Y_G   = 0x20000000,
			.Cb_B  = 0x21000000,
			.Cr_R  = 0x22000000,
			.Alpha = 0x20000000,	// dummy
		},
		.g_width = {
			.Y_G   = 1280,
			.Cb_B  = 1280 /2,
			.Cr_R  = 1280 /2,
			.Alpha = 1280,			// dummy
		},
		.width = 1280,
		.lines = 720,
	};
	T_SAMPLE_IMGINFO				dst_img = {
		.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,
		.pix_format = E_IM_IIP_PFMT_YCC422,
		.addr = {
			.Y_G   = 0x23000000,
			.Cb_B  = 0x24000000,
			.Cr_R  = 0x25000000,
			.Alpha = 0x23000000,	// dummy
		},
		.g_width = {
			.Y_G   = 640,
			.Cb_B  = 640 /2,
			.Cr_R  = 640 /2,
			.Alpha = 640,			// dummy
		},
		.width = 640,
		.lines = 360,
	};

	sample_reduce_half_util( &src_img, &dst_img );
}

@endcode
<br><br>

@}
*/


#ifndef _IM_IIP_H_
#define _IM_IIP_H_

#include "driver_common.h"

#include "ddim_typedef.h"

#define CO_IM_IIP_ENABLE_UTILITY	/**< If this definition is valid, Im_IIP_Util_xxx functions are valid. */

#ifdef CO_DEBUG_PRINT_XX_YYY
#define CO_DEBUG_PRINT_IM_IIP	/**< DEBUG */
#endif



/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/**
   @weakgroup im_iip
   @{
*/
/**
   @name Returned value
   @{
*/

#define	D_IM_IIP_OK									(D_DDIM_OK)								/**< Normal end                      (0) */
#define	D_IM_IIP_INVALID_ARG_ERR					(D_IM_IIP | D_DDIM_INPUT_PARAM_ERROR)	/**< Invalid argument error (0x0C000001) */
#define	D_IM_IIP_TIMEOUT_ERR						(D_IM_IIP | D_DDIM_TIMEOUT)				/**< Wait timeout error     (0x0C000013) */
#define	D_IM_IIP_SYSTEMCALL_ERR						(D_IM_IIP | D_DDIM_SYSTEM_ERROR)		/**< System call error      (0x0C000099) */
#define	D_IM_IIP_ERR								(D_IM_IIP | 0x100)						/**< Other error            (0x0C000100) */
#define	D_IM_IIP_ABORT_FAILED_ERR					(D_IM_IIP | 0x101)						/**< Abort failed error     (0x0C000101) */

/** @} */	// name

#define D_IM_IIP_UNIT_LD_MAXNUM						(8)		/**< Loader Unit number */
#define D_IM_IIP_UNIT_AFN_MAXNUM					(4)		/**< AFN Unit number */
#define D_IM_IIP_UNIT_FRECT_MAXNUM					(2)		/**< FRECT Unit number */
#define D_IM_IIP_UNIT_SL_MAXNUM						(9)		/**< SL Unit number */
#define D_IM_IIP_UNIT_CSC_MAXNUM					(4)		/**< CSC Unit number */
#define D_IM_IIP_UNIT_CFL_MAXNUM					(1)		/**< CFC Unit number */
#define D_IM_IIP_UNIT_BLEND_MAXNUM					(2)		/**< BLEND Unit number */
#define D_IM_IIP_CACHE_MAXNUM						(3)		/**< Cache number */
#define D_IM_IIP_PIXID_MAXNUM						(12)	/**< PIXID number */

#define D_IM_IIP_ENABLE_OFF							(0)		/**< The function is disable */
#define D_IM_IIP_ENABLE_ON							(1)		/**< The function is enable */

#define D_IM_IIP_STOP								(0)		/**< Stop IIP */
#define D_IM_IIP_ABORT								(1)		/**< Abort IIP */

// PLDUNIT BitMask
#define D_IM_IIP_PARAM_PLDUNIT_NONE			(0ULL)			/**< None			*/
#define D_IM_IIP_PARAM_PLDUNIT_LD0			(1ULL << 0)		/**< Loader 0		*/
#define D_IM_IIP_PARAM_PLDUNIT_LD1			(1ULL << 1)		/**< Loader 1		*/
#define D_IM_IIP_PARAM_PLDUNIT_LD2			(1ULL << 2)		/**< Loader 2		*/
#define D_IM_IIP_PARAM_PLDUNIT_LD3			(1ULL << 3)		/**< Loader 3		*/
#define D_IM_IIP_PARAM_PLDUNIT_FLT			(1ULL << 4)		/**< FLT			*/
#define D_IM_IIP_PARAM_PLDUNIT_AFN0			(1ULL << 5)		/**< AFN 0			*/
#define D_IM_IIP_PARAM_PLDUNIT_AFN1			(1ULL << 6)		/**< AFN 1			*/
#define D_IM_IIP_PARAM_PLDUNIT_AFN2			(1ULL << 7)		/**< AFN 2			*/
#define D_IM_IIP_PARAM_PLDUNIT_FRECT0		(1ULL << 8)		/**< FRECT 0		*/
#define D_IM_IIP_PARAM_PLDUNIT_MON			(1ULL << 9)		/**< Monitor		*/
#define D_IM_IIP_PARAM_PLDUNIT_SL0			(1ULL << 10)	/**< Store 0		*/
#define D_IM_IIP_PARAM_PLDUNIT_SL1			(1ULL << 11)	/**< Store 1		*/
#define D_IM_IIP_PARAM_PLDUNIT_SL2			(1ULL << 12)	/**< Store 2		*/
#define D_IM_IIP_PARAM_PLDUNIT_SL3			(1ULL << 13)	/**< Store 3		*/
#define D_IM_IIP_PARAM_PLDUNIT_SL4			(1ULL << 14)	/**< Store 4		*/
#define D_IM_IIP_PARAM_PLDUNIT_SL5			(1ULL << 15)	/**< Store 5		*/
#define D_IM_IIP_PARAM_PLDUNIT_CSC0			(1ULL << 16)	/**< CSC 0			*/
#define D_IM_IIP_PARAM_PLDUNIT_CSC1			(1ULL << 17)	/**< CSC 1			*/
#define D_IM_IIP_PARAM_PLDUNIT_LUT			(1ULL << 18)	/**< LUT			*/
#define D_IM_IIP_PARAM_PLDUNIT_GPC			(1ULL << 19)	/**< GPC			*/
#define D_IM_IIP_PARAM_PLDUNIT_BLEND0		(1ULL << 20)	/**< Blend 0		*/
#define D_IM_IIP_PARAM_PLDUNIT_BLEND1		(1ULL << 21)	/**< Blend 1		*/
#define D_IM_IIP_PARAM_PLDUNIT_FRECT1		(1ULL << 22)	/**< FRECT 1		*/
#define D_IM_IIP_PARAM_PLDUNIT_CFL0			(1ULL << 23)	/**< Cache Flow 0	*/
#define D_IM_IIP_PARAM_PLDUNIT_CSC2			(1ULL << 27)	/**< CSC 2			*/
#define D_IM_IIP_PARAM_PLDUNIT_CSC3			(1ULL << 28)	/**< CSC 3			*/
#define D_IM_IIP_PARAM_PLDUNIT_SL6			(1ULL << 29)	/**< Store 6		*/
#define D_IM_IIP_PARAM_PLDUNIT_SL7			(1ULL << 30)	/**< Store 7		*/
#define D_IM_IIP_PARAM_PLDUNIT_SL8			(1ULL << 31)	/**< Store 8		*/
#define D_IM_IIP_PARAM_PLDUNIT_LD4			(1ULL << 32)	/**< Loader 4		*/
#define D_IM_IIP_PARAM_PLDUNIT_LD5			(1ULL << 33)	/**< Loader 5		*/
#define D_IM_IIP_PARAM_PLDUNIT_LD6			(1ULL << 34)	/**< Loader 6		*/
#define D_IM_IIP_PARAM_PLDUNIT_LD7			(1ULL << 35)	/**< Loader 7		*/
#define D_IM_IIP_PARAM_PLDUNIT_AFN3			(1ULL << 36)	/**< AFN 3			*/
#define D_IM_IIP_PARAM_PLDUNIT_MFT			(1ULL << 37)	/**< MFT			*/

// UNITINFTBL.SWTRG
#define D_IM_IIP_SWTRG_OFF							(0)		/**< Don't care unit	*/
#define D_IM_IIP_SWTRG_ON							(1)		/**< Start unit */

// UNITINFTBL.HWEN
#define	D_IM_IIP_HW_CTRL_SWTRG						(0)		/**< SWTRG control	 */
#define	D_IM_IIP_HW_CTRL_AUTO						(1)		/**< HW auto control */

// UNITINFTBL.CHAIN
#define	D_IM_IIP_PLDUNIT_CHAIN_DISABLE				(0)		/**< PLDUNIT do not chain operation */
#define	D_IM_IIP_PLDUNIT_CHAIN_ENABLE				(1)		/**< PLDUNIT do chain operation */

// PIXFMTTBL.PDEPTH
#define	D_IM_IIP_PDEPTH_8BITS						(0)		/**< 8bits			*/
#define	D_IM_IIP_PDEPTH_12BITS						(1)		/**< 12bits-packed	*/
#define	D_IM_IIP_PDEPTH_16BITS						(2)		/**< 16bits(14bits)	*/

// PIXFMTTBL.CHKYORDR
#define	D_IM_IIP_CHUNKY_COLOR_Y0_G					(0)		/**< YO(G)	   */
#define	D_IM_IIP_CHUNKY_COLOR_CB_B					(1)		/**< Cb(B)	   */
#define	D_IM_IIP_CHUNKY_COLOR_CR_R					(2)		/**< Cr(R)	   */
#define	D_IM_IIP_CHUNKY_COLOR_Y1_A					(3)		/**< Y1(Alpha) */

// PIXFMTTBL.SGYG, SGB, SGR, SGA
#define	D_IM_IIP_UNSIGNED_DATA						(0)		/**< Unsigned data */
#define	D_IM_IIP_SIGNED_DATA						(1)		/**< Signed data   */

// PIXFMTTBL.ALPHA
#define	D_IM_IIP_ALPHA_FALSE						(0)		/**< Image without alpha */
#define	D_IM_IIP_ALPHA_TRUE							(1)		/**< Image with alpha	 */

// PIXFMTTBL.ASUBSPL
#define	D_IM_IIP_ALPHA_NO_SUBSAMP					(0)		/**< No subsampling							*/
#define	D_IM_IIP_ALPHA_H_SUBSAMP					(1)		/**< Horizontal subsampling					*/
#define	D_IM_IIP_ALPHA_HV_SUBSAMP					(2)		/**< Horizontal and Vertical subsampling	*/

// PIXFMTTBL.MTIFYG, MTIFB, MTIFR, MTIFA (for SL2/SL5 only)
#define	D_IM_IIP_MASTER_IF0							(1)		/**< Master I/F 0 */
#define	D_IM_IIP_MASTER_IF1							(0)		/**< Master I/F 1 */
#define	D_IM_IIP_MASTER_IF2							(0)		/**< Master I/F 2 */

// PIXFMTTBL.OARBHV
#define	D_IM_IIP_OUTSIDE_IMG_FIXED_VALUE			(0)		/**< Fixed value */
#define	D_IM_IIP_OUTSIDE_IMG_EDGE					(1)		/**< Image edge value (value of pixels on top, bottom, left and right lines) */

#if 0	// B2Y Link not support
// IZACTL1.LKEN
#define	D_IM_IIP_LINK_B2Y_OFF						(0)		/**< Disable linkage between B2Y to IIP.	*/
#define	D_IM_IIP_LINK_B2Y_ON						(1)		/**< Enable linkage between B2Y to IIP.	*/
#endif	// B2Y Link not support

#define	D_IM_IIP_INT_FACTOR_IZAEND					(1<<0)	/**< IZAFLG interrupt factor ID (0x0000001) */
#define	D_IM_IIP_INT_FACTOR_AXIERR					(1<<1)	/**< AXEFLG interrupt factor ID (0x0000002) */
#define	D_IM_IIP_INT_FACTOR_SL0END					(1<<2)	/**< SL0FF interrupt factor ID  (0x0000004) */
#define	D_IM_IIP_INT_FACTOR_SL1END					(1<<3)	/**< SL1FF interrupt factor ID  (0x0000008) */
#define	D_IM_IIP_INT_FACTOR_SL2END					(1<<4)	/**< SL2FF interrupt factor ID  (0x0000010) */
#define	D_IM_IIP_INT_FACTOR_SL3END					(1<<5)	/**< SL3FF interrupt factor ID  (0x0000020) */
#define	D_IM_IIP_INT_FACTOR_SL4END					(1<<6)	/**< SL4FF interrupt factor ID  (0x0000040) */
#define	D_IM_IIP_INT_FACTOR_SL5END					(1<<7)	/**< SL5FF interrupt factor ID  (0x0000080) */
#define	D_IM_IIP_INT_FACTOR_SL6END					(1<<8)	/**< SL6FF interrupt factor ID  (0x0000100) */
#define	D_IM_IIP_INT_FACTOR_SL7END					(1<<9)	/**< SL7FF interrupt factor ID  (0x0000200) */
#define	D_IM_IIP_INT_FACTOR_SL8END					(1<<10)	/**< SL8FF interrupt factor ID  (0x0000400) */
#define	D_IM_IIP_INT_FACTOR_SL0_LINEEND				(1<<11)	/**< SL0MF interrupt factor ID  (0x0000800) */
#define	D_IM_IIP_INT_FACTOR_SL1_LINEEND				(1<<12)	/**< SL1MF interrupt factor ID  (0x0001000) */
#define	D_IM_IIP_INT_FACTOR_SL2_LINEEND				(1<<13)	/**< SL2MF interrupt factor ID  (0x0002000) */
#define	D_IM_IIP_INT_FACTOR_SL3_LINEEND				(1<<14)	/**< SL3MF interrupt factor ID  (0x0004000) */
#define	D_IM_IIP_INT_FACTOR_SL4_LINEEND				(1<<15)	/**< SL4MF interrupt factor ID  (0x0008000) */
#define	D_IM_IIP_INT_FACTOR_SL5_LINEEND				(1<<16)	/**< SL5MF interrupt factor ID  (0x0010000) */
#define	D_IM_IIP_INT_FACTOR_SL6_LINEEND				(1<<17)	/**< SL6MF interrupt factor ID  (0x0020000) */
#define	D_IM_IIP_INT_FACTOR_SL7_LINEEND				(1<<18)	/**< SL7MF interrupt factor ID  (0x0040000) */
#define	D_IM_IIP_INT_FACTOR_SL8_LINEEND				(1<<19)	/**< SL8MF interrupt factor ID  (0x0080000) */

// PDCCTL.FLHALL
#define	D_IM_IIP_NON_FLUSH_IMGCACHE_START			(0)		/**< Non flush cache data when IIP begins to run.	*/
#define	D_IM_IIP_FLUSH_IMGCACHE_START				(1)		/**< Flush cache data when IIP begins to run.		*/

// PDCCTL.EXBIT
#define	D_IM_IIP_EXTBIT_LOWER_ALIGNED				(0)		/**< Store data on lower bit. */
#define	D_IM_IIP_EXTBIT_UPPER_ALIGNED				(1)		/**< Store data on upper bit. */

// SLAXICTl.SL0PKG, SL1PKG, SL2PKG, SL3PKG, SL4PKG, SL5PKG, SL6PKG, SL7PKG, SL8PKG
#define	D_IM_IIP_NON_BURST							(0)		/**< Non burst transfer. */
#define	D_IM_IIP_BURST_MAX_2LINES					(1)		/**< Up to 2lines burst transfer. */
#define	D_IM_IIP_BURST_MAX_4LINES					(2)		/**< Up to 4lines burst transfer. */
#define	D_IM_IIP_BURST_OVER_4LINES					(3)		/**< Over 4lines burst transfer. */

#define D_IM_IIP_AND_WAIT							(0)		/**< AND WAIT */
#define D_IM_IIP_OR_WAIT							(1)		/**< OR WAIT  */

#define D_IM_IIP_HIST_MAXCOUNTS						(256)	/**< Array counts of Histgram buffer */
#define D_IM_IIP_HIST_MAXBYTES						(D_IM_IIP_HIST_MAXCOUNTS * sizeof(ULONG))	/**< Byte number of Histgram buffer */

#define D_IM_IIP_LUT_MAXNUM							(1024)	/**< Array number of LUTRAM */


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/** Cache channel No. */
typedef enum {
	E_IM_IIP_CACHE_CH_0 = 0,			/**< Cache channel 0 */
	E_IM_IIP_CACHE_CH_1,				/**< Cache channel 1 */
	E_IM_IIP_CACHE_CH_2,				/**< Cache channel 2 */
	E_IM_IIP_CACHE_CH_MAX				/**< Cache channel Maximum number */
} E_IM_IIP_CACHE_CH;

/** PIXFMTTBL FTYPE[2:0] bits */
typedef enum {
	E_IM_IIP_FTYPE_Y_PACKED_CBCR = 0,	/**< Planar format of Y and Chunky format of CbCr */
	E_IM_IIP_FTYPE_CHUNKY = 1,			/**< Chunky format of RGB/YCbCr or<br> Chunky format of CbCr without Y */
	E_IM_IIP_FTYPE_PLANE = 2			/**< Planar format of YCbCr/GBR or<br> Planar format of CbCr without Y */
} E_IM_IIP_FTYPE;

/** PIXFMTTBL PFMT[3:0] bits */
typedef enum {
	E_IM_IIP_PFMT_YCC422	  = 0,		/**< YCbCr422				*/
	E_IM_IIP_PFMT_YCC444	  = 1,		/**< YCbCr444(RGB)			*/
	E_IM_IIP_PFMT_YCC420	  = 2,		/**< YCbCr420				*/
	E_IM_IIP_PFMT_BAYER		  = 3,		/**< Bayer					*/
	E_IM_IIP_PFMT_CBCR		  = 4,		/**< CbCr without Y			*/
	E_IM_IIP_PFMT_VIDEO		  = 5,		/**< Video format (JMILH264_6 uses it) */
	E_IM_IIP_PFMT_PLANE		  = 6,		/**< Plane					*/
	E_IM_IIP_PFMT_RGB565	  = 7,		/**< RGB565					*/
	E_IM_IIP_PFMT_RGBA5551	  = 8,		/**< RGBA5551				*/
	E_IM_IIP_PFMT_RGBA4444	  = 9		/**< RGBA4444				*/
} E_IM_IIP_PFMT;

/** PIXFMTTBL ADEPTH[2:0] bits */
typedef enum {
	E_IM_IIP_ADEPTH_8BITS  = 0,			/**< 8bit			*/
	E_IM_IIP_ADEPTH_4BITS  = 3			/**< 4bit			*/
} E_IM_IIP_ADEPTH;

/** UNIT ID (same as PLDUNIT BitNo) */
typedef enum {
	E_IM_IIP_UNIT_ID_TOP = 0,			/**< Minimum value of enum	*/
	E_IM_IIP_UNIT_ID_LD0 = 0,			/**< Loader 0				*/
	E_IM_IIP_UNIT_ID_LD1 = 1,			/**< Loader 1				*/
	E_IM_IIP_UNIT_ID_LD2 = 2,			/**< Loader 2				*/
	E_IM_IIP_UNIT_ID_LD3 = 3,			/**< Loader 3				*/
	E_IM_IIP_UNIT_ID_FLT = 4,			/**< FLT					*/
	E_IM_IIP_UNIT_ID_AFN0 = 5,			/**< Affine 0				*/
	E_IM_IIP_UNIT_ID_AFN1 = 6,			/**< Affine 1				*/
	E_IM_IIP_UNIT_ID_AFN2 = 7,			/**< Affine 2				*/
	E_IM_IIP_UNIT_ID_FRECT0 = 8,		/**< FRECT 0				*/
	E_IM_IIP_UNIT_ID_MON = 9,			/**< Monitor				*/
	E_IM_IIP_UNIT_ID_SL0 = 10,			/**< Store 0				*/
	E_IM_IIP_UNIT_ID_SL1 = 11,			/**< Store 1				*/
	E_IM_IIP_UNIT_ID_SL2 = 12,			/**< Store 2				*/
	E_IM_IIP_UNIT_ID_SL3 = 13,			/**< Store 3				*/
	E_IM_IIP_UNIT_ID_SL4 = 14,			/**< Store 4				*/
	E_IM_IIP_UNIT_ID_SL5 = 15,			/**< Store 5				*/
	E_IM_IIP_UNIT_ID_CSC0 = 16,			/**< CSC 0					*/
	E_IM_IIP_UNIT_ID_CSC1 = 17,			/**< CSC 1					*/
	E_IM_IIP_UNIT_ID_LUT = 18,			/**< LUT					*/
	E_IM_IIP_UNIT_ID_GPC = 19,			/**< GPC					*/
	E_IM_IIP_UNIT_ID_BLEND0 = 20,		/**< Blend 0				*/
	E_IM_IIP_UNIT_ID_BLEND1 = 21,		/**< Blend 1				*/
	E_IM_IIP_UNIT_ID_FRECT1 = 22,		/**< FRECT 1				*/
	E_IM_IIP_UNIT_ID_CFL0 = 23,			/**< Cache Flow 0			*/
	E_IM_IIP_UNIT_ID_CSC2 = 27,			/**< CSC 2					*/
	E_IM_IIP_UNIT_ID_CSC3 = 28,			/**< CSC 3					*/
	E_IM_IIP_UNIT_ID_SL6 = 29,			/**< Store 6				*/
	E_IM_IIP_UNIT_ID_SL7 = 30,			/**< Store 7				*/
	E_IM_IIP_UNIT_ID_SL8 = 31,			/**< Store 8				*/
	E_IM_IIP_UNIT_ID_LD4 = 32,			/**< Loader 4				*/
	E_IM_IIP_UNIT_ID_LD5 = 33,			/**< Loader 5				*/
	E_IM_IIP_UNIT_ID_LD6 = 34,			/**< Loader 6				*/
	E_IM_IIP_UNIT_ID_LD7 = 35,			/**< Loader 7				*/
	E_IM_IIP_UNIT_ID_AFN3 = 36,			/**< Affine 3				*/
	E_IM_IIP_UNIT_ID_MFT = 37,			/**< MFT					*/
	E_IM_IIP_UNIT_ID_MAX = 38,			/**< Maximum value of enum	*/
} E_IM_IIP_UNIT_ID;

/** PIXFMTTBL ID BitMask */
typedef enum {
	E_IM_IIP_PIXID_0 = 1 << 0,				/**< PIXFMTTBL No.0			*/
	E_IM_IIP_PIXID_1 = 1 << 1,				/**< PIXFMTTBL No.1			*/
	E_IM_IIP_PIXID_2 = 1 << 2,				/**< PIXFMTTBL No.2			*/
	E_IM_IIP_PIXID_3 = 1 << 3,				/**< PIXFMTTBL No.3			*/
	E_IM_IIP_PIXID_4 = 1 << 4,				/**< PIXFMTTBL No.4			*/
	E_IM_IIP_PIXID_5 = 1 << 5,				/**< PIXFMTTBL No.5			*/
	E_IM_IIP_PIXID_6 = 1 << 6,				/**< PIXFMTTBL No.6			*/
	E_IM_IIP_PIXID_7 = 1 << 7,				/**< PIXFMTTBL No.7			*/
	E_IM_IIP_PIXID_8 = 1 << 8,				/**< PIXFMTTBL No.8			*/
	E_IM_IIP_PIXID_9 = 1 << 9,				/**< PIXFMTTBL No.9			*/
	E_IM_IIP_PIXID_10 = 1 << 10,			/**< PIXFMTTBL No.10		*/
	E_IM_IIP_PIXID_11 = 1 << 11,			/**< PIXFMTTBL No.11		*/
	E_IM_IIP_PIXID_INVALID = 1 << 16,		/**< Invalid value. (Driver internal use only)		*/
} E_IM_IIP_PIXID;

/** AXI ID */
typedef enum {
	E_IM_IIP_AXI_ID_TOP = 0,								/**< Top value of this enum. */
	E_IM_IIP_AXI_ID_CACHE0 = 0,								/**< Read cache 0 */
	E_IM_IIP_AXI_ID_CACHE1,									/**< Read cache 1 */
	E_IM_IIP_AXI_ID_CACHE2,									/**< Read cache 2 */
	E_IM_IIP_AXI_ID_DP,										/**< Dispatch sequencer */
	E_IM_IIP_AXI_ID_AFN0,									/**< Affine 0 */
	E_IM_IIP_AXI_ID_AFN1,									/**< Affine 1 */
	E_IM_IIP_AXI_ID_AFN2,									/**< Affine 2 */
	E_IM_IIP_AXI_ID_AFN3,									/**< Affine 3 */
	E_IM_IIP_AXI_ID_FRECT0,									/**< FRECT 0 */
	E_IM_IIP_AXI_ID_FRECT1,									/**< FRECT 1 */
	E_IM_IIP_AXI_ID_GPC,									/**< GPC */
	E_IM_IIP_AXI_ID_SL0,									/**< Store 0 */
	E_IM_IIP_AXI_ID_SL1,									/**< Store 1 */
	E_IM_IIP_AXI_ID_SL2,									/**< Store 2 */
	E_IM_IIP_AXI_ID_SL3,									/**< Store 3 */
	E_IM_IIP_AXI_ID_SL4,									/**< Store 4 */
	E_IM_IIP_AXI_ID_SL5,									/**< Store 5 */
	E_IM_IIP_AXI_ID_SL6,									/**< Store 6 */
	E_IM_IIP_AXI_ID_SL7,									/**< Store 7 */
	E_IM_IIP_AXI_ID_SL8,									/**< Store 8 */
	E_IM_IIP_AXI_ID_MAX										/**< Maximum value of this enum. */
} E_IM_IIP_AXI_ID;

/** LUTRAM type */
typedef enum {
	E_IM_IIP_LUTRAM_TYPE_A = 0,			/**< LUTRAM A */
	E_IM_IIP_LUTRAM_TYPE_B,				/**< LUTRAM B */
	E_IM_IIP_LUTRAM_TYPE_C,				/**< LUTRAM C */
	E_IM_IIP_LUTRAM_TYPE_D,				/**< LUTRAM D */
	E_IM_IIP_LUTRAM_TYPE_E,				/**< LUTRAM E */
	E_IM_IIP_LUTRAM_TYPE_F,				/**< LUTRAM F */
	E_IM_IIP_LUTRAM_TYPE_MAX,			/**< Maximum value of this enum. */
} E_IM_IIP_LUTRAM_TYPE;

/** Resource BitMask */
typedef enum {
	E_IM_IIP_OPEN_RES_NONE     = 0,					/**< None */
	E_IM_IIP_OPEN_RES_LUT_A    = (1 << 0),			/**< LUT A */
	E_IM_IIP_OPEN_RES_LUT_B    = (1 << 1),			/**< LUT B */
	E_IM_IIP_OPEN_RES_LUT_C    = (1 << 2),			/**< LUT C */
	E_IM_IIP_OPEN_RES_LUT_D    = (1 << 3),			/**< LUT D */
	E_IM_IIP_OPEN_RES_LUT_E    = (1 << 4),			/**< LUT E */
	E_IM_IIP_OPEN_RES_LUT_F    = (1 << 5),			/**< LUT F */
	E_IM_IIP_OPEN_RES_HIST_0   = (1 << 6),			/**< Histogram 0 */
	E_IM_IIP_OPEN_RES_HIST_1   = (1 << 7),			/**< Histogram 1 */
	E_IM_IIP_OPEN_RES_HIST_2   = (1 << 8),			/**< Histogram 2 */
	E_IM_IIP_OPEN_RES_HIST_3   = (1 << 9),			/**< Histogram 3 */
	E_IM_IIP_OPEN_RES_CACHE0   = (1 << 10),			/**< Image Cache 0 */
	E_IM_IIP_OPEN_RES_CACHE1   = (1 << 11),			/**< Image Cache 1 */
	E_IM_IIP_OPEN_RES_CACHE2   = (1 << 12),			/**< Image Cache 2 */
	E_IM_IIP_OPEN_RES_BICUBIC0 = (1 << 13),			/**< Bi-Cubic calculator 0 */
	E_IM_IIP_OPEN_RES_BICUBIC1 = (1 << 14),			/**< Bi-Cubic calculator 1 */
} E_IM_IIP_OPEN_RES;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

typedef VOID (*T_IM_IIP_CALLBACK)( ULONG interrupt_flag_mask );	/**< Callback function pointer	*/

/** Chunky component structure */
typedef struct {
	UCHAR	component0;		/**< PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
								 <ul>
									 <li>@ref D_IM_IIP_CHUNKY_COLOR_Y0_G	[PIXFMTTBL.CHKYORDR[1:0]=00b]
									 <li>@ref D_IM_IIP_CHUNKY_COLOR_CB_B	[PIXFMTTBL.CHKYORDR[1:0]=01b]
									 <li>@ref D_IM_IIP_CHUNKY_COLOR_CR_R	[PIXFMTTBL.CHKYORDR[1:0]=10b]
									 <li>@ref D_IM_IIP_CHUNKY_COLOR_Y1_A	[PIXFMTTBL.CHKYORDR[1:0]=11b]
								 </ul> */
	UCHAR	component1;		/**< PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1). Setting value is same as component0. */
	UCHAR	component2;		/**< PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2). Setting value is same as component0. */
	UCHAR	component3;		/**< PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3). Setting value is same as component0. */
} T_IM_IIP_CHUNKY;

/** The size of byte of Global image 1 line. */
typedef struct {
	LONG	Y_G;			/**< PIXFMTTBL.GHSZYG[17:0] <br><br>
								 The size of byte of Global Y(G) image 1 line.
								 Should be aligned on 8.
							 */
	LONG	Cb_B;			/**< PIXFMTTBL.GHSZB[17:0] <br><br>
								 The size of byte of Global Cb(B) image 1 line.
								 Should be aligned on 8.
							 */
	LONG	Cr_R;			/**< PIXFMTTBL.GHSZR[17:0] <br><br>
								 The size of byte of Global Cr(R) image 1 line.
								 Should be aligned on 8.
							 */
	LONG	Alpha;			/**< PIXFMTTBL.GHSZA[17:0] <br><br>
								 The size of byte of Global Alpha image 1 line.
								 Should be aligned on 8.
							 */
} T_IM_IIP_LINE_BYTES;

/** The beginning address of Global image. */
typedef struct {
	ULONG	Y_G;			/**< PIXFMTTBL.ADRSYG[31:0] <br><br>
								 Beginning address of Global Y(G) image.
								 Should be aligned on 8.
							 */
	ULONG	Cb_B;			/**< PIXFMTTBL.ADRSB[31:0] <br><br>
								 Beginning address of Global Cb(B) image.
								 Should be aligned on 8.
							 */
	ULONG	Cr_R;			/**< PIXFMTTBL.ADRSR[31:0] <br><br>
								 Beginning address of Global Cr(R) image.
								 Should be aligned on 8.
							 */
	ULONG	Alpha;			/**< PIXFMTTBL.ADRSA[31:0] <br><br>
								 Beginning address of Global Alpha image.
								 Should be aligned on 8.
							 */
} T_IM_IIP_IMG_ADDR;

/** Im_IIP_Ctrl_PIXFMTTBL() parameter structure */
typedef struct {
	UINT32				pix_depth;				/**< PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
													 <ul>
														 <li>@ref D_IM_IIP_PDEPTH_8BITS	 [PIXFMTTBL.PDEPTH[2:0] = 000b]
														 <li>@ref D_IM_IIP_PDEPTH_12BITS [PIXFMTTBL.PDEPTH[2:0] = 001b]
														 <li>@ref D_IM_IIP_PDEPTH_16BITS [PIXFMTTBL.PDEPTH[2:0] = 010b]
													 </ul> */
	UINT32				alpha;					/**< PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
													 <ul>
														 <li>@ref D_IM_IIP_ALPHA_FALSE	[PIXFMTTBL.ALPHA = 0b]
														 <li>@ref D_IM_IIP_ALPHA_TRUE	[PIXFMTTBL.ALPHA = 1b]
													 </ul> */
	E_IM_IIP_FTYPE		frame_type;				/**< PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
													 <ul>
														 <li>@ref E_IM_IIP_FTYPE_Y_PACKED_CBCR	[PIXFMTTBL.FTYPE[2:0] = 000b]
														 <li>@ref E_IM_IIP_FTYPE_CHUNKY			[PIXFMTTBL.FTYPE[2:0] = 001b]
														 <li>@ref E_IM_IIP_FTYPE_PLANE			[PIXFMTTBL.FTYPE[2:0] = 010b]
													 </ul> */
	E_IM_IIP_PFMT		pix_format;				/**< PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
													 <ul>
														 <li>@ref E_IM_IIP_PFMT_YCC422		[PIXFMTTBL.PFMT[3:0] = 0000b]
														 <li>@ref E_IM_IIP_PFMT_YCC444		[PIXFMTTBL.PFMT[3:0] = 0001b]
														 <li>@ref E_IM_IIP_PFMT_YCC420		[PIXFMTTBL.PFMT[3:0] = 0010b]
														 <li>@ref E_IM_IIP_PFMT_BAYER		[PIXFMTTBL.PFMT[3:0] = 0011b]
														 <li>@ref E_IM_IIP_PFMT_CBCR		[PIXFMTTBL.PFMT[3:0] = 0100b]
														 <li>@ref E_IM_IIP_PFMT_VIDEO		[PIXFMTTBL.PFMT[3:0] = 0101b]
														 <li>@ref E_IM_IIP_PFMT_PLANE		[PIXFMTTBL.PFMT[3:0] = 0110b]
														 <li>@ref E_IM_IIP_PFMT_RGB565		[PIXFMTTBL.PFMT[3:0] = 0111b]
														 <li>@ref E_IM_IIP_PFMT_RGBA5551	[PIXFMTTBL.PFMT[3:0] = 1000b]
														 <li>@ref E_IM_IIP_PFMT_RGBA4444	[PIXFMTTBL.PFMT[3:0] = 1001b]
													 </ul> */
	E_IM_IIP_ADEPTH		alpha_depth;			/**< PIXFMTTBL.ADEPTH[2:0] <br><br> Depth of alpha.
													 <ul>
														 <li>@ref E_IM_IIP_ADEPTH_8BITS		[PIXFMTTBL.ADEPTH[2:0] = 000b]
														 <li>@ref E_IM_IIP_ADEPTH_4BITS		[PIXFMTTBL.ADEPTH[2:0] = 011b]
													 </ul> */
	T_IM_IIP_CHUNKY		chunky_color;			/**< PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component. */

	UCHAR				sign_Y_G;				/**< PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
													 <ul>
														 <li>@ref D_IM_IIP_UNSIGNED_DATA	[PIXFMTTBL.SGYG = 0b]
														 <li>@ref D_IM_IIP_SIGNED_DATA		[PIXFMTTBL.SGYG = 1b]
													 </ul> */
	UCHAR				sign_Cb_B;				/**< PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
													 <ul>
														 <li>@ref D_IM_IIP_UNSIGNED_DATA	[PIXFMTTBL.SGB = 0b]
														 <li>@ref D_IM_IIP_SIGNED_DATA		[PIXFMTTBL.SGB = 1b]
													 </ul> */
	UCHAR				sign_Cr_R;				/**< PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
													 <ul>
														 <li>@ref D_IM_IIP_UNSIGNED_DATA	[PIXFMTTBL.SGR = 0b]
														 <li>@ref D_IM_IIP_SIGNED_DATA		[PIXFMTTBL.SGR = 1b]
													 </ul> */
	UCHAR				sign_D3;				/**< PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
													 <ul>
														 <li>@ref D_IM_IIP_UNSIGNED_DATA	[PIXFMTTBL.SGA = 0b]
														 <li>@ref D_IM_IIP_SIGNED_DATA		[PIXFMTTBL.SGA = 1b]
													 </ul> */

	UCHAR				masterIF_Y_G;			/**< PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) output image is master I/F 0, 1 or 2.
													 <TABLE>
														<TR><TH>Store Unit No.</TH><TH>Valid parameter</TH></TR>
														<TR><TH>SL0</TH><TH>D.C.<BR>Fixed value D_IM_IIP_MASTER_IF0 by hardware.</TH></TR>
														<TR><TH>SL1</TH><TH>D.C.<BR>Fixed value D_IM_IIP_MASTER_IF1 by hardware.</TH></TR>
														<TR><TH>SL2</TH><TH>D_IM_IIP_MASTER_IF0 = not supported<BR>D_IM_IIP_MASTER_IF1 = 1<BR>D_IM_IIP_MASTER_IF2 = 2</TH></TR>
														<TR><TH>SL3</TH><TH>D.C.<BR>Fixed value D_IM_IIP_MASTER_IF0 by hardware.</TH></TR>
														<TR><TH>SL4</TH><TH>D.C.<BR>Fixed value D_IM_IIP_MASTER_IF1 by hardware.</TH></TR>
														<TR><TH>SL5</TH><TH>D_IM_IIP_MASTER_IF0 = 0<BR>D_IM_IIP_MASTER_IF1 = not supported<BR>D_IM_IIP_MASTER_IF2 = 2</TH></TR>
														<TR><TH>SL6</TH><TH>D.C.<BR>Fixed value D_IM_IIP_MASTER_IF0 by hardware.</TH></TR>
														<TR><TH>SL7</TH><TH>D.C.<BR>Fixed value D_IM_IIP_MASTER_IF1 by hardware.</TH></TR>
														<TR><TH>SL8</TH><TH>D.C.<BR>Fixed value D_IM_IIP_MASTER_IF2 by hardware.</TH></TR>
													 </TABLE>
													 <ul>
														 <li>@ref D_IM_IIP_MASTER_IF0	[PIXFMTTBL.MTIFYG = 1b]
														 <li>@ref D_IM_IIP_MASTER_IF1	[PIXFMTTBL.MTIFYG = 0b]
														 <li>@ref D_IM_IIP_MASTER_IF2	[PIXFMTTBL.MTIFYG = 0b]
													 </ul> */
	UCHAR				masterIF_Cb_B;			/**< PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) output image is master I/F 0, 1 or 2.
													 Please refrer @ref masterIF_Y_G.
													 */
	UCHAR				masterIF_Cr_R;			/**< PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) output image is master I/F 0, 1 or 2.
													 Please refrer @ref masterIF_Y_G.
													 */
	UCHAR				masterIF_Alpha;			/**< PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha output image is master I/F 0, 1 or 2
													 Please refrer @ref masterIF_Y_G.
													 */
	UINT32				alpha_subsampling;		/**< PIXFMTTBL.ASUBSPL[1:0] <br><br> Control Alpha subsampling.
													 <ul>
														 <li>@ref D_IM_IIP_ALPHA_NO_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 00b]
														 <li>@ref D_IM_IIP_ALPHA_H_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 01b]
														 <li>@ref D_IM_IIP_ALPHA_HV_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 10b]
													 </ul> */
	UINT32				pix_outside_img;		/**< PIXFMTTBL.OARBHV <br><br> Control pixel value outside image.
													 <ul>
														 <li>@ref D_IM_IIP_OUTSIDE_IMG_FIXED_VALUE	[PIXFMTTBL.OARBHV = 0b]
														 <li>@ref D_IM_IIP_OUTSIDE_IMG_EDGE			[PIXFMTTBL.OARBHV = 1b]
													 </ul> */
	USHORT				Y_G_val_outside_img;	/**< PIXFMTTBL.OARVALYG[15:0] <br><br> pixel value outside Y(G) image.
												   This parameter is valid
												   only when T_IM_IIP_PIXFMTTBL::pix_outside_img is set to D_IM_IIP_OUTSIDE_IMG_FIXED_VALUE.*/
	USHORT				Cb_B_val_outside_img;	/**< PIXFMTTBL.OARVALB[15:0] <br><br> pixel value outside Cb(B) image.
												   This parameter is valid
												   only when T_IM_IIP_PIXFMTTBL::pix_outside_img is set to D_IM_IIP_OUTSIDE_IMG_FIXED_VALUE.*/
	USHORT				Cr_R_val_outside_img;	/**< PIXFMTTBL.OARVALR[15:0] <br><br> pixel value outside Cr(R) image.
												   This parameter is valid
												   only when T_IM_IIP_PIXFMTTBL::pix_outside_img is set to D_IM_IIP_OUTSIDE_IMG_FIXED_VALUE.*/
	USHORT				Alpha_val_outside_img;	/**< PIXFMTTBL.OARVALA[15:0] <br><br> pixel value outside Alpha image.
												   This parameter is valid
												   only when T_IM_IIP_PIXFMTTBL::pix_outside_img is set to D_IM_IIP_OUTSIDE_IMG_FIXED_VALUE.*/
	USHORT				width;					/**< PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383] */
	USHORT				lines;					/**< PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383] */

	T_IM_IIP_LINE_BYTES	line_bytes;				/**< The size of byte of Global image 1 line. */
	T_IM_IIP_IMG_ADDR	addr;					/**< Beginning address of Global image. */
} T_IM_IIP_PIXFMTTBL;

/** UNITINFTBL structure */
typedef struct {
	UCHAR	unit_ctrl;			/**< UNITINFTBL.HWEN <br><br> Control unit.
													 <ul>
														 <li>@ref D_IM_IIP_HW_CTRL_SWTRG	[UNITINFTBL.HWEN = 0b]
														 <li>@ref D_IM_IIP_HW_CTRL_AUTO		[UNITINFTBL.HWEN = 1b]
													 </ul> */
	UCHAR	chain_enable;		/**< UNITINFTBL.CHAIN <br><br> Chain control.
													 <ul>
														 <li>@ref D_IM_IIP_PLDUNIT_CHAIN_DISABLE	[UNITINFTBL.CHAIN = 0b]
														 <li>@ref D_IM_IIP_PLDUNIT_CHAIN_ENABLE		[UNITINFTBL.CHAIN = 1b]
													 </ul> */
	ULONG	unit_param_addr;	/**< UNITINFTBL.PADRS[31:0] <br><br> Beginning address of unit parameter. Should be aligned on 8. */
	ULLONG	load_unit_param_flag;	/**< UNITINFTBL.PLDUNIT[63:0] <br><br> Control downloading unit parameter. for SL unit only.
										 <ul>
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_LD0		[UNITINFTBL.PLDUNIT[0] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_LD1		[UNITINFTBL.PLDUNIT[1] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_LD2		[UNITINFTBL.PLDUNIT[2] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_LD3		[UNITINFTBL.PLDUNIT[3] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_FLT		[UNITINFTBL.PLDUNIT[4] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_AFN0	[UNITINFTBL.PLDUNIT[5] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_AFN1	[UNITINFTBL.PLDUNIT[6] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_AFN2	[UNITINFTBL.PLDUNIT[7] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_FRECT0	[UNITINFTBL.PLDUNIT[8] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_MON		[UNITINFTBL.PLDUNIT[9] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_SL0		[UNITINFTBL.PLDUNIT[10] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_SL1		[UNITINFTBL.PLDUNIT[11] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_SL2		[UNITINFTBL.PLDUNIT[12] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_SL3		[UNITINFTBL.PLDUNIT[13] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_SL4		[UNITINFTBL.PLDUNIT[14] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_SL5		[UNITINFTBL.PLDUNIT[15] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_CSC0	[UNITINFTBL.PLDUNIT[16] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_CSC1	[UNITINFTBL.PLDUNIT[17] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_LUT		[UNITINFTBL.PLDUNIT[18] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_GPC		[UNITINFTBL.PLDUNIT[19] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_BLEND0	[UNITINFTBL.PLDUNIT[20] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_BLEND1	[UNITINFTBL.PLDUNIT[21] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_FRECT1	[UNITINFTBL.PLDUNIT[22] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_CFL0	[UNITINFTBL.PLDUNIT[23] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_CSC2	[UNITINFTBL.PLDUNIT[27] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_CSC3	[UNITINFTBL.PLDUNIT[28] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_SL6		[UNITINFTBL.PLDUNIT[29] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_SL7		[UNITINFTBL.PLDUNIT[30] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_SL8		[UNITINFTBL.PLDUNIT[31] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_LD4		[UNITINFTBL.PLDUNIT[32] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_LD5		[UNITINFTBL.PLDUNIT[33] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_LD6		[UNITINFTBL.PLDUNIT[34] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_LD7		[UNITINFTBL.PLDUNIT[35] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_AFN3	[UNITINFTBL.PLDUNIT[36] = 1b]
											<li>@ref D_IM_IIP_PARAM_PLDUNIT_MFT		[UNITINFTBL.PLDUNIT[37] = 1b]
										 </ul>
									   If T_IM_IIP_SL_CFG::load_unit_param_flag is set to
									   (D_IM_IIP_PARAM_PLDUNIT_FRECT0 | D_IM_IIP_PARAM_PLDUNIT_SL5),
									   UNITINFTBL.PLDUNIT[63:0] is set to 0000_0000_0000_8100h.
										 */
} T_IM_IIP_UNIT_CFG;

/** Im_IIP_Ctrl_UNITINFTBL() parameter structure */
typedef struct {
	T_IM_IIP_UNIT_CFG*	pLD[D_IM_IIP_UNIT_LD_MAXNUM];	/**< Pointer of Loader UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b.
										 <ul>
											<li>pLD[0]	Loader 0
											<li>pLD[1]	Loader 1
											<li>pLD[2]	Loader 2
											<li>pLD[3]	Loader 3
											<li>pLD[4]	Loader 4
											<li>pLD[5]	Loader 5
											<li>pLD[6]	Loader 6
											<li>pLD[7]	Loader 7
										 </ul> */
	T_IM_IIP_UNIT_CFG*	pFLT;		/**< Pointer of FLT UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b. */
	T_IM_IIP_UNIT_CFG*	pAFN[D_IM_IIP_UNIT_AFN_MAXNUM];	/**< Pointer of Affine UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b.
										 <ul>
											<li>pAFN[0]	Affine 0
											<li>pAFN[1]	Affine 1
											<li>pAFN[2]	Affine 2
											<li>pAFN[3]	Affine 3
										 </ul> */
	T_IM_IIP_UNIT_CFG*	pFRECT[D_IM_IIP_UNIT_FRECT_MAXNUM];	/**< Pointer of FRECT UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b.
										 <ul>
											<li>pFRECT[0]	FRECT 0
											<li>pFRECT[1]	FRECT 1
										 </ul> */
	T_IM_IIP_UNIT_CFG*	pMON;		/**< Pointer of Monitor UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b. */
	T_IM_IIP_UNIT_CFG*	pSL[D_IM_IIP_UNIT_SL_MAXNUM];	/**< Pointer of Store UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b.
										 <ul>
											<li>pSL[0]	Store 0
											<li>pSL[1]	Store 1
											<li>pSL[2]	Store 2
											<li>pSL[3]	Store 3
											<li>pSL[4]	Store 4
											<li>pSL[5]	Store 5
											<li>pSL[6]	Store 6
											<li>pSL[7]	Store 7
											<li>pSL[8]	Store 8
										 </ul> */
	T_IM_IIP_UNIT_CFG*	pCSC[D_IM_IIP_UNIT_CSC_MAXNUM];	/**< Pointer of CSC UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b.
										 <ul>
											<li>pCSC[0]	CSC 0
											<li>pCSC[1]	CSC 1
											<li>pCSC[2]	CSC 2
											<li>pCSC[3]	CSC 3
										 </ul> */
	T_IM_IIP_UNIT_CFG*	pLUT;		/**< Pointer of LUT UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b. */
	T_IM_IIP_UNIT_CFG*	pGPC;		/**< Pointer of GPC UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b. */
	T_IM_IIP_UNIT_CFG*	pBLEND[D_IM_IIP_UNIT_BLEND_MAXNUM];	/**< Pointer of Blend UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b.
										 <ul>
											<li>pBLEND[0]	Blend 0
											<li>pBLEND[1]	Blend 1
										 </ul> */
	T_IM_IIP_UNIT_CFG*	pCFL;	/**< Pointer of Cache Flow UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b.
										 */
	T_IM_IIP_UNIT_CFG*	pMFT;	/**< Pointer of MFT UNITINFTBL parameters.
										 If it is set to NULL, the unit's UNITINFTBL.HWEN is set to 0b.
										 */
} T_IM_IIP_UNITINFTBL;

/** Im_IIP_Ctrl_LUT() parameter structure */
typedef struct {
	UINT32					buffer_bytes;	/**< The bytes of user LUT buffer "buffer_addr". Should be align on 2. */
	const USHORT*			buffer_addr;	/**< Beginning address of user LUT buffer. */
	E_IM_IIP_LUTRAM_TYPE	lutram_type;	/**< Beginning position of written LUTRAM. */
} T_IM_IIP_LUT;

/** Im_IIP_Ctrl_AXI() parameter structure */
typedef struct {
	UINT32	trans_req_ctrl;		/**< Transfer request lines control.
									<TABLE>
									<TR><TH>Unit</TH><TH>Register</TH></TR>
									<TR><TD>Image Cache</TD><TD>PDCCTL.PDCPKG</TD></TR>
									<TR><TD>Dispatch Sequencer</TD><TD>D.C.</TD></TR>
									<TR><TD>AFN0</TD><TD>AFNAXICTL.AFN0PK</TD></TR>
									<TR><TD>AFN1</TD><TD>AFNAXICTL.AFN1PK</TD></TR>
									<TR><TD>AFN2</TD><TD>AFNAXICTL.AFN2PK</TD></TR>
									<TR><TD>AFN3</TD><TD>AFNAXICTL.AFN3PK</TD></TR>
									<TR><TD>FRECT0</TD><TD>FRAXICTL.FR0PKG</TD></TR>
									<TR><TD>FRECT1</TD><TD>FRAXICTL.FR1PKG</TD></TR>
									<TR><TD>GPC</TD><TD>D.C.</TD></TR>
									<TR><TD>SL2</TD><TD>SLAXICTL.SL2PKG[1:0]</TD></TR>
									<TR><TD>SL3</TD><TD>SLAXICTL.SL3PKG[1:0]</TD></TR>
									<TR><TD>SL4</TD><TD>SLAXICTL.SL4PKG[1:0]</TD></TR>
									<TR><TD>SL5</TD><TD>SLAXICTL.SL5PKG[1:0]</TD></TR>
									<TR><TD>SL6</TD><TD>SLAXICTL.SL6PKG[1:0]</TD></TR>
									<TR><TD>SL7</TD><TD>SLAXICTL.SL7PKG[1:0]</TD></TR>
									<TR><TD>SL8</TD><TD>SLAXICTL.SL8PKG[1:0]</TD></TR>
									</TABLE>

									<ul>
										<li>@ref D_IM_IIP_NON_BURST			[SLAXICTL1.SLnPKG = 00b]
										<li>@ref D_IM_IIP_BURST_MAX_2LINES	[SLAXICTL1.SLnPKG = 01b]
										<li>@ref D_IM_IIP_BURST_MAX_4LINES	[SLAXICTL1.SLnPKG = 10b]
										<li>@ref D_IM_IIP_BURST_OVER_4LINES	[SLAXICTL1.SLnPKG = 11b]
									</ul>
									*/
	UINT32	cache_type;			/**< Cache type.
									<TABLE>
									<TR><TH>Unit</TH><TH>Register</TH></TR>
									<TR><TD>Image Cache</TD><TD>PDCCTL.PDCARCACHE[3:0]</TD></TR>
									<TR><TD>Dispatch Sequencer</TD><TD>DPAXICTL.DPARCACHE[3:0]</TD></TR>
									<TR><TD>AFN0</TD><TD>AFNAXICTL.AFN0ARCACHE[3:0]</TD></TR>
									<TR><TD>AFN1</TD><TD>AFNAXICTL.AFN1ARCACHE[3:0]</TD></TR>
									<TR><TD>AFN2</TD><TD>AFNAXICTL.AFN2ARCACHE[3:0]</TD></TR>
									<TR><TD>AFN3</TD><TD>AFNAXICTL.AFN3ARCACHE[3:0]</TD></TR>
									<TR><TD>FRECT0</TD><TD>FRAXICTL.FR0ARCACHE[3:0]</TD></TR>
									<TR><TD>FRECT1</TD><TD>FRAXICTL.FR1ARCACHE[3:0]</TD></TR>
									<TR><TD>GPC</TD><TD>GPCAXICTL.GPCARCACHE[3:0]</TD></TR>
									<TR><TD>SL0</TD><TD>SLAXICTL.SL0AWCACHE[3:0]</TD></TR>
									<TR><TD>SL1</TD><TD>SLAXICTL.SL1AWCACHE[3:0]</TD></TR>
									<TR><TD>SL2</TD><TD>SLAXICTL.SL2AWCACHE[3:0]</TD></TR>
									<TR><TD>SL3</TD><TD>SLAXICTL.SL3AWCACHE[3:0]</TD></TR>
									<TR><TD>SL4</TD><TD>SLAXICTL.SL4AWCACHE[3:0]</TD></TR>
									<TR><TD>SL5</TD><TD>SLAXICTL.SL5AWCACHE[3:0]</TD></TR>
									<TR><TD>SL6</TD><TD>SLAXICTL.SL6AWCACHE[3:0]</TD></TR>
									<TR><TD>SL7</TD><TD>SLAXICTL.SL7AWCACHE[3:0]</TD></TR>
									<TR><TD>SL8</TD><TD>SLAXICTL.SL8AWCACHE[3:0]</TD></TR>
									</TABLE>
									 */
	UINT32	protection_type;	/**< Protection type. 
									<TABLE>
									<TR><TH>Unit</TH><TH>Register</TH></TR>
									<TR><TD>Image Cache</TD><TD>PDCCTL.PDCARPROT[2:0]</TD></TR>
									<TR><TD>Dispatch Sequencer</TD><TD>DPAXICTL.DPARPROT[2:0]</TD></TR>
									<TR><TD>AFN0</TD><TD>AFNAXICTL.AFN0ARPROT[2:0]</TD></TR>
									<TR><TD>AFN1</TD><TD>AFNAXICTL.AFN1ARPROT[2:0]</TD></TR>
									<TR><TD>AFN2</TD><TD>AFNAXICTL.AFN2ARPROT[2:0]</TD></TR>
									<TR><TD>AFN3</TD><TD>AFNAXICTL.AFN3ARPROT[2:0]</TD></TR>
									<TR><TD>FRECT0</TD><TD>FRAXICTL.FR0ARPROT[2:0]</TD></TR>
									<TR><TD>FRECT1</TD><TD>FRAXICTL.FR1ARPROT[2:0]</TD></TR>
									<TR><TD>GPC</TD><TD>GPCAXICTL.GPCARPROT[2:0]</TD></TR>
									<TR><TD>SL0</TD><TD>SLAXICTL1.SL0AWPROT[2:0]</TD></TR>
									<TR><TD>SL1</TD><TD>SLAXICTL1.SL1AWPROT[2:0]</TD></TR>
									<TR><TD>SL2</TD><TD>SLAXICTL1.SL2AWPROT[2:0]</TD></TR>
									<TR><TD>SL3</TD><TD>SLAXICTL1.SL3AWPROT[2:0]</TD></TR>
									<TR><TD>SL4</TD><TD>SLAXICTL1.SL4AWPROT[2:0]</TD></TR>
									<TR><TD>SL5</TD><TD>SLAXICTL1.SL5AWPROT[2:0]</TD></TR>
									<TR><TD>SL6</TD><TD>SLAXICTL1.SL6AWPROT[2:0]</TD></TR>
									<TR><TD>SL7</TD><TD>SLAXICTL1.SL7AWPROT[2:0]</TD></TR>
									<TR><TD>SL8</TD><TD>SLAXICTL1.SL8AWPROT[2:0]</TD></TR>
									</TABLE>
									 */
} T_IM_IIP_CTRL_AXI;

/** Im_IIP_Ctrl_AXI_ALL() parameter structure */
typedef struct {
	T_IM_IIP_CTRL_AXI*		imgcache0;	/**< Pointer of AXI control parameters for Image data cache0. */
	T_IM_IIP_CTRL_AXI*		imgcache1;	/**< Pointer of AXI control parameters for Image data cache1. */
	T_IM_IIP_CTRL_AXI*		imgcache2;	/**< Pointer of AXI control parameters for Image data cache3. */
	T_IM_IIP_CTRL_AXI*		dp;			/**< Pointer of AXI control parameters for Dispatch sequencer. */
	T_IM_IIP_CTRL_AXI*		afn0;		/**< Pointer of AXI control parameters for Affine 0. */
	T_IM_IIP_CTRL_AXI*		afn1;		/**< Pointer of AXI control parameters for Affine 1. */
	T_IM_IIP_CTRL_AXI*		afn2;		/**< Pointer of AXI control parameters for Affine 2. */
	T_IM_IIP_CTRL_AXI*		afn3;		/**< Pointer of AXI control parameters for Affine 3. */
	T_IM_IIP_CTRL_AXI*		frect0;		/**< Pointer of AXI control parameters for FRECT 0. */
	T_IM_IIP_CTRL_AXI*		frect1;		/**< Pointer of AXI control parameters for FRECT 1. */
	T_IM_IIP_CTRL_AXI*		gpc;		/**< Pointer of AXI control parameters for GPC. */
	T_IM_IIP_CTRL_AXI*		sl0;		/**< Pointer of AXI control parameters for Store 0. */
	T_IM_IIP_CTRL_AXI*		sl1;		/**< Pointer of AXI control parameters for Store 1. */
	T_IM_IIP_CTRL_AXI*		sl2;		/**< Pointer of AXI control parameters for Store 2. */
	T_IM_IIP_CTRL_AXI*		sl3;		/**< Pointer of AXI control parameters for Store 3. */
	T_IM_IIP_CTRL_AXI*		sl4;		/**< Pointer of AXI control parameters for Store 4. */
	T_IM_IIP_CTRL_AXI*		sl5;		/**< Pointer of AXI control parameters for Store 5. */
	T_IM_IIP_CTRL_AXI*		sl6;		/**< Pointer of AXI control parameters for Store 6. */
	T_IM_IIP_CTRL_AXI*		sl7;		/**< Pointer of AXI control parameters for Store 7. */
	T_IM_IIP_CTRL_AXI*		sl8;		/**< Pointer of AXI control parameters for Store 8. */
} T_IM_IIP_CTRL_AXI_ALL;

/** Im_IIP_Ctrl_Base() parameter structure */
typedef struct {
#if 0	// B2Y Link not support
	UINT32		b2y_link;					/**< IZACTL1.LKEN <br><br> Control linkage between IIP and B2Y
												 <ul>
													 <li>@ref D_IM_IIP_LINK_B2Y_OFF		[IZACTL1.LKEN = 0b]
													 <li>@ref D_IM_IIP_LINK_B2Y_ON		[IZACTL1.LKEN = 1b]
												 </ul> */
#endif	// B2Y Link not support
	UINT32		enable_interrupt_flag;		/**< INTEN0 and INTEN1 register <br><br> Control interrupt.
												 <ul>
													 <li>@ref D_IM_IIP_INT_FACTOR_IZAEND	[INTEN0.IZAEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_AXIERR	[INTEN0.AXEEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL0END	[INTEN1.SL0FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL1END	[INTEN1.SL1FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2END	[INTEN1.SL2FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3END	[INTEN1.SL3FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4END	[INTEN1.SL4FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5END	[INTEN1.SL5FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6END	[INTEN1.SL6FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL7END	[INTEN1.SL7FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL8END	[INTEN1.SL8FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL0_LINEEND	[INTEN1.SL0MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL1_LINEEND	[INTEN1.SL1MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2_LINEEND	[INTEN1.SL2MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3_LINEEND	[INTEN1.SL3MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4_LINEEND	[INTEN1.SL4MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5_LINEEND	[INTEN1.SL5MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6_LINEEND	[INTEN1.SL6MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL7_LINEEND	[INTEN1.SL7MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL8_LINEEND	[INTEN1.SL8MEN = 1]
												</ul>
												When T_IM_IIP_CTRL_BASE::enable_interrupt_flag is set to
												(D_IM_IIP_INT_FACTOR_IZAEND | D_IM_IIP_INT_FACTOR_SL0END),
												Both INTEN0 and INTEN1 are set to 00000001h.
												 */
	UINT32		sl0_interrupt_lines;		/**< LINE_VAL_INT_SL0.LINE_VAL_INT_SL0[13:0] <br><br>
												 The interrupt line number which Store 2 transfers.
												 Should be align on 2.
												 If this set to 0, disable interrupt which is associated with INTEN1.SL0MEN.
											 */
	UINT32		sl1_interrupt_lines;		/**< LINE_VAL_INT_SL1.LINE_VAL_INT_SL1[13:0] <br><br>
												 The interrupt line number which Store 2 transfers.
												 Should be align on 2.
												 If this set to 0, disable interrupt which is associated with INTEN1.SL1MEN.
											 */
	UINT32		sl2_interrupt_lines;		/**< LINE_VAL_INT_SL2.LINE_VAL_INT_SL2[13:0] <br><br>
												 The interrupt line number which Store 2 transfers.
												 Should be align on 2.
												 If this set to 0, disable interrupt which is associated with INTEN1.SL2MEN.
											 */
	UINT32		sl3_interrupt_lines;		/**< LINE_VAL_INT_SL3.LINE_VAL_INT_SL3[13:0] <br><br>
												 The interrupt line number which Store 3 transfers.
												 Should be align on 2.
												 If this set to 0, disable interrupt which is associated with INTEN1.SL3MEN.
											 */
	UINT32		sl4_interrupt_lines;		/**< LINE_VAL_INT_SL4.LINE_VAL_INT_SL3[13:0] <br><br>
												 The interrupt line number which Store 4 transfers.
												 Should be align on 2.
												 If this set to 0, disable interrupt which is associated with INTEN1.SL4MEN.
											 */
	UINT32		sl5_interrupt_lines;		/**< LINE_VAL_INT_SL5.LINE_VAL_INT_SL5[13:0] <br><br>
												 The interrupt line number which Store 5 transfers.
												 Should be align on 2.
												 If this set to 0, disable interrupt which is associated with INTEN1.SL5MEN.
											 */
	UINT32		sl6_interrupt_lines;		/**< LINE_VAL_INT_SL6.LINE_VAL_INT_SL6[13:0] <br><br>
												 The interrupt line number which Store 6 transfers.
												 Should be align on 2.
												 If this set to 0, disable interrupt which is associated with INTEN1.SL6MEN.
											 */
	UINT32		sl7_interrupt_lines;		/**< LINE_VAL_INT_SL7.LINE_VAL_INT_SL7[13:0] <br><br>
												 The interrupt line number which Store 6 transfers.
												 Should be align on 2.
												 If this set to 0, disable interrupt which is associated with INTEN1.SL7MEN.
											 */
	UINT32		sl8_interrupt_lines;		/**< LINE_VAL_INT_SL8.LINE_VAL_INT_SL8[13:0] <br><br>
												 The interrupt line number which Store 6 transfers.
												 Should be align on 2.
												 If this set to 0, disable interrupt which is associated with INTEN1.SL8MEN.
											 */
	T_IM_IIP_CALLBACK		int_handler;	/**< The pointer of callback function. The callback function can be called in Im_IIP_Int_Handler.
												 If this parameter is set to NULL, Im_IIP_Int_Handler does not call callback function. <br><br>
												 Sample of callback function.<br>
												 @code
													VOID IIP_User_Callback( UINT32 interrupt_flag_mask )
													{
														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_IZAEND ) {
															// process according to IIPFLG interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_AXIERR ) {
															// process according to AXEFLG interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL0END ) {
															// process according to SL0FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL1END ) {
															// process according to SL1FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL2END ) {
															// process according to SL2FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL3END ) {
															// process according to SL3FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL4END ) {
															// process according to SL4FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL5END ) {
															// process according to SL5FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL6END ) {
															// process according to SL6FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL7END ) {
															// process according to SL6FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL6END ) {
															// process according to SL6FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL0_LINEEND ) {
															// process according to SL2MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL1_LINEEND ) {
															// process according to SL2MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL2_LINEEND ) {
															// process according to SL2MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL3_LINEEND ) {
															// process according to SL3MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL4_LINEEND ) {
															// process according to SL4MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL5_LINEEND ) {
															// process according to SL5MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL6_LINEEND ) {
															// process according to SL6MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL7_LINEEND ) {
															// process according to SL7MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL8_LINEEND ) {
															// process according to SL8MF interrupt
														}
													}
												 @endcode
											 */
	UINT32		cache_flush_ctrl[D_IM_IIP_CACHE_MAXNUM];	/**< PDCCTL[n].FLHALL <br><br> Control flush cache.
												 <ul>
													 <li>@ref D_IM_IIP_NON_FLUSH_IMGCACHE_START	[PDCCTL.FLHALL = 0b]
													 <li>@ref D_IM_IIP_FLUSH_IMGCACHE_START		[PDCCTL.FLHALL = 1b]
												 </ul> */
	UINT32		ext_bit_ctrl_less_8bits[D_IM_IIP_CACHE_MAXNUM];	/**< PDCCTL[n].EXTBIT <br><br> bit extenstion.
												 <ul>
													 <li>@ref D_IM_IIP_EXTBIT_LOWER_ALIGNED		[PDCCTL.EXTBIT = 0b]
													 <li>@ref D_IM_IIP_EXTBIT_UPPER_ALIGNED		[PDCCTL.EXTBIT = 1b]
												 </ul> */
} T_IM_IIP_CTRL_BASE;

/** Im_IIP_Get_MAXMON() and Im_IIP_Get_MINMON() parameter structure */
typedef struct {
	USHORT	pos_x;		/**< MAXMON.HPOINTMAX_0/1/2/3[13:0], MINMON.HPOINTMIN_0/1/2/3[13:0] and ADDMON.HPOINTADD_0/1/2/3[13:0] */
	USHORT	pos_y;		/**< MAXMON.VPOINTMAX_0/1/2/3[13:0], MINMON.VPOINTMIN_0/1/2/3[13:0] and ADDMON.VPOINTADD_0/1/2/3[13:0] */
	INT32	value;		/**< MAXMON.MAXVAL_0/1/2/3[14:0], MINMON.MINVAL_0/1/2/3[14:0] and ADDMON.ADDVAL_0/1/2/3[31:0] */
} T_IM_IIP_MON;

/** Im_IIP_Get_MAXMON() parameter structure */
typedef struct {
	T_IM_IIP_MON	max[4];			/**< The result of maximum value monitor.
										 <ul>
											 <li>max[0]	D0 component
											 <li>max[1]	D1 component
											 <li>max[2]	D2 component
											 <li>max[3]	D3 component
										 </ul> */
} T_IM_IIP_MAXMON;

/** Im_IIP_Get_MINMON() parameter structure */
typedef struct {
	T_IM_IIP_MON	min[4];			/**< The result of minimum value monitor.
										 <ul>
											 <li>min[0]	D0 component
											 <li>min[1]	D1 component
											 <li>min[2]	D2 component
											 <li>min[3]	D3 component
										 </ul> */
} T_IM_IIP_MINMON;

/** Im_IIP_Get_ADDMON() parameter structure */
typedef struct {
	T_IM_IIP_MON	add[4];			/**< The result of cumulo-addition monitor.
										 <ul>
											 <li>add[0]	D0 component
											 <li>add[1]	D1 component
											 <li>add[2]	D2 component
											 <li>add[3]	D3 component
										 </ul> */
} T_IM_IIP_ADDMON;

/** Im_IIP_Get_ImgCache_Access_Info() parameter structure */
typedef struct {
	ULONG	real_access_counter;	/**< PDCRAC.PDCRAC[31:0]	*/
	ULONG	real_miss_counter;		/**< PDCRMC.PDCRMC[31:0]	*/
} T_IM_IIP_ACCESS_INFO;

/** Im_IIP_Get_AXI_Status() parameter structure */
typedef struct {
	ULONG	read_channel_status;	/**< AXRSTS	register */
	ULONG	write_channel_status;	/**< AXWSTS register */
} T_IM_IIP_AXI_STATUS;

/** Im_IIP_Get_Genline() parameter structure */
typedef struct {
	USHORT	b2y_outlineno;									/**< Generated lines of B2Y macro @@GENLINE.B2Y_OUTLINENO */
	USHORT	afn_outlineno[D_IM_IIP_UNIT_AFN_MAXNUM];		/**< Generated lines of AFN0~2 unit @@GENLINE.AFNn_OUTLINENO (n=0~3) */
	USHORT	frect_outlineno[D_IM_IIP_UNIT_FRECT_MAXNUM];	/**< Generated lines of FRECT0~1 unit @@GENLINE.FRn_OUTLINENO (n=0~1)*/
	USHORT	ld_outlineno[D_IM_IIP_UNIT_LD_MAXNUM];			/**< Generated lines of LD0~7 unit @@GENLINE.LDn_OUTLINENO (n=0~7) */
	USHORT	flt_outlineno;									/**< Generated lines of FLT unit @@GENLINE.FLT_OUTLINENO */
} T_IM_IIP_GENLINE;

/** Im_IIP_Get_Debug_Monitor() parameter structure */
typedef struct {
	ULLONG		data_out_port;								/**< DTOUT_MON register value. */
	ULLONG		waiting_port;								/**< WAIT_MON register value. */
	USHORT		imgcache_data_out[D_IM_IIP_CACHE_MAXNUM];	/**< PDC_DTOUT_MON register value. */
	USHORT		imgcache_ready[D_IM_IIP_CACHE_MAXNUM];		/**< PDC_RDY_MON register value. */
} T_IM_IIP_DEBUG_MONITOR;

/** @} */	// end of im_iip

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/** Set PLDUNIT 64bit value to register parameter. */
#define Im_IIP_Set_PLDUNIT( pldunit_reg, pldunit_val )	( \
		pldunit_reg.bit.PLDUNIT_SL_LO = Im_IIP_Get_PLDUNIT_Lo( pldunit_val ), \
		pldunit_reg.bit.PLDUNIT_SL_HI = Im_IIP_Get_PLDUNIT_Hi( pldunit_val ) \
		)

/** Get Lower 32bit of PLDUNIT value. */
#define Im_IIP_Get_PLDUNIT_Lo( pldunit_val )	( (ULONG)((pldunit_val) & 0xFFFFFFFFULL) )

/** Get Upper 32bit of PLDUNIT value. */
#define Im_IIP_Get_PLDUNIT_Hi( pldunit_val )	( (ULONG)(((pldunit_val) & 0xFFFFFFFF00000000ULL) >> 32) )

/** Get PLDUNIT 64bit value from register parameter. */
#define Im_IIP_Get_Reg_PLDUNIT64( pldunit_reg )	( \
		((ULLONG)pldunit_reg.bit.PLDUNIT_LO) & (((ULLONG)pldunit_reg.bit.PLDUNIT_HI) << 32) \
	)



/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/**
   @weakgroup im_iip
   @{
*/

/**
  HCLK ON
  @remarks		For debugging use only (for register dump on debugger)
*/
extern VOID Im_IIP_On_Hclk( VOID );

/**
  HCLK OFF
  @remarks		For debugging use only (for register dump on debugger)
*/
extern VOID Im_IIP_Off_Hclk( VOID );

/**
  PCLK ON
  @remarks		For debugging use only (for register dump on debugger)
*/
extern VOID Im_IIP_On_Pclk( VOID );

/**
  PCLK OFF
  @remarks		For debugging use only (for register dump on debugger)
*/
extern VOID Im_IIP_Off_Pclk( VOID );

/**
   Initialize driver. This API executes software-reset [IZACTL0.SR = 1b] and releases software-reset [IZACTL0.SR = 0b].

   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_ERR				: Error. This API is called when IIP is running.
   @remarks		After this API is called, any other API should be called. So, this is called at first.<br>
				This API should be called when IIP is not running.
@remarks		This API uses DDIM_User_Set_Flg().
*/
extern INT32 Im_IIP_Init( VOID );

/**
   Initialize IIP Unit. This API executes software-reset [UNITSR.UNITSR = 1b] and releases software-reset [UNITSR.UNITSR = 0b].

   @param[in]	unit_id						: UNIT ID.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_ERR				: Error. This API is called when IIP Unit is running.
   @remarks		This API should be called when IIP Unit is not running.
*/
extern INT32 Im_IIP_Init_Unit( const E_IM_IIP_UNIT_ID unit_id );

/**
   Open driver. This API gets IIP semaphore using system-call.

   @param[in]	wait_time : Wait time [msec]. The valid range is -1, 0, ... .
							If this parameter is set to -1,
							API waits permanently unless system-call gets IIP semaphore.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System-call error.
   @retval		D_IM_IIP_TIMEOUT_ERR		: Wait timeout error.
   @remarks		This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
				This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern INT32 Im_IIP_Open( const INT32 wait_time );

/**
   Close driver. This API releases IIP semaphore using system-call.

   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System-call error.
   @remarks		This API uses DDIM_User_Sig_Sem().
   @remarks		This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_IIP_Close( VOID );

/**
   Configure PIXFMTTBL registers.

   @param[in]	pixid	: PIXID [0~15]
   @param[in]	tbl		: Pointer of PIXFMTTBL parameters. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Ctrl_PIXFMTTBL( const UINT32 pixid, const T_IM_IIP_PIXFMTTBL* const tbl );

/**
   Configure UNITINFTBL registers.

   @param[in]	tbl		: Pointer of UNITINFTBL parameter. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Ctrl_UNITINFTBL( const T_IM_IIP_UNITINFTBL* const tbl );

/**
   Configure LUTRAM registers.<br>
   The value of user LUT buffer are set to LUTRAM register.<br>
   @image html im_iip_lutram.png

   @param[in]	lut		: Pointer of Im_IIP_Ctrl_LUT's parameters. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_ERR				: Error. When the value AHBSTS.LUTxST is 1b.
*/
extern INT32 Im_IIP_Ctrl_LUT( const T_IM_IIP_LUT* const lut );

/**
   Open driver. This API gets IIP's UNIT and PIXFMTTBL semaphore using SWTRG system-call.

   @param[in]	unitid_bitmask64 : UNIT ID bitmask. refer D_IM_IIP_PARAM_PLDUNIT_xxx.
   @param[in]	pixid_bitmask : PIXFMTTBL ID bitmask. @ref E_IM_IIP_PIXID.
   @param[in]	open_res_bitmask : Resource ID bitmask. @ref E_IM_IIP_OPEN_RES.
   @param[in]	wait_time : Wait time [msec]. The valid range is -1, 0, ... .
							If this parameter is set to -1,
							API waits permanently unless system-call gets IIP semaphore.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_TIMEOUT_ERR		: Wait timeout error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System-call error.
   @remarks		This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
				This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
				This API uses DDIM_User_Sig_Sem().
   @remarks		This API uses DDIM_User_Set_Flg().
   @remarks		This API uses DDIM_User_Clr_Flg().
   @remarks		This API uses DDIM_User_Pol_Flg().
   @remarks		This API uses DDIM_User_Twai_Flg().
   @remarks		This API uses DDIM_User_Get_Tim().
*/
extern INT32 Im_IIP_Open_SWTRG( const ULLONG unitid_bitmask64, const UINT32 pixid_bitmask, const UINT32 open_res_bitmask, const INT32 wait_time );

/**
   Close driver. This API releases IIP's UNIT and PIXFMTTBL semaphore using SWTRG system-call.

   @param[in]	unitid_bitmask64 : UNIT ID bitmask. D_IM_IIP_PARAM_PLDUNIT_xxx.
   @param[in]	pixid_bitmask : PIXFMTTBL ID bitmask. @ref E_IM_IIP_PIXID.
   @param[in]	open_res_bitmask : Other resource bitmask. @ref E_IM_IIP_OPEN_RES.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System-call error.
   @retval		D_IM_IIP_ABORT_FAILED_ERR	: Abort-failed error. Must be LSI-reset.
   @remarks		This API uses DDIM_User_Sig_Sem().
   @remarks		This API uses DDIM_User_Set_Flg().
   @remarks		This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_IIP_Close_SWTRG( const ULLONG unitid_bitmask64, const UINT32 pixid_bitmask, const UINT32 open_res_bitmask );

/**
   Configure UNITINFTBL registers for SWTRG.

   @param[in]	unit_id			: UNIT ID. @ref E_IM_IIP_UNIT_ID.
   @param[in]	p_unitcfg		: Pointer of UNITINFTBL parameter. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Ctrl_SWTRG_Unit( const E_IM_IIP_UNIT_ID unit_id, const T_IM_IIP_UNIT_CFG* const p_unitcfg );

/**
   Configure AXI control registers except for Cache and Protection type.

   @param[in]	axi_id	: AXI ID
   @param[in]	cfg		: Pointer of AXI control parameter. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Ctrl_AXI( const E_IM_IIP_AXI_ID axi_id, const T_IM_IIP_CTRL_AXI* const cfg );

/**
   Configure AXI control registers including Cache and Protection type.

   @param[in]	cfg		: Pointer of AXI control parameter. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @remarks		Need not configure Cache and protection type.
				So, Im_IIP_Ctrl_AXI() is used most commonly
				if firmware want to configure AXI control registers.
*/
extern INT32 Im_IIP_Ctrl_AXI_ALL( const T_IM_IIP_CTRL_AXI_ALL* const cfg );

/**
   Configure the following.
   <ul>
	   <li>Frame mode (IIP running mode)
	   <li>Bank control
	   <li>Linkage control [IIP-B2Y]
	   <li>Cache control
	   <li>Interruption
   </ul>

   @param[in]	cfg		: Pointer of Im_IIP_Ctrl_Base's parameters. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Ctrl_Base( const T_IM_IIP_CTRL_BASE* const cfg );

/**
   Set Interrupt enable bit.
   Clear Interrupt cause flag bit.

   @param[in]	interrupt_bitmask		: Interrupt bit mask.
												<ul>
													 <li>@ref D_IM_IIP_INT_FACTOR_IZAEND	[INTEN0.IZAEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_AXIERR	[INTEN0.AXEEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL0END	[INTEN1.SL0FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL1END	[INTEN1.SL1FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2END	[INTEN1.SL2FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3END	[INTEN1.SL3FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4END	[INTEN1.SL4FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5END	[INTEN1.SL5FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6END	[INTEN1.SL6FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL7END	[INTEN1.SL7FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL8END	[INTEN1.SL8FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL0_LINEEND	[INTEN1.SL0MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL1_LINEEND	[INTEN1.SL1MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2_LINEEND	[INTEN1.SL2MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3_LINEEND	[INTEN1.SL3MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4_LINEEND	[INTEN1.SL4MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5_LINEEND	[INTEN1.SL5MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6_LINEEND	[INTEN1.SL6MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL7_LINEEND	[INTEN1.SL7MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL8_LINEEND	[INTEN1.SL8MEN = 1]
												</ul>
												When interrupt_bit set to (D_IM_IIP_INT_FACTOR_IZAEND | D_IM_IIP_INT_FACTOR_SL0END) and paermission_flg set to 1,
												Both INTEN0 and INTEN1 are set to 00000001h.
   @param[in]	permission_flg		: 1:Set interrupt to enable following interrupt_bit. 0:Set interrupt to disable following interrupt_bit.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Set_Interrupt( const ULONG interrupt_bitmask, const UCHAR permission_flg );

/**
   Set Interrupt enable bit.

   @param[in]	interrupt_bitmask	: Interrupt bit mask.
												<ul>
													 <li>@ref D_IM_IIP_INT_FACTOR_IZAEND	[INTEN0.IZAEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_AXIERR	[INTEN0.AXEEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL0END	[INTEN1.SL0FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL1END	[INTEN1.SL1FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2END	[INTEN1.SL2FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3END	[INTEN1.SL3FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4END	[INTEN1.SL4FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5END	[INTEN1.SL5FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6END	[INTEN1.SL6FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2_LINEEND	[INTEN1.SL2MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3_LINEEND	[INTEN1.SL3MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4_LINEEND	[INTEN1.SL4MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5_LINEEND	[INTEN1.SL5MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6_LINEEND	[INTEN1.SL6MEN = 1]
												</ul>
												When interrupt_bit set to (D_IM_IIP_INT_FACTOR_IZAEND | D_IM_IIP_INT_FACTOR_SL0END) and paermission_flg set to 1,
												Both INTEN0 and INTEN1 are set to 00000001h.
   @param[in]	int_handler		: The pointer of callback function. The callback function can be called in Im_IIP_Int_Handler.
												 If this parameter is set to NULL, Im_IIP_Int_Handler does not call callback function. <br><br>
												 Sample of callback function.<br>
												 @code
													VOID IIP_User_Callback( UINT32 interrupt_flag_mask )
													{
														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_IZAEND ) {
															// process according to IIPFLG interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_AXIERR ) {
															// process according to AXEFLG interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL0END ) {
															// process according to SL0FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL1END ) {
															// process according to SL1FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL2END ) {
															// process according to SL2FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL3END ) {
															// process according to SL3FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL4END ) {
															// process according to SL4FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL5END ) {
															// process according to SL5FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL6END ) {
															// process according to SL6FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL2_LINEEND ) {
															// process according to SL2MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL3_LINEEND ) {
															// process according to SL3MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL4_LINEEND ) {
															// process according to SL4MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL5_LINEEND ) {
															// process according to SL5MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL6_LINEEND ) {
															// process according to SL6MF interrupt
														}
													}
												 @endcode
   @retval		D_IM_IIP_OK					: Normal end.
*/
extern INT32 Im_IIP_Set_IntHandler( const ULONG interrupt_bitmask, const T_IM_IIP_CALLBACK int_handler );

/**
   Set interrupt line number which Store 3~6 transfers.

   @param[in]	interrupt_bitmask	: Target line interrupt bit mask.
												<ul>
													 <li>@ref D_IM_IIP_INT_FACTOR_SL0_LINEEND	[LINE_VAL_INT_SL0.LINE_VAL_INT_SL0[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL1_LINEEND	[LINE_VAL_INT_SL1.LINE_VAL_INT_SL1[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2_LINEEND	[LINE_VAL_INT_SL2.LINE_VAL_INT_SL2[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3_LINEEND	[LINE_VAL_INT_SL3.LINE_VAL_INT_SL3[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4_LINEEND	[LINE_VAL_INT_SL4.LINE_VAL_INT_SL4[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5_LINEEND	[LINE_VAL_INT_SL5.LINE_VAL_INT_SL5[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6_LINEEND	[LINE_VAL_INT_SL6.LINE_VAL_INT_SL6[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL7_LINEEND	[LINE_VAL_INT_SL7.LINE_VAL_INT_SL7[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL8_LINEEND	[LINE_VAL_INT_SL8.LINE_VAL_INT_SL8[13:0] = trans_lines]
												 </ul>
   @param[in]	trans_lines			: The interrupt line number which Store 0~8 transfers.
												 Should be align on 2.
												 If this set to 0, disable interrupt which is associated with INTEN1.SLnMEN.
   @retval		D_IM_IIP_OK					: Normal end.
*/
extern INT32 Im_IIP_Set_SLIntLines( const ULONG interrupt_bitmask, const UINT32 trans_lines );

/**
   Update PADRS.

   @param[in]	unit_id			: UNIT ID.
   @param[in]	unit_param_addr	: Beginning address of unit parameter. Should be align on 8.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Update_PADRS( const E_IM_IIP_UNIT_ID unit_id, const ULONG unit_param_addr );

/**
   Start IIP. In this API, IZACTL1.TRG[1:0] is set to 01b after event flag is cleard.

   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System-call error.
   @retval		D_IM_IIP_ERR				: Error. IIP is running before IZACTL1.TRG[1:0] is set to 01b.
   @remarks		This API should be called when IIP is not running. <br>
				This API uses DDIM_User_Clr_Flg() just before IZACTL1.TRG[1:0] is set to 01b.
*/
extern INT32 Im_IIP_Start( VOID );

/**
   Configure UNITINFTBL.SWTRG[1:0] which is associated with unit_id for B2Y-IIP link mode.

   @param[in]	unit_id	: UNIT ID.
   @param[in]	swtrg	: UNITINFTBL.SWTRG[1:0].
						  <ul>
							  <li>@ref D_IM_IIP_SWTRG_OFF	[UNITINFTBL.SWTRG[1:0] = 00b]
							  <li>@ref D_IM_IIP_SWTRG_ON	[UNITINFTBL.SWTRG[1:0] = 01b]
						  </ul>
   @param[in]	link_onoff	: D_IM_IIP_ENABLE_OFF:link disable. D_IM_IIP_ENABLE_ON:link enable.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_ERR				: Error. When UNITINFTBL.HWEN is set to 1b.
   @remarks		This API should be called while IIP is running.
*/
extern INT32 Im_IIP_LinkStart_SWTRG( const E_IM_IIP_UNIT_ID unit_id, const INT32 swtrg, const UINT32 link_onoff );

/**
   Configure UNITINFTBL.SWTRG[1:0] which is associated with unit_id.

   @param[in]	unit_id	: UNIT ID.
   @param[in]	swtrg	: UNITINFTBL.SWTRG[1:0].
						  <ul>
							  <li>@ref D_IM_IIP_SWTRG_OFF	[UNITINFTBL.SWTRG[1:0] = 00b]
							  <li>@ref D_IM_IIP_SWTRG_ON	[UNITINFTBL.SWTRG[1:0] = 01b]
						  </ul>
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_ERR				: Error. When UNITINFTBL.HWEN is set to 1b.
   @remarks		This API should be called while IIP is running.
*/
extern INT32 Im_IIP_Start_SWTRG( const E_IM_IIP_UNIT_ID unit_id, const INT32 swtrg );

/**
   Stop IIP. If unit is started using Im_IIP_SWTRG(), firmware must stop IIP using this API.
   <br>
   When stop_type is set to D_IM_IIP_STOP, IIP stops after 1 frame process is finished.
   Firmware can detect IIP using following methods.
   <ul>
	   <li>Detect IIPFLG interrupt [using Im_IIP_Wait_End()]
	   <li>Check the status which ALL Store unit is stopped. [using Im_IIP_Get_SL_Status()]
   </ul>
   <br>
   When stop_type is set to D_IM_IIP_ABORT, IIP stops at once.

   @param[in]	stop_type	: Stop control
							  <ul>
								  <li>@ref D_IM_IIP_STOP	[IZACTL1.TRG[1:0] = 00b]
								  <li>@ref D_IM_IIP_ABORT	[IZACTL2.ABORT[1:0] = 00b]
							  </ul>
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System-call error.
   @retval		D_IM_IIP_ERR				: Error. After IZACTL2.ABORT[1:0] is set to 00b, IIP is still running.
   @remarks		This API uses DDIM_User_Set_Flg() right after IZACTL1.TRG[1:0] is set to 01b.
				So, it cancels Im_IIP_Wait_End's wait process.
   @remarks		This API uses DDIM_User_Clr_Flg().
   @remarks		This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Im_IIP_Stop( const UINT32 stop_type );

/**
   Wait process until interruption occurs.

   @param[in]	p_wait_factor	: Pointer of factor parameter which release wait process.
								  If Im_IIP_Stop( ch, D_IM_IIP_ABORT ) is called, factor parameter is set to 00000800h.
   @param[in]	wait_factor		: wait factor
								  <ul>
										 <li>@ref D_IM_IIP_INT_FACTOR_IZAEND
										 <li>@ref D_IM_IIP_INT_FACTOR_AXIERR
										 <li>@ref D_IM_IIP_INT_FACTOR_SL0END
										 <li>@ref D_IM_IIP_INT_FACTOR_SL1END
										 <li>@ref D_IM_IIP_INT_FACTOR_SL2END
										 <li>@ref D_IM_IIP_INT_FACTOR_SL3END
										 <li>@ref D_IM_IIP_INT_FACTOR_SL4END
										 <li>@ref D_IM_IIP_INT_FACTOR_SL5END
										 <li>@ref D_IM_IIP_INT_FACTOR_SL6END
										 <li>@ref D_IM_IIP_INT_FACTOR_SL2_LINEEND
										 <li>@ref D_IM_IIP_INT_FACTOR_SL3_LINEEND
										 <li>@ref D_IM_IIP_INT_FACTOR_SL4_LINEEND
										 <li>@ref D_IM_IIP_INT_FACTOR_SL5_LINEEND
										 <li>@ref D_IM_IIP_INT_FACTOR_SL6_LINEEND
								  </ul>
								  When wait_factor is set to (D_IM_IIP_INT_FACTOR_IZAEND | D_IM_IIP_INT_FACTOR_SL3END),
								  IZAFLG and SL3FF interrupt are used as wait factors.
   @param[in]	wait_mode		: wait mode
								  <ul>
									  <li>@ref D_IM_IIP_AND_WAIT	AND wait
									  <li>@ref D_IM_IIP_OR_WAIT		OR wait
								  </ul>
   @param[in]	wait_time : Wait time [msec]. The valid range is -1, 0, ... .
							If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System-call error.
   @remarks		This API uses DDIM_User_Twai_Flg(). <br>
				This API uses DDIM_User_Set_Flg(). <br>
				This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_IIP_Wait_End( UINT32* const p_wait_factor, const UINT32 wait_factor, const UINT32 wait_mode, const INT32 wait_time );

/**
   Interrupt Handler. This API executes following process.
   <ul>
	   <li>Clear interrupt (Write INTIZ0 and INTIZ1 registers)
	   <li>Set event-flag using DDIM_User_Set_Flg()
	   <li>Call callback function (if callback function is set at Im_IIP_Ctrl_Base().)
   </ul>

   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System-call error.
*/
extern INT32 Im_IIP_Int_Handler( VOID );

/**
   Zero clear Histgram of AHB registers.

   @param[in]	histgram_a_clr_flag	: Zero clear flag of HISTGRAM A.
										<ul>
											<li>0: Not zero clear.
											<li>1: Zero clear HISTGRAM.
										</ul>
   @param[in]	histgram_b_clr_flag	: Zero clear flag of HISTGRAM B.
										<ul>
											<li>0: Not zero clear.
											<li>1: Zero clear HISTGRAM.
										</ul>
   @param[in]	histgram_c_clr_flag	: Zero clear flag of HISTGRAM C.
										<ul>
											<li>0: Not zero clear.
											<li>1: Zero clear HISTGRAM.
										</ul>
   @param[in]	histgram_d_clr_flag	: Zero clear flag of HISTGRAM D.
										<ul>
											<li>0: Not zero clear.
											<li>1: Zero clear HISTGRAM.
										</ul>
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_ERR				: Error. This API is called when the value of AHBSTS.HISSTS is 1b.
*/
extern INT32 Im_IIP_Clear_HISTGRAM( const UINT32 histgram_a_clr_flag, const UINT32 histgram_b_clr_flag, const UINT32 histgram_c_clr_flag, const UINT32 histgram_d_clr_flag );

/**
   Get the value of HISTGRAM registers.
   This API word-copies the value from HISTGRAM A/B/C register to user buffer.

   @param[out]	buffer_histgram_a	: Beginning address of HISTGRAM A's user buffer. <br>
									  If it is set to NULL, there is no copy for HISTGRAM A.
									  Should be align on 4. Buffer size is @ref D_IM_IIP_HIST_MAXBYTES.
   @param[out]	buffer_histgram_b	: Beginning address of HISTGRAM B's user buffer. <br>
									  If it is set to NULL, there is no copy for HISTGRAM B.
									  Should be align on 4. Buffer size is @ref D_IM_IIP_HIST_MAXBYTES.
   @param[out]	buffer_histgram_c	: Beginning address of HISTGRAM C's user buffer. <br>
									  If it is set to NULL, there is no copy for HISTGRAM C.
									  Should be align on 4. Buffer size is @ref D_IM_IIP_HIST_MAXBYTES.
   @param[out]	buffer_histgram_d	: Beginning address of HISTGRAM D's user buffer. <br>
									  If it is set to NULL, there is no copy for HISTGRAM D.
									  Should be align on 4. Buffer size is @ref D_IM_IIP_HIST_MAXBYTES.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_ERR				: Error. This API is called when the value of AHBSTS.HISSTS is 1b.
*/
extern INT32 Im_IIP_Get_HISTGRAM( ULONG* const buffer_histgram_a, ULONG* const buffer_histgram_b, ULONG* const buffer_histgram_c, ULONG* const buffer_histgram_d );

/**
   Get the most occured-value.

   @param[out]	histmax_a	: Pointer of parameter which the most occured-value of HISTGRAM A is stored. <br>
							  If it is set to NULL, API skips to get the most occured-value.
   @param[out]	histmax_b	: Pointer of parameter which the most occured-value of HISTGRAM B is stored. <br>
							  If it is set to NULL, API skips to get the most occured-value.
   @param[out]	histmax_c	: Pointer of parameter which the most occured-value of HISTGRAM C is stored. <br>
							  If it is set to NULL, API skips to get the most occured-value.
   @param[out]	histmax_d	: Pointer of parameter which the most occured-value of HISTGRAM D is stored. <br>
							  If it is set to NULL, API skips to get the most occured-value.
   @retval		D_IM_IIP_OK					: Normal end.
*/
extern INT32 Im_IIP_Get_HISTMAX( UCHAR* const histmax_a, UCHAR* const histmax_b, UCHAR* const histmax_c, UCHAR* const histmax_d );

/**
   Get result of maximum value monitor.

   @param[out]	mon			: Pointer of result parameters. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Get_MAXMON( T_IM_IIP_MAXMON* const mon );

/**
   Get result of minimum value monitor.

   @param[out]	mon			: Pointer of result parameters. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Get_MINMON( T_IM_IIP_MINMON* const mon );

/**
   Get result of cumulo-addition monitor.

   @param[out]	mon			: Pointer of result parameters. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Get_ADDMON( T_IM_IIP_ADDMON* const mon );

/**
   Get access information for image data cache.

   @param[in]	cache_ch	: Cache channel number.
   @param[out]	info	: Pointer of information parameters. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Get_ImgCache_Access_Info( const E_IM_IIP_CACHE_CH cache_ch, T_IM_IIP_ACCESS_INFO* const info );

/**
   Get AXI status registers.

   @param[out]	sts		: Pointer of AXI status parameters. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Get_AXI_Status( T_IM_IIP_AXI_STATUS* const sts );

/**
   Get Store unit status [IZACTL1.SLALE].

   @param[out]	sl_status	: Pointer of parameter which has all store unit status. Should not be NULL.
							  <ul>
								  <li>0	Some store units are running.
								  <li>1	All store unit is stopped.
							  </ul>
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Get_SL_Status( UINT32* const sl_status );

/**
   Get the latest unit parameter.
   <br>
   This API word-copies unit parameter value to user buffer.
   <br>
   Use in debug.

   @param[in]	unit_id					: UNIT ID
   @param[out]	p_param_buffer			: Beginning address of user buffer. Should be aling on 4. Should not be NULL.
   @param[out]	size					: Copied size (bytes).
										  <ul>
											  <li>040h		Loader
											  <li>240h		SPF
											  <li>070h		Affine
											  <li>070h		FRECT
											  <li>060h		Monitor
											  <li>3A0h		Store
											  <li>060h		CSC
											  <li>0E0h		LUT
											  <li>01Ch		GPC
											  <li>0DCh		Blend
											  <li>020h		Cache Flow
											  <li>020h		MFT
											  <li>000h		Error
										  </ul>
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @remarks		This API should be called when IIP is not running.
*/
extern INT32 Im_IIP_Get_UNIT_PARAM( const E_IM_IIP_UNIT_ID unit_id, ULONG* const p_param_buffer, UINT32* const size );

/**
   Set the access enable register (ACESEN0).

   @param[in]	res_bitmask				: Target resource bitmask.<br>
											<ul>
												<li>	E_IM_IIP_OPEN_RES_LUT_A		[ACESEN0.LUARAC]
												<li>	E_IM_IIP_OPEN_RES_LUT_B		[ACESEN0.LUARBC]
												<li>	E_IM_IIP_OPEN_RES_LUT_C		[ACESEN0.LUARCC]
												<li>	E_IM_IIP_OPEN_RES_LUT_D		[ACESEN0.LUARDC]
												<li>	E_IM_IIP_OPEN_RES_LUT_E		[ACESEN0.LUAREC]
												<li>	E_IM_IIP_OPEN_RES_LUT_F		[ACESEN0.LUARFC]
												<li>	E_IM_IIP_OPEN_RES_HIST_0	[ACESEN0.MO0RAC]
												<li>	E_IM_IIP_OPEN_RES_HIST_1	[ACESEN0.MO1RAC]
												<li>	E_IM_IIP_OPEN_RES_HIST_2	[ACESEN0.MO2RAC]
												<li>	E_IM_IIP_OPEN_RES_HIST_3	[ACESEN0.MO3RAC]
												<li>	E_IM_IIP_OPEN_RES_CACHE1	[ACESEN0.CA1RAC]
												<li>	E_IM_IIP_OPEN_RES_CACHE2	[ACESEN0.CA2RAC]
											</ul>

   @param[in]	permission_flg			: Access permission. 0:register set to 0.  1:register set to 1.
   @retval		D_IM_IIP_OK				: Normal end.
*/
extern INT32 Im_IIP_Set_AccessEnable( const UINT32 res_bitmask, const UCHAR permission_flg );

#if 0	// B2Y Link not support
/**
   Set Link enable register (LKEN).

   @param[in]	b2y_link			: <ul>
   										<li> @ref D_IM_IIP_ENABLE_ON : B2Y Link enable
   										<li> @ref D_IM_IIP_ENABLE_OFF : B2Y Linke disable
									  </ul>
   @retval		D_IM_IIP_OK				: Normal end.
*/
extern INT32 Im_IIP_Set_B2YLinkEnable( const UCHAR b2y_link );
#endif	// B2Y Link not support

/**
   Get Generated lines status of B2Y macro, AFN0~2 units, FRECT0~2 units, LD0~7 units and FLT unit.

   @param[out]	genline	: Pointer of parameter which has all macro and all units status. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
extern INT32 Im_IIP_Get_Genline( T_IM_IIP_GENLINE* const genline );

/**
   Get Debug monitor registers value.

   @param[out]	monitor_val	: Pointer of parameter which has debug monitor registers value. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
*/
extern INT32 Im_IIP_Get_Debug_Monitor( T_IM_IIP_DEBUG_MONITOR* const monitor_val );

/**
   Start Unit register dump to UNITDEBUG register memory space.

   @param[in]	unit_id					: UNIT ID
   @retval		D_IM_IIP_OK				: Normal end.
*/
extern INT32 Im_IIP_Start_DumpUnitReg( const E_IM_IIP_UNIT_ID unit_id );

/**
   Start Unit register dump to UNITDEBUG register memory space.

   @param[in]	unit_name				: UNIT Name.<br>
   										<ul>
   										<li> @ref D_IM_IIP_ENABLE_ON : B2Y Link enable
   										<li> @ref D_IM_IIP_ENABLE_OFF : B2Y Linke disable
									  </ul>
   @retval		D_IM_IIP_OK				: Normal end.
*/
extern INT32 Im_IIP_Start_DumpUnitReg_ByName( const CHAR* const unit_name );

/**
   Stop Unit register dump to UNITDEBUG register memory space.

   @retval		D_IM_IIP_OK				: Normal end.
*/
extern INT32 Im_IIP_Stop_DumpUnitReg( VOID );

/** @} */	// end of im_iip



#ifdef __cplusplus
}
#endif	// __cplusplus










//-----------------------------------------------------------------------
#ifdef CO_IM_IIP_ENABLE_UTILITY

#include "im_iip_param.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_IM_IIP_UTIL_RR_BUF_BYTES				(268 +8)	/**< Buffer bytes of IM_IIP_Utility_ResizeRotate() */
#define D_IM_IIP_UTIL_CSC_BUF_BYTES				(280 +8)	/**< Buffer bytes of IM_IIP_Utility_CSC() */
#define D_IM_IIP_UTIL_LUT_BUF_BYTES				(408 +8)	/**< Buffer bytes of IM_IIP_Utility_LUT() */
#define D_IM_IIP_UTIL_ALPHABLEND_BUF_BYTES		(352 +8)	/**< Buffer bytes of IM_IIP_Utility_AlphaBlend() */


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Angle of rotation (clockwise) */
typedef enum {
	E_IM_IIP_UTIL_ROTATE_000 = 0,	/**< 0 */
	E_IM_IIP_UTIL_ROTATE_090 = 1,	/**< 90 */
	E_IM_IIP_UTIL_ROTATE_180 = 2,	/**< 180 */
	E_IM_IIP_UTIL_ROTATE_270 = 3,	/**< 270 */
	E_IM_IIP_UTIL_ROTATE_MAX = 4,	/**< Maximum value */
} E_IM_IIP_UTIL_ROTATE;

/** Resize mode (Interpolation mode) */
typedef enum {
	E_IM_IIP_UTIL_RESIZE_MODE_NEAREST = E_IM_IIP_PARAM_ITMD_NEAREST,	/**< Nearest */
	E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR = E_IM_IIP_PARAM_ITMD_BI_LINEAR,	/**< Bi-linear */
	E_IM_IIP_UTIL_RESIZE_MODE_BICUBIC = E_IM_IIP_PARAM_ITMD_BI_CUBIC	/**< Bi-cubic */
} E_IM_IIP_UTIL_RESIZE_MODE;

/** Flip status */
typedef enum {
	E_IM_IIP_UTIL_FLIP_OFF = 0,			/**< Off */
	E_IM_IIP_UTIL_FLIP_MIRROR,			/**< Mirror */
	E_IM_IIP_UTIL_FLIP_REVERSE,			/**< Reverse */
	E_IM_IIP_UTIL_FLIP_MIR_AND_REV,		/**< Mirror and Reverse */
	E_IM_IIP_UTIL_FLIP_MAX,				/**< Maximum value */
} E_IM_IIP_UTIL_FLIP;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Rectangle (Source image area) */
typedef struct {
	DOUBLE	top;		/**< Vertical position of top-left pixel */
	DOUBLE	left;		/**< Horizontal position of top-left pixel */
	USHORT	width;		/**< Width */
	USHORT	lines;		/**< Lines */
} T_IM_IIP_UTIL_RECT;

/** Rectangle (Source image area) */
typedef struct {
	USHORT	top;		/**< Vertical position of top-left pixel */
	USHORT	left;		/**< Horizontal position of top-left pixel */
	USHORT	width;		/**< Width */
	USHORT	lines;		/**< Lines */
} T_IM_IIP_UTIL_RECT_INT;

/** Global Image info. */
typedef struct {
	UINT32				alpha;					/**< PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
													 <ul>
														 <li>@ref D_IM_IIP_ALPHA_FALSE	[PIXFMTTBL.ALPHA = 0b]
														 <li>@ref D_IM_IIP_ALPHA_TRUE	[PIXFMTTBL.ALPHA = 1b]
													 </ul> */
	E_IM_IIP_FTYPE		frame_type;				/**< PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
													 <ul>
														 <li>@ref E_IM_IIP_FTYPE_Y_PACKED_CBCR	[PIXFMTTBL.FTYPE[2:0] = 000b]
														 <li>@ref E_IM_IIP_FTYPE_CHUNKY			[PIXFMTTBL.FTYPE[2:0] = 001b]
														 <li>@ref E_IM_IIP_FTYPE_PLANE			[PIXFMTTBL.FTYPE[2:0] = 010b]
													 </ul> */
	E_IM_IIP_PFMT		pix_format;				/**< PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
													 The following formats are supported.
													 <ul>
														 <li>@ref E_IM_IIP_PFMT_YCC422		[PIXFMTTBL.PFMT[3:0] = 0000b]
														 <li>@ref E_IM_IIP_PFMT_YCC444		[PIXFMTTBL.PFMT[3:0] = 0001b]
														 <li>@ref E_IM_IIP_PFMT_YCC420		[PIXFMTTBL.PFMT[3:0] = 0010b]
														 <li>@ref E_IM_IIP_PFMT_PLANE		[PIXFMTTBL.PFMT[3:0] = 0110b]
													 </ul>
													 Other format is not supported.
												 */
	T_IM_IIP_CHUNKY		chunky_color;			/**< PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component. */
	UCHAR				sign_Y_G;				/**< PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
													 <ul>
														 <li>@ref D_IM_IIP_UNSIGNED_DATA	[PIXFMTTBL.SGYG = 0b]
														 <li>@ref D_IM_IIP_SIGNED_DATA		[PIXFMTTBL.SGYG = 1b]
													 </ul> */
	UCHAR				sign_Cb_B;				/**< PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
													 <ul>
														 <li>@ref D_IM_IIP_UNSIGNED_DATA	[PIXFMTTBL.SGB = 0b]
														 <li>@ref D_IM_IIP_SIGNED_DATA		[PIXFMTTBL.SGB = 1b]
													 </ul> */
	UCHAR				sign_Cr_R;				/**< PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
													 <ul>
														 <li>@ref D_IM_IIP_UNSIGNED_DATA	[PIXFMTTBL.SGR = 0b]
														 <li>@ref D_IM_IIP_SIGNED_DATA		[PIXFMTTBL.SGR = 1b]
													 </ul> */
	UCHAR				sign_D3;				/**< PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
													 <ul>
														 <li>@ref D_IM_IIP_UNSIGNED_DATA	[PIXFMTTBL.SGA = 0b]
														 <li>@ref D_IM_IIP_SIGNED_DATA		[PIXFMTTBL.SGA = 1b]
													 </ul> */
	USHORT				width;					/**< PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383] */
	USHORT				lines;					/**< PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]	*/
	T_IM_IIP_LINE_BYTES	line_bytes;				/**< The size of byte of Global image 1 line. */
	T_IM_IIP_IMG_ADDR	addr;					/**< Beginning address of Global image. */
	UCHAR				masterIF_Y_G;			/**< PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
													 <ul>
														 <li>@ref D_IM_IIP_MASTER_IF0	[PIXFMTTBL.MTIFYG = 0b]
														 <li>@ref D_IM_IIP_MASTER_IF1	[PIXFMTTBL.MTIFYG = 1b]
													 </ul> */
	UCHAR				masterIF_Cb_B;			/**< PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
													 <ul>
														 <li>@ref D_IM_IIP_MASTER_IF0	[PIXFMTTBL.MTIFB = 0b]
														 <li>@ref D_IM_IIP_MASTER_IF1	[PIXFMTTBL.MTIFB = 1b]
													 </ul> */
	UCHAR				masterIF_Cr_R;			/**< PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
													 <ul>
														 <li>@ref D_IM_IIP_MASTER_IF0	[PIXFMTTBL.MTIFR = 0b]
														 <li>@ref D_IM_IIP_MASTER_IF1	[PIXFMTTBL.MTIFR = 1b]
													 </ul> */
	UCHAR				masterIF_Alpha;			/**< PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
													 <ul>
														 <li>@ref D_IM_IIP_MASTER_IF0	[PIXFMTTBL.MTIFA = 0b]
														 <li>@ref D_IM_IIP_MASTER_IF1	[PIXFMTTBL.MTIFA = 1b]
													 </ul> */
} T_IM_IIP_UTIL_IMG_GBL;

/** Image info. */
typedef struct {
	T_IM_IIP_UTIL_RECT			rect;	/**< Rectangle info. (Source image area) */
	T_IM_IIP_UTIL_IMG_GBL		gbl;	/**< Global image info. */
} T_IM_IIP_UTIL_IMG;

/** Image info. */
typedef struct {
	T_IM_IIP_UTIL_RECT_INT		rect;	/**< Rectangle info. (Source image area) */
	T_IM_IIP_UTIL_IMG_GBL		gbl;	/**< Global image info. */
} T_IM_IIP_UTIL_IMG_INT;

/** Alpha unit info. */
typedef struct {
	E_IM_IIP_PARAM_ALPHA_STEP	step;			/**< Bit depth of alpha */
	E_IM_IIP_PARAM_ALPHA_SEL	sel;			/**< Select alpha value which IIP uses */
	E_IM_IIP_PARAM_ALPHA_OUT	out;			/**< Select alpha value which IIP outputs */
	USHORT						ALPHA_VAL[4];	/**< Fixed alpha value.
													 This parameter is valid
													 only when T_IM_IIP_UTIL_ALPHA::sel is E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL. */
} T_IM_IIP_UTIL_ALPHA;

/** Parameter of IM_IIP_Utility_ResizeRotate */
typedef struct {
	E_IM_IIP_UNIT_ID			afn_unitid;			/**< UNITID of AFN */
	E_IM_IIP_UNIT_ID			sl_unitid;			/**< UNITID of SL */
	E_IM_IIP_PIXID				src_pixid;			/**< Source image PIXID */
	E_IM_IIP_PIXID				dst_pixid;			/**< Destination image PIXID */
	E_IM_IIP_PARAM_SCUB			bi_cubic_unitid;	/**< BI-Cubic Unit ID */
	T_IM_IIP_UTIL_IMG			src;				/**< Source image info. */
	T_IM_IIP_UTIL_IMG_INT		dst;				/**< Destination image info. */
	E_IM_IIP_PARAM_CSEL			afn_cache_select;	/**< AFN cache select. */
	UINT32						pix_depth;			/**< PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
													 <ul>
														 <li>@ref D_IM_IIP_PDEPTH_8BITS	 [PIXFMTTBL.PDEPTH[2:0] = 000b]
														 <li>@ref D_IM_IIP_PDEPTH_12BITS [PIXFMTTBL.PDEPTH[2:0] = 001b]
														 <li>@ref D_IM_IIP_PDEPTH_16BITS [PIXFMTTBL.PDEPTH[2:0] = 010b]
													 </ul> */

	E_IM_IIP_UTIL_ROTATE		rotation;			/**< Angle of rotation */
	E_IM_IIP_UTIL_FLIP			flip;				/**< flip state */
	E_IM_IIP_UTIL_RESIZE_MODE	resize_mode;		/**< Resize mode */
} T_IM_IIP_UTIL_RR;

/** Parameter of IM_IIP_Utility_CSC */
typedef struct {
	E_IM_IIP_UNIT_ID			ld_unitid;			/**< UNITID of Loader */
	E_IM_IIP_UNIT_ID			csc_unitid;			/**< UNITID of CSC */
	E_IM_IIP_UNIT_ID			sl_unitid;			/**< UNITID of SL */
	E_IM_IIP_PIXID				src_pixid;			/**< Source image PIXID */
	E_IM_IIP_PIXID				dst_pixid;			/**< Destination image PIXID */
	T_IM_IIP_UTIL_IMG_INT		src;				/**< Source image info. */
	T_IM_IIP_UTIL_IMG_INT		dst;				/**< Destination image info. */
	E_IM_IIP_PARAM_CSEL			ld_cache_select;	/**< Loader cache select. */
	UINT32						pix_depth;			/**< PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
													 <ul>
														 <li>@ref D_IM_IIP_PDEPTH_8BITS	 [PIXFMTTBL.PDEPTH[2:0] = 000b]
														 <li>@ref D_IM_IIP_PDEPTH_12BITS [PIXFMTTBL.PDEPTH[2:0] = 001b]
														 <li>@ref D_IM_IIP_PDEPTH_16BITS [PIXFMTTBL.PDEPTH[2:0] = 010b]
													 </ul> */
	E_IM_IIP_ADEPTH				alpha_depth;		/**< PIXFMTTBL.ADEPTH[2:0] <br><br> Depth of alpha.
													 <ul>
														 <li>@ref E_IM_IIP_ADEPTH_8BITS		[PIXFMTTBL.ADEPTH[2:0] = 000b]
														 <li>@ref E_IM_IIP_ADEPTH_4BITS		[PIXFMTTBL.ADEPTH[2:0] = 011b]
													 </ul> */
	UINT32						alpha_subsampling;	/**< PIXFMTTBL.ASUBSPL[1:0] <br><br> Control Alpha subsampling.
													 <ul>
														 <li>@ref D_IM_IIP_ALPHA_NO_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 00b]
														 <li>@ref D_IM_IIP_ALPHA_H_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 01b]
														 <li>@ref D_IM_IIP_ALPHA_HV_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 10b]
													 </ul> */

	SHORT						csck[4][4];			/**< CSCK register */
	ULONG						csck_mode_sel;		/**< MDSEL register */
	E_IM_IIP_PARAM_ALPSEL		alpha_in_sel;		/**< ALPSEL register */
	E_IM_IIP_PARAM_ALOSEL		alpha_out_sel;		/**< ALOSEL register */
	UINT32						alpha_val;			/**< ALPVAL register.
														 This parameter is valid only when
														 T_IM_IIP_UTIL_CSC::alpha_sel is E_IM_IIP_PARAM_ALPSEL_ALPVAL_REG. */
	BYTE						sl_sftval;				/**< SL unit's SFTVAL */


} T_IM_IIP_UTIL_CSC;

/** LUTRAM info. */
typedef struct {
	UINT32	buffer_bytes[E_IM_IIP_LUTRAM_TYPE_MAX];	/**< The size of user LUT buffer [bytes].<br>
								   index 0 is used for LUT A.<br>
								   index 1 is used for LUT B.<br>
								   index 2 is used for LUT C.<br>
								   index 3 is used for LUT D.<br>
								   index 4 is used for LUT E.<br>
								   index 5 is used for LUT F.
								 */
	const USHORT*	buffer_addr[E_IM_IIP_LUTRAM_TYPE_MAX];		/**< Beginning address of user LUT buffer.<br>
								   index 0 is used for LUT A.<br>
								   index 1 is used for LUT B.<br>
								   index 2 is used for LUT C.<br>
								   index 3 is used for LUT D.<br>
								   index 4 is used for LUT E.<br>
								   index 5 is used for LUT F.
								 */
} T_IM_IIP_LUTRAM;

/** Parameter of IM_IIP_Utility_LUT */
typedef struct {
	E_IM_IIP_UNIT_ID			ld_unitid;			/**< UNITID of Loader */
	E_IM_IIP_UNIT_ID			lut_unitid;			/**< UNITID of LUT */
	E_IM_IIP_UNIT_ID			sl_unitid;			/**< UNITID of SL */
	E_IM_IIP_PIXID				src_pixid;			/**< Source image PIXID */
	E_IM_IIP_PIXID				dst_pixid;			/**< Destination image PIXID */
	T_IM_IIP_UTIL_IMG_INT		src;				/**< Source image info. */
	T_IM_IIP_UTIL_IMG_INT		dst;				/**< Destination image info. */
	E_IM_IIP_PARAM_CSEL			ld_cache_select;	/**< Loader cache select. */
	UINT32						pix_depth;			/**< PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
													 <ul>
														 <li>@ref D_IM_IIP_PDEPTH_8BITS	 [PIXFMTTBL.PDEPTH[2:0] = 000b]
														 <li>@ref D_IM_IIP_PDEPTH_12BITS [PIXFMTTBL.PDEPTH[2:0] = 001b]
														 <li>@ref D_IM_IIP_PDEPTH_16BITS [PIXFMTTBL.PDEPTH[2:0] = 010b]
													 </ul> */
	E_IM_IIP_ADEPTH				alpha_depth;		/**< PIXFMTTBL.ADEPTH[2:0] <br><br> Depth of alpha.
													 <ul>
														 <li>@ref E_IM_IIP_ADEPTH_8BITS		[PIXFMTTBL.ADEPTH[2:0] = 000b]
														 <li>@ref E_IM_IIP_ADEPTH_4BITS		[PIXFMTTBL.ADEPTH[2:0] = 011b]
													 </ul> */
	UINT32						alpha_subsampling;	/**< PIXFMTTBL.ASUBSPL[1:0] <br><br> Control Alpha subsampling.
													 <ul>
														 <li>@ref D_IM_IIP_ALPHA_NO_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 00b]
														 <li>@ref D_IM_IIP_ALPHA_H_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 01b]
														 <li>@ref D_IM_IIP_ALPHA_HV_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 10b]
													 </ul> */

	UINT32						lut_pix_depth[4];	/**< LUTCTL.LUTBIT <br>
														 index 0 is used for LUT A.<br>
														 index 1 is used for LUT B.<br>
														 index 2 is used for LUT C.<br>
														 index 3 is used for LUT D.
													 */
	UINT32						splbit[4];			/**< LUTCTL.SPLBIT <br>
														 index 0 is used for LUT A.<br>
														 index 1 is used for LUT B.<br>
														 index 2 is used for LUT C.<br>
														 index 3 is used for LUT D.
													 */
	T_IM_IIP_LUTRAM				lutram;				/**< Parameters of LUTRAM */
} T_IM_IIP_UTIL_LUT;

/** Parameter of IM_IIP_Utility_AlphaBlend */
typedef struct {
	E_IM_IIP_UNIT_ID			ld_unitid[2];		/**< UNITID of Loader */
	E_IM_IIP_UNIT_ID			blend_unitid;		/**< UNITID of Alpha */
	E_IM_IIP_UNIT_ID			sl_unitid;			/**< UNITID of SL */
	E_IM_IIP_PIXID				src_pixid[2];		/**< Source image PIXID */
	E_IM_IIP_PIXID				dst_pixid;			/**< Destination image PIXID */
	T_IM_IIP_UTIL_IMG_INT		src[2];				/**< Source image info. */
	T_IM_IIP_UTIL_IMG_INT		dst;				/**< Destination image info. */
	E_IM_IIP_PARAM_CSEL			ld_cache_select[2];	/**< Loader cache select. */
	UINT32						pix_depth;			/**< PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
													 <ul>
														 <li>@ref D_IM_IIP_PDEPTH_8BITS	 [PIXFMTTBL.PDEPTH[2:0] = 000b]
														 <li>@ref D_IM_IIP_PDEPTH_12BITS [PIXFMTTBL.PDEPTH[2:0] = 001b]
														 <li>@ref D_IM_IIP_PDEPTH_16BITS [PIXFMTTBL.PDEPTH[2:0] = 010b]
													 </ul> */
	E_IM_IIP_ADEPTH				alpha_depth;		/**< PIXFMTTBL.ADEPTH[2:0] <br><br> Depth of alpha.
													 <ul>
														 <li>@ref E_IM_IIP_ADEPTH_8BITS		[PIXFMTTBL.ADEPTH[2:0] = 000b]
														 <li>@ref E_IM_IIP_ADEPTH_4BITS		[PIXFMTTBL.ADEPTH[2:0] = 011b]
													 </ul> */
	UINT32						alpha_subsampling;	/**< PIXFMTTBL.ASUBSPL[1:0] <br><br> Control Alpha subsampling.
													 <ul>
														 <li>@ref D_IM_IIP_ALPHA_NO_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 00b]
														 <li>@ref D_IM_IIP_ALPHA_H_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 01b]
														 <li>@ref D_IM_IIP_ALPHA_HV_SUBSAMP	[PIXFMTTBL.ASUBSPL[1:0] = 10b]
													 </ul> */

	T_IM_IIP_UTIL_ALPHA			alpha;				/**< Parameter of Alpha blend */
} T_IM_IIP_UTIL_ALPHABLEND;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/**
   @weakgroup im_iip
   @{
*/
/**
   @name Utility Fucntions
   @{
*/


/**
   Resize and Rotation.<br>

   \<Unit configuration of IIP\><BR>
   @image html im_iip_afn.png

   \<Source and Destination image\><BR>
   @image html im_iip_afn_coord.png

   <ul>
		<li>B2Y Link OFF.
		<li>This API returns when IIP process is completed.
		<li>If cfg1 is set to NULL, AFN and SL don't run.
		<li>AFN unit can not transfer alpha data.
   </ul>

   @param[in]	cfg0					: Parameters of AFN and SL. Should not be NULL.
   @param[in]	cfg1					: Parameters of AFN and SL. If it is NULL, AFN and SL is not used.
   @param[in]	param_master_IF			: Master I/F of parameter buffer.
										  <ul>
											  <li>@ref D_IM_IIP_MASTER_IF0
											  <li>@ref D_IM_IIP_MASTER_IF1
										  </ul>
   @param[in]	param_buffer_addr		: Beginning address of parameter buffer. Shoud not be NULL.<br>
										  If cfg0 is used only, the size of buffer is @ref D_IM_IIP_UTIL_RR_BUF_BYTES bytes.<br>
										  If both cfg0 and cfg1 are used, the size is @ref D_IM_IIP_UTIL_RR_BUF_BYTES x 2 bytes.
   @param[in]	wait_time				: Wait time [msec]. The valid range is -1, 0, ... .
										  If this parameter is set to -1,
										  API waits permanently unless system-call sets event-flag.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System call error.
   @retval		D_IM_IIP_ERR				: Other error.
   @remarks		For supported image fomat, please refer to T_IM_IIP_UTIL_IMG_GBL::frame_type and T_IM_IIP_UTIL_IMG_GBL::pix_format.

*/
extern INT32 IM_IIP_Utility_ResizeRotate( const T_IM_IIP_UTIL_RR* const cfg0,
										  const T_IM_IIP_UTIL_RR* const cfg1,
										  const UCHAR param_master_IF,
										  const ULONG param_buffer_addr,
										  const INT32 wait_time );

/**
   Color space conversion.<br>

   \<Unit configuration of IIP\><BR>
   @image html im_iip_csc.png

   <ul>
		<li>B2Y Link OFF.
		<li>This API returns when IIP process is completed.
		<li>If cfg1 is set to NULL, Loader, CSC and SL don't run.
   </ul>

   @param[in]	cfg0					: Parameters of Loader, CSC and SL. Should not be NULL.
   @param[in]	cfg1					: Parameters of Loader, CSC and SL. If it is NULL, Loader, CSC and SL is not used.
   @param[in]	param_master_IF			: Master I/F of parameter buffer.
										  <ul>
											  <li>@ref D_IM_IIP_MASTER_IF0
											  <li>@ref D_IM_IIP_MASTER_IF1
										  </ul>
   @param[in]	param_buffer_addr		: Beginning address of parameter buffer. Shoud not be NULL.<br>
										  If cfg0 is used only, the size of buffer is @ref D_IM_IIP_UTIL_CSC_BUF_BYTES bytes.<br>
										  If both cfg0 and cfg1 are used, the size is @ref D_IM_IIP_UTIL_CSC_BUF_BYTES x 2 bytes.
   @param[in]	wait_time				: Wait time [msec]. The valid range is -1, 0, ... .
										  If this parameter is set to -1,
										  API waits permanently unless system-call sets event-flag.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System call error.
   @retval		D_IM_IIP_ERR				: Other error.
   @remarks		For supported image fomat, please refer to T_IM_IIP_UTIL_IMG_GBL::frame_type and T_IM_IIP_UTIL_IMG_GBL::pix_format.
*/
extern INT32 IM_IIP_Utility_CSC( const T_IM_IIP_UTIL_CSC* const cfg0,
								 const T_IM_IIP_UTIL_CSC* const cfg1,
								 const UCHAR param_master_IF,
								 const ULONG param_buffer_addr,
								 const INT32 wait_time );

/**
   LUT<br>

   \<Unit configuration of IIP\><BR>
   @image html im_iip_lut.png

   <ul>
		<li>B2Y Link OFF.
		<li>This API returns when IIP process is completed.
		<li>PORTEN bit (LUTUNITCTL) is set to 0x1. For more detailed information, please refer to the IIP Hardware specification.
   </ul>

   @param[in]	cfg						: Parameters of Loader, LUT and SL. Should not be NULL.
   @param[in]	param_master_IF			: Master I/F of parameter buffer.
										  <ul>
											  <li>@ref D_IM_IIP_MASTER_IF0
											  <li>@ref D_IM_IIP_MASTER_IF1
										  </ul>
   @param[in]	param_buffer_addr		: Beginning address of parameter buffer. Shoud not be NULL.<br>
										  The size of buffer is @ref D_IM_IIP_UTIL_LUT_BUF_BYTES bytes.
   @param[in]	wait_time				: Wait time [msec]. The valid range is -1, 0, ... .
										  If this parameter is set to -1,
										  API waits permanently unless system-call sets event-flag.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System call error.
   @retval		D_IM_IIP_ERR				: Other error.
   @remarks		For supported image fomat, please refer to T_IM_IIP_UTIL_IMG_GBL::frame_type and T_IM_IIP_UTIL_IMG_GBL::pix_format.
*/
extern INT32 IM_IIP_Utility_LUT( const T_IM_IIP_UTIL_LUT* const cfg,
								 const UCHAR param_master_IF,
								 const ULONG param_buffer_addr,
								 const INT32 wait_time );

/**
   Alpha Blend<br>

   \<Unit configuration of IIP\><BR>
   @image html im_iip_alpha.png

   <ul>
		<li>B2Y Link OFF.
		<li>This API returns when IIP process is completed.
   </ul>

   @param[in]	cfg						: Parameters of Loader, Alpha and SL. Should not be NULL.
   @param[in]	param_master_IF			: Master I/F of parameter buffer.
										  <ul>
											  <li>@ref D_IM_IIP_MASTER_IF0
											  <li>@ref D_IM_IIP_MASTER_IF1
										  </ul>
   @param[in]	param_buffer_addr		: Beginning address of parameter buffer. Shoud not be NULL.<br>
										  The size of buffer is @ref D_IM_IIP_UTIL_ALPHABLEND_BUF_BYTES bytes.
   @param[in]	wait_time				: Wait time [msec]. The valid range is -1, 0, ... .
										  If this parameter is set to -1,
										  API waits permanently unless system-call sets event-flag.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System call error.
   @retval		D_IM_IIP_ERR				: Other error.
   @remarks		For supported image fomat, please refer to T_IM_IIP_UTIL_IMG_GBL::frame_type and T_IM_IIP_UTIL_IMG_GBL::pix_format.
*/
extern INT32 IM_IIP_Utility_AlphaBlend( const T_IM_IIP_UTIL_ALPHABLEND* cfg,
										const UCHAR param_master_IF,
										const ULONG param_buffer_addr,
										const INT32 wait_time );


/** @} */	// end of name
/** @} */	// end of im_iip

/**
   @weakgroup im_iip
   @{
   @defgroup im_iip_param IIP SDRAM Parameters
*/
/** @} */	// end of im_iip group

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// CO_IM_IIP_ENABLE_UTILITY

#endif	// _IM_IIP_H_

