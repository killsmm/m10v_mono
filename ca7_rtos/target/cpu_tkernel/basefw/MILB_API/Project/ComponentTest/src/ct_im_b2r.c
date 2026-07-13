/**
 * @file		ct_im_b2r.c
 * @brief		B2R ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copybright 2015 Socionext Inc.</I></B>
 */

#include "ct_im_b2r.h"

#include "im_b2r.h"
#include "im_pro.h"
#include "im_pro_common.h"

#include "driver_common.h"
#include "ddim_user_custom.h"
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
#endif

#include "jdsb2r.h"

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
#ifdef D_IM_B2R_DEBUG_ON_PC
extern volatile UCHAR gIM_B2R_macro_fake_finish[2];
#endif


ULONG gct_im_b2r_in_addr = 0x54000000;
ULONG gct_im_b2r_out_addr[2] = {
	0x55000000,
	0x56000000,
};

#ifdef CO_MSG_PRINT_ON
#define D_IM_B2R_RESULT(statement)		result = statement
#else
#define D_IM_B2R_RESULT
#endif

static VOID ct_im_b2r_handler( UINT32* result, UINT32 user_param );

// Max/Min table
static T_IM_B2R_CTRL gct_im_b2r_ctrl[] = {
	[0] = {	// max
		.bayer_top_pixel = 3,

		.ybw = {
			.axi_write_mode = {
				.out_sel = 1,
				.burst_length = 2,
			},
			.output_dtype = 3,
			.write_request_mask[D_IM_B2R_PORT_0] = 0xFFF,
			.write_request_mask[D_IM_B2R_PORT_1] = 0xFFF,
			.write_request_mask[D_IM_B2R_PORT_2] = 0xFFF,
			.trim_out_enable = 1,
			.knee_enable = 1,
		},

		.line_intr = {
			.count = 0x3FFF,
		},
		.ybw_continue_start_enable = 1,
		.deknee_enable = 1,
		.cia_bypass = 1,
		.cia_padding = 1,
		.ext_out_en = 1,
		.ext_out_alarm = 3,
	},
	[1] = {	// min
		.bayer_top_pixel = 0,

		.ybw = {
			.axi_write_mode = {
				.out_sel = 0,
				.burst_length = 0,
			},
			.output_dtype = 0,
			.write_request_mask[D_IM_B2R_PORT_0] = 0,
			.write_request_mask[D_IM_B2R_PORT_1] = 0,
			.write_request_mask[D_IM_B2R_PORT_2] = 0,
			.trim_out_enable = 0,
			.knee_enable = 0,
		},

		.line_intr = {
			.count = 0,
		},
		.ybw_continue_start_enable = 0,
		.deknee_enable = 0,
		.cia_bypass = 0,
		.cia_padding = 0,
		.ext_out_alarm = 0,
	},
};
// Max/Min table
static T_IM_B2R_CTRL_SDRAM_INPUT gct_im_b2r_ctrl_sdram_in[] = {

		[0] = {	// max
			.burst_length = 2,
			.read_request_mask = 0xFFF,
			.input_dtype = 3,
			.top_offset = 7,
			.input_global = 0xFFFF,
			.line_space = 0x3FF,
		},
		[1] = {	// min
			.burst_length = 0,
			.read_request_mask = 0,
			.input_dtype = 0,
			.top_offset = 0,
			.input_global = 0,
			.line_space = 0,
		},
	};

// TestBase table
static T_IM_B2R_CTRL gct_im_b2r_ctrl_base = {
	.bayer_top_pixel = D_IM_B2R_TOP_PIXEL_R,

	.ybw = {
		.axi_write_mode = {
			.out_sel = D_IM_B2R_YBW_OUTPUT_RGB,
			.burst_length = D_IM_B2R_BRST_512,
		},
		.output_dtype = D_IM_B2R_STL_DTYP_PACK12,
		.write_request_mask[D_IM_B2R_PORT_R] = 0,
		.write_request_mask[D_IM_B2R_PORT_G] = 0,
		.write_request_mask[D_IM_B2R_PORT_B] = 0,
		.trim_out_enable = D_IM_B2R_ENABLE_OFF,
		.knee_enable = 1,
	},

	.line_intr = {
		.count = 0,
	},
	.ybw_continue_start_enable = D_IM_B2R_ENABLE_OFF,
	.deknee_enable = D_IM_B2R_ENABLE_OFF,
	.cia_bypass = D_IM_B2R_ENABLE_OFF,
	.cia_padding = D_IM_B2R_ENABLE_ON,
	.ext_out_en = D_IM_B2R_ENABLE_OFF,
	.ext_out_alarm = D_IM_B2R_EXT_OUT_ALARM_1LINE,
	.b2r_user_handler = ct_im_b2r_handler,
	.user_param = 0,
};
// TestBase table
static T_IM_B2R_CTRL_SDRAM_INPUT gct_im_b2r_ctrl_sdram_in_base = {
	.burst_length = D_IM_B2R_BRST_512,
	.read_request_mask = 0,
	.input_dtype = D_IM_B2R_STL_DTYP_PACK16,
	.top_offset = 0,
	.input_global = 2360 *2,
	.line_space = 0,
};

// T_IM_B2R_CTRL table for RAW input
static T_IM_B2R_CTRL gct_im_b2r_ctrl_raw = {
	.bayer_top_pixel = D_IM_B2R_TOP_PIXEL_R,

	.ybw = {
		.axi_write_mode = {
			.out_sel = D_IM_B2R_YBW_OUTPUT_RGB,
			.burst_length = D_IM_B2R_BRST_512,
		},
		.output_dtype = D_IM_B2R_STL_DTYP_PACK8,
		.write_request_mask[D_IM_B2R_PORT_R] = 0,
		.write_request_mask[D_IM_B2R_PORT_G] = 0,
		.write_request_mask[D_IM_B2R_PORT_B] = 0,
		.trim_out_enable = D_IM_B2R_ENABLE_OFF,
		.knee_enable = 1,
	},

	.line_intr = {
		.count = 0,
	},
	.ybw_continue_start_enable = D_IM_B2R_ENABLE_OFF,
	.deknee_enable = D_IM_B2R_ENABLE_OFF,
	.cia_bypass = D_IM_B2R_ENABLE_OFF,
	.cia_padding = D_IM_B2R_ENABLE_ON,
	.ext_out_en = D_IM_B2R_ENABLE_OFF,
	.ext_out_alarm = D_IM_B2R_EXT_OUT_ALARM_1LINE,
	.b2r_user_handler = ct_im_b2r_handler,
	.user_param = 0,
};

// T_IM_B2R_CTRL_SDRAM_INPUT table for RAW 16bit input
static T_IM_B2R_CTRL_SDRAM_INPUT gct_im_b2r_ctrl_sdram_in_raw16bit = {
	.burst_length = D_IM_B2R_BRST_512,
	.read_request_mask = 0,
	.input_dtype = D_IM_B2R_STL_DTYP_PACK16,
	.top_offset = 0,
	.input_global = D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_16BIT_NONPACK,
	.line_space = 0,
};

// T_IM_B2R_CTRL_SDRAM_INPUT table for RAW 12bit input
static T_IM_B2R_CTRL_SDRAM_INPUT gct_im_b2r_ctrl_sdram_in_raw12bit = {
	.burst_length = D_IM_B2R_BRST_512,
	.read_request_mask = 0,
	.input_dtype = D_IM_B2R_STL_DTYP_PACK12,
	.top_offset = 0,
	.input_global = D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_12BIT_PACK,
	.line_space = 0,
};

// T_IM_B2R_CTRL_SDRAM_INPUT table for RAW 10bit input
static T_IM_B2R_CTRL_SDRAM_INPUT gct_im_b2r_ctrl_sdram_in_raw10bit = {
	.burst_length = D_IM_B2R_BRST_512,
	.read_request_mask = 0,
	.input_dtype = D_IM_B2R_STL_DTYP_PACK10,
	.top_offset = 0,
	.input_global = D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_10BIT_PACK,
	.line_space = 0,
};

// T_IM_B2R_CTRL_SDRAM_INPUT table for RAW 8bit input
static T_IM_B2R_CTRL_SDRAM_INPUT gct_im_b2r_ctrl_sdram_in_raw8bit = {
	.burst_length = D_IM_B2R_BRST_512,
	.read_request_mask = 0,
	.input_dtype = D_IM_B2R_STL_DTYP_PACK8,
	.top_offset = 0,
	.input_global = D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_8BIT_NONPACK,
	.line_space = 0,
};

// T_IM_B2R_RECT table for RGB U16
static T_IM_B2R_RECT gct_im_b2r_rect_param_out_rgb_u16 = {
	.input_size.img_left = 0,
	.input_size.img_top = 0,
	.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH,
	.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES,
	.output_size = {
		.output_global_w = D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U16,
	},
};

// T_IM_B2R_RECT table for RGB P12
static T_IM_B2R_RECT gct_im_b2r_rect_param_out_rgb_p12 = {
	.input_size.img_left = 0,
	.input_size.img_top = 0,
	.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH,
	.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES,
	.output_size = {
		.output_global_w = D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_P12,
	},
};

// T_IM_B2R_RECT table for RGB U8
static T_IM_B2R_RECT gct_im_b2r_rect_param_out_rgb_u8 = {
	.input_size.img_left = 0,
	.input_size.img_top = 0,
	.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH,
	.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES,
	.output_size = {
		.output_global_w = D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U8,
	},
};

// T_IM_B2R_INADDR_INFO table for RAW input
static T_IM_B2R_INADDR_INFO gct_im_b2r_in_addr_raw = {
	.raw.addr = NULL,
};

// T_IM_B2R_OUTBANK_INFO table for RGB output
static T_IM_B2R_OUTBANK_INFO gct_im_b2r_out_addr_rgb = {
	.bank_initial_position = D_IM_B2R_YBW_BANK_0,
	.use_bank_num = D_IM_B2R_YBW_BANK_0,
	.output_addr[D_IM_B2R_YBW_BANK_0] = {
		.rgb.addr_R = NULL,
		.rgb.addr_G = NULL,
		.rgb.addr_B = NULL,
	},
};

// T_IM_B2R_CTRL_B2B_DIRECT table
T_IM_B2R_CTRL_B2B_DIRECT gct_im_b2r_ctrl_b2b_direct = {
	.even_line_start_timming = 1000,
	.frame_stop = 1,
};


// Deknee table.
static const USHORT gct_im_b2r_deknee_tbl[D_IM_B2R_TABLE_MAX_BAYER_DEKNEE] = {
	     0,    16,    32,    48,    64,    80,    96,   112,   128,   144,   160,   176,   192,   208,   224,   240,
	   256,   273,   289,   305,   321,   337,   353,   369,   385,   401,   417,   433,   449,   465,   481,   497,
	   513,   529,   546,   562,   578,   594,   610,   626,   642,   658,   674,   690,   706,   722,   738,   754,
	   770,   786,   802,   819,   835,   851,   867,   883,   899,   915,   931,   947,   963,   979,   995,  1011,
	  1027,  1043,  1059,  1075,  1092,  1108,  1124,  1140,  1156,  1172,  1188,  1204,  1220,  1236,  1252,  1268,
	  1284,  1300,  1316,  1332,  1348,  1365,  1381,  1397,  1413,  1429,  1445,  1461,  1477,  1493,  1509,  1525,
	  1541,  1557,  1573,  1589,  1605,  1621,  1638,  1654,  1670,  1686,  1702,  1718,  1734,  1750,  1766,  1782,
	  1798,  1814,  1830,  1846,  1862,  1878,  1894,  1911,  1927,  1943,  1959,  1975,  1991,  2007,  2023,  2039,
	  2055,  2071,  2087,  2103,  2119,  2135,  2151,  2167,  2184,  2200,  2216,  2232,  2248,  2264,  2280,  2296,
	  2312,  2328,  2344,  2360,  2376,  2392,  2408,  2424,  2440,  2457,  2473,  2489,  2505,  2521,  2537,  2553,
	  2569,  2585,  2601,  2617,  2633,  2649,  2665,  2681,  2697,  2713,  2730,  2746,  2762,  2778,  2794,  2810,
	  2826,  2842,  2858,  2874,  2890,  2906,  2922,  2938,  2954,  2970,  2986,  3003,  3019,  3035,  3051,  3067,
	  3083,  3099,  3115,  3131,  3147,  3163,  3179,  3195,  3211,  3227,  3243,  3259,  3276,  3292,  3308,  3324,
	  3340,  3356,  3372,  3388,  3404,  3420,  3436,  3452,  3468,  3484,  3500,  3516,  3532,  3549,  3565,  3581,
	  3597,  3613,  3629,  3645,  3661,  3677,  3693,  3709,  3725,  3741,  3757,  3773,  3789,  3805,  3822,  3838,
	  3854,  3870,  3886,  3902,  3918,  3934,  3950,  3966,  3982,  3998,  4014,  4030,  4046,  4062,  4078,  4095,
};

// Deknee table.
static const UCHAR gct_im_b2r_knee_tbl[D_IM_B2R_TABLE_MAX_RGB_KNEE] = {
	  0,   1,   1,   2,   2,   3,   3,   4,   4,   5,   5,   6,   6,   7,   8,  12,
	 16,  20,  24,  28,  32,  36,  40,  44,  48,  52,  56,  58,  60,  62,  64,  66,
	 68,  70,  72,  74,  76,  78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98,
	100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 119, 120, 121, 122, 123, 124,
	125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
	141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156,
	157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
	173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188,
	189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204,
	205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
	221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 230, 231, 231, 232, 232, 233,
	233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238, 239, 239, 240, 240, 241,
	241, 242, 242, 243, 243, 243, 243, 244, 244, 244, 244, 245, 245, 245, 245, 246,
	246, 246, 246, 247, 247, 247, 247, 248, 248, 248, 248, 249, 249, 249, 249, 250,
	250, 250, 250, 251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252, 253,
	253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255
};


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef IM_B2R_STATUS_PRINT
extern VOID Im_B2R_Print_Status( VOID );
extern VOID Im_B2R_Print_ClockStatus( VOID );
extern VOID Im_B2R_Print_AccEnStatus( VOID );
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define ct_im_b2r_check_target_pipe_no_1(a)		(((a)+1) & (D_IM_B2R_PIPE1+1))
#define ct_im_b2r_check_target_pipe_no_2(a)		(((a)+1) & (D_IM_B2R_PIPE2+1))


static VOID ct_im_b2r_handler( UINT32* result, UINT32 user_param )
{
	if( ((*result) & D_IM_B2R1_INT_STATE_YBR_END) != 0 ) {
		Ddim_Print(( "B2R1 YBRch end. %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R1_INT_STATE_YBW_END) != 0 ) {
		Ddim_Print(( "B2R1 YBWch end. %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R1_INT_STATE_FB2R_END) != 0 ) {
		Ddim_Print(( "B2R1 F_B2R end. %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R1_INT_STATE_LINE_END) != 0 ) {
		Ddim_Print(( "B2R1 line end. %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R1_INT_STATE_YBR_ERR) != 0 ) {
		Ddim_Print(( "B2R1 YBRch error! %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R1_INT_STATE_YBW_ERR) != 0 ) {
		Ddim_Print(( "B2R1 YBWch error! %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R1_INT_STATE_AXR_ERR) != 0 ) {
		Ddim_Print(( "B2R1 AXR error! %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R1_INT_STATE_AXW_ERR) != 0 ) {
		Ddim_Print(( "B2R1 AXW error! %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R1_INT_STATE_CPERR_ERR) != 0 ) {
		Ddim_Print(( "B2R1 CP error! %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R2_INT_STATE_YBR_END) != 0 ) {
		Ddim_Print(( "B2R2 YBRch end. %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R2_INT_STATE_YBW_END) != 0 ) {
		Ddim_Print(( "B2R2 YBWch end. %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R2_INT_STATE_FB2R_END) != 0 ) {
		Ddim_Print(( "B2R2 F_B2R end. %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R2_INT_STATE_LINE_END) != 0 ) {
		Ddim_Print(( "B2R2 line end. %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R2_INT_STATE_YBR_ERR) != 0 ) {
		Ddim_Print(( "B2R2 YBRch error! %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R2_INT_STATE_YBW_ERR) != 0 ) {
		Ddim_Print(( "B2R2 YBWch error! %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R2_INT_STATE_AXR_ERR) != 0 ) {
		Ddim_Print(( "B2R2 AXR error! %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R2_INT_STATE_AXW_ERR) != 0 ) {
		Ddim_Print(( "B2R2 AXW error! %x\n", user_param ));
	}
	if( ((*result) & D_IM_B2R2_INT_STATE_CPERR_ERR) != 0 ) {
		Ddim_Print(( "B2R2 CP error! %x\n", user_param ));
	}
}

static VOID ct_im_b2r_b2b_prch_int_handler( T_CALLBACK_ID* cb_id, ULONG result, ULONG user_param )
{
	Ddim_Print(( "PRch Int %u, %u, %u %lx, %lx\n", cb_id->unit_no, cb_id->block_type, cb_id->channel, result, user_param ));
}

static volatile BOOL gct_im_b2r_b2b_vd_flg = FALSE;
static VOID ct_im_b2r_b2b_vd_int_handler( T_CALLBACK_ID* cb_id, ULONG result, ULONG user_param )
{
	Ddim_Print(( "VD Int %u, %u, %u %lx, %lx\n", cb_id->unit_no, cb_id->block_type, cb_id->channel, result, user_param ));
	gct_im_b2r_b2b_vd_flg = TRUE;
}

static VOID ct_im_b2r_set_b2b( E_IM_PRO_UNIT_NUM unit_no )
{
	T_IM_PRO_B2BTOP_CTRL		b2b_ctrl;
	T_IM_PRO_PG_CTRL			pg_ctrl;
	T_IM_PRO_M2P_CTRL			m2p_ctrl;
	T_IM_PRO_PRCH_CTRL			pr_ctrl;
	T_IM_PRO_PRCH_AREA			pr_area = {
		.width = 640 * 1.5,				// bytes
		.height = 480 / 2,
		.global_width = 640 * 2 * 1.5,	// 12bit pack, Interlace
	};
	T_IM_PRO_INT_B2BTOP_CTRL	int_ctrl;
	T_IM_PRO_CALLBACK_CFG		callback_cfg;

	Im_PRO_B2BTOP_Init( unit_no );

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Dd_Top_Set_CLKSTOP10_B2B1CK( 1 );
	Dd_Top_Set_CLKSTOP12_B2B2CK( 1 );
	Dd_Top_Set_CLKSEL5_B2B1CLK( 0 );		// 300MHz
	Dd_Top_Set_CLKSEL7_B2B2CLK( 0 );		// 300MHz
	Dd_Top_Set_CLKSTOP10_B2B1CK( 0 );
	Dd_Top_Set_CLKSTOP12_B2B2CK( 0 );

	Ddim_Print(( "B2B1 CLK %u\n", Dd_Top_Get_CLKSEL5_B2B1CLK() ));
	Ddim_Print(( "B2B2 CLK %u\n", Dd_Top_Get_CLKSEL7_B2B2CLK() ));
#endif

	Im_PRO_B2BTOP_Control_Clock( unit_no, E_IM_PRO_B2BTOP_CLK_TYPE_B2BCLK, D_IM_PRO_CLOCK_ON, 1 );

	// B2BTOP(and PAT) settings
	b2b_ctrl.input_mode		= E_IM_PRO_B2BTOP_INPUT_MODE_REINPUT;
	b2b_ctrl.cag_target_sel	= E_IM_PRO_B2BTOP_CAG_TARGET_BLOCK_SRO;
	b2b_ctrl.first_pixel[0]	= E_IM_PRO_ORG_R;
	b2b_ctrl.first_pixel[1]	= E_IM_PRO_ORG_GB;
	Im_PRO_B2BTOP_Ctrl( unit_no, &b2b_ctrl );

	// PG settings
	pg_ctrl.mode				= E_IM_PRO_PG_MODE_2LINE;
	pg_ctrl.input_img_width		= D_IM_B2R_SRC_IMG_PIXS_WIDTH;
	pg_ctrl.input_img_lines		= D_IM_B2R_SRC_IMG_PIXS_LINES / 2;
	pg_ctrl.vertical_blanking	= 200;		// branking line after image
	pg_ctrl.horizontal_blanking	= 1000;		// branking line after image
	pg_ctrl.start_blanking		= 100;		// branking line before image
	Im_PRO_PG_Ctrl( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, &pg_ctrl );

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
	Im_PRO_M2P_Ctrl( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_0, &m2p_ctrl );
	Im_PRO_M2P_Ctrl( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_1, &m2p_ctrl );

	// PRch settings
	pr_ctrl.trans_mode		= E_IM_PRO_PRCH_BURST_512BYTE;
	pr_ctrl.req_threshold	= E_IM_PRO_PRCH_CNT_NOLIMIT;
	pr_ctrl.trans_to		= E_IM_PRO_PRCH_TRANS_IO_0;
	Im_PRO_PRch_Ctrl( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_0, &pr_ctrl );
	Im_PRO_PRch_Ctrl( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_1, &pr_ctrl );

	Im_PRO_PRch_Set_Addr( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_0, D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP );
	Im_PRO_PRch_Set_Addr( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_1, D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP + pr_area.global_width / 2 );

	Im_PRO_PRch_Set_Area( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_0, &pr_area );
	Im_PRO_PRch_Set_Area( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_1, &pr_area );

	int_ctrl.int_mode						= E_IM_PRO_INT_INTMD_OR;
	int_ctrl.vd_enable[0].interrupt_bit		= D_IM_PRO_B2BTOP_INT_VDE0;
	int_ctrl.vd_enable[0].permission_flg	= 1;
	int_ctrl.hd_enable[0].interrupt_bit		= 0;
	int_ctrl.hd_enable[0].permission_flg	= 0;
	int_ctrl.vd_enable[1].interrupt_bit		= D_IM_PRO_B2BTOP_INT_VDE1;
	int_ctrl.vd_enable[1].permission_flg	= 1;
	int_ctrl.hd_enable[1].interrupt_bit		= 0;
	int_ctrl.hd_enable[1].permission_flg	= 0;
	Im_PRO_B2BTOP_Interrupt_Ctrl( unit_no, &int_ctrl );

	callback_cfg.inthandler	= ct_im_b2r_b2b_vd_int_handler;
	callback_cfg.user_param	= 0;
	Im_PRO_B2BTOP_Pipe_Set_VD_Int_Handler( unit_no, &callback_cfg );

	callback_cfg.inthandler	= ct_im_b2r_b2b_prch_int_handler;
	callback_cfg.user_param	= 0;
	Im_PRO_PRch_Set_Int_Handler( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_0, &callback_cfg );
	Im_PRO_PRch_Set_Int_Handler( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_1, &callback_cfg );

}

static VOID ct_im_b2r_start_b2b( E_IM_PRO_UNIT_NUM unit_no )
{
	gct_im_b2r_b2b_vd_flg = FALSE;

	Im_PRO_PRch_Start( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_0 );
	Im_PRO_PRch_Start( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_1 );
	Im_PRO_M2P_Set_PAEN( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_0, E_IM_PRO_M2P_PAEN_EN );
	Im_PRO_M2P_Set_PAEN( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_1, E_IM_PRO_M2P_PAEN_EN );
	Im_PRO_M2P_Start( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_0 );
	Im_PRO_M2P_Start( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_1 );

	Im_PRO_PG_Start( unit_no, E_IM_PRO_BLOCK_TYPE_B2B );
}

static VOID ct_im_b2r_wait_vd_b2b( VOID )
{
	UINT32 loopcnt;
#ifdef CO_DEBUG_ON_PC
	T_CALLBACK_ID cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};
#endif

	for( loopcnt = 0; gct_im_b2r_b2b_vd_flg == FALSE; loopcnt++ ) {
		if( loopcnt >= 10 ) {
			Ddim_Print(( "B2B wait end timeout.\n" ));
			break;
		}

		DDIM_User_Dly_Tsk( 1 );
#ifdef CO_DEBUG_ON_PC
		ct_im_b2r_b2b_vd_int_handler( &cb_id, 0, 0 );
#endif
	}

	gct_im_b2r_b2b_vd_flg = FALSE;
}

static VOID ct_im_b2r_waitend_stop_b2b( E_IM_PRO_UNIT_NUM unit_no )
{
	ct_im_b2r_wait_vd_b2b();

	Im_PRO_PWch_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PWCH_0, 0 );
	Im_PRO_P2M_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_P2M_CH0, 0 );
	Im_PRO_PWch_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PWCH_1, 0 );
	Im_PRO_P2M_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_P2M_CH1, 0 );

	Im_PRO_PRch_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_0, 0 );
	Im_PRO_M2P_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_0, 0 );
	Im_PRO_PRch_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_1, 0 );
	Im_PRO_M2P_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_1, 0 );

	ct_im_b2r_wait_vd_b2b();

	Im_PRO_PG_Stop( unit_no, E_IM_PRO_BLOCK_TYPE_B2B );
}


static VOID ct_im_b2r_set_wb_gain( UCHAR pipe_no )
{
	static T_IM_B2R_BAYER_COLOR im_b2r_wb_gain = {
		(USHORT)(1.67592592592593F * 0x100),	// R
		(USHORT)(1.94623655913978F * 0x100),	// B
		(USHORT)(1.0F * 0x100),	// Gr
		(USHORT)(1.0F * 0x100),	// Gb
	};
	INT32	im_ercd;
	im_ercd = Im_B2R_Set_WB_Gain( pipe_no, &im_b2r_wb_gain );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( "Im_B2R_Set_WB_Gain() error=%d\n", im_ercd ));
	}
}

static VOID ct_im_b2r_print_b2r_common_reg( VOID )
{
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.YBINTE = 0x%lx\n", IO_B2R_P1.B2R_CMN.YBINTE.word ));
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.YBINTF = 0x%lx\n", IO_B2R_P1.B2R_CMN.YBINTF.word ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.YBINTE = 0x%lx\n", IO_B2R_P2.B2R_CMN.YBINTE.word ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.YBINTF = 0x%lx\n", IO_B2R_P2.B2R_CMN.YBINTF.word ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}

static VOID ct_im_b2r_print_b2r_ybr_reg( VOID )
{
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 0x%x\n", IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG ));
	Ddim_Print(( "IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 0x%x\n", IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}

static VOID ct_im_b2r_print_b2r_ybw_reg( VOID )
{
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWTRG.bit.YBWTRG = 0x%x\n", IO_B2R_P1.YBW.YBWTRG.bit.YBWTRG ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWTRG.bit.YBWTRG = 0x%x\n", IO_B2R_P2.YBW.YBWTRG.bit.YBWTRG ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}

static VOID ct_im_b2r_print_b2r_ctrl_reg( VOID )
{
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXMD.bit.YBWDOS        = %u\n",   IO_B2R_P1.YBCH.YBWAXMD.bit.YBWDOS ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXMD.bit.YBWBL         = %u\n",   IO_B2R_P1.YBCH.YBWAXMD.bit.YBWBL ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBWREQMSK.bit.YBWREQMSK_0 = 0x%x\n", IO_B2R_P1.YBCH.YBWREQMSK.bit.YBWREQMSK_0 ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBWREQMSK.bit.YBWREQMSK_1 = 0x%x\n", IO_B2R_P1.YBCH.YBWREQMSK.bit.YBWREQMSK_1 ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBWREQMSK.bit.YBWREQMSK_2 = 0x%x\n", IO_B2R_P1.YBCH.YBWREQMSK.bit.YBWREQMSK_2 ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.ELNLV.bit.ELNLV        = 0x%x\n", IO_B2R_P1.B2R_CMN.ELNLV.bit.ELNLV ));
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.B2RMODE.bit.CIABYP     = 0x%x\n", IO_B2R_P1.B2R_CMN.B2RMODE.bit.CIABYP ));
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.B2RMODE.bit.CIAPDD     = 0x%x\n", IO_B2R_P1.B2R_CMN.B2RMODE.bit.CIAPDD ));
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.EXALMSL.bit.EXALMSL    = 0x%x\n", IO_B2R_P1.B2R_CMN.EXALMSL.bit.EXALMSL ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.YBR.YBRCTL.bit.PIXEL           = %u\n",   IO_B2R_P1.YBR.YBRCTL.bit.PIXEL ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWCTL.bit.YBWDTYP         = %u\n",   IO_B2R_P1.YBW.YBWCTL.bit.YBWDTYP ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWCTL.bit.YBWCSE          = %u\n",   IO_B2R_P1.YBW.YBWCTL.bit.YBWCSE ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWMD.bit.TRMEN            = %u\n",   IO_B2R_P1.YBW.YBWMD.bit.TRMEN ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWMD.bit.KNEEN            = %u\n",   IO_B2R_P1.YBW.YBWMD.bit.KNEEN ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXMD.bit.YBWDOS        = %u\n",   IO_B2R_P2.YBCH.YBWAXMD.bit.YBWDOS ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXMD.bit.YBWBL         = %u\n",   IO_B2R_P2.YBCH.YBWAXMD.bit.YBWBL ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBWREQMSK.bit.YBWREQMSK_0 = 0x%x\n", IO_B2R_P2.YBCH.YBWREQMSK.bit.YBWREQMSK_0 ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBWREQMSK.bit.YBWREQMSK_1 = 0x%x\n", IO_B2R_P2.YBCH.YBWREQMSK.bit.YBWREQMSK_1 ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBWREQMSK.bit.YBWREQMSK_2 = 0x%x\n", IO_B2R_P2.YBCH.YBWREQMSK.bit.YBWREQMSK_2 ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.ELNLV.bit.ELNLV        = 0x%x\n", IO_B2R_P2.B2R_CMN.ELNLV.bit.ELNLV ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.B2RMODE.bit.CIABYP     = 0x%x\n", IO_B2R_P2.B2R_CMN.B2RMODE.bit.CIABYP ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.B2RMODE.bit.CIAPDD     = 0x%x\n", IO_B2R_P2.B2R_CMN.B2RMODE.bit.CIAPDD ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.EXALMSL.bit.EXALMSL    = 0x%x\n", IO_B2R_P2.B2R_CMN.EXALMSL.bit.EXALMSL ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P2.YBR.YBRCTL.bit.PIXEL           = %u\n",   IO_B2R_P2.YBR.YBRCTL.bit.PIXEL ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWCTL.bit.YBWDTYP         = %u\n",   IO_B2R_P2.YBW.YBWCTL.bit.YBWDTYP ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWCTL.bit.YBWCSE          = %u\n",   IO_B2R_P2.YBW.YBWCTL.bit.YBWCSE ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWMD.bit.TRMEN            = %u\n",   IO_B2R_P2.YBW.YBWMD.bit.TRMEN ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWMD.bit.KNEEN            = %u\n",   IO_B2R_P2.YBW.YBWMD.bit.KNEEN ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}

static VOID ct_im_b2r_print_b2r_ctrl_modesdraminput_reg( VOID )
{
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.DINSEL.bit.B2BDIN    = %u\n",   IO_B2R_P1.B2R_CMN.DINSEL.bit.B2BDIN ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBRAXMD.bit.YBRBL       = %u\n",   IO_B2R_P1.YBCH.YBRAXMD.bit.YBRBL ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBRREQMSK.bit.YBRREQMSK = 0x%x\n", IO_B2R_P1.YBCH.YBRREQMSK.bit.YBRREQMSK ));
	Ddim_Print(( "IO_B2R_P1.YBR.YBRCTL.bit.YBRDTYP       = %u\n",   IO_B2R_P1.YBR.YBRCTL.bit.YBRDTYP ));
	Ddim_Print(( "IO_B2R_P1.YBR.YBRCTL.bit.DKNEN         = 0x%x\n", IO_B2R_P1.YBR.YBRCTL.bit.DKNEN ));
	Ddim_Print(( "IO_B2R_P1.YBR.YBROFS.bit.YBROFS        = %u\n",   IO_B2R_P1.YBR.YBROFS.bit.YBROFS ));
	Ddim_Print(( "IO_B2R_P1.YBR.YBRDEF.bit.YBRDEF        = 0x%x\n", IO_B2R_P1.YBR.YBRDEF.bit.YBRDEF ));
	Ddim_Print(( "IO_B2R_P1.YBR.YBRHBLNK.bit.YBRHBLNK    = 0x%x\n", IO_B2R_P1.YBR.YBRHBLNK.bit.YBRHBLNK ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.DINSEL.bit.B2BDIN    = %u\n",   IO_B2R_P2.B2R_CMN.DINSEL.bit.B2BDIN ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBRAXMD.bit.YBRBL       = %u\n",   IO_B2R_P2.YBCH.YBRAXMD.bit.YBRBL ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBRREQMSK.bit.YBRREQMSK = 0x%x\n", IO_B2R_P2.YBCH.YBRREQMSK.bit.YBRREQMSK ));
	Ddim_Print(( "IO_B2R_P2.YBR.YBRCTL.bit.YBRDTYP       = %u\n",   IO_B2R_P2.YBR.YBRCTL.bit.YBRDTYP ));
	Ddim_Print(( "IO_B2R_P2.YBR.YBRCTL.bit.DKNEN         = 0x%x\n", IO_B2R_P2.YBR.YBRCTL.bit.DKNEN ));
	Ddim_Print(( "IO_B2R_P2.YBR.YBROFS.bit.YBROFS        = %u\n",   IO_B2R_P2.YBR.YBROFS.bit.YBROFS ));
	Ddim_Print(( "IO_B2R_P2.YBR.YBRDEF.bit.YBRDEF        = 0x%x\n", IO_B2R_P2.YBR.YBRDEF.bit.YBRDEF ));
	Ddim_Print(( "IO_B2R_P2.YBR.YBRHBLNK.bit.YBRHBLNK    = 0x%x\n", IO_B2R_P2.YBR.YBRHBLNK.bit.YBRHBLNK ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}

static VOID ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg( VOID )
{
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.DINSEL.bit.B2BDIN = %u\n",   IO_B2R_P1.B2R_CMN.DINSEL.bit.B2BDIN ));
	Ddim_Print(( "IO_B2R_P1.YBR.DINELS.bit.DINELS     = 0x%x\n", IO_B2R_P1.YBR.DINELS.bit.DINELS ));
	Ddim_Print(( "IO_B2R_P1.YBR.DINCTL.bit.FRMSTP     = %u\n",   IO_B2R_P1.YBR.DINCTL.bit.FRMSTP ));
	Ddim_Print(( "IO_B2R_P1.YBR.DINHSTA               = %lu\n",  IO_B2R_P1.YBR.DINHSTA.word ));
	Ddim_Print(( "IO_B2R_P1.YBR.DINVSTA               = %lu\n",  IO_B2R_P1.YBR.DINVSTA.word ));
	Ddim_Print(( "IO_B2R_P1.YBR.DINHSIZ               = %lu\n",  IO_B2R_P1.YBR.DINHSIZ.word ));
	Ddim_Print(( "IO_B2R_P1.YBR.DINVSIZ               = %lu\n",  IO_B2R_P1.YBR.DINVSIZ.word ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.DINSEL.bit.B2BDIN = %u\n",   IO_B2R_P2.B2R_CMN.DINSEL.bit.B2BDIN ));
	Ddim_Print(( "IO_B2R_P2.YBR.DINELS.bit.DINELS     = 0x%x\n", IO_B2R_P2.YBR.DINELS.bit.DINELS ));
	Ddim_Print(( "IO_B2R_P2.YBR.DINCTL.bit.FRMSTP     = %u\n",   IO_B2R_P2.YBR.DINCTL.bit.FRMSTP ));
	Ddim_Print(( "IO_B2R_P2.YBR.DINHSTA               = %lu\n",  IO_B2R_P2.YBR.DINHSTA.word ));
	Ddim_Print(( "IO_B2R_P2.YBR.DINVSTA               = %lu\n",  IO_B2R_P2.YBR.DINVSTA.word ));
	Ddim_Print(( "IO_B2R_P2.YBR.DINHSIZ               = %lu\n",  IO_B2R_P2.YBR.DINHSIZ.word ));
	Ddim_Print(( "IO_B2R_P2.YBR.DINVSIZ               = %lu\n",  IO_B2R_P2.YBR.DINVSIZ.word ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}

static VOID ct_im_b2r_print_rect_reg( VOID )
{
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "\n" ));
	// im_b2r_calc_ybr_address_sdram_input(), im_b2r_set_ybr_address_sdram_input(), Im_B2R_Set_InAddr_Info()
	Ddim_Print(( "IO_B2R_P1.YBR.YBRA.bit.YBRA     = %lx\n",    IO_B2R_P1.YBR.YBRA.bit.YBRA ));
	// im_b2r_calc_in_rect_sdram_input()
	Ddim_Print(( "IO_B2R_P1.YBR.YBRHSIZ           = %lu\n",    IO_B2R_P1.YBR.YBRHSIZ.word ));
	Ddim_Print(( "IO_B2R_P1.YBR.YBRVSIZ           = %lu\n",    IO_B2R_P1.YBR.YBRVSIZ.word ));
	// Im_B2R_Set_Rect()
	Ddim_Print(( "IO_B2R_P1.YBW.YBWDEF.bit.YBWDEF = %x(%d)\n", IO_B2R_P1.YBW.YBWDEF.bit.YBWDEF, IO_B2R_P1.YBW.YBWDEF.bit.YBWDEF ));

	Ddim_Print(( "\n" ));
	// im_b2r_calc_ybr_address_sdram_input(), im_b2r_set_ybr_address_sdram_input(), Im_B2R_Set_InAddr_Info()
	Ddim_Print(( "IO_B2R_P2.YBR.YBRA.bit.YBRA     = %lx\n",    IO_B2R_P2.YBR.YBRA.bit.YBRA ));
	// im_b2r_calc_in_rect_sdram_input()
	Ddim_Print(( "IO_B2R_P2.YBR.YBRHSIZ           = %lu\n",    IO_B2R_P2.YBR.YBRHSIZ.word ));
	Ddim_Print(( "IO_B2R_P2.YBR.YBRVSIZ           = %lu\n",    IO_B2R_P2.YBR.YBRVSIZ.word ));
	// Im_B2R_Set_Rect()
	Ddim_Print(( "IO_B2R_P2.YBW.YBWDEF.bit.YBWDEF = %x(%d)\n", IO_B2R_P2.YBW.YBWDEF.bit.YBWDEF, IO_B2R_P2.YBW.YBWDEF.bit.YBWDEF ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}

static VOID ct_im_b2r_print_trimming_reg( VOID )
{
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWTRMH.bit.TRMHSTA = %u\n", IO_B2R_P1.YBW.YBWTRMH.bit.TRMHSTA ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWTRMH.bit.TRMHSIZ = %u\n", IO_B2R_P1.YBW.YBWTRMH.bit.TRMHSIZ ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWTRMV.bit.TRMVSTA = %u\n", IO_B2R_P1.YBW.YBWTRMV.bit.TRMVSTA ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWTRMV.bit.TRMVSIZ = %u\n", IO_B2R_P1.YBW.YBWTRMV.bit.TRMVSIZ ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWTRMH.bit.TRMHSTA = %u\n", IO_B2R_P2.YBW.YBWTRMH.bit.TRMHSTA ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWTRMH.bit.TRMHSIZ = %u\n", IO_B2R_P2.YBW.YBWTRMH.bit.TRMHSIZ ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWTRMV.bit.TRMVSTA = %u\n", IO_B2R_P2.YBW.YBWTRMV.bit.TRMVSTA ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWTRMV.bit.TRMVSIZ = %u\n", IO_B2R_P2.YBW.YBWTRMV.bit.TRMVSIZ ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}

static VOID ct_im_b2r_print_other_reg( VOID )
{
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	// Other
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWCTL.bit.USEBANK  = %u\n",   IO_B2R_P1.YBW.YBWCTL.bit.USEBANK ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_0_0   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_0_0 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_1_0   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_1_0 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_2_0   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_2_0 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_0_1   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_0_1 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_1_1   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_1_1 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_2_1   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_2_1 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_0_2   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_0_2 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_1_2   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_1_2 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_2_2   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_2_2 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_0_3   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_0_3 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_1_3   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_1_3 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_2_3   = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_2_3 ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWBANK.bit.BANKPS  = %u\n",   IO_B2R_P1.YBW.YBWBANK.bit.BANKPS ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWBANK.bit.BANKSTS = %u\n",   IO_B2R_P1.YBW.YBWBANK.bit.BANKSTS ));
	Ddim_Print(( "IO_B2R_P1.YBW.YBWBANK.bit.BANKST  = %u\n",   IO_B2R_P1.YBW.YBWBANK.bit.BANKST ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWCTL.bit.USEBANK  = %u\n",   IO_B2R_P2.YBW.YBWCTL.bit.USEBANK ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_0_0   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_0_0 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_1_0   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_1_0 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_2_0   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_2_0 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_0_1   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_0_1 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_1_1   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_1_1 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_2_1   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_2_1 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_0_2   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_0_2 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_1_2   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_1_2 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_2_2   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_2_2 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_0_3   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_0_3 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_1_3   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_1_3 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_2_3   = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_2_3 ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWBANK.bit.BANKPS  = %u\n",   IO_B2R_P2.YBW.YBWBANK.bit.BANKPS ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWBANK.bit.BANKSTS = %u\n",   IO_B2R_P2.YBW.YBWBANK.bit.BANKSTS ));
	Ddim_Print(( "IO_B2R_P2.YBW.YBWBANK.bit.BANKST  = %u\n",   IO_B2R_P2.YBW.YBWBANK.bit.BANKST ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}

static VOID ct_im_b2r_print_fb2r_ctrl_reg( VOID )
{
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "(F_B2R)\n" ));
	Ddim_Print(( "IO_B2R_P1.F_B2R.CNTL.CNTL.bit.TRG = %u\n",    IO_B2R_P1.F_B2R.CNTL.CNTL.bit.TRG ));
	Ddim_Print(( "IO_B2R_P1.F_B2R.CNTL.B2RINTE      = 0x%lx\n", IO_B2R_P1.F_B2R.CNTL.B2RINTE.word ));
	Ddim_Print(( "IO_B2R_P1.F_B2R.CNTL.B2RINTF      = 0x%lx\n", IO_B2R_P1.F_B2R.CNTL.B2RINTF.word ));
	Ddim_Print(( "IO_B2R_P1.F_B2R.CNTL.FLTAEN.CIAEN = 0x%x\n",  IO_B2R_P1.F_B2R.CNTL.FLTAEN.bit.CIAEN ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P2.F_B2R.CNTL.CNTL.bit.TRG = %u\n",    IO_B2R_P2.F_B2R.CNTL.CNTL.bit.TRG ));
	Ddim_Print(( "IO_B2R_P2.F_B2R.CNTL.B2RINTE      = 0x%lx\n", IO_B2R_P2.F_B2R.CNTL.B2RINTE.word ));
	Ddim_Print(( "IO_B2R_P2.F_B2R.CNTL.B2RINTF      = 0x%lx\n", IO_B2R_P2.F_B2R.CNTL.B2RINTF.word ));
	Ddim_Print(( "IO_B2R_P2.F_B2R.CNTL.FLTAEN.CIAEN = 0x%x\n",  IO_B2R_P2.F_B2R.CNTL.FLTAEN.bit.CIAEN ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}


static VOID ct_im_b2r_print_axi_reg( VOID )
{
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBRAXCTL.bit.YBRCACHE   = %u\n", IO_B2R_P1.YBCH.YBRAXCTL.bit.YBRCACHE ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBRAXCTL.bit.YBRPROT    = %u\n", IO_B2R_P1.YBCH.YBRAXCTL.bit.YBRPROT ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_0 = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_0 ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_0  = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_0 ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_1 = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_1 ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_1  = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_1 ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_2 = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_2 ));
	Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_2  = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_2 ));
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBRAXCTL.bit.YBRCACHE   = %u\n", IO_B2R_P2.YBCH.YBRAXCTL.bit.YBRCACHE ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBRAXCTL.bit.YBRPROT    = %u\n", IO_B2R_P2.YBCH.YBRAXCTL.bit.YBRPROT ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_0 = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_0 ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_0  = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_0 ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_1 = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_1 ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_1  = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_1 ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_2 = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_2 ));
	Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_2  = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_2 ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}

VOID CT_Im_B2r_PrintReg( VOID )
{
	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_1: "
INT32 CT_Im_B2r_1_1( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	return Im_B2R_Init( pipe_no );
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_2: "
INT32 CT_Im_B2r_1_2( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Pclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_3: "
INT32 CT_Im_B2r_1_3( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	Im_B2R_On_Pclk( pipe_no );

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_Off_Pclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_4: "
INT32 CT_Im_B2r_1_4( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Pclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Pclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Pclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_Force_Off_Pclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_5: "
INT32 CT_Im_B2r_1_5( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Hclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_Off_Hclk( pipe_no );

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_6: "
INT32 CT_Im_B2r_1_6( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	Im_B2R_On_Hclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_Off_Hclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_7: "
INT32 CT_Im_B2r_1_7( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Hclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Hclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Hclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_Force_Off_Hclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_8: "
INT32 CT_Im_B2r_1_8( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Iclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_Off_Iclk( pipe_no );

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_9: "
INT32 CT_Im_B2r_1_9( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	Im_B2R_On_Iclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif


	Im_B2R_Off_Iclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_10: "
INT32 CT_Im_B2r_1_10( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Iclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Iclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Iclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_Force_Off_Iclk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_11: "
INT32 CT_Im_B2r_1_11( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Clk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_Off_Clk( pipe_no );

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_12: "
INT32 CT_Im_B2r_1_12( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	Im_B2R_On_Clk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_Off_Clk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_13: "
INT32 CT_Im_B2r_1_13( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Clk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Clk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_On_Clk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	Im_B2R_Force_Off_Clk( pipe_no );
#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_ClockStatus();
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_14: "
INT32 CT_Im_B2r_1_14( UCHAR pipe_no )
{
	UINT32				loopcnt;
	T_IM_B2R_CTRL_AXI	b2r_ctrl_axi[] = {
		[0] = {	// max
			.ybr.cache_type = 0xF,
			.ybr.protect_type = 7,
			.ybw[0].cache_type = 0xF,
			.ybw[0].protect_type = 7,
			.ybw[1].cache_type = 0xF,
			.ybw[1].protect_type = 7,
			.ybw[2].cache_type = 0xF,
			.ybw[2].protect_type = 7,
		},
		[1] = {	// min
			.ybr.cache_type = 0,
			.ybr.protect_type = 0,
			.ybw[0].cache_type = 0,
			.ybw[0].protect_type = 0,
			.ybw[1].cache_type = 0,
			.ybw[1].protect_type = 0,
			.ybw[2].cache_type = 0,
			.ybw[2].protect_type = 0,
		},
	};
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(b2r_ctrl_axi) / sizeof(b2r_ctrl_axi[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Ctrl_Axi( pipe_no, &b2r_ctrl_axi[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
		Im_B2R_Ctrl_Axi( pipe_no, &b2r_ctrl_axi[loopcnt] );
#endif

		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( "IO_B2R_P1.YBCH.YBRAXCTL.bit.YBRCACHE   = %u\n", IO_B2R_P1.YBCH.YBRAXCTL.bit.YBRCACHE ));
		Ddim_Print(( "IO_B2R_P1.YBCH.YBRAXCTL.bit.YBRPROT    = %u\n", IO_B2R_P1.YBCH.YBRAXCTL.bit.YBRPROT ));

		Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_0 = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_0 ));
		Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_0  = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_0 ));
		Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_1 = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_1 ));
		Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_1  = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_1 ));
		Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_2 = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWCACHE_2 ));
		Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_2  = %u\n", IO_B2R_P1.YBCH.YBWAXCTL.bit.YBWPROT_2 ));

		Ddim_Print(( "IO_B2R_P2.YBCH.YBRAXCTL.bit.YBRCACHE   = %u\n", IO_B2R_P2.YBCH.YBRAXCTL.bit.YBRCACHE ));
		Ddim_Print(( "IO_B2R_P2.YBCH.YBRAXCTL.bit.YBRPROT    = %u\n", IO_B2R_P2.YBCH.YBRAXCTL.bit.YBRPROT ));

		Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_0 = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_0 ));
		Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_0  = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_0 ));
		Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_1 = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_1 ));
		Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_1  = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_1 ));
		Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_2 = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWCACHE_2 ));
		Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_2  = %u\n", IO_B2R_P2.YBCH.YBWAXCTL.bit.YBWPROT_2 ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_15: "
INT32 CT_Im_B2r_1_15( UCHAR pipe_no )
{
	T_IM_B2R_AXI_YBR_STAT b2r_axi_read_stat;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


#ifdef CO_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		IO_B2R_P1.YBCH.YBRAXSTS.bit.YBRRESP = 3;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		IO_B2R_P2.YBCH.YBRAXSTS.bit.YBRRESP = 3;
	}
#endif // CO_DEBUG_ON_PC
	memset( &b2r_axi_read_stat, '\0', sizeof(b2r_axi_read_stat) );
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_AxiReadStat( pipe_no, &b2r_axi_read_stat );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
	Im_B2R_Get_AxiReadStat( pipe_no, &b2r_axi_read_stat );
#endif
	Ddim_Print(( "ybr_axi_stat = %u\n", b2r_axi_read_stat.ybr_axi_stat ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		IO_B2R_P1.YBCH.YBRAXSTS.bit.YBRRESP = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		IO_B2R_P2.YBCH.YBRAXSTS.bit.YBRRESP = 0;
	}
#endif // CO_DEBUG_ON_PC
	memset( &b2r_axi_read_stat, '\0', sizeof(b2r_axi_read_stat) );
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_AxiReadStat( pipe_no, &b2r_axi_read_stat );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
	Im_B2R_Get_AxiReadStat( pipe_no, &b2r_axi_read_stat );
#endif
	Ddim_Print(( "ybr_axi_stat = %u\n", b2r_axi_read_stat.ybr_axi_stat ));

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_16: "
INT32 CT_Im_B2r_1_16( UCHAR pipe_no )
{
	T_IM_B2R_AXI_YBW_STAT b2r_axi_write_stat;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		IO_B2R_P1.YBCH.YBWAXSTS.bit.YBBRESP_0 = 3;
		IO_B2R_P1.YBCH.YBWAXSTS.bit.YBBRESP_1 = 3;
		IO_B2R_P1.YBCH.YBWAXSTS.bit.YBBRESP_2 = 3;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		IO_B2R_P2.YBCH.YBWAXSTS.bit.YBBRESP_0 = 3;
		IO_B2R_P2.YBCH.YBWAXSTS.bit.YBBRESP_1 = 3;
		IO_B2R_P2.YBCH.YBWAXSTS.bit.YBBRESP_2 = 3;
	}
#endif //CO_DEBUG_ON_PC

	memset( &b2r_axi_write_stat, '\0', sizeof(b2r_axi_write_stat) );
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_AxiWriteStat( pipe_no, &b2r_axi_write_stat );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
	Im_B2R_Get_AxiWriteStat( pipe_no, &b2r_axi_write_stat );
#endif

	Ddim_Print(( "ybw_axi_stat[0] = %u\n", b2r_axi_write_stat.ybw_axi_stat[D_IM_B2R_PORT_0] ));
	Ddim_Print(( "ybw_axi_stat[1] = %u\n", b2r_axi_write_stat.ybw_axi_stat[D_IM_B2R_PORT_1] ));
	Ddim_Print(( "ybw_axi_stat[2] = %u\n", b2r_axi_write_stat.ybw_axi_stat[D_IM_B2R_PORT_2] ));




#ifdef CO_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		IO_B2R_P1.YBCH.YBWAXSTS.bit.YBBRESP_0 = 0;
		IO_B2R_P1.YBCH.YBWAXSTS.bit.YBBRESP_1 = 0;
		IO_B2R_P1.YBCH.YBWAXSTS.bit.YBBRESP_2 = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		IO_B2R_P2.YBCH.YBWAXSTS.bit.YBBRESP_0 = 0;
		IO_B2R_P2.YBCH.YBWAXSTS.bit.YBBRESP_1 = 0;
		IO_B2R_P2.YBCH.YBWAXSTS.bit.YBBRESP_2 = 0;
	}
#endif //CO_DEBUG_ON_PC

	memset( &b2r_axi_write_stat, '\0', sizeof(b2r_axi_write_stat) );
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_AxiWriteStat( pipe_no, &b2r_axi_write_stat );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
	Im_B2R_Get_AxiWriteStat( pipe_no, &b2r_axi_write_stat );
#endif

	Ddim_Print(( "ybw_axi_stat[0] = %u\n", b2r_axi_write_stat.ybw_axi_stat[D_IM_B2R_PORT_0] ));
	Ddim_Print(( "ybw_axi_stat[1] = %u\n", b2r_axi_write_stat.ybw_axi_stat[D_IM_B2R_PORT_1] ));
	Ddim_Print(( "ybw_axi_stat[2] = %u\n", b2r_axi_write_stat.ybw_axi_stat[D_IM_B2R_PORT_2] ));

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_17: "
INT32 CT_Im_B2r_1_17( UCHAR pipe_no )
{
	T_IM_B2R_AXI_YBW_MODE	b2r_axi_write_mode;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	if( pipe_no == D_IM_B2R_PIPE12 ){
		return D_DDIM_OK;
	}

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		IO_B2R_P1.YBCH.YBWAXMD.bit.YBWDOS = 1;
		IO_B2R_P1.YBCH.YBWAXMD.bit.YBWBL = 2;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		IO_B2R_P2.YBCH.YBWAXMD.bit.YBWDOS = 1;
		IO_B2R_P2.YBCH.YBWAXMD.bit.YBWBL = 2;
	}

	memset( &b2r_axi_write_mode, '\0', sizeof(b2r_axi_write_mode) );
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_AxiWriteMode( pipe_no, &b2r_axi_write_mode );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
	Im_B2R_Get_AxiWriteMode( pipe_no, &b2r_axi_write_mode );
#endif

	Ddim_Print(( "out_sel      = %u\n", b2r_axi_write_mode.out_sel ));
	Ddim_Print(( "burst_length = %u\n", b2r_axi_write_mode.burst_length ));


	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		IO_B2R_P1.YBCH.YBWAXMD.bit.YBWDOS = 0;
		IO_B2R_P1.YBCH.YBWAXMD.bit.YBWBL = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		IO_B2R_P2.YBCH.YBWAXMD.bit.YBWDOS = 0;
		IO_B2R_P2.YBCH.YBWAXMD.bit.YBWBL = 0;
	}

	memset( &b2r_axi_write_mode, '\0', sizeof(b2r_axi_write_mode) );
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_AxiWriteMode( pipe_no, &b2r_axi_write_mode );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
	Im_B2R_Get_AxiWriteMode( pipe_no, &b2r_axi_write_mode );
#endif

	Ddim_Print(( "out_sel      = %u\n", b2r_axi_write_mode.out_sel ));
	Ddim_Print(( "burst_length = %u\n", b2r_axi_write_mode.burst_length ));

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_18: "
INT32 CT_Im_B2r_1_18( UCHAR pipe_no )
{
	UINT32					loopcnt;
	T_IM_B2R_AXI_YBW_MODE	b2r_axi_write_mode[] = {
		[0] = { // max
			.out_sel = 1,
			.burst_length = 2,
		},
		[1] = { // min
			.out_sel = 0,
			.burst_length = 0,
		},
	};
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(b2r_axi_write_mode) / sizeof(b2r_axi_write_mode[0])); loopcnt++ ) {
#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Set_AxiWriteMode( pipe_no, &b2r_axi_write_mode[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
		Im_B2R_Set_AxiWriteMode( pipe_no, b2r_axi_write_mode[loopcnt] );
#endif

		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXMD.bit.YBWDOS = 0x%x\n", IO_B2R_P1.YBCH.YBWAXMD.bit.YBWDOS ));
		Ddim_Print(( "IO_B2R_P1.YBCH.YBWAXMD.bit.YBWBL  = 0x%x\n", IO_B2R_P1.YBCH.YBWAXMD.bit.YBWBL ));
		Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXMD.bit.YBWDOS = 0x%x\n", IO_B2R_P2.YBCH.YBWAXMD.bit.YBWDOS ));
		Ddim_Print(( "IO_B2R_P2.YBCH.YBWAXMD.bit.YBWBL  = 0x%x\n", IO_B2R_P2.YBCH.YBWAXMD.bit.YBWBL ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_19: "
INT32 CT_Im_B2r_1_19( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(gct_im_b2r_ctrl) / sizeof(gct_im_b2r_ctrl[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
		Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl[loopcnt] );
#endif

		ct_im_b2r_print_b2r_ctrl_reg();

#ifdef IM_B2R_STATUS_PRINT
		Im_B2R_Print_Status();
#endif
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_20: "
INT32 CT_Im_B2r_1_20( UCHAR pipe_no )
{
	UINT32 loopcnt;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


	for( loopcnt = 0; loopcnt < (sizeof(gct_im_b2r_ctrl_sdram_in) / sizeof(gct_im_b2r_ctrl_sdram_in[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));

		ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
		Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
		Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in[loopcnt] );
#endif

		ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();

#ifdef IM_B2R_STATUS_PRINT
		Im_B2R_Print_Status();
#endif
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_21: "
INT32 CT_Im_B2r_1_21( UCHAR pipe_no )
{
	UINT32 loopcnt;
	T_IM_B2R_CTRL_B2B_DIRECT b2r_ctrl_b2b_direct[] = {
		[0] = {	// max
			.even_line_start_timming = 0x1FFF,
			.frame_stop = 1,
		},
		[1] = {	// min
			.even_line_start_timming = 0,
			.frame_stop = 0,
		},
	};
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < (sizeof(b2r_ctrl_b2b_direct) / sizeof(b2r_ctrl_b2b_direct[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));

		ercd = Im_B2R_Ctrl_ModeB2BDirect( pipe_no, &b2r_ctrl_b2b_direct[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
		Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
		Im_B2R_Ctrl_ModeB2BDirect( pipe_no, &b2r_ctrl_b2b_direct[loopcnt] );
#endif

		ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();

#ifdef IM_B2R_STATUS_PRINT
		Im_B2R_Print_Status();
#endif
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_22: "
INT32 CT_Im_B2r_1_22( UCHAR pipe_no )
{
	UINT32 loopcnt;
	T_IM_B2R_INADDR_INFO b2r_in_addr[] = {
		[0] = { // max
			.raw.addr = (VOID*)0xFFFFFFFF,
		},
		[1] = { // min
			.raw.addr = (VOID*)0,
		},
		[2] = { // indvisual
			.raw.addr = (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP,
		},
	};
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


	for( loopcnt = 0; loopcnt < (sizeof(b2r_in_addr) / sizeof(b2r_in_addr[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));

		ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));

		ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
		Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
		Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
		Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr[loopcnt] );
#endif

		ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();

		// im_b2r_calc_ybr_address_sdram_input(), im_b2r_set_ybr_address_sdram_input(), Im_B2R_Set_InAddr_Info()
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( "IO_B2R_P1.YBR.YBRA = %lx\n", IO_B2R_P1.YBR.YBRA.bit.YBRA ));
		Ddim_Print(( "IO_B2R_P2.YBR.YBRA = %lx\n", IO_B2R_P2.YBR.YBRA.bit.YBRA ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );

#ifdef IM_B2R_STATUS_PRINT
		Im_B2R_Print_Status();
#endif
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_23: "
INT32 CT_Im_B2r_1_23( UCHAR pipe_no )
{
	T_IM_B2R_OUTBANK_INFO b2r_addr;
	UINT32 loopcnt;
	UINT32 ybw_bank_no;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	if( pipe_no == D_IM_B2R_PIPE12 ){
		return D_DDIM_OK;
	}

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	for( loopcnt = 0; loopcnt < 3; loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));


		switch( loopcnt ) {
			case 0:	// max
				b2r_addr.bank_initial_position = D_IM_B2R_YBW_BANK_3;
				b2r_addr.use_bank_num = D_IM_B2R_YBW_BANK_3;
				for( ybw_bank_no = D_IM_B2R_YBW_BANK_0; ybw_bank_no < D_IM_B2R_YBW_BANK_MAX; ybw_bank_no++ ) {
					b2r_addr.output_addr[ybw_bank_no].rgb.addr_R = (VOID*)0xFFFFFFFF;
					b2r_addr.output_addr[ybw_bank_no].rgb.addr_G = (VOID*)0xFFFFFFFF;
					b2r_addr.output_addr[ybw_bank_no].rgb.addr_B = (VOID*)0xFFFFFFFF;
				}
				break;
			case 1:	// min
				b2r_addr.bank_initial_position = D_IM_B2R_YBW_BANK_0;
				b2r_addr.use_bank_num = D_IM_B2R_YBW_BANK_0;
				for( ybw_bank_no = D_IM_B2R_YBW_BANK_0; ybw_bank_no < D_IM_B2R_YBW_BANK_MAX; ybw_bank_no++ ) {
					b2r_addr.output_addr[ybw_bank_no].rgb.addr_R = (VOID*)0;
					b2r_addr.output_addr[ybw_bank_no].rgb.addr_G = (VOID*)0;
					b2r_addr.output_addr[ybw_bank_no].rgb.addr_B = (VOID*)0;
				}
				break;
			default:
//			case 2:	// indivisual
				b2r_addr.bank_initial_position = D_IM_B2R_YBW_BANK_1;
				b2r_addr.use_bank_num = D_IM_B2R_YBW_BANK_2;
				for( ybw_bank_no = D_IM_B2R_YBW_BANK_0; ybw_bank_no < D_IM_B2R_YBW_BANK_MAX; ybw_bank_no++ ) {
					b2r_addr.output_addr[ybw_bank_no].rgb.addr_R = (VOID*)0x1FFFFFF0;
					b2r_addr.output_addr[ybw_bank_no].rgb.addr_G = (VOID*)0x2FFFFFF0;
					b2r_addr.output_addr[ybw_bank_no].rgb.addr_B = (VOID*)0x3FFFFFF0;
				}
				break;
		}

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));

		ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));

		ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_addr );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
		Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
		Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
		Im_B2R_Set_OutBankInfo( pipe_no, &b2r_addr );
#endif

		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( "IO_B2R_P1.YBW.YBWCTL.bit.USEBANK = %u\n",   (UINT32)IO_B2R_P1.YBW.YBWCTL.bit.USEBANK ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_0_0  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_0_0 ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_1_0  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_1_0 ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_2_0  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_2_0 ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_0_1  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_0_1 ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_1_1  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_1_1 ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_2_1  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_2_1 ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_0_2  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_0_2 ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_1_2  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_1_2 ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_2_2  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_2_2 ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_0_3  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_0_3 ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_1_3  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_1_3 ));
		Ddim_Print(( "IO_B2R_P1.YBW.YBWA.bit.YBWA_2_3  = 0x%x\n", (UINT32)IO_B2R_P1.YBW.YBWA.bit.YBWA_2_3 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWCTL.bit.USEBANK = %u\n",   (UINT32)IO_B2R_P2.YBW.YBWCTL.bit.USEBANK ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_0_0  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_0_0 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_1_0  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_1_0 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_2_0  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_2_0 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_0_1  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_0_1 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_1_1  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_1_1 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_2_1  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_2_1 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_0_2  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_0_2 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_1_2  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_1_2 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_2_2  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_2_2 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_0_3  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_0_3 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_1_3  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_1_3 ));
		Ddim_Print(( "IO_B2R_P2.YBW.YBWA.bit.YBWA_2_3  = 0x%x\n", (UINT32)IO_B2R_P2.YBW.YBWA.bit.YBWA_2_3 ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );


#ifdef IM_B2R_STATUS_PRINT
		Im_B2R_Print_Status();
#endif
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_24: "
INT32 CT_Im_B2r_1_24( UCHAR pipe_no )
{
	T_IM_B2R_RECT b2r_rect_param;
	T_IM_B2R_INADDR_INFO b2r_in_addr;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	UINT32 loopcnt;
	UCHAR bank_index;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	if( pipe_no == D_IM_B2R_PIPE12 ){
		return D_DDIM_OK;
	}

	b2r_rect_param.input_size.img_top = 0;
	b2r_rect_param.input_size.img_left = 0;
	b2r_rect_param.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH;
	b2r_rect_param.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES;
	b2r_rect_param.output_size.output_global_w = D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U8;

	b2r_in_addr.raw.addr = (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP;

	memset( &b2r_out_addr, '\x0', sizeof(b2r_out_addr) );
	b2r_out_addr.bank_initial_position = D_IM_B2R_YBW_BANK_3;
	b2r_out_addr.use_bank_num = D_IM_B2R_YBW_BANK_3;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP + 0x0);
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP + 0x0);
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP + 0x0);
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_R = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP + 0x4);
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_G = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP + 0x4);
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_B = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP + 0x4);
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_R = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP + 0x8);
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_G = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP + 0x8);
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_B = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP + 0x8);
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_R = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP + 0xC);
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_G = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP + 0xC);
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_B = (VOID*)(D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP + 0xC);

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "Status\n" ));
	Im_B2R_Print_Status();
#endif

	for( loopcnt = 0; loopcnt < D_IM_B2R_YBW_BANK_MAX; loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef D_IM_B2R_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			gIM_B2R_macro_fake_finish[0] = 1;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			gIM_B2R_macro_fake_finish[1] = 1;
		}
#endif
		Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));


#ifdef CO_MSG_PRINT_ON
		Ddim_Print(( D_IM_B2R_FUNC_NAME "loopcnt = %u\n", loopcnt ));
		ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
		Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			gIM_B2R_macro_fake_finish[0] = 0;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			gIM_B2R_macro_fake_finish[1] = 0;
		}
#endif

		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
		}
#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

		ercd = Im_B2R_Get_OutBankIndex( pipe_no, &bank_index );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d bank_idx=%u\n", ercd, bank_index ));

		ercd = Im_B2R_Stop( pipe_no );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
		Im_B2R_WaitEnd( NULL, flgptn, 60 );
		Im_B2R_Get_OutBankIndex( pipe_no, &bank_index );
		Im_B2R_Stop( pipe_no );
#endif


#ifdef IM_B2R_STATUS_PRINT
		Im_B2R_Print_Status();
#endif
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_25: "
INT32 CT_Im_B2r_1_25( UCHAR pipe_no )
{
	UINT32 loopcnt;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

#ifdef CO_MSG_PRINT_ON
	for( loopcnt = D_IM_B2R_YBW_BANK_0; loopcnt < D_IM_B2R_YBW_BANK_MAX; loopcnt++ ) {
		Ddim_Print(( D_IM_B2R_FUNC_NAME "##loop %u\n", loopcnt ));
		ercd = Im_B2R_Set_OutBankIndex( pipe_no, loopcnt );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.YBW.YBWBANK.bit.BANKST = %u\n", IO_B2R_P1.YBW.YBWBANK.bit.BANKST ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.YBW.YBWBANK.bit.BANKPS = %u\n", IO_B2R_P1.YBW.YBWBANK.bit.BANKPS ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.YBW.YBWBANK.bit.BANKST = %u\n", IO_B2R_P2.YBW.YBWBANK.bit.BANKST ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.YBW.YBWBANK.bit.BANKPS = %u\n", IO_B2R_P2.YBW.YBWBANK.bit.BANKPS ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
	}
#endif
	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_26: "
INT32 CT_Im_B2r_1_26( UCHAR pipe_no )
{
	UINT32 loopcnt;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;

	memset( &b2r_out_addr, '\x0', sizeof(b2r_out_addr) );
	b2r_out_addr.bank_initial_position = D_IM_B2R_YBW_BANK_3;
	b2r_out_addr.use_bank_num = D_IM_B2R_YBW_BANK_3;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	for( loopcnt = D_IM_B2R_YBW_BANK_0; loopcnt < D_IM_B2R_YBW_BANK_MAX; loopcnt++ ) {
		Ddim_Print(( D_IM_B2R_FUNC_NAME "##loop %u\n", loopcnt ));

#ifdef CO_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			IO_B2R_P1.YBW.YBWBANK.bit.BANKSTS = loopcnt;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			IO_B2R_P2.YBW.YBWBANK.bit.BANKSTS = loopcnt;
		}
#endif //CO_DEBUG_ON_PC

		ercd = Im_B2R_Inc_OutBankIndex( pipe_no );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.YBW.YBWBANK.bit.BANKPS = %u\n", IO_B2R_P1.YBW.YBWBANK.bit.BANKPS ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.YBW.YBWBANK.bit.BANKST = %u\n", IO_B2R_P1.YBW.YBWBANK.bit.BANKST ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.YBW.YBWBANK.bit.BANKPS = %u\n", IO_B2R_P2.YBW.YBWBANK.bit.BANKPS ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.YBW.YBWBANK.bit.BANKST = %u\n", IO_B2R_P2.YBW.YBWBANK.bit.BANKST ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
	}
#endif


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_27: "
INT32 CT_Im_B2r_1_27( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_base;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_base;
	T_IM_B2R_RECT b2r_rect_param;
	T_IM_B2R_INADDR_INFO b2r_in_addr = {
		.raw.addr = (VOID*)0x10000000,		// RAW
	};
	const UCHAR input_dtype[4] = {
		D_IM_B2R_STL_DTYP_PACK8,
		D_IM_B2R_STL_DTYP_PACK10,
		D_IM_B2R_STL_DTYP_PACK12,
		D_IM_B2R_STL_DTYP_PACK16
	};

#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));




	for( loopcnt = 0; loopcnt < 4; loopcnt++ ) {
		Ddim_Print(( "## %u\n", loopcnt ));

		b2r_ctrl_sdram_in.input_dtype = input_dtype[loopcnt];

		b2r_rect_param.input_size.img_top = 180;
		b2r_rect_param.input_size.img_left = 320;
		b2r_rect_param.input_size.img_width = 1920 - b2r_rect_param.input_size.img_left *2;
		b2r_rect_param.input_size.img_lines = 1080 - b2r_rect_param.input_size.img_top *2;
		b2r_rect_param.output_size.output_global_w = 0x1000;

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));

		ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));

		ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));

		ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
#else
		Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
		Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
		Im_B2R_Set_Rect( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
		Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
#endif

		ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
		ct_im_b2r_print_rect_reg();

#ifdef IM_B2R_STATUS_PRINT
		Im_B2R_Print_Status();
#endif
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_28: "
INT32 CT_Im_B2r_1_28( UCHAR pipe_no )
{
	T_IM_B2R_RECT b2r_rect_param;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	if( pipe_no == D_IM_B2R_PIPE12 ){
		return D_DDIM_OK;
	}

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	ercd = CT_Im_B2r_1_27( pipe_no );
	if( ercd != D_DDIM_OK ) {
		return ercd;
	}

#ifdef CO_MSG_PRINT_ON
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		ercd = Im_B2R_Get_Rect_Param( 0, &b2r_rect_param );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));

		Ddim_Print(( "###pipe_no      = %u\n", 0 ));
		Ddim_Print(( "img_top         = %u\n", b2r_rect_param.input_size.img_top ));
		Ddim_Print(( "img_left        = %u\n", b2r_rect_param.input_size.img_left ));
		Ddim_Print(( "img_width       = %u\n", b2r_rect_param.input_size.img_width ));
		Ddim_Print(( "img_lines       = %u\n", b2r_rect_param.input_size.img_lines ));
		Ddim_Print(( "output_global_w = %u\n", b2r_rect_param.output_size.output_global_w ));
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		ercd = Im_B2R_Get_Rect_Param( 1, &b2r_rect_param );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));

		Ddim_Print(( "###pipe_no      = %u\n", 1 ));
		Ddim_Print(( "img_top         = %u\n", b2r_rect_param.input_size.img_top ));
		Ddim_Print(( "img_left        = %u\n", b2r_rect_param.input_size.img_left ));
		Ddim_Print(( "img_width       = %u\n", b2r_rect_param.input_size.img_width ));
		Ddim_Print(( "img_lines       = %u\n", b2r_rect_param.input_size.img_lines ));
		Ddim_Print(( "output_global_w = %u\n", b2r_rect_param.output_size.output_global_w ));
	}
#else
	Im_B2R_Get_Rect_Param( pipe_no, &b2r_rect_param );
#endif

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_Status();
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_29: "
INT32 CT_Im_B2r_1_29( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_base;
	T_IM_B2R_RECT b2r_rect_param;
	T_IM_B2R_INADDR_INFO b2r_in_addr;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	T_IM_B2R_OUTADDR_INFO out_addr_info;
	UINT32 loopcnt;
	UCHAR bank_index;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
	const char dumpmsg[] = "%d idx=%u addr y=0x%05lx cb=0x%05lx cr=0x%05lx\n";
#endif

	b2r_rect_param.input_size.img_top = 0;
	b2r_rect_param.input_size.img_left = 0;
	b2r_rect_param.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH;
	b2r_rect_param.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES;
	b2r_rect_param.output_size.output_global_w = D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U8;

	b2r_in_addr.raw.addr = (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP;

	memset( &b2r_out_addr, '\x0', sizeof(b2r_out_addr) );
	b2r_out_addr.bank_initial_position = D_IM_B2R_YBW_BANK_3;
	b2r_out_addr.use_bank_num = D_IM_B2R_YBW_BANK_3;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)0x100;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)0x200;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)0x300;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_R = (VOID*)0x400;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_G = (VOID*)0x500;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_B = (VOID*)0x600;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_R = (VOID*)0x700;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_G = (VOID*)0x800;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_B = (VOID*)0x900;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_R = (VOID*)0xa00;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_G = (VOID*)0xb00;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_B = (VOID*)0xc00;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "Status\n" ));
	Im_B2R_Print_Status();
#endif

	for( loopcnt = 0; loopcnt < D_IM_B2R_YBW_BANK_MAX; loopcnt++ ) {
#ifdef D_IM_B2R_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			gIM_B2R_macro_fake_finish[0] = 1;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			gIM_B2R_macro_fake_finish[1] = 1;
		}
#endif
		Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));


#ifdef CO_MSG_PRINT_ON
		Ddim_Print(( D_IM_B2R_FUNC_NAME "loopcnt = %u\n", loopcnt ));
		ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
		Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			gIM_B2R_macro_fake_finish[0] = 0;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			gIM_B2R_macro_fake_finish[1] = 0;
		}
#endif

		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
		}
#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			ercd = Im_B2R_Get_Latest_OutAddr( 0, &bank_index, &out_addr_info );
			Ddim_Print(( dumpmsg,
						ercd,
						bank_index,
						(ULONG)out_addr_info.rgb.addr_R,
						(ULONG)out_addr_info.rgb.addr_G,
						(ULONG)out_addr_info.rgb.addr_B
						));
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			ercd = Im_B2R_Get_Latest_OutAddr( 1, &bank_index, &out_addr_info );
			Ddim_Print(( dumpmsg,
						ercd,
						bank_index,
						(ULONG)out_addr_info.rgb.addr_R,
						(ULONG)out_addr_info.rgb.addr_G,
						(ULONG)out_addr_info.rgb.addr_B
						));
		}

		ercd = Im_B2R_Stop( pipe_no );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
		Im_B2R_WaitEnd( NULL, flgptn, 60 );
		Im_B2R_Get_OutBankIndex( pipe_no, &bank_index );
		Im_B2R_Stop( pipe_no );
#endif


#ifdef IM_B2R_STATUS_PRINT
//		Im_B2R_Print_Status();
#endif
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_30: "
INT32 CT_Im_B2r_1_30( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Set_FrameStop( pipe_no, 1 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.YBR.DINCTL.bit.FRMSTP = %u\n", IO_B2R_P1.YBR.DINCTL.bit.FRMSTP ));
	Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.YBR.DINCTL.bit.FRMSTP = %u\n", IO_B2R_P2.YBR.DINCTL.bit.FRMSTP ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
	ercd = Im_B2R_Set_FrameStop( pipe_no, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.YBR.DINCTL.bit.FRMSTP = %u\n", IO_B2R_P1.YBR.DINCTL.bit.FRMSTP ));
	Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.YBR.DINCTL.bit.FRMSTP = %u\n", IO_B2R_P2.YBR.DINCTL.bit.FRMSTP ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_31: "
INT32 CT_Im_B2r_1_31( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	BOOL busy_state;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 3;
		Im_B2R_Get_YbrBusy( pipe_no, &busy_state );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "pipe1:busy = %u\n", busy_state ));
		IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 2;
		Im_B2R_Get_YbrBusy( pipe_no, &busy_state );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "pipe1:busy = %u\n", busy_state ));
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 3;
		Im_B2R_Get_YbrBusy( pipe_no, &busy_state );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "pipe2:busy = %u\n", busy_state ));
		IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 2;
		Im_B2R_Get_YbrBusy( pipe_no, &busy_state );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "pipe2:busy = %u\n", busy_state ));
	}
#endif


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_32: "
INT32 CT_Im_B2r_1_32( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
	USHORT ring_pixs;
#endif
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_base;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.cia_bypass = 0;
	b2r_ctrl.cia_padding = 0;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "%d\n", ercd ));
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		ercd = Im_B2R_Get_HRingPixs( 0, &ring_pixs );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "ring = %u %d\n", ring_pixs, ercd ));
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		ercd = Im_B2R_Get_HRingPixs( 1, &ring_pixs );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "ring = %u %d\n", ring_pixs, ercd ));
	}
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_33: "
INT32 CT_Im_B2r_1_33( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	UINT32 err_factor;
	INT32 ercd;
#endif
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		IO_B2R_P1.YBR.DINERR.bit.TRMER = 1;
		IO_B2R_P1.YBR.DINERR.bit.HPDDER = 1;
		IO_B2R_P1.YBR.DINERR.bit.WAITER = 1;
		ercd = Im_B2R_Get_YBR_Error_Factor( 0, &err_factor );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "error factor = %x %d\n", err_factor, ercd ));
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		IO_B2R_P2.YBR.DINERR.bit.TRMER = 1;
		IO_B2R_P2.YBR.DINERR.bit.HPDDER = 1;
		IO_B2R_P2.YBR.DINERR.bit.WAITER = 1;
		ercd = Im_B2R_Get_YBR_Error_Factor( 1, &err_factor );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "error factor = %x %d\n", err_factor, ercd ));
	}
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_34: "
INT32 CT_Im_B2r_1_34( UCHAR pipe_no )
{
	T_IM_B2R_CTRL_TRIMMING b2r_ctrl_trimming[] = {
		[0] = {	// max
			.trimming_enable = 1,
			.start_x = 0x1FFF,
			.width = 0x1FFF,
			.start_y = 0x3FFF,
			.lines = 0x3FFF,
		},
		[1] = {	// min
			.trimming_enable = 0,
			.start_x = 0,
			.width = 0,
			.start_y = 0,
			.lines = 0,
		},
	};
	UINT32 loopcnt;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


#ifdef CO_MSG_PRINT_ON
	for( loopcnt = 0; loopcnt < (sizeof(b2r_ctrl_trimming) / sizeof(b2r_ctrl_trimming[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));
		ercd = Im_B2R_Ctrl_Trimming( pipe_no, &b2r_ctrl_trimming[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.YBW.YBWMD.bit.TRMEN     = %u\n", IO_B2R_P1.YBW.YBWMD.bit.TRMEN ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.YBW.YBWTRMH.bit.TRMHSTA = %u\n", IO_B2R_P1.YBW.YBWTRMH.bit.TRMHSTA ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.YBW.YBWTRMH.bit.TRMHSIZ = %u\n", IO_B2R_P1.YBW.YBWTRMH.bit.TRMHSIZ ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.YBW.YBWTRMV.bit.TRMVSTA = %u\n", IO_B2R_P1.YBW.YBWTRMV.bit.TRMVSTA ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.YBW.YBWTRMV.bit.TRMVSIZ = %u\n", IO_B2R_P1.YBW.YBWTRMV.bit.TRMVSIZ ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.YBW.YBWMD.bit.TRMEN     = %u\n", IO_B2R_P2.YBW.YBWMD.bit.TRMEN ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.YBW.YBWTRMH.bit.TRMHSTA = %u\n", IO_B2R_P2.YBW.YBWTRMH.bit.TRMHSTA ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.YBW.YBWTRMH.bit.TRMHSIZ = %u\n", IO_B2R_P2.YBW.YBWTRMH.bit.TRMHSIZ ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.YBW.YBWTRMV.bit.TRMVSTA = %u\n", IO_B2R_P2.YBW.YBWTRMV.bit.TRMVSTA ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.YBW.YBWTRMV.bit.TRMVSIZ = %u\n", IO_B2R_P2.YBW.YBWTRMV.bit.TRMVSIZ ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
	}
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_35: "
INT32 CT_Im_B2r_1_35( UCHAR pipe_no )
{
#ifdef CO_DEBUG_ON_PC
	struct t_ct_im_b2r_1_35 {
		USHORT src_tbl[D_IM_B2R_TABLE_MAX_BAYER_DEKNEE];
	};
	struct t_ct_im_b2r_1_35 tbl_body;
	struct t_ct_im_b2r_1_35* tbl = &tbl_body;
#else //!CO_DEBUG_ON_PC
	struct t_ct_im_b2r_1_35 {
		USHORT src_tbl[D_IM_B2R_TABLE_MAX_BAYER_DEKNEE];
	}* tbl = (struct t_ct_im_b2r_1_35*)D_IM_B2R_IMG_MEM_IN_ADDR_TOP;
#endif //!CO_DEBUG_ON_PC
	UINT32			loopcnt;
	UINT32			loopcnt2;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	const char errmsg[] = "%u unmatch\n";

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


	for( loopcnt = 0; loopcnt < 3; loopcnt++ ) {
		switch( loopcnt ) {
			default:	// max
//			case 0:
				for( loopcnt2 = 0; loopcnt2 < D_IM_B2R_TABLE_MAX_BAYER_DEKNEE; loopcnt2++ ) {
					tbl->src_tbl[loopcnt2] = 0xFFF;
				}
				break;
			case 1:	// min
				for( loopcnt2 = 0; loopcnt2 < D_IM_B2R_TABLE_MAX_BAYER_DEKNEE; loopcnt2++ ) {
					tbl->src_tbl[loopcnt2] = 0;
				}
				break;
			case 2:	// indvisual
				for( loopcnt2 = 0; loopcnt2 < D_IM_B2R_TABLE_MAX_BAYER_DEKNEE; loopcnt2++ ) {
					tbl->src_tbl[loopcnt2] = 0xFFF - loopcnt2;
				}
				break;
		}


#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Set_Deknee_Table( pipe_no, tbl->src_tbl );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Hclk( pipe_no );
		Im_B2R_Set_DekneeAccessEnable( pipe_no, D_IM_B2R_ENABLE_ON, D_IM_B2R_WAIT_ON );
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			for( loopcnt2 = 0; loopcnt2 < D_IM_B2R_TABLE_MAX_BAYER_DEKNEE; loopcnt2++ ) {
				if( IO_B2R_TBL_P1.DKNTBL.hword[loopcnt2] != tbl->src_tbl[loopcnt2] ) {
					Ddim_Print(( errmsg, loopcnt2 ));
				}
			}
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			for( loopcnt2 = 0; loopcnt2 < D_IM_B2R_TABLE_MAX_BAYER_DEKNEE; loopcnt2++ ) {
				if( IO_B2R_TBL_P2.DKNTBL.hword[loopcnt2] != tbl->src_tbl[loopcnt2] ) {
					Ddim_Print(( errmsg, loopcnt2 ));
				}
			}
		}
		Im_B2R_Set_DekneeAccessEnable( pipe_no, D_IM_B2R_ENABLE_OFF, D_IM_B2R_WAIT_OFF );
		Im_B2R_Off_Hclk( pipe_no );
#else
		Im_B2R_Set_Deknee_Table( pipe_no, tbl->src_tbl );
#endif
	}


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_36: "
INT32 CT_Im_B2r_1_36( UCHAR pipe_no )
{
	UCHAR access_enable[] = {
		[0] = 1,	// max
		[1] = 0,	// min
	};
	UINT32 loopcnt;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	Im_B2R_Init( D_IM_B2R_PIPE12 );
	Ddim_Print(( "Initial value\n" ));
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.RAMAEN.word                  = 0x%lx\n", IO_B2R_P1.B2R_CMN.RAMAEN.word ));
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.RAMAEN.bit.RAMAEN_BAY_DEKNEE = %u\n",    IO_B2R_P1.B2R_CMN.RAMAEN.bit.RAMAEN_BAY_DEKNEE ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.RAMAEN.word                  = 0x%lx\n", IO_B2R_P2.B2R_CMN.RAMAEN.word ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.RAMAEN.bit.RAMAEN_BAY_DEKNEE = %u\n",    IO_B2R_P2.B2R_CMN.RAMAEN.bit.RAMAEN_BAY_DEKNEE ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif

	for( loopcnt = 0; loopcnt < (sizeof(access_enable) / sizeof(access_enable[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Set_DekneeAccessEnable( pipe_no, access_enable[loopcnt], D_IM_B2R_WAIT_ON );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( "IO_B2R_P1.B2R_CMN.RAMAEN.word                  = 0x%lx\n", IO_B2R_P1.B2R_CMN.RAMAEN.word ));
		Ddim_Print(( "IO_B2R_P1.B2R_CMN.RAMAEN.bit.RAMAEN_BAY_DEKNEE = %u\n",    IO_B2R_P1.B2R_CMN.RAMAEN.bit.RAMAEN_BAY_DEKNEE ));
		Ddim_Print(( "IO_B2R_P2.B2R_CMN.RAMAEN.word                  = 0x%lx\n", IO_B2R_P2.B2R_CMN.RAMAEN.word ));
		Ddim_Print(( "IO_B2R_P2.B2R_CMN.RAMAEN.bit.RAMAEN_BAY_DEKNEE = %u\n",    IO_B2R_P2.B2R_CMN.RAMAEN.bit.RAMAEN_BAY_DEKNEE ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif
	}


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_37: "
INT32 CT_Im_B2r_1_37( UCHAR pipe_no )
{
#ifdef CO_DEBUG_ON_PC
	struct t_ct_im_b2r_1_37 {
		UCHAR src_tbl[D_IM_B2R_TABLE_MAX_RGB_KNEE];
	};
	struct t_ct_im_b2r_1_37 tbl_body;
	struct t_ct_im_b2r_1_37* tbl = &tbl_body;
#else //!CO_DEBUG_ON_PC
	struct t_ct_im_b2r_1_37 {
		UCHAR src_tbl[D_IM_B2R_TABLE_MAX_RGB_KNEE];
	}* tbl = (struct t_ct_im_b2r_1_37*)D_IM_B2R_IMG_MEM_IN_ADDR_TOP;
#endif //!CO_DEBUG_ON_PC
	UINT32			loopcnt;
	E_B2R_RGBTBL	loopcnt2;
	UINT32			loopcnt3;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	const char errmsg[] = "%u[%u] unmatch\n";

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


	for( loopcnt = 0; loopcnt < 3; loopcnt++ ) {
		switch( loopcnt ) {
			default:	// max
//			case 0:
				for( loopcnt3 = 0; loopcnt3 < D_IM_B2R_TABLE_MAX_RGB_KNEE; loopcnt3++ ) {
					tbl->src_tbl[loopcnt3] = 0xFF;
				}
				break;
			case 1:	// min
				for( loopcnt3 = 0; loopcnt3 < D_IM_B2R_TABLE_MAX_RGB_KNEE; loopcnt3++ ) {
					tbl->src_tbl[loopcnt3] = 0;
				}
				break;
			case 2:	// indvisual
				for( loopcnt3 = 0; loopcnt3 < D_IM_B2R_TABLE_MAX_RGB_KNEE; loopcnt3++ ) {
					tbl->src_tbl[loopcnt3] = 0xFF - loopcnt3;
				}
				break;
		}

		for( loopcnt2 = (E_B2R_RGBTBL)0; loopcnt2 < E_B2R_RGBTBL_MAX; loopcnt2++ ) {
			Ddim_Print(( "** %u - %u\n", loopcnt, loopcnt2 ));


#ifdef CO_MSG_PRINT_ON
			ercd = Im_B2R_Set_Knee_Table( pipe_no, loopcnt2, tbl->src_tbl );
			Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
			Im_B2R_On_Hclk( pipe_no );
			Im_B2R_Set_RGB_KneeAccessEnable( pipe_no, D_IM_B2R_ENABLE_ON, D_IM_B2R_WAIT_ON );
			switch( loopcnt2 ) {
				default:
//				case E_B2R_RGBTBL_R:
					if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
						for( loopcnt3 = 0; loopcnt3 < D_IM_B2R_TABLE_MAX_RGB_KNEE; loopcnt3++ ) {
							if( IO_B2R_TBL_P1.KNETBLR.byte[loopcnt3] != tbl->src_tbl[loopcnt3] ) {
								Ddim_Print(( errmsg, loopcnt2, loopcnt3 ));
							}
						}
					}
					if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
						for( loopcnt3 = 0; loopcnt3 < D_IM_B2R_TABLE_MAX_RGB_KNEE; loopcnt3++ ) {
							if( IO_B2R_TBL_P2.KNETBLR.byte[loopcnt3] != tbl->src_tbl[loopcnt3] ) {
								Ddim_Print(( errmsg, loopcnt2, loopcnt3 ));
							}
						}
					}
					break;
				case E_B2R_RGBTBL_G:
					if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
						for( loopcnt3 = 0; loopcnt3 < D_IM_B2R_TABLE_MAX_RGB_KNEE; loopcnt3++ ) {
							if( IO_B2R_TBL_P1.KNETBLG.byte[loopcnt3] != tbl->src_tbl[loopcnt3] ) {
								Ddim_Print(( errmsg, loopcnt2, loopcnt3 ));
							}
						}
					}
					if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
						for( loopcnt3 = 0; loopcnt3 < D_IM_B2R_TABLE_MAX_RGB_KNEE; loopcnt3++ ) {
							if( IO_B2R_TBL_P2.KNETBLG.byte[loopcnt3] != tbl->src_tbl[loopcnt3] ) {
								Ddim_Print(( errmsg, loopcnt2, loopcnt3 ));
							}
						}
					}
					break;
				case E_B2R_RGBTBL_B:
					if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
						for( loopcnt3 = 0; loopcnt3 < D_IM_B2R_TABLE_MAX_RGB_KNEE; loopcnt3++ ) {
							if( IO_B2R_TBL_P1.KNETBLB.byte[loopcnt3] != tbl->src_tbl[loopcnt3] ) {
								Ddim_Print(( errmsg, loopcnt2, loopcnt3 ));
							}
						}
					}
					if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
						for( loopcnt3 = 0; loopcnt3 < D_IM_B2R_TABLE_MAX_RGB_KNEE; loopcnt3++ ) {
							if( IO_B2R_TBL_P2.KNETBLB.byte[loopcnt3] != tbl->src_tbl[loopcnt3] ) {
								Ddim_Print(( errmsg, loopcnt2, loopcnt3 ));
							}
						}
					}
					break;
			}
			Im_B2R_Set_RGB_KneeAccessEnable( pipe_no, D_IM_B2R_ENABLE_OFF, D_IM_B2R_WAIT_OFF );
			Im_B2R_Off_Hclk( pipe_no );
#else
			Im_B2R_Set_Knee_Table( pipe_no, loopcnt2, tbl->src_tbl );
#endif
		}
	}


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_38: "
INT32 CT_Im_B2r_1_38( UCHAR pipe_no )
{
	UCHAR access_enable[] = {
		[0] = 1,	// max
		[1] = 0,	// min
	};
	UINT32 loopcnt;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	Im_B2R_Init( D_IM_B2R_PIPE12 );
	Ddim_Print(( "Initial value\n" ));
	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.RAMAEN.word                = 0x%lx\n", IO_B2R_P1.B2R_CMN.RAMAEN.word ));
	Ddim_Print(( "IO_B2R_P1.B2R_CMN.RAMAEN.bit.RAMAEN_RGB_KNEE = %u\n",    IO_B2R_P1.B2R_CMN.RAMAEN.bit.RAMAEN_RGB_KNEE ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.RAMAEN.word                = 0x%lx\n", IO_B2R_P2.B2R_CMN.RAMAEN.word ));
	Ddim_Print(( "IO_B2R_P2.B2R_CMN.RAMAEN.bit.RAMAEN_RGB_KNEE = %u\n",    IO_B2R_P2.B2R_CMN.RAMAEN.bit.RAMAEN_RGB_KNEE ));
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif

	for( loopcnt = 0; loopcnt < (sizeof(access_enable) / sizeof(access_enable[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Set_RGB_KneeAccessEnable( pipe_no, access_enable[loopcnt], D_IM_B2R_WAIT_ON );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( "IO_B2R_P1.B2R_CMN.RAMAEN.word                = 0x%lx\n", IO_B2R_P1.B2R_CMN.RAMAEN.word ));
		Ddim_Print(( "IO_B2R_P1.B2R_CMN.RAMAEN.bit.RAMAEN_RGB_KNEE = %u\n",    IO_B2R_P1.B2R_CMN.RAMAEN.bit.RAMAEN_RGB_KNEE ));
		Ddim_Print(( "IO_B2R_P2.B2R_CMN.RAMAEN.word                = 0x%lx\n", IO_B2R_P2.B2R_CMN.RAMAEN.word ));
		Ddim_Print(( "IO_B2R_P2.B2R_CMN.RAMAEN.bit.RAMAEN_RGB_KNEE = %u\n",    IO_B2R_P2.B2R_CMN.RAMAEN.bit.RAMAEN_RGB_KNEE ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif
	}


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_39: "
INT32 CT_Im_B2r_1_39( UCHAR pipe_no )
{
	T_IM_B2R_OFS ofs[] = {
		[0] = {	// max
			.R = 0x1FF,
			.B = 0x1FF,
			.Gr = 0x1FF,
			.Gb = 0x1FF,
		},
		[1] = {	// max
			.R = -1,
			.B = -1,
			.Gr = -1,
			.Gb = -1,
		},
		[2] = {	// min
			.R = 0,
			.B = 0,
			.Gr = 0,
			.Gb = 0,
		},
		[3] = {	// indvisual
			.R = 1,
			.B = 2,
			.Gr = 3,
			.Gb = 4,
		},
	};
	UINT32 loopcnt;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


	for( loopcnt = 0; loopcnt < (sizeof(ofs) / sizeof(ofs[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Set_Offset( pipe_no, &ofs[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( "IO_B2R_P1.F_B2R.OFSTRB.word       = 0x%lx\n", IO_B2R_P1.F_B2R.OFG.OFSTRB.word ));
		Ddim_Print(( "IO_B2R_P1.F_B2R.OFSTRB.bit.OFSTRR = 0x%x\n",  IO_B2R_P1.F_B2R.OFG.OFSTRB.bit.OFSTRR ));
		Ddim_Print(( "IO_B2R_P1.F_B2R.OFSTRB.bit.OFSTBB = 0x%x\n",  IO_B2R_P1.F_B2R.OFG.OFSTRB.bit.OFSTBB ));
		Ddim_Print(( "IO_B2R_P1.F_B2R.OFSTG.word        = 0x%lx\n", IO_B2R_P1.F_B2R.OFG.OFSTG.word ));
		Ddim_Print(( "IO_B2R_P1.F_B2R.OFSTG.bit.OFSTGR  = 0x%x\n",  IO_B2R_P1.F_B2R.OFG.OFSTG.bit.OFSTGR ));
		Ddim_Print(( "IO_B2R_P1.F_B2R.OFSTG.bit.OFSTGB  = 0x%x\n",  IO_B2R_P1.F_B2R.OFG.OFSTG.bit.OFSTGB ));
		Ddim_Print(( "IO_B2R_P2.F_B2R.OFSTRB.word       = 0x%lx\n", IO_B2R_P2.F_B2R.OFG.OFSTRB.word ));
		Ddim_Print(( "IO_B2R_P2.F_B2R.OFSTRB.bit.OFSTRR = 0x%x\n",  IO_B2R_P2.F_B2R.OFG.OFSTRB.bit.OFSTRR ));
		Ddim_Print(( "IO_B2R_P2.F_B2R.OFSTRB.bit.OFSTBB = 0x%x\n",  IO_B2R_P2.F_B2R.OFG.OFSTRB.bit.OFSTBB ));
		Ddim_Print(( "IO_B2R_P2.F_B2R.OFSTG.word        = 0x%lx\n", IO_B2R_P2.F_B2R.OFG.OFSTG.word ));
		Ddim_Print(( "IO_B2R_P2.F_B2R.OFSTG.bit.OFSTGR  = 0x%x\n",  IO_B2R_P2.F_B2R.OFG.OFSTG.bit.OFSTGR ));
		Ddim_Print(( "IO_B2R_P2.F_B2R.OFSTG.bit.OFSTGB  = 0x%x\n",  IO_B2R_P2.F_B2R.OFG.OFSTG.bit.OFSTGB ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif
	}


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_40: "
INT32 CT_Im_B2r_1_40( UCHAR pipe_no )
{
	UCHAR bayer_left_shift[] = {
		[0] = 3,	// max
		[1] = 0,	// min
	};
	UCHAR saturation_mode[] = {
		[0] = D_IM_B2R_ENABLE_ON,
		[1] = D_IM_B2R_ENABLE_OFF,
	};
	UINT32 loopcnt;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


	for( loopcnt = 0; loopcnt < (sizeof(bayer_left_shift) / sizeof(bayer_left_shift[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, bayer_left_shift[loopcnt], saturation_mode[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.BLSFT.bit.BLSFT  = %u\n", IO_B2R_P1.F_B2R.OFG.BLSFT.bit.BLSFT ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.BLSFT.bit.BLSFT  = %u\n", IO_B2R_P2.F_B2R.OFG.BLSFT.bit.BLSFT ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.BLSFT.bit.BLSFMD = %u\n", IO_B2R_P1.F_B2R.OFG.BLSFT.bit.BLSFMD ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.BLSFT.bit.BLSFMD = %u\n", IO_B2R_P2.F_B2R.OFG.BLSFT.bit.BLSFMD ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif
	}


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_41: "
INT32 CT_Im_B2r_1_41( UCHAR pipe_no )
{
	T_IM_B2R_BAYER_COLOR b2r_bay_color[] = {
		[0] = {	// max
			.R = 0x7FF,
			.B = 0x7FF,
			.Gr = 0x7FF,
			.Gb = 0x7FF,
		},
		[1] = {	// min
			.R = 0,
			.B = 0,
			.Gr = 0,
			.Gb = 0,
		},
		[2] = {	// indvisual
			.R = 1,
			.B = 2,
			.Gr = 3,
			.Gb = 4,
		}
	};
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


	for( loopcnt = 0; loopcnt < (sizeof(b2r_bay_color) / sizeof(b2r_bay_color[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Set_WB_Gain( pipe_no, &b2r_bay_color[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBGARB.bit.WBGARR = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBGARB.bit.WBGARR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBGARB.bit.WBGABB = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBGARB.bit.WBGABB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBGAGR.bit.WBGAGR = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBGAGR.bit.WBGAGR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBGAGR.bit.WBGAGB = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBGAGR.bit.WBGAGB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBGARB.bit.WBGARR = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBGARB.bit.WBGARR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBGARB.bit.WBGABB = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBGARB.bit.WBGABB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBGAGR.bit.WBGAGR = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBGAGR.bit.WBGAGR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBGAGR.bit.WBGAGB = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBGAGR.bit.WBGAGB ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif
	}


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_42: "
INT32 CT_Im_B2r_1_42( UCHAR pipe_no )
{
	T_IM_B2R_BAYER_COLOR b2r_bay_color[] = {
		[0] = {	// max
			.R = 0xFFF,
			.B = 0xFFF,
			.Gr = 0xFFF,
			.Gb = 0xFFF,
		},
		[1] = {	// min
			.R = 0,
			.B = 0,
			.Gr = 0,
			.Gb = 0,
		},
		[2] = {	// indvisual
			.R = 1,
			.B = 2,
			.Gr = 3,
			.Gb = 4,
		}
	};
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


	for( loopcnt = 0; loopcnt < (sizeof(b2r_bay_color) / sizeof(b2r_bay_color[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Set_WB_Slope_Gain( pipe_no, (loopcnt & 1), &b2r_bay_color[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBHCTL.bit.WBHCEN = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBHCTL.bit.WBHCEN ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBSLP.bit.WBSLPRR = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBSLP.bit.WBSLPRR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBSLP.bit.WBSLPBB = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBSLP.bit.WBSLPBB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBSLP.bit.WBSLPGR = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBSLP.bit.WBSLPGR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBSLP.bit.WBSLPGB = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBSLP.bit.WBSLPGB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBHCTL.bit.WBHCEN = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBHCTL.bit.WBHCEN ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBSLP.bit.WBSLPRR = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBSLP.bit.WBSLPRR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBSLP.bit.WBSLPBB = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBSLP.bit.WBSLPBB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBSLP.bit.WBSLPGR = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBSLP.bit.WBSLPGR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBSLP.bit.WBSLPGB = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBSLP.bit.WBSLPGB ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif
	}


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_43: "
INT32 CT_Im_B2r_1_43( UCHAR pipe_no )
{
	T_IM_B2R_BAYER_COLOR b2r_bay_color[] = {
		[0] = {	// max
			.R = 0xFFF,
			.B = 0xFFF,
			.Gr = 0xFFF,
			.Gb = 0xFFF,
		},
		[1] = {	// min
			.R = 0,
			.B = 0,
			.Gr = 0,
			.Gb = 0,
		},
		[2] = {	// indvisual
			.R = 1,
			.B = 2,
			.Gr = 3,
			.Gb = 4,
		}
	};
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


	for( loopcnt = 0; loopcnt < (sizeof(b2r_bay_color) / sizeof(b2r_bay_color[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Set_WB_Clip_Level( pipe_no, &b2r_bay_color[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBLVRB.bit.WBLVRR = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBLVRB.bit.WBLVRR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBLVRB.bit.WBLVBB = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBLVRB.bit.WBLVBB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBLVGR.bit.WBLVGR = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBLVGR.bit.WBLVGR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.OFG.WBLVGR.bit.WBLVGB = 0x%x\n", IO_B2R_P1.F_B2R.OFG.WBLVGR.bit.WBLVGB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBLVRB.bit.WBLVRR = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBLVRB.bit.WBLVRR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBLVRB.bit.WBLVBB = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBLVRB.bit.WBLVBB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBLVGR.bit.WBLVGR = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBLVGR.bit.WBLVGR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.OFG.WBLVGR.bit.WBLVGB = 0x%x\n", IO_B2R_P2.F_B2R.OFG.WBLVGR.bit.WBLVGB ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif
	}


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_44: "
INT32 CT_Im_B2r_1_44( UCHAR pipe_no )
{
	T_IM_B2R_CTRL_ADIP b2r_ctrl_adip[] = {
		[0] = {	// max
			.sharp_level_a = 0xF,
			.sharp_level_b = 0xF,
			.sharp_level_c = 0xF,
			.adj_feel_a = 0xF,
			.adj_feel_b = 0xFF,
			.lf_strength = 0x10,
			.lf_threshold = 0x3FF,
			.cmc_level = 0x4,
			.cmc_limit = 0x3FF,
			.dir_reliab_threshold = 0xFFF,
		},
		[1] = {	// min
			.sharp_level_a = 0,
			.sharp_level_b = 0,
			.sharp_level_c = 0,
			.adj_feel_a = 0,
			.adj_feel_b = 0,
			.lf_strength = 0,
			.lf_threshold = 0,
			.cmc_level = 0,
			.cmc_limit = 0,
			.dir_reliab_threshold = 0,
		},
		[2] = {	// ind
			.sharp_level_a = 10,
			.sharp_level_b = 9,
			.sharp_level_c = 8,
			.adj_feel_a = 7,
			.adj_feel_b = 6,
			.lf_strength = 5,
			.lf_threshold = 4,
			.cmc_level = 3,
			.cmc_limit = 2,
			.dir_reliab_threshold = 1,
		},
	};
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));



	for( loopcnt = 0; loopcnt < (sizeof(b2r_ctrl_adip) / sizeof(b2r_ctrl_adip[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Ctrl_Sensitivity( pipe_no, &b2r_ctrl_adip[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPA      = 0x%x\n", IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPA ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPB      = 0x%x\n", IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPC      = 0x%x\n", IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPC ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.CIA.AIRESY.bit.AIRESYA    = 0x%x\n", IO_B2R_P1.F_B2R.CIA.AIRESY.bit.AIRESYA ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.CIA.AIRESY.bit.AIRESYB    = 0x%x\n", IO_B2R_P1.F_B2R.CIA.AIRESY.bit.AIRESYB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.CIA.LF.bit.LFSTR          = 0x%x\n", IO_B2R_P1.F_B2R.CIA.LF.bit.LFSTR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.CIA.LF.bit.LFTH           = 0x%x\n", IO_B2R_P1.F_B2R.CIA.LF.bit.LFTH ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.CIA.CMC.bit.CMCSCL        = 0x%x\n", IO_B2R_P1.F_B2R.CIA.CMC.bit.CMCSCL ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.CIA.CMC.bit.CMCLMT        = 0x%x\n", IO_B2R_P1.F_B2R.CIA.CMC.bit.CMCLMT ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.F_B2R.CIA.DIRRLITH.bit.DIRRLITH = 0x%x\n", IO_B2R_P1.F_B2R.CIA.DIRRLITH.bit.DIRRLITH ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPA      = 0x%x\n", IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPA ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPB      = 0x%x\n", IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPC      = 0x%x\n", IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPC ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.CIA.AIRESY.bit.AIRESYA    = 0x%x\n", IO_B2R_P2.F_B2R.CIA.AIRESY.bit.AIRESYA ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.CIA.AIRESY.bit.AIRESYB    = 0x%x\n", IO_B2R_P2.F_B2R.CIA.AIRESY.bit.AIRESYB ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.CIA.LF.bit.LFSTR          = 0x%x\n", IO_B2R_P2.F_B2R.CIA.LF.bit.LFSTR ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.CIA.LF.bit.LFTH           = 0x%x\n", IO_B2R_P2.F_B2R.CIA.LF.bit.LFTH ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.CIA.CMC.bit.CMCSCL        = 0x%x\n", IO_B2R_P2.F_B2R.CIA.CMC.bit.CMCSCL ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.CIA.CMC.bit.CMCLMT        = 0x%x\n", IO_B2R_P2.F_B2R.CIA.CMC.bit.CMCLMT ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.F_B2R.CIA.DIRRLITH.bit.DIRRLITH = 0x%x\n", IO_B2R_P2.F_B2R.CIA.DIRRLITH.bit.DIRRLITH ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif
	}


	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_45: "
INT32 CT_Im_B2r_1_45( UCHAR pipe_no )
{
	T_IM_B2R_CTRL_HPF b2r_ctrl_hpf[] = {
		[0] = {	// max
			.hpf_enable = 1,
			.flat_ctrl_enable = 1,
			.y_ctrl_enable = 1,
			.hpf_k = {
				0x3FF,	// 0
				-1,	// 1
				-1,	// 2
				-1,	// 3
				-1,	// 4
				-1,	// 5
				-1,	// 6
				-1,	// 7
				-1,	// 8
				-1,	// 9
			},
			.hpf_level_clip_p = 0x1FF,
			.hpf_level_clip_m = 0x1FF,
			.flat_scale = 0xF,
			.flat_threshold = 0xFFF,
			.flat_gain_limit = 0xFFF,
			.flat_level_clip_p = 0x1FF,
			.flat_level_clip_m = 0x1FF,
			.y_convert_coef = {
				0xFF, 0xFF, 0xFF,
				  -1,   -1,   -1,
				  -1,   -1,   -1,
			},
			.y_ctrl_offset = {
				0xFFF, 0xFFF, 0xFFF, 0xFFF,
			},
			.y_ctrl_gain = {
				-1, -1, -1, -1,
			},
			.y_ctrl_border = {
				0xFFF, 0xFFF, 0xFFF, 0xFFF,
			},
			.y_ctrl_gain_posi_dec1 = 0x7,
			.y_ctrl_gain_posi_dec2 = 0xF,
			.y_ctrl_level_clip_p = 0x1FF,
			.y_ctrl_level_clip_m = 0x1FF,
			.y_ctrl_cor_p = 0x3F,
			.y_ctrl_cor_m = 0x3F,
			.hpf_add_level_clip_r = 0xFFF,
			.hpf_add_level_clip_g = 0xFFF,
			.hpf_add_level_clip_b = 0xFFF,
			.area_a_enable = 1,
			.area_b_enable = 1,
			.area_marge_method = 2,
			.area_a_ctrl = {
				.ellipse_area_focus1_x = -1,
				.ellipse_area_focus1_y = -1,
				.ellipse_area_focus2_x = -1,
				.ellipse_area_focus2_y = -1,
				.c_area_scale_offset = 0xFF,
				.c_area_scale_gain = {
					-1, -1,
				},
				.c_area_scale_border = 0x7F,
				.c_area_scale_clip_l = 0xFF,
				.c_area_scale_clip_h = 0xFF,
				.y_area_scale_gain = {
					0xFF, 0xFF,
				},
				.y_area_scale_border = {
					0xFFF, 0xFFF,
				},
				.y_area_scale_clip_l = 0x1FF,
				.y_area_scale_clip_h = 0x1FF,
			},
			.area_b_ctrl = {
				.ellipse_area_focus1_x = -1,
				.ellipse_area_focus1_y = -1,
				.ellipse_area_focus2_x = -1,
				.ellipse_area_focus2_y = -1,
				.c_area_scale_offset = 0xFF,
				.c_area_scale_gain = {
					-1, -1,
				},
				.c_area_scale_border = 0x7F,
				.c_area_scale_clip_l = 0xFF,
				.c_area_scale_clip_h = 0xFF,
				.y_area_scale_gain = {
					0xFF, 0xFF,
				},
				.y_area_scale_border = {
					0xFFF, 0xFFF,
				},
				.y_area_scale_clip_l = 0x1FF,
				.y_area_scale_clip_h = 0x1FF,
			}
		},
		[1] = {	// min
			.hpf_enable = 0,
			.flat_ctrl_enable = 0,
			.y_ctrl_enable = 0,
			.hpf_k = {
				0,	// 0
				0,	// 1
				0,	// 2
				0,	// 3
				0,	// 4
				0,	// 5
				0,	// 6
				0,	// 7
				0,	// 8
				0,	// 9
			},
			.hpf_level_clip_p = 0,
			.hpf_level_clip_m = 0,
			.flat_scale = 0,
			.flat_threshold = 0,
			.flat_gain_limit = 0,
			.flat_level_clip_p = 0,
			.flat_level_clip_m = 0,
			.y_convert_coef = {
				0, 0, 0,
				0, 0, 0,
				0, 0, 0,
			},
			.y_ctrl_offset = {
				0, 0, 0, 0,
			},
			.y_ctrl_gain = {
				0, 0, 0, 0,
			},
			.y_ctrl_border = {
				0, 0, 0, 0,
			},
			.y_ctrl_gain_posi_dec1 = 0,
			.y_ctrl_gain_posi_dec2 = 0,
			.y_ctrl_level_clip_p = 0,
			.y_ctrl_level_clip_m = 0,
			.y_ctrl_cor_p = 0,
			.y_ctrl_cor_m = 0,
			.hpf_add_level_clip_r = 0,
			.hpf_add_level_clip_g = 0,
			.hpf_add_level_clip_b = 0,
			.area_a_enable = 0,
			.area_b_enable = 0,
			.area_marge_method = 0,
			.area_a_ctrl = {
				.ellipse_area_focus1_x = 0,
				.ellipse_area_focus1_y = 0,
				.ellipse_area_focus2_x = 0,
				.ellipse_area_focus2_y = 0,
				.c_area_scale_offset = 0,
				.c_area_scale_gain = {
					0, 0,
				},
				.c_area_scale_border = 0,
				.c_area_scale_clip_l = 0,
				.c_area_scale_clip_h = 0,
				.y_area_scale_gain = {
					0, 0,
				},
				.y_area_scale_border = {
					0, 0,
				},
				.y_area_scale_clip_l = 0,
				.y_area_scale_clip_h = 0,
			},
			.area_b_ctrl = {
				.ellipse_area_focus1_x = 0,
				.ellipse_area_focus1_y = 0,
				.ellipse_area_focus2_x = 0,
				.ellipse_area_focus2_y = 0,
				.c_area_scale_offset = 0,
				.c_area_scale_gain = {
					0, 0,
				},
				.c_area_scale_border = 0,
				.c_area_scale_clip_l = 0,
				.c_area_scale_clip_h = 0,
				.y_area_scale_gain = {
					0, 0,
				},
				.y_area_scale_border = {
					0, 0,
				},
				.y_area_scale_clip_l = 0,
				.y_area_scale_clip_h = 0,
			}
		},
		[2] = {	// ind
			.hpf_enable = 1,
			.flat_ctrl_enable = 0,
			.y_ctrl_enable = 1,
			.hpf_k = {
				2,	// 0
				3,	// 1
				4,	// 2
				5,	// 3
				6,	// 4
				7,	// 5
				8,	// 6
				9,	// 7
				10,	// 8
				11,	// 9
			},
			.hpf_level_clip_p = 12,
			.hpf_level_clip_m = 13,
			.flat_scale = 14,
			.flat_threshold = 15,
			.flat_gain_limit = 16,
			.flat_level_clip_p = 17,
			.flat_level_clip_m = 18,
			.y_convert_coef = {
				19, 20, 21,
				22, 23, 24,
				25, 26, 27,
			},
			.y_ctrl_offset = {
				28, 29, 30, 31,
			},
			.y_ctrl_gain = {
				32, 33, 34, 35,
			},
			.y_ctrl_border = {
				36, 37, 38, 39,
			},
			.y_ctrl_gain_posi_dec1 = 2,
			.y_ctrl_gain_posi_dec2 = 3,
			.y_ctrl_level_clip_p = 42,
			.y_ctrl_level_clip_m = 43,
			.y_ctrl_cor_p = 44,
			.y_ctrl_cor_m = 45,
			.hpf_add_level_clip_r = 46,
			.hpf_add_level_clip_g = 47,
			.hpf_add_level_clip_b = 48,
			.area_a_enable = 1,
			.area_b_enable = 0,
			.area_marge_method = 1,
			.area_a_ctrl = {
				.ellipse_area_focus1_x = 49,
				.ellipse_area_focus1_y = 50,
				.ellipse_area_focus2_x = 51,
				.ellipse_area_focus2_y = 52,
				.c_area_scale_offset = 53,
				.c_area_scale_gain = {
					54, 55,
				},
				.c_area_scale_border = 56,
				.c_area_scale_clip_l = 57,
				.c_area_scale_clip_h = 58,
				.y_area_scale_gain = {
					59, 60,
				},
				.y_area_scale_border = {
					61, 62,
				},
				.y_area_scale_clip_l = 63,
				.y_area_scale_clip_h = 64,
			},
			.area_b_ctrl = {
				.ellipse_area_focus1_x = 65,
				.ellipse_area_focus1_y = 66,
				.ellipse_area_focus2_x = 67,
				.ellipse_area_focus2_y = 68,
				.c_area_scale_offset = 69,
				.c_area_scale_gain = {
					70, 71,
				},
				.c_area_scale_border = 72,
				.c_area_scale_clip_l = 73,
				.c_area_scale_clip_h = 74,
				.y_area_scale_gain = {
					75, 76,
				},
				.y_area_scale_border = {
					77, 78,
				},
				.y_area_scale_clip_l = 79,
				.y_area_scale_clip_h = 80,
			}
		},
	};
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));



	for( loopcnt = 0; loopcnt < (sizeof(b2r_ctrl_hpf) / sizeof(b2r_ctrl_hpf[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Ctrl_HighPassFilter( pipe_no, &b2r_ctrl_hpf[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( "IO_B2R_P1.HFEEN          = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACTL.bit.HFEEN ));
		Ddim_Print(( "IO_B2R_P1.HFAFEN         = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACTL.bit.HFAFEN ));
		Ddim_Print(( "IO_B2R_P1.HFAYEN         = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACTL.bit.HFAYEN ));
		Ddim_Print(( "IO_B2R_P1.HFK[0]         = 0x%08lx\n", IO_B2R_P1.F_B2R.HFA.HFK.word[0] ));
		Ddim_Print(( "IO_B2R_P1.HFK[1]         = 0x%08lx\n", IO_B2R_P1.F_B2R.HFA.HFK.word[1] ));
		Ddim_Print(( "IO_B2R_P1.HFK[2]         = 0x%08lx\n", IO_B2R_P1.F_B2R.HFA.HFK.word[2] ));
		Ddim_Print(( "IO_B2R_P1.HFK[3]         = 0x%08lx\n", IO_B2R_P1.F_B2R.HFA.HFK.word[3] ));
		Ddim_Print(( "IO_B2R_P1.HFK0           = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK0 ));
		Ddim_Print(( "IO_B2R_P1.HFK1           = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK1 ));
		Ddim_Print(( "IO_B2R_P1.HFK2           = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK2 ));
		Ddim_Print(( "IO_B2R_P1.HFK3           = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK3 ));
		Ddim_Print(( "IO_B2R_P1.HFK4           = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK4 ));
		Ddim_Print(( "IO_B2R_P1.HFK5           = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK5 ));
		Ddim_Print(( "IO_B2R_P1.HFK6           = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK6 ));
		Ddim_Print(( "IO_B2R_P1.HFK7           = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK7 ));
		Ddim_Print(( "IO_B2R_P1.HFK8           = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK8 ));
		Ddim_Print(( "IO_B2R_P1.HFK9           = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK9 ));
		Ddim_Print(( "IO_B2R_P1.HFEFCLPP       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFEFCLP.bit.HFEFCLPP ));
		Ddim_Print(( "IO_B2R_P1.HFEFCLPM       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFEFCLP.bit.HFEFCLPM ));
		Ddim_Print(( "IO_B2R_P1.HFAFSL         = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAFST.bit.HFAFSL ));
		Ddim_Print(( "IO_B2R_P1.HFAFTH         = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAFST.bit.HFAFTH ));
		Ddim_Print(( "IO_B2R_P1.HFAFGU         = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAFGU.bit.HFAFGU ));
		Ddim_Print(( "IO_B2R_P1.HFAFCLPP       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAFCLP.bit.HFAFCLPP ));
		Ddim_Print(( "IO_B2R_P1.HFAFCLPM       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAFCLP.bit.HFAFCLPM ));
		Ddim_Print(( "IO_B2R_P1.HFAYC[0]       = 0x%lx\n",   IO_B2R_P1.F_B2R.HFA.HFAYC.word[0] ));
		Ddim_Print(( "IO_B2R_P1.HFAYC[1]       = 0x%lx\n",   IO_B2R_P1.F_B2R.HFA.HFAYC.word[1] ));
		Ddim_Print(( "IO_B2R_P1.HFAYC[2]       = 0x%lx\n",   IO_B2R_P1.F_B2R.HFA.HFAYC.word[2] ));
		Ddim_Print(( "IO_B2R_P1.HFAYC[3]       = 0x%lx\n",   IO_B2R_P1.F_B2R.HFA.HFAYC.word[3] ));
		Ddim_Print(( "IO_B2R_P1.HFAYC00        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC00 ));
		Ddim_Print(( "IO_B2R_P1.HFAYC01        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC01 ));
		Ddim_Print(( "IO_B2R_P1.HFAYC02        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC02 ));
		Ddim_Print(( "IO_B2R_P1.HFAYC10        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC10 ));
		Ddim_Print(( "IO_B2R_P1.HFAYC11        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC11 ));
		Ddim_Print(( "IO_B2R_P1.HFAYC12        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC12 ));
		Ddim_Print(( "IO_B2R_P1.HFAYC20        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC20 ));
		Ddim_Print(( "IO_B2R_P1.HFAYC21        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC21 ));
		Ddim_Print(( "IO_B2R_P1.HFAYC22        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC22 ));
		Ddim_Print(( "IO_B2R_P1.HFAYOF_0       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYOF.bit.HFAYOF_0 ));
		Ddim_Print(( "IO_B2R_P1.HFAYOF_1       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYOF.bit.HFAYOF_1 ));
		Ddim_Print(( "IO_B2R_P1.HFAYOF_2       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYOF.bit.HFAYOF_2 ));
		Ddim_Print(( "IO_B2R_P1.HFAYOF_3       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYOF.bit.HFAYOF_3 ));
		Ddim_Print(( "IO_B2R_P1.HFAYGA         = 0x%lx\n",   IO_B2R_P1.F_B2R.HFA.HFAYGA.word ));
		Ddim_Print(( "IO_B2R_P1.HFAYGA_0       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYGA.bit.HFAYGA_0 ));
		Ddim_Print(( "IO_B2R_P1.HFAYGA_1       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYGA.bit.HFAYGA_1 ));
		Ddim_Print(( "IO_B2R_P1.HFAYGA_2       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYGA.bit.HFAYGA_2 ));
		Ddim_Print(( "IO_B2R_P1.HFAYGA_3       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYGA.bit.HFAYGA_3 ));
		Ddim_Print(( "IO_B2R_P1.HFAYBD_0       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYBD.bit.HFAYBD_0 ));
		Ddim_Print(( "IO_B2R_P1.HFAYBD_1       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYBD.bit.HFAYBD_1 ));
		Ddim_Print(( "IO_B2R_P1.HFAYBD_2       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYBD.bit.HFAYBD_2 ));
		Ddim_Print(( "IO_B2R_P1.HFAYBD_3       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYBD.bit.HFAYBD_3 ));
		Ddim_Print(( "IO_B2R_P1.HFAYDIV1       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYDIV.bit.HFAYDIV1 ));
		Ddim_Print(( "IO_B2R_P1.HFAYDIV2       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYDIV.bit.HFAYDIV2 ));
		Ddim_Print(( "IO_B2R_P1.HFAYCLPP       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYCLP.bit.HFAYCLPP ));
		Ddim_Print(( "IO_B2R_P1.HFAYCLPM       = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYCLP.bit.HFAYCLPM ));
		Ddim_Print(( "IO_B2R_P1.HFACORP        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACOR.bit.HFACORP ));
		Ddim_Print(( "IO_B2R_P1.HFACORM        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACOR.bit.HFACORM ));
		Ddim_Print(( "IO_B2R_P1.HFACLPR        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACLP.bit.HFACLPR ));
		Ddim_Print(( "IO_B2R_P1.HFACLPG        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACLP.bit.HFACLPG ));
		Ddim_Print(( "IO_B2R_P1.HFACLPB        = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACLP.bit.HFACLPB ));
		Ddim_Print(( "IO_B2R_P1.HFACAE         = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVCTL.bit.HFACAE ));
		Ddim_Print(( "IO_B2R_P1.HFACBE         = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVCTL.bit.HFACBE ));
		Ddim_Print(( "IO_B2R_P1.HFACRVMRG      = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVCTL.bit.HFACRVMRG ));
		Ddim_Print(( "IO_B2R_P1.HFACRVAFX      = 0x%lx\n",   IO_B2R_P1.F_B2R.HFA.HFACVAFX.word ));
		Ddim_Print(( "IO_B2R_P1.HFACRVAFX1     = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACVAFX.bit.HFACRVAFX1 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVAFY1     = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACVAFX.bit.HFACRVAFY1 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVAFX2     = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACVAFX.bit.HFACRVAFX2 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVAFY2     = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACVAFX.bit.HFACRVAFY2 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVASCLOF1  = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVASCLOF.bit.HFACRVASCLOF1 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVASCLGA   = 0x%lx\n",   IO_B2R_P1.F_B2R.HFA.HFACRVASCLGA.word ));
		Ddim_Print(( "IO_B2R_P1.HFACRVASCLGA_0 = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVASCLGA.bit.HFACRVASCLGA_0 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVASCLGA_1 = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVASCLGA.bit.HFACRVASCLGA_1 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVASCLBD1  = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVASCLBD.bit.HFACRVASCLBD1 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVASCLCPL  = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVASCLCP.bit.HFACRVASCLCPL ));
		Ddim_Print(( "IO_B2R_P1.HFACRVASCLCPH  = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVASCLCP.bit.HFACRVASCLCPH ));
		Ddim_Print(( "IO_B2R_P1.HFAYASCLGA_0   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYASCLGA.bit.HFAYASCLGA_0 ));
		Ddim_Print(( "IO_B2R_P1.HFAYASCLGA_1   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYASCLGA.bit.HFAYASCLGA_1 ));
		Ddim_Print(( "IO_B2R_P1.HFAYASCLBD_0   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYASCLBD.bit.HFAYASCLBD_0 ));
		Ddim_Print(( "IO_B2R_P1.HFAYASCLBD_1   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYASCLBD.bit.HFAYASCLBD_1 ));
		Ddim_Print(( "IO_B2R_P1.HFAYASCLCLPL   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYASCLCLP.bit.HFAYASCLCLPL ));
		Ddim_Print(( "IO_B2R_P1.HFAYASCLCLPH   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYASCLCLP.bit.HFAYASCLCLPH ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBFX      = 0x%lx\n",   IO_B2R_P1.F_B2R.HFA.HFACVBFX.word ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBFX1     = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACVBFX.bit.HFACRVBFX1 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBFY1     = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACVBFX.bit.HFACRVBFY1 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBFX2     = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACVBFX.bit.HFACRVBFX2 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBFY2     = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACVBFX.bit.HFACRVBFY2 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBSCLOF1  = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVBSCLOF.bit.HFACRVBSCLOF1 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBSCLGA   = 0x%lx\n",   IO_B2R_P1.F_B2R.HFA.HFACRVBSCLGA.word ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBSCLGA_0 = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVBSCLGA.bit.HFACRVBSCLGA_0 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBSCLGA_1 = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVBSCLGA.bit.HFACRVBSCLGA_1 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBSCLBD1  = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVBSCLBD.bit.HFACRVBSCLBD1 ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBSCLCPL  = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVBSCLCP.bit.HFACRVBSCLCPL ));
		Ddim_Print(( "IO_B2R_P1.HFACRVBSCLCPH  = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFACRVBSCLCP.bit.HFACRVBSCLCPH ));
		Ddim_Print(( "IO_B2R_P1.HFAYBSCLGA_0   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYBSCLGA.bit.HFAYBSCLGA_0 ));
		Ddim_Print(( "IO_B2R_P1.HFAYBSCLGA_1   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYBSCLGA.bit.HFAYBSCLGA_1 ));
		Ddim_Print(( "IO_B2R_P1.HFAYBSCLBD_0   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYBSCLBD.bit.HFAYBSCLBD_0 ));
		Ddim_Print(( "IO_B2R_P1.HFAYBSCLBD_1   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYBSCLBD.bit.HFAYBSCLBD_1 ));
		Ddim_Print(( "IO_B2R_P1.HFAYBSCLCLPL   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYBSCLCLP.bit.HFAYBSCLCLPL ));
		Ddim_Print(( "IO_B2R_P1.HFAYBSCLCLPH   = 0x%x\n",    IO_B2R_P1.F_B2R.HFA.HFAYBSCLCLP.bit.HFAYBSCLCLPH ));
		Ddim_Print(( "IO_B2R_P2.HFEEN          = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACTL.bit.HFEEN ));
		Ddim_Print(( "IO_B2R_P2.HFAFEN         = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACTL.bit.HFAFEN ));
		Ddim_Print(( "IO_B2R_P2.HFAYEN         = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACTL.bit.HFAYEN ));
		Ddim_Print(( "IO_B2R_P2.HFK[0]         = 0x%08lx\n", IO_B2R_P2.F_B2R.HFA.HFK.word[0] ));
		Ddim_Print(( "IO_B2R_P2.HFK[1]         = 0x%08lx\n", IO_B2R_P2.F_B2R.HFA.HFK.word[1] ));
		Ddim_Print(( "IO_B2R_P2.HFK[2]         = 0x%08lx\n", IO_B2R_P2.F_B2R.HFA.HFK.word[2] ));
		Ddim_Print(( "IO_B2R_P2.HFK[3]         = 0x%08lx\n", IO_B2R_P2.F_B2R.HFA.HFK.word[3] ));
		Ddim_Print(( "IO_B2R_P2.HFK0           = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK0 ));
		Ddim_Print(( "IO_B2R_P2.HFK1           = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK1 ));
		Ddim_Print(( "IO_B2R_P2.HFK2           = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK2 ));
		Ddim_Print(( "IO_B2R_P2.HFK3           = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK3 ));
		Ddim_Print(( "IO_B2R_P2.HFK4           = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK4 ));
		Ddim_Print(( "IO_B2R_P2.HFK5           = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK5 ));
		Ddim_Print(( "IO_B2R_P2.HFK6           = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK6 ));
		Ddim_Print(( "IO_B2R_P2.HFK7           = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK7 ));
		Ddim_Print(( "IO_B2R_P2.HFK8           = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK8 ));
		Ddim_Print(( "IO_B2R_P2.HFK9           = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK9 ));
		Ddim_Print(( "IO_B2R_P2.HFEFCLPP       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFEFCLP.bit.HFEFCLPP ));
		Ddim_Print(( "IO_B2R_P2.HFEFCLPM       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFEFCLP.bit.HFEFCLPM ));
		Ddim_Print(( "IO_B2R_P2.HFAFSL         = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAFST.bit.HFAFSL ));
		Ddim_Print(( "IO_B2R_P2.HFAFTH         = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAFST.bit.HFAFTH ));
		Ddim_Print(( "IO_B2R_P2.HFAFGU         = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAFGU.bit.HFAFGU ));
		Ddim_Print(( "IO_B2R_P2.HFAFCLPP       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAFCLP.bit.HFAFCLPP ));
		Ddim_Print(( "IO_B2R_P2.HFAFCLPM       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAFCLP.bit.HFAFCLPM ));
		Ddim_Print(( "IO_B2R_P2.HFAYC[0]       = 0x%lx\n",   IO_B2R_P2.F_B2R.HFA.HFAYC.word[0] ));
		Ddim_Print(( "IO_B2R_P2.HFAYC[1]       = 0x%lx\n",   IO_B2R_P2.F_B2R.HFA.HFAYC.word[1] ));
		Ddim_Print(( "IO_B2R_P2.HFAYC[2]       = 0x%lx\n",   IO_B2R_P2.F_B2R.HFA.HFAYC.word[2] ));
		Ddim_Print(( "IO_B2R_P2.HFAYC[3]       = 0x%lx\n",   IO_B2R_P2.F_B2R.HFA.HFAYC.word[3] ));
		Ddim_Print(( "IO_B2R_P2.HFAYC00        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC00 ));
		Ddim_Print(( "IO_B2R_P2.HFAYC01        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC01 ));
		Ddim_Print(( "IO_B2R_P2.HFAYC02        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC02 ));
		Ddim_Print(( "IO_B2R_P2.HFAYC10        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC10 ));
		Ddim_Print(( "IO_B2R_P2.HFAYC11        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC11 ));
		Ddim_Print(( "IO_B2R_P2.HFAYC12        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC12 ));
		Ddim_Print(( "IO_B2R_P2.HFAYC20        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC20 ));
		Ddim_Print(( "IO_B2R_P2.HFAYC21        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC21 ));
		Ddim_Print(( "IO_B2R_P2.HFAYC22        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC22 ));
		Ddim_Print(( "IO_B2R_P2.HFAYOF_0       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYOF.bit.HFAYOF_0 ));
		Ddim_Print(( "IO_B2R_P2.HFAYOF_1       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYOF.bit.HFAYOF_1 ));
		Ddim_Print(( "IO_B2R_P2.HFAYOF_2       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYOF.bit.HFAYOF_2 ));
		Ddim_Print(( "IO_B2R_P2.HFAYOF_3       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYOF.bit.HFAYOF_3 ));
		Ddim_Print(( "IO_B2R_P2.HFAYGA         = 0x%lx\n",   IO_B2R_P2.F_B2R.HFA.HFAYGA.word ));
		Ddim_Print(( "IO_B2R_P2.HFAYGA_0       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYGA.bit.HFAYGA_0 ));
		Ddim_Print(( "IO_B2R_P2.HFAYGA_1       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYGA.bit.HFAYGA_1 ));
		Ddim_Print(( "IO_B2R_P2.HFAYGA_2       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYGA.bit.HFAYGA_2 ));
		Ddim_Print(( "IO_B2R_P2.HFAYGA_3       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYGA.bit.HFAYGA_3 ));
		Ddim_Print(( "IO_B2R_P2.HFAYBD_0       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYBD.bit.HFAYBD_0 ));
		Ddim_Print(( "IO_B2R_P2.HFAYBD_1       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYBD.bit.HFAYBD_1 ));
		Ddim_Print(( "IO_B2R_P2.HFAYBD_2       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYBD.bit.HFAYBD_2 ));
		Ddim_Print(( "IO_B2R_P2.HFAYBD_3       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYBD.bit.HFAYBD_3 ));
		Ddim_Print(( "IO_B2R_P2.HFAYDIV1       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYDIV.bit.HFAYDIV1 ));
		Ddim_Print(( "IO_B2R_P2.HFAYDIV2       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYDIV.bit.HFAYDIV2 ));
		Ddim_Print(( "IO_B2R_P2.HFAYCLPP       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYCLP.bit.HFAYCLPP ));
		Ddim_Print(( "IO_B2R_P2.HFAYCLPM       = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYCLP.bit.HFAYCLPM ));
		Ddim_Print(( "IO_B2R_P2.HFACORP        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACOR.bit.HFACORP ));
		Ddim_Print(( "IO_B2R_P2.HFACORM        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACOR.bit.HFACORM ));
		Ddim_Print(( "IO_B2R_P2.HFACLPR        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACLP.bit.HFACLPR ));
		Ddim_Print(( "IO_B2R_P2.HFACLPG        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACLP.bit.HFACLPG ));
		Ddim_Print(( "IO_B2R_P2.HFACLPB        = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACLP.bit.HFACLPB ));
		Ddim_Print(( "IO_B2R_P2.HFACAE         = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVCTL.bit.HFACAE ));
		Ddim_Print(( "IO_B2R_P2.HFACBE         = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVCTL.bit.HFACBE ));
		Ddim_Print(( "IO_B2R_P2.HFACRVMRG      = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVCTL.bit.HFACRVMRG ));
		Ddim_Print(( "IO_B2R_P2.HFACRVAFX      = 0x%lx\n",   IO_B2R_P2.F_B2R.HFA.HFACVAFX.word ));
		Ddim_Print(( "IO_B2R_P2.HFACRVAFX1     = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACVAFX.bit.HFACRVAFX1 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVAFY1     = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACVAFX.bit.HFACRVAFY1 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVAFX2     = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACVAFX.bit.HFACRVAFX2 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVAFY2     = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACVAFX.bit.HFACRVAFY2 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVASCLOF1  = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVASCLOF.bit.HFACRVASCLOF1 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVASCLGA   = 0x%lx\n",   IO_B2R_P2.F_B2R.HFA.HFACRVASCLGA.word ));
		Ddim_Print(( "IO_B2R_P2.HFACRVASCLGA_0 = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVASCLGA.bit.HFACRVASCLGA_0 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVASCLGA_1 = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVASCLGA.bit.HFACRVASCLGA_1 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVASCLBD1  = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVASCLBD.bit.HFACRVASCLBD1 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVASCLCPL  = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVASCLCP.bit.HFACRVASCLCPL ));
		Ddim_Print(( "IO_B2R_P2.HFACRVASCLCPH  = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVASCLCP.bit.HFACRVASCLCPH ));
		Ddim_Print(( "IO_B2R_P2.HFAYASCLGA_0   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYASCLGA.bit.HFAYASCLGA_0 ));
		Ddim_Print(( "IO_B2R_P2.HFAYASCLGA_1   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYASCLGA.bit.HFAYASCLGA_1 ));
		Ddim_Print(( "IO_B2R_P2.HFAYASCLBD_0   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYASCLBD.bit.HFAYASCLBD_0 ));
		Ddim_Print(( "IO_B2R_P2.HFAYASCLBD_1   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYASCLBD.bit.HFAYASCLBD_1 ));
		Ddim_Print(( "IO_B2R_P2.HFAYASCLCLPL   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYASCLCLP.bit.HFAYASCLCLPL ));
		Ddim_Print(( "IO_B2R_P2.HFAYASCLCLPH   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYASCLCLP.bit.HFAYASCLCLPH ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBFX      = 0x%lx\n",   IO_B2R_P2.F_B2R.HFA.HFACVBFX.word ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBFX1     = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACVBFX.bit.HFACRVBFX1 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBFY1     = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACVBFX.bit.HFACRVBFY1 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBFX2     = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACVBFX.bit.HFACRVBFX2 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBFY2     = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACVBFX.bit.HFACRVBFY2 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBSCLOF1  = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVBSCLOF.bit.HFACRVBSCLOF1 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBSCLGA   = 0x%lx\n",   IO_B2R_P2.F_B2R.HFA.HFACRVBSCLGA.word ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBSCLGA_0 = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVBSCLGA.bit.HFACRVBSCLGA_0 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBSCLGA_1 = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVBSCLGA.bit.HFACRVBSCLGA_1 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBSCLBD1  = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVBSCLBD.bit.HFACRVBSCLBD1 ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBSCLCPL  = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVBSCLCP.bit.HFACRVBSCLCPL ));
		Ddim_Print(( "IO_B2R_P2.HFACRVBSCLCPH  = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFACRVBSCLCP.bit.HFACRVBSCLCPH ));
		Ddim_Print(( "IO_B2R_P2.HFAYBSCLGA_0   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYBSCLGA.bit.HFAYBSCLGA_0 ));
		Ddim_Print(( "IO_B2R_P2.HFAYBSCLGA_1   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYBSCLGA.bit.HFAYBSCLGA_1 ));
		Ddim_Print(( "IO_B2R_P2.HFAYBSCLBD_0   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYBSCLBD.bit.HFAYBSCLBD_0 ));
		Ddim_Print(( "IO_B2R_P2.HFAYBSCLBD_1   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYBSCLBD.bit.HFAYBSCLBD_1 ));
		Ddim_Print(( "IO_B2R_P2.HFAYBSCLCLPL   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYBSCLCLP.bit.HFAYBSCLCLPL ));
		Ddim_Print(( "IO_B2R_P2.HFAYBSCLCLPH   = 0x%x\n",    IO_B2R_P2.F_B2R.HFA.HFAYBSCLCLP.bit.HFAYBSCLCLPH ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_46: "
INT32 CT_Im_B2r_1_46( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;
	const T_IM_B2R_CTRL_RDMA_DEKNEE_TBL_ADDR* addr;
	ULONG* pt_addr;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_RdmaAddr_Deknee_Table( pipe_no, &addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
	pt_addr = (ULONG*)addr;
	for( loopcnt = 0; loopcnt < sizeof(T_IM_B2R_CTRL_RDMA_DEKNEE_TBL_ADDR) / sizeof(ULONG); loopcnt++ ) {
		Ddim_Print(( "address[%d] = 0x%lx\n", loopcnt, *pt_addr ));
		pt_addr++;
	}
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_47: "
INT32 CT_Im_B2r_1_47( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;
	const T_IM_B2R_CTRL_RDMA_KNEE_TBL_ADDR* addr;
	ULONG* pt_addr;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_RdmaAddr_Knee_Table( pipe_no, E_B2R_RGBTBL_R, &addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
	pt_addr = (ULONG*)addr;
	for( loopcnt = 0; loopcnt < sizeof(T_IM_B2R_CTRL_RDMA_KNEE_TBL_ADDR) / sizeof(ULONG); loopcnt++ ) {
		Ddim_Print(( "R address[%d] = 0x%lx\n", loopcnt, *pt_addr ));
		pt_addr++;
	}

	ercd = Im_B2R_Get_RdmaAddr_Knee_Table( pipe_no, E_B2R_RGBTBL_G, &addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
	pt_addr = (ULONG*)addr;
	for( loopcnt = 0; loopcnt < sizeof(T_IM_B2R_CTRL_RDMA_KNEE_TBL_ADDR) / sizeof(ULONG); loopcnt++ ) {
		Ddim_Print(( "G address[%d] = 0x%lx\n", loopcnt, *pt_addr ));
		pt_addr++;
	}

	ercd = Im_B2R_Get_RdmaAddr_Knee_Table( pipe_no, E_B2R_RGBTBL_B, &addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
	pt_addr = (ULONG*)addr;
	for( loopcnt = 0; loopcnt < sizeof(T_IM_B2R_CTRL_RDMA_KNEE_TBL_ADDR) / sizeof(ULONG); loopcnt++ ) {
		Ddim_Print(( "B address[%d] = 0x%lx\n", loopcnt, *pt_addr ));
		pt_addr++;
	}
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_48: "
INT32 CT_Im_B2r_1_48( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;
	const T_IM_B2R_CTRL_RDMA_OFST_ADDR* addr;
	ULONG* pt_addr;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_RdmaAddr_Offset_Cntl( pipe_no, &addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
	pt_addr = (ULONG*)addr;
	for( loopcnt = 0; loopcnt < sizeof(T_IM_B2R_CTRL_RDMA_OFST_ADDR) / sizeof(ULONG); loopcnt++ ) {
		Ddim_Print(( "address[%d] = 0x%lx\n", loopcnt, *pt_addr ));
		pt_addr++;
	}
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_49: "
INT32 CT_Im_B2r_1_49( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;
	const T_IM_B2R_CTRL_RDMA_WB_SLOPE_ADDR* addr;
	ULONG* pt_addr;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_RdmaAddr_WB_Slope_Cntl( pipe_no, &addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
	pt_addr = (ULONG*)addr;
	for( loopcnt = 0; loopcnt < sizeof(T_IM_B2R_CTRL_RDMA_WB_SLOPE_ADDR) / sizeof(ULONG); loopcnt++ ) {
		Ddim_Print(( "address[%d] = 0x%lx\n", loopcnt, *pt_addr ));
		pt_addr++;
	}
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_50: "
INT32 CT_Im_B2r_1_50( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;
	const T_IM_B2R_CTRL_RDMA_WB_CLIP_ADDR* addr;
	ULONG* pt_addr;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_RdmaAddr_WB_Clip_Cntl( pipe_no, &addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
	pt_addr = (ULONG*)addr;
	for( loopcnt = 0; loopcnt < sizeof(T_IM_B2R_CTRL_RDMA_WB_CLIP_ADDR) / sizeof(ULONG); loopcnt++ ) {
		Ddim_Print(( "address[%d] = 0x%lx\n", loopcnt, *pt_addr ));
		pt_addr++;
	}
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_51: "
INT32 CT_Im_B2r_1_51( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;
	const T_IM_B2R_CTRL_RDMA_SENSITIVITY_ADDR* addr;
	ULONG* pt_addr;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_RdmaAddr_Sensitivity_Cntl( pipe_no, &addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
	pt_addr = (ULONG*)addr;
	for( loopcnt = 0; loopcnt < sizeof(T_IM_B2R_CTRL_RDMA_SENSITIVITY_ADDR) / sizeof(ULONG); loopcnt++ ) {
		Ddim_Print(( "address[%d] = 0x%lx\n", loopcnt, *pt_addr ));
		pt_addr++;
	}
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_52: "
INT32 CT_Im_B2r_1_52( UCHAR pipe_no )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif
	UINT32 loopcnt;
	const T_IM_B2R_CTRL_RDMA_HPF_ADDR* addr;
	ULONG* pt_addr;

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Get_RdmaAddr_HPF_Cntl( pipe_no, &addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
	pt_addr = (ULONG*)addr;
	for( loopcnt = 0; loopcnt < sizeof(T_IM_B2R_CTRL_RDMA_HPF_ADDR) / sizeof(ULONG); loopcnt++ ) {
		Ddim_Print(( "address[%d] = 0x%lx\n", loopcnt, *pt_addr ));
		pt_addr++;
	}
#endif

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_53: "
INT32 CT_Im_B2r_1_53( UCHAR pipe_no )
{
	UCHAR ext_out_en[] = {
		[0] = D_IM_B2R_ENABLE_ON,
		[1] = D_IM_B2R_ENABLE_OFF,
	};
	UCHAR ext_out_alarm[] = {
		[0] = D_IM_B2R_EXT_OUT_ALARM_VALID_DATA,
		[1] = D_IM_B2R_EXT_OUT_ALARM_1LINE,
	};
	UINT32 loopcnt;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


	for( loopcnt = 0; loopcnt < (sizeof(ext_out_en) / sizeof(ext_out_en[0])); loopcnt++ ) {
		Ddim_Print(( "** %u\n", loopcnt ));

#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_Set_ExternalIfOutput( pipe_no, ext_out_en[loopcnt], ext_out_alarm[loopcnt] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
		Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.B2R_CMN.DINSEL.bit.EXTOEN   = %u\n", IO_B2R_P1.B2R_CMN.DINSEL.bit.EXTOEN   ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.B2R_CMN.DINSEL.bit.EXTOEN   = %u\n", IO_B2R_P2.B2R_CMN.DINSEL.bit.EXTOEN   ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P1.B2R_CMN.EXALMSL.bit.EXALMSL = %u\n", IO_B2R_P1.B2R_CMN.EXALMSL.bit.EXALMSL ));
		Ddim_Print(( D_IM_B2R_FUNC_NAME "IO_B2R_P2.B2R_CMN.EXALMSL.bit.EXALMSL = %u\n", IO_B2R_P2.B2R_CMN.EXALMSL.bit.EXALMSL ));
		Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
#endif
	}

	return D_DDIM_OK;
}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_1_54: "
INT32 CT_Im_B2r_1_54( UCHAR pipe_no )
{
	UCHAR ext_out_en;
	UCHAR ext_out_alarm;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	if( pipe_no == D_IM_B2R_PIPE12 ){
		return D_DDIM_OK;
	}

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));


#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Set_ExternalIfOutput( pipe_no, D_IM_B2R_ENABLE_ON, D_IM_B2R_EXT_OUT_ALARM_3LINE );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Get_ExternalIfOutput( pipe_no, &ext_out_en, &ext_out_alarm );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	Ddim_Print(( D_IM_B2R_FUNC_NAME "ext_out_en    = %u\n", ext_out_en ));
	Ddim_Print(( D_IM_B2R_FUNC_NAME "ext_out_alarm = %u\n", ext_out_alarm ));
#endif

	return D_DDIM_OK;
}

static INT32 ct_im_b2r_run_1( UCHAR pipe_no, const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-1-1-1
		case 1:
			return CT_Im_B2r_1_1( pipe_no );

		// Test-1-1-2
		case 2:
			return CT_Im_B2r_1_2( pipe_no );

		// Test-1-1-3
		case 3:
			return CT_Im_B2r_1_3( pipe_no );

		// Test-1-1-4
		case 4:
			return CT_Im_B2r_1_4( pipe_no );

		// Test-1-1-5
		case 5:
			return CT_Im_B2r_1_5( pipe_no );

		// Test-1-1-6
		case 6:
			return CT_Im_B2r_1_6( pipe_no );

		// Test-1-1-7
		case 7:
			return CT_Im_B2r_1_7( pipe_no );

		// Test-1-1-8
		case 8:
			return CT_Im_B2r_1_8( pipe_no );

		// Test-1-1-9
		case 9:
			return CT_Im_B2r_1_9( pipe_no );

		// Test-1-1-10
		case 10:
			return CT_Im_B2r_1_10( pipe_no );

		// Test-1-1-11
		case 11:
			return CT_Im_B2r_1_11( pipe_no );

		// Test-1-1-12
		case 12:
			return CT_Im_B2r_1_12( pipe_no );

		// Test-1-1-13
		case 13:
			return CT_Im_B2r_1_13( pipe_no );

		// Test-1-1-14
		case 14:
			return CT_Im_B2r_1_14( pipe_no );

		// Test-1-1-15
		case 15:
			return CT_Im_B2r_1_15( pipe_no );

		// Test-1-1-16
		case 16:
			return CT_Im_B2r_1_16( pipe_no );

		// Test-1-1-17
		case 17:
			return CT_Im_B2r_1_17( pipe_no );

		// Test-1-1-18
		case 18:
			return CT_Im_B2r_1_18( pipe_no );

		// Test-1-1-19
		case 19:
			return CT_Im_B2r_1_19( pipe_no );

		// Test-1-1-20
		case 20:
			return CT_Im_B2r_1_20( pipe_no );

		// Test-1-1-21
		case 21:
			return CT_Im_B2r_1_21( pipe_no );

		// Test-1-1-22
		case 22:
			return CT_Im_B2r_1_22( pipe_no );

		// Test-1-1-23
		case 23:
			return CT_Im_B2r_1_23( pipe_no );

		// Test-1-1-24
		case 24:
			return CT_Im_B2r_1_24( pipe_no );

		// Test-1-1-25
		case 25:
			return CT_Im_B2r_1_25( pipe_no );

		// Test-1-1-26
		case 26:
			return CT_Im_B2r_1_26( pipe_no );

		// Test-1-1-27
		case 27:
			return CT_Im_B2r_1_27( pipe_no );

		// Test-1-1-28
		case 28:
			return CT_Im_B2r_1_28( pipe_no );

		// Test-1-1-29
		case 29:
			return CT_Im_B2r_1_29( pipe_no );

		// Test-1-1-30
		case 30:
			return CT_Im_B2r_1_30( pipe_no );

		// Test-1-1-31
		case 31:
			return CT_Im_B2r_1_31( pipe_no );

		// Test-1-1-32
		case 32:
			return CT_Im_B2r_1_32( pipe_no );

		// Test-1-1-33
		case 33:
			return CT_Im_B2r_1_33( pipe_no );

		// Test-1-1-34
		case 34:
			return CT_Im_B2r_1_34( pipe_no );

		// Test-1-1-35
		case 35:
			return CT_Im_B2r_1_35( pipe_no );

		// Test-1-1-36
		case 36:
			return CT_Im_B2r_1_36( pipe_no );

		// Test-1-1-37
		case 37:
			return CT_Im_B2r_1_37( pipe_no );

		// Test-1-1-38
		case 38:
			return CT_Im_B2r_1_38( pipe_no );

		// Test-1-1-39
		case 39:
			return CT_Im_B2r_1_39( pipe_no );

		// Test-1-1-40
		case 40:
			return CT_Im_B2r_1_40( pipe_no );

		// Test-1-1-41
		case 41:
			return CT_Im_B2r_1_41( pipe_no );

		// Test-1-1-42
		case 42:
			return CT_Im_B2r_1_42( pipe_no );

		// Test-1-1-43
		case 43:
			return CT_Im_B2r_1_43( pipe_no );

		// Test-1-1-44
		case 44:
			return CT_Im_B2r_1_44( pipe_no );

		// Test-1-1-45
		case 45:
			return CT_Im_B2r_1_45( pipe_no );

		// Test-1-1-46
		case 46:
			return CT_Im_B2r_1_46( pipe_no );

		// Test-1-1-47
		case 47:
			return CT_Im_B2r_1_47( pipe_no );

		// Test-1-1-48
		case 48:
			return CT_Im_B2r_1_48( pipe_no );

		// Test-1-1-49
		case 49:
			return CT_Im_B2r_1_49( pipe_no );

		// Test-1-1-50
		case 50:
			return CT_Im_B2r_1_50( pipe_no );

		// Test-1-1-51
		case 51:
			return CT_Im_B2r_1_51( pipe_no );

		// Test-1-1-52
		case 52:
			return CT_Im_B2r_1_52( pipe_no );

		// Test-1-1-53
		case 53:
			return CT_Im_B2r_1_53( pipe_no );

		// Test-1-1-54
		case 54:
			return CT_Im_B2r_1_54( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}

#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_2_1: "
INT32 CT_Im_B2r_2_1( UCHAR pipe_no )
{
	T_IM_B2R_RECT b2r_rect_param;
	T_IM_B2R_INADDR_INFO b2r_in_addr;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	UINT32 loopcnt;
	UCHAR bank_index;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

#ifdef CO_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 2;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 2;
	}
#endif	//CO_DEBUG_ON_PC

	b2r_rect_param.input_size.img_top = 0;
	b2r_rect_param.input_size.img_left = 0;
	b2r_rect_param.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH;
	b2r_rect_param.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES;
	b2r_rect_param.output_size.output_global_w = D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U8;

	b2r_in_addr.raw.addr = (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP;

	memset( &b2r_out_addr, '\x0', sizeof(b2r_out_addr) );
	b2r_out_addr.bank_initial_position = D_IM_B2R_YBW_BANK_3;
	b2r_out_addr.use_bank_num = D_IM_B2R_YBW_BANK_3;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "Status\n" ));
	Im_B2R_Print_Status();
#endif

	for( loopcnt = 0; loopcnt < D_IM_B2R_YBW_BANK_MAX; loopcnt++ ) {
#ifdef D_IM_B2R_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			gIM_B2R_macro_fake_finish[0] = 1;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			gIM_B2R_macro_fake_finish[1] = 1;
		}
#endif
		Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));


#ifdef CO_MSG_PRINT_ON
		Ddim_Print(( D_IM_B2R_FUNC_NAME "loopcnt = %u\n", loopcnt ));
		ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#ifdef CO_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 3;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 3;
		}
#endif	//CO_DEBUG_ON_PC

		ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_raw16bit );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
		Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#ifdef CO_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 3;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 3;
		}
#endif	//CO_DEBUG_ON_PC
		Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			gIM_B2R_macro_fake_finish[0] = 0;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			gIM_B2R_macro_fake_finish[1] = 0;
		}
#endif

		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
		}
#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			ercd = Im_B2R_Get_OutBankIndex( 0, &bank_index );
			Ddim_Print(( D_IM_B2R_FUNC_NAME "%d bank_idx=%u\n", ercd, bank_index ));
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			ercd = Im_B2R_Get_OutBankIndex( 1, &bank_index );
			Ddim_Print(( D_IM_B2R_FUNC_NAME "%d bank_idx=%u\n", ercd, bank_index ));
		}

		ercd = Im_B2R_Stop( pipe_no );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#ifdef CO_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 2;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 2;
		}
#endif	//CO_DEBUG_ON_PC
#else
		Im_B2R_WaitEnd( NULL, flgptn, 60 );
		Im_B2R_Get_OutBankIndex( pipe_no, &bank_index );
		Im_B2R_Stop( pipe_no );
#ifdef CO_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 2;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 2;
		}
#endif	//CO_DEBUG_ON_PC
#endif


#ifdef IM_B2R_STATUS_PRINT
		Im_B2R_Print_Status();
#endif
	}

	return D_DDIM_OK;
}



#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_2_2: "
INT32 CT_Im_B2r_2_2( UCHAR pipe_no )
{
	T_IM_B2R_RECT b2r_rect_param;
	T_IM_B2R_INADDR_INFO b2r_in_addr;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	UINT32 loopcnt;
	UCHAR bank_index;
	DDIM_USER_FLGPTN flgptn = 0;
	T_IM_B2R_CTRL_B2B_DIRECT b2r_ctrl_b2b_direct[] = {
		[0] = {	// max
			.even_line_start_timming = 0x3FF,
			.frame_stop = 1,
		},
	};
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

#ifdef CO_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 2;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 2;
	}
#endif	//CO_DEBUG_ON_PC
	b2r_rect_param.input_size.img_top = 0;
	b2r_rect_param.input_size.img_left = 0;
	b2r_rect_param.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH;
	b2r_rect_param.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES;
	b2r_rect_param.output_size.output_global_w = D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U8;

	b2r_in_addr.raw.addr = (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP;

	memset( &b2r_out_addr, '\x0', sizeof(b2r_out_addr) );
	b2r_out_addr.bank_initial_position = D_IM_B2R_YBW_BANK_3;
	b2r_out_addr.use_bank_num = D_IM_B2R_YBW_BANK_3;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Ctrl( pipe_no, &gct_im_b2r_ctrl_base );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_base );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "Status\n" ));
	Im_B2R_Print_Status();
#endif

	for( loopcnt = 0; loopcnt < D_IM_B2R_YBW_BANK_MAX; loopcnt++ ) {
#ifdef D_IM_B2R_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			gIM_B2R_macro_fake_finish[0] = 1;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			gIM_B2R_macro_fake_finish[1] = 1;
		}
#endif
		Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));


#ifdef CO_MSG_PRINT_ON
		Ddim_Print(( D_IM_B2R_FUNC_NAME "loopcnt = %u\n", loopcnt ));
		ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#ifdef CO_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 3;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 3;
		}
#endif	//CO_DEBUG_ON_PC

		ercd = Im_B2R_Ctrl_ModeB2BDirect( pipe_no, &b2r_ctrl_b2b_direct[0] );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
		Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#ifdef CO_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 3;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 3;
		}
#endif	//CO_DEBUG_ON_PC
		Im_B2R_Ctrl_ModeB2BDirect( pipe_no, &b2r_ctrl_b2b_direct[0] );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			gIM_B2R_macro_fake_finish[0] = 0;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			gIM_B2R_macro_fake_finish[1] = 0;
		}
#endif

		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
		}
#ifdef CO_MSG_PRINT_ON
		ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			ercd = Im_B2R_Get_OutBankIndex( 0, &bank_index );
			Ddim_Print(( D_IM_B2R_FUNC_NAME "%d bank_idx=%u\n", ercd, bank_index ));
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			ercd = Im_B2R_Get_OutBankIndex( 1, &bank_index );
			Ddim_Print(( D_IM_B2R_FUNC_NAME "%d bank_idx=%u\n", ercd, bank_index ));
		}

		ercd = Im_B2R_Stop( pipe_no );
		Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#ifdef CO_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 2;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 2;
		}
#endif	//CO_DEBUG_ON_PC
#else
		Im_B2R_WaitEnd( NULL, flgptn, 60 );
		Im_B2R_Get_OutBankIndex( pipe_no, &bank_index );
		Im_B2R_Stop( pipe_no );
#ifdef CO_DEBUG_ON_PC
		if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
			IO_B2R_P1.YBR.YBRTRG.bit.YBRTRG = 2;
		}
		if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
			IO_B2R_P2.YBR.YBRTRG.bit.YBRTRG = 2;
		}
#endif	//CO_DEBUG_ON_PC
#endif


#ifdef IM_B2R_STATUS_PRINT
		Im_B2R_Print_Status();
#endif
	}

	return D_DDIM_OK;
}


static INT32 ct_im_b2r_run_2( UCHAR pipe_no, const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-1-2-1
		case 1:
			return CT_Im_B2r_2_1( pipe_no );

		case 2:
			return CT_Im_B2r_2_2( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}



#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_1: "
INT32 CT_Im_B2r_3_1( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw16bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_2: "
INT32 CT_Im_B2r_3_2( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw16bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_p12;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.ybw.output_dtype = D_IM_B2R_STL_DTYP_PACK12;
	b2r_ctrl.ybw.knee_enable = 0;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_B_ADDR_TOP;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_3: "
INT32 CT_Im_B2r_3_3( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw12bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_4: "
INT32 CT_Im_B2r_3_4( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw12bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_p12;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.ybw.output_dtype = D_IM_B2R_STL_DTYP_PACK12;
	b2r_ctrl.ybw.knee_enable = 0;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_B_ADDR_TOP;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_5: "
INT32 CT_Im_B2r_3_5( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw10bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 2, 1 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 2, 1 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_6: "
INT32 CT_Im_B2r_3_6( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw10bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_p12;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.ybw.output_dtype = D_IM_B2R_STL_DTYP_PACK12;
	b2r_ctrl.ybw.knee_enable = 0;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_B_ADDR_TOP;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 2, 1 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 2, 1 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_7: "
INT32 CT_Im_B2r_3_7( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw8bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 3, 1 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 3, 1 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_8: "
INT32 CT_Im_B2r_3_8( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw8bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_p12;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.ybw.output_dtype = D_IM_B2R_STL_DTYP_PACK12;
	b2r_ctrl.ybw.knee_enable = 0;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_B_ADDR_TOP;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 3, 1 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 3, 1 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_9: "
INT32 CT_Im_B2r_3_9( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_B2B_DIRECT b2r_ctrl_b2b_direct = gct_im_b2r_ctrl_b2b_direct;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	USHORT ring_pixs;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	Im_B2R_Get_HRingPixs( 0, &ring_pixs );
	b2r_ctrl_b2b_direct.even_line_start_timming -= ring_pixs /2;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

	ct_im_b2r_set_b2b( (E_IM_PRO_UNIT_NUM)pipe_no );

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	// Set clock to B2B-B2R direct input mode.
	Dd_Top_Set_CLKSTOP10_B2R1CK( 1 );	// ★B2R1 Clock Stop
	Dd_Top_Set_CLKSEL4_B2R1CLK( 24 );	// ★B2R1 Clock select(SRO1CLK_2)
	Dd_Top_Set_CLKSTOP10_B2R1CK( 0 );	// ★B2R1 Clock Start
	Dd_Top_Set_CLKSTOP12_B2R2CK( 1 );	// ★B2R2 Clock Stop
	Dd_Top_Set_CLKSEL6_B2R2CLK( 24 );	// ★B2R2 Clock select(SRO2CLK_2)
	Dd_Top_Set_CLKSTOP12_B2R2CK( 0 );	// ★B2R2 Clock Start
#endif

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeB2BDirect( pipe_no, &b2r_ctrl_b2b_direct );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeB2BDirect( pipe_no, &b2r_ctrl_b2b_direct );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	ct_im_b2r_set_wb_gain( pipe_no );

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	ct_im_b2r_start_b2b( (E_IM_PRO_UNIT_NUM)pipe_no );
	ct_im_b2r_waitend_stop_b2b( (E_IM_PRO_UNIT_NUM)pipe_no );

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 30 );
	Im_B2R_Stop( D_IM_B2R_IRIDIX_WDR_STOP_SECURITY_RESET_ON );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_10: "
INT32 CT_Im_B2r_3_10( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_B2B_DIRECT b2r_ctrl_b2b_direct = gct_im_b2r_ctrl_b2b_direct;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_p12;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
	USHORT ring_pixs;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.ybw.output_dtype = D_IM_B2R_STL_DTYP_PACK12;
	b2r_ctrl.ybw.knee_enable = 0;

	Im_B2R_Get_HRingPixs( 1, &ring_pixs );
	b2r_ctrl_b2b_direct.even_line_start_timming -= ring_pixs / 2;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_B_ADDR_TOP;

	ct_im_b2r_set_b2b( (E_IM_PRO_UNIT_NUM)pipe_no );

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	// Set clock to B2B-B2R direct input mode.
	Dd_Top_Set_CLKSTOP10_B2R1CK( 1 );	// ★B2R1 Clock Stop
	Dd_Top_Set_CLKSEL4_B2R1CLK( 24 );	// ★B2R1 Clock select(SRO1CLK_2)
	Dd_Top_Set_CLKSTOP10_B2R1CK( 0 );	// ★B2R1 Clock Start
	Dd_Top_Set_CLKSTOP12_B2R2CK( 1 );	// ★B2R2 Clock Stop
	Dd_Top_Set_CLKSEL6_B2R2CLK( 24 );	// ★B2R2 Clock select(SRO2CLK_2)
	Dd_Top_Set_CLKSTOP12_B2R2CK( 0 );	// ★B2R2 Clock Start
#endif

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeB2BDirect( pipe_no, &b2r_ctrl_b2b_direct );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeB2BDirect( pipe_no, &b2r_ctrl_b2b_direct );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	ct_im_b2r_set_wb_gain( pipe_no );

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	ct_im_b2r_start_b2b( (E_IM_PRO_UNIT_NUM)pipe_no );
	ct_im_b2r_waitend_stop_b2b( (E_IM_PRO_UNIT_NUM)pipe_no );

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 30 );
	Im_B2R_Stop( D_IM_B2R_IRIDIX_WDR_STOP_SECURITY_RESET_ON );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_11: "
INT32 CT_Im_B2r_3_11( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw16bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.line_intr.count = D_IM_B2R_DST_IMG_PIXS_LINES /2;
	b2r_ctrl.b2r_user_handler = ct_im_b2r_handler;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "Status\n" ));
	Im_B2R_Print_Status();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_other_reg();

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_Status();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_17: "
INT32 CT_Im_B2r_3_17( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	return CT_Im_B2r_3_11( pipe_no );
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_18: "
INT32 CT_Im_B2r_3_18( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw16bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_rect_param.input_size.img_top = D_IM_B2R_SRC_IMG_PIXS_LINES / 4 - 1;
	b2r_rect_param.input_size.img_left = D_IM_B2R_SRC_IMG_PIXS_WIDTH / 4 - 1;
	b2r_rect_param.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH / 2;
	b2r_rect_param.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES / 2;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "Status\n" ));
	Im_B2R_Print_Status();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_other_reg();

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_Status();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_19: "
INT32 CT_Im_B2r_3_19( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	return CT_Im_B2r_3_18( pipe_no );
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_20: "
INT32 CT_Im_B2r_3_20( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	return CT_Im_B2r_3_18( pipe_no );
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_21: "
INT32 CT_Im_B2r_3_21( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw12bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_rect_param.input_size.img_top = D_IM_B2R_SRC_IMG_PIXS_LINES / 4;
	b2r_rect_param.input_size.img_left = D_IM_B2R_SRC_IMG_PIXS_WIDTH / 4 - 1;
	b2r_rect_param.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH / 2;
	b2r_rect_param.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES / 2;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "Status\n" ));
	Im_B2R_Print_Status();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_other_reg();

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_Status();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_22: "
INT32 CT_Im_B2r_3_22( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	return CT_Im_B2r_3_21( pipe_no );
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_23: "
INT32 CT_Im_B2r_3_23( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw10bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_rect_param.input_size.img_top = D_IM_B2R_SRC_IMG_PIXS_LINES / 4 - 1;
	b2r_rect_param.input_size.img_left = D_IM_B2R_SRC_IMG_PIXS_WIDTH / 4;
	b2r_rect_param.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH / 2;
	b2r_rect_param.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES / 2;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 2, 1 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 2, 1 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "Status\n" ));
	Im_B2R_Print_Status();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_other_reg();

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_Status();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_24: "
INT32 CT_Im_B2r_3_24( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	return CT_Im_B2r_3_23( pipe_no );
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_25: "
INT32 CT_Im_B2r_3_25( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw8bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_rect_param.input_size.img_top = D_IM_B2R_SRC_IMG_PIXS_LINES / 4 - 1;
	b2r_rect_param.input_size.img_left = D_IM_B2R_SRC_IMG_PIXS_WIDTH / 4 - 1;
	b2r_rect_param.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH / 2;
	b2r_rect_param.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES / 2;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 3, 1 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 3, 1 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "Status\n" ));
	Im_B2R_Print_Status();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_other_reg();

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_Status();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_26: "
INT32 CT_Im_B2r_3_26( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	return CT_Im_B2r_3_25( pipe_no );
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_27: "
INT32 CT_Im_B2r_3_27( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_B2B_DIRECT b2r_ctrl_b2b_direct = gct_im_b2r_ctrl_b2b_direct;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
	USHORT ring_pixs;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.bayer_top_pixel = D_IM_B2R_TOP_PIXEL_GR;

	Im_B2R_Get_HRingPixs( 0, &ring_pixs );
	b2r_ctrl_b2b_direct.even_line_start_timming -= ring_pixs / 2;

	b2r_rect_param.input_size.img_top = D_IM_B2R_SRC_IMG_PIXS_LINES / 4;
	b2r_rect_param.input_size.img_left = D_IM_B2R_SRC_IMG_PIXS_WIDTH / 4 - 1;
	b2r_rect_param.input_size.img_width = D_IM_B2R_SRC_IMG_PIXS_WIDTH / 2;
	b2r_rect_param.input_size.img_lines = D_IM_B2R_SRC_IMG_PIXS_LINES / 2;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

	ct_im_b2r_set_b2b( (E_IM_PRO_UNIT_NUM)pipe_no );

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	// Set clock to B2B-B2R direct input mode.
	Dd_Top_Set_CLKSTOP10_B2R1CK( 1 );	// ★B2R1 Clock Stop
	Dd_Top_Set_CLKSEL4_B2R1CLK( 24 );	// ★B2R1 Clock select(SRO1CLK_2)
	Dd_Top_Set_CLKSTOP10_B2R1CK( 0 );	// ★B2R1 Clock Start
	Dd_Top_Set_CLKSTOP12_B2R2CK( 1 );	// ★B2R2 Clock Stop
	Dd_Top_Set_CLKSEL6_B2R2CLK( 24 );	// ★B2R2 Clock select(SRO2CLK_2)
	Dd_Top_Set_CLKSTOP12_B2R2CK( 0 );	// ★B2R2 Clock Start
#endif

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeB2BDirect( pipe_no, &b2r_ctrl_b2b_direct );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeB2BDirect( pipe_no, &b2r_ctrl_b2b_direct );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	ct_im_b2r_set_wb_gain( pipe_no );

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	ct_im_b2r_start_b2b( (E_IM_PRO_UNIT_NUM)pipe_no );
	ct_im_b2r_waitend_stop_b2b( (E_IM_PRO_UNIT_NUM)pipe_no );

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 30 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 30 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_28: "
INT32 CT_Im_B2r_3_28( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_p12;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.ybw.output_dtype = D_IM_B2R_STL_DTYP_PACK12;
	b2r_ctrl.ybw.knee_enable = 0;
	b2r_ctrl.deknee_enable = 1;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_P12_B_ADDR_TOP;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &gct_im_b2r_ctrl_sdram_in_raw8bit );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Deknee_Table( pipe_no, gct_im_b2r_deknee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Set_Deknee_Table( pipe_no, gct_im_b2r_deknee_tbl );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_29: "
INT32 CT_Im_B2r_3_29( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	return CT_Im_B2r_3_1( pipe_no );
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_30: "
INT32 CT_Im_B2r_3_30( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw16bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.cia_bypass = 1;
	b2r_ctrl.cia_padding = 0;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_31: "
INT32 CT_Im_B2r_3_31( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw16bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.cia_padding = 0;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_32: "
INT32 CT_Im_B2r_3_32( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw16bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_CTRL_TRIMMING b2r_ctrl_trimming;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl_trimming.trimming_enable = D_IM_B2R_ENABLE_ON;
	b2r_ctrl_trimming.start_x = D_IM_B2R_DST_IMG_PIXS_WIDTH / 4;
	b2r_ctrl_trimming.start_y = D_IM_B2R_DST_IMG_PIXS_LINES / 4;
	b2r_ctrl_trimming.width = D_IM_B2R_DST_IMG_PIXS_WIDTH / 2;
	b2r_ctrl_trimming.lines = D_IM_B2R_DST_IMG_PIXS_LINES / 2;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Trimming( pipe_no, &b2r_ctrl_trimming );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Ctrl_Trimming( pipe_no, &b2r_ctrl_trimming );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "Status\n" ));
	Im_B2R_Print_Status();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_other_reg();

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_Status();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_33: "
INT32 CT_Im_B2r_3_33( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw16bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u8;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;

	// 180 degree rotation.
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)(((ULONG)b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R)
															  + (b2r_rect_param.output_size.output_global_w
															  * (b2r_rect_param.input_size.img_lines -1)));
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)(((ULONG)b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G)
															  + (b2r_rect_param.output_size.output_global_w
															  * (b2r_rect_param.input_size.img_lines -1)));
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)(((ULONG)b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B)
															  + (b2r_rect_param.output_size.output_global_w
															  * (b2r_rect_param.input_size.img_lines -1)));

	b2r_rect_param.output_size.output_global_w = -b2r_rect_param.output_size.output_global_w;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_R, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_G, gct_im_b2r_knee_tbl );
	Im_B2R_Set_Knee_Table( pipe_no, E_B2R_RGBTBL_B, gct_im_b2r_knee_tbl );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	ct_im_b2r_set_wb_gain( pipe_no );

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "Status\n" ));
	Im_B2R_Print_Status();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();

#ifdef IM_B2R_STATUS_PRINT
	Im_B2R_Print_Status();
#endif

	return D_DDIM_OK;
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_34: "
INT32 CT_Im_B2r_3_34( UCHAR pipe_no )
{
	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	return CT_Im_B2r_3_1( pipe_no );
}


#undef D_IM_B2R_FUNC_NAME
#define D_IM_B2R_FUNC_NAME "CT_Im_B2r_3_35: "
INT32 CT_Im_B2r_3_35( UCHAR pipe_no )
{
	T_IM_B2R_CTRL b2r_ctrl = gct_im_b2r_ctrl_raw;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in = gct_im_b2r_ctrl_sdram_in_raw16bit;
	T_IM_B2R_RECT b2r_rect_param = gct_im_b2r_rect_param_out_rgb_u16;
	T_IM_B2R_INADDR_INFO b2r_in_addr = gct_im_b2r_in_addr_raw;
	T_IM_B2R_OUTBANK_INFO b2r_out_addr;
	DDIM_USER_FLGPTN flgptn = 0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_B2R_FUNC_NAME ));

	b2r_ctrl.ybw.output_dtype = D_IM_B2R_STL_DTYP_PACK16;
	b2r_ctrl.ybw.knee_enable = 0;

	memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
	b2r_out_addr = gct_im_b2r_out_addr_rgb;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U16_R_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U16_G_ADDR_TOP;
	b2r_out_addr.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U16_B_ADDR_TOP;

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ct_im_b2r_set_wb_gain( pipe_no );
#else
	Im_B2R_Ctrl( pipe_no, &b2r_ctrl );
	Im_B2R_Ctrl_ModeSDRAMInput( pipe_no, &b2r_ctrl_sdram_in );
	Im_B2R_Ctrl_Left_Shift( pipe_no, 0, 0 );
	Im_B2R_Set_Rect( pipe_no, &b2r_rect_param );
	Im_B2R_Set_InAddr_Info( pipe_no, &b2r_in_addr );
	Im_B2R_Set_OutBankInfo( pipe_no, &b2r_out_addr );
#endif

	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", 0 ));

#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 1;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 1;
	}
#endif
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R Start\n" ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_Start( pipe_no, D_IM_B2R_START_WITH_YBW );
#endif

#ifdef D_IM_B2R_DEBUG_ON_PC
	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_macro_fake_finish[0] = 0;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_macro_fake_finish[1] = 0;
	}
#endif

	if( ct_im_b2r_check_target_pipe_no_1( pipe_no ) ){
		flgptn |= D_IM_B2R1_INT_FLG_YBW_END;
	}
	if( ct_im_b2r_check_target_pipe_no_2( pipe_no ) ){
		flgptn |= D_IM_B2R2_INT_FLG_YBW_END;
	}
#ifdef CO_MSG_PRINT_ON
	ercd = Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));

	ercd = Im_B2R_Stop( pipe_no );
	Ddim_Print(( D_IM_B2R_FUNC_NAME "0x%x\n", ercd ));
#else
	Im_B2R_WaitEnd( NULL, flgptn, 60 * 10 );
	Im_B2R_Stop( pipe_no );
#endif

	ct_im_b2r_print_b2r_common_reg();
	ct_im_b2r_print_b2r_ybr_reg();
	ct_im_b2r_print_b2r_ybw_reg();
	ct_im_b2r_print_b2r_ctrl_reg();
	ct_im_b2r_print_b2r_ctrl_modesdraminput_reg();
	ct_im_b2r_print_b2r_ctrl_modeb2bdirect_reg();
	ct_im_b2r_print_rect_reg();
	ct_im_b2r_print_trimming_reg();
	ct_im_b2r_print_other_reg();
	ct_im_b2r_print_fb2r_ctrl_reg();
	ct_im_b2r_print_axi_reg();


#ifdef IM_B2R_STATUS_PRINT
	Ddim_Print(( D_IM_B2R_FUNC_NAME "B2R End Status\n" ));
	Im_B2R_Print_Status();
	Im_B2R_Print_ClockStatus();
	Im_B2R_Print_AccEnStatus();
#endif

	return D_DDIM_OK;
}


static INT32 ct_im_b2r_run_3( UCHAR pipe_no, const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-1-2-1
		case 1:
			return CT_Im_B2r_3_1( pipe_no );

		case 2:
			return CT_Im_B2r_3_2( pipe_no );

		case 3:
			return CT_Im_B2r_3_3( pipe_no );

		case 4:
			return CT_Im_B2r_3_4( pipe_no );

		case 5:
			return CT_Im_B2r_3_5( pipe_no );

		case 6:
			return CT_Im_B2r_3_6( pipe_no );

		case 7:
			return CT_Im_B2r_3_7( pipe_no );

		case 8:
			return CT_Im_B2r_3_8( pipe_no );

		case 9:
			return CT_Im_B2r_3_9( pipe_no );

		case 10:
			return CT_Im_B2r_3_10( pipe_no );

		case 11:
			return CT_Im_B2r_3_11( pipe_no );

		case 17:
			return CT_Im_B2r_3_17( pipe_no );

		case 18:
			return CT_Im_B2r_3_18( pipe_no );

		case 19:
			return CT_Im_B2r_3_19( pipe_no );

		case 20:
			return CT_Im_B2r_3_20( pipe_no );

		case 21:
			return CT_Im_B2r_3_21( pipe_no );

		case 22:
			return CT_Im_B2r_3_22( pipe_no );

		case 23:
			return CT_Im_B2r_3_23( pipe_no );

		case 24:
			return CT_Im_B2r_3_24( pipe_no );

		case 25:
			return CT_Im_B2r_3_25( pipe_no );

		case 26:
			return CT_Im_B2r_3_26( pipe_no );

		case 27:
			return CT_Im_B2r_3_27( pipe_no );

		case 28:
			return CT_Im_B2r_3_28( pipe_no );

		case 29:
			return CT_Im_B2r_3_29( pipe_no );

		case 30:
			return CT_Im_B2r_3_30( pipe_no );

		case 31:
			return CT_Im_B2r_3_31( pipe_no );

		case 32:
			return CT_Im_B2r_3_32( pipe_no );

		case 33:
			return CT_Im_B2r_3_33( pipe_no );

		case 34:
			return CT_Im_B2r_3_34( pipe_no );

		case 35:
			return CT_Im_B2r_3_35( pipe_no );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}


VOID CT_Im_B2R_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
#ifdef CO_MSG_PRINT_ON
	INT32 result = D_DDIM_INPUT_PARAM_ERROR;
#endif
	static BOOL is_init = 0;
	UCHAR loop_cnt;

	Ddim_Print(( "CT_Im_B2R_Run( %u, %u, %u ) Begin\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	gct_im_b2r_in_addr_raw.raw.addr = (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP;
	gct_im_b2r_out_addr_rgb.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP;
	gct_im_b2r_out_addr_rgb.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP;
	gct_im_b2r_out_addr_rgb.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B = (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP;


	if( is_init == 0 ) {
#ifdef CO_DEBUG_ON_PC
		memset( (VOID*)&IO_B2R_P1, '\0', sizeof(IO_B2R_P1) );
		memset( (VOID*)&IO_B2R_P2, '\0', sizeof(IO_B2R_P2) );
		memset( (VOID*)&IO_B2R_P3, '\0', sizeof(IO_B2R_P3) );
		memset( (VOID*)&IO_B2R_TBL_P1, '\0', sizeof(IO_B2R_TBL_P1) );
		memset( (VOID*)&IO_B2R_TBL_P2, '\0', sizeof(IO_B2R_TBL_P2) );
		memset( (VOID*)&IO_B2R_TBL_P3, '\0', sizeof(IO_B2R_TBL_P3) );
#endif // CO_DEBUG_ON_PC
		if( !((ct_idx_1st == 1) && (ct_idx_2nd == 1)) ) {
			Ddim_Print(( "Im_B2R_Init() Begin\n" ));
			Im_B2R_Init( D_IM_B2R_PIPE12 );
			Ddim_Print(( "Im_B2R_Init() End\n" ));
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#ifdef CO_DEBUG_ON_PC
			IO_CHIPTOP.CLKSTOP10.bit.B2R1CK = 0;
			IO_CHIPTOP.CLKSTOP10.bit.B2R1AP = 0;
			IO_CHIPTOP.CLKSTOP10.bit.B2R1AH = 0;
			IO_CHIPTOP.CLKSTOP10.bit.B2R1AX = 0;
			IO_CHIPTOP.CLKSTOP12.bit.B2R2CK = 0;
			IO_CHIPTOP.CLKSTOP12.bit.B2R2AP = 0;
			IO_CHIPTOP.CLKSTOP12.bit.B2R2AH = 0;
			IO_CHIPTOP.CLKSTOP12.bit.B2R2AX = 0;
#endif // CO_DEBUG_ON_PC
#endif
			is_init = 1;
		}
	}

	if(( ct_idx_1st == 1 ) || ( ct_idx_1st == 2 )){
#ifdef CO_DEBUG_ON_PC
		for( loop_cnt = D_IM_B2R_PIPE1; loop_cnt <= D_IM_B2R_PIPE2; loop_cnt++ ){
#else // CO_DEBUG_ON_PC
		for( loop_cnt = D_IM_B2R_PIPE1; loop_cnt <= D_IM_B2R_PIPE12; loop_cnt++ ){
#endif // CO_DEBUG_ON_PC
			switch( ct_idx_1st ) {
				case 1:
					D_IM_B2R_RESULT( ct_im_b2r_run_1( loop_cnt, ct_idx_2nd ) );
					break;
	
				case 2:
					D_IM_B2R_RESULT( ct_im_b2r_run_2( loop_cnt, ct_idx_2nd ) );
					break;
	
				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}

#ifdef CO_MSG_PRINT_ON
			Ddim_Print(( "result = 0x%x\n", result ));
#endif
			Ddim_Print(( "CT_Im_B2R_Run( %u, %u, %u) End\n", ct_idx_1st, ct_idx_2nd, loop_cnt ));
		}
	}
	else{
		D_IM_B2R_RESULT( ct_im_b2r_run_3( ct_idx_3rd, ct_idx_2nd ) );
#ifdef CO_MSG_PRINT_ON
		Ddim_Print(( "result = 0x%x\n", result ));
#endif
		Ddim_Print(( "CT_Im_B2R_Run( %u, %u, %u) End\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));
	}

	return;
}


// for Debug console interface
VOID CT_Im_B2R_Main( INT32 argc, CHAR** argv )
{
	if( argc < 3 ) {
		Ddim_Print(( "Ct_Im_B2R_Main: parameter error.\n" ));
		return;
	}

	CT_Im_B2R_Run( (UINT32)atoi((const char*)argv[1]),
				   (UINT32)atoi((const char*)argv[2]),
				   (UINT32)atoi((const char*)argv[3]) );
}

