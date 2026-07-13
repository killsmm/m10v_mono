/**
 * @file		ct_im_iip_gpc.c
 * @brief		IIP GPC unit ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// CT header
#include "ct_im_iip.h"
#include "ct_im_iip_gpc.h"

// im_iip header
#include "im_iip.h"

// MILB register header
#include "jdsiip.h"

// for Memory barrier
#include "dd_arm.h"

// for memcpy
#include <string.h>


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#ifndef CO_CT_IM_IIP_DISABLE

#if 1
#include <stdio.h>
#undef Ddim_Print
#define Ddim_Print(arg)		{DDIM_User_Printf arg;}		/**< printf function inside DDIM */
#endif



/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

// GPC Unit Parameter
typedef struct{
	T_IM_IIP_PARAM_GPC_SEQ_CTRL			seq;
	T_IM_IIP_PARAM_GPC_PRECAL			precal[2];
	T_IM_IIP_PARAM_GPC_CALC				calc;			// D0~D2
	T_IM_IIP_PARAM_GPC_ALPHA			alpha;			// D3(alpha)
	T_IM_IIP_PARAM_GPC_INTER_COMP_CALC	inter_comp_calc;
} T_IM_IZA_PARAM_GPC_CONFIG;

// GPC Unit Parameter
typedef struct{
	T_IM_IIP_PARAM_GPC_SEQ_CTRL			seq;
	T_IM_IIP_PARAM_GPC_PRECAL			precal[2];
	T_IM_IIP_PARAM_GPC_CALC				calc[3];		// D0~D2
	T_IM_IIP_PARAM_GPC_ALPHA			alpha;			// D3(alpha)
	T_IM_IIP_PARAM_GPC_INTER_COMP_CALC	inter_comp_calc;
} T_IM_IZA_PARAM_GPC_CONFIG_13_1_2;

typedef struct{
	T_IM_IIP_PARAM_GPC_SEQ_CTRL			seq;
	T_IM_IIP_PARAM_GPC_PRECAL			precal_p0_d0_d1;			// Input port0 D0-D1
	T_IM_IIP_PARAM_GPC_PRECAL			precal_p0_d2_d3;			// Input port0 D2-D3
	T_IM_IIP_PARAM_GPC_PRECAL			precal_p1_d0_d1;			// Input port1 D0-D1
	T_IM_IIP_PARAM_GPC_PRECAL			precal_p1_d2_d3;			// Input port1 D2-D3
	T_IM_IIP_PARAM_GPC_CALC				calc;						// D0~D2
	T_IM_IIP_PARAM_GPC_INTER_COMP_CALC	inter_comp_calc;
} T_IM_IZA_PARAM_GPC_CONFIG_13_1_3;


typedef struct {
	E_IM_IIP_PFMT			pfmt;
	E_IM_IIP_FTYPE			ffmt;
	USHORT					g_width;
	USHORT					width;
	USHORT					lines;
	ULONG					addr_y;
	ULONG					addr_c;
} T_CT_IIP_GPC_IMG;

typedef struct {
	USHORT					sta_x;
	USHORT					sta_y;
	T_CT_IIP_GPC_IMG		src_a;
	T_CT_IIP_GPC_IMG		src_b;
	T_CT_IIP_GPC_IMG		dst;
	ULONG					alpha_addr;
} T_CT_IIP_GPC_BLEND;

/*----------------------------------------------------------------------*/
/* Local Method															*/
/*----------------------------------------------------------------------*/

static const T_IM_IIP_PARAM_GPC g_ct_im_iip_param_gpc_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 7,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
//	.GPCTOPCNF[0].bit.WAITCONF = ,		// set later
//	.GPCTOPCNF[0].bit.DATACONF = ,		// set later
//	.GPCTOPCNF[1].bit.WAITCONF = ,		// set later
//	.GPCTOPCNF[1].bit.DATACONF = ,		// set later
	.GPCCONTROL.bit.COMMONPARA = E_IM_IIP_PARAM_COMMONPARA_D0_D0_D0,
	.GPCCONTROL.bit.MD2DLK = 0,			// SLINK mode
	.GPCCONTROL.bit.ALPEN0 = 0,			// No alpha plane
	.GPCCONTROL.bit.ALPEN1 = 0,			// No alpha plane
	.GPCCONTROL.bit.ATRB0 = E_IM_IIP_PARAM_ATRB_PORT0,
	.GPCCONTROL.bit.ATRB1 = E_IM_IIP_PARAM_ATRB_PORT0,
};

#ifdef __GNUC__
static T_IM_IIP_PARAM_GPC			g_ct_im_iip_unit_param_gpc __attribute__((aligned(8)));			// Needs 8bytes align
#else
static __align(8) T_IM_IIP_PARAM_GPC			g_ct_im_iip_unit_param_gpc;								// Needs 8bytes align
#endif

#define D_GPC_STEPNUM		(5)			// GeneralCalculator0(4) + Register9(1) = 5
#ifdef __GNUC__
static T_IM_IZA_PARAM_GPC_CONFIG		g_ct_im_iip_gpc_config __attribute__((aligned(64))) = {		// Needs 64bytes align
#else
static __align(64) T_IM_IZA_PARAM_GPC_CONFIG		g_ct_im_iip_gpc_config = {							// Needs 64bytes align
#endif
	.seq = {
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_SEQ_CTRL,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.SEQLOOP = 1,
		.SEQCTL.bit = {
			.STEPNUM = D_GPC_STEPNUM,

			.CNTEND_0 = 640 * 480,
			.SEQCONF_0 = 0,		// Use parameter 0.
			.CNFOUT_0 = 1,		// Enable output

			.CNTEND_1 = 0,		// dummy(invalid)
			.SEQCONF_1 = 0,		// dummy(invalid)
			.CNFOUT_1 = 0,		// Disable output

			.CNTEND_2 = 0,		// dummy(invalid)
			.SEQCONF_2 = 0,		// dummy(invalid)
			.CNFOUT_2 = 0,		// Disable output

			.CNTEND_3 = 0,		// dummy(invalid)
			.SEQCONF_3 = 0,		// dummy(invalid)
			.CNFOUT_3 = 0,		// Disable output
		},
	},
	.precal[0] = {	// D0, D1
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_PRECAL_P0_D0_1,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.PRECAL[0].bit = {	// D0
			.PREOFSET = 0,
			.PRECLIPMAX = 0x3fff,
			.PRECTHMAX = 0x3fff,
			.PRECLIPMIN = 0x4000,
			.PRECTHMIN = 0x4000,
			.PREAE = 0,
		},
		.PRECAL[1].bit = {	// D1
			.PREOFSET = 0,
			.PRECLIPMAX = 0xff,
			.PRECTHMAX = 0xff,
			.PRECLIPMIN = 0x100,
			.PRECTHMIN = 0x100,
			.PREAE = 0,
		},
	},
	.precal[1] = {	// D2, D3
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_PRECAL_P0_D2_3,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.PRECAL[0].bit = {	// D2
			.PREOFSET = 0,
			.PRECLIPMAX = 0xff,
			.PRECTHMAX = 0xff,
			.PRECLIPMIN = 0x100,
			.PRECTHMIN = 0x100,
			.PREAE = 0,
		},
		.PRECAL[1].bit = {	// D3
			.PREOFSET = 0,
			.PRECLIPMAX = 0xff,
			.PRECTHMAX = 0xff,
			.PRECLIPMIN = 0x100,
			.PRECTHMIN = 0x100,
			.PREAE = 0,
		},
	},
	.calc = {
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_CALC_D0_CFG0,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.GPC0PARA0.bit = {
			.GPC0SEL0 = E_IM_IIP_PARAM_GPCSEL_P0D0,
			.GPC0SEL1 = E_IM_IIP_PARAM_GPCSEL_CFG0,		// dummy
			.GPC0SEL2 = E_IM_IIP_PARAM_GPCSEL_P1D0,
			.GPC0SEL3 = E_IM_IIP_PARAM_GPCSEL_CFG1,		// dummy
			.G0MD0 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,
			.G0MD1 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,
			.G0MD2 = E_IM_IIP_PARAM_GMD_BIN_ADD,
		},
		.GPC0PARA1.bit = {
			.G0RND_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_0 = 0,
			.G0RND_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_1 = 0,
			.G0RND_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_2 = 0,
			.G0RND_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_3 = 0,
			.G0RND_4 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_4 = 0,
			.G0RND_5 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_5 = 0,
			.G0RND_6 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_6 = 0,
		},
		.GPC1PARA0.bit = {
			.GPC1SEL0 = E_IM_IIP_PARAM_GPCSEL_P0D0,		// dummy
			.GPC1SEL1 = E_IM_IIP_PARAM_GPCSEL_P1D0,		// dummy
			.G1MD0 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,	// dummy
			.GPC1SEL2 = E_IM_IIP_PARAM_GPCSEL_CFG1,		// dummy
			.G1MD1 = E_IM_IIP_PARAM_GMD_BIN_THROUGH,	// dummy
		},
		.GPC1PARA1.bit = {
			.G1RND_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_0 = 0,
			.G1RND_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_1 = 0,
			.G1RND_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_2 = 0,
			.G1RND_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_3 = 0,
			.G1RND_4 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_4 = 0,
		},
		.SERPASS.bit = {
			.SEDLY0 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SESEL0 = E_IM_IIP_PARAM_SESEL0_INPUT_SELECTOR,
			.SEDLY1 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SESEL1 = E_IM_IIP_PARAM_SESEL0_INPUT_SELECTOR,
			.SEDLY2 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY3 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY4 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY5 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY6 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY7 = E_IM_IIP_PARAM_SEDLY_0_STEP,
		},
		.COMPSEL.bit = {
			.COMPSEL = E_IM_IIP_PARAM_COMPSEL_CFG0,
			.CMPRND = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.CMPSFT = 0,
			.CORPEN = 0,
			.CORMEN = 0,
			.CLPPEN = 0,
			.CLPMEN = 0,
		},
		.CONFCNT.bit = {
			.CONFWEN0 = 0,
			.CONFWEN1 = 0,
			.CONFWEN2 = 0,
			.CONFWEN3 = 0,
			.CONFREGSEL0 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL1 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL2 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL3 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL4 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL5 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL6 = E_IM_IIP_PARAM_CONFREGSEL_CALC0,		// Port1 output = calculator0 value
			.CONFREGSEL7 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL8 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL9 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,		// Port0 output = fixed value
		},
		.CONFREG = {
			0,
			0,
			0,
			0,
		},
	},
	.alpha = {
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_ALPHA,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.GPCALP = {
			[0].bit = {
				.GPCALPFIX = 0,
				.GPCALPSEL0 = E_IM_IIP_PARAM_GPCALPSEL_P0D3,
				.GPCALPSEL1 = E_IM_IIP_PARAM_GPCALPSEL_P1D3,
				.GPCALPMODE = E_IM_IIP_PARAM_GPCALPMODE_THROUGH,
			},
			[1].bit = {
				.GPCALPFIX = 0,
				.GPCALPSEL0 = E_IM_IIP_PARAM_GPCALPSEL_P0D3,
				.GPCALPSEL1 = E_IM_IIP_PARAM_GPCALPSEL_P1D3,
				.GPCALPMODE = E_IM_IIP_PARAM_GPCALPMODE_THROUGH,
			},
			[2].bit = {
				.GPCALPFIX = 0,
				.GPCALPSEL0 = E_IM_IIP_PARAM_GPCALPSEL_P0D3,
				.GPCALPSEL1 = E_IM_IIP_PARAM_GPCALPSEL_P1D3,
				.GPCALPMODE = E_IM_IIP_PARAM_GPCALPMODE_THROUGH,
			},
			[3].bit = {
				.GPCALPFIX = 0,
				.GPCALPSEL0 = E_IM_IIP_PARAM_GPCALPSEL_P0D3,
				.GPCALPSEL1 = E_IM_IIP_PARAM_GPCALPSEL_P1D3,
				.GPCALPMODE = E_IM_IIP_PARAM_GPCALPMODE_THROUGH,
			},
		},
	},
	.inter_comp_calc = {
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_INTER_COMP_CALC,
		.PARACNT.bit.SEQEN = 1,		// Sequence start.
		.PARACNT.bit.LPARA = 1,		// this parameter is last.
		.P1OUTMD.bit = {
			.P1RNDO_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_0 = 0,
			.P1SELO_0 = E_IM_IIP_PARAM_P1SELO_CONF6_D0,
			.P1RNDO_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_1 = 0,
			.P1SELO_1 = E_IM_IIP_PARAM_P1SELO_CONF6_D1,
			.P1RNDO_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_2 = 0,
			.P1SELO_2 = E_IM_IIP_PARAM_P1SELO_CONF6_D2,
			.P1RNDO_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_3 = 0,
			.P1SELO_3 = E_IM_IIP_PARAM_P1SELO_ALPHA,
		},
		.P0OUTMD_D3.bit = {
			.P0RNDO_D3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P0SFTO_D3 = 0,
			.P0SELO_D3 = E_IM_IIP_PARAM_P0SELO_ALPHA,
		},
		.ELECAL = {
			[0].bit = {
				.ELESEL0 = E_IM_IIP_PARAM_ELESEL_D0_CFG9,
				.ELERD0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT0 = 0,
				.ELESEL1 = E_IM_IIP_PARAM_ELESEL_D1_CFG9,
				.ELERD1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT1 = 0,
				.ELESEL2 = E_IM_IIP_PARAM_ELESEL_D2_CFG9,
				.ELERD2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT2 = 0,
				.ELESEL3 = E_IM_IIP_PARAM_ELESEL_D3_ALPHA,
				.ELERD3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT3 = 0,
				.ELEMODE0 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE1 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE2 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELERDO = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFTO = 0,
			},
			[1].bit = {
				.ELESEL0 = E_IM_IIP_PARAM_ELESEL_D1_CFG9,
				.ELERD0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT0 = 0,
				.ELESEL1 = E_IM_IIP_PARAM_ELESEL_D2_CFG9,
				.ELERD1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT1 = 0,
				.ELESEL2 = E_IM_IIP_PARAM_ELESEL_D3_ALPHA,
				.ELERD2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT2 = 0,
				.ELESEL3 = E_IM_IIP_PARAM_ELESEL_D0_CFG9,
				.ELERD3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT3 = 0,
				.ELEMODE0 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE1 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE2 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELERDO = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFTO = 0,
			},
			[2].bit = {
				.ELESEL0 = E_IM_IIP_PARAM_ELESEL_D2_CFG9,
				.ELERD0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT0 = 0,
				.ELESEL1 = E_IM_IIP_PARAM_ELESEL_D3_ALPHA,
				.ELERD1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT1 = 0,
				.ELESEL2 = E_IM_IIP_PARAM_ELESEL_D0_CFG9,
				.ELERD2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT2 = 0,
				.ELESEL3 = E_IM_IIP_PARAM_ELESEL_D1_CFG9,
				.ELERD3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT3 = 0,
				.ELEMODE0 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE1 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE2 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELERDO = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFTO = 0,
			},
		},
	},
};

#ifdef __GNUC__
static T_IM_IZA_PARAM_GPC_CONFIG_13_1_2 g_ct_im_iip_gpc_config_13_1_2 __attribute__((aligned(64))) = {		// Needs 64bytes align
#else
static __align(64) T_IM_IZA_PARAM_GPC_CONFIG_13_1_2 g_ct_im_iip_gpc_config_13_1_2 = {					// Needs 64bytes align
#endif
	.seq = {
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_SEQ_CTRL,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.SEQLOOP = 1,
		.SEQCTL.bit = {
			.STEPNUM = D_GPC_STEPNUM,

			.CNTEND_0 = 640 * 480,
			.SEQCONF_0 = 0,		// Use parameter 0.
			.CNFOUT_0 = 1,		// Enable output

			.CNTEND_1 = 0,		// dummy(invalid)
			.SEQCONF_1 = 0,		// dummy(invalid)
			.CNFOUT_1 = 0,		// Disable output

			.CNTEND_2 = 0,		// dummy(invalid)
			.SEQCONF_2 = 0,		// dummy(invalid)
			.CNFOUT_2 = 0,		// Disable output

			.CNTEND_3 = 0,		// dummy(invalid)
			.SEQCONF_3 = 0,		// dummy(invalid)
			.CNFOUT_3 = 0,		// Disable output
		},
	},
	.precal[0] = {	// D0, D1
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_PRECAL_P0_D0_1,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.PRECAL[0].bit = {	// D0
			.PREOFSET = 0,
			.PRECLIPMAX = 0x3fff,
			.PRECTHMAX = 0x3fff,
			.PRECLIPMIN = 0x4000,
			.PRECTHMIN = 0x4000,
			.PREAE = 0,
		},
		.PRECAL[1].bit = {	// D1
			.PREOFSET = 0,
			.PRECLIPMAX = 0xff,
			.PRECTHMAX = 0xff,
			.PRECLIPMIN = 0x100,
			.PRECTHMIN = 0x100,
			.PREAE = 0,
		},
	},
	.precal[1] = {	// D2, D3
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_PRECAL_P0_D2_3,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.PRECAL[0].bit = {	// D2
			.PREOFSET = 0,
			.PRECLIPMAX = 0xff,
			.PRECTHMAX = 0xff,
			.PRECLIPMIN = 0x100,
			.PRECTHMIN = 0x100,
			.PREAE = 0,
		},
		.PRECAL[1].bit = {	// D3
			.PREOFSET = 0,
			.PRECLIPMAX = 0xff,
			.PRECTHMAX = 0xff,
			.PRECLIPMIN = 0x100,
			.PRECTHMIN = 0x100,
			.PREAE = 0,
		},
	},
	.calc[0] = {
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_CALC_D0_CFG0,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.GPC0PARA0.bit = {
			.GPC0SEL0 = E_IM_IIP_PARAM_GPCSEL_P0D0,
			.GPC0SEL1 = E_IM_IIP_PARAM_GPCSEL_CFG0,		// dummy
			.GPC0SEL2 = E_IM_IIP_PARAM_GPCSEL_P1D0,
			.GPC0SEL3 = E_IM_IIP_PARAM_GPCSEL_CFG1,		// dummy
			.G0MD0 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,
			.G0MD1 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,
			.G0MD2 = E_IM_IIP_PARAM_GMD_BIN_ADD,
		},
		.GPC0PARA1.bit = {
			.G0RND_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_0 = 0,
			.G0RND_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_1 = 0,
			.G0RND_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_2 = 0,
			.G0RND_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_3 = 0,
			.G0RND_4 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_4 = 0,
			.G0RND_5 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_5 = 0,
			.G0RND_6 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_6 = 0,
		},
		.GPC1PARA0.bit = {
			.GPC1SEL0 = E_IM_IIP_PARAM_GPCSEL_P0D0,		// dummy
			.GPC1SEL1 = E_IM_IIP_PARAM_GPCSEL_P1D0,		// dummy
			.G1MD0 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,	// dummy
			.GPC1SEL2 = E_IM_IIP_PARAM_GPCSEL_CFG1,		// dummy
			.G1MD1 = E_IM_IIP_PARAM_GMD_BIN_THROUGH,	// dummy
		},
		.GPC1PARA1.bit = {
			.G1RND_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_0 = 0,
			.G1RND_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_1 = 0,
			.G1RND_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_2 = 0,
			.G1RND_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_3 = 0,
			.G1RND_4 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_4 = 0,
		},
		.SERPASS.bit = {
			.SEDLY0 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SESEL0 = E_IM_IIP_PARAM_SESEL0_INPUT_SELECTOR,
			.SEDLY1 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SESEL1 = E_IM_IIP_PARAM_SESEL0_INPUT_SELECTOR,
			.SEDLY2 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY3 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY4 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY5 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY6 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY7 = E_IM_IIP_PARAM_SEDLY_0_STEP,
		},
		.COMPSEL.bit = {
			.COMPSEL = E_IM_IIP_PARAM_COMPSEL_CFG0,
			.CMPRND = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.CMPSFT = 0,
			.CORPEN = 0,
			.CORMEN = 0,
			.CLPPEN = 0,
			.CLPMEN = 0,
		},
		.CONFCNT.bit = {
			.CONFWEN0 = 0,
			.CONFWEN1 = 0,
			.CONFWEN2 = 0,
			.CONFWEN3 = 0,
			.CONFREGSEL0 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL1 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL2 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL3 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL4 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL5 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL6 = E_IM_IIP_PARAM_CONFREGSEL_CALC0,		// Port1 output = calculator0 value
			.CONFREGSEL7 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL8 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL9 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,		// Port0 output = fixed value
		},
		.CONFREG = {
			0,
			0,
			0,
			0,
		},
	},
	.calc[1] = {
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_CALC_D1_CFG0,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.GPC0PARA0.bit = {
			.GPC0SEL0 = E_IM_IIP_PARAM_GPCSEL_P0D0,
			.GPC0SEL1 = E_IM_IIP_PARAM_GPCSEL_CFG0,		// dummy
			.GPC0SEL2 = E_IM_IIP_PARAM_GPCSEL_P1D0,
			.GPC0SEL3 = E_IM_IIP_PARAM_GPCSEL_CFG1,		// dummy
			.G0MD0 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,
			.G0MD1 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,
			.G0MD2 = E_IM_IIP_PARAM_GMD_BIN_THROUGH,
		},
		.GPC0PARA1.bit = {
			.G0RND_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_0 = 0,
			.G0RND_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_1 = 0,
			.G0RND_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_2 = 0,
			.G0RND_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_3 = 0,
			.G0RND_4 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_4 = 0,
			.G0RND_5 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_5 = 0,
			.G0RND_6 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_6 = 0,
		},
		.GPC1PARA0.bit = {
			.GPC1SEL0 = E_IM_IIP_PARAM_GPCSEL_P0D0,		// dummy
			.GPC1SEL1 = E_IM_IIP_PARAM_GPCSEL_P1D0,		// dummy
			.G1MD0 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,	// dummy
			.GPC1SEL2 = E_IM_IIP_PARAM_GPCSEL_CFG1,		// dummy
			.G1MD1 = E_IM_IIP_PARAM_GMD_BIN_THROUGH,	// dummy
		},
		.GPC1PARA1.bit = {
			.G1RND_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_0 = 0,
			.G1RND_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_1 = 0,
			.G1RND_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_2 = 0,
			.G1RND_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_3 = 0,
			.G1RND_4 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_4 = 0,
		},
		.SERPASS.bit = {
			.SEDLY0 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SESEL0 = E_IM_IIP_PARAM_SESEL0_INPUT_SELECTOR,
			.SEDLY1 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SESEL1 = E_IM_IIP_PARAM_SESEL0_INPUT_SELECTOR,
			.SEDLY2 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY3 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY4 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY5 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY6 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY7 = E_IM_IIP_PARAM_SEDLY_0_STEP,
		},
		.COMPSEL.bit = {
			.COMPSEL = E_IM_IIP_PARAM_COMPSEL_CFG0,
			.CMPRND = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.CMPSFT = 0,
			.CORPEN = 0,
			.CORMEN = 0,
			.CLPPEN = 0,
			.CLPMEN = 0,
		},
		.CONFCNT.bit = {
			.CONFWEN0 = 0,
			.CONFWEN1 = 0,
			.CONFWEN2 = 0,
			.CONFWEN3 = 0,
			.CONFREGSEL0 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL1 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL2 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL3 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL4 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL5 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL6 = E_IM_IIP_PARAM_CONFREGSEL_CALC0,		// Port1 output = calculator0 value
			.CONFREGSEL7 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL8 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL9 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,		// Port0 output = fixed value
		},
		.CONFREG = {
			0,
			0,
			0,
			0,
		},
	},
	.calc[2] = {
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_CALC_D2_CFG0,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.GPC0PARA0.bit = {
			.GPC0SEL0 = E_IM_IIP_PARAM_GPCSEL_P0D0,
			.GPC0SEL1 = E_IM_IIP_PARAM_GPCSEL_CFG0,		// dummy
			.GPC0SEL2 = E_IM_IIP_PARAM_GPCSEL_P1D0,
			.GPC0SEL3 = E_IM_IIP_PARAM_GPCSEL_CFG1,		// dummy
			.G0MD0 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,
			.G0MD1 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,
			.G0MD2 = E_IM_IIP_PARAM_GMD_BIN_THROUGH,
		},
		.GPC0PARA1.bit = {
			.G0RND_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_0 = 0,
			.G0RND_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_1 = 0,
			.G0RND_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_2 = 0,
			.G0RND_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_3 = 0,
			.G0RND_4 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_4 = 0,
			.G0RND_5 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_5 = 0,
			.G0RND_6 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_6 = 0,
		},
		.GPC1PARA0.bit = {
			.GPC1SEL0 = E_IM_IIP_PARAM_GPCSEL_P0D0,		// dummy
			.GPC1SEL1 = E_IM_IIP_PARAM_GPCSEL_P1D0,		// dummy
			.G1MD0 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,	// dummy
			.GPC1SEL2 = E_IM_IIP_PARAM_GPCSEL_CFG1,		// dummy
			.G1MD1 = E_IM_IIP_PARAM_GMD_BIN_THROUGH,	// dummy
		},
		.GPC1PARA1.bit = {
			.G1RND_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_0 = 0,
			.G1RND_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_1 = 0,
			.G1RND_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_2 = 0,
			.G1RND_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_3 = 0,
			.G1RND_4 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_4 = 0,
		},
		.SERPASS.bit = {
			.SEDLY0 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SESEL0 = E_IM_IIP_PARAM_SESEL0_INPUT_SELECTOR,
			.SEDLY1 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SESEL1 = E_IM_IIP_PARAM_SESEL0_INPUT_SELECTOR,
			.SEDLY2 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY3 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY4 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY5 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY6 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY7 = E_IM_IIP_PARAM_SEDLY_0_STEP,
		},
		.COMPSEL.bit = {
			.COMPSEL = E_IM_IIP_PARAM_COMPSEL_CFG0,
			.CMPRND = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.CMPSFT = 0,
			.CORPEN = 0,
			.CORMEN = 0,
			.CLPPEN = 0,
			.CLPMEN = 0,
		},
		.CONFCNT.bit = {
			.CONFWEN0 = 0,
			.CONFWEN1 = 0,
			.CONFWEN2 = 0,
			.CONFWEN3 = 0,
			.CONFREGSEL0 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL1 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL2 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL3 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL4 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL5 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL6 = E_IM_IIP_PARAM_CONFREGSEL_CALC0,		// Port1 output = calculator0 value
			.CONFREGSEL7 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL8 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL9 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,		// Port0 output = fixed value
		},
		.CONFREG = {
			0,
			0,
			0,
			0,
		},
	},
	.alpha = {
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_ALPHA,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.GPCALP = {
			[0].bit = {
				.GPCALPFIX = 0,
				.GPCALPSEL0 = E_IM_IIP_PARAM_GPCALPSEL_P0D3,
				.GPCALPSEL1 = E_IM_IIP_PARAM_GPCALPSEL_P1D3,
				.GPCALPMODE = E_IM_IIP_PARAM_GPCALPMODE_THROUGH,
			},
			[1].bit = {
				.GPCALPFIX = 0,
				.GPCALPSEL0 = E_IM_IIP_PARAM_GPCALPSEL_P0D3,
				.GPCALPSEL1 = E_IM_IIP_PARAM_GPCALPSEL_P1D3,
				.GPCALPMODE = E_IM_IIP_PARAM_GPCALPMODE_THROUGH,
			},
			[2].bit = {
				.GPCALPFIX = 0,
				.GPCALPSEL0 = E_IM_IIP_PARAM_GPCALPSEL_P0D3,
				.GPCALPSEL1 = E_IM_IIP_PARAM_GPCALPSEL_P1D3,
				.GPCALPMODE = E_IM_IIP_PARAM_GPCALPMODE_THROUGH,
			},
			[3].bit = {
				.GPCALPFIX = 0,
				.GPCALPSEL0 = E_IM_IIP_PARAM_GPCALPSEL_P0D3,
				.GPCALPSEL1 = E_IM_IIP_PARAM_GPCALPSEL_P1D3,
				.GPCALPMODE = E_IM_IIP_PARAM_GPCALPMODE_THROUGH,
			},
		},
	},
	.inter_comp_calc = {
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_INTER_COMP_CALC,
		.PARACNT.bit.SEQEN = 1,		// Sequence start.
		.PARACNT.bit.LPARA = 1,		// this parameter is last.
		.P1OUTMD.bit = {
			.P1RNDO_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_0 = 0,
			.P1SELO_0 = E_IM_IIP_PARAM_P1SELO_CONF6_D0,
			.P1RNDO_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_1 = 0,
			.P1SELO_1 = E_IM_IIP_PARAM_P1SELO_CONF6_D1,
			.P1RNDO_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_2 = 0,
			.P1SELO_2 = E_IM_IIP_PARAM_P1SELO_CONF6_D2,
			.P1RNDO_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_3 = 0,
			.P1SELO_3 = E_IM_IIP_PARAM_P1SELO_ALPHA,
		},
		.P0OUTMD_D3.bit = {
			.P0RNDO_D3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P0SFTO_D3 = 0,
			.P0SELO_D3 = E_IM_IIP_PARAM_P0SELO_ALPHA,
		},
		.ELECAL = {
			[0].bit = {
				.ELESEL0 = E_IM_IIP_PARAM_ELESEL_D0_CFG9,
				.ELERD0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT0 = 0,
				.ELESEL1 = E_IM_IIP_PARAM_ELESEL_D1_CFG9,
				.ELERD1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT1 = 0,
				.ELESEL2 = E_IM_IIP_PARAM_ELESEL_D2_CFG9,
				.ELERD2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT2 = 0,
				.ELESEL3 = E_IM_IIP_PARAM_ELESEL_D3_ALPHA,
				.ELERD3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT3 = 0,
				.ELEMODE0 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE1 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE2 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELERDO = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFTO = 0,
			},
			[1].bit = {
				.ELESEL0 = E_IM_IIP_PARAM_ELESEL_D1_CFG9,
				.ELERD0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT0 = 0,
				.ELESEL1 = E_IM_IIP_PARAM_ELESEL_D2_CFG9,
				.ELERD1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT1 = 0,
				.ELESEL2 = E_IM_IIP_PARAM_ELESEL_D3_ALPHA,
				.ELERD2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT2 = 0,
				.ELESEL3 = E_IM_IIP_PARAM_ELESEL_D0_CFG9,
				.ELERD3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT3 = 0,
				.ELEMODE0 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE1 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE2 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELERDO = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFTO = 0,
			},
			[2].bit = {
				.ELESEL0 = E_IM_IIP_PARAM_ELESEL_D2_CFG9,
				.ELERD0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT0 = 0,
				.ELESEL1 = E_IM_IIP_PARAM_ELESEL_D3_ALPHA,
				.ELERD1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT1 = 0,
				.ELESEL2 = E_IM_IIP_PARAM_ELESEL_D0_CFG9,
				.ELERD2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT2 = 0,
				.ELESEL3 = E_IM_IIP_PARAM_ELESEL_D1_CFG9,
				.ELERD3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT3 = 0,
				.ELEMODE0 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE1 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE2 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELERDO = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFTO = 0,
			},
		},
	},
};

#define D_GPC_STEPNUM_13_1_3		(10)
#ifdef __GNUC__
static T_IM_IZA_PARAM_GPC_CONFIG_13_1_3 g_ct_im_iip_gpc_config_13_1_3 __attribute__((aligned(64))) = {					// Needs 64bytes align
#else
static __align(64) T_IM_IZA_PARAM_GPC_CONFIG_13_1_3 g_ct_im_iip_gpc_config_13_1_3 = {					// Needs 64bytes align
#endif
	.seq = {
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_SEQ_CTRL,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.SEQLOOP = D_IM_IIP_VGA_LINES,
		.SEQCTL.bit = {
			.STEPNUM = D_GPC_STEPNUM_13_1_3,

			.CNTEND_0 = D_IM_IIP_VGA_WIDTH,
			.SEQCONF_0 = 0,		// Use parameter 0.
			.CNFOUT_0 = 1,		// Enable output

			.CNTEND_1 = 0,		// dummy(invalid)
			.SEQCONF_1 = 0,		// dummy(invalid)
			.CNFOUT_1 = 0,		// Disable output

			.CNTEND_2 = 0,		// dummy(invalid)
			.SEQCONF_2 = 0,		// dummy(invalid)
			.CNFOUT_2 = 0,		// Disable output

			.CNTEND_3 = 0,		// dummy(invalid)
			.SEQCONF_3 = 0,		// dummy(invalid)
			.CNFOUT_3 = 0,		// Disable output
		},
	},
	.precal_p0_d0_d1 = {		// Through
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_PRECAL_P0_D0_1,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.PRECAL[0].bit = {	// D0
			.PREOFSET = 0,
			.PRECLIPMAX = 0x3FFF,
			.PRECTHMAX = 0x3FFF,
			.PRECLIPMIN =  0x4000,
			.PRECTHMIN =  0x4000,
			.PREAE = 0,
		},
		.PRECAL[1].bit = {	// D1
			.PREOFSET = 0,
			.PRECLIPMAX = 0xFF,
			.PRECTHMAX = 0xFF,
			.PRECLIPMIN =  0x100,
			.PRECTHMIN =  0x100,
			.PREAE = 0,
		},
	},
	.precal_p0_d2_d3 = {		// Through
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_PRECAL_P0_D2_3,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.PRECAL[0].bit = {	// D2
			.PREOFSET = 0,
			.PRECLIPMAX = 0xFF,
			.PRECTHMAX = 0xFF,
			.PRECLIPMIN =  0x100,
			.PRECTHMIN =  0x100,
			.PREAE = 0,
		},
		.PRECAL[1].bit = {	// D3
			.PREOFSET = 0,
			.PRECLIPMAX = 0xFF,
			.PRECTHMAX = 0xFF,
			.PRECLIPMIN =  0x100,
			.PRECTHMIN =  0x100,
			.PREAE = 0,
		},
	},
	.precal_p1_d0_d1 = {		// Through
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_PRECAL_P1_D0_1,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.PRECAL[0].bit = {	// D0
			.PREOFSET = 0,
			.PRECLIPMAX = 0x3FFF,
			.PRECTHMAX = 0x3FFF,
			.PRECLIPMIN =  0x4000,
			.PRECTHMIN =  0x4000,
			.PREAE = 0,
		},
		.PRECAL[1].bit = {	// D1
			.PREOFSET = 0,
			.PRECLIPMAX = 0xFF,
			.PRECTHMAX = 0xFF,
			.PRECLIPMIN =  0x100,
			.PRECTHMIN =  0x100,
			.PREAE = 0,
		},
	},
	.precal_p1_d2_d3 = {		// Through
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_PRECAL_P1_D2_3,
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.PRECAL[0].bit = {	// D2
			.PREOFSET = 0,
			.PRECLIPMAX = 0xFF,
			.PRECTHMAX = 0xFF,
			.PRECLIPMIN =  0x100,
			.PRECTHMIN =  0x100,
			.PREAE = 0,
		},
		.PRECAL[1].bit = {	// D3
			.PREOFSET = 0,
			.PRECLIPMAX = 0x1,
			.PRECTHMAX = 0x1,
			.PRECLIPMIN =  0x100,
			.PRECTHMIN =  0x100,
			.PREAE = 0,
		},
	},
	.calc = {	// Blend
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_CALC_D0_CFG0,	// 0x200
		.PARACNT.bit.SEQEN = 0,		// Sequence dont start.
		.PARACNT.bit.LPARA = 0,		// read next parameter.
		.GPC0PARA0.bit = {
			.GPC0SEL0 = E_IM_IIP_PARAM_GPCSEL_CFG2,
			.GPC0SEL1 = E_IM_IIP_PARAM_GPCSEL_CFG3,
			.GPC0SEL2 = E_IM_IIP_PARAM_GPCSEL_CFG4,
			.GPC0SEL3 = E_IM_IIP_PARAM_GPCSEL_CFG1,
			.G0MD0 = E_IM_IIP_PARAM_GMD_MUL_MUL,
			.G0MD1 = E_IM_IIP_PARAM_GMD_MUL_MUL,
			.G0MD2 = E_IM_IIP_PARAM_GMD_BIN_ADD,
		},
		.GPC0PARA1.bit = {
			.G0RND_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_0 = 0,
			.G0RND_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_1 = 0,
			.G0RND_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_2 = 0,
			.G0RND_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_3 = 0,
			.G0RND_4 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_4 = 0,
			.G0RND_5 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_5 = 0,
			.G0RND_6 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G0SFT_6 = 0,
		},
		.GPC1PARA0.bit = {
			.GPC1SEL0 = E_IM_IIP_PARAM_GPCSEL_CFG0,
			.GPC1SEL1 = E_IM_IIP_PARAM_GPCSEL_P1D3,
			.G1MD0 = E_IM_IIP_PARAM_GMD_MUL_THROUGH,
			.GPC1SEL2 = E_IM_IIP_PARAM_GPCSEL_CFG1,
			.G1MD1 = E_IM_IIP_PARAM_GMD_BIN_SUB,
		},
		.GPC1PARA1.bit = {
			.G1RND_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_0 = 0,
			.G1RND_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_1 = 0,
			.G1RND_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_2 = 0,
			.G1RND_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_3 = 0,
			.G1RND_4 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.G1SFT_4 = 0,
		},
		.SERPASS.bit = {
			.SEDLY0 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SESEL0 = E_IM_IIP_PARAM_SESEL0_INPUT_SELECTOR,
			.SEDLY1 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SESEL1 = E_IM_IIP_PARAM_SESEL1_INPUT_SELECTOR,
			.SEDLY2 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY3 = E_IM_IIP_PARAM_SEDLY_0_STEP,
			.SEDLY4 = E_IM_IIP_PARAM_SEDLY_4_STEP,
			.SEDLY5 = E_IM_IIP_PARAM_SEDLY_4_STEP,
			.SEDLY6 = E_IM_IIP_PARAM_SEDLY_4_STEP,
			.SEDLY7 = E_IM_IIP_PARAM_SEDLY_0_STEP,
		},
		.COMPSEL.bit = {
			.COMPSEL = E_IM_IIP_PARAM_COMPSEL_CFG0,
			.CMPRND = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.CMPSFT = 0,
			.CORPEN = 0,
			.CORMEN = 0,
			.CLPPEN = 0,
			.CLPMEN = 0,
		},
		.CONFCNT.bit = {
			.CONFWEN0 = 1,	// Enable
			.CONFWEN1 = 0,
			.CONFWEN2 = 0,
			.CONFWEN3 = 0,
			.CONFREGSEL0 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL1 = E_IM_IIP_PARAM_CONFREGSEL_CALC1,
			.CONFREGSEL2 = E_IM_IIP_PARAM_CONFREGSEL_P0D0,
			.CONFREGSEL3 = E_IM_IIP_PARAM_CONFREGSEL_P1D3,
			.CONFREGSEL4 = E_IM_IIP_PARAM_CONFREGSEL_P1D0,
			.CONFREGSEL5 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL6 = E_IM_IIP_PARAM_CONFREGSEL_CALC0,		// Port1 output enable
			.CONFREGSEL7 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL8 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
			.CONFREGSEL9 = E_IM_IIP_PARAM_CONFREGSEL_CFG_n,
		},
		.CONFREG = {
			0x1,
			0,
			0,
			0,
		},
	},
	.inter_comp_calc = {	// Port0 D.C.   Port1 Through
		.PARACNT.bit.PARAAD = E_IM_IIP_PARAM_PARAAD_INTER_COMP_CALC,
		.PARACNT.bit.SEQEN = 1,		// Sequence start.
		.PARACNT.bit.LPARA = 1,		// this parameter is last.
		.P1OUTMD.bit = {
			.P1RNDO_0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_0 = 0,
			.P1SELO_0 = E_IM_IIP_PARAM_P1SELO_CONF6_D0,
			.P1RNDO_1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_1 = 0,
			.P1SELO_1 = E_IM_IIP_PARAM_P1SELO_CONF6_D1,
			.P1RNDO_2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_2 = 0,
			.P1SELO_2 = E_IM_IIP_PARAM_P1SELO_CONF6_D2,
			.P1RNDO_3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P1SFTO_3 = 0,
			.P1SELO_3 = E_IM_IIP_PARAM_P1SELO_ALPHA,
		},
		.P0OUTMD_D3.bit = {
			.P0RNDO_D3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
			.P0SFTO_D3 = 0,
			.P0SELO_D3 = E_IM_IIP_PARAM_P0SELO_ALPHA,
		},
		.ELECAL = {
			[0].bit = {
				.ELESEL0 = E_IM_IIP_PARAM_ELESEL_D0_CFG9,
				.ELERD0 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT0 = 0,
				.ELESEL1 = E_IM_IIP_PARAM_ELESEL_D1_CFG9,
				.ELERD1 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT1 = 0,
				.ELESEL2 = E_IM_IIP_PARAM_ELESEL_D2_CFG9,
				.ELERD2 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT2 = 0,
				.ELESEL3 = E_IM_IIP_PARAM_ELESEL_D3_ALPHA,
				.ELERD3 = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFT3 = 0,
				.ELEMODE0 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE1 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELEMODE2 = E_IM_IIP_PARAM_ELEMODE_THROUGH,
				.ELERDO = E_IM_IIP_PARAM_RND_ROUNDDOWN,
				.ELESHIFTO = 0,
			},
			// [1].bit
			// [2].bit
		},
	},
};


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
static ULONG ct_im_iip_gpc_set_pnsz0( ULONG sta, ULONG pnsz1 )
{
	ULONG pnsz0;

	if( sta != 0 ) {
		pnsz0 = pnsz1 - (sta % pnsz1);
		if( pnsz0 == 0 ) {
			pnsz0 = pnsz1;
		}
	}
	else {
		pnsz0 = pnsz1;
	}

	return pnsz0;
}

static INT32 ct_im_iip_gpc_blend( T_CT_IIP_GPC_BLEND *blend_inf, CHAR* func_nm )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0[2];	// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg[2];
	T_IM_IIP_UNIT_CFG				gpc_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf[2] = { &g_ct_im_iip_unit_param_1dl0, &g_ct_im_iip_unit_param_1dl1 };
	T_IM_IIP_PARAM_GPC*				gpc_unit_inf = &g_ct_im_iip_unit_param_gpc;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_GPC | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_3 | E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID			src_unitid[2] = { E_IM_IIP_UNIT_ID_LD1, E_IM_IIP_UNIT_ID_LD2 };
	const E_IM_IIP_UNIT_ID			filter_unitid = E_IM_IIP_UNIT_ID_GPC;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID		src_portid[2] = { E_IM_IIP_PARAM_PORTID_LD1, E_IM_IIP_PARAM_PORTID_LD2 };
	const E_IM_IIP_PARAM_PORTID		filter_portid[2] = { E_IM_IIP_PARAM_PORTID_GPC_P0, E_IM_IIP_PARAM_PORTID_GPC_P1 };
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32					src_pixid[2] = { 3, 4 };
	const UINT32					dst_pixid = 5;
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( "%s \n", func_nm ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	// A
	pixfmt_tbl_0[0] = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0[0].frame_type			= blend_inf->src_a.ffmt;
	pixfmt_tbl_0[0].pix_format			= blend_inf->src_a.pfmt;
	pixfmt_tbl_0[0].width				= blend_inf->src_a.width;
	pixfmt_tbl_0[0].lines				= blend_inf->src_a.lines;
	pixfmt_tbl_0[0].line_bytes.Y_G		= blend_inf->src_a.g_width;
	pixfmt_tbl_0[0].line_bytes.Cb_B		= blend_inf->src_a.g_width;	// Y_PACKED_CBCR
	pixfmt_tbl_0[0].line_bytes.Cr_R		= blend_inf->src_a.g_width;	// Y_PACKED_CBCR
	pixfmt_tbl_0[0].line_bytes.Alpha	= blend_inf->src_a.g_width;
	pixfmt_tbl_0[0].addr.Y_G			= blend_inf->src_a.addr_y;
	pixfmt_tbl_0[0].addr.Cb_B			= blend_inf->src_a.addr_c;
	pixfmt_tbl_0[0].addr.Cr_R			= 0;	// not used
	pixfmt_tbl_0[0].addr.Alpha			= 0;	// not used

	// B
	pixfmt_tbl_0[1] = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0[1].alpha				= D_IM_IIP_ALPHA_TRUE;		// alpha lane enable
	pixfmt_tbl_0[1].frame_type			= blend_inf->src_b.ffmt;
	pixfmt_tbl_0[1].pix_format			= blend_inf->src_b.pfmt;
	pixfmt_tbl_0[1].width				= blend_inf->src_b.width;
	pixfmt_tbl_0[1].lines				= blend_inf->src_b.lines;
	pixfmt_tbl_0[1].line_bytes.Y_G		= blend_inf->src_b.g_width;
	pixfmt_tbl_0[1].line_bytes.Cb_B		= blend_inf->src_b.g_width;	// Y_PACKED_CBCR
	pixfmt_tbl_0[1].line_bytes.Cr_R		= blend_inf->src_b.g_width;	// Y_PACKED_CBCR
	pixfmt_tbl_0[1].line_bytes.Alpha	= blend_inf->src_b.g_width;
	pixfmt_tbl_0[1].addr.Y_G			= blend_inf->src_b.addr_y;
	pixfmt_tbl_0[1].addr.Cb_B			= blend_inf->src_b.addr_c;
	pixfmt_tbl_0[1].addr.Cr_R			= 0;	// not used
	pixfmt_tbl_0[1].addr.Alpha			= blend_inf->alpha_addr;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.frame_type				= blend_inf->dst.ffmt;
	pixfmt_tbl_1.pix_format				= blend_inf->dst.pfmt;
	pixfmt_tbl_1.width					= blend_inf->dst.width;
	pixfmt_tbl_1.lines					= blend_inf->dst.lines;
	pixfmt_tbl_1.line_bytes.Y_G			= blend_inf->dst.g_width;
	pixfmt_tbl_1.line_bytes.Cb_B		= blend_inf->dst.g_width;	// Y_PACKED_CBCR
	pixfmt_tbl_1.line_bytes.Cr_R		= blend_inf->dst.g_width;	// Y_PACKED_CBCR
	pixfmt_tbl_1.line_bytes.Alpha		= blend_inf->dst.g_width;
	pixfmt_tbl_1.addr.Y_G				= blend_inf->dst.addr_y;
	pixfmt_tbl_1.addr.Cb_B				= blend_inf->dst.addr_c;
	pixfmt_tbl_1.addr.Cr_R				= 0;	// not used
	pixfmt_tbl_1.addr.Alpha				= 0;

	Ddim_Print(( "%s oned_unit_inf[0] = 0x%x 0x%x\n", func_nm, (UINT32)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[0], '\0', sizeof(*oned_unit_inf[0]) );
	*oned_unit_inf[0] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[0]->LD_TOPCNF0.bit.WAITCONF = filter_portid[0];
	oned_unit_inf[0]->PIXIDDEF.bit.IPIXID = src_pixid[0];
	oned_unit_inf[0]->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_2PIX_CYC;
	oned_unit_inf[0]->LD_PREFETCH.bit.PF_PDIST = E_IM_IIP_PARAM_PF_PDIST_64PIX;
	oned_unit_inf[0]->LD_HSIZE.bit.HSTART = blend_inf->sta_x;
	oned_unit_inf[0]->LD_HSIZE.bit.HEND = (blend_inf->sta_x + blend_inf->src_b.width) - 1;
	oned_unit_inf[0]->LD_VSIZE.bit.VSTART = blend_inf->sta_y;
	oned_unit_inf[0]->LD_VSIZE.bit.VEND = (blend_inf->sta_y + blend_inf->src_b.lines) - 1;
	oned_unit_inf[0]->LD_PHSZ.bit.PHSZ1 = 64;
	oned_unit_inf[0]->LD_PVSZ.bit.PVSZ1 = 8;
	oned_unit_inf[0]->LD_PHSZ.bit.PHSZ0 = ct_im_iip_gpc_set_pnsz0( oned_unit_inf[0]->LD_HSIZE.bit.HSTART, oned_unit_inf[0]->LD_PHSZ.bit.PHSZ1 );
	oned_unit_inf[0]->LD_PVSZ.bit.PVSZ0 = ct_im_iip_gpc_set_pnsz0( oned_unit_inf[0]->LD_VSIZE.bit.VSTART, oned_unit_inf[0]->LD_PVSZ.bit.PVSZ1 );

	oned_cfg[0].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[0].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[0].unit_param_addr = (ULONG)oned_unit_inf[0];
	oned_cfg[0].load_unit_param_flag = 0;

	Ddim_Print(( "%s oned_unit_inf[1] = 0x%x 0x%x\n", func_nm, (UINT32)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[1], '\0', sizeof(*oned_unit_inf[1]) );
	*oned_unit_inf[1] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[1]->LD_TOPCNF0.bit.WAITCONF = filter_portid[1];
	oned_unit_inf[1]->PIXIDDEF.bit.IPIXID = src_pixid[1];
	oned_unit_inf[1]->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_2PIX_CYC;
	oned_unit_inf[1]->LD_PREFETCH.bit.PF_PDIST = E_IM_IIP_PARAM_PF_PDIST_64PIX;
	oned_unit_inf[1]->LD_HSIZE.bit.HSTART = 0;
	oned_unit_inf[1]->LD_HSIZE.bit.HEND = blend_inf->src_b.width - 1;
	oned_unit_inf[1]->LD_VSIZE.bit.VSTART = 0;
	oned_unit_inf[1]->LD_VSIZE.bit.VEND = blend_inf->src_b.lines - 1;
	oned_unit_inf[1]->LD_PHSZ.bit.PHSZ1 = 64;
	oned_unit_inf[1]->LD_PVSZ.bit.PVSZ1 = 8;
	oned_unit_inf[1]->LD_PHSZ.bit.PHSZ0 = ct_im_iip_gpc_set_pnsz0( oned_unit_inf[1]->LD_HSIZE.bit.HSTART, oned_unit_inf[1]->LD_PHSZ.bit.PHSZ1 );
	oned_unit_inf[1]->LD_PVSZ.bit.PVSZ0 = ct_im_iip_gpc_set_pnsz0( oned_unit_inf[1]->LD_VSIZE.bit.VSTART, oned_unit_inf[1]->LD_PVSZ.bit.PVSZ1 );

	oned_cfg[1].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[1].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[1].unit_param_addr = (ULONG)oned_unit_inf[1];
	oned_cfg[1].load_unit_param_flag = 0;

	Ddim_Print(( "%s gpc_unit_inf = 0x%x 0x%x\n", func_nm, (UINT32)gpc_unit_inf, sizeof(T_IM_IIP_PARAM_GPC) ));

	memset( gpc_unit_inf, '\0', sizeof(*gpc_unit_inf) );
	g_ct_im_iip_gpc_config_13_1_3.seq.SEQLOOP = blend_inf->src_a.width;
	g_ct_im_iip_gpc_config_13_1_3.seq.SEQCTL.bit.CNTEND_0 = blend_inf->src_a.lines;

	*gpc_unit_inf = g_ct_im_iip_param_gpc_base;
	gpc_unit_inf->GPCTOPCNF[0].bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE;	// Port1 output
	gpc_unit_inf->GPCTOPCNF[0].bit.DATACONF = src_portid[0];
	gpc_unit_inf->GPCTOPCNF[1].bit.WAITCONF = dst_portid;					// Port1 output
	gpc_unit_inf->GPCTOPCNF[1].bit.DATACONF = src_portid[1];
	gpc_unit_inf->GPCCONTROL.bit.COMMONPARA = E_IM_IIP_PARAM_COMMONPARA_D0_D0_D0;	// used common parameter
	gpc_unit_inf->GPCCLOADADR.bit.CLOADADR = (ULONG)&g_ct_im_iip_gpc_config_13_1_3;

	gpc_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	gpc_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	gpc_cfg.unit_param_addr = (ULONG)gpc_unit_inf;
	gpc_cfg.load_unit_param_flag = 0;

	Ddim_Print(( "%s sl_unit_inf = 0x%x 0x%x\n", func_nm, (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = filter_portid[1];				// Port1 output
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;
	sl_unit_inf->BASE.CLPTH_U_0.bit.CLPTH_U = 0xFF;
	sl_unit_inf->BASE.CLPVAL_U_0.bit.CLPVAL_U = 0xFF;

	sl_unit_inf->BASE.SL_HSIZE.bit.HSTART = blend_inf->sta_x;
	sl_unit_inf->BASE.SL_HSIZE.bit.HEND = (blend_inf->sta_x + blend_inf->src_b.width) - 1;
	sl_unit_inf->BASE.SL_VSIZE.bit.VSTART = blend_inf->sta_y;
	sl_unit_inf->BASE.SL_VSIZE.bit.VEND = (blend_inf->sta_y + blend_inf->src_b.lines) - 1;
	sl_unit_inf->BASE.SL_PHSZ.bit.PHSZ1 =  64;
	sl_unit_inf->BASE.SL_PVSZ.bit.PVSZ1 =  8;
	sl_unit_inf->BASE.SL_PHSZ.bit.PHSZ0 = ct_im_iip_gpc_set_pnsz0( sl_unit_inf->BASE.SL_HSIZE.bit.HSTART, sl_unit_inf->BASE.SL_PHSZ.bit.PHSZ1 );
	sl_unit_inf->BASE.SL_PVSZ.bit.PVSZ0 = ct_im_iip_gpc_set_pnsz0( sl_unit_inf->BASE.SL_VSIZE.bit.VSTART, sl_unit_inf->BASE.SL_PVSZ.bit.PVSZ1 );

	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)gpc_unit_inf, sizeof(T_IM_IIP_PARAM_GPC) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)&g_ct_im_iip_gpc_config_13_1_3, sizeof(T_IM_IZA_PARAM_GPC_CONFIG_13_1_3) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[0], &pixfmt_tbl_0[0] );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[1], &pixfmt_tbl_0[1] );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[0], &oned_cfg[0] );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[1], &oned_cfg[1] );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter_unitid, &gpc_cfg );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( "%s Start1\n", func_nm ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	Ddim_Print(( "%s Start2\n", func_nm ));
	ercd = Im_IIP_Start_SWTRG( filter_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	Ddim_Print(( "%s Start3\n", func_nm ));
	ercd = Im_IIP_Start_SWTRG( src_unitid[0], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	Ddim_Print(( "%s Start4\n", func_nm ));
	ercd = Im_IIP_Start_SWTRG( src_unitid[1], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	Ddim_Print(( "%s WaitEnd\n", func_nm ));
	ercd = Im_IIP_Wait_End( &wait_factor_result, wait_factor, D_IM_IIP_OR_WAIT, 30 );
	Ddim_Print(( "%s ercd = 0x%x factor=0x%x\n", func_nm, ercd, wait_factor_result ));
	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( "%s Stop\n", func_nm ));
		ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
		Ddim_Print(( "%s ercd = 0x%x factor=0x%x\n", func_nm, ercd, wait_factor_result ));
	}

	Ddim_Print(( "%s Im_IIP_Close_SWTRG\n", func_nm ));
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( "%s ercd = 0x%x\n", func_nm, ercd ));

	return D_DDIM_OK;
}

// GPC Unit 13-1-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_13_1_1: "
INT32 CT_Im_IIP_13_1_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0[2];	// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg[2];
	T_IM_IIP_UNIT_CFG				gpc_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf[2] = { &g_ct_im_iip_unit_param_1dl0, &g_ct_im_iip_unit_param_1dl1 };
	T_IM_IIP_PARAM_GPC*				gpc_unit_inf = &g_ct_im_iip_unit_param_gpc;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_BLEND0 | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_3 | E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID			src_unitid[2] = { E_IM_IIP_UNIT_ID_LD1, E_IM_IIP_UNIT_ID_LD2 };
	const E_IM_IIP_UNIT_ID			filter_unitid = E_IM_IIP_UNIT_ID_GPC;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID		src_portid[2] = { E_IM_IIP_PARAM_PORTID_LD1, E_IM_IIP_PARAM_PORTID_LD2 };
	const E_IM_IIP_PARAM_PORTID		filter_portid[2] = { E_IM_IIP_PARAM_PORTID_GPC_P0, E_IM_IIP_PARAM_PORTID_GPC_P1 };
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32					src_pixid[2] = { 3, 4 };
	const UINT32					dst_pixid = 5;
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0[0] = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0[0].line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y;
	pixfmt_tbl_0[0].addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0[0].addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0[0].addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A;

	pixfmt_tbl_0[1] = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0[1].line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_Y;
	pixfmt_tbl_0[1].addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C;
	pixfmt_tbl_0[1].addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C;
	pixfmt_tbl_0[1].addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_A;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[0] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[0], '\0', sizeof(*oned_unit_inf[0]) );
	*oned_unit_inf[0] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[0]->LD_TOPCNF0.bit.WAITCONF = filter_portid[0];
	oned_unit_inf[0]->PIXIDDEF.bit.IPIXID = src_pixid[0];
	oned_unit_inf[0]->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;


	oned_cfg[0].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[0].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[0].unit_param_addr = (ULONG)oned_unit_inf[0];
	oned_cfg[0].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[1] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[1], '\0', sizeof(*oned_unit_inf[1]) );
	*oned_unit_inf[1] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[1]->LD_TOPCNF0.bit.WAITCONF = filter_portid[1];
	oned_unit_inf[1]->PIXIDDEF.bit.IPIXID = src_pixid[1];
	oned_unit_inf[1]->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;

	oned_cfg[1].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[1].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[1].unit_param_addr = (ULONG)oned_unit_inf[1];
	oned_cfg[1].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "gpc_unit_inf = 0x%x 0x%x\n", (UINT32)gpc_unit_inf, sizeof(T_IM_IIP_PARAM_GPC) ));

	memset( gpc_unit_inf, '\0', sizeof(*gpc_unit_inf) );
	*gpc_unit_inf = g_ct_im_iip_param_gpc_base;
	gpc_unit_inf->GPCTOPCNF[0].bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE;	// Port1 output
	gpc_unit_inf->GPCTOPCNF[0].bit.DATACONF = src_portid[0];
	gpc_unit_inf->GPCTOPCNF[1].bit.WAITCONF = dst_portid;					// Port1 output
	gpc_unit_inf->GPCTOPCNF[1].bit.DATACONF = src_portid[1];
	gpc_unit_inf->GPCCLOADADR.bit.CLOADADR = (ULONG)&g_ct_im_iip_gpc_config;

	gpc_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	gpc_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	gpc_cfg.unit_param_addr = (ULONG)gpc_unit_inf;
	gpc_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = filter_portid[1];				// Port1 output
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)gpc_unit_inf, sizeof(T_IM_IIP_PARAM_GPC) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)&g_ct_im_iip_gpc_config, sizeof(T_IM_IZA_PARAM_GPC_CONFIG) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#if 0
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)gpc_unit_inf, sizeof(T_IM_IIP_PARAM_GPC) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[0], &pixfmt_tbl_0[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[1], &pixfmt_tbl_0[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[0], &oned_cfg[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[1], &oned_cfg[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter_unitid, &gpc_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( filter_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start3\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid[0], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start4\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid[1], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

#if 0
	for( UINT32 loopcnt = 0; loopcnt < 10; loopcnt++ ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME " SLALE=%u INTIZ0=0x%lx INTIZ1=0x%lx\n",
					IO_IIP.IZACTL1.bit.SLALE,
					IO_IIP.INTIZ0.word,
					IO_IIP.INTIZ1.word
				   ));
		DDIM_User_Dly_Tsk( 1 );
	}
#endif

	Ddim_Print(( D_IM_IIP_FUNC_NAME "WaitEnd\n" ));
	ercd = Im_IIP_Wait_End( &wait_factor_result, wait_factor, D_IM_IIP_OR_WAIT, 30 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "Stop\n" ));
		ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	}

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#if 0
	// UnitINFダンプ
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)gpc_unit_inf, sizeof(T_IM_IIP_PARAM_LUT) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif


#if 0
	// dump UNITINF ONED[1]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( src_unitid[0], g_get_unit_param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF ONED[1] %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)g_get_unit_param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)g_get_unit_param, param_bytes );
		}
	}
#endif


#if 0
	// dump UNITINF ONED[2]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( src_unitid[1], g_get_unit_param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF ONED[2] %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)g_get_unit_param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)g_get_unit_param, param_bytes );
		}
	}
#endif


#if 0
	// dump UNITINF gpc
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( filter_unitid, g_get_unit_param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF gpc %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)g_get_unit_param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)g_get_unit_param, param_bytes );
		}
	}
#endif


#if 0
	// dump UNITINF SL[2]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( dst_unitid, g_get_unit_param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF SL[2] %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)g_get_unit_param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)g_get_unit_param, param_bytes );
		}
	}
#endif


#if 0
#ifndef CO_DEBUG_ON_PC
	// dump IO_IIP
	{
		ULONG regdump_addr = D_IM_IIP_REGDUMP_ADDR;
		ULONG regdump_bytes = sizeof(IO_IIP);
		Ddim_Print(( D_IM_IIP_FUNC_NAME "dump IO_IIP %u\n", regdump_bytes ));
		memcpy( (VOID*)regdump_addr, (VOID*)&IO_IIP, regdump_bytes );
		Palladium_Set_Out_Localstack( regdump_addr, regdump_bytes );
	}
#endif //!CO_DEBUG_ON_PC
#endif
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Im_IIP_Close_SWTRG\n" ));
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


// GPC Unit 13-1-2
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_13_1_2: "
INT32 CT_Im_IIP_13_1_2( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0[2];	// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg[2];
	T_IM_IIP_UNIT_CFG				gpc_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf[2] = { &g_ct_im_iip_unit_param_1dl0, &g_ct_im_iip_unit_param_1dl1 };
	T_IM_IIP_PARAM_GPC*				gpc_unit_inf = &g_ct_im_iip_unit_param_gpc;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_BLEND0 | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_3 | E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID			src_unitid[2] = { E_IM_IIP_UNIT_ID_LD1, E_IM_IIP_UNIT_ID_LD2 };
	const E_IM_IIP_UNIT_ID			filter_unitid = E_IM_IIP_UNIT_ID_GPC;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID		src_portid[2] = { E_IM_IIP_PARAM_PORTID_LD1, E_IM_IIP_PARAM_PORTID_LD2 };
	const E_IM_IIP_PARAM_PORTID		filter_portid[2] = { E_IM_IIP_PARAM_PORTID_GPC_P0, E_IM_IIP_PARAM_PORTID_GPC_P1 };
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32					src_pixid[2] = { 3, 4 };
	const UINT32					dst_pixid = 5;
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0[0] = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0[0].line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y;
	pixfmt_tbl_0[0].addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0[0].addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0[0].addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A;

	pixfmt_tbl_0[1] = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0[1].line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_Y;
	pixfmt_tbl_0[1].addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C;
	pixfmt_tbl_0[1].addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C;
	pixfmt_tbl_0[1].addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_A;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[0] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[0], '\0', sizeof(*oned_unit_inf[0]) );
	*oned_unit_inf[0] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[0]->LD_TOPCNF0.bit.WAITCONF = filter_portid[0];
	oned_unit_inf[0]->PIXIDDEF.bit.IPIXID = src_pixid[0];
	oned_unit_inf[0]->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;


	oned_cfg[0].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[0].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[0].unit_param_addr = (ULONG)oned_unit_inf[0];
	oned_cfg[0].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[1] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[1], '\0', sizeof(*oned_unit_inf[1]) );
	*oned_unit_inf[1] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[1]->LD_TOPCNF0.bit.WAITCONF = filter_portid[1];
	oned_unit_inf[1]->PIXIDDEF.bit.IPIXID = src_pixid[1];
	oned_unit_inf[1]->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;

	oned_cfg[1].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[1].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[1].unit_param_addr = (ULONG)oned_unit_inf[1];
	oned_cfg[1].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "gpc_unit_inf = 0x%x 0x%x\n", (UINT32)gpc_unit_inf, sizeof(T_IM_IIP_PARAM_GPC) ));

	memset( gpc_unit_inf, '\0', sizeof(*gpc_unit_inf) );
	*gpc_unit_inf = g_ct_im_iip_param_gpc_base;
	gpc_unit_inf->GPCTOPCNF[0].bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE;	// Port1 output
	gpc_unit_inf->GPCTOPCNF[0].bit.DATACONF = src_portid[0];
	gpc_unit_inf->GPCTOPCNF[1].bit.WAITCONF = dst_portid;					// Port1 output
	gpc_unit_inf->GPCTOPCNF[1].bit.DATACONF = src_portid[1];
	gpc_unit_inf->GPCCONTROL.bit.COMMONPARA = E_IM_IIP_PARAM_COMMONPARA_D0_D1_D2;
	gpc_unit_inf->GPCCLOADADR.bit.CLOADADR = (ULONG)&g_ct_im_iip_gpc_config_13_1_2;

	gpc_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	gpc_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	gpc_cfg.unit_param_addr = (ULONG)gpc_unit_inf;
	gpc_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = filter_portid[1];				// Port1 output
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)gpc_unit_inf, sizeof(T_IM_IIP_PARAM_GPC) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)&g_ct_im_iip_gpc_config_13_1_2, sizeof(T_IM_IZA_PARAM_GPC_CONFIG_13_1_2) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[0], &pixfmt_tbl_0[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[1], &pixfmt_tbl_0[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[0], &oned_cfg[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[1], &oned_cfg[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter_unitid, &gpc_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( filter_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start3\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid[0], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start4\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid[1], D_IM_IIP_SWTRG_ON );
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

// GPC Unit 13-1-3
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_13_1_3: "
INT32 CT_Im_IIP_13_1_3( VOID )
{
	T_CT_IIP_GPC_BLEND				blend_info;

	// VGA all area
	blend_info.sta_x	= 0;
	blend_info.sta_y	= 0;

	blend_info.src_a.pfmt		= E_IM_IIP_PFMT_YCC422;
	blend_info.src_a.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_a.g_width	= D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	blend_info.src_a.width		= D_IM_IIP_VGA_WIDTH;
	blend_info.src_a.lines		= D_IM_IIP_VGA_LINES;
	blend_info.src_a.addr_y		= D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y;
	blend_info.src_a.addr_c		= D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;

	blend_info.src_b.pfmt		= E_IM_IIP_PFMT_YCC420;
	blend_info.src_b.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_b.g_width	= D_IM_IIP_VGA_YCC420_U8_Y_GLOBAL_WIDTH;
	blend_info.src_b.width		= D_IM_IIP_VGA_WIDTH;
	blend_info.src_b.lines		= D_IM_IIP_VGA_LINES;
	blend_info.src_b.addr_y		= D_IM_IIP_IMG_MEM_ADDR_4_YCC420_U8_Y;
	blend_info.src_b.addr_c		= D_IM_IIP_IMG_MEM_ADDR_4_YCC420_U8_C;
	blend_info.alpha_addr		= D_IM_IIP_IMG_MEM_ADDR_4_YCC420_U8_A;

	blend_info.dst.pfmt			= E_IM_IIP_PFMT_YCC422;
	blend_info.dst.ffmt			= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.dst.g_width		= D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	blend_info.dst.width		= D_IM_IIP_VGA_WIDTH;
	blend_info.dst.lines		= D_IM_IIP_VGA_LINES;
	blend_info.dst.addr_y		= D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	blend_info.dst.addr_c		= D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;

	return ct_im_iip_gpc_blend( &blend_info, D_IM_IIP_FUNC_NAME );
}

// GPC Unit 13-1-4
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_13_1_4: "
INT32 CT_Im_IIP_13_1_4( VOID )
{
	T_CT_IIP_GPC_BLEND				blend_info;

	// VGA 10%
	blend_info.sta_x	= ((D_IM_IIP_VGA_WIDTH - D_IM_IIP_VGA_1_10_WIDTH) >> 1) & 0xFFC0;
	blend_info.sta_y	= ((D_IM_IIP_VGA_LINES - D_IM_IIP_VGA_1_10_LINES) >> 1) & 0xFFF0;

	blend_info.src_a.pfmt		= E_IM_IIP_PFMT_YCC422;
	blend_info.src_a.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_a.g_width	= D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	blend_info.src_a.width		= D_IM_IIP_VGA_WIDTH;
	blend_info.src_a.lines		= D_IM_IIP_VGA_LINES;
	blend_info.src_a.addr_y		= D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y;
	blend_info.src_a.addr_c		= D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;

	blend_info.src_b.pfmt		= E_IM_IIP_PFMT_YCC420;
	blend_info.src_b.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_b.g_width	= D_IM_IIP_VGA_1_10_YCC420_U8_Y_GLOBAL_WIDTH;
	blend_info.src_b.width		= D_IM_IIP_VGA_1_10_WIDTH;
	blend_info.src_b.lines		= D_IM_IIP_VGA_1_10_LINES;
	blend_info.src_b.addr_y		= D_IM_IIP_IMG_MEM_ADDR_4_VGA_1_10_YCC420_U8_Y;
	blend_info.src_b.addr_c		= D_IM_IIP_IMG_MEM_ADDR_4_VGA_1_10_YCC420_U8_C;
	blend_info.alpha_addr		= D_IM_IIP_IMG_MEM_ADDR_4_VGA_1_10_YCC420_U8_A;

	// The same settings as the src_a
	blend_info.dst				= blend_info.src_a;

	return ct_im_iip_gpc_blend( &blend_info, D_IM_IIP_FUNC_NAME );
}

// GPC Unit 13-1-5
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_13_1_5: "
INT32 CT_Im_IIP_13_1_5( VOID )
{
	T_CT_IIP_GPC_BLEND			blend_info;

	// Rebery VGA all area
	blend_info.sta_x	= 0;
	blend_info.sta_y	= 0;

	blend_info.src_a.pfmt		= E_IM_IIP_PFMT_VIDEO;
	blend_info.src_a.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_a.g_width	= D_IM_IIP_VGA_RIBERY_U8_Y_GLOBAL_WIDTH;
	blend_info.src_a.width		= D_IM_IIP_VGA_WIDTH;
	blend_info.src_a.lines		= D_IM_IIP_VGA_LINES;
	blend_info.src_a.addr_y		= D_IM_IIP_IMG_MEM_ADDR_0_VGA_RIBERY_U8_Y;
	blend_info.src_a.addr_c		= D_IM_IIP_IMG_MEM_ADDR_0_VGA_RIBERY_U8_C;

	blend_info.src_b.pfmt		= E_IM_IIP_PFMT_YCC420;
	blend_info.src_b.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_b.g_width	= D_IM_IIP_VGA_YCC420_U8_Y_GLOBAL_WIDTH;
	blend_info.src_b.width		= D_IM_IIP_VGA_WIDTH;
	blend_info.src_b.lines		= D_IM_IIP_VGA_LINES;
	blend_info.src_b.addr_y		= D_IM_IIP_IMG_MEM_ADDR_4_YCC420_U8_Y;
	blend_info.src_b.addr_c		= D_IM_IIP_IMG_MEM_ADDR_4_YCC420_U8_C;
	blend_info.alpha_addr		= D_IM_IIP_IMG_MEM_ADDR_4_YCC420_U8_A;

	blend_info.dst.pfmt			= E_IM_IIP_PFMT_VIDEO;
	blend_info.dst.ffmt			= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.dst.g_width		= D_IM_IIP_VGA_RIBERY_U8_Y_GLOBAL_WIDTH;
	blend_info.dst.width		= D_IM_IIP_VGA_WIDTH;
	blend_info.dst.lines		= D_IM_IIP_VGA_LINES;
	blend_info.dst.addr_y		= D_IM_IIP_IMG_MEM_ADDR_1_VGA_RIBERY_U8_Y;
	blend_info.dst.addr_c		= D_IM_IIP_IMG_MEM_ADDR_1_VGA_RIBERY_U8_C;

	return ct_im_iip_gpc_blend( &blend_info, D_IM_IIP_FUNC_NAME );
}

// GPC Unit 13-1-6
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_13_1_6: "
INT32 CT_Im_IIP_13_1_6( VOID )
{
	T_CT_IIP_GPC_BLEND				blend_info;

	// VGA 10%
	blend_info.sta_x	= ((D_IM_IIP_VGA_WIDTH - D_IM_IIP_VGA_1_10_WIDTH) >> 1) & 0xFFC0;
	blend_info.sta_y	= ((D_IM_IIP_VGA_LINES - D_IM_IIP_VGA_1_10_LINES) >> 1) & 0xFFF0;
	blend_info.src_a.pfmt		= E_IM_IIP_PFMT_VIDEO;
	blend_info.src_a.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_a.g_width	= D_IM_IIP_VGA_RIBERY_U8_Y_GLOBAL_WIDTH;
	blend_info.src_a.width		= D_IM_IIP_VGA_WIDTH;
	blend_info.src_a.lines		= D_IM_IIP_VGA_LINES;
	blend_info.src_a.addr_y		= D_IM_IIP_IMG_MEM_ADDR_0_VGA_RIBERY_U8_Y;
	blend_info.src_a.addr_c		= D_IM_IIP_IMG_MEM_ADDR_0_VGA_RIBERY_U8_C;

	blend_info.src_b.pfmt		= E_IM_IIP_PFMT_YCC420;
	blend_info.src_b.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_b.g_width	= D_IM_IIP_VGA_1_10_YCC420_U8_Y_GLOBAL_WIDTH;
	blend_info.src_b.width		= D_IM_IIP_VGA_1_10_WIDTH;
	blend_info.src_b.lines		= D_IM_IIP_VGA_1_10_LINES;
	blend_info.src_b.addr_y		= D_IM_IIP_IMG_MEM_ADDR_4_VGA_1_10_YCC420_U8_Y;
	blend_info.src_b.addr_c		= D_IM_IIP_IMG_MEM_ADDR_4_VGA_1_10_YCC420_U8_C;
	blend_info.alpha_addr		= D_IM_IIP_IMG_MEM_ADDR_4_VGA_1_10_YCC420_U8_A;

	// The same settings as the src_a
	blend_info.dst				= blend_info.src_a;

	return ct_im_iip_gpc_blend( &blend_info, D_IM_IIP_FUNC_NAME );
}

// GPC Unit 13-1-7
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_13_1_7: "
INT32 CT_Im_IIP_13_1_7( VOID )
{
	T_CT_IIP_GPC_BLEND				blend_info;

	// FullHD 10%
	blend_info.sta_x	= ((D_IM_IIP_FULLHD_WIDTH - D_IM_IIP_FULLHD_1_10_WIDTH) >> 1) & 0xFFC0;
	blend_info.sta_y	= ((D_IM_IIP_FULLHD_LINES - D_IM_IIP_FULLHD_1_10_LINES) >> 1) & 0xFFF0;
	blend_info.src_a.pfmt		= E_IM_IIP_PFMT_YCC422;
	blend_info.src_a.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_a.g_width	= D_IM_IIP_FULLHD_YCC422_U8_Y_GLOBAL_WIDTH;
	blend_info.src_a.width		= D_IM_IIP_FULLHD_WIDTH;
	blend_info.src_a.lines		= D_IM_IIP_FULLHD_LINES;
	blend_info.src_a.addr_y		= D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_Y;
	blend_info.src_a.addr_c		= D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_C;

	blend_info.src_b.pfmt		= E_IM_IIP_PFMT_YCC420;
	blend_info.src_b.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_b.g_width	= D_IM_IIP_FULLHD_1_10_YCC420_U8_Y_GLOBAL_WIDTH;
	blend_info.src_b.width		= D_IM_IIP_FULLHD_1_10_WIDTH;
	blend_info.src_b.lines		= D_IM_IIP_FULLHD_1_10_LINES;
	blend_info.src_b.addr_y		= D_IM_IIP_IMG_MEM_ADDR_4_FULLHD_1_10_YCC420_U8_Y;
	blend_info.src_b.addr_c		= D_IM_IIP_IMG_MEM_ADDR_4_FULLHD_1_10_YCC420_U8_C;
	blend_info.alpha_addr		= D_IM_IIP_IMG_MEM_ADDR_4_FULLHD_1_10_YCC420_U8_A;

	// The same settings as the src_a
	blend_info.dst				= blend_info.src_a;

	return ct_im_iip_gpc_blend( &blend_info, D_IM_IIP_FUNC_NAME );
}

// GPC Unit 13-1-8
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_13_1_8: "
INT32 CT_Im_IIP_13_1_8( VOID )
{
	T_CT_IIP_GPC_BLEND				blend_info;

	// FullHD 10%
	blend_info.sta_x	= ((D_IM_IIP_FULLHD_WIDTH - D_IM_IIP_FULLHD_1_10_WIDTH) >> 1) & 0xFFC0;
	blend_info.sta_y	= ((D_IM_IIP_FULLHD_LINES - D_IM_IIP_FULLHD_1_10_LINES) >> 1) & 0xFFF0;
	blend_info.src_a.pfmt		= E_IM_IIP_PFMT_VIDEO;
	blend_info.src_a.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_a.g_width	= D_IM_IIP_FULLHD_RIBERY_U8_Y_GLOBAL_WIDTH;
	blend_info.src_a.width		= D_IM_IIP_FULLHD_WIDTH;
	blend_info.src_a.lines		= D_IM_IIP_FULLHD_LINES;
	blend_info.src_a.addr_y		= D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_RIBERY_U8_Y;
	blend_info.src_a.addr_c		= D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_RIBERY_U8_C;

	blend_info.src_b.pfmt		= E_IM_IIP_PFMT_YCC420;
	blend_info.src_b.ffmt		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
	blend_info.src_b.g_width	= D_IM_IIP_FULLHD_1_10_YCC420_U8_Y_GLOBAL_WIDTH;
	blend_info.src_b.width		= D_IM_IIP_FULLHD_1_10_WIDTH;
	blend_info.src_b.lines		= D_IM_IIP_FULLHD_1_10_LINES;
	blend_info.src_b.addr_y		= D_IM_IIP_IMG_MEM_ADDR_4_FULLHD_1_10_YCC420_U8_Y;
	blend_info.src_b.addr_c		= D_IM_IIP_IMG_MEM_ADDR_4_FULLHD_1_10_YCC420_U8_C;
	blend_info.alpha_addr		= D_IM_IIP_IMG_MEM_ADDR_4_FULLHD_1_10_YCC420_U8_A;

	// The same settings as the src_a
	blend_info.dst				= blend_info.src_a;

	return ct_im_iip_gpc_blend( &blend_info, D_IM_IIP_FUNC_NAME );
}

// GPC Unit 13-1-X
INT32 CT_Im_IIP_Run_13_1( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-13-1-1
		case 1:
			return CT_Im_IIP_13_1_1();

		// Test-13-1-2
		case 2:
			return CT_Im_IIP_13_1_2();

		// Test-13-1-3
		case 3:
			return CT_Im_IIP_13_1_3();

		// Test-13-1-4
		case 4:
			return CT_Im_IIP_13_1_4();

		// Test-13-1-5
		case 5:
			return CT_Im_IIP_13_1_5();

		// Test-13-1-6
		case 6:
			return CT_Im_IIP_13_1_6();

		// Test-13-1-7
		case 7:
			return CT_Im_IIP_13_1_7();

		// Test-13-1-8
		case 8:
			return CT_Im_IIP_13_1_8();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#endif //CO_CT_IM_IIP_DISABLE

