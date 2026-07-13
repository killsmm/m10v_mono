/**
 * @file		still_iip.c
 * @brief		IIP control functions
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// FJ header
#include "fj_host.h"

// Still header
#include "still_iip.h"
#include "still_dcache.h"
#include "still_common.h"

// IQ Header
// delete_basefw\fj\iq
//#include "iq_frect_calculate.h"
//#include "iq_frect_tbl.h"

// Map Header
#include "sdram_map_common.h"
#include "sdram_map_cache_define.h"

// Utility Header
#include "debug.h"
#include "mba_if.h"

// OS Header
#include "os_user_custom.h"

// Driver Header
#include "dd_arm.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

// Enable debug log
//#define CO_BF_STILL_IIP_DBG_DUMP

#define D_BF_STILL_IIP_IMG_ADDR_ALIGN			(8)
#ifdef DEBUG_ON_PC
#define D_BF_STILL_IIP_INT_FACTOR_ERR			(0)
#else
#define D_BF_STILL_IIP_INT_FACTOR_ERR			(D_IM_IIP_INT_FACTOR_AXIERR )
#endif

#define D_BF_STILL_IIP_GPC_STEPNUM_BLEND			(10)

#define D_BF_STILL_IIP_TID_DISABLE				(0xFFFF)


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

// GPC Unit Parameter (Blend)
typedef struct{
	T_IM_IIP_PARAM_GPC_SEQ_CTRL			seq;
	T_IM_IIP_PARAM_GPC_PRECAL			precal_p0_d0_d1;			// Input port0 D0-D1
	T_IM_IIP_PARAM_GPC_PRECAL			precal_p0_d2_d3;			// Input port0 D2-D3
	T_IM_IIP_PARAM_GPC_PRECAL			precal_p1_d0_d1;			// Input port1 D0-D1
	T_IM_IIP_PARAM_GPC_PRECAL			precal_p1_d2_d3;			// Input port1 D2-D3
	T_IM_IIP_PARAM_GPC_CALC				calc;						// D0~D2
	T_IM_IIP_PARAM_GPC_INTER_COMP_CALC	inter_comp_calc;
} T_BF_STILL_IIP_GPC_BLEND_CMD;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

static const T_IM_IIP_PARAM_AFN gbf_still_iip_param_afn_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 29,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
	.AFNTOPCNF.bit.DATACONF = 0,
//	.AFNTOPCNF.bit.WAITCONF,							// set later
	.AFNLDCNF.bit.DTHD = E_IM_IIP_PARAM_DTHD_NORMAL,
	.AFNLDCNF.bit.CBC = E_IM_IIP_PARAM_CBC_DISABLE,
	.AFNCTL.bit.HSMD = E_IM_IIP_PARAM_HSMD_HIGH,
	.AFNCTL.bit.LPLVL = E_IM_IIP_PARAM_LPLVL_H_LEVEL,
	.AFNCTL.bit.HDBL = E_IM_IIP_PARAM_HDBL_SAME_AS_PICHSZ,
	.AFNCTL.bit.VDBL = E_IM_IIP_PARAM_VDBL_SAME_AS_PICVSZ,
	.AFNCTL.bit.GHSZ = E_IM_IIP_PARAM_GHSZ_64PIX,		// Needs tuning
	.AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_8PIX,		// Needs tuning
	.PFCTL.bit.PFEXTMC = 4,
	.PFCTL.bit.PFEXTADC = 64,							// Needs tuning
	.PFCTL.bit.PFSP1 = E_IM_IIP_PARAM_PFSP_2,			// Needs tuning
	.PFCTL.bit.PFSP2 = E_IM_IIP_PARAM_PFSP_16,			// Needs tuning
	.PFCTL.bit.PFVSP = E_IM_IIP_PARAM_PFVSP_VERTICAL_0,	// Needs tuning
	.PFCTL.bit.PFOFF = E_IM_IIP_PARAM_PFOFF_RUN,
	.PIXIDEF.bit.EXA = E_IM_IIP_PARAM_EXA_ALPHA_TO_NEXT_UNIT,
	.PIXIDEF.bit.CSEL = E_IM_IIP_PARAM_CSEL_0,			// ImsgCache
//	.PIXIDEF.bit.IPIXID,								// set later
	.OUTSIZE.bit.OUTHSZ = 0,
	.OUTSIZE.bit.OUTVSZ = 0,
	.OUTSTARTPOS.bit.STARTX = 0,
	.OUTSTARTPOS.bit.STARTY = 0,
	.CALMETHOD.bit.OPCOL = 0,
	.CALMETHOD.bit.ITMD = E_IM_IIP_PARAM_ITMD_BI_LINEAR,
	.CALMETHOD.bit.ARA = E_IM_IIP_PARAM_ARA_ITMD,
	.CALMETHOD.bit.FILMD = 0,
	.CALMETHOD.bit.DANTI = 1,
	.CALMETHOD.bit.AANTI = 1,
	.CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_AFN0,
	.CALMETHOD.bit.SCUB = E_IM_IIP_PARAM_SCUB_0,
	.FILVAL0.bit.FILVALYG = 0x0,
	.FILVAL0.bit.FILVALB = 0x0,
	.FILVAL1.bit.FILVALR = 0x0,
	.FILVAL1.bit.FILVALA = 0x0,
	.OPMD.bit.OPMD0 = E_IM_IIP_PARAM_OPMD_NON,
	.OPY.bit.OPBY = 0,
	.OPY.bit.OPAY = 0,
	.OPB.bit.OPBB = 0,
	.OPB.bit.OPAB = 0,
	.OPR.bit.OPBR = 0,
	.OPR.bit.OPAR = 0,
	.OPA.bit.OPBA = 0,
	.OPA.bit.OPAA = 0,
	.CLIPLVLY.bit.CLIPLVLYH = 0xFFFF,
	.CLIPLVLY.bit.CLIPLVLYL = 0,
	.CLIPLVLB.bit.CLIPLVLBH = 0xFFFF,
	.CLIPLVLB.bit.CLIPLVLBL = 0,
	.CLIPLVLR.bit.CLIPLVLRH = 0xFFFF,
	.CLIPLVLR.bit.CLIPLVLRL = 0,
	.CLIPLVLA.bit.CLIPLVLAH = 0xFFFF,
	.CLIPLVLA.bit.CLIPLVLAL = 0,
	.AFNPCNT.bit.AFNPHCNT = 1,
	.AFNPCNT.bit.AFNPVCNT = 1,
//	.AFNPA.word = (ULONG)afntbl,						// set later
	.UPLK_TARGET.bit.UPSU0 = E_IM_IIP_PARAM_UPSU_NONE,
	.UPLK_TARGET.bit.UPSU1 = E_IM_IIP_PARAM_UPSU_NONE,
	.UPLK_TARGET.bit.UPSU2 = E_IM_IIP_PARAM_UPSU_NONE,
	.DWLK_TARGET.bit.DWSU0 = E_IM_IIP_PARAM_DWSU_NONE,
	.DWLK_TARGET.bit.DWSU1 = E_IM_IIP_PARAM_DWSU_NONE,
	.DWLK_TARGET.bit.DWSU2 = E_IM_IIP_PARAM_DWSU_NONE,
	.LKNUM.bit.UPNUM = E_IM_IIP_PARAM_UPNUM_NONE,
	.LKNUM.bit.DWNUM = E_IM_IIP_PARAM_DWNUM_NONE,
	.RING.bit.URINGSIZE = 0,
	.RING.bit.UMARGINIYSZ = 0,
	.RING.bit.DRINGSIZE = 0,
};
static const T_IM_IIP_PARAM_FRECT gbf_still_iip_param_frect_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 29,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
	.FRTOPCNF.bit.DATACONF = 0,
//	.FRTOPCNF.bit.WAITCONF,							// set later
	.FRLDCNF.bit.DTHD = E_IM_IIP_PARAM_DTHD_NORMAL,
	.FRCTL.bit.HSMD = E_IM_IIP_PARAM_HSMD_HIGH,
	.FRCTL.bit.LPLVL = E_IM_IIP_PARAM_LPLVL_H_LEVEL,
	.FRCTL.bit.HDBL = E_IM_IIP_PARAM_HDBL_SAME_AS_PICHSZ,
	.FRCTL.bit.VDBL = E_IM_IIP_PARAM_VDBL_SAME_AS_PICVSZ,
//	.FRCTL.bit.GHSZ = E_IM_IIP_PARAM_GHSZ_64PIX,		// set later
//	.FRCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_8PIX,			// set later
	.PFCTL.bit.PFEXTMC = 4,
	.PFCTL.bit.PFEXTADC = 16,							// Needs tuning
	.PFCTL.bit.PFSP1 = E_IM_IIP_PARAM_PFSP_1,			// set later
	.PFCTL.bit.PFSP2 = E_IM_IIP_PARAM_PFSP_128,			// set later
//	.PFCTL.bit.PFSP1 = E_IM_IIP_PARAM_PFSP_16,			// set later
//	.PFCTL.bit.PFSP2 = E_IM_IIP_PARAM_PFSP_16,			// set later
	.PFCTL.bit.PFVSP = E_IM_IIP_PARAM_PFVSP_VERTICAL_0,	// Needs tuning
	.PFCTL.bit.PFOFF = E_IM_IIP_PARAM_PFOFF_RUN,
	.PIXIDEF.bit.EXA = E_IM_IIP_PARAM_EXA_NO_ALPHA_TO_NEXT_UNIT,
	.PIXIDEF.bit.CSEL = E_IM_IIP_PARAM_CSEL_0,			// ImsgCache
//	.PIXIDEF.bit.IPIXID,								// set later
	.OUTSIZE.bit.OUTHSZ = 0,
	.OUTSIZE.bit.OUTVSZ = 0,
	.OUTSTARTPOS.bit.STARTX = 0,
	.OUTSTARTPOS.bit.STARTY = 0,
	.CALMETHOD.bit.OPCOL = 0,
	.CALMETHOD.bit.ITMD = E_IM_IIP_PARAM_ITMD_BI_LINEAR,
	.CALMETHOD.bit.ARA = E_IM_IIP_PARAM_ARA_ITMD,
	.CALMETHOD.bit.FILMD = 0,
	.CALMETHOD.bit.DANTI = 1,
	.CALMETHOD.bit.AANTI = 1,
	.CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_FRECT0,
	.CALMETHOD.bit.SCUB = E_IM_IIP_PARAM_SCUB_0,
	.FILVAL0.bit.FILVALYG = 0x0,
	.FILVAL0.bit.FILVALB = 0x0,
	.FILVAL1.bit.FILVALR = 0x0,
	.FILVAL1.bit.FILVALA = 0x0,
	.OPMD.bit.OPMD0 = E_IM_IIP_PARAM_OPMD_NON,
	.OPY.bit.OPBY = 0,
	.OPY.bit.OPAY = 0,
	.OPB.bit.OPBB = 0,
	.OPB.bit.OPAB = 0,
	.OPR.bit.OPBR = 0,
	.OPR.bit.OPAR = 0,
	.OPA.bit.OPBA = 0,
	.OPA.bit.OPAA = 0,
	.CLIPLVLY.bit.CLIPLVLYH = 0xFFFF,
	.CLIPLVLY.bit.CLIPLVLYL = 0,
	.CLIPLVLB.bit.CLIPLVLBH = 0xFFFF,
	.CLIPLVLB.bit.CLIPLVLBL = 0,
	.CLIPLVLR.bit.CLIPLVLRH = 0xFFFF,
	.CLIPLVLR.bit.CLIPLVLRL = 0,
	.CLIPLVLA.bit.CLIPLVLAH = 0xFFFF,
	.CLIPLVLA.bit.CLIPLVLAL = 0,
//	.FRPCNT.bit.FRPHCNT = 1,							// set later
//	.FRPCNT.bit.FRPVCNT = 1,							// set later
//	.PADRS.word = (ULONG)frecttbl,						// set later
	.UPLK_TARGET.bit.UPSU0 = E_IM_IIP_PARAM_UPSU_NONE,
	.UPLK_TARGET.bit.UPSU1 = E_IM_IIP_PARAM_UPSU_NONE,
	.UPLK_TARGET.bit.UPSU2 = E_IM_IIP_PARAM_UPSU_NONE,
	.DWLK_TARGET.bit.DWSU0 = E_IM_IIP_PARAM_DWSU_NONE,
	.DWLK_TARGET.bit.DWSU1 = E_IM_IIP_PARAM_DWSU_NONE,
	.DWLK_TARGET.bit.DWSU2 = E_IM_IIP_PARAM_DWSU_NONE,
	.LKNUM.bit.UPNUM = E_IM_IIP_PARAM_UPNUM_NONE,
	.LKNUM.bit.DWNUM = E_IM_IIP_PARAM_DWNUM_NONE,
	.RING.bit.URINGSIZE = 0,
	.RING.bit.UMARGINIYSZ = 0,
	.RING.bit.DRINGSIZE = 0,
};
static const T_IM_IIP_PARAM_STS_FILL gbf_still_iip_param_sts_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 26,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY,
	.BASE.SL_TOPCNF0.bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE,
//	.BASE.SL_TOPCNF0.bit.DATACONF,		// set later
//	.BASE.PIXIDDEF.bit.OPIXID,			// set later
	.BASE.PIXIDDEF.bit.CSEL = E_IM_IIP_PARAM_CSEL_0,	// dummy
	.BASE.PIXIDDEF.bit.DIV1ST = E_IM_IIP_PARAM_DIV1ST_1ST,
	.BASE.PIXIDDEF.bit.LKDIV = 0,

	.BASE.PLDUNIT_SL.bit.PLDUNIT_SL_LO = (ULONG)(D_IM_IIP_PARAM_PLDUNIT_NONE & 0xFFFFFFFFULL),
	.BASE.PLDUNIT_SL.bit.PLDUNIT_SL_HI = (ULONG)((D_IM_IIP_PARAM_PLDUNIT_NONE & 0xFFFFFFFF00000000ULL) >> 32),

	.BASE.SL_CTL.bit.SL_MODE = 0,
	.BASE.SL_CTL.bit.LOADMD = 0,
	.BASE.SL_CTL.bit.MASKEN = E_IM_IIP_PARAM_MASKEN_NO_MASK,

	.BASE.OUTCOL.bit.OUTCOL = 0,

	.BASE.SL_HSIZE.bit.HSTART = 0,
//	.BASE.SL_HSIZE.bit.HEND = ,		// set later
	.BASE.SL_VSIZE.bit.VSTART = 0,
//	.BASE.SL_VSIZE.bit.VEND = ,		// set later
	.BASE.SL_PHSZ.bit.PHSZ1 =  64,
	.BASE.SL_PVSZ.bit.PVSZ1 =  8,
	.BASE.SL_PHSZ.bit.PHSZ0 = 64,
	.BASE.SL_PVSZ.bit.PVSZ0 = 8,

	.BASE.SEL_OPARA.bit.SEL_OPARA = 0,

	.BASE.OFSET_0.bit.OFSET = 0,
	.BASE.CLPTH_U_0.bit.CLPTH_U = 0xFF,
	.BASE.CLPTH_L_0.bit.CLPTH_L = 0,
	.BASE.CLPVAL_U_0.bit.CLPVAL_U = 0xFF,
	.BASE.CLPVAL_L_0.bit.CLPVAL_L = 0,
//	.BASE.SFT_0.bit.SFTVAL = 0,		// set later
	.BASE.SFT_0.bit.SFTM = E_IM_IIP_PARAM_SFTM_ROUNDDOWN,
	.BASE.W14MD_0.bit.W14LSFT = 0,
	.BASE.W14MD_0.bit.W14SAT = 0,

	.header1.bit.BeginningAddress = 0x0200,
	.header1.bit.WordLength = 3,
	.header1.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY,
	.FILL_EN.bit.FILL_EN0 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN1 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN2 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN3 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN4 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN5 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN6 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN7 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN8 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN9 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN10 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN11 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN12 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN13 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN14 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN15 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN16 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN17 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN18 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_EN.bit.FILL_EN19 = E_IM_IIP_PARAM_FILL_EN_DISABLE,
	.FILL_DATA.bit.FILL_D0 = 0,
	.FILL_DATA.bit.FILL_D1 = 128,
	.FILL_DATA.bit.FILL_D2 = 128,

	.header2.bit.BeginningAddress = 0x0300,
	.header2.bit.WordLength = 41,
	.header2.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
	.FILL_SIZE = {
		[0] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[1] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[2] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[3] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[4] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[5] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[6] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[7] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[8] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[9] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[10] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[11] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[12] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[13] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[14] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[15] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[16] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[17] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[18] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
		[19] = { .bit = { .FILL_HSTA = 0, .FILL_HEND = 0, .FILL_VSTA = 0, .FILL_VEND = 0, } },
	},
};

static const T_IM_IIP_UTIL_RR g_still_iip_param_util_rr_base = {
//		.afn_unitid = D_BF_STILL_RESCONV_AFN_UNITID,	// UNITID of AFN
//		.sl_unitid = D_BF_STILL_RESCONV_SL_UNITID,		// UNITID of SL
//		.src_pixid = D_BF_STILL_RESCONV_SRC_PIXID_MASK,	// Source image PIXID
//		.dst_pixid = D_BF_STILL_RESCONV_DST_PIXID_MASK,	// Destination image PIXID
		.bi_cubic_unitid = E_IM_IIP_PARAM_SCUB_0,		// UNITID of BI-Cubic
		.src = {								// Source image info.
//			.rect = {								// Rectangle info. (Source image area)
//				.top = 0.0,								// Vertical position of top-left pixel
//				.left = 0.0,							// Horizontal position of top-left pixel
//				.width = 1280,							// Width
//				.lines = 720,							// Lines
//			},
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
//				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
//				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
//				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
//				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
//			.rect = {								// Rectangle info. (Source image area)
//				.top = 0.0,								// Vertical position of top-left pixel
//				.left = 0.0,							// Horizontal position of top-left pixel
//				.width = 640,							// Width
//				.lines = 360,							// Lines
//			},
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
//				.masterIF_Y_G = D_BF_STILL_RESCONV_DST_MASTER_IF,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
//				.masterIF_Cb_B = D_BF_STILL_RESCONV_DST_MASTER_IF,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
//				.masterIF_Cr_R = D_BF_STILL_RESCONV_DST_MASTER_IF,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
//				.masterIF_Alpha = D_BF_STILL_RESCONV_DST_MASTER_IF,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
//		.afn_cache_select = D_BF_STILL_RESCONV_AFN_CSEL,	// AFN cache select.
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,					// PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
		.rotation = E_IM_IIP_UTIL_ROTATE_000,				// Angle of rotation
		.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR,	// Resize mode
	};
static const T_IM_IIP_PARAM_CSC g_still_iip_param_csc_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 25,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
//	.CSCTOPCNF.bit.WAITCONF = ,		// set later
//	.CSCTOPCNF.bit.DATACONF = ,		// set later
	.CSCK = {
		[0] = {	// Y/G
			.bit.CSCK_0 = (LONG)(1.0F * 0x1000),	// Y/G
			.bit.CSCK_1 = 0,	// Cb/B
			.bit.CSCK_2 = 0,	// Cr/R
			.bit.CSCK_3 = 0,	// A
		},
		[1] = {	// Cb/B
			.bit.CSCK_0 = 0,	// Y/G
			.bit.CSCK_1 = (LONG)(1.0F * 0x1000),	// Cb/B
			.bit.CSCK_2 = 0,	// Cr/R
			.bit.CSCK_3 = 0,	// A
		},
		[2] = {	// Cr/R
			.bit.CSCK_0 = 0,	// Y/G
			.bit.CSCK_1 = 0,	// Cb/B
			.bit.CSCK_2 = (LONG)(1.0F * 0x1000),	// Cr/R
			.bit.CSCK_3 = 0,	// A
		},
		[3] = {	// A
			.bit.CSCK_0 = 0,	// Y/G
			.bit.CSCK_1 = 0,	// Cb/B
			.bit.CSCK_2 = 0,	// Cr/R
			.bit.CSCK_3 = 0,	// A
		},
	},
	.MDSEL.bit.MDSEL = 1,		// SXX.XXXX_XXXX_XXXX ==> 1.0=0x1000
	.ALPSEL.bit.ALPSEL = E_IM_IIP_PARAM_ALPSEL_DIRECT,
	.ALPSEL.bit.ALOSEL = E_IM_IIP_PARAM_ALOSEL_CALCULATED,
	.ALPVAL.bit.ALPVAL = 0,
	.MAXMIN.bit = {
		.MAXMIN0 = E_IM_IIP_PARAM_MAXMIN_THROUGH,
		.MAXMIN1 = E_IM_IIP_PARAM_MAXMIN_THROUGH,
		.MAXMIN2 = E_IM_IIP_PARAM_MAXMIN_THROUGH,
		.MAXMIN3 = E_IM_IIP_PARAM_MAXMIN_THROUGH,
	},
	.OUTCLIP.bit = {
		.CLPMAX0 = 0x3FFF,
		.CLPMAX1 = 0x3FFF,
		.CLPMAX2 = 0x3FFF,
		.CLPMAX3 = 0xFF,
		.CLPMIN0 = -0x4000,
		.CLPMIN1 = -0x4000,
		.CLPMIN2 = -0x4000,
		.CLPMIN3 = -0x100,
	},
};

static const T_IM_IIP_PARAM_1DL gbf_still_iip_param_ld_base = {
	.header0.bit.BeginningAddress = 0x0004,
	.header0.bit.WordLength = 16,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
//	.LD_TOPCNF0.bit.WAITCONF =						// set later
	.LD_TOPCNF1.bit.LINK_UID = E_IM_IIP_PARAM_PORTID_NONE,
//	.PIXIDDEF.bit.IPIXID =							// set later
	.LD_CTL.bit.LD_MODE = E_IM_IIP_PARAM_LD_MODE_NORMAL_LOAD,
	.LD_CTL.bit.LD_OUT = 0,
	.LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC,
	.LD_CTL.bit.SAVERD = 0,
	.LD_CTL.bit.CSEL = E_IM_IIP_PARAM_CSEL_0,		// Cache 0
	.LD_PREFETCH.bit.PFOFF = E_IM_IIP_PARAM_PFOFF_RUN,
	.LD_PREFETCH.bit.PF_PDIST = E_IM_IIP_PARAM_PF_PDIST_8PIX,
	.LD_PREFETCH.bit.PF_1ST = E_IM_IIP_PARAM_PF_1ST_ACCESS_BEFORE_PF,
	.LD_PREFETCH.bit.PFB_NUM = 0,
	.LD_PREFETCH.bit.PF_ADD = 0,
//	.LD_HSIZE.bit.HSTART =							// set later
//	.LD_HSIZE.bit.HEND =							// set later
//	.LD_VSIZE.bit.VSTART =							// set later
//	.LD_VSIZE.bit.VEND =							// set later
//	.LD_PHSZ.bit.PHSZ0 =							// set later
//	.LD_PHSZ.bit.PHSZ1 =							// set later
//	.LD_PVSZ.bit.PVSZ0 =							// set later
//	.LD_PVSZ.bit.PVSZ1 =							// set later
	.UPLK_TARGET.bit.UPSU0 = E_IM_IIP_PARAM_UPSU_NONE,
	.UPLK_TARGET.bit.UPSU1 = E_IM_IIP_PARAM_UPSU_NONE,
	.UPLK_TARGET.bit.UPSU2 = 0,
	.DWLK_TARGET.bit.DWSU0 = E_IM_IIP_PARAM_DWSU_NONE,
	.DWLK_TARGET.bit.DWSU1 = E_IM_IIP_PARAM_DWSU_NONE,
	.DWLK_TARGET.bit.DWSU2 = 0,
	.LKNUM.bit.UPNUM = E_IM_IIP_PARAM_UPNUM_NONE,
	.LKNUM.bit.DWNUM = E_IM_IIP_PARAM_DWNUM_NONE,
	.RING.bit.URINGSIZE = 0,
	.RING.bit.UMARGINIYSZ = 0,
	.RING.bit.DRINGSIZE = 0,
};

static const T_IM_IIP_PARAM_BLEND gbf_still_iip_param_bld_base =
{
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 4,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY,
//	.BLENDTOPCNF0.bit.WAITCONF_0 =			// set later
//	.BLENDTOPCNF0.bit.DATACONF_0 =			// set later
//	.BLENDTOPCNF1.bit.WAITCONF_1 =			// set later
//	.BLENDTOPCNF1.bit.DATACONF_1 =			// set later
//	.BLENDTOPCNF2.bit.DATACONF_2 =			// set later
	.header1.bit.BeginningAddress = 0x0080,
	.header1.bit.WordLength = 24,
	.header1.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY,
	.BLENDCTL.bit.BLD_MD = E_IM_IIP_PARAM_BLD_MD_ALPHA_BLEND,		// alphablend
	.BLENDCTL.bit.CLP_MD = E_IM_IIP_PARAM_CLP_MD_NO_CLIP,
	.BLENDCTL.bit.ALPHA_OUT = E_IM_IIP_PARAM_ALPHA_OUT_NONE,
	.ALPHACTL0.bit.ALPHA_STEP = E_IM_IIP_PARAM_ALPHA_STEP_8BITS,
	.ALPHACTL0.bit.ALPHA_SEL = E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL,
	.ALPHACTL1.bit.ALPHA_VAL0 = 0,			// set later
	.ALPHACTL1.bit.ALPHA_VAL1 = 0,			// set later
	.ALPHACTL2.bit.ALPHA_VAL2 = 0,			// set later
	.ALPHACTL2.bit.ALPHA_VAL3 = 0,
	.WADDCTL[0].bit.WADD_KA = 0,
	.WADDCTL[0].bit.WADD_KB = 0,
	.WADDCTL[1].bit.WADD_KA = 0,
	.WADDCTL[1].bit.WADD_KB = 0,
	.WADDCTL[2].bit.WADD_KA = 0,
	.WADDCTL[2].bit.WADD_KB = 0,
	.WADDCTL[3].bit.WADD_KA = 0,
	.WADDCTL[3].bit.WADD_KB = 0,
	.DIVCTL0.bit.RSFT = 0xf,
	.MSKCTL0.bit.MSKSEL = 0,
	.MSKCTL0.bit.DINSEL = 0,
	.MULCTL[0].bit.ADDVAL_A = 0,
	.MULCTL[0].bit.ADDVAL_B = 0,
	.MULCTL[0].bit.ADDVAL_C = 0,
	.MULCTL[0].bit.SFTSEL = E_IM_IIP_PARAM_SFTSEL_NONE,
	.MULCTL[1].bit.ADDVAL_A = 0,
	.MULCTL[1].bit.ADDVAL_B = 0,
	.MULCTL[1].bit.ADDVAL_C = 0,
	.MULCTL[1].bit.SFTSEL = E_IM_IIP_PARAM_SFTSEL_NONE,
	.MULCTL[2].bit.ADDVAL_A = 0,
	.MULCTL[2].bit.ADDVAL_B = 0,
	.MULCTL[2].bit.ADDVAL_C = 0,
	.MULCTL[2].bit.SFTSEL = E_IM_IIP_PARAM_SFTSEL_NONE,
	.MULCTL[3].bit.ADDVAL_A = 0,
	.MULCTL[3].bit.ADDVAL_B = 0,
	.MULCTL[3].bit.ADDVAL_C = 0,
	.MULCTL[3].bit.SFTSEL = E_IM_IIP_PARAM_SFTSEL_NONE,
};

static const T_IM_IIP_PARAM_GPC gbf_still_iip_param_gpc_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 7,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
//	.GPCTOPCNF[0].bit.WAITCONF =					// set later
//	.GPCTOPCNF[0].bit.DATACONF =					// set later
//	.GPCTOPCNF[1].bit.WAITCONF =					// set later
//	.GPCTOPCNF[1].bit.DATACONF =					// set later
	.GPCCONTROL.bit.COMMONPARA = E_IM_IIP_PARAM_COMMONPARA_D0_D0_D0,
	.GPCCONTROL.bit.MD2DLK = 0,
	.GPCCONTROL.bit.ALPEN0 = 0,
	.GPCCONTROL.bit.ALPEN1 = 0,
	.GPCCONTROL.bit.ATRB0 = E_IM_IIP_PARAM_ATRB_PORT0,
	.GPCCONTROL.bit.ATRB1 = E_IM_IIP_PARAM_ATRB_PORT0,
//	.GPCCLOADADR.bit.CLOADADR =						// set later
};

volatile UINT32 g_still_iip_afn_unit_dump_tid = D_BF_STILL_IIP_TID_DISABLE;
volatile UINT32 g_still_iip_cbcr_afn_unit_dump_tid = D_BF_STILL_IIP_TID_DISABLE;
volatile UINT32 g_still_iip_gpc_unit_dump_tid = D_BF_STILL_IIP_TID_DISABLE;
volatile UINT32 g_still_iip_blend_unit_dump_tid = D_BF_STILL_IIP_TID_DISABLE;


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/

static VOID bf_still_iip_int_handler( ULONG interrupt_flag_mask )
{
	T_IM_IIP_AXI_STATUS sts;
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_AXIERR) != 0 ) {
		Im_IIP_Get_AXI_Status( &sts );
		BF_Debug_Print_Error(( "bf_still_iip_int_handler: AXI Error. AXRSTS=0x%lx AXWSTS=0x%lx\n",
					sts.read_channel_status,
					sts.write_channel_status
					));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL2_LINEEND) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_FRECT, FJ_MBALOG_TYPE_POINT, "IIP int SL2 start" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL3_LINEEND) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_FRECT, FJ_MBALOG_TYPE_POINT, "IIP int SL3 start" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL4_LINEEND) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_FRECT, FJ_MBALOG_TYPE_POINT, "IIP int SL4 start" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL5_LINEEND) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_FRECT, FJ_MBALOG_TYPE_POINT, "IIP int SL5 start" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL6_LINEEND) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_FRECT, FJ_MBALOG_TYPE_POINT, "IIP int SL6 start" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL0END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL0END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL1END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL1END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL2END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL2END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL3END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL3END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL4END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL4END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL5END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL5END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL6END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL6END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL7END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL7END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL8END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL8END, FJ_MBALOG_TYPE_POINT, "" ));
	}
}


static VOID bf_still_iip_check_res_combi_csel( const UCHAR src_master_if, const E_IM_IIP_PARAM_CSEL src_cache_sel, const CHAR* const funcname )
{
	static const CHAR wrnmsg[] = "%s: src cache and master_if mismatch. mif=%u csel=%u\n";

	if( (src_cache_sel == E_IM_IIP_PARAM_CSEL_0)
	 && (src_master_if == D_IM_IIP_MASTER_IF0) ) {
		// OK.
	} else
	if( (src_cache_sel == E_IM_IIP_PARAM_CSEL_1)
	 && (src_master_if == D_IM_IIP_MASTER_IF1) ) {
		// OK.
	} else
	if( (src_cache_sel == E_IM_IIP_PARAM_CSEL_2)
	 && (src_master_if == D_IM_IIP_MASTER_IF2) ) {
		// OK.
	}
	else {
		BF_Debug_Print_Warning(( wrnmsg, funcname, src_master_if, src_cache_sel ));
	}
}


static VOID bf_still_iip_check_res_combi_sl( const UCHAR dst_master_if, const E_IM_IIP_UNIT_ID dst_unitid, const CHAR* const funcname )
{
	static const CHAR wrnmsg[] = "%s: dst master_if and SL unit mismatch. mif=%u unit=%u\n";

	if( (dst_unitid == E_IM_IIP_UNIT_ID_SL0)
	||	(dst_unitid == E_IM_IIP_UNIT_ID_SL3)
	||	(dst_unitid == E_IM_IIP_UNIT_ID_SL6) ) {
		if( dst_master_if != D_IM_IIP_MASTER_IF0 ) {
			BF_Debug_Print_Warning(( wrnmsg, funcname, dst_master_if, dst_unitid ));
		}
	}
	if( (dst_unitid == E_IM_IIP_UNIT_ID_SL1)
	||	(dst_unitid == E_IM_IIP_UNIT_ID_SL4)
	||	(dst_unitid == E_IM_IIP_UNIT_ID_SL7) ) {
		if( dst_master_if != D_IM_IIP_MASTER_IF1 ) {
			BF_Debug_Print_Warning(( wrnmsg, funcname, dst_master_if, dst_unitid ));
		}
	}
	if( dst_unitid == E_IM_IIP_UNIT_ID_SL8 ) {
		if( dst_master_if != D_IM_IIP_MASTER_IF2 ) {
			BF_Debug_Print_Warning(( wrnmsg, funcname, dst_master_if, dst_unitid ));
		}
	}
	if( dst_unitid == E_IM_IIP_UNIT_ID_SL2 ) {
		if( (dst_master_if != D_IM_IIP_MASTER_IF1)
		 && (dst_master_if != D_IM_IIP_MASTER_IF2) ) {	/* pgr1222 */
			BF_Debug_Print_Warning(( wrnmsg, funcname, dst_master_if, dst_unitid ));
		}
	}
	if( dst_unitid == E_IM_IIP_UNIT_ID_SL5 ) {
		if( (dst_master_if != D_IM_IIP_MASTER_IF0)
		 && (dst_master_if != D_IM_IIP_MASTER_IF2) ) {
			BF_Debug_Print_Warning(( wrnmsg, funcname, dst_master_if, dst_unitid ));
		}
	}
}


static VOID bf_still_iip_check_res_combi_rr( const T_BF_STILL_IIP_RESOURCE_RR* const res_rr )
{
	const CHAR funcname[] = "bf_still_iip_check_res_combi_rr";

	bf_still_iip_check_res_combi_csel( res_rr->src_master_if, res_rr->src_cache_sel, funcname );

	bf_still_iip_check_res_combi_sl( res_rr->dst_master_if, res_rr->dst_unitid, funcname );
}


static VOID bf_still_iip_check_res_combi( const T_BF_STILL_IIP_RESOURCE* const res )
{
	const CHAR funcname[] = "bf_still_iip_check_res_combi";

	bf_still_iip_check_res_combi_csel( res->src_master_if, res->src_cache_sel, funcname );

	bf_still_iip_check_res_combi_sl( res->dst_master_if, res->dst_unitid, funcname );
}

// delete_basefw\fj\iq
//static FJ_ERR_CODE bf_still_iip_calc_vis_single( const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IIP_VIS_RECT* const vis_rect, const T_BF_IQ_FRECT_TBL_SET* const frect_tbl_set, T_BF_STILL_IIP_FRECT_OUTPOS_SET* const outpos_set )
//{
//	T_BF_STILL_IIP_FRECT_OUTPOS*	outpos;

//	memset( outpos_set, '\0', sizeof(T_BF_STILL_IIP_FRECT_OUTPOS_SET) );

//	outpos = &outpos_set->ary[E_BF_IQ_FRECT_TBL_TYPE_SINGLE];

	// no-zoom
//	if( (vis_rect->sta_x == 0)
//	 && (vis_rect->sta_y == 0)
//	 && (vis_rect->width == dst_img->pixs.width)
//	 && (vis_rect->lines == dst_img->pixs.lines)
//	 ) {
//		outpos->OUTSIZE.bit.OUTHSZ = dst_img->pixs.width;
//		outpos->OUTSIZE.bit.OUTVSZ = dst_img->pixs.lines;
//		outpos->OUTSTARTPOS.bit.STARTX = 0;
//		outpos->OUTSTARTPOS.bit.STARTY = 0;
//		outpos->SL_HSIZE.bit.HSTART = 0;
//		outpos->SL_HSIZE.bit.HEND = dst_img->pixs.width -1;
//		outpos->SL_VSIZE.bit.VSTART = 0;
//		outpos->SL_VSIZE.bit.VEND = dst_img->pixs.lines -1;
//		outpos->valid = TRUE;

//		return FJ_ERR_OK;
//	}

//	outpos->OUTSIZE.bit.OUTHSZ = vis_rect->width;
//	outpos->OUTSIZE.bit.OUTVSZ = vis_rect->lines;
//	outpos->OUTSTARTPOS.bit.STARTX = vis_rect->sta_x;
//	outpos->OUTSTARTPOS.bit.STARTY = vis_rect->sta_y;
//	outpos->SL_HSIZE.bit.HSTART = 0;
//	outpos->SL_HSIZE.bit.HEND = outpos->OUTSIZE.bit.OUTHSZ -1;
//	outpos->SL_VSIZE.bit.VSTART = 0;
//	outpos->SL_VSIZE.bit.VEND = outpos->OUTSIZE.bit.OUTVSZ -1;
//	outpos->valid = TRUE;

//	return FJ_ERR_OK;
//}


// delete_basefw\fj\iq
//static FJ_ERR_CODE bf_still_iip_calc_vis_2virtical( const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IIP_VIS_RECT* const vis_rect, const T_BF_IQ_FRECT_TBL_SET* const frect_tbl_set, T_BF_STILL_IIP_FRECT_OUTPOS_SET* const outpos_set )
//{
//	T_BF_STILL_IIP_FRECT_OUTPOS*	outpos;
//	UINT32							frect0_tbl_width;
//	UINT32							frect1_tbl_width;
//	UINT32							frect_tbl_lines;
//	USHORT							sta_x;
//	USHORT							sta_y;
//	FJ_ERR_CODE						fj_ercd;

//	memset( outpos_set, '\0', sizeof(T_BF_STILL_IIP_FRECT_OUTPOS_SET) );

	// Calc FRECT0 table width pixs.
//	fj_ercd = BF_IQ_Frect_Tbl_Calc_Width( &frect0_tbl_width, frect_tbl_set, E_BF_IQ_FRECT_TBL_SUB_IDX_LEFT );
//	if( fj_ercd != FJ_ERR_OK ) {
//		BF_Debug_Print_Error(( "bf_still_iip_calc_vis_2virtical(): BF_IQ_Frect_Tbl_Calc_Width() error. %d\n", fj_ercd ));
//		return fj_ercd;
//	}
	// Calc FRECT1 table width pixs.
//	fj_ercd = BF_IQ_Frect_Tbl_Calc_Width( &frect1_tbl_width, frect_tbl_set, E_BF_IQ_FRECT_TBL_SUB_IDX_RIGHT );
//	if( fj_ercd != FJ_ERR_OK ) {
//		BF_Debug_Print_Error(( "bf_still_iip_calc_vis_2virtical(): BF_IQ_Frect_Tbl_Calc_Width() error. %d\n", fj_ercd ));
//		return fj_ercd;
//	}
	// Calc FRECT01 table lines.
//	fj_ercd = BF_IQ_Frect_Tbl_Calc_Lines( &frect_tbl_lines, frect_tbl_set, E_BF_IQ_FRECT_TBL_SUB_IDX_LEFT );
//	if( fj_ercd != FJ_ERR_OK ) {
//		BF_Debug_Print_Error(( "bf_still_iip_calc_vis_2virtical(): BF_IQ_Frect_Tbl_Calc_Lines() error. %d\n", fj_ercd ));
//		return fj_ercd;
//	}


//	if( (frect0_tbl_width + frect1_tbl_width) < (vis_rect->sta_x + vis_rect->width) ) {
//		BF_Debug_Print_Warning(( "bf_still_iip_calc_vis_2virtical(): crop rectanble over range. sta+width=%u(%u+%u) full_width=%u\n",
//					vis_rect->sta_x + vis_rect->width,
//					vis_rect->sta_x,
//					vis_rect->width,
//					frect0_tbl_width + frect1_tbl_width ));
//		sta_x = frect0_tbl_width + frect1_tbl_width - vis_rect->width;
//	}
//	else {
//		sta_x = vis_rect->sta_x;
//	}

//	if( frect_tbl_lines < (vis_rect->sta_y + vis_rect->lines) ) {
//		BF_Debug_Print_Warning(( "bf_still_iip_calc_vis_2virtical(): crop rectanble over range. sta+lines=%u(%u+%u) full_lines=%u\n",
//					vis_rect->sta_y + vis_rect->lines,
//					vis_rect->sta_y,
//					vis_rect->lines,
//					frect_tbl_lines ));
//		sta_y = frect_tbl_lines - vis_rect->lines;
//	}
//	else {
//		sta_y = vis_rect->sta_y;
//	}


	// Calc left
//	outpos = &outpos_set->ary[E_BF_IQ_FRECT_TBL_SUB_IDX_LEFT];
//	outpos->OUTSIZE.bit.OUTHSZ = frect0_tbl_width - sta_x;
//	outpos->OUTSIZE.bit.OUTVSZ = vis_rect->lines;
//	outpos->OUTSTARTPOS.bit.STARTX = sta_x;
//	outpos->OUTSTARTPOS.bit.STARTY = sta_y;
//	outpos->SL_HSIZE.bit.HSTART = 0;
//	outpos->SL_HSIZE.bit.HEND = outpos->OUTSIZE.bit.OUTHSZ -1;
//	outpos->SL_VSIZE.bit.VSTART = 0;
//	outpos->SL_VSIZE.bit.VEND = outpos->OUTSIZE.bit.OUTVSZ -1;
//	outpos->valid = TRUE;


	// Calc Right
//	outpos = &outpos_set->ary[E_BF_IQ_FRECT_TBL_SUB_IDX_RIGHT];
//	outpos->OUTSIZE.bit.OUTHSZ = vis_rect->width - (frect0_tbl_width - sta_x);
//	outpos->OUTSIZE.bit.OUTVSZ = vis_rect->lines;
//	outpos->OUTSTARTPOS.bit.STARTX = 0;
//	outpos->OUTSTARTPOS.bit.STARTY = sta_y;
//	outpos->SL_HSIZE.bit.HSTART = vis_rect->width - outpos->OUTSIZE.bit.OUTHSZ;
//	outpos->SL_HSIZE.bit.HEND = outpos->SL_HSIZE.bit.HSTART + outpos->OUTSIZE.bit.OUTHSZ -1;
//	outpos->SL_VSIZE.bit.VSTART = 0;
//	outpos->SL_VSIZE.bit.VEND = outpos->SL_VSIZE.bit.VSTART + outpos->OUTSIZE.bit.OUTVSZ -1;
//	outpos->valid = TRUE;

//	return FJ_ERR_OK;
//}

static FJ_ERR_CODE bf_still_iip_set_afn_dst_pos( const T_BF_STILL_IMG_CONV* const dst_img, const T_BF_STILL_IMG_WINDOW_L* const dst_pos, T_IM_IIP_PARAM_AFN* const afn_unit_inf, T_IM_IIP_PARAM_STS_FILL* const sl_unit_inf )
{
	ULONG		start_addr;
	ULONG		start_ofs;
	const CHAR	errmsg[] = "%s: err id=%u\n";

	if( dst_img->plane == E_BF_STILL_PLANE_TYPE_VIDEO ) {
		if( (dst_pos->sta_x == 0) && (dst_pos->sta_y == 0) && (dst_pos->width == 0) && (dst_pos->lines == 0) ) {
			sl_unit_inf->BASE.SL_HSIZE.bit.HSTART 	= 0;
			sl_unit_inf->BASE.SL_HSIZE.bit.HEND		= dst_img->pixs.width -1;
			sl_unit_inf->BASE.SL_VSIZE.bit.VSTART	= 0;
			sl_unit_inf->BASE.SL_VSIZE.bit.VEND		= dst_img->pixs.lines -1;

			return FJ_ERR_OK;
		}
	}

	if( (dst_pos->sta_x == 0)
	  && (dst_pos->sta_y == 0)
	  && (dst_pos->width == 0)
	  && (dst_pos->lines == 0)
	   ) {
		sl_unit_inf->BASE.SL_HSIZE.bit.HSTART	= 0;
		sl_unit_inf->BASE.SL_HSIZE.bit.HEND		= dst_img->pixs.width -1;
		sl_unit_inf->BASE.SL_VSIZE.bit.VSTART	= 0;
		sl_unit_inf->BASE.SL_VSIZE.bit.VEND		= dst_img->pixs.lines -1;
	}
	else {
		sl_unit_inf->BASE.SL_HSIZE.bit.HSTART	= dst_pos->sta_x;
		sl_unit_inf->BASE.SL_HSIZE.bit.HEND		= dst_pos->sta_x + dst_pos->width -1;
		sl_unit_inf->BASE.SL_VSIZE.bit.VSTART	= dst_pos->sta_y;
		sl_unit_inf->BASE.SL_VSIZE.bit.VEND		= dst_pos->sta_y + dst_pos->lines -1;
	}

	// Bandwidth aggravation check.
	start_addr = dst_img->addr.y_g + sl_unit_inf->BASE.SL_HSIZE.bit.HSTART;
	if( (start_addr % D_BF_STILL_IIP_IMG_ADDR_ALIGN) != 0 ) {
		BF_Debug_Print_Error(( errmsg, __func__, 1 ));		// Y Bandwidth aggravation
	}

	start_ofs = sl_unit_inf->BASE.SL_HSIZE.bit.HSTART;

	switch( dst_img->type ) {
		case E_BF_STILL_IMG_TYPE_YCC420:			// FALLTHROUGH
		case E_BF_STILL_IMG_TYPE_YCC422:			// FALLTHROUGH
		case E_BF_STILL_IMG_TYPE_VIDEO:
			start_ofs /= 2;
			break;
		case E_BF_STILL_IMG_TYPE_RGB888:			// FALLTHROUGH
		case E_BF_STILL_IMG_TYPE_RGB444:			// FALLTHROUGH
		case E_BF_STILL_IMG_TYPE_YCC444:
			// do nothing
			break;
		default:
			BF_Debug_Print_Error(( errmsg, __func__, 2 ));	// Unknown image type
			return FJ_ERR_NG_INPUT_PARAM;
	}

	switch( dst_img->plane ) {
		case E_BF_STILL_PLANE_TYPE_CHUNKY:			// FALLTHROUGH
			if( dst_img->type == E_BF_STILL_IMG_TYPE_RGB888 ) {
				start_ofs *= 3;
			}
			else if( dst_img->type == E_BF_STILL_IMG_TYPE_RGB444 ) {
				start_ofs *= 3;
			}
			else {
				BF_Debug_Print_Error(( errmsg, __func__, 3 ));	// Unsupported plane type
			}
			break;
		case E_BF_STILL_PLANE_TYPE_PLANAR:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_PLANAR:	// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_C_LINEAR:
			// do nothing
			break;
		case E_BF_STILL_PLANE_TYPE_TILE:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_1PLANE:
			BF_Debug_Print_Error(( errmsg, __func__, 4 ));	// Unsupported plane type
			break;
		case E_BF_STILL_PLANE_TYPE_LINEAR_C_INTL:	// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_C_INTL:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_VIDEO:
			start_ofs *= 2;
			break;
		default:
			BF_Debug_Print_Error(( errmsg, __func__, 5 ));	// Unknown plane type
			return FJ_ERR_NG_INPUT_PARAM;
	}

	start_addr = dst_img->addr.cb_b + start_ofs;
	if( (start_addr % D_BF_STILL_IIP_IMG_ADDR_ALIGN) != 0 ) {
		BF_Debug_Print_Error(( errmsg, __func__, 6 ));		// CbCr Bandwidth aggravation
	}

	return FJ_ERR_OK;
}

static FJ_ERR_CODE bf_still_iip_set_afn_dst_pos_cbcronly( const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IMG_WINDOW_L* const dst_pos, T_IM_IIP_PARAM_AFN* const afn_unit_inf, T_IM_IIP_PARAM_STS_FILL* const sl_unit_inf )
{
	ULONG		start_addr;
	ULONG		start_ofs;
	T_BF_STILL_IMG_WINDOW_L dst_pos_cbcr = {
		.sta_x = 0, .sta_y = 0, .width = 0, .lines = 0
	};
	const CHAR	errmsg[] = "%s: err id=%u\n";

	if( (dst_pos->sta_x == 0)
	 && (dst_pos->sta_y == 0)
	 && (dst_pos->width == 0)
	 && (dst_pos->lines == 0)
	 ) {
		dst_pos_cbcr.sta_x = 0;
		dst_pos_cbcr.sta_y = 0;
		dst_pos_cbcr.width = dst_img->pixs.width;
		dst_pos_cbcr.lines = dst_img->pixs.lines;
	}
	else {
		dst_pos_cbcr = *dst_pos;
	}


	// Resize Y to CbCr rectangle.
	switch( dst_img->type ) {
		case E_BF_STILL_IMG_TYPE_YCC420:
			dst_pos_cbcr.sta_x /= 2;
			dst_pos_cbcr.sta_y /= 2;
			dst_pos_cbcr.width /= 2;
			dst_pos_cbcr.lines /= 2;
			break;
		case E_BF_STILL_IMG_TYPE_YCC422:
			dst_pos_cbcr.sta_x /= 2;
			dst_pos_cbcr.width /= 2;
			break;
		case E_BF_STILL_IMG_TYPE_YCC444:
			// do nothing
			break;
		default:
			BF_Debug_Print_Error(( errmsg, __func__, 1 ));	// Unknown image type
			return FJ_ERR_NG_INPUT_PARAM;
	}


	sl_unit_inf->BASE.SL_HSIZE.bit.HSTART = dst_pos_cbcr.sta_x;
	sl_unit_inf->BASE.SL_HSIZE.bit.HEND = dst_pos_cbcr.sta_x + dst_pos_cbcr.width -1;
	sl_unit_inf->BASE.SL_VSIZE.bit.VSTART = dst_pos_cbcr.sta_y;
	sl_unit_inf->BASE.SL_VSIZE.bit.VEND = dst_pos_cbcr.sta_y + dst_pos_cbcr.lines -1;


	// Bandwidth aggravation check
	start_ofs = sl_unit_inf->BASE.SL_HSIZE.bit.HSTART;
	switch( dst_img->type ) {
		case E_BF_STILL_IMG_TYPE_YCC420:			// FALLTHROUGH
		case E_BF_STILL_IMG_TYPE_YCC422:
			start_ofs /= 2;
			break;
		case E_BF_STILL_IMG_TYPE_YCC444:
			// do nothing
			break;
		default:
			BF_Debug_Print_Error(( errmsg, __func__, 2 ));	// Unknown image type
			return FJ_ERR_NG_INPUT_PARAM;
	}
	switch( dst_img->plane ) {
		case E_BF_STILL_PLANE_TYPE_PLANAR:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_PLANAR:	// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_C_LINEAR:
			// do nothing
			break;
		case E_BF_STILL_PLANE_TYPE_VIDEO:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_TILE:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_1PLANE:
			BF_Debug_Print_Error(( errmsg, __func__, 3 ));	// Unsupported plane type
			break;
		case E_BF_STILL_PLANE_TYPE_LINEAR_C_INTL:	// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_C_INTL:
			start_ofs *= 2;
			break;
		default:
			BF_Debug_Print_Error(( errmsg, __func__, 4 ));	// Unknown plane type
			return FJ_ERR_NG_INPUT_PARAM;
	}

	start_addr = dst_img->addr.cb + start_ofs;
	if( (start_addr % D_BF_STILL_IIP_IMG_ADDR_ALIGN) != 0 ) {
		BF_Debug_Print_Error(( errmsg, __func__, 5 ));		// CbCr Bandwidth aggravation
	}

	return FJ_ERR_OK;
}

static VOID bf_still_iip_set_afn_alignment( const T_IMAGE_SIZE* const alignment, T_IM_IIP_PARAM_AFN_OPCOL_0* const afn_tbl, T_IM_IIP_PARAM_STS_FILL* const sl_unit_inf )
{
	UINT32 orig_width;
	UINT32 fill_width;	// All width include orig_width and fill.
	UINT32 orig_lines;
	UINT32 fill_lines;	// All lines include orig_lines and fill.

	if( alignment->width > 1 ) {
		orig_width = sl_unit_inf->BASE.SL_HSIZE.bit.HEND +1 - sl_unit_inf->BASE.SL_HSIZE.bit.HSTART;
		fill_width = M_BF_STILL_ROUNDUP_N( orig_width, alignment->width );
		sl_unit_inf->BASE.SL_HSIZE.bit.HEND = sl_unit_inf->BASE.SL_HSIZE.bit.HSTART + fill_width -1;

		afn_tbl->SZ.bit.AFNDHSZ = M_BF_STILL_ROUNDUP_N( afn_tbl->SZ.bit.AFNDHSZ, alignment->width );
	}

	if( alignment->lines > 1 ) {
		orig_lines = sl_unit_inf->BASE.SL_VSIZE.bit.VEND +1 - sl_unit_inf->BASE.SL_VSIZE.bit.VSTART;
		fill_lines = M_BF_STILL_ROUNDUP_N( orig_lines, alignment->lines );
		sl_unit_inf->BASE.SL_VSIZE.bit.VEND = sl_unit_inf->BASE.SL_VSIZE.bit.VSTART + fill_lines -1;

		afn_tbl->SZ.bit.AFNDVSZ = M_BF_STILL_ROUNDUP_N( afn_tbl->SZ.bit.AFNDVSZ, alignment->lines );
	}
}

static FJ_ERR_CODE bf_still_iip_set_src_unitid_portid( E_IM_IIP_UNIT_ID* const src_unitid, E_IM_IIP_PARAM_PORTID* const src_portid, ULLONG* const unitid_bitmask64 )
{
	UINT32						loopcnt;
	static const struct {
		ULLONG					unit_bitmask;
		E_IM_IIP_UNIT_ID		unitid;
		E_IM_IIP_PARAM_PORTID	portid;
	} unitid_tbl[] = {
		{ D_IM_IIP_PARAM_PLDUNIT_AFN0,		E_IM_IIP_UNIT_ID_AFN0,		E_IM_IIP_PARAM_PORTID_AFN0 },
		{ D_IM_IIP_PARAM_PLDUNIT_AFN1,		E_IM_IIP_UNIT_ID_AFN1,		E_IM_IIP_PARAM_PORTID_AFN1 },
		{ D_IM_IIP_PARAM_PLDUNIT_AFN2,		E_IM_IIP_UNIT_ID_AFN2,		E_IM_IIP_PARAM_PORTID_AFN2 },
		{ D_IM_IIP_PARAM_PLDUNIT_AFN3,		E_IM_IIP_UNIT_ID_AFN3,		E_IM_IIP_PARAM_PORTID_AFN3 },
		{ D_IM_IIP_PARAM_PLDUNIT_FRECT0,	E_IM_IIP_UNIT_ID_FRECT0,	E_IM_IIP_PARAM_PORTID_FRECT0 },
		{ D_IM_IIP_PARAM_PLDUNIT_FRECT1,	E_IM_IIP_UNIT_ID_FRECT1,	E_IM_IIP_PARAM_PORTID_FRECT1 },
		{ D_IM_IIP_PARAM_PLDUNIT_LD0,		E_IM_IIP_UNIT_ID_LD0,		E_IM_IIP_PARAM_PORTID_LD0 },
		{ D_IM_IIP_PARAM_PLDUNIT_LD1,		E_IM_IIP_UNIT_ID_LD1,		E_IM_IIP_PARAM_PORTID_LD1 },
		{ D_IM_IIP_PARAM_PLDUNIT_LD2,		E_IM_IIP_UNIT_ID_LD2,		E_IM_IIP_PARAM_PORTID_LD2 },
		{ D_IM_IIP_PARAM_PLDUNIT_LD3,		E_IM_IIP_UNIT_ID_LD3,		E_IM_IIP_PARAM_PORTID_LD3 },
		{ D_IM_IIP_PARAM_PLDUNIT_LD4,		E_IM_IIP_UNIT_ID_LD4,		E_IM_IIP_PARAM_PORTID_LD4 },
		{ D_IM_IIP_PARAM_PLDUNIT_LD5,		E_IM_IIP_UNIT_ID_LD5,		E_IM_IIP_PARAM_PORTID_LD5 },
		{ D_IM_IIP_PARAM_PLDUNIT_LD6,		E_IM_IIP_UNIT_ID_LD6,		E_IM_IIP_PARAM_PORTID_LD6 },
		{ D_IM_IIP_PARAM_PLDUNIT_LD7,		E_IM_IIP_UNIT_ID_LD7,		E_IM_IIP_PARAM_PORTID_LD7 },
		{ D_IM_IIP_PARAM_PLDUNIT_FLT,		E_IM_IIP_UNIT_ID_FLT,		E_IM_IIP_PARAM_PORTID_FLT_P0 },	// Is not possible to correspond to multiple ports.
		{ D_IM_IIP_PARAM_PLDUNIT_CSC0,		E_IM_IIP_UNIT_ID_CSC0,		E_IM_IIP_PARAM_PORTID_CSC0 },
		{ D_IM_IIP_PARAM_PLDUNIT_CSC1,		E_IM_IIP_UNIT_ID_CSC1,		E_IM_IIP_PARAM_PORTID_CSC1 },
		{ D_IM_IIP_PARAM_PLDUNIT_CSC2,		E_IM_IIP_UNIT_ID_CSC2,		E_IM_IIP_PARAM_PORTID_CSC2 },
		{ D_IM_IIP_PARAM_PLDUNIT_CSC3,		E_IM_IIP_UNIT_ID_CSC3,		E_IM_IIP_PARAM_PORTID_CSC3 },
		{ D_IM_IIP_PARAM_PLDUNIT_LUT,		E_IM_IIP_UNIT_ID_LUT,		E_IM_IIP_PARAM_PORTID_LUT_P0 },	// Is not possible to correspond to multiple ports.
		{ D_IM_IIP_PARAM_PLDUNIT_GPC,		E_IM_IIP_UNIT_ID_GPC,		E_IM_IIP_PARAM_PORTID_GPC_P0 },	// Is not possible to correspond to multiple ports.
		{ D_IM_IIP_PARAM_PLDUNIT_BLEND0,	E_IM_IIP_UNIT_ID_BLEND0,	E_IM_IIP_PARAM_PORTID_BLEND0_P0 },	// Is not possible to correspond to multiple ports.
		{ D_IM_IIP_PARAM_PLDUNIT_BLEND1,	E_IM_IIP_UNIT_ID_BLEND1,	E_IM_IIP_PARAM_PORTID_BLEND1_P0 },	// Is not possible to correspond to multiple ports.
	};

	for( loopcnt = 0; loopcnt < (sizeof(unitid_tbl) / sizeof(unitid_tbl[0])); loopcnt++ ) {
		if( IS_FLGPTN_EQUAL(*unitid_bitmask64, unitid_tbl[loopcnt].unit_bitmask) ){
			*unitid_bitmask64 &= ~unitid_tbl[loopcnt].unit_bitmask;
			*src_unitid	= unitid_tbl[loopcnt].unitid;
			*src_portid	= unitid_tbl[loopcnt].portid;
			return FJ_ERR_OK;
		}
	}

	BF_Debug_Print_Warning(( "bf_still_iip_set_src_unitid_portid: setting parameter is not support.\n" ));

	return FJ_ERR_NG_INPUT_PARAM;
}

static FJ_ERR_CODE bf_still_iip_set_filter_unitid_portid( E_IM_IIP_UNIT_ID* const filter_unitid, E_IM_IIP_PARAM_PORTID filter_portid[2], ULLONG* const unitid_bitmask64 )
{
	UINT32						loopcnt;
	static const struct {
		ULLONG					unit_bitmask;
		E_IM_IIP_UNIT_ID		unitid;
		E_IM_IIP_PARAM_PORTID	portid[2];
	} unitid_tbl[] = {
		{ D_IM_IIP_PARAM_PLDUNIT_FLT,		E_IM_IIP_UNIT_ID_FLT,		{ E_IM_IIP_PARAM_PORTID_FLT_P0, E_IM_IIP_PARAM_PORTID_FLT_P1 } },
		{ D_IM_IIP_PARAM_PLDUNIT_CSC0,		E_IM_IIP_UNIT_ID_CSC0,		{ E_IM_IIP_PARAM_PORTID_CSC0 } },
		{ D_IM_IIP_PARAM_PLDUNIT_CSC1,		E_IM_IIP_UNIT_ID_CSC1,		{ E_IM_IIP_PARAM_PORTID_CSC1 } },
		{ D_IM_IIP_PARAM_PLDUNIT_CSC2,		E_IM_IIP_UNIT_ID_CSC2,		{ E_IM_IIP_PARAM_PORTID_CSC2 } },
		{ D_IM_IIP_PARAM_PLDUNIT_CSC3,		E_IM_IIP_UNIT_ID_CSC3,		{ E_IM_IIP_PARAM_PORTID_CSC3 } },
		{ D_IM_IIP_PARAM_PLDUNIT_LUT,		E_IM_IIP_UNIT_ID_LUT,		{ E_IM_IIP_PARAM_PORTID_LUT_P0, E_IM_IIP_PARAM_PORTID_LUT_P1 } },	// Is not possible to correspond to multiple ports.
		{ D_IM_IIP_PARAM_PLDUNIT_GPC,		E_IM_IIP_UNIT_ID_GPC,		{ E_IM_IIP_PARAM_PORTID_GPC_P0, E_IM_IIP_PARAM_PORTID_GPC_P1 } },
		{ D_IM_IIP_PARAM_PLDUNIT_BLEND0,	E_IM_IIP_UNIT_ID_BLEND0,	{ E_IM_IIP_PARAM_PORTID_BLEND0_P0, E_IM_IIP_PARAM_PORTID_BLEND0_P1 } },	// Is not possible to correspond to multiple ports.
		{ D_IM_IIP_PARAM_PLDUNIT_BLEND1,	E_IM_IIP_UNIT_ID_BLEND1,	{ E_IM_IIP_PARAM_PORTID_BLEND1_P0, E_IM_IIP_PARAM_PORTID_BLEND1_P1 } },	// Is not possible to correspond to multiple ports.
	};

	for( loopcnt = 0; loopcnt < (sizeof(unitid_tbl) / sizeof(unitid_tbl[0])); loopcnt++ ) {
		if( IS_FLGPTN_EQUAL(*unitid_bitmask64, unitid_tbl[loopcnt].unit_bitmask) ){
			*unitid_bitmask64 &= ~unitid_tbl[loopcnt].unit_bitmask;
			*filter_unitid		= unitid_tbl[loopcnt].unitid;
			filter_portid[0]	= unitid_tbl[loopcnt].portid[0];
			filter_portid[1]	= unitid_tbl[loopcnt].portid[1];
			return FJ_ERR_OK;
		}
	}

	BF_Debug_Print_Warning(( "bf_still_iip_set_filter_unitid_portid: setting parameter is not support.\n" ));

	return FJ_ERR_NG_INPUT_PARAM;
}

static FJ_ERR_CODE bf_still_iip_set_dst_unitid_portid( E_IM_IIP_UNIT_ID* const dst_unitid, E_IM_IIP_PARAM_PORTID* const dst_portid, UINT32* const wait_factor, ULLONG* const unitid_bitmask64 )
{
	UINT32						loopcnt;
	static const struct {
		ULLONG					unit_bitmask;
		E_IM_IIP_UNIT_ID		unitid;
		E_IM_IIP_PARAM_PORTID	portid;
		UINT32					wait_factor;
	} unitid_tbl[] = {
		{ D_IM_IIP_PARAM_PLDUNIT_SL0,		E_IM_IIP_UNIT_ID_SL0,		E_IM_IIP_PARAM_PORTID_SL0,		(D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL0END) },
		{ D_IM_IIP_PARAM_PLDUNIT_SL1,		E_IM_IIP_UNIT_ID_SL1,		E_IM_IIP_PARAM_PORTID_SL1,		(D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL1END) },
		{ D_IM_IIP_PARAM_PLDUNIT_SL2,		E_IM_IIP_UNIT_ID_SL2,		E_IM_IIP_PARAM_PORTID_SL2,		(D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END) },
		{ D_IM_IIP_PARAM_PLDUNIT_SL3,		E_IM_IIP_UNIT_ID_SL3,		E_IM_IIP_PARAM_PORTID_SL3,		(D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL3END) },
		{ D_IM_IIP_PARAM_PLDUNIT_SL4,		E_IM_IIP_UNIT_ID_SL4,		E_IM_IIP_PARAM_PORTID_SL4,		(D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL4END) },
		{ D_IM_IIP_PARAM_PLDUNIT_SL5,		E_IM_IIP_UNIT_ID_SL5,		E_IM_IIP_PARAM_PORTID_SL5,		(D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL5END) },
		{ D_IM_IIP_PARAM_PLDUNIT_SL6,		E_IM_IIP_UNIT_ID_SL6,		E_IM_IIP_PARAM_PORTID_SL6,		(D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL6END) },
		{ D_IM_IIP_PARAM_PLDUNIT_SL7,		E_IM_IIP_UNIT_ID_SL7,		E_IM_IIP_PARAM_PORTID_SL7,		(D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL7END) },
		{ D_IM_IIP_PARAM_PLDUNIT_SL8,		E_IM_IIP_UNIT_ID_SL8,		E_IM_IIP_PARAM_PORTID_SL8,		(D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL8END) },
		{ D_IM_IIP_PARAM_PLDUNIT_CFL0,		E_IM_IIP_UNIT_ID_CFL0,		E_IM_IIP_PARAM_PORTID_CFL0_P0,	(D_IM_IIP_INT_FACTOR_AXIERR) },	// Is not possible to correspond to multiple ports.
	};

	for( loopcnt = 0; loopcnt < (sizeof(unitid_tbl) / sizeof(unitid_tbl[0])); loopcnt++ ) {
		if( IS_FLGPTN_EQUAL(*unitid_bitmask64, unitid_tbl[loopcnt].unit_bitmask) ){
			*unitid_bitmask64 &= ~unitid_tbl[loopcnt].unit_bitmask;
			*dst_unitid		= unitid_tbl[loopcnt].unitid;
			*dst_portid		= unitid_tbl[loopcnt].portid;
			*wait_factor	= unitid_tbl[loopcnt].wait_factor;
			return FJ_ERR_OK;
		}
	}

	BF_Debug_Print_Warning(( "bf_still_iip_set_dst_unitid_portid: setting parameter is not support.\n" ));

	return FJ_ERR_NG_INPUT_PARAM;
}

static FJ_ERR_CODE bf_still_iip_check_pixid( UINT32* const target_pixid, UINT32* const pixid_bitmask )
{
	if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_0) ){			// pixid-0
		*target_pixid = 0;
		*pixid_bitmask -= E_IM_IIP_PIXID_0;
	}
	else if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_1) ){		// pixid-1
		*target_pixid = 1;
		*pixid_bitmask -= E_IM_IIP_PIXID_1;
	}
	else if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_2) ){		// pixid-2
		*target_pixid = 2;
		*pixid_bitmask -= E_IM_IIP_PIXID_2;
	}
	else if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_3) ){		// pixid-3
		*target_pixid = 3;
		*pixid_bitmask -= E_IM_IIP_PIXID_3;
	}
	else if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_4) ){		// pixid-4
		*target_pixid = 4;
		*pixid_bitmask -= E_IM_IIP_PIXID_4;
	}
	else if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_5) ){		// pixid-5
		*target_pixid = 5;
		*pixid_bitmask -= E_IM_IIP_PIXID_5;
	}
	else if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_6) ){		// pixid-6
		*target_pixid = 6;
		*pixid_bitmask -= E_IM_IIP_PIXID_6;
	}
	else if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_7) ){		// pixid-7
		*target_pixid = 7;
		*pixid_bitmask -= E_IM_IIP_PIXID_7;
	}
	else if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_8) ){		// pixid-8
		*target_pixid = 8;
		*pixid_bitmask -= E_IM_IIP_PIXID_8;
	}
	else if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_9) ){		// pixid-9
		*target_pixid = 9;
		*pixid_bitmask -= E_IM_IIP_PIXID_9;
	}
	else if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_10) ){		// pixid-10
		*target_pixid = 10;
		*pixid_bitmask -= E_IM_IIP_PIXID_10;
	}
	else if( IS_FLGPTN_EQUAL(*pixid_bitmask, E_IM_IIP_PIXID_11) ){		// pixid-11
		*target_pixid = 11;
		*pixid_bitmask -= E_IM_IIP_PIXID_11;
	}
	else{
		BF_Debug_Print_Warning(( "bf_still_iip_check_pixid: setting parameter is not support.\n" ));
		*target_pixid = 0;
		return FJ_ERR_NG_INPUT_PARAM;
	}

	return FJ_ERR_OK;
}

static FJ_ERR_CODE bf_still_iip_set_pixid( T_BF_STILL_IIP_RESOURCE* const res_resource )
{
	FJ_ERR_CODE	fj_ercd = FJ_ERR_OK;
	UINT32	pixid_bitmask;

	pixid_bitmask = res_resource->pixid_bitmask;

	// dst pixID
	if( pixid_bitmask != 0 ) {
		fj_ercd = bf_still_iip_check_pixid( &res_resource->dst_pixid, &pixid_bitmask );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}
	}
	else {
		res_resource->dst_pixid = D_BF_STILL_IIP_PIXID_INVALID;
	}

	// src pixID
	if( pixid_bitmask != 0 ) {
		fj_ercd = bf_still_iip_check_pixid( &res_resource->src_pixid, &pixid_bitmask );
	}
	else {
		res_resource->src_pixid = D_BF_STILL_IIP_PIXID_INVALID;
	}

	return fj_ercd;
}

static FJ_ERR_CODE bf_still_iip_set_pixid2( T_BF_STILL_IIP_RESOURCE2* const res_resource )
{
	FJ_ERR_CODE	fj_ercd;
	UINT32	pixid_bitmask;

	pixid_bitmask = res_resource->pixid_bitmask;

	// dst pixID
	fj_ercd = bf_still_iip_check_pixid( &res_resource->dst_pixid, &pixid_bitmask );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	// src pixID
	fj_ercd = bf_still_iip_check_pixid( &res_resource->src_pixid[0], &pixid_bitmask );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	if( pixid_bitmask != 0 ) {
		fj_ercd = bf_still_iip_check_pixid( &res_resource->src_pixid[1], &pixid_bitmask );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}
	}

	return fj_ercd;
}

static FJ_ERR_CODE bf_still_iip_set_pixid3( T_BF_STILL_IIP_RESOURCE_SPLIT* const res_resource )
{
	FJ_ERR_CODE	fj_ercd;
	UINT32	pixid_bitmask;

	pixid_bitmask = res_resource->pixid_bitmask;

	// dst pixID
	fj_ercd = bf_still_iip_check_pixid( &res_resource->dst_pixid, &pixid_bitmask );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	// disp pixID
	fj_ercd = bf_still_iip_check_pixid( &res_resource->disp_pixid, &pixid_bitmask );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	// src pixID
	fj_ercd = bf_still_iip_check_pixid( &res_resource->src_pixid, &pixid_bitmask );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	return fj_ercd;
}

static FJ_ERR_CODE bf_still_iip_set_pixid4( T_BF_STILL_IIP_RESOURCE_SPLIT* const res_resource )
{
	FJ_ERR_CODE	fj_ercd;
	UINT32	pixid_bitmask;

	pixid_bitmask = res_resource->pixid_bitmask;

	// dst pixID
	fj_ercd = bf_still_iip_check_pixid( &res_resource->dst_pixid, &pixid_bitmask );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	// src pixID
	fj_ercd = bf_still_iip_check_pixid( &res_resource->src_pixid, &pixid_bitmask );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	return fj_ercd;
}

static UINT32 bf_still_iip_set_lineend_intr( const UINT32 res_wait_factor )
{

	INT32	ercd;
	UINT32	wait_factor;

	if( (res_wait_factor & D_IM_IIP_INT_FACTOR_SL2END) != 0 ) {
		wait_factor = D_IM_IIP_INT_FACTOR_SL2_LINEEND;
	}
	else
	if( (res_wait_factor & D_IM_IIP_INT_FACTOR_SL3END) != 0 ) {
		wait_factor = D_IM_IIP_INT_FACTOR_SL3_LINEEND;
	}
	else
	if( (res_wait_factor & D_IM_IIP_INT_FACTOR_SL4END) != 0 ) {
		wait_factor = D_IM_IIP_INT_FACTOR_SL4_LINEEND;
	}
	else
	if( (res_wait_factor & D_IM_IIP_INT_FACTOR_SL5END) != 0 ) {
		wait_factor = D_IM_IIP_INT_FACTOR_SL5_LINEEND;
	}
	else
	if( (res_wait_factor & D_IM_IIP_INT_FACTOR_SL6END) != 0 ) {
		wait_factor = D_IM_IIP_INT_FACTOR_SL6_LINEEND;
	}
	else {
		return D_IM_IIP_OK;
	}

	ercd = Im_IIP_Set_SLIntLines( wait_factor, 2 );		// Interrupt of start time.
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}
	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}
	ercd = Im_IIP_Set_IntHandler( wait_factor, bf_still_iip_int_handler );
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	return ercd;
}

static VOID bf_still_iip_set_stx_clip( const E_BF_STILL_IMG_DEPTH depth, T_IM_IIP_PARAM_STS_FILL* const sl_unit_inf )
{
	switch( depth ) {
		case E_BF_STILL_IMG_DEPTH_U8:
			sl_unit_inf->BASE.CLPTH_U_0.bit.CLPTH_U		= 0xFF;
			sl_unit_inf->BASE.CLPVAL_U_0.bit.CLPVAL_U	= 0xFF;
			break;
		case E_BF_STILL_IMG_DEPTH_U12:
			sl_unit_inf->BASE.CLPTH_U_0.bit.CLPTH_U		= 0xFFF;
			sl_unit_inf->BASE.CLPVAL_U_0.bit.CLPVAL_U	= 0xFFF;
			break;
		case E_BF_STILL_IMG_DEPTH_U16:
			sl_unit_inf->BASE.CLPTH_U_0.bit.CLPTH_U		= 0x3FFF;
			sl_unit_inf->BASE.CLPVAL_U_0.bit.CLPVAL_U	= 0x3FFF;
			break;
		default:
			BF_Debug_Print_Error(( "%s: unsupported image depth. %u\n", __func__, depth ));
			sl_unit_inf->BASE.CLPTH_U_0.bit.CLPTH_U		= 0x3FFF;	// fail safe
			sl_unit_inf->BASE.CLPVAL_U_0.bit.CLPVAL_U	= 0x3FFF;	// fail safe
			break;
	}
}

static VOID bf_still_iip_set_stx_mask_cbcronly( T_IM_IIP_PARAM_STS_FILL* const iip_param_sts_fill, const BF_MASK_CTRL* const mask_ctrl, const E_BF_STILL_IMG_TYPE type )
{
	UINT32	loopcnt;
	UINT32	mask_num;
	UINT32	reduce_x;
	UINT32	reduce_y;

	switch( type ) {
		case E_BF_STILL_IMG_TYPE_YCC422:
			reduce_x = 2;
			reduce_y = 1;
			break;
		case E_BF_STILL_IMG_TYPE_YCC420:
			reduce_x = 2;
			reduce_y = 2;
			break;

//		case E_BF_STILL_IMG_TYPE_YCC444:
		default:
			reduce_x = 1;
			reduce_y = 1;
			break;
	}

	// Set color
	iip_param_sts_fill->FILL_DATA.bit.FILL_D0 = mask_ctrl->mask_color.mask_color_y;
	iip_param_sts_fill->FILL_DATA.bit.FILL_D1 = mask_ctrl->mask_color.mask_color_cb;
	iip_param_sts_fill->FILL_DATA.bit.FILL_D2 = mask_ctrl->mask_color.mask_color_cr;


	// Set disable all.
	iip_param_sts_fill->FILL_EN.word = 0ul;

	// Set enalble flag 0~(mask_num -1)
	mask_num = mask_ctrl->mask_num;
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN0 = mask_ctrl->mask_info[0].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN1 = mask_ctrl->mask_info[1].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN2 = mask_ctrl->mask_info[2].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN3 = mask_ctrl->mask_info[3].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN4 = mask_ctrl->mask_info[4].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN5 = mask_ctrl->mask_info[5].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN6 = mask_ctrl->mask_info[6].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN7 = mask_ctrl->mask_info[7].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN8 = mask_ctrl->mask_info[8].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN9 = mask_ctrl->mask_info[9].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN10 = mask_ctrl->mask_info[10].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN11 = mask_ctrl->mask_info[11].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN12 = mask_ctrl->mask_info[12].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN13 = mask_ctrl->mask_info[13].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN14 = mask_ctrl->mask_info[14].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN15 = mask_ctrl->mask_info[15].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN16 = mask_ctrl->mask_info[16].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN17 = mask_ctrl->mask_info[17].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN18 = mask_ctrl->mask_info[18].enable;
		mask_num--;
	}
	if( mask_num != 0 ) {
		iip_param_sts_fill->FILL_EN.bit.FILL_EN19 = mask_ctrl->mask_info[19].enable;
		mask_num--;
	}


	// Set rectangle.
	for( loopcnt = 0; loopcnt < mask_ctrl->mask_num; loopcnt++ ) {
		iip_param_sts_fill->FILL_SIZE[loopcnt].bit.FILL_HSTA = mask_ctrl->mask_info[loopcnt].mask_rect.start_x / reduce_x;
		iip_param_sts_fill->FILL_SIZE[loopcnt].bit.FILL_HEND = mask_ctrl->mask_info[loopcnt].mask_rect.end_x / reduce_x;
		iip_param_sts_fill->FILL_SIZE[loopcnt].bit.FILL_VSTA = mask_ctrl->mask_info[loopcnt].mask_rect.start_y / reduce_y;
		iip_param_sts_fill->FILL_SIZE[loopcnt].bit.FILL_VEND = mask_ctrl->mask_info[loopcnt].mask_rect.end_y / reduce_y;
	}
}

static FJ_ERR_CODE bf_still_iip_set_afn_bicubic( T_IM_IIP_PARAM_AFN* const afn_unit_inf, const UINT32 open_res_bitmask, const E_IM_IIP_UNIT_ID src_unitid, const BOOL nearest_neighbor )
{
	if( nearest_neighbor != FALSE ) {
		afn_unit_inf->CALMETHOD.bit.ITMD = E_IM_IIP_PARAM_ITMD_NEAREST;
		afn_unit_inf->CALMETHOD.bit.SCUB = E_IM_IIP_PARAM_SCUB_0;	// D.C.
		return FJ_ERR_OK;
	}

	// Interpolation Mode
	if( (open_res_bitmask & E_IM_IIP_OPEN_RES_BICUBIC0) != 0 ) {
		afn_unit_inf->CALMETHOD.bit.ITMD = E_IM_IIP_PARAM_ITMD_BI_CUBIC;
		afn_unit_inf->CALMETHOD.bit.SCUB = E_IM_IIP_PARAM_SCUB_0;
	}
	else if( (open_res_bitmask & E_IM_IIP_OPEN_RES_BICUBIC1) != 0 ) {
		afn_unit_inf->CALMETHOD.bit.ITMD = E_IM_IIP_PARAM_ITMD_BI_CUBIC;
		afn_unit_inf->CALMETHOD.bit.SCUB = E_IM_IIP_PARAM_SCUB_1;
	}
	else {
		afn_unit_inf->CALMETHOD.bit.ITMD = E_IM_IIP_PARAM_ITMD_BI_LINEAR;
		afn_unit_inf->CALMETHOD.bit.SCUB = E_IM_IIP_PARAM_SCUB_0;	// D.C.
	}

	if ( afn_unit_inf->CALMETHOD.bit.ITMD == E_IM_IIP_PARAM_ITMD_BI_CUBIC ) {
	// for Bi-Cubic
		switch ( src_unitid ) {
			case E_IM_IIP_UNIT_ID_AFN0:
				afn_unit_inf->CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_AFN0;
				break;
			case E_IM_IIP_UNIT_ID_AFN1:
				afn_unit_inf->CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_AFN1;
				break;
			case E_IM_IIP_UNIT_ID_AFN2:
				afn_unit_inf->CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_AFN2;
				break;
			case E_IM_IIP_UNIT_ID_AFN3:
				afn_unit_inf->CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_AFN3;
				break;
			default:
				return FJ_ERR_NG;
		}
		afn_unit_inf->OPMD.bit.OPMD0 = E_IM_IIP_PARAM_OPMD_ADD;
		afn_unit_inf->OPY.bit.OPBY = 0;		// +0
		afn_unit_inf->OPY.bit.OPAY = 0x10;	// x1.0
		afn_unit_inf->OPB.bit.OPBB = 0;		// +0
		afn_unit_inf->OPB.bit.OPAB = 0x10;	// x1.0
		afn_unit_inf->OPR.bit.OPBR = 0;		// +0
		afn_unit_inf->OPR.bit.OPAR = 0x10;	// x1.0
		afn_unit_inf->OPA.bit.OPBA = 0;		// +0
		afn_unit_inf->OPA.bit.OPAA = 0x10;	// x1.0
		afn_unit_inf->CLIPLVLY.bit.CLIPLVLYH = 0xFF;
		afn_unit_inf->CLIPLVLY.bit.CLIPLVLYL = 0;
		afn_unit_inf->CLIPLVLB.bit.CLIPLVLBH = 0xFF;
		afn_unit_inf->CLIPLVLB.bit.CLIPLVLBL = 0;
		afn_unit_inf->CLIPLVLR.bit.CLIPLVLRH = 0xFF;
		afn_unit_inf->CLIPLVLR.bit.CLIPLVLRL = 0;
		afn_unit_inf->CLIPLVLA.bit.CLIPLVLAH = 0xFF;
		afn_unit_inf->CLIPLVLA.bit.CLIPLVLAL = 0;
	}
	return FJ_ERR_OK;
}

static FJ_ERR_CODE bf_still_iip_set_afn_grain_pixs( T_IM_IIP_PARAM_AFN* const afn_unit_inf, const T_IMAGE_SIZE* const grain_pixs )
{
	const CHAR							errmsg[] = "%s: no=%u data=%u\n";

	switch( grain_pixs->width ) {
		case 64:
			afn_unit_inf->AFNCTL.bit.GHSZ = E_IM_IIP_PARAM_GHSZ_64PIX;
			break;
		case 32:
			afn_unit_inf->AFNCTL.bit.GHSZ = E_IM_IIP_PARAM_GHSZ_32PIX;
			break;
		default:
			BF_Debug_Print_Error(( errmsg, __func__, 1, grain_pixs->width ));
			return FJ_ERR_NG;
	}

	switch( grain_pixs->lines ) {
		case 2:
			afn_unit_inf->AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_2PIX;
			break;
		case 4:
			afn_unit_inf->AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_4PIX;
			break;
		case 8:
			afn_unit_inf->AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_8PIX;
			break;
		case 16:
			afn_unit_inf->AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_16PIX;
			break;
		case 32:
			afn_unit_inf->AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_32PIX;
			break;
		case 64:
			afn_unit_inf->AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_64PIX;
			break;
		case 128:
			afn_unit_inf->AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_128PIX;
			break;
		case 256:
			afn_unit_inf->AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_256PIX;
			break;
		default:
			BF_Debug_Print_Error(( errmsg, __func__, 1, grain_pixs->lines ));
			return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

static FJ_ERR_CODE bf_still_iip_set_stx_grain_pixs( T_IM_IIP_PARAM_STS_FILL* const sl_unit_inf, const T_IM_IIP_PARAM_AFN* const afn_unit_inf, const T_IMAGE_SIZE* const grain_pixs )
{
	const CHAR							errmsg[] = "%s: no=%u data=%lu\n";

	switch( afn_unit_inf->AFNCTL.bit.GHSZ ) {
		case E_IM_IIP_PARAM_GHSZ_32PIX:
			sl_unit_inf->BASE.SL_PHSZ.bit.PHSZ1 = 32;
			sl_unit_inf->BASE.SL_PHSZ.bit.PHSZ0 = 32;
			break;
		case E_IM_IIP_PARAM_GHSZ_64PIX:
			sl_unit_inf->BASE.SL_PHSZ.bit.PHSZ1 = 64;
			sl_unit_inf->BASE.SL_PHSZ.bit.PHSZ0 = 64;
			break;
		default:
			BF_Debug_Print_Error(( errmsg, __func__, 1, (ULONG)afn_unit_inf->AFNCTL.bit.GHSZ ));
			return FJ_ERR_NG;
	}

	sl_unit_inf->BASE.SL_PVSZ.bit.PVSZ1 = grain_pixs->lines;
	sl_unit_inf->BASE.SL_PVSZ.bit.PVSZ0 = grain_pixs->lines;

	return FJ_ERR_OK;
}

static VOID bf_still_iip_set_afn_fill_black( T_IM_IIP_PARAM_AFN* const afn_unit_inf )
{
	afn_unit_inf->CALMETHOD.bit.FILMD	= 1;
	afn_unit_inf->FILVAL0.bit.FILVALYG	= 0x0;
	afn_unit_inf->FILVAL0.bit.FILVALB	= 0x80;
	afn_unit_inf->FILVAL1.bit.FILVALR	= 0x80;
	afn_unit_inf->FILVAL1.bit.FILVALA	= 0x0;
	afn_unit_inf->CALMETHOD.bit.DANTI	= 0;
	afn_unit_inf->CALMETHOD.bit.AANTI	= 0;
}

static VOID bf_still_iip_set_stx_shift( T_IM_IIP_PARAM_STS_FILL* const sl_unit_inf, const E_BF_STILL_IMG_TYPE type )
{
	if ( type == E_BF_STILL_IMG_TYPE_RGB444 ) {
		sl_unit_inf->BASE.SFT_0.bit.SFTVAL = 4;
	}
	else {
		sl_unit_inf->BASE.SFT_0.bit.SFTVAL = 0;
	}
}

static INT32 bf_still_iip_mirror_afntbl( T_IM_IIP_PARAM_AFN_OPCOL_0* const afn_tbl, const LONG width )
{
	afn_tbl->STAXY0.bit.STAX = width - M_BF_STILL_AFN_CONV_TBL_VAL(1) - afn_tbl->STAXY0.bit.STAX;
	afn_tbl->D_OUT_IN_0.bit.DXX = -afn_tbl->D_OUT_IN_0.bit.DXX;

	return D_OS_USER_E_OK;
}

static INT32 bf_still_iip_rotate_afntbl( T_IM_IIP_PARAM_AFN_OPCOL_0* const afn_tbl, const T_BF_STILL_IMG_WINDOW_L* const crop_rect, const FJ_IMG_ROTATE_DEGREE rotate_deg )
{
	LONG afndhsz_orig;
	LONG afndvsz_orig;
	LONG dxx_orig;
	LONG dyy_orig;

	switch( rotate_deg ) {
		case FJ_IMG_ROTATE_DEGREE_0:
			// Do nothing.
			return D_OS_USER_E_OK;
		case FJ_IMG_ROTATE_DEGREE_90:
			afndhsz_orig = afn_tbl->SZ.bit.AFNDHSZ;
			afndvsz_orig = afn_tbl->SZ.bit.AFNDVSZ;
			dxx_orig = afn_tbl->D_OUT_IN_0.bit.DXX;
			dyy_orig = afn_tbl->D_OUT_IN_0.bit.DYY;
			afn_tbl->STAXY0.bit.STAY += (afndvsz_orig -1) * dyy_orig;
			afn_tbl->D_OUT_IN_0.bit.DXX = 0x0000;		// X increment X=0.0
			afn_tbl->D_OUT_IN_0.bit.DXY = -dyy_orig;	// X increment Y
			afn_tbl->D_OUT_IN_0.bit.DYX = dxx_orig;		// Y increment X
			afn_tbl->D_OUT_IN_0.bit.DYY = 0x0000;		// Y increment Y=0.0
			afn_tbl->SZ.bit.AFNDHSZ = afndvsz_orig;
			afn_tbl->SZ.bit.AFNDVSZ = afndhsz_orig;
			break;
		case FJ_IMG_ROTATE_DEGREE_180:
			afndhsz_orig = afn_tbl->SZ.bit.AFNDHSZ;
			afndvsz_orig = afn_tbl->SZ.bit.AFNDVSZ;
			dxx_orig = afn_tbl->D_OUT_IN_0.bit.DXX;
			dyy_orig = afn_tbl->D_OUT_IN_0.bit.DYY;
			afn_tbl->STAXY0.bit.STAX += (afndhsz_orig -1) * dxx_orig;
			afn_tbl->STAXY0.bit.STAY += (afndvsz_orig -1) * dyy_orig;
			afn_tbl->D_OUT_IN_0.bit.DXX = -afn_tbl->D_OUT_IN_0.bit.DXX;
			afn_tbl->D_OUT_IN_0.bit.DYY = -afn_tbl->D_OUT_IN_0.bit.DYY;
			break;
		case FJ_IMG_ROTATE_DEGREE_270:
			afndhsz_orig = afn_tbl->SZ.bit.AFNDHSZ;
			afndvsz_orig = afn_tbl->SZ.bit.AFNDVSZ;
			dxx_orig = afn_tbl->D_OUT_IN_0.bit.DXX;
			dyy_orig = afn_tbl->D_OUT_IN_0.bit.DYY;
			afn_tbl->STAXY0.bit.STAX += (afndhsz_orig -1) * dxx_orig;
			afn_tbl->D_OUT_IN_0.bit.DXX = 0x0000;		// X increment X=0.0
			afn_tbl->D_OUT_IN_0.bit.DXY = dyy_orig;		// X increment Y
			afn_tbl->D_OUT_IN_0.bit.DYX = -dxx_orig;	// Y increment X
			afn_tbl->D_OUT_IN_0.bit.DYY = 0x0000;		// Y increment Y=0.0
			afn_tbl->SZ.bit.AFNDHSZ = afndvsz_orig;
			afn_tbl->SZ.bit.AFNDVSZ = afndhsz_orig;
			break;
		default:
			return D_OS_USER_E_PAR;
	}

	return D_OS_USER_E_OK;
}



static FJ_ERR_CODE bf_still_iip_calc_afntbl( const T_BF_STILL_IMG_CONV* const src_img, const T_BF_STILL_IMG_CONV* const dst_img, const T_BF_STILL_IIP_AFN_PARAM* const afn )
{
	T_BF_STILL_IMG_CONV		out_img;
	INT32					ercd;

	if( (afn->dst_pos.sta_x == 0)
	 && (afn->dst_pos.sta_y == 0)
	 && (afn->dst_pos.width == 0)
	 && (afn->dst_pos.lines == 0)
	 ) {
		// Set dst rectangle before rotate.
		switch( afn->rotate_deg ) {
			case FJ_IMG_ROTATE_DEGREE_0:	// FALLTHROUGH
			case FJ_IMG_ROTATE_DEGREE_180:
				out_img.pixs = dst_img->pixs;
				break;
			case FJ_IMG_ROTATE_DEGREE_90:	// FALLTHROUGH
			case FJ_IMG_ROTATE_DEGREE_270:
				out_img.pixs.width = dst_img->pixs.lines;
				out_img.pixs.lines = dst_img->pixs.width;
				break;
		default:
			return FJ_ERR_NG_INPUT_PARAM;
		}
	}
	else {
		// Set dst rectangle before rotate.
		switch( afn->rotate_deg ) {
			case FJ_IMG_ROTATE_DEGREE_0:	// FALLTHROUGH
			case FJ_IMG_ROTATE_DEGREE_180:
				out_img.pixs.width = (USHORT)afn->dst_pos.width;
				out_img.pixs.lines = (USHORT)afn->dst_pos.lines;
				break;
			case FJ_IMG_ROTATE_DEGREE_90:	// FALLTHROUGH
			case FJ_IMG_ROTATE_DEGREE_270:
				// Set dst rectangle before rotate.
				out_img.pixs.width = (USHORT)afn->dst_pos.lines;
				out_img.pixs.lines = (USHORT)afn->dst_pos.width;
				break;
		default:
			return FJ_ERR_NG_INPUT_PARAM;
		}
	}

	// Calc afn table before rotate.
	ercd = BF_Still_ImgInfo_Calc_ResizeRectAfnTbl( afn->afn_tbl, &afn->crop_rect, &out_img, src_img, afn->nearest_neighbor );
	if( ercd != D_OS_USER_E_OK ) {
		return FJ_ERR_NG;
	}

	// Mirror afn table.
	if( afn->mirror != FALSE ) {
		ercd = bf_still_iip_mirror_afntbl( afn->afn_tbl, afn->crop_rect.width );
		if( ercd != D_OS_USER_E_OK ) {
			return FJ_ERR_NG;
		}
	}

	// Rotate afn table.
	ercd = bf_still_iip_rotate_afntbl( afn->afn_tbl, &afn->crop_rect, afn->rotate_deg );
	if( ercd != D_OS_USER_E_OK ) {
		return FJ_ERR_NG;
	}

#if defined(CO_BF_STILL_IIP_DBG_DUMP) || defined(CO_BF_STILL_SUBR_DBG_AFN_SPLIT)
	BF_Debug_Print_Information(( "%s(): AFNTBL STA=(%.1f, %.1f) PIT=(%.1f, %.1f) RECT=(%lu, %lu)\n",
				__func__,
				M_BF_STILL_AFN_CONV_FLORT_VAL( afn->afn_tbl->STAXY0.bit.STAX ),    M_BF_STILL_AFN_CONV_FLORT_VAL( afn->afn_tbl->STAXY0.bit.STAY ),
				M_BF_STILL_AFN_CONV_FLORT_VAL( afn->afn_tbl->D_OUT_IN_0.bit.DXX ), M_BF_STILL_AFN_CONV_FLORT_VAL( afn->afn_tbl->D_OUT_IN_0.bit.DYY ),
				afn->afn_tbl->SZ.bit.AFNDHSZ,                                      afn->afn_tbl->SZ.bit.AFNDVSZ
				));

	BF_Debug_Print_Information(( "%s(): AFNTBL STA=(0x%lx, 0x%lx) PIT=(0x%lx, 0x%lx) RECT=(0x%lx, 0x%lx) DXY=0x%lx DYX=0x%lx\n",
				__func__,
				afn->afn_tbl->STAXY0.bit.STAX,    afn->afn_tbl->STAXY0.bit.STAY,
				afn->afn_tbl->D_OUT_IN_0.bit.DXX, afn->afn_tbl->D_OUT_IN_0.bit.DYY,
				afn->afn_tbl->SZ.bit.AFNDHSZ,     afn->afn_tbl->SZ.bit.AFNDVSZ,
				afn->afn_tbl->D_OUT_IN_0.bit.DXY, afn->afn_tbl->D_OUT_IN_0.bit.DYX
				));
#endif // CO_BF_STILL_IIP_DBG_DUMP

	return FJ_ERR_OK;
}

static FJ_ERR_CODE bf_still_iip_calc_afntbl_cbcronly( const T_BF_STILL_IMG* const src_img, const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IIP_AFN_PARAM* const afn )
{
	T_BF_STILL_IMG			out_img;
	INT32					ercd;

	if( (afn->dst_pos.sta_x == 0)
	 && (afn->dst_pos.sta_y == 0)
	 && (afn->dst_pos.width == 0)
	 && (afn->dst_pos.lines == 0)
	 ) {
		ercd = BF_Still_ImgInfo_Calc_ResizeRectAfnTbl_CbCrOnly( afn->afn_tbl, &afn->crop_rect, dst_img, src_img );
		if( ercd != D_OS_USER_E_OK ) {
			return FJ_ERR_NG;
		}
	}
	else {
		out_img.pixs.width = (USHORT)afn->dst_pos.width;
		out_img.pixs.lines = (USHORT)afn->dst_pos.lines;
		ercd = BF_Still_ImgInfo_Calc_ResizeRectAfnTbl_CbCrOnly( afn->afn_tbl, &afn->crop_rect, &out_img, src_img );
		if( ercd != D_OS_USER_E_OK ) {
			return FJ_ERR_NG;
		}
	}

#ifdef CO_BF_STILL_IIP_DBG_DUMP
	BF_Debug_Print_Information(( "%s(): AFNTBL STA=(0x%lx, 0x%lx) PIT=(0x%lx, 0x%lx) RECT=(0x%lx, 0x%lx) DXY=0x%lx DYX=0x%lx\n",
				__func__,
				afn->afn_tbl->STAXY0.bit.STAX,    afn->afn_tbl->STAXY0.bit.STAY,
				afn->afn_tbl->D_OUT_IN_0.bit.DXX, afn->afn_tbl->D_OUT_IN_0.bit.DYY,
				afn->afn_tbl->SZ.bit.AFNDHSZ,     afn->afn_tbl->SZ.bit.AFNDVSZ,
				afn->afn_tbl->D_OUT_IN_0.bit.DXY, afn->afn_tbl->D_OUT_IN_0.bit.DYX
				));
#endif // CO_BF_STILL_IIP_DBG_DUMP

	return FJ_ERR_OK;
}

static FJ_ERR_CODE bf_still_iip_set_pfsp12( T_IM_IIP_PARAM_AFN* const afn_unit_inf, const FJ_IMG_ROTATE_DEGREE rotate_deg )
{
	switch( rotate_deg ) {
		case FJ_IMG_ROTATE_DEGREE_0:	// FALLTHROUGH
		case FJ_IMG_ROTATE_DEGREE_180:
			// nothing to do.
			break;
		case FJ_IMG_ROTATE_DEGREE_90:	// FALLTHROUGH
		case FJ_IMG_ROTATE_DEGREE_270:
			afn_unit_inf->PFCTL.bit.PFSP1 = E_IM_IIP_PARAM_PFSP_1;
			afn_unit_inf->PFCTL.bit.PFSP2 = E_IM_IIP_PARAM_PFSP_1;
			break;
	default:
		return FJ_ERR_NG_INPUT_PARAM;
	}

	return FJ_ERR_OK;
}

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

VOID BF_Still_Iip_Int_Handler( ULONG interrupt_flag_mask )
{
	T_IM_IIP_AXI_STATUS sts;
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_AXIERR) != 0 ) {
		Im_IIP_Get_AXI_Status( &sts );
		BF_Debug_Print_Error(( "BF_Still_Iip_Int_Handler: AXI Error. AXRSTS=0x%lx AXWSTS=0x%lx\n",
					sts.read_channel_status,
					sts.write_channel_status
					));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL2_LINEEND) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_FRECT, FJ_MBALOG_TYPE_POINT, "IIP int SL2 start" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL3_LINEEND) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_FRECT, FJ_MBALOG_TYPE_POINT, "IIP int SL3 start" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL4_LINEEND) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_FRECT, FJ_MBALOG_TYPE_POINT, "IIP int SL4 start" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL5_LINEEND) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_FRECT, FJ_MBALOG_TYPE_POINT, "IIP int SL5 start" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL6_LINEEND) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_FRECT, FJ_MBALOG_TYPE_POINT, "IIP int SL6 start" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL0END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL0END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL1END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL1END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL2END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL2END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL3END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL3END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL4END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL4END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL5END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL5END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL6END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL6END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL7END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL7END, FJ_MBALOG_TYPE_POINT, "" ));
	}
	if ( (interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL8END) != 0 ) {
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_IIP_SL8END, FJ_MBALOG_TYPE_POINT, "" ));
	}
}

INT32 BF_Still_Iip_Open( const T_BF_STILL_IIP_RESOURCE* const res )
{
	INT32						ercd;

	Dd_ARM_Dmb_Pou();

	ercd = Im_IIP_Open_SWTRG( res->unitid_bitmask64, res->pixid_bitmask, res->open_res_bitmask, res->wait_time );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( "%s: ercd = 0x%x unit=0x%llx pixid=0x%x open_res=0x%x\n", __func__, ercd, res->unitid_bitmask64, res->pixid_bitmask, res->open_res_bitmask ));
	}

	return ercd;
}

INT32 BF_Still_Iip_Start( const T_BF_STILL_IIP_RESOURCE* const res, const E_BF_STILL_IIP_LINK b2y_iip_link_onoff )
{
	INT32						ercd;
	const CHAR					errmsg[] = "BF_Still_Iip_Start: ercd = 0x%x unitid=%u\n";

	UINT32 iip_link_onoff;

	if( b2y_iip_link_onoff == E_BF_STILL_IIP_LINK_OFF ) {
		iip_link_onoff = D_IM_IIP_ENABLE_OFF;
	}
	else {
		iip_link_onoff = D_IM_IIP_ENABLE_ON;
	}

	ercd = Im_IIP_LinkStart_SWTRG( res->dst_unitid, D_IM_IIP_SWTRG_ON, iip_link_onoff );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, ercd, res->dst_unitid ));
		return ercd;
	}

	ercd = Im_IIP_LinkStart_SWTRG( res->src_unitid, D_IM_IIP_SWTRG_ON, iip_link_onoff );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, ercd, res->src_unitid ));
		return ercd;
	}

	Dd_ARM_Dsb_Pou();

	return ercd;
}

INT32 BF_Still_Iip_WaitEnd( const T_BF_STILL_IIP_RESOURCE* const res )
{
	INT32						ercd;
	UINT32						wait_factor_result = 0;
	OS_USER_SYSTIM				cur_time;
	static OS_USER_SYSTIM		last_time;
	static UINT32				err_count = 0;
	const CHAR					errmsg[] = "BF_Still_Iip_WaitEnd: ercd = 0x%x factor=0x%x\n";

#ifdef DEBUG_ON_PC
	OS_User_Dly_Tsk( res->wait_time /100 );
	return 0;
#endif

	ercd = Im_IIP_Wait_End( &wait_factor_result, res->wait_factor, D_IM_IIP_OR_WAIT, res->wait_time );
	if( ercd != D_IM_IIP_OK ) {
		OS_User_Get_Tim( &cur_time );

		if( ((err_count % 200) == 0)
		 || (cur_time < last_time)									// time counter rollover
		 || ((cur_time - last_time) >= 1000)						// over 1000msec
		 ) {
			BF_Debug_Print_Error(( errmsg, ercd, wait_factor_result ));
			BF_Debug_Print_Error(( "error count=%u\n", err_count ));
			BF_Still_Iip_Check_Cache_Access();
			M_MBA_END();
//			BF_Debug_Print_Warning(( "**** MBA OUT\n" ));
//			M_MBA_OUTPUT( E_MBALOG_OUT_FILE, "I:\\cap_err.mba" );
//			OS_User_Dly_Tsk( 1000 );
		}
		err_count++;
		last_time = cur_time;
		return ercd;
	}

	return ercd;
}

INT32 BF_Still_Iip_Close( const T_BF_STILL_IIP_RESOURCE* const res )
{
	INT32						ercd = D_IM_IIP_OK;

	ercd = Im_IIP_Close_SWTRG( res->unitid_bitmask64, res->pixid_bitmask, res->open_res_bitmask );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( "%s: ercd = 0x%x unit=0x%llx pixid=0x%x open_res=0x%x\n", __func__, ercd, res->unitid_bitmask64, res->pixid_bitmask, res->open_res_bitmask ));
		if( ercd == D_IM_IIP_ABORT_FAILED_ERR ) {
		}
		return ercd;
	}

	return ercd;
}

// for T_BF_STILL_IIP_RESOURCE2
INT32 BF_Still_Iip_Open2( const T_BF_STILL_IIP_RESOURCE2* const res )
{
	INT32						ercd;

	Dd_ARM_Dmb_Pou();

	ercd = Im_IIP_Open_SWTRG( res->unitid_bitmask64, res->pixid_bitmask, res->open_res_bitmask, res->wait_time );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( "%s: ercd = 0x%x unit=0x%llx pixid=0x%x open_res=0x%x\n", __func__, ercd, res->unitid_bitmask64, res->pixid_bitmask, res->open_res_bitmask ));
	}

	return ercd;
}

// for T_BF_STILL_IIP_RESOURCE2
INT32 BF_Still_Iip_Start2( const T_BF_STILL_IIP_RESOURCE2* const res )
{
	INT32						ercd;
	const CHAR					errmsg[] = "BF_Still_Iip_Start2: ercd = 0x%x unitid=%u\n";

	ercd = Im_IIP_Start_SWTRG( res->dst_unitid, D_IM_IIP_SWTRG_ON );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, ercd, res->dst_unitid ));
		return ercd;
	}

	ercd = Im_IIP_Start_SWTRG( res->filter_unitid, D_IM_IIP_SWTRG_ON );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, ercd, res->filter_unitid ));
		return ercd;
	}

	if ( res->src_unitid[1] != E_IM_IIP_UNIT_ID_MAX ) {
		ercd = Im_IIP_Start_SWTRG( res->src_unitid[1], D_IM_IIP_SWTRG_ON );
		if( ercd != D_IM_IIP_OK ) {
			BF_Debug_Print_Error(( errmsg, ercd, res->src_unitid[1] ));
			return ercd;
		}
	}

	ercd = Im_IIP_Start_SWTRG( res->src_unitid[0], D_IM_IIP_SWTRG_ON );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, ercd, res->src_unitid[0] ));
		return ercd;
	}

	Dd_ARM_Dsb_Pou();

	return ercd;
}

// for T_BF_STILL_IIP_RESOURCE2
INT32 BF_Still_Iip_WaitEnd2( const T_BF_STILL_IIP_RESOURCE2* const res )
{
	INT32						ercd;
	UINT32						wait_factor_result = 0;
	OS_USER_SYSTIM				cur_time;
	static OS_USER_SYSTIM		last_time;
	static UINT32				err_count = 0;
	const CHAR					errmsg[] = "BF_Still_Iip_WaitEnd: ercd = 0x%x factor=0x%x\n";

#ifdef DEBUG_ON_PC
	OS_User_Dly_Tsk( res->wait_time /100 );
	return 0;
#endif

	ercd = Im_IIP_Wait_End( &wait_factor_result, res->wait_factor, D_IM_IIP_OR_WAIT, res->wait_time );
	if( ercd != D_IM_IIP_OK ) {
		OS_User_Get_Tim( &cur_time );

		if( ((err_count % 200) == 0)
		 || (cur_time < last_time)									// time counter rollover
		 || ((cur_time - last_time) >= 1000)						// over 1000msec
		 ) {
			BF_Debug_Print_Error(( errmsg, ercd, wait_factor_result ));
			BF_Debug_Print_Error(( "error count=%u\n", err_count ));
			BF_Still_Iip_Check_Cache_Access();
			M_MBA_END();
//			BF_Debug_Print_Warning(( "**** MBA OUT\n" ));
//			M_MBA_OUTPUT( E_MBALOG_OUT_FILE, "I:\\cap_err.mba" );
//			OS_User_Dly_Tsk( 1000 );
		}
		err_count++;
		last_time = cur_time;
		return ercd;
	}

	return ercd;
}

// for T_BF_STILL_IIP_RESOURCE2
INT32 BF_Still_Iip_Close2( const T_BF_STILL_IIP_RESOURCE2* const res )
{
	INT32						ercd = D_IM_IIP_OK;

	ercd = Im_IIP_Close_SWTRG( res->unitid_bitmask64, res->pixid_bitmask, res->open_res_bitmask );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( "%s: ercd = 0x%x unit=0x%llx pixid=0x%x open_res=0x%x\n", __func__, ercd, res->unitid_bitmask64, res->pixid_bitmask, res->open_res_bitmask ));
		if( ercd == D_IM_IIP_ABORT_FAILED_ERR ) {
		}
		return ercd;
	}

	return ercd;
}

VOID BF_Still_Iip_Set_ResizeRotate( const T_BF_STILL_IMG* const src_img, const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IIP_RESOURCE_RR* const res_rr, T_IM_IIP_UTIL_RR* const iip_util_rr )
{
	// Check combination ImageCache, MasterIF and StoreLinkUnit No.
	bf_still_iip_check_res_combi_rr( res_rr );

	*iip_util_rr = g_still_iip_param_util_rr_base;

	iip_util_rr->afn_unitid = res_rr->src_unitid;
	iip_util_rr->sl_unitid = res_rr->dst_unitid;
	iip_util_rr->src_pixid = res_rr->src_pixid;
	iip_util_rr->dst_pixid = res_rr->dst_pixid;
	iip_util_rr->afn_cache_select = res_rr->src_cache_sel;

	iip_util_rr->src.rect.top = (DOUBLE)0.0;
	iip_util_rr->src.rect.left = (DOUBLE)0.0;
	iip_util_rr->src.rect.width = src_img->pixs.width;
	iip_util_rr->src.rect.lines = src_img->pixs.lines;
	switch( src_img->type ) {
		case E_BF_STILL_IMG_TYPE_YCC420:
			iip_util_rr->src.gbl.pix_format = E_IM_IIP_PFMT_YCC420;
			break;
		case E_BF_STILL_IMG_TYPE_YCC422:
			iip_util_rr->src.gbl.pix_format = E_IM_IIP_PFMT_YCC422;
			break;
		case E_BF_STILL_IMG_TYPE_RGB888:	// FALLTHROUGH
		case E_BF_STILL_IMG_TYPE_YCC444:
			iip_util_rr->src.gbl.pix_format = E_IM_IIP_PFMT_YCC444;
			break;
		default:
			BF_Debug_Print_Warning(( "BF_Still_Iip_Set_ResizeRotate: unknown parameter. use YCC420.\n" ));
			iip_util_rr->src.gbl.pix_format = E_IM_IIP_PFMT_YCC420;
			break;
	}
	switch( src_img->plane ) {
		case E_BF_STILL_PLANE_TYPE_C_LINEAR:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_PLANAR:	// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_PLANAR:
			iip_util_rr->src.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_VIDEO:
			iip_util_rr->src.gbl.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR;
			iip_util_rr->src.gbl.pix_format = E_IM_IIP_PFMT_VIDEO;
			break;
		case E_BF_STILL_PLANE_TYPE_TILE:
			BF_Debug_Print_Warning(( "BF_Still_Iip_Set_ResizeRotate: Unsupported image format. use PLANE.\n" ));
			iip_util_rr->src.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_C_INTL:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_C_INTL:
			iip_util_rr->src.gbl.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR;
			break;
		case E_BF_STILL_PLANE_TYPE_1PLANE:
			iip_util_rr->src.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
			iip_util_rr->src.gbl.pix_format = E_IM_IIP_PFMT_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_CHUNKY:
			iip_util_rr->src.gbl.frame_type = E_IM_IIP_FTYPE_CHUNKY;
			break;
		default:
			BF_Debug_Print_Warning(( "BF_Still_Iip_Set_ResizeRotate: unknown parameter. use PLANAR.\n" ));
			iip_util_rr->src.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
			break;
	}
	iip_util_rr->src.gbl.width = src_img->pixs.width;
	iip_util_rr->src.gbl.lines = src_img->pixs.lines;
	iip_util_rr->src.gbl.line_bytes.Y_G = src_img->g_width.y;
	iip_util_rr->src.gbl.line_bytes.Cb_B = src_img->g_width.cb;
	iip_util_rr->src.gbl.line_bytes.Cr_R = src_img->g_width.cr;
	iip_util_rr->src.gbl.line_bytes.Alpha = src_img->g_width.y;	// dummy
	iip_util_rr->src.gbl.addr.Y_G = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(src_img->addr.y);
	iip_util_rr->src.gbl.addr.Cb_B = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(src_img->addr.cb);
	iip_util_rr->src.gbl.addr.Cr_R = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(src_img->addr.cr);
	iip_util_rr->src.gbl.addr.Alpha = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(src_img->addr.y);	// dummy
	iip_util_rr->src.gbl.masterIF_Y_G = res_rr->src_master_if;
	iip_util_rr->src.gbl.masterIF_Cb_B = res_rr->src_master_if;
	iip_util_rr->src.gbl.masterIF_Cr_R = res_rr->src_master_if;
	iip_util_rr->src.gbl.masterIF_Alpha = res_rr->src_master_if;

	iip_util_rr->dst.rect.top = (DOUBLE)0.0;
	iip_util_rr->dst.rect.left = (DOUBLE)0.0;
	iip_util_rr->dst.rect.width = dst_img->pixs.width;
	iip_util_rr->dst.rect.lines = dst_img->pixs.lines;
	switch( dst_img->type ) {
		case E_BF_STILL_IMG_TYPE_YCC420:
			iip_util_rr->dst.gbl.pix_format = E_IM_IIP_PFMT_YCC420;
			break;
		case E_BF_STILL_IMG_TYPE_YCC422:
			iip_util_rr->dst.gbl.pix_format = E_IM_IIP_PFMT_YCC422;
			break;
		case E_BF_STILL_IMG_TYPE_RGB888:	// FALLTHROUGH
		case E_BF_STILL_IMG_TYPE_YCC444:
			iip_util_rr->dst.gbl.pix_format = E_IM_IIP_PFMT_YCC444;
			break;
		default:
			BF_Debug_Print_Warning(( "BF_Still_Iip_Set_ResizeRotate: unknown parameter. use YCC420.\n" ));
			iip_util_rr->dst.gbl.pix_format = E_IM_IIP_PFMT_YCC420;
			break;
	}
	switch( dst_img->plane ) {
		case E_BF_STILL_PLANE_TYPE_C_LINEAR:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_PLANAR:	// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_PLANAR:
			iip_util_rr->dst.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_VIDEO:
			iip_util_rr->dst.gbl.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR;
			iip_util_rr->dst.gbl.pix_format = E_IM_IIP_PFMT_VIDEO;
			break;
		case E_BF_STILL_PLANE_TYPE_TILE:
			BF_Debug_Print_Warning(( "BF_Still_Iip_Set_ResizeRotate: Unsupported image format. use PLANE.\n" ));
			iip_util_rr->dst.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_C_INTL:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_C_INTL:
			iip_util_rr->dst.gbl.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR;
			break;
		case E_BF_STILL_PLANE_TYPE_1PLANE:
			iip_util_rr->dst.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
			iip_util_rr->dst.gbl.pix_format = E_IM_IIP_PFMT_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_CHUNKY:
			iip_util_rr->dst.gbl.frame_type = E_IM_IIP_FTYPE_CHUNKY;
			break;
		default:
			BF_Debug_Print_Warning(( "BF_Still_Iip_Set_ResizeRotate: unknown parameter. use PLANAR.\n" ));
			iip_util_rr->dst.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
			break;
	}
	iip_util_rr->dst.gbl.width = dst_img->pixs.width;
	iip_util_rr->dst.gbl.lines = dst_img->pixs.lines;
	iip_util_rr->dst.gbl.line_bytes.Y_G = dst_img->g_width.y;
	iip_util_rr->dst.gbl.line_bytes.Cb_B = dst_img->g_width.cb;
	iip_util_rr->dst.gbl.line_bytes.Cr_R = dst_img->g_width.cr;
	iip_util_rr->dst.gbl.line_bytes.Alpha = dst_img->g_width.y;	// dummy
	iip_util_rr->dst.gbl.addr.Y_G = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(dst_img->addr.y);
	iip_util_rr->dst.gbl.addr.Cb_B = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(dst_img->addr.cb);
	iip_util_rr->dst.gbl.addr.Cr_R = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(dst_img->addr.cr);
	iip_util_rr->dst.gbl.addr.Alpha = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(dst_img->addr.y);	// dummy
	iip_util_rr->src.gbl.masterIF_Y_G = res_rr->dst_master_if;
	iip_util_rr->src.gbl.masterIF_Cb_B = res_rr->dst_master_if;
	iip_util_rr->src.gbl.masterIF_Cr_R = res_rr->dst_master_if;
	iip_util_rr->src.gbl.masterIF_Alpha = res_rr->dst_master_if;
}


FJ_ERR_CODE BF_Still_Iip_Set_Afn( const T_BF_STILL_IMG_CONV* const src_img, const T_BF_STILL_IMG_CONV* const dst_img, const T_BF_STILL_IIP_RESOURCE* const res, const T_BF_STILL_IIP_AFN_PARAM* const afn )
{
	INT32								ercd;
	FJ_ERR_CODE							fj_ercd;
	T_IM_IIP_PIXFMTTBL					pixfmt_tbl_0;		// AFN Unit input.
	T_IM_IIP_PIXFMTTBL					pixfmt_tbl_1;		// SL Unit output.
	T_IM_IIP_UNIT_CFG					afn_cfg;
	T_IM_IIP_UNIT_CFG					sl_cfg;
	T_IM_IIP_PARAM_AFN*					afn_unit_inf;
	T_IM_IIP_PARAM_STS_FILL*			sl_unit_inf;
	const CHAR							errmsg[] = "%s: no=%u ercd=%d\n";


	// Check combination ImageCache, MasterIF and StoreLinkUnit No.
	bf_still_iip_check_res_combi( res );

	ercd = BF_Still_ImgInfo_Set_IipPixFmtTblRgb( &pixfmt_tbl_0, src_img, res->src_master_if );
	if( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 1, ercd ));
		return FJ_ERR_NG;
	}

	ercd = BF_Still_ImgInfo_Set_IipPixFmtTblRgb( &pixfmt_tbl_1, dst_img, res->dst_master_if );
	if( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 2, ercd ));
		return FJ_ERR_NG;
	}


	// Create AFN table.
	fj_ercd = bf_still_iip_calc_afntbl( src_img, dst_img, afn );
	if( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 3, ercd ));
		return fj_ercd;
	}


	// Create AFN Unit Inf.
	afn_unit_inf							= afn->afn_unit_inf;
	*afn_unit_inf							= gbf_still_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF	= res->dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID		= res->src_pixid;
	afn_unit_inf->AFNPA.word				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)(afn->afn_tbl) );
	afn_unit_inf->PIXIDEF.bit.CSEL			= res->src_cache_sel;
	afn_unit_inf->PFCTL.bit.PFOFF			= afn->prefetch_off;


	fj_ercd = bf_still_iip_set_pfsp12( afn_unit_inf, afn->rotate_deg );
	if( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 4, ercd ));
		return fj_ercd;
	}

	// Set Interpolation Mode
	fj_ercd = bf_still_iip_set_afn_bicubic( afn_unit_inf, res->open_res_bitmask, res->src_unitid, afn->nearest_neighbor );
	if( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 5, ercd ));
		return fj_ercd;
	}

	// Set to use fill function by AFN unit.
	if ( afn->fil_md ) {
		bf_still_iip_set_afn_fill_black( afn_unit_inf );
	}

	fj_ercd = bf_still_iip_set_afn_grain_pixs( afn_unit_inf, &afn->grain_pixs );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}


	// Set AFN Unit conifg (Driver parameter).
	afn_cfg.unit_ctrl				= D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable			= D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr			= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)afn_unit_inf );
	afn_cfg.load_unit_param_flag	= 0;


	// Create STX Unit Inf.
	sl_unit_inf									= afn->sl_unit_inf;
	*sl_unit_inf								= gbf_still_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF	= res->src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID		= res->dst_pixid;
	bf_still_iip_set_stx_shift( sl_unit_inf, dst_img->type );

	fj_ercd = bf_still_iip_set_stx_grain_pixs( sl_unit_inf, afn_unit_inf, &afn->grain_pixs );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}


	bf_still_iip_set_stx_clip( dst_img->depth, sl_unit_inf );


	// Set STX Unit conifg (Driver parameter)
	sl_cfg.unit_ctrl			= D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable			= D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr		= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)sl_unit_inf );
	sl_cfg.load_unit_param_flag	= res->unitid_bitmask64;


	fj_ercd = bf_still_iip_set_afn_dst_pos( dst_img, &afn->dst_pos, afn_unit_inf, sl_unit_inf );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	if( (afn->alignment.lines > 1) || (afn->alignment.width > 1) ) {
		bf_still_iip_set_afn_alignment( &afn->alignment, afn->afn_tbl, sl_unit_inf );
	}

	// Data cache clean.
	BF_Still_Dcache_Clean( afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	BF_Still_Dcache_Clean( sl_unit_inf,  sizeof(T_IM_IIP_PARAM_STS_FILL) );
	BF_Still_Dcache_Clean( afn->afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );

	// Set parameter to register.
	ercd = Im_IIP_Ctrl_PIXFMTTBL( res->src_pixid, &pixfmt_tbl_0 );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 6, ercd ));
		return FJ_ERR_NG;
	}

	ercd = Im_IIP_Ctrl_PIXFMTTBL( res->dst_pixid, &pixfmt_tbl_1 );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 7, ercd ));
		return FJ_ERR_NG;
	}

	ercd = Im_IIP_Ctrl_SWTRG_Unit( res->src_unitid, &afn_cfg );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 8, ercd ));
		return FJ_ERR_NG;
	}

	ercd = Im_IIP_Ctrl_SWTRG_Unit( res->dst_unitid, &sl_cfg );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 9, ercd ));
		return FJ_ERR_NG;
	}

	ercd = Im_IIP_Set_Interrupt( res->wait_factor | D_BF_STILL_IIP_INT_FACTOR_ERR, D_IM_IIP_ENABLE_ON );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 10, ercd ));
		return FJ_ERR_NG;
	}

	ercd = bf_still_iip_set_lineend_intr( res->wait_factor );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 11, ercd ));
		return FJ_ERR_NG;
	}

	ercd = Im_IIP_Set_IntHandler( res->wait_factor | D_BF_STILL_IIP_INT_FACTOR_ERR, bf_still_iip_int_handler );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 12, ercd ));
		return FJ_ERR_NG;
	}

	BF_Still_Iip_Dump_UnitParam( "%s(): unit param\n", __func__, res, &g_still_iip_afn_unit_dump_tid );

	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Still_Iip_Set_Afn_CbCrOnly( const T_BF_STILL_IMG* const src_img, const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IIP_RESOURCE* const res, const T_BF_STILL_IIP_AFN_PARAM* const afn )
{
	INT32								ercd;
	FJ_ERR_CODE							fj_ercd;
	T_IM_IIP_PIXFMTTBL					pixfmt_tbl_0;		// AFN Unit input.
	T_IM_IIP_PIXFMTTBL					pixfmt_tbl_1;		// SL Unit output.
	T_IM_IIP_UNIT_CFG					afn_cfg;
	T_IM_IIP_UNIT_CFG					sl_cfg;
	T_IM_IIP_PARAM_AFN*					afn_unit_inf;
	T_IM_IIP_PARAM_STS_FILL*			sl_unit_inf;
	const CHAR							errmsg[] = "%s: no=%u ercd=%d\n";

	// Check combination ImageCache, MasterIF and StoreLinkUnit No.
	bf_still_iip_check_res_combi( res );


	// Set pixfmttbl structure.
	ercd = BF_Still_ImgInfo_Set_IipPixFmtTblCbCr( &pixfmt_tbl_0, src_img, res->src_master_if );
	if( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 1, ercd ));
		return FJ_ERR_NG;
	}

	ercd = BF_Still_ImgInfo_Set_IipPixFmtTblCbCr( &pixfmt_tbl_1, dst_img, res->dst_master_if );
	if( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 2, ercd ));
		return FJ_ERR_NG;
	}


	// Create AFN table.
	fj_ercd = bf_still_iip_calc_afntbl_cbcronly( src_img, dst_img, afn );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}


	// Create AFN Unit Inf.
	afn_unit_inf = afn->afn_unit_inf;
	*afn_unit_inf = gbf_still_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF = res->dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID = res->src_pixid;
	afn_unit_inf->AFNPA.word = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)(afn->afn_tbl) );/* pgr1223 */
	afn_unit_inf->PIXIDEF.bit.CSEL = res->src_cache_sel;
	afn_unit_inf->PFCTL.bit.PFOFF = afn->prefetch_off;


	fj_ercd = bf_still_iip_set_afn_grain_pixs( afn_unit_inf, &afn->grain_pixs );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}


	// Set AFN Unit conifg (Driver parameter).
	afn_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)afn_unit_inf );/* pgr1223 */
	afn_cfg.load_unit_param_flag = 0;


	// Create STX Unit Inf.
	sl_unit_inf = afn->sl_unit_inf;
	*sl_unit_inf = gbf_still_iip_param_sts_base;
	sl_unit_inf->BASE.SFT_0.bit.SFTVAL = 0;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = res->src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = res->dst_pixid;

	fj_ercd = bf_still_iip_set_stx_grain_pixs( sl_unit_inf, afn_unit_inf, &afn->grain_pixs );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	bf_still_iip_set_stx_clip( dst_img->depth, sl_unit_inf );

	if( afn->mask_enable != FALSE ) {
		bf_still_iip_set_stx_mask_cbcronly( sl_unit_inf, &afn->mask_ctrl, dst_img->type );
	}


	// Set STX Unit conifg (Driver parameter)
	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)sl_unit_inf );/* pgr1223 */
	sl_cfg.load_unit_param_flag = res->unitid_bitmask64;

	fj_ercd = bf_still_iip_set_afn_dst_pos_cbcronly( dst_img, &afn->dst_pos, afn_unit_inf, sl_unit_inf );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}


	// Data cache clean.
	BF_Still_Dcache_Clean( afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	BF_Still_Dcache_Clean( sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS_FILL) );
	BF_Still_Dcache_Clean( afn->afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );


	// Set parameter to register.
	ercd = Im_IIP_Ctrl_PIXFMTTBL( res->src_pixid, &pixfmt_tbl_0 );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 3, ercd ));
		return FJ_ERR_NG;
	}

	ercd = Im_IIP_Ctrl_PIXFMTTBL( res->dst_pixid, &pixfmt_tbl_1 );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 4, ercd ));
		return FJ_ERR_NG;
	}

	ercd = Im_IIP_Ctrl_SWTRG_Unit( res->src_unitid, &afn_cfg );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 5, ercd ));
		return FJ_ERR_NG;
	}

	ercd = Im_IIP_Ctrl_SWTRG_Unit( res->dst_unitid, &sl_cfg );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 6, ercd ));
		return FJ_ERR_NG;
	}

	ercd = Im_IIP_Set_Interrupt( res->wait_factor | D_BF_STILL_IIP_INT_FACTOR_ERR, D_IM_IIP_ENABLE_ON );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 7, ercd ));
		return FJ_ERR_NG;
	}

	ercd = Im_IIP_Set_IntHandler( D_BF_STILL_IIP_INT_FACTOR_ERR, BF_Still_Iip_Int_Handler );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, __func__, 8, ercd ));
		return FJ_ERR_NG;
	}

	BF_Still_Iip_Dump_UnitParam( "%s(): unit param\n", __func__, res, &g_still_iip_cbcr_afn_unit_dump_tid );

	return FJ_ERR_OK;
}

// Append CSC Unit after AFN.
FJ_ERR_CODE BF_Still_Iip_Append_AfnCsc( const T_BF_STILL_IIP_RESOURCE2* const res2, const T_BF_STILL_IIP_AFN_PARAM* const afn, const T_BF_STILL_IIP_CSC_PARAM* const csc )
{
	T_IM_IIP_UNIT_CFG			csc_cfg;
	T_IM_IIP_PARAM_AFN*			afn_unit_inf;
	T_IM_IIP_PARAM_CSC*			csc_unit_inf;
	T_IM_IIP_PARAM_STS_FILL*	sl_unit_inf;
	INT32						ercd;
	const CHAR					errmsg[] = "BF_Still_Iip_Append_AfnCsc: no=%u ercd=%d\n";

	// Set CSC Unitinf parameter.
	csc_unit_inf								= csc->csc_unit_inf;
	*csc_unit_inf								= g_still_iip_param_csc_base;
	csc_unit_inf->CSCTOPCNF.bit.DATACONF		= res2->src_portid[0];
	csc_unit_inf->CSCTOPCNF.bit.WAITCONF		= res2->dst_portid;
	csc_unit_inf->CSCK[0]						= csc->csck[0];
	csc_unit_inf->CSCK[1]						= csc->csck[1];
	csc_unit_inf->CSCK[2]						= csc->csck[2];
	csc_unit_inf->CSCK[3]						= csc->csck[3];
	csc_unit_inf->MDSEL.bit.MDSEL				= csc->mdsel;
	csc_unit_inf->ALPSEL.bit.ALPSEL				= csc->alpha_in_sel;
	csc_unit_inf->ALPSEL.bit.ALOSEL				= csc->alpha_out_sel;
	csc_unit_inf->ALPVAL.bit.ALPVAL				= csc->alpha_val;

	csc_cfg.unit_ctrl							= D_IM_IIP_HW_CTRL_SWTRG;
	csc_cfg.chain_enable						= D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	csc_cfg.unit_param_addr						= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)csc_unit_inf );
	csc_cfg.load_unit_param_flag				= 0;

	BF_Still_Dcache_Clean( csc_unit_inf, sizeof(T_IM_IIP_PARAM_CSC) );

	// Append CSC Unit between AFN Unit and SL Unit.
	afn_unit_inf								= afn->afn_unit_inf;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF		= res2->filter_portid[0];
	sl_unit_inf									= afn->sl_unit_inf;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF	= res2->filter_portid[0];

	// Note: PLDUNIT register set by BF_Still_Iip_Set_Afn().
	BF_Still_Dcache_Clean( afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	BF_Still_Dcache_Clean( sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS_FILL) );

	ercd = Im_IIP_Ctrl_SWTRG_Unit( res2->filter_unitid, &csc_cfg );
	if( ercd != D_IM_IIP_OK ) {
		BF_Debug_Print_Error(( errmsg, 1, ercd ));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


// delete_basefw\fj\iq
//FJ_ERR_CODE BF_Still_Iip_Calc_Vis( const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IIP_VIS_RECT* const vis_rect, const T_BF_IQ_FRECT_TBL_SET* const frect_tbl_set, T_BF_STILL_IIP_FRECT_OUTPOS_SET* const outpos_set )
//{
//	const CHAR							errmsg[] = "BF_Still_Iip_Calc_Vis: no=%u ercd=%d\n";
//	T_BF_STILL_IIP_VIS_RECT				vis_rect_rounddown;

//	vis_rect_rounddown = *vis_rect;
//	vis_rect_rounddown.sta_x = M_BF_STILL_ROUNDDOWN_2(vis_rect_rounddown.sta_x);	// for YCC420 and YCC422
//	vis_rect_rounddown.sta_y = M_BF_STILL_ROUNDDOWN_2(vis_rect_rounddown.sta_y);	// for YCC420
//	vis_rect_rounddown.width = M_BF_STILL_ROUNDDOWN_2(vis_rect_rounddown.width);	// for YCC420 and YCC422
//	vis_rect_rounddown.lines = M_BF_STILL_ROUNDDOWN_2(vis_rect_rounddown.lines);	// for YCC420

//	if( vis_rect_rounddown.width > dst_img->pixs.width ) {
//		BF_Debug_Print_Error(( errmsg, 1, 0 ));
//		return FJ_ERR_NG_INPUT_PARAM;
//	}
//	if( vis_rect_rounddown.lines > dst_img->pixs.lines ) {
//		BF_Debug_Print_Error(( errmsg, 2, 0 ));
//		return FJ_ERR_NG_INPUT_PARAM;
//	}

//	switch( frect_tbl_set->frect_free_tbl_type ) {
//		case E_BF_IQ_FRECT_TBL_TYPE_2VERTICAL:
//			return bf_still_iip_calc_vis_2virtical( dst_img, &vis_rect_rounddown, frect_tbl_set, outpos_set );
//		case E_BF_IQ_FRECT_TBL_TYPE_SINGLE:
//			return bf_still_iip_calc_vis_single( dst_img, &vis_rect_rounddown, frect_tbl_set, outpos_set );
//		default:
//			BF_Debug_Print_Error(( errmsg, 3, 0 ));
//			return FJ_ERR_NG_INPUT_PARAM;
//	}
//}


FJ_ERR_CODE BF_Still_Iip_Set_Config( const T_FJ_IIP_RES_CONFIG_OUT* const res_config, T_BF_STILL_IIP_RESOURCE* const res_resource )
{
	FJ_ERR_CODE	fj_ercd;
	ULLONG		unitid_bitmask64;

	// set unitid/pixid bitmask
	res_resource->unitid_bitmask64	= res_config->unitid_bitmask64;
	res_resource->pixid_bitmask		= res_config->pixid_bitmask;
	res_resource->open_res_bitmask	= res_config->other_bitmask;

	// set unitid/portid
	unitid_bitmask64 = res_config->unitid_bitmask64;
	fj_ercd = bf_still_iip_set_src_unitid_portid( &res_resource->src_unitid, &res_resource->src_portid, &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	fj_ercd = bf_still_iip_set_dst_unitid_portid( &res_resource->dst_unitid, &res_resource->dst_portid, &res_resource->wait_factor, &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	// set pixid
	fj_ercd = bf_still_iip_set_pixid(res_resource);

	return fj_ercd;
}


FJ_ERR_CODE BF_Still_Iip_Set_Config2( const T_FJ_IIP_RES_CONFIG_OUT* const res_config, T_BF_STILL_IIP_RESOURCE2* const res_resource )
{
	FJ_ERR_CODE	fj_ercd;
	ULLONG		unitid_bitmask64;

	// set unitid/pixid bitmask
	res_resource->unitid_bitmask64	= res_config->unitid_bitmask64;
	res_resource->pixid_bitmask		= res_config->pixid_bitmask;

	// set unitid/portid
	unitid_bitmask64 = res_config->unitid_bitmask64;
	fj_ercd = bf_still_iip_set_filter_unitid_portid( &res_resource->filter_unitid, res_resource->filter_portid, &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	fj_ercd = bf_still_iip_set_dst_unitid_portid( &res_resource->dst_unitid, &res_resource->dst_portid, &res_resource->wait_factor, &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	fj_ercd = bf_still_iip_set_src_unitid_portid( &res_resource->src_unitid[0], &res_resource->src_portid[0], &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	if( unitid_bitmask64 != 0 ) {
		fj_ercd = bf_still_iip_set_src_unitid_portid( &res_resource->src_unitid[1], &res_resource->src_portid[1], &unitid_bitmask64 );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}
	}

	// set pixid
	fj_ercd = bf_still_iip_set_pixid2( res_resource );

	return fj_ercd;
}

FJ_ERR_CODE BF_Still_Iip_Set_Config_Split( const T_FJ_IIP_RES_CONFIG_OUT* const res_config, T_BF_STILL_IIP_RESOURCE_SPLIT* const res_resource )
{
	FJ_ERR_CODE	fj_ercd;
	ULLONG		unitid_bitmask64;

	// set unitid/pixid bitmask
	res_resource->unitid_bitmask64	= res_config->unitid_bitmask64;
	res_resource->pixid_bitmask		= res_config->pixid_bitmask;

	// set unitid/portid
	unitid_bitmask64 = res_config->unitid_bitmask64;

	fj_ercd = bf_still_iip_set_dst_unitid_portid( &res_resource->dst_unitid[2], &res_resource->dst_portid[2], &res_resource->wait_factor[2], &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	fj_ercd = bf_still_iip_set_dst_unitid_portid( &res_resource->dst_unitid[0], &res_resource->dst_portid[0], &res_resource->wait_factor[0], &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	fj_ercd = bf_still_iip_set_dst_unitid_portid( &res_resource->dst_unitid[1], &res_resource->dst_portid[1], &res_resource->wait_factor[1], &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	fj_ercd = bf_still_iip_set_src_unitid_portid( &res_resource->src_unitid[0], &res_resource->src_portid[0], &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	fj_ercd = bf_still_iip_set_src_unitid_portid( &res_resource->src_unitid[1], &res_resource->src_portid[1], &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	fj_ercd = bf_still_iip_set_src_unitid_portid( &res_resource->src_unitid[2], &res_resource->src_portid[2], &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	// set pixid
	fj_ercd = bf_still_iip_set_pixid3( res_resource );

	return fj_ercd;
}

FJ_ERR_CODE BF_Still_Iip_Set_Config_Split_2( const T_FJ_IIP_RES_CONFIG_OUT* const res_config, T_BF_STILL_IIP_RESOURCE_SPLIT* const res_resource )
{
	FJ_ERR_CODE	fj_ercd;
	ULLONG		unitid_bitmask64;

	// set unitid/pixid bitmask
	res_resource->unitid_bitmask64	= res_config->unitid_bitmask64;
	res_resource->pixid_bitmask		= res_config->pixid_bitmask;

	// set unitid/portid
	unitid_bitmask64 = res_config->unitid_bitmask64;

	fj_ercd = bf_still_iip_set_dst_unitid_portid( &res_resource->dst_unitid[0], &res_resource->dst_portid[0], &res_resource->wait_factor[0], &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	fj_ercd = bf_still_iip_set_dst_unitid_portid( &res_resource->dst_unitid[1], &res_resource->dst_portid[1], &res_resource->wait_factor[1], &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	fj_ercd = bf_still_iip_set_src_unitid_portid( &res_resource->src_unitid[0], &res_resource->src_portid[0], &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}
	fj_ercd = bf_still_iip_set_src_unitid_portid( &res_resource->src_unitid[1], &res_resource->src_portid[1], &unitid_bitmask64 );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	// set pixid
	fj_ercd = bf_still_iip_set_pixid4( res_resource );

	return fj_ercd;
}


E_IM_IIP_PIXID BF_Still_Iip_Conv_Pixid2UtilPixid( const UINT32 pixid )
{
	switch( pixid ) {
		case 0:
			return E_IM_IIP_PIXID_0;
		case 1:
			return E_IM_IIP_PIXID_1;
		case 2:
			return E_IM_IIP_PIXID_2;
		case 3:
			return E_IM_IIP_PIXID_3;
		case 4:
			return E_IM_IIP_PIXID_4;
		case 5:
			return E_IM_IIP_PIXID_5;
		case 6:
			return E_IM_IIP_PIXID_6;
		case 7:
			return E_IM_IIP_PIXID_7;
		case 8:
			return E_IM_IIP_PIXID_8;
		case 9:
			return E_IM_IIP_PIXID_9;
		case 10:
			return E_IM_IIP_PIXID_10;
		case 11:
			return E_IM_IIP_PIXID_11;
		default:
			BF_Debug_Print_Error(( "BF_Still_Iip_Conv_Pixid2UtilPixid() unknown pixid. 0x%x\n", pixid ));
			return E_IM_IIP_PIXID_11;		// fallthrough
	}
}


FJ_ERR_CODE BF_Still_Iip_Set_SrcCacheSel( E_IM_IIP_PARAM_CSEL* const src_cache_sel, const UCHAR src_mif )
{
	// Select Master IF
	if ( src_mif == D_BF_STILL_IIP_MASTER_IF2 ) {
		*src_cache_sel = E_IM_IIP_PARAM_CSEL_2;
	}
	else if ( src_mif == D_BF_STILL_IIP_MASTER_IF1 ) {
		*src_cache_sel = E_IM_IIP_PARAM_CSEL_1;
	}
	else {
		*src_cache_sel = E_IM_IIP_PARAM_CSEL_0;
	}

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_Still_Iip_Set_SrcMasterIf( UCHAR* const res_src_mif, const UCHAR src_mif )
{
	// Select Master IF
	if ( src_mif == D_BF_STILL_IIP_MASTER_IF2 ) {
		*res_src_mif = D_IM_IIP_MASTER_IF2;
	}
	else if ( src_mif == D_BF_STILL_IIP_MASTER_IF1 ) {
		*res_src_mif = D_IM_IIP_MASTER_IF1;
	}
	else {
		*res_src_mif = D_IM_IIP_MASTER_IF0;
	}

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_Still_Iip_Set_DstMasterIf( UCHAR* const res_dst_mif, T_FJ_IIP_RES_CONFIG_ARG* const iip_res, const UCHAR dst_mif, const BOOL mask_enable )
{
	// Select Master IF
	if ( dst_mif == D_BF_STILL_IIP_MASTER_IF2 ) {
		*res_dst_mif = D_IM_IIP_MASTER_IF2;
		if( mask_enable == FALSE ) {
			iip_res->unit.sl_master_if_2 = 1;
		}
		else {
			iip_res->unit.sl_0_2_master_if_2 = 1;
		}
	}
	else if ( dst_mif == D_BF_STILL_IIP_MASTER_IF1 ) {
		*res_dst_mif = D_IM_IIP_MASTER_IF1;
		if( mask_enable == FALSE ) {
			iip_res->unit.sl_master_if_1 = 1;
		}
		else {
			iip_res->unit.sl_0_2_master_if_1 = 1;
		}
	}
	else {
		*res_dst_mif = D_IM_IIP_MASTER_IF0;
		if( mask_enable == FALSE ) {
			iip_res->unit.sl_master_if_0 = 1;
		}
		else {
			iip_res->unit.sl_0_2_master_if_0 = 1;
		}
	}

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_Still_Iip_Check_Cache_Access( VOID )
{
	E_IM_IIP_CACHE_CH		cache_ch;
	INT32					ercd;
	DOUBLE					ratio;
	T_IM_IIP_ACCESS_INFO	cache_info;
	const CHAR				errmsg[] = "BF_Still_Iip_Check_Cache_Access: no=%u ercd=%d\n";

	for( cache_ch = E_IM_IIP_CACHE_CH_0; cache_ch < E_IM_IIP_CACHE_CH_MAX; cache_ch++ ) {
		// check Cache-0ch
		ercd = Im_IIP_Get_ImgCache_Access_Info( cache_ch, &cache_info );
		if( ercd != D_IM_IIP_OK ) {
			BF_Debug_Print_Error(( errmsg, 1, ercd ));
			return FJ_ERR_NG;
		}

		if( cache_info.real_access_counter > 0 ) {
			ratio = ((DOUBLE)(cache_info.real_access_counter - cache_info.real_miss_counter) * 100) / cache_info.real_access_counter;
		}
		else {
			ratio = 0.0;
		}

		BF_Debug_Print_Information(( "IIP cache status ( %uch ) : cache hit ratio = %.2f%%\n", cache_ch, ratio ));
	}

	return FJ_ERR_OK;
}


VOID BF_Still_Iip_Dump_UnitParam( const CHAR begin_msg[], const CHAR* const funcname, const T_BF_STILL_IIP_RESOURCE* const res, volatile UINT32* const dump_tid )
{
	OS_USER_ID tskid;
	extern VOID CT_Im_IIP_Print_Unitcfg_Byid( const E_IM_IIP_UNIT_ID unitid );
	extern VOID CT_Im_IIP_Print_Pixfmttbl_Detail( const UINT32 pixid );

	OS_User_Get_Tid( &tskid );

	if( (*dump_tid) != tskid ) {
		return;
	}
	*dump_tid = D_BF_STILL_IIP_TID_DISABLE;

	BF_Debug_Print_Information(( begin_msg, __func__ ));
	CT_Im_IIP_Print_Unitcfg_Byid( res->src_unitid );
	CT_Im_IIP_Print_Pixfmttbl_Detail( res->src_pixid );
	CT_Im_IIP_Print_Unitcfg_Byid( res->dst_unitid );
	CT_Im_IIP_Print_Pixfmttbl_Detail( res->dst_pixid );
}


VOID BF_Still_Iip_Dump_UnitParam2( const CHAR begin_msg[], const T_BF_STILL_IIP_RESOURCE2* const res, volatile UINT32* const dump_tid )
{
	OS_USER_ID tskid;
	extern VOID CT_Im_IIP_Print_Unitcfg_Byid( const E_IM_IIP_UNIT_ID unitid );
	extern VOID CT_Im_IIP_Print_Pixfmttbl_Detail( const UINT32 pixid );

	OS_User_Get_Tid( &tskid );

	if( (*dump_tid) != tskid ) {
		return;
	}
	*dump_tid = D_BF_STILL_IIP_TID_DISABLE;

	BF_Debug_Print_Information(( begin_msg ));
	CT_Im_IIP_Print_Unitcfg_Byid( res->src_unitid[0] );
	CT_Im_IIP_Print_Pixfmttbl_Detail( res->src_pixid[0] );
	CT_Im_IIP_Print_Unitcfg_Byid( res->src_unitid[1] );
	CT_Im_IIP_Print_Pixfmttbl_Detail( res->src_pixid[1] );
	CT_Im_IIP_Print_Unitcfg_Byid( res->filter_unitid );
	CT_Im_IIP_Print_Unitcfg_Byid( res->dst_unitid );
	CT_Im_IIP_Print_Pixfmttbl_Detail( res->dst_pixid );
}


