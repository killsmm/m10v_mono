/**
 * @file		audio_decoder.c
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
#include "audio_decoder.h"
#include "audio_aac_dec.h"
#include "audio_data.h"
#include "audio_aac_if.h"
#include "audio_encdec.h"
#include "audio_out.h"
#include "dd_arm.h"
#include "sdram_map_movie_play.h"
#include "sdram_map_movie_play_define.h"
#include "os_user_custom.h"
#include "debug.h"
#include "mba_if.h"
#include "movie_video_attr.h"


/*----------------------------------------------------------------------*/
/* Compile Option                                                       */
/*----------------------------------------------------------------------*/
//#define CO_AUDIO_DEC_CT

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/

#define D_AUDIO_DEC_NUM_SAMPLE				(1024U)

// ON/OFF
#define D_AUDIO_DEC_ON						(1)
#define D_AUDIO_DEC_OFF						(0)

#define D_AUDIO_DEC_INVALID					(0xFFU)

#define D_AUDIO_DEC_STREAM_BUFF_ARRAY		(193U)
#define D_AUDIO_DEC_OUT_BUFF_ARRAY			(96U)
#define D_AUDIO_DEC_SDRAM_BNK_AUDIO_STREAM	(192U)

#define D_AUDIO_DEC_SEMAPHORE_TIMEOUT		(10)

#define D_AUDIO_DEC_PCM_BUFF_ADDR			(SDRAM_ADR_MOVIE_PLAY_AUDIO_PCM_0)
#define D_AUDIO_DEC_PCM_BUFF_SIZE			(SDRAM_SIZ_MOVIE_PLAY_AUDIO_PCM * SDRAM_BNK_MOVIE_PLAY_AUDIO_PCM)

#define D_AUDIO_DEC_FEED_STS_NO_REQ			(0)			// No Request (Init or After SetStream)
#define D_AUDIO_DEC_FEED_STS_GET_STRM		(1)			// After FJ_Host_VideoPlayback_GetAudioStreamCB()
#define D_AUDIO_DEC_FEED_STS_MEM_ALLC		(2)			// After MemoryAllocate

#define D_AUDIO_DEC_AAC_FRAME_MAX_SIZE		(0x600U)
#define D_AUDIO_DEC_STRM_BUFF_ADDR			(SDRAM_ADR_MOVIE_PLAY_AUDIO_STREAM_0)
#define D_AUDIO_DEC_STRM_BUFF_SIZE			(D_AUDIO_DEC_AAC_FRAME_MAX_SIZE * D_AUDIO_DEC_SDRAM_BNK_AUDIO_STREAM)
#define D_AUDIO_DEC_STRM_BUFF_END			(SDRAM_ADR_MOVIE_PLAY_AUDIO_STREAM_0 + D_AUDIO_DEC_STRM_BUFF_SIZE)

#define D_AUDIO_DEC_NUM_SMPL_RATE			(10U)
#define D_AUDIO_DEC_SAMPLE_RATE_OTHERS		(0xFFFFFFFFU)

#define D_AUDIO_DEC_PCM_THRESHOLD			(4)

#define D_AUDIO_DEC_CON_OUT0				(0)
#define D_AUDIO_DEC_CON_OUT1				(1)
#define D_AUDIO_DEC_NUM_CON_OUT				(2)

#define D_AUDIO_DEC_AAC_FRAME_MAX_SIXE		(0x1536U)

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/

typedef struct{
	UINT32	in_pos;			/**< data int postion */
	UINT32	out_pos;		/**< data out postion */
	UINT32	count;			/**< not read data remain */
	UINT32	data_num;	/**< Number of frame */
}T_AUDIO_DEC_STRM_BUFFER_INFO;

typedef struct{
	UINT32	in_pos;								/**< data int postion */
	UINT32	out_pos[D_AUDIO_DEC_NUM_CON_OUT];	/**< data out postion */
	UINT32	count_out[D_AUDIO_DEC_NUM_CON_OUT];	/**< not read data remain */
	UINT32	data_num;	/**< Number of frame */
}T_AUDIO_DEC_OUT_BUFFER_INFO;

typedef struct {
	UINT32					addr;			// Input Sampling Rate
	UINT32					size;			// Output Sampling Rate
	UINT8					end_flg;		// end_flg
	UINT64					pts;			// pts
} T_AUDIO_DEC_IN_BUFF;

typedef struct {
	UINT32	smpl_rate;
	INT32	(*process)(UINT8 ch_type, UINT32* audio_spec_cfg);
} T_AUDIO_DEC_SET_SPEC_CFG;


typedef struct {
	E_FJ_MOVIE_PLAY_DECODER_STATUS	dec_status;		// Status of Decoder
	UINT64							pts;			// pts
	BOOL							error;			// TURE:ERROR / FALSE:Normal
} T_AUDIO_DEC_DECODER_CB_INFO;


/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/

static volatile T_AUDIO_DEC_INSTANCE gBF_Audio_Dec_Inst =
{
	D_AUDIO_DEC_STATUS_STOP,		// status
	FJ_AUDIO_DEC_ID_0,				// audio_dec_id
	FJ_AUDIO_DISABLE,				// enable
	E_FJ_AUDIO_DEC_AAC,				// type
	FJ_AUDIO_DISABLE,				// dec_enable
	D_AUDIO_DEC_CMPR_AAC,			// compress_type
	{									// connect_out_id[]
		D_AUDIO_DEC_INVALID,			// connect_out_id[0]
		D_AUDIO_DEC_INVALID				// connect_out_id[1]
	},
	D_AUDIO_SAMPLE_RATE_48000,		// smpl_rate
	FJ_AUDIO_NUM_CHANNEL_2,			// ch_type
	0,								// dec_count
	0,								// current_pts
	0,								// offset
	D_AUDIO_DEC_OFF					// file_end
};

static volatile T_AUDIO_DEC_STRM_BUFFER_INFO gBF_AudioDec_Stream_Buff_Info;
static volatile T_AUDIO_DEC_IN_BUFF gBF_AudioDec_Stream_Buff[D_AUDIO_DEC_STREAM_BUFF_ARRAY];
static volatile T_AUDIO_DEC_OUT_BUFFER_INFO gBF_AudioDec_Out_Buff_Info;
static volatile T_AUDIO_DEC_IN_BUFF gBF_AudioDec_Output_Buff[D_AUDIO_DEC_OUT_BUFF_ARRAY];
static volatile UINT8 gBF_AudioDec_WorkArea[D_AUDIO_DEC_NUM_SAMPLE * 4 * 2];
static volatile UINT8 gBF_AudioDec_Stop_Notification_Flag;
static volatile UINT64 gBF_AudioDec_Last_Pts;
static volatile UINT8 gBF_AudioDec_InitDec_Flg = D_AUDIO_DEC_OFF;
static volatile UINT8 gBF_AudioDec_Error_Flg = D_AUDIO_DEC_OFF;
static volatile UINT32 gBF_AudioDec_StreamNextWritePos;
static volatile UINT32 gBF_AudioDec_StreamNextReadPos;
static volatile T_AUDIO_DEC_DECODER_CB_INFO gBF_AudioDec_DecoderCB_Info;


// Critical Section
// for gBF_AudioDec_Stream_Buff_Info.count
static ULONG gBF_AudioDec_CS_Sb_Cnt __attribute__((section(".LOCK_SECTION"), aligned(64)));

// for gBF_AudioDec_Out_Buff_Info.count
static ULONG gBF_AudioDec_CS_Ob_Cnt __attribute__((section(".LOCK_SECTION"), aligned(64)));

// for gBF_AudioDec_StreamNextWritePost
static ULONG gBF_AudioDec_Cntr_Next_Buff __attribute__((section(".LOCK_SECTION"), aligned(64)));


/*----------------------------------------------------------------------*/
/* Local Function Prototype Definition                                  */
/*----------------------------------------------------------------------*/
static VOID bf_audio_dec_init_stream_buffer_info( VOID );
static INT32 bf_audio_dec_renov_streambuff_in( VOID );
static INT32 bf_audio_dec_renov_streambuff_out( VOID );
static UINT32 bf_audio_dec_get_streambuff_info_cnt( VOID );
static UINT32 bf_audio_dec_get_streambuff_addr( VOID );
static UINT32 bf_audio_dec_get_streambuff_size( VOID );
static UINT64 bf_audio_dec_get_streambuff_pts( VOID );
static UINT64 bf_audio_dec_get_streambuff_pts_next( VOID );
static UINT64 bf_audio_dec_get_out_stream_pts( UCHAR audio_out_id );
static UINT64 bf_audio_dec_get_connect_out_stream_pts( UINT8 connect_out );
static UINT8 bf_audio_dec_get_streambuff_endflg( VOID );

static VOID bf_audio_dec_init_out_buffer_info( VOID );
static INT32 bf_audio_dec_renov_outputbuff_in( VOID );
static INT32 bf_audio_dec_renov_outputbuff_out( UINT8 connect_out );
static UINT32 bf_audio_dec_get_outputbuff_info_cnt( UCHAR audio_out_id );
static UINT32 bf_audio_dec_get_outputbuff_info_cnt_out( UINT8 connect_out );
static UINT32 bf_audio_dec_get_outputbuff_in_addr( VOID );
static UINT32 bf_audio_dec_get_outputbuff_in_size( VOID ) __attribute__((unused));
static UINT32 bf_audio_dec_get_outputbuff_out_addr( UINT8 connect_out );
static UINT32 bf_audio_dec_get_outputbuff_out_size( UINT8 connect_out );
static INT32 bf_audio_dec_set_audio_out( UCHAR audio_out_id );
static INT32 bf_audio_dec_get_stream_buf( UINT32* addr, UINT32* size );
static VOID bf_audio_dec_start_dec_stts_updt( VOID );
static VOID bf_audio_dec_start_dec_end(  BOOL dec_error );
static BOOL bf_audio_dec_start_dec_chk_threshold( UINT8 connect_out );
static VOID bf_audio_dec_start_dec_req_dec( VOID );
static VOID bf_audio_dec_start_dec_callback( BOOL dec_error );
static VOID bf_audio_dec_start_dec_offset_adjust_pcm( UINT32 addr, UINT32 size );
static INT32 bf_audio_dec_chk_inst_id( UCHAR audio_dec_id );
static INT32 bf_audio_dec_cfg_chk_prm( UCHAR audio_dec_id, const T_FJ_AUDIO_DEC_CONFIG_PARAM* audio_dec_config );
static INT32 bf_audio_dec_start_chk_prm( UCHAR audio_dec_id );
static INT32 bf_audio_dec_start_get_video_info( UINT64* start_pts, UINT32* video_time_scale );
static INT32 bf_audio_dec_start_search_start_pos( UINT64 start_pts );
static VOID bf_audio_dec_decode_cb( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG decoded_pts, BOOL error );

static INT32 bf_audio_dec_init_param_48khz( UINT8 ch_type, UINT32* audio_spec_cfg );
static INT32 bf_audio_dec_init_param_44khz( UINT8 ch_type, UINT32* audio_spec_cfg );
static INT32 bf_audio_dec_init_param_32khz( UINT8 ch_type, UINT32* audio_spec_cfg );
static INT32 bf_audio_dec_init_param_24khz( UINT8 ch_type, UINT32* audio_spec_cfg );
static INT32 bf_audio_dec_init_param_22khz( UINT8 ch_type, UINT32* audio_spec_cfg );
static INT32 bf_audio_dec_init_param_16khz( UINT8 ch_type, UINT32* audio_spec_cfg );
static INT32 bf_audio_dec_init_param_12khz( UINT8 ch_type, UINT32* audio_spec_cfg );
static INT32 bf_audio_dec_init_param_11khz( UINT8 ch_type, UINT32* audio_spec_cfg );
static INT32 bf_audio_dec_init_param_8khz( UINT8 ch_type, UINT32* audio_spec_cfg );
static INT32 bf_audio_dec_init_param_others( UINT8 ch_type, UINT32* audio_spec_cfg );

static INT32 bf_audio_dec_check_aac_lib_version( VOID );

/*----------------------------------------------------------------------*/
/* Function Table Definition											*/
/*----------------------------------------------------------------------*/
static const T_AUDIO_DEC_SET_SPEC_CFG gBF_Audio_Dec_Set_SpecCfg_Tbl[D_AUDIO_DEC_NUM_SMPL_RATE] = {
	{	D_AUDIO_SAMPLE_RATE_48000,		bf_audio_dec_init_param_48khz		},
	{	D_AUDIO_SAMPLE_RATE_44100,		bf_audio_dec_init_param_44khz		},
	{	D_AUDIO_SAMPLE_RATE_32000,		bf_audio_dec_init_param_32khz		},
	{	D_AUDIO_SAMPLE_RATE_24000,		bf_audio_dec_init_param_24khz		},
	{	D_AUDIO_SAMPLE_RATE_22050,		bf_audio_dec_init_param_22khz		},
	{	D_AUDIO_SAMPLE_RATE_16000,		bf_audio_dec_init_param_16khz		},
	{	D_AUDIO_SAMPLE_RATE_12000,		bf_audio_dec_init_param_12khz		},
	{	D_AUDIO_SAMPLE_RATE_11025,		bf_audio_dec_init_param_11khz		},
	{	D_AUDIO_SAMPLE_RATE_8000,		bf_audio_dec_init_param_8khz		},
	{	D_AUDIO_DEC_SAMPLE_RATE_OTHERS,	bf_audio_dec_init_param_others		},
};

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/

/**
 *  @brief		Initialize Output Buffer Control Information.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_dec_init_stream_buffer_info( VOID )
{
	UINT32 loop;
	
	gBF_AudioDec_Stream_Buff_Info.in_pos	= 0;
	gBF_AudioDec_Stream_Buff_Info.out_pos	= 0;
	gBF_AudioDec_Stream_Buff_Info.count		= 0;
	gBF_AudioDec_Stream_Buff_Info.data_num	= 0;
	
	for (loop = 0; loop < D_AUDIO_DEC_STREAM_BUFF_ARRAY; loop++){
		gBF_AudioDec_Stream_Buff[loop].addr	 	= 0;
		gBF_AudioDec_Stream_Buff[loop].size		= 0;
		gBF_AudioDec_Stream_Buff[loop].end_flg	= D_AUDIO_DEC_OFF;
		gBF_AudioDec_Stream_Buff[loop].pts		= 0;
	}
}

/**
 *  @brief		Stream Buffer input position Renovation.
 *	@param		None
 *	@return		INT32		OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_renov_streambuff_in( VOID )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioDec_CS_Sb_Cnt );
	
	if (gBF_AudioDec_Stream_Buff_Info.count <  D_AUDIO_DEC_STREAM_BUFF_ARRAY - 1){
		gBF_AudioDec_Stream_Buff_Info.in_pos++;
		gBF_AudioDec_Stream_Buff_Info.in_pos %= D_AUDIO_DEC_STREAM_BUFF_ARRAY;
		
		gBF_AudioDec_Stream_Buff_Info.count++;	// counter increment
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no empty area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioDec_CS_Sb_Cnt );
	
	return retcd;
}

/**
 *  @brief		Stream Buffer output position Renovation.
 *	@param		None
 *	@return		INT32		OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_renov_streambuff_out( VOID )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioDec_CS_Sb_Cnt );
	
	if (gBF_AudioDec_Stream_Buff_Info.count > 1){
		gBF_AudioDec_Stream_Buff_Info.out_pos++;
		gBF_AudioDec_Stream_Buff_Info.out_pos %= D_AUDIO_DEC_STREAM_BUFF_ARRAY;
		
		gBF_AudioDec_Stream_Buff_Info.count--;		// counter decrement
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no busy area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioDec_CS_Sb_Cnt );
	
	return retcd;
}

/**
 *  @brief		Get AUDIO Stream buffer Count Information
 *	@param		None
 *	@return		UINT32		Count Information
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_dec_get_streambuff_info_cnt( VOID )
{
	return gBF_AudioDec_Stream_Buff_Info.count;
}

/**
 *  @brief		Get AUDIO Stream buffer address
 *	@param		None
 *	@return		UINT32		address
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_dec_get_streambuff_addr( VOID )
{
	return gBF_AudioDec_Stream_Buff[gBF_AudioDec_Stream_Buff_Info.out_pos].addr;
}

/**
 *  @brief		Get AUDIO Stream buffer size
 *	@param		None
 *	@return		UINT32		size
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_dec_get_streambuff_size( VOID )
{
	return gBF_AudioDec_Stream_Buff[gBF_AudioDec_Stream_Buff_Info.out_pos].size;
}

/**
 *  @brief		Get PTS of AUDIO Stream
 *	@param		None
 *	@return		UINT64		PTS
 *	@note		None
 *	@attention	None
 */
static UINT64 bf_audio_dec_get_streambuff_pts( VOID )
{
	return gBF_AudioDec_Stream_Buff[gBF_AudioDec_Stream_Buff_Info.out_pos].pts;
}

/**
 *  @brief		Get next PTS of AUDIO Stream
 *	@param		None
 *	@return		UINT64		PTS
 *	@note		None
 *	@attention	None
 */
static UINT64 bf_audio_dec_get_streambuff_pts_next( VOID )
{
	return gBF_AudioDec_Stream_Buff[(gBF_AudioDec_Stream_Buff_Info.out_pos + 1) % D_AUDIO_DEC_STREAM_BUFF_ARRAY].pts;
}

/**
 *  @brief		Get Output PTS
 *	@param		UCHAR audio out id
 *	@return		UINT64		PTS
 *	@note		None
 *	@attention	None
 */
static UINT64 bf_audio_dec_get_out_stream_pts( UCHAR audio_out_id )
{
	if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] == audio_out_id){
		return bf_audio_dec_get_connect_out_stream_pts(D_AUDIO_DEC_CON_OUT0);
	}
	else if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] == audio_out_id){
		return bf_audio_dec_get_connect_out_stream_pts(D_AUDIO_DEC_CON_OUT1);
	}
	else {
		return 0;
	}
}

/**
 *  @brief		Get Connect AudioOut PTS
 *	@param		None
 *	@return		UINT64		PTS
 *	@note		None
 *	@attention	None
 */
static UINT64 bf_audio_dec_get_connect_out_stream_pts( UINT8 connect_out )
{
	UINT32 pos;
	
	pos = (gBF_AudioDec_Stream_Buff_Info.out_pos + D_AUDIO_DEC_STREAM_BUFF_ARRAY - gBF_AudioDec_Out_Buff_Info.count_out[connect_out] - 1) % D_AUDIO_DEC_STREAM_BUFF_ARRAY;
	
	return gBF_AudioDec_Stream_Buff[pos].pts;
}

/**
 *  @brief		Get endflf of stream buffer
 *	@param		None
 *	@return		UINT8	end flag
 *	@note		None
 *	@attention	None
 */
static UINT8 bf_audio_dec_get_streambuff_endflg( VOID )
{
	return gBF_AudioDec_Stream_Buff[gBF_AudioDec_Stream_Buff_Info.out_pos].end_flg;
}

/**
 *  @brief		Initialize Output Buffer Control Information.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_dec_init_out_buffer_info( VOID )
{
	INT32 loop;
	UINT32 addr;
	UINT32 size;
	UINT32 end_size;
	
	// Input Data Buffer Setting
	addr = D_AUDIO_DEC_PCM_BUFF_ADDR;
	size = D_AUDIO_DEC_NUM_SAMPLE * gBF_Audio_Dec_Inst.ch_type * 4;
	end_size = D_AUDIO_DEC_PCM_BUFF_SIZE;
	
	gBF_AudioDec_Out_Buff_Info.data_num	= 0;
	
	for (loop = 0; loop < D_AUDIO_DEC_OUT_BUFF_ARRAY; loop++){
		gBF_AudioDec_Output_Buff[loop].addr = addr;
		gBF_AudioDec_Output_Buff[loop].size = D_AUDIO_DEC_NUM_SAMPLE * 2 * gBF_Audio_Dec_Inst.ch_type;
		
		addr += size;
		
		if ((addr + size - D_AUDIO_DEC_PCM_BUFF_ADDR) > end_size){
			gBF_AudioDec_Out_Buff_Info.data_num = loop + 1;
			break;
		}
		
		if (loop >= (D_AUDIO_DEC_OUT_BUFF_ARRAY - 1)){
			gBF_AudioDec_Out_Buff_Info.data_num = loop + 1;
			break;
		}
	}
	
	gBF_AudioDec_Out_Buff_Info.in_pos							= 0;
	gBF_AudioDec_Out_Buff_Info.out_pos[D_AUDIO_DEC_CON_OUT0]	= 0;
	gBF_AudioDec_Out_Buff_Info.out_pos[D_AUDIO_DEC_CON_OUT1]	= 0;
	gBF_AudioDec_Out_Buff_Info.count_out[D_AUDIO_DEC_CON_OUT0]	= 0;
	gBF_AudioDec_Out_Buff_Info.count_out[D_AUDIO_DEC_CON_OUT1]	= 0;
}

/**
 *  @brief		PCM Buffer input position Renovation.
 *	@param		None
 *	@return		INT32		OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_renov_outputbuff_in( VOID )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioDec_CS_Ob_Cnt );
	
	if ((gBF_AudioDec_Out_Buff_Info.count_out[D_AUDIO_DEC_CON_OUT0] <  (gBF_AudioDec_Out_Buff_Info.data_num - 1)) &&
		(gBF_AudioDec_Out_Buff_Info.count_out[D_AUDIO_DEC_CON_OUT1] <  (gBF_AudioDec_Out_Buff_Info.data_num - 1)) ){
		gBF_AudioDec_Out_Buff_Info.in_pos++;
		gBF_AudioDec_Out_Buff_Info.in_pos %= gBF_AudioDec_Out_Buff_Info.data_num;
		
		gBF_AudioDec_Out_Buff_Info.count_out[D_AUDIO_DEC_CON_OUT0]++;	// counter increment
		gBF_AudioDec_Out_Buff_Info.count_out[D_AUDIO_DEC_CON_OUT1]++;	// counter increment
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no empty area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioDec_CS_Ob_Cnt );
	
	return retcd;
}

/**
 *  @brief		PCM Buffer output(AudioOut0) position Renovation.
 *	@param		None
 *	@return		INT32		OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_renov_outputbuff_out( UINT8 connect_out )
{
	INT32 retcd;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioDec_CS_Ob_Cnt );
	
	if (gBF_AudioDec_Out_Buff_Info.count_out[connect_out] > 0){
		gBF_AudioDec_Out_Buff_Info.out_pos[connect_out]++;
		gBF_AudioDec_Out_Buff_Info.out_pos[connect_out] %= gBF_AudioDec_Out_Buff_Info.data_num;
		
		gBF_AudioDec_Out_Buff_Info.count_out[connect_out]--;		// counter decrement
		
		retcd = D_AUDIO_OK;
	}
	else {
		// no busy area
		retcd = D_AUDIO_ERR;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioDec_CS_Ob_Cnt );
	
	return retcd;
}

/**
 *  @brief		Get AUDIO Stream buffer Count Information.
 *	@param		UCHAR		audio out id
 *	@return		UINT32		Count Information
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_dec_get_outputbuff_info_cnt( UCHAR audio_out_id )
{
	if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] == audio_out_id){
		return bf_audio_dec_get_outputbuff_info_cnt_out(D_AUDIO_DEC_CON_OUT0);
	}
	else if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] == audio_out_id){
		return bf_audio_dec_get_outputbuff_info_cnt_out(D_AUDIO_DEC_CON_OUT1);
	}
	else {
		return 0;
	}
}

/**
 *  @brief		Get AUDIO Stream buffer Count Information(AudioOut0).
 *	@param		None
 *	@return		UINT32		Count Information
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_dec_get_outputbuff_info_cnt_out( UINT8 connect_out )
{
	return gBF_AudioDec_Out_Buff_Info.count_out[connect_out];
}

/**
 *  @brief		Get input address of output-buffer
 *	@param		None
 *	@return		UINT32		Address
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_dec_get_outputbuff_in_addr( VOID )
{
	return gBF_AudioDec_Output_Buff[gBF_AudioDec_Out_Buff_Info.in_pos].addr;
}

/**
 *  @brief		Get input size of output-buffer
 *	@param		None
 *	@return		UINT32		size
 *	@note		None
 *	@attention	None
 */

static UINT32 bf_audio_dec_get_outputbuff_in_size( VOID )
{
	return gBF_AudioDec_Output_Buff[gBF_AudioDec_Out_Buff_Info.in_pos].size;
}

/**
 *  @brief		Get output(AudioOut0) address of output-buffer
 *	@param		None
 *	@return		UINT32		Address
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_dec_get_outputbuff_out_addr( UINT8 connect_out )
{
	return gBF_AudioDec_Output_Buff[gBF_AudioDec_Out_Buff_Info.out_pos[connect_out]].addr;
}

/**
 *  @brief		Get output(AudioOut0) size of output-buffer
 *	@param		None
 *	@return		UINT32		Address
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_dec_get_outputbuff_out_size( UINT8 connect_out )
{
	return gBF_AudioDec_Output_Buff[gBF_AudioDec_Out_Buff_Info.out_pos[connect_out]].size;
}

/**
 *  @brief		Set configuration of AudioOut
 *	@param		UCHAR		AudioOut ID
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_set_audio_out( UCHAR audio_out_id )
{
	UINT32 count;
	
	BF_AudioOut_SetChannelType(audio_out_id, gBF_Audio_Dec_Inst.ch_type);
	BF_AudioOut_SetSmplRate(audio_out_id, gBF_Audio_Dec_Inst.smpl_rate);
	
	count= 0;
	while (bf_audio_dec_get_outputbuff_info_cnt(audio_out_id) < 2){
		OS_User_Dly_Tsk(1);
		count++;
		
		if (count > 22 ){
			BF_Debug_Print_Information(("[AUDIO_DEC]BF_AudioDec_StartOutput(). AAC frame is insufficient. num=%d L=%d\n",
										 bf_audio_dec_get_outputbuff_info_cnt(audio_out_id), __LINE__));
			return D_AUDIO_END;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get stream buffer address and size
 *	@param		UINT32*		address
 *	@param		UINT32*		size
 *	@return		INT32		ok/error
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_get_stream_buf( UINT32* addr, UINT32* size )
{
	UINT32 req_size;
	INT32 ret = D_AUDIO_OK;
	
	if (addr == NULL){
		BF_Debug_Print_Error(("[AUDIO_DEC]bf_audio_dec_get_stream_buf() param err. addr=NULL L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (size == NULL){
		BF_Debug_Print_Error(("[AUDIO_DEC]bf_audio_dec_get_stream_buf() param err. size=NULL L=%d\n", __LINE__));
	}
	
	req_size = *size;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioDec_Cntr_Next_Buff );
	
	if (gBF_AudioDec_StreamNextWritePos >= gBF_AudioDec_StreamNextReadPos){
		if (gBF_AudioDec_StreamNextWritePos < D_AUDIO_DEC_STRM_BUFF_END){
			if ((D_AUDIO_DEC_STRM_BUFF_END - gBF_AudioDec_StreamNextWritePos) >= req_size){
//				printf("[0]W:0x%x,R:0x%x,size:0x%x\n",gBF_AudioDec_StreamNextWritePos, gBF_AudioDec_StreamNextReadPos, req_size);
				*addr = gBF_AudioDec_StreamNextWritePos;
			}
			else {
				if ((gBF_AudioDec_StreamNextReadPos - D_AUDIO_DEC_STRM_BUFF_ADDR) > (D_AUDIO_DEC_STRM_BUFF_END - gBF_AudioDec_StreamNextWritePos)){
//			printf("[1]W:0x%x,R:0x%x,size:0x%x\n",gBF_AudioDec_StreamNextWritePos, gBF_AudioDec_StreamNextReadPos, req_size);
					
					gBF_AudioDec_StreamNextWritePos = D_AUDIO_DEC_STRM_BUFF_ADDR;
					*addr = gBF_AudioDec_StreamNextWritePos;
					
					if ((gBF_AudioDec_StreamNextReadPos - D_AUDIO_DEC_STRM_BUFF_ADDR) < req_size){
						ret = D_AUDIO_ERR;
					}
				}
				else {
					ret = D_AUDIO_ERR;
				}
			}
		}
		else {
//			printf("[2]W:0x%x,R:0x%x,size:0x%x\n",gBF_AudioDec_StreamNextWritePos, gBF_AudioDec_StreamNextReadPos, req_size);
			
			*addr = gBF_AudioDec_StreamNextWritePos;
			
			if ((gBF_AudioDec_StreamNextReadPos - D_AUDIO_DEC_STRM_BUFF_ADDR) < req_size){
				ret = D_AUDIO_ERR;
			}
			else {
				gBF_AudioDec_StreamNextWritePos = D_AUDIO_DEC_STRM_BUFF_ADDR;
			}
		}
	}
	else {
//		printf("[3]W:0x%x,R:0x%x,size:0x%x\n",gBF_AudioDec_StreamNextWritePos, gBF_AudioDec_StreamNextReadPos, req_size);
		
		*addr = gBF_AudioDec_StreamNextWritePos;
		
		if ((gBF_AudioDec_StreamNextReadPos - gBF_AudioDec_StreamNextWritePos) <= req_size){
			ret = D_AUDIO_ERR;
		}
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioDec_Cntr_Next_Buff );
	
	return ret;
}

/**
 *  @brief		Update status of BF_AudioDec_Start_Decode()
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_dec_start_dec_stts_updt( VOID )
{
	OS_USER_ER ercd;
	
	if (gBF_Audio_Dec_Inst.status == D_AUDIO_DEC_STATUS_INIT){
		gBF_Audio_Dec_Inst.status = D_AUDIO_DEC_STATUS_START;
	}
	
	if (gBF_Audio_Dec_Inst.status == D_AUDIO_DEC_STATUS_STOP_PROC){
		gBF_Audio_Dec_Inst.status = D_AUDIO_DEC_STATUS_STOP;
		ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_STOP_NTFY );
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
}

/**
 *  @brief		Decode end process of BF_AudioDec_Start_Decode()
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_dec_start_dec_end( BOOL dec_error )
{
	OS_USER_ER ercd;
	
	if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] != D_AUDIO_DEC_INVALID){
		gBF_Audio_Dec_Inst.current_pts = bf_audio_dec_get_out_stream_pts(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0]);
	}
	else if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] != D_AUDIO_DEC_INVALID){
		gBF_Audio_Dec_Inst.current_pts = bf_audio_dec_get_out_stream_pts(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1]);
	}
	else {
		gBF_Audio_Dec_Inst.current_pts = bf_audio_dec_get_streambuff_pts();
	}
	
	// bf_audio_dec_decode_cb(E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST, gBF_Audio_Dec_Inst.current_pts, FALSE);
	
	gBF_AudioDec_DecoderCB_Info.dec_status	= E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST;
	gBF_AudioDec_DecoderCB_Info.pts			= gBF_Audio_Dec_Inst.current_pts;
	gBF_AudioDec_DecoderCB_Info.error		= dec_error;
	
	ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_DEC_CB );
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
	}
	
	gBF_Audio_Dec_Inst.status = D_AUDIO_DEC_STATUS_STOP;
}

/**
 *  @brief		Check PCM Buffer Threshold of decode process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static BOOL bf_audio_dec_start_dec_chk_threshold( UINT8 connect_out )
{
	if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] != D_AUDIO_DEC_INVALID){
		if (bf_audio_dec_get_outputbuff_info_cnt(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0]) < D_AUDIO_DEC_PCM_THRESHOLD){
			return TRUE;
		}
	}
	
	if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] != D_AUDIO_DEC_INVALID){
		if (bf_audio_dec_get_outputbuff_info_cnt(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1]) < D_AUDIO_DEC_PCM_THRESHOLD){
			return TRUE;
		}
	}
	
	return FALSE;
}

/**
 *  @brief		Request decode process of BF_AudioDec_Start_Decode()
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_dec_start_dec_req_dec( VOID )
{
	OS_USER_ER ercd;
	
	if ((bf_audio_dec_start_dec_chk_threshold(D_AUDIO_DEC_CON_OUT0) != FALSE)	||
		(bf_audio_dec_start_dec_chk_threshold(D_AUDIO_DEC_CON_OUT1) != FALSE)	){
		
		ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_START );
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
}

/**
 *  @brief		Callback process of BF_AudioDec_Start_Decode()
 *	@param		BOOL	error of decode
 *	@return		None
 *	@note		None
 *	@attention	None
*/
static VOID bf_audio_dec_start_dec_callback( BOOL dec_error )
{
	OS_USER_ER ercd;
	
	if (bf_audio_dec_get_streambuff_endflg() == D_AUDIO_DEC_ON){
		// bf_audio_dec_decode_cb(E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST, gBF_Audio_Dec_Inst.current_pts, dec_error);
		
		gBF_AudioDec_DecoderCB_Info.dec_status = E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST;
		gBF_AudioDec_DecoderCB_Info.pts = gBF_Audio_Dec_Inst.current_pts;
		gBF_AudioDec_DecoderCB_Info.error = dec_error;
		
		ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_DEC_CB );
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		
		gBF_Audio_Dec_Inst.status = D_AUDIO_DEC_STATUS_STOP;
	}
	else {
		// Request new aac stream
		if (gBF_Audio_Dec_Inst.file_end == D_AUDIO_DEC_OFF){;
			// bf_audio_dec_decode_cb(E_FJ_MOVIE_PLAY_DECODER_STATUS_NORMAL, gBF_Audio_Dec_Inst.current_pts, dec_error);
			
			gBF_AudioDec_DecoderCB_Info.dec_status = E_FJ_MOVIE_PLAY_DECODER_STATUS_NORMAL;
			gBF_AudioDec_DecoderCB_Info.pts = gBF_Audio_Dec_Inst.current_pts;
			gBF_AudioDec_DecoderCB_Info.error = dec_error;
			
			ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_DEC_CB );
			if (ercd != D_OS_USER_E_OK){
				BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
			}
			
			ercd = OS_User_Set_Flg(FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_REQ_STRM);
			if (ercd != D_OS_USER_E_OK){
				BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
			}
		}
	}
}

/**
 *  @brief		Offset adjust pcm data of BF_AudioDec_Start_Decode()
 *	@param		UINT32 address of pcm data
 *	@param		UINT32 size of pcm data
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_dec_start_dec_offset_adjust_pcm( UINT32 addr, UINT32 size )
{
	INT32 ret;
	UINT32 offset_size;
	
	offset_size = gBF_Audio_Dec_Inst.offset * 2 * gBF_Audio_Dec_Inst.ch_type;
	
	if (gBF_Audio_Dec_Inst.dec_count > 3){		// 2 frame is dummy
		if (gBF_Audio_Dec_Inst.offset != 0){
			memcpy((VOID*)(bf_audio_dec_get_outputbuff_in_addr() + size - offset_size),
				   (VOID*)addr,
				   offset_size);
		}
		
		ret = bf_audio_dec_renov_outputbuff_in();
		if (ret != D_AUDIO_OK){
			//Error
			BF_Debug_Print_Error(("[AUDIO_DEC]bf_audio_dec_renov_outputbuff_in(). PCM Buffer Full. %d\n", __LINE__));
		}
	}
	else {
		memset((VOID*)bf_audio_dec_get_outputbuff_in_addr(), 0, size);
	}
	
	memcpy((VOID*)bf_audio_dec_get_outputbuff_in_addr(),
		   (VOID*)(addr + offset_size),
		   (size - offset_size));
}

/**
 *  @brief		Check Audio Decoder ID
 *	@param		UCHAR				audio decoder id
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_chk_inst_id( UCHAR audio_dec_id )
{
	if (audio_dec_id != FJ_AUDIO_DEC_ID_0){
		BF_Debug_Print_Error(("[AUDIO_DEC]bf_audio_dec_chk_inst_id(). Instance ID(=%d) is Invalid. L=%d\n", audio_dec_id, __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (gBF_Audio_Dec_Inst.enable == FJ_AUDIO_DISABLE){
		BF_Debug_Print_Error(("[AUDIO_DEC]bf_audio_dec_chk_inst_id(). Instance ID(=%d) is Invalid. L=%d\n", audio_dec_id, __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Check parameter of BF_AudioDec_Config()
 *	@param		UCHAR				audio decoder id
 *	@param		T_FJ_AUDIO_DEC_CONFIG_PARAM	config parameter
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_cfg_chk_prm( UCHAR audio_dec_id, const T_FJ_AUDIO_DEC_CONFIG_PARAM* audio_dec_config )
{
	INT32 ret;
	
	ret = bf_audio_dec_chk_inst_id(audio_dec_id);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Config(). Instance not exist. L=%d\n", __LINE__));
		return ret;
	}
	
#if 0
	if (audio_dec_config->codec_type != E_FJ_AUDIO_DEC_CODEC_AAC){
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Config(). Not Support CompressType.codec_type=%d, L=%d\n", (INT32)audio_dec_config->codec_type, __LINE__));
		return D_AUDIO_ERR;
	}
#endif
	
	if ((audio_dec_config->audio_ch > FJ_AUDIO_NUM_CHANNEL_2) ||
		(audio_dec_config->audio_ch < FJ_AUDIO_NUM_CHANNEL_1) ){
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Config(). Not Support ChannelType.channel=%d, L=%d\n", (INT32)audio_dec_config->audio_ch, __LINE__));
		return D_AUDIO_ERR;
	}

	if ((audio_dec_config->sampling_rate != D_AUDIO_SAMPLE_RATE_8000)	&&
		(audio_dec_config->sampling_rate != D_AUDIO_SAMPLE_RATE_11025)	&&
		(audio_dec_config->sampling_rate != D_AUDIO_SAMPLE_RATE_12000)	&&
		(audio_dec_config->sampling_rate != D_AUDIO_SAMPLE_RATE_16000)	&&
		(audio_dec_config->sampling_rate != D_AUDIO_SAMPLE_RATE_22050)	&&
		(audio_dec_config->sampling_rate != D_AUDIO_SAMPLE_RATE_24000)	&&
		(audio_dec_config->sampling_rate != D_AUDIO_SAMPLE_RATE_32000)	&&
		(audio_dec_config->sampling_rate != D_AUDIO_SAMPLE_RATE_44100)	&&
		(audio_dec_config->sampling_rate != D_AUDIO_SAMPLE_RATE_48000)	){
		
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Config(). Not Support SamplingRate. sampling_rate=%d, L=%d\n", (INT32)audio_dec_config->sampling_rate, __LINE__));
		return D_AUDIO_ERR;
	}

	return D_AUDIO_OK;
}


/**
 *  @brief		Check parameter of BF_AudioDec_Start()
 *	@param		UCHAR				audio decoder id
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_start_chk_prm( UCHAR audio_dec_id )
{
	INT32 ret;
	
	ret = bf_audio_dec_chk_inst_id(audio_dec_id);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Start(). Instance not exist. L=%d\n", __LINE__));
		return ret;
	}
	
	if (gBF_Audio_Dec_Inst.status != D_AUDIO_DEC_STATUS_INIT){
		if (gBF_AudioDec_InitDec_Flg == D_AUDIO_DEC_OFF){
			BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Start(). Not Configure Decoder Setting. Status=%d, L=%d\n", gBF_Audio_Dec_Inst.status, __LINE__));
			return D_AUDIO_ERR;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Get Video parameter of BF_AudioDec_Start()
 *	@param		UINT64*			Start PTS
 *	@param		T_H264_IF_DECODER_PARAM*	video decorder parameter
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_start_get_video_info( UINT64* start_pts, UINT32* video_time_scale )
{

#ifndef CO_AUDIO_DEC_CT
	FJ_ERR_CODE ret_code;
	UINT8 vidoe_codec_type;
	T_H264_IF_DECODER_PARAM h264_dec_param;
	T_H265_IF_DECODER_PARAM h265_dec_param;
	
	if (start_pts == NULL){
		BF_Debug_Print_Error(("[AUDIO_DEC]bf_audio_dec_start_get_video_info(). Param Error. start_pts=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (video_time_scale == NULL){
		BF_Debug_Print_Error(("[AUDIO_DEC]bf_audio_dec_start_get_video_info(). Param Error. video_time_scale=NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret_code = BF_Movie_Video_Attr_Get_Decoder_Play_Start_Pts(start_pts);
	if (ret_code != FJ_ERR_OK){
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_Movie_Video_Attr_Get_Decoder_Play_Start_Pts(). Error L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret_code = BF_Movie_Video_Attr_Get_Decoder_Codec_Type(&vidoe_codec_type);
	if (ret_code != FJ_ERR_OK){
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_Movie_Video_Attr_Get_Decoder_Codec_Type(). Error L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	else {
		if (vidoe_codec_type == FJ_MOVIE_PLAY_CODEC_H264){
			ret_code = BF_Movie_Video_Attr_Get_H264_Decoder_Config(&h264_dec_param);
			if (ret_code != FJ_ERR_OK){
				BF_Debug_Print_Error(("[AUDIO_DEC]BF_Movie_Video_Attr_Get_H264_Decoder_Config(). Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
			
			*video_time_scale = h264_dec_param.time_scale;
			
			if (*video_time_scale == 0){
				BF_Debug_Print_Error(("[AUDIO_DEC]BF_Movie_Video_Attr_Get_H264_Decoder_Config(). Error. Get Time Scale is 0. Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
		}
		else if (vidoe_codec_type == FJ_MOVIE_PLAY_CODEC_H265){
			ret_code = BF_Movie_Video_Attr_Get_H265_Decoder_Config(&h265_dec_param);
			if (ret_code != FJ_ERR_OK){
				BF_Debug_Print_Error(("[AUDIO_DEC]BF_Movie_Video_Attr_Get_H265_Decoder_Config(). Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
			
			*video_time_scale = h265_dec_param.time_scale;
			
			if (*video_time_scale == 0){
				BF_Debug_Print_Error(("[AUDIO_DEC]BF_Movie_Video_Attr_Get_H265_Decoder_Config(). Error. Get Time Scale is 0. Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
		}
		else {
			BF_Debug_Print_Error(("[AUDIO_DEC]BF_Movie_Video_Attr_Get_Decoder_Codec_Type(). Error Invalid CodecType=%d, L=%d\n", vidoe_codec_type, __LINE__));
			return D_AUDIO_ERR;
		}
	}
#else	//CO_AUDIO_DEC_CT
	*start_pts = 0;
	*video_time_scale = 90000;
#endif	//CO_AUDIO_DEC_CT
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Search Start Position
 *	@param		UINT64			start PTS
 *	@return		INT32			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_start_search_start_pos( UINT64 start_pts )
{
	INT32 ret;
	
	if (bf_audio_dec_get_streambuff_info_cnt() > 1){
		while (start_pts >= bf_audio_dec_get_streambuff_pts_next()){
			ret = bf_audio_dec_renov_streambuff_out();
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_DEC]bf_audio_dec_renov_streambuff_out(). Not found sample to play. L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
			
			if (bf_audio_dec_get_streambuff_info_cnt() < 2){
				break;
			}
		}
	}
	
	if (start_pts < bf_audio_dec_get_streambuff_pts()){
//		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Start(). Not found sample to play. startpts=%llx, feedpts=%llx, L=%d\n",
//							 start_pts, bf_audio_dec_get_streambuff_pts(), __LINE__));
		{
			UINT32 spts_up;
			UINT32 spts_low;
			UINT32 fpts_up;
			UINT32 fpts_low;
			
			spts_up = (UINT32)( start_pts >> 32);
			spts_low = (UINT32)( start_pts & 0x00000000FFFFFFFF);
			
			fpts_up = (UINT32)( bf_audio_dec_get_streambuff_pts() >> 32);
			fpts_low = (UINT32)( bf_audio_dec_get_streambuff_pts() & 0x00000000FFFFFFFF);
			
			BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Start(). Not found sample to play. startpts_upper=%x, startpts_lower=%x, feedpts_upper=%x, feedpts_lower=%x,L=%d\n",
								 spts_up, spts_low, fpts_up, fpts_low, __LINE__));
		}
		
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Call callback function "FJ_Host_VideoPlayback_AudioDecodeCB()"
 *	@param		E_FJ_MOVIE_PLAY_DECODER_STATUS			Decoder status
 *	@param		ULLONG									PTS
 *	@param		BOOL									Error
 *	@return		None			OK/ERROR
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_dec_decode_cb( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG decoded_pts, BOOL error )
{
	OS_USER_ER ercd;
	T_OS_USER_RSEM pk_rsm ={0, 0};
	
	ercd = OS_User_Ref_Sem(SID_AUDIO_ENCDEC, &pk_rsm);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("[AUDIO_ENC]ref_tsk error(%d) L=%d\n", ercd, __LINE__));
	}
	
	if (pk_rsm.semcnt == 0){
		(VOID)BF_AudioEncDec_Sig_Sem();
	}
	
	FJ_Host_VideoPlayback_AudioDecodeCB(decoder_status, decoded_pts, error);
	
	if (pk_rsm.semcnt == 0){
		(VOID)BF_AudioEncDec_Wai_Sem();
	}
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
static INT32 bf_audio_dec_init_param_48khz( UINT8 ch_type, UINT32* audio_spec_cfg )
{
	/* AudioSpecificConfig()							*/
	/* audioObjectType			:	5bit(=10b:AAC LC)	*/
	/* samplingFrequencyIndex	:	4bit(=3..15)		*/
	/* channelConfiguration		:	4bit(=1,2)			*/
	/* frameLengthFlag			:	1bit(=0:1024sample)	*/
	/* dependsOnCoreCoder		:	1bit(=0)			*/
	/* extensionFlag			:	1bit(=0)			*/
	
	if (audio_spec_cfg == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_dec_init_param_48khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		*audio_spec_cfg = 0x00001188;
	}
	else {
		*audio_spec_cfg = 0x00001190;
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
static INT32 bf_audio_dec_init_param_44khz( UINT8 ch_type, UINT32* audio_spec_cfg )
{
	if (audio_spec_cfg == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_dec_init_param_44khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		*audio_spec_cfg = 0x00001208;
	}
	else {
		*audio_spec_cfg = 0x00001210;
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
static INT32 bf_audio_dec_init_param_32khz( UINT8 ch_type, UINT32* audio_spec_cfg )
{
	if (audio_spec_cfg == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_dec_init_param_32khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		*audio_spec_cfg = 0x00001288;
	}
	else {
		*audio_spec_cfg = 0x00001290;
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
static INT32 bf_audio_dec_init_param_24khz( UINT8 ch_type, UINT32* audio_spec_cfg )
{
	if (audio_spec_cfg == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_dec_init_param_24khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		*audio_spec_cfg = 0x00001308;
	}
	else {
		*audio_spec_cfg = 0x00001310;
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
static INT32 bf_audio_dec_init_param_22khz( UINT8 ch_type, UINT32* audio_spec_cfg )
{
	if (audio_spec_cfg == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_dec_init_param_22khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		*audio_spec_cfg = 0x00001388;
	}
	else {
		*audio_spec_cfg = 0x00001390;
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
static INT32 bf_audio_dec_init_param_16khz( UINT8 ch_type, UINT32* audio_spec_cfg )
{
	if (audio_spec_cfg == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_dec_init_param_16khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		*audio_spec_cfg = 0x00001408;
	}
	else {
		*audio_spec_cfg = 0x00001410;
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
static INT32 bf_audio_dec_init_param_12khz( UINT8 ch_type, UINT32* audio_spec_cfg )
{
	if (audio_spec_cfg == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_dec_init_param_12khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		*audio_spec_cfg = 0x00001488;
	}
	else {
		*audio_spec_cfg = 0x00001490;
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
static INT32 bf_audio_dec_init_param_11khz( UINT8 ch_type, UINT32* audio_spec_cfg )
{
	if (audio_spec_cfg == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_dec_init_param_11khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		*audio_spec_cfg = 0x00001508;
	}
	else {
		*audio_spec_cfg = 0x00001510;
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
static INT32 bf_audio_dec_init_param_8khz( UINT8 ch_type, UINT32* audio_spec_cfg )
{
	if (audio_spec_cfg == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_dec_init_param_8khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (ch_type == FJ_AUDIO_NUM_CHANNEL_1){
		*audio_spec_cfg = 0x00001588;
	}
	else {
		*audio_spec_cfg = 0x00001590;
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
static INT32 bf_audio_dec_init_param_others( UINT8 ch_type, UINT32* audio_spec_cfg )
{
	*audio_spec_cfg = 0x00001190;
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Check Version of AAC Decoder Lib
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_dec_check_aac_lib_version( VOID )
{
//	BF_Debug_Print_Information(("AAC Dec Lib Ver=0x%x\n",BF_AudioAAC_Dec_GetVersion()));
	
	if (BF_AudioAAC_Dec_GetVersion() >= 0x10000){
			return D_AUDIO_OK;
	}
	
	return D_AUDIO_ERR;
}


/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/

/**
 *  @brief		Open Audio Decoder instance
 *	@param		E_FJ_AUDIO_DEC_ENUM	Audio Decoder
 *	@param		UCHAR*				Audio Decoder ID
 *	@return		INT32				Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_Open( E_FJ_AUDIO_DEC_ENUM audio_dec, UCHAR* audio_dec_id )
{
	INT32 ret = D_AUDIO_OK;
	
	if (audio_dec_id == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_AudioDec_Open() Param Err. audio_dec_id = NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	if (gBF_Audio_Dec_Inst.enable == FJ_AUDIO_DISABLE){
		if (audio_dec == E_FJ_AUDIO_DEC_AAC){
			if (bf_audio_dec_check_aac_lib_version() == D_AUDIO_OK){
				gBF_Audio_Dec_Inst.enable = FJ_AUDIO_ENABLE;
				*audio_dec_id = gBF_Audio_Dec_Inst.audio_dec_id;
				gBF_Audio_Dec_Inst.type = audio_dec;
				gBF_Audio_Dec_Inst.compress_type = D_AUDIO_DEC_CMPR_AAC;
			}
			else {
				ret = D_AUDIO_ERR;
			}
		}
		else {
			*audio_dec_id = 0;
			BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Open. Not Support Compress Type. L=%d\n", __LINE__));
			ret = D_AUDIO_ERR;
		}
	}
	else {
		*audio_dec_id = 0;
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Open. All instance are used. L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	BF_AudioEncDec_TaskStart();
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Close Audio Decoder instance
 *	@param		UCHAR		Audio Decoder ID
 *	@return		INT32		Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_Close( UCHAR audio_dec_id )
{
	INT32 ret = D_AUDIO_OK;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	if (audio_dec_id == FJ_AUDIO_DEC_ID_0){
		gBF_Audio_Dec_Inst.enable = FJ_AUDIO_DISABLE;
		
		if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] != D_AUDIO_DEC_INVALID){
			BF_AudioOut_DisconnectDec(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0], audio_dec_id);
			gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] = D_AUDIO_DEC_INVALID;
		}
		
		if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] != D_AUDIO_DEC_INVALID){
			BF_AudioOut_DisconnectDec(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1], audio_dec_id);
			gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] = D_AUDIO_DEC_INVALID;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Close(). Instance ID(=%d) is Invalid. L=%d\n", audio_dec_id, __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Connect Audio Out instance
 *	@param		UCHAR		Audio Decoder ID
 *	@param		UCHAR		Audio Out ID
 *	@return		INT32		Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_Connect( UCHAR audio_dec_id, UCHAR audio_out_id )
{
	INT32 ret;
	INT32 ret_val = D_AUDIO_OK;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	if (audio_dec_id == FJ_AUDIO_DEC_ID_0){
		if (gBF_Audio_Dec_Inst.enable == FJ_AUDIO_ENABLE){
			if ((gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] != audio_out_id)	&&
				(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] != audio_out_id)	){
				
				if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] == D_AUDIO_DEC_INVALID){
					ret = BF_AudioOut_ConnectDec(audio_out_id, audio_dec_id);
					if (ret != D_AUDIO_OK){
						ret_val = D_AUDIO_ERR;
						BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioOut_ConnectDec() Error. AudioOut is allready connected. L=%d\n", __LINE__));
					}
					else {
						gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] = audio_out_id;
					}
				}
				else if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1]  == D_AUDIO_DEC_INVALID){
					ret = BF_AudioOut_ConnectDec(audio_out_id, audio_dec_id);
					if (ret != D_AUDIO_OK){
						ret_val = D_AUDIO_ERR;
						BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioOut_ConnectDec() Error. AudioOut is allready connected. L=%d\n", __LINE__));
					}
					else {
						gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] = audio_out_id;
					}
				}
				else {
					BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Connect() Instance is full. L=%d\n", __LINE__));
					ret_val = D_AUDIO_ERR;
				}
				
				if (ret_val == D_AUDIO_OK){
					BF_AudioOut_ConnectDec(audio_out_id, audio_dec_id);
				}
			}
		}
		else {
			BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Connect(). Instance ID(=%d) is Invalid. L=%d\n", audio_dec_id, __LINE__));
			ret_val = D_AUDIO_ERR_PARAM;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Connect(). Instance ID(=%d) is Invalid. L=%d\n", audio_dec_id, __LINE__));
		ret_val = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret_val;
}

/**
 *  @brief		Disconnect Audio Out instance
 *	@param		UCHAR				Audio Decoder ID
 *	@param		UCHAR				Audio Out ID
 *	@return		INT32				Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_Disconnect( UCHAR audio_dec_id, UCHAR audio_out_id )
{
	INT32 ret = D_AUDIO_OK;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	if (audio_dec_id == FJ_AUDIO_DEC_ID_0){
		if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] == audio_out_id){
			gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] = D_AUDIO_DEC_INVALID;
		}
		else if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1]  == audio_out_id){
			gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1]  = D_AUDIO_DEC_INVALID;
		}
		else {
			;
		}
		
		BF_AudioOut_DisconnectDec(audio_out_id, audio_dec_id);
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Disconnect(). Instance ID(=%d) is Invalid. L=%d\n", audio_dec_id, __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Initialize AudioDecoder instance
 *	@param		UCHAR				Audio Decoder ID
 *	@return		INT32				Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_Init( UCHAR audio_dec_id )
{
	INT32 ret = D_AUDIO_OK;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	if (audio_dec_id == FJ_AUDIO_DEC_ID_0){
		if (gBF_Audio_Dec_Inst.enable == FJ_AUDIO_ENABLE){
			bf_audio_dec_init_stream_buffer_info();
			
			gBF_Audio_Dec_Inst.dec_count	= 0;
			gBF_Audio_Dec_Inst.current_pts	= 0;
			gBF_Audio_Dec_Inst.file_end		= D_AUDIO_DEC_OFF;
			gBF_AudioDec_InitDec_Flg		= D_AUDIO_DEC_OFF;
			gBF_AudioDec_Error_Flg			= D_AUDIO_DEC_OFF;
			
			Dd_ARM_Critical_Section_Start( gBF_AudioDec_Cntr_Next_Buff );
			gBF_AudioDec_StreamNextWritePos	= D_AUDIO_DEC_STRM_BUFF_ADDR;
			gBF_AudioDec_StreamNextReadPos	= D_AUDIO_DEC_STRM_BUFF_ADDR;
			Dd_ARM_Critical_Section_End( gBF_AudioDec_Cntr_Next_Buff );
		}
		else {
			BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Init(). Instance ID(=%d) is Invalid. L=%d\n", audio_dec_id, __LINE__));
			ret = D_AUDIO_ERR_PARAM;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Init(). Instance ID(=%d) is Invalid. L=%d\n", audio_dec_id, __LINE__));
		ret = D_AUDIO_ERR_PARAM;
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Configuration AudioDecoder instance
 *	@param		UCHAR							Audio Decoder ID
 *	@param		T_FJ_AUDIO_DEC_CONFIG_PARAM		Configuration Information
 *	@return		INT32							Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_Config( UCHAR audio_dec_id, T_FJ_AUDIO_DEC_CONFIG_PARAM* audio_dec_config )
{
	INT32 ret;
	OS_USER_ER ercd;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_dec_cfg_chk_prm(audio_dec_id, audio_dec_config);
	if (ret == D_AUDIO_OK){
		if(audio_dec_config->codec_type == E_FJ_AUDIO_DEC_CODEC_PCM){
			gBF_Audio_Dec_Inst.type				= E_FJ_AUDIO_DEC_NONE_0;
			gBF_Audio_Dec_Inst.compress_type	= D_AUDIO_DEC_CMPR_PCM;
		}
		else{
			gBF_Audio_Dec_Inst.type				= E_FJ_AUDIO_DEC_AAC;
			gBF_Audio_Dec_Inst.compress_type	= D_AUDIO_DEC_CMPR_AAC;
		}
		gBF_Audio_Dec_Inst.ch_type			= audio_dec_config->audio_ch;
		gBF_Audio_Dec_Inst.smpl_rate		= audio_dec_config->sampling_rate;
		
		bf_audio_dec_init_out_buffer_info();
		
		ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_INIT );
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
			ret = D_AUDIO_ERR;
		}
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Set AudioStream Data
 *	@param		UCHAR								Audio Decoder ID
 *	@param		UINT32								Number of Stored Stream Data
 *	@param		T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM	Stream Information
 *	@return		INT32								Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_SetAudioStream( UCHAR audio_dec_id, UINT32 store_num, T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM* audio_stream )
{
	UINT32 loop;
	INT32 ret;
	
	ret = bf_audio_dec_chk_inst_id(audio_dec_id);
	if (ret == D_AUDIO_OK){
		if (store_num != 0){
			if ((bf_audio_dec_get_streambuff_info_cnt() + store_num) >= (D_AUDIO_DEC_STREAM_BUFF_ARRAY - 1) ){
				ret = D_AUDIO_ERR_OVERFLOW;
			}
			else {
				for (loop = 0; loop < store_num; loop++){
					gBF_AudioDec_Stream_Buff[gBF_AudioDec_Stream_Buff_Info.in_pos].addr	 	= audio_stream[loop].stream_address;
					gBF_AudioDec_Stream_Buff[gBF_AudioDec_Stream_Buff_Info.in_pos].size		= audio_stream[loop].stream_size;
					gBF_AudioDec_Stream_Buff[gBF_AudioDec_Stream_Buff_Info.in_pos].end_flg	= audio_stream[loop].stream_end_flag;
					gBF_AudioDec_Stream_Buff[gBF_AudioDec_Stream_Buff_Info.in_pos].pts		= audio_stream[loop].pts;
					
					if (audio_stream[loop].stream_end_flag == D_AUDIO_DEC_ON){
						gBF_Audio_Dec_Inst.file_end = D_AUDIO_DEC_ON;
						gBF_AudioDec_Last_Pts = audio_stream[loop].pts;
					}
					
					BF_Audio_Data_Clean_Flush_Dcache((VOID*)audio_stream[loop].stream_address, audio_stream[loop].stream_size);
					
					if (gBF_AudioDec_Stream_Buff[gBF_AudioDec_Stream_Buff_Info.in_pos].size > 0){
						ret = bf_audio_dec_renov_streambuff_in();
						if (ret != D_AUDIO_OK){
							BF_Debug_Print_Error(("[AUDIO_DEC]bf_audio_dec_renov_streambuff_in(). AudioStream Buffer Full. L=%d\n", __LINE__));
							break;
						}
					}
				}
			}
		}
	}
	
	return ret;
}

/**
 *  @brief		Start AudioDecoder
 *	@param		UCHAR		Audio Decoder ID
 *	@return		INT32		Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_Start( UCHAR audio_dec_id )
{
	OS_USER_ER ercd;
	INT32 ret;
	UINT64 start_pts;
	UINT32 video_time_scale;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_dec_start_chk_prm(audio_dec_id);
	if (ret == D_AUDIO_OK){
		gBF_Audio_Dec_Inst.status = D_AUDIO_DEC_STATUS_INIT;
		
		ret = bf_audio_dec_start_get_video_info(&start_pts, &video_time_scale);
		if (ret == D_AUDIO_OK){
			ret = bf_audio_dec_start_search_start_pos(start_pts);
			if (ret == D_AUDIO_OK){
				gBF_AudioDec_Stop_Notification_Flag = D_AUDIO_DEC_OFF;
				gBF_AudioDec_Last_Pts = 0;
				gBF_Audio_Dec_Inst.offset = (UINT32)((DOUBLE)(start_pts - bf_audio_dec_get_streambuff_pts()) * (DOUBLE)gBF_Audio_Dec_Inst.smpl_rate / (DOUBLE)video_time_scale);
				
				if (gBF_Audio_Dec_Inst.offset >= D_AUDIO_DEC_NUM_SAMPLE){
					BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Start(). Not found sample to play. L=%d\n", __LINE__));
					BF_Debug_Print_Error(("start_pts = %llx. stream_pts = %llx. time_scale=%x\n", start_pts, bf_audio_dec_get_streambuff_pts(), video_time_scale));
					ret = D_AUDIO_ERR;
				}
				else {
					ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_START );
					if (ercd != D_OS_USER_E_OK){
						BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
						ret = D_AUDIO_ERR;
					}
				}
			}
		}
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Stop AudioDecoder
 *	@param		UCHAR		Audio Decoder ID
 *	@return		INT32		Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_Stop( UCHAR audio_dec_id )
{
	OS_USER_ER ercd;
	INT32 ret;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_dec_chk_inst_id(audio_dec_id);
	if (ret == D_AUDIO_OK){
		ercd = OS_User_Set_Flg(FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_STOP);
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
			ret = D_AUDIO_ERR;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_Stop(). Instance not exist. L=%d\n", __LINE__));
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Start Output decoded pcm data
 *	@param		UCHAR		Audio Decoder ID
 *	@return		INT32		Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_StartOutput( UCHAR audio_dec_id )
{
	INT32 ret;
	INT32 ret_out0 = D_AUDIO_OK;
	INT32 ret_out1 = D_AUDIO_OK;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_dec_chk_inst_id(audio_dec_id);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] != D_AUDIO_DEC_INVALID){
			ret_out0 = bf_audio_dec_set_audio_out(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0]);
		}
		
		if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] != D_AUDIO_DEC_INVALID){
			ret_out1 = bf_audio_dec_set_audio_out(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1]);
		}
		
		if ((gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] != D_AUDIO_DEC_INVALID)	&&
			(ret_out0 == D_AUDIO_OK)															){
			
			BF_AudioOut_Start_Decoder(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0]);
		}
		
		if ((gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] != D_AUDIO_DEC_INVALID)	&&
			(ret_out1 == D_AUDIO_OK)															){
			
			BF_AudioOut_Start_Decoder(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1]);
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_StartOutput(). Instance not exist. L=%d\n", __LINE__));
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Stop Output decoded pcm data
 *	@param		UCHAR		Audio Decoder ID
 *	@return		INT32		Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_StopOutput( UCHAR audio_dec_id )
{
	INT32 ret;
	
	(VOID)BF_AudioEncDec_Wai_Sem();
	
	ret = bf_audio_dec_chk_inst_id(audio_dec_id);
	if (ret == D_AUDIO_OK){
		if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] != D_AUDIO_DEC_INVALID){
			BF_AudioOut_Stop_Decoder(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0]);
		}
		
		if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] != D_AUDIO_DEC_INVALID){
			BF_AudioOut_Stop_Decoder(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1]);
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_StopOutput(). Instance not exist. L=%d\n", __LINE__));
	}
	
	(VOID)BF_AudioEncDec_Sig_Sem();
	
	return ret;
}

/**
 *  @brief		Allocate Memory area to store stream data
 *	@param		UCHAR		Audio Decoder ID
 *	@param		UINT32		size
 *	@param		UINT32		Address
 *	@return		INT32		Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_MemAllocate( UCHAR audio_dec_id, UINT32* size, UINT32* address )
{
	INT32 ret;
	
	if (size == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_AudioDec_MemAllocate() Param Err. size = NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (address == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_AudioDec_MemAllocate() Param Err. address = NULL. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = bf_audio_dec_chk_inst_id(audio_dec_id);
	if (ret == D_AUDIO_OK){
		ret = bf_audio_dec_get_stream_buf(address, size);
		if (ret == D_AUDIO_OK){
			Dd_ARM_Critical_Section_Start( gBF_AudioDec_Cntr_Next_Buff );
			gBF_AudioDec_StreamNextWritePos += *size;
			Dd_ARM_Critical_Section_End( gBF_AudioDec_Cntr_Next_Buff );
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioDec_MemAllocate(). Instance not exist. L=%d\n", __LINE__));
	}
	
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)*address, (UINT32)*size);
	
	return ret;
}

/**
 *  @brief		Request decode
 *	@param		UCHAR		Audio Decoder ID
 *	@return		INT32		Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_Request_Decode( UCHAR audio_dec_id, UCHAR audio_out_id )
{
	OS_USER_ER ercd;
	INT32 ret = D_AUDIO_OK;
	
	if (audio_dec_id == FJ_AUDIO_DEC_ID_0){
		if (gBF_Audio_Dec_Inst.status != D_AUDIO_DEC_STATUS_STOP){
			
			if ((gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] != D_AUDIO_DEC_INVALID)	&&
				(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] != D_AUDIO_DEC_INVALID)	){
				
				if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] != audio_out_id){
					return D_AUDIO_OK;
				}
			}
			
			if ((bf_audio_dec_get_outputbuff_info_cnt(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0]) < (gBF_AudioDec_Out_Buff_Info.data_num - 10))	&&
				(bf_audio_dec_get_outputbuff_info_cnt(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1]) < (gBF_AudioDec_Out_Buff_Info.data_num - 10))	){
				
				ercd = OS_User_Set_Flg(FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_START);
				
				if (ercd != D_OS_USER_E_OK){
					BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
					ret = D_AUDIO_ERR;
				}
			}
		}
	}
	
	return ret;
}

/**
 *  @brief		Renovation output position of output-buffer
 *	@param		UCHAR		Audio Decoder ID
 *	@return		INT32		Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioDec_Renov_Buff( UCHAR audio_out_id )
{
	INT32 ret = D_AUDIO_OK;
	
	if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] == audio_out_id){
		ret = bf_audio_dec_renov_outputbuff_out(D_AUDIO_DEC_CON_OUT0);
		
		if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] == D_AUDIO_DEC_INVALID){
			(VOID)bf_audio_dec_renov_outputbuff_out(D_AUDIO_DEC_CON_OUT1);
		}
	}
	else if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] == audio_out_id){
		ret = bf_audio_dec_renov_outputbuff_out(D_AUDIO_DEC_CON_OUT1);
		
		if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] == D_AUDIO_DEC_INVALID){
			(VOID)bf_audio_dec_renov_outputbuff_out(D_AUDIO_DEC_CON_OUT0);
		}
	}
	else {
		ret = D_AUDIO_ERR;
	}
	
	return ret;
}

/**
 *  @brief		Get address of output-buffer for output pcm
 *	@param		UCHAR		Audio Out ID
 *	@return		INT32		Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
UINT32 BF_AudioDec_Get_BuffAddr( UCHAR audio_out_id )
{
	UINT32 addr;
	
	if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] == audio_out_id){
		addr = bf_audio_dec_get_outputbuff_out_addr(D_AUDIO_DEC_CON_OUT0);
	}
	else if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] == audio_out_id){
		addr = bf_audio_dec_get_outputbuff_out_addr(D_AUDIO_DEC_CON_OUT1);
	}
	else {
		addr = 0;
	}
	
	return addr;
}

/**
 *  @brief		Get size of output-buffer for output pcm
 *	@param		UCHAR		Audio Out ID
 *	@return		INT32		Result(D_AUDIO_OK/D_AUDIO_ERR)
 *	@note		None
 *	@attention	None
 */
UINT32 BF_AudioDec_Get_BuffSize( UCHAR audio_out_id )
{
	UINT32 size;
	
	if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] == audio_out_id){
		size = bf_audio_dec_get_outputbuff_out_size(D_AUDIO_DEC_CON_OUT0);
	}
	else if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] == audio_out_id){
		size = bf_audio_dec_get_outputbuff_out_size(D_AUDIO_DEC_CON_OUT1);
	}
	else {
		size = 0;
	}
	
	return size;
}

/****
 Input Process
****/

/**
 *  @brief		Initialize Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioDec_Init_Decode( VOID )
{
	UINT32 ret_value;
	T_AUDIO_AAC_DEC_INIT dec_param;
	UINT32 loop;
	INT32 ret = D_AUDIO_ERR;

//	BF_Debug_Print_Warning(("AAC DEC INIT \n"));
	
	for (loop = 0; loop < D_AUDIO_DEC_NUM_SMPL_RATE; loop++){
		if ((gBF_Audio_Dec_Inst.smpl_rate == gBF_Audio_Dec_Set_SpecCfg_Tbl[loop].smpl_rate)		||
			(gBF_Audio_Dec_Set_SpecCfg_Tbl[loop].smpl_rate == D_AUDIO_DEC_SAMPLE_RATE_OTHERS)	){
			
			ret = gBF_Audio_Dec_Set_SpecCfg_Tbl[loop].process(gBF_Audio_Dec_Inst.ch_type, &dec_param.audio_spec_cfg);
			break;
		}
	}
	
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_DEC]gBF_Audio_Dec_Set_SpecCfg_Tbl[%d].process Error L=%d\n", loop, __LINE__));
	}
	
	dec_param.format	= AAC_LIB_AACDEC_RAW;
	dec_param.endian	= AAC_ENDIAN_LITTLE;
	dec_param.cfg_size	= 2;
	
	ret_value = BF_AudioAAC_Dec_Init(&dec_param);
	
	if ( ret_value != AAC_RET_SUCCESS ){
		//Error
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioAAC_Dec_Init() L=%d\n", __LINE__));
	}
	else {
		gBF_Audio_Dec_Inst.status = D_AUDIO_DEC_STATUS_INIT;
		gBF_AudioDec_InitDec_Flg = D_AUDIO_DEC_ON;
	}
}

/**
 *  @brief		Decode Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioDec_Start_Decode( VOID )
{
	UINT32 ret_value;
	T_AUDIO_AAC_DEC_PROC dec_param;
	INT32 ret;
	BOOL dec_error;
	UINT32 data_size_org;
	OS_USER_ER ercd;
	UINT8 endflg;
	
	bf_audio_dec_start_dec_stts_updt();
	
	if ((gBF_Audio_Dec_Inst.status != D_AUDIO_DEC_STATUS_START) || (gBF_AudioDec_Error_Flg == D_AUDIO_DEC_ON)){
		return;
	}
	
	gBF_Audio_Dec_Inst.current_pts = bf_audio_dec_get_streambuff_pts();

	memset(&dec_param, 0x0, sizeof(T_AUDIO_AAC_DEC_PROC));
	
	dec_param.soft_gain = 0;
	dec_param.in_data_addr = bf_audio_dec_get_streambuff_addr();
	dec_param.in_data_size = bf_audio_dec_get_streambuff_size();
	data_size_org = dec_param.in_data_size;
	dec_param.out_data_addr = (UINT32)gBF_AudioDec_WorkArea;
	
	if(gBF_Audio_Dec_Inst.type == E_FJ_AUDIO_DEC_AAC){
	//	printf("Dec addr:0x%x,size:0x%x,op:%d,ip:%d,cnt:%d\n",dec_param.in_data_addr, dec_param.in_data_size, gBF_AudioDec_Stream_Buff_Info.out_pos, gBF_AudioDec_Stream_Buff_Info.in_pos,gBF_AudioDec_Stream_Buff_Info.count);
		ret_value = BF_AudioAAC_Dec_Process(&dec_param);
	}
	else{
		memcpy((VOID*)dec_param.out_data_addr, (VOID*)dec_param.in_data_addr, dec_param.in_data_size);
		dec_param.out_data_size = dec_param.in_data_size;
		ret_value = AAC_RET_SUCCESS;
	}
	
	if (ret_value != AAC_RET_SUCCESS){
		//Error
		dec_error = TRUE;
		BF_Debug_Print_Error(("[AUDIO_DEC]BF_AudioAAC_Dec_Process() Addr=0x%x, Size=0x%x, Line=%d\n",
								dec_param.in_data_addr, data_size_org, __LINE__));
	}
	else {
		dec_error = FALSE;
		gBF_Audio_Dec_Inst.dec_count++;
	}
	
	Dd_ARM_Critical_Section_Start( gBF_AudioDec_Cntr_Next_Buff );
	gBF_AudioDec_StreamNextReadPos = bf_audio_dec_get_streambuff_addr() + bf_audio_dec_get_streambuff_size();
	Dd_ARM_Critical_Section_End( gBF_AudioDec_Cntr_Next_Buff );
	
	endflg = bf_audio_dec_get_streambuff_endflg();
	if (endflg == D_AUDIO_DEC_OFF){
		ret = bf_audio_dec_renov_streambuff_out();
		if (ret != D_AUDIO_OK){
			//Error
			BF_Debug_Print_Error(("[AUDIO_DEC]bf_audio_dec_renov_streambuff_out(). StreamBuffer Empty. %d\n", __LINE__));
			
			// bf_audio_dec_decode_cb(E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST, gBF_Audio_Dec_Inst.current_pts, TRUE);
			gBF_AudioDec_DecoderCB_Info.dec_status = E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST;
			gBF_AudioDec_DecoderCB_Info.pts = gBF_Audio_Dec_Inst.current_pts;
			gBF_AudioDec_DecoderCB_Info.error = TRUE;
			
			ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_DEC_CB );
			if (ercd != D_OS_USER_E_OK){
				BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
			}
			
			gBF_AudioDec_Error_Flg = D_AUDIO_DEC_ON;
			return ;
		}
	}
	
	bf_audio_dec_start_dec_offset_adjust_pcm(dec_param.out_data_addr, dec_param.out_data_size);
	
	if (endflg == D_AUDIO_DEC_OFF){
		bf_audio_dec_start_dec_callback(dec_error);
		bf_audio_dec_start_dec_req_dec();
	}
	else {
		bf_audio_dec_start_dec_end(dec_error);
	}
}

/**
 *  @brief		Decode Stop Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioDec_Stop_Decode( VOID )
{
	OS_USER_ER ercd;
	
	if ((gBF_Audio_Dec_Inst.status == D_AUDIO_DEC_STATUS_START) && (gBF_AudioDec_Error_Flg == D_AUDIO_DEC_OFF)){
		gBF_Audio_Dec_Inst.status = D_AUDIO_DEC_STATUS_STOP_PROC;
		
		ercd = OS_User_Set_Flg(FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_START);
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
	else {
		ercd = OS_User_Set_Flg(FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_STOP_NTFY);
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
}

/**
 *  @brief		Notification fo Decoder stop
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioDec_Stop_Decode_Ntfy( VOID )
{
	OS_USER_ER ercd;
	
	if (gBF_AudioDec_Stop_Notification_Flag == D_AUDIO_DEC_OFF){
		gBF_AudioDec_Stop_Notification_Flag = D_AUDIO_DEC_ON;
		
		if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0] != D_AUDIO_DEC_INVALID){
			gBF_Audio_Dec_Inst.current_pts = bf_audio_dec_get_out_stream_pts(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT0]);
		}
		else if (gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1] != D_AUDIO_DEC_INVALID){
			gBF_Audio_Dec_Inst.current_pts = bf_audio_dec_get_out_stream_pts(gBF_Audio_Dec_Inst.connect_out_id[D_AUDIO_DEC_CON_OUT1]);
		}
		else {
			gBF_Audio_Dec_Inst.current_pts = bf_audio_dec_get_streambuff_pts();
		}
		
		if (gBF_AudioDec_Last_Pts != 0){
			if (gBF_Audio_Dec_Inst.current_pts > gBF_AudioDec_Last_Pts){
				gBF_Audio_Dec_Inst.current_pts = gBF_AudioDec_Last_Pts;
			}
			gBF_AudioDec_Last_Pts = 0;
		}
		
		// bf_audio_dec_decode_cb(E_FJ_MOVIE_PLAY_DECODER_STATUS_IDLE, gBF_Audio_Dec_Inst.current_pts, FALSE);
		gBF_AudioDec_DecoderCB_Info.dec_status = E_FJ_MOVIE_PLAY_DECODER_STATUS_IDLE;
		gBF_AudioDec_DecoderCB_Info.pts = gBF_Audio_Dec_Inst.current_pts;
		gBF_AudioDec_DecoderCB_Info.error = FALSE;
		
		ercd = OS_User_Set_Flg( FID_AUDIO_ENCDEC, FLG_AUDIO_ENCDEC_DEC_DEC_CB );
		if (ercd != D_OS_USER_E_OK){
			BF_Debug_Print_Error(("[AUDIO_DEC]set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		
		bf_audio_dec_init_stream_buffer_info();
		bf_audio_dec_init_out_buffer_info();
		
		gBF_Audio_Dec_Inst.dec_count = 0;
		gBF_Audio_Dec_Inst.current_pts = 0;
		gBF_Audio_Dec_Inst.file_end = D_AUDIO_DEC_OFF;
		gBF_AudioDec_Error_Flg = D_AUDIO_DEC_OFF;
		
		Dd_ARM_Critical_Section_Start( gBF_AudioDec_Cntr_Next_Buff );
		gBF_AudioDec_StreamNextWritePos = D_AUDIO_DEC_STRM_BUFF_ADDR;
		gBF_AudioDec_StreamNextReadPos = D_AUDIO_DEC_STRM_BUFF_ADDR;
		Dd_ARM_Critical_Section_End( gBF_AudioDec_Cntr_Next_Buff );
	}
}

/**
 *  @brief		Request AudioStream Data
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioDec_Ntfy_DecoderInfo( VOID )
{
	bf_audio_dec_decode_cb(gBF_AudioDec_DecoderCB_Info.dec_status,
						   gBF_AudioDec_DecoderCB_Info.pts,
						   gBF_AudioDec_DecoderCB_Info.error);
}

/**
 *  @brief		Request AudioStream Data
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioDec_Req_AudioStream( VOID )
{
	OS_USER_ER ercd;
	T_OS_USER_RSEM pk_rsm ={0, 0};
	
	ercd = OS_User_Ref_Sem(SID_AUDIO_ENCDEC, &pk_rsm);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("[AUDIO_ENC]ref_tsk error(%d) L=%d\n", ercd, __LINE__));
	}
	
	if (pk_rsm.semcnt == 0){
		(VOID)BF_AudioEncDec_Sig_Sem();
	}
	
	FJ_Host_VideoPlayback_GetAudioStreamCB();
	
	if (pk_rsm.semcnt == 0){
		(VOID)BF_AudioEncDec_Wai_Sem();
	}
}

/**
 *  @brief		Get AudioDecoder Instance Information
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
T_AUDIO_DEC_INSTANCE BF_AudioDec_Get_InstanceInfo( VOID )
{
	return gBF_Audio_Dec_Inst;
}
