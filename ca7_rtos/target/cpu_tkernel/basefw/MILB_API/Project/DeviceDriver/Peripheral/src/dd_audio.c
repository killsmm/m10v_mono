/**
 * @file		dd_audio.c
 * @brief		AudioI/F Macro driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include <string.h>
#include "audio_if.h"
#include "dd_audio.h"
#include "dd_arm.h"
#include "dd_top.h"
#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#define D_DD_AUDIO_STOP_WAIT_LOOP	(5)

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

static volatile T_DD_AUDIO_CALLBACK gDd_Audio_OverFlow_Callback_Func[D_DD_AUDIO_IF_CH_NUM_MAX]	= {NULL, NULL, NULL, NULL, NULL, NULL};
static volatile T_DD_AUDIO_CALLBACK gDd_Audio_UnderFlow_Callback_Func[D_DD_AUDIO_IF_CH_NUM_MAX]	= {NULL, NULL, NULL, NULL, NULL, NULL};
static volatile T_DD_AUDIO_CALLBACK gDd_Audio_Input_Callback_Func[D_DD_AUDIO_IF_CH_NUM_MAX]		= {NULL, NULL, NULL, NULL, NULL, NULL};
static volatile T_DD_AUDIO_CALLBACK gDd_Audio_Output_Callback_Func[D_DD_AUDIO_IF_CH_NUM_MAX]	= {NULL, NULL, NULL, NULL, NULL, NULL};



// for ch0
static volatile T_DD_AUDIO_CTRL_COMMON	gDd_Audio_Ctrl_Cmmn_Ch0 =
{
											E_DD_AUDIO_FIFO_USAGE_STAGES_8,			// fifo_usage
											E_DD_AUDIO_CHANNEL_MONO					// channel
};

static volatile T_DD_AUDIO_CTRL_IN		gDd_Audio_Ctrl_In_Ch0 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
};
 
static volatile T_DD_AUDIO_CTRL_OUT		gDd_Audio_Ctrl_Out_Ch0 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											D_DD_AUDIO_DISABLE,						// lr_copy
											D_DD_AUDIO_DISABLE,						// mix_play
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
 };
static volatile T_DD_AUDIO_I2S_CMMN		gDd_Audio_I2S_Cmmn_Ch0 =
{
											E_DD_AUDIO_MASTER_CLOCK_8_192,			// aumclki
											E_DD_AUDIO_AUMCLKO_DIV_2,				// div_aumclko
											E_DD_AUDIO_AUCLK_DIV_16,				// div_auclk
											E_DD_AUDIO_AULR_DIV_32,					// div_lrclk
											D_DD_AUDIO_DISABLE,						// clk_div_enable
											E_DD_AUDIO_CLK_SLAVE					// master_slave
};

static volatile T_DD_AUDIO_I2S_IN		gDd_Audio_I2S_In_Ch0 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_in
											E_DD_AUDIO_BIT_8BIT						// bit_in
};

static volatile T_DD_AUDIO_I2S_OUT		gDd_Audio_I2S_Out_Ch0 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_out
											E_DD_AUDIO_BIT_8BIT,					// bit_out
											E_DD_AUDIO_FS_32FS						// fs
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_In_Ch0 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_Out_Ch0 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

// for ch1
static volatile T_DD_AUDIO_CTRL_COMMON	gDd_Audio_Ctrl_Cmmn_Ch1 =
{
											E_DD_AUDIO_FIFO_USAGE_STAGES_8,			// fifo_usage
											E_DD_AUDIO_CHANNEL_MONO					// channel
};

static volatile T_DD_AUDIO_CTRL_OUT		gDd_Audio_Ctrl_Out_Ch1 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											D_DD_AUDIO_DISABLE,						// lr_copy
											D_DD_AUDIO_DISABLE,						// mix_play
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
};

static volatile T_DD_AUDIO_CTRL_IN		gDd_Audio_Ctrl_In_Ch1 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
};

#if 0
static volatile T_DD_AUDIO_I2S_CMMN		gDd_Audio_I2S_Cmmn_Ch1 =
{
											E_DD_AUDIO_MASTER_CLOCK_8_192,			// aumclki
											E_DD_AUDIO_AUMCLKO_DIV_2,				// div_aumclko
											E_DD_AUDIO_AUCLK_DIV_16,				// div_auclk
											E_DD_AUDIO_AULR_DIV_32,					// div_lrclk
											D_DD_AUDIO_DISABLE,						// clk_div_enable
											E_DD_AUDIO_CLK_SLAVE					// master_slave
};
#endif

static volatile T_DD_AUDIO_I2S_IN		gDd_Audio_I2S_In_Ch1 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_in
											E_DD_AUDIO_BIT_8BIT						// bit_in
};

static volatile T_DD_AUDIO_I2S_OUT		gDd_Audio_I2S_Out_Ch1 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_out
											E_DD_AUDIO_BIT_8BIT,					// bit_out
											E_DD_AUDIO_FS_32FS						// fs
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_In_Ch1 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_Out_Ch1 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

// for ch2
static volatile T_DD_AUDIO_CTRL_COMMON	gDd_Audio_Ctrl_Cmmn_Ch2 =
{
											E_DD_AUDIO_FIFO_USAGE_STAGES_8,			// fifo_usage
											E_DD_AUDIO_CHANNEL_MONO					// channel
};

static volatile T_DD_AUDIO_CTRL_OUT		gDd_Audio_Ctrl_Out_Ch2 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											D_DD_AUDIO_DISABLE,						// lr_copy
											D_DD_AUDIO_DISABLE,						// mix_play
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
};

static volatile T_DD_AUDIO_CTRL_IN		gDd_Audio_Ctrl_In_Ch2 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
};

static volatile T_DD_AUDIO_I2S_CMMN		gDd_Audio_I2S_Cmmn_Ch2 =
{
											E_DD_AUDIO_MASTER_CLOCK_8_192,			// aumclki
											E_DD_AUDIO_AUMCLKO_DIV_2,				// div_aumclko
											E_DD_AUDIO_AUCLK_DIV_16,				// div_auclk
											E_DD_AUDIO_AULR_DIV_32,					// div_lrclk
											D_DD_AUDIO_DISABLE,						// clk_div_enable
											E_DD_AUDIO_CLK_SLAVE					// master_slave
};

static volatile T_DD_AUDIO_I2S_IN		gDd_Audio_I2S_In_Ch2 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_in
											E_DD_AUDIO_BIT_8BIT						// bit_in
};

static volatile T_DD_AUDIO_I2S_OUT		gDd_Audio_I2S_Out_Ch2 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_out
											E_DD_AUDIO_BIT_8BIT,					// bit_out
											E_DD_AUDIO_FS_32FS						// fs
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_In_Ch2 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_Out_Ch2 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

// for ch3
static volatile T_DD_AUDIO_CTRL_COMMON	gDd_Audio_Ctrl_Cmmn_Ch3 =
{
											E_DD_AUDIO_FIFO_USAGE_STAGES_8,			// fifo_usage
											E_DD_AUDIO_CHANNEL_MONO					// channel
};

static volatile T_DD_AUDIO_CTRL_OUT		gDd_Audio_Ctrl_Out_Ch3 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											D_DD_AUDIO_DISABLE,						// lr_copy
											D_DD_AUDIO_DISABLE,						// mix_play
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
};

static volatile T_DD_AUDIO_CTRL_IN		gDd_Audio_Ctrl_In_Ch3 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
};

static volatile T_DD_AUDIO_I2S_CMMN		gDd_Audio_I2S_Cmmn_Ch3 =
{
											E_DD_AUDIO_MASTER_CLOCK_8_192,			// aumclki
											E_DD_AUDIO_AUMCLKO_DIV_2,				// div_aumclko
											E_DD_AUDIO_AUCLK_DIV_16,				// div_auclk
											E_DD_AUDIO_AULR_DIV_32,					// div_lrclk
											D_DD_AUDIO_DISABLE,						// clk_div_enable
											E_DD_AUDIO_CLK_SLAVE					// master_slave
};

static volatile T_DD_AUDIO_I2S_IN		gDd_Audio_I2S_In_Ch3 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_in
											E_DD_AUDIO_BIT_8BIT						// bit_in
};

static volatile T_DD_AUDIO_I2S_OUT		gDd_Audio_I2S_Out_Ch3 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_out
											E_DD_AUDIO_BIT_8BIT,					// bit_out
											E_DD_AUDIO_FS_32FS						// fs
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_In_Ch3 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_Out_Ch3 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

// for ch4
static volatile T_DD_AUDIO_CTRL_COMMON	gDd_Audio_Ctrl_Cmmn_Ch4 =
{
											E_DD_AUDIO_FIFO_USAGE_STAGES_8,			// fifo_usage
											E_DD_AUDIO_CHANNEL_MONO					// channel
};

static volatile T_DD_AUDIO_CTRL_OUT		gDd_Audio_Ctrl_Out_Ch4 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											D_DD_AUDIO_DISABLE,						// lr_copy
											D_DD_AUDIO_DISABLE,						// mix_play
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
};

static volatile T_DD_AUDIO_CTRL_IN		gDd_Audio_Ctrl_In_Ch4 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
};

static volatile T_DD_AUDIO_I2S_CMMN		gDd_Audio_I2S_Cmmn_Ch4 =
{
											E_DD_AUDIO_MASTER_CLOCK_8_192,			// aumclki
											E_DD_AUDIO_AUMCLKO_DIV_2,				// div_aumclko
											E_DD_AUDIO_AUCLK_DIV_16,				// div_auclk
											E_DD_AUDIO_AULR_DIV_32,					// div_lrclk
											D_DD_AUDIO_DISABLE,						// clk_div_enable
											E_DD_AUDIO_CLK_SLAVE					// master_slave
};

static volatile T_DD_AUDIO_I2S_IN		gDd_Audio_I2S_In_Ch4 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_in
											E_DD_AUDIO_BIT_8BIT						// bit_in
};

static volatile T_DD_AUDIO_I2S_OUT		gDd_Audio_I2S_Out_Ch4 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_out
											E_DD_AUDIO_BIT_8BIT,					// bit_out
											E_DD_AUDIO_FS_32FS						// fs
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_In_Ch4 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_Out_Ch4 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

// for ch5
static volatile T_DD_AUDIO_CTRL_COMMON	gDd_Audio_Ctrl_Cmmn_Ch5 =
{
											E_DD_AUDIO_FIFO_USAGE_STAGES_8,			// fifo_usage
											E_DD_AUDIO_CHANNEL_MONO					// channel
};

static volatile T_DD_AUDIO_CTRL_OUT		gDd_Audio_Ctrl_Out_Ch5 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											D_DD_AUDIO_DISABLE,						// lr_copy
											D_DD_AUDIO_DISABLE,						// mix_play
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
};

static volatile T_DD_AUDIO_CTRL_IN		gDd_Audio_Ctrl_In_Ch5 =
{
											E_DD_AUDIO_DATA_REG_FRMT_R_JUST,		// format
											E_DD_AUDIO_FIFO_STAGES_1,				// fifo_stages
											E_DD_AUDIO_AHB_FRMT_R_JUST,				// ahb_format
											E_DD_AUDIO_BIT_SHIFT_0					// bit_shift
};

static volatile T_DD_AUDIO_I2S_CMMN		gDd_Audio_I2S_Cmmn_Ch5 =
{
											E_DD_AUDIO_MASTER_CLOCK_8_192,			// aumclki
											E_DD_AUDIO_AUMCLKO_DIV_2,				// div_aumclko
											E_DD_AUDIO_AUCLK_DIV_16,				// div_auclk
											E_DD_AUDIO_AULR_DIV_32,					// div_lrclk
											D_DD_AUDIO_DISABLE,						// clk_div_enable
											E_DD_AUDIO_CLK_SLAVE					// master_slave
};

static volatile T_DD_AUDIO_I2S_IN		gDd_Audio_I2S_In_Ch5 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_in
											E_DD_AUDIO_BIT_8BIT						// bit_in
};

static volatile T_DD_AUDIO_I2S_OUT		gDd_Audio_I2S_Out_Ch5 =
{
											E_DD_AUDIO_MODE_L_JUST,					// mode_out
											E_DD_AUDIO_BIT_8BIT,					// bit_out
											E_DD_AUDIO_FS_32FS						// fs
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_In_Ch5 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

static volatile T_DD_AUDIO_DMA			gDd_Audio_Dma_Out_Ch5 =
{
											D_DD_AUDIO_DISABLE,						// dma_2ch
											0										// dma_trnsf_cnt
};

static volatile T_DD_AUDIO_CTRL_COMMON		*gDd_Audio_Ctrl_Cmmn[D_DD_AUDIO_IF_CH_NUM_MAX] =
{
	&gDd_Audio_Ctrl_Cmmn_Ch0,
	&gDd_Audio_Ctrl_Cmmn_Ch1,
	&gDd_Audio_Ctrl_Cmmn_Ch2,
	&gDd_Audio_Ctrl_Cmmn_Ch3,
	&gDd_Audio_Ctrl_Cmmn_Ch4,
	&gDd_Audio_Ctrl_Cmmn_Ch5
};

static volatile T_DD_AUDIO_CTRL_IN		*gDd_Audio_Ctrl_In[D_DD_AUDIO_IF_CH_NUM_MAX] =
{
	&gDd_Audio_Ctrl_In_Ch0,
	&gDd_Audio_Ctrl_In_Ch1,
	&gDd_Audio_Ctrl_In_Ch2,
	&gDd_Audio_Ctrl_In_Ch3,
	&gDd_Audio_Ctrl_In_Ch4,
	&gDd_Audio_Ctrl_In_Ch5
};

static volatile T_DD_AUDIO_CTRL_OUT		*gDd_Audio_Ctrl_Out[D_DD_AUDIO_IF_CH_NUM_MAX] =
{
	&gDd_Audio_Ctrl_Out_Ch0,
	&gDd_Audio_Ctrl_Out_Ch1,
	&gDd_Audio_Ctrl_Out_Ch2,
	&gDd_Audio_Ctrl_Out_Ch3,
	&gDd_Audio_Ctrl_Out_Ch4,
	&gDd_Audio_Ctrl_Out_Ch5
};


static volatile T_DD_AUDIO_I2S_CMMN		*gDd_Audio_I2S_Cmmn[D_DD_AUDIO_IF_CH_NUM_MAX] =
{
	&gDd_Audio_I2S_Cmmn_Ch0,
	&gDd_Audio_I2S_Cmmn_Ch0,
	&gDd_Audio_I2S_Cmmn_Ch2,
	&gDd_Audio_I2S_Cmmn_Ch3,
	&gDd_Audio_I2S_Cmmn_Ch4,
	&gDd_Audio_I2S_Cmmn_Ch5
};

static volatile T_DD_AUDIO_I2S_IN		*gDd_Audio_I2S_In[D_DD_AUDIO_IF_CH_NUM_MAX] =
{
	&gDd_Audio_I2S_In_Ch0,
	&gDd_Audio_I2S_In_Ch1,
	&gDd_Audio_I2S_In_Ch2,
	&gDd_Audio_I2S_In_Ch3,
	&gDd_Audio_I2S_In_Ch4,
	&gDd_Audio_I2S_In_Ch5
};

static volatile T_DD_AUDIO_I2S_OUT		*gDd_Audio_I2S_Out[D_DD_AUDIO_IF_CH_NUM_MAX] =
{
	&gDd_Audio_I2S_Out_Ch0,
	&gDd_Audio_I2S_Out_Ch1,
	&gDd_Audio_I2S_Out_Ch2,
	&gDd_Audio_I2S_Out_Ch3,
	&gDd_Audio_I2S_Out_Ch4,
	&gDd_Audio_I2S_Out_Ch5
};

static volatile T_DD_AUDIO_DMA		*gDd_Audio_Dma_In[D_DD_AUDIO_IF_CH_NUM_MAX] =
{
	&gDd_Audio_Dma_In_Ch0,
	&gDd_Audio_Dma_In_Ch1,
	&gDd_Audio_Dma_In_Ch2,
	&gDd_Audio_Dma_In_Ch3,
	&gDd_Audio_Dma_In_Ch4,
	&gDd_Audio_Dma_In_Ch5
};

static volatile T_DD_AUDIO_DMA		*gDd_Audio_Dma_Out[D_DD_AUDIO_IF_CH_NUM_MAX] =
{
	&gDd_Audio_Dma_Out_Ch0,
	&gDd_Audio_Dma_Out_Ch1,
	&gDd_Audio_Dma_Out_Ch2,
	&gDd_Audio_Dma_Out_Ch3,
	&gDd_Audio_Dma_Out_Ch4,
	&gDd_Audio_Dma_Out_Ch5
};


static const UINT8	gDd_Audio_AUOF_Tbl_8_L[4] =			// 8bit Left-Justified
{
	0x00,				// 32FS
	0x10,				// 48FS
	0x01,				// 64FS
	0x20				// 128FS
};

static const UINT8	gDd_Audio_AUOF_Tbl_8_R[4] =			// 8bit Right-Justified
{
	0x02,				// 32FS
	0x12,				// 48FS
	0x03,				// 64FS
	0x22				// 128FS
};

static const UINT8	gDd_Audio_AUOF_Tbl_8_I2S[4] =			// 8bit I2S
{
	0x40,				// 32FS
	0x50,				// 48FS
	0x41,				// 64FS
	0x60				// 128FS
};

static const UINT8	gDd_Audio_AUOF_Tbl_16_L[4] =			// 16bit Left-Justified
{
	0x04,				// 32FS
	0x14,				// 48FS
	0x05,				// 64FS
	0x24				// 128FS
};

static const UINT8	gDd_Audio_AUOF_Tbl_16_R[4] =			// 16bit Right-Justified
{
	0x06,				// 32FS
	0x17,				// 48FS
	0x07,				// 64FS
	0x27				// 128FS
};

static const UINT8	gDd_Audio_AUOF_Tbl_16_I2S[4] =			// 16bit I2S
{
	0x44,				// 32FS
	0x54,				// 48FS
	0x45,				// 64FS
	0x64				// 128FS
};

static const UINT8	gDd_Audio_AUOF_Tbl_24_L[4] =			// 24bit Left-Justified
{
	0xFF,				// 32FS
	0x18,				// 48FS
	0x09,				// 64FS
	0x28				// 128FS
};

static const UINT8	gDd_Audio_AUOF_Tbl_24_R[4] =			// 24bit Right-Justified
{
	0xFF,				// 32FS
	0x1A,				// 48FS
	0x0B,				// 64FS
	0x2B				// 128FS
};

static const UINT8	gDd_Audio_AUOF_Tbl_24_I2S[4] =			// 24bit I2S
{
	0xFF,				// 32FS
	0x58,				// 48FS
	0x49,				// 64FS
	0x68				// 128FS
};

static const UINT8	*gDd_Audio_AUOF_Tbl[3][3] =			// index [bits][mode]
{
	//	L							R							I2S
	{	gDd_Audio_AUOF_Tbl_8_L,		gDd_Audio_AUOF_Tbl_8_R,		gDd_Audio_AUOF_Tbl_8_I2S	},	// 8bit
	{	gDd_Audio_AUOF_Tbl_16_L,	gDd_Audio_AUOF_Tbl_16_R,	gDd_Audio_AUOF_Tbl_16_I2S	},	// 16bit
	{	gDd_Audio_AUOF_Tbl_24_L,	gDd_Audio_AUOF_Tbl_24_R,	gDd_Audio_AUOF_Tbl_24_I2S	}	// 24bit
};

static const UINT8	gDd_Audio_AUIF_Tbl[3][3] =				// index [bits][mode]
{
	//  L       R       I2S
	{	0x00,	0x01,	0x08	},		// 8bit
	{	0x02,	0x03,	0x0A	},		// 16bit
	{	0x04,	0x05,	0x0C	}		// 24bit
};

static volatile E_DD_AUDIO_MASTER_CLOCK gDd_Audio_MasterClock = (E_DD_AUDIO_MASTER_CLOCK)0xFF;

// Spin Lock
static ULONG gDd_Audio_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64))) = 0;


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define dd_audio_wait_usec( usec )	Dd_ARM_Wait_ns( (usec * 1000) )

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/**** AURES ****/
static INT32 dd_audio_set_sw_reset( UINT8 ch );

/**** AUIE ****/
static INT32 dd_audio_set_enable_audio_in( UINT8 ch, UINT8 enable );

/**** AUOE ****/
static INT32 dd_audio_set_enable_audio_out( UINT8 ch, UINT8 enable );

/**** AULBE ****/
static INT32 dd_audio_set_enable_loopback( UINT8 ch, UINT8 enable );

/**** AUCR ****/
static INT32 dd_audio_set_enable_in_data24bit( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_enable_in_data24bit( UINT8 ch );
static INT32 dd_audio_set_enable_out_data24bit( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_in_full( UINT8 ch );
static BOOL dd_audio_get_out_empty( UINT8 ch );
static INT32 dd_audio_set_enable_in_full_intr( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_enable_in_full_intr( UINT8 ch );
static INT32 dd_audio_set_enable_out_empty_intr( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_enable_out_empty_intr( UINT8 ch );
static BOOL dd_audio_get_audio_in_enable_flag( UINT8 ch );
static BOOL dd_audio_get_audio_out_enable_flag( UINT8 ch );
static INT32 dd_audio_set_enable_fifo_full_dma_in( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_enable_fifo_full_dma_in( UINT8 ch );
static INT32 dd_audio_set_enable_fifo_empty_dma_out( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_enable_fifo_empty_dma_out( UINT8 ch );
static INT32 dd_audio_set_input_data_format( UINT8 ch, UINT8 format );
static INT32 dd_audio_set_input_register_format( UINT8 ch, UINT8 format );
static INT32 dd_audio_set_output_data_format( UINT8 ch, UINT8 format );
static INT32 dd_audio_set_output_register_format( UINT8 ch, UINT8 format );

/**** AUMD ****/
static INT32 dd_audio_set_input_byte_swap( UINT8 ch, UINT8 enable );
static INT32 dd_audio_set_input_hw_swap( UINT8 ch, UINT8 enable );
static INT32 dd_audio_set_output_byte_swap( UINT8 ch, UINT8 enable );
static INT32 dd_audio_set_output_hw_swap( UINT8 ch, UINT8 enable );
static INT32 dd_audio_set_in_fifo( UINT8 ch, UINT8 stage );
static INT32 dd_audio_set_out_fifo( UINT8 ch, UINT8 stage );
static INT32 dd_audio_set_stereo( UINT8 ch, UINT8 stereo );
static BOOL dd_audio_get_stereo( UINT8 ch );
static BOOL dd_audio_get_loopback_flag( UINT8 ch );

/**** AUST ****/
static BOOL dd_audio_get_in_overflow_flag( UINT8 ch );
static INT32 dd_audio_clear_in_overflow_flag( UINT8 ch );
static BOOL dd_audio_get_out_underflow_flag( UINT8 ch );
static INT32 dd_audio_clear_out_underflow_flag( UINT8 ch );
static INT32 dd_audio_set_enable_in_overflow_intr( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_enable_in_overflow_intr( UINT8 ch );
static INT32 dd_audio_set_enable_out_underflow_intr( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_enable_out_underflow_intr( UINT8 ch );
static BOOL dd_audio_get_in_usage_flag( UINT8 ch );
static BOOL dd_audio_get_out_usage_flag( UINT8 ch );
static INT32 dd_audio_set_enable_in_usage_intr( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_enable_in_usage_intr( UINT8 ch );
static INT32 dd_audio_set_enable_out_usage_intr( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_enable_out_usage_intr( UINT8 ch );
static INT32 dd_audio_set_input_dma_request_enable( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_input_dma_request_enable( UINT8 ch );
static INT32 dd_audio_set_output_dma_request_enable( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_output_dma_request_enable( UINT8 ch );
static INT32 dd_audio_set_register_usage( UINT8 ch, UINT8 control );

/**** AUCC ****/
static INT32 dd_audio_set_clock_div( UINT8 ch, UINT8 mclock, UINT8 auclock, UINT8 lrclock );
static INT32 dd_audio_set_enable_clock_division( UINT8 ch, UINT8 enable );
static BOOL dd_audio_get_enable_clock_division( UINT8 ch );
static INT32 dd_audio_set_enable_output_clock( UINT8 ch, UINT8 enable );

/**** AUDP ****/
static INT32 dd_audio_set_mix_play( UINT8 ch, UINT8 control );
static INT32 dd_audio_set_lr_data_copy( UINT8 ch, UINT8 copy );
static INT32 dd_audio_set_in_ahb_format( UINT8 ch, UINT8 format );
static INT32 dd_audio_set_out_ahb_format( UINT8 ch, UINT8 format );
static INT32 dd_audio_set_out_data_shift( UINT8 ch, UINT8 shift );
static INT32 dd_audio_set_in_data_shift( UINT8 ch, UINT8 shift );

/**** AUDMA2CTL ****/
static INT32 dd_audio_set_enable_in_dma2ch( UINT8 ch, UINT8 enable );
static INT32 dd_audio_set_enable_out_dma2ch( UINT8 ch, UINT8 enable );

/**** AUDMISAMPLE ****/
static INT32 dd_audio_set_in_dma_sample( UINT8 ch, UINT16 sample );

/**** AUDMOSAMPLE ****/
static INT32 dd_audio_set_out_dma_sample( UINT8 ch, UINT16 sample );

/**** ChipTop ****/
static VOID dd_audio_set_clkstop_auxck( UINT8 ch, UINT8 val );
static VOID dd_audio_set_clkstop_auxck_all( UINT8 val );
static VOID dd_audio_set_clksel_auxsel( UINT8 ch, UINT8 val );
static VOID dd_audio_select_aumclk_in( UINT8 ch, UINT8 val );
static INT32 dd_audio_set_masterclock( E_DD_AUDIO_MASTER_CLOCK mclk );


/**** Local function ****/
static INT32 dd_audio_disable_input_intr( UINT8 ch );
static INT32 dd_audio_disable_output_intr( UINT8 ch );
static VOID dd_audio_wait_empty_input_fifo( UINT8 ch );
static VOID dd_audio_wait_empty_output_fifo( UINT8 ch );
static VOID dd_audio_wait_store_output_fifo( UINT8 ch );


//---------------------- driver  section -------------------------------

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

/**** AURES ****/

/**
 * @brief  Reset Audio Interface.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_sw_reset( UINT8 ch )
{
	volatile union io_audio_aures aures;
	
	aures.word = IO_AUDIO.AUDIOIF_CTRL.AURES.word;
	
	switch (ch){
		case D_DD_AUDIO_IF_CH0 :
			aures.bit.AUIF0_RST = 1;
			break;
		case D_DD_AUDIO_IF_CH1 :
			aures.bit.AUIF1_RST = 1;
			break;
		case D_DD_AUDIO_IF_CH2 :
			aures.bit.AUIF2_RST = 1;
			break;
		case D_DD_AUDIO_IF_CH3 :
			aures.bit.AUIF3_RST = 1;
			break;
		case D_DD_AUDIO_IF_CH4 :
			aures.bit.AUIF4_RST = 1;
			break;
		case D_DD_AUDIO_IF_CH5 :
			aures.bit.AUIF5_RST = 1;
			break;
		default :
#ifdef CO_PARAM_CHECK
			Ddim_Assertion(("[DD_AUDIO]]AUDIO Reset:input channel error : %d\n", ch));
#endif	// CO_PARAM_CHECK
			return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	// bit6 = 0
	aures.word &= 0x3F;
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	IO_AUDIO.AUDIOIF_CTRL.AURES.word = aures.word;
	IO_AUDIO.AUDIOIF_CTRL.AURES.word = 0;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return D_DDIM_OK;
}

/**** AUIE ****/

/**
 * @brief  Set register AUIOE(AUIE).
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_audio_in( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_auioe auioe;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set AUIE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (ch == D_DD_AUDIO_IF_CH0_CH1){
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH0) != FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUIE:already start input(0)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH1) != FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUIE:already start input(1)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
		else {
			if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH0) == FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUIE:already stop input(0)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH1) == FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUIE:already stop input(1)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
	}
	else if (ch == D_DD_AUDIO_IF_CH4_CH5){
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH4) != FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUIE:already start input(4)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH5) != FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUIE:already start input(5)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
		else {
			if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH4) == FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUIE:already stop input(4)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH5) == FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUIE:already stop input(5)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
	}
	else {
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUIE:already start input\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
		else {
			if (dd_audio_get_audio_in_enable_flag(ch) == FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUIE:already stop input\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
	}
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	auioe.word = IO_AUDIO.AUDIOIF_CTRL.AUIOE.word;
	
	switch (ch){
		case D_DD_AUDIO_IF_CH0 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUIE0 = 1;
			}
			else {
				auioe.bit.AUIE0 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH1 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUIE1 = 1;
			}
			else {
				auioe.bit.AUIE1 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH2 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUIE2 = 1;
			}
			else {
				auioe.bit.AUIE2 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH3 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUIE3 = 1;
			}
			else {
				auioe.bit.AUIE3 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH4 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUIE4 = 1;
			}
			else {
				auioe.bit.AUIE4 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH5 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUIE5 = 1;
			}
			else {
				auioe.bit.AUIE5 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH0_CH1 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUIE0 = 1;
				auioe.bit.AUIE1 = 1;
			}
			else {
				auioe.bit.AUIE0 = 2;
				auioe.bit.AUIE1 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH4_CH5 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUIE4 = 1;
				auioe.bit.AUIE5 = 1;
			}
			else {
				auioe.bit.AUIE4 = 2;
				auioe.bit.AUIE5 = 2;
			}
			break;
		default :
			Ddim_Print(("[DD_AUDIO]Set AUIE:input channel error : %d\n", ch));
			ret_val = D_DD_AUDIO_INPUT_PARAM_ERROR;
			break;
	}
	
	if (ret_val == D_DDIM_OK){
		IO_AUDIO.AUDIOIF_CTRL.AUIOE.word = auioe.word;
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**** AUOE ****/

/**
 * @brief  Set register AUIOE(AUOE).
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_audio_out( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_auioe auioe;
	INT32 ret_val = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set AUOE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (ch == D_DD_AUDIO_IF_CH0_CH1){
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_audio_out_enable_flag(D_DD_AUDIO_IF_CH0) != FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUOE:already start output(0)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_audio_out_enable_flag(D_DD_AUDIO_IF_CH1) != FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUOE:already start output(1)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_loopback_flag(D_DD_AUDIO_IF_CH0) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set AUOE:LBF(0) = 1\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_loopback_flag(D_DD_AUDIO_IF_CH1) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set AUOE:LBF(1) = 1\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
		else {
			if (dd_audio_get_audio_out_enable_flag(D_DD_AUDIO_IF_CH0) == FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUOE:already stop output(0)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_audio_out_enable_flag(D_DD_AUDIO_IF_CH1) == FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUOE:already stop output(1)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
	}
	else {
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUOE:already start output\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_loopback_flag(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set AUOE:LBF = 1\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
		else {
			if (dd_audio_get_audio_out_enable_flag(ch) == FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AUOE:already stop output\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
	}
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	auioe.word = IO_AUDIO.AUDIOIF_CTRL.AUIOE.word;
	
	switch (ch){
		case D_DD_AUDIO_IF_CH0 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUOE0 = 1;
			}
			else {
				auioe.bit.AUOE0 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH1 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUOE1 = 1;
			}
			else {
				auioe.bit.AUOE1 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH2 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUOE2 = 1;
			}
			else {
				auioe.bit.AUOE2 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH3 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUOE3 = 1;
			}
			else {
				auioe.bit.AUOE3 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH4 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUOE4 = 1;
			}
			else {
				auioe.bit.AUOE4 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH5 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUOE5 = 1;
			}
			else {
				auioe.bit.AUOE5 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH0_CH1 :
			if (enable == D_DD_AUDIO_ENABLE){
				auioe.bit.AUOE0 = 1;
				auioe.bit.AUOE1 = 1;
			}
			else {
				auioe.bit.AUOE0 = 2;
				auioe.bit.AUOE1 = 2;
			}
			break;
		default :
			Ddim_Print(("[DD_AUDIO]Set AUOE:input channel error : %d\n", ch));
			ret_val = D_DD_AUDIO_INPUT_PARAM_ERROR;
			break;
	}
	
	if (ret_val == D_DDIM_OK){
		IO_AUDIO.AUDIOIF_CTRL.AUIOE.word = auioe.word;
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}


/**** AULBE ****/

/**
 * @brief  Set register LB.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_loopback( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aulbe aulbe;
	INT32 ret_val = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set AULBE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (ch == D_DD_AUDIO_IF_CH0_CH1){
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_loopback_flag(D_DD_AUDIO_IF_CH0) != FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AULBE:already start loopback(0)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_loopback_flag(D_DD_AUDIO_IF_CH1) != FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AULBE:already start loopback(1)\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH0) == FALSE){
				Ddim_Print(("[DD_AUDIO]Set AULBE: AUIEF(0) = 0\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH1) == FALSE){
				Ddim_Print(("[DD_AUDIO]Set AULBE: AUIEF(1) = 0\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_audio_out_enable_flag(D_DD_AUDIO_IF_CH0) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set AULBE:AUOEF(0) = 1\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_audio_out_enable_flag(D_DD_AUDIO_IF_CH1) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set AULBE:AUOEF(1) = 1\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
		else {
			if (dd_audio_get_loopback_flag(D_DD_AUDIO_IF_CH0) == FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AULBE:already stop loopback\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			
			if (dd_audio_get_loopback_flag(D_DD_AUDIO_IF_CH1) == FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AULBE:already stop loopback\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
	}
	else {
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_loopback_flag(ch) != FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AULBE:already start loopback\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			if (dd_audio_get_audio_in_enable_flag(ch) == FALSE){
				Ddim_Print(("[DD_AUDIO]Set AULBE Error: AUIEF = 0\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
			if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set AULBE:AUOEF = 1\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
		else {
			if (dd_audio_get_loopback_flag(ch) == FALSE){
				Ddim_Assertion(("[DD_AUDIO]Set AULBE:already stop loopback\n"));
				return D_DD_AUDIO_SYSTEM_ERROR;
			}
		}
	}
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aulbe.word = IO_AUDIO.AUDIOIF_CTRL.AULBE.word;
	
	switch (ch){
		case D_DD_AUDIO_IF_CH0 :
			if (enable == D_DD_AUDIO_ENABLE){
				aulbe.bit.AULBE0 = 1;
			}
			else {
				aulbe.bit.AULBE0 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH1 :
			if (enable == D_DD_AUDIO_ENABLE){
				aulbe.bit.AULBE1 = 1;
			}
			else {
				aulbe.bit.AULBE1 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH2 :
			if (enable == D_DD_AUDIO_ENABLE){
				aulbe.bit.AULBE2 = 1;
			}
			else {
				aulbe.bit.AULBE2 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH3 :
			if (enable == D_DD_AUDIO_ENABLE){
				aulbe.bit.AULBE3 = 1;
			}
			else {
				aulbe.bit.AULBE3 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH4 :
			if (enable == D_DD_AUDIO_ENABLE){
				aulbe.bit.AULBE4 = 1;
			}
			else {
				aulbe.bit.AULBE4 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH5 :
			if (enable == D_DD_AUDIO_ENABLE){
				aulbe.bit.AULBE5 = 1;
			}
			else {
				aulbe.bit.AULBE5 = 2;
			}
			break;
		case D_DD_AUDIO_IF_CH0_CH1 :
			if (enable == D_DD_AUDIO_ENABLE){
				aulbe.bit.AULBE0 = 1;
				aulbe.bit.AULBE1 = 1;
			}
			else {
				aulbe.bit.AULBE0 = 2;
				aulbe.bit.AULBE1 = 2;
			}
			break;
		default :
			Ddim_Print(("[DD_AUDIO]Set AULBE:input channel error : %d\n", ch));
			ret_val = D_DD_AUDIO_INPUT_PARAM_ERROR;
			break;
	}
	
	if (ret_val == D_DDIM_OK){
		IO_AUDIO.AUDIOIF_CTRL.AULBE.word = aulbe.word;
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**** AUCR ****/

/**
 * @brief  Set register AUIDSEL.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_in_data24bit( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aucr aucr;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUIDSEL:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set AUIDSEL:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aucr.word = IO_AUDIO.AUDIOIF[ch].AUCR.word;
	aucr.bit.AUIDSEL = enable;
	IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief  Get register AUIDSEL.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static BOOL dd_audio_get_enable_in_data24bit( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUIDSEL:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUCR.bit.AUIDSEL == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register AUODSEL.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_out_data24bit( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aucr aucr;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUODSEL:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set AUODSEL:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aucr.word = IO_AUDIO.AUDIOIF[ch].AUCR.word;
	aucr.bit.AUODSEL = enable;
	IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief  Get register INTI.
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
static BOOL dd_audio_get_in_full( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get INTI:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUCR.bit.INTI == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Get register INTO.
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
static BOOL dd_audio_get_out_empty( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get INTO:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUCR.bit.INTO == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register INTIE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_in_full_intr( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aucr aucr;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set INTIE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set INTIE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (enable == D_DD_AUDIO_ENABLE){
		if (dd_audio_get_enable_in_usage_intr(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set INTIE Error:EINTIE = 1\n"));
			return D_DD_AUDIO_SYSTEM_ERROR;
		}
	}
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aucr.word = IO_AUDIO.AUDIOIF[ch].AUCR.word;
	aucr.bit.INTIE = enable;
	IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief  Get register INTIE.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static BOOL dd_audio_get_enable_in_full_intr( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get INTIE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUCR.bit.INTIE == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register INTOE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_out_empty_intr( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aucr aucr;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set INTOE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set INTOE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (enable == D_DD_AUDIO_ENABLE){
		if (dd_audio_get_enable_out_usage_intr(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set INTOE Error:EINTOE = 1\n"));
			return D_DD_AUDIO_SYSTEM_ERROR;
		}
	}
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aucr.word = IO_AUDIO.AUDIOIF[ch].AUCR.word;
	aucr.bit.INTOE = enable;
	IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief  Get register INTOE.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static BOOL dd_audio_get_enable_out_empty_intr( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get INTOE:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUCR.bit.INTOE == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Get register AUIEF.
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
static BOOL dd_audio_get_audio_in_enable_flag( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get AUIEF:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUCR.bit.AUIEF == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Get register AUOE.
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
static BOOL dd_audio_get_audio_out_enable_flag( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get AUOEF:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUCR.bit.AUOEF == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register DMIE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_fifo_full_dma_in( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aucr aucr;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set DMIE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set DMIE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aucr.word = IO_AUDIO.AUDIOIF[ch].AUCR.word;
	
	if (aucr.bit.DMIE != enable){								/* pgr0872 */
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_input_dma_request_enable(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set DMIE Error:EDMIE = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else {
				aucr.bit.DMIE = enable;
				IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
			}
		}
		else {
			aucr.bit.DMIE = enable;
			IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Get register DMIE.
 * @param  UINT8 ch
 * @return BOOL TRUE/FALSE
 */
static BOOL dd_audio_get_enable_fifo_full_dma_in( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get DMIE:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUCR.bit.DMIE == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register DMOE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_fifo_empty_dma_out( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aucr aucr;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set DMOE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set DMOE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
 	
	aucr.word = IO_AUDIO.AUDIOIF[ch].AUCR.word;
	
	if (aucr.bit.DMOE != enable){								/* pgr0872 */
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_output_dma_request_enable(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set DMOE Error:EDMOE = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else {
				aucr.bit.DMOE = enable;
				IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
			}
		}
		else {
			aucr.bit.DMOE = enable;
			IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Get register DMOE.
 * @param  UINT8 ch
 * @return BOOL TRUE/FALSE
 */
static BOOL dd_audio_get_enable_fifo_empty_dma_out( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get DMOE:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUCR.bit.DMOE == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register AUIF.
 * @param  UINT8 ch
 * @param  UINT8 format
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_input_data_format( UINT8 ch, UINT8 format )
{
	volatile union io_audio_aucr aucr;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUIF:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (format > 0x0E){
		Ddim_Assertion(("[DD_AUDIO]Set AUIF:input format error : %d\n", format));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
 	
	aucr.word = IO_AUDIO.AUDIOIF[ch].AUCR.word;
	
	if (aucr.bit.AUIF != format){						/* pgr0872 */
		if (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set AUIF Error:AUIEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aucr.bit.AUIF = format;
			IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register AUIRF.
 * @param  UINT8 ch
 * @param  UINT8 format
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_input_register_format( UINT8 ch, UINT8 format )
{
	volatile union io_audio_aucr aucr;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUIRF:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (format > 1){
		Ddim_Assertion(("[DD_AUDIO]Set AUIRF:input format error : %d\n", format));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aucr.word = IO_AUDIO.AUDIOIF[ch].AUCR.word;
	
	if (aucr.bit.AUIRF != format){					/* pgr0872 */
		if (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set AUIRF Error: AUIEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aucr.bit.AUIRF = format;
			IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register AUOF.
 * @param  UINT8 ch
 * @param  UINT8 format
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_output_data_format( UINT8 ch, UINT8 format )
{
	volatile union io_audio_aucr aucr;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUOF:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (format > 0x6D){
		Ddim_Assertion(("[DD_AUDIO]Set AUOF:input format error : %d\n", format));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aucr.word = IO_AUDIO.AUDIOIF[ch].AUCR.word;
	
	if (aucr.bit.AUOF != format){								/* pgr0872 */
		if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set AUOF Error:AUOEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else if (dd_audio_get_loopback_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set AUOF Error:LBF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aucr.bit.AUOF = format;
			IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register AUORF.
 * @param  UINT8 ch
 * @param  UINT8 format
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_output_register_format( UINT8 ch, UINT8 format )
{
	volatile union io_audio_aucr aucr;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUORF:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (format > 1){
		Ddim_Assertion(("[DD_AUDIO]Set AUORF:input format error : %d\n", format));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aucr.word = IO_AUDIO.AUDIOIF[ch].AUCR.word;
	
	if (aucr.bit.AUORF != format){								/* pgr0872 */
		if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set AUORF Error:AUOEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else if (dd_audio_get_loopback_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set AUORF Error:LBF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aucr.bit.AUORF = format;
			IO_AUDIO.AUDIOIF[ch].AUCR.word = aucr.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}


/**** AUMD ****/

/**
 * @brief  Set register IBYTE_SWP.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_input_byte_swap( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aumd aumd;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set IBYTE_SWP:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set IBYTE_SWP:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aumd.word = IO_AUDIO.AUDIOIF[ch].AUMD.word;
	
	if (aumd.bit.IBYTE_SWP != enable){								/* pgr0872 */
		if (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set IBYTE_SWP Error:AUIEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aumd.bit.IBYTE_SWP = enable;
			IO_AUDIO.AUDIOIF[ch].AUMD.word = aumd.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register IHW_SWP.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_input_hw_swap( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aumd aumd;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set IHW_SWP:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set IHW_SWP:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aumd.word = IO_AUDIO.AUDIOIF[ch].AUMD.word;
	
	if (aumd.bit.IHW_SWP != enable){								/* pgr0872 */
		if (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set IHW_SWP Error:AUIEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aumd.bit.IHW_SWP = enable;
			IO_AUDIO.AUDIOIF[ch].AUMD.word = aumd.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register OBYTE_SWP.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_output_byte_swap( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aumd aumd;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set OBYTE_SWP:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set OBYTE_SWP:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aumd.word = IO_AUDIO.AUDIOIF[ch].AUMD.word;
	
	if (aumd.bit.OBYTE_SWP != enable){								/* pgr0872 */
		if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set OBYTE_SWP Error:AUOEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else if (dd_audio_get_loopback_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set OBYTE_SWP Error:LBF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aumd.bit.OBYTE_SWP = enable;
			IO_AUDIO.AUDIOIF[ch].AUMD.word = aumd.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register OHW_SWP.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_output_hw_swap( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aumd aumd;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set OHW_SWP:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set OHW_SWP:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aumd.word = IO_AUDIO.AUDIOIF[ch].AUMD.word;
	
	if (aumd.bit.OHW_SWP != enable){								/* pgr0872 */
		if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set OHW_SWP Error:AUOEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else if (dd_audio_get_loopback_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set OHW_SWP Error:LBF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aumd.bit.OHW_SWP = enable;
			IO_AUDIO.AUDIOIF[ch].AUMD.word = aumd.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register ISTG16.
 * @param  UINT8 ch
 * @param  UINT8 stage
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_in_fifo( UINT8 ch, UINT8 stage )
{
	volatile union io_audio_aumd aumd;
	INT32 ret_val = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set ISTG16:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (stage > 1){
		Ddim_Assertion(("[DD_AUDIO]Set ISTG16:input stage error : %d\n", stage));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aumd.word = IO_AUDIO.AUDIOIF[ch].AUMD.word;
	
	if (aumd.bit.ISTG16 != stage){							/* pgr0872 */
		if (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set ISTG16 Error:AUIEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aumd.bit.ISTG16 = stage;
			IO_AUDIO.AUDIOIF[ch].AUMD.word = aumd.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register OSTG16.
 * @param  UINT8 ch
 * @param  UINT8 stage
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_out_fifo( UINT8 ch, UINT8 stage )
{
	volatile union io_audio_aumd aumd;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set OSTG16:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (stage > 1){
		Ddim_Assertion(("[DD_AUDIO]Set OSTG16:input stage error : %d\n", stage));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aumd.word = IO_AUDIO.AUDIOIF[ch].AUMD.word;
	
	if (aumd.bit.OSTG16 != stage){							/* pgr0872 */
		if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set OSTG16 Error:AUOEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else if (dd_audio_get_loopback_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set OSTG16 Error:LBF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aumd.bit.OSTG16 = stage;
			IO_AUDIO.AUDIOIF[ch].AUMD.word = aumd.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register STEREO.
 * @param  UINT8 ch
 * @param  UINT8 stereo
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_stereo( UINT8 ch, UINT8 stereo )
{
	volatile union io_audio_aumd aumd;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set STEREO:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (stereo > 1){
		Ddim_Assertion(("[DD_AUDIO]Set STEREO:input stereo error : %d\n", stereo));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aumd.word = IO_AUDIO.AUDIOIF[ch].AUMD.word;
	
	if (aumd.bit.STEREO != stereo){								/* pgr0872 */
		if (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set STEREO Error:AUIEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set STEREO Error:AUOEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else if (dd_audio_get_loopback_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set STEREO Error:LBF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aumd.bit.STEREO = stereo;
			IO_AUDIO.AUDIOIF[ch].AUMD.word = aumd.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Get register STEREO.
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
static BOOL dd_audio_get_stereo( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get STEREO:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUMD.bit.STEREO == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Get register LB.
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
static BOOL dd_audio_get_loopback_flag( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get LBF:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUMD.bit.LBF == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}


/**** AUST ****/

/**
 * @brief  Get register IROF.
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
static BOOL dd_audio_get_in_overflow_flag( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get IROF:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUST.bit.IROF == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Clear register IROF.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_clear_in_overflow_flag( UINT8 ch )
{
	volatile union io_audio_aust aust;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Clear IROF:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aust.word = IO_AUDIO.AUDIOIF[ch].AUST.word;
	aust.bit.IROF = 0;
	IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief  Get register ORUF.
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
static BOOL dd_audio_get_out_underflow_flag( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get ORUF:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUST.bit.ORUF == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Clear register ORUF.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_clear_out_underflow_flag( UINT8 ch )
{
	volatile union io_audio_aust aust;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Clear ORUF:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aust.word = IO_AUDIO.AUDIOIF[ch].AUST.word;
	aust.bit.ORUF = 0;
	IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief  Set register OFIE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_in_overflow_intr( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aust aust;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set OFIE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set OFIE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aust.word = IO_AUDIO.AUDIOIF[ch].AUST.word;
	aust.bit.OFIE = enable;
	IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief  Get register OFIE.
 * @param  UINT8 ch
 * @return BOOL TRUE/FALSE
 */
static BOOL dd_audio_get_enable_in_overflow_intr( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get OFIE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUST.bit.OFIE == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register UFIE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_out_underflow_intr( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aust aust;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set UFIE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set UFIE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aust.word = IO_AUDIO.AUDIOIF[ch].AUST.word;
	aust.bit.UFIE = enable;
	IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return D_DDIM_OK;
}


/**
 * @brief  Get register UFIE.
 * @param  UINT8 ch
 * @return BOOL TRUE/FALSE
 */
static BOOL dd_audio_get_enable_out_underflow_intr( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get UFIE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUST.bit.UFIE == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Get register EINTI.
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
static BOOL dd_audio_get_in_usage_flag( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get EINTI:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUST.bit.EINTI == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Get register EINTO.
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
static BOOL dd_audio_get_out_usage_flag( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get EINTO:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUST.bit.EINTO == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register EINTIE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_in_usage_intr( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aust aust;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set EINTIE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set EINTIE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aust.word = IO_AUDIO.AUDIOIF[ch].AUST.word;
	
	if (aust.bit.EINTIE != enable){								/* pgr0872 */
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_enable_in_full_intr(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set EINTIE Error:INTIE = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else {
				aust.bit.EINTIE = enable;
				IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
			}
		}
		else {
			aust.bit.EINTIE = enable;
			IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	Dd_ARM_Dsb_Pou();
	
	return ret_val;
}

/**
 * @brief  Get register EINTIE.
 * @param  UINT8 ch
 * @return BOOL TRUE/FALSE
 */
static BOOL dd_audio_get_enable_in_usage_intr( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get EINTIE:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUST.bit.EINTIE == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register EINTOE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_out_usage_intr( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aust aust;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set EINTOE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set EINTOE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aust.word = IO_AUDIO.AUDIOIF[ch].AUST.word;
	
	if (aust.bit.EINTOE != enable){								/* pgr0872 */
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_enable_out_empty_intr(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set EINTOE Error:INTOE = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else {
				aust.bit.EINTOE = enable;
				IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
			}
		}
		else {
			aust.bit.EINTOE = enable;
			IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	Dd_ARM_Dsb_Pou();
	
	return ret_val;
}

/**
 * @brief  Get register EINTOE.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static BOOL dd_audio_get_enable_out_usage_intr( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get EINTOE:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUST.bit.EINTOE == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register EDMIE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_input_dma_request_enable( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aust aust;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set EDMIE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set EDMIE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aust.word = IO_AUDIO.AUDIOIF[ch].AUST.word;
	
	if (aust.bit.EDMIE != enable){								/* pgr0872 */
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_enable_fifo_full_dma_in(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set EDMIE Error:DMIE = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else {
				aust.bit.EDMIE = enable;
				IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
			}
		}
		else {
			aust.bit.EDMIE = enable;
			IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Get register EDMIE.
 * @param  UINT8 ch
 * @return BOOL TRUE/FALSE
 */
static BOOL dd_audio_get_input_dma_request_enable( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get EDMIE:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUST.bit.EDMIE == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register EDMOE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_output_dma_request_enable( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aust aust;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set EDMOE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set EDMOE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aust.word = IO_AUDIO.AUDIOIF[ch].AUST.word;
	
	if (aust.bit.EDMOE != enable){								/* pgr0872 */
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_enable_fifo_empty_dma_out(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set EDMOE Error:DMOE = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else {
				aust.bit.EDMOE = enable;
				IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
			}
		}
		else {
			aust.bit.EDMOE = enable;
			IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Get register EDMOE.
 * @param  UINT8 ch
 * @return BOOL TRUE/FALSE
 */
static BOOL dd_audio_get_output_dma_request_enable( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get EDMOE:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUST.bit.EDMOE == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register ESTG.
 * @param  UINT8 ch
 * @param  UINT8 control
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_register_usage( UINT8 ch, UINT8 control )
{
	volatile union io_audio_aust aust;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set ESTG:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (control > 3){
		Ddim_Assertion(("[DD_AUDIO]Set ESTG:input control error : %d\n", control));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aust.word = IO_AUDIO.AUDIOIF[ch].AUST.word;
	aust.bit.ESTG = control;
	IO_AUDIO.AUDIOIF[ch].AUST.word = aust.word;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return D_DDIM_OK;
}


/**** AUCC ****/

/**
 * @brief  Set register DIVMCK/DIVCK/DIVLR.
 * @param  UINT8 ch
 * @param  UINT8 mclock
 * @param  UINT8 auclock
 * @param  UINT8 lrclock
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_clock_div( UINT8 ch, UINT8 mclock, UINT8 auclock, UINT8 lrclock )
{
	volatile union io_audio_aucc aucc;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set DIVCLK:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (mclock > 31){
		Ddim_Assertion(("[DD_AUDIO]Set DIVCLK:input mclock error : %d\n", mclock));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (auclock > 7){
		Ddim_Assertion(("[DD_AUDIO]Set DIVCLK:input auclock error : %d\n", auclock));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (lrclock > 3){
		Ddim_Assertion(("[DD_AUDIO]Set DIVCLK:input lrclock error : %d\n", lrclock));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aucc.word = IO_AUDIO.AUDIOIF[ch].AUCC.word;
	
	if ((aucc.bit.DIVMCK != mclock)	||				/* pgr0872 */
		(aucc.bit.DIVCK != auclock)	||				/* pgr0872 */
		(aucc.bit.DIVLR != lrclock)	){				/* pgr0872 */
		
		if (dd_audio_get_enable_clock_division(ch) != FALSE){
			Ddim_Assertion(("[DD_AUDIO]Set DIVCLK Error:DIVE = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			aucc.bit.DIVMCK = mclock;
			aucc.bit.DIVCK = auclock;
			aucc.bit.DIVLR = lrclock;
			IO_AUDIO.AUDIOIF[ch].AUCC.word = aucc.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register DIVE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_clock_division( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aucc aucc;
	INT32 ret_val = D_DDIM_OK;
	volatile UINT32 wait_counter;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set DIVE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set DIVE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aucc.word = IO_AUDIO.AUDIOIF[ch].AUCC.word;
	
	if (aucc.bit.DIVE != enable){								/* pgr0872 */
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set DIVE Error:AUIEF = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set DIVE Error:AUOEF = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else if (dd_audio_get_loopback_flag(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set DIVE Error:LBF = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else {
				aucc.bit.DIVE = enable;
				IO_AUDIO.AUDIOIF[ch].AUCC.word = aucc.word;
			}
		}
		else {
			aucc.bit.DIVE = enable;
			IO_AUDIO.AUDIOIF[ch].AUCC.word = aucc.word;
		}
	}
	
	Dd_ARM_Dsb_Pou();
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
//	printf("[Set]DIVE%d=%d\n", ch, IO_AUDIO.AUDIOIF[ch].AUCC.bit.DIVE);
//	printf("[Set]DIVE%d=%d\n", ch, aucc.bit.DIVE);
	
	// wait DIVE readable
	if (IO_AUDIO.AUDIOIF[ch].AUCC.bit.DIVE != aucc.bit.DIVE){
		wait_counter = 0;
		while (IO_AUDIO.AUDIOIF[ch].AUCC.bit.DIVE != aucc.bit.DIVE){
			DDIM_User_Dly_Tsk(0);
			
			if (wait_counter > 10){
				Ddim_Print(("[DD_AUDIO]DIVE not read correct value\n"));
				break;
			}
			wait_counter++;
		}
	}
	
	return ret_val;
}

/**
 * @brief  Get register DIVE.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static BOOL dd_audio_get_enable_clock_division( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Get DIVE:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (IO_AUDIO.AUDIOIF[ch].AUCC.bit.DIVE == 1){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 * @brief  Set register AUCKOE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_output_clock( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_aucc aucc;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUCKOE: input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set AUCKOE:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	aucc.word = IO_AUDIO.AUDIOIF[ch].AUCC.word;
	
	if (aucc.bit.AUCKOE != enable){								/* pgr0872 */
		if (enable == D_DD_AUDIO_ENABLE){
			if (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set AUCKOE Error:AUIEF = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set AUCKOE Error:AUOEF = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else if (dd_audio_get_loopback_flag(ch) != FALSE){
				Ddim_Print(("[DD_AUDIO]Set AUCKOE Error:LBF = 1\n"));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else {
				aucc.bit.AUCKOE = enable;
				IO_AUDIO.AUDIOIF[ch].AUCC.word = aucc.word;
			}
		}
		else {
			aucc.bit.AUCKOE = enable;
			IO_AUDIO.AUDIOIF[ch].AUCC.word = aucc.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**** AUDP ****/

/**
 * @brief  Set register MIXPLAY.
 * @param  UINT8 ch
 * @param  UINT8 copy
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_mix_play( UINT8 ch, UINT8 control )
{
	volatile union io_audio_audp audp;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set MIXPLAY:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (control > 1){
		Ddim_Assertion(("[DD_AUDIO]Set MIXPLAY:input control error : %d\n", control));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	audp.word = IO_AUDIO.AUDIOIF[ch].AUDP.word;
	
	if (audp.bit.MIXPLAY != control){								/* pgr0872 */
		if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set MIXPLAY Error:AUOEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else if (control == 1){
			if(dd_audio_get_stereo(ch) == FALSE){
				Ddim_Print(("[DD_AUDIO]Set MIXPLAY Error. MIXPLAY = 1, STEREO = 0, ch = %d\n",ch));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else {
				audp.bit.MIXPLAY = control;
				IO_AUDIO.AUDIOIF[ch].AUDP.word = audp.word;
			}
		}
		else {
			audp.bit.MIXPLAY = control;
			IO_AUDIO.AUDIOIF[ch].AUDP.word = audp.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register AULRCP.
 * @param  UINT8 ch
 * @param  UINT8 copy
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_lr_data_copy( UINT8 ch, UINT8 copy )
{
	volatile union io_audio_audp audp;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AULRCP:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (copy > 1){
		Ddim_Assertion(("[DD_AUDIO]Set AULRCP:input copy error : %d\n", copy));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	audp.word = IO_AUDIO.AUDIOIF[ch].AUDP.word;
	
	if (audp.bit.AULRCP != copy){								/* pgr0872 */
		if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set AULRCP Error:AUOEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else if (copy == D_DD_AUDIO_ENABLE){
			if (IO_AUDIO.AUDIOIF[ch].AUCC.bit.AUCKOE == 0){
				Ddim_Print(("[DD_AUDIO]Set AULRCP = 1, AUCKOE = 0, ch = %d\n",ch));
				ret_val = D_DD_AUDIO_SYSTEM_ERROR;
			}
			else {
				audp.bit.AULRCP = copy;
				IO_AUDIO.AUDIOIF[ch].AUDP.word = audp.word;
			}
		}
		else {
			audp.bit.AULRCP = copy;
			IO_AUDIO.AUDIOIF[ch].AUDP.word = audp.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register AUIDF.
 * @param  UINT8 ch
 * @param  UINT8 shift
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_in_ahb_format( UINT8 ch, UINT8 format )
{
	volatile union io_audio_audp audp;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUIDF:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (format > 1){
		Ddim_Assertion(("[DD_AUDIO]Set AUIDF:input format error : %d\n", format));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	audp.word = IO_AUDIO.AUDIOIF[ch].AUDP.word;
	
	if (audp.bit.AUIDF != format){								/* pgr0872 */
		if (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set AUIDF Error:AUIEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			audp.bit.AUIDF = format;
			IO_AUDIO.AUDIOIF[ch].AUDP.word = audp.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register AUODF.
 * @param  UINT8 ch
 * @param  UINT8 shift
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_out_ahb_format( UINT8 ch, UINT8 format )
{
	volatile union io_audio_audp audp;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUODF:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (format > 1){
		Ddim_Assertion(("[DD_AUDIO]Set AUODF:input format error : %d\n", format));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	audp.word = IO_AUDIO.AUDIOIF[ch].AUDP.word;
	
	if (audp.bit.AUODF != format){								/* pgr0872 */
		if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set AUODF Error:AUOEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			audp.bit.AUODF = format;
			IO_AUDIO.AUDIOIF[ch].AUDP.word = audp.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register AUODS.
 * @param  UINT8 ch
 * @param  UINT8 shift
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_out_data_shift( UINT8 ch, UINT8 shift )
{
	volatile union io_audio_audp audp;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUODS:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (shift > 3){
		Ddim_Assertion(("[DD_AUDIO]Set AUODS:input shift error : %d\n", shift));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	audp.word = IO_AUDIO.AUDIOIF[ch].AUDP.word;
	
	if (audp.bit.AUODS != shift){								/* pgr0872 */
		if (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set AUODS Error:AUOEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			audp.bit.AUODS = shift;
			IO_AUDIO.AUDIOIF[ch].AUDP.word = audp.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**
 * @brief  Set register AUIDS.
 * @param  UINT8 ch
 * @param  UINT8 shift
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_in_data_shift( UINT8 ch, UINT8 shift )
{
	volatile union io_audio_audp audp;
	INT32 ret_val = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set AUIDS : input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (shift > 3){
		Ddim_Assertion(("[DD_AUDIO]Set AUIDS : input shift error : %d\n", shift));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	audp.word = IO_AUDIO.AUDIOIF[ch].AUDP.word;
	
	if (audp.bit.AUIDS != shift){								/* pgr0872 */
		if (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
			Ddim_Print(("[DD_AUDIO]Set AUIDS Error:AUIEF = 1\n"));
			ret_val = D_DD_AUDIO_SYSTEM_ERROR;
		}
		else {
			audp.bit.AUIDS = shift;
			IO_AUDIO.AUDIOIF[ch].AUDP.word = audp.word;
		}
	}
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return ret_val;
}

/**** AUDMA2CTL ****/

/**
 * @brief  Set register DMI2CHEN.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_in_dma2ch( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_audma2ctl audma2ctl;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set DMI2CHEN:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set DMI2CHEN:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	audma2ctl.word = IO_AUDIO.AUDIOIF[ch].AUDMA2CTL.word;
	audma2ctl.bit.DMI2CHEN = enable;
	IO_AUDIO.AUDIOIF[ch].AUDMA2CTL.word = audma2ctl.word;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief  Set register DMO2CHEN.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_enable_out_dma2ch( UINT8 ch, UINT8 enable )
{
	volatile union io_audio_audma2ctl audma2ctl;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set DMO2CHEN:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Set DMO2CHEN:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	audma2ctl.word = IO_AUDIO.AUDIOIF[ch].AUDMA2CTL.word;
	audma2ctl.bit.DMO2CHEN = enable;
	IO_AUDIO.AUDIOIF[ch].AUDMA2CTL.word = audma2ctl.word;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return D_DDIM_OK;
}


/**** AUDMISAMPLE ****/

/**
 * @brief  Set register DMISPL.
 * @param  UINT8 ch
 * @param  UINT16 sample
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_in_dma_sample( UINT8 ch, UINT16 sample )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set DMISPL:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	IO_AUDIO.AUDIOIF[ch].AUDMISAMPLE.bit.DMISPL = sample;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return D_DDIM_OK;
}


/**** AUDMOSAMPLE ****/

/**
 * @brief  Set register DMOSPL.
 * @param  UINT8 ch
 * @param  UINT16 sample
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_out_dma_sample( UINT8 ch, UINT16 sample )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Set DMOSPL : input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	IO_AUDIO.AUDIOIF[ch].AUDMOSAMPLE.bit.DMOSPL = sample;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
	
	return D_DDIM_OK;
}


/**
 * @brief  Set register AUxCK.
 * @param  UINT8 ch
 * @param  UINT8 val[1:stop/0:supply]
 */
static VOID dd_audio_set_clkstop_auxck( UINT8 ch, UINT8 val )
{
	switch (ch){
		case D_DD_AUDIO_IF_CH0 :
			Dd_Top_Set_CLKSTOP1_AU0CK(val);
			break;
		case D_DD_AUDIO_IF_CH2 :
			Dd_Top_Set_CLKSTOP1_AU2CK(val);
			break;
		case D_DD_AUDIO_IF_CH3 :
			Dd_Top_Set_CLKSTOP1_AU3CK(val);
			break;
		case D_DD_AUDIO_IF_CH4 :
			Dd_Top_Set_CLKSTOP1_AU4CK(val);
			break;
		case D_DD_AUDIO_IF_CH5 :
			Dd_Top_Set_CLKSTOP1_AU5CK(val);
			break;
		default :
			break;
	}
}

/**
 * @brief  Set register all AUxCK.
 * @param  UINT8 val[1:stop/0:supply]
 */
static VOID dd_audio_set_clkstop_auxck_all( UINT8 val )
{
	Dd_Top_Set_CLKSTOP1_AU0CK(val);
	Dd_Top_Set_CLKSTOP1_AU2CK(val);
	Dd_Top_Set_CLKSTOP1_AU3CK(val);
	Dd_Top_Set_CLKSTOP1_AU4CK(val);
	Dd_Top_Set_CLKSTOP1_AU5CK(val);
}

/**
 * @brief  Set register AUCKSEL.
 * @param  UINT8 ch
 * @param  UINT8 val
 */
static VOID dd_audio_set_clksel_auxsel( UINT8 ch, UINT8 val )
{
	switch (ch){
		case D_DD_AUDIO_IF_CH0 :
			Dd_Top_Set_CLKSEL8_AU0SEL(val);
			break;
		case D_DD_AUDIO_IF_CH2 :
			Dd_Top_Set_CLKSEL8_AU2SEL(val);
			break;
		case D_DD_AUDIO_IF_CH3 :
			Dd_Top_Set_CLKSEL8_AU3SEL(val);
			break;
		default :
			break;
	}
}

/**
 * @brief  Change AUCKSEL.
 * @param  UINT8 ch
 * @param  UINT8 val
 */
static VOID dd_audio_select_aumclk_in( UINT8 ch, UINT8 val )
{
	switch (ch){
		case D_DD_AUDIO_IF_CH0 :
			if (Dd_Top_Get_CLKSEL8_AU0SEL() == (val & 0x01)){
				return ;
			}
			break;
		case D_DD_AUDIO_IF_CH2 :
			if (Dd_Top_Get_CLKSEL8_AU2SEL() == (val & 0x01)){
				return ;
			}
			break;
		case D_DD_AUDIO_IF_CH3 :
			if (Dd_Top_Get_CLKSEL8_AU3SEL() == (val & 0x01)){
				return ;
			}
			break;
		default :
			return;
	}
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Audio_Spin_Lock);
	
	dd_audio_set_clkstop_auxck(ch, 1);
	dd_audio_set_clksel_auxsel(ch, val);
	dd_audio_set_clkstop_auxck(ch, 0);
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Audio_Spin_Lock);
}

/**
 * @brief  Set AudioMasterClock.
 * @param  E_DD_AUDIO_MASTER_CLOCK mclk
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
static INT32 dd_audio_set_masterclock( E_DD_AUDIO_MASTER_CLOCK mclk )
{
/*
	MCLK[MHz]	|	AUCLK	|	P10APLLDIV
	--------------------------------------
	8.192		|	3		|	0
	22.5792		|	1		|	1
	24.576		|	2		|	0
	36.864		|	0		|	2
*/

	volatile UINT8 net_auck;
	
	if (Dd_Top_Get_PLLCNT1_PL10ST() == 0){
		Ddim_Print(("[DD_AUDIO]dd_audio_set_masterclock:PLL10 is not started. \n"));
		return D_DD_AUDIO_SYSTEM_ERROR;
	}
	
	net_auck = Dd_Top_Get_CLKSTOP1_NETAUCK();
	
	switch (mclk){
		case E_DD_AUDIO_MASTER_CLOCK_8_192:
			if ((Dd_Top_Get_PLLCNT9_P10APLLDIV() != 0)	||
				(Dd_Top_Get_CLKSEL8_AUCLK() != 3)		||
				(Dd_Top_Get_PLLCNT2_PL10ASEL() == 0)	){
				
				dd_audio_set_clkstop_auxck_all(1);
				Dd_Top_Set_CLKSTOP1_NETAUCK(1);
				Dd_Top_Stop_Pll10A();
				dd_audio_wait_usec(6);			// wait 6us
				Dd_Top_Set_PLLCNT9_P10APLLDIV(0);
				Dd_Top_Start_Pll10A();
				Dd_Top_Set_CLKSEL8_AUCLK(3);
			}
			
			dd_audio_set_clkstop_auxck_all(0);
			Dd_Top_Set_CLKSTOP1_NETAUCK(net_auck);
			break;
			
		case E_DD_AUDIO_MASTER_CLOCK_22_5792:
			if ((Dd_Top_Get_PLLCNT9_P10APLLDIV() != 1)	||
				(Dd_Top_Get_CLKSEL8_AUCLK() != 1)		||
				(Dd_Top_Get_PLLCNT2_PL10ASEL() == 0)	){
				
				dd_audio_set_clkstop_auxck_all(1);
				Dd_Top_Set_CLKSTOP1_NETAUCK(1);
				Dd_Top_Stop_Pll10A();
				dd_audio_wait_usec(6);			// wait 6us
				Dd_Top_Set_PLLCNT9_P10APLLDIV(1);
				Dd_Top_Start_Pll10A();
				Dd_Top_Set_CLKSEL8_AUCLK(1);
			}
			
			dd_audio_set_clkstop_auxck_all(0);
			Dd_Top_Set_CLKSTOP1_NETAUCK(net_auck);
			break;
			
		case E_DD_AUDIO_MASTER_CLOCK_24_576:
			if ((Dd_Top_Get_PLLCNT9_P10APLLDIV() != 0)	||
				(Dd_Top_Get_CLKSEL8_AUCLK() != 2)		||
				(Dd_Top_Get_PLLCNT2_PL10ASEL() == 0)	){
				
				dd_audio_set_clkstop_auxck_all(1);
				Dd_Top_Set_CLKSTOP1_NETAUCK(1);
				Dd_Top_Stop_Pll10A();
				dd_audio_wait_usec(6);			// wait 6us
				Dd_Top_Set_PLLCNT9_P10APLLDIV(0);
				Dd_Top_Start_Pll10A();
				Dd_Top_Set_CLKSEL8_AUCLK(2);
			}
			
			dd_audio_set_clkstop_auxck_all(0);
			Dd_Top_Set_CLKSTOP1_NETAUCK(net_auck);
			break;
			
		case E_DD_AUDIO_MASTER_CLOCK_36_864:
			if ((Dd_Top_Get_PLLCNT9_P10APLLDIV() != 2)	||
				(Dd_Top_Get_CLKSEL8_AUCLK() != 0)		||
				(Dd_Top_Get_PLLCNT2_PL10ASEL() == 0)	){
				
				dd_audio_set_clkstop_auxck_all(1);
				Dd_Top_Set_CLKSTOP1_NETAUCK(1);
				Dd_Top_Stop_Pll10A();
				dd_audio_wait_usec(6);			// wait 6us
				Dd_Top_Set_PLLCNT9_P10APLLDIV(2);
				Dd_Top_Start_Pll10A();
				Dd_Top_Set_CLKSEL8_AUCLK(0);
			}
			
			dd_audio_set_clkstop_auxck_all(0);
			Dd_Top_Set_CLKSTOP1_NETAUCK(net_auck);
			break;
			
		case E_DD_AUDIO_MASTER_CLOCK_AUMCLKI:
			break;
			
		default:
			Ddim_Print(("[DD_AUDIO]dd_audio_set_masterclock: input mclk error : %d\n", mclk));
			return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	return D_DDIM_OK;
}


// Local Functions
/**
 * @brief  Disable Input Interrupt.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_SYSTEM_ERROR
 */
static INT32 dd_audio_disable_input_intr( UINT8 ch )
{
	INT32 ret;
	
	if (gDd_Audio_Ctrl_Cmmn[ch]->fifo_usage == E_DD_AUDIO_FIFO_USAGE_STAGES_16){
		ret = dd_audio_set_enable_in_full_intr(ch, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]dd_audio_disable_input_intr:dd_audio_set_enable_in_full_intr() = %x\n", ret));
			return D_DD_AUDIO_SYSTEM_ERROR;
		}
	}
	else {
		ret = dd_audio_set_enable_in_usage_intr(ch, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]dd_audio_disable_input_intr:dd_audio_set_enable_in_usage_intr() = %x\n", ret));
			return D_DD_AUDIO_SYSTEM_ERROR;
		}
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Disable Output Interrupt.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_SYSTEM_ERROR
 */
static INT32 dd_audio_disable_output_intr( UINT8 ch )
{
	INT32 ret;
	
	if (gDd_Audio_Ctrl_Cmmn[ch]->fifo_usage == E_DD_AUDIO_FIFO_USAGE_STAGES_16){
		ret = dd_audio_set_enable_out_empty_intr(ch, D_DD_AUDIO_DISABLE);
	}
	else {
		ret = dd_audio_set_enable_out_usage_intr(ch, D_DD_AUDIO_DISABLE);
	}
	
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]dd_audio_disable_output_intr:Set INTOE/EINTOE = %x\n", ret));
		return D_DD_AUDIO_SYSTEM_ERROR;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  wait for empty state of fifo.
 * @param  UINT8 ch
 */
static VOID dd_audio_wait_empty_input_fifo( UINT8 ch )
{
	volatile UINT8 wait_counter;
	DDIM_USER_ER ercd;
	volatile UINT32 dummy_read = 0;
	
	wait_counter = 0;
	while (dd_audio_get_audio_in_enable_flag(ch) != FALSE){
		ercd = DDIM_User_Dly_Tsk(0);
		
		if (ercd != D_DDIM_USER_E_OK){
			Ddim_Print(("[DD_AUDIO]dd_audio_wait_empty_input_fifo:dly_tsk error. ercd = %d\n", ercd));
			break;
		}
		
		if (wait_counter > D_DD_AUDIO_STOP_WAIT_LOOP){
			Ddim_Print(("[DD_AUDIO]dd_audio_wait_empty_input_fifo:TimeOut \n"));
			wait_counter = 0;
			if (dd_audio_get_enable_in_data24bit(ch) != FALSE){
				while (wait_counter < 15){
					dummy_read = IO_AUDIO.AUDIOIF[ch].AUIDL;
					dummy_read = IO_AUDIO.AUDIOIF[ch].AUIDR;
					wait_counter++;
				}
			}
			else {
				while (wait_counter < 15){
					dummy_read = IO_AUDIO.AUDIOIF[ch].AUIDLR;
					wait_counter++;
				}
			}
			
			break;				// time out
		}
		
		wait_counter++;
	}
	
	// remove the warnning of the compiler
	if (dummy_read == 0){
		; // Nothing
	}
}

/**
 * @brief  wait for empty state of fifo.
 * @param  UINT8 ch
 */
static VOID dd_audio_wait_empty_output_fifo( UINT8 ch )
{
	volatile UINT8 wait_counter;
	DDIM_USER_ER ercd;
	
	wait_counter = 0;
	while (dd_audio_get_audio_out_enable_flag(ch) != FALSE){
		ercd = DDIM_User_Dly_Tsk(0);
		
		if (ercd != D_DDIM_USER_E_OK){
			Ddim_Print(("[DD_AUDIO]dd_audio_wait_empty_output_fifo:dly_tsk error. ercd = %d\n", ercd));
			break;
		}
		
		if (wait_counter > D_DD_AUDIO_STOP_WAIT_LOOP){
			Ddim_Print(("[DD_AUDIO]dd_audio_wait_empty_output_fifo:TimeOut \n"));
			break;				// time out
		}
		
		wait_counter++;
	}
}

/**
 * @brief  wait for store data to fifo.
 * @param  UINT8 ch
 */
static VOID dd_audio_wait_store_output_fifo( UINT8 ch )
{
	volatile UINT8 wait_counter;
	
	wait_counter = 0;
	while (Dd_Audio_Get_OutputFifoStatus(ch) == 0){
		wait_counter++;
		
		// fail safe
		if (wait_counter > 100){
			Ddim_Print(("[DD_AUDIO]dd_audio_wait_store_output_fifo:TimeOut. ch=%d\n", ch));
			break;
		}
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/**
 * @brief  The audio input channel is exclusively controlled.
 * @param  UINT8 ch
 * @param  INT32 tmout
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_SEM_NG/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SEM_TIMEOUT
 */
INT32 Dd_Audio_Open_Input( UINT8 ch, INT32 tmout )
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Open_Input:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (tmout < D_DDIM_USER_SEM_WAIT_FEVR){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Open_Input:input timeout error : %d\n", tmout));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// Exclusive check
	if (tmout == D_DDIM_USER_SEM_WAIT_POL){
		ercd = DDIM_User_Pol_Sem(SID_DD_AUDIO_IF_IN(ch));							// pol_sem()
	}
	else {
		ercd = DDIM_User_Twai_Sem(SID_DD_AUDIO_IF_IN(ch), (DDIM_USER_TMO)tmout);	// twai_sem()
	}
	
	switch (ercd){
		case D_DDIM_USER_E_OK:
			return D_DDIM_OK;
		case D_DDIM_USER_E_TMOUT:
			return D_DD_AUDIO_SEM_TIMEOUT;
		default:
			return D_DD_AUDIO_SEM_NG;
	}
}

/**
 * @brief  The audio output channel is exclusively controlled.
 * @param  UINT8 ch
 * @param  INT32 tmout
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_SEM_NG/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SEM_TIMEOUT
 */
INT32 Dd_Audio_Open_Output( UINT8 ch, INT32 tmout )
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Open_Output:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (tmout < D_DDIM_USER_SEM_WAIT_FEVR){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Open_Output:input timeout error : %d\n", tmout));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// Exclusive check
	if (tmout == D_DDIM_USER_SEM_WAIT_POL){
		ercd = DDIM_User_Pol_Sem(SID_DD_AUDIO_IF_OUT(ch));							// pol_sem()
	}
	else {
		ercd = DDIM_User_Twai_Sem(SID_DD_AUDIO_IF_OUT(ch), (DDIM_USER_TMO)tmout);	// twai_sem()
	}
	
	switch (ercd){
		case D_DDIM_USER_E_OK:
			return D_DDIM_OK;
		case D_DDIM_USER_E_TMOUT:
			return D_DD_AUDIO_SEM_TIMEOUT;
		default:
			return D_DD_AUDIO_SEM_NG;
	}
}

/**
 * @brief  The exclusive control of control Input Channel it is released.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_SEM_NG/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Close_Input( UINT8 ch )
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Close_Input:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	// Exclusive release
	ercd = DDIM_User_Sig_Sem(SID_DD_AUDIO_IF_IN(ch));					// sig_sem()
	if (ercd == D_DDIM_USER_E_OK){
		return D_DDIM_OK;
	}
	else {
		return D_DD_AUDIO_SEM_NG;
	}
}

/**
 * @brief  The exclusive control of control Output Channel it is released.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_SEM_NG/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Close_Output( UINT8 ch )
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Close_Output:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	// Exclusive release
	ercd = DDIM_User_Sig_Sem(SID_DD_AUDIO_IF_OUT(ch));					// sig_sem()
	if (ercd == D_DDIM_USER_E_OK){
		return D_DDIM_OK;
	}
	else {
		return D_DD_AUDIO_SEM_NG;
	}
}

/**
 * @brief  AudioI/F interrupt handler
 * @param  UINT8 ch
 */
VOID Dd_Audio_Int_Handler( UINT8 ch )
{
	T_DD_AUDIO_CALLBACK callback_func;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Int_Handler:input channel error : %d\n", ch));
		return ;
	}
#endif	// CO_PARAM_CHECK
	
	// input interrupt
	if ((dd_audio_get_enable_in_full_intr(ch) != FALSE) && (dd_audio_get_in_full(ch) != FALSE)){
		
		Ddim_Print(("[DD_AUDIO]Interrupt:InputRegisterFull[%d]\n",ch));
		
		(VOID)dd_audio_set_enable_in_full_intr(ch, D_DD_AUDIO_DISABLE);
		
		callback_func = gDd_Audio_Input_Callback_Func[ch];
		if (callback_func != NULL){
			callback_func();
		}
	}
	
	if ((dd_audio_get_enable_in_usage_intr(ch) != FALSE) && (dd_audio_get_in_usage_flag(ch) != FALSE)){
		
		Ddim_Print(("[DD_AUDIO]Interrupt:Write Completion to Input Register[%d]\n",ch));
		
		(VOID)dd_audio_set_enable_in_usage_intr(ch, D_DD_AUDIO_DISABLE);
		
		callback_func = gDd_Audio_Input_Callback_Func[ch];
		if (callback_func != NULL){
			callback_func();
		}
	}
	
	// input error interrupt
	if ((dd_audio_get_enable_in_overflow_intr(ch) != FALSE) && (dd_audio_get_in_overflow_flag(ch) != FALSE)){
		
		Ddim_Print(("[DD_AUDIO]Interrupt:OverFlow[%d]\n",ch));
		
		(VOID)dd_audio_clear_in_overflow_flag(ch);
		
		callback_func = gDd_Audio_OverFlow_Callback_Func[ch];
		if (callback_func != NULL){
			callback_func();
		}
	}
	
	// output interrupt
	if ((dd_audio_get_enable_out_empty_intr(ch) != FALSE) && (dd_audio_get_out_empty(ch) != FALSE)){
		
		Ddim_Print(("[DD_AUDIO]Interrupt:OutputRegisterEmpty[%d]\n",ch));
		
		(VOID)dd_audio_set_enable_out_empty_intr(ch, D_DD_AUDIO_DISABLE);
		
		callback_func = gDd_Audio_Output_Callback_Func[ch];
		if (callback_func != NULL){
			callback_func();
		}
	}
	
	if ((dd_audio_get_enable_out_usage_intr(ch) != FALSE) && (dd_audio_get_out_usage_flag(ch) != FALSE)){
		
		Ddim_Print(("[DD_AUDIO]Interrupt:Read Completion from Output Register[%d]\n",ch));
		
		(VOID)dd_audio_set_enable_out_usage_intr(ch, D_DD_AUDIO_DISABLE);
		
		callback_func = gDd_Audio_Output_Callback_Func[ch];
		if (callback_func != NULL){
			callback_func();
		}
	}
	
	// output error interrupt
	if ((dd_audio_get_enable_out_underflow_intr(ch) != FALSE) && (dd_audio_get_out_underflow_flag(ch) != FALSE)){
		
		Ddim_Print(("[DD_AUDIO]Interrupt:UnderFlow[%d]\n",ch))
		
		(VOID)dd_audio_clear_out_underflow_flag(ch);
		
		callback_func = gDd_Audio_UnderFlow_Callback_Func[ch];
		if (callback_func != NULL){
			callback_func();
		}
	}
}

/**
 * @brief  Initialized AudioMacro Setting
 */
VOID Dd_Audio_Init( VOID )
{
	// software reset
	(VOID)Dd_Audio_Reset(D_DD_AUDIO_IF_CH0);
	(VOID)Dd_Audio_Reset(D_DD_AUDIO_IF_CH1);
	(VOID)Dd_Audio_Reset(D_DD_AUDIO_IF_CH2);
	(VOID)Dd_Audio_Reset(D_DD_AUDIO_IF_CH3);
	(VOID)Dd_Audio_Reset(D_DD_AUDIO_IF_CH4);
	(VOID)Dd_Audio_Reset(D_DD_AUDIO_IF_CH5);
	
	// Set Audio PLL
	if (Dd_Top_Get_PLLCNT1_PL10AST() == 0){
		Dd_Top_Stop_Pll10A();
		dd_audio_wait_usec(6);			// wait 6us
		
		Dd_Top_Set_CLKSTOP3_PDM0CK(1);
		Dd_Top_Set_CLKSTOP3_PDM1CK(1);
		
		Dd_Top_Start_Pll10A();
	}
	
	gDd_Audio_MasterClock = (E_DD_AUDIO_MASTER_CLOCK)0xFF;
}

/**
 * @brief  Reset AudioI/F Macro
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Reset( UINT8 ch )
{
	INT32 ret;
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Reset:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	ret = dd_audio_set_sw_reset(ch);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Reset:dd_audio_set_sw_reset() = %x\n", ret));
		return D_DD_AUDIO_SYSTEM_ERROR;
	}
	
	// Initialize Global Data
	gDd_Audio_Ctrl_Cmmn[ch]->fifo_usage = E_DD_AUDIO_FIFO_USAGE_STAGES_8;
	gDd_Audio_Ctrl_Cmmn[ch]->channel = E_DD_AUDIO_CHANNEL_MONO;
	
	gDd_Audio_Ctrl_In[ch]->format = E_DD_AUDIO_DATA_REG_FRMT_R_JUST;
	gDd_Audio_Ctrl_In[ch]->fifo_stages = E_DD_AUDIO_FIFO_STAGES_1;
	gDd_Audio_Ctrl_In[ch]->ahb_format = E_DD_AUDIO_AHB_FRMT_R_JUST;
	gDd_Audio_Ctrl_In[ch]->bit_shift = E_DD_AUDIO_BIT_SHIFT_0;
	
	gDd_Audio_Ctrl_Out[ch]->format = E_DD_AUDIO_DATA_REG_FRMT_R_JUST;
	gDd_Audio_Ctrl_Out[ch]->fifo_stages = E_DD_AUDIO_FIFO_STAGES_1;
	gDd_Audio_Ctrl_Out[ch]->ahb_format = E_DD_AUDIO_AHB_FRMT_R_JUST;
	gDd_Audio_Ctrl_Out[ch]->lr_copy = D_DD_AUDIO_DISABLE;
	gDd_Audio_Ctrl_Out[ch]->mix_play = D_DD_AUDIO_DISABLE;
	gDd_Audio_Ctrl_Out[ch]->bit_shift = E_DD_AUDIO_BIT_SHIFT_0;
	
	if (ch != D_DD_AUDIO_IF_CH1){
		gDd_Audio_I2S_Cmmn[ch]->aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
		gDd_Audio_I2S_Cmmn[ch]->div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_2;
		gDd_Audio_I2S_Cmmn[ch]->div_auclk = E_DD_AUDIO_AUCLK_DIV_16;
		gDd_Audio_I2S_Cmmn[ch]->div_lrclk = E_DD_AUDIO_AULR_DIV_32;
		gDd_Audio_I2S_Cmmn[ch]->clk_div_enable = D_DD_AUDIO_DISABLE;
		gDd_Audio_I2S_Cmmn[ch]->master_slave = E_DD_AUDIO_CLK_SLAVE;
	}
	
	gDd_Audio_I2S_In[ch]->mode_in = E_DD_AUDIO_MODE_L_JUST;
	gDd_Audio_I2S_In[ch]->bit_in = E_DD_AUDIO_BIT_8BIT;
	
	gDd_Audio_I2S_Out[ch]->mode_out = E_DD_AUDIO_MODE_L_JUST;
	gDd_Audio_I2S_Out[ch]->bit_out = E_DD_AUDIO_BIT_8BIT;
	gDd_Audio_I2S_Out[ch]->fs = E_DD_AUDIO_FS_32FS;
	
	gDd_Audio_Dma_In[ch]->dma_2ch = D_DD_AUDIO_DISABLE;
	gDd_Audio_Dma_In[ch]->dma_trnsf_cnt = 0;
	
	gDd_Audio_Dma_Out[ch]->dma_2ch = D_DD_AUDIO_DISABLE;
	gDd_Audio_Dma_Out[ch]->dma_trnsf_cnt = 0;
	
	gDd_Audio_OverFlow_Callback_Func[ch] = NULL;
	gDd_Audio_UnderFlow_Callback_Func[ch] = NULL;
	gDd_Audio_Input_Callback_Func[ch] = NULL;
	gDd_Audio_Output_Callback_Func[ch] = NULL;
	
	return D_DDIM_OK;
}

/**
 * @brief  Set Audio Control Information(common)
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_CTRL *ctrl_inf
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Ctrl_Common( UINT8 ch, T_DD_AUDIO_CTRL_COMMON* ctrl_inf )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_Common:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (ctrl_inf == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_Common:ctrl_inf = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (ctrl_inf->fifo_usage < E_DD_AUDIO_FIFO_USAGE_STAGES_16){
		ret = dd_audio_set_register_usage(ch, ctrl_inf->fifo_usage);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Common:dd_audio_set_register_usage() = %x\n", ret));
			return ret;
		}
	}
	else if (ctrl_inf->fifo_usage == E_DD_AUDIO_FIFO_USAGE_STAGES_16){
		;		// Do Nothing.
	}
	else {
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_Common:input fifo_usage error : %d\n", ctrl_inf->fifo_usage));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	gDd_Audio_Ctrl_Cmmn[ch]->fifo_usage = ctrl_inf->fifo_usage;
	
	if (ctrl_inf->channel <= E_DD_AUDIO_CHANNEL_STEREO){
		ret = dd_audio_set_stereo(ch, ctrl_inf->channel);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Common:dd_audio_set_stereo() = %x\n", ret));
			return ret;
		}
		else {
			gDd_Audio_Ctrl_Cmmn[ch]->channel = ctrl_inf->channel;
		}
	}
	else {
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_Common:input audio channel error : %d\n", ctrl_inf->channel));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Get Audio Control Information(common)
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_CTRL_COMMON* ctrl_inf
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Get_Ctrl_Common( UINT8 ch, T_DD_AUDIO_CTRL_COMMON* ctrl_inf )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_Common:input channel error : %d\n", ch));
		*ctrl_inf = *gDd_Audio_Ctrl_Cmmn[0];
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (ctrl_inf == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_Common:ctrl_inf = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	*ctrl_inf = *gDd_Audio_Ctrl_Cmmn[ch];
	
	return D_DDIM_OK;
}

/**
 * @brief  Set Audio Control Information(input)
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_CTRL_IN *ctrl_inf
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Ctrl_Input( UINT8 ch, T_DD_AUDIO_CTRL_IN* ctrl_inf )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_Input:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (ctrl_inf == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_Input:ctrl_inf = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	ret = dd_audio_set_input_register_format(ch, ctrl_inf->format);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Input:dd_audio_set_input_register_format() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Ctrl_In[ch]->format = ctrl_inf->format;
	}
	
	ret = dd_audio_set_in_fifo(ch, ctrl_inf->fifo_stages);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Input:dd_audio_set_in_fifo() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Ctrl_In[ch]->fifo_stages = ctrl_inf->fifo_stages;
	}
	
	ret = dd_audio_set_in_ahb_format(ch, ctrl_inf->ahb_format);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Input:dd_audio_set_in_ahb_format() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Ctrl_In[ch]->ahb_format = ctrl_inf->ahb_format;
	}

	ret = dd_audio_set_in_data_shift(ch, ctrl_inf->bit_shift);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Input:dd_audio_set_in_data_shift() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Ctrl_In[ch]->bit_shift = ctrl_inf->bit_shift;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Get Audio Control Information(input)
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_CTRL_IN *ctrl_inf
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Get_Ctrl_Input( UINT8 ch, T_DD_AUDIO_CTRL_IN* ctrl_inf )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_Input:input channel error : %d\n", ch));
		*ctrl_inf = *gDd_Audio_Ctrl_In[0];
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (ctrl_inf == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_Input:ctrl_inf = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	*ctrl_inf = *gDd_Audio_Ctrl_In[ch];
	return D_DDIM_OK;
}

/**
 * @brief  Set Audio Control Information(output)
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_CTRL_OUT *ctrl_inf
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Ctrl_Output( UINT8 ch, T_DD_AUDIO_CTRL_OUT* ctrl_inf )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_Output:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (ctrl_inf == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_Output:ctrl_inf = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	ret = dd_audio_set_output_register_format(ch, ctrl_inf->format);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Output:dd_audio_set_output_register_format() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Ctrl_Out[ch]->format = ctrl_inf->format;
	}
	
	ret = dd_audio_set_out_fifo(ch, ctrl_inf->fifo_stages);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Output:dd_audio_set_out_fifo() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Ctrl_Out[ch]->fifo_stages = ctrl_inf->fifo_stages;
	}
	
	ret = dd_audio_set_out_ahb_format(ch, ctrl_inf->ahb_format);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Output:dd_audio_set_out_ahb_format() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Ctrl_Out[ch]->ahb_format = ctrl_inf->ahb_format;
	}
	
	if ((ctrl_inf->lr_copy == D_DD_AUDIO_ENABLE) && (gDd_Audio_I2S_Cmmn[ch]->master_slave == E_DD_AUDIO_CLK_SLAVE)){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Output: [SlaveMode] LR Copy Enable\n"));
		return D_DD_AUDIO_SYSTEM_ERROR;
	}
	
	ret = dd_audio_set_lr_data_copy(ch, ctrl_inf->lr_copy);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Output:dd_audio_set_lr_data_copy() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Ctrl_Out[ch]->lr_copy = ctrl_inf->lr_copy;
	}
	
	ret = dd_audio_set_mix_play(ch, ctrl_inf->mix_play);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Output:dd_audio_set_mix_play() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Ctrl_Out[ch]->mix_play = ctrl_inf->mix_play;
	}
	
	ret = dd_audio_set_out_data_shift(ch, ctrl_inf->bit_shift);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_Output:dd_audio_set_out_data_shift() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Ctrl_Out[ch]->bit_shift = ctrl_inf->bit_shift;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Get Audio Control Information(output)
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_CTRL_OUT *ctrl_inf
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Get_Ctrl_Output( UINT8 ch, T_DD_AUDIO_CTRL_OUT* ctrl_inf )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_Output:input channel error : %d\n", ch));
		*ctrl_inf = *gDd_Audio_Ctrl_Out[0];
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (ctrl_inf == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_Output:ctrl_inf = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	*ctrl_inf = *gDd_Audio_Ctrl_Out[ch];
	
	return D_DDIM_OK;
}

/**
 * @brief  Set I2S Common Setting
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_I2S_CMMN *i2s_setting
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Ctrl_I2sCmmn( UINT8 ch, T_DD_AUDIO_I2S_CMMN* i2s_setting )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sCmmn:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (i2s_setting == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sCmmn:i2s_setting = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if ((ch >= D_DD_AUDIO_IF_CH4) && (i2s_setting->aumclki == E_DD_AUDIO_MASTER_CLOCK_AUMCLKI)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sCmmn:Invalid setting ch=%d, aumclki=%d\n", ch, i2s_setting->aumclki));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
#endif	// CO_PARAM_CHECK
	
	if (ch == D_DD_AUDIO_IF_CH1){
		return D_DDIM_OK;
	}
	
	if (gDd_Audio_MasterClock != i2s_setting->aumclki){
		if (i2s_setting->aumclki == E_DD_AUDIO_MASTER_CLOCK_AUMCLKI){
			// APLCLK -> AUMCLKI
			dd_audio_select_aumclk_in(ch, 1);
		}
		else {
			dd_audio_select_aumclk_in(ch, 0);
		}
		
		Dd_ARM_Dsb_Pou();
		
		ret = dd_audio_set_masterclock(i2s_setting->aumclki);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_I2sCmmn:dd_audio_set_masterclock() = %x\n", ret));
			return ret;
		}
		gDd_Audio_I2S_Cmmn[ch]->aumclki = i2s_setting->aumclki;
		gDd_Audio_MasterClock = i2s_setting->aumclki;
	}
	
	if ((gDd_Audio_I2S_Cmmn[ch]->div_aumclko != i2s_setting->div_aumclko)	||
		(gDd_Audio_I2S_Cmmn[ch]->div_auclk != i2s_setting->div_auclk)		||
		(gDd_Audio_I2S_Cmmn[ch]->div_lrclk != i2s_setting->div_lrclk)		){
		
		ret = dd_audio_set_enable_clock_division(ch, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_I2sCmmn:dd_audio_set_enable_clock_division() = %x\n", ret));
			return ret;
		}
		
		Dd_ARM_Dsb_Pou();
		
		ret = dd_audio_set_clock_div(ch, i2s_setting->div_aumclko, i2s_setting->div_auclk, i2s_setting->div_lrclk);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_I2sCmmn:dd_audio_set_clock_div() = %x\n", ret));
			return ret;
		}
		else {
			gDd_Audio_I2S_Cmmn[ch]->div_aumclko = i2s_setting->div_aumclko;
			gDd_Audio_I2S_Cmmn[ch]->div_auclk = i2s_setting->div_auclk;
			gDd_Audio_I2S_Cmmn[ch]->div_lrclk = i2s_setting->div_lrclk;
		}
	}
	
	if (i2s_setting->master_slave == E_DD_AUDIO_CLK_MASTER){
		ret = dd_audio_set_enable_output_clock(ch, D_DD_AUDIO_ENABLE);
	}
	else if (i2s_setting->master_slave == E_DD_AUDIO_CLK_SLAVE){
		ret = dd_audio_set_enable_output_clock(ch, D_DD_AUDIO_DISABLE);
	}
	else {
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sCmmn:master_slave is invalid(=%d)\n", i2s_setting->master_slave));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_I2sCmmn:Set AUCKOE: ret=%x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_I2S_Cmmn[ch]->master_slave = i2s_setting->master_slave;
	}
	
	ret = dd_audio_set_enable_clock_division(ch, i2s_setting->clk_div_enable);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_I2sCmmn:Set DIVE: ret=%x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_I2S_Cmmn[ch]->clk_div_enable = i2s_setting->clk_div_enable;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Get I2S Common Setting
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_I2S_CMMN *i2s_setting
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Get_Ctrl_I2sCmmn( UINT8 ch, T_DD_AUDIO_I2S_CMMN* i2s_setting )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_I2sCmmn:input channel error : %d\n", ch));
		*i2s_setting = *gDd_Audio_I2S_Cmmn[0];
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (i2s_setting == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_I2sCmmn:i2s_setting = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	*i2s_setting = *gDd_Audio_I2S_Cmmn[ch];
	
	return D_DDIM_OK;
}

/**
 * @brief  Set I2S Setting(Input)
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_I2S_IN *i2s_setting
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Ctrl_I2sIn( UINT8 ch, T_DD_AUDIO_I2S_IN* i2s_setting )
{
	INT32 ret;
	UINT8 format;
	
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sIn:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (i2s_setting == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sIn:i2s_setting = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (i2s_setting->bit_in > E_DD_AUDIO_BIT_24BIT){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sIn:bit_in error = %d\n", i2s_setting->bit_in));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (i2s_setting->mode_in > E_DD_AUDIO_MODE_I2S){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sIn:mode_in error = %d\n", i2s_setting->mode_in));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	format = gDd_Audio_AUIF_Tbl[i2s_setting->bit_in][i2s_setting->mode_in];
	
	ret = dd_audio_set_input_data_format(ch, format);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Set_Ctrl_I2sIn:dd_audio_set_input_data_format() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_I2S_In[ch]->mode_in = i2s_setting->mode_in;
		gDd_Audio_I2S_In[ch]->bit_in = i2s_setting->bit_in;
	}
	
	if (i2s_setting->bit_in == E_DD_AUDIO_BIT_24BIT){
		ret = dd_audio_set_enable_in_data24bit(ch, D_DD_AUDIO_ENABLE);
	}
	else {
		ret = dd_audio_set_enable_in_data24bit(ch, D_DD_AUDIO_DISABLE);
	}
	
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_I2sIn:dd_audio_set_enable_in_data24bit() = %x\n", ret));
		return ret;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Get I2S Setting Information
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_I2S_IN *i2s_setting
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Get_Ctrl_I2sIn( UINT8 ch, T_DD_AUDIO_I2S_IN* i2s_setting )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_I2sIn:input channel error : %d\n", ch));
		*i2s_setting = *gDd_Audio_I2S_In[0];
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (i2s_setting == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_I2sIn:i2s_setting = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	*i2s_setting = *gDd_Audio_I2S_In[ch];
	
	return D_DDIM_OK;
}

/**
 * @brief  Set I2S Setting(Output)
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_I2S_OUT *i2s_setting
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Ctrl_I2sOut( UINT8 ch, T_DD_AUDIO_I2S_OUT* i2s_setting )
{
	INT32 ret;
	UINT8 format;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sOut:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (i2s_setting == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sOut:i2s_setting = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (i2s_setting->bit_out > E_DD_AUDIO_BIT_24BIT){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sOut:bit_out error = %d\n", i2s_setting->bit_out));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (i2s_setting->mode_out > E_DD_AUDIO_MODE_I2S){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sOut:mode_out error = %d\n", i2s_setting->mode_out));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (i2s_setting->fs > E_DD_AUDIO_FS_128FS){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_I2sOut:fs error = %d\n", i2s_setting->fs));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	format = (gDd_Audio_AUOF_Tbl[i2s_setting->bit_out][i2s_setting->mode_out])[i2s_setting->fs];
	if (0xFF == format){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_I2sOut:AUOF Not Set: bit=%d, mode=%d, fs=%d\n", 
					(int)i2s_setting->bit_out, (int)i2s_setting->mode_out, (int)i2s_setting->fs));
		return D_DD_AUDIO_SYSTEM_ERROR;
	}
	else {
		ret = dd_audio_set_output_data_format(ch, format);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_I2sOut:dd_audio_set_output_data_format() = %x\n", ret));
			return ret;
		}
		else {
			gDd_Audio_I2S_Out[ch]->mode_out = i2s_setting->mode_out;
			gDd_Audio_I2S_Out[ch]->bit_out = i2s_setting->bit_out;
			gDd_Audio_I2S_Out[ch]->fs = i2s_setting->fs;
		}
	}
	
	if (i2s_setting->bit_out == E_DD_AUDIO_BIT_24BIT){
		ret = dd_audio_set_enable_out_data24bit(ch, D_DD_AUDIO_ENABLE);
	}
	else {
		ret = dd_audio_set_enable_out_data24bit(ch, D_DD_AUDIO_DISABLE);
	}
	
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_I2sOut:dd_audio_set_enable_out_data24bit() = %x\n", ret));
		return ret;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Get I2S Setting Information
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_I2S_OUT *i2s_setting
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Get_Ctrl_I2sOut( UINT8 ch, T_DD_AUDIO_I2S_OUT* i2s_setting )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_I2sOut:input channel error : %d\n", ch));
		*i2s_setting = *gDd_Audio_I2S_Out[0];
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (i2s_setting == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_I2sOut:i2s_setting = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	*i2s_setting = *gDd_Audio_I2S_Out[ch];
	
	return D_DDIM_OK;
}


/**
 * @brief  Set Audio DMA Information of Input
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_CTRL *ctrl_inf
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Ctrl_DmaIn( UINT8 ch, T_DD_AUDIO_DMA* dma_setting )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_DmaIn:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (dma_setting == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_DmaIn:dma_setting = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	ret = dd_audio_set_enable_in_dma2ch(ch, dma_setting->dma_2ch);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_DmaIn:dd_audio_set_enable_in_dma2ch() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Dma_In[ch]->dma_2ch = dma_setting->dma_2ch;
	}
	
	if (dma_setting->dma_2ch == D_DD_AUDIO_ENABLE){
		if ((dd_audio_get_enable_fifo_full_dma_in(ch) == FALSE) && (dd_audio_get_input_dma_request_enable(ch) == FALSE)){
			ret = dd_audio_set_in_dma_sample(ch, dma_setting->dma_trnsf_cnt);
			if (ret != D_DDIM_OK){
				Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_DmaIn:dd_audio_set_in_dma_sample() = %x\n", ret));
				return ret;
			}
			else {
				gDd_Audio_Dma_In[ch]->dma_trnsf_cnt = dma_setting->dma_trnsf_cnt;
			}
		}
		else {
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_DmaIn:DMIE=%d, EDMIE=%d\n", dd_audio_get_enable_fifo_full_dma_in(ch), dd_audio_get_input_dma_request_enable(ch)));
			return D_DD_AUDIO_SYSTEM_ERROR;
		}
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Get Audio DMA Information(Input)
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_CTRL *ctrl_inf
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Get_Ctrl_DmaIn( UINT8 ch, T_DD_AUDIO_DMA* dma_setting )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_DmaIn:input channel error : %d\n", ch));
		*dma_setting = *gDd_Audio_Dma_In[0];
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (dma_setting == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_DmaIn:dma_setting = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	*dma_setting = *gDd_Audio_Dma_In[ch];
	
	return D_DDIM_OK;
}

/**
 * @brief  Set Audio DMA Information(Output)
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_CTRL *ctrl_inf
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Ctrl_DmaOut( UINT8 ch, T_DD_AUDIO_DMA* dma_setting )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_DmaOut:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (dma_setting == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Ctrl_DmaOut:dma_setting = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	ret = dd_audio_set_enable_out_dma2ch(ch, dma_setting->dma_2ch);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_DmaOut:dd_audio_set_enable_out_dma2ch() = %x\n", ret));
		return ret;
	}
	else {
		gDd_Audio_Dma_Out[ch]->dma_2ch = dma_setting->dma_2ch;
	}
	
	if (dma_setting->dma_2ch == D_DD_AUDIO_ENABLE){
		if ((dd_audio_get_enable_fifo_empty_dma_out(ch) == FALSE) && (dd_audio_get_output_dma_request_enable(ch) == FALSE) ){
			ret = dd_audio_set_out_dma_sample(ch, dma_setting->dma_trnsf_cnt);
			if (ret != D_DDIM_OK){
				Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_DmaOut:dd_audio_set_out_dma_sample() = %x\n", ret));
				return ret;
			}
			else {
				gDd_Audio_Dma_Out[ch]->dma_trnsf_cnt = dma_setting->dma_trnsf_cnt;
			}
		}
		else {
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Ctrl_DmaOut:DMOE=%d, EDMOE=%d\n", dd_audio_get_enable_fifo_empty_dma_out(ch), dd_audio_get_output_dma_request_enable(ch)));
			return D_DD_AUDIO_SYSTEM_ERROR;
		}
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Get Audio DMA Information(Output)
 * @param  UINT8 ch
 * @param  T_DD_AUDIO_CTRL *ctrl_inf
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Get_Ctrl_DmaOut( UINT8 ch, T_DD_AUDIO_DMA* dma_setting )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_DmaOut:input channel error : %d\n", ch));
		*dma_setting = *gDd_Audio_Dma_Out[0];
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (dma_setting == NULL){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Ctrl_DmaOut:dma_setting = NULL\n"));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	*dma_setting = *gDd_Audio_Dma_Out[ch];
	
	return D_DDIM_OK;
}

/**
 * @brief  Start Audio Input
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Start_Input( UINT8 ch )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if (((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)) &&
		 (ch != D_DD_AUDIO_IF_CH0_CH1)									   &&
		 (ch != D_DD_AUDIO_IF_CH4_CH5)										){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Start_Input:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	// Clear OverFlow Flag
	if (ch == D_DD_AUDIO_IF_CH0_CH1){
		// CH0
		ret = dd_audio_clear_in_overflow_flag(D_DD_AUDIO_IF_CH0);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_clear_in_overflow_flag() = %x\n", ret));
		}
		// CH1
		ret = dd_audio_clear_in_overflow_flag(D_DD_AUDIO_IF_CH1);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_clear_in_overflow_flag() = %x\n", ret));
		}
	}
	else if (ch == D_DD_AUDIO_IF_CH4_CH5){
		// CH0
		ret = dd_audio_clear_in_overflow_flag(D_DD_AUDIO_IF_CH4);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_clear_in_overflow_flag() = %x\n", ret));
		}
		// CH1
		ret = dd_audio_clear_in_overflow_flag(D_DD_AUDIO_IF_CH5);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_clear_in_overflow_flag() = %x\n", ret));
		}
	}
	else {
		ret = dd_audio_clear_in_overflow_flag(ch);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_clear_in_overflow_flag() = %x\n", ret));
		}
	}
	
	ret = dd_audio_set_enable_audio_in(ch, D_DD_AUDIO_ENABLE);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_set_enable_audio_in() = %x\n", ret));
		return ret;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Stop Audio Input
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32	Dd_Audio_Stop_Input( UINT8 ch )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if (((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)) &&
		 (ch != D_DD_AUDIO_IF_CH0_CH1)									   &&
		 (ch != D_DD_AUDIO_IF_CH4_CH5)										){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Stop_Input:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (ch == D_DD_AUDIO_IF_CH0_CH1){
		// Disable Interrupt
		ret = dd_audio_set_enable_in_overflow_intr(D_DD_AUDIO_IF_CH0, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_set_enable_in_overflow_intr() = %x\n", ret));
		}
		
		ret = dd_audio_set_enable_in_overflow_intr(D_DD_AUDIO_IF_CH1, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_set_enable_in_overflow_intr() = %x\n", ret));
		}
		
		// CH0
		ret = dd_audio_disable_input_intr(D_DD_AUDIO_IF_CH0);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_disable_input_intr() = %x\n", ret));
		}
		
		// CH1
		ret = dd_audio_disable_input_intr(D_DD_AUDIO_IF_CH1);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_disable_input_intr() = %x\n", ret));
		}
		
		//stop input
		ret = dd_audio_set_enable_audio_in(ch, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Stop_Input:dd_audio_set_enable_audio_in() = %x\n", ret));
			return ret;
		}
		
		Dd_ARM_Dsb_Pou();
		
		// CH0
		dd_audio_wait_empty_input_fifo(D_DD_AUDIO_IF_CH0);
		
		// CH1
		dd_audio_wait_empty_input_fifo(D_DD_AUDIO_IF_CH1);
	}
	else if (ch == D_DD_AUDIO_IF_CH4_CH5){
		// Disable Interrupt
		ret = dd_audio_set_enable_in_overflow_intr(D_DD_AUDIO_IF_CH4, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_set_enable_in_overflow_intr() = %x\n", ret));
		}
		
		ret = dd_audio_set_enable_in_overflow_intr(D_DD_AUDIO_IF_CH5, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_set_enable_in_overflow_intr() = %x\n", ret));
		}
		
		// CH0
		ret = dd_audio_disable_input_intr(D_DD_AUDIO_IF_CH4);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_disable_input_intr() = %x\n", ret));
		}
		
		// CH1
		ret = dd_audio_disable_input_intr(D_DD_AUDIO_IF_CH5);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_disable_input_intr() = %x\n", ret));
		}
		
		//stop input
		ret = dd_audio_set_enable_audio_in(ch, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Stop_Input:dd_audio_set_enable_audio_in() = %x\n", ret));
			return ret;
		}
		
		Dd_ARM_Dsb_Pou();
		
		// CH0
		dd_audio_wait_empty_input_fifo(D_DD_AUDIO_IF_CH4);
		
		// CH1
		dd_audio_wait_empty_input_fifo(D_DD_AUDIO_IF_CH5);
	}
	else {
		// Disable Interrupt
		ret = dd_audio_set_enable_in_overflow_intr(ch, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_set_enable_in_overflow_intr() = %x\n", ret));
		}
		
		ret = dd_audio_disable_input_intr(ch);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Input:dd_audio_disable_input_intr() = %x\n", ret));
		}
		
		//stop input
		ret = dd_audio_set_enable_audio_in(ch, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Stop_Input:dd_audio_set_enable_audio_in() = %x\n", ret));
			return ret;
		}
		
		Dd_ARM_Dsb_Pou();
		
		dd_audio_wait_empty_input_fifo(ch);
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Get Status of Input Operation
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
BOOL Dd_Audio_Get_Status_Input( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Status_Input:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_get_audio_in_enable_flag(ch);
}

/**
 * @brief  Start Audio Output
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Start_Output( UINT8 ch )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX) && (ch != D_DD_AUDIO_IF_CH0_CH1)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Start_Output:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (ch == D_DD_AUDIO_IF_CH0_CH1){
		// CH0
		ret = dd_audio_clear_out_underflow_flag(D_DD_AUDIO_IF_CH0);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Output:dd_audio_clear_out_underflow_flag() = %x\n", ret));
		}
		
		// CH1
		ret = dd_audio_clear_out_underflow_flag(D_DD_AUDIO_IF_CH1);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Output:dd_audio_clear_out_underflow_flag() = %x\n", ret));
		}
		
		// CH0
		dd_audio_wait_store_output_fifo(D_DD_AUDIO_IF_CH0);
		
		// CH1
		dd_audio_wait_store_output_fifo(D_DD_AUDIO_IF_CH1);
	}
	else {
		ret = dd_audio_clear_out_underflow_flag(ch);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Output:dd_audio_clear_out_underflow_flag() = %x\n", ret));
		}
		
		dd_audio_wait_store_output_fifo(ch);
	}
	
	ret = dd_audio_set_enable_audio_out(ch, D_DD_AUDIO_ENABLE);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_Output:dd_audio_set_enable_audio_out() = %x\n", ret));
		return ret;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Stop Audio Ouput
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR/D_DD_AUDIO_SYSTEM_ERROR
 */
INT32 Dd_Audio_Stop_Output( UINT8 ch )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX) && (ch != D_DD_AUDIO_IF_CH0_CH1)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Stop_Output:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (ch == D_DD_AUDIO_IF_CH0_CH1){
		// Disable Interrupt
		// CH0
		ret = dd_audio_set_enable_out_underflow_intr(D_DD_AUDIO_IF_CH0, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Stop_Output:dd_audio_set_enable_out_underflow_intr = %x\n", ret));
		}
		
		// CH1
		ret = dd_audio_set_enable_out_underflow_intr(D_DD_AUDIO_IF_CH1, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Stop_Output:dd_audio_set_enable_out_underflow_intr = %x\n", ret));
		}
		
		// CH0
		ret = dd_audio_disable_output_intr(D_DD_AUDIO_IF_CH0);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Stop_Output:Set INTOE/EINTOE = %x\n", ret));
		}
		
		// CH1
		ret = dd_audio_disable_output_intr(D_DD_AUDIO_IF_CH1);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Stop_Output:Set INTOE/EINTOE = %x\n", ret));
		}
		
		//stop output
		ret = dd_audio_set_enable_audio_out(ch, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Stop_Output:dd_audio_set_enable_audio_out() = %x\n", ret));
			return ret;
		}
		
		dd_audio_wait_empty_output_fifo(D_DD_AUDIO_IF_CH0);
		dd_audio_wait_empty_output_fifo(D_DD_AUDIO_IF_CH1);
	}
	else {
		// Disable Interrupt
		ret = dd_audio_set_enable_out_underflow_intr(ch, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Stop_Output:dd_audio_set_enable_out_underflow_intr = %x\n", ret));
		}
		
		ret = dd_audio_disable_output_intr(ch);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Stop_Output:Set INTOE/EINTOE = %x\n", ret));
		}
		
		//stop output
		ret = dd_audio_set_enable_audio_out(ch, D_DD_AUDIO_DISABLE);
		if (ret != D_DDIM_OK){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Stop_Output:dd_audio_set_enable_audio_out() = %x\n", ret));
			return ret;
		}
		
		dd_audio_wait_empty_output_fifo(ch);
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  Get Status of Output Operation
 * @param  UINT8 ch
 * @return TRUE/FALSE
 */
BOOL Dd_Audio_Get_Status_Output( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Status_Output:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_get_audio_out_enable_flag(ch);
}

/**
 * @brief  Start LoopBack Operation.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Start_LoopBack( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_LB_CH_NUM_MAX) && (ch != D_DD_AUDIO_IF_CH0_CH1)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Start_LoopBack:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	if (ch == D_DD_AUDIO_IF_CH0_CH1){
		if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH0) == FALSE){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_LoopBack:input operation is not performed \n"));
			return D_DD_AUDIO_SYSTEM_ERROR;
		}
		
		if (dd_audio_get_audio_in_enable_flag(D_DD_AUDIO_IF_CH1) == FALSE){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_LoopBack:input operation is not performed \n"));
			return D_DD_AUDIO_SYSTEM_ERROR;
		}
	}
	else {
		if (dd_audio_get_audio_in_enable_flag(ch) == FALSE){
			Ddim_Print(("[DD_AUDIO]Dd_Audio_Start_LoopBack:input operation is not performed \n"));
			return D_DD_AUDIO_SYSTEM_ERROR;
		}
	}
	
	return dd_audio_set_enable_loopback(ch, D_DD_AUDIO_ENABLE);
}

/**
 * @brief  Stop LoopBack Operation.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Stop_LoopBack( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_LB_CH_NUM_MAX) && (ch != D_DD_AUDIO_IF_CH0_CH1)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Stop_LoopBack:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_set_enable_loopback(ch, D_DD_AUDIO_DISABLE); 
}

/**
 * @brief  Get Status of LoopBack Operation.
 * @param  UINT8 ch
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
BOOL Dd_Audio_Get_Status_LoopBack( UINT8 ch )
{
#ifdef CO_PARAM_CHECKa
	if (ch >= D_DD_AUDIO_IF_LB_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Status_LoopBack:input channel error : %d\n", ch));
		return FALSE;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_get_loopback_flag(ch);
}

/**
 * @brief  Set register DMIE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_EnableFifoFullDmaIn( UINT8 ch, UINT8 enable )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_EnableFifoFullDmaIn:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_EnableFifoFullDmaIn:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_set_enable_fifo_full_dma_in(ch, enable);
}

/**
 * @brief  Set register DMOE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_EnableFifoEmptyDmaOut( UINT8 ch, UINT8 enable )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_EnableFifoEmptyDmaOut:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_EnableFifoEmptyDmaOut:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_set_enable_fifo_empty_dma_out(ch, enable);
}

/**
 * @brief  Set register EDMIE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_InputDMARequestEnable( UINT8 ch, UINT8 enable )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_InputDMARequestEnable:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_InputDMARequestEnable:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_set_input_dma_request_enable(ch, enable);
}

/**
 * @brief  Set register EDMOE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_OutputDMARequestEnable( UINT8 ch, UINT8 enable )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_OutputDMARequestEnable:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_OutputDMARequestEnable:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_set_output_dma_request_enable(ch, enable);
}


/**** DMA Transfer Mode ***/

/**
 * @brief  Get register AUDMISAMPLE.
 * @param  UINT8 ch
 * @return UINT16 value of register AUDMISAMPLE.DMISPL
 */
UINT16 Dd_Audio_Get_InDmaSample( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_InDmaSample:input channel error : %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	
	return IO_AUDIO.AUDIOIF[ch].AUDMISAMPLE.bit.DMISPL;
}

/**
 * @brief  Get register AUDMOSAMPLE.
 * @param  UINT8 ch
 * @return UINT16 value of register AUDMOSAMPLE
 */
UINT16 Dd_Audio_Get_OutDmaSample( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_OutDmaSample:input channel error : %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	
	return IO_AUDIO.AUDIOIF[ch].AUDMOSAMPLE.bit.DMOSPL;
}

/**
 * @brief  Swap a reading value from AUIDLR by 1Byte unit.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_SwapByte_AUIDLR( UINT8 ch, UINT8 enable )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_SwapByte_AUIDLR:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_set_input_byte_swap(ch, enable);
}

/**
 * @brief  Swap a reading value from AUIDLR by 2Byte unit.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_SwapHW_AUIDLR( UINT8 ch, UINT8 enable )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_SwapHW_AUIDLR:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_set_input_hw_swap(ch, enable);
}

/**
 * @brief  Swap a writing value to AUIDLR by 1Byte unit.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_SwapByte_AUODLR( UINT8 ch, UINT8 enable )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_SwapByte_AUODLR:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_set_output_byte_swap(ch, enable);
}

/**
 * @brief  Swap a writing value to AUODLR by 2Byte unit.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_SwapHW_AUODLR( UINT8 ch, UINT8 enable )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_SwapHW_AUODLR:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	return dd_audio_set_output_hw_swap(ch, enable);
}

/**** Get Adress of Data Register ****/

/**
 * @brief  Get address of register AUIDLR.
 * @param  UINT8 ch
 * @return UINT32 address of register AUIDLR 
 */
UINT32 Dd_Audio_Get_Addr_Reg_AUIDLR( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Addr_Reg_AUIDLR:input channel error: %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	
	return (UINT32)&IO_AUDIO.AUDIOIF[ch].AUIDLR;
}

/**
 * @brief  Get address of register AUODLR.
 * @param  UINT8 ch
 * @return UINT32 address of register AUODLR 
 */
UINT32 Dd_Audio_Get_Addr_Reg_AUODLR( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Addr_Reg_AUODLR:input channel error: %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	
	return (UINT32)&IO_AUDIO.AUDIOIF[ch].AUODLR;
}

/**
 * @brief  Get address of register AUIDL.
 * @param  UINT8 ch
 * @return UINT32 address of register AUIDL 
 */
UINT32 Dd_Audio_Get_Addr_Reg_AUIDL( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Addr_Reg_AUIDL:input channel error: %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	return (UINT32)&IO_AUDIO.AUDIOIF[ch].AUIDL;
}

/**
 * @brief  Get address of register AUIDR.
 * @param  UINT8 ch
 * @return UINT32 address of register AUIDR 
 */
UINT32 Dd_Audio_Get_Addr_Reg_AUIDR( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Addr_Reg_AUIDR:input channel error: %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	return (UINT32)&IO_AUDIO.AUDIOIF[ch].AUIDR;
}

/**
 * @brief  Get address of register AUODL.
 * @param  UINT8 ch
 * @return UINT32 address of register AUODL 
 */
UINT32 Dd_Audio_Get_Addr_Reg_AUODL( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Addr_Reg_AUODL:input channel error: %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	return (UINT32)&IO_AUDIO.AUDIOIF[ch].AUODL;
}

/**
 * @brief  Get address of register AUODR.
 * @param  UINT8 ch
 * @return UINT32 address of register AUODR 
 */
UINT32 Dd_Audio_Get_Addr_Reg_AUODR( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Addr_Reg_AUODR:input channel error: %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	return (UINT32)&IO_AUDIO.AUDIOIF[ch].AUODR;
}

/**
 * @brief  Get address of register AUIDDMAPT.
 * @param  UINT8 ch
 * @return UINT32 address of register AUIDDMAPT
 */
UINT32 Dd_Audio_Get_Addr_Reg_AUIDDMAPT( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Addr_Reg_AUIDDMAPT:input channel error: %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	return (UINT32)&IO_AUDIO.AUDIOIF[ch].AUIDDMAPT;
}

/**
 * @brief  Get address of register AUODDMAPT.
 * @param  UINT8 ch
 * @return UINT32 address of register AUODDMAPT
 */
UINT32 Dd_Audio_Get_Addr_Reg_AUODDMAPT( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_Addr_Reg_AUODDMAPT:input channel error: %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	return (UINT32)&IO_AUDIO.AUDIOIF[ch].AUODDMAPT;
}

/**
 * @brief  Set register INTIE/EINTIE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @param  T_DD_AUDIO_CALLBACK callback
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_EnableInputIntr( UINT8 ch, UINT8 enable, T_DD_AUDIO_CALLBACK callback )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_EnableInputIntr:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_EnableInputIntr:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if ((gDd_Audio_Ctrl_Cmmn[ch]->fifo_usage == E_DD_AUDIO_FIFO_USAGE_STAGES_16)	||
		(gDd_Audio_Ctrl_In[ch]->fifo_stages == E_DD_AUDIO_FIFO_STAGES_1)			){
		
		ret = dd_audio_set_enable_in_full_intr(ch, enable);
	}
	else {
		ret = dd_audio_set_enable_in_usage_intr(ch, enable);
	}
	
	if ((ret == D_DDIM_OK) && (enable == D_DD_AUDIO_ENABLE)){
		gDd_Audio_Input_Callback_Func[ch] = callback;
	}
	else {
		gDd_Audio_Input_Callback_Func[ch] = NULL;
	}
	
	return ret;
}

/**
 * @brief  Set register INTOE/EINTOE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_EnableOutputIntr( UINT8 ch, UINT8 enable, T_DD_AUDIO_CALLBACK callback )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_EnableOutputIntr:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_EnableOutputIntr:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if ((gDd_Audio_Ctrl_Cmmn[ch]->fifo_usage == E_DD_AUDIO_FIFO_USAGE_STAGES_16)	||
		(gDd_Audio_Ctrl_Out[ch]->fifo_stages == E_DD_AUDIO_FIFO_STAGES_1)			){
		
		ret = dd_audio_set_enable_out_empty_intr(ch, enable);
	}
	else {
		ret = dd_audio_set_enable_out_usage_intr(ch, enable);
	}
	
	if ((ret == D_DDIM_OK) && (enable == D_DD_AUDIO_ENABLE)){
		gDd_Audio_Output_Callback_Func[ch] = callback;
	}
	else {
		gDd_Audio_Output_Callback_Func[ch] = NULL;
	}
	
	return ret;
}

/**
 * @brief  Set register OFIE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_EnableInOverFlowIntr( UINT8 ch, UINT8 enable, T_DD_AUDIO_CALLBACK callback )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_EnableInOverFlowIntr:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_EnableInOverFlowIntr:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	ret = dd_audio_clear_in_overflow_flag(ch);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Set_EnableInOverFlowIntr:dd_audio_clear_in_overflow_flag() = %x\n", ret));
	}
	
	ret = dd_audio_set_enable_in_overflow_intr(ch, enable);
	
	if ((ret == D_DDIM_OK) && (enable == D_DD_AUDIO_ENABLE)){
		gDd_Audio_OverFlow_Callback_Func[ch] = callback;
	}
	else {
		gDd_Audio_OverFlow_Callback_Func[ch] = NULL;
	}
	
	return ret;
}

/**
 * @brief  Set register UFIE.
 * @param  UINT8 ch
 * @param  UINT8 enable
 * @return INT32 D_DDIM_OK/D_DD_AUDIO_INPUT_PARAM_ERROR
 */
INT32 Dd_Audio_Set_EnableOutUnderFlowIntr( UINT8 ch, UINT8 enable, T_DD_AUDIO_CALLBACK callback )
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Set_EnableOutUnderFlowIntr:input channel error : %d\n", ch));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
	
	if (enable > 1){
		Ddim_Assertion(("[DD_AUDIO]DDd_Audio_Set_EnableOutUnderFlowIntr:input enable error : %d\n", enable));
		return D_DD_AUDIO_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
	ret = dd_audio_clear_out_underflow_flag(ch);
	if (ret != D_DDIM_OK){
		Ddim_Print(("[DD_AUDIO]Dd_Audio_Set_EnableOutUnderFlowIntr:dd_audio_clear_out_underflow_flag() = %x\n", ret));
	}
	
	ret = dd_audio_set_enable_out_underflow_intr(ch, enable);
	
	if ((ret == D_DDIM_OK) && (enable == D_DD_AUDIO_ENABLE)){
		gDd_Audio_UnderFlow_Callback_Func[ch] = callback;
	}
	else {
		gDd_Audio_UnderFlow_Callback_Func[ch] = NULL;
	}
	
	return ret;
}

/**
 * @brief  Get register AUIFST.
 * @param  UINT8 ch
 * @return UINT32 value of register AUIFST
 */
UINT32 Dd_Audio_Get_InputFifoStatus( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if ((ch >= D_DD_AUDIO_IF_IN_CH_NUM_MAX) || (ch == D_DD_AUDIO_IF_CH3)){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_InputFifoStatus:input channel error : %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	
	return IO_AUDIO.AUDIOIF[ch].AUIFST.word;
}

/**
 * @brief  Get register AUOFST.
 * @param  UINT8 ch
 * @return UINT32 value of register AUOFST
 */
UINT32 Dd_Audio_Get_OutputFifoStatus( UINT8 ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_AUDIO_IF_OUT_CH_NUM_MAX){
		Ddim_Assertion(("[DD_AUDIO]Dd_Audio_Get_OutputFifoStatus:input channel error : %d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	
	return IO_AUDIO.AUDIOIF[ch].AUOFST.word;
}


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

