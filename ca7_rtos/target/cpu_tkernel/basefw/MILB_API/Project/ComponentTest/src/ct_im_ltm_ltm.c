/**
 * @file		ct_im_ltm_ltm.c
 * @brief		LTM(LTM block) ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copybright 2016 Socionext Inc.</I></B>
 */

#include "ct_im_b2r.h"
#include "ct_im_ltm.h"

#include "im_b2r.h"
#include "im_ltm.h"
#include "im_r2y.h"

#include "driver_common.h"
#include "ddim_user_custom.h"
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
#endif

#include "jdsimg.h"
#include "jdsltm.h"

#include <stdlib.h>
#include <string.h>



/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// nothing special.


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

static VOID ct_im_ltm_ltm_handler( UINT32* result, UINT32 user_param );

// Max/Min table
static T_IM_LTM_LTM_CTRL_COMMON gct_im_ltm_ltm_ctrl_common[] = {
	[0] = {	// max
		.line_intr = 0x3FFF,
		.rch_burst_length_yp = D_IM_LTM_BRST_256,
		.rch_burst_length_yn = D_IM_LTM_BRST_256,
		.wch_burst_length_r = D_IM_LTM_BRST_256,
		.wch_burst_length_g = D_IM_LTM_BRST_256,
		.wch_burst_length_b = D_IM_LTM_BRST_256,
		.mask_write_r = D_IM_LTM_MASK_WRITE_DISABLE,
		.mask_write_g = D_IM_LTM_MASK_WRITE_DISABLE,
		.mask_write_b = D_IM_LTM_MASK_WRITE_DISABLE,
		.read_select_yp = D_IM_LTM_PORT_SWITCH,
		.read_select_yn = D_IM_LTM_PORT_SWITCH,
		.in_data_type_yp = D_IM_LTM_DTYP_UNPACK12,
		.in_data_type_yn = D_IM_LTM_DTYP_UNPACK12,
		.out_select =  D_IM_LTM_LTM_OUT_R2Y_DIRECT,
		.out_data_type = D_IM_LTM_DTYP_UNPACK12,
		.input_rect = {
			.img_top = 1015,
			.img_left = 846,
			.img_width = 854,
			.img_lines = 1023,
		},
		.global_width_yp = 0xFFF0,
		.global_width_yn = 0xFFF0,
	},
	[1] = {	// min
		.line_intr = 0,
		.rch_burst_length_yp = D_IM_LTM_BRST_512,
		.rch_burst_length_yn = D_IM_LTM_BRST_512,
		.wch_burst_length_r = D_IM_LTM_BRST_512,
		.wch_burst_length_g = D_IM_LTM_BRST_512,
		.wch_burst_length_b = D_IM_LTM_BRST_512,
		.mask_write_r = D_IM_LTM_MASK_WRITE_ENABLE,
		.mask_write_g = D_IM_LTM_MASK_WRITE_ENABLE,
		.mask_write_b = D_IM_LTM_MASK_WRITE_ENABLE,
		.read_select_yp = D_IM_LTM_PORT_DEFAULT,
		.read_select_yn = D_IM_LTM_PORT_DEFAULT,
		.in_data_type_yp = D_IM_LTM_DTYP_PACK12,
		.in_data_type_yn = D_IM_LTM_DTYP_PACK12,
		.out_select =  D_IM_LTM_LTM_OUT_SDRAM,
		.out_data_type = D_IM_LTM_DTYP_PACK12,
		.input_rect = {
			.img_top = 0,
			.img_left = 0,
			.img_width = 8,
			.img_lines = 8,
		},
		.global_width_yp = 0,
		.global_width_yn = 0,
	},
};

static T_IM_LTM_LTM_CTRL_SDRAM_INPUT gct_im_ltm_ltm_ctrl_sdram_in[] = {
	[0] = {	// max
		.rch_burst_length_r = D_IM_LTM_BRST_256,
		.rch_burst_length_g = D_IM_LTM_BRST_256,
		.rch_burst_length_b = D_IM_LTM_BRST_256,
		.input_global_rgb = 0xFFF0,
		.input_rect = {
			.img_top = 2264,
			.img_left = 8888,
			.img_width = 2360,
			.img_lines = 8960,
		},
	},
	[1] = {	// min
		.rch_burst_length_r = D_IM_LTM_BRST_512,
		.rch_burst_length_g = D_IM_LTM_BRST_512,
		.rch_burst_length_b = D_IM_LTM_BRST_512,
		.input_global_rgb = 0,
		.input_rect = {
			.img_top = 0,
			.img_left = 0,
			.img_width = 96,
			.img_lines = 72,
		},
	},
};

static T_IM_LTM_LTM_CTRL_B2R_DIRECT gct_im_ltm_ltm_ctrl_b2r_direct[] = {
	[0] = {	// max
		.input_rect = {
			.img_top = 2264,
			.img_left = 8888,
			.img_width = 2360,
			.img_lines = 8960,
		},
		.bypass = D_IM_LTM_ENABLE_ON,
	},
	[1] = {	// min
		.input_rect = {
			.img_top = 0,
			.img_left = 0,
			.img_width = 96,
			.img_lines = 72,
		},
		.bypass = D_IM_LTM_ENABLE_OFF,
	},
};

// TestBase table
static T_IM_LTM_LTM_CTRL_COMMON gct_im_ltm_ltm_ctrl_common_base = {
	.line_intr = 0,
	.rch_burst_length_yp = D_IM_LTM_BRST_512,
	.rch_burst_length_yn = D_IM_LTM_BRST_512,
	.wch_burst_length_r = D_IM_LTM_BRST_512,
	.wch_burst_length_g = D_IM_LTM_BRST_512,
	.wch_burst_length_b = D_IM_LTM_BRST_512,
	.mask_write_r = D_IM_LTM_MASK_WRITE_ENABLE,
	.mask_write_g = D_IM_LTM_MASK_WRITE_ENABLE,
	.mask_write_b = D_IM_LTM_MASK_WRITE_ENABLE,
	.read_select_yp = D_IM_LTM_PORT_DEFAULT,
	.read_select_yn = D_IM_LTM_PORT_DEFAULT,
	.in_data_type_yp = D_IM_LTM_DTYP_UNPACK12,
	.in_data_type_yn = D_IM_LTM_DTYP_UNPACK12,
	.out_select =  D_IM_LTM_LTM_OUT_SDRAM,
	.out_data_type = D_IM_LTM_DTYP_UNPACK12,
	.input_rect = {
		.img_top = 0,
		.img_left = 0,
		.img_width = D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH,
		.img_lines = D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_LINES,
	},
	.global_width_yp = D_IM_LTM_LTM_SRC_IMG_MAP_GLOBAL_WIDTH_UP,
	.global_width_yn = D_IM_LTM_LTM_SRC_IMG_MAP_GLOBAL_WIDTH_UP,
	.ltm_user_handler = ct_im_ltm_ltm_handler,
	.user_param = 0,
};

static T_IM_LTM_LTM_CTRL_SDRAM_INPUT gct_im_ltm_ltm_ctrl_sdram_in_base = {
	.rch_burst_length_r = D_IM_LTM_BRST_512,
	.rch_burst_length_g = D_IM_LTM_BRST_512,
	.rch_burst_length_b = D_IM_LTM_BRST_512,
	.input_global_rgb = D_IM_LTM_LTM_SRC_IMG_RGB_GLOBAL_WIDTH,
	.input_rect = {
		.img_top = 0,
		.img_left = 0,
		.img_width = D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_WIDTH,
		.img_lines = D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_LINES,
	},
};

static T_IM_LTM_LTM_CTRL_B2R_DIRECT gct_im_ltm_ltm_ctrl_b2r_direct_base = {
	.input_rect = {
		.img_top = 0,
		.img_left = 0,
		.img_width = D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_WIDTH,
		.img_lines = D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_LINES,
	},
	.bypass = D_IM_LTM_ENABLE_OFF,
};

static T_IM_LTM_INADDR_INFO gct_im_ltm_ltm_rgb_in_addr_base = {
	.addr_r = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,
	.addr_g = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_G_ADDR_TOP,
	.addr_b = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_B_ADDR_TOP,
};

static T_IM_LTM_LTM_INDATA_INFO gct_im_ltm_ltm_indata_base = {
	.addr_yp = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP,
	.addr_yn = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP,
	.top_bit_offset_yp = 0,
	.top_bit_offset_yn = 0,
};

static T_IM_LTM_LTM_OUTDATA_INFO gct_im_ltm_ltm_outdata_base = {
	.addr[0] = (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP,
	.addr[1] = (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_G_ADDR_TOP,
	.addr[2] = (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_B_ADDR_TOP,
	.global_width[0] = D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP,
	.global_width[1] = D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP,
	.global_width[2] = D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP,
};

static T_IM_LTM_LTM_CTRL_BLEND gct_im_ltm_ltm_ctrl_blend = {
	.ratio		= 8,
	.offset[0]	= 0,
	.offset[1]	= 0,
	.offset[2]	= 0,
	.offset[3]	= 0,
	.offset[4]	= 0,
	.offset[5]	= 0,
	.offset[6]	= 0,
	.offset[7]	= 4096,
	.gain[0]	= 0,
	.gain[1]	= 0,
	.gain[2]	= 0,
	.gain[3]	= 0,
	.gain[4]	= 0,
	.gain[5]	= 0,
	.gain[6]	= 0,
	.gain[7]	= 0,
	.border[0]	= 0,
	.border[1]	= 0,
	.border[2]	= 0,
	.border[3]	= 0,
	.border[4]	= 0,
	.border[5]	= 0,
	.border[6]	= 0,
	.border[7]	= 0,
	.mode		= 1,
};

static T_IM_LTM_CTRL_YLOG gct_im_ltm_ltm_ctrl_ylog_tbl = {
	.use_tbl = 0,
	.end_point0_tbl0 = 0x3FFF,
	.end_point0_tbl1 = 0x3FFF,
	.end_point1_tbl0 = 0x3FFF,
	.end_point1_tbl1 = 0x3FFF,
};

const USHORT gct_im_ltm_ltm_ylog_tbl[D_IM_LTM_TABLE_MAX_YLOG] = {
	   0,  905, 1434, 1810, 2101, 2340, 2541, 2715, 2869, 3007, 3131, 3245, 3349, 3446, 3536, 3620,
	3700, 3774, 3845, 3912, 3976, 4036, 4094, 4150, 4203, 4255, 4304, 4351, 4397, 4441, 4484, 4526,
	4566, 4605, 4643, 4680, 4715, 4750, 4784, 4817, 4849, 4881, 4912, 4942, 4971, 5000, 5028, 5055,
	5082, 5109, 5134, 5160, 5185, 5209, 5233, 5257, 5280, 5302, 5325, 5347, 5368, 5389, 5410, 5431,
	5451, 5471, 5491, 5510, 5529, 5548, 5566, 5585, 5603, 5621, 5638, 5655, 5672, 5689, 5706, 5722,
	5739, 5755, 5770, 5786, 5802, 5817, 5832, 5847, 5862, 5876, 5891, 5905, 5919, 5933, 5947, 5960,
	5974, 5987, 6001, 6014, 6027, 6040, 6052, 6065, 6077, 6090, 6102, 6114, 6126, 6138, 6150, 6162,
	6173, 6185, 6196, 6208, 6219, 6230, 6241, 6252, 6263, 6273, 6284, 6295, 6305, 6316, 6326, 6336,
	6346, 6356, 6366, 6376, 6386, 6396, 6406, 6415, 6425, 6434, 6444, 6453, 6462, 6472, 6481, 6490,
	6499, 6508, 6517, 6526, 6535, 6543, 6552, 6561, 6569, 6578, 6586, 6595, 6603, 6611, 6619, 6628,
	6636, 6644, 6652, 6660, 6668, 6676, 6684, 6691, 6699, 6707, 6714, 6722, 6730, 6737, 6745, 6752,
	6759, 6767, 6774, 6781, 6789, 6796, 6803, 6810, 6817, 6824, 6831, 6838, 6845, 6852, 6859, 6866,
	6872, 6879, 6886, 6893, 6899, 6906, 6912, 6919, 6926, 6932, 6938, 6945, 6951, 6958, 6964, 6970,
	6976, 6983, 6989, 6995, 7001, 7007, 7013, 7020, 7026, 7032, 7038, 7043, 7049, 7055, 7061, 7067,
	7073, 7079, 7084, 7090, 7096, 7102, 7107, 7113, 7118, 7124, 7130, 7135, 7141, 7146, 7152, 7157,
	7163, 7168, 7173, 7179, 7184, 7189, 7195, 7200, 7205, 7210, 7216, 7221, 7226, 7231, 7236, 7241,
	7246, 7252, 7257, 7262, 7267, 7272, 7277, 7282, 7287, 7291, 7296, 7301, 7306, 7311, 7316, 7321,
	7325, 7330, 7335, 7340, 7344, 7349, 7354, 7358, 7363, 7368, 7372, 7377, 7382, 7386, 7391, 7395,
	7400, 7404, 7409, 7413, 7418, 7422, 7427, 7431, 7435, 7440, 7444, 7449, 7453, 7457, 7462, 7466,
	7470, 7474, 7479, 7483, 7487, 7491, 7496, 7500, 7504, 7508, 7512, 7516, 7521, 7525, 7529, 7533,
	7537, 7541, 7545, 7549, 7553, 7557, 7561, 7565, 7569, 7573, 7577, 7581, 7585, 7589, 7593, 7597,
	7600, 7604, 7608, 7612, 7616, 7620, 7623, 7627, 7631, 7635, 7639, 7642, 7646, 7650, 7654, 7657,
	7661, 7665, 7668, 7672, 7676, 7679, 7683, 7687, 7690, 7694, 7697, 7701, 7705, 7708, 7712, 7715,
	7719, 7722, 7726, 7729, 7733, 7736, 7740, 7743, 7747, 7750, 7754, 7757, 7761, 7764, 7768, 7771,
	7774, 7778, 7781, 7784, 7788, 7791, 7795, 7798, 7801, 7805, 7808, 7811, 7814, 7818, 7821, 7824,
	7828, 7831, 7834, 7837, 7840, 7844, 7847, 7850, 7853, 7856, 7860, 7863, 7866, 7869, 7872, 7875,
	7879, 7882, 7885, 7888, 7891, 7894, 7897, 7900, 7903, 7906, 7910, 7913, 7916, 7919, 7922, 7925,
	7928, 7931, 7934, 7937, 7940, 7943, 7946, 7949, 7952, 7955, 7958, 7961, 7963, 7966, 7969, 7972,
	7975, 7978, 7981, 7984, 7987, 7990, 7992, 7995, 7998, 8001, 8004, 8007, 8010, 8012, 8015, 8018,
	8021, 8024, 8026, 8029, 8032, 8035, 8038, 8040, 8043, 8046, 8049, 8051, 8054, 8057, 8060, 8062,
	8065, 8068, 8070, 8073, 8076, 8079, 8081, 8084, 8087, 8089, 8092, 8095, 8097, 8100, 8103, 8105,
	8108, 8110, 8113, 8116, 8118, 8121, 8123, 8126, 8129, 8131, 8134, 8136, 8139, 8142, 8144, 8147,
};


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef IM_LTM_STATUS_PRINT
extern VOID Im_LTM_LTM_Print_Status( VOID );
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define ct_im_ltm_ltm_check_target_pipe_no_1(a)		(((a)+1) & (D_IM_LTM_PIPE1+1))
#define ct_im_ltm_ltm_check_target_pipe_no_2(a)		(((a)+1) & (D_IM_LTM_PIPE2+1))


static VOID ct_im_ltm_ltm_handler( UINT32* result, UINT32 user_param )
{
	if( ((*result) & D_IM_LTM_LTM1_INT_STATE_LTM_END) != 0 ) {
		Ddim_Print(( "LTM LTM1 end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_LTM1_INT_STATE_EXIP_END) != 0 ) {
		Ddim_Print(( "LTM LTM1 Received transmission starting pulse. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_LTM1_INT_STATE_LINE_END) != 0 ) {
		Ddim_Print(( "LTM LTM1 line end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_LTM1_INT_STATE_AXR_ERR) != 0 ) {
		Ddim_Print(( "LTM LTM1 AXR error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_LTM1_INT_STATE_AXW_ERR) != 0 ) {
		Ddim_Print(( "LTM LTM1 AXW error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_LTM1_INT_STATE_YLOG_ERR) != 0 ) {
		Ddim_Print(( "LTM LTM1 YLOG access error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_LTM2_INT_STATE_LTM_END) != 0 ) {
		Ddim_Print(( "LTM LTM2 end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_LTM2_INT_STATE_EXIP_END) != 0 ) {
		Ddim_Print(( "LTM LTM2 Received transmission starting pulse. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_LTM2_INT_STATE_LINE_END) != 0 ) {
		Ddim_Print(( "LTM LTM2 line end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_LTM2_INT_STATE_AXR_ERR) != 0 ) {
		Ddim_Print(( "LTM LTM2 AXR error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_LTM2_INT_STATE_AXW_ERR) != 0 ) {
		Ddim_Print(( "LTM LTM2 AXW error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_LTM2_INT_STATE_YLOG_ERR) != 0 ) {
		Ddim_Print(( "LTM LTM2 YLOG access error! 0x%x\n", user_param ));
	}
}

static VOID ct_im_ltm_ltm_b2r_int_handler( UINT32* result, UINT32 user_param )
{
	Ddim_Print(( "B2R Int %x, %x\n", *result, user_param ));
}

static VOID ct_im_ltm_ltm_set_b2r( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in;
	T_IM_B2R_RECT b2r_rect_param;
	T_IM_B2R_INADDR_INFO b2r_in_addr;
	static T_IM_B2R_BAYER_COLOR im_b2r_wb_gain = {
		(USHORT)(1.67592592592593F * 0x100),	// R
		(USHORT)(1.94623655913978F * 0x100),	// B
		(USHORT)(1.0F * 0x100),	// Gr
		(USHORT)(1.0F * 0x100),	// Gb
	};

	IO_IMG.IMGPIPEPSW.bit.B2RCNC = 1;

	b2r_ctrl.bayer_top_pixel                         = D_IM_B2R_TOP_PIXEL_R;
	b2r_ctrl.ybw.axi_write_mode.out_sel              = D_IM_B2R_YBW_OUTPUT_RGB;
	b2r_ctrl.ybw.axi_write_mode.burst_length         = D_IM_B2R_BRST_512;
	b2r_ctrl.ybw.write_request_mask[D_IM_B2R_PORT_R] = 0;
	b2r_ctrl.ybw.write_request_mask[D_IM_B2R_PORT_G] = 0;
	b2r_ctrl.ybw.write_request_mask[D_IM_B2R_PORT_B] = 0;
	b2r_ctrl.ybw.trim_out_enable                     = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.ybw.knee_enable                         = 0;
	b2r_ctrl.line_intr.count                         = 0;
	b2r_ctrl.ybw_continue_start_enable               = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.deknee_enable                           = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.cia_bypass                              = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.cia_padding                             = D_IM_B2R_ENABLE_ON;
	b2r_ctrl.ext_out_en                              = D_IM_B2R_ENABLE_ON;
	b2r_ctrl.ext_out_alarm                           = D_IM_B2R_EXT_OUT_ALARM_1LINE;
	b2r_ctrl.b2r_user_handler                        = ct_im_ltm_ltm_b2r_int_handler;
	b2r_ctrl.user_param                              = 0;

	b2r_ctrl_sdram_in.burst_length                   = D_IM_B2R_BRST_512;
	b2r_ctrl_sdram_in.read_request_mask              = 0;
	b2r_ctrl_sdram_in.input_dtype                    = D_IM_B2R_STL_DTYP_PACK16;
	b2r_ctrl_sdram_in.top_offset                     = 0;
	b2r_ctrl_sdram_in.input_global                   = D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_16BIT_NONPACK;
	b2r_ctrl_sdram_in.line_space                     = 0;

	b2r_rect_param.input_size.img_left               = 0;
	b2r_rect_param.input_size.img_top                = 0;
	b2r_rect_param.input_size.img_width              = D_IM_B2R_SRC_IMG_PIXS_WIDTH;
	b2r_rect_param.input_size.img_lines              = D_IM_B2R_SRC_IMG_PIXS_LINES;
	b2r_rect_param.output_size.output_global_w       = D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES;	// dummy

	b2r_in_addr.raw.addr                             = (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP;

	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_WB_Gain( pipe_no, &im_b2r_wb_gain );
}

static VOID ct_im_ltm_ltm_start_b2r( UCHAR pipe_no )
{
	Im_B2R_Start( pipe_no, D_IM_B2R_START_EXTERNAL_IF_ONLY );
}

static VOID ct_im_ltm_ltm_r2y_int_handler( UINT32* result, UINT32 user_param )
{
	Ddim_Print(( "R2Y Int %x, %x\n", *result, user_param ));
}

static VOID ct_im_ltm_ltm_set_r2y( UCHAR pipe_no )
{
	T_IM_R2Y_CTRL r2y_ctrl;
	T_IM_R2Y_CTRL_DIRECT r2y_ctrl_direct;
	T_IM_R2Y_RESIZE_RECT r2y_resize_rect_param;
	T_IM_R2Y_OUTBANK_INFO r2y_addr_0;
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

	IO_IMG.IMGPIPEPSW.bit.R2YSEL = 1;

	memset( &r2y_ctrl, '\0', sizeof(T_IM_R2Y_CTRL) );
	memset( &r2y_ctrl_direct, '\0', sizeof(T_IM_R2Y_CTRL_DIRECT) );
	memset( &r2y_resize_rect_param, '\0', sizeof(T_IM_R2Y_RESIZE_RECT) );
	memset( &r2y_addr_0, '\0', sizeof(T_IM_R2Y_OUTBANK_INFO) );

	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.master_if_select = D_IM_R2Y_MASTER_IF0_PORT0 | D_IM_R2Y_MASTER_IF0_PORT1 | D_IM_R2Y_MASTER_IF0_PORT2;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.burst_length = D_IM_R2Y_BRST_512;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_0] = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_1] = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_2] = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_0] = 0;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_1] = 0;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_2] = 0;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].resize_mode = D_IM_R2Y_RSZ_BILINEAR;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].pixel_avg_reduction_mode = D_IM_R2Y_RDC_MODE_DIV_2;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].pixel_avg_reduction_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].ycc_cc_thin_select = D_IM_R2Y_THIN_OUT_YCC422_CENTER;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].trim_out_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].planar_interleaved_mode = D_IM_R2Y_PORT_INTERLEAVED;
	r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_0] = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.yyw_continue_start_enable = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.r2y_user_handler = ct_im_ltm_ltm_r2y_int_handler;
	r2y_ctrl.user_param = 0;

	r2y_resize_rect_param.input_size.img_left = 0;
	r2y_resize_rect_param.input_size.img_top = 0;
	r2y_resize_rect_param.input_size.img_width = 640;
	r2y_resize_rect_param.input_size.img_lines = 480;
	r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].yyw_width = 640;
	r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].yyw_lines = 480;
	r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].output_global_w[D_IM_R2Y_PORT_Y] = 640;
	r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].output_global_w[D_IM_R2Y_PORT_CBCR] = 640;
	r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].output_global_w[D_IM_R2Y_PORT_2] = 0;	// dummy

	r2y_addr_0.bank_initial_position = D_IM_R2Y_YYW_BANK_0;
	r2y_addr_0.use_bank_num = D_IM_R2Y_YYW_BANK_0;
	r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Y  = (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_Y_ADDR_TOP;
	r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Cb = (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_C_ADDR_TOP;
	r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Cr = 0;

	Im_R2Y_Ctrl( pipe_no, &r2y_ctrl );
	Im_R2Y_Ctrl_ModeDirect( pipe_no, &r2y_ctrl_direct );
	Im_R2Y_Set_Resize_Rect( pipe_no, &r2y_resize_rect_param );
	Im_R2Y_Set_OutBankInfo( pipe_no, D_IM_R2Y_YYW_CH_0, &r2y_addr_0 );
	Im_R2Y_Ctrl_Gamma( pipe_no, &r2y_ctrl_gamma );
	Im_R2Y_Set_Gamma_Table( pipe_no, 0, dgamma_abs, dgamma_ofs );
}

static VOID ct_im_ltm_ltm_start_r2y( UCHAR pipe_no )
{
	Im_R2Y_Start( pipe_no );
	Im_R2Y_Set_FrameStop( pipe_no, 1 );
}

static VOID ct_im_ltm_ltm_print_ctrl_common_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.LTM.LLINTLV.bit.LINTLV     = 0x%x\n", IO_LTM_P1.LTM.LLINTLV.bit.LINTLV     ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYP  = 0x%x\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYP  ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYN  = 0x%x\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYN  ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWR   = 0x%x\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWR   ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWG   = 0x%x\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWG   ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWB   = 0x%x\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWB   ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKR  = 0x%x\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKR  ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKG  = 0x%x\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKG  ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKB  = 0x%x\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKB  ));
	Ddim_Print(( "IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL1   = 0x%x\n", IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL1   ));
	Ddim_Print(( "IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL2   = 0x%x\n", IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL2   ));
	Ddim_Print(( "IO_LTM_P1.LTM.LIOCTL.bit.R3DTYP      = 0x%x\n", IO_LTM_P1.LTM.LIOCTL.bit.R3DTYP      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LIOCTL.bit.R4DTYP      = 0x%x\n", IO_LTM_P1.LTM.LIOCTL.bit.R4DTYP      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LIOCTL.bit.OUTSEL      = 0x%x\n", IO_LTM_P1.LTM.LIOCTL.bit.OUTSEL      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LIOCTL.bit.W1DTYP      = 0x%x\n", IO_LTM_P1.LTM.LIOCTL.bit.W1DTYP      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRHSZR3.bit.SDRHSZR3 = 0x%x\n", IO_LTM_P1.LTM.LSDRHSZR3.bit.SDRHSZR3 ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRDEFR3.bit.SDRDEFR3 = 0x%x\n", IO_LTM_P1.LTM.LSDRDEFR3.bit.SDRDEFR3 ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRVSZR3.bit.SDRVSZR3 = 0x%x\n", IO_LTM_P1.LTM.LSDRVSZR3.bit.SDRVSZR3 ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRDEFR4.bit.SDRDEFR4 = 0x%x\n", IO_LTM_P1.LTM.LSDRDEFR4.bit.SDRDEFR4 ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.LTM.LLINTLV.bit.LINTLV     = 0x%x\n", IO_LTM_P2.LTM.LLINTLV.bit.LINTLV     ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYP  = 0x%x\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYP  ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYN  = 0x%x\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYN  ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWR   = 0x%x\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWR   ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWG   = 0x%x\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWG   ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWB   = 0x%x\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWB   ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKR  = 0x%x\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKR  ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKG  = 0x%x\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKG  ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKB  = 0x%x\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKB  ));
	Ddim_Print(( "IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL1   = 0x%x\n", IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL1   ));
	Ddim_Print(( "IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL2   = 0x%x\n", IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL2   ));
	Ddim_Print(( "IO_LTM_P2.LTM.LIOCTL.bit.R3DTYP      = 0x%x\n", IO_LTM_P2.LTM.LIOCTL.bit.R3DTYP      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LIOCTL.bit.R4DTYP      = 0x%x\n", IO_LTM_P2.LTM.LIOCTL.bit.R4DTYP      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LIOCTL.bit.OUTSEL      = 0x%x\n", IO_LTM_P2.LTM.LIOCTL.bit.OUTSEL      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LIOCTL.bit.W1DTYP      = 0x%x\n", IO_LTM_P2.LTM.LIOCTL.bit.W1DTYP      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRHSZR3.bit.SDRHSZR3 = 0x%x\n", IO_LTM_P2.LTM.LSDRHSZR3.bit.SDRHSZR3 ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRDEFR3.bit.SDRDEFR3 = 0x%x\n", IO_LTM_P2.LTM.LSDRDEFR3.bit.SDRDEFR3 ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRVSZR3.bit.SDRVSZR3 = 0x%x\n", IO_LTM_P2.LTM.LSDRVSZR3.bit.SDRVSZR3 ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRDEFR4.bit.SDRDEFR4 = 0x%x\n", IO_LTM_P2.LTM.LSDRDEFR4.bit.SDRDEFR4 ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_ltm_print_ctrl_sdram_input_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRR   = 0x%x\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRR   ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRG   = 0x%x\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRG   ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRB   = 0x%x\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRB   ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRDEFR2.bit.SDRDEFR2 = 0x%x\n", IO_LTM_P1.LTM.LSDRDEFR2.bit.SDRDEFR2 ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRHSZR2.bit.SDRHSZR2 = 0x%x\n", IO_LTM_P1.LTM.LSDRHSZR2.bit.SDRHSZR2 ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRVSZR2.bit.SDRVSZR2 = 0x%x\n", IO_LTM_P1.LTM.LSDRVSZR2.bit.SDRVSZR2 ));
	Ddim_Print(( "IO_LTM_P1.LTM.LIOCTL.bit.INSEL       = 0x%x\n", IO_LTM_P1.LTM.LIOCTL.bit.INSEL       ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRR   = 0x%x\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRR   ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRG   = 0x%x\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRG   ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRB   = 0x%x\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRB   ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRDEFR2.bit.SDRDEFR2 = 0x%x\n", IO_LTM_P2.LTM.LSDRDEFR2.bit.SDRDEFR2 ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRHSZR2.bit.SDRHSZR2 = 0x%x\n", IO_LTM_P2.LTM.LSDRHSZR2.bit.SDRHSZR2 ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRVSZR2.bit.SDRVSZR2 = 0x%x\n", IO_LTM_P2.LTM.LSDRVSZR2.bit.SDRVSZR2 ));
	Ddim_Print(( "IO_LTM_P2.LTM.LIOCTL.bit.INSEL       = 0x%x\n", IO_LTM_P2.LTM.LIOCTL.bit.INSEL       ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_ltm_print_b2r_direct_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.LTM.LIOCTL.bit.INSEL       = 0x%x\n", IO_LTM_P1.LTM.LIOCTL.bit.INSEL       ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRHSZR2.bit.SDRHSZR2 = 0x%x\n", IO_LTM_P1.LTM.LSDRHSZR2.bit.SDRHSZR2 ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRVSZR2.bit.SDRVSZR2 = 0x%x\n", IO_LTM_P1.LTM.LSDRVSZR2.bit.SDRVSZR2 ));
	Ddim_Print(( "IO_LTM_P1.LTM.LIOCTL.bit.BYPASS      = 0x%x\n", IO_LTM_P1.LTM.LIOCTL.bit.BYPASS      ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.LTM.LIOCTL.bit.INSEL       = 0x%x\n", IO_LTM_P2.LTM.LIOCTL.bit.INSEL       ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRHSZR2.bit.SDRHSZR2 = 0x%x\n", IO_LTM_P2.LTM.LSDRHSZR2.bit.SDRHSZR2 ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRVSZR2.bit.SDRVSZR2 = 0x%x\n", IO_LTM_P2.LTM.LSDRVSZR2.bit.SDRVSZR2 ));
	Ddim_Print(( "IO_LTM_P2.LTM.LIOCTL.bit.BYPASS      = 0x%x\n", IO_LTM_P2.LTM.LIOCTL.bit.BYPASS      ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_ltm_print_table( UCHAR pipe_no, UCHAR tbl_no )
{
	USHORT loopcnt;

	Im_LTM_On_Hclk( D_IM_LTM_PIPE12 );
	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		switch( tbl_no ){
			case 0:
			default:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P1.LTM_SRAM.LYLOG0TBL0[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P1.LTM_SRAM.LYLOG0TBL0.hword[loopcnt] ));
				}
				break;

			case 1:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P1.LTM_SRAM.LYLOG0TBL1[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P1.LTM_SRAM.LYLOG0TBL1.hword[loopcnt] ));
				}
				break;

			case 2:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P1.LTM_SRAM.LYLOG1TBL0[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P1.LTM_SRAM.LYLOG1TBL0.hword[loopcnt] ));
				}
				break;

			case 3:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P1.LTM_SRAM.LYLOG1TBL1[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P1.LTM_SRAM.LYLOG1TBL1.hword[loopcnt] ));
				}
				break;
		}
	}

	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		switch( tbl_no ){
			case 0:
			default:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P2.LTM_SRAM.LYLOG0TBL0[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P2.LTM_SRAM.LYLOG0TBL0.hword[loopcnt] ));
				}
				break;

			case 1:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P2.LTM_SRAM.LYLOG0TBL1[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P2.LTM_SRAM.LYLOG0TBL1.hword[loopcnt] ));
				}
				break;

			case 2:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P2.LTM_SRAM.LYLOG1TBL0[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P2.LTM_SRAM.LYLOG1TBL0.hword[loopcnt] ));
				}
				break;

			case 3:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P2.LTM_SRAM.LYLOG1TBL1[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P2.LTM_SRAM.LYLOG1TBL1.hword[loopcnt] ));
				}
				break;
		}
	}
	Im_LTM_Off_Hclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_ltm_print_all_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.LTM.LTMSR        = 0x%lx\n", IO_LTM_P1.LTM.LTMSR.word        ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSRAMEN      = 0x%lx\n", IO_LTM_P1.LTM.LSRAMEN.word      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LTRG         = 0x%lx\n", IO_LTM_P1.LTM.LTRG.word         ));
	Ddim_Print(( "IO_LTM_P1.LTM.LFRSTP       = 0x%lx\n", IO_LTM_P1.LTM.LFRSTP.word       ));
	Ddim_Print(( "IO_LTM_P1.LTM.LPARMSK      = 0x%lx\n", IO_LTM_P1.LTM.LPARMSK.word      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LINT0        = 0x%lx\n", IO_LTM_P1.LTM.LINT0.word        ));
	Ddim_Print(( "IO_LTM_P1.LTM.LINT1        = 0x%lx\n", IO_LTM_P1.LTM.LINT1.word        ));
	Ddim_Print(( "IO_LTM_P1.LTM.LLINTLV      = 0x%lx\n", IO_LTM_P1.LTM.LLINTLV.word      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR1    = 0x%lx\n", IO_LTM_P1.LTM.LAXICTLR1.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR2    = 0x%lx\n", IO_LTM_P1.LTM.LAXICTLR2.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIRERR     = 0x%lx\n", IO_LTM_P1.LTM.LAXIRERR.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLW     = 0x%lx\n", IO_LTM_P1.LTM.LAXICTLW.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIWERR     = 0x%lx\n", IO_LTM_P1.LTM.LAXIWERR.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ     = 0x%lx\n", IO_LTM_P1.LTM.LAXIBSIZ.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.AXIRSEL      = 0x%lx\n", IO_LTM_P1.LTM.AXIRSEL.word      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRHSZR2    = 0x%lx\n", IO_LTM_P1.LTM.LSDRHSZR2.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRDEFR2    = 0x%lx\n", IO_LTM_P1.LTM.LSDRDEFR2.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRVSZR2    = 0x%lx\n", IO_LTM_P1.LTM.LSDRVSZR2.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR2R    = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR2R.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR2G    = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR2G.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR2B    = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR2B.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRHSZR3    = 0x%lx\n", IO_LTM_P1.LTM.LSDRHSZR3.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRDEFR3    = 0x%lx\n", IO_LTM_P1.LTM.LSDRDEFR3.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRVSZR3    = 0x%lx\n", IO_LTM_P1.LTM.LSDRVSZR3.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR3     = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR3.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRDEFR4    = 0x%lx\n", IO_LTM_P1.LTM.LSDRDEFR4.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR4     = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR4.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDROFS      = 0x%lx\n", IO_LTM_P1.LTM.LSDROFS.word      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDWADR      = 0x%lx\n", IO_LTM_P1.LTM.LSDWADR.word      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDWDEFR     = 0x%lx\n", IO_LTM_P1.LTM.LSDWDEFR.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDWADG      = 0x%lx\n", IO_LTM_P1.LTM.LSDWADG.word      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDWDEFG     = 0x%lx\n", IO_LTM_P1.LTM.LSDWDEFG.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDWADB      = 0x%lx\n", IO_LTM_P1.LTM.LSDWADB.word      ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDWDEFB     = 0x%lx\n", IO_LTM_P1.LTM.LSDWDEFB.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.LIOCTL       = 0x%lx\n", IO_LTM_P1.LTM.LIOCTL.word       ));
	Ddim_Print(( "IO_LTM_P1.LTM.LHSTA        = 0x%lx\n", IO_LTM_P1.LTM.LHSTA.word        ));
	Ddim_Print(( "IO_LTM_P1.LTM.LHPIT        = 0x%lx\n", IO_LTM_P1.LTM.LHPIT.word        ));
	Ddim_Print(( "IO_LTM_P1.LTM.LVSTA        = 0x%lx\n", IO_LTM_P1.LTM.LVSTA.word        ));
	Ddim_Print(( "IO_LTM_P1.LTM.LVPIT        = 0x%lx\n", IO_LTM_P1.LTM.LVPIT.word        ));
	Ddim_Print(( "IO_LTM_P1.LTM.NLVTHOF7     = 0x%lx\n", IO_LTM_P1.LTM.NLVTHOF7.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.PLVTHOF7     = 0x%lx\n", IO_LTM_P1.LTM.PLVTHOF7.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.LYSHADOW     = 0x%lx\n", IO_LTM_P1.LTM.LYSHADOW.word     ));
	Ddim_Print(( "IO_LTM_P1.LTM.LYHIGHLIGHT  = 0x%lx\n", IO_LTM_P1.LTM.LYHIGHLIGHT.word  ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDYRT       = 0x%lx\n", IO_LTM_P1.LTM.BLDYRT.word       ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF1 = 0x%lx\n", IO_LTM_P1.LTM.BLDOF.BLDOF1.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF2 = 0x%lx\n", IO_LTM_P1.LTM.BLDOF.BLDOF2.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF3 = 0x%lx\n", IO_LTM_P1.LTM.BLDOF.BLDOF3.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF4 = 0x%lx\n", IO_LTM_P1.LTM.BLDOF.BLDOF4.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA1 = 0x%lx\n", IO_LTM_P1.LTM.BLDGA.BLDGA1.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA2 = 0x%lx\n", IO_LTM_P1.LTM.BLDGA.BLDGA2.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA3 = 0x%lx\n", IO_LTM_P1.LTM.BLDGA.BLDGA3.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA4 = 0x%lx\n", IO_LTM_P1.LTM.BLDGA.BLDGA4.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD1 = 0x%lx\n", IO_LTM_P1.LTM.BLDBD.BLDBD1.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD2 = 0x%lx\n", IO_LTM_P1.LTM.BLDBD.BLDBD2.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD3 = 0x%lx\n", IO_LTM_P1.LTM.BLDBD.BLDBD3.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD4 = 0x%lx\n", IO_LTM_P1.LTM.BLDBD.BLDBD4.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.LTMMD        = 0x%lx\n", IO_LTM_P1.LTM.LTMMD.word        ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF1   = 0x%lx\n", IO_LTM_P1.LTM.CGOF.CGOF1.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF2   = 0x%lx\n", IO_LTM_P1.LTM.CGOF.CGOF2.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF3   = 0x%lx\n", IO_LTM_P1.LTM.CGOF.CGOF3.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF4   = 0x%lx\n", IO_LTM_P1.LTM.CGOF.CGOF4.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA1   = 0x%lx\n", IO_LTM_P1.LTM.CGGA.CGGA1.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA2   = 0x%lx\n", IO_LTM_P1.LTM.CGGA.CGGA2.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA3   = 0x%lx\n", IO_LTM_P1.LTM.CGGA.CGGA3.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA4   = 0x%lx\n", IO_LTM_P1.LTM.CGGA.CGGA4.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGBD.CGBD1   = 0x%lx\n", IO_LTM_P1.LTM.CGBD.CGBD1.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGBD.CGBD2   = 0x%lx\n", IO_LTM_P1.LTM.CGBD.CGBD2.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGBD.CGBD3   = 0x%lx\n", IO_LTM_P1.LTM.CGBD.CGBD3.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.CGBD.CGBD4   = 0x%lx\n", IO_LTM_P1.LTM.CGBD.CGBD4.word   ));
	Ddim_Print(( "IO_LTM_P1.LTM.LYLCTL       = 0x%lx\n", IO_LTM_P1.LTM.LYLCTL.word       ));
	Ddim_Print(( "IO_LTM_P1.LTM.LYLOG0EP0    = 0x%lx\n", IO_LTM_P1.LTM.LYLOG0EP0.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LYLOG0EP1    = 0x%lx\n", IO_LTM_P1.LTM.LYLOG0EP1.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LYLOG1EP0    = 0x%lx\n", IO_LTM_P1.LTM.LYLOG1EP0.word    ));
	Ddim_Print(( "IO_LTM_P1.LTM.LYLOG1EP1    = 0x%lx\n", IO_LTM_P1.LTM.LYLOG1EP1.word    ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.LTM.LTMSR        = 0x%lx\n", IO_LTM_P2.LTM.LTMSR.word        ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSRAMEN      = 0x%lx\n", IO_LTM_P2.LTM.LSRAMEN.word      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LTRG         = 0x%lx\n", IO_LTM_P2.LTM.LTRG.word         ));
	Ddim_Print(( "IO_LTM_P2.LTM.LFRSTP       = 0x%lx\n", IO_LTM_P2.LTM.LFRSTP.word       ));
	Ddim_Print(( "IO_LTM_P2.LTM.LPARMSK      = 0x%lx\n", IO_LTM_P2.LTM.LPARMSK.word      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LINT0        = 0x%lx\n", IO_LTM_P2.LTM.LINT0.word        ));
	Ddim_Print(( "IO_LTM_P2.LTM.LINT1        = 0x%lx\n", IO_LTM_P2.LTM.LINT1.word        ));
	Ddim_Print(( "IO_LTM_P2.LTM.LLINTLV      = 0x%lx\n", IO_LTM_P2.LTM.LLINTLV.word      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR1    = 0x%lx\n", IO_LTM_P2.LTM.LAXICTLR1.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR2    = 0x%lx\n", IO_LTM_P2.LTM.LAXICTLR2.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIRERR     = 0x%lx\n", IO_LTM_P2.LTM.LAXIRERR.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLW     = 0x%lx\n", IO_LTM_P2.LTM.LAXICTLW.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIWERR     = 0x%lx\n", IO_LTM_P2.LTM.LAXIWERR.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ     = 0x%lx\n", IO_LTM_P2.LTM.LAXIBSIZ.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.AXIRSEL      = 0x%lx\n", IO_LTM_P2.LTM.AXIRSEL.word      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRHSZR2    = 0x%lx\n", IO_LTM_P2.LTM.LSDRHSZR2.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRDEFR2    = 0x%lx\n", IO_LTM_P2.LTM.LSDRDEFR2.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRVSZR2    = 0x%lx\n", IO_LTM_P2.LTM.LSDRVSZR2.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR2R    = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR2R.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR2G    = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR2G.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR2B    = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR2B.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRHSZR3    = 0x%lx\n", IO_LTM_P2.LTM.LSDRHSZR3.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRDEFR3    = 0x%lx\n", IO_LTM_P2.LTM.LSDRDEFR3.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRVSZR3    = 0x%lx\n", IO_LTM_P2.LTM.LSDRVSZR3.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR3     = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR3.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRDEFR4    = 0x%lx\n", IO_LTM_P2.LTM.LSDRDEFR4.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR4     = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR4.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDROFS      = 0x%lx\n", IO_LTM_P2.LTM.LSDROFS.word      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDWADR      = 0x%lx\n", IO_LTM_P2.LTM.LSDWADR.word      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDWDEFR     = 0x%lx\n", IO_LTM_P2.LTM.LSDWDEFR.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDWADG      = 0x%lx\n", IO_LTM_P2.LTM.LSDWADG.word      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDWDEFG     = 0x%lx\n", IO_LTM_P2.LTM.LSDWDEFG.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDWADB      = 0x%lx\n", IO_LTM_P2.LTM.LSDWADB.word      ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDWDEFB     = 0x%lx\n", IO_LTM_P2.LTM.LSDWDEFB.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.LIOCTL       = 0x%lx\n", IO_LTM_P2.LTM.LIOCTL.word       ));
	Ddim_Print(( "IO_LTM_P2.LTM.LHSTA        = 0x%lx\n", IO_LTM_P2.LTM.LHSTA.word        ));
	Ddim_Print(( "IO_LTM_P2.LTM.LHPIT        = 0x%lx\n", IO_LTM_P2.LTM.LHPIT.word        ));
	Ddim_Print(( "IO_LTM_P2.LTM.LVSTA        = 0x%lx\n", IO_LTM_P2.LTM.LVSTA.word        ));
	Ddim_Print(( "IO_LTM_P2.LTM.LVPIT        = 0x%lx\n", IO_LTM_P2.LTM.LVPIT.word        ));
	Ddim_Print(( "IO_LTM_P2.LTM.NLVTHOF7     = 0x%lx\n", IO_LTM_P2.LTM.NLVTHOF7.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.PLVTHOF7     = 0x%lx\n", IO_LTM_P2.LTM.PLVTHOF7.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.LYSHADOW     = 0x%lx\n", IO_LTM_P2.LTM.LYSHADOW.word     ));
	Ddim_Print(( "IO_LTM_P2.LTM.LYHIGHLIGHT  = 0x%lx\n", IO_LTM_P2.LTM.LYHIGHLIGHT.word  ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDYRT       = 0x%lx\n", IO_LTM_P2.LTM.BLDYRT.word       ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF1 = 0x%lx\n", IO_LTM_P2.LTM.BLDOF.BLDOF1.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF2 = 0x%lx\n", IO_LTM_P2.LTM.BLDOF.BLDOF2.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF3 = 0x%lx\n", IO_LTM_P2.LTM.BLDOF.BLDOF3.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF4 = 0x%lx\n", IO_LTM_P2.LTM.BLDOF.BLDOF4.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA1 = 0x%lx\n", IO_LTM_P2.LTM.BLDGA.BLDGA1.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA2 = 0x%lx\n", IO_LTM_P2.LTM.BLDGA.BLDGA2.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA3 = 0x%lx\n", IO_LTM_P2.LTM.BLDGA.BLDGA3.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA4 = 0x%lx\n", IO_LTM_P2.LTM.BLDGA.BLDGA4.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD1 = 0x%lx\n", IO_LTM_P2.LTM.BLDBD.BLDBD1.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD2 = 0x%lx\n", IO_LTM_P2.LTM.BLDBD.BLDBD2.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD3 = 0x%lx\n", IO_LTM_P2.LTM.BLDBD.BLDBD3.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD4 = 0x%lx\n", IO_LTM_P2.LTM.BLDBD.BLDBD4.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.LTMMD        = 0x%lx\n", IO_LTM_P2.LTM.LTMMD.word        ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF1   = 0x%lx\n", IO_LTM_P2.LTM.CGOF.CGOF1.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF2   = 0x%lx\n", IO_LTM_P2.LTM.CGOF.CGOF2.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF3   = 0x%lx\n", IO_LTM_P2.LTM.CGOF.CGOF3.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF4   = 0x%lx\n", IO_LTM_P2.LTM.CGOF.CGOF4.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA1   = 0x%lx\n", IO_LTM_P2.LTM.CGGA.CGGA1.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA2   = 0x%lx\n", IO_LTM_P2.LTM.CGGA.CGGA2.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA3   = 0x%lx\n", IO_LTM_P2.LTM.CGGA.CGGA3.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA4   = 0x%lx\n", IO_LTM_P2.LTM.CGGA.CGGA4.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGBD.CGBD1   = 0x%lx\n", IO_LTM_P2.LTM.CGBD.CGBD1.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGBD.CGBD2   = 0x%lx\n", IO_LTM_P2.LTM.CGBD.CGBD2.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGBD.CGBD3   = 0x%lx\n", IO_LTM_P2.LTM.CGBD.CGBD3.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.CGBD.CGBD4   = 0x%lx\n", IO_LTM_P2.LTM.CGBD.CGBD4.word   ));
	Ddim_Print(( "IO_LTM_P2.LTM.LYLCTL       = 0x%lx\n", IO_LTM_P2.LTM.LYLCTL.word       ));
	Ddim_Print(( "IO_LTM_P2.LTM.LYLOG0EP0    = 0x%lx\n", IO_LTM_P2.LTM.LYLOG0EP0.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LYLOG0EP1    = 0x%lx\n", IO_LTM_P2.LTM.LYLOG0EP1.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LYLOG1EP0    = 0x%lx\n", IO_LTM_P2.LTM.LYLOG1EP0.word    ));
	Ddim_Print(( "IO_LTM_P2.LTM.LYLOG1EP1    = 0x%lx\n", IO_LTM_P2.LTM.LYLOG1EP1.word    ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_1: "
INT32 CT_Im_Ltm_Ltm_1_1( UCHAR pipe_no )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Init( pipe_no );

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.LTM.LTMSR   = 0x%lx\n", IO_LTM_P1.LTM.LTMSR.word ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSRAMEN = 0x%lx\n", IO_LTM_P1.LTM.LSRAMEN.word ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.LTM.LTMSR   = 0x%lx\n", IO_LTM_P2.LTM.LTMSR.word ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSRAMEN = 0x%lx\n", IO_LTM_P2.LTM.LSRAMEN.word ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_2: "
INT32 CT_Im_Ltm_Ltm_1_2( UCHAR pipe_no )
{
	UINT32					loopcnt;
	T_IM_LTM_LTM_CTRL_AXI	ltm_ctrl_axi[] = {
		[0] = {	// max
			.rch_rgb[0].cache_type = 0xF,
			.rch_rgb[0].protect_type = 7,
			.rch_rgb[1].cache_type = 0xF,
			.rch_rgb[1].protect_type = 7,
			.rch_rgb[2].cache_type = 0xF,
			.rch_rgb[2].protect_type = 7,
			.rch_y[0].cache_type = 0xF,
			.rch_y[0].protect_type = 7,
			.rch_y[1].cache_type = 0xF,
			.rch_y[1].protect_type = 7,
			.wch[0].cache_type = 0xF,
			.wch[0].protect_type = 7,
			.wch[1].cache_type = 0xF,
			.wch[1].protect_type = 7,
			.wch[2].cache_type = 0xF,
			.wch[2].protect_type = 7,
		},
		[1] = {	// min
			.rch_rgb[0].cache_type = 0,
			.rch_rgb[0].protect_type = 0,
			.rch_rgb[1].cache_type = 0,
			.rch_rgb[1].protect_type = 0,
			.rch_rgb[2].cache_type = 0,
			.rch_rgb[2].protect_type = 0,
			.rch_y[0].cache_type = 0,
			.rch_y[0].protect_type = 0,
			.rch_y[1].cache_type = 0,
			.rch_y[1].protect_type = 0,
			.wch[0].cache_type = 0,
			.wch[0].protect_type = 0,
			.wch[1].cache_type = 0,
			.wch[1].protect_type = 0,
			.wch[2].cache_type = 0,
			.wch[2].protect_type = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(ltm_ctrl_axi) / sizeof(ltm_ctrl_axi[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Ctrl_Axi( pipe_no, &ltm_ctrl_axi[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR1.bit.ARCACHE_R  = %u\n", IO_LTM_P1.LTM.LAXICTLR1.bit.ARCACHE_R  ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR1.bit.ARPROT_R   = %u\n", IO_LTM_P1.LTM.LAXICTLR1.bit.ARPROT_R   ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR1.bit.ARCACHE_G  = %u\n", IO_LTM_P1.LTM.LAXICTLR1.bit.ARCACHE_G  ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR1.bit.ARPROT_G   = %u\n", IO_LTM_P1.LTM.LAXICTLR1.bit.ARPROT_G   ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR1.bit.ARCACHE_B  = %u\n", IO_LTM_P1.LTM.LAXICTLR1.bit.ARCACHE_B  ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR1.bit.ARPROT_B   = %u\n", IO_LTM_P1.LTM.LAXICTLR1.bit.ARPROT_B   ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR2.bit.ARCACHE_YP = %u\n", IO_LTM_P1.LTM.LAXICTLR2.bit.ARCACHE_YP ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR2.bit.ARPROT_YP  = %u\n", IO_LTM_P1.LTM.LAXICTLR2.bit.ARPROT_YP  ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR2.bit.ARCACHE_YN = %u\n", IO_LTM_P1.LTM.LAXICTLR2.bit.ARCACHE_YN ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLR2.bit.ARPROT_YN  = %u\n", IO_LTM_P1.LTM.LAXICTLR2.bit.ARPROT_YN  ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLW.bit.AWCACHE_R   = %u\n", IO_LTM_P1.LTM.LAXICTLW.bit.AWCACHE_R   ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLW.bit.AWPROT_R    = %u\n", IO_LTM_P1.LTM.LAXICTLW.bit.AWPROT_R    ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLW.bit.AWCACHE_G   = %u\n", IO_LTM_P1.LTM.LAXICTLW.bit.AWCACHE_G   ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLW.bit.AWPROT_G    = %u\n", IO_LTM_P1.LTM.LAXICTLW.bit.AWPROT_G    ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLW.bit.AWCACHE_B   = %u\n", IO_LTM_P1.LTM.LAXICTLW.bit.AWCACHE_B   ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXICTLW.bit.AWPROT_B    = %u\n", IO_LTM_P1.LTM.LAXICTLW.bit.AWPROT_B    ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR1.bit.ARCACHE_R  = %u\n", IO_LTM_P2.LTM.LAXICTLR1.bit.ARCACHE_R  ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR1.bit.ARPROT_R   = %u\n", IO_LTM_P2.LTM.LAXICTLR1.bit.ARPROT_R   ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR1.bit.ARCACHE_G  = %u\n", IO_LTM_P2.LTM.LAXICTLR1.bit.ARCACHE_G  ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR1.bit.ARPROT_G   = %u\n", IO_LTM_P2.LTM.LAXICTLR1.bit.ARPROT_G   ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR1.bit.ARCACHE_B  = %u\n", IO_LTM_P2.LTM.LAXICTLR1.bit.ARCACHE_B  ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR1.bit.ARPROT_B   = %u\n", IO_LTM_P2.LTM.LAXICTLR1.bit.ARPROT_B   ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR2.bit.ARCACHE_YP = %u\n", IO_LTM_P2.LTM.LAXICTLR2.bit.ARCACHE_YP ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR2.bit.ARPROT_YP  = %u\n", IO_LTM_P2.LTM.LAXICTLR2.bit.ARPROT_YP  ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR2.bit.ARCACHE_YN = %u\n", IO_LTM_P2.LTM.LAXICTLR2.bit.ARCACHE_YN ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLR2.bit.ARPROT_YN  = %u\n", IO_LTM_P2.LTM.LAXICTLR2.bit.ARPROT_YN  ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLW.bit.AWCACHE_R   = %u\n", IO_LTM_P2.LTM.LAXICTLW.bit.AWCACHE_R   ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLW.bit.AWPROT_R    = %u\n", IO_LTM_P2.LTM.LAXICTLW.bit.AWPROT_R    ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLW.bit.AWCACHE_G   = %u\n", IO_LTM_P2.LTM.LAXICTLW.bit.AWCACHE_G   ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLW.bit.AWPROT_G    = %u\n", IO_LTM_P2.LTM.LAXICTLW.bit.AWPROT_G    ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLW.bit.AWCACHE_B   = %u\n", IO_LTM_P2.LTM.LAXICTLW.bit.AWCACHE_B   ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXICTLW.bit.AWPROT_B    = %u\n", IO_LTM_P2.LTM.LAXICTLW.bit.AWPROT_B    ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_3: "
INT32 CT_Im_Ltm_Ltm_1_3( UCHAR pipe_no )
{
	T_IM_LTM_LTM_AXI_RCH_STAT ltm_axi_read_stat;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.LTM.LAXIRERR.bit.RRESP_R  = 3;
		IO_LTM_P1.LTM.LAXIRERR.bit.RRESP_G  = 3;
		IO_LTM_P1.LTM.LAXIRERR.bit.RRESP_B  = 3;
		IO_LTM_P1.LTM.LAXIRERR.bit.RRESP_YP = 3;
		IO_LTM_P1.LTM.LAXIRERR.bit.RRESP_YN = 3;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.LTM.LAXIRERR.bit.RRESP_R  = 3;
		IO_LTM_P2.LTM.LAXIRERR.bit.RRESP_G  = 3;
		IO_LTM_P2.LTM.LAXIRERR.bit.RRESP_B  = 3;
		IO_LTM_P2.LTM.LAXIRERR.bit.RRESP_YP = 3;
		IO_LTM_P2.LTM.LAXIRERR.bit.RRESP_YN = 3;
	}
#endif // CO_DEBUG_ON_PC
	memset( &ltm_axi_read_stat, '\0', sizeof(ltm_axi_read_stat) );
	Im_LTM_LTM_Get_AxiReadStat( pipe_no, &ltm_axi_read_stat );
	Ddim_Print(( "rch_rgb_axi_stat[0] = %u\n", ltm_axi_read_stat.rch_rgb_axi_stat[0] ));
	Ddim_Print(( "rch_rgb_axi_stat[1] = %u\n", ltm_axi_read_stat.rch_rgb_axi_stat[1] ));
	Ddim_Print(( "rch_rgb_axi_stat[2] = %u\n", ltm_axi_read_stat.rch_rgb_axi_stat[2] ));
	Ddim_Print(( "rch_y_axi_stat[0]   = %u\n", ltm_axi_read_stat.rch_y_axi_stat[0]   ));
	Ddim_Print(( "rch_y_axi_stat[1]   = %u\n", ltm_axi_read_stat.rch_y_axi_stat[1]   ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.LTM.LAXIRERR.bit.RRESP_R  = 0;
		IO_LTM_P1.LTM.LAXIRERR.bit.RRESP_G  = 0;
		IO_LTM_P1.LTM.LAXIRERR.bit.RRESP_B  = 0;
		IO_LTM_P1.LTM.LAXIRERR.bit.RRESP_YP = 0;
		IO_LTM_P1.LTM.LAXIRERR.bit.RRESP_YN = 0;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.LTM.LAXIRERR.bit.RRESP_R  = 0;
		IO_LTM_P2.LTM.LAXIRERR.bit.RRESP_G  = 0;
		IO_LTM_P2.LTM.LAXIRERR.bit.RRESP_B  = 0;
		IO_LTM_P2.LTM.LAXIRERR.bit.RRESP_YP = 0;
		IO_LTM_P2.LTM.LAXIRERR.bit.RRESP_YN = 0;
	}
#endif // CO_DEBUG_ON_PC
	memset( &ltm_axi_read_stat, '\0', sizeof(ltm_axi_read_stat) );
	Im_LTM_LTM_Get_AxiReadStat( pipe_no, &ltm_axi_read_stat );
	Ddim_Print(( "rch_rgb_axi_stat[0] = %u\n", ltm_axi_read_stat.rch_rgb_axi_stat[0] ));
	Ddim_Print(( "rch_rgb_axi_stat[1] = %u\n", ltm_axi_read_stat.rch_rgb_axi_stat[1] ));
	Ddim_Print(( "rch_rgb_axi_stat[2] = %u\n", ltm_axi_read_stat.rch_rgb_axi_stat[2] ));
	Ddim_Print(( "rch_y_axi_stat[0]   = %u\n", ltm_axi_read_stat.rch_y_axi_stat[0]   ));
	Ddim_Print(( "rch_y_axi_stat[1]   = %u\n", ltm_axi_read_stat.rch_y_axi_stat[1]   ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_4: "
INT32 CT_Im_Ltm_Ltm_1_4( UCHAR pipe_no )
{
	T_IM_LTM_LTM_AXI_WCH_STAT ltm_axi_write_stat;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.LTM.LAXIWERR.bit.WRESP_R = 3;
		IO_LTM_P1.LTM.LAXIWERR.bit.WRESP_G = 3;
		IO_LTM_P1.LTM.LAXIWERR.bit.WRESP_B = 3;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.LTM.LAXIWERR.bit.WRESP_R = 3;
		IO_LTM_P2.LTM.LAXIWERR.bit.WRESP_G = 3;
		IO_LTM_P2.LTM.LAXIWERR.bit.WRESP_B = 3;
	}
#endif // CO_DEBUG_ON_PC
	memset( &ltm_axi_write_stat, '\0', sizeof(ltm_axi_write_stat) );
	Im_LTM_LTM_Get_AxiWriteStat( pipe_no, &ltm_axi_write_stat );
	Ddim_Print(( "wch_axi_stat[0] = %u\n", ltm_axi_write_stat.wch_axi_stat[0] ));
	Ddim_Print(( "wch_axi_stat[1] = %u\n", ltm_axi_write_stat.wch_axi_stat[1] ));
	Ddim_Print(( "wch_axi_stat[2] = %u\n", ltm_axi_write_stat.wch_axi_stat[2] ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.LTM.LAXIWERR.bit.WRESP_R = 0;
		IO_LTM_P1.LTM.LAXIWERR.bit.WRESP_G = 0;
		IO_LTM_P1.LTM.LAXIWERR.bit.WRESP_B = 0;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.LTM.LAXIWERR.bit.WRESP_R = 0;
		IO_LTM_P2.LTM.LAXIWERR.bit.WRESP_G = 0;
		IO_LTM_P2.LTM.LAXIWERR.bit.WRESP_B = 0;
	}
#endif // CO_DEBUG_ON_PC
	memset( &ltm_axi_write_stat, '\0', sizeof(ltm_axi_write_stat) );
	Im_LTM_LTM_Get_AxiWriteStat( pipe_no, &ltm_axi_write_stat );
	Ddim_Print(( "wch_axi_stat[0] = %u\n", ltm_axi_write_stat.wch_axi_stat[0] ));
	Ddim_Print(( "wch_axi_stat[1] = %u\n", ltm_axi_write_stat.wch_axi_stat[1] ));
	Ddim_Print(( "wch_axi_stat[2] = %u\n", ltm_axi_write_stat.wch_axi_stat[2] ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_5: "
INT32 CT_Im_Ltm_Ltm_1_5( UCHAR pipe_no )
{
	T_IM_LTM_LTM_AXI_CTRL_PARAM ltm_axi_ctrl_param;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRR  = 1;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRG  = 1;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRB  = 1;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYP = 1;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYN = 1;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWR  = 1;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWG  = 1;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWB  = 1;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKR = 1;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKG = 1;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKB = 1;
		IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL1  = 1;
		IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL2  = 1;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRR  = 1;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRG  = 1;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRB  = 1;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYP = 1;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYN = 1;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWR  = 1;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWG  = 1;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWB  = 1;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKR = 1;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKG = 1;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKB = 1;
		IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL1  = 1;
		IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL2  = 1;
	}
	memset( &ltm_axi_ctrl_param, '\0', sizeof(ltm_axi_ctrl_param) );
	Im_LTM_LTM_Get_AxiCtrlParam( pipe_no, &ltm_axi_ctrl_param );
	Ddim_Print(( "rch_burst_length_r  = %u\n", ltm_axi_ctrl_param.rch_burst_length_r  ));
	Ddim_Print(( "rch_burst_length_g  = %u\n", ltm_axi_ctrl_param.rch_burst_length_g  ));
	Ddim_Print(( "rch_burst_length_b  = %u\n", ltm_axi_ctrl_param.rch_burst_length_b  ));
	Ddim_Print(( "rch_burst_length_yp = %u\n", ltm_axi_ctrl_param.rch_burst_length_yp ));
	Ddim_Print(( "rch_burst_length_yn = %u\n", ltm_axi_ctrl_param.rch_burst_length_yn ));
	Ddim_Print(( "wch_burst_length_r  = %u\n", ltm_axi_ctrl_param.wch_burst_length_r  ));
	Ddim_Print(( "wch_burst_length_g  = %u\n", ltm_axi_ctrl_param.wch_burst_length_g  ));
	Ddim_Print(( "wch_burst_length_b  = %u\n", ltm_axi_ctrl_param.wch_burst_length_b  ));
	Ddim_Print(( "mask_write_r        = %u\n", ltm_axi_ctrl_param.mask_write_r        ));
	Ddim_Print(( "mask_write_g        = %u\n", ltm_axi_ctrl_param.mask_write_g        ));
	Ddim_Print(( "mask_write_b        = %u\n", ltm_axi_ctrl_param.mask_write_b        ));
	Ddim_Print(( "read_select_yp      = %u\n", ltm_axi_ctrl_param.read_select_yp      ));
	Ddim_Print(( "read_select_yn      = %u\n", ltm_axi_ctrl_param.read_select_yn      ));

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRR  = 0;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRG  = 0;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRB  = 0;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYP = 0;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYN = 0;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWR  = 0;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWG  = 0;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWB  = 0;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKR = 0;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKG = 0;
		IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKB = 0;
		IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL1  = 0;
		IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL2  = 0;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRR  = 0;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRG  = 0;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRB  = 0;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYP = 0;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYN = 0;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWR  = 0;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWG  = 0;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWB  = 0;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKR = 0;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKG = 0;
		IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKB = 0;
		IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL1  = 0;
		IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL2  = 0;
	}
	memset( &ltm_axi_ctrl_param, '\0', sizeof(ltm_axi_ctrl_param) );
	Im_LTM_LTM_Get_AxiCtrlParam( pipe_no, &ltm_axi_ctrl_param );
	Ddim_Print(( "rch_burst_length_r  = %u\n", ltm_axi_ctrl_param.rch_burst_length_r  ));
	Ddim_Print(( "rch_burst_length_g  = %u\n", ltm_axi_ctrl_param.rch_burst_length_g  ));
	Ddim_Print(( "rch_burst_length_b  = %u\n", ltm_axi_ctrl_param.rch_burst_length_b  ));
	Ddim_Print(( "rch_burst_length_yp = %u\n", ltm_axi_ctrl_param.rch_burst_length_yp ));
	Ddim_Print(( "rch_burst_length_yn = %u\n", ltm_axi_ctrl_param.rch_burst_length_yn ));
	Ddim_Print(( "wch_burst_length_r  = %u\n", ltm_axi_ctrl_param.wch_burst_length_r  ));
	Ddim_Print(( "wch_burst_length_g  = %u\n", ltm_axi_ctrl_param.wch_burst_length_g  ));
	Ddim_Print(( "wch_burst_length_b  = %u\n", ltm_axi_ctrl_param.wch_burst_length_b  ));
	Ddim_Print(( "mask_write_r        = %u\n", ltm_axi_ctrl_param.mask_write_r        ));
	Ddim_Print(( "mask_write_g        = %u\n", ltm_axi_ctrl_param.mask_write_g        ));
	Ddim_Print(( "mask_write_b        = %u\n", ltm_axi_ctrl_param.mask_write_b        ));
	Ddim_Print(( "read_select_yp      = %u\n", ltm_axi_ctrl_param.read_select_yp      ));
	Ddim_Print(( "read_select_yn      = %u\n", ltm_axi_ctrl_param.read_select_yn      ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_6: "
INT32 CT_Im_Ltm_Ltm_1_6( UCHAR pipe_no )
{
	UINT32						loopcnt;
	T_IM_LTM_LTM_AXI_CTRL_PARAM	ltm_axi_ctrl_param[] = {
		[0] = {	// max
			.rch_burst_length_r = 1,
			.rch_burst_length_g = 1,
			.rch_burst_length_b = 1,
			.rch_burst_length_yp = 1,
			.rch_burst_length_yn = 1,
			.wch_burst_length_r = 1,
			.wch_burst_length_g = 1,
			.wch_burst_length_b = 1,
			.mask_write_r = 1,
			.mask_write_g = 1,
			.mask_write_b = 1,
			.read_select_yp = 1,
			.read_select_yn = 1,
		},
		[1] = {	// min
			.rch_burst_length_r = 0,
			.rch_burst_length_g = 0,
			.rch_burst_length_b = 0,
			.rch_burst_length_yp = 0,
			.rch_burst_length_yn = 0,
			.wch_burst_length_r = 0,
			.wch_burst_length_g = 0,
			.wch_burst_length_b = 0,
			.mask_write_r = 0,
			.mask_write_g = 0,
			.mask_write_b = 0,
			.read_select_yp = 0,
			.read_select_yn = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(ltm_axi_ctrl_param) / sizeof(ltm_axi_ctrl_param[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Set_AxiCtrlParam( pipe_no, &ltm_axi_ctrl_param[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRR  = %u\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRR  ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRG  = %u\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRG  ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRB  = %u\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRB  ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYP = %u\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYP ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYN = %u\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTRYN ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWR  = %u\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWR  ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWG  = %u\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWG  ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWB  = %u\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.BURSTWB  ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKR = %u\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKR ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKG = %u\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKG ));
		Ddim_Print(( "IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKB = %u\n", IO_LTM_P1.LTM.LAXIBSIZ.bit.WSTBMSKB ));
		Ddim_Print(( "IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL1  = %u\n", IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL1  ));
		Ddim_Print(( "IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL2  = %u\n", IO_LTM_P1.LTM.AXIRSEL.bit.AXIRSEL2  ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRR  = %u\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRR  ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRG  = %u\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRG  ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRB  = %u\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRB  ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYP = %u\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYP ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYN = %u\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTRYN ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWR  = %u\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWR  ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWG  = %u\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWG  ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWB  = %u\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.BURSTWB  ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKR = %u\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKR ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKG = %u\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKG ));
		Ddim_Print(( "IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKB = %u\n", IO_LTM_P2.LTM.LAXIBSIZ.bit.WSTBMSKB ));
		Ddim_Print(( "IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL1  = %u\n", IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL1  ));
		Ddim_Print(( "IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL2  = %u\n", IO_LTM_P2.LTM.AXIRSEL.bit.AXIRSEL2  ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_7: "
INT32 CT_Im_Ltm_Ltm_1_7( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(gct_im_ltm_ltm_ctrl_common) / sizeof(gct_im_ltm_ltm_ctrl_common[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common[loopcnt] );

		ct_im_ltm_ltm_print_ctrl_common_reg();
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_8: "
INT32 CT_Im_Ltm_Ltm_1_8( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(gct_im_ltm_ltm_ctrl_sdram_in) / sizeof(gct_im_ltm_ltm_ctrl_sdram_in[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in[loopcnt] );

		ct_im_ltm_ltm_print_ctrl_sdram_input_reg();
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_9: "
INT32 CT_Im_Ltm_Ltm_1_9( UCHAR pipe_no )
{
	UINT32 loopcnt;
	T_IM_LTM_LTM_CTRL_SDRAM_INPUT ltm_ctrl_sdram_in = gct_im_ltm_ltm_ctrl_sdram_in_base;
	T_IM_LTM_INADDR_INFO ltm_in_addr[] = {
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
			.addr_r = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,
			.addr_g = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_G_ADDR_TOP,
			.addr_b = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_B_ADDR_TOP,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &ltm_ctrl_sdram_in );

	for( loopcnt = 0; loopcnt < (sizeof(ltm_in_addr) / sizeof(ltm_in_addr[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &ltm_in_addr[loopcnt] );

		ct_im_ltm_ltm_print_ctrl_sdram_input_reg();

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR2R.bit.SDRADR2R = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR2R.bit.SDRADR2R ));
		Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR2G.bit.SDRADR2G = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR2G.bit.SDRADR2G ));
		Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR2B.bit.SDRADR2B = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR2B.bit.SDRADR2B ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR2R.bit.SDRADR2R = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR2R.bit.SDRADR2R ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR2G.bit.SDRADR2G = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR2G.bit.SDRADR2G ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR2B.bit.SDRADR2B = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR2B.bit.SDRADR2B ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	ltm_ctrl_sdram_in.input_rect.img_top = 100;
	ltm_ctrl_sdram_in.input_rect.img_left = 200;

	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &ltm_ctrl_sdram_in );

	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &ltm_in_addr[2] );

	ct_im_ltm_ltm_print_ctrl_sdram_input_reg();

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR2R.bit.SDRADR2R = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR2R.bit.SDRADR2R ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR2G.bit.SDRADR2G = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR2G.bit.SDRADR2G ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR2B.bit.SDRADR2B = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR2B.bit.SDRADR2B ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR2R.bit.SDRADR2R = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR2R.bit.SDRADR2R ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR2G.bit.SDRADR2G = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR2G.bit.SDRADR2G ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR2B.bit.SDRADR2B = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR2B.bit.SDRADR2B ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_10: "
INT32 CT_Im_Ltm_Ltm_1_10( UCHAR pipe_no )
{
	UINT32 loopcnt;
	T_IM_LTM_LTM_CTRL_COMMON ltm_ctrl_common = gct_im_ltm_ltm_ctrl_common_base;
	T_IM_LTM_LTM_INDATA_INFO ltm_in_data[] = {
		[0] = { // max
			.addr_yp = (VOID*)0xFFFFFFFE,
			.addr_yn = (VOID*)0xFFFFFFFE,
			.top_bit_offset_yp = 1,
			.top_bit_offset_yn = 1,
		},
		[1] = { // min
			.addr_yp = (VOID*)0,
			.addr_yn = (VOID*)0,
			.top_bit_offset_yp = 0,
			.top_bit_offset_yn = 0,
		},
		[2] = { // indvisual
			.addr_yp = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP,
			.addr_yn = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP,
			.top_bit_offset_yp = 0,
			.top_bit_offset_yn = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );

	for( loopcnt = 0; loopcnt < (sizeof(ltm_in_data) / sizeof(ltm_in_data[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Set_InData_Info( pipe_no, &ltm_in_data[loopcnt] );

		ct_im_ltm_ltm_print_ctrl_sdram_input_reg();

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR3.bit.SDRADR3 = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR3.bit.SDRADR3 ));
		Ddim_Print(( "IO_LTM_P1.LTM.LSDROFS.bit.SDROFSR3 = 0x%x\n",  IO_LTM_P1.LTM.LSDROFS.bit.SDROFSR3 ));
		Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR4.bit.SDRADR4 = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR4.bit.SDRADR4 ));
		Ddim_Print(( "IO_LTM_P1.LTM.LSDROFS.bit.SDROFSR4 = 0x%x\n",  IO_LTM_P1.LTM.LSDROFS.bit.SDROFSR4 ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR3.bit.SDRADR3 = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR3.bit.SDRADR3 ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDROFS.bit.SDROFSR3 = 0x%x\n",  IO_LTM_P2.LTM.LSDROFS.bit.SDROFSR3 ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR4.bit.SDRADR4 = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR4.bit.SDRADR4 ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDROFS.bit.SDROFSR4 = 0x%x\n",  IO_LTM_P2.LTM.LSDROFS.bit.SDROFSR4 ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	ltm_ctrl_common.input_rect.img_top = 100;
	ltm_ctrl_common.input_rect.img_left = 200;

	Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );

	Im_LTM_LTM_Set_InData_Info( pipe_no, &ltm_in_data[2] );

	ct_im_ltm_ltm_print_ctrl_sdram_input_reg();

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR3.bit.SDRADR3 = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR3.bit.SDRADR3 ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDROFS.bit.SDROFSR3 = 0x%x\n",  IO_LTM_P1.LTM.LSDROFS.bit.SDROFSR3 ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDRADR4.bit.SDRADR4 = 0x%lx\n", IO_LTM_P1.LTM.LSDRADR4.bit.SDRADR4 ));
	Ddim_Print(( "IO_LTM_P1.LTM.LSDROFS.bit.SDROFSR4 = 0x%x\n",  IO_LTM_P1.LTM.LSDROFS.bit.SDROFSR4 ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR3.bit.SDRADR3 = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR3.bit.SDRADR3 ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDROFS.bit.SDROFSR3 = 0x%x\n",  IO_LTM_P2.LTM.LSDROFS.bit.SDROFSR3 ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDRADR4.bit.SDRADR4 = 0x%lx\n", IO_LTM_P2.LTM.LSDRADR4.bit.SDRADR4 ));
	Ddim_Print(( "IO_LTM_P2.LTM.LSDROFS.bit.SDROFSR4 = 0x%x\n",  IO_LTM_P2.LTM.LSDROFS.bit.SDROFSR4 ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_11: "
INT32 CT_Im_Ltm_Ltm_1_11( UCHAR pipe_no )
{
	UINT32						loopcnt;
	T_IM_LTM_LTM_OUTDATA_INFO	ltm_outdata_info[] = {
		[0] = {	// max
			.addr[0] = (VOID*)0xFFFFFFFE,
			.addr[1] = (VOID*)0xFFFFFFFE,
			.addr[2] = (VOID*)0xFFFFFFFE,
			.global_width[0] = 0xFFF8,
			.global_width[1] = 0xFFF8,
			.global_width[2] = 0xFFF8,
		},
		[1] = {	// min
			.addr[0] = (VOID*)0,
			.addr[1] = (VOID*)0,
			.addr[2] = (VOID*)0,
			.global_width[0] = 0,
			.global_width[1] = 0,
			.global_width[2] = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(ltm_outdata_info) / sizeof(ltm_outdata_info[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Set_OutData_Info( pipe_no, &ltm_outdata_info[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.LSDWADR.bit.SDWADR   = 0x%lx\n", IO_LTM_P1.LTM.LSDWADR.bit.SDWADR   ));
		Ddim_Print(( "IO_LTM_P1.LTM.LSDWDEFR.bit.SDWDEFR = 0x%x\n",  IO_LTM_P1.LTM.LSDWDEFR.bit.SDWDEFR ));
		Ddim_Print(( "IO_LTM_P1.LTM.LSDWADG.bit.SDWADG   = 0x%lx\n", IO_LTM_P1.LTM.LSDWADG.bit.SDWADG   ));
		Ddim_Print(( "IO_LTM_P1.LTM.LSDWDEFG.bit.SDWDEFG = 0x%x\n",  IO_LTM_P1.LTM.LSDWDEFG.bit.SDWDEFG ));
		Ddim_Print(( "IO_LTM_P1.LTM.LSDWADB.bit.SDWADB   = 0x%lx\n", IO_LTM_P1.LTM.LSDWADB.bit.SDWADB   ));
		Ddim_Print(( "IO_LTM_P1.LTM.LSDWDEFB.bit.SDWDEFB = 0x%x\n",  IO_LTM_P1.LTM.LSDWDEFB.bit.SDWDEFB ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDWADR.bit.SDWADR   = 0x%lx\n", IO_LTM_P2.LTM.LSDWADR.bit.SDWADR   ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDWDEFR.bit.SDWDEFR = 0x%x\n",  IO_LTM_P2.LTM.LSDWDEFR.bit.SDWDEFR ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDWADG.bit.SDWADG   = 0x%lx\n", IO_LTM_P2.LTM.LSDWADG.bit.SDWADG   ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDWDEFG.bit.SDWDEFG = 0x%x\n",  IO_LTM_P2.LTM.LSDWDEFG.bit.SDWDEFG ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDWADB.bit.SDWADB   = 0x%lx\n", IO_LTM_P2.LTM.LSDWADB.bit.SDWADB   ));
		Ddim_Print(( "IO_LTM_P2.LTM.LSDWDEFB.bit.SDWDEFB = 0x%x\n",  IO_LTM_P2.LTM.LSDWDEFB.bit.SDWDEFB ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_12: "
INT32 CT_Im_Ltm_Ltm_1_12( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(gct_im_ltm_ltm_ctrl_b2r_direct) / sizeof(gct_im_ltm_ltm_ctrl_b2r_direct[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Ctrl_ModeB2RDirect( pipe_no, &gct_im_ltm_ltm_ctrl_b2r_direct[loopcnt] );

		ct_im_ltm_ltm_print_b2r_direct_reg();
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_13: "
INT32 CT_Im_Ltm_Ltm_1_13( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_COMMON ltm_ctrl_common = gct_im_ltm_ltm_ctrl_common_base;
	T_IM_LTM_LTM_INDATA_INFO ltm_in_data = {
		.addr_yp = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP,
		.addr_yn = (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP,
		.top_bit_offset_yp = 0,
		.top_bit_offset_yn = 0,
	};
	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &ltm_in_data );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "IO_LTM_P1.LTM.LHSTA.bit.HSTA = 0x%x\n", IO_LTM_P1.LTM.LHSTA.bit.HSTA ));
	Ddim_Print(( "IO_LTM_P1.LTM.LHPIT.bit.HPIT = 0x%x\n", IO_LTM_P1.LTM.LHPIT.bit.HPIT ));
	Ddim_Print(( "IO_LTM_P1.LTM.LVSTA.bit.VSTA = 0x%x\n", IO_LTM_P1.LTM.LVSTA.bit.VSTA ));
	Ddim_Print(( "IO_LTM_P1.LTM.LVPIT.bit.VPIT = 0x%x\n", IO_LTM_P1.LTM.LVPIT.bit.VPIT ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.LTM.LHSTA.bit.HSTA = 0x%x\n", IO_LTM_P2.LTM.LHSTA.bit.HSTA ));
	Ddim_Print(( "IO_LTM_P2.LTM.LHPIT.bit.HPIT = 0x%x\n", IO_LTM_P2.LTM.LHPIT.bit.HPIT ));
	Ddim_Print(( "IO_LTM_P2.LTM.LVSTA.bit.VSTA = 0x%x\n", IO_LTM_P2.LTM.LVSTA.bit.VSTA ));
	Ddim_Print(( "IO_LTM_P2.LTM.LVPIT.bit.VPIT = 0x%x\n", IO_LTM_P2.LTM.LVPIT.bit.VPIT ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	ltm_ctrl_common.input_rect.img_width /= 2;
	ltm_ctrl_common.input_rect.img_lines /= 2;

	Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.LTM.LHSTA.bit.HSTA = 0x%x\n", IO_LTM_P1.LTM.LHSTA.bit.HSTA ));
	Ddim_Print(( "IO_LTM_P1.LTM.LHPIT.bit.HPIT = 0x%x\n", IO_LTM_P1.LTM.LHPIT.bit.HPIT ));
	Ddim_Print(( "IO_LTM_P1.LTM.LVSTA.bit.VSTA = 0x%x\n", IO_LTM_P1.LTM.LVSTA.bit.VSTA ));
	Ddim_Print(( "IO_LTM_P1.LTM.LVPIT.bit.VPIT = 0x%x\n", IO_LTM_P1.LTM.LVPIT.bit.VPIT ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.LTM.LHSTA.bit.HSTA = 0x%x\n", IO_LTM_P2.LTM.LHSTA.bit.HSTA ));
	Ddim_Print(( "IO_LTM_P2.LTM.LHPIT.bit.HPIT = 0x%x\n", IO_LTM_P2.LTM.LHPIT.bit.HPIT ));
	Ddim_Print(( "IO_LTM_P2.LTM.LVSTA.bit.VSTA = 0x%x\n", IO_LTM_P2.LTM.LVSTA.bit.VSTA ));
	Ddim_Print(( "IO_LTM_P2.LTM.LVPIT.bit.VPIT = 0x%x\n", IO_LTM_P2.LTM.LVPIT.bit.VPIT ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_14: "
INT32 CT_Im_Ltm_Ltm_1_14( UCHAR pipe_no )
{
	UINT32	loopcnt;
	T_IM_LTM_LTM_MAP_RESIZE_PITCH ltm_resize_pitch[] = {
		[0] = {	// max
			.hsta = 0xFFFFFF,
			.hpit = 0x7FFF,
			.vsta = 0xFFFFFF,
			.vpit = 0x7FFF,
		},
		[1] = {	// min
			.hsta = -0x1000000,
			.hpit = 0,
			.vsta = -0x1000000,
			.vpit = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Set_MapImage_Resize_Pitch( pipe_no, &ltm_resize_pitch[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.LHSTA.bit.HSTA = 0x%x\n", IO_LTM_P1.LTM.LHSTA.bit.HSTA ));
		Ddim_Print(( "IO_LTM_P1.LTM.LHPIT.bit.HPIT = 0x%x\n", IO_LTM_P1.LTM.LHPIT.bit.HPIT ));
		Ddim_Print(( "IO_LTM_P1.LTM.LVSTA.bit.VSTA = 0x%x\n", IO_LTM_P1.LTM.LVSTA.bit.VSTA ));
		Ddim_Print(( "IO_LTM_P1.LTM.LVPIT.bit.VPIT = 0x%x\n", IO_LTM_P1.LTM.LVPIT.bit.VPIT ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.LHSTA.bit.HSTA = 0x%x\n", IO_LTM_P2.LTM.LHSTA.bit.HSTA ));
		Ddim_Print(( "IO_LTM_P2.LTM.LHPIT.bit.HPIT = 0x%x\n", IO_LTM_P2.LTM.LHPIT.bit.HPIT ));
		Ddim_Print(( "IO_LTM_P2.LTM.LVSTA.bit.VSTA = 0x%x\n", IO_LTM_P2.LTM.LVSTA.bit.VSTA ));
		Ddim_Print(( "IO_LTM_P2.LTM.LVPIT.bit.VPIT = 0x%x\n", IO_LTM_P2.LTM.LVPIT.bit.VPIT ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_15: "
INT32 CT_Im_Ltm_Ltm_1_15( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Set_FrameStop( pipe_no, loopcnt );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.LFRSTP.bit.FRSTP = %u\n", IO_LTM_P1.LTM.LFRSTP.bit.FRSTP ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.LFRSTP.bit.FRSTP = %u\n", IO_LTM_P2.LTM.LFRSTP.bit.FRSTP ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_16: "
INT32 CT_Im_Ltm_Ltm_1_16( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Set_ParamHold( pipe_no, loopcnt );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.LPARMSK.bit.PARMSK = %u\n", IO_LTM_P1.LTM.LPARMSK.bit.PARMSK ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.LPARMSK.bit.PARMSK = %u\n", IO_LTM_P2.LTM.LPARMSK.bit.PARMSK ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_17: "
INT32 CT_Im_Ltm_Ltm_1_17( UCHAR pipe_no )
{
	BOOL busy_stat = FALSE;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.LTM.LTRG.bit.TRG = 3;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.LTM.LTRG.bit.TRG = 3;
	}
#endif // CO_DEBUG_ON_PC
	Im_LTM_LTM_Get_LtmBusy( pipe_no, &busy_stat );
	Ddim_Print(( "busy_stat = %u\n", busy_stat ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.LTM.LTRG.bit.TRG = 2;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.LTM.LTRG.bit.TRG = 2;
	}
#endif // CO_DEBUG_ON_PC

	busy_stat = FALSE;

	Im_LTM_LTM_Get_LtmBusy( pipe_no, &busy_stat );
	Ddim_Print(( "busy_stat = %u\n", busy_stat ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_18: "
INT32 CT_Im_Ltm_Ltm_1_18( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	Im_LTM_LTM_ContStart( pipe_no );

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "IO_LTM_P1.LTM.LTRG.bit.TRG = %u\n", IO_LTM_P1.LTM.LTRG.bit.TRG ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.LTM.LTRG.bit.TRG = %u\n", IO_LTM_P2.LTM.LTRG.bit.TRG ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_19: "
INT32 CT_Im_Ltm_Ltm_1_19( UCHAR pipe_no )
{
	UINT32							loopcnt;
	T_IM_LTM_LTM_CTRL_HF_STRENGTH	ltm_hf_ctrl[] = {
		[0] = {	// max
			.strength = 0x1000,
		},
		[1] = {	// min
			.strength = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(ltm_hf_ctrl) / sizeof(ltm_hf_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Set_High_Frequency_Strength( pipe_no, &ltm_hf_ctrl[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.NLVTHOF7.bit.NLVTHOF_7 = 0x%x\n", IO_LTM_P1.LTM.NLVTHOF7.bit.NLVTHOF_7 ));
		Ddim_Print(( "IO_LTM_P1.LTM.PLVTHOF7.bit.PLVTHOF_7 = 0x%x\n", IO_LTM_P1.LTM.PLVTHOF7.bit.PLVTHOF_7 ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.NLVTHOF7.bit.NLVTHOF_7 = 0x%x\n", IO_LTM_P2.LTM.NLVTHOF7.bit.NLVTHOF_7 ));
		Ddim_Print(( "IO_LTM_P2.LTM.PLVTHOF7.bit.PLVTHOF_7 = 0x%x\n", IO_LTM_P2.LTM.PLVTHOF7.bit.PLVTHOF_7 ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_20: "
INT32 CT_Im_Ltm_Ltm_1_20( UCHAR pipe_no )
{
	UINT32					loopcnt;
	T_IM_LTM_INTERNAL_PARAM	ltm_internal_param[] = {
		[0] = {	// max
			.yshadow = 0x3FFF,
			.yhighlight = 0x3FFF,
		},
		[1] = {	// min
			.yshadow = 0,
			.yhighlight = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(ltm_internal_param) / sizeof(ltm_internal_param[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Set_Internal_Param( pipe_no, &ltm_internal_param[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.LYSHADOW.bit.YSHADOW       = 0x%x\n", IO_LTM_P1.LTM.LYSHADOW.bit.YSHADOW       ));
		Ddim_Print(( "IO_LTM_P1.LTM.LYHIGHLIGHT.bit.YHIGHLIGHT = 0x%x\n", IO_LTM_P1.LTM.LYHIGHLIGHT.bit.YHIGHLIGHT ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.LYSHADOW.bit.YSHADOW       = 0x%x\n", IO_LTM_P2.LTM.LYSHADOW.bit.YSHADOW       ));
		Ddim_Print(( "IO_LTM_P2.LTM.LYHIGHLIGHT.bit.YHIGHLIGHT = 0x%x\n", IO_LTM_P2.LTM.LYHIGHLIGHT.bit.YHIGHLIGHT ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_21: "
INT32 CT_Im_Ltm_Ltm_1_21( UCHAR pipe_no )
{
	UINT32					loopcnt;
	T_IM_LTM_LTM_CTRL_BLEND	ltm_ctrl_blend[] = {
		[0] = {	// max
			.ratio = 8,
			.offset = {   4096,   4096,   4096,   4096,   4096,   4096,   4096,   4096 },
			.gain   = { 0x3FFF, 0x3FFF, 0x3FFF, 0x3FFF, 0x3FFF, 0x3FFF, 0x3FFF, 0x3FFF },
			.border = {  0xFFF,  0xFFF,  0xFFF,  0xFFF,  0xFFF,  0xFFF,  0xFFF,  0xFFF },
			.mode = 1,
		},
		[1] = {	// min
			.ratio = 0,
			.offset = { 0, 0, 0, 0, 0, 0, 0, 0 },
			.gain   = { -0x4000, -0x4000, -0x4000, -0x4000, -0x4000, -0x4000, -0x4000, -0x4000 },
			.border = { 0, 0, 0, 0, 0, 0, 0, 0 },
			.mode = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(ltm_ctrl_blend) / sizeof(ltm_ctrl_blend[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Ctrl_Blend( pipe_no, &ltm_ctrl_blend[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.BLDYRT.bit.BLDYRT        = 0x%x\n", IO_LTM_P1.LTM.BLDYRT.bit.BLDYRT        ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF1.bit.BLDOF_0 = 0x%x\n", IO_LTM_P1.LTM.BLDOF.BLDOF1.bit.BLDOF_0 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF1.bit.BLDOF_1 = 0x%x\n", IO_LTM_P1.LTM.BLDOF.BLDOF1.bit.BLDOF_1 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF2.bit.BLDOF_2 = 0x%x\n", IO_LTM_P1.LTM.BLDOF.BLDOF2.bit.BLDOF_2 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF2.bit.BLDOF_3 = 0x%x\n", IO_LTM_P1.LTM.BLDOF.BLDOF2.bit.BLDOF_3 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF3.bit.BLDOF_4 = 0x%x\n", IO_LTM_P1.LTM.BLDOF.BLDOF3.bit.BLDOF_4 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF3.bit.BLDOF_5 = 0x%x\n", IO_LTM_P1.LTM.BLDOF.BLDOF3.bit.BLDOF_5 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF4.bit.BLDOF_6 = 0x%x\n", IO_LTM_P1.LTM.BLDOF.BLDOF4.bit.BLDOF_6 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDOF.BLDOF4.bit.BLDOF_7 = 0x%x\n", IO_LTM_P1.LTM.BLDOF.BLDOF4.bit.BLDOF_7 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA1.bit.BLDGA_0 = 0x%x\n", IO_LTM_P1.LTM.BLDGA.BLDGA1.bit.BLDGA_0 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA1.bit.BLDGA_1 = 0x%x\n", IO_LTM_P1.LTM.BLDGA.BLDGA1.bit.BLDGA_1 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA2.bit.BLDGA_2 = 0x%x\n", IO_LTM_P1.LTM.BLDGA.BLDGA2.bit.BLDGA_2 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA2.bit.BLDGA_3 = 0x%x\n", IO_LTM_P1.LTM.BLDGA.BLDGA2.bit.BLDGA_3 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA3.bit.BLDGA_4 = 0x%x\n", IO_LTM_P1.LTM.BLDGA.BLDGA3.bit.BLDGA_4 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA3.bit.BLDGA_5 = 0x%x\n", IO_LTM_P1.LTM.BLDGA.BLDGA3.bit.BLDGA_5 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA4.bit.BLDGA_6 = 0x%x\n", IO_LTM_P1.LTM.BLDGA.BLDGA4.bit.BLDGA_6 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDGA.BLDGA4.bit.BLDGA_7 = 0x%x\n", IO_LTM_P1.LTM.BLDGA.BLDGA4.bit.BLDGA_7 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD1.bit.BLDBD_0 = 0x%x\n", IO_LTM_P1.LTM.BLDBD.BLDBD1.bit.BLDBD_0 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD1.bit.BLDBD_1 = 0x%x\n", IO_LTM_P1.LTM.BLDBD.BLDBD1.bit.BLDBD_1 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD2.bit.BLDBD_2 = 0x%x\n", IO_LTM_P1.LTM.BLDBD.BLDBD2.bit.BLDBD_2 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD2.bit.BLDBD_3 = 0x%x\n", IO_LTM_P1.LTM.BLDBD.BLDBD2.bit.BLDBD_3 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD3.bit.BLDBD_4 = 0x%x\n", IO_LTM_P1.LTM.BLDBD.BLDBD3.bit.BLDBD_4 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD3.bit.BLDBD_5 = 0x%x\n", IO_LTM_P1.LTM.BLDBD.BLDBD3.bit.BLDBD_5 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD4.bit.BLDBD_6 = 0x%x\n", IO_LTM_P1.LTM.BLDBD.BLDBD4.bit.BLDBD_6 ));
		Ddim_Print(( "IO_LTM_P1.LTM.BLDBD.BLDBD4.bit.BLDBD_7 = 0x%x\n", IO_LTM_P1.LTM.BLDBD.BLDBD4.bit.BLDBD_7 ));
		Ddim_Print(( "IO_LTM_P1.LTM.LTMMD.bit.LTMMD          = 0x%x\n", IO_LTM_P1.LTM.LTMMD.bit.LTMMD          ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDYRT.bit.BLDYRT        = 0x%x\n", IO_LTM_P2.LTM.BLDYRT.bit.BLDYRT        ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF1.bit.BLDOF_0 = 0x%x\n", IO_LTM_P2.LTM.BLDOF.BLDOF1.bit.BLDOF_0 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF1.bit.BLDOF_1 = 0x%x\n", IO_LTM_P2.LTM.BLDOF.BLDOF1.bit.BLDOF_1 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF2.bit.BLDOF_2 = 0x%x\n", IO_LTM_P2.LTM.BLDOF.BLDOF2.bit.BLDOF_2 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF2.bit.BLDOF_3 = 0x%x\n", IO_LTM_P2.LTM.BLDOF.BLDOF2.bit.BLDOF_3 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF3.bit.BLDOF_4 = 0x%x\n", IO_LTM_P2.LTM.BLDOF.BLDOF3.bit.BLDOF_4 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF3.bit.BLDOF_5 = 0x%x\n", IO_LTM_P2.LTM.BLDOF.BLDOF3.bit.BLDOF_5 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF4.bit.BLDOF_6 = 0x%x\n", IO_LTM_P2.LTM.BLDOF.BLDOF4.bit.BLDOF_6 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDOF.BLDOF4.bit.BLDOF_7 = 0x%x\n", IO_LTM_P2.LTM.BLDOF.BLDOF4.bit.BLDOF_7 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA1.bit.BLDGA_0 = 0x%x\n", IO_LTM_P2.LTM.BLDGA.BLDGA1.bit.BLDGA_0 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA1.bit.BLDGA_1 = 0x%x\n", IO_LTM_P2.LTM.BLDGA.BLDGA1.bit.BLDGA_1 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA2.bit.BLDGA_2 = 0x%x\n", IO_LTM_P2.LTM.BLDGA.BLDGA2.bit.BLDGA_2 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA2.bit.BLDGA_3 = 0x%x\n", IO_LTM_P2.LTM.BLDGA.BLDGA2.bit.BLDGA_3 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA3.bit.BLDGA_4 = 0x%x\n", IO_LTM_P2.LTM.BLDGA.BLDGA3.bit.BLDGA_4 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA3.bit.BLDGA_5 = 0x%x\n", IO_LTM_P2.LTM.BLDGA.BLDGA3.bit.BLDGA_5 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA4.bit.BLDGA_6 = 0x%x\n", IO_LTM_P2.LTM.BLDGA.BLDGA4.bit.BLDGA_6 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDGA.BLDGA4.bit.BLDGA_7 = 0x%x\n", IO_LTM_P2.LTM.BLDGA.BLDGA4.bit.BLDGA_7 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD1.bit.BLDBD_0 = 0x%x\n", IO_LTM_P2.LTM.BLDBD.BLDBD1.bit.BLDBD_0 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD1.bit.BLDBD_1 = 0x%x\n", IO_LTM_P2.LTM.BLDBD.BLDBD1.bit.BLDBD_1 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD2.bit.BLDBD_2 = 0x%x\n", IO_LTM_P2.LTM.BLDBD.BLDBD2.bit.BLDBD_2 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD2.bit.BLDBD_3 = 0x%x\n", IO_LTM_P2.LTM.BLDBD.BLDBD2.bit.BLDBD_3 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD3.bit.BLDBD_4 = 0x%x\n", IO_LTM_P2.LTM.BLDBD.BLDBD3.bit.BLDBD_4 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD3.bit.BLDBD_5 = 0x%x\n", IO_LTM_P2.LTM.BLDBD.BLDBD3.bit.BLDBD_5 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD4.bit.BLDBD_6 = 0x%x\n", IO_LTM_P2.LTM.BLDBD.BLDBD4.bit.BLDBD_6 ));
		Ddim_Print(( "IO_LTM_P2.LTM.BLDBD.BLDBD4.bit.BLDBD_7 = 0x%x\n", IO_LTM_P2.LTM.BLDBD.BLDBD4.bit.BLDBD_7 ));
		Ddim_Print(( "IO_LTM_P2.LTM.LTMMD.bit.LTMMD          = 0x%x\n", IO_LTM_P2.LTM.LTMMD.bit.LTMMD          ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_22: "
INT32 CT_Im_Ltm_Ltm_1_22( UCHAR pipe_no )
{
	UINT32						loopcnt;
	T_IM_LTM_LTM_CTRL_CHROMA	ltm_ctrl_chroma[] = {
		[0] = {	// max
			.offset = {  4096,  4096,  4096,  4096,  4096,  4096,  4096,  4096 },
			.gain   = { 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF },
			.border = { 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF },
		},
		[1] = {	// min
			.offset = { 0, 0, 0, 0, 0, 0, 0, 0 },
			.gain   = { -0x1000, -0x1000, -0x1000, -0x1000, -0x1000, -0x1000, -0x1000, -0x1000 },
			.border = { 0, 0, 0, 0, 0, 0, 0 },
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(ltm_ctrl_chroma) / sizeof(ltm_ctrl_chroma[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Ctrl_Chroma_Correction_Strength( pipe_no, &ltm_ctrl_chroma[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF1.bit.CGOF_0 = 0x%x\n", IO_LTM_P1.LTM.CGOF.CGOF1.bit.CGOF_0 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF1.bit.CGOF_1 = 0x%x\n", IO_LTM_P1.LTM.CGOF.CGOF1.bit.CGOF_1 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF2.bit.CGOF_2 = 0x%x\n", IO_LTM_P1.LTM.CGOF.CGOF2.bit.CGOF_2 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF2.bit.CGOF_3 = 0x%x\n", IO_LTM_P1.LTM.CGOF.CGOF2.bit.CGOF_3 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF3.bit.CGOF_4 = 0x%x\n", IO_LTM_P1.LTM.CGOF.CGOF3.bit.CGOF_4 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF3.bit.CGOF_5 = 0x%x\n", IO_LTM_P1.LTM.CGOF.CGOF3.bit.CGOF_5 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF4.bit.CGOF_6 = 0x%x\n", IO_LTM_P1.LTM.CGOF.CGOF4.bit.CGOF_6 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGOF.CGOF4.bit.CGOF_7 = 0x%x\n", IO_LTM_P1.LTM.CGOF.CGOF4.bit.CGOF_7 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA1.bit.CGGA_0 = 0x%x\n", IO_LTM_P1.LTM.CGGA.CGGA1.bit.CGGA_0 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA1.bit.CGGA_1 = 0x%x\n", IO_LTM_P1.LTM.CGGA.CGGA1.bit.CGGA_1 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA2.bit.CGGA_2 = 0x%x\n", IO_LTM_P1.LTM.CGGA.CGGA2.bit.CGGA_2 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA2.bit.CGGA_3 = 0x%x\n", IO_LTM_P1.LTM.CGGA.CGGA2.bit.CGGA_3 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA3.bit.CGGA_4 = 0x%x\n", IO_LTM_P1.LTM.CGGA.CGGA3.bit.CGGA_4 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA3.bit.CGGA_5 = 0x%x\n", IO_LTM_P1.LTM.CGGA.CGGA3.bit.CGGA_5 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA4.bit.CGGA_6 = 0x%x\n", IO_LTM_P1.LTM.CGGA.CGGA4.bit.CGGA_6 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGGA.CGGA4.bit.CGGA_7 = 0x%x\n", IO_LTM_P1.LTM.CGGA.CGGA4.bit.CGGA_7 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGBD.CGBD1.bit.CGBD_1 = 0x%x\n", IO_LTM_P1.LTM.CGBD.CGBD1.bit.CGBD_1 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGBD.CGBD2.bit.CGBD_2 = 0x%x\n", IO_LTM_P1.LTM.CGBD.CGBD2.bit.CGBD_2 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGBD.CGBD2.bit.CGBD_3 = 0x%x\n", IO_LTM_P1.LTM.CGBD.CGBD2.bit.CGBD_3 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGBD.CGBD3.bit.CGBD_4 = 0x%x\n", IO_LTM_P1.LTM.CGBD.CGBD3.bit.CGBD_4 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGBD.CGBD3.bit.CGBD_5 = 0x%x\n", IO_LTM_P1.LTM.CGBD.CGBD3.bit.CGBD_5 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGBD.CGBD4.bit.CGBD_6 = 0x%x\n", IO_LTM_P1.LTM.CGBD.CGBD4.bit.CGBD_6 ));
		Ddim_Print(( "IO_LTM_P1.LTM.CGBD.CGBD4.bit.CGBD_7 = 0x%x\n", IO_LTM_P1.LTM.CGBD.CGBD4.bit.CGBD_7 ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF1.bit.CGOF_0 = 0x%x\n", IO_LTM_P2.LTM.CGOF.CGOF1.bit.CGOF_0 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF1.bit.CGOF_1 = 0x%x\n", IO_LTM_P2.LTM.CGOF.CGOF1.bit.CGOF_1 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF2.bit.CGOF_2 = 0x%x\n", IO_LTM_P2.LTM.CGOF.CGOF2.bit.CGOF_2 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF2.bit.CGOF_3 = 0x%x\n", IO_LTM_P2.LTM.CGOF.CGOF2.bit.CGOF_3 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF3.bit.CGOF_4 = 0x%x\n", IO_LTM_P2.LTM.CGOF.CGOF3.bit.CGOF_4 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF3.bit.CGOF_5 = 0x%x\n", IO_LTM_P2.LTM.CGOF.CGOF3.bit.CGOF_5 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF4.bit.CGOF_6 = 0x%x\n", IO_LTM_P2.LTM.CGOF.CGOF4.bit.CGOF_6 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGOF.CGOF4.bit.CGOF_7 = 0x%x\n", IO_LTM_P2.LTM.CGOF.CGOF4.bit.CGOF_7 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA1.bit.CGGA_0 = 0x%x\n", IO_LTM_P2.LTM.CGGA.CGGA1.bit.CGGA_0 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA1.bit.CGGA_1 = 0x%x\n", IO_LTM_P2.LTM.CGGA.CGGA1.bit.CGGA_1 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA2.bit.CGGA_2 = 0x%x\n", IO_LTM_P2.LTM.CGGA.CGGA2.bit.CGGA_2 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA2.bit.CGGA_3 = 0x%x\n", IO_LTM_P2.LTM.CGGA.CGGA2.bit.CGGA_3 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA3.bit.CGGA_4 = 0x%x\n", IO_LTM_P2.LTM.CGGA.CGGA3.bit.CGGA_4 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA3.bit.CGGA_5 = 0x%x\n", IO_LTM_P2.LTM.CGGA.CGGA3.bit.CGGA_5 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA4.bit.CGGA_6 = 0x%x\n", IO_LTM_P2.LTM.CGGA.CGGA4.bit.CGGA_6 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGGA.CGGA4.bit.CGGA_7 = 0x%x\n", IO_LTM_P2.LTM.CGGA.CGGA4.bit.CGGA_7 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGBD.CGBD1.bit.CGBD_1 = 0x%x\n", IO_LTM_P2.LTM.CGBD.CGBD1.bit.CGBD_1 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGBD.CGBD2.bit.CGBD_2 = 0x%x\n", IO_LTM_P2.LTM.CGBD.CGBD2.bit.CGBD_2 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGBD.CGBD2.bit.CGBD_3 = 0x%x\n", IO_LTM_P2.LTM.CGBD.CGBD2.bit.CGBD_3 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGBD.CGBD3.bit.CGBD_4 = 0x%x\n", IO_LTM_P2.LTM.CGBD.CGBD3.bit.CGBD_4 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGBD.CGBD3.bit.CGBD_5 = 0x%x\n", IO_LTM_P2.LTM.CGBD.CGBD3.bit.CGBD_5 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGBD.CGBD4.bit.CGBD_6 = 0x%x\n", IO_LTM_P2.LTM.CGBD.CGBD4.bit.CGBD_6 ));
		Ddim_Print(( "IO_LTM_P2.LTM.CGBD.CGBD4.bit.CGBD_7 = 0x%x\n", IO_LTM_P2.LTM.CGBD.CGBD4.bit.CGBD_7 ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_23: "
INT32 CT_Im_Ltm_Ltm_1_23( UCHAR pipe_no )
{
	UINT32				loopcnt;
	T_IM_LTM_CTRL_YLOG	ltm_ctrl_ylog[] = {
		[0] = {	// max
			.use_tbl = 1,
			.end_point0_tbl0 = 0x3FFF,
			.end_point0_tbl1 = 0x3FFF,
			.end_point1_tbl0 = 0x3FFF,
			.end_point1_tbl1 = 0x3FFF,
		},
		[1] = {	// min
			.use_tbl = 0,
			.end_point0_tbl0 = 0,
			.end_point0_tbl1 = 0,
			.end_point1_tbl0 = 0,
			.end_point1_tbl1 = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(ltm_ctrl_ylog) / sizeof(ltm_ctrl_ylog[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &ltm_ctrl_ylog[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.LTM.LYLCTL.bit.YLOGSL      = 0x%x\n", IO_LTM_P1.LTM.LYLCTL.bit.YLOGSL      ));
		Ddim_Print(( "IO_LTM_P1.LTM.LYLOG0EP0.bit.YLOG0EP0 = 0x%x\n", IO_LTM_P1.LTM.LYLOG0EP0.bit.YLOG0EP0 ));
		Ddim_Print(( "IO_LTM_P1.LTM.LYLOG0EP1.bit.YLOG0EP1 = 0x%x\n", IO_LTM_P1.LTM.LYLOG0EP1.bit.YLOG0EP1 ));
		Ddim_Print(( "IO_LTM_P1.LTM.LYLOG1EP0.bit.YLOG1EP0 = 0x%x\n", IO_LTM_P1.LTM.LYLOG1EP0.bit.YLOG1EP0 ));
		Ddim_Print(( "IO_LTM_P1.LTM.LYLOG1EP1.bit.YLOG1EP1 = 0x%x\n", IO_LTM_P1.LTM.LYLOG1EP1.bit.YLOG1EP1 ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.LTM.LYLCTL.bit.YLOGSL      = 0x%x\n", IO_LTM_P2.LTM.LYLCTL.bit.YLOGSL      ));
		Ddim_Print(( "IO_LTM_P2.LTM.LYLOG0EP0.bit.YLOG0EP0 = 0x%x\n", IO_LTM_P2.LTM.LYLOG0EP0.bit.YLOG0EP0 ));
		Ddim_Print(( "IO_LTM_P2.LTM.LYLOG0EP1.bit.YLOG0EP1 = 0x%x\n", IO_LTM_P2.LTM.LYLOG0EP1.bit.YLOG0EP1 ));
		Ddim_Print(( "IO_LTM_P2.LTM.LYLOG1EP0.bit.YLOG1EP0 = 0x%x\n", IO_LTM_P2.LTM.LYLOG1EP0.bit.YLOG1EP0 ));
		Ddim_Print(( "IO_LTM_P2.LTM.LYLOG1EP1.bit.YLOG1EP1 = 0x%x\n", IO_LTM_P2.LTM.LYLOG1EP1.bit.YLOG1EP1 ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_24: "
INT32 CT_Im_Ltm_Ltm_1_24( UCHAR pipe_no )
{
	UCHAR act_tbl_no;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.LTM.LYLCTL.bit.YLOGSTA = 1;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.LTM.LYLCTL.bit.YLOGSTA = 1;
	}
#endif // CO_DEBUG_ON_PC
	Im_LTM_LTM_Get_Active_Ylog_Tbl_No( pipe_no, &act_tbl_no );
	Ddim_Print(( "act_tbl_no = %u\n", act_tbl_no ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.LTM.LYLCTL.bit.YLOGSTA = 0;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.LTM.LYLCTL.bit.YLOGSTA = 0;
	}
#endif // CO_DEBUG_ON_PC
	Im_LTM_LTM_Get_Active_Ylog_Tbl_No( pipe_no, &act_tbl_no );
	Ddim_Print(( "act_tbl_no = %u\n", act_tbl_no ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_1_25: "
INT32 CT_Im_Ltm_Ltm_1_25( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );

	ct_im_ltm_ltm_print_table( pipe_no, 0 );

	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_01, &gct_im_ltm_ltm_ylog_tbl[0] );

	ct_im_ltm_ltm_print_table( pipe_no, 1 );

	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );

	ct_im_ltm_ltm_print_table( pipe_no, 2 );

	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_11, &gct_im_ltm_ltm_ylog_tbl[0] );

	ct_im_ltm_ltm_print_table( pipe_no, 3 );

	return D_DDIM_OK;
}

static INT32 ct_im_ltm_ltm_run_1( UCHAR pipe_no, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-1-1
		case 1:
			return CT_Im_Ltm_Ltm_1_1( pipe_no );

		// Test-1-1-2
		case 2:
			return CT_Im_Ltm_Ltm_1_2( pipe_no );

		// Test-1-1-3
		case 3:
			return CT_Im_Ltm_Ltm_1_3( pipe_no );

		// Test-1-1-4
		case 4:
			return CT_Im_Ltm_Ltm_1_4( pipe_no );

		// Test-1-1-5
		case 5:
			return CT_Im_Ltm_Ltm_1_5( pipe_no );

		// Test-1-1-6
		case 6:
			return CT_Im_Ltm_Ltm_1_6( pipe_no );

		// Test-1-1-7
		case 7:
			return CT_Im_Ltm_Ltm_1_7( pipe_no );

		// Test-1-1-8
		case 8:
			return CT_Im_Ltm_Ltm_1_8( pipe_no );

		// Test-1-1-9
		case 9:
			return CT_Im_Ltm_Ltm_1_9( pipe_no );

		// Test-1-1-10
		case 10:
			return CT_Im_Ltm_Ltm_1_10( pipe_no );

		// Test-1-1-11
		case 11:
			return CT_Im_Ltm_Ltm_1_11( pipe_no );

		// Test-1-1-12
		case 12:
			return CT_Im_Ltm_Ltm_1_12( pipe_no );

		// Test-1-1-13
		case 13:
			return CT_Im_Ltm_Ltm_1_13( pipe_no );

		// Test-1-1-14
		case 14:
			return CT_Im_Ltm_Ltm_1_14( pipe_no );

		// Test-1-1-15
		case 15:
			return CT_Im_Ltm_Ltm_1_15( pipe_no );

		// Test-1-1-16
		case 16:
			return CT_Im_Ltm_Ltm_1_16( pipe_no );

		// Test-1-1-17
		case 17:
			return CT_Im_Ltm_Ltm_1_17( pipe_no );

		// Test-1-1-18
		case 18:
			return CT_Im_Ltm_Ltm_1_18( pipe_no );

		// Test-1-1-19
		case 19:
			return CT_Im_Ltm_Ltm_1_19( pipe_no );

		// Test-1-1-20
		case 20:
			return CT_Im_Ltm_Ltm_1_20( pipe_no );

		// Test-1-1-21
		case 21:
			return CT_Im_Ltm_Ltm_1_21( pipe_no );

		// Test-1-1-22
		case 22:
			return CT_Im_Ltm_Ltm_1_22( pipe_no );

		// Test-1-1-23
		case 23:
			return CT_Im_Ltm_Ltm_1_23( pipe_no );

		// Test-1-1-24
		case 24:
			return CT_Im_Ltm_Ltm_1_24( pipe_no );

		// Test-1-1-25
		case 25:
			return CT_Im_Ltm_Ltm_1_25( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}



#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_1: "
INT32 CT_Im_Ltm_Ltm_2_1( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Ctrl_Axi( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM ctrl axi error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_2: "
INT32 CT_Im_Ltm_Ltm_2_2( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Get_AxiReadStat( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM get axi read stat error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_3: "
INT32 CT_Im_Ltm_Ltm_2_3( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Get_AxiWriteStat( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM get axi write stat error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_4: "
INT32 CT_Im_Ltm_Ltm_2_4( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Get_AxiCtrlParam( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM get axi ctrl param error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_5: "
INT32 CT_Im_Ltm_Ltm_2_5( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Set_AxiCtrlParam( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM set axi ctrl param error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_6: "
INT32 CT_Im_Ltm_Ltm_2_6( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM set ctrl common error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_7: "
INT32 CT_Im_Ltm_Ltm_2_7( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM set ctrl sdram input error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_8: "
INT32 CT_Im_Ltm_Ltm_2_8( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "TLM set inaddr rgb error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_9: "
INT32 CT_Im_Ltm_Ltm_2_9( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM set indata error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_10: "
INT32 CT_Im_Ltm_Ltm_2_10( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM set outdata error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_11: "
INT32 CT_Im_Ltm_Ltm_2_11( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Ctrl_ModeB2RDirect( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM ctrl b2r direct error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_12: "
INT32 CT_Im_Ltm_Ltm_2_12( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM set resize pitch error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_13: "
INT32 CT_Im_Ltm_Ltm_2_13( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Get_LtmBusy( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM get ltm busy error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_14: "
INT32 CT_Im_Ltm_Ltm_2_14( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Set_High_Frequency_Strength( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM set hf strength error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_15: "
INT32 CT_Im_Ltm_Ltm_2_15( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Set_Internal_Param( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM set internal param error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_16: "
INT32 CT_Im_Ltm_Ltm_2_16( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Ctrl_Blend( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM ctrl blend error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_17: "
INT32 CT_Im_Ltm_Ltm_2_17( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Ctrl_Chroma_Correction_Strength( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM ctrl chroma correction strength error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_18: "
INT32 CT_Im_Ltm_Ltm_2_18( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM ctrl ylog tbl error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_19: "
INT32 CT_Im_Ltm_Ltm_2_19( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Get_Active_Ylog_Tbl_No( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM get active ylog tbl no error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_20: "
INT32 CT_Im_Ltm_Ltm_2_20( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, 0, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM set ylog tbl error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_2_21: "
INT32 CT_Im_Ltm_Ltm_2_21( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;
	DDIM_USER_FLGPTN flgptn;
	DDIM_USER_FLGPTN waiptn;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	waiptn = 0x11110000;
	ercd = Im_LTM_LTM_WaitEnd( &flgptn, waiptn, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "LTM wait end error = 0x%x\n", ercd ));
	}

	return ercd;
}

static INT32 ct_im_ltm_ltm_run_2( UCHAR pipe_no, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-2-1
		case 1:
			return CT_Im_Ltm_Ltm_2_1( pipe_no );

		// Test-1-2-2
		case 2:
			return CT_Im_Ltm_Ltm_2_2( pipe_no );

		// Test-1-2-3
		case 3:
			return CT_Im_Ltm_Ltm_2_3( pipe_no );

		// Test-1-2-4
		case 4:
			return CT_Im_Ltm_Ltm_2_4( pipe_no );

		// Test-1-2-5
		case 5:
			return CT_Im_Ltm_Ltm_2_5( pipe_no );

		// Test-1-2-6
		case 6:
			return CT_Im_Ltm_Ltm_2_6( pipe_no );

		// Test-1-2-7
		case 7:
			return CT_Im_Ltm_Ltm_2_7( pipe_no );

		// Test-1-2-8
		case 8:
			return CT_Im_Ltm_Ltm_2_8( pipe_no );

		// Test-1-2-9
		case 9:
			return CT_Im_Ltm_Ltm_2_9( pipe_no );

		// Test-1-2-10
		case 10:
			return CT_Im_Ltm_Ltm_2_10( pipe_no );

		// Test-1-2-11
		case 11:
			return CT_Im_Ltm_Ltm_2_11( pipe_no );

		// Test-1-2-12
		case 12:
			return CT_Im_Ltm_Ltm_2_12( pipe_no );

		// Test-1-2-13
		case 13:
			return CT_Im_Ltm_Ltm_2_13( pipe_no );

		// Test-1-2-14
		case 14:
			return CT_Im_Ltm_Ltm_2_14( pipe_no );

		// Test-1-2-15
		case 15:
			return CT_Im_Ltm_Ltm_2_15( pipe_no );

		// Test-1-2-16
		case 16:
			return CT_Im_Ltm_Ltm_2_16( pipe_no );

		// Test-1-2-17
		case 17:
			return CT_Im_Ltm_Ltm_2_17( pipe_no );

		// Test-1-2-18
		case 18:
			return CT_Im_Ltm_Ltm_2_18( pipe_no );

		// Test-1-2-19
		case 19:
			return CT_Im_Ltm_Ltm_2_19( pipe_no );

		// Test-1-2-20
		case 20:
			return CT_Im_Ltm_Ltm_2_20( pipe_no );

		// Test-1-2-21
		case 21:
			return CT_Im_Ltm_Ltm_2_21( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}



#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_1: "
INT32 CT_Im_Ltm_Ltm_3_1( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_SW_Reset( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_SW_Reset( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_2: "
INT32 CT_Im_Ltm_Ltm_3_2( UCHAR pipe_no )
{
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_3: "
INT32 CT_Im_Ltm_Ltm_3_3( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_COMMON ltm_ctrl_common = gct_im_ltm_ltm_ctrl_common_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ltm_ctrl_common.in_data_type_yp = D_IM_LTM_DTYP_PACK12;
	ltm_ctrl_common.in_data_type_yn = D_IM_LTM_DTYP_PACK12;
	ltm_ctrl_common.global_width_yp = D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH * 3 / 2;
	ltm_ctrl_common.global_width_yn = D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH * 3 / 2;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_4: "
INT32 CT_Im_Ltm_Ltm_3_4( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_COMMON ltm_ctrl_common = gct_im_ltm_ltm_ctrl_common_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ltm_ctrl_common.global_width_yp = ( D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH / 2 ) * 2;
	ltm_ctrl_common.global_width_yn = ( D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH / 2 ) * 2;
	ltm_ctrl_common.input_rect.img_top = 0;
	ltm_ctrl_common.input_rect.img_left = 0;
	ltm_ctrl_common.input_rect.img_width = ( D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH / 2 );
	ltm_ctrl_common.input_rect.img_lines = ( D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_LINES / 2 );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_5: "
INT32 CT_Im_Ltm_Ltm_3_5( UCHAR pipe_no )
{
	T_IM_LTM_LTM_MAP_RESIZE_PITCH resize_pitch;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	resize_pitch.hsta = -0x2000;
	resize_pitch.hpit = 64;
	resize_pitch.vsta = -0x2000;
	resize_pitch.vpit = 64;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch( pipe_no, &resize_pitch );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch( pipe_no, &resize_pitch );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_6: "
INT32 CT_Im_Ltm_Ltm_3_6( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_COMMON ltm_ctrl_common = gct_im_ltm_ltm_ctrl_common_base;
	T_IM_LTM_LTM_CTRL_SDRAM_INPUT ltm_ctrl_sdram_in = gct_im_ltm_ltm_ctrl_sdram_in_base;
	T_IM_LTM_LTM_OUTDATA_INFO ltm_outdata_info = gct_im_ltm_ltm_outdata_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ltm_ctrl_common.input_rect.img_top = ( D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_LINES - ( D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_LINES / 2 )) / 2;
	ltm_ctrl_common.input_rect.img_left = ( D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH - ( D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH / 2 )) / 2;
	ltm_ctrl_common.input_rect.img_width = D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH - ( D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH / 2 );
	ltm_ctrl_common.input_rect.img_lines = D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_LINES - ( D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_LINES / 2 );
	ltm_ctrl_sdram_in.input_rect.img_top = ( D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_LINES - ( D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_LINES / 2 )) / 2;
	ltm_ctrl_sdram_in.input_rect.img_left = ( D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_WIDTH - ( D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_WIDTH / 2 )) / 2;
	ltm_ctrl_sdram_in.input_rect.img_width = D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_WIDTH - ( D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_WIDTH / 2 );
	ltm_ctrl_sdram_in.input_rect.img_lines = D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_LINES - ( D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_LINES / 2 );
	ltm_outdata_info.addr[1] = ltm_outdata_info.addr[0] + D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP * D_IM_LTM_LTM_DST_IMG_PIXS_LINES / 4;
	ltm_outdata_info.addr[2] = ltm_outdata_info.addr[1] + D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP * D_IM_LTM_LTM_DST_IMG_PIXS_LINES / 4;
	ltm_outdata_info.global_width[0] = D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP / 2;
	ltm_outdata_info.global_width[1] = D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP / 2;
	ltm_outdata_info.global_width[2] = D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP / 2;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &ltm_ctrl_sdram_in );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &ltm_outdata_info );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &ltm_ctrl_sdram_in );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &ltm_outdata_info );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_7: "
INT32 CT_Im_Ltm_Ltm_3_7( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_HF_STRENGTH hf_ctrl;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	hf_ctrl.strength = 4096;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_High_Frequency_Strength( pipe_no, &hf_ctrl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Set_High_Frequency_Strength( pipe_no, &hf_ctrl );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_8: "
INT32 CT_Im_Ltm_Ltm_3_8( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_HF_STRENGTH hf_ctrl;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	hf_ctrl.strength = 0;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_High_Frequency_Strength( pipe_no, &hf_ctrl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Set_High_Frequency_Strength( pipe_no, &hf_ctrl );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_9: "
INT32 CT_Im_Ltm_Ltm_3_9( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_BLEND blend_ctrl = gct_im_ltm_ltm_ctrl_blend;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	blend_ctrl.ratio = 0;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Blend( pipe_no, &blend_ctrl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Ctrl_Blend( pipe_no, &blend_ctrl );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_10: "
INT32 CT_Im_Ltm_Ltm_3_10( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_BLEND blend_ctrl = gct_im_ltm_ltm_ctrl_blend;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	blend_ctrl.ratio = 8;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Blend( pipe_no, &blend_ctrl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Ctrl_Blend( pipe_no, &blend_ctrl );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_11: "
INT32 CT_Im_Ltm_Ltm_3_11( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_BLEND blend_ctrl = gct_im_ltm_ltm_ctrl_blend;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	blend_ctrl.mode = 0;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Blend( pipe_no, &blend_ctrl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Ctrl_Blend( pipe_no, &blend_ctrl );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_12: "
INT32 CT_Im_Ltm_Ltm_3_12( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_BLEND blend_ctrl = gct_im_ltm_ltm_ctrl_blend;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	blend_ctrl.mode = 1;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Blend( pipe_no, &blend_ctrl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Ctrl_Blend( pipe_no, &blend_ctrl );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_13: "
INT32 CT_Im_Ltm_Ltm_3_13( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_COMMON ltm_ctrl_common = gct_im_ltm_ltm_ctrl_common_base;
	T_IM_LTM_LTM_OUTDATA_INFO ltm_outdata_info = gct_im_ltm_ltm_outdata_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ltm_ctrl_common.out_data_type = D_IM_LTM_DTYP_PACK12;
	ltm_outdata_info.global_width[0] = D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_P;
	ltm_outdata_info.global_width[1] = D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_P;
	ltm_outdata_info.global_width[2] = D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_P;
	ltm_outdata_info.addr[1] = (VOID*)(D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP + D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_P * D_IM_LTM_LTM_DST_IMG_PIXS_LINES);
	ltm_outdata_info.addr[2] = (VOID*)(D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP + D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_P * D_IM_LTM_LTM_DST_IMG_PIXS_LINES * 2);

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &ltm_outdata_info );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &ltm_ctrl_common );
	Im_LTM_LTM_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_ltm_ctrl_sdram_in_base );
	Im_LTM_LTM_Set_InAddr_Info_Rgb( pipe_no, &gct_im_ltm_ltm_rgb_in_addr_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &ltm_outdata_info );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Start( pipe_no );
#endif

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_14: "
INT32 CT_Im_Ltm_Ltm_3_14( UCHAR pipe_no )
{
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ct_im_ltm_ltm_set_b2r( pipe_no );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeB2RDirect( pipe_no, &gct_im_ltm_ltm_ctrl_b2r_direct_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Im_LTM_LTM_Ctrl_ModeB2RDirect( pipe_no, &gct_im_ltm_ltm_ctrl_b2r_direct_base );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Im_LTM_LTM_Start( pipe_no );
#endif

	ct_im_ltm_ltm_start_b2r( pipe_no );

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Ltm_3_15: "
INT32 CT_Im_Ltm_Ltm_3_15( UCHAR pipe_no )
{
	T_IM_LTM_LTM_CTRL_B2R_DIRECT ltm_ctrl_direct = gct_im_ltm_ltm_ctrl_b2r_direct_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ltm_ctrl_direct.bypass = 1;

	ct_im_ltm_ltm_set_b2r( pipe_no );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_ModeB2RDirect( pipe_no, &ltm_ctrl_direct );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_Ctrl_Common( pipe_no, &gct_im_ltm_ltm_ctrl_common_base );
	Im_LTM_LTM_Ctrl_ModeB2RDirect( pipe_no, &ltm_ctrl_direct );
	Im_LTM_LTM_Set_InData_Info( pipe_no, &gct_im_ltm_ltm_indata_base );
	Im_LTM_LTM_Set_OutData_Info( pipe_no, &gct_im_ltm_ltm_outdata_base );
	Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( pipe_no );
	Im_LTM_LTM_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_ltm_ctrl_ylog_tbl );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_ltm_ylog_tbl[0] );
	Im_LTM_LTM_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_ltm_ylog_tbl[0] );
#endif

	ct_im_ltm_ltm_set_r2y( pipe_no );
	ct_im_ltm_ltm_start_r2y( pipe_no );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

#else
	Im_LTM_LTM_Start( pipe_no );
#endif

	ct_im_ltm_ltm_start_b2r( pipe_no );

	if( ct_im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM1_INT_FLG_LTM_END;
	}
	if( ct_im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_LTM2_INT_FLG_LTM_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_LTM_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_LTM_WaitEnd( NULL, flgptn, 30 );
	Im_LTM_LTM_Stop( pipe_no );
#endif

	ct_im_ltm_ltm_print_all_reg();

	return D_DDIM_OK;
}

static INT32 ct_im_ltm_ltm_run_3( UCHAR pipe_no, const UINT32 ct_no_2nd )
{
	Im_LTM_LTM_Ctrl_Blend( pipe_no, &gct_im_ltm_ltm_ctrl_blend );

	switch( ct_no_2nd ) {
		// Test-1-3-1
		case 1:
			return CT_Im_Ltm_Ltm_3_1( pipe_no );

		// Test-1-3-2
		case 2:
			return CT_Im_Ltm_Ltm_3_2( pipe_no );

		// Test-1-3-3
		case 3:
			return CT_Im_Ltm_Ltm_3_3( pipe_no );

		// Test-1-3-4
		case 4:
			return CT_Im_Ltm_Ltm_3_4( pipe_no );

		// Test-1-3-5
		case 5:
			return CT_Im_Ltm_Ltm_3_5( pipe_no );

		// Test-1-3-6
		case 6:
			return CT_Im_Ltm_Ltm_3_6( pipe_no );

		// Test-1-3-7
		case 7:
			return CT_Im_Ltm_Ltm_3_7( pipe_no );

		// Test-1-3-8
		case 8:
			return CT_Im_Ltm_Ltm_3_8( pipe_no );

		// Test-1-3-9
		case 9:
			return CT_Im_Ltm_Ltm_3_9( pipe_no );

		// Test-1-3-10
		case 10:
			return CT_Im_Ltm_Ltm_3_10( pipe_no );

		// Test-1-3-11
		case 11:
			return CT_Im_Ltm_Ltm_3_11( pipe_no );

		// Test-1-3-12
		case 12:
			return CT_Im_Ltm_Ltm_3_12( pipe_no );

		// Test-1-3-13
		case 13:
			return CT_Im_Ltm_Ltm_3_13( pipe_no );

		// Test-1-3-14
		case 14:
			return CT_Im_Ltm_Ltm_3_14( pipe_no );

		// Test-1-3-15
		case 15:
			return CT_Im_Ltm_Ltm_3_15( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}


VOID CT_Im_LTM_LTM_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
#ifdef CO_MSG_PRINT_ON
	INT32 result = D_DDIM_INPUT_PARAM_ERROR;
#endif
	static BOOL is_init = 0;
	UCHAR loop_cnt;

	Ddim_Print(( "CT_Im_LTM_LTM_Run( %u, %u, %u ) Begin\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	if( is_init == 0 ) {
#ifdef CO_DEBUG_ON_PC
		memset( (VOID*)&IO_LTM_P1.LTM, '\0', sizeof(IO_LTM_P1.LTM) );
		memset( (VOID*)&IO_LTM_P2.LTM, '\0', sizeof(IO_LTM_P2.LTM) );
		IO_LTM_P1.LTM.LTRG.word = 2;
		IO_LTM_P2.LTM.LTRG.word = 2;
#endif // CO_DEBUG_ON_PC
		if( !((ct_idx_1st == 1) && (ct_idx_2nd == 1)) ) {
			Ddim_Print(( "Im_LTM_LTM_Init() Begin\n" ));
			Im_LTM_LTM_Init( D_IM_LTM_PIPE12 );
			Ddim_Print(( "Im_LTM_LTM_Init() End\n" ));
			is_init = 1;
		}
	}

	if( ct_idx_1st != 3 ){
		for( loop_cnt = 0; loop_cnt <= 1; loop_cnt++ ){
			switch( ct_idx_1st ) {
				case 1:
					D_IM_LTM_RESULT( ct_im_ltm_ltm_run_1( loop_cnt, ct_idx_2nd ) );
					break;

				case 2:
					D_IM_LTM_RESULT( ct_im_ltm_ltm_run_2( loop_cnt, ct_idx_2nd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}

#ifdef CO_MSG_PRINT_ON
			Ddim_Print(( "result = 0x%x\n", result ));
#endif
			Ddim_Print(( "CT_Im_LTM_LTM_Run( %u, %u, %u) End\n", ct_idx_1st, ct_idx_2nd, loop_cnt ));
		}
	}
	else{
		D_IM_LTM_RESULT( ct_im_ltm_ltm_run_3( ct_idx_3rd, ct_idx_2nd ) );
#ifdef CO_MSG_PRINT_ON
		Ddim_Print(( "result = 0x%x\n", result ));
#endif
		Ddim_Print(( "CT_Im_LTM_LTM_Run( %u, %u, %u) End\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));
	}

	return;
}


// for Debug console interface
VOID CT_Im_LTM_LTM_Main( INT32 argc, CHAR** argv )
{
	if( argc < 3 ) {
		Ddim_Print(( "CT_Im_LTM_LTM_Main: parameter error.\n" ));
		return;
	}

	CT_Im_LTM_LTM_Run( (UINT32)atoi((const char*)argv[1]),
					   (UINT32)atoi((const char*)argv[2]),
					   (UINT32)atoi((const char*)argv[3]) );
}

