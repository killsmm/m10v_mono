/**
 * @file		ct_im_iip.c
 * @brief		IIP ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// CT header
#include "ct_im_iip.h"
#include "ct_im_iip_b2y_frect_afn.h"
#include "ct_im_iip_afn.h"
#include "ct_im_iip_frect.h"
#include "ct_im_iip_lut.h"
#include "ct_im_iip_blend.h"
#include "ct_im_iip_csc.h"
#include "ct_im_iip_ld.h"
#include "ct_im_iip_flt.h"
#include "ct_im_iip_sl.h"
#include "ct_im_iip_mon.h"
#include "ct_im_iip_cfl.h"
#include "ct_im_iip_gpc.h"
#include "ct_im_iip_utility.h"
#include "ct_im_iip_mft.h"

// im_iip header
#include "im_iip.h"

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#include "palladium_test.h"
#include "palladium_test_common.h"
#endif
// ### REMOVE_RELEASE END

// Driver common header
#include "driver_common.h"
#include "ddim_user_custom.h"

// Clock Control
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
#endif

// Memory barrier
#include "dd_arm.h"

// MILB register header
#include "jdsiip.h"


// C Standard header
#include <string.h>
#include <stdlib.h>


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

//#define CO_CT_IM_IIP_DISABLE
#ifndef CO_CT_IM_IIP_DISABLE

#if 1
#include <stdio.h>
#undef Ddim_Print
#define Ddim_Print(arg)		{DDIM_User_Printf arg;}		/**< printf function inside DDIM */
#endif

//#define CO_CT_IM_IIP_2_5_3_DISABLE_FRECT_TBL
//#define CO_CT_IM_IIP_2_5_4_DISABLE_FRECT_TBL
//#define CO_CT_IM_IIP_5_1_1_DISABLE_LUT
//#define CO_CT_IM_IIP_5_1_2_DISABLE_LUT
//#define CO_CT_IM_IIP_14_3_1_DISABLE_LUT
//#define CO_CT_IM_IIP_14_3_2_DISABLE_LUT
//#define CO_CT_IM_IIP_3_1_1_FILL_TEST
//#define CO_CT_IM_IIP_2_7_1_DISABLE_HIST
//#define CO_CT_IM_IIP_11_1_2_DISABLE_HIST
#define CO_CT_IM_IIP_DCACHE_CLEAN_ENABLE

// Enable CT test number 1-1-26
//#define CO_CT_IM_IIP_ENABLE_1_1_26
// Enable CT test number 1-1-34
//#define CO_CT_IM_IIP_ENABLE_1_1_34
// Enable Image SRAM by B2Y-IIP link and/or IIP-IIP link
#define CO_CT_IM_IIP_USE_SRAM


#define D_CT_IM_IIP_UNIT_ARRAY_MAX		(sizeof(g_ct_im_iip_unitid_array) / sizeof(g_ct_im_iip_unitid_array[0]))



/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UINT64							pldunit;
	E_IM_IIP_UNIT_ID				unitid;
	CHAR*							unitname;
	volatile struct io_iip_uinftbl* pUNITINFTBL;
	VOID (*unit_param_dump)( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid );
	UCHAR							is_sl;
} T_CT_IM_IIP_UNITID_TBL;

typedef struct {
	CHAR*			axiname;
	volatile ULONG*	reg_addr0;
	volatile ULONG*	reg_addr1;
} T_CT_IM_IIP_AXI_PRINT_TBL;


/*----------------------------------------------------------------------*/
/* Local Method															*/
/*----------------------------------------------------------------------*/
#define ct_im_iip_conv_afntbl_float(regval)		((DOUBLE)(regval) / D_IM_IIP_PARAM_AFN_VAL_1_0)

static VOID ct_im_iip_print_afn_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid );
static VOID ct_im_iip_print_frect_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid );
static VOID ct_im_iip_print_sl_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid );
static VOID ct_im_iip_print_csc_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid );
static VOID ct_im_iip_print_1d_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid );
static VOID ct_im_iip_print_lut_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid );
static VOID ct_im_iip_print_blend_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid );
static VOID ct_im_iip_print_lutram( const E_IM_IIP_LUTRAM_TYPE lutram_type );
static VOID ct_im_iip_print_mft_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid );
static VOID ct_im_iip_print_flt_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid );

#endif //CO_CT_IM_IIP_DISABLE


// Input Image Memory(for Simulator test)
ULONG gct_im_iip_in_addr[2] = {
	0x48000000UL,
	0x49000000UL,
};
// Output Image Memory(for Simulator test)
ULONG gct_im_iip_out_addr[3] = {
	0x4A000000UL,
	0x4B000000UL,
	0x4C000000UL,
};


#ifndef CO_CT_IM_IIP_DISABLE

static const T_CT_IM_IIP_UNITID_TBL g_ct_im_iip_unitid_array[] = {
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_LD0,
	  .unitid = E_IM_IIP_UNIT_ID_LD0,
	  .unitname = "LD0",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_LD0,
	  .unit_param_dump = ct_im_iip_print_1d_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_LD1,
	  .unitid = E_IM_IIP_UNIT_ID_LD1,
	  .unitname = "LD1",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_LD1,
	  .unit_param_dump = ct_im_iip_print_1d_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_LD2,
	  .unitid = E_IM_IIP_UNIT_ID_LD2,
	  .unitname = "LD2",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_LD2,
	  .unit_param_dump = ct_im_iip_print_1d_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_LD3,
	  .unitid = E_IM_IIP_UNIT_ID_LD3,
	  .unitname = "LD3",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_LD3,
	  .unit_param_dump = ct_im_iip_print_1d_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_FLT,
	  .unitid = E_IM_IIP_UNIT_ID_FLT,
	  .unitname = "FLT",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_FLT,
	  .unit_param_dump = ct_im_iip_print_flt_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_AFN0,
	  .unitid = E_IM_IIP_UNIT_ID_AFN0,
	  .unitname = "AFN0",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_AFN0,
	  .unit_param_dump = ct_im_iip_print_afn_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_AFN1,
	  .unitid = E_IM_IIP_UNIT_ID_AFN1,
	  .unitname = "AFN1",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_AFN1,
	  .unit_param_dump = ct_im_iip_print_afn_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_AFN2,
	  .unitid = E_IM_IIP_UNIT_ID_AFN2,
	  .unitname = "AFN2",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_AFN2,
	  .unit_param_dump = ct_im_iip_print_afn_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_FRECT0,
	  .unitid = E_IM_IIP_UNIT_ID_FRECT0,
	  .unitname = "FRC0",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_FRC0,
	  .unit_param_dump = ct_im_iip_print_frect_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_MON,
	  .unitid = E_IM_IIP_UNIT_ID_MON,
	  .unitname = "MON",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_MON,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_SL0,
	  .unitid = E_IM_IIP_UNIT_ID_SL0,
	  .unitname = "SL0",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_SL0,
	  .unit_param_dump = ct_im_iip_print_sl_unit_param,
	  .is_sl = 1,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_SL1,
	  .unitid = E_IM_IIP_UNIT_ID_SL1,
	  .unitname = "SL1",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_SL1,
	  .unit_param_dump = ct_im_iip_print_sl_unit_param,
	  .is_sl = 1,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_SL2,
	  .unitid = E_IM_IIP_UNIT_ID_SL2,
	  .unitname = "SL2",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_SL2,
	  .unit_param_dump = ct_im_iip_print_sl_unit_param,
	  .is_sl = 1,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_SL3,
	  .unitid = E_IM_IIP_UNIT_ID_SL3,
	  .unitname = "SL3",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_SL3,
	  .unit_param_dump = ct_im_iip_print_sl_unit_param,
	  .is_sl = 1,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_SL4,
	  .unitid = E_IM_IIP_UNIT_ID_SL4,
	  .unitname = "SL4",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_SL4,
	  .unit_param_dump = ct_im_iip_print_sl_unit_param,
	  .is_sl = 1,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_SL5,
	  .unitid = E_IM_IIP_UNIT_ID_SL5,
	  .unitname = "SL5",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_SL5,
	  .unit_param_dump = ct_im_iip_print_sl_unit_param,
	  .is_sl = 1,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_CSC0,
	  .unitid = E_IM_IIP_UNIT_ID_CSC0,
	  .unitname = "CSC0",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_CSC0,
	  .unit_param_dump = ct_im_iip_print_csc_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_CSC1,
	  .unitid = E_IM_IIP_UNIT_ID_CSC1,
	  .unitname = "CSC1",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_CSC1,
	  .unit_param_dump = ct_im_iip_print_csc_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_LUT,
	  .unitid = E_IM_IIP_UNIT_ID_LUT,
	  .unitname = "LUT",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_LUT,
	  .unit_param_dump = ct_im_iip_print_lut_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_GPC,
	  .unitid = E_IM_IIP_UNIT_ID_GPC,
	  .unitname = "GPC",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_GPC,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_BLEND0,
	  .unitid = E_IM_IIP_UNIT_ID_BLEND0,
	  .unitname = "BLD0",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_BLD0,
	  .unit_param_dump = ct_im_iip_print_blend_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_BLEND1,
	  .unitid = E_IM_IIP_UNIT_ID_BLEND1,
	  .unitname = "BLD1",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_BLD1,
	  .unit_param_dump = ct_im_iip_print_blend_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_FRECT1,
	  .unitid = E_IM_IIP_UNIT_ID_FRECT1,
	  .unitname = "FRC1",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_FRC1,
	  .unit_param_dump = ct_im_iip_print_frect_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_CFL0,
	  .unitid = E_IM_IIP_UNIT_ID_CFL0,
	  .unitname = "CFL0",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_CFL0,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_CSC2,
	  .unitid = E_IM_IIP_UNIT_ID_CSC2,
	  .unitname = "CSC2",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_CSC2,
	  .unit_param_dump = ct_im_iip_print_csc_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_CSC3,
	  .unitid = E_IM_IIP_UNIT_ID_CSC3,
	  .unitname = "CSC3",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_CSC3,
	  .unit_param_dump = ct_im_iip_print_csc_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_SL6,
	  .unitid = E_IM_IIP_UNIT_ID_SL6,
	  .unitname = "SL6",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_SL6,
	  .unit_param_dump = ct_im_iip_print_sl_unit_param,
	  .is_sl = 1,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_SL7,
	  .unitid = E_IM_IIP_UNIT_ID_SL7,
	  .unitname = "SL7",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_SL7,
	  .unit_param_dump = ct_im_iip_print_sl_unit_param,
	  .is_sl = 1,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_SL8,
	  .unitid = E_IM_IIP_UNIT_ID_SL8,
	  .unitname = "SL8",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_SL8,
	  .unit_param_dump = ct_im_iip_print_sl_unit_param,
	  .is_sl = 1,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_LD4,
	  .unitid = E_IM_IIP_UNIT_ID_LD4,
	  .unitname = "LD4",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_LD4,
	  .unit_param_dump = ct_im_iip_print_1d_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_LD5,
	  .unitid = E_IM_IIP_UNIT_ID_LD5,
	  .unitname = "LD5",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_LD5,
	  .unit_param_dump = ct_im_iip_print_1d_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_LD6,
	  .unitid = E_IM_IIP_UNIT_ID_LD6,
	  .unitname = "LD6",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_LD6,
	  .unit_param_dump = ct_im_iip_print_1d_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_LD7,
	  .unitid = E_IM_IIP_UNIT_ID_LD7,
	  .unitname = "LD7",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_LD7,
	  .unit_param_dump = ct_im_iip_print_1d_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_AFN3,
	  .unitid = E_IM_IIP_UNIT_ID_AFN3,
	  .unitname = "AFN3",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_AFN3,
	  .unit_param_dump = ct_im_iip_print_afn_unit_param,
	  .is_sl = 0,
	},
	{ .pldunit = D_IM_IIP_PARAM_PLDUNIT_MFT,
	  .unitid = E_IM_IIP_UNIT_ID_MFT,
	  .unitname = "MFT",
	  .pUNITINFTBL = &IO_IIP.UNITINFTBL_MFT,
	  .unit_param_dump = ct_im_iip_print_mft_unit_param,
	  .is_sl = 0,
	},
};

static T_CT_IM_IIP_AXI_PRINT_TBL g_ct_im_iip_axi_print_tbl[E_IM_IIP_AXI_ID_MAX] = {
	[E_IM_IIP_AXI_ID_CACHE0] = { "PDCCTL[0]", &IO_IIP.PDCCTL[0].word },
	[E_IM_IIP_AXI_ID_CACHE1] = { "PDCCTL[1]", &IO_IIP.PDCCTL[1].word },
	[E_IM_IIP_AXI_ID_CACHE2] = { "PDCCTL[2]", &IO_IIP.PDCCTL[2].word },
	[E_IM_IIP_AXI_ID_DP] = { "DPAXICTL", &IO_IIP.DPAXICTL.word },
	[E_IM_IIP_AXI_ID_AFN0] = { "AFNAXICTL", &IO_IIP.AFNAXICTL.word[0], &IO_IIP.AFNAXICTL.word[1] },
	[E_IM_IIP_AXI_ID_AFN1] = { "AFNAXICTL", &IO_IIP.AFNAXICTL.word[0], &IO_IIP.AFNAXICTL.word[1] },
	[E_IM_IIP_AXI_ID_AFN2] = { "AFNAXICTL", &IO_IIP.AFNAXICTL.word[0], &IO_IIP.AFNAXICTL.word[1] },
	[E_IM_IIP_AXI_ID_AFN3] = { "AFNAXICTL", &IO_IIP.AFNAXICTL.word[0], &IO_IIP.AFNAXICTL.word[1] },
	[E_IM_IIP_AXI_ID_FRECT0] = { "FRAXICTL", &IO_IIP.FRAXICTL.word },
	[E_IM_IIP_AXI_ID_FRECT1] = { "FRAXICTL", &IO_IIP.FRAXICTL.word },
	[E_IM_IIP_AXI_ID_GPC] = { "GPCAXICTL", &IO_IIP.GPCAXICTL.word },
	[E_IM_IIP_AXI_ID_SL0] = { "SLAXICTL1", &IO_IIP.SLAXICTL.SLAXICTL1.word },
	[E_IM_IIP_AXI_ID_SL1] = { "SLAXICTL1", &IO_IIP.SLAXICTL.SLAXICTL1.word },
	[E_IM_IIP_AXI_ID_SL2] = { "SLAXICTL2", &IO_IIP.SLAXICTL.SLAXICTL2.word },
	[E_IM_IIP_AXI_ID_SL3] = { "SLAXICTL2", &IO_IIP.SLAXICTL.SLAXICTL2.word },
	[E_IM_IIP_AXI_ID_SL4] = { "SLAXICTL3", &IO_IIP.SLAXICTL.SLAXICTL3.word },
	[E_IM_IIP_AXI_ID_SL5] = { "SLAXICTL3", &IO_IIP.SLAXICTL.SLAXICTL3.word },
	[E_IM_IIP_AXI_ID_SL6] = { "SLAXICTL4", &IO_IIP.SLAXICTL.SLAXICTL4.word },
	[E_IM_IIP_AXI_ID_SL7] = { "SLAXICTL4", &IO_IIP.SLAXICTL.SLAXICTL4.word },
	[E_IM_IIP_AXI_ID_SL8] = { "SLAXICTL5", &IO_IIP.SLAXICTL.SLAXICTL5.word },
};

const T_IM_IIP_PIXFMTTBL g_ct_im_iip_pixfmttbl_base = {
	.pix_depth = D_IM_IIP_PDEPTH_8BITS,
	.alpha = D_IM_IIP_ALPHA_FALSE,
	.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,
	.pix_format = E_IM_IIP_PFMT_YCC422,
	.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
	.chunky_color = {
		.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,
		.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,
		.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,
		.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,
	},
	.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,
	.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,
	.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,
	.sign_D3 = D_IM_IIP_UNSIGNED_DATA,
	.masterIF_Y_G = D_IM_IIP_MASTER_IF0,
	.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,
	.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,
	.masterIF_Alpha = D_IM_IIP_MASTER_IF0,
	.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
	.pix_outside_img = D_IM_IIP_OUTSIDE_IMG_EDGE,
	.Y_G_val_outside_img = 0,				// dummy
	.Cb_B_val_outside_img = 0,				// dummy
	.Cr_R_val_outside_img = 0,				// dummy
	.Alpha_val_outside_img = 0,				// dummy
	.width = D_IM_IIP_VGA_WIDTH,
	.lines = D_IM_IIP_VGA_LINES,
//	.line_bytes = {
//		.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
//		.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
//		.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
//		.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH,
//	},
//	.addr = {
//		.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y,
//		.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
//		.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
//		.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A,
//	},
};

const T_IM_IIP_PARAM_AFN g_ct_im_iip_param_afn_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 29,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
	.AFNTOPCNF.bit.DATACONF = 0,						// Register default
//	.AFNTOPCNF.bit.WAITCONF,							// set later
	.AFNLDCNF.bit.DTHD = E_IM_IIP_PARAM_DTHD_NORMAL,
	.AFNLDCNF.bit.CBC = E_IM_IIP_PARAM_CBC_DISABLE,
	.AFNCTL.bit.HSMD = E_IM_IIP_PARAM_HSMD_NORMAL,
	.AFNCTL.bit.LPLVL = E_IM_IIP_PARAM_LPLVL_H_LEVEL,
	.AFNCTL.bit.HDBL = E_IM_IIP_PARAM_HDBL_SAME_AS_PICHSZ,
	.AFNCTL.bit.VDBL = E_IM_IIP_PARAM_VDBL_SAME_AS_PICVSZ,
	.AFNCTL.bit.GHSZ = E_IM_IIP_PARAM_GHSZ_64PIX,		// Needs tuning
	.AFNCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_8PIX,		// Needs tuning
	.PFCTL.bit.PFEXTMC = 4,
	.PFCTL.bit.PFEXTADC = 64,							// Needs tuning
	.PFCTL.bit.PFSP1 = E_IM_IIP_PARAM_PFSP_16,			// Needs tuning
	.PFCTL.bit.PFSP2 = E_IM_IIP_PARAM_PFSP_16,			// Needs tuning
	.PFCTL.bit.PFVSP = E_IM_IIP_PARAM_PFVSP_VERTICAL_0,	// Needs tuning
	.PFCTL.bit.PFOFF = E_IM_IIP_PARAM_PFOFF_RUN,
	.PIXIDEF.bit.EXA = E_IM_IIP_PARAM_EXA_NO_ALPHA_TO_NEXT_UNIT,
	.PIXIDEF.bit.CSEL = E_IM_IIP_PARAM_CSEL_0,			// ImgCache
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

const T_IM_IIP_PARAM_FRECT g_ct_im_iip_param_frect_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 29,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
	.FRTOPCNF.bit.DATACONF = 0,
//	.FRTOPCNF.bit.WAITCONF,							// set later
	.FRLDCNF.bit.DTHD = E_IM_IIP_PARAM_DTHD_NORMAL,
	.FRCTL.bit.HSMD = E_IM_IIP_PARAM_HSMD_NORMAL,
	.FRCTL.bit.LPLVL = E_IM_IIP_PARAM_LPLVL_H_LEVEL,
	.FRCTL.bit.HDBL = E_IM_IIP_PARAM_HDBL_SAME_AS_PICHSZ,
	.FRCTL.bit.VDBL = E_IM_IIP_PARAM_VDBL_SAME_AS_PICVSZ,
	.FRCTL.bit.GHSZ = E_IM_IIP_PARAM_GHSZ_64PIX,		// Needs tuning
	.FRCTL.bit.GVSZ = E_IM_IIP_PARAM_GVSZ_SAME_AS_VERTICAL_PIX,		// Needs tuning
	.PFCTL.bit.PFEXTMC = 4,
	.PFCTL.bit.PFEXTADC = 16,							// Needs tuning
	.PFCTL.bit.PFSP1 = E_IM_IIP_PARAM_PFSP_16,			// Needs tuning
	.PFCTL.bit.PFSP2 = E_IM_IIP_PARAM_PFSP_16,			// Needs tuning
	.PFCTL.bit.PFVSP = E_IM_IIP_PARAM_PFVSP_VERTICAL_0,	// Needs tuning
	.PFCTL.bit.PFOFF = E_IM_IIP_PARAM_PFOFF_RUN,
	.PIXIDEF.bit.EXA = E_IM_IIP_PARAM_EXA_NO_ALPHA_TO_NEXT_UNIT,
	.PIXIDEF.bit.CSEL = E_IM_IIP_PARAM_CSEL_0,			// ImgCache
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
//	.FRPCNT.bit.FRPHCNT = 1,							// set later
//	.FRPCNT.bit.FRPVCNT = 1,							// set later
//	.PADRS.word = (ULONG)frecttbl,						// set later
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

const T_IM_IIP_PARAM_1DL g_ct_im_iip_param_1dl_base = {
	.header0.bit.BeginningAddress = 0x0004,
	.header0.bit.WordLength = 16,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
//	.LD_TOPCNF0.bit.WAITCONF,							// set later
	.LD_TOPCNF1.bit.LINK_UID = 0,
//	.PIXIDDEF.bit.IPIXID,								// set later
	.LD_CTL.bit.LD_MODE = E_IM_IIP_PARAM_LD_MODE_NORMAL_LOAD,
	.LD_CTL.bit.LD_OUT = 0,
	.LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC,
	.LD_CTL.bit.SAVERD = 0,
	.LD_CTL.bit.CSEL = E_IM_IIP_PARAM_CSEL_0,
	.LD_PREFETCH.bit.PFOFF = E_IM_IIP_PARAM_PFOFF_RUN,
	.LD_PREFETCH.bit.PF_PDIST = E_IM_IIP_PARAM_PF_PDIST_8PIX,
	.LD_PREFETCH.bit.PF_1ST = E_IM_IIP_PARAM_PF_1ST_ACCESS_BEFORE_PF,
	.LD_PREFETCH.bit.PFB_NUM = 0,						// Need tuning
	.LD_PREFETCH.bit.PF_ADD = 0,						// Need tuning
	.LD_HSIZE.bit.HSTART = 0,
	.LD_HSIZE.bit.HEND = D_IM_IIP_VGA_WIDTH - 1,
	.LD_VSIZE.bit.VSTART = 0,
	.LD_VSIZE.bit.VEND = D_IM_IIP_VGA_LINES - 1,
	.LD_PHSZ.bit.PHSZ1 = 64,
	.LD_PVSZ.bit.PVSZ1 = 8,
	.LD_PHSZ.bit.PHSZ0 = 64,
	.LD_PVSZ.bit.PVSZ0 = 8,
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

const T_IM_IIP_PARAM_BLEND g_ct_im_iip_param_blend_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 4,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY,
//	.BLENDTOPCNF0.bit.WAITCONF_0 = ,							// set later
//	.BLENDTOPCNF0.bit.DATACONF_0 = ,							// set later
//	.BLENDTOPCNF1.bit.WAITCONF_1 = ,							// set later
//	.BLENDTOPCNF1.bit.DATACONF_1 = ,							// set later
	.BLENDTOPCNF2.bit.DATACONF_2 = E_IM_IIP_PARAM_PORTID_NONE,
	.header1.bit.BeginningAddress = 0x0080,
	.header1.bit.WordLength = 24,
	.header1.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
	.BLENDCTL.bit.BLD_MD = E_IM_IIP_PARAM_BLD_MD_ALPHA_BLEND,
	.BLENDCTL.bit.CLP_MD = E_IM_IIP_PARAM_CLP_MD_NO_CLIP,
	.BLENDCTL.bit.ALPHA_OUT = E_IM_IIP_PARAM_ALPHA_OUT_NONE,
	.ALPHACTL0.bit.ALPHA_STEP = E_IM_IIP_PARAM_ALPHA_STEP_8BITS,
	.ALPHACTL0.bit.ALPHA_SEL = E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL,
	.ALPHACTL1.bit.ALPHA_VAL0 = 128,
	.ALPHACTL1.bit.ALPHA_VAL1 = 128,
	.ALPHACTL2.bit.ALPHA_VAL2 = 128,
	.ALPHACTL2.bit.ALPHA_VAL3 = 128,
	.WADDCTL[0].bit.WADD_KA = 0,
	.WADDCTL[0].bit.WADD_KB = 0,
	.WADDCTL[1].bit.WADD_KA = 0,
	.WADDCTL[1].bit.WADD_KB = 0,
	.WADDCTL[2].bit.WADD_KA = 0,
	.WADDCTL[2].bit.WADD_KB = 0,
	.WADDCTL[3].bit.WADD_KA = 0,
	.WADDCTL[3].bit.WADD_KB = 0,
	.DIVCTL0.bit.RSFT = 0xF,
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

const T_IM_IIP_PARAM_STS g_ct_im_iip_param_sts_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 26,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY,
	.BASE = {
		.SL_TOPCNF0.bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE,
//		.SL_TOPCNF0.bit.DATACONF,		// set later
//		.PIXIDDEF.bit.OPIXID,			// set later
		.PIXIDDEF.bit.CSEL = E_IM_IIP_PARAM_CSEL_0,	// dummy
		.PIXIDDEF.bit.DIV1ST = E_IM_IIP_PARAM_DIV1ST_1ST,
		.PIXIDDEF.bit.LKDIV = 0,

		.PLDUNIT_SL.bit.PLDUNIT_SL_LO = Im_IIP_Get_PLDUNIT_Lo( D_IM_IIP_PARAM_PLDUNIT_NONE ),
		.PLDUNIT_SL.bit.PLDUNIT_SL_HI = Im_IIP_Get_PLDUNIT_Hi( D_IM_IIP_PARAM_PLDUNIT_NONE ),

		.SL_CTL.bit.SL_MODE = 0,
		.SL_CTL.bit.LOADMD = 0,
		.SL_CTL.bit.MASKEN = E_IM_IIP_PARAM_MASKEN_NO_MASK,

		.OUTCOL.bit.OUTCOL = 0,

		.SL_HSIZE.bit.HSTART = 0,
		.SL_HSIZE.bit.HEND = D_IM_IIP_VGA_WIDTH - 1,
		.SL_VSIZE.bit.VSTART = 0,
		.SL_VSIZE.bit.VEND = D_IM_IIP_VGA_LINES - 1,
		.SL_PHSZ.bit.PHSZ1 =  64,
		.SL_PVSZ.bit.PVSZ1 =  8,
		.SL_PHSZ.bit.PHSZ0 = 64,
		.SL_PVSZ.bit.PVSZ0 = 8,

		.SEL_OPARA.bit.SEL_OPARA = 0,

		.OFSET_0.bit.OFSET = 0,
		.CLPTH_U_0.bit.CLPTH_U = 0x3FFF,
		.CLPTH_L_0.bit.CLPTH_L = -0x4000,
		.CLPVAL_U_0.bit.CLPVAL_U = 0x3FFF,
		.CLPVAL_L_0.bit.CLPVAL_L = -0x4000,
		.SFT_0.bit.SFTVAL = 0,
		.SFT_0.bit.SFTM = E_IM_IIP_PARAM_SFTM_ROUNDDOWN,
		.W14MD_0.bit.W14LSFT = 0,
		.W14MD_0.bit.W14SAT = 0,
	},

	.header1.bit.BeginningAddress = 0x0200,
	.header1.bit.WordLength = 2,
	.header1.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
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
};

const T_IM_IIP_PARAM_CSC g_ct_im_iip_param_csc_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 25,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
//	.CSCTOPCNF.bit.WAITCONF = ,		// set later
//	.CSCTOPCNF.bit.DATACONF = ,		// set later
	.CSCK = {
		[0] = {
			.bit.CSCK_0 = (LONG)(1.0F * 0x1000),
			.bit.CSCK_1 = 0,
			.bit.CSCK_2 = 0,
			.bit.CSCK_3 = 0,
		},
		[1] = {
			.bit.CSCK_0 = 0,
			.bit.CSCK_1 = (LONG)(1.0F * 0x1000),
			.bit.CSCK_2 = 0,
			.bit.CSCK_3 = 0,
		},
		[2] = {
			.bit.CSCK_0 = 0,
			.bit.CSCK_1 = 0,
			.bit.CSCK_2 = (LONG)(1.0F * 0x1000),
			.bit.CSCK_3 = 0,
		},
		[3] = {
			.bit.CSCK_0 = 0,
			.bit.CSCK_0 = 0,
			.bit.CSCK_2 = 0,
			.bit.CSCK_3 = (LONG)(1.0F * 0x1000),
		},
	},
	.MDSEL.bit.MDSEL = 1,
	.ALPSEL.bit.ALPSEL = E_IM_IIP_PARAM_ALPSEL_DIRECT,
	.ALPSEL.bit.ALOSEL = E_IM_IIP_PARAM_ALOSEL_CALCULATED,
	.ALPVAL.bit.ALPVAL = 0,
	.MAXMIN.bit.MAXMIN0 = E_IM_IIP_PARAM_MAXMIN_THROUGH,
	.MAXMIN.bit.MAXMIN1 = E_IM_IIP_PARAM_MAXMIN_THROUGH,
	.MAXMIN.bit.MAXMIN2 = E_IM_IIP_PARAM_MAXMIN_THROUGH,
	.MAXMIN.bit.MAXMIN3 = E_IM_IIP_PARAM_MAXMIN_THROUGH,
	.OUTCLIP.bit.CLPMAX0 = 0x3fff,
	.OUTCLIP.bit.CLPMIN0 = 0x4000,
	.OUTCLIP.bit.CLPMAX1 = 0x3fff,
	.OUTCLIP.bit.CLPMIN1 = 0x4000,
	.OUTCLIP.bit.CLPMAX2 = 0x3fff,
	.OUTCLIP.bit.CLPMIN2 = 0x4000,
	.OUTCLIP.bit.CLPMAX3 =  0xff,
	.OUTCLIP.bit.CLPMIN3 = 0x100,
};

const T_IM_IIP_PARAM_FLT g_ct_im_iip_param_flt_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 145,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
//	.TDLTOPCNF.bit.WAITCONF_0 = 0,		// set later
	.TDLTOPCNF.bit.WAITCONF_1 = E_IM_IIP_PARAM_PORTID_NONE,
	.TDLTOPCNF.bit.DATACONF = E_IM_IIP_PARAM_PORTID_NONE,
//	.PIXIDDEF.bit.IPIXID0 = ,		// set later
	.PIXIDDEF.bit.IPIXID1 = 0,
	.TDL_CTL.bit.FM = E_IM_IIP_PARAM_FM_3x3,
	.TDL_CTL.bit.LNUM = E_IM_IIP_PARAM_LNUM_SLOW,
	.TDL_CTL.bit.SAVERD = 0,
	.TDL_CTL.bit.INPUT_MODE = E_IM_IIP_PARAM_INPUT_MODE_NORMAL,
	.TDL_CTL.bit.CSEL = E_IM_IIP_PARAM_CSEL_0,
	.CALC_MD.bit.CALC_MD = E_IM_IIP_PARAM_CALC_MD_NORMAL,
	.TDL_HSIZE.bit.HSTART = 0,
	.TDL_HSIZE.bit.HEND = D_IM_IIP_VGA_WIDTH - 1,
	.TDL_VSIZE.bit.VSTART = 0,
	.TDL_VSIZE.bit.VEND = D_IM_IIP_VGA_LINES - 1,
	.TDL_PHSZ.bit.PHSZ0 = 64,
	.TDL_PVSZ.bit.PVSZ0 = 8,
	.TDL_PHSZ.bit.PHSZ1 = 64,
	.TDL_PVSZ.bit.PVSZ1 = 8,
	.TDL_PF.bit.PF_ON = E_IM_IIP_PARAM_PF_ON_ACTIVE,
	.TDL_PF.bit.PF_MD = E_IM_IIP_PARAM_PF_MD_PREFETCH,
	.TDL_PF.bit.PF_NUM = 16,
	.TDL_PF.bit.PF_ADJ_NUM = 0xf,
	.TDL_PF.bit.PF_ADJ_VAL = 0xffff,
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
	.UNIT_OUT.bit.UNIT_OUT_A_D0 = E_IM_IIP_PARAM_UNIT_OUT_SPF,		// port 0
	.UNIT_OUT.bit.UNIT_OUT_A_D1 = E_IM_IIP_PARAM_UNIT_OUT_SPF,		// port 0
	.UNIT_OUT.bit.UNIT_OUT_A_D2 = E_IM_IIP_PARAM_UNIT_OUT_SPF,		// port 0
	.UNIT_OUT.bit.UNIT_OUT_B_D0 = E_IM_IIP_PARAM_UNIT_OUT_DISABLE,	// port 1
	.UNIT_OUT.bit.UNIT_OUT_B_D1 = E_IM_IIP_PARAM_UNIT_OUT_DISABLE,	// port 1
	.UNIT_OUT.bit.UNIT_OUT_B_D2 = E_IM_IIP_PARAM_UNIT_OUT_DISABLE,	// port 1
	.FIN_CLP_A_D0.bit.FIN_CLVLP_A_D0 = 0x3fff,
	.FIN_CLP_A_D0.bit.FIN_CLVLM_A_D0 = 0x4000,
	.FIN_CLP_A_D1.bit.FIN_CLVLP_A_D1 = 0xff,
	.FIN_CLP_A_D1.bit.FIN_CLVLM_A_D1 = 0x100,
	.FIN_CLP_A_D2.bit.FIN_CLVLP_A_D2 = 0xff,
	.FIN_CLP_A_D2.bit.FIN_CLVLM_A_D2 = 0x100,
	.FIN_CLP_B_D0.bit.FIN_CLVLP_B_D0 = 0x3fff,
	.FIN_CLP_B_D0.bit.FIN_CLVLM_B_D0 = 0x4000,
	.FIN_CLP_B_D1.bit.FIN_CLVLP_B_D1 = 0xff,
	.FIN_CLP_B_D1.bit.FIN_CLVLM_B_D1 = 0x100,
	.FIN_CLP_B_D2.bit.FIN_CLVLP_B_D2 = 0xff,
	.FIN_CLP_B_D2.bit.FIN_CLVLM_B_D2 = 0x100,
	.ISO_MD.bit.ISO_JM = 0,
	.ISO_MD.bit.ISO_CM = 0,
	.ISO_TH_D[0].bit.ISO_UTH_D = 0x7fff,
	.ISO_TH_D[0].bit.ISO_LTH_D = 0x7fff,
	.ISO_TH_D[1].bit.ISO_UTH_D = 0x7fff,
	.ISO_TH_D[1].bit.ISO_LTH_D = 0x7fff,
	.ISO_TH_D[2].bit.ISO_UTH_D = 0x7fff,
	.ISO_TH_D[2].bit.ISO_LTH_D = 0x7fff,
	.ISO_TH_D[3].bit.ISO_UTH_D = 0x7fff,
	.ISO_TH_D[3].bit.ISO_LTH_D = 0x7fff,
	.ISO_GAIN_D[0].bit.ISO_GA_D = 0x100,
	.ISO_GAIN_D[0].bit.ISO_GB_D = 0x100,
	.ISO_GAIN_D[1].bit.ISO_GA_D = 0x100,
	.ISO_GAIN_D[1].bit.ISO_GB_D = 0x100,
	.ISO_GAIN_D[2].bit.ISO_GA_D = 0x100,
	.ISO_GAIN_D[2].bit.ISO_GB_D = 0x100,
	.ISO_GAIN_D[3].bit.ISO_GA_D = 0x100,
	.ISO_GAIN_D[3].bit.ISO_GB_D = 0x100,
	.SPF_MD.bit.SPF_MD = E_IM_IIP_PARAM_SPF_MD_SPF,
	.SPF_FLTW_D0.bit = {
		// 3x3
		0x020, 0x020, 0x020,	// 0.1250 0.1250 0.1250
		0x020, 0x000, 0x020,	// 0.1250 0.0000 0.1250
		0x020, 0x020, 0x020,	// 0.1250 0.1250 0.1250
		// dummy(9~16)
		0, 0, 0, 0, 0, 0, 0, 0,
	},
	.SPF_FLTW_D1.bit = {
		// 3x3
		0x020, 0x020, 0x020,	// 0.1250 0.1250 0.1250
		0x020, 0x000, 0x020,	// 0.1250 0.0000 0.1250
		0x020, 0x020, 0x020,	// 0.1250 0.1250 0.1250
		// dummy(9~16)
		0, 0, 0, 0, 0, 0, 0, 0,
	},
	.SPF_FLTW_D2.bit = {
		// 3x3
		0x020, 0x020, 0x020,	// 0.1250 0.1250 0.1250
		0x020, 0x000, 0x020,	// 0.1250 0.0000 0.1250
		0x020, 0x020, 0x020,	// 0.1250 0.1250 0.1250
		// dummy(9~16)
		0, 0, 0, 0, 0, 0, 0, 0,
	},
	.SPF_FLTW_D3.bit = {
		// 3x3
		0x020, 0x020, 0x020,	// 0.1250 0.1250 0.1250
		0x020, 0x000, 0x020,	// 0.1250 0.0000 0.1250
		0x020, 0x020, 0x020,	// 0.1250 0.1250 0.1250
		// dummy(9~16)
		0, 0, 0, 0, 0, 0, 0, 0,
	},
	.SPF_VTH_D0.bit.SPF_UTH_D0 = 0x7fff,
	.SPF_VTH_D0.bit.SPF_LTH_D0 = 0,
	.SPF_VTH_D1.bit.SPF_UTH_D1 = 0x1ff,
	.SPF_VTH_D1.bit.SPF_LTH_D1 = 0,
	.SPF_VTH_D2.bit.SPF_UTH_D2 = 0x1ff,
	.SPF_VTH_D2.bit.SPF_LTH_D2 = 0,
	.SPF_CORE_D0.bit.SPF_CTHP_D0 = 0,
	.SPF_CORE_D0.bit.SPF_CTHM_D0 = 0,
	.SPF_CORE_D1.bit.SPF_CTHP_D1 = 0,
	.SPF_CORE_D1.bit.SPF_CTHM_D1 = 0,
	.SPF_CORE_D2.bit.SPF_CTHP_D2 = 0,
	.SPF_CORE_D2.bit.SPF_CTHM_D2 = 0,
	.SPF_CLP_D0.bit.SPF_CLVLP_D0 = 0x3fff,
	.SPF_CLP_D0.bit.SPF_CLVLM_D0 = 0x4000,
	.SPF_CLP_D1.bit.SPF_CLVLP_D1 = 0xff,
	.SPF_CLP_D1.bit.SPF_CLVLM_D1 = 0x100,
	.SPF_CLP_D2.bit.SPF_CLVLP_D2 = 0xff,
	.SPF_CLP_D2.bit.SPF_CLVLM_D2 = 0x100,
	.SPF_EOADD_D0.bit.SPF_EOADD_D0 = 0,
	.SPF_EOADD_D1.bit.SPF_EOADD_D1 = 0,
	.SPF_EOADD_D2.bit.SPF_EOADD_D2 = 0,
	.EP_MD.bit.EP_MD = E_IM_IIP_PARAM_EP_MD_NR_LOW,
	.EP_GA_D[0].bit.EP_GA_0 = 0x200,
	.EP_GA_D[0].bit.EP_GA_1 = 0x100,
	.EP_GA_D[0].bit.EP_GA_2 = 0x080,
	.EP_GA_D[1].bit.EP_GA_0 = 0x200,
	.EP_GA_D[1].bit.EP_GA_1 = 0x100,
	.EP_GA_D[1].bit.EP_GA_2 = 0x080,
	.EP_GA_D[2].bit.EP_GA_0 = 0x200,
	.EP_GA_D[2].bit.EP_GA_1 = 0x100,
	.EP_GA_D[2].bit.EP_GA_2 = 0x080,
	.EP_GA_D[3].bit.EP_GA_0 = 0x200,
	.EP_GA_D[3].bit.EP_GA_1 = 0x100,
	.EP_GA_D[3].bit.EP_GA_2 = 0x080,
	.EP_OFS_D[0].bit.EP_OFS_0 = 0x000,
	.EP_OFS_D[0].bit.EP_OFS_1 = 0x020,
	.EP_OFS_D[0].bit.EP_OFS_2 = 0x040,
	.EP_OFS_D[1].bit.EP_OFS_0 = 0x000,
	.EP_OFS_D[1].bit.EP_OFS_1 = 0x020,
	.EP_OFS_D[1].bit.EP_OFS_2 = 0x040,
	.EP_OFS_D[2].bit.EP_OFS_0 = 0x000,
	.EP_OFS_D[2].bit.EP_OFS_1 = 0x020,
	.EP_OFS_D[2].bit.EP_OFS_2 = 0x040,
	.EP_OFS_D[3].bit.EP_OFS_0 = 0x000,
	.EP_OFS_D[3].bit.EP_OFS_1 = 0x020,
	.EP_OFS_D[3].bit.EP_OFS_2 = 0x040,
	.EP_BD_D[0].bit.EP_BD_0 = 0x000,
	.EP_BD_D[0].bit.EP_BD_1 = 0x040,
	.EP_BD_D[0].bit.EP_BD_2 = 0x080,
	.EP_BD_D[1].bit.EP_BD_0 = 0x000,
	.EP_BD_D[1].bit.EP_BD_1 = 0x040,
	.EP_BD_D[1].bit.EP_BD_2 = 0x080,
	.EP_BD_D[2].bit.EP_BD_0 = 0x000,
	.EP_BD_D[2].bit.EP_BD_1 = 0x040,
	.EP_BD_D[2].bit.EP_BD_2 = 0x080,
	.EP_BD_D[3].bit.EP_BD_0 = 0x000,
	.EP_BD_D[3].bit.EP_BD_1 = 0x040,
	.EP_BD_D[3].bit.EP_BD_2 = 0x080,
	.ED_FM.bit.ED_FM_0 = E_IM_IIP_PARAM_ED_FM_NEAR4,
	.ED_FM.bit.ED_FM_1 = E_IM_IIP_PARAM_ED_FM_NEAR4,
	.ED_FM.bit.ED_FM_2 = E_IM_IIP_PARAM_ED_FM_NEAR4,
	.ED_FM.bit.ED_FM_3 = E_IM_IIP_PARAM_ED_FM_NEAR4,
	.ED_OUT.bit.ED_OUT_0 = E_IM_IIP_PARAM_ED_OUT_DIFF_ABS_MIN,
	.ED_OUT.bit.ED_OUT_1 = E_IM_IIP_PARAM_ED_OUT_DIFF_ABS_MIN,
	.ED_OUT.bit.ED_OUT_2 = E_IM_IIP_PARAM_ED_OUT_DIFF_ABS_MIN,
	.ED_OUT.bit.ED_OUT_3 = E_IM_IIP_PARAM_ED_OUT_DIFF_ABS_MIN,
	.ADAPT_K.bit.ADAPT_K0 = 0x08,
	.ADAPT_K.bit.ADAPT_K1 = 0x04,
	.ADAPT_K.bit.ADAPT_K2 = 0x00,
	.ADAPT_TH.bit.ADAPT_TH = 0,
	.ADAPT_DIV.bit.ADAPT_DIV = E_IM_IIP_PARAM_ADAPT_DIV_1_DIVBY_2,
	.BF_MD.bit.BF_FMT = E_IM_IIP_PARAM_BF_FMT_VALID_D0_D1_D2,
	.BF_MD.bit.BF_PRT = E_IM_IIP_PARAM_BF_PRT_PORT0,
};

const T_IM_IIP_PARAM_MON g_ct_im_iip_param_mon_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 25,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
	.MONCTL.bit.HISTAE = 1,
	.MONCTL.bit.HISTBE = 1,
	.MONCTL.bit.HISTCE = 1,
	.MONCTL.bit.HISTDE = 1,
	.MONCTL.bit.MAXE = 1,
	.MONCTL.bit.MINE = 1,
	.MONCTL.bit.ADDE = 1,
	.MONDSEL.bit.DSEL = E_IM_IIP_PARAM_DSEL_SL0,
	.MONDSEL.bit.SGNEN0 = E_IM_IIP_PARAM_SGNEN_UNSIGN,
	.MONDSEL.bit.SGNEN1 = E_IM_IIP_PARAM_SGNEN_UNSIGN,
	.MONDSEL.bit.SGNEN2 = E_IM_IIP_PARAM_SGNEN_UNSIGN,
	.MONDSEL.bit.SGNEN3 = E_IM_IIP_PARAM_SGNEN_UNSIGN,
	.HISTCTL[0].bit.HISTSEL = 0,		// D0
	.HISTCTL[0].bit.BITSEL = E_IM_IIP_PARAM_BITSEL_7_0,
	.HISTCTL[0].bit.HISTSKIP_H = 2,
	.HISTCTL[0].bit.HISTSKIP_V = 2,
	.HISTCTL[1].bit.HISTSEL = 1,		// D1
	.HISTCTL[1].bit.BITSEL = E_IM_IIP_PARAM_BITSEL_7_0,
	.HISTCTL[1].bit.HISTSKIP_H = 2,
	.HISTCTL[1].bit.HISTSKIP_V = 2,
	.HISTCTL[2].bit.HISTSEL = 2,		// D2
	.HISTCTL[2].bit.BITSEL = E_IM_IIP_PARAM_BITSEL_7_0,
	.HISTCTL[2].bit.HISTSKIP_H = 2,
	.HISTCTL[2].bit.HISTSKIP_V = 2,
	.HISTCTL[3].bit.HISTSEL = 3,		// D3
	.HISTCTL[3].bit.BITSEL = E_IM_IIP_PARAM_BITSEL_7_0,
	.HISTCTL[3].bit.HISTSKIP_H = 2,
	.HISTCTL[3].bit.HISTSKIP_V = 2,
	.ABSMAX.bit.ABSMAX = 0,
	.ABSMIN.bit.ABSMIN = 0,
	.ABSADD.bit.ABSADD = 0,
	.OVERVALUE[0].bit.OVERVALUE = 0x7FFFFFFF,
	.OVERVALUE[1].bit.OVERVALUE = 0x7FFFFFFF,
	.OVERVALUE[2].bit.OVERVALUE = 0x7FFFFFFF,
	.OVERVALUE[3].bit.OVERVALUE = 0x7FFFFFFF,
};


#ifdef __GNUC__
T_IM_IIP_PARAM_AFN			g_ct_im_iip_unit_param_afn __attribute__((aligned(8)));			// Needs 8bytes align
T_IM_IIP_PARAM_STS			g_ct_im_iip_unit_param_sl0 __attribute__((aligned(8)));			// Needs 8bytes align
T_IM_IIP_PARAM_STS			g_ct_im_iip_unit_param_sl1 __attribute__((aligned(8)));			// Needs 8bytes align
T_IM_IIP_PARAM_STS			g_ct_im_iip_unit_param_sl2 __attribute__((aligned(8)));			// Needs 8bytes align
T_IM_IIP_PARAM_AFN_OPCOL_0	g_ct_im_iip_afntbl __attribute__((aligned(8)));					// Needs 8bytes align
T_IM_IIP_PARAM_FRECT			g_ct_im_iip_unit_param_frect0 __attribute__((aligned(8)));	// Needs 8bytes align
T_IM_IIP_PARAM_FRECT			g_ct_im_iip_unit_param_frect1 __attribute__((aligned(8)));	// Needs 8bytes align
T_IM_IIP_PARAM_1DL			g_ct_im_iip_unit_param_1dl0 __attribute__((aligned(8)));		// Needs 8bytes align
T_IM_IIP_PARAM_1DL			g_ct_im_iip_unit_param_1dl1 __attribute__((aligned(8)));		// Needs 8bytes align
T_IM_IIP_PARAM_1DL			g_ct_im_iip_unit_param_1dl2 __attribute__((aligned(8)));		// Needs 8bytes align
T_IM_IIP_PARAM_BLEND			g_ct_im_iip_unit_param_blend __attribute__((aligned(8)));	// Needs 8bytes align
T_IM_IIP_PARAM_CSC			g_ct_im_iip_unit_param_csc __attribute__((aligned(8)));			// Needs 8bytes align
T_IM_IIP_PARAM_FLT			g_ct_im_iip_unit_param_flt __attribute__((aligned(8)));			// Needs 8bytes align
T_IM_IIP_PARAM_MON			g_ct_im_iip_unit_param_mon __attribute__((aligned(8)));			// Needs 8bytes align
#else
__align(8) T_IM_IIP_PARAM_AFN			g_ct_im_iip_unit_param_afn;								// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_STS			g_ct_im_iip_unit_param_sl0;								// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_STS			g_ct_im_iip_unit_param_sl1;								// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_STS			g_ct_im_iip_unit_param_sl2;								// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_AFN_OPCOL_0	g_ct_im_iip_afntbl;										// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_FRECT			g_ct_im_iip_unit_param_frect0;							// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_FRECT			g_ct_im_iip_unit_param_frect1;							// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_1DL			g_ct_im_iip_unit_param_1dl0;							// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_1DL			g_ct_im_iip_unit_param_1dl1;							// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_1DL			g_ct_im_iip_unit_param_1dl2;							// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_BLEND			g_ct_im_iip_unit_param_blend;							// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_CSC			g_ct_im_iip_unit_param_csc;								// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_FLT			g_ct_im_iip_unit_param_flt;								// Needs 8bytes align
__align(8) T_IM_IIP_PARAM_MON			g_ct_im_iip_unit_param_mon;								// Needs 8bytes align
#endif

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
ULONG gCtImIIP_Get_Unit_Param[0x3A0 / sizeof(ULONG)];
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef CO_MSG_PRINT_ON
#define D_IM_IIP_RESULT(statement)		result = statement
#else
#define D_IM_IIP_RESULT
#endif


extern VOID Im_IIP_Print_DebugStatus( VOID );
#ifdef CO_IIP_DEBUG
extern VOID Im_IIP_Print_ClockStatus( VOID );
#endif


#ifdef CO_DEBUG_ON_PC
#define ct_im_iip_print_hwen( unit_name, unitinftbl_reg )		Ddim_Print(( "%s HWEN = %u\n", (unit_name), (unitinftbl_reg).UNITINFTBL0.bit.HWEN ))
static VOID ct_im_iip_print_hwen_all( VOID )
{
	Im_IIP_On_Pclk();

	ct_im_iip_print_hwen( "LD0", IO_IIP.UNITINFTBL_LD0 );
	ct_im_iip_print_hwen( "LD1", IO_IIP.UNITINFTBL_LD1 );
	ct_im_iip_print_hwen( "LD2", IO_IIP.UNITINFTBL_LD2 );
	ct_im_iip_print_hwen( "LD3", IO_IIP.UNITINFTBL_LD3 );
	ct_im_iip_print_hwen( "LD4", IO_IIP.UNITINFTBL_LD4 );
	ct_im_iip_print_hwen( "LD5", IO_IIP.UNITINFTBL_LD5 );
	ct_im_iip_print_hwen( "LD6", IO_IIP.UNITINFTBL_LD6 );
	ct_im_iip_print_hwen( "LD7", IO_IIP.UNITINFTBL_LD7 );
	ct_im_iip_print_hwen( "SPF",   IO_IIP.UNITINFTBL_FLT );
	ct_im_iip_print_hwen( "AFN0", IO_IIP.UNITINFTBL_AFN0 );
	ct_im_iip_print_hwen( "AFN1", IO_IIP.UNITINFTBL_AFN1 );
	ct_im_iip_print_hwen( "AFN2", IO_IIP.UNITINFTBL_AFN2 );
	ct_im_iip_print_hwen( "AFN3", IO_IIP.UNITINFTBL_AFN3 );
	ct_im_iip_print_hwen( "FRCT0", IO_IIP.UNITINFTBL_FRC0 );
	ct_im_iip_print_hwen( "FRCT1", IO_IIP.UNITINFTBL_FRC1 );
	ct_im_iip_print_hwen( "MON",  IO_IIP.UNITINFTBL_MON );
	ct_im_iip_print_hwen( "SL0", IO_IIP.UNITINFTBL_SL0 );
	ct_im_iip_print_hwen( "SL1", IO_IIP.UNITINFTBL_SL1 );
	ct_im_iip_print_hwen( "SL2", IO_IIP.UNITINFTBL_SL2 );
	ct_im_iip_print_hwen( "SL3", IO_IIP.UNITINFTBL_SL3 );
	ct_im_iip_print_hwen( "SL4", IO_IIP.UNITINFTBL_SL4 );
	ct_im_iip_print_hwen( "SL5", IO_IIP.UNITINFTBL_SL5 );
	ct_im_iip_print_hwen( "SL6", IO_IIP.UNITINFTBL_SL6 );
	ct_im_iip_print_hwen( "SL7", IO_IIP.UNITINFTBL_SL7 );
	ct_im_iip_print_hwen( "SL8", IO_IIP.UNITINFTBL_SL8 );
	ct_im_iip_print_hwen( "CSC0", IO_IIP.UNITINFTBL_CSC0 );
	ct_im_iip_print_hwen( "CSC1", IO_IIP.UNITINFTBL_CSC1 );
	ct_im_iip_print_hwen( "CSC2", IO_IIP.UNITINFTBL_CSC2 );
	ct_im_iip_print_hwen( "CSC3", IO_IIP.UNITINFTBL_CSC3 );
	ct_im_iip_print_hwen( "LUT",   IO_IIP.UNITINFTBL_LUT );
	ct_im_iip_print_hwen( "GPC",   IO_IIP.UNITINFTBL_GPC );
	ct_im_iip_print_hwen( "BLD0", IO_IIP.UNITINFTBL_BLD0 );
	ct_im_iip_print_hwen( "BLD1", IO_IIP.UNITINFTBL_BLD1 );
	ct_im_iip_print_hwen( "CFL0", IO_IIP.UNITINFTBL_CFL0 );
	ct_im_iip_print_hwen( "MFT", IO_IIP.UNITINFTBL_MFT );
	Im_IIP_Off_Pclk();
}

static VOID ct_im_iip_set_hwen_auto_all( VOID )
{
	struct io_iip_uinftbl uinftbl;

	memset( &uinftbl, '\0', sizeof(uinftbl) );
	uinftbl.UNITINFTBL0.bit.HWEN = D_IM_IIP_HW_CTRL_AUTO;

	IO_IIP.UNITINFTBL_LD0 = uinftbl;
	IO_IIP.UNITINFTBL_LD1 = uinftbl;
	IO_IIP.UNITINFTBL_LD2 = uinftbl;
	IO_IIP.UNITINFTBL_LD3 = uinftbl;
	IO_IIP.UNITINFTBL_LD4 = uinftbl;
	IO_IIP.UNITINFTBL_LD5 = uinftbl;
	IO_IIP.UNITINFTBL_LD6 = uinftbl;
	IO_IIP.UNITINFTBL_LD7 = uinftbl;
	IO_IIP.UNITINFTBL_FLT = uinftbl;
	IO_IIP.UNITINFTBL_AFN0 = uinftbl;
	IO_IIP.UNITINFTBL_AFN1 = uinftbl;
	IO_IIP.UNITINFTBL_AFN2 = uinftbl;
	IO_IIP.UNITINFTBL_AFN3 = uinftbl;
	IO_IIP.UNITINFTBL_FRC0 = uinftbl;
	IO_IIP.UNITINFTBL_FRC1 = uinftbl;
	IO_IIP.UNITINFTBL_MON = uinftbl;
	IO_IIP.UNITINFTBL_SL0 = uinftbl;
	IO_IIP.UNITINFTBL_SL1 = uinftbl;
	IO_IIP.UNITINFTBL_SL2 = uinftbl;
	IO_IIP.UNITINFTBL_SL3 = uinftbl;
	IO_IIP.UNITINFTBL_SL4 = uinftbl;
	IO_IIP.UNITINFTBL_SL5 = uinftbl;
	IO_IIP.UNITINFTBL_SL6 = uinftbl;
	IO_IIP.UNITINFTBL_SL7 = uinftbl;
	IO_IIP.UNITINFTBL_SL8 = uinftbl;
	IO_IIP.UNITINFTBL_CSC0 = uinftbl;
	IO_IIP.UNITINFTBL_CSC1 = uinftbl;
	IO_IIP.UNITINFTBL_CSC2 = uinftbl;
	IO_IIP.UNITINFTBL_CSC3 = uinftbl;
	IO_IIP.UNITINFTBL_LUT = uinftbl;
	IO_IIP.UNITINFTBL_GPC = uinftbl;
	IO_IIP.UNITINFTBL_BLD0 = uinftbl;
	IO_IIP.UNITINFTBL_BLD1 = uinftbl;
	IO_IIP.UNITINFTBL_CFL0 = uinftbl;
	IO_IIP.UNITINFTBL_MFT = uinftbl;
}
#endif // CO_DEBUG_ON_PC


static VOID ct_im_iip_print_base( VOID )
{
	Im_IIP_On_Pclk();

	Ddim_Print(( "IZACTL1: 0x%lx\n", IO_IIP.IZACTL1.word ));
	Ddim_Print(( "INTEN0: 0x%lx\n", IO_IIP.INTEN0.word ));
	Ddim_Print(( "INTEN1: 0x%lx\n", IO_IIP.INTEN1.word ));
	Ddim_Print(( "INTEN2: 0x%lx\n", IO_IIP.INTEN2.word ));
	Ddim_Print(( "LINE_VAL_INT_SL0: 0x%lx\n", IO_IIP.LINE_VAL_INT_SL0.word ));
	Ddim_Print(( "LINE_VAL_INT_SL1: 0x%lx\n", IO_IIP.LINE_VAL_INT_SL1.word ));
	Ddim_Print(( "LINE_VAL_INT_SL2: 0x%lx\n", IO_IIP.LINE_VAL_INT_SL2.word ));
	Ddim_Print(( "LINE_VAL_INT_SL3: 0x%lx\n", IO_IIP.LINE_VAL_INT_SL3.word ));
	Ddim_Print(( "LINE_VAL_INT_SL4: 0x%lx\n", IO_IIP.LINE_VAL_INT_SL4.word ));
	Ddim_Print(( "LINE_VAL_INT_SL5: 0x%lx\n", IO_IIP.LINE_VAL_INT_SL5.word ));
	Ddim_Print(( "LINE_VAL_INT_SL6: 0x%lx\n", IO_IIP.LINE_VAL_INT_SL6.word ));
	Ddim_Print(( "LINE_VAL_INT_SL7: 0x%lx\n", IO_IIP.LINE_VAL_INT_SL7.word ));
	Ddim_Print(( "LINE_VAL_INT_SL8: 0x%lx\n", IO_IIP.LINE_VAL_INT_SL8.word ));
	Ddim_Print(( "PDCCTL[0]: 0x%lx\n", IO_IIP.PDCCTL[0].word ));
	Ddim_Print(( "PDCCTL[1]: 0x%lx\n", IO_IIP.PDCCTL[1].word ));
	Ddim_Print(( "PDCCTL[2]: 0x%lx\n", IO_IIP.PDCCTL[2].word ));

	Im_IIP_Off_Pclk();
}

static VOID ct_im_iip_print_pixfmttbl( const UINT32 pixid )
{
	volatile struct io_iip_pixfmttbl*	pPIXFMTTBL;

	pPIXFMTTBL = &IO_IIP.PIXFMTTBL[pixid];

	Im_IIP_On_Pclk();

	Ddim_Print(( "[%u] PIXFMTTBL_0 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_0.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_1 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_1.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_2 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_2.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_3 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_3.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_4 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_4.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_5 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_5.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_6 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_6.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_7 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_7.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_8 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_8.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_9 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_9.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_10 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_10.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_11 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_11.word ));
	Ddim_Print(( "[%u] PIXFMTTBL_12 = 0x%08lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_12.word ));

	Im_IIP_Off_Pclk();
}

#endif //CO_CT_IM_IIP_DISABLE

VOID CT_Im_IIP_Print_Pixfmttbl_Detail( const UINT32 pixid )
{
	volatile struct io_iip_pixfmttbl*	pPIXFMTTBL;
	static const CHAR* const ftype_name_tbl[8] = {
		"E_IM_IIP_FTYPE_Y_PACKED_CBCR",
		"E_IM_IIP_FTYPE_CHUNKY",
		"E_IM_IIP_FTYPE_PLANE",
	};
	static const CHAR* const pfmt_name_tbl[16] = {
		"E_IM_IIP_PFMT_YCC422",
		"E_IM_IIP_PFMT_YCC444",
		"E_IM_IIP_PFMT_YCC420",
		"E_IM_IIP_PFMT_BAYER",
		"E_IM_IIP_PFMT_CBCR",
		"E_IM_IIP_PFMT_VIDEO",
		"E_IM_IIP_PFMT_PLANE",
		"E_IM_IIP_PFMT_RGB565",
		"E_IM_IIP_PFMT_RGBA5551",
		"E_IM_IIP_PFMT_RGBA4444",
	};
	static const CHAR* const pdepth_name_tbl[8] = {
		"D_IM_IIP_PDEPTH_8BITS",
		"D_IM_IIP_PDEPTH_12BITS",
		"D_IM_IIP_PDEPTH_16BITS",
	};
	static const CHAR* const adepth_name_tbl[8] = {
		"E_IM_IIP_ADEPTH_8BITS",
		"(null)",
		"(null)",
		"E_IM_IIP_ADEPTH_4BITS",
	};
	static const CHAR* const sgX_name_tbl[2] = {
		"D_IM_IIP_UNSIGNED_DATA",
		"D_IM_IIP_SIGNED_DATA",
	};
	static const CHAR* const asubspl_name_tbl[4] = {
		"D_IM_IIP_ALPHA_NO_SUBSAMP",
		"D_IM_IIP_ALPHA_H_SUBSAMP",
		"D_IM_IIP_ALPHA_HV_SUBSAMP",
	};
	static const CHAR* const oarbhv_name_tbl[2] = {
		"D_IM_IIP_OUTSIDE_IMG_FIXED_VALUE",
		"D_IM_IIP_OUTSIDE_IMG_EDGE",
	};
	static const CHAR* const chkyordr_name_tbl[4] = {
		"D_IM_IIP_CHUNKY_COLOR_Y0_G",
		"D_IM_IIP_CHUNKY_COLOR_CB_B",
		"D_IM_IIP_CHUNKY_COLOR_CR_R",
		"D_IM_IIP_CHUNKY_COLOR_Y1_A",
	};

	pPIXFMTTBL = &IO_IIP.PIXFMTTBL[pixid];

	Im_IIP_On_Pclk();

	DDIM_User_Printf( "[%u] PIXFMTTBL.FTYPE = %s\n", pixid, ftype_name_tbl[pPIXFMTTBL->PIXFMTTBL_0.bit.FTYPE] );
	DDIM_User_Printf( "[%u] PIXFMTTBL.PFMT = %s\n", pixid, pfmt_name_tbl[pPIXFMTTBL->PIXFMTTBL_0.bit.PFMT] );
	DDIM_User_Printf( "[%u] PIXFMTTBL.ALPHA = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_0.bit.ALPHA );
	DDIM_User_Printf( "[%u] PIXFMTTBL.PDEPTH = %s\n", pixid, pdepth_name_tbl[pPIXFMTTBL->PIXFMTTBL_0.bit.PDEPTH] );
	DDIM_User_Printf( "[%u] PIXFMTTBL.ADEPTH = %s\n", pixid, adepth_name_tbl[pPIXFMTTBL->PIXFMTTBL_0.bit.ADEPTH] );
	DDIM_User_Printf( "[%u] PIXFMTTBL.CHKYORDR D0=%s D1=%s D2=%s D3=%s\n",
				pixid,
				chkyordr_name_tbl[(pPIXFMTTBL->PIXFMTTBL_0.bit.CHKYORDR & 0x03)],
				chkyordr_name_tbl[(pPIXFMTTBL->PIXFMTTBL_0.bit.CHKYORDR & 0x0c) >> 2],
				chkyordr_name_tbl[(pPIXFMTTBL->PIXFMTTBL_0.bit.CHKYORDR & 0x30) >> 4],
				chkyordr_name_tbl[(pPIXFMTTBL->PIXFMTTBL_0.bit.CHKYORDR & 0xc0) >> 6]
				);
	DDIM_User_Printf( "[%u] PIXFMTTBL.SGYG = %s\n", pixid, sgX_name_tbl[pPIXFMTTBL->PIXFMTTBL_1.bit.SGYG] );
	DDIM_User_Printf( "[%u] PIXFMTTBL.SGB = %s\n", pixid, sgX_name_tbl[pPIXFMTTBL->PIXFMTTBL_1.bit.SGB] );
	DDIM_User_Printf( "[%u] PIXFMTTBL.SGR = %s\n", pixid, sgX_name_tbl[pPIXFMTTBL->PIXFMTTBL_1.bit.SGR] );
	DDIM_User_Printf( "[%u] PIXFMTTBL.SGA = %s\n", pixid, sgX_name_tbl[pPIXFMTTBL->PIXFMTTBL_1.bit.SGA] );
	DDIM_User_Printf( "[%u] PIXFMTTBL.ASUBSPL = %s\n", pixid, asubspl_name_tbl[pPIXFMTTBL->PIXFMTTBL_1.bit.ASUBSPL] );
	DDIM_User_Printf( "[%u] PIXFMTTBL.MTIFYG = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFYG );
	DDIM_User_Printf( "[%u] PIXFMTTBL.MTIFB = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFB );
	DDIM_User_Printf( "[%u] PIXFMTTBL.MTIFR = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFR );
	DDIM_User_Printf( "[%u] PIXFMTTBL.MTIFA = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFA );
	DDIM_User_Printf( "[%u] PIXFMTTBL.OARBHV = %s\n", pixid, oarbhv_name_tbl[pPIXFMTTBL->PIXFMTTBL_1.bit.OARBHV] );
	DDIM_User_Printf( "[%u] PIXFMTTBL.OARVALYG = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_2.bit.OARVALYG );
	DDIM_User_Printf( "[%u] PIXFMTTBL.OARVALB = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_2.bit.OARVALB );
	DDIM_User_Printf( "[%u] PIXFMTTBL.OARVALR = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_3.bit.OARVALR );
	DDIM_User_Printf( "[%u] PIXFMTTBL.OARVALA = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_3.bit.OARVALA );
	DDIM_User_Printf( "[%u] PIXFMTTBL.GHSZYG = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_4.bit.GHSZYG );
	DDIM_User_Printf( "[%u] PIXFMTTBL.GHSZB = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_5.bit.GHSZB );
	DDIM_User_Printf( "[%u] PIXFMTTBL.GHSZR = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_6.bit.GHSZR );
	DDIM_User_Printf( "[%u] PIXFMTTBL.GHSZA = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_7.bit.GHSZA );
	DDIM_User_Printf( "[%u] PIXFMTTBL.PICHSZ = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_8.bit.PICHSZ );
	DDIM_User_Printf( "[%u] PIXFMTTBL.PICVSZ = %u\n", pixid, pPIXFMTTBL->PIXFMTTBL_8.bit.PICVSZ );
	DDIM_User_Printf( "[%u] PIXFMTTBL.ADRSYG = 0x%lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_9.bit.ADRSYG );
	DDIM_User_Printf( "[%u] PIXFMTTBL.ADRSB = 0x%lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_10.bit.ADRSB );
	DDIM_User_Printf( "[%u] PIXFMTTBL.ADRSR = 0x%lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_11.bit.ADRSR );
	DDIM_User_Printf( "[%u] PIXFMTTBL.ADRSA = 0x%lx\n", pixid, pPIXFMTTBL->PIXFMTTBL_12.bit.ADRSA );

	Im_IIP_Off_Pclk();
}

#ifndef CO_CT_IM_IIP_DISABLE

VOID CT_Im_IIP_Print_Pixfmttbl_Byid( const E_IM_IIP_PIXID pixid )
{
	UINT32 loopcnt;

	for( loopcnt = 0; loopcnt < D_IM_IIP_PIXID_MAXNUM; loopcnt++ ) {
		if( pixid == (1 << loopcnt) ) {
//			ct_im_iip_print_pixfmttbl( loopcnt );
			CT_Im_IIP_Print_Pixfmttbl_Detail( loopcnt );
			return;
		}
	}

	DDIM_User_Printf( "Unknown PIXID = 0x%x\n", pixid );
}

static const CHAR* ct_im_iip_conv_portid_to_name( const E_IM_IIP_PARAM_PORTID portid )
{
	switch( portid ) {
		case E_IM_IIP_PARAM_PORTID_LD0:
			return "E_IM_IIP_PARAM_PORTID_LD0";
		case E_IM_IIP_PARAM_PORTID_LD1:
			return "E_IM_IIP_PARAM_PORTID_LD1";
		case E_IM_IIP_PARAM_PORTID_LD2:
			return "E_IM_IIP_PARAM_PORTID_LD2";
		case E_IM_IIP_PARAM_PORTID_FLT_P0:
			return "E_IM_IIP_PARAM_PORTID_FLT_P0";	// Only port0
		case E_IM_IIP_PARAM_PORTID_AFN0:
			return "E_IM_IIP_PARAM_PORTID_AFN0";
		case E_IM_IIP_PARAM_PORTID_AFN1:
			return "E_IM_IIP_PARAM_PORTID_AFN1";
		case E_IM_IIP_PARAM_PORTID_AFN2:
			return "E_IM_IIP_PARAM_PORTID_AFN2";
		case E_IM_IIP_PARAM_PORTID_AFN3:
			return "E_IM_IIP_PARAM_PORTID_AFN3";
		case E_IM_IIP_PARAM_PORTID_FRECT0:
			return "E_IM_IIP_PARAM_PORTID_FRECT0";
		case E_IM_IIP_PARAM_PORTID_NONE:
			return "E_IM_IIP_PARAM_PORTID_NONE";		// Port not found
		case E_IM_IIP_PARAM_PORTID_SL0:
			return "E_IM_IIP_PARAM_PORTID_SL0";
		case E_IM_IIP_PARAM_PORTID_SL1:
			return "E_IM_IIP_PARAM_PORTID_SL1";
		case E_IM_IIP_PARAM_PORTID_SL2:
			return "E_IM_IIP_PARAM_PORTID_SL2";
		case E_IM_IIP_PARAM_PORTID_SL3:
			return "E_IM_IIP_PARAM_PORTID_SL3";
		case E_IM_IIP_PARAM_PORTID_SL4:
			return "E_IM_IIP_PARAM_PORTID_SL4";
		case E_IM_IIP_PARAM_PORTID_SL5:
			return "E_IM_IIP_PARAM_PORTID_SL5";
		case E_IM_IIP_PARAM_PORTID_CSC0:
			return "E_IM_IIP_PARAM_PORTID_CSC0";
		case E_IM_IIP_PARAM_PORTID_CSC1:
			return "E_IM_IIP_PARAM_PORTID_CSC1";
		case E_IM_IIP_PARAM_PORTID_LUT_P0:
			return "E_IM_IIP_PARAM_PORTID_LUT_P0";	// Only port0
		case E_IM_IIP_PARAM_PORTID_GPC_P0:
			return "E_IM_IIP_PARAM_PORTID_GPC_P0";	// Only port0
		case E_IM_IIP_PARAM_PORTID_BLEND0_P0:
			return "E_IM_IIP_PARAM_PORTID_BLEND0_P0";
		case E_IM_IIP_PARAM_PORTID_BLEND0_P1:
			return "E_IM_IIP_PARAM_PORTID_BLEND0_P1";
		case E_IM_IIP_PARAM_PORTID_BLEND0_P2:
			return "E_IM_IIP_PARAM_PORTID_BLEND0_P2";
		case E_IM_IIP_PARAM_PORTID_BLEND1_P0:
			return "E_IM_IIP_PARAM_PORTID_BLEND1_P0";
		case E_IM_IIP_PARAM_PORTID_BLEND1_P1:
			return "E_IM_IIP_PARAM_PORTID_BLEND2_P1";
		case E_IM_IIP_PARAM_PORTID_BLEND1_P2:
			return "E_IM_IIP_PARAM_PORTID_BLEND1_P2";
		case E_IM_IIP_PARAM_PORTID_FRECT1:
			return "E_IM_IIP_PARAM_PORTID_FRECT1";
		case E_IM_IIP_PARAM_PORTID_CFL0_P0:
			return "E_IM_IIP_PARAM_PORTID_CFL0_P0";	// Only port0
		case E_IM_IIP_PARAM_PORTID_CSC2:
			return "E_IM_IIP_PARAM_PORTID_CSC2";
		case E_IM_IIP_PARAM_PORTID_CSC3:
			return "E_IM_IIP_PARAM_PORTID_CSC3";
		case E_IM_IIP_PARAM_PORTID_SL6:
			return "E_IM_IIP_PARAM_PORTID_SL6";
		case E_IM_IIP_PARAM_PORTID_SL7:
			return "E_IM_IIP_PARAM_PORTID_SL7";
		case E_IM_IIP_PARAM_PORTID_SL8:
			return "E_IM_IIP_PARAM_PORTID_SL8";
		case E_IM_IIP_PARAM_PORTID_LD3:
			return "E_IM_IIP_PARAM_PORTID_LD3";
		case E_IM_IIP_PARAM_PORTID_LD4:
			return "E_IM_IIP_PARAM_PORTID_LD4";
		case E_IM_IIP_PARAM_PORTID_LD5:
			return "E_IM_IIP_PARAM_PORTID_LD5";
		case E_IM_IIP_PARAM_PORTID_LD6:
			return "E_IM_IIP_PARAM_PORTID_LD6";
		case E_IM_IIP_PARAM_PORTID_LD7:
			return "E_IM_IIP_PARAM_PORTID_LD7";
		case E_IM_IIP_PARAM_PORTID_MFT:
			return "E_IM_IIP_PARAM_PORTID_MFT";
		default:
			return "(null)";	// Invalid parameter
	}
}

static VOID ct_im_iip_print_afn_tbl( const ULONG addr )
{
	const T_IM_IIP_PARAM_AFN_OPCOL_0* afntbl = (T_IM_IIP_PARAM_AFN_OPCOL_0*)addr;

	if( afntbl == NULL ) {
		return;
	}

	Ddim_Print(( "STAX = 0x%x %f\n", afntbl->STAXY0.bit.STAX, ct_im_iip_conv_afntbl_float(afntbl->STAXY0.bit.STAX) ));
	Ddim_Print(( "STAY = 0x%x %f\n", afntbl->STAXY0.bit.STAY, ct_im_iip_conv_afntbl_float(afntbl->STAXY0.bit.STAY) ));
	Ddim_Print(( "DXX = 0x%x %f\n", afntbl->D_OUT_IN_0.bit.DXX, ct_im_iip_conv_afntbl_float(afntbl->D_OUT_IN_0.bit.DXX) ));
	Ddim_Print(( "DXY = 0x%x %f\n", afntbl->D_OUT_IN_0.bit.DXY, ct_im_iip_conv_afntbl_float(afntbl->D_OUT_IN_0.bit.DXY) ));
	Ddim_Print(( "DYX = 0x%x %f\n", afntbl->D_OUT_IN_0.bit.DYX, ct_im_iip_conv_afntbl_float(afntbl->D_OUT_IN_0.bit.DYX) ));
	Ddim_Print(( "DYY = 0x%x %f\n", afntbl->D_OUT_IN_0.bit.DYY, ct_im_iip_conv_afntbl_float(afntbl->D_OUT_IN_0.bit.DYY) ));
	Ddim_Print(( "AFNDHSZ = %u\n", afntbl->SZ.bit.AFNDHSZ ));
	Ddim_Print(( "AFNDVSZ = %u\n", afntbl->SZ.bit.AFNDVSZ ));
}

static VOID ct_im_iip_get_upsu_name( const UINT32 idx, const ULONG upsu )
{
	static const CHAR* const upsu_name_tbl[16] = {
		[E_IM_IIP_PARAM_UPSU_NONE] = "E_IM_IIP_PARAM_UPSU_NONE",
		[E_IM_IIP_PARAM_UPSU_B2Y] = "E_IM_IIP_PARAM_UPSU_B2Y",
		[E_IM_IIP_PARAM_UPSU_AFN0] = "E_IM_IIP_PARAM_UPSU_AFN0",
		[E_IM_IIP_PARAM_UPSU_AFN1] = "E_IM_IIP_PARAM_UPSU_AFN1",
		[E_IM_IIP_PARAM_UPSU_AFN2] = "E_IM_IIP_PARAM_UPSU_AFN2",
		[E_IM_IIP_PARAM_UPSU_AFN3] = "E_IM_IIP_PARAM_UPSU_AFN3",
		[E_IM_IIP_PARAM_UPSU_FRECT0] = "E_IM_IIP_PARAM_UPSU_FRECT0",
		[E_IM_IIP_PARAM_UPSU_FRECT1] = "E_IM_IIP_PARAM_UPSU_FRECT1",
		[E_IM_IIP_PARAM_UPSU_FLT] = "E_IM_IIP_PARAM_UPSU_FLT",
		[E_IM_IIP_PARAM_UPSU_LD0] = "E_IM_IIP_PARAM_UPSU_LD0",
		[E_IM_IIP_PARAM_UPSU_LD1] = "E_IM_IIP_PARAM_UPSU_LD1",
		[E_IM_IIP_PARAM_UPSU_LD2] = "E_IM_IIP_PARAM_UPSU_LD2",
		[E_IM_IIP_PARAM_UPSU_LD3] = "E_IM_IIP_PARAM_UPSU_LD3",
		[E_IM_IIP_PARAM_UPSU_LD4] = "E_IM_IIP_PARAM_UPSU_LD4",
		[E_IM_IIP_PARAM_UPSU_LD5] = "E_IM_IIP_PARAM_UPSU_LD5",
		[E_IM_IIP_PARAM_UPSU_LD6] = "E_IM_IIP_PARAM_UPSU_LD6",
	};

	Ddim_Print(( "UPLK_TARGET.UPSU%u = %s\n", idx, upsu_name_tbl[upsu] ));
}

static VOID ct_im_iip_get_dwsu_name( const UINT32 idx, const ULONG dwsu )
{
	static const CHAR* const dwsu_name_tbl[16] = {
		[E_IM_IIP_PARAM_DWSU_NONE] = "E_IM_IIP_PARAM_DWSU_NONE",
		[E_IM_IIP_PARAM_DWSU_AFN0] = "E_IM_IIP_PARAM_DWSU_AFN0",
		[E_IM_IIP_PARAM_DWSU_AFN1] = "E_IM_IIP_PARAM_DWSU_AFN1",
		[E_IM_IIP_PARAM_DWSU_AFN2] = "E_IM_IIP_PARAM_DWSU_AFN2",
		[E_IM_IIP_PARAM_DWSU_AFN3] = "E_IM_IIP_PARAM_DWSU_AFN3",
		[E_IM_IIP_PARAM_DWSU_FLT] = "E_IM_IIP_PARAM_DWSU_FLT",
		[E_IM_IIP_PARAM_DWSU_LD0] = "E_IM_IIP_PARAM_DWSU_LD0",
		[E_IM_IIP_PARAM_DWSU_LD1] = "E_IM_IIP_PARAM_DWSU_LD1",
		[E_IM_IIP_PARAM_DWSU_LD2] = "E_IM_IIP_PARAM_DWSU_LD2",
		[E_IM_IIP_PARAM_DWSU_LD3] = "E_IM_IIP_PARAM_DWSU_LD3",
		[E_IM_IIP_PARAM_DWSU_LD4] = "E_IM_IIP_PARAM_DWSU_LD4",
		[E_IM_IIP_PARAM_DWSU_LD5] = "E_IM_IIP_PARAM_DWSU_LD5",
		[E_IM_IIP_PARAM_DWSU_LD6] = "E_IM_IIP_PARAM_DWSU_LD6",
	};
	const CHAR* name;

	if( dwsu_name_tbl[dwsu] == NULL ) {
		name = "(UNKNOWN)";
	}
	else {
		name = dwsu_name_tbl[dwsu];
	}

	Ddim_Print(( "DWLK_TARGET.DWSU%u = %s\n", idx, name ));
}

static const CHAR* ct_im_iip_get_cubsel_name( const ULONG cubsel )
{
	static const CHAR* const cubsel_name_tbl[8] = {
		[E_IM_IIP_PARAM_CUBSEL_AFN0] = "E_IM_IIP_PARAM_CUBSEL_AFN0",
		[E_IM_IIP_PARAM_CUBSEL_AFN1] = "E_IM_IIP_PARAM_CUBSEL_AFN1",
		[E_IM_IIP_PARAM_CUBSEL_AFN2] = "E_IM_IIP_PARAM_CUBSEL_AFN2",
		[E_IM_IIP_PARAM_CUBSEL_AFN3] = "E_IM_IIP_PARAM_CUBSEL_AFN3",
		[E_IM_IIP_PARAM_CUBSEL_FRECT0] = "E_IM_IIP_PARAM_CUBSEL_FRECT0",
		[E_IM_IIP_PARAM_CUBSEL_FRECT1] = "E_IM_IIP_PARAM_CUBSEL_FRECT1",
	};

	if( cubsel_name_tbl[cubsel] == NULL ) {
		return "(UNKNOWN)";
	}
	else {
		return cubsel_name_tbl[cubsel];
	}
}

static VOID ct_im_iip_print_afn_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid )
{
	static const CHAR* const itmd_name_tbl[4] = {
		"E_IM_IIP_PARAM_ITMD_BI_LINEAR",
		"E_IM_IIP_PARAM_ITMD_NEAREST",
		"E_IM_IIP_PARAM_ITMD_BI_CUBIC",
	};
	static const CHAR* const ghsz_name_tbl[2] = {
		"E_IM_IIP_PARAM_GHSZ_64PIX",
		"E_IM_IIP_PARAM_GHSZ_32PIX",
	};
	static const CHAR* const gvsz_name_tbl[16] = {
		"E_IM_IIP_PARAM_GVSZ_SAME_AS_VERTICAL_PIX",
		"E_IM_IIP_PARAM_GVSZ_2PIX",
		"E_IM_IIP_PARAM_GVSZ_4PIX",
		"E_IM_IIP_PARAM_GVSZ_8PIX",
		"E_IM_IIP_PARAM_GVSZ_16PIX",
		"E_IM_IIP_PARAM_GVSZ_32PIX",
		"E_IM_IIP_PARAM_GVSZ_64PIX",
		"E_IM_IIP_PARAM_GVSZ_128PIX",
		"E_IM_IIP_PARAM_GVSZ_256PIX",
	};
	const T_IM_IIP_PARAM_AFN* afn = (T_IM_IIP_PARAM_AFN*)pUNITINFTBL->UNITINFTBL2.word;

	if( afn == NULL ) {
		return;
	}

	Ddim_Print(( "AFNTOPCNF.WAITCONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)afn->AFNTOPCNF.bit.WAITCONF) ));
	Ddim_Print(( "AFNCTL.GHSZ = %s\n", ghsz_name_tbl[afn->AFNCTL.bit.GHSZ] ));
	Ddim_Print(( "AFNCTL.GVSZ = %s\n", gvsz_name_tbl[afn->AFNCTL.bit.GVSZ] ));
	Ddim_Print(( "AFNCTL.HSMD = %u\n", afn->AFNCTL.bit.HSMD ));
	Ddim_Print(( "PFCTL.PFSP1 = %u\n", afn->PFCTL.bit.PFSP1 ));
	Ddim_Print(( "PFCTL.PFSP2 = %u\n", afn->PFCTL.bit.PFSP2 ));
	Ddim_Print(( "PIXIDEF.IPIXID = %u\n", afn->PIXIDEF.bit.IPIXID ));
	Ddim_Print(( "CALMETHOD.ITMD = %s\n", itmd_name_tbl[afn->CALMETHOD.bit.ITMD] ));
	Ddim_Print(( "CALMETHOD.DANTI = %u\n", afn->CALMETHOD.bit.DANTI ));
	Ddim_Print(( "CALMETHOD.AANTI = %u\n", afn->CALMETHOD.bit.AANTI ));
	Ddim_Print(( "CALMETHOD.CUBSEL = %s\n", ct_im_iip_get_cubsel_name( afn->CALMETHOD.bit.CUBSEL ) ));
	Ddim_Print(( "CLIPLVLY.CLIPLVLYH = %u\n", afn->CLIPLVLY.bit.CLIPLVLYH ));
	Ddim_Print(( "CLIPLVLY.CLIPLVLYL = %u\n", afn->CLIPLVLY.bit.CLIPLVLYL ));
	Ddim_Print(( "CLIPLVLB.CLIPLVLBH = %u\n", afn->CLIPLVLB.bit.CLIPLVLBH ));
	Ddim_Print(( "CLIPLVLB.CLIPLVLBL = %u\n", afn->CLIPLVLB.bit.CLIPLVLBL ));
	Ddim_Print(( "CLIPLVLR.CLIPLVLRH = %u\n", afn->CLIPLVLR.bit.CLIPLVLRH ));
	Ddim_Print(( "CLIPLVLR.CLIPLVLRL = %u\n", afn->CLIPLVLR.bit.CLIPLVLRL ));
	Ddim_Print(( "CLIPLVLA.CLIPLVLAH = %u\n", afn->CLIPLVLA.bit.CLIPLVLAH ));
	Ddim_Print(( "CLIPLVLA.CLIPLVLAL = %u\n", afn->CLIPLVLA.bit.CLIPLVLAL ));
	Ddim_Print(( "OUTSIZE.OUTHSZ = %u\n", afn->OUTSIZE.bit.OUTHSZ ));
	Ddim_Print(( "OUTSIZE.OUTVSZ = %u\n", afn->OUTSIZE.bit.OUTVSZ ));
	Ddim_Print(( "OUTSTARTPOS.STARTX = %u\n", afn->OUTSTARTPOS.bit.STARTX ));
	Ddim_Print(( "OUTSTARTPOS.STARTY = %u\n", afn->OUTSTARTPOS.bit.STARTY ));
	ct_im_iip_get_upsu_name( 0, afn->UPLK_TARGET.bit.UPSU0 );
	ct_im_iip_get_upsu_name( 1, afn->UPLK_TARGET.bit.UPSU1 );
	ct_im_iip_get_upsu_name( 2, afn->UPLK_TARGET.bit.UPSU2 );
	Ddim_Print(( "LKNUM.UPNUM = %u\n", afn->LKNUM.bit.UPNUM ));
	Ddim_Print(( "RING.URINGSIZE = %u\n", afn->RING.bit.URINGSIZE ));
	Ddim_Print(( "RING.UMARGINIYSZ = %u\n", afn->RING.bit.UMARGINIYSZ ));
	ct_im_iip_get_dwsu_name( 0, afn->DWLK_TARGET.bit.DWSU0 );
	ct_im_iip_get_dwsu_name( 1, afn->DWLK_TARGET.bit.DWSU1 );
	ct_im_iip_get_dwsu_name( 2, afn->DWLK_TARGET.bit.DWSU2 );
	Ddim_Print(( "LKNUM.DWNUM = %u\n", afn->LKNUM.bit.DWNUM ));
	Ddim_Print(( "RING.DRINGSIZE = %u\n", afn->RING.bit.DRINGSIZE ));
	Ddim_Print(( "AFNPA = 0x%lx\n", afn->AFNPA.word ));

	ct_im_iip_print_afn_tbl( afn->AFNPA.word );
}

static VOID ct_im_iip_print_frect_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid )
{
	static const CHAR* const itmd_name_tbl[4] = {
		"E_IM_IIP_PARAM_ITMD_BI_LINEAR",
		"E_IM_IIP_PARAM_ITMD_NEAREST",
		"E_IM_IIP_PARAM_ITMD_BI_CUBIC",
	};
	static const CHAR* const ghsz_name_tbl[2] = {
		"E_IM_IIP_PARAM_GHSZ_64PIX",
		"E_IM_IIP_PARAM_GHSZ_32PIX",
	};
	static const CHAR* const gvsz_name_tbl[16] = {
		"E_IM_IIP_PARAM_GVSZ_SAME_AS_VERTICAL_PIX",
		"E_IM_IIP_PARAM_GVSZ_2PIX",
		"E_IM_IIP_PARAM_GVSZ_4PIX",
		"E_IM_IIP_PARAM_GVSZ_8PIX",
		"E_IM_IIP_PARAM_GVSZ_16PIX",
		"E_IM_IIP_PARAM_GVSZ_32PIX",
		"E_IM_IIP_PARAM_GVSZ_64PIX",
		"E_IM_IIP_PARAM_GVSZ_128PIX",
		"E_IM_IIP_PARAM_GVSZ_256PIX",
	};
	const T_IM_IIP_PARAM_FRECT* fr = (T_IM_IIP_PARAM_FRECT*)pUNITINFTBL->UNITINFTBL2.word;

	if( fr == NULL ) {
		return;
	}

	Ddim_Print(( "FRTOPCNF.WAITCONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)fr->FRTOPCNF.bit.WAITCONF) ));
	Ddim_Print(( "FRCTL.GHSZ = %s\n", ghsz_name_tbl[fr->FRCTL.bit.GHSZ] ));
	Ddim_Print(( "FRCTL.GVSZ = %s\n", gvsz_name_tbl[fr->FRCTL.bit.GVSZ] ));
	Ddim_Print(( "FRCTL.HSMD = %u\n", fr->FRCTL.bit.HSMD ));
	Ddim_Print(( "PFCTL.PFSP1 = %u\n", fr->PFCTL.bit.PFSP1 ));
	Ddim_Print(( "PFCTL.PFSP2 = %u\n", fr->PFCTL.bit.PFSP2 ));
	Ddim_Print(( "PIXIDEF.IPIXID = %u\n", fr->PIXIDEF.bit.IPIXID ));
	Ddim_Print(( "CALMETHOD.ITMD = %s\n", itmd_name_tbl[fr->CALMETHOD.bit.ITMD] ));
	Ddim_Print(( "CALMETHOD.DANTI = %u\n", fr->CALMETHOD.bit.DANTI ));
	Ddim_Print(( "CALMETHOD.AANTI = %u\n", fr->CALMETHOD.bit.AANTI ));
	Ddim_Print(( "CALMETHOD.CUBSEL = %s\n", ct_im_iip_get_cubsel_name( fr->CALMETHOD.bit.CUBSEL ) ));
	Ddim_Print(( "CLIPLVLY.CLIPLVLYH = %u\n", fr->CLIPLVLY.bit.CLIPLVLYH ));
	Ddim_Print(( "CLIPLVLY.CLIPLVLYL = %u\n", fr->CLIPLVLY.bit.CLIPLVLYL ));
	Ddim_Print(( "CLIPLVLB.CLIPLVLBH = %u\n", fr->CLIPLVLB.bit.CLIPLVLBH ));
	Ddim_Print(( "CLIPLVLB.CLIPLVLBL = %u\n", fr->CLIPLVLB.bit.CLIPLVLBL ));
	Ddim_Print(( "CLIPLVLR.CLIPLVLRH = %u\n", fr->CLIPLVLR.bit.CLIPLVLRH ));
	Ddim_Print(( "CLIPLVLR.CLIPLVLRL = %u\n", fr->CLIPLVLR.bit.CLIPLVLRL ));
	Ddim_Print(( "CLIPLVLA.CLIPLVLAH = %u\n", fr->CLIPLVLA.bit.CLIPLVLAH ));
	Ddim_Print(( "CLIPLVLA.CLIPLVLAL = %u\n", fr->CLIPLVLA.bit.CLIPLVLAL ));
	Ddim_Print(( "OUTSIZE.OUTHSZ = %u\n", fr->OUTSIZE.bit.OUTHSZ ));
	Ddim_Print(( "OUTSIZE.OUTVSZ = %u\n", fr->OUTSIZE.bit.OUTVSZ ));
	Ddim_Print(( "OUTSTARTPOS.STARTX = %u\n", fr->OUTSTARTPOS.bit.STARTX ));
	Ddim_Print(( "OUTSTARTPOS.STARTY = %u\n", fr->OUTSTARTPOS.bit.STARTY ));
	ct_im_iip_get_upsu_name( 0, fr->UPLK_TARGET.bit.UPSU0 );
	ct_im_iip_get_upsu_name( 1, fr->UPLK_TARGET.bit.UPSU1 );
	ct_im_iip_get_upsu_name( 2, fr->UPLK_TARGET.bit.UPSU2 );
	Ddim_Print(( "LKNUM.UPNUM = %u\n", fr->LKNUM.bit.UPNUM ));
	Ddim_Print(( "RING.URINGSIZE = %u\n", fr->RING.bit.URINGSIZE ));
	Ddim_Print(( "RING.UMARGINIYSZ = %u\n", fr->RING.bit.UMARGINIYSZ ));
	ct_im_iip_get_dwsu_name( 0, fr->DWLK_TARGET.bit.DWSU0 );
	ct_im_iip_get_dwsu_name( 1, fr->DWLK_TARGET.bit.DWSU1 );
	ct_im_iip_get_dwsu_name( 2, fr->DWLK_TARGET.bit.DWSU2 );
	Ddim_Print(( "LKNUM.DWNUM = %u\n", fr->LKNUM.bit.DWNUM ));
	Ddim_Print(( "RING.DRINGSIZE = %u\n", fr->RING.bit.DRINGSIZE ));
	Ddim_Print(( "PADRS = 0x%lx\n", fr->PADRS.word ));
}

static VOID ct_im_iip_print_sl_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid )
{
	const T_IM_IIP_PARAM_STS*			sl = (T_IM_IIP_PARAM_STS*)pUNITINFTBL->UNITINFTBL2.word;
	volatile struct io_iip_pixfmttbl*	pPIXFMTTBL;
	UINT32								pixid;
	UINT32								mif_no_y;
	UINT32								mif_no_cb;
	UINT32								mif_no_cr;
	UINT32								mif_no_a;
	static const UINT32					mif_tbl_sl2[2] = { 2, 1 };
	static const UINT32					mif_tbl_sl5[2] = { 2, 0 };

	if( sl == NULL ) {
		return;
	}

	Ddim_Print(( "SL_TOPCNF0.DATACONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)sl->BASE.SL_TOPCNF0.bit.DATACONF) ));
	Ddim_Print(( "SL_TOPCNF0.WAITCONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)sl->BASE.SL_TOPCNF0.bit.WAITCONF) ));
	Ddim_Print(( "PIXIDDEF.bit.OPIXID = %u\n", sl->BASE.PIXIDDEF.bit.OPIXID ));
	Ddim_Print(( "SL_HSIZE.HSTART = %u\n", sl->BASE.SL_HSIZE.bit.HSTART ));
	Ddim_Print(( "SL_HSIZE.HEND = %u\n", sl->BASE.SL_HSIZE.bit.HEND ));
	Ddim_Print(( "SL_VSIZE.VSTART = %u\n", sl->BASE.SL_VSIZE.bit.VSTART ));
	Ddim_Print(( "SL_VSIZE.VEND = %u\n", sl->BASE.SL_VSIZE.bit.VEND ));
	Ddim_Print(( "SL_PHSZ.PHSZ0 = %u\n", sl->BASE.SL_PHSZ.bit.PHSZ0 ));
	Ddim_Print(( "SL_PHSZ.PHSZ1 = %u\n", sl->BASE.SL_PHSZ.bit.PHSZ1 ));
	Ddim_Print(( "SL_PVSZ.PVSZ0 = %u\n", sl->BASE.SL_PVSZ.bit.PVSZ0 ));
	Ddim_Print(( "SL_PVSZ.PVSZ1 = %u\n", sl->BASE.SL_PVSZ.bit.PVSZ1 ));
	Ddim_Print(( "CLPTH_U_0.CLPTH_U = %u\n", sl->BASE.CLPTH_U_0.bit.CLPTH_U ));
	Ddim_Print(( "CLPTH_L_0.CLPTH_L = %u\n", sl->BASE.CLPTH_L_0.bit.CLPTH_L ));

	pixid = sl->BASE.PIXIDDEF.bit.OPIXID;
	pPIXFMTTBL = &IO_IIP.PIXFMTTBL[pixid];

	switch( unitid ) {
		case E_IM_IIP_UNIT_ID_SL0:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL3:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL6:
			mif_no_y = 0;
			mif_no_cb = 0;
			mif_no_cr = 0;
			mif_no_a = 0;
			break;
		case E_IM_IIP_UNIT_ID_SL1:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL4:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL7:
			mif_no_y = 1;
			mif_no_cb = 1;
			mif_no_cr = 1;
			mif_no_a = 1;
			break;
		case E_IM_IIP_UNIT_ID_SL8:
			mif_no_y = 2;
			mif_no_cb = 2;
			mif_no_cr = 2;
			mif_no_a = 2;
			break;
		case E_IM_IIP_UNIT_ID_SL2:
			mif_no_y = mif_tbl_sl2[pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFYG];
			mif_no_cb = mif_tbl_sl2[pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFB];
			mif_no_cr = mif_tbl_sl2[pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFR];
			mif_no_a = mif_tbl_sl2[pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFA];
			break;
		case E_IM_IIP_UNIT_ID_SL5:
			mif_no_y = mif_tbl_sl5[pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFYG];
			mif_no_cb = mif_tbl_sl5[pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFB];
			mif_no_cr = mif_tbl_sl5[pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFR];
			mif_no_a = mif_tbl_sl5[pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFA];
			break;
		default:
			mif_no_y = 0xFF;
			mif_no_cb = 0xFF;
			mif_no_cr = 0xFF;
			mif_no_a = 0xFF;
			break;
	}

	DDIM_User_Printf( "MasterIF Y=%u(%u) Cb=%u(%u) Cr=%u(%u) A=%u(%u)\n",
			mif_no_y, pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFYG,
			mif_no_cb, pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFB,
			mif_no_cr, pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFR,
			mif_no_a, pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFA );
}

static VOID ct_im_iip_print_1d_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid )
{
	const T_IM_IIP_PARAM_1DL* pLD = (T_IM_IIP_PARAM_1DL*)pUNITINFTBL->UNITINFTBL2.word;

	if( pLD == NULL ) {
		return;
	}

	Ddim_Print(( "LD_TOPCNF0.WAITCONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)pLD->LD_TOPCNF0.bit.WAITCONF) ));
	Ddim_Print(( "PIXIDDEF.IPIXID  = %u\n", pLD->PIXIDDEF.bit.IPIXID ));
	Ddim_Print(( "LD_PREFETCH.PF_PDIST = %u\n", pLD->LD_PREFETCH.bit.PF_PDIST ));
	Ddim_Print(( "LD_HSIZE.HSTART = %u\n", pLD->LD_HSIZE.bit.HSTART ));
	Ddim_Print(( "LD_HSIZE.HEND = %u\n", pLD->LD_HSIZE.bit.HEND ));
	Ddim_Print(( "LD_VSIZE.VSTART = %u\n", pLD->LD_VSIZE.bit.VSTART ));
	Ddim_Print(( "LD_VSIZE.VEND = %u\n", pLD->LD_VSIZE.bit.VEND ));
	Ddim_Print(( "LD_PHSZ.PHSZ0 = %u\n", pLD->LD_PHSZ.bit.PHSZ0 ));
	Ddim_Print(( "LD_PHSZ.PHSZ1 = %u\n", pLD->LD_PHSZ.bit.PHSZ1 ));
	Ddim_Print(( "LD_PVSZ.PVSZ0 = %u\n", pLD->LD_PVSZ.bit.PVSZ0 ));
	Ddim_Print(( "LD_PVSZ.PVSZ1 = %u\n", pLD->LD_PVSZ.bit.PVSZ1 ));
	ct_im_iip_get_upsu_name( 0, pLD->UPLK_TARGET.bit.UPSU0 );
	ct_im_iip_get_upsu_name( 1, pLD->UPLK_TARGET.bit.UPSU1 );
	ct_im_iip_get_upsu_name( 2, pLD->UPLK_TARGET.bit.UPSU2 );
	Ddim_Print(( "LKNUM.UPNUM = %u\n", pLD->LKNUM.bit.UPNUM ));
	Ddim_Print(( "RING.URINGSIZE = %u\n", pLD->RING.bit.URINGSIZE ));
	Ddim_Print(( "RING.UMARGINIYSZ = %u\n", pLD->RING.bit.UMARGINIYSZ ));
	ct_im_iip_get_dwsu_name( 0, pLD->DWLK_TARGET.bit.DWSU0 );
	ct_im_iip_get_dwsu_name( 1, pLD->DWLK_TARGET.bit.DWSU1 );
	ct_im_iip_get_dwsu_name( 2, pLD->DWLK_TARGET.bit.DWSU2 );
	Ddim_Print(( "LKNUM.DWNUM = %u\n", pLD->LKNUM.bit.DWNUM ));
	Ddim_Print(( "RING.DRINGSIZE = %u\n", pLD->RING.bit.DRINGSIZE ));
}

static VOID ct_im_iip_print_csc_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid )
{
	const T_IM_IIP_PARAM_CSC* csc = (T_IM_IIP_PARAM_CSC*)pUNITINFTBL->UNITINFTBL2.word;
	UINT32 loopcnt;

	if( csc == NULL ) {
		return;
	}

	Ddim_Print(( "CSCTOPCNF.DATACONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)csc->CSCTOPCNF.bit.DATACONF) ));
	Ddim_Print(( "CSCTOPCNF.WAITCONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)csc->CSCTOPCNF.bit.WAITCONF) ));
	Ddim_Print(( "CSCK\n" ));
	for( loopcnt = 0; loopcnt < 4; loopcnt++ ) {
		Ddim_Print(( " %u %u %u %u\n",
					csc->CSCK[loopcnt].bit.CSCK_0,
					csc->CSCK[loopcnt].bit.CSCK_1,
					csc->CSCK[loopcnt].bit.CSCK_2,
					csc->CSCK[loopcnt].bit.CSCK_3
					));
	}
	Ddim_Print(( "MDSEL.MDSEL = 0x%x\n", csc->MDSEL.bit.MDSEL ));
	Ddim_Print(( "ALPSEL.ALPSEL = 0x%x\n", csc->ALPSEL.bit.ALPSEL ));
	Ddim_Print(( "ALPSEL.ALOSEL = 0x%x\n", csc->ALPSEL.bit.ALOSEL ));
	Ddim_Print(( "ALPVAL.ALPVAL = 0x%x\n", csc->ALPVAL.bit.ALPVAL ));
}

static VOID ct_im_iip_print_lut_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid )
{
	const T_IM_IIP_PARAM_LUT* lut = (T_IM_IIP_PARAM_LUT*)pUNITINFTBL->UNITINFTBL2.word;
	static const CHAR* sgX_name_tbl[2] = {
		"D_IM_IIP_UNSIGNED_DATA",
		"D_IM_IIP_SIGNED_DATA",
	};

	if( lut == NULL ) {
		return;
	}

	Ddim_Print(( "LUTTOPCNF[0].DATACONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)lut->LUTTOPCNF[0].bit.DATACONF) ));
	Ddim_Print(( "LUTTOPCNF[0].WAITCONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)lut->LUTTOPCNF[0].bit.WAITCONF) ));
	Ddim_Print(( "LUTUNITCTL.PORTEN = %u\n", lut->LUTUNITCTL.bit.PORTEN ));
	Ddim_Print(( "LUTCTL_A.LUTBIT = %u\n", lut->LUTCTL_A.bit.LUTBIT ));
	Ddim_Print(( "LUTCTL_A.USE = %u\n", lut->LUTCTL_A.bit.USE ));
	Ddim_Print(( "LUTCTL_A.SGN = %s\n", sgX_name_tbl[lut->LUTCTL_A.bit.SGN] ));
	Ddim_Print(( "LUTCTL_A.SPLBIT = %u\n", lut->LUTCTL_A.bit.SPLBIT ));
	Ddim_Print(( "LUTPRECAL_A.OFSET = %u\n", lut->LUTPRECAL_A.bit.OFSET ));
	Ddim_Print(( "LUTPRECAL_A.CLPMAX = %u\n", lut->LUTPRECAL_A.bit.CLPMAX ));
	Ddim_Print(( "LUTPRECAL_A.THMAX = %u\n", lut->LUTPRECAL_A.bit.THMAX ));
	Ddim_Print(( "LUTPRECAL_A.CLPMIN = %u\n", lut->LUTPRECAL_A.bit.CLPMIN ));
	Ddim_Print(( "LUTPRECAL_A.THMIN = %u\n", lut->LUTPRECAL_A.bit.THMIN ));
	Ddim_Print(( "LUTPRECAL_A.ABS = %u\n", lut->LUTPRECAL_A.bit.ABS ));
	Ddim_Print(( "LUTCTL_B.LUTBIT = %u\n", lut->LUTCTL_B.bit.LUTBIT ));
	Ddim_Print(( "LUTCTL_B.USE = %u\n", lut->LUTCTL_B.bit.USE ));
	Ddim_Print(( "LUTCTL_B.SGN = %s\n", sgX_name_tbl[lut->LUTCTL_B.bit.SGN] ));
	Ddim_Print(( "LUTCTL_B.SPLBIT = %u\n", lut->LUTCTL_B.bit.SPLBIT ));
	Ddim_Print(( "LUTPRECAL_B.OFSET = %u\n", lut->LUTPRECAL_B.bit.OFSET ));
	Ddim_Print(( "LUTPRECAL_B.CLPMAX = %u\n", lut->LUTPRECAL_B.bit.CLPMAX ));
	Ddim_Print(( "LUTPRECAL_B.THMAX = %u\n", lut->LUTPRECAL_B.bit.THMAX ));
	Ddim_Print(( "LUTPRECAL_B.CLPMIN = %u\n", lut->LUTPRECAL_B.bit.CLPMIN ));
	Ddim_Print(( "LUTPRECAL_B.THMIN = %u\n", lut->LUTPRECAL_B.bit.THMIN ));
	Ddim_Print(( "LUTPRECAL_B.ABS = %u\n", lut->LUTPRECAL_B.bit.ABS ));
	Ddim_Print(( "LUTCTL_C.LUTBIT = %u\n", lut->LUTCTL_C.bit.LUTBIT ));
	Ddim_Print(( "LUTCTL_C.USE = %u\n", lut->LUTCTL_C.bit.USE ));
	Ddim_Print(( "LUTCTL_C.SGN = %s\n", sgX_name_tbl[lut->LUTCTL_C.bit.SGN] ));
	Ddim_Print(( "LUTCTL_C.SPLBIT = %u\n", lut->LUTCTL_C.bit.SPLBIT ));
	Ddim_Print(( "LUTPRECAL_C.OFSET = %u\n", lut->LUTPRECAL_C.bit.OFSET ));
	Ddim_Print(( "LUTPRECAL_C.CLPMAX = %u\n", lut->LUTPRECAL_C.bit.CLPMAX ));
	Ddim_Print(( "LUTPRECAL_C.THMAX = %u\n", lut->LUTPRECAL_C.bit.THMAX ));
	Ddim_Print(( "LUTPRECAL_C.CLPMIN = %u\n", lut->LUTPRECAL_C.bit.CLPMIN ));
	Ddim_Print(( "LUTPRECAL_C.THMIN = %u\n", lut->LUTPRECAL_C.bit.THMIN ));
	Ddim_Print(( "LUTPRECAL_C.ABS = %u\n", lut->LUTPRECAL_C.bit.ABS ));
	Ddim_Print(( "LUTCTL_D.LUTBIT = %u\n", lut->LUTCTL_D.bit.LUTBIT ));
	Ddim_Print(( "LUTCTL_D.USE = %u\n", lut->LUTCTL_D.bit.USE ));
	Ddim_Print(( "LUTCTL_D.SGN = %s\n", sgX_name_tbl[lut->LUTCTL_D.bit.SGN] ));
	Ddim_Print(( "LUTCTL_D.SPLBIT = %u\n", lut->LUTCTL_D.bit.SPLBIT ));
	Ddim_Print(( "LUTPRECAL_D.OFSET = %u\n", lut->LUTPRECAL_D.bit.OFSET ));
	Ddim_Print(( "LUTPRECAL_D.CLPMAX = %u\n", lut->LUTPRECAL_D.bit.CLPMAX ));
	Ddim_Print(( "LUTPRECAL_D.THMAX = %u\n", lut->LUTPRECAL_D.bit.THMAX ));
	Ddim_Print(( "LUTPRECAL_D.CLPMIN = %u\n", lut->LUTPRECAL_D.bit.CLPMIN ));
	Ddim_Print(( "LUTPRECAL_D.THMIN = %u\n", lut->LUTPRECAL_D.bit.THMIN ));
	Ddim_Print(( "LUTPRECAL_D.ABS = %u\n", lut->LUTPRECAL_D.bit.ABS ));

	Ddim_Print(( "LUTRAM A:\n" ));
	ct_im_iip_print_lutram( E_IM_IIP_LUTRAM_TYPE_A );
	Ddim_Print(( "LUTRAM B:\n" ));
	ct_im_iip_print_lutram( E_IM_IIP_LUTRAM_TYPE_B );
	Ddim_Print(( "LUTRAM C:\n" ));
	ct_im_iip_print_lutram( E_IM_IIP_LUTRAM_TYPE_C );
	Ddim_Print(( "LUTRAM D:\n" ));
	ct_im_iip_print_lutram( E_IM_IIP_LUTRAM_TYPE_D );
	Ddim_Print(( "LUTRAM E:\n" ));
	ct_im_iip_print_lutram( E_IM_IIP_LUTRAM_TYPE_E );
	Ddim_Print(( "LUTRAM F:\n" ));
	ct_im_iip_print_lutram( E_IM_IIP_LUTRAM_TYPE_F );
}

static VOID ct_im_iip_print_blend_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid )
{
	const T_IM_IIP_PARAM_BLEND* alp = (T_IM_IIP_PARAM_BLEND*)pUNITINFTBL->UNITINFTBL2.word;
	static const CHAR* const alpha_out_name_tbl[4] = {
		"E_IM_IIP_PARAM_ALPHA_OUT_P0_ALPHA",
		"E_IM_IIP_PARAM_ALPHA_OUT_P1_ALPHA",
		"E_IM_IIP_PARAM_ALPHA_OUT_FIXED_VAL",
		"E_IM_IIP_PARAM_ALPHA_OUT_NONE",
	};
	static const CHAR* const alpha_step_name_tbl[4] = {
		"E_IM_IIP_PARAM_ALPHA_STEP_8BITS",
		"(null)",
		"(null)",
		"E_IM_IIP_PARAM_ALPHA_STEP_4BITS",
	};
	static const CHAR* const alpha_sel_name_tbl[8] = {
		"E_IM_IIP_PARAM_ALPHA_SEL_P0_ALPHA",
		"E_IM_IIP_PARAM_ALPHA_SEL_P1_ALPHA",
		"E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL",
		"E_IM_IIP_PARAM_ALPHA_SEL_P2_D0_D1_D2_ALPHA",
		"E_IM_IIP_PARAM_ALPHA_SEL_P2_D0_ALPHA",
		"(null)",
		"(null)",
		"(null)",
	};

	if( alp == NULL ) {
		return;
	}

	Ddim_Print(( "BLENDTOPCNF0.DATACONF_0 = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)alp->BLENDTOPCNF0.bit.DATACONF_0) ));
	Ddim_Print(( "BLENDTOPCNF1.DATACONF_1 = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)alp->BLENDTOPCNF1.bit.DATACONF_1) ));
	Ddim_Print(( "BLENDTOPCNF0.WAITCONF_0 = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)alp->BLENDTOPCNF0.bit.WAITCONF_0) ));
	Ddim_Print(( "BLENDCTL.ALPHA_OUT = %s\n", alpha_out_name_tbl[alp->BLENDCTL.bit.ALPHA_OUT] ));
	Ddim_Print(( "ALPHACTL0.ALPHA_STEP = %s\n", alpha_step_name_tbl[alp->ALPHACTL0.bit.ALPHA_STEP] ));
	Ddim_Print(( "ALPHACTL0.ALPHA_SEL = 0x%s\n", alpha_sel_name_tbl[alp->ALPHACTL0.bit.ALPHA_SEL] ));
	Ddim_Print(( "ALPHACTL1.ALPHA_VAL0 = 0x%x\n", alp->ALPHACTL1.bit.ALPHA_VAL0 ));
	Ddim_Print(( "ALPHACTL1.ALPHA_VAL1 = 0x%x\n", alp->ALPHACTL1.bit.ALPHA_VAL1 ));
	Ddim_Print(( "ALPHACTL2.ALPHA_VAL2 = 0x%x\n", alp->ALPHACTL2.bit.ALPHA_VAL2 ));
	Ddim_Print(( "ALPHACTL2.ALPHA_VAL3 = 0x%x\n", alp->ALPHACTL2.bit.ALPHA_VAL3 ));
}

static VOID ct_im_iip_print_mft_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid )
{
	const T_IM_IIP_PARAM_MFT* mft = (T_IM_IIP_PARAM_MFT*)pUNITINFTBL->UNITINFTBL2.word;
	static const CHAR* const mftmd_name_tbl[2] = {
		"E_IM_IIP_PARAM_MFTMD_MIN",
		"E_IM_IIP_PARAM_MFTMD_MAX",
	};
	static const CHAR* const fix_name_tbl[2] = {
		"E_IM_IIP_PARAM_DNFIX_FILTER",
		"E_IM_IIP_PARAM_DNFIX_FIXVAL",
	};
	UINT32 loopcnt;

	if( mft == NULL ) {
		return;
	}

	Ddim_Print(( "FTTOPCONF.DATACONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)mft->FTTOPCONF.bit.DATACONF) ));
	Ddim_Print(( "FTTOPCONF.WAITCONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)mft->FTTOPCONF.bit.WAITCONF) ));
	Ddim_Print(( "MFTMD.MFTMD = %s\n", mftmd_name_tbl[mft->MFTMD.bit.MFTMD] ));
	Ddim_Print(( "FLTTPSZ.FLTTPSZ = %u\n", mft->FLTTPSZ.bit.FLTTPSZ ));
	Ddim_Print(( "FLTOUTHSZ.FLTOUTHSZ = %u\n", mft->FLTOUTHSZ.bit.FLTOUTHSZ ));
	for( loopcnt = 0; loopcnt < 4; loopcnt++ ) {
		Ddim_Print(( "MFTCTLD[%u].FIX = %s\n", loopcnt, fix_name_tbl[mft->MFTCTLD[loopcnt].bit.FIX] ));
		Ddim_Print(( "MFTCTLD[%u].VAL = 0x%x\n", loopcnt, mft->MFTCTLD[loopcnt].bit.VAL ));
	}
}

static VOID ct_im_iip_print_flt_unit_param( volatile struct io_iip_uinftbl* const pUNITINFTBL, const E_IM_IIP_UNIT_ID unitid )
{
	const T_IM_IIP_PARAM_FLT* flt = (T_IM_IIP_PARAM_FLT*)pUNITINFTBL->UNITINFTBL2.word;

	if( flt == NULL ) {
		return;
	}
	
	Im_IIP_On_Pclk();
	Ddim_Print(( "FLT_TDLTOPCNF.DATACONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)flt->TDLTOPCNF.bit.WAITCONF_0) ));
	Ddim_Print(( "FLT_TDLTOPCNF.WAITCONF = %s\n", ct_im_iip_conv_portid_to_name((E_IM_IIP_PARAM_PORTID)flt->TDLTOPCNF.bit.DATACONF) ));
	Ddim_Print(( "PIXIDDEF.IPIXID0 = %u\n", flt->PIXIDDEF.bit.IPIXID0 ));
	Ddim_Print(( "TDL_CTL.FM = %u\n", flt->TDL_CTL.bit.FM ));
	Ddim_Print(( "TDL_CTL.LNUM = %u\n", flt->TDL_CTL.bit.LNUM ));
	Ddim_Print(( "TDL_CTL.SAVERD = %u\n", flt->TDL_CTL.bit.SAVERD ));
	Ddim_Print(( "TDL_CTL.INPUT_MODE = %u\n", flt->TDL_CTL.bit.INPUT_MODE ));
	Ddim_Print(( "TDL_CTL.CSEL = %u\n", flt->TDL_CTL.bit.CSEL ));
	Ddim_Print(( "CALC_MD.CALC_MD = %u\n", flt->CALC_MD.bit.CALC_MD ));

	Ddim_Print(( "TDL_HSIZE.HSTART = %u\n", flt->TDL_HSIZE.bit.HSTART ));
	Ddim_Print(( "TDL_HSIZE.HEND = %u\n", flt->TDL_HSIZE.bit.HEND ));
	Ddim_Print(( "TDL_VSIZE.VSTART = %u\n", flt->TDL_VSIZE.bit.VSTART ));
	Ddim_Print(( "TDL_VSIZE.VEND = %u\n", flt->TDL_VSIZE.bit.VEND ));

	Ddim_Print(( "TDL_PHSZ.PHSZ1 = %u\n", flt->TDL_PHSZ.bit.PHSZ1 ));
	Ddim_Print(( "TDL_PVSZ.PVSZ1 = %u\n", flt->TDL_PVSZ.bit.PVSZ1 ));
	Ddim_Print(( "TDL_PHSZ.PHSZ0 = %u\n", flt->TDL_PHSZ.bit.PHSZ0 ));
	Ddim_Print(( "TDL_PVSZ.PVSZ0 = %u\n", flt->TDL_PVSZ.bit.PVSZ0 ));

	Ddim_Print(( "TDL_PF.PF_ON = %u\n", flt->TDL_PF.bit.PF_ON ));
	Ddim_Print(( "TDL_PF.PF_MD = %u\n", flt->TDL_PF.bit.PF_MD ));
	Ddim_Print(( "TDL_PF.PF_NUM = %u\n", flt->TDL_PF.bit.PF_NUM ));
	Ddim_Print(( "TDL_PF.PF_ADJ_NUM = %u\n", flt->TDL_PF.bit.PF_ADJ_NUM ));
	Ddim_Print(( "TDL_PF.PF_ADJ_VAL = 0x%x\n", flt->TDL_PF.bit.PF_ADJ_VAL ));

	ct_im_iip_get_upsu_name( 0, flt->UPLK_TARGET.bit.UPSU0 );
	ct_im_iip_get_upsu_name( 1, flt->UPLK_TARGET.bit.UPSU1 );
	ct_im_iip_get_upsu_name( 2, flt->UPLK_TARGET.bit.UPSU2 );
	Ddim_Print(( "LKNUM.UPNUM = %u\n", flt->LKNUM.bit.UPNUM ));
	Ddim_Print(( "RING.URINGSIZE = %u\n", flt->RING.bit.URINGSIZE ));
	Ddim_Print(( "RING.UMARGINIYSZ = %u\n", flt->RING.bit.UMARGINIYSZ ));
	ct_im_iip_get_dwsu_name( 0, flt->DWLK_TARGET.bit.DWSU0 );
	ct_im_iip_get_dwsu_name( 1, flt->DWLK_TARGET.bit.DWSU1 );
	ct_im_iip_get_dwsu_name( 2, flt->DWLK_TARGET.bit.DWSU2 );
	Ddim_Print(( "LKNUM.DWNUM = %u\n", flt->LKNUM.bit.DWNUM ));

	Ddim_Print(( "UNIT_OUT.UNIT_OUT_A_D0 = %u\n", flt->UNIT_OUT.bit.UNIT_OUT_A_D0 ));
	Ddim_Print(( "UNIT_OUT.UNIT_OUT_A_D1 = %u\n", flt->UNIT_OUT.bit.UNIT_OUT_A_D1 ));
	Ddim_Print(( "UNIT_OUT.UNIT_OUT_A_D2 = %u\n", flt->UNIT_OUT.bit.UNIT_OUT_A_D2 ));
	Ddim_Print(( "UNIT_OUT.UNIT_OUT_B_D0 = %u\n", flt->UNIT_OUT.bit.UNIT_OUT_B_D0 ));
	Ddim_Print(( "UNIT_OUT.UNIT_OUT_B_D1 = %u\n", flt->UNIT_OUT.bit.UNIT_OUT_B_D1 ));
	Ddim_Print(( "UNIT_OUT.UNIT_OUT_B_D2 = %u\n", flt->UNIT_OUT.bit.UNIT_OUT_B_D2 ));

	Ddim_Print(( "FIN_CLP_A_D0.FIN_CLVLP_A_D0 = 0x%x\n", flt->FIN_CLP_A_D0.bit.FIN_CLVLP_A_D0 ));
	Ddim_Print(( "FIN_CLP_A_D0.FIN_CLVLM_A_D0 = 0x%x\n", flt->FIN_CLP_A_D0.bit.FIN_CLVLM_A_D0 ));
	Ddim_Print(( "FIN_CLP_A_D1.FIN_CLVLP_A_D1 = 0x%x\n", flt->FIN_CLP_A_D1.bit.FIN_CLVLP_A_D1 ));
	Ddim_Print(( "FIN_CLP_A_D1.FIN_CLVLM_A_D1 = 0x%x\n", flt->FIN_CLP_A_D1.bit.FIN_CLVLM_A_D1 ));
	Ddim_Print(( "FIN_CLP_A_D2.FIN_CLVLP_A_D2 = 0x%x\n", flt->FIN_CLP_A_D2.bit.FIN_CLVLP_A_D2 ));
	Ddim_Print(( "FIN_CLP_A_D2.FIN_CLVLM_A_D2 = 0x%x\n", flt->FIN_CLP_A_D2.bit.FIN_CLVLM_A_D2 ));
	Ddim_Print(( "FIN_CLP_B_D0.FIN_CLVLP_B_D0 = 0x%x\n", flt->FIN_CLP_B_D0.bit.FIN_CLVLP_B_D0 ));
	Ddim_Print(( "FIN_CLP_B_D0.FIN_CLVLM_B_D0 = 0x%x\n", flt->FIN_CLP_B_D0.bit.FIN_CLVLM_B_D0 ));
	Ddim_Print(( "FIN_CLP_B_D1.FIN_CLVLP_B_D1 = 0x%x\n", flt->FIN_CLP_B_D1.bit.FIN_CLVLP_B_D1 ));
	Ddim_Print(( "FIN_CLP_B_D1.FIN_CLVLM_B_D1 = 0x%x\n", flt->FIN_CLP_B_D1.bit.FIN_CLVLM_B_D1 ));
	Ddim_Print(( "FIN_CLP_B_D2.FIN_CLVLP_B_D2 = 0x%x\n", flt->FIN_CLP_B_D2.bit.FIN_CLVLP_B_D2 ));
	Ddim_Print(( "FIN_CLP_B_D2.FIN_CLVLM_B_D2 = 0x%x\n", flt->FIN_CLP_B_D2.bit.FIN_CLVLM_B_D2 ));

	Ddim_Print(( "ISO_MD.ISO_JM = 0x%x\n", flt->ISO_MD.bit.ISO_JM ));
	Ddim_Print(( "ISO_MD.ISO_CM = 0x%x\n", flt->ISO_MD.bit.ISO_CM ));
	Ddim_Print(( "ISO_TH_D[0].ISO_UTH_D = 0x%x\n", flt->ISO_TH_D[0].bit.ISO_UTH_D ));
	Ddim_Print(( "ISO_TH_D[0].ISO_LTH_D = 0x%x\n", flt->ISO_TH_D[0].bit.ISO_LTH_D ));
	Ddim_Print(( "ISO_TH_D[1].ISO_UTH_D = 0x%x\n", flt->ISO_TH_D[1].bit.ISO_UTH_D ));
	Ddim_Print(( "ISO_TH_D[1].ISO_LTH_D = 0x%x\n", flt->ISO_TH_D[1].bit.ISO_LTH_D ));
	Ddim_Print(( "ISO_TH_D[2].ISO_UTH_D = 0x%x\n", flt->ISO_TH_D[2].bit.ISO_UTH_D ));
	Ddim_Print(( "ISO_TH_D[2].ISO_LTH_D = 0x%x\n", flt->ISO_TH_D[2].bit.ISO_LTH_D ));
	Ddim_Print(( "ISO_TH_D[3].ISO_UTH_D = 0x%x\n", flt->ISO_TH_D[3].bit.ISO_UTH_D ));
	Ddim_Print(( "ISO_TH_D[3].ISO_LTH_D = 0x%x\n", flt->ISO_TH_D[3].bit.ISO_LTH_D ));
	Ddim_Print(( "ISO_GAIN_D[0].ISO_GA_D = 0x%x\n", flt->ISO_GAIN_D[0].bit.ISO_GA_D ));
	Ddim_Print(( "ISO_GAIN_D[0].ISO_GB_D = 0x%x\n", flt->ISO_GAIN_D[0].bit.ISO_GB_D ));
	Ddim_Print(( "ISO_GAIN_D[1].ISO_GA_D = 0x%x\n", flt->ISO_GAIN_D[1].bit.ISO_GA_D ));
	Ddim_Print(( "ISO_GAIN_D[1].ISO_GB_D = 0x%x\n", flt->ISO_GAIN_D[1].bit.ISO_GB_D ));
	Ddim_Print(( "ISO_GAIN_D[2].ISO_GA_D = 0x%x\n", flt->ISO_GAIN_D[2].bit.ISO_GA_D ));
	Ddim_Print(( "ISO_GAIN_D[2].ISO_GB_D = 0x%x\n", flt->ISO_GAIN_D[2].bit.ISO_GB_D ));
	Ddim_Print(( "ISO_GAIN_D[3].ISO_GA_D = 0x%x\n", flt->ISO_GAIN_D[3].bit.ISO_GA_D ));
	Ddim_Print(( "ISO_GAIN_D[3].ISO_GB_D = 0x%x\n", flt->ISO_GAIN_D[3].bit.ISO_GB_D ));

	Ddim_Print(( "SPF_MD.SPF_MD = 0x%x\n", flt->SPF_MD.bit.SPF_MD ));

	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_0  = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_0  ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_1  = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_1  ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_2  = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_2  ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_3  = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_3  ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_4  = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_4  ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_5  = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_5  ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_6  = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_6  ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_7  = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_7  ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_8  = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_8  ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_9  = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_9  ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_10 = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_10 ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_11 = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_11 ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_12 = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_12 ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_13 = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_13 ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_14 = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_14 ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_15 = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_15 ));
	Ddim_Print(( "SPF_FLTW_D0.SPF_FLTW_16 = 0x%x\n", flt->SPF_FLTW_D0.bit.SPF_FLTW_16 ));

	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_0  = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_0  ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_1  = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_1  ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_2  = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_2  ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_3  = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_3  ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_4  = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_4  ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_5  = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_5  ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_6  = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_6  ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_7  = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_7  ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_8  = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_8  ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_9  = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_9  ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_10 = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_10 ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_11 = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_11 ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_12 = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_12 ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_13 = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_13 ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_14 = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_14 ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_15 = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_15 ));
	Ddim_Print(( "SPF_FLTW_D1.SPF_FLTW_16 = 0x%x\n", flt->SPF_FLTW_D1.bit.SPF_FLTW_16 ));

	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_0  = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_0  ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_1  = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_1  ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_2  = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_2  ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_3  = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_3  ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_4  = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_4  ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_5  = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_5  ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_6  = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_6  ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_7  = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_7  ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_8  = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_8  ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_9  = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_9  ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_10 = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_10 ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_11 = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_11 ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_12 = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_12 ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_13 = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_13 ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_14 = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_14 ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_15 = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_15 ));
	Ddim_Print(( "SPF_FLTW_D2.SPF_FLTW_16 = 0x%x\n", flt->SPF_FLTW_D2.bit.SPF_FLTW_16 ));

	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_0  = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_0  ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_1  = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_1  ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_2  = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_2  ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_3  = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_3  ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_4  = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_4  ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_5  = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_5  ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_6  = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_6  ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_7  = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_7  ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_8  = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_8  ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_9  = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_9  ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_10 = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_10 ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_11 = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_11 ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_12 = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_12 ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_13 = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_13 ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_14 = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_14 ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_15 = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_15 ));
	Ddim_Print(( "SPF_FLTW_D3.SPF_FLTW_16 = 0x%x\n", flt->SPF_FLTW_D3.bit.SPF_FLTW_16 ));

	Ddim_Print(( "SPF_VTH_D0.SPF_UTH_D0 = 0x%x\n", flt->SPF_VTH_D0.bit.SPF_UTH_D0 ));
	Ddim_Print(( "SPF_VTH_D0.SPF_LTH_D0 = 0x%x\n", flt->SPF_VTH_D0.bit.SPF_LTH_D0 ));

	Ddim_Print(( "SPF_VTH_D1.SPF_UTH_D1 = 0x%x\n", flt->SPF_VTH_D1.bit.SPF_UTH_D1 ));
	Ddim_Print(( "SPF_VTH_D1.SPF_LTH_D1 = 0x%x\n", flt->SPF_VTH_D1.bit.SPF_LTH_D1 ));

	Ddim_Print(( "SPF_VTH_D2.SPF_UTH_D2 = 0x%x\n", flt->SPF_VTH_D2.bit.SPF_UTH_D2 ));
	Ddim_Print(( "SPF_VTH_D2.SPF_LTH_D2 = 0x%x\n", flt->SPF_VTH_D2.bit.SPF_LTH_D2 ));

	Ddim_Print(( "SPF_CORE_D0.SPF_CTHP_D0 = 0x%x\n", flt->SPF_CORE_D0.bit.SPF_CTHP_D0 ));
	Ddim_Print(( "SPF_CORE_D0.SPF_CTHM_D0 = 0x%x\n", flt->SPF_CORE_D0.bit.SPF_CTHM_D0 ));

	Ddim_Print(( "SPF_CORE_D1.SPF_CTHP_D1 = 0x%x\n", flt->SPF_CORE_D1.bit.SPF_CTHP_D1 ));
	Ddim_Print(( "SPF_CORE_D1.SPF_CTHM_D1 = 0x%x\n", flt->SPF_CORE_D1.bit.SPF_CTHM_D1 ));

	Ddim_Print(( "SPF_CORE_D2.SPF_CTHP_D2 = 0x%x\n", flt->SPF_CORE_D2.bit.SPF_CTHP_D2 ));
	Ddim_Print(( "SPF_CORE_D2.SPF_CTHM_D2 = 0x%x\n", flt->SPF_CORE_D2.bit.SPF_CTHM_D2 ));

	Ddim_Print(( "SPF_CLP_D0.SPF_CLVLP_D0 = 0x%x\n", flt->SPF_CLP_D0.bit.SPF_CLVLP_D0 ));
	Ddim_Print(( "SPF_CLP_D0.SPF_CLVLM_D0 = 0x%x\n", flt->SPF_CLP_D0.bit.SPF_CLVLM_D0 ));

	Ddim_Print(( "SPF_CLP_D1.SPF_CLVLP_D1 = 0x%x\n", flt->SPF_CLP_D1.bit.SPF_CLVLP_D1 ));
	Ddim_Print(( "SPF_CLP_D1.SPF_CLVLM_D1 = 0x%x\n", flt->SPF_CLP_D1.bit.SPF_CLVLM_D1 ));

	Ddim_Print(( "SPF_CLP_D2.SPF_CLVLP_D2 = 0x%x\n", flt->SPF_CLP_D2.bit.SPF_CLVLP_D2 ));
	Ddim_Print(( "SPF_CLP_D2.SPF_CLVLM_D2 = 0x%x\n", flt->SPF_CLP_D2.bit.SPF_CLVLM_D2 ));

	Ddim_Print(( "SPF_EOADD_D0.SPF_EOADD_D0 = 0x%x\n", flt->SPF_EOADD_D0.bit.SPF_EOADD_D0 ));
	Ddim_Print(( "SPF_EOADD_D1.SPF_EOADD_D1 = 0x%x\n", flt->SPF_EOADD_D1.bit.SPF_EOADD_D1 ));
	Ddim_Print(( "SPF_EOADD_D2.SPF_EOADD_D2 = 0x%x\n", flt->SPF_EOADD_D2.bit.SPF_EOADD_D2 ));

	Ddim_Print(( "EP_MD.EP_MD = 0x%x\n", flt->EP_MD.bit.EP_MD ));
	Ddim_Print(( "EP_GA_D[0].EP_GA_0 = 0x%x\n", flt->EP_GA_D[0].bit.EP_GA_0 ));
	Ddim_Print(( "EP_GA_D[0].EP_GA_1 = 0x%x\n", flt->EP_GA_D[0].bit.EP_GA_1 ));
	Ddim_Print(( "EP_GA_D[0].EP_GA_2 = 0x%x\n", flt->EP_GA_D[0].bit.EP_GA_2 ));
	Ddim_Print(( "EP_GA_D[1].EP_GA_0 = 0x%x\n", flt->EP_GA_D[1].bit.EP_GA_0 ));
	Ddim_Print(( "EP_GA_D[1].EP_GA_1 = 0x%x\n", flt->EP_GA_D[1].bit.EP_GA_1 ));
	Ddim_Print(( "EP_GA_D[1].EP_GA_2 = 0x%x\n", flt->EP_GA_D[1].bit.EP_GA_2 ));
	Ddim_Print(( "EP_GA_D[2].EP_GA_0 = 0x%x\n", flt->EP_GA_D[2].bit.EP_GA_0 ));
	Ddim_Print(( "EP_GA_D[2].EP_GA_1 = 0x%x\n", flt->EP_GA_D[2].bit.EP_GA_1 ));
	Ddim_Print(( "EP_GA_D[2].EP_GA_2 = 0x%x\n", flt->EP_GA_D[2].bit.EP_GA_2 ));
	Ddim_Print(( "EP_GA_D[3].EP_GA_0 = 0x%x\n", flt->EP_GA_D[3].bit.EP_GA_0 ));
	Ddim_Print(( "EP_GA_D[3].EP_GA_1 = 0x%x\n", flt->EP_GA_D[3].bit.EP_GA_1 ));
	Ddim_Print(( "EP_GA_D[3].EP_GA_2 = 0x%x\n", flt->EP_GA_D[3].bit.EP_GA_2 ));
	Ddim_Print(( "EP_OFS_D[0].EP_OFS_0 = 0x%x\n", flt->EP_OFS_D[0].bit.EP_OFS_0 ));
	Ddim_Print(( "EP_OFS_D[0].EP_OFS_1 = 0x%x\n", flt->EP_OFS_D[0].bit.EP_OFS_1 ));
	Ddim_Print(( "EP_OFS_D[0].EP_OFS_2 = 0x%x\n", flt->EP_OFS_D[0].bit.EP_OFS_2 ));
	Ddim_Print(( "EP_OFS_D[1].EP_OFS_0 = 0x%x\n", flt->EP_OFS_D[1].bit.EP_OFS_0 ));
	Ddim_Print(( "EP_OFS_D[1].EP_OFS_1 = 0x%x\n", flt->EP_OFS_D[1].bit.EP_OFS_1 ));
	Ddim_Print(( "EP_OFS_D[1].EP_OFS_2 = 0x%x\n", flt->EP_OFS_D[1].bit.EP_OFS_2 ));
	Ddim_Print(( "EP_OFS_D[2].EP_OFS_0 = 0x%x\n", flt->EP_OFS_D[2].bit.EP_OFS_0 ));
	Ddim_Print(( "EP_OFS_D[2].EP_OFS_1 = 0x%x\n", flt->EP_OFS_D[2].bit.EP_OFS_1 ));
	Ddim_Print(( "EP_OFS_D[2].EP_OFS_2 = 0x%x\n", flt->EP_OFS_D[2].bit.EP_OFS_2 ));
	Ddim_Print(( "EP_OFS_D[3].EP_OFS_0 = 0x%x\n", flt->EP_OFS_D[3].bit.EP_OFS_0 ));
	Ddim_Print(( "EP_OFS_D[3].EP_OFS_1 = 0x%x\n", flt->EP_OFS_D[3].bit.EP_OFS_1 ));
	Ddim_Print(( "EP_OFS_D[3].EP_OFS_2 = 0x%x\n", flt->EP_OFS_D[3].bit.EP_OFS_2 ));
	Ddim_Print(( "EP_BD_D[0].EP_BD_0 = 0x%x\n", flt->EP_BD_D[0].bit.EP_BD_0 ));
	Ddim_Print(( "EP_BD_D[0].EP_BD_1 = 0x%x\n", flt->EP_BD_D[0].bit.EP_BD_1 ));
	Ddim_Print(( "EP_BD_D[0].EP_BD_2 = 0x%x\n", flt->EP_BD_D[0].bit.EP_BD_2 ));
	Ddim_Print(( "EP_BD_D[1].EP_BD_0 = 0x%x\n", flt->EP_BD_D[1].bit.EP_BD_0 ));
	Ddim_Print(( "EP_BD_D[1].EP_BD_1 = 0x%x\n", flt->EP_BD_D[1].bit.EP_BD_1 ));
	Ddim_Print(( "EP_BD_D[1].EP_BD_2 = 0x%x\n", flt->EP_BD_D[1].bit.EP_BD_2 ));
	Ddim_Print(( "EP_BD_D[2].EP_BD_0 = 0x%x\n", flt->EP_BD_D[2].bit.EP_BD_0 ));
	Ddim_Print(( "EP_BD_D[2].EP_BD_1 = 0x%x\n", flt->EP_BD_D[2].bit.EP_BD_1 ));
	Ddim_Print(( "EP_BD_D[2].EP_BD_2 = 0x%x\n", flt->EP_BD_D[2].bit.EP_BD_2 ));
	Ddim_Print(( "EP_BD_D[3].EP_BD_0 = 0x%x\n", flt->EP_BD_D[3].bit.EP_BD_0 ));
	Ddim_Print(( "EP_BD_D[3].EP_BD_1 = 0x%x\n", flt->EP_BD_D[3].bit.EP_BD_1 ));
	Ddim_Print(( "EP_BD_D[3].EP_BD_2 = 0x%x\n", flt->EP_BD_D[3].bit.EP_BD_2 ));

	Ddim_Print(( "ED_FM = 0x%lx\n", flt->ED_FM.word ));
	Ddim_Print(( "ED_OUT = 0x%lx\n", flt->ED_OUT.word ));

	Ddim_Print(( "ADAPT_K.ADAPT_K0 = 0x%x\n", flt->ADAPT_K.bit.ADAPT_K0 ));
	Ddim_Print(( "ADAPT_K.ADAPT_K1 = 0x%x\n", flt->ADAPT_K.bit.ADAPT_K1 ));
	Ddim_Print(( "ADAPT_K.ADAPT_K2 = 0x%x\n", flt->ADAPT_K.bit.ADAPT_K2 ));
	Ddim_Print(( "ADAPT_TH.ADAPT_TH = 0x%x\n", flt->ADAPT_TH.bit.ADAPT_TH ));
	Ddim_Print(( "ADAPT_DIV.ADAPT_DIV = 0x%x\n", flt->ADAPT_DIV.bit.ADAPT_DIV ));

	Ddim_Print(( "BF_MD = 0x%lx\n", flt->BF_MD.word ));

	Im_IIP_Off_Pclk();

}

static VOID ct_im_iip_print_unitcfg( const CHAR* const unit_name, volatile struct io_iip_uinftbl* const pUNITINFTBL, const UCHAR is_sl )
{
	Im_IIP_On_Pclk();

	Ddim_Print(( "%s UNITINFTBL0 = 0x%08lx\n", unit_name, pUNITINFTBL->UNITINFTBL0.word ));
	if( is_sl != 0 ) {
		Ddim_Print(( "%s UNITINFTBL1 = 0x%08lx%08lx\n", unit_name, pUNITINFTBL->UNITINFTBL1.word[0], pUNITINFTBL->UNITINFTBL1.word[1] ));
	}
	Ddim_Print(( "%s UNITINFTBL2 = 0x%08lx\n", unit_name, pUNITINFTBL->UNITINFTBL2.word ));
	Ddim_Print(( "%s UNITSR      = 0x%08lx\n", unit_name, pUNITINFTBL->UNITSR.word ));

	Im_IIP_Off_Pclk();
}

static VOID ct_im_iip_print_unitcfg_detail( const CHAR* const unit_name, volatile struct io_iip_uinftbl* const pUNITINFTBL )
{
	UINT32					loopcnt;
	T_CT_IM_IIP_UNITID_TBL	unitid_tbl;

	Im_IIP_On_Pclk();

	if( pUNITINFTBL->UNITINFTBL1.word != 0 ) {
		Ddim_Print(( "%s UNITINFTBL1.PLDUNIT =", unit_name ));
		for( loopcnt = 0; loopcnt < D_CT_IM_IIP_UNIT_ARRAY_MAX; loopcnt++ ) {
			unitid_tbl = g_ct_im_iip_unitid_array[loopcnt];

			if( (unitid_tbl.pldunit & Im_IIP_Get_Reg_PLDUNIT64( pUNITINFTBL->UNITINFTBL1 )) != 0 ) {
				Ddim_Print(( " %s", unitid_tbl.unitname ));
			}
		}
		Ddim_Print(( "\n" ));
	}

	Im_IIP_Off_Pclk();
}

static VOID ct_im_iip_print_unitcfg_all( VOID )
{
	UINT32					loopcnt;
	T_CT_IM_IIP_UNITID_TBL	unitid;

	for( loopcnt = 0; loopcnt < D_CT_IM_IIP_UNIT_ARRAY_MAX; loopcnt++ ) {
		unitid = g_ct_im_iip_unitid_array[loopcnt];

		ct_im_iip_print_unitcfg( unitid.unitname, unitid.pUNITINFTBL, unitid.is_sl );
	}
}

VOID CT_Im_IIP_Print_Unitcfg_Byid( const E_IM_IIP_UNIT_ID unitid )
{
	UINT32					loopcnt;
	T_CT_IM_IIP_UNITID_TBL	unitid_tbl;

	for( loopcnt = 0; loopcnt < D_CT_IM_IIP_UNIT_ARRAY_MAX; loopcnt++ ) {
		unitid_tbl = g_ct_im_iip_unitid_array[loopcnt];

		if( unitid_tbl.unitid == unitid ) {
			ct_im_iip_print_unitcfg( unitid_tbl.unitname, unitid_tbl.pUNITINFTBL, unitid_tbl.is_sl );
			ct_im_iip_print_unitcfg_detail( unitid_tbl.unitname, unitid_tbl.pUNITINFTBL );
			if( unitid_tbl.unit_param_dump ) {
				unitid_tbl.unit_param_dump( unitid_tbl.pUNITINFTBL, unitid_tbl.unitid );
			}
			break;
		}
	}
}

static VOID ct_im_iip_print_lutram( const E_IM_IIP_LUTRAM_TYPE lutram_type )
{
	UINT32 loopcnt;
	UINT32 lutram_type_idx;

	switch( lutram_type ) {
		case E_IM_IIP_LUTRAM_TYPE_A:
			lutram_type_idx = 0;
			break;
		case E_IM_IIP_LUTRAM_TYPE_B:
			lutram_type_idx = 2;
			break;
		case E_IM_IIP_LUTRAM_TYPE_C:
			lutram_type_idx = 4;
			break;
		case E_IM_IIP_LUTRAM_TYPE_D:
			lutram_type_idx = 1;
			break;
		case E_IM_IIP_LUTRAM_TYPE_E:
			lutram_type_idx = 3;
			break;
		case E_IM_IIP_LUTRAM_TYPE_F:
			lutram_type_idx = 5;
			break;
		default:
			return;
	}

	Im_IIP_On_Pclk();
	Im_IIP_On_Hclk();

	for( loopcnt = 0; loopcnt < 1024; loopcnt++ ) {
		Ddim_Print(( "[%4u]=0x%04lx ", loopcnt, IO_IIP_TBL.LUTRAM.LUTRAM.word[lutram_type_idx][loopcnt] ));
		if( (loopcnt % 8) == 7 ) {
			Ddim_Print(( "\n" ));
		}
	}

	Im_IIP_Off_Hclk();
	Im_IIP_Off_Pclk();
}

static VOID ct_im_iip_print_axictl( const E_IM_IIP_AXI_ID axiid )
{
	ULONG reg0 = 0;
	ULONG reg1 = 0;
	T_CT_IM_IIP_AXI_PRINT_TBL axi_tbl;

	axi_tbl = g_ct_im_iip_axi_print_tbl[axiid];

	if( axi_tbl.reg_addr0 == NULL ) {
		Ddim_Print(( "ct_im_iip_print_axictl: ERROR!!\n" ));
		return;
	}

	Im_IIP_On_Pclk();

	reg0 = *axi_tbl.reg_addr0;
	if( axi_tbl.reg_addr1 != NULL ) {
		reg1 = *axi_tbl.reg_addr1;
	}

	Ddim_Print(( "%s: 0x%lx 0x%lx\n", axi_tbl.axiname, reg0, reg1 ));

	Im_IIP_Off_Pclk();
}

static VOID ct_im_iip_print_hist( volatile ULONG (*hist_addr)[D_IM_IIP_HIST_MAXCOUNTS] )
{
	UINT32 loopcnt;
	UINT32 count;

	Im_IIP_On_Pclk();
	Im_IIP_On_Hclk();

	count = sizeof(*hist_addr) / sizeof((*hist_addr)[0]);
	for( loopcnt = 0; loopcnt < count; loopcnt++ ) {
		Ddim_Print(( "0x%lx ", (*hist_addr)[loopcnt] ));
		if( (loopcnt %16) == 15 ) {
			Ddim_Print(( "\n" ));
		}
	}

	Im_IIP_Off_Hclk();
	Im_IIP_Off_Pclk();

	Ddim_Print(( "\n" ));
}

#if !defined(CO_CT_IM_IIP_2_7_1_DISABLE_HIST) && !defined(CO_CT_IM_IIP_11_1_2_DISABLE_HIST)
VOID CT_Im_IIP_Plot_Hist( const ULONG* buffer_histgram, const UINT32 count, const UCHAR histmax )
{
	CHAR	bar_graph_str[112 +1];
	ULONG	loopcnt;
	ULONG	loopcnt2;

	for( loopcnt = 0; loopcnt < count; loopcnt++ ) {
		if( buffer_histgram[histmax] != 0 ) {
			for( loopcnt2 = 0; loopcnt2 < (112 * buffer_histgram[loopcnt] / buffer_histgram[histmax]); loopcnt2++ ) {
				bar_graph_str[loopcnt2] = '*';
			}
		}
		else {
			loopcnt2 = 0;
		}
		bar_graph_str[loopcnt2] = '\0';
		Ddim_Print(( "[%4lu] 0x%-6lx %s\n", loopcnt, buffer_histgram[loopcnt], bar_graph_str ));
		if( (loopcnt % 10) == 0 ) {
			DDIM_User_Dly_Tsk( 100 );		// Wait for UART output.
		}
	}
}
#endif

#ifdef CO_DEBUG_ON_PC
static VOID ct_im_iip_print_mon( const T_IM_IIP_MON const mon[4] )
{
	UINT32 loopcnt;

	for( loopcnt = 0; loopcnt < 4; loopcnt++ ) {
		Ddim_Print(( "pos_x = 0x%04x pos_y = 0x%04x value = 0x%04x(%d)\n", mon[loopcnt].pos_x, mon[loopcnt].pos_y, mon[loopcnt].value, mon[loopcnt].value ));
	}
}
#endif //CO_DEBUG_ON_PC

/************/
/* API test */
/************/

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_2: "
INT32 CT_Im_IIP_1_1_2( VOID )
{
	INT32 ercd;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

#ifdef CO_DEBUG_ON_PC
	IO_IIP.IZACTL1.bit.TRG = 2;
#endif // CO_DEBUG_ON_PC

	// check: result = 0x0
	ercd = Im_IIP_Init();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

#ifdef CO_DEBUG_ON_PC
	// check: result = 0x0C000100
	IO_IIP.IZACTL1.bit.TRG = 3;
	ercd = Im_IIP_Init();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	IO_IIP.IZACTL1.bit.TRG = 2;
#endif // CO_DEBUG_ON_PC

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_3: "
INT32 CT_Im_IIP_1_1_3( VOID )
{
#ifdef CO_DEBUG_ON_PC
	INT32 ercd;
	const struct {
		volatile struct io_iip_uinftbl* uinftbl;
		E_IM_IIP_UNIT_ID unit_id;
	} ct_param[] = {
		[0] = {
			.unit_id = E_IM_IIP_UNIT_ID_MFT,	// max
			.uinftbl = &IO_IIP.UNITINFTBL_MFT,
		},
		[1] = {
			.unit_id = E_IM_IIP_UNIT_ID_LD0,	// min
			.uinftbl = &IO_IIP.UNITINFTBL_LD0,
		},
	};
	UINT32 loopcnt;
#endif

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

#ifdef CO_DEBUG_ON_PC
	for( loopcnt = 0; loopcnt < (sizeof(ct_param) / sizeof(ct_param[0])); loopcnt++ ) {
		// check: result = 0x0
		ct_param[loopcnt].uinftbl->UNITINFTBL0.bit.SWTRG = 2;
		ercd = Im_IIP_Init_Unit( ct_param[loopcnt].unit_id );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		// check: result = 0x0C000100
		ct_param[loopcnt].uinftbl->UNITINFTBL0.bit.SWTRG = 3;
		ercd = Im_IIP_Init_Unit( ct_param[loopcnt].unit_id );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_param[loopcnt].uinftbl->UNITINFTBL0.bit.SWTRG = 2;
	}
#endif // CO_DEBUG_ON_PC

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_4: "
INT32 CT_Im_IIP_1_1_4( VOID )
{
	INT32 ercd;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	// check: result = 0x0
	ercd = Im_IIP_Open( 100 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check: result = 0x0C000013
	ercd = Im_IIP_Open( 100 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check: result = 0x0
	ercd = Im_IIP_Close();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check: result = 0x0C000099
	ercd = Im_IIP_Close();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check: result = 0x0
	ercd = Im_IIP_Open( 100 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

#ifdef CO_DEBUG_ON_PC
	// check: HWEN = 1
	ct_im_iip_set_hwen_auto_all();
	ct_im_iip_print_hwen_all();
#endif // CO_DEBUG_ON_PC

	// check: result = 0x0
	ercd = Im_IIP_Close();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

#ifdef CO_DEBUG_ON_PC
	// check: HWEN = 0
	ct_im_iip_print_hwen_all();
#endif // CO_DEBUG_ON_PC

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_5: "
INT32 CT_Im_IIP_1_1_5( VOID )
{
	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	CT_Im_IIP_1_1_4();

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_6: "
INT32 CT_Im_IIP_1_1_6( VOID )
{
	T_IM_IIP_PIXFMTTBL pixfmttbl_min = {
		.pix_depth = 0,
		.alpha = 0,
		.frame_type = (E_IM_IIP_FTYPE)0,
		.pix_format = (E_IM_IIP_PFMT)0,
		.alpha_depth = (E_IM_IIP_ADEPTH)0,
		.chunky_color = {
			0, 0, 0, 0,
		},
		.sign_Y_G = 0,
		.sign_Cb_B = 0,
		.sign_Cr_R = 0,
		.sign_D3 = 0,
		.masterIF_Y_G = 0,
		.masterIF_Cb_B = 0,
		.masterIF_Cr_R = 0,
		.masterIF_Alpha = 0,
		.alpha_subsampling = 0,
		.pix_outside_img = 0,
		.Y_G_val_outside_img = 0,
		.Cb_B_val_outside_img = 0,
		.Cr_R_val_outside_img = 0,
		.Alpha_val_outside_img = 0,
		.width = 0,
		.lines = 0,
		.line_bytes = {
			0, 0, 0, 0,
		},
		.addr = {
			0, 0, 0, 0,
		},
	};
	T_IM_IIP_PIXFMTTBL pixfmttbl_max = {
		.pix_depth = 3,
		.alpha = 7,
		.frame_type = (E_IM_IIP_FTYPE)3,
		.pix_format = (E_IM_IIP_PFMT)0xf,
		.alpha_depth = (E_IM_IIP_ADEPTH)3,
		.chunky_color = {
			0x3, 0x3, 0x3, 0x3,
		},
		.sign_Y_G = 1,
		.sign_Cb_B = 1,
		.sign_Cr_R = 1,
		.sign_D3 = 1,
		.masterIF_Y_G = 1,
		.masterIF_Cb_B = 1,
		.masterIF_Cr_R = 1,
		.masterIF_Alpha = 1,
		.alpha_subsampling = 3,
		.pix_outside_img = 1,
		.Y_G_val_outside_img = 0xffff,
		.Cb_B_val_outside_img = 0xffff,
		.Cr_R_val_outside_img = 0xffff,
		.Alpha_val_outside_img = 0xffff,
		.width = 0x7fff,
		.lines = 0x3fff,
		.line_bytes = {
			0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff,
		},
		.addr = {
			0xffffffffUL, 0xffffffffUL, 0xffffffffUL, 0xffffffffUL,
		},
	};
	INT32 ercd;
	UINT32 pixid;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	// check: result = 0x0
	ercd = Im_IIP_Open( 100 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	for( pixid = 0; pixid < D_IM_IIP_PIXID_MAXNUM; pixid++ ) {
		Ddim_Print(( "** %u\n", (UINT32)pixid ));

		// check: Assertion
		// check: result = 0xc000001
		ercd = Im_IIP_Ctrl_PIXFMTTBL( pixid, NULL );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		// check: All valid bit is '1'
		ercd = Im_IIP_Ctrl_PIXFMTTBL( pixid, &pixfmttbl_max );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_pixfmttbl( pixid );

		// check: All valid bit is '0'
		ercd = Im_IIP_Ctrl_PIXFMTTBL( pixid, &pixfmttbl_min );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_pixfmttbl( pixid );
	}

	// check: Assertion
	ercd = Im_IIP_Ctrl_PIXFMTTBL( D_IM_IIP_PIXID_MAXNUM, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check: result = 0x0
	ercd = Im_IIP_Close();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_7: "
INT32 CT_Im_IIP_1_1_7( VOID )
{
	T_IM_IIP_UNIT_CFG unitcfg_min = {
		.unit_ctrl = 0,
		.chain_enable = 0,
		.unit_param_addr = 0,
		.load_unit_param_flag = 0,
	};
	T_IM_IIP_UNIT_CFG unitcfg_max = {
		.unit_ctrl = 1,
		.chain_enable = 1,
		.unit_param_addr = 0xffffffffUL,
		.load_unit_param_flag = 0xffffffffffffffffULL,
	};
	T_IM_IIP_UNITINFTBL unitinftbl_min = {
		.pLD[0] = &unitcfg_min,
		.pLD[1] = &unitcfg_min,
		.pLD[2] = &unitcfg_min,
		.pLD[3] = &unitcfg_min,
		.pLD[4] = &unitcfg_min,
		.pLD[5] = &unitcfg_min,
		.pLD[6] = &unitcfg_min,
		.pLD[7] = &unitcfg_min,
		.pFLT = &unitcfg_min,
		.pAFN[0] = &unitcfg_min,
		.pAFN[1] = &unitcfg_min,
		.pAFN[2] = &unitcfg_min,
		.pAFN[3] = &unitcfg_min,
		.pFRECT[0] = &unitcfg_min,
		.pFRECT[1] = &unitcfg_min,
		.pMON = &unitcfg_min,
		.pSL[0] = &unitcfg_min,
		.pSL[1] = &unitcfg_min,
		.pSL[2] = &unitcfg_min,
		.pSL[3] = &unitcfg_min,
		.pSL[4] = &unitcfg_min,
		.pSL[5] = &unitcfg_min,
		.pSL[6] = &unitcfg_min,
		.pSL[7] = &unitcfg_min,
		.pSL[8] = &unitcfg_min,
		.pCSC[0] = &unitcfg_min,
		.pCSC[1] = &unitcfg_min,
		.pCSC[2] = &unitcfg_min,
		.pCSC[3] = &unitcfg_min,
		.pLUT = &unitcfg_min,
		.pGPC = &unitcfg_min,
		.pBLEND[0] = &unitcfg_min,
		.pBLEND[1] = &unitcfg_min,
		.pCFL = &unitcfg_min,
		.pMFT = &unitcfg_min,
	};
	T_IM_IIP_UNITINFTBL unitinftbl_max = {
		.pLD[0] = &unitcfg_max,
		.pLD[1] = &unitcfg_max,
		.pLD[2] = &unitcfg_max,
		.pLD[3] = &unitcfg_max,
		.pLD[4] = &unitcfg_max,
		.pLD[5] = &unitcfg_max,
		.pLD[6] = &unitcfg_max,
		.pLD[7] = &unitcfg_max,
		.pFLT = &unitcfg_max,
		.pAFN[0] = &unitcfg_max,
		.pAFN[1] = &unitcfg_max,
		.pAFN[2] = &unitcfg_max,
		.pAFN[3] = &unitcfg_max,
		.pFRECT[0] = &unitcfg_max,
		.pFRECT[1] = &unitcfg_max,
		.pMON = &unitcfg_max,
		.pSL[0] = &unitcfg_max,
		.pSL[1] = &unitcfg_max,
		.pSL[2] = &unitcfg_max,
		.pSL[3] = &unitcfg_max,
		.pSL[4] = &unitcfg_max,
		.pSL[5] = &unitcfg_max,
		.pSL[6] = &unitcfg_max,
		.pSL[7] = &unitcfg_max,
		.pSL[8] = &unitcfg_max,
		.pCSC[0] = &unitcfg_max,
		.pCSC[1] = &unitcfg_max,
		.pCSC[2] = &unitcfg_max,
		.pCSC[3] = &unitcfg_max,
		.pLUT = &unitcfg_max,
		.pGPC = &unitcfg_max,
		.pBLEND[0] = &unitcfg_max,
		.pBLEND[1] = &unitcfg_max,
		.pCFL = &unitcfg_max,
		.pMFT = &unitcfg_max,
	};
	T_IM_IIP_UNITINFTBL unitinftbl_null = {
		.pLD[0] = NULL,
		.pLD[1] = NULL,
		.pLD[2] = NULL,
		.pLD[3] = NULL,
		.pLD[4] = NULL,
		.pLD[5] = NULL,
		.pLD[6] = NULL,
		.pLD[7] = NULL,
		.pFLT = NULL,
		.pAFN[0] = NULL,
		.pAFN[1] = NULL,
		.pAFN[2] = NULL,
		.pAFN[3] = NULL,
		.pFRECT[0] = NULL,
		.pFRECT[1] = NULL,
		.pMON = NULL,
		.pSL[0] = NULL,
		.pSL[1] = NULL,
		.pSL[2] = NULL,
		.pSL[3] = NULL,
		.pSL[4] = NULL,
		.pSL[5] = NULL,
		.pSL[6] = NULL,
		.pSL[7] = NULL,
		.pSL[8] = NULL,
		.pCSC[0] = NULL,
		.pCSC[1] = NULL,
		.pCSC[2] = NULL,
		.pCSC[3] = NULL,
		.pLUT = NULL,
		.pGPC = NULL,
		.pBLEND[1] = NULL,
		.pBLEND[0] = NULL,
		.pCFL = NULL,
		.pMFT = NULL,
	};

	INT32 ercd;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	// check: result = 0x0
	ercd = Im_IIP_Open( 100 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check: Assertion
	// check: result = 0xc000001
	ercd = Im_IIP_Ctrl_UNITINFTBL( NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check: result = 0x0
	// check: All valid bit is '1'
	ercd = Im_IIP_Ctrl_UNITINFTBL( &unitinftbl_max );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	ct_im_iip_print_unitcfg_all();

	// check: result = 0x0
	// check: HWEN = 0
	ercd = Im_IIP_Ctrl_UNITINFTBL( &unitinftbl_null );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	ct_im_iip_print_unitcfg_all();

	// check: result = 0x0
	// check: All valid bit is '0'
	ercd = Im_IIP_Ctrl_UNITINFTBL( &unitinftbl_min );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	ct_im_iip_print_unitcfg_all();

	// check: result = 0x0
	ercd = Im_IIP_Close();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_8: "
INT32 CT_Im_IIP_1_1_8( VOID )
{
	USHORT lut_tbl[1024];
	T_IM_IIP_LUT ctrl_lut = {
		.buffer_bytes = 1024 * sizeof(USHORT),
		.buffer_addr = lut_tbl,
		.lutram_type = E_IM_IIP_LUTRAM_TYPE_A,
	};
	UINT32 loopcnt;
	E_IM_IIP_LUTRAM_TYPE lutram_type;
	INT32 ercd;
	E_IM_IIP_OPEN_RES accen_param[E_IM_IIP_LUTRAM_TYPE_MAX] = {
		E_IM_IIP_OPEN_RES_LUT_A,
		E_IM_IIP_OPEN_RES_LUT_B,
		E_IM_IIP_OPEN_RES_LUT_C,
		E_IM_IIP_OPEN_RES_LUT_D,
		E_IM_IIP_OPEN_RES_LUT_E,
		E_IM_IIP_OPEN_RES_LUT_F,
	};

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	// check: result = 0x0
	ercd = Im_IIP_Open( 100 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check: Assertion
	ercd = Im_IIP_Ctrl_LUT( NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	for( lutram_type = E_IM_IIP_LUTRAM_TYPE_A; lutram_type < E_IM_IIP_LUTRAM_TYPE_MAX; lutram_type++ ) {
		ctrl_lut.lutram_type = lutram_type;

		Ddim_Print(( D_IM_IIP_FUNC_NAME "** LUTRAM %u\n", lutram_type ));

		// check result = 0x0
		ercd = Im_IIP_Set_AccessEnable( accen_param[lutram_type], 1 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		// check: All valid bit is '1'
		for( loopcnt = 0; loopcnt < 1024; loopcnt++ ) {
			lut_tbl[loopcnt] = 0x3fff;
		}
		ercd = Im_IIP_Ctrl_LUT( &ctrl_lut );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_lutram( lutram_type );

		// check: All valid bit is '0'
		for( loopcnt = 0; loopcnt < 1024; loopcnt++ ) {
			lut_tbl[loopcnt] = 0;
		}
		ercd = Im_IIP_Ctrl_LUT( &ctrl_lut );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_lutram( lutram_type );

		// check: Countup 0~1023
		for( loopcnt = 0; loopcnt < 1024; loopcnt++ ) {
			lut_tbl[loopcnt] = loopcnt;
		}
		ercd = Im_IIP_Ctrl_LUT( &ctrl_lut );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_lutram( lutram_type );

		// check: Access denied (result = 0xc000100)
#ifdef CO_DEBUG_ON_PC
		switch( lutram_type ) {
			case E_IM_IIP_LUTRAM_TYPE_A:
				IO_IIP.AHBSTS.bit.LUTAST = 1;
				break;
			case E_IM_IIP_LUTRAM_TYPE_B:
				IO_IIP.AHBSTS.bit.LUTBST = 1;
				break;
			case E_IM_IIP_LUTRAM_TYPE_C:
				IO_IIP.AHBSTS.bit.LUTCST = 1;
				break;
			case E_IM_IIP_LUTRAM_TYPE_D:
				IO_IIP.AHBSTS.bit.LUTDST = 1;
				break;
			case E_IM_IIP_LUTRAM_TYPE_E:
				IO_IIP.AHBSTS.bit.LUTEST = 1;
				break;
			case E_IM_IIP_LUTRAM_TYPE_F:
				IO_IIP.AHBSTS.bit.LUTFST = 1;
				break;
			default:
				break;
		}
		for( loopcnt = 0; loopcnt < 1024; loopcnt++ ) {
			lut_tbl[loopcnt] = 0x10;
		}
		ercd = Im_IIP_Ctrl_LUT( &ctrl_lut );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "denied test result = 0x%x\n", ercd ));
		ct_im_iip_print_lutram( lutram_type );
		switch( lutram_type ) {
			case E_IM_IIP_LUTRAM_TYPE_A:
				IO_IIP.AHBSTS.bit.LUTAST = 0;
				break;
			case E_IM_IIP_LUTRAM_TYPE_B:
				IO_IIP.AHBSTS.bit.LUTBST = 0;
				break;
			case E_IM_IIP_LUTRAM_TYPE_C:
				IO_IIP.AHBSTS.bit.LUTCST = 0;
				break;
			case E_IM_IIP_LUTRAM_TYPE_D:
				IO_IIP.AHBSTS.bit.LUTDST = 0;
				break;
			case E_IM_IIP_LUTRAM_TYPE_E:
				IO_IIP.AHBSTS.bit.LUTEST = 0;
				break;
			case E_IM_IIP_LUTRAM_TYPE_F:
				IO_IIP.AHBSTS.bit.LUTFST = 0;
				break;
			default:
				break;
		}
#endif // CO_DEBUG_ON_PC

		// check result = 0x0
		ercd = Im_IIP_Set_AccessEnable( accen_param[lutram_type], 0 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	}

	// check: result = 0x0
	ercd = Im_IIP_Close();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_9: "
INT32 CT_Im_IIP_1_1_9( VOID )
{
	UINT32 loopcnt;
	INT32 ercd;
	ULLONG all_units_mask = 0
		| D_IM_IIP_PARAM_PLDUNIT_LD0
		| D_IM_IIP_PARAM_PLDUNIT_LD1
		| D_IM_IIP_PARAM_PLDUNIT_LD2
		| D_IM_IIP_PARAM_PLDUNIT_LD3
		| D_IM_IIP_PARAM_PLDUNIT_FLT
		| D_IM_IIP_PARAM_PLDUNIT_AFN0
		| D_IM_IIP_PARAM_PLDUNIT_AFN1
		| D_IM_IIP_PARAM_PLDUNIT_AFN2
		| D_IM_IIP_PARAM_PLDUNIT_FRECT0
		| D_IM_IIP_PARAM_PLDUNIT_MON
		| D_IM_IIP_PARAM_PLDUNIT_SL0
		| D_IM_IIP_PARAM_PLDUNIT_SL1
		| D_IM_IIP_PARAM_PLDUNIT_SL2
		| D_IM_IIP_PARAM_PLDUNIT_SL3
		| D_IM_IIP_PARAM_PLDUNIT_SL4
		| D_IM_IIP_PARAM_PLDUNIT_SL5
		| D_IM_IIP_PARAM_PLDUNIT_CSC0
		| D_IM_IIP_PARAM_PLDUNIT_CSC1
		| D_IM_IIP_PARAM_PLDUNIT_LUT
		| D_IM_IIP_PARAM_PLDUNIT_GPC
		| D_IM_IIP_PARAM_PLDUNIT_BLEND0
		| D_IM_IIP_PARAM_PLDUNIT_BLEND1
		| D_IM_IIP_PARAM_PLDUNIT_FRECT1
		| D_IM_IIP_PARAM_PLDUNIT_CFL0
		| D_IM_IIP_PARAM_PLDUNIT_CSC2
		| D_IM_IIP_PARAM_PLDUNIT_CSC3
		| D_IM_IIP_PARAM_PLDUNIT_SL6
		| D_IM_IIP_PARAM_PLDUNIT_SL7
		| D_IM_IIP_PARAM_PLDUNIT_SL8
		| D_IM_IIP_PARAM_PLDUNIT_LD4
		| D_IM_IIP_PARAM_PLDUNIT_LD5
		| D_IM_IIP_PARAM_PLDUNIT_LD6
		| D_IM_IIP_PARAM_PLDUNIT_LD7
		| D_IM_IIP_PARAM_PLDUNIT_AFN3
		| D_IM_IIP_PARAM_PLDUNIT_MFT
		;
	UINT32 all_pixid_mask = 0
		| E_IM_IIP_PIXID_0
		| E_IM_IIP_PIXID_1
		| E_IM_IIP_PIXID_2
		| E_IM_IIP_PIXID_3
		| E_IM_IIP_PIXID_4
		| E_IM_IIP_PIXID_5
		| E_IM_IIP_PIXID_6
		| E_IM_IIP_PIXID_7
		| E_IM_IIP_PIXID_8
		| E_IM_IIP_PIXID_9
		| E_IM_IIP_PIXID_10
		| E_IM_IIP_PIXID_11
		;
	UINT32 all_openres_mask = 0
		| E_IM_IIP_OPEN_RES_LUT_A
		| E_IM_IIP_OPEN_RES_LUT_B
		| E_IM_IIP_OPEN_RES_LUT_C
		| E_IM_IIP_OPEN_RES_LUT_D
		| E_IM_IIP_OPEN_RES_LUT_E
		| E_IM_IIP_OPEN_RES_LUT_F
		| E_IM_IIP_OPEN_RES_HIST_0
		| E_IM_IIP_OPEN_RES_HIST_1
		| E_IM_IIP_OPEN_RES_HIST_2
		| E_IM_IIP_OPEN_RES_HIST_3
		| E_IM_IIP_OPEN_RES_CACHE0
		| E_IM_IIP_OPEN_RES_CACHE1
		| E_IM_IIP_OPEN_RES_CACHE2
		| E_IM_IIP_OPEN_RES_BICUBIC0
		| E_IM_IIP_OPEN_RES_BICUBIC1
		;
	ULONG all_int_factor = 0
		| D_IM_IIP_INT_FACTOR_IZAEND
		| D_IM_IIP_INT_FACTOR_AXIERR
		| D_IM_IIP_INT_FACTOR_SL0END
		| D_IM_IIP_INT_FACTOR_SL1END
		| D_IM_IIP_INT_FACTOR_SL2END
		| D_IM_IIP_INT_FACTOR_SL3END
		| D_IM_IIP_INT_FACTOR_SL4END
		| D_IM_IIP_INT_FACTOR_SL5END
		| D_IM_IIP_INT_FACTOR_SL6END
		| D_IM_IIP_INT_FACTOR_SL7END
		| D_IM_IIP_INT_FACTOR_SL8END
		| D_IM_IIP_INT_FACTOR_SL0_LINEEND
		| D_IM_IIP_INT_FACTOR_SL1_LINEEND
		| D_IM_IIP_INT_FACTOR_SL2_LINEEND
		| D_IM_IIP_INT_FACTOR_SL3_LINEEND
		| D_IM_IIP_INT_FACTOR_SL4_LINEEND
		| D_IM_IIP_INT_FACTOR_SL5_LINEEND
		| D_IM_IIP_INT_FACTOR_SL6_LINEEND
		| D_IM_IIP_INT_FACTOR_SL7_LINEEND
		| D_IM_IIP_INT_FACTOR_SL8_LINEEND
#if 0	// INTIZ2/INTEN2 delete
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN0
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN1
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN2
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN3
		| D_IM_IIP_INT_FACTOR_RINGFULL_FRECT0
		| D_IM_IIP_INT_FACTOR_RINGFULL_FRECT1
		| D_IM_IIP_INT_FACTOR_RINGFULL_FLT
#endif	// INTIZ2/INTEN2 delete
		;
	ULLONG units_bitmask_array[] = {
		D_IM_IIP_PARAM_PLDUNIT_LD0,
		D_IM_IIP_PARAM_PLDUNIT_LD1,
		D_IM_IIP_PARAM_PLDUNIT_LD2,
		D_IM_IIP_PARAM_PLDUNIT_LD3,
		D_IM_IIP_PARAM_PLDUNIT_FLT,
		D_IM_IIP_PARAM_PLDUNIT_AFN0,
		D_IM_IIP_PARAM_PLDUNIT_AFN1,
		D_IM_IIP_PARAM_PLDUNIT_AFN2,
		D_IM_IIP_PARAM_PLDUNIT_FRECT0,
		D_IM_IIP_PARAM_PLDUNIT_MON,
		D_IM_IIP_PARAM_PLDUNIT_SL0,
		D_IM_IIP_PARAM_PLDUNIT_SL1,
		D_IM_IIP_PARAM_PLDUNIT_SL2,
		D_IM_IIP_PARAM_PLDUNIT_SL3,
		D_IM_IIP_PARAM_PLDUNIT_SL4,
		D_IM_IIP_PARAM_PLDUNIT_SL5,
		D_IM_IIP_PARAM_PLDUNIT_CSC0,
		D_IM_IIP_PARAM_PLDUNIT_CSC1,
		D_IM_IIP_PARAM_PLDUNIT_LUT,
		D_IM_IIP_PARAM_PLDUNIT_GPC,
		D_IM_IIP_PARAM_PLDUNIT_BLEND0,
		D_IM_IIP_PARAM_PLDUNIT_BLEND1,
		D_IM_IIP_PARAM_PLDUNIT_FRECT1,
		D_IM_IIP_PARAM_PLDUNIT_CFL0,
		D_IM_IIP_PARAM_PLDUNIT_CSC2,
		D_IM_IIP_PARAM_PLDUNIT_CSC3,
		D_IM_IIP_PARAM_PLDUNIT_SL6,
		D_IM_IIP_PARAM_PLDUNIT_SL7,
		D_IM_IIP_PARAM_PLDUNIT_SL8,
		D_IM_IIP_PARAM_PLDUNIT_LD4,
		D_IM_IIP_PARAM_PLDUNIT_LD5,
		D_IM_IIP_PARAM_PLDUNIT_LD6,
		D_IM_IIP_PARAM_PLDUNIT_LD7,
		D_IM_IIP_PARAM_PLDUNIT_AFN3,
		D_IM_IIP_PARAM_PLDUNIT_MFT,
	};
	UINT32 pixid_bitmask_array[] = {
		E_IM_IIP_PIXID_0,
		E_IM_IIP_PIXID_1,
		E_IM_IIP_PIXID_2,
		E_IM_IIP_PIXID_3,
		E_IM_IIP_PIXID_4,
		E_IM_IIP_PIXID_5,
		E_IM_IIP_PIXID_6,
		E_IM_IIP_PIXID_7,
		E_IM_IIP_PIXID_8,
		E_IM_IIP_PIXID_9,
		E_IM_IIP_PIXID_10,
		E_IM_IIP_PIXID_11,
	};
	UINT32 open_res_bitmask_array[] = {
		E_IM_IIP_OPEN_RES_LUT_A,
		E_IM_IIP_OPEN_RES_LUT_B,
		E_IM_IIP_OPEN_RES_LUT_C,
		E_IM_IIP_OPEN_RES_LUT_D,
		E_IM_IIP_OPEN_RES_LUT_E,
		E_IM_IIP_OPEN_RES_LUT_F,
		E_IM_IIP_OPEN_RES_HIST_0,
		E_IM_IIP_OPEN_RES_HIST_1,
		E_IM_IIP_OPEN_RES_HIST_2,
		E_IM_IIP_OPEN_RES_HIST_3,
		E_IM_IIP_OPEN_RES_CACHE0,
		E_IM_IIP_OPEN_RES_CACHE1,
		E_IM_IIP_OPEN_RES_CACHE2,
		E_IM_IIP_OPEN_RES_BICUBIC0,
		E_IM_IIP_OPEN_RES_BICUBIC1,
	};
	ULLONG unit_bitmask;
	UINT32 pixid_bitmask;
	UINT32 open_res_bitmask;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

#if 1
	// check all open.
	Ddim_Print(( D_IM_IIP_FUNC_NAME "** All device open:\n" ));
	ercd = Im_IIP_Open_SWTRG( all_units_mask, all_pixid_mask, all_openres_mask, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	Im_IIP_Print_DebugStatus();
#endif

#if 1
	Ddim_Print(( D_IM_IIP_FUNC_NAME "** Indivisual device open error:\n" ));
	// check unit open error.
	for( loopcnt = 0; loopcnt < (sizeof(units_bitmask_array) / sizeof(units_bitmask_array[0])); loopcnt++ ) {
		ercd = Im_IIP_Open_SWTRG( units_bitmask_array[loopcnt], 0, 0, 200 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx result = 0x%x\n", units_bitmask_array[loopcnt], ercd ));
		if( ercd == D_IM_IIP_OK ) {
			ercd = Im_IIP_Close_SWTRG( units_bitmask_array[loopcnt], 0, 0 );
			Ddim_Print(( D_IM_IIP_FUNC_NAME "close unitmask=0x%llx result = 0x%x\n", units_bitmask_array[loopcnt], ercd ));
		}
	}
	// check pixfmttbl open error.
	for( loopcnt = 0; loopcnt < (sizeof(pixid_bitmask_array) / sizeof(pixid_bitmask_array[0])); loopcnt++ ) {
		ercd = Im_IIP_Open_SWTRG( 0, pixid_bitmask_array[loopcnt], 0, 200 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "open pixidmask=0x%x result = 0x%x\n", pixid_bitmask_array[loopcnt], ercd ));
		if( ercd == D_IM_IIP_OK ) {
			ercd = Im_IIP_Close_SWTRG( 0, pixid_bitmask_array[loopcnt], 0 );
			Ddim_Print(( D_IM_IIP_FUNC_NAME "close pixidmask=0x%x result = 0x%x\n", pixid_bitmask_array[loopcnt], ercd ));
		}
	}
	// check res open error.
	for( loopcnt = 0; loopcnt < (sizeof(open_res_bitmask_array) / sizeof(open_res_bitmask_array[0])); loopcnt++ ) {
		ercd = Im_IIP_Open_SWTRG( 0, 0, open_res_bitmask_array[loopcnt], 200 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "open resmask=0x%x result = 0x%x\n", open_res_bitmask_array[loopcnt], ercd ));
		// check: E_IM_IIP_OPEN_RES_CACHE0 and E_IM_IIP_OPEN_RES_CACHE1 successfull always.
		if( ercd == D_IM_IIP_OK ) {
			ercd = Im_IIP_Close_SWTRG( 0, 0, open_res_bitmask_array[loopcnt] );
			Ddim_Print(( D_IM_IIP_FUNC_NAME "close resmask=0x%x result = 0x%x\n", open_res_bitmask_array[loopcnt], ercd ));
		}
	}
	Im_IIP_Print_DebugStatus();
#endif

#if 1
	Ddim_Print(( D_IM_IIP_FUNC_NAME "** Indiviaual device close & open:\n" ));
	// check close successful and open successful.
	for( loopcnt = 0; loopcnt < (sizeof(units_bitmask_array) / sizeof(units_bitmask_array[0])); loopcnt++ ) {
		ercd = Im_IIP_Close_SWTRG( units_bitmask_array[loopcnt], 0, 0 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "close unitmask=0x%llx result = 0x%x\n", units_bitmask_array[loopcnt], ercd ));
		ercd = Im_IIP_Open_SWTRG( units_bitmask_array[loopcnt], 0, 0, 200 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx result = 0x%x\n", units_bitmask_array[loopcnt], ercd ));
	}
	// check close successful and open successful.
	for( loopcnt = 0; loopcnt < (sizeof(pixid_bitmask_array) / sizeof(pixid_bitmask_array[0])); loopcnt++ ) {
		ercd = Im_IIP_Close_SWTRG( 0, pixid_bitmask_array[loopcnt], 0 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "close pixidmask=0x%x result = 0x%x\n", pixid_bitmask_array[loopcnt], ercd ));
		ercd = Im_IIP_Open_SWTRG( 0, pixid_bitmask_array[loopcnt], 0, 200 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "open pixidmask=0x%x result = 0x%x\n", pixid_bitmask_array[loopcnt], ercd ));
	}
	// check close successful and open successful.
	for( loopcnt = 0; loopcnt < (sizeof(open_res_bitmask_array) / sizeof(open_res_bitmask_array[0])); loopcnt++ ) {
		ercd = Im_IIP_Close_SWTRG( 0, 0, open_res_bitmask_array[loopcnt] );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "close resmask=0x%x result = 0x%x\n", open_res_bitmask_array[loopcnt], ercd ));
		ercd = Im_IIP_Open_SWTRG( 0, 0, open_res_bitmask_array[loopcnt], 200 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "open resmask=0x%x result = 0x%x\n", open_res_bitmask_array[loopcnt], ercd ));
	}
	Im_IIP_Print_DebugStatus();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "** All device close:\n" ));
	// check all close.
	ercd = Im_IIP_Close_SWTRG( all_units_mask, all_pixid_mask, all_openres_mask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	Im_IIP_Print_DebugStatus();
#endif

#if 1
	// check Unit competition open.
	Ddim_Print(( D_IM_IIP_FUNC_NAME "** Unit competition open:\n" ));
	// user 'A' open - successful.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD0;
	pixid_bitmask = 0;
	open_res_bitmask = 0;
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask, 200 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));
	// user 'B' open - error.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD0 | D_IM_IIP_PARAM_PLDUNIT_SL4;
	pixid_bitmask = E_IM_IIP_PIXID_2 | E_IM_IIP_PIXID_4;
	open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1;
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask, 200 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));
	// user 'A' close - successful.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD0;
	pixid_bitmask = 0;
	open_res_bitmask = 0;
	ercd = Im_IIP_Close_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "close unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));
	// user 'B' open - successful.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD0 | D_IM_IIP_PARAM_PLDUNIT_SL4;
	pixid_bitmask = E_IM_IIP_PIXID_2 | E_IM_IIP_PIXID_4;
	open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1;
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask, 200 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));



	// check Pixid competition open.
	Ddim_Print(( D_IM_IIP_FUNC_NAME "** Pixid competition open:\n" ));
	// user 'A' open - successful.
	unit_bitmask = 0;
	pixid_bitmask = E_IM_IIP_PIXID_5;
	open_res_bitmask = 0;
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask, 200 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));
	// user 'B' open - error.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_SL5;
	pixid_bitmask = E_IM_IIP_PIXID_3 | E_IM_IIP_PIXID_5;
	open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1;
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask, 200 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));
	// user 'A' close - successful.
	unit_bitmask = 0;
	pixid_bitmask = E_IM_IIP_PIXID_5;
	open_res_bitmask = 0;
	ercd = Im_IIP_Close_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "close unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));
	// user 'B' open - successful.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_SL5;
	pixid_bitmask = E_IM_IIP_PIXID_3 | E_IM_IIP_PIXID_5;
	open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1;
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask, 200 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));



	// check res competition open.
	Ddim_Print(( D_IM_IIP_FUNC_NAME "** Res competition open:\n" ));
	// user 'A' open - successful.
	unit_bitmask = 0;
	pixid_bitmask = 0;
	open_res_bitmask = E_IM_IIP_OPEN_RES_LUT_B;
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask, 500 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));
	// user 'B' open - error.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_SL6;
	pixid_bitmask = E_IM_IIP_PIXID_1 | E_IM_IIP_PIXID_6;
	open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1 | E_IM_IIP_OPEN_RES_LUT_B;
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask, 500 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));
	// user 'A' close - successful.
	unit_bitmask = 0;
	pixid_bitmask = 0;
	open_res_bitmask = E_IM_IIP_OPEN_RES_LUT_B;
	ercd = Im_IIP_Close_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "close unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));
	// user 'B' open - successful.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_SL6;
	pixid_bitmask = E_IM_IIP_PIXID_1 | E_IM_IIP_PIXID_6;
	open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1 | E_IM_IIP_OPEN_RES_LUT_B;
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask, 500 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));



	// close of 'Unit competition open'.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD0 | D_IM_IIP_PARAM_PLDUNIT_SL4;
	pixid_bitmask = E_IM_IIP_PIXID_2 | E_IM_IIP_PIXID_4;
	open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1;
	ercd = Im_IIP_Close_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "close unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));



	// close of 'Pixid competition open'.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_SL5;
	pixid_bitmask = E_IM_IIP_PIXID_3 | E_IM_IIP_PIXID_5;
	open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1;
	ercd = Im_IIP_Close_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "close unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));



	// close of 'res competition open'.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_SL6;
	pixid_bitmask = E_IM_IIP_PIXID_1 | E_IM_IIP_PIXID_6;
	open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1 | E_IM_IIP_OPEN_RES_LUT_B;
	ercd = Im_IIP_Close_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "close unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));
	Im_IIP_Print_DebugStatus();
#endif

#if 1
	// check parameter error.
	unit_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_SL6;
	pixid_bitmask = E_IM_IIP_PIXID_1 | E_IM_IIP_PIXID_6;
	open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1 | E_IM_IIP_OPEN_RES_LUT_B;
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, pixid_bitmask, open_res_bitmask, -3 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx pixidmask=0x%x resmask=0x%x result=0x%x\n", unit_bitmask, pixid_bitmask, open_res_bitmask, ercd ));
	Im_IIP_Print_DebugStatus();
#endif

#if 1
	Ddim_Print(( D_IM_IIP_FUNC_NAME "** check interrupt enable bit unset:\n" ));
	ercd = Im_IIP_Open_SWTRG( all_units_mask, 0, 0, 200 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "open unitmask=0x%llx result = 0x%x\n", all_units_mask, ercd ));

	ercd = Im_IIP_Set_Interrupt( all_int_factor, 1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	ct_im_iip_print_base();

	for( loopcnt = 0; loopcnt < (sizeof(units_bitmask_array) / sizeof(units_bitmask_array[0])); loopcnt++ ) {
		ercd = Im_IIP_Close_SWTRG( units_bitmask_array[loopcnt], 0, 0 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "close unitmask=0x%llx result = 0x%x\n", units_bitmask_array[loopcnt], ercd ));
		ct_im_iip_print_base();
	}

#endif

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_10: "
INT32 CT_Im_IIP_1_1_10( VOID )
{
	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	return CT_Im_IIP_1_1_9();
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_11: "
INT32 CT_Im_IIP_1_1_11( VOID )
{
	T_IM_IIP_UNIT_CFG		unitcfg;
	T_CT_IM_IIP_UNITID_TBL	unitid;
	UINT32					loopcnt;
	INT32					ercd;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	memset( &unitcfg, '\0', sizeof(unitcfg) );

	for( loopcnt = 0; loopcnt < D_CT_IM_IIP_UNIT_ARRAY_MAX; loopcnt++ ) {
		unitid = g_ct_im_iip_unitid_array[loopcnt];

		Ddim_Print(( "** UNIT: %s\n", unitid.unitname ));
		ercd = Im_IIP_Open_SWTRG( unitid.pldunit, 0, 0, 1000 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		// check result=0x0C000001
		ercd = Im_IIP_Ctrl_SWTRG_Unit( unitid.unitid, NULL );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		// check register max value
		unitcfg.unit_ctrl = 1;
		unitcfg.chain_enable = 1;
		unitcfg.unit_param_addr = 0xFFFFFFFFul;
		unitcfg.load_unit_param_flag = 0xFFFFFFFFFFFFFFFFull;
		ercd = Im_IIP_Ctrl_SWTRG_Unit( unitid.unitid, &unitcfg );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_unitcfg( unitid.unitname, unitid.pUNITINFTBL, unitid.is_sl );

		// check register min value
		unitcfg.unit_ctrl = 0;
		unitcfg.chain_enable = 0;
		unitcfg.unit_param_addr = 0;
		unitcfg.load_unit_param_flag = 0;
		ercd = Im_IIP_Ctrl_SWTRG_Unit( unitid.unitid, &unitcfg );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_unitcfg( unitid.unitname, unitid.pUNITINFTBL, unitid.is_sl );

		ercd = Im_IIP_Close_SWTRG( unitid.pldunit, 0, 0 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	}

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_12: "
INT32 CT_Im_IIP_1_1_12( VOID )
{
	T_IM_IIP_CTRL_AXI		axicfg;
	E_IM_IIP_AXI_ID			axiid;
	INT32					ercd;
	ULLONG					unit_bitmask = 0xFFFFFFFFFFFFFFFFull;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	memset( &axicfg, '\0', sizeof(axicfg) );
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		Ddim_Print(( "** AXIID: %u\n", axiid ));

		// check result=0x0C000001
		ercd = Im_IIP_Ctrl_AXI( axiid, NULL );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		// check register max value
		axicfg.trans_req_ctrl = 3;
		axicfg.cache_type = 0xF;
		axicfg.protection_type = 7;
		ercd = Im_IIP_Ctrl_AXI( axiid, &axicfg );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_axictl( axiid );

		// check register min value
		axicfg.trans_req_ctrl = 0;
		axicfg.cache_type = 0;
		axicfg.protection_type = 0;
		ercd = Im_IIP_Ctrl_AXI( axiid, &axicfg );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_axictl( axiid );
	}

	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_13: "
INT32 CT_Im_IIP_1_1_13( VOID )
{
	T_IM_IIP_CTRL_AXI		axicfg_max;
	T_IM_IIP_CTRL_AXI		axicfg_min;
	T_IM_IIP_CTRL_AXI_ALL	axicfgall;
	E_IM_IIP_AXI_ID			axiid;
	INT32					ercd;
	UINT32					unit_bitmask = 0xFFFFFFFF;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	memset( &axicfg_max, '\0', sizeof(axicfg_max) );
	axicfg_max.trans_req_ctrl = 3;
	axicfg_max.cache_type = 0xF;
	axicfg_max.protection_type = 7;

	memset( &axicfg_min, '\0', sizeof(axicfg_min) );
	axicfg_min.trans_req_ctrl = 0;
	axicfg_min.cache_type = 0;
	axicfg_min.protection_type = 0;


	Ddim_Print(( "** imgcache0\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.imgcache0 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.imgcache0 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** imgcache1\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.imgcache1 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.imgcache1 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** imgcache2\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.imgcache2 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.imgcache2 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** dp\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.dp = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.dp = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** afn0\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.afn0 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.afn0 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** afn1\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.afn1 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.afn1 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** afn2\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.afn2 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.afn2 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** afn3\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.afn3 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.afn3 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** frect0\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.frect0 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.frect0 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** frect1\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.frect1 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.frect1 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** gpc\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.gpc = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.gpc = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** sl0\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl0 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl0 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** sl1\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl1 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl1 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** sl2\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl2 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl2 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** sl3\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl3 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl3 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** sl4\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl4 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl4 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** sl5\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl5 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl5 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** sl6\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl6 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl6 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** sl7\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl7 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl7 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	Ddim_Print(( "** sl8\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl8 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.sl8 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}



	Ddim_Print(( "** all\n" ));
	// check register max value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.imgcache0 = &axicfg_max;
	axicfgall.imgcache1 = &axicfg_max;
	axicfgall.imgcache2 = &axicfg_max;
	axicfgall.dp = &axicfg_max;
	axicfgall.afn0 = &axicfg_max;
	axicfgall.afn1 = &axicfg_max;
	axicfgall.afn2 = &axicfg_max;
	axicfgall.afn3 = &axicfg_max;
	axicfgall.frect0 = &axicfg_max;
	axicfgall.frect1 = &axicfg_max;
	axicfgall.gpc = &axicfg_max;
	axicfgall.sl0 = &axicfg_max;
	axicfgall.sl1 = &axicfg_max;
	axicfgall.sl2 = &axicfg_max;
	axicfgall.sl3 = &axicfg_max;
	axicfgall.sl4 = &axicfg_max;
	axicfgall.sl5 = &axicfg_max;
	axicfgall.sl6 = &axicfg_max;
	axicfgall.sl7 = &axicfg_max;
	axicfgall.sl8 = &axicfg_max;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}
	// check register min value
	memset( &axicfgall, '\0', sizeof(axicfgall) );
	axicfgall.imgcache0 = &axicfg_min;
	axicfgall.imgcache1 = &axicfg_min;
	axicfgall.imgcache2 = &axicfg_min;
	axicfgall.dp = &axicfg_min;
	axicfgall.afn0 = &axicfg_min;
	axicfgall.afn1 = &axicfg_min;
	axicfgall.afn2 = &axicfg_min;
	axicfgall.afn3 = &axicfg_min;
	axicfgall.frect0 = &axicfg_min;
	axicfgall.frect1 = &axicfg_min;
	axicfgall.gpc = &axicfg_min;
	axicfgall.sl0 = &axicfg_min;
	axicfgall.sl1 = &axicfg_min;
	axicfgall.sl2 = &axicfg_min;
	axicfgall.sl3 = &axicfg_min;
	axicfgall.sl4 = &axicfg_min;
	axicfgall.sl5 = &axicfg_min;
	axicfgall.sl6 = &axicfg_min;
	axicfgall.sl7 = &axicfg_min;
	axicfgall.sl8 = &axicfg_min;
	ercd = Im_IIP_Ctrl_AXI_ALL( &axicfgall );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	for( axiid = E_IM_IIP_AXI_ID_TOP; axiid < E_IM_IIP_AXI_ID_MAX; axiid++ ) {
		ct_im_iip_print_axictl( axiid );
	}


	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_14: "
INT32 CT_Im_IIP_1_1_14( VOID )
{
	T_IM_IIP_CTRL_BASE		basecfg;
	INT32					ercd;
	UINT32					unit_bitmask = 0xFFFFFFFF;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	memset( &basecfg, '\0', sizeof(basecfg) );
#if 0	// B2Y Link not support
	basecfg.b2y_link = 1;
#endif	// B2Y Link not support
	basecfg.enable_interrupt_flag = 0
		| D_IM_IIP_INT_FACTOR_IZAEND
		| D_IM_IIP_INT_FACTOR_AXIERR
		| D_IM_IIP_INT_FACTOR_SL0END
		| D_IM_IIP_INT_FACTOR_SL1END
		| D_IM_IIP_INT_FACTOR_SL2END
		| D_IM_IIP_INT_FACTOR_SL3END
		| D_IM_IIP_INT_FACTOR_SL4END
		| D_IM_IIP_INT_FACTOR_SL5END
		| D_IM_IIP_INT_FACTOR_SL6END
		| D_IM_IIP_INT_FACTOR_SL7END
		| D_IM_IIP_INT_FACTOR_SL8END
		| D_IM_IIP_INT_FACTOR_SL0_LINEEND
		| D_IM_IIP_INT_FACTOR_SL1_LINEEND
		| D_IM_IIP_INT_FACTOR_SL2_LINEEND
		| D_IM_IIP_INT_FACTOR_SL3_LINEEND
		| D_IM_IIP_INT_FACTOR_SL4_LINEEND
		| D_IM_IIP_INT_FACTOR_SL5_LINEEND
		| D_IM_IIP_INT_FACTOR_SL6_LINEEND
		| D_IM_IIP_INT_FACTOR_SL7_LINEEND
		| D_IM_IIP_INT_FACTOR_SL8_LINEEND
#if 0	// IMG-SRAM not support
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN0
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN1
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN2
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN3
		| D_IM_IIP_INT_FACTOR_RINGFULL_FRECT0
		| D_IM_IIP_INT_FACTOR_RINGFULL_FRECT1
		| D_IM_IIP_INT_FACTOR_RINGFULL_FLT
#endif	// IMG-SRAM not support
		;
	basecfg.sl0_interrupt_lines = 0x3FFF;
	basecfg.sl1_interrupt_lines = 0x3FFF;
	basecfg.sl2_interrupt_lines = 0x3FFF;
	basecfg.sl3_interrupt_lines = 0x3FFF;
	basecfg.sl4_interrupt_lines = 0x3FFF;
	basecfg.sl5_interrupt_lines = 0x3FFF;
	basecfg.sl6_interrupt_lines = 0x3FFF;
	basecfg.sl7_interrupt_lines = 0x3FFF;
	basecfg.sl8_interrupt_lines = 0x3FFF;
	basecfg.int_handler = (T_IM_IIP_CALLBACK)0xFFFFFFFF;
	basecfg.cache_flush_ctrl[0] = 1;
	basecfg.cache_flush_ctrl[1] = 1;
	basecfg.cache_flush_ctrl[2] = 1;
	basecfg.ext_bit_ctrl_less_8bits[0] = 1;
	basecfg.ext_bit_ctrl_less_8bits[1] = 1;
	basecfg.ext_bit_ctrl_less_8bits[2] = 1;


	// check result = 0x0C000001
	ercd = Im_IIP_Ctrl_Base( NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check register max value
	ercd = Im_IIP_Ctrl_Base( &basecfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	ct_im_iip_print_base();

	// check register min value
	memset( &basecfg, '\0', sizeof(basecfg) );
	ercd = Im_IIP_Ctrl_Base( &basecfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	ct_im_iip_print_base();

	Im_IIP_Print_DebugStatus();


	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_15: "
INT32 CT_Im_IIP_1_1_15( VOID )
{
	UINT32	enable_interrupt_flag;
	INT32	ercd;
	UINT32	unit_bitmask = 0xFFFFFFFF;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	enable_interrupt_flag = 0
		| D_IM_IIP_INT_FACTOR_IZAEND
		| D_IM_IIP_INT_FACTOR_AXIERR
		| D_IM_IIP_INT_FACTOR_SL0END
		| D_IM_IIP_INT_FACTOR_SL1END
		| D_IM_IIP_INT_FACTOR_SL2END
		| D_IM_IIP_INT_FACTOR_SL3END
		| D_IM_IIP_INT_FACTOR_SL4END
		| D_IM_IIP_INT_FACTOR_SL5END
		| D_IM_IIP_INT_FACTOR_SL6END
		| D_IM_IIP_INT_FACTOR_SL7END
		| D_IM_IIP_INT_FACTOR_SL8END
		| D_IM_IIP_INT_FACTOR_SL0_LINEEND
		| D_IM_IIP_INT_FACTOR_SL1_LINEEND
		| D_IM_IIP_INT_FACTOR_SL2_LINEEND
		| D_IM_IIP_INT_FACTOR_SL3_LINEEND
		| D_IM_IIP_INT_FACTOR_SL4_LINEEND
		| D_IM_IIP_INT_FACTOR_SL5_LINEEND
		| D_IM_IIP_INT_FACTOR_SL6_LINEEND
		| D_IM_IIP_INT_FACTOR_SL7_LINEEND
		| D_IM_IIP_INT_FACTOR_SL8_LINEEND
#if 0	// INTIZ2/INTEN2 delete
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN0
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN1
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN2
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN3
		| D_IM_IIP_INT_FACTOR_RINGFULL_FRECT0
		| D_IM_IIP_INT_FACTOR_RINGFULL_FRECT1
		| D_IM_IIP_INT_FACTOR_RINGFULL_FLT
#endif	// INTIZ2/INTEN2 delete
		;


	// check register max value
	ercd = Im_IIP_Set_Interrupt( enable_interrupt_flag, 1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	ct_im_iip_print_base();

	// check register min value
	ercd = Im_IIP_Set_Interrupt( enable_interrupt_flag, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	ct_im_iip_print_base();


	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_16: "
INT32 CT_Im_IIP_1_1_16( VOID )
{
	UINT32	enable_interrupt_flag;
	INT32	ercd;
	UINT32	loopcnt;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
	UINT32 int_factor_array[] = {
		( D_IM_IIP_INT_FACTOR_IZAEND
		| D_IM_IIP_INT_FACTOR_AXIERR
		| D_IM_IIP_INT_FACTOR_SL0END
		| D_IM_IIP_INT_FACTOR_SL1END
		| D_IM_IIP_INT_FACTOR_SL2END
		| D_IM_IIP_INT_FACTOR_SL3END
		| D_IM_IIP_INT_FACTOR_SL4END
		| D_IM_IIP_INT_FACTOR_SL5END
		| D_IM_IIP_INT_FACTOR_SL6END
		| D_IM_IIP_INT_FACTOR_SL7END
		| D_IM_IIP_INT_FACTOR_SL8END
		| D_IM_IIP_INT_FACTOR_SL0_LINEEND
		| D_IM_IIP_INT_FACTOR_SL1_LINEEND
		| D_IM_IIP_INT_FACTOR_SL2_LINEEND
		| D_IM_IIP_INT_FACTOR_SL3_LINEEND
		| D_IM_IIP_INT_FACTOR_SL4_LINEEND
		| D_IM_IIP_INT_FACTOR_SL5_LINEEND
		| D_IM_IIP_INT_FACTOR_SL6_LINEEND
		| D_IM_IIP_INT_FACTOR_SL7_LINEEND
		| D_IM_IIP_INT_FACTOR_SL8_LINEEND
#if 0	// INTIZ2/INTEN2 delete
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN0
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN1
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN2
		| D_IM_IIP_INT_FACTOR_RINGFULL_AFN3
		| D_IM_IIP_INT_FACTOR_RINGFULL_FRECT0
		| D_IM_IIP_INT_FACTOR_RINGFULL_FRECT1
		| D_IM_IIP_INT_FACTOR_RINGFULL_FLT
#endif	// INTIZ2/INTEN2 delete
		 ),
		D_IM_IIP_INT_FACTOR_IZAEND,
		D_IM_IIP_INT_FACTOR_AXIERR,
		D_IM_IIP_INT_FACTOR_SL0END,
		D_IM_IIP_INT_FACTOR_SL1END,
		D_IM_IIP_INT_FACTOR_SL2END,
		D_IM_IIP_INT_FACTOR_SL3END,
		D_IM_IIP_INT_FACTOR_SL4END,
		D_IM_IIP_INT_FACTOR_SL5END,
		D_IM_IIP_INT_FACTOR_SL6END,
		D_IM_IIP_INT_FACTOR_SL7END,
		D_IM_IIP_INT_FACTOR_SL8END,
		D_IM_IIP_INT_FACTOR_SL0_LINEEND,
		D_IM_IIP_INT_FACTOR_SL1_LINEEND,
		D_IM_IIP_INT_FACTOR_SL2_LINEEND,
		D_IM_IIP_INT_FACTOR_SL3_LINEEND,
		D_IM_IIP_INT_FACTOR_SL4_LINEEND,
		D_IM_IIP_INT_FACTOR_SL5_LINEEND,
		D_IM_IIP_INT_FACTOR_SL6_LINEEND,
		D_IM_IIP_INT_FACTOR_SL7_LINEEND,
		D_IM_IIP_INT_FACTOR_SL8_LINEEND,
#if 0	// INTIZ2/INTEN2 delete
		D_IM_IIP_INT_FACTOR_RINGFULL_AFN0,
		D_IM_IIP_INT_FACTOR_RINGFULL_AFN1,
		D_IM_IIP_INT_FACTOR_RINGFULL_AFN2,
		D_IM_IIP_INT_FACTOR_RINGFULL_AFN3,
		D_IM_IIP_INT_FACTOR_RINGFULL_FRECT0,
		D_IM_IIP_INT_FACTOR_RINGFULL_FRECT1,
		D_IM_IIP_INT_FACTOR_RINGFULL_FLT,
#endif	// INTIZ2/INTEN2 delete
	};


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	for( loopcnt = 0; loopcnt < (sizeof(int_factor_array) / sizeof(int_factor_array[0])); loopcnt++ ) {
		enable_interrupt_flag = int_factor_array[loopcnt];

		Ddim_Print(( D_IM_IIP_FUNC_NAME "** flag bit = 0x%x\n", enable_interrupt_flag ));

		// check register max value
		ercd = Im_IIP_Set_IntHandler( enable_interrupt_flag, (T_IM_IIP_CALLBACK)0xFFFFFFFF );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_base();
		Im_IIP_Print_DebugStatus();

		// check register min value
		ercd = Im_IIP_Set_IntHandler( enable_interrupt_flag, (T_IM_IIP_CALLBACK)0 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_base();
		Im_IIP_Print_DebugStatus();

		DDIM_User_Dly_Tsk( 300 );		// Wait for UART output.
	}


	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_17: "
INT32 CT_Im_IIP_1_1_17( VOID )
{
	UINT32	enable_interrupt_flag;
	INT32	ercd;
	UINT32	loopcnt;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
	UINT32 int_factor_array[] = {
		( D_IM_IIP_INT_FACTOR_SL0_LINEEND
		| D_IM_IIP_INT_FACTOR_SL1_LINEEND
		| D_IM_IIP_INT_FACTOR_SL2_LINEEND
		| D_IM_IIP_INT_FACTOR_SL3_LINEEND
		| D_IM_IIP_INT_FACTOR_SL4_LINEEND
		| D_IM_IIP_INT_FACTOR_SL5_LINEEND
		| D_IM_IIP_INT_FACTOR_SL6_LINEEND
		| D_IM_IIP_INT_FACTOR_SL7_LINEEND
		| D_IM_IIP_INT_FACTOR_SL8_LINEEND ),
		D_IM_IIP_INT_FACTOR_SL0_LINEEND,
		D_IM_IIP_INT_FACTOR_SL1_LINEEND,
		D_IM_IIP_INT_FACTOR_SL2_LINEEND,
		D_IM_IIP_INT_FACTOR_SL3_LINEEND,
		D_IM_IIP_INT_FACTOR_SL4_LINEEND,
		D_IM_IIP_INT_FACTOR_SL5_LINEEND,
		D_IM_IIP_INT_FACTOR_SL6_LINEEND,
		D_IM_IIP_INT_FACTOR_SL7_LINEEND,
		D_IM_IIP_INT_FACTOR_SL8_LINEEND,
	};


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	for( loopcnt = 0; loopcnt < (sizeof(int_factor_array) / sizeof(int_factor_array[0])); loopcnt++ ) {
		enable_interrupt_flag = int_factor_array[loopcnt];

		Ddim_Print(( D_IM_IIP_FUNC_NAME "** flag bit = 0x%x\n", enable_interrupt_flag ));

		// check register max value
		ercd = Im_IIP_Set_SLIntLines( enable_interrupt_flag, 0x3FFFFFFF );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_base();

		// check register min value
		ercd = Im_IIP_Set_SLIntLines( enable_interrupt_flag, 0 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_base();
	}


	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_18: "
INT32 CT_Im_IIP_1_1_18( VOID )
{
	INT32	ercd;
	UINT32	loopcnt;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
	T_CT_IM_IIP_UNITID_TBL	unitid;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	for( loopcnt = 0; loopcnt < D_CT_IM_IIP_UNIT_ARRAY_MAX; loopcnt++ ) {
		unitid = g_ct_im_iip_unitid_array[loopcnt];

		Ddim_Print(( "** UNIT: %s\n", unitid.unitname ));

		// check register max value
		ercd = Im_IIP_Update_PADRS( unitid.unitid, 0xFFFFFFFF );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_unitcfg( unitid.unitname, unitid.pUNITINFTBL, unitid.is_sl );

		// check register min value
		ercd = Im_IIP_Update_PADRS( unitid.unitid, 0 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_unitcfg( unitid.unitname, unitid.pUNITINFTBL, unitid.is_sl );
	}


	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_19: "
INT32 CT_Im_IIP_1_1_19( VOID )
{
	INT32	ercd;
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	CHAR	clk_dump_msg[] = "PCLK=%u ICLK=%u CLK=%u\n";
#endif
	CHAR	clk_dump_msg1[] = "TRG=%u ABORT=%u\n";
	CHAR	clk_dump_msg2[] = "IZARAC=%u CA1RAC=%u CA2RAC=%u\n";

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif

	// check: result = 0x0
	ercd = Im_IIP_Open( 100 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif

	// !!! Need set Unit and Pixid parameter when physical or emnulator using.


	Ddim_Print(( "** Start test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Start();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 3
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 1
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 0 0
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif


	Ddim_Print(( "** Re-Start test\n" ));
	// check: result = 0x0C000100
	ercd = Im_IIP_Start();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 3
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 1
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 0 0
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif



	Ddim_Print(( "** Stop test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Stop( D_IM_IIP_STOP );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 2
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 0
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 1 1
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif


	Ddim_Print(( "** Aobrt test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Start();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check: result = 0x0
	ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 2
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 0
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 1 1
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif


	// check: result = 0x0
	ercd = Im_IIP_Close();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_20: "
INT32 CT_Im_IIP_1_1_20( VOID )
{
	INT32	ercd;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	CHAR	clk_dump_msg[] = "PCLK=%u ICLK=%u CLK=%u\n";
#endif
	CHAR	clk_dump_msg1[] = "TRG=%u ABORT=%u\n";
	CHAR	clk_dump_msg2[] = "IZARAC=%u CA1RAC=%u CA2RAC=%u\n";

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif

	// check: result = 0x0
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif

	// !!! Need set Unit and Pixid parameter when physical or emnulator using.



	Ddim_Print(( "** Start test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_LinkStart_SWTRG( E_IM_IIP_UNIT_ID_SL2, D_IM_IIP_SWTRG_ON, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 3
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 1
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 0 0
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif
	Im_IIP_On_Pclk();
	Ddim_Print(( "UNITINF TRG = %u\n", IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.SWTRG ));
	Im_IIP_Off_Pclk();



	Ddim_Print(( "** Start test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_LinkStart_SWTRG( E_IM_IIP_UNIT_ID_SL3, D_IM_IIP_SWTRG_ON, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 3
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 1
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 0 0
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif
	Im_IIP_On_Pclk();
	Ddim_Print(( "UNITINF TRG = %u\n", IO_IIP.UNITINFTBL_SL3.UNITINFTBL0.bit.SWTRG ));
	Im_IIP_Off_Pclk();


	Ddim_Print(( "** Start test\n" ));
	// check: result = 0x0C000001
	ercd = Im_IIP_LinkStart_SWTRG( E_IM_IIP_UNIT_ID_MAX, D_IM_IIP_SWTRG_ON, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	Ddim_Print(( "** Close test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Close_SWTRG( 0xFFFFull, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 3
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 1
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 0 0
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif



	Ddim_Print(( "** Close test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Close_SWTRG( 0xFFFFFFFFFFFF0000ull, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 2
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 0
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 1 1
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif


	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_21: "
INT32 CT_Im_IIP_1_1_21( VOID )
{
	INT32	ercd;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	CHAR	clk_dump_msg[] = "PCLK=%u ICLK=%u CLK=%u\n";
#endif
	CHAR	clk_dump_msg1[] = "TRG=%u ABORT=%u\n";
	CHAR	clk_dump_msg2[] = "IZARAC=%u CA1RAC=%u CA2RAC=%u\n";

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif

	// check: result = 0x0
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif

	// !!! Need set Unit and Pixid parameter when physical or emnulator using.



	Ddim_Print(( "** Start test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Start_SWTRG( E_IM_IIP_UNIT_ID_SL2, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 3
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 1
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 0 0
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif
	Im_IIP_On_Pclk();
	Ddim_Print(( "UNITINF TRG = %u\n", IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.SWTRG ));
	Im_IIP_Off_Pclk();



	Ddim_Print(( "** Start test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Start_SWTRG( E_IM_IIP_UNIT_ID_SL3, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 3
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 1
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 0 0
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif
	Im_IIP_On_Pclk();
	Ddim_Print(( "UNITINF TRG = %u\n", IO_IIP.UNITINFTBL_SL3.UNITINFTBL0.bit.SWTRG ));
	Im_IIP_Off_Pclk();


	Ddim_Print(( "** Start test\n" ));
	// check: result = 0x0C000001
	ercd = Im_IIP_Start_SWTRG( E_IM_IIP_UNIT_ID_MAX, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	Ddim_Print(( "** Close test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Close_SWTRG( 0xFFFFull, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 3
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 1
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 0 0
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif



	Ddim_Print(( "** Close test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Close_SWTRG( 0xFFFFFFFFFFFF0000ull, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 2
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 0
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 1 1
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif


	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_22: "
INT32 CT_Im_IIP_1_1_22( VOID )
{
	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	return CT_Im_IIP_1_1_19();
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_23: "
INT32 CT_Im_IIP_1_1_23( VOID )
{
	INT32	ercd;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	CHAR	clk_dump_msg[] = "PCLK=%u ICLK=%u CLK=%u\n";
#endif
	CHAR	clk_dump_msg1[] = "TRG=%u ABORT=%u\n";
	CHAR	clk_dump_msg2[] = "IZARAC=%u CA1RAC=%u CA2RAC=%u\n";
	UINT32	wait_factor_2 = 0;
	UINT32	wait_factor_3 = 0;
	UINT32	wait_factor_6 = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif

	// check: result = 0x0
	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif

	// !!! Need set Unit and Pixid parameter when physical or emnulator using.


	ercd = Im_IIP_Set_Interrupt( D_IM_IIP_INT_FACTOR_SL5END | D_IM_IIP_INT_FACTOR_SL0END | D_IM_IIP_INT_FACTOR_SL8END, 1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	ct_im_iip_print_base();


	Ddim_Print(( "** Start test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Start_SWTRG( E_IM_IIP_UNIT_ID_SL0, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 3
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 1
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 0 0 0
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif
	Im_IIP_On_Pclk();
	Ddim_Print(( "UNITINF TRG = %u\n", IO_IIP.UNITINFTBL_SL0.UNITINFTBL0.bit.SWTRG ));
	Im_IIP_Off_Pclk();



	Ddim_Print(( "** Start test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Start_SWTRG( E_IM_IIP_UNIT_ID_SL5, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 3
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 1
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 0 0 0
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif
	Im_IIP_On_Pclk();
	Ddim_Print(( "UNITINF TRG = %u\n", IO_IIP.UNITINFTBL_SL5.UNITINFTBL0.bit.SWTRG ));
	Im_IIP_Off_Pclk();



#ifdef CO_DEBUG_ON_PC
	IO_IIP.INTIZ1.bit.__SL0FF = 1;
	IO_IIP.INTIZ1.bit.__SL5FF = 1;
	IO_IIP.INTIZ1.bit.__SL0MF = 1;
	Im_IIP_Int_Handler();
#endif // CO_DEBUG_ON_PC

	Ddim_Print(( "** WaitEnd test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Wait_End( &wait_factor_3, D_IM_IIP_INT_FACTOR_SL5END, D_IM_IIP_AND_WAIT, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x wait_factor_3 = 0x%x\n", ercd, wait_factor_3 ));

	Ddim_Print(( "** WaitEnd test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Wait_End( &wait_factor_2, D_IM_IIP_INT_FACTOR_SL0END, D_IM_IIP_AND_WAIT, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x wait_factor_2 = 0x%x\n", ercd, wait_factor_2 ));

	Ddim_Print(( "** WaitEnd test\n" ));
	// check: result = 0x0C000013
	ercd = Im_IIP_Wait_End( &wait_factor_6, D_IM_IIP_INT_FACTOR_SL0END, D_IM_IIP_AND_WAIT, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x wait_factor_6 = 0x%x\n", ercd, wait_factor_6 ));



	Ddim_Print(( "** WaitEnd test\n" ));
	// check: result = 0x0C000013
	ercd = Im_IIP_Wait_End( &wait_factor_2, D_IM_IIP_INT_FACTOR_SL0_LINEEND, D_IM_IIP_AND_WAIT, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x wait_factor_2 = 0x%x\n", ercd, wait_factor_2 ));

	ercd = Im_IIP_Set_Interrupt( D_IM_IIP_INT_FACTOR_SL0_LINEEND, 1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
#ifdef CO_DEBUG_ON_PC
	Im_IIP_Int_Handler();
#endif // CO_DEBUG_ON_PC

	Ddim_Print(( "** WaitEnd test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Wait_End( &wait_factor_2, D_IM_IIP_INT_FACTOR_SL0_LINEEND, D_IM_IIP_AND_WAIT, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x wait_factor_2 = 0x%x\n", ercd, wait_factor_2 ));
	ct_im_iip_print_base();


	Ddim_Print(( "** Close test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Close_SWTRG( D_IM_IIP_PARAM_PLDUNIT_SL0, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	ct_im_iip_print_base();

	Ddim_Print(( "** Close test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Close_SWTRG( D_IM_IIP_PARAM_PLDUNIT_SL5, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	ct_im_iip_print_base();

	Ddim_Print(( "** Close test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Close_SWTRG( 0xFFFFull & ~(D_IM_IIP_PARAM_PLDUNIT_SL0|D_IM_IIP_PARAM_PLDUNIT_SL5), 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 3
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 1
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 0 0
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif
	ct_im_iip_print_base();



	Ddim_Print(( "** Close test\n" ));
	// check: result = 0x0
	ercd = Im_IIP_Close_SWTRG( 0xFFFFFFFFFFFF0000ull & ~(D_IM_IIP_PARAM_PLDUNIT_SL0|D_IM_IIP_PARAM_PLDUNIT_SL5), 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	// check: value is 2
	Ddim_Print(( clk_dump_msg1, IO_IIP.IZACTL1.bit.TRG, IO_IIP.IZACTL2.bit.ABORT ));
	// check: value is 0
	Ddim_Print(( clk_dump_msg2, IO_IIP.ACESEN0.bit.IZARAC, IO_IIP.ACESEN0.bit.CA1RAC, IO_IIP.ACESEN0.bit.CA2RAC ));
	Im_IIP_Off_Pclk();
	// check: value is 1 1 1
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Ddim_Print(( clk_dump_msg, Dd_Top_Get_CLKSTOP3_IIPAP(), Dd_Top_Get_CLKSTOP3_IIPAX(), Dd_Top_Get_CLKSTOP3_IIPCK() ));
#endif
	ct_im_iip_print_base();


	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_24: "
INT32 CT_Im_IIP_1_1_24( VOID )
{
	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	return CT_Im_IIP_1_1_23();
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_25: "
INT32 CT_Im_IIP_1_1_25( VOID )
{
	INT32	ercd;
	UINT32	loopcnt;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
	UINT32	param_tbl_idx;
	UINT32	hist_resol_max;
	struct {
		E_IM_IIP_OPEN_RES	res_bitmask;
		UINT32				hist_type_idx;
		volatile ULONG		(*hist_array)[D_IM_IIP_HIST_MAXCOUNTS];		// Pointer of HISTGRAM array.
		UCHAR				clear_hist_func_param[4];
	} param_tbl[] = {
		[0] = {
			.res_bitmask = E_IM_IIP_OPEN_RES_HIST_0,
			.hist_type_idx = 0,
			.hist_array = &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[0],
			.clear_hist_func_param = { D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF },
		},
		[1] = {
			.res_bitmask = E_IM_IIP_OPEN_RES_HIST_1,
			.hist_type_idx = 1,
			.hist_array = &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[1],
			.clear_hist_func_param = { D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF },
		},
		[2] = {
			.res_bitmask = E_IM_IIP_OPEN_RES_HIST_2,
			.hist_type_idx = 2,
			.hist_array = &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[2],
			.clear_hist_func_param = { D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_OFF },
		},
		[3] = {
			.res_bitmask = E_IM_IIP_OPEN_RES_HIST_3,
			.hist_type_idx = 3,
			.hist_array = &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[3],
			.clear_hist_func_param = { D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_ON },
		},
	};


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	for( param_tbl_idx = 0; param_tbl_idx < (sizeof(param_tbl) / sizeof(param_tbl[0])); param_tbl_idx++ ) {
		Ddim_Print(( "** hist[%u]\n", param_tbl[param_tbl_idx].hist_type_idx ));

		hist_resol_max = sizeof((*param_tbl[param_tbl_idx].hist_array)) / sizeof((*param_tbl[param_tbl_idx].hist_array)[0]);

		ercd = Im_IIP_Set_AccessEnable( param_tbl[param_tbl_idx].res_bitmask, 1 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		Im_IIP_On_Pclk();
		Im_IIP_On_Hclk();
		for( loopcnt = 0; loopcnt < hist_resol_max; loopcnt++ ) {
			(*param_tbl[param_tbl_idx].hist_array)[loopcnt] = 0xFFFFFFul;
		}
		Im_IIP_Off_Hclk();
		Im_IIP_Off_Pclk();

#ifdef CO_DEBUG_ON_PC
		// check result = 0x0C000100
		IO_IIP.AHBSTS.bit.HISSTS = 1;
		ercd = Im_IIP_Clear_HISTGRAM(
				param_tbl[param_tbl_idx].clear_hist_func_param[0],
				param_tbl[param_tbl_idx].clear_hist_func_param[1],
				param_tbl[param_tbl_idx].clear_hist_func_param[2],
				param_tbl[param_tbl_idx].clear_hist_func_param[3]
				);
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		IO_IIP.AHBSTS.bit.HISSTS = 0;
#endif //CO_DEBUG_ON_PC

		// check histgram value = 0xFFFFFF
		ct_im_iip_print_hist( param_tbl[param_tbl_idx].hist_array );

		// check histgram value = 0
		ercd = Im_IIP_Clear_HISTGRAM(
				param_tbl[param_tbl_idx].clear_hist_func_param[0],
				param_tbl[param_tbl_idx].clear_hist_func_param[1],
				param_tbl[param_tbl_idx].clear_hist_func_param[2],
				param_tbl[param_tbl_idx].clear_hist_func_param[3]
				);
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		ct_im_iip_print_hist( param_tbl[param_tbl_idx].hist_array );

		ercd = Im_IIP_Set_AccessEnable( param_tbl[param_tbl_idx].res_bitmask, 0 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	}



	//
	// MONINIT
	//
	Ddim_Print(( "** moninit\n" ));

	ercd = Im_IIP_Set_AccessEnable( E_IM_IIP_OPEN_RES_HIST_0 | E_IM_IIP_OPEN_RES_HIST_1 | E_IM_IIP_OPEN_RES_HIST_2 | E_IM_IIP_OPEN_RES_HIST_3, 1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


#ifdef CO_DEBUG_ON_PC
	// check result = 0x0C000100
	IO_IIP.AHBSTS.bit.HISSTS = 1;
	ercd = Im_IIP_Clear_HISTGRAM( D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	IO_IIP.AHBSTS.bit.HISSTS = 0;

	Ddim_Print(( "MONINIT = 0x%x\n", IO_IIP.MONINIT.bit.INIT ));


	// check clear monitor1 register
	IO_IIP.MONINIT.bit.INIT = 0;
	ercd = Im_IIP_Clear_HISTGRAM( D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	Ddim_Print(( "MONINIT = 0x%x\n", IO_IIP.MONINIT.bit.INIT ));
	IO_IIP.MONINIT.bit.INIT = 0;
#endif //CO_DEBUG_ON_PC

	ercd = Im_IIP_Set_AccessEnable( E_IM_IIP_OPEN_RES_HIST_0 | E_IM_IIP_OPEN_RES_HIST_1 | E_IM_IIP_OPEN_RES_HIST_2 | E_IM_IIP_OPEN_RES_HIST_3, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#ifdef CO_CT_IM_IIP_ENABLE_1_1_26
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_26: "
INT32 CT_Im_IIP_1_1_26( VOID )
{
	INT32	ercd;
	UINT32	loopcnt;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
	static volatile ULONG g_ct_im_iip_hist_buf[D_IM_IIP_HIST_MAXCOUNTS];
	UINT32	param_tbl_idx;
	UINT32	hist_resol_max;
	struct {
		E_IM_IIP_OPEN_RES	res_bitmask;
		UINT32				hist_type_idx;
		volatile ULONG		(*hist_array)[D_IM_IIP_HIST_MAXCOUNTS];		// Pointer of HISTGRAM array.
		UCHAR				clear_hist_func_param[4];
		volatile ULONG*		get_hist_func_param[4];
	} param_tbl[] = {
		[0] = {
			.res_bitmask = E_IM_IIP_OPEN_RES_HIST_0,
			.hist_type_idx = 0,
			.hist_array = &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[0],
			.clear_hist_func_param = { D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF },
			.get_hist_func_param =   { g_ct_im_iip_hist_buf, NULL,              NULL,                NULL },
		},
		[1] = {
			.res_bitmask = E_IM_IIP_OPEN_RES_HIST_1,
			.hist_type_idx = 1,
			.hist_array = &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[1],
			.clear_hist_func_param = { D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF },
			.get_hist_func_param =   { NULL,                g_ct_im_iip_hist_buf, NULL,              NULL },
		},
		[2] = {
			.res_bitmask = E_IM_IIP_OPEN_RES_HIST_2,
			.hist_type_idx = 2,
			.hist_array = &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[2],
			.clear_hist_func_param = { D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_ON, D_IM_IIP_ENABLE_OFF },
			.get_hist_func_param =   { NULL,                NULL,                g_ct_im_iip_hist_buf, NULL },
		},
		[3] = {
			.res_bitmask = E_IM_IIP_OPEN_RES_HIST_3,
			.hist_type_idx = 3,
			.hist_array = &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[3],
			.clear_hist_func_param = { D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_OFF, D_IM_IIP_ENABLE_ON },
			.get_hist_func_param =   { NULL,                NULL,                NULL,                g_ct_im_iip_hist_buf },
		},
	};

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	for( param_tbl_idx = 0; param_tbl_idx < (sizeof(param_tbl) / sizeof(param_tbl[0])); param_tbl_idx++ ) {
		Ddim_Print(( "** hist[%u]\n", param_tbl[param_tbl_idx].hist_type_idx ));

		ercd = Im_IIP_Set_AccessEnable( param_tbl[param_tbl_idx].res_bitmask, 1 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		hist_resol_max = sizeof((*param_tbl[param_tbl_idx].hist_array)) / sizeof((*param_tbl[param_tbl_idx].hist_array)[0]);

		Im_IIP_On_Pclk();
		Im_IIP_On_Hclk();
		for( loopcnt = 0; loopcnt < hist_resol_max; loopcnt++ ) {
			(*param_tbl[param_tbl_idx].hist_array)[loopcnt] = 0xFFFFFFul;
		}
		Im_IIP_Off_Hclk();
		Im_IIP_Off_Pclk();

#ifdef CO_DEBUG_ON_PC
		// check result = 0x0C000100
		IO_IIP.AHBSTS.bit.HISSTS = 1;
		ercd = Im_IIP_Get_HISTGRAM(
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[0],
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[1],
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[2],
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[3]
				);
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		IO_IIP.AHBSTS.bit.HISSTS = 0;
#endif //CO_DEBUG_ON_PC

		// check get histgram = 0xFFFFFF
		ercd = Im_IIP_Get_HISTGRAM(
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[0],
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[1],
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[2],
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[3]
				);
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		ct_im_iip_print_hist( &g_ct_im_iip_hist_buf );

		// check result = 0x0
		ercd = Im_IIP_Clear_HISTGRAM(
				param_tbl[param_tbl_idx].clear_hist_func_param[0],
				param_tbl[param_tbl_idx].clear_hist_func_param[1],
				param_tbl[param_tbl_idx].clear_hist_func_param[2],
				param_tbl[param_tbl_idx].clear_hist_func_param[3]
				);
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		// check get histgram = 0
		ercd = Im_IIP_Get_HISTGRAM(
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[0],
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[1],
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[2],
				(ULONG*)param_tbl[param_tbl_idx].get_hist_func_param[3]
				);
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		ct_im_iip_print_hist( &g_ct_im_iip_hist_buf );

		ercd = Im_IIP_Set_AccessEnable( param_tbl[param_tbl_idx].res_bitmask, 0 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	}



	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}
#endif //CO_CT_IM_IIP_ENABLE_1_1_26

#ifdef CO_DEBUG_ON_PC
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_27: "
INT32 CT_Im_IIP_1_1_27( VOID )
{
	INT32	ercd;
	UCHAR	histmax;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	Ddim_Print(( "** HISTMAX0\n" ));
	IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_0 = 0xFF;

	// check get HISTMAX0
	ercd = Im_IIP_Get_HISTMAX( &histmax, NULL, NULL, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x histmax=0x%x\n", ercd, histmax ));

	IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_0 = 0;

	// check get HISTMAX0
	ercd = Im_IIP_Get_HISTMAX( &histmax, NULL, NULL, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x histmax=0x%x\n", ercd, histmax ));




	Ddim_Print(( "** HISTMAX1\n" ));
	IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_1 = 0xFF;

	// check get HISTMAX1
	ercd = Im_IIP_Get_HISTMAX( NULL, &histmax, NULL, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x histmax=0x%x\n", ercd, histmax ));

	IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_1 = 0;

	// check get HISTMAX1
	ercd = Im_IIP_Get_HISTMAX( NULL, &histmax, NULL, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x histmax=0x%x\n", ercd, histmax ));




	Ddim_Print(( "** HISTMAX2\n" ));
	IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_2 = 0xFF;

	// check get HISTMAX2
	ercd = Im_IIP_Get_HISTMAX( NULL, NULL, &histmax, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x histmax=0x%x\n", ercd, histmax ));

	IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_2 = 0;

	// check get HISTMAX2
	ercd = Im_IIP_Get_HISTMAX( NULL, NULL, &histmax, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x histmax=0x%x\n", ercd, histmax ));




	Ddim_Print(( "** HISTMAX3\n" ));
	IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_3 = 0xFF;

	// check get HISTMAX3
	ercd = Im_IIP_Get_HISTMAX( NULL, NULL, NULL, &histmax );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x histmax=0x%x\n", ercd, histmax ));

	IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_3 = 0;

	// check get HISTMAX3
	ercd = Im_IIP_Get_HISTMAX( NULL, NULL, NULL, &histmax );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x histmax=0x%x\n", ercd, histmax ));




	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}
#endif //CO_DEBUG_ON_PC

#ifdef CO_DEBUG_ON_PC
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_28: "
INT32 CT_Im_IIP_1_1_28( VOID )
{
	INT32	ercd;
	UINT32	loopcnt;
	T_IM_IIP_MAXMON mon;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	for( loopcnt = 0; loopcnt < (sizeof(mon.max) / sizeof(mon.max[0])); loopcnt++ ) {
		IO_IIP.MAXMON[loopcnt].bit.HPOINTMAX = 0x3FFF;
		IO_IIP.MAXMON[loopcnt].bit.VPOINTMAX = 0;
		IO_IIP.MAXMON[loopcnt].bit.MAXVAL = -0x4000;

		// check get MAXMON
		ercd = Im_IIP_Get_MAXMON( &mon );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_mon( mon.max );

		IO_IIP.MAXMON[loopcnt].bit.HPOINTMAX = 0;
		IO_IIP.MAXMON[loopcnt].bit.VPOINTMAX = 0x3FFF;
		IO_IIP.MAXMON[loopcnt].bit.MAXVAL = 0x3FFF;

		// check get MAXMON
		ercd = Im_IIP_Get_MAXMON( &mon );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_mon( mon.max );
	}


	// check result = 0x0C000001
	ercd = Im_IIP_Get_MAXMON( NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));



	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}
#endif //CO_DEBUG_ON_PC

#ifdef CO_DEBUG_ON_PC
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_29: "
INT32 CT_Im_IIP_1_1_29( VOID )
{
	INT32	ercd;
	UINT32	loopcnt;
	T_IM_IIP_MINMON mon;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	for( loopcnt = 0; loopcnt < (sizeof(mon.min) / sizeof(mon.min[0])); loopcnt++ ) {
		IO_IIP.MINMON[loopcnt].bit.HPOINTMIN = 0x3FFF;
		IO_IIP.MINMON[loopcnt].bit.VPOINTMIN = 0;
		IO_IIP.MINMON[loopcnt].bit.MINVAL = -0x4000;

		// check get MINMON
		ercd = Im_IIP_Get_MINMON( &mon );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_mon( mon.min );

		IO_IIP.MINMON[loopcnt].bit.HPOINTMIN = 0;
		IO_IIP.MINMON[loopcnt].bit.VPOINTMIN = 0x3FFF;
		IO_IIP.MINMON[loopcnt].bit.MINVAL = 0x3FFF;

		// check get MINMON
		ercd = Im_IIP_Get_MINMON( &mon );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_mon( mon.min );
	}


	// check result = 0x0C000001
	ercd = Im_IIP_Get_MINMON( NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));



	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}
#endif //CO_DEBUG_ON_PC

#ifdef CO_DEBUG_ON_PC
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_30: "
INT32 CT_Im_IIP_1_1_30( VOID )
{
	INT32	ercd;
	UINT32	loopcnt;
	T_IM_IIP_ADDMON mon;
	ULLONG	unit_bitmask = 0xFFFFFFFFFFFFFFFFull;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	for( loopcnt = 0; loopcnt < (sizeof(mon.add) / sizeof(mon.add[0])); loopcnt++ ) {
		IO_IIP.ADDMON[loopcnt].bit.HPOINTADD = 0x3FFF;
		IO_IIP.ADDMON[loopcnt].bit.VPOINTADD = 0;
		IO_IIP.ADDMON[loopcnt].bit.ADDVAL = -0x4000;

		// check get ADDMON
		ercd = Im_IIP_Get_ADDMON( &mon );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_mon( mon.add );

		IO_IIP.ADDMON[loopcnt].bit.HPOINTADD = 0;
		IO_IIP.ADDMON[loopcnt].bit.VPOINTADD = 0x3FFF;
		IO_IIP.ADDMON[loopcnt].bit.ADDVAL = 0x3FFF;

		// check get ADDMON
		ercd = Im_IIP_Get_ADDMON( &mon );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		ct_im_iip_print_mon( mon.add );
	}

	// check result = 0x0C000001
	ercd = Im_IIP_Get_ADDMON( NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));



	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}
#endif //CO_DEBUG_ON_PC

#ifdef CO_DEBUG_ON_PC
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_31: "
INT32 CT_Im_IIP_1_1_31( VOID )
{
	INT32					ercd;
	E_IM_IIP_CACHE_CH		loopcnt;
	T_IM_IIP_ACCESS_INFO	info;
	ULLONG					unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
	static const CHAR		dump_msg[] = "acc=0x%08lx miss=0x%08lx\n";

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	for( loopcnt = E_IM_IIP_CACHE_CH_0; loopcnt < E_IM_IIP_CACHE_CH_MAX; loopcnt++ ) {
		IO_IIP.PDCRAC[loopcnt].bit.PDCRAC = 0xFFFFFFFF;
		IO_IIP.PDCRMC[loopcnt].bit.PDCRMC = 0;

		ercd = Im_IIP_Get_ImgCache_Access_Info( loopcnt, &info );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		Ddim_Print(( dump_msg, info.real_access_counter, info.real_miss_counter ));


		IO_IIP.PDCRAC[loopcnt].bit.PDCRAC = 0;
		IO_IIP.PDCRMC[loopcnt].bit.PDCRMC = 0xFFFFFFFF;

		ercd = Im_IIP_Get_ImgCache_Access_Info( loopcnt, &info );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		Ddim_Print(( dump_msg, info.real_access_counter, info.real_miss_counter ));


		IO_IIP.PDCRAC[loopcnt].bit.PDCRAC = 0;
		IO_IIP.PDCRMC[loopcnt].bit.PDCRMC = 0;

		ercd = Im_IIP_Get_ImgCache_Access_Info( loopcnt, &info );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		Ddim_Print(( dump_msg, info.real_access_counter, info.real_miss_counter ));
	}


	// check result = 0x0C000001
	ercd = Im_IIP_Get_ImgCache_Access_Info( E_IM_IIP_CACHE_CH_MAX, &info );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check result = 0x0C000001
	ercd = Im_IIP_Get_ImgCache_Access_Info( E_IM_IIP_CACHE_CH_0, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}
#endif //CO_DEBUG_ON_PC

#ifdef CO_DEBUG_ON_PC
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_32: "
INT32 CT_Im_IIP_1_1_32( VOID )
{
	INT32					ercd;
	T_IM_IIP_AXI_STATUS		sts;
	ULLONG					unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
	static const CHAR		dump_msg[] = "read=0x%08lx write=0x%08lx\n";

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	IO_IIP.AXRSTS.word = 0xFFFFFFFFul;
	IO_IIP.AXWSTS.word = 0;

	ercd = Im_IIP_Get_AXI_Status( &sts );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	Ddim_Print(( dump_msg, sts.read_channel_status, sts.write_channel_status ));

	IO_IIP.AXRSTS.word = 0;
	IO_IIP.AXWSTS.word = 0xFFFFFFFFul;

	ercd = Im_IIP_Get_AXI_Status( &sts );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	Ddim_Print(( dump_msg, sts.read_channel_status, sts.write_channel_status ));

	ercd = Im_IIP_Get_AXI_Status( NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}
#endif //CO_DEBUG_ON_PC

#ifdef CO_DEBUG_ON_PC
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_33: "
INT32 CT_Im_IIP_1_1_33( VOID )
{
	INT32					ercd;
	UINT32					sl_status;
	ULLONG					unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
	static const CHAR		dump_msg[] = "sl_stat=0x%u\n";

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	IO_IIP.IZACTL1.bit.SLALE = 1;

	ercd = Im_IIP_Get_SL_Status( &sl_status );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	Ddim_Print(( dump_msg, sl_status ));

	IO_IIP.IZACTL1.bit.SLALE = 0;

	ercd = Im_IIP_Get_SL_Status( &sl_status );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	Ddim_Print(( dump_msg, sl_status ));

	ercd = Im_IIP_Get_SL_Status( NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}
#endif //CO_DEBUG_ON_PC

#ifdef CO_CT_IM_IIP_ENABLE_1_1_34
#ifdef CO_DEBUG_ON_PC
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_34: "
INT32 CT_Im_IIP_1_1_34( VOID )
{
	INT32					ercd;
	volatile ULONG*			p_iip_debug;
	UINT32					out_bytes;
	UINT32					loopcnt;
	UINT32					loopcnt2;
	T_CT_IM_IIP_UNITID_TBL	unitid;
	ULLONG					unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
	static volatile ULONG g_ct_im_iip_get_unitparam_buf[sizeof(IO_IIP.UNITDEBUG) /4];


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	p_iip_debug = IO_IIP.UNITDEBUG;

	for( loopcnt = 0; loopcnt < (65536 / sizeof(ULONG)); loopcnt++ ) {
		p_iip_debug[loopcnt] = ((65536 / sizeof(ULONG)) -1 - loopcnt)
							 | ((65536 / sizeof(ULONG)) -1 - loopcnt) * 0x10000;
	}

	for( loopcnt = 0; loopcnt < D_CT_IM_IIP_UNIT_ARRAY_MAX; loopcnt++ ) {
		unitid = g_ct_im_iip_unitid_array[loopcnt];

		Ddim_Print(( "** UNIT: %s\n", unitid.unitname ));

		memset( (VOID*)g_ct_im_iip_get_unitparam_buf, '\0', sizeof(g_ct_im_iip_get_unitparam_buf) );
		ercd = Im_IIP_Get_UNIT_PARAM( unitid.unitid, (ULONG*)g_ct_im_iip_get_unitparam_buf, &out_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x bytes=%u(0x%x)\n", ercd, out_bytes, out_bytes ));

		for( loopcnt2 = 0; loopcnt2 < (out_bytes / sizeof(ULONG) +1); loopcnt2++ ) {
			Ddim_Print(( "0x%08lx ", g_ct_im_iip_get_unitparam_buf[loopcnt2] ));
			if( (loopcnt2 % 16) == 15 ) {
				Ddim_Print(( "\n" ));
			}
		}

		Ddim_Print(( "\n" ));
	}


	// check result = 0x0C000001
	ercd = Im_IIP_Get_UNIT_PARAM( E_IM_IIP_UNIT_ID_LD0, NULL, &out_bytes );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	// check result = 0x0C000001
	ercd = Im_IIP_Get_UNIT_PARAM( E_IM_IIP_UNIT_ID_LD0, (ULONG*)g_ct_im_iip_get_unitparam_buf, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}
#endif //CO_DEBUG_ON_PC
#endif // CO_CT_IM_IIP_ENABLE_1_1_34

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_35: "
INT32 CT_Im_IIP_1_1_35( VOID )
{
	INT32					ercd;
	UINT32					loopcnt;
	ULLONG					unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
	static const CHAR		dump_msg[] = "accen0=0x%08lx\n";
	UINT32					accen_bitmask[] = {
		  E_IM_IIP_OPEN_RES_LUT_A
		| E_IM_IIP_OPEN_RES_LUT_B
		| E_IM_IIP_OPEN_RES_LUT_C
		| E_IM_IIP_OPEN_RES_LUT_D
		| E_IM_IIP_OPEN_RES_LUT_E
		| E_IM_IIP_OPEN_RES_LUT_F
		| E_IM_IIP_OPEN_RES_HIST_0
		| E_IM_IIP_OPEN_RES_HIST_1
		| E_IM_IIP_OPEN_RES_HIST_2
		| E_IM_IIP_OPEN_RES_HIST_3
		| E_IM_IIP_OPEN_RES_CACHE1
		| E_IM_IIP_OPEN_RES_CACHE2,
		E_IM_IIP_OPEN_RES_LUT_A,
		E_IM_IIP_OPEN_RES_LUT_B,
		E_IM_IIP_OPEN_RES_LUT_C,
		E_IM_IIP_OPEN_RES_LUT_D,
		E_IM_IIP_OPEN_RES_LUT_E,
		E_IM_IIP_OPEN_RES_LUT_F,
		E_IM_IIP_OPEN_RES_HIST_0,
		E_IM_IIP_OPEN_RES_HIST_1,
		E_IM_IIP_OPEN_RES_HIST_2,
		E_IM_IIP_OPEN_RES_HIST_3,
		E_IM_IIP_OPEN_RES_CACHE1,
		E_IM_IIP_OPEN_RES_CACHE2,
	};

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));




	for( loopcnt = 0; loopcnt < (sizeof(accen_bitmask) / sizeof(accen_bitmask[0])); loopcnt++ ) {
		Ddim_Print(( "** bitmask = 0x%x\n", accen_bitmask[loopcnt] ));

		ercd = Im_IIP_Set_AccessEnable( accen_bitmask[loopcnt], 1 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		Im_IIP_On_Pclk();
		Ddim_Print(( dump_msg, IO_IIP.ACESEN0.word ));
		Im_IIP_Off_Pclk();

		ercd = Im_IIP_Set_AccessEnable( accen_bitmask[loopcnt], 0 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
		Im_IIP_On_Pclk();
		Ddim_Print(( dump_msg, IO_IIP.ACESEN0.word ));
		Im_IIP_Off_Pclk();
	}




	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_36: "
INT32 CT_Im_IIP_1_1_36( VOID )
{
	INT32					ercd;
	ULLONG					unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
//	static const CHAR		dump_msg[] = "LKEN=%u\n";

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));



#if 0	// B2Y Link not support
	ercd = Im_IIP_Set_B2YLinkEnable( 1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	Im_IIP_On_Pclk();
	Ddim_Print(( dump_msg, IO_IIP.IZACTL1.bit.LKEN ));
	Im_IIP_Off_Pclk();

	ercd = Im_IIP_Set_B2YLinkEnable( 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	Im_IIP_On_Pclk();
	Ddim_Print(( dump_msg, IO_IIP.IZACTL1.bit.LKEN ));
	Im_IIP_Off_Pclk();
#endif	// B2Y Link not support



	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#ifdef CO_DEBUG_ON_PC
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_37: "
INT32 CT_Im_IIP_1_1_37( VOID )
{
	INT32					ercd;
	ULLONG					unit_bitmask = 0xFFFFFFFFFFFFFFFFull;
	static const CHAR		dump_msg1[] = "B2Y=0x%x AFN0=0x%x AFN1=0x%x AFN2=0x%x AFN3=0x%x FR0=0x%x FR1=0x%x\n";
	static const CHAR		dump_msg2[] = "LD0=0x%x LD1=0x%x LD2=0x%x LD3=0x%x LD4=0x%x LD5=0x%x LD6=0x%x FLT=0x%x\n";
	T_IM_IIP_GENLINE		genline;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unit_bitmask, 0, 0, 1000 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));



	IO_IIP.GENLINE.GENLINE1.bit.B2Y_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE1.bit.AFN0_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE1.bit.AFN1_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE1.bit.AFN2_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE2.bit.AFN3_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE2.bit.FR0_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE2.bit.FR1_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE2.bit.LD0_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE3.bit.LD1_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE3.bit.LD2_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE3.bit.LD3_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE3.bit.LD4_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE4.bit.LD5_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE4.bit.LD6_OUTLINENO = 0x3FFF;
	IO_IIP.GENLINE.GENLINE4.bit.FLT_OUTLINENO = 0x3FFF;

	ercd = Im_IIP_Get_Genline( &genline );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	Ddim_Print(( dump_msg1,
				genline.b2y_outlineno,
				genline.afn_outlineno[0],
				genline.afn_outlineno[1],
				genline.afn_outlineno[2],
				genline.afn_outlineno[3],
				genline.frect_outlineno[0],
				genline.frect_outlineno[1]
				));
	Ddim_Print(( dump_msg2,
				genline.ld_outlineno[0],
				genline.ld_outlineno[1],
				genline.ld_outlineno[2],
				genline.ld_outlineno[3],
				genline.ld_outlineno[4],
				genline.ld_outlineno[5],
				genline.ld_outlineno[6],
				genline.flt_outlineno
				));


	IO_IIP.GENLINE.GENLINE1.bit.B2Y_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE1.bit.AFN0_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE1.bit.AFN1_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE1.bit.AFN2_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE2.bit.AFN3_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE2.bit.FR0_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE2.bit.FR1_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE2.bit.LD0_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE3.bit.LD1_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE3.bit.LD2_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE3.bit.LD3_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE3.bit.LD4_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE4.bit.LD5_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE4.bit.LD6_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE4.bit.FLT_OUTLINENO = 0;

	ercd = Im_IIP_Get_Genline( &genline );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	Ddim_Print(( dump_msg1,
				genline.b2y_outlineno,
				genline.afn_outlineno[0],
				genline.afn_outlineno[1],
				genline.afn_outlineno[2],
				genline.afn_outlineno[3],
				genline.frect_outlineno[0],
				genline.frect_outlineno[1]
				));
	Ddim_Print(( dump_msg2,
				genline.ld_outlineno[0],
				genline.ld_outlineno[1],
				genline.ld_outlineno[2],
				genline.ld_outlineno[3],
				genline.ld_outlineno[4],
				genline.ld_outlineno[5],
				genline.ld_outlineno[6],
				genline.flt_outlineno
				));


	IO_IIP.GENLINE.GENLINE1.bit.B2Y_OUTLINENO = 0;
	IO_IIP.GENLINE.GENLINE1.bit.AFN0_OUTLINENO = 1;
	IO_IIP.GENLINE.GENLINE1.bit.AFN1_OUTLINENO = 2;
	IO_IIP.GENLINE.GENLINE1.bit.AFN2_OUTLINENO = 3;
	IO_IIP.GENLINE.GENLINE2.bit.AFN3_OUTLINENO = 4;
	IO_IIP.GENLINE.GENLINE2.bit.FR0_OUTLINENO = 5;
	IO_IIP.GENLINE.GENLINE2.bit.FR1_OUTLINENO = 6;
	IO_IIP.GENLINE.GENLINE2.bit.LD0_OUTLINENO = 7;
	IO_IIP.GENLINE.GENLINE3.bit.LD1_OUTLINENO = 8;
	IO_IIP.GENLINE.GENLINE3.bit.LD2_OUTLINENO = 9;
	IO_IIP.GENLINE.GENLINE3.bit.LD3_OUTLINENO = 10;
	IO_IIP.GENLINE.GENLINE3.bit.LD4_OUTLINENO = 11;
	IO_IIP.GENLINE.GENLINE4.bit.LD5_OUTLINENO = 12;
	IO_IIP.GENLINE.GENLINE4.bit.LD6_OUTLINENO = 13;
	IO_IIP.GENLINE.GENLINE4.bit.FLT_OUTLINENO = 14;

	ercd = Im_IIP_Get_Genline( &genline );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));
	Ddim_Print(( dump_msg1,
				genline.b2y_outlineno,
				genline.afn_outlineno[0],
				genline.afn_outlineno[1],
				genline.afn_outlineno[2],
				genline.afn_outlineno[3],
				genline.frect_outlineno[0],
				genline.frect_outlineno[1]
				));
	Ddim_Print(( dump_msg2,
				genline.ld_outlineno[0],
				genline.ld_outlineno[1],
				genline.ld_outlineno[2],
				genline.ld_outlineno[3],
				genline.ld_outlineno[4],
				genline.ld_outlineno[5],
				genline.ld_outlineno[6],
				genline.flt_outlineno
				));


	// check: result = 0x0C000001
	ercd = Im_IIP_Get_Genline( NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	ercd = Im_IIP_Close_SWTRG( unit_bitmask, 0, 0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}
#endif //CO_DEBUG_ON_PC

#ifdef CO_DEBUG_ON_PC
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_38: "
INT32 CT_Im_IIP_1_1_38( VOID )
{
	INT32					ercd;
	UINT32					loopcnt;
	UINT32					loopcnt2;
	struct {
		ULONG		dtout_mon_lo;
		ULONG		dtout_mon_hi;
		ULONG		wait_mon_lo;
		ULONG		wait_mon_hi;
		ULONG		pdc_dtout_mon_0;
		ULONG		pdc_dtout_mon_1;
		ULONG		pdc_dtout_mon_2;
		ULONG		pdc_rdy_mon_0;
		ULONG		pdc_rdy_mon_1;
		ULONG		pdc_rdy_mon_2;
	} param_tbl[] = {
		[0] = {
			.dtout_mon_lo = 0xFFFFFFFFul,
			.dtout_mon_hi = 0xFFFFFFFFul,
			.wait_mon_lo = 0xFFFFFFFFul,
			.wait_mon_hi = 0xFFFFFFFFul,
			.pdc_dtout_mon_0 = 0x7FFFul,
			.pdc_dtout_mon_1 = 0x7FFFul,
			.pdc_dtout_mon_2 = 0x7FFFul,
			.pdc_rdy_mon_0 = 0x7FFFul,
			.pdc_rdy_mon_1 = 0x7FFFul,
			.pdc_rdy_mon_2 = 0x7FFFul,
		},
		[1] = {
			.dtout_mon_lo = 0,
			.dtout_mon_hi = 0,
			.wait_mon_lo = 0,
			.wait_mon_hi = 0,
			.pdc_dtout_mon_0 = 0,
			.pdc_dtout_mon_1 = 0,
			.pdc_dtout_mon_2 = 0,
			.pdc_rdy_mon_0 = 0,
			.pdc_rdy_mon_1 = 0,
			.pdc_rdy_mon_2 = 0,
		},
		[2] = {
			.dtout_mon_lo = 1,
			.dtout_mon_hi = 2,
			.wait_mon_lo = 3,
			.wait_mon_hi = 4,
			.pdc_dtout_mon_0 = 5,
			.pdc_dtout_mon_1 = 6,
			.pdc_dtout_mon_2 = 7,
			.pdc_rdy_mon_0 = 8,
			.pdc_rdy_mon_1 = 9,
			.pdc_rdy_mon_2 = 10,
		},
	};
	T_IM_IIP_DEBUG_MONITOR	monitor_val;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	for( loopcnt = 0; loopcnt < (sizeof(param_tbl) / sizeof(param_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** param%u\n", loopcnt ));

		IO_IIP.DTOUT_MON.bit.DTOUT_MON_LO = param_tbl[loopcnt].dtout_mon_lo;
		IO_IIP.DTOUT_MON.bit.DTOUT_MON_HI = param_tbl[loopcnt].dtout_mon_hi;
		IO_IIP.WAIT_MON.bit.WAIT_MON_LO = param_tbl[loopcnt].wait_mon_lo;
		IO_IIP.WAIT_MON.bit.WAIT_MON_HI = param_tbl[loopcnt].wait_mon_hi;
		IO_IIP.PDC_DTOUT_MON.PDC_DTOUT_MON1.bit.PDC_DTOUT_MON_0 = param_tbl[loopcnt].pdc_dtout_mon_0;
		IO_IIP.PDC_DTOUT_MON.PDC_DTOUT_MON1.bit.PDC_DTOUT_MON_1 = param_tbl[loopcnt].pdc_dtout_mon_1;
		IO_IIP.PDC_DTOUT_MON.PDC_DTOUT_MON2.bit.PDC_DTOUT_MON_2 = param_tbl[loopcnt].pdc_dtout_mon_2;
		IO_IIP.PDC_RDY_MON.PDC_RDY_MON1.bit.PDC_RDY_MON_0 = param_tbl[loopcnt].pdc_rdy_mon_0;
		IO_IIP.PDC_RDY_MON.PDC_RDY_MON1.bit.PDC_RDY_MON_1 = param_tbl[loopcnt].pdc_rdy_mon_1;
		IO_IIP.PDC_RDY_MON.PDC_RDY_MON2.bit.PDC_RDY_MON_2 = param_tbl[loopcnt].pdc_rdy_mon_2;

		ercd = Im_IIP_Get_Debug_Monitor( &monitor_val );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		Ddim_Print(( "data_out_port = 0x%llx\n", monitor_val.data_out_port ));
		Ddim_Print(( "waiting_port = 0x%llx\n", monitor_val.waiting_port ));
		for( loopcnt2 = 0; loopcnt2 < D_IM_IIP_CACHE_MAXNUM; loopcnt2++ ) {
			Ddim_Print(( "imgcache_data_out[%u] = 0x%x\n", loopcnt2, monitor_val.imgcache_data_out[loopcnt2] ));
		}
		for( loopcnt2 = 0; loopcnt2 < D_IM_IIP_CACHE_MAXNUM; loopcnt2++ ) {
			Ddim_Print(( "imgcache_ready[%u] = 0x%x\n", loopcnt2, monitor_val.imgcache_ready[loopcnt2] ));
		}
	}


	return D_DDIM_OK;
}
#endif //CO_DEBUG_ON_PC

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_39: "
INT32 CT_Im_IIP_1_1_39( VOID )
{
	INT32					ercd;
	UINT32					loopcnt;
	T_CT_IM_IIP_UNITID_TBL	unitid;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	for( loopcnt = 0; loopcnt < D_CT_IM_IIP_UNIT_ARRAY_MAX; loopcnt++ ) {
		unitid = g_ct_im_iip_unitid_array[loopcnt];

		Ddim_Print(( "** UNIT: %s\n", unitid.unitname ));

#ifdef CO_DEBUG_ON_PC
		IO_IIP.IZACTL2.bit.ABORT = 3;
#endif //CO_DEBUG_ON_PC
		ercd = Im_IIP_Start_DumpUnitReg( unitid.unitid );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		Ddim_Print(( "ABORT=%u SELRDREG_UNIT=%u DBEN=%u\n", IO_IIP.IZACTL2.bit.ABORT, IO_IIP.IZACTL1.bit.SELRDREG_UNIT, IO_IIP.IZACTL1.bit.DBEN ));
	}


	ercd = Im_IIP_Stop_DumpUnitReg();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_40: "
INT32 CT_Im_IIP_1_1_40( VOID )
{
	INT32					ercd;
	UINT32					loopcnt;
	static CHAR*			unit_name[] = {
		"ld0",
		"ld1",
		"ld2",
		"ld3",
		"flt",
		"afn0",
		"afn1",
		"afn2",
		"frect0",
		"mon",
		"sl0",
		"sl1",
		"sl2",
		"sl3",
		"sl4",
		"sl5",
		"csc0",
		"csc1",
		"lut",
		"gpc",
		"blend0",
		"blend1",
		"frect1",
		"cfl0",
		"csc2",
		"csc3",
		"sl6",
		"sl7",
		"sl8",
		"ld4",
		"ld5",
		"ld6",
		"ld7",
		"afn3",
		"mft",
	};

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	for( loopcnt = 0; loopcnt < (sizeof(unit_name) / sizeof(unit_name[0])); loopcnt++ ) {	/* pgr0834 */
		Ddim_Print(( "** UNIT: %s\n", unit_name[loopcnt] ));

#ifdef CO_DEBUG_ON_PC
		IO_IIP.IZACTL2.bit.ABORT = 3;
#endif //CO_DEBUG_ON_PC
		ercd = Im_IIP_Start_DumpUnitReg_ByName( unit_name[loopcnt] );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));

		Ddim_Print(( "ABORT=%u SELRDREG_UNIT=%u DBEN=%u\n", IO_IIP.IZACTL2.bit.ABORT, IO_IIP.IZACTL1.bit.SELRDREG_UNIT, IO_IIP.IZACTL1.bit.DBEN ));
	}


	ercd = Im_IIP_Stop_DumpUnitReg();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x\n", ercd ));


	return D_DDIM_OK;
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_41: "
INT32 CT_Im_IIP_1_1_41( VOID )
{
	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	return CT_Im_IIP_1_1_39();
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_42: "
INT32 CT_Im_IIP_1_1_42( VOID )
{
	T_IM_IIP_UTIL_RR iip_util_rr_0 = {
//		.afn_unitid = E_IM_IIP_UNIT_ID_AFN0,	// UNITID of AFN
//		.sl_unitid = D_SAMPLE_SL_UNITID,		// UNITID of SL
//		.src_pixid = E_IM_IIP_PIXID_0,			// Source image PIXID
//		.dst_pixid = E_IM_IIP_PIXID_1,			// Destination image PIXID
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 1280,							// Width
				.lines = 720,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
				.width = 1280,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 720,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 1280,
					.Cb_B  = 1280,
					.Cr_R  = 1280,
					.Alpha = 1280,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = 0x20000000,
					.Cb_B  = 0x21000000,
					.Cr_R  = 0x22000000,
					.Alpha = 0x20000000,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 640,							// Width
				.lines = 360,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
				.width = 640,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 360,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 640,
					.Cb_B  = 640,
					.Cr_R  = 640,
					.Alpha = 640,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = 0x23000000,
					.Cb_B  = 0x24000000,
					.Cr_R  = 0x25000000,
					.Alpha = 0x23000000,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.afn_cache_select = E_IM_IIP_PARAM_CSEL_0,	// AFN cache select.
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,			// PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
		.rotation = E_IM_IIP_UTIL_ROTATE_000,		// Angle of rotation
		.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR,// Resize mode
	};
	T_IM_IIP_UTIL_RR iip_util_rr_1 = {
//		.afn_unitid = E_IM_IIP_UNIT_ID_AFN0,	// UNITID of AFN
//		.sl_unitid = D_SAMPLE_SL_UNITID,		// UNITID of SL
//		.src_pixid = E_IM_IIP_PIXID_0,			// Source image PIXID
//		.dst_pixid = E_IM_IIP_PIXID_1,			// Destination image PIXID
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 1440,							// Width
				.lines = 1080,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC420,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
				.width = 1440,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 1080,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 1440,
					.Cb_B  = 1440 /2,
					.Cr_R  = 1440 /2,
					.Alpha = 1440,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = 0x20000000,
					.Cb_B  = 0x21000000,
					.Cr_R  = 0x22000000,
					.Alpha = 0x20000000,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 960,							// Width
				.lines = 720,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC420,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
				.width = 960,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 720,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 960,
					.Cb_B  = 960 /2,
					.Cr_R  = 960 /2,
					.Alpha = 960,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = 0x23000000,
					.Cb_B  = 0x24000000,
					.Cr_R  = 0x25000000,
					.Alpha = 0x23000000,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.afn_cache_select = E_IM_IIP_PARAM_CSEL_0,	// AFN cache select.
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,			// PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
		.rotation = E_IM_IIP_UTIL_ROTATE_000,		// Angle of rotation
		.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR,// Resize mode
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES *2 +8];
	ULONG param_buffer_addr = (((ULONG)param_buffer) + 7UL) & 0xFFFFFFF8UL;	// Convet 8bytes align
	INT32 wait_time = 100;
	INT32 ercd;
	UINT32 loopcnt;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));


	for( loopcnt = 0; ; loopcnt++ ) {
		switch( loopcnt ) {
			default:
				return D_DDIM_OK;
			case 0:
				iip_util_rr_0.afn_unitid = E_IM_IIP_UNIT_ID_AFN0;
				iip_util_rr_0.sl_unitid = E_IM_IIP_UNIT_ID_SL2;
				iip_util_rr_0.src_pixid = E_IM_IIP_PIXID_0;
				iip_util_rr_0.dst_pixid = E_IM_IIP_PIXID_1;
				iip_util_rr_0.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;
				iip_util_rr_1.afn_unitid = E_IM_IIP_UNIT_ID_AFN1;
				iip_util_rr_1.sl_unitid = E_IM_IIP_UNIT_ID_SL3;
				iip_util_rr_1.src_pixid = E_IM_IIP_PIXID_2;
				iip_util_rr_1.dst_pixid = E_IM_IIP_PIXID_3;
				iip_util_rr_1.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_NEAREST;
				break;
			case 1:
				iip_util_rr_0.afn_unitid = E_IM_IIP_UNIT_ID_AFN2;
				iip_util_rr_0.sl_unitid = E_IM_IIP_UNIT_ID_SL4;
				iip_util_rr_0.src_pixid = E_IM_IIP_PIXID_4;
				iip_util_rr_0.dst_pixid = E_IM_IIP_PIXID_5;
				iip_util_rr_0.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BICUBIC;
				iip_util_rr_1.afn_unitid = E_IM_IIP_UNIT_ID_AFN0;
				iip_util_rr_1.sl_unitid = E_IM_IIP_UNIT_ID_SL5;
				iip_util_rr_1.src_pixid = E_IM_IIP_PIXID_6;
				iip_util_rr_1.dst_pixid = E_IM_IIP_PIXID_7;
				iip_util_rr_1.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;
				break;
			case 2:
				iip_util_rr_0.afn_unitid = E_IM_IIP_UNIT_ID_AFN2;
				iip_util_rr_0.sl_unitid = E_IM_IIP_UNIT_ID_SL6;
				iip_util_rr_0.src_pixid = E_IM_IIP_PIXID_8;
				iip_util_rr_0.dst_pixid = E_IM_IIP_PIXID_9;
				iip_util_rr_0.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BICUBIC;
				iip_util_rr_1.afn_unitid = E_IM_IIP_UNIT_ID_AFN0;
				iip_util_rr_1.sl_unitid = E_IM_IIP_UNIT_ID_SL5;
				iip_util_rr_1.src_pixid = E_IM_IIP_PIXID_10;
				iip_util_rr_1.dst_pixid = E_IM_IIP_PIXID_11;
				iip_util_rr_1.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;
				break;
		}

		Ddim_Print(( "** %u\n", loopcnt ));
		ercd = IM_IIP_Utility_ResizeRotate( &iip_util_rr_0,
											&iip_util_rr_1,
											param_master_IF,
											param_buffer_addr,
											wait_time );
		printf( "IM_IIP_Utility_ResizeRotate result = 0x%x.\n", ercd );

		switch( ercd ) {
			case D_IM_IIP_OK:	// FALLTHROUGH
			case D_IM_IIP_TIMEOUT_ERR:
				CT_Im_IIP_Print_Unitcfg_Byid( iip_util_rr_0.afn_unitid );
				CT_Im_IIP_Print_Unitcfg_Byid( iip_util_rr_0.sl_unitid );
				CT_Im_IIP_Print_Pixfmttbl_Byid( iip_util_rr_0.src_pixid );
				CT_Im_IIP_Print_Pixfmttbl_Byid( iip_util_rr_0.dst_pixid  );

				CT_Im_IIP_Print_Unitcfg_Byid( iip_util_rr_1.afn_unitid );
				CT_Im_IIP_Print_Unitcfg_Byid( iip_util_rr_1.sl_unitid );
				CT_Im_IIP_Print_Pixfmttbl_Byid( iip_util_rr_1.src_pixid );
				CT_Im_IIP_Print_Pixfmttbl_Byid( iip_util_rr_1.dst_pixid  );
				break;
			default:
				// do nothing
				break;
		}
	}
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_43: "
INT32 CT_Im_IIP_1_1_43( VOID )
{
	T_IM_IIP_UTIL_CSC iip_util_csc_0 = {
//		.ld_unitid = 0,
//		.csc_unitid = 0,
//		.sl_unitid = 0,
//		.src_pixid = 0,
//		.dst_pixid = 0,
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 1280,							// Width
				.lines = 720,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
				.width = 1280,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 720,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 1280,
					.Cb_B  = 1280,
					.Cr_R  = 1280,
					.Alpha = 1280,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = 0x20000000,
					.Cb_B  = 0x21000000,
					.Cr_R  = 0x22000000,
					.Alpha = 0x20000000,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 640,							// Width
				.lines = 360,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
				.width = 640,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 360,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 640,
					.Cb_B  = 640,
					.Cr_R  = 640,
					.Alpha = 640,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = 0x23000000,
					.Cb_B  = 0x24000000,
					.Cr_R  = 0x25000000,
					.Alpha = 0x23000000,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select = E_IM_IIP_PARAM_CSEL_1,
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.csck = {
			{ 0, 1, 2, 3 },
			{ 4, 5, 6, 7 },
			{ 8, 9, 10, 11 },
		},
		.csck_mode_sel = 0,
		.alpha_in_sel = E_IM_IIP_PARAM_ALPSEL_DIRECT,
		.alpha_out_sel = E_IM_IIP_PARAM_ALOSEL_CALCULATED,
		.alpha_val = 0,
		.sl_sftval = 0,
	};
	T_IM_IIP_UTIL_CSC iip_util_csc_1 = {
//		.ld_unitid = 0,
//		.csc_unitid = 0,
//		.sl_unitid = 0,
//		.src_pixid = 0,
//		.dst_pixid = 0,
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 1440,							// Width
				.lines = 1080,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC420,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
				.width = 1440,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 1080,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 1440,
					.Cb_B  = 1440 /2,
					.Cr_R  = 1440 /2,
					.Alpha = 1440,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = 0x20000000,
					.Cb_B  = 0x21000000,
					.Cr_R  = 0x22000000,
					.Alpha = 0x20000000,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 960,							// Width
				.lines = 720,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC420,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
				.width = 960,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 720,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 960,
					.Cb_B  = 960 /2,
					.Cr_R  = 960 /2,
					.Alpha = 960,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = 0x23000000,
					.Cb_B  = 0x24000000,
					.Cr_R  = 0x25000000,
					.Alpha = 0x23000000,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select = E_IM_IIP_PARAM_CSEL_0,
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.csck = {
			{ 12, 13, 14, 15 },
			{ 16, 17, 18, 19 },
			{ 20, 21, 22, 23 },
			{ 24, 25, 26, 27 },
		},
		.csck_mode_sel = 0,
		.alpha_in_sel = E_IM_IIP_PARAM_ALPSEL_DIRECT,
		.alpha_out_sel = E_IM_IIP_PARAM_ALOSEL_CALCULATED,
		.alpha_val = 0,
		.sl_sftval = 0,
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_CSC_BUF_BYTES *2 +8];
	ULONG param_buffer_addr = (((ULONG)param_buffer) + 7UL) & 0xFFFFFFF8UL;	// Convet 8bytes align
	INT32 wait_time = 100;
	INT32 ercd;
	UINT32 loopcnt;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));


	for( loopcnt = 0; ; loopcnt++ ) {
		switch( loopcnt ) {
			default:
				return D_DDIM_OK;
			case 0:
				iip_util_csc_0.ld_unitid = E_IM_IIP_UNIT_ID_LD0;
				iip_util_csc_0.csc_unitid = E_IM_IIP_UNIT_ID_CSC0;
				iip_util_csc_0.sl_unitid = E_IM_IIP_UNIT_ID_SL2;
				iip_util_csc_0.src_pixid = E_IM_IIP_PIXID_0;
				iip_util_csc_0.dst_pixid = E_IM_IIP_PIXID_1;
				iip_util_csc_1.ld_unitid = E_IM_IIP_UNIT_ID_LD1;
				iip_util_csc_1.csc_unitid = E_IM_IIP_UNIT_ID_CSC1;
				iip_util_csc_1.sl_unitid = E_IM_IIP_UNIT_ID_SL3;
				iip_util_csc_1.src_pixid = E_IM_IIP_PIXID_2;
				iip_util_csc_1.dst_pixid = E_IM_IIP_PIXID_3;
				break;
			case 1:
				iip_util_csc_0.ld_unitid = E_IM_IIP_UNIT_ID_LD2;
				iip_util_csc_0.csc_unitid = E_IM_IIP_UNIT_ID_CSC2;
				iip_util_csc_0.sl_unitid = E_IM_IIP_UNIT_ID_SL4;
				iip_util_csc_0.src_pixid = E_IM_IIP_PIXID_4;
				iip_util_csc_0.dst_pixid = E_IM_IIP_PIXID_5;
				iip_util_csc_1.ld_unitid = E_IM_IIP_UNIT_ID_LD1;
				iip_util_csc_1.csc_unitid = E_IM_IIP_UNIT_ID_CSC3;
				iip_util_csc_1.sl_unitid = E_IM_IIP_UNIT_ID_SL5;
				iip_util_csc_1.src_pixid = E_IM_IIP_PIXID_6;
				iip_util_csc_1.dst_pixid = E_IM_IIP_PIXID_7;
				break;
			case 2:
				iip_util_csc_0.ld_unitid = E_IM_IIP_UNIT_ID_LD2;
				iip_util_csc_0.csc_unitid = E_IM_IIP_UNIT_ID_CSC3;
				iip_util_csc_0.sl_unitid = E_IM_IIP_UNIT_ID_SL6;
				iip_util_csc_0.src_pixid = E_IM_IIP_PIXID_8;
				iip_util_csc_0.dst_pixid = E_IM_IIP_PIXID_9;
				iip_util_csc_1.ld_unitid = E_IM_IIP_UNIT_ID_LD1;
				iip_util_csc_1.csc_unitid = E_IM_IIP_UNIT_ID_CSC2;
				iip_util_csc_1.sl_unitid = E_IM_IIP_UNIT_ID_SL5;
				iip_util_csc_1.src_pixid = E_IM_IIP_PIXID_10;
				iip_util_csc_1.dst_pixid = E_IM_IIP_PIXID_11;
				break;
		}

		Ddim_Print(( "** %u\n", loopcnt ));
		ercd = IM_IIP_Utility_CSC( &iip_util_csc_0,
								   &iip_util_csc_1,
								   param_master_IF,
								   param_buffer_addr,
								   wait_time );
		printf( "IM_IIP_Utility_CSC result = 0x%x.\n", ercd );

		switch( ercd ) {
			case D_IM_IIP_OK:	// FALLTHROUGH
			case D_IM_IIP_TIMEOUT_ERR:
				CT_Im_IIP_Print_Unitcfg_Byid( iip_util_csc_0.ld_unitid );
				CT_Im_IIP_Print_Unitcfg_Byid( iip_util_csc_0.csc_unitid );
				CT_Im_IIP_Print_Unitcfg_Byid( iip_util_csc_0.sl_unitid );
				CT_Im_IIP_Print_Pixfmttbl_Byid( iip_util_csc_0.src_pixid );
				CT_Im_IIP_Print_Pixfmttbl_Byid( iip_util_csc_0.dst_pixid  );

				CT_Im_IIP_Print_Unitcfg_Byid( iip_util_csc_1.ld_unitid );
				CT_Im_IIP_Print_Unitcfg_Byid( iip_util_csc_1.csc_unitid );
				CT_Im_IIP_Print_Unitcfg_Byid( iip_util_csc_1.sl_unitid );
				CT_Im_IIP_Print_Pixfmttbl_Byid( iip_util_csc_1.src_pixid );
				CT_Im_IIP_Print_Pixfmttbl_Byid( iip_util_csc_1.dst_pixid  );
				break;
			default:
				// do nothing
				break;
		}
	}
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_44: "
INT32 CT_Im_IIP_1_1_44( VOID )
{
	USHORT lutram_a[D_IM_IIP_LUT_MAXNUM];
	USHORT lutram_b[D_IM_IIP_LUT_MAXNUM];
	USHORT lutram_c[D_IM_IIP_LUT_MAXNUM];
	USHORT lutram_d[D_IM_IIP_LUT_MAXNUM];
	T_IM_IIP_UTIL_LUT cfg = {
//		.ld_unitid = 0,
//		.lut_unitid = 0,
//		.sl_unitid = 0,
//		.src_pixid = 0,
//		.dst_pixid = 0,
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 1280,							// Width
				.lines = 720,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
				.width = 1280,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 720,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 1280,
					.Cb_B  = 1280,
					.Cr_R  = 1280,
					.Alpha = 1280,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = 0x20000000,
					.Cb_B  = 0x21000000,
					.Cr_R  = 0x22000000,
					.Alpha = 0x20000000,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 640,							// Width
				.lines = 360,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
				.width = 640,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 360,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 640,
					.Cb_B  = 640,
					.Cr_R  = 640,
					.Alpha = 640,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = 0x23000000,
					.Cb_B  = 0x24000000,
					.Cr_R  = 0x25000000,
					.Alpha = 0x23000000,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select = E_IM_IIP_PARAM_CSEL_1,
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.lut_pix_depth = {
			0, 1, 2, 3
		},
		.splbit = {
			4, 5, 6, 7,
		},
		.lutram = {
			.buffer_bytes = {
				sizeof(lutram_a), sizeof(lutram_b), sizeof(lutram_c), sizeof(lutram_d), 0, 0,
			},
			.buffer_addr = {
				lutram_a, lutram_b, lutram_c, lutram_d, 0, 0,
			},
		},
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_LUT_BUF_BYTES +8];
	ULONG param_buffer_addr = (((ULONG)param_buffer) + 7UL) & 0xFFFFFFF8UL;	// Convet 8bytes align
	INT32 wait_time = 100;
	INT32 ercd;
	UINT32 loopcnt;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	for( loopcnt = 0; loopcnt < D_IM_IIP_LUT_MAXNUM; loopcnt++ ) {
		lutram_a[loopcnt] = 0x0000 + loopcnt;
		lutram_b[loopcnt] = 0x1000 + loopcnt;
		lutram_c[loopcnt] = 0x2000 + loopcnt;
		lutram_d[loopcnt] = 0x3000 + loopcnt;
	}

	for( loopcnt = 0; ; loopcnt++ ) {
		switch( loopcnt ) {
			default:
				return D_DDIM_OK;
			case 0:
				cfg.ld_unitid = E_IM_IIP_UNIT_ID_LD0;
				cfg.lut_unitid = E_IM_IIP_UNIT_ID_LUT;
				cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL2;
				cfg.src_pixid = E_IM_IIP_PIXID_0;
				cfg.dst_pixid = E_IM_IIP_PIXID_1;
				break;
			case 1:
				cfg.ld_unitid = E_IM_IIP_UNIT_ID_LD1;
				cfg.lut_unitid = E_IM_IIP_UNIT_ID_LUT;
				cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL3;
				cfg.src_pixid = E_IM_IIP_PIXID_2;
				cfg.dst_pixid = E_IM_IIP_PIXID_3;
				break;
			case 2:
				cfg.ld_unitid = E_IM_IIP_UNIT_ID_LD2;
				cfg.lut_unitid = E_IM_IIP_UNIT_ID_LUT;
				cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL4;
				cfg.src_pixid = E_IM_IIP_PIXID_4;
				cfg.dst_pixid = E_IM_IIP_PIXID_5;
				break;
			case 3:
				cfg.ld_unitid = E_IM_IIP_UNIT_ID_LD2;
				cfg.lut_unitid = E_IM_IIP_UNIT_ID_LUT;
				cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL5;
				cfg.src_pixid = E_IM_IIP_PIXID_6;
				cfg.dst_pixid = E_IM_IIP_PIXID_7;
				break;
			case 4:
				cfg.ld_unitid = E_IM_IIP_UNIT_ID_LD2;
				cfg.lut_unitid = E_IM_IIP_UNIT_ID_LUT;
				cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL6;
				cfg.src_pixid = E_IM_IIP_PIXID_8;
				cfg.dst_pixid = E_IM_IIP_PIXID_9;
				break;
			case 5:
				cfg.ld_unitid = E_IM_IIP_UNIT_ID_LD2;
				cfg.lut_unitid = E_IM_IIP_UNIT_ID_LUT;
				cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL6;
				cfg.src_pixid = E_IM_IIP_PIXID_10;
				cfg.dst_pixid = E_IM_IIP_PIXID_11;
				break;
		}

		Ddim_Print(( "** %u\n", loopcnt ));
		ercd = IM_IIP_Utility_LUT( &cfg,
								   param_master_IF,
								   param_buffer_addr,
								   wait_time );
		printf( "IM_IIP_Utility_LUT result = 0x%x.\n", ercd );

		switch( ercd ) {
			case D_IM_IIP_OK:	// FALLTHROUGH
			case D_IM_IIP_TIMEOUT_ERR:
				CT_Im_IIP_Print_Unitcfg_Byid( cfg.ld_unitid );
				CT_Im_IIP_Print_Unitcfg_Byid( cfg.lut_unitid );
				CT_Im_IIP_Print_Unitcfg_Byid( cfg.sl_unitid );
				CT_Im_IIP_Print_Pixfmttbl_Byid( cfg.src_pixid );
				CT_Im_IIP_Print_Pixfmttbl_Byid( cfg.dst_pixid  );
				break;
			default:
				// do nothing
				break;
		}
	}
}

#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_1_1_45: "
INT32 CT_Im_IIP_1_1_45( VOID )
{
	T_IM_IIP_UTIL_ALPHABLEND cfg = {
//		.ld_unitid = {
//			0, 0,
//		},
//		.blend_unitid = 0,
//		.sl_unitid = 0,
//		.src_pixid = {
//			0, 0,
//		},
//		.dst_pixid = 0,
		.src = {
			[0] = {									// Source image info[0].
				.rect = {								// Rectangle info. (Source image area)
					.top = 0.0,								// Vertical position of top-left pixel
					.left = 0.0,							// Horizontal position of top-left pixel
					.width = 1920,							// Width
					.lines = 1080,							// Lines
				},
				.gbl = {								// Global image info.
					.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
					.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
					.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
					.width = 1920,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
					.lines = 1080,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
					.line_bytes = {								// The size of byte of Global image 1 line.
						.Y_G   = 1920,
						.Cb_B  = 1920,
						.Cr_R  = 1920,
						.Alpha = 1920,			// dummy
					},
					.addr = {									// Beginning address of Global image.
						.Y_G   = 0x20000000,
						.Cb_B  = 0x21000000,
						.Cr_R  = 0x22000000,
						.Alpha = 0x20000000,	// dummy
					},
					.masterIF_Y_G = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
					.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
					.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
					.masterIF_Alpha = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
				},
			},
			[1] = {									// Source image info[1].
				.rect = {								// Rectangle info. (Source image area)
					.top = 0.0,								// Vertical position of top-left pixel
					.left = 0.0,							// Horizontal position of top-left pixel
					.width = 1280,							// Width
					.lines = 720,							// Lines
				},
				.gbl = {								// Global image info.
					.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
					.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
					.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
					.width = 1280,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
					.lines = 720,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
					.line_bytes = {								// The size of byte of Global image 1 line.
						.Y_G   = 1280,
						.Cb_B  = 1280,
						.Cr_R  = 1280,
						.Alpha = 1280,			// dummy
					},
					.addr = {									// Beginning address of Global image.
						.Y_G   = 0x23000000,
						.Cb_B  = 0x24000000,
						.Cr_R  = 0x25000000,
						.Alpha = 0x23000000,	// dummy
					},
					.masterIF_Y_G = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
					.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
					.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
					.masterIF_Alpha = D_IM_IIP_MASTER_IF0,			// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
				},
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 640,							// Width
				.lines = 360,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
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
				.width = 640,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 360,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 640,
					.Cb_B  = 640,
					.Cr_R  = 640,
					.Alpha = 640,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = 0x26000000,
					.Cb_B  = 0x27000000,
					.Cr_R  = 0x28000000,
					.Alpha = 0x26000000,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select = {
			E_IM_IIP_PARAM_CSEL_0, E_IM_IIP_PARAM_CSEL_1,
		},
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.alpha = {
			.step = E_IM_IIP_PARAM_ALPHA_STEP_8BITS,
			.sel = E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL,
			.out = E_IM_IIP_PARAM_ALPHA_OUT_NONE,
			.ALPHA_VAL = {
				1, 2, 3, 4,
			},
		},
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_ALPHABLEND_BUF_BYTES +8];
	ULONG param_buffer_addr = (((ULONG)param_buffer) + 7UL) & 0xFFFFFFF8UL;	// Convet 8bytes align
	INT32 wait_time = 100;
	INT32 ercd;
	UINT32 loopcnt;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	for( loopcnt = 0; ; loopcnt++ ) {
		switch( loopcnt ) {
			default:
				return D_DDIM_OK;
			case 0:
				cfg.ld_unitid[0] = E_IM_IIP_UNIT_ID_LD0;
				cfg.ld_unitid[1] = E_IM_IIP_UNIT_ID_LD1;
				cfg.blend_unitid = E_IM_IIP_UNIT_ID_BLEND0;
				cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL2;
				cfg.src_pixid[0] = E_IM_IIP_PIXID_0;
				cfg.src_pixid[1] = E_IM_IIP_PIXID_1;
				cfg.dst_pixid = E_IM_IIP_PIXID_2;
				break;
			case 1:
				cfg.ld_unitid[0] = E_IM_IIP_UNIT_ID_LD1;
				cfg.ld_unitid[1] = E_IM_IIP_UNIT_ID_LD2;
				cfg.blend_unitid = E_IM_IIP_UNIT_ID_BLEND0;
				cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL3;
				cfg.src_pixid[0] = E_IM_IIP_PIXID_3;
				cfg.src_pixid[1] = E_IM_IIP_PIXID_4;
				cfg.dst_pixid = E_IM_IIP_PIXID_5;
				break;
			case 2:
				cfg.ld_unitid[0] = E_IM_IIP_UNIT_ID_LD0;
				cfg.ld_unitid[1] = E_IM_IIP_UNIT_ID_LD1;
				cfg.blend_unitid = E_IM_IIP_UNIT_ID_BLEND0;
				cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL4;
				cfg.src_pixid[0] = E_IM_IIP_PIXID_6;
				cfg.src_pixid[1] = E_IM_IIP_PIXID_7;
				cfg.dst_pixid = E_IM_IIP_PIXID_8;
				break;
			case 3:
				cfg.ld_unitid[0] = E_IM_IIP_UNIT_ID_LD0;
				cfg.ld_unitid[1] = E_IM_IIP_UNIT_ID_LD1;
				cfg.blend_unitid = E_IM_IIP_UNIT_ID_BLEND0;
				cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL5;
				cfg.src_pixid[0] = E_IM_IIP_PIXID_9;
				cfg.src_pixid[1] = E_IM_IIP_PIXID_10;
				cfg.dst_pixid = E_IM_IIP_PIXID_11;
				break;
		}

		Ddim_Print(( "** %u\n", loopcnt ));
		ercd = IM_IIP_Utility_AlphaBlend( &cfg,
								   param_master_IF,
								   param_buffer_addr,
								   wait_time );
		printf( "IM_IIP_Utility_AlphaBlend result = 0x%x.\n", ercd );

		switch( ercd ) {
			case D_IM_IIP_OK:	// FALLTHROUGH
			case D_IM_IIP_TIMEOUT_ERR:
				CT_Im_IIP_Print_Unitcfg_Byid( cfg.ld_unitid[0] );
				CT_Im_IIP_Print_Unitcfg_Byid( cfg.ld_unitid[1] );
				CT_Im_IIP_Print_Unitcfg_Byid( cfg.blend_unitid );
				CT_Im_IIP_Print_Unitcfg_Byid( cfg.sl_unitid );
				CT_Im_IIP_Print_Pixfmttbl_Byid( cfg.src_pixid[0] );
				CT_Im_IIP_Print_Pixfmttbl_Byid( cfg.src_pixid[1] );
				CT_Im_IIP_Print_Pixfmttbl_Byid( cfg.dst_pixid  );
				break;
			default:
				// do nothing
				break;
		}
	}
}

static INT32 CT_Im_IIP_Run_1_1( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-1-1-2
		case 2:
			return CT_Im_IIP_1_1_2();

		// Test-1-1-3
		case 3:
			return CT_Im_IIP_1_1_3();

		// Test-1-1-4
		case 4:
			return CT_Im_IIP_1_1_4();

		// Test-1-1-5
		case 5:
			return CT_Im_IIP_1_1_5();

		// Test-1-1-6
		case 6:
			return CT_Im_IIP_1_1_6();

		// Test-1-1-7
		case 7:
			return CT_Im_IIP_1_1_7();

		// Test-1-1-8
		case 8:
			return CT_Im_IIP_1_1_8();

		// Test-1-1-9
		case 9:
			return CT_Im_IIP_1_1_9();

		// Test-1-1-10
		case 10:
			return CT_Im_IIP_1_1_10();

		// Test-1-1-11
		case 11:
			return CT_Im_IIP_1_1_11();

		// Test-1-1-12
		case 12:
			return CT_Im_IIP_1_1_12();

		// Test-1-1-13
		case 13:
			return CT_Im_IIP_1_1_13();

		// Test-1-1-14
		case 14:
			return CT_Im_IIP_1_1_14();

		// Test-1-1-15
		case 15:
			return CT_Im_IIP_1_1_15();

		// Test-1-1-16
		case 16:
			return CT_Im_IIP_1_1_16();

		// Test-1-1-17
		case 17:
			return CT_Im_IIP_1_1_17();

		// Test-1-1-18
		case 18:
			return CT_Im_IIP_1_1_18();

		// Test-1-1-19
		case 19:
			return CT_Im_IIP_1_1_19();

		// Test-1-1-20
		case 20:
			return CT_Im_IIP_1_1_20();

		// Test-1-1-21
		case 21:
			return CT_Im_IIP_1_1_21();

		// Test-1-1-22
		case 22:
			return CT_Im_IIP_1_1_22();

		// Test-1-1-23
		case 23:
			return CT_Im_IIP_1_1_23();

		// Test-1-1-24
		case 24:
			return CT_Im_IIP_1_1_24();

		// Test-1-1-25
		case 25:
			return CT_Im_IIP_1_1_25();

#ifdef CO_CT_IM_IIP_ENABLE_1_1_26
		// Test-1-1-26
		case 26:
			return CT_Im_IIP_1_1_26();
#endif

#ifdef CO_DEBUG_ON_PC
		// Test-1-1-27
		case 27:
			return CT_Im_IIP_1_1_27();
#endif

#ifdef CO_DEBUG_ON_PC
		// Test-1-1-28
		case 28:
			return CT_Im_IIP_1_1_28();
#endif

#ifdef CO_DEBUG_ON_PC
		// Test-1-1-29
		case 29:
			return CT_Im_IIP_1_1_29();
#endif

#ifdef CO_DEBUG_ON_PC
		// Test-1-1-30
		case 30:
			return CT_Im_IIP_1_1_30();
#endif

#ifdef CO_DEBUG_ON_PC
		// Test-1-1-31
		case 31:
			return CT_Im_IIP_1_1_31();
#endif

#ifdef CO_DEBUG_ON_PC
		// Test-1-1-32
		case 32:
			return CT_Im_IIP_1_1_32();
#endif

#ifdef CO_DEBUG_ON_PC
		// Test-1-1-33
		case 33:
			return CT_Im_IIP_1_1_33();
#endif

#ifdef CO_CT_IM_IIP_ENABLE_1_1_34
		// Test-1-1-34
		case 34:
			return CT_Im_IIP_1_1_34();
#endif

		// Test-1-1-35
		case 35:
			return CT_Im_IIP_1_1_35();

		// Test-1-1-36
		case 36:
			return CT_Im_IIP_1_1_36();

#ifdef CO_DEBUG_ON_PC
		// Test-1-1-37
		case 37:
			return CT_Im_IIP_1_1_37();
#endif

#ifdef CO_DEBUG_ON_PC
		// Test-1-1-38
		case 38:
			return CT_Im_IIP_1_1_38();
#endif

		// Test-1-1-39
		case 39:
			return CT_Im_IIP_1_1_39();

		// Test-1-1-40
		case 40:
			return CT_Im_IIP_1_1_40();

		// Test-1-1-41
		case 41:
			return CT_Im_IIP_1_1_41();

		// Test-1-1-42
		case 42:
			return CT_Im_IIP_1_1_42();

		// Test-1-1-43
		case 43:
			return CT_Im_IIP_1_1_43();

		// Test-1-1-44
		case 44:
			return CT_Im_IIP_1_1_44();

		// Test-1-1-45
		case 45:
			return CT_Im_IIP_1_1_45();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// Im_IIP_Init 2-1-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_2_1_1: "
INT32 CT_Im_IIP_2_1_1( VOID )
{
	INT32							ercd;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Init();
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Init_Unit 2-1-2
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_2_1_2: "
INT32 CT_Im_IIP_2_1_2( VOID )
{
	INT32							ercd;
	UINT32							loopcnt;
	static const E_IM_IIP_UNIT_ID	unit_id[] = {
		E_IM_IIP_UNIT_ID_LD0,
		E_IM_IIP_UNIT_ID_LD1,
		E_IM_IIP_UNIT_ID_LD2,
		E_IM_IIP_UNIT_ID_LD3,
		E_IM_IIP_UNIT_ID_FLT,
		E_IM_IIP_UNIT_ID_AFN0,
		E_IM_IIP_UNIT_ID_AFN1,
		E_IM_IIP_UNIT_ID_AFN2,
		E_IM_IIP_UNIT_ID_FRECT0,
		E_IM_IIP_UNIT_ID_MON,
		E_IM_IIP_UNIT_ID_SL0,
		E_IM_IIP_UNIT_ID_SL1,
		E_IM_IIP_UNIT_ID_SL2,
		E_IM_IIP_UNIT_ID_SL3,
		E_IM_IIP_UNIT_ID_SL4,
		E_IM_IIP_UNIT_ID_SL5,
		E_IM_IIP_UNIT_ID_CSC0,
		E_IM_IIP_UNIT_ID_CSC1,
		E_IM_IIP_UNIT_ID_LUT,
		E_IM_IIP_UNIT_ID_GPC,
		E_IM_IIP_UNIT_ID_BLEND0,
		E_IM_IIP_UNIT_ID_BLEND1,
		E_IM_IIP_UNIT_ID_FRECT1,
		E_IM_IIP_UNIT_ID_CFL0,
		E_IM_IIP_UNIT_ID_CSC2,
		E_IM_IIP_UNIT_ID_CSC3,
		E_IM_IIP_UNIT_ID_SL6,
		E_IM_IIP_UNIT_ID_SL7,
		E_IM_IIP_UNIT_ID_SL8,
		E_IM_IIP_UNIT_ID_LD4,
		E_IM_IIP_UNIT_ID_LD5,
		E_IM_IIP_UNIT_ID_LD6,
		E_IM_IIP_UNIT_ID_LD7,
		E_IM_IIP_UNIT_ID_AFN3,
		E_IM_IIP_UNIT_ID_MFT,
	};

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	for( loopcnt = 0; loopcnt < (sizeof(unit_id) / sizeof(unit_id[0])); loopcnt++ ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "** unit_id %u\n", unit_id[loopcnt] ));
		ercd = Im_IIP_Init_Unit( unit_id[loopcnt] );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));
	}

	return D_DDIM_OK;
}


// Im_IIP_Update_PADRS 2-1-3
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_2_1_3: "

typedef struct {
	E_IM_IIP_UNIT_ID				unit_id;
	volatile struct io_iip_uinftbl*	uinftbl;
} T_CT_IM_IIP_2_1_3;

INT32 CT_Im_IIP_2_1_3( VOID )
{
	INT32							ercd;
	UINT32							loopcnt;
	T_CT_IM_IIP_2_1_3				padrs_tbl[] = {
		{ E_IM_IIP_UNIT_ID_LD0, &IO_IIP.UNITINFTBL_LD0 },
		{ E_IM_IIP_UNIT_ID_LD1, &IO_IIP.UNITINFTBL_LD1 },
		{ E_IM_IIP_UNIT_ID_LD2, &IO_IIP.UNITINFTBL_LD2 },
		{ E_IM_IIP_UNIT_ID_FLT, &IO_IIP.UNITINFTBL_FLT },
		{ E_IM_IIP_UNIT_ID_AFN0, &IO_IIP.UNITINFTBL_AFN0 },
		{ E_IM_IIP_UNIT_ID_AFN1, &IO_IIP.UNITINFTBL_AFN1 },
		{ E_IM_IIP_UNIT_ID_AFN2, &IO_IIP.UNITINFTBL_AFN2 },
		{ E_IM_IIP_UNIT_ID_FRECT0, &IO_IIP.UNITINFTBL_FRC0 },
		{ E_IM_IIP_UNIT_ID_MON, &IO_IIP.UNITINFTBL_MON },
		{ E_IM_IIP_UNIT_ID_SL0, &IO_IIP.UNITINFTBL_SL0 },
		{ E_IM_IIP_UNIT_ID_SL1, &IO_IIP.UNITINFTBL_SL1 },
		{ E_IM_IIP_UNIT_ID_SL2, &IO_IIP.UNITINFTBL_SL2 },
		{ E_IM_IIP_UNIT_ID_SL3, &IO_IIP.UNITINFTBL_SL3 },
		{ E_IM_IIP_UNIT_ID_SL4, &IO_IIP.UNITINFTBL_SL4 },
		{ E_IM_IIP_UNIT_ID_SL5, &IO_IIP.UNITINFTBL_SL5 },
		{ E_IM_IIP_UNIT_ID_CSC0, &IO_IIP.UNITINFTBL_CSC0 },
		{ E_IM_IIP_UNIT_ID_CSC1, &IO_IIP.UNITINFTBL_CSC1 },
		{ E_IM_IIP_UNIT_ID_LUT, &IO_IIP.UNITINFTBL_LUT },
		{ E_IM_IIP_UNIT_ID_GPC, &IO_IIP.UNITINFTBL_GPC },
		{ E_IM_IIP_UNIT_ID_BLEND0, &IO_IIP.UNITINFTBL_BLD0 },
		{ E_IM_IIP_UNIT_ID_BLEND1, &IO_IIP.UNITINFTBL_BLD1 },
		{ E_IM_IIP_UNIT_ID_FRECT1, &IO_IIP.UNITINFTBL_FRC1 },
		{ E_IM_IIP_UNIT_ID_CFL0, &IO_IIP.UNITINFTBL_CFL0 },
		{ E_IM_IIP_UNIT_ID_CSC2, &IO_IIP.UNITINFTBL_CSC2 },
		{ E_IM_IIP_UNIT_ID_CSC3, &IO_IIP.UNITINFTBL_CSC3 },
		{ E_IM_IIP_UNIT_ID_SL6, &IO_IIP.UNITINFTBL_SL6 },
		{ E_IM_IIP_UNIT_ID_SL7, &IO_IIP.UNITINFTBL_SL7 },
		{ E_IM_IIP_UNIT_ID_SL8, &IO_IIP.UNITINFTBL_SL8 },
		{ E_IM_IIP_UNIT_ID_LD3, &IO_IIP.UNITINFTBL_LD3 },
		{ E_IM_IIP_UNIT_ID_LD4, &IO_IIP.UNITINFTBL_LD4 },
		{ E_IM_IIP_UNIT_ID_LD5, &IO_IIP.UNITINFTBL_LD5 },
		{ E_IM_IIP_UNIT_ID_LD6, &IO_IIP.UNITINFTBL_LD6 },
		{ E_IM_IIP_UNIT_ID_LD7, &IO_IIP.UNITINFTBL_LD7 },
		{ E_IM_IIP_UNIT_ID_MFT, &IO_IIP.UNITINFTBL_MFT },
	};

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	for( loopcnt = 0; loopcnt < (sizeof(padrs_tbl) / sizeof(T_CT_IM_IIP_2_1_3)); loopcnt++ ) {
		ercd = Im_IIP_Update_PADRS( padrs_tbl[loopcnt].unit_id, loopcnt *8 + 0x1000 );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

		Im_IIP_On_Pclk();
		if( padrs_tbl[loopcnt].uinftbl->UNITINFTBL2.bit.PADRS != (loopcnt *8 + 0x1000) ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "Error unit_id=%u\n", padrs_tbl[loopcnt].unit_id ));
		}
		Im_IIP_Off_Pclk();
	}

	return D_DDIM_OK;
}


// Im_IIP_Init, Im_IIP_Update_PADRS 2-1-X
static INT32 CT_Im_IIP_Run_2_1( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-2-1-1
		case 1:
			return CT_Im_IIP_2_1_1();

		// Test-2-1-2
		case 2:
			return CT_Im_IIP_2_1_2();

		// Test-2-1-3
		case 3:
			return CT_Im_IIP_2_1_3();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// Im_IIP_Stop 2-2-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_2_2_1: "
INT32 CT_Im_IIP_2_2_1( VOID )
{
	INT32						ercd;
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_0;		// AFN Unit input
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG			afn_cfg;
	T_IM_IIP_PARAM_AFN*			afn_unit_inf = &g_ct_im_iip_unit_param_afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	afn_tbl = &g_ct_im_iip_afntbl;
	T_IM_IIP_PARAM_STS*			sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG			sl_cfg;
	UINT32						wait_factor_result = 0;
	const UINT32				wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32				unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_AFN0 | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32				pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID		src_unitid = E_IM_IIP_UNIT_ID_AFN0;
	const E_IM_IIP_UNIT_ID		dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID	src_portid = E_IM_IIP_PARAM_PORTID_AFN0;
	const E_IM_IIP_PARAM_PORTID	dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32				src_pixid = 4;
	const UINT32				dst_pixid = 5;
	const UINT32				open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y;
	pixfmt_tbl_0.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_tbl = 0x%x 0x%x\n", (UINT32)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) ));

	memset( afn_tbl, '\0', sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	afn_tbl->STAXY0.bit.STAX = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->STAXY0.bit.STAY = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->D_OUT_IN_0.bit.DXX = M_CT_IM_IIP_AFNTBL_CALC_POS(1.0F);	// X increment X=1.0
	afn_tbl->D_OUT_IN_0.bit.DXY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// X increment Y=0.0
	afn_tbl->D_OUT_IN_0.bit.DYX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// Y increment X=0.0
	afn_tbl->D_OUT_IN_0.bit.DYY = M_CT_IM_IIP_AFNTBL_CALC_POS(1.0F);	// Y increment Y=1.0
	afn_tbl->SZ.bit.AFNDHSZ = D_IM_IIP_VGA_WIDTH;
	afn_tbl->SZ.bit.AFNDVSZ = D_IM_IIP_VGA_LINES;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_unit_inf = 0x%x 0x%x\n", (UINT32)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) ));
	Ddim_Print(( D_IM_IIP_FUNC_NAME "STAX=0x%x STAY=0x%x\n", (INT32)afn_tbl->STAXY0.bit.STAX, (INT32)afn_tbl->STAXY0.bit.STAY ));

	memset( afn_unit_inf, '\0', sizeof(*afn_unit_inf) );
	*afn_unit_inf = g_ct_im_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF = dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID = src_pixid;
	afn_unit_inf->AFNPA.word = (ULONG)afn_tbl;

	afn_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr = (ULONG)afn_unit_inf;
	afn_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &afn_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "AFN[0]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Stop( D_IM_IIP_STOP );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "WaitEnd\n" ));
	ercd = Im_IIP_Wait_End( &wait_factor_result, wait_factor, D_IM_IIP_OR_WAIT, 30 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));

	Im_IIP_On_Pclk();
	if( IO_IIP.IZACTL1.bit.TRG != 2 ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "Error\n" ));
	}
	Im_IIP_Off_Pclk();

	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "Stop\n" ));
		ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	}

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Im_IIP_Close_SWTRG\n" ));
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Stop 2-2-2
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_2_2_2: "
INT32 CT_Im_IIP_2_2_2( VOID )
{
	INT32						ercd;
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_0;		// AFN Unit input
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG			afn_cfg;
	T_IM_IIP_PARAM_AFN*			afn_unit_inf = &g_ct_im_iip_unit_param_afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	afn_tbl = &g_ct_im_iip_afntbl;
	T_IM_IIP_PARAM_STS*			sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG			sl_cfg;
	UINT32						wait_factor_result = 0;
	const UINT32				wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32				unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_AFN0 | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32				pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID		src_unitid = E_IM_IIP_UNIT_ID_AFN0;
	const E_IM_IIP_UNIT_ID		dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID	src_portid = E_IM_IIP_PARAM_PORTID_AFN0;
	const E_IM_IIP_PARAM_PORTID	dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32				src_pixid = 4;
	const UINT32				dst_pixid = 5;
	const UINT32				open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y;
	pixfmt_tbl_0.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_tbl = 0x%x 0x%x\n", (UINT32)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) ));

	memset( afn_tbl, '\0', sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	afn_tbl->STAXY0.bit.STAX = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->STAXY0.bit.STAY = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->D_OUT_IN_0.bit.DXX = M_CT_IM_IIP_AFNTBL_CALC_POS(1.0F);	// X increment X=1.0
	afn_tbl->D_OUT_IN_0.bit.DXY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// X increment Y=0.0
	afn_tbl->D_OUT_IN_0.bit.DYX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// Y increment X=0.0
	afn_tbl->D_OUT_IN_0.bit.DYY = M_CT_IM_IIP_AFNTBL_CALC_POS(1.0F);	// Y increment Y=1.0
	afn_tbl->SZ.bit.AFNDHSZ = D_IM_IIP_VGA_WIDTH;
	afn_tbl->SZ.bit.AFNDVSZ = D_IM_IIP_VGA_LINES;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_unit_inf = 0x%x 0x%x\n", (UINT32)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) ));
	Ddim_Print(( D_IM_IIP_FUNC_NAME "STAX=0x%x STAY=0x%x\n", (INT32)afn_tbl->STAXY0.bit.STAX, (INT32)afn_tbl->STAXY0.bit.STAY ));

	memset( afn_unit_inf, '\0', sizeof(*afn_unit_inf) );
	*afn_unit_inf = g_ct_im_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF = dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID = src_pixid;
	afn_unit_inf->AFNPA.word = (ULONG)afn_tbl;

	afn_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr = (ULONG)afn_unit_inf;
	afn_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &afn_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "AFN[0]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "WaitEnd\n" ));
	ercd = Im_IIP_Wait_End( &wait_factor_result, wait_factor, D_IM_IIP_OR_WAIT, 30 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));

	Im_IIP_On_Pclk();
	if( IO_IIP.IZACTL1.bit.TRG != 2 ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "Error\n" ));
	}
	Im_IIP_Off_Pclk();

	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "Stop\n" ));
		ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	}

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Im_IIP_Close_SWTRG\n" ));
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Stop 2-2-X
static INT32 CT_Im_IIP_Run_2_2( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-2-2-1
		case 1:
			return CT_Im_IIP_2_2_1();

		// Test-2-2-2
		case 2:
			return CT_Im_IIP_2_2_2();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// Im_IIP_Start_SWTRG 2-4-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_2_4_1: "
INT32 CT_Im_IIP_2_4_1( VOID )
{
	INT32						ercd;
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_0;		// AFN Unit input
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG			afn_cfg;
	T_IM_IIP_PARAM_AFN*			afn_unit_inf = &g_ct_im_iip_unit_param_afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	afn_tbl = &g_ct_im_iip_afntbl;
	T_IM_IIP_PARAM_STS*			sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG			sl_cfg;
	UINT32						wait_factor_result = 0;
	const UINT32				wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL5END;
	const UINT32				unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_AFN2 | D_IM_IIP_PARAM_PLDUNIT_SL5;
	const UINT32				pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID		src_unitid = E_IM_IIP_UNIT_ID_AFN2;
	const E_IM_IIP_UNIT_ID		dst_unitid = E_IM_IIP_UNIT_ID_SL5;
	const E_IM_IIP_PARAM_PORTID	src_portid = E_IM_IIP_PARAM_PORTID_AFN2;
	const E_IM_IIP_PARAM_PORTID	dst_portid = E_IM_IIP_PARAM_PORTID_SL5;
	const UINT32				src_pixid = 4;
	const UINT32				dst_pixid = 5;
	const UINT32				open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y;
	pixfmt_tbl_0.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_tbl = 0x%x 0x%x\n", (UINT32)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) ));

	memset( afn_tbl, '\0', sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	afn_tbl->STAXY0.bit.STAX = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->STAXY0.bit.STAY = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->D_OUT_IN_0.bit.DXX = M_CT_IM_IIP_AFNTBL_CALC_POS(1.0F);	// X increment X=1.0
	afn_tbl->D_OUT_IN_0.bit.DXY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// X increment Y=0.0
	afn_tbl->D_OUT_IN_0.bit.DYX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// Y increment X=0.0
	afn_tbl->D_OUT_IN_0.bit.DYY = M_CT_IM_IIP_AFNTBL_CALC_POS(1.0F);	// Y increment Y=1.0
	afn_tbl->SZ.bit.AFNDHSZ = D_IM_IIP_VGA_WIDTH;
	afn_tbl->SZ.bit.AFNDVSZ = D_IM_IIP_VGA_LINES;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_unit_inf = 0x%x 0x%x\n", (UINT32)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) ));
	Ddim_Print(( D_IM_IIP_FUNC_NAME "STAX=0x%x STAY=0x%x\n", (INT32)afn_tbl->STAXY0.bit.STAX, (INT32)afn_tbl->STAXY0.bit.STAY ));

	memset( afn_unit_inf, '\0', sizeof(*afn_unit_inf) );
	*afn_unit_inf = g_ct_im_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF = dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID = src_pixid;
	afn_unit_inf->AFNPA.word = (ULONG)afn_tbl;

	afn_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr = (ULONG)afn_unit_inf;
	afn_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &afn_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "AFN[0]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "WaitEnd\n" ));
	ercd = Im_IIP_Wait_End( &wait_factor_result, wait_factor, D_IM_IIP_OR_WAIT, 30 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "Stop\n" ));
		ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	}

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Im_IIP_Close_SWTRG\n" ));
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Stop 2-4-X
static INT32 CT_Im_IIP_Run_2_4( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-2-4-1
		case 1:
			return CT_Im_IIP_2_4_1();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// UNIT register dump 2-6-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_2_6_1: "
INT32 CT_Im_IIP_2_6_1( VOID )
{
	ULONG*	unitdebug;
	ULONG*	sdram;
#ifndef CO_DEBUG_ON_PC
	UINT32	loopcnt;
#endif
	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	Im_IIP_Start_DumpUnitReg_ByName( "afn0" );

	unitdebug = (ULONG*)&IO_IIP.UNITDEBUG;
	sdram = (ULONG*)D_IM_IIP_IMG_MEM_ADDR_1_TOP;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "unitdebug = 0x%lx\n", (ULONG)unitdebug ));
	Ddim_Print(( D_IM_IIP_FUNC_NAME "sdram = 0x%lx\n", (ULONG)sdram ));

	Im_IIP_On_Pclk();
#ifndef CO_DEBUG_ON_PC
	for( loopcnt = 0; loopcnt < (65536 / sizeof(*sdram)); loopcnt++ ) {
		sdram[loopcnt] = unitdebug[loopcnt];
	}
#endif
	Im_IIP_Off_Pclk();

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sdram, 65536 );

	return D_DDIM_OK;
}


// UNIT register dump 2-6-x
static INT32 CT_Im_IIP_Run_2_6( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-2-6-1
		case 1:
			return CT_Im_IIP_2_6_1();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// Im_IIP_Clear_HISTGRAM 2-7-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_2_7_1: "
INT32 CT_Im_IIP_2_7_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;		// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf = &g_ct_im_iip_unit_param_1dl0;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	T_IM_IIP_PARAM_MON*				mon_unit_inf = &g_ct_im_iip_unit_param_mon;
	T_IM_IIP_UNIT_CFG				mon_cfg;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL4END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_SL4 | D_IM_IIP_PARAM_PLDUNIT_MON;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID			src_unitid = E_IM_IIP_UNIT_ID_LD1;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL4;
	const E_IM_IIP_UNIT_ID			mon_unitid = E_IM_IIP_UNIT_ID_MON;
	const E_IM_IIP_PARAM_PORTID		src_portid = E_IM_IIP_PARAM_PORTID_LD1;
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL4;
	const UINT32					src_pixid = 4;
	const UINT32					dst_pixid = 5;
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0 | E_IM_IIP_OPEN_RES_HIST_0 | E_IM_IIP_OPEN_RES_HIST_1 | E_IM_IIP_OPEN_RES_HIST_2;
	T_IM_IIP_MAXMON					maxmon;
	T_IM_IIP_MINMON					minmon;
	T_IM_IIP_ADDMON					addmon;
	UINT32							loopcnt;
#ifndef CO_CT_IM_IIP_2_7_1_DISABLE_HIST
	static ULONG					buffer_histgram[D_IM_IIP_HIST_MAXBYTES / sizeof(ULONG)] = { 0ul };
	UCHAR							histmax;
#endif


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y;
	pixfmt_tbl_0.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A;
	pixfmt_tbl_0.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;
	pixfmt_tbl_1.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf = 0x%x 0x%x\n", (UINT32)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf, '\0', sizeof(*oned_unit_inf) );
	*oned_unit_inf = g_ct_im_iip_param_1dl_base;
	oned_unit_inf->LD_TOPCNF0.bit.WAITCONF = dst_portid;
	oned_unit_inf->PIXIDDEF.bit.IPIXID = src_pixid;

	oned_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg.unit_param_addr = (ULONG)oned_unit_inf;
	oned_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "mon_unit_inf = 0x%x 0x%x\n", (UINT32)mon_unit_inf, sizeof(T_IM_IIP_PARAM_MON) ));

	memset( mon_unit_inf, '\0', sizeof(*mon_unit_inf) );
	*mon_unit_inf = g_ct_im_iip_param_mon_base;
	mon_unit_inf->MONDSEL.bit.DSEL = E_IM_IIP_PARAM_DSEL_SL4;
	mon_unit_inf->HISTCTL[0].bit.BITSEL = E_IM_IIP_PARAM_BITSEL_7_0;
	mon_unit_inf->HISTCTL[1].bit.BITSEL = E_IM_IIP_PARAM_BITSEL_7_0;
	mon_unit_inf->HISTCTL[2].bit.BITSEL = E_IM_IIP_PARAM_BITSEL_7_0;
	mon_unit_inf->HISTCTL[3].bit.BITSEL = E_IM_IIP_PARAM_BITSEL_7_0;


	mon_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	mon_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	mon_cfg.unit_param_addr = (ULONG)mon_unit_inf;
	mon_cfg.load_unit_param_flag = 0;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)mon_unit_inf, sizeof(T_IM_IIP_PARAM_MON) );

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &oned_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( mon_unitid, &mon_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Clear_HISTGRAM( 1, 1, 1, 1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "ONED[1]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_LD1.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_LD1.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( mon_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start3\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));


	Ddim_Print(( D_IM_IIP_FUNC_NAME "WaitEnd\n" ));
	ercd = Im_IIP_Wait_End( &wait_factor_result, wait_factor, D_IM_IIP_OR_WAIT, 30 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "Abort\n" ));
		ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	}

	ercd = Im_IIP_Clear_HISTGRAM( 1, 1, 1, 1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

#ifndef CO_CT_IM_IIP_2_7_1_DISABLE_HIST
	Ddim_Print(( D_IM_IIP_FUNC_NAME "** hist_a\n" ));
	ercd = Im_IIP_Get_HISTGRAM( buffer_histgram, NULL, NULL, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Get_HISTMAX( &histmax, NULL, NULL, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)buffer_histgram, D_IM_IIP_HIST_MAXBYTES );
	Palladium_Set_Out_Localstack( (ULONG)buffer_histgram, D_IM_IIP_HIST_MAXBYTES );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "histmax = %u\n", histmax ));
#else
// ### REMOVE_RELEASE END
	CT_Im_IIP_Plot_Hist( buffer_histgram, (D_IM_IIP_HIST_MAXBYTES / sizeof(ULONG)), histmax );
	DDIM_User_Dly_Tsk( 500 );		// Wait for UART output.
// ### REMOVE_RELEASE BEGIN
#endif
// ### REMOVE_RELEASE END


	Ddim_Print(( D_IM_IIP_FUNC_NAME "** hist_b\n" ));
	ercd = Im_IIP_Get_HISTGRAM( NULL, buffer_histgram, NULL, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Get_HISTMAX( NULL, &histmax, NULL, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)buffer_histgram, D_IM_IIP_HIST_MAXBYTES );
	Palladium_Set_Out_Localstack( (ULONG)buffer_histgram, D_IM_IIP_HIST_MAXBYTES );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "histmax = %u\n", histmax ));
#else
// ### REMOVE_RELEASE END
	CT_Im_IIP_Plot_Hist( buffer_histgram, (D_IM_IIP_HIST_MAXBYTES / sizeof(ULONG)), histmax );
	DDIM_User_Dly_Tsk( 500 );		// Wait for UART output.
// ### REMOVE_RELEASE BEGIN
#endif
// ### REMOVE_RELEASE END


	Ddim_Print(( D_IM_IIP_FUNC_NAME "** hist_c\n" ));
	ercd = Im_IIP_Get_HISTGRAM( NULL, NULL, buffer_histgram, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Get_HISTMAX( NULL, NULL, &histmax, NULL );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)buffer_histgram, D_IM_IIP_HIST_MAXBYTES );
	Palladium_Set_Out_Localstack( (ULONG)buffer_histgram, D_IM_IIP_HIST_MAXBYTES );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "histmax = %u\n", histmax ));
#else
// ### REMOVE_RELEASE END
	CT_Im_IIP_Plot_Hist( buffer_histgram, (D_IM_IIP_HIST_MAXBYTES / sizeof(ULONG)), histmax );
	DDIM_User_Dly_Tsk( 500 );		// Wait for UART output.
// ### REMOVE_RELEASE BEGIN
#endif
// ### REMOVE_RELEASE END


	Ddim_Print(( D_IM_IIP_FUNC_NAME "** hist_d\n" ));
	ercd = Im_IIP_Get_HISTGRAM( NULL, NULL, NULL, buffer_histgram );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Get_HISTMAX( NULL, NULL, NULL, &histmax );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)buffer_histgram, D_IM_IIP_HIST_MAXBYTES );
	Palladium_Set_Out_Localstack( (ULONG)buffer_histgram, D_IM_IIP_HIST_MAXBYTES );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "histmax = %u\n", histmax ));
#else
// ### REMOVE_RELEASE END
	CT_Im_IIP_Plot_Hist( buffer_histgram, (D_IM_IIP_HIST_MAXBYTES / sizeof(ULONG)), histmax );
// ### REMOVE_RELEASE BEGIN
#endif
// ### REMOVE_RELEASE END
#endif

	ercd = Im_IIP_Get_MAXMON( &maxmon );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Get_MINMON( &minmon );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Get_ADDMON( &addmon );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	for( loopcnt = 0; loopcnt < 4; loopcnt++ ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "max_mon[%u]:x=%u y=%u V=%d\n",
					loopcnt,
					maxmon.max[loopcnt].pos_x,
					maxmon.max[loopcnt].pos_y,
					maxmon.max[loopcnt].value
					));
		Ddim_Print(( D_IM_IIP_FUNC_NAME "min_mon[%u]:x=%u y=%u V=%d\n",
					loopcnt,
					minmon.min[loopcnt].pos_x,
					minmon.min[loopcnt].pos_y,
					minmon.min[loopcnt].value
					));
		Ddim_Print(( D_IM_IIP_FUNC_NAME "add_mon[%u]:x=%u y=%u V=%d\n",
					loopcnt,
					addmon.add[loopcnt].pos_x,
					addmon.add[loopcnt].pos_y,
					addmon.add[loopcnt].value
					));
	}

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Im_IIP_Close_SWTRG\n" ));
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Clear_HISTGRAM 2-7-x
static INT32 CT_Im_IIP_Run_2_7( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-2-7-1
		case 1:
			return CT_Im_IIP_2_7_1();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


VOID CT_Im_IIP_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
#ifdef CO_MSG_PRINT_ON
	INT32 result = D_DDIM_INPUT_PARAM_ERROR;
#endif
	static BOOL is_init = 0;
	T_IM_IIP_LUT lut;
	ULONG hist[ 4 ];
	INT32 ercd;

	Ddim_Print(( "CT_Im_IIP_Run( %u, %u, %u ) Begin\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	if( is_init == 0 ) {
#ifdef CO_DEBUG_ON_PC
		memset( (VOID*)&IO_IIP, '\0', sizeof(IO_IIP) );
		IO_IIP.IZACTL1.bit.TRG = 2;
		IO_IIP.IZACTL2.bit.ABORT = 2;
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
		Dd_Top_Set_CLKSTOP3_IIPAP( 1 );
		Dd_Top_Set_CLKSTOP3_IIPAH( 1 );
		Dd_Top_Set_CLKSTOP3_IIPAX( 1 );
		Dd_Top_Set_CLKSTOP3_IIPCK( 1 );
#endif
#endif // CO_DEBUG_ON_PC

		if( !((ct_idx_1st == 1) && (ct_idx_2nd == 1) && (ct_idx_3rd == 1)) ) {
			Ddim_Print(( "Im_IIP_Init() Begin\n" ));
			ercd = Im_IIP_Init();
			if( ercd != D_DDIM_USER_E_OK ) {
				Ddim_Print(( "Im_IIP_Init() Error ercd=0x%x\n", ercd ));
			}
			else {
				is_init = 1;
			}
			Ddim_Print(( "Im_IIP_Init() End\n" ));
		}
	}

#ifdef CO_IIP_DEBUG
	Im_IIP_Print_ClockStatus();
#endif

	switch( ct_idx_1st ) {
		case 0:
			switch( ct_idx_2nd ) {
				case 0:
					CT_Im_IIP_Print_Pixfmttbl_Detail( ct_idx_3rd );
					break;

#ifdef CO_IM_IIP_UTIL_DEBUG
				case 1:
					{
						extern VOID IM_IIP_Utility_PrintParamBufferBytes( VOID );
						IM_IIP_Utility_PrintParamBufferBytes();
					}
					break;
#endif //CO_IM_IIP_UTIL_DEBUG

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 1:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_1_1( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 2:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_2_1( ct_idx_3rd ) );
					break;

				case 2:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_2_2( ct_idx_3rd ) );
					break;

				case 4:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_2_4( ct_idx_3rd ) );
					break;

#if 0
				case 5:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_2_5( ct_idx_3rd ) );
					break;
#endif

				case 6:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_2_6( ct_idx_3rd ) );
					break;

				case 7:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_2_7( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 3:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_3_1( ct_idx_3rd ) );
					break;

				case 2:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_3_2( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 4:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_4_1( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 5:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_5_1( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 6:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_6_1( ct_idx_3rd ) );
					break;

				case 2:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_6_2( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 7:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_7_1( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 8:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_8_1( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 9:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_9_1( ct_idx_3rd ) );
					break;

				case 2:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_9_2( ct_idx_3rd ) );
					break;

				case 3:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_9_3( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 10:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_10_1( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 11:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_11_1( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 12:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_12_1( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 13:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_13_1( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 14:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_14_1( ct_idx_3rd ) );
					break;

				case 2:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_14_2( ct_idx_3rd ) );
					break;

				case 3:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_14_3( ct_idx_3rd ) );
					break;

				case 4:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_14_4( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		case 15:
			switch( ct_idx_2nd ) {
				case 1:
					D_IM_IIP_RESULT( CT_Im_IIP_Run_15_1( ct_idx_3rd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		// for Jpeg isse workaround.
		case 99:
			switch( ct_idx_2nd ) {
				case 1:
					Im_IIP_Ctrl_LUT( &lut );
					break;
				case 2:
					Im_IIP_Clear_HISTGRAM( 0, 0, 0, 0 );
					break;
				case 3:
					Im_IIP_Get_HISTGRAM( &hist[ 0 ], &hist[ 1 ], &hist[ 2 ], &hist[ 3 ] );
					break;
				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}


#ifdef CO_MSG_PRINT_ON
	Ddim_Print(( "result = 0x%x\n", result ));
#endif

#ifdef CO_IIP_DEBUG
	Im_IIP_Print_ClockStatus();
#endif

	Ddim_Print(( "CT_Im_IIP_Run( %u, %u, %u ) End\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	return;
}


// for Debug console interface
VOID CT_Im_IIP_Main( INT32 argc, CHAR** argv )
{
	if( argc < 4 ) {
		Ddim_Print(( "Ct_Im_IIP_Main: parameter error.\n" ));
		return;
	}

	CT_Im_IIP_Run( (UINT32)atoi((const CHAR*)argv[1]),
				   (UINT32)atoi((const CHAR*)argv[2]),
				   (UINT32)atoi((const CHAR*)argv[3]) );
}

#else //CO_CT_IM_IIP_DISABLE

VOID CT_Im_IIP_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{

	switch( ct_idx_1st ) {
		case 0:
			switch( ct_idx_2nd ) {
				case 0:
					CT_Im_IIP_Print_Pixfmttbl_Detail( ct_idx_3rd );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}
			break;

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}

	return;
}


VOID CT_Im_IIP_Main( INT32 argc, CHAR** argv )
{
}

#endif //CO_CT_IM_IIP_DISABLE

