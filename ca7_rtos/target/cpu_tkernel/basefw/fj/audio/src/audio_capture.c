/**
 * @file		audio_capture.c
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
#include "audio_capture.h"
#include "audio_auif.h"
#include "audio_dma.h"
#include "audio_out.h"
#include "audio_encoder.h"
#include "audio_data.h"
#include "audio_pdm.h"
#include "dd_arm.h"
#include "sdram_map_define.h"
#include "os_user_custom.h"
#include "debug.h"
#include "mba_if.h"


/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
#define D_AUDIO_CAP_CH_0			(0)
#define D_AUDIO_CAP_CH_1			(1)

#define D_AUDIO_CAP_ON				(1)
#define D_AUDIO_CAP_OFF				(0)
#define D_AUDIO_CAP_INVALID			(0xFFU)

#define D_AUDIO_CAPTURE_NUM_SAMPLE			(1024UL)

#define D_AUDIO_CAPTURE_BUFF_ARRAY			(24UL)
#define D_AUDIO_CAPTURE_PL1_BUFF_ARRAY		(196UL)

// Primary
#define D_AUDIO_CAPTURE_FRAME_SIZE				(D_AUDIO_CAPTURE_NUM_SAMPLE * 4 * 4)

#define D_AUDIO_CAPTURE_ID0_IN_DATA_BUFF_ADDR		(SDRAM_ADR_AUDIO_CAPTURE_AREA_0)
#define D_AUDIO_CAPTURE_ID0_IN_DATA_BUFF_SIZE		(D_AUDIO_CAPTURE_FRAME_SIZE * 13)
#define D_AUDIO_CAPTURE_ID0_PLUGIN0_BUFF_ADDR		(SDRAM_ADR_AUDIO_CAPTURE_AREA_0 + D_AUDIO_CAPTURE_ID0_IN_DATA_BUFF_SIZE)
#define D_AUDIO_CAPTURE_ID0_PLUGIN0_BUFF_SIZE		(D_AUDIO_CAPTURE_FRAME_SIZE * 13)

#define D_AUDIO_CAPTURE_ID0_PLUGIN1_BUFF_ADDR		(SDRAM_ADR_AUDIO_CAPTURE_PLUGIN1_AREA_0)
#define D_AUDIO_CAPTURE_ID0_PLUGIN1_BUFF_SIZE		(SDRAM_SIZ_AUDIO_CAPTURE_PLUGIN1_AREA)

// Secondary
#define D_AUDIO_CAPTURE_ID1_IN_DATA_BUFF_ADDR		(SDRAM_ADR_AUDIO_CAPTURE_AREA_1)
#define D_AUDIO_CAPTURE_ID1_IN_DATA_BUFF_SIZE		(D_AUDIO_CAPTURE_FRAME_SIZE * 13)
#define D_AUDIO_CAPTURE_ID1_PLUGIN0_BUFF_ADDR		(SDRAM_ADR_AUDIO_CAPTURE_AREA_1 + D_AUDIO_CAPTURE_ID1_IN_DATA_BUFF_SIZE)
#define D_AUDIO_CAPTURE_ID1_PLUGIN0_BUFF_SIZE		(D_AUDIO_CAPTURE_FRAME_SIZE * 13)

#define D_AUDIO_CAPTURE_ID1_PLUGIN1_BUFF_ADDR		(SDRAM_ADR_AUDIO_CAPTURE_PLUGIN1_AREA_1)
#define D_AUDIO_CAPTURE_ID1_PLUGIN1_BUFF_SIZE		(SDRAM_SIZ_AUDIO_CAPTURE_PLUGIN1_AREA)


#define D_AUDIO_CAPTURE_PLUGIN1_ON					(1)
#define D_AUDIO_CAPTURE_PLUGIN1_OFF					(0)

#define D_AUDIO_CAP_NUM_AUDIO_PROCESS				(10U)

#define D_AUDIO_CAP_PLUGIN_ADDR_MODE_DIFF			(0)
#define D_AUDIO_CAP_PLUGIN_ADDR_MODE_EQUAL			(1)

#define D_AUDIO_CAP_CON_ENC0						(0)
#define D_AUDIO_CAP_CON_ENC1						(1)
#define D_AUDIO_CAP_NUM_CON_ENC						(2)

#define D_AUDIO_CAP_DMA_ID0							(0)
#define D_AUDIO_CAP_DMA_ID1							(1)

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
typedef struct {
	UINT32				addr;				// address of data
	UINT32				in_size;			// input size of data
	UINT32				out_size;			// output size of data
} T_AUDIO_CAP_BUFF;

typedef struct {
	UINT32				addr;				// address of data
	UINT32				size;				// size of data
} T_AUDIO_CAP_BUFF_PI0;

typedef struct {
	UINT32				addr;				// address of data
	UINT32				size;				// size of data
	UINT8				use_flg;			// use flag
} T_AUDIO_CAP_BUFF_PI1;

typedef struct{
	UINT8	in_pos;									/**< data int postion  */
	UINT8	out_pos_enc[D_AUDIO_CAP_NUM_CON_ENC];	/**< data out postion for audio encoder*/
	UINT8	out_pos_out;							/**< data out postion for audio out*/
	UINT8	count_enc[D_AUDIO_CAP_NUM_CON_ENC];		/**< not read data remain */
	UINT8	count_out;								/**< not read data remain */
	UINT8	data_num;								/**< Number of frame */
}T_AUDIO_CAP_BUFFER_INFO;

typedef VOID (*T_AUDIO_CAP_DMA_CALLBACK)( UCHAR dma_status );

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
static volatile T_AUDIO_CAP_BUFF gBF_AudioCap_Input_Buff[D_AUDIO_CAPTURE_INST_NUM][D_AUDIO_CAPTURE_BUFF_ARRAY];
static volatile T_AUDIO_CAP_BUFF_PI0 gBF_AudioCap_PI0_Buff[D_AUDIO_CAPTURE_INST_NUM][D_AUDIO_CAPTURE_BUFF_ARRAY];
static volatile T_AUDIO_CAP_BUFF_PI1 gBF_AudioCap_PI1_Buff[D_AUDIO_CAPTURE_INST_NUM][D_AUDIO_CAPTURE_PL1_BUFF_ARRAY];
static volatile T_AUDIO_CAP_BUFFER_INFO gBF_AudioCap_Input_Buff_Info[D_AUDIO_CAPTURE_INST_NUM];
static volatile T_AUDIO_CAP_BUFFER_INFO gBF_AudioCap_PI0_Buff_Info[D_AUDIO_CAPTURE_INST_NUM];

static T_AUDIO_CAPTURE_INSTANCE gBF_Audio_Cap_Inst[D_AUDIO_CAPTURE_INST_NUM] =
{
	{
		D_AUDIO_CAPTURE_STATUS_STOP,	// status
		FJ_AUDIO_CAP_ID_0,				// audio_capture_id
		FJ_AUDIO_DISABLE,				// enable
		E_FJ_AUDIO_CAPTURE_0,			// type
		{									// connect_enc_id[]
			D_AUDIO_CAP_INVALID,			// connect_enc_id[0]
			D_AUDIO_CAP_INVALID				// connect_enc_id[1]
		},
		D_AUDIO_CAP_INVALID,			// connect_out_id
		FJ_AUDIO_DISABLE,				// out_start_flg
		E_FJ_AUDIO_IN_PORT_I2S0,		// in_port
		{								// i2s_cfg
			FJ_AUDIO_IF_MASTER,					// master_slave
			D_AUDIO_SAMPLE_RATE_48000,			// lr_clock
			D_AUDIO_I2S_CYCLE_32FS,				// data_cycle
			E_FJ_AUDIO_I2S_Data_FORMAT_LEFT,	// i2s_format
			D_AUDIO_PCM_16_BIT					// bit_count
		},
		FJ_AUDIO_DISABLE,				// plug_in1_enable
		D_AUDIO_SAMPLE_RATE_48000,		// in_smpl_rate
		D_AUDIO_SAMPLE_RATE_48000,		// out_smpl_rate
		FJ_AUDIO_NUM_CHANNEL_2,			// in_ch_type
		FJ_AUDIO_NUM_CHANNEL_2,			// out_ch_type
		0,								// in_out_addr_mode
		{									// dma_id[]
			D_AUDIO_CAP_INVALID,			// dma_id[0]
			D_AUDIO_CAP_INVALID,			// dma_id[1] for 4ch
		},
		0								// input_count
	},
	{
		D_AUDIO_CAPTURE_STATUS_STOP,	// status
		FJ_AUDIO_CAP_ID_1,				// audio_capture_id
		FJ_AUDIO_DISABLE,				// enable
		E_FJ_AUDIO_CAPTURE_1,			// type
		{									// connect_enc_id[]
			D_AUDIO_CAP_INVALID,			// connect_enc_id[0]
			D_AUDIO_CAP_INVALID				// connect_enc_id[1]
		},
		D_AUDIO_CAP_INVALID,			// connect_out_id
		FJ_AUDIO_DISABLE,				// out_start_flg
		E_FJ_AUDIO_IN_PORT_I2S2,		// in_port
		{								// i2s_cfg
			FJ_AUDIO_IF_MASTER,					// master_slave
			D_AUDIO_SAMPLE_RATE_48000,			// lr_clock
			D_AUDIO_I2S_CYCLE_32FS,				// data_cycle
			E_FJ_AUDIO_I2S_Data_FORMAT_LEFT,	// i2s_format
			D_AUDIO_PCM_16_BIT					// bit_count
		},
		FJ_AUDIO_DISABLE,				// plug_in1_enable
		D_AUDIO_SAMPLE_RATE_48000,		// in_smpl_rate
		D_AUDIO_SAMPLE_RATE_48000,		// out_smpl_rate
		FJ_AUDIO_NUM_CHANNEL_2,			// in_ch_type
		FJ_AUDIO_NUM_CHANNEL_2,			// out_ch_type
		0,								// in_out_addr_mode
		{									// dma_id[]
			D_AUDIO_CAP_INVALID,			// dma_id[0]
			D_AUDIO_CAP_INVALID,			// dma_id[1] for 4ch
		},
		0								// input_count
	}
};

static volatile UINT32 gBF_AudioCap_StartEncBuffAddr[D_AUDIO_CAPTURE_INST_NUM][2];
static volatile UINT8 gBF_AudioCap_EncStart_Flg[D_AUDIO_CAPTURE_INST_NUM][2];
static volatile UINT8 gBF_AudioCap_TestMode = 0;

static UINT32 gBF_AudioCap_Work[D_AUDIO_CAPTURE_NUM_SAMPLE * 4] __attribute__((aligned(64)));

static volatile T_FJ_AUDIO_CAP_PLUGIN1_CB_PARAM gBF_AudioCap_PI1_Param[D_AUDIO_CAPTURE_INST_NUM];

static volatile UINT32 gBF_AudioCap_Pi0Buf_Offset[D_AUDIO_CAPTURE_INST_NUM] = {0};

// Critical Section
// for gBF_AudioCap_Input_Buff_Info[].count_out
static ULONG gBF_AudioCap_CS_Ib_CntOut __attribute__((section(".LOCK_SECTION"), aligned(64)));

// for gBF_AudioCap_Input_Buff_Info[].count_enc[]
static ULONG gBF_AudioCap_CS_Ib_CntEnc __attribute__((section(".LOCK_SECTION"), aligned(64)));

// for gBF_AudioCap_PI0_Buff_Info[].count_out
static ULONG gBF_AudioCap_CS_Pi0_CntOut __attribute__((section(".LOCK_SECTION"), aligned(64)));

// for gBF_AudioCap_PI0_Buff_Info[].count_enc[]
static ULONG gBF_AudioCap_CS_Pi0_CntEnc __attribute__((section(".LOCK_SECTION"), aligned(64)));

// EventFlag Table
static const OS_USER_FLGPTN gBF_AudioCap_EventFlag_PcmRcv_Tbl[D_AUDIO_CAPTURE_INST_NUM] = {
	FLG_AUDIO_CAP_IN0_PCM_RCV,
	FLG_AUDIO_CAP_IN1_PCM_RCV
};

static const OS_USER_FLGPTN gBF_AudioCap_EventFlag_PcmRcv2nd_Tbl[D_AUDIO_CAPTURE_INST_NUM] = {
	FLG_AUDIO_CAP_IN0_PCM_RCV_2ND,
	FLG_AUDIO_CAP_IN1_PCM_RCV_2ND
};


/*----------------------------------------------------------------------*/
/* Local Function Prototype Definition                                  */
/*----------------------------------------------------------------------*/
static INT32 bf_audio_cap_wai_sem( VOID );
static INT32 bf_audio_cap_sig_sem( VOID );
static INT32 bf_audio_cap_clear_state_change_flg( VOID );
static INT32 bf_audio_cap_state_change_wait( VOID );
static VOID bf_audio_cap_set_state_change_flg( VOID );
static VOID bf_audio_cap_set_state_error_flg( VOID );
static INT32 bf_audio_cap_init_buffer( UINT8 cap_ch );
static UINT8 bf_audio_cap_get_inputbuff_in_pos( UINT8 cap_ch );
static INT32 bf_audio_cap_renov_inputbuff_in( UINT8 cap_ch );
static INT32 bf_audio_cap_renov_inputbuff_out_all( UINT8 cap_ch);
static INT32 bf_audio_cap_renov_inputbuff_out_enc( UINT8 cap_ch, UINT8 connect_enc );
static INT32 bf_audio_cap_renov_inputbuff_out_out( UINT8 cap_ch );
static UINT32 bf_audio_cap_get_inputbuff_in_addr( UINT8 cap_ch );
static UINT32 bf_audio_cap_get_inputbuff_in_size( UINT8 cap_ch );
static UINT32 bf_audio_cap_get_inputbuff_out_enc_addr( UINT8 cap_ch, UINT8 connect_enc );
static UINT32 bf_audio_cap_get_inputbuff_out_enc_size( UINT8 cap_ch, UINT8 connect_enc );
static UINT32 bf_audio_cap_get_inputbuff_out_out_addr( UINT8 cap_ch );
static UINT32 bf_audio_cap_get_inputbuff_out_out_size( UINT8 cap_ch );
static UINT32 bf_audio_cap_get_inputbuff_out_out_adj_size( UINT8 cap_ch );
static VOID bf_audio_cap_set_inputbuff_out_out_adj_size( UINT8 cap_ch, UINT32 size );
static INT32 bf_audio_cap_renov_pi0_buff_in( UINT8 cap_ch, UINT8 cnt );
static INT32 bf_audio_cap_renov_pi0_buff_out_enc( UINT8 cap_ch, UINT8 connect_enc, UINT8 cnt );
static INT32 bf_audio_cap_renov_pi0_buff_out_out( UINT8 cap_ch, UINT8 cnt );
static UINT32 bf_audio_cap_get_pi0_buff_in_addr( UINT8 cap_ch );
static UINT32 bf_audio_cap_get_pi0_buff_out_enc_addr( UINT8 cap_ch, UINT8 connect_enc );
static UINT32 bf_audio_cap_get_pi0_buff_out_enc_size( UINT8 cap_ch, UINT8 connect_enc );
static UINT32 bf_audio_cap_get_pi0_buff_out_out_addr( UINT8 cap_ch );
static UINT32 bf_audio_cap_get_pi0_buff_out_out_size( UINT8 cap_ch );
static UINT8 bf_audio_cap_get_pi1_buff_index( UINT8 cap_ch );
static INT32 bf_audio_cap_free_pi1_buff( UINT8 cap_ch, UINT8 free_index );
static INT32 bf_audio_cap_plugin0_cb( UINT8 cap_ch, T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM* param );
static VOID bf_audio_cap_plugin1_cb( UINT8 cap_ch, T_FJ_AUDIO_CAP_PLUGIN1_CB_PARAM* param );
static INT32 bf_audio_cap_get_cap_ch( UCHAR cap_id, UINT8* cap_ch );
static UINT8 bf_audio_cap_get_dev_ch( UINT8 cap_ch );
static INT32 bf_audio_cap_start_pdm( UINT8 cap_ch );
static INT32 bf_audio_cap_stop_pdm( UINT8 cap_ch );
static INT32 bf_audio_cap_setup_auif( UINT8 cap_ch, UINT8 dev_ch );
static INT32 bf_audio_cap_sort_pcm_data( UINT8 cap_ch, UINT32 data_addr, UINT32 data_size, UINT32* outdata_size );
static INT32 bf_audio_cap_sort_24pcm( UINT8 cap_ch, UINT32 data_addr, UINT32 data_size, UINT32* outdata_size );
static INT32 bf_audio_cap_sort_16pcm( UINT8 cap_ch, UINT32 data_addr, UINT32 data_size, UINT32* outdata_size );
static INT32 bf_audio_cap_sort_8pcm( UINT8 cap_ch, UINT32 data_addr, UINT32 data_size, UINT32* outdata_size );
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_1_enc( UINT8 cap_ch, UINT8 connect_enc, UINT32 inc_val, UINT32 addr );
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_1_out( UINT8 cap_ch, UINT32 inc_val, UINT8 ch_type, UINT32 smpl_rate );
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_1( UINT8 cap_ch, const T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM* param );
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_2_enc( UINT8 cap_ch, UINT8 connect_enc, UINT32 addr );
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_2_out( UINT8 cap_ch, UINT8 ch_type, UINT32 smpl_rate );
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_2( UINT8 cap_ch, const T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM* param );
static UINT32 bf_audio_cap_get_data_reg_addr( UINT8 cap_ch, UINT8 dev_ch );
static INT32 bf_audio_cap_set_dma_param( UINT8 cap_ch, UINT8 dev_ch, T_AUDIO_DMA_START_PARAM* dma_start );
static INT32 bf_audio_cap_set_i2s_audio_if( UINT8 cap_ch );
static VOID bf_audio_cap_tx_plugin1( UINT8 cap_ch, UINT8 index );
static VOID bf_audio_cap_pcm_receive( UINT8 cap_ch );
static VOID bf_audio_cap_pcm_receive_sync_4ch( UINT8 cap_ch );
static INT32 bf_audio_cap_check_prm_sample_rate( UINT32 sample_rate );
static INT32 bf_audio_cap_config_check_param( const T_FJ_AUDIO_CAPTURE_CFG* audio_capture_config );
static INT32 bf_audio_cap_smpl_cfg_check_prm( UINT8 cap_ch, const T_FJ_AUDIO_CAP_SMPL_CFG* audio_smpl_config );
static INT32 bf_audio_cap_check_dma_available( UINT8 cap_ch );
static INT32 bf_audio_cap_check_inst_enable( UCHAR audio_cap_id, UINT8* cap_ch );
static INT32 bf_audio_cap_set_eventflg_init( UINT8 cap_ch );
static INT32 bf_audio_cap_set_eventflg_start( UINT8 cap_ch );
static INT32 bf_audio_cap_set_eventflg_stop( UINT8 cap_ch );

static VOID bf_audio_cap_intr0( UINT8 cap_ch );
static VOID bf_audio_cap_intr1( UINT8 cap_ch );
static VOID bf_audio_cap_intr0_2nd( UINT8 cap_ch );
static VOID bf_audio_cap_intr1_2nd( UINT8 cap_ch );
static VOID bf_audio_cap0_intr0( UCHAR dma_status );
static VOID bf_audio_cap0_intr1( UCHAR dma_status );
static VOID bf_audio_cap0_intr0_2nd( UCHAR dma_status );
static VOID bf_audio_cap0_intr1_2nd( UCHAR dma_status );
static VOID bf_audio_cap1_intr0( UCHAR dma_status );
static VOID bf_audio_cap1_intr1( UCHAR dma_status );
static VOID bf_audio_cap1_intr0_2nd( UCHAR dma_status );
static VOID bf_audio_cap1_intr1_2nd( UCHAR dma_status );

static VOID bf_audio_capture_input0_init(VOID);
static VOID bf_audio_capture_input1_init(VOID);
static VOID bf_audio_capture_input0_start(VOID);
static VOID bf_audio_capture_input1_start(VOID);
static VOID bf_audio_capture_input0_stop(VOID);
static VOID bf_audio_capture_input1_stop(VOID);
static VOID bf_audio_capture_input0_plugin(VOID);
static VOID bf_audio_capture_input1_plugin(VOID);
static VOID bf_audio_capture_input0_pcm_receive(VOID);
static VOID bf_audio_capture_input1_pcm_receive(VOID);

/*----------------------------------------------------------------------*/
/* Function Table Definition                                            */
/*----------------------------------------------------------------------*/
static const T_AUDIO_TASK_PROCESS gBF_Audio_Capture_Process[D_AUDIO_CAP_NUM_AUDIO_PROCESS] = {
	{	FLG_AUDIO_CAP_IN0_INIT,			bf_audio_capture_input0_init		},
	{	FLG_AUDIO_CAP_IN1_INIT,			bf_audio_capture_input1_init		},
	{	FLG_AUDIO_CAP_IN0_START,		bf_audio_capture_input0_start		},
	{	FLG_AUDIO_CAP_IN1_START,		bf_audio_capture_input1_start		},
	{	FLG_AUDIO_CAP_IN0_STOP,			bf_audio_capture_input0_stop		},
	{	FLG_AUDIO_CAP_IN1_STOP,			bf_audio_capture_input1_stop		},
	{	FLG_AUDIO_CAP_IN0_PLUGIN,		bf_audio_capture_input0_plugin		},
	{	FLG_AUDIO_CAP_IN1_PLUGIN,		bf_audio_capture_input1_plugin		},
	{	FLG_AUDIO_CAP_IN0_PCM_RCV,		bf_audio_capture_input0_pcm_receive	},
	{	FLG_AUDIO_CAP_IN1_PCM_RCV,		bf_audio_capture_input1_pcm_receive	},
};

// DMA Callback Table
static const T_AUDIO_CAP_DMA_CALLBACK gBf_AudioCap_Dma0_Callback[D_AUDIO_CAPTURE_INST_NUM] = {
	bf_audio_cap0_intr0,
	bf_audio_cap1_intr0
};

static const T_AUDIO_CAP_DMA_CALLBACK gBf_AudioCap_Dma1_Callback[D_AUDIO_CAPTURE_INST_NUM] = {
	bf_audio_cap0_intr1,
	bf_audio_cap1_intr1
};

static const T_AUDIO_CAP_DMA_CALLBACK gBf_AudioCap_Dma0_2nd_Callback[D_AUDIO_CAPTURE_INST_NUM] = {
	bf_audio_cap0_intr0_2nd,
	bf_audio_cap1_intr0_2nd
};

static const T_AUDIO_CAP_DMA_CALLBACK gBf_AudioCap_Dma1_2nd_Callback[D_AUDIO_CAPTURE_INST_NUM] = {
	bf_audio_cap0_intr1_2nd,
	bf_audio_cap1_intr1_2nd
};

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/

/**
 *  @brief		Wait on Semaphore
 *	@param		None
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_wai_sem( VOID )
{
	OS_USER_ER ercd;
	
	//TimeOut:66ms(22*3). To work in a period for 21.3ms(48kHz)
	ercd = OS_User_Twai_Sem(SID_AUDIO_CAP, 100);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]twai_sem error(%d)\n", ercd));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Signal Semaphore
 *	@param		None
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_sig_sem( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Sig_Sem(SID_AUDIO_CAP);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]sig_sem error(%d)\n", ercd));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Clear State Change Flag
 *	@param		None
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_clear_state_change_flg( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Clr_Flg( FID_AUDIO_CAP, ~(FLG_AUDIO_CAP_IN_STATE_CHANGE | FLG_AUDIO_CAP_IN_STATE_ERR) );
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("[AUDIO_CAP]clr_flg error(%d) L=%d\n", ercd, __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Wait State Change
 *	@param		None
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_state_change_wait( VOID )
{
	INT32			retry;
	T_OS_USER_RTSK	rftsk;
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flgptn = 0;
	INT32 return_val = D_AUDIO_OK;
	T_OS_USER_RSEM pk_rsm ={0, 0};
	
	ercd = OS_User_Ref_Sem(SID_AUDIO_CAP, &pk_rsm);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("[AUDIO_CAP]ref_tsk error(%d) L=%d\n", ercd, __LINE__));
	}
	
	if (pk_rsm.semcnt == 0){
		(VOID)bf_audio_cap_sig_sem();
	}
	
	retry = 50;		// wait max 5000ms(100ms * 50count)
	while ( retry > 0 ){
		ercd = OS_User_Ref_Tsk( TID_AUDIO_CAP, &rftsk );
		if( ercd != D_OS_USER_E_OK ){
			BF_Debug_Print_Error(("[AUDIO_CAP]ref_tsk error(%d) L=%d\n", ercd, __LINE__));
			return_val =  D_AUDIO_ERR;
			break;
		}
		
		if( rftsk.tskstat == D_OS_USER_TTS_DMT ){
			BF_Debug_Print_Error(("[AUDIO_CAP]ref_tsk TTS_DMT L=%d\n", __LINE__));
			return_val =  D_AUDIO_ERR;
			break;
		}
		
		ercd = OS_User_Twai_Flg( FID_AUDIO_CAP,
								 (FLG_AUDIO_CAP_IN_STATE_CHANGE | FLG_AUDIO_CAP_IN_STATE_ERR),
								 D_OS_USER_TWF_ORW, &flgptn, 100 );
		
		if( ercd != D_OS_USER_E_OK ){
			BF_Debug_Print_Error(("[AUDIO_CAP]twai_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		else{
			break;
		}
		
		ercd = OS_User_Dly_Tsk(10);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]dly_tsk error(%d) L=%d\n", ercd, __LINE__));
		}
		
		retry--;
	}
	
	if (return_val == D_AUDIO_OK){
		if ((flgptn & FLG_AUDIO_CAP_IN_STATE_CHANGE) != 0){
			ercd = OS_User_Clr_Flg( FID_AUDIO_CAP, ~FLG_AUDIO_CAP_IN_STATE_CHANGE );
			if( ercd != D_OS_USER_E_OK ){
				BF_Debug_Print_Error(("[AUDIO_CAP]clr_flg error(%d) L=%d\n", ercd, __LINE__));
			}
			
			return_val = D_AUDIO_OK;
		}
		
		if ((flgptn & FLG_AUDIO_CAP_IN_STATE_ERR) != 0){
			ercd = OS_User_Clr_Flg( FID_AUDIO_CAP, ~FLG_AUDIO_CAP_IN_STATE_ERR );
			if( ercd != D_OS_USER_E_OK ){
				BF_Debug_Print_Error(("[AUDIO_CAP]clr_flg error(%d) L=%d\n", ercd, __LINE__));
			}
			
			return_val = D_AUDIO_ERR;
		}
	}
	
	if (pk_rsm.semcnt == 0){
		(VOID)bf_audio_cap_wai_sem();
	}
	
	return return_val;
}

/**
 *  @brief		Set State Channge Flag
 *	@param		None
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap_set_state_change_flg( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Set_Flg( FID_AUDIO_CAP, FLG_AUDIO_CAP_IN_STATE_CHANGE );
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]set_flg error(%d) L=%d\n", ercd, __LINE__));
	}
}

/**
 *  @brief		Set State Error Flag
 *	@param		None
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap_set_state_error_flg( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Set_Flg( FID_AUDIO_CAP, FLG_AUDIO_CAP_IN_STATE_ERR );
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]set_flg error(%d) L=%d\n", ercd, __LINE__));
	}
}

/**
 *  @brief		Initialize Buffer of AudioCaptureID0
 *	@param		UINT8			AudioCapture Channel
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_init_buffer( UINT8 cap_ch )
{
	INT32 loop;
	UINT32 in_addr;
	UINT32 in_size;
	UINT32 pl_addr;
	UINT32 pl_size;
	UINT8 byte_cnt;
	UINT32 end_size;
	UINT32 smpl_scale;
	BYTE bit_count;
	UINT8 in_ch_type;
	UINT32 in_smpl_rate;
	UINT32 out_smpl_rate;
	static const UINT32 in_buff_area_addr_tbl[D_AUDIO_CAPTURE_INST_NUM] = {
		D_AUDIO_CAPTURE_ID0_IN_DATA_BUFF_ADDR,
		D_AUDIO_CAPTURE_ID1_IN_DATA_BUFF_ADDR
	};
	static const UINT32 in_buff_area_size_tbl[D_AUDIO_CAPTURE_INST_NUM] = {
		D_AUDIO_CAPTURE_ID0_IN_DATA_BUFF_SIZE,
		D_AUDIO_CAPTURE_ID1_IN_DATA_BUFF_SIZE
	};
	static const UINT32 plug_in0_buff_area_addr_tbl[D_AUDIO_CAPTURE_INST_NUM] = {
		D_AUDIO_CAPTURE_ID0_PLUGIN0_BUFF_ADDR,
		D_AUDIO_CAPTURE_ID1_PLUGIN0_BUFF_ADDR
	};
	static const UINT32 plug_in0_buff_area_size_tbl[D_AUDIO_CAPTURE_INST_NUM] = {
		D_AUDIO_CAPTURE_ID0_PLUGIN0_BUFF_SIZE,
		D_AUDIO_CAPTURE_ID1_PLUGIN0_BUFF_SIZE
	};
	static const UINT32 plug_in1_buff_area_addr_tbl[D_AUDIO_CAPTURE_INST_NUM] = {
		D_AUDIO_CAPTURE_ID0_PLUGIN1_BUFF_ADDR,
		D_AUDIO_CAPTURE_ID1_PLUGIN1_BUFF_ADDR
	};
	static const UINT32 plug_in1_buff_area_size_tbl[D_AUDIO_CAPTURE_INST_NUM] = {
		D_AUDIO_CAPTURE_ID0_PLUGIN1_BUFF_SIZE,
		D_AUDIO_CAPTURE_ID1_PLUGIN1_BUFF_SIZE
	};
	
	// load global data
	bit_count = gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.bit_count;
	in_ch_type = gBF_Audio_Cap_Inst[cap_ch].in_ch_type;
	in_smpl_rate = gBF_Audio_Cap_Inst[cap_ch].in_smpl_rate;
	out_smpl_rate = gBF_Audio_Cap_Inst[cap_ch].out_smpl_rate;
	
	if (bit_count == D_AUDIO_PCM_24_BIT){
		byte_cnt = 4;
	}
	else {
		byte_cnt = 2;
	}
	
	// Input Data Buffer Setting
	// Initialize
	for (loop = 0; loop < D_AUDIO_CAPTURE_BUFF_ARRAY; loop++){
		gBF_AudioCap_Input_Buff[cap_ch][loop].addr		= 0;
		gBF_AudioCap_Input_Buff[cap_ch][loop].in_size	= 0;
		gBF_AudioCap_Input_Buff[cap_ch][loop].out_size	= 0;
	}
	
	in_addr = in_buff_area_addr_tbl[cap_ch];
	
	if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		in_size = D_AUDIO_CAPTURE_NUM_SAMPLE * 4 * byte_cnt;
	}
	else {
		in_size = D_AUDIO_CAPTURE_NUM_SAMPLE * in_ch_type * byte_cnt;
	}
	
	end_size = in_buff_area_size_tbl[cap_ch] - in_size;
	
	for (loop = 0; loop < D_AUDIO_CAPTURE_BUFF_ARRAY; loop++){
		gBF_AudioCap_Input_Buff[cap_ch][loop].addr = in_addr;
		
		if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
			gBF_AudioCap_Input_Buff[cap_ch][loop].in_size	= D_AUDIO_CAPTURE_NUM_SAMPLE * 2 * byte_cnt;
			gBF_AudioCap_Input_Buff[cap_ch][loop].out_size	= D_AUDIO_CAPTURE_NUM_SAMPLE * 2 * byte_cnt;
		}
		else {
			gBF_AudioCap_Input_Buff[cap_ch][loop].in_size	= in_size;
			gBF_AudioCap_Input_Buff[cap_ch][loop].out_size	= in_size;
		}
		
		in_addr += in_size;
		
		if ((in_addr + in_size - in_buff_area_addr_tbl[cap_ch]) > end_size){
			gBF_AudioCap_Input_Buff[cap_ch][loop + 1].addr		= in_addr;
			gBF_AudioCap_Input_Buff[cap_ch][loop + 1].in_size	= in_size;
			gBF_AudioCap_Input_Buff[cap_ch][loop + 1].out_size	= in_size;
			gBF_AudioCap_Input_Buff_Info[cap_ch].data_num		= loop;
			break;
		}
		
		if (loop >= (D_AUDIO_CAPTURE_BUFF_ARRAY - 2)){
			gBF_AudioCap_Input_Buff_Info[cap_ch].data_num = loop;
			break;
		}
	}
	
	if (in_smpl_rate >= out_smpl_rate){
		smpl_scale = 1;
	}
	else {
		smpl_scale = out_smpl_rate / in_smpl_rate;
		
		if ((smpl_scale * in_smpl_rate) != out_smpl_rate){
			smpl_scale++;
		}
	}
	
	if (bit_count == D_AUDIO_PCM_24_BIT){
		byte_cnt = 3;
	}
	else {
		byte_cnt = 2;
	}
	
	// PlugIn0 Buffer Setting
	// Initialize
	for (loop = 0; loop < D_AUDIO_CAPTURE_BUFF_ARRAY; loop++){
		gBF_AudioCap_PI0_Buff[cap_ch][loop].addr = 0;
		gBF_AudioCap_PI0_Buff[cap_ch][loop].size = 0;
	}
	
	pl_addr = plug_in0_buff_area_addr_tbl[cap_ch];
	pl_size = D_AUDIO_CAPTURE_NUM_SAMPLE * gBF_Audio_Cap_Inst[cap_ch].out_ch_type * byte_cnt;
	end_size = plug_in0_buff_area_size_tbl[cap_ch] - (pl_size * smpl_scale);
	
	for (loop = 0; loop < D_AUDIO_CAPTURE_BUFF_ARRAY; loop++){
		gBF_AudioCap_PI0_Buff[cap_ch][loop].addr = pl_addr;
		gBF_AudioCap_PI0_Buff[cap_ch][loop].size = pl_size;
		
		pl_addr += pl_size;
		
		if ((pl_addr + pl_size - plug_in0_buff_area_addr_tbl[cap_ch]) > end_size){
			gBF_AudioCap_PI0_Buff[cap_ch][loop+1].addr	= pl_addr;
			gBF_AudioCap_PI0_Buff[cap_ch][loop+1].size	= pl_size;
			gBF_AudioCap_PI0_Buff_Info[cap_ch].data_num	= loop;
			break;
		}
		
		if (loop >= (D_AUDIO_CAPTURE_BUFF_ARRAY - 2)){
			gBF_AudioCap_PI0_Buff_Info[cap_ch].data_num	= loop;
			gBF_AudioCap_PI0_Buff_Info[cap_ch].data_num	-= gBF_AudioCap_PI0_Buff_Info[cap_ch].data_num % smpl_scale;
			break;
		}
	}
	
	gBF_AudioCap_Pi0Buf_Offset[cap_ch] = 0;
	
	// PlugIn1 Buffer Setting
	// Initialize
	for (loop = 0; loop < D_AUDIO_CAPTURE_PL1_BUFF_ARRAY; loop++){
		gBF_AudioCap_PI1_Buff[cap_ch][loop].addr	= 0;
		gBF_AudioCap_PI1_Buff[cap_ch][loop].size	= 0;
		gBF_AudioCap_PI1_Buff[cap_ch][loop].use_flg	= D_AUDIO_CAPTURE_PLUGIN1_OFF;
	}
	
	pl_addr = plug_in1_buff_area_addr_tbl[cap_ch];
	
	for (loop = 0; loop < D_AUDIO_CAPTURE_PL1_BUFF_ARRAY; loop++){
		gBF_AudioCap_PI1_Buff[cap_ch][loop].addr	= pl_addr;
		gBF_AudioCap_PI1_Buff[cap_ch][loop].size	= pl_size;
		gBF_AudioCap_PI1_Buff[cap_ch][loop].use_flg	= D_AUDIO_CAPTURE_PLUGIN1_OFF;
		
		pl_addr += pl_size;
		
		if (((pl_addr + pl_size - plug_in1_buff_area_addr_tbl[cap_ch]) > plug_in1_buff_area_size_tbl[cap_ch])	||
			(loop >= (D_AUDIO_CAPTURE_PL1_BUFF_ARRAY - 2))														){
			gBF_AudioCap_PI1_Buff[cap_ch][loop + 1].addr	= 0;
			gBF_AudioCap_PI1_Buff[cap_ch][loop + 1].size	= 0;
			gBF_AudioCap_PI1_Buff[cap_ch][loop + 1].use_flg	= D_AUDIO_CAPTURE_PLUGIN1_OFF;
			break;
		}
	}
	
	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Ib_CntEnc );
	gBF_AudioCap_Input_Buff_Info[cap_ch].in_pos		= 0;
	gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_enc[D_AUDIO_CAP_CON_ENC0]	= 0;
	gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC0]	= 0;
	gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_enc[D_AUDIO_CAP_CON_ENC1]	= 0;
	gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC1]	= 0;
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Ib_CntEnc );
	
	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Ib_CntOut );
	gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_out	= 0;
	gBF_AudioCap_Input_Buff_Info[cap_ch].count_out		= 0;
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Ib_CntOut );
	
	gBF_AudioCap_PI0_Buff_Info[cap_ch].in_pos = 0;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Pi0_CntEnc );
	gBF_AudioCap_PI0_Buff_Info[cap_ch].out_pos_enc[D_AUDIO_CAP_CON_ENC0]	= 0;
	gBF_AudioCap_PI0_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC0]		= 0;
	gBF_AudioCap_PI0_Buff_Info[cap_ch].out_pos_enc[D_AUDIO_CAP_CON_ENC1]	= 0;
	gBF_AudioCap_PI0_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC1]		= 0;
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Pi0_CntEnc );
	
	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Pi0_CntOut );
	gBF_AudioCap_PI0_Buff_Info[cap_ch].out_pos_out	= 0;
	gBF_AudioCap_PI0_Buff_Info[cap_ch].count_out	= 0;
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Pi0_CntOut );
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Input Position of PCM Input Buffer
 *	@param		None
 *	@return		UINT8			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static UINT8 bf_audio_cap_get_inputbuff_in_pos( UINT8 cap_ch )
{
	return ((gBF_AudioCap_Input_Buff_Info[cap_ch].in_pos - 2 + gBF_AudioCap_Input_Buff_Info[cap_ch].data_num)
			 % gBF_AudioCap_Input_Buff_Info[cap_ch].data_num);
}

/**
 *  @brief		Renovation of Input Buffer InputPositin
 *	@param		UINT8			AudioCapture Channel
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_renov_inputbuff_in( UINT8 cap_ch )
{
	INT32	retcd;
	
//	printf("[IN]%d\n",gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC0]);

	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Ib_CntOut );
	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Ib_CntEnc );
	
	if ((gBF_AudioCap_Input_Buff_Info[cap_ch].count_out < gBF_AudioCap_Input_Buff_Info[cap_ch].data_num - 1)						&&
		(gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC0] < gBF_AudioCap_Input_Buff_Info[cap_ch].data_num - 1)	&&
		(gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC1] < gBF_AudioCap_Input_Buff_Info[cap_ch].data_num - 1)	){
		
		gBF_AudioCap_Input_Buff_Info[cap_ch].in_pos++;
		gBF_AudioCap_Input_Buff_Info[cap_ch].in_pos %= gBF_AudioCap_Input_Buff_Info[cap_ch].data_num;
		
		gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC0]++;	// counter increment
		gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC1]++;	// counter increment
		gBF_AudioCap_Input_Buff_Info[cap_ch].count_out++;		// counter increment
		retcd = D_AUDIO_OK;
	}
	else {
		// no empty area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Ib_CntEnc );
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Ib_CntOut );
	
	return retcd;
}

/**
 *	@brief		Renovation of Input Buffer OutputPosition for Encoder0
 *	@param		UINT8			AudioCapture Channel
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_renov_inputbuff_out_enc( UINT8 cap_ch, UINT8 connect_enc )
{
	INT32	retcd;
	
//	printf("[OUT%d]%d\n",connect_enc,gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[connect_enc]);
	
	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Ib_CntEnc );
	
	if (gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[connect_enc] > 0){
		gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_enc[connect_enc]++;
		gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_enc[connect_enc] %= gBF_AudioCap_Input_Buff_Info[cap_ch].data_num;
		
		gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[connect_enc]--;	// counter decrement
		retcd = D_AUDIO_OK;
	}
	else {
		// no busy area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Ib_CntEnc );
	
	return retcd;
}

/**
 *  @brief		Renovation of Input Buffer OutputPosition for AudioOut
 *	@param		UINT8			AudioCapture Channel
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_renov_inputbuff_out_out( UINT8 cap_ch )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Ib_CntOut );
	
	if (gBF_AudioCap_Input_Buff_Info[cap_ch].count_out > 0){
		gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_out++;
		gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_out %= gBF_AudioCap_Input_Buff_Info[cap_ch].data_num;
		
		gBF_AudioCap_Input_Buff_Info[cap_ch].count_out--;	// counter decrement
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no busy area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Ib_CntOut );
	
	return retcd;
}

/**
 *  @brief		Renovation of Input Buffer OutputPosition
 *	@param		UINT8			AudioCapture Channel
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_renov_inputbuff_out_all( UINT8 cap_ch)
{
	INT32 ret;
	INT32 retcd = D_AUDIO_OK;
	
	ret = bf_audio_cap_renov_inputbuff_out_enc(cap_ch, D_AUDIO_CAP_CON_ENC0);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_renov_inputbuff_out_enc(%d,%d) Error L=%d\n",cap_ch, D_AUDIO_CAP_CON_ENC0, __LINE__));
		retcd = D_AUDIO_ERR;
	}
	
	ret = bf_audio_cap_renov_inputbuff_out_enc(cap_ch, D_AUDIO_CAP_CON_ENC1);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_renov_inputbuff_out_enc(%d,%d) Error L=%d\n",cap_ch, D_AUDIO_CAP_CON_ENC1, __LINE__));
		retcd = D_AUDIO_ERR;
	}
	
	ret = bf_audio_cap_renov_inputbuff_out_out(cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_renov_inputbuff_out_out(%d) Error L=%d\n",cap_ch, __LINE__));
		retcd = D_AUDIO_ERR;
	}
	
	return retcd;
}

/**
 *  @brief		Get Address of PCM InputBuffer Input Position
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Address of PCM InputBuffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_inputbuff_in_addr( UINT8 cap_ch )
{
	return gBF_AudioCap_Input_Buff[cap_ch][gBF_AudioCap_Input_Buff_Info[cap_ch].in_pos].addr;
}

/**
 *  @brief		Get Size of PCM InputBuffer Input Position
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Size of PCM InputBuffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_inputbuff_in_size( UINT8 cap_ch )
{
	return gBF_AudioCap_Input_Buff[cap_ch][gBF_AudioCap_Input_Buff_Info[cap_ch].in_pos].in_size;
}

/**
 *  @brief		Get Address of PCM InputBuffer Output Position for Encoder0
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Address of PCM InputBuffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_inputbuff_out_enc_addr( UINT8 cap_ch, UINT8 connect_enc )
{
	return gBF_AudioCap_Input_Buff[cap_ch][gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_enc[connect_enc]].addr;
}

/**
 *  @brief		Get Size of PCM InputBuffer Output Position for Encoder
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Size of PCM InputBuffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_inputbuff_out_enc_size( UINT8 cap_ch, UINT8 connect_enc )
{
	return gBF_AudioCap_Input_Buff[cap_ch][gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_enc[connect_enc]].out_size;
}

/**
 *	@brief		Get Address of PCM InputBuffer Output Position for AudioOut
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Address of PCM InputBuffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_inputbuff_out_out_addr( UINT8 cap_ch )
{
	return gBF_AudioCap_Input_Buff[cap_ch][gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_out].addr;
}

/**
 *  @brief		Get Size of PCM InputBuffer Output Position for AudioOut
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Size of PCM InputBuffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_inputbuff_out_out_size( UINT8 cap_ch )
{
	return gBF_AudioCap_Input_Buff[cap_ch][gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_out].in_size;
}

/**
 *  @brief		Get Size of PCM InputBuffer Output Position for AudioOut
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Size of PCM InputBuffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_inputbuff_out_out_adj_size( UINT8 cap_ch )
{
	return gBF_AudioCap_Input_Buff[cap_ch][gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_out].out_size;
}

/**
 *  @brief		Get Size of PCM InputBuffer Output Position for AudioOut
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Size of PCM InputBuffer
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap_set_inputbuff_out_out_adj_size( UINT8 cap_ch, UINT32 size )
{
	gBF_AudioCap_Input_Buff[cap_ch][gBF_AudioCap_Input_Buff_Info[cap_ch].out_pos_out].out_size = size;
}

/**
 *  @brief		Renovation of PlugIn0 Buffer InputPositin
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT8			count of increment index
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_renov_pi0_buff_in( UINT8 cap_ch, UINT8 cnt )
{
	INT32	retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Pi0_CntEnc );
	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Pi0_CntOut );
	
	if (((gBF_AudioCap_PI0_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC0] + cnt) < gBF_AudioCap_PI0_Buff_Info[cap_ch].data_num)	&&
		((gBF_AudioCap_PI0_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC1] + cnt) < gBF_AudioCap_PI0_Buff_Info[cap_ch].data_num)	&&
		((gBF_AudioCap_PI0_Buff_Info[cap_ch].count_out + cnt) < gBF_AudioCap_PI0_Buff_Info[cap_ch].data_num)						){
		
		gBF_AudioCap_PI0_Buff_Info[cap_ch].in_pos += cnt;
		gBF_AudioCap_PI0_Buff_Info[cap_ch].in_pos %= gBF_AudioCap_PI0_Buff_Info[cap_ch].data_num;
		
		gBF_AudioCap_PI0_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC0] += cnt;	// counter increment
		gBF_AudioCap_PI0_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC1] += cnt;	// counter increment
		gBF_AudioCap_PI0_Buff_Info[cap_ch].count_out += cnt;							// counter increment
		retcd = D_AUDIO_OK;
	}
	else {
		// no empty area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Pi0_CntOut );
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Pi0_CntEnc );
	
	return retcd;
}

/**
 *	@brief		Renovation of PlugIn0 Buffer OutputPosition for Encoder0
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT8			count of increment index
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_renov_pi0_buff_out_enc( UINT8 cap_ch, UINT8 connect_enc, UINT8 cnt )
{
	INT32	retcd = 0;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Pi0_CntEnc );
	
	if (gBF_AudioCap_PI0_Buff_Info[cap_ch].count_enc[connect_enc] >= cnt){
		gBF_AudioCap_PI0_Buff_Info[cap_ch].out_pos_enc[connect_enc] += cnt;
		gBF_AudioCap_PI0_Buff_Info[cap_ch].out_pos_enc[connect_enc] %= gBF_AudioCap_PI0_Buff_Info[cap_ch].data_num;
		
		gBF_AudioCap_PI0_Buff_Info[cap_ch].count_enc[connect_enc] -= cnt;		// counter decrement
		retcd = D_AUDIO_OK;
	}
	else {
		// no busy area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Pi0_CntEnc );
	
	return retcd;
}

/**
 *  @brief		Renovation of PlugIn0 Buffer OutputPosition for AudioOut
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT8			count of increment index
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_renov_pi0_buff_out_out( UINT8 cap_ch, UINT8 cnt )
{
	INT32	retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioCap_CS_Pi0_CntOut );
	
	if (gBF_AudioCap_PI0_Buff_Info[cap_ch].count_out >= cnt){
		gBF_AudioCap_PI0_Buff_Info[cap_ch].out_pos_out += cnt;
		gBF_AudioCap_PI0_Buff_Info[cap_ch].out_pos_out %= gBF_AudioCap_PI0_Buff_Info[cap_ch].data_num;
		
		gBF_AudioCap_PI0_Buff_Info[cap_ch].count_out -= cnt;	// counter decrement
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no busy area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioCap_CS_Pi0_CntOut );
	
	return retcd;
}

/**
 *  @brief		Get Address of PlugIn0 Buffer Input Position
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Address of PlugIn0 Buffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_pi0_buff_in_addr( UINT8 cap_ch )
{
	return gBF_AudioCap_PI0_Buff[cap_ch][gBF_AudioCap_PI0_Buff_Info[cap_ch].in_pos].addr;
}

/**
 *  @brief		Get Address of PlugIn0 Buffer Output Position for Encoder0
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Address of PlugIn0 Buffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_pi0_buff_out_enc_addr( UINT8 cap_ch, UINT8 connect_enc )
{
	return gBF_AudioCap_PI0_Buff[cap_ch][gBF_AudioCap_PI0_Buff_Info[cap_ch].out_pos_enc[connect_enc]].addr;
}

/**
 *  @brief		Get Size of PlugIn0 Buffer Output Position for Encoder
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Size of PlugIn0 Buffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_pi0_buff_out_enc_size( UINT8 cap_ch, UINT8 connect_enc )
{
	return gBF_AudioCap_PI0_Buff[cap_ch][gBF_AudioCap_PI0_Buff_Info[cap_ch].out_pos_enc[connect_enc]].size;
}

/**
 *	@brief		Get Address of PlugIn0 Buffer Output Position for AudioOut
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Address of PlugIn0 Buffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_pi0_buff_out_out_addr( UINT8 cap_ch )
{
	return gBF_AudioCap_PI0_Buff[cap_ch][gBF_AudioCap_PI0_Buff_Info[cap_ch].out_pos_out].addr;
}

/**
 *	@brief		Get Size of PlugIn0 Buffer Output Position for AudioOut
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Size of PlugIn0 Buffer
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_pi0_buff_out_out_size( UINT8 cap_ch )
{
	return gBF_AudioCap_PI0_Buff[cap_ch][gBF_AudioCap_PI0_Buff_Info[cap_ch].out_pos_out].size;
}

/**
 *	@brief		Get Index of not used PlugIn1 Buffer
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT32			Index of PlugIn1 Buffer
 *	@note		None
 *	@attention	None
 */
static UINT8 bf_audio_cap_get_pi1_buff_index( UINT8 cap_ch )
{
	UINT8	buff_index = D_AUDIO_CAP_INVALID;
	INT32	loop;
	
	for (loop = 0; loop < D_AUDIO_CAPTURE_PL1_BUFF_ARRAY; loop++){
		if (gBF_AudioCap_PI1_Buff[cap_ch][loop].addr == 0){
			break;
		}
		
		if (gBF_AudioCap_PI1_Buff[cap_ch][loop].use_flg == D_AUDIO_CAPTURE_PLUGIN1_OFF){
			buff_index = loop;
			break;
		}
	}
	
	if (buff_index == D_AUDIO_CAP_INVALID){
		BF_Debug_Print_Error(("[AUDIO_CAP]PlugIn1 Buffer FULL L=%d\n", __LINE__));
	}
	
	return buff_index;
}

/**
 *	@brief		Free PlugIn1 Buffer
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT8			index of PlugIn1 Buffer
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_free_pi1_buff( UINT8 cap_ch, UINT8 free_index )
{
	INT32 result;
	
	if (free_index < D_AUDIO_CAPTURE_PL1_BUFF_ARRAY){
		gBF_AudioCap_PI1_Buff[cap_ch][free_index].use_flg = D_AUDIO_CAPTURE_PLUGIN1_OFF;
		result = D_AUDIO_OK;
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_free_pi1_buff() param err. free_index=%d, L=%d\n", free_index, __LINE__));
		result = D_AUDIO_ERR;
	}
	
	return result;
}

/**
 *	@brief		Callback AudioOut PlugIn0
 *	@param		UCHAR							AudioCapture ID
 *	@param		T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM	Callback information
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_plugin0_cb( UINT8 cap_ch, T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM* param )
{
	UINT8 byte_cnt;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_plugin0_cb() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	// False Safe
	param->out_size			 = param->in_size;
	param->out_ch_type		 = gBF_Audio_Cap_Inst[cap_ch].out_ch_type;
	param->out_sampling_rate = gBF_Audio_Cap_Inst[cap_ch].out_smpl_rate;
	param->out_addr			 = bf_audio_cap_get_pi0_buff_in_addr(cap_ch);
	
	if (gBF_AudioCap_Pi0Buf_Offset[cap_ch] > 0){
		param->out_addr += gBF_AudioCap_Pi0Buf_Offset[cap_ch];
	}
	
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)param->in_addr, param->in_size);
	FJ_Host_AudioCapturePlugIn0CB(gBF_Audio_Cap_Inst[cap_ch].audio_capture_id, param);
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)param->out_addr, param->out_size);
	
	// Area of "param->in_addr" is HDMAC Access Area.
	if (param->out_addr != bf_audio_cap_get_pi0_buff_in_addr(cap_ch)){
		memcpy((VOID*)bf_audio_cap_get_pi0_buff_in_addr(cap_ch), (VOID*)param->out_addr, param->out_size);
		param->out_addr = bf_audio_cap_get_pi0_buff_in_addr(cap_ch);
	}
	
	if (param->out_sampling_rate < param->in_sampling_rate){
		gBF_AudioCap_Pi0Buf_Offset[cap_ch] += param->out_size;
		
		byte_cnt = gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.bit_count / 8;
		
		if (((gBF_AudioCap_Pi0Buf_Offset[cap_ch] / param->out_ch_type) / byte_cnt) >= D_AUDIO_CAPTURE_NUM_SAMPLE){
			param->out_size = gBF_AudioCap_Pi0Buf_Offset[cap_ch];
			param->out_addr = bf_audio_cap_get_pi0_buff_in_addr(cap_ch);
			
			gBF_AudioCap_Pi0Buf_Offset[cap_ch] = 0;
			return D_AUDIO_OK;
		}
		else {
			return D_AUDIO_END;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Callback AudioOut PlugIn1
 *	@param		UCHAR							AudioCapture ID
 *	@param		T_FJ_AUDIO_CAP_PLUGIN1_CB_PARAM	Callback information
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap_plugin1_cb( UINT8 cap_ch, T_FJ_AUDIO_CAP_PLUGIN1_CB_PARAM* param )
{
	OS_USER_ER ercd;
	T_OS_USER_RSEM pk_rsm = {0, 0};
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_plugin1_cb() param err. L=%d\n", __LINE__));
		return ;
	}
	
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)param->addr, param->size);
	
	ercd = OS_User_Ref_Sem(SID_AUDIO_CAP, &pk_rsm);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("[AUDIO_CAP]ref_tsk error(%d) L=%d\n", ercd, __LINE__));
	}
	
	if (pk_rsm.semcnt == 0){
		(VOID)bf_audio_cap_sig_sem();
	}
	
	FJ_Host_AudioCapturePlugIn1CB(gBF_Audio_Cap_Inst[cap_ch].audio_capture_id, param);
	
	if (pk_rsm.semcnt == 0){
		(VOID)bf_audio_cap_wai_sem();
	}
}

/**
 *	@brief		Get AudioCapture Channel
 *	@param		UCHAR			AudioCapture ID
 *	@return		INT32			Instance index
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_get_cap_ch( UCHAR cap_id, UINT8* cap_ch )
{
	INT32 ret = D_AUDIO_ERR;
	INT32 loop;
	
	if (cap_ch == NULL){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_get_cap_ch() param err. L=%d\n", __LINE__));
	}
	else {
		for (loop = 0; loop < D_AUDIO_CAPTURE_INST_NUM; loop++){
			if (gBF_Audio_Cap_Inst[loop].audio_capture_id == cap_id){
				*cap_ch = loop;
				ret = D_AUDIO_OK;
				break;
			}
		}
	}
	
	return ret;
}

/**
 *	@brief		Get Instance index
 *	@param		UINT8			AudioCapture Channel
 *	@return		UINT8			AudioIF Macro channel
 *	@note		None
 *	@attention	None
 */
static UINT8 bf_audio_cap_get_dev_ch( UINT8 cap_ch )
{
	UINT8 dev_ch;
	
	switch (gBF_Audio_Cap_Inst[cap_ch].in_port) {
		case E_FJ_AUDIO_IN_PORT_I2S0:
			dev_ch = D_AUDIO_AUIF_CH0;
			break;
			
		case E_FJ_AUDIO_IN_PORT_I2S2:
			dev_ch = D_AUDIO_AUIF_CH2;
			break;
			
		case E_FJ_AUDIO_IN_PORT_I2S4:
			dev_ch = D_AUDIO_AUIF_CH4;
			break;
			
		default :
			dev_ch = D_AUDIO_AUIF_CH0;
			break;
	}
	
	return dev_ch;
}

/**
 *	@brief		Start PDM
 *	@param		UINT8			AudioCapture Channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_start_pdm( UINT8 cap_ch )
{
	UINT8 dev_ch;
	INT32 ret;
	UINT8 pdm_status;
	
	dev_ch = bf_audio_cap_get_dev_ch(cap_ch);
	
	if (dev_ch == D_AUDIO_AUIF_CH4){
		pdm_status = BF_Audio_Pdm_GetStatus(D_AUDIO_PDM_CH0);
		
		if (pdm_status == D_AUDIO_PDM_STAUS_START){
			bf_audio_cap_stop_pdm(cap_ch);
		}
		
		BF_Audio_Pdm_Init_PdmMacro();
		
		ret = BF_Audio_Pdm_Open(D_AUDIO_PDM_CH0);
		if (ret != D_AUDIO_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Pdm_Open Error. pdmch=%d, L=%d\n", D_AUDIO_PDM_CH0, __LINE__));
			return D_AUDIO_ERR;
		}
		
		ret = BF_Audio_Pdm_Config(D_AUDIO_PDM_CH0, gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.lr_clock);
		if (ret != D_AUDIO_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Pdm_Config Error. pdmch=%d, L=%d\n", D_AUDIO_PDM_CH0, __LINE__));
			
			(VOID)BF_Audio_Pdm_Close(D_AUDIO_PDM_CH0);
			return D_AUDIO_ERR;
		}
		
		ret = BF_Audio_Pdm_Start(D_AUDIO_PDM_CH0);
		if (ret != D_AUDIO_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Pdm_Start Error. pdmch=%d, L=%d\n", D_AUDIO_PDM_CH0, __LINE__));
			
			(VOID)BF_Audio_Pdm_Close(D_AUDIO_PDM_CH0);
			return D_AUDIO_ERR;
		}
		
		if (gBF_Audio_Cap_Inst[cap_ch].in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
			ret = BF_Audio_Pdm_Open(D_AUDIO_PDM_CH1);
			if (ret != D_AUDIO_OK) {
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Pdm_Open Error. pdmch=%d, L=%d\n", D_AUDIO_PDM_CH1, __LINE__));
				return D_AUDIO_ERR;
			}
			
			ret = BF_Audio_Pdm_Config(D_AUDIO_PDM_CH1, gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.lr_clock);
			if (ret != D_AUDIO_OK) {
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Pdm_Config Error. pdmch=%d, L=%d\n", D_AUDIO_PDM_CH1, __LINE__));
				
				(VOID)BF_Audio_Pdm_Close(D_AUDIO_PDM_CH0);
				(VOID)BF_Audio_Pdm_Close(D_AUDIO_PDM_CH1);
				return D_AUDIO_ERR;
			}
			
			ret = BF_Audio_Pdm_Start(D_AUDIO_PDM_CH1);
			if (ret != D_AUDIO_OK) {
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Pdm_Start Error. pdmch=%d, L=%d\n", D_AUDIO_PDM_CH1, __LINE__));
				
				(VOID)BF_Audio_Pdm_Close(D_AUDIO_PDM_CH0);
				(VOID)BF_Audio_Pdm_Close(D_AUDIO_PDM_CH1);
				return D_AUDIO_ERR;
			}
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Stop PDM
 *	@param		UINT8			AudioCapture Channel
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_stop_pdm( UINT8 cap_ch )
{
	UINT8 dev_ch;
	INT32 ret = D_AUDIO_OK;
	UINT8 pdm_status;
	
	dev_ch = bf_audio_cap_get_dev_ch(cap_ch);
	
	if (dev_ch == D_AUDIO_AUIF_CH4){
		ret = BF_Audio_Pdm_Stop(D_AUDIO_PDM_CH0);
		if (ret != D_AUDIO_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Pdm_Stop Error. pdmch=%d, L=%d\n", D_AUDIO_PDM_CH0, __LINE__));
		}
		
		ret = BF_Audio_Pdm_Close(D_AUDIO_PDM_CH0);
		if (ret != D_AUDIO_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Pdm_Close Error. pdmch=%d, L=%d\n", D_AUDIO_PDM_CH0, __LINE__));
		}
		
		pdm_status = BF_Audio_Pdm_GetStatus(D_AUDIO_PDM_CH1);
		
		if (pdm_status == D_AUDIO_PDM_STAUS_START){
			ret = BF_Audio_Pdm_Stop(D_AUDIO_PDM_CH1);
			if (ret != D_AUDIO_OK) {
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Pdm_Stop Error. pdmch=%d, L=%d\n", D_AUDIO_PDM_CH1, __LINE__));
			}
			
			ret = BF_Audio_Pdm_Close(D_AUDIO_PDM_CH1);
			if (ret != D_AUDIO_OK) {
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Pdm_Close Error. pdmch=%d, L=%d\n", D_AUDIO_PDM_CH1, __LINE__));
			}
		}
	}
	
	return ret;
}

/**
 *	@brief		Setup AudioIF Macro Wrapper
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT8			AudioIF Macro Channel
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_setup_auif( UINT8 cap_ch, UINT8 dev_ch )
{
	INT32 ret;
	BYTE bit_count;
	UINT8 in_ch_type;
	
	// Load global data
	bit_count = gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.bit_count;
	in_ch_type = gBF_Audio_Cap_Inst[cap_ch].in_ch_type;
	
	ret = BF_Audio_Auif_OpenInput(dev_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_OpenInput() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = BF_Audio_Auif_Set_Channel_Config(dev_ch, in_ch_type);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_Set_Channel_Config() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (bit_count != D_AUDIO_PCM_8_BIT){
		ret = BF_Audio_Auif_Input_Ctrl(dev_ch, D_AUDIO_AUIF_AHB_FORMAT_R);
	}
	else {
		ret = BF_Audio_Auif_Input_Ctrl(dev_ch, D_AUDIO_AUIF_AHB_FORMAT_L);
	}
	
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_Input_Ctrl() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (in_ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		ret = BF_Audio_Auif_Set_Dma_ModeIn(dev_ch, E_AUDIO_DMA_MODE_IN_2CH, (D_AUDIO_CAPTURE_NUM_SAMPLE / 2));
	}
	else {
		// FJ_CHANNEL_TYPE_STEREO
		ret = BF_Audio_Auif_Set_Dma_ModeIn(dev_ch, E_AUDIO_DMA_MODE_IN_2CH, D_AUDIO_CAPTURE_NUM_SAMPLE);
	}
	
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_Set_Dma_ModeIn() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	// Swap Setting
	if (bit_count != D_AUDIO_PCM_24_BIT){
		//BF_Audio_Auif_Swap_InputData(dev_ch, FJ_AUDIO_ENABLE);
		// don't swap L/R channel by default
		BF_Audio_Auif_Swap_InputData(dev_ch, FJ_AUDIO_DISABLE);
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Sort PCM Data for PlugIn
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT32			Data Address
 *	@param		UINT32			Data Size
 *	@param		UINT32			Output Data Size
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_sort_pcm_data( UINT8 cap_ch, UINT32 data_addr, UINT32 data_size, UINT32* outdata_size )
{
	INT32 ret;
	BYTE bit_count;
	
	// Load global data
	bit_count = gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.bit_count;
	
	if (bit_count == D_AUDIO_PCM_24_BIT){
		ret = bf_audio_cap_sort_24pcm(cap_ch, data_addr, data_size, outdata_size);
		if (ret != D_AUDIO_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_sort_24pcm() Error. L=%d\n", __LINE__));
		}
	}
	else if (bit_count == D_AUDIO_PCM_16_BIT){
		ret = bf_audio_cap_sort_16pcm(cap_ch,  data_addr, data_size, outdata_size);
		if (ret != D_AUDIO_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_sort_16pcm() Error. L=%d\n", __LINE__));
		}
	}
	else {
		// bit_count == 8
		ret = bf_audio_cap_sort_8pcm(cap_ch, data_addr, data_size, outdata_size);
		if (ret != D_AUDIO_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_sort_8pcm() Error. L=%d\n", __LINE__));
		}
	}
	
	return ret;
}

/**
 *	@brief		Sort 24bitPCM
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT32			Data Address
 *	@param		UINT32			Data Size
 *	@param		UINT32			Output Data Size
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_sort_24pcm( UINT8 cap_ch, UINT32 data_addr, UINT32 data_size, UINT32* outdata_size )
{
	UINT8 in_ch_type;
	
	if (outdata_size == NULL){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_sort_24pcm() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	// Load global data
	in_ch_type = gBF_Audio_Cap_Inst[cap_ch].in_ch_type;
	
	if (in_ch_type == FJ_AUDIO_NUM_CHANNEL_4){
		BF_Audio_Data_Swp_32bitPcm_2chTo4ch((UINT32*)data_addr,
											(UINT32*)(data_addr + data_size),
											gBF_AudioCap_Work,
											data_size);
		
		BF_Audio_Data_4ByteTo3Byte((UINT8*)gBF_AudioCap_Work,
								   (UINT8*)data_addr,
								   (data_size * 2));
		
		*outdata_size = (data_size / 4) * 3 * 2;
	}
	else if (in_ch_type == FJ_AUDIO_NUM_CHANNEL_3){
		BF_Audio_Data_Swp_32bitPcm_2chTo3ch((UINT32*)data_addr,
											(UINT32*)(data_addr + data_size),
											gBF_AudioCap_Work,
											data_size);
		
		BF_Audio_Data_4ByteTo3Byte((UINT8*)gBF_AudioCap_Work,
								   (UINT8*)data_addr,
								   (data_size * 3 / 2 ));
		
		*outdata_size = (((data_size / 2) * 3) / 4) * 3;
	}
	else {
		BF_Audio_Data_4ByteTo3Byte((UINT8*)data_addr,
								   (UINT8*)data_addr,
								   data_size);
		
		*outdata_size = (data_size / 4) * 3 ;
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Sort 16bitPCM
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT32			Data Address
 *	@param		UINT32			Data Size
 *	@param		UINT32			Output Data Size
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_sort_16pcm( UINT8 cap_ch, UINT32 data_addr, UINT32 data_size, UINT32* outdata_size )
{
	if (outdata_size == NULL){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_sort_16pcm() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_Audio_Cap_Inst[cap_ch].in_ch_type == FJ_AUDIO_NUM_CHANNEL_4){
		BF_Audio_Data_Swp_16bitPcm_2chTo4ch((UINT32*)data_addr,
											(UINT32*)(data_addr + data_size),
											gBF_AudioCap_Work,
											data_size);
		
		memcpy((VOID*)data_addr, gBF_AudioCap_Work, (data_size * 2));
		
		*outdata_size = data_size * 2;
	}
	else if (gBF_Audio_Cap_Inst[cap_ch].in_ch_type == FJ_AUDIO_NUM_CHANNEL_3){
		BF_Audio_Data_Swp_16bitPcm_2chTo3ch((UINT16*)data_addr,
											(UINT16*)(data_addr + data_size),
											(UINT16*)gBF_AudioCap_Work,
											data_size);
		
		memcpy((VOID*)data_addr, gBF_AudioCap_Work, (data_size * 3 / 2));
		
		*outdata_size = data_size * 3 / 2;
	}
	else {
		*outdata_size = data_size;
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Sort 16bitPCM
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT32			Data Address
 *	@param		UINT32			Data Size
 *	@param		UINT32			Output Data Size
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_sort_8pcm( UINT8 cap_ch, UINT32 data_addr, UINT32 data_size, UINT32* outdata_size )
{
	if (outdata_size == NULL){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_sort_8pcm() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_Audio_Cap_Inst[cap_ch].in_ch_type == FJ_AUDIO_NUM_CHANNEL_4){
		BF_Audio_Data_Swp_16bitPcm_2chTo4ch((UINT32*)data_addr,
											(UINT32*)(data_addr + data_size),
											gBF_AudioCap_Work,
											data_size);
		
		BF_Audio_Data_Pcm_16to8((INT16*)gBF_AudioCap_Work, (INT8*)data_addr, (data_size / 2), FJ_AUDIO_NUM_CHANNEL_4);
		
		*outdata_size = data_size;
	}
	else if (gBF_Audio_Cap_Inst[cap_ch].in_ch_type == FJ_AUDIO_NUM_CHANNEL_3){
		BF_Audio_Data_Swp_16bitPcm_2chTo3ch((UINT16*)data_addr,
											(UINT16*)(data_addr + data_size),
											(UINT16*)gBF_AudioCap_Work,
											data_size);
		
		BF_Audio_Data_Pcm_16to8((INT16*)gBF_AudioCap_Work, (INT8*)data_addr, (data_size / 2), FJ_AUDIO_NUM_CHANNEL_3);
		
		*outdata_size = data_size * 3 / 4;
	}
	else {
		BF_Audio_Data_Pcm_16to8((INT16*)data_addr,
								(INT8*)data_addr,
								(data_size / 2),
								gBF_Audio_Cap_Inst[cap_ch].in_ch_type);
		
		*outdata_size = data_size / 2;
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		After Plugin Data in case of "src_addr != dst_addr"(Connected Enc0)
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT8			Connected Encoder Channel
 *	@param		UINT32			Data Address
 *	@param		UINT32			Data Size
 *	@param		UINT32			Output Data Size
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_1_enc( UINT8 cap_ch, UINT8 connect_enc, UINT32 inc_val, UINT32 addr )
{
	INT32 ret;
	UCHAR cnnct_enc_id;
	
	cnnct_enc_id = gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[connect_enc];
	
	if (cnnct_enc_id != D_AUDIO_CAP_INVALID){
		// connected audio encoder
		if ((gBF_AudioCap_StartEncBuffAddr[cap_ch][connect_enc] == addr)		||
			(gBF_AudioCap_EncStart_Flg[cap_ch][connect_enc] == D_AUDIO_CAP_ON)	){
			
			gBF_AudioCap_EncStart_Flg[cap_ch][connect_enc] = D_AUDIO_CAP_ON;
			
			ret = BF_AudioEnc_Request_Encode(cnnct_enc_id, inc_val);
			if (ret != D_AUDIO_OK){
				(VOID)bf_audio_cap_renov_pi0_buff_out_enc(cap_ch, connect_enc, inc_val);
			}
		}
		else {
			(VOID)bf_audio_cap_renov_pi0_buff_out_enc(cap_ch, connect_enc, inc_val);
		}
	}
	else {
		(VOID)bf_audio_cap_renov_pi0_buff_out_enc(cap_ch, connect_enc, inc_val);
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		After Plugin Data in case of "src_addr != dst_addr"(Connected Out)
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT32			Data Address
 *	@param		UINT32			Data Size
 *	@param		UINT32			Output Data Size
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_1_out( UINT8 cap_ch, UINT32 inc_val, UINT8 ch_type, UINT32 smpl_rate )
{
	INT32 ret;
	UCHAR cnnct_out_id;
	
	cnnct_out_id = gBF_Audio_Cap_Inst[cap_ch].connect_out_id;
	
	if (cnnct_out_id != D_AUDIO_CAP_INVALID){
		// connected audio out
		if (gBF_Audio_Cap_Inst[cap_ch].out_start_flg == FJ_AUDIO_DISABLE){
			
			gBF_Audio_Cap_Inst[cap_ch].out_start_flg = FJ_AUDIO_ENABLE;
			
			ret = BF_AudioOut_SetChannelType(cnnct_out_id, ch_type);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioOut_SetChannelType Error L=%d\n", __LINE__));
			}
			
			ret = BF_AudioOut_SetSmplRate(cnnct_out_id, smpl_rate);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioOut_SetSmplRate Error L=%d\n", __LINE__));
			}
			
			// Out Start
			BF_AudioOut_Start_Capture(cnnct_out_id);
		}
		else {
			if (BF_AudioOut_Get_Status(cnnct_out_id) == D_AUDIO_OUT_STATUS_STOP){
				
				BF_AudioOut_DisconnectCap(gBF_Audio_Cap_Inst[cap_ch].connect_out_id, gBF_Audio_Cap_Inst[cap_ch].audio_capture_id);
				gBF_Audio_Cap_Inst[cap_ch].connect_out_id = D_AUDIO_CAP_INVALID;
				
				(VOID)bf_audio_cap_renov_pi0_buff_out_out(cap_ch, inc_val);
			}
		}
	}
	else {
		(VOID)bf_audio_cap_renov_pi0_buff_out_out(cap_ch, inc_val);
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		After Plugin Data in case of "src_addr != dst_addr"
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT32			Data Address
 *	@param		UINT32			Data Size
 *	@param		UINT32			Output Data Size
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_1( UINT8 cap_ch, const T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM* param )
{
	UINT32 inc_val;
	
	gBF_Audio_Cap_Inst[cap_ch].in_out_addr_mode = D_AUDIO_CAP_PLUGIN_ADDR_MODE_DIFF;
	
	if ( param->out_size > 0 ){
		BF_Audio_Data_Clean_Flush_Dcache((VOID*)param->out_addr, param->out_size);
		if ( param->out_sampling_rate > param->in_sampling_rate){
			inc_val = param->out_sampling_rate / param->in_sampling_rate;
		}
		else {
			inc_val = 1;
		}
		
		(VOID)bf_audio_cap_renov_pi0_buff_in(cap_ch, inc_val);
		
		if ( param->out_sampling_rate < param->in_sampling_rate){
			(VOID)bf_audio_cap_rcv_pcm_tx_cnnct_inst_1_enc(cap_ch,
														   D_AUDIO_CAP_CON_ENC0,
														   inc_val,
														   gBF_AudioCap_StartEncBuffAddr[cap_ch][D_AUDIO_CAP_CON_ENC0]);
			
			(VOID)bf_audio_cap_rcv_pcm_tx_cnnct_inst_1_enc(cap_ch,
														   D_AUDIO_CAP_CON_ENC1,
														   inc_val,
														   gBF_AudioCap_StartEncBuffAddr[cap_ch][D_AUDIO_CAP_CON_ENC1]);
		}
		else {
			(VOID)bf_audio_cap_rcv_pcm_tx_cnnct_inst_1_enc(cap_ch, D_AUDIO_CAP_CON_ENC0, inc_val, param->in_addr);
			(VOID)bf_audio_cap_rcv_pcm_tx_cnnct_inst_1_enc(cap_ch, D_AUDIO_CAP_CON_ENC1, inc_val, param->in_addr);
		}
		
		(VOID)bf_audio_cap_rcv_pcm_tx_cnnct_inst_1_out(cap_ch, inc_val, param->out_ch_type, param->out_sampling_rate);
	}
	
	(VOID)bf_audio_cap_renov_inputbuff_out_all(cap_ch);
	
	return D_AUDIO_OK;
}

/**
 *	@brief		After Plugin Data in case of "src_addr == dst_addr"(Connected Enc0)
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT32			Data Address
 *	@param		UINT32			Data Size
 *	@param		UINT32			Output Data Size
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_2_enc( UINT8 cap_ch, UINT8 connect_enc, UINT32 addr )
{
	INT32 ret;
	UCHAR cnnct_enc_id;
	
	cnnct_enc_id = gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[connect_enc];
	
	if (cnnct_enc_id != D_AUDIO_CAP_INVALID){
		// connected audio encoder
		if ((gBF_AudioCap_StartEncBuffAddr[cap_ch][connect_enc] == addr)		||
			(gBF_AudioCap_EncStart_Flg[cap_ch][connect_enc] == D_AUDIO_CAP_ON)	){
			
			gBF_AudioCap_EncStart_Flg[cap_ch][connect_enc] = D_AUDIO_CAP_ON;
			
			ret = BF_AudioEnc_Request_Encode(cnnct_enc_id, 1);
			if (ret != D_AUDIO_OK){
				(VOID)bf_audio_cap_renov_inputbuff_out_enc(cap_ch, connect_enc);
			}
		}
		else {
			(VOID)bf_audio_cap_renov_inputbuff_out_enc(cap_ch, connect_enc);
		}
	}
	else {
		(VOID)bf_audio_cap_renov_inputbuff_out_enc(cap_ch, connect_enc);
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		After Plugin Data in case of "src_addr == dst_addr"(Connected Out)
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT32			Data Address
 *	@param		UINT32			Data Size
 *	@param		UINT32			Output Data Size
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_2_out( UINT8 cap_ch, UINT8 ch_type, UINT32 smpl_rate )
{
	INT32 ret;
	UCHAR cnnct_out_id;
	
	cnnct_out_id = gBF_Audio_Cap_Inst[cap_ch].connect_out_id;
	
	if (cnnct_out_id != D_AUDIO_CAP_INVALID){
		// connected audio out
		if (gBF_Audio_Cap_Inst[cap_ch].out_start_flg == FJ_AUDIO_DISABLE){
			
			gBF_Audio_Cap_Inst[cap_ch].out_start_flg = FJ_AUDIO_ENABLE;
			
			ret = BF_AudioOut_SetChannelType(cnnct_out_id, ch_type);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioOut_SetChannelType Error L=%d\n", __LINE__));
			}
			
			ret = BF_AudioOut_SetSmplRate(cnnct_out_id, smpl_rate);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioOut_SetSmplRate Error L=%d\n", __LINE__));
			}
			
			BF_AudioOut_Start_Capture(cnnct_out_id);
		}
		else {
			if (BF_AudioOut_Get_Status(cnnct_out_id) == D_AUDIO_OUT_STATUS_STOP){
				
				BF_AudioOut_DisconnectCap(gBF_Audio_Cap_Inst[cap_ch].connect_out_id,
										  gBF_Audio_Cap_Inst[cap_ch].audio_capture_id);
				
				gBF_Audio_Cap_Inst[cap_ch].connect_out_id = D_AUDIO_CAP_INVALID;
				
				(VOID)bf_audio_cap_renov_inputbuff_out_out(cap_ch);
			}
		}
	}
	else {
		(VOID)bf_audio_cap_renov_inputbuff_out_out(cap_ch);
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		After Plugin Data in case of "src_addr == dst_addr"
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT32			Data Address
 *	@param		UINT32			Data Size
 *	@param		UINT32			Output Data Size
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_rcv_pcm_tx_cnnct_inst_2( UINT8 cap_ch, const T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM* param )
{
	gBF_Audio_Cap_Inst[cap_ch].in_out_addr_mode = D_AUDIO_CAP_PLUGIN_ADDR_MODE_EQUAL;
	
	(VOID)bf_audio_cap_rcv_pcm_tx_cnnct_inst_2_enc(cap_ch, D_AUDIO_CAP_CON_ENC0, param->in_addr);
	(VOID)bf_audio_cap_rcv_pcm_tx_cnnct_inst_2_enc(cap_ch, D_AUDIO_CAP_CON_ENC1, param->in_addr);
	(VOID)bf_audio_cap_rcv_pcm_tx_cnnct_inst_2_out(cap_ch, param->in_ch_type, param->in_sampling_rate);
	
	(VOID)bf_audio_cap_renov_pi0_buff_in(cap_ch, 1);
	(VOID)bf_audio_cap_renov_pi0_buff_out_enc(cap_ch, D_AUDIO_CAP_CON_ENC0, 1);
	(VOID)bf_audio_cap_renov_pi0_buff_out_enc(cap_ch, D_AUDIO_CAP_CON_ENC1, 1);
	(VOID)bf_audio_cap_renov_pi0_buff_out_out(cap_ch, 1);
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Get Address of Input Data Register
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT8			AudioIF Macro Channel
 *	@return		UINT32			Address of Input Data Register
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_cap_get_data_reg_addr( UINT8 cap_ch, UINT8 dev_ch )
{
	UINT32 addr;
	
	if (gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT){
		addr = BF_Audio_Auif_Get_Addr_Reg_AUIDDMAPT(dev_ch);
	}
	else {
		addr = BF_Audio_Auif_Get_Addr_Reg_AUIDLR(dev_ch);
	}
	
	return addr;
}

/**
 *	@brief		Get Address of Input Data Register
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT8			AudioIF Macro Channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_set_dma_param( UINT8 cap_ch, UINT8 dev_ch, T_AUDIO_DMA_START_PARAM* dma_start )
{
	INT32 result;
	
	if (dma_start != NULL){
		dma_start->audio_dev_ch	= dev_ch;
		dma_start->src_address	= bf_audio_cap_get_data_reg_addr(cap_ch, dev_ch);
		dma_start->trans_size	= bf_audio_cap_get_inputbuff_in_size(cap_ch);
		
		if ((dev_ch == D_AUDIO_AUIF_CH1)	||
			(dev_ch == D_AUDIO_AUIF_CH5)	){
			dma_start->dma_id		= gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID1];
			dma_start->dst_address	= bf_audio_cap_get_inputbuff_in_addr(cap_ch) + dma_start->trans_size;
		}
		else {
			dma_start->dma_id		= gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID0];
			dma_start->dst_address	= bf_audio_cap_get_inputbuff_in_addr(cap_ch);
		}
		
		result = D_AUDIO_OK;
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_set_dma_param() param err. L=%d\n", __LINE__));
		result = D_AUDIO_ERR;
	}
	
	return result;
}

/**
 *	@brief		Set I2S Setting
 *	@param		UINT8			AudioCapture Channel
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_set_i2s_audio_if( UINT8 cap_ch )
{
	INT32 ret;
	INT32 ret_open;
	UCHAR dev_ch;
	
	ret = BF_Audio_Auif_Init_AudioMacro();
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_Init_AudioMacro() Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	dev_ch = bf_audio_cap_get_dev_ch(cap_ch);
	
	ret_open = BF_Audio_Auif_OpenInput(dev_ch);
	if (ret_open != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_OpenInput() Error. ch=%d, L=%d\n", dev_ch, __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = BF_Audio_Auif_Set_I2sIn_Config(dev_ch, &gBF_Audio_Cap_Inst[cap_ch].i2s_cfg);
	
	if (ret != D_AUDIO_OK){
		BF_Audio_Auif_CloseInput(dev_ch);
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_Set_I2sIn_Config() Error. ch=%d, L=%d\n", dev_ch, __LINE__));
		return D_AUDIO_ERR;
	}
	
	if ((dev_ch == D_AUDIO_AUIF_CH4)										&&
		(gBF_Audio_Cap_Inst[cap_ch].in_ch_type > FJ_AUDIO_NUM_CHANNEL_2)	){
		
		ret_open = BF_Audio_Auif_OpenInput(D_AUDIO_AUIF_CH5);
		if (ret_open != D_AUDIO_OK){
			BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH4);
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_OpenInput() Error. ch=5, L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
		
		ret = BF_Audio_Auif_Set_I2sIn_Config(D_AUDIO_AUIF_CH5, &gBF_Audio_Cap_Inst[cap_ch].i2s_cfg);
		
		BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH5);
		
		if (ret != D_AUDIO_OK){
			BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH4);
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_Set_I2sIn_Config() Error. ch=5, L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
	}
	
	ret_open = BF_Audio_Auif_CloseInput(dev_ch);
	if (ret_open != D_AUDIO_OK){
		// BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_CloseInput() Error. L=%d\n", __LINE__));
		;
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Set I2S Setting
 *	@param		UINT8			AudioCapture Channel
 *	@param		UINT8			index of PlugIn1 Buffer Table
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap_tx_plugin1( UINT8 cap_ch, UINT8 index )
{
	OS_USER_ER ercd;
	
	// EventFlag Table
	static const OS_USER_FLGPTN event_flg_plugin_tbl[D_AUDIO_CAPTURE_INST_NUM] = {
		FLG_AUDIO_CAP_IN0_PLUGIN,
		FLG_AUDIO_CAP_IN1_PLUGIN
	};
	
	if (index < D_AUDIO_CAPTURE_PL1_BUFF_ARRAY){
		if (gBF_Audio_Cap_Inst[cap_ch].plug_in1_enable == FJ_AUDIO_ENABLE){
			if (index != D_AUDIO_CAP_INVALID){
				
				gBF_AudioCap_PI1_Buff[cap_ch][index].use_flg = D_AUDIO_CAPTURE_PLUGIN1_ON;
				
				gBF_AudioCap_PI1_Param[cap_ch].area_index	 = index;
				gBF_AudioCap_PI1_Param[cap_ch].addr			 = gBF_AudioCap_PI1_Buff[cap_ch][index].addr;
				gBF_AudioCap_PI1_Param[cap_ch].size			 = gBF_AudioCap_PI1_Buff[cap_ch][index].size;
				gBF_AudioCap_PI1_Param[cap_ch].ch_type		 = gBF_Audio_Cap_Inst[cap_ch].in_ch_type;
				gBF_AudioCap_PI1_Param[cap_ch].sampling_rate = gBF_Audio_Cap_Inst[cap_ch].in_smpl_rate;
				
				ercd = OS_User_Set_Flg(FID_AUDIO_CAP, event_flg_plugin_tbl[cap_ch]);
				if (ercd != D_OS_USER_E_OK) {
					BF_Debug_Print_Error(("[AUDIO_CAP]set_flg error(%d) Line=%d\n", ercd, __LINE__));
				}
			}
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_tx_plugin1() param err. index=%d, Line=%d\n", index, __LINE__));
	}
}

/**
 *	@brief		Receive PCM Process for ID 0
 *	@param		UINT8			AudioCapture Channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap_pcm_receive( UINT8 cap_ch )
{
	T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM pi0;
	UINT8 pi1_index;
	INT32 ret;
	UINT32 plugin_size;
	UINT32 inbuff_addr;
	UINT32 inbuff_size;
	UINT8 in_ch_type;
	
	in_ch_type = gBF_Audio_Cap_Inst[cap_ch].in_ch_type;
	
	// Address count update
	ret = bf_audio_cap_renov_inputbuff_in(cap_ch);
	if (ret != D_AUDIO_OK) {
		BF_Debug_Print_Error(("[AUDIO_CAP]Renov InBuff:%d\n", __LINE__));
	}
	
	if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		BF_Audio_Data_Flush_Dcache((VOID*)bf_audio_cap_get_inputbuff_in_addr(cap_ch), (bf_audio_cap_get_inputbuff_in_size(cap_ch) * 2));
	}
	else {
		BF_Audio_Data_Flush_Dcache((VOID*)bf_audio_cap_get_inputbuff_in_addr(cap_ch), bf_audio_cap_get_inputbuff_in_size(cap_ch));
	}
	
//	printf("Flush addr=%x, size=%x\n", bf_audio_cap_get_inputbuff_in_addr(cap_ch), bf_audio_cap_get_inputbuff_in_size(cap_ch));
	
	if (gBF_Audio_Cap_Inst[cap_ch].status != D_AUDIO_CAPTURE_STATUS_STOP){
		inbuff_addr = bf_audio_cap_get_inputbuff_out_out_addr(cap_ch);
		inbuff_size = bf_audio_cap_get_inputbuff_out_out_size(cap_ch);
		
		if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
			BF_Audio_Data_Flush_Dcache((VOID*)inbuff_addr, (inbuff_size * 2));
		}
		else {
			BF_Audio_Data_Flush_Dcache((VOID*)inbuff_addr, inbuff_size);
		}
		
		ret = bf_audio_cap_sort_pcm_data(cap_ch, inbuff_addr, inbuff_size, &plugin_size);
		if (ret != D_AUDIO_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_sort_pcm_data() Error. L=%d\n", __LINE__));
		}
		
		bf_audio_cap_set_inputbuff_out_out_adj_size(cap_ch, plugin_size);
		pi1_index = bf_audio_cap_get_pi1_buff_index(cap_ch);
		
		if (gBF_Audio_Cap_Inst[cap_ch].plug_in1_enable == FJ_AUDIO_ENABLE){
			if (pi1_index != D_AUDIO_CAP_INVALID){
				memcpy((VOID*)gBF_AudioCap_PI1_Buff[cap_ch][pi1_index].addr,
					   (VOID*)inbuff_addr,
					   plugin_size);
			}
		}
		
		pi0.in_addr			 = inbuff_addr;
		pi0.in_size			 = plugin_size;
		pi0.in_ch_type		 = in_ch_type;
		pi0.in_sampling_rate = gBF_Audio_Cap_Inst[cap_ch].in_smpl_rate;
		
		ret = bf_audio_cap_plugin0_cb(cap_ch, &pi0);
		if ((ret == D_AUDIO_OK) || (gBF_AudioCap_TestMode == 1)){
			// TestMode
			if (gBF_AudioCap_TestMode == 1){
				memcpy((VOID*)pi0.out_addr, (VOID*)pi0.in_addr, pi0.in_size);
				pi0.out_size = pi0.in_size;
			}
			
			if ((gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] != D_AUDIO_CAP_INVALID)	||
				(gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] != D_AUDIO_CAP_INVALID)	||
				(gBF_Audio_Cap_Inst[cap_ch].connect_out_id != D_AUDIO_CAP_INVALID)							){
				
				if (pi0.in_addr != pi0.out_addr){
					ret = bf_audio_cap_rcv_pcm_tx_cnnct_inst_1(cap_ch, &pi0);
					if (ret != D_AUDIO_OK){
						BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_rcv_pcm_tx_connect_inst_1 Error L=%d\n", __LINE__));
					}
				}
				else {
					ret = bf_audio_cap_rcv_pcm_tx_cnnct_inst_2(cap_ch, &pi0);
					if (ret != D_AUDIO_OK){
						BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_rcv_pcm_tx_connect_inst_2 Error L=%d\n", __LINE__));
					}
				}
			}
			else {
				// not connected
				(VOID)bf_audio_cap_renov_inputbuff_out_all(cap_ch);
				(VOID)bf_audio_cap_renov_pi0_buff_in(cap_ch, 1);
				(VOID)bf_audio_cap_renov_pi0_buff_out_enc(cap_ch, D_AUDIO_CAP_CON_ENC0, 1);
				(VOID)bf_audio_cap_renov_pi0_buff_out_enc(cap_ch, D_AUDIO_CAP_CON_ENC1, 1);
				(VOID)bf_audio_cap_renov_pi0_buff_out_out(cap_ch, 1);
			}
		}
		else {
			(VOID)bf_audio_cap_renov_inputbuff_out_all(cap_ch);
		}
		
		bf_audio_cap_tx_plugin1(cap_ch, pi1_index);
	}
}

/**
 *	@brief		Receive PCM Process for synchronization of 4ch
 *	@param		UINT8			AudioCapture Channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap_pcm_receive_sync_4ch( UINT8 cap_ch )
{
	OS_USER_FLGPTN flgptn = 0;
	OS_USER_TMO timeout;
	OS_USER_ER ercd;
	
	if (gBF_Audio_Cap_Inst[cap_ch].in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		timeout = D_AUDIO_CAPTURE_NUM_SAMPLE / (gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.lr_clock / (UINT32)1000);
		ercd = OS_User_Twai_Flg( FID_AUDIO_CAP,
								 gBF_AudioCap_EventFlag_PcmRcv2nd_Tbl[cap_ch],
								 D_OS_USER_TWF_ORW, &flgptn,
								 timeout);
		
		if( ercd != D_OS_USER_E_OK ) {
			if( ercd == D_OS_USER_E_TMOUT ) {
				BF_Debug_Print_Error(("[AUDIO_CAP]twai_flg time out(%d) L=%d\n", ercd, __LINE__));
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_CAP]twai_flg error(%d) L=%d\n", ercd, __LINE__));
			}
		}
		
		ercd = OS_User_Clr_Flg(FID_AUDIO_CAP, ~gBF_AudioCap_EventFlag_PcmRcv2nd_Tbl[cap_ch]);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]clr_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
}

/**
 *	@brief		Check Sampling Rate Value
 *	@param		UINT32	Sampling Rate
 *	@return		INT32	Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_check_prm_sample_rate( UINT32 sample_rate )
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
 *	@param		T_FJ_AUDIO_CAPTURE_CFG	Configuration information
 *	@return		INT32					Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_config_check_param( const T_FJ_AUDIO_CAPTURE_CFG* audio_capture_config )
{
	if (audio_capture_config->audio_in_port > E_FJ_AUDIO_IN_PORT_I2S4){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Config(). audio_in_port is out of range(=%d). L=%d\n",
								(INT32)audio_capture_config->audio_in_port, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if ((audio_capture_config->audio_in_port == (E_FJ_AUDIO_IN_PORT_ENUM)1) ||				/* pgr0839 */
		(audio_capture_config->audio_in_port == (E_FJ_AUDIO_IN_PORT_ENUM)3) ){				/* pgr0839 */
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Config(). audio_in_port is out of range(=%d). L=%d\n",
								(INT32)audio_capture_config->audio_in_port, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if (audio_capture_config->i2s_cfg.master_slave > FJ_AUDIO_IF_SLAVE){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Config(). master_slave is out of range(=%d). L=%d\n",
								(INT32)audio_capture_config->i2s_cfg.master_slave, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if (bf_audio_cap_check_prm_sample_rate(audio_capture_config->i2s_cfg.lr_clock) != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Config(). lr_clock is out of range(=%d). L=%d\n",
								(INT32)audio_capture_config->i2s_cfg.lr_clock, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if ((audio_capture_config->i2s_cfg.data_cycle != D_AUDIO_I2S_CYCLE_32FS) &&
		(audio_capture_config->i2s_cfg.data_cycle != D_AUDIO_I2S_CYCLE_64FS) ){
		
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Config(). data_cycle is out of range(=%d). L=%d\n",
								audio_capture_config->i2s_cfg.data_cycle, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if (audio_capture_config->i2s_cfg.i2s_format > E_FJ_AUDIO_I2S_Data_FORMAT_I2S){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Config(). i2s_format is out of range(=%d). L=%d\n",
								(INT32)audio_capture_config->i2s_cfg.i2s_format, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if ((audio_capture_config->i2s_cfg.bit_count != D_AUDIO_PCM_8_BIT)	&&
		(audio_capture_config->i2s_cfg.bit_count != D_AUDIO_PCM_16_BIT) &&
		(audio_capture_config->i2s_cfg.bit_count != D_AUDIO_PCM_24_BIT) ){
		
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Config(). bit_count is out of range(=%d). L=%d\n",
								audio_capture_config->i2s_cfg.bit_count, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if ((audio_capture_config->i2s_cfg.data_cycle == D_AUDIO_I2S_CYCLE_32FS) &&
		(audio_capture_config->i2s_cfg.bit_count == D_AUDIO_PCM_24_BIT)		 ){
		
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Config(). Not Correct Parameter. data_cycle=%d, bit_count=%d L=%d\n",
								audio_capture_config->i2s_cfg.data_cycle, audio_capture_config->i2s_cfg.bit_count, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	// Limitation of PDM
	if (audio_capture_config->audio_in_port == E_FJ_AUDIO_IN_PORT_I2S4){
		
		if (audio_capture_config->i2s_cfg.master_slave != FJ_AUDIO_IF_MASTER){
			BF_Debug_Print_Error(("[AUDIO_CAP][PDM]BF_AudioCapture_Config(). PDM master_slave is out of range(=%d). L=%d\n",
									(INT32)audio_capture_config->i2s_cfg.master_slave, __LINE__));
			
			return D_AUDIO_ERR_PARAM;
		}
		
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		if ((audio_capture_config->i2s_cfg.lr_clock != D_AUDIO_SAMPLE_RATE_32000) &&
			(audio_capture_config->i2s_cfg.lr_clock != D_AUDIO_SAMPLE_RATE_48000) ){
			
			BF_Debug_Print_Error(("[AUDIO_CAP][PDM]BF_AudioCapture_Config(). PDM lr_clock is out of range(=%d). L=%d\n",
									(INT32)audio_capture_config->i2s_cfg.lr_clock, __LINE__));
			
			return D_AUDIO_ERR_PARAM;
		}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	 // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		if ((audio_capture_config->i2s_cfg.lr_clock != D_AUDIO_SAMPLE_RATE_16000) &&
			(audio_capture_config->i2s_cfg.lr_clock != D_AUDIO_SAMPLE_RATE_24000) &&
			(audio_capture_config->i2s_cfg.lr_clock != D_AUDIO_SAMPLE_RATE_32000) &&
			(audio_capture_config->i2s_cfg.lr_clock != D_AUDIO_SAMPLE_RATE_48000) ){
			
			BF_Debug_Print_Error(("[AUDIO_CAP][PDM]BF_AudioCapture_Config(). PDM lr_clock is out of range(=%d). L=%d\n",
									(INT32)audio_capture_config->i2s_cfg.lr_clock, __LINE__));
			
			return D_AUDIO_ERR_PARAM;
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		
		if (audio_capture_config->i2s_cfg.data_cycle != D_AUDIO_I2S_CYCLE_64FS){
			
			BF_Debug_Print_Error(("[AUDIO_CAP][PDM]BF_AudioCapture_Config(). PDM data_cycle is out of range(=%d). L=%d\n",
									audio_capture_config->i2s_cfg.data_cycle, __LINE__));
			
			return D_AUDIO_ERR_PARAM;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Parameter Check of BF_AudioCapture_SampleConfig
 *	@param		UINT8					AudioCapture Channel
 *	@param		T_FJ_AUDIO_CAP_SMPL_CFG	configuration information
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_smpl_cfg_check_prm( UINT8 cap_ch, const T_FJ_AUDIO_CAP_SMPL_CFG* audio_smpl_config )
{
	if (bf_audio_cap_check_prm_sample_rate(audio_smpl_config->in_sampling_rate) != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_SampleConfig(). in_sampling_rate=%ld, L=%d\n",
								audio_smpl_config->in_sampling_rate, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if (bf_audio_cap_check_prm_sample_rate(audio_smpl_config->out_sampling_rate) != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_SampleConfig(). out_sampling_rate=%ld, L=%d\n",
								audio_smpl_config->out_sampling_rate, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if ((audio_smpl_config->in_ch_type > FJ_AUDIO_NUM_CHANNEL_2)		&&
		(gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S2)	){
		
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_SampleConfig(). in_ch_type=%d, inport=%d, L=%d\n",
							audio_smpl_config->in_ch_type, gBF_Audio_Cap_Inst[cap_ch].in_port, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if ( audio_smpl_config->in_ch_type < FJ_AUDIO_NUM_CHANNEL_1){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_SampleConfig(). in_ch_type=%d, L=%d\n",
								audio_smpl_config->in_ch_type, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if ( audio_smpl_config->out_ch_type < FJ_AUDIO_NUM_CHANNEL_1){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_SampleConfig(). out_ch_type=%d, L=%d\n",
								audio_smpl_config->out_ch_type, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Check Number of Dma ID
 *	@param		UINT8			audio out channel
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_check_dma_available( UINT8 cap_ch )
{
	INT32 ret;
	
//	printf("Number of DMA:%d\n", BF_Audio_DMA_Get_RemainNumDmaID());
	
	if (gBF_Audio_Cap_Inst[cap_ch].in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
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

/**
 *	@brief		Check Enable of Instance
 *	@param		UCHAR			audio capture id
 *	@param		UINT8			audio capture channel
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_check_inst_enable( UCHAR audio_cap_id, UINT8* cap_ch )
{
	INT32 ret;
	
	ret = bf_audio_cap_get_cap_ch(audio_cap_id, cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_check_inst_enable(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		if (gBF_Audio_Cap_Inst[*cap_ch].enable == FJ_AUDIO_DISABLE){
			BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_check_inst_enable(). Instance is disable. L=%d\n", __LINE__));
			ret = D_AUDIO_ERR;
		}
	}
	
	return ret;
}

/**
 *	@brief		Set event flag
 *	@param		UINT8		audio capture channel
 *	@return		INT32		OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_set_eventflg_init( UINT8 cap_ch )
{
	INT32 ret;
	OS_USER_ER ercd;
	// EventFlag Table
	static const OS_USER_FLGPTN event_flg_init_tbl[D_AUDIO_CAPTURE_INST_NUM] = {
		FLG_AUDIO_CAP_IN0_INIT,
		FLG_AUDIO_CAP_IN1_INIT
	};
	
	ercd = OS_User_Set_Flg(FID_AUDIO_CAP, event_flg_init_tbl[cap_ch]);
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
 *	@param		UINT8		audio capture channel
 *	@return		INT32		OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_set_eventflg_start( UINT8 cap_ch )
{
	INT32 ret;
	OS_USER_ER ercd;
	// EventFlag Table
	static const OS_USER_FLGPTN event_flg_start_tbl[D_AUDIO_CAPTURE_INST_NUM] = {
		FLG_AUDIO_CAP_IN0_START,
		FLG_AUDIO_CAP_IN1_START
	};
	
	ercd = OS_User_Set_Flg(FID_AUDIO_CAP, event_flg_start_tbl[cap_ch]);
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
 *	@param		UINT8		audio capture channel
 *	@return		INT32		OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_cap_set_eventflg_stop( UINT8 cap_ch )
{
	INT32 ret;
	OS_USER_ER ercd;
	// EventFlag Table
	static const OS_USER_FLGPTN event_flg_stop_tbl[D_AUDIO_CAPTURE_INST_NUM] = {
		FLG_AUDIO_CAP_IN0_STOP,
		FLG_AUDIO_CAP_IN1_STOP
	};
	
	ercd = OS_User_Set_Flg(FID_AUDIO_CAP, event_flg_stop_tbl[cap_ch]);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_OUT]set_flg error(%d) L=%d\n", ercd, __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		ret = D_AUDIO_OK;
	}
	
	return ret;
}

/*********************/
/* Interrupt Handler */
/*********************/

/**
 *  @brief		Audio Caputure Interrupt handler
 *	@param		UINT8 Audio capture channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap_intr0( UINT8 cap_ch )
{
	OS_USER_ER ercd;
	T_AUDIO_DMA_START_PARAM dma_start;
	UINT8 dev_ch;
	
	if (gBF_Audio_Cap_Inst[cap_ch].status != D_AUDIO_CAPTURE_STATUS_STOP){
		gBF_Audio_Cap_Inst[cap_ch].input_count++;
		
		dev_ch = bf_audio_cap_get_dev_ch(cap_ch);
		
		dma_start.dma_id		= gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID0];
		dma_start.audio_dev_ch	= dev_ch;
		dma_start.src_address	= bf_audio_cap_get_data_reg_addr(cap_ch, dev_ch);
		dma_start.dst_address	= bf_audio_cap_get_inputbuff_in_addr(cap_ch);
		dma_start.trans_size	= bf_audio_cap_get_inputbuff_in_size(cap_ch);
		
		// DMA Start
		BF_Audio_DMA_Input_Start0(&dma_start);
		
		ercd = OS_User_Set_Flg(FID_AUDIO_CAP, gBF_AudioCap_EventFlag_PcmRcv_Tbl[cap_ch]);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]set_flg error(%d) Line=%d\n", ercd, __LINE__));
		}
	}
}

 /**
 *  @brief		Audio Caputure Interrupt handler
 *	@param		UINT8 Audio capture channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap_intr1( UINT8 cap_ch )
{
	OS_USER_ER ercd;
	T_AUDIO_DMA_START_PARAM dma_start;
	UINT8 dev_ch;
	
	if (gBF_Audio_Cap_Inst[cap_ch].status != D_AUDIO_CAPTURE_STATUS_STOP){
		gBF_Audio_Cap_Inst[cap_ch].input_count++;
		
		dev_ch = bf_audio_cap_get_dev_ch(cap_ch);
		
		dma_start.dma_id		= gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID0];
		dma_start.audio_dev_ch	= dev_ch;
		dma_start.src_address	= bf_audio_cap_get_data_reg_addr(cap_ch, dev_ch);
		dma_start.dst_address	= bf_audio_cap_get_inputbuff_in_addr(cap_ch);
		dma_start.trans_size	= bf_audio_cap_get_inputbuff_in_size(cap_ch);
		
		// DMA Start
		BF_Audio_DMA_Input_Start1(&dma_start);
		
		ercd = OS_User_Set_Flg(FID_AUDIO_CAP, gBF_AudioCap_EventFlag_PcmRcv_Tbl[cap_ch]);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]set_flg error(%d) Line=%d\n", ercd, __LINE__));
		}
	}
}

/**
 *  @brief		Audio Caputure Interrupt handler for 4ch Capture
 *	@param		UINT8 Audio capture channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap_intr0_2nd( UINT8 cap_ch )
{
	OS_USER_ER ercd;
	T_AUDIO_DMA_START_PARAM dma_start;
	
	if (gBF_Audio_Cap_Inst[cap_ch].status != D_AUDIO_CAPTURE_STATUS_STOP){
		gBF_Audio_Cap_Inst[cap_ch].input_count++;
		
		dma_start.dma_id		= gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID1];
		
		if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
			dma_start.audio_dev_ch	= D_AUDIO_AUIF_CH1;
			dma_start.src_address	= bf_audio_cap_get_data_reg_addr(cap_ch, D_AUDIO_AUIF_CH1);
		}
		else {
			dma_start.audio_dev_ch	= D_AUDIO_AUIF_CH5;
			dma_start.src_address	= bf_audio_cap_get_data_reg_addr(cap_ch, D_AUDIO_AUIF_CH5);
		}
		
		dma_start.trans_size	= bf_audio_cap_get_inputbuff_in_size(cap_ch);
		dma_start.dst_address	= bf_audio_cap_get_inputbuff_in_addr(cap_ch) + dma_start.trans_size;
		
		// DMA Start
		BF_Audio_DMA_Input_Start0(&dma_start);
		
		ercd = OS_User_Set_Flg(FID_AUDIO_CAP, gBF_AudioCap_EventFlag_PcmRcv2nd_Tbl[cap_ch]);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]set_flg error(%d) Line=%d\n", ercd, __LINE__));
		}
	}
}

 /**
 *  @brief		Audio Caputure Interrupt handler
 *	@param		UINT8 Audio capture channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap_intr1_2nd( UINT8 cap_ch )
{
	OS_USER_ER ercd;
	T_AUDIO_DMA_START_PARAM dma_start;
	
	if (gBF_Audio_Cap_Inst[cap_ch].status != D_AUDIO_CAPTURE_STATUS_STOP){
		gBF_Audio_Cap_Inst[cap_ch].input_count++;
		
		dma_start.dma_id		= gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID1];
		
		if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
			dma_start.audio_dev_ch	= D_AUDIO_AUIF_CH1;
			dma_start.src_address	= bf_audio_cap_get_data_reg_addr(cap_ch, D_AUDIO_AUIF_CH1);
		}
		else {
			dma_start.audio_dev_ch	= D_AUDIO_AUIF_CH5;
			dma_start.src_address	= bf_audio_cap_get_data_reg_addr(cap_ch, D_AUDIO_AUIF_CH5);
		}
		
		dma_start.trans_size	= bf_audio_cap_get_inputbuff_in_size(cap_ch);
		dma_start.dst_address	= bf_audio_cap_get_inputbuff_in_addr(cap_ch) + dma_start.trans_size;
		
		// DMA Start
		BF_Audio_DMA_Input_Start1(&dma_start);
		
		ercd = OS_User_Set_Flg(FID_AUDIO_CAP, gBF_AudioCap_EventFlag_PcmRcv2nd_Tbl[cap_ch]);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]set_flg error(%d) Line=%d\n", ercd, __LINE__));
		}
	}
}

/**
 *  @brief		Audio Caputure Interrupt handler
 *	@param		UCHAR dma_status
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap0_intr0( UCHAR dma_status )
{
	bf_audio_cap_intr0(D_AUDIO_CAP_CH_0);
}

 /**
 *  @brief		Audio Caputure Interrupt handler
 *	@param		UCHAR dma_status
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap0_intr1( UCHAR dma_status )
{
	bf_audio_cap_intr1(D_AUDIO_CAP_CH_0);
}


/**
 *  @brief		Audio Caputure Interrupt handler for 4ch Capture
 *	@param		UCHAR dma_status
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap0_intr0_2nd( UCHAR dma_status )
{
	bf_audio_cap_intr0_2nd(D_AUDIO_CAP_CH_0);
}

 /**
 *  @brief		Audio Caputure Interrupt handler
 *	@param		UCHAR dma_status
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap0_intr1_2nd( UCHAR dma_status )
{
	bf_audio_cap_intr1_2nd(D_AUDIO_CAP_CH_0);
}

/**
 *  @brief		Audio Caputure Interrupt handler
 *	@param		UCHAR dma_status
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap1_intr0( UCHAR dma_status )
{
	bf_audio_cap_intr0(D_AUDIO_CAP_CH_1);
}

/**
 *  @brief		Audio Caputure Interrupt handler
 *	@param		UCHAR dma_status
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap1_intr1( UCHAR dma_status )
{
	bf_audio_cap_intr1(D_AUDIO_CAP_CH_1);
}

/**
 *  @brief		Audio Caputure Interrupt handler
 *	@param		UCHAR dma_status
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap1_intr0_2nd( UCHAR dma_status )
{
	bf_audio_cap_intr0_2nd(D_AUDIO_CAP_CH_1);
}

/**
 *  @brief		Audio Caputure Interrupt handler
 *	@param		UCHAR dma_status
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_cap1_intr1_2nd( UCHAR dma_status )
{
	bf_audio_cap_intr1_2nd(D_AUDIO_CAP_CH_1);
}

/****************/
/* Task Process */
/****************/

/**
 *	@brief		AudioCapture Initialize Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_capture_input0_init( VOID )
{
	BF_AudioCapture_InputInit(D_AUDIO_CAP_CH_0);
}

/**
 *	@brief		AudioCapture Initialize Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_capture_input1_init( VOID )
{
	BF_AudioCapture_InputInit(D_AUDIO_CAP_CH_1);
}

/**
 *	@brief		AudioCapture Start Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_capture_input0_start( VOID )
{
	BF_AudioCapture_InputStart(D_AUDIO_CAP_CH_0);
}

/**
 *	@brief		AudioCapture Start Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_capture_input1_start( VOID )
{
	BF_AudioCapture_InputStart(D_AUDIO_CAP_CH_1);
}

/**
 *	@brief		AudioCapture Stop Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_capture_input0_stop( VOID )
{
	BF_AudioCapture_InputStop(D_AUDIO_CAP_CH_0);
}

/**
 *	@brief		AudioCapture Stop Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_capture_input1_stop( VOID )
{
	BF_AudioCapture_InputStop(D_AUDIO_CAP_CH_1);
}

/**
 *	@brief		AudioCapture Stop Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_capture_input0_plugin( VOID )
{
	BF_AudioCapture_TxPlugIn1(D_AUDIO_CAP_CH_0);
}

/**
 *	@brief		AudioCapture Stop Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_capture_input1_plugin( VOID )
{
	BF_AudioCapture_TxPlugIn1(D_AUDIO_CAP_CH_1);
}

/**
 *	@brief		AudioCapture Receive Process(AudioCaptureID 0)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_capture_input0_pcm_receive( VOID )

{
	BF_AudioCapture_PcmReceive(D_AUDIO_CAP_CH_0);
}

/**
 *	@brief		AudioCapture Receive Process(AudioCaptureID 1)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_capture_input1_pcm_receive( VOID )
{
	BF_AudioCapture_PcmReceive(D_AUDIO_CAP_CH_1);
}

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
/**
 *	@brief		Audio Capture Task
 *	@param		INT32	Task Start Code
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Audio_Cap_Process( INT32 stacd )
{
	OS_USER_FLGPTN flgptn = 0;
	ULONG wai_flg_ptn;
	UINT32 loop;
	OS_USER_ER ercd ;
	
	wai_flg_ptn = FLG_AUDIO_CAP_IN0_INIT	|
				  FLG_AUDIO_CAP_IN1_INIT	|
				  FLG_AUDIO_CAP_IN0_START	|
				  FLG_AUDIO_CAP_IN1_START	|
				  FLG_AUDIO_CAP_IN0_STOP	|
				  FLG_AUDIO_CAP_IN1_STOP	|
				  FLG_AUDIO_CAP_IN0_PLUGIN	|
				  FLG_AUDIO_CAP_IN1_PLUGIN	|
				  FLG_AUDIO_CAP_IN0_PCM_RCV	|
				  FLG_AUDIO_CAP_IN1_PCM_RCV	;
	
	while (1){
		ercd = OS_User_Wai_Flg( FID_AUDIO_CAP, wai_flg_ptn, D_OS_USER_TWF_ORW, &flgptn);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]wai_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "f=%lx", flgptn ));
		flgptn &= wai_flg_ptn;
		
		for (loop = 0; loop < D_AUDIO_CAP_NUM_AUDIO_PROCESS; loop++){
			if ((flgptn & gBF_Audio_Capture_Process[loop].event_flag) != 0){
				ercd = OS_User_Clr_Flg(FID_AUDIO_CAP, ~(gBF_Audio_Capture_Process[loop].event_flag));
				if (ercd != D_OS_USER_E_OK) {
					BF_Debug_Print_Error(("[AUDIO_CAP]clr_flg error(%d) L=%d\n", ercd, __LINE__));
				}
				
				(VOID)bf_audio_cap_wai_sem();
				
				gBF_Audio_Capture_Process[loop].process();
				
				(VOID)bf_audio_cap_sig_sem();
			}
		}
		
		M_MBA_PRINTF((D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "" ));
	}
}

/**
 *  @brief		Start Audio Capture Task
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioCapture_TaskStart( VOID )
{
	OS_USER_ER ercd_sta;
	OS_USER_ER ercd_ter;
	OS_USER_ER ercd ;
	T_OS_USER_RTSK pk_rtsk;
	
	OS_User_Ref_Tsk(TID_AUDIO_CAP, &pk_rtsk);
	if (pk_rtsk.tskstat == D_OS_USER_TTS_DMT){
		// clear flag
		ercd = OS_User_Clr_Flg(FID_AUDIO_CAP, 0);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_CAP]clr_flg error(%d) Line=%d\n", ercd, __LINE__));
		}
		
		// Create Task process
		do {
			ercd_sta = OS_User_Sta_Tsk(TID_AUDIO_CAP, 0);
			if (ercd_sta == D_OS_USER_E_OBJ){
				ercd_ter = OS_User_Ter_Tsk(TID_AUDIO_CAP);
				if (ercd_ter != D_OS_USER_E_OK){
					BF_Debug_Print_Information(("[AUDIO_CAP] ter_tsk Error! ercd(%d), Line=%d\n", ercd, __LINE__));
				}
			}
		}
		while (ercd_sta != 0);
	}
}

/**
 *  @brief		Stop Audio Capture Task
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioCapture_TaskStop( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Ter_Tsk(TID_AUDIO_CAP);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Information(("[AUDIO_CAP] ter_tsk Error! ercd(%d), Line=%d\n", ercd, __LINE__));
	}
}

/**
 *	@brief		Open Audio Capture Instance
 *	@param		E_FJ_AUDIO_CAPTURE_ENUM	Audio Capture
 *	@param		UCHAR*			Audio Capture ID
 *	@return		INT32			Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_Open( E_FJ_AUDIO_CAPTURE_ENUM audio_cap, UCHAR* audio_capture_id )
{
	INT32 loop;
	UINT8 ret_flg = D_AUDIO_CAP_OFF;
	INT32 ret_val;
	
	if (audio_capture_id == NULL){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Open() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_cap_wai_sem();
	
	for (loop = 0; loop < D_AUDIO_CAPTURE_INST_NUM; loop++){
		if (gBF_Audio_Cap_Inst[loop].enable == FJ_AUDIO_DISABLE){
			
			gBF_Audio_Cap_Inst[loop].enable = FJ_AUDIO_ENABLE;
			gBF_Audio_Cap_Inst[loop].type = audio_cap;
			
			*audio_capture_id = gBF_Audio_Cap_Inst[loop].audio_capture_id;
			
			ret_flg = D_AUDIO_CAP_ON;
			break;
		}
	}
	
	BF_AudioCapture_TaskStart();
	
	if (ret_flg == D_AUDIO_CAP_OFF){
		BF_Debug_Print_Error(("[AUDIO_CAP]Open Error. All instance are used. L=%d\n", __LINE__));
		ret_val =  D_AUDIO_ERR;
	}
	else {
		ret_val =  D_AUDIO_OK;
	}
	
	(VOID)bf_audio_cap_sig_sem();
	
	return ret_val;
}

/**
 *	@brief		Close Audio Capture Instance
 *	@param		UCHAR			Audio Capture ID
 *	@return		INT32			Result(OK)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_Close( UCHAR audio_capture_id )
{
	INT32 ret;
	UINT8 cap_ch;
	INT32 ret_val;
	
	(VOID)bf_audio_cap_wai_sem();
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		gBF_Audio_Cap_Inst[cap_ch].enable = FJ_AUDIO_DISABLE;
		
		if (gBF_Audio_Cap_Inst[cap_ch].connect_out_id != D_AUDIO_CAP_INVALID){
			BF_AudioOut_DisconnectCap(gBF_Audio_Cap_Inst[cap_ch].connect_out_id, audio_capture_id);
			gBF_Audio_Cap_Inst[cap_ch].connect_out_id = D_AUDIO_CAP_INVALID;
		}
		
		if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] != D_AUDIO_CAP_INVALID){
			BF_AudioEnc_Disconnect(gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0], audio_capture_id);
			gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] = D_AUDIO_CAP_INVALID;
		}
		
		if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] != D_AUDIO_CAP_INVALID){
			BF_AudioEnc_Disconnect(gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1], audio_capture_id);
			gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] = D_AUDIO_CAP_INVALID;
		}
		
		ret_val = D_AUDIO_OK;
	}
	else {
		ret_val = D_AUDIO_ERR;
	}
	
	// All Close
	//	BF_AudioCapture_TaskStop();
	
	(VOID)bf_audio_cap_sig_sem();
	
	return ret_val;
}

/**
 *	@brief		Connect Audio Capture Instance with Audio Out Instance
 *	@param		UCHAR			Audio Capture ID
 *	@param		UCHAR			Audio Out ID
 *	@return		INT32			Result(OK/ERR/ERR_PARAM)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_ConnectOut( UCHAR audio_capture_id, UCHAR audio_out_id )
{
	INT32 ret_val = D_AUDIO_OK;
	INT32 ret;
	UINT8 cap_ch;
	
	(VOID)bf_audio_cap_wai_sem();
	
	ret = bf_audio_cap_check_inst_enable(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Cap_Inst[cap_ch].status != D_AUDIO_CAPTURE_STATUS_START){
			ret = BF_AudioOut_ConnectCap(audio_out_id, audio_capture_id);
			if (ret != D_AUDIO_OK){
				ret_val = D_AUDIO_ERR;
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioOut_ConnectCap() Error. AudioOut is allready connected. L=%d\n", __LINE__));
			}
			else {
				gBF_Audio_Cap_Inst[cap_ch].connect_out_id = audio_out_id;
				
				ret = BF_AudioOut_SetChannelType(gBF_Audio_Cap_Inst[cap_ch].connect_out_id, gBF_Audio_Cap_Inst[cap_ch].in_ch_type);
				if (ret != D_AUDIO_OK){
					BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioOut_SetChannelType Error L=%d\n", __LINE__));
				}
			}
		}
		else {
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_ConnectOut(). Can not Connect(Capture Processing) L=%d\n", __LINE__));
			ret_val =  D_AUDIO_ERR;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_ConnectOut(). Instance not exist. L=%d\n", __LINE__));
		ret_val =  D_AUDIO_ERR;
	}
	
	(VOID)bf_audio_cap_sig_sem();
	
	return ret_val;
}

/**
 *	@brief		DisConnect Audio Out Instance
 *	@param		UCHAR			Audio Capture ID
 *	@param		UCHAR			Audio Out ID
 *	@return		INT32			Result(OK)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_DisconnectOut( UCHAR audio_capture_id, UCHAR audio_out_id )
{
	INT32 ret;
	UINT8 cap_ch = 0;
	
	(VOID)bf_audio_cap_wai_sem();
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		gBF_Audio_Cap_Inst[cap_ch].connect_out_id = D_AUDIO_CAP_INVALID;
	}
	
	if (gBF_Audio_Cap_Inst[cap_ch].status == D_AUDIO_CAPTURE_STATUS_START){
		BF_Debug_Print_Warning(("[AUDIO_CAP]BF_AudioCapture_DisconnectOut(%d,%d) <<Input Operation Processing>> L=%d\n", audio_capture_id, audio_out_id, __LINE__));
	}
	
	BF_AudioOut_DisconnectCap(audio_out_id, audio_capture_id);
	
	(VOID)bf_audio_cap_sig_sem();
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Connect Audio Capture Instance with Audio Encoder Instance
 *	@param		UCHAR			Audio Capture ID
 *	@param		UCHAR			Audio Encoder ID
 *	@return		INT32			Result(OK/ERR/ERR_PARAM)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_ConnectEnc( UCHAR audio_capture_id, UCHAR audio_enc_id )
{
	INT32 ret;
	UINT8 cap_ch;
	INT32 ret_val = D_AUDIO_OK;
	UCHAR cnnct_enc_id0;
	UCHAR cnnct_enc_id1;
	
	(VOID)bf_audio_cap_wai_sem();
	
	ret = bf_audio_cap_check_inst_enable(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Cap_Inst[cap_ch].status == D_AUDIO_CAPTURE_STATUS_START){
			BF_Debug_Print_Warning(("[AUDIO_CAP]BF_AudioCapture_ConnectEnc() <<Input Operation Processing>> L=%d\n", __LINE__));
		}
		
		cnnct_enc_id0 = gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0];
		cnnct_enc_id1 = gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1];
		
		if ((cnnct_enc_id0 != audio_enc_id) && (cnnct_enc_id1 != audio_enc_id)	){
			
			if (cnnct_enc_id0 == D_AUDIO_CAP_INVALID){
				ret = BF_AudioEnc_Connect(audio_enc_id, audio_capture_id);
				if (ret == D_AUDIO_OK){
					gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] = audio_enc_id;
				}
				else {
					ret_val =  ret;
				}
			}
			else if (cnnct_enc_id1 == D_AUDIO_CAP_INVALID){
				ret = BF_AudioEnc_Connect(audio_enc_id, audio_capture_id);
				if (ret == D_AUDIO_OK){
					gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] = audio_enc_id;
				}
				else {
					ret_val =  ret;
				}
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_ConnectEnc() Instance is full. L=%d\n", __LINE__));
				ret_val = D_AUDIO_ERR;
			}
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_ConnectEnc(). Instance not exist. L=%d\n", __LINE__));
		ret_val =  D_AUDIO_ERR_PARAM;
	}
	
	(VOID)bf_audio_cap_sig_sem();
	
	return ret_val;
}

/**
 *	@brief		DisConnect Audio Out Instance
 *	@param		UCHAR			Audio Capture ID
 *	@param		UCHAR			Audio Out ID
 *	@return		INT32			Result(OK)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_DisconnectEnc( UCHAR audio_capture_id, UCHAR audio_enc_id )
{
	UINT8 cap_ch;
	INT32 ret;
	
	(VOID)bf_audio_cap_wai_sem();
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] == audio_enc_id){
			gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] = D_AUDIO_CAP_INVALID;
		}
		else if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] == audio_enc_id){
			gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] = D_AUDIO_CAP_INVALID;
		}
		else {
			;
		}
	}
	
	BF_AudioEnc_Disconnect(audio_enc_id, audio_capture_id);
	
	(VOID)bf_audio_cap_sig_sem();
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Configuration Audio Capture Instance
 *	@param		UCHAR					AudioCapture ID
 *	@param		T_FJ_AUDIO_CAPTURE_CFG	Configuration information
 *	@return		INT32					Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_Config( UCHAR audio_capture_id, T_FJ_AUDIO_CAPTURE_CFG* audio_capture_config )
{
	UINT8 cap_ch;
	INT32 ret;
	T_FJ_AUDIO_CAPTURE_CFG cap_cfg;
	
	(VOID)bf_audio_cap_wai_sem();
	
	cap_cfg = *audio_capture_config;
	
	ret = bf_audio_cap_config_check_param(&cap_cfg);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_cap_check_inst_enable(audio_capture_id, &cap_ch);
		if (ret == D_AUDIO_OK){
			if (gBF_Audio_Cap_Inst[cap_ch].status != D_AUDIO_CAPTURE_STATUS_START){
				gBF_Audio_Cap_Inst[cap_ch].in_port				= cap_cfg.audio_in_port;
				gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.master_slave	= cap_cfg.i2s_cfg.master_slave;
				gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.lr_clock		= cap_cfg.i2s_cfg.lr_clock;
				gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.data_cycle	= cap_cfg.i2s_cfg.data_cycle;
				gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.i2s_format	= cap_cfg.i2s_cfg.i2s_format;
				gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.bit_count	= cap_cfg.i2s_cfg.bit_count;
				
				(VOID)bf_audio_cap_clear_state_change_flg();
				
				ret = bf_audio_cap_set_eventflg_init(cap_ch);
				if (ret == D_AUDIO_OK){
					ret = bf_audio_cap_state_change_wait();
				}
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Config(). AudioCapture Processing. status=%d, L=%d\n", gBF_Audio_Cap_Inst[cap_ch].status, __LINE__));
				ret = D_AUDIO_ERR;
			}
		}
		else {
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Config(). Instance not exist. L=%d\n", __LINE__));
			ret = D_AUDIO_ERR;
		}
	}
	
	(VOID)bf_audio_cap_sig_sem();
	
	return ret;
}

/**
 *	@brief		Configure Audio Capture input/output samples
 *	@param		UCHAR					AudioCapture ID
 *	@param		T_FJ_AUDIO_CAP_SMPL_CFG	input/output sample information
 *	@return		INT32					Result(OK/ERR_PARAM)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_SampleConfig( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_SMPL_CFG* audio_smpl_config )
{
	INT32 ret;
	UINT8 cap_ch;
	
	(VOID)bf_audio_cap_wai_sem();
	
	ret = bf_audio_cap_check_inst_enable(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_cap_smpl_cfg_check_prm(cap_ch, audio_smpl_config);
		if (ret == D_AUDIO_OK){
			if (gBF_Audio_Cap_Inst[cap_ch].status != D_AUDIO_CAPTURE_STATUS_START){
				gBF_Audio_Cap_Inst[cap_ch].in_smpl_rate	 = audio_smpl_config->in_sampling_rate;
				gBF_Audio_Cap_Inst[cap_ch].out_smpl_rate = audio_smpl_config->out_sampling_rate;
				gBF_Audio_Cap_Inst[cap_ch].in_ch_type	 = audio_smpl_config->in_ch_type;
				gBF_Audio_Cap_Inst[cap_ch].out_ch_type	 = audio_smpl_config->out_ch_type;
				
				if (gBF_Audio_Cap_Inst[cap_ch].connect_out_id != D_AUDIO_CAP_INVALID){
					ret = BF_AudioOut_SetChannelType(gBF_Audio_Cap_Inst[cap_ch].connect_out_id, gBF_Audio_Cap_Inst[cap_ch].in_ch_type);
					if (ret != D_AUDIO_OK){
						BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioOut_SetChannelType Error L=%d\n", __LINE__));
					}
				}
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_SampleConfig(). AudioCapture Processing. status=%d, L=%d\n", gBF_Audio_Cap_Inst[cap_ch].status, __LINE__));
				ret = D_AUDIO_ERR;
			}
		}
		else {
			BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_smpl_cfg_check_prm(). Error L=%d\n", __LINE__));
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_SampleConfig(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)bf_audio_cap_sig_sem();
	
	return ret;
}

/**
 *	@brief		Start  Audio Capture
 *	@param		UCHAR					AudioCapture ID
 *	@return		INT32					Result(OK/ERR/ERR_PARAM)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_Start( UCHAR audio_capture_id )
{
	INT32 ret;
	UINT8 cap_ch;
	T_FJ_AUDIO_OUT_CFG out_cfg;
	
	(VOID)bf_audio_cap_wai_sem();
	
	ret = bf_audio_cap_check_inst_enable(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Cap_Inst[cap_ch].status != D_AUDIO_CAPTURE_STATUS_START){
			if (bf_audio_cap_check_dma_available(cap_ch) == D_AUDIO_OK){
				(VOID)bf_audio_cap_clear_state_change_flg();
				
				ret = bf_audio_cap_set_eventflg_init(cap_ch);
				if (ret == D_AUDIO_OK){
					ret = bf_audio_cap_state_change_wait();
					if (ret == D_AUDIO_OK){
						if (gBF_Audio_Cap_Inst[cap_ch].connect_out_id != D_AUDIO_CAP_INVALID){
							if (BF_AudioOut_Get_Status(gBF_Audio_Cap_Inst[cap_ch].connect_out_id) == D_AUDIO_OUT_STATUS_STOP){
								out_cfg.audio_out_port		 = BF_AudioOut_Get_Config_I2sCh(gBF_Audio_Cap_Inst[cap_ch].connect_out_id);
								out_cfg.i2s_cfg.master_slave = gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.master_slave;
								out_cfg.i2s_cfg.lr_clock	 = gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.lr_clock;
								out_cfg.i2s_cfg.data_cycle	 = gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.data_cycle;
								out_cfg.i2s_cfg.i2s_format	 = gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.i2s_format;
								out_cfg.i2s_cfg.bit_count	 = gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.bit_count;
								
								BF_AudioOut_Config(gBF_Audio_Cap_Inst[cap_ch].connect_out_id, &out_cfg);
							}
						}
						
						(VOID)bf_audio_cap_clear_state_change_flg();
						
						ret = bf_audio_cap_set_eventflg_start(cap_ch);
						if (ret == D_AUDIO_OK){
							ret = bf_audio_cap_state_change_wait();
						}
					}
				}
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Start(). Not Available HDMAC0. L=%d\n", __LINE__));
			}
		}
		else {
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Start(). already started.  Status=%d L=%d\n", gBF_Audio_Cap_Inst[cap_ch].status, __LINE__));
			ret = D_AUDIO_ERR;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Start(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)bf_audio_cap_sig_sem();
	
	return ret;
}

/**
 *	@brief		Stop  Audio Capture
 *	@param		UCHAR					AudioCapture ID
 *	@return		INT32					Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_Stop( UCHAR audio_capture_id )
{
	INT32 ret;
	UINT8 cap_ch;
	
	(VOID)bf_audio_cap_wai_sem();
	
	ret = bf_audio_cap_check_inst_enable(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Cap_Inst[cap_ch].connect_out_id != D_AUDIO_CAP_INVALID){
			BF_AudioOut_Stop(gBF_Audio_Cap_Inst[cap_ch].connect_out_id);
		}
		
		if (gBF_Audio_Cap_Inst[cap_ch].status != D_AUDIO_CAPTURE_STATUS_STOP){
			(VOID)bf_audio_cap_clear_state_change_flg();
			
			ret = bf_audio_cap_set_eventflg_stop(cap_ch);
			if (ret == D_AUDIO_OK){
				ret = bf_audio_cap_state_change_wait();
			}
		}
		
		if (gBF_Audio_Cap_Inst[cap_ch].connect_out_id != D_AUDIO_CAP_INVALID){
			gBF_Audio_Cap_Inst[cap_ch].out_start_flg = FJ_AUDIO_DISABLE;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Stop(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)bf_audio_cap_sig_sem();
	
	return ret;
}

/**
 *	@brief		Enable/Disable PlugIn1
 *	@param		UCHAR				AudioCapture ID
 *	@param		UINT8				enable/disable
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_EnablePlugIn1( UCHAR audio_capture_id, UINT8 enable )
{
	INT32 ret = D_AUDIO_OK;
	UINT8 cap_ch;
	
	(VOID)bf_audio_cap_wai_sem();
	
	ret = bf_audio_cap_check_inst_enable(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if ( enable > FJ_AUDIO_ENABLE){
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_EnablePlugIn1(). enable is out of range(=%d). L=%d\n", enable, __LINE__));
			ret = D_AUDIO_ERR_PARAM;
		}
		else {
			gBF_Audio_Cap_Inst[cap_ch].plug_in1_enable = enable;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_EnablePlugIn1(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)bf_audio_cap_sig_sem();
	
	return ret;
}

/**
 *	@brief		Notify complete plugin1 process
 *	@param		UCHAR				AudioCapture ID
 *	@param		UINT8				index of free area
 *	@return		INT32				Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_CompletePlugIn1( UCHAR audio_capture_id, UINT8 free_area_index )
{
	INT32 ret;
	UINT8 cap_ch;
	
//	(VOID)bf_audio_cap_wai_sem();
	
	ret = bf_audio_cap_check_inst_enable(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_cap_free_pi1_buff(cap_ch, free_area_index);
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_CompletePlugIn1(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
//	(VOID)bf_audio_cap_sig_sem();
	
	return ret;
}

/**
 *	@brief		Set Test Mode
 *	@param		UINT8			TestMode[0:OFF, 1:Send Before PlugIn0]
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioCapture_SetTestMode( UINT8 mode )
{
	(VOID)bf_audio_cap_wai_sem();
	
	gBF_AudioCap_TestMode = mode;
	
	(VOID)bf_audio_cap_sig_sem();
}

/****
 AudioCapture Task Process
****/

/**
 *	@brief		Initialize Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioCapture_InputInit( UINT8 cap_ch )
{
	INT32 ret;
	
	if (gBF_Audio_Cap_Inst[cap_ch].enable == FJ_AUDIO_DISABLE){
		bf_audio_cap_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_CAP]Config Error. Instance not exist. L=%d\n", __LINE__));
		return ;
	}
	
	gBF_Audio_Cap_Inst[cap_ch].out_start_flg = FJ_AUDIO_DISABLE;
	
	// DeviceDriverConfiguration
	ret = bf_audio_cap_set_i2s_audio_if(cap_ch);
	if (ret != D_AUDIO_OK){
		bf_audio_cap_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_set_i2s_audio_if() Error. L=%d\n", __LINE__));
		return ;
	}
	
	ret = bf_audio_cap_start_pdm(cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_start_pdm() Error. L=%d\n", __LINE__));
	}

	gBF_Audio_Cap_Inst[cap_ch].input_count = 0;
	gBF_AudioCap_StartEncBuffAddr[cap_ch][D_AUDIO_CAP_CON_ENC0] =  0;
	gBF_AudioCap_StartEncBuffAddr[cap_ch][D_AUDIO_CAP_CON_ENC1] =  0;
	
	gBF_Audio_Cap_Inst[cap_ch].status = D_AUDIO_CAPTURE_STATUS_INIT;
	
	bf_audio_cap_set_state_change_flg();
}

/**
 *	@brief		Start Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioCapture_InputStart( UINT8 cap_ch )
{
	UINT8 dev_ch;
	INT32 ret;
	T_AUDIO_DMA_START_PARAM dma_start;
	UINT8 in_ch_type;
	
	ret = bf_audio_cap_init_buffer(cap_ch);
	if (ret != D_AUDIO_OK){
		bf_audio_cap_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_init_buffer() error L=%d\n", __LINE__));
		return;
	}
	
	dev_ch = bf_audio_cap_get_dev_ch(cap_ch);
	
	ret = bf_audio_cap_setup_auif(cap_ch, dev_ch);
	if (ret != D_AUDIO_OK){
		bf_audio_cap_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_setup_auif() Error. L=%d\n", __LINE__));
		return;
	}
	
	in_ch_type = gBF_Audio_Cap_Inst[cap_ch].in_ch_type;
	if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
			ret = bf_audio_cap_setup_auif(cap_ch, D_AUDIO_AUIF_CH1);
			if (ret != D_AUDIO_OK){
				bf_audio_cap_set_state_error_flg();
				BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_setup_auif() Error. L=%d\n", __LINE__));
				return;
			}
		}
		else {
			ret = bf_audio_cap_setup_auif(cap_ch, D_AUDIO_AUIF_CH5);
			if (ret != D_AUDIO_OK){
				bf_audio_cap_set_state_error_flg();
				BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_setup_auif() Error. L=%d\n", __LINE__));
				return;
			}
		}
	}
	
	ret = BF_Audio_DMA_Init(&gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID0]);
	if (ret != D_AUDIO_OK){
		(VOID)BF_Audio_Auif_CloseInput(dev_ch);
		
		if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
			if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH1);
			}
			else {
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH5);
			}
		}
		
		bf_audio_cap_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_DMA_Init() Error. L=%d\n", __LINE__));
		return;
	}
	
	if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		ret = BF_Audio_DMA_Init(&gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID1]);
		if (ret != D_AUDIO_OK){
			(VOID)BF_Audio_Auif_CloseInput(dev_ch);
			if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH1);
			}
			else {
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH5);
			}
			
			bf_audio_cap_set_state_error_flg();
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_DMA_Init() Error. L=%d\n", __LINE__));
			return;
		}
	}
	
	BF_Audio_DMA_Set_Callback_Fnc0(gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID0], gBf_AudioCap_Dma0_Callback[cap_ch]);
	BF_Audio_DMA_Set_Callback_Fnc1(gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID0], gBf_AudioCap_Dma1_Callback[cap_ch]);
	
	if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		BF_Audio_DMA_Set_Callback_Fnc0(gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID1], gBf_AudioCap_Dma0_2nd_Callback[cap_ch]);
		BF_Audio_DMA_Set_Callback_Fnc1(gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID1], gBf_AudioCap_Dma1_2nd_Callback[cap_ch]);
	}
	
	ret = bf_audio_cap_set_dma_param(cap_ch, dev_ch, &dma_start);
	if (ret != D_AUDIO_OK){
		(VOID)BF_Audio_Auif_CloseInput(dev_ch);
		if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
			(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH1);
		}
		else {
			(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH5);
		}
		
		bf_audio_cap_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_set_dma_param() Error. L=%d\n", __LINE__));
		return;
	}
	
	BF_Audio_Data_Flush_Dcache((VOID*)dma_start.dst_address, dma_start.trans_size);
	
	BF_Audio_DMA_Input_Start0(&dma_start);
	
	if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
			ret = bf_audio_cap_set_dma_param(cap_ch, D_AUDIO_AUIF_CH1, &dma_start);
			if (ret != D_AUDIO_OK){
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH0);
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH1);
				bf_audio_cap_set_state_error_flg();
				BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_set_dma_param() Error. L=%d\n", __LINE__));
				return;
			}
		}
		else {
			ret = bf_audio_cap_set_dma_param(cap_ch, D_AUDIO_AUIF_CH5, &dma_start);
			if (ret != D_AUDIO_OK){
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH4);
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH5);
				bf_audio_cap_set_state_error_flg();
				BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_set_dma_param() Error. L=%d\n", __LINE__));
				return;
			}
		}
		
		BF_Audio_Data_Flush_Dcache((VOID*)dma_start.dst_address, dma_start.trans_size);
		
		BF_Audio_DMA_Input_Start0(&dma_start);
	}
	
	ret= bf_audio_cap_renov_inputbuff_in(cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_renov_inputbuff_in() error L=%d\n", __LINE__));
	}
	
	ret = bf_audio_cap_set_dma_param(cap_ch, dev_ch, &dma_start);
	if (ret != D_AUDIO_OK){
		(VOID)BF_Audio_Auif_CloseInput(dev_ch);
		if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
			(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH1);
		}
		else {
			(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH5);
		}
		
		bf_audio_cap_set_state_error_flg();
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_set_dma_param() Error. L=%d\n", __LINE__));
		return;
	}
	
	BF_Audio_Data_Flush_Dcache((VOID*)dma_start.dst_address, dma_start.trans_size);
	
	BF_Audio_DMA_Input_Start1(&dma_start);
	
	if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
			ret = bf_audio_cap_set_dma_param(cap_ch, D_AUDIO_AUIF_CH1, &dma_start);
			if (ret != D_AUDIO_OK){
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH0);
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH1);
				bf_audio_cap_set_state_error_flg();
				BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_set_dma_param() Error. L=%d\n", __LINE__));
				return;
			}
		}
		else {
			ret = bf_audio_cap_set_dma_param(cap_ch, D_AUDIO_AUIF_CH5, &dma_start);
			if (ret != D_AUDIO_OK){
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH4);
				(VOID)BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH5);
				bf_audio_cap_set_state_error_flg();
				BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_set_dma_param() Error. L=%d\n", __LINE__));
				return;
			}
		}
		
		BF_Audio_Data_Flush_Dcache((VOID*)dma_start.dst_address, dma_start.trans_size);
		
		BF_Audio_DMA_Input_Start1(&dma_start);
	}
	
	ret= bf_audio_cap_renov_inputbuff_in(cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_renov_inputbuff_in() error L=%d\n", __LINE__));
	}
	
	gBF_Audio_Cap_Inst[cap_ch].status = D_AUDIO_CAPTURE_STATUS_START;
	
	if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
			BF_Audio_Auif_Input_Start(D_AUDIO_AUIF_CH0_CH1);
		}
		else {
			BF_Audio_Auif_Input_Start(D_AUDIO_AUIF_CH4_CH5);
		}
	}
	else {
		BF_Audio_Auif_Input_Start(dev_ch);
	}
	
	bf_audio_cap_set_state_change_flg();
}

/**
 *	@brief		Stop Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioCapture_InputStop( UINT8 cap_ch )
{
	UINT8 dev_ch;
	INT32 ret;
	UINT8 in_ch_type;
	
	gBF_Audio_Cap_Inst[cap_ch].status = D_AUDIO_CAPTURE_STATUS_STOP;
	
	dev_ch = bf_audio_cap_get_dev_ch(cap_ch);
	
	in_ch_type = gBF_Audio_Cap_Inst[cap_ch].in_ch_type;
	
	if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
			BF_Audio_Auif_Input_Stop(D_AUDIO_AUIF_CH0_CH1);
		}
		else {
			BF_Audio_Auif_Input_Stop(D_AUDIO_AUIF_CH4_CH5);
		}
	}
	else {
		BF_Audio_Auif_Input_Stop(dev_ch);
	}
	
	ret = BF_Audio_DMA_Stop(gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID0]);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_DMA_Stop Error L=%d\n", __LINE__));
	}
	
	if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		ret = BF_Audio_DMA_Stop(gBF_Audio_Cap_Inst[cap_ch].dma_id[D_AUDIO_CAP_DMA_ID1]);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_DMA_Stop Error L=%d\n", __LINE__));
		}
	}
	
	ret = BF_Audio_Auif_CloseInput(dev_ch);
	if (ret != D_AUDIO_OK){
		// BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_CloseInput() Error. L=%d\n", __LINE__));
		;
	}
	
	if (in_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		if (gBF_Audio_Cap_Inst[cap_ch].in_port == E_FJ_AUDIO_IN_PORT_I2S0){
			ret = BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH1);
			if (ret != D_AUDIO_OK){
				// BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_CloseInput() Error. L=%d\n", __LINE__));
				;
			}
		}
		else {
			ret = BF_Audio_Auif_CloseInput(D_AUDIO_AUIF_CH5);
			if (ret != D_AUDIO_OK){
				// BF_Debug_Print_Error(("[AUDIO_CAP]BF_Audio_Auif_CloseInput() Error. L=%d\n", __LINE__));
				;
			}
		}
	}
	
	ret = bf_audio_cap_stop_pdm(cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]bf_audio_cap_stop_pdm() Error. L=%d\n", __LINE__));
	}
	
	bf_audio_cap_set_state_change_flg();
}

/**
 *	@brief		Receive PCM Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioCapture_PcmReceive( UINT8 cap_ch )
{
	bf_audio_cap_pcm_receive_sync_4ch(cap_ch);
	bf_audio_cap_pcm_receive(cap_ch);
}

/**
 *	@brief		Transfer PCM Data for PlugIn1
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioCapture_TxPlugIn1( UINT8 cap_ch )
{
	bf_audio_cap_plugin1_cb(cap_ch, (T_FJ_AUDIO_CAP_PLUGIN1_CB_PARAM*)&gBF_AudioCap_PI1_Param[cap_ch]);
}

/****
 Called BF_Audio
****/

/**
 *	@brief		Renovation Buffer Output Position for AudioOut
 *	@param		UCHAR		Audio Capture ID
 *	@return		INT32		Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_Renov_Buff_Out( UCHAR audio_capture_id )
{
	INT32 ret;
	UINT8 cap_ch;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Cap_Inst[cap_ch].in_out_addr_mode == D_AUDIO_CAP_PLUGIN_ADDR_MODE_DIFF){
			ret = bf_audio_cap_renov_pi0_buff_out_out(cap_ch, 1);
		}
		else {
			ret = bf_audio_cap_renov_inputbuff_out_out(cap_ch);
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Renov_Buff_Out(). Instance not exist. L=%d\n", __LINE__));
	}
	
	return ret;
}

/**
 *	@brief		Get Address of Buffer for AudioOut
 *	@param		UCHAR		Audio Capture ID
 *	@return		UINT32		Address of Buffer
 *	@note		None
 *	@attention	None
 */
UINT32 BF_AudioCapture_Get_BuffAddr_Out( UCHAR audio_capture_id )
{
	UINT32 addr;
	UINT8 cap_ch;
	INT32 ret;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Cap_Inst[cap_ch].in_out_addr_mode == D_AUDIO_CAP_PLUGIN_ADDR_MODE_DIFF){
			addr = bf_audio_cap_get_pi0_buff_out_out_addr(cap_ch);
		}
		else {
			addr = bf_audio_cap_get_inputbuff_out_out_addr(cap_ch);
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Get_BuffAddr_Out(). Instance not exist. L=%d\n", __LINE__));
		addr = 0;
	}
	
	return addr;
}

/**
 *	@brief		Get Size of Buffer for AudioOut
 *	@param		UCHAR		Audio Capture ID
 *	@return		UINT32		Size of Buffer
 *	@note		None
 *	@attention	None
 */
UINT32 BF_AudioCapture_Get_BuffSize_Out( UCHAR audio_capture_id )
{
	UINT32 size;
	UINT8 cap_ch;
	INT32 ret;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Cap_Inst[cap_ch].in_out_addr_mode == D_AUDIO_CAP_PLUGIN_ADDR_MODE_DIFF){
			size = bf_audio_cap_get_pi0_buff_out_out_size(cap_ch);
		}
		else {
			size = bf_audio_cap_get_inputbuff_out_out_adj_size(cap_ch);
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Get_BuffSize_Out(). Instance not exist. L=%d\n", __LINE__));
		size = 0;
	}
	
	return size;
}

/**
 *	@brief		Get Input Position of PCM Input Buffer
 *	@param		UCHAR		Audio Capture ID
 *	@return		UINT8		Input Position of Input Buffer
 *	@note		None
 *	@attention	None
 */
UINT8 BF_AudioCapture_Get_Buff_InPos( UCHAR audio_capture_id )
{
	UINT8 cap_ch = 0;
	INT32 ret;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Get_Buff_InPos(). Invalid Instance ID. L=%d\n", __LINE__));
	}
	
	return (bf_audio_cap_get_inputbuff_in_pos(cap_ch));
}

/**
 *	@brief		Set Start Buffer Position for AudioEncoder
 *	@param		UCHAR		Audio Capture ID
 *	@param		UCHAR		Audio Encoder ID
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioCapture_Set_EncStartBuffPos( UCHAR audio_capture_id, UCHAR audio_enc_id )
{
	INT32 ret;
	UINT8 cap_ch;
	
	ret = bf_audio_cap_check_inst_enable(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] == audio_enc_id){
			gBF_AudioCap_EncStart_Flg[cap_ch][D_AUDIO_CAP_CON_ENC0] = D_AUDIO_CAP_OFF;
			
			gBF_AudioCap_StartEncBuffAddr[cap_ch][D_AUDIO_CAP_CON_ENC0]
			 = gBF_AudioCap_Input_Buff[cap_ch][BF_AudioCapture_Get_Buff_InPos(audio_capture_id)].addr;
		}
		else if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] == audio_enc_id){
			gBF_AudioCap_EncStart_Flg[cap_ch][D_AUDIO_CAP_CON_ENC1] = D_AUDIO_CAP_OFF;
			
			gBF_AudioCap_StartEncBuffAddr[cap_ch][D_AUDIO_CAP_CON_ENC1]
			 = gBF_AudioCap_Input_Buff[cap_ch][BF_AudioCapture_Get_Buff_InPos(audio_capture_id)].addr;
		}
		else {
			;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Set_EncStartBuffPos(). Instance not exist. L=%d\n", __LINE__));
	}
}

/**
 *	@brief		Get Byte Per Sample of Buffer Data
 *	@param		UCHAR		Audio Capture ID
 *	@return		UINT8		Byte Per Sample
 *	@note		None
 *	@attention	None
 */
UINT8 BF_AudioCapture_Get_Buff_BytePerSample( UCHAR audio_capture_id )
{
	INT32 ret;
	UINT8 cap_ch;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Get_Buff_BytePerSample(). Instance not exist. L=%d\n", __LINE__));
		return 0;
	}
	
	return (gBF_Audio_Cap_Inst[cap_ch].i2s_cfg.bit_count / 8);
}

/**
 *	@brief		Renovation Buffer for AudioEncoder
 *	@param		UCHAR		Audio Capture ID
 *	@param		UCHAR		Audio Encoder ID
 *	@return		INT32		Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_Renov_Buff_Enc( UCHAR audio_capture_id, UCHAR audio_enc_id )
{
	INT32 ret;
	UINT8 cap_ch;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Cap_Inst[cap_ch].in_out_addr_mode == D_AUDIO_CAP_PLUGIN_ADDR_MODE_DIFF){
			if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] == audio_enc_id){
				ret = bf_audio_cap_renov_pi0_buff_out_enc(cap_ch, D_AUDIO_CAP_CON_ENC0, 1);
			}
			else if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] == audio_enc_id){
				ret = bf_audio_cap_renov_pi0_buff_out_enc(cap_ch, D_AUDIO_CAP_CON_ENC1, 1);
			}
			else {
				ret = D_AUDIO_ERR;
			}
		}
		else {
			if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] == audio_enc_id){
				ret = bf_audio_cap_renov_inputbuff_out_enc(cap_ch, D_AUDIO_CAP_CON_ENC0);
			}
			else if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] == audio_enc_id){
				ret = bf_audio_cap_renov_inputbuff_out_enc(cap_ch, D_AUDIO_CAP_CON_ENC1);
			}
			else {
				ret = D_AUDIO_ERR;
			}
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Renov_Buff_Enc(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	return ret;
}

/**
 *	@brief		Get Address of Buffer for AudioEncoder
 *	@param		UCHAR		Audio Capture ID
 *	@param		UCHAR		Audio Encoder ID
 *	@return		UINT32		Address of Buffer
 *	@note		None
 *	@attention	None
 */
UINT32 BF_AudioCapture_Get_BuffAddr_Enc( UCHAR audio_capture_id, UCHAR audio_enc_id )
{
	UINT32 addr;
	UINT8 cap_ch;
	INT32 ret;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Get_BuffAddr_Enc(). Instance not exist. L=%d\n", __LINE__));
		return 0;
	}
	
	if (gBF_Audio_Cap_Inst[cap_ch].in_out_addr_mode == D_AUDIO_CAP_PLUGIN_ADDR_MODE_DIFF){
		if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] == audio_enc_id){
			addr = bf_audio_cap_get_pi0_buff_out_enc_addr(cap_ch, D_AUDIO_CAP_CON_ENC0);
		}
		else if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] == audio_enc_id){
			addr = bf_audio_cap_get_pi0_buff_out_enc_addr(cap_ch, D_AUDIO_CAP_CON_ENC0);
		}
		else {
			addr = 0;
		}
	}
	else {
		if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] == audio_enc_id){
			addr = bf_audio_cap_get_inputbuff_out_enc_addr(cap_ch, D_AUDIO_CAP_CON_ENC0);
		}
		else if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] == audio_enc_id){
			addr = bf_audio_cap_get_inputbuff_out_enc_addr(cap_ch, D_AUDIO_CAP_CON_ENC1);
		}
		else {
			addr = 0;
		}
	}
	
	return addr;
}

/**
 *	@brief		Get Size of Buffer for AudioEncoder
 *	@param		UCHAR		Audio Capture ID
 *	@return		UINT32		Size of Buffer
 *	@note		None
 *	@attention	None
 */
UINT32 BF_AudioCapture_Get_BuffSize_Enc( UCHAR audio_capture_id, UCHAR audio_enc_id )
{
	UINT32 size;
	UINT8 cap_ch;
	INT32 ret;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Get_BuffSize_Enc(). Instance not exist. L=%d\n", __LINE__));
		return 0;
	}
	
	if (gBF_Audio_Cap_Inst[cap_ch].in_out_addr_mode == D_AUDIO_CAP_PLUGIN_ADDR_MODE_DIFF){
		if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] == audio_enc_id){
			size = bf_audio_cap_get_pi0_buff_out_enc_size(cap_ch, D_AUDIO_CAP_CON_ENC0);
		}
		else if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] == audio_enc_id){
			size = bf_audio_cap_get_pi0_buff_out_enc_size(cap_ch, D_AUDIO_CAP_CON_ENC1);
		}
		else {
			size = 0;
		}
	}
	else {
		if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] == audio_enc_id){
			size = bf_audio_cap_get_inputbuff_out_enc_size(cap_ch, D_AUDIO_CAP_CON_ENC0);
		}
		else if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] == audio_enc_id){
			size = bf_audio_cap_get_inputbuff_out_enc_size(cap_ch, D_AUDIO_CAP_CON_ENC1);
		}
		else {
			size = 0;
		}
	}
	
	return size;
}

/**
 *	@brief		Get BankCount of Buffer for AudioEncoder
 *	@param		UCHAR		Audio Capture ID
 *	@param		UCHAR		Audio Encoder ID
 *	@return		UINT32		BankCount of Buffer
 *	@note		None
 *	@attention	None
 */
UINT32 BF_AudioCapture_Get_BuffBankCount_Enc( UCHAR audio_capture_id, UCHAR audio_enc_id )
{
	UINT32 count = 0;
	UINT8 cap_ch;
	INT32 ret;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Get_BuffBankCount_Enc(). Instance not exist. L=%d\n", __LINE__));
		return 0;
	}
	
	if (gBF_Audio_Cap_Inst[cap_ch].in_out_addr_mode == D_AUDIO_CAP_PLUGIN_ADDR_MODE_DIFF){
		if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] == audio_enc_id){
			count = gBF_AudioCap_PI0_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC0];
		}
		else if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] == audio_enc_id){
			count = gBF_AudioCap_PI0_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC1];
		}
		else {
			count = 0;
		}
	}
	else {
		if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] == audio_enc_id){
			count = gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC0];
		}
		else if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] == audio_enc_id){
			count = gBF_AudioCap_Input_Buff_Info[cap_ch].count_enc[D_AUDIO_CAP_CON_ENC1];
		}
		else {
			count = 0;
		}
	}
	
	return count;
}

/**
 *	@brief		Get Channel Type of Buffer
 *	@param		UCHAR		Audio Capture ID
 *	@return		UINT8		Channel Type(Stereo or Mono)
 *	@note		None
 *	@attention	None
 */
UINT8 BF_AudioCapture_Get_BuffChType( UCHAR audio_capture_id )
{
	UINT8 cap_ch;
	INT32 ret;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Get_BuffChType(). Instance not exist. L=%d\n", __LINE__));
		return FJ_AUDIO_NUM_CHANNEL_2;
	}
	
	return gBF_Audio_Cap_Inst[cap_ch].out_ch_type;
}

/**
 *	@brief		Get Sample Offset
 *	@param		UCHAR		Audio Capture ID
 *	@return		UINT16		Sample Offset
 *	@note		None
 *	@attention	None
 */
UINT16 BF_AudioCapture_Get_SampleOffset( UCHAR audio_capture_id )
{
	UINT8 dev_ch;
	UINT16 sample;
	UINT8 cap_ch;
	INT32 ret;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Get_SampleOffset(). Instance not exist. L=%d\n", __LINE__));
		return 0;
	}
	
	dev_ch = bf_audio_cap_get_dev_ch(cap_ch);
	
	sample = BF_Audio_Auif_Get_Input_Trns_Smaple(dev_ch);
	
	// in case of processing data, do not adjust it.
	if (gBF_Audio_Cap_Inst[cap_ch].out_smpl_rate != gBF_Audio_Cap_Inst[cap_ch].in_smpl_rate){
		sample = 0;
	}
	
	return sample;
}

/**
 *	@brief		Clear Start Flag of AudioEncoder
 *	@param		UCHAR		Audio Capture ID
 *	@param		UCHAR		Audio Encoder ID
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioCapture_Clear_EncStartFlag( UCHAR audio_capture_id, UCHAR audio_enc_id)
{
	INT32 ret;
	UINT8 cap_ch;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC0] == audio_enc_id){
			gBF_AudioCap_EncStart_Flg[cap_ch][D_AUDIO_CAP_CON_ENC0]		= D_AUDIO_CAP_OFF;
			gBF_AudioCap_StartEncBuffAddr[cap_ch][D_AUDIO_CAP_CON_ENC0]	= 0;
		}
		else if (gBF_Audio_Cap_Inst[cap_ch].connect_enc_id[D_AUDIO_CAP_CON_ENC1] == audio_enc_id){
			gBF_AudioCap_EncStart_Flg[cap_ch][D_AUDIO_CAP_CON_ENC1]		= D_AUDIO_CAP_OFF;
			gBF_AudioCap_StartEncBuffAddr[cap_ch][D_AUDIO_CAP_CON_ENC1]	= 0;
		}
		else {
			;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Clear_EncStartFlag(). Instance not exist. L=%d\n", __LINE__));
	}
}

/**
 *	@brief		Get Status
 *	@param		UCHAR		Audio Capture ID
 *	@return		UINT8		Status
 *	@note		None
 *	@attention	None
 */
UINT8 BF_AudioCapture_Get_Status( UCHAR audio_capture_id )
{
	INT32 ret;
	UINT8 cap_ch;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Get_Status(). Instance not exist. L=%d\n", __LINE__));
		return D_AUDIO_CAPTURE_STATUS_STOP;
	}
	
	return gBF_Audio_Cap_Inst[cap_ch].status;
}

/**
 *	@brief		Set Volume
 *	@param		UCHAR		Audio Capture ID
 *	@return		UINT8		volume
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioCapture_Set_Volume( UCHAR audio_capture_id, UINT8 volume )
{
	INT32 ret;
	UINT8 cap_ch;
	UINT8 dev_ch;
	
	ret = bf_audio_cap_get_cap_ch(audio_capture_id, &cap_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_CAP]BF_AudioCapture_Set_Volume(). Instance not exist. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	dev_ch = bf_audio_cap_get_dev_ch(cap_ch);
	
	ret = BF_Audio_Auif_Set_InputGain(dev_ch, volume);
	if (ret == D_AUDIO_OK){
		if (dev_ch == D_AUDIO_AUIF_CH0){
			ret = BF_Audio_Auif_Set_InputGain(D_AUDIO_AUIF_CH1, volume);
		}
	}
	
	return ret;
}

/**
 *	@brief		Get InstanceInformation
 *	@param		UCHAR		Audio Capture ID
 *	@return		UINT8		Status
 *	@note		None
 *	@attention	None
 */
T_AUDIO_CAPTURE_INSTANCE BF_AudioCapture_Get_InstanceInfo( UINT8 index )
{
	return gBF_Audio_Cap_Inst[index];
}
