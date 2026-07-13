/**
 * @file		h265_decode.c
 * @brief		H.265 Decode Process.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h264_decode.h"
#include "h265_decode.h"
#include "sdram_map_movie_play.h"
#include "sdram_map_movie_play_define.h"
#include "mba_if.h"
#include "h264_user.h"
#include "user_streaming.h"
#include "h264_movie.h"
#include "sdram_map_play.h"
#ifdef CO_H265_SECTION_ON
//#pragma arm section code="H265_CODE"
//#pragma arm section rodata="H265_CONST"
//#pragma arm section rwdata="H265_INIT"
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_H265_DEC_SDRAM_CH_CHECK					(SDRAM_ADR_CH0_TOP | SDRAM_ADR_CH1_TOP | SDRAM_ADR_CH2_TOP)

#define D_H265_DEC_TIMER_FFREW_2X_TIME_SCALE		(4000)		// 250ms cycle (4fps)
#define D_H265_DEC_TIMER_FFREW_2X_TIME_DELTA		(1000)
#define D_H265_DEC_TIMER_FFREW_4X_TIME_SCALE		(8000)		// 125ms cycle (8fps)
#define D_H265_DEC_TIMER_FFREW_4X_TIME_DELTA		(1000)
#define D_H265_DEC_TIMER_FFREW_8X_TIME_SCALE		(16000)		// 62.5ms cycle (16fps)
#define D_H265_DEC_TIMER_FFREW_8X_TIME_DELTA		(1000)

#define D_H265_DEC_SPECIAL_TRICK_VDPB_COEFFICIENT	(4)			// for Reverse Play.

#define D_H265_DEC_MAX_PTS							(0x7FFFFFFF)

#define D_H265_DEC_NOT_SET_PTS						(0xFFFFFFFFFFFFFFFF)

#define D_H265_DEC_FIRST_BUFFERING_COEFFICIENT		(10)		// Based on the "30fps is 3frame buffering". buffring_num = fps / coefficient.

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum{
	E_H265_DEC_PTS_SEARCH_MODE_SAME			= 0,	// ex.Use Normal Play
	E_H265_DEC_PTS_SEARCH_MODE_SMALL		= 1,	// ex.Use FF
 	E_H265_DEC_PTS_SEARCH_MODE_BIG			= 2,	// ex.Use REW
} E_H265_DEC_PTS_SEARCH_MODE;


typedef enum{
	E_H265_DEC_DISPLAY_DEVICE_LCD			= 0,	// LCD
	E_H265_DEC_DISPLAY_DEVICE_HDMI			= 1,	// HDMI
} E_H265_DEC_DISPLAY_DEVICE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* --- Decode Condition --------------------------------------------------- */
static	volatile	T_H265_IF_DECODER_PARAM		gH265_Dec_Param;
/* --- Decode control data ------------------------------------------------ */
static	volatile	T_H265_DEC_CTL				gH265_Dec_Ctrl;
/* --- Demux Control Data ------------------------------------------------- */
static	volatile	T_H265_DEC_DMUX_CTL			gH265_Dmux_Ctrl;
/* --- Display Control Data ----------------------------------------------- */
static	volatile	T_H265_DEC_DISP_CTL			gH265_Disp_Ctrl;

static	volatile	UINT32						gH265_Dec_Serial_Num;		// for VCPB request

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static	VOID				h265_decode_initialize( T_H265_IF_DECODER_PARAM* decoder_param, T_H265_IF_DISPLAY_INFO* display_info );
static	VOID				h265_decode_initialize_gdec_param( T_H265_IF_DECODER_PARAM* decoder_param );
static	VOID				h265_decode_initialize_gdec_control( VOID );
static	VOID				h265_decode_initialize_gdmux_control( VOID );
static	VOID				h265_decode_initialize_gdisp_control( T_H265_IF_DISPLAY_INFO* display_info );
static	INT32				h265_decode_init_param( VOID );
static	VOID				h265_decode_task_start( UINT32 task_id );
static	VOID				h265_decode_task_forced_stop( VOID );
static	E_H265_PLAY_MODE	h265_decode_flush( VOID );
static	E_H265_PLAY_MODE	h265_decode_play( VOID );
static	E_H265_PLAY_MODE	h265_decode_pause( VOID );
static	E_H265_PLAY_MODE	h265_decode_resume( VOID );
static	E_H265_PLAY_MODE	h265_decode_stop( VOID );
static	E_H265_PLAY_MODE	h265_decode_end( VOID );
static	INT32				h265_decode_set_vcpb( UINT32 video_index, UCHAR vdpb_use_num);
static	INT32				h265_decode_next_video_sample_get( UINT32* video_index );
static	INT32				h265_decode_next_video_sample_request( VOID );
static	INT32				h265_decode_get_vdpb( T_SR_FM_OUT_INFO* fminfo );
static	VOID				h265_decode_vdpb_free( E_H265_IF_PLAY_DIRECTION direction, T_H265_DEC_DISP_REFER_INDEX protect_index );
static	INT32				h265_decode_search_display_index( E_H265_DEC_PTS_SEARCH_MODE search_mode, UCHAR* display_index, T_H265_DEC_DISP_REFER_INDEX protect_index );
static	VOID				h265_decode_display_image( T_H265_IF_DISPLAY_PARAM* display_info, T_H265_MOVIE_ADDR* dec_addr, T_H265_MOVIE_ADDR* disp_addr, BOOL codec_format );
static	INT32				h265_decode_top_image_get( ULLONG req_pts, T_H265_IF_VIDEO_YUV_INFO* decode_yuv_info );
static	VOID				h265_decode_flush_execute( UCHAR* disp_bank );

typedef E_H265_PLAY_MODE (*T_H265_DEC_PLAY_FUNC)(VOID);

const T_H265_DEC_PLAY_FUNC h265_dec_play_func[E_H265_PLAY_MODE_MAX] = {
	h265_decode_flush,
	h265_decode_play,
	h265_decode_pause,
	h265_decode_resume,
	h265_decode_stop,
	h265_decode_end
};


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 *  @brief		Movie Decode ENTropy Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Decode_ENT_Main( VOID )
{
	INT32			ret;
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flgptn;
	OS_USER_FLGPTN	waiptn;
	UINT32			video_index;
	UINT32			next_free_pointer;
	BOOL			stop_wait_flag	= FALSE;
	
#if 0	// for Debug
	OS_USER_SYSTIM					time_before, time_after;
#endif	// for Debug
	
	/* Report ENT Task Initailization End */
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_ENT_INIT_END );
	
	// Check Another Task Result
	waiptn = (FLG_H265_START | FLG_H265_STOP);
	OS_User_Wai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	
#if 0	// for Debug
	OS_User_Get_Tim(&time_before);
#endif	// for Debug
	
/* --- Video Stream Transfer to Decoder Loop Process --- */
	while(1){
		
		/* Check User Stop Request */
		ercd = OS_User_Pol_Flg( FID_H265, FLG_H265_STOP, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			H265_Info_Print(("+++ ENT: H265_STOP Event received\n" ));
			break;
		}
		else if( ercd == D_OS_USER_E_TMOUT ) {
			/* No event */
			;
		}
		else{
			H265_Err_Print(("--- ENT: OS_User_Pol_Flg error!! error=%d\n", ercd ));
		}
		
		if( (gH265_Dec_Ctrl.error == TRUE) || (stop_wait_flag == TRUE) ){
			// In the case of Flush request, it will release the stop waiting.
			ercd = OS_User_Pol_Flg( FID_H265, FLG_H265_FLUSH_OPS, D_OS_USER_TWF_ORW, &flgptn );
			if( ercd == D_OS_USER_E_OK ) {
				stop_wait_flag = FALSE;
			}
			else{
				// wait for dispatch
				OS_User_Dly_Tsk( 10 );
				
				// It waits for a stop from the host.
				continue;
			}
		}
		
		// FLUSH operation detect.
		ercd = OS_User_Pol_Flg( FID_H265, FLG_H265_FLUSH_OPS, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_ENT_STATE_CHANGE );
			
			// Initialize Stream Control.
			Sdram_Map_Movie_Play_H265_Stream_Ctrl_Initialize();
			
			// wait for start/stop
			OS_User_Wai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			// dispatch trigger
			OS_User_Dly_Tsk( 1 );
			continue;
		}
		
		ret = h265_decode_next_video_sample_get( &video_index );
		if( ret != H265_OK ){
			// Video Stream is nothing.
			// Wait until the stream is feed.
			OS_User_Wai_Flg( FID_H265, (FLG_H265_DEC_STREAM_RECEIVE | FLG_H265_FLUSH_OPS | FLG_H265_STOP), D_OS_USER_TWF_ORW, &flgptn );
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_DEC_STREAM_RECEIVE );
			continue;
		}
		
		if( gH265_Dmux_Ctrl.decode_video[video_index].pts > D_H265_DEC_MAX_PTS ){
			H265_Err_Print(("--- ENT: It is possible to have exceeded the PTS to be handled. %d\n", D_H265_DEC_MAX_PTS ));
			gH265_Dec_Ctrl.error = TRUE;
			break;
		}
		
		while(1){
			
#if 0	// for Debug
		H265_Info_Print(("buf_count = %lu\n", gH265_Disp_Ctrl.buf_count ));
#endif
	
#if 0	// for Debug
		OS_User_Get_Tim(&time_after);
		
		H265_Info_Print(("EntTime: %d\n", (UINT32)(time_after - time_before) ));
		
		OS_User_Get_Tim(&time_before);
#endif
			
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_DEC_STREAM_INPUT );
			
			// VCPB_Infor_Table Set
			ret = h265_decode_set_vcpb( video_index, gH265_Disp_Ctrl.vdpb_use_num );
			if( ret == H265_OK ){
				
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Vcpb:%lu", (ULONG)gH265_Dmux_Ctrl.decode_video[video_index].pts ));
#else
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Vcpb:%llu", gH265_Dmux_Ctrl.decode_video[video_index].pts ));
#endif
				
				if( (gH265_Dec_Ctrl.direction == E_H265_IF_PLAY_DIRECTION_RWD) && (gH265_Dec_Ctrl.iframe_only == FALSE) ){
					if( gH265_Dmux_Ctrl.decode_video[video_index].pts == 0 ){
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
						H265_Info_Print(("+++ ENT: Last Frame Detection.[pts:%lu]\n", (ULONG)gH265_Dmux_Ctrl.decode_video[video_index].pts ));
#else
						H265_Info_Print(("+++ ENT: Last Frame Detection.[pts:%llu]\n", gH265_Dmux_Ctrl.decode_video[video_index].pts ));
#endif
						gH265_Dec_Ctrl.last_display_pts = gH265_Dmux_Ctrl.decode_video[video_index].pts;
					}
					if( gH265_Dmux_Ctrl.decode_video[video_index].stream_end_flag == 1 ){
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
						H265_Info_Print(("+++ ENT: Last Stream Detection.[pts:%lu]\n", (ULONG)gH265_Dmux_Ctrl.decode_video[video_index].pts ));
#else
						H265_Info_Print(("+++ ENT: Last Stream Detection.[pts:%llu]\n", gH265_Dmux_Ctrl.decode_video[video_index].pts ));
#endif
						gH265_Dec_Ctrl.last_stream_pts = gH265_Dmux_Ctrl.decode_video[video_index].pts;
						stop_wait_flag = TRUE;
					}
				}
				else if( (gH265_Dec_Ctrl.direction == E_H265_IF_PLAY_DIRECTION_FWD) && (gH265_Dec_Ctrl.iframe_only == FALSE) ){
					if( gH265_Dmux_Ctrl.decode_video[video_index].stream_end_flag == 1 ){
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
						H265_Info_Print(("+++ ENT: Last Stream Detection.[pts:%lu]\n", (ULONG)gH265_Dmux_Ctrl.decode_video[video_index].pts ));
#else
						H265_Info_Print(("+++ ENT: Last Stream Detection.[pts:%llu]\n", gH265_Dmux_Ctrl.decode_video[video_index].pts ));
#endif
						gH265_Dec_Ctrl.last_stream_pts	= gH265_Dmux_Ctrl.decode_video[video_index].pts;
						stop_wait_flag = TRUE;
					}
				}
				else{
					if( gH265_Dmux_Ctrl.decode_video[video_index].stream_end_flag == 1 ){
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
						H265_Info_Print(("+++ ENT: Last Frame/Stream Detection.[pts:%lu]\n", (ULONG)gH265_Dmux_Ctrl.decode_video[video_index].pts ));
#else
						H265_Info_Print(("+++ ENT: Last Frame/Stream Detection.[pts:%llu]\n", gH265_Dmux_Ctrl.decode_video[video_index].pts ));
#endif
						gH265_Dec_Ctrl.last_display_pts	= gH265_Dmux_Ctrl.decode_video[video_index].pts;
						gH265_Dec_Ctrl.last_stream_pts	= gH265_Dmux_Ctrl.decode_video[video_index].pts;
						stop_wait_flag = TRUE;
					}
				}
				
				// Wait flag "Decode Complete" or "FLUSH operation" or "Stop Request".
				OS_User_Wai_Flg( FID_H265, (FLG_H265_DEC_STREAM_INPUT | FLG_H265_FLUSH_OPS | FLG_H265_STOP), D_OS_USER_TWF_ORW, &flgptn );
				if( 0 != ( flgptn & FLG_H265_DEC_STREAM_INPUT )){
					
					OS_User_Clr_Flg( FID_H265, ~FLG_H265_DEC_STREAM_INPUT );
					
					// free vcpb index check.
					next_free_pointer = gH265_Dmux_Ctrl.free_pointer + 1;
					if( next_free_pointer >= D_H265_DEC_SAVE_SAMPLE_MAX_NUM ){
						next_free_pointer = 0;
					}
					
					// Free Stream Area.
					Sdram_Map_Movie_Play_H265_Release_Stream_Area( gH265_Dmux_Ctrl.decode_video[next_free_pointer].stream_address + gH265_Dmux_Ctrl.decode_video[next_free_pointer].stream_size );
					
					// free_pointer update.
					gH265_Dmux_Ctrl.free_pointer = next_free_pointer;
					
					break;
				}
			}
			else{
				OS_User_Dly_Tsk( 1 );	// dispatch trigger
			}
			
			// Check flag "FLUSH operation" or "Stop Request".
			ercd = OS_User_Pol_Flg( FID_H265, (FLG_H265_FLUSH_OPS | FLG_H265_STOP), D_OS_USER_TWF_ORW, &flgptn );
			if( ercd == D_OS_USER_E_OK ) {
				break;
			}
			else if( ercd == D_OS_USER_E_TMOUT ) {
				// No event. Nothing Special
				;
			}
			else{
				H265_Err_Print(("--- ENT: OS_User_Pol_Flg error!! error=%d\n", ercd ));
			}
			if( gH265_Dec_Ctrl.error == TRUE ){
				break;
			}
		}
	}
	
	// Wait CC Task End
	OS_User_Wai_Flg( FID_H265_TASK, FLG_H265_TASK_CC_STOP, D_OS_USER_TWF_ORW, &flgptn );
	
	// Set ENT Task End
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_ENT_STOP );
	
	return;
}

/**
 *  @brief		Movie Decode Codec Core Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Decode_CC_Main( VOID )
{
	INT32				ret;
	OS_USER_ER			ercd;
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn_normal;
	OS_USER_FLGPTN		waiptn;
	UINT32				bank_num;
	T_SR_FM_OUT_INFO	fminfo;
	
#if 0	// for Debug
	OS_USER_SYSTIM					time_before, time_after;
#endif	// for Debug
	
	/* Report CCch Task Initailization End */
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_CC_INIT_END );
	
	// Check Another Task Result
	waiptn = (FLG_H265_START | FLG_H265_STOP);
	OS_User_Wai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	
	// Check Another Task Result
	waiptn_normal = ( FLG_H265_DEC_FRAMEEND_COMP | FLG_H265_DEC_ERROR | FLG_H265_STOP | FLG_H265_CODEC_FLUSH_COMP | FLG_H265_FLUSH_OPS);
	waiptn = waiptn_normal;
	
#if 0	// for Debug
	OS_User_Get_Tim(&time_before);
#endif	// for Debug
	
	while(1){
		OS_User_Wai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		/* Received STOP Event */
		if(      0 != ( flgptn & FLG_H265_STOP )){
			OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_IDX_SET );
			OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_UPDATE );
			H265_Info_Print(("+++ CC: STOP Event received\n"		));
			break;
		}
		else if( 0 != ( flgptn & FLG_H265_DEC_ERROR )){
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_DEC_ERROR );
			if( gH265_Dec_Param.conceal_error == E_H265_IF_CONCEAL_ERROR_STOP ){
				H265_Err_Print(("--- conceal error detected.\n"));
				gH265_Dec_Ctrl.error = TRUE;
			}
			continue;
		}
		
		OS_User_Clr_Flg( FID_H265, ~FLG_H265_DEC_FRAMEEND_COMP );
		
		if( gH265_Dec_Ctrl.error == TRUE ){
			// wait for dispatch
			OS_User_Dly_Tsk( 10 );
			
			// It waits for a stop from the host.
			continue;
		}
		
		// FLUSH complete detect.
		if( flgptn & FLG_H265_CODEC_FLUSH_COMP){
			OS_User_Clr_Flg( FID_H265, ~FLG_H265_CODEC_FLUSH_COMP );
			OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_CC_STATE_CHANGE );
			
			// wait start/stop
			waiptn = ( FLG_H265_START | FLG_H265_STOP );
			OS_User_Wai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			
			// wait pattern resetting (normal)
			waiptn = waiptn_normal;
			continue;
		}
		
		// During the flush, it does not do the normal processing.
		ercd = OS_User_Pol_Flg( FID_H265, FLG_H265_FLUSH_OPS, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			OS_User_Dly_Tsk(10);	// dispatch trigger
			continue;
		}
		
		/* Get Bank Number and VDPB Information as Decode Result. */
		ret = h265_decode_get_vdpb( &fminfo );
		if( ret == H265_OK ){
			OS_User_Dly_Tsk(1);	// dispatch trigger
			OS_User_Set_Flg( FID_H265, FLG_H265_DEC_FRAMEEND_COMP );
		}
		else if( ret == H265_OK_COMPLETE ){
			// Alloc of the image has been completed.
			continue;
		}
		else{
			H265_Err_Print(("--- h265_decode_get_vdpb error. ret=%d\n", ret ));
			gH265_Dec_Ctrl.task_result = H265_ERR_SYSTEM;
			OS_User_Set_Flg( FID_H265, FLG_H265_STOP );
			break;
		}
		
#if 0	// for Debug
		OS_User_Get_Tim(&time_after);
		
		H265_Info_Print(("CcTime: %d\n", (UINT32)(time_after - time_before) ));
		
		OS_User_Get_Tim(&time_before);
#endif
		/* Make Display Control Table */
		bank_num = fminfo.id;
		
		gH265_Disp_Ctrl.inf[bank_num].PTS			= fminfo.pts;
		gH265_Disp_Ctrl.inf[bank_num].DTS			= fminfo.pts;
		gH265_Disp_Ctrl.inf[bank_num].release_flg	= FALSE;
		gH265_Disp_Ctrl.buf_count++;
		gH265_Disp_Ctrl.inf[bank_num].use_flg		= TRUE;
		
#if 0	// for Debug

#if 1	// MILB : Movie porting. printf 64bit Not Supported.
		H265_Info_Print(("decPTS: [%lu] [%lu] %lu\n", bank_num, gH265_Disp_Ctrl.buf_count, (ULONG)gH265_Disp_Ctrl.inf[bank_num].PTS ));
#else
		H265_Info_Print(("decPTS: [%lu] [%lu] %llu\n", bank_num, gH265_Disp_Ctrl.buf_count, gH265_Disp_Ctrl.inf[bank_num].PTS ));
#endif

#endif
		if( gH265_Disp_Ctrl.inf[bank_num].PTS == gH265_Dec_Ctrl.last_stream_pts ){
			gH265_Dec_Ctrl.last_stream_dec_done = TRUE;
		}
		
		// Video Stream Request to HostFW.
		h265_decode_next_video_sample_request();
		
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Vdpb:%lu", (ULONG)gH265_Disp_Ctrl.inf[bank_num].PTS ));
#else
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Vdpb:%llu", gH265_Disp_Ctrl.inf[bank_num].PTS ));
#endif
	}
	
	// Wait DISP Task End
	OS_User_Wai_Flg( FID_H265_TASK, FLG_H265_TASK_DISP_STOP, D_OS_USER_TWF_ORW, &flgptn );
	
	/* Set ENT Task End */
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_CC_STOP );
	
	return;
}


/**
 *  @brief		Movie Decode Display Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Decode_DISP_Main( VOID )
{
	OS_USER_FLGPTN					flgptn;
	OS_USER_FLGPTN					waiptn;
	OS_USER_FLGPTN					pause_waiptn;
	INT32							ret;
	OS_USER_ER						ercd;
	BOOL							stop_wait_flag			= FALSE;
	E_H265_PLAY_MODE				play_mode;
	UCHAR							first_buf_num			= 0;
	BOOL							first_play_flg			= TRUE;	/* Play/Replay: it need 4 banks image */
	BOOL							display_target_flg;
	UCHAR							detect_index			= 0;
	UCHAR							skip_count				= 0;
	UCHAR							retry_cnt				= 0;
	T_H265_DEC_DISP_REFER_INDEX		protect_index			= { 0xFF, 0xFF, 0xFF, 0xFF };
	UINT32							thinning_counter		= 0;
	UINT32							thinning_coefficient	= 0;
	E_H265_DEC_PTS_SEARCH_MODE		pts_search_mode;
#if 0	// for Debug
	UINT32							timer_log_counter = 0;
	OS_USER_SYSTIM					time_before, time_after;
#endif	// for Debug
	
	waiptn			= 0;
	pause_waiptn	= 0;
	
	// Start LCD task.
	if( gH265_Disp_Ctrl.device_info.lcd_info.valid_flag ){
		h265_decode_task_start( TID_H265_DISP_LCD );
		waiptn |= FLG_H265_TASK_DISP_LCD_INIT_END;
		pause_waiptn |= FLG_H265_DECODER_DISP_LCD_PAUSE;
	}
	// Start HDMI task.
	if( gH265_Disp_Ctrl.device_info.hdmi_info.valid_flag ){
		h265_decode_task_start( TID_H265_DISP_HDMI );
		waiptn |= FLG_H265_TASK_DISP_HDMI_INIT_END;
		pause_waiptn |= FLG_H265_DECODER_DISP_HDMI_PAUSE;
	}
	
	// task init end wait.
	OS_User_Wai_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
	
	/* Set Event for Task Start */
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_DISP_INIT_END );
	
#if 0	// for Debug
	OS_User_Get_Tim(&time_before);
#endif	// for Debug
	
	while( 1 ){
#ifdef CO_DEBUG_ON_PC
		OS_User_Dly_Tsk( 200 );
		OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_DEC );
#endif
		/* Wait Sync(Reload Timer) */
		OS_User_Wai_Flg( FID_H265_SYNC, FLG_H265_SYNC_DEC, D_OS_USER_TWF_ORW, &flgptn );
		OS_User_Clr_Flg( FID_H265_SYNC, ~FLG_H265_SYNC_DEC );
		
		ercd = OS_User_Pol_Flg( FID_H265, FLG_H265_STOP, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_IDX_SET );
			OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_UPDATE );
			/* Received STOP Event */
			H265_Info_Print(("+++ DISP: H265_STOP Event received\n" ));
			break;
		}
		
		if( stop_wait_flag == TRUE ){
			// In the case of Flush request, it will release the stop waiting.
			ercd = OS_User_Pol_Flg( FID_H265, FLG_H265_FLUSH_OPS, D_OS_USER_TWF_ORW, &flgptn );
			if( ercd == D_OS_USER_E_OK ) {
				stop_wait_flag = FALSE;
			}
			
			if( gH265_Dec_Ctrl.mode == E_H265_PLAY_MODE_PAUSE ){
				stop_wait_flag = FALSE;
				continue;
			}
			
			// wait for dispatch
			OS_User_Dly_Tsk( 10 );
			
			// It waits for a stop from the host.
			if( gH265_Dec_Ctrl.mode == E_H265_PLAY_MODE_STOP ){
				// Transition to the end processing (setting the FLG_H265_STOP)
				break;
			}
			continue;
		}
		// FLUSH operation detect.
		ercd = OS_User_Pol_Flg( FID_H265, FLG_H265_FLUSH_OPS, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			// Check display task status.
			ercd = OS_User_Pol_Flg( FID_H265_DECODER, pause_waiptn, D_OS_USER_TWF_ANDW, &flgptn );
			if( ercd != D_OS_USER_E_OK ) {
				// It is not already in the pause state.
				OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_PAUSE_REQUEST );
				OS_User_Wai_Flg( FID_H265_DECODER, pause_waiptn, D_OS_USER_TWF_ANDW, &flgptn );
				OS_User_Clr_Flg( FID_H265_DECODER, ~FLG_H265_DECODER_DISP_START );
				
				first_play_flg = TRUE;
			}
			
			protect_index.lcd_decide		= gH265_Disp_Ctrl.refer_index.lcd_decide;
			protect_index.lcd_displaying	= gH265_Disp_Ctrl.refer_index.lcd_displaying;
			protect_index.hdmi_decide		= gH265_Disp_Ctrl.refer_index.hdmi_decide;
			protect_index.hdmi_displaying	= gH265_Disp_Ctrl.refer_index.hdmi_displaying;
			
			gH265_Disp_Ctrl.inf[gH265_Disp_Ctrl.display_index].release_flg = TRUE;
			OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_DISP_STATE_CHANGE );
			continue;
		}
		
		play_mode = gH265_Dec_Ctrl.mode;
		
		if( (gH265_Dec_Ctrl.pause == TRUE) && (first_play_flg == TRUE) ){
			play_mode = E_H265_PLAY_MODE_PLAY;
		}
		
		switch( play_mode ){
			case E_H265_PLAY_MODE_PLAY:
				if( first_play_flg == TRUE ){
					// First : Buffering
					display_target_flg = FALSE;
					
					// thinning setting.
					// ex)
					// coefficient = 0 or 1 -> No thinning.
					// coefficient = 2      -> 1/2 thinning.
					// coefficient = 3      -> 1/3 thinning.
					// coefficient = 4      -> 1/4 thinning.
					thinning_counter = 0;
					
					switch( gH265_Dec_Ctrl.speed ){
						case E_H265_IF_PLAY_SPEED_1_2X:
							thinning_coefficient = 2;
							break;
						case E_H265_IF_PLAY_SPEED_1_4X:
							thinning_coefficient = 4;
							break;
						case E_H265_IF_PLAY_SPEED_1_8X:
							thinning_coefficient = 8;
							break;
						default:
							thinning_coefficient = 0;
							break;
					}
					
					// VDPB free
					if( gH265_Dec_Ctrl.iframe_only == FALSE ){
						h265_decode_vdpb_free( gH265_Dec_Ctrl.direction, protect_index );
					}
					
					if( (gH265_Dec_Ctrl.direction == E_H265_IF_PLAY_DIRECTION_RWD) && (gH265_Dec_Ctrl.iframe_only == FALSE) ){
						
						if( gH265_Dec_Ctrl.trick_limit == TRUE ){
							
							H265_Err_Print(( "--- Disp_Main : It has detected a Trick limit-breaking.\n" ));
							
							// Having detected an error, and shifts to the stop waiting state.
							gH265_Dec_Ctrl.error = TRUE;
							Host_H265_Notify_VideoDecodeCB( E_H265_IF_PLAY_DECODER_STATUS_NORMAL, gH265_Disp_Ctrl.display_pts, gH265_Dec_Ctrl.error );
							stop_wait_flag = TRUE;
							continue;
						}

#ifdef CO_ES1_HARDWARE
						first_buf_num = gH265_Disp_Ctrl.vdpb_use_num - 1; // HEVC Decoder has a delay of 1 frame
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
						first_buf_num = gH265_Disp_Ctrl.vdpb_use_num;
#endif // CO_ES3_HARDWARE
					}
					else{
						if (User_IsDualStream()) {
							first_buf_num = 1;
						} else {
							if( (gH265_Dec_Param.time_scale / gH265_Dec_Param.time_delta) % D_H265_DEC_FIRST_BUFFERING_COEFFICIENT ){
								first_buf_num = ((gH265_Dec_Param.time_scale / gH265_Dec_Param.time_delta) / D_H265_DEC_FIRST_BUFFERING_COEFFICIENT) + 1;
							}else{
								first_buf_num = ((gH265_Dec_Param.time_scale / gH265_Dec_Param.time_delta) / D_H265_DEC_FIRST_BUFFERING_COEFFICIENT);
							}
							if( first_buf_num < 3 ){
								first_buf_num = 3;
							}
							if( first_buf_num > gH265_Disp_Ctrl.vdpb_use_num ){
								first_buf_num = gH265_Disp_Ctrl.vdpb_use_num;
							}
						}
					}
					if ( (gH265_Disp_Ctrl.buf_count >= first_buf_num) || (gH265_Dec_Ctrl.last_stream_dec_done == TRUE) ){
						if( gH265_Dec_Ctrl.iframe_only == FALSE ){	// All frame decode/display
							// Decode waiting of a display picture.
							ret = h265_decode_search_display_index( E_H265_DEC_PTS_SEARCH_MODE_SAME, &detect_index, protect_index );
							if( ret == H265_OK ){
								display_target_flg = TRUE;
							}
						}else{
							// I-frame only decode/display
							display_target_flg = TRUE;
						}
					}
				}else{
					display_target_flg = TRUE;
				}
				
				if( thinning_counter != 0 ){
					display_target_flg = FALSE;
				}
				
				thinning_counter++;
				
				if( thinning_counter >= thinning_coefficient ){
					thinning_counter = 0;
				}
				
				break;
				
			case E_H265_PLAY_MODE_PAUSE:
				
				// Check display task status.
				ercd = OS_User_Pol_Flg( FID_H265_DECODER, pause_waiptn, D_OS_USER_TWF_ANDW, &flgptn );
				if( ercd != D_OS_USER_E_OK ) {
					// It is not already in the pause state.
					OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_PAUSE_REQUEST );
					OS_User_Wai_Flg( FID_H265_DECODER, pause_waiptn, D_OS_USER_TWF_ANDW, &flgptn );
					OS_User_Clr_Flg( FID_H265_DECODER, ~FLG_H265_DECODER_DISP_START );
					
					// change target pts
					gH265_Disp_Ctrl.display_target_pts	= gH265_Disp_Ctrl.display_pts;
					first_play_flg						= TRUE;
					gH265_Dec_Ctrl.pause				= FALSE;
					H265_If_Play_Notify_STATE_CHANGE_COMPLETE( H265_OK );
				}
				
				// VDPB free
				h265_decode_vdpb_free( gH265_Dec_Ctrl.direction, protect_index );
				display_target_flg = FALSE;
				break;
				
			default :	/* Unknown Case */
				display_target_flg = FALSE;
		}
		
		/* Stop Request End */
		if( play_mode == E_H265_PLAY_MODE_STOP ){
			break;
		}
		
		if( gH265_Dec_Ctrl.error == TRUE ){
			// Having detected an error, and shifts to the stop waiting state.
			stop_wait_flag = TRUE;
			continue;
		}
		
		/* It judge need to display image */
		if( display_target_flg == FALSE ){
			continue;
		}
		
		switch( play_mode ){
			case E_H265_PLAY_MODE_PLAY:		// Fallthrough
				
				if( gH265_Dec_Ctrl.iframe_only == FALSE ){	// All frame decode/display
					
					if (User_IsDualStream() &&	!User_IsH265MainStream() && !H264_IsLastPTSSet_Or_Error()) {
						ercd = OS_User_Wai_Flg(FID_H264_DECODER, FLG_H264_DECODER_DISP_IDX_SET, D_OS_USER_TWF_ORW, &flgptn);
						if( ercd != D_OS_USER_E_OK ) H265_Err_Print(( "Disp_Main : OS_User_Wai_Flg [FLG_H264_DECODER_DISP_IDX_SET] NG\n" ));
						ercd = OS_User_Clr_Flg(FID_H264_DECODER, ~FLG_H264_DECODER_DISP_IDX_SET);
						if( ercd != D_OS_USER_E_OK ) H265_Err_Print(( "Disp_Main : OS_User_Clr_Flg [FLG_H264_DECODER_DISP_IDX_SET] NG\n" ));
					}

					if( (gH265_Dec_Ctrl.last_stream_dec_done == TRUE) && (gH265_Dec_Ctrl.last_display_pts == D_H265_DEC_NOT_SET_PTS) ){
						// From the decoded frame, to determine the last PTS.(Maximum of PTS)
						ret = h265_decode_search_display_index( E_H265_DEC_PTS_SEARCH_MODE_BIG, &detect_index, protect_index );
						H265_Info_Print(("+++ DISP: Last Frame Detection.[pts:%lu]\n", (ULONG)gH265_Disp_Ctrl.inf[detect_index].PTS ));
						gH265_Dec_Ctrl.last_display_pts = gH265_Disp_Ctrl.inf[detect_index].PTS;
					}
					
					// Search for an image to be displayed using the "display_target_pts".
					ret = h265_decode_search_display_index( E_H265_DEC_PTS_SEARCH_MODE_SAME, &detect_index, protect_index );
					
					if( ret == H265_OK ){
						if( gH265_Dec_Ctrl.direction == E_H265_IF_PLAY_DIRECTION_FWD ){
							// display_target_pts update
							if( gH265_Dec_Param.scan_mode == E_H265_IF_SCAN_MODE_PROGRESSIVE ){
								gH265_Disp_Ctrl.display_target_pts += gH265_Dec_Param.time_delta;
							}else{
								gH265_Disp_Ctrl.display_target_pts += (gH265_Dec_Param.time_delta * 2);
							}
						}else{
							// display_target_pts update
							if( gH265_Disp_Ctrl.display_target_pts >= gH265_Dec_Param.time_delta ){
								if( gH265_Dec_Param.scan_mode == E_H265_IF_SCAN_MODE_PROGRESSIVE ){
									gH265_Disp_Ctrl.display_target_pts -= gH265_Dec_Param.time_delta;
								}else{
									gH265_Disp_Ctrl.display_target_pts -= (gH265_Dec_Param.time_delta * 2);
								}
							}
						}
					}
				}else{	// I-frame only decode/display
					if( gH265_Dec_Ctrl.direction == E_H265_IF_PLAY_DIRECTION_FWD ){
						pts_search_mode = E_H265_DEC_PTS_SEARCH_MODE_SMALL;
					}else{
						pts_search_mode = E_H265_DEC_PTS_SEARCH_MODE_BIG;
					}
					
					ret = h265_decode_search_display_index( pts_search_mode, &detect_index, protect_index );
					gH265_Disp_Ctrl.display_target_pts = gH265_Disp_Ctrl.inf[detect_index].PTS;
				}
				// Check whether there is a display frame.
				if( ret != H265_OK ){
					/* Not detect sample. so, in this timing, system does not change image data */
					H265_Info_Print(("--- Not detect sample. --- target_pts:%ld\n", (LONG)gH265_Disp_Ctrl.display_target_pts));
					M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Not Detect Sample PTS:%ld", (LONG)gH265_Disp_Ctrl.display_target_pts));
#if 0				// Try 10 times to get that frame if over, break the decoder
					skip_count++;
					if( skip_count > 10 ){
						// Not detect sample. so, in this timing, FS read worst!?
						break;
					}
					continue;
#else				// ** Forcibly continue playing **
					// ** Forced to assign a target pts because display_pts is not updated If not display even once. **
					retry_cnt++;
					if (retry_cnt > 10 && gH265_Disp_Ctrl.display_target_pts != gH265_Dec_Ctrl.last_display_pts) {
						// Try to get next PTS, skip this frame of PTS which is not found
						if( gH265_Dec_Ctrl.iframe_only == FALSE && gH265_Dec_Ctrl.direction == E_H265_IF_PLAY_DIRECTION_FWD ){
							// display_target_pts resume to last one not detected
							if( gH265_Dec_Param.scan_mode == E_H265_IF_SCAN_MODE_PROGRESSIVE ){
								gH265_Disp_Ctrl.display_target_pts += gH265_Dec_Param.time_delta;
							}else{
								gH265_Disp_Ctrl.display_target_pts += (gH265_Dec_Param.time_delta * 2);
							}
						}
						retry_cnt = 0;
					}
					gH265_Disp_Ctrl.display_pts = gH265_Disp_Ctrl.display_target_pts;
					h265_decode_vdpb_free( gH265_Dec_Ctrl.direction, protect_index );
#endif
				} else {
					retry_cnt = 0;
					M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Disp:%lu", (ULONG)gH265_Disp_Ctrl.inf[detect_index].PTS ));
				}
				break;
			default :	/* Not exist case */
				break;
		} // switch( play_mode ){
		
		if( skip_count > 10 ){
			// Having detected an error, and shifts to the stop waiting state.
			gH265_Dec_Ctrl.error = TRUE;
			Host_H265_Notify_VideoDecodeCB( E_H265_IF_PLAY_DECODER_STATUS_NORMAL, gH265_Disp_Ctrl.display_pts, gH265_Dec_Ctrl.error );
			stop_wait_flag = TRUE;
			continue;
		}else{
			// skip_count reset
			skip_count = 0;
		}
		
		ercd = OS_User_Twai_Sem(SID_H265, D_H265_DEC_SEMAPHORE_TIMEOUT);
		if( ercd != D_OS_USER_E_OK ){
			H265_Err_Print(( "--- Disp_Main : gH265_Disp_Ctrl semaphore NG\n" ));
		}
		// information update
		gH265_Disp_Ctrl.display_index	= detect_index;						// display target index

		if (detect_index != 0xFF) {
			OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_IDX_SET );
		}

		if (detect_index != 0xFF) {
			gH265_Disp_Ctrl.display_pts		= gH265_Disp_Ctrl.inf[detect_index].PTS;	// next   :Vsync display area (PTS keep)
			gH265_Disp_Ctrl.display_dts		= gH265_Disp_Ctrl.inf[detect_index].DTS;	//                            (DTS keep)
		}
		
		protect_index.lcd_decide		= gH265_Disp_Ctrl.refer_index.lcd_decide;
		protect_index.lcd_displaying	= gH265_Disp_Ctrl.refer_index.lcd_displaying;
		protect_index.hdmi_decide		= gH265_Disp_Ctrl.refer_index.hdmi_decide;
		protect_index.hdmi_displaying	= gH265_Disp_Ctrl.refer_index.hdmi_displaying;
		
		OS_User_Sig_Sem(SID_H265);
		
		if( first_play_flg == TRUE ){
			OS_User_Clr_Flg( FID_H265_DECODER, ~FLG_H265_DECODER_DISP_PAUSE_REQUEST );
			// Wait the first display.
			if (!User_IsDualStream()) {
				OS_User_Wai_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_START, D_OS_USER_TWF_ORW, &flgptn );
			}
		}

		if (User_IsDualStream() && detect_index != 0xFF) {
			OS_User_Wai_Flg(FID_H265_DECODER, FLG_H265_DECODER_DISP_FRAME_DONE, D_OS_USER_TWF_ORW, &flgptn);
			OS_User_Clr_Flg(FID_H265_DECODER, ~FLG_H265_DECODER_DISP_FRAME_DONE);
		}

		first_play_flg = FALSE;
		
		if( (gH265_Dec_Ctrl.error == TRUE) || (gH265_Disp_Ctrl.display_pts == gH265_Dec_Ctrl.last_display_pts) ){
			// Having detected an error, and shifts to the stop waiting state.
			stop_wait_flag = TRUE;
			
			if( gH265_Dec_Ctrl.pause == TRUE ){
				// In the case of the last frame display by the pause state, not to forcibly stop state.
				stop_wait_flag = FALSE;
			}
		}
		// VDPB free
		h265_decode_vdpb_free( gH265_Dec_Ctrl.direction, protect_index );
	} // while( 1 ){
	
	if (BF_H264_User_Is_Play_Stop()) {
		/* Reload Timer Stop */
		BF_Movie_Common_Timer_Stop( E_BF_MOVIE_COMMON_RTIMER_MODE_DEC );
	}
	
	// Set Stop
	OS_User_Set_Flg( FID_H265, FLG_H265_STOP );
	
	waiptn = 0;
	
	if( gH265_Disp_Ctrl.device_info.lcd_info.valid_flag ){
		waiptn |= FLG_H265_TASK_DISP_LCD_STOP;
	}
	if( gH265_Disp_Ctrl.device_info.hdmi_info.valid_flag ){
		waiptn |= FLG_H265_TASK_DISP_HDMI_STOP;
	}
	
	// LCD/HDMI task end wait.
	OS_User_Wai_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
	
	/* Set Task End */
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_DISP_STOP );
	
	return;
}

/**
 *  @brief		Movie Decode Display Lcd Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Decode_DISP_LCD_Main( VOID )
{
	OS_USER_ER						ercd;
	OS_USER_FLGPTN					flgptn;
	BOOL							first_display_flag		= TRUE;
	UINT8							lcd_decide_index		= 0xFF;
	UINT8							lcd_displaying_index	= 0xFF;
	T_H265_MOVIE_ADDR				dec_addr;
	T_H265_MOVIE_ADDR				disp_addr;
	BOOL							stop_req_flag			= FALSE;
	BOOL							pause_complete_flag		= FALSE;
	BOOL							codec_format;
	E_H265_IF_PLAY_DECODER_STATUS	decoder_status;
	BOOL							update = TRUE;
	USHORT							lcd_img_w;
	USHORT							lcd_img_l;
#if 0	// for Debug
	OS_USER_SYSTIM					time_before, time_after;
#endif	// for Debug
	
	lcd_img_w = gH265_Disp_Ctrl.device_info.lcd_info.image_width;
	lcd_img_l = gH265_Disp_Ctrl.device_info.lcd_info.image_lines;

	// To decide the input format to the DISP.
	if( (gH265_Disp_Ctrl.device_info.lcd_info.image_width == gH265_Dec_Param.image_width) &&
		(gH265_Disp_Ctrl.device_info.lcd_info.image_lines == gH265_Dec_Param.image_lines) &&
		!User_IsDualStream() ){
		
		if( (gH265_Disp_Ctrl.device_info.lcd_info.rotate == E_H265_IF_DISP_ROTATE_90)  ||
			(gH265_Disp_Ctrl.device_info.lcd_info.rotate == E_H265_IF_DISP_ROTATE_180) ||
			(gH265_Disp_Ctrl.device_info.lcd_info.rotate == E_H265_IF_DISP_ROTATE_270) ){
			codec_format = FALSE;
		}
		else{
			codec_format = TRUE;
		}
	}
	else{
		codec_format = FALSE;
	}
	
	/* Set Event for Task Start */
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_DISP_LCD_INIT_END );
	
	if (User_IsDualStream() && !User_IsH265MainStream()) {
		update = FALSE;
	} else {
		update = TRUE;
	}
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "LCD" ));
	
	while(1){
		
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "LCD" ));
		
		H265_Movie_Lcd_Wait();
		
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "LCD" ));
		
		if( stop_req_flag == TRUE ){
			break;
		}
		
		// Disp task stop detect.
		ercd = OS_User_Pol_Flg( FID_H265, FLG_H265_STOP, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_IDX_SET );
			OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_UPDATE );
			// update the last image before the end.
			stop_req_flag = TRUE;
			
			// If last_picture_keep is FALSE, it does black back setting.
			if( gH265_Disp_Ctrl.last_picture_keep == FALSE ){
				FJ_Disp_Video_Disable( gH265_Disp_Ctrl.device_info.lcd_info.disp_id );
			}
		}
		
		// PAUSE operation detect.
		ercd = OS_User_Pol_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_PAUSE_REQUEST, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			if( pause_complete_flag == FALSE ){
				// Set the TRUE to pause_complete_flag, and perform the image update.
				pause_complete_flag = TRUE;
			}
			else{
				OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_LCD_PAUSE );
				first_display_flag = TRUE;
				continue;
			}
		}
		else{
			OS_User_Clr_Flg( FID_H265_DECODER, ~FLG_H265_DECODER_DISP_LCD_PAUSE );
			pause_complete_flag = FALSE;
		}
		
		ercd = OS_User_Twai_Sem(SID_H265, D_H265_DEC_SEMAPHORE_TIMEOUT);
		if( ercd != D_OS_USER_E_OK ){
			H265_Err_Print(( "--- H265_Decode_DISP_LCD_Main : gH265_Disp_Ctrl semaphore NG\n" ));
		}
		
		if( gH265_Disp_Ctrl.display_index == 0xFF ){
			// Still there is no image to be displayed.
			OS_User_Sig_Sem(SID_H265);
			continue;
		}
		
		lcd_displaying_index	= lcd_decide_index;
		lcd_decide_index		= gH265_Disp_Ctrl.display_index;
		
		gH265_Disp_Ctrl.refer_index.lcd_decide		= lcd_decide_index;
		gH265_Disp_Ctrl.refer_index.lcd_displaying	= lcd_displaying_index;
		
		if( pause_complete_flag == TRUE ){
			if( lcd_decide_index != lcd_displaying_index ){
				pause_complete_flag = FALSE;
			}
		}
		
		OS_User_Sig_Sem(SID_H265);
		
		if( (lcd_decide_index == lcd_displaying_index) && (first_display_flag == FALSE) ){
			// No need resize image.
			continue;
		}
		/* Get Decode Area Address */
		Sdram_Map_Movie_Play_H265_Get_Vdec( lcd_decide_index, &dec_addr.y_address, &dec_addr.c_address );

		if (User_IsStreamingProcess() && User_IsDualStream() &&	!User_IsH265MainStream() && !H264_IsLastPTSDisplay_Or_Error()) {
			ercd = OS_User_Wai_Flg(FID_H264_DECODER, FLG_H264_DECODER_DISP_UPDATE, D_OS_USER_TWF_ORW, &flgptn);
			if( ercd != D_OS_USER_E_OK ) H265_Err_Print(( "Disp_Main : OS_User_Wai_Flg [FLG_H264_DECODER_DISP_UPDATE] NG\n" ));
			ercd = OS_User_Clr_Flg(FID_H264_DECODER, ~FLG_H264_DECODER_DISP_UPDATE);
			if( ercd != D_OS_USER_E_OK ) H265_Err_Print(( "Disp_Main : OS_User_Clr_Flg [FLG_H264_DECODER_DISP_UPDATE] NG\n" ));
		}

		if (User_IsStreamingProcess() && User_IsDualStream() &&	!User_IsH265MainStream() &&
			H264_IsLastPTSDisplay_Or_Error() && (H264_GetLastPTS() != gH265_Disp_Ctrl.inf[lcd_decide_index].PTS)) {
			update = TRUE;
			gH265_Disp_Ctrl.device_info.lcd_info.image_width = lcd_img_w << 1;
			gH265_Disp_Ctrl.device_info.lcd_info.image_lines = lcd_img_l << 1;
		}

		if (User_IsStreamingProcess() && User_IsDualStream() && H264_IsLastPTSDisplay_Or_Error() &&
			(H264_GetLastPTS() != gH265_Disp_Ctrl.inf[lcd_decide_index].PTS)) {
			if( (gH265_Disp_Ctrl.device_info.lcd_info.image_width == gH265_Dec_Param.image_width) &&
				(gH265_Disp_Ctrl.device_info.lcd_info.image_lines == gH265_Dec_Param.image_lines)) {

				if( (gH265_Disp_Ctrl.device_info.lcd_info.rotate == E_H265_IF_DISP_ROTATE_90)  ||
					(gH265_Disp_Ctrl.device_info.lcd_info.rotate == E_H265_IF_DISP_ROTATE_180) ||
					(gH265_Disp_Ctrl.device_info.lcd_info.rotate == E_H265_IF_DISP_ROTATE_270) ){
					codec_format = FALSE;
				}else{
					gH265_Disp_Ctrl.device_info.lcd_info.global_width = gH265_Dec_Param.image_bank_y_width;
					gH265_Disp_Ctrl.device_info.lcd_info.global_lines = gH265_Dec_Param.image_bank_y_lines;
					codec_format = TRUE;
				}
			}else{
				codec_format = FALSE;
			}
		}

		/* Get Display Area address */
		H265_If_Play_Notify_LCD_UPDATE( &disp_addr.y_address, &disp_addr.c_address, update);
		
#if 0	// for Debug
		OS_User_Get_Tim(&time_before);
#endif	// for Debug

		h265_decode_display_image( (T_H265_IF_DISPLAY_PARAM*)&gH265_Disp_Ctrl.device_info.lcd_info, &dec_addr, &disp_addr, codec_format );
		
#if 0	// for Debug
		OS_User_Get_Tim(&time_after);
		H265_Info_Print(("resize_time: %d\n", (UINT32)(time_after - time_before) ));
#endif	// for Debug
		
// CO_DEBUG_RS_PLAY_MEDIA_OFF
		// [CP3 board check] ICE break and dump. Movie Play
// CO_DEBUG_RS_PLAY_MEDIA_OFF
		
		/* Set Image Data */
		H265_Movie_Lcd_Update( (T_H265_IF_DISPLAY_PARAM*)&gH265_Disp_Ctrl.device_info.lcd_info, &disp_addr, codec_format );

		OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_UPDATE );
		OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_FRAME_DONE );

		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "DDisp:%lu, addr:%x", (ULONG)gH265_Disp_Ctrl.inf[lcd_decide_index].PTS, disp_addr.y_address ));
		
		if( gH265_Disp_Ctrl.inf[lcd_decide_index].PTS == gH265_Dec_Ctrl.last_display_pts ){
			decoder_status = E_H265_IF_PLAY_DECODER_STATUS_LAST;
			gH265_Disp_Ctrl.last_pts_display = TRUE;
		}
		else{
			decoder_status = E_H265_IF_PLAY_DECODER_STATUS_NORMAL;
		}
		
		if( first_display_flag == TRUE ){
			// first display
			Host_H265_Notify_FirstFrameDisplayCB( decoder_status, gH265_Disp_Ctrl.inf[lcd_decide_index].PTS, gH265_Dec_Ctrl.error );
			first_display_flag = FALSE;
			OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_START );
		}
		else{
			// normal display
			Host_H265_Notify_VideoDecodeCB( decoder_status, gH265_Disp_Ctrl.inf[lcd_decide_index].PTS, gH265_Dec_Ctrl.error );
		}
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "LCD" ));
	
	/* Set Task End */
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_DISP_LCD_STOP );
	
	return;
}

/**
 *  @brief		Movie Decode Display Hdmi Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Decode_DISP_HDMI_Main( VOID )
{
	OS_USER_ER						ercd;
	OS_USER_FLGPTN					flgptn;
	BOOL							first_display_flag		= TRUE;
	UINT8							hdmi_decide_index		= 0xFF;
	UINT8							hdmi_displaying_index	= 0xFF;
	T_H265_MOVIE_ADDR				dec_addr;
	T_H265_MOVIE_ADDR				disp_addr;
	BOOL							stop_req_flag			= FALSE;
	BOOL							pause_complete_flag		= FALSE;
	BOOL							codec_format;
	E_H265_IF_PLAY_DECODER_STATUS	decoder_status;
	BOOL							update = TRUE;
#if 0	// for Debug
	OS_USER_SYSTIM					time_before, time_after;
#endif	// for Debug
	USHORT							hdmi_img_w;
	USHORT							hdmi_img_l;

	hdmi_img_w = gH265_Disp_Ctrl.device_info.hdmi_info.image_width;
	hdmi_img_l = gH265_Disp_Ctrl.device_info.hdmi_info.image_lines;
	// To decide the input format to the DISP.
	if( (gH265_Disp_Ctrl.device_info.hdmi_info.image_width == gH265_Dec_Param.image_width) &&
		(gH265_Disp_Ctrl.device_info.hdmi_info.image_lines == gH265_Dec_Param.image_lines) &&
		 !User_IsDualStream() ){
		
		if( (gH265_Disp_Ctrl.device_info.hdmi_info.rotate == E_H265_IF_DISP_ROTATE_90)  ||
			(gH265_Disp_Ctrl.device_info.hdmi_info.rotate == E_H265_IF_DISP_ROTATE_180) ||
			(gH265_Disp_Ctrl.device_info.hdmi_info.rotate == E_H265_IF_DISP_ROTATE_270) ){
			codec_format = FALSE;
		}
		else{
			codec_format = TRUE;
		}
	}
	else{
		codec_format = FALSE;
	}
	
	/* Set Event for Task Start */
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_DISP_HDMI_INIT_END );
	
	if (User_IsDualStream() && !User_IsH265MainStream()) {
		update = FALSE;
	} else {
		update = TRUE;
	}

//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "HDMI" ));
	
	while(1){
		
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "HDMI" ));
		
		H265_Movie_Hdmi_Wait();
		//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "DD S"));
		
		if( stop_req_flag == TRUE ){
			break;
		}
		
		// Disp task stop detect.
		ercd = OS_User_Pol_Flg( FID_H265, FLG_H265_STOP, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_IDX_SET );
			OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_UPDATE );
			// update the last image before the end.
			stop_req_flag = TRUE;
			
			// If last_picture_keep is FALSE, it does black back setting.
			if( gH265_Disp_Ctrl.last_picture_keep == FALSE ){
				FJ_Disp_Video_Disable( gH265_Disp_Ctrl.device_info.hdmi_info.disp_id );
			}
		}
		
		// PAUSE operation detect.
		ercd = OS_User_Pol_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_PAUSE_REQUEST, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			if( pause_complete_flag == FALSE ){
				// Set the TRUE to pause_complete_flag, and perform the image update.
				pause_complete_flag = TRUE;
			}
			else{
				OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_HDMI_PAUSE );
				first_display_flag = TRUE;
				continue;
			}
		}
		else{
			OS_User_Clr_Flg( FID_H265_DECODER, ~FLG_H265_DECODER_DISP_HDMI_PAUSE );
			pause_complete_flag = FALSE;
		}
		
		ercd = OS_User_Twai_Sem(SID_H265, D_H265_DEC_SEMAPHORE_TIMEOUT);
		if( ercd != D_OS_USER_E_OK ){
			H265_Err_Print(( "--- H265_Decode_DISP_HDMI_Main : gH265_Disp_Ctrl semaphore NG\n" ));
		}
		
		if( gH265_Disp_Ctrl.display_index == 0xFF ){
			// Still there is no image to be displayed.
			OS_User_Sig_Sem(SID_H265);
			continue;
		}
		
		hdmi_displaying_index	= hdmi_decide_index;
		hdmi_decide_index		= gH265_Disp_Ctrl.display_index;
		
		gH265_Disp_Ctrl.refer_index.hdmi_decide		= hdmi_decide_index;
		gH265_Disp_Ctrl.refer_index.hdmi_displaying	= hdmi_displaying_index;
		
		if( pause_complete_flag == TRUE ){
			if( hdmi_decide_index != hdmi_displaying_index ){
				pause_complete_flag = FALSE;
			}
		}
		
		OS_User_Sig_Sem(SID_H265);
		
		if( (hdmi_decide_index == hdmi_displaying_index) && (first_display_flag == FALSE) ){
			// No need resize image.
			continue;
		}
		/* Get Decode Area Address */
		Sdram_Map_Movie_Play_H265_Get_Vdec( hdmi_decide_index, &dec_addr.y_address, &dec_addr.c_address );

		if (User_IsStreamingProcess() && User_IsDualStream() &&	!User_IsH265MainStream() && !H264_IsLastPTSDisplay_Or_Error()) {
			ercd = OS_User_Wai_Flg(FID_H264_DECODER, FLG_H264_DECODER_DISP_UPDATE, D_OS_USER_TWF_ORW, &flgptn);
			if( ercd != D_OS_USER_E_OK ) H265_Err_Print(( "Disp_Main : OS_User_Wai_Flg [FLG_H264_DECODER_DISP_UPDATE] NG\n" ));
			ercd = OS_User_Clr_Flg(FID_H264_DECODER, ~FLG_H264_DECODER_DISP_UPDATE);
			if( ercd != D_OS_USER_E_OK ) H265_Err_Print(( "Disp_Main : OS_User_Clr_Flg [FLG_H264_DECODER_DISP_UPDATE] NG\n" ));
		}

		if (User_IsStreamingProcess() && User_IsDualStream() &&	!User_IsH265MainStream() &&
			H264_IsLastPTSDisplay_Or_Error() && (H264_GetLastPTS() != gH265_Disp_Ctrl.inf[hdmi_decide_index].PTS)) {
			update = TRUE;
			gH265_Disp_Ctrl.device_info.hdmi_info.image_width = hdmi_img_w << 1;
			gH265_Disp_Ctrl.device_info.hdmi_info.image_lines = hdmi_img_l << 1;
		}

		if (User_IsStreamingProcess() && User_IsDualStream() && H264_IsLastPTSDisplay_Or_Error() &&
			(H264_GetLastPTS() != gH265_Disp_Ctrl.inf[hdmi_decide_index].PTS)) {
			if( (gH265_Disp_Ctrl.device_info.hdmi_info.image_width == gH265_Dec_Param.image_width) &&
				(gH265_Disp_Ctrl.device_info.hdmi_info.image_lines == gH265_Dec_Param.image_lines)) {

				if( (gH265_Disp_Ctrl.device_info.hdmi_info.rotate == E_H265_IF_DISP_ROTATE_90)  ||
					(gH265_Disp_Ctrl.device_info.hdmi_info.rotate == E_H265_IF_DISP_ROTATE_180) ||
					(gH265_Disp_Ctrl.device_info.hdmi_info.rotate == E_H265_IF_DISP_ROTATE_270) ){
					codec_format = FALSE;
				}else{
					gH265_Disp_Ctrl.device_info.hdmi_info.global_width = gH265_Dec_Param.image_bank_y_width;
					gH265_Disp_Ctrl.device_info.hdmi_info.global_lines = gH265_Dec_Param.image_bank_y_lines;
					codec_format = TRUE;
				}
			}else{
				codec_format = FALSE;
			}
		}

		/* Get Display Area address */
		H265_If_Play_Notify_HDMI_UPDATE( &disp_addr.y_address, &disp_addr.c_address, update );

#if 0	// for Debug
		OS_User_Get_Tim(&time_before);
#endif	// for Debug

		//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "DD C"));
		h265_decode_display_image( (T_H265_IF_DISPLAY_PARAM*)&gH265_Disp_Ctrl.device_info.hdmi_info, &dec_addr, &disp_addr, codec_format );
		//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "DD C"));

#if 0	// for Debug
		OS_User_Get_Tim(&time_after);
		H265_Info_Print(("resize_time: %d\n", (UINT32)(time_after - time_before) ));
#endif	// for Debug

		if (User_IsStreamingProcess() && User_IsDualStream() &&	User_IsH265MainStream() && !H264_IsLastPTSDisplay_Or_Error()) {
			Sdram_Map_Play_Free_HDMI_LockIdx();
		} else {
			/* Set Image Data */
			H265_Movie_Hdmi_Update( (T_H265_IF_DISPLAY_PARAM*)&gH265_Disp_Ctrl.device_info.hdmi_info, &disp_addr, codec_format );
		}

		OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_UPDATE );
		OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_FRAME_DONE );

		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "DDisp:%lu, addr:%x", (ULONG)gH265_Disp_Ctrl.inf[hdmi_decide_index].PTS, disp_addr.y_address ));

		if( gH265_Disp_Ctrl.device_info.lcd_info.valid_flag == FALSE ){
			if( gH265_Disp_Ctrl.inf[hdmi_decide_index].PTS == gH265_Dec_Ctrl.last_display_pts ){
				decoder_status = E_H265_IF_PLAY_DECODER_STATUS_LAST;
				gH265_Disp_Ctrl.last_pts_display = TRUE;
			}
			else{
				decoder_status = E_H265_IF_PLAY_DECODER_STATUS_NORMAL;
			}
			
			if( first_display_flag == TRUE ){
				// first display (If there is no LCD.)
				Host_H265_Notify_FirstFrameDisplayCB( decoder_status, gH265_Disp_Ctrl.inf[hdmi_decide_index].PTS, gH265_Dec_Ctrl.error );
				first_display_flag = FALSE;
				OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_START );
			}
			else{
				// normal display (If there is no LCD.)
				Host_H265_Notify_VideoDecodeCB( decoder_status, gH265_Disp_Ctrl.inf[hdmi_decide_index].PTS, gH265_Dec_Ctrl.error );
			}
		}
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "HDMI" ));
	
	/* Set Task End */
	OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_DISP_HDMI_STOP );
	
	return;
}


/**
 *  @brief		Decode Main Function.\n
 *	@param		file_id			i:File Identifier
 *	@param		play_index		i:Table Index
 *	@return		H265_OK					(0)Normal End
 *	@return		H265_ERR_INPUT_PARAM		(1)Input Parameter Error
 *	@return		H265_ERR_FILE_ACCESS		(8)File Access Error
 *	@return		H265_ERR_FILE_NO_SUPPORT	(9)Not Support File Error
 *	@return		H265_ERR_SYSTEM			(99)System Fault
 *	@note		None
 *	@attention	None
 */
INT32 H265_Decode_Main( VOID )
{
	INT32				ret;
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn;
	OS_USER_ER			ercd;
	UINT32				wait_count;
	UCHAR				main_skip_flg;
	
/* --- 3. Initialize/Change Decoder and Set condition --- */
	ret = h265_decode_init_param();
	if( ret != H265_OK ){
		H265_If_Play_Notify_STOP_COMPLETE();
		return ret;
	}
	
/* --- 7. Initialize Audio Process and Execute some tasks --- */
	/* ENTropy Task */
	h265_decode_task_start( TID_H265_ENT );
	/* CodecCore Task */
	h265_decode_task_start( TID_H265_CC );
	/* DISP Task */
	h265_decode_task_start( TID_H265_DISP );
	
	/* Confirm All of Task Status */
	waiptn = FLG_H265_TASK_ENT_INIT_END | FLG_H265_TASK_CC_INIT_END | FLG_H265_TASK_DISP_INIT_END;
	
	wait_count = 0;
	
	/* Wait Task Start Status */
	while(1){
		/* Check Task Status */
		ercd = OS_User_Pol_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			break;
		}
		else if( ercd == D_OS_USER_E_TMOUT ) {
			if( wait_count > 300 ){
				H265_Err_Print(("--- Main: It cannot change task status!! waiptn=%lx, flgptn=%lx\n", waiptn, flgptn ));
				h265_decode_task_forced_stop();
				H265_If_Play_Notify_STOP_COMPLETE();
				return H265_ERR_SYSTEM;
			}
		}
		else{	// error
			H265_Err_Print(("--- Main: Wait task start. OS_User_Pol_Flg ercd=%x\n", ercd ));
			h265_decode_task_forced_stop();
			H265_If_Play_Notify_STOP_COMPLETE();
			return H265_ERR_SYSTEM;
		}
		OS_User_Dly_Tsk(10);
		wait_count++;
	}
	
	/* Check Task Result Status before Recording */
	waiptn = FLG_H265_TASK_ENT_STOP | FLG_H265_TASK_CC_STOP | FLG_H265_TASK_DISP_STOP;
	
	ercd = OS_User_Pol_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	if( ercd == D_OS_USER_E_OK ) {
		H265_Err_Print(("--- Main: It found MOVIE Task End before Play Start. flgptn=%lx\n", flgptn ));
		OS_User_Set_Flg( FID_H265, FLG_H265_STOP );
		H265_If_Play_Notify_STOP_COMPLETE();
		return H265_ERR_SYSTEM;
	}
	
	H265_Info_Print(("\n"));
	H265_Info_Print(("+++ Main: MOVIE PLAY INIT ====\n"));
	
	/* It set "Initialization" to current status */
	H265_If_Play_Api_Set_Status( E_H265_IF_PLAY_STATUS_INIT );
	
	H265_If_Play_Notify_STATE_CHANGE_COMPLETE( H265_OK );
	
/* --- 9. Read Audio/Video Stream, and Check Event and File read end  --- */
	while(1){
		OS_User_Dly_Tsk(1);
		/* 8.1 Check Task Result */
		ercd = OS_User_Pol_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ){
			gH265_Dec_Ctrl.mode			= E_H265_PLAY_MODE_END;
			gH265_Dec_Ctrl.next_mode	= E_H265_PLAY_MODE_END;
			H265_Info_Print(("H265_Decode_Main : Stop Detected.(flgptn=%#lx)\n", flgptn));
			break;
		}
		else if( ercd == D_OS_USER_E_TMOUT ) {
			;
		}
		else{
			H265_Err_Print(("--- MP4: OS_User_Pol_Flg(8.1) ercd=%x\n", ercd ));
		}
		
		main_skip_flg = H265_OFF;
		
		if( gH265_Dec_Ctrl.next_mode == gH265_Dec_Ctrl.mode ){
			OS_User_Dly_Tsk(10);
			main_skip_flg = H265_ON;
		}
		
		/* According to Play Mode, it execute main part */
		if( main_skip_flg == H265_OFF ){
			gH265_Dec_Ctrl.mode = h265_dec_play_func[ gH265_Dec_Ctrl.mode ]();
		}
		
		/* Check User Request */
		if( gH265_Dec_Ctrl.mode != E_H265_PLAY_MODE_STOP ){
			if( gH265_Dec_Ctrl.mode != gH265_Dec_Ctrl.next_mode ){
				gH265_Dec_Ctrl.mode = gH265_Dec_Ctrl.next_mode;
			}
			else{
				gH265_Dec_Ctrl.next_mode = H265_Movie_Play_Event_Check( gH265_Dec_Ctrl.mode );
			}
		}
	}
	
	// dummy sync for top image display
	OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_DEC );
	
	waiptn = FLG_H265_TASK_ENT_STOP | FLG_H265_TASK_CC_STOP | FLG_H265_TASK_DISP_STOP;
/* --- 10. Wait Task End  --- */
	wait_count = 0;
	while(1){
		/* Check Task Status */
		ercd = OS_User_Pol_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			break;
		}
		else if( ercd == D_OS_USER_E_TMOUT ) {
			if( wait_count > 200 ){
				H265_Err_Print(("--- Main: Task End.\n"));
				h265_decode_task_forced_stop();
				break;
			}
		}
		else{
			H265_Err_Print(("--- MP4: Task End OS_User_Pol_Flg ercd=%x\n", ercd ));
			h265_decode_task_forced_stop();
			break;
		}
		OS_User_Dly_Tsk(10);
		wait_count++;
	}
	
/* --- 11. Stop Sirius Decoder	--- */
	H265_Sirius_Dec_Stop();
	H265_Sirius_Finalize();
	
/* --- 14. Report Decode End to U.I. */
	H265_If_Play_Notify_STOP_COMPLETE();
	
	H265_Info_Print(("+++ Main: MOVIE PLAY END!! ====\n"));
	
	return H265_OK;
}


/**
 *  @brief		Initialize Play Link Table and File Format Memory Table.\n
 *	@param		None
 *	@return		H265_OK						(0)Normal End
 *	@note		None
 *	@attention	None
 */
INT32 H265_Decode_Play_Start( ULLONG start_pts, E_H265_IF_PLAY_DIRECTION direction, E_H265_IF_PLAY_SPEED speed, BOOL pause )
{
	INT32	ret;
	UINT32	scan_mode;
	UINT32	timer_set_time_scale=0;
	UINT32	timer_set_time_delta=0;
	UINT32	sirius_playback_mode;
	
	gH265_Disp_Ctrl.display_target_pts	= start_pts;
	gH265_Disp_Ctrl.display_pts			= start_pts;
	gH265_Dec_Ctrl.direction			= direction;
	gH265_Dec_Ctrl.speed				= speed;
	gH265_Dec_Ctrl.pause				= pause;
	
	switch( speed ){
		case E_H265_IF_PLAY_SPEED_1X:
			timer_set_time_scale		= gH265_Dec_Param.time_scale;
			timer_set_time_delta		= gH265_Dec_Param.time_delta;
			gH265_Dec_Ctrl.iframe_only	= FALSE;
			break;
		
		case E_H265_IF_PLAY_SPEED_2X:
			timer_set_time_scale		= D_H265_DEC_TIMER_FFREW_2X_TIME_SCALE;
			timer_set_time_delta		= D_H265_DEC_TIMER_FFREW_2X_TIME_DELTA;
			gH265_Dec_Ctrl.iframe_only	= TRUE;
			break;
		
		case E_H265_IF_PLAY_SPEED_4X:
			timer_set_time_scale		= D_H265_DEC_TIMER_FFREW_4X_TIME_SCALE;
			timer_set_time_delta		= D_H265_DEC_TIMER_FFREW_4X_TIME_DELTA;
			gH265_Dec_Ctrl.iframe_only	= TRUE;
			break;
		
		case E_H265_IF_PLAY_SPEED_8X:
			timer_set_time_scale		= D_H265_DEC_TIMER_FFREW_8X_TIME_SCALE;
			timer_set_time_delta		= D_H265_DEC_TIMER_FFREW_8X_TIME_DELTA;
			gH265_Dec_Ctrl.iframe_only	= TRUE;
			break;
		
		case E_H265_IF_PLAY_SPEED_1_2X:
			timer_set_time_scale		= gH265_Dec_Param.time_scale;
			timer_set_time_delta		= gH265_Dec_Param.time_delta;
			gH265_Dec_Ctrl.iframe_only	= FALSE;
			break;
		
		case E_H265_IF_PLAY_SPEED_1_4X:
			timer_set_time_scale		= gH265_Dec_Param.time_scale;
			timer_set_time_delta		= gH265_Dec_Param.time_delta;
			gH265_Dec_Ctrl.iframe_only	= FALSE;
			break;
		
		case E_H265_IF_PLAY_SPEED_1_8X:
			timer_set_time_scale		= gH265_Dec_Param.time_scale;
			timer_set_time_delta		= gH265_Dec_Param.time_delta;
			gH265_Dec_Ctrl.iframe_only	= FALSE;
			break;
		
		default :
			H265_Err_Print(("--- BF_Movie_Common_Timer_Start Error.\n"));
			break;
	}
	
	if( gH265_Dec_Param.scan_mode == E_H265_IF_SCAN_MODE_PROGRESSIVE ){
		scan_mode = D_BF_MOVIE_COMMON_SCAN_MODE_PROGRESSIVE;
	}
	else{
		scan_mode = D_BF_MOVIE_COMMON_SCAN_MODE_INTERLACE;
	}
	
	if( (gH265_Dec_Ctrl.direction==E_H265_IF_PLAY_DIRECTION_FWD) && (gH265_Dec_Ctrl.iframe_only==FALSE) ){
		sirius_playback_mode = SR_DECODE_FORWARD;
	}
	else if( (gH265_Dec_Ctrl.direction==E_H265_IF_PLAY_DIRECTION_RWD) && (gH265_Dec_Ctrl.iframe_only==FALSE) ){
		sirius_playback_mode = SR_DECODE_BACKWARD;
	}
	else if( (gH265_Dec_Ctrl.direction==E_H265_IF_PLAY_DIRECTION_FWD) && (gH265_Dec_Ctrl.iframe_only==TRUE) ){
		sirius_playback_mode = SR_DECODE_FF;
	}
	else if( (gH265_Dec_Ctrl.direction==E_H265_IF_PLAY_DIRECTION_RWD) && (gH265_Dec_Ctrl.iframe_only==TRUE) ){
		sirius_playback_mode = SR_DECODE_REW;
	}
	else{
		H265_Err_Print(("H265_Decode_Play_Start : Abnormal root.\n"));
		return H265_NG;
	}
	
	// Decode Start Request.
	ret = H265_Sirius_Dec_Start( sirius_playback_mode );
	if( ret != H265_OK ){
		H265_Err_Print(("H265_Decode_Play_Start : H265_Sirius_Dec_Start NG\n"));
		return H265_NG;
	}
	
	/* Decide Reload Timer */
	BF_Movie_Common_Timer_Calculate( timer_set_time_delta, timer_set_time_scale, scan_mode );
	
	/* Reload Timer Enable */
	ret = BF_Movie_Common_Timer_Start( E_BF_MOVIE_COMMON_RTIMER_MODE_DEC );

	if( ret != H265_OK ){
		H265_Err_Print(("--- BF_Movie_Common_Timer_Start Error.\n"));
		// Dont stop timer since dual decoding H264/H265 open timer twice
		/* Reload Timer Stop */
		//BF_Movie_Common_Timer_Stop( E_BF_MOVIE_COMMON_RTIMER_MODE_DEC );
		//return H265_NG;
	}

	/* Send Start Event */
	OS_User_Set_Flg( FID_H265, FLG_H265_START );
	
	return H265_OK;
}


/**
 *  @brief		Initialize Play Link Table and File Format Memory Table.\n
 *	@param		None
 *	@return		H265_OK						(0)Normal End
 *	@note		None
 *	@attention	None
 */
INT32 H265_Decode_Play_Initialize( VOID )
{
	return H265_OK;
}

/**
 *  @brief		Release Play Link Table.\n
 *	@param		None
 *	@return		H265_OK						(0)Normal End
 *	@note		None
 *	@attention	None
 */
INT32 H265_Decode_Play_Release( VOID )
{
	return H265_OK;
}


/**
 *  @brief		Delete Play Link Table.\n
 *	@param		i:play_index		sysmode_play Link Table Index
 *	@return		H265_OK						(0)Normal End
 *	@return		H265_ERR_INPUT_PARAM			(1)Input Parameter Error
 *	@note		None
 *	@attention	None
 */
INT32 H265_Decode_Play_Delete( UCHAR play_index )
{
	return H265_OK;
}


/**
 *  @brief		Delete Play Link Table.\n
 *	@param		i:play_info			Play Informaton
 *	@param		i:play_index		sysmode_play Link Table Index
 *	@return		H265_OK						(0)Normal End
 *	@return		H265_ERR_INPUT_PARAM			(1)Input Parameter Error
 *	@note		None
 *	@attention	None
 */
INT32 H265_Decode_Top_Init( T_H265_IF_DECODER_PARAM* decoder_param, T_H265_IF_DISPLAY_INFO* display_info )
{
	if( decoder_param == NULL){
		return H265_ERR_INPUT_PARAM;
	}
	if( display_info == NULL){
		return H265_ERR_INPUT_PARAM;
	}
	h265_decode_initialize( decoder_param, display_info );
	
	return 	H265_OK;
}


/**
 *  @brief		Top Image Display Main Processing.\n
 *	@param		i:play_info			Play Informaton
 *	@param		i:disp_size			Display size
 *	@param		i:va_adr_y			VRAM Y Address of Codec Format
 *	@param		i:play_index		sysmode_play Link Table Index(Max.8)
 *	@param		i:dec_flag			decode execution flag\n
 *									H265_ON  : Decode Execution     -> (mov/mp4 free_atom only) info get & YCC420 get
 *									H265_OFF : Decode Not Execution -> (mov/mp4 free_atom only) spread atom 
 *	@return		H265_OK					(0)Normal End
 *	@return		H265_ERR_INPUT_PARAM		(1)Input Parameter Error
 *	@return		H265_ERR_FILE_ACCESS		(8)File Access Error
 *	@return		H265_ERR_FILE_NO_SUPPORT	(9)Not Support File Error
 *	@note		None
 *	@attention	None
 */
INT32 H265_Decode_Top_Main( ULLONG req_pts, T_H265_IF_VIDEO_YUV_INFO* decode_yuv_info )
{
	INT32 ret;
	
	ret = h265_decode_top_image_get( req_pts, decode_yuv_info );
	if( ret != H265_OK ){
		H265_Err_Print(("--- TOP: h265_decode_top_image_get Error=%d\n", ret ));
		return ret;
	}
	
	return 	H265_OK;
}

INT32 H265_Decode_Video_Stream_Set( UINT32 store_num, T_H265_IF_PARAM_SET_STREAM* video_stream_param )
{
	UINT32	next_write_pointer;
	UINT32	free_pointer;
	UINT32	stream_loop;
	
	// free area check
	next_write_pointer	= gH265_Dmux_Ctrl.write_pointer;
	free_pointer		= gH265_Dmux_Ctrl.free_pointer;
	
	for( stream_loop=0; stream_loop<store_num; stream_loop++){
		next_write_pointer++;
		
		if( next_write_pointer >= D_H265_DEC_SAVE_SAMPLE_MAX_NUM ){
			next_write_pointer = 0;
		}
		if( next_write_pointer == free_pointer ){
			// When passing through this route, there is a problem in the control of Stream request.
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
//			H265_Info_Print(("--- H265_Decode_Video_Stream_Set: The number of settings beyond the Stream management area. pts=%lu\n", (ULONG)video_stream_param[stream_loop].pts ));
#else
//			H265_Info_Print(("--- H265_Decode_Video_Stream_Set: The number of settings beyond the Stream management area. pts=%llu\n", video_stream_param[stream_loop].pts ));
#endif
			return H265_NG;
		}
	}
	
	next_write_pointer	= gH265_Dmux_Ctrl.write_pointer;
	free_pointer		= gH265_Dmux_Ctrl.free_pointer;
	
	for( stream_loop=0; stream_loop<store_num; stream_loop++){
		
		next_write_pointer++;
		
		if( next_write_pointer >= D_H265_DEC_SAVE_SAMPLE_MAX_NUM ){
			next_write_pointer = 0;
		}
		
		// set gH265_Dmux_Ctrl
		gH265_Dmux_Ctrl.decode_video[next_write_pointer].stream_end_flag	= video_stream_param[stream_loop].stream_end_flag;
		if( gH265_Dmux_Ctrl.decode_video[next_write_pointer].stream_end_flag == 1 ){
			gH265_Dmux_Ctrl.stream_end = TRUE;
		}
		gH265_Dmux_Ctrl.decode_video[next_write_pointer].stss			= video_stream_param[stream_loop].stss;
		gH265_Dmux_Ctrl.decode_video[next_write_pointer].stream_size	= video_stream_param[stream_loop].stream_size;
		gH265_Dmux_Ctrl.decode_video[next_write_pointer].stream_address	= video_stream_param[stream_loop].stream_address;
		gH265_Dmux_Ctrl.decode_video[next_write_pointer].pts			= video_stream_param[stream_loop].pts;
		gH265_Dmux_Ctrl.decode_video[next_write_pointer].dts			= video_stream_param[stream_loop].dts;
		
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "FeedRcv:%lu", (ULONG)gH265_Dmux_Ctrl.decode_video[next_write_pointer].pts ));
#else
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "FeedRcv:%llu", gH265_Dmux_Ctrl.decode_video[next_write_pointer].pts ));
#endif
	}
	
	// write_pointer update
	gH265_Dmux_Ctrl.write_pointer = next_write_pointer;
	OS_User_Set_Flg( FID_H265, FLG_H265_DEC_STREAM_RECEIVE );
	
	return H265_OK;
}


INT32 H265_Decode_Video_Display_Frame_Get( T_H265_IF_VIDEO_YUV_INFO* display_yuv_info )
{
	if( gH265_Disp_Ctrl.display_index == 0xFF ){
		H265_Err_Print(("--- H265_Decode_Video_Display_Frame_Get: display_index is abnormal.\n" ));
		return H265_NG;
	}
	
	if( gH265_Dec_Ctrl.mode != E_H265_PLAY_MODE_PAUSE ){
		H265_Err_Print(("--- H265_Decode_Video_Display_Frame_Get: Video state is not a Pause.\n" ));
		return H265_NG;
	}
	
	Sdram_Map_Movie_Play_H265_Get_Vdec( gH265_Disp_Ctrl.display_index, &display_yuv_info->y_addr, &display_yuv_info->cbcr_addr );
	
	display_yuv_info->g_width		= gH265_Dec_Ctrl.bank_y_width;
	display_yuv_info->g_lines		= gH265_Dec_Ctrl.bank_y_lines;
	display_yuv_info->width			= gH265_Dec_Param.image_width;
	display_yuv_info->lines			= gH265_Dec_Param.image_lines;
	display_yuv_info->yuv_format	= E_H265_IF_YUV_FORMAT_VIDEO;
	
	return H265_OK;
}



VOID H265_Decode_Stop_Request( BOOL last_picture_keep )
{
	gH265_Disp_Ctrl.last_picture_keep = last_picture_keep;
	OS_User_Set_Flg( FID_H265, FLG_H265_STOP );
	OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_DEC );	// Put the dummy sync in order to stop processing the DISP task.
	
	return;
}

VOID H265_Decode_Flush_Request( VOID )
{
	UCHAR		disp_bank = 0xFF;
	
	OS_User_Clr_Flg( FID_H265, ~FLG_H265_START );
	
	// Flush execute
	h265_decode_flush_execute( &disp_bank );
	
	if(disp_bank == 0xFF){
		H265_Err_Print(("+++ Not Found Disp_bank.\n"));
	}
	
	h265_decode_initialize_gdmux_control();
	
	// FLUSH operation release.
	OS_User_Clr_Flg( FID_H265, ~FLG_H265_FLUSH_OPS );
	
	H265_If_Play_Notify_STATE_CHANGE_COMPLETE( H265_OK );
	
	return;
}

BOOL H265_IsLastPTSSet_Or_Error(void)
{
	if ((gH265_Dec_Ctrl.last_display_pts != D_H265_DEC_NOT_SET_PTS) || gH265_Dec_Ctrl.error) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL H265_IsLastPTSDisplay_Or_Error(void)
{
	if (gH265_Disp_Ctrl.last_pts_display || gH265_Dec_Ctrl.error) {
		return TRUE;
	} else {
		return FALSE;
	}
}

ULLONG H265_GetLastPTS(void)
{
	return gH265_Dec_Ctrl.last_display_pts;
}

/**
 *	@brief		Flush execute.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */

static VOID h265_decode_flush_execute( UCHAR* disp_bank )
{
	INT32			ret;
	OS_USER_FLGPTN	flgptn;
	OS_USER_FLGPTN	waiptn = 0;
	UINT32			table_count;
	
	OS_User_Clr_Flg( FID_H265_TASK, ~FLG_H265_TASK_DISP_STATE_CHANGE );
	OS_User_Clr_Flg( FID_H265_TASK, ~FLG_H265_TASK_ENT_STATE_CHANGE );
	OS_User_Clr_Flg( FID_H265_TASK, ~FLG_H265_TASK_CC_STATE_CHANGE );
	
	// set FLUSH operation
	OS_User_Set_Flg( FID_H265, FLG_H265_FLUSH_OPS );
	
	// wait for DISP/ENT state change.
	waiptn = ( FLG_H265_TASK_DISP_STATE_CHANGE | FLG_H265_TASK_ENT_STATE_CHANGE );
	
	OS_User_Wai_Flg( FID_H265_TASK, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
	
	/* Reload Timer Stop */
	BF_Movie_Common_Timer_Stop( E_BF_MOVIE_COMMON_RTIMER_MODE_DEC );
	
	// To release the bank use_flg and release_flg is enabled.
	for( table_count=0; table_count<gH265_Disp_Ctrl.vdpb_use_num; table_count++ ){
		
		if( table_count != gH265_Disp_Ctrl.display_index ){
			if( (gH265_Disp_Ctrl.inf[table_count].use_flg == TRUE) && (gH265_Disp_Ctrl.inf[table_count].release_flg == TRUE) ){
				H265_Sirius_Dec_Frame_Free(table_count);
				gH265_Disp_Ctrl.inf[table_count].use_flg		= FALSE;
				gH265_Disp_Ctrl.use_count--;	// (YC area bank) use count        *max 9 VDPB
				gH265_Disp_Ctrl.buf_count--;	// (YC area bank) buffering count  *over 4 bank -> display start
#if 1	// for Debug
			H265_Info_Print(("h265_decode_flush_execute: release id = %u\n", table_count ));
#endif
			}
		}
	}
	
	// Keep id setting
	ret = H265_Sirius_Dec_Frame_Keepid( gH265_Disp_Ctrl.display_index );
	if( ret != H265_OK ){
		H265_Err_Print(("h265_decode_flush_execute : H265_Sirius_Dec_Frame_Keepid error.\n" ));
	}
	
#if 1	// for Debug
	H265_Info_Print(("h265_decode_flush_execute: keep id = %u\n", gH265_Disp_Ctrl.display_index ));
#endif
	
	// FLUSH request
	ret = H265_Sirius_Dec_Stop();
	if( ret != H265_OK ){
		H265_Err_Print(("h265_decode_flush_execute : H265_Sirius_Dec_Stop error.\n" ));
	}
	
	// wait for CC state change.
	OS_User_Wai_Flg( FID_H265_TASK, FLG_H265_TASK_CC_STATE_CHANGE, D_OS_USER_TWF_ORW, &flgptn );
	
	// Disp_Ctrl init
	for( table_count=0; table_count<gH265_Disp_Ctrl.vdpb_use_num; table_count++ ){
		if( gH265_Disp_Ctrl.inf[table_count].use_flg == TRUE ){
			if( table_count == gH265_Disp_Ctrl.display_index ){
				// display bank is not initialize.
				*disp_bank = table_count;
			}
			else{
				gH265_Disp_Ctrl.inf[table_count].use_flg = FALSE;
			}
		}
	}
	
// for Debug
#if 0
	// Check refer index
	if( (gH265_Disp_Ctrl.refer_index.lcd_decide != 0xFF) &&
		(gH265_Disp_Ctrl.refer_index.lcd_decide != gH265_Disp_Ctrl.display_index) ){
		H265_Err_Print(("lcd_decide index is abnormal!!\n"));
	}
	if( (gH265_Disp_Ctrl.refer_index.lcd_displaying != 0xFF) &&
		(gH265_Disp_Ctrl.refer_index.lcd_displaying != gH265_Disp_Ctrl.display_index) ){
		H265_Err_Print(("lcd_displaying index is abnormal!!\n"));
	}
	if( (gH265_Disp_Ctrl.refer_index.hdmi_decide != 0xFF) &&
		(gH265_Disp_Ctrl.refer_index.hdmi_decide != gH265_Disp_Ctrl.display_index) ){
		H265_Err_Print(("hdmi_decide index is abnormal!!\n"));
	}
	if( (gH265_Disp_Ctrl.refer_index.hdmi_displaying != 0xFF) &&
		(gH265_Disp_Ctrl.refer_index.hdmi_displaying != gH265_Disp_Ctrl.display_index) ){
		H265_Err_Print(("hdmi_displaying index is abnormal!!\n"));
	}
#endif
// for Debug
	
	gH265_Dec_Serial_Num				= 0;
	gH265_Disp_Ctrl.use_count			= 1;
	gH265_Disp_Ctrl.buf_count			= 1;
	gH265_Dec_Ctrl.last_display_pts		= D_H265_DEC_NOT_SET_PTS;
	gH265_Dec_Ctrl.last_stream_pts		= D_H265_DEC_NOT_SET_PTS;
	gH265_Dec_Ctrl.last_stream_dec_done	= FALSE;
	
	return;
}

/* -------------------------------------------------------------------------------------------- */
/*  Local Method																				*/
/* -------------------------------------------------------------------------------------------- */


/**
 *	@brief		Start Task Process.\n
 *				In case of Audio Task, this method initialize audio process.
 *	@param		i:task_id	Task Identifier
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_decode_task_start( UINT32 task_id )
{
	INT32	ercd;
	
	/* Create Task process */
	do{
		ercd = OS_User_Sta_Tsk( task_id, H265_MODE_PLAY );
		if( ercd == D_OS_USER_E_OBJ ){
			OS_User_Ter_Tsk(task_id);
		}
	} while(ercd != D_OS_USER_E_OK);
	
	return;
}


/**
 *	@brief		It initializes Deccode Control Data.\n
 *				1) Global Data\n
 *				2) SDRAM\n
 *	@param		None
 *	@return		H265_OK					(0) Normal End
 *	@note		None
 *	@attention	None
 */
static VOID h265_decode_initialize( T_H265_IF_DECODER_PARAM* decoder_param, T_H265_IF_DISPLAY_INFO* display_info )
{
	UINT32	total_bank_num;
	UINT32	frame_rate_multiply_100;
	
	/* Decode Condition (Default: Video/VGA 30fps High, Audio/48KHz,192bps)*/
	h265_decode_initialize_gdec_param( decoder_param );
	
	/* Decode Control Data */
	h265_decode_initialize_gdec_control();
	
	/* Initialize Demux Control Data */
	h265_decode_initialize_gdmux_control();
	
	/* Initialize Display Control Data */
	h265_decode_initialize_gdisp_control( display_info );
	
	// input image size setting
	gH265_Dec_Ctrl.bank_y_width	= decoder_param->image_bank_y_width;
	gH265_Dec_Ctrl.bank_y_lines	= decoder_param->image_bank_y_lines;
	gH265_Dec_Ctrl.bank_c_lines	= decoder_param->image_bank_c_lines;
	gH265_Dec_Ctrl.bank_y_size	= decoder_param->image_bank_y_size;
	gH265_Dec_Ctrl.bank_c_size	= decoder_param->image_bank_c_size;
	
	frame_rate_multiply_100 = (decoder_param->time_scale * 100) / decoder_param->time_delta;
	if( frame_rate_multiply_100 % 200 ){
		total_bank_num = (frame_rate_multiply_100 / 200) + D_H265_DEC_SPECIAL_TRICK_VDPB_COEFFICIENT + 1;
		gH265_Disp_Ctrl.vdpb_use_num	= total_bank_num;
	}
	else{
		total_bank_num = (frame_rate_multiply_100 / 200) + D_H265_DEC_SPECIAL_TRICK_VDPB_COEFFICIENT;
		gH265_Disp_Ctrl.vdpb_use_num	= total_bank_num;
	}
	
	if( total_bank_num < D_H265_DEC_VDPB_MIN_NUM ){
		total_bank_num					= D_H265_DEC_VDPB_MIN_NUM;
		gH265_Disp_Ctrl.vdpb_use_num	= D_H265_DEC_VDPB_MIN_NUM;
	}
	
	if( decoder_param->trick == FALSE ){
		total_bank_num					= D_H265_DEC_VDPB_MIN_NUM;
	}
	
	// Special handling for 240fps.
	if( total_bank_num == 124 ){
		total_bank_num					= D_H265_DEC_VDPB_MAX_NUM;
		gH265_Disp_Ctrl.vdpb_use_num	= D_H265_DEC_VDPB_MAX_NUM;
	}
	
	/* Initialize SDRAM area for Movie Play */
	Sdram_Map_Movie_Play_H265_Initialize( gH265_Dec_Ctrl.bank_y_size, gH265_Dec_Ctrl.bank_c_size, &total_bank_num );
	
	if( total_bank_num < gH265_Disp_Ctrl.vdpb_use_num ){
		if( decoder_param->trick == FALSE ){
			H265_Warning_Print(("--- h265_decode_initialize : Trick limit mode is enabled.\n"));
		}
		else{
			H265_Warning_Print(("--- h265_decode_initialize : Image bank area is small and did not acquire a buffer for performing reverse playback.\n" ));
		}
		H265_Warning_Print(("total_num:%d use_max_num:%d\n", total_bank_num, gH265_Disp_Ctrl.vdpb_use_num ));
		gH265_Dec_Ctrl.trick_limit		= TRUE;
		gH265_Disp_Ctrl.vdpb_use_num	= total_bank_num;
	}
	
#if 0	// for Debug
	H265_Info_Print(("h265_decode_initialize : total_num:%d use_max_num:%d\n", total_bank_num, gH265_Disp_Ctrl.vdpb_use_num ));
#endif // for Debug
	
	return;
}


/**
 *	@brief		Initialize Decode Condition of gDec_Param.\n
 *	@param		file_format_type	i: file_format type
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_decode_initialize_gdec_param( T_H265_IF_DECODER_PARAM* decoder_param )
{
	gH265_Dec_Param = *decoder_param;
	
	return;
}


/**
 *	@brief		Initialize Decode Control Data of gH265_Dec_Ctrl.\n
 *	@param		play_index				play index data
 *	@return		H265_OK					(0) Normal End
 *	@note		None
 *	@attention	None
 */
static VOID h265_decode_initialize_gdec_control( VOID )
{
	gH265_Dec_Serial_Num = 0;
	H265_Movie_Memset( (VOID*)&gH265_Dec_Ctrl, 0, sizeof(gH265_Dec_Ctrl) );
	gH265_Dec_Ctrl.mode						= E_H265_PLAY_MODE_PLAY;
	gH265_Dec_Ctrl.next_mode				= E_H265_PLAY_MODE_PLAY;
	gH265_Dec_Ctrl.direction				= E_H265_IF_PLAY_DIRECTION_FWD;
	gH265_Dec_Ctrl.speed					= E_H265_IF_PLAY_SPEED_1X;
	gH265_Dec_Ctrl.trick_limit				= FALSE;
	gH265_Dec_Ctrl.pause					= FALSE;
	gH265_Dec_Ctrl.iframe_only				= FALSE;
	gH265_Dec_Ctrl.task_result				= H265_OK;
	gH265_Dec_Ctrl.error					= FALSE;
	gH265_Dec_Ctrl.last_display_pts			= D_H265_DEC_NOT_SET_PTS;
	gH265_Dec_Ctrl.last_stream_pts			= D_H265_DEC_NOT_SET_PTS;
	gH265_Dec_Ctrl.last_stream_dec_done		= FALSE;
	
	return;
}


/**
 *	@brief		Initialize Stream Control Table of gH265_Dmux_Ctrl.\n
 *	@param		None
 *	@return		H265_OK					(0) Normal End
 *	@note		None
 *	@attention	None
 */
static VOID h265_decode_initialize_gdmux_control( VOID )
{
	H265_Movie_Memset( (VOID*)&gH265_Dmux_Ctrl, 0, sizeof(gH265_Dmux_Ctrl) );
	gH265_Dmux_Ctrl.stream_end = FALSE;
	
	return;
}


/**
 *	@brief		Initialize Display Control Data of gH265_Disp_Ctrl.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_decode_initialize_gdisp_control( T_H265_IF_DISPLAY_INFO* display_info )
{
	H265_Movie_Memset( (VOID*)&gH265_Disp_Ctrl, 0, sizeof(T_H265_DEC_DISP_CTL) );
	gH265_Disp_Ctrl.last_picture_keep			= TRUE;
	gH265_Disp_Ctrl.device_info					= *display_info;
	gH265_Disp_Ctrl.display_index				= 0xFF;
	
	gH265_Disp_Ctrl.refer_index.lcd_decide		= 0xFF;
	gH265_Disp_Ctrl.refer_index.lcd_displaying	= 0xFF;
	gH265_Disp_Ctrl.refer_index.hdmi_decide		= 0xFF;
	gH265_Disp_Ctrl.refer_index.hdmi_displaying	= 0xFF;
	
	return;
}



/**
 *	@brief		Set Decode Condition and Ribery parameters from File Format Informations.\n
 *	@param		None
 *	@return		H265_OK						(0)Normal End
 *	@return		H265_ERR_INPUT_PARAM			(1)Input Parameter Error
 *	@return		H265_ERR_FILE_NO_SUPPORT		(9)Not Support File Error
 *	@note		None
 *	@attention	None
 */
static INT32 h265_decode_init_param( VOID )
{
	INT32	ret						= H265_OK;
	ULONG	param_addr				= 0;
	ULONG	colpic_addr				= 0;
	ULONG	stream_buffer_addr		= 0;
	ULONG	stream_buffer_end_addr	= 0;
	
	UINT32	y_start_addr			= 0;
	UINT32	y_end_addr				= 0;
	UINT32	c_start_addr			= 0;
	UINT32	c_end_addr				= 0;
	
	ULONG	bufy_bank;
	ULONG	bufc_bank;
	ULONG	map_mode;
	
#if 0	// Automatic setting example
	UINT32	tmp_common_size;
#endif
	
	T_SR_BRIDGE_MAP_PARAM	bridge_map;
	
	// Initialize Event Flag
	OS_User_Clr_Flg( FID_H265, 0x0000 );
	OS_User_Clr_Flg( FID_H265_SYNC, 0x0000 );
	OS_User_Clr_Flg( FID_H265_TASK, 0x0000 );
	OS_User_Clr_Flg( FID_H265_DECODER, 0x0000 );
	
	memset( (void*)&bridge_map, 0, sizeof(bridge_map) );
	
	param_addr				= SDRAM_ADR_MOVIE_PLAY_H265_PARAM;
	colpic_addr				= SDRAM_ADR_MOVIE_PLAY_H265_COLPIC;
	stream_buffer_addr		= SDRAM_ADR_MOVIE_PLAY_H265_STREAM;
	stream_buffer_end_addr	= SDRAM_ADR_MOVIE_PLAY_H265_STREAM + SDRAM_SIZ_MOVIE_PLAY_H265_STREAM;
	
	bufy_bank = gH265_Dec_Ctrl.bank_y_size;
	bufc_bank = gH265_Dec_Ctrl.bank_c_size;
	
	if(      (bufy_bank == D_H265_DEC_MAP_MODE_NHD_Y_BANK) && (bufc_bank == D_H265_DEC_MAP_MODE_NHD_C_BANK) ){
		map_mode = D_H265_DEC_MAP_MODE_NHD_NUM;
	}
	else if( (bufy_bank == D_H265_DEC_MAP_MODE_1080P_Y_BANK) && (bufc_bank == D_H265_DEC_MAP_MODE_1080P_C_BANK) ){
		map_mode = D_H265_DEC_MAP_MODE_1080P_NUM;
	}
	else if( (bufy_bank == D_H265_DEC_MAP_MODE_HD_Y_BANK) && (bufc_bank == D_H265_DEC_MAP_MODE_HD_C_BANK) ){
		map_mode = D_H265_DEC_MAP_MODE_HD_NUM;
	}
	else if( (bufy_bank == D_H265_DEC_MAP_MODE_2_7K_16_9_Y_BANK) && (bufc_bank == D_H265_DEC_MAP_MODE_2_7K_16_9_C_BANK) ){
		map_mode = D_H265_DEC_MAP_MODE_2_7K_16_9_NUM;
	}
	else if( (bufy_bank == D_H265_DEC_MAP_MODE_2_7K_4_3_Y_BANK) && (bufc_bank == D_H265_DEC_MAP_MODE_2_7K_4_3_C_BANK) ){
		map_mode = D_H265_DEC_MAP_MODE_2_7K_4_3_NUM;
	}
	else if( (bufy_bank == D_H265_DEC_MAP_MODE_4K_Y_BANK) && (bufc_bank == D_H265_DEC_MAP_MODE_4K_C_BANK) ){
		map_mode = D_H265_DEC_MAP_MODE_4K_NUM;
	}
	else if( (bufy_bank == D_H265_DEC_MAP_MODE_4K_CINEMA_Y_BANK) && (bufc_bank == D_H265_DEC_MAP_MODE_4K_CINEMA_C_BANK) ){
		map_mode = D_H265_DEC_MAP_MODE_4K_CINEMA_NUM;
	}
	else if( (bufy_bank == D_H265_DEC_MAP_MODE_3K_Y_BANK) && (bufc_bank == D_H265_DEC_MAP_MODE_3K_C_BANK) ){
		map_mode = D_H265_DEC_MAP_MODE_3K_NUM;
	}
	else if( (bufy_bank == D_H265_DEC_MAP_MODE_4K3K_Y_BANK) && (bufc_bank == D_H265_DEC_MAP_MODE_4K3K_C_BANK) ){
		map_mode = D_H265_DEC_MAP_MODE_4K3K_NUM;
	}
	else if( (bufy_bank == D_H265_DEC_MAP_MODE_6K3K_Y_BANK) && (bufc_bank == D_H265_DEC_MAP_MODE_6K3K_C_BANK) ){
		map_mode = D_H265_DEC_MAP_MODE_6K3K_NUM;
	}
	else{
		H265_Err_Print(("h265_decode_init_param : map_mode setting error. bank size is abnormal.\n" ));
		return H265_NG;
	}
	
	Sdram_Map_Movie_Play_H265_Get_Vdec_Area( &y_start_addr, &y_end_addr, &c_start_addr, &c_end_addr );
	
	// Bridge Parameter Setting.
	// top address setting.
	bridge_map.ddr_config.ddr_ch0_top_addr			= SDRAM_ADR_CH0_TOP;
	bridge_map.ddr_config.ddr_ch1_top_addr			= SDRAM_ADR_CH1_TOP;
	
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#ifdef CO_MEMORY_1GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_1GB BEGIN ---
	// ** common_addr is setting the 1GB minutes from SDRAM last. **
	if( ((SDRAM_ADR_CH0_END - SDRAM_ADR_CH0_TOP) < D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB) || ((SDRAM_ADR_CH1_END - SDRAM_ADR_CH1_TOP) < D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB) ){
		H265_Err_Print(("h265_decode_init_param : There is a problem with a fixed set of common area.\n" ));
		return H265_NG;
	}
	
	// common address setting.
	bridge_map.ddr_config.ddr_ch0_top_common_addr	= SDRAM_ADR_CH0_END - D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB;
	bridge_map.ddr_config.ddr_ch1_top_common_addr	= SDRAM_ADR_CH1_END - D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB;
	
	// ** common_size is 1GB fixed. **
	bridge_map.ddr_config.ddr_ch0_common_size	= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB;
	bridge_map.ddr_config.ddr_ch1_common_size	= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB;
	
	// Address Check
	// SDRAM ch0
	if( (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START < bridge_map.ddr_config.ddr_ch0_top_common_addr)
	 || (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END   > (bridge_map.ddr_config.ddr_ch0_top_common_addr + bridge_map.ddr_config.ddr_ch0_common_size)) ){
		H265_Err_Print(("h265_decode_init_param : VIDEO_CODEC_CH0 address is incorrect.\n" ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START    : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END      : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_top_common_addr : %x\n", bridge_map.ddr_config.ddr_ch0_top_common_addr ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_common_size     : %x\n", bridge_map.ddr_config.ddr_ch0_common_size ));
		return H265_NG;
	}
	if( (SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_START < bridge_map.ddr_config.ddr_ch0_top_common_addr)
	 || (SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_END   > (bridge_map.ddr_config.ddr_ch0_top_common_addr + bridge_map.ddr_config.ddr_ch0_common_size)) ){
		H265_Err_Print(("h265_decode_init_param : SIRIUS_FIRM_CH0 address is incorrect.\n" ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_START    : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_END      : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_top_common_addr : %x\n", bridge_map.ddr_config.ddr_ch0_top_common_addr ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_common_size     : %x\n", bridge_map.ddr_config.ddr_ch0_common_size ));
		return H265_NG;
	}
	
	// SDRAM ch1
	if( (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_START < bridge_map.ddr_config.ddr_ch1_top_common_addr)
	 || (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END   > (bridge_map.ddr_config.ddr_ch1_top_common_addr + bridge_map.ddr_config.ddr_ch1_common_size)) ){
		H265_Err_Print(("h265_decode_init_param : VIDEO_CODEC_CH1 address is incorrect.\n" ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_START    : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_START ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END      : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch1_top_common_addr : %x\n", bridge_map.ddr_config.ddr_ch1_top_common_addr ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch1_common_size     : %x\n", bridge_map.ddr_config.ddr_ch1_common_size ));
		return H265_NG;
	}
// --- REMOVE_MEMORY_1GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_1GB
#ifdef CO_MEMORY_2GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_2GB BEGIN ---
	// ** common_addr is setting the 1GB minutes from SDRAM last. **
	if( ((SDRAM_ADR_CH0_END - SDRAM_ADR_CH0_TOP) < D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB) || ((SDRAM_ADR_CH1_END - SDRAM_ADR_CH1_TOP) < D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB) ){
		H265_Err_Print(("h265_decode_init_param : There is a problem with a fixed set of common area.\n" ));
		return H265_NG;
	}
	
	// common address setting.
	bridge_map.ddr_config.ddr_ch0_top_common_addr	= SDRAM_ADR_CH0_END - D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB;
	bridge_map.ddr_config.ddr_ch1_top_common_addr	= SDRAM_ADR_CH1_END - D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB;
	
	// ** common_size is 1GB fixed. **
	bridge_map.ddr_config.ddr_ch0_common_size	= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB;
	bridge_map.ddr_config.ddr_ch1_common_size	= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB;
	
	// Address Check
	// SDRAM ch0
	if( (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START < bridge_map.ddr_config.ddr_ch0_top_common_addr)
	 || (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END   > (bridge_map.ddr_config.ddr_ch0_top_common_addr + bridge_map.ddr_config.ddr_ch0_common_size)) ){
		H265_Err_Print(("h265_decode_init_param : VIDEO_CODEC_CH0 address is incorrect.\n" ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START    : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END      : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_top_common_addr : %x\n", bridge_map.ddr_config.ddr_ch0_top_common_addr ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_common_size     : %x\n", bridge_map.ddr_config.ddr_ch0_common_size ));
		return H265_NG;
	}
	if( (SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_START < bridge_map.ddr_config.ddr_ch0_top_common_addr)
	 || (SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_END   > (bridge_map.ddr_config.ddr_ch0_top_common_addr + bridge_map.ddr_config.ddr_ch0_common_size)) ){
		H265_Err_Print(("h265_decode_init_param : SIRIUS_FIRM_CH0 address is incorrect.\n" ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_START    : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_END      : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_top_common_addr : %x\n", bridge_map.ddr_config.ddr_ch0_top_common_addr ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_common_size     : %x\n", bridge_map.ddr_config.ddr_ch0_common_size ));
		return H265_NG;
	}
	
	// SDRAM ch1
	if( (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_START < bridge_map.ddr_config.ddr_ch1_top_common_addr)
	 || (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END   > (bridge_map.ddr_config.ddr_ch1_top_common_addr + bridge_map.ddr_config.ddr_ch1_common_size)) ){
		H265_Err_Print(("h265_decode_init_param : VIDEO_CODEC_CH1 address is incorrect.\n" ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_START    : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_START ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END      : %lx\n", SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch1_top_common_addr : %x\n", bridge_map.ddr_config.ddr_ch1_top_common_addr ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch1_common_size     : %x\n", bridge_map.ddr_config.ddr_ch1_common_size ));
		return H265_NG;
	}
// --- REMOVE_MEMORY_2GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_2GB
#ifdef CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_3GB BEGIN ---
	// ** common_addr is setting the 1GB minutes from SDRAM last. **
	if( ((SDRAM_ADR_CH0_END - SDRAM_ADR_CH0_TOP) < D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB) || ((SDRAM_ADR_CH1_END - SDRAM_ADR_CH1_TOP) < D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB) ){
		H265_Err_Print(("h265_decode_init_param : There is a problem with a fixed set of common area.\n" ));
		return H265_NG;
	}
	
	// common address setting.
	bridge_map.ddr_config.ddr_ch0_top_common_addr	= SDRAM_ADR_CH0_END - D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB;
	bridge_map.ddr_config.ddr_ch1_top_common_addr	= SDRAM_ADR_CH1_END - D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB;
	
	// ** common_size is 1GB fixed. **
	bridge_map.ddr_config.ddr_ch0_common_size	= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB;
	bridge_map.ddr_config.ddr_ch1_common_size	= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB;
	
	// Address Check
	// SDRAM ch0
	if( (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START < bridge_map.ddr_config.ddr_ch0_top_common_addr)
	 || (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END   > (bridge_map.ddr_config.ddr_ch0_top_common_addr + bridge_map.ddr_config.ddr_ch0_common_size)) ){
		H265_Err_Print(("h265_decode_init_param : VIDEO_CODEC_CH0 address is incorrect.\n" ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START    : %x\n", (UINT32)SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END      : %x\n", (UINT32)SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_top_common_addr : %x\n", bridge_map.ddr_config.ddr_ch0_top_common_addr ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_common_size     : %x\n", bridge_map.ddr_config.ddr_ch0_common_size ));
		return H265_NG;
	}
	if( (SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_START < bridge_map.ddr_config.ddr_ch0_top_common_addr)
	 || (SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_END   > (bridge_map.ddr_config.ddr_ch0_top_common_addr + bridge_map.ddr_config.ddr_ch0_common_size)) ){
		H265_Err_Print(("h265_decode_init_param : SIRIUS_FIRM_CH0 address is incorrect.\n" ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_START    : %x\n", (UINT32)SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_SIRIUS_FIRM_CH0_END      : %x\n", (UINT32)SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_top_common_addr : %x\n", bridge_map.ddr_config.ddr_ch0_top_common_addr ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch0_common_size     : %x\n", bridge_map.ddr_config.ddr_ch0_common_size ));
		return H265_NG;
	}
	
	// SDRAM ch1
	if( (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_START < bridge_map.ddr_config.ddr_ch1_top_common_addr)
	 || (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END   > (bridge_map.ddr_config.ddr_ch1_top_common_addr + bridge_map.ddr_config.ddr_ch1_common_size)) ){
		H265_Err_Print(("h265_decode_init_param : VIDEO_CODEC_CH1 address is incorrect.\n" ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_START    : %x\n", (UINT32)SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_START ));
		H265_Err_Print(("SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END      : %x\n", (UINT32)SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch1_top_common_addr : %x\n", bridge_map.ddr_config.ddr_ch1_top_common_addr ));
		H265_Err_Print(("bridge_map.ddr_config.ddr_ch1_common_size     : %x\n", bridge_map.ddr_config.ddr_ch1_common_size ));
		return H265_NG;
	}
// --- REMOVE_MEMORY_3GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---

#if 0	// [sample]In the case of automatic setting.
	// common address setting.
	bridge_map.ddr_config.ddr_ch0_top_common_addr	= SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_START & 0xF0000000;
	bridge_map.ddr_config.ddr_ch1_top_common_addr	= SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_START & 0xF0000000;
	
	// common size setting.
	// ch0
	if( SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END & 0x0FFFFFFF ){
		tmp_common_size	= ((SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END & 0xF0000000) + 0x10000000) - bridge_map.ddr_config.ddr_ch0_top_common_addr;
	}
	else{
		tmp_common_size	= (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH0_END & 0xF0000000) - bridge_map.ddr_config.ddr_ch0_top_common_addr;
	}
	
	if( tmp_common_size <= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_256MB ){
		tmp_common_size = D_H265_SIRIUS_BRIDGE_COMMON_SIZE_256MB;
	}
	else if( (D_H265_SIRIUS_BRIDGE_COMMON_SIZE_256MB < tmp_common_size) && (tmp_common_size <= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB) ){
		tmp_common_size = D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB;
	}
	else if( (D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB < tmp_common_size) && (tmp_common_size <= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB) ){
		tmp_common_size = D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB;
	}
	else if( (D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB < tmp_common_size) && (tmp_common_size <= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1_5GB) ){
		tmp_common_size = D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1_5GB;
	}
	else if( (D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1_5GB < tmp_common_size) && (tmp_common_size <= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_2GB) ){
		tmp_common_size = D_H265_SIRIUS_BRIDGE_COMMON_SIZE_2GB;
	}
	else{
		H265_Err_Print(("h265_decode_init_param : Error! Common size is over 2GB(ch0).\n" ));
	}
	bridge_map.ddr_config.ddr_ch0_common_size	= tmp_common_size;
	
	// ch1
	if( SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END & 0x0FFFFFFF ){
		tmp_common_size = ((SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END & 0xF0000000) + 0x10000000) - bridge_map.ddr_config.ddr_ch1_top_common_addr;
	}
	else{
		tmp_common_size = (SDRAM_ADR_MOVIE_PLAY_VIDEO_CODEC_CH1_END & 0xF0000000) - bridge_map.ddr_config.ddr_ch1_top_common_addr;
	}
	if( tmp_common_size <= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_256MB ){
		tmp_common_size = D_H265_SIRIUS_BRIDGE_COMMON_SIZE_256MB;
	}
	else if( (D_H265_SIRIUS_BRIDGE_COMMON_SIZE_256MB < tmp_common_size) && (tmp_common_size <= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB) ){
		tmp_common_size = D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB;
	}
	else if( (D_H265_SIRIUS_BRIDGE_COMMON_SIZE_512MB < tmp_common_size) && (tmp_common_size <= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB) ){
		tmp_common_size = D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB;
	}
	else if( (D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1GB < tmp_common_size) && (tmp_common_size <= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1_5GB) ){
		tmp_common_size = D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1_5GB;
	}
	else if( (D_H265_SIRIUS_BRIDGE_COMMON_SIZE_1_5GB < tmp_common_size) && (tmp_common_size <= D_H265_SIRIUS_BRIDGE_COMMON_SIZE_2GB) ){
		tmp_common_size = D_H265_SIRIUS_BRIDGE_COMMON_SIZE_2GB;
	}
	else{
		H265_Err_Print(("h265_decode_init_param : Error! Common size is over 2GB(ch1).\n" ));
	}
	bridge_map.ddr_config.ddr_ch1_common_size	= tmp_common_size;
	
	if( (bridge_map.ddr_config.ddr_ch0_common_size + bridge_map.ddr_config.ddr_ch1_common_size) > D_H265_SIRIUS_BRIDGE_COMMON_SIZE_2GB ){
		H265_Err_Print(("h265_decode_init_param : Error! Common size is over 2GB(ch0+ch1).\n" ));
		return H265_NG;
	}
#endif
	
	// decode
	bridge_map.map_dec[0].param.start_addr			= param_addr;
	bridge_map.map_dec[0].param.end_addr			= param_addr  + D_H265_SIRIUS_BRIDGE_PARAM_SIZE;
	bridge_map.map_dec[0].colpic.start_addr			= colpic_addr;
	bridge_map.map_dec[0].colpic.end_addr			= colpic_addr + D_H265_SIRIUS_BRIDGE_COLPIC_SIZE;
	bridge_map.map_dec[0].vbb.start_addr			= stream_buffer_addr;
	bridge_map.map_dec[0].vbb.end_addr				= stream_buffer_end_addr;
	bridge_map.map_dec[0].fm_y.start_addr			= y_start_addr;
	bridge_map.map_dec[0].fm_y.end_addr				= y_end_addr;
	bridge_map.map_dec[0].fm_c.start_addr			= c_start_addr;
	bridge_map.map_dec[0].fm_c.end_addr				= c_end_addr;
	bridge_map.map_dec[0].decode_mode				= map_mode;
	
#if 1	// for Debug
	H265_Info_Print(("h265_decode_init_param : ddr_ch0_top_addr         = %#x\n", bridge_map.ddr_config.ddr_ch0_top_addr ));
	H265_Info_Print(("h265_decode_init_param : ddr_ch0_top_common_addr  = %#x\n", bridge_map.ddr_config.ddr_ch0_top_common_addr ));
	H265_Info_Print(("h265_decode_init_param : ddr_ch0_common_size      = %#x\n", bridge_map.ddr_config.ddr_ch0_common_size ));
	H265_Info_Print(("h265_decode_init_param : ddr_ch1_top_addr         = %#x\n", bridge_map.ddr_config.ddr_ch1_top_addr ));
	H265_Info_Print(("h265_decode_init_param : ddr_ch1_top_common_addr  = %#x\n", bridge_map.ddr_config.ddr_ch1_top_common_addr ));
	H265_Info_Print(("h265_decode_init_param : ddr_ch1_common_size      = %#x\n", bridge_map.ddr_config.ddr_ch1_common_size ));
	H265_Info_Print(("h265_decode_init_param : param start_addr         = %#x\n", bridge_map.map_dec[0].param.start_addr ));
	H265_Info_Print(("h265_decode_init_param : param end_addr           = %#x\n", bridge_map.map_dec[0].param.end_addr ));
	H265_Info_Print(("h265_decode_init_param : colpic start_addr        = %#x\n", bridge_map.map_dec[0].colpic.start_addr ));
	H265_Info_Print(("h265_decode_init_param : colpic end_addr          = %#x\n", bridge_map.map_dec[0].colpic.end_addr ));
	H265_Info_Print(("h265_decode_init_param : vbb start_addr           = %#x\n", bridge_map.map_dec[0].vbb.start_addr ));
	H265_Info_Print(("h265_decode_init_param : vbb end_addr             = %#x\n", bridge_map.map_dec[0].vbb.end_addr ));
	H265_Info_Print(("h265_decode_init_param : fm_y start_addr          = %#x\n", bridge_map.map_dec[0].fm_y.start_addr ));
	H265_Info_Print(("h265_decode_init_param : fm_y end_addr            = %#x\n", bridge_map.map_dec[0].fm_y.end_addr ));
	H265_Info_Print(("h265_decode_init_param : fm_c start_addr          = %#x\n", bridge_map.map_dec[0].fm_c.start_addr ));
	H265_Info_Print(("h265_decode_init_param : fm_c end_addr            = %#x\n", bridge_map.map_dec[0].fm_c.end_addr ));
	H265_Info_Print(("h265_decode_init_param : decode_mode              = %#x\n", bridge_map.map_dec[0].decode_mode ));
#endif	// for Debug

	// system boot/video boot
	ret = H265_Sirius_Initialize( H265_MODE_PLAY, (T_SR_BRIDGE_MAP_PARAM *)&bridge_map );
	if( ret != H265_OK ){
		H265_Err_Print(("h265_decode_init_param : H265_Sirius_Initialize NG\n"));
		return H265_NG;
	}
	
	return H265_OK;
}

/**
 *	@brief		FLUSH Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H265_PLAY_MODE h265_decode_flush( VOID )
{
	return E_H265_PLAY_MODE_FLUSH;
}

/**
 *	@brief		PLAY Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H265_PLAY_MODE h265_decode_play( VOID )
{
	return E_H265_PLAY_MODE_PLAY;
}

/**
 *	@brief		PAUSE Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H265_PLAY_MODE h265_decode_pause( VOID )
{
	return E_H265_PLAY_MODE_PAUSE;
}

/**
 *	@brief		RESUME Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H265_PLAY_MODE h265_decode_resume( VOID )
{
	return E_H265_PLAY_MODE_RESUME;
}

/**
 *	@brief		STOP Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H265_PLAY_MODE h265_decode_stop( VOID )
{
	return E_H265_PLAY_MODE_STOP;
}

/**
 *	@brief		END Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H265_PLAY_MODE h265_decode_end( VOID )
{
	return E_H265_PLAY_MODE_END;
}


/**
 *	@brief		Forced Stop relational tasks.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_decode_task_forced_stop( VOID )
{
	OS_User_Ter_Tsk( TID_H265_ENT );
	OS_User_Ter_Tsk( TID_H265_CC );
	OS_User_Ter_Tsk( TID_H265_DISP );
	
	return;
}

/**
 *	@brief		Set VCPB Information Table & Execute Ribery Decoder
 *	@param		video_index		video chunk index
 *	@param		video_index		sample index
 *	@return		H265_OK		Normal End
 *	@return		H265_NG		It cannot get vacant VDPB
 *	@note		None
 *	@attention	None
 */
static INT32 h265_decode_set_vcpb( UINT32 video_index, UCHAR vdpb_use_num)
{
	INT32					ret  = 0;
	UINT32					alloc_result;
	T_SR_DEC_PICTURE_INFO	picture_info;
	
	H265_Movie_Memset( &picture_info, 0, sizeof(picture_info) );
	
	// Input Parameter Check
	if( video_index >= D_H265_DEC_SAVE_SAMPLE_MAX_NUM ){
		H265_Info_Print(("h265_decode_set_vcpb : Parameter Error. video_index=%d\n",video_index ));
		return H265_NG;
	}
	
	// It checks Number of Using VCPB/VDPB bank
	if( gH265_Disp_Ctrl.use_count >= vdpb_use_num ){
		/* Full of VDPB Bank. So, it cannot register VCPB Information Table. */
		return H265_NG;
	}
	
	picture_info.pic_addr	= gH265_Dmux_Ctrl.decode_video[video_index].stream_address;
	picture_info.pic_size	= gH265_Dmux_Ctrl.decode_video[video_index].stream_size;
	picture_info.pts		= gH265_Dmux_Ctrl.decode_video[video_index].pts;
	
	H265_Movie_Cache_Clean( picture_info.pic_addr, picture_info.pic_size );
	
#if 0	// for Debug
//	H265_Info_Print(("h265_decode_set_vcpb : Decode request!!\n"));
//	H265_Info_Print(("use_count=%d  use_num=%d\n", gH265_Disp_Ctrl.use_count, vdpb_use_num ));
	H265_Info_Print(("addr=%#x  sizenum=%#x  pts=%d\n", picture_info.pic_addr, picture_info.pic_size, picture_info.pts ));
#endif	// for Debug
	
	// Decode Request.
	ret = H265_Sirius_Dec_Frame( &picture_info, &alloc_result );
	if( ret == H265_NG ){
		H265_Err_Print(( "h265_decode_set_vcpb : H265_Sirius_Dec_Frame NG\n" ));
		return H265_NG;
	}
	
	if( alloc_result != SR_FM_ALLOC_OK ){
		H265_Warning_Print(("h265_decode_set_vcpb : alloc_result NG. This route should not pass through.\n" ));
		return H265_NG;
	}
	
	gH265_Disp_Ctrl.use_count++;
	gH265_Dec_Serial_Num++;
	
	return H265_OK;
}

/**
 *	@brief		Get Next sample and set Dt Table.\n
 *	@param		i:video_index		video sample index
 *	@return		H265_OK				(0)Normal End
 *	@return		H265_END			(1)Normal End( Data Read Completed )
 *	@note		None
 *	@attention	None
 */
static INT32 h265_decode_next_video_sample_get( UINT32* video_index )
{
	UINT32	read_pointer;
	UINT32	next_read_pointer;
	UINT32	write_pointer;
	
	write_pointer		= gH265_Dmux_Ctrl.write_pointer;
	read_pointer		= gH265_Dmux_Ctrl.read_pointer;
	next_read_pointer	= read_pointer + 1;
	if( next_read_pointer >= D_H265_DEC_SAVE_SAMPLE_MAX_NUM ){
		next_read_pointer = 0;
	}
	
	if( write_pointer ==  read_pointer ){
		// Video Stream is nothing.
		return H265_NG;
	}
	
	*video_index					= next_read_pointer;
	gH265_Dmux_Ctrl.read_pointer	= next_read_pointer;
	
	return H265_OK;
}

/**
 *	@brief		Get Next sample and set Dt Table.\n
 *	@param		i:video_index		video sample index
 *	@return		H265_OK				(0)Normal End
 *	@return		H265_END			(1)Normal End( Data Read Completed )
 *	@note		None
 *	@attention	None
 */
static INT32 h265_decode_next_video_sample_request( VOID )
{
	BOOL			host_ret;
	UINT32			next_write_pointer;
	UINT32			free_pointer;
	
	if( gH265_Dmux_Ctrl.stream_end == TRUE ){
		// Already Stream End.
		return H265_OK;
	}
	
	next_write_pointer	= gH265_Dmux_Ctrl.write_pointer + 1;
	free_pointer		= gH265_Dmux_Ctrl.free_pointer;
	
	if( next_write_pointer >= D_H265_DEC_SAVE_SAMPLE_MAX_NUM ){
		next_write_pointer = 0;
	}
	if( next_write_pointer == free_pointer ){
		// Don't request because management table is full.
		return H265_OK;
	}
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "FeedReq" ));
	
	host_ret = Host_H265_Notify_Video_Stream_Request();
	if( host_ret == FALSE ){
		H265_Err_Print(("--- h265_decode_next_video_sample_request:Host_H265_Notify_Video_Stream_Request Error\n"));
		return H265_NG;
	}
	
	return H265_OK;
}


/**
 *	@brief		Get VDPB_Information
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 h265_decode_get_vdpb( T_SR_FM_OUT_INFO* fminfo )
{
	INT32	ret;
	UINT32	alloc_result;
	
	if( fminfo == NULL ){
		return H265_ERR_INPUT_PARAM;
	}
	
	// Get Decode Information.
	ret = H265_Sirius_Dec_Frame_Alloc( fminfo, &alloc_result);
	if( ret == H265_OK ){
		
		// It checks alloc_result
		if( alloc_result == SR_FM_ALLOC_OK ){
			
			if( fminfo->id >= gH265_Disp_Ctrl.vdpb_use_num ){
				H265_Info_Print(("--- h265_decode_get_vdpb: Frame ID is max over!! id=%d\n", fminfo->id));
				ret = H265_NG;
			}
		}
		else{
			// Alloc of the image has been completed.
			ret = H265_OK_COMPLETE;
		}
	}
	
	return ret;
}


/**
 *	@brief		Free not use VDPB bank.\n
 *	@param		i:direction			Play Direction
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_decode_vdpb_free( E_H265_IF_PLAY_DIRECTION direction, T_H265_DEC_DISP_REFER_INDEX protect_index )
{
	UINT32		table_count;
	
	for( table_count=0; table_count<gH265_Disp_Ctrl.vdpb_use_num; table_count++ ){
		// display use bank is not target
		if( (protect_index.lcd_decide  == table_count) || (protect_index.lcd_displaying  == table_count) ||
			(protect_index.hdmi_decide == table_count) || (protect_index.hdmi_displaying == table_count) ||
			(gH265_Disp_Ctrl.display_index == table_count )){
			continue;
		}
		if( gH265_Disp_Ctrl.inf[table_count].use_flg == TRUE ){
			
			if( gH265_Disp_Ctrl.inf[table_count].release_flg == FALSE ){
				
				if( direction == E_H265_IF_PLAY_DIRECTION_FWD ){
					
					if( gH265_Disp_Ctrl.display_pts <= gH265_Disp_Ctrl.inf[table_count].PTS ){
						// In larger PTS than "display PTS", nothing is done.
						continue;
					}
				}
				else{
					if( gH265_Disp_Ctrl.display_pts >= gH265_Disp_Ctrl.inf[table_count].PTS ){
						// In PTS smaller than "display PTS", nothing is done.
						continue;
					}
				}
			}
			/* Detect!! */
#if 0	// for Debug
			H265_Info_Print(("h265_decode_vdpb_free : Free request!! %d\n", gH265_Disp_Ctrl.inf[table_count].PTS));
#endif	// for Debug
			H265_Sirius_Dec_Frame_Free(table_count);
			gH265_Disp_Ctrl.inf[table_count].use_flg		= FALSE;
			gH265_Disp_Ctrl.use_count--;	// (YC area bank) use count        *max 9 VDPB
			gH265_Disp_Ctrl.buf_count--;	// (YC area bank) buffering count  *over 4 bank -> display start
		}
	}
	
	return;
}


/**
 *	@brief		Search display index.\n
 *	@param		i:display_index		Display Index
 *	@return		H265_OK				Normal End
 *	@note		None
 *	@attention	None
 */
static INT32 h265_decode_search_display_index( E_H265_DEC_PTS_SEARCH_MODE search_mode, UCHAR* display_index, T_H265_DEC_DISP_REFER_INDEX protect_index )
{
	UINT32		table_count;
	
	// Initialize display_index
	*display_index = 0xFF;
	
	for( table_count=0; table_count<gH265_Disp_Ctrl.vdpb_use_num; table_count++ ){
		
		// display use bank is not target
		if( (protect_index.lcd_decide  == table_count) || (protect_index.lcd_displaying  == table_count) ||
			(protect_index.hdmi_decide == table_count) || (protect_index.hdmi_displaying == table_count) ){
			continue;
		}
		
		if( gH265_Disp_Ctrl.inf[table_count].use_flg == TRUE ){
			
			if( gH265_Disp_Ctrl.inf[table_count].release_flg == TRUE ){
				continue;
			}
			// The same "PTS" is searched.
			if( search_mode == E_H265_DEC_PTS_SEARCH_MODE_SAME ){
				if( gH265_Disp_Ctrl.display_target_pts == gH265_Disp_Ctrl.inf[table_count].PTS ){
					*display_index = table_count;
					break;
				}
			}
			// Smallest "PTS" in bank is searched. (Case of Fast Forward)
			else if( search_mode == E_H265_DEC_PTS_SEARCH_MODE_SMALL ){
				if( gH265_Disp_Ctrl.display_index == table_count ){
					continue;
				}
				if( *display_index == 0xFF ){
					// found valid index. "display_index" renew.
					*display_index = table_count;
					continue;
				}
				if( gH265_Disp_Ctrl.inf[*display_index].PTS > gH265_Disp_Ctrl.inf[table_count].PTS ){
					*display_index = table_count;
				}
			}
			// Largest "PTS" in bank is searched. (Case of Fast Rewind)
			else{
				if( gH265_Disp_Ctrl.display_index == table_count ){
					continue;
				}
				if( *display_index == 0xFF ){
					// found valid index. "display_index" renew.
					*display_index = table_count;
					continue;
				}
				if( gH265_Disp_Ctrl.inf[*display_index].PTS < gH265_Disp_Ctrl.inf[table_count].PTS ){
					*display_index = table_count;
				}
			}
		}
	}
	
	if( *display_index == 0xFF ){
		return H265_NG;
	}
	
	return H265_OK;
}


/**
 *	@brief		Make Display Image.\n
 *	@param		i:decode_addr		Decode Address
 *	@param		i:disp_addr			Display Address
 *	@param		i:fill_counter		Fill Counter
 *	@note		None
 *	@attention	None
 */
static VOID h265_decode_display_image( T_H265_IF_DISPLAY_PARAM* display_info, T_H265_MOVIE_ADDR* dec_addr, T_H265_MOVIE_ADDR* disp_addr, BOOL codec_format )
{
	USHORT							src_img_width = 0;
	USHORT							src_img_lines = 0;
	USHORT							src_g_img_width = 0;
	USHORT							src_g_img_lines = 0;
	USHORT							dst_img_width = 0;
	USHORT							dst_img_lines = 0;
	USHORT							dst_g_img_width = 0;
	USHORT							dst_g_img_lines = 0;
	ULONG							src_y_addr;
	ULONG							src_c_addr;
	ULONG							dst_y_addr;
	ULONG							dst_c_addr;
	BOOL							ribery_mode = TRUE;
	E_H265_IF_DISP_ROTATE			rotate;
	E_H265_RESIZE_MODE				resize_mode;
	
	T_H265_MOVIE_REC_TBL			rec_ctrl;
	
#if 0	// for Debug
	OS_USER_SYSTIM					time_before, time_after;
#endif	// for Debug
	
	H265_Movie_Memset( &rec_ctrl, 0, sizeof( rec_ctrl ) );
	
	if( codec_format == TRUE ){
		disp_addr->y_address	= dec_addr->y_address;
		disp_addr->c_address	= dec_addr->c_address;
	}
	else{
		if( (display_info->rotate == E_H265_IF_DISP_ROTATE_90) ||
			(display_info->rotate == E_H265_IF_DISP_ROTATE_270) ){
			
			if( (display_info->image_lines <= SDRAM_WIDTH_MOVIE_PLAY_ROTATE_WORK ) &&	// Width and Lines are reversed during 90/270 rotation.
				(display_info->image_width <= SDRAM_LINES_MOVIE_PLAY_ROTATE_WORK ) ){	// Width and Lines are reversed during 90/270 rotation.
				
				rec_ctrl.src_width		= gH265_Dec_Param.image_width;
				rec_ctrl.src_lines		= gH265_Dec_Param.image_lines;
				rec_ctrl.g_src_width	= gH265_Dec_Ctrl.bank_y_width;
				rec_ctrl.g_src_lines	= gH265_Dec_Ctrl.bank_y_lines;
				rec_ctrl.src_addr[0]	= dec_addr->y_address;
				rec_ctrl.src_addr[1]	= dec_addr->c_address;
				
				rec_ctrl.dst_width		= display_info->image_lines;	// Width and Lines are reversed during 90/270 rotation.
				rec_ctrl.dst_lines		= display_info->image_width;	// Width and Lines are reversed during 90/270 rotation.
				rec_ctrl.g_dst_width	= SDRAM_WIDTH_MOVIE_PLAY_ROTATE_WORK;
				rec_ctrl.g_dst_lines	= SDRAM_LINES_MOVIE_PLAY_ROTATE_WORK;
				rec_ctrl.dst_addr[0]	= SDRAM_ADR_MOVIE_PLAY_ROTATE_WORK;
				rec_ctrl.dst_addr[1]	= SDRAM_ADR_MOVIE_PLAY_ROTATE_WORK + (SDRAM_WIDTH_MOVIE_PLAY_ROTATE_WORK * SDRAM_LINES_MOVIE_PLAY_ROTATE_WORK);
				
				rec_ctrl.ribery_mode	= TRUE;
				rec_ctrl.rotate			= E_H265_IF_DISP_ROTATE_0;
				rec_ctrl.resize_mode	= E_H265_RESIZE_MODE_NEAREST;
				
#if 0	// for Debug
		OS_User_Get_Tim(&time_before);
#endif	// for Debug
				// resize
				H265_Movie_Rec_Resize( &rec_ctrl );
#if 0	// for Debug
		OS_User_Get_Tim(&time_after);
		H265_Info_Print(("resize_time: %d\n", (UINT32)(time_after - time_before) ));
#endif	// for Debug
				
				ribery_mode = FALSE;
				src_img_width		= display_info->image_lines;	// Width and Lines are reversed during 90/270 rotation.
				src_img_lines		= display_info->image_width;	// Width and Lines are reversed during 90/270 rotation.
				src_g_img_width		= SDRAM_WIDTH_MOVIE_PLAY_ROTATE_WORK;
				src_g_img_lines		= SDRAM_LINES_MOVIE_PLAY_ROTATE_WORK;
				src_y_addr			= SDRAM_ADR_MOVIE_PLAY_ROTATE_WORK;
				src_c_addr			= SDRAM_ADR_MOVIE_PLAY_ROTATE_WORK + (SDRAM_WIDTH_MOVIE_PLAY_ROTATE_WORK * SDRAM_LINES_MOVIE_PLAY_ROTATE_WORK);
			}
			else{
				ribery_mode = TRUE;
				src_img_width		= gH265_Dec_Param.image_width;
				src_img_lines		= gH265_Dec_Param.image_lines;
				src_g_img_width		= gH265_Dec_Ctrl.bank_y_width;
				src_g_img_lines		= gH265_Dec_Ctrl.bank_y_lines;
				src_y_addr			= dec_addr->y_address;
				src_c_addr			= dec_addr->c_address;
			}
			dst_img_width		= display_info->image_width;
			dst_img_lines		= display_info->image_lines;
			dst_g_img_width		= display_info->global_width;
			dst_g_img_lines		= display_info->global_lines;
			dst_y_addr			= disp_addr->y_address;
			dst_c_addr			= disp_addr->c_address;
			
			rotate				= display_info->rotate;
			resize_mode			= E_H265_RESIZE_MODE_NEAREST;
		}
		else{
			ribery_mode = TRUE;
			src_img_width		= gH265_Dec_Param.image_width;
			src_img_lines		= gH265_Dec_Param.image_lines;
			src_g_img_width		= gH265_Dec_Ctrl.bank_y_width;
			src_g_img_lines		= gH265_Dec_Ctrl.bank_y_lines;
			src_y_addr			= dec_addr->y_address;
			src_c_addr			= dec_addr->c_address;
			
			dst_img_width		= display_info->image_width;
			dst_img_lines		= display_info->image_lines;
			dst_g_img_width		= display_info->global_width;
			dst_g_img_lines		= display_info->global_lines;
			dst_y_addr			= disp_addr->y_address;
			dst_c_addr			= disp_addr->c_address;
			
			rotate				= display_info->rotate;
			resize_mode			= E_H265_RESIZE_MODE_BILINEAR;
		}
		
		rec_ctrl.src_width		= src_img_width;
		rec_ctrl.src_lines		= src_img_lines;
		rec_ctrl.g_src_width	= src_g_img_width;
		rec_ctrl.g_src_lines	= src_g_img_lines;
		rec_ctrl.src_addr[0]	= src_y_addr;
		rec_ctrl.src_addr[1]	= src_c_addr;
		
		rec_ctrl.dst_width		= dst_img_width;
		rec_ctrl.dst_lines		= dst_img_lines;
		rec_ctrl.g_dst_width	= dst_g_img_width;
		rec_ctrl.g_dst_lines	= dst_g_img_lines;
		rec_ctrl.dst_addr[0]	= dst_y_addr;
		rec_ctrl.dst_addr[1]	= dst_c_addr;
		
		rec_ctrl.rotate			= rotate;
		rec_ctrl.resize_mode	= resize_mode;
		rec_ctrl.ribery_mode	= ribery_mode;
		
#if 0	// for Debug
		OS_User_Get_Tim(&time_before);
#endif	// for Debug
		if (User_IsDualStream() && User_IsH265MainStream()) {
			H265_Movie_SGDE_Resize(&rec_ctrl);
		} else {
			//resize & rotate
			H265_Movie_Rec_Resize( &rec_ctrl );
		}
#if 0	// for Debug
		OS_User_Get_Tim(&time_after);
		H265_Info_Print(("resize_rotate_time: %d\n", (UINT32)(time_after - time_before) ));
#endif	// for Debug
	}
	
	return;
}


/**
 *	@brief		Get video stream of top image.\n
 *	@param		i:file_id				File Identifier
 *	@param		i:video_stream_address	video stream address
 *	@param		i/o:video_stream_size	video stream size
 *	@return		H265_OK					(0)Normal End
 *	@return		H265_ERR_INPUT_PARAM		(1)Input Parameter Error
 *	@return		H265_ERR_FILE_ACCESS		(8)File Access Error
 *	@return		H265_ERR_FILE_NO_SUPPORT	(9)Not Support File Error
 *	@note		None
 *	@attention	None
 */
static INT32 h265_decode_top_image_get( ULLONG req_pts, T_H265_IF_VIDEO_YUV_INFO* decode_yuv_info )
{
	INT32					ret			= H265_NG;
	OS_USER_ER				ercd		= 0;
	OS_USER_FLGPTN			flgptn		= 0;
	OS_USER_FLGPTN			waiptn		= 0;
	UINT32					video_index	= 0;
	UINT32					bank_num	= 0;
	T_SR_FM_OUT_INFO		fminfo;
	
	// Decode Start Request.
	ret = H265_Sirius_Dec_Start( SR_DECODE_FORWARD );
	if( ret != H265_OK ){
		H265_Err_Print(("h265_decode_top_image_get : H265_Sirius_Dec_Start NG\n"));
		return H265_NG;
	}
	
	waiptn = ( FLG_H265_DEC_FRAMEEND_COMP | FLG_H265_DEC_ERROR );
	
	while(1){
		
		ret = h265_decode_next_video_sample_get( &video_index );
		if( ret != H265_OK ){
			H265_Err_Print(("h265_decode_top_image_get : Video stream has not been set.\n"));
			ret = H265_NG;
			break;
		}
		
		ret = h265_decode_set_vcpb( video_index, gH265_Disp_Ctrl.vdpb_use_num );
		if( ret != H265_OK ){
			H265_Err_Print(("h265_decode_top_image_get : vcpb setting error!!.\n"));
			break;
		}
		
		ercd = OS_User_Twai_Flg( FID_H265, waiptn, D_OS_USER_TWF_ORW, &flgptn, 1000 );
		if( ercd != D_OS_USER_E_OK ){
			H265_Err_Print(("h265_decode_top_image_get : Can not get Top Image for Sirius(T/O).\n"));
			ret = H265_NG;
			break;
		}
		OS_User_Clr_Flg( FID_H265, ~(FLG_H265_DEC_FRAMEEND_COMP | FLG_H265_DEC_ERROR) );
		
		if( flgptn & FLG_H265_DEC_ERROR ){
			H265_Err_Print(("h265_decode_top_image_get : conceal error detected.\n"));
			continue;
		}
		
		ret = h265_decode_get_vdpb( &fminfo );
		if( ret != H265_NG ){
			ret = H265_OK;
		}
		else{
			H265_Err_Print(("h265_decode_top_image_get : h265_decode_get_vdpb error. ret=%d\n", ret ));
			break;
		}
		
		if( fminfo.pts != (UINT32)req_pts ){
			continue;
		}
		
		bank_num = fminfo.id;
		Sdram_Map_Movie_Play_H265_Get_Vdec( bank_num, &decode_yuv_info->y_addr, &decode_yuv_info->cbcr_addr );
		
		decode_yuv_info->g_width	= gH265_Dec_Ctrl.bank_y_width;
		decode_yuv_info->g_lines	= gH265_Dec_Ctrl.bank_y_lines;
		decode_yuv_info->width		= gH265_Dec_Param.image_width;
		decode_yuv_info->lines		= gH265_Dec_Param.image_lines;
		decode_yuv_info->yuv_format	= E_H265_IF_YUV_FORMAT_VIDEO;
		break;
	}
	
	return ret;
}
