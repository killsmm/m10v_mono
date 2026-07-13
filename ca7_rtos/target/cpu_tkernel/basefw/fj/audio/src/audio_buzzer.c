/**
 * @file		audio_buzzer.c
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
#include "audio.h"
#include "audio_buzzer.h"
#include "audio_out.h"
#include "audio_encdec.h"
#include "audio_data.h"
#include "mwwav.h"
#include "wav_codec.h"
#include "dd_arm.h"
#include "sdram_map_define.h"
#include "os_user_custom.h"
#include "debug.h"
#include "mba_if.h"


/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
#define D_AUDIO_BUZZER_ON						(1)
#define D_AUDIO_BUZZER_OFF						(0)

#define D_AUDIO_BUZZER_DATA_CNT					(30U)						/**< data buffer count for BEEP sound */
#define D_AUDIO_BZ_WAV_START_NUM				(1)

#define D_AUDIO_BZ_NUM_SAMPLE					(1024U)
#define D_AUDIO_BZ_NUM_FRAME_ADDR_TBL			(500U)

#define D_AUDIO_BZ_OUT_CH0						(0)
#define D_AUDIO_BZ_OUT_CH1						(1)

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
typedef struct {
	UINT8	flg;
	UINT8	index;
	UINT32	smpl_rate;
	UINT32	addr;
	UINT32	data_size;
	UINT8	channel;
	UINT16	bits_per_sample;
} T_AUDIO_BZ_SOUND_DATA;

typedef struct {
	UINT32					addr;			// address
	UINT32					size;			// size
} T_AUDIO_BZ_BUFF;

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
static volatile UINT8 gBF_AudioBZ_WAV_ID_Current = 0;
static volatile UINT8 gBF_AudioBZ_WAV_ID_Index = 0;
static volatile UINT32 gBF_AudioBZ_WAV_Data_Remain_Size = SDRAM_SIZ_AUDIO_BUZZER_DATA_AREA;
static volatile UINT32 gBF_AudioBZ_WAV_Data_Last_Addr = SDRAM_ADR_AUDIO_BUZZER_DATA_AREA;

static volatile T_AUDIO_BZ_BUFF gBF_AudioBZ_WAV_Data_Addr_Tbl[D_AUDIO_OUT_INST_NUM][D_AUDIO_BZ_NUM_FRAME_ADDR_TBL];
static volatile UINT16 gBF_AudioBZ_WAV_Data_Addr_Tbl_Pos[D_AUDIO_OUT_INST_NUM];
static volatile UINT16 gBF_AudioBZ_WAV_Data_Addr_Tbl_Pos_End[D_AUDIO_OUT_INST_NUM];


static volatile T_AUDIO_BZ_SOUND_DATA gBF_Audio_WAV_Sound[D_AUDIO_BUZZER_DATA_CNT] = {
	/* flag, index, smpl_rate, addr, data_size, channel, bits_per_sample */
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  },
	{   0,     0,     0,        0,    0,         0,        0,  }
};

// For WAV Header Lib
WAV_Header_Func FS_Func = { wav_write, wav_read, wav_seek, wav_seek_cur };

static volatile UINT32 gBF_Audio_WavFile_Addr_Base;
static volatile UINT32 gBF_Audio_WavFile_Addr_Current;
static volatile UINT8 gBF_AudioBZ_WAV_Data_BitsPerSample[D_AUDIO_OUT_INST_NUM];
static volatile UINT8 gBF_AudioBZ_WAV_Data_Channel[D_AUDIO_OUT_INST_NUM];

// Critical Section
// for gBF_AudioBZ_WAV_Data_Addr_Tbl_Pos
static ULONG gBF_AudioBZ_Tbl_Pos __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Local Function Prototype Definition                                  */
/*----------------------------------------------------------------------*/
static INT32 bf_audio_bz_wai_sem( VOID );
static INT32 bf_audio_bz_sig_sem( VOID );
static UINT32 bf_audio_buzzer_direct_load_wav( UINT32 addr );
static INT32 bf_audio_buzzer_get_wav_id( UINT32* wav_id );
static INT32 bf_audio_buzzer_clear_wav_id( UINT32 wav_id );
static VOID bf_audio_buzzer_sort_pcm_data( UINT32 copy_loop_cnt, UINT8 dlt_index );
static INT32 bf_audio_buzzer_wav2pcm( UINT32 wav_id, UINT32 wav_file_addr );
static INT32 bf_audio_buzzer_get_pcm_data_size( UINT32* check_size, const mwwav_wav_info* wav_info );
static INT32 bf_audio_buzzer_read_pcm_data( UINT32 wav_id, const mwwav_wav_info* wav_info );
static UINT32 bf_audio_buzzer_search_data_chunk_wav( VOID );
static INT32 bf_audio_buzzer_get_wav_id_info( UINT32 wav_id, UINT32* addr, UINT32* size );
static INT32 bf_audio_buzzer_load_wav_addr_tbl( UINT8 audio_out_ch, UINT32 wav_index, UINT32 addr, UINT32 size );

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
static INT32 bf_audio_bz_wai_sem( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Twai_Sem(SID_AUDIO_BUZZER, 100);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_BZ]twai_sem error(%d)\n", ercd));
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
static INT32 bf_audio_bz_sig_sem( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Sig_Sem(SID_AUDIO_BUZZER);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_BZ]sig_sem error(%d)\n", ercd));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Load wave file to memory
 *	@return		INT32	Number of wave id for play
 *	@note		None
 *	@attention	Return value 0 is an error
 */
static UINT32 bf_audio_buzzer_direct_load_wav( UINT32 addr )
{
	UINT32 wav_id = 0;	// Return code (wave file number)
	INT32 ret;
	UINT32 wav_file_addr;
	
	BF_AudioBuzzer_Set_WavFile_Addr(addr);
	
	// Get Wav ID
	ret = bf_audio_buzzer_get_wav_id( &wav_id );
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_BZ]WavID Get ERR Line=%d\n", __LINE__));
		wav_id = 0;
	}
	else {
		// Get load address
		wav_file_addr = gBF_Audio_WAV_Sound[wav_id - 1].addr;
		if (wav_file_addr == 0){
			bf_audio_buzzer_clear_wav_id(wav_id);
			BF_Debug_Print_Error(("[AUDIO_BZ]Wav Table ID Get ERR : wav_file_addr=0x%x, Line=%d\n", wav_file_addr, __LINE__));
			wav_id = 0;
		}
		else {
			// Load
			ret = bf_audio_buzzer_wav2pcm(wav_id, wav_file_addr);
			if (ret != D_AUDIO_OK){
				// Clear Wav ID
				bf_audio_buzzer_clear_wav_id(wav_id);
				BF_Debug_Print_Error(("[AUDIO_BZ]Wav to PCM : ret=%d, Line=%d\n", ret, __LINE__));
				wav_id = 0;
			}
		}
	}
	
	BF_AudioBuzzer_Clear_WavFile_Addr();
	
	return wav_id;
}

/**
 *  @brief		Get Wav ID
 *	@param		INT32*	wav_id
 *	@return		INT32	OK(0) /Error(1)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_buzzer_get_wav_id( UINT32* wav_id )
{
	INT32 ret;
	UINT8 cnt;
	UINT8 find_flg = D_AUDIO_BUZZER_OFF;
	UINT8 crrnt_id;
	UINT8 id_index;
	
	if (wav_id == NULL){
		BF_Debug_Print_Error(("[AUDIO_BZ]bf_audio_buzzer_get_wav_id() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	bf_audio_bz_wai_sem();
	
	crrnt_id = gBF_AudioBZ_WAV_ID_Current;
	id_index = gBF_AudioBZ_WAV_ID_Index;
	
	if (crrnt_id >= D_AUDIO_BUZZER_DATA_CNT){
		crrnt_id = 0;
	}
	
	if (gBF_AudioBZ_WAV_Data_Remain_Size < 4){
		ret =  D_AUDIO_ERR;
	}
	else {
		// Table search (For Except Shide Show)
		for (cnt = 0; cnt < D_AUDIO_BUZZER_DATA_CNT; cnt++){
			if (gBF_Audio_WAV_Sound[crrnt_id].flg == D_AUDIO_BUZZER_OFF){
				gBF_Audio_WAV_Sound[crrnt_id].flg = D_AUDIO_BUZZER_ON;
				find_flg = D_AUDIO_BUZZER_ON;
				break;
			}
			else {
				crrnt_id++;
				if (crrnt_id >= D_AUDIO_BUZZER_DATA_CNT){
					crrnt_id = 0;
				}
			}
		}
		
		if (find_flg == D_AUDIO_BUZZER_ON){
			*wav_id = crrnt_id + D_AUDIO_BZ_WAV_START_NUM;
			gBF_Audio_WAV_Sound[crrnt_id].addr = (UINT32)gBF_AudioBZ_WAV_Data_Last_Addr;
			gBF_Audio_WAV_Sound[crrnt_id].index = id_index;
			crrnt_id++;
			id_index++;
			ret = D_AUDIO_OK;
		}
		else {
			ret = D_AUDIO_ERR;
		}
	}
	
	gBF_AudioBZ_WAV_ID_Current = crrnt_id;
	gBF_AudioBZ_WAV_ID_Index = id_index;
	
	bf_audio_bz_sig_sem();
	
	return ret;
}

/**
 *  @brief		Clear Wav ID
 *	@param		INT32	wav_id
 *	@return		INT32	OK(0) /Error(1)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_buzzer_clear_wav_id( UINT32 wav_id )
{
	UINT32 l_wav_id;
	UINT32 copy_loop_cnt;
	INT32 ret = D_AUDIO_OK;
	UINT32 data_size;
	UINT8 id_index;
	
	if (wav_id == 0){
		return D_AUDIO_ERR;
	}
	
	bf_audio_bz_wai_sem();
	
	id_index = gBF_AudioBZ_WAV_ID_Index;
	
	// Acquire a table number to delete
	l_wav_id = wav_id - D_AUDIO_BZ_WAV_START_NUM;
	
	data_size = gBF_Audio_WAV_Sound[l_wav_id].data_size;
	
	// A flag judgment during the use
	if (gBF_Audio_WAV_Sound[l_wav_id].flg == D_AUDIO_BUZZER_OFF){
		ret = D_AUDIO_ERR;
	}
	else {
		// Set last address and remaing size
		gBF_AudioBZ_WAV_Data_Last_Addr		-= data_size;
		gBF_AudioBZ_WAV_Data_Remain_Size	+= data_size;
	
		if (id_index != 0){
			id_index--;
		}
		
		// Set copy loop count
		copy_loop_cnt = id_index - gBF_Audio_WAV_Sound[l_wav_id].index;
		
		// Clear table
		gBF_Audio_WAV_Sound[l_wav_id].flg = D_AUDIO_BUZZER_OFF;
		gBF_Audio_WAV_Sound[l_wav_id].index = 0;
		
		if ((gBF_Audio_WAV_Sound[l_wav_id].smpl_rate != 0) && (data_size != 0)){
			bf_audio_buzzer_sort_pcm_data(copy_loop_cnt, l_wav_id);
		}
		
		gBF_Audio_WAV_Sound[l_wav_id].smpl_rate = 0;
		gBF_Audio_WAV_Sound[l_wav_id].addr		= 0;
		gBF_Audio_WAV_Sound[l_wav_id].data_size	= 0;
		gBF_Audio_WAV_Sound[l_wav_id].channel	= 0;
		
		gBF_AudioBZ_WAV_ID_Index = id_index;
	}
	
	bf_audio_bz_sig_sem();
	
	return ret;
}

/**
 *  @brief		Sort PCM Data
 *	@param		UINT32	Count of Copy
 *	@param		UINT8	Deleted WavID Index
 *	@return		INT32	OK(0) /Error(1)
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_buzzer_sort_pcm_data( UINT32 copy_loop_cnt, UINT8 dlt_index )
{
	UINT32 loop_cnt;
	UINT8 find_flg = D_AUDIO_BUZZER_OFF;
	UINT8 src_index = 0;
	UINT32 src_addr;
	UINT32 src_size;
	UINT32 dst_addr;
	UINT32 copy_src_addr;
	UINT32 copy_cnt = 0;
	UINT32 data_addr;
	UINT32 data_size;
	
	// Copy process
	if (copy_loop_cnt != 0){
		// Set Src_copy_addr
		copy_src_addr = gBF_Audio_WAV_Sound[dlt_index].addr + gBF_Audio_WAV_Sound[dlt_index].data_size;
		
		// Because I deleted data, I move data behind.
		for (; copy_loop_cnt > 0; copy_loop_cnt--){
			
			// Search Src_index
			for (loop_cnt = 0; loop_cnt < D_AUDIO_BUZZER_DATA_CNT; loop_cnt++){
				if (gBF_Audio_WAV_Sound[loop_cnt].addr == copy_src_addr){
					src_index = loop_cnt;
					find_flg = D_AUDIO_BUZZER_ON;
					break;
				}
			}
			
			if (find_flg == D_AUDIO_BUZZER_ON){
				find_flg = D_AUDIO_BUZZER_OFF;
			}
			else {
				BF_Debug_Print_Information(("[AUDIO_BZ]NOT FIND Address : copy_loop_cnt=%d, copy_src_addr=0x%x\n", copy_loop_cnt, copy_src_addr));
			}
			
			// Set Src_addr and Dst_addr
			src_addr = gBF_Audio_WAV_Sound[src_index].addr;
			src_size = gBF_Audio_WAV_Sound[src_index].data_size;
			data_addr = gBF_Audio_WAV_Sound[dlt_index].addr;
			data_size = gBF_Audio_WAV_Sound[dlt_index].data_size;
			
			if (copy_cnt == 0){
				dst_addr = data_addr;
			}
			else {
				dst_addr = data_addr + data_size;
			}
			
			memcpy((VOID*)dst_addr, (VOID*)src_addr, src_size);
			
			BF_Audio_Data_Clean_Flush_Dcache((VOID*)dst_addr, src_size);
			
			// Set Next Src_copy_addr
			copy_src_addr = src_addr + src_size;
			
			// Set address
			if (copy_cnt == 0){
				gBF_Audio_WAV_Sound[src_index].addr = data_addr;
			}
			else {
				gBF_Audio_WAV_Sound[src_index].addr = data_addr + data_size;
			}
			
			// Set index
			gBF_Audio_WAV_Sound[src_index].index--;
			
			// Set Next Dst_index
			dlt_index = src_index;
			
			// Judge it whether the loop number of times is the first time
			copy_cnt++;
		}
	}
}


/**
 *  @brief		from Wav file to PCM
 *	@param		INT32	wav_id
 *	@param		UINT32	Address of Wav file data
 *	@return		INT32	OK(0) /Error(1)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_buzzer_wav2pcm( UINT32 wav_id, UINT32 wav_file_addr )
{
	INT32 ret;
	INT32 return_value = D_AUDIO_OK;
	UINT32 check_size;
	mwwav_wav_info	wav_info;
	
	bf_audio_bz_wai_sem();
	
	if (wav_file_addr == 0){
		BF_Debug_Print_Error(("[AUDIO_BZ]Wav Table ID Get ERR Line=%d\n", __LINE__));
		return_value =  D_AUDIO_ERR;
	}
	else {
		// Header Analysis
		ret = mwwav_wav_header_analysis(&wav_info);
		if (ret != MWWAV_OK) {
			BF_Debug_Print_Error(("[AUDIO_BZ]WAV Header ERR : ret=%d, Line=%d\n", ret, __LINE__));
			return_value = D_AUDIO_WAV_HEADER_ERR;
		}
		else {
			// Size check
			if (wav_info.data_size == 0) {
				BF_Debug_Print_Error(("[AUDIO_BZ]WAV Header SIZE ERR : Line=%d\n", __LINE__));
				return_value = D_AUDIO_WAV_TIME_ERR;
			}
			else {
				return_value = bf_audio_buzzer_get_pcm_data_size(&check_size, &wav_info);
				if (return_value == D_AUDIO_OK){
					if (gBF_AudioBZ_WAV_Data_Remain_Size < check_size) {
						gBF_AudioBZ_WAV_ID_Current--;
						gBF_AudioBZ_WAV_ID_Index--;
						gBF_Audio_WAV_Sound[gBF_AudioBZ_WAV_ID_Current].flg		= D_AUDIO_BUZZER_OFF;
						gBF_Audio_WAV_Sound[gBF_AudioBZ_WAV_ID_Current].addr	= 0;
						gBF_Audio_WAV_Sound[gBF_AudioBZ_WAV_ID_Current].index	= 0;
						
						BF_Debug_Print_Error(("[AUDIO_BZ]Remain size=%d < Load Size=%d, Line=%d\n",
												gBF_AudioBZ_WAV_Data_Remain_Size, check_size, __LINE__));
						
						return_value = D_AUDIO_ERR;
					}
					else {
						return_value = bf_audio_buzzer_read_pcm_data(wav_id, &wav_info);
					}
				}
			}
		}
	}
	
	bf_audio_bz_sig_sem();
	
	return return_value;
}

/**
 *  @brief		Get PCM data size from Wav file infomation
 *	@param		ULONG* check_size
 *	@param		mwwav_wav_info wav_info
 *	@return		INT32	OK(0) /Error(D_AUDIO_WAV_FORMAT_OVER)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_buzzer_get_pcm_data_size( UINT32* check_size, const mwwav_wav_info* wav_info )
{
	INT32 ret = D_AUDIO_OK;
	
	// Size check
	if (wav_info->wFormatTag == PCM_TYPE) {
		switch (wav_info->wBitsPerSample){
			case D_AUDIO_PCM_16_BIT:
				if (wav_info->data_size % (D_AUDIO_BZ_NUM_SAMPLE * 2 * wav_info->channel) != 0){
					*check_size = wav_info->data_size
								  + ((D_AUDIO_BZ_NUM_SAMPLE * 2 * wav_info->channel)
								  - (wav_info->data_size % (D_AUDIO_BZ_NUM_SAMPLE * 2 * wav_info->channel)));
				}
				else {
					*check_size = wav_info->data_size;
				}
				
				break;
			case D_AUDIO_PCM_8_BIT:
				if (wav_info->data_size % (D_AUDIO_BZ_NUM_SAMPLE * wav_info->channel) != 0){
					*check_size = (wav_info->data_size * 2)
								  + ((D_AUDIO_BZ_NUM_SAMPLE * 2 * wav_info->channel)
								  - (wav_info->data_size % (D_AUDIO_BZ_NUM_SAMPLE * 2 * wav_info->channel)));;
				}
				else {
					*check_size = wav_info->data_size * 2;
				}
				
				break;
			case D_AUDIO_PCM_24_BIT:
				if (wav_info->data_size % (D_AUDIO_BZ_NUM_SAMPLE * 3 * wav_info->channel) != 0){
					*check_size = wav_info->data_size
								  + ((D_AUDIO_BZ_NUM_SAMPLE * 3 * wav_info->channel)
								  - (wav_info->data_size % (D_AUDIO_BZ_NUM_SAMPLE * 3 * wav_info->channel)));
				}
				else {
					*check_size = wav_info->data_size;
				}
				
				break;
			default:
				*check_size = wav_info->data_size;
				break;
		}
	}
	else if ((wav_info->wFormatTag == u_Law_TYPE)	||
			 (wav_info->wFormatTag == a_Law_TYPE)	){
		
		if (wav_info->data_size % (D_AUDIO_BZ_NUM_SAMPLE * wav_info->channel) != 0){
			*check_size = (wav_info->data_size * 2)
						  + ((D_AUDIO_BZ_NUM_SAMPLE * 2 * wav_info->channel)
						  - (wav_info->data_size % (D_AUDIO_BZ_NUM_SAMPLE * 2 * wav_info->channel)));;
		}
		else {
			*check_size = wav_info->data_size * 2;
		}
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_BZ]WAV Codec ERR : format=0x%x, Line=%d\n", wav_info->wFormatTag, __LINE__));
		ret = D_AUDIO_WAV_FORMAT_OVER;
	}
	
	return ret;
}

/**
 *  @brief		Read PCM data from Wav file
 *	@param		INT32 wav_id
 *	@param		mwwav_wav_info wav_info
 *	@return		INT32 return_value
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_buzzer_read_pcm_data( UINT32 wav_id, const mwwav_wav_info* wav_info )
{
	INT32 return_value = D_AUDIO_OK;
	UINT32 sample_cnt;
	UINT32 total_size = 0;
	UINT32 pcm_addr;
	UINT32 add_data_size;
	UINT32 data_addr;
	UINT32 data_size;
	
	// Search Data Chunk
	data_addr = bf_audio_buzzer_search_data_chunk_wav();
	data_size = wav_info->data_size;
	
	pcm_addr = (UINT32)gBF_Audio_WAV_Sound[wav_id - 1].addr;
	
	if (wav_info->wFormatTag == PCM_TYPE){
		
		if (wav_info->wBitsPerSample == D_AUDIO_PCM_8_BIT){
			if (wav_info->channel == FJ_AUDIO_NUM_CHANNEL_2){
				sample_cnt = data_size / 2;
			}
			else {
				sample_cnt = data_size;
			}
			
			BF_Audio_Data_Pcm_8to16((INT8*)data_addr, (INT16*)pcm_addr, sample_cnt, wav_info->channel);
		}
		else {
			memcpy((VOID*)pcm_addr, (VOID*)data_addr, data_size);
		}
	}
	else if (wav_info->wFormatTag == u_Law_TYPE){
		if (wav_info->channel == FJ_AUDIO_NUM_CHANNEL_2){
			sample_cnt = data_size / 2;
		}
		else {
			sample_cnt = data_size;
		}
		
		ULAW_Dec((SHORT*)pcm_addr, (UCHAR*)data_addr, sample_cnt, wav_info->channel);
	}
	else if (wav_info->wFormatTag == a_Law_TYPE){
		if (wav_info->channel == FJ_AUDIO_NUM_CHANNEL_2){
			sample_cnt = data_size / 2;
		}
		else {
			sample_cnt = data_size;
		}
		
		ALAW_Dec((SHORT*)pcm_addr, (UCHAR*)data_addr, sample_cnt, wav_info->channel);
	}
	else {
		return_value = D_AUDIO_ERR;
	}
	
	if (data_size % (D_AUDIO_BZ_NUM_SAMPLE * 2 * wav_info->channel) != 0){
		add_data_size = (D_AUDIO_BZ_NUM_SAMPLE * 2 * wav_info->channel)
						- (data_size % (D_AUDIO_BZ_NUM_SAMPLE * 2 * wav_info->channel));
		
		pcm_addr += data_size;
		memset((VOID*)pcm_addr, 0, add_data_size);
		total_size = data_size + add_data_size;
	}
	else {
		total_size = data_size;
	}
	
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)gBF_Audio_WAV_Sound[wav_id-1].addr, total_size);
	
	// Data Setting
	gBF_Audio_WAV_Sound[wav_id - 1].smpl_rate		= wav_info->sps;
	gBF_Audio_WAV_Sound[wav_id - 1].data_size		= total_size;
	gBF_Audio_WAV_Sound[wav_id - 1].channel			= wav_info->channel;
	gBF_Audio_WAV_Sound[wav_id - 1].bits_per_sample	= wav_info->wBitsPerSample;
	gBF_AudioBZ_WAV_Data_Remain_Size				-= total_size;
	gBF_AudioBZ_WAV_Data_Last_Addr					= gBF_Audio_WAV_Sound[wav_id-1].addr + total_size;
	
	return return_value;
}

/**
 *  @brief		Search data chunk
 *	@param		None
 *	@return		UINT32 data address
 *	@note		None
 *	@attention	None
 */
static UINT32 bf_audio_buzzer_search_data_chunk_wav( VOID )
{
	UINT32 tag_name;
	INT32 result;
	UINT32* addr;
	UINT32 chunk_size;
	
	addr = (UINT32*)gBF_Audio_WavFile_Addr_Base;
	addr = (UINT32*)((UINT32)addr + 16);			// move fmt chunk size;
	chunk_size = (UINT32)*addr;
	addr = (UINT32*)((UINT32)addr + chunk_size + 4);	// move next chunk;
	
	// Search Data Chunk
	while (1) {
		tag_name = (UINT32)*addr;
		
		result = memcmp((const void*)&tag_name, "data", 4);
		if (result != 0) {
			addr = (UINT32*)((UINT32)addr + 4);
			chunk_size = (UINT32)*addr;
			addr = (UINT32*)((UINT32)addr + chunk_size + 4);	// move next chunk;
		}
		else {
			addr = (UINT32*)((UINT32)addr + 8);
			break;
		}
	}
	
	return (UINT32)addr;
}


/**
 *  @brief		Get Wav Information(address and size)
 *	@param		UINT32		Wav ID
 *	@param		UINT32		Address
 *	@param		UINT32		Size
 *	@return		INT32		OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_buzzer_get_wav_id_info( UINT32 wav_id, UINT32* addr, UINT32* size )
{
	INT32 ret = D_AUDIO_OK;
	
	if (wav_id == 0){
		BF_Debug_Print_Error(("[AUDIO_BZ]BF_AudioBuzzer_DirectPlayWav() invalid wav_id L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		*addr = gBF_Audio_WAV_Sound[wav_id - 1].addr;
		
		if (*addr == 0){
			BF_Debug_Print_Error(("[AUDIO_BZ]BF_AudioBuzzer_DirectPlayWav() invalid wav_id L=%d\n", __LINE__));
			ret = D_AUDIO_ERR;
		}
		else {
			*size = D_AUDIO_BZ_NUM_SAMPLE * 2 * gBF_Audio_WAV_Sound[wav_id - 1].channel;
			if (*size == 0){
				BF_Debug_Print_Error(("[AUDIO_BZ]BF_AudioBuzzer_DirectPlayWav() invalid wav_id L=%d\n", __LINE__));
				ret = D_AUDIO_ERR;
			}
		}
	}
	
	return ret;
}

/**
 *  @brief		Load Wav Data Address Table
 *	@param		UINT8		Audio Out Channel
 *	@param		UINT32		Wav Index
 *	@param		UINT32		Address
 *	@param		UINT32		Size
 *	@return		INT32		OK/ERROR
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_buzzer_load_wav_addr_tbl( UINT8 audio_out_ch, UINT32 wav_index, UINT32 addr, UINT32 size )
{
	INT32 ret = D_AUDIO_OK;
	UINT32 loop;
	UINT32 total_size;
	
	gBF_AudioBZ_WAV_Data_Addr_Tbl_Pos[audio_out_ch] = 0;
	
	total_size = 0;
	
	for (loop = 0; loop < (D_AUDIO_BZ_NUM_FRAME_ADDR_TBL - 1); loop++){
		gBF_AudioBZ_WAV_Data_Addr_Tbl[audio_out_ch][loop].addr = addr;
		gBF_AudioBZ_WAV_Data_Addr_Tbl[audio_out_ch][loop].size = size;
		addr += size;
		total_size += size;
		
		if ( total_size >= gBF_Audio_WAV_Sound[wav_index].data_size){
			gBF_AudioBZ_WAV_Data_Addr_Tbl[audio_out_ch][loop + 1].addr = (UINT32)BF_Audio_Data_Get_NullData_Addr(); // Set Null Data after Last frame.
			gBF_AudioBZ_WAV_Data_Addr_Tbl[audio_out_ch][loop + 1].size = size;
			gBF_AudioBZ_WAV_Data_Addr_Tbl_Pos_End[audio_out_ch] = loop;
			break;
		}
	}
	
	return ret;
}

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/

/**
 *  @brief		Load PCM data from WAV file data
 *	@param		UINT32	address of stored wav file
 *	@param		UINT32	size of stored wav file
 *	@param		UINT32	wav id
 *	@return		INT32	Result(OK/ERROR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioBuzzer_DirectLoadWav( UINT32 addr, UINT32 size, UINT32* wav_id )
{
	if (wav_id == NULL){
		BF_Debug_Print_Error(("[AUDIO_BZ]BF_AudioBuzzer_DirectLoadWav() param err. wav_id = NULL L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (size == 0){
		BF_Debug_Print_Error(("[AUDIO_BZ]BF_AudioBuzzer_DirectLoadWav() param err. size = 0 L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (addr == 0){
		BF_Debug_Print_Error(("[AUDIO_BZ]BF_AudioBuzzer_DirectLoadWav() param err. addr = 0 L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	BF_Audio_Data_Clean_Flush_Dcache((VOID*)addr, size);
	
	*wav_id = bf_audio_buzzer_direct_load_wav(addr);
	
	if (*wav_id != 0){
		return D_AUDIO_OK;
	}
	else {
		return D_AUDIO_ERR;
	}
}

/**
 *  @brief		Unload PCM data
 *	@param		UINT32	wav id
 *	@return		INT32	Result(OK/ERROR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioBuzzer_DirectUnloadWav( UINT32 wav_id )
{
	return bf_audio_buzzer_clear_wav_id(wav_id);
}

/**
 *  @brief		Play loaded PCM data
 *	@param		UCHAR	audio out id
 *	@param		UINT32	wav id
 *	@return		INT32	Result(OK/ERROR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioBuzzer_DirectPlayWav( UCHAR audio_out_id, UINT32 wav_id )
{
	UINT32 addr;
	UINT32 size;
	UINT8 audio_out_status;
	INT32 ret;
	T_FJ_AUDIO_OUT_CFG out_cfg;
	UINT8 audio_out_ch;
	UINT32 wav_index;
	
	ret = bf_audio_buzzer_get_wav_id_info(wav_id, &addr, &size);
	if (ret == D_AUDIO_OK){
		
		ret = BF_AudioOut_Get_OutCh(audio_out_id, &audio_out_ch);
		if (ret == D_AUDIO_OK){
			wav_index = wav_id - 1;
			
			(VOID)bf_audio_buzzer_load_wav_addr_tbl(audio_out_ch, wav_index, addr, size);
			
			gBF_AudioBZ_WAV_Data_BitsPerSample[audio_out_ch] = gBF_Audio_WAV_Sound[wav_index].bits_per_sample;
			gBF_AudioBZ_WAV_Data_Channel[audio_out_ch] = gBF_Audio_WAV_Sound[wav_index].channel;
			
			audio_out_status = BF_AudioOut_Get_Status(audio_out_id);
			if (audio_out_status != D_AUDIO_OUT_STATUS_START){
				if (BF_AudioOut_Get_ConfigSetFlag(audio_out_id) == D_AUDIO_OFF){
					out_cfg.audio_out_port			= BF_AudioOut_Get_Config_I2sCh(audio_out_id);
					out_cfg.i2s_cfg.master_slave	= FJ_AUDIO_IF_MASTER;
					out_cfg.i2s_cfg.lr_clock		= gBF_Audio_WAV_Sound[wav_index].smpl_rate;
					out_cfg.i2s_cfg.data_cycle		= D_AUDIO_I2S_CYCLE_64FS;
					out_cfg.i2s_cfg.i2s_format		= E_FJ_AUDIO_I2S_Data_FORMAT_LEFT;
					out_cfg.i2s_cfg.bit_count		= D_AUDIO_PCM_16_BIT;
					
					ret = BF_AudioOut_Config(audio_out_id, &out_cfg);
					if (ret != D_AUDIO_OK){
						BF_Debug_Print_Error(("[AUDIO_BZ]BF_AudioOut_Config(). L=%d\n", __LINE__));
						ret = D_AUDIO_ERR;
					}
					else {
						ret = BF_AudioOut_SetChannelType(audio_out_id, gBF_Audio_WAV_Sound[wav_index].channel);
						if (ret != D_AUDIO_OK){
							BF_Debug_Print_Error(("[AUDIO_BZ]BF_AudioOut_SetChannelType Error L=%d\n", __LINE__));
						}
						
						ret = BF_AudioOut_SetSmplRate(audio_out_id, gBF_Audio_WAV_Sound[wav_index].smpl_rate);
						if (ret != D_AUDIO_OK){
							BF_Debug_Print_Error(("[AUDIO_BZ]BF_AudioOut_SetChannelType Error L=%d\n", __LINE__));
						}
					}
				}
			}
			
			ret = BF_AudioOut_Start_Buzzer(audio_out_id);
			if (ret != D_AUDIO_OK){
				BF_Debug_Print_Error(("[AUDIO_BZ]BF_AudioOut_Start_Buzzer Error L=%d\n", __LINE__));
				ret = D_AUDIO_ERR;
			}
		}
	}
	
	return ret;
}

/**
 *  @brief		Stop Playback
 *	@param		UCHAR	audio out id
 *	@param		UINT32	wav id
 *	@return		INT32	Result(OK/ERROR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioBuzzer_DirectStopWav( UCHAR audio_out_id, UINT32 wav_id )
{
	return BF_AudioOut_Stop_Buzzer(audio_out_id);
}

/**
 *  @brief		Control Playback(Mix Setting Buzzer and Decoder)
 *	@param		UCHAR	audio out id
 *	@param		UINT32	wav id
 *	@return		INT32	Result(OK/ERROR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioBuzzer_DirectControlWav( UCHAR audio_out_id, E_FJ_AUDIO_WAV_CTL control )
{
	return BF_AudioOut_Set_BuzzerMode(audio_out_id, control);
}

/**
 *  @brief		Renovation position of wav data address table
 *	@param		None
 *	@return		INT32	Result(OK/ERROR)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioBuzzer_Renov_Buff( UINT8 audio_out_ch )
{
	INT32 ret = D_AUDIO_OK;
	
	Dd_ARM_Critical_Section_Start( gBF_AudioBZ_Tbl_Pos );
	
	if (gBF_AudioBZ_WAV_Data_Addr_Tbl_Pos[audio_out_ch] >= gBF_AudioBZ_WAV_Data_Addr_Tbl_Pos_End[audio_out_ch]){
		ret = D_AUDIO_BZ_PCM_END;
	}
	else {
		gBF_AudioBZ_WAV_Data_Addr_Tbl_Pos[audio_out_ch]++;
	}
	
	Dd_ARM_Critical_Section_End( gBF_AudioBZ_Tbl_Pos );
	
	return ret;
}

/**
 *  @brief		Get address of pcm data
 *	@param		None
 *	@return		UINT32	address
 *	@note		None
 *	@attention	None
 */
UINT32 BF_AudioBuzzer_Get_BuffAddr( UINT8 audio_out_ch )
{
	return gBF_AudioBZ_WAV_Data_Addr_Tbl[audio_out_ch][gBF_AudioBZ_WAV_Data_Addr_Tbl_Pos[audio_out_ch]].addr;
}

/**
 *  @brief		Get size of pcm data
 *	@param		None
 *	@return		UINT32	size
 *	@note		None
 *	@attention	None
 */
UINT32 BF_AudioBuzzer_Get_BuffSize( UINT8 audio_out_ch )
{
	return gBF_AudioBZ_WAV_Data_Addr_Tbl[audio_out_ch][gBF_AudioBZ_WAV_Data_Addr_Tbl_Pos[audio_out_ch]].size;
}

/**
 *  @brief		Get bits/sample of pcm data
 *	@param		None
 *	@return		UINT8	bits/sample information
 *	@note		None
 *	@attention	None
 */
UINT8 BF_AudioBuzzer_Get_Buff_BitsPerSample( UINT8 audio_out_ch )
{
	return gBF_AudioBZ_WAV_Data_BitsPerSample[audio_out_ch];
}

/**
 *  @brief		Get channel of pcm data
 *	@param		None
 *	@return		UINT8	channel information
 *	@note		None
 *	@attention	None
 */
UINT8 BF_AudioBuzzer_Get_Buff_Channel( UINT8 audio_out_ch )
{
	return gBF_AudioBZ_WAV_Data_Channel[audio_out_ch];
}

/****************************/
/* For WAV Header Lib		*/
/****************************/

/**
 *  @brief		Set Address of Wav File
 *	@param		UINT32	address
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioBuzzer_Set_WavFile_Addr( UINT32 addr )
{
	gBF_Audio_WavFile_Addr_Base = addr;
	gBF_Audio_WavFile_Addr_Current = addr;
}

/**
 *  @brief		Get Address of Wav File
 *	@param		UINT32	address
 *	@return		None
 *	@note		None
 *	@attention	None
 */
UINT32 BF_AudioBuzzer_Get_WavFile_Addr( VOID )
{
	return gBF_Audio_WavFile_Addr_Current;
}

/**
 *  @brief		Clear Address of Wav File
 *	@param		UINT32	address
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioBuzzer_Clear_WavFile_Addr( VOID )
{
	gBF_Audio_WavFile_Addr_Base = 0;
	gBF_Audio_WavFile_Addr_Current = 0;
}

 /*
 * param        *buf            write data
 * param        bytes           write data size
 */

INT32 wav_write( VOID *buf, ULONG bytes )
{
	// Dummy
	
   return 0;
}

/*
 * param        *buf            read data
 * param        bytes           read data size
 */
INT32 wav_read( VOID *buf, ULONG bytes )
{
	memcpy( buf, (VOID*)gBF_Audio_WavFile_Addr_Current, bytes );
	gBF_Audio_WavFile_Addr_Current += bytes;
	
    return 0;
}

/*
 * move a file pointer
 * param        pos				the offset value (byte) from the standard position.
 */
INT32 wav_seek( LONG pos )
{
	gBF_Audio_WavFile_Addr_Current = gBF_Audio_WavFile_Addr_Base + pos;
	
    return 0;
}

/*
 * move a file pointer
 * param        pos				the offset value (byte) from the standard position.
 */
INT32 wav_seek_cur( LONG pos )
{
	gBF_Audio_WavFile_Addr_Current += pos;
	
    return 0;
}
