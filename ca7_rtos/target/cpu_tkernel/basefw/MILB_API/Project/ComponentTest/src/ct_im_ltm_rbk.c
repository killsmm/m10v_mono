/**
 * @file		ct_im_ltm_rbk.c
 * @brief		LTM(RBK block) ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copybright 2016 Socionext Inc.</I></B>
 */

#include "ct_im_ltm.h"

#include "im_ltm.h"
#include "im_pro.h"
#include "im_pro_common.h"

#include "driver_common.h"
#include "ddim_user_custom.h"
//#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
//#endif

#include "jdsltm.h"

#include <stdlib.h>
#include <string.h>



/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define IM_LTM_RBK_ROUNDUP_N( val, align_val )	( ( ( (val) + (align_val - 1) ) / align_val ) * align_val )


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// nothing special.


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// nothing special.


/*----------------------------------------------------------------------*/
/* Local Method															*/
/*----------------------------------------------------------------------*/
#ifdef CO_MSG_PRINT_ON
#define D_IM_LTM_RESULT(statement)		result = statement
#else
#define D_IM_LTM_RESULT
#endif

static VOID ct_im_ltm_rbk_handler( UINT32* result, UINT32 user_param );

// Max/Min table
static T_IM_LTM_RBK_CTRL_COMMON gct_im_ltm_rbk_ctrl_common[] = {
	[0] = {	// max
		.line_intr = 0x3FFF,
		.burst_length = D_IM_LTM_BRST_256,
		.mask_write = D_IM_LTM_MASK_WRITE_DISABLE,
		.out_select = D_IM_LTM_RBK_OUT_MAP_DIRECT,
		.data_type = D_IM_LTM_DTYP_UNPACK12,
		.reduction_enable = D_IM_LTM_ENABLE_ON,
		.reduction_mode = D_IM_LTM_RBK_RDC_MODE_DIV_8,
	},
	[1] = {	// min
		.line_intr = 0,
		.burst_length = D_IM_LTM_BRST_512,
		.mask_write = D_IM_LTM_MASK_WRITE_ENABLE,
		.out_select = D_IM_LTM_RBK_OUT_SDRAM,
		.data_type = D_IM_LTM_DTYP_PACK12,
		.reduction_enable = D_IM_LTM_ENABLE_OFF,
		.reduction_mode = D_IM_LTM_RBK_RDC_MODE_DIV_2,
	},
};

static T_IM_LTM_RBK_CTRL_SDRAM_INPUT gct_im_ltm_rbk_ctrl_sdram_in[] = {
	[0] = {	// max
		.burst_length_r = D_IM_LTM_BRST_256,
		.burst_length_g = D_IM_LTM_BRST_256,
		.burst_length_b = D_IM_LTM_BRST_256,
		.input_global = 0xFFF0,
		.input_rect = {
			.img_top = 8888,
			.img_left = 2264,
			.img_width = 2360,
			.img_lines = 8960,
		},
		.shift_enable = D_IM_LTM_ENABLE_ON,
	},
	[1] = {	// min
		.burst_length_r = D_IM_LTM_BRST_512,
		.burst_length_g = D_IM_LTM_BRST_512,
		.burst_length_b = D_IM_LTM_BRST_512,
		.input_global = 0,
		.input_rect = {
			.img_top = 0,
			.img_left = 0,
			.img_width = 96,
			.img_lines = 72,
		},
		.shift_enable = D_IM_LTM_ENABLE_OFF,
	},
};

static T_IM_LTM_RBK_CTRL_SRO_DIRECT gct_im_ltm_rbk_ctrl_sro_direct[] = {
	[0] = {	// max
		.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_B,
		.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_B,
		.sro_mode = D_IM_LTM_RBK_SRO_DIRECT_2,
		.shift_enable0 = D_IM_LTM_ENABLE_ON,
		.shift_enable1 = D_IM_LTM_ENABLE_ON,
		.half_reduction_enable = D_IM_LTM_ENABLE_ON,
		.bayer_trim0 = {
			.img_top = 4095,
			.img_left = 1023,
			.img_width = 2360,
			.img_lines = 8960,
		},
		.bayer_trim1 = {
			.img_top = 4095,
			.img_left = 1023,
			.img_width = 2360,
			.img_lines = 8960,
		},
	},
	[1] = {	// min
		.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_R,
		.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_R,
		.sro_mode = D_IM_LTM_RBK_SRO_DIRECT_1,
		.shift_enable0 = D_IM_LTM_ENABLE_OFF,
		.shift_enable1 = D_IM_LTM_ENABLE_OFF,
		.half_reduction_enable = D_IM_LTM_ENABLE_OFF,
		.bayer_trim0 = {
			.img_top = 0,
			.img_left = 0,
			.img_width = 96,
			.img_lines = 72,
		},
		.bayer_trim1 = {
			.img_top = 0,
			.img_left = 0,
			.img_width = 96,
			.img_lines = 72,
		},
	},
};

// TestBase table
static T_IM_LTM_RBK_CTRL_COMMON gct_im_ltm_rbk_ctrl_common_base = {
	.line_intr = 0,
	.burst_length = D_IM_LTM_BRST_512,
	.mask_write = D_IM_LTM_MASK_WRITE_ENABLE,
	.out_select = D_IM_LTM_RBK_OUT_SDRAM,
	.data_type = D_IM_LTM_DTYP_UNPACK12,
	.reduction_enable = D_IM_LTM_ENABLE_ON,
	.reduction_mode = D_IM_LTM_RBK_RDC_MODE_DIV_4,
	.rbk_user_handler = ct_im_ltm_rbk_handler,
	.user_param = 0,
};

static T_IM_LTM_RBK_CTRL_SDRAM_INPUT gct_im_ltm_rbk_ctrl_sdram_in_base = {
	.burst_length_r = D_IM_LTM_BRST_512,
	.burst_length_g = D_IM_LTM_BRST_512,
	.burst_length_b = D_IM_LTM_BRST_512,
	.input_global = D_IM_LTM_RBK_SRC_IMG_RGB_GLOBAL_WIDTH,
	.input_rect = {
		.img_top = 0,
		.img_left = 0,
		.img_width = D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH,
		.img_lines = D_IM_LTM_RBK_SRC_IMG_PIXS_LINES,
	},
	.shift_enable = D_IM_LTM_ENABLE_ON,
};

static T_IM_LTM_RBK_CTRL_SRO_DIRECT gct_im_ltm_rbk_ctrl_sro_direct_base = {
	.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_R,
	.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_R,
	.sro_mode = D_IM_LTM_RBK_SRO_DIRECT_1,
	.shift_enable0 = D_IM_LTM_ENABLE_ON,
	.shift_enable1 = D_IM_LTM_ENABLE_OFF,
	.half_reduction_enable = D_IM_LTM_ENABLE_OFF,
	.bayer_trim0 = {
		.img_top = 0,
		.img_left = 0,
		.img_width = D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH,
		.img_lines = D_IM_LTM_RBK_SRC_IMG_PIXS_LINES,
	},
	.bayer_trim1 = {
		.img_top = 0,
		.img_left = 0,
		.img_width = 0,
		.img_lines = 0,
	},
};

static T_IM_LTM_INADDR_INFO gct_im_ltm_rbk_in_addr_base = {
	.addr_r = (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP,
	.addr_g = (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_G_ADDR_TOP,
	.addr_b = (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_B_ADDR_TOP,
};

static T_IM_LTM_RBK_OUTDATA_INFO gct_im_ltm_rbk_outdata_base = {
	.addr = (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP,
	.global_width = IM_LTM_RBK_ROUNDUP_N( D_IM_LTM_RBK_DST_IMG_GLOBAL_WIDTH_UP, 512 ),
};


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef IM_LTM_STATUS_PRINT
extern VOID Im_LTM_RBK_Print_Status( VOID );
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define ct_im_ltm_rbk_check_target_pipe_no_1(a)		(((a)+1) & (D_IM_LTM_PIPE1+1))
#define ct_im_ltm_rbk_check_target_pipe_no_2(a)		(((a)+1) & (D_IM_LTM_PIPE2+1))


static VOID ct_im_ltm_rbk_handler( UINT32* result, UINT32 user_param )
{
	if( ((*result) & D_IM_LTM_RBK1_INT_STATE_RBK_END) != 0 ) {
		Ddim_Print(( "LTM RBK1 end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_RBK1_INT_STATE_FRAME_END) != 0 ) {
		Ddim_Print(( "LTM RBK1 Receiving the top of the frame. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_RBK1_INT_STATE_LINE_END) != 0 ) {
		Ddim_Print(( "LTM RBK1 line end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_RBK1_INT_STATE_WAIT_ERR) != 0 ) {
		Ddim_Print(( "LTM RBK1 overrun error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_RBK1_INT_STATE_AXR_ERR) != 0 ) {
		Ddim_Print(( "LTM RBK1 AXR error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_RBK1_INT_STATE_AXW_ERR) != 0 ) {
		Ddim_Print(( "LTM RBK1 AXW error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_RBK2_INT_STATE_RBK_END) != 0 ) {
		Ddim_Print(( "LTM RBK2 end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_RBK2_INT_STATE_FRAME_END) != 0 ) {
		Ddim_Print(( "LTM RBK2 Receiving the top of the frame. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_RBK2_INT_STATE_LINE_END) != 0 ) {
		Ddim_Print(( "LTM RBK2 line end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_RBK2_INT_STATE_WAIT_ERR) != 0 ) {
		Ddim_Print(( "LTM RBK2 overrun error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_RBK2_INT_STATE_AXR_ERR) != 0 ) {
		Ddim_Print(( "LTM RBK2 AXR error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_RBK2_INT_STATE_AXW_ERR) != 0 ) {
		Ddim_Print(( "LTM RBK2 AXW error! 0x%x\n", user_param ));
	}
}

static VOID ct_im_ltm_rbk_print_ctrl_common_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.RBK.RLINTLV.bit.LINTLV   = 0x%x\n", IO_LTM_P1.RBK.RLINTLV.bit.LINTLV   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTW  = 0x%x\n", IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTW  ));
	Ddim_Print(( "IO_LTM_P1.RBK.RAXIBSIZ.bit.WSTBMSK = 0x%x\n", IO_LTM_P1.RBK.RAXIBSIZ.bit.WSTBMSK ));
	Ddim_Print(( "IO_LTM_P1.RBK.RIOCTL.bit.OUTSEL    = 0x%x\n", IO_LTM_P1.RBK.RIOCTL.bit.OUTSEL    ));
	Ddim_Print(( "IO_LTM_P1.RBK.RIOCTL.bit.W0DTYP    = 0x%x\n", IO_LTM_P1.RBK.RIOCTL.bit.W0DTYP    ));
	Ddim_Print(( "IO_LTM_P1.RBK.RDCMD.bit.RDCEN      = 0x%x\n", IO_LTM_P1.RBK.RDCMD.bit.RDCEN      ));
	Ddim_Print(( "IO_LTM_P1.RBK.RDCMD.bit.RDCMD      = 0x%x\n", IO_LTM_P1.RBK.RDCMD.bit.RDCMD      ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.RBK.RLINTLV.bit.LINTLV   = 0x%x\n", IO_LTM_P2.RBK.RLINTLV.bit.LINTLV   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTW  = 0x%x\n", IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTW  ));
	Ddim_Print(( "IO_LTM_P2.RBK.RAXIBSIZ.bit.WSTBMSK = 0x%x\n", IO_LTM_P2.RBK.RAXIBSIZ.bit.WSTBMSK ));
	Ddim_Print(( "IO_LTM_P2.RBK.RIOCTL.bit.OUTSEL    = 0x%x\n", IO_LTM_P2.RBK.RIOCTL.bit.OUTSEL    ));
	Ddim_Print(( "IO_LTM_P2.RBK.RIOCTL.bit.W0DTYP    = 0x%x\n", IO_LTM_P2.RBK.RIOCTL.bit.W0DTYP    ));
	Ddim_Print(( "IO_LTM_P2.RBK.RDCMD.bit.RDCEN      = 0x%x\n", IO_LTM_P2.RBK.RDCMD.bit.RDCEN      ));
	Ddim_Print(( "IO_LTM_P2.RBK.RDCMD.bit.RDCMD      = 0x%x\n", IO_LTM_P2.RBK.RDCMD.bit.RDCMD      ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_rbk_print_ctrl_sdram_input_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRR = 0x%x\n", IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRR ));
	Ddim_Print(( "IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRG = 0x%x\n", IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRG ));
	Ddim_Print(( "IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRB = 0x%x\n", IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRB ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRDEF.bit.SDRDEF   = 0x%x\n", IO_LTM_P1.RBK.RSDRDEF.bit.SDRDEF   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRHSZ.bit.SDRHSZ   = 0x%x\n", IO_LTM_P1.RBK.RSDRHSZ.bit.SDRHSZ   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRVSZ.bit.SDRVSZ   = 0x%x\n", IO_LTM_P1.RBK.RSDRVSZ.bit.SDRVSZ   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RIOCTL.bit.INSEL     = 0x%x\n", IO_LTM_P1.RBK.RIOCTL.bit.INSEL     ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSFEN.bit.RSFEN      = 0x%x\n", IO_LTM_P1.RBK.RSFEN.bit.RSFEN      ));
	Ddim_Print(( "IO_LTM_P1.RBK.HVEN.bit.HVEN        = 0x%x\n", IO_LTM_P1.RBK.HVEN.bit.HVEN        ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRR = 0x%x\n", IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRR ));
	Ddim_Print(( "IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRG = 0x%x\n", IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRG ));
	Ddim_Print(( "IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRB = 0x%x\n", IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRB ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRDEF.bit.SDRDEF   = 0x%x\n", IO_LTM_P2.RBK.RSDRDEF.bit.SDRDEF   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRHSZ.bit.SDRHSZ   = 0x%x\n", IO_LTM_P2.RBK.RSDRHSZ.bit.SDRHSZ   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRVSZ.bit.SDRVSZ   = 0x%x\n", IO_LTM_P2.RBK.RSDRVSZ.bit.SDRVSZ   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RIOCTL.bit.INSEL     = 0x%x\n", IO_LTM_P2.RBK.RIOCTL.bit.INSEL     ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSFEN.bit.RSFEN      = 0x%x\n", IO_LTM_P2.RBK.RSFEN.bit.RSFEN      ));
	Ddim_Print(( "IO_LTM_P2.RBK.HVEN.bit.HVEN        = 0x%x\n", IO_LTM_P2.RBK.HVEN.bit.HVEN        ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_rbk_print_sro_direct_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.RBK.RIOCTL.bit.INSEL    = 0x%x\n", IO_LTM_P1.RBK.RIOCTL.bit.INSEL    ));
	Ddim_Print(( "IO_LTM_P1.RBK.RIOCTL.bit.BAYER0   = 0x%x\n", IO_LTM_P1.RBK.RIOCTL.bit.BAYER0   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RIOCTL.bit.BAYER1   = 0x%x\n", IO_LTM_P1.RBK.RIOCTL.bit.BAYER1   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RIOCTL.bit.SROMD    = 0x%x\n", IO_LTM_P1.RBK.RIOCTL.bit.SROMD    ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSFEN.bit.RSFEN0    = 0x%x\n", IO_LTM_P1.RBK.RSFEN.bit.RSFEN0    ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSFEN.bit.RSFEN1    = 0x%x\n", IO_LTM_P1.RBK.RSFEN.bit.RSFEN1    ));
	Ddim_Print(( "IO_LTM_P1.RBK.HVEN.bit.HVEN       = 0x%x\n", IO_LTM_P1.RBK.HVEN.bit.HVEN       ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTHSTA0.bit.BTHSTA0 = 0x%x\n", IO_LTM_P1.RBK.BTHSTA0.bit.BTHSTA0 ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTVSTA0.bit.BTVSTA0 = 0x%x\n", IO_LTM_P1.RBK.BTVSTA0.bit.BTVSTA0 ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTHSIZ0.bit.BTHSIZ0 = 0x%x\n", IO_LTM_P1.RBK.BTHSIZ0.bit.BTHSIZ0 ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTVSIZ0.bit.BTVSIZ0 = 0x%x\n", IO_LTM_P1.RBK.BTVSIZ0.bit.BTVSIZ0 ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTHSTA1.bit.BTHSTA1 = 0x%x\n", IO_LTM_P1.RBK.BTHSTA1.bit.BTHSTA1 ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTVSTA1.bit.BTVSTA1 = 0x%x\n", IO_LTM_P1.RBK.BTVSTA1.bit.BTVSTA1 ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTHSIZ1.bit.BTHSIZ1 = 0x%x\n", IO_LTM_P1.RBK.BTHSIZ1.bit.BTHSIZ1 ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTVSIZ1.bit.BTVSIZ1 = 0x%x\n", IO_LTM_P1.RBK.BTVSIZ1.bit.BTVSIZ1 ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.RBK.RIOCTL.bit.INSEL    = 0x%x\n", IO_LTM_P2.RBK.RIOCTL.bit.INSEL    ));
	Ddim_Print(( "IO_LTM_P2.RBK.RIOCTL.bit.BAYER0   = 0x%x\n", IO_LTM_P2.RBK.RIOCTL.bit.BAYER0   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RIOCTL.bit.BAYER1   = 0x%x\n", IO_LTM_P2.RBK.RIOCTL.bit.BAYER1   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RIOCTL.bit.SROMD    = 0x%x\n", IO_LTM_P2.RBK.RIOCTL.bit.SROMD    ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSFEN.bit.RSFEN0    = 0x%x\n", IO_LTM_P2.RBK.RSFEN.bit.RSFEN0    ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSFEN.bit.RSFEN1    = 0x%x\n", IO_LTM_P2.RBK.RSFEN.bit.RSFEN1    ));
	Ddim_Print(( "IO_LTM_P2.RBK.HVEN.bit.HVEN       = 0x%x\n", IO_LTM_P2.RBK.HVEN.bit.HVEN       ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTHSTA0.bit.BTHSTA0 = 0x%x\n", IO_LTM_P2.RBK.BTHSTA0.bit.BTHSTA0 ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTVSTA0.bit.BTVSTA0 = 0x%x\n", IO_LTM_P2.RBK.BTVSTA0.bit.BTVSTA0 ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTHSIZ0.bit.BTHSIZ0 = 0x%x\n", IO_LTM_P2.RBK.BTHSIZ0.bit.BTHSIZ0 ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTVSIZ0.bit.BTVSIZ0 = 0x%x\n", IO_LTM_P2.RBK.BTVSIZ0.bit.BTVSIZ0 ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTHSTA1.bit.BTHSTA1 = 0x%x\n", IO_LTM_P2.RBK.BTHSTA1.bit.BTHSTA1 ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTVSTA1.bit.BTVSTA1 = 0x%x\n", IO_LTM_P2.RBK.BTVSTA1.bit.BTVSTA1 ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTHSIZ1.bit.BTHSIZ1 = 0x%x\n", IO_LTM_P2.RBK.BTHSIZ1.bit.BTHSIZ1 ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTVSIZ1.bit.BTVSIZ1 = 0x%x\n", IO_LTM_P2.RBK.BTVSIZ1.bit.BTVSIZ1 ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_rbk_print_all_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.RBK.RBKSR     = 0x%lx\n", IO_LTM_P1.RBK.RBKSR.word     ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSRAMEN   = 0x%lx\n", IO_LTM_P1.RBK.RSRAMEN.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RTRG      = 0x%lx\n", IO_LTM_P1.RBK.RTRG.word      ));
	Ddim_Print(( "IO_LTM_P1.RBK.RFRSTP    = 0x%lx\n", IO_LTM_P1.RBK.RFRSTP.word    ));
	Ddim_Print(( "IO_LTM_P1.RBK.RPARMSK   = 0x%lx\n", IO_LTM_P1.RBK.RPARMSK.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RINT0     = 0x%lx\n", IO_LTM_P1.RBK.RINT0.word     ));
	Ddim_Print(( "IO_LTM_P1.RBK.RINT1     = 0x%lx\n", IO_LTM_P1.RBK.RINT1.word     ));
	Ddim_Print(( "IO_LTM_P1.RBK.RLINTLV   = 0x%lx\n", IO_LTM_P1.RBK.RLINTLV.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RAXICTLR1 = 0x%lx\n", IO_LTM_P1.RBK.RAXICTLR1.word ));
	Ddim_Print(( "IO_LTM_P1.RBK.RAXIRERR  = 0x%lx\n", IO_LTM_P1.RBK.RAXIRERR.word  ));
	Ddim_Print(( "IO_LTM_P1.RBK.RAXICTLW1 = 0x%lx\n", IO_LTM_P1.RBK.RAXICTLW1.word ));
	Ddim_Print(( "IO_LTM_P1.RBK.RAXIWERR  = 0x%lx\n", IO_LTM_P1.RBK.RAXIWERR.word  ));
	Ddim_Print(( "IO_LTM_P1.RBK.RAXIBSIZ  = 0x%lx\n", IO_LTM_P1.RBK.RAXIBSIZ.word  ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRHSZ   = 0x%lx\n", IO_LTM_P1.RBK.RSDRHSZ.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRDEF   = 0x%lx\n", IO_LTM_P1.RBK.RSDRDEF.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRVSZ   = 0x%lx\n", IO_LTM_P1.RBK.RSDRVSZ.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRADR   = 0x%lx\n", IO_LTM_P1.RBK.RSDRADR.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRADG   = 0x%lx\n", IO_LTM_P1.RBK.RSDRADG.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRADB   = 0x%lx\n", IO_LTM_P1.RBK.RSDRADB.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDWAD    = 0x%lx\n", IO_LTM_P1.RBK.RSDWAD.word    ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDWDEF   = 0x%lx\n", IO_LTM_P1.RBK.RSDWDEF.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RIOCTL    = 0x%lx\n", IO_LTM_P1.RBK.RIOCTL.word    ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTHSTA0   = 0x%lx\n", IO_LTM_P1.RBK.BTHSTA0.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTVSTA0   = 0x%lx\n", IO_LTM_P1.RBK.BTVSTA0.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTHSIZ0   = 0x%lx\n", IO_LTM_P1.RBK.BTHSIZ0.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTVSIZ0   = 0x%lx\n", IO_LTM_P1.RBK.BTVSIZ0.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTHSTA1   = 0x%lx\n", IO_LTM_P1.RBK.BTHSTA1.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTVSTA1   = 0x%lx\n", IO_LTM_P1.RBK.BTVSTA1.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTHSIZ1   = 0x%lx\n", IO_LTM_P1.RBK.BTHSIZ1.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.BTVSIZ1   = 0x%lx\n", IO_LTM_P1.RBK.BTVSIZ1.word   ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSFEN     = 0x%lx\n", IO_LTM_P1.RBK.RSFEN.word     ));
	Ddim_Print(( "IO_LTM_P1.RBK.HVEN      = 0x%lx\n", IO_LTM_P1.RBK.HVEN.word      ));
	Ddim_Print(( "IO_LTM_P1.RBK.RHSTA     = 0x%lx\n", IO_LTM_P1.RBK.RHSTA.word     ));
	Ddim_Print(( "IO_LTM_P1.RBK.RHPIT     = 0x%lx\n", IO_LTM_P1.RBK.RHPIT.word     ));
	Ddim_Print(( "IO_LTM_P1.RBK.RVSTA     = 0x%lx\n", IO_LTM_P1.RBK.RVSTA.word     ));
	Ddim_Print(( "IO_LTM_P1.RBK.RVPIT     = 0x%lx\n", IO_LTM_P1.RBK.RVPIT.word     ));
	Ddim_Print(( "IO_LTM_P1.RBK.RDCMD     = 0x%lx\n", IO_LTM_P1.RBK.RDCMD.word     ));
	Ddim_Print(( "IO_LTM_P1.RBK.RTEN      = 0x%lx\n", IO_LTM_P1.RBK.RTEN.word      ));
	Ddim_Print(( "IO_LTM_P1.RBK.RTHSTA    = 0x%lx\n", IO_LTM_P1.RBK.RTHSTA.word    ));
	Ddim_Print(( "IO_LTM_P1.RBK.RTVSTA    = 0x%lx\n", IO_LTM_P1.RBK.RTVSTA.word    ));
	Ddim_Print(( "IO_LTM_P1.RBK.RTHSIZ    = 0x%lx\n", IO_LTM_P1.RBK.RTHSIZ.word    ));
	Ddim_Print(( "IO_LTM_P1.RBK.RTVSIZ    = 0x%lx\n", IO_LTM_P1.RBK.RTVSIZ.word    ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.RBK.RBKSR     = 0x%lx\n", IO_LTM_P2.RBK.RBKSR.word     ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSRAMEN   = 0x%lx\n", IO_LTM_P2.RBK.RSRAMEN.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RTRG      = 0x%lx\n", IO_LTM_P2.RBK.RTRG.word      ));
	Ddim_Print(( "IO_LTM_P2.RBK.RFRSTP    = 0x%lx\n", IO_LTM_P2.RBK.RFRSTP.word    ));
	Ddim_Print(( "IO_LTM_P2.RBK.RPARMSK   = 0x%lx\n", IO_LTM_P2.RBK.RPARMSK.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RINT0     = 0x%lx\n", IO_LTM_P2.RBK.RINT0.word     ));
	Ddim_Print(( "IO_LTM_P2.RBK.RINT1     = 0x%lx\n", IO_LTM_P2.RBK.RINT1.word     ));
	Ddim_Print(( "IO_LTM_P2.RBK.RLINTLV   = 0x%lx\n", IO_LTM_P2.RBK.RLINTLV.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RAXICTLR1 = 0x%lx\n", IO_LTM_P2.RBK.RAXICTLR1.word ));
	Ddim_Print(( "IO_LTM_P2.RBK.RAXIRERR  = 0x%lx\n", IO_LTM_P2.RBK.RAXIRERR.word  ));
	Ddim_Print(( "IO_LTM_P2.RBK.RAXICTLW1 = 0x%lx\n", IO_LTM_P2.RBK.RAXICTLW1.word ));
	Ddim_Print(( "IO_LTM_P2.RBK.RAXIWERR  = 0x%lx\n", IO_LTM_P2.RBK.RAXIWERR.word  ));
	Ddim_Print(( "IO_LTM_P2.RBK.RAXIBSIZ  = 0x%lx\n", IO_LTM_P2.RBK.RAXIBSIZ.word  ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRHSZ   = 0x%lx\n", IO_LTM_P2.RBK.RSDRHSZ.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRDEF   = 0x%lx\n", IO_LTM_P2.RBK.RSDRDEF.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRVSZ   = 0x%lx\n", IO_LTM_P2.RBK.RSDRVSZ.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRADR   = 0x%lx\n", IO_LTM_P2.RBK.RSDRADR.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRADG   = 0x%lx\n", IO_LTM_P2.RBK.RSDRADG.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRADB   = 0x%lx\n", IO_LTM_P2.RBK.RSDRADB.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDWAD    = 0x%lx\n", IO_LTM_P2.RBK.RSDWAD.word    ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDWDEF   = 0x%lx\n", IO_LTM_P2.RBK.RSDWDEF.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RIOCTL    = 0x%lx\n", IO_LTM_P2.RBK.RIOCTL.word    ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTHSTA0   = 0x%lx\n", IO_LTM_P2.RBK.BTHSTA0.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTVSTA0   = 0x%lx\n", IO_LTM_P2.RBK.BTVSTA0.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTHSIZ0   = 0x%lx\n", IO_LTM_P2.RBK.BTHSIZ0.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTVSIZ0   = 0x%lx\n", IO_LTM_P2.RBK.BTVSIZ0.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTHSTA1   = 0x%lx\n", IO_LTM_P2.RBK.BTHSTA1.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTVSTA1   = 0x%lx\n", IO_LTM_P2.RBK.BTVSTA1.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTHSIZ1   = 0x%lx\n", IO_LTM_P2.RBK.BTHSIZ1.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.BTVSIZ1   = 0x%lx\n", IO_LTM_P2.RBK.BTVSIZ1.word   ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSFEN     = 0x%lx\n", IO_LTM_P2.RBK.RSFEN.word     ));
	Ddim_Print(( "IO_LTM_P2.RBK.HVEN      = 0x%lx\n", IO_LTM_P2.RBK.HVEN.word      ));
	Ddim_Print(( "IO_LTM_P2.RBK.RHSTA     = 0x%lx\n", IO_LTM_P2.RBK.RHSTA.word     ));
	Ddim_Print(( "IO_LTM_P2.RBK.RHPIT     = 0x%lx\n", IO_LTM_P2.RBK.RHPIT.word     ));
	Ddim_Print(( "IO_LTM_P2.RBK.RVSTA     = 0x%lx\n", IO_LTM_P2.RBK.RVSTA.word     ));
	Ddim_Print(( "IO_LTM_P2.RBK.RVPIT     = 0x%lx\n", IO_LTM_P2.RBK.RVPIT.word     ));
	Ddim_Print(( "IO_LTM_P2.RBK.RDCMD     = 0x%lx\n", IO_LTM_P2.RBK.RDCMD.word     ));
	Ddim_Print(( "IO_LTM_P2.RBK.RTEN      = 0x%lx\n", IO_LTM_P2.RBK.RTEN.word      ));
	Ddim_Print(( "IO_LTM_P2.RBK.RTHSTA    = 0x%lx\n", IO_LTM_P2.RBK.RTHSTA.word    ));
	Ddim_Print(( "IO_LTM_P2.RBK.RTVSTA    = 0x%lx\n", IO_LTM_P2.RBK.RTVSTA.word    ));
	Ddim_Print(( "IO_LTM_P2.RBK.RTHSIZ    = 0x%lx\n", IO_LTM_P2.RBK.RTHSIZ.word    ));
	Ddim_Print(( "IO_LTM_P2.RBK.RTVSIZ    = 0x%lx\n", IO_LTM_P2.RBK.RTVSIZ.word    ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_rbk_sro_prch_int_handler( T_CALLBACK_ID* cb_id, ULONG result, ULONG user_param )
{
	Ddim_Print(( "PRch Int %u, %u, %u %lx, %lx\n", cb_id->unit_no, cb_id->block_type, cb_id->channel, result, user_param ));
}

static volatile BOOL gct_im_ltm_rbk_sro_vd_flg = FALSE;
static VOID ct_im_ltm_rbk_sro_vd_int_handler( T_CALLBACK_ID* cb_id, ULONG result, ULONG user_param )
{
	Ddim_Print(( "VD Int %u, %u, %u %lx, %lx\n", cb_id->unit_no, cb_id->block_type, cb_id->channel, result, user_param ));
	gct_im_ltm_rbk_sro_vd_flg = TRUE;
}

static VOID ct_im_ltm_rbk_set_sro( E_IM_PRO_UNIT_NUM unit_no, UCHAR test_ptn )
{
	T_IM_PRO_SROTOP_CTRL		sro_ctrl;
	T_IM_PRO_PG_CTRL			pg_ctrl;
	T_IM_PRO_M2P_CTRL			m2p_ctrl;
	T_IM_PRO_PRCH_CTRL			pr_ctrl;
	T_IM_PRO_PRCH_AREA			pr_area = {
		.width = 1280 * 1.5,				// bytes
		.height = 960 / 2,
		.global_width = 1280 * 2 * 1.5,	// 12bit pack, Interlace
	};
	T_IM_PRO_INT_SROTOP_CTRL	int_ctrl;
	T_IM_PRO_CALLBACK_CFG		callback_cfg;

	if( test_ptn == 1 ){
		pr_area.width  = 4680 * 3 / 2;
		pr_area.height = 3500 / 2;
		pr_area.global_width = 4680 * 2 * 3 / 2;
	}

	Im_PRO_SROTOP_Init( unit_no );

//#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Dd_Top_Set_CLKSTOP10_SRO1CK( 1 );
	Dd_Top_Set_CLKSTOP12_SRO2CK( 1 );
	Dd_Top_Set_CLKSEL5_SRO1CLK( 0 );		// 300MHz
	Dd_Top_Set_CLKSEL7_SRO2CLK( 0 );		// 300MHz
	Dd_Top_Set_CLKSTOP10_SRO1CK( 0 );
	Dd_Top_Set_CLKSTOP12_SRO2CK( 0 );

	Ddim_Print(( "SRO1 CLK %u\n", Dd_Top_Get_CLKSEL5_SRO1CLK() ));
	Ddim_Print(( "SRO2 CLK %u\n", Dd_Top_Get_CLKSEL7_SRO2CLK() ));

	Dd_Top_Set_CLKSTOP10_LTMRBK1CK( 1 );
	Dd_Top_Set_CLKSTOP12_LTMRBK2CK( 1 );
	Dd_Top_Set_CLKSEL5_LTMRBK1CLK( 8 );		// SRO1CLK
	Dd_Top_Set_CLKSEL7_LTMRBK2CLK( 8 );		// SRO2CLK
	Dd_Top_Set_CLKSTOP12_LTMRBK2CK( 0 );
	Dd_Top_Set_CLKSTOP10_LTMRBK1CK( 0 );
//#endif

	Im_PRO_SROTOP_Control_Clock( unit_no, E_IM_PRO_SROTOP_CLK_TYPE_SROCLK, D_IM_PRO_CLOCK_ON, 1 );

	// B2BTOP(and PAT) settings
	sro_ctrl.input_mode				= E_IM_PRO_SROTOP_INPUT_MODE_REINPUT;
	sro_ctrl.sro_mode_sel			= E_IM_PRO_SROTOP_MODE0;
	sro_ctrl.cag_target_sel			= E_IM_PRO_SROTOP_CAG_TARGET_BLOCK_SRO;
	sro_ctrl.bay_out_path_sel		= E_IM_PRO_SROTOP_PATH_SEL_BAY_OUT_WITHOUT_BR;
	sro_ctrl.b2b_connect_path_sel	= E_IM_PRO_SROTOP_PATH_SEL_B2B_CONN_WITHOUT_BR;
	sro_ctrl.ltm_connect_path_sel	= E_IM_PRO_SROTOP_PATH_SEL_LTM_CONN_WITHOUT_BR;
	sro_ctrl.first_pixel[0]			= E_IM_PRO_ORG_R;
	sro_ctrl.first_pixel[1]			= E_IM_PRO_ORG_GB;
	Im_PRO_SROTOP_Ctrl( unit_no, &sro_ctrl );

	// PG settings
	pg_ctrl.mode				= E_IM_PRO_PG_MODE_2LINE;
	if( unit_no == E_IM_PRO_BOTH_UNIT ){
		pg_ctrl.input_img_width		= D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH / 2;
	}
	else{
		pg_ctrl.input_img_width		= D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH;
	}
	pg_ctrl.input_img_lines		= D_IM_LTM_RBK_SRC_IMG_PIXS_LINES / 2;
	if( test_ptn == 1 ){
		if( unit_no == E_IM_PRO_BOTH_UNIT ){
			pg_ctrl.input_img_width		= 4680 / 2;
		}
		else{
			pg_ctrl.input_img_width		= 4680;
		}
		pg_ctrl.input_img_lines		= 3500 / 2;
	}
	pg_ctrl.vertical_blanking	= 200;		// branking line after image
	pg_ctrl.horizontal_blanking	= 1000;		// branking line after image
	pg_ctrl.start_blanking		= 100;		// branking line before image
	Im_PRO_PG_Ctrl( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, &pg_ctrl );

	// M2P settings
	m2p_ctrl.data_type				= E_IM_PRO_M2P_DATA_TYPE_12BIT;
	m2p_ctrl.deknee_enable			= E_IM_PRO_M2P_DEKNEE_DIS;
	m2p_ctrl.shift_bit				= E_IM_PRO_M2P_SHIFT_0BIT;
	m2p_ctrl.shift_sat				= E_IM_PRO_M2P_SAT_NOCOMP;
	m2p_ctrl.bit_shift				= E_IM_PRO_M2P_BIT_SHIFT_L;
	m2p_ctrl.valid_bit_count		= 8;	// dummy
	m2p_ctrl.shift_value			= 0;
	m2p_ctrl.upper_limit_clip_value	= 0xFFFF;
	m2p_ctrl.lower_limit_clip_value	= 0;
	Im_PRO_M2P_Ctrl( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_0, &m2p_ctrl );
	Im_PRO_M2P_Ctrl( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_1, &m2p_ctrl );

	// PRch settings
	pr_ctrl.trans_mode		= E_IM_PRO_PRCH_BURST_512BYTE;
	pr_ctrl.req_threshold	= E_IM_PRO_PRCH_CNT_NOLIMIT;
	pr_ctrl.trans_to		= E_IM_PRO_PRCH_TRANS_IO_0;
	Im_PRO_PRch_Ctrl( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, &pr_ctrl );
	Im_PRO_PRch_Ctrl( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, &pr_ctrl );

	if( unit_no == E_IM_PRO_BOTH_UNIT ){
		pr_area.width /= 2;
	}

	if( unit_no == E_IM_PRO_BOTH_UNIT ){
		Im_PRO_PRch_Set_Addr( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP );
		Im_PRO_PRch_Set_Addr( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP + pr_area.global_width / 2 );
		Im_PRO_PRch_Set_Addr( E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP + pr_area.width );
		Im_PRO_PRch_Set_Addr( E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP + pr_area.width + pr_area.global_width / 2 );
	}
	else{
		Im_PRO_PRch_Set_Addr( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP );
		Im_PRO_PRch_Set_Addr( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP + pr_area.global_width / 2 );
	}

	Im_PRO_PRch_Set_Area( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, &pr_area );
	Im_PRO_PRch_Set_Area( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, &pr_area );

	int_ctrl.int_mode						= E_IM_PRO_INT_INTMD_OR;
	int_ctrl.vd_enable[0].interrupt_bit		= D_IM_PRO_SROTOP_INT_VDE0;
	int_ctrl.vd_enable[0].permission_flg	= 1;
	int_ctrl.hd_enable[0].interrupt_bit		= 0;
	int_ctrl.hd_enable[0].permission_flg	= 0;
	int_ctrl.vd_enable[1].interrupt_bit		= D_IM_PRO_SROTOP_INT_VDE1;
	int_ctrl.vd_enable[1].permission_flg	= 1;
	int_ctrl.hd_enable[1].interrupt_bit		= 0;
	int_ctrl.hd_enable[1].permission_flg	= 0;
	Im_PRO_SROTOP_Interrupt_Ctrl( unit_no, &int_ctrl );

	callback_cfg.inthandler	= ct_im_ltm_rbk_sro_vd_int_handler;
	callback_cfg.user_param	= 0;
	Im_PRO_SROTOP_Pipe_Set_VD_Int_Handler( E_IM_PRO_UNIT_NUM_1, &callback_cfg );

	callback_cfg.inthandler	= ct_im_ltm_rbk_sro_prch_int_handler;
	callback_cfg.user_param	= 0;
	Im_PRO_PRch_Set_Int_Handler( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, &callback_cfg );
	Im_PRO_PRch_Set_Int_Handler( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, &callback_cfg );

}

static VOID ct_im_ltm_rbk_start_sro( E_IM_PRO_UNIT_NUM unit_no )
{
	gct_im_ltm_rbk_sro_vd_flg = FALSE;

	Im_PRO_PRch_Start( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0 );
	Im_PRO_PRch_Start( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1 );
	Im_PRO_M2P_Set_PAEN( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_0, E_IM_PRO_M2P_PAEN_EN );
	Im_PRO_M2P_Set_PAEN( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_1, E_IM_PRO_M2P_PAEN_EN );
	Im_PRO_M2P_Start( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_0 );
	Im_PRO_M2P_Start( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_1 );

	Im_PRO_PG_Start( unit_no, E_IM_PRO_BLOCK_TYPE_SRO );
}

static VOID ct_im_ltm_rbk_wait_vd_sro( VOID )
{
	UINT32 loopcnt;
#ifdef CO_DEBUG_ON_PC
	T_CALLBACK_ID cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};
#endif

	for( loopcnt = 0; gct_im_ltm_rbk_sro_vd_flg == FALSE; loopcnt++ ) {
		if( loopcnt >= 10 ) {
			Ddim_Print(( "SRO wait end timeout.\n" ));
			break;
		}

		DDIM_User_Dly_Tsk( 1 );
#ifdef CO_DEBUG_ON_PC
		ct_im_ltm_rbk_sro_vd_int_handler( &cb_id, 0, 0 );
#endif
	}

	gct_im_ltm_rbk_sro_vd_flg = FALSE;
}

static VOID ct_im_ltm_rbk_waitend_stop_sro( E_IM_PRO_UNIT_NUM unit_no )
{
	ct_im_ltm_rbk_wait_vd_sro();

	Im_PRO_PWch_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PWCH_0, 0 );
	Im_PRO_P2M_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_P2M_CH0, 0 );
	Im_PRO_PWch_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PWCH_1, 0 );
	Im_PRO_P2M_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_P2M_CH1, 0 );

	Im_PRO_PRch_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, 0 );
	Im_PRO_M2P_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_0, 0 );
	Im_PRO_PRch_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, 0 );
	Im_PRO_M2P_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_1, 0 );

	ct_im_ltm_rbk_wait_vd_sro();

	Im_PRO_PG_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_SRO );
}


#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_1: "
INT32 CT_Im_Ltm_Rbk_1_1( UCHAR pipe_no )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Init( pipe_no );

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.RBK.RBKSR   = 0x%lx\n", IO_LTM_P1.RBK.RBKSR.word ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSRAMEN = 0x%lx\n", IO_LTM_P1.RBK.RSRAMEN.word ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.RBK.RBKSR   = 0x%lx\n", IO_LTM_P2.RBK.RBKSR.word ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSRAMEN = 0x%lx\n", IO_LTM_P2.RBK.RSRAMEN.word ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_2: "
INT32 CT_Im_Ltm_Rbk_1_2( UCHAR pipe_no )
{
	UINT32					loopcnt;
	T_IM_LTM_RBK_CTRL_AXI	rbk_ctrl_axi[] = {
		[0] = {	// max
			.rch[0].cache_type = 0xF,
			.rch[0].protect_type = 7,
			.rch[1].cache_type = 0xF,
			.rch[1].protect_type = 7,
			.rch[2].cache_type = 0xF,
			.rch[2].protect_type = 7,
			.wch.cache_type = 0xF,
			.wch.protect_type = 7,
		},
		[1] = {	// min
			.rch[0].cache_type = 0,
			.rch[0].protect_type = 0,
			.rch[1].cache_type = 0,
			.rch[1].protect_type = 0,
			.rch[2].cache_type = 0,
			.rch[2].protect_type = 0,
			.wch.cache_type = 0,
			.wch.protect_type = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(rbk_ctrl_axi) / sizeof(rbk_ctrl_axi[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_RBK_Ctrl_Axi( pipe_no, &rbk_ctrl_axi[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.RBK.RAXICTLR1.bit.ARCACHE_R = %u\n", IO_LTM_P1.RBK.RAXICTLR1.bit.ARCACHE_R ));
		Ddim_Print(( "IO_LTM_P1.RBK.RAXICTLR1.bit.ARPROT_R  = %u\n", IO_LTM_P1.RBK.RAXICTLR1.bit.ARPROT_R  ));
		Ddim_Print(( "IO_LTM_P1.RBK.RAXICTLR1.bit.ARCACHE_G = %u\n", IO_LTM_P1.RBK.RAXICTLR1.bit.ARCACHE_G ));
		Ddim_Print(( "IO_LTM_P1.RBK.RAXICTLR1.bit.ARPROT_G  = %u\n", IO_LTM_P1.RBK.RAXICTLR1.bit.ARPROT_G  ));
		Ddim_Print(( "IO_LTM_P1.RBK.RAXICTLR1.bit.ARCACHE_B = %u\n", IO_LTM_P1.RBK.RAXICTLR1.bit.ARCACHE_B ));
		Ddim_Print(( "IO_LTM_P1.RBK.RAXICTLR1.bit.ARPROT_B  = %u\n", IO_LTM_P1.RBK.RAXICTLR1.bit.ARPROT_B  ));
		Ddim_Print(( "IO_LTM_P1.RBK.RAXICTLW1.bit.AWCACHE   = %u\n", IO_LTM_P1.RBK.RAXICTLW1.bit.AWCACHE   ));
		Ddim_Print(( "IO_LTM_P1.RBK.RAXICTLW1.bit.AWPROT    = %u\n", IO_LTM_P1.RBK.RAXICTLW1.bit.AWPROT    ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXICTLR1.bit.ARCACHE_R = %u\n", IO_LTM_P2.RBK.RAXICTLR1.bit.ARCACHE_R ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXICTLR1.bit.ARPROT_R  = %u\n", IO_LTM_P2.RBK.RAXICTLR1.bit.ARPROT_R  ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXICTLR1.bit.ARCACHE_G = %u\n", IO_LTM_P2.RBK.RAXICTLR1.bit.ARCACHE_G ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXICTLR1.bit.ARPROT_G  = %u\n", IO_LTM_P2.RBK.RAXICTLR1.bit.ARPROT_G  ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXICTLR1.bit.ARCACHE_B = %u\n", IO_LTM_P2.RBK.RAXICTLR1.bit.ARCACHE_B ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXICTLR1.bit.ARPROT_B  = %u\n", IO_LTM_P2.RBK.RAXICTLR1.bit.ARPROT_B  ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXICTLW1.bit.AWCACHE   = %u\n", IO_LTM_P2.RBK.RAXICTLW1.bit.AWCACHE   ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXICTLW1.bit.AWPROT    = %u\n", IO_LTM_P2.RBK.RAXICTLW1.bit.AWPROT    ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_3: "
INT32 CT_Im_Ltm_Rbk_1_3( UCHAR pipe_no )
{
	T_IM_LTM_RBK_AXI_RCH_STAT rbk_axi_read_stat;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.RBK.RAXIRERR.bit.RRESP_R = 3;
		IO_LTM_P1.RBK.RAXIRERR.bit.RRESP_G = 3;
		IO_LTM_P1.RBK.RAXIRERR.bit.RRESP_B = 3;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.RBK.RAXIRERR.bit.RRESP_R = 3;
		IO_LTM_P2.RBK.RAXIRERR.bit.RRESP_G = 3;
		IO_LTM_P2.RBK.RAXIRERR.bit.RRESP_B = 3;
	}
#endif // CO_DEBUG_ON_PC
	memset( &rbk_axi_read_stat, '\0', sizeof(rbk_axi_read_stat) );
	Im_LTM_RBK_Get_AxiReadStat( pipe_no, &rbk_axi_read_stat );
	Ddim_Print(( "rch_axi_stat[0] = %u\n", rbk_axi_read_stat.rch_axi_stat[0] ));
	Ddim_Print(( "rch_axi_stat[1] = %u\n", rbk_axi_read_stat.rch_axi_stat[1] ));
	Ddim_Print(( "rch_axi_stat[2] = %u\n", rbk_axi_read_stat.rch_axi_stat[2] ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.RBK.RAXIRERR.bit.RRESP_R = 0;
		IO_LTM_P1.RBK.RAXIRERR.bit.RRESP_G = 0;
		IO_LTM_P1.RBK.RAXIRERR.bit.RRESP_B = 0;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.RBK.RAXIRERR.bit.RRESP_R = 0;
		IO_LTM_P2.RBK.RAXIRERR.bit.RRESP_G = 0;
		IO_LTM_P2.RBK.RAXIRERR.bit.RRESP_B = 0;
	}
#endif // CO_DEBUG_ON_PC
	memset( &rbk_axi_read_stat, '\0', sizeof(rbk_axi_read_stat) );
	Im_LTM_RBK_Get_AxiReadStat( pipe_no, &rbk_axi_read_stat );
	Ddim_Print(( "rch_axi_stat[0] = %u\n", rbk_axi_read_stat.rch_axi_stat[0] ));
	Ddim_Print(( "rch_axi_stat[1] = %u\n", rbk_axi_read_stat.rch_axi_stat[1] ));
	Ddim_Print(( "rch_axi_stat[2] = %u\n", rbk_axi_read_stat.rch_axi_stat[2] ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_4: "
INT32 CT_Im_Ltm_Rbk_1_4( UCHAR pipe_no )
{
	T_IM_LTM_RBK_AXI_WCH_STAT rbk_axi_write_stat;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.RBK.RAXIWERR.bit.WRESP = 3;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.RBK.RAXIWERR.bit.WRESP = 3;
	}
#endif // CO_DEBUG_ON_PC
	memset( &rbk_axi_write_stat, '\0', sizeof(rbk_axi_write_stat) );
	Im_LTM_RBK_Get_AxiWriteStat( pipe_no, &rbk_axi_write_stat );
	Ddim_Print(( "wch_axi_stat = %u\n", rbk_axi_write_stat.wch_axi_stat ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.RBK.RAXIWERR.bit.WRESP = 0;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.RBK.RAXIWERR.bit.WRESP = 0;
	}
#endif // CO_DEBUG_ON_PC
	memset( &rbk_axi_write_stat, '\0', sizeof(rbk_axi_write_stat) );
	Im_LTM_RBK_Get_AxiWriteStat( pipe_no, &rbk_axi_write_stat );
	Ddim_Print(( "wch_axi_stat = %u\n", rbk_axi_write_stat.wch_axi_stat ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_5: "
INT32 CT_Im_Ltm_Rbk_1_5( UCHAR pipe_no )
{
	T_IM_LTM_RBK_AXI_CTRL_PARAM rbk_axi_ctrl_param;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRR = 1;
		IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRG = 1;
		IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRB = 1;
		IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTW  = 1;
		IO_LTM_P1.RBK.RAXIBSIZ.bit.WSTBMSK = 1;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRR = 1;
		IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRG = 1;
		IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRB = 1;
		IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTW  = 1;
		IO_LTM_P2.RBK.RAXIBSIZ.bit.WSTBMSK = 1;
	}
	memset( &rbk_axi_ctrl_param, '\0', sizeof(rbk_axi_ctrl_param) );
	Im_LTM_RBK_Get_AxiCtrlParam( pipe_no, &rbk_axi_ctrl_param );
	Ddim_Print(( "rch_burst_length_r = %u\n", rbk_axi_ctrl_param.rch_burst_length_r ));
	Ddim_Print(( "rch_burst_length_g = %u\n", rbk_axi_ctrl_param.rch_burst_length_g ));
	Ddim_Print(( "rch_burst_length_b = %u\n", rbk_axi_ctrl_param.rch_burst_length_b ));
	Ddim_Print(( "wch_burst_length   = %u\n", rbk_axi_ctrl_param.wch_burst_length   ));
	Ddim_Print(( "mask_write         = %u\n", rbk_axi_ctrl_param.mask_write         ));

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRR = 0;
		IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRG = 0;
		IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRB = 0;
		IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTW  = 0;
		IO_LTM_P1.RBK.RAXIBSIZ.bit.WSTBMSK = 0;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRR = 0;
		IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRG = 0;
		IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRB = 0;
		IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTW  = 0;
		IO_LTM_P2.RBK.RAXIBSIZ.bit.WSTBMSK = 0;
	}
	memset( &rbk_axi_ctrl_param, '\0', sizeof(rbk_axi_ctrl_param) );
	Im_LTM_RBK_Get_AxiCtrlParam( pipe_no, &rbk_axi_ctrl_param );
	Ddim_Print(( "rch_burst_length_r = %u\n", rbk_axi_ctrl_param.rch_burst_length_r ));
	Ddim_Print(( "rch_burst_length_g = %u\n", rbk_axi_ctrl_param.rch_burst_length_g ));
	Ddim_Print(( "rch_burst_length_b = %u\n", rbk_axi_ctrl_param.rch_burst_length_b ));
	Ddim_Print(( "wch_burst_length   = %u\n", rbk_axi_ctrl_param.wch_burst_length   ));
	Ddim_Print(( "mask_write         = %u\n", rbk_axi_ctrl_param.mask_write         ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_6: "
INT32 CT_Im_Ltm_Rbk_1_6( UCHAR pipe_no )
{
	UINT32						loopcnt;
	T_IM_LTM_RBK_AXI_CTRL_PARAM	rbk_axi_ctrl_param[] = {
		[0] = {	// max
			.rch_burst_length_r = 1,
			.rch_burst_length_g = 1,
			.rch_burst_length_b = 1,
			.wch_burst_length = 1,
			.mask_write = 1,
		},
		[1] = {	// min
			.rch_burst_length_r = 0,
			.rch_burst_length_g = 0,
			.rch_burst_length_b = 0,
			.wch_burst_length = 0,
			.mask_write = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(rbk_axi_ctrl_param) / sizeof(rbk_axi_ctrl_param[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_RBK_Set_AxiCtrlParam( pipe_no, &rbk_axi_ctrl_param[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRR = %u\n", IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRR ));
		Ddim_Print(( "IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRG = %u\n", IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRG ));
		Ddim_Print(( "IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRB = %u\n", IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTRB ));
		Ddim_Print(( "IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTW  = %u\n", IO_LTM_P1.RBK.RAXIBSIZ.bit.BURSTW  ));
		Ddim_Print(( "IO_LTM_P1.RBK.RAXIBSIZ.bit.WSTBMSK = %u\n", IO_LTM_P1.RBK.RAXIBSIZ.bit.WSTBMSK ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRR = %u\n", IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRR ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRG = %u\n", IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRG ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRB = %u\n", IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTRB ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTW  = %u\n", IO_LTM_P2.RBK.RAXIBSIZ.bit.BURSTW  ));
		Ddim_Print(( "IO_LTM_P2.RBK.RAXIBSIZ.bit.WSTBMSK = %u\n", IO_LTM_P2.RBK.RAXIBSIZ.bit.WSTBMSK ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_7: "
INT32 CT_Im_Ltm_Rbk_1_7( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(gct_im_ltm_rbk_ctrl_common) / sizeof(gct_im_ltm_rbk_ctrl_common[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_RBK_Ctrl_Common( pipe_no, &gct_im_ltm_rbk_ctrl_common[loopcnt] );

		ct_im_ltm_rbk_print_ctrl_common_reg();
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_8: "
INT32 CT_Im_Ltm_Rbk_1_8( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(gct_im_ltm_rbk_ctrl_sdram_in) / sizeof(gct_im_ltm_rbk_ctrl_sdram_in[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in[loopcnt] );

		ct_im_ltm_rbk_print_ctrl_sdram_input_reg();
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_9: "
INT32 CT_Im_Ltm_Rbk_1_9( UCHAR pipe_no )
{
	UINT32 loopcnt;
	T_IM_LTM_RBK_CTRL_SDRAM_INPUT rbk_ctrl_sdram_in = gct_im_ltm_rbk_ctrl_sdram_in_base;
	T_IM_LTM_INADDR_INFO rbk_in_addr[] = {
		[0] = { // max
			.addr_r = (VOID*)0xFFFFFFFE,
			.addr_g = (VOID*)0xFFFFFFFE,
			.addr_b = (VOID*)0xFFFFFFFE,
		},
		[1] = { // min
			.addr_r = (VOID*)0,
			.addr_g = (VOID*)0,
			.addr_b = (VOID*)0,
		},
		[2] = { // indvisual
			.addr_r = (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP,
			.addr_g = (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_G_ADDR_TOP,
			.addr_b = (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_B_ADDR_TOP,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	Im_LTM_RBK_Ctrl_Common( pipe_no, &gct_im_ltm_rbk_ctrl_common_base );
	Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &rbk_ctrl_sdram_in );

	for( loopcnt = 0; loopcnt < (sizeof(rbk_in_addr) / sizeof(rbk_in_addr[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_RBK_Set_InAddr_Info( pipe_no, &rbk_in_addr[loopcnt] );

		ct_im_ltm_rbk_print_ctrl_sdram_input_reg();

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.RBK.RSDRADR.bit.SDRADR = 0x%lx\n", IO_LTM_P1.RBK.RSDRADR.bit.SDRADR ));
		Ddim_Print(( "IO_LTM_P1.RBK.RSDRADG.bit.SDRADG = 0x%lx\n", IO_LTM_P1.RBK.RSDRADG.bit.SDRADG ));
		Ddim_Print(( "IO_LTM_P1.RBK.RSDRADB.bit.SDRADB = 0x%lx\n", IO_LTM_P1.RBK.RSDRADB.bit.SDRADB ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.RBK.RSDRADR.bit.SDRADR = 0x%lx\n", IO_LTM_P2.RBK.RSDRADR.bit.SDRADR ));
		Ddim_Print(( "IO_LTM_P2.RBK.RSDRADG.bit.SDRADG = 0x%lx\n", IO_LTM_P2.RBK.RSDRADG.bit.SDRADG ));
		Ddim_Print(( "IO_LTM_P2.RBK.RSDRADB.bit.SDRADB = 0x%lx\n", IO_LTM_P2.RBK.RSDRADB.bit.SDRADB ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	rbk_ctrl_sdram_in.input_rect.img_top = 100;
	rbk_ctrl_sdram_in.input_rect.img_left = 200;

	Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &rbk_ctrl_sdram_in );

	Im_LTM_RBK_Set_InAddr_Info( pipe_no, &rbk_in_addr[2] );

	ct_im_ltm_rbk_print_ctrl_sdram_input_reg();

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRADR.bit.SDRADR = 0x%lx\n", IO_LTM_P1.RBK.RSDRADR.bit.SDRADR ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRADG.bit.SDRADG = 0x%lx\n", IO_LTM_P1.RBK.RSDRADG.bit.SDRADG ));
	Ddim_Print(( "IO_LTM_P1.RBK.RSDRADB.bit.SDRADB = 0x%lx\n", IO_LTM_P1.RBK.RSDRADB.bit.SDRADB ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRADR.bit.SDRADR = 0x%lx\n", IO_LTM_P2.RBK.RSDRADR.bit.SDRADR ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRADG.bit.SDRADG = 0x%lx\n", IO_LTM_P2.RBK.RSDRADG.bit.SDRADG ));
	Ddim_Print(( "IO_LTM_P2.RBK.RSDRADB.bit.SDRADB = 0x%lx\n", IO_LTM_P2.RBK.RSDRADB.bit.SDRADB ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_10: "
INT32 CT_Im_Ltm_Rbk_1_10( UCHAR pipe_no )
{
	UINT32						loopcnt;
	T_IM_LTM_RBK_OUTDATA_INFO	rbk_outdata_info[] = {
		[0] = {	// max
			.addr = (VOID*)0xFFFFFFFF,
			.global_width = 0xFFF8,
		},
		[1] = {	// min
			.addr = (VOID*)0,
			.global_width = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(rbk_outdata_info) / sizeof(rbk_outdata_info[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata_info[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.RBK.RSDWAD.bit.SDWAD   = 0x%lx\n", IO_LTM_P1.RBK.RSDWAD.bit.SDWAD   ));
		Ddim_Print(( "IO_LTM_P1.RBK.RSDWDEF.bit.SDWDEF = 0x%x\n",  IO_LTM_P1.RBK.RSDWDEF.bit.SDWDEF ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.RBK.RSDWAD.bit.SDWAD   = 0x%lx\n", IO_LTM_P2.RBK.RSDWAD.bit.SDWAD   ));
		Ddim_Print(( "IO_LTM_P2.RBK.RSDWDEF.bit.SDWDEF = 0x%x\n",  IO_LTM_P2.RBK.RSDWDEF.bit.SDWDEF ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_11: "
INT32 CT_Im_Ltm_Rbk_1_11( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(gct_im_ltm_rbk_ctrl_sro_direct) / sizeof(gct_im_ltm_rbk_ctrl_sro_direct[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &gct_im_ltm_rbk_ctrl_sro_direct[loopcnt] );

		ct_im_ltm_rbk_print_sro_direct_reg();
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_12: "
INT32 CT_Im_Ltm_Rbk_1_12( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_RBK_Ctrl_Resolution_Conversion( pipe_no, loopcnt );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.RBK.RHSTA.bit.HSTA = 0x%x\n", IO_LTM_P1.RBK.RHSTA.bit.HSTA ));
		Ddim_Print(( "IO_LTM_P1.RBK.RHPIT.bit.HPIT = 0x%x\n", IO_LTM_P1.RBK.RHPIT.bit.HPIT ));
		Ddim_Print(( "IO_LTM_P1.RBK.RVSTA.bit.VSTA = 0x%x\n", IO_LTM_P1.RBK.RVSTA.bit.VSTA ));
		Ddim_Print(( "IO_LTM_P1.RBK.RVPIT.bit.VPIT = 0x%x\n", IO_LTM_P1.RBK.RVPIT.bit.VPIT ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.RBK.RHSTA.bit.HSTA = 0x%x\n", IO_LTM_P2.RBK.RHSTA.bit.HSTA ));
		Ddim_Print(( "IO_LTM_P2.RBK.RHPIT.bit.HPIT = 0x%x\n", IO_LTM_P2.RBK.RHPIT.bit.HPIT ));
		Ddim_Print(( "IO_LTM_P2.RBK.RVSTA.bit.VSTA = 0x%x\n", IO_LTM_P2.RBK.RVSTA.bit.VSTA ));
		Ddim_Print(( "IO_LTM_P2.RBK.RVPIT.bit.VPIT = 0x%x\n", IO_LTM_P2.RBK.RVPIT.bit.VPIT ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_13: "
INT32 CT_Im_Ltm_Rbk_1_13( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_RBK_Set_FrameStop( pipe_no, loopcnt );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.RBK.RFRSTP.bit.FRSTP = %u\n", IO_LTM_P1.RBK.RFRSTP.bit.FRSTP ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.RBK.RFRSTP.bit.FRSTP = %u\n", IO_LTM_P2.RBK.RFRSTP.bit.FRSTP ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_14: "
INT32 CT_Im_Ltm_Rbk_1_14( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_RBK_Set_ParamHold( pipe_no, loopcnt );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.RBK.RPARMSK.bit.PARMSK = %u\n", IO_LTM_P1.RBK.RPARMSK.bit.PARMSK ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.RBK.RPARMSK.bit.PARMSK = %u\n", IO_LTM_P2.RBK.RPARMSK.bit.PARMSK ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_15: "
INT32 CT_Im_Ltm_Rbk_1_15( UCHAR pipe_no )
{
	BOOL busy_stat = FALSE;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.RBK.RTRG.bit.TRG = 3;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.RBK.RTRG.bit.TRG = 3;
	}
#endif // CO_DEBUG_ON_PC
	Im_LTM_RBK_Get_RbkBusy( pipe_no, &busy_stat );
	Ddim_Print(( "busy_stat = %u\n", busy_stat ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.RBK.RTRG.bit.TRG = 2;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.RBK.RTRG.bit.TRG = 2;
	}
#endif // CO_DEBUG_ON_PC

	busy_stat = FALSE;

	Im_LTM_RBK_Get_RbkBusy( pipe_no, &busy_stat );
	Ddim_Print(( "busy_stat = %u\n", busy_stat ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_16: "
INT32 CT_Im_Ltm_Rbk_1_16( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	Im_LTM_RBK_ContStart( pipe_no );

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "IO_LTM_P1.RBK.RTRG.bit.TRG = %u\n", IO_LTM_P1.RBK.RTRG.bit.TRG ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.RBK.RTRG.bit.TRG = %u\n", IO_LTM_P2.RBK.RTRG.bit.TRG ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_1_17: "
INT32 CT_Im_Ltm_Rbk_1_17( UCHAR pipe_no )
{
	UINT32						loopcnt;
	T_IM_LTM_RBK_RGB_TRIMMING	rbk_rgb_trim[] = {
		[0] = {	// max
			.trimming_enable = D_IM_LTM_ENABLE_ON,
			.rgb_trim = {
				.img_top = 1014,
				.img_left = 845,
				.img_width = 854,
				.img_lines = 1022,
			},
		},
		[1] = {	// min
			.trimming_enable = D_IM_LTM_ENABLE_OFF,
			.rgb_trim = {
				.img_top = 0,
				.img_left = 0,
				.img_width = 8,
				.img_lines = 8,
			},
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(rbk_rgb_trim) / sizeof(rbk_rgb_trim[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_RBK_Ctrl_RGBTrimming( pipe_no, &rbk_rgb_trim[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.RBK.RTEN.bit.RTEN     = 0x%x\n", IO_LTM_P1.RBK.RTEN.bit.RTEN     ));
		Ddim_Print(( "IO_LTM_P1.RBK.RTHSTA.bit.RTHSTA = 0x%x\n", IO_LTM_P1.RBK.RTHSTA.bit.RTHSTA ));
		Ddim_Print(( "IO_LTM_P1.RBK.RTHSIZ.bit.RTHSIZ = 0x%x\n", IO_LTM_P1.RBK.RTHSIZ.bit.RTHSIZ ));
		Ddim_Print(( "IO_LTM_P1.RBK.RTVSTA.bit.RTVSTA = 0x%x\n", IO_LTM_P1.RBK.RTVSTA.bit.RTVSTA ));
		Ddim_Print(( "IO_LTM_P1.RBK.RTVSIZ.bit.RTVSIZ = 0x%x\n", IO_LTM_P1.RBK.RTVSIZ.bit.RTVSIZ ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.RBK.RTEN.bit.RTEN     = 0x%x\n", IO_LTM_P2.RBK.RTEN.bit.RTEN     ));
		Ddim_Print(( "IO_LTM_P2.RBK.RTHSTA.bit.RTHSTA = 0x%x\n", IO_LTM_P2.RBK.RTHSTA.bit.RTHSTA ));
		Ddim_Print(( "IO_LTM_P2.RBK.RTHSIZ.bit.RTHSIZ = 0x%x\n", IO_LTM_P2.RBK.RTHSIZ.bit.RTHSIZ ));
		Ddim_Print(( "IO_LTM_P2.RBK.RTVSTA.bit.RTVSTA = 0x%x\n", IO_LTM_P2.RBK.RTVSTA.bit.RTVSTA ));
		Ddim_Print(( "IO_LTM_P2.RBK.RTVSIZ.bit.RTVSIZ = 0x%x\n", IO_LTM_P2.RBK.RTVSIZ.bit.RTVSIZ ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

static INT32 ct_im_ltm_rbk_run_1( UCHAR pipe_no, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-1-1
		case 1:
			return CT_Im_Ltm_Rbk_1_1( pipe_no );

		// Test-1-1-2
		case 2:
			return CT_Im_Ltm_Rbk_1_2( pipe_no );

		// Test-1-1-3
		case 3:
			return CT_Im_Ltm_Rbk_1_3( pipe_no );

		// Test-1-1-4
		case 4:
			return CT_Im_Ltm_Rbk_1_4( pipe_no );

		// Test-1-1-5
		case 5:
			return CT_Im_Ltm_Rbk_1_5( pipe_no );

		// Test-1-1-6
		case 6:
			return CT_Im_Ltm_Rbk_1_6( pipe_no );

		// Test-1-1-7
		case 7:
			return CT_Im_Ltm_Rbk_1_7( pipe_no );

		// Test-1-1-8
		case 8:
			return CT_Im_Ltm_Rbk_1_8( pipe_no );

		// Test-1-1-9
		case 9:
			return CT_Im_Ltm_Rbk_1_9( pipe_no );

		// Test-1-1-10
		case 10:
			return CT_Im_Ltm_Rbk_1_10( pipe_no );

		// Test-1-1-11
		case 11:
			return CT_Im_Ltm_Rbk_1_11( pipe_no );

		// Test-1-1-12
		case 12:
			return CT_Im_Ltm_Rbk_1_12( pipe_no );

		// Test-1-1-13
		case 13:
			return CT_Im_Ltm_Rbk_1_13( pipe_no );

		// Test-1-1-14
		case 14:
			return CT_Im_Ltm_Rbk_1_14( pipe_no );

		// Test-1-1-15
		case 15:
			return CT_Im_Ltm_Rbk_1_15( pipe_no );

		// Test-1-1-16
		case 16:
			return CT_Im_Ltm_Rbk_1_16( pipe_no );

		// Test-1-1-17
		case 17:
			return CT_Im_Ltm_Rbk_1_17( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}



#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_1: "
INT32 CT_Im_Ltm_Rbk_2_1( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Ctrl_Axi( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK ctrl axi error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_2: "
INT32 CT_Im_Ltm_Rbk_2_2( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Get_AxiReadStat( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK get axi read stat error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_3: "
INT32 CT_Im_Ltm_Rbk_2_3( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Get_AxiWriteStat( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK get axi write stat error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_4: "
INT32 CT_Im_Ltm_Rbk_2_4( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Get_AxiCtrlParam( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK get axi ctrl param error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_5: "
INT32 CT_Im_Ltm_Rbk_2_5( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Set_AxiCtrlParam( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK set axi ctrl param error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_6: "
INT32 CT_Im_Ltm_Rbk_2_6( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK set ctrl common error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_7: "
INT32 CT_Im_Ltm_Rbk_2_7( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK set ctrl sdram input error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_8: "
INT32 CT_Im_Ltm_Rbk_2_8( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Set_InAddr_Info( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK set inaddr error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_9: "
INT32 CT_Im_Ltm_Rbk_2_9( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK set outdata error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_10: "
INT32 CT_Im_Ltm_Rbk_2_10( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK ctrl sro direct error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_11: "
INT32 CT_Im_Ltm_Rbk_2_11( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Get_RbkBusy( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK get rbk busy error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_12: "
INT32 CT_Im_Ltm_Rbk_2_12( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_RBK_Ctrl_RGBTrimming( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK ctrl rgb trimming error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_2_13: "
INT32 CT_Im_Ltm_Rbk_2_13( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;
	DDIM_USER_FLGPTN flgptn;
	DDIM_USER_FLGPTN waiptn;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	waiptn = 0x11110000;
	ercd = Im_LTM_RBK_WaitEnd( &flgptn, waiptn, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "RBK wait end error = 0x%x\n", ercd ));
	}

	return ercd;
}

static INT32 ct_im_ltm_rbk_run_2( UCHAR pipe_no, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-2-1
		case 1:
			return CT_Im_Ltm_Rbk_2_1( pipe_no );

		// Test-1-2-2
		case 2:
			return CT_Im_Ltm_Rbk_2_2( pipe_no );

		// Test-1-2-3
		case 3:
			return CT_Im_Ltm_Rbk_2_3( pipe_no );

		// Test-1-2-4
		case 4:
			return CT_Im_Ltm_Rbk_2_4( pipe_no );

		// Test-1-2-5
		case 5:
			return CT_Im_Ltm_Rbk_2_5( pipe_no );

		// Test-1-2-6
		case 6:
			return CT_Im_Ltm_Rbk_2_6( pipe_no );

		// Test-1-2-7
		case 7:
			return CT_Im_Ltm_Rbk_2_7( pipe_no );

		// Test-1-2-8
		case 8:
			return CT_Im_Ltm_Rbk_2_8( pipe_no );

		// Test-1-2-9
		case 9:
			return CT_Im_Ltm_Rbk_2_9( pipe_no );

		// Test-1-2-10
		case 10:
			return CT_Im_Ltm_Rbk_2_10( pipe_no );

		// Test-1-2-11
		case 11:
			return CT_Im_Ltm_Rbk_2_11( pipe_no );

		// Test-1-2-12
		case 12:
			return CT_Im_Ltm_Rbk_2_12( pipe_no );

		// Test-1-2-13
		case 13:
			return CT_Im_Ltm_Rbk_2_13( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}



#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_1: "
INT32 CT_Im_Ltm_Rbk_3_1( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_SW_Reset( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_SW_Reset( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_2: "
INT32 CT_Im_Ltm_Rbk_3_2( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_CTRL_SDRAM_INPUT rbk_ctrl_sdram_in = gct_im_ltm_rbk_ctrl_sdram_in_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;
	rbk_ctrl_sdram_in.shift_enable = D_IM_LTM_ENABLE_OFF;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &rbk_ctrl_sdram_in );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &rbk_ctrl_sdram_in );
	Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Im_LTM_RBK_Start( pipe_no );
#endif

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_3: "
INT32 CT_Im_Ltm_Rbk_3_3( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Im_LTM_RBK_Start( pipe_no );
#endif

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_4: "
INT32 CT_Im_Ltm_Rbk_3_4( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_OUTDATA_INFO rbk_outdata = gct_im_ltm_rbk_outdata_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;
	rbk_outdata.global_width = IM_LTM_RBK_ROUNDUP_N(( D_IM_LTM_RBK_DST_IMG_GLOBAL_WIDTH_UP / 2 ), 512 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_Resolution_Conversion( pipe_no, D_IM_LTM_ENABLE_ON );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Im_LTM_RBK_Ctrl_Resolution_Conversion( pipe_no, D_IM_LTM_ENABLE_ON );
	Im_LTM_RBK_Start( pipe_no );
#endif

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_5: "
INT32 CT_Im_Ltm_Rbk_3_5( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_OUTDATA_INFO rbk_outdata = gct_im_ltm_rbk_outdata_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_mode = D_IM_LTM_RBK_RDC_MODE_DIV_2,
	rbk_outdata.global_width = IM_LTM_RBK_ROUNDUP_N(( D_IM_LTM_RBK_DST_IMG_GLOBAL_WIDTH_UP / 2 ), 512 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Im_LTM_RBK_Start( pipe_no );
#endif

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_6: "
INT32 CT_Im_Ltm_Rbk_3_6( UCHAR pipe_no )
{
	T_IM_LTM_RBK_OUTDATA_INFO rbk_outdata = gct_im_ltm_rbk_outdata_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_outdata.global_width = IM_LTM_RBK_ROUNDUP_N(( D_IM_LTM_RBK_DST_IMG_GLOBAL_WIDTH_UP / 4 ), 512 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &gct_im_ltm_rbk_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &gct_im_ltm_rbk_ctrl_common_base );
	Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Im_LTM_RBK_Start( pipe_no );
#endif

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_7: "
INT32 CT_Im_Ltm_Rbk_3_7( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_OUTDATA_INFO rbk_outdata = gct_im_ltm_rbk_outdata_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_mode = D_IM_LTM_RBK_RDC_MODE_DIV_8,
	rbk_outdata.global_width = IM_LTM_RBK_ROUNDUP_N(( D_IM_LTM_RBK_DST_IMG_GLOBAL_WIDTH_UP / 8 ), 512 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Im_LTM_RBK_Start( pipe_no );
#endif

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_8: "
INT32 CT_Im_Ltm_Rbk_3_8( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_OUTDATA_INFO rbk_outdata = gct_im_ltm_rbk_outdata_base;
	T_IM_LTM_RBK_RGB_TRIMMING rbk_ctrl_rgb_trim;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;
	rbk_outdata.global_width = IM_LTM_RBK_ROUNDUP_N(( D_IM_LTM_RBK_DST_IMG_GLOBAL_WIDTH_UP / 2 ), 512 );
	rbk_ctrl_rgb_trim.trimming_enable    = D_IM_LTM_ENABLE_ON;
	rbk_ctrl_rgb_trim.rgb_trim.img_top   = 120;
	rbk_ctrl_rgb_trim.rgb_trim.img_left  = 160;
	rbk_ctrl_rgb_trim.rgb_trim.img_width = 320;
	rbk_ctrl_rgb_trim.rgb_trim.img_lines = 240;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_RGBTrimming( pipe_no, &rbk_ctrl_rgb_trim );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Im_LTM_RBK_Ctrl_RGBTrimming( pipe_no, &rbk_ctrl_rgb_trim );
	Im_LTM_RBK_Start( pipe_no );
#endif

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_9: "
INT32 CT_Im_Ltm_Rbk_3_9( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_CTRL_SRO_DIRECT rbk_ctrl_sro_direct = gct_im_ltm_rbk_ctrl_sro_direct_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;
	rbk_ctrl_sro_direct.shift_enable0 = D_IM_LTM_ENABLE_OFF;

	// SRO setting
	ct_im_ltm_rbk_set_sro( (E_IM_PRO_UNIT_NUM)pipe_no, 0 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Im_LTM_RBK_Start( pipe_no );
#endif

	// SRO start
	ct_im_ltm_rbk_start_sro( (E_IM_PRO_UNIT_NUM)pipe_no );
	ct_im_ltm_rbk_waitend_stop_sro( (E_IM_PRO_UNIT_NUM)pipe_no );

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_10: "
INT32 CT_Im_Ltm_Rbk_3_10( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;

	// SRO setting
	ct_im_ltm_rbk_set_sro( (E_IM_PRO_UNIT_NUM)pipe_no, 0 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &gct_im_ltm_rbk_ctrl_sro_direct_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &gct_im_ltm_rbk_ctrl_sro_direct_base );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Im_LTM_RBK_Start( pipe_no );
#endif

	// SRO start
	ct_im_ltm_rbk_start_sro( (E_IM_PRO_UNIT_NUM)pipe_no );
	ct_im_ltm_rbk_waitend_stop_sro( (E_IM_PRO_UNIT_NUM)pipe_no );

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_11: "
INT32 CT_Im_Ltm_Rbk_3_11( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_CTRL_SRO_DIRECT rbk_ctrl_sro_direct = gct_im_ltm_rbk_ctrl_sro_direct_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	// SRO setting
	ct_im_ltm_rbk_set_sro( (E_IM_PRO_UNIT_NUM)pipe_no, 0 );

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;
	rbk_ctrl_sro_direct.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_B;
	rbk_ctrl_sro_direct.bayer_trim0.img_top   = 241;
	rbk_ctrl_sro_direct.bayer_trim0.img_left  = 321;
	rbk_ctrl_sro_direct.bayer_trim0.img_width = 640;
	rbk_ctrl_sro_direct.bayer_trim0.img_lines = 480;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Im_LTM_RBK_Start( pipe_no );
#endif

	// SRO start
	ct_im_ltm_rbk_start_sro( (E_IM_PRO_UNIT_NUM)pipe_no );
	ct_im_ltm_rbk_waitend_stop_sro( (E_IM_PRO_UNIT_NUM)pipe_no );

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_12: "
INT32 CT_Im_Ltm_Rbk_3_12( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_CTRL_SRO_DIRECT rbk_ctrl_sro_direct = gct_im_ltm_rbk_ctrl_sro_direct_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;
	rbk_ctrl_sro_direct.half_reduction_enable = D_IM_LTM_ENABLE_ON;

	// SRO setting
	ct_im_ltm_rbk_set_sro( (E_IM_PRO_UNIT_NUM)pipe_no, 0 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Im_LTM_RBK_Start( pipe_no );
#endif

	// SRO start
	ct_im_ltm_rbk_start_sro( (E_IM_PRO_UNIT_NUM)pipe_no );
	ct_im_ltm_rbk_waitend_stop_sro( (E_IM_PRO_UNIT_NUM)pipe_no );

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_13: "
INT32 CT_Im_Ltm_Rbk_3_13( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_CTRL_SRO_DIRECT rbk_ctrl_sro_direct = gct_im_ltm_rbk_ctrl_sro_direct_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;
	rbk_ctrl_sro_direct.half_reduction_enable = D_IM_LTM_ENABLE_ON;
	rbk_ctrl_sro_direct.sro_mode = D_IM_LTM_RBK_SRO_DIRECT_2,
	rbk_ctrl_sro_direct.shift_enable0 = D_IM_LTM_ENABLE_OFF;
	rbk_ctrl_sro_direct.bayer_trim0.img_width = D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH / 2;
	rbk_ctrl_sro_direct.bayer_trim1.img_width = D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH / 2;
	rbk_ctrl_sro_direct.bayer_trim1.img_lines = D_IM_LTM_RBK_SRC_IMG_PIXS_LINES;

	// SRO setting(2pipe)
	ct_im_ltm_rbk_set_sro( E_IM_PRO_BOTH_UNIT, 0 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Im_LTM_RBK_Start( pipe_no );
#endif

	// SRO start(2pipe)
	ct_im_ltm_rbk_start_sro( E_IM_PRO_BOTH_UNIT );
	ct_im_ltm_rbk_waitend_stop_sro( E_IM_PRO_BOTH_UNIT );

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_14: "
INT32 CT_Im_Ltm_Rbk_3_14( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_CTRL_SRO_DIRECT rbk_ctrl_sro_direct = gct_im_ltm_rbk_ctrl_sro_direct_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;
	rbk_ctrl_sro_direct.half_reduction_enable = D_IM_LTM_ENABLE_ON;
	rbk_ctrl_sro_direct.sro_mode = D_IM_LTM_RBK_SRO_DIRECT_2,
	rbk_ctrl_sro_direct.shift_enable1 = D_IM_LTM_ENABLE_ON;
	rbk_ctrl_sro_direct.bayer_trim0.img_width = D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH / 2;
	rbk_ctrl_sro_direct.bayer_trim1.img_width = D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH / 2;
	rbk_ctrl_sro_direct.bayer_trim1.img_lines = D_IM_LTM_RBK_SRC_IMG_PIXS_LINES;

	// SRO setting(2pipe)
	ct_im_ltm_rbk_set_sro( E_IM_PRO_BOTH_UNIT, 0 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Im_LTM_RBK_Start( pipe_no );
#endif

	// SRO start(2pipe)
	ct_im_ltm_rbk_start_sro( E_IM_PRO_BOTH_UNIT );
	ct_im_ltm_rbk_waitend_stop_sro( E_IM_PRO_BOTH_UNIT );

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_15: "
INT32 CT_Im_Ltm_Rbk_3_15( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_CTRL_SRO_DIRECT rbk_ctrl_sro_direct = gct_im_ltm_rbk_ctrl_sro_direct_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;
	rbk_ctrl_sro_direct.half_reduction_enable = D_IM_LTM_ENABLE_ON;
	rbk_ctrl_sro_direct.sro_mode = D_IM_LTM_RBK_SRO_DIRECT_2,
	rbk_ctrl_sro_direct.shift_enable1 = D_IM_LTM_ENABLE_ON;
	rbk_ctrl_sro_direct.bayer_trim0.img_top   = ( D_IM_LTM_RBK_SRC_IMG_PIXS_LINES - ( D_IM_LTM_RBK_SRC_IMG_PIXS_LINES / 2 )) / 2;
	rbk_ctrl_sro_direct.bayer_trim0.img_left  = ( D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH - ( D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH / 2 )) / 2;
	rbk_ctrl_sro_direct.bayer_trim0.img_width = ( D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH - ( D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH / 2 )) / 2;
	rbk_ctrl_sro_direct.bayer_trim0.img_lines = ( D_IM_LTM_RBK_SRC_IMG_PIXS_LINES - ( D_IM_LTM_RBK_SRC_IMG_PIXS_LINES / 2 ));
	rbk_ctrl_sro_direct.bayer_trim1.img_top   = ( D_IM_LTM_RBK_SRC_IMG_PIXS_LINES - ( D_IM_LTM_RBK_SRC_IMG_PIXS_LINES / 2 )) / 2;
	rbk_ctrl_sro_direct.bayer_trim1.img_left  = 0;
	rbk_ctrl_sro_direct.bayer_trim1.img_width = ( D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH - ( D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH / 2 )) / 2;
	rbk_ctrl_sro_direct.bayer_trim1.img_lines = ( D_IM_LTM_RBK_SRC_IMG_PIXS_LINES - ( D_IM_LTM_RBK_SRC_IMG_PIXS_LINES / 2 ));

	// SRO setting(2pipe)
	ct_im_ltm_rbk_set_sro( E_IM_PRO_BOTH_UNIT, 0 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Im_LTM_RBK_Start( pipe_no );
#endif

	// SRO start(2pipe)
	ct_im_ltm_rbk_start_sro( E_IM_PRO_BOTH_UNIT );
	ct_im_ltm_rbk_waitend_stop_sro( E_IM_PRO_BOTH_UNIT );

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_16: "
INT32 CT_Im_Ltm_Rbk_3_16( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_CTRL_SRO_DIRECT rbk_ctrl_sro_direct = gct_im_ltm_rbk_ctrl_sro_direct_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	// SRO setting
	ct_im_ltm_rbk_set_sro( E_IM_PRO_BOTH_UNIT, 1 );

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;
	rbk_ctrl_sro_direct.shift_enable0 = D_IM_LTM_ENABLE_OFF;
	rbk_ctrl_sro_direct.bayer_trim0.img_top   = 0;
	rbk_ctrl_sro_direct.bayer_trim0.img_left  = 0;
	rbk_ctrl_sro_direct.bayer_trim0.img_width = 1282;
	rbk_ctrl_sro_direct.bayer_trim0.img_lines = 962;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSRODirect( pipe_no, &rbk_ctrl_sro_direct );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &gct_im_ltm_rbk_outdata_base );
	Im_LTM_RBK_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Im_LTM_RBK_Start( pipe_no );
#endif

	// SRO start
	ct_im_ltm_rbk_start_sro( E_IM_PRO_BOTH_UNIT );
	ct_im_ltm_rbk_waitend_stop_sro( E_IM_PRO_BOTH_UNIT );

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Rbk_3_18: "
INT32 CT_Im_Ltm_Rbk_3_18( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common = gct_im_ltm_rbk_ctrl_common_base;
	T_IM_LTM_RBK_OUTDATA_INFO rbk_outdata = gct_im_ltm_rbk_outdata_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	rbk_ctrl_common.reduction_enable = D_IM_LTM_ENABLE_OFF;
	rbk_ctrl_common.data_type = D_IM_LTM_DTYP_PACK12,
	rbk_outdata.global_width = IM_LTM_RBK_ROUNDUP_N( D_IM_LTM_RBK_DST_IMG_GLOBAL_WIDTH_P, 512 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_rbk_ctrl_sdram_in_base );
	Im_LTM_RBK_Set_InAddr_Info( pipe_no, &gct_im_ltm_rbk_in_addr_base );
	Im_LTM_RBK_Set_OutData_Info( pipe_no, &rbk_outdata );
	Im_LTM_RBK_Start( pipe_no );
#endif

	if( ct_im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
	}
	if( ct_im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_RBK_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_RBK_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_RBK_Stop( pipe_no );
#endif

	ct_im_ltm_rbk_print_all_reg();

	return D_DDIM_OK;
}

static INT32 ct_im_ltm_rbk_run_3( UCHAR pipe_no, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-3-1
		case 1:
			return CT_Im_Ltm_Rbk_3_1( pipe_no );

		// Test-1-3-2
		case 2:
			return CT_Im_Ltm_Rbk_3_2( pipe_no );

		// Test-1-3-3
		case 3:
			return CT_Im_Ltm_Rbk_3_3( pipe_no );

		// Test-1-3-4
		case 4:
			return CT_Im_Ltm_Rbk_3_4( pipe_no );

		// Test-1-3-5
		case 5:
			return CT_Im_Ltm_Rbk_3_5( pipe_no );

		// Test-1-3-6
		case 6:
			return CT_Im_Ltm_Rbk_3_6( pipe_no );

		// Test-1-3-7
		case 7:
			return CT_Im_Ltm_Rbk_3_7( pipe_no );

		// Test-1-3-8
		case 8:
			return CT_Im_Ltm_Rbk_3_8( pipe_no );

		// Test-1-3-9
		case 9:
			return CT_Im_Ltm_Rbk_3_9( pipe_no );

		// Test-1-3-10
		case 10:
			return CT_Im_Ltm_Rbk_3_10( pipe_no );

		// Test-1-3-11
		case 11:
			return CT_Im_Ltm_Rbk_3_11( pipe_no );

		// Test-1-3-12
		case 12:
			return CT_Im_Ltm_Rbk_3_12( pipe_no );

		// Test-1-3-13
		case 13:
			return CT_Im_Ltm_Rbk_3_13( pipe_no );

		// Test-1-3-14
		case 14:
			return CT_Im_Ltm_Rbk_3_14( pipe_no );

		// Test-1-3-15
		case 15:
			return CT_Im_Ltm_Rbk_3_15( pipe_no );

		// Test-1-3-16
		case 16:
			return CT_Im_Ltm_Rbk_3_16( pipe_no );

		// Test-1-3-18
		case 18:
			return CT_Im_Ltm_Rbk_3_18( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}


VOID CT_Im_LTM_RBK_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
#ifdef CO_MSG_PRINT_ON
	INT32 result = D_DDIM_INPUT_PARAM_ERROR;
#endif
	static BOOL is_init = 0;
	UCHAR loop_cnt;

	Ddim_Print(( "CT_Im_LTM_RBK_Run( %u, %u, %u ) Begin\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	if( is_init == 0 ) {
#ifdef CO_DEBUG_ON_PC
		memset( (VOID*)&IO_LTM_P1.RBK, '\0', sizeof(IO_LTM_P1.RBK) );
		memset( (VOID*)&IO_LTM_P2.RBK, '\0', sizeof(IO_LTM_P2.RBK) );
		IO_LTM_P1.RBK.RTRG.word = 2;
		IO_LTM_P2.RBK.RTRG.word = 2;
#endif // CO_DEBUG_ON_PC
		if( !((ct_idx_1st == 1) && (ct_idx_2nd == 1)) ) {
			Ddim_Print(( "Im_LTM_RBK_Init() Begin\n" ));
			Im_LTM_RBK_Init( D_IM_LTM_PIPE12 );
			Ddim_Print(( "Im_LTM_RBK_Init() End\n" ));
			is_init = 1;
		}
	}
	Dd_Top_Set_CLKSTOP10_LTMRBK1CK( 1 );
	Dd_Top_Set_CLKSTOP12_LTMRBK2CK( 1 );
	Dd_Top_Set_CLKSEL5_LTMRBK1CLK( 0 );		// 300MHz
	Dd_Top_Set_CLKSEL7_LTMRBK2CLK( 0 );		// 300MHz
	Dd_Top_Set_CLKSTOP12_LTMRBK2CK( 0 );
	Dd_Top_Set_CLKSTOP10_LTMRBK1CK( 0 );

	if( ct_idx_1st != 3 ){
		for( loop_cnt = 0; loop_cnt <= 1; loop_cnt++ ){
			switch( ct_idx_1st ) {
				case 1:
					D_IM_LTM_RESULT( ct_im_ltm_rbk_run_1( loop_cnt, ct_idx_2nd ) );
					break;

				case 2:
					D_IM_LTM_RESULT( ct_im_ltm_rbk_run_2( loop_cnt, ct_idx_2nd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}

#ifdef CO_MSG_PRINT_ON
			Ddim_Print(( "result = 0x%x\n", result ));
#endif
			Ddim_Print(( "CT_Im_LTM_RBK_Run( %u, %u, %u) End\n", ct_idx_1st, ct_idx_2nd, loop_cnt ));
		}
	}
	else{
		D_IM_LTM_RESULT( ct_im_ltm_rbk_run_3( ct_idx_3rd, ct_idx_2nd ) );

#ifdef CO_MSG_PRINT_ON
		Ddim_Print(( "result = 0x%x\n", result ));
#endif
		Ddim_Print(( "CT_Im_LTM_RBK_Run( %u, %u, %u) End\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));
	}

	return;
}


// for Debug console interface
VOID CT_Im_LTM_RBK_Main( INT32 argc, CHAR** argv )
{
	if( argc < 3 ) {
		Ddim_Print(( "CT_Im_LTM_RBK_Main: parameter error.\n" ));
		return;
	}

	CT_Im_LTM_RBK_Run( (UINT32)atoi((const char*)argv[1]),
					   (UINT32)atoi((const char*)argv[2]),
					   (UINT32)atoi((const char*)argv[3]) );
}

