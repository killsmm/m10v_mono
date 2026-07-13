/**
 * @file		ct_im_cnr.c
 * @brief		CNR ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "ct_im_cnr.h"
#include "im_cnr.h"
#include "dd_top.h"
#include "driver_common.h"
#include "ddim_user_custom.h"
#include "im_rdma.h"
#include "jdscnr.h"
#include <string.h>
#include <stdlib.h>


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define IO_CNR(ch)		((D_IM_CNR_CH_0 == ((ch))) ? &IO_CNR1 : (D_IM_CNR_CH_1 == ((ch))) ? &IO_CNR2 : &IO_CNR3)

#define M_16ROUNDUP(a)	(( ((a)+0x0F) >> 4) << 4)		/**< 16 byte alignment */

#define D_IM_CNR_TEST_H_PIXS	(640)
#define D_IM_CNR_TEST_V_PIXS	(480)

#define D_IM_CNR_TEST_IMG_SRC_ADDR	(0x7D000000)

// Src address(YCC422 Planar format)
#define D_IM_CNR_TEST_IMG_SRC_Y_ADDR	(D_IM_CNR_TEST_IMG_SRC_ADDR)														// 0x7D000000
#define D_IM_CNR_TEST_IMG_SRC_CB_ADDR	(D_IM_CNR_TEST_IMG_SRC_ADDR + D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS)			// 0x7D04B000
#define D_IM_CNR_TEST_IMG_SRC_CR_ADDR	(D_IM_CNR_TEST_IMG_SRC_CB_ADDR + D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS / 2)	// 0x7D070800
#define D_IM_CNR_TEST_IMG_SRC_BYTES		(D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS * 2)									// 0x96000

// Dst address(YCC422 Planar format)
#define D_IM_CNR_TEST_IMG_DST_ADDR		(0x7E000000)																		// 0x7E000000

#define D_IM_CNR_TEST_IMG_DST_Y_ADDR	(D_IM_CNR_TEST_IMG_DST_ADDR)														// 0x7E000000
#define D_IM_CNR_TEST_IMG_DST_CB_ADDR	(D_IM_CNR_TEST_IMG_DST_ADDR + D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS)			// 0x7E04B000
#define D_IM_CNR_TEST_IMG_DST_CR_ADDR	(D_IM_CNR_TEST_IMG_DST_CB_ADDR + D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS / 2)	// 0x7E070800
#define D_IM_CNR_TEST_IMG_DST_BYTES		(D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS * 2)									// 0x96000

// Temporary address(for CNR macro use)
#define D_IM_CNR_TEST_IMG_TMP_ADDR		(0x7F000000)																		// 0x7F000000
#define D_IM_CNR_TEST_IMG_TMP_BYTES		(D_IM_CNR_TEST_V_PIXS * 576)														// 0x43800


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// nothing special.


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// nothing special.


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
UINT32 gCT_IM_CNR_In_Addr = D_IM_CNR_TEST_IMG_SRC_ADDR;
UINT32 gCT_IM_CNR_Out_Addr = D_IM_CNR_TEST_IMG_DST_ADDR;


/*----------------------------------------------------------------------*/
/* Local Method															*/
/*----------------------------------------------------------------------*/
#ifdef CO_MSG_PRINT_ON
#define D_IM_CNR_RESULT(statement)		result = statement
#else
#define D_IM_CNR_RESULT
#endif

#ifdef D_IM_R2Y_DEBUG_ON_PC
extern volatile UCHAR gIM_R2Y_macro_fake_finish[2];
#endif


#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
#include "im_r2y.h"

#define D_INPUT_IMG_RGB_R_ADDR			(0x60000000)
#define D_INPUT_IMG_RGB_G_ADDR			(D_INPUT_IMG_RGB_R_ADDR + 640 * 480 * 3 / 2)
#define D_INPUT_IMG_RGB_B_ADDR			(D_INPUT_IMG_RGB_G_ADDR + 640 * 480 * 3 / 2)
#define D_OUTPUT_IMG_YC422_Y_ADDR		(0x63000000)
#define D_OUTPUT_IMG_YC422_CB_ADDR		(D_OUTPUT_IMG_YC422_Y_ADDR + 640 * 480)
#define D_OUTPUT_IMG_YC422_CR_ADDR		(0x64000000)
#define D_IM_R2Y_SRC_IMG_PIXS_G_WIDTH	(960)	// 640*3/2
#define D_IM_R2Y_SRC_IMG_PIXS_WIDTH		(640)
#define D_IM_R2Y_SRC_IMG_PIXS_LINES		(480)

#define D_IM_R2Y_DST_IMG_PIXS_G_WIDTH	(640)
#define D_IM_R2Y_DST_IMG_PIXS_WIDTH		(640)
#define D_IM_R2Y_DST_IMG_PIXS_LINES		(480)

// TestBase table
static T_IM_R2Y_CTRL gct_im_r2y_ctrl_base = {
	// D_IM_R2Y_YYW_CH_0: enable
	.yyw[D_IM_R2Y_YYW_CH_0] = {
		.axi_write_mode = {
			.master_if_select				= D_IM_R2Y_MASTER_IF0_PORT0 | D_IM_R2Y_MASTER_IF0_PORT1 | D_IM_R2Y_MASTER_IF0_PORT2,
			.burst_length					= D_IM_R2Y_BRST_512,
			.out_enable[D_IM_R2Y_PORT_0]	= D_IM_R2Y_ENABLE_ON,
			.out_enable[D_IM_R2Y_PORT_1]	= D_IM_R2Y_ENABLE_OFF,
			.out_enable[D_IM_R2Y_PORT_2]	= D_IM_R2Y_ENABLE_OFF,
		},
		.write_request_mask[D_IM_R2Y_PORT_0]	= 0,
		.write_request_mask[D_IM_R2Y_PORT_1]	= 0,
		.write_request_mask[D_IM_R2Y_PORT_2]	= 0,
		.resize_mode							= D_IM_R2Y_RSZ_BILINEAR,
		.pixel_avg_reduction_mode				= D_IM_R2Y_RDC_MODE_DIV_2,
		.pixel_avg_reduction_enable				= D_IM_R2Y_ENABLE_OFF,
		.ycc_cc_thin_select						= D_IM_R2Y_THIN_OUT_YCC422_CENTER,
		.trim_out_enable						= D_IM_R2Y_ENABLE_OFF,
		.planar_interleaved_mode				= D_IM_R2Y_PORT_INTERLEAVED,
	},
	// D_IM_R2Y_YYW_CH_1: disable
	.yyw[D_IM_R2Y_YYW_CH_1] = {
		.axi_write_mode = {
			.master_if_select				= D_IM_R2Y_MASTER_IF0_PORT0 | D_IM_R2Y_MASTER_IF0_PORT1 | D_IM_R2Y_MASTER_IF0_PORT2,
			.burst_length					= D_IM_R2Y_BRST_512,
			.out_enable[D_IM_R2Y_PORT_0]	= D_IM_R2Y_ENABLE_OFF,
			.out_enable[D_IM_R2Y_PORT_1]	= D_IM_R2Y_ENABLE_OFF,
			.out_enable[D_IM_R2Y_PORT_2]	= D_IM_R2Y_ENABLE_OFF,
		},
		.write_request_mask[D_IM_R2Y_PORT_0]	= 0,
		.write_request_mask[D_IM_R2Y_PORT_1]	= 0,
		.write_request_mask[D_IM_R2Y_PORT_2]	= 0,
		.resize_mode							= D_IM_R2Y_RSZ_BILINEAR,
		.pixel_avg_reduction_mode				= D_IM_R2Y_RDC_MODE_DIV_2,
		.pixel_avg_reduction_enable				= D_IM_R2Y_ENABLE_OFF,
		.ycc_cc_thin_select						= D_IM_R2Y_THIN_OUT_YCC420_CENTER,
		.trim_out_enable						= D_IM_R2Y_ENABLE_OFF,
		.planar_interleaved_mode				= D_IM_R2Y_PORT_INTERLEAVED,
	},
	// D_IM_R2Y_YYW_CH_2: enable
	.yyw[D_IM_R2Y_YYW_CH_2] = {
		.axi_write_mode = {
			.master_if_select				= D_IM_R2Y_MASTER_IF0_PORT0 | D_IM_R2Y_MASTER_IF0_PORT1 | D_IM_R2Y_MASTER_IF0_PORT2,
			.burst_length					= D_IM_R2Y_BRST_512,
			.out_enable[D_IM_R2Y_PORT_0]	= D_IM_R2Y_ENABLE_ON,
			.out_enable[D_IM_R2Y_PORT_1]	= D_IM_R2Y_ENABLE_OFF,
			.out_enable[D_IM_R2Y_PORT_2]	= D_IM_R2Y_ENABLE_OFF,
		},
		.write_request_mask[D_IM_R2Y_PORT_0]	= 0,
		.write_request_mask[D_IM_R2Y_PORT_1]	= 0,
		.write_request_mask[D_IM_R2Y_PORT_2]	= 0,
		.resize_mode							= D_IM_R2Y_RSZ_BILINEAR,
		.pixel_avg_reduction_mode				= D_IM_R2Y_RDC_MODE_DIV_2,
		.pixel_avg_reduction_enable				= D_IM_R2Y_ENABLE_OFF,
		.ycc_cc_thin_select						= D_IM_R2Y_THIN_OUT_YCC420_CENTER,
		.trim_out_enable						= D_IM_R2Y_ENABLE_OFF,
		.planar_interleaved_mode				= D_IM_R2Y_PORT_INTERLEAVED,
	},
	// D_IM_R2Y_YYW_CH_0A: disable
	.yyw[D_IM_R2Y_YYW_CH_0A] = {
		.axi_write_mode = {
			.master_if_select				= D_IM_R2Y_MASTER_IF0_PORT0 | D_IM_R2Y_MASTER_IF0_PORT1 | D_IM_R2Y_MASTER_IF0_PORT2,
			.burst_length					= D_IM_R2Y_BRST_512,
			.out_enable[D_IM_R2Y_PORT_0]	= D_IM_R2Y_ENABLE_OFF,
			.out_enable[D_IM_R2Y_PORT_1]	= D_IM_R2Y_ENABLE_OFF,
			.out_enable[D_IM_R2Y_PORT_2]	= D_IM_R2Y_ENABLE_OFF,
		},
		.write_request_mask[D_IM_R2Y_PORT_0]	= 0,
		.write_request_mask[D_IM_R2Y_PORT_1]	= 0,
		.write_request_mask[D_IM_R2Y_PORT_2]	= 0,
		.resize_mode							= D_IM_R2Y_RSZ_BILINEAR,
		.pixel_avg_reduction_mode				= D_IM_R2Y_RDC_MODE_DIV_2,
		.pixel_avg_reduction_enable				= D_IM_R2Y_ENABLE_OFF,
		.ycc_cc_thin_select						= D_IM_R2Y_THIN_OUT_YCC420_CENTER,
		.trim_out_enable						= D_IM_R2Y_ENABLE_OFF,
		.planar_interleaved_mode				= D_IM_R2Y_PORT_INTERLEAVED,
	},

	.line_intr[D_IM_R2Y_LINE_INT_0] = {
		.level = 0,
		.yyw_select = D_IM_R2Y_YYW_CH_0,
	},
	.line_intr[D_IM_R2Y_LINE_INT_1] = {
		.level = 0,
		.yyw_select = D_IM_R2Y_YYW_CH_0,
	},
	.line_intr[D_IM_R2Y_LINE_INT_2] = {
		.level = 0,
		.yyw_select = D_IM_R2Y_YYW_CH_0,
	},

	.yyw_enable[D_IM_R2Y_YYW_CH_0]	= D_IM_R2Y_ENABLE_ON,
	.yyw_enable[D_IM_R2Y_YYW_CH_1]	= D_IM_R2Y_ENABLE_OFF,
	.yyw_enable[D_IM_R2Y_YYW_CH_2]	= D_IM_R2Y_ENABLE_OFF,
	.line_transfer_cycle			= 0,
	.yyw_continue_start_enable		= D_IM_R2Y_ENABLE_OFF,
	.yyw_horizontal_flip			= D_IM_R2Y_ENABLE_OFF,	// Flip OFF
	.video_format_out_select_0		= D_IM_R2Y_VFM_NORMAL,
	.ipu_macro_output_enable		= D_IM_R2Y_ENABLE_OFF,	// IPU output OFF
	.ipu_macro_trimming_enable		= D_IM_R2Y_ENABLE_OFF,
	.cnr_macro_output_enable		= D_IM_R2Y_ENABLE_ON,	// CNR output ON
	.cnr_macro_trimming_enable		= D_IM_R2Y_ENABLE_OFF,
	.output_mode_0a					= D_IM_R2Y_YYW0A_OUTPUT_MODE_STOP,
	.video_format_out_select_0a		= D_IM_R2Y_VFM_NORMAL,
	.output_format_sel1				= D_IM_R2Y_DATA_FORMAT_8,
	.output_type_sel1				= D_IM_R2Y_WRITE_DTYP_PACK8,
	.ycf_bypass						= D_IM_R2Y_ENABLE_OFF,
	.ycf_padding					= D_IM_R2Y_ENABLE_ON,
	.mcc_select						= D_IM_R2Y_MCC_AFTER_CC0,
	.mcc_bit_shift					= D_IM_R2Y_ENABLE_OFF,
	.r2y_user_handler				= NULL,
	.user_param						= 0,
};

// T_IM_R2Y_CTRL_SDRAM_INPUT table for RGB 12bit input
static T_IM_R2Y_CTRL_SDRAM_INPUT gct_im_r2y_ctrl_sdram_in_rgb12bit = {
	.burst_length						= D_IM_R2Y_BRST_512,
	.read_request_mask[D_IM_R2Y_PORT_0]	= 0,
	.read_request_mask[D_IM_R2Y_PORT_1]	= 0,
	.read_request_mask[D_IM_R2Y_PORT_2]	= 0,
	.input_dtype						= D_IM_R2Y_READ_DTYP_PACK12,
	.mono_ebable						= D_IM_R2Y_ENABLE_OFF,
	.rgb_deknee_enable					= D_IM_R2Y_ENABLE_OFF,
	.rgb_left_shift						= D_IM_R2Y_STL_RGB_LEFT_SHIFT_NONE,
	.rgb_saturation_mode				= D_IM_R2Y_ENABLE_OFF,
	.top_offset[D_IM_R2Y_PORT_0]		= 0,
	.top_offset[D_IM_R2Y_PORT_1]		= 0,
	.top_offset[D_IM_R2Y_PORT_2]		= 0,
	.input_global						= D_IM_R2Y_SRC_IMG_PIXS_G_WIDTH,
};

// T_IM_R2Y_RESIZE_RECT table for YC422 output
static T_IM_R2Y_RESIZE_RECT gct_im_r2y_resize_rect_param_out_yc422 = {
	.input_size.img_left	= 0,
	.input_size.img_top		= 0,
	.input_size.img_width	= D_IM_R2Y_SRC_IMG_PIXS_WIDTH,
	.input_size.img_lines	= D_IM_R2Y_SRC_IMG_PIXS_LINES,
	.output_size[D_IM_R2Y_YYW_CH_0] = {
		.yyw_width								= D_IM_R2Y_DST_IMG_PIXS_WIDTH,
		.yyw_lines								= D_IM_R2Y_DST_IMG_PIXS_LINES,
		.output_global_w[D_IM_R2Y_PORT_Y]		= D_IM_R2Y_DST_IMG_PIXS_G_WIDTH,
		.output_global_w[D_IM_R2Y_PORT_CBCR]	= 0,	// dummy
		.output_global_w[D_IM_R2Y_PORT_2]		= 0,	// dummy
	},
	.output_size[D_IM_R2Y_YYW_CH_1] = {
		.yyw_width								= 0,	// dummy
		.yyw_lines								= 0,	// dummy
		.output_global_w[D_IM_R2Y_PORT_Y]		= 0,	// dummy
		.output_global_w[D_IM_R2Y_PORT_CBCR]	= 0,	// dummy
		.output_global_w[D_IM_R2Y_PORT_2]		= 0,	// dummy
	},
	.output_size[D_IM_R2Y_YYW_CH_2] = {
		.yyw_width								= 0,	// dummy
		.yyw_lines								= 0,	// dummy
		.output_global_w[D_IM_R2Y_PORT_Y]		= 0,	// dummy
		.output_global_w[D_IM_R2Y_PORT_1]		= 0,	// dummy
		.output_global_w[D_IM_R2Y_PORT_2]		= 0,	// dummy
	},
	.output_size[D_IM_R2Y_YYW_CH_0A] = {
		.yyw_width								= 0,	// dummy
		.yyw_lines								= 0,	// dummy
		.output_global_w[D_IM_R2Y_PORT_Y]		= 0,	// dummy
		.output_global_w[D_IM_R2Y_PORT_CBCR]	= 0,	// dummy
		.output_global_w[D_IM_R2Y_PORT_2]		= 0,	// dummy
	},
};

static T_IM_R2Y_RGB_COLOR im_r2y_wb_gain = {
	(USHORT)(1.0F * 0x100),	// R
	(USHORT)(1.0F * 0x100),	// G
	(USHORT)(1.0F * 0x100),	// B
};

static VOID ct_im_r2y_set_gamma_on( UCHAR ch )
{
	static const USHORT dgamma_abs[D_IM_R2Y_TABLE_MAX_GAMMA] = {
		0x0000, 0x0052, 0x0071, 0x0088, 0x009B, 0x00AB, 0x00BA, 0x00C7,
		0x00D4, 0x00DF, 0x00EA, 0x00F5, 0x00FF, 0x0108, 0x0111, 0x011A,
		0x0122, 0x012A, 0x0132, 0x013A, 0x0141, 0x0148, 0x014F, 0x0156,
		0x015D, 0x0163, 0x016A, 0x0170, 0x0176, 0x017C, 0x0182, 0x0188,
		0x018E, 0x0193, 0x0199, 0x019E, 0x01A3, 0x01A9, 0x01AE, 0x01B3,
		0x01B8, 0x01BD, 0x01C2, 0x01C7, 0x01CC, 0x01D0, 0x01D5, 0x01D9,
		0x01DE, 0x01E3, 0x01E7, 0x01EB, 0x01F0, 0x01F4, 0x01F8, 0x01FD,
		0x0201, 0x0205, 0x0209, 0x020D, 0x0211, 0x0215, 0x0219, 0x021D,
		0x0221, 0x0225, 0x0229, 0x022C, 0x0230, 0x0234, 0x0237, 0x023B,
		0x023F, 0x0242, 0x0246, 0x024A, 0x024D, 0x0251, 0x0254, 0x0258,
		0x025B, 0x025E, 0x0262, 0x0265, 0x0269, 0x026C, 0x026F, 0x0272,
		0x0276, 0x0279, 0x027C, 0x027F, 0x0283, 0x0286, 0x0289, 0x028C,
		0x028F, 0x0292, 0x0295, 0x0298, 0x029B, 0x029E, 0x02A1, 0x02A4,
		0x02A7, 0x02AA, 0x02AD, 0x02B0, 0x02B3, 0x02B6, 0x02B9, 0x02BC,
		0x02BF, 0x02C1, 0x02C4, 0x02C7, 0x02CA, 0x02CD, 0x02D0, 0x02D2,
		0x02D5, 0x02D8, 0x02DA, 0x02DD, 0x02E0, 0x02E3, 0x02E5, 0x02E8,
		0x02EB, 0x02ED, 0x02F0, 0x02F3, 0x02F5, 0x02F8, 0x02FA, 0x02FD,
		0x02FF, 0x0302, 0x0305, 0x0307, 0x030A, 0x030C, 0x030F, 0x0311,
		0x0314, 0x0316, 0x0319, 0x031B, 0x031E, 0x0320, 0x0322, 0x0325,
		0x0327, 0x032A, 0x032C, 0x032E, 0x0331, 0x0333, 0x0336, 0x0338,
		0x033A, 0x033D, 0x033F, 0x0341, 0x0344, 0x0346, 0x0348, 0x034B,
		0x034D, 0x034F, 0x0351, 0x0354, 0x0356, 0x0358, 0x035A, 0x035D,
		0x035F, 0x0361, 0x0363, 0x0366, 0x0368, 0x036A, 0x036C, 0x036E,
		0x0371, 0x0373, 0x0375, 0x0377, 0x0379, 0x037B, 0x037D, 0x0380,
		0x0382, 0x0384, 0x0386, 0x0388, 0x038A, 0x038C, 0x038E, 0x0390,
		0x0393, 0x0395, 0x0397, 0x0399, 0x039B, 0x039D, 0x039F, 0x03A1,
		0x03A3, 0x03A5, 0x03A7, 0x03A9, 0x03AB, 0x03AD, 0x03AF, 0x03B1,
		0x03B3, 0x03B5, 0x03B7, 0x03B9, 0x03BB, 0x03BD, 0x03BF, 0x03C1,
		0x03C3, 0x03C5, 0x03C7, 0x03C9, 0x03CB, 0x03CD, 0x03CF, 0x03D0,
		0x03D2, 0x03D4, 0x03D6, 0x03D8, 0x03DA, 0x03DC, 0x03DE, 0x03E0,
		0x03E2, 0x03E3, 0x03E5, 0x03E7, 0x03E9, 0x03EB, 0x03ED, 0x03EF,
		0x03F0, 0x03F2, 0x03F4, 0x03F6, 0x03F8, 0x03FA, 0x03FB, 0x03FD,
	};
	static const ULLONG dgamma_ofs[D_IM_R2Y_TABLE_MAX_GAMMA] = {
		0xB83926251B1B1B1Aull, 0x18121212120A1211ull, 0x080A0A0A0A0A1109ull, 0x08090A0911090909ull,
		0x0809090909090909ull, 0x0809090901090909ull, 0x0801090901090901ull, 0x0809080901090109ull,
		0x0009010901090109ull, 0x0801090109080109ull, 0x0801090801090808ull, 0x0009080801090808ull,
		0x0001090808010101ull, 0x0808080801010101ull, 0x0808080808010101ull, 0x0001010101090808ull,
		0x0808080808080808ull, 0x0808080808080808ull, 0x0808000101010101ull, 0x0001010108080808ull,
		0x0800010101010108ull, 0x0808080001010108ull, 0x0808000101010808ull, 0x0800010101080800ull,
		0x0001080808000101ull, 0x0808000101080800ull, 0x0001080800010808ull, 0x0001010808000108ull,
		0x0800010808000108ull, 0x0800010808000108ull, 0x0001010800010808ull, 0x0001080001080800ull,
		0x0008000108000108ull, 0x0800010800010800ull, 0x0008000108000108ull, 0x0001080001080001ull,
		0x0800010800010800ull, 0x0008000108000108ull, 0x0001000108000108ull, 0x0001080001000108ull,
		0x0001080001000108ull, 0x0001080008000108ull, 0x0001000108000100ull, 0x0008000100010800ull,
		0x0000010800080001ull, 0x0800080001000108ull, 0x0008000100010800ull, 0x0800010001080008ull,
		0x0001000108000800ull, 0x0000010800080008ull, 0x0001000100010800ull, 0x0800080001000100ull,
		0x0008000800080001ull, 0x0001000100010800ull, 0x0800080008000100ull, 0x0000010001000100ull,
		0x0008000800080008ull, 0x0008000800080008ull, 0x0008000100010001ull, 0x0001000100010001ull,
		0x0001000100010001ull, 0x0001000100010001ull, 0x0001000800080008ull, 0x0008000800080008ull,
		0x0008000800000100ull, 0x0000010001000100ull, 0x0000080008000800ull, 0x0800000100010001ull,
		0x0001000800080008ull, 0x0000010001000100ull, 0x0800080008000001ull, 0x0001000100080008ull,
		0x0000010001000100ull, 0x0800080000010001ull, 0x0008000800000100ull, 0x0000080008000001ull,
		0x0001000800080000ull, 0x0000080008000001ull, 0x0001000800000100ull, 0x0000080000010001ull,
		0x0008000001000100ull, 0x0800000100010008ull, 0x0000010008000800ull, 0x0001000800000100ull,
		0x0000080000010008ull, 0x0000010008000800ull, 0x0001000800000100ull, 0x0800000100080000ull,
		0x0000080008000001ull, 0x0008000001000800ull, 0x0001000800000100ull, 0x0800000100080000ull,
		0x0000080000010008ull, 0x0000080000010008ull, 0x0000010008000001ull, 0x0008000001000800ull,
		0x0008000001000800ull, 0x0001000800000800ull, 0x0001000800000100ull, 0x0800000800000100ull,
		0x0800000800000100ull, 0x0800000100000100ull, 0x0800000100000100ull, 0x0800000100000100ull,
		0x0800000800000100ull, 0x0800000800000100ull, 0x0001000800000800ull, 0x0001000001000800ull,
		0x0008000001000001ull, 0x0008000008000001ull, 0x0000010008000008ull, 0x0000080000010000ull,
		0x0000080000080000ull, 0x0800000100000100ull, 0x0001000800000800ull, 0x0008000001000001ull,
		0x0000010008000008ull, 0x0000080000080000ull, 0x0000000100000100ull, 0x0001000001000800ull,
		0x0008000008000008ull, 0x0000080000080000ull, 0x0800000100000100ull, 0x0001000001000001ull,
		0x0000010000010000ull, 0x0000000100000100ull, 0x0001000001000001ull, 0x0000010000010000ull,
		0x0000000100000100ull, 0x0001000001000001ull, 0x0000010000010000ull, 0x0000000800000800ull,
		0x0008000008000008ull, 0x0000080000080000ull, 0x0001000001000001ull, 0x0000010000010000ull,
		0x0800000800000800ull, 0x0008000000010000ull, 0x0000000100000800ull, 0x0008000008000000ull,
		0x0000000100000100ull, 0x0008000008000008ull, 0x0000000100000100ull, 0x0008000008000008ull,
		0x0000000100000100ull, 0x0008000008000000ull, 0x0000000100000800ull, 0x0008000000010000ull,
		0x0000000800000800ull, 0x0000010000010000ull, 0x0800000001000001ull, 0x0000080000080000ull,
		0x0001000008000008ull, 0x0000000100000800ull, 0x0008000000010000ull, 0x0800000800000001ull,
		0x0000080000000100ull, 0x0001000008000000ull, 0x0000000800000800ull, 0x0000010000080000ull,
		0x0001000008000008ull, 0x0000000100000800ull, 0x0000010000080000ull, 0x0001000008000000ull,
		0x0000000100000800ull, 0x0000010000080000ull, 0x0001000008000000ull, 0x0000000800000001ull,
		0x0000080000000100ull, 0x0008000000010000ull, 0x0800000001000008ull, 0x0000000100000001ull,
		0x0000080000000100ull, 0x0008000000010000ull, 0x0800000001000008ull, 0x0000000800000001ull,
		0x0000080000000100ull, 0x0008000000010000ull, 0x0001000008000000ull, 0x0000000800000008ull,
		0x0000000100000800ull, 0x0000080000000100ull, 0x0008000000010000ull, 0x0001000008000000ull,
		0x0000000001000008ull, 0x0000000800000001ull, 0x0000080000000800ull, 0x0000010000000100ull,
		0x0008000000080000ull, 0x0001000008000000ull, 0x0800000001000000ull, 0x0000000800000008ull,
		0x0000000100000001ull, 0x0000000100000800ull, 0x0000080000000100ull, 0x0000010000080000ull,
		0x0008000000080000ull, 0x0001000000010000ull, 0x0001000008000000ull, 0x0800000008000000ull,
		0x0000000001000000ull, 0x0000000001000008ull, 0x0000000800000008ull, 0x0000000100000001ull,
		0x0000000100000001ull, 0x0000000100000800ull, 0x0000080000000800ull, 0x0000080000000800ull,
		0x0000080000000800ull, 0x0000010000000100ull, 0x0000010000000100ull, 0x0000010000000100ull,
		0x0000010000000100ull, 0x0000010000000100ull, 0x0000010000000100ull, 0x0000010000000100ull,
		0x0000010000000100ull, 0x0000010000000100ull, 0x0000010000000100ull, 0x0000010000000100ull,
		0x0000010000000800ull, 0x0000080000000800ull, 0x0000080000000800ull, 0x0000080000000001ull,
		0x0000000100000001ull, 0x0000000100000001ull, 0x0000000800000008ull, 0x0000000800000008ull,
		0x0000000001000000ull, 0x0000000001000000ull, 0x0000000008000000ull, 0x0800000008000000ull,
		0x0001000000010000ull, 0x0008000000080000ull, 0x0008000000000100ull, 0x0000010000000800ull,
		0x0000080000000800ull, 0x0000000100000001ull, 0x0000000800000008ull, 0x0000000001000000ull,
		0x0000000008000000ull, 0x0800000000010000ull, 0x0001000000080000ull, 0x0008000000000100ull,
		0x0000080000000800ull, 0x0000000100000001ull, 0x0000000800000000ull, 0x0000000001000000ull,
		0x0800000000010000ull, 0x0001000000080000ull, 0x0000010000000100ull, 0x0000080000000001ull,
		0x0000000100000008ull, 0x0000000001000000ull, 0x0800000000010000ull, 0x0001000000080000ull,
	};
	static const T_IM_R2Y_CTRL_GAMMA r2y_ctrl_gamma = {
			.gamma_enable = D_IM_R2Y_ENABLE_ON,
			.gamma_mode = D_IM_R2Y_GAMMA_MODE_12,
			.gamma_yb_tbl_simul = D_IM_R2Y_ENABLE_ON,
	};
	INT32 ercd;

	ercd = Im_R2Y_Ctrl_Gamma( ch, &r2y_ctrl_gamma );
	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( "im_r2y_set_gamma Im_R2Y_Ctrl_Gamma ercd=%d\n", ercd ));
	}
	ercd = Im_R2Y_Set_Gamma_Table( ch, 0, dgamma_abs, dgamma_ofs );
	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( "im_r2y_set_gamma Im_R2Y_Set_Gamma_Table ercd=%d\n", ercd ));
	}
}

// input : RGB-12bit
// output: YC422
INT32 ct_im_cnr_set_im_r2y_set( UCHAR ch )
{
	T_IM_R2Y_CTRL				r2y_ctrl = gct_im_r2y_ctrl_base;
	T_IM_R2Y_CTRL_SDRAM_INPUT	r2y_ctrl_sdram_in = gct_im_r2y_ctrl_sdram_in_rgb12bit;
	T_IM_R2Y_RESIZE_RECT		r2y_resize_rect_param = gct_im_r2y_resize_rect_param_out_yc422;
	T_IM_R2Y_CTRL_TRIMMING_EXT	r2y_ctrl_trimming;
	T_IM_R2Y_INADDR_INFO		r2y_in_addr;
	T_IM_R2Y_OUTBANK_INFO		r2y_addr_0;
	INT32 ercd;

	Ddim_Print(( "R2Y setting\n" ));

	// output select
	r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_0]	= D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_1]	= D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_2]	= D_IM_R2Y_ENABLE_OFF;

	r2y_ctrl_trimming.trimming_enable_b			= D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_trimming.trimming_enable_c			= D_IM_R2Y_ENABLE_ON;
	r2y_ctrl_trimming.trim_window_c.img_top		= 0;
	r2y_ctrl_trimming.trim_window_c.img_left	= 0;
	r2y_ctrl_trimming.trim_window_c.img_width	= 640;
	r2y_ctrl_trimming.trim_window_c.img_lines	= 480;

	// Set address
	// input : RGB 12bit
	r2y_in_addr.rgb.addr_R = (VOID*)D_INPUT_IMG_RGB_R_ADDR;
	r2y_in_addr.rgb.addr_G = (VOID*)D_INPUT_IMG_RGB_G_ADDR;
	r2y_in_addr.rgb.addr_B = (VOID*)D_INPUT_IMG_RGB_B_ADDR;
	// output : YC420 (set 1 bank)
	r2y_addr_0.bank_initial_position	= D_IM_R2Y_YYW_BANK_0;
	r2y_addr_0.use_bank_num				= D_IM_R2Y_YYW_BANK_0;
	r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Y	= (VOID*)D_IM_CNR_TEST_IMG_DST_Y_ADDR;
	r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Cb	= (VOID*)D_IM_CNR_TEST_IMG_DST_CB_ADDR;
	r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Cr	= (VOID*)D_IM_CNR_TEST_IMG_DST_CR_ADDR;

	ercd = Im_R2Y_Init( ch );
	Ddim_Print(( "Im_R2Y_Init() : 0x%x\n", ercd ));

	ercd = Im_R2Y_Ctrl( ch, &r2y_ctrl );
	Ddim_Print(( "Im_R2Y_Ctrl() : 0x%x\n", ercd ));

	ercd = Im_R2Y_Ctrl_ModeSDRAMInput( ch, &r2y_ctrl_sdram_in );
	Ddim_Print(( "Im_R2Y_Ctrl_ModeSDRAMInput() : 0x%x\n", ercd ));

	ercd = Im_R2Y_Set_Resize_Rect( ch, &r2y_resize_rect_param );
	Ddim_Print(( "Im_R2Y_Set_Resize_Rect() : 0x%x\n", ercd ));

	ercd = Im_R2Y_Ctrl_Trimming_External( ch, &r2y_ctrl_trimming );
	Ddim_Print(( "Im_R2Y_Ctrl_Trimming_External() : 0x%x\n", ercd ));

	ercd = Im_R2Y_Set_InAddr_Info( ch, &r2y_in_addr );
	Ddim_Print(( "Im_R2Y_Set_InAddr_Info() : 0x%x\n", ercd ));

	ercd = Im_R2Y_Set_OutBankInfo( ch, D_IM_R2Y_YYW_CH_0, &r2y_addr_0 );
	Ddim_Print(( "Im_R2Y_Set_OutBankInfo() : 0x%x\n", ercd ));

	ercd = Im_R2Y_Set_WB_Gain( ch, &im_r2y_wb_gain );
	Ddim_Print(( "Im_R2Y_Set_WB_Gain() : 0x%x\n", ercd ));

	ct_im_r2y_set_gamma_on( ch );

	return D_DDIM_OK;
}

INT32 ct_im_cnr_set_im_r2y_start( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "R2Y Start\n" ));

#ifdef D_IM_R2Y_DEBUG_ON_PC
	if( ch == 0 ){
		gIM_R2Y_macro_fake_finish[0] = 1;
	}
	else if( ch == 1 ){
		gIM_R2Y_macro_fake_finish[1] = 1;
	}
	else{
		gIM_R2Y_macro_fake_finish[0] = 1;
		gIM_R2Y_macro_fake_finish[1] = 1;
	}
#endif

	ercd = Im_R2Y_Start( ch );
	Ddim_Print(( "Im_R2Y_Start() : 0x%x\n", ercd ));

	return D_DDIM_OK;
}

INT32 ct_im_cnr_set_im_r2y_stop( UCHAR ch )
{
	DDIM_USER_FLGPTN flgptn = 0;
	INT32 ercd;

	if( ch == 0 ){
		flgptn |= D_IM_R2Y1_INT_FLG_YYW0_END;
	}
	else if( ch == 1 ){
		flgptn |= D_IM_R2Y2_INT_FLG_YYW0_END;
	}
	else{
		flgptn |= D_IM_R2Y1_INT_FLG_YYW0_END;
		flgptn |= D_IM_R2Y2_INT_FLG_YYW0_END;
	}
	ercd = Im_R2Y_WaitEnd( NULL, flgptn, 3000 );
	Ddim_Print(( "Im_R2Y_WaitEnd() : 0x%x\n", ercd ));

	ercd = Im_R2Y_Stop( ch );
	Ddim_Print(( "Im_R2Y_Stop() : 0x%x\n", ercd ));

#ifdef D_IM_R2Y_DEBUG_ON_PC
	gIM_R2Y_macro_fake_finish[0] = 0;
	gIM_R2Y_macro_fake_finish[1] = 0;
#endif

	Ddim_Print(( "R2Y End\n" ));

	return D_DDIM_OK;
}
#endif	// D_IM_CNR_R2Y_MACRO_ENABLE



static VOID ct_Im_CNR_get_loop_cnt( UCHAR ch, UCHAR* sta, UCHAR* end )
{
	switch( ch ){
		case 0:
			*sta = 0;
			*end = 0;
			break;
		case 1:
			*sta = 1;
			*end = 1;
			break;
//		case 2:
		default:
			*sta = 0;
			*end = 1;
			break;
	}
}

static VOID ct_Im_CNR_OFL_print_debug_ctrl_reg( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "YUV_DFMT = %u\n",    IO_CNR(loop)->OFL_REG_RW.SPRMOD.bit.YUV_MODE ));
		Ddim_Print(( "HSIZE    = 0x%x\n",  IO_CNR(loop)->OFL_REG_RW.HSIZE.bit.HSIZE ));
		Ddim_Print(( "VSIZE    = 0x%x\n",  IO_CNR(loop)->OFL_REG_RW.VSIZE.bit.VSIZE ));

		Ddim_Print(( "RYDEF    = 0x%x\n",  IO_CNR(loop)->OFL_REG_RW.RYDEF.bit.RYDEF ));
		Ddim_Print(( "RYTA     = 0x%lx\n", IO_CNR(loop)->OFL_REG_RW.RYTA.bit.RYTA ));

		Ddim_Print(( "RCDEF    = 0x%x\n",  IO_CNR(loop)->OFL_REG_RW.RCDEF.bit.RCDEF ));
		Ddim_Print(( "RCBTA    = 0x%lx\n", IO_CNR(loop)->OFL_REG_RW.RCBTA.bit.RCBTA ));
		Ddim_Print(( "RCRTA    = 0x%lx\n", IO_CNR(loop)->OFL_REG_RW.RCRTA.bit.RCRTA ));

		Ddim_Print(( "WYDEF    = 0x%x\n",  IO_CNR(loop)->OFL_REG_RW.WYDEF.bit.WYDEF ));
		Ddim_Print(( "WYTA     = 0x%lx\n", IO_CNR(loop)->OFL_REG_RW.WYTA.bit.WYTA ));

		Ddim_Print(( "WCDEF    = 0x%x\n",  IO_CNR(loop)->OFL_REG_RW.WCDEF.bit.WCDEF ));
		Ddim_Print(( "WCBTA    = 0x%lx\n", IO_CNR(loop)->OFL_REG_RW.WCBTA.bit.WCBTA ));
		Ddim_Print(( "WCRTA    = 0x%lx\n", IO_CNR(loop)->OFL_REG_RW.WCRTA.bit.WCRTA ));

		Ddim_Print(( "TMPTA    = 0x%lx\n", IO_CNR(loop)->OFL_REG_RW.TMPTA.bit.TMPTA ));

		Ddim_Print(( "VDIVT    = %u\n",    IO_CNR(loop)->OFL_REG_RW.VDIV.bit.VDIVT ));
		Ddim_Print(( "VDIVB    = %u\n",    IO_CNR(loop)->OFL_REG_RW.VDIV.bit.VDIVB ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_ctrl_reg( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "YUV_DFMT = %u\n",    IO_CNR(loop)->OTF_REG_RW.SPRMOD.bit.YUV_MODE ));
		Ddim_Print(( "EXMODE   = %u\n",    IO_CNR(loop)->OTF_REG_RW.SPRMOD.bit.EXMODE ));
		Ddim_Print(( "CNRVFM   = %u\n",    IO_CNR(loop)->OTF_REG_RW.SPRMOD.bit.CNRVFM ));

		Ddim_Print(( "HSIZE    = 0x%x\n",  IO_CNR(loop)->OTF_REG_RW.HSIZE.bit.HSIZE ));
		Ddim_Print(( "VSIZE    = 0x%x\n",  IO_CNR(loop)->OTF_REG_RW.VSIZE.bit.VSIZE ));

		Ddim_Print(( "WCDEF    = 0x%x\n",  IO_CNR(loop)->OTF_REG_RW.WCDEF.bit.WCDEF ));
		Ddim_Print(( "WCTA     = 0x%lx\n", IO_CNR(loop)->OTF_REG_RW.WCTA.bit.WCTA ));

		Ddim_Print(( "CNRWOFSX = %u\n",    IO_CNR(loop)->OTF_REG_RW.CNRWOFSX.bit.CNRWOFSX ));

		Ddim_Print(( "CNRCSE   = %u\n",    IO_CNR(loop)->OTF_REG_RW.SPRTRG.bit.CNRCSE ));

		Ddim_Print(( "HDIVL    = %u\n",    IO_CNR(loop)->OTF_REG_RW.HDIV.bit.HDIVL ));
		Ddim_Print(( "HDIVR    = %u\n",    IO_CNR(loop)->OTF_REG_RW.HDIV.bit.HDIVR ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_ctrl_axi( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "ARCACHE = 0x%x\n", IO_CNR(loop)->OFL_REG_RW.SPRAXISET.bit.ARCACHE ));
		Ddim_Print(( "ARPROT  = 0x%x\n", IO_CNR(loop)->OFL_REG_RW.SPRAXISET.bit.ARPROT ));
		Ddim_Print(( "AWCACHE = 0x%x\n", IO_CNR(loop)->OFL_REG_RW.SPRAXISET.bit.AWCACHE ));
		Ddim_Print(( "AWPROT  = 0x%x\n", IO_CNR(loop)->OFL_REG_RW.SPRAXISET.bit.AWPROT ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_ctrl_axi( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "AWCACHE = 0x%x\n", IO_CNR(loop)->OTF_REG_RW.SPRAXISET.bit.AWCACHE ));
		Ddim_Print(( "AWPROT  = 0x%x\n",   IO_CNR(loop)->OTF_REG_RW.SPRAXISET.bit.AWPROT ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_stat_axi( const T_IM_CNR_OFL_AXI_STATUS* const axi_stat )
{
	Ddim_Print(( "read_channel_response  = 0x%x\n", axi_stat->read_channel_response ));
	Ddim_Print(( "write_channel_response = 0x%x\n", axi_stat->write_channel_response ));
}

static VOID ct_Im_CNR_OTF_print_debug_stat_axi( const T_IM_CNR_OTF_AXI_STATUS* const axi_stat )
{
	Ddim_Print(( "write_channel_response = 0x%x\n", axi_stat->write_channel_response ));
}

static VOID ct_Im_CNR_OFL_print_debug_ctrl_c( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "CSPREN    = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.CSPREN.bit.CSPREN ));
		Ddim_Print(( "CGDKEN    = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CGDKEN.bit.CGDKEN ));
		Ddim_Print(( "MCEN      = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCEN.bit.MCEN ));
		Ddim_Print(( "MCYTHH_0  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYTHH.bit.MCYTHH_0 ));
		Ddim_Print(( "MCYTHH_1  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYTHH.bit.MCYTHH_1 ));
		Ddim_Print(( "MCYTHH_2  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYTHH.bit.MCYTHH_2 ));
		Ddim_Print(( "MCYTHH_3  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYTHH.bit.MCYTHH_3 ));
		Ddim_Print(( "MCYTHV_0  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYTHV.bit.MCYTHV_0 ));
		Ddim_Print(( "MCYTHV_1  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYTHV.bit.MCYTHV_1 ));
		Ddim_Print(( "MCYTHV_2  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYTHV.bit.MCYTHV_2 ));
		Ddim_Print(( "MCYTHV_3  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYTHV.bit.MCYTHV_3 ));
		Ddim_Print(( "MCCTHH_0  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCTHH.bit.MCCTHH_0 ));
		Ddim_Print(( "MCCTHH_1  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCTHH.bit.MCCTHH_1 ));
		Ddim_Print(( "MCCTHH_2  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCTHH.bit.MCCTHH_2 ));
		Ddim_Print(( "MCCTHH_3  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCTHH.bit.MCCTHH_3 ));
		Ddim_Print(( "MCCTHV_0  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCTHV.bit.MCCTHV_0 ));
		Ddim_Print(( "MCCTHV_1  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCTHV.bit.MCCTHV_1 ));
		Ddim_Print(( "MCCTHV_2  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCTHV.bit.MCCTHV_2 ));
		Ddim_Print(( "MCCTHV_3  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCTHV.bit.MCCTHV_3 ));

		Ddim_Print(( "MCYDYMYCR = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYDYMDT.bit.MCYDYMYCR ));
		Ddim_Print(( "MCYDYMYG  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYDYMDT.bit.MCYDYMYG ));
		Ddim_Print(( "MCYDYM    = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYDYM.bit.MCYDYM ));

		Ddim_Print(( "MCYSCL    = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.MCYSCL.bit.MCYSCL ));
		Ddim_Print(( "MCSSCL    = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.MCSSCL.bit.MCSSCL ));
		Ddim_Print(( "MCCSCL    = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.MCCSCL.bit.MCCSCL ));

		Ddim_Print(( "LCEN      = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCEN.bit.LCEN ));
		Ddim_Print(( "LCYTHH_0  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYTHH.bit.LCYTHH_0 ));
		Ddim_Print(( "LCYTHH_1  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYTHH.bit.LCYTHH_1 ));
		Ddim_Print(( "LCYTHH_2  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYTHH.bit.LCYTHH_2 ));
		Ddim_Print(( "LCYTHH_3  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYTHH.bit.LCYTHH_3 ));
		Ddim_Print(( "LCYTHV_0  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYTHV.bit.LCYTHV_0 ));
		Ddim_Print(( "LCYTHV_1  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYTHV.bit.LCYTHV_1 ));
		Ddim_Print(( "LCYTHV_2  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYTHV.bit.LCYTHV_2 ));
		Ddim_Print(( "LCYTHV_3  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYTHV.bit.LCYTHV_3 ));
		Ddim_Print(( "LCCTHH_0  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCTHH.bit.LCCTHH_0 ));
		Ddim_Print(( "LCCTHH_1  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCTHH.bit.LCCTHH_1 ));
		Ddim_Print(( "LCCTHH_2  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCTHH.bit.LCCTHH_2 ));
		Ddim_Print(( "LCCTHH_3  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCTHH.bit.LCCTHH_3 ));
		Ddim_Print(( "LCCTHV_0  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCTHV.bit.LCCTHV_0 ));
		Ddim_Print(( "LCCTHV_1  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCTHV.bit.LCCTHV_1 ));
		Ddim_Print(( "LCCTHV_2  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCTHV.bit.LCCTHV_2 ));
		Ddim_Print(( "LCCTHV_3  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCTHV.bit.LCCTHV_3 ));

		Ddim_Print(( "LCYDYMYCR = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYDYMDT.bit.LCYDYMYCR ));
		Ddim_Print(( "LCYDYMYG  = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYDYMDT.bit.LCYDYMYG ));
		Ddim_Print(( "LCYDYM    = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYDYM.bit.LCYDYM ));

		Ddim_Print(( "LCYSCL    = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.LCYSCL.bit.LCYSCL ));
		Ddim_Print(( "LCSSCL    = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.LCSSCL.bit.LCSSCL ));
		Ddim_Print(( "LCCSCL    = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.LCCSCL.bit.LCCSCL ));

		Ddim_Print(( "WHTEN     = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.WHTMD.bit.WHTEN ));
		Ddim_Print(( "WHTVAL    = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.WHTMD.bit.WHTVAL ));
		Ddim_Print(( "WHTBASE   = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.WHTMD.bit.WHTBASE ));
		Ddim_Print(( "BLENDMD   = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.BLEND.bit.BLENDMD ));
		Ddim_Print(( "CSPRALPBD = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.BLEND.bit.CSPRALPBD ));
		Ddim_Print(( "CSRGEN    = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.BLEND.bit.CSRGEN ));
		Ddim_Print(( "CSRGMD    = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.BLEND.bit.CSRGMD ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_ctrl_c( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "CSPREN    = %u\n",   IO_CNR(loop)->OTF_CSPR_REG.CSPREN.bit.CSPREN ));
		Ddim_Print(( "CGDKEN    = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CGDKEN.bit.CGDKEN ));
		Ddim_Print(( "MCEN      = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCEN.bit.MCEN ));
		Ddim_Print(( "MCYTHH_0  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYTHH.bit.MCYTHH_0 ));
		Ddim_Print(( "MCYTHH_1  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYTHH.bit.MCYTHH_1 ));
		Ddim_Print(( "MCYTHH_2  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYTHH.bit.MCYTHH_2 ));
		Ddim_Print(( "MCYTHH_3  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYTHH.bit.MCYTHH_3 ));
		Ddim_Print(( "MCYTHV_0  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYTHV.bit.MCYTHV_0 ));
		Ddim_Print(( "MCYTHV_1  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYTHV.bit.MCYTHV_1 ));
		Ddim_Print(( "MCYTHV_2  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYTHV.bit.MCYTHV_2 ));
		Ddim_Print(( "MCYTHV_3  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYTHV.bit.MCYTHV_3 ));
		Ddim_Print(( "MCCTHH_0  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCTHH.bit.MCCTHH_0 ));
		Ddim_Print(( "MCCTHH_1  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCTHH.bit.MCCTHH_1 ));
		Ddim_Print(( "MCCTHH_2  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCTHH.bit.MCCTHH_2 ));
		Ddim_Print(( "MCCTHH_3  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCTHH.bit.MCCTHH_3 ));
		Ddim_Print(( "MCCTHV_0  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCTHV.bit.MCCTHV_0 ));
		Ddim_Print(( "MCCTHV_1  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCTHV.bit.MCCTHV_1 ));
		Ddim_Print(( "MCCTHV_2  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCTHV.bit.MCCTHV_2 ));
		Ddim_Print(( "MCCTHV_3  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCTHV.bit.MCCTHV_3 ));

		Ddim_Print(( "MCYDYMYCR = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYDYMDT.bit.MCYDYMYCR ));
		Ddim_Print(( "MCYDYMYG  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYDYMDT.bit.MCYDYMYG ));
		Ddim_Print(( "MCYDYM    = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYDYM.bit.MCYDYM ));

		Ddim_Print(( "MCYSCL    = %u\n",   IO_CNR(loop)->OTF_CSPR_REG.MCYSCL.bit.MCYSCL ));
		Ddim_Print(( "MCCSCL    = %u\n",   IO_CNR(loop)->OTF_CSPR_REG.MCCSCL.bit.MCCSCL ));

		Ddim_Print(( "LCEN      = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCEN.bit.LCEN ));
		Ddim_Print(( "LCYTHH_0  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYTHH.bit.LCYTHH_0 ));
		Ddim_Print(( "LCYTHH_1  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYTHH.bit.LCYTHH_1 ));
		Ddim_Print(( "LCYTHH_2  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYTHH.bit.LCYTHH_2 ));
		Ddim_Print(( "LCYTHH_3  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYTHH.bit.LCYTHH_3 ));
		Ddim_Print(( "LCYTHV_0  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYTHV.bit.LCYTHV_0 ));
		Ddim_Print(( "LCYTHV_1  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYTHV.bit.LCYTHV_1 ));
		Ddim_Print(( "LCYTHV_2  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYTHV.bit.LCYTHV_2 ));
		Ddim_Print(( "LCYTHV_3  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYTHV.bit.LCYTHV_3 ));
		Ddim_Print(( "LCCTHH_0  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCTHH.bit.LCCTHH_0 ));
		Ddim_Print(( "LCCTHH_1  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCTHH.bit.LCCTHH_1 ));
		Ddim_Print(( "LCCTHH_2  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCTHH.bit.LCCTHH_2 ));
		Ddim_Print(( "LCCTHH_3  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCTHH.bit.LCCTHH_3 ));
		Ddim_Print(( "LCCTHV_0  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCTHV.bit.LCCTHV_0 ));
		Ddim_Print(( "LCCTHV_1  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCTHV.bit.LCCTHV_1 ));
		Ddim_Print(( "LCCTHV_2  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCTHV.bit.LCCTHV_2 ));
		Ddim_Print(( "LCCTHV_3  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCTHV.bit.LCCTHV_3 ));

		Ddim_Print(( "LCYDYMYCR = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYDYMDT.bit.LCYDYMYCR ));
		Ddim_Print(( "LCYDYMYG  = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYDYMDT.bit.LCYDYMYG ));
		Ddim_Print(( "LCYDYM    = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYDYM.bit.LCYDYM ));

		Ddim_Print(( "LCYSCL    = %u\n",   IO_CNR(loop)->OTF_CSPR_REG.LCYSCL.bit.LCYSCL ));
		Ddim_Print(( "LCCSCL    = %u\n",   IO_CNR(loop)->OTF_CSPR_REG.LCCSCL.bit.LCCSCL ));

		Ddim_Print(( "WHTVAL    = %u\n",   IO_CNR(loop)->OTF_CSPR_REG.WHTMD.bit.WHTVAL ));
		Ddim_Print(( "BLENDMD   = %u\n",   IO_CNR(loop)->OTF_CSPR_REG.BLEND.bit.BLENDMD ));
		Ddim_Print(( "CSPRALPBD = %u\n",   IO_CNR(loop)->OTF_CSPR_REG.BLEND.bit.CSPRALPBD ));
		Ddim_Print(( "CSRGMD    = %u\n",   IO_CNR(loop)->OTF_CSPR_REG.BLEND.bit.CSRGMD ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_ctrl_y( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "YSPREN    = %u\n",   IO_CNR(loop)->OFL_REG_YSPR.YSPREN.bit.YSPREN ));
		Ddim_Print(( "YSPRMD    = %u\n",   IO_CNR(loop)->OFL_REG_YSPR.YSPRMODE.bit.YSPRMD ));
		Ddim_Print(( "YSPRFE    = %u\n",   IO_CNR(loop)->OFL_REG_YSPR.YSPRMODE.bit.YSPRFE ));
		Ddim_Print(( "YSPRFXEYH = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRFXEY.bit.YSPRFXEYH ));
		Ddim_Print(( "YSPRFXEYV = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRFXEY.bit.YSPRFXEYV ));
		Ddim_Print(( "YSPRFXECH = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRFXEC.bit.YSPRFXECH ));
		Ddim_Print(( "YSPRFXECV = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRFXEC.bit.YSPRFXECV ));
		Ddim_Print(( "YSPRALPBD = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRALPBD.bit.YSPRALPBD ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_c_mid_y_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "MCYSCLBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_1 ));
		Ddim_Print(( "MCYSCLBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_2 ));
		Ddim_Print(( "MCYSCLBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_3 ));
		Ddim_Print(( "MCYSCLBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_4 ));
		Ddim_Print(( "MCYSCLBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD2.bit.MCYSCLBD_5 ));

		Ddim_Print(( "MCYSCLOF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_0 ));
		Ddim_Print(( "MCYSCLOF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_1 ));
		Ddim_Print(( "MCYSCLOF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_2 ));
		Ddim_Print(( "MCYSCLOF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_3 ));
		Ddim_Print(( "MCYSCLOF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF2.bit.MCYSCLOF_4 ));
		Ddim_Print(( "MCYSCLOF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF2.bit.MCYSCLOF_5 ));

		Ddim_Print(( "MCYSCLGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA1.bit.MCYSCLGA_0 ));
		Ddim_Print(( "MCYSCLGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA1.bit.MCYSCLGA_1 ));
		Ddim_Print(( "MCYSCLGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA1.bit.MCYSCLGA_2 ));
		Ddim_Print(( "MCYSCLGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA1.bit.MCYSCLGA_3 ));
		Ddim_Print(( "MCYSCLGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA2.bit.MCYSCLGA_4 ));
		Ddim_Print(( "MCYSCLGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA2.bit.MCYSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_c_mid_y_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "MCYSCLBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_1 ));
		Ddim_Print(( "MCYSCLBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_2 ));
		Ddim_Print(( "MCYSCLBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_3 ));
		Ddim_Print(( "MCYSCLBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_4 ));
		Ddim_Print(( "MCYSCLBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD2.bit.MCYSCLBD_5 ));

		Ddim_Print(( "MCYSCLOF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_0 ));
		Ddim_Print(( "MCYSCLOF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_1 ));
		Ddim_Print(( "MCYSCLOF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_2 ));
		Ddim_Print(( "MCYSCLOF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_3 ));
		Ddim_Print(( "MCYSCLOF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF2.bit.MCYSCLOF_4 ));
		Ddim_Print(( "MCYSCLOF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF2.bit.MCYSCLOF_5 ));

		Ddim_Print(( "MCYSCLGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA1.bit.MCYSCLGA_0 ));
		Ddim_Print(( "MCYSCLGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA1.bit.MCYSCLGA_1 ));
		Ddim_Print(( "MCYSCLGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA1.bit.MCYSCLGA_2 ));
		Ddim_Print(( "MCYSCLGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA1.bit.MCYSCLGA_3 ));
		Ddim_Print(( "MCYSCLGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA2.bit.MCYSCLGA_4 ));
		Ddim_Print(( "MCYSCLGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA2.bit.MCYSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_c_mid_cc_c_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "MCSSCLBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_1 ));
		Ddim_Print(( "MCSSCLBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_2 ));
		Ddim_Print(( "MCSSCLBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_3 ));
		Ddim_Print(( "MCSSCLBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_4 ));
		Ddim_Print(( "MCSSCLBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD2.bit.MCSSCLBD_5 ));

		Ddim_Print(( "MCSSCLOF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_0 ));
		Ddim_Print(( "MCSSCLOF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_1 ));
		Ddim_Print(( "MCSSCLOF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_2 ));
		Ddim_Print(( "MCSSCLOF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_3 ));
		Ddim_Print(( "MCSSCLOF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF2.bit.MCSSCLOF_4 ));
		Ddim_Print(( "MCSSCLOF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF2.bit.MCSSCLOF_5 ));

		Ddim_Print(( "MCSSCLGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA1.bit.MCSSCLGA_0 ));
		Ddim_Print(( "MCSSCLGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA1.bit.MCSSCLGA_1 ));
		Ddim_Print(( "MCSSCLGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA1.bit.MCSSCLGA_2 ));
		Ddim_Print(( "MCSSCLGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA1.bit.MCSSCLGA_3 ));
		Ddim_Print(( "MCSSCLGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA2.bit.MCSSCLGA_4 ));
		Ddim_Print(( "MCSSCLGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA2.bit.MCSSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_c_mid_cc_c_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "MCSSCLBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_1 ));
		Ddim_Print(( "MCSSCLBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_2 ));
		Ddim_Print(( "MCSSCLBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_3 ));
		Ddim_Print(( "MCSSCLBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_4 ));
		Ddim_Print(( "MCSSCLBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD2.bit.MCSSCLBD_5 ));

		Ddim_Print(( "MCSSCLOF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_0 ));
		Ddim_Print(( "MCSSCLOF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_1 ));
		Ddim_Print(( "MCSSCLOF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_2 ));
		Ddim_Print(( "MCSSCLOF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_3 ));
		Ddim_Print(( "MCSSCLOF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF2.bit.MCSSCLOF_4 ));
		Ddim_Print(( "MCSSCLOF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF2.bit.MCSSCLOF_5 ));

		Ddim_Print(( "MCSSCLGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA1.bit.MCSSCLGA_0 ));
		Ddim_Print(( "MCSSCLGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA1.bit.MCSSCLGA_1 ));
		Ddim_Print(( "MCSSCLGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA1.bit.MCSSCLGA_2 ));
		Ddim_Print(( "MCSSCLGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA1.bit.MCSSCLGA_3 ));
		Ddim_Print(( "MCSSCLGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA2.bit.MCSSCLGA_4 ));
		Ddim_Print(( "MCSSCLGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA2.bit.MCSSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_c_mid_cc_y_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "MCCSCLBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_1 ));
		Ddim_Print(( "MCCSCLBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_2 ));
		Ddim_Print(( "MCCSCLBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_3 ));
		Ddim_Print(( "MCCSCLBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_4 ));
		Ddim_Print(( "MCCSCLBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD2.bit.MCCSCLBD_5 ));

		Ddim_Print(( "MCCSCLOF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_0 ));
		Ddim_Print(( "MCCSCLOF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_1 ));
		Ddim_Print(( "MCCSCLOF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_2 ));
		Ddim_Print(( "MCCSCLOF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_3 ));
		Ddim_Print(( "MCCSCLOF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF2.bit.MCCSCLOF_4 ));
		Ddim_Print(( "MCCSCLOF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF2.bit.MCCSCLOF_5 ));

		Ddim_Print(( "MCCSCLGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA1.bit.MCCSCLGA_0 ));
		Ddim_Print(( "MCCSCLGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA1.bit.MCCSCLGA_1 ));
		Ddim_Print(( "MCCSCLGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA1.bit.MCCSCLGA_2 ));
		Ddim_Print(( "MCCSCLGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA1.bit.MCCSCLGA_3 ));
		Ddim_Print(( "MCCSCLGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA2.bit.MCCSCLGA_4 ));
		Ddim_Print(( "MCCSCLGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA2.bit.MCCSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_c_mid_cc_y_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "MCCSCLBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_1 ));
		Ddim_Print(( "MCCSCLBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_2 ));
		Ddim_Print(( "MCCSCLBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_3 ));
		Ddim_Print(( "MCCSCLBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_4 ));
		Ddim_Print(( "MCCSCLBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD2.bit.MCCSCLBD_5 ));

		Ddim_Print(( "MCCSCLOF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_0 ));
		Ddim_Print(( "MCCSCLOF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_1 ));
		Ddim_Print(( "MCCSCLOF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_2 ));
		Ddim_Print(( "MCCSCLOF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_3 ));
		Ddim_Print(( "MCCSCLOF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF2.bit.MCCSCLOF_4 ));
		Ddim_Print(( "MCCSCLOF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF2.bit.MCCSCLOF_5 ));

		Ddim_Print(( "MCCSCLGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA1.bit.MCCSCLGA_0 ));
		Ddim_Print(( "MCCSCLGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA1.bit.MCCSCLGA_1 ));
		Ddim_Print(( "MCCSCLGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA1.bit.MCCSCLGA_2 ));
		Ddim_Print(( "MCCSCLGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA1.bit.MCCSCLGA_3 ));
		Ddim_Print(( "MCCSCLGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA2.bit.MCCSCLGA_4 ));
		Ddim_Print(( "MCCSCLGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA2.bit.MCCSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_c_low_y_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "LCYSCLBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_1 ));
		Ddim_Print(( "LCYSCLBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_2 ));
		Ddim_Print(( "LCYSCLBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_3 ));
		Ddim_Print(( "LCYSCLBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_4 ));
		Ddim_Print(( "LCYSCLBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD2.bit.LCYSCLBD_5 ));

		Ddim_Print(( "LCYSCLOF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_0 ));
		Ddim_Print(( "LCYSCLOF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_1 ));
		Ddim_Print(( "LCYSCLOF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_2 ));
		Ddim_Print(( "LCYSCLOF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_3 ));
		Ddim_Print(( "LCYSCLOF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF2.bit.LCYSCLOF_4 ));
		Ddim_Print(( "LCYSCLOF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF2.bit.LCYSCLOF_5 ));

		Ddim_Print(( "LCYSCLGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA1.bit.LCYSCLGA_0 ));
		Ddim_Print(( "LCYSCLGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA1.bit.LCYSCLGA_1 ));
		Ddim_Print(( "LCYSCLGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA1.bit.LCYSCLGA_2 ));
		Ddim_Print(( "LCYSCLGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA1.bit.LCYSCLGA_3 ));
		Ddim_Print(( "LCYSCLGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA2.bit.LCYSCLGA_4 ));
		Ddim_Print(( "LCYSCLGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA2.bit.LCYSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_c_low_y_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "LCYSCLBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_1 ));
		Ddim_Print(( "LCYSCLBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_2 ));
		Ddim_Print(( "LCYSCLBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_3 ));
		Ddim_Print(( "LCYSCLBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_4 ));
		Ddim_Print(( "LCYSCLBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD2.bit.LCYSCLBD_5 ));

		Ddim_Print(( "LCYSCLOF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_0 ));
		Ddim_Print(( "LCYSCLOF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_1 ));
		Ddim_Print(( "LCYSCLOF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_2 ));
		Ddim_Print(( "LCYSCLOF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_3 ));
		Ddim_Print(( "LCYSCLOF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF2.bit.LCYSCLOF_4 ));
		Ddim_Print(( "LCYSCLOF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF2.bit.LCYSCLOF_5 ));

		Ddim_Print(( "LCYSCLGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA1.bit.LCYSCLGA_0 ));
		Ddim_Print(( "LCYSCLGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA1.bit.LCYSCLGA_1 ));
		Ddim_Print(( "LCYSCLGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA1.bit.LCYSCLGA_2 ));
		Ddim_Print(( "LCYSCLGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA1.bit.LCYSCLGA_3 ));
		Ddim_Print(( "LCYSCLGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA2.bit.LCYSCLGA_4 ));
		Ddim_Print(( "LCYSCLGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA2.bit.LCYSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_c_low_cc_c_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "LCSSCLBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_1 ));
		Ddim_Print(( "LCSSCLBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_2 ));
		Ddim_Print(( "LCSSCLBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_3 ));
		Ddim_Print(( "LCSSCLBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_4 ));
		Ddim_Print(( "LCSSCLBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD2.bit.LCSSCLBD_5 ));

		Ddim_Print(( "LCSSCLOF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_0 ));
		Ddim_Print(( "LCSSCLOF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_1 ));
		Ddim_Print(( "LCSSCLOF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_2 ));
		Ddim_Print(( "LCSSCLOF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_3 ));
		Ddim_Print(( "LCSSCLOF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF2.bit.LCSSCLOF_4 ));
		Ddim_Print(( "LCSSCLOF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF2.bit.LCSSCLOF_5 ));

		Ddim_Print(( "LCSSCLGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA1.bit.LCSSCLGA_0 ));
		Ddim_Print(( "LCSSCLGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA1.bit.LCSSCLGA_1 ));
		Ddim_Print(( "LCSSCLGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA1.bit.LCSSCLGA_2 ));
		Ddim_Print(( "LCSSCLGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA1.bit.LCSSCLGA_3 ));
		Ddim_Print(( "LCSSCLGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA2.bit.LCSSCLGA_4 ));
		Ddim_Print(( "LCSSCLGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA2.bit.LCSSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_cspr_edge_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "CESCLBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_1 ));
		Ddim_Print(( "CESCLBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_2 ));
		Ddim_Print(( "CESCLBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_3 ));
		Ddim_Print(( "CESCLBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_4 ));
		Ddim_Print(( "CESCLBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLBD.CESCLBD2.bit.CESCLBD_5 ));

		Ddim_Print(( "CESCLOF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_0 ));
		Ddim_Print(( "CESCLOF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_1 ));
		Ddim_Print(( "CESCLOF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_2 ));
		Ddim_Print(( "CESCLOF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_3 ));
		Ddim_Print(( "CESCLOF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLOF.CESCLOF2.bit.CESCLOF_4 ));
		Ddim_Print(( "CESCLOF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLOF.CESCLOF2.bit.CESCLOF_5 ));

		Ddim_Print(( "CESCLGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLGA.CESCLGA1.bit.CESCLGA_0 ));
		Ddim_Print(( "CESCLGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLGA.CESCLGA1.bit.CESCLGA_1 ));
		Ddim_Print(( "CESCLGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLGA.CESCLGA1.bit.CESCLGA_2 ));
		Ddim_Print(( "CESCLGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLGA.CESCLGA1.bit.CESCLGA_3 ));
		Ddim_Print(( "CESCLGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLGA.CESCLGA2.bit.CESCLGA_4 ));
		Ddim_Print(( "CESCLGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CESCLGA.CESCLGA2.bit.CESCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_cspr_edge_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "CESCLBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_1 ));
		Ddim_Print(( "CESCLBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_2 ));
		Ddim_Print(( "CESCLBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_3 ));
		Ddim_Print(( "CESCLBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_4 ));
		Ddim_Print(( "CESCLBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLBD.CESCLBD2.bit.CESCLBD_5 ));

		Ddim_Print(( "CESCLOF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_0 ));
		Ddim_Print(( "CESCLOF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_1 ));
		Ddim_Print(( "CESCLOF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_2 ));
		Ddim_Print(( "CESCLOF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_3 ));
		Ddim_Print(( "CESCLOF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLOF.CESCLOF2.bit.CESCLOF_4 ));
		Ddim_Print(( "CESCLOF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLOF.CESCLOF2.bit.CESCLOF_5 ));

		Ddim_Print(( "CESCLGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLGA.CESCLGA1.bit.CESCLGA_0 ));
		Ddim_Print(( "CESCLGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLGA.CESCLGA1.bit.CESCLGA_1 ));
		Ddim_Print(( "CESCLGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLGA.CESCLGA1.bit.CESCLGA_2 ));
		Ddim_Print(( "CESCLGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLGA.CESCLGA1.bit.CESCLGA_3 ));
		Ddim_Print(( "CESCLGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLGA.CESCLGA2.bit.CESCLGA_4 ));
		Ddim_Print(( "CESCLGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CESCLGA.CESCLGA2.bit.CESCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_c_low_cc_c_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "LCSSCLBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_1 ));
		Ddim_Print(( "LCSSCLBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_2 ));
		Ddim_Print(( "LCSSCLBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_3 ));
		Ddim_Print(( "LCSSCLBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_4 ));
		Ddim_Print(( "LCSSCLBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD2.bit.LCSSCLBD_5 ));

		Ddim_Print(( "LCSSCLOF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_0 ));
		Ddim_Print(( "LCSSCLOF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_1 ));
		Ddim_Print(( "LCSSCLOF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_2 ));
		Ddim_Print(( "LCSSCLOF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_3 ));
		Ddim_Print(( "LCSSCLOF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF2.bit.LCSSCLOF_4 ));
		Ddim_Print(( "LCSSCLOF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF2.bit.LCSSCLOF_5 ));

		Ddim_Print(( "LCSSCLGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA1.bit.LCSSCLGA_0 ));
		Ddim_Print(( "LCSSCLGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA1.bit.LCSSCLGA_1 ));
		Ddim_Print(( "LCSSCLGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA1.bit.LCSSCLGA_2 ));
		Ddim_Print(( "LCSSCLGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA1.bit.LCSSCLGA_3 ));
		Ddim_Print(( "LCSSCLGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA2.bit.LCSSCLGA_4 ));
		Ddim_Print(( "LCSSCLGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA2.bit.LCSSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_c_low_cc_y_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "LCCSCLBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_1 ));
		Ddim_Print(( "LCCSCLBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_2 ));
		Ddim_Print(( "LCCSCLBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_3 ));
		Ddim_Print(( "LCCSCLBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_4 ));
		Ddim_Print(( "LCCSCLBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD2.bit.LCCSCLBD_5 ));

		Ddim_Print(( "LCCSCLOF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_0 ));
		Ddim_Print(( "LCCSCLOF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_1 ));
		Ddim_Print(( "LCCSCLOF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_2 ));
		Ddim_Print(( "LCCSCLOF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_3 ));
		Ddim_Print(( "LCCSCLOF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF2.bit.LCCSCLOF_4 ));
		Ddim_Print(( "LCCSCLOF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF2.bit.LCCSCLOF_5 ));

		Ddim_Print(( "LCCSCLGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA1.bit.LCCSCLGA_0 ));
		Ddim_Print(( "LCCSCLGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA1.bit.LCCSCLGA_1 ));
		Ddim_Print(( "LCCSCLGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA1.bit.LCCSCLGA_2 ));
		Ddim_Print(( "LCCSCLGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA1.bit.LCCSCLGA_3 ));
		Ddim_Print(( "LCCSCLGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA2.bit.LCCSCLGA_4 ));
		Ddim_Print(( "LCCSCLGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA2.bit.LCCSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_c_low_cc_y_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "LCCSCLBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_1 ));
		Ddim_Print(( "LCCSCLBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_2 ));
		Ddim_Print(( "LCCSCLBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_3 ));
		Ddim_Print(( "LCCSCLBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_4 ));
		Ddim_Print(( "LCCSCLBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD2.bit.LCCSCLBD_5 ));

		Ddim_Print(( "LCCSCLOF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_0 ));
		Ddim_Print(( "LCCSCLOF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_1 ));
		Ddim_Print(( "LCCSCLOF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_2 ));
		Ddim_Print(( "LCCSCLOF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_3 ));
		Ddim_Print(( "LCCSCLOF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF2.bit.LCCSCLOF_4 ));
		Ddim_Print(( "LCCSCLOF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF2.bit.LCCSCLOF_5 ));

		Ddim_Print(( "LCCSCLGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA1.bit.LCCSCLGA_0 ));
		Ddim_Print(( "LCCSCLGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA1.bit.LCCSCLGA_1 ));
		Ddim_Print(( "LCCSCLGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA1.bit.LCCSCLGA_2 ));
		Ddim_Print(( "LCCSCLGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA1.bit.LCCSCLGA_3 ));
		Ddim_Print(( "LCCSCLGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA2.bit.LCCSCLGA_4 ));
		Ddim_Print(( "LCCSCLGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA2.bit.LCCSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_y_tbl( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "YSPRDYEYBD_1 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_1 ));
		Ddim_Print(( "YSPRDYEYBD_2 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_2 ));
		Ddim_Print(( "YSPRDYEYBD_3 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_3 ));
		Ddim_Print(( "YSPRDYEYBD_4 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_4 ));
		Ddim_Print(( "YSPRDYEYBD_5 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD2.bit.YSPRDYEYBD_5 ));

		Ddim_Print(( "YSPRDYEYOF_0 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_0 ));
		Ddim_Print(( "YSPRDYEYOF_1 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_1 ));
		Ddim_Print(( "YSPRDYEYOF_2 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_2 ));
		Ddim_Print(( "YSPRDYEYOF_3 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_3 ));
		Ddim_Print(( "YSPRDYEYOF_4 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF2.bit.YSPRDYEYOF_4 ));
		Ddim_Print(( "YSPRDYEYOF_5 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF2.bit.YSPRDYEYOF_5 ));

		Ddim_Print(( "YSPRDYEYGA_0 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA1.bit.YSPRDYEYGA_0 ));
		Ddim_Print(( "YSPRDYEYGA_1 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA1.bit.YSPRDYEYGA_1 ));
		Ddim_Print(( "YSPRDYEYGA_2 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA1.bit.YSPRDYEYGA_2 ));
		Ddim_Print(( "YSPRDYEYGA_3 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA1.bit.YSPRDYEYGA_3 ));
		Ddim_Print(( "YSPRDYEYGA_4 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA2.bit.YSPRDYEYGA_4 ));
		Ddim_Print(( "YSPRDYEYGA_5 = 0x%x\n", IO_CNR(loop)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA2.bit.YSPRDYEYGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_ctrl_src_cc( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "SPRS       = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.SPRS.bit.SPRS ));

		// CB
		Ddim_Print(( "SPRSCBBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD1.bit.SPRSCBBD_1 ));
		Ddim_Print(( "SPRSCBBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD1.bit.SPRSCBBD_2 ));
		Ddim_Print(( "SPRSCBBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD1.bit.SPRSCBBD_3 ));
		Ddim_Print(( "SPRSCBBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD1.bit.SPRSCBBD_4 ));
		Ddim_Print(( "SPRSCBBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD2.bit.SPRSCBBD_5 ));

		Ddim_Print(( "SPRSCBOF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF1.bit.SPRSCBOF_0 ));
		Ddim_Print(( "SPRSCBOF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF1.bit.SPRSCBOF_1 ));
		Ddim_Print(( "SPRSCBOF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF1.bit.SPRSCBOF_2 ));
		Ddim_Print(( "SPRSCBOF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF1.bit.SPRSCBOF_3 ));
		Ddim_Print(( "SPRSCBOF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF2.bit.SPRSCBOF_4 ));
		Ddim_Print(( "SPRSCBOF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF2.bit.SPRSCBOF_5 ));

		Ddim_Print(( "SPRSCBGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA1.bit.SPRSCBGA_0 ));
		Ddim_Print(( "SPRSCBGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA1.bit.SPRSCBGA_1 ));
		Ddim_Print(( "SPRSCBGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA2.bit.SPRSCBGA_2 ));
		Ddim_Print(( "SPRSCBGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA2.bit.SPRSCBGA_3 ));
		Ddim_Print(( "SPRSCBGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA3.bit.SPRSCBGA_4 ));
		Ddim_Print(( "SPRSCBGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA3.bit.SPRSCBGA_5 ));

		// CR
		Ddim_Print(( "SPRSCRBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD1.bit.SPRSCRBD_1 ));
		Ddim_Print(( "SPRSCRBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD1.bit.SPRSCRBD_2 ));
		Ddim_Print(( "SPRSCRBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD1.bit.SPRSCRBD_3 ));
		Ddim_Print(( "SPRSCRBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD1.bit.SPRSCRBD_4 ));
		Ddim_Print(( "SPRSCRBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD2.bit.SPRSCRBD_5 ));

		Ddim_Print(( "SPRSCROF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCROF.SPRSCROF1.bit.SPRSCROF_0 ));
		Ddim_Print(( "SPRSCROF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCROF.SPRSCROF1.bit.SPRSCROF_1 ));
		Ddim_Print(( "SPRSCROF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCROF.SPRSCROF1.bit.SPRSCROF_2 ));
		Ddim_Print(( "SPRSCROF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCROF.SPRSCROF1.bit.SPRSCROF_3 ));
		Ddim_Print(( "SPRSCROF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCROF.SPRSCROF2.bit.SPRSCROF_4 ));
		Ddim_Print(( "SPRSCROF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCROF.SPRSCROF2.bit.SPRSCROF_5 ));

		Ddim_Print(( "SPRSCRGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA1.bit.SPRSCRGA_0 ));
		Ddim_Print(( "SPRSCRGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA1.bit.SPRSCRGA_1 ));
		Ddim_Print(( "SPRSCRGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA2.bit.SPRSCRGA_2 ));
		Ddim_Print(( "SPRSCRGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA2.bit.SPRSCRGA_3 ));
		Ddim_Print(( "SPRSCRGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA3.bit.SPRSCRGA_4 ));
		Ddim_Print(( "SPRSCRGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA3.bit.SPRSCRGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_ctrl_src_cc( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "SPRS       = %u\n",   IO_CNR(loop)->OTF_CSPR_REG.SPRS.bit.SPRS ));

		// CB
		Ddim_Print(( "SPRSCBBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD1.bit.SPRSCBBD_1 ));
		Ddim_Print(( "SPRSCBBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD1.bit.SPRSCBBD_2 ));
		Ddim_Print(( "SPRSCBBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD1.bit.SPRSCBBD_3 ));
		Ddim_Print(( "SPRSCBBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD1.bit.SPRSCBBD_4 ));
		Ddim_Print(( "SPRSCBBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD2.bit.SPRSCBBD_5 ));

		Ddim_Print(( "SPRSCBOF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF1.bit.SPRSCBOF_0 ));
		Ddim_Print(( "SPRSCBOF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF1.bit.SPRSCBOF_1 ));
		Ddim_Print(( "SPRSCBOF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF1.bit.SPRSCBOF_2 ));
		Ddim_Print(( "SPRSCBOF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF1.bit.SPRSCBOF_3 ));
		Ddim_Print(( "SPRSCBOF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF2.bit.SPRSCBOF_4 ));
		Ddim_Print(( "SPRSCBOF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF2.bit.SPRSCBOF_5 ));

		Ddim_Print(( "SPRSCBGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA1.bit.SPRSCBGA_0 ));
		Ddim_Print(( "SPRSCBGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA1.bit.SPRSCBGA_1 ));
		Ddim_Print(( "SPRSCBGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA2.bit.SPRSCBGA_2 ));
		Ddim_Print(( "SPRSCBGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA2.bit.SPRSCBGA_3 ));
		Ddim_Print(( "SPRSCBGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA3.bit.SPRSCBGA_4 ));
		Ddim_Print(( "SPRSCBGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA3.bit.SPRSCBGA_5 ));

		// CR
		Ddim_Print(( "SPRSCRBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD1.bit.SPRSCRBD_1 ));
		Ddim_Print(( "SPRSCRBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD1.bit.SPRSCRBD_2 ));
		Ddim_Print(( "SPRSCRBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD1.bit.SPRSCRBD_3 ));
		Ddim_Print(( "SPRSCRBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD1.bit.SPRSCRBD_4 ));
		Ddim_Print(( "SPRSCRBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD2.bit.SPRSCRBD_5 ));

		Ddim_Print(( "SPRSCROF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCROF.SPRSCROF1.bit.SPRSCROF_0 ));
		Ddim_Print(( "SPRSCROF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCROF.SPRSCROF1.bit.SPRSCROF_1 ));
		Ddim_Print(( "SPRSCROF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCROF.SPRSCROF1.bit.SPRSCROF_2 ));
		Ddim_Print(( "SPRSCROF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCROF.SPRSCROF1.bit.SPRSCROF_3 ));
		Ddim_Print(( "SPRSCROF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCROF.SPRSCROF2.bit.SPRSCROF_4 ));
		Ddim_Print(( "SPRSCROF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCROF.SPRSCROF2.bit.SPRSCROF_5 ));

		Ddim_Print(( "SPRSCRGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA1.bit.SPRSCRGA_0 ));
		Ddim_Print(( "SPRSCRGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA1.bit.SPRSCRGA_1 ));
		Ddim_Print(( "SPRSCRGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA2.bit.SPRSCRGA_2 ));
		Ddim_Print(( "SPRSCRGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA2.bit.SPRSCRGA_3 ));
		Ddim_Print(( "SPRSCRGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA3.bit.SPRSCRGA_4 ));
		Ddim_Print(( "SPRSCRGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA3.bit.SPRSCRGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_ctrl_dst_cc( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "SPRD       = %u\n",   IO_CNR(loop)->OFL_CSPR_REG.SPRD.bit.SPRD ));

		// CB
		Ddim_Print(( "SPRSCBBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD1.bit.SPRDCBBD_1 ));
		Ddim_Print(( "SPRSCBBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD1.bit.SPRDCBBD_2 ));
		Ddim_Print(( "SPRSCBBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD1.bit.SPRDCBBD_3 ));
		Ddim_Print(( "SPRSCBBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD1.bit.SPRDCBBD_4 ));
		Ddim_Print(( "SPRSCBBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD2.bit.SPRDCBBD_5 ));

		Ddim_Print(( "SPRSCBOF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF1.bit.SPRDCBOF_0 ));
		Ddim_Print(( "SPRSCBOF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF1.bit.SPRDCBOF_1 ));
		Ddim_Print(( "SPRSCBOF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF1.bit.SPRDCBOF_2 ));
		Ddim_Print(( "SPRSCBOF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF1.bit.SPRDCBOF_3 ));
		Ddim_Print(( "SPRSCBOF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF2.bit.SPRDCBOF_4 ));
		Ddim_Print(( "SPRSCBOF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF2.bit.SPRDCBOF_5 ));

		Ddim_Print(( "SPRSCBGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA1.bit.SPRDCBGA_0 ));
		Ddim_Print(( "SPRSCBGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA1.bit.SPRDCBGA_1 ));
		Ddim_Print(( "SPRSCBGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA2.bit.SPRDCBGA_2 ));
		Ddim_Print(( "SPRSCBGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA2.bit.SPRDCBGA_3 ));
		Ddim_Print(( "SPRSCBGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA3.bit.SPRDCBGA_4 ));
		Ddim_Print(( "SPRSCBGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA3.bit.SPRDCBGA_5 ));

		// CR
		Ddim_Print(( "SPRSCRBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD1.bit.SPRDCRBD_1 ));
		Ddim_Print(( "SPRSCRBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD1.bit.SPRDCRBD_2 ));
		Ddim_Print(( "SPRSCRBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD1.bit.SPRDCRBD_3 ));
		Ddim_Print(( "SPRSCRBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD1.bit.SPRDCRBD_4 ));
		Ddim_Print(( "SPRSCRBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD2.bit.SPRDCRBD_5 ));

		Ddim_Print(( "SPRSCROF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCROF.SPRDCROF1.bit.SPRDCROF_0 ));
		Ddim_Print(( "SPRSCROF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCROF.SPRDCROF1.bit.SPRDCROF_1 ));
		Ddim_Print(( "SPRSCROF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCROF.SPRDCROF1.bit.SPRDCROF_2 ));
		Ddim_Print(( "SPRSCROF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCROF.SPRDCROF1.bit.SPRDCROF_3 ));
		Ddim_Print(( "SPRSCROF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCROF.SPRDCROF2.bit.SPRDCROF_4 ));
		Ddim_Print(( "SPRSCROF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCROF.SPRDCROF2.bit.SPRDCROF_5 ));

		Ddim_Print(( "SPRSCRGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA1.bit.SPRDCRGA_0 ));
		Ddim_Print(( "SPRSCRGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA1.bit.SPRDCRGA_1 ));
		Ddim_Print(( "SPRSCRGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA2.bit.SPRDCRGA_2 ));
		Ddim_Print(( "SPRSCRGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA2.bit.SPRDCRGA_3 ));
		Ddim_Print(( "SPRSCRGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA3.bit.SPRDCRGA_4 ));
		Ddim_Print(( "SPRSCRGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA3.bit.SPRDCRGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_ctrl_dst_cc( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "SPRD       = %u\n",   IO_CNR(loop)->OTF_CSPR_REG.SPRD.bit.SPRD ));

		// CB
		Ddim_Print(( "SPRSCBBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD1.bit.SPRDCBBD_1 ));
		Ddim_Print(( "SPRSCBBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD1.bit.SPRDCBBD_2 ));
		Ddim_Print(( "SPRSCBBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD1.bit.SPRDCBBD_3 ));
		Ddim_Print(( "SPRSCBBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD1.bit.SPRDCBBD_4 ));
		Ddim_Print(( "SPRSCBBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD2.bit.SPRDCBBD_5 ));

		Ddim_Print(( "SPRSCBOF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF1.bit.SPRDCBOF_0 ));
		Ddim_Print(( "SPRSCBOF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF1.bit.SPRDCBOF_1 ));
		Ddim_Print(( "SPRSCBOF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF1.bit.SPRDCBOF_2 ));
		Ddim_Print(( "SPRSCBOF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF1.bit.SPRDCBOF_3 ));
		Ddim_Print(( "SPRSCBOF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF2.bit.SPRDCBOF_4 ));
		Ddim_Print(( "SPRSCBOF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF2.bit.SPRDCBOF_5 ));

		Ddim_Print(( "SPRSCBGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA1.bit.SPRDCBGA_0 ));
		Ddim_Print(( "SPRSCBGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA1.bit.SPRDCBGA_1 ));
		Ddim_Print(( "SPRSCBGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA2.bit.SPRDCBGA_2 ));
		Ddim_Print(( "SPRSCBGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA2.bit.SPRDCBGA_3 ));
		Ddim_Print(( "SPRSCBGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA3.bit.SPRDCBGA_4 ));
		Ddim_Print(( "SPRSCBGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA3.bit.SPRDCBGA_5 ));

		// CR
		Ddim_Print(( "SPRSCRBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD1.bit.SPRDCRBD_1 ));
		Ddim_Print(( "SPRSCRBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD1.bit.SPRDCRBD_2 ));
		Ddim_Print(( "SPRSCRBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD1.bit.SPRDCRBD_3 ));
		Ddim_Print(( "SPRSCRBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD1.bit.SPRDCRBD_4 ));
		Ddim_Print(( "SPRSCRBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD2.bit.SPRDCRBD_5 ));

		Ddim_Print(( "SPRSCROF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCROF.SPRDCROF1.bit.SPRDCROF_0 ));
		Ddim_Print(( "SPRSCROF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCROF.SPRDCROF1.bit.SPRDCROF_1 ));
		Ddim_Print(( "SPRSCROF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCROF.SPRDCROF1.bit.SPRDCROF_2 ));
		Ddim_Print(( "SPRSCROF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCROF.SPRDCROF1.bit.SPRDCROF_3 ));
		Ddim_Print(( "SPRSCROF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCROF.SPRDCROF2.bit.SPRDCROF_4 ));
		Ddim_Print(( "SPRSCROF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCROF.SPRDCROF2.bit.SPRDCROF_5 ));

		Ddim_Print(( "SPRSCRGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA1.bit.SPRDCRGA_0 ));
		Ddim_Print(( "SPRSCRGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA1.bit.SPRDCRGA_1 ));
		Ddim_Print(( "SPRSCRGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA2.bit.SPRDCRGA_2 ));
		Ddim_Print(( "SPRSCRGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA2.bit.SPRDCRGA_3 ));
		Ddim_Print(( "SPRSCRGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA3.bit.SPRDCRGA_4 ));
		Ddim_Print(( "SPRSCRGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA3.bit.SPRDCRGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OFL_print_debug_ctrl_hue_cc( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "CDCB      = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CPHPT.bit.CDCB ));
		Ddim_Print(( "CDCR      = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CPHPT.bit.CDCR ));
		Ddim_Print(( "KCB       = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CPHPTK.bit.KCB ));
		Ddim_Print(( "KCR       = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CPHPTK.bit.KCR ));

		Ddim_Print(( "CDSCLBD_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLBD.CDSCLBD1.bit.CDSCLBD_1 ));
		Ddim_Print(( "CDSCLBD_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLBD.CDSCLBD1.bit.CDSCLBD_2 ));
		Ddim_Print(( "CDSCLBD_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLBD.CDSCLBD2.bit.CDSCLBD_3 ));
		Ddim_Print(( "CDSCLBD_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLBD.CDSCLBD2.bit.CDSCLBD_4 ));
		Ddim_Print(( "CDSCLBD_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLBD.CDSCLBD3.bit.CDSCLBD_5 ));

		Ddim_Print(( "CDSCLOF_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLOF.CDSCLOF1.bit.CDSCLOF_0 ));
		Ddim_Print(( "CDSCLOF_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLOF.CDSCLOF1.bit.CDSCLOF_1 ));
		Ddim_Print(( "CDSCLOF_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLOF.CDSCLOF2.bit.CDSCLOF_2 ));
		Ddim_Print(( "CDSCLOF_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLOF.CDSCLOF2.bit.CDSCLOF_3 ));
		Ddim_Print(( "CDSCLOF_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLOF.CDSCLOF3.bit.CDSCLOF_4 ));
		Ddim_Print(( "CDSCLOF_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLOF.CDSCLOF3.bit.CDSCLOF_5 ));

		Ddim_Print(( "CDSCLGA_0 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLGA.CDSCLGA1.bit.CDSCLGA_0 ));
		Ddim_Print(( "CDSCLGA_1 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLGA.CDSCLGA1.bit.CDSCLGA_1 ));
		Ddim_Print(( "CDSCLGA_2 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLGA.CDSCLGA2.bit.CDSCLGA_2 ));
		Ddim_Print(( "CDSCLGA_3 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLGA.CDSCLGA2.bit.CDSCLGA_3 ));
		Ddim_Print(( "CDSCLGA_4 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLGA.CDSCLGA3.bit.CDSCLGA_4 ));
		Ddim_Print(( "CDSCLGA_5 = 0x%x\n", IO_CNR(loop)->OFL_CSPR_REG.CDSCLGA.CDSCLGA3.bit.CDSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}

static VOID ct_Im_CNR_OTF_print_debug_ctrl_hue_cc( UCHAR ch )
{
	UCHAR loop, loop_sta, loop_end;

	Im_CNR_On_Pclk(ch);

	ct_Im_CNR_get_loop_cnt( ch, &loop_sta, &loop_end );

	for( loop = loop_sta; loop <= loop_end; loop++ ){
		Ddim_Print(( "ch[%d]\n",           loop ));
		Ddim_Print(( "CDCB       = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CPHPT.bit.CDCB ));
		Ddim_Print(( "CDCR       = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CPHPT.bit.CDCR ));
		Ddim_Print(( "KCB        = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CPHPTK.bit.KCB ));
		Ddim_Print(( "KCR        = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CPHPTK.bit.KCR ));

		Ddim_Print(( "SPRSCRBD_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLBD.CDSCLBD1.bit.CDSCLBD_1 ));
		Ddim_Print(( "SPRSCRBD_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLBD.CDSCLBD1.bit.CDSCLBD_2 ));
		Ddim_Print(( "SPRSCRBD_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLBD.CDSCLBD2.bit.CDSCLBD_3 ));
		Ddim_Print(( "SPRSCRBD_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLBD.CDSCLBD2.bit.CDSCLBD_4 ));
		Ddim_Print(( "SPRSCRBD_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLBD.CDSCLBD3.bit.CDSCLBD_5 ));

		Ddim_Print(( "SPRSCROF_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLOF.CDSCLOF1.bit.CDSCLOF_0 ));
		Ddim_Print(( "SPRSCROF_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLOF.CDSCLOF1.bit.CDSCLOF_1 ));
		Ddim_Print(( "SPRSCROF_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLOF.CDSCLOF2.bit.CDSCLOF_2 ));
		Ddim_Print(( "SPRSCROF_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLOF.CDSCLOF2.bit.CDSCLOF_3 ));
		Ddim_Print(( "SPRSCROF_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLOF.CDSCLOF3.bit.CDSCLOF_4 ));
		Ddim_Print(( "SPRSCROF_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLOF.CDSCLOF3.bit.CDSCLOF_5 ));

		Ddim_Print(( "SPRSCRGA_0 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLGA.CDSCLGA1.bit.CDSCLGA_0 ));
		Ddim_Print(( "SPRSCRGA_1 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLGA.CDSCLGA1.bit.CDSCLGA_1 ));
		Ddim_Print(( "SPRSCRGA_2 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLGA.CDSCLGA2.bit.CDSCLGA_2 ));
		Ddim_Print(( "SPRSCRGA_3 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLGA.CDSCLGA2.bit.CDSCLGA_3 ));
		Ddim_Print(( "SPRSCRGA_4 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLGA.CDSCLGA3.bit.CDSCLGA_4 ));
		Ddim_Print(( "SPRSCRGA_5 = 0x%x\n", IO_CNR(loop)->OTF_CSPR_REG.CDSCLGA.CDSCLGA3.bit.CDSCLGA_5 ));
	}

	Im_CNR_Off_Pclk(ch);
}


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

#ifdef CO_CNR_DEBUG
extern VOID Im_CNR_Print_ClockStatus( VOID );
#endif



#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_1: "
INT32 CT_IM_CNR_OFL_1_1( UCHAR ch )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_IM_CNR_SEM_TIMEOUT_ERR ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_2: "
INT32 CT_IM_CNR_OFL_1_2( UCHAR ch )
{
	INT32 im_ercd;
	DDIM_USER_ID sid_back;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	sid_back = SID_IM_CNR_OFL(ch);
	SID_IM_CNR_OFL(ch) = 0xfff;

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_IM_CNR_SEM_NG ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		Im_CNR_OFL_Close(ch);
		return im_ercd;
	}

	SID_IM_CNR_OFL(ch) = sid_back;

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_3: "
INT32 CT_IM_CNR_OFL_1_3( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OFL_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OFL_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.r_y_width = D_IM_CNR_TEST_H_PIXS,
		.r_y_addr = D_IM_CNR_TEST_IMG_SRC_Y_ADDR,

		.r_c_width = D_IM_CNR_TEST_H_PIXS,
		.r_cb_addr = D_IM_CNR_TEST_IMG_SRC_CB_ADDR,
		.r_cr_addr = D_IM_CNR_TEST_IMG_SRC_CR_ADDR,

		.w_y_width = D_IM_CNR_TEST_H_PIXS,
		.w_y_addr = D_IM_CNR_TEST_IMG_DST_Y_ADDR,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,
		.w_cb_addr = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
		.w_cr_addr = D_IM_CNR_TEST_IMG_DST_CR_ADDR,

		.work_addr = D_IM_CNR_TEST_IMG_TMP_ADDR,

		.divide_top = E_IM_CNR_VPROCESS_NORM,
		.divide_bot = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_OFL_CTRL_Y spr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.threshold_type = E_IM_CNR_THRES_REGISTER,
		.level = E_IM_CNR_LEVEL_HIGH,
		.h_y_threshold = 0xFF,
		.v_y_threshold = 0xFF,
		.h_c_threshold = 0xFF,
		.v_c_threshold = 0xFF,
		.alpha_blend_ratio = 0x1F,
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_Y( ch, &spr_ctrl_y );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_Y error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Start_Sync( ch, 10 );
	if( im_ercd != D_IM_CNR_SEM_TIMEOUT_ERR ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_CNR_OFL_Close(ch);
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_4: "
INT32 CT_IM_CNR_OFL_1_4( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 wait_factor;
	T_IM_CNR_OFL_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OFL_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.r_y_width = D_IM_CNR_TEST_H_PIXS,
		.r_y_addr = D_IM_CNR_TEST_IMG_SRC_Y_ADDR,

		.r_c_width = D_IM_CNR_TEST_H_PIXS,
		.r_cb_addr = D_IM_CNR_TEST_IMG_SRC_CB_ADDR,
		.r_cr_addr = D_IM_CNR_TEST_IMG_SRC_CR_ADDR,

		.w_y_width = D_IM_CNR_TEST_H_PIXS,
		.w_y_addr = D_IM_CNR_TEST_IMG_DST_Y_ADDR,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,
		.w_cb_addr = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
		.w_cr_addr = D_IM_CNR_TEST_IMG_DST_CR_ADDR,

		.work_addr = D_IM_CNR_TEST_IMG_TMP_ADDR,

		.divide_top = E_IM_CNR_VPROCESS_NORM,
		.divide_bot = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_OFL_CTRL_Y spr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.threshold_type = E_IM_CNR_THRES_REGISTER,
		.level = E_IM_CNR_LEVEL_HIGH,
		.h_y_threshold = 0xFF,
		.v_y_threshold = 0xFF,
		.h_c_threshold = 0xFF,
		.v_c_threshold = 0xFF,
		.alpha_blend_ratio = 0x1F,
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	Im_CNR_OFL_Init(ch);


	im_ercd = Im_CNR_OFL_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_Y( ch, &spr_ctrl_y );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_Y error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Ddim_Print(( "[Before Start]ch0 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.ACESEN.bit.SPRACS ));
	Ddim_Print(( "[Before Start]ch1 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.ACESEN.bit.SPRACS ));

	im_ercd = Im_CNR_OFL_Start_Async(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start Async error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Ddim_Print(( "[After Start]ch0 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.ACESEN.bit.SPRACS ));
	Ddim_Print(( "[After Start]ch1 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.ACESEN.bit.SPRACS ));

#ifdef CO_DEBUG_ON_PC
	if( ch == D_IM_CNR_CH_2 ){
		IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.SPRICF.bit.__SPDF = 1;
		IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.SPRICF.bit.__SPDF = 1;
		Im_CNR0_Int_Handler();
		Im_CNR1_Int_Handler();
	}
	else{
		IO_CNR(ch)->OFL_REG_RW.SPRICF.bit.__SPDF = 1;
		if( ch == D_IM_CNR_CH_0 ){
			Im_CNR0_Int_Handler();
		}
		else{
			Im_CNR1_Int_Handler();
		}
	}
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_CNR_OFL_Wait_End( ch, &wait_factor, 1000 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Ddim_Print(( "[After Stop]ch0 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.ACESEN.bit.SPRACS ));
	Ddim_Print(( "[After Stop]ch1 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.ACESEN.bit.SPRACS ));

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_5: "
INT32 CT_IM_CNR_OFL_1_5( UCHAR ch )
{
	INT32 im_ercd;
#ifdef CO_DEBUG_ON_PC
	UINT32 wait_factor;
#endif //CO_DEBUG_ON_PC

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Start_Async(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start Async error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Wait_End( ch, &wait_factor, 10 );
	if( im_ercd != D_IM_CNR_SEM_TIMEOUT_ERR ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
#else //!CO_DEBUG_ON_PC
	Ddim_Print(( "%s: Only debug on PC\n", D_IM_CNR_FUNC_NAME ));
	im_ercd = D_DDIM_SYSTEM_ERROR;
#endif //!CO_DEBUG_ON_PC

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_6: "
INT32 CT_IM_CNR_OFL_1_6( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 wait_factor;
	DDIM_USER_ID fid_back;
	T_IM_CNR_OFL_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OFL_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.r_y_width = D_IM_CNR_TEST_H_PIXS,
		.r_y_addr = D_IM_CNR_TEST_IMG_SRC_Y_ADDR,

		.r_c_width = D_IM_CNR_TEST_H_PIXS,
		.r_cb_addr = D_IM_CNR_TEST_IMG_SRC_CB_ADDR,
		.r_cr_addr = D_IM_CNR_TEST_IMG_SRC_CR_ADDR,

		.w_y_width = D_IM_CNR_TEST_H_PIXS,
		.w_y_addr = D_IM_CNR_TEST_IMG_DST_Y_ADDR,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,
		.w_cb_addr = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
		.w_cr_addr = D_IM_CNR_TEST_IMG_DST_CR_ADDR,

		.work_addr = D_IM_CNR_TEST_IMG_TMP_ADDR,

		.divide_top = E_IM_CNR_VPROCESS_NORM,
		.divide_bot = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_OFL_CTRL_Y spr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.threshold_type = E_IM_CNR_THRES_REGISTER,
		.level = E_IM_CNR_LEVEL_HIGH,
		.h_y_threshold = 0xFF,
		.v_y_threshold = 0xFF,
		.h_c_threshold = 0xFF,
		.v_c_threshold = 0xFF,
		.alpha_blend_ratio = 0x1F,
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}


	im_ercd = Im_CNR_OFL_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_Y( ch, &spr_ctrl_y );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_Y error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Start_Async(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start Async error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	fid_back = FID_IM_CNR;
	FID_IM_CNR = 0xfff;
	im_ercd = Im_CNR_OFL_Wait_End( ch, &wait_factor, 10 );
	if( im_ercd != D_IM_CNR_SEM_NG ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	FID_IM_CNR = fid_back;

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_7: "
INT32 CT_IM_CNR_OFL_1_7( UCHAR ch )
{
	INT32 im_ercd;
	DDIM_USER_ID sid_back;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	sid_back = SID_IM_CNR_OFL(ch);
	SID_IM_CNR_OFL(ch) = 0xfff;

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_IM_CNR_SEM_NG ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	SID_IM_CNR_OFL(ch) = sid_back;

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_8: "
INT32 CT_IM_CNR_OFL_1_8( UCHAR ch )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_9: "
INT32 CT_IM_CNR_OFL_1_9( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_OFL_CTRL spr_ctrl[2] = {
		[0] = {	// max
			.mode = (E_IM_CNR_OFL_YCC_MODE)7,

			.width = 0x7FFF,
			.lines = 0x7FFF,

			.r_y_width = 0x3FFFF,
			.r_y_addr = 0xFFFFFFFF,

			.r_c_width = 0x3FFFF,
			.r_cb_addr = 0xFFFFFFFF,
			.r_cr_addr = 0xFFFFFFFF,

			.w_y_width = 0x3FFFF,
			.w_y_addr = 0xFFFFFFFF,

			.w_c_width = 0x3FFFF,
			.w_cb_addr = 0xFFFFFFFF,
			.w_cr_addr = 0xFFFFFFFF,

			.work_addr = 0xFFFFFFFF,

			.divide_top = (E_IM_CNR_VPROCESS)1,
			.divide_bot = (E_IM_CNR_VPROCESS)1,

			.pCallBack = (T_IM_CNR_CALLBACK)0xFFFFFFFF,
			.user_param = 0xFFFFFFFF,
		},
		[1] = {	// min
			.mode = (E_IM_CNR_OFL_YCC_MODE)0,

			.width = 0,
			.lines = 0,

			.r_y_width = 0,
			.r_y_addr = 0,

			.r_c_width = 0,
			.r_cb_addr = 0,
			.r_cr_addr = 0,

			.w_y_width = 0,
			.w_y_addr = 0,

			.w_c_width = 0,
			.w_cb_addr = 0,
			.w_cr_addr = 0,

			.work_addr = 0,

			.divide_top = (E_IM_CNR_VPROCESS)0,
			.divide_bot = (E_IM_CNR_VPROCESS)0,

			.pCallBack = (T_IM_CNR_CALLBACK)0,
			.user_param = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_ctrl) / sizeof(spr_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Ctrl(ch, &spr_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_ctrl_reg(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_10: "
INT32 CT_IM_CNR_OFL_1_10( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_OFL_AXI axi_ctrl[2] = {
		[0] = {	// max
			.read_cache_type = 0xF,
			.read_protection_type = 7,
			.write_cache_type = 0xF,
			.write_protection_type = 7,
		},
		[1] = {	// min
			.read_cache_type = 0,
			.read_protection_type = 0,
			.write_cache_type = 0,
			.write_protection_type = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(axi_ctrl) / sizeof(axi_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Ctrl_Axi( ch, &axi_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_ctrl_axi(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_11: "
INT32 CT_IM_CNR_OFL_1_11( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OFL_AXI_STATUS axi_stat;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	IO_CNR(ch)->OFL_REG_RW.SPRAXISET.bit.RRESP = 3;
	IO_CNR(ch)->OFL_REG_RW.SPRAXISET.bit.WRESP = 3;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_CNR_OFL_Get_AXI_Status( ch, &axi_stat );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Get error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	ct_Im_CNR_OFL_print_debug_stat_axi( &axi_stat );

#ifdef CO_DEBUG_ON_PC
	IO_CNR(ch)->OFL_REG_RW.SPRAXISET.bit.RRESP = 0;
	IO_CNR(ch)->OFL_REG_RW.SPRAXISET.bit.WRESP = 0;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_CNR_OFL_Get_AXI_Status( ch, &axi_stat );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Get error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	ct_Im_CNR_OFL_print_debug_stat_axi( &axi_stat );

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_12: "
INT32 CT_IM_CNR_OFL_1_12( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_CTRL_C spr_ctrl[3] = {
		[0] = {	// max
			.enable = (E_IM_CNR_ONOFF)1,
			.gradation_keep_en = (E_IM_CNR_ONOFF)1,
			.mid_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)1,
				.h_y_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.v_y_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.h_c_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.v_c_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.y_threshold_cor = 0xFF,
				.y_threshold_gain = 0x01FF,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)1,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)1,
				.c_y_threshold_scl_en = (E_IM_CNR_ONOFF)1,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)1,
			},
			.low_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)1,
				.h_y_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.v_y_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.h_c_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.v_c_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.y_threshold_cor = 0xFF,
				.y_threshold_gain = 0x01FF,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)1,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)1,
				.c_y_threshold_scl_en = (E_IM_CNR_ONOFF)1,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)1,
			},
			.whitening_enable = (E_IM_CNR_ONOFF)1,
			.whitening_value = 0xF,
			.whitening_base = (E_IM_CNR_WHITEN_BASE)3,
			.c_blend_md = E_IM_CNR_C_BLEND_MD_HUE_SPEC,
			.c_alpha_blend = 0xF,
			.c_range = (E_IM_CNR_ONOFF)1,
			.c_range_mode = E_IM_CNR_C_RANGE_MODE_0_64,
		},
		[1] = {	// min
			.enable = (E_IM_CNR_ONOFF)0,
			.gradation_keep_en = (E_IM_CNR_ONOFF)0,
			.mid_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)0,
				.h_y_threshold = {
					0, 0, 0, 0,
				},
				.v_y_threshold = {
					0, 0, 0, 0,
				},
				.h_c_threshold = {
					0, 0, 0, 0,
				},
				.v_c_threshold = {
					0, 0, 0, 0,
				},
				.y_threshold_cor = 0x00,
				.y_threshold_gain = 0x0000,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)0,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)0,
				.c_y_threshold_scl_en = (E_IM_CNR_ONOFF)0,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)0,
			},
			.low_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)0,
				.h_y_threshold = {
					0, 0, 0, 0,
				},
				.v_y_threshold = {
					0, 0, 0, 0,
				},
				.h_c_threshold = {
					0, 0, 0, 0,
				},
				.v_c_threshold = {
					0, 0, 0, 0,
				},
				.y_threshold_cor = 0x00,
				.y_threshold_gain = 0x0000,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)0,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)0,
				.c_y_threshold_scl_en = (E_IM_CNR_ONOFF)0,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)0,
			},
			.whitening_enable = (E_IM_CNR_ONOFF)0,
			.whitening_value = 0,
			.whitening_base = (E_IM_CNR_WHITEN_BASE)0,
			.c_blend_md = E_IM_CNR_C_BLEND_MD_FIX,
			.c_alpha_blend = 0,
			.c_range = (E_IM_CNR_ONOFF)0,
			.c_range_mode = E_IM_CNR_C_RANGE_MODE_0_1,
		},
		[2] = {	// countup
			.enable = (E_IM_CNR_ONOFF)0,
			.gradation_keep_en = (E_IM_CNR_ONOFF)1,
			.mid_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)1,
				.h_y_threshold = {
					0x1, 0x2, 0x3, 0x4,
				},
				.v_y_threshold = {
					0x5, 0x6, 0x7, 0x8,
				},
				.h_c_threshold = {
					0x9, 0xA, 0xB, 0xC,
				},
				.v_c_threshold = {
					0xD, 0xE, 0xF, 0x10,
				},
				.y_threshold_cor = 0x11,
				.y_threshold_gain = 0x0012,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)1,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)0,
				.c_y_threshold_scl_en = (E_IM_CNR_ONOFF)1,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)0,
			},
			.low_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)0,
				.h_y_threshold = {
					0x21, 0x22, 0x23, 0x24,
				},
				.v_y_threshold = {
					0x25, 0x26, 0x27, 0x28,
				},
				.h_c_threshold = {
					0x29, 0x2A, 0x2B, 0x2C,
				},
				.v_c_threshold = {
					0x2D, 0x2E, 0x2F, 0x30,
				},
				.y_threshold_cor = 0x31,
				.y_threshold_gain = 0x0032,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)1,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)1,
				.c_y_threshold_scl_en = (E_IM_CNR_ONOFF)0,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)0,
			},
			.whitening_enable = (E_IM_CNR_ONOFF)1,
			.whitening_value = 7,
			.whitening_base = (E_IM_CNR_WHITEN_BASE)2,
			.c_blend_md = E_IM_CNR_C_BLEND_MD_COL_DIFF_EDGE,
			.c_alpha_blend = 8,
			.c_range = (E_IM_CNR_ONOFF)1,
			.c_range_mode = E_IM_CNR_C_RANGE_MODE_0_8,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_ctrl) / sizeof(spr_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Ctrl_C( ch, &spr_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_ctrl_c(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_13: "
INT32 CT_IM_CNR_OFL_1_13( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_OFL_CTRL_Y spr_ctrl[2] = {
		[0] = {	// max
			.enable = (E_IM_CNR_ONOFF)1,
			.threshold_type = (E_IM_CNR_THRESHOLD)1,
			.level = (E_IM_CNR_LEVEL)3,
			.h_y_threshold = 0xFF,
			.v_y_threshold = 0xFF,
			.h_c_threshold = 0xFF,
			.v_c_threshold = 0xFF,
			.alpha_blend_ratio = 0x1F,
		},
		[1] = {	// min
			.enable = (E_IM_CNR_ONOFF)0,
			.threshold_type = (E_IM_CNR_THRESHOLD)0,
			.level = (E_IM_CNR_LEVEL)0,
			.h_y_threshold = 0,
			.v_y_threshold = 0,
			.h_c_threshold = 0,
			.v_c_threshold = 0,
			.alpha_blend_ratio = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_ctrl) / sizeof(spr_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Ctrl_Y( ch, &spr_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_ctrl_y(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_14: "
INT32 CT_IM_CNR_OFL_1_14( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Set_CSPR_Mid_Y_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_c_mid_y_tbl(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_15: "
INT32 CT_IM_CNR_OFL_1_15( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Set_CSPR_Mid_CC_Y_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_c_mid_cc_y_tbl(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_16: "
INT32 CT_IM_CNR_OFL_1_16( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Set_CSPR_Mid_CC_C_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_c_mid_cc_c_tbl(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_17: "
INT32 CT_IM_CNR_OFL_1_17( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Set_CSPR_Low_Y_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_c_low_y_tbl(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_18: "
INT32 CT_IM_CNR_OFL_1_18( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Set_CSPR_Low_CC_Y_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_c_low_cc_y_tbl(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_19: "
INT32 CT_IM_CNR_OFL_1_19( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Set_CSPR_Low_CC_C_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_c_low_cc_c_tbl(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_20: "
INT32 CT_IM_CNR_OFL_1_20( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Set_YSPR_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_y_tbl(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_21: "
INT32 CT_IM_CNR_OFL_1_21( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OFL_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OFL_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.r_y_width = D_IM_CNR_TEST_H_PIXS,
		.r_y_addr = D_IM_CNR_TEST_IMG_SRC_Y_ADDR,

		.r_c_width = D_IM_CNR_TEST_H_PIXS,
		.r_cb_addr = D_IM_CNR_TEST_IMG_SRC_CB_ADDR,
		.r_cr_addr = D_IM_CNR_TEST_IMG_SRC_CR_ADDR,

		.w_y_width = D_IM_CNR_TEST_H_PIXS,
		.w_y_addr = D_IM_CNR_TEST_IMG_DST_Y_ADDR,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,
		.w_cb_addr = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
		.w_cr_addr = D_IM_CNR_TEST_IMG_DST_CR_ADDR,

		.work_addr = D_IM_CNR_TEST_IMG_TMP_ADDR,

		.divide_top = E_IM_CNR_VPROCESS_NORM,
		.divide_bot = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_OFL_CTRL_Y spr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.threshold_type = E_IM_CNR_THRES_REGISTER,
		.level = E_IM_CNR_LEVEL_HIGH,
		.h_y_threshold = 0xFF,
		.v_y_threshold = 0xFF,
		.h_c_threshold = 0xFF,
		.v_c_threshold = 0xFF,
		.alpha_blend_ratio = 0x1F,
	};


	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_CNR_OFL_Init(ch);

	im_ercd = Im_CNR_OFL_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_Y( ch, &spr_ctrl_y );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_Y error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Start_Sync( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_22: "
INT32 CT_IM_CNR_OFL_1_22( UCHAR ch )
{
	INT32 im_ercd;
	const char dumpmsg[] = "ch%d SPRTRG = %u\n";

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	IO_CNR(ch)->OFL_REG_RW.SPRTRG.bit.SPRTRG = 3;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_CNR_OFL_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	Im_CNR_On_Pclk(ch);
	Ddim_Print(( dumpmsg, D_IM_CNR_CH_0, IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.SPRTRG.bit.SPRTRG ));
	Ddim_Print(( dumpmsg, D_IM_CNR_CH_1, IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.SPRTRG.bit.SPRTRG ));
	Im_CNR_Off_Pclk(ch);

#ifdef CO_DEBUG_ON_PC
	IO_CNR(ch)->OFL_REG_RW.SPRTRG.bit.SPRTRG = 2;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_CNR_OFL_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	Im_CNR_On_Pclk(ch);
	Ddim_Print(( dumpmsg, D_IM_CNR_CH_0, IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.SPRTRG.bit.SPRTRG ));
	Ddim_Print(( dumpmsg, D_IM_CNR_CH_1, IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.SPRTRG.bit.SPRTRG ));
	Im_CNR_Off_Pclk(ch);

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_23: "
INT32 CT_IM_CNR_OFL_1_23( UCHAR ch )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	Im_CNR_OFL_Open(ch, 100);
	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_24: "
INT32 CT_IM_CNR_OFL_1_24( UCHAR ch )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Reset(ch);

	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Reset error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_25: "
INT32 CT_IM_CNR_OFL_1_25( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_CTRL_CC_SRC_DST spr_ctrl[3] = {
		[0] = {	//max
			.enable = (E_IM_CNR_ONOFF)1,
			.cb_table = {
				.bd1 = 0x7F,
				.bd2 = 0x7F,
				.bd3 = 0x7F,
				.bd4 = 0x7F,
				.bd5 = 0x7F,
				.of0 = 0x7F,
				.of1 = 0x7F,
				.of2 = 0x7F,
				.of3 = 0x7F,
				.of4 = 0x7F,
				.of5 = 0x7F,
				.gain0 = 0x07FF,
				.gain1 = 0x07FF,
				.gain2 = 0x07FF,
				.gain3 = 0x07FF,
				.gain4 = 0x07FF,
				.gain5 = 0x07FF
			},
			.cr_table = {
				.bd1 = 0x7F,
				.bd2 = 0x7F,
				.bd3 = 0x7F,
				.bd4 = 0x7F,
				.bd5 = 0x7F,
				.of0 = 0x7F,
				.of1 = 0x7F,
				.of2 = 0x7F,
				.of3 = 0x7F,
				.of4 = 0x7F,
				.of5 = 0x7F,
				.gain0 = 0x07FF,
				.gain1 = 0x07FF,
				.gain2 = 0x07FF,
				.gain3 = 0x07FF,
				.gain4 = 0x07FF,
				.gain5 = 0x07FF
			}
		},
		[1] = {	//min
			.enable = (E_IM_CNR_ONOFF)0,
			.cb_table = {
				.bd1 = -0x80,
				.bd2 = -0x80,
				.bd3 = -0x80,
				.bd4 = -0x80,
				.bd5 = -0x80,
				.of0 = -0x80,
				.of1 = -0x80,
				.of2 = -0x80,
				.of3 = -0x80,
				.of4 = -0x80,
				.of5 = -0x80,
				.gain0 = -0x0800,
				.gain1 = -0x0800,
				.gain2 = -0x0800,
				.gain3 = -0x0800,
				.gain4 = -0x0800,
				.gain5 = -0x0800
			},
			.cr_table = {
				.bd1 = -0x80,
				.bd2 = -0x80,
				.bd3 = -0x80,
				.bd4 = -0x80,
				.bd5 = -0x80,
				.of0 = -0x80,
				.of1 = -0x80,
				.of2 = -0x80,
				.of3 = -0x80,
				.of4 = -0x80,
				.of5 = -0x80,
				.gain0 = -0x0800,
				.gain1 = -0x0800,
				.gain2 = -0x0800,
				.gain3 = -0x0800,
				.gain4 = -0x0800,
				.gain5 = -0x0800
			}
		},
		[2] = {	//countup
			.enable = (E_IM_CNR_ONOFF)1,
			.cb_table = {
				.bd1 = 0x01,
				.bd2 = 0x02,
				.bd3 = 0x03,
				.bd4 = 0x04,
				.bd5 = 0x05,
				.of0 = 0x06,
				.of1 = 0x07,
				.of2 = 0x08,
				.of3 = 0x09,
				.of4 = 0x0A,
				.of5 = 0x0B,
				.gain0 = 0x000C,
				.gain1 = 0x000D,
				.gain2 = 0x000E,
				.gain3 = 0x000F,
				.gain4 = 0x0010,
				.gain5 = 0x0011
			},
			.cr_table = {
				.bd1 = 0x01,
				.bd2 = 0x02,
				.bd3 = 0x03,
				.bd4 = 0x04,
				.bd5 = 0x05,
				.of0 = 0x06,
				.of1 = 0x07,
				.of2 = 0x08,
				.of3 = 0x09,
				.of4 = 0x0A,
				.of5 = 0x0B,
				.gain0 = 0x000C,
				.gain1 = 0x000D,
				.gain2 = 0x000E,
				.gain3 = 0x000F,
				.gain4 = 0x0010,
				.gain5 = 0x0011
			}
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_ctrl) / sizeof(spr_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Ctrl_SPRS_CC( ch, &spr_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_ctrl_src_cc(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_26: "
INT32 CT_IM_CNR_OFL_1_26( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_CTRL_CC_SRC_DST spr_ctrl[3] = {
		[0] = {	//max
			.enable = (E_IM_CNR_ONOFF)1,
			.cb_table = {
				.bd1 = 0x7F,
				.bd2 = 0x7F,
				.bd3 = 0x7F,
				.bd4 = 0x7F,
				.bd5 = 0x7F,
				.of0 = 0x7F,
				.of1 = 0x7F,
				.of2 = 0x7F,
				.of3 = 0x7F,
				.of4 = 0x7F,
				.of5 = 0x7F,
				.gain0 = 0x07FF,
				.gain1 = 0x07FF,
				.gain2 = 0x07FF,
				.gain3 = 0x07FF,
				.gain4 = 0x07FF,
				.gain5 = 0x07FF
			},
			.cr_table = {
				.bd1 = 0x7F,
				.bd2 = 0x7F,
				.bd3 = 0x7F,
				.bd4 = 0x7F,
				.bd5 = 0x7F,
				.of0 = 0x7F,
				.of1 = 0x7F,
				.of2 = 0x7F,
				.of3 = 0x7F,
				.of4 = 0x7F,
				.of5 = 0x7F,
				.gain0 = 0x07FF,
				.gain1 = 0x07FF,
				.gain2 = 0x07FF,
				.gain3 = 0x07FF,
				.gain4 = 0x07FF,
				.gain5 = 0x07FF
			}
		},
		[1] = {	//min
			.enable = (E_IM_CNR_ONOFF)0,
			.cb_table = {
				.bd1 = -0x80,
				.bd2 = -0x80,
				.bd3 = -0x80,
				.bd4 = -0x80,
				.bd5 = -0x80,
				.of0 = -0x80,
				.of1 = -0x80,
				.of2 = -0x80,
				.of3 = -0x80,
				.of4 = -0x80,
				.of5 = -0x80,
				.gain0 = -0x0800,
				.gain1 = -0x0800,
				.gain2 = -0x0800,
				.gain3 = -0x0800,
				.gain4 = -0x0800,
				.gain5 = -0x0800
			},
			.cr_table = {
				.bd1 = -0x80,
				.bd2 = -0x80,
				.bd3 = -0x80,
				.bd4 = -0x80,
				.bd5 = -0x80,
				.of0 = -0x80,
				.of1 = -0x80,
				.of2 = -0x80,
				.of3 = -0x80,
				.of4 = -0x80,
				.of5 = -0x80,
				.gain0 = -0x0800,
				.gain1 = -0x0800,
				.gain2 = -0x0800,
				.gain3 = -0x0800,
				.gain4 = -0x0800,
				.gain5 = -0x0800
			}
		},
		[2] = {	//countup
			.enable = (E_IM_CNR_ONOFF)1,
			.cb_table = {
				.bd1 = 0x01,
				.bd2 = 0x02,
				.bd3 = 0x03,
				.bd4 = 0x04,
				.bd5 = 0x05,
				.of0 = 0x06,
				.of1 = 0x07,
				.of2 = 0x08,
				.of3 = 0x09,
				.of4 = 0x0A,
				.of5 = 0x0B,
				.gain0 = 0x000C,
				.gain1 = 0x000D,
				.gain2 = 0x000E,
				.gain3 = 0x000F,
				.gain4 = 0x0010,
				.gain5 = 0x0011
			},
			.cr_table = {
				.bd1 = 0x01,
				.bd2 = 0x02,
				.bd3 = 0x03,
				.bd4 = 0x04,
				.bd5 = 0x05,
				.of0 = 0x06,
				.of1 = 0x07,
				.of2 = 0x08,
				.of3 = 0x09,
				.of4 = 0x0A,
				.of5 = 0x0B,
				.gain0 = 0x000C,
				.gain1 = 0x000D,
				.gain2 = 0x000E,
				.gain3 = 0x000F,
				.gain4 = 0x0010,
				.gain5 = 0x0011
			}
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_ctrl) / sizeof(spr_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Ctrl_SPRD_CC( ch, &spr_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_ctrl_dst_cc(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_27: "
INT32 CT_IM_CNR_OFL_1_27( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_HUE_SPEC_TABLE spr_hue_ctrl[3] = {
		[0] = {	//max
			.cd_cb = 0x7F,
			.cd_cr = 0x7F,
			.k_cb = 0x3FF,
			.k_cr = 0x3FF,
			.bd1 = 0x7FF,
			.bd2 = 0x7FF,
			.bd3 = 0x7FF,
			.bd4 = 0x7FF,
			.bd5 = 0x7FF,
			.of0 = 0x100,
			.of1 = 0x100,
			.of2 = 0x100,
			.of3 = 0x100,
			.of4 = 0x100,
			.of5 = 0x100,
			.gain0 = 0x03FF,
			.gain1 = 0x03FF,
			.gain2 = 0x03FF,
			.gain3 = 0x03FF,
			.gain4 = 0x03FF,
			.gain5 = 0x03FF
		},
		[1] = {	//min
			.cd_cb = -0x80,
			.cd_cr = -0x80,
			.k_cb = 0,
			.k_cr = 0,
			.bd1 = 0x00,
			.bd2 = 0x00,
			.bd3 = 0x00,
			.bd4 = 0x00,
			.bd5 = 0x00,
			.of0 = 0x00,
			.of1 = 0x00,
			.of2 = 0x00,
			.of3 = 0x00,
			.of4 = 0x00,
			.of5 = 0x00,
			.gain0 = -0x0400,
			.gain1 = -0x0400,
			.gain2 = -0x0400,
			.gain3 = -0x0400,
			.gain4 = -0x0400,
			.gain5 = -0x0400,
		},
		[2] = {	//countup
			.cd_cb = 1,
			.cd_cr = 2,
			.k_cb = 3,
			.k_cr = 4,
			.bd1 = 0x05,
			.bd2 = 0x06,
			.bd3 = 0x07,
			.bd4 = 0x08,
			.bd5 = 0x09,
			.of0 = 0x0A,
			.of1 = 0x0B,
			.of2 = 0x0C,
			.of3 = 0x0D,
			.of4 = 0x0E,
			.of5 = 0x0F,
			.gain0 = 0x0010,
			.gain1 = 0x0011,
			.gain2 = 0x0012,
			.gain3 = 0x0013,
			.gain4 = 0x0014,
			.gain5 = 0x0015
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_hue_ctrl) / sizeof(spr_hue_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OFL_Set_CSPR_Hue_Specified_Table( ch, &spr_hue_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OFL_print_debug_ctrl_hue_cc(ch);
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}


// test RDMA
#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_30: "
INT32 CT_IM_CNR_OFL_1_30( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CTRL_C* cnr_ctrl_c;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_Ctrl_C( ch, &cnr_ctrl_c, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_Ctrl_C() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cnr_ctrl_c;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_31: "
INT32 CT_IM_CNR_OFL_1_31( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.gradation_keep_en = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_cor = 0xFF,
			.y_threshold_gain = 0x01FF,
			.y_threshold_dym_en = E_IM_CNR_ONOFF_ENABLE,
			.y_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
			.c_y_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_cor = 0xFF,
			.y_threshold_gain = 0x01FF,
			.y_threshold_dym_en = E_IM_CNR_ONOFF_ENABLE,
			.y_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
			.c_y_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
		},
		.whitening_enable = E_IM_CNR_ONOFF_ENABLE,
		.whitening_value = 0xF,
		.whitening_base = E_IM_CNR_WHITEN_BASE_MIDDLE,
		.c_blend_md = E_IM_CNR_C_BLEND_MD_COL_DIFF_EDGE,
		.c_alpha_blend = 0xF,
		.c_range = E_IM_CNR_ONOFF_ENABLE,
		.c_range_mode = E_IM_CNR_C_RANGE_MODE_0_32,
	};
	T_IM_CNR_RDMA_DATA_CTRL_C rdma_ctrl_c;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_Ctrl_C( &spr_ctrl_c, &rdma_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_Ctrl_C() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_ctrl_c.CSPREN   = 0x%lx\n", rdma_ctrl_c.CSPREN.word ));
	Ddim_Print(( "rdma_ctrl_c.CGDKEN   = 0x%lx\n", rdma_ctrl_c.CGDKEN.word ));
	Ddim_Print(( "rdma_ctrl_c.MCEN     = 0x%lx\n", rdma_ctrl_c.MCEN.word ));
	Ddim_Print(( "rdma_ctrl_c.MCYTHH   = 0x%lx\n", rdma_ctrl_c.MCYTHH.word ));
	Ddim_Print(( "rdma_ctrl_c.MCYTHV   = 0x%lx\n", rdma_ctrl_c.MCYTHV.word ));
	Ddim_Print(( "rdma_ctrl_c.MCCTHH   = 0x%lx\n", rdma_ctrl_c.MCCTHH.word ));
	Ddim_Print(( "rdma_ctrl_c.MCCTHV   = 0x%lx\n", rdma_ctrl_c.MCCTHV.word ));
	Ddim_Print(( "rdma_ctrl_c.MCYDYM   = 0x%lx\n", rdma_ctrl_c.MCYDYM.word ));
	Ddim_Print(( "rdma_ctrl_c.MCYDYMDT = 0x%lx\n", rdma_ctrl_c.MCYDYMDT.word ));
	Ddim_Print(( "rdma_ctrl_c.MCYSCL   = 0x%lx\n", rdma_ctrl_c.MCYSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.MCCSCL   = 0x%lx\n", rdma_ctrl_c.MCCSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.MCSSCL   = 0x%lx\n", rdma_ctrl_c.MCSSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.LCEN     = 0x%lx\n", rdma_ctrl_c.LCEN.word ));
	Ddim_Print(( "rdma_ctrl_c.LCYTHH   = 0x%lx\n", rdma_ctrl_c.LCYTHH.word ));
	Ddim_Print(( "rdma_ctrl_c.LCYTHV   = 0x%lx\n", rdma_ctrl_c.LCYTHV.word ));
	Ddim_Print(( "rdma_ctrl_c.LCCTHH   = 0x%lx\n", rdma_ctrl_c.LCCTHH.word ));
	Ddim_Print(( "rdma_ctrl_c.LCCTHV   = 0x%lx\n", rdma_ctrl_c.LCCTHV.word ));
	Ddim_Print(( "rdma_ctrl_c.LCYDYM   = 0x%lx\n", rdma_ctrl_c.LCYDYM.word ));
	Ddim_Print(( "rdma_ctrl_c.LCYDYMDT = 0x%lx\n", rdma_ctrl_c.LCYDYMDT.word ));
	Ddim_Print(( "rdma_ctrl_c.LCYSC    = 0x%lx\n", rdma_ctrl_c.LCYSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.LCCSC    = 0x%lx\n", rdma_ctrl_c.LCCSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.LCSSC    = 0x%lx\n", rdma_ctrl_c.LCSSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.BLEND    = 0x%lx\n", rdma_ctrl_c.BLEND.word ));
	Ddim_Print(( "rdma_ctrl_c.WHTMD    = 0x%lx\n", rdma_ctrl_c.WHTMD.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_32: "
INT32 CT_IM_CNR_OFL_1_32( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_OFL_CTRL_Y* cnr_ctrl_y;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_Ctrl_Y( ch, &cnr_ctrl_y, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_Ctrl_Y() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cnr_ctrl_y;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_33: "
INT32 CT_IM_CNR_OFL_1_33( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OFL_CTRL_Y spr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.threshold_type = E_IM_CNR_THRES_REGISTER,
		.level = E_IM_CNR_LEVEL_HIGH,
		.h_y_threshold = 0xFF,
		.v_y_threshold = 0xFF,
		.h_c_threshold = 0xFF,
		.v_c_threshold = 0xFF,
		.alpha_blend_ratio = 0x1F,
	};
	T_IM_CNR_RDMA_DATA_OFL_CTRL_Y rdma_ctrl_y;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_Ctrl_Y( &spr_ctrl_y, &rdma_ctrl_y );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_Ctrl_Y() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_ctrl_y.YSPREN    = 0x%lx\n", rdma_ctrl_y.YSPREN.word ));
	Ddim_Print(( "rdma_ctrl_y.YSPRMODE  = 0x%lx\n", rdma_ctrl_y.YSPRMODE.word ));
	Ddim_Print(( "rdma_ctrl_y.YSPRFXEY  = 0x%lx\n", rdma_ctrl_y.YSPRFXEY.word ));
	Ddim_Print(( "rdma_ctrl_y.YSPRFXEC  = 0x%lx\n", rdma_ctrl_y.YSPRFXEC.word ));
	Ddim_Print(( "rdma_ctrl_y.YSPRALPBD = 0x%lx\n", rdma_ctrl_y.YSPRALPBD.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_34: "
INT32 CT_IM_CNR_OFL_1_34( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_OFL_YSPR_TBL* cnr_yspr_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_YSPR_Table( ch, &cnr_yspr_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_YSPR_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cnr_yspr_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_35: "
INT32 CT_IM_CNR_OFL_1_35( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE yspr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_OFL_YSPR_TBL rdma_yspr_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_YSPR_Table( &yspr_table, &rdma_yspr_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_YSPR_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_yspr_table.YSPRDYEYOF.YSPRDYEYOF1 = 0x%lx\n", rdma_yspr_table.YSPRDYEYOF.YSPRDYEYOF1.word ));
	Ddim_Print(( "rdma_yspr_table.YSPRDYEYOF.YSPRDYEYOF2 = 0x%lx\n", rdma_yspr_table.YSPRDYEYOF.YSPRDYEYOF2.word ));
	Ddim_Print(( "rdma_yspr_table.YSPRDYEYGA.YSPRDYEYGA1 = 0x%lx\n", rdma_yspr_table.YSPRDYEYGA.YSPRDYEYGA1.word ));
	Ddim_Print(( "rdma_yspr_table.YSPRDYEYGA.YSPRDYEYGA2 = 0x%lx\n", rdma_yspr_table.YSPRDYEYGA.YSPRDYEYGA2.word ));
	Ddim_Print(( "rdma_yspr_table.YSPRDYEYBD.YSPRDYEYBD1 = 0x%lx\n", rdma_yspr_table.YSPRDYEYBD.YSPRDYEYBD1.word ));
	Ddim_Print(( "rdma_yspr_table.YSPRDYEYBD.YSPRDYEYBD2 = 0x%lx\n", rdma_yspr_table.YSPRDYEYBD.YSPRDYEYBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_36: "
INT32 CT_IM_CNR_OFL_1_36( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL* cspr_mid_y_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table( ch, &cspr_mid_y_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_mid_y_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_37: "
INT32 CT_IM_CNR_OFL_1_37( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE cspr_mid_y_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL rdma_cspr_mid_y_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table( &cspr_mid_y_table, &rdma_cspr_mid_y_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLOF.MCYSCLOF1 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLOF.MCYSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLOF.MCYSCLOF2 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLOF.MCYSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLGA.MCYSCLGA1 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLGA.MCYSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLGA.MCYSCLGA2 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLGA.MCYSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLBD.MCYSCLBD1 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLBD.MCYSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLBD.MCYSCLBD2 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLBD.MCYSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_38: "
INT32 CT_IM_CNR_OFL_1_38( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL* cspr_mid_cc_y_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_Y_Table( ch, &cspr_mid_cc_y_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_mid_cc_y_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_39: "
INT32 CT_IM_CNR_OFL_1_39( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE spr_cspr_mid_cc_y = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL rdma_cspr_mid_cc_y_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_Y_Table( &spr_cspr_mid_cc_y, &rdma_cspr_mid_cc_y_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLOF.MCCSCLOF1 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLOF.MCCSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLOF.MCCSCLOF2 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLOF.MCCSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLGA.MCCSCLGA1 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLGA.MCCSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLGA.MCCSCLGA2 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLGA.MCCSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLBD.MCCSCLBD1 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLBD.MCCSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLBD.MCCSCLBD2 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLBD.MCCSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_40: "
INT32 CT_IM_CNR_OFL_1_40( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL* cspr_mid_cc_c_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_C_Table( ch, &cspr_mid_cc_c_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_C_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_mid_cc_c_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_41: "
INT32 CT_IM_CNR_OFL_1_41( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE spr_cspr_mid_cc_c = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL rdma_cspr_mid_cc_c_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_C_Table( &spr_cspr_mid_cc_c, &rdma_cspr_mid_cc_c_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_C_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLOF.MCCSCLOF1 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLOF.MCSSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLOF.MCCSCLOF2 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLOF.MCSSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLGA.MCCSCLGA1 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLGA.MCSSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLGA.MCCSCLGA2 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLGA.MCSSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLBD.MCCSCLBD1 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLBD.MCSSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLBD.MCCSCLBD2 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLBD.MCSSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_42: "
INT32 CT_IM_CNR_OFL_1_42( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL* cspr_low_y_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_Y_Table( ch, &cspr_low_y_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_low_y_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_43: "
INT32 CT_IM_CNR_OFL_1_43( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE cspr_low_y_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL rdma_cspr_low_y_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_CSPR_Low_Y_Table( &cspr_low_y_table, &rdma_cspr_low_y_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_CSPR_Low_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLOF.LCYSCLOF1 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLOF.LCYSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLOF.LCYSCLOF2 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLOF.LCYSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLGA.LCYSCLGA1 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLGA.LCYSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLGA.LCYSCLGA2 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLGA.LCYSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLBD.LCYSCLBD1 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLBD.LCYSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLBD.LCYSCLBD2 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLBD.LCYSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_44: "
INT32 CT_IM_CNR_OFL_1_44( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL* cspr_low_cc_y_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_Y_Table( ch, &cspr_low_cc_y_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_low_cc_y_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_45: "
INT32 CT_IM_CNR_OFL_1_45( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE spr_cspr_low_cc_y = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL rdma_cspr_low_cc_y_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_Y_Table( &spr_cspr_low_cc_y, &rdma_cspr_low_cc_y_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLOF.LCCSCLOF1 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLOF.LCCSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLOF.LCCSCLOF2 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLOF.LCCSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLGA.LCCSCLGA1 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLGA.LCCSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLGA.LCCSCLGA2 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLGA.LCCSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLBD.LCCSCLBD1 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLBD.LCCSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLBD.LCCSCLBD2 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLBD.LCCSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_46: "
INT32 CT_IM_CNR_OFL_1_46( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL* cspr_low_cc_c_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_C_Table( ch, &cspr_low_cc_c_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_C_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_low_cc_c_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_47: "
INT32 CT_IM_CNR_OFL_1_47( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE spr_cspr_low_cc_c = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL rdma_cspr_low_cc_c_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_C_Table( &spr_cspr_low_cc_c, &rdma_cspr_low_cc_c_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_C_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLOF.LCCSCLOF1 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLOF.LCSSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLOF.LCCSCLOF2 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLOF.LCSSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLGA.LCCSCLGA1 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLGA.LCSSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLGA.LCCSCLGA2 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLGA.LCSSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLBD.LCCSCLBD1 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLBD.LCSSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLBD.LCCSCLBD2 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLBD.LCSSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_48: "
INT32 CT_IM_CNR_OFL_1_48( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL* cspr_edge_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_CSPR_Edge_Table( ch, &cspr_edge_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_CSPR_Edge_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_edge_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_49: "
INT32 CT_IM_CNR_OFL_1_49( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE spr_cspr_edge = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0x3F,
		.of1 = 0x3F,
		.of2 = 0x3F,
		.of3 = 0x3F,
		.of4 = 0x3F,
		.of5 = 0x3F,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL rdma_cspr_edge_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_CSPR_Edge_Table( &spr_cspr_edge, &rdma_cspr_edge_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_CSPR_Edge_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_edge_table.CESCLOF.CESCLOF1 = 0x%lx\n", rdma_cspr_edge_table.CESCLOF.CESCLOF1.word ));
	Ddim_Print(( "rdma_cspr_edge_table.CESCLOF.CESCLOF2 = 0x%lx\n", rdma_cspr_edge_table.CESCLOF.CESCLOF2.word ));
	Ddim_Print(( "rdma_cspr_edge_table.CESCLGA.CESCLGA1 = 0x%lx\n", rdma_cspr_edge_table.CESCLGA.CESCLGA1.word ));
	Ddim_Print(( "rdma_cspr_edge_table.CESCLGA.CESCLGA2 = 0x%lx\n", rdma_cspr_edge_table.CESCLGA.CESCLGA2.word ));
	Ddim_Print(( "rdma_cspr_edge_table.CESCLBD.CESCLBD1 = 0x%lx\n", rdma_cspr_edge_table.CESCLBD.CESCLBD1.word ));
	Ddim_Print(( "rdma_cspr_edge_table.CESCLBD.CESCLBD2 = 0x%lx\n", rdma_cspr_edge_table.CESCLBD.CESCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_50: "
INT32 CT_IM_CNR_OFL_1_50( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL* cspr_hue_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_CSPR_Hue_Spec_Table( ch, &cspr_hue_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_CSPR_Hue_Spec_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_hue_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_51: "
INT32 CT_IM_CNR_OFL_1_51( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_HUE_SPEC_TABLE spr_cspr_hue = {
		.cd_cb = 0xFF,
		.cd_cr = 0xFF,
		.k_cb = 0x3FF,
		.k_cr = 0x3FF,
		.bd1 = 0x7FF,
		.bd2 = 0x7FF,
		.bd3 = 0x7FF,
		.bd4 = 0x7FF,
		.bd5 = 0x7FF,
		.of0 = 0x100,
		.of1 = 0x100,
		.of2 = 0x100,
		.of3 = 0x100,
		.of4 = 0x100,
		.of5 = 0x100,
		.gain0 = 0x03FF,
		.gain1 = 0x03FF,
		.gain2 = 0x03FF,
		.gain3 = 0x03FF,
		.gain4 = 0x03FF,
		.gain5 = 0x03FF
	};
	T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL rdma_cspr_hue_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_CSPR_Hue_Spec_Table( &spr_cspr_hue, &rdma_cspr_hue_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_CSPR_Hue_Spec_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_hue_table.CPHPT            = 0x%lx\n", rdma_cspr_hue_table.CPHPT.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CPHPTK           = 0x%lx\n", rdma_cspr_hue_table.CPHPTK.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLOF.CDSCLOF1 = 0x%lx\n", rdma_cspr_hue_table.CDSCLOF.CDSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLOF.CDSCLOF2 = 0x%lx\n", rdma_cspr_hue_table.CDSCLOF.CDSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLOF.CDSCLOF3 = 0x%lx\n", rdma_cspr_hue_table.CDSCLOF.CDSCLOF3.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLGA.CDSCLGA1 = 0x%lx\n", rdma_cspr_hue_table.CDSCLGA.CDSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLGA.CDSCLGA2 = 0x%lx\n", rdma_cspr_hue_table.CDSCLGA.CDSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLGA.CDSCLGA3 = 0x%lx\n", rdma_cspr_hue_table.CDSCLGA.CDSCLGA3.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLBD.CDSCLBD1 = 0x%lx\n", rdma_cspr_hue_table.CDSCLBD.CDSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLBD.CDSCLBD2 = 0x%lx\n", rdma_cspr_hue_table.CDSCLBD.CDSCLBD2.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLBD.CDSCLBD3 = 0x%lx\n", rdma_cspr_hue_table.CDSCLBD.CDSCLBD3.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_52: "
INT32 CT_IM_CNR_OFL_1_52( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL* cspr_sprs_cc_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRS_CC_Table( ch, &cspr_sprs_cc_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRS_CC_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_sprs_cc_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_53: "
INT32 CT_IM_CNR_OFL_1_53( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_CTRL_CC_SRC_DST cspr_sprs_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0x7F,
			.bd2 = 0x7F,
			.bd3 = 0x7F,
			.bd4 = 0x7F,
			.bd5 = 0x7F,
			.of0 = 0x7F,
			.of1 = 0x7F,
			.of2 = 0x7F,
			.of3 = 0x7F,
			.of4 = 0x7F,
			.of5 = 0x7F,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0x7F,
			.bd2 = 0x7F,
			.bd3 = 0x7F,
			.bd4 = 0x7F,
			.bd5 = 0x7F,
			.of0 = 0x7F,
			.of1 = 0x7F,
			.of2 = 0x7F,
			.of3 = 0x7F,
			.of4 = 0x7F,
			.of5 = 0x7F,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRS_CC_TBL rdma_cspr_sprs_cc;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_CSPR_SPRS_CC_Table( &cspr_sprs_cc, &rdma_cspr_sprs_cc );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_CSPR_SPRS_CC_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRS               = 0x%lx\n", rdma_cspr_sprs_cc.SPRS.word ));

	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBOF.SPRSCBOF1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBOF.SPRSCBOF1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBOF.SPRSCBOF2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBOF.SPRSCBOF2.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA2.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA3 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA3.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBBD.SPRSCBBD1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBBD.SPRSCBBD1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBBD.SPRSCBBD2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBBD.SPRSCBBD2.word ));

	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCROF.SPRSCROF1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCROF.SPRSCROF1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCROF.SPRSCROF2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCROF.SPRSCROF2.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA2.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA3 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA3.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCRBD.SPRSCRBD1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCRBD.SPRSCRBD1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCRBD.SPRSCRBD2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCRBD.SPRSCRBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_54: "
INT32 CT_IM_CNR_OFL_1_54( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL* cspr_sprd_cc_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRD_CC_Table( ch, &cspr_sprd_cc_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRD_CC_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_sprd_cc_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_1_55: "
INT32 CT_IM_CNR_OFL_1_55( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_CTRL_CC_SRC_DST cspr_sprd_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0x7F,
			.bd2 = 0x7F,
			.bd3 = 0x7F,
			.bd4 = 0x7F,
			.bd5 = 0x7F,
			.of0 = 0x7F,
			.of1 = 0x7F,
			.of2 = 0x7F,
			.of3 = 0x7F,
			.of4 = 0x7F,
			.of5 = 0x7F,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0x7F,
			.bd2 = 0x7F,
			.bd3 = 0x7F,
			.bd4 = 0x7F,
			.bd5 = 0x7F,
			.of0 = 0x7F,
			.of1 = 0x7F,
			.of2 = 0x7F,
			.of3 = 0x7F,
			.of4 = 0x7F,
			.of5 = 0x7F,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRD_CC_TBL rdma_cspr_sprd_cc;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OFL_CSPR_SPRD_CC_Table( &cspr_sprd_cc, &rdma_cspr_sprd_cc );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OFL_CSPR_SPRD_CC_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRD               = 0x%lx\n", rdma_cspr_sprd_cc.SPRD.word ));

	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBOF.SPRDCBOF1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBOF.SPRDCBOF1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBOF.SPRDCBOF2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBOF.SPRDCBOF2.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA2.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA3 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA3.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBBD.SPRDCBBD1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBBD.SPRDCBBD1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBBD.SPRDCBBD2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBBD.SPRDCBBD2.word ));

	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCROF.SPRDCROF1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCROF.SPRDCROF1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCROF.SPRDCROF2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCROF.SPRDCROF2.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA2.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA3 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA3.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCRBD.SPRDCRBD1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCRBD.SPRDCRBD1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCRBD.SPRDCRBD2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCRBD.SPRDCRBD2.word ));

	return im_ercd;
}

static INT32 ct_Im_CNR_run_1( const UINT32 ct_no_2nd, const UINT32 ct_no_3rd )
{
	Ddim_Print(( "ct_Im_CNR_run_1 ct_no_2nd = 0x%x ct_no_3rd = 0x%x\n", ct_no_2nd, ct_no_3rd ));
	switch( ct_no_2nd ) {
		// Test-1-1
		case 1:
			return CT_IM_CNR_OFL_1_1(ct_no_3rd);

		// Test-1-2
		case 2:
			return CT_IM_CNR_OFL_1_2(ct_no_3rd);

		// Test-1-3
		case 3:
			return CT_IM_CNR_OFL_1_3(ct_no_3rd);

		// Test-1-4
		case 4:
			return CT_IM_CNR_OFL_1_4(ct_no_3rd);

		// Test-1-5
		case 5:
			return CT_IM_CNR_OFL_1_5(ct_no_3rd);

		// Test-1-6
		case 6:
			return CT_IM_CNR_OFL_1_6(ct_no_3rd);

		// Test-1-7
		case 7:
			return CT_IM_CNR_OFL_1_7(ct_no_3rd);

		// Test-1-8
		case 8:
			return CT_IM_CNR_OFL_1_8(ct_no_3rd);

		// Test-1-9
		case 9:
			return CT_IM_CNR_OFL_1_9(ct_no_3rd);

		// Test-1-10
		case 10:
			return CT_IM_CNR_OFL_1_10(ct_no_3rd);

		// Test-1-11
		case 11:
			return CT_IM_CNR_OFL_1_11(ct_no_3rd);

		// Test-1-12
		case 12:
			return CT_IM_CNR_OFL_1_12(ct_no_3rd);

		// Test-1-13
		case 13:
			return CT_IM_CNR_OFL_1_13(ct_no_3rd);

		// Test-1-14
		case 14:
			return CT_IM_CNR_OFL_1_14(ct_no_3rd);

		// Test-1-15
		case 15:
			return CT_IM_CNR_OFL_1_15(ct_no_3rd);

		// Test-1-16
		case 16:
			return CT_IM_CNR_OFL_1_16(ct_no_3rd);

		// Test-1-17
		case 17:
			return CT_IM_CNR_OFL_1_17(ct_no_3rd);

		// Test-1-18
		case 18:
			return CT_IM_CNR_OFL_1_18(ct_no_3rd);

		// Test-1-19
		case 19:
			return CT_IM_CNR_OFL_1_19(ct_no_3rd);

		// Test-1-20
		case 20:
			return CT_IM_CNR_OFL_1_20(ct_no_3rd);

		// Test-1-21
		case 21:
			return CT_IM_CNR_OFL_1_21(ct_no_3rd);

		// Test-1-22
		case 22:
			return CT_IM_CNR_OFL_1_22(ct_no_3rd);

		// Test-1-23
		case 23:
			return CT_IM_CNR_OFL_1_23(ct_no_3rd);

		// Test-1-24
		case 24:
			return CT_IM_CNR_OFL_1_24(ct_no_3rd);

		// Test-1-25
		case 25:
			return CT_IM_CNR_OFL_1_25(ct_no_3rd);

		// Test-1-26
		case 26:
			return CT_IM_CNR_OFL_1_26(ct_no_3rd);

		// Test-1-27
		case 27:
			return CT_IM_CNR_OFL_1_27(ct_no_3rd);


// test RDMA
		// Test-1-30
		case 30:
			return CT_IM_CNR_OFL_1_30(ct_no_3rd);
		// Test-1-31
		case 31:
			return CT_IM_CNR_OFL_1_31(ct_no_3rd);
		// Test-1-32
		case 32:
			return CT_IM_CNR_OFL_1_32(ct_no_3rd);
		// Test-1-33
		case 33:
			return CT_IM_CNR_OFL_1_33(ct_no_3rd);
		// Test-1-34
		case 34:
			return CT_IM_CNR_OFL_1_34(ct_no_3rd);
		// Test-1-35
		case 35:
			return CT_IM_CNR_OFL_1_35(ct_no_3rd);
		// Test-1-36
		case 36:
			return CT_IM_CNR_OFL_1_36(ct_no_3rd);
		// Test-1-37
		case 37:
			return CT_IM_CNR_OFL_1_37(ct_no_3rd);
		// Test-1-38
		case 38:
			return CT_IM_CNR_OFL_1_38(ct_no_3rd);
		// Test-1-39
		case 39:
			return CT_IM_CNR_OFL_1_39(ct_no_3rd);
		// Test-1-40
		case 40:
			return CT_IM_CNR_OFL_1_40(ct_no_3rd);
		// Test-1-41
		case 41:
			return CT_IM_CNR_OFL_1_41(ct_no_3rd);
		// Test-1-42
		case 42:
			return CT_IM_CNR_OFL_1_42(ct_no_3rd);
		// Test-1-43
		case 43:
			return CT_IM_CNR_OFL_1_43(ct_no_3rd);
		// Test-1-44
		case 44:
			return CT_IM_CNR_OFL_1_44(ct_no_3rd);
		// Test-1-45
		case 45:
			return CT_IM_CNR_OFL_1_45(ct_no_3rd);
		// Test-1-46
		case 46:
			return CT_IM_CNR_OFL_1_46(ct_no_3rd);
		// Test-1-47
		case 47:
			return CT_IM_CNR_OFL_1_47(ct_no_3rd);
		// Test-1-48
		case 48:
			return CT_IM_CNR_OFL_1_48(ct_no_3rd);
		// Test-1-49
		case 49:
			return CT_IM_CNR_OFL_1_49(ct_no_3rd);
		// Test-1-50
		case 50:
			return CT_IM_CNR_OFL_1_50(ct_no_3rd);
		// Test-1-51
		case 51:
			return CT_IM_CNR_OFL_1_51(ct_no_3rd);
		// Test-1-52
		case 52:
			return CT_IM_CNR_OFL_1_52(ct_no_3rd);
		// Test-1-53
		case 53:
			return CT_IM_CNR_OFL_1_53(ct_no_3rd);
		// Test-1-54
		case 54:
			return CT_IM_CNR_OFL_1_54(ct_no_3rd);
		// Test-1-55
		case 55:
			return CT_IM_CNR_OFL_1_55(ct_no_3rd);

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_1: "
INT32 CT_IM_CNR_OFL_2_1( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Ctrl( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_2: "
INT32 CT_IM_CNR_OFL_2_2( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Ctrl_Axi( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_3: "
INT32 CT_IM_CNR_OFL_2_3( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Get_AXI_Status( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_4: "
INT32 CT_IM_CNR_OFL_2_4( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Ctrl_C( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_5: "
INT32 CT_IM_CNR_OFL_2_5( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Ctrl_Y( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_6: "
INT32 CT_IM_CNR_OFL_2_6( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Set_CSPR_Mid_Y_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_7: "
INT32 CT_IM_CNR_OFL_2_7( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Set_CSPR_Mid_CC_Y_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_8: "
INT32 CT_IM_CNR_OFL_2_8( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Set_CSPR_Mid_CC_C_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_9: "
INT32 CT_IM_CNR_OFL_2_9( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Set_CSPR_Low_Y_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_10: "
INT32 CT_IM_CNR_OFL_2_10( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Set_CSPR_Low_CC_Y_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_11: "
INT32 CT_IM_CNR_OFL_2_11( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Set_CSPR_Low_CC_C_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_12: "
INT32 CT_IM_CNR_OFL_2_12( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Set_YSPR_Table( 0, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_13: "
INT32 CT_IM_CNR_OFL_2_13( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Wait_End( 0, NULL, 0 );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_14: "
INT32 CT_IM_CNR_OFL_2_14( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Ctrl_SPRS_CC( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OFL_2_15: "
INT32 CT_IM_CNR_OFL_2_15( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Ctrl_SPRD_CC( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

static INT32 ct_Im_CNR_run_2( const UINT32 ct_no_2nd, const UINT32 ct_no_3rd )
{
	Ddim_Print(( "ct_Im_CNR_run_2 ct_no_2nd = 0x%x ct_no_3rd = 0x%x\n", ct_no_2nd, ct_no_3rd ));
	switch( ct_no_2nd ) {
		// Test-2-1
		case 1:
			return CT_IM_CNR_OFL_2_1(ct_no_3rd);

		// Test-2-2
		case 2:
			return CT_IM_CNR_OFL_2_2(ct_no_3rd);

		// Test-2-3
		case 3:
			return CT_IM_CNR_OFL_2_3(ct_no_3rd);

		// Test-2-4
		case 4:
			return CT_IM_CNR_OFL_2_4(ct_no_3rd);

		// Test-2-5
		case 5:
			return CT_IM_CNR_OFL_2_5(ct_no_3rd);

		// Test-2-6
		case 6:
			return CT_IM_CNR_OFL_2_6(ct_no_3rd);

		// Test-2-7
		case 7:
			return CT_IM_CNR_OFL_2_7(ct_no_3rd);

		// Test-2-8
		case 8:
			return CT_IM_CNR_OFL_2_8(ct_no_3rd);

		// Test-2-9
		case 9:
			return CT_IM_CNR_OFL_2_9(ct_no_3rd);

		// Test-2-10
		case 10:
			return CT_IM_CNR_OFL_2_10(ct_no_3rd);

		// Test-2-11
		case 11:
			return CT_IM_CNR_OFL_2_11(ct_no_3rd);

		// Test-2-12
		case 12:
			return CT_IM_CNR_OFL_2_12(ct_no_3rd);

		// Test-2-13
		case 13:
			return CT_IM_CNR_OFL_2_13(ct_no_3rd);

		// Test-2-14
		case 14:
			return CT_IM_CNR_OFL_2_14(ct_no_3rd);

		// Test-2-15
		case 15:
			return CT_IM_CNR_OFL_2_15(ct_no_3rd);

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_1: "
INT32 CT_IM_CNR_OTF_3_1( UCHAR ch )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_IM_CNR_SEM_TIMEOUT_ERR ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_2: "
INT32 CT_IM_CNR_OTF_3_2( UCHAR ch )
{
	INT32 im_ercd;
	DDIM_USER_ID sid_back;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	sid_back = SID_IM_CNR_OTF(ch);
	SID_IM_CNR_OTF(ch) = 0xfff;

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_IM_CNR_SEM_NG ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		Im_CNR_OTF_Close(ch);
		return im_ercd;
	}

	SID_IM_CNR_OTF(ch) = sid_back;

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_3: "
INT32 CT_IM_CNR_OTF_3_3( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OTF_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,

		.divide_left = E_IM_CNR_VPROCESS_NORM,
		.divide_right = E_IM_CNR_VPROCESS_NORM,

		.w_offset_x = 1000,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_OTF_OUTADDR_INFO addr_info = {
		.use_bank_num = 1,
		.w_c_addr[0] = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Set_OutAddr_Info( ch, &addr_info );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Set OutAddrInfo error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	ct_im_cnr_set_im_r2y_set( ch );
	ct_im_cnr_set_im_r2y_start( ch );
#endif

	im_ercd = Im_CNR_OTF_Start_Sync( ch, 10 );
	if( im_ercd != D_IM_CNR_SEM_TIMEOUT_ERR ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_CNR_OTF_Close(ch);
		return im_ercd;
	}

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	ct_im_cnr_set_im_r2y_stop( ch );
#endif

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_4: "
INT32 CT_IM_CNR_OTF_3_4( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 wait_factor;
#ifdef CO_DEBUG_ON_PC
	INT32 roop_count;
	ULONG out_addr;
#endif //CO_DEBUG_ON_PC
	T_IM_CNR_OTF_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,

		.divide_left = E_IM_CNR_VPROCESS_NORM,
		.divide_right = E_IM_CNR_VPROCESS_NORM,

		.w_offset_x = 1000,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_OTF_OUTADDR_INFO addr_info = {
		.use_bank_num = 4,
		.w_c_addr[0] = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
		.w_c_addr[1] = D_IM_CNR_TEST_IMG_DST_CB_ADDR + 0x1000,
		.w_c_addr[2] = D_IM_CNR_TEST_IMG_DST_CB_ADDR + 0x2000,
		.w_c_addr[3] = D_IM_CNR_TEST_IMG_DST_CB_ADDR + 0x3000,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	Im_CNR_OTF_Init(ch);


	im_ercd = Im_CNR_OTF_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Set_OutAddr_Info( ch, &addr_info );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Set OutAddrInfo error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Ddim_Print(( "[Before Start]ch0 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.ACESEN.bit.SPRACS ));
	Ddim_Print(( "[Before Start]ch1 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.ACESEN.bit.SPRACS ));

	im_ercd = Im_CNR_OTF_Start_Async(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start Async error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Ddim_Print(( "[After Start]ch0 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.ACESEN.bit.SPRACS ));
	Ddim_Print(( "[After Start]ch1 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.ACESEN.bit.SPRACS ));

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	ct_im_cnr_set_im_r2y_set( ch );
	ct_im_cnr_set_im_r2y_start( ch );
#endif

#ifdef CO_DEBUG_ON_PC
	if( ch == D_IM_CNR_CH_2 ){
		IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRICF.bit.__SPDF = 1;
		IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRICF.bit.__SPDF = 1;
		Im_CNR0_Int_Handler();
		Im_CNR1_Int_Handler();
	}
	else{
		IO_CNR(ch)->OTF_REG_RW.SPRICF.bit.__SPDF = 1;
		if( ch == D_IM_CNR_CH_0 ){
			Im_CNR0_Int_Handler();
		}
		else{
			Im_CNR1_Int_Handler();
		}
	}
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_CNR_OTF_Wait_End( ch, &wait_factor, 1000 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	ct_im_cnr_set_im_r2y_stop( ch );
#endif

#ifdef CO_DEBUG_ON_PC
	// check output address update
	for( roop_count=0; roop_count<12; roop_count++ ){
		if( ch == D_IM_CNR_CH_2 ){
			Im_CNR_OTF_Get_Latest_OutAddr( D_IM_CNR_CH_0, &out_addr );
			Ddim_Print(( "Im_CNR_OTF_Get_Latest_OutAddr() = ch=0, 0x%lx\n", out_addr ));
			Im_CNR_OTF_Get_Latest_OutAddr( D_IM_CNR_CH_1, &out_addr );
			Ddim_Print(( "Im_CNR_OTF_Get_Latest_OutAddr() = ch=1, 0x%lx\n", out_addr ));

			IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRICF.bit.__SPDF = 1;
			IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRICF.bit.__SPDF = 1;
			Im_CNR0_Int_Handler();
			Im_CNR1_Int_Handler();
		}
		else{
			Im_CNR_OTF_Get_Latest_OutAddr( ch, &out_addr );
			Ddim_Print(( "Im_CNR_OTF_Get_Latest_OutAddr() = ch=%d, 0x%lx\n", ch, out_addr ));

			IO_CNR(ch)->OTF_REG_RW.SPRICF.bit.__SPDF = 1;
			if( ch == D_IM_CNR_CH_0 ){
				Im_CNR0_Int_Handler();
			}
			else{
				Im_CNR1_Int_Handler();
			}
		}
	}
#endif //CO_DEBUG_ON_PC


	Ddim_Print(( "[After Stop]ch0 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.ACESEN.bit.SPRACS ));
	Ddim_Print(( "[After Stop]ch1 SPRACS = %u\n", IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.ACESEN.bit.SPRACS ));

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_5: "
INT32 CT_IM_CNR_OTF_3_5( UCHAR ch )
{
	INT32 im_ercd;
#ifdef CO_DEBUG_ON_PC
	UINT32 wait_factor;
#endif //CO_DEBUG_ON_PC

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Start_Async(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start Async error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Wait_End( ch, &wait_factor, 10 );
	if( im_ercd != D_IM_CNR_SEM_TIMEOUT_ERR ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
#else //!CO_DEBUG_ON_PC
	Ddim_Print(( "%s: Only debug on PC\n", D_IM_CNR_FUNC_NAME ));
	im_ercd = D_DDIM_SYSTEM_ERROR;
#endif //!CO_DEBUG_ON_PC

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_6: "
INT32 CT_IM_CNR_OTF_3_6( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 wait_factor;
	DDIM_USER_ID fid_back;
	T_IM_CNR_OTF_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,

		.divide_left = E_IM_CNR_VPROCESS_NORM,
		.divide_right = E_IM_CNR_VPROCESS_NORM,

		.w_offset_x = 1000,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_OTF_OUTADDR_INFO addr_info = {
		.use_bank_num = 1,
		.w_c_addr[0] = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}


	im_ercd = Im_CNR_OTF_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Set_OutAddr_Info( ch, &addr_info );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Set OutAddrInfo error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Start_Async(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start Async error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	fid_back = FID_IM_CNR;
	FID_IM_CNR = 0xfff;
	im_ercd = Im_CNR_OTF_Wait_End( ch, &wait_factor, 10 );
	if( im_ercd != D_IM_CNR_SEM_NG ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	FID_IM_CNR = fid_back;

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_7: "
INT32 CT_IM_CNR_OTF_3_7( UCHAR ch )
{
	INT32 im_ercd;
	DDIM_USER_ID sid_back;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	sid_back = SID_IM_CNR_OTF(ch);
	SID_IM_CNR_OTF(ch) = 0xfff;

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_IM_CNR_SEM_NG ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	SID_IM_CNR_OTF(ch) = sid_back;

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_8: "
INT32 CT_IM_CNR_OTF_3_8( UCHAR ch )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_9: "
INT32 CT_IM_CNR_OTF_3_9( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_OTF_CTRL spr_ctrl[2] = {
		[0] = {	// max
			.mode = (E_IM_CNR_OTF_YCC_MODE)7,

			.width = 2360,
			.lines = 4096,

			.w_c_width = 0x3FFF8,

			.divide_left = (E_IM_CNR_VPROCESS)1,
			.divide_right = (E_IM_CNR_VPROCESS)1,

			.w_offset_x = 2360,

			.continuous = E_IM_CNR_ONOFF_ENABLE,

			.pCallBack = (T_IM_CNR_CALLBACK)0xFFFFFFFF,
			.user_param = 0xFFFFFFFF,
		},
		[1] = {	// min
			.mode = (E_IM_CNR_OTF_YCC_MODE)0,

			.width = 96,
			.lines = 72,

			.w_c_width = 0,

			.divide_left = (E_IM_CNR_VPROCESS)0,
			.divide_right = (E_IM_CNR_VPROCESS)0,

			.w_offset_x = 0,

			.continuous = E_IM_CNR_ONOFF_DISABLE,

			.pCallBack = (T_IM_CNR_CALLBACK)0,
			.user_param = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_ctrl) / sizeof(spr_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Ctrl(ch, &spr_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_ctrl_reg(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_10: "
INT32 CT_IM_CNR_OTF_3_10( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_OTF_AXI axi_ctrl[2] = {
		[0] = {	// max
			.write_cache_type = 0xF,
			.write_protection_type = 7,
		},
		[1] = {	// min
			.write_cache_type = 0,
			.write_protection_type = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(axi_ctrl) / sizeof(axi_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Ctrl_Axi( ch, &axi_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_ctrl_axi(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_11: "
INT32 CT_IM_CNR_OTF_3_11( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OTF_AXI_STATUS axi_stat;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
//OTF	IO_CNR(ch)->OTF_REG_RW.SPRAXISET.bit.RRESP = 3;
	IO_CNR(ch)->OTF_REG_RW.SPRAXISET.bit.WRESP = 3;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_CNR_OTF_Get_AXI_Status( ch, &axi_stat );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Get error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	ct_Im_CNR_OTF_print_debug_stat_axi( &axi_stat );

#ifdef CO_DEBUG_ON_PC
//OTF	IO_CNR(ch)->OTF_REG_RW.SPRAXISET.bit.RRESP = 0;
	IO_CNR(ch)->OTF_REG_RW.SPRAXISET.bit.WRESP = 0;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_CNR_OTF_Get_AXI_Status( ch, &axi_stat );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Get error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	ct_Im_CNR_OTF_print_debug_stat_axi( &axi_stat );

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_12: "
INT32 CT_IM_CNR_OTF_3_12( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_CTRL_C spr_ctrl[3] = {
		[0] = {	// max
			.enable = (E_IM_CNR_ONOFF)1,
			.gradation_keep_en = (E_IM_CNR_ONOFF)1,
			.mid_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)1,
				.h_y_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.v_y_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.h_c_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.v_c_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.y_threshold_cor = 0xFF,
				.y_threshold_gain = 0x01FF,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)1,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)1,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)1,
			},
			.low_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)1,
				.h_y_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.v_y_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.h_c_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.v_c_threshold = {
					0xFF, 0xFF, 0xFF, 0xFF,
				},
				.y_threshold_cor = 0xFF,
				.y_threshold_gain = 0x01FF,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)1,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)1,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)1,
			},
			.whitening_value = 0xF,
			.c_blend_md = E_IM_CNR_C_BLEND_MD_HUE_SPEC,
			.c_alpha_blend = 0xF,
			.c_range_mode = E_IM_CNR_C_RANGE_MODE_0_64,
		},
		[1] = {	// min
			.enable = (E_IM_CNR_ONOFF)0,
			.gradation_keep_en = (E_IM_CNR_ONOFF)0,
			.mid_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)0,
				.h_y_threshold = {
					0, 0, 0, 0,
				},
				.v_y_threshold = {
					0, 0, 0, 0,
				},
				.h_c_threshold = {
					0, 0, 0, 0,
				},
				.v_c_threshold = {
					0, 0, 0, 0,
				},
				.y_threshold_cor = 0x00,
				.y_threshold_gain = 0x0000,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)0,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)0,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)0,
			},
			.low_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)0,
				.h_y_threshold = {
					0, 0, 0, 0,
				},
				.v_y_threshold = {
					0, 0, 0, 0,
				},
				.h_c_threshold = {
					0, 0, 0, 0,
				},
				.v_c_threshold = {
					0, 0, 0, 0,
				},
				.y_threshold_cor = 0x00,
				.y_threshold_gain = 0x0000,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)0,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)0,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)0,
			},
			.whitening_value = 0,
			.c_blend_md = E_IM_CNR_C_BLEND_MD_FIX,
			.c_alpha_blend = 0,
			.c_range_mode = E_IM_CNR_C_RANGE_MODE_0_1,
		},
		[2] = {	// countup
			.enable = (E_IM_CNR_ONOFF)0,
			.gradation_keep_en = (E_IM_CNR_ONOFF)1,
			.mid_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)1,
				.h_y_threshold = {
					0x1, 0x2, 0x3, 0x4,
				},
				.v_y_threshold = {
					0x5, 0x6, 0x7, 0x8,
				},
				.h_c_threshold = {
					0x9, 0xA, 0xB, 0xC,
				},
				.v_c_threshold = {
					0xD, 0xE, 0xF, 0x10,
				},
				.y_threshold_cor = 0x11,
				.y_threshold_gain = 0x0012,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)1,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)0,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)1,
			},
			.low_freq_nr = {
				.enable = (E_IM_CNR_ONOFF)0,
				.h_y_threshold = {
					0x21, 0x22, 0x23, 0x24,
				},
				.v_y_threshold = {
					0x25, 0x26, 0x27, 0x28,
				},
				.h_c_threshold = {
					0x29, 0x2A, 0x2B, 0x2C,
				},
				.v_c_threshold = {
					0x2D, 0x2E, 0x2F, 0x30,
				},
				.y_threshold_cor = 0x31,
				.y_threshold_gain = 0x0032,
				.y_threshold_dym_en = (E_IM_CNR_ONOFF)1,
				.y_threshold_scl_en = (E_IM_CNR_ONOFF)1,
				.c_threshold_scl_en = (E_IM_CNR_ONOFF)0,
			},
			.whitening_value = 7,
			.c_blend_md = E_IM_CNR_C_BLEND_MD_COL_DIFF_EDGE,
			.c_alpha_blend = 8,
			.c_range_mode = E_IM_CNR_C_RANGE_MODE_0_8,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_ctrl) / sizeof(spr_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Ctrl_C( ch, &spr_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_ctrl_c(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_13: "
INT32 CT_IM_CNR_OTF_3_13( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Set_CSPR_Mid_Y_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_c_mid_y_tbl(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_14: "
INT32 CT_IM_CNR_OTF_3_14( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Set_CSPR_Mid_CC_Y_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_c_mid_cc_y_tbl(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_15: "
INT32 CT_IM_CNR_OTF_3_15( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Set_CSPR_Mid_CC_C_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_c_mid_cc_c_tbl(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_16: "
INT32 CT_IM_CNR_OTF_3_16( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Set_CSPR_Low_Y_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_c_low_y_tbl(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_17: "
INT32 CT_IM_CNR_OTF_3_17( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Set_CSPR_Low_CC_Y_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_c_low_cc_y_tbl(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_18: "
INT32 CT_IM_CNR_OTF_3_18( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_TABLE spr_tbl[3] = {
		[0] = {	// max
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x7F,
			.gain1 = 0x7F,
			.gain2 = 0x7F,
			.gain3 = 0x7F,
			.gain4 = 0x7F,
			.gain5 = 0x7F,
		},
		[1] = {	// min
			.bd1 = 0,
			.bd2 = 0,
			.bd3 = 0,
			.bd4 = 0,
			.bd5 = 0,
			.of0 = 0,
			.of1 = 0,
			.of2 = 0,
			.of3 = 0,
			.of4 = 0,
			.of5 = 0,
			.gain0 = -0x80,
			.gain1 = -0x80,
			.gain2 = -0x80,
			.gain3 = -0x80,
			.gain4 = -0x80,
			.gain5 = -0x80,
		},
		[2] = {	// countup
			.bd1 = 1,
			.bd2 = 2,
			.bd3 = 3,
			.bd4 = 4,
			.bd5 = 5,
			.of0 = 6,
			.of1 = 7,
			.of2 = 8,
			.of3 = 9,
			.of4 = 0xA,
			.of5 = 0xB,
			.gain0 = 0xC,
			.gain1 = 0xD,
			.gain2 = 0xE,
			.gain3 = 0xF,
			.gain4 = 0x10,
			.gain5 = 0x11,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_tbl) / sizeof(spr_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Set_CSPR_Low_CC_C_Table( ch, &spr_tbl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_c_low_cc_c_tbl(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_19: "
INT32 CT_IM_CNR_OTF_3_19( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OTF_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,

		.divide_left = E_IM_CNR_VPROCESS_NORM,
		.divide_right = E_IM_CNR_VPROCESS_NORM,

		.w_offset_x = 1000,

		.continuous = E_IM_CNR_ONOFF_ENABLE,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_OTF_OUTADDR_INFO addr_info = {
		.use_bank_num = 1,
		.w_c_addr[0] = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};


	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_CNR_OTF_Init(ch);

	im_ercd = Im_CNR_OTF_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Set_OutAddr_Info( ch, &addr_info );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Set OutAddrInfo error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Start_Sync( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_20: "
INT32 CT_IM_CNR_OTF_3_20( UCHAR ch )
{
	INT32 im_ercd;
	const char dumpmsg[] = "ch%d SPRTRG = %u\n";

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	IO_CNR(ch)->OTF_REG_RW.SPRTRG.bit.SPRTRG = 3;
#endif //CO_DEBUG_ON_PC
	Im_CNR_On_Pclk(ch);
	Ddim_Print(( dumpmsg, D_IM_CNR_CH_0, IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRTRG.bit.SPRTRG ));
	Ddim_Print(( dumpmsg, D_IM_CNR_CH_1, IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRTRG.bit.SPRTRG ));

	Im_CNR_Off_Pclk(ch);

	im_ercd = Im_CNR_OTF_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	Im_CNR_On_Pclk(ch);
	Ddim_Print(( dumpmsg, D_IM_CNR_CH_0, IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRTRG.bit.SPRTRG ));
	Ddim_Print(( dumpmsg, D_IM_CNR_CH_1, IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRTRG.bit.SPRTRG ));
	Im_CNR_Off_Pclk(ch);

#ifdef CO_DEBUG_ON_PC
	IO_CNR(ch)->OTF_REG_RW.SPRTRG.bit.SPRTRG = 2;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_CNR_OTF_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	Im_CNR_On_Pclk(ch);
	Ddim_Print(( dumpmsg, D_IM_CNR_CH_0, IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRTRG.bit.SPRTRG ));
	Ddim_Print(( dumpmsg, D_IM_CNR_CH_1, IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRTRG.bit.SPRTRG ));
	Im_CNR_Off_Pclk(ch);

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_21: "
INT32 CT_IM_CNR_OTF_3_21( UCHAR ch )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	Im_CNR_OTF_Open(ch, 100);
	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_22: "
INT32 CT_IM_CNR_OTF_3_22( UCHAR ch )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Reset(ch);

	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Reset error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_23: "
INT32 CT_IM_CNR_OTF_3_23( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_CTRL_CC_SRC_DST spr_ctrl[3] = {
		[0] = {	//max
			.enable = (E_IM_CNR_ONOFF)1,
			.cb_table = {
				.bd1 = 0x7F,
				.bd2 = 0x7F,
				.bd3 = 0x7F,
				.bd4 = 0x7F,
				.bd5 = 0x7F,
				.of0 = 0x7F,
				.of1 = 0x7F,
				.of2 = 0x7F,
				.of3 = 0x7F,
				.of4 = 0x7F,
				.of5 = 0x7F,
				.gain0 = 0x07FF,
				.gain1 = 0x07FF,
				.gain2 = 0x07FF,
				.gain3 = 0x07FF,
				.gain4 = 0x07FF,
				.gain5 = 0x07FF
			},
			.cr_table = {
				.bd1 = 0x7F,
				.bd2 = 0x7F,
				.bd3 = 0x7F,
				.bd4 = 0x7F,
				.bd5 = 0x7F,
				.of0 = 0x7F,
				.of1 = 0x7F,
				.of2 = 0x7F,
				.of3 = 0x7F,
				.of4 = 0x7F,
				.of5 = 0x7F,
				.gain0 = 0x07FF,
				.gain1 = 0x07FF,
				.gain2 = 0x07FF,
				.gain3 = 0x07FF,
				.gain4 = 0x07FF,
				.gain5 = 0x07FF
			}
		},
		[1] = {	//min
			.enable = (E_IM_CNR_ONOFF)0,
			.cb_table = {
				.bd1 = -0x80,
				.bd2 = -0x80,
				.bd3 = -0x80,
				.bd4 = -0x80,
				.bd5 = -0x80,
				.of0 = -0x80,
				.of1 = -0x80,
				.of2 = -0x80,
				.of3 = -0x80,
				.of4 = -0x80,
				.of5 = -0x80,
				.gain0 = -0x0800,
				.gain1 = -0x0800,
				.gain2 = -0x0800,
				.gain3 = -0x0800,
				.gain4 = -0x0800,
				.gain5 = -0x0800
			},
			.cr_table = {
				.bd1 = -0x80,
				.bd2 = -0x80,
				.bd3 = -0x80,
				.bd4 = -0x80,
				.bd5 = -0x80,
				.of0 = -0x80,
				.of1 = -0x80,
				.of2 = -0x80,
				.of3 = -0x80,
				.of4 = -0x80,
				.of5 = -0x80,
				.gain0 = -0x0800,
				.gain1 = -0x0800,
				.gain2 = -0x0800,
				.gain3 = -0x0800,
				.gain4 = -0x0800,
				.gain5 = -0x0800
			}
		},
		[2] = {	//countup
			.enable = (E_IM_CNR_ONOFF)1,
			.cb_table = {
				.bd1 = 0x01,
				.bd2 = 0x02,
				.bd3 = 0x03,
				.bd4 = 0x04,
				.bd5 = 0x05,
				.of0 = 0x06,
				.of1 = 0x07,
				.of2 = 0x08,
				.of3 = 0x09,
				.of4 = 0x0A,
				.of5 = 0x0B,
				.gain0 = 0x000C,
				.gain1 = 0x000D,
				.gain2 = 0x000E,
				.gain3 = 0x000F,
				.gain4 = 0x0010,
				.gain5 = 0x0011
			},
			.cr_table = {
				.bd1 = 0x01,
				.bd2 = 0x02,
				.bd3 = 0x03,
				.bd4 = 0x04,
				.bd5 = 0x05,
				.of0 = 0x06,
				.of1 = 0x07,
				.of2 = 0x08,
				.of3 = 0x09,
				.of4 = 0x0A,
				.of5 = 0x0B,
				.gain0 = 0x000C,
				.gain1 = 0x000D,
				.gain2 = 0x000E,
				.gain3 = 0x000F,
				.gain4 = 0x0010,
				.gain5 = 0x0011
			}
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_ctrl) / sizeof(spr_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Ctrl_SPRS_CC( ch, &spr_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_ctrl_src_cc(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_24: "
INT32 CT_IM_CNR_OTF_3_24( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_CTRL_CC_SRC_DST spr_ctrl[3] = {
		[0] = {	//max
			.enable = (E_IM_CNR_ONOFF)1,
			.cb_table = {
				.bd1 = 0x7F,
				.bd2 = 0x7F,
				.bd3 = 0x7F,
				.bd4 = 0x7F,
				.bd5 = 0x7F,
				.of0 = 0x7F,
				.of1 = 0x7F,
				.of2 = 0x7F,
				.of3 = 0x7F,
				.of4 = 0x7F,
				.of5 = 0x7F,
				.gain0 = 0x07FF,
				.gain1 = 0x07FF,
				.gain2 = 0x07FF,
				.gain3 = 0x07FF,
				.gain4 = 0x07FF,
				.gain5 = 0x07FF
			},
			.cr_table = {
				.bd1 = 0x7F,
				.bd2 = 0x7F,
				.bd3 = 0x7F,
				.bd4 = 0x7F,
				.bd5 = 0x7F,
				.of0 = 0x7F,
				.of1 = 0x7F,
				.of2 = 0x7F,
				.of3 = 0x7F,
				.of4 = 0x7F,
				.of5 = 0x7F,
				.gain0 = 0x07FF,
				.gain1 = 0x07FF,
				.gain2 = 0x07FF,
				.gain3 = 0x07FF,
				.gain4 = 0x07FF,
				.gain5 = 0x07FF
			}
		},
		[1] = {	//min
			.enable = (E_IM_CNR_ONOFF)0,
			.cb_table = {
				.bd1 = -0x80,
				.bd2 = -0x80,
				.bd3 = -0x80,
				.bd4 = -0x80,
				.bd5 = -0x80,
				.of0 = -0x80,
				.of1 = -0x80,
				.of2 = -0x80,
				.of3 = -0x80,
				.of4 = -0x80,
				.of5 = -0x80,
				.gain0 = -0x0800,
				.gain1 = -0x0800,
				.gain2 = -0x0800,
				.gain3 = -0x0800,
				.gain4 = -0x0800,
				.gain5 = -0x0800
			},
			.cr_table = {
				.bd1 = -0x80,
				.bd2 = -0x80,
				.bd3 = -0x80,
				.bd4 = -0x80,
				.bd5 = -0x80,
				.of0 = -0x80,
				.of1 = -0x80,
				.of2 = -0x80,
				.of3 = -0x80,
				.of4 = -0x80,
				.of5 = -0x80,
				.gain0 = -0x0800,
				.gain1 = -0x0800,
				.gain2 = -0x0800,
				.gain3 = -0x0800,
				.gain4 = -0x0800,
				.gain5 = -0x0800
			}
		},
		[2] = {	//countup
			.enable = (E_IM_CNR_ONOFF)1,
			.cb_table = {
				.bd1 = 0x01,
				.bd2 = 0x02,
				.bd3 = 0x03,
				.bd4 = 0x04,
				.bd5 = 0x05,
				.of0 = 0x06,
				.of1 = 0x07,
				.of2 = 0x08,
				.of3 = 0x09,
				.of4 = 0x0A,
				.of5 = 0x0B,
				.gain0 = 0x000C,
				.gain1 = 0x000D,
				.gain2 = 0x000E,
				.gain3 = 0x000F,
				.gain4 = 0x0010,
				.gain5 = 0x0011
			},
			.cr_table = {
				.bd1 = 0x01,
				.bd2 = 0x02,
				.bd3 = 0x03,
				.bd4 = 0x04,
				.bd5 = 0x05,
				.of0 = 0x06,
				.of1 = 0x07,
				.of2 = 0x08,
				.of3 = 0x09,
				.of4 = 0x0A,
				.of5 = 0x0B,
				.gain0 = 0x000C,
				.gain1 = 0x000D,
				.gain2 = 0x000E,
				.gain3 = 0x000F,
				.gain4 = 0x0010,
				.gain5 = 0x0011
			}
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_ctrl) / sizeof(spr_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Ctrl_SPRD_CC( ch, &spr_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_ctrl_dst_cc(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_25: "
INT32 CT_IM_CNR_OTF_3_25( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_HUE_SPEC_TABLE spr_hue_ctrl[3] = {
		[0] = {	//max
			.cd_cb = 0x7F,
			.cd_cr = 0x7F,
			.k_cb = 0x3FF,
			.k_cr = 0x3FF,
			.bd1 = 0x7FF,
			.bd2 = 0x7FF,
			.bd3 = 0x7FF,
			.bd4 = 0x7FF,
			.bd5 = 0x7FF,
			.of0 = 0x100,
			.of1 = 0x100,
			.of2 = 0x100,
			.of3 = 0x100,
			.of4 = 0x100,
			.of5 = 0x100,
			.gain0 = 0x03FF,
			.gain1 = 0x03FF,
			.gain2 = 0x03FF,
			.gain3 = 0x03FF,
			.gain4 = 0x03FF,
			.gain5 = 0x03FF
		},
		[1] = {	//min
			.cd_cb = -0x80,
			.cd_cr = -0x80,
			.k_cb = 0,
			.k_cr = 0,
			.bd1 = 0x00,
			.bd2 = 0x00,
			.bd3 = 0x00,
			.bd4 = 0x00,
			.bd5 = 0x00,
			.of0 = 0x00,
			.of1 = 0x00,
			.of2 = 0x00,
			.of3 = 0x00,
			.of4 = 0x00,
			.of5 = 0x00,
			.gain0 = -0x0400,
			.gain1 = -0x0400,
			.gain2 = -0x0400,
			.gain3 = -0x0400,
			.gain4 = -0x0400,
			.gain5 = -0x0400,
		},
		[2] = {	//countup
			.cd_cb = 1,
			.cd_cr = 2,
			.k_cb = 3,
			.k_cr = 4,
			.bd1 = 0x05,
			.bd2 = 0x06,
			.bd3 = 0x07,
			.bd4 = 0x08,
			.bd5 = 0x09,
			.of0 = 0x0A,
			.of1 = 0x0B,
			.of2 = 0x0C,
			.of3 = 0x0D,
			.of4 = 0x0E,
			.of5 = 0x0F,
			.gain0 = 0x0010,
			.gain1 = 0x0011,
			.gain2 = 0x0012,
			.gain3 = 0x0013,
			.gain4 = 0x0014,
			.gain5 = 0x0015
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(spr_hue_ctrl) / sizeof(spr_hue_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Set_CSPR_Hue_Specified_Table( ch, &spr_hue_ctrl[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_ctrl_hue_cc(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_26: "
INT32 CT_IM_CNR_OTF_3_26( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 loopcnt;
	T_IM_CNR_OTF_OUTADDR_INFO addr_info[2] = {
		[0] = {	// max
			.use_bank_num = 1,
			.w_c_addr[0] = 0xFFFFFFFF,
		},
		[1] = {	// min
			.use_bank_num = 1,
			.w_c_addr[0] = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	for( loopcnt = 0; loopcnt < (sizeof(addr_info) / sizeof(addr_info[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		im_ercd = Im_CNR_OTF_Set_OutAddr_Info( ch, &addr_info[loopcnt] );
		if( im_ercd != D_DDIM_OK ) {
			Ddim_Print(( D_IM_CNR_FUNC_NAME "Set OutAddrInfo error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_Im_CNR_OTF_print_debug_ctrl_reg(ch);
	}

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_30: "
INT32 CT_IM_CNR_OTF_3_30( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CTRL_C* cnr_ctrl_c;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OTF_Ctrl_C( ch, &cnr_ctrl_c, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OTF_Ctrl_C() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cnr_ctrl_c;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_31: "
INT32 CT_IM_CNR_OTF_3_31( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.gradation_keep_en = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_cor = 0xFF,
			.y_threshold_gain = 0x01FF,
			.y_threshold_dym_en = E_IM_CNR_ONOFF_ENABLE,
			.y_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
			.c_y_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_cor = 0xFF,
			.y_threshold_gain = 0x01FF,
			.y_threshold_dym_en = E_IM_CNR_ONOFF_ENABLE,
			.y_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
			.c_y_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_ENABLE,
		},
		.whitening_enable = E_IM_CNR_ONOFF_ENABLE,
		.whitening_value = 0xF,
		.whitening_base = E_IM_CNR_WHITEN_BASE_MIDDLE,
		.c_blend_md = E_IM_CNR_C_BLEND_MD_COL_DIFF_EDGE,
		.c_alpha_blend = 0xF,
		.c_range = E_IM_CNR_ONOFF_ENABLE,
		.c_range_mode = E_IM_CNR_C_RANGE_MODE_0_32,
	};
	T_IM_CNR_RDMA_DATA_CTRL_C rdma_ctrl_c;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OTF_Ctrl_C( &spr_ctrl_c, &rdma_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OTF_Ctrl_C() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_ctrl_c.CSPREN   = 0x%lx\n", rdma_ctrl_c.CSPREN.word ));
	Ddim_Print(( "rdma_ctrl_c.CGDKEN   = 0x%lx\n", rdma_ctrl_c.CGDKEN.word ));
	Ddim_Print(( "rdma_ctrl_c.MCEN     = 0x%lx\n", rdma_ctrl_c.MCEN.word ));
	Ddim_Print(( "rdma_ctrl_c.MCYTHH   = 0x%lx\n", rdma_ctrl_c.MCYTHH.word ));
	Ddim_Print(( "rdma_ctrl_c.MCYTHV   = 0x%lx\n", rdma_ctrl_c.MCYTHV.word ));
	Ddim_Print(( "rdma_ctrl_c.MCCTHH   = 0x%lx\n", rdma_ctrl_c.MCCTHH.word ));
	Ddim_Print(( "rdma_ctrl_c.MCCTHV   = 0x%lx\n", rdma_ctrl_c.MCCTHV.word ));
	Ddim_Print(( "rdma_ctrl_c.MCYDYM   = 0x%lx\n", rdma_ctrl_c.MCYDYM.word ));
	Ddim_Print(( "rdma_ctrl_c.MCYDYMDT = 0x%lx\n", rdma_ctrl_c.MCYDYMDT.word ));
	Ddim_Print(( "rdma_ctrl_c.MCYSCL   = 0x%lx\n", rdma_ctrl_c.MCYSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.MCCSCL   = 0x%lx\n", rdma_ctrl_c.MCCSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.MCSSCL   = 0x%lx\n", rdma_ctrl_c.MCSSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.LCEN     = 0x%lx\n", rdma_ctrl_c.LCEN.word ));
	Ddim_Print(( "rdma_ctrl_c.LCYTHH   = 0x%lx\n", rdma_ctrl_c.LCYTHH.word ));
	Ddim_Print(( "rdma_ctrl_c.LCYTHV   = 0x%lx\n", rdma_ctrl_c.LCYTHV.word ));
	Ddim_Print(( "rdma_ctrl_c.LCCTHH   = 0x%lx\n", rdma_ctrl_c.LCCTHH.word ));
	Ddim_Print(( "rdma_ctrl_c.LCCTHV   = 0x%lx\n", rdma_ctrl_c.LCCTHV.word ));
	Ddim_Print(( "rdma_ctrl_c.LCYDYM   = 0x%lx\n", rdma_ctrl_c.LCYDYM.word ));
	Ddim_Print(( "rdma_ctrl_c.LCYDYMDT = 0x%lx\n", rdma_ctrl_c.LCYDYMDT.word ));
	Ddim_Print(( "rdma_ctrl_c.LCYSCL   = 0x%lx\n", rdma_ctrl_c.LCYSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.LCCSCL   = 0x%lx\n", rdma_ctrl_c.LCCSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.LCSSCL   = 0x%lx\n", rdma_ctrl_c.LCSSCL.word ));
	Ddim_Print(( "rdma_ctrl_c.BLEND    = 0x%lx\n", rdma_ctrl_c.BLEND.word ));
	Ddim_Print(( "rdma_ctrl_c.WHTMD    = 0x%lx\n", rdma_ctrl_c.WHTMD.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_32: "
INT32 CT_IM_CNR_OTF_3_32( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL* cspr_mid_y_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table( ch, &cspr_mid_y_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_mid_y_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_33: "
INT32 CT_IM_CNR_OTF_3_33( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE cspr_mid_y_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL rdma_cspr_mid_y_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table( &cspr_mid_y_table, &rdma_cspr_mid_y_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLOF.MCYSCLOF1 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLOF.MCYSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLOF.MCYSCLOF2 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLOF.MCYSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLGA.MCYSCLGA1 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLGA.MCYSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLGA.MCYSCLGA2 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLGA.MCYSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLBD.MCYSCLBD1 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLBD.MCYSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_mid_y_table.MCYSCLBD.MCYSCLBD2 = 0x%lx\n", rdma_cspr_mid_y_table.MCYSCLBD.MCYSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_34: "
INT32 CT_IM_CNR_OTF_3_34( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL* cspr_mid_cc_y_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_Y_Table( ch, &cspr_mid_cc_y_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_mid_cc_y_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_35: "
INT32 CT_IM_CNR_OTF_3_35( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE spr_cspr_mid_cc_y = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL rdma_cspr_mid_cc_y_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_Y_Table( &spr_cspr_mid_cc_y, &rdma_cspr_mid_cc_y_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLOF.MCCSCLOF1 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLOF.MCCSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLOF.MCCSCLOF2 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLOF.MCCSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLGA.MCCSCLGA1 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLGA.MCCSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLGA.MCCSCLGA2 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLGA.MCCSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLBD.MCCSCLBD1 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLBD.MCCSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_y_table.MCCSCLBD.MCCSCLBD2 = 0x%lx\n", rdma_cspr_mid_cc_y_table.MCCSCLBD.MCCSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_36: "
INT32 CT_IM_CNR_OTF_3_36( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL* cspr_mid_cc_c_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_C_Table( ch, &cspr_mid_cc_c_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_C_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_mid_cc_c_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_37: "
INT32 CT_IM_CNR_OTF_3_37( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE spr_cspr_mid_cc_c = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL rdma_cspr_mid_cc_c_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_C_Table( &spr_cspr_mid_cc_c, &rdma_cspr_mid_cc_c_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_C_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLOF.MCCSCLOF1 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLOF.MCSSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLOF.MCCSCLOF2 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLOF.MCSSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLGA.MCCSCLGA1 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLGA.MCSSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLGA.MCCSCLGA2 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLGA.MCSSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLBD.MCCSCLBD1 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLBD.MCSSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_mid_cc_c_table.MCSSCLBD.MCCSCLBD2 = 0x%lx\n", rdma_cspr_mid_cc_c_table.MCSSCLBD.MCSSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_38: "
INT32 CT_IM_CNR_OTF_3_38( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL* cspr_low_y_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_Y_Table( ch, &cspr_low_y_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_low_y_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_39: "
INT32 CT_IM_CNR_OTF_3_39( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE cspr_low_y_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL rdma_cspr_low_y_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OTF_CSPR_Low_Y_Table( &cspr_low_y_table, &rdma_cspr_low_y_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OTF_CSPR_Low_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLOF.LCYSCLOF1 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLOF.LCYSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLOF.LCYSCLOF2 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLOF.LCYSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLGA.LCYSCLGA1 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLGA.LCYSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLGA.LCYSCLGA2 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLGA.LCYSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLBD.LCYSCLBD1 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLBD.LCYSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_low_y_table.LCYSCLBD.LCYSCLBD2 = 0x%lx\n", rdma_cspr_low_y_table.LCYSCLBD.LCYSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_40: "
INT32 CT_IM_CNR_OTF_3_40( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL* cspr_low_cc_y_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_Y_Table( ch, &cspr_low_cc_y_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_low_cc_y_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_41: "
INT32 CT_IM_CNR_OTF_3_41( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE spr_cspr_low_cc_y = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL rdma_cspr_low_cc_y_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_Y_Table( &spr_cspr_low_cc_y, &rdma_cspr_low_cc_y_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_Y_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLOF.LCCSCLOF1 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLOF.LCCSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLOF.LCCSCLOF2 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLOF.LCCSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLGA.LCCSCLGA1 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLGA.LCCSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLGA.LCCSCLGA2 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLGA.LCCSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLBD.LCCSCLBD1 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLBD.LCCSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_y_table.LCCSCLBD.LCCSCLBD2 = 0x%lx\n", rdma_cspr_low_cc_y_table.LCCSCLBD.LCCSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_42: "
INT32 CT_IM_CNR_OTF_3_42( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL* cspr_low_cc_c_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_C_Table( ch, &cspr_low_cc_c_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_C_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_low_cc_c_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_43: "
INT32 CT_IM_CNR_OTF_3_43( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE spr_cspr_low_cc_c = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL rdma_cspr_low_cc_c_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_C_Table( &spr_cspr_low_cc_c, &rdma_cspr_low_cc_c_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_C_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLOF.LCCSCLOF1 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLOF.LCSSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLOF.LCCSCLOF2 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLOF.LCSSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLGA.LCCSCLGA1 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLGA.LCSSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLGA.LCCSCLGA2 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLGA.LCSSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLBD.LCCSCLBD1 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLBD.LCSSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_low_cc_c_table.LCSSCLBD.LCCSCLBD2 = 0x%lx\n", rdma_cspr_low_cc_c_table.LCSSCLBD.LCSSCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_44: "
INT32 CT_IM_CNR_OTF_3_44( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL* cspr_edge_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OTF_CSPR_Edge_Table( ch, &cspr_edge_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OTF_CSPR_Edge_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_edge_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_45: "
INT32 CT_IM_CNR_OTF_3_45( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_TABLE spr_cspr_edge = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0x3F,
		.of1 = 0x3F,
		.of2 = 0x3F,
		.of3 = 0x3F,
		.of4 = 0x3F,
		.of5 = 0x3F,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL rdma_cspr_edge_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OTF_CSPR_Edge_Table( &spr_cspr_edge, &rdma_cspr_edge_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OTF_CSPR_Edge_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_edge_table.CESCLOF.CESCLOF1 = 0x%lx\n", rdma_cspr_edge_table.CESCLOF.CESCLOF1.word ));
	Ddim_Print(( "rdma_cspr_edge_table.CESCLOF.CESCLOF2 = 0x%lx\n", rdma_cspr_edge_table.CESCLOF.CESCLOF2.word ));
	Ddim_Print(( "rdma_cspr_edge_table.CESCLGA.CESCLGA1 = 0x%lx\n", rdma_cspr_edge_table.CESCLGA.CESCLGA1.word ));
	Ddim_Print(( "rdma_cspr_edge_table.CESCLGA.CESCLGA2 = 0x%lx\n", rdma_cspr_edge_table.CESCLGA.CESCLGA2.word ));
	Ddim_Print(( "rdma_cspr_edge_table.CESCLBD.CESCLBD1 = 0x%lx\n", rdma_cspr_edge_table.CESCLBD.CESCLBD1.word ));
	Ddim_Print(( "rdma_cspr_edge_table.CESCLBD.CESCLBD2 = 0x%lx\n", rdma_cspr_edge_table.CESCLBD.CESCLBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_46: "
INT32 CT_IM_CNR_OTF_3_46( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL* cspr_hue_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OTF_CSPR_Hue_Spec_Table( ch, &cspr_hue_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OTF_CSPR_Hue_Spec_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_hue_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_47: "
INT32 CT_IM_CNR_OTF_3_47( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_HUE_SPEC_TABLE spr_cspr_hue = {
		.cd_cb = 0xFF,
		.cd_cr = 0xFF,
		.k_cb = 0x3FF,
		.k_cr = 0x3FF,
		.bd1 = 0x7FF,
		.bd2 = 0x7FF,
		.bd3 = 0x7FF,
		.bd4 = 0x7FF,
		.bd5 = 0x7FF,
		.of0 = 0x100,
		.of1 = 0x100,
		.of2 = 0x100,
		.of3 = 0x100,
		.of4 = 0x100,
		.of5 = 0x100,
		.gain0 = 0x07FF,
		.gain1 = 0x07FF,
		.gain2 = 0x07FF,
		.gain3 = 0x07FF,
		.gain4 = 0x07FF,
		.gain5 = 0x07FF
	};
	T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL rdma_cspr_hue_table;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OTF_CSPR_Hue_Spec_Table( &spr_cspr_hue, &rdma_cspr_hue_table );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OTF_CSPR_Hue_Spec_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_hue_table.CPHPT            = 0x%lx\n", rdma_cspr_hue_table.CPHPT.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CPHPTK           = 0x%lx\n", rdma_cspr_hue_table.CPHPTK.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLOF.CDSCLOF1 = 0x%lx\n", rdma_cspr_hue_table.CDSCLOF.CDSCLOF1.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLOF.CDSCLOF2 = 0x%lx\n", rdma_cspr_hue_table.CDSCLOF.CDSCLOF2.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLOF.CDSCLOF3 = 0x%lx\n", rdma_cspr_hue_table.CDSCLOF.CDSCLOF3.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLGA.CDSCLGA1 = 0x%lx\n", rdma_cspr_hue_table.CDSCLGA.CDSCLGA1.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLGA.CDSCLGA2 = 0x%lx\n", rdma_cspr_hue_table.CDSCLGA.CDSCLGA2.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLGA.CDSCLGA3 = 0x%lx\n", rdma_cspr_hue_table.CDSCLGA.CDSCLGA3.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLBD.CDSCLBD1 = 0x%lx\n", rdma_cspr_hue_table.CDSCLBD.CDSCLBD1.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLBD.CDSCLBD2 = 0x%lx\n", rdma_cspr_hue_table.CDSCLBD.CDSCLBD2.word ));
	Ddim_Print(( "rdma_cspr_hue_table.CDSCLBD.CDSCLBD3 = 0x%lx\n", rdma_cspr_hue_table.CDSCLBD.CDSCLBD3.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_48: "
INT32 CT_IM_CNR_OTF_3_48( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL* cspr_sprs_cc_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRS_CC_Table( ch, &cspr_sprs_cc_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRS_CC_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_sprs_cc_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_49: "
INT32 CT_IM_CNR_OTF_3_49( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_CTRL_CC_SRC_DST cspr_sprs_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRS_CC_TBL rdma_cspr_sprs_cc;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OTF_CSPR_SPRS_CC_Table( &cspr_sprs_cc, &rdma_cspr_sprs_cc );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OTF_CSPR_SPRS_CC_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRS               = 0x%lx\n", rdma_cspr_sprs_cc.SPRS.word ));

	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBOF.SPRSCBOF1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBOF.SPRSCBOF1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBOF.SPRSCBOF2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBOF.SPRSCBOF2.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA2.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA3 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBGA.SPRSCBGA3.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBBD.SPRSCBBD1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBBD.SPRSCBBD1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCBBD.SPRSCBBD2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCBBD.SPRSCBBD2.word ));

	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCROF.SPRSCROF1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCROF.SPRSCROF1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCROF.SPRSCROF2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCROF.SPRSCROF2.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA2.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA3 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCRGA.SPRSCRGA3.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCRBD.SPRSCRBD1 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCRBD.SPRSCRBD1.word ));
	Ddim_Print(( "rdma_cspr_sprs_cc.SPRSCRBD.SPRSCRBD2 = 0x%lx\n", rdma_cspr_sprs_cc.SPRSCRBD.SPRSCRBD2.word ));

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_50: "
INT32 CT_IM_CNR_OTF_3_50( UCHAR ch )
{
	INT32 im_ercd;
	const T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL* cspr_sprd_cc_tbl;
	ULONG size;
	ULONG* pt_addr;
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRD_CC_Table( ch, &cspr_sprd_cc_tbl, &size );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRD_CC_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "size = 0x%lx\n", size ));
	pt_addr = (ULONG*)cspr_sprd_cc_tbl;
	for( loopcnt = 0; loopcnt < size; loopcnt+=4 ) {
		Ddim_Print(( "address = 0x%lx\n", *pt_addr ));
		pt_addr++;
	}

	return im_ercd;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_3_51: "
INT32 CT_IM_CNR_OTF_3_51( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_CTRL_CC_SRC_DST cspr_sprd_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRD_CC_TBL rdma_cspr_sprd_cc;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	im_ercd = Im_CNR_Set_RdmaValue_OTF_CSPR_SPRD_CC_Table( &cspr_sprd_cc, &rdma_cspr_sprd_cc );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Im_CNR_Set_RdmaValue_OTF_CSPR_SPRD_CC_Table() error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	// Check parameter
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRD               = 0x%lx\n", rdma_cspr_sprd_cc.SPRD.word ));

	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBOF.SPRDCBOF1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBOF.SPRDCBOF1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBOF.SPRDCBOF2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBOF.SPRDCBOF2.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA2.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA3 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBGA.SPRDCBGA3.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBBD.SPRDCBBD1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBBD.SPRDCBBD1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCBBD.SPRDCBBD2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCBBD.SPRDCBBD2.word ));

	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCROF.SPRDCROF1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCROF.SPRDCROF1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCROF.SPRDCROF2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCROF.SPRDCROF2.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA2.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA3 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCRGA.SPRDCRGA3.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCRBD.SPRDCRBD1 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCRBD.SPRDCRBD1.word ));
	Ddim_Print(( "rdma_cspr_sprd_cc.SPRDCRBD.SPRDCRBD2 = 0x%lx\n", rdma_cspr_sprd_cc.SPRDCRBD.SPRDCRBD2.word ));

	return im_ercd;
}

static INT32 ct_Im_CNR_run_3( const UINT32 ct_no_2nd, const UINT32 ct_no_3rd )
{
	Ddim_Print(( "ct_Im_CNR_run_3 ct_no_2nd = 0x%x ct_no_3rd = 0x%x\n", ct_no_2nd, ct_no_3rd ));
	switch( ct_no_2nd ) {
		// Test-3-1
		case 1:
			return CT_IM_CNR_OTF_3_1(ct_no_3rd);

		// Test-3-2
		case 2:
			return CT_IM_CNR_OTF_3_2(ct_no_3rd);

		// Test-3-3
		case 3:
			return CT_IM_CNR_OTF_3_3(ct_no_3rd);

		// Test-3-4
		case 4:
			return CT_IM_CNR_OTF_3_4(ct_no_3rd);

		// Test-3-5
		case 5:
			return CT_IM_CNR_OTF_3_5(ct_no_3rd);

		// Test-3-6
		case 6:
			return CT_IM_CNR_OTF_3_6(ct_no_3rd);

		// Test-3-7
		case 7:
			return CT_IM_CNR_OTF_3_7(ct_no_3rd);

		// Test-3-8
		case 8:
			return CT_IM_CNR_OTF_3_8(ct_no_3rd);

		// Test-3-9
		case 9:
			return CT_IM_CNR_OTF_3_9(ct_no_3rd);

		// Test-3-10
		case 10:
			return CT_IM_CNR_OTF_3_10(ct_no_3rd);

		// Test-3-11
		case 11:
			return CT_IM_CNR_OTF_3_11(ct_no_3rd);

		// Test-3-12
		case 12:
			return CT_IM_CNR_OTF_3_12(ct_no_3rd);

		// Test-3-13
		case 13:
			return CT_IM_CNR_OTF_3_13(ct_no_3rd);

		// Test-3-14
		case 14:
			return CT_IM_CNR_OTF_3_14(ct_no_3rd);

		// Test-3-15
		case 15:
			return CT_IM_CNR_OTF_3_15(ct_no_3rd);

		// Test-3-16
		case 16:
			return CT_IM_CNR_OTF_3_16(ct_no_3rd);

		// Test-3-17
		case 17:
			return CT_IM_CNR_OTF_3_17(ct_no_3rd);

		// Test-3-18
		case 18:
			return CT_IM_CNR_OTF_3_18(ct_no_3rd);

		// Test-3-19
		case 19:
			return CT_IM_CNR_OTF_3_19(ct_no_3rd);

		// Test-3-20
		case 20:
			return CT_IM_CNR_OTF_3_20(ct_no_3rd);

		// Test-3-21
		case 21:
			return CT_IM_CNR_OTF_3_21(ct_no_3rd);

		// Test-3-22
		case 22:
			return CT_IM_CNR_OTF_3_22(ct_no_3rd);

		// Test-3-23
		case 23:
			return CT_IM_CNR_OTF_3_23(ct_no_3rd);

		// Test-3-24
		case 24:
			return CT_IM_CNR_OTF_3_24(ct_no_3rd);

		// Test-3-25
		case 25:
			return CT_IM_CNR_OTF_3_25(ct_no_3rd);

		// Test-3-26
		case 26:
			return CT_IM_CNR_OTF_3_26(ct_no_3rd);


// test RDMA
		// Test-3-30
		case 30:
			return CT_IM_CNR_OTF_3_30(ct_no_3rd);
		// Test-3-31
		case 31:
			return CT_IM_CNR_OTF_3_31(ct_no_3rd);
		// Test-3-32
		case 32:
			return CT_IM_CNR_OTF_3_32(ct_no_3rd);
		// Test-3-33
		case 33:
			return CT_IM_CNR_OTF_3_33(ct_no_3rd);
		// Test-3-34
		case 34:
			return CT_IM_CNR_OTF_3_34(ct_no_3rd);
		// Test-3-35
		case 35:
			return CT_IM_CNR_OTF_3_35(ct_no_3rd);
		// Test-3-36
		case 36:
			return CT_IM_CNR_OTF_3_36(ct_no_3rd);
		// Test-3-37
		case 37:
			return CT_IM_CNR_OTF_3_37(ct_no_3rd);
		// Test-3-38
		case 38:
			return CT_IM_CNR_OTF_3_38(ct_no_3rd);
		// Test-3-39
		case 39:
			return CT_IM_CNR_OTF_3_39(ct_no_3rd);
		// Test-3-40
		case 40:
			return CT_IM_CNR_OTF_3_40(ct_no_3rd);
		// Test-3-41
		case 41:
			return CT_IM_CNR_OTF_3_41(ct_no_3rd);
		// Test-3-42
		case 42:
			return CT_IM_CNR_OTF_3_42(ct_no_3rd);
		// Test-3-43
		case 43:
			return CT_IM_CNR_OTF_3_43(ct_no_3rd);
		// Test-3-44
		case 44:
			return CT_IM_CNR_OTF_3_44(ct_no_3rd);
		// Test-3-45
		case 45:
			return CT_IM_CNR_OTF_3_45(ct_no_3rd);
		// Test-3-46
		case 46:
			return CT_IM_CNR_OTF_3_46(ct_no_3rd);
		// Test-3-47
		case 47:
			return CT_IM_CNR_OTF_3_47(ct_no_3rd);
		// Test-3-48
		case 48:
			return CT_IM_CNR_OTF_3_48(ct_no_3rd);
		// Test-3-49
		case 49:
			return CT_IM_CNR_OTF_3_49(ct_no_3rd);
		// Test-3-50
		case 50:
			return CT_IM_CNR_OTF_3_50(ct_no_3rd);
		// Test-3-51
		case 51:
			return CT_IM_CNR_OTF_3_51(ct_no_3rd);

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_1: "
INT32 CT_IM_CNR_OTF_4_1( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Ctrl( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_2: "
INT32 CT_IM_CNR_OTF_4_2( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Ctrl_Axi( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_3: "
INT32 CT_IM_CNR_OTF_4_3( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Get_AXI_Status( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_4: "
INT32 CT_IM_CNR_OTF_4_4( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Ctrl_C( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_5: "
INT32 CT_IM_CNR_OTF_4_5( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Set_CSPR_Mid_Y_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_6: "
INT32 CT_IM_CNR_OTF_4_6( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Set_CSPR_Mid_CC_Y_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_7: "
INT32 CT_IM_CNR_OTF_4_7( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Set_CSPR_Mid_CC_C_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_8: "
INT32 CT_IM_CNR_OTF_4_8( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Set_CSPR_Low_Y_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_9: "
INT32 CT_IM_CNR_OTF_4_9( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Set_CSPR_Low_CC_Y_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_10: "
INT32 CT_IM_CNR_OTF_4_10( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Set_CSPR_Low_CC_C_Table( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_11: "
INT32 CT_IM_CNR_OTF_4_11( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Wait_End( 0, NULL, 0 );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_12: "
INT32 CT_IM_CNR_OTF_4_12( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Ctrl_SPRS_CC( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_13: "
INT32 CT_IM_CNR_OTF_4_13( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Ctrl_SPRD_CC( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_14: "
INT32 CT_IM_CNR_OTF_4_14( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Get_Latest_OutAddr( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_OTF_4_15: "
INT32 CT_IM_CNR_OTF_4_15( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Set_OutAddr_Info( ch, NULL );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

static INT32 ct_Im_CNR_run_4( const UINT32 ct_no_2nd, const UINT32 ct_no_3rd )
{
	Ddim_Print(( "ct_Im_CNR_run_4 ct_no_2nd = 0x%x ct_no_3rd = 0x%x\n", ct_no_2nd, ct_no_3rd ));
	switch( ct_no_2nd ) {
		// Test-4-1
		case 1:
			return CT_IM_CNR_OTF_4_1(ct_no_3rd);

		// Test-4-2
		case 2:
			return CT_IM_CNR_OTF_4_2(ct_no_3rd);

		// Test-4-3
		case 3:
			return CT_IM_CNR_OTF_4_3(ct_no_3rd);

		// Test-4-4
		case 4:
			return CT_IM_CNR_OTF_4_4(ct_no_3rd);

		// Test-4-5
		case 5:
			return CT_IM_CNR_OTF_4_5(ct_no_3rd);

		// Test-4-6
		case 6:
			return CT_IM_CNR_OTF_4_6(ct_no_3rd);

		// Test-4-7
		case 7:
			return CT_IM_CNR_OTF_4_7(ct_no_3rd);

		// Test-4-8
		case 8:
			return CT_IM_CNR_OTF_4_8(ct_no_3rd);

		// Test-4-9
		case 9:
			return CT_IM_CNR_OTF_4_9(ct_no_3rd);

		// Test-4-10
		case 10:
			return CT_IM_CNR_OTF_4_10(ct_no_3rd);

		// Test-4-11
		case 11:
			return CT_IM_CNR_OTF_4_11(ct_no_3rd);

		// Test-4-12
		case 12:
			return CT_IM_CNR_OTF_4_12(ct_no_3rd);

		// Test-4-13
		case 13:
			return CT_IM_CNR_OTF_4_13(ct_no_3rd);

		// Test-4-14
		case 14:
			return CT_IM_CNR_OTF_4_14(ct_no_3rd);

		// Test-4-15
		case 15:
			return CT_IM_CNR_OTF_4_15(ct_no_3rd);

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_1: "
INT32 CT_IM_CNR_5_1( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OFL_Init(ch);
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_2: "
INT32 CT_IM_CNR_5_2( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OFL_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OFL_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.r_y_width = D_IM_CNR_TEST_H_PIXS,
		.r_y_addr = D_IM_CNR_TEST_IMG_SRC_Y_ADDR,

		.r_c_width = D_IM_CNR_TEST_H_PIXS,
		.r_cb_addr = D_IM_CNR_TEST_IMG_SRC_CB_ADDR,
		.r_cr_addr = D_IM_CNR_TEST_IMG_SRC_CR_ADDR,

		.w_y_width = D_IM_CNR_TEST_H_PIXS,
		.w_y_addr = D_IM_CNR_TEST_IMG_DST_Y_ADDR,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,
		.w_cb_addr = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
		.w_cr_addr = D_IM_CNR_TEST_IMG_DST_CR_ADDR,

		.work_addr = D_IM_CNR_TEST_IMG_TMP_ADDR,

		.divide_top = E_IM_CNR_VPROCESS_NORM,
		.divide_bot = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_OFL_CTRL_Y spr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.threshold_type = E_IM_CNR_THRES_REGISTER,
		.level = E_IM_CNR_LEVEL_HIGH,
		.h_y_threshold = 0xFF,
		.v_y_threshold = 0xFF,
		.h_c_threshold = 0xFF,
		.v_c_threshold = 0xFF,
		.alpha_blend_ratio = 0x1F,
	};

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_Y( ch, &spr_ctrl_y );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_Y error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Start_Sync( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_CNR_OFL_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_3: "
INT32 CT_IM_CNR_5_3( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OFL_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OFL_YCC_MODE_YCC422,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.r_y_width = D_IM_CNR_TEST_H_PIXS,
		.r_y_addr = D_IM_CNR_TEST_IMG_SRC_Y_ADDR,

		.r_c_width = D_IM_CNR_TEST_H_PIXS,
		.r_cb_addr = D_IM_CNR_TEST_IMG_SRC_CB_ADDR,
		.r_cr_addr = D_IM_CNR_TEST_IMG_SRC_CR_ADDR,

		.w_y_width = D_IM_CNR_TEST_H_PIXS,
		.w_y_addr = D_IM_CNR_TEST_IMG_DST_Y_ADDR,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,
		.w_cb_addr = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
		.w_cr_addr = D_IM_CNR_TEST_IMG_DST_CR_ADDR,

		.work_addr = D_IM_CNR_TEST_IMG_TMP_ADDR,

		.divide_top = E_IM_CNR_VPROCESS_NORM,
		.divide_bot = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_DISABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_DISABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_OFL_CTRL_Y spr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.threshold_type = E_IM_CNR_THRES_REGISTER,
		.level = E_IM_CNR_LEVEL_HIGH,
		.h_y_threshold = 0xFF,
		.v_y_threshold = 0xFF,
		.h_c_threshold = 0xFF,
		.v_c_threshold = 0xFF,
		.alpha_blend_ratio = 0,
	};

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Ctrl_Y( ch, &spr_ctrl_y );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_Y error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OFL_Start_Sync( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_CNR_OFL_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_4: "
INT32 CT_IM_CNR_5_4( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));
	ercd = CT_IM_CNR_5_1( ch );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_5: "
INT32 CT_IM_CNR_5_5( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));
	ercd = CT_IM_CNR_5_2( ch );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_6: "
INT32 CT_IM_CNR_5_6( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));
	ercd = CT_IM_CNR_5_3( ch );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_7: "
INT32 CT_IM_CNR_5_7( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));

	ercd = Im_CNR_OTF_Init(ch);
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_8: "
INT32 CT_IM_CNR_5_8( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 wait_factor;
	T_IM_CNR_OTF_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,

		.divide_left = E_IM_CNR_VPROCESS_NORM,
		.divide_right = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_OTF_OUTADDR_INFO addr_info = {
		.use_bank_num= 1,
		.w_c_addr[0] = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Set_OutAddr_Info( ch, &addr_info );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Set OutAddrInfo error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Start_Async( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start error = 0x%x\n", im_ercd ));
	}

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	im_ercd = ct_im_cnr_set_im_r2y_set( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "ct_im_cnr_set_im_r2y_set error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	im_ercd = ct_im_cnr_set_im_r2y_start( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "ct_im_cnr_set_im_r2y_start error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
#endif	// D_IM_CNR_R2Y_MACRO_ENABLE

	im_ercd = Im_CNR_OTF_Wait_End( ch, &wait_factor, 1000 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_CNR_OTF_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	im_ercd = ct_im_cnr_set_im_r2y_stop( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "ct_im_cnr_set_im_r2y_stop error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
#endif	// D_IM_CNR_R2Y_MACRO_ENABLE

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_9: "
INT32 CT_IM_CNR_5_9( UCHAR ch )
{
	INT32 im_ercd;
	UINT32 wait_factor;
	T_IM_CNR_OTF_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,

		.divide_left = E_IM_CNR_VPROCESS_NORM,
		.divide_right = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
		.user_param = 0,
	};
	T_IM_CNR_OTF_OUTADDR_INFO addr_info = {
		.use_bank_num= 1,
		.w_c_addr[0] = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_DISABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_DISABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Set_OutAddr_Info( ch, &addr_info );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Set OutAddrInfo error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Ctrl_C error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_CNR_OTF_Start_Async( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Start error = 0x%x\n", im_ercd ));
	}

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	im_ercd = ct_im_cnr_set_im_r2y_set( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "ct_im_cnr_set_im_r2y_set error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	im_ercd = ct_im_cnr_set_im_r2y_start( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "ct_im_cnr_set_im_r2y_start error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
#endif	// D_IM_CNR_R2Y_MACRO_ENABLE

	im_ercd = Im_CNR_OTF_Wait_End( ch, &wait_factor, 1000 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_CNR_OTF_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	im_ercd = ct_im_cnr_set_im_r2y_stop( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "ct_im_cnr_set_im_r2y_stop error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
#endif	// D_IM_CNR_R2Y_MACRO_ENABLE

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_CNR_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_10: "
INT32 CT_IM_CNR_5_10( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));
	ercd = CT_IM_CNR_5_7( ch );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_11: "
INT32 CT_IM_CNR_5_11( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));
	ercd = CT_IM_CNR_5_8( ch );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_5_12: "
INT32 CT_IM_CNR_5_12( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_CNR_FUNC_NAME ));
	ercd = CT_IM_CNR_5_9( ch );
	Ddim_Print(( D_IM_CNR_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

static INT32 ct_Im_CNR_run_5( const UINT32 ct_no_2nd )
{
	Ddim_Print(( "ct_Im_CNR_run_5 ct_no_2nd = 0x%x\n", ct_no_2nd ));
	switch( ct_no_2nd ) {
		// Test-5-1
		case 1:
			return CT_IM_CNR_5_1( 0 );

		// Test-5-2
		case 2:
			return CT_IM_CNR_5_2( 0 );

		// Test-5-3
		case 3:
			return CT_IM_CNR_5_3( 0 );

		// Test-5-4
		case 4:
			return CT_IM_CNR_5_4( 1 );

		// Test-5-5
		case 5:
			return CT_IM_CNR_5_5( 1 );

		// Test-5-6
		case 6:
			return CT_IM_CNR_5_6( 1 );

		// Test-5-7
		case 7:
			return CT_IM_CNR_5_7( 0 );

		// Test-5-8
		case 8:
			return CT_IM_CNR_5_8( 0 );

		// Test-5-9
		case 9:
			return CT_IM_CNR_5_9( 0 );

		// Test-5-10
		case 10:
			return CT_IM_CNR_5_10( 1 );

		// Test-5-11
		case 11:
			return CT_IM_CNR_5_11( 1 );

		// Test-5-12
		case 12:
			return CT_IM_CNR_5_12( 1 );

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_1: "
INT32 CT_IM_CNR_6_1( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_RDMA_DATA_CTRL_C	rdma_ctrl_c;
	const  T_IM_CNR_RDMA_ADDR_CTRL_C*	cnr_ctrl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_Ctrl_C( ch, &cnr_ctrl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_Ctrl_C error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_Ctrl_C( &spr_ctrl_c, &rdma_ctrl_c );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_Ctrl_C error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)cnr_ctrl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_ctrl_c;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_ctrl_c( 0 );
		ct_Im_CNR_OFL_print_debug_ctrl_c( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_ctrl_c( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_2: "
INT32 CT_IM_CNR_6_2( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_OFL_CTRL_Y			spr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.threshold_type = E_IM_CNR_THRES_REGISTER,
		.level = E_IM_CNR_LEVEL_HIGH,
		.h_y_threshold = 0xFF,
		.v_y_threshold = 0xFF,
		.h_c_threshold = 0xFF,
		.v_c_threshold = 0xFF,
		.alpha_blend_ratio = 0x1F,
	};
	T_IM_CNR_RDMA_DATA_OFL_CTRL_Y	rdma_ctrl_y;
	const  T_IM_CNR_RDMA_ADDR_OFL_CTRL_Y*	cnr_ctrl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_Ctrl_Y( ch, &cnr_ctrl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_Ctrl_Y error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_Ctrl_Y( &spr_ctrl_y, &rdma_ctrl_y );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_Ctrl_Y error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)cnr_ctrl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_ctrl_y;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_ctrl_y( 0 );
		ct_Im_CNR_OFL_print_debug_ctrl_y( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_ctrl_y( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_3: "
INT32 CT_IM_CNR_6_3( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	yspr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_OFL_YSPR_TBL	rdma_yspr_tbl;
	const  T_IM_CNR_RDMA_ADDR_OFL_YSPR_TBL*	yspr_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_YSPR_Table( ch, &yspr_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_YSPR_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_YSPR_Table( &yspr_table, &rdma_yspr_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_YSPR_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)yspr_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_yspr_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_y_tbl( 0 );
		ct_Im_CNR_OFL_print_debug_y_tbl( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_y_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_4: "
INT32 CT_IM_CNR_6_4( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL	rdma_mid_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL*	mid_y_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table( ch, &mid_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table( &cnr_table, &rdma_mid_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_c_mid_y_tbl( 0 );
		ct_Im_CNR_OFL_print_debug_c_mid_y_tbl( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_c_mid_y_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_5: "
INT32 CT_IM_CNR_6_5( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL	rdma_mid_cc_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL*	mid_cc_y_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_Y_Table( ch, &mid_cc_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_Y_Table( &cnr_table, &rdma_mid_cc_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_cc_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_cc_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_c_mid_cc_y_tbl( 0 );
		ct_Im_CNR_OFL_print_debug_c_mid_cc_y_tbl( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_c_mid_cc_y_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_6: "
INT32 CT_IM_CNR_6_6( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL	rdma_mid_cc_c_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL*	mid_cc_c_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_C_Table( ch, &mid_cc_c_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_C_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_C_Table( &cnr_table, &rdma_mid_cc_c_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_C_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_cc_c_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_cc_c_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_c_mid_cc_c_tbl( 0 );
		ct_Im_CNR_OFL_print_debug_c_mid_cc_c_tbl( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_c_mid_cc_c_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_7: "
INT32 CT_IM_CNR_6_7( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL	rdma_low_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL*	low_y_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_Y_Table( ch, &low_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Low_Y_Table( &cnr_table, &rdma_low_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Low_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_c_low_y_tbl( 0 );
		ct_Im_CNR_OFL_print_debug_c_low_y_tbl( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_c_low_y_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_8: "
INT32 CT_IM_CNR_6_8( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL	rdma_low_cc_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL*	low_cc_y_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_Y_Table( ch, &low_cc_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_Y_Table( &cnr_table, &rdma_low_cc_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_cc_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_cc_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_c_low_cc_y_tbl( 0 );
		ct_Im_CNR_OFL_print_debug_c_low_cc_y_tbl( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_c_low_cc_y_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_9: "
INT32 CT_IM_CNR_6_9( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL	rdma_low_cc_c_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL*	low_cc_c_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_C_Table( ch, &low_cc_c_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_C_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_C_Table( &cnr_table, &rdma_low_cc_c_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_C_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_cc_c_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_cc_c_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_c_low_cc_c_tbl( 0 );
		ct_Im_CNR_OFL_print_debug_c_low_cc_c_tbl( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_c_low_cc_c_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_10: "
INT32 CT_IM_CNR_6_10( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL	rdma_edge_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL*	edge_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Edge_Table( ch, &edge_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Edge_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Edge_Table( &cnr_table, &rdma_edge_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Edge_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)edge_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_edge_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_cspr_edge_tbl( 0 );
		ct_Im_CNR_OFL_print_debug_cspr_edge_tbl( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_cspr_edge_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_11: "
INT32 CT_IM_CNR_6_11( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_HUE_SPEC_TABLE	spr_cspr_hue = {
		.cd_cb = 0xFF,
		.cd_cr = 0xFF,
		.k_cb = 0x3FF,
		.k_cr = 0x3FF,
		.bd1 = 0x7FF,
		.bd2 = 0x7FF,
		.bd3 = 0x7FF,
		.bd4 = 0x7FF,
		.bd5 = 0x7FF,
		.of0 = 0x100,
		.of1 = 0x100,
		.of2 = 0x100,
		.of3 = 0x100,
		.of4 = 0x100,
		.of5 = 0x100,
		.gain0 = 0x07FF,
		.gain1 = 0x07FF,
		.gain2 = 0x07FF,
		.gain3 = 0x07FF,
		.gain4 = 0x07FF,
		.gain5 = 0x07FF
	};
	T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL	rdma_hue_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL*	hue_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Hue_Spec_Table( ch, &hue_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Hue_Spec_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Hue_Spec_Table( &spr_cspr_hue, &rdma_hue_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Hue_Spec_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)hue_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_hue_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_ctrl_hue_cc( 0 );
		ct_Im_CNR_OFL_print_debug_ctrl_hue_cc( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_ctrl_hue_cc( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_12: "
INT32 CT_IM_CNR_6_12( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_CC_SRC_DST	cspr_sprs_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRS_CC_TBL	rdma_sprs_tbl;
	const  T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL*	sprs_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRS_CC_Table( ch, &sprs_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRS_CC_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_SPRS_CC_Table( &cspr_sprs_cc, &rdma_sprs_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_SPRS_CC_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)sprs_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_sprs_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_ctrl_src_cc( 0 );
		ct_Im_CNR_OFL_print_debug_ctrl_src_cc( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_ctrl_src_cc( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_13: "
INT32 CT_IM_CNR_6_13( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_CC_SRC_DST	cspr_sprd_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRD_CC_TBL	rdma_sprd_tbl;
	const  T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL*	sprd_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRD_CC_Table( ch, &sprd_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRD_CC_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_SPRD_CC_Table( &cspr_sprd_cc, &rdma_sprd_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_SPRD_CC_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)sprd_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_sprd_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OFL_print_debug_ctrl_dst_cc( 0 );
		ct_Im_CNR_OFL_print_debug_ctrl_dst_cc( 1 );
	}
	else{
		ct_Im_CNR_OFL_print_debug_ctrl_dst_cc( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_14: "
INT32 CT_IM_CNR_6_14( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_RDMA_DATA_CTRL_C	rdma_ctrl_c;
	const  T_IM_CNR_RDMA_ADDR_CTRL_C*	cnr_ctrl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OTF_Ctrl_C( ch, &cnr_ctrl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_Ctrl_C error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_Ctrl_C( &spr_ctrl_c, &rdma_ctrl_c );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_Ctrl_C error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)cnr_ctrl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_ctrl_c;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OTF_print_debug_c_mid_y_tbl( 0 );
		ct_Im_CNR_OTF_print_debug_c_mid_y_tbl( 1 );
	}
	else{
		ct_Im_CNR_OTF_print_debug_c_mid_y_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_15: "
INT32 CT_IM_CNR_6_15( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL	rdma_mid_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL*	mid_y_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table( ch, &mid_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table( &cnr_table, &rdma_mid_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OTF_print_debug_c_mid_y_tbl( 0 );
		ct_Im_CNR_OTF_print_debug_c_mid_y_tbl( 1 );
	}
	else{
		ct_Im_CNR_OTF_print_debug_c_mid_y_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_16: "
INT32 CT_IM_CNR_6_16( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL	rdma_mid_cc_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL*	mid_cc_y_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_Y_Table( ch, &mid_cc_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_Y_Table( &cnr_table, &rdma_mid_cc_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_cc_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_cc_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OTF_print_debug_c_mid_cc_y_tbl( 0 );
		ct_Im_CNR_OTF_print_debug_c_mid_cc_y_tbl( 1 );
	}
	else{
		ct_Im_CNR_OTF_print_debug_c_mid_cc_y_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_17: "
INT32 CT_IM_CNR_6_17( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL	rdma_mid_cc_c_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL*	mid_cc_c_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_C_Table( ch, &mid_cc_c_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_C_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_C_Table( &cnr_table, &rdma_mid_cc_c_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_C_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_cc_c_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_cc_c_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OTF_print_debug_c_mid_cc_c_tbl( 0 );
		ct_Im_CNR_OTF_print_debug_c_mid_cc_c_tbl( 1 );
	}
	else{
		ct_Im_CNR_OTF_print_debug_c_mid_cc_c_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_18: "
INT32 CT_IM_CNR_6_18( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL	rdma_low_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL*	low_y_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_Y_Table( ch, &low_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Low_Y_Table( &cnr_table, &rdma_low_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Low_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OTF_print_debug_c_low_y_tbl( 0 );
		ct_Im_CNR_OTF_print_debug_c_low_y_tbl( 1 );
	}
	else{
		ct_Im_CNR_OTF_print_debug_c_low_y_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_19: "
INT32 CT_IM_CNR_6_19( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL	rdma_low_cc_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL*	low_cc_y_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_Y_Table( ch, &low_cc_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_Y_Table( &cnr_table, &rdma_low_cc_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_Y_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_cc_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_cc_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OTF_print_debug_c_low_cc_y_tbl( 0 );
		ct_Im_CNR_OTF_print_debug_c_low_cc_y_tbl( 1 );
	}
	else{
		ct_Im_CNR_OTF_print_debug_c_low_cc_y_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_20: "
INT32 CT_IM_CNR_6_20( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL	rdma_low_cc_c_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL*	low_cc_c_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_C_Table( ch, &low_cc_c_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_C_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_C_Table( &cnr_table, &rdma_low_cc_c_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_C_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_cc_c_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_cc_c_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OTF_print_debug_c_low_cc_c_tbl( 0 );
		ct_Im_CNR_OTF_print_debug_c_low_cc_c_tbl( 1 );
	}
	else{
		ct_Im_CNR_OTF_print_debug_c_low_cc_c_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_21: "
INT32 CT_IM_CNR_6_21( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL	rdma_edge_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL*	edge_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Edge_Table( ch, &edge_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Edge_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Edge_Table( &cnr_table, &rdma_edge_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Edge_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)edge_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_edge_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OTF_print_debug_cspr_edge_tbl( 0 );
		ct_Im_CNR_OTF_print_debug_cspr_edge_tbl( 1 );
	}
	else{
		ct_Im_CNR_OTF_print_debug_cspr_edge_tbl( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_22: "
INT32 CT_IM_CNR_6_22( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_HUE_SPEC_TABLE	spr_cspr_hue = {
		.cd_cb = 0xFF,
		.cd_cr = 0xFF,
		.k_cb = 0x3FF,
		.k_cr = 0x3FF,
		.bd1 = 0x7FF,
		.bd2 = 0x7FF,
		.bd3 = 0x7FF,
		.bd4 = 0x7FF,
		.bd5 = 0x7FF,
		.of0 = 0x100,
		.of1 = 0x100,
		.of2 = 0x100,
		.of3 = 0x100,
		.of4 = 0x100,
		.of5 = 0x100,
		.gain0 = 0x07FF,
		.gain1 = 0x07FF,
		.gain2 = 0x07FF,
		.gain3 = 0x07FF,
		.gain4 = 0x07FF,
		.gain5 = 0x07FF
	};
	T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL	rdma_hue_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL*	hue_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Hue_Spec_Table( ch, &hue_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Hue_Spec_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Hue_Spec_Table( &spr_cspr_hue, &rdma_hue_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Hue_Spec_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)hue_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_hue_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OTF_print_debug_ctrl_hue_cc( 0 );
		ct_Im_CNR_OTF_print_debug_ctrl_hue_cc( 1 );
	}
	else{
		ct_Im_CNR_OTF_print_debug_ctrl_hue_cc( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_23: "
INT32 CT_IM_CNR_6_23( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_CC_SRC_DST	cspr_sprs_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRS_CC_TBL	rdma_sprs_tbl;
	const  T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL*	sprs_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRS_CC_Table( ch, &sprs_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRS_CC_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_SPRS_CC_Table( &cspr_sprs_cc, &rdma_sprs_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_SPRS_CC_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)sprs_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_sprs_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OTF_print_debug_ctrl_src_cc( 0 );
		ct_Im_CNR_OTF_print_debug_ctrl_src_cc( 1 );
	}
	else{
		ct_Im_CNR_OTF_print_debug_ctrl_src_cc( ch );
	}

	return D_DDIM_OK;
}

#undef D_IM_CNR_FUNC_NAME
#define D_IM_CNR_FUNC_NAME "CT_IM_CNR_6_24: "
INT32 CT_IM_CNR_6_24( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_CC_SRC_DST	cspr_sprd_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRD_CC_TBL	rdma_sprd_tbl;
	const  T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL*	sprd_tbl_addr;
	ULONG	size;

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRD_CC_Table( ch, &sprd_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRD_CC_Table error = 0x%x\n", ret ));
		return ret;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_SPRD_CC_Table( &cspr_sprd_cc, &rdma_sprd_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_SPRD_CC_Table error = 0x%x\n", ret ));
		return ret;
	}

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)sprd_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_sprd_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ret;
	}

	if( ch == D_IM_CNR_CH_2 ){
		ct_Im_CNR_OTF_print_debug_ctrl_dst_cc( 0 );
		ct_Im_CNR_OTF_print_debug_ctrl_dst_cc( 1 );
	}
	else{
		ct_Im_CNR_OTF_print_debug_ctrl_dst_cc( ch );
	}

	return D_DDIM_OK;
}

static INT32 ct_Im_CNR_run_6( const UINT32 ct_no_2nd, const UINT32 ct_no_3rd )
{
	Ddim_Print(( "ct_Im_CNR_run_6 ct_no_2nd = 0x%x, ct_no_3rd = 0x%x\n", ct_no_2nd, ct_no_3rd ));
	switch( ct_no_2nd ) {
		// Test-6-1
		case 1:
			return CT_IM_CNR_6_1( ct_no_3rd );

		// Test-6-2
		case 2:
			return CT_IM_CNR_6_2( ct_no_3rd );

		// Test-6-3
		case 3:
			return CT_IM_CNR_6_3( ct_no_3rd );

		// Test-6-4
		case 4:
			return CT_IM_CNR_6_4( ct_no_3rd );

		// Test-6-5
		case 5:
			return CT_IM_CNR_6_5( ct_no_3rd );

		// Test-6-6
		case 6:
			return CT_IM_CNR_6_6( ct_no_3rd );

		// Test-6-7
		case 7:
			return CT_IM_CNR_6_7( ct_no_3rd );

		// Test-6-8
		case 8:
			return CT_IM_CNR_6_8( ct_no_3rd );

		// Test-6-9
		case 9:
			return CT_IM_CNR_6_9( ct_no_3rd );

		// Test-6-10
		case 10:
			return CT_IM_CNR_6_10( ct_no_3rd );

		// Test-6-11
		case 11:
			return CT_IM_CNR_6_11( ct_no_3rd );

		// Test-6-12
		case 12:
			return CT_IM_CNR_6_12( ct_no_3rd );

		// Test-6-13
		case 13:
			return CT_IM_CNR_6_13( ct_no_3rd );

		// Test-6-14
		case 14:
			return CT_IM_CNR_6_14( ct_no_3rd );

		// Test-6-15
		case 15:
			return CT_IM_CNR_6_15( ct_no_3rd );

		// Test-6-16
		case 16:
			return CT_IM_CNR_6_16( ct_no_3rd );

		// Test-6-17
		case 17:
			return CT_IM_CNR_6_17( ct_no_3rd );

		// Test-6-18
		case 18:
			return CT_IM_CNR_6_18( ct_no_3rd );

		// Test-6-19
		case 19:
			return CT_IM_CNR_6_19( ct_no_3rd );

		// Test-6-20
		case 20:
			return CT_IM_CNR_6_20( ct_no_3rd );

		// Test-6-21
		case 21:
			return CT_IM_CNR_6_21( ct_no_3rd );

		// Test-6-22
		case 22:
			return CT_IM_CNR_6_22( ct_no_3rd );

		// Test-6-23
		case 23:
			return CT_IM_CNR_6_23( ct_no_3rd );

		// Test-6-24
		case 24:
			return CT_IM_CNR_6_24( ct_no_3rd );

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}

VOID CT_IM_CNR_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
#ifdef CO_MSG_PRINT_ON
	INT32 result = D_DDIM_INPUT_PARAM_ERROR;
#endif
	static BOOL is_init = 0;

	Ddim_Print(( "CT_IM_CNR_Run( %u, %u, %u ) Begin\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	if( is_init == 0 ) {
#ifdef CO_DEBUG_ON_PC
		memset( (VOID*)&IO_CNR1, '\0', sizeof(IO_CNR1) );
//		Dd_Top_Set_CLKSTOP_CNR1AP( 1 );
//		Dd_Top_Set_CLKSTOP_CNR1AX( 1 );
//		Dd_Top_Set_CLKSTOP_CNR1CK( 1 );
#endif // CO_DEBUG_ON_PC

		Ddim_Print(( "Im_CNR_OFL_Init() Begin\n" ));
		Im_CNR_OFL_Init(ct_idx_3rd);

#ifndef CO_DEBUG_ON_PC
//		memset( (void *)D_IM_CNR_TEST_IMG_SRC_ADDR, '\0', D_IM_CNR_TEST_IMG_SRC_BYTES );
//		memset( (void *)D_IM_CNR_TEST_IMG_DST_ADDR, '\0', D_IM_CNR_TEST_IMG_DST_BYTES );
#endif // CO_DEBUG_ON_PC

		Ddim_Print(( "Im_CNR_OFL_Init() End\n" ));
		Ddim_Print(( "Im_CNR_OTF_Init() Begin\n" ));
		Im_CNR_OTF_Init(ct_idx_3rd);

#ifndef CO_DEBUG_ON_PC
//		memset( (void *)D_IM_CNR_TEST_IMG_SRC_ADDR, '\0', D_IM_CNR_TEST_IMG_SRC_BYTES );
//		memset( (void *)D_IM_CNR_TEST_IMG_DST_ADDR, '\0', D_IM_CNR_TEST_IMG_DST_BYTES );
#endif // CO_DEBUG_ON_PC

		Ddim_Print(( "Im_CNR_OTF_Init() End\n" ));
		is_init = 1;
	}

#ifdef CO_CNR_DEBUG
	Im_CNR_Print_ClockStatus();
#endif
	Ddim_Print(( "CT_IM_CNR_Run ct_idx_1st = 0x%x ct_no_2nd = 0x%x ct_no_3rd = 0x%x\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	switch( ct_idx_1st ) {
		case 1:
			D_IM_CNR_RESULT( ct_Im_CNR_run_1( ct_idx_2nd, ct_idx_3rd ) );
			break;

		case 2:
			D_IM_CNR_RESULT( ct_Im_CNR_run_2( ct_idx_2nd, ct_idx_3rd ) );
			break;

		case 3:
			D_IM_CNR_RESULT( ct_Im_CNR_run_3( ct_idx_2nd, ct_idx_3rd ) );
			break;

		case 4:
			D_IM_CNR_RESULT( ct_Im_CNR_run_4( ct_idx_2nd, ct_idx_3rd ) );
			break;

		case 5:
			D_IM_CNR_RESULT( ct_Im_CNR_run_5( ct_idx_2nd ) );
			break;

		case 6:
			D_IM_CNR_RESULT( ct_Im_CNR_run_6( ct_idx_2nd, ct_idx_3rd ) );
			break;

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}


#ifdef CO_MSG_PRINT_ON
	Ddim_Print(( "result = 0x%x\n", result ));
#endif

#ifdef CO_CNR_DEBUG
	Im_CNR_Print_ClockStatus();
#endif

	Ddim_Print(( "CT_IM_CNR_Run( %u, %u, %u ) End\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	return;
}

VOID CT_IM_CNR_Main( INT32 argc, CHAR** argv )
{
	if( argc < 3 ) {
		Ddim_Print(( "CT_IM_CNR_Main: parameter error.\n" ));
		return;
	}

	CT_IM_CNR_Run( (UINT32)atoi((const char*)argv[1]),
				   (UINT32)atoi((const char*)argv[2]),
				   (UINT32)atoi((const char*)argv[3]) );
}

