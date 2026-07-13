/**
 * @file		audio_out.c
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "ddim_typedef.h"
#include "TKernel_id.h"
#include "fj_audio.h"
#include "fj_encoder.h"
#include "fj_decoder.h"
#include "fj_host.h"
#include "audio.h"
#include "audio_out.h"
#include "audio_capture.h"
#include "audio_auif.h"
#include "audio_dma.h"
#include "audio_buzzer.h"
#include "audio_data.h"
#include "audio_decoder.h"
#include "dd_arm.h"
#include "sdram_map_define.h"
#include "sdram_map_movie_play_define.h"
#include "sdram_map_movie_record_pattern0_define.h"
#include "os_user_custom.h"
#include "debug.h"
#include "mba_if.h"
#include "disp.h"		// for HDMI Audio Setting
#include "fj_memory.h"
#include "wav_codec.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/

#define D_AUDIO_OUT_CH_0		(0)
#define D_AUDIO_OUT_CH_1		(1)

#define D_AUDIO_OUT_ON			(1)
#define D_AUDIO_OUT_OFF			(0)

#define D_AUDIO_OUT_WAIT_END	(2)

#define D_AUDIO_OUT_INVALID		(0xFFU)

#define D_AUDIO_OUT_NUM_SAMPLE			(1024U)

#define D_AUDIO_OUT_BUFF_ARRAY			(48U)

// Primary
#define D_AUDIO_OUT_IN_DATA_BUFF0_ADDR	(SDRAM_ADR_AUDIO_OUT_AREA)
#define D_AUDIO_OUT_IN_DATA_BUFF0_SIZE	(D_AUDIO_OUT_NUM_SAMPLE * 4 * 2 * 12)
#define D_AUDIO_OUT_PLUGIN_BUFF0_ADDR	(D_AUDIO_OUT_IN_DATA_BUFF0_ADDR + D_AUDIO_OUT_IN_DATA_BUFF0_SIZE)
#define D_AUDIO_OUT_PLUGIN_BUFF0_SIZE	(D_AUDIO_OUT_NUM_SAMPLE * 4 * 2 * 12)

// Secondary
#define D_AUDIO_OUT_IN_DATA_BUFF1_ADDR	(D_AUDIO_OUT_PLUGIN_BUFF0_ADDR + D_AUDIO_OUT_PLUGIN_BUFF0_SIZE)
#define D_AUDIO_OUT_IN_DATA_BUFF1_SIZE	(D_AUDIO_OUT_NUM_SAMPLE * 4 * 2 * 12)
#define D_AUDIO_OUT_PLUGIN_BUFF1_ADDR	(D_AUDIO_OUT_IN_DATA_BUFF1_ADDR + D_AUDIO_OUT_IN_DATA_BUFF1_SIZE)
#define D_AUDIO_OUT_PLUGIN_BUFF1_SIZE	(D_AUDIO_OUT_NUM_SAMPLE * 4 * 2 * 12)

#define D_AUDIO_OUT_NUM_AUDIO_PROCESS		(10U)

#define D_AUDIO_OUT_DMA_ID0					(0)
#define D_AUDIO_OUT_DMA_ID1					(1)

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
typedef struct {
	UINT32					addr;			// Input Sampling Rate
	UINT32					size;			// Output Sampling Rate
} T_AUDIO_OUT_BUFF;


typedef struct{
	UINT8	in_pos;		/**< data int postion */
	UINT8	out_pos;	/**< data out postion */
	UINT8	count;		/**< not read data remain */
	UINT8	data_num;	/**< Number of frame */
}T_AUDIO_OUT_BUFFER_INFO;

typedef struct{
	UINT32	in_addr;	/**< address of input data */
	UINT32	out_addr;	/**< address of output data */
	UINT32	size;		/**< input data size */
	UINT8	data_ch;	/**< channel of audio data */
}T_AUDIO_OUT_CNV_DATA_INFO;

typedef VOID (*T_AUDIO_OUT_DMA_CALLBACK)( UCHAR dma_status );

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
static T_AUDIO_OUT_BUFF gBF_AudioOut_Input_Buff[D_AUDIO_OUT_INST_NUM][D_AUDIO_OUT_BUFF_ARRAY];
static T_AUDIO_OUT_BUFF gBF_AudioOut_PI_Buff[D_AUDIO_OUT_INST_NUM][D_AUDIO_OUT_BUFF_ARRAY];
static T_AUDIO_OUT_BUFFER_INFO gBF_AudioOut_Input_Buff_Info[D_AUDIO_OUT_INST_NUM];
static T_AUDIO_OUT_BUFFER_INFO gBF_AudioOut_PI_Buff_Info[D_AUDIO_OUT_INST_NUM];

static T_AUDIO_OUT_BUFF gBF_AudioOut_Cap_Input_Buff[D_AUDIO_OUT_INST_NUM][D_AUDIO_OUT_BUFF_ARRAY];
static T_AUDIO_OUT_BUFFER_INFO gBF_AudioOut_Cap_PI_Buff_Info[D_AUDIO_OUT_INST_NUM] ;

static T_AUDIO_OUT_INSTANCE gBF_Audio_Out_Inst[D_AUDIO_OUT_INST_NUM] =
{
	{
		D_AUDIO_OUT_STATUS_STOP,		// status
		FJ_AUDIO_OUT_ID_0,				// audio_out_id
		FJ_AUDIO_DISABLE,				// enable
		E_FJ_AUDIO_OUT_0,				// type
		D_AUDIO_OUT_OFF,				// buzzer_flg
		D_AUDIO_OUT_OFF,				// cap_flg
		D_AUDIO_OUT_OFF,				// dec_flg
		E_FJ_AUDIO_WAV_CTL_NONE,		// bz_mode
		D_AUDIO_OUT_INVALID,			// connect_cap_id
		D_AUDIO_OUT_INVALID,			// connect_dec_id
		D_AUDIO_OFF,					// set_cfg_flg
		E_FJ_AUDIO_OUT_PORT_I2S0,		// out_port
		{								// i2s_cfg
			FJ_AUDIO_IF_MASTER,					// master_slave
			D_AUDIO_SAMPLE_RATE_48000,			// lr_clock
			D_AUDIO_I2S_CYCLE_32FS,				// data_cycle
			E_FJ_AUDIO_I2S_Data_FORMAT_LEFT,	// i2s_format
			D_AUDIO_PCM_16_BIT					// bit_count
		},
		D_AUDIO_SAMPLE_RATE_48000,		// in_smpl_rate
		D_AUDIO_SAMPLE_RATE_48000,		// out_smpl_rate
		FJ_AUDIO_NUM_CHANNEL_2,			// in_ch_type
		FJ_AUDIO_NUM_CHANNEL_2,			// out_ch_type
		{									// dma_id[]
			D_AUDIO_OUT_INVALID,			// dma_id
			D_AUDIO_OUT_INVALID				// dma_id_2nd
		},
		0								// output_count
	},
	{
		D_AUDIO_OUT_STATUS_STOP,		// status
		FJ_AUDIO_OUT_ID_1,				// audio_out_id
		FJ_AUDIO_DISABLE,				// enable
		E_FJ_AUDIO_OUT_1,				// type
		D_AUDIO_OUT_OFF,				// buzzer_flg
		D_AUDIO_OUT_OFF,				// cap_flg
		D_AUDIO_OUT_OFF,				// dec_flg
		E_FJ_AUDIO_WAV_CTL_NONE,		// bz_mode
		D_AUDIO_OUT_INVALID,			// connect_cap_id
		D_AUDIO_OUT_INVALID,			// connect_dec_id
		D_AUDIO_OFF,					// set_cfg_flg
		E_FJ_AUDIO_OUT_PORT_I2S2,		// out_port
		{								// i2s_cfg
			FJ_AUDIO_IF_MASTER,					// master_slave
			D_AUDIO_SAMPLE_RATE_48000,			// lr_clock
			D_AUDIO_I2S_CYCLE_32FS,				// data_cycle
			E_FJ_AUDIO_I2S_Data_FORMAT_LEFT,	// i2s_format
			D_AUDIO_PCM_16_BIT					// bit_count
		},
		D_AUDIO_SAMPLE_RATE_48000,		// in_smpl_rate
		D_AUDIO_SAMPLE_RATE_48000,		// out_smpl_rate
		FJ_AUDIO_NUM_CHANNEL_2,			// in_ch_type
		FJ_AUDIO_NUM_CHANNEL_2,			// out_ch_type
		{									// dma_id[]
			D_AUDIO_OUT_INVALID,			// dma_id
			D_AUDIO_OUT_INVALID				// dma_id_2nd
		},
		0								// output_count
	}
};

static UINT8 gBF_AudioOut_StopProcessing_Id[D_AUDIO_OUT_INST_NUM] = {D_AUDIO_OUT_INVALID, D_AUDIO_OUT_INVALID};

static UINT8 gBF_AudioOut_Buzzer_End_Flg[D_AUDIO_OUT_INST_NUM];
static UINT8 gBF_AudioOut_Buzzer_StopReq[D_AUDIO_OUT_INST_NUM];			// for Host CallBack
static UINT8 gBF_AudioOut_Buzzer_StopFactor[D_AUDIO_OUT_INST_NUM];		// for Host CallBack

static UINT8 gBF_AudioOut_Dec_End_Flg[D_AUDIO_OUT_INST_NUM];
static UINT8 gBF_AudioOut_Dec_StopFlg[D_AUDIO_OUT_INST_NUM];

static UINT8 gBF_AudioOut_MixWork_Bz[D_AUDIO_OUT_NUM_SAMPLE * 4 * 4] __attribute__((aligned(64))) = {0};
static UINT32 gBF_AudioOut_Work[D_AUDIO_OUT_NUM_SAMPLE * 4] __attribute__((aligned(64)));
static UINT8 gBF_AudioOut_Bz_Cnv_Work[D_AUDIO_OUT_NUM_SAMPLE * 4 * 4] __attribute__((aligned(64)));

static volatile UINT8 gBF_AudioOut_TestMode[D_AUDIO_OUT_INST_NUM] = {0};				// 0:Normal, 1:After PlugIn, 2:Before PlugIn
static volatile UINT32 gBF_AudioOut_TestMode_StartAddr[D_AUDIO_OUT_INST_NUM] = {0};
static volatile UINT32 gBF_AudioOut_TestMode_CurrentAddr[D_AUDIO_OUT_INST_NUM] = {0};
static volatile UINT32 gBF_AudioOut_TestMode_Size[D_AUDIO_OUT_INST_NUM] = {0};

// Critical Section
// for gBF_AudioOut_Input_Buff_Info[].count
static ULONG gBF_AudioOut_CS_BzIb_Cnt __attribute__((section(".LOCK_SECTION"), aligned(64)));

// for gBF_AudioOut_PI_Buff_Info[].count
static ULONG gBF_AudioOut_CS_BzPi_Cnt __attribute__((section(".LOCK_SECTION"), aligned(64)));

// for gBF_AudioOut_Cap_PI_Buff_Info[].count
static ULONG gBF_AudioOut_CS_CapPi_Cnt __attribute__((section(".LOCK_SECTION"), aligned(64)));

// for gBF_Audio_Out_Inst[].buzzer_flg
static ULONG gBF_AudioOut_CS_Bz_Flg __attribute__((section(".LOCK_SECTION"), aligned(64)));

// for gBF_AudioOut_Buzzer_End_Flg[]
static ULONG gBF_AudioOut_CS_Bz_EndFlg __attribute__((section(".LOCK_SECTION"), aligned(64)));


// EventFlag Table
static const OS_USER_FLGPTN gBF_AudioOut_EventFlag_BzStopNtfy_Tbl[D_AUDIO_OUT_INST_NUM] = {
	FLG_AUDIO_OUT_BZ_STOP_NTFY0,
	FLG_AUDIO_OUT_BZ_STOP_NTFY1
};

static const OS_USER_FLGPTN gBF_AudioOut_EventFlag_PcmSnd_Tbl[D_AUDIO_OUT_INST_NUM] = {
	FLG_AUDIO_OUT_OUT_PCM_SND0,
	FLG_AUDIO_OUT_OUT_PCM_SND1
};

static const OS_USER_FLGPTN gBF_AudioOut_EventFlag_PcmSnd2nd_Tbl[D_AUDIO_OUT_INST_NUM] = {
	FLG_AUDIO_OUT_OUT_PCM_SND0_2ND,
	FLG_AUDIO_OUT_OUT_PCM_SND1_2ND
};


/*----------------------------------------------------------------------*/
/* Local Function Prototype Definition                                  */
/*----------------------------------------------------------------------*/
static INT32 bf_audio_out_wai_sem( VOID );
static INT32 bf_audio_out_sig_sem( VOID );
static INT32 bf_audio_out_clear_state_change_flg( VOID );
static VOID bf_audio_out_set_state_change_flg( VOID );
static VOID bf_audio_out_set_state_error_flg( VOID );
static INT32 bf_audio_out_state_change_wait( VOID );
static INT32 bf_audio_out_init_buffer( UINT8 out_ch );
static INT32 bf_audio_out_renov_inputbuff_in( UINT8 out_ch );
static INT32 bf_audio_out_renov_inputbuff_out( UINT8 out_ch );
static UINT32 bf_audio_out_get_inputbuff_in_addr( UINT8 out_ch );
static UINT32 bf_audio_out_get_inputbuff_in_size( UINT8 out_ch ) __attribute__((unused));
static UINT32 bf_audio_out_get_inputbuff_out_addr( UINT8 out_ch );
static UINT32 bf_audio_out_get_inputbuff_out_size( UINT8 out_ch ) __attribute__((unused));
static INT32 bf_audio_out_renov_pi_buff_in( UINT8 out_ch );
static INT32 bf_audio_out_renov_pi_buff_out( UINT8 out_ch );
static UINT32 bf_audio_out_get_pi_buff_in_addr( UINT8 out_ch );
static UINT32 bf_audio_out_get_pi_buff_in_size( UINT8 out_ch ) __attribute__((unused));
static UINT32 bf_audio_out_get_pi_buff_out_addr( UINT8 out_ch );
static UINT32 bf_audio_out_get_pi_buff_out_size( UINT8 out_ch );
static INT32 bf_audio_out_cap_init_buffer( UINT8 out_ch );
static INT32 bf_audio_out_cap_renov_inputbuff_in( UINT8 out_ch );
static INT32 bf_audio_out_cap_renov_inputbuff_out( UINT8 out_ch );
static UINT32 bf_audio_out_cap_get_inputbuff_addr( UINT8 out_ch, UCHAR cap_id );
static UINT32 bf_audio_out_cap_get_inputbuff_size( UINT8 out_ch, UCHAR cap_id );
static UINT8 bf_audio_out_get_i2s_dev_ch( UINT8 out_ch );
static INT32 bf_audio_out_chg_buzzer_format_bitcnt( UINT32 addr, UINT32* size, UINT8 out_ch );
static INT32 bf_audio_out_chg_buzzer_format_channel( UINT32 addr, UINT32* size, UINT8 out_ch );
static INT32 bf_audio_out_change_buzzer_format( UINT32 addr, UINT32* size, UINT8 out_ch );
static INT32 bf_audio_out_change_decoded_pcm_format( UINT32 addr, UINT32* size, UINT8 out_ch );
static UINT32 bf_audio_out_get_decoded_pcm_addr( UINT8 out_ch );
static INT32 bf_audio_out_output_start_cap_set_prm( INT32 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static INT32 bf_audio_out_output_start_cap_get_data( INT32 out_ch );
static INT32 bf_audio_out_output_start_dec_set_prm( INT32 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static INT32 bf_audio_out_output_start_dec_get_data( INT32 out_ch );
static INT32 bf_audio_out_output_start_bz_set_prm( INT32 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static INT32 bf_audio_out_output_start_bz_get_data( INT32 out_ch );
static VOID bf_audio_out_testmode_copy( UINT8 out_ch, UINT32 addr, UINT32 size );
static VOID bf_audio_out_testmode_stop( UINT8 out_ch );
static INT32 bf_audio_out_check_config_param( UCHAR out_id, const T_FJ_AUDIO_OUT_CFG* audio_out_config );
static INT32 bf_audio_out_plugin_cb_preproc( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static INT32 bf_audio_out_plugin_cb( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static VOID bf_audio_out_bz_stop_req_ntf( UINT8 out_ch );
static INT32 bf_audio_out_set_auif_data_format( UINT8 out_ch, UINT8 dev_ch );
static UINT32 bf_audio_out_get_auif_data_reg_addr( UINT8 out_ch, UINT8 dev_ch );
static INT32 bf_audio_out_setup_auif( UINT8 out_ch, UINT8 dev_ch );
static INT32 bf_audio_out_output_start_setup( UINT8 out_ch, UINT8 dev_ch );
static INT32 bf_audio_out_output_start_dma( UINT8 out_ch );
static INT32 bf_audio_out_output_start_dma_cap( UINT8 out_ch );
static INT32 bf_audio_out_output_start_dma_dec( UINT8 out_ch );
static INT32 bf_audio_out_output_start_dma_bz( UINT8 out_ch );
static INT32 bf_audio_out_conversion_output_data( UINT8 out_ch, T_AUDIO_OUT_CNV_DATA_INFO* info );
static INT32 bf_audio_out_get_out_ch( UCHAR out_id, UINT8* out_ch );
static VOID bf_audio_out_wait_intr_multi_ch( UINT8 out_ch );
static INT32 bf_audio_out_trns_pcm_rx_connect_mix( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static INT32 bf_audio_out_trns_pcm_rx_connect_replace( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static INT32 bf_audio_out_trns_pcm_rx_connect_none( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static INT32 bf_audio_out_trns_pcm_rx_connect_inst_cap( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static INT32 bf_audio_out_trns_pcm_rx_connect_inst_dec( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static INT32 bf_audio_out_trns_pcm_rx_connect_inst_bz( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static INT32 bf_audio_out_trns_pcm_rx_connect_inst( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param );
static VOID bf_audio_out_request_stop( UINT8 out_ch );
static VOID bf_audio_out_hdmi_config( T_FJ_AUDIO_OUT_CFG* audio_out_config );
static INT32 bf_audio_out_start_wait_stop_proc( UINT8 out_ch );
static VOID bf_audio_out_start_bz_mix( UINT8 out_ch );
static INT32 bf_audio_out_start_bz_only( UINT8 out_ch );
static INT32 bf_audio_out_set_eventflg_init( UINT8 out_ch );
static INT32 bf_audio_out_set_eventflg_start( UINT8 out_ch );
static INT32 bf_audio_out_check_inst_enable( UCHAR out_id, UINT8* out_ch );
static INT32 bf_audio_out_check_dma_available( UINT8 out_ch );
static VOID bf_audio_out_stop_check_bz_off( UINT8 out_ch, UINT8* bz_off_flg );
static BOOL bf_audio_out_stop_check( UINT8 out_ch );
static VOID bf_audio_out_intr0( UINT8 out_ch );
static VOID bf_audio_out_intr1( UINT8 out_ch );
static VOID bf_audio_out_intr0_2nd( UINT8 out_ch );
static VOID bf_audio_out_intr1_2nd( UINT8 out_ch );
static VOID bf_audio_out0_intr0( UCHAR dma_status );
static VOID bf_audio_out0_intr1( UCHAR dma_status );
static VOID bf_audio_out0_intr0_2nd( UCHAR dma_status );
static VOID bf_audio_out0_intr1_2nd( UCHAR dma_status );
static VOID bf_audio_out1_intr0( UCHAR dma_status );
static VOID bf_audio_out1_intr1( UCHAR dma_status );
static VOID bf_audio_out1_intr0_2nd( UCHAR dma_status );
static VOID bf_audio_out1_intr1_2nd( UCHAR dma_status );
static VOID bf_audio_out_output_init0( VOID );
static VOID bf_audio_out_output_init1( VOID );
static VOID bf_audio_out_output_start0( VOID );
static VOID bf_audio_out_output_start1( VOID );
static VOID bf_audio_out_output_stop0( VOID );
static VOID bf_audio_out_output_stop1( VOID );
static VOID bf_audio_out_output_pcm_send0( VOID );
static VOID bf_audio_out_output_pcm_send1( VOID );
static VOID bf_audio_out_bz_stop_notification0( VOID );
static VOID bf_audio_out_bz_stop_notification1( VOID );

/*----------------------------------------------------------------------*/
/* Function Table Definition                                            */
/*----------------------------------------------------------------------*/
static const T_AUDIO_TASK_PROCESS gBF_Audio_Out_Process[D_AUDIO_OUT_NUM_AUDIO_PROCESS] = {
	{	FLG_AUDIO_OUT_OUT0_INIT,		bf_audio_out_output_init0			},
	{	FLG_AUDIO_OUT_OUT1_INIT,		bf_audio_out_output_init1			},
	{	FLG_AUDIO_OUT_OUT0_START,		bf_audio_out_output_start0			},
	{	FLG_AUDIO_OUT_OUT1_START,		bf_audio_out_output_start1			},
	{	FLG_AUDIO_OUT_OUT0_STOP,		bf_audio_out_output_stop0			},
	{	FLG_AUDIO_OUT_OUT1_STOP,		bf_audio_out_output_stop1			},
	{	FLG_AUDIO_OUT_OUT_PCM_SND0,		bf_audio_out_output_pcm_send0		},
	{	FLG_AUDIO_OUT_OUT_PCM_SND1,		bf_audio_out_output_pcm_send1		},
	{	FLG_AUDIO_OUT_BZ_STOP_NTFY0,	bf_audio_out_bz_stop_notification0	},
	{	FLG_AUDIO_OUT_BZ_STOP_NTFY1,	bf_audio_out_bz_stop_notification1	},
};

// DMA Callback Table
static const T_AUDIO_OUT_DMA_CALLBACK gBf_AudioOut_Dma0_Callback[D_AUDIO_OUT_INST_NUM]	= {
	bf_audio_out0_intr0,
	bf_audio_out1_intr0
};

static const T_AUDIO_OUT_DMA_CALLBACK gBf_AudioOut_Dma1_Callback[D_AUDIO_OUT_INST_NUM]	= {
	bf_audio_out0_intr1,
	bf_audio_out1_intr1
};

static const T_AUDIO_OUT_DMA_CALLBACK gBf_AudioOut_Dma0_2nd_Callback[D_AUDIO_OUT_INST_NUM]	= {
	bf_audio_out0_intr0_2nd,
	bf_audio_out1_intr0_2nd
};

static const T_AUDIO_OUT_DMA_CALLBACK gBf_AudioOut_Dma1_2nd_Callback[D_AUDIO_OUT_INST_NUM]	= {
	bf_audio_out0_intr1_2nd,
	bf_audio_out1_intr1_2nd
};


/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
/**
 *  @brief		Wait on Semaphore
 *	@param		None
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_wai_sem( VOID )
{
	OS_USER_ER ercd;
	
	//TimeOut:66ms(22*3). To work in a period for 21.3ms(48kHz)
	ercd = OS_User_Twai_Sem(SID_AUDIO_OUT, 100);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]twai_sem error(%d)\n", ercd));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Signal Semaphore
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_sig_sem( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Sig_Sem(SID_AUDIO_OUT);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]sig_sem error(%d)\n", ercd));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *	Clear Evnet Flag of Process End
 *	@param		VOID
 *	@return		D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_clear_state_change_flg( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Clr_Flg( FID_AUDIO_OUT, ~(FLG_AUDIO_OUT_OUT_STATE_CHANGE | FLG_AUDIO_OUT_OUT_STATE_ERR) );
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("[AUDIO_OUT]clr_flg error(%d) L=%d\n", ercd, __LINE__));
		
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set State Channge Flag
 *	@param		None
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_set_state_change_flg( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Set_Flg( FID_AUDIO_OUT, FLG_AUDIO_OUT_OUT_STATE_CHANGE );
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) L=%d\n", ercd, __LINE__));
	}
}

/**
 *  @brief		Set State Error Flag
 *	@param		None
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_set_state_error_flg( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Set_Flg( FID_AUDIO_OUT, FLG_AUDIO_OUT_OUT_STATE_ERR );
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) L=%d\n", ercd, __LINE__));
	}
}

/**
 *	Wait Evnet Flag of Process End
 *	@param		VOID
 *	@return		D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_state_change_wait( VOID )
{
	INT32			retry;
	T_OS_USER_RTSK	rftsk;
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flgptn = 0;
	INT32 return_val = D_AUDIO_OK;
	T_OS_USER_RSEM pk_rsm ={0, 0};
	
	ercd = OS_User_Ref_Sem(SID_AUDIO_OUT, &pk_rsm);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("[AUDIO_OUT]ref_tsk error(%d) L=%d\n", ercd, __LINE__));
	}
	
	if (pk_rsm.semcnt == 0){
		(VOID)bf_audio_out_sig_sem();
	}
	
	retry = 50;		// wait max 5000ms( 100ms * 50count)
	while (retry){
		ercd = OS_User_Ref_Tsk( TID_AUDIO_OUT, &rftsk );
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]ref_tsk error(%d) L=%d\n", ercd, __LINE__));
			return D_AUDIO_ERR;
		}
		
		if (rftsk.tskstat == D_OS_USER_TTS_DMT){
			BF_Debug_Print_Error(("[AUDIO_OUT]ref_tsk TTS_DMT L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
		
		ercd = OS_User_Twai_Flg(FID_AUDIO_OUT, 
								(FLG_AUDIO_OUT_OUT_STATE_CHANGE | FLG_AUDIO_OUT_OUT_STATE_ERR),
								D_OS_USER_TWF_ORW,
								&flgptn,
								100 );
		
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]twai_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		else{
			break;
		}
		
		ercd = OS_User_Dly_Tsk(10);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_OUT]dly_tsk error(%d) L=%d\n", ercd, __LINE__));
		}
		
		retry--;
	}
	
	if ((flgptn & FLG_AUDIO_OUT_OUT_STATE_CHANGE) != 0){
		ercd = OS_User_Clr_Flg( FID_AUDIO_OUT, ~FLG_AUDIO_OUT_OUT_STATE_CHANGE );
		if( ercd != D_OS_USER_E_OK ){
			BF_Debug_Print_Error(("[AUDIO_OUT]clr_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		
		return_val = D_AUDIO_OK;
	}
	
	if ((flgptn & FLG_AUDIO_OUT_OUT_STATE_ERR) != 0){
		ercd = OS_User_Clr_Flg( FID_AUDIO_OUT, ~FLG_AUDIO_OUT_OUT_STATE_ERR );
		if( ercd != D_OS_USER_E_OK ){
			BF_Debug_Print_Error(("[AUDIO_OUT]clr_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		
		return_val = D_AUDIO_ERR;
	}
	
	if (pk_rsm.semcnt == 0){
		(VOID)bf_audio_out_wai_sem();
	}
	
	return return_val;
}

/**
 *	Initialize Buffer of Buzzer
 *	@param		UINT8	AudioOut Channel
 *	@return		INT32	D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_init_buffer( UINT8 out_ch )
{
	UINT8 loop;
	UINT32 in_addr;
	UINT32 in_size;
	UINT32 pl_addr;
	UINT32 pl_size;
	UCHAR byte_cnt;
	
	// Table Data
	static const UINT32 in_buff_area_addr_tbl[D_AUDIO_OUT_INST_NUM] = {
		D_AUDIO_OUT_IN_DATA_BUFF0_ADDR,
		D_AUDIO_OUT_IN_DATA_BUFF1_ADDR
	};
	
	static const UINT32 in_buff_area_size_tbl[D_AUDIO_OUT_INST_NUM] = {
		D_AUDIO_OUT_IN_DATA_BUFF0_SIZE,
		D_AUDIO_OUT_IN_DATA_BUFF1_SIZE
	};
	
	static const UINT32 plug_in_buff_area_addr_tbl[D_AUDIO_OUT_INST_NUM] = {
		D_AUDIO_OUT_PLUGIN_BUFF0_ADDR,
		D_AUDIO_OUT_PLUGIN_BUFF1_ADDR
	};
	
	static const UINT32 plug_in_buff_area_size_tbl[D_AUDIO_OUT_INST_NUM] = {
		D_AUDIO_OUT_PLUGIN_BUFF0_SIZE,
		D_AUDIO_OUT_PLUGIN_BUFF1_SIZE
	};
	
	if (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT){
		byte_cnt = 4;
	}
	else {
		byte_cnt = 2;
	}
	
	// Input Data Buffer Setting
	in_addr = in_buff_area_addr_tbl[out_ch];
	
	if (gBF_Audio_Out_Inst[out_ch].in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		in_size = D_AUDIO_OUT_NUM_SAMPLE * 4 * byte_cnt;
	}
	else {
		in_size = D_AUDIO_OUT_NUM_SAMPLE * gBF_Audio_Out_Inst[out_ch].in_ch_type * byte_cnt;
	}
	
	for (loop = 0; loop < D_AUDIO_OUT_BUFF_ARRAY; loop++){
		gBF_AudioOut_Input_Buff[out_ch][loop].addr = in_addr;
		
		if (gBF_Audio_Out_Inst[out_ch].in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
			gBF_AudioOut_Input_Buff[out_ch][loop].size = D_AUDIO_OUT_NUM_SAMPLE * 2 * byte_cnt;
		}
		else {
			gBF_AudioOut_Input_Buff[out_ch][loop].size = in_size;
		}
		
		in_addr += in_size;
		
		if (((in_addr + in_size - in_buff_area_addr_tbl[out_ch]) > in_buff_area_size_tbl[out_ch])	||
			(loop >= (D_AUDIO_OUT_BUFF_ARRAY - 2))													){
			
			gBF_AudioOut_Input_Buff[out_ch][loop + 1].addr = 0;
			gBF_AudioOut_Input_Buff[out_ch][loop + 1].size = 0;
			gBF_AudioOut_Input_Buff_Info[out_ch].data_num = loop + 1;
			break;
		}
	}
	
	// PlugIn0 Buffer Setting
	pl_addr = plug_in_buff_area_addr_tbl[out_ch];
	
	if (gBF_Audio_Out_Inst[out_ch].in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		pl_size = D_AUDIO_OUT_NUM_SAMPLE * 4 * byte_cnt;
	}
	else {
		pl_size = D_AUDIO_OUT_NUM_SAMPLE * gBF_Audio_Out_Inst[out_ch].in_ch_type * byte_cnt;
	}
	
	for (loop = 0; loop < D_AUDIO_OUT_BUFF_ARRAY; loop++){
		gBF_AudioOut_PI_Buff[out_ch][loop].addr = pl_addr;
		
		if (gBF_Audio_Out_Inst[out_ch].in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
			gBF_AudioOut_PI_Buff[out_ch][loop].size = D_AUDIO_OUT_NUM_SAMPLE * 2 * byte_cnt;
		}
		else {
			gBF_AudioOut_PI_Buff[out_ch][loop].size = pl_size;
		}
		
		pl_addr += pl_size;
		
		if (((pl_addr + pl_size - plug_in_buff_area_addr_tbl[out_ch]) > plug_in_buff_area_size_tbl[out_ch])	||
			(loop >= (D_AUDIO_OUT_BUFF_ARRAY - 2))															){
			gBF_AudioOut_PI_Buff[out_ch][loop + 1].addr = 0;
			gBF_AudioOut_PI_Buff[out_ch][loop + 1].size = 0;
			gBF_AudioOut_PI_Buff_Info[out_ch].data_num	= loop + 1;
			break;
		}
	}
	
	gBF_AudioOut_Input_Buff_Info[out_ch].in_pos	= 0;
	gBF_AudioOut_Input_Buff_Info[out_ch].out_pos	= 0;
	gBF_AudioOut_Input_Buff_Info[out_ch].count	= 0;
	gBF_AudioOut_PI_Buff_Info[out_ch].in_pos		= 0;
	gBF_AudioOut_PI_Buff_Info[out_ch].out_pos		= 0;
	gBF_AudioOut_PI_Buff_Info[out_ch].count		= 0;
	
	return D_AUDIO_OK;
}

/**
 *	AUDIO PCM Input Buffer Renovation
 *	@param		UINT8	AudioOut Channel
 *	@return		D_AUDIO_OK/ D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_renov_inputbuff_in( UINT8 out_ch )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioOut_CS_BzIb_Cnt );
	
	if (gBF_AudioOut_Input_Buff_Info[out_ch].count < (gBF_AudioOut_Input_Buff_Info[out_ch].data_num - 1)){
		gBF_AudioOut_Input_Buff_Info[out_ch].in_pos++;
		gBF_AudioOut_Input_Buff_Info[out_ch].in_pos %= gBF_AudioOut_Input_Buff_Info[out_ch].data_num;
		
		gBF_AudioOut_Input_Buff_Info[out_ch].count++;	// counter increment
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no empty area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioOut_CS_BzIb_Cnt );
	
	return retcd;
}

/**
 *	AUDIO PCM Output Buffer Renovation
 *	@param		UINT8	AudioOut Channel
 *	@return  D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_renov_inputbuff_out( UINT8 out_ch )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioOut_CS_BzIb_Cnt );
	
	if (gBF_AudioOut_Input_Buff_Info[out_ch].count > 0){
		gBF_AudioOut_Input_Buff_Info[out_ch].out_pos++;
		gBF_AudioOut_Input_Buff_Info[out_ch].out_pos %= gBF_AudioOut_Input_Buff_Info[out_ch].data_num;
		
		gBF_AudioOut_Input_Buff_Info[out_ch].count--;	// counter decrement
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no busy area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioOut_CS_BzIb_Cnt );
	
	return retcd;
}

/**
 *	Get AUDIO PCM input address
 *	@param		UINT8	AudioOut Channel
 *	@return		Address of Buzzer Buffer(Input Position)
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_out_get_inputbuff_in_addr( UINT8 out_ch )
{
	return gBF_AudioOut_Input_Buff[out_ch][gBF_AudioOut_Input_Buff_Info[out_ch].in_pos].addr;
}


/**
 * Get AUDIO PCM input address
 * @param   VOID
 * @return  VOID
 */
static UINT32 bf_audio_out_get_inputbuff_in_size( UINT8 out_ch )
{
	return gBF_AudioOut_Input_Buff[out_ch][gBF_AudioOut_Input_Buff_Info[out_ch].in_pos].size;
}

/**
 *	Get AUDIO PCM output address
 *	@param		UINT8	AudioOut Channel
 *	@return		Address of Buzzer Buffer(Output Position)
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_out_get_inputbuff_out_addr( UINT8 out_ch )
{
	return gBF_AudioOut_Input_Buff[out_ch][gBF_AudioOut_Input_Buff_Info[out_ch].out_pos].addr;
}

/**
 * Get AUDIO PCM output address
 * @param   VOID
 * @return  VOID
 */
static UINT32 bf_audio_out_get_inputbuff_out_size( UINT8 out_ch )
{
	return gBF_AudioOut_Input_Buff[out_ch][gBF_AudioOut_Input_Buff_Info[out_ch].out_pos].size;
}

/**
 *	AUDIO PCM Input Buffer Renovation
 *	@param		UINT8	AudioOut Channel
 *	@return		D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_renov_pi_buff_in( UINT8 out_ch )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioOut_CS_BzPi_Cnt );
	
	if (gBF_AudioOut_PI_Buff_Info[out_ch].count < (gBF_AudioOut_PI_Buff_Info[out_ch].data_num - 1)){
		gBF_AudioOut_PI_Buff_Info[out_ch].in_pos++;
		gBF_AudioOut_PI_Buff_Info[out_ch].in_pos %= gBF_AudioOut_PI_Buff_Info[out_ch].data_num;
		
		gBF_AudioOut_PI_Buff_Info[out_ch].count++;	// counter increment
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no empty area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioOut_CS_BzPi_Cnt );
	
	return retcd;
}

/**
 *	AUDIO PCM Output Buffer Renovation
 *	@param		UINT8	AudioOut Channel
 *	@return		D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_renov_pi_buff_out( UINT8 out_ch )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioOut_CS_BzPi_Cnt );
	
	if (gBF_AudioOut_PI_Buff_Info[out_ch].count > 0){
		gBF_AudioOut_PI_Buff_Info[out_ch].out_pos++;
		gBF_AudioOut_PI_Buff_Info[out_ch].out_pos %= gBF_AudioOut_PI_Buff_Info[out_ch].data_num;
		
		gBF_AudioOut_PI_Buff_Info[out_ch].count--;	// counter decrement
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no busy area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioOut_CS_BzPi_Cnt );
	
	return retcd;
}

/**
 *	Get AUDIO PCM input address
 *	@param		UINT8	AudioOut Channel
 *	@return		Address of PlugIn Buffer(Input Position)
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_out_get_pi_buff_in_addr( UINT8 out_ch )
{
	return gBF_AudioOut_PI_Buff[out_ch][gBF_AudioOut_PI_Buff_Info[out_ch].in_pos].addr;
}

/**
 * Get AUDIO PCM input address
 * @param   VOID
 * @return  VOID
 */
static UINT32 bf_audio_out_get_pi_buff_in_size( UINT8 out_ch )
{
	return gBF_AudioOut_PI_Buff[out_ch][gBF_AudioOut_PI_Buff_Info[out_ch].in_pos].size;
}

/**
 *	Get AUDIO PCM output address
 *	@param		UINT8	AudioOut Channel
 *	@return		Address of PlugIn Buffer(Output Position)
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_out_get_pi_buff_out_addr( UINT8 out_ch )
{
	return gBF_AudioOut_PI_Buff[out_ch][gBF_AudioOut_PI_Buff_Info[out_ch].out_pos].addr;
}

/**
 *	Get AUDIO PCM output size
 *	@param		UINT8	AudioOut Channel
 *	@return		Size of PlugIn Buffer(Output Position)
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_out_get_pi_buff_out_size( UINT8 out_ch )
{
	return gBF_AudioOut_PI_Buff[out_ch][gBF_AudioOut_PI_Buff_Info[out_ch].out_pos].size;
}

/**
 * Initialize Capture PCM Buffer
 * @param  VOID
 * @return D_AUDIO_OK
 */
static INT32 bf_audio_out_cap_init_buffer( UINT8 out_ch )
{
	gBF_AudioOut_Cap_PI_Buff_Info[out_ch].in_pos	= 0;
	gBF_AudioOut_Cap_PI_Buff_Info[out_ch].out_pos	= 0;
	gBF_AudioOut_Cap_PI_Buff_Info[out_ch].count		= 0;
	gBF_AudioOut_Cap_PI_Buff_Info[out_ch].data_num	= 0;
	
	return D_AUDIO_OK;
}


/**
 * Capture PCM Input Buffer Renovation
 * @param   VOID
 * @return  D_AUDIO_OK / D_AUDIO_ERR
 */
static INT32 bf_audio_out_cap_renov_inputbuff_in( UINT8 out_ch )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioOut_CS_CapPi_Cnt );
	
	if (gBF_AudioOut_Cap_PI_Buff_Info[out_ch].count < (D_AUDIO_OUT_BUFF_ARRAY - 1)){
		gBF_AudioOut_Cap_PI_Buff_Info[out_ch].in_pos++;
		gBF_AudioOut_Cap_PI_Buff_Info[out_ch].in_pos %= D_AUDIO_OUT_BUFF_ARRAY;
		
		gBF_AudioOut_Cap_PI_Buff_Info[out_ch].count++;	// counter increment
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no empty area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioOut_CS_CapPi_Cnt );
	
	return retcd;
}

/**
 * Capture PCM Output Buffer Renovation
 * @param   VOID
 * @return  D_AUDIO_OK / D_AUDIO_ERR
 */
static INT32 bf_audio_out_cap_renov_inputbuff_out( UINT8 out_ch )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioOut_CS_CapPi_Cnt );
	
	if (gBF_AudioOut_Cap_PI_Buff_Info[out_ch].count > 0){
		gBF_AudioOut_Cap_PI_Buff_Info[out_ch].out_pos++;
		gBF_AudioOut_Cap_PI_Buff_Info[out_ch].out_pos %= D_AUDIO_OUT_BUFF_ARRAY;
		
		gBF_AudioOut_Cap_PI_Buff_Info[out_ch].count--;	// counter decrement
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no busy area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioOut_CS_CapPi_Cnt );
	
	return retcd;
}

/**
 * Get AUDIO PCM input address
 * @param   VOID
 * @return  VOID
 */
static UINT32 bf_audio_out_cap_get_inputbuff_addr( UINT8 out_ch, UCHAR cap_id )
{
	UINT32 addr;
	UINT32 size;
	UINT32 frame;
	UINT32 loop;
	UINT8 channel;
	INT32 ret;
	UINT8 byte_cnt;
	
	if (gBF_AudioOut_Cap_PI_Buff_Info[out_ch].data_num < 1){
		addr = BF_AudioCapture_Get_BuffAddr_Out(cap_id);
		size = BF_AudioCapture_Get_BuffSize_Out(cap_id);
		channel = BF_AudioCapture_Get_BuffChType(cap_id);
		byte_cnt = BF_AudioCapture_Get_Buff_BytePerSample(cap_id);
		
		ret = BF_AudioCapture_Renov_Buff_Out(cap_id);
		if (ret != D_AUDIO_OK){
			bf_audio_out_request_stop(out_ch);
			
			BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioCapture_Renov_Buff_Out() Buffer UnderFlow L=%d\n", __LINE__));
		}
		
		frame = size / byte_cnt / channel/ D_AUDIO_OUT_NUM_SAMPLE;
		
		for (loop = 0; loop < frame; loop++){
			gBF_AudioOut_Cap_Input_Buff[out_ch][gBF_AudioOut_Cap_PI_Buff_Info[out_ch].in_pos].addr = addr;
			gBF_AudioOut_Cap_Input_Buff[out_ch][gBF_AudioOut_Cap_PI_Buff_Info[out_ch].in_pos].size = D_AUDIO_OUT_NUM_SAMPLE * channel * byte_cnt;
			addr += D_AUDIO_OUT_NUM_SAMPLE * byte_cnt * channel;
			
			(VOID)bf_audio_out_cap_renov_inputbuff_in(out_ch);
		}
		
		gBF_AudioOut_Cap_PI_Buff_Info[out_ch].data_num += frame;
	}
	
	gBF_AudioOut_Cap_PI_Buff_Info[out_ch].data_num--;
	
	return gBF_AudioOut_Cap_Input_Buff[out_ch][gBF_AudioOut_Cap_PI_Buff_Info[out_ch].out_pos].addr;
}

/**
 * Get AUDIO PCM input address
 * @param   VOID
 * @return  VOID
 */
static UINT32 bf_audio_out_cap_get_inputbuff_size( UINT8 out_ch, UCHAR cap_id )
{
	return gBF_AudioOut_Cap_Input_Buff[out_ch][gBF_AudioOut_Cap_PI_Buff_Info[out_ch].out_pos].size;
}

/**
 * Get I2S Channel
 * @param   VOID
 * @return  VOID
 */
static UINT8 bf_audio_out_get_i2s_dev_ch( UINT8 out_ch )
{
	UINT8 dev_ch;
	
	if (gBF_Audio_Out_Inst[out_ch].out_port == E_FJ_AUDIO_OUT_PORT_I2S0){
		dev_ch = D_AUDIO_AUIF_CH0;
	}
	else if (gBF_Audio_Out_Inst[out_ch].out_port == E_FJ_AUDIO_OUT_PORT_I2S2){
		dev_ch = D_AUDIO_AUIF_CH2;
	}
	else {
		dev_ch = D_AUDIO_AUIF_CH3;
	}
	
	return dev_ch;
}

/**
 * Change Buzzer PCM Data Format
 * @param   VOID
 * @return  VOID
 */
static INT32 bf_audio_out_chg_buzzer_format_bitcnt( UINT32 addr, UINT32* size, UINT8 out_ch )
{
	UINT8 bz_bit_cnt;
	
	if (size == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_chg_buzzer_format_bitcnt() param err. size=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	bz_bit_cnt = BF_AudioBuzzer_Get_Buff_BitsPerSample(out_ch);
	if (bz_bit_cnt == gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count){
		memcpy((VOID*)addr,
			   (VOID*)BF_AudioBuzzer_Get_BuffAddr(out_ch),
				BF_AudioBuzzer_Get_BuffSize(out_ch));
		
		*size = BF_AudioBuzzer_Get_BuffSize(out_ch);
	}
	else {
		// Change BitsPerSample
		if ((bz_bit_cnt == D_AUDIO_PCM_16_BIT) && (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT)){
			
			BF_Audio_Data_Pcm_16bitTo24bit((UINT8*)BF_AudioBuzzer_Get_BuffAddr(out_ch),
										   (UINT8*)addr,
										   BF_AudioBuzzer_Get_BuffSize(out_ch));
			
			*size = (BF_AudioBuzzer_Get_BuffSize(out_ch) / 2) * 3;
		}
		else {
			memcpy((VOID*)addr,
				   (VOID*)BF_AudioBuzzer_Get_BuffAddr(out_ch),
					BF_AudioBuzzer_Get_BuffSize(out_ch));
			
			*size = BF_AudioBuzzer_Get_BuffSize(out_ch);
		}
	}
	
	return D_AUDIO_OK;
}

/**
 * Change Buzzer PCM Data Format
 * @param   VOID
 * @return  VOID
 */
static INT32 bf_audio_out_chg_buzzer_format_channel( UINT32 addr, UINT32* size, UINT8 out_ch )
{
	UINT8 bz_channel;
	UINT8 in_ch_type;
	
	if (size == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_chg_buzzer_format_channel() param err. size=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	bz_channel = BF_AudioBuzzer_Get_Buff_Channel(out_ch);
	in_ch_type = gBF_Audio_Out_Inst[out_ch].in_ch_type;
	
	if (in_ch_type <= FJ_AUDIO_NUM_CHANNEL_2){
		if (bz_channel != in_ch_type){
			if (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT){
				if (in_ch_type == FJ_AUDIO_NUM_CHANNEL_1){
					// stereo->mono
					BF_Audio_Data_24bitPcm_StereoToMono((UINT8*)addr, (UINT8*)addr, *size);
					*size /= 2;
				}
				else {
					//if (in_ch_type == 2){
					// mono->stereo
					BF_Audio_Data_24bitPcm_MonoToStereo((UINT8*)addr, (UINT8*)addr, *size);
					*size *= 2;
				}
			}
			else {
				// gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count == D_AUDIO_PCM_16_BIT
				if (in_ch_type == FJ_AUDIO_NUM_CHANNEL_1){
					// stereo->mono
					pcm_Stereo_to_Mono((unsigned short*)addr, *size / 4 );
					*size /= 2;
				}
				else {
					//if (in_ch_type == 2){
					// mono->stereo
					pcm_Mono_to_Stereo((unsigned short*)addr, *size / 2 );
					*size *= 2;
				}
			}
		}
	}
	else {
		if (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT){
			if (bz_channel == FJ_AUDIO_NUM_CHANNEL_1){
				BF_Audio_Data_24bitPcm_MonoToStereo((UINT8*)addr, (UINT8*)addr, *size);
				*size *= 2;
			}
			
			if (in_ch_type == FJ_AUDIO_NUM_CHANNEL_3){
				BF_Audio_Data_24bitPcm_Cnv2chTo3ch((UINT8*)addr, (UINT8*)gBF_AudioOut_Bz_Cnv_Work, *size);
				*size = (*size / 2) * 3;
				memcpy((VOID*)addr, (VOID*)gBF_AudioOut_Bz_Cnv_Work, *size);
			}
			else if (in_ch_type == FJ_AUDIO_NUM_CHANNEL_4){
				BF_Audio_Data_24bitPcm_Cnv2chTo4ch((UINT8*)addr, (UINT8*)gBF_AudioOut_Bz_Cnv_Work, *size);
				*size *= 2;
				memcpy((VOID*)addr, (VOID*)gBF_AudioOut_Bz_Cnv_Work, *size);
			}
			else {
				;
			}
		}
		else {
			// 16bit
			if (bz_channel == FJ_AUDIO_NUM_CHANNEL_1){
				pcm_Mono_to_Stereo((unsigned short*)addr, *size / 2 );
				*size *= 2;
			}
			
			if (in_ch_type == FJ_AUDIO_NUM_CHANNEL_3){
				BF_Audio_Data_16bitPcm_Cnv2chTo3ch((UINT16*)addr, (UINT16*)gBF_AudioOut_Bz_Cnv_Work, *size);
				*size = (*size / 2) * 3;
				memcpy((VOID*)addr, (VOID*)gBF_AudioOut_Bz_Cnv_Work, *size);
			}
			else if (in_ch_type == FJ_AUDIO_NUM_CHANNEL_4){
				BF_Audio_Data_16bitPcm_Cnv2chTo4ch((UINT16*)addr, (UINT16*)gBF_AudioOut_Bz_Cnv_Work, *size);
				*size *= 2;
				memcpy((VOID*)addr, (VOID*)gBF_AudioOut_Bz_Cnv_Work, *size);
			}
			else {
				;
			}
		}
	}
	
	return D_AUDIO_OK;
}

/**
 * Change Buzzer PCM Data Format
 * @param   VOID
 * @return  VOID
 */
static INT32 bf_audio_out_change_buzzer_format( UINT32 addr, UINT32* size, UINT8 out_ch )
{
	INT32 ret;
	UINT8 dev_ch;
	
	// bit count
	ret = bf_audio_out_chg_buzzer_format_bitcnt(addr, size, out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_chg_buzzer_format_bitcnt() err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	// Change Channels
	ret = bf_audio_out_chg_buzzer_format_channel(addr, size, out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_chg_buzzer_format_channel() err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = BF_AudioBuzzer_Renov_Buff(out_ch);
	if (ret != D_AUDIO_OK){
		gBF_AudioOut_Buzzer_End_Flg[out_ch] = D_AUDIO_OUT_ON;
		
		dev_ch = bf_audio_out_get_i2s_dev_ch(out_ch);
		
		BF_Audio_Auif_Set_Disable_UnderFlow_Int(dev_ch);
		
		gBF_AudioOut_Buzzer_StopFactor[out_ch] = FJ_AUDIO_CB_OUT_AUTO_STOP;
		
//		BF_Debug_Print_Debug(("[AUDIO_OUT]BF_AudioBuzzer_Renov_Buff() Buzzer END L=%d\n", __LINE__));
	}
	
	return D_AUDIO_OK;
}


/**
 * Change Decoded PCM Data Format
 * @param   VOID
 * @return  VOID
 */
static INT32 bf_audio_out_change_decoded_pcm_format( UINT32 addr, UINT32* size, UINT8 out_ch )
{
	INT32 ret;
	UINT8 dev_ch;
	
	if (size == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_change_decoded_pcm_format() param err. size=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT){
		// Change BitsPerSample
		BF_Audio_Data_Pcm_16bitTo24bit((UINT8*)bf_audio_out_get_decoded_pcm_addr(out_ch), (UINT8*)addr, *size);
		*size = (*size / 2) * 3;
	}
	else {
		memcpy((VOID*)addr, (VOID*)bf_audio_out_get_decoded_pcm_addr(out_ch), *size);
	}
	
	ret = BF_AudioDec_Renov_Buff(gBF_Audio_Out_Inst[out_ch].audio_out_id);
	if (ret != D_AUDIO_OK){
		if (gBF_AudioOut_Dec_End_Flg[out_ch] == D_AUDIO_OFF){
			gBF_AudioOut_Dec_End_Flg[out_ch] = D_AUDIO_OUT_WAIT_END;
			
			dev_ch = bf_audio_out_get_i2s_dev_ch(out_ch);
			
			BF_Audio_Auif_Set_Disable_UnderFlow_Int(dev_ch);
			
//			BF_Debug_Print_Debug(("[AUDIO_OUT]BF_AudioDec_Renov_Buff() AAC END L=%d\n", __LINE__));
		}
	}
	
	return D_AUDIO_OK;
}

/**
 * Get Decoded PCM Data Address
 * @param   VOID
 * @return  VOID
 */
static UINT32 bf_audio_out_get_decoded_pcm_addr( UINT8 out_ch )
{
	UINT32 addr;
	
	if (gBF_AudioOut_Dec_End_Flg[out_ch] == D_AUDIO_OFF){
		addr = BF_AudioDec_Get_BuffAddr(gBF_Audio_Out_Inst[out_ch].audio_out_id);
	}
	else {
		addr = (UINT32)BF_Audio_Data_Get_NullData_Addr();
	}
	
	return addr;
}

/**
 *  @brief		Set parameter of Start Process(Capture).
 *	@param		INT32 index of Instance Array
 *	@return		D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_output_start_cap_set_prm( INT32 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	INT32 ret;
	T_AUDIO_OUT_CNV_DATA_INFO cnv_info;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_output_start_cap_set_prm() param err. param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	param->in_addr			= bf_audio_out_cap_get_inputbuff_addr(out_ch, gBF_Audio_Out_Inst[out_ch].connect_cap_id);
	param->in_size			= bf_audio_out_cap_get_inputbuff_size(out_ch, gBF_Audio_Out_Inst[out_ch].connect_cap_id);
	param->in_ch_type		= gBF_Audio_Out_Inst[out_ch].in_ch_type;
	param->in_sampling_rate = gBF_Audio_Out_Inst[out_ch].in_smpl_rate;
	param->out_addr			= bf_audio_out_get_pi_buff_in_addr(out_ch);
	
	ret = bf_audio_out_plugin_cb_preproc(out_ch, param);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_plugin_cb_preproc() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	cnv_info.in_addr	= param->out_addr;
	cnv_info.out_addr	= bf_audio_out_get_pi_buff_in_addr(out_ch);
	cnv_info.size		= param->out_size;
	cnv_info.data_ch	= param->out_ch_type;
	
	ret = bf_audio_out_conversion_output_data((UINT8)out_ch, &cnv_info);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_conversion_output_data() error. L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_renov_pi_buff_in(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_renov_pi_buff_in() Buffer OverFlow L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_cap_renov_inputbuff_out(out_ch);
	if (ret != D_AUDIO_OK){
		bf_audio_out_request_stop(out_ch);
		
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_cap_renov_inputbuff_out() Buffer UnderFlow L=%d\n", __LINE__));
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Start Process(Capture).
 *	@param		INT32 index of Instance Array
 *	@return  D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_output_start_cap_get_data( INT32 out_ch )
{
	T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM param;
	OS_USER_ER ercd;
	INT32 ret;
	
	(VOID)bf_audio_out_cap_init_buffer(out_ch);
	
	ret = bf_audio_out_output_start_cap_set_prm(out_ch, &param);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_cap_set_prm() Err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_AudioOut_Cap_PI_Buff_Info[out_ch].data_num <= 1){
		if (gBF_AudioOut_Cap_PI_Buff_Info[out_ch].data_num == 0){
			ercd = OS_User_Dly_Tsk((D_AUDIO_OUT_NUM_SAMPLE * 1000 / gBF_Audio_Out_Inst[out_ch].in_smpl_rate) + 1);
			if (ercd != D_OS_USER_E_OK) {
				BF_Debug_Print_Error(("[AUDIO_OUT]dly_tsk error(%d) Line=%d\n", ercd, __LINE__));
			}
		}
		
		ret = bf_audio_out_output_start_cap_set_prm(out_ch, &param);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_cap_set_prm() Err. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
		
		if (gBF_AudioOut_Cap_PI_Buff_Info[out_ch].data_num <= 1){
			if (gBF_AudioOut_Cap_PI_Buff_Info[out_ch].data_num == 0){
				ercd = OS_User_Dly_Tsk((D_AUDIO_OUT_NUM_SAMPLE * 1000 / gBF_Audio_Out_Inst[out_ch].in_smpl_rate) + 1);
				if (ercd != D_OS_USER_E_OK) {
					BF_Debug_Print_Error(("[AUDIO_OUT]dly_tsk error(%d) Line=%d\n", ercd, __LINE__));
				}
			}
			
			ret = bf_audio_out_output_start_cap_set_prm(out_ch, &param);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_cap_set_prm() Err. L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
		}
	}
	
	// for Sync AudioCapture
	ercd = OS_User_Dly_Tsk((32 * 1000 / gBF_Audio_Out_Inst[out_ch].in_smpl_rate) + 1);			// 32sample=AudioMacro BufferSize
	if (ercd != D_OS_USER_E_OK) {
		BF_Debug_Print_Error(("[AUDIO_OUT]dly_tsk error(%d) Line=%d\n", ercd, __LINE__));
	}
	
	gBF_Audio_Out_Inst[out_ch].out_ch_type = param.out_ch_type;
	gBF_Audio_Out_Inst[out_ch].out_smpl_rate = param.out_sampling_rate;
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set parameter of Start Process(Decoder).
 *	@param		INT32 index of Instance Array
 *	@return		D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_output_start_dec_set_prm( INT32 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	INT32 ret;
	T_AUDIO_OUT_CNV_DATA_INFO cnv_info;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_output_start_dec_set_prm() param err. param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	param->in_size = BF_AudioDec_Get_BuffSize(gBF_Audio_Out_Inst[out_ch].audio_out_id);
	
	ret = bf_audio_out_change_decoded_pcm_format(bf_audio_out_get_inputbuff_in_addr(out_ch), (UINT32*)&param->in_size, out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_change_decoded_pcm_format() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_renov_inputbuff_in(out_ch);
	
	param->in_addr = bf_audio_out_get_inputbuff_out_addr(out_ch);
	
	(VOID)bf_audio_out_renov_inputbuff_out(out_ch);
	
	param->in_ch_type = gBF_Audio_Out_Inst[out_ch].in_ch_type;
	param->in_sampling_rate = gBF_Audio_Out_Inst[out_ch].in_smpl_rate;
	param->out_addr = bf_audio_out_get_pi_buff_in_addr(out_ch);
	
	ret = bf_audio_out_plugin_cb_preproc(out_ch, param);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_plugin_cb_preproc() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	cnv_info.in_addr	= param->out_addr;
	cnv_info.out_addr	= bf_audio_out_get_pi_buff_in_addr(out_ch);
	cnv_info.size		= param->out_size;
	cnv_info.data_ch	= param->out_ch_type;
	
	ret = bf_audio_out_conversion_output_data((UINT8)out_ch, &cnv_info);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_conversion_output_data() error. L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_renov_pi_buff_in(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_renov_pi_buff_in() Buffer OverFlow L=%d\n", __LINE__));
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Start Process(Decoder).
 *	@param		INT32 index of Instance Array
 *	@return  D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_output_start_dec_get_data( INT32 out_ch )
{
	T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM param;
	INT32 ret;
	
	// First Frame
	ret = bf_audio_out_output_start_dec_set_prm(out_ch, &param);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_dec_set_prm() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	// Second Frame
	if (gBF_AudioOut_Dec_End_Flg[out_ch] == D_AUDIO_OFF){
		ret = bf_audio_out_output_start_dec_set_prm(out_ch, &param);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_dec_set_prm() error. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
		
		if (gBF_AudioOut_Dec_End_Flg[out_ch] == D_AUDIO_OFF){
			ret = bf_audio_out_output_start_dec_set_prm(out_ch, &param);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_dec_set_prm() error. L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
		}
	}
	
	gBF_AudioOut_Dec_StopFlg[out_ch] = D_AUDIO_OFF;
	
	gBF_Audio_Out_Inst[out_ch].out_ch_type	= param.out_ch_type;
	gBF_Audio_Out_Inst[out_ch].out_smpl_rate	= param.out_sampling_rate;
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set parameter of Start Process(Buzzer).
 *	@param		INT32 index of Instance Array
 *	@return		D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_output_start_bz_set_prm( INT32 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	INT32 ret;
	T_AUDIO_OUT_CNV_DATA_INFO cnv_info;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_output_start_bz_set_prm() param err. param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = bf_audio_out_change_buzzer_format(bf_audio_out_get_inputbuff_in_addr(out_ch), (UINT32*)&param->in_size, out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_change_buzzer_format() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_renov_inputbuff_in(out_ch);
	
	param->in_addr = bf_audio_out_get_inputbuff_out_addr(out_ch);
	
	(VOID)bf_audio_out_renov_inputbuff_out(out_ch);
	
	param->in_ch_type		= gBF_Audio_Out_Inst[out_ch].in_ch_type;
	param->in_sampling_rate	= gBF_Audio_Out_Inst[out_ch].in_smpl_rate;
	param->out_addr			= bf_audio_out_get_pi_buff_in_addr(out_ch);
	
	ret = bf_audio_out_plugin_cb_preproc(out_ch, param);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_plugin_cb_preproc() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	cnv_info.in_addr	= param->out_addr;
	cnv_info.out_addr	= bf_audio_out_get_pi_buff_in_addr(out_ch);
	cnv_info.size		= param->out_size;
	cnv_info.data_ch	= param->out_ch_type;
	
	ret = bf_audio_out_conversion_output_data((UINT8)out_ch, &cnv_info);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_conversion_output_data() error. L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_renov_pi_buff_in(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_renov_pi_buff_in() Buffer OverFlow L=%d\n", __LINE__));
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Start Process(Buzzer).
 *	@param		INT32 index of Instance Array
 *	@return  D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_output_start_bz_get_data( INT32 out_ch )
{
	T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM param;
	INT32 ret;
	
	ret = bf_audio_out_output_start_bz_set_prm(out_ch, &param);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_bz_set_prm() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_AudioOut_Buzzer_End_Flg[out_ch] == D_AUDIO_OUT_OFF){
		ret = bf_audio_out_output_start_bz_set_prm(out_ch, &param);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_bz_set_prm() error. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
		
		if (gBF_AudioOut_Buzzer_End_Flg[out_ch] == D_AUDIO_OUT_OFF){
			ret = bf_audio_out_output_start_bz_set_prm(out_ch, &param);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_bz_set_prm() error. L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
		}
	}
	
	gBF_Audio_Out_Inst[out_ch].out_ch_type = param.out_ch_type;
	gBF_Audio_Out_Inst[out_ch].out_smpl_rate = param.out_sampling_rate;
	
	return D_AUDIO_OK;
}

/**
 * Change Buzzer PCM Data Format
 * @param   VOID
 * @return  VOID
 */
static VOID bf_audio_out_testmode_copy( UINT8 out_ch, UINT32 addr, UINT32 size )
{
	UINT32 remain_size;
	
	if (gBF_AudioOut_TestMode[out_ch] != 0){
		remain_size = gBF_AudioOut_TestMode_Size[out_ch]
					  - (gBF_AudioOut_TestMode_CurrentAddr[out_ch] - gBF_AudioOut_TestMode_StartAddr[out_ch]);
		
		if (remain_size >= size){
			memcpy((VOID*)gBF_AudioOut_TestMode_CurrentAddr[out_ch], (VOID*)addr, size);
			
			gBF_AudioOut_TestMode_CurrentAddr[out_ch] += size;
		}
		else {
			memcpy((VOID*)gBF_AudioOut_TestMode_CurrentAddr[out_ch], (VOID*)addr, remain_size);
			
			BF_Audio_Data_Clean_Flush_Dcache((VOID*)gBF_AudioOut_TestMode_StartAddr[out_ch], gBF_AudioOut_TestMode_Size[out_ch]);
			
			BF_Debug_Print_Audio(("[AUDIO_OUT]Dump Finished. Addr=0x%x, Size=0x%x\n",
									gBF_AudioOut_TestMode_StartAddr[out_ch], gBF_AudioOut_TestMode_Size[out_ch]));
			
			gBF_AudioOut_TestMode[out_ch] = 0;
		}
	}
}

/**
 * Change Buzzer PCM Data Format
 * @param   VOID
 * @return  VOID
 */
static VOID bf_audio_out_testmode_stop( UINT8 out_ch )
{
	UINT32 dump_size;
	
	if (gBF_AudioOut_TestMode[out_ch] != 0){
		dump_size = gBF_AudioOut_TestMode_CurrentAddr[out_ch] - gBF_AudioOut_TestMode_StartAddr[out_ch];
		
		BF_Audio_Data_Clean_Flush_Dcache((VOID*)gBF_AudioOut_TestMode_StartAddr[out_ch], dump_size);
		
		BF_Debug_Print_Audio(("[AUDIO_OUT]Dump Finished. Addr=0x%x, Size=0x%x\n", gBF_AudioOut_TestMode_StartAddr[out_ch], dump_size));
		
		gBF_AudioOut_TestMode[out_ch] = 0;
	}
}

/**
 *	@brief		Check Sampling Rate Value
 *	@param		UINT32	Sampling Rate
 *	@return		INT32	Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_check_prm_sample_rate( UINT32 sample_rate )
{
	if ((sample_rate == D_AUDIO_SAMPLE_RATE_8000)	||
		(sample_rate == D_AUDIO_SAMPLE_RATE_11025)	||
		(sample_rate == D_AUDIO_SAMPLE_RATE_12000)	||
		(sample_rate == D_AUDIO_SAMPLE_RATE_16000)	||
		(sample_rate == D_AUDIO_SAMPLE_RATE_22050)	||
		(sample_rate == D_AUDIO_SAMPLE_RATE_24000)	||
		(sample_rate == D_AUDIO_SAMPLE_RATE_32000)	||
		(sample_rate == D_AUDIO_SAMPLE_RATE_44100)	||
		(sample_rate == D_AUDIO_SAMPLE_RATE_48000)	||
		(sample_rate == D_AUDIO_SAMPLE_RATE_96000)	){
		
		return D_AUDIO_OK;
	}
	else {
		return D_AUDIO_ERR_PARAM;
	}
}

/**
 *	@brief		Check Configuration Value
 *	@param		T_FJ_AUDIO_OUT_CFG	Configuration information
 *	@return		INT32					Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_check_config_param( UCHAR out_id, const T_FJ_AUDIO_OUT_CFG* audio_out_config )
{
	INT32 loop;
	
	if (audio_out_config->audio_out_port > E_FJ_AUDIO_OUT_PORT_I2S3){
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Config(). audio_out_port is out of range(=%d). L=%d\n",
								(INT32)audio_out_config->audio_out_port, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if (audio_out_config->audio_out_port == (E_FJ_AUDIO_OUT_PORT_ENUM)1){				/* pgr0839 */
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Config(). audio_out_port is out of range(=%d). L=%d\n",
								(INT32)audio_out_config->audio_out_port, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}

	if (audio_out_config->i2s_cfg.master_slave > FJ_AUDIO_IF_SLAVE){
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Config(). master_slave is out of range(=%d). L=%d\n",
								(INT32)audio_out_config->i2s_cfg.master_slave, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if (bf_audio_out_check_prm_sample_rate(audio_out_config->i2s_cfg.lr_clock) != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Config(). lr_clock is out of range(=%d). L=%d\n",
								(INT32)audio_out_config->i2s_cfg.lr_clock, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if ((audio_out_config->i2s_cfg.data_cycle != D_AUDIO_I2S_CYCLE_32FS) &&
		(audio_out_config->i2s_cfg.data_cycle != D_AUDIO_I2S_CYCLE_64FS) ){
		
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Config(). data_cycle is out of range(=%d). L=%d\n",
								audio_out_config->i2s_cfg.data_cycle, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if (audio_out_config->i2s_cfg.i2s_format > E_FJ_AUDIO_I2S_Data_FORMAT_I2S){
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Config(). i2s_format is out of range(=%d). L=%d\n",
								(INT32)audio_out_config->i2s_cfg.i2s_format, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if ((audio_out_config->i2s_cfg.bit_count != D_AUDIO_PCM_8_BIT)	&&
		(audio_out_config->i2s_cfg.bit_count != D_AUDIO_PCM_16_BIT) &&
		(audio_out_config->i2s_cfg.bit_count != D_AUDIO_PCM_24_BIT) ){
		
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Config(). bit_count is out of range(=%d). L=%d\n",
								audio_out_config->i2s_cfg.bit_count, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if ((audio_out_config->i2s_cfg.data_cycle == D_AUDIO_I2S_CYCLE_32FS) &&
		(audio_out_config->i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT)		 ){
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Config(). Not Correct Parameter. data_cycle=%d, bit_count=%d L=%d\n",
		 audio_out_config->i2s_cfg.data_cycle, audio_out_config->i2s_cfg.bit_count, __LINE__));
		return D_AUDIO_ERR_PARAM;
	}
	
	for (loop = 0; loop < D_AUDIO_OUT_INST_NUM; loop++){
		if (audio_out_config->audio_out_port == gBF_Audio_Out_Inst[loop].out_port){
			if (gBF_Audio_Out_Inst[loop].audio_out_id != out_id){
				if (gBF_Audio_Out_Inst[loop].set_cfg_flg == D_AUDIO_ON){
					BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Config(). I2S used already. L=%d\n", __LINE__));
					return D_AUDIO_ERR;
				}
			}
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Callback AudioOut PlugIn
 *	@param		UCHAR				AudioOut ID
 *	@param		T_FJ_AUDIO_OUT_CFG	Callback information
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_plugin_cb_preproc( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	INT32 ret;
	
	if (gBF_AudioOut_TestMode[out_ch] == 2){
		bf_audio_out_testmode_copy(out_ch, param->in_addr, param->in_size);
	}
	
	ret = bf_audio_out_plugin_cb(out_ch, param);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_plugin_cb() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_AudioOut_TestMode[out_ch] == 1){
		bf_audio_out_testmode_copy(out_ch, param->out_addr, param->out_size);
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Callback AudioOut PlugIn
 *	@param		UCHAR				AudioOut ID
 *	@param		T_FJ_AUDIO_OUT_CFG	Callback information
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_plugin_cb( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_plugin_cb() param err. param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	// initialize out parameter(fail safe)
	param->out_size				= param->in_size;
	param->out_ch_type			= param->in_ch_type;
	param->out_sampling_rate	= param->in_sampling_rate;
	
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)param->in_addr, param->in_size);
	FJ_Host_AudioOutPlugInCB(gBF_Audio_Out_Inst[out_ch].audio_out_id, param);
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)param->out_addr, param->out_size);
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Request Notification of Buzzer Stop
 *	@param		UCHAR				AudioOut ID
 *	@param		T_FJ_AUDIO_OUT_CFG	Callback information
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_bz_stop_req_ntf( UINT8 out_ch )
{
	OS_USER_ER ercd;
	
	if (gBF_AudioOut_Buzzer_StopReq[out_ch] == D_AUDIO_OUT_ON){
		gBF_AudioOut_Buzzer_StopReq[out_ch] = D_AUDIO_OUT_OFF;
		gBF_AudioOut_Buzzer_StopFactor[out_ch] = FJ_AUDIO_CB_OUT_STOP;
		
		ercd = OS_User_Set_Flg(FID_AUDIO_OUT, gBF_AudioOut_EventFlag_BzStopNtfy_Tbl[out_ch]);
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
}

/**
 *  @brief		Setup start output process.
 *	@param		UINT8	audio_out_ch
 *	@param		UINT8	audio_dev_ch
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_set_auif_data_format( UINT8 out_ch, UINT8 dev_ch )
{
	INT32 ret;
	
	if (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count != D_AUDIO_PCM_8_BIT){
		ret = BF_Audio_Auif_Output_Ctrl(dev_ch, D_AUDIO_AUIF_AHB_FORMAT_R);
	}
	else {
		ret = BF_Audio_Auif_Output_Ctrl(dev_ch, D_AUDIO_AUIF_AHB_FORMAT_L);
	}
	
	return ret;
}

/**
 *  @brief		Get address of output data register .
 *	@param		UINT8	audio_out_ch
 *	@param		UINT8	audio_dev_ch
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_out_get_auif_data_reg_addr( UINT8 out_ch, UINT8 dev_ch )
{
	UINT32 addr;
	
	if (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT){
		addr = BF_Audio_Auif_Get_Addr_Reg_AUODDMAPT(dev_ch);
	}
	else {
		addr = BF_Audio_Auif_Get_Addr_Reg_AUODLR(dev_ch);
	}
	
	return addr;
}

/**
 *	@brief		Setup AudioIF Macro Wrapper
 *	@param		UINT8			AudioOut Channel
 *	@param		UINT8			AudioIF Macro Channel
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_setup_auif( UINT8 out_ch, UINT8 dev_ch )
{
	INT32 ret;
	
	ret = BF_Audio_Auif_OpenOutput(dev_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_Auif_OpenOutput() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = BF_Audio_Auif_Set_Channel_Config(dev_ch, gBF_Audio_Out_Inst[out_ch].out_ch_type);
	if (ret != D_AUDIO_OK){
		(VOID)BF_Audio_Auif_CloseOutput(dev_ch);
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_Auif_Set_Channel_Config() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = bf_audio_out_set_auif_data_format(out_ch, dev_ch);
	if (ret != D_AUDIO_OK){
		(VOID)BF_Audio_Auif_CloseOutput(dev_ch);
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_Auif_Output_Ctrl() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_Audio_Out_Inst[out_ch].out_ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		ret = BF_Audio_Auif_Set_Dma_ModeOut(dev_ch, E_AUDIO_DMA_MODE_OUT_2CH, (D_AUDIO_OUT_NUM_SAMPLE / 2));
	}
	else {
		// FJ_CHANNEL_TYPE_STEREO
		ret = BF_Audio_Auif_Set_Dma_ModeOut(dev_ch, E_AUDIO_DMA_MODE_OUT_2CH, D_AUDIO_OUT_NUM_SAMPLE);
	}
	
	if (ret != D_AUDIO_OK){
		(VOID)BF_Audio_Auif_CloseOutput(dev_ch);
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_Auif_Set_Dma_ModeOut() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count != D_AUDIO_PCM_24_BIT){
		BF_Audio_Auif_Swap_OutputData(dev_ch, FJ_AUDIO_ENABLE);
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Setup start output process.
 *	@param		UINT8	out_ch
 *	@param		UINT8	dev_ch
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_output_start_setup( UINT8 out_ch, UINT8 dev_ch )
{
	INT32 ret;
	UINT8 out_ch_type;
	
	ret = bf_audio_out_setup_auif(out_ch, dev_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_setup_auif() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	out_ch_type = gBF_Audio_Out_Inst[out_ch].out_ch_type;
	
	if (out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		
		ret = bf_audio_out_setup_auif(out_ch, D_AUDIO_AUIF_CH1);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_setup_auif() Error. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
	}
	
	ret = BF_Audio_DMA_Init(&gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID0]);
	if (ret != D_AUDIO_OK){
		(VOID)BF_Audio_Auif_CloseOutput(dev_ch);
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_DMA_Init() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		ret = BF_Audio_DMA_Init(&gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID1]);
		if (ret != D_AUDIO_OK){
			(VOID)BF_Audio_Auif_CloseOutput(dev_ch);
			(VOID)BF_Audio_Auif_CloseOutput(D_AUDIO_AUIF_CH1);
			BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_DMA_Init() Error. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Setup start output process(for AudioCapture data).
 *	@param		UINT8	out_ch	AudioOut Channel
 *	@param		UINT8	dev_ch	AudioIF Macro Channel
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_output_start_dma( UINT8 out_ch )
{
	UINT8 dev_ch;
	INT32 ret;
	T_AUDIO_DMA_START_PARAM dma_start;
	UINT8 out_ch_type;

	dev_ch = bf_audio_out_get_i2s_dev_ch(out_ch);

	ret = bf_audio_out_output_start_setup(out_ch, dev_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_setup(). L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	BF_Audio_DMA_Set_Callback_Fnc0(gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID0], gBf_AudioOut_Dma0_Callback[out_ch]);
	BF_Audio_DMA_Set_Callback_Fnc1(gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID0], gBf_AudioOut_Dma1_Callback[out_ch]);
	
	out_ch_type = gBF_Audio_Out_Inst[out_ch].out_ch_type;
	
	if (out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		BF_Audio_DMA_Set_Callback_Fnc0(gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID1], gBf_AudioOut_Dma0_2nd_Callback[out_ch]);
		BF_Audio_DMA_Set_Callback_Fnc1(gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID1], gBf_AudioOut_Dma1_2nd_Callback[out_ch]);
	}
	
	dma_start.dma_id		= gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID0];
	dma_start.audio_dev_ch	= dev_ch;
	dma_start.dst_address	= bf_audio_out_get_auif_data_reg_addr(out_ch, dev_ch);
	dma_start.trans_size	= bf_audio_out_get_pi_buff_out_size(out_ch);
	dma_start.src_address	= bf_audio_out_get_pi_buff_out_addr(out_ch);
	
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)dma_start.src_address, dma_start.trans_size);
	BF_Audio_DMA_Output_Start0(&dma_start);
	
	if (out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		dma_start.dma_id		= gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID1];
		dma_start.audio_dev_ch	= D_AUDIO_AUIF_CH1;
		dma_start.dst_address	= bf_audio_out_get_auif_data_reg_addr(out_ch, D_AUDIO_AUIF_CH1);
		dma_start.trans_size	= bf_audio_out_get_pi_buff_out_size(out_ch);
		dma_start.src_address	= bf_audio_out_get_pi_buff_out_addr(out_ch) + dma_start.trans_size;
		
		BF_Audio_Data_Clean_Flush_Dcache((VOID*)dma_start.src_address, dma_start.trans_size);
		BF_Audio_DMA_Output_Start0(&dma_start);
	}
	
	ret= bf_audio_out_renov_pi_buff_out(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_renov_pi_buff_out() error L=%d\n", __LINE__));
	}
	
	dma_start.dma_id		= gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID0];
	dma_start.audio_dev_ch	= dev_ch;
	dma_start.dst_address	= bf_audio_out_get_auif_data_reg_addr(out_ch, dev_ch);
	dma_start.trans_size	= bf_audio_out_get_pi_buff_out_size(out_ch);
	dma_start.src_address	= bf_audio_out_get_pi_buff_out_addr(out_ch);
	
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)dma_start.src_address, dma_start.trans_size);
	BF_Audio_DMA_Output_Start1(&dma_start);
	
	if (out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		dma_start.dma_id		= gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID1];
		dma_start.audio_dev_ch	= D_AUDIO_AUIF_CH1;
		dma_start.dst_address	= bf_audio_out_get_auif_data_reg_addr(out_ch, D_AUDIO_AUIF_CH1);
		dma_start.trans_size	= bf_audio_out_get_pi_buff_out_size(out_ch);
		dma_start.src_address	= bf_audio_out_get_pi_buff_out_addr(out_ch) + dma_start.trans_size;
		
		BF_Audio_Data_Clean_Flush_Dcache((VOID*)dma_start.src_address, dma_start.trans_size);
		BF_Audio_DMA_Output_Start1(&dma_start);
	}
	
	ret= bf_audio_out_renov_pi_buff_out(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_renov_pi_buff_out() error L=%d\n", __LINE__));
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Setup start output process(for AudioCapture data).
 *	@param		UINT8	out_ch	AudioOut Channel
 *	@param		UINT8	dev_ch	AudioIF Macro Channel
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_output_start_dma_cap( UINT8 out_ch )
{
	INT32 ret;
	
	ret = bf_audio_out_init_buffer(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_init_buffer() error L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_output_start_cap_get_data(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_cap_get_data(). L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_output_start_dma(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_dma(). L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/*
 *  @brief		Setup start output process(for AudioDecoder data).
 *	@param		UINT8	out_ch	AudioOut Channel
 *	@param		UINT8	dev_ch	AudioIF Macro Channel
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_output_start_dma_dec( UINT8 out_ch )
{
	INT32 ret;
	
	ret = bf_audio_out_init_buffer(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_init_buffer() error L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_output_start_dec_get_data(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_dec_get_data(). L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_output_start_dma(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_dma(). L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/*
 *  @brief		Setup start output process(for Buzzer data).
 *	@param		UINT8	out_ch	AudioOut Channel
 *	@param		UINT8	dev_ch	AudioIF Macro Channel
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_output_start_dma_bz( UINT8 out_ch )
{
	INT32 ret;
	
	ret = bf_audio_out_init_buffer(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_init_buffer() error L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_output_start_bz_get_data(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_bz_get_data(). L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_output_start_dma(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_dma(). L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set parameter of Start Process(Capture).
 *	@param		UINT8	out_ch index of Instance Array
 *	@param		T_AUDIO_OUT_CNV_DATA_INFO information of conversion data
 *	@return		D_AUDIO_OK / D_AUDIO_ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_conversion_output_data( UINT8 out_ch, T_AUDIO_OUT_CNV_DATA_INFO* cnv_info )
{
	if (cnv_info == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_conversion_output_data() param err. cnv_info=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (cnv_info->data_ch <= FJ_AUDIO_NUM_CHANNEL_2){
		if (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT){
			BF_Audio_Data_3ByteTo4Byte((UINT8*)cnv_info->in_addr, (UINT8*)cnv_info->out_addr, cnv_info->size);
			cnv_info->size = (cnv_info->size / 3) * 4;
		}
		else {
			if (cnv_info->in_addr != cnv_info->out_addr){
				memcpy((VOID*)cnv_info->out_addr, (VOID*)cnv_info->in_addr, cnv_info->size);
			}
		}
	}
	else if (cnv_info->data_ch == FJ_AUDIO_NUM_CHANNEL_3){
		if (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT){
			BF_Audio_Data_3ByteTo4Byte((UINT8*)cnv_info->in_addr, (UINT8*)cnv_info->out_addr, cnv_info->size);
			
			cnv_info->size = (cnv_info->size / 3) * 4;
			
			BF_Audio_Data_Swp_32bitPcm_3chTo2ch((UINT32*)cnv_info->out_addr,
												(UINT32*)gBF_AudioOut_Work,
												(UINT32*)((UINT32)gBF_AudioOut_Work + (cnv_info->size * 2 / 3)),
												cnv_info->size);
			
			cnv_info->size = (cnv_info->size / 3) * 4;
			
			memcpy((VOID*)cnv_info->out_addr, (VOID*)gBF_AudioOut_Work, cnv_info->size);
		}
		else {
			BF_Audio_Data_Swp_16bitPcm_3chTo2ch((UINT16*)cnv_info->in_addr,
												(UINT16*)gBF_AudioOut_Work,
												(UINT16*)((UINT32)gBF_AudioOut_Work + (cnv_info->size * 2 / 3)),
												cnv_info->size);
												
			cnv_info->size = (cnv_info->size / 3) * 4;
			
			memcpy((VOID*)cnv_info->out_addr, (VOID*)gBF_AudioOut_Work, cnv_info->size);
		}
	}
	else {
		//param->cnv_info->data_ch == FJ_AUDIO_NUM_CHANNEL_4
		if (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT){
			BF_Audio_Data_3ByteTo4Byte((UINT8*)cnv_info->in_addr, (UINT8*)cnv_info->out_addr, cnv_info->size);
			cnv_info->size = (cnv_info->size / 3) * 4;
			
			BF_Audio_Data_Swp_32bitPcm_4chTo2ch((UINT32*)cnv_info->out_addr,
												(UINT32*)gBF_AudioOut_Work,
												(UINT32*)((UINT32)gBF_AudioOut_Work + (cnv_info->size / 2)),
												cnv_info->size);
												
			memcpy((VOID*)cnv_info->out_addr, (VOID*)gBF_AudioOut_Work, cnv_info->size);
		}
		else {
			BF_Audio_Data_Swp_16bitPcm_4chTo2ch((UINT32*)cnv_info->in_addr,
												(UINT32*)gBF_AudioOut_Work,
												(UINT32*)((UINT32)gBF_AudioOut_Work + (cnv_info->size / 2)),
												cnv_info->size);
			memcpy((VOID*)cnv_info->out_addr, (VOID*)gBF_AudioOut_Work, cnv_info->size);
		}
	}
	
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)cnv_info->out_addr, cnv_info->size);
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Get Instance audio_out_ch
 *	@param		UCHAR			AudioOut ID
 *	@param		UINT8			AudioOut Channel
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_get_out_ch( UCHAR out_id, UINT8* out_ch )
{
	INT32 ret = D_AUDIO_ERR;
	INT32 loop;

	if (out_ch == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_get_out_ch() param err. out_ch=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	for (loop = 0; loop < D_AUDIO_OUT_INST_NUM; loop++){
		if (gBF_Audio_Out_Inst[loop].audio_out_id == out_id){
			*out_ch = loop;
			ret = D_AUDIO_OK;
			break;
		}
	}
	
	return ret;
}

/**
 *  @brief		Wait Interrupt for Multi Channel Operation(Tx PCM Data).
 *	@param		UINT8 audio_out_ch
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_wait_intr_multi_ch( UINT8 out_ch )
{
	OS_USER_FLGPTN flgptn = 0;
	OS_USER_TMO timeout;
	OS_USER_ER ercd;
	
	if (gBF_Audio_Out_Inst[out_ch].out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		timeout = D_AUDIO_OUT_NUM_SAMPLE / (gBF_Audio_Out_Inst[out_ch].i2s_cfg.lr_clock / 1000);
		
		ercd = OS_User_Twai_Flg(FID_AUDIO_OUT,
								gBF_AudioOut_EventFlag_PcmSnd2nd_Tbl[out_ch],
								D_OS_USER_TWF_ORW,
								&flgptn,
								timeout);
		
		if ( ercd != D_OS_USER_E_OK ){
			if ( ercd == D_OS_USER_E_TMOUT ){
				BF_Debug_Print_Error(("[AUDIO_OUT]twai_flg time out(%d) L=%d\n", ercd, __LINE__));
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_OUT]twai_flg error(%d) L=%d\n", ercd, __LINE__));
			}
		}
		
		ercd = OS_User_Clr_Flg(FID_AUDIO_OUT, ~gBF_AudioOut_EventFlag_PcmSnd2nd_Tbl[out_ch]);
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]clr_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
}

/**
 *	@brief		Before Plugin Data
 *	@param		UINT8							AudioOut Channel
 *	@param		T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM	PlugIn Pramter
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_trns_pcm_rx_connect_mix( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	INT32 ret;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_trns_pcm_rx_connect_mix() param err. param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_AudioOut_Dec_End_Flg[out_ch] == D_AUDIO_OFF){
		param->in_size = BF_AudioDec_Get_BuffSize(gBF_Audio_Out_Inst[out_ch].audio_out_id);
		
		ret = bf_audio_out_change_decoded_pcm_format(bf_audio_out_get_inputbuff_in_addr(out_ch), (UINT32*)&param->in_size, out_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_change_decoded_pcm_format() error. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
		
		(VOID)bf_audio_out_renov_inputbuff_in(out_ch);
		
		ret = bf_audio_out_change_buzzer_format((UINT32)gBF_AudioOut_MixWork_Bz, (UINT32*)&param->in_size, out_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_change_buzzer_format() error. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
		
		param->in_addr = bf_audio_out_get_inputbuff_out_addr(out_ch);
		
		(VOID)bf_audio_out_renov_inputbuff_out(out_ch);
		
		if (gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT){
			BF_Audio_Data_Add_24bitPcm((UINT8*)param->in_addr, (UINT8*)gBF_AudioOut_MixWork_Bz, (UINT8*)param->in_addr, param->in_size);
		}
		else {
			BF_Audio_Data_Add_16bitPcm((INT16*)param->in_addr, (INT16*)gBF_AudioOut_MixWork_Bz, (INT16*)param->in_addr, param->in_size);
		}
	}
	else {
		param->in_addr = (UINT32)gBF_AudioOut_MixWork_Bz;
		ret = bf_audio_out_change_buzzer_format(param->in_addr, (UINT32*)&param->in_size, out_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_change_buzzer_format() error. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Before Plugin Data
 *	@param		UINT8							AudioOut Channel
 *	@param		T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM	PlugIn Pramter
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_trns_pcm_rx_connect_replace( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	INT32 ret;
	UINT8 dev_ch;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_trns_pcm_rx_connect_replace() param err. param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_AudioOut_Dec_End_Flg[out_ch] == D_AUDIO_OFF){
		ret = BF_AudioDec_Renov_Buff(gBF_Audio_Out_Inst[out_ch].audio_out_id);
		if (ret != D_AUDIO_OK){
			gBF_AudioOut_Dec_End_Flg[out_ch] = D_AUDIO_OUT_WAIT_END;
			dev_ch = bf_audio_out_get_i2s_dev_ch(out_ch);
			
			BF_Audio_Auif_Set_Disable_UnderFlow_Int(dev_ch);
			
			BF_Debug_Print_Debug(("[AUDIO_OUT]BF_AudioDec_Renov_Buff() AAC END L=%d\n", __LINE__));
		}
	}
	
	param->in_addr = (UINT32)gBF_AudioOut_MixWork_Bz;
	ret = bf_audio_out_change_buzzer_format(param->in_addr, (UINT32*)&param->in_size, out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_change_buzzer_format() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Before Plugin Data
 *	@param		UINT8							AudioOut Channel
 *	@param		T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM	PlugIn Pramter
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_trns_pcm_rx_connect_none( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	INT32 ret;
	UINT8 dev_ch;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_trns_pcm_rx_connect_none() param err. param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_Start( gBF_AudioOut_CS_Bz_EndFlg );
	
	if (gBF_AudioOut_Buzzer_End_Flg[out_ch] == D_AUDIO_OUT_OFF){
		ret = BF_AudioBuzzer_Renov_Buff(out_ch);
		if (ret != D_AUDIO_OK){
			gBF_AudioOut_Buzzer_End_Flg[out_ch] = D_AUDIO_OUT_ON;
			dev_ch = bf_audio_out_get_i2s_dev_ch(out_ch);
			BF_Audio_Auif_Set_Disable_UnderFlow_Int(dev_ch);
			gBF_AudioOut_Buzzer_StopFactor[out_ch] = FJ_AUDIO_CB_OUT_AUTO_STOP;
		}
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioOut_CS_Bz_EndFlg );
	
	param->in_size = BF_AudioDec_Get_BuffSize(gBF_Audio_Out_Inst[out_ch].audio_out_id);
	
	ret = bf_audio_out_change_decoded_pcm_format(bf_audio_out_get_inputbuff_in_addr(out_ch),
										  (UINT32*)&param->in_size, out_ch);
	
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_change_decoded_pcm_format() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_renov_inputbuff_in(out_ch);
	
	param->in_addr = bf_audio_out_get_inputbuff_out_addr(out_ch);
	
	(VOID)bf_audio_out_renov_inputbuff_out(out_ch);
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Before Plugin Data
 *	@param		UINT8							AudioOut Channel
 *	@param		T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM	PlugIn Pramter
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_trns_pcm_rx_connect_inst_cap( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	INT32 ret;
	T_AUDIO_OUT_CNV_DATA_INFO cnv_info;
	UINT8 cnnct_cap_id;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_trns_pcm_rx_connect_inst_cap() param err. param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	cnnct_cap_id = gBF_Audio_Out_Inst[out_ch].connect_cap_id;
	
	if ((gBF_Audio_Out_Inst[out_ch].buzzer_flg == D_AUDIO_OUT_OFF)	||
		(gBF_AudioOut_Buzzer_End_Flg[out_ch] != D_AUDIO_OUT_OFF)	){
		
		param->in_addr = bf_audio_out_cap_get_inputbuff_addr(out_ch, cnnct_cap_id);
		param->in_size = bf_audio_out_cap_get_inputbuff_size(out_ch, cnnct_cap_id);
		
		ret = bf_audio_out_cap_renov_inputbuff_out(out_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_cap_renov_inputbuff_out() Buffer UnderFlow L=%d\n", __LINE__));
		}
		
		bf_audio_out_bz_stop_req_ntf(out_ch);
	}
	else {
		// Output Capture & Buzzer -> Buzzer
		
		// Renovation Input Buffer: Free capture data area.
		(VOID)bf_audio_out_cap_get_inputbuff_addr(out_ch, cnnct_cap_id);
		(VOID)bf_audio_out_cap_renov_inputbuff_out(out_ch);
		
		ret = bf_audio_out_change_buzzer_format(bf_audio_out_get_inputbuff_in_addr(out_ch), (UINT32*)&param->in_size, out_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_change_buzzer_format() error. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
		
		(VOID)bf_audio_out_renov_inputbuff_in(out_ch);
		
		param->in_addr = bf_audio_out_get_inputbuff_out_addr(out_ch);
		
		(VOID)bf_audio_out_renov_inputbuff_out(out_ch);
	}
	
	param->in_ch_type = gBF_Audio_Out_Inst[out_ch].in_ch_type;
	param->in_sampling_rate = gBF_Audio_Out_Inst[out_ch].in_smpl_rate;
	param->out_addr = bf_audio_out_get_pi_buff_in_addr(out_ch);
	
	ret = bf_audio_out_plugin_cb_preproc(out_ch, param);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_plugin_cb_preproc() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	cnv_info.in_addr	= param->out_addr;
	cnv_info.out_addr	= bf_audio_out_get_pi_buff_in_addr(out_ch);
	cnv_info.size		= param->out_size;
	cnv_info.data_ch	= param->out_ch_type;
	
	ret = bf_audio_out_conversion_output_data((UINT8)out_ch, &cnv_info);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_conversion_output_data() error. L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_renov_pi_buff_in(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_renov_pi_buff_in() Buffer OverFlow L=%d\n", __LINE__));
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Before Plugin Data
 *	@param		UINT8							AudioOut Channel
 *	@param		T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM	PlugIn Pramter
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_trns_pcm_rx_connect_inst_dec( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	INT32 ret;
	T_AUDIO_OUT_CNV_DATA_INFO cnv_info;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_trns_pcm_rx_connect_inst_dec() param err. param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if ((gBF_Audio_Out_Inst[out_ch].buzzer_flg == D_AUDIO_OUT_OFF)	||
		(gBF_AudioOut_Buzzer_End_Flg[out_ch] != D_AUDIO_OUT_OFF)	){
		
		param->in_size = BF_AudioDec_Get_BuffSize(gBF_Audio_Out_Inst[out_ch].audio_out_id);
		
		ret = bf_audio_out_change_decoded_pcm_format(bf_audio_out_get_inputbuff_in_addr(out_ch),
											   (UINT32*)&param->in_size, out_ch);
		
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_change_decoded_pcm_format() error. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
		
		(VOID)bf_audio_out_renov_inputbuff_in(out_ch);
		
		param->in_addr = bf_audio_out_get_inputbuff_out_addr(out_ch);
		
		(VOID)bf_audio_out_renov_inputbuff_out(out_ch);
	}
	else {
		bf_audio_out_bz_stop_req_ntf(out_ch);
		
		if (gBF_Audio_Out_Inst[out_ch].bz_mode == E_FJ_AUDIO_WAV_CTL_MIX){
			ret = bf_audio_out_trns_pcm_rx_connect_mix(out_ch, param);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_trns_pcm_rx_connect_mix() error. L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
		}
		else if (gBF_Audio_Out_Inst[out_ch].bz_mode == E_FJ_AUDIO_WAV_CTL_REPLACE){
			// E_FJ_AUDIO_WAV_CTL_REPLACE
			ret = bf_audio_out_trns_pcm_rx_connect_replace(out_ch, param);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_trns_pcm_rx_connect_replace() error. L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
		}
		else {
			ret = bf_audio_out_trns_pcm_rx_connect_none(out_ch, param);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_trns_pcm_rx_connect_none() error. L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
		}
	}
	
	param->in_ch_type		= gBF_Audio_Out_Inst[out_ch].in_ch_type;
	param->in_sampling_rate	= gBF_Audio_Out_Inst[out_ch].in_smpl_rate;
	param->out_addr			= bf_audio_out_get_pi_buff_in_addr(out_ch);
	
	ret = bf_audio_out_plugin_cb_preproc(out_ch, param);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_plugin_cb_preproc() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	cnv_info.in_addr	= param->out_addr;
	cnv_info.out_addr	= bf_audio_out_get_pi_buff_in_addr(out_ch);
	cnv_info.size		= param->out_size;
	cnv_info.data_ch	= param->out_ch_type;
	
	ret = bf_audio_out_conversion_output_data((UINT8)out_ch, &cnv_info);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_conversion_output_data() error. L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_renov_pi_buff_in(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_renov_pi_buff_in() Buffer OverFlow L=%d\n", __LINE__));
	}
	
	BF_AudioDec_Request_Decode(gBF_Audio_Out_Inst[out_ch].connect_dec_id,
							   gBF_Audio_Out_Inst[out_ch].audio_out_id);
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Before Plugin Data
 *	@param		UINT8							AudioOut Channel
 *	@param		T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM	PlugIn Pramter
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_trns_pcm_rx_connect_inst_bz( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	INT32 ret;
	T_AUDIO_OUT_CNV_DATA_INFO cnv_info;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_out_trns_pcm_rx_connect_inst_bz() param err. param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = bf_audio_out_change_buzzer_format(bf_audio_out_get_inputbuff_in_addr(out_ch), (UINT32*)&param->in_size, out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_change_buzzer_format() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_renov_inputbuff_in(out_ch);
	
	param->in_addr = bf_audio_out_get_inputbuff_out_addr(out_ch);
	
	(VOID)bf_audio_out_renov_inputbuff_out(out_ch);
	
	param->in_ch_type		= gBF_Audio_Out_Inst[out_ch].in_ch_type;
	param->in_sampling_rate	= gBF_Audio_Out_Inst[out_ch].in_smpl_rate;
	param->out_addr			= bf_audio_out_get_pi_buff_in_addr(out_ch);
	
	ret = bf_audio_out_plugin_cb_preproc(out_ch, param);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_plugin_cb_preproc() error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	cnv_info.in_addr	= param->out_addr;
	cnv_info.out_addr	= bf_audio_out_get_pi_buff_in_addr(out_ch);
	cnv_info.size		= param->out_size;
	cnv_info.data_ch	= param->out_ch_type;
	
	ret = bf_audio_out_conversion_output_data((UINT8)out_ch, &cnv_info);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_conversion_output_data() error. L=%d\n", __LINE__));
	}
	
	ret = bf_audio_out_renov_pi_buff_in(out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_renov_pi_buff_in() Buffer OverFlow L=%d\n", __LINE__));
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Before Plugin Data
 *	@param		UINT8							AudioOut Channel
 *	@param		T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM	PlugIn Pramter
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_trns_pcm_rx_connect_inst( UINT8 out_ch, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM* param )
{
	INT32 ret;
	
	if (gBF_Audio_Out_Inst[out_ch].cap_flg == D_AUDIO_OUT_ON){
		// Capture
		ret = bf_audio_out_trns_pcm_rx_connect_inst_cap(out_ch, param);
	}
	else if (gBF_Audio_Out_Inst[out_ch].dec_flg == D_AUDIO_OUT_ON){
		// Decoder
		ret = bf_audio_out_trns_pcm_rx_connect_inst_dec(out_ch, param);
	}
	else {
		// buzzer
		ret = bf_audio_out_trns_pcm_rx_connect_inst_bz(out_ch, param);
	}
	
	return ret;
}

/**
 *	@brief		Request Stop
 *	@param		UINT8							AudioOut Channel
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_request_stop( UINT8 out_ch )
{
	OS_USER_ER ercd;
	
	// EventFlag Table
	static const OS_USER_FLGPTN event_flg_stop_tbl[D_AUDIO_OUT_INST_NUM] = {
		FLG_AUDIO_OUT_OUT0_STOP,
		FLG_AUDIO_OUT_OUT1_STOP
	};
	
	gBF_AudioOut_StopProcessing_Id[out_ch] = gBF_Audio_Out_Inst[out_ch].audio_out_id;
	
	ercd = OS_User_Set_Flg(FID_AUDIO_OUT, event_flg_stop_tbl[out_ch]);
	if (ercd != D_OS_USER_E_OK) {
		BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) Line=%d\n", ercd, __LINE__));
	}
}

/**
 *	@brief		Configuration of HDMI Audio
 *	@param		T_FJ_AUDIO_OUT_CFG			Configration
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_hdmi_config( T_FJ_AUDIO_OUT_CFG* out_cfg )
{
	E_BF_DISP_HDMI_AUDIO_I2S_JUST hdmi_i2s_frmt;
	E_BF_DISP_HDMI_AUDIO_I2S_LEN hdmi_i2s_bit_cnt;
	
	if (out_cfg->i2s_cfg.i2s_format == E_FJ_AUDIO_I2S_Data_FORMAT_RIGHT){
		hdmi_i2s_frmt = E_BF_DISP_HDMI_AUDIO_I2S_JUST_RIGHT;
	}
	else if (out_cfg->i2s_cfg.i2s_format == E_FJ_AUDIO_I2S_Data_FORMAT_LEFT){
		hdmi_i2s_frmt = E_BF_DISP_HDMI_AUDIO_I2S_JUST_LEFT;
	}
	else {
		hdmi_i2s_frmt = E_BF_DISP_HDMI_AUDIO_I2S_STANDARD;
	}
	
	if (out_cfg->i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT){
		hdmi_i2s_bit_cnt = E_BF_DISP_HDMI_AUDIO_I2S_LEN_24;
	}
	else {
		hdmi_i2s_bit_cnt = E_BF_DISP_HDMI_AUDIO_I2S_LEN_16;
	}
	
	(VOID)BF_Disp_Set_Audio_I2S((ULONG)out_cfg->i2s_cfg.lr_clock, hdmi_i2s_frmt, hdmi_i2s_bit_cnt);
}

/**
 *	@brief		Wait stop process
 *	@param		UINT8			audio out channel
 *	@return		INT32	OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_start_wait_stop_proc( UINT8 out_ch )
{
	UINT32 count;
	
	count = 0;
	while ( gBF_AudioOut_StopProcessing_Id[out_ch] != D_AUDIO_OUT_INVALID){
		OS_User_Dly_Tsk(1);
		count++;
		
		if (count > 200){
			BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Start(). TimeOut. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Buzzer Mixing Start
 *	@param		UINT8			audio out channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_start_bz_mix( UINT8 out_ch )
{
	OS_USER_ER ercd;
	
	if (gBF_Audio_Out_Inst[out_ch].bz_mode == E_FJ_AUDIO_WAV_CTL_NONE){
		ercd = OS_User_Set_Flg(FID_AUDIO_OUT, gBF_AudioOut_EventFlag_BzStopNtfy_Tbl[out_ch]);
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
	else {
		// E_FJ_AUDIO_WAV_CTL_MIX, E_FJ_AUDIO_WAV_CTL_REPLACE
		gBF_Audio_Out_Inst[out_ch].buzzer_flg = D_AUDIO_OUT_ON;
	}
}

/**
 *	@brief		Buzzer Mixing Start
 *	@param		UINT8			audio out channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_start_bz_only( UINT8 out_ch )
{
	INT32 ret;
	
	gBF_Audio_Out_Inst[out_ch].in_ch_type = BF_AudioBuzzer_Get_Buff_Channel(out_ch);
	
	(VOID)bf_audio_out_clear_state_change_flg();
	
	ret = bf_audio_out_set_eventflg_init(out_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_out_state_change_wait();
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_state_change_wait error(%d). Time Out L=%d\n", ret, __LINE__));
		}
		
		ret = bf_audio_out_set_eventflg_start(out_ch);
	}
	
	return ret;
}

/**
 *	@brief		Set event flag
 *	@param		UINT8			audio out channel
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_set_eventflg_init( UINT8 out_ch )
{
	INT32 ret;
	OS_USER_ER ercd;
	
	// EventFlag Table
	static const OS_USER_FLGPTN event_flg_init_tbl[D_AUDIO_OUT_INST_NUM] = {
		FLG_AUDIO_OUT_OUT0_INIT,
		FLG_AUDIO_OUT_OUT1_INIT
	};
	
	ercd = OS_User_Set_Flg( FID_AUDIO_OUT, event_flg_init_tbl[out_ch] );
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) L=%d\n", ercd, __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		ret = D_AUDIO_OK;
	}
	
	return ret;
}

/**
 *	@brief		Set event flag
 *	@param		UINT8			audio out channel
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_set_eventflg_start( UINT8 out_ch )
{
	INT32 ret;
	OS_USER_ER ercd;
	
	// EventFlag Table
	static const OS_USER_FLGPTN event_flg_start_tbl[D_AUDIO_OUT_INST_NUM] = {
		FLG_AUDIO_OUT_OUT0_START,
		FLG_AUDIO_OUT_OUT1_START
	};
	
	ercd = OS_User_Set_Flg( FID_AUDIO_OUT, event_flg_start_tbl[out_ch] );
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) L=%d\n", ercd, __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		ret = D_AUDIO_OK;
	}
	
	return ret;
}

/**
 *	@brief		Check Enable of Instance
 *	@param		UCHAR			audio out id
 *	@param		UINT8			audio out channel
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_check_inst_enable( UCHAR out_id, UINT8* out_ch )
{
	INT32 ret;
	
	ret = bf_audio_out_get_out_ch(out_id, out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_get_out_ch(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		if (gBF_Audio_Out_Inst[*out_ch].enable == FJ_AUDIO_DISABLE){
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_check_inst_enable(). Instance is disable. L=%d\n", __LINE__));
			ret = D_AUDIO_ERR;
		}
	}
	
	return ret;
}

/**
 *	@brief		Check Number of Dma ID
 *	@param		UINT8			audio out channel
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_out_check_dma_available( UINT8 out_ch )
{
	INT32 ret;
	
//	BF_Debug_Print_Information(("Number of Remain DMA:%d\n", BF_Audio_DMA_Get_RemainNumDmaID()));
	
	if (gBF_Audio_Out_Inst[out_ch].out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		if (BF_Audio_DMA_Get_RemainNumDmaID() >= 2){
			return D_AUDIO_OK;
		}
		else {
			ret = D_AUDIO_ERR;
		}
	}
	else {
		if (BF_Audio_DMA_Get_RemainNumDmaID() >= 1){
			return D_AUDIO_OK;
		}
		else {
			ret = D_AUDIO_ERR;
		}
	}
	
	return ret;
}

/****
 Interrupt Handler
****/
static VOID bf_audio_out_stop_check_bz_off( UINT8 out_ch, UINT8* bz_off_flg )
{
	OS_USER_ER ercd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioOut_CS_Bz_Flg );
	Dd_ARM_Critical_Section_Start( gBF_AudioOut_CS_Bz_EndFlg );
	
	*bz_off_flg = D_AUDIO_OFF;
	
	if ((gBF_Audio_Out_Inst[out_ch].buzzer_flg == D_AUDIO_OUT_ON)	&&
		(gBF_AudioOut_Buzzer_End_Flg[out_ch] == D_AUDIO_ON)			){
		// Buzzer ON->OFF
		
		gBF_AudioOut_Buzzer_End_Flg[out_ch]		= D_AUDIO_OFF;
		gBF_AudioOut_Buzzer_StopFactor[out_ch]	= FJ_AUDIO_CB_OUT_AUTO_STOP;
		gBF_Audio_Out_Inst[out_ch].buzzer_flg	= D_AUDIO_OUT_OFF;
		
		*bz_off_flg = D_AUDIO_ON;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioOut_CS_Bz_EndFlg );
	Dd_ARM_Critical_Section_End( gBF_AudioOut_CS_Bz_Flg );
	
	if (*bz_off_flg == D_AUDIO_ON){
		ercd = OS_User_Set_Flg(FID_AUDIO_OUT, gBF_AudioOut_EventFlag_BzStopNtfy_Tbl[out_ch]);
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
}


/**
 *  @brief		Check Stop of Interrupt handler
 *	@param		UINT8 audio out channel
 *	@return		BOOL
 *	@note		None
 *	@attention	None
 */
static BOOL bf_audio_out_stop_check( UINT8 out_ch )
{
	UINT8 bz_off_flg;
	
	// Stop Check
	if (gBF_Audio_Out_Inst[out_ch].cap_flg == D_AUDIO_OUT_ON){
		// Capture ON
		bf_audio_out_stop_check_bz_off(out_ch, &bz_off_flg);
	}
	else  if (gBF_Audio_Out_Inst[out_ch].dec_flg == D_AUDIO_OUT_ON){
		// Decoder ON
		if (gBF_AudioOut_Dec_End_Flg[out_ch] == D_AUDIO_ON){
			gBF_AudioOut_Dec_StopFlg[out_ch] = D_AUDIO_ON;
			
			if (gBF_Audio_Out_Inst[out_ch].buzzer_flg == D_AUDIO_OUT_OFF){
				bf_audio_out_request_stop(out_ch);
				return FALSE;
			}
		}
		
		if (gBF_AudioOut_Dec_End_Flg[out_ch] == D_AUDIO_OUT_WAIT_END){
			gBF_AudioOut_Dec_End_Flg[out_ch] = D_AUDIO_ON;
			
			if (gBF_Audio_Out_Inst[out_ch].buzzer_flg == D_AUDIO_OUT_OFF){
				return FALSE;
			}
		}
		
		bf_audio_out_stop_check_bz_off(out_ch, &bz_off_flg);
		
		if (bz_off_flg == D_AUDIO_ON){
			if (gBF_AudioOut_Dec_StopFlg[out_ch] == D_AUDIO_ON){
				bf_audio_out_request_stop(out_ch);
				return FALSE;
			}
		}
	}
	else {
		// Decoder OFF
		// Only Buzzer
		;
	}
	
	return TRUE;
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		UINT8 audio out channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_intr0( UINT8 out_ch )
{
	OS_USER_ER ercd;
	T_AUDIO_DMA_START_PARAM dma_start;
	
	gBF_Audio_Out_Inst[out_ch].output_count++;
	
	// Stop Check
	if (bf_audio_out_stop_check(out_ch) != FALSE){
		if (gBF_Audio_Out_Inst[out_ch].status != D_AUDIO_OUT_STATUS_STOP){
			dma_start.dma_id		= gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID0];
			dma_start.audio_dev_ch	= bf_audio_out_get_i2s_dev_ch(out_ch);
			dma_start.trans_size	= bf_audio_out_get_pi_buff_out_size(out_ch);
			dma_start.src_address	= bf_audio_out_get_pi_buff_out_addr(out_ch);
			dma_start.dst_address	= bf_audio_out_get_auif_data_reg_addr(out_ch, dma_start.audio_dev_ch);
			
			// DMA Start
			BF_Audio_DMA_Output_Start0(&dma_start);
			
			ercd = OS_User_Set_Flg(FID_AUDIO_OUT, gBF_AudioOut_EventFlag_PcmSnd_Tbl[out_ch]);
			if (ercd != D_OS_USER_E_OK) {
				BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) Line=%d\n", ercd, __LINE__));
			}
		}
	}
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		UINT8 audio out channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_intr1( UINT8 out_ch )
{
	OS_USER_ER ercd;
	T_AUDIO_DMA_START_PARAM dma_start;
	
	gBF_Audio_Out_Inst[out_ch].output_count++;
	
	// Stop Check
	if (bf_audio_out_stop_check(out_ch) != FALSE){
		if (gBF_Audio_Out_Inst[out_ch].status != D_AUDIO_OUT_STATUS_STOP){
			dma_start.dma_id		= gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID0];
			dma_start.audio_dev_ch	= bf_audio_out_get_i2s_dev_ch(out_ch);
			dma_start.trans_size	= bf_audio_out_get_pi_buff_out_size(out_ch);
			dma_start.src_address	= bf_audio_out_get_pi_buff_out_addr(out_ch);
			dma_start.dst_address	= bf_audio_out_get_auif_data_reg_addr(out_ch, dma_start.audio_dev_ch);
			
			// DMA Start
			BF_Audio_DMA_Output_Start1(&dma_start);
			
			ercd = OS_User_Set_Flg(FID_AUDIO_OUT, gBF_AudioOut_EventFlag_PcmSnd_Tbl[out_ch]);
			if (ercd != D_OS_USER_E_OK) {
				BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) Line=%d\n", ercd, __LINE__));
			}
		}
	}
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		UINT8 audio out channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_intr0_2nd( UINT8 out_ch )
{
	OS_USER_ER ercd;
	T_AUDIO_DMA_START_PARAM dma_start;
	
	gBF_Audio_Out_Inst[out_ch].output_count++;
	
	// Stop Check
	if (bf_audio_out_stop_check(out_ch) != FALSE){
		if (gBF_Audio_Out_Inst[out_ch].status != D_AUDIO_OUT_STATUS_STOP){
			dma_start.dma_id		= gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID1];
			dma_start.audio_dev_ch	= D_AUDIO_AUIF_CH1;
			dma_start.trans_size	= bf_audio_out_get_pi_buff_out_size(out_ch);
			dma_start.src_address	= bf_audio_out_get_pi_buff_out_addr(out_ch) + dma_start.trans_size;
			dma_start.dst_address	= bf_audio_out_get_auif_data_reg_addr(out_ch, dma_start.audio_dev_ch);
				
			// DMA Start
			BF_Audio_DMA_Output_Start0(&dma_start);
			
			ercd = OS_User_Set_Flg(FID_AUDIO_OUT, gBF_AudioOut_EventFlag_PcmSnd2nd_Tbl[out_ch]);
			if (ercd != D_OS_USER_E_OK) {
				BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) Line=%d\n", ercd, __LINE__));
			}
		}
	}
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_intr1_2nd( UINT8 out_ch )
{
	OS_USER_ER ercd;
	T_AUDIO_DMA_START_PARAM dma_start;
	
	gBF_Audio_Out_Inst[out_ch].output_count++;
	
	// Stop Check
	if (bf_audio_out_stop_check(out_ch) != FALSE){
		if (gBF_Audio_Out_Inst[out_ch].status != D_AUDIO_OUT_STATUS_STOP){
			dma_start.dma_id		= gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID1];
			dma_start.audio_dev_ch	= D_AUDIO_AUIF_CH1;
			dma_start.trans_size	= bf_audio_out_get_pi_buff_out_size(out_ch);
			dma_start.src_address	= bf_audio_out_get_pi_buff_out_addr(out_ch) + dma_start.trans_size;
			dma_start.dst_address	= bf_audio_out_get_auif_data_reg_addr(out_ch, dma_start.audio_dev_ch);
			
			// DMA Start
			BF_Audio_DMA_Output_Start1(&dma_start);
			
			ercd = OS_User_Set_Flg(FID_AUDIO_OUT, gBF_AudioOut_EventFlag_PcmSnd2nd_Tbl[out_ch]);
			if (ercd != D_OS_USER_E_OK) {
				BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) Line=%d\n", ercd, __LINE__));
			}
		}
	}
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out0_intr0( UCHAR dma_status )
{
#if 0
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	Dd_ARM_Critical_Section_Start( critseq );
	__disable_fiq();
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "#DMA"));
#endif
	
	bf_audio_out_intr0(D_AUDIO_OUT_CH_0);
	
#if 0
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "#DMA"));
	__enable_fiq();
	Dd_ARM_Critical_Section_End( critseq );
#endif
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out0_intr1( UCHAR dma_status )
{
#if 0
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	Dd_ARM_Critical_Section_Start( critseq );
	__disable_fiq();
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "#DMA"));
#endif
	
	bf_audio_out_intr1(D_AUDIO_OUT_CH_0);
	
#if 0
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "#DMA"));
	__enable_fiq();
	Dd_ARM_Critical_Section_End( critseq );
#endif
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out0_intr0_2nd( UCHAR dma_status )
{
#if 0
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	Dd_ARM_Critical_Section_Start( critseq );
	__disable_fiq();
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "#DMA"));
#endif
	
	bf_audio_out_intr0_2nd(D_AUDIO_OUT_CH_0);
	
#if 0
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "#DMA"));
	__enable_fiq();
	Dd_ARM_Critical_Section_End( critseq );
#endif
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out0_intr1_2nd( UCHAR dma_status )
{
#if 0
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	Dd_ARM_Critical_Section_Start( critseq );
	__disable_fiq();
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "#DMA"));
#endif
	
	bf_audio_out_intr1_2nd(D_AUDIO_OUT_CH_0);
	
#if 0
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "#DMA"));
	__enable_fiq();
	Dd_ARM_Critical_Section_End( critseq );
#endif
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out1_intr0( UCHAR dma_status )
{
#if 0
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	Dd_ARM_Critical_Section_Start( critseq );
	__disable_fiq();
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "#DMA"));
#endif
	
	bf_audio_out_intr0(D_AUDIO_OUT_CH_1);
	
#if 0
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "#DMA"));
	__enable_fiq();
	Dd_ARM_Critical_Section_End( critseq );
#endif
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out1_intr1( UCHAR dma_status )
{
#if 0
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	Dd_ARM_Critical_Section_Start( critseq );
	__disable_fiq();
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "#DMA"));
#endif
	
	bf_audio_out_intr1(D_AUDIO_OUT_CH_1);
	
#if 0
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "#DMA"));
	__enable_fiq();
	Dd_ARM_Critical_Section_End( critseq );
#endif
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out1_intr0_2nd( UCHAR dma_status )
{
#if 0
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	Dd_ARM_Critical_Section_Start( critseq );
	__disable_fiq();
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "#DMA"));
#endif
	
	bf_audio_out_intr0_2nd(D_AUDIO_OUT_CH_1);
	
#if 0
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "#DMA"));
	__enable_fiq();
	Dd_ARM_Critical_Section_End( critseq );
#endif
}

/**
 *  @brief		Audio Record Interrupt handler
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out1_intr1_2nd( UCHAR dma_status )
{
#if 0
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	Dd_ARM_Critical_Section_Start( critseq );
	__disable_fiq();
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "#DMA"));
#endif
	
	bf_audio_out_intr1_2nd(D_AUDIO_OUT_CH_1);
	
#if 0
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "#DMA"));
	__enable_fiq();
	Dd_ARM_Critical_Section_End( critseq );
#endif
}

/**
 *	@brief		Output Initialize Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_output_init0( VOID )
{
	BF_AudioOut_OutputInit(D_AUDIO_OUT_CH_0);
}
/**
 *	@brief		Output Initialize Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_output_init1( VOID )
{
	BF_AudioOut_OutputInit(D_AUDIO_OUT_CH_1);
}

/**
 *	@brief		Output Start Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_output_start0( VOID )
{
	BF_AudioOut_OutputStart(D_AUDIO_OUT_CH_0);
}

/**
 *	@brief		Output Start Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_output_start1( VOID )
{
	BF_AudioOut_OutputStart(D_AUDIO_OUT_CH_1);
}

/**
 *	@brief		Output Stop Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_output_stop0( VOID )
{
	BF_AudioOut_OutputStop(D_AUDIO_OUT_CH_0);
}

/**
 *	@brief		Output Stop Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_output_stop1( VOID )
{
	BF_AudioOut_OutputStop(D_AUDIO_OUT_CH_1);
}

/**
 *	@brief		Preparation Process of PCM Data from Capture and Buzzer
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_output_pcm_send0( VOID )
{
	BF_AudioOut_PcmSend(D_AUDIO_OUT_CH_0);
}

/**
 *	@brief		Preparation Process of PCM Data from Decoder and Buzzer
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_output_pcm_send1( VOID )
{
	BF_AudioOut_PcmSend(D_AUDIO_OUT_CH_1);
}

/**
 *	@brief		Buzzer Stop Notification Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_bz_stop_notification0( VOID )
{
	BF_AudioOut_NotifyBZStop(D_AUDIO_OUT_CH_0);
}

/**
 *	@brief		Buzzer Stop Notification Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_out_bz_stop_notification1( VOID )
{
	BF_AudioOut_NotifyBZStop(D_AUDIO_OUT_CH_1);
}


/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/

/**
 *  @brief		Audio Out Task
 *	@param		INT32	Task Start Code
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Audio_Out_Process( INT32 stacd )
{
	OS_USER_FLGPTN flgptn = 0;
	ULONG wai_flg_ptn;
	UINT32 loop;
	OS_USER_ER ercd ;
	
	wai_flg_ptn = FLG_AUDIO_OUT_OUT0_INIT		|
				  FLG_AUDIO_OUT_OUT1_INIT		|
				  FLG_AUDIO_OUT_OUT0_START		|
				  FLG_AUDIO_OUT_OUT1_START		|
				  FLG_AUDIO_OUT_OUT0_STOP		|
				  FLG_AUDIO_OUT_OUT1_STOP		|
				  FLG_AUDIO_OUT_OUT_PCM_SND0	|
				  FLG_AUDIO_OUT_OUT_PCM_SND1	|
				  FLG_AUDIO_OUT_BZ_STOP_NTFY0	|
				  FLG_AUDIO_OUT_BZ_STOP_NTFY1	;
	
	while (1) {
		ercd = OS_User_Wai_Flg( FID_AUDIO_OUT, wai_flg_ptn, D_OS_USER_TWF_ORW, &flgptn);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_OUT]wai_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "f=%lx", flgptn ));
		flgptn &= wai_flg_ptn;
		
		for (loop = 0; loop < D_AUDIO_OUT_NUM_AUDIO_PROCESS; loop++){
			if ((flgptn & gBF_Audio_Out_Process[loop].event_flag) != 0){
				ercd = OS_User_Clr_Flg(FID_AUDIO_OUT, ~(gBF_Audio_Out_Process[loop].event_flag));
				if (ercd != D_OS_USER_E_OK) {
					BF_Debug_Print_Error(("[AUDIO_OUT]clr_flg error(%d) L=%d\n", ercd, __LINE__));
				}
				
				(VOID)bf_audio_out_wai_sem();
				
				gBF_Audio_Out_Process[loop].process();
				
				(VOID)bf_audio_out_sig_sem();
			}
		}
		
		M_MBA_PRINTF((D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "f=%lx", flgptn));
	}
}

/**
 *  @brief		Start Audio Out Task
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioOut_TaskStart( VOID )
{
	OS_USER_ER ercd_sta;
	OS_USER_ER ercd_ter;
	OS_USER_ER ercd ;
	T_OS_USER_RTSK pk_rtsk;

	OS_User_Ref_Tsk(TID_AUDIO_OUT, &pk_rtsk);
	if (pk_rtsk.tskstat == D_OS_USER_TTS_DMT){
		// clear flag
		ercd = OS_User_Clr_Flg(FID_AUDIO_OUT, 0);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_OUT]clr_flg error(%d) Line=%d\n", ercd, __LINE__));
		}
		
		// Create Task process
		do {
			ercd_sta = OS_User_Sta_Tsk(TID_AUDIO_OUT, 0);
			if (ercd_sta == D_OS_USER_E_OBJ){
				ercd_ter = OS_User_Ter_Tsk(TID_AUDIO_OUT);
				if (ercd_ter != D_OS_USER_E_OK){
					BF_Debug_Print_Information(("[AUDIO_OUT] ter_tsk Error! ercd(%d), Line=%d\n", ercd, __LINE__));
				}
			}
		}
		while (ercd_sta != 0);
	}
}

/**
 *  @brief		Stop Audio Out Task
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioOut_TaskStop( VOID )
{
	OS_USER_ER ercd;

	ercd = OS_User_Ter_Tsk(TID_AUDIO_OUT);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Information(("[AUDIO_OUT] ter_tsk Error! ercd(%d), Line=%d\n", ercd, __LINE__));
	}
}

/**
 *  @brief		Open AudioOut Instance
 *	@param		E_FJ_AUDIO_OUT_ENUM	Audio Out
 *	@param		UCHAR*				Audio Out ID
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Open( E_FJ_AUDIO_OUT_ENUM audio_out, UCHAR* audio_out_id )
{
	INT32 loop;
	UINT8 ret_flg = D_AUDIO_OUT_OFF;
	INT32 ret;
	
	if (audio_out_id == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_AudioOut_Open() param err. audio_out_id=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_wai_sem();
	
	for (loop = 0; loop < D_AUDIO_OUT_INST_NUM; loop++){
		if (gBF_Audio_Out_Inst[loop].enable == FJ_AUDIO_DISABLE){
			gBF_Audio_Out_Inst[loop].enable = FJ_AUDIO_ENABLE;
			*audio_out_id = gBF_Audio_Out_Inst[loop].audio_out_id;
			gBF_Audio_Out_Inst[loop].type = audio_out;
			ret_flg = D_AUDIO_OUT_ON;
			break;
		}
	}
	
	BF_AudioOut_TaskStart();
	
	if (ret_flg == D_AUDIO_OUT_OFF){
		BF_Debug_Print_Error(("[AUDIO_OUT]Open Error. All instance are used. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		ret = D_AUDIO_OK;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Close AudioOut Instance
 *	@param		UCHAR				AudioOut ID
 *	@return		INT32				Result(OK)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Close( UCHAR audio_out_id )
{
	INT32 ret;
	UINT8 out_ch;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_get_out_ch(audio_out_id, &out_ch);
	if (ret != D_AUDIO_OK){
		ret = D_AUDIO_ERR;
	}
	else {
		// if processing, stop
		if (gBF_Audio_Out_Inst[out_ch].status == D_AUDIO_OUT_STATUS_START){
			BF_Debug_Print_Warning(("[AUDIO_OUT]BF_AudioOut_Close(%d) Output Operation Processing... L=%d\n", audio_out_id, __LINE__));
			(VOID)bf_audio_out_clear_state_change_flg();
			bf_audio_out_request_stop(out_ch);
			
			(VOID)bf_audio_out_state_change_wait();
		}
		
		gBF_Audio_Out_Inst[out_ch].enable			= FJ_AUDIO_DISABLE;
		gBF_Audio_Out_Inst[out_ch].set_cfg_flg		= D_AUDIO_OFF;
		gBF_Audio_Out_Inst[out_ch].connect_cap_id	= D_AUDIO_OUT_INVALID;
		gBF_Audio_Out_Inst[out_ch].connect_dec_id	= D_AUDIO_OUT_INVALID;
		gBF_Audio_Out_Inst[out_ch].connect_dec_id	= D_AUDIO_OUT_INVALID;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

INT32 BF_AudioOut_ForceClose( UCHAR audio_out_id )
{
	INT32 ret;
	UINT8 out_ch;
	
	ret = bf_audio_out_get_out_ch(audio_out_id, &out_ch);
	if (ret != D_AUDIO_OK){
		ret = D_AUDIO_ERR;
	}
	else {
		// if processing, stop
		if (gBF_Audio_Out_Inst[out_ch].status == D_AUDIO_OUT_STATUS_START){
			BF_Debug_Print_Warning(("[AUDIO_OUT]BF_AudioOut_Close(%d) Output Operation Processing... L=%d\n", audio_out_id, __LINE__));
			(VOID)bf_audio_out_clear_state_change_flg();
			bf_audio_out_request_stop(out_ch);
			
			(VOID)bf_audio_out_state_change_wait();
		}
		
		gBF_Audio_Out_Inst[out_ch].enable			= FJ_AUDIO_DISABLE;
		gBF_Audio_Out_Inst[out_ch].set_cfg_flg		= D_AUDIO_OFF;
		gBF_Audio_Out_Inst[out_ch].connect_cap_id	= D_AUDIO_OUT_INVALID;
		gBF_Audio_Out_Inst[out_ch].connect_dec_id	= D_AUDIO_OUT_INVALID;
		gBF_Audio_Out_Inst[out_ch].connect_dec_id	= D_AUDIO_OUT_INVALID;
	}
	
	return ret;
}

/**
 *  @brief		Connect AudioOut Instance with AudioCapture Instance
 *	@param		UCHAR				AudioOut ID
 *	@param		UCHAR				AudioCapture ID
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_ConnectCap( UCHAR audio_out_id, UCHAR audio_cap_id )
{
	INT32 ret;
	UINT8 out_ch;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret != D_AUDIO_OK){
		ret = D_AUDIO_ERR;
	}
	else {
		if (gBF_Audio_Out_Inst[out_ch].connect_dec_id == D_AUDIO_OUT_INVALID){
			gBF_Audio_Out_Inst[out_ch].connect_cap_id = audio_cap_id;
		}
		else {
			ret = D_AUDIO_ERR;
		}
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Disconnect AudioCapture Instance
 *	@param		UCHAR				AudioOut ID
 *	@param		UCHAR				AudioCapture ID
 *	@return		INT32				Result(OK)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_DisconnectCap( UCHAR audio_out_id, UCHAR audio_cap_id )
{
	INT32 ret;
	UINT8 out_ch;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_get_out_ch(audio_out_id, &out_ch);
	if (ret != D_AUDIO_OK){
		ret = D_AUDIO_ERR;
	}
	else {
		// if processing, stop
		if (gBF_Audio_Out_Inst[out_ch].status != D_AUDIO_OUT_STATUS_STOP){
			(VOID)bf_audio_out_clear_state_change_flg();
			bf_audio_out_request_stop(out_ch);
			
			(VOID)bf_audio_out_state_change_wait();
		}
		
		gBF_Audio_Out_Inst[out_ch].connect_cap_id = D_AUDIO_OUT_INVALID;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Connect AudioOut Instance with AudioDecoder Instance
 *	@param		UCHAR				AudioOut ID
 *	@param		UCHAR				AudioDecoder ID
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_ConnectDec( UCHAR audio_out_id, UCHAR audio_dec_id )
{
	INT32 ret;
	UINT8 out_ch;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret != D_AUDIO_OK){
		ret = D_AUDIO_ERR;
	}
	else {
		if (gBF_Audio_Out_Inst[out_ch].connect_cap_id == D_AUDIO_OUT_INVALID){
			gBF_Audio_Out_Inst[out_ch].connect_dec_id = audio_dec_id;
		}
		else {
			ret = D_AUDIO_ERR;
		}
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Disconnect AudioDecoder Instance
 *	@param		UCHAR				AudioOut ID
 *	@param		UCHAR				AudioDecoder ID
 *	@return		INT32				Result(OK)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_DisconnectDec( UCHAR audio_out_id, UCHAR audio_dec_id )
{
	INT32 ret;
	UINT8 out_ch;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_get_out_ch(audio_out_id, &out_ch);
	if (ret != D_AUDIO_OK){
		ret = D_AUDIO_ERR;
	}
	else {
		gBF_Audio_Out_Inst[out_ch].connect_dec_id = D_AUDIO_OUT_INVALID;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *	@brief		Configuration AudioOut Instance
 *	@param		UCHAR				AudioOut ID
 *	@param		T_FJ_AUDIO_OUT_CFG	Configuration information
 *	@return		INT32				Result(OK)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Config( UCHAR audio_out_id, T_FJ_AUDIO_OUT_CFG* audio_out_config )
{
	UINT8 out_ch;
	INT32 ret;
	T_FJ_AUDIO_OUT_CFG out_cfg;
	
	(VOID)bf_audio_out_wai_sem();
	
	out_cfg = *audio_out_config;
	
	ret = bf_audio_out_check_config_param(audio_out_id, &out_cfg);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
		if (ret == D_AUDIO_OK){
			gBF_Audio_Out_Inst[out_ch].out_port				= out_cfg.audio_out_port;
			gBF_Audio_Out_Inst[out_ch].i2s_cfg.master_slave	= out_cfg.i2s_cfg.master_slave;
			gBF_Audio_Out_Inst[out_ch].i2s_cfg.lr_clock		= out_cfg.i2s_cfg.lr_clock;
			gBF_Audio_Out_Inst[out_ch].i2s_cfg.data_cycle	= out_cfg.i2s_cfg.data_cycle;
			gBF_Audio_Out_Inst[out_ch].i2s_cfg.i2s_format	= out_cfg.i2s_cfg.i2s_format;
			gBF_Audio_Out_Inst[out_ch].i2s_cfg.bit_count	= out_cfg.i2s_cfg.bit_count;
			
			// HDMI Audio Setting
			if (out_cfg.audio_out_port == E_FJ_AUDIO_OUT_PORT_I2S3){
				bf_audio_out_hdmi_config(&out_cfg);
			}
			
			gBF_Audio_Out_Inst[out_ch].set_cfg_flg = D_AUDIO_ON;
			
			(VOID)bf_audio_out_clear_state_change_flg();
			
			ret = bf_audio_out_set_eventflg_init(out_ch);
			if (ret == D_AUDIO_OK){
				ret = bf_audio_out_state_change_wait();
			}
		}
		else {
			BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Config(). Instance not exist. L=%d\n", __LINE__));
			ret = D_AUDIO_ERR;
		}
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Set Channel Type
 *	@param		UCHAR				AudioOut ID
 *	@param		UINT8				Channel Type
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_SetChannelType( UCHAR audio_out_id, UINT8 ch_type )
{
	INT32 ret;
	UINT8 out_ch;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret == D_AUDIO_OK){
		gBF_Audio_Out_Inst[out_ch].in_ch_type = ch_type;
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_SetChannelType(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Set Sampling Rate
 *	@param		UCHAR				AudioOut ID
 *	@param		UINT32				Sampling Rate
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_SetSmplRate( UCHAR audio_out_id, UINT32 smpl_rate )
{
	INT32 ret;
	UINT8 out_ch;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret == D_AUDIO_OK){
		gBF_Audio_Out_Inst[out_ch].in_smpl_rate = smpl_rate;
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_SetSmplRate(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Start Audio Output
 *	@param		UCHAR				AudioOut ID
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Start( UCHAR audio_out_id )
{
	INT32 ret;
	UINT8 out_ch;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret == D_AUDIO_OK){
		if (bf_audio_out_check_dma_available(out_ch) == D_AUDIO_OK){
			ret = bf_audio_out_start_wait_stop_proc(out_ch);
			if (ret == D_AUDIO_OK){
				(VOID)bf_audio_out_clear_state_change_flg();
				
				ret = bf_audio_out_set_eventflg_start(out_ch);
				if (ret == D_AUDIO_OK){
					ret = bf_audio_out_state_change_wait();
				}
			}
		}
		else {
			BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Start(). Not Available HDMAC0. L=%d\n", __LINE__));
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Start(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Start Audio Output(AudioCapture)
 *	@param		UCHAR				AudioOut ID
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Start_Capture( UCHAR audio_out_id )
{
	INT32 ret;
	UINT8 out_ch;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret == D_AUDIO_OK){
		if (bf_audio_out_check_dma_available(out_ch) == D_AUDIO_OK){
			ret = bf_audio_out_start_wait_stop_proc(out_ch);
			if (ret == D_AUDIO_OK){
				gBF_Audio_Out_Inst[out_ch].cap_flg = D_AUDIO_OUT_ON;
				ret = bf_audio_out_set_eventflg_start(out_ch);
			}
		}
		else {
			BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Start_Capture(). Not Available HDMAC0. L=%d\n", __LINE__));
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Start_Capture(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Start Audio Output(Buzzer)
 *	@param		UCHAR				AudioOut ID
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Start_Buzzer( UCHAR audio_out_id )
{
	INT32 ret;
	UINT8 out_ch;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Out_Inst[out_ch].buzzer_flg == D_AUDIO_OUT_ON){
			ret = D_AUDIO_ERR;
		}
		else {
			if (bf_audio_out_check_dma_available(out_ch) == D_AUDIO_OK){
				ret = bf_audio_out_start_wait_stop_proc(out_ch);
				if (ret == D_AUDIO_OK){
					gBF_AudioOut_Buzzer_End_Flg[out_ch]		= D_AUDIO_OUT_OFF;
					gBF_AudioOut_Buzzer_StopFactor[out_ch]	= FJ_AUDIO_CB_OUT_AUTO_STOP;
					gBF_AudioOut_Buzzer_StopReq[out_ch]		= D_AUDIO_OUT_OFF;
					
					if (gBF_Audio_Out_Inst[out_ch].dec_flg == D_AUDIO_OUT_ON){
						bf_audio_out_start_bz_mix(out_ch);
					}
					else {
						gBF_Audio_Out_Inst[out_ch].buzzer_flg = D_AUDIO_OUT_ON;
						
						if (gBF_Audio_Out_Inst[out_ch].cap_flg == D_AUDIO_OUT_OFF){
							ret = bf_audio_out_start_bz_only(out_ch);
						}
					}
				}
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Start_Buzzer(). Not Available HDMAC0. L=%d\n", __LINE__));
			}
		}
	}
	else {
		ret = D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Start Audio Output(AudioDecoder)
 *	@param		UCHAR				AudioOut ID
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Start_Decoder( UCHAR audio_out_id )
{
	UINT8 out_ch;
	INT32 ret;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret == D_AUDIO_OK){
		if (bf_audio_out_check_dma_available(out_ch) == D_AUDIO_OK){
			ret = bf_audio_out_start_wait_stop_proc(out_ch);
			if (ret == D_AUDIO_OK){
				gBF_AudioOut_Dec_End_Flg[out_ch]	= D_AUDIO_OFF;
				gBF_Audio_Out_Inst[out_ch].dec_flg	= D_AUDIO_OUT_ON;
				gBF_AudioOut_Dec_StopFlg[out_ch]	= D_AUDIO_OFF;
				
				if ((gBF_Audio_Out_Inst[out_ch].cap_flg == D_AUDIO_OUT_OFF)		&&
					(gBF_Audio_Out_Inst[out_ch].buzzer_flg == D_AUDIO_OUT_OFF)	){
					
					ret = bf_audio_out_set_eventflg_start(out_ch);
				}
			}
		}
		else {
			BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Start_Decoder(). Not Available HDMAC0. L=%d\n", __LINE__));
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Start_Decoder(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Stop Audio Output
 *	@param		UCHAR				AudioOut ID
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Stop( UCHAR audio_out_id )
{
	UINT8 out_ch;
	INT32 ret;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Out_Inst[out_ch].status != D_AUDIO_OUT_STATUS_STOP){
			(VOID)bf_audio_out_clear_state_change_flg();
			bf_audio_out_request_stop(out_ch);
			
			ret = bf_audio_out_state_change_wait();
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Stop(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Stop Audio Output(Buzzer)
 *	@param		UCHAR				AudioOut ID
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Stop_Buzzer( UCHAR audio_out_id )
{
	UINT8 out_ch;
	INT32 ret;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret == D_AUDIO_OK){
		if ((gBF_Audio_Out_Inst[out_ch].cap_flg == D_AUDIO_OUT_OFF)	&&
			(gBF_Audio_Out_Inst[out_ch].dec_flg == D_AUDIO_OUT_OFF)	){
			
			if (gBF_Audio_Out_Inst[out_ch].status != D_AUDIO_OUT_STATUS_STOP){
				(VOID)bf_audio_out_clear_state_change_flg();
				bf_audio_out_request_stop(out_ch);
			}
		}
		else {
			gBF_AudioOut_Buzzer_StopReq[out_ch] = D_AUDIO_ON;
			gBF_Audio_Out_Inst[out_ch].buzzer_flg = D_AUDIO_OUT_OFF;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Stop_Buzzer(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Stop Audio Output(AudioDecoder)
 *	@param		UCHAR				AudioOut ID
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Stop_Decoder( UCHAR audio_out_id )
{
	UINT8 out_ch;
	INT32 ret;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Out_Inst[out_ch].buzzer_flg == D_AUDIO_OUT_OFF){
			
			gBF_AudioOut_Buzzer_StopReq[out_ch] = audio_out_id;
			
			(VOID)bf_audio_out_clear_state_change_flg();
			
			bf_audio_out_request_stop(out_ch);
		}
		else {
			gBF_Audio_Out_Inst[out_ch].dec_flg = D_AUDIO_OUT_OFF;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Stop_Decoder(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return ret;
}

/**
 *  @brief		Get Status of Audio Output
 *	@param		UCHAR				AudioOut ID
 *	@return		UINT8				Status
 *	@note		None
 *	@attention	None
 */
UINT8 BF_AudioOut_Get_Status( UCHAR audio_out_id )
{
	UINT8 out_ch;
	INT32 ret;
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Get_Status(). Instance not exist. L=%d\n", __LINE__));
		return D_AUDIO_OUT_STATUS_STOP;
	}
	
	return gBF_Audio_Out_Inst[out_ch].status;
}

/**
 *  @brief		Get Flag of Set(or Not Set) Configuration.
 *	@param		UCHAR				AudioOut ID
 *	@return		UINT8				Flag
 *	@note		None
 *	@attention	None
 */
UINT8 BF_AudioOut_Get_ConfigSetFlag( UCHAR audio_out_id )
{
	UINT8 out_ch;
	INT32 ret;
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Get_ConfigSetFlag(). Instance not exist. L=%d\n", __LINE__));
		return D_AUDIO_OFF;
	}
	
	return gBF_Audio_Out_Inst[out_ch].set_cfg_flg;
}

/**
 *  @brief		Get BuzzerMode.
 *	@param		UCHAR					AudioOut ID
 *	@return		E_FJ_AUDIO_WAV_CTL		Buzzer Mode
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Set_BuzzerMode( UCHAR audio_out_id, E_FJ_AUDIO_WAV_CTL mode )
{
	UINT8 out_ch;
	INT32 ret;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Set_BuzzerMode(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		gBF_Audio_Out_Inst[out_ch].bz_mode = mode;
	}
	
	(VOID)bf_audio_out_sig_sem();
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Channel of I2S.
 *	@param		UCHAR					AudioOut ID
 *	@return		E_FJ_AUDIO_OUT_PORT_ENUM	Channel of I2S
 *	@note		None
 *	@attention	None
 */
E_FJ_AUDIO_OUT_PORT_ENUM BF_AudioOut_Get_Config_I2sCh( UCHAR audio_out_id )
{
	UINT8 out_ch;
	INT32 ret;
	
	ret = bf_audio_out_check_inst_enable(audio_out_id, &out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_Get_Config_I2sCh(). Instance not exist. L=%d\n", __LINE__));
		return gBF_Audio_Out_Inst[0].out_port;
	}
	
	return gBF_Audio_Out_Inst[out_ch].out_port;
}


VOID BF_AudioOut_SetTestMode( UCHAR audio_out_id, UINT8 mode )
{
	UINT8 out_ch;
	INT32 ret;
	
	(VOID)bf_audio_out_wai_sem();
	
	ret = bf_audio_out_get_out_ch(audio_out_id, &out_ch);
	if (ret == D_AUDIO_OK){
		gBF_AudioOut_TestMode[out_ch] = mode;
		
		if (mode != 0){
			
			if (gBF_AudioOut_TestMode_StartAddr[out_ch] == 0){
				gBF_AudioOut_TestMode_StartAddr[out_ch] = FJ_DirectPoolAlloc(48 * D_AUDIO_OUT_NUM_SAMPLE * 6 * 10);				// 48kHz, Stereo, 24bitPCM, 10sec(480frame)
				
				if (gBF_AudioOut_TestMode_StartAddr[out_ch] != 0){
					gBF_AudioOut_TestMode_CurrentAddr[out_ch] = gBF_AudioOut_TestMode_StartAddr[out_ch];
					gBF_AudioOut_TestMode_Size[out_ch] = 48 * D_AUDIO_OUT_NUM_SAMPLE * 6 * 10;
				}
				else {
					gBF_AudioOut_TestMode_StartAddr[out_ch] = FJ_DirectPoolAlloc(48 * D_AUDIO_OUT_NUM_SAMPLE * 6 * 5);			// 48kHz, Stereo, 24bitPCM, 5sec(240frame)
					
					if (gBF_AudioOut_TestMode_StartAddr[out_ch] != 0){
						gBF_AudioOut_TestMode_CurrentAddr[out_ch] = gBF_AudioOut_TestMode_StartAddr[out_ch];
						gBF_AudioOut_TestMode_Size[out_ch] = 48 * D_AUDIO_OUT_NUM_SAMPLE * 6 * 5;
					}
					else {
						gBF_AudioOut_TestMode_StartAddr[out_ch] = FJ_DirectPoolAlloc(48 * D_AUDIO_OUT_NUM_SAMPLE * 6 * 3);		// 48kHz, Stereo, 24bitPCM, 3sec(144frame)
						
						if (gBF_AudioOut_TestMode_StartAddr[out_ch] != 0){
							gBF_AudioOut_TestMode_CurrentAddr[out_ch] = gBF_AudioOut_TestMode_StartAddr[out_ch];
							gBF_AudioOut_TestMode_Size[out_ch] = 48 * D_AUDIO_OUT_NUM_SAMPLE * 6 * 3;
						}
						else {
							BF_Debug_Print_Error(("[AUDIO_OUT]Can not start DumpFunction. Can not geet work memory. L=%d\n", __LINE__));
						}
					}
				}
			}
			else {
				gBF_AudioOut_TestMode_CurrentAddr[out_ch] = gBF_AudioOut_TestMode_StartAddr[out_ch];
			}
		}
		else {
			if (gBF_AudioOut_TestMode_StartAddr[out_ch]!= 0){
				FJ_DirectPoolFree(gBF_AudioOut_TestMode_StartAddr[out_ch]);
				gBF_AudioOut_TestMode[out_ch] = 0;
			}
		}
	}
	
	(VOID)bf_audio_out_sig_sem();
}



/****
 Output Process
****/

/**
 *  @brief		Initialize Process.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioOut_OutputInit( UINT8 out_ch )
{
	UCHAR dev_ch;
	INT32 ret;
	INT32 ret_open;
	
	if (gBF_Audio_Out_Inst[out_ch].enable == FJ_AUDIO_DISABLE){
		bf_audio_out_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_OUT]Init Error. Instance not exist. L=%d\n", __LINE__));
		return ;
	}
	
	dev_ch = bf_audio_out_get_i2s_dev_ch(out_ch);
	
	// DeviceDriverConfiguration
	ret = BF_Audio_Auif_Init_AudioMacro();
	if (ret != D_AUDIO_OK){
		bf_audio_out_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_Auif_Init_AudioMacro() Error. L=%d\n", __LINE__));
		return ;
	}
	
	ret_open = BF_Audio_Auif_OpenOutput(dev_ch);
	if (ret_open != D_AUDIO_OK){
		bf_audio_out_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_Auif_OpenOutput() Error. L=%d\n", __LINE__));
		return;
	}
	
	ret = BF_Audio_Auif_Set_I2sOut_Config(dev_ch, &gBF_Audio_Out_Inst[out_ch].i2s_cfg);
	
	ret_open = BF_Audio_Auif_CloseOutput(dev_ch);
	if (ret_open != D_AUDIO_OK){
		// BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_Auif_CloseOutput() Error. L=%d\n", __LINE__));
		;
	}
	
	if (ret != D_AUDIO_OK){
		bf_audio_out_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_Auif_Init_AudioMacro() Error. L=%d\n", __LINE__));
		return ;
	}
	
	gBF_Audio_Out_Inst[out_ch].status = D_AUDIO_OUT_STATUS_INIT;
	
	bf_audio_out_set_state_change_flg();
}

/**
 *  @brief		Start Process.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioOut_OutputStart( UINT8 out_ch )
{
	UINT8 dev_ch;
	INT32 ret;
	
//	BF_Debug_Print_Debug(("BF_AudioOut_OutputStart(%d)\n",out_ch));
	
	if (gBF_Audio_Out_Inst[out_ch].enable == FJ_AUDIO_DISABLE){
		bf_audio_out_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_OUT]OutPut Start Error. Instance not exist. L=%d\n", __LINE__));
		return;
	}
	
	gBF_Audio_Out_Inst[out_ch].output_count = 0;
	
	if (gBF_Audio_Out_Inst[out_ch].cap_flg == D_AUDIO_OUT_ON){
		// Output Data from AudioCapture
		ret = bf_audio_out_output_start_dma_cap(out_ch);
		if (ret != D_AUDIO_OK){
			bf_audio_out_set_state_error_flg();
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_cap(). L=%d\n", __LINE__));
			gBF_Audio_Out_Inst[out_ch].cap_flg = D_AUDIO_OUT_OFF;
			return;
		}
	}
	else if (gBF_Audio_Out_Inst[out_ch].dec_flg == D_AUDIO_OUT_ON){
		/* Output Data form AudioDecoder. */
		ret = bf_audio_out_output_start_dma_dec(out_ch);
		if (ret != D_AUDIO_OK){
			bf_audio_out_set_state_error_flg();
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_dec(). L=%d\n", __LINE__));
			gBF_Audio_Out_Inst[out_ch].dec_flg = D_AUDIO_OUT_OFF;
			return;
		}
	}
	else {
		// Output Data from buzzer
		ret = bf_audio_out_output_start_dma_bz(out_ch);
		if (ret != D_AUDIO_OK){
			bf_audio_out_set_state_error_flg();
			BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_output_start_bz(). L=%d\n", __LINE__));
			gBF_Audio_Out_Inst[out_ch].buzzer_flg = D_AUDIO_OUT_OFF;
			return;
		}
	}
	
	gBF_Audio_Out_Inst[out_ch].status = D_AUDIO_OUT_STATUS_START;
	
	if (gBF_Audio_Out_Inst[out_ch].out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		BF_Audio_Auif_Output_Start(D_AUDIO_AUIF_CH0_CH1);
	}
	else {
		dev_ch = bf_audio_out_get_i2s_dev_ch(out_ch);
		BF_Audio_Auif_Output_Start(dev_ch);
	}
	
	bf_audio_out_set_state_change_flg();
}

/**
 *  @brief		Stop Process.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioOut_OutputStop( UINT8 out_ch )
{
	UINT8 dev_ch;
	INT32 ret;
	OS_USER_ER ercd;
	UINT32 buzzer_flg;

//	BF_Debug_Print_Debug(("BF_AudioOut_OutputStop(%d)\n",out_ch));
	
	if (gBF_Audio_Out_Inst[out_ch].enable == FJ_AUDIO_DISABLE){
		gBF_AudioOut_StopProcessing_Id[out_ch] = D_AUDIO_OUT_INVALID;
		bf_audio_out_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_AudioOut_OutputStop. Instance not exist. L=%d\n", __LINE__));
		return;
	}
	
	if (gBF_Audio_Out_Inst[out_ch].status == D_AUDIO_OUT_STATUS_STOP){
		gBF_AudioOut_StopProcessing_Id[out_ch] = D_AUDIO_OUT_INVALID;
		bf_audio_out_set_state_change_flg();
		return;
	}
	
	gBF_Audio_Out_Inst[out_ch].status = D_AUDIO_OUT_STATUS_STOP;
	
	ret = BF_Audio_DMA_Stop(gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID0]);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_DMA_Stop Error L=%d\n", __LINE__));
	}
	
	if (gBF_Audio_Out_Inst[out_ch].out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		ret = BF_Audio_DMA_Stop(gBF_Audio_Out_Inst[out_ch].dma_id[D_AUDIO_OUT_DMA_ID1]);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_DMA_Stop Error L=%d\n", __LINE__));
		}
	}
	
	dev_ch = bf_audio_out_get_i2s_dev_ch(out_ch);
	
	if (gBF_Audio_Out_Inst[out_ch].out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		BF_Audio_Auif_Output_Stop(D_AUDIO_AUIF_CH0_CH1);
	}
	else {
		BF_Audio_Auif_Output_Stop(dev_ch);
	}
	
	ret = BF_Audio_Auif_CloseOutput(dev_ch);
	if (ret != D_AUDIO_OK){
		// BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_Auif_CloseOutput() Error. L=%d\n", __LINE__));
		;
	}
	
	if (gBF_Audio_Out_Inst[out_ch].out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		ret = BF_Audio_Auif_CloseOutput(D_AUDIO_AUIF_CH1);
		if (ret != D_AUDIO_OK){
			// BF_Debug_Print_Error(("[AUDIO_OUT]BF_Audio_Auif_CloseOutput() Error. L=%d\n", __LINE__));
			;
		}
	}
	
	Dd_ARM_Critical_Section_Start( gBF_AudioOut_CS_Bz_Flg );
	
	buzzer_flg = gBF_Audio_Out_Inst[out_ch].buzzer_flg;
	
	if (gBF_Audio_Out_Inst[out_ch].buzzer_flg == D_AUDIO_OUT_ON){
		gBF_Audio_Out_Inst[out_ch].buzzer_flg = D_AUDIO_OUT_OFF;
		
		if (gBF_AudioOut_Buzzer_End_Flg[out_ch] == D_AUDIO_OUT_OFF){
			gBF_AudioOut_Buzzer_StopFactor[out_ch] = FJ_AUDIO_CB_OUT_STOP;
		}
		else {
			gBF_AudioOut_Buzzer_StopFactor[out_ch] = FJ_AUDIO_CB_OUT_AUTO_STOP;
		}
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioOut_CS_Bz_Flg );
	
	if( buzzer_flg == D_AUDIO_OUT_ON){
		ercd = OS_User_Set_Flg(FID_AUDIO_OUT, gBF_AudioOut_EventFlag_BzStopNtfy_Tbl[out_ch]);
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
	
	gBF_Audio_Out_Inst[out_ch].cap_flg = D_AUDIO_OUT_OFF;
	gBF_Audio_Out_Inst[out_ch].dec_flg = D_AUDIO_OUT_OFF;
	
	bf_audio_out_testmode_stop(out_ch);
	
	gBF_AudioOut_StopProcessing_Id[out_ch] = D_AUDIO_OUT_INVALID;

	bf_audio_out_set_state_change_flg();
}

/**
 *  @brief		Make Send Pcm Data.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioOut_PcmSend( UINT8 out_ch )
{
	T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM param;
	INT32 ret;
	
	bf_audio_out_wait_intr_multi_ch(out_ch);
	
	ret = bf_audio_out_renov_pi_buff_out(out_ch);
	if (ret != D_AUDIO_OK) {
		bf_audio_out_request_stop(out_ch);
	}
	else {
		if (gBF_Audio_Out_Inst[out_ch].status != D_AUDIO_OUT_STATUS_STOP){
			
			if ((gBF_Audio_Out_Inst[out_ch].dec_flg != D_AUDIO_OUT_OFF)	 ||
				(gBF_Audio_Out_Inst[out_ch].cap_flg != D_AUDIO_OUT_OFF)	 ||
				(gBF_AudioOut_Buzzer_End_Flg[out_ch] == D_AUDIO_OUT_OFF) ){
				
				ret = bf_audio_out_trns_pcm_rx_connect_inst(out_ch, &param);
				if (ret != D_AUDIO_OK){
					BF_Debug_Print_Error(("[AUDIO_OUT]bf_audio_out_trns_pcm_rx_connect_inst() Error. L=%d\n", __LINE__));
				}
			}
		}
	}
}

/**
 *  @brief		Notify Stop of Buzzer.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioOut_NotifyBZStop( UINT8 out_ch )
{
	FJ_Host_AudioOut_StopWavCB(gBF_Audio_Out_Inst[out_ch].audio_out_id, gBF_AudioOut_Buzzer_StopFactor[out_ch]);
}

/**
 *	@brief		Get Instance audio_out_ch
 *	@param		UCHAR			AudioOut ID
 *	@param		UINT8			AudioOut Channel
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Get_OutCh( UCHAR audio_out_id, UINT8* out_ch )
{
	return bf_audio_out_get_out_ch(audio_out_id, out_ch);
}

/**
 *	@brief		Set Output Volume
 *	@param		UCHAR		Audio Out ID
 *	@return		UINT8		volume
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioOut_Set_Volume( UCHAR audio_out_id, UINT8 volume )
{
	INT32 ret;
	UINT8 out_ch;
	UINT8 dev_ch;
	
	ret = bf_audio_out_get_out_ch(audio_out_id, &out_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioOut_Set_Volume(). Instance not exist. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	dev_ch = bf_audio_out_get_i2s_dev_ch(out_ch);
	
	ret = BF_Audio_Auif_Set_OutputGain(dev_ch, volume);
	if (ret == D_AUDIO_OK){
		if (dev_ch == D_AUDIO_AUIF_CH0){
			ret = BF_Audio_Auif_Set_OutputGain(D_AUDIO_AUIF_CH1, volume);
		}
	}
	
	return ret;
}

/**
 *  @brief		Get instance information.
 *	@param		UINT8					audio out channel
 *	@return		T_AUDIO_OUT_INSTANCE	instance information
 *	@note		None
 *	@attention	None
 */
T_AUDIO_OUT_INSTANCE BF_AudioOut_Get_InstanceInfo( UINT8 out_ch )
{
	return gBF_Audio_Out_Inst[out_ch];
}

