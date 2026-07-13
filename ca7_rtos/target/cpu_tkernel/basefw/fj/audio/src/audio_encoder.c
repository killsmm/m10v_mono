/**
 * @file		audio_encoder.c
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
#include "audio_encoder.h"
#include "audio_capture.h"
#include "audio_aac_enc.h"
#include "audio_data.h"
#include "audio_aac_if.h"
#include "audio_encdec.h"
#include "dd_arm.h"
#include "sdram_map_movie_record.h"
#include "os_user_custom.h"
#include "debug.h"
#include "mba_if.h"


/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/

#define D_AUDIO_ENC_NUM_SAMPLE			(1024UL)

// ON/OFF
#define D_AUDIO_ENC_ON					(1)
#define D_AUDIO_ENC_OFF					(0)

#define D_AUDIO_ENC_INVALID				(0xFFU)

#define D_AUDIO_ENC_CH_0				(0)
#define D_AUDIO_ENC_CH_1				(1)

#define D_AUDIO_ENC_BUFF_ARRAY				(48U)
#define SDRAM_BNK_AUDIO_STREAM_RECORD		(SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM / 2)

#define D_AUDIO_ENC_UAC_OFFSET				(64)	// UAC offset 64byte
#define D_AUDIO_ENC_AAC_PES_OUT_DATA_OFFSET	(14)	// output data adress offset 14byte
#define D_AUDIO_ENC_AAC_ES_OUT_DATA_OFFSET	(21 + 64)	// output data adress offset 21byte, PES 14bytes, ADTS header 7bytes, UAC 64 bytes

#define D_AUDIO_ENC_VIDEO_PTS_COUNTERT		(90000U)	// 90KHz Counter.

#define D_AUDIO_ENC_SEMAPHORE_TIMEOUT		(10)

#define D_AUDIO_ENC_PRE_ENC_BUF_SIZE		(D_AUDIO_ENC_NUM_SAMPLE * 4 * 4)
#define D_AUDIO_ENC_PRE_ENC_BUF_NUM			(2)

#define D_AUDIO_ENC_NUM_SMPL_RATE			(10)

#define D_AUDIO_ENC_SAMPLE_RATE_OTHERS		(0xFFFFFFFFUL)


#define D_AUDIO_ENC_STREAM_WORK0			(SDRAM_ADR_MOVIE_RECORD0_AUDIO_STREAM_WORK_0)
#define D_AUDIO_ENC_STREAM_WORK1			(SDRAM_ADR_MOVIE_RECORD0_AUDIO_STREAM_WORK_1)

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/

typedef struct{
	UINT8	in_pos;		/**< data int postion */
	UINT8	out_pos;	/**< data out postion */
	UINT8	count;		/**< not read data remain */
	UINT8	data_num;	/**< Number of frame */
}T_AUDIO_ENC_BUFFER_INFO;

typedef struct {
	UINT32				addr;			// Input Sampling Rate
	UINT32				size;			// Output Sampling Rate
	UINT8				end_flg;		// end_flg
} T_AUDIO_ENC_IN_BUFF;

// --- Record Control Informations ---
typedef struct {
	UINT32				sample_size[SDRAM_BNK_AUDIO_STREAM_RECORD];		// Audio Stream Size
	UINT32				sample_data[SDRAM_BNK_AUDIO_STREAM_RECORD];		// Audio Stream Pointer
	UINT64				duration[SDRAM_BNK_AUDIO_STREAM_RECORD];		// Audio Stream Duration
	UINT16				sample_count[SDRAM_BNK_AUDIO_STREAM_RECORD];	// Audio Stream Count
	UINT16				use_flg[SDRAM_BNK_AUDIO_STREAM_RECORD];			// Audio Stream Set Flg
	UINT64				total_time;										// Audio Time
} T_AUDIO_ENC_STREAM_CTRL;

typedef struct {
	UINT32	smpl_rate;
	INT32	(*process)(UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate);
} T_AUDIO_ENC_CHK_BIT_RATE;

typedef struct{
	UINT32			area_index;			/**< index of buffer */
	UINT8			end_flag;			/**< stream end flag */
}T_AUDIO_ENC_TX_DATA_INFO;

typedef UINT32 (*T_AUDIO_ENC_GET_SDRAM_AREA)( VOID );

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/

static volatile T_AUDIO_ENC_INSTANCE gBF_Audio_Enc_Inst[D_AUDIO_ENC_INST_NUM] =
{
	{
		D_AUDIO_ENC_STATUS_STOP,		// status
		FJ_AUDIO_ENC_ID_0,				// audio_enc_id
		FJ_AUDIO_DISABLE,				// enable
		E_FJ_AUDIO_ENC_AAC_0,			// type
		FJ_AUDIO_DISABLE,				// enc_enable
		D_AUDIO_ENC_CMPR_AAC,			// compress_type
		D_AUDIO_ENC_INVALID,			// connect_cap_id
		D_AUDIO_SAMPLE_RATE_48000,		// smpl_rate
		128000,							// bit_rate
		FJ_AUDIO_NUM_CHANNEL_2,			// ch_type
		0,								// enc_count
		0,								// offset
		D_AUDIO_ENC_DEMUTE,				// mute
	},
	{
		D_AUDIO_ENC_STATUS_STOP,		// status
		FJ_AUDIO_ENC_ID_1,				// audio_enc_id
		FJ_AUDIO_DISABLE,				// enable
		E_FJ_AUDIO_ENC_AAC_1,			// type
		FJ_AUDIO_DISABLE,				// enc_enable
		D_AUDIO_ENC_CMPR_AAC,			// compress_type
		D_AUDIO_ENC_INVALID,			// connect_cap_id
		D_AUDIO_SAMPLE_RATE_48000,		// smpl_rate
		128000,							// bit_rate
		FJ_AUDIO_NUM_CHANNEL_2,			// ch_type
		0,								// enc_count
		0,								// offset
		D_AUDIO_ENC_DEMUTE,				// mute
	}
};

static volatile T_AUDIO_ENC_BUFFER_INFO gBF_AudioEnc_In_Buff_Info[D_AUDIO_ENC_INST_NUM];
static volatile T_AUDIO_ENC_IN_BUFF gBF_AudioEnc_Input_Buff[D_AUDIO_ENC_INST_NUM][D_AUDIO_ENC_BUFF_ARRAY];
static volatile T_AUDIO_ENC_STREAM_CTRL gBF_AudioEnc_Stream_Ctrl[D_AUDIO_ENC_INST_NUM];

static volatile UINT8 gBF_AudioEnc_PreEncArea[D_AUDIO_ENC_INST_NUM][D_AUDIO_ENC_PRE_ENC_BUF_NUM][D_AUDIO_ENC_PRE_ENC_BUF_SIZE];
static volatile UINT8 gBF_AudioEnc_PreEncAreaIndex[D_AUDIO_ENC_INST_NUM];

static volatile UINT8 gBF_AudioEnc_TestMode = 0;		// 0:Normal, 1:PCM

static volatile T_AUDIO_ENC_TX_DATA_INFO gBF_AudioEnc_TxStreamData_Info[D_AUDIO_ENC_INST_NUM];
static volatile UINT8 gBF_AudioEnc_BuffRnov_Flg[D_AUDIO_ENC_INST_NUM] = {0, 0};
static volatile UINT8 gBF_AudioEnc_ReqEnc_Cnt[D_AUDIO_ENC_INST_NUM] = {0, 0};


// Critical Section
// for gBF_AudioEnc_In_Buff_Info[].count
static ULONG gBF_AudioEnc_CS_Ib_Cnt __attribute__((section(".LOCK_SECTION"), aligned(64)));


/*----------------------------------------------------------------------*/
/* Local Function Prototype Definition                                  */
/*----------------------------------------------------------------------*/
static VOID bf_audio_enc_init_stream_ctrl_info( UINT8 enc_ch );
static VOID bf_audio_enc_init_in_buffer_info( UINT8 enc_ch );
static INT32 bf_audio_enc_renov_inputbuff_in( UINT8 enc_ch );
static INT32 bf_audio_enc_renov_inputbuff_out( UINT8 enc_ch );
static INT32 bf_audio_enc_get_inputbuff_addr( UINT8 enc_ch, UCHAR audio_cap_id, UINT32* addr );
static UINT32 bf_audio_enc_get_outputbuff_addr( UINT8 enc_ch );
static UINT32 bf_audio_enc_get_sdram_area_strm0( VOID );
static UINT32 bf_audio_enc_get_sdram_area_strm1( VOID );
static UINT32 bf_audio_enc_get_inputbuff_size( UINT8 enc_ch );
static VOID bf_audio_enc_set_pes_header( UINT32 addr, UINT32 size, UINT64 pts );
static UINT64 bf_audio_enc_get_pts( UINT64 audio_time, UINT32 smpl_rate );
static VOID bf_audio_enc_sample_collect( UINT8 enc_ch, UINT32 data_adress, UINT32 data_size, UINT8 end_flag );
static INT32 bf_audio_enc_stream_send( UINT8 enc_ch, UINT32 area_index, UINT8 end_flag );
static UINT32 bf_audio_enc_get_remain_area( UINT8 enc_ch );
static INT32 bf_audio_enc_init_param_48khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate );
static INT32 bf_audio_enc_init_param_44khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate );
static INT32 bf_audio_enc_init_param_32khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate );
static INT32 bf_audio_enc_init_param_24khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate );
static INT32 bf_audio_enc_init_param_22khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate );
static INT32 bf_audio_enc_init_param_16khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate );
static INT32 bf_audio_enc_init_param_12khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate );
static INT32 bf_audio_enc_init_param_11khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate );
static INT32 bf_audio_enc_init_param_8khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate );
static INT32 bf_audio_enc_init_param_others( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate );
static INT32 bf_audio_enc_get_encoder_init_param( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate );
static INT32 bf_audio_enc_free_astream( UINT8 enc_ch, UINT32 area_index );
static INT32 bf_audio_enc_start_encode_check_stop( UINT8 enc_ch, UINT8* end_flg );
static INT32 bf_audio_enc_start_encode_stream_data( UINT8 enc_ch, T_AUDIO_AAC_ENC_PROC* enc_param, UINT8 end_flg );
static VOID bf_audio_enc_request_next_encode( UINT8 enc_ch );
static INT32 bf_audio_enc_check_id( UINT8 enc_ch );
static INT32 bf_audio_enc_get_cap_ch( UCHAR audio_enc_id, UINT8* enc_ch );
static INT32 bf_audio_enc_start_chk_status( UINT8 enc_ch );
static INT32 bf_audio_enc_start_24bit_pcm( UINT8 enc_ch, UINT32 in_address, T_AUDIO_AAC_ENC_PROC* enc_param);
static INT32 bf_audio_enc_start_16bit_pcm( UINT8 enc_ch, UINT32 in_address, T_AUDIO_AAC_ENC_PROC* enc_param);
static INT32 bf_audio_enc_config_check_param( const T_FJ_AUDIO_AAC_CONFIG_PARAM* audio_capture_config );
static INT32 bf_audio_enc_request_encode( UINT8 enc_ch );
static INT32 bf_audio_enc_check_aac_lib_version( VOID );

/*----------------------------------------------------------------------*/
/* Function Table Definition											*/
/*----------------------------------------------------------------------*/
static const T_AUDIO_ENC_CHK_BIT_RATE gBF_Audio_Enc_Check_BitRate_Tbl[D_AUDIO_ENC_NUM_SMPL_RATE] = {
	{	D_AUDIO_SAMPLE_RATE_48000,		bf_audio_enc_init_param_48khz		},
	{	D_AUDIO_SAMPLE_RATE_44100,		bf_audio_enc_init_param_44khz		},
	{	D_AUDIO_SAMPLE_RATE_32000,		bf_audio_enc_init_param_32khz		},
	{	D_AUDIO_SAMPLE_RATE_24000,		bf_audio_enc_init_param_24khz		},
	{	D_AUDIO_SAMPLE_RATE_22050,		bf_audio_enc_init_param_22khz		},
	{	D_AUDIO_SAMPLE_RATE_16000,		bf_audio_enc_init_param_16khz		},
	{	D_AUDIO_SAMPLE_RATE_12000,		bf_audio_enc_init_param_12khz		},
	{	D_AUDIO_SAMPLE_RATE_11025,		bf_audio_enc_init_param_11khz		},
	{	D_AUDIO_SAMPLE_RATE_8000,		bf_audio_enc_init_param_8khz		},
	{	D_AUDIO_ENC_SAMPLE_RATE_OTHERS,	bf_audio_enc_init_param_others		},
};

static const T_AUDIO_ENC_GET_SDRAM_AREA gBf_AudioEnc_Get_SdramArea[D_AUDIO_ENC_INST_NUM]	= {
	bf_audio_enc_get_sdram_area_strm0,
	bf_audio_enc_get_sdram_area_strm1
};

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/

/**
 *	Initialize stream control information.
 *	@param   UINT8	audio encoder channel
 *	@return  VOID
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_enc_init_stream_ctrl_info( UINT8 enc_ch )
{
	UINT32 loop;
	OS_USER_ER	ercd;
	
	ercd = OS_User_Twai_Sem(SID_AUDIO_FREE_STREAM, D_AUDIO_ENC_SEMAPHORE_TIMEOUT);
	
	if (ercd != D_OS_USER_E_OK) {
		BF_Debug_Print_Error(("[AUDIO_ENC]twai_sem error(%d) L=%d\n", ercd, __LINE__));
		return ;
	}

	gBF_AudioEnc_Stream_Ctrl[enc_ch].total_time = 0;
	
	for (loop = 0; loop < SDRAM_BNK_AUDIO_STREAM_RECORD; loop++){
		gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_size[loop]	= 0;
		gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_data[loop]	= 0;
		gBF_AudioEnc_Stream_Ctrl[enc_ch].duration[loop]		= 0;
		gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_count[loop]	= 0;
		gBF_AudioEnc_Stream_Ctrl[enc_ch].use_flg[loop] = D_AUDIO_ENC_OFF;
	}

	ercd = OS_User_Sig_Sem(SID_AUDIO_FREE_STREAM);
	if (ercd != D_OS_USER_E_OK) {
		BF_Debug_Print_Error(("[AUDIO_ENC]sig_sem error(%d) L=%d\n", ercd, __LINE__));
		return ;
	}

}

static VOID bf_audio_enc_init_in_buffer_info( UINT8 enc_ch )
{
	gBF_AudioEnc_In_Buff_Info[enc_ch].in_pos	= 0;
	gBF_AudioEnc_In_Buff_Info[enc_ch].out_pos	= 0;
	gBF_AudioEnc_In_Buff_Info[enc_ch].count		= 0;
	gBF_AudioEnc_In_Buff_Info[enc_ch].data_num	= 0;
}


/**
 *	Input Buffer Renovation(Input)
 *	@param		UINT8	audio encoder channel
 *	@return		INT32	OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_renov_inputbuff_in( UINT8 enc_ch )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioEnc_CS_Ib_Cnt );
	
	if (gBF_AudioEnc_In_Buff_Info[enc_ch].count <  (D_AUDIO_ENC_BUFF_ARRAY - 1)){
		gBF_AudioEnc_In_Buff_Info[enc_ch].in_pos++;
		gBF_AudioEnc_In_Buff_Info[enc_ch].in_pos %= D_AUDIO_ENC_BUFF_ARRAY;

		gBF_AudioEnc_In_Buff_Info[enc_ch].count++;	// counter increment

		retcd = D_AUDIO_OK;
	}
	else {
		// no empty area
		retcd = D_AUDIO_ERR;
	}

	Dd_ARM_Critical_Section_End( gBF_AudioEnc_CS_Ib_Cnt );

	return retcd;
}

/**
 *	Input Buffer Renovation(Output)
 *	@param		UINT8	audio encoder channel
 *	@return		INT32	OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_renov_inputbuff_out( UINT8 enc_ch )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioEnc_CS_Ib_Cnt );
	
	if (gBF_AudioEnc_In_Buff_Info[enc_ch].count > 0){
		gBF_AudioEnc_In_Buff_Info[enc_ch].out_pos++;
		gBF_AudioEnc_In_Buff_Info[enc_ch].out_pos %= D_AUDIO_ENC_BUFF_ARRAY;

		gBF_AudioEnc_In_Buff_Info[enc_ch].count--;	// counter decrement

		retcd = D_AUDIO_OK;
	}
	else {
		// no busy area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioEnc_CS_Ib_Cnt );

	return retcd;
}

/**
 *	Get address of Input Buffer
 *	@param		UINT8	audio encoder channel
 *	@param		UCHAR	audio capture id
 *	@param		UINT32	adress of pcm data
 *	@return		INT32	OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_get_inputbuff_addr( UINT8 enc_ch, UCHAR audio_cap_id, UINT32* in_addr )
{
	UINT32 addr;
	UINT32 size;
	UINT32 frame;
	UINT32 loop;
	UINT8 channel;
	UINT8 sample_size;
	INT32 ret;
	UCHAR enc_id;
	
	if (in_addr == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_get_inputbuff_addr() param err. in_addr=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_AudioEnc_In_Buff_Info[enc_ch].data_num < 1){
		enc_id = gBF_Audio_Enc_Inst[enc_ch].audio_enc_id;
		
		if (gBF_AudioEnc_BuffRnov_Flg[enc_ch] != 0){
			ret = BF_AudioCapture_Renov_Buff_Enc(audio_cap_id, enc_id);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Warning(("[AUDIO_ENC]BF_AudioCapture_Renov_Buff_Enc() Buffer UnderFlow L=%d\n", __LINE__));
				*in_addr = 0;
				return D_AUDIO_ERR;
			}
		}
		
		gBF_AudioEnc_BuffRnov_Flg[enc_ch] = 1;
		
		addr = BF_AudioCapture_Get_BuffAddr_Enc(audio_cap_id, enc_id);
		size = BF_AudioCapture_Get_BuffSize_Enc(audio_cap_id, enc_id);
		sample_size = BF_AudioCapture_Get_Buff_BytePerSample(audio_cap_id);
//		channel = gBF_Audio_Enc_Inst[enc_ch].ch_type;
		channel = BF_AudioCapture_Get_BuffChType(audio_cap_id);
		
		if (gBF_AudioEnc_TestMode != 1){
			frame = size / sample_size / channel / D_AUDIO_ENC_NUM_SAMPLE;
			
			if (frame == 0){
				BF_Debug_Print_Error(("[AUDIO_ENC]bf_audio_enc_get_inputbuff_addr() frame=%d, size=0x%x, L=%d\n",frame, size, __LINE__));
				frame = 1;
			}
			
			for (loop = 0; loop < frame; loop++){
				gBF_AudioEnc_Input_Buff[enc_ch][gBF_AudioEnc_In_Buff_Info[enc_ch].in_pos].addr = addr;
				gBF_AudioEnc_Input_Buff[enc_ch][gBF_AudioEnc_In_Buff_Info[enc_ch].in_pos].size = D_AUDIO_ENC_NUM_SAMPLE * sample_size * channel;
				addr += D_AUDIO_ENC_NUM_SAMPLE * 2 * channel;
				
				BF_Audio_Data_Clean_Flush_Dcache((VOID*)gBF_AudioEnc_Input_Buff[enc_ch][gBF_AudioEnc_In_Buff_Info[enc_ch].in_pos].addr,
												 gBF_AudioEnc_Input_Buff[enc_ch][gBF_AudioEnc_In_Buff_Info[enc_ch].in_pos].size);
				
				(VOID)bf_audio_enc_renov_inputbuff_in(enc_ch);
			}
		}
		else {
			// TestMode
			gBF_AudioEnc_Input_Buff[enc_ch][gBF_AudioEnc_In_Buff_Info[enc_ch].in_pos].addr = addr;
			gBF_AudioEnc_Input_Buff[enc_ch][gBF_AudioEnc_In_Buff_Info[enc_ch].in_pos].size = size;
			BF_Audio_Data_Clean_Flush_Dcache((VOID*)gBF_AudioEnc_Input_Buff[enc_ch][gBF_AudioEnc_In_Buff_Info[enc_ch].in_pos].addr,
											 gBF_AudioEnc_Input_Buff[enc_ch][gBF_AudioEnc_In_Buff_Info[enc_ch].in_pos].size);
			
			frame = 1;
			(VOID)bf_audio_enc_renov_inputbuff_in(enc_ch);
		}
		
		gBF_AudioEnc_In_Buff_Info[enc_ch].data_num += frame;
	}
	
	gBF_AudioEnc_In_Buff_Info[enc_ch].data_num--;
	
	*in_addr = gBF_AudioEnc_Input_Buff[enc_ch][gBF_AudioEnc_In_Buff_Info[enc_ch].out_pos].addr;
	
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)*in_addr, gBF_AudioEnc_Input_Buff[enc_ch][gBF_AudioEnc_In_Buff_Info[enc_ch].out_pos].size);
	
	return D_AUDIO_OK;
}

/**
 *	Get size of Input Buffer
 *	@param		UINT8	audio encoder channel
 *	@return		UINT32	size 
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_enc_get_inputbuff_size( UINT8 enc_ch )
{
	return gBF_AudioEnc_Input_Buff[enc_ch][gBF_AudioEnc_In_Buff_Info[enc_ch].out_pos].size;
}

/**
 *  @brief		Get Audio Stream Area Address(SDRAM MAP IF)
 *	@param		None	audio encoder channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_enc_get_sdram_area_strm0( VOID )
{
	UINT32	stream_area;
	
	stream_area = Sdram_Map_Movie_Record_Get_Audio_Stream0();
	
	// Side Change
	Sdram_Map_Movie_Record_Chg_Audio_Stream0();
	
	return stream_area;
}

/**
 *  @brief		Get Audio Stream Area Address(SDRAM MAP IF)
 *	@param		None	audio encoder channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_enc_get_sdram_area_strm1( VOID )
{
	UINT32	stream_area;
	
	stream_area = Sdram_Map_Movie_Record_Get_Audio_Stream1();
	
	// Side Change
	Sdram_Map_Movie_Record_Chg_Audio_Stream1();
	
	return stream_area;
}

/**
 *  @brief		Encode Get Audio Stream Area Address.\n
 *	@param		UINT8	audio encoder channel
 *	@return		Audio Stream Area Address Pointer
 *	@note		None
 *	@attention	if Audio Stream Area Pointer is NULL, it means no need to continue sampling.
 */
static UINT32 bf_audio_enc_get_outputbuff_addr( UINT8 enc_ch )
{
	UINT32	stream_area;
	UINT32	area_index1;
	UINT32	area_index2;
	UINT8	use_flag;
	
	use_flag = D_AUDIO_ENC_OFF;
	
	for (area_index1 = 0; area_index1 < SDRAM_BNK_AUDIO_STREAM_RECORD; area_index1++){
		// Get address
		stream_area = gBf_AudioEnc_Get_SdramArea[enc_ch]();
		
		for (area_index2 = 0; area_index2 < SDRAM_BNK_AUDIO_STREAM_RECORD; area_index2++){
			if ((stream_area == (UINT32)gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_data[area_index2])	&&
				(gBF_AudioEnc_Stream_Ctrl[enc_ch].use_flg[area_index2] == D_AUDIO_ENC_ON)			){
				
//				BF_Debug_Print_Warning(("[AUDIO_STREAM_REC]Audio Stream Area[%lx] Used\n", stream_area));
				stream_area = 0;
				use_flag = D_AUDIO_ENC_ON;
				break;
			}
		}
		
		if (use_flag == D_AUDIO_ENC_ON){
			use_flag = D_AUDIO_ENC_OFF;
		}
		else {
			// Completed to get Audio Stream Area
			break;
		}
	}
	
	if (stream_area == 0) {
		BF_Debug_Print_Error(("[AUDIO_STREAM_REC]Audio Stream Area BufferFull L=%d\n", __LINE__));
	}
	
	return stream_area;
}

/**
 *  @brief		Set PES Header
 *	@param		i:addr	address of Stream data
 *	@param		i:size	size of Stream data
 *	@param		i:pts	PTS
 *	@note		None
 */
static VOID bf_audio_enc_set_pes_header( UINT32 addr, UINT32 size, UINT64 pts)
{
	UCHAR tmp_pts;
/*
addr                                                                   
  |                                  size                              
  |<----------------------------------------------------------------->|
  |                                                                   |
  +----------+--------------------------------------------------------+
  | PES area | ES                                                     |
  +----------+--------------------------------------------------------+
*/
	*(UINT8*)addr			= 0x00;									// start_code					:23-16bit
	*(UINT8*)(addr + 1)		= 0x00;									// start_code					:15-8bit
	*(UINT8*)(addr + 2)		= 0x01;									// start_code					:7-0bit
	*(UINT8*)(addr + 3) 	= 0xC0;									// stream_id					:8bit	:110xxxxxb
	*(UINT8*)(addr + 4)		= (UCHAR)((size - 6) >> 8);				// packet_length				:15-8bit
	*(UINT8*)(addr + 5)		= (UCHAR)((size - 6) & 0x00FF);			// packet_length				:7-0bit
	*(UINT16*)(addr + 6)	= 0x8081;								// sync_code					:2bit	:(10b)
																	// scrambling_control			:2bit	:00b -> 0
																	// priority						:1bit	:0b  -> 0
																	// alignment_indicator			:1bit	:0b  -> 0
																	// copyright					:1bit	:0b  -> 0
																	// original_or_copy				:1bit	:1b  -> 1
																	// PTS_DTS_flag					:2bit	:10b -> 2
																	// ESCR_flag					:1bit	:0b  -> 0
																	// ES_rate_flag					:1bit	:0b  -> 0
																	// DSM_trick_mode_flag			:1bit	:0b  -> 0
																	// additional_copy_info_flag	:1bit	:0b  -> 0
																	// PES_CRC_flag					:1bit	:0b  -> 0
																	// PES_extension_flag			:1bit	:0b  -> 0
	*(UINT8*)(addr + 8)		= 0x05;									// PES_header_data_length		:8bit	:5
	
	// PTS
	tmp_pts = (UINT8)((pts & 0x00000001C0000000) >> 29);
	tmp_pts |= 0x31;										// "0011" & marker_bit
	*(UINT8*)(addr + 9)  = tmp_pts;
	tmp_pts = (UINT8)((pts & 0x000000003FC00000) >> 22);
	*(UINT8*)(addr + 10) = tmp_pts;
	tmp_pts = (UINT8)((pts & 0x00000000003F8000) >> 14);
	tmp_pts |= 0x01;
	*(UINT8*)(addr + 11) = tmp_pts;
	tmp_pts = (UINT8)((pts & 0x0000000000007F80) >> 7);
	*(UINT8*)(addr + 12) = tmp_pts;
	tmp_pts = (UINT8)((pts & 0x000000000000007F) << 1);
	tmp_pts |= 0x01;
	*(UINT8*)(addr + 13) = tmp_pts;
}

/**
 *  @brief		Get PTS(90kHz Units)
 *	@param		i:addr	address of Stream data
 *	@param		i:size	size of Stream data
 *	@param		i:pts	PTS
 *	@note		None
 */
static UINT64 bf_audio_enc_get_pts( UINT64 audio_time, UINT32 smpl_rate)
{
	if (audio_time <= D_AUDIO_ENC_NUM_SAMPLE){
		return 0;
	}
	
	return (((audio_time - D_AUDIO_ENC_NUM_SAMPLE) * D_AUDIO_ENC_VIDEO_PTS_COUNTERT) / smpl_rate);
}

/**
 *	Set information of encoded data
 *	@param		UINT8	audio encoder channel
 *	@param		UINT32	address of data
 *	@param		UINT32	size of data
 *	@param		UINT8	flag of end
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_enc_sample_collect( UINT8 enc_ch, UINT32 data_adress, UINT32 data_size, UINT8 end_flag )
{
	OS_USER_ER	ercd;
	UINT32	area_index;
	UINT8	send_flg;
	
	// EventFlag Table
	static const OS_USER_FLGPTN event_flg_txdata_tbl[D_AUDIO_ENC_INST_NUM] = {
		FLG_AUDIO_ENCDEC_ENC0_TX_DATA,
		FLG_AUDIO_ENCDEC_ENC1_TX_DATA
	};
	
	if( data_adress == 0 ){
		BF_Debug_Print_Error(("[AUDIO_ENC]bf_audio_enc_sample_collect() data_adress==0\n"));
		return;
	}
	if( data_size == 0 ){
		BF_Debug_Print_Error(("[AUDIO_ENC]bf_audio_enc_sample_collect() size==0\n"));
		return;
	}
	
	send_flg = 1;
	
	// Collect the audio Data
	for ( area_index = 0; area_index < SDRAM_BNK_AUDIO_STREAM_RECORD; area_index++ ){
		if( gBF_AudioEnc_Stream_Ctrl[enc_ch].use_flg[area_index] == D_AUDIO_ENC_OFF ){
			ercd = OS_User_Twai_Sem(SID_AUDIO_FREE_STREAM, D_AUDIO_ENC_SEMAPHORE_TIMEOUT);
			
			if (ercd != D_OS_USER_E_OK) {
				BF_Debug_Print_Error(("[AUDIO_ENC]twai_sem error(%d) L=%d\n", ercd, __LINE__));
				return ;
			}
			
			gBF_AudioEnc_Stream_Ctrl[enc_ch].duration[area_index]
			 = bf_audio_enc_get_pts(gBF_AudioEnc_Stream_Ctrl[enc_ch].total_time, gBF_Audio_Enc_Inst[enc_ch].smpl_rate);
			
			gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_count[area_index]	= D_AUDIO_ENC_NUM_SAMPLE;
			gBF_AudioEnc_Stream_Ctrl[enc_ch].use_flg[area_index]		= D_AUDIO_ENC_ON;
			gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_size[area_index]	= data_size  + D_AUDIO_ENC_UAC_OFFSET;
			gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_data[area_index]	= data_adress - D_AUDIO_ENC_UAC_OFFSET;
			
			gBF_AudioEnc_TxStreamData_Info[enc_ch].area_index	= area_index;
			gBF_AudioEnc_TxStreamData_Info[enc_ch].end_flag		= end_flag;
			
			ercd = OS_User_Sig_Sem(SID_AUDIO_FREE_STREAM);
			if (ercd != D_OS_USER_E_OK) {
				BF_Debug_Print_Error(("[AUDIO_ENC]sig_sem error(%d) L=%d\n", ercd, __LINE__));
				return ;
			}
			
			// send audio stream to host
			ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, event_flg_txdata_tbl[enc_ch] );
			if (ercd != D_OS_USER_E_OK){
				BF_Debug_Print_Error(("[AUDIO_ENC]set_flg error(%d) L=%d\n", ercd, __LINE__));
			}
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_POINT, "use on ch:%d, idx:%d", enc_ch, area_index));
			send_flg = 0;
			break;
		}
	}
	
	if (send_flg == 1){
		BF_Debug_Print_Error(("[AUDIO_STREAM_REC]Audio Stream Area BufferFull L=%d\n", __LINE__));
		BF_Debug_Print_Error(("[AUDIO_ENC]Encode Force Stop. L=%d\n", __LINE__));
		
		//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_POINT, "audio full"));

		gBF_Audio_Enc_Inst[enc_ch].status = D_AUDIO_ENC_STATUS_STOP;
		
		BF_AudioCapture_Clear_EncStartFlag(gBF_Audio_Enc_Inst[enc_ch].connect_cap_id, gBF_Audio_Enc_Inst[enc_ch].audio_enc_id);
	}
}

/**
 *	Send stream data
 *	@param		UINT8	audio encoder channel
 *	@param		UINT32	index of stream area
 *	@param		UINT8	flag of end
 *	@return		None	OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_stream_send( UINT8 enc_ch, UINT32 area_index, UINT8 end_flag )
{
	OS_USER_ER ercd;
	T_OS_USER_RSEM pk_rsm ={0, 0};
	
	if (gBF_AudioEnc_TestMode != 1){
		// AAC
		T_FJ_AUDIO_AAC_STREAM_PARAM stream_info_aac;
		
		stream_info_aac.area_index		= area_index;
		stream_info_aac.num_remain_area	= bf_audio_enc_get_remain_area(enc_ch);
		stream_info_aac.uac_data		= (UCHAR*)gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_data[area_index];
		stream_info_aac.uac_size		= gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_size[area_index];
		stream_info_aac.pes_data		= (UCHAR*)gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_data[area_index] + D_AUDIO_ENC_UAC_OFFSET;
		stream_info_aac.pes_size		= gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_size[area_index] - D_AUDIO_ENC_UAC_OFFSET;
		stream_info_aac.adts_data		= (UCHAR *)gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_data[area_index] + D_AUDIO_ENC_UAC_OFFSET + D_AUDIO_ENC_AAC_PES_OUT_DATA_OFFSET;
		stream_info_aac.adts_size		= gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_size[area_index] - D_AUDIO_ENC_UAC_OFFSET - D_AUDIO_ENC_AAC_PES_OUT_DATA_OFFSET;
		stream_info_aac.es_data		= (UCHAR*)(gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_data[area_index] + D_AUDIO_ENC_AAC_ES_OUT_DATA_OFFSET);
		stream_info_aac.es_size		= gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_size[area_index] - D_AUDIO_ENC_AAC_ES_OUT_DATA_OFFSET;

		stream_info_aac.time_stamp		= gBF_AudioEnc_Stream_Ctrl[enc_ch].duration[area_index];
		stream_info_aac.error_info		= 0;
		stream_info_aac.ch_type			= gBF_Audio_Enc_Inst[enc_ch].ch_type;
		stream_info_aac.frame_length	= gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_count[area_index];
		stream_info_aac.sampling_freq	= gBF_Audio_Enc_Inst[enc_ch].smpl_rate;
		stream_info_aac.bit_rate		= gBF_Audio_Enc_Inst[enc_ch].bit_rate;
		stream_info_aac.stream_end_flag	= end_flag;
		stream_info_aac.frame_no		= gBF_Audio_Enc_Inst[enc_ch].enc_count-1; /* pgr0063 */
				
		ercd = OS_User_Ref_Sem(SID_AUDIO_ENCDEC, &pk_rsm);
		if( ercd != D_OS_USER_E_OK ){
			BF_Debug_Print_Error(("[AUDIO_ENC]ref_tsk error(%d) L=%d\n", ercd, __LINE__));
		}
		
		if (pk_rsm.semcnt == 0){
			(VOID)BF_AudioEncDec_Sig_Sem();
		}
		
		FJ_Host_AACStreamCB(gBF_Audio_Enc_Inst[enc_ch].audio_enc_id, &stream_info_aac);
		
		if (pk_rsm.semcnt == 0){
			(VOID)BF_AudioEncDec_Wai_Sem();
		}
	}
	else {
		// PCM
		T_FJ_AUDIO_PCM_STREAM_PARAM stream_info_pcm;
		
		stream_info_pcm.area_index		= area_index;
		stream_info_pcm.num_remain_area	= bf_audio_enc_get_remain_area(enc_ch);
		stream_info_pcm.pcm_data		= (UCHAR*)(gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_data[area_index] + D_AUDIO_ENC_UAC_OFFSET);
		stream_info_pcm.pcm_size		= gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_size[area_index] - D_AUDIO_ENC_UAC_OFFSET;
		stream_info_pcm.uac_data		= (UCHAR*)(gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_data[area_index]);
		stream_info_pcm.uac_size		= gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_size[area_index];

		stream_info_pcm.time_stamp		= gBF_AudioEnc_Stream_Ctrl[enc_ch].duration[area_index];
		stream_info_pcm.error_info		= 0;
		stream_info_pcm.ch_type			= gBF_Audio_Enc_Inst[enc_ch].ch_type;
		stream_info_pcm.frame_length	= gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_count[area_index];
		stream_info_pcm.sampling_freq	= gBF_Audio_Enc_Inst[enc_ch].smpl_rate;
		stream_info_pcm.bit_rate		= gBF_Audio_Enc_Inst[enc_ch].bit_rate;
		stream_info_pcm.stream_end_flag	= end_flag;
		stream_info_pcm.frame_no		= gBF_Audio_Enc_Inst[enc_ch].enc_count-1; /* pgr0063 */
				
		ercd = OS_User_Ref_Sem(SID_AUDIO_ENCDEC, &pk_rsm);
		if( ercd != D_OS_USER_E_OK ){
			BF_Debug_Print_Error(("[AUDIO_ENC]ref_tsk error(%d) L=%d\n", ercd, __LINE__));
		}
		
		if (pk_rsm.semcnt == 0){
			(VOID)BF_AudioEncDec_Sig_Sem();
		}
		
		FJ_Host_PCMStreamCB(gBF_Audio_Enc_Inst[enc_ch].audio_enc_id, &stream_info_pcm);
		
		if (pk_rsm.semcnt == 0){
			(VOID)BF_AudioEncDec_Wai_Sem();
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *	Get number of remain areas
 *	@param		UINT8	audio encoder channel
 *	@param		UINT32	index of stream area
 *	@param		UINT8	flag of end
 *	@return		None	OK/ERROR
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_enc_get_remain_area( UINT8 enc_ch )
{
	UINT32 count;
	UINT32 loop;
	
	count = 0;
	
	for (loop = 0; loop < SDRAM_BNK_AUDIO_STREAM_RECORD; loop++ ){
		if (gBF_AudioEnc_Stream_Ctrl[enc_ch].use_flg[loop] == D_AUDIO_ENC_OFF){
			count++;
		}
	}
	
	return count;
}

/**
 *  @brief		Get Parameter of Initialize(48kHz)
 *	@param		UINT8		audio encoder channel
 *	@param		UINT32*		sampling rate code
 *	@param		UINT32*		bit rate
 *	@return		INT32		ok
 *	@note		None
 *	@attention	None
*/
static INT32 bf_audio_enc_init_param_48khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate )
{
	// [MONO]  range : 8000 <= op5 <= 256000
	// [STEREO]range : 8000 <= op5 <= 128000
	
	if (smpl_aac == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_48khz() param err. smpl_aac=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (bit_rate == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_48khz() param err. bit_rate=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*smpl_aac = AUDIO_AAC_IF_REQ_SMP_48000;
	if (gBF_Audio_Enc_Inst[enc_ch].ch_type == FJ_AUDIO_NUM_CHANNEL_2){
		if (*bit_rate > 128000){
			*bit_rate = 128000;
		}
	}
	else {
		if (*bit_rate > 256000){
			*bit_rate = 256000;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Parameter of Initialize(44.1kHz)
 *	@param		UINT8		audio encoder channel
 *	@param		UINT32*		sampling rate code
 *	@param		UINT32*		bit rate
 *	@return		INT32		ok
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_init_param_44khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate )
{
	// [MONO]  range : 8000 <= op5 <= 256000
	// [STEREO]range : 8000 <= op5 <= 128000
	
	if (smpl_aac == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_44khz() param err. smpl_aac=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (bit_rate == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_44khz() param err. bit_rate=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*smpl_aac = AUDIO_AAC_IF_REQ_SMP_44100;
	if (gBF_Audio_Enc_Inst[enc_ch].ch_type == FJ_AUDIO_NUM_CHANNEL_2){
		if (*bit_rate > 128000){
			*bit_rate = 128000;
		}
	}
	else {
		if (*bit_rate > 256000){
			*bit_rate = 256000;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Parameter of Initialize(32kHz)
 *	@param		UINT8		audio encoder channel
 *	@param		UINT32*		sampling rate code
 *	@param		UINT32*		bit rate
 *	@return		INT32		ok
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_init_param_32khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate )
{
	// [MONO]  range : 8000 <= op5 <= 192000
	// [STEREO]range : 8000 <= op5 <= 128000
	
	if (smpl_aac == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_32khz() param err. smpl_aac=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (bit_rate == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_32khz() param err. bit_rate=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*smpl_aac = AUDIO_AAC_IF_REQ_SMP_32000;
	if (gBF_Audio_Enc_Inst[enc_ch].ch_type == FJ_AUDIO_NUM_CHANNEL_2){
		if (*bit_rate > 128000){
			*bit_rate = 128000;
		}
	}
	else {
		if (*bit_rate > 192000){
			*bit_rate = 192000;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Parameter of Initialize(24kHz)
 *	@param		UINT8		audio encoder channel
 *	@param		UINT32*		sampling rate code
 *	@param		UINT32*		bit rate
 *	@return		INT32		ok
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_init_param_24khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate )
{
	// [MONO]  range : 8000 <= op5 <= 144000
	// [STEREO]range : 8000 <= op5 <= 128000
	
	if (smpl_aac == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_24khz() param err. smpl_aac=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (bit_rate == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_24khz() param err. bit_rate=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*smpl_aac = AUDIO_AAC_IF_REQ_SMP_24000;
	if (gBF_Audio_Enc_Inst[enc_ch].ch_type == FJ_AUDIO_NUM_CHANNEL_2){
		if (*bit_rate > 128000){
			*bit_rate = 128000;
		}
	}
	else {
		if (*bit_rate > 144000){
			*bit_rate = 144000;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Parameter of Initialize(22.05kHz)
 *	@param		UINT8		audio encoder channel
 *	@param		UINT32*		sampling rate code
 *	@param		UINT32*		bit rate
 *	@return		INT32		ok
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_init_param_22khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate )
{
	// [MONO]  range : 8000 <= op5 <= 132300
	// [STEREO]range : 8000 <= op5 <= 128000
	
	if (smpl_aac == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_22khz() param err. smpl_aac=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (bit_rate == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_22khz() param err. bit_rate=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*smpl_aac = AUDIO_AAC_IF_REQ_SMP_22050;
	if (gBF_Audio_Enc_Inst[enc_ch].ch_type == FJ_AUDIO_NUM_CHANNEL_2){
		if (*bit_rate > 128000){
			*bit_rate = 128000;
		}
	}
	else {
		if (*bit_rate > 132300){
			*bit_rate = 132300;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Parameter of Initialize(16kHz)
 *	@param		UINT8		audio encoder channel
 *	@param		UINT32*		sampling rate code
 *	@param		UINT32*		bit rate
 *	@return		INT32		ok
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_init_param_16khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate )
{
	// [MONO]  range : 8000 <= op5 <= 96000
	// [STEREO]range : 8000 <= op5 <= 96000
	
	if (smpl_aac == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_16khz() param err. smpl_aac=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (bit_rate == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_16khz() param err. bit_rate=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*smpl_aac = AUDIO_AAC_IF_REQ_SMP_16000;
	if (*bit_rate > 96000){
		*bit_rate = 96000;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Parameter of Initialize(12kHz)
 *	@param		UINT8		audio encoder channel
 *	@param		UINT32*		sampling rate code
 *	@param		UINT32*		bit rate
 *	@return		INT32		ok
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_init_param_12khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate )
{
	// [MONO]  range : 8000 <= op5 <= 72000
	// [STEREO]range : 8000 <= op5 <= 72000
	
	if (smpl_aac == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_12khz() param err. smpl_aac=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (bit_rate == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_12khz() param err. bit_rate=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*smpl_aac = AUDIO_AAC_IF_REQ_SMP_12000;
	if (*bit_rate > 72000){
		*bit_rate = 72000;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Parameter of Initialize(11.025kHz)
 *	@param		UINT8		audio encoder channel
 *	@param		UINT32*		sampling rate code
 *	@param		UINT32*		bit rate
 *	@return		INT32		ok
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_init_param_11khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate )
{
	// [MONO]  range : 8000 <= op5 <= 66150
	// [STEREO]range : 8000 <= op5 <= 66150
	
	if (smpl_aac == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_11khz() param err. smpl_aac=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (bit_rate == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_11khz() param err. bit_rate=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*smpl_aac = AUDIO_AAC_IF_REQ_SMP_11025;
	if (*bit_rate > 66150){
		*bit_rate = 66150;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Parameter of Initialize(8kHz)
 *	@param		UINT8		audio encoder channel
 *	@param		UINT32*		sampling rate code
 *	@param		UINT32*		bit rate
 *	@return		INT32		ok
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_init_param_8khz( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate )
{
	// [MONO]  range : 8000 <= op5 <= 48000
	// [STEREO]range : 8000 <= op5 <= 48000
	
	if (smpl_aac == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_8khz() param err. smpl_aac=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (bit_rate == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_8khz() param err. bit_rate=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*smpl_aac = AUDIO_AAC_IF_REQ_SMP_8000;
	if (*bit_rate > 48000){
		*bit_rate = 48000;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Parameter of Initialize(Others)
 *	@param		UINT8		audio encoder channel
 *	@param		UINT32*		sampling rate code
 *	@param		UINT32*		bit rate
 *	@return		INT32		ok
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_init_param_others( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate )
{
	if (smpl_aac == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_others() param err. smpl_aac=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (bit_rate == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_init_param_others() param err. bit_rate=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	// Sampling rate is undefined.
	*smpl_aac = AUDIO_AAC_IF_REQ_SMP_48000;
	*bit_rate = 128000;
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Parameter of Initialize
 *	@param		UINT8		audio encoder channel
 *	@param		UINT32*		sampling rate code
 *	@param		UINT32*		bit rate
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_get_encoder_init_param( UINT8 enc_ch, UINT32* smpl_aac, UINT32* bit_rate )
{
	UINT32 loop;
	INT32 ret = D_AUDIO_ERR;
	
	if (bit_rate == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_get_encoder_init_param() param err. bit_rate=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*bit_rate = gBF_Audio_Enc_Inst[enc_ch].bit_rate / gBF_Audio_Enc_Inst[enc_ch].ch_type;		// bit_rate per channel
	
	for (loop = 0; loop < D_AUDIO_ENC_NUM_SMPL_RATE; loop++){
		if ((gBF_Audio_Enc_Inst[enc_ch].smpl_rate == gBF_Audio_Enc_Check_BitRate_Tbl[loop].smpl_rate)	||
			(gBF_Audio_Enc_Check_BitRate_Tbl[loop].smpl_rate == D_AUDIO_ENC_SAMPLE_RATE_OTHERS)			){
			
			ret = gBF_Audio_Enc_Check_BitRate_Tbl[loop].process(enc_ch, smpl_aac, bit_rate);
			break;
		}
	}
	
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_ENC]gBF_Audio_Enc_Check_BitRate_Tbl[%d].process Error L=%d\n", loop, __LINE__));
	}
	
	// minimam check
	if (*bit_rate < 8000){
		*bit_rate = 8000;
	}
	
	*bit_rate *= gBF_Audio_Enc_Inst[enc_ch].ch_type;			// bit_rate(all channel)
	
	return D_AUDIO_OK;
}


/**
 *	@brief		Free audio stream.
 *	@param		UINT8		AudioEncoder Channel
 *	@param		UINT32		Area Index of Stored Sample Table
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_free_astream( UINT8 enc_ch, UINT32 area_index )
{
	OS_USER_ER	ercd;
	
	if (area_index >= SDRAM_BNK_AUDIO_STREAM_RECORD){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_free_astream() param err. area_index is out of range. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ercd = OS_User_Twai_Sem(SID_AUDIO_FREE_STREAM, D_AUDIO_ENC_SEMAPHORE_TIMEOUT);
	if (ercd != D_OS_USER_E_OK) {
		BF_Debug_Print_Error(("[AUDIO_ENC]twai_sem error(%d) L=%d\n", ercd, __LINE__));
		return D_AUDIO_ERR;
	}
	
	gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_size[area_index]	= 0;
	gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_data[area_index]	= 0;
	gBF_AudioEnc_Stream_Ctrl[enc_ch].duration[area_index]		= 0;
	gBF_AudioEnc_Stream_Ctrl[enc_ch].sample_count[area_index]	= 0;
	gBF_AudioEnc_Stream_Ctrl[enc_ch].use_flg[area_index]		= D_AUDIO_ENC_OFF;
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_POINT, "use off ch:%d, idx:%d", enc_ch, area_index));
	
	ercd = OS_User_Sig_Sem(SID_AUDIO_FREE_STREAM);
	if (ercd != D_OS_USER_E_OK) {
		BF_Debug_Print_Error(("[AUDIO_ENC]sig_sem error(%d) L=%d\n", ercd, __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Free audio stream.
 *	@param		UINT8		AudioEncoder Channel
 *	@param		UINT8*		Encode End Flag
 *	@return		INT32		OK/END
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_start_encode_check_stop( UINT8 enc_ch, UINT8* end_flg )
{
	INT32 ret = D_AUDIO_OK;
	
	if (end_flg == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_start_encode_check_stop() param err. end_flg=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_STOP){
		ret = D_AUDIO_END;
	}
	else if (gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_START_PROC){
		gBF_Audio_Enc_Inst[enc_ch].status = D_AUDIO_ENC_STATUS_START;
		ret = D_AUDIO_END;
	}
	else if (gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_STOP_PROC){
		gBF_Audio_Enc_Inst[enc_ch].status = D_AUDIO_ENC_STATUS_STOP;
		*end_flg = D_AUDIO_ENC_ON;
		BF_AudioCapture_Clear_EncStartFlag(gBF_Audio_Enc_Inst[enc_ch].connect_cap_id, gBF_Audio_Enc_Inst[enc_ch].audio_enc_id);
	}
	else if (gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_START_PROC_STOP){
		gBF_Audio_Enc_Inst[enc_ch].status = D_AUDIO_ENC_STATUS_STOP_PROC;
		*end_flg = D_AUDIO_ENC_OFF;
	}
	else {
		*end_flg = D_AUDIO_ENC_OFF;
	}
	
	return ret;
}

/**
 *	@brief		Encode stream.
 *	@param		UINT8						AudioEncoder Channel
 *	@param		T_AUDIO_AAC_ENC_PROC		Encode Parameter
 *	@param		UINT8						End Flag
 *	@return		INT32		OK/ERR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_start_encode_stream_data( UINT8 enc_ch, T_AUDIO_AAC_ENC_PROC* enc_param, UINT8 end_flg )
{
	UINT32 out_address;
	UINT32 out_data_size;
	UINT32 ret;
	
	if (gBF_Audio_Enc_Inst[enc_ch].mute == D_AUDIO_ENC_MUTE){
		// Encoder Mute
		enc_param->in_data_addr = BF_Audio_Data_Get_NullData_Addr();
	}
	
	enc_param->out_data_addr = bf_audio_enc_get_outputbuff_addr(enc_ch) + D_AUDIO_ENC_AAC_PES_OUT_DATA_OFFSET + D_AUDIO_ENC_UAC_OFFSET;
	
	if (enc_param->out_data_addr == D_AUDIO_ENC_AAC_PES_OUT_DATA_OFFSET + D_AUDIO_ENC_UAC_OFFSET){
		BF_Debug_Print_Error(("[AUDIO_ENC]bf_audio_enc_get_outputbuff_addr() Encode Force Stop. L=%d\n", __LINE__));
		
		gBF_Audio_Enc_Inst[enc_ch].status = D_AUDIO_ENC_STATUS_STOP;
		
		BF_AudioCapture_Clear_EncStartFlag(gBF_Audio_Enc_Inst[enc_ch].connect_cap_id, gBF_Audio_Enc_Inst[enc_ch].audio_enc_id);
		
		return D_AUDIO_ERR;
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_POINT, "AAC_START"));
	
	ret = BF_AudioAAC_Enc_Process(enc_param);
	
//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_POINT, "AAC_STOP"));
	
	if ( ret != AAC_RET_SUCCESS ){
		//Error
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioAAC_Enc_Process() %d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	else {
		gBF_Audio_Enc_Inst[enc_ch].enc_count++;
	}
	
	out_address = enc_param->out_data_addr - D_AUDIO_ENC_AAC_PES_OUT_DATA_OFFSET;
	out_data_size = enc_param->out_data_size + D_AUDIO_ENC_AAC_PES_OUT_DATA_OFFSET;
	
	gBF_AudioEnc_Stream_Ctrl[enc_ch].total_time += D_AUDIO_ENC_NUM_SAMPLE;
	
	bf_audio_enc_set_pes_header(out_address,
								out_data_size,
								bf_audio_enc_get_pts(gBF_AudioEnc_Stream_Ctrl[enc_ch].total_time, gBF_Audio_Enc_Inst[enc_ch].smpl_rate));
	
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)out_address, out_data_size);
	
//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_POINT, "SC_START"));
	
	bf_audio_enc_sample_collect(enc_ch, out_address, out_data_size, end_flg);
	
//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_POINT, "SC_END"));

	return D_AUDIO_OK;
}

/**
 *	@brief		Request next encode process.
 *	@param		UINT8		AudioEncoder Channel
 *	@param		UINT8		AudioEncoder ID
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_enc_request_next_encode( UINT8 enc_ch )
{
	if (gBF_AudioEnc_In_Buff_Info[enc_ch].data_num > 1){
		bf_audio_enc_request_encode(enc_ch);
	}
	else if (BF_AudioCapture_Get_BuffBankCount_Enc(gBF_Audio_Enc_Inst[enc_ch].connect_cap_id, gBF_Audio_Enc_Inst[enc_ch].audio_enc_id) > 3){
		bf_audio_enc_request_encode(enc_ch);
	}
	else if (gBF_AudioEnc_ReqEnc_Cnt[enc_ch] > 0){
		bf_audio_enc_request_encode(enc_ch);
	}
	else {
		;
	}
}

/**
 *	@brief		Check AudioEncoder ID
 *	@param		UINT8		AudioEncoder ID
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_check_id( UINT8 enc_ch )
{
	if (gBF_Audio_Enc_Inst[enc_ch].enable == FJ_AUDIO_DISABLE){
		BF_Debug_Print_Error(("[AUDIO_ENC]bf_audio_enc_check_id(). Invalid Instance ID. L=%d\n", __LINE__));
		return D_AUDIO_ERR_INST_DISABLE;
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Get AudioEncoder Channel
 *	@param		UCHAR			AudioEncoder ID
 *	@param		UINT8*			AudioEncoder Channel
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_get_cap_ch( UCHAR audio_enc_id, UINT8* enc_ch )
{
	INT32 ret = D_AUDIO_ERR;
	INT32 loop;
	
	if (enc_ch == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_get_cap_ch() param err. enc_ch=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	for (loop = 0; loop < D_AUDIO_ENC_INST_NUM; loop++){
		if (gBF_Audio_Enc_Inst[loop].audio_enc_id == audio_enc_id){
			*enc_ch = loop;
			ret = D_AUDIO_OK;
			break;
		}
	}
	
	return ret;
}

/**
 *	@brief		Check status of BF_AudioEnc_Start
 *	@param		UCHAR			AudioEncoder ID
 *	@param		UINT8*			AudioEncoder Channel
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_start_chk_status( UINT8 enc_ch )
{
	INT32 ret;
	
	if (gBF_Audio_Enc_Inst[enc_ch].enable == FJ_AUDIO_DISABLE){
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Start(). Invalid Instance ID. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		if (gBF_Audio_Enc_Inst[enc_ch].connect_cap_id == D_AUDIO_ENC_INVALID){
			BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Start(). Not Connected. L=%d\n", __LINE__));
			ret = D_AUDIO_ERR;
		}
		else {
			ret = D_AUDIO_OK;
		}
	}
	
	return ret;
}

/**
 *	@brief		Prepare for pcm data for encoder
 *	@param		UINT8					AudioEncoder Channel
 *	@param		UINT8					AudioEncoder Channel
 *	@param		T_AUDIO_AAC_ENC_PROC	Encoder parameter
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_start_24bit_pcm( UINT8 enc_ch, UINT32 in_address, T_AUDIO_AAC_ENC_PROC* enc_param)
{
	UINT32 offset_size;
	static const UINT32 stream_work_addr_tbl[D_AUDIO_ENC_INST_NUM] = {
		D_AUDIO_ENC_STREAM_WORK0,
		D_AUDIO_ENC_STREAM_WORK1
	};
	UINT8 channel;
	
	if (enc_param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_start_24bit_pcm() param err. enc_param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	channel = BF_AudioCapture_Get_BuffChType(gBF_Audio_Enc_Inst[enc_ch].connect_cap_id);
	
	if ( gBF_Audio_Enc_Inst[enc_ch].offset != 0){
		offset_size = (gBF_Audio_Enc_Inst[enc_ch].offset * 3 * channel);
		
		if (gBF_AudioEnc_PreEncAreaIndex[enc_ch] == D_AUDIO_ENC_INVALID){
			
			memcpy((VOID*)&gBF_AudioEnc_PreEncArea[enc_ch][0][0],
				   (VOID*)(in_address + offset_size),
				   (bf_audio_enc_get_inputbuff_size(enc_ch) - offset_size));
			
			gBF_AudioEnc_PreEncAreaIndex[enc_ch] = 0;
			
			if (gBF_AudioEnc_In_Buff_Info[enc_ch].data_num > 1){
				bf_audio_enc_request_encode(enc_ch);
			}
			return D_AUDIO_END;
		}
		else {
			memcpy((VOID*)&gBF_AudioEnc_PreEncArea[enc_ch][gBF_AudioEnc_PreEncAreaIndex[enc_ch]][(bf_audio_enc_get_inputbuff_size(enc_ch) - offset_size)],
				   (VOID*)in_address,
				   offset_size);
			
			BF_Audio_Data_Pcm_24bitTo16bit((UINT8*)&gBF_AudioEnc_PreEncArea[enc_ch][gBF_AudioEnc_PreEncAreaIndex[enc_ch]][0],
										   (UINT8*)stream_work_addr_tbl[enc_ch],
										   bf_audio_enc_get_inputbuff_size(enc_ch));
			
			gBF_AudioEnc_PreEncAreaIndex[enc_ch]++;
			gBF_AudioEnc_PreEncAreaIndex[enc_ch] %= D_AUDIO_ENC_PRE_ENC_BUF_NUM;
			
			memcpy((VOID*)&gBF_AudioEnc_PreEncArea[enc_ch][gBF_AudioEnc_PreEncAreaIndex[enc_ch]][0],
			       (VOID*)(in_address + offset_size),
			       (bf_audio_enc_get_inputbuff_size(enc_ch) - offset_size));
		}
	}
	else {
		BF_Audio_Data_Pcm_24bitTo16bit((UINT8*)in_address,
									   (UINT8*)stream_work_addr_tbl[enc_ch],
									   bf_audio_enc_get_inputbuff_size(enc_ch));
	}
	
	enc_param->in_data_addr = stream_work_addr_tbl[enc_ch];
	enc_param->in_data_size = (bf_audio_enc_get_inputbuff_size(enc_ch) / 3) * 2;
	
	return D_AUDIO_OK;
}


/**
 *	@brief		Prepare for pcm data for encoder
 *	@param		UINT8					AudioEncoder Channel
 *	@param		UINT8					AudioEncoder Channel
 *	@param		T_AUDIO_AAC_ENC_PROC	Encoder parameter
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_start_16bit_pcm( UINT8 enc_ch, UINT32 in_address, T_AUDIO_AAC_ENC_PROC* enc_param)
{
	UINT32 offset_size;
	UINT8 channel;
	
	if (enc_param == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_enc_start_16bit_pcm() param err. enc_param=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	channel = BF_AudioCapture_Get_BuffChType(gBF_Audio_Enc_Inst[enc_ch].connect_cap_id);
	
	if ( gBF_Audio_Enc_Inst[enc_ch].offset != 0){
		offset_size = (gBF_Audio_Enc_Inst[enc_ch].offset * 2 * channel);
		
		if (gBF_AudioEnc_PreEncAreaIndex[enc_ch] == D_AUDIO_ENC_INVALID){
			memcpy((VOID*)&gBF_AudioEnc_PreEncArea[enc_ch][0][0],
				   (VOID*)(in_address + offset_size),
				   (bf_audio_enc_get_inputbuff_size(enc_ch) - offset_size));
				   
			gBF_AudioEnc_PreEncAreaIndex[enc_ch] = 0;
			
			if (gBF_AudioEnc_In_Buff_Info[enc_ch].data_num > 1){
				bf_audio_enc_request_encode(enc_ch);
			}
			return D_AUDIO_END;
		}
		else {
			memcpy((VOID*)&gBF_AudioEnc_PreEncArea[enc_ch][gBF_AudioEnc_PreEncAreaIndex[enc_ch]][(bf_audio_enc_get_inputbuff_size(enc_ch) - offset_size)],
				   (VOID*)in_address,
				   offset_size);
			
			enc_param->in_data_addr = (UINT32)&gBF_AudioEnc_PreEncArea[enc_ch][gBF_AudioEnc_PreEncAreaIndex[enc_ch]][0];
			
			gBF_AudioEnc_PreEncAreaIndex[enc_ch]++;
			gBF_AudioEnc_PreEncAreaIndex[enc_ch] %= D_AUDIO_ENC_PRE_ENC_BUF_NUM;
			
			memcpy((VOID*)&gBF_AudioEnc_PreEncArea[enc_ch][gBF_AudioEnc_PreEncAreaIndex[enc_ch]][0],
			       (VOID*)(in_address + offset_size),
			       (bf_audio_enc_get_inputbuff_size(enc_ch) - offset_size));
		}
	}
	else {
		enc_param->in_data_addr = in_address;
	}
	
	enc_param->in_data_size = bf_audio_enc_get_inputbuff_size(enc_ch);
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Check Configuration Value
 *	@param		T_FJ_AUDIO_AAC_CONFIG_PARAM	Configuration information
 *	@return		INT32					Result(OK/ERR)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_config_check_param( const T_FJ_AUDIO_AAC_CONFIG_PARAM* audio_enc_config )
{
	if (audio_enc_config->enable_flag > FJ_AUDIO_ENABLE){
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Config(). enable_flag is out of range(=%d). L=%d\n",
								(INT32)audio_enc_config->enable_flag, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	if ((audio_enc_config->sampling_rate != D_AUDIO_SAMPLE_RATE_8000)	&&
		(audio_enc_config->sampling_rate != D_AUDIO_SAMPLE_RATE_11025)	&&
		(audio_enc_config->sampling_rate != D_AUDIO_SAMPLE_RATE_12000)	&&
		(audio_enc_config->sampling_rate != D_AUDIO_SAMPLE_RATE_16000)	&&
		(audio_enc_config->sampling_rate != D_AUDIO_SAMPLE_RATE_22050)	&&
		(audio_enc_config->sampling_rate != D_AUDIO_SAMPLE_RATE_24000)	&&
		(audio_enc_config->sampling_rate != D_AUDIO_SAMPLE_RATE_32000)	&&
		(audio_enc_config->sampling_rate != D_AUDIO_SAMPLE_RATE_44100)	&&
		(audio_enc_config->sampling_rate != D_AUDIO_SAMPLE_RATE_48000)	){
		
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Config(). sampling_rate is out of range(=%d). L=%d\n",
								(INT32)audio_enc_config->sampling_rate, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	
	if (audio_enc_config->out_ch_type > FJ_AUDIO_NUM_CHANNEL_2){
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Config(). out_ch_type is out of range(=%d). L=%d\n",
								(INT32)audio_enc_config->out_ch_type, __LINE__));
		
		return D_AUDIO_ERR_PARAM;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Request encode
 *	@param		UCHAR		audio encoder id
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_request_encode( UINT8 enc_ch )
{
	OS_USER_ER ercd = D_OS_USER_E_OK;
	
//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_POINT, "Req_Enc"));	
	
	// EventFlag Table
	static const OS_USER_FLGPTN event_flg_start_tbl[D_AUDIO_ENC_INST_NUM] = {
		FLG_AUDIO_ENCDEC_ENC0_START,
		FLG_AUDIO_ENCDEC_ENC1_START
	};
	
	if ((gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_START)				||
		(gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_START_PROC)		||
		(gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_START_PROC_STOP)	||
		(gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_STOP_PROC)			){

		if (gBF_AudioEnc_ReqEnc_Cnt[enc_ch] > 0){
			gBF_AudioEnc_ReqEnc_Cnt[enc_ch]--;
		}
		ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, event_flg_start_tbl[enc_ch] );
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_ENC]set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
	else {
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Check Version of AAC Encoder Lib
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_enc_check_aac_lib_version( VOID )
{
//	BF_Debug_Print_Information(("AAC Enc Lib Ver=0x%x\n",BF_AudioAAC_Enc_GetVersion()));
	
	if (BF_AudioAAC_Enc_GetVersion() >= 0x10000){
			return D_AUDIO_OK;
	}
	
	return D_AUDIO_ERR;
}

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/

/**
 *  @brief		Open Audio Encoder instance
 *	@param		E_FJ_AUDIO_ENC_ENUM	Audio Encoder
 *	@param		UCHAR*				Audio Encoder ID
 *	@return		FJ_ERR_CODE			Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEnc_Open( E_FJ_AUDIO_ENC_ENUM audio_enc, UCHAR* audio_enc_id )
{
	INT32 loop;
	UINT8 ret_flg = D_AUDIO_ENC_OFF;
	INT32 ret = D_AUDIO_OK;
	
	if (audio_enc_id == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_AudioEnc_Open() param err. audio_enc_id=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	for (loop = 0; loop < D_AUDIO_ENC_INST_NUM; loop++){
		if (gBF_Audio_Enc_Inst[loop].enable == FJ_AUDIO_DISABLE){
			if ((audio_enc == E_FJ_AUDIO_ENC_AAC_0) || (audio_enc == E_FJ_AUDIO_ENC_AAC_1)){
				if (bf_audio_enc_check_aac_lib_version() == D_AUDIO_OK){
					gBF_Audio_Enc_Inst[loop].enable = FJ_AUDIO_ENABLE;
					*audio_enc_id = gBF_Audio_Enc_Inst[loop].audio_enc_id;
					gBF_Audio_Enc_Inst[loop].type = audio_enc;
					gBF_Audio_Enc_Inst[loop].compress_type = D_AUDIO_ENC_CMPR_AAC;
				}
				else {
					ret = D_AUDIO_ERR;
				}
			}
			else {
				if (gBF_AudioEnc_TestMode != 1){
					BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Open. Not Support Compress Type. L=%d\n", __LINE__));
					ret = D_AUDIO_ERR;
				}
				else {
					if (audio_enc == E_FJ_AUDIO_ENC_NONE_0){
						gBF_Audio_Enc_Inst[loop].enable = FJ_AUDIO_ENABLE;
						*audio_enc_id = gBF_Audio_Enc_Inst[loop].audio_enc_id;
						gBF_Audio_Enc_Inst[loop].type = audio_enc;
						gBF_Audio_Enc_Inst[loop].compress_type = D_AUDIO_ENC_CMPR_PCM;
					}
					else {
						BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Open. Not Support Compress Type. L=%d\n", __LINE__));
						ret = D_AUDIO_ERR;
					}
				}
			}
			ret_flg = D_AUDIO_ENC_ON;
			break;
		}
	}
	
	BF_AudioEncDec_TaskStart();
	
	if (ret_flg == D_AUDIO_ENC_OFF){
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Open. All instance are used. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Close Audio Encoder instance
 *	@param		UCHAR				Audio Encoder ID
 *	@return		FJ_ERR_CODE			Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEnc_Close( UCHAR audio_enc_id )
{
	INT32 ret;
	UINT8 enc_ch;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_enc_get_cap_ch(audio_enc_id, &enc_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_enc_check_id(enc_ch);
		if (ret == D_AUDIO_ERR_PARAM){
			BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Close(). Instance not exist. L=%d\n", __LINE__));
		}
		else {
			gBF_Audio_Enc_Inst[enc_ch].enable = FJ_AUDIO_DISABLE;
			gBF_Audio_Enc_Inst[enc_ch].connect_cap_id = D_AUDIO_ENC_INVALID;
		}
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Connect Audio capture instance
 *	@param		UCHAR				Audio Encoder ID
 *	@param		UCHAR				Audio Capture ID
 *	@return		FJ_ERR_CODE			Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEnc_Connect( UCHAR audio_enc_id, UCHAR audio_cap_id )
{
	INT32 ret;
	UINT8 enc_ch;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_enc_get_cap_ch(audio_enc_id, &enc_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_enc_check_id(enc_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Connect(). Instance not exist. L=%d\n", __LINE__));
		}
		else {
			gBF_Audio_Enc_Inst[enc_ch].connect_cap_id = audio_cap_id;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Connect(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Disconnect audio capture instance
 *	@param		UCHAR				Audio Encoder ID
 *	@param		UCHAR				Audio Capture ID
 *	@return		FJ_ERR_CODE			Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEnc_Disconnect( UCHAR audio_enc_id, UCHAR audio_cap_id )
{
	INT32 ret;
	UINT8 enc_ch;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_enc_get_cap_ch(audio_enc_id, &enc_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_enc_check_id(enc_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Disconnect(). Instance not exist. L=%d\n", __LINE__));
		}
		else {
			gBF_Audio_Enc_Inst[enc_ch].connect_cap_id = D_AUDIO_ENC_INVALID;
		}
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Configuration of audio encoder
 *	@param		UCHAR				Audio Encoder ID
 *	@param		T_FJ_AUDIO_AAC_CONFIG_PARAM		encoder setting
 *	@return		FJ_ERR_CODE			Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEnc_Config( UCHAR audio_enc_id, T_FJ_AUDIO_AAC_CONFIG_PARAM* audio_enc_config )
{
	INT32 ret;
	UINT8 enc_ch;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_enc_get_cap_ch(audio_enc_id, &enc_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_enc_check_id(enc_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Config(). Instance not exist. L=%d\n", __LINE__));
		}
		else {
			ret = bf_audio_enc_config_check_param(audio_enc_config);
			if (ret == D_AUDIO_OK){
				gBF_Audio_Enc_Inst[enc_ch].enc_enable	= audio_enc_config->enable_flag;
				gBF_Audio_Enc_Inst[enc_ch].smpl_rate	= audio_enc_config->sampling_rate;
				gBF_Audio_Enc_Inst[enc_ch].bit_rate	= audio_enc_config->bit_rate;
				gBF_Audio_Enc_Inst[enc_ch].ch_type	= audio_enc_config->out_ch_type;
			}
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Config(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Setup audio encoder
 *	@param		UCHAR				Audio Encoder ID
 *	@return		FJ_ERR_CODE			Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEnc_Setup( UCHAR audio_enc_id )
{
	OS_USER_ER ercd;
	INT32 ret;
	UINT8 enc_ch;
	
	// EventFlag Table
	static const OS_USER_FLGPTN event_flg_init_tbl[D_AUDIO_ENC_INST_NUM] = {
		FLG_AUDIO_ENCDEC_ENC0_INIT,
		FLG_AUDIO_ENCDEC_ENC1_INIT
	};
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_enc_get_cap_ch(audio_enc_id, &enc_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_enc_check_id(enc_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Setup(). Instance not exist. L=%d\n", __LINE__));
		}
		else {
			ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, event_flg_init_tbl[enc_ch] );
			if (ercd != D_OS_USER_E_OK){
				BF_Debug_Print_Error(("[AUDIO_ENC]set_flg error(%d) L=%d\n", ercd, __LINE__));
				ret = D_AUDIO_ERR;
			}
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Setup(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Start audio encoder
 *	@param		UCHAR				Audio Encoder ID
 *	@return		FJ_ERR_CODE			Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEnc_Start( UCHAR audio_enc_id )
{
	UINT32 t_count;
	UINT32 t_count_max = 30;
	INT32 ret;
	UINT8 enc_ch;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_enc_get_cap_ch(audio_enc_id, &enc_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_enc_check_id(enc_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Start(). Instance not exist. L=%d\n", __LINE__));
		}
		else {
			ret = bf_audio_enc_start_chk_status(enc_ch);
			if (ret == D_AUDIO_OK){
				t_count = 0;
				t_count_max = ((D_AUDIO_ENC_NUM_SAMPLE * 1000) / gBF_Audio_Enc_Inst[enc_ch].smpl_rate);
				
				while ( BF_AudioCapture_Get_Status(gBF_Audio_Enc_Inst[enc_ch].connect_cap_id) != D_AUDIO_CAPTURE_STATUS_START){
					OS_User_Dly_Tsk(1);
					t_count++;
					
					if (t_count > t_count_max){
						BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Start(). AudioCapture Not Start. L=%d\n", __LINE__));
						ret = D_AUDIO_ERR_TIMEOUT;
						break;
					}
				}
				
				if (ret == D_AUDIO_OK){
					if (gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_INIT){
						BF_AudioCapture_Set_EncStartBuffPos(gBF_Audio_Enc_Inst[enc_ch].connect_cap_id, audio_enc_id);
						gBF_Audio_Enc_Inst[enc_ch].offset = BF_AudioCapture_Get_SampleOffset(gBF_Audio_Enc_Inst[enc_ch].connect_cap_id);
						gBF_Audio_Enc_Inst[enc_ch].status = D_AUDIO_ENC_STATUS_START_PROC;
						gBF_AudioEnc_ReqEnc_Cnt[enc_ch] = 0;
					}
					else {
						BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Start(). Not Setup. L=%d\n", __LINE__));
						ret = D_AUDIO_ERR;
					}
				}
			}
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Start(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Stop audio encoder
 *	@param		UCHAR				Audio Encoder ID
 *	@return		FJ_ERR_CODE			Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEnc_Stop( UCHAR audio_enc_id )
{
	OS_USER_ER ercd;
	INT32 ret;
	UINT8 enc_ch;
	
	// EventFlag Table
	static const OS_USER_FLGPTN event_flg_stop_tbl[D_AUDIO_ENC_INST_NUM] = {
		FLG_AUDIO_ENCDEC_ENC0_STOP,
		FLG_AUDIO_ENCDEC_ENC1_STOP
	};
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_enc_get_cap_ch(audio_enc_id, &enc_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_enc_check_id(enc_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Stop(). Instance not exist. L=%d\n", __LINE__));
		}
		else {
			ercd = OS_User_Set_Flg(FID_AUDIO_ENCDEC, event_flg_stop_tbl[enc_ch]);
			if (ercd != D_OS_USER_E_OK){
				BF_Debug_Print_Error(("[AUDIO_ENC]set_flg error(%d) L=%d\n", ercd, __LINE__));
				ret = D_AUDIO_ERR;
			}
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Stop(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Free area of stored stream data
 *	@param		UCHAR				Audio Encoder ID
 *	@param		UINT32				index of stream area
 *	@param		BOOL				flag of last free operation
 *	@return		FJ_ERR_CODE			Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		Unavalibalible "OS_User_Wai_Sem(SID_AUDIO_ENCDEC)"
 *	@attention	None
 */
INT32 BF_AudioEnc_FreeIndex( UCHAR audio_enc_id, UINT32 free_astream_index, BOOL last_free_flg )
{
	INT32 ret;
	UINT8 enc_ch;
	
	if (free_astream_index >= SDRAM_BNK_AUDIO_STREAM_RECORD){
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_AudioEnc_FreeIndex() param err. free_astream_index is out of range. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = bf_audio_enc_get_cap_ch(audio_enc_id, &enc_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_enc_check_id(enc_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_FreeIndex(). Instance not exist. L=%d\n", __LINE__));
		}
		else {
			if (last_free_flg == FALSE){
				(VOID)bf_audio_enc_free_astream(enc_ch, free_astream_index);
			}
			else {
				bf_audio_enc_init_stream_ctrl_info(enc_ch);
			}
		}
	}
	
	return ret;
}

/**
 *  @brief		Start mute
 *	@param		UCHAR				Audio Encoder ID
 *	@return		FJ_ERR_CODE			Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEnc_StartMute( UCHAR audio_enc_id )
{
	INT32 ret;
	UINT8 enc_ch;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_enc_get_cap_ch(audio_enc_id, &enc_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_enc_check_id(enc_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_StartMute(). Instance not exist. L=%d\n", __LINE__));
		}
		else {
			gBF_Audio_Enc_Inst[enc_ch].mute = D_AUDIO_ENC_MUTE;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_StartMute(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Stop mute
 *	@param		UCHAR				Audio Encoder ID
 *	@return		FJ_ERR_CODE			Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEnc_StopMute( UCHAR audio_enc_id )
{
	INT32 ret;
	UINT8 enc_ch;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_enc_get_cap_ch(audio_enc_id, &enc_ch);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_enc_check_id(enc_ch);
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_StopMute(). Instance not exist. L=%d\n", __LINE__));
		}
		else {
			gBF_Audio_Enc_Inst[enc_ch].mute = D_AUDIO_ENC_DEMUTE;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_StopMute(). Instance not exist. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Set test mode
 *	@param		UINT8		mode
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioEnc_SetTestMode( UINT8 mode )
{
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	gBF_AudioEnc_TestMode = mode;
	
	(VOID)BF_AudioEncDec_Sig_Sem();
}

/**
 *  @brief		Get test mode
 *	@param		UINT8		mode
 *	@return		None
 *	@note		None
 *	@attention	None
 */
UINT8 BF_AudioEnc_GetTestMode( VOID )
{
	return gBF_AudioEnc_TestMode;
}

/****
 Input Process
****/

/**
 *  @brief		Initialize Encoder
 *	@param		UINT8		audio encoder channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioEnc_Init_Encoder( UINT8 enc_ch )
{
	UINT32 bit_rate;
	UINT32 ret_value;
	UINT32 smpl_aac;
	T_AUDIO_AAC_ENC_INIT enc_param;
	INT32 ret;
	
	bf_audio_enc_init_stream_ctrl_info(enc_ch);
	bf_audio_enc_init_in_buffer_info(enc_ch);
	gBF_Audio_Enc_Inst[enc_ch].enc_count = 0;
	gBF_AudioEnc_PreEncAreaIndex[enc_ch] = D_AUDIO_ENC_INVALID;
	gBF_AudioEnc_BuffRnov_Flg[enc_ch] = 0;
	gBF_AudioEnc_ReqEnc_Cnt[enc_ch] = 0;
	
	ret = bf_audio_enc_get_encoder_init_param(enc_ch, &smpl_aac, &bit_rate);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioEnc_Init_Encoder() Parameter Initialize Error. L=%d\n", __LINE__));
		return;
	}
	
	enc_param.aac_id = enc_ch;
	enc_param.format = AAC_LIB_AACENC;
	enc_param.endian = AAC_ENDIAN_LITTLE;
	
	if (gBF_Audio_Enc_Inst[enc_ch].ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		enc_param.channel = AAC_MODE_STRM_MONO;
	}
	else if (gBF_Audio_Enc_Inst[enc_ch].ch_type == FJ_AUDIO_NUM_CHANNEL_2){
		enc_param.channel = AAC_MODE_STRM_STEREO;
	}
	else {
		// ERROR
		return;
	}
	
	enc_param.smpl_rate = smpl_aac;
	enc_param.bit_rate = bit_rate;
	
	ret_value = BF_AudioAAC_Enc_Init(&enc_param);
	if ( ret_value != AAC_RET_SUCCESS ){
		//Error
		BF_Debug_Print_Error(("[AUDIO_ENC]BF_AudioAAC_Enc_Init() Error. L=%d\n", __LINE__));
	}
	else {
		gBF_Audio_Enc_Inst[enc_ch].status = D_AUDIO_ENC_STATUS_INIT;
	}
}

/**
 *  @brief		Start Encode(AudioEncoder ch0)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioEnc_StartEncode( UINT8 enc_ch )
{
	UINT32 in_address = 0;
	UINT32 out_address;
	UINT32 stream_size;
	T_AUDIO_AAC_ENC_PROC enc_param;
	UINT8 end_flg;
	INT32 ret_buff;
	INT32 ret;
	
	ret = bf_audio_enc_start_encode_check_stop(enc_ch, &end_flg);
	if (ret != D_AUDIO_OK) {
		return ;
	}
	
	enc_param.aac_id = enc_ch;
	enc_param.soft_gain = 0;
	
	ret_buff = bf_audio_enc_get_inputbuff_addr(enc_ch, gBF_Audio_Enc_Inst[enc_ch].connect_cap_id, &in_address);
	if (ret_buff != D_AUDIO_OK){
		return;
	}
	
	if (in_address == 0){
		OS_User_Dly_Tsk(10);
		bf_audio_enc_request_encode(enc_ch);
		return ;
	}
	
	if (gBF_AudioEnc_TestMode != 1){
		if (BF_AudioCapture_Get_Buff_BytePerSample(gBF_Audio_Enc_Inst[enc_ch].connect_cap_id) == 3){
			// 24bitsPerSample
			ret = bf_audio_enc_start_24bit_pcm(enc_ch, in_address, &enc_param);
			if (ret != D_AUDIO_OK){
				return ;
			}
		}
		else {
			// 16bitsPerSample
			ret = bf_audio_enc_start_16bit_pcm(enc_ch, in_address, &enc_param);
			if (ret != D_AUDIO_OK){
				return ;
			}
		}
		
		ret = bf_audio_enc_start_encode_stream_data(enc_ch, &enc_param, end_flg);
		if ( ret != D_AUDIO_OK ){
			//Error
			BF_Debug_Print_Error(("[AUDIO_ENC]bf_audio_enc_start_encode_stream_data() Error. L=%d\n", __LINE__));
			
			return;
		}
	}
	else {
		out_address = bf_audio_enc_get_outputbuff_addr(enc_ch) + D_AUDIO_ENC_UAC_OFFSET;
		if (out_address == 0){
			BF_Debug_Print_Error(("[AUDIO_ENC]bf_audio_enc_get_outputbuff_addr() Encode Force Stop. L=%d\n", __LINE__));
			
			gBF_Audio_Enc_Inst[enc_ch].status = D_AUDIO_ENC_STATUS_STOP;
			BF_AudioCapture_Clear_EncStartFlag(gBF_Audio_Enc_Inst[enc_ch].connect_cap_id, gBF_Audio_Enc_Inst[enc_ch].audio_enc_id);
			return;
		}
		
		stream_size = bf_audio_enc_get_inputbuff_size(enc_ch);
		memcpy((VOID*)out_address, (VOID*)in_address, stream_size);
		
		gBF_Audio_Enc_Inst[enc_ch].enc_count++;
		gBF_AudioEnc_Stream_Ctrl[enc_ch].total_time += D_AUDIO_ENC_NUM_SAMPLE;
		bf_audio_enc_sample_collect(enc_ch, out_address, stream_size, end_flg);
	}
	
	(VOID)bf_audio_enc_renov_inputbuff_out(enc_ch);
	bf_audio_enc_request_next_encode(enc_ch);
}

/**
 *  @brief		Stop encoder
 *	@param		UINT8		audio encoder channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioEnc_StopEncode( UINT8 enc_ch )
{
	if (gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_START){
		if (gBF_AudioEnc_PreEncAreaIndex[enc_ch] == D_AUDIO_ENC_INVALID){
			// Before getting the first audio data for encoding.
			gBF_Audio_Enc_Inst[enc_ch].status = D_AUDIO_ENC_STATUS_START_PROC_STOP;
		}
		else {
			gBF_Audio_Enc_Inst[enc_ch].status = D_AUDIO_ENC_STATUS_STOP_PROC;
		}
	}
	else if (gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_START_PROC){
		gBF_Audio_Enc_Inst[enc_ch].status = D_AUDIO_ENC_STATUS_START_PROC_STOP;
	}
}

/**
 *  @brief		Transfer Stream Data
 *	@param		UINT8	audio encoder channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioEnc_TxStreamData( UINT8 enc_ch )
{
	(VOID)bf_audio_enc_stream_send(enc_ch,
								   gBF_AudioEnc_TxStreamData_Info[enc_ch].area_index,
								   gBF_AudioEnc_TxStreamData_Info[enc_ch].end_flag);
}

/**
 *  @brief		Request encode
 *	@param		UCHAR		audio encoder id
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEnc_Request_Encode( UCHAR audio_enc_id, UINT8 enc_cnt)
{
	INT32 ret;
	UINT8 enc_ch;
	
	ret = bf_audio_enc_get_cap_ch(audio_enc_id, &enc_ch);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_ENC]bf_audio_enc_get_cap_ch(). Instance not exist. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if ((gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_START)				||
		(gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_START_PROC)		||
		(gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_START_PROC_STOP)	||
		(gBF_Audio_Enc_Inst[enc_ch].status == D_AUDIO_ENC_STATUS_STOP_PROC)			){
		gBF_AudioEnc_ReqEnc_Cnt[enc_ch] += enc_cnt;
	}
	
	return bf_audio_enc_request_encode(enc_ch);
}

/**
 *  @brief		Get Instance Information.
 *	@param		UCHAR		audio encoder channel
 *	@return		None
 *	@note		None
 *	@attention	None
 */
T_AUDIO_ENC_INSTANCE BF_AudioEnc_Get_InstanceInfo( UINT8 index )
{
	return gBF_Audio_Enc_Inst[index];
}
