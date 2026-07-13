/**
 * @file		h265_encode.c
 * @brief		H.265 Encode Process.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h265_encode.h"
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#include "h265_sirius.h"
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#include "vpuapi.h"
#include "vpuapifunc.h"
#include "h265_wave.h"
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
#include "mba_if.h"
#include "share.h"

#ifdef CO_H265_SECTION_ON
//#pragma arm section code="H265_CODE"
//#pragma arm section rodata="H265_CONST"
//#pragma arm section rwdata="H265_INIT"
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_H265_ENCODE_SDRAM_CH_CHECK	(SDRAM_ADR_CH0_TOP | SDRAM_ADR_CH1_TOP)

#define D_H265_ENCODE_LATER_THRESHOLD		(30)

// for Video PES
#define D_H265_ENCODE_PES_HEADER_VIDEO_SIZE	(19)	// PES header size

#define D_H265_ENCODE_ONESHOT_ENC_NUM		(1)


#define H265_PTS_90K_BASE	(90000)

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define D_H265_ENCODE_WAVE_STREAM_GAP_SIZE	(512)
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Record Parameters
static	volatile	T_H265_MOVIE_UI					gH265_Rec_Param;

// Encode Control Data
static	volatile	T_H265_ENC_CTL					gH265_Enc_Ctrl = {0};
static	volatile	UINT32							gH265_Sync_Intterupt_Count[E_H265_IF_STREAM_CH_MAX];
static	volatile	ULONG							gH265_Stream_Stop_FlgID[E_H265_IF_STREAM_CH_MAX] = {FLG_H265_SYNC_STOP_ENC_CH0, FLG_H265_SYNC_STOP_ENC_CH1};
static	volatile	ULONG							gH265_Stream_TimeLapse_Sync_FlgID[E_H265_IF_STREAM_CH_MAX] = {FLG_H265_SYNC_ENC_CH0, FLG_H265_SYNC_ENC_CH1};
static	volatile	T_H265_ENC_VIN_ADDRESS_CONTROL	gH265_Vin_Adr_Ctl[E_H265_IF_STREAM_CH_MAX];
static	volatile	INT32							gH265_Vin_Adr_Emp_Over_Flg[E_H265_IF_STREAM_CH_MAX];

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
static	volatile	UINT32							gH265_Fm_Table_Use_Flag[E_H265_IF_STREAM_CH_MAX][ENC_FM_NUM];
static	volatile	OS_USER_FLGPTN					gH265_Sirius_Stop_Done_Wiat_Ptn;
static	volatile	ULONG							gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_MAX] = {0, 0};
static	volatile	T_SR_ENC_VINFO					gH265_Frame_Info[E_H265_IF_STREAM_CH_MAX][D_H265_ENCODE_VINFO_FIFO_MAX];
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
static	EncHandle									gH265_Codec_Handle[E_H265_IF_STREAM_CH_MAX];
static	volatile	EncOutputInfo					gH265_Codec_Output_Info[E_H265_IF_STREAM_CH_MAX][D_H265_ENCODE_VINFO_FIFO_MAX];
static	volatile	UINT32							gH265_Codec_Stream_Wpoint_Notify[E_H265_IF_STREAM_CH_MAX][D_H265_ENCODE_VINFO_FIFO_MAX];
static	volatile	UINT32							gH265_Stream_Buffer_ReadPtr[2];
static	volatile	UINT32							gH265_Stream_Buffer_WritePtr[2];
static	volatile	T_H265_ENC_NAL_SIZE				gH265_Codec_Output_NAL_Info[E_H265_IF_STREAM_CH_MAX][D_H265_ENCODE_VINFO_FIFO_MAX];
static	volatile	UINT32							gH265_Codec_Output_NAL_Info_Sei_Pointer[E_H265_IF_STREAM_CH_MAX];
static	volatile	INT32							gH265_Codec_End_Flg[E_H265_IF_STREAM_CH_MAX] = {0, 0};
static	volatile	FrameBuffer						gH265_YC_Buffer[E_H265_IF_STREAM_CH_MAX][D_WAVE_YC_BANK_MAX];
static	volatile	T_H265_ENC_HEADER_INFO			gH265_Codec_Header_Info[E_H265_IF_STREAM_CH_MAX];
static	volatile	DDRChannelParam					gH265_Codec_Ddr_Ch_Select;


static	volatile	UINT32							gH265_Codec_Enc_Core_Idx[E_H265_IF_STREAM_CH_MAX];
static	volatile	BOOL							gH265_Codec_Ready_To_Pallalel[E_H265_IF_STREAM_CH_MAX];



static	volatile	T_H265_WAVE_QUEUE*				gH265_Codec_Queue_Ctrl[MAX_NUM_INSTANCE] = {NULL, };
static	volatile	hrd_t							gH265_Codec_HDR_Info[E_H265_IF_STREAM_CH_MAX];
static	volatile	vui_t							gH265_Codec_VUI_Info[E_H265_IF_STREAM_CH_MAX];
static	volatile	BOOL							gH265_Codec_Already_Stop_Req_Flag[E_H265_IF_STREAM_CH_MAX];


static	volatile	T_H265_WAVE_SEI_PIC_TIMING			gH265_Codec_Pic_Timing_Sei[E_H265_IF_STREAM_CH_MAX];
static	volatile	T_H265_WAVE_SEI_ACTIVE_PARAMETER	gH265_Codec_Active_Parameter_Sei[E_H265_IF_STREAM_CH_MAX];
static	volatile	T_H265_WAVE_SEI_BUFFERING_PERIOD	gH265_Codec_Buffering_Period_Sei[E_H265_IF_STREAM_CH_MAX];


// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
static	volatile	UINT32							gH265_Enc_Serial_Num[E_H265_IF_STREAM_CH_MAX] = {0, 0};
static	volatile	UINT32							gH265_Map_Enable_Flg[E_H265_IF_STREAM_CH_MAX] = {0, 0};
static	volatile	UINT32							gH265_Map_First_Flg[E_H265_IF_STREAM_CH_MAX] = {1, 1};
static	volatile	UINT32							gH265_Vstrem_Serial_Num[E_H265_IF_STREAM_CH_MAX] = {0, 0};
static	volatile	UINT32							gH265_ViewLater_Count = 0;
static	volatile	UINT32							gH265_MovieLater_Count = 0;
static	volatile	UINT32							gH265_ViewLater_Total_Count = 0;
static	volatile	UINT32							gH265_MovieLater_Total_Count = 0;

static	volatile	T_H265_IF_DYNAMIC_PARAM_RECORD	gH265_Dynamic_Param[E_H265_IF_STREAM_CH_MAX];
static	volatile	UINT32							gH265_Dynamic_Change_Flag[E_H265_IF_STREAM_CH_MAX];

static	volatile	UINT32							gH265_Insert_IDR_Flag[E_H265_IF_STREAM_CH_MAX];

static	volatile	UINT32							gH265_GOP_Change_Flag[E_H265_IF_STREAM_CH_MAX];

static	volatile	T_H265_ENC_VSTRM_FIFO_CTRL		gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_MAX];

static	volatile	T_H265_ENC_SLICE_INFO			gH265_Slice_Info[E_H265_IF_STREAM_CH_MAX];
static	volatile	UINT32							gH265_VframeComp_Set_Flag[E_H265_IF_STREAM_CH_MAX];

// Free Stream
static	volatile	UINT32							gH265_Free_Vstream_Index[D_H265_ENC_STREAM_FIFO_COUNT];
static	volatile	UINT32							gH265_Free_Vstream_End_Flg[E_H265_IF_STREAM_CH_MAX] = { H265_ON, H265_ON };	/**< Free Video Stream End Flag */
static	volatile	UINT32							gH265_Free_Vstream_Count = 0;										/**< Free Video Stream Count */
static	volatile	T_H265_ENC_STREAM_FREE_CONTROL	gH265_Free_Vstream_Ctrl[D_H265_ENC_STREAM_FIFO_COUNT];

static	volatile	ULLONG							gH265_Image_Get_Count[E_H265_IF_STREAM_CH_MAX];
static	volatile	ULLONG							gH265_Image_Store_Count[E_H265_IF_STREAM_CH_MAX];

ULLONG	gH265_First_FrameNo = 0;
ULLONG	gH265_Last_FrameNo = 0;

UINT32	gH265_Slice_Encode_En = 0;
UINT32	gH265_Slice_per_Frame = 0;
UINT32	gH265_Prev_Frame_Aligned_Address = 0;
UINT32	gH265_Prev_Frame_Size = 0;
UINT32	gH265_Prev_Frame_Padding = 0;	// alignment between frames
UINT32	gH265_Prev_Frame_PicCnt = 0;
UINT32	gH265_Prev_Slice_PicType = 0;
UINT32	gH265_Prev_Slice_WAddr = 0;	// write pointer of slice 0, 1, 2, 3


#ifdef CO_ES3_HARDWARE
static T_H265_IF_VIDEO_STREAM_PARAM g_video_stream_info[E_H265_IF_STREAM_CH_MAX];
#endif
/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static	INT32	h265_encode_task_create( VOID );
static	VOID	h265_encode_task_destroy( VOID );
static	INT32	h265_encode_init_codec( VOID );
static	INT32	h265_encode_set_param_codec( E_H265_IF_STREAM_CH stream_ch );
static	VOID	h265_encode_task_start( UINT32 task_id );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
static	INT32	h265_encode_sirius_enc_param_set(
					E_H265_IF_STREAM_CH		stream_ch,
					T_SR_ENC_PARAM			*enc_param,
					T_SR_ENC_FM_TABLE		*fm_table,
					ULLONG					*frame_no,
					USHORT					vop_cnt,
					USHORT					*gop_cnt
					);
static	INT32	h265_encode_sirius_enc_request( E_H265_IF_STREAM_CH stream_ch, T_SR_ENC_PARAM* enc_param, T_SR_ENC_FM_TABLE *fm_table );
static	INT32	h265_encode_sirius_result_get( E_H265_IF_STREAM_CH stream_ch, T_H265_ENC_VIDEO_INFO* video_info );
static	VOID	h265_encode_stream_around_check( E_H265_IF_STREAM_CH stream_ch, T_H265_ENC_VIDEO_INFO* video_info, T_SR_ENC_VINFO vstrm_info );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
static	INT32	h265_encode_wave_enc_one_frame( E_H265_IF_STREAM_CH stream_ch, INT32 stop_req_flag, INT32 *vop_cnt, INT32 *gop_cnt, INT32* delay_flag, INT32* end_flag );
static	INT32	h265_encode_wave_result_get( E_H265_IF_STREAM_CH stream_ch, T_H265_ENC_VIDEO_INFO* video_info );
static	VOID	h265_encode_wave_stream_around_check( E_H265_IF_STREAM_CH stream_ch, T_H265_ENC_VIDEO_INFO* video_info, EncOutputInfo vstrm_info );
static	VOID	h265_encode_ch_map_reset( E_H265_IF_STREAM_CH stream_ch );

static	VOID	h265_encode_stream_buffer_init( INT32 index, UINT32 addr );
static	INT32	h265_encode_stream_buffer_allocate( INT32 index, UINT32 addr, UINT32 size, BOOL roundup );
static	VOID	h265_encode_stream_buffer_free( INT32 index, UINT32 addr );
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
static	VOID	h265_encode_pes_header_set( ULONG stream_ch, ULONG addr, ULONG size, ULLONG pts, ULLONG dts );
static	VOID	h265_encode_illagal( VOID );
static	VOID	h265_encode_Vin_initialize( VOID );
static	INT32	h265_encode_Vin_get_address( E_H265_IF_STREAM_CH stream_ch, ULLONG frame_no, E_H265_IF_IMAGE_SIZE size, T_H265_IF_GET_IMAGE_ADDR_PARAM* addr_param );
static	VOID	h265_encode_Vin_decide_address( E_H265_IF_STREAM_CH stream_ch, ULLONG frame_no );

static	VOID	h265_encode_Vin_set_decide_bank( E_H265_IF_STREAM_CH stream_ch, INT32 bank, ULLONG frame_no );
static	INT32	h265_encode_Vin_get_decide_bank( E_H265_IF_STREAM_CH stream_ch, INT32* bank, ULLONG* frame_no );
static	VOID	h265_encode_Vin_set_empty_bank( E_H265_IF_STREAM_CH stream_ch, INT32 bank );
static	INT32	h265_encode_Vin_get_empty_bank( E_H265_IF_STREAM_CH stream_ch, INT32* bank );

static	VOID	h265_encode_video_stream_param_set( UINT32 stream_ch, T_H265_ENC_VIDEO_INFO* video_info, T_H265_IF_VIDEO_STREAM_PARAM* video_stream_info, ULLONG* total_duration);
static	VOID	h265_encode_result_notify( INT32 enc_result );
static	VOID	h265_encode_free_vstream( VOID );

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 *  @brief		Movie Encode Ready Status Process.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H265_Encode_Main_Ready( E_H265_IF_STREAM_CH stream_ch, T_H265_MOVIE_UI* record_info )
{
	INT32	ret;
	
	if( record_info == NULL ){
		return H265_ERR_INPUT_PARAM;
	}
	
	// Initialize Control Data(Global Data & SDRAM)
	ret = H265_Encode_Initialize( stream_ch, record_info );
	if( ret != H265_OK ){
		if( gH265_Enc_Ctrl.rec_status != H265_ENC_RECORDING ){
			H265_Encode_Finalize();
		}
		return ret;
	}
	if( gH265_Enc_Ctrl.rec_status != H265_ENC_RECORDING ){
		// Initialize Ribery Macro
		// Ribery F/W DL, Start Encoder
		ret = h265_encode_init_codec();
		if( ret != H265_OK ){
			// Free MP4 Memory, and Close the file
			H265_Encode_Finalize();
			return ret;
		}
	}
	
	// Ribery Parameter Setting
	ret = h265_encode_set_param_codec( stream_ch );
	if( ret != H265_OK ){
		// Free MP4 Memory, and Close the file
		H265_Encode_Finalize();
		return ret;
	}
	
	// Create CC, ENT task
	ret = h265_encode_task_create();
	if( ret != H265_OK ){
		OS_User_Set_Flg( FID_H265, FLG_H265_STOP );
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		H265_Sirius_Finalize();
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		H265_Wave_Finalize();
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
		H265_Encode_Finalize();
		return ret;
	}
	gH265_Enc_Ctrl.rec_status = H265_ENC_RECORDING;
	
	return H265_OK;
}

/**
 *  @brief		Movie Encode Check Result.\n
 *	@param		None
 *	@return		End Result		End Result\n
 *								H265_OK   : Not End
 *								anything : Not End(Continue)
 *	@note		None
 *	@attention	None
 */
INT32 H265_Encode_Main_Check_End( T_H265_MOVIE_UI* record_info )
{
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn;
	OS_USER_ER			ercd;
	
#ifdef CO_H265_ENC_DEBUG_VIEW_DUMMY
		waiptn = FLG_H265_TASK_ENT_STOP | FLG_H265_TASK_CC_STOP | FLG_H265_TASK_DISP_STOP;
#else
		waiptn = FLG_H265_TASK_ENT_STOP | FLG_H265_TASK_CC_STOP;
#endif
	
	ercd = OS_User_Pol_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
	if( ercd == D_OS_USER_E_OK ){
		H265_Info_Print(("--- H265: STOP by Task End. (flg=%lx)\n",flgptn ));
		return gH265_Enc_Ctrl.task_result;
	}
	else if( ercd == D_OS_USER_E_TMOUT ) {
		;
	}
	else{
		H265_Err_Print(("--- pol_flg(6.3) ercd=%x\n", ercd ));
	}
	
	return H265_OK;
}

/**
 *  @brief		Encode Process End.\n
 *	@param		None
 *	@return		End Result		End Result\n
 *								H265_OK  : Not End
 *								H265_OFF : Not End(Continue)
 *	@note		None
 *	@attention	None
 */
VOID H265_Encode_Main_End( E_H265_IF_STREAM_CH stream_ch, T_H265_MOVIE_UI* record_info )
{
	E_VIDEO_ENOCDE_ID video_enc_id;
	
	if( gH265_Rec_Param.codec_param[stream_ch].enable_flg == H265_ON ){
		OS_User_Set_Flg( FID_H265_SYNC, gH265_Stream_Stop_FlgID[stream_ch] );
		
#if 1	// This is decide customer specification. This case is self call FrameEnabled, and last frame is dummy.
		BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H265( stream_ch, &video_enc_id );
		if( BF_Movie_Video_Attr_Get_Timelapse_Flag(video_enc_id) != H265_OFF ){
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u DummySync", stream_ch ));
			H265_Encode_Sync_Interrupt_Count_Increment( stream_ch );
			OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_ENC_MASTER );
			OS_User_Set_Flg( FID_H265_SYNC, gH265_Stream_TimeLapse_Sync_FlgID[stream_ch] );
		}
#endif
	}
	else{
		// none
	}
	
	return;
}

/**
 *  @brief		Encode CC Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Encode_CC_Main( VOID )
{
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn;
	OS_USER_ER			ercd;
	INT32				ret;
	ULLONG				frame_no = 0;
	INT32				vop_cnt[E_H265_IF_STREAM_CH_MAX] = {0, 0};
	USHORT				gop_cnt[E_H265_IF_STREAM_CH_MAX] = {1, 1};
	UCHAR				first_flag[E_H265_IF_STREAM_CH_MAX] = {0, 0};
	UINT32				waitcnt[E_H265_IF_STREAM_CH_MAX] = {0, 0};
	UCHAR				stop_req_flag[E_H265_IF_STREAM_CH_MAX] = {0, 0};
	E_H265_IF_STREAM_CH	stream_ch;
	UINT32				free_loop;
	UCHAR				detect_h265_stop = FALSE;
	T_H265_IF_MSG		msg;
	OS_USER_FLGPTN		frame_done_wait_ptn = 0;
	UINT32				di_status;

	T_SR_ENC_PARAM		enc_param;
	T_SR_ENC_FM_TABLE	fm_table;
	
	// Initilize control data
	H265_Movie_Memset( &enc_param, 0, sizeof(T_SR_ENC_PARAM) );
	H265_Movie_Memset( &fm_table, 0, sizeof(fm_table) );
	
	// Set Event for REC Task Start
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_CC_INIT_END );
	
	//wait start video process.
	waiptn = (FLG_H265_WDT | FLG_H265_FORCE_STOP | FLG_H265_STOP | FLG_H265_START);
	OS_User_Wai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	
	//Process FLG_H265_WDT & FLG_H265_FORCE_STOP  in next "CC Loop Process".
	if( 0 != (flgptn & FLG_H265_STOP) ){
		detect_h265_stop = TRUE;
		OS_User_Set_Flg( FID_H265, FLG_H265_STOP_ENT_TSK );
		H265_Info_Print(("+++ H265_CC: Request Stop\n"));
	}
	
	if (detect_h265_stop == FALSE){
		// CC Loop Process start
		while(1){
			waiptn = (FLG_H265_WDT | FLG_H265_FORCE_STOP);
			// received wdt time out msg from Ribery or Record Froce Stop
			ercd = OS_User_Pol_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			if( ercd == D_OS_USER_E_OK ){
				if( 0 != (flgptn & FLG_H265_WDT) ){
					// Received WDT T.O Event
					H265_Err_Print(("+++ H265_CC: WDT Event Received\n"));
				}
				else{ //0 != (flgptn & FLG_H265_FORCE_STOP)
					// Received Force Stop Event
					H265_Err_Print(("+++ H265_CC: Force Stop\n"));
				}
				
				waiptn = 0;
				if( gH265_Rec_Param.codec_param[E_H265_IF_STREAM_CH_0].enable_flg == H265_ON ){
					waiptn |= FLG_H265_SYNC_STOP_ENC_CH0;
				}
				if( gH265_Rec_Param.codec_param[E_H265_IF_STREAM_CH_1].enable_flg == H265_ON ){
					waiptn |= FLG_H265_SYNC_STOP_ENC_CH1;
				}
				
				OS_User_Wai_Flg( FID_H265_SYNC, waiptn, D_OS_USER_TWF_ANDW, &flgptn ); 
				OS_User_Clr_Flg( FID_H265_SYNC, ~flgptn );
				H265_Info_Print(("--- H265_CC: all stop request\n" ));
				break;
			}
			else if( ercd == D_OS_USER_E_TMOUT ) {
				;
			}
			else{
				H265_Err_Print(("--- H265_CC: pol_flg ercd1=%x\n", ercd ));
			}
			
			waiptn = FLG_H265_SYNC_ENC_MASTER | FLG_H265_SYNC_ENC_CH0 | FLG_H265_SYNC_ENC_CH1;
			
//			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_CC" ));
			
			OS_User_Wai_Flg( FID_H265_SYNC, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			
//			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H265_CC" ));
			
			// Stop Complete Check
			if( (gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_0] == H265_ON) &&
				(gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_1] == H265_ON) ){
				break;
			}
			
			if( 0 != (flgptn & FLG_H265_SYNC_ENC_MASTER) ){
				OS_User_Clr_Flg( FID_H265_SYNC, ~FLG_H265_SYNC_ENC_MASTER );
				for( stream_ch=E_H265_IF_STREAM_CH_0; stream_ch<E_H265_IF_STREAM_CH_MAX; stream_ch++ ){
					if( gH265_Rec_Param.codec_param[stream_ch].enable_flg == H265_ON ){
						if( gH265_Map_Enable_Flg[stream_ch] == H265_ON ){
							if( (gH265_Enc_Serial_Num[stream_ch] == 0) && (first_flag[stream_ch] == H265_OFF) ){
								if( gH265_Vin_Adr_Ctl[stream_ch].hold_max_no < 1 ){
									waitcnt[stream_ch]++;
		//							printf("waitcnt[%d][%d]\n",stream_ch, waitcnt[stream_ch] );
									if( waitcnt[stream_ch] > 1200 ){
										H265_Err_Print(("+++ H265_CC: View Task Image Update Nothing!!(Between 150VD) %d\n", stream_ch ));
										OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
										break;
									}
								}
								else{
									// "total_time" is taken as internal time. (It is from 0 after a stop.) 
									BF_Movie_Common_Sync_Start_H265( stream_ch );
									first_flag[stream_ch] = H265_ON;
								}
							}
						}
					}
				}
				continue;
			}
			else if( 0 != (flgptn & FLG_H265_SYNC_ENC_CH0) ){
				OS_User_Clr_Flg( FID_H265_SYNC, ~FLG_H265_SYNC_ENC_CH0 );
				stream_ch = E_H265_IF_STREAM_CH_0;
				waiptn = FLG_H265_SYNC_STOP_ENC_CH0;
			}
			else if( 0 != (flgptn & FLG_H265_SYNC_ENC_CH1) ){
				OS_User_Clr_Flg( FID_H265_SYNC, ~FLG_H265_SYNC_ENC_CH1 );
				stream_ch = E_H265_IF_STREAM_CH_1;
				waiptn = FLG_H265_SYNC_STOP_ENC_CH1;
			}
			else{
				H265_Err_Print(("+++ H265_CC: Sync Wait: The route which is not assumed.\n" ));
				stream_ch = E_H265_IF_STREAM_CH_0;
			}
			
			// Stop Complete Check
			if( gH265_Sirius_Stop_Flg[stream_ch] == H265_ON ){
				continue;
			}
			
			// Enc Param Free
			ercd = OS_User_Twai_Sem(SID_H265, H265_ENC_SEMAPHORE_TIMEOUT);
			if( ercd != D_OS_USER_E_OK ){
				H265_Err_Print(( "--- H265_CC: Fm semaphore NG\n" ));
				OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
			}
			ret = H265_Sirius_Enc_Fm_Free_Get( stream_ch, &fm_table );
			if( ret != H265_OK ){
				if( gH265_Sirius_Stop_Flg[stream_ch] == H265_ON ){
					// Enc Stop Done
					OS_User_Sig_Sem(SID_H265);
					continue;
				}
				H265_Err_Print(( "--- H265_CC: H265_Sirius_Enc_Fm_Free_Get NG\n" ));
				OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
			}
			for( free_loop=0; free_loop<ENC_FM_NUM; free_loop++ ){
				if( gH265_Fm_Table_Use_Flag[stream_ch][free_loop] == H265_ON ){
					if( fm_table.use_flag[free_loop] == H265_OFF ){
						h265_encode_Vin_set_empty_bank( stream_ch, fm_table.physical_bank_idx[free_loop] );
						
						gH265_Fm_Table_Use_Flag[stream_ch][free_loop] = H265_OFF;
						M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "vfree %u", free_loop ));
						H265_Sirius_Enc_Fm_Empty_Field_Set( stream_ch, free_loop );
					}
				}
				fm_table.use_flag[free_loop] = H265_OFF;
			}
			OS_User_Sig_Sem(SID_H265);
			
			// It updates vop_cnt
			vop_cnt[stream_ch]++;
			if( vop_cnt[stream_ch] > gH265_Rec_Param.codec_param[stream_ch].gop_num ){
				vop_cnt[stream_ch] = 1;
				gop_cnt[stream_ch]++;
				if( gop_cnt[stream_ch] > gH265_Rec_Param.codec_param[stream_ch].gop_num_idr_to_idr ){
					gop_cnt[stream_ch] = 1;
				}
			}
			if( vop_cnt[stream_ch] == 1 ){
				H265_If_Record_Notify_PreEncode( stream_ch, &vop_cnt[stream_ch] );
			}
			
			if( frame_done_wait_ptn != 0 ){
				ercd = OS_User_Twai_Flg( FID_H265, frame_done_wait_ptn, D_OS_USER_TWF_ORW, &flgptn, 1000 );
				if( ercd != D_OS_USER_E_OK ){
					H265_Err_Print(("+++ H265_CC: Not FRAME_DONE interrupt \n"));
					OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
					M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "not FRAME_DONE" ));
					
#if 1	// debug
					for( free_loop=0; free_loop<ENC_FM_NUM; free_loop++ ){
						M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u", gH265_Fm_Table_Use_Flag[stream_ch][free_loop] ));
					}
#endif
				}
				OS_User_Clr_Flg( FID_H265, ~frame_done_wait_ptn );
			}
			
			// Check Stop Request
			if( stop_req_flag[stream_ch] == H265_OFF ){
				ercd = OS_User_Pol_Flg( FID_H265_SYNC, waiptn, D_OS_USER_TWF_ORW, &flgptn );
				if( ercd == D_OS_USER_E_OK ){
						H265_Info_Print(("--- H265_CC: stop_req_flag stream_ch =%d\n", stream_ch));
						// recv STOP new image not store
						M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265_CC_stop" ));
						ret = H265_Sirius_Enc_Stop( stream_ch );
						if( ret != H265_OK ){
							H265_Err_Print(("--- H265_CC: H265_Sirius_Enc_Stop stream_ch =%d\n", stream_ch));
						}
						stop_req_flag[stream_ch] = H265_ON;
				}
				else if( ercd == D_OS_USER_E_TMOUT ){
					;
				}
				else{
					H265_Err_Print(("--- H265_CC: pol_flg ercd2=%x\n", ercd ));
				}
			}
			else{
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "st_wait" ));
				// Already Stop Request Send
				// After Stop, Codec must defend the API order.  Stop Req -> loop[Enc Req -> VEB Set]
				ercd = OS_User_Twai_Flg( FID_H265, FLG_H265_ENC_VEB_INFO_SET, D_OS_USER_TWF_ORW, &flgptn, 1000 );	
				if( ercd != D_OS_USER_E_OK ){
					H265_Err_Print(("+++ H265_CC: Not conp VEB Set \n"));
					continue;
				}
			}
			
			// Stop Complete Check
			if( gH265_Sirius_Stop_Flg[stream_ch] == H265_ON ){
				continue;
			}
			
			// H.265 Encode process
			// Parameter Setting
			ret = h265_encode_sirius_enc_param_set(
						stream_ch,
						&enc_param,
						&fm_table,
						&frame_no,
						 vop_cnt[stream_ch],
						&gop_cnt[stream_ch]
						 );
			if( ret != H265_OK ){
				H265_Err_Print(("+++ h265_encode_sirius_enc_param_set ERR\n"));
				OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
				break;
			}
			
//			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Vreq" ));
			// Encode Request to Ribery Macro.
			ret = h265_encode_sirius_enc_request( stream_ch, &enc_param, &fm_table );
			if( ret != H265_OK ){
				if( gH265_Sirius_Stop_Flg[stream_ch] == H265_ON ){
					// Enc Stop Done
					continue;
				}
				H265_Err_Print(("+++ H265_CC: Encode Parameter Error=%d\n", ret ));
				OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
				break;
			}
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_VEB_INFO_SET );
			
			if( stream_ch == E_H265_IF_STREAM_CH_0 ){
				frame_done_wait_ptn = FLG_H265_ENC_VFRAME_COMP0_TO_CC;
				di_status = H265_Movie_DI();
				gH265_Sync_Intterupt_Count[E_H265_IF_STREAM_CH_0]--;
				H265_Movie_EI(di_status);
			}
			else{
				frame_done_wait_ptn = FLG_H265_ENC_VFRAME_COMP1_TO_CC;
				di_status = H265_Movie_DI();
				gH265_Sync_Intterupt_Count[E_H265_IF_STREAM_CH_1]--;
				H265_Movie_EI(di_status);
			}
			
#ifdef CO_DEBUG_ON_PC
			if( enc_param.slice_num == 4 ){
				int i;
				for(i=0; i<4; i++){
					OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSLICE_COMP0 );
					OS_User_Tslp_Tsk( 1 );
				}
				OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VFRAME_COMP0_TO_ENT );
			}
			else{
				OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSLICE_COMP0 );
				OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VFRAME_COMP0_TO_ENT );
			}
#endif
			
			// Recording Time Update
			gH265_Enc_Ctrl.video[stream_ch].total_time_increment += gH265_Enc_Ctrl.video[stream_ch].time_increment;
			
			if( gH265_Sync_Intterupt_Count[stream_ch] > 1 ){
				if( stream_ch == E_H265_IF_STREAM_CH_0 ){
					OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_ENC_CH0 );
				}
				else{
					OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_ENC_CH1 );
				}
			}
			
//			printf("incr:[%d][%d]\n", stream_ch, gH265_Enc_Ctrl.video[stream_ch].total_time_increment);
			
		}	// CC loop End
	}
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_CC" ));
	
	// for Debug
	if( (gH265_ViewLater_Total_Count != 0) || (gH265_MovieLater_Total_Count != 0) ){
		H265_Warning_Print(("gH265_ViewLater_Total_Count is %d\n", gH265_ViewLater_Total_Count ));
		H265_Warning_Print(("gH265_MovieLater_Total_Count is %d\n", gH265_MovieLater_Total_Count ));
	}
	
//	H265_Info_Print(("+++ H265_CC: CC Task End Complete!!\n"));
	
	// Set CC Task Stop Event
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_CC_STOP );
	
	msg.type = E_H265_IF_TYPE_NO_REQUEST;
	ret = H265_If_Set_Msg( &msg, E_H265_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		H265_Err_Print(("--- H265_CC End Msg ERR. ret=%x\n", ret ));
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn;
	OS_USER_ER			ercd;
	INT32				ret;
	INT32				vop_cnt					= 0;
	INT32				gop_cnt					= 1;
	UCHAR				first_gop_flg			= H265_ON;	// use only IBBP
	UCHAR				first_flag				= 0;
	//UCHAR				first_delay_count		= 0;
	UINT32				waitcnt					= 0;
	UCHAR				stop_req_flag			= 0;
	E_H265_IF_STREAM_CH	stream_ch				= E_H265_IF_STREAM_CH_0;
	UCHAR				detect_h265_stop		= FALSE;
	T_H265_IF_MSG		msg;
	INT32				delay_flag				= H265_OFF;
	E_VIDEO_ENOCDE_ID	video_enc_id;
	
	// Set Event for REC Task Start
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_CC_INIT_END );
	
	//wait start video process.
	waiptn = (FLG_H265_WDT | FLG_H265_FORCE_STOP | FLG_H265_STOP | FLG_H265_START);
	OS_User_Wai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	
	//Process FLG_H265_WDT & FLG_H265_FORCE_STOP  in next "CC Loop Process".
	if( 0 != (flgptn & FLG_H265_STOP) ){
		detect_h265_stop = TRUE;
		OS_User_Set_Flg( FID_H265, FLG_H265_STOP_ENT_TSK );
		H265_Info_Print(("+++ H265_CC: Request Stop\n"));
	}
	
	if( detect_h265_stop == FALSE ){
		gH265_Prev_Frame_Aligned_Address = 0;
		gH265_Prev_Frame_Size = 0;
		gH265_Prev_Frame_Padding = 0;
		gH265_Prev_Frame_PicCnt = 0;
		gH265_Prev_Slice_PicType = 0;
		gH265_Prev_Slice_WAddr = 0;
		
		while(1){
			waiptn = (FLG_H265_WDT | FLG_H265_FORCE_STOP);
			// received wdt time out msg from Ribery or Record Froce Stop
			ercd = OS_User_Pol_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			if( ercd == D_OS_USER_E_OK ){
				if( 0 != (flgptn & FLG_H265_WDT) ){
					// Received WDT T.O Event
					H265_Err_Print(("+++ H265_CC: WDT Event Received\n"));
				}
				else{ //0 != (flgptn & FLG_H265_FORCE_STOP)
					// Received Force Stop Event
					H265_Err_Print(("+++ H265_CC: Force Stop\n"));
				}
				
				waiptn = FLG_H265_SYNC_STOP_ENC_CH0;
				
				OS_User_Wai_Flg( FID_H265_SYNC, waiptn, D_OS_USER_TWF_ANDW, &flgptn ); 
				OS_User_Clr_Flg( FID_H265_SYNC, ~flgptn );
				H265_Info_Print(("--- H265_CC: all stop request\n" ));
				break;
			}
			else if( ercd == D_OS_USER_E_TMOUT ) {
				;
			}
			else{
				H265_Err_Print(("--- H265_CC: pol_flg ercd1=%x\n", ercd ));
			}
			
			waiptn = FLG_H265_SYNC_ENC_MASTER | FLG_H265_SYNC_ENC_CH0;
			
//			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_CC" ));
			
			OS_User_Wai_Flg( FID_H265_SYNC, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			
//			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H265_CC" ));
			
			if( 0 != (flgptn & FLG_H265_SYNC_ENC_MASTER) ){
				OS_User_Clr_Flg( FID_H265_SYNC, ~FLG_H265_SYNC_ENC_MASTER );
				
				if( (gH265_Image_Store_Count[stream_ch] >= 1) && (first_flag == H265_OFF) ){
					BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H265( stream_ch, &video_enc_id );
					
					if( (gH265_Rec_Param.codec_param[stream_ch].oneshot_encode_flag == H265_OFF) &&
						(BF_Movie_Video_Attr_Get_Timelapse_Flag(video_enc_id) == H265_OFF ) ){
						
						if( gH265_Vin_Adr_Ctl[stream_ch].hold_max_no < 1 ){
							waitcnt++;
//							printf("waitcnt[%d][%d]\n",stream_ch, waitcnt );
							if( waitcnt > 1200 ){
								H265_Err_Print(("+++ H265_CC: View Task Image Update Nothing!!(Between 150VD) %d\n", stream_ch ));
								OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
								break;
							}
						}
						else{
							#if 0
							// Delay of Movie start timming for reloadtimer. (for View Fluctuation)
							switch( gH265_Rec_Param.codec_param[stream_ch].frame_rate ){
								// for 240fps timer
								case E_H265_IF_FRAME_RATE_240_0 :
								case E_H265_IF_FRAME_RATE_239_76 :
									if( first_delay_count == 0 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_120_0 :
								case E_H265_IF_FRAME_RATE_119_88 :
									if( first_delay_count == 1 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_73_0 :
								case E_H265_IF_FRAME_RATE_72_0 :
								case E_H265_IF_FRAME_RATE_70_0 :
								case E_H265_IF_FRAME_RATE_60_0 :
								case E_H265_IF_FRAME_RATE_59_94 :
									if( first_delay_count == 2 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_50_0 :
									if( first_delay_count == 2 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_29_0 :
								case E_H265_IF_FRAME_RATE_25_0 :
									if( first_delay_count == 3 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_24_0 :
								case E_H265_IF_FRAME_RATE_23_97 :
									if( first_delay_count == 6 ){
										first_flag = H265_ON;
									}
									break;
#if 0	// unsupported.
								case E_H265_IF_FRAME_RATE_48_0 :
								case E_H265_IF_FRAME_RATE_47_95 :
									if( first_delay_count == 3 ){
										first_flag = H265_ON;
									}
									break;
#endif
								case E_H265_IF_FRAME_RATE_30_0 :
								case E_H265_IF_FRAME_RATE_29_97 :
									if( first_delay_count == 4 ){
										first_flag = H265_ON;
									}
									break;
#if 0	// unsupported.
								case E_H265_IF_FRAME_RATE_24_0 :
								case E_H265_IF_FRAME_RATE_23_97 :
									if( first_delay_count == 6 ){
										first_flag = H265_ON;
									}
									break;
#endif
								case E_H265_IF_FRAME_RATE_15_0 :
								case E_H265_IF_FRAME_RATE_14_98 :
									if( first_delay_count == 8 ){
										first_flag = H265_ON;
									}
									break;
									
#if 0	// unsupported.
								// for 200fps timer
								case E_H265_IF_FRAME_RATE_200_0 :
									if( first_delay_count == 0 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_100_0 :
									if( first_delay_count == 1 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_50_0 :
									if( first_delay_count == 2 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_25_0 :
									if( first_delay_count == 3 ){
										first_flag = H265_ON;
									}
									break;
#endif
								default :
									H265_Err_Print(("+++ H265_CC: Parameter Error\n"));
									OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
									break;
							}
							
							first_delay_count++;
							#else
							first_flag = H265_ON;
							#endif
							
							// "total_time" is taken as internal time. (It is from 0 after a stop.) 
							if( first_flag == H265_ON ){
								H265_Info_Print(("ch[%d] first_flag on(hold_max_no=%d)\n", stream_ch, gH265_Vin_Adr_Ctl[stream_ch].hold_max_no));
								BF_Movie_Common_Sync_Start_H265( stream_ch );
							}
						}
					}
					else{
						if( gH265_Vin_Adr_Ctl[stream_ch].hold_max_no < 1 ){
							waitcnt++;
//							printf("waitcnt[%d][%d]\n",stream_ch, waitcnt );
							if( waitcnt > 1200 ){
								H265_Err_Print(("+++ H265_CC: View Task Image Update Nothing!!(Between 150VD) %d\n", stream_ch ));
								OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
								break;
							}
						}
						else{
							first_flag = H265_ON;
							BF_Movie_Common_Sync_Start_H265( stream_ch );
							H265_Info_Print(("ch[%d] first_flag on(hold_max_no=%d)\n", stream_ch, gH265_Vin_Adr_Ctl[stream_ch].hold_max_no));
						}
					}
				}
				continue;
			}
			else if( 0 != (flgptn & FLG_H265_SYNC_ENC_CH0) ){
				OS_User_Clr_Flg( FID_H265_SYNC, ~FLG_H265_SYNC_ENC_CH0 );
				waiptn = FLG_H265_SYNC_STOP_ENC_CH0;
			}
			else{
				H265_Err_Print(("+++ H265_CC: Sync Wait: The route which is not assumed.\n" ));
			}
			
			// It updates vop_cnt
			vop_cnt++;
			
			if( gH265_Rec_Param.codec_param[stream_ch].gop_struct != E_H265_IF_GOP_STR_IBBP ){
				if( vop_cnt > gH265_Rec_Param.codec_param[stream_ch].gop_num ){
					vop_cnt = 1;
					gop_cnt++;
					if( gop_cnt > gH265_Rec_Param.codec_param[stream_ch].gop_num_idr_to_idr ){
						gop_cnt = 1;
					}
				}
			}
			else{
				if( first_gop_flg == H265_OFF ){
					if( vop_cnt > gH265_Rec_Param.codec_param[stream_ch].gop_num ){
						vop_cnt = 1;
						gop_cnt++;
						if( gop_cnt > gH265_Rec_Param.codec_param[stream_ch].gop_num_idr_to_idr ){
							gop_cnt = 1;
						}
					}
				}
				else{
					// first GOP is gop_num - 2
					if( vop_cnt > gH265_Rec_Param.codec_param[stream_ch].gop_num - 2 ){
						first_gop_flg = H265_OFF;
						vop_cnt = 1;
						gop_cnt++;
						if( gop_cnt > gH265_Rec_Param.codec_param[stream_ch].gop_num_idr_to_idr ){
							gop_cnt = 1;
						}
					}
				}
			}
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%d vop", vop_cnt ));

			if (gH265_Slice_Encode_En)
				;// move to h265_encode_wave_enc_one_frame()
			else {
				// Free Video Stream
				h265_encode_free_vstream();
			}
			
			if( vop_cnt == 1 ){
				H265_If_Record_Notify_PreEncode( stream_ch, &vop_cnt );
			}
			
			// Check Stop Request
			if( stop_req_flag == H265_OFF ){
				ercd = OS_User_Pol_Flg( FID_H265_SYNC, waiptn, D_OS_USER_TWF_ORW, &flgptn );
				if( ercd == D_OS_USER_E_OK ){
					if( gH265_Rec_Param.codec_param[stream_ch].gop_struct == E_H265_IF_GOP_STR_IBBP ){	// for IBBP struct
						if( first_gop_flg == H265_OFF ){
							if( (vop_cnt % 3) == 0 ){
								H265_Info_Print(("--- H265_CC: stop_req_flag stream_ch =%d\n", stream_ch));
								// recv STOP new image not store
								M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265_CC_stop" ));
								stop_req_flag = H265_ON;
							}
						}
						else{
							if( ((vop_cnt-1) % 3) == 0 ){
								H265_Info_Print(("--- H265_CC: stop_req_flag stream_ch =%d\n", stream_ch));
								// recv STOP new image not store
								M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265_CC_stop" ));
								stop_req_flag = H265_ON;
							}
						}
					}
					else{																// for IPPP/IIII struct
						H265_Info_Print(("--- H265_CC: stop_req_flag stream_ch =%d\n", stream_ch));
						// recv STOP new image not store
						M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265_CC_stop" ));
						stop_req_flag = H265_ON;
					}
				}
				else if( ercd == D_OS_USER_E_TMOUT ){
					;
				}
				else{
					H265_Err_Print(("--- H265_CC: pol_flg ercd2=%x\n", ercd ));
				}
			}
			else{
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "st_wait" ));
				// Already Stop Request Send
			}
			
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Enc_frame S"));
			ret = h265_encode_wave_enc_one_frame( stream_ch, stop_req_flag, &vop_cnt, &gop_cnt, &delay_flag, (INT32*)&gH265_Codec_End_Flg[stream_ch] );
			if( ret != H265_OK ){
				H265_Err_Print(("+++ H265_CC: Encode Parameter Error=%d\n", ret ));
				OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
				break;
			}
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Enc_frame E, delay:%d", delay_flag));
			
			// Recording Time Update
			gH265_Enc_Ctrl.video[stream_ch].total_time_increment += gH265_Enc_Ctrl.video[stream_ch].time_increment;
			
			if (gH265_Slice_Encode_En)
				;// move to h265_encode_wave_enc_one_frame()
			else {
				if( (delay_flag != H265_ON) || (gH265_Codec_Already_Stop_Req_Flag[stream_ch] == TRUE) ){
					//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "SYNC"));
					OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSTRM_COMP0 );
				}
			}

			if( stop_req_flag == H265_ON ){
				// In stream stop & discharge, set flags myself for processing at the fastest speed.
				OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_ENC_CH0 );
				// wait for dispatch
				OS_User_Dly_Tsk(1);
			}
			
			if( gH265_Codec_End_Flg[E_H265_IF_STREAM_CH_0] == H265_ON ){
				break;
			}
		}
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_CC" ));
	
	// for Debug
	if( (gH265_ViewLater_Total_Count != 0) || (gH265_MovieLater_Total_Count != 0) ){
		H265_Warning_Print(("gH265_ViewLater_Total_Count is %d\n", gH265_ViewLater_Total_Count ));
		H265_Warning_Print(("gH265_MovieLater_Total_Count is %d\n", gH265_MovieLater_Total_Count ));
	}
	
	H265_Info_Print(("+++ H265_CC: CC Task End Complete!!\n"));
	
	// Set CC Task Stop Event
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_CC_STOP );
	
	msg.type = E_H265_IF_TYPE_NO_REQUEST;
	ret = H265_If_Set_Msg( &msg, E_H265_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		H265_Err_Print(("--- H265_CC End Msg ERR. ret=%x\n", ret ));
	}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
	
	return;
}


// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
 *  @brief		Encode CC(2nd) Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Encode_CC_2_Main( VOID )
{
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn;
	OS_USER_ER			ercd;
	INT32				ret;
	INT32				vop_cnt					= 0;
	INT32				gop_cnt					= 1;
	UCHAR				first_flag				= 0;
	//UCHAR				first_delay_count		= 0;
	UINT32				waitcnt					= 0;
	UCHAR				stop_req_flag			= 0;
	E_H265_IF_STREAM_CH	stream_ch				= E_H265_IF_STREAM_CH_1;
	UCHAR				detect_h265_stop		= FALSE;
	T_H265_IF_MSG		msg;
	INT32				delay_flag				= H265_OFF;
	E_VIDEO_ENOCDE_ID	video_enc_id;
	
	// Set Event for REC Task Start
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_CC_2_INIT_END );
	
	//wait start video process.
	waiptn = (FLG_H265_WDT | FLG_H265_FORCE_STOP | FLG_H265_STOP | FLG_H265_START);
	OS_User_Wai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	
	//Process FLG_H265_WDT & FLG_H265_FORCE_STOP  in next "CC Loop Process".
	if( 0 != (flgptn & FLG_H265_STOP) ){
		detect_h265_stop = TRUE;
		OS_User_Set_Flg( FID_H265, FLG_H265_STOP_ENT_TSK_CC2 );
		H265_Info_Print(("+++ H265_CC_2: Request Stop\n"));
	}
	
	if( detect_h265_stop == FALSE ){
		while(1){
			waiptn = (FLG_H265_WDT | FLG_H265_FORCE_STOP);
			// received wdt time out msg from Ribery or Record Froce Stop
			ercd = OS_User_Pol_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			if( ercd == D_OS_USER_E_OK ){
				if( 0 != (flgptn & FLG_H265_WDT) ){
					// Received WDT T.O Event
					H265_Err_Print(("+++ H265_CC_2: WDT Event Received\n"));
				}
				else{ //0 != (flgptn & FLG_H265_FORCE_STOP)
					// Received Force Stop Event
					H265_Err_Print(("+++ H265_CC_2: Force Stop\n"));
				}
				
				waiptn = FLG_H265_SYNC_STOP_ENC_CH1;
				
				OS_User_Wai_Flg( FID_H265_SYNC, waiptn, D_OS_USER_TWF_ANDW, &flgptn ); 
				OS_User_Clr_Flg( FID_H265_SYNC, ~flgptn );
				H265_Info_Print(("--- H265_CC_2: all stop request\n" ));
				break;
			}
			else if( ercd == D_OS_USER_E_TMOUT ) {
				;
			}
			else{
				H265_Err_Print(("--- H265_CC_2: pol_flg ercd1=%x\n", ercd ));
			}
			
			waiptn = FLG_H265_SYNC_ENC_MASTER_2 | FLG_H265_SYNC_ENC_CH1;
			
//			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_CC_2" ));
			
			OS_User_Wai_Flg( FID_H265_SYNC, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			
//			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H265_CC_2" ));
			
			if( 0 != (flgptn & FLG_H265_SYNC_ENC_MASTER_2) ){
				OS_User_Clr_Flg( FID_H265_SYNC, ~FLG_H265_SYNC_ENC_MASTER_2 );
				
				if( (gH265_Image_Store_Count[stream_ch] >= 1) && (first_flag == H265_OFF) ){
					BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H265( stream_ch, &video_enc_id );
					
					if( (gH265_Rec_Param.codec_param[stream_ch].oneshot_encode_flag == H265_OFF) &&
						(BF_Movie_Video_Attr_Get_Timelapse_Flag(video_enc_id) == H265_OFF ) ){
						
						if( gH265_Vin_Adr_Ctl[stream_ch].hold_max_no < 1 ){
							waitcnt++;
//							printf("waitcnt_2[%d][%d]\n",stream_ch, waitcnt );
							if( waitcnt > 1200 ){
								H265_Err_Print(("+++ H265_CC_2: View Task Image Update Nothing!!(Between 150VD) %d\n", stream_ch ));
								OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
								break;
							}
						}
						else{
							#if 0
							// Delay of Movie start timming for reloadtimer. (for View Fluctuation)
							switch( gH265_Rec_Param.codec_param[stream_ch].frame_rate ){
								// for 240fps timer
								case E_H265_IF_FRAME_RATE_240_0 :
								case E_H265_IF_FRAME_RATE_239_76 :
									if( first_delay_count == 0 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_120_0 :
								case E_H265_IF_FRAME_RATE_119_88 :
									if( first_delay_count == 1 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_73_0 :
								case E_H265_IF_FRAME_RATE_72_0 :
								case E_H265_IF_FRAME_RATE_70_0 :
								case E_H265_IF_FRAME_RATE_60_0 :
								case E_H265_IF_FRAME_RATE_59_94 :
									if( first_delay_count == 2 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_50_0 :
									if( first_delay_count == 2 ){
										first_flag = H265_ON;
									}
									break;
#if 0	// unsupported.
								case E_H265_IF_FRAME_RATE_48_0 :
								case E_H265_IF_FRAME_RATE_47_95 :
									if( first_delay_count == 3 ){
										first_flag = H265_ON;
									}
									break;
#endif
								case E_H265_IF_FRAME_RATE_30_0 :
								case E_H265_IF_FRAME_RATE_29_97 :
									if( first_delay_count == 4 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_29_0 :
								case E_H265_IF_FRAME_RATE_25_0 :
									if( first_delay_count == 3 ){
										first_flag = H265_ON;
									}
									break;
#if 1	// unsupported. NEED TO CHECK
								case E_H265_IF_FRAME_RATE_24_0 :
								case E_H265_IF_FRAME_RATE_23_97 :
									if( first_delay_count == 6 ){
										first_flag = H265_ON;
									}
									break;
#endif
								case E_H265_IF_FRAME_RATE_15_0 :
								case E_H265_IF_FRAME_RATE_14_98 :
									if( first_delay_count == 8 ){
										first_flag = H265_ON;
									}
									break;
									
#if 0	// unsupported.
								// for 200fps timer
								case E_H265_IF_FRAME_RATE_200_0 :
									if( first_delay_count == 0 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_100_0 :
									if( first_delay_count == 1 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_50_0 :
									if( first_delay_count == 2 ){
										first_flag = H265_ON;
									}
									break;
								case E_H265_IF_FRAME_RATE_25_0 :
									if( first_delay_count == 3 ){
										first_flag = H265_ON;
									}
									break;
#endif
								default :
									H265_Err_Print(("+++ H265_CC: Parameter Error\n"));
									OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
									break;
							}
							
							first_delay_count++;
							#else
							first_flag = H265_ON;
							#endif
							
							// "total_time" is taken as internal time. (It is from 0 after a stop.) 
							if( first_flag == H265_ON ){
								H265_Info_Print(("ch[%d] first_flag on(hold_max_no=%d)\n", stream_ch, gH265_Vin_Adr_Ctl[stream_ch].hold_max_no));
								BF_Movie_Common_Sync_Start_H265( stream_ch );
							}
						}
					}
					else{
						if( gH265_Vin_Adr_Ctl[stream_ch].hold_max_no < 1 ){
							waitcnt++;
//							printf("waitcnt_2[%d][%d]\n",stream_ch, waitcnt );
							if( waitcnt > 1200 ){
								H265_Err_Print(("+++ H265_CC_2: View Task Image Update Nothing!!(Between 150VD) %d\n", stream_ch ));
								OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
								break;
							}
						}
						else{
							first_flag = H265_ON;
							BF_Movie_Common_Sync_Start_H265( stream_ch );
							H265_Info_Print(("ch[%d] first_flag on(hold_max_no=%d)\n", stream_ch, gH265_Vin_Adr_Ctl[stream_ch].hold_max_no));
						}
					}
				}
				continue;
			}
			else if( 0 != (flgptn & FLG_H265_SYNC_ENC_CH1) ){
				OS_User_Clr_Flg( FID_H265_SYNC, ~FLG_H265_SYNC_ENC_CH1 );
				waiptn = FLG_H265_SYNC_STOP_ENC_CH1;
			}
			else{
				H265_Err_Print(("+++ H265_CC_2: Sync Wait: The route which is not assumed.\n" ));
			}
			
			// It updates vop_cnt
			vop_cnt++;
			if( vop_cnt > gH265_Rec_Param.codec_param[stream_ch].gop_num ){
				vop_cnt = 1;
				gop_cnt++;
				if( gop_cnt > gH265_Rec_Param.codec_param[stream_ch].gop_num_idr_to_idr ){
					gop_cnt = 1;
				}
			}
			
			// Free Video Stream
			h265_encode_free_vstream();
			
			if( vop_cnt == 1 ){
				H265_If_Record_Notify_PreEncode( stream_ch, &vop_cnt );
			}
			
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "Enc_frame_2" ));
			ret = h265_encode_wave_enc_one_frame( stream_ch, stop_req_flag, &vop_cnt, &gop_cnt, &delay_flag, (INT32*)&gH265_Codec_End_Flg[stream_ch] );
			if( ret != H265_OK ){
				H265_Err_Print(("+++ H265_CC_2: Encode Parameter Error=%d\n", ret ));
				OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
				break;
			}
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "Enc_frame_2" ));
			
			// Recording Time Update
			gH265_Enc_Ctrl.video[stream_ch].total_time_increment += gH265_Enc_Ctrl.video[stream_ch].time_increment;
			
			if( (delay_flag != H265_ON) || (gH265_Codec_Already_Stop_Req_Flag[stream_ch] == TRUE) ){
				OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSTRM_COMP1 );
			}
			if( stop_req_flag == H265_ON ){
				// In stream stop & discharge, set flags myself for processing at the fastest speed.
				OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_ENC_CH1 );
				// wait for dispatch
				OS_User_Dly_Tsk(1);
			}
			
			if( gH265_Codec_End_Flg[E_H265_IF_STREAM_CH_1] == H265_ON ){
				break;
			}
			// Check Stop Request
			if( stop_req_flag == H265_OFF ){
				ercd = OS_User_Pol_Flg( FID_H265_SYNC, waiptn, D_OS_USER_TWF_ORW, &flgptn );
				if( ercd == D_OS_USER_E_OK ){
					if( gH265_Rec_Param.codec_param[stream_ch].gop_struct == E_H265_IF_GOP_STR_IBBP ){	// for IBBP struct
						if( (vop_cnt % 3) == 0 ){
							H265_Info_Print(("--- H265_CC_2: stop_req_flag stream_ch =%d\n", stream_ch));
							// recv STOP new image not store
							M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265_CC_2_stop" ));
							stop_req_flag = H265_ON;
							// In stream stop & discharge, set flags myself for processing at the fastest speed.
							OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_ENC_CH1 );
							// wait for dispatch
							OS_User_Dly_Tsk(1);
						}
					}
					else{																// for IPPP/IIII struct
						H265_Info_Print(("--- H265_CC_2: stop_req_flag stream_ch =%d\n", stream_ch));
						// recv STOP new image not store
						M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265_CC_2_stop" ));
						stop_req_flag = H265_ON;
						// In stream stop & discharge, set flags myself for processing at the fastest speed.
						OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_ENC_CH1 );
						// wait for dispatch
						OS_User_Dly_Tsk(1);
					}
				}
				else if( ercd == D_OS_USER_E_TMOUT ){
					;
				}
				else{
					H265_Err_Print(("--- H265_CC_2: pol_flg ercd2=%x\n", ercd ));
				}
			}
			else{
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "st_wait_2" ));
				// Already Stop Request Send
			}
		}
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_CC_2" ));
	
#if 0	// It is unnecessary here because it is done by CC_Main task.
	// for Debug
	if( (gH265_ViewLater_Total_Count != 0) || (gH265_MovieLater_Total_Count != 0) ){
		H265_Warning_Print(("gH265_ViewLater_Total_Count is %d\n", gH265_ViewLater_Total_Count ));
		H265_Warning_Print(("gH265_MovieLater_Total_Count is %d\n", gH265_MovieLater_Total_Count ));
	}
#endif
	
	H265_Info_Print(("+++ H265_CC_2: CC Task End Complete!!\n"));
	
	// Set CC Task Stop Event
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_CC_2_STOP );
	
	msg.type = E_H265_IF_TYPE_NO_REQUEST;
	ret = H265_If_Set_Msg( &msg, E_H265_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		H265_Err_Print(("--- H265_CC_2 End Msg ERR. ret=%x\n", ret ));
	}
	
	return;
}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---


// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
/**
 *  @brief		Encode ENT INT Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Encode_ENT_INT_Main( VOID )
{
	INT32					ret;
	OS_USER_FLGPTN			flgptn;
	OS_USER_FLGPTN			waiptn;
	T_SR_ENC_VINFO			vinfo0, vinfo1;
	T_SR_ENC_VINFO			vinfo_tmp;
	UINT32					nal_loop;
	
	gH265_VframeComp_Set_Flag[E_H265_IF_STREAM_CH_0] = H265_OFF;
	gH265_VframeComp_Set_Flag[E_H265_IF_STREAM_CH_1] = H265_OFF;
//	H265_Movie_Memset( (VOID*)&gH265_Vstrm_Fifo, 0, sizeof(gH265_Vstrm_Fifo) );	// Move to common initialization.
	H265_Movie_Memset( (VOID*)&gH265_Slice_Info, 0, sizeof(gH265_Slice_Info) );
	H265_Movie_Memset( (VOID*)&gH265_Frame_Info, 0, sizeof(gH265_Frame_Info) );
	H265_Movie_Memset( (VOID*)&vinfo0,           0, sizeof(vinfo0) );
	H265_Movie_Memset( (VOID*)&vinfo1,           0, sizeof(vinfo1) );
	H265_Movie_Memset( (VOID*)&vinfo_tmp,        0, sizeof(vinfo_tmp) );
	
	// Report ENT INT Task Start
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_ENT_INT_INIT_END );
	
	waiptn = (FLG_H265_ENC_VSLICE_COMP0 | FLG_H265_ENC_VSLICE_COMP1 | FLG_H265_ENC_VFRAME_COMP0_TO_ENT | FLG_H265_ENC_VFRAME_COMP1_TO_ENT | FLG_H265_WDT | FLG_H265_FORCE_STOP | FLG_H265_STOP_ENT_TSK);
	
	while(1){
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_EINT" ));
		
		OS_User_Wai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H265_EINT" ));
		
		if( 0 != (flgptn & FLG_H265_WDT) ){
			H265_Err_Print(("+++ H265 ENT INT Task: WDT Event Received\n"));
			return;
		}
		if( 0 != (flgptn & FLG_H265_FORCE_STOP) ){
			H265_Err_Print(("+++ H265 ENT INT Task: Force Stop Received\n"));
			return;
		}
		if( 0 != (flgptn & FLG_H265_STOP_ENT_TSK) ){
			H265_Info_Print(("+++ H265 ENT INT Task: ENT STOP Received\n"));
			return;
		}
		
		if( 0 != (flgptn & FLG_H265_ENC_VSLICE_COMP0) ){
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_VSLICE_COMP0 );
			
			gH265_VframeComp_Set_Flag[E_H265_IF_STREAM_CH_0] = H265_OFF;
			
			ret = H265_Sirius_Enc_Frame_Info_Get( E_H265_IF_STREAM_CH_0, &vinfo0 );
			if( ret == H265_NG ){
				H265_Err_Print(("--- H265_Sirius_Enc_Frame_Info_Get Error\n" ));
			}
			
			for( nal_loop=0; nal_loop<vinfo0.nal_unit_num; nal_loop++ ){
				gH265_Slice_Info[E_H265_IF_STREAM_CH_0].nal_unit_size[nal_loop] += vinfo0.nal_unit_size[nal_loop];
			}
			
#ifdef CO_DEBUG_ON_PC
			// dispatch
			OS_User_Tslp_Tsk( 1 );
#endif
			if( 0 != (flgptn & FLG_H265_ENC_VFRAME_COMP0_TO_ENT) ){
				OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_VFRAME_COMP0_TO_ENT );
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265_EINT1_0" ));
				
				gH265_VframeComp_Set_Flag[E_H265_IF_STREAM_CH_0] = H265_ON;
				
				OS_User_Wai_Sem(SID_H265_STREAM_INT);
				if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].fifo_over_flg == H265_ON ){
					H265_Err_Print(("--- H265 VstrmCH0 Fifo Over Flow!!\n" ));
				}
				OS_User_Sig_Sem(SID_H265_STREAM_INT);
				
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].frame_end_veb_pointer	= vinfo0.frame_end_veb_pointer;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].flag					= vinfo0.flag;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].type					= vinfo0.type;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].period					= vinfo0.period;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].cpb_over_flag			= vinfo0.cpb_over_flag;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].dts					= vinfo0.dts;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].pts					= vinfo0.pts;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].picture_size			= vinfo0.picture_size;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].nal_unit_num			= vinfo0.nal_unit_num;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].serial_num				= vinfo0.serial_num;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].frame_count_in_gop		= vinfo0.frame_count_in_gop;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].poc_val				= vinfo0.poc_val;
				
				if( (vinfo0.flag &= 0x0001) == 1 ){
					gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_0] = H265_ON;
					gH265_Map_Enable_Flg[E_H265_IF_STREAM_CH_0] = H265_OFF;
					gH265_Map_First_Flg[E_H265_IF_STREAM_CH_0] = H265_ON;
					M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "END_FLAG" ));
				}
				
				for( nal_loop=0; nal_loop<vinfo0.nal_unit_num; nal_loop++ ){
					gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].nal_unit_size[nal_loop] = gH265_Slice_Info[E_H265_IF_STREAM_CH_0].nal_unit_size[nal_loop];
					gH265_Slice_Info[E_H265_IF_STREAM_CH_0].nal_unit_size[nal_loop] = 0;
				}
				
				OS_User_Wai_Sem(SID_H265_STREAM_INT);
				gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point++;
				if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point >= D_H265_ENCODE_VINFO_FIFO_MAX ){
					gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point = 0;
				}
				
				if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point == gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].r_point ){
					gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].fifo_over_flg = H265_ON;
				}
				OS_User_Sig_Sem(SID_H265_STREAM_INT);
				OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSTRM_COMP0 );
			}
		}
		else if( 0 != (flgptn & FLG_H265_ENC_VSLICE_COMP1) ){
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_VSLICE_COMP1 );
			
			gH265_VframeComp_Set_Flag[E_H265_IF_STREAM_CH_1] = H265_OFF;
			
			ret = H265_Sirius_Enc_Frame_Info_Get( E_H265_IF_STREAM_CH_1, &vinfo1 );
			if( ret == H265_NG ){
				H265_Err_Print(("--- H265_Sirius_Enc_Frame_Info_Get Error\n" ));
			}
			
			for( nal_loop=0; nal_loop<vinfo1.nal_unit_num; nal_loop++ ){
				gH265_Slice_Info[E_H265_IF_STREAM_CH_1].nal_unit_size[nal_loop] += vinfo1.nal_unit_size[nal_loop];
			}
			
#ifdef CO_DEBUG_ON_PC
			// dispatch
			OS_User_Tslp_Tsk( 1 );
#endif
			if( 0 != (flgptn & FLG_H265_ENC_VFRAME_COMP1_TO_ENT) ){
				OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_VFRAME_COMP1_TO_ENT );
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265_EINT1_1" ));
				
				gH265_VframeComp_Set_Flag[E_H265_IF_STREAM_CH_1] = H265_ON;
				
				OS_User_Wai_Sem(SID_H265_STREAM_INT);
				if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].fifo_over_flg == H265_ON ){
					H265_Err_Print(("--- H265 VstrmCH1 Fifo Over Flow!!\n" ));
				}
				OS_User_Sig_Sem(SID_H265_STREAM_INT);
				
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].frame_end_veb_pointer	= vinfo1.frame_end_veb_pointer;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].flag					= vinfo1.flag;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].type					= vinfo1.type;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].period					= vinfo1.period;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].cpb_over_flag			= vinfo1.cpb_over_flag;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].dts					= vinfo1.dts;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].pts					= vinfo1.pts;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].picture_size			= vinfo1.picture_size;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].nal_unit_num			= vinfo1.nal_unit_num;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].serial_num				= vinfo1.serial_num;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].frame_count_in_gop		= vinfo1.frame_count_in_gop;
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].poc_val				= vinfo1.poc_val;
				
				if( (vinfo1.flag &= 0x0001) == 1 ){
					gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_1] = H265_ON;
					gH265_Map_Enable_Flg[E_H265_IF_STREAM_CH_1] = H265_OFF;
					gH265_Map_First_Flg[E_H265_IF_STREAM_CH_1] = H265_ON;
				}
				
				for( nal_loop=0; nal_loop<vinfo1.nal_unit_num; nal_loop++ ){
					gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].nal_unit_size[nal_loop] = gH265_Slice_Info[E_H265_IF_STREAM_CH_1].nal_unit_size[nal_loop];
					gH265_Slice_Info[E_H265_IF_STREAM_CH_1].nal_unit_size[nal_loop] = 0;
				}
				
				OS_User_Wai_Sem(SID_H265_STREAM_INT);
				gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point++;
				if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point >= D_H265_ENCODE_VINFO_FIFO_MAX ){
					gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point = 0;
				}
				
				if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point == gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].r_point ){
					gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].fifo_over_flg = H265_ON;
				}
				OS_User_Sig_Sem(SID_H265_STREAM_INT);
				OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSTRM_COMP1 );
			}
		}
		
		if( (0 == (flgptn & FLG_H265_ENC_VSLICE_COMP0)) &&
			(0 != (flgptn & FLG_H265_ENC_VFRAME_COMP0_TO_ENT)) ){
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_VFRAME_COMP0_TO_ENT );
			
			if( gH265_VframeComp_Set_Flag[E_H265_IF_STREAM_CH_0] == H265_ON ){
				continue;
			}
			
			ret = H265_Sirius_Enc_Frame_Info_Get( E_H265_IF_STREAM_CH_0, &vinfo_tmp );
			if( ret == H265_NG ){
				H265_Err_Print(("--- H265_Sirius_Enc_Frame_Info_Get Error\n" ));
			}
			if( 0 != (vinfo_tmp.flag & 0x0080) ){
				H265_Info_Print(("--- Bframe Deley 0\n" ));
				continue;
			}
			
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265_EINT2_0" ));
			
			OS_User_Wai_Sem(SID_H265_STREAM_INT);
			if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].fifo_over_flg == H265_ON ){
				H265_Err_Print(("--- H265 VstrmCH0 Fifo Over Flow!! 2\n" ));
			}
			OS_User_Sig_Sem(SID_H265_STREAM_INT);
			
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].frame_end_veb_pointer	= vinfo0.frame_end_veb_pointer;	/* pgr0872 */ /* pgr0000 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].flag					= vinfo0.flag;					/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].type					= vinfo0.type;					/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].period					= vinfo0.period;				/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].cpb_over_flag			= vinfo0.cpb_over_flag;			/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].dts					= vinfo0.dts;					/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].pts					= vinfo0.pts;					/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].picture_size			= vinfo0.picture_size;			/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].nal_unit_num			= vinfo0.nal_unit_num;			/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].serial_num				= vinfo0.serial_num;			/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].frame_count_in_gop		= vinfo0.frame_count_in_gop;	/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].poc_val				= vinfo0.poc_val;				/* pgr0872 */
			
			if( (vinfo0.flag &= 0x0001) == 1 ){
				gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_0] = H265_ON;
				gH265_Map_Enable_Flg[E_H265_IF_STREAM_CH_0] = H265_OFF;
				gH265_Map_First_Flg[E_H265_IF_STREAM_CH_0] = H265_ON;
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "END_FLAG" ));
			}
			
			for( nal_loop=0; nal_loop<vinfo0.nal_unit_num; nal_loop++ ){
				gH265_Frame_Info[E_H265_IF_STREAM_CH_0][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point].nal_unit_size[nal_loop] = gH265_Slice_Info[E_H265_IF_STREAM_CH_0].nal_unit_size[nal_loop];
				gH265_Slice_Info[E_H265_IF_STREAM_CH_0].nal_unit_size[nal_loop] = 0;
			}
			
			OS_User_Wai_Sem(SID_H265_STREAM_INT);
			gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point++;
			if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point >= D_H265_ENCODE_VINFO_FIFO_MAX ){
				gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point = 0;
			}
			
			if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].w_point == gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].r_point ){
				gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_0].fifo_over_flg = H265_ON;
			}
			OS_User_Sig_Sem(SID_H265_STREAM_INT);
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSTRM_COMP0 );
		}
		else if( (0 == (flgptn & FLG_H265_ENC_VSLICE_COMP1)) &&
				 (0 != (flgptn & FLG_H265_ENC_VFRAME_COMP1_TO_ENT)) ){
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_VFRAME_COMP1_TO_ENT );
			
			if( gH265_VframeComp_Set_Flag[E_H265_IF_STREAM_CH_1] == H265_ON ){
				continue;
			}
			
			ret = H265_Sirius_Enc_Frame_Info_Get( E_H265_IF_STREAM_CH_1, &vinfo_tmp );
			if( ret == H265_NG ){
				H265_Err_Print(("--- H265_Sirius_Enc_Frame_Info_Get Error\n" ));
			}
			if( 0 != (vinfo_tmp.flag & 0x0080) ){
				H265_Info_Print(("--- Bframe Deley 1\n" ));
				continue;
			}
			
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265_EINT2_1" ));
			
			OS_User_Wai_Sem(SID_H265_STREAM_INT);
			if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].fifo_over_flg == H265_ON ){
				H265_Err_Print(("--- H265 VstrmCH1 Fifo Over Flow!! 2\n" ));
			}
			OS_User_Sig_Sem(SID_H265_STREAM_INT);
			
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].frame_end_veb_pointer	= vinfo1.frame_end_veb_pointer;	/* pgr0872 */ /* pgr0000 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].flag					= vinfo1.flag;					/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].type					= vinfo1.type;					/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].period					= vinfo1.period;				/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].cpb_over_flag			= vinfo1.cpb_over_flag;			/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].dts					= vinfo1.dts;					/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].pts					= vinfo1.pts;					/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].picture_size			= vinfo1.picture_size;			/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].nal_unit_num			= vinfo1.nal_unit_num;			/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].serial_num				= vinfo1.serial_num;			/* pgr0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].frame_count_in_gop		= vinfo1.frame_count_in_gop;	/* pgt0872 */
			gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].poc_val				= vinfo1.poc_val;				/* pgr0872 */
			
			if( (vinfo1.flag &= 0x0001) == 1 ){
				gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_1] = H265_ON;
				gH265_Map_Enable_Flg[E_H265_IF_STREAM_CH_1] = H265_OFF;
				gH265_Map_First_Flg[E_H265_IF_STREAM_CH_1] = H265_ON;
			}
			
			for( nal_loop=0; nal_loop<vinfo1.nal_unit_num; nal_loop++ ){
				gH265_Frame_Info[E_H265_IF_STREAM_CH_1][gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point].nal_unit_size[nal_loop] = gH265_Slice_Info[E_H265_IF_STREAM_CH_1].nal_unit_size[nal_loop];
				gH265_Slice_Info[E_H265_IF_STREAM_CH_1].nal_unit_size[nal_loop] = 0;
			}
			
			OS_User_Wai_Sem(SID_H265_STREAM_INT);
			gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point++;
			if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point >= D_H265_ENCODE_VINFO_FIFO_MAX ){
				gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point = 0;
			}
			
			if( gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].w_point == gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].r_point ){
				gH265_Vstrm_Fifo[E_H265_IF_STREAM_CH_1].fifo_over_flg = H265_ON;
			}
			OS_User_Sig_Sem(SID_H265_STREAM_INT);
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSTRM_COMP1 );
		}
		
		if( (gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_0] == H265_ON) &&
			(gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_1] == H265_ON) ){
			H265_Info_Print(("+++ H265 ENT INT Task: All Ch STOP Received\n"));
			break;
		}
	}
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_EINT" ));
	
	return;
}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---

/**
 *  @brief		Encode ENT Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Encode_ENT_Main( VOID )
{
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	INT32					ret;
	ULLONG					total_duration;
	OS_USER_FLGPTN			flgptn;
	OS_USER_FLGPTN			waiptn;
	E_H265_IF_STREAM_CH		stream_ch = E_H265_IF_STREAM_CH_0;
	T_SR_ENC_FM_TABLE		fm_table;
	OS_USER_ER				ercd;
	T_H265_ENC_VIDEO_INFO	vstrm_info;
	T_H265_IF_VIDEO_STREAM_PARAM video_stream_info;
	LLONG					pes_set_pts;
	LLONG					pes_set_dts;
	T_H265_IF_MSG			msg;
	INT32					loop_count;
	INT32					break_flg = H265_OFF;
#ifndef CO_DEBUG_ON_PC
	UCHAR					flag_first_proc = H265_ON;
#endif
	
	// 0. Initialize Local Data
	H265_Movie_Memset( &vstrm_info, 0, sizeof(vstrm_info) );
	H265_Movie_Memset( &video_stream_info, 0, sizeof(video_stream_info) );
	H265_Movie_Memset( &fm_table, 0, sizeof(fm_table) );
	total_duration = 0;
	ret = H265_OK;
	
	// 2. Report ENT Task Start
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_ENT_INIT_END );
	
	waiptn = (FLG_H265_ENC_VSTRM_COMP0 | FLG_H265_ENC_VSTRM_COMP1 | FLG_H265_WDT | FLG_H265_FORCE_STOP | FLG_H265_STOP_ENT_TSK);
	
	// 3. ENT Task Loop Process
	while(1){
		
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_ENT" ));
		
		ercd = OS_User_Wai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H265_ENT" ));
		
		// Received WDT T.O Event
		if( 0 != (flgptn & FLG_H265_WDT) ){
			H265_Err_Print(("+++ ENT Task: WDT Event Received\n"));
			// Set Record End Status
			gH265_Enc_Ctrl.task_result = H265_ERR_SYSTEM;
			h265_encode_illagal();
			
			return;
		}
		// Received Force Stop Event
		if( 0 != (flgptn & FLG_H265_FORCE_STOP) ){
			H265_Err_Print(("+++ ENT Task: Force Stop\n"));
			// Set Record End Status
			gH265_Enc_Ctrl.task_result = H265_ERR_SYSTEM;
			h265_encode_illagal();
			
			return;
		}		
		// Received Task Stop Event
		if (0 != (flgptn & FLG_H265_STOP_ENT_TSK)){
			H265_Info_Print(("+++ ENT Task: Task Request Stop\n"));
			break;
		}
		
		if( 0 != (flgptn & FLG_H265_ENC_VSTRM_COMP0) ){
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_VSTRM_COMP0 );
			stream_ch = E_H265_IF_STREAM_CH_0;
		}
		else if( 0 != (flgptn & FLG_H265_ENC_VSTRM_COMP1) ){
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_VSTRM_COMP1 );
			stream_ch = E_H265_IF_STREAM_CH_1;
		}
		
#ifndef CO_DEBUG_ON_PC
		flag_first_proc = H265_OFF;
#endif
		// Free Video Stream
		h265_encode_free_vstream();
		
		// 3.1. Get Encoder Result
		ret = h265_encode_sirius_result_get( stream_ch, &vstrm_info );
		if( ret != H265_OK ){
			// PARAM NG
			H265_Err_Print(("+++ H265_ENT: h265_encode_sirius_result_get. ret=%d\n", ret ));
			continue;
		}
		
		if( gH265_Rec_Param.codec_param[stream_ch].stream_format == E_H265_IF_STREAM_FORMAT_PES ){
			pes_set_pts = (LLONG)vstrm_info.sample_pts;
			pes_set_dts = (LLONG)vstrm_info.sample_dts;
			
			h265_encode_pes_header_set( stream_ch,
										(ULONG)vstrm_info.sample_address,
										vstrm_info.sample_size,
										pes_set_pts,
										pes_set_dts );
			
			vstrm_info.sample_address	-= D_H265_ENCODE_PES_HEADER_VIDEO_SIZE;
			vstrm_info.sample_size		+= D_H265_ENCODE_PES_HEADER_VIDEO_SIZE;
		}
		else{
			// stream_format is H265_STREAM_FORMAT_MP4
			// do nothing.
		}
		
		if( vstrm_info.stream_end_flg == H265_ON ){
			gH265_Last_FrameNo = vstrm_info.frame_no;
			H265_Info_Print(("+++ H265_ENT: [ch%d]Received Last Stream.\n", stream_ch ));
			gH265_Rec_Param.codec_param[stream_ch].enable_flg = H265_OFF;
		}
		
		h265_encode_video_stream_param_set( stream_ch, &vstrm_info, &video_stream_info, &total_duration );
		video_stream_info.error_flg = FALSE;
		ret = H265_If_Record_Notify_Video_Stream( stream_ch, &video_stream_info );
		if( !ret ){
			H265_Err_Print(("+++ H265_ENT: H265_If_Record_Notify_Video_Stream Error.ret=%d\n",ret ));
			
			// Set Record End Status
			gH265_Enc_Ctrl.task_result = H265_ERR_SYSTEM;
			h265_encode_illagal();
			
			return;
		}
		
		// Check End of All Video Stream
		if( (gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_0] == H265_ON) &&
			(gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_1] == H265_ON) ){
			
			loop_count = 0;
			
			while(1){
				ercd = OS_User_Pol_Flg( FID_H265, gH265_Sirius_Stop_Done_Wiat_Ptn, D_OS_USER_TWF_ANDW, &flgptn );
				if( ercd == D_OS_USER_E_OK ){
					break_flg = H265_ON;
					break;
				}
				
				OS_User_Dly_Tsk(1);
				loop_count++;
				
				h265_encode_free_vstream();
				
				if( loop_count > 100 ){
					H265_Err_Print(("+++ H265_ENT: stop done not comming.\n" ));
					break_flg = H265_ON;
					break;
				}
			}
		}
		if( break_flg == H265_ON ){
			break;
		}
	}
#ifndef CO_DEBUG_ON_PC
	if (flag_first_proc == H265_OFF){
		// Free all video stream
		OS_User_Twai_Flg( FID_H265, FLG_H265_ENC_FREE_ALL_VSTREAM, D_OS_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_FOREVER );
		OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_FREE_ALL_VSTREAM );
	}
#endif
	
	gH265_Enc_Ctrl.task_result = H265_OK_NORMAL_END;
	
	waiptn = FLG_H265_TASK_CC_STOP;
	ercd = OS_User_Twai_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ANDW, &flgptn, 1000 );
	if( ercd != D_OS_USER_E_OK ){
		H265_Err_Print(("+++ ENT Task End: Time out ercd=%x\n", ercd ));
	}
	
	ercd = OS_User_Twai_Flg( FID_H265, gH265_Sirius_Stop_Done_Wiat_Ptn, D_OS_USER_TWF_ANDW, &flgptn, 1000 );
	if( ercd != D_OS_USER_E_OK ){
		H265_Err_Print(("+++ ENT Task: Sirius Stop Done Time out ercd=%x\n", ercd ));
	}
	
	// ENT Task End Process
	h265_encode_result_notify( gH265_Enc_Ctrl.task_result );
	H265_Sirius_Finalize();
	
	H265_Info_Print(("+++ H265_ENT: ENT Task End Complete!!\n"));
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_ENT" ));
	
	// 4. Set ENT Task Stop Event 
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_ENT_STOP );
	OS_User_Set_Flg( FID_H265, FLG_H265_STOP_ENT_TSK );
		
	msg.type = E_H265_IF_TYPE_NO_REQUEST;
	ret = H265_If_Set_Msg( &msg, E_H265_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		H265_Err_Print(("--- ENT End Msg ERR. ret=%x\n", ret ));
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	INT32					ret;
	ULLONG					total_duration;
	OS_USER_FLGPTN			flgptn;
	OS_USER_FLGPTN			waiptn;
	E_H265_IF_STREAM_CH		stream_ch = E_H265_IF_STREAM_CH_0;
	OS_USER_ER				ercd;
	T_H265_ENC_VIDEO_INFO	vstrm_info;
	T_H265_IF_VIDEO_STREAM_PARAM video_stream_info;
	LLONG					pes_set_pts;
	LLONG					pes_set_dts;
	T_H265_IF_MSG			msg;
#ifndef CO_DEBUG_ON_PC
	UCHAR					flag_first_proc = H265_ON;
#endif
	BOOL					cc2_enable = FALSE;
	ULLONG					tmp_dts;
	static UINT32 			end_time = 0, start_time = 0, diff_time;
	
	// Initialize Local Data
	H265_Movie_Memset( &vstrm_info, 0, sizeof(vstrm_info) );
	H265_Movie_Memset( &video_stream_info, 0, sizeof(video_stream_info) );
	if( gH265_Rec_Param.codec_param[E_H265_IF_STREAM_CH_1].enable_flg == H265_ON ){
		cc2_enable = TRUE;
	}
	
	// Report ENT Task Start
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_ENT_INIT_END );
	
	waiptn = (FLG_H265_ENC_VSTRM_COMP0 | FLG_H265_ENC_VSTRM_COMP1 | FLG_H265_WDT | FLG_H265_FORCE_STOP | FLG_H265_STOP_ENT_TSK);
	
	if( cc2_enable == TRUE ){
		waiptn |= FLG_H265_STOP_ENT_TSK_CC2;
	}
	
	// 3. ENT Task Loop Process
	while(1){
		
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_ENT" ));
		
		ercd = OS_User_Wai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H265_ENT" ));
		
		// Received WDT T.O Event
		if( 0 != (flgptn & FLG_H265_WDT) ){
			H265_Err_Print(("+++ ENT Task: WDT Event Received\n"));
			// Set Record End Status
			gH265_Enc_Ctrl.task_result = H265_ERR_SYSTEM;
			h265_encode_illagal();
			
			return;
		}
		// Received Force Stop Event
		if( 0 != (flgptn & FLG_H265_FORCE_STOP) ){
			H265_Err_Print(("+++ ENT Task: Force Stop\n"));
			// Set Record End Status
			gH265_Enc_Ctrl.task_result = H265_ERR_SYSTEM;
			h265_encode_illagal();
			
			return;
		}		
		// Received Task Stop Event
		if( cc2_enable == TRUE ){
			if( (0 != (flgptn & FLG_H265_STOP_ENT_TSK)) && (0 != (flgptn & FLG_H265_STOP_ENT_TSK_CC2)) ){
				H265_Info_Print(("+++ ENT Task: Task Request Stop\n"));
				break;
			}
		}
		else{
			if( 0 != (flgptn & FLG_H265_STOP_ENT_TSK) ){
				H265_Info_Print(("+++ ENT Task: Task Request Stop\n"));
				break;
			}
		}
		
		if( 0 != (flgptn & FLG_H265_ENC_VSTRM_COMP0) ){
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_VSTRM_COMP0 );
			stream_ch = E_H265_IF_STREAM_CH_0;
		}
		else if( 0 != (flgptn & FLG_H265_ENC_VSTRM_COMP1) ){
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_VSTRM_COMP1 );
			stream_ch = E_H265_IF_STREAM_CH_1;
		}
		else{
			H265_Info_Print(("+++ H265_ENT: continue...\n" ));
			continue;
		}

		if (gH265_Rec_Param.codec_param[stream_ch].delay_notify_flag == TRUE) {
			tmp_dts	= ((ULLONG)gH265_Vstrem_Serial_Num[stream_ch] ) * gH265_Enc_Ctrl.video[ stream_ch ].time_increment;
			if (tmp_dts > 0) {
				end_time= BF_Get_Time_us();
				if(start_time == 0) diff_time = 0;
				else diff_time = end_time - start_time;
				start_time= BF_Get_Time_us();
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Notify %d", diff_time ));
				ret = H265_If_Record_Notify_Video_Stream( stream_ch, &g_video_stream_info[ stream_ch ] );
				if( !ret ){
					H265_Err_Print(("+++ H265_ENT: H265_If_Record_Notify_Video_Stream Error.ret=%d\n",ret ));

					// Set Record End Status
					gH265_Enc_Ctrl.task_result = H265_ERR_SYSTEM;
					h265_encode_illagal();

					return;
				}
			}
		}
		
		// 3.1. Get Encoder Result
		ret = h265_encode_wave_result_get( stream_ch, &vstrm_info );
		if( ret != H265_OK ){
			// PARAM NG
			H265_Err_Print(("+++ H265_ENT: h265_encode_wave_result_get. ret=%d\n", ret ));
			continue;
		}
		if( (gH265_Rec_Param.codec_param[stream_ch].stream_format == E_H265_IF_STREAM_FORMAT_PES) && (vstrm_info.stream_end_flg == H265_OFF) ){
			pes_set_pts = (LLONG)vstrm_info.sample_pts;
			pes_set_dts = (LLONG)vstrm_info.sample_dts;
			
			h265_encode_pes_header_set( stream_ch,
										(ULONG)vstrm_info.sample_address,
										vstrm_info.sample_size,
										pes_set_pts,
										pes_set_dts );
			
			vstrm_info.sample_address	-= D_H265_ENCODE_PES_HEADER_VIDEO_SIZE;
			vstrm_info.sample_size		+= D_H265_ENCODE_PES_HEADER_VIDEO_SIZE;
		}
		else{
			// stream_format is H265_STREAM_FORMAT_MP4
			// do nothing.
		}
		
		if( vstrm_info.stream_end_flg == H265_ON ){
			gH265_Last_FrameNo = vstrm_info.frame_no;
			H265_Info_Print(("+++ H265_ENT: [ch%d]Received Last Stream.\n", stream_ch ));
			gH265_Rec_Param.codec_param[stream_ch].enable_flg = H265_OFF;
			if ( gH265_Codec_Queue_Ctrl[gH265_Codec_Handle[stream_ch]->instIndex] ){
				H265_Wave_Queue_Destroy( (T_H265_WAVE_QUEUE*)gH265_Codec_Queue_Ctrl[gH265_Codec_Handle[stream_ch]->instIndex] );
			}
			H265_Wave_EncClose( stream_ch, gH265_Codec_Handle[stream_ch] );
			h265_encode_ch_map_reset( stream_ch );
		}
		
		h265_encode_video_stream_param_set( stream_ch, &vstrm_info, &video_stream_info, &total_duration );
		video_stream_info.error_flg = FALSE;

		if (gH265_Rec_Param.codec_param[stream_ch].delay_notify_flag == TRUE) {
			if (vstrm_info.stream_end_flg == H265_ON) {
				ret = H265_If_Record_Notify_Video_Stream( stream_ch, &video_stream_info );
				if( !ret ){
					H265_Err_Print(("+++ H265_ENT: H265_If_Record_Notify_Video_Stream Error.ret=%d\n",ret ));
		
					// Set Record End Status
					gH265_Enc_Ctrl.task_result = H265_ERR_SYSTEM;
					h265_encode_illagal();
		
					return;
				}
			} else {
				memcpy(&g_video_stream_info[ stream_ch ], &video_stream_info, sizeof(video_stream_info));
			}
		} else {
			ret = H265_If_Record_Notify_Video_Stream( stream_ch, &video_stream_info );
			if( !ret ){
				H265_Err_Print(("+++ H265_ENT: H265_If_Record_Notify_Video_Stream Error.ret=%d\n",ret ));
			
				// Set Record End Status
				gH265_Enc_Ctrl.task_result = H265_ERR_SYSTEM;
				h265_encode_illagal();
		
				return;
			}
		}

		// Check End of All Video Stream
		if( (gH265_Rec_Param.codec_param[E_H265_IF_STREAM_CH_0].enable_flg == H265_OFF) &&
			(gH265_Rec_Param.codec_param[E_H265_IF_STREAM_CH_1].enable_flg == H265_OFF) ){
			h265_encode_free_vstream();
			H265_Info_Print(("+++ ENT Task: All codecs are stopped.\n"));
			break;
		}
	}
#ifndef CO_DEBUG_ON_PC
	if (flag_first_proc == H265_OFF){
		// Free all video stream
		OS_User_Twai_Flg( FID_H265, FLG_H265_ENC_FREE_ALL_VSTREAM, D_OS_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_FOREVER );
		OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_FREE_ALL_VSTREAM );
	}
#endif
	
	gH265_Enc_Ctrl.task_result = H265_OK_NORMAL_END;
	
	waiptn = FLG_H265_TASK_CC_STOP;
	ercd = OS_User_Twai_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ANDW, &flgptn, 1000 );
	if( ercd != D_OS_USER_E_OK ){
		H265_Err_Print(("+++ ENT Task End: Time out ercd=%x\n", ercd ));
	}
	
	// ENT Task End Process
	h265_encode_result_notify( gH265_Enc_Ctrl.task_result );
	H265_Wave_Finalize();
	
	H265_Info_Print(("+++ H265_ENT: ENT Task End Complete!!\n"));
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_ENT" ));
	
	// 4. Set ENT Task Stop Event 
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_ENT_STOP );
	OS_User_Set_Flg( FID_H265, FLG_H265_STOP_ENT_TSK );
		
	msg.type = E_H265_IF_TYPE_NO_REQUEST;
	ret = H265_If_Set_Msg( &msg, E_H265_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		H265_Err_Print(("--- ENT End Msg ERR. ret=%x\n", ret ));
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
	
	return;
}

/**
 *	@brief		H265 Encode Initilize control data.\n
 *				And then this method will check media size.\n
 *				1) Global Data\n
 *				2) SDRAM\n
 *				3) MEDIA vacant area check.\n
 *	@param		record_info						Record Condition\n
 *	@return		H265_OK						(0) Normal End
 *	@return 	H265_ERR_MEDIA_AREA_NOTHING	(2) MEDIA Size Error
 *	@note		None
 *	@attention	None
 */
INT32 H265_Encode_Initialize( E_H265_IF_STREAM_CH stream_ch, T_H265_MOVIE_UI* record_info )
{
	INT32	ret;
	
	if( record_info == NULL ){
		return H265_ERR_SYSTEM;
	}
	
	if( gH265_Enc_Ctrl.rec_status != H265_ENC_RECORDING ){
// --- 0. Initialize Event Flag ---
		OS_User_Clr_Flg( FID_H265, 0x0000 );
		OS_User_Clr_Flg( FID_H265_TASK, 0x0000 );
		OS_User_Clr_Flg( FID_H265_SYNC, 0x0000 );
		
// --- 1. Initialize Global Data ---
		// gH265_Rec_Param
		gH265_Rec_Param = *record_info;
		
		// gH265_Enc_Ctrl
		gH265_Enc_Ctrl.task_result = H265_OK;
		
		// Global Reset
		gH265_Enc_Serial_Num[E_H265_IF_STREAM_CH_0] = 0;
		gH265_Enc_Serial_Num[E_H265_IF_STREAM_CH_1] = 0;
		gH265_Vstrem_Serial_Num[E_H265_IF_STREAM_CH_0] = 0;
		gH265_Vstrem_Serial_Num[E_H265_IF_STREAM_CH_1] = 0;
		
		gH265_ViewLater_Count = 0;
		gH265_MovieLater_Count = 0;
		gH265_ViewLater_Total_Count = 0;
		gH265_MovieLater_Total_Count = 0;
		gH265_Free_Vstream_End_Flg[E_H265_IF_STREAM_CH_0] = H265_ON;
		gH265_Free_Vstream_End_Flg[E_H265_IF_STREAM_CH_1] = H265_ON;
		gH265_Free_Vstream_Count = 0;
		
		H265_Movie_Memset( (VOID*)gH265_Vstrm_Fifo, 0, sizeof(gH265_Vstrm_Fifo) );
		
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_0] = H265_ON;
		gH265_Sirius_Stop_Flg[E_H265_IF_STREAM_CH_1] = H265_ON;
		gH265_Sirius_Stop_Done_Wiat_Ptn = 0;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---	
		
		H265_Movie_Memset( (VOID*)&gH265_Free_Vstream_Index[0], 0, sizeof(gH265_Free_Vstream_Index) );
		H265_Movie_Memset( (VOID*)&gH265_Free_Vstream_Ctrl[0], 0, sizeof(gH265_Free_Vstream_Ctrl) );
		
		gH265_Dynamic_Change_Flag[E_H265_IF_STREAM_CH_0] = H265_OFF;
		gH265_Dynamic_Change_Flag[E_H265_IF_STREAM_CH_1] = H265_OFF;
		gH265_Insert_IDR_Flag[E_H265_IF_STREAM_CH_0] = H265_OFF;
		gH265_Insert_IDR_Flag[E_H265_IF_STREAM_CH_1] = H265_OFF;
		gH265_GOP_Change_Flag[E_H265_IF_STREAM_CH_0] = H265_OFF;
		gH265_GOP_Change_Flag[E_H265_IF_STREAM_CH_1] = H265_OFF;
		gH265_Sync_Intterupt_Count[E_H265_IF_STREAM_CH_0] = 0;
		gH265_Sync_Intterupt_Count[E_H265_IF_STREAM_CH_1] = 0;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		H265_Movie_Memset( (VOID*)gH265_Codec_Output_NAL_Info, 0, sizeof(gH265_Codec_Output_NAL_Info) );
		gH265_Codec_Output_NAL_Info_Sei_Pointer[E_H265_IF_STREAM_CH_0] = 0;
		gH265_Codec_Output_NAL_Info_Sei_Pointer[E_H265_IF_STREAM_CH_1] = 0;
		gH265_Codec_End_Flg[E_H265_IF_STREAM_CH_0] = H265_ON;
		gH265_Codec_End_Flg[E_H265_IF_STREAM_CH_1] = H265_ON;
		gH265_Codec_Enc_Core_Idx[0] = 0;
		gH265_Codec_Enc_Core_Idx[1] = 0;
		gH265_Codec_Ready_To_Pallalel[0] = FALSE;
		gH265_Codec_Ready_To_Pallalel[1] = FALSE;
		
		gH265_Image_Get_Count[E_H265_IF_STREAM_CH_0] = 0;
		gH265_Image_Get_Count[E_H265_IF_STREAM_CH_1] = 0;
		gH265_Image_Store_Count[E_H265_IF_STREAM_CH_0] = 0;
		gH265_Image_Store_Count[E_H265_IF_STREAM_CH_1] = 0;
		
// --- REMOVE_ES3_HARDWARE END ---	
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
	}
	else{
		gH265_Rec_Param.codec_param[stream_ch] = record_info->codec_param[stream_ch];
		gH265_Sync_Intterupt_Count[stream_ch] = 0;
		
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		gH265_Codec_Enc_Core_Idx[stream_ch] = 0;
		gH265_Codec_Ready_To_Pallalel[stream_ch] = FALSE;
// --- REMOVE_ES3_HARDWARE END ---	
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
	}
	
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	H265_Sirius_Enc_Fm_Empty_Init( stream_ch );
	
	H265_Movie_Memset( (VOID*)&gH265_Fm_Table_Use_Flag[stream_ch][0], 0, sizeof(UINT32)*ENC_FM_NUM );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	H265_Movie_Memset( (VOID*)&gH265_Codec_Handle[stream_ch], 0, sizeof(EncHandle) );
	H265_Movie_Memset( (VOID*)&gH265_Codec_Header_Info[stream_ch], 0, sizeof(T_H265_ENC_HEADER_INFO) );
	H265_Movie_Memset( (VOID*)&gH265_Codec_Output_Info[stream_ch][0], 0, sizeof(EncOutputInfo)*D_H265_ENCODE_VINFO_FIFO_MAX );
	H265_Movie_Memset( (VOID*)&gH265_YC_Buffer[stream_ch][0], 0, sizeof(FrameBuffer)*D_WAVE_YC_BANK_MAX );
	
	gH265_Codec_Already_Stop_Req_Flag[stream_ch] = FALSE;
	
	gH265_Codec_End_Flg[stream_ch] = H265_OFF;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---	
	
	// video
	// Set Video track information
	ret = H265_Movie_Convert_Framerate( gH265_Rec_Param.codec_param[stream_ch].frame_rate, (ULONG*)&gH265_Enc_Ctrl.video[stream_ch].time_increment, (ULONG*)&gH265_Enc_Ctrl.video[stream_ch].time_scale );
	if( ret != H265_OK ){
		H265_Err_Print(("+++ H265_Movie_Convert_Framerate: NG!!\n"));
		return H265_ERR_INPUT_PARAM;
	}	
	gH265_Enc_Ctrl.video[stream_ch].total_time_increment	= 0;
	
	gH265_Free_Vstream_End_Flg[stream_ch] = H265_OFF;
	
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	gH265_Sirius_Stop_Flg[stream_ch] = H265_OFF;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
	
	return H265_OK;
}

/**
 *	@brief		Get Image Setting Address.\n
  *	@param		i:stream_ch		stream channel\n
  *	@param		i:size			stream size\n
 *	@param		i:frame_no		frame number\n
 *	@param		o:y_addr		Y    Address\n
 *	@param		o:c_addr		CbCr Address\n
 *	@param		o:g_width		Global Width\n
 *	@param		o:dummy_flg		Dummy bank use flag\n
 *	@return		H265_OK					(0) Normal End
 *	@return		H265_ERR_INPUT_PARAM	(1) Input Parameter Error
 *	@return		H265_ERR_SYSTEM			(99) System Error
 *	@note		None
 *	@attention	None
 */
INT32 H265_Encode_Image_Address_Get( E_H265_IF_STREAM_CH stream_ch, E_H265_IF_IMAGE_SIZE size, ULLONG frame_no, T_H265_IF_GET_IMAGE_ADDR_PARAM* addr_param )
{
	INT32 ret;
	
	addr_param->dummy_flg = FALSE;
	
	if( gH265_Map_Enable_Flg[stream_ch] != H265_ON ){
		Sdram_Map_Movie_Record_Get_H265_Vin_Dummy( stream_ch, size, addr_param );
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-%u-%u Vget", stream_ch, (UINT32)frame_no, addr_param->dummy_flg ));
		return H265_OK;
	}
	if (gH265_Map_First_Flg[stream_ch] == H265_ON) {
		gH265_Map_First_Flg[stream_ch] = H265_OFF;
		gH265_First_FrameNo = frame_no;
	}
	ret = h265_encode_Vin_get_address( stream_ch, frame_no, size, addr_param );
	if( ret != H265_OK ){
		addr_param->dummy_flg = TRUE;
	}
	
	gH265_Image_Get_Count[stream_ch]++;
	
	if( gH265_Image_Get_Count[stream_ch] >= D_H265_ENCODE_ONESHOT_ENC_NUM ){
		
		if( gH265_Rec_Param.codec_param[stream_ch].oneshot_encode_flag == H265_ON ){
			gH265_Map_Enable_Flg[stream_ch]			= H265_OFF;
			gH265_Map_First_Flg[stream_ch]			= H265_ON;
			H265_Info_Print(("[ch=%d] gH265_Map_Enable_Flg is OFF!!\n", stream_ch));
		}
	}
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-%u-%u Vget", stream_ch, (UINT32)frame_no, addr_param->dummy_flg ));
	
	return H265_OK;
}

/**
 *	@brief		Decide Image Setting Address.\n
  *	@param		i:stream_ch		stream channel\n
 *	@param		i:frame_no		frame number\n
 *	@param		i:dummy_flg		dummy bank use flag\n
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Encode_Image_Address_Decide( E_H265_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg )
{
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-%u-%u Vdec", stream_ch, (UINT32)frame_no, dummy_flg ));
	if( !dummy_flg ){
		h265_encode_Vin_decide_address( stream_ch, frame_no );
		
		gH265_Image_Store_Count[stream_ch]++;
		
		if( gH265_Image_Store_Count[stream_ch] >= D_H265_ENCODE_ONESHOT_ENC_NUM ){
			
			if( gH265_Rec_Param.codec_param[stream_ch].oneshot_encode_flag == H265_ON ){
				
				if( gH265_Rec_Param.codec_param[stream_ch].enable_flg == H265_ON ){
					H265_Info_Print(("ch[%d] : Start auto stop!!\n", stream_ch ));
					OS_User_Set_Flg( FID_H265_SYNC, gH265_Stream_Stop_FlgID[stream_ch] );
				}
				else{
					H265_Err_Print(("ch[%d] : It can not pass this route!!\n", stream_ch ));
				}
			}
		}
	}
	
	return;
}

/**
 *	@brief		Decide Image Setting Address.\n
  *	@param		i:stream_ch		stream channel\n
 *	@param		i:enabled		enabled flag\n
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Encode_Frame_Enabled( E_H265_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg, BOOL enabled )
{
	INT32			ret = 0;
	INT32			erace_bank;
	ULLONG			erace_frame_no;
	E_VIDEO_ENOCDE_ID video_enc_id;
	
	// If timelapse flag off, nothing special.
	BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H265( stream_ch, &video_enc_id );
	if( (BF_Movie_Video_Attr_Get_Timelapse_Flag(video_enc_id) != H265_OFF) && (gH265_Map_Enable_Flg[stream_ch] == H265_ON) ){
		if( !dummy_flg ){
			if( enabled ){
				// TRUE
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-%u-%u Venb", stream_ch, (UINT32)frame_no, dummy_flg ));
				H265_Encode_Sync_Interrupt_Count_Increment( stream_ch );
				OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_ENC_MASTER );
				OS_User_Set_Flg( FID_H265_SYNC, gH265_Stream_TimeLapse_Sync_FlgID[stream_ch] );
			}
			else{
				// FALSE
				// Free to unnecessary Bank 
				OS_User_Wai_Sem(SID_H265);
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-%u-%u Vfal", stream_ch, (UINT32)frame_no, dummy_flg ));
				ret = h265_encode_Vin_get_decide_bank( stream_ch, &erace_bank, &erace_frame_no );
				if( ret == H265_ENC_VIN_CTL_NONEDATA ){
					H265_Err_Print(( "Enabled frame nondata\n" ));
					OS_User_Sig_Sem(SID_H265);
					return;
				}
				h265_encode_Vin_set_empty_bank( stream_ch, erace_bank );
				OS_User_Sig_Sem(SID_H265);
			}
		}
	}
	return;
}


/**
 *  @brief		Finalize Vin Address Ctl Data.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Encode_Finalize( VOID )
{
	gH265_Map_Enable_Flg[E_H265_IF_STREAM_CH_0] = H265_OFF;
	gH265_Map_Enable_Flg[E_H265_IF_STREAM_CH_1] = H265_OFF;
	gH265_Map_First_Flg[E_H265_IF_STREAM_CH_0] = H265_ON;
	gH265_Map_First_Flg[E_H265_IF_STREAM_CH_1] = H265_ON;
	gH265_Enc_Ctrl.rec_status = H265_ENC_STOP_END;
	return;
}

/**
 *  @brief		Free video stream.
 *	@param		i:vstream_free_info		Free video stream information
 *	@return		H265_OK	(0)Normal End
 *	@return		H265_NG	(1)Error
 *	@note		None
 *	@attention	None
 */
INT32 H265_Encode_Free_Video_Index( E_H265_IF_STREAM_CH stream_ch, UINT32 free_vstream_index, BOOL last_free_flg )
{
	OS_USER_FLGPTN		flgptn ;
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "free %d", gH265_Free_Vstream_Ctrl[free_vstream_index].free_size ));

	OS_User_Wai_Sem(SID_H265_STREAM);
	gH265_Free_Vstream_Index[gH265_Free_Vstream_Count] = free_vstream_index;
	
	if( last_free_flg == H265_ON ){
		gH265_Free_Vstream_End_Flg[stream_ch] = H265_ON;
	}
	gH265_Free_Vstream_Count++;
	if( gH265_Free_Vstream_Count >= D_H265_ENC_STREAM_FIFO_COUNT ){
		H265_Err_Print(("--- H265 VSTRM free table overflow\n" ));
	}
	OS_User_Sig_Sem(SID_H265_STREAM);
	
	if( (gH265_Free_Vstream_End_Flg[E_H265_IF_STREAM_CH_0] == H265_ON) &&
		(gH265_Free_Vstream_End_Flg[E_H265_IF_STREAM_CH_1] == H265_ON) ){
		// All VSTRM Table Free Complete 
		OS_User_Set_Flg( FID_H265, FLG_H265_ENC_FREE_ALL_VSTREAM );
		
		// wait all task stop
		OS_User_Wai_Flg( FID_H265_TASK, FLG_H265_TASK_CONTROLLER_STOP, D_OS_USER_TWF_ORW, &flgptn );
		H265_If_Record_Notify_STOP_COMPLETE();
	}
	
	return H265_OK;
}

/**
 *  @brief		Interrupt Counter Increment
 *	@param		stream_ch
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Encode_Sync_Interrupt_Count_Increment( E_H265_IF_STREAM_CH stream_ch )
{
	UINT32				di_status;
	
	di_status = H265_Movie_DI();
	gH265_Sync_Intterupt_Count[stream_ch]++;
	H265_Movie_EI(di_status);
	return;
}


/**
 *	@brief		Encode Task Create.\n
 *	@param		None
 *	@return		H265_OK				(0)  Normal End
 *	@return 	H265_ERR_SYSTEM		(99) It Cannot create task
 *	@note		None
 *	@attention	None
 */
static INT32 h265_encode_task_create( VOID )
{
	ULONG				frameCnt = 0;
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn;
	OS_USER_ER			ercd;
	
	if( gH265_Enc_Ctrl.rec_status != H265_ENC_RECORDING ){
		// Video CC Task (create file)
		h265_encode_task_start( TID_H265_CC );
		// Video ENT Task
		h265_encode_task_start( TID_H265_ENT );
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		// Video ENT INT Task
		h265_encode_task_start( TID_H265_ENT_INT );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
		
#ifdef CO_H265_ENC_DEBUG_VIEW_DUMMY
		// Video DISP Task (Not used on Record Mode)
		h265_encode_task_start( TID_H265_DISP );
#endif
		// Confirm All of Task Status
		waiptn = FLG_H265_TASK_ENT_INIT_END | FLG_H265_TASK_CC_INIT_END;
		frameCnt = 0;
		
		while(1){
			OS_User_Tslp_Tsk( 5 );
			// Check Task Status
			ercd = OS_User_Pol_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
			if( ercd == D_OS_USER_E_OK ) {
				break;
			}
			else if( ercd == D_OS_USER_E_TMOUT ) {
				if( frameCnt > 400 ){
					H265_Err_Print(("--- It cannot change task status!! flgptn=%lx\n", flgptn ));
					
					h265_encode_task_destroy();
					
					return H265_ERR_SYSTEM;
				}
			}
			else{
				H265_Err_Print(("--- Wait task start. pol_flg ercd=%x\n", ercd ));
				
				h265_encode_task_destroy();
				
				return H265_ERR_SYSTEM;
			}
			frameCnt++;
		}
		
		// Check Task Result Status before Recording
		waiptn = FLG_H265_TASK_ENT_STOP | FLG_H265_TASK_CC_STOP;
		
		ercd = OS_User_Pol_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			H265_Err_Print(("--- CAN'T RECORD by TASK. flg=%lx\n", flgptn ));
			
			h265_encode_task_destroy();
			
			return H265_ERR_SYSTEM;
		}
	}
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	else{
		if( gH265_Rec_Param.codec_param[E_H265_IF_STREAM_CH_1].enable_flg == H265_ON ){
			// Video CC2 Task
			h265_encode_task_start( TID_H265_CC_2 );
			
			waiptn = FLG_H265_TASK_CC_2_INIT_END;
			frameCnt = 0;
			
			while(1){
				OS_User_Tslp_Tsk( 5 );
				// Check Task Status
				ercd = OS_User_Pol_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
				if( ercd == D_OS_USER_E_OK ) {
					break;
				}
				else if( ercd == D_OS_USER_E_TMOUT ) {
					if( frameCnt > 400 ){
						H265_Err_Print(("--- It cannot change task status!! flgptn=%lx\n", flgptn ));
						
						h265_encode_task_destroy();
						
						return H265_ERR_SYSTEM;
					}
				}
				else{
					H265_Err_Print(("--- Wait task start. pol_flg ercd=%x\n", ercd ));
					
					h265_encode_task_destroy();
					
					return H265_ERR_SYSTEM;
				}
				frameCnt++;
			}
			
			// Check Task Result Status before Recording
			waiptn = FLG_H265_TASK_CC_2_STOP;
			
			ercd = OS_User_Pol_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			if( ercd == D_OS_USER_E_OK ) {
				H265_Err_Print(("--- CAN'T RECORD by TASK. flg=%lx\n", flgptn ));
				
				h265_encode_task_destroy();
				
				return H265_ERR_SYSTEM;
			}
		}
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
	
	return H265_OK;
}


/**
 *	@brief		Encode Task Create.\n
 *	@param		None
 *	@return		H265_OK				(0)  Normal End
 *	@return 	H265_ERR_SYSTEM		(99) It Cannot create task
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_task_destroy( VOID )
{
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	OS_User_Ter_Tsk( TID_H265_ENT_INT );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
	OS_User_Ter_Tsk( TID_H265_ENT );
	OS_User_Ter_Tsk( TID_H265_CC );
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	if( gH265_Rec_Param.codec_param[E_H265_IF_STREAM_CH_1].enable_flg == H265_ON ){
		OS_User_Ter_Tsk( TID_H265_CC_2 );
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
#ifdef CO_H265_ENC_DEBUG_VIEW_DUMMY
	OS_User_Ter_Tsk( TID_H265_DISP );
#endif
	
	return;
}

/**
 *	@brief		Initialize codec.\n
 *	@param		None
 *	@return		H265_OK					(0) Normal End
 *	@return		H265_ERR_MACRO			(5) Macro Error
 *	@note		None
 *	@attention	None
 */
static INT32 h265_encode_init_codec( VOID )
{
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	INT32 ret;
	INT32 i;
	T_SR_BRIDGE_MAP_PARAM bridge_map;
	T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_REMAP_CONFIG remap_config;
	T_SDRAM_MAP_MOVIE_RECORD_SIRIUS map_sirius;
	
	H265_Movie_Memset( &bridge_map, 0, sizeof(bridge_map) );
	
	// SDRAM Infomation
	Sdram_Map_Movie_Record_Get_H265_Remap_Config( &remap_config );
	
	bridge_map.ddr_config.ddr_ch0_top_addr			= SDRAM_ADR_CH0_TOP;
	bridge_map.ddr_config.ddr_ch1_top_addr			= SDRAM_ADR_CH1_TOP;
	bridge_map.ddr_config.ddr_ch0_top_common_addr	= remap_config.remap_ch0_top;
	bridge_map.ddr_config.ddr_ch0_common_size		= remap_config.remap_ch0_size;
	bridge_map.ddr_config.ddr_ch1_top_common_addr	= remap_config.remap_ch1_top;
	bridge_map.ddr_config.ddr_ch1_common_size		= remap_config.remap_ch1_size;
	
	
	//******  Stream CH_0 Setting  ******//
	Sdram_Map_Movie_Record_Get_H265_Map( E_H265_IF_STREAM_CH_0, &map_sirius );
	
	for( i=0; i<D_SIRIUS_YC_BANK_MAX; i++ ){
		// bufy0
		bridge_map.map_enc[0].encbufy[i].start_addr	= map_sirius.yc.addr[i].y;
		bridge_map.map_enc[0].encbufy[i].end_addr	= map_sirius.yc.addr[i].y + map_sirius.yc.y_bank;
		
		// bufc0
		bridge_map.map_enc[0].encbufc[i].start_addr	= map_sirius.yc.addr[i].c;
		bridge_map.map_enc[0].encbufc[i].end_addr	= map_sirius.yc.addr[i].c + map_sirius.yc.c_bank;
	}
	
	// dpb0
	bridge_map.map_enc[0].dpb.start_addr			= map_sirius.dpb.start_addr;
	bridge_map.map_enc[0].dpb.end_addr				= map_sirius.dpb.end_addr;
	
	// veb0
	bridge_map.map_enc[0].veb.start_addr			= map_sirius.veb.start_addr;
	bridge_map.map_enc[0].veb.end_addr				= map_sirius.veb.end_addr;
	
	// encbuf_cmprs0
	bridge_map.map_enc[0].encbuf_cmprs.start_addr	= map_sirius.cmprs.start_addr;
	bridge_map.map_enc[0].encbuf_cmprs.end_addr		= map_sirius.cmprs.end_addr;
	
	// dpb_cmprs0
	bridge_map.map_enc[0].dpb_cmprs.start_addr		= map_sirius.dpb_cmprs.start_addr;
	bridge_map.map_enc[0].dpb_cmprs.end_addr		= map_sirius.dpb_cmprs.end_addr;
	
	// encbuf_cmprs_mode0
	if( gH265_Rec_Param.codec_param[0].compression_enable == H265_ON ){
		bridge_map.map_enc[0].encbuf_cmprs_mode		= 1;	// for LossLess Compression
	}
	else{
		bridge_map.map_enc[0].encbuf_cmprs_mode		= 0;	// for Through
	}
	
	// dpb_cmprs_mode0
	if( gH265_Rec_Param.codec_param[0].compression_enable == H265_ON ){
		bridge_map.map_enc[0].dpb_cmprs_mode		= 1;	// for LossLess Compression
	}
	else{
		bridge_map.map_enc[0].dpb_cmprs_mode		= 0;	// for Through
	}
	
	// map_mode0
	if( map_sirius.map_mode != 0xFF ){
		bridge_map.map_enc[0].encode_mode				= map_sirius.map_mode;
	}
	else{
		bridge_map.map_enc[0].frame_width				= map_sirius.yc.g_width;
		bridge_map.map_enc[0].frame_height				= map_sirius.yc.g_lines;
	}
	
	
	//******  Stream CH_1 Setting  ******//
	Sdram_Map_Movie_Record_Get_H265_Map( E_H265_IF_STREAM_CH_1, &map_sirius );

	for( i=0; i<D_SIRIUS_YC_BANK_MAX; i++ ){
		// bufy1
		bridge_map.map_enc[1].encbufy[i].start_addr	= map_sirius.yc.addr[i].y;
		bridge_map.map_enc[1].encbufy[i].end_addr	= map_sirius.yc.addr[i].y + map_sirius.yc.y_bank;
		
		// bufc1
		bridge_map.map_enc[1].encbufc[i].start_addr	= map_sirius.yc.addr[i].c;
		bridge_map.map_enc[1].encbufc[i].end_addr	= map_sirius.yc.addr[i].c + map_sirius.yc.c_bank;
	}
	
	// dpb1
	bridge_map.map_enc[1].dpb.start_addr			= map_sirius.dpb.start_addr;
	bridge_map.map_enc[1].dpb.end_addr				= map_sirius.dpb.end_addr;
	
	// veb1
	bridge_map.map_enc[1].veb.start_addr			= map_sirius.veb.start_addr;
	bridge_map.map_enc[1].veb.end_addr				= map_sirius.veb.end_addr;
	
	// encbuf_cmprs1
	bridge_map.map_enc[1].encbuf_cmprs.start_addr	= map_sirius.cmprs.start_addr;
	bridge_map.map_enc[1].encbuf_cmprs.end_addr		= map_sirius.cmprs.end_addr;
	
	// dpb_cmprs1
	bridge_map.map_enc[1].dpb_cmprs.start_addr		= map_sirius.dpb_cmprs.start_addr;
	bridge_map.map_enc[1].dpb_cmprs.end_addr		= map_sirius.dpb_cmprs.end_addr;
	
	// encbuf_cmprs_mode1
	if( gH265_Rec_Param.codec_param[1].compression_enable == H265_ON ){
		bridge_map.map_enc[1].encbuf_cmprs_mode		= 1;	// for LossLess Compression
	}
	else{
		bridge_map.map_enc[1].encbuf_cmprs_mode		= 0;	// for Through
	}
	
	// dpb_cmprs_mode1
	if( gH265_Rec_Param.codec_param[1].compression_enable == H265_ON ){
		bridge_map.map_enc[1].dpb_cmprs_mode		= 1;	// for LossLess Compression
	}
	else{
		bridge_map.map_enc[1].dpb_cmprs_mode		= 0;	// for Through
	}
	
	// map_mode1
	if( map_sirius.map_mode != 0xFF ){
		bridge_map.map_enc[1].encode_mode				= map_sirius.map_mode;
	}
	else{
		bridge_map.map_enc[1].frame_width				= map_sirius.yc.g_width;
		bridge_map.map_enc[1].frame_height				= map_sirius.yc.g_lines;
	}
	
	// decode area (dummy)
	bridge_map.map_dec[0].param.start_addr			= Sdram_Map_Movie_Record_Get_H265_Work();
	bridge_map.map_dec[0].param.end_addr			= Sdram_Map_Movie_Record_Get_H265_Work() + 1*1024*1024;	// 1MByte
	bridge_map.map_dec[0].colpic.start_addr			= Sdram_Map_Movie_Record_Get_H265_Work();
	bridge_map.map_dec[0].colpic.end_addr			= Sdram_Map_Movie_Record_Get_H265_Work() + 1*1024*1024;	// 1MByte
	bridge_map.map_dec[0].vbb.start_addr			= Sdram_Map_Movie_Record_Get_H265_Work();
	bridge_map.map_dec[0].vbb.end_addr				= Sdram_Map_Movie_Record_Get_H265_Work() + 1*1024*1024;	// 1MByte
#ifndef CO_DEBUG_ON_PC
	bridge_map.map_dec[0].fm_y.start_addr			= Sdram_Map_Movie_Record_Get_H265_Work();
	bridge_map.map_dec[0].fm_y.end_addr				= Sdram_Map_Movie_Record_Get_H265_Work() + 1*1024*1024;	// 1MByte
	bridge_map.map_dec[0].fm_c.start_addr			= Sdram_Map_Movie_Record_Get_H265_Work();
	bridge_map.map_dec[0].fm_c.end_addr				= Sdram_Map_Movie_Record_Get_H265_Work() + 1*1024*1024;	// 1MByte
#else
	bridge_map.map_dec[0].fm_y.start_addr			= Sdram_Map_Movie_Record_Get_H265_Work();
	bridge_map.map_dec[0].fm_y.end_addr				= Sdram_Map_Movie_Record_Get_H265_Work() + 0x1144000*12;	// 1MByte
	bridge_map.map_dec[0].fm_c.start_addr			= Sdram_Map_Movie_Record_Get_H265_Work();
	bridge_map.map_dec[0].fm_c.end_addr				= Sdram_Map_Movie_Record_Get_H265_Work() + 0x8A4000*12;	// 1MByte
#endif
	bridge_map.map_dec[0].decode_mode				= SR_DECODE_MODE_0;
	
	ret = H265_Sirius_Initialize( H265_MODE_RECORD, (T_SR_BRIDGE_MAP_PARAM *)&bridge_map );
	if( ret != H265_OK ){
		H265_Err_Print(( "h265_encode_init_codec: H265_Sirius_Initialize NG\n" ));
		return H265_NG;
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	INT32 ret;
	T_SDRAM_MAP_MOVIE_RECORD_WAVE map_wave;
	T_H265_WAVE_CODE code;
	
	Sdram_Map_Movie_Record_Get_H265_Map( E_H265_IF_STREAM_CH_0, &map_wave );
	
	// Code Area & Stream Buffer & VUI, NAL, HEADER, SEI
	if( H265_Movie_Sdram_Ch_Check( map_wave.stream.addr[0] ) == SDRAM_ADR_CH0_TOP ){
		gH265_Codec_Ddr_Ch_Select.vCPUCh = DDR_CHANNEL_0;
		gH265_Codec_Ddr_Ch_Select.vCoreCh = DDR_CHANNEL_0;
	}
	else{
		gH265_Codec_Ddr_Ch_Select.vCPUCh = DDR_CHANNEL_1;
		gH265_Codec_Ddr_Ch_Select.vCoreCh = DDR_CHANNEL_1;
	}
	if( H265_Movie_Sdram_Ch_Check( map_wave.nal_report[0].addr ) == SDRAM_ADR_CH0_TOP ){
		if( gH265_Codec_Ddr_Ch_Select.vCoreCh != DDR_CHANNEL_0 ){
			H265_Err_Print(( "Map Error! : NAL report and stream need same CH!\n" ));
		}
	}
	else{
		if( gH265_Codec_Ddr_Ch_Select.vCoreCh != DDR_CHANNEL_1 ){
			H265_Err_Print(( "Map Error! : NAL report and stream need same CH!\n" ));
		}
	}
	if( H265_Movie_Sdram_Ch_Check( map_wave.sei[0].addr ) == SDRAM_ADR_CH0_TOP ){
		if( gH265_Codec_Ddr_Ch_Select.vCoreCh != DDR_CHANNEL_0 ){
			H265_Err_Print(( "Map Error! : SEI and stream need same CH!\n" ));
		}
	}
	else{
		if( gH265_Codec_Ddr_Ch_Select.vCoreCh != DDR_CHANNEL_1 ){
			H265_Err_Print(( "Map Error! : SEI and stream need same CH!\n" ));
		}
	}
	if( H265_Movie_Sdram_Ch_Check( map_wave.vui.addr ) == SDRAM_ADR_CH0_TOP ){
		if( gH265_Codec_Ddr_Ch_Select.vCoreCh != DDR_CHANNEL_0 ){
			H265_Err_Print(( "Map Error! : VUI and stream need same CH!\n" ));
		}
	}
	else{
		if( gH265_Codec_Ddr_Ch_Select.vCoreCh != DDR_CHANNEL_1 ){
			H265_Err_Print(( "Map Error! : VUI and stream need same CH!\n" ));
		}
	}
	if( H265_Movie_Sdram_Ch_Check( map_wave.header.addr ) == SDRAM_ADR_CH0_TOP ){
		if( gH265_Codec_Ddr_Ch_Select.vCoreCh != DDR_CHANNEL_0 ){
			H265_Err_Print(( "Map Error! : HEADER and stream need same CH!\n" ));
		}
	}
	else{
		if( gH265_Codec_Ddr_Ch_Select.vCoreCh != DDR_CHANNEL_1 ){
			H265_Err_Print(( "Map Error! : HEADER and stream need same CH!\n" ));
		}
	}
	
	// Src Buffer
	if( H265_Movie_Sdram_Ch_Check( map_wave.yc.addr[0].y ) == SDRAM_ADR_CH0_TOP ){
		gH265_Codec_Ddr_Ch_Select.fcPrpCh = DDR_CHANNEL_0;
	}
	else{
		gH265_Codec_Ddr_Ch_Select.fcPrpCh = DDR_CHANNEL_1;
	}
	if( H265_Movie_Sdram_Ch_Check( map_wave.yc.addr[0].c ) == SDRAM_ADR_CH0_TOP ){
		if( gH265_Codec_Ddr_Ch_Select.fcPrpCh != DDR_CHANNEL_0 ){
			H265_Err_Print(( "Map Error! : Y and C need same CH!\n" ));
		}
	}
	else{
		if( gH265_Codec_Ddr_Ch_Select.fcPrpCh != DDR_CHANNEL_1 ){
			H265_Err_Print(( "Map Error! : Y and C need same CH!\n" ));
		}
	}
	if( H265_Movie_Sdram_Ch_Check( map_wave.cmprs.addr[0].y ) == SDRAM_ADR_CH0_TOP ){
		if( gH265_Codec_Ddr_Ch_Select.fcPrpCh != DDR_CHANNEL_0 ){
			H265_Err_Print(( "Map Error! : YC and Comp need same CH!\n" ));
		}
	}
	else{
		if( gH265_Codec_Ddr_Ch_Select.fcPrpCh != DDR_CHANNEL_1 ){
			H265_Err_Print(( "Map Error! : YC and Comp need same CH!\n" ));
		}
	}
	// LocalDec
	if( H265_Movie_Sdram_Ch_Check( map_wave.localdec.addr[0] ) == SDRAM_ADR_CH0_TOP ){
		gH265_Codec_Ddr_Ch_Select.fbcCh = DDR_CHANNEL_0;
	}
	else{
		gH265_Codec_Ddr_Ch_Select.fbcCh = DDR_CHANNEL_1;
	}
	
	H265_Wave_SetDDRChannel( gH265_Codec_Ddr_Ch_Select );
	
	Sdram_Map_Movie_Record_Get_H265_Code( &code.addr_ch0, &code.size_ch0, &code.addr_ch1, &code.size_ch1 );
	
	// wave boot
	ret = H265_Wave_Initialize( H265_MODE_RECORD, 2, code ); // 2 is wave core number. fixed
	if( ret == H265_OK ){
		;
	}
	else{
		H265_Err_Print(( "h265_encode_wave_init: H265_Wave_Initialize NG\n" ));
	}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---

	// Vin Initialize
	h265_encode_Vin_initialize();
	
	return H265_OK;
}

/**
 *	@brief		Initialize codec Parameter.\n
 *	@param		None
 *	@return		H265_OK					(0) Normal End
 *	@return		H265_ERR_MACRO			(5) Macro Error
 *	@note		None
 *	@attention	None
 */
static INT32 h265_encode_set_param_codec( E_H265_IF_STREAM_CH stream_ch )
{
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	INT32		ret;
	T_SR_ENC_INITPARAM param;
	T_SR_ENC_FM_TABLE fmtable;
	
	H265_Movie_Memset( &fmtable, 0, sizeof(fmtable) );
	
	if( gH265_Rec_Param.codec_param[stream_ch].enable_flg == H265_ON ){
		// init param setting
		param.gopn				= gH265_Rec_Param.codec_param[stream_ch].gop_num;
		
		if( gH265_Rec_Param.codec_param[stream_ch].gop_struct == E_H265_IF_GOP_STR_IPPP ){
			param.gopm				= 1;
			param.encstart_delay	= 3;	// Actualy this parameter is 1, But need set 3 for Dynamic GOP struct change.
		}
		else{
			param.gopm				= 3;
			param.encstart_delay	= 3;
		}
		
		param.level				= gH265_Rec_Param.codec_param[stream_ch].level;
		param.venchsize			= gH265_Rec_Param.codec_param[stream_ch].width;
		param.vencvsize			= gH265_Rec_Param.codec_param[stream_ch].lines;
		
		param.videomaxbitrate	= gH265_Rec_Param.codec_param[stream_ch].bitrate / 1000;
		param.videoavrgbitrate	= gH265_Rec_Param.codec_param[stream_ch].average_bitrate / 1000;
		
		switch( gH265_Rec_Param.codec_param[stream_ch].frame_rate ){
			case E_H265_IF_FRAME_RATE_239_76 :
				param.frame_rate_numerator		= 240000;
				param.frame_rate_denominator	= 1001;
				break;
			case E_H265_IF_FRAME_RATE_119_88 :
				param.frame_rate_numerator		= 120000;
				param.frame_rate_denominator	= 1001;
				break;
			case E_H265_IF_FRAME_RATE_59_94 :
				param.frame_rate_numerator		= 60000;
				param.frame_rate_denominator	= 1001;
				break;
			case E_H265_IF_FRAME_RATE_29_97 :
				param.frame_rate_numerator		= 30000;
				param.frame_rate_denominator	= 1001;
				break;
			case E_H265_IF_FRAME_RATE_23_97 :
				param.frame_rate_numerator		= 24000;
				param.frame_rate_denominator	= 1001;
				break;
			case E_H265_IF_FRAME_RATE_14_98 :
				param.frame_rate_numerator		= 15000;
				param.frame_rate_denominator	= 1001;
				break;
			case E_H265_IF_FRAME_RATE_240_0 :
				param.frame_rate_numerator		= 240000;
				param.frame_rate_denominator	= 1000;
				break;
			case E_H265_IF_FRAME_RATE_120_0 :
				param.frame_rate_numerator		= 120000;
				param.frame_rate_denominator	= 1000;
				break;
			case E_H265_IF_FRAME_RATE_73_0 :
				param.frame_rate_numerator		= 73000;
				param.frame_rate_denominator	= 1000;
				break;
			case E_H265_IF_FRAME_RATE_72_0 :
				param.frame_rate_numerator		= 72000;
				param.frame_rate_denominator	= 1000;
				break;
			case E_H265_IF_FRAME_RATE_70_0 :
				param.frame_rate_numerator		= 70000;
				param.frame_rate_denominator	= 1000;
				break;
			case E_H265_IF_FRAME_RATE_60_0 :
				param.frame_rate_numerator		= 60000;
				param.frame_rate_denominator	= 1000;
				break;
			case E_H265_IF_FRAME_RATE_50_0 :
				param.frame_rate_numerator		= 50000;
				param.frame_rate_denominator	= 1000;
				break;
			case E_H265_IF_FRAME_RATE_30_0 :
				param.frame_rate_numerator		= 30000;
				param.frame_rate_denominator	= 1000;
				break;
			case E_H265_IF_FRAME_RATE_29_0 :
				param.frame_rate_numerator		= 29000;
				param.frame_rate_denominator	= 1000;
				break;
			case E_H265_IF_FRAME_RATE_25_0 :
				param.frame_rate_numerator		= 25000;
				param.frame_rate_denominator	= 1000;
				break;
			case E_H265_IF_FRAME_RATE_15_0 :
				param.frame_rate_numerator		= 15000;
				param.frame_rate_denominator	= 1000;
				break;
			default :
				H265_Err_Print(( "h265_encode_set_param_codec: FrameRate NG\n" ));
				return H265_NG;
		}
		
		// VUI setteing		
		param.video_signal_type_present_flag	= gH265_Rec_Param.codec_param[stream_ch].vui.video_signal_type_present_flag;
		param.video_format						= gH265_Rec_Param.codec_param[stream_ch].vui.video_format;
		param.video_full_range_flag				= gH265_Rec_Param.codec_param[stream_ch].vui.video_full_range_flag;
		param.colour_description_present_flag	= gH265_Rec_Param.codec_param[stream_ch].vui.colour_description_present_flag;
		param.colour_primaries					= gH265_Rec_Param.codec_param[stream_ch].vui.colour_primaries;
		param.transfer_characteristics			= gH265_Rec_Param.codec_param[stream_ch].vui.transfer_characteristics;
		param.matrix_coeffs						= gH265_Rec_Param.codec_param[stream_ch].vui.matrix_coefficients;
//		param.aspect_ratio_info_present_flag		= gH265_Rec_Param.codec_param[stream_ch].vui.aspect_ratio_info_present_flag;	// Invalid
		param.aspectratio						= gH265_Rec_Param.codec_param[stream_ch].vui.aspect_ratio_idc;
		param.sarwidth							= gH265_Rec_Param.codec_param[stream_ch].vui.sar_width;
		param.sarheight							= gH265_Rec_Param.codec_param[stream_ch].vui.sar_height;
		param.chroma_loc_info_present_flag		= gH265_Rec_Param.codec_param[stream_ch].vui.chroma_loc_info_present_flag;
		param.chroma_sample_loc_type_top		= gH265_Rec_Param.codec_param[stream_ch].vui.chroma_sample_loc_type_top_field;
		param.chroma_sample_loc_type_bottom		= gH265_Rec_Param.codec_param[stream_ch].vui.chroma_sample_loc_type_bottom_field;
		
		param.insert_pic_struct		= gH265_Rec_Param.codec_param[stream_ch].pictstruct_flag;
		param.buf_ctrl_param		= gH265_Rec_Param.codec_param[stream_ch].buffer_ctrl_flag;
		param.filler_en				= gH265_Rec_Param.codec_param[stream_ch].filler_flag;
		
		if( gH265_Rec_Param.codec_param[stream_ch].stream_format == E_H265_IF_STREAM_FORMAT_PES ){
			param.insert_pes_header	= 1;	// PES Header Space ON
			param.rec_format		= 0;	// Default Mode
		}
		else if( gH265_Rec_Param.codec_param[stream_ch].stream_format == E_H265_IF_STREAM_FORMAT_ES_HEADER ){
			param.insert_pes_header	= 0;	// PES Header Space OFF
			param.rec_format		= 0;	// Default Mode
		}
		else{	// E_H265_IF_STREAM_FORMAT_ES
			param.insert_pes_header	= 0;	// PES Header Space OFF
			param.rec_format		= 1;	// MP4 Mode
		}
		
		if( gH265_Rec_Param.codec_param[stream_ch].rate_mode == E_H265_IF_RATE_MODE_CBR ){
			param.ratectl			= 1;
		}
		else if( gH265_Rec_Param.codec_param[stream_ch].rate_mode == E_H265_IF_RATE_MODE_VBR ){
			param.ratectl			= 0;
		}
		else{
			param.ratectl			= 2;
		}
		
		if( gH265_Rec_Param.codec_param[stream_ch].closed_gop == H265_ON ){
			param.gopstructure		= 1;	// ClosedGOP
		}
		else{
			param.gopstructure		= 0;	// OpenGOP
		}
		
		param.pic_crop_right_offset		= gH265_Rec_Param.codec_param[stream_ch].frame_crop_right_offset;
		param.pic_crop_bottom_offset	= gH265_Rec_Param.codec_param[stream_ch].frame_crop_bottom_offset;
		
		// Interim MILB
		param.power_mode			= 0;
		param.partial_qpmod			= 0;
		
		ret = H265_Sirius_Enc_Start( (UINT32)stream_ch, &param, &fmtable );
		if( ret != H265_OK ){
			H265_Err_Print(("--- Sirius Enc Start Err ch=%d\n", stream_ch));
			return ret;
		}
		
		if( stream_ch == E_H265_IF_STREAM_CH_0 ){
			gH265_Sirius_Stop_Done_Wiat_Ptn |= FLG_H265_CODEC_STOP_COMP0;
		}
		else{
			gH265_Sirius_Stop_Done_Wiat_Ptn |= FLG_H265_CODEC_STOP_COMP1;
		}
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	INT32					ret;
	INT32					i;
	UINT32					header_addr;
	UINT32					header_size;
	EncOpenParam			encOP;
	EncOpenParam*			pEncOP;
	EncHevcParam*			pEncHEVC;
	EncInitialInfo			initialInfo;
	SecAxiUse				secAxiUse;
	FrameBufferAllocInfo	fbAllocInfo;
	FrameBuffer				localdecBuffer[D_WAVE_LOCALDEC_BANK_MAX];
	EncHeaderParam			encHeaderParam;
	UINT32					nal_report_addr, nal_report_size;
	
	double	dframeRate = 0;
	INT32	rbspByteSize;
	INT32	rbspBitSize;
	UINT32	vui_rbsp_addr = 0;
	UINT32	vui_rbsp_size;
	
	T_SDRAM_MAP_MOVIE_RECORD_WAVE	map_wave;
	
	H265_Movie_Memset( &encOP, 0, sizeof(encOP) );
	H265_Movie_Memset( &fbAllocInfo, 0, sizeof(fbAllocInfo) );
	H265_Movie_Memset( &localdecBuffer[0], 0, sizeof(FrameBuffer) );
	H265_Movie_Memset( (VOID*)&gH265_YC_Buffer[stream_ch][0], 0, sizeof(FrameBuffer)*D_WAVE_YC_BANK_MAX );
	
	if( gH265_Rec_Param.codec_param[stream_ch].enable_flg == H265_ON ){
		Sdram_Map_Movie_Record_Get_H265_Map( stream_ch, &map_wave );
		
		h265_encode_stream_buffer_init( 0, map_wave.stream.addr[0] );
		h265_encode_stream_buffer_init( 1, map_wave.stream.addr[1] );
		
		pEncOP		= &encOP;
		pEncHEVC	= &pEncOP->EncStdParam.hevcParam;
		
		pEncOP->bitstreamFormat			= STD_HEVC;
		pEncOP->picWidth				= gH265_Rec_Param.codec_param[stream_ch].width;
		pEncOP->picHeight				= gH265_Rec_Param.codec_param[stream_ch].lines;
		switch( gH265_Rec_Param.codec_param[stream_ch].frame_rate ){
			case E_H265_IF_FRAME_RATE_240_0 :
			case E_H265_IF_FRAME_RATE_239_76 :
				pEncOP->frameRateInfo			= 240;
				break;
			case E_H265_IF_FRAME_RATE_120_0 :
			case E_H265_IF_FRAME_RATE_119_88 :
				pEncOP->frameRateInfo			= 120;
				break;
			case E_H265_IF_FRAME_RATE_73_0 :
				pEncOP->frameRateInfo			= 73;
				break;
			case E_H265_IF_FRAME_RATE_72_0 :
				pEncOP->frameRateInfo			= 72;
				break;
			case E_H265_IF_FRAME_RATE_70_0 :
				pEncOP->frameRateInfo			= 70;
				break;
			case E_H265_IF_FRAME_RATE_60_0 :
			case E_H265_IF_FRAME_RATE_59_94 :
				pEncOP->frameRateInfo			= 60;
				break;
			case E_H265_IF_FRAME_RATE_50_0 :
				pEncOP->frameRateInfo			= 50;
				break;
			case E_H265_IF_FRAME_RATE_30_0 :
			case E_H265_IF_FRAME_RATE_29_97 :
				pEncOP->frameRateInfo			= 30;
				break;
			case E_H265_IF_FRAME_RATE_29_0 :
				pEncOP->frameRateInfo			= 29;
				break;
			case E_H265_IF_FRAME_RATE_25_0 :
				pEncOP->frameRateInfo			= 25;
				break;
			case E_H265_IF_FRAME_RATE_24_0 :
			case E_H265_IF_FRAME_RATE_23_97 :
				pEncOP->frameRateInfo			= 24;
				break;
			case E_H265_IF_FRAME_RATE_15_0 :
			case E_H265_IF_FRAME_RATE_14_98 :
				pEncOP->frameRateInfo			= 15;
				break;
			default :
				H265_Err_Print(( "h265_encode_set_param_codec: FrameRate NG\n" ));
				return H265_NG;
		}
//		pEncOP->MESearchRange			= 0;				// not use
//		pEncOP->vbvBufferSize			= 0;				// not use
//		pEncOP->meBlkMode				= 0;				// not use
//		pEncOP->frameSkipDisable		= 0;				// not use
//		pEncOP->gopSize					= 0;				// not use
//		pEncOP->sliceMode.sliceMode		= 0;				// not use
//		pEncOP->sliceMode.sliceSizeMode	= 0;				// not use
//		pEncOP->sliceMode.sliceSize		= 0;				// not use
//		pEncOP->intraRefresh			= 0;				// not use
//		pEncOP->rcIntraQp				= 0;				// not use
//		pEncOP->userQpMax				= 0;				// not use
//		pEncOP->userGamma				= 0;				// not use
//		pEncOP->rcIntervalMode			= 0;				// not use
//		pEncOP->mbInterval				= 0;				// not use
//		pEncOP->MEUseZeroPmv			= 0;				// not use
//		pEncOP->intraCostWeight			= 0;				// not use

		// gnso
		#ifdef SUPPORT_WAVE4ENC_SLICE_INTERRUPT
		{
			pEncOP->sliceIntEnable		= 1;
		}
		#endif
		
		if( gH265_Rec_Param.codec_param[stream_ch].rate_mode == E_H265_IF_RATE_MODE_CBR ){
			pEncOP->bitRate			= gH265_Rec_Param.codec_param[stream_ch].bitrate;
			pEncHEVC->transRate		= gH265_Rec_Param.codec_param[stream_ch].bitrate;
		}
		else if( gH265_Rec_Param.codec_param[stream_ch].rate_mode == E_H265_IF_RATE_MODE_VBR ){
			pEncOP->bitRate			= gH265_Rec_Param.codec_param[stream_ch].average_bitrate;
			pEncHEVC->transRate		= gH265_Rec_Param.codec_param[stream_ch].bitrate;
		}
		else{
			pEncOP->bitRate			= gH265_Rec_Param.codec_param[stream_ch].average_bitrate;
			pEncHEVC->transRate		= 0;
		}
		pEncHEVC->level					= gH265_Rec_Param.codec_param[stream_ch].level;
		if( gH265_Rec_Param.codec_param[stream_ch].tier == E_H265_IF_TIER_MAIN ){
			pEncHEVC->tier					= HEVC_TIER_MAIN;				// Main tier
		}
		else{
			pEncHEVC->tier					= HEVC_TIER_HIGH;				// High tier
		}
		pEncOP->srcBitDepth				= 8;				// Fixed
		pEncHEVC->internalBitDepth		= 8;				// Fixed
		if( pEncHEVC->internalBitDepth > 8 ){
			pEncHEVC->profile			= HEVC_PROFILE_MAIN10;
		}
		else{
			pEncHEVC->profile			= HEVC_PROFILE_MAIN;
		}
//		pEncHEVC->chromaFormatIdc		= 0;				// not use
//		pEncHEVC->losslessEnable		= 0;				// not use
//		pEncHEVC->constIntraPredFlag	= 0;				// not use
//		pEncHEVC->useLongTerm			= 0;				// not use
		
		if( gH265_Rec_Param.codec_param[stream_ch].gop_struct == E_H265_IF_GOP_STR_IPPP ){
			pEncHEVC->gopPresetIdx		= PRESET_IDX_LP_IPPPP_GOP_SIZE_4;
		}
		else if( gH265_Rec_Param.codec_param[stream_ch].gop_struct == E_H265_IF_GOP_STR_IBBP ){
			pEncHEVC->gopPresetIdx		= PRESET_IDX_IBBP_GOP_SIZE_3;
		}
		else if( gH265_Rec_Param.codec_param[stream_ch].gop_struct == E_H265_IF_GOP_STR_IIII ){
			pEncHEVC->gopPresetIdx		= PRESET_IDX_ALL_I;
		}
		else{
			H265_Err_Print(( "h265_encode_set_param_codec: gop_struct NG\n" ));
			return H265_NG;
		}
		
		pEncHEVC->decodingRefreshType	= 2;	// IDR
		pEncHEVC->intraPeriod			= gH265_Rec_Param.codec_param[stream_ch].gop_num;
		pEncHEVC->intraQP				= 32;				// 0 = ignore
//		pEncHEVC->forcedIdrHeaderEnable	= 0;				// not use
		
		if(gH265_Rec_Param.codec_param[stream_ch].frame_crop_bottom_offset % 2){
			pEncHEVC->confWinTop			= gH265_Rec_Param.codec_param[stream_ch].frame_crop_bottom_offset - 1;//0;
			pEncHEVC->confWinBot			= gH265_Rec_Param.codec_param[stream_ch].frame_crop_bottom_offset + 1;
		}
		else{
			pEncHEVC->confWinTop			= gH265_Rec_Param.codec_param[stream_ch].frame_crop_bottom_offset;//0;
			pEncHEVC->confWinBot			= gH265_Rec_Param.codec_param[stream_ch].frame_crop_bottom_offset;
		}
		if(gH265_Rec_Param.codec_param[stream_ch].frame_crop_right_offset % 2){
			pEncHEVC->confWinLeft			= gH265_Rec_Param.codec_param[stream_ch].frame_crop_right_offset - 1;//0;
			pEncHEVC->confWinRight			= gH265_Rec_Param.codec_param[stream_ch].frame_crop_right_offset + 1;
		}
		else{
			pEncHEVC->confWinLeft			= gH265_Rec_Param.codec_param[stream_ch].frame_crop_right_offset;//0;
			pEncHEVC->confWinRight			= gH265_Rec_Param.codec_param[stream_ch].frame_crop_right_offset;
		}
		
		if( gH265_Rec_Param.codec_param[stream_ch].slice_encode_flag == H265_ON ){
			UINT32 int_line = gH265_Rec_Param.codec_param[stream_ch].slice_int_line;
			int ctuNum = 0;
			
			if( int_line > 100 ){
				int_line = 100;
			}
			ctuNum = ((((pEncOP->picWidth+63)/64)*((pEncOP->picHeight+63)/64))*int_line+99)/100;

			pEncHEVC->independSliceMode		= 1;
			pEncHEVC->independSliceModeArg	= ctuNum;
			pEncHEVC->dependSliceMode		= 1;// 1 : Number of CTUs per slice segment
			pEncHEVC->dependSliceModeArg	= ctuNum;
			gH265_Slice_per_Frame = (100+int_line-1)/int_line;
			gH265_Slice_Encode_En = 1;
			H265_Info_Print(("H265 Slice encode: size= %dx%d, int_line= %d, ctuNum= %d\n", 
				pEncOP->picWidth, pEncOP->picHeight, int_line, ctuNum));
		}
		else{
			pEncHEVC->independSliceMode		= 0;
			pEncHEVC->independSliceModeArg	= 0;
			pEncHEVC->dependSliceMode		= 0;
			pEncHEVC->dependSliceModeArg	= 0;
			gH265_Slice_Encode_En = 0;
		}
		
//		pEncHEVC->intraRefreshMode		= 0;				// not use
//		pEncHEVC->intraRefreshArg		= 0;				// not use
		pEncHEVC->useRecommendEncParam	= 0;				// for power saving mode	ES3_Porting
		
		if (pEncHEVC->useRecommendEncParam == 0) {
			// All Fixed
			pEncHEVC->scalingListEnable				= 0;
			pEncHEVC->cuSizeMode					= 0x7;		// enable CU8x8, CU16x16, CU32x32
			pEncHEVC->tmvpEnable					= 1;
			pEncHEVC->wppEnable						= 0;
			pEncHEVC->maxNumMerge					= 2;
			pEncHEVC->dynamicMerge8x8Enable			= 1;
			pEncHEVC->dynamicMerge16x16Enable		= 1;
			pEncHEVC->dynamicMerge32x32Enable		= 1;
			pEncHEVC->disableDeblk					= 0;
			pEncHEVC->lfCrossSliceBoundaryEnable	= 1;
			pEncHEVC->betaOffsetDiv2				= 0;
			pEncHEVC->tcOffsetDiv2					= 0;
			pEncHEVC->skipIntraTrans				= 1;
			pEncHEVC->saoEnable						= 1;
			pEncHEVC->intraInInterSliceEnable		= 1;
			pEncHEVC->intraNxNEnable				= 1;
		}
		
		pEncHEVC->vuiParam.vuiParamFlags= 0;				// not use
//		pEncOP->encodeVuiRbsp			= 0;				// not use
//		pEncOP->encodeHrdRbspInVPS		= 0;				// not use
//		pEncOP->encodeHrdRbspInVUI		= 0;				// not use
		
		pEncOP->rcEnable					= 1;			// Fixed
		pEncOP->initialDelay				= 1000;			// Fixed
//		pEncHEVC->ctuOptParam.roiEnable		= 0;			// not use
		pEncHEVC->ctuOptParam.roiDeltaQp	= 3;			// Fixed
		
//		pEncHEVC->intraQpOffset			= 0;				// not use
		pEncHEVC->initBufLevelx8		= 1;				// Fixed
		pEncHEVC->bitAllocMode			= 0;				// Fixed
		for (i = 0; i < MAX_GOP_NUM; i++) {
			pEncHEVC->fixedBitRatio[i]	= 1;				// Fixed
		}
//		pEncHEVC->cuLevelRCEnable		= 0;				// not use
		pEncHEVC->hvsQPEnable			= 1;				// Fixed
		pEncHEVC->hvsQpScale			= 2;				// Fixed
		pEncHEVC->hvsQpScaleEnable		= (pEncHEVC->hvsQpScale > 0) ? 1: 0;
		
		pEncHEVC->minQp					= 8;				// Fixed
		pEncHEVC->maxQp					= 51;				// Fixed
		pEncHEVC->maxDeltaQp			= 10;				// Fixed
		
		pEncHEVC->vuiParam.vuiAspectRatioIdc	= gH265_Rec_Param.codec_param[stream_ch].vui.aspect_ratio_idc;
		pEncHEVC->vuiParam.vuiSarSize			= ((UINT32)gH265_Rec_Param.codec_param[stream_ch].vui.sar_width << 16) + gH265_Rec_Param.codec_param[stream_ch].vui.sar_height;
		
		pEncHEVC->numTicksPocDiffOne	= 0;
		switch( gH265_Rec_Param.codec_param[stream_ch].frame_rate ){
			case E_H265_IF_FRAME_RATE_240_0 :
				pEncHEVC->timeScale				= 240*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_239_76 :
				pEncHEVC->timeScale				= 239.76*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_120_0 :
				pEncHEVC->timeScale				= 120*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_119_88 :
				pEncHEVC->timeScale				= 119.88*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_73_0 :
				pEncHEVC->timeScale				= 73*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_72_0 :
				pEncHEVC->timeScale				= 72*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_70_0 :
				pEncHEVC->timeScale				= 70*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_60_0 :
				pEncHEVC->timeScale				= 60*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_50_0 :
				pEncHEVC->timeScale				= 50*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_59_94 :
				pEncHEVC->timeScale				= 59.94*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_30_0 :
				pEncHEVC->timeScale				= 30*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_29_97 :
				pEncHEVC->timeScale				= 29.97*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_29_0 :
				pEncHEVC->timeScale				= 29.00*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_25_0 :
				pEncHEVC->timeScale				= 25.00*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_24_0 :
				pEncHEVC->timeScale				= 24*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_23_97 :
				pEncHEVC->timeScale				= 23.97*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_15_0 :
				pEncHEVC->timeScale				= 15*1000.0;
				break;
			case E_H265_IF_FRAME_RATE_14_98 :
				pEncHEVC->timeScale				= 14.98*1000.0;
				break;
			default :
				H265_Err_Print(( "h265_encode_set_param_codec: FrameRate NG\n" ));
				return H265_NG;
		}
		pEncHEVC->numUnitsInTick		= 1000;
		
//		pEncHEVC->chromaCbQpOffset		= 0;				// not use
//		pEncHEVC->chromaCrQpOffset		= 0;				// not use
		pEncHEVC->initialRcQp			= 25;				// Fixed
//		pEncHEVC->nrYEnable				= 0;				// not use
//		pEncHEVC->nrCbEnable			= 0;				// not use
//		pEncHEVC->nrCrEnable			= 0;				// not use
//		pEncHEVC->nrNoiseEstEnable		= 0;				// not use
//		pEncHEVC->nrNoiseSigmaY			= 0;				// not use
//		pEncHEVC->nrNoiseSigmaCb		= 0;				// not use
//		pEncHEVC->nrNoiseSigmaCr		= 0;				// not use
//		pEncHEVC->nrIntraWeightY		= 0;				// not use
//		pEncHEVC->nrIntraWeightCb		= 0;				// not use
//		pEncHEVC->nrIntraWeightCr		= 0;				// not use
//		pEncHEVC->nrInterWeightY		= 0;				// not use
//		pEncHEVC->nrInterWeightCb		= 0;				// not use
//		pEncHEVC->nrInterWeightCr		= 0;				// not use
		
		pEncHEVC->intraMinQp			= 8;				// Fixed
		pEncHEVC->intraMaxQp			= 51;				// Fixed
		
		if( gH265_Rec_Param.codec_param[stream_ch].filler_flag == H265_ON ){
			pEncHEVC->fillerDataEnable		= 1;
		}
		else{
			pEncHEVC->fillerDataEnable		= 0;
		}
		
		pEncOP->mvReportEnable				= 0;			// Vector? ES3_Porting
//		pEncOP->distortionMapReportEnable	= 0;			// Fixed
		
		pEncOP->srcFormat				= FORMAT_420;		// Fixed;
//		pEncOP->nv21					= 0;				// not use;
		
		pEncOP->bitstreamBuffer			= map_wave.stream.addr[0];	// stream buffer addr
		pEncOP->bitstreamBufferSize		= map_wave.stream.size;		// stream buffer size
		
		pEncOP->ringBufferEnable		= 0;				// Fixed
		pEncOP->cbcrInterleave			= 1;				// Fixed
		
		pEncOP->frameEndian				= VDI_128BIT_BIG_ENDIAN;	// Fixed
		pEncOP->streamEndian			= VDI_BIG_ENDIAN;			// Fixed
		pEncOP->sourceEndian			= VDI_128BIT_BIG_ENDIAN;	// Fixed
		
		pEncOP->lineBufIntEn			= 1;					// Fixed
		pEncOP->coreIdx					= 0;					// Fixed
		pEncOP->cbcrOrder				= CBCR_ORDER_NORMAL;	// Fixed
		
		pEncOP->numCores				= 1;
		
		if( (gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_4000_3000) ||
			(gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_4096_2304) ||
			(gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_3000_3000) ){
			if( pEncOP->frameRateInfo > 30 ){
				pEncOP->numCores = 2;
			}
		}
		else if( (gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_4096_2160) ||
				 (gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_4096_2048) ||
				 (gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_3840_2160) ){
			if( pEncOP->frameRateInfo > 30 ){
				pEncOP->numCores = 2;
			}
		}
		else if( gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_2704_2028 ){
			if( pEncOP->frameRateInfo > 48 ){
				pEncOP->numCores = 2;
			}
		}
		else if( (gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_2704_1520) ||
				 (gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_1920_1440) ){
			if( pEncOP->frameRateInfo > 60 ){
				pEncOP->numCores = 2;
			}
		}
		else if( (gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_1920_1080) ||
				 (gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_1440_1080) ||
				 (gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_1440_720) ||
				 (gH265_Rec_Param.codec_param[stream_ch].size == E_H265_IF_IMAGE_SIZE_1280_960) ){
			if( pEncOP->frameRateInfo > 120 ){
				pEncOP->numCores = 2;
			}
		}
		
		pEncOP->EncStdParam.hevcParam.useLongTerm	= 0;	// Fixed
		
		// VUI or HDR
		memset( (void*)&gH265_Codec_VUI_Info[stream_ch], 0x00, sizeof(vui_t) );
		
		if( gH265_Rec_Param.codec_param[stream_ch].vui.aspect_ratio_info_present_flag == H265_ON ){
			gH265_Codec_VUI_Info[stream_ch].vui_parameters_presesent_flag	= 1;
			
			gH265_Codec_VUI_Info[stream_ch].aspect_ratio_info_present_flag	= 1;
			gH265_Codec_VUI_Info[stream_ch].aspect_ratio_idc				= gH265_Rec_Param.codec_param[stream_ch].vui.aspect_ratio_idc;
			if( gH265_Rec_Param.codec_param[stream_ch].vui.aspect_ratio_idc == 255 ){
				gH265_Codec_VUI_Info[stream_ch].sar_width					= gH265_Rec_Param.codec_param[stream_ch].vui.sar_width;
				gH265_Codec_VUI_Info[stream_ch].sar_height					= gH265_Rec_Param.codec_param[stream_ch].vui.sar_height;
			}
		}
		if( gH265_Rec_Param.codec_param[stream_ch].vui.video_signal_type_present_flag == H265_ON ){
			gH265_Codec_VUI_Info[stream_ch].vui_parameters_presesent_flag	= 1;
			
			gH265_Codec_VUI_Info[stream_ch].video_signal_type_present_flag	= 1;
			gH265_Codec_VUI_Info[stream_ch].video_format					= gH265_Rec_Param.codec_param[stream_ch].vui.video_format;
			gH265_Codec_VUI_Info[stream_ch].video_full_range_flag			= gH265_Rec_Param.codec_param[stream_ch].vui.video_full_range_flag;
			if( gH265_Rec_Param.codec_param[stream_ch].vui.colour_description_present_flag == H265_ON ){
				gH265_Codec_VUI_Info[stream_ch].colour_description_present_flag	= 1;
				gH265_Codec_VUI_Info[stream_ch].colour_primaries				= gH265_Rec_Param.codec_param[stream_ch].vui.colour_primaries;
				gH265_Codec_VUI_Info[stream_ch].transfer_characteristics		= gH265_Rec_Param.codec_param[stream_ch].vui.transfer_characteristics;
				gH265_Codec_VUI_Info[stream_ch].matrix_coeffs					= gH265_Rec_Param.codec_param[stream_ch].vui.matrix_coefficients;
			}
		}
		if( gH265_Rec_Param.codec_param[stream_ch].vui.chroma_loc_info_present_flag == H265_ON ){
			gH265_Codec_VUI_Info[stream_ch].vui_parameters_presesent_flag		= 1;
			
			gH265_Codec_VUI_Info[stream_ch].chroma_loc_info_present_flag		= 1;
			gH265_Codec_VUI_Info[stream_ch].chroma_sample_loc_type_top_field	= gH265_Rec_Param.codec_param[stream_ch].vui.chroma_sample_loc_type_top_field;
			gH265_Codec_VUI_Info[stream_ch].chroma_sample_loc_type_bottom_field	= gH265_Rec_Param.codec_param[stream_ch].vui.chroma_sample_loc_type_bottom_field;
		}
		if( gH265_Rec_Param.codec_param[stream_ch].buffer_ctrl_flag == H265_ON ){
			gH265_Codec_VUI_Info[stream_ch].vui_parameters_presesent_flag	= 1;
			
			memset( (void*)&gH265_Codec_HDR_Info[stream_ch], 0x00, sizeof(hrd_t) );
			
			switch( gH265_Rec_Param.codec_param[stream_ch].frame_rate ){
				case E_H265_IF_FRAME_RATE_240_0 :
					dframeRate			= 240;
					break;
				case E_H265_IF_FRAME_RATE_239_76 :
					dframeRate			= 239.76;
					break;
#if 0	// unsupported.
				case E_H265_IF_FRAME_RATE_200_0 :
					dframeRate			= 200;
					break;
#endif
				case E_H265_IF_FRAME_RATE_120_0 :
					dframeRate			= 120;
					break;
				case E_H265_IF_FRAME_RATE_119_88 :
					dframeRate			= 119.88;
					break;
				case E_H265_IF_FRAME_RATE_100_0 :
					dframeRate			= 100;
					break;
				case E_H265_IF_FRAME_RATE_73_0 :
					dframeRate			= 73;
					break;
				case E_H265_IF_FRAME_RATE_72_0 :
					dframeRate			= 72;
					break;
				case E_H265_IF_FRAME_RATE_70_0 :
					dframeRate			= 70;
					break;
				case E_H265_IF_FRAME_RATE_60_0 :
					dframeRate			= 60;
					break;
				case E_H265_IF_FRAME_RATE_59_94 :
					dframeRate			= 59.94;
					break;
				case E_H265_IF_FRAME_RATE_50_0 :
					dframeRate			= 50;
					break;
				case E_H265_IF_FRAME_RATE_48_0 :
					dframeRate			= 48;
					break;
#if 0	// unsupported.
				case E_H265_IF_FRAME_RATE_47_95 :
					dframeRate			= 47.95;
					break;
#endif
				case E_H265_IF_FRAME_RATE_30_0 :
					dframeRate			= 30;
					break;
				case E_H265_IF_FRAME_RATE_29_97 :
					dframeRate			= 29.97;
					break;
				case E_H265_IF_FRAME_RATE_29_0 :
					dframeRate			= 29;
					break;
#if 1	// unsupported. NEED TO CHECK
				case E_H265_IF_FRAME_RATE_25_0 :
					dframeRate			= 25;
					break;
#endif
				case E_H265_IF_FRAME_RATE_24_0 :
					dframeRate			= 24;
					break;
				case E_H265_IF_FRAME_RATE_23_97 :
					dframeRate			= 23.97;
					break;
				case E_H265_IF_FRAME_RATE_15_0 :
					dframeRate			= 15;
					break;
				case E_H265_IF_FRAME_RATE_14_98 :
					dframeRate			= 14.98;
					break;
				default :
					H265_Err_Print(( "h265_encode_set_param_codec: FrameRate NG\n" ));
					return H265_NG;
			}
			
			gH265_Codec_VUI_Info[stream_ch].vui_timing_info_present_flag	= 1;
			gH265_Codec_VUI_Info[stream_ch].vui_num_units_in_tick			= 1000;
			gH265_Codec_VUI_Info[stream_ch].vui_time_scale					= dframeRate * 1000.0;
			gH265_Codec_VUI_Info[stream_ch].vui_hrd_parameters_present_flag	= 1;
		}
		
		if( gH265_Codec_VUI_Info[stream_ch].vui_parameters_presesent_flag != 0 ){
			Sdram_Map_Movie_Record_Get_H265_VUI( stream_ch, &vui_rbsp_addr, &vui_rbsp_size );
			
			pEncOP->encodeVuiRbsp   = 1;
			pEncOP->vuiRbspDataAddr = vui_rbsp_addr;
			memset( (void*)vui_rbsp_addr, 0x00, (4*1024) );
			
			(VOID)H265_Wave_EncodeHRDInVUI( (hrd_t*)&gH265_Codec_HDR_Info[stream_ch], (vui_t*)&gH265_Codec_VUI_Info[stream_ch], (UINT8*)vui_rbsp_addr, (INT32*)&rbspByteSize, (INT32*)&rbspBitSize, dframeRate );
			H265_Movie_Cache_Clean( (ULONG)vui_rbsp_addr, rbspByteSize );
			pEncOP->vuiRbspDataSize = rbspBitSize;
			memset( (void*)&gH265_Codec_Pic_Timing_Sei[stream_ch],       0x00, sizeof(T_H265_WAVE_SEI_PIC_TIMING) );
			memset( (void*)&gH265_Codec_Active_Parameter_Sei[stream_ch], 0x00, sizeof(T_H265_WAVE_SEI_ACTIVE_PARAMETER) );
			memset( (void*)&gH265_Codec_Buffering_Period_Sei[stream_ch], 0x00, sizeof(T_H265_WAVE_SEI_BUFFERING_PERIOD) );
		}
		
		// nal report
		Sdram_Map_Movie_Record_Get_H265_NAL_Report( stream_ch, 0, &nal_report_addr, &nal_report_size );
		encOP.nalSizeReportAddr = nal_report_addr;
		encOP.nalSizeReportSize = (nal_report_size * 2);
		
		ret = H265_Wave_EncOpen( stream_ch, (EncHandle*)&gH265_Codec_Handle[stream_ch], &encOP );
		if (ret != RETCODE_SUCCESS) {
			H265_Err_Print(("H265_Wave_EncOpen error (%d)\n", ret ));
			return H265_NG;
		}
		
//		H265_Wave_EncGiveCommand( stream_ch, gH265_Codec_Handle[stream_ch], ENABLE_LOGGING, 0 );	// for Debug
		
		ret = H265_Wave_EncGetInitialInfo( stream_ch, gH265_Codec_Handle[stream_ch], &initialInfo );
		if (ret != RETCODE_SUCCESS) {
			H265_Err_Print(("H265_Wave_EncGetInitialInfo error (%d)\n", ret ));
			H265_Wave_EncClose( stream_ch, gH265_Codec_Handle[stream_ch] );
			return H265_NG;
		}
		
		// debug print for vmem_size
		H265_Wave_VmemSize_Calculation( stream_ch, gH265_Codec_Handle[stream_ch], &initialInfo );
		
		H265_Movie_Memset( &secAxiUse, 0x00, sizeof(SecAxiUse) );
		secAxiUse.u.wave4.useEncImdEnable	= 1;		// (encConfig.secondary_axi & 0x1)?TRUE:FALSE;  //USE_IMD_INTERNAL_BUF
		secAxiUse.u.wave4.useEncRdoEnable	= 1;		// (encConfig.secondary_axi & 0x2)?TRUE:FALSE;  //USE_RDO_INTERNAL_BUF
		secAxiUse.u.wave4.useEncLfEnable	= 1;		// (encConfig.secondary_axi & 0x4)?TRUE:FALSE;  //USE_LF_INTERNAL_BUF
		
		ret = H265_Wave_EncGiveCommand( stream_ch, gH265_Codec_Handle[stream_ch], SET_SEC_AXI, &secAxiUse );
		if (ret != RETCODE_SUCCESS) {
			H265_Err_Print(( "H265_Wave_EncGiveCommand(AXI) error (%d)\n", ret ));
			H265_Wave_EncClose( stream_ch, gH265_Codec_Handle[stream_ch] );
			return H265_NG;
		}
		
		for( i=0; i<D_WAVE_LOCALDEC_BANK_MAX; i++ ){
			localdecBuffer[i].bufY			= map_wave.localdec.addr[i];
			localdecBuffer[i].bufCb			= 0xFFFFFFFF;
			localdecBuffer[i].bufCr			= 0xFFFFFFFF;
			localdecBuffer[i].size			= map_wave.localdec.size;
			localdecBuffer[i].updateFbInfo	= TRUE;
		}
		
		ret = H265_Wave_EncRegisterFrameBuffer( stream_ch, gH265_Codec_Handle[stream_ch], localdecBuffer, map_wave.localdec.max_bank, VPU_ALIGN16( pEncOP->picWidth+16 ), VPU_ALIGN8( pEncOP->picHeight ), COMPRESSED_FRAME_MAP );
		if( ret != RETCODE_SUCCESS ) {
			H265_Err_Print(("H265_Wave_EncRegisterFrameBuffer error (%d)\n", ret ));
			H265_Wave_EncClose( stream_ch, gH265_Codec_Handle[stream_ch] );
			return H265_NG;
		}
		
		
		for( i=0; i<D_WAVE_YC_BANK_MAX; i++ ){
			gH265_YC_Buffer[stream_ch][i].bufY				= map_wave.yc.addr[i].y;
			gH265_YC_Buffer[stream_ch][i].bufCb				= 0xFFFFFFFF;
			gH265_YC_Buffer[stream_ch][i].bufCr				= 0xFFFFFFFF;
			gH265_YC_Buffer[stream_ch][i].cbcrInterleave	= 1;
			gH265_YC_Buffer[stream_ch][i].nv21				= 0;
			gH265_YC_Buffer[stream_ch][i].updateFbInfo		= TRUE;
		}
		
		if( gH265_Rec_Param.codec_param[stream_ch].compression_enable == H265_ON ){
			fbAllocInfo.mapType		= ARM_COMPRESSED_FRAME_MAP;
		}
		else{
			fbAllocInfo.mapType		= LINEAR_FRAME_MAP;
		}
		fbAllocInfo.format			= FORMAT_420;				// Fixed
		fbAllocInfo.cbcrInterleave	= 1;						// Fixed;
		fbAllocInfo.stride			= map_wave.yc.g_width;
		fbAllocInfo.height			= map_wave.yc.g_lines;
		fbAllocInfo.endian			= pEncOP->sourceEndian;
		fbAllocInfo.type			= FB_TYPE_PPU;				// Fixed
		fbAllocInfo.num				= map_wave.yc.max_bank;
		fbAllocInfo.nv21			= 0;						// Fixed
		
		ret = H265_Wave_EncAllocateFrameBuffer( stream_ch, gH265_Codec_Handle[stream_ch], fbAllocInfo, (FrameBuffer*)&gH265_YC_Buffer[stream_ch][0] );
		if (ret != RETCODE_SUCCESS) {
			H265_Err_Print(("H265_Wave_EncAllocateFrameBuffer error (%d)\n", ret ));
			H265_Wave_EncClose( stream_ch, gH265_Codec_Handle[stream_ch] );
			return H265_NG;
		}
		
		// VPS/SPS/PPS create			// ES3_Porting   maybe other point
		H265_Movie_Memset( &encHeaderParam, 0x00, sizeof(EncHeaderParam) );
		Sdram_Map_Movie_Record_Get_H265_Header( stream_ch, &header_addr, &header_size );
		
//		encHeaderParam.buf			= header_addr;				// for FrameBuffer
		encHeaderParam.buf			= map_wave.stream.addr[0];	// for LineBuffer
		encHeaderParam.size			= header_size;
		encHeaderParam.headerType	= CODEOPT_ENC_VPS | CODEOPT_ENC_SPS | CODEOPT_ENC_PPS | CODEOPT_ENC_AUD;	// Fixed
		encHeaderParam.nalSizeReportEnable = 1;
		
		ret = H265_Wave_EncGiveCommand( stream_ch, gH265_Codec_Handle[stream_ch], ENC_PUT_VIDEO_HEADER, &encHeaderParam );
		if (ret != RETCODE_SUCCESS) {
			H265_Err_Print(("H265_Wave_EncGiveCommand(HEADER) error (%d)\n", ret ));
			H265_Wave_EncClose( stream_ch, gH265_Codec_Handle[stream_ch] );
			return H265_NG;
		}
		if (encHeaderParam.size == 0) {
			H265_Err_Print(("encHeaderParam.size error (%d)\n", encHeaderParam.size ));
			H265_Wave_EncClose( stream_ch, gH265_Codec_Handle[stream_ch] );
			return H265_NG;
		}
		
		// If Ring buffer, header is output to stream buffer, so need copy to header addr.
		H265_Movie_Cache_Flush( (ULONG)encHeaderParam.buf, encHeaderParam.size );
		H265_Movie_Memcpy( (VOID*)header_addr, (VOID*)encHeaderParam.buf, encHeaderParam.size );
		H265_Movie_Cache_Clean( (ULONG)header_addr, encHeaderParam.size );
		
		gH265_Codec_Header_Info[stream_ch].header_addr	= header_addr;
		gH265_Codec_Header_Info[stream_ch].header_size	= encHeaderParam.size;
		gH265_Codec_Header_Info[stream_ch].AUD_NAL_size	= encHeaderParam.retEncSizeAUD;
		gH265_Codec_Header_Info[stream_ch].VPS_NAL_size	= encHeaderParam.retEncSizeVPS;
		gH265_Codec_Header_Info[stream_ch].SPS_NAL_size	= encHeaderParam.retEncSizeSPS;
		gH265_Codec_Header_Info[stream_ch].PPS_NAL_size	= encHeaderParam.retEncSizePPS;
		
		gH265_Codec_Queue_Ctrl[gH265_Codec_Handle[stream_ch]->instIndex] = H265_Wave_Queue_Create( map_wave.stream.max_bank, sizeof( EncOutputInfo ) );
		if( stream_ch == E_H265_IF_STREAM_CH_0 ){
			gH265_Codec_Queue_Ctrl[gH265_Codec_Handle[stream_ch]->instIndex]->lock = SID_WAVE1_QUEUE;
		}
		else{
			gH265_Codec_Queue_Ctrl[gH265_Codec_Handle[stream_ch]->instIndex]->lock = SID_WAVE2_QUEUE;
		}
	}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
	
	return H265_OK;
}

/**
 *	@brief		Start Task Process.\n
 *				In case of Audio Task, this method initialize audio process.
 *	@param		i:task_id	Task Identifier
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_task_start( UINT32 task_id )
{
	OS_USER_ER			ercd, ercd2;
	T_OS_USER_RTSK		rtsk;	// Task Information
	
	OS_User_Ter_Tsk(task_id);
	// Create Task process
	do{
		ercd = OS_User_Sta_Tsk( task_id, H265_MODE_RECORD );
		if( ercd != D_OS_USER_E_OK ){
			if( ercd == D_OS_USER_E_OBJ ){
				ercd2 = OS_User_Ref_Tsk( task_id, &rtsk );
				if( ercd2 == D_OS_USER_E_OK ){
					if( rtsk.tskstat == D_OS_USER_TTS_DMT ){
						;
					}
					else{
						;
					}
				}
				OS_User_Ter_Tsk(task_id);
			}
		}
	} while(ercd != D_OS_USER_E_OK);
	return;
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/**
 *	@brief		It sets Sirius Parameters for Frame Encode
 *	@param		stream_ch		Stream CH(Sirius CH)
 *	@param		enc_param		ENC_PARAM table pointer
 *	@param		fm_table		FM_TABLE table pointer
 *	@param		frame_no		frame number
 *	@param		gop_cnt			gop counter for Picture Type
 *	@return		H265_OK			Normal End
 *	@return		H265_OK			Abnormal End
 *	@note		None
 *	@attention	None
 */
static INT32 h265_encode_sirius_enc_param_set(
				E_H265_IF_STREAM_CH		stream_ch,
				T_SR_ENC_PARAM			*enc_param,
				T_SR_ENC_FM_TABLE		*fm_table,
				ULLONG					*frame_no,
				USHORT					vop_cnt,
				USHORT					*gop_cnt
				)
{
	INT32	ret;
	UINT32	empty_index;
	UINT32	change_flg = H265_OFF;
	
	// input parameter check
	if( (enc_param == NULL) || (fm_table == NULL) || (frame_no == NULL) ){
		H265_Err_Print(("---  h265_encode_sirius_enc_param_set: NULL\n"));
		return H265_NG;
	}
	
	OS_User_Wai_Sem(SID_H265);
	ret = H265_Sirius_Enc_Fm_Empty_Field_Get( stream_ch, &empty_index );
	if( ret != H265_OK ){
		H265_Err_Print(( "H265_Sirius_Enc_Fm_Empty_Field_Get NG\n" ));
		OS_User_Sig_Sem(SID_H265);
		return H265_NG;
	}
	ret = h265_encode_Vin_get_decide_bank( stream_ch, (INT32*)&fm_table->physical_bank_idx[empty_index], frame_no );
	if( ret == H265_ENC_VIN_CTL_NONEDATA ){
		if( gH265_ViewLater_Count == 0 ){
			H265_Warning_Print(( "H265 View task later!! ch[%d] (timming NG) \n", stream_ch ));
		}
		gH265_ViewLater_Count++;
		gH265_ViewLater_Total_Count++;
		if( gH265_ViewLater_Count > D_H265_ENCODE_LATER_THRESHOLD ){
			gH265_ViewLater_Count = 0;
		}
		if( gH265_Vin_Adr_Ctl[stream_ch].decide_read_point == 0 ){
			h265_encode_Vin_set_decide_bank( stream_ch, gH265_Vin_Adr_Ctl[stream_ch].decide_bank[ gH265_Vin_Adr_Ctl[stream_ch].max_bank_num - 1 ], gH265_Vin_Adr_Ctl[stream_ch].decide_frame_no[ gH265_Vin_Adr_Ctl[stream_ch].max_bank_num - 1 ] );
		}
		else{
			h265_encode_Vin_set_decide_bank( stream_ch, gH265_Vin_Adr_Ctl[stream_ch].decide_bank[ gH265_Vin_Adr_Ctl[stream_ch].decide_read_point - 1 ], gH265_Vin_Adr_Ctl[stream_ch].decide_frame_no[ gH265_Vin_Adr_Ctl[stream_ch].decide_read_point - 1 ] );
		}
		h265_encode_Vin_get_decide_bank( stream_ch, (INT32*)&fm_table->physical_bank_idx[empty_index], frame_no );
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265 View later %u-0x%lX", stream_ch, (ULONG)*frame_no ));
		
		H265_If_Record_Notify_RepeatFrame( stream_ch );
		
		gH265_Vin_Adr_Ctl[stream_ch].duplicate_check_tbl[fm_table->physical_bank_idx[empty_index]]++;
	}
	
	fm_table->use_flag[empty_index]	= H265_ON;
	fm_table->pts[empty_index]		= gH265_Enc_Ctrl.video[stream_ch].total_time_increment;
	gH265_Fm_Table_Use_Flag[stream_ch][empty_index] = H265_ON;
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-0x%lX FM", stream_ch, (ULONG)*frame_no ));
	
	OS_User_Sig_Sem(SID_H265);
	
	enc_param->frame_param_flag = 0;	// reset
	
	// Serial Number
	gH265_Enc_Serial_Num[stream_ch]++;
	enc_param->serial_num = (UINT32)*frame_no;

	// Frame Count in GOP
	enc_param->frame_count_in_gop	= vop_cnt;
	
	if( (vop_cnt == 1) && (gH265_Dynamic_Change_Flag[stream_ch] == H265_ON) ){
		if( gH265_Rec_Param.codec_param[stream_ch].bitrate > gH265_Dynamic_Param[stream_ch].bitrate ){
			gH265_Rec_Param.codec_param[stream_ch].average_bitrate = gH265_Dynamic_Param[stream_ch].bitrate;
		}
		else{
			H265_Warning_Print(("+++ H265 Dynamic parameter bitrat is over MAX bitrate.\n" ));
		}
		
		gH265_Rec_Param.codec_param[stream_ch].gop_num				= gH265_Dynamic_Param[stream_ch].frame_num_of_gop;
		gH265_Rec_Param.codec_param[stream_ch].gop_struct			= gH265_Dynamic_Param[stream_ch].gop_struct;
		gH265_Rec_Param.codec_param[stream_ch].gop_num_idr_to_idr	= gH265_Dynamic_Param[stream_ch].gop_num_idr_to_idr;
		gH265_Rec_Param.codec_param[stream_ch].closed_gop			= gH265_Dynamic_Param[stream_ch].closed_gop;
		
		*gop_cnt = 1;	// GOP Reset
		
		change_flg = H265_ON;
		gH265_Dynamic_Change_Flag[stream_ch] = H265_OFF;
	}
	
	enc_param->sumy_top = 0;
	enc_param->sumy_bot = 0;
	enc_param->ssqy_top = 0;
	enc_param->ssqy_bot = 0;
	enc_param->sumcb_top = 0;
	enc_param->sumcb_bot = 0;
	enc_param->ssqcb_top = 0;
	enc_param->ssqcb_bot = 0;
	enc_param->sumcr_top = 0;
	enc_param->sumcr_bot = 0;
	enc_param->ssqcr_top = 0;
	enc_param->ssqcr_bot = 0;
	
	enc_param->partial_adapqpen[0] = 0;
	enc_param->partial_adapqpen[1] = 0;
	enc_param->partial_qdownrange[0] = 0;
	enc_param->partial_qdownrange[1] = 0;
	enc_param->partial_qpmin[0] = 0;
	enc_param->partial_qpmin[1] = 0;
	enc_param->partial_startpos[0] = 0;
	enc_param->partial_startpos[1] = 0;
	enc_param->partial_widthsize[0] = 0;
	enc_param->partial_widthsize[1] = 0;
	enc_param->partial_height[0] = 0;
	enc_param->partial_height[1] = 0;
	enc_param->partial_exptadapqpen = 0;
	enc_param->partial_qpuprange = 0;
	enc_param->partial_qpmax = 0;
	
	if( *gop_cnt == 1 ){
		enc_param->frame_param_flag	|= 0x08;
		
		enc_param->gopstructure		= 1;	// next GOP is Closed
	}
	else{
		enc_param->frame_param_flag	|= 0x08;
		
		enc_param->gopstructure		= 0;	// next GOP is Open
	}
	
	if( change_flg == H265_ON ){
		enc_param->frame_param_flag |= 0x14;
		
#if 0	// not use Dynamic
		enc_param->aspectratio	= 0;
		enc_param->sarwidth		= 0;
		enc_param->sarheight	= 0;
#endif
		
		enc_param->videoavrgbitrate	= gH265_Rec_Param.codec_param[stream_ch].average_bitrate / 1000;
		enc_param->gopn				= gH265_Rec_Param.codec_param[stream_ch].gop_num;
		
		if( gH265_Rec_Param.codec_param[stream_ch].gop_struct == E_H265_IF_GOP_STR_IPPP){
			enc_param->gopm			= 1;
		}
		else{
			enc_param->gopm			= 3;
		}
	}
	
	if( gH265_Rec_Param.codec_param[stream_ch].slice_encode_flag == H265_ON ){
		enc_param->slice_num = 4;
	}
	else{
		enc_param->slice_num = 1;
	}
	
	return H265_OK;
}

/**
 *	@brief		It executes Ribery Encoder.
 *	@param		stream_ch		Stream CH
 *	@param		enc_param		ENC_PARAM
 *	@param		fm_table		FM_TABLE
 *	@return		H265_OK			Normal End
 *	@return		H265_NG			Abnormal End
 *	@note		None
 *	@attention	None
 */
static INT32 h265_encode_sirius_enc_request(
						E_H265_IF_STREAM_CH	stream_ch,
						T_SR_ENC_PARAM*		enc_param,
						T_SR_ENC_FM_TABLE*	fm_table
						)
{
	INT32	ret = 0;
	
	if( (enc_param == NULL) || (fm_table == NULL) ){
		H265_Err_Print(("---  h265_encode_sirius_enc_request: NULL\n"));
		return H265_NG;
	}
	
	// H.265 Encode Start
	ret = H265_Sirius_Enc_Frame( stream_ch, enc_param, fm_table );
	
	return ret;
}

/**
 *	@brief		Get Encoder Result.\n
 *	@param		i/o:video_info		Video Informations
 *	@return		H265_OK				OK - Next Exist
 *				H265_OK_COMPLETE		OK - Read Completed
 *	@return		H265_ERR_INPUT_PARAM	Input Parameter Error
 *	@return		H265_NG				No Sample
 *	@note		None
 *	@attention	None
 */
static INT32 h265_encode_sirius_result_get( E_H265_IF_STREAM_CH stream_ch, T_H265_ENC_VIDEO_INFO* video_info )
{
	UINT32					i;
	UINT32					vstrm_flg;
	UINT32					nal_count;
	
	if( video_info == NULL ){
		return H265_ERR_INPUT_PARAM;
	}
	
	vstrm_flg = H265_OFF;
	
	for( i=0; i<D_H265_ENC_STREAM_FIFO_COUNT; i++ ){
		if( gH265_Free_Vstream_Ctrl[i].use_flg != H265_ON ){
			gH265_Free_Vstream_Ctrl[i].use_flg		= H265_ON;
			gH265_Free_Vstream_Ctrl[i].ch			= stream_ch;
			gH265_Free_Vstream_Ctrl[i].free_size	= gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].picture_size;
			vstrm_flg = H265_ON;
			video_info->vstream_index = i;
			break;
		}
	}
	if( vstrm_flg != H265_ON ){
		H265_Err_Print(("--- Vstrm Table Not Empty\n" ));
		return H265_NG;
	}
	
	gH265_Vstrem_Serial_Num[stream_ch]++;
	
	video_info->vstrem_serial_num	= gH265_Vstrem_Serial_Num[stream_ch];
	video_info->frame_no			= gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].serial_num;
	video_info->frame_count_in_gop	= gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].frame_count_in_gop;
	
	h265_encode_stream_around_check( stream_ch, video_info, gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point] );
	
	if( gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].type == 0 ){
		video_info->sample_type		= E_H265_IF_PIC_TYPE_IDR;
	}
	else if( gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].type == 1 ){
		video_info->sample_type		= E_H265_IF_PIC_TYPE_I;
	}
	else if( gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].type == 2 ){
		video_info->sample_type		= E_H265_IF_PIC_TYPE_P;
	}
	else{
		video_info->sample_type		= E_H265_IF_PIC_TYPE_B;
	}
	
	video_info->bitrate				= gH265_Rec_Param.codec_param[stream_ch].bitrate;
	video_info->ave_bitrate			= gH265_Rec_Param.codec_param[stream_ch].average_bitrate;
	
	nal_count = 0;
	for( i=0; i<gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].nal_unit_num; i++ ){
		if( gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].nal_unit_size[i] != 0 ){
			video_info->nal_unit_size[nal_count] = gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].nal_unit_size[i];
			gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].nal_unit_size[i] = 0;
			nal_count++;
		}
	}
	video_info->nal_unit_num		= nal_count;
	
	video_info->sample_pts			= (LLONG)gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].pts;
	video_info->sample_dts			= (LLONG)gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].dts;
	video_info->cpb_over_flag		= gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].cpb_over_flag;
	video_info->poc					= gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].poc_val;
	
	if( (gH265_Frame_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].flag &= 0x0001) == 1 ){
		video_info->stream_end_flg	= H265_ON;
	}
	
	OS_User_Wai_Sem(SID_H265_STREAM_INT);
	gH265_Vstrm_Fifo[stream_ch].r_point++;
	if( gH265_Vstrm_Fifo[stream_ch].r_point >= D_H265_ENCODE_VINFO_FIFO_MAX ){
		gH265_Vstrm_Fifo[stream_ch].r_point = 0;
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "r %d-%d",gH265_Vstrm_Fifo[stream_ch].r_point, gH265_Vstrm_Fifo[stream_ch].w_point ));
	if( (gH265_Vstrm_Fifo[stream_ch].w_point != gH265_Vstrm_Fifo[stream_ch].r_point) ||
		( (gH265_Vstrm_Fifo[stream_ch].w_point == gH265_Vstrm_Fifo[stream_ch].r_point) && (gH265_Vstrm_Fifo[stream_ch].fifo_over_flg == H265_ON) ) ){
		if( stream_ch == E_H265_IF_STREAM_CH_0 ){
//			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "lop %d-%d",gH265_Vstrm_Fifo[stream_ch].r_point, gH265_Vstrm_Fifo[stream_ch].w_point ));
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSTRM_COMP0 );
		}
		else{
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSTRM_COMP1 );
		}
	}
	
	gH265_Vstrm_Fifo[stream_ch].fifo_over_flg = H265_OFF;
	OS_User_Sig_Sem(SID_H265_STREAM_INT);
	
	return H265_OK;
}


/**
 *	@brief		Checked whether Stream Area of HEVC went around.\n
 *	@param		i/o:video_info		Video Informations\n
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_stream_around_check( E_H265_IF_STREAM_CH stream_ch, T_H265_ENC_VIDEO_INFO* video_info, T_SR_ENC_VINFO vstrm_info )
{
#ifndef CO_DEBUG_ON_PC
	ULONG				sample_end_address;
	UINT32				check_size;
	UINT32				size_before;
	UINT32				size_after;
	ULONG				strm_start_address;
	ULONG				strm_end_address;
	ULONG				stream_work_address;
	UINT32				stream_work_size;
	
	sample_end_address	= vstrm_info.frame_end_veb_pointer;
	
	if( gH265_Rec_Param.codec_param[stream_ch].stream_format != E_H265_IF_STREAM_FORMAT_PES ){
		check_size			= vstrm_info.picture_size;
	}
	else{
		check_size			= vstrm_info.picture_size + 64;
	}
	
	strm_start_address	= Sdram_Map_Movie_Record_Get_H265_Stream_Start_Address( stream_ch );
	
	if( (sample_end_address - check_size) < strm_start_address ){
		// Init address
		strm_end_address	= Sdram_Map_Movie_Record_Get_H265_Stream_End_Address( stream_ch );
		stream_work_address	= Sdram_Map_Movie_Record_Get_H265_Stream_Work( stream_ch, &stream_work_size );	// case HEVC, PES Header Area Include copy area.
		
		size_after = sample_end_address - strm_start_address;
		size_before = check_size - size_after;
		
		H265_Movie_Cache_Flush( (ULONG)(strm_end_address - size_before), (ULONG)size_before );
		H265_Movie_Cache_Flush( (ULONG)strm_start_address, (ULONG)size_after );
		
		H265_Movie_Memcpy( (UCHAR*)stream_work_address, (UCHAR*)(strm_end_address - size_before), size_before );
		H265_Movie_Memcpy( (UCHAR*)(stream_work_address + size_before), (UCHAR*)strm_start_address, size_after );
		
		if( gH265_Rec_Param.codec_param[stream_ch].stream_format != E_H265_IF_STREAM_FORMAT_PES ){
			video_info->sample_address	= stream_work_address;
		}
		else{
			video_info->sample_address	= stream_work_address + 64;		// sample_address is except PES area.
		}
		video_info->sample_size		= vstrm_info.picture_size;
		
		H265_Movie_Cache_Clean( stream_work_address, vstrm_info.picture_size );
		
		if(check_size > stream_work_size ){
			H265_Info_Print(("---  H265_ENT: STRM_WORK_AREA check_size=%x\n",check_size));
		}
	}
	else{
		video_info->sample_address	= sample_end_address - vstrm_info.picture_size;
		video_info->sample_size		= vstrm_info.picture_size;
		
		H265_Movie_Cache_Flush( (sample_end_address - vstrm_info.picture_size), vstrm_info.picture_size );
	}
#endif
}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---

/**
 *	@brief		It sets Sirius Parameters for Frame Encode
 *	@param		stream_ch		Stream CH(Sirius CH)
 *	@param		enc_param		ENC_PARAM table pointer
 *	@param		fm_table		FM_TABLE table pointer
 *	@param		frame_no		frame number
 *	@param		gop_cnt			gop counter for Picture Type
 *	@return		H265_OK			Normal End
 *	@return		H265_OK			Abnormal End
 *	@note		None
 *	@attention	None
 */
static INT32 h265_encode_wave_enc_one_frame( E_H265_IF_STREAM_CH stream_ch, INT32 stop_req_flag, INT32 *vop_cnt, INT32 *gop_cnt, INT32* delay_flag, INT32* end_flag )
{
	OS_USER_ER		ercd;
	INT32			ret;
	INT32			i;
	UINT32			src_index = 0;
	UINT32			nal_report_addr, nal_report_size;
	ULLONG			frame_no = 0;
	EncParam		encParam;
	EncOutputInfo	encOutParam;
	T_H265_IF_GET_IMAGE_ADDR_PARAM addr_param;
	HevcSEIDataEnc			seiDataEnc;
	INT32					seiByteSize = 0;
	UINT32			tmp_addr;
	UINT32			tmp_size;
	INT32			buf_ctrl_param_flag = 0;
	BOOL			insert_idr_flag = FALSE;
	
	memset(&addr_param, 0x00, sizeof(addr_param));
	
	*end_flag = H265_OFF;
	insert_idr_flag = FALSE;
	
	if( gH265_Codec_Already_Stop_Req_Flag[stream_ch] == H265_OFF ){
		OS_User_Wai_Sem(SID_H265);
		
		ret = h265_encode_Vin_get_decide_bank( stream_ch, (INT32*)&src_index, &frame_no );
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "VencGet %u-%u", stream_ch, (UINT32)frame_no ));
		if( ret == H265_ENC_VIN_CTL_NONEDATA ){
			if( gH265_Vin_Adr_Ctl[stream_ch].decide_read_point == 0 ){
				h265_encode_Vin_set_decide_bank( stream_ch, gH265_Vin_Adr_Ctl[stream_ch].decide_bank[ gH265_Vin_Adr_Ctl[stream_ch].max_bank_num - 1 ], gH265_Vin_Adr_Ctl[stream_ch].decide_frame_no[ gH265_Vin_Adr_Ctl[stream_ch].max_bank_num - 1 ] );
			}
			else{
				h265_encode_Vin_set_decide_bank( stream_ch, gH265_Vin_Adr_Ctl[stream_ch].decide_bank[ gH265_Vin_Adr_Ctl[stream_ch].decide_read_point - 1 ], gH265_Vin_Adr_Ctl[stream_ch].decide_frame_no[ gH265_Vin_Adr_Ctl[stream_ch].decide_read_point - 1 ] );
			}
			h265_encode_Vin_get_decide_bank( stream_ch, (INT32*)&src_index, &frame_no );
			if( gH265_ViewLater_Count == 0 ){
				H265_Warning_Print(( "H265 View task later!! ch[%d] frame[%d] (timming NG) \n", stream_ch, (UINT32)frame_no ));
			}
			gH265_ViewLater_Count++;
			gH265_ViewLater_Total_Count++;
			if( gH265_ViewLater_Count > D_H265_ENCODE_LATER_THRESHOLD ){
				gH265_ViewLater_Count = 0;
			}
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265 View later %u-%u", stream_ch, (UINT32)frame_no ));
			
			H265_If_Record_Notify_RepeatFrame( stream_ch );
			
			gH265_Vin_Adr_Ctl[stream_ch].duplicate_check_tbl[src_index]++;
		}
		OS_User_Sig_Sem(SID_H265);
		
		Sdram_Map_Movie_Record_Get_H265_Vin( stream_ch, src_index, &addr_param );
	}
	else{
		// dummy parameter set.
		src_index				= 0;
		addr_param.y_cmp_addr	= 0;
		addr_param.c_cmp_addr	= 0;
	}
	
	memset(&encParam, 0x00, sizeof(EncParam));
	encParam.skipPicture						= 0;
	encParam.quantParam							= 0;
	encParam.forcePicQpEnable					= 0;
	encParam.forcePicQpI						= 0;
	encParam.forcePicQpP						= 0;
	encParam.forcePicQpB						= 0;

	if( gH265_Insert_IDR_Flag[stream_ch] == H265_ON ){
		H265_Info_Print(("--- !! [H265] Insert IDR Picture [%d] !!\n", stream_ch ));
		switch( gH265_Rec_Param.codec_param[stream_ch].gop_struct ){
			case E_H265_IF_GOP_STR_IIII:
			case E_H265_IF_GOP_STR_IPPP:
				*vop_cnt = 1;
				*gop_cnt = 1;
				insert_idr_flag = TRUE;
				break;
			case E_H265_IF_GOP_STR_IBBP:
				if( (*vop_cnt % 3) == 1 ){
					*vop_cnt = 1;
					*gop_cnt = 1;
				}
				else if( *vop_cnt == 3 ){
					insert_idr_flag = TRUE;
				}
				H265_Warning_Print(("+++ H265 Insert IDR Not Support in IBBP struct. ch[%d]\n", stream_ch ));
				break;
			default :
				break;
		}
		if( insert_idr_flag == TRUE ){
			encParam.forcePicTypeEnable = 1;
			encParam.forcePicType		= 3;
			gH265_Insert_IDR_Flag[stream_ch] = H265_OFF;
			gH265_GOP_Change_Flag[stream_ch] = H265_ON;
//			printf(">>>>>>> gH265_Insert_IDR_Flag[%02d] = H265_OFF <<<<<\n", stream_ch );
		}
		else{
			if( gH265_Rec_Param.codec_param[stream_ch].gop_struct == E_H265_IF_GOP_STR_IBBP ){
			encParam.forcePicTypeEnable 			= 0;
				encParam.forcePicType					= 0;
		}
			else{
				encParam.forcePicTypeEnable 			= 0;
				encParam.forcePicType					= 0;
			}
		}
	}
	else {
		if( *vop_cnt == 1 ){
			// IDR frame
			encParam.forcePicTypeEnable				= 0;
			encParam.forcePicType					= 0;
		}
		else{
			if( gH265_Rec_Param.codec_param[stream_ch].gop_struct == E_H265_IF_GOP_STR_IBBP ){
			encParam.forcePicTypeEnable				= 0;
				encParam.forcePicType					= 0;
			}
			else{
				encParam.forcePicTypeEnable				= 0;
				encParam.forcePicType					= 0;
			}
		}
	}
	
#if 0	// old
	if( vop_cnt == 1 ){
		if( gop_cnt == 1 ){
			// IDR frame
			encParam.forcePicTypeEnable			= 1;
			encParam.forcePicType				= 3;
		}
		else{
			// I frame
			encParam.forcePicTypeEnable			= 1;
			encParam.forcePicType				= 0;
		}
	}
	else{
		encParam.forcePicTypeEnable				= 0;
		encParam.forcePicType					= 0;
	}
#endif
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "i %d-%d", encParam.forcePicTypeEnable, encParam.forcePicType ));
	
	encParam.codeOption.implicitHeaderEncode	= 1;      // FW will encode header data implicitly when changing the header syntaxes
	encParam.codeOption.encodeAUD				= 0;
	
	// fcParam
	encParam.fcParam.powerDownMode0				= 0;
	encParam.fcParam.powerDownMode1				= 0;
	encParam.fcParam.ddrCh1StartAddrUmc			= 0;
	if( gH265_Rec_Param.codec_param[stream_ch].compression_enable == H265_ON  ){
		encParam.fcParam.thModeTdc1				= 0;
		encParam.fcParam.thModeTdc0				= 0;
		encParam.fcParam.modeSelTdc1			= 1;
		encParam.fcParam.modeSelTdc0			= 1;
		encParam.fcParam.endianMode				= 0;
		encParam.fcParam.ddrReadMasterSelTdc	= 1;
	}
	else{
		encParam.fcParam.thModeTdc1				= 1;
		encParam.fcParam.thModeTdc0				= 1;
		encParam.fcParam.modeSelTdc1			= 0;
		encParam.fcParam.modeSelTdc0			= 0;
		encParam.fcParam.endianMode				= 1;
		encParam.fcParam.ddrReadMasterSelTdc	= 0;
	}
	if( gH265_Codec_Ddr_Ch_Select.fcPrpCh == DDR_CHANNEL_0 ){
		encParam.fcParam.ddrReadTdcCh			= 0;
		encParam.fcParam.pxtDdrChSel			= 0;
	}
	else{
		encParam.fcParam.ddrReadTdcCh			= 1;
		encParam.fcParam.pxtDdrChSel			= 1;
	}
	encParam.fcParam.ddrCh0StartAddrUmc			= 0;
	encParam.fcParam.tdcTableAddrY				= addr_param.y_cmp_addr;
	encParam.fcParam.tdcTableAddrC				= addr_param.c_cmp_addr;
	
	encParam.srcIdx								= src_index;
	if( stop_req_flag == H265_ON ){
		encParam.codeOption.encodeEOS	= 1;
		encParam.codeOption.encodeEOB	= 1;
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "EOS!" ));

		if( gH265_Codec_Already_Stop_Req_Flag[stream_ch] == TRUE ){
			encParam.srcEndFlag			= 1;
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "EOF!" ));
			H265_Info_Print(( "h265_encode_wave_enc_one_frame: set encParam.srcEndFlag = 1\n" ));
		}
		gH265_Codec_Already_Stop_Req_Flag[stream_ch] = TRUE;
	}
	
	encParam.sourceFrame						= (FrameBuffer*)&gH265_YC_Buffer[stream_ch][src_index];
	encParam.sourceFrame->sourceLBurstEn		= 0;
	
	encParam.serialNum							= frame_no;
	encParam.pts								= gH265_Enc_Ctrl.video[stream_ch].total_time_increment;
	encParam.picStreamBufferRdPtrGapSize		= D_H265_ENCODE_WAVE_STREAM_GAP_SIZE;	// stream GAP for PES or VPS/SPS/PPS
	
	Sdram_Map_Movie_Record_Get_H265_Stream_Bank( stream_ch, gH265_Codec_Enc_Core_Idx[stream_ch], (UINT32*)&encParam.picStreamBufferAddr, (UINT32*)&encParam.picStreamBufferSize );
	
	encParam.nalSizeReportEnable = 1;
	
	ret = H265_Wave_EncGiveCommand( stream_ch, gH265_Codec_Handle[stream_ch], ENC_SET_TEMP_QUEUE_IDX, (void*)&gH265_Codec_Enc_Core_Idx[stream_ch] );	//only two temp buffer
	if (ret != RETCODE_SUCCESS) {
		H265_Err_Print(("H265_Wave_EncGiveCommand(ENC_SET_TEMP_QUEUE_IDX) error (%d)\n", ret ));
		H265_Wave_EncClose( stream_ch, gH265_Codec_Handle[stream_ch] );
		return H265_NG;
	}
	
	if( gH265_Codec_Already_Stop_Req_Flag[stream_ch] == H265_OFF ){
		if( (gH265_Rec_Param.codec_param[stream_ch].pictstruct_flag) ||
			(gH265_Rec_Param.codec_param[stream_ch].buffer_ctrl_flag) ){
			
			if( (gH265_Rec_Param.codec_param[stream_ch].buffer_ctrl_flag) &&
				(*vop_cnt == 1) 											  ){
				buf_ctrl_param_flag = 1;
			}
			memset( &seiDataEnc, 0x00, sizeof(seiDataEnc) );
			
			seiDataEnc.prefixSeiNalEnable = 1;
			seiDataEnc.prefixSeiDataEncOrder = 0;
			Sdram_Map_Movie_Record_Get_H265_SEI( stream_ch, src_index, &tmp_addr, &tmp_size );
			
				seiDataEnc.prefixSeiNalAddr = tmp_addr;
			
			seiByteSize = H265_Wave_EncodePrefixSEI( gH265_Rec_Param.codec_param[stream_ch].pictstruct_flag,
													 (T_H265_WAVE_SEI_ACTIVE_PARAMETER*)&gH265_Codec_Active_Parameter_Sei[stream_ch],
													 (T_H265_WAVE_SEI_PIC_TIMING*)&gH265_Codec_Pic_Timing_Sei[stream_ch],
													 buf_ctrl_param_flag,
													 (T_H265_WAVE_SEI_BUFFERING_PERIOD*)&gH265_Codec_Buffering_Period_Sei[stream_ch],
													 (hrd_t*)&gH265_Codec_HDR_Info[stream_ch],
													 (UINT8*)tmp_addr );
			
			gH265_Codec_Output_NAL_Info[stream_ch][gH265_Codec_Output_NAL_Info_Sei_Pointer[stream_ch]].NAL_size[4] = gH265_Codec_Active_Parameter_Sei[stream_ch].size;		// Active Parameter SEI
			gH265_Codec_Output_NAL_Info[stream_ch][gH265_Codec_Output_NAL_Info_Sei_Pointer[stream_ch]].NAL_size[5] = gH265_Codec_Pic_Timing_Sei[stream_ch].size;			// Picture Timing SEI
			gH265_Codec_Output_NAL_Info[stream_ch][gH265_Codec_Output_NAL_Info_Sei_Pointer[stream_ch]].NAL_size[6] = gH265_Codec_Buffering_Period_Sei[stream_ch].size;		// Buffering Period SEI
			
			gH265_Codec_Output_NAL_Info_Sei_Pointer[stream_ch]++;
			
			if( gH265_Codec_Output_NAL_Info_Sei_Pointer[stream_ch] >= D_H265_ENCODE_VINFO_FIFO_MAX ){
				gH265_Codec_Output_NAL_Info_Sei_Pointer[stream_ch] = 0;
			}
			
			seiDataEnc.prefixSeiDataSize = seiByteSize;
			if( seiByteSize != 0 ){
				H265_Movie_Cache_Clean( tmp_addr, seiByteSize );
				ret = H265_Wave_EncGiveCommand( stream_ch, gH265_Codec_Handle[stream_ch], ENC_SET_SEI_NAL_DATA, &seiDataEnc );
				if (ret != RETCODE_SUCCESS) {
				H265_Err_Print(("H265_Wave_EncGiveCommand(ENC_SET_SEI_NAL_DATA) error (%d) ch[%d]\n", ret, stream_ch ));
					H265_Wave_EncClose( stream_ch, gH265_Codec_Handle[stream_ch] );
					return H265_NG;
				}
			}
		}
		
		if( (*vop_cnt == 1) && (gH265_Dynamic_Change_Flag[stream_ch] == H265_ON) ){
			EncChangeParam	param;
			if( gH265_Rec_Param.codec_param[stream_ch].bitrate > gH265_Dynamic_Param[stream_ch].bitrate ){
				gH265_Rec_Param.codec_param[stream_ch].average_bitrate = gH265_Dynamic_Param[stream_ch].bitrate;
			}
			else{
				H265_Warning_Print(("+++ H265 Dynamic parameter bitrat is over MAX bitrate. ch[%d]\n", stream_ch ));
			}
			
			param.changeParaMode = OPT_COMMON;
			param.enable_option  = (ENC_RC_TARGET_RATE_CHANGE | ENC_RC_TRANS_RATE_CHANGE | ENC_INTRA_PARAM_CHANGE);
			if( gH265_Rec_Param.codec_param[stream_ch].rate_mode == E_H265_IF_RATE_MODE_CBR ){
				param.bitRate		= gH265_Rec_Param.codec_param[stream_ch].bitrate;
				param.transRate		= gH265_Rec_Param.codec_param[stream_ch].bitrate;
			}
			else if( gH265_Rec_Param.codec_param[stream_ch].rate_mode == E_H265_IF_RATE_MODE_VBR ){
				param.bitRate		= gH265_Rec_Param.codec_param[stream_ch].average_bitrate;
				param.transRate		= gH265_Rec_Param.codec_param[stream_ch].bitrate;
			}
			else{
				param.bitRate		= gH265_Rec_Param.codec_param[stream_ch].average_bitrate;
				param.transRate		= 0;
			}
			
			// CBR
			// bitRate == transRate
			//
			// VBR
			// bitRate   : ave_bitrate
			// transRate : max_bitrate
			

			// GOP number
			gH265_Rec_Param.codec_param[stream_ch].gop_num	= gH265_Dynamic_Param[stream_ch].frame_num_of_gop;
			param.intraPeriod								= (int)gH265_Rec_Param.codec_param[stream_ch].gop_num;

			ret = H265_Wave_EncGiveCommand( stream_ch, gH265_Codec_Handle[stream_ch], ENC_SET_PARA_CHANGE, (void *)&param );
			if (ret != RETCODE_SUCCESS) {
				H265_Err_Print(("H265_Wave_EncGiveCommand(ENC_SET_PARA_CHANGE) error (%d) ch[%d]\n", ret, stream_ch ));
				H265_Wave_EncClose( stream_ch, gH265_Codec_Handle[stream_ch] );
				return H265_NG;
			}
			gH265_Dynamic_Change_Flag[stream_ch] = H265_OFF;
		}
		else if((*vop_cnt == 1) && (gH265_GOP_Change_Flag[stream_ch] == H265_ON)){ // dummy to make gop reset
			EncChangeParam	param;
			
			param.changeParaMode = OPT_COMMON;
			param.enable_option  = ENC_INTRA_PARAM_CHANGE;

			// GOP number
			param.intraPeriod	= (int)gH265_Rec_Param.codec_param[stream_ch].gop_num;

			ret = H265_Wave_EncGiveCommand( stream_ch, gH265_Codec_Handle[stream_ch], ENC_SET_PARA_CHANGE, (void *)&param );
			if (ret != RETCODE_SUCCESS) {
				H265_Err_Print(("H265_Wave_EncGiveCommand(ENC_SET_PARA_CHANGE) error (%d) ch[%d]\n", ret, stream_ch ));
				H265_Wave_EncClose( stream_ch, gH265_Codec_Handle[stream_ch] );
				return H265_NG;
			}
			gH265_GOP_Change_Flag[stream_ch] = H265_OFF;
		}
	}
	
#if 0	// ICE Debug
	printf("break point");

	H265_Movie_Cache_Clean_Flush( (ULONG)0x71FE0000, 0x8000 );
	
	H265_Movie_Cache_Clean_Flush( (ULONG)0x6BA44000, 0xCC0000 );
#endif
	
	MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_50, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "%d", (INT32)frame_no )); //gnso
	// Encode Request
	ret = H265_Wave_EncStartOneFrame( stream_ch, gH265_Codec_Handle[stream_ch], &encParam );
	if( ret != RETCODE_SUCCESS ){
		H265_Err_Print(("H265_Wave_EncStartOneFrame error (%d) ch[%d]\n", ret, stream_ch ));
		return H265_NG;
	}
	
	// Wait Interrupt
	{
		INT32	timeout = -1;
		INT32	timeout_count = 0;
		INT32	int_reason = 0;
		
		UINT32 readPtr=0, writePtr=0, sizePtr=0, sliceNum=0;
		UINT32 sliceAddr=0;			// update encOutParam.bitstreamBuffer for slice
		UINT32 sliceSize=0;			// update encOutParam.bitstreamSize for slice
		UINT32 accSliceSize=0;	// size of slice 0+1+2
		UINT32 slice0SizePtr=0;	// bitstream size return from H265_Wave_EncGetBitstreamBuffer() when slice 0
		UINT32 slice1SizePtr=0;	// bitstream size return from H265_Wave_EncGetBitstreamBuffer() when slice 1
		UINT32 gopNum = gH265_Rec_Param.codec_param[stream_ch].gop_num;
		
#define ENCODE_START_ADDRESS	(SDRAM_ADR_MOVIE_RECORD0_WAVE_0_STREAM_0)
#define ENCODE_END_ADDRESS		(SDRAM_ADR_MOVIE_RECORD0_WAVE_0_STREAM_0+SDRAM_SIZ_MOVIE_RECORD0_WAVE_0_STREAM)
#define ENCODE_GAP_SIZE 		(D_H265_ENCODE_WAVE_STREAM_GAP_SIZE)
#define ENCODE_WRAP_THRESHOLD	(ENCODE_END_ADDRESS-0x9000-ENCODE_GAP_SIZE)
		
#define DBG_SLICE_INFO_LEVEL 0	// 1: wrap-around info only, 2: wrap-around info & slice info

		while( 1 ){
			int_reason = H265_Wave_WaitInterrupt( stream_ch, 0, timeout );
			MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_51, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "0x%X", (UINT32)int_reason )); //gnso
			if( int_reason == -1 ){
				if( (timeout_count * timeout) > 16 ){
					H265_Err_Print(( "H265_Wave_WaitInterrupt Timeout ch[%d]\n", stream_ch ));
					return H265_NG;
				}
				int_reason = 0;
				timeout_count++;
			}
			if( int_reason & (1<<INT_BIT_BIT_BUF_FULL) ){
				H265_Info_Print(( "h265_encode_wave_enc_one_frame: int_reason & (1<<INT_BIT_BIT_BUF_FULL) ch[%d]\n", stream_ch ));
				return H265_NG;
			}
			if( int_reason ){
				H265_Wave_ClearInterrupt( stream_ch, 0 );
				
				if ( (int_reason==0x8) || (int_reason==0x80) ) {
					// slice INT
					if ( (int_reason==0x80) )  {
						encOutParam.reconFrameIndex = 0;
						MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_53, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "%d", (INT32)encOutParam.reconFrameIndex )); //gnso
					}

					// frame INT
					if (int_reason==0x8) {
						ret = H265_Wave_EncGetOutputInfo( stream_ch, gH265_Codec_Handle[stream_ch], &encOutParam );
						MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_52, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "get %d %d %d", (INT32)encOutParam.reconFrameIndex, (INT32)encOutParam.serialNum, (INT32)encOutParam.picType )); //gnso
						if (ret != RETCODE_SUCCESS) {
							H265_Err_Print(("H265_Wave_EncGetOutputInfo error (%d) ch[%d]\n", ret, stream_ch ));
							return H265_NG;
						}
					}
					
					if (gH265_Slice_Encode_En) {
						// Free Video Stream
						h265_encode_free_vstream();

						H265_Wave_EncGetBitstreamBuffer(gH265_Codec_Handle[stream_ch], &readPtr, &writePtr, &sizePtr);
						writePtr |= SDRAM_ADR_CH0_TOP;
#if (DBG_SLICE_INFO_LEVEL>=2)
						printf("\033[1;33m get W= 0x%08X (0x%X) \033[0;0m\n", writePtr, sizePtr);
#endif
						
						switch(sliceNum) {
							// slice 0
							case 0:
								sliceSize = sizePtr - gH265_Prev_Frame_Size;
								sliceAddr = writePtr - sliceSize;
								slice0SizePtr = sizePtr;
								break;
							// slice 1
							case 1:
								sliceSize = sizePtr - slice0SizePtr;
								sliceAddr = writePtr - sliceSize;
								slice1SizePtr = sizePtr;
								break;
							// slice 2
							case 2:
								sliceSize = sizePtr - slice1SizePtr;
								sliceAddr = writePtr - sliceSize;
								break;
						}
						
						// slice 3 (frame end)
						/* keep previous frame info */
						if (int_reason==0x8) {
							// 1st frame, size=0
							if (gH265_Prev_Frame_Aligned_Address==0) {
								sliceSize = 0;
								sliceAddr = ENCODE_START_ADDRESS;
								gH265_Prev_Frame_Aligned_Address = ((sliceAddr + sizePtr +ENCODE_GAP_SIZE)+0xF) & (~0xF);
								gH265_Prev_Frame_Padding = gH265_Prev_Frame_Aligned_Address - (sliceAddr + sliceSize);
								gH265_Prev_Frame_Size = gH265_Prev_Frame_Padding;
							}
							else {
								sliceSize = encOutParam.bitstreamSize - accSliceSize;
								sliceAddr = writePtr - sliceSize;
								gH265_Prev_Frame_Aligned_Address = ((sliceAddr + sliceSize +ENCODE_GAP_SIZE)+0xF) & (~0xF);
								gH265_Prev_Frame_Padding = gH265_Prev_Frame_Aligned_Address - (sliceAddr + sliceSize);
								gH265_Prev_Frame_Size = sizePtr + gH265_Prev_Frame_Padding;
							}
							gH265_Prev_Frame_PicCnt	= encOutParam.encPicCnt;
						}
						/* update slice info */
						else {
							encOutParam.encPicCnt = gH265_Prev_Frame_PicCnt+1;
							if( gH265_Rec_Param.codec_param[stream_ch].gop_struct == E_H265_IF_GOP_STR_IPPP ){
								encOutParam.encPicPoc = (encOutParam.encPicCnt % gopNum) -1;
								if (encOutParam.encPicPoc<0)
									encOutParam.encPicPoc = gopNum -1;
								encOutParam.picType = ( (encOutParam.encPicCnt-1)%gopNum )? PIC_TYPE_P : PIC_TYPE_IDR;
								encOutParam.reconFrameIndex = (encOutParam.encPicCnt % 4) -1;
								if (encOutParam.reconFrameIndex<0)
									encOutParam.reconFrameIndex = 3;
							}
						}

						/* wrap-around */
						encOutParam.bitstreamWrapAround = 0;
						if (writePtr < gH265_Prev_Slice_WAddr) {

							// condition 1. FRAME write pointer > 0x5CEEEE00, next frame will start at 0x5BDF8200
							if ( (sliceNum==0) && (gH265_Prev_Slice_WAddr > ENCODE_WRAP_THRESHOLD) ) {
								sliceAddr = ENCODE_START_ADDRESS+ENCODE_GAP_SIZE;
								sliceSize = writePtr - sliceAddr;
#if (DBG_SLICE_INFO_LEVEL>=1)
							printf("\033[0;42m condition 1. frame start at 0x5BDF8200 \033[0;0m\n");
							printf("\033[1;33m wAddr= 0x%08X, prev= 0x%08X \033[0;0m\n", writePtr, gH265_Prev_Slice_WAddr);
							printf("\033[1;36m make slice%d= 0x%08X +0x%X= 0x%08X\033[0;0m\n", sliceNum, sliceAddr, sliceSize, sliceAddr+sliceSize);
							printf("\n\n\n");
#endif
							}
							// condition 2. buffer overflow, set bitstreamWrapAround = 1
							else {
								UINT32 address_wrap = gH265_Prev_Slice_WAddr;
								UINT32 size_before = 0;

								for (; address_wrap<=(ENCODE_END_ADDRESS-0x2000);) {
									address_wrap += ENCODE_GAP_SIZE;
									size_before += ENCODE_GAP_SIZE;
								}
								
								sliceAddr = gH265_Prev_Slice_WAddr;	// address_before
								sliceSize = writePtr - ENCODE_START_ADDRESS + size_before; // total size
								
								encOutParam.bitstreamWrapAround = 1;
								encOutParam.bitstreamWrapAroundPtr = address_wrap;
#if (DBG_SLICE_INFO_LEVEL>=1)
								printf("\033[0;42m condition 2. set bitstreamWrapAround = 1 \033[0;0m\n");
								printf("\033[1;33m wAddr= 0x%08X, prev= 0x%08X \033[0;0m\n", writePtr, gH265_Prev_Slice_WAddr);
								printf("\033[1;36m make slice%d= 0x%08X +0x%X= 0x%08X\033[0;0m\n", sliceNum, sliceAddr, sliceSize, sliceAddr+sliceSize);
								printf("\033[1;31m before= 0x%08X +0x%X (0x%08X) \033[0;0m\n", gH265_Prev_Slice_WAddr, size_before, gH265_Prev_Slice_WAddr+size_before);
								printf("\033[1;31m after = 0x%08X +0x%X (0x%08X) \033[0;0m\n", ENCODE_START_ADDRESS, writePtr-ENCODE_START_ADDRESS, writePtr);
								printf("\n\n\n");
#endif
							}
						}
						
						gH265_Prev_Slice_WAddr = writePtr;
						accSliceSize += sliceSize;
						
#if (DBG_SLICE_INFO_LEVEL>=2)
						printf("\033[1;36m make slice%d= 0x%08X +0x%X= 0x%08X\033[0;0m\n", sliceNum, sliceAddr, sliceSize, sliceAddr+sliceSize);
						if (int_reason==0x8) {
							printf("\033[1;31m frame, buffer= 0x%08X +0x%X +0x%X\n= 0x%08X \033[0;0m\n\n", 
								encOutParam.bitstreamBuffer, encOutParam.bitstreamSize, gH265_Prev_Frame_Padding,
								encOutParam.bitstreamBuffer+encOutParam.bitstreamSize+gH265_Prev_Frame_Padding);
						}
#endif
						encOutParam.retEncSizeFiller = encOutParam.encPicCnt-2;
						encOutParam.bitstreamBuffer = sliceAddr;
						encOutParam.bitstreamSize = sliceSize;
						encOutParam.numOfSlices = sliceNum;
						
						sliceNum++;
					}

					if( encOutParam.reconFrameIndex == -5 ){ 
						gH265_Codec_Ready_To_Pallalel[stream_ch] = TRUE; // bReadyToParalle means that wave420 firmware is ready to start multiple core run
					}
					if( gH265_Codec_Handle[stream_ch]->CodecInfo->encInfo.openParam.numCores > 1 ){
						if( gH265_Codec_Ready_To_Pallalel[stream_ch] == TRUE ){
							//	bReadyToParalle means that wave420 firmware is ready to start multiple core run
							gH265_Codec_Enc_Core_Idx[stream_ch] = (gH265_Codec_Enc_Core_Idx[stream_ch]+1)%gH265_Codec_Handle[stream_ch]->CodecInfo->encInfo.openParam.numCores;
						}
					}
	
					// enc delay?
					if( (encOutParam.reconFrameIndex == -2) || (encOutParam.reconFrameIndex == -5) ){
						H265_Info_Print(( "Delay ch[%d]\n", stream_ch ));
						
						// Output Delay for Bframe
						*delay_flag = H265_ON;

						OS_User_Set_Flg( FID_ENCODER, D_BF_FLG_START_SYNC_H265 );
						H265_Info_Print(( "H265 sync Start ch[%d]\n", stream_ch ));
					}
					else{
						if( encOutParam.reconFrameIndex != -4 ){
							//to remove duplicated last pic-run
							H265_Wave_Queue_Enqueue( (T_H265_WAVE_QUEUE*)gH265_Codec_Queue_Ctrl[encOutParam.encInstIdx], (VOID*)&encOutParam );
						}
						
						if( H265_Wave_Queue_Get_Cnt( (T_H265_WAVE_QUEUE*)gH265_Codec_Queue_Ctrl[gH265_Codec_Handle[stream_ch]->instIndex]) != 0 ) {
							EncOutputInfo	*temp;
							temp = (EncOutputInfo*)H265_Wave_Queue_Dequeue( (T_H265_WAVE_QUEUE*)gH265_Codec_Queue_Ctrl[gH265_Codec_Handle[stream_ch]->instIndex] );
							memcpy( &encOutParam, temp, sizeof(encOutParam));
						}
						else{
							H265_Info_Print(( "Queue none ch[%d]\n", stream_ch ));
							
							// Output nothing
							*delay_flag = H265_ON;
							return H265_OK;
						}
						
						H265_Wave_EncGiveCommand( stream_ch, gH265_Codec_Handle[stream_ch], ENC_UPDATE_PIC_OUTPUT_INFO, &encOutParam );
						if (gH265_Slice_Encode_En) {
							encOutParam.pts = (UINT64)encOutParam.encPicCnt-1;
						}
						encOutParam.serialNum = (UINT64)frame_no;
						
						MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_52, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "get %d %d %d", (INT32)encOutParam.reconFrameIndex, (INT32)encOutParam.serialNum, (INT32)encOutParam.picType ) ); //gnso
						*delay_flag = H265_OFF;
				
						if( encOutParam.reconFrameIndex == -1 ) {
							
							*end_flag = H265_ON;
							gH265_Map_Enable_Flg[stream_ch] = H265_OFF;
							gH265_Map_First_Flg[stream_ch] = H265_ON;
							H265_Info_Print(( "reconFrameIndex is -1. ch[%d]\n", stream_ch ));
							
							// Stream notificition for terminate.
							OS_User_Wai_Sem(SID_H265_STREAM_INT);
							if( gH265_Vstrm_Fifo[stream_ch].fifo_over_flg == H265_ON ){
								H265_Err_Print(("--- H265 VstrmCH[%d] Fifo Over Flow!!\n", stream_ch ));
							}
							OS_User_Sig_Sem(SID_H265_STREAM_INT);
							memcpy( (void*)&gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point], (const void*)&encOutParam, sizeof(encOutParam) );
							
							OS_User_Wai_Sem(SID_H265_STREAM);
							if( gH265_Codec_Handle[stream_ch]->CodecInfo->encInfo.openParam.numCores > 1 ){
								gH265_Codec_Stream_Wpoint_Notify[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point] = gH265_Stream_Buffer_WritePtr[encOutParam.encodedCoreIdx];
							}
							else{
								gH265_Codec_Stream_Wpoint_Notify[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point] = gH265_Stream_Buffer_WritePtr[stream_ch];
							}
							OS_User_Sig_Sem(SID_H265_STREAM);
							
							OS_User_Wai_Sem(SID_H265_STREAM_INT);
							gH265_Vstrm_Fifo[stream_ch].w_point++;
							if( gH265_Vstrm_Fifo[stream_ch].w_point >= D_H265_ENCODE_VINFO_FIFO_MAX ){
								gH265_Vstrm_Fifo[stream_ch].w_point = 0;
							}
							
							if( gH265_Vstrm_Fifo[stream_ch].w_point == gH265_Vstrm_Fifo[stream_ch].r_point ){
								gH265_Vstrm_Fifo[stream_ch].fifo_over_flg = H265_ON;
							}
							OS_User_Sig_Sem(SID_H265_STREAM_INT);
							return H265_OK;
						}
						
						// Enc Param Free
						ercd = OS_User_Twai_Sem(SID_H265, H265_ENC_SEMAPHORE_TIMEOUT);
						if( ercd != D_OS_USER_E_OK ){
							H265_Err_Print(( "--- h265_encode_wave_enc_one_frame: Fm semaphore NG ch[%d]\n", stream_ch ));
							OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
						}
						
						if (gH265_Slice_Encode_En) {
							if (int_reason==0x8) {
								h265_encode_Vin_set_empty_bank( stream_ch, encOutParam.encSrcIdx );
							}
						}
						else {
							h265_encode_Vin_set_empty_bank( stream_ch, encOutParam.encSrcIdx );
						}

						OS_User_Sig_Sem(SID_H265);
						
						OS_User_Wai_Sem(SID_H265_STREAM_INT);
						if( gH265_Vstrm_Fifo[stream_ch].fifo_over_flg == H265_ON ){
							H265_Err_Print(("--- H265 VstrmCH[%d] Fifo Over Flow!!\n", stream_ch ));
						}
						OS_User_Sig_Sem(SID_H265_STREAM_INT);
						
				//		H265_Movie_Cache_Flush( encOutParam.bitstreamBuffer, encOutParam.bitstreamSize );
						memcpy( (void*)&gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point], (const void*)&encOutParam, sizeof(encOutParam) );
						
						{
							unsigned int *ptrNalSize;
							unsigned int slice_count;
#if 0
							unsigned int total_sei_nal_size = 0;
#endif
							
							slice_count = ((encOutParam.numOfSlices >> 16) + (encOutParam.numOfSlices&0xffff));
							//			printf("slice_count %d\n", slice_count);

							Sdram_Map_Movie_Record_Get_H265_NAL_Report( stream_ch, 0, &nal_report_addr, &nal_report_size );
											
							H265_Movie_Cache_Flush( encOutParam.nalSizeReportAddr, nal_report_size );
							ptrNalSize = (unsigned int *)(encOutParam.nalSizeReportAddr + 256);
							
							gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[0] = ptrNalSize[0];	// AUD
							gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[1] = ptrNalSize[1];	// VPS
							gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[2] = ptrNalSize[2];	// SPS
							gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[3] = ptrNalSize[3];	// PPS
#if 0
							// PREFIX SEI check
							if( ptrNalSize[4] != 0 ){
								total_sei_nal_size += gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[4];	// PREFIX SEI : Active Parameter SEI
								total_sei_nal_size += gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[5];	// PREFIX SEI : Picture Timing SEI
								total_sei_nal_size += gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[6];	// PREFIX SEI : Buffering Period SEI
								if( ptrNalSize[4] != total_sei_nal_size ){
									H265_Err_Print(("h265_encode_wave_enc_one_frame : SEI Nal Size Check Error!! ch[%d]\n", stream_ch));
								}
							}
#endif
							for( i=0; i<slice_count; i++ ){
								gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[7 + i] = ptrNalSize[5 + i];	// SLICE  MAX 7
							}
							gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[14] = ptrNalSize[133]; // SUFFIX SEI
							gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[15] = ptrNalSize[134]; // EOS
							gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[16] = ptrNalSize[135]; // EOB
							if( gH265_Rec_Param.codec_param[stream_ch].filler_flag == H265_ON ){
								gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point].NAL_size[17] = encOutParam.retEncSizeFiller;	// Filler
							}
						}
						
						{
							UINT32	stream_size_before;
							UINT32	stream_size;
							UINT32	stream_addr;
							UINT32	tmp_size;
							BOOL	roundup;
							INT32	ret;
							
							if( encOutParam.bitstreamWrapAround == 1 ){
								stream_size_before	= encOutParam.bitstreamWrapAroundPtr - encOutParam.bitstreamBuffer;
								stream_size 		= encOutParam.bitstreamSize - stream_size_before;
								if( gH265_Codec_Handle[stream_ch]->CodecInfo->encInfo.openParam.numCores > 1 ){
									Sdram_Map_Movie_Record_Get_H265_Stream_Bank( stream_ch, encOutParam.encodedCoreIdx, &stream_addr, &tmp_size );
								}
								else{
									Sdram_Map_Movie_Record_Get_H265_Stream_Bank( stream_ch, gH265_Codec_Enc_Core_Idx[stream_ch], &stream_addr, &tmp_size );
								}
								roundup = TRUE;
							}
							else{
								stream_size = encOutParam.bitstreamSize;
								stream_addr = encOutParam.bitstreamBuffer;
								roundup 	= FALSE;
							}
							
							if( stream_addr == 0 ){
								H265_Err_Print(("--- H265[%d] stream addr is 0!!\n", stream_ch ));
							}
							if( gH265_Codec_Handle[stream_ch]->CodecInfo->encInfo.openParam.numCores > 1 ){
								OS_User_Wai_Sem(SID_H265_STREAM);
								if( gH265_Stream_Buffer_WritePtr[encOutParam.encodedCoreIdx] > stream_addr ){
									roundup = TRUE;
								}
								ret = h265_encode_stream_buffer_allocate( encOutParam.encodedCoreIdx, stream_addr, stream_size, roundup );
								gH265_Codec_Stream_Wpoint_Notify[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point] = gH265_Stream_Buffer_WritePtr[encOutParam.encodedCoreIdx];
								OS_User_Sig_Sem(SID_H265_STREAM);
							}
							else{
								OS_User_Wai_Sem(SID_H265_STREAM);
								if( gH265_Stream_Buffer_WritePtr[stream_ch] > stream_addr ){
									roundup = TRUE;
								}
								ret = h265_encode_stream_buffer_allocate( stream_ch, stream_addr, stream_size, roundup );
								gH265_Codec_Stream_Wpoint_Notify[stream_ch][gH265_Vstrm_Fifo[stream_ch].w_point] = gH265_Stream_Buffer_WritePtr[stream_ch];
								OS_User_Sig_Sem(SID_H265_STREAM);
							}
							if( ret != H265_OK ){
								return H265_NG;
							}
						}
						
						OS_User_Wai_Sem(SID_H265_STREAM_INT);
						gH265_Vstrm_Fifo[stream_ch].w_point++;
						if( gH265_Vstrm_Fifo[stream_ch].w_point >= D_H265_ENCODE_VINFO_FIFO_MAX ){
							gH265_Vstrm_Fifo[stream_ch].w_point = 0;
						}
						
						if( gH265_Vstrm_Fifo[stream_ch].w_point == gH265_Vstrm_Fifo[stream_ch].r_point ){
							gH265_Vstrm_Fifo[stream_ch].fifo_over_flg = H265_ON;
						}
						OS_User_Sig_Sem(SID_H265_STREAM_INT);
					}
				}

				if (gH265_Slice_Encode_En) {
					if (sliceSize) {
						if( (*delay_flag != H265_ON) || (gH265_Codec_Already_Stop_Req_Flag[stream_ch] == TRUE) ){
							//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "SYNC"));
							OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSTRM_COMP0 );
						}
					}
				}

				// after whole frame done
				if (int_reason & (1<<INT_WAVE_ENC_PIC)) {
					//H265_Info_Print(( "h265_encode_wave_enc_one_frame: int_reason & (1<<INT_WAVE_ENC_PIC)\n" ));
					break;
				}
			}
		}
	}
	MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_50, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "265" )); //gnso
	
	
	
	return H265_OK;
}

/**
 *	@brief		Get Encoder Result.\n
 *	@param		i/o:video_info		Video Informations
 *	@return		H265_OK				OK - Next Exist
 *				H265_OK_COMPLETE		OK - Read Completed
 *	@return		H265_ERR_INPUT_PARAM	Input Parameter Error
 *	@return		H265_NG				No Sample
 *	@note		None
 *	@attention	None
 */
static INT32 h265_encode_wave_result_get( E_H265_IF_STREAM_CH stream_ch, T_H265_ENC_VIDEO_INFO* video_info )
{
	UINT32				i;
	UINT32				vstrm_flg;
	UINT32				nal_count;
	LLONG				tmp_pts, tmp_dts;
	UINT32				tmp_addr;
	UCHAR				add_header;
	
	if( video_info == NULL ){
		return H265_ERR_INPUT_PARAM;
	}
	
	vstrm_flg = H265_OFF;
	
	for( i=0; i<D_H265_ENC_STREAM_FIFO_COUNT; i++ ){
		if( gH265_Free_Vstream_Ctrl[i].use_flg != H265_ON ){
			gH265_Free_Vstream_Ctrl[i].use_flg			= H265_ON;
			gH265_Free_Vstream_Ctrl[i].ch				= stream_ch;
			if( gH265_Codec_Handle[stream_ch]->CodecInfo->encInfo.openParam.numCores > 1 ){
			gH265_Free_Vstream_Ctrl[i].free_buf_index	= gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].encodedCoreIdx;
			}
			else{
				gH265_Free_Vstream_Ctrl[i].free_buf_index	= stream_ch;
			}
			gH265_Free_Vstream_Ctrl[i].free_addr		= gH265_Codec_Stream_Wpoint_Notify[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point];
			
			vstrm_flg = H265_ON;
			video_info->vstream_index = i;
			break;
		}
	}
	if( vstrm_flg != H265_ON ){
		H265_Err_Print(("--- Vstrm Table Not Empty\n" ));
		return H265_NG;
	}
	
	if( gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].reconFrameIndex == -1 ){
		video_info->stream_end_flg	= H265_ON;
		
		if( gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].bitstreamSize != 0 ){
			H265_Warning_Print(( "h265_encode_wave_result_get : The stream size is not 0 although encoding is completed.\n" ));
		}		
	}
	else{
		video_info->stream_end_flg	= H265_OFF;
	
		h265_encode_wave_stream_around_check( stream_ch, video_info, gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point] );
		
		gH265_Vstrem_Serial_Num[stream_ch]++;
		
		video_info->vstrem_serial_num	= gH265_Vstrem_Serial_Num[stream_ch];
		video_info->frame_no			= gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].serialNum;

		if( gH265_Rec_Param.codec_param[stream_ch].gop_struct == E_H265_IF_GOP_STR_IBBP ){
			if( gH265_Vstrem_Serial_Num[stream_ch] <= (gH265_Rec_Param.codec_param[stream_ch].gop_num-2) ){
				video_info->frame_count_in_gop	= gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].encPicPoc + 1;	// WAVE is 0 start
			}
			else{
				video_info->frame_count_in_gop	= gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].encPicPoc + 3;	// WAVE is 0 start
			}
		}
		else{	// for IPPP
			video_info->frame_count_in_gop	= gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].encPicPoc + 1;	// WAVE is 0 start
		}
		
		if( gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].picType == PIC_TYPE_IDR ){
			video_info->sample_type		= E_H265_IF_PIC_TYPE_IDR;
		}
		else if( gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].picType == PIC_TYPE_I ){
			video_info->sample_type		= E_H265_IF_PIC_TYPE_I;
		}
		else if( gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].picType == PIC_TYPE_P ){
			video_info->sample_type		= E_H265_IF_PIC_TYPE_P;
		}
		else{
			video_info->sample_type		= E_H265_IF_PIC_TYPE_B;
		}
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "g %d", gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].picType ));
		
		video_info->bitrate				= gH265_Rec_Param.codec_param[stream_ch].bitrate;
		video_info->ave_bitrate			= gH265_Rec_Param.codec_param[stream_ch].average_bitrate;

		add_header = 1;
		if (gH265_Slice_Encode_En) {
			if (gH265_Prev_Slice_PicType == gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].picType) // header only be added after 1st slice of each frame
				add_header = 0;
		}
		
		if (add_header) {
			if( gH265_Rec_Param.codec_param[stream_ch].stream_format == E_H265_IF_STREAM_FORMAT_ES ){
				if( gH265_Vstrem_Serial_Num[stream_ch] == 1 ){
					tmp_addr = video_info->sample_address-gH265_Codec_Header_Info[stream_ch].header_size;
					memcpy( (void *)tmp_addr, (const void *)gH265_Codec_Header_Info[stream_ch].header_addr, gH265_Codec_Header_Info[stream_ch].header_size );
					H265_Movie_Cache_Clean_Flush( tmp_addr, gH265_Codec_Header_Info[stream_ch].header_size );
					video_info->sample_address	-= gH265_Codec_Header_Info[stream_ch].header_size;
					video_info->sample_size		+= gH265_Codec_Header_Info[stream_ch].header_size;

					gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].NAL_size[0] = gH265_Codec_Header_Info[stream_ch].AUD_NAL_size;	// AUD
					gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].NAL_size[1] = gH265_Codec_Header_Info[stream_ch].VPS_NAL_size;	// VPS
					gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].NAL_size[2] = gH265_Codec_Header_Info[stream_ch].SPS_NAL_size;	// SPS
					gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].NAL_size[3] = gH265_Codec_Header_Info[stream_ch].PPS_NAL_size;	// PPS
				}
			}
			else{		// PES or ES_HEADER
				if( gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].encPicPoc == 0 ){
					tmp_addr = video_info->sample_address-gH265_Codec_Header_Info[stream_ch].header_size;
					memcpy( (void *)tmp_addr, (const void *)gH265_Codec_Header_Info[stream_ch].header_addr, gH265_Codec_Header_Info[stream_ch].header_size );
					H265_Movie_Cache_Clean_Flush( tmp_addr, gH265_Codec_Header_Info[stream_ch].header_size );
					video_info->sample_address	-= gH265_Codec_Header_Info[stream_ch].header_size;
					video_info->sample_size		+= gH265_Codec_Header_Info[stream_ch].header_size;
					
					gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].NAL_size[0] = gH265_Codec_Header_Info[stream_ch].AUD_NAL_size;	// AUD
					gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].NAL_size[1] = gH265_Codec_Header_Info[stream_ch].VPS_NAL_size;	// VPS
					gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].NAL_size[2] = gH265_Codec_Header_Info[stream_ch].SPS_NAL_size;	// SPS
					gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].NAL_size[3] = gH265_Codec_Header_Info[stream_ch].PPS_NAL_size;	// PPS
				}
			}
		}
		
		gH265_Prev_Slice_PicType = gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].picType;

		nal_count = 0;
		for( i=0; i<D_H265_ENCODE_NAL_MAX_SIZE; i++ ){
			if( gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].NAL_size[i] != 0 ){
				video_info->nal_unit_size[nal_count] = gH265_Codec_Output_NAL_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].NAL_size[i];
				nal_count++;
			}
		}
		video_info->nal_unit_num = nal_count;
		
		tmp_pts		= (LLONG)gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].pts;
		tmp_dts		= ((LLONG)gH265_Vstrem_Serial_Num[stream_ch] - 1) * gH265_Enc_Ctrl.video[ stream_ch ].time_increment;
		
		if (gH265_Slice_Encode_En) {
			tmp_dts	/= gH265_Slice_per_Frame;
		}
		
		// ((PTS*90KHz)/time_scale) (PTS is 90KHz)
		video_info->sample_pts = (((LLONG)tmp_pts * H265_PTS_90K_BASE) / gH265_Enc_Ctrl.video[stream_ch].time_scale);
		// ((DTS*90KHz)/time_scale) (DTS is 90KHz)
		video_info->sample_dts = (((LLONG)tmp_dts * H265_PTS_90K_BASE) / gH265_Enc_Ctrl.video[stream_ch].time_scale);
		
		video_info->poc					= gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].encPicPoc;
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%lX addr%d", video_info->sample_address, gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].encodedCoreIdx ));
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%lX size%d", video_info->sample_size, gH265_Codec_Output_Info[stream_ch][gH265_Vstrm_Fifo[stream_ch].r_point].encodedCoreIdx ));
	OS_User_Wai_Sem(SID_H265_STREAM_INT);
	gH265_Vstrm_Fifo[stream_ch].r_point++;
	if( gH265_Vstrm_Fifo[stream_ch].r_point >= D_H265_ENCODE_VINFO_FIFO_MAX ){
		gH265_Vstrm_Fifo[stream_ch].r_point = 0;
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "r %d-%d",gH265_Vstrm_Fifo[stream_ch].r_point, gH265_Vstrm_Fifo[stream_ch].w_point ));
	if( (gH265_Vstrm_Fifo[stream_ch].w_point != gH265_Vstrm_Fifo[stream_ch].r_point) ||
		( (gH265_Vstrm_Fifo[stream_ch].w_point == gH265_Vstrm_Fifo[stream_ch].r_point) && (gH265_Vstrm_Fifo[stream_ch].fifo_over_flg == H265_ON) ) ){
		if( stream_ch == E_H265_IF_STREAM_CH_0 ){
//			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "lop %d-%d",gH265_Vstrm_Fifo[stream_ch].r_point, gH265_Vstrm_Fifo[stream_ch].w_point ));
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSTRM_COMP0 );
		}
		else{
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSTRM_COMP1 );
		}
	}
	
	gH265_Vstrm_Fifo[stream_ch].fifo_over_flg = H265_OFF;
	OS_User_Sig_Sem(SID_H265_STREAM_INT);
	
	return H265_OK;
}

/**
 *	@brief		Checked whether Stream Area of HEVC went around.\n
 *	@param		i/o:video_info		Video Informations\n
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_wave_stream_around_check( E_H265_IF_STREAM_CH stream_ch, T_H265_ENC_VIDEO_INFO* video_info, EncOutputInfo vstrm_info )
{
#ifndef CO_DEBUG_ON_PC
	UINT32				size_before;
	UINT32				size_after;
	ULONG				address_before;
	ULONG				address_after;
	UINT32				tmp_size;
	ULONG				stream_work_address;
	UINT32				stream_work_size;
	
	if( (vstrm_info.reconFrameIndex == -1) && (vstrm_info.bitstreamSize == 0) ){
		return;
	}
	
	if( vstrm_info.bitstreamWrapAround == 1 ){
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%X around %ld", vstrm_info.bitstreamWrapAroundPtr, vstrm_info.encodedCoreIdx ));
		
		size_before		= vstrm_info.bitstreamWrapAroundPtr - vstrm_info.bitstreamBuffer;
		address_before	= vstrm_info.bitstreamBuffer; 
		
		size_after		= vstrm_info.bitstreamSize - size_before;
		
		if( gH265_Codec_Handle[stream_ch]->CodecInfo->encInfo.openParam.numCores > 1 ){
			Sdram_Map_Movie_Record_Get_H265_Stream_Bank( stream_ch, vstrm_info.encodedCoreIdx, (UINT32*)&address_after, &tmp_size );
			stream_work_address = Sdram_Map_Movie_Record_Get_H265_Stream_Work( stream_ch, &stream_work_size, vstrm_info.encodedCoreIdx );
		}
		else{
			Sdram_Map_Movie_Record_Get_H265_Stream_Bank( stream_ch, gH265_Codec_Enc_Core_Idx[stream_ch], (UINT32*)&address_after, &tmp_size );
			stream_work_address = Sdram_Map_Movie_Record_Get_H265_Stream_Work( stream_ch, &stream_work_size, gH265_Codec_Enc_Core_Idx[stream_ch] );
		}
#if 0
		if( vstrm_info.encodedCoreIdx == 0 ){
			// vCore0 buffer
			Sdram_Map_Movie_Record_Get_H265_Stream_Bank( stream_ch, 0, (UINT32*)&address_after, &tmp_size );
			stream_work_address = Sdram_Map_Movie_Record_Get_H265_Stream_Work( stream_ch, &stream_work_size, 0 );
		}
		else{
			// vCore1 buffer
			Sdram_Map_Movie_Record_Get_H265_Stream_Bank( stream_ch, 1, (UINT32*)&address_after, &tmp_size );
			stream_work_address = Sdram_Map_Movie_Record_Get_H265_Stream_Work( stream_ch, &stream_work_size, 1 );
		}
#endif
		
		stream_work_address += D_H265_ENCODE_WAVE_STREAM_GAP_SIZE; 	// case HEVC, PES Header Area Include copy area.
		
		H265_Movie_Cache_Flush( address_before, (ULONG)size_before );
		H265_Movie_Cache_Flush( address_after, (ULONG)size_after );
		
		H265_Movie_Memcpy( (UCHAR*)stream_work_address, (UCHAR*)address_before, size_before );
		H265_Movie_Memcpy( (UCHAR*)(stream_work_address + size_before), (UCHAR*)address_after, size_after );
		
		video_info->sample_address	= stream_work_address;
		video_info->sample_size		= size_before + size_after;
		
		H265_Movie_Cache_Clean( video_info->sample_address, video_info->sample_size );
	}
	else{
		video_info->sample_address	= vstrm_info.bitstreamBuffer;
		video_info->sample_size		= vstrm_info.bitstreamSize;
		
		H265_Movie_Cache_Flush( video_info->sample_address, video_info->sample_size );
	}
#endif
	return;
}

/**
 *  @brief		StreamBuffer Space Control \n
 *	@param		sream_ch
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID h265_encode_stream_buffer_init( INT32 index, UINT32 addr )
{
	gH265_Stream_Buffer_ReadPtr[index]	= addr;
	gH265_Stream_Buffer_WritePtr[index]	= addr;
	return;
}

INT32 h265_encode_stream_buffer_allocate( INT32 index, UINT32 addr, UINT32 size, BOOL roundup )
{
	if( gH265_Stream_Buffer_WritePtr[index] >= gH265_Stream_Buffer_ReadPtr[index] ){
		gH265_Stream_Buffer_WritePtr[index] = addr + size;
		
		if( roundup == FALSE ){
			// nothing special
		}
		else{
			if( gH265_Stream_Buffer_WritePtr[index] < gH265_Stream_Buffer_ReadPtr[index] ){
				// nothing special
			}
			else{
				H265_Err_Print(("--- H265 StreamBuffer OverFlow1\n"));
				return H265_NG;
			}
		}
	}
	else{
		gH265_Stream_Buffer_WritePtr[index] = addr + size;
		
		if( roundup == FALSE ){
			if( gH265_Stream_Buffer_WritePtr[index] < gH265_Stream_Buffer_ReadPtr[index] ){
				// nothing special
			}
			else{
				H265_Err_Print(("--- H265 StreamBuffer OverFlow2\n"));
				return H265_NG;
			}
		}
		else{
				H265_Err_Print(("--- H265 StreamBuffer OverFlow3\n"));
				return H265_NG;
		}
	}
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "st_wp %d-0x%X", index, gH265_Stream_Buffer_WritePtr[index] ));
	
	return H265_OK;
}

VOID h265_encode_stream_buffer_free( INT32 index, UINT32 addr )
{
	gH265_Stream_Buffer_ReadPtr[index]	= addr;
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "st_rp %d-0x%X", index, gH265_Stream_Buffer_ReadPtr[index] ));
	return;
}


/**
 *  @brief		Reset Channel Map Information \n
 *	@param		sream_ch
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID h265_encode_ch_map_reset( E_H265_IF_STREAM_CH stream_ch )
{
	E_H265_IF_STREAM_CH							first_stream_ch = E_H265_IF_STREAM_CH_MAX;
	UINT32										i;
	UINT32										free1, free2;
	T_SDRAM_MAP_MOVIE_RECORD_BASE_INFORMATION	map_base_info;
	T_SDRAM_MAP_MOVIE_RECORD_INFORMATION		map_info;
	
	Sdram_Map_Movie_Record_Get_Map_Information( &map_info );
	Sdram_Map_Movie_Record_Get_Map_Base_Information( &map_base_info );
	
	if( map_info.wave_map_count == 0 ){
		;	// nothing (for error route)
	}
	else if( map_info.wave_map_count == 1 ){
		map_info.wave_map_localdec_use			= map_base_info.wave_localdec_top_addr;
		map_info.wave_map_localdec_free_size	= map_base_info.wave_localdec_bottom_addr - map_base_info.wave_localdec_top_addr;
		map_info.wave_map_yc_use				= map_base_info.wave_yc_top_addr;;
		map_info.wave_map_yc_free_size			= map_base_info.wave_yc_bottom_addr - map_base_info.wave_yc_top_addr;
	}
	else if( map_info.wave_map_count == 2 ){
		for( i=0; i<E_H265_IF_STREAM_CH_MAX; i++ ){
			if( i != stream_ch ){
				if( map_info.wave_map_size[i] != E_H265_IF_IMAGE_SIZE_MAX ){
					first_stream_ch = i;
					break;
				}
			}
		}
		if( first_stream_ch == E_H265_IF_STREAM_CH_MAX ){
			H265_Err_Print(("stream ch error!\n"));		// This route does not pass through.
		}
		
		free1 = map_info.wave_map_yc_top_addr[first_stream_ch] - map_base_info.wave_yc_top_addr;
		free2 = map_base_info.wave_yc_bottom_addr - map_info.wave_map_yc_bottom_addr[first_stream_ch];
		
		if( free1 > free2 ){
			map_info.wave_map_localdec_use			= map_base_info.wave_localdec_top_addr;
			map_info.wave_map_localdec_free_size	= map_info.wave_map_localdec_top_addr[first_stream_ch] - map_base_info.wave_localdec_top_addr;
			map_info.wave_map_yc_use				= map_base_info.wave_yc_top_addr;
			map_info.wave_map_yc_free_size			= free1;
		}
		else{
			map_info.wave_map_localdec_use			= map_info.wave_map_localdec_bottom_addr[first_stream_ch];
			map_info.wave_map_localdec_free_size	= map_base_info.wave_localdec_bottom_addr - map_info.wave_map_localdec_bottom_addr[first_stream_ch];
			map_info.wave_map_yc_use				= map_info.wave_map_yc_bottom_addr[first_stream_ch];
			map_info.wave_map_yc_free_size			= free2;
		}
	}
	else{
		// CH3 is not exist
	}
	
	map_info.wave_map_size[stream_ch]					=  E_H265_IF_IMAGE_SIZE_MAX;
	map_info.wave_map_localdec_total_size[stream_ch]	=  0;
	map_info.wave_map_localdec_top_addr[stream_ch]		=  0;
	map_info.wave_map_localdec_bottom_addr[stream_ch]	=  0;
	map_info.wave_map_yc_total_size[stream_ch]			=  0;
	map_info.wave_map_yc_top_addr[stream_ch]			=  0;
	map_info.wave_map_yc_bottom_addr[stream_ch]			=  0;
	map_info.wave_map_count--;
	
	Sdram_Map_Movie_Record_Set_H265_Map_Information( &map_info );
	
	return;
}


// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---

/**
 *	@brief		PES Header set
 *	@param		stream_ch	stream channel
 *	@param		addr		stream address
 *	@param		size		stream size
 *	@param		pts			PTS
 *	@param		dts			DTS
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID h265_encode_pes_header_set( ULONG stream_ch, ULONG addr, ULONG size, ULLONG pts, ULLONG dts )
{
#ifndef CO_DEBUG_ON_PC
	UCHAR tmp_dts;
	UCHAR tmp_pts;
/*
  In the case of VIDEO                                                            
                                                                                  
           addr                                                                   
             |                       size                                         
             |<------------------------------------------------------>|           
             |                                                        |           
  +----------+--------------------------------------------------------+           
  | PES area | ES                                                     |           
  +----------+--------------------------------------------------------+           
                                                                                  
  It is cautious of "addr" and "size" not containing "PES area".                  
*/
	*(USHORT*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE)     = 0x0000;			// start_code					:23-8bit
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 2)  = 0x01;				// start_code					:7-0bit
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 3)  = 0xE0 | stream_ch;	// stream_ch					:8bit	:1110xxxxb. (Sets the value of the Channel to the xxxx.)
	*(USHORT*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 4) = 0x0000;			// packet_length				:16bit	:0 fixed.(PES length undefined)
	*(USHORT*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 6) = 0xC081;			// sync_code					:2bit	:(10b)
																					// scrambling_control			:2bit	:00b -> 0 fixed.
																					// priority						:1bit	:0b  -> 0 fixed.
																					// alignment_indicator			:1bit	:0b  -> 0 fixed.
																					// copyright					:1bit	:0b  -> 0 fixed.
																					// original_or_copy				:1bit	:1b  -> 1 fixed.
																					// PTS_DTS_flag					:2bit	:11b -> 3 fixed.
																					// ESCR_flag					:1bit	:0b  -> 0 fixed.
																					// ES_rate_flag					:1bit	:0b  -> 0 fixed.
																					// DSM_trick_mode_flag			:1bit	:0b  -> 0 fixed.
																					// additional_copy_info_flag	:1bit	:0b  -> 0 fixed.
																					// PES_CRC_flag					:1bit	:0b  -> 0 fixed.
																					// PES_extension_flag			:1bit	:0b  -> 0 fixed.
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 8)  = 0x0A;				// PES_header_data_length		:8bit	:PTS:5 + DTS:5 = 10byte
	
	// PTS
	tmp_pts = (pts & 0x00000001C0000000) >> 29;
	tmp_pts |= 0x31;	// "0011" & marker_bit
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 9)  = tmp_pts;
	tmp_pts = (pts & 0x000000003FC00000) >> 22;
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 10) = tmp_pts;
	tmp_pts = (pts & 0x00000000003F8000) >> 14;
	tmp_pts |= 0x01;
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 11) = tmp_pts;
	tmp_pts = (pts & 0x0000000000007F80) >> 7;
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 12) = tmp_pts;
	tmp_pts = (pts & 0x000000000000007F) << 1;
	tmp_pts |= 0x01;
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 13) = tmp_pts;
	
	// DTS
	tmp_dts = (dts & 0x00000001C0000000) >> 29;
	tmp_dts |= 0x11;
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 14) = tmp_dts;
	tmp_dts = (dts & 0x000000003FC00000) >> 22;
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 15) = tmp_dts;
	tmp_dts = (dts & 0x00000000003F8000) >> 14;
	tmp_dts |= 0x01;
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 16) = tmp_dts;
	tmp_dts = (dts & 0x0000000000007F80) >> 7;
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 17) = tmp_dts;
	tmp_dts = (dts & 0x000000000000007F) << 1;
	tmp_dts |= 0x01;
	*(UCHAR*)(addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE + 18) = tmp_dts;
	
	H265_Movie_Cache_Clean_Flush( (addr - D_H265_ENCODE_PES_HEADER_VIDEO_SIZE), D_H265_ENCODE_PES_HEADER_VIDEO_SIZE );
#endif
}

/**
 *	@brief		ENT Task Illegal end.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_illagal( VOID )
{
	INT32			ret;
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	waiptn;
	OS_USER_FLGPTN	flgptn;
	T_H265_IF_VIDEO_STREAM_PARAM video_stream_info;
	E_H265_IF_STREAM_CH	notify_loop;
	T_H265_IF_MSG		msg;
	
	// CC_Task End wait
	OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
	
	for( notify_loop=E_H265_IF_STREAM_CH_0; notify_loop < E_H265_IF_STREAM_CH_MAX; notify_loop++ ){
		if( gH265_Rec_Param.codec_param[notify_loop].enable_flg == H265_ON ){
			video_stream_info.stream_ch = notify_loop;
			video_stream_info.error_flg = TRUE;
			H265_If_Record_Notify_Video_Stream( notify_loop, &video_stream_info );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			h265_encode_ch_map_reset( notify_loop );
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		}
	}
	
	waiptn = FLG_H265_TASK_CC_STOP;
	ercd = OS_User_Twai_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ANDW, &flgptn, 1000 );
	if( ercd != D_OS_USER_E_OK ){
		H265_Err_Print(("+++ H265_ENT Illigal: Time out ercd=%x\n", ercd ));
	}
	
	h265_encode_result_notify( gH265_Enc_Ctrl.task_result );
	
	// wait free all flag
	OS_User_Twai_Flg( FID_H265, FLG_H265_ENC_FREE_ALL_VSTREAM, D_OS_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_FOREVER );
	OS_User_Clr_Flg( FID_H265, ~FLG_H265_ENC_FREE_ALL_VSTREAM );
	
	H265_Err_Print(("+++ h265_encode_illagal\n"));
	
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	H265_Sirius_Finalize();
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE	
// --- REMOVE_ES_COMPILE_OPT END ---
	if( gH265_Rec_Param.codec_param[E_H265_IF_STREAM_CH_0].enable_flg == H265_ON ){
		if ( gH265_Codec_Queue_Ctrl[gH265_Codec_Handle[0]->instIndex] ){
			H265_Wave_Queue_Destroy( (T_H265_WAVE_QUEUE*)gH265_Codec_Queue_Ctrl[gH265_Codec_Handle[0]->instIndex] );
		}
		H265_Wave_EncClose( 0, gH265_Codec_Handle[0] );
	}
	if( gH265_Rec_Param.codec_param[E_H265_IF_STREAM_CH_1].enable_flg == H265_ON  ){
		if ( gH265_Codec_Queue_Ctrl[gH265_Codec_Handle[1]->instIndex] ){
			H265_Wave_Queue_Destroy( (T_H265_WAVE_QUEUE*)gH265_Codec_Queue_Ctrl[gH265_Codec_Handle[1]->instIndex] );
		}
		H265_Wave_EncClose( 1, gH265_Codec_Handle[1] );
	}
	H265_Wave_Finalize();
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
	
	// ENT Task End Flag Set
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_ENT_STOP );
	
	msg.type = E_H265_IF_TYPE_NO_REQUEST;
	ret = H265_If_Set_Msg( &msg, E_H265_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		H265_Err_Print(("--- H265 End Msg ERR. ret=%x\n", ret ));
	}
	
	return;
}

/**
 *  @brief		Initialize Vin Address Ctl Data.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_Vin_initialize( VOID )
{
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	E_H265_IF_STREAM_CH stream_ch;
	INT32 i;
	
	T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_YC sirius_yc;
	
	for (stream_ch = E_H265_IF_STREAM_CH_0; stream_ch < E_H265_IF_STREAM_CH_MAX; stream_ch++){
		gH265_Vin_Adr_Emp_Over_Flg[stream_ch] = H265_OFF;
		
		Sdram_Map_Movie_Record_Get_H265_Map_YC_Information( stream_ch, &sirius_yc );
		
		gH265_Vin_Adr_Ctl[stream_ch].max_bank_num = sirius_yc.max_bank;
		gH265_Vin_Adr_Ctl[stream_ch].write_point = 0;
		gH265_Vin_Adr_Ctl[stream_ch].read_point = 0;
		gH265_Vin_Adr_Ctl[stream_ch].decide_write_point = 0;
		gH265_Vin_Adr_Ctl[stream_ch].decide_read_point = 0;
		gH265_Vin_Adr_Ctl[stream_ch].hold_max_no = 0;
		gH265_Vin_Adr_Ctl[stream_ch].next_decide_write_point = 0;
		gH265_Vin_Adr_Ctl[stream_ch].next_decide_read_point = 0;
		
		for( i=0; i<D_H265_ENCODE_DECIDE_BANK_FIFO_MAX; i++ ){
			gH265_Vin_Adr_Ctl[stream_ch].next_decide_bank[i] = H265_ENC_VIN_IDX_NODATA;
			gH265_Vin_Adr_Ctl[stream_ch].next_decide_frame_no[i] = 0;
		}
		for( i=0; i<D_SIRIUS_YC_BANK_MAX; i++ ){
			gH265_Vin_Adr_Ctl[stream_ch].free_bank_tbl[i] = i;
			gH265_Vin_Adr_Ctl[stream_ch].decide_bank[i] = 0;
			gH265_Vin_Adr_Ctl[stream_ch].decide_frame_no[i] = 0;
			gH265_Vin_Adr_Ctl[stream_ch].duplicate_check_tbl[i] = 0;
		}
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE	
// --- REMOVE_ES_COMPILE_OPT END ---
	E_H265_IF_STREAM_CH stream_ch;
	INT32 i;
	
	T_SDRAM_MAP_MOVIE_RECORD_WAVE_YC wave_yc;
	
	for (stream_ch = E_H265_IF_STREAM_CH_0; stream_ch < E_H265_IF_STREAM_CH_MAX; stream_ch++){
		gH265_Vin_Adr_Emp_Over_Flg[stream_ch] = H265_OFF;
		
		Sdram_Map_Movie_Record_Get_H265_Map_YC_Information( stream_ch, &wave_yc );
		
		gH265_Vin_Adr_Ctl[stream_ch].max_bank_num = wave_yc.max_bank;
		gH265_Vin_Adr_Ctl[stream_ch].write_point = 0;
		gH265_Vin_Adr_Ctl[stream_ch].read_point = 0;
		gH265_Vin_Adr_Ctl[stream_ch].decide_write_point = 0;
		gH265_Vin_Adr_Ctl[stream_ch].decide_read_point = 0;
		gH265_Vin_Adr_Ctl[stream_ch].hold_max_no = 0;
		gH265_Vin_Adr_Ctl[stream_ch].next_decide_write_point = 0;
		gH265_Vin_Adr_Ctl[stream_ch].next_decide_read_point = 0;
		
		for( i=0; i<D_H265_ENCODE_DECIDE_BANK_FIFO_MAX; i++ ){
			gH265_Vin_Adr_Ctl[stream_ch].next_decide_bank[i] = H265_ENC_VIN_IDX_NODATA;
			gH265_Vin_Adr_Ctl[stream_ch].next_decide_frame_no[i] = 0;
		}
		for( i=0; i<D_WAVE_YC_BANK_MAX; i++ ){
			gH265_Vin_Adr_Ctl[stream_ch].free_bank_tbl[i] = i;
			gH265_Vin_Adr_Ctl[stream_ch].decide_bank[i] = 0;
			gH265_Vin_Adr_Ctl[stream_ch].decide_frame_no[i] = 0;
			gH265_Vin_Adr_Ctl[stream_ch].duplicate_check_tbl[i] = 0;
		}
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
	return;
}

/**
 *	@brief		Set HEVC input area.\n
 *	@param		i:stream_ch		Stream CH (HEVC CH)
 *	@param		i:bank			Bank Number
 *	@param		i:frame_no		Frame Number
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_Vin_set_decide_bank( E_H265_IF_STREAM_CH stream_ch, INT32 bank, ULLONG frame_no )
{
	if( bank != H265_ENC_VIN_IDX_NODATA ){
		gH265_Vin_Adr_Ctl[stream_ch].decide_bank[ gH265_Vin_Adr_Ctl[stream_ch].decide_write_point ] = bank;
		gH265_Vin_Adr_Ctl[stream_ch].decide_frame_no[ gH265_Vin_Adr_Ctl[stream_ch].decide_write_point ] = frame_no;
		gH265_Vin_Adr_Ctl[stream_ch].decide_write_point++;
		if( gH265_Vin_Adr_Ctl[stream_ch].decide_write_point >= gH265_Vin_Adr_Ctl[stream_ch].max_bank_num ){
			gH265_Vin_Adr_Ctl[stream_ch].decide_write_point = 0;
		}
		gH265_Vin_Adr_Ctl[stream_ch].hold_max_no++;				// Hold Max Num 3
#if 0	// debug
		H265_Info_Print(( "vsd=  [%d]\n", bank ));
#endif
	}
	return;
}

/**
 *	@brief		Get HEVC input Area.\n
 *	@param		i:stream_ch		Stream CH (HEVC CH)
 *	@param		o:bank			Bank Number
 *	@param		o:frame_no		Frame Number
 *	@return		H265_OK			Normal End
 *	@return		H265_NG			Abnormal End
 *	@note		None
 *	@attention	None
 */
static INT32 h265_encode_Vin_get_decide_bank( E_H265_IF_STREAM_CH stream_ch, INT32* bank, ULLONG* frame_no )
{
	if( gH265_Vin_Adr_Ctl[stream_ch].hold_max_no == 0 ){
		return H265_ENC_VIN_CTL_NONEDATA;
	}
	*bank = gH265_Vin_Adr_Ctl[stream_ch].decide_bank[ gH265_Vin_Adr_Ctl[stream_ch].decide_read_point ];
	*frame_no = gH265_Vin_Adr_Ctl[stream_ch].decide_frame_no[ gH265_Vin_Adr_Ctl[stream_ch].decide_read_point ];
	
#if 0	// debug
	H265_Info_Print(( "vgd=[%d]\n", *bank ));
#endif
	
	gH265_Vin_Adr_Ctl[stream_ch].decide_read_point++;
	if( gH265_Vin_Adr_Ctl[stream_ch].decide_read_point >= gH265_Vin_Adr_Ctl[stream_ch].max_bank_num ){
		gH265_Vin_Adr_Ctl[stream_ch].decide_read_point = 0;
	}
	gH265_Vin_Adr_Ctl[stream_ch].hold_max_no--;
	
	if( gH265_Vin_Adr_Ctl[stream_ch].hold_max_no == 0 ){
		return H265_ENC_VIN_CTL_LASTDATA;
	}
	
	return H265_OK;
}

/**
 *	@brief		Set Empty HEVC input area.\n
 *	@param		i:stream_ch		Stream CH (HEVC CH)
 *	@param		i:bank			Empty Bank Number
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_Vin_set_empty_bank( E_H265_IF_STREAM_CH stream_ch, INT32 bank )
{
	if( gH265_Vin_Adr_Ctl[stream_ch].duplicate_check_tbl[bank] > 0 ){
		gH265_Vin_Adr_Ctl[stream_ch].duplicate_check_tbl[bank]--;
#if 0	// debug
		H265_Info_Print(( "vse_d=[%d]\n", bank ));
#endif
		return;
	}
	gH265_Vin_Adr_Ctl[stream_ch].free_bank_tbl[ gH265_Vin_Adr_Ctl[stream_ch].write_point ] = bank;
	gH265_Vin_Adr_Ctl[stream_ch].write_point++;
	if( gH265_Vin_Adr_Ctl[stream_ch].write_point >= gH265_Vin_Adr_Ctl[stream_ch].max_bank_num ){
		gH265_Vin_Adr_Ctl[stream_ch].write_point = 0;
	}
	
	gH265_Vin_Adr_Emp_Over_Flg[stream_ch] = H265_OFF;
	
#if 0	// debug
	H265_Info_Print(( "vse  =[%d]\n", bank ));
#endif

	return;
}

/**
 *	@brief		Get Empty HEVC input area.\n
 *	@param		i:stream_ch		Stream CH (HEVC CH)
 *	@param		o:bank			Empty Bank Number
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 h265_encode_Vin_get_empty_bank( E_H265_IF_STREAM_CH stream_ch, INT32* bank )
{
	if( gH265_Vin_Adr_Emp_Over_Flg[stream_ch] == H265_ON ){
		H265_Err_Print(("H265 Image Bank Over Flow! ch=%d\n",stream_ch));
		return H265_NG;
	}
	
	*bank = gH265_Vin_Adr_Ctl[stream_ch].free_bank_tbl[ gH265_Vin_Adr_Ctl[stream_ch].read_point ];
	gH265_Vin_Adr_Ctl[stream_ch].read_point++;
	if( gH265_Vin_Adr_Ctl[stream_ch].read_point >= gH265_Vin_Adr_Ctl[stream_ch].max_bank_num ){
		gH265_Vin_Adr_Ctl[stream_ch].read_point = 0;
	}
	
	if( gH265_Vin_Adr_Ctl[stream_ch].write_point == gH265_Vin_Adr_Ctl[stream_ch].read_point ){
		gH265_Vin_Adr_Emp_Over_Flg[stream_ch] = H265_ON;
	}
#if 0	// debug
	H265_Info_Print(( "vge=  [%d]\n", *bank ));
#endif
	
	return H265_OK;
}

/**
 *	@brief		Get HEVC input address.\n
 *	@param		i:stream_ch		Encode Stream Channel.
 *	@param		i:frame_no		Encode Frame Number.
 *	@param		i:size			Encode Size.
 *	@param		o:y_addr		Y Address Pointer.
 *	@param		o:c_addr		C Address Pointer.
 *	@param		o:g_width		Global Width for Encode Bank.
 *	@return		H265_OK			Normal End
 *	@return		H265_NG			Abnormal End (Return Dummy Address)
 *	@note		None
 *	@attention	None
 */
static INT32 h265_encode_Vin_get_address( E_H265_IF_STREAM_CH stream_ch, ULLONG frame_no, E_H265_IF_IMAGE_SIZE size, T_H265_IF_GET_IMAGE_ADDR_PARAM* addr_param )
{
	INT32			bank = 0;
	INT32			ret;
	OS_USER_ER		ercd;
	
	ercd = OS_User_Twai_Sem(SID_H265, H265_ENC_SEMAPHORE_TIMEOUT);
	if( ercd != D_OS_USER_E_OK ){
		H265_Err_Print(( "h265_encode_Vin_get_address: semaphore NG\n" ));
		Sdram_Map_Movie_Record_Get_H265_Vin_Dummy( stream_ch, size, addr_param );
		OS_User_Sig_Sem(SID_H265);
		return H265_NG;
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "v_get" ));
	ret = h265_encode_Vin_get_empty_bank( stream_ch, &bank );
	if( ret != H265_OK ){
		Sdram_Map_Movie_Record_Get_H265_Vin_Dummy( stream_ch, size, addr_param );
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "over" ));
		OS_User_Sig_Sem(SID_H265);
		return H265_NG;
	}
	
	gH265_Vin_Adr_Ctl[stream_ch].next_decide_bank[gH265_Vin_Adr_Ctl[stream_ch].next_decide_write_point] = bank;
	gH265_Vin_Adr_Ctl[stream_ch].next_decide_frame_no[gH265_Vin_Adr_Ctl[stream_ch].next_decide_write_point] = frame_no;
	
	gH265_Vin_Adr_Ctl[stream_ch].next_decide_write_point++;
	if( gH265_Vin_Adr_Ctl[stream_ch].next_decide_write_point >= D_H265_ENCODE_DECIDE_BANK_FIFO_MAX ){
		gH265_Vin_Adr_Ctl[stream_ch].next_decide_write_point = 0;
	}
	if( gH265_Vin_Adr_Ctl[stream_ch].next_decide_write_point == gH265_Vin_Adr_Ctl[stream_ch].next_decide_read_point ){
		H265_Err_Print(("H265 get_address continued! (decide_address does not come) ch=%d\n", stream_ch));
		Sdram_Map_Movie_Record_Get_H265_Vin_Dummy( stream_ch, size, addr_param );
		OS_User_Sig_Sem(SID_H265);
		return H265_NG;
	}
	
	OS_User_Sig_Sem(SID_H265);
	Sdram_Map_Movie_Record_Get_H265_Vin( stream_ch, bank, addr_param );
	
	return H265_OK;
}

/**
 *	@brief		Decide HEVC input address.\n
 *	@param		i:stream_ch		Encode Stream Channel.
 *	@param		i:frame_no		Encode Frame Number.
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_Vin_decide_address( E_H265_IF_STREAM_CH stream_ch, ULLONG frame_no )
{
	INT32			ret = 0;
	OS_USER_ER		ercd;
	INT32			erace_bank;
	ULLONG			erace_frame_no;
	
	ercd = OS_User_Twai_Sem(SID_H265, H265_ENC_SEMAPHORE_TIMEOUT);
	if( ercd != D_OS_USER_E_OK ){
		H265_Err_Print(( "h265_encode_Vin_decide_address: semaphore NG\n" ));
		OS_User_Sig_Sem(SID_H265);
		return;
	}
	
	if( gH265_Vin_Adr_Ctl[stream_ch].next_decide_frame_no[gH265_Vin_Adr_Ctl[stream_ch].next_decide_read_point] != frame_no ){
		H265_Err_Print(("H265 frameNo different!\n"));
	}
	h265_encode_Vin_set_decide_bank( stream_ch,
									 gH265_Vin_Adr_Ctl[stream_ch].next_decide_bank[gH265_Vin_Adr_Ctl[stream_ch].next_decide_read_point],
									 gH265_Vin_Adr_Ctl[stream_ch].next_decide_frame_no[gH265_Vin_Adr_Ctl[stream_ch].next_decide_read_point] );
	
	gH265_Vin_Adr_Ctl[stream_ch].next_decide_read_point++;
	if( gH265_Vin_Adr_Ctl[stream_ch].next_decide_read_point >= D_H265_ENCODE_DECIDE_BANK_FIFO_MAX ){
		gH265_Vin_Adr_Ctl[stream_ch].next_decide_read_point = 0;
	}	
	
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	if( gH265_Vin_Adr_Ctl[stream_ch].hold_max_no > 2 )
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE	
// --- REMOVE_ES_COMPILE_OPT END ---
	if( gH265_Vin_Adr_Ctl[stream_ch].hold_max_no > 2 )
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
	{
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
		// Free to oldest Bank 
		if( E_H265_IF_RECORD_STATUS_RUNNING == H265_If_Record_Api_Get_Status() ){
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265 Movie later %u-%u", stream_ch, (UINT32)frame_no ));
			if( gH265_MovieLater_Count == 0 ){
				H265_Warning_Print(( "H265 Movie task later!! ch[%d] frame[%d] (timming NG) \n", stream_ch, (UINT32)frame_no ));
			}
			gH265_MovieLater_Count++;
			gH265_MovieLater_Total_Count++;
			if( gH265_MovieLater_Count > D_H265_ENCODE_LATER_THRESHOLD ){
				gH265_MovieLater_Count = 0;
			}
		}
		ret = h265_encode_Vin_get_decide_bank( stream_ch, &erace_bank, &erace_frame_no );
		if( ret != H265_OK ){
			H265_Err_Print(( "Vin decide NG1\n" ));
			OS_User_Sig_Sem(SID_H265);
			return;
		}
		h265_encode_Vin_set_empty_bank( stream_ch, erace_bank );
	}
	OS_User_Sig_Sem(SID_H265);
	return;
}

/**
 *	@brief		Set video stream information parameter.\n
 *	@param		i:T_H265_ENC_VIDEO_INFO			video_info
 *	@param		o:T_H265_IF_VIDEO_STREAM_PARAM*	video_stream_info
 *	@param		o:ULLONG* 						total_duration
 *	@return		none
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_video_stream_param_set( UINT32 stream_ch, T_H265_ENC_VIDEO_INFO* video_info, T_H265_IF_VIDEO_STREAM_PARAM* video_stream_info, ULLONG* total_duration)
{
	USHORT nal_cnt;
	
	video_stream_info->stream_ch		= stream_ch;
	
	if( video_info->stream_end_flg == H265_ON){
		// In the case of stream end notification, only flag setting.
		// Other parameters are do not care.
		video_stream_info->stream_end_flg = video_info->stream_end_flg;
		return;
	}
	
	video_stream_info->frame_no			= video_info->frame_no;
	
	switch( gH265_Rec_Param.codec_param[stream_ch].size ){
		case E_H265_IF_IMAGE_SIZE_4000_3000 :
			video_stream_info->width = 4000;
			video_stream_info->lines = 3000;
			break;
		case E_H265_IF_IMAGE_SIZE_4096_2304 :
			video_stream_info->width = 4096;
			video_stream_info->lines = 2304;
			break;
		case E_H265_IF_IMAGE_SIZE_4096_2160 :
			video_stream_info->width = 4096;
			video_stream_info->lines = 2160;
			break;
		case E_H265_IF_IMAGE_SIZE_4096_2048 :
			video_stream_info->width = 4096;
			video_stream_info->lines = 2048;
			break;
		case E_H265_IF_IMAGE_SIZE_3840_2160 :
			video_stream_info->width = 3840;
			video_stream_info->lines = 2160;
			break;
		case E_H265_IF_IMAGE_SIZE_3840_1920 :
			video_stream_info->width = 3840;
			video_stream_info->lines = 1920;
			break;
		case E_H265_IF_IMAGE_SIZE_3000_3000 :
			video_stream_info->width = 3000;
			video_stream_info->lines = 3000;
			break;
		case E_H265_IF_IMAGE_SIZE_2704_2028 :
			video_stream_info->width = 2704;
			video_stream_info->lines = 2028;
			break;
		case E_H265_IF_IMAGE_SIZE_2704_1520 :
			video_stream_info->width = 2704;
			video_stream_info->lines = 1520;
			break;
		case E_H265_IF_IMAGE_SIZE_2560_1920 :
			video_stream_info->width = 2560;
			video_stream_info->lines = 1920;
			break;
		case E_H265_IF_IMAGE_SIZE_2560_1440 :
			video_stream_info->width = 2560;
			video_stream_info->lines = 1440;
			break;
		case E_H265_IF_IMAGE_SIZE_1920_1440 :
			video_stream_info->width = 1920;
			video_stream_info->lines = 1440;
			break;
		case E_H265_IF_IMAGE_SIZE_1920_1080 :
			video_stream_info->width = WIDTH_FHD;
			video_stream_info->lines = LINES_FHD;
			break;
		case E_H265_IF_IMAGE_SIZE_1920_960 :
			video_stream_info->width = 1920;
			video_stream_info->lines = 960;
			break;
		case E_H265_IF_IMAGE_SIZE_1504_1504 :
			video_stream_info->width = 1504;
			video_stream_info->lines = 1504;
			break;
		case E_H265_IF_IMAGE_SIZE_1440_1080 :
			video_stream_info->width = WIDTH_1440;
			video_stream_info->lines = LINES_1440;
			break;
		case E_H265_IF_IMAGE_SIZE_1440_720 :
			video_stream_info->width = WIDTH_1440;
			video_stream_info->lines = 720;
			break;
		case E_H265_IF_IMAGE_SIZE_1280_960 :
			video_stream_info->width = 1280;
			video_stream_info->lines = 960;
			break;
		case E_H265_IF_IMAGE_SIZE_1280_720 :
			video_stream_info->width = 1280;
			video_stream_info->lines = 720;
			break;
		case E_H265_IF_IMAGE_SIZE_960_540 :
			video_stream_info->width = 960;
			video_stream_info->lines = 540;
			break;
		case E_H265_IF_IMAGE_SIZE_960_480 :
			video_stream_info->width = 960;
			video_stream_info->lines = 480;
			break;
		case E_H265_IF_IMAGE_SIZE_864_480 :
			video_stream_info->width = 864;
			video_stream_info->lines = 480;
			break;
		case E_H265_IF_IMAGE_SIZE_848_480 :
			video_stream_info->width = 848;
			video_stream_info->lines = 480;
			break;
		case E_H265_IF_IMAGE_SIZE_800_600 :
			video_stream_info->width = 800;
			video_stream_info->lines = 600;
			break;
		case E_H265_IF_IMAGE_SIZE_640_480 :
			video_stream_info->width = 640;
			video_stream_info->lines = 480;
			break;
		case E_H265_IF_IMAGE_SIZE_640_360 :
			video_stream_info->width = 640;
			video_stream_info->lines = 360;
			break;
		case E_H265_IF_IMAGE_SIZE_480_270 :
			video_stream_info->width = 480;
			video_stream_info->lines = 270;
			break;
		case E_H265_IF_IMAGE_SIZE_720_576 :
			video_stream_info->width = 720;
			video_stream_info->lines = 576;
			break;
		case E_H265_IF_IMAGE_SIZE_720_480 :
			video_stream_info->width = 720;
			video_stream_info->lines = 480;
			break;
		case E_H265_IF_IMAGE_SIZE_1024_768 :
			video_stream_info->width = 1024;
			video_stream_info->lines = 768;
			break;
		case E_H265_IF_IMAGE_SIZE_1280_768:
			video_stream_info->width = 1280;
			video_stream_info->lines = 768;
			break;
		case E_H265_IF_IMAGE_SIZE_1280_1024:
			video_stream_info->width = 1280;
			video_stream_info->lines = 1024;
			break;
		case E_H265_IF_IMAGE_SIZE_1600_896:
			video_stream_info->width = 1600;
			video_stream_info->lines = 896;
			break;
		case E_H265_IF_IMAGE_SIZE_1024_576:
			video_stream_info->width = 1024;
			video_stream_info->lines = 576;
			break;
		case E_H265_IF_IMAGE_SIZE_800_448:
			video_stream_info->width = 800;
			video_stream_info->lines = 448;
			break;
		case E_H265_IF_IMAGE_SIZE_352_288:
			video_stream_info->width = 352;
			video_stream_info->lines = 388;
			break;
		case E_H265_IF_IMAGE_SIZE_2592_1944:
			video_stream_info->width = 2592;
			video_stream_info->lines = 1944;
			break;
		case E_H265_IF_IMAGE_SIZE_2304_1296:
			video_stream_info->width = 2304;
			video_stream_info->lines = 1296;
			break;
		case E_H265_IF_IMAGE_SIZE_432_240 :
			video_stream_info->width = 432;
			video_stream_info->lines = 240;
			break;
		case E_H265_IF_IMAGE_SIZE_320_240 :
			video_stream_info->width = 320;
			video_stream_info->lines = 240;
			break;
	  	case E_H265_IF_IMAGE_SIZE_1920_1280:
		  	video_stream_info->width = 1920;
		  	video_stream_info->lines = 1280;
			break;
	  	case E_H265_IF_IMAGE_SIZE_640_512:
		  	video_stream_info->width = 640;
		  	video_stream_info->lines = 512;
			break;
	  	case E_H265_IF_IMAGE_SIZE_384_288:
		  	video_stream_info->width = 384;
		  	video_stream_info->lines = 288;
			break;
		default :
			H265_Err_Print(("--- Size Error!! size=%d\n", gH265_Rec_Param.codec_param[stream_ch].size ));
			break;
	}
	video_stream_info->stream_serial_num		= video_info->vstrem_serial_num;
	video_stream_info->stream_end_flg			= video_info->stream_end_flg;
	video_stream_info->vstream_index			= video_info->vstream_index;
	video_stream_info->sample_address			= video_info->sample_address;
	video_stream_info->sample_size				= video_info->sample_size;
	video_stream_info->sample_type				= video_info->sample_type;
	video_stream_info->sample_poc				= video_info->poc;
	video_stream_info->frame_of_GOP				= video_info->frame_count_in_gop;
	video_stream_info->nal_unit_num				= video_info->nal_unit_num;
	for (nal_cnt = 0; nal_cnt < H265_NAL_UNIT_MAX_NUM; nal_cnt++) {
		video_stream_info->nal_unit_size[nal_cnt] = video_info->nal_unit_size[nal_cnt];
	}
	video_stream_info->sample_pts				= (LLONG)video_info->sample_pts;
	video_stream_info->sample_dts				= (LLONG)video_info->sample_dts;
	video_stream_info->bitrate					= video_info->bitrate;
	video_stream_info->ave_bitrate				= video_info->ave_bitrate;
	
	*total_duration += gH265_Enc_Ctrl.video[ stream_ch ].time_increment;
	
	return;
}

/**
 *	@brief		Encode Task End process.\n
 *	@param		i:enc_result		Encode Result
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_result_notify( INT32 enc_result )
{
	// befor File write STOP trigger send
	if( enc_result == H265_OK_NORMAL_END ){
		H265_If_Record_Notify_NORMAL_END();
	}
	else{
		H265_If_Record_Notify_FATAL_ERR();
	}
	
	return;
}

/**
 *	@brief		Free video stream.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_encode_free_vstream( VOID )
{
	USHORT i;
	USHORT loop;
	
	OS_User_Wai_Sem(SID_H265_STREAM);
	loop = gH265_Free_Vstream_Count;
	for(i = 0; i < loop; i++){
		if( gH265_Free_Vstream_Ctrl[gH265_Free_Vstream_Index[i]].use_flg != H265_OFF ){
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			H265_Sirius_Enc_Veb_Info_Set( gH265_Free_Vstream_Ctrl[gH265_Free_Vstream_Index[i]].ch, gH265_Free_Vstream_Ctrl[gH265_Free_Vstream_Index[i]].free_size );
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VEB_INFO_SET );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "f %d-%d",gH265_Free_Vstream_Ctrl[gH265_Free_Vstream_Index[i]].ch, gH265_Free_Vstream_Index[i] ));
			h265_encode_stream_buffer_free( gH265_Free_Vstream_Ctrl[gH265_Free_Vstream_Index[i]].free_buf_index, gH265_Free_Vstream_Ctrl[gH265_Free_Vstream_Index[i]].free_addr );
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
			gH265_Free_Vstream_Ctrl[gH265_Free_Vstream_Index[i]].use_flg = H265_OFF;
		}
		else{
			H265_Warning_Print(("+++ free index not use.\n" ));
		}
	}
	gH265_Free_Vstream_Count = 0;;
	OS_User_Sig_Sem(SID_H265_STREAM);
	
	return;
}

#ifdef CO_H265_ENC_DEBUG_VIEW_DUMMY
/* for Debug (view task dummy) */
/**
 *  @brief		Encode DISP Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Encode_DISP_Main( VOID )
{
	OS_USER_FLGPTN			flgptn;
	OS_USER_ER				ercd;
	OS_USER_FLGPTN			waiptn;
	T_H265_IF_MSG			msg;
	INT32					ret;
	E_H265_IF_STREAM_CH		stream_ch;
	ULONG					frame_no[E_H265_IF_STREAM_CH_MAX] = {0,0};
	ULONG					frame_PTS[E_H265_IF_STREAM_CH_MAX] = {0,0};
	T_H265_IF_GET_IMAGE_ADDR_PARAM addr_param;
	
	waiptn = FLG_H265_SYNC_ENC_DUMMY_CH0 | FLG_H265_SYNC_ENC_DUMMY_CH1;
	
	while(1){
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_DISP" ));
		
		ercd = OS_User_Wai_Flg( FID_H265_SYNC, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd != D_OS_USER_E_OK ){
			H265_Info_Print(("+++ DISP Task: wai_flg ercd=%x\n", ercd ));
		}
		
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H265_DISP" ));
		
		if( 0 != (flgptn & FLG_H265_SYNC_ENC_DUMMY_CH0) ){
			OS_User_Clr_Flg( FID_H265_SYNC, ~FLG_H265_SYNC_ENC_DUMMY_CH0 );
			stream_ch = E_H265_IF_STREAM_CH_0;
//			printf("disp:sync ch0\n");
		}
		else if( 0 != (flgptn & FLG_H265_SYNC_ENC_DUMMY_CH1) ){
			OS_User_Clr_Flg( FID_H265_SYNC, ~FLG_H265_SYNC_ENC_DUMMY_CH1 );
			stream_ch = E_H265_IF_STREAM_CH_1;
//			printf("disp:sync ch1\n");
		}
		else{
			H265_Err_Print(("+++ CC Task: Sync Wait: The route which is not assumed.\n" ));
			stream_ch = E_H265_IF_STREAM_CH_0;
		}
		
//		H265_Encode_Vin_Address_Valid( stream_ch );
		
		if( gH265_Rec_Param.codec_param[stream_ch].enable_flg == H265_ON ){
			H265_Encode_Image_Address_Get(stream_ch, gH265_Rec_Param.codec_param[stream_ch].size, frame_no[stream_ch], &addr_param );
			H265_Encode_Image_Address_Decide( stream_ch, frame_no[stream_ch], addr_param.dummy_flg );
			frame_no[stream_ch]++;
			frame_PTS[stream_ch] += gH265_Enc_Ctrl.video[stream_ch].time_increment;
		}
		else{
			H265_Info_Print(("+++ DISP Task: Sync of the invalid channel occurred.\n" ));
		}
		
		ercd = OS_User_Pol_Flg( FID_H265_TASK, FLG_H265_TASK_CC_STOP, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ){
			break;
		}
		else if( ercd == D_OS_USER_E_TMOUT ) {
			;
		}
		else{
			H265_Err_Print(("--- DISP Task: pol_flg ercd=%x\n", ercd ));
		}
	}
	// Set DISP Task Stop Event 
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_DISP_STOP );
	
	msg.type = E_H265_IF_TYPE_NO_REQUEST;
	ret = H265_If_Set_Msg( &msg, E_H265_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		H265_Err_Print(("--- H265_CC End Msg ERR. ret=%x\n", ret ));
	}
	
	return;
}
#endif

/**
 * Valid Image Area Address on Movie Mode
 * @param		stream_ch		stream channel
 * @return		None
 * @attention	None
 */
VOID H265_Encode_Vin_Address_Valid( E_H265_IF_STREAM_CH stream_ch )
{
	gH265_Map_Enable_Flg[stream_ch] = H265_ON;
//	printf("gH265_Map_Enable_Flg is ON!!\n");
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "MapEnable ON[%u]", stream_ch ));
	return;
}

/**
 * Set Dynamic Parameter  on Movie Mode
 * @param		stream_ch				stream channel
 * @param		dynamic_param			dynamic parameter
 * @return		None
 * @attention	None
 */
VOID H265_Encode_Dynamic_Parameter_Set( E_H265_IF_STREAM_CH stream_ch, T_H265_IF_DYNAMIC_PARAM_RECORD* dynamic_param )
{
	gH265_Dynamic_Param[stream_ch].bitrate				= dynamic_param->bitrate;
	gH265_Dynamic_Param[stream_ch].gop_struct			= dynamic_param->gop_struct;
	gH265_Dynamic_Param[stream_ch].frame_num_of_gop		= dynamic_param->frame_num_of_gop;
	gH265_Dynamic_Param[stream_ch].gop_num_idr_to_idr	= dynamic_param->gop_num_idr_to_idr;
	gH265_Dynamic_Param[stream_ch].closed_gop			= dynamic_param->closed_gop;
	
	gH265_Dynamic_Change_Flag[stream_ch] = H265_ON;
	
	return;
}

/**
 * Set Insert IDR Trriger
 * @param		stream_ch				stream channel
 * @return		None
 * @attention	None
 */
VOID H265_Encode_Insert_IDR_Trriger_Set( E_H265_IF_STREAM_CH stream_ch )
{
	gH265_Insert_IDR_Flag[stream_ch] = H265_ON;
//	printf("[H265]Insert_IDR_Flag[%d] is ON!!\n", stream_ch);
	return;
}

