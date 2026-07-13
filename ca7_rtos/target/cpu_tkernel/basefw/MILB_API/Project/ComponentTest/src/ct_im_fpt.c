/**
 * @file		ct_im_fpt.c
 * @brief		FPT ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copybright 2016 Socionext Inc.</I></B>
 */

#include "ct_im_fpt.h"

#include "im_fpt.h"

#include "driver_common.h"
#include "ddim_user_custom.h"
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
#endif

#include "jdsfpt.h"

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
#ifdef D_IM_FPT_DEBUG_ON_PC
extern volatile UCHAR gIM_FPT_macro_fake_finish[2];
#endif

#ifdef CO_MSG_PRINT_ON
#define D_IM_FPT_RESULT(statement)		result = statement
#else
#define D_IM_FPT_RESULT
#endif

static VOID ct_im_fpt_handler( UINT32 result, UCHAR ch );

// Max/Min table
static T_IM_FPT_MODE0 gct_im_fpt_ctrl_mode0[] = {
	[0] = {	// max
		.global_width = 0x3FF8,
		.width = 4480,
		.lines = 3000,
		.lutsel = E_IM_FPT_LUTSEL_1,
		.window_size = D_IM_FPT_WINDOW_7x7,
		.down_sample_x = D_IM_FPT_DS_MAX,
		.down_sample_y = D_IM_FPT_DS_MAX,
		.img_addr = 0xFFFFFFF8,
		.feature_amount_addr = 0xFFFFFFF8,
		.feature_amount_line_byte = 0x3FF8,
		.histgram_addr = 0xFFFFFFF8,
	},
	[1] = {	// min
		.global_width = 13,
		.width = 13,
		.lines = 13,
		.lutsel = E_IM_FPT_LUTSEL_OFF,
		.window_size = D_IM_FPT_WINDOW_5x5,
		.down_sample_x = D_IM_FPT_DS_MIN,
		.down_sample_y = D_IM_FPT_DS_MIN,
		.img_addr = 0,
		.feature_amount_addr = 0,
		.feature_amount_line_byte = 0,
		.histgram_addr = 0,
	},
};

static T_IM_FPT_MODE1 gct_im_fpt_ctrl_mode1[] = {
	[0] = {	// max
		.num_feature_point = 127,
		.output = D_IM_FPT_OUT_COORD_AND_FEATURE_AMOUNT,
		.global_width = 0x3FF8,
		.width = 640,
		.lines = 640,
		.lutsel = E_IM_FPT_LUTSEL_1,
		.window_size = D_IM_FPT_WINDOW_7x7,
		.down_sample_x = D_IM_FPT_DS_MAX,
		.down_sample_y = D_IM_FPT_DS_MAX,
		.img_addr = 0xFFFFFFF8,
		.input_coord_addr = 0xFFFFFFF8,
		.output_addr = 0xFFFFFFF8,
	},
	[1] = {	// min
		.num_feature_point = 1,
		.output = D_IM_FPT_OUT_COORD_ONLY,
		.global_width = 13,
		.width = 13,
		.lines = 13,
		.lutsel = E_IM_FPT_LUTSEL_OFF,
		.window_size = D_IM_FPT_WINDOW_5x5,
		.down_sample_x = D_IM_FPT_DS_MIN,
		.down_sample_y = D_IM_FPT_DS_MIN,
		.img_addr = 0,
		.input_coord_addr = 0,
		.output_addr = 0,
	},
};

static T_IM_FPT_MODE2 gct_im_fpt_ctrl_mode2[] = {
	[0] = {	// max
		.base_global_width = 0x3FF8,
		.target_global_width = 0x3FF8,
		.num_feature_point = 127,
		.usage_target_coord = D_IM_FPT_SAME_AS_BASE_COORD,
		.down_sample_x = D_IM_FPT_DS_MAX,
		.down_sample_y = D_IM_FPT_DS_MAX,
		.lutcfg = E_IM_FPT_LUTCFG_BASE_0__TARGET_1,
		.search_window = {
			.left = 0x800,
			.right = 0x8FF,
			.top = 0x800,
			.bottom = 0x8FF,
		},
		.tracking_threshold = 0x8FF,
		.num_iteration = 10,
		.base_img_addr = 0xFFFFFFF8,
		.target_img_addr = 0xFFFFFFF8,
		.base_coord_addr = 0xFFFFFFF8,
		.target_coord_addr = 0xFFFFFFF8,
		.tracking_result_addr = 0xFFFFFFF8,
	},
	[1] = {	// min
		.base_global_width = 13,
		.target_global_width = 13,
		.num_feature_point = 1,
		.usage_target_coord = D_IM_FPT_READ_FROM_RAM,
		.down_sample_x = D_IM_FPT_DS_MIN,
		.down_sample_y = D_IM_FPT_DS_MIN,
		.lutcfg = E_IM_FPT_LUTCFG_OFF,
		.search_window = {
			.left = 0,
			.right = 0,
			.top = 0,
			.bottom = 0,
		},
		.tracking_threshold = 0,
		.num_iteration = 1,
		.base_img_addr = 0,
		.target_img_addr = 0,
		.base_coord_addr = 0,
		.target_coord_addr = 0,
		.tracking_result_addr = 0,
	},
};

static T_IM_FPT_MODE3 gct_im_fpt_ctrl_mode3[] = {
	[0] = {	// max
		.width = 640,
		.lines = 640,
		.window_size = D_IM_FPT_WINDOW_7x7,
		.output_addr = 0xFFFFFFF8,
		.base_global_width = 0x3FF8,
		.target_global_width = 0x3FF8,
		.num_feature_point = 127,
		.usage_target_coord = D_IM_FPT_SAME_AS_BASE_COORD,
		.down_sample_x = D_IM_FPT_DS_MAX,
		.down_sample_y = D_IM_FPT_DS_MAX,
		.lutcfg = E_IM_FPT_LUTCFG_BASE_0__TARGET_1,
		.search_window = {
			.left = 0x800,
			.right = 0x8FF,
			.top = 0x800,
			.bottom = 0x8FF,
		},
		.tracking_threshold = 0x8FF,
		.num_iteration = 10,
		.base_img_addr = 0xFFFFFFF8,
		.target_img_addr = 0xFFFFFFF8,
		.base_coord_addr = 0xFFFFFFF8,
		.target_coord_addr = 0xFFFFFFF8,
		.tracking_result_addr = 0xFFFFFFF8,
	},
	[1] = {	// min
		.width = 13,
		.lines = 13,
		.window_size = D_IM_FPT_WINDOW_5x5,
		.output_addr = 0,
		.base_global_width = 13,
		.target_global_width = 13,
		.num_feature_point = 1,
		.usage_target_coord = D_IM_FPT_READ_FROM_RAM,
		.down_sample_x = D_IM_FPT_DS_MIN,
		.down_sample_y = D_IM_FPT_DS_MIN,
		.lutcfg = E_IM_FPT_LUTCFG_OFF,
		.search_window = {
			.left = 0,
			.right = 0,
			.top = 0,
			.bottom = 0,
		},
		.tracking_threshold = 0,
		.num_iteration = 1,
		.base_img_addr = 0,
		.target_img_addr = 0,
		.base_coord_addr = 0,
		.target_coord_addr = 0,
		.tracking_result_addr = 0,
	},
};

// TestBase table
static T_IM_FPT_MODE0 gct_im_fpt_ctrl_base_mode0 = {
	.global_width = 200,
	.width = 200,
	.lines = 150,
	.lutsel = E_IM_FPT_LUTSEL_0,
	.window_size = D_IM_FPT_WINDOW_5x5,
	.down_sample_x = D_IM_FPT_DS_MIN,
	.down_sample_y = D_IM_FPT_DS_MIN,
	.img_addr = 0x60000000,
	.feature_amount_addr = 0x68000000,
	.feature_amount_line_byte = 200 * 2,
	.histgram_addr = 0x70000000,
};

static T_IM_FPT_MODE1 gct_im_fpt_ctrl_base_mode1 = {
	.num_feature_point = 100,
	.output = D_IM_FPT_OUT_COORD_ONLY,
	.global_width = 200,
	.width = 13,
	.lines = 13,
	.lutsel = E_IM_FPT_LUTSEL_OFF,
	.window_size = D_IM_FPT_WINDOW_5x5,
	.down_sample_x = D_IM_FPT_DS_MIN,
	.down_sample_y = D_IM_FPT_DS_MIN,
	.img_addr = 0x60000000,
	.input_coord_addr = 0x68000000,
	.output_addr = 0x70000000,
};

static T_IM_FPT_MODE2 gct_im_fpt_ctrl_base_mode2 = {
	.base_global_width = 200,
	.target_global_width = 200,
	.num_feature_point = 100,
	.usage_target_coord = D_IM_FPT_SAME_AS_BASE_COORD,
	.down_sample_x = D_IM_FPT_DS_MIN,
	.down_sample_y = D_IM_FPT_DS_MIN,
	.lutcfg = E_IM_FPT_LUTCFG_BASE_1__TARGET_0,
	.search_window = {
		.left = 0x600,
		.right = 0x600,
		.top = 0x600,
		.bottom = 0x600,
	},
	.tracking_threshold = 0x19,
	.num_iteration = 10,
	.base_img_addr = 0x60000000,
	.target_img_addr = 0x64000000,
	.base_coord_addr = 0x68000000,
	.target_coord_addr = 0x6C000000,
	.tracking_result_addr = 0x70000000,
};

static T_IM_FPT_MODE3 gct_im_fpt_ctrl_base_mode3 = {
		.width = 13,
		.lines = 13,
		.window_size = D_IM_FPT_WINDOW_5x5,
		.output_addr = 0x74000000,
		.base_global_width = 200,
		.target_global_width = 200,
		.num_feature_point = 100,
		.usage_target_coord = D_IM_FPT_SAME_AS_BASE_COORD,
		.down_sample_x = D_IM_FPT_DS_MIN,
		.down_sample_y = D_IM_FPT_DS_MIN,
		.lutcfg = E_IM_FPT_LUTCFG_BASE_1__TARGET_0,
		.search_window = {
			.left = 0x600,
			.right = 0x600,
			.top = 0x600,
			.bottom = 0x600,
		},
		.tracking_threshold = 0x19,
		.num_iteration = 10,
		.base_img_addr = 0x60000000,
		.target_img_addr = 0x64000000,
		.base_coord_addr = 0x68000000,
		.target_coord_addr = 0x6C000000,
		.tracking_result_addr = 0x70000000,
};

static UCHAR gct_im_fpt_lut_tbl[2][256] = {
	{
		  1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,
		 17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,
		 33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
		 49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,
		 65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,
		 81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,
		 97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
		113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128,
		129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144,
		145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
		161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
		177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
		193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
		209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
		225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
		241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,   0,
	},
	{
		255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240,
		239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224,
		223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209, 208,
		207, 206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193, 192,
		191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 180, 179, 178, 177, 176,
		175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162, 161, 160,
		159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144,
		143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128,
		127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112,
		111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100,  99,  98,  97,  96,
		 95,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  83,  82,  81,  80,
		 79,  78,  77,  76,  75,  74,  73,  72,  71,  70,  69,  68,  67,  66,  65,  64,
		 63,  62,  61,  60,  59,  58,  57,  56,  55,  54,  53,  52,  51,  50,  49,  48,
		 47,  46,  45,  44,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,  32,
		 31,  30,  29,  28,  27,  26,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16,
		 15,  14,  13,  12,  11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0,
	},
};

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef IM_FPT_STATUS_PRINT
extern VOID Im_FPT_Print_Status( VOID );
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// nothing special.


static VOID ct_im_fpt_handler( UINT32 result, UCHAR ch )
{
	if( ((result) & D_IM_FPT_INTERRUPT_COMPLETE) != 0 ) {
		Ddim_Print(( "FPT%d end.\n", ch ));
	}
	if( ((result) & D_IM_FPT_INTERRUPT_AXI_ERR) != 0 ) {
		Ddim_Print(( "FPT%d AXI error.\n", ch ));
	}
	if( ((result) & D_IM_FPT_INTERRUPT_PARAM_ERR) != 0 ) {
		Ddim_Print(( "FPT%d param setting error.\n", ch ));
	}
}

static VOID ct_im_fpt_on_pclk( UCHAR ch )
{
#ifdef CO_ACT_PCLOCK
	if( ch == 0 ){
		Dd_Top_Set_CLKSTOP4_FPT0AP( 0 );
	}
	else{
		Dd_Top_Set_CLKSTOP4_FPT1AP( 0 );
	}
#endif
}

static VOID ct_im_fpt_off_pclk( UCHAR ch )
{
#ifdef CO_ACT_PCLOCK
	if( ch == 0 ){
		Dd_Top_Set_CLKSTOP4_FPT0AP( 1 );
	}
	else{
		Dd_Top_Set_CLKSTOP4_FPT1AP( 1 );
	}
#endif
}

static VOID ct_im_fpt_on_hclk( UCHAR ch )
{
#ifdef CO_ACT_HCLOCK
	if( ch == 0 ){
		Dd_Top_Set_CLKSTOP4_FPT0AH( 0 );
	}
	else{
		Dd_Top_Set_CLKSTOP4_FPT1AH( 0 );
	}
#endif
}

static VOID ct_im_fpt_off_hclk( UCHAR ch )
{
#ifdef CO_ACT_HCLOCK
	if( ch == 0 ){
		Dd_Top_Set_CLKSTOP4_FPT0AH( 1 );
	}
	else{
		Dd_Top_Set_CLKSTOP4_FPT1AH( 1 );
	}
#endif
}

static VOID ct_im_fpt_print_mode0_reg( UCHAR ch )
{
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL2   = 0x%lx\n", ch, IO_FPT[ch].FPTCTL2.word   ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL3   = 0x%lx\n", ch, IO_FPT[ch].FPTCTL3.word   ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL0 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL0.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL1 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL1.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL2 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL2.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL3 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL3.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL4 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL4.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL5 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL5.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL8 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL8.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL9 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL9.word ));
	ct_im_fpt_off_pclk( ch );
}

static VOID ct_im_fpt_print_mode1_reg( UCHAR ch )
{
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL2   = 0x%lx\n", ch, IO_FPT[ch].FPTCTL2.word   ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL3   = 0x%lx\n", ch, IO_FPT[ch].FPTCTL3.word   ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL0 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL0.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL1 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL1.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL2 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL2.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL3 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL3.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL6 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL6.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL7 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL7.word ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL9 = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL9.word ));
	ct_im_fpt_off_pclk( ch );
}

static VOID ct_im_fpt_print_mode2_reg( UCHAR ch )
{
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL2    = 0x%lx\n", ch, IO_FPT[ch].FPTCTL2.word    ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL3    = 0x%lx\n", ch, IO_FPT[ch].FPTCTL3.word    ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL0  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL0.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL1  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL1.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL2  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL2.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL3  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL3.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL4  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL4.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL5  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL5.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL6  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL6.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL7  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL7.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL8  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL8.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL9  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL9.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL10 = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL10.word ));
	ct_im_fpt_off_pclk( ch );
}

static VOID ct_im_fpt_print_mode3_reg( UCHAR ch )
{
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "\n" ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL2    = 0x%lx\n", ch, IO_FPT[ch].FPTCTL2.word    ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL3    = 0x%lx\n", ch, IO_FPT[ch].FPTCTL3.word    ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL1  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL1.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL2  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL2.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL7  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL7.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL0  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL0.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL1  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL1.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL2  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL2.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL3  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL3.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL4  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL4.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL5  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL5.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL6  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL6.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL7  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL7.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL8  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL8.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL9  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL9.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL10 = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL10.word ));
	ct_im_fpt_off_pclk( ch );
}

static VOID ct_im_fpt_print_all_reg( UCHAR ch )
{
	USHORT loop;

	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "APB\n" ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL0    = 0x%lx\n", ch, IO_FPT[ch].FPTCTL0.word    ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL1    = 0x%lx\n", ch, IO_FPT[ch].FPTCTL1.word    ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL2    = 0x%lx\n", ch, IO_FPT[ch].FPTCTL2.word    ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL0  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL0.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL1  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL1.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL2  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL2.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL3  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL3.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL4  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL4.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL5  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL5.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL6  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL6.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL7  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL7.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL8  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL8.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFQCTL9  = 0x%lx\n", ch, IO_FPT[ch].FPTFQCTL9.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL0  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL0.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL1  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL1.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL2  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL2.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL3  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL3.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL4  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL4.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL5  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL5.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL6  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL6.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL7  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL7.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL8  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL8.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL9  = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL9.word  ));
	Ddim_Print(( "IO_FPT[%d].FPTFTCTL10 = 0x%lx\n", ch, IO_FPT[ch].FPTFTCTL10.word ));
	Ddim_Print(( "IO_FPT[%d].FPTINTE0   = 0x%lx\n", ch, IO_FPT[ch].FPTINTE0.word   ));
	Ddim_Print(( "IO_FPT[%d].FPTINTF0   = 0x%lx\n", ch, IO_FPT[ch].FPTINTF0.word   ));
	Ddim_Print(( "IO_FPT[%d].FPTAXICTL0 = 0x%lx\n", ch, IO_FPT[ch].FPTAXICTL0.word ));
	Ddim_Print(( "IO_FPT[%d].FPTAXIERR0 = 0x%lx\n", ch, IO_FPT[ch].FPTAXIERR0.word ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL3    = 0x%lx\n", ch, IO_FPT[ch].FPTCTL3.word    ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL4    = 0x%lx\n", ch, IO_FPT[ch].FPTCTL4.word    ));
	Ddim_Print(( "IO_FPT[%d].FPTCTL5    = 0x%lx\n", ch, IO_FPT[ch].FPTCTL5.word    ));

	ct_im_fpt_on_hclk( ch );
	Im_FPT_Open( ch, 100 );
	Ddim_Print(( "AHB\n" ));
	for( loop = 0; loop < 256 / 4; loop++ ){
		Ddim_Print(( "IO_FPT_AHB[%d].FPTLUT0[%d] = 0x%lx\n", ch, loop, IO_FPT_AHB[ch].FPTLUT0.word[loop] ));
	}
	for( loop = 0; loop < 256 / 4; loop++ ){
		Ddim_Print(( "IO_FPT_AHB[%d].FPTLUT1[%d] = 0x%lx\n", ch, loop, IO_FPT_AHB[ch].FPTLUT1.word[loop] ));
	}
	for( loop = 0; loop < 512 / 4; loop++ ){
		Ddim_Print(( "IO_FPT_AHB[%d].FPTREG0[%d] = 0x%lx\n", ch, loop, IO_FPT_AHB[ch].FPTREG0.word[loop] ));
	}
	for( loop = 0; loop < 512 / 4; loop++ ){
		Ddim_Print(( "IO_FPT_AHB[%d].FPTREG1[%d] = 0x%lx\n", ch, loop, IO_FPT_AHB[ch].FPTREG1.word[loop] ));
	}
	Im_FPT_Close( ch );
	ct_im_fpt_off_hclk( ch );
	ct_im_fpt_off_pclk( ch );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_1: "
INT32 CT_Im_Fpt_1_1( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Init( ch );
#ifdef CO_MSG_PRINT_ON
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#endif

	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "IO_FPT[%d].FPTCTL0    = 0x%lx\n", ch, IO_FPT[ch].FPTCTL0.word ));
	Ddim_Print(( "IO_FPT[%d].FPTAXICTL0 = 0x%lx\n", ch, IO_FPT[ch].FPTAXICTL0.word ));
	ct_im_fpt_off_pclk( ch );

	return ercd;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_2: "
INT32 CT_Im_Fpt_1_2( UCHAR ch )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode0( ch, &gct_im_fpt_ctrl_mode0[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode0( ch, &gct_im_fpt_ctrl_mode0[0] );
#endif
	ct_im_fpt_print_mode0_reg( ch );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode0( ch, &gct_im_fpt_ctrl_mode0[1] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode0( ch, &gct_im_fpt_ctrl_mode0[1] );
#endif
	ct_im_fpt_print_mode0_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_3: "
INT32 CT_Im_Fpt_1_3( UCHAR ch )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode1( ch, &gct_im_fpt_ctrl_mode1[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode1( ch, &gct_im_fpt_ctrl_mode1[0] );
#endif
	ct_im_fpt_print_mode1_reg( ch );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode1( ch, &gct_im_fpt_ctrl_mode1[1] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode1( ch, &gct_im_fpt_ctrl_mode1[1] );
#endif
	ct_im_fpt_print_mode1_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_4: "
INT32 CT_Im_Fpt_1_4( UCHAR ch )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode2( ch, &gct_im_fpt_ctrl_mode2[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode2( ch, &gct_im_fpt_ctrl_mode2[0] );
#endif
	ct_im_fpt_print_mode2_reg( ch );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode2( ch, &gct_im_fpt_ctrl_mode2[1] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode2( ch, &gct_im_fpt_ctrl_mode2[1] );
#endif
	ct_im_fpt_print_mode2_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_5: "
INT32 CT_Im_Fpt_1_5( UCHAR ch )
{
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode3( ch, &gct_im_fpt_ctrl_mode3[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode3( ch, &gct_im_fpt_ctrl_mode3[0] );
#endif
	ct_im_fpt_print_mode3_reg( ch );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode3( ch, &gct_im_fpt_ctrl_mode3[1] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode3( ch, &gct_im_fpt_ctrl_mode3[1] );
#endif
	ct_im_fpt_print_mode3_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_6: "
INT32 CT_Im_Fpt_1_6( UCHAR ch )
{
	T_IM_FPT_MODE_CONFIG param[2] = {
		{
			// max
			.io_mode = E_IM_FPT_IO_MODE_REGISTER,
			.pipe_en = E_IM_FPT_PIPE_EN_ENABLE,
			.debug_mode = E_IM_FPT_DEBUG_MODE_DEBUG,
		},
		{
			// min
			.io_mode = E_IM_FPT_IO_MODE_SDRAM,
			.pipe_en = E_IM_FPT_PIPE_EN_DISABLE,
			.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL,
		},
	};
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode_Config( ch, &param[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode_Config( ch, &param[0] );
#endif
	Ddim_Print(( "\n" ));
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "IO_FPT[%d].FPTCTL2 = 0x%lx\n", ch, IO_FPT[ch].FPTCTL2.word ));
	ct_im_fpt_off_pclk( ch );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode_Config( ch, &param[1] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode_Config( ch, &param[1] );
#endif
	Ddim_Print(( "\n" ));
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "IO_FPT[%d].FPTCTL2 = 0x%lx\n", ch, IO_FPT[ch].FPTCTL2.word ));
	ct_im_fpt_off_pclk( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_7: "
INT32 CT_Im_Fpt_1_7( UCHAR ch )
{
	T_IM_FPT_BRC_CONFIG param[2] = {
		{
			// max
			.img_offset = 0xFF,
			.img_clip_max = 0xFF,
			.img_clip_min = 0xFF,
		},
		{
			// min
			.img_offset = -0x100,
			.img_clip_max = 0,
			.img_clip_min = 0,
		},
	};
	T_IM_FPT_MODE0 ctrl_param = gct_im_fpt_ctrl_base_mode0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ctrl_param.lutsel = E_IM_FPT_LUTSEL_0;
#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode0( ch, &ctrl_param );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));

	ercd = Im_FPT_Ctrl_BRC_Config_Base_Img( ch, &param[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode0( ch, &ctrl_param );
	Im_FPT_Ctrl_BRC_Config_Base_Img( ch, &param[0] );
#endif

	Ddim_Print(( "\n" ));
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "IO_FPT[%d].FPTCTL4 = 0x%lx\n", ch, IO_FPT[ch].FPTCTL4.word ));
	ct_im_fpt_off_pclk( ch );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_BRC_Config_Base_Img( ch, &param[1] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_BRC_Config_Base_Img( ch, &param[1] );
#endif
	Ddim_Print(( "\n" ));
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "IO_FPT[%d].FPTCTL4 = 0x%lx\n", ch, IO_FPT[ch].FPTCTL4.word ));
	ct_im_fpt_off_pclk( ch );

	ctrl_param.lutsel = E_IM_FPT_LUTSEL_OFF;
#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode0( ch, &ctrl_param );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode0( ch, &ctrl_param );
#endif

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_BRC_Config_Base_Img( ch, &param[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_BRC_Config_Base_Img( ch, &param[0] );
#endif
	Ddim_Print(( "\n" ));
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "IO_FPT[%d].FPTCTL4 = 0x%lx\n", ch, IO_FPT[ch].FPTCTL4.word ));
	ct_im_fpt_off_pclk( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_8: "
INT32 CT_Im_Fpt_1_8( UCHAR ch )
{
	T_IM_FPT_BRC_CONFIG param[2] = {
		{
			// max
			.img_offset = 0xFF,
			.img_clip_max = 0xFF,
			.img_clip_min = 0xFF,
		},
		{
			// min
			.img_offset = -0x100,
			.img_clip_max = 0,
			.img_clip_min = 0,
		},
	};
	T_IM_FPT_MODE0 ctrl_param = gct_im_fpt_ctrl_base_mode0;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ctrl_param.lutsel = E_IM_FPT_LUTSEL_0;
#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode0( ch, &ctrl_param );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));

	ercd = Im_FPT_Ctrl_BRC_Config_Target_Img( ch, &param[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode0( ch, &ctrl_param );
	Im_FPT_Ctrl_BRC_Config_Target_Img( ch, &param[0] );
#endif
	Ddim_Print(( "\n" ));
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "IO_FPT[%d].FPTCTL5 = 0x%lx\n", ch, IO_FPT[ch].FPTCTL5.word ));
	ct_im_fpt_off_pclk( ch );

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_BRC_Config_Target_Img( ch, &param[1] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_BRC_Config_Target_Img( ch, &param[1] );
#endif
	Ddim_Print(( "\n" ));
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "IO_FPT[%d].FPTCTL5 = 0x%lx\n", ch, IO_FPT[ch].FPTCTL5.word ));
	ct_im_fpt_off_pclk( ch );

	ctrl_param.lutsel = E_IM_FPT_LUTSEL_OFF;
#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode0( ch, &ctrl_param );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));

	ercd = Im_FPT_Ctrl_BRC_Config_Target_Img( ch, &param[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode0( ch, &ctrl_param );
	Im_FPT_Ctrl_BRC_Config_Target_Img( ch, &param[0] );
#endif
	Ddim_Print(( "\n" ));
	ct_im_fpt_on_pclk( ch );
	Ddim_Print(( "IO_FPT[%d].FPTCTL5 = 0x%lx\n", ch, IO_FPT[ch].FPTCTL5.word ));
	ct_im_fpt_off_pclk( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_9: "
INT32 CT_Im_Fpt_1_9( UCHAR ch )
{
	UCHAR lut[256];
	USHORT loop;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Open( ch, 100 );

	for( loop = 0; loop < 256; loop++ ){
		lut[loop] = loop;
	}

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_LUTRAM( ch, 0, &lut[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_LUTRAM( ch, 0, &lut[0] );
#endif

	Ddim_Print(( "\n" ));
	ct_im_fpt_on_hclk( ch );
	for( loop = 0; loop < 256 / 4; loop++ ){
		Ddim_Print(( "IO_FPT_AHB[%d].FPTLUT0 = 0x%lx\n", ch, IO_FPT_AHB[ch].FPTLUT0.word[loop] ));
	}
	ct_im_fpt_off_hclk( ch );


	for( loop = 0; loop < 256; loop++ ){
		lut[loop] = 255 - loop;
	}

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_LUTRAM( ch, 1, &lut[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_LUTRAM( ch, 1, &lut[0] );
#endif

	Ddim_Print(( "\n" ));
	ct_im_fpt_on_hclk( ch );
	for( loop = 0; loop < 256 / 4; loop++ ){
		Ddim_Print(( "IO_FPT_AHB[%d].FPTLUT1 = 0x%lx\n", ch, IO_FPT_AHB[ch].FPTLUT1.word[loop] ));
	}
	ct_im_fpt_off_hclk( ch );

	Im_FPT_Close( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_10: "
INT32 CT_Im_Fpt_1_10( UCHAR ch )
{
	USHORT reg[256];
	USHORT loop;
	T_IM_FPT_MODE_CONFIG param;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Open( ch, 100 );

	param.io_mode = E_IM_FPT_IO_MODE_REGISTER;
	param.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	param.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;
#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode_Config( ch, &param );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode_Config( ch, &param );
#endif

	for( loop = 0; loop < 256; loop+=2 ){
		reg[loop]   = loop;
		reg[loop+1] = loop + 1;
	}

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Set_REGRAM( ch, 0, (UCHAR*)&reg[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Set_REGRAM( ch, 0, (UCHAR*)&reg[0] );
#endif

	Ddim_Print(( "\n" ));
	ct_im_fpt_on_hclk( ch );
	for( loop = 0; loop < 512 / 4; loop++ ){
		Ddim_Print(( "IO_FPT_AHB[%d].FPTREG0 = 0x%lx\n", ch, IO_FPT_AHB[ch].FPTREG0.word[loop] ));
	}
	ct_im_fpt_off_hclk( ch );


	for( loop = 0; loop < 256; loop+=2 ){
		reg[loop]   = 65535 - loop;
		reg[loop+1] = 65535 - loop - 1;
	}

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Set_REGRAM( ch, 1, (UCHAR*)&reg[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Set_REGRAM( ch, 1, (UCHAR*)&reg[0] );
#endif

	Ddim_Print(( "\n" ));
	ct_im_fpt_on_hclk( ch );
	for( loop = 0; loop < 512 / 4; loop++ ){
		Ddim_Print(( "IO_FPT_AHB[%d].FPTREG1 = 0x%lx\n", ch, IO_FPT_AHB[ch].FPTREG1.word[loop] ));
	}
	ct_im_fpt_off_hclk( ch );

	Im_FPT_Close( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_11: "
INT32 CT_Im_Fpt_1_11( UCHAR ch )
{
	USHORT reg[256];
	ULONG get_reg[128];
	USHORT loop;
	T_IM_FPT_MODE_CONFIG param;
#ifdef CO_MSG_PRINT_ON
	INT32 ercd;
#endif

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Open( ch, 100 );

	param.io_mode = E_IM_FPT_IO_MODE_REGISTER;
	param.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	param.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;
#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Ctrl_Mode_Config( ch, &param );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Ctrl_Mode_Config( ch, &param );
#endif

	for( loop = 0; loop < 256; loop+=2 ){
		reg[loop]   = loop;
		reg[loop+1] = loop + 1;
	}

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Set_REGRAM( ch, 0, (UCHAR*)&reg[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));

	ercd = Im_FPT_Get_REGRAM( ch, 0, &get_reg[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Set_REGRAM( ch, 0, (UCHAR*)&reg[0] );
	Im_FPT_Get_REGRAM( ch, 0, &get_reg[0] );
#endif

	Ddim_Print(( "\n" ));
	for( loop = 0; loop < 128; loop++ ){
		Ddim_Print(( "get_reg[%d] = 0x%lx\n", loop, get_reg[loop] ));
	}


	for( loop = 0; loop < 256; loop+=2 ){
		reg[loop]   = 65535 - loop;
		reg[loop+1] = 65535 - loop - 1;
	}

#ifdef CO_MSG_PRINT_ON
	ercd = Im_FPT_Set_REGRAM( ch, 1, (UCHAR*)&reg[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));

	ercd = Im_FPT_Get_REGRAM( ch, 1, &get_reg[0] );
	Ddim_Print(( D_IM_FPT_FUNC_NAME "%d\n", ercd ));
#else
	Im_FPT_Set_REGRAM( ch, 1, (UCHAR*)&reg[0] );
	Im_FPT_Get_REGRAM( ch, 1, &get_reg[0] );
#endif

	Ddim_Print(( "\n" ));
	for( loop = 0; loop < 128; loop++ ){
		Ddim_Print(( "get_reg[%d] = 0x%lx\n", loop, get_reg[loop] ));
	}

	Im_FPT_Close( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_1_12: "
INT32 CT_Im_Fpt_1_12( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Open( ch, 100 );

	Im_FPT_DebugDump_All_Registers( ch, 0x60000000, 0x68000000 );

	Im_FPT_Close( ch );

	return D_DDIM_OK;
}

static INT32 ct_im_fpt_run_1( UCHAR ch, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-1-1
		case 1:
			return CT_Im_Fpt_1_1( ch );

		// Test-1-1-2
		case 2:
			return CT_Im_Fpt_1_2( ch );

		// Test-1-1-3
		case 3:
			return CT_Im_Fpt_1_3( ch );

		// Test-1-1-4
		case 4:
			return CT_Im_Fpt_1_4( ch );

		// Test-1-1-5
		case 5:
			return CT_Im_Fpt_1_5( ch );

		// Test-1-1-6
		case 6:
			return CT_Im_Fpt_1_6( ch );

		// Test-1-1-7
		case 7:
			return CT_Im_Fpt_1_7( ch );

		// Test-1-1-8
		case 8:
			return CT_Im_Fpt_1_8( ch );

		// Test-1-1-9
		case 9:
			return CT_Im_Fpt_1_9( ch );

		// Test-1-1-10
		case 10:
			return CT_Im_Fpt_1_10( ch );

		// Test-1-1-11
		case 11:
			return CT_Im_Fpt_1_11( ch );

		// Test-1-1-12
		case 12:
			return CT_Im_Fpt_1_12( ch );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}



#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_1: "
INT32 CT_Im_Fpt_2_1( UCHAR ch )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Open( ch, 100 );
	Im_FPT_Init( ch );
	Im_FPT_Ctrl_Mode0( ch, &gct_im_fpt_ctrl_mode0[0] );
	Im_FPT_Start( ch );
	ercd = Im_FPT_Init( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Init error = 0x%x\n", ercd ));
	}
	Im_FPT_Stop( ch );
	Im_FPT_Close( ch );

	return ercd;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_2: "
INT32 CT_Im_Fpt_2_2( UCHAR ch )
{
	INT32 ercd;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Open0 error = 0x%x\n", ercd ));
		return ercd;
	}

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Open1 error = 0x%x\n", ercd ));
		Im_FPT_Close( ch );
		return ercd;
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Close error = 0x%x\n", ercd ));
		return ercd;
	}

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_3: "
INT32 CT_Im_Fpt_2_3( UCHAR ch )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Open( ch, 100 );
	Im_FPT_Init( ch );
	Im_FPT_Ctrl_Mode0( ch, &gct_im_fpt_ctrl_mode0[0] );
	Im_FPT_Start( ch );
	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Close error = 0x%x\n", ercd ));
	}
	Im_FPT_Stop( ch );
	Im_FPT_Close( ch );

	return ercd;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_4: "
INT32 CT_Im_Fpt_2_4( UCHAR ch )
{
	INT32 ercd = D_DDIM_OK;
	DDIM_USER_ID sid_back;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	sid_back = SID_IM_FPT( ch );
	SID_IM_FPT( ch ) = 0xfff;

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Close error = 0x%x\n", ercd ));
	}

	SID_IM_FPT( ch ) = sid_back;

	return ercd;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_5: "
INT32 CT_Im_Fpt_2_5( UCHAR ch )
{
	INT32 ercd = D_DDIM_OK;
	UCHAR lut[256];

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Open( ch, 100 );
	Im_FPT_Init( ch );
	Im_FPT_Ctrl_Mode0( ch, &gct_im_fpt_ctrl_mode0[0] );
	Im_FPT_Start( ch );
	ercd = Im_FPT_Ctrl_LUTRAM( ch, 0, lut );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Ctrl_LUTRAM error = 0x%x\n", ercd ));
	}
	Im_FPT_Stop( ch );
	Im_FPT_Close( ch );

	return ercd;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_6: "
INT32 CT_Im_Fpt_2_6( UCHAR ch )
{
	INT32 ercd = D_DDIM_OK;
	UCHAR reg[512];

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Open( ch, 100 );
	Im_FPT_Init( ch );
	Im_FPT_Ctrl_Mode0( ch, &gct_im_fpt_ctrl_mode0[0] );
	Im_FPT_Start( ch );
	ercd = Im_FPT_Set_REGRAM( ch, 0, reg );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Set_REGRAM error = 0x%x\n", ercd ));
	}
	Im_FPT_Stop( ch );
	Im_FPT_Close( ch );

	return ercd;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_7: "
INT32 CT_Im_Fpt_2_7( UCHAR ch )
{
	INT32 ercd = D_DDIM_OK;
	UCHAR reg[512];
	T_IM_FPT_MODE_CONFIG param;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.io_mode = E_IM_FPT_IO_MODE_SDRAM;
	param.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	param.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;

	Im_FPT_Open( ch, 100 );
	Im_FPT_Init( ch );
	Im_FPT_Ctrl_Mode_Config( ch, &param );
	ercd = Im_FPT_Set_REGRAM( ch, 0, reg );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Set_REGRAM error = 0x%x\n", ercd ));
	}
	Im_FPT_Stop( ch );
	Im_FPT_Close( ch );

	return ercd;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_8: "
INT32 CT_Im_Fpt_2_8( UCHAR ch )
{
	INT32 ercd = D_DDIM_OK;
	ULONG reg[128];

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Open( ch, 100 );
	Im_FPT_Init( ch );
	Im_FPT_Start( ch );
	ercd = Im_FPT_Get_REGRAM( ch, 0, reg );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Get_REGRAM error = 0x%x\n", ercd ));
	}
	Im_FPT_Stop( ch );
	Im_FPT_Close( ch );

	return ercd;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_9: "
INT32 CT_Im_Fpt_2_9( UCHAR ch )
{
	INT32 ercd = D_DDIM_OK;
	ULONG reg[128];
	T_IM_FPT_MODE_CONFIG param;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.io_mode = E_IM_FPT_IO_MODE_SDRAM;
	param.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	param.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;

	Im_FPT_Open( ch, 100 );
	Im_FPT_Init( ch );
	Im_FPT_Ctrl_Mode_Config( ch, &param );
	ercd = Im_FPT_Get_REGRAM( ch, 0, reg );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Get_REGRAM error = 0x%x\n", ercd ));
	}
	Im_FPT_Stop( ch );
	Im_FPT_Close( ch );

	return ercd;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_10: "
INT32 CT_Im_Fpt_2_10( UCHAR ch )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Open( ch, 100 );
	Im_FPT_Init( ch );
	Im_FPT_Ctrl_Mode0( ch, &gct_im_fpt_ctrl_mode0[0] );
	Im_FPT_Start( ch );
	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Start error = 0x%x\n", ercd ));
	}
	Im_FPT_Stop( ch );
	Im_FPT_Close( ch );

	return ercd;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_11: "
INT32 CT_Im_Fpt_2_11( UCHAR ch )
{
	INT32 ercd = D_DDIM_OK;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	return ercd;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_2_12: "
INT32 CT_Im_Fpt_2_12( UCHAR ch )
{
	INT32 ercd = D_DDIM_OK;
	UINT32 status;
	T_IM_FPT_INTERRUPT param;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = NULL;
	Im_FPT_Ctrl_Interrupt( ch, &param );

	ercd = Im_FPT_WaitEnd( ch, &status, 10 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Wait end error = 0x%x\n", ercd ));
	}

	return ercd;
}

static INT32 ct_im_fpt_run_2( UCHAR ch, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-2-1
		case 1:
			return CT_Im_Fpt_2_1( ch );

		// Test-1-2-2
		case 2:
			return CT_Im_Fpt_2_2( ch );

		// Test-1-2-3
		case 3:
			return CT_Im_Fpt_2_3( ch );

		// Test-1-2-4
		case 4:
			return CT_Im_Fpt_2_4( ch );

		// Test-1-2-5
		case 5:
			return CT_Im_Fpt_2_5( ch );

		// Test-1-2-6
		case 6:
			return CT_Im_Fpt_2_6( ch );

		// Test-1-2-7
		case 7:
			return CT_Im_Fpt_2_7( ch );

		// Test-1-2-8
		case 8:
			return CT_Im_Fpt_2_8( ch );

		// Test-1-2-9
		case 9:
			return CT_Im_Fpt_2_9( ch );

		// Test-1-2-10
		case 10:
			return CT_Im_Fpt_2_10( ch );

		// Test-1-2-11
		case 11:
			return CT_Im_Fpt_2_11( ch );

		// Test-1-2-12
		case 12:
			return CT_Im_Fpt_2_12( ch );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}



#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_1: "
INT32 CT_Im_Fpt_3_1( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_Open( ch, -2 );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_2: "
INT32 CT_Im_Fpt_3_2( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_Ctrl_Mode0( ch, NULL );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_3: "
INT32 CT_Im_Fpt_3_3( UCHAR ch )
{
	INT32 ercd;
	T_IM_FPT_MODE0 param = gct_im_fpt_ctrl_base_mode0;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.down_sample_x = D_IM_FPT_DS_MIN - 1;
	ercd = Im_FPT_Ctrl_Mode0( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	param.down_sample_x = D_IM_FPT_DS_MAX + 1;
	ercd = Im_FPT_Ctrl_Mode0( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_4: "
INT32 CT_Im_Fpt_3_4( UCHAR ch )
{
	INT32 ercd;
	T_IM_FPT_MODE0 param = gct_im_fpt_ctrl_base_mode0;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.down_sample_y = D_IM_FPT_DS_MIN - 1;
	ercd = Im_FPT_Ctrl_Mode0( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	param.down_sample_y = D_IM_FPT_DS_MAX + 1;
	ercd = Im_FPT_Ctrl_Mode0( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_5: "
INT32 CT_Im_Fpt_3_5( UCHAR ch )
{
	T_IM_FPT_MODE0 param = gct_im_fpt_ctrl_base_mode0;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.lutsel = 3;
	return Im_FPT_Ctrl_Mode0( ch, &param );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_6: "
INT32 CT_Im_Fpt_3_6( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_Ctrl_Mode1( ch, NULL );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_7: "
INT32 CT_Im_Fpt_3_7( UCHAR ch )
{
	INT32 ercd;
	T_IM_FPT_MODE1 param = gct_im_fpt_ctrl_base_mode1;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.down_sample_x = D_IM_FPT_DS_MIN - 1;
	ercd = Im_FPT_Ctrl_Mode1( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	param.down_sample_x = D_IM_FPT_DS_MAX + 1;
	ercd = Im_FPT_Ctrl_Mode1( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_8: "
INT32 CT_Im_Fpt_3_8( UCHAR ch )
{
	INT32 ercd;
	T_IM_FPT_MODE1 param = gct_im_fpt_ctrl_base_mode1;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.down_sample_y = D_IM_FPT_DS_MIN - 1;
	ercd = Im_FPT_Ctrl_Mode1( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	param.down_sample_y = D_IM_FPT_DS_MAX + 1;
	ercd = Im_FPT_Ctrl_Mode1( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_9: "
INT32 CT_Im_Fpt_3_9( UCHAR ch )
{
	T_IM_FPT_MODE1 param = gct_im_fpt_ctrl_base_mode1;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.lutsel = 3;
	return Im_FPT_Ctrl_Mode1( ch, &param );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_10: "
INT32 CT_Im_Fpt_3_10( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_Ctrl_Mode2( ch, NULL );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_11: "
INT32 CT_Im_Fpt_3_11( UCHAR ch )
{
	T_IM_FPT_MODE2 param = gct_im_fpt_ctrl_base_mode2;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.lutcfg = 5;
	return Im_FPT_Ctrl_Mode2( ch, &param );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_12: "
INT32 CT_Im_Fpt_3_12( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_Ctrl_Mode3( ch, NULL );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_13: "
INT32 CT_Im_Fpt_3_13( UCHAR ch )
{
	INT32 ercd;
	T_IM_FPT_MODE3 param = gct_im_fpt_ctrl_base_mode3;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.down_sample_x = D_IM_FPT_DS_MIN - 1;
	ercd = Im_FPT_Ctrl_Mode3( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	param.down_sample_x = D_IM_FPT_DS_MAX + 1;
	ercd = Im_FPT_Ctrl_Mode3( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_14: "
INT32 CT_Im_Fpt_3_14( UCHAR ch )
{
	INT32 ercd;
	T_IM_FPT_MODE3 param = gct_im_fpt_ctrl_base_mode3;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.down_sample_y = D_IM_FPT_DS_MIN - 1;
	ercd = Im_FPT_Ctrl_Mode3( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	param.down_sample_y = D_IM_FPT_DS_MAX + 1;
	ercd = Im_FPT_Ctrl_Mode3( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_15: "
INT32 CT_Im_Fpt_3_15( UCHAR ch )
{
	T_IM_FPT_MODE3 param = gct_im_fpt_ctrl_base_mode3;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.lutcfg = 5;
	return Im_FPT_Ctrl_Mode3( ch, &param );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_16: "
INT32 CT_Im_Fpt_3_16( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_Ctrl_Mode_Config( ch, NULL );;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_17: "
INT32 CT_Im_Fpt_3_17( UCHAR ch )
{
	T_IM_FPT_MODE_CONFIG param;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.io_mode = 2;
	param.pipe_en = 0;
	param.debug_mode = 0;

	return Im_FPT_Ctrl_Mode_Config( ch, &param );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_18: "
INT32 CT_Im_Fpt_3_18( UCHAR ch )
{
	T_IM_FPT_MODE_CONFIG param;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.io_mode = 0;
	param.pipe_en = 2;
	param.debug_mode = 0;

	return Im_FPT_Ctrl_Mode_Config( ch, &param );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_19: "
INT32 CT_Im_Fpt_3_19( UCHAR ch )
{
	T_IM_FPT_MODE_CONFIG param;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	param.io_mode = 0;
	param.pipe_en = 0;
	param.debug_mode = 2;

	return Im_FPT_Ctrl_Mode_Config( ch, &param );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_20: "
INT32 CT_Im_Fpt_3_20( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_Ctrl_BRC_Config_Base_Img( ch, NULL );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_21: "
INT32 CT_Im_Fpt_3_21( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_Ctrl_BRC_Config_Target_Img( ch, NULL );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_22: "
INT32 CT_Im_Fpt_3_22( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_Ctrl_Interrupt( ch, NULL );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_23: "
INT32 CT_Im_Fpt_3_23( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_Ctrl_LUTRAM( ch, 0, NULL );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_24: "
INT32 CT_Im_Fpt_3_24( UCHAR ch )
{
	UCHAR lut[256];

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_Ctrl_LUTRAM( ch, 2, lut );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_25: "
INT32 CT_Im_Fpt_3_25( UCHAR ch )
{
	T_IM_FPT_MODE_CONFIG config;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	config.io_mode = E_IM_FPT_IO_MODE_REGISTER;
	config.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	config.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;
	Im_FPT_Ctrl_Mode_Config( ch, &config );

	return Im_FPT_Set_REGRAM( ch, 0, NULL );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_26: "
INT32 CT_Im_Fpt_3_26( UCHAR ch )
{
	T_IM_FPT_MODE_CONFIG config;
	UCHAR reg[512];

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	config.io_mode = E_IM_FPT_IO_MODE_REGISTER;
	config.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	config.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;
	Im_FPT_Ctrl_Mode_Config( ch, &config );

	return Im_FPT_Set_REGRAM( ch, 2, reg );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_27: "
INT32 CT_Im_Fpt_3_27( UCHAR ch )
{
	T_IM_FPT_MODE_CONFIG config;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	config.io_mode = E_IM_FPT_IO_MODE_REGISTER;
	config.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	config.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;
	Im_FPT_Ctrl_Mode_Config( ch, &config );

	return Im_FPT_Get_REGRAM( ch, 0, NULL );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_28: "
INT32 CT_Im_Fpt_3_28( UCHAR ch )
{
	T_IM_FPT_MODE_CONFIG config;
	ULONG reg[128];

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	config.io_mode = E_IM_FPT_IO_MODE_REGISTER;
	config.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	config.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;
	Im_FPT_Ctrl_Mode_Config( ch, &config );

	return Im_FPT_Get_REGRAM( ch, 2, reg );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_29: "
INT32 CT_Im_Fpt_3_29( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_WaitEnd( ch, NULL, 100 );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_30: "
INT32 CT_Im_Fpt_3_30( UCHAR ch )
{
	UINT32 status;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	return Im_FPT_WaitEnd( ch, &status, -2 );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_3_31: "
INT32 CT_Im_Fpt_3_31( UCHAR ch )
{
	INT32 ercd;
	UCHAR buf;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Get_Axi_Response( ch, &buf, NULL );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	ercd = Im_FPT_Get_Axi_Response( ch, NULL, &buf );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	return D_DDIM_OK;
}

static INT32 ct_im_fpt_run_3( UCHAR ch, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-3-1
		case 1:
			return CT_Im_Fpt_3_1( ch );

		// Test-1-3-2
		case 2:
			return CT_Im_Fpt_3_2( ch );

		// Test-1-3-3
		case 3:
			return CT_Im_Fpt_3_3( ch );

		// Test-1-3-4
		case 4:
			return CT_Im_Fpt_3_4( ch );

		// Test-1-3-5
		case 5:
			return CT_Im_Fpt_3_5( ch );

		// Test-1-3-6
		case 6:
			return CT_Im_Fpt_3_6( ch );

		// Test-1-3-7
		case 7:
			return CT_Im_Fpt_3_7( ch );

		// Test-1-3-8
		case 8:
			return CT_Im_Fpt_3_8( ch );

		// Test-1-3-9
		case 9:
			return CT_Im_Fpt_3_9( ch );

		// Test-1-3-10
		case 10:
			return CT_Im_Fpt_3_10( ch );

		// Test-1-3-11
		case 11:
			return CT_Im_Fpt_3_11( ch );

		// Test-1-3-12
		case 12:
			return CT_Im_Fpt_3_12( ch );

		// Test-1-3-13
		case 13:
			return CT_Im_Fpt_3_13( ch );

		// Test-1-3-14
		case 14:
			return CT_Im_Fpt_3_14( ch );

		// Test-1-3-15
		case 15:
			return CT_Im_Fpt_3_15( ch );

		// Test-1-3-16
		case 16:
			return CT_Im_Fpt_3_16( ch );

		// Test-1-3-17
		case 17:
			return CT_Im_Fpt_3_17( ch );

		// Test-1-3-18
		case 18:
			return CT_Im_Fpt_3_18( ch );

		// Test-1-3-19
		case 19:
			return CT_Im_Fpt_3_19( ch );

		// Test-1-3-20
		case 20:
			return CT_Im_Fpt_3_20( ch );

		// Test-1-3-21
		case 21:
			return CT_Im_Fpt_3_21( ch );

		// Test-1-3-22
		case 22:
			return CT_Im_Fpt_3_22( ch );

		// Test-1-3-23
		case 23:
			return CT_Im_Fpt_3_23( ch );

		// Test-1-3-24
		case 24:
			return CT_Im_Fpt_3_24( ch );

		// Test-1-3-25
		case 25:
			return CT_Im_Fpt_3_25( ch );

		// Test-1-3-26
		case 26:
			return CT_Im_Fpt_3_26( ch );

		// Test-1-3-27
		case 27:
			return CT_Im_Fpt_3_27( ch );

		// Test-1-3-28
		case 28:
			return CT_Im_Fpt_3_28( ch );

		// Test-1-3-29
		case 29:
			return CT_Im_Fpt_3_29( ch );

		// Test-1-3-30
		case 30:
			return CT_Im_Fpt_3_30( ch );

		// Test-1-3-31
		case 31:
			return CT_Im_Fpt_3_31( ch );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}


#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_1: "
INT32 CT_Im_Fpt_4_1( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Init( ch );

	ct_im_fpt_print_all_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_2: "
INT32 CT_Im_Fpt_4_2( UCHAR ch )
{
	INT32 ercd;
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "result = %d\n", ercd ));
	}

	Im_FPT_Close( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_3: "
INT32 CT_Im_Fpt_4_3( UCHAR ch )
{
	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	Im_FPT_Open( ch, 100 );

	return Im_FPT_Close( ch );
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_4: "
INT32 CT_Im_Fpt_4_4( UCHAR ch )
{
	INT32 ercd;
	UINT32 status;
	T_IM_FPT_INTERRUPT param;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "open error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_Mode0( ch, &gct_im_fpt_ctrl_base_mode0 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl mode error = %d\n", ercd ));
	}

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = ct_im_fpt_handler;
	ercd = Im_FPT_Ctrl_Interrupt( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl interrupt error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 0, &gct_im_fpt_lut_tbl[0][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut error = %d\n", ercd ));
	}

	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "start error = %d\n", ercd ));
	}

#ifdef CO_DEBUG_ON_PC
	IO_FPT[ch].FPTINTF0.bit.__FPTFLG = 1;
	Im_FPT_Int_Handler( ch );
#endif

	ercd = Im_FPT_WaitEnd( ch, &status, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "wait end error = %d\n", ercd ));
	}

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "close error = %d\n", ercd ));
	}

	ct_im_fpt_print_mode0_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_5: "
INT32 CT_Im_Fpt_4_5( UCHAR ch )
{
	INT32 ercd;
	USHORT loop;
	UINT32 status;
	ULONG get_reg[128];
	T_IM_FPT_INTERRUPT param;
	T_IM_FPT_MODE_CONFIG config;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "open error = %d\n", ercd ));
	}

	config.io_mode = E_IM_FPT_IO_MODE_REGISTER;
	config.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	config.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;
	ercd = Im_FPT_Ctrl_Mode_Config( ch, &config );

	ercd = Im_FPT_Ctrl_Mode0( ch, &gct_im_fpt_ctrl_base_mode0 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl mode error = %d\n", ercd ));
	}

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = ct_im_fpt_handler;
	ercd = Im_FPT_Ctrl_Interrupt( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl interrupt error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 0, &gct_im_fpt_lut_tbl[0][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut error = %d\n", ercd ));
	}

	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "start error = %d\n", ercd ));
	}

#ifdef CO_DEBUG_ON_PC
	IO_FPT[ch].FPTINTF0.bit.__FPTFLG = 1;
	Im_FPT_Int_Handler( ch );
#endif

	ercd = Im_FPT_WaitEnd( ch, &status, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "wait end error = %d\n", ercd ));
	}

	ercd = Im_FPT_Get_REGRAM( ch, 1, &get_reg[0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "get regram error = %d\n", ercd ));
	}
	Ddim_Print(( "\n" ));
	for( loop = 0; loop < 128; loop++ ){
		Ddim_Print(( "get_reg[%d] = 0x%lx\n", loop, get_reg[loop] ));
	}

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "close error = %d\n", ercd ));
	}

	ct_im_fpt_print_mode0_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_6: "
INT32 CT_Im_Fpt_4_6( UCHAR ch )
{
	INT32 ercd;
	UINT32 status;
	T_IM_FPT_INTERRUPT param;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "open error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_Mode1( ch, &gct_im_fpt_ctrl_base_mode1 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl mode error = %d\n", ercd ));
	}

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = ct_im_fpt_handler;
	ercd = Im_FPT_Ctrl_Interrupt( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl interrupt error = %d\n", ercd ));
	}

	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "start error = %d\n", ercd ));
	}

#ifdef CO_DEBUG_ON_PC
	IO_FPT[ch].FPTINTF0.bit.__FPTFLG = 1;
	Im_FPT_Int_Handler( ch );
#endif

	ercd = Im_FPT_WaitEnd( ch, &status, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "wait end error = %d\n", ercd ));
	}

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "close error = %d\n", ercd ));
	}

	ct_im_fpt_print_mode1_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_7: "
INT32 CT_Im_Fpt_4_7( UCHAR ch )
{
	INT32 ercd;
	USHORT loop;
	UINT32 status;
	UCHAR* reg = (UCHAR*)0x68000000;
	ULONG get_reg[128];
	T_IM_FPT_INTERRUPT param;
	T_IM_FPT_MODE_CONFIG config;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "open error = %d\n", ercd ));
	}

	config.io_mode = E_IM_FPT_IO_MODE_REGISTER;
	config.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	config.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;
	ercd = Im_FPT_Ctrl_Mode_Config( ch, &config );

	ercd = Im_FPT_Ctrl_Mode1( ch, &gct_im_fpt_ctrl_base_mode1 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl mode error = %d\n", ercd ));
	}

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = ct_im_fpt_handler;
	ercd = Im_FPT_Ctrl_Interrupt( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl interrupt error = %d\n", ercd ));
	}

	ercd = Im_FPT_Set_REGRAM( ch, 0, (UCHAR*)&reg[0] );

	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "start error = %d\n", ercd ));
	}

#ifdef CO_DEBUG_ON_PC
	IO_FPT[ch].FPTINTF0.bit.__FPTFLG = 1;
	Im_FPT_Int_Handler( ch );
#endif

	ercd = Im_FPT_WaitEnd( ch, &status, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "wait end error = %d\n", ercd ));
	}

	ercd = Im_FPT_Get_REGRAM( ch, 1, &get_reg[0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "get regram error = %d\n", ercd ));
	}
	Ddim_Print(( "\n" ));
	for( loop = 0; loop < 128; loop++ ){
		Ddim_Print(( "get_reg[%d] = 0x%lx\n", loop, get_reg[loop] ));
	}

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "close error = %d\n", ercd ));
	}

	ct_im_fpt_print_mode1_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_8: "
INT32 CT_Im_Fpt_4_8( UCHAR ch )
{
	INT32 ercd;
	UINT32 status;
	T_IM_FPT_INTERRUPT param;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "open error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_Mode2( ch, &gct_im_fpt_ctrl_base_mode2 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl mode error = %d\n", ercd ));
	}

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = ct_im_fpt_handler;
	ercd = Im_FPT_Ctrl_Interrupt( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl interrupt error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 0, &gct_im_fpt_lut_tbl[1][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut0 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 1, &gct_im_fpt_lut_tbl[0][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut1 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "start error = %d\n", ercd ));
	}

#ifdef CO_DEBUG_ON_PC
	IO_FPT[ch].FPTINTF0.bit.__FPTFLG = 1;
	Im_FPT_Int_Handler( ch );
#endif

	ercd = Im_FPT_WaitEnd( ch, &status, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "wait end error = %d\n", ercd ));
	}

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "close error = %d\n", ercd ));
	}

	ct_im_fpt_print_mode2_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_9: "
INT32 CT_Im_Fpt_4_9( UCHAR ch )
{
	INT32 ercd;
	USHORT loop;
	UINT32 status;
	UCHAR* reg = (UCHAR*)0x68000000;
	ULONG get_reg[128];
	T_IM_FPT_INTERRUPT param;
	T_IM_FPT_MODE_CONFIG config;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "open error = %d\n", ercd ));
	}

	config.io_mode = E_IM_FPT_IO_MODE_REGISTER;
	config.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	config.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;
	ercd = Im_FPT_Ctrl_Mode_Config( ch, &config );

	ercd = Im_FPT_Ctrl_Mode2( ch, &gct_im_fpt_ctrl_base_mode2 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl mode error = %d\n", ercd ));
	}

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = ct_im_fpt_handler;
	ercd = Im_FPT_Ctrl_Interrupt( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl interrupt error = %d\n", ercd ));
	}

	ercd = Im_FPT_Set_REGRAM( ch, 0, (UCHAR*)&reg[0] );

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 0, &gct_im_fpt_lut_tbl[1][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut0 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 1, &gct_im_fpt_lut_tbl[0][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut1 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "start error = %d\n", ercd ));
	}

#ifdef CO_DEBUG_ON_PC
	IO_FPT[ch].FPTINTF0.bit.__FPTFLG = 1;
	Im_FPT_Int_Handler( ch );
#endif

	ercd = Im_FPT_WaitEnd( ch, &status, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "wait end error = %d\n", ercd ));
	}

	ercd = Im_FPT_Get_REGRAM( ch, 1, &get_reg[0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "get regram error = %d\n", ercd ));
	}
	Ddim_Print(( "\n" ));
	for( loop = 0; loop < 128; loop++ ){
		Ddim_Print(( "get_reg[%d] = 0x%lx\n", loop, get_reg[loop] ));
	}

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "close error = %d\n", ercd ));
	}

	ct_im_fpt_print_mode2_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_10: "
INT32 CT_Im_Fpt_4_10( UCHAR ch )
{
	INT32 ercd;
	UINT32 status;
	T_IM_FPT_INTERRUPT param;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "open error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_Mode3( ch, &gct_im_fpt_ctrl_base_mode3 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl mode error = %d\n", ercd ));
	}

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = ct_im_fpt_handler;
	ercd = Im_FPT_Ctrl_Interrupt( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl interrupt error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 0, &gct_im_fpt_lut_tbl[1][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut0 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 1, &gct_im_fpt_lut_tbl[0][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut1 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "start error = %d\n", ercd ));
	}

#ifdef CO_DEBUG_ON_PC
	IO_FPT[ch].FPTINTF0.bit.__FPTFLG = 1;
	Im_FPT_Int_Handler( ch );
#endif

	ercd = Im_FPT_WaitEnd( ch, &status, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "wait end error = %d\n", ercd ));
	}

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "close error = %d\n", ercd ));
	}

	ct_im_fpt_print_mode3_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_11: "
INT32 CT_Im_Fpt_4_11( UCHAR ch )
{
	INT32 ercd;
	USHORT loop;
	UINT32 status;
	UCHAR* reg = (UCHAR*)0x68000000;
	ULONG get_reg[128];
	T_IM_FPT_INTERRUPT param;
	T_IM_FPT_MODE_CONFIG config;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "open error = %d\n", ercd ));
	}

	config.io_mode = E_IM_FPT_IO_MODE_REGISTER;
	config.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	config.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;
	ercd = Im_FPT_Ctrl_Mode_Config( ch, &config );

	ercd = Im_FPT_Ctrl_Mode3( ch, &gct_im_fpt_ctrl_base_mode3 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl mode error = %d\n", ercd ));
	}

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = ct_im_fpt_handler;
	ercd = Im_FPT_Ctrl_Interrupt( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl interrupt error = %d\n", ercd ));
	}

	ercd = Im_FPT_Set_REGRAM( ch, 0, (UCHAR*)&reg[0] );

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 0, &gct_im_fpt_lut_tbl[1][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut0 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 1, &gct_im_fpt_lut_tbl[0][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut1 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "start error = %d\n", ercd ));
	}

#ifdef CO_DEBUG_ON_PC
	IO_FPT[ch].FPTINTF0.bit.__FPTFLG = 1;
	Im_FPT_Int_Handler( ch );
#endif

	ercd = Im_FPT_WaitEnd( ch, &status, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "wait end error = %d\n", ercd ));
	}

	ercd = Im_FPT_Get_REGRAM( ch, 1, &get_reg[0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "get regram error = %d\n", ercd ));
	}
	Ddim_Print(( "\n" ));
	for( loop = 0; loop < 128; loop++ ){
		Ddim_Print(( "get_reg[%d] = 0x%lx\n", loop, get_reg[loop] ));
	}

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "close error = %d\n", ercd ));
	}

	ct_im_fpt_print_mode3_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_12: "
INT32 CT_Im_Fpt_4_12( UCHAR ch )
{
	INT32 ercd;
	UINT32 status;
	T_IM_FPT_INTERRUPT param;
	T_IM_FPT_MODE_CONFIG config;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "open error = %d\n", ercd ));
	}

	config.io_mode = E_IM_FPT_IO_MODE_SDRAM;
	config.pipe_en = E_IM_FPT_PIPE_EN_ENABLE;
	config.debug_mode = E_IM_FPT_DEBUG_MODE_NORMAL;
	ercd = Im_FPT_Ctrl_Mode_Config( ch, &config );

	ercd = Im_FPT_Ctrl_Mode2( ch, &gct_im_fpt_ctrl_base_mode2 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl mode error = %d\n", ercd ));
	}

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = ct_im_fpt_handler;
	ercd = Im_FPT_Ctrl_Interrupt( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl interrupt error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 0, &gct_im_fpt_lut_tbl[1][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut0 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 1, &gct_im_fpt_lut_tbl[0][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut1 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "start error = %d\n", ercd ));
	}

#ifdef CO_DEBUG_ON_PC
	IO_FPT[ch].FPTINTF0.bit.__FPTFLG = 1;
	Im_FPT_Int_Handler( ch );
#endif

	ercd = Im_FPT_WaitEnd( ch, &status, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "wait end error = %d\n", ercd ));
	}

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "close error = %d\n", ercd ));
	}

	ct_im_fpt_print_mode2_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_13: "
INT32 CT_Im_Fpt_4_13( UCHAR ch )
{
	INT32 ercd;
	UINT32 status;
	T_IM_FPT_INTERRUPT param;
	T_IM_FPT_MODE_CONFIG config;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "open error = %d\n", ercd ));
	}

	config.io_mode = E_IM_FPT_IO_MODE_SDRAM;
	config.pipe_en = E_IM_FPT_PIPE_EN_DISABLE;
	config.debug_mode = E_IM_FPT_DEBUG_MODE_DEBUG;
	ercd = Im_FPT_Ctrl_Mode_Config( ch, &config );

	ercd = Im_FPT_Ctrl_Mode3( ch, &gct_im_fpt_ctrl_base_mode3 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl mode error = %d\n", ercd ));
	}

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = ct_im_fpt_handler;
	ercd = Im_FPT_Ctrl_Interrupt( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl interrupt error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 0, &gct_im_fpt_lut_tbl[1][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut0 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Ctrl_LUTRAM( ch, 1, &gct_im_fpt_lut_tbl[0][0] );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl lut1 error = %d\n", ercd ));
	}

	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "start error = %d\n", ercd ));
	}

#ifdef CO_DEBUG_ON_PC
	IO_FPT[ch].FPTINTF0.bit.__FPTFLG = 1;
	Im_FPT_Int_Handler( ch );
#endif

	ercd = Im_FPT_WaitEnd( ch, &status, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "wait end error = %d\n", ercd ));
	}

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "close error = %d\n", ercd ));
	}

	ct_im_fpt_print_mode3_reg( ch );

	return D_DDIM_OK;
}

#undef D_IM_FPT_FUNC_NAME
#define D_IM_FPT_FUNC_NAME "CT_Im_Fpt_4_14: "
INT32 CT_Im_Fpt_4_14( UCHAR ch )
{
	INT32 ercd;
	UINT32 status;
	T_IM_FPT_INTERRUPT param;
	T_IM_FPT_MODE2 mode2_ctrl = gct_im_fpt_ctrl_base_mode2;

	Ddim_Print(( "%s\n", D_IM_FPT_FUNC_NAME ));

	ercd = Im_FPT_Open( ch, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "open error = %d\n", ercd ));
	}

	mode2_ctrl.lutcfg = E_IM_FPT_LUTCFG_OFF;
	ercd = Im_FPT_Ctrl_Mode2( ch, &mode2_ctrl );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl mode error = %d\n", ercd ));
	}

	param.flag = (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR);
	param.callback = ct_im_fpt_handler;
	ercd = Im_FPT_Ctrl_Interrupt( ch, &param );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "ctrl interrupt error = %d\n", ercd ));
	}

	ercd = Im_FPT_Start( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "start error = %d\n", ercd ));
	}

#ifdef CO_DEBUG_ON_PC
	IO_FPT[ch].FPTINTF0.bit.__FPTFLG = 1;
	Im_FPT_Int_Handler( ch );
#endif

	ercd = Im_FPT_WaitEnd( ch, &status, 100 );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "wait end error = %d\n", ercd ));
	}

	ercd = Im_FPT_Stop( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_FPT_FUNC_NAME "Stop error = 0x%x\n", ercd ));
	}

	ercd = Im_FPT_Close( ch );
	if( ercd != D_DDIM_OK ){
		Ddim_Print(( "close error = %d\n", ercd ));
	}

	ct_im_fpt_print_mode2_reg( ch );

	return D_DDIM_OK;
}

static INT32 ct_im_fpt_run_4( UCHAR ch, const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ) {
		// Test-1-4-1
		case 1:
			return CT_Im_Fpt_4_1( ch );

		// Test-1-4-2
		case 2:
			return CT_Im_Fpt_4_2( ch );

		// Test-1-4-3
		case 3:
			return CT_Im_Fpt_4_3( ch );

		// Test-1-4-4
		case 4:
			return CT_Im_Fpt_4_4( ch );

		// Test-1-4-5
		case 5:
			return CT_Im_Fpt_4_5( ch );

		// Test-1-4-6
		case 6:
			return CT_Im_Fpt_4_6( ch );

		// Test-1-4-7
		case 7:
			return CT_Im_Fpt_4_7( ch );

		// Test-1-4-8
		case 8:
			return CT_Im_Fpt_4_8( ch );

		// Test-1-4-9
		case 9:
			return CT_Im_Fpt_4_9( ch );

		// Test-1-4-10
		case 10:
			return CT_Im_Fpt_4_10( ch );

		// Test-1-4-11
		case 11:
			return CT_Im_Fpt_4_11( ch );

		// Test-1-4-12
		case 12:
			return CT_Im_Fpt_4_12( ch );

		// Test-1-4-13
		case 13:
			return CT_Im_Fpt_4_13( ch );

		// Test-1-4-14
		case 14:
			return CT_Im_Fpt_4_14( ch );

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;

}


VOID CT_Im_FPT_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
#ifdef CO_MSG_PRINT_ON
	INT32 result = D_DDIM_INPUT_PARAM_ERROR;
#endif
	static BOOL is_init = 0;
	UCHAR loop_cnt;
	INT32 ercd;

	Ddim_Print(( "CT_Im_FPT_Run( %u, %u, %u ) Begin\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	if( is_init == 0 ) {
#ifdef CO_DEBUG_ON_PC
		memset( (VOID*)&IO_FPT[0], '\0', sizeof(IO_FPT) );
		memset( (VOID*)&IO_FPT_AHB[0], '\0', sizeof(IO_FPT_AHB) );
		IO_FPT[0].FPTCTL1.word = 2;
		IO_FPT[1].FPTCTL1.word = 2;
#endif // CO_DEBUG_ON_PC
		if( !((ct_idx_1st == 1) && (ct_idx_2nd == 1)) ) {
			Ddim_Print(( "Im_FPT_Init() Begin\n" ));
			ercd = Im_FPT_Init( 0 );
			if( ercd != D_IM_FPT_RETVAL_OK ){
				Ddim_Print(( "Im_FPT_Init(0) error\n" ));
			}
			ercd = Im_FPT_Init( 1 );
			if( ercd != D_IM_FPT_RETVAL_OK ){
				Ddim_Print(( "Im_FPT_Init(1) error\n" ));
			}
			Ddim_Print(( "Im_FPT_Init() End\n" ));
			is_init = 1;
		}
	}

	if( ct_idx_1st != 4 ){
		for( loop_cnt = 0; loop_cnt <= 1; loop_cnt++ ){
			switch( ct_idx_1st ) {
				case 1:
					D_IM_FPT_RESULT( ct_im_fpt_run_1( loop_cnt, ct_idx_2nd ) );
					break;

				case 2:
					D_IM_FPT_RESULT( ct_im_fpt_run_2( loop_cnt, ct_idx_2nd ) );
					break;

				case 3:
					D_IM_FPT_RESULT( ct_im_fpt_run_3( loop_cnt, ct_idx_2nd ) );
					break;

				default:
					Ddim_Print(( "Error: Unknown command.\n" ));
					break;
			}

#ifdef CO_MSG_PRINT_ON
			Ddim_Print(( "result = %d\n", result ));
#endif
			Ddim_Print(( "CT_Im_FPT_Run( %u, %u, %u) End\n", ct_idx_1st, ct_idx_2nd, loop_cnt ));
		}
	}
	else{
		D_IM_FPT_RESULT( ct_im_fpt_run_4( ct_idx_3rd, ct_idx_2nd ) );

#ifdef CO_MSG_PRINT_ON
		Ddim_Print(( "result = %d\n", result ));
#endif
		Ddim_Print(( "CT_Im_FPT_Run( %u, %u, %u) End\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));
	}

	return;
}


// for Debug console interface
VOID CT_Im_FPT_Main( INT32 argc, CHAR** argv )
{
	if( argc < 3 ) {
		Ddim_Print(( "CT_Im_FPT_Main: parameter error.\n" ));
		return;
	}

	CT_Im_FPT_Run( (UINT32)atoi((const char*)argv[1]),
				   (UINT32)atoi((const char*)argv[2]),
				   (UINT32)atoi((const char*)argv[3]) );
}

