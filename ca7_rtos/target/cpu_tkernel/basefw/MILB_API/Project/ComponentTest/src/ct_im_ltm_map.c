/**
 * @file		ct_im_ltm_map.c
 * @brief		LTM(MAP block) ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copybright 2016 Socionext Inc.</I></B>
 */

#include "ct_im_ltm.h"

#include "im_ltm.h"

#include "driver_common.h"
#include "ddim_user_custom.h"
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
#endif

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

static VOID ct_im_ltm_map_handler( UINT32* result, UINT32 user_param );

// Max/Min table
static T_IM_LTM_MAP_CTRL_COMMON gct_im_ltm_map_ctrl_common[] = {
	[0] = {	// max
		.line_intr = 0x3FF,
		.wch_burst_length_yp = D_IM_LTM_BRST_256,
		.wch_burst_length_yn = D_IM_LTM_BRST_256,
		.mask_write_yp = D_IM_LTM_MASK_WRITE_DISABLE,
		.mask_write_yn = D_IM_LTM_MASK_WRITE_DISABLE,
		.write_select_yp = D_IM_LTM_PORT_SWITCH,
		.write_select_yn = D_IM_LTM_PORT_SWITCH,
		.data_type_yp = D_IM_LTM_DTYP_UNPACK12,
		.data_type_yn = D_IM_LTM_DTYP_UNPACK12,
	},
	[1] = {	// min
		.line_intr = 0,
		.wch_burst_length_yp = D_IM_LTM_BRST_512,
		.wch_burst_length_yn = D_IM_LTM_BRST_512,
		.mask_write_yp = D_IM_LTM_MASK_WRITE_ENABLE,
		.mask_write_yn = D_IM_LTM_MASK_WRITE_ENABLE,
		.write_select_yp = D_IM_LTM_PORT_DEFAULT,
		.write_select_yn = D_IM_LTM_PORT_DEFAULT,
		.data_type_yp = D_IM_LTM_DTYP_PACK12,
		.data_type_yn = D_IM_LTM_DTYP_PACK12,
	},
};

static T_IM_LTM_MAP_CTRL_SDRAM_INPUT gct_im_ltm_map_ctrl_sdram_in[] = {
	[0] = {	// max
		.burst_length = D_IM_LTM_BRST_256,
		.input_global = 0xFFF0,
		.input_rect = {
			.img_top = 846,
			.img_left = 1015,
			.img_width = 854,
			.img_lines = 1023,
		},
		.data_type = D_IM_LTM_DTYP_UNPACK12,
	},
	[1] = {	// min
		.burst_length = D_IM_LTM_BRST_512,
		.input_global = 0,
		.input_rect = {
			.img_top = 0,
			.img_left = 0,
			.img_width = 8,
			.img_lines = 8,
		},
		.data_type = D_IM_LTM_DTYP_PACK12,
	},
};

// TestBase table
static T_IM_LTM_MAP_CTRL_COMMON gct_im_ltm_map_ctrl_common_base = {
	.line_intr = 0,
	.wch_burst_length_yp = D_IM_LTM_BRST_512,
	.wch_burst_length_yn = D_IM_LTM_BRST_512,
	.mask_write_yp = D_IM_LTM_MASK_WRITE_ENABLE,
	.mask_write_yn = D_IM_LTM_MASK_WRITE_ENABLE,
	.write_select_yp = D_IM_LTM_PORT_DEFAULT,
	.write_select_yn = D_IM_LTM_PORT_DEFAULT,
	.data_type_yp = D_IM_LTM_DTYP_UNPACK12,
	.data_type_yn = D_IM_LTM_DTYP_UNPACK12,
	.map_user_handler = ct_im_ltm_map_handler,
	.user_param = 0,
};

static T_IM_LTM_MAP_CTRL_SDRAM_INPUT gct_im_ltm_map_ctrl_sdram_in_base = {
	.burst_length = D_IM_LTM_BRST_512,
	.input_global = D_IM_LTM_MAP_SRC_IMG_GLOBAL_WIDTH_UP,
	.input_rect = {
		.img_top = 0,
		.img_left = 0,
		.img_width = D_IM_LTM_MAP_SRC_IMG_PIXS_WIDTH,
		.img_lines = D_IM_LTM_MAP_SRC_IMG_PIXS_LINES,
	},
	.data_type = D_IM_LTM_DTYP_UNPACK12,
};

static T_IM_LTM_MAP_INDATA_INFO gct_im_ltm_map_indata_base = {
	.addr = (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP,
	.top_bit_offset = 0,
};

static T_IM_LTM_MAP_OUTDATA_INFO gct_im_ltm_map_outdata_base = {
	.addr_yp = (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP,
	.global_width_yp = D_IM_LTM_MAP_DST_IMG_GLOBAL_WIDTH_UP,
	.addr_yn = (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP,
	.global_width_yn = D_IM_LTM_MAP_DST_IMG_GLOBAL_WIDTH_UP,
};

static T_IM_LTM_CTRL_YLOG gct_im_ltm_map_ctrl_ylog_tbl = {
	.use_tbl = 0,
	.end_point0_tbl0 = 0x3FFF,
	.end_point0_tbl1 = 0x3FFF,
	.end_point1_tbl0 = 0x3FFF,
	.end_point1_tbl1 = 0x3FFF,
};

const USHORT gct_im_ltm_map_ylog_tbl[D_IM_LTM_TABLE_MAX_YLOG] = {
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
extern VOID Im_LTM_MAP_Print_Status( VOID );
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define ct_im_ltm_map_check_target_pipe_no_1(a)		(((a)+1) & (D_IM_LTM_PIPE1+1))
#define ct_im_ltm_map_check_target_pipe_no_2(a)		(((a)+1) & (D_IM_LTM_PIPE2+1))


static VOID ct_im_ltm_map_handler( UINT32* result, UINT32 user_param )
{
	if( ((*result) & D_IM_LTM_MAP1_INT_STATE_MAP_END) != 0 ) {
		Ddim_Print(( "LTM MAP1 end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_MAP1_INT_STATE_LINE_END) != 0 ) {
		Ddim_Print(( "LTM MAP1 line end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_MAP1_INT_STATE_AXR_ERR) != 0 ) {
		Ddim_Print(( "LTM MAP1 AXR error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_MAP1_INT_STATE_AXW_ERR) != 0 ) {
		Ddim_Print(( "LTM MAP1 AXW error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_MAP1_INT_STATE_YLOG_ERR) != 0 ) {
		Ddim_Print(( "LTM MAP1 YLOG access error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_MAP2_INT_STATE_MAP_END) != 0 ) {
		Ddim_Print(( "LTM MAP2 end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_MAP2_INT_STATE_LINE_END) != 0 ) {
		Ddim_Print(( "LTM MAP2 line end. 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_MAP2_INT_STATE_AXR_ERR) != 0 ) {
		Ddim_Print(( "LTM MAP2 AXR error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_MAP2_INT_STATE_AXW_ERR) != 0 ) {
		Ddim_Print(( "LTM MAP2 AXW error! 0x%x\n", user_param ));
	}
	if( ((*result) & D_IM_LTM_MAP2_INT_STATE_YLOG_ERR) != 0 ) {
		Ddim_Print(( "LTM MAP2 YLOG access error! 0x%x\n", user_param ));
	}
}

static VOID ct_im_ltm_map_rbk_int_handler( UINT32* result, UINT32 user_param )
{
	Ddim_Print(( "RBK Int %x, %x\n", *result, user_param ));
}

static VOID ct_im_ltm_map_set_rbk( UCHAR pipe_no )
{
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl_common;
	T_IM_LTM_RBK_CTRL_SDRAM_INPUT rbk_ctrl_sdram_in;
	T_IM_LTM_INADDR_INFO rbk_in_addr;

	memset( &rbk_ctrl_common,   '\0', sizeof(T_IM_LTM_RBK_CTRL_COMMON)      );
	memset( &rbk_ctrl_sdram_in, '\0', sizeof(T_IM_LTM_RBK_CTRL_SDRAM_INPUT) );
	memset( &rbk_in_addr,       '\0', sizeof(T_IM_LTM_INADDR_INFO)          );

	rbk_ctrl_common.out_select = D_IM_LTM_RBK_OUT_MAP_DIRECT;
	rbk_ctrl_common.rbk_user_handler = ct_im_ltm_map_rbk_int_handler;
	rbk_ctrl_common.user_param = 0;

	rbk_ctrl_sdram_in.input_global = D_IM_LTM_RBK_SRC_IMG_RGB_GLOBAL_WIDTH;
	rbk_ctrl_sdram_in.input_rect.img_width = D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH;
	rbk_ctrl_sdram_in.input_rect.img_lines = D_IM_LTM_RBK_SRC_IMG_PIXS_LINES;
	rbk_ctrl_sdram_in.shift_enable = D_IM_LTM_ENABLE_ON;

	rbk_in_addr.addr_r = (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP;
	rbk_in_addr.addr_g = (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_G_ADDR_TOP;
	rbk_in_addr.addr_b = (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_B_ADDR_TOP;

	Im_LTM_RBK_Ctrl_Common( pipe_no, &rbk_ctrl_common );
	Im_LTM_RBK_Ctrl_ModeSDRAMInput( pipe_no, &rbk_ctrl_sdram_in );
	Im_LTM_RBK_Set_InAddr_Info( pipe_no, &rbk_in_addr );
}

static VOID ct_im_ltm_map_start_rbk( UCHAR pipe_no )
{
	Im_LTM_RBK_Start( pipe_no );
}

static VOID ct_im_ltm_map_print_ctrl_common_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.MAP.MLINTLV.bit.LINTLV     = 0x%x\n", IO_LTM_P1.MAP.MLINTLV.bit.LINTLV     ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYP  = 0x%x\n", IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYP  ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYN  = 0x%x\n", IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYN  ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYP = 0x%x\n", IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYP ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYN = 0x%x\n", IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYN ));
	Ddim_Print(( "IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL1  = 0x%x\n", IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL1  ));
	Ddim_Print(( "IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL2  = 0x%x\n", IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL2  ));
	Ddim_Print(( "IO_LTM_P1.MAP.MIOCTL.bit.W0DTYP      = 0x%x\n", IO_LTM_P1.MAP.MIOCTL.bit.W0DTYP      ));
	Ddim_Print(( "IO_LTM_P1.MAP.MIOCTL.bit.W1DTYP      = 0x%x\n", IO_LTM_P1.MAP.MIOCTL.bit.W1DTYP      ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.MAP.MLINTLV.bit.LINTLV     = 0x%x\n", IO_LTM_P2.MAP.MLINTLV.bit.LINTLV     ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYP  = 0x%x\n", IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYP  ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYN  = 0x%x\n", IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYN  ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYP = 0x%x\n", IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYP ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYN = 0x%x\n", IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYN ));
	Ddim_Print(( "IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL1  = 0x%x\n", IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL1  ));
	Ddim_Print(( "IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL2  = 0x%x\n", IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL2  ));
	Ddim_Print(( "IO_LTM_P2.MAP.MIOCTL.bit.W0DTYP      = 0x%x\n", IO_LTM_P2.MAP.MIOCTL.bit.W0DTYP      ));
	Ddim_Print(( "IO_LTM_P2.MAP.MIOCTL.bit.W1DTYP      = 0x%x\n", IO_LTM_P2.MAP.MIOCTL.bit.W1DTYP      ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_map_print_ctrl_sdram_input_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTR = 0x%x\n", IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTR ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDRDEF.bit.SDRDEF  = 0x%x\n", IO_LTM_P1.MAP.MSDRDEF.bit.SDRDEF  ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDRHSZ.bit.SDRHSZ  = 0x%x\n", IO_LTM_P1.MAP.MSDRHSZ.bit.SDRHSZ  ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDRVSZ.bit.SDRVSZ  = 0x%x\n", IO_LTM_P1.MAP.MSDRVSZ.bit.SDRVSZ  ));
	Ddim_Print(( "IO_LTM_P1.MAP.MIOCTL.bit.INSEL    = 0x%x\n", IO_LTM_P1.MAP.MIOCTL.bit.INSEL    ));
	Ddim_Print(( "IO_LTM_P1.MAP.MIOCTL.bit.R1DTYP   = 0x%x\n", IO_LTM_P1.MAP.MIOCTL.bit.R1DTYP   ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTR = 0x%x\n", IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTR ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDRDEF.bit.SDRDEF  = 0x%x\n", IO_LTM_P2.MAP.MSDRDEF.bit.SDRDEF  ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDRHSZ.bit.SDRHSZ  = 0x%x\n", IO_LTM_P2.MAP.MSDRHSZ.bit.SDRHSZ  ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDRVSZ.bit.SDRVSZ  = 0x%x\n", IO_LTM_P2.MAP.MSDRVSZ.bit.SDRVSZ  ));
	Ddim_Print(( "IO_LTM_P2.MAP.MIOCTL.bit.INSEL    = 0x%x\n", IO_LTM_P2.MAP.MIOCTL.bit.INSEL    ));
	Ddim_Print(( "IO_LTM_P2.MAP.MIOCTL.bit.R1DTYP   = 0x%x\n", IO_LTM_P2.MAP.MIOCTL.bit.R1DTYP   ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_map_print_rbk_direct_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.MAP.MIOCTL.bit.INSEL = 0x%x\n", IO_LTM_P1.MAP.MIOCTL.bit.INSEL ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.MAP.MIOCTL.bit.INSEL = 0x%x\n", IO_LTM_P2.MAP.MIOCTL.bit.INSEL ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_map_print_table( UCHAR pipe_no, UCHAR tbl_no )
{
	USHORT loopcnt;

	Im_LTM_On_Hclk( D_IM_LTM_PIPE12 );
	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		switch( tbl_no ){
			case 0:
			default:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P1.MAP_SRAM.MYLOG0TBL0[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P1.MAP_SRAM.MYLOG0TBL0.hword[loopcnt] ));
				}
				break;

			case 1:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P1.MAP_SRAM.MYLOG0TBL1[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P1.MAP_SRAM.MYLOG0TBL1.hword[loopcnt] ));
				}
				break;

			case 2:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P1.MAP_SRAM.MYLOG1TBL0[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P1.MAP_SRAM.MYLOG1TBL0.hword[loopcnt] ));
				}
				break;

			case 3:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P1.MAP_SRAM.MYLOG1TBL1[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P1.MAP_SRAM.MYLOG1TBL1.hword[loopcnt] ));
				}
				break;
		}
	}

	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		switch( tbl_no ){
			case 0:
			default:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P2.MAP_SRAM.MYLOG0TBL0[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P2.MAP_SRAM.MYLOG0TBL0.hword[loopcnt] ));
				}
				break;

			case 1:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P2.MAP_SRAM.MYLOG0TBL1[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P2.MAP_SRAM.MYLOG0TBL1.hword[loopcnt] ));
				}
				break;

			case 2:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P2.MAP_SRAM.MYLOG1TBL0[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P2.MAP_SRAM.MYLOG1TBL0.hword[loopcnt] ));
				}
				break;

			case 3:
				Ddim_Print(( "\n" ));
				for( loopcnt = 0; loopcnt < D_IM_LTM_TABLE_MAX_YLOG; loopcnt++ ){
					Ddim_Print(( "IO_LTM_TBL_P2.MAP_SRAM.MYLOG1TBL1[%d] = 0x%x\n", loopcnt, IO_LTM_TBL_P2.MAP_SRAM.MYLOG1TBL1.hword[loopcnt] ));
				}
				break;
		}
	}
	Im_LTM_Off_Hclk( D_IM_LTM_PIPE12 );
}

static VOID ct_im_ltm_map_print_all_reg( VOID )
{
	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAPSR      = 0x%lx\n", IO_LTM_P1.MAP.MAPSR.word      ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSRAMEN    = 0x%lx\n", IO_LTM_P1.MAP.MSRAMEN.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.MTRG       = 0x%lx\n", IO_LTM_P1.MAP.MTRG.word       ));
	Ddim_Print(( "IO_LTM_P1.MAP.MFRSTP     = 0x%lx\n", IO_LTM_P1.MAP.MFRSTP.word     ));
	Ddim_Print(( "IO_LTM_P1.MAP.MPARMSK    = 0x%lx\n", IO_LTM_P1.MAP.MPARMSK.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.MINT0      = 0x%lx\n", IO_LTM_P1.MAP.MINT0.word      ));
	Ddim_Print(( "IO_LTM_P1.MAP.MINT1      = 0x%lx\n", IO_LTM_P1.MAP.MINT1.word      ));
	Ddim_Print(( "IO_LTM_P1.MAP.MLINTLV    = 0x%lx\n", IO_LTM_P1.MAP.MLINTLV.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.AXICHSEL   = 0x%lx\n", IO_LTM_P1.MAP.AXICHSEL.word   ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAXICTLR   = 0x%lx\n", IO_LTM_P1.MAP.MAXICTLR.word   ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAXIRERR   = 0x%lx\n", IO_LTM_P1.MAP.MAXIRERR.word   ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAXICTLW   = 0x%lx\n", IO_LTM_P1.MAP.MAXICTLW.word   ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAXIWERR   = 0x%lx\n", IO_LTM_P1.MAP.MAXIWERR.word   ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAXIBSIZ   = 0x%lx\n", IO_LTM_P1.MAP.MAXIBSIZ.word   ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDRHSZ    = 0x%lx\n", IO_LTM_P1.MAP.MSDRHSZ.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDRDEF    = 0x%lx\n", IO_LTM_P1.MAP.MSDRDEF.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDRVSZ    = 0x%lx\n", IO_LTM_P1.MAP.MSDRVSZ.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDRAD     = 0x%lx\n", IO_LTM_P1.MAP.MSDRAD.word     ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDROFS    = 0x%lx\n", IO_LTM_P1.MAP.MSDROFS.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDWADW0   = 0x%lx\n", IO_LTM_P1.MAP.MSDWADW0.word   ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDWDEFW0  = 0x%lx\n", IO_LTM_P1.MAP.MSDWDEFW0.word  ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDWADW1   = 0x%lx\n", IO_LTM_P1.MAP.MSDWADW1.word   ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDWDEFW1  = 0x%lx\n", IO_LTM_P1.MAP.MSDWDEFW1.word  ));
	Ddim_Print(( "IO_LTM_P1.MAP.MIOCTL     = 0x%lx\n", IO_LTM_P1.MAP.MIOCTL.word     ));
	Ddim_Print(( "IO_LTM_P1.MAP.GAINR      = 0x%lx\n", IO_LTM_P1.MAP.GAINR.word      ));
	Ddim_Print(( "IO_LTM_P1.MAP.GAING      = 0x%lx\n", IO_LTM_P1.MAP.GAING.word      ));
	Ddim_Print(( "IO_LTM_P1.MAP.GAINB      = 0x%lx\n", IO_LTM_P1.MAP.GAINB.word      ));
	Ddim_Print(( "IO_LTM_P1.MAP.GAINCPR    = 0x%lx\n", IO_LTM_P1.MAP.GAINCPR.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.GAINCPG    = 0x%lx\n", IO_LTM_P1.MAP.GAINCPG.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.GAINCPB    = 0x%lx\n", IO_LTM_P1.MAP.GAINCPB.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.PEPS0M     = 0x%lx\n", IO_LTM_P1.MAP.PEPS0M.word     ));
	Ddim_Print(( "IO_LTM_P1.MAP.PEPS0OF    = 0x%lx\n", IO_LTM_P1.MAP.PEPS0OF.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.PEPS1M     = 0x%lx\n", IO_LTM_P1.MAP.PEPS1M.word     ));
	Ddim_Print(( "IO_LTM_P1.MAP.PEPS1OF    = 0x%lx\n", IO_LTM_P1.MAP.PEPS1OF.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.PEPS2M     = 0x%lx\n", IO_LTM_P1.MAP.PEPS2M.word     ));
	Ddim_Print(( "IO_LTM_P1.MAP.PEPS2OF    = 0x%lx\n", IO_LTM_P1.MAP.PEPS2OF.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.NEPS0M     = 0x%lx\n", IO_LTM_P1.MAP.NEPS0M.word     ));
	Ddim_Print(( "IO_LTM_P1.MAP.NEPS0OF    = 0x%lx\n", IO_LTM_P1.MAP.NEPS0OF.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.NEPS1M     = 0x%lx\n", IO_LTM_P1.MAP.NEPS1M.word     ));
	Ddim_Print(( "IO_LTM_P1.MAP.NEPS1OF    = 0x%lx\n", IO_LTM_P1.MAP.NEPS1OF.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.NEPS2M     = 0x%lx\n", IO_LTM_P1.MAP.NEPS2M.word     ));
	Ddim_Print(( "IO_LTM_P1.MAP.NEPS2OF    = 0x%lx\n", IO_LTM_P1.MAP.NEPS2OF.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.YSHADOW    = 0x%lx\n", IO_LTM_P1.MAP.YSHADOW.word    ));
	Ddim_Print(( "IO_LTM_P1.MAP.YHIGHLIGHT = 0x%lx\n", IO_LTM_P1.MAP.YHIGHLIGHT.word ));
	Ddim_Print(( "IO_LTM_P1.MAP.MYLCTL     = 0x%lx\n", IO_LTM_P1.MAP.MYLCTL.word     ));
	Ddim_Print(( "IO_LTM_P1.MAP.MYLOG0EP0  = 0x%lx\n", IO_LTM_P1.MAP.MYLOG0EP0.word  ));
	Ddim_Print(( "IO_LTM_P1.MAP.MYLOG0EP1  = 0x%lx\n", IO_LTM_P1.MAP.MYLOG0EP1.word  ));
	Ddim_Print(( "IO_LTM_P1.MAP.MYLOG1EP0  = 0x%lx\n", IO_LTM_P1.MAP.MYLOG1EP0.word  ));
	Ddim_Print(( "IO_LTM_P1.MAP.MYLOG1EP1  = 0x%lx\n", IO_LTM_P1.MAP.MYLOG1EP1.word  ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAPSR      = 0x%lx\n", IO_LTM_P2.MAP.MAPSR.word      ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSRAMEN    = 0x%lx\n", IO_LTM_P2.MAP.MSRAMEN.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.MTRG       = 0x%lx\n", IO_LTM_P2.MAP.MTRG.word       ));
	Ddim_Print(( "IO_LTM_P2.MAP.MFRSTP     = 0x%lx\n", IO_LTM_P2.MAP.MFRSTP.word     ));
	Ddim_Print(( "IO_LTM_P2.MAP.MPARMSK    = 0x%lx\n", IO_LTM_P2.MAP.MPARMSK.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.MINT0      = 0x%lx\n", IO_LTM_P2.MAP.MINT0.word      ));
	Ddim_Print(( "IO_LTM_P2.MAP.MINT1      = 0x%lx\n", IO_LTM_P2.MAP.MINT1.word      ));
	Ddim_Print(( "IO_LTM_P2.MAP.MLINTLV    = 0x%lx\n", IO_LTM_P2.MAP.MLINTLV.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.AXICHSEL   = 0x%lx\n", IO_LTM_P2.MAP.AXICHSEL.word   ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAXICTLR   = 0x%lx\n", IO_LTM_P2.MAP.MAXICTLR.word   ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAXIRERR   = 0x%lx\n", IO_LTM_P2.MAP.MAXIRERR.word   ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAXICTLW   = 0x%lx\n", IO_LTM_P2.MAP.MAXICTLW.word   ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAXIWERR   = 0x%lx\n", IO_LTM_P2.MAP.MAXIWERR.word   ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAXIBSIZ   = 0x%lx\n", IO_LTM_P2.MAP.MAXIBSIZ.word   ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDRHSZ    = 0x%lx\n", IO_LTM_P2.MAP.MSDRHSZ.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDRDEF    = 0x%lx\n", IO_LTM_P2.MAP.MSDRDEF.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDRVSZ    = 0x%lx\n", IO_LTM_P2.MAP.MSDRVSZ.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDRAD     = 0x%lx\n", IO_LTM_P2.MAP.MSDRAD.word     ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDROFS    = 0x%lx\n", IO_LTM_P2.MAP.MSDROFS.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDWADW0   = 0x%lx\n", IO_LTM_P2.MAP.MSDWADW0.word   ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDWDEFW0  = 0x%lx\n", IO_LTM_P2.MAP.MSDWDEFW0.word  ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDWADW1   = 0x%lx\n", IO_LTM_P2.MAP.MSDWADW1.word   ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDWDEFW1  = 0x%lx\n", IO_LTM_P2.MAP.MSDWDEFW1.word  ));
	Ddim_Print(( "IO_LTM_P2.MAP.MIOCTL     = 0x%lx\n", IO_LTM_P2.MAP.MIOCTL.word     ));
	Ddim_Print(( "IO_LTM_P2.MAP.GAINR      = 0x%lx\n", IO_LTM_P2.MAP.GAINR.word      ));
	Ddim_Print(( "IO_LTM_P2.MAP.GAING      = 0x%lx\n", IO_LTM_P2.MAP.GAING.word      ));
	Ddim_Print(( "IO_LTM_P2.MAP.GAINB      = 0x%lx\n", IO_LTM_P2.MAP.GAINB.word      ));
	Ddim_Print(( "IO_LTM_P2.MAP.GAINCPR    = 0x%lx\n", IO_LTM_P2.MAP.GAINCPR.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.GAINCPG    = 0x%lx\n", IO_LTM_P2.MAP.GAINCPG.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.GAINCPB    = 0x%lx\n", IO_LTM_P2.MAP.GAINCPB.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.PEPS0M     = 0x%lx\n", IO_LTM_P2.MAP.PEPS0M.word     ));
	Ddim_Print(( "IO_LTM_P2.MAP.PEPS0OF    = 0x%lx\n", IO_LTM_P2.MAP.PEPS0OF.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.PEPS1M     = 0x%lx\n", IO_LTM_P2.MAP.PEPS1M.word     ));
	Ddim_Print(( "IO_LTM_P2.MAP.PEPS1OF    = 0x%lx\n", IO_LTM_P2.MAP.PEPS1OF.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.PEPS2M     = 0x%lx\n", IO_LTM_P2.MAP.PEPS2M.word     ));
	Ddim_Print(( "IO_LTM_P2.MAP.PEPS2OF    = 0x%lx\n", IO_LTM_P2.MAP.PEPS2OF.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.NEPS0M     = 0x%lx\n", IO_LTM_P2.MAP.NEPS0M.word     ));
	Ddim_Print(( "IO_LTM_P2.MAP.NEPS0OF    = 0x%lx\n", IO_LTM_P2.MAP.NEPS0OF.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.NEPS1M     = 0x%lx\n", IO_LTM_P2.MAP.NEPS1M.word     ));
	Ddim_Print(( "IO_LTM_P2.MAP.NEPS1OF    = 0x%lx\n", IO_LTM_P2.MAP.NEPS1OF.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.NEPS2M     = 0x%lx\n", IO_LTM_P2.MAP.NEPS2M.word     ));
	Ddim_Print(( "IO_LTM_P2.MAP.NEPS2OF    = 0x%lx\n", IO_LTM_P2.MAP.NEPS2OF.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.YSHADOW    = 0x%lx\n", IO_LTM_P2.MAP.YSHADOW.word    ));
	Ddim_Print(( "IO_LTM_P2.MAP.YHIGHLIGHT = 0x%lx\n", IO_LTM_P2.MAP.YHIGHLIGHT.word ));
	Ddim_Print(( "IO_LTM_P2.MAP.MYLCTL     = 0x%lx\n", IO_LTM_P2.MAP.MYLCTL.word     ));
	Ddim_Print(( "IO_LTM_P2.MAP.MYLOG0EP0  = 0x%lx\n", IO_LTM_P2.MAP.MYLOG0EP0.word  ));
	Ddim_Print(( "IO_LTM_P2.MAP.MYLOG0EP1  = 0x%lx\n", IO_LTM_P2.MAP.MYLOG0EP1.word  ));
	Ddim_Print(( "IO_LTM_P2.MAP.MYLOG1EP0  = 0x%lx\n", IO_LTM_P2.MAP.MYLOG1EP0.word  ));
	Ddim_Print(( "IO_LTM_P2.MAP.MYLOG1EP1  = 0x%lx\n", IO_LTM_P2.MAP.MYLOG1EP1.word  ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_1: "
INT32 CT_Im_Ltm_Map_1_1( UCHAR pipe_no )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Init( pipe_no );

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P1.MAP.MAPSR   = 0x%lx\n", IO_LTM_P1.MAP.MAPSR.word ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSRAMEN = 0x%lx\n", IO_LTM_P1.MAP.MSRAMEN.word ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.MAP.MAPSR   = 0x%lx\n", IO_LTM_P2.MAP.MAPSR.word ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSRAMEN = 0x%lx\n", IO_LTM_P2.MAP.MSRAMEN.word ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_2: "
INT32 CT_Im_Ltm_Map_1_2( UCHAR pipe_no )
{
	UINT32					loopcnt;
	T_IM_LTM_MAP_CTRL_AXI	map_ctrl_axi[] = {
		[0] = {	// max
			.rch.cache_type = 0xF,
			.rch.protect_type = 7,
			.wch[0].cache_type = 0xF,
			.wch[0].protect_type = 7,
			.wch[1].cache_type = 0xF,
			.wch[1].protect_type = 7,
		},
		[1] = {	// min
			.rch.cache_type = 0,
			.rch.protect_type = 0,
			.wch[0].cache_type = 0,
			.wch[0].protect_type = 0,
			.wch[1].cache_type = 0,
			.wch[1].protect_type = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(map_ctrl_axi) / sizeof(map_ctrl_axi[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_MAP_Ctrl_Axi( pipe_no, &map_ctrl_axi[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.MAP.MAXICTLR.bit.ARCACHE    = %u\n", IO_LTM_P1.MAP.MAXICTLR.bit.ARCACHE    ));
		Ddim_Print(( "IO_LTM_P1.MAP.MAXICTLR.bit.ARPROT     = %u\n", IO_LTM_P1.MAP.MAXICTLR.bit.ARPROT     ));
		Ddim_Print(( "IO_LTM_P1.MAP.MAXICTLW.bit.AWCACHE_YP = %u\n", IO_LTM_P1.MAP.MAXICTLW.bit.AWCACHE_YP ));
		Ddim_Print(( "IO_LTM_P1.MAP.MAXICTLW.bit.AWPROT_YP  = %u\n", IO_LTM_P1.MAP.MAXICTLW.bit.AWPROT_YP  ));
		Ddim_Print(( "IO_LTM_P1.MAP.MAXICTLW.bit.AWCACHE_YN = %u\n", IO_LTM_P1.MAP.MAXICTLW.bit.AWCACHE_YN ));
		Ddim_Print(( "IO_LTM_P1.MAP.MAXICTLW.bit.AWPROT_YN  = %u\n", IO_LTM_P1.MAP.MAXICTLW.bit.AWPROT_YN  ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.MAP.MAXICTLR.bit.ARCACHE    = %u\n", IO_LTM_P2.MAP.MAXICTLR.bit.ARCACHE    ));
		Ddim_Print(( "IO_LTM_P2.MAP.MAXICTLR.bit.ARPROT     = %u\n", IO_LTM_P2.MAP.MAXICTLR.bit.ARPROT     ));
		Ddim_Print(( "IO_LTM_P2.MAP.MAXICTLW.bit.AWCACHE_YP = %u\n", IO_LTM_P2.MAP.MAXICTLW.bit.AWCACHE_YP ));
		Ddim_Print(( "IO_LTM_P2.MAP.MAXICTLW.bit.AWPROT_YP  = %u\n", IO_LTM_P2.MAP.MAXICTLW.bit.AWPROT_YP  ));
		Ddim_Print(( "IO_LTM_P2.MAP.MAXICTLW.bit.AWCACHE_YN = %u\n", IO_LTM_P2.MAP.MAXICTLW.bit.AWCACHE_YN ));
		Ddim_Print(( "IO_LTM_P2.MAP.MAXICTLW.bit.AWPROT_YN  = %u\n", IO_LTM_P2.MAP.MAXICTLW.bit.AWPROT_YN  ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_3: "
INT32 CT_Im_Ltm_Map_1_3( UCHAR pipe_no )
{
	T_IM_LTM_MAP_AXI_RCH_STAT map_axi_read_stat;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.MAXIRERR.bit.RRESP = 3;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.MAXIRERR.bit.RRESP = 3;
	}
#endif // CO_DEBUG_ON_PC
	memset( &map_axi_read_stat, '\0', sizeof(map_axi_read_stat) );
	Im_LTM_MAP_Get_AxiReadStat( pipe_no, &map_axi_read_stat );
	Ddim_Print(( "rch_axi_stat = %u\n", map_axi_read_stat.rch_axi_stat ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.MAXIRERR.bit.RRESP = 0;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.MAXIRERR.bit.RRESP = 0;
	}
#endif // CO_DEBUG_ON_PC
	memset( &map_axi_read_stat, '\0', sizeof(map_axi_read_stat) );
	Im_LTM_MAP_Get_AxiReadStat( pipe_no, &map_axi_read_stat );
	Ddim_Print(( "rch_axi_stat = %u\n", map_axi_read_stat.rch_axi_stat ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_4: "
INT32 CT_Im_Ltm_Map_1_4( UCHAR pipe_no )
{
	T_IM_LTM_MAP_AXI_WCH_STAT map_axi_write_stat;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.MAXIWERR.bit.WRESP_YP = 3;
		IO_LTM_P1.MAP.MAXIWERR.bit.WRESP_YN = 3;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.MAXIWERR.bit.WRESP_YP = 3;
		IO_LTM_P2.MAP.MAXIWERR.bit.WRESP_YN = 3;
	}
#endif // CO_DEBUG_ON_PC
	memset( &map_axi_write_stat, '\0', sizeof(map_axi_write_stat) );
	Im_LTM_MAP_Get_AxiWriteStat( pipe_no, &map_axi_write_stat );
	Ddim_Print(( "wch_axi_stat[0] = %u\n", map_axi_write_stat.wch_axi_stat[0] ));
	Ddim_Print(( "wch_axi_stat[1] = %u\n", map_axi_write_stat.wch_axi_stat[1] ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.MAXIWERR.bit.WRESP_YP = 0;
		IO_LTM_P1.MAP.MAXIWERR.bit.WRESP_YN = 0;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.MAXIWERR.bit.WRESP_YP = 0;
		IO_LTM_P2.MAP.MAXIWERR.bit.WRESP_YN = 0;
	}
#endif // CO_DEBUG_ON_PC
	memset( &map_axi_write_stat, '\0', sizeof(map_axi_write_stat) );
	Im_LTM_MAP_Get_AxiWriteStat( pipe_no, &map_axi_write_stat );
	Ddim_Print(( "wch_axi_stat[0] = %u\n", map_axi_write_stat.wch_axi_stat[0] ));
	Ddim_Print(( "wch_axi_stat[1] = %u\n", map_axi_write_stat.wch_axi_stat[1] ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_5: "
INT32 CT_Im_Ltm_Map_1_5( UCHAR pipe_no )
{
	T_IM_LTM_MAP_AXI_CTRL_PARAM map_axi_ctrl_param;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTR    = 1;
		IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYP  = 1;
		IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYN  = 1;
		IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYP = 1;
		IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYN = 1;
		IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL1  = 1;
		IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL2  = 1;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTR    = 1;
		IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYP  = 1;
		IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYN  = 1;
		IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYP = 1;
		IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYN = 1;
		IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL1  = 1;
		IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL2  = 1;
	}

	memset( &map_axi_ctrl_param, '\0', sizeof(map_axi_ctrl_param) );
	Im_LTM_MAP_Get_AxiCtrlParam( pipe_no, &map_axi_ctrl_param );
	Ddim_Print(( "rch_burst_length    = %u\n", map_axi_ctrl_param.rch_burst_length    ));
	Ddim_Print(( "wch_burst_length_yp = %u\n", map_axi_ctrl_param.wch_burst_length_yp ));
	Ddim_Print(( "wch_burst_length_yn = %u\n", map_axi_ctrl_param.wch_burst_length_yn ));
	Ddim_Print(( "mask_write_yp       = %u\n", map_axi_ctrl_param.mask_write_yp       ));
	Ddim_Print(( "mask_write_yn       = %u\n", map_axi_ctrl_param.mask_write_yn       ));
	Ddim_Print(( "write_select_yp     = %u\n", map_axi_ctrl_param.write_select_yp     ));
	Ddim_Print(( "write_select_yn     = %u\n", map_axi_ctrl_param.write_select_yn     ));

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTR    = 0;
		IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYP  = 0;
		IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYN  = 0;
		IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYP = 0;
		IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYN = 0;
		IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL1  = 0;
		IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL2  = 0;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTR    = 0;
		IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYP  = 0;
		IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYN  = 0;
		IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYP = 0;
		IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYN = 0;
		IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL1  = 0;
		IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL2  = 0;
	}

	memset( &map_axi_ctrl_param, '\0', sizeof(map_axi_ctrl_param) );
	Im_LTM_MAP_Get_AxiCtrlParam( pipe_no, &map_axi_ctrl_param );
	Ddim_Print(( "rch_burst_length    = %u\n", map_axi_ctrl_param.rch_burst_length    ));
	Ddim_Print(( "wch_burst_length_yp = %u\n", map_axi_ctrl_param.wch_burst_length_yp ));
	Ddim_Print(( "wch_burst_length_yn = %u\n", map_axi_ctrl_param.wch_burst_length_yn ));
	Ddim_Print(( "mask_write_yp       = %u\n", map_axi_ctrl_param.mask_write_yp       ));
	Ddim_Print(( "mask_write_yn       = %u\n", map_axi_ctrl_param.mask_write_yn       ));
	Ddim_Print(( "write_select_yp     = %u\n", map_axi_ctrl_param.write_select_yp     ));
	Ddim_Print(( "write_select_yn     = %u\n", map_axi_ctrl_param.write_select_yn     ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_6: "
INT32 CT_Im_Ltm_Map_1_6( UCHAR pipe_no )
{
	UINT32						loopcnt;
	T_IM_LTM_MAP_AXI_CTRL_PARAM	map_axi_ctrl_param[] = {
		[0] = {	// max
			.rch_burst_length = 1,
			.wch_burst_length_yp = 1,
			.wch_burst_length_yn = 1,
			.mask_write_yp = 1,
			.mask_write_yn = 1,
			.write_select_yp = 1,
			.write_select_yn = 1,
		},
		[1] = {	// min
			.rch_burst_length = 0,
			.wch_burst_length_yp = 0,
			.wch_burst_length_yn = 0,
			.mask_write_yp = 0,
			.mask_write_yn = 0,
			.write_select_yp = 0,
			.write_select_yn = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(map_axi_ctrl_param) / sizeof(map_axi_ctrl_param[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_MAP_Set_AxiCtrlParam( pipe_no, &map_axi_ctrl_param[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTR    = %u\n", IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTR    ));
		Ddim_Print(( "IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYP  = %u\n", IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYP  ));
		Ddim_Print(( "IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYN  = %u\n", IO_LTM_P1.MAP.MAXIBSIZ.bit.BURSTWYN  ));
		Ddim_Print(( "IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYP = %u\n", IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYP ));
		Ddim_Print(( "IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYN = %u\n", IO_LTM_P1.MAP.MAXIBSIZ.bit.WSTBMSKYN ));
		Ddim_Print(( "IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL1  = %u\n", IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL1  ));
		Ddim_Print(( "IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL2  = %u\n", IO_LTM_P1.MAP.AXICHSEL.bit.AXIWSEL2  ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTR    = %u\n", IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTR    ));
		Ddim_Print(( "IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYP  = %u\n", IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYP  ));
		Ddim_Print(( "IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYN  = %u\n", IO_LTM_P2.MAP.MAXIBSIZ.bit.BURSTWYN  ));
		Ddim_Print(( "IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYP = %u\n", IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYP ));
		Ddim_Print(( "IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYN = %u\n", IO_LTM_P2.MAP.MAXIBSIZ.bit.WSTBMSKYN ));
		Ddim_Print(( "IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL1  = %u\n", IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL1  ));
		Ddim_Print(( "IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL2  = %u\n", IO_LTM_P2.MAP.AXICHSEL.bit.AXIWSEL2  ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_7: "
INT32 CT_Im_Ltm_Map_1_7( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(gct_im_ltm_map_ctrl_common) / sizeof(gct_im_ltm_map_ctrl_common[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common[loopcnt] );

		ct_im_ltm_map_print_ctrl_common_reg();
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_8: "
INT32 CT_Im_Ltm_Map_1_8( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(gct_im_ltm_map_ctrl_sdram_in) / sizeof(gct_im_ltm_map_ctrl_sdram_in[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in[loopcnt] );

		ct_im_ltm_map_print_ctrl_sdram_input_reg();
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_9: "
INT32 CT_Im_Ltm_Map_1_9( UCHAR pipe_no )
{
	UINT32 loopcnt;
	T_IM_LTM_MAP_CTRL_SDRAM_INPUT map_ctrl_sdram_in = gct_im_ltm_map_ctrl_sdram_in_base;
	T_IM_LTM_MAP_INDATA_INFO map_indata[] = {
		[0] = { // max
			.addr = (VOID*)0xFFFFFFFF,
			.top_bit_offset = 1,
		},
		[1] = { // min
			.addr = (VOID*)0,
			.top_bit_offset = 0,
		},
		[2] = { // indvisual
			.addr = (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP,
			.top_bit_offset = 1,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &map_ctrl_sdram_in );

	for( loopcnt = 0; loopcnt < (sizeof(map_indata) / sizeof(map_indata[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_MAP_Set_InData_Info( pipe_no, &map_indata[loopcnt] );

		ct_im_ltm_map_print_ctrl_sdram_input_reg();

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.MAP.MSDRAD.bit.SDRAD   = 0x%lx\n", IO_LTM_P1.MAP.MSDRAD.bit.SDRAD   ));
		Ddim_Print(( "IO_LTM_P1.MAP.MSDROFS.bit.SDROFS = 0x%x\n",  IO_LTM_P1.MAP.MSDROFS.bit.SDROFS ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.MAP.MSDRAD.bit.SDRAD   = 0x%lx\n", IO_LTM_P2.MAP.MSDRAD.bit.SDRAD   ));
		Ddim_Print(( "IO_LTM_P2.MAP.MSDROFS.bit.SDROFS = 0x%x\n",  IO_LTM_P2.MAP.MSDROFS.bit.SDROFS ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	map_ctrl_sdram_in.input_rect.img_top = 100;
	map_ctrl_sdram_in.input_rect.img_left = 200;

	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &map_ctrl_sdram_in );

	Im_LTM_MAP_Set_InData_Info( pipe_no, &map_indata[2] );

	ct_im_ltm_map_print_ctrl_sdram_input_reg();

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "IO_LTM_P1.MAP.MSDRAD.bit.SDRAD   = 0x%lx\n", IO_LTM_P1.MAP.MSDRAD.bit.SDRAD   ));
	Ddim_Print(( "IO_LTM_P1.MAP.MSDROFS.bit.SDROFS = 0x%x\n",  IO_LTM_P1.MAP.MSDROFS.bit.SDROFS ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDRAD.bit.SDRAD   = 0x%lx\n", IO_LTM_P2.MAP.MSDRAD.bit.SDRAD   ));
	Ddim_Print(( "IO_LTM_P2.MAP.MSDROFS.bit.SDROFS = 0x%x\n",  IO_LTM_P2.MAP.MSDROFS.bit.SDROFS ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_10: "
INT32 CT_Im_Ltm_Map_1_10( UCHAR pipe_no )
{
	UINT32						loopcnt;
	T_IM_LTM_MAP_OUTDATA_INFO	map_outdata_info[] = {
		[0] = {	// max
			.addr_yp = (VOID*)0xFFFFFFFF,
			.global_width_yp = 0xFFF8,
			.addr_yn = (VOID*)0xFFFFFFFF,
			.global_width_yn = 0xFFF8,
		},
		[1] = {	// min
			.addr_yp = (VOID*)0,
			.global_width_yp = 0,
			.addr_yn = (VOID*)0,
			.global_width_yn = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(map_outdata_info) / sizeof(map_outdata_info[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_MAP_Set_OutData_Info( pipe_no, &map_outdata_info[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.MAP.MSDWADW0.bit.SDWADW0   = 0x%lx\n", IO_LTM_P1.MAP.MSDWADW0.bit.SDWADW0   ));
		Ddim_Print(( "IO_LTM_P1.MAP.MSDWDEFW0.bit.SDWDEFW0 = 0x%x\n",  IO_LTM_P1.MAP.MSDWDEFW0.bit.SDWDEFW0 ));
		Ddim_Print(( "IO_LTM_P1.MAP.MSDWADW1.bit.SDWADW1   = 0x%lx\n", IO_LTM_P1.MAP.MSDWADW1.bit.SDWADW1   ));
		Ddim_Print(( "IO_LTM_P1.MAP.MSDWDEFW1.bit.SDWDEFW1 = 0x%x\n",  IO_LTM_P1.MAP.MSDWDEFW1.bit.SDWDEFW1 ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.MAP.MSDWADW0.bit.SDWADW0   = 0x%lx\n", IO_LTM_P2.MAP.MSDWADW0.bit.SDWADW0   ));
		Ddim_Print(( "IO_LTM_P2.MAP.MSDWDEFW0.bit.SDWDEFW0 = 0x%x\n",  IO_LTM_P2.MAP.MSDWDEFW0.bit.SDWDEFW0 ));
		Ddim_Print(( "IO_LTM_P2.MAP.MSDWADW1.bit.SDWADW1   = 0x%lx\n", IO_LTM_P2.MAP.MSDWADW1.bit.SDWADW1   ));
		Ddim_Print(( "IO_LTM_P2.MAP.MSDWDEFW1.bit.SDWDEFW1 = 0x%x\n",  IO_LTM_P2.MAP.MSDWDEFW1.bit.SDWDEFW1 ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_11: "
INT32 CT_Im_Ltm_Map_1_11( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	Im_LTM_MAP_Ctrl_ModeRBKDirect( pipe_no );

	ct_im_ltm_map_print_rbk_direct_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_12: "
INT32 CT_Im_Ltm_Map_1_12( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_MAP_Set_FrameStop( pipe_no, loopcnt );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.MAP.MFRSTP.bit.FRSTP = %u\n", IO_LTM_P1.MAP.MFRSTP.bit.FRSTP ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.MAP.MFRSTP.bit.FRSTP = %u\n", IO_LTM_P2.MAP.MFRSTP.bit.FRSTP ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_13: "
INT32 CT_Im_Ltm_Map_1_13( UCHAR pipe_no )
{
	UINT32	loopcnt;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_MAP_Set_ParamHold( pipe_no, loopcnt );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.MAP.MPARMSK.bit.PARMSK = %u\n", IO_LTM_P1.MAP.MPARMSK.bit.PARMSK ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.MAP.MPARMSK.bit.PARMSK = %u\n", IO_LTM_P2.MAP.MPARMSK.bit.PARMSK ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_14: "
INT32 CT_Im_Ltm_Map_1_14( UCHAR pipe_no )
{
	BOOL busy_stat = FALSE;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.MTRG.bit.TRG = 3;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.MTRG.bit.TRG = 3;
	}
#endif // CO_DEBUG_ON_PC
	Im_LTM_MAP_Get_MapBusy( pipe_no, &busy_stat );
	Ddim_Print(( "busy_stat = %u\n", busy_stat ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.MTRG.bit.TRG = 2;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.MTRG.bit.TRG = 2;
	}
#endif // CO_DEBUG_ON_PC

	busy_stat = FALSE;

	Im_LTM_MAP_Get_MapBusy( pipe_no, &busy_stat );
	Ddim_Print(( "busy_stat = %u\n", busy_stat ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_15: "
INT32 CT_Im_Ltm_Map_1_15( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	Im_LTM_MAP_ContStart( pipe_no );

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	Ddim_Print(( "IO_LTM_P1.MAP.MTRG.bit.TRG = %u\n", IO_LTM_P1.MAP.MTRG.bit.TRG ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_LTM_P2.MAP.MTRG.bit.TRG = %u\n", IO_LTM_P2.MAP.MTRG.bit.TRG ));
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_16: "
INT32 CT_Im_Ltm_Map_1_16( UCHAR pipe_no )
{
	UINT32				loopcnt;
	T_IM_LTM_MAP_GAIN	map_wb_param[] = {
		[0] = {	// max
			.gain_r = 0x7FF,
			.gain_g = 0x7FF,
			.gain_b = 0x7FF,
			.clip_r = 0xFFF,
			.clip_g = 0xFFF,
			.clip_b = 0xFFF,
		},
		[1] = {	// min
			.gain_r = 0,
			.gain_g = 0,
			.gain_b = 0,
			.clip_r = 0,
			.clip_g = 0,
			.clip_b = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(map_wb_param) / sizeof(map_wb_param[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_MAP_Set_WB_Gain( pipe_no, &map_wb_param[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.MAP.GAINR.bit.GAINR     = 0x%x\n", IO_LTM_P1.MAP.GAINR.bit.GAINR     ));
		Ddim_Print(( "IO_LTM_P1.MAP.GAING.bit.GAING     = 0x%x\n", IO_LTM_P1.MAP.GAING.bit.GAING     ));
		Ddim_Print(( "IO_LTM_P1.MAP.GAINB.bit.GAINB     = 0x%x\n", IO_LTM_P1.MAP.GAINB.bit.GAINB     ));
		Ddim_Print(( "IO_LTM_P1.MAP.GAINCPR.bit.GAINCPR = 0x%x\n", IO_LTM_P1.MAP.GAINCPR.bit.GAINCPR ));
		Ddim_Print(( "IO_LTM_P1.MAP.GAINCPG.bit.GAINCPG = 0x%x\n", IO_LTM_P1.MAP.GAINCPG.bit.GAINCPG ));
		Ddim_Print(( "IO_LTM_P1.MAP.GAINCPB.bit.GAINCPB = 0x%x\n", IO_LTM_P1.MAP.GAINCPB.bit.GAINCPB ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.MAP.GAINR.bit.GAINR     = 0x%x\n", IO_LTM_P2.MAP.GAINR.bit.GAINR     ));
		Ddim_Print(( "IO_LTM_P2.MAP.GAING.bit.GAING     = 0x%x\n", IO_LTM_P2.MAP.GAING.bit.GAING     ));
		Ddim_Print(( "IO_LTM_P2.MAP.GAINB.bit.GAINB     = 0x%x\n", IO_LTM_P2.MAP.GAINB.bit.GAINB     ));
		Ddim_Print(( "IO_LTM_P2.MAP.GAINCPR.bit.GAINCPR = 0x%x\n", IO_LTM_P2.MAP.GAINCPR.bit.GAINCPR ));
		Ddim_Print(( "IO_LTM_P2.MAP.GAINCPG.bit.GAINCPG = 0x%x\n", IO_LTM_P2.MAP.GAINCPG.bit.GAINCPG ));
		Ddim_Print(( "IO_LTM_P2.MAP.GAINCPB.bit.GAINCPB = 0x%x\n", IO_LTM_P2.MAP.GAINCPB.bit.GAINCPB ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_17: "
INT32 CT_Im_Ltm_Map_1_17( UCHAR pipe_no )
{
	UINT32								loopcnt;
	T_IM_LTM_MAP_CTRL_GENERATION_MODE	map_mode_ctrl[] = {
		[0] = {	// max
			.mode0 = 1,
			.coef0 = 0xFFF,
			.mode1 = 1,
			.coef1 = 0xFFF,
			.mode2 = 1,
			.coef2 = 0xFFF,
		},
		[1] = {	// min
			.mode0 = 0,
			.coef0 = 0,
			.mode1 = 0,
			.coef1 = 0,
			.mode2 = 0,
			.coef2 = 0,
		},
	};

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(map_mode_ctrl) / sizeof(map_mode_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_MAP_Ctrl_Generation_Mode( pipe_no, &map_mode_ctrl[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.MAP.PEPS0M.bit.PEPS0M     = 0x%x\n", IO_LTM_P1.MAP.PEPS0M.bit.PEPS0M     ));
		Ddim_Print(( "IO_LTM_P1.MAP.PEPS0OF.bit.PEPS0OF_3 = 0x%x\n", IO_LTM_P1.MAP.PEPS0OF.bit.PEPS0OF_3 ));
		Ddim_Print(( "IO_LTM_P1.MAP.PEPS1M.bit.PEPS1M     = 0x%x\n", IO_LTM_P1.MAP.PEPS1M.bit.PEPS1M     ));
		Ddim_Print(( "IO_LTM_P1.MAP.PEPS1OF.bit.PEPS1OF_3 = 0x%x\n", IO_LTM_P1.MAP.PEPS1OF.bit.PEPS1OF_3 ));
		Ddim_Print(( "IO_LTM_P1.MAP.PEPS2M.bit.PEPS2M     = 0x%x\n", IO_LTM_P1.MAP.PEPS2M.bit.PEPS2M     ));
		Ddim_Print(( "IO_LTM_P1.MAP.PEPS2OF.bit.PEPS2OF_3 = 0x%x\n", IO_LTM_P1.MAP.PEPS2OF.bit.PEPS2OF_3 ));
		Ddim_Print(( "IO_LTM_P1.MAP.NEPS0M.bit.NEPS0M     = 0x%x\n", IO_LTM_P1.MAP.NEPS0M.bit.NEPS0M     ));
		Ddim_Print(( "IO_LTM_P1.MAP.NEPS0OF.bit.NEPS0OF_3 = 0x%x\n", IO_LTM_P1.MAP.NEPS0OF.bit.NEPS0OF_3 ));
		Ddim_Print(( "IO_LTM_P1.MAP.NEPS1M.bit.NEPS1M     = 0x%x\n", IO_LTM_P1.MAP.NEPS1M.bit.NEPS1M     ));
		Ddim_Print(( "IO_LTM_P1.MAP.NEPS1OF.bit.NEPS1OF_3 = 0x%x\n", IO_LTM_P1.MAP.NEPS1OF.bit.NEPS1OF_3 ));
		Ddim_Print(( "IO_LTM_P1.MAP.NEPS2M.bit.NEPS2M     = 0x%x\n", IO_LTM_P1.MAP.NEPS2M.bit.NEPS2M     ));
		Ddim_Print(( "IO_LTM_P1.MAP.NEPS2OF.bit.NEPS2OF_3 = 0x%x\n", IO_LTM_P1.MAP.NEPS2OF.bit.NEPS2OF_3 ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.MAP.PEPS0M.bit.PEPS0M     = 0x%x\n", IO_LTM_P2.MAP.PEPS0M.bit.PEPS0M     ));
		Ddim_Print(( "IO_LTM_P2.MAP.PEPS0OF.bit.PEPS0OF_3 = 0x%x\n", IO_LTM_P2.MAP.PEPS0OF.bit.PEPS0OF_3 ));
		Ddim_Print(( "IO_LTM_P2.MAP.PEPS1M.bit.PEPS1M     = 0x%x\n", IO_LTM_P2.MAP.PEPS1M.bit.PEPS1M     ));
		Ddim_Print(( "IO_LTM_P2.MAP.PEPS1OF.bit.PEPS1OF_3 = 0x%x\n", IO_LTM_P2.MAP.PEPS1OF.bit.PEPS1OF_3 ));
		Ddim_Print(( "IO_LTM_P2.MAP.PEPS2M.bit.PEPS2M     = 0x%x\n", IO_LTM_P2.MAP.PEPS2M.bit.PEPS2M     ));
		Ddim_Print(( "IO_LTM_P2.MAP.PEPS2OF.bit.PEPS2OF_3 = 0x%x\n", IO_LTM_P2.MAP.PEPS2OF.bit.PEPS2OF_3 ));
		Ddim_Print(( "IO_LTM_P2.MAP.NEPS0M.bit.NEPS0M     = 0x%x\n", IO_LTM_P2.MAP.NEPS0M.bit.NEPS0M     ));
		Ddim_Print(( "IO_LTM_P2.MAP.NEPS0OF.bit.NEPS0OF_3 = 0x%x\n", IO_LTM_P2.MAP.NEPS0OF.bit.NEPS0OF_3 ));
		Ddim_Print(( "IO_LTM_P2.MAP.NEPS1M.bit.NEPS1M     = 0x%x\n", IO_LTM_P2.MAP.NEPS1M.bit.NEPS1M     ));
		Ddim_Print(( "IO_LTM_P2.MAP.NEPS1OF.bit.NEPS1OF_3 = 0x%x\n", IO_LTM_P2.MAP.NEPS1OF.bit.NEPS1OF_3 ));
		Ddim_Print(( "IO_LTM_P2.MAP.NEPS2M.bit.NEPS2M     = 0x%x\n", IO_LTM_P2.MAP.NEPS2M.bit.NEPS2M     ));
		Ddim_Print(( "IO_LTM_P2.MAP.NEPS2OF.bit.NEPS2OF_3 = 0x%x\n", IO_LTM_P2.MAP.NEPS2OF.bit.NEPS2OF_3 ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_18: "
INT32 CT_Im_Ltm_Map_1_18( UCHAR pipe_no )
{
	T_IM_LTM_INTERNAL_PARAM map_internal_param;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.YSHADOW.bit.YSHADOW = 0x3FFF;
		IO_LTM_P1.MAP.YHIGHLIGHT.bit.YHIGHLIGHT = 0x3FFF;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.YSHADOW.bit.YSHADOW = 0x3FFF;
		IO_LTM_P2.MAP.YHIGHLIGHT.bit.YHIGHLIGHT = 0x3FFF;
	}
#endif // CO_DEBUG_ON_PC
	memset( &map_internal_param, '\0', sizeof(map_internal_param) );
	Im_LTM_MAP_Get_Internal_Param( pipe_no, &map_internal_param );
	Ddim_Print(( "yshadow    = 0x%x\n", map_internal_param.yshadow ));
	Ddim_Print(( "yhighlight = 0x%x\n", map_internal_param.yhighlight ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.YSHADOW.bit.YSHADOW = 0;
		IO_LTM_P1.MAP.YHIGHLIGHT.bit.YHIGHLIGHT = 0;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.YSHADOW.bit.YSHADOW = 0;
		IO_LTM_P2.MAP.YHIGHLIGHT.bit.YHIGHLIGHT = 0;
	}
#endif // CO_DEBUG_ON_PC
	memset( &map_internal_param, '\0', sizeof(map_internal_param) );
	Im_LTM_MAP_Get_Internal_Param( pipe_no, &map_internal_param );
	Ddim_Print(( "yshadow    = 0x%x\n", map_internal_param.yshadow ));
	Ddim_Print(( "yhighlight = 0x%x\n", map_internal_param.yhighlight ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_19: "
INT32 CT_Im_Ltm_Map_1_19( UCHAR pipe_no )
{
	UINT32				loopcnt;
	T_IM_LTM_CTRL_YLOG	map_ylog_ctrl[] = {
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

	for( loopcnt = 0; loopcnt < (sizeof(map_ylog_ctrl) / sizeof(map_ylog_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

		Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &map_ylog_ctrl[loopcnt] );

		Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
		Ddim_Print(( "IO_LTM_P1.MAP.MYLCTL.bit.YLOGSL      = 0x%x\n", IO_LTM_P1.MAP.MYLCTL.bit.YLOGSL      ));
		Ddim_Print(( "IO_LTM_P1.MAP.MYLOG0EP0.bit.YLOG0EP0 = 0x%x\n", IO_LTM_P1.MAP.MYLOG0EP0.bit.YLOG0EP0 ));
		Ddim_Print(( "IO_LTM_P1.MAP.MYLOG0EP1.bit.YLOG0EP1 = 0x%x\n", IO_LTM_P1.MAP.MYLOG0EP1.bit.YLOG0EP1 ));
		Ddim_Print(( "IO_LTM_P1.MAP.MYLOG1EP0.bit.YLOG1EP0 = 0x%x\n", IO_LTM_P1.MAP.MYLOG1EP0.bit.YLOG1EP0 ));
		Ddim_Print(( "IO_LTM_P1.MAP.MYLOG1EP1.bit.YLOG1EP1 = 0x%x\n", IO_LTM_P1.MAP.MYLOG1EP1.bit.YLOG1EP1 ));
		Ddim_Print(( "\n" ));
		Ddim_Print(( "IO_LTM_P2.MAP.MYLCTL.bit.YLOGSL      = 0x%x\n", IO_LTM_P2.MAP.MYLCTL.bit.YLOGSL      ));
		Ddim_Print(( "IO_LTM_P2.MAP.MYLOG0EP0.bit.YLOG0EP0 = 0x%x\n", IO_LTM_P2.MAP.MYLOG0EP0.bit.YLOG0EP0 ));
		Ddim_Print(( "IO_LTM_P2.MAP.MYLOG0EP1.bit.YLOG0EP1 = 0x%x\n", IO_LTM_P2.MAP.MYLOG0EP1.bit.YLOG0EP1 ));
		Ddim_Print(( "IO_LTM_P2.MAP.MYLOG1EP0.bit.YLOG1EP0 = 0x%x\n", IO_LTM_P2.MAP.MYLOG1EP0.bit.YLOG1EP0 ));
		Ddim_Print(( "IO_LTM_P2.MAP.MYLOG1EP1.bit.YLOG1EP1 = 0x%x\n", IO_LTM_P2.MAP.MYLOG1EP1.bit.YLOG1EP1 ));
		Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_20: "
INT32 CT_Im_Ltm_Map_1_20( UCHAR pipe_no )
{
	UCHAR act_tbl_no;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.MYLCTL.bit.YLOGSTA = 1;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.MYLCTL.bit.YLOGSTA = 1;
	}
#endif // CO_DEBUG_ON_PC
	Im_LTM_MAP_Get_Active_Ylog_Tbl_No( pipe_no, &act_tbl_no );
	Ddim_Print(( "act_tbl_no = %u\n", act_tbl_no ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		IO_LTM_P1.MAP.MYLCTL.bit.YLOGSTA = 0;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		IO_LTM_P2.MAP.MYLCTL.bit.YLOGSTA = 0;
	}
#endif // CO_DEBUG_ON_PC
	Im_LTM_MAP_Get_Active_Ylog_Tbl_No( pipe_no, &act_tbl_no );
	Ddim_Print(( "act_tbl_no = %u\n", act_tbl_no ));

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_1_21: "
INT32 CT_Im_Ltm_Map_1_21( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );

	ct_im_ltm_map_print_table( pipe_no, 0 );

	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_01, &gct_im_ltm_map_ylog_tbl[0] );

	ct_im_ltm_map_print_table( pipe_no, 1 );

	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );

	ct_im_ltm_map_print_table( pipe_no, 2 );

	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_11, &gct_im_ltm_map_ylog_tbl[0] );

	ct_im_ltm_map_print_table( pipe_no, 3 );

	return D_DDIM_OK;
}

static INT32 ct_im_ltm_map_run_1( UCHAR pipe_no, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-1-1
		case 1:
			return CT_Im_Ltm_Map_1_1( pipe_no );

		// Test-1-1-2
		case 2:
			return CT_Im_Ltm_Map_1_2( pipe_no );

		// Test-1-1-3
		case 3:
			return CT_Im_Ltm_Map_1_3( pipe_no );

		// Test-1-1-4
		case 4:
			return CT_Im_Ltm_Map_1_4( pipe_no );

		// Test-1-1-5
		case 5:
			return CT_Im_Ltm_Map_1_5( pipe_no );

		// Test-1-1-6
		case 6:
			return CT_Im_Ltm_Map_1_6( pipe_no );

		// Test-1-1-7
		case 7:
			return CT_Im_Ltm_Map_1_7( pipe_no );

		// Test-1-1-8
		case 8:
			return CT_Im_Ltm_Map_1_8( pipe_no );

		// Test-1-1-9
		case 9:
			return CT_Im_Ltm_Map_1_9( pipe_no );

		// Test-1-1-10
		case 10:
			return CT_Im_Ltm_Map_1_10( pipe_no );

		// Test-1-1-11
		case 11:
			return CT_Im_Ltm_Map_1_11( pipe_no );

		// Test-1-1-12
		case 12:
			return CT_Im_Ltm_Map_1_12( pipe_no );

		// Test-1-1-13
		case 13:
			return CT_Im_Ltm_Map_1_13( pipe_no );

		// Test-1-1-14
		case 14:
			return CT_Im_Ltm_Map_1_14( pipe_no );

		// Test-1-1-15
		case 15:
			return CT_Im_Ltm_Map_1_15( pipe_no );

		// Test-1-1-16
		case 16:
			return CT_Im_Ltm_Map_1_16( pipe_no );

		// Test-1-1-17
		case 17:
			return CT_Im_Ltm_Map_1_17( pipe_no );

		// Test-1-1-18
		case 18:
			return CT_Im_Ltm_Map_1_18( pipe_no );

		// Test-1-1-19
		case 19:
			return CT_Im_Ltm_Map_1_19( pipe_no );

		// Test-1-1-20
		case 20:
			return CT_Im_Ltm_Map_1_20( pipe_no );

		// Test-1-1-21
		case 21:
			return CT_Im_Ltm_Map_1_21( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}



#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_1: "
INT32 CT_Im_Ltm_Map_2_1( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Ctrl_Axi( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP ctrl axi error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_2: "
INT32 CT_Im_Ltm_Map_2_2( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Get_AxiReadStat( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP get axi read stat error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_3: "
INT32 CT_Im_Ltm_Map_2_3( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Get_AxiWriteStat( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP get axi write stat error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_4: "
INT32 CT_Im_Ltm_Map_2_4( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Get_AxiCtrlParam( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP get axi ctrl param error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_5: "
INT32 CT_Im_Ltm_Map_2_5( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Set_AxiCtrlParam( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP set axi ctrl param error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_6: "
INT32 CT_Im_Ltm_Map_2_6( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP set ctrl common error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_7: "
INT32 CT_Im_Ltm_Map_2_7( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP set ctrl sdram input error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_8: "
INT32 CT_Im_Ltm_Map_2_8( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Set_InData_Info( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP set indata error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_9: "
INT32 CT_Im_Ltm_Map_2_9( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP set outdata error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_10: "
INT32 CT_Im_Ltm_Map_2_10( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Get_MapBusy( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP get map busy error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_11: "
INT32 CT_Im_Ltm_Map_2_11( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Set_WB_Gain( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP set wbgain error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_12: "
INT32 CT_Im_Ltm_Map_2_12( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Ctrl_Generation_Mode( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP ctrl generation mode error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_13: "
INT32 CT_Im_Ltm_Map_2_13( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Get_Internal_Param( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP get internal param error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_14: "
INT32 CT_Im_Ltm_Map_2_14( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP ctrl ylog tbl error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_15: "
INT32 CT_Im_Ltm_Map_2_15( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Get_Active_Ylog_Tbl_No( pipe_no, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP get active ylog tbl no error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_16: "
INT32 CT_Im_Ltm_Map_2_16( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, 0, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP set ylog tbl error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_2_17: "
INT32 CT_Im_Ltm_Map_2_17( UCHAR pipe_no )
{
	INT32 ercd = D_DDIM_OK;
	DDIM_USER_FLGPTN flgptn;
	DDIM_USER_FLGPTN waiptn;

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	waiptn = 0x11110000;
	ercd = Im_LTM_MAP_WaitEnd( &flgptn, waiptn, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_LTM_FUNC_NAME "MAP wait end error = 0x%x\n", ercd ));
	}

	return ercd;
}

static INT32 ct_im_ltm_map_run_2( UCHAR pipe_no, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-2-1
		case 1:
			return CT_Im_Ltm_Map_2_1( pipe_no );

		// Test-1-2-2
		case 2:
			return CT_Im_Ltm_Map_2_2( pipe_no );

		// Test-1-2-3
		case 3:
			return CT_Im_Ltm_Map_2_3( pipe_no );

		// Test-1-2-4
		case 4:
			return CT_Im_Ltm_Map_2_4( pipe_no );

		// Test-1-2-5
		case 5:
			return CT_Im_Ltm_Map_2_5( pipe_no );

		// Test-1-2-6
		case 6:
			return CT_Im_Ltm_Map_2_6( pipe_no );

		// Test-1-2-7
		case 7:
			return CT_Im_Ltm_Map_2_7( pipe_no );

		// Test-1-2-8
		case 8:
			return CT_Im_Ltm_Map_2_8( pipe_no );

		// Test-1-2-9
		case 9:
			return CT_Im_Ltm_Map_2_9( pipe_no );

		// Test-1-2-10
		case 10:
			return CT_Im_Ltm_Map_2_10( pipe_no );

		// Test-1-2-11
		case 11:
			return CT_Im_Ltm_Map_2_11( pipe_no );

		// Test-1-2-12
		case 12:
			return CT_Im_Ltm_Map_2_12( pipe_no );

		// Test-1-2-13
		case 13:
			return CT_Im_Ltm_Map_2_13( pipe_no );

		// Test-1-2-14
		case 14:
			return CT_Im_Ltm_Map_2_14( pipe_no );

		// Test-1-2-15
		case 15:
			return CT_Im_Ltm_Map_2_15( pipe_no );

		// Test-1-2-16
		case 16:
			return CT_Im_Ltm_Map_2_16( pipe_no );

		// Test-1-2-17
		case 17:
			return CT_Im_Ltm_Map_2_17( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}



#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_1: "
INT32 CT_Im_Ltm_Map_3_1( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_SW_Reset( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_SW_Reset( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_2: "
INT32 CT_Im_Ltm_Map_3_2( UCHAR pipe_no )
{
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Start( pipe_no );
#endif

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP1_INT_FLG_MAP_END;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP2_INT_FLG_MAP_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_MAP_Stop( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_3: "
INT32 CT_Im_Ltm_Map_3_3( UCHAR pipe_no )
{
	T_IM_LTM_MAP_CTRL_SDRAM_INPUT map_ctrl_sdram_in = gct_im_ltm_map_ctrl_sdram_in_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	map_ctrl_sdram_in.data_type = D_IM_LTM_DTYP_PACK12;
	map_ctrl_sdram_in.input_global = D_IM_LTM_MAP_SRC_IMG_GLOBAL_WIDTH_P;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &map_ctrl_sdram_in );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &map_ctrl_sdram_in );
	Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Start( pipe_no );
#endif

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP1_INT_FLG_MAP_END;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP2_INT_FLG_MAP_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_MAP_Stop( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_4: "
INT32 CT_Im_Ltm_Map_3_4( UCHAR pipe_no )
{
	T_IM_LTM_MAP_GAIN	map_wb_param = {
		.gain_r = 0x100,
		.gain_g = 0x100,
		.gain_b = 0x100,
		.clip_r = 0xFFF,
		.clip_g = 0xFFF,
		.clip_b = 0xFFF,
	};
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_WB_Gain( pipe_no, &map_wb_param );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_WB_Gain( pipe_no, &map_wb_param );
	Im_LTM_MAP_Start( pipe_no );
#endif

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP1_INT_FLG_MAP_END;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP2_INT_FLG_MAP_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_MAP_Stop( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_5: "
INT32 CT_Im_Ltm_Map_3_5( UCHAR pipe_no )
{
	T_IM_LTM_MAP_GAIN	map_wb_param = {
		.gain_r = (USHORT)(1.67592592592593F * 0x100),
		.gain_g = 0x100,
		.gain_b = (USHORT)(1.94623655913978F * 0x100),
		.clip_r = 0xFFF,
		.clip_g = 0xFFF,
		.clip_b = 0xFFF,
	};
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_WB_Gain( pipe_no, &map_wb_param );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_WB_Gain( pipe_no, &map_wb_param );
	Im_LTM_MAP_Start( pipe_no );
#endif

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP1_INT_FLG_MAP_END;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP2_INT_FLG_MAP_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_MAP_Stop( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_6: "
INT32 CT_Im_Ltm_Map_3_6( UCHAR pipe_no )
{
	T_IM_LTM_MAP_GAIN	map_wb_param = {
		.gain_r = 0x100,
		.gain_g = 0x100,
		.gain_b = 0x100,
		.clip_r = 0x400,
		.clip_g = 0x400,
		.clip_b = 0x400,
	};
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_WB_Gain( pipe_no, &map_wb_param );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_WB_Gain( pipe_no, &map_wb_param );
	Im_LTM_MAP_Start( pipe_no );
#endif

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP1_INT_FLG_MAP_END;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP2_INT_FLG_MAP_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_MAP_Stop( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_7: "
INT32 CT_Im_Ltm_Map_3_7( UCHAR pipe_no )
{
	T_IM_LTM_MAP_CTRL_GENERATION_MODE	map_mode_ctrl = {
		.mode0 = 0,
		.coef0 = 0x100,
		.mode1 = 0,
		.coef1 = 0x100,
		.mode2 = 0,
		.coef2 = 0x100,
	};
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Generation_Mode( pipe_no, &map_mode_ctrl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Ctrl_Generation_Mode( pipe_no, &map_mode_ctrl );
	Im_LTM_MAP_Start( pipe_no );
#endif

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP1_INT_FLG_MAP_END;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP2_INT_FLG_MAP_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_MAP_Stop( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_8: "
INT32 CT_Im_Ltm_Map_3_8( UCHAR pipe_no )
{
	T_IM_LTM_MAP_CTRL_GENERATION_MODE	map_mode_ctrl = {
		.mode0 = 1,
		.coef0 = 0x100,
		.mode1 = 1,
		.coef1 = 0x100,
		.mode2 = 1,
		.coef2 = 0x100,
	};
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Generation_Mode( pipe_no, &map_mode_ctrl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Ctrl_Generation_Mode( pipe_no, &map_mode_ctrl );
	Im_LTM_MAP_Start( pipe_no );
#endif

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP1_INT_FLG_MAP_END;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP2_INT_FLG_MAP_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_MAP_Stop( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_9: "
INT32 CT_Im_Ltm_Map_3_9( UCHAR pipe_no )
{
	T_IM_LTM_MAP_CTRL_GENERATION_MODE	map_mode_ctrl = {
		.mode0 = 0,
		.coef0 = 0xFFF,
		.mode1 = 0,
		.coef1 = 0xFFF,
		.mode2 = 0,
		.coef2 = 0xFFF,
	};
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Generation_Mode( pipe_no, &map_mode_ctrl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Ctrl_Generation_Mode( pipe_no, &map_mode_ctrl );
	Im_LTM_MAP_Start( pipe_no );
#endif

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP1_INT_FLG_MAP_END;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP2_INT_FLG_MAP_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_MAP_Stop( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_10: "
INT32 CT_Im_Ltm_Map_3_10( UCHAR pipe_no )
{
	T_IM_LTM_MAP_CTRL_GENERATION_MODE	map_mode_ctrl = {
		.mode0 = 0,
		.coef0 = 0,
		.mode1 = 0,
		.coef1 = 0,
		.mode2 = 0,
		.coef2 = 0,
	};
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Generation_Mode( pipe_no, &map_mode_ctrl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Ctrl_Generation_Mode( pipe_no, &map_mode_ctrl );
	Im_LTM_MAP_Start( pipe_no );
#endif

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP1_INT_FLG_MAP_END;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP2_INT_FLG_MAP_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_MAP_Stop( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_11: "
INT32 CT_Im_Ltm_Map_3_11( UCHAR pipe_no )
{
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	ct_im_ltm_map_set_rbk( pipe_no );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_ModeRBKDirect( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_Ctrl_Common( pipe_no, &gct_im_ltm_map_ctrl_common_base );
	Im_LTM_MAP_Ctrl_ModeRBKDirect( pipe_no );
	Im_LTM_MAP_Set_OutData_Info( pipe_no, &gct_im_ltm_map_outdata_base );
	Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_FrameStop( pipe_no, D_IM_LTM_ENABLE_ON );
	Im_LTM_MAP_Start( pipe_no );
#endif

	ct_im_ltm_map_start_rbk( pipe_no );

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP1_INT_FLG_MAP_END;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP2_INT_FLG_MAP_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_MAP_Stop( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

#undef D_IM_LTM_FUNC_NAME
#define D_IM_LTM_FUNC_NAME "CT_Im_Ltm_Map_3_12: "
INT32 CT_Im_Ltm_Map_3_12( UCHAR pipe_no )
{
	T_IM_LTM_MAP_CTRL_COMMON map_ctrl_common = gct_im_ltm_map_ctrl_common_base;
	T_IM_LTM_MAP_OUTDATA_INFO map_outdata_param = gct_im_ltm_map_outdata_base;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_LTM_FUNC_NAME ));

	map_ctrl_common.data_type_yp = D_IM_LTM_DTYP_PACK12;
	map_ctrl_common.data_type_yn = D_IM_LTM_DTYP_PACK12;
	map_outdata_param.global_width_yp = D_IM_LTM_MAP_DST_IMG_GLOBAL_WIDTH_P;
	map_outdata_param.global_width_yn = D_IM_LTM_MAP_DST_IMG_GLOBAL_WIDTH_P;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_Ctrl_Common( pipe_no, &map_ctrl_common );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_OutData_Info( pipe_no, &map_outdata_param );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Start( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_Ctrl_Common( pipe_no, &map_ctrl_common );
	Im_LTM_MAP_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_ltm_map_ctrl_sdram_in_base );
	Im_LTM_MAP_Set_InData_Info( pipe_no, &gct_im_ltm_map_indata_base );
	Im_LTM_MAP_Set_OutData_Info( pipe_no, &map_outdata_param );
	Im_LTM_MAP_Ctrl_Ylog_Tbl( pipe_no, &gct_im_ltm_map_ctrl_ylog_tbl );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_00, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Set_YLOG_Table( pipe_no, D_LTM_YLOG_TBL_10, &gct_im_ltm_map_ylog_tbl[0] );
	Im_LTM_MAP_Start( pipe_no );
#endif

	if( ct_im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP1_INT_FLG_MAP_END;
	}
	if( ct_im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_LTM_MAP2_INT_FLG_MAP_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_LTM_MAP_Stop( pipe_no );
	Ddim_Print(( D_IM_LTM_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_LTM_MAP_WaitEnd( NULL, flgptn, 60 );
	Im_LTM_MAP_Stop( pipe_no );
#endif

	ct_im_ltm_map_print_all_reg();

	return D_DDIM_OK;
}

static INT32 ct_im_ltm_map_run_3( UCHAR pipe_no, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-3-1
		case 1:
			return CT_Im_Ltm_Map_3_1( pipe_no );

		// Test-1-3-2
		case 2:
			return CT_Im_Ltm_Map_3_2( pipe_no );

		// Test-1-3-3
		case 3:
			return CT_Im_Ltm_Map_3_3( pipe_no );

		// Test-1-3-4
		case 4:
			return CT_Im_Ltm_Map_3_4( pipe_no );

		// Test-1-3-5
		case 5:
			return CT_Im_Ltm_Map_3_5( pipe_no );

		// Test-1-3-6
		case 6:
			return CT_Im_Ltm_Map_3_6( pipe_no );

		// Test-1-3-7
		case 7:
			return CT_Im_Ltm_Map_3_7( pipe_no );

		// Test-1-3-8
		case 8:
			return CT_Im_Ltm_Map_3_8( pipe_no );

		// Test-1-3-9
		case 9:
			return CT_Im_Ltm_Map_3_9( pipe_no );

		// Test-1-3-10
		case 10:
			return CT_Im_Ltm_Map_3_10( pipe_no );

		// Test-1-3-11
		case 11:
			return CT_Im_Ltm_Map_3_11( pipe_no );

		// Test-1-3-12
		case 12:
			return CT_Im_Ltm_Map_3_12( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}


VOID CT_Im_LTM_MAP_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
#ifdef CO_MSG_PRINT_ON
	INT32 result = D_DDIM_INPUT_PARAM_ERROR;
#endif
	static BOOL is_init = 0;
	UCHAR loop_cnt;

	Ddim_Print(( "CT_Im_LTM_MAP_Run( %u, %u, %u ) Begin\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	if( is_init == 0 ) {
#ifdef CO_DEBUG_ON_PC
		memset( (VOID*)&IO_LTM_P1.MAP, '\0', sizeof(IO_LTM_P1.MAP) );
		memset( (VOID*)&IO_LTM_P2.MAP, '\0', sizeof(IO_LTM_P2.MAP) );
		IO_LTM_P1.MAP.MTRG.word = 2;
		IO_LTM_P2.MAP.MTRG.word = 2;
#endif // CO_DEBUG_ON_PC
		if( !((ct_idx_1st == 1) && (ct_idx_2nd == 1)) ) {
			Ddim_Print(( "Im_LTM_MAP_Init() Begin\n" ));
			Im_LTM_MAP_Init( D_IM_LTM_PIPE12 );
			Ddim_Print(( "Im_LTM_MAP_Init() End\n" ));
			is_init = 1;
		}
	}

	if( ct_idx_1st != 3 ){
		for( loop_cnt = 0; loop_cnt <= 1; loop_cnt++ ){
			switch( ct_idx_1st ) {
				case 1:
					D_IM_LTM_RESULT( ct_im_ltm_map_run_1( loop_cnt, ct_idx_2nd ) );
					break;

				case 2:
					D_IM_LTM_RESULT( ct_im_ltm_map_run_2( loop_cnt, ct_idx_2nd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}

#ifdef CO_MSG_PRINT_ON
			Ddim_Print(( "result = 0x%x\n", result ));
#endif
			Ddim_Print(( "CT_Im_LTM_MAP_Run( %u, %u, %u) End\n", ct_idx_1st, ct_idx_2nd, loop_cnt ));
		}
	}
	else{
		D_IM_LTM_RESULT( ct_im_ltm_map_run_3( ct_idx_3rd, ct_idx_2nd ) );

#ifdef CO_MSG_PRINT_ON
		Ddim_Print(( "result = 0x%x\n", result ));
#endif
		Ddim_Print(( "CT_Im_LTM_MAP_Run( %u, %u, %u) End\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));
	}

	return;
}


// for Debug console interface
VOID CT_Im_LTM_MAP_Main( INT32 argc, CHAR** argv )
{
	if( argc < 3 ) {
		Ddim_Print(( "CT_Im_LTM_MAP_Main: parameter error.\n" ));
		return;
	}

	CT_Im_LTM_MAP_Run( (UINT32)atoi((const char*)argv[1]),
					   (UINT32)atoi((const char*)argv[2]),
					   (UINT32)atoi((const char*)argv[3]) );
}

