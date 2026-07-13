/**
 * @file		h264_decode.c
 * @brief		H.264 Decode Process.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#include "image_info.h"
#include "h264_decode.h"
#include "h265_decode.h"
#include "sdram_map_movie_play.h"
#include "sdram_map_movie_play_define.h"
#include "mba_if.h"
#include "h265_user.h"
#include "user_streaming.h"
#include "h265_movie.h"
#include "sdram_map_play.h"
#ifdef CO_H264_SECTION_ON
//#pragma arm section code="H264_CODE"
//#pragma arm section rodata="H264_CONST"
//#pragma arm section rwdata="H264_INIT"
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_H264_DEC_TIMER_FFREW_2X_TIME_SCALE		(4000)		// 250ms cycle (4fps)
#define D_H264_DEC_TIMER_FFREW_2X_TIME_DELTA		(1000)
#define D_H264_DEC_TIMER_FFREW_4X_TIME_SCALE		(8000)		// 125ms cycle (8fps)
#define D_H264_DEC_TIMER_FFREW_4X_TIME_DELTA		(1000)
#define D_H264_DEC_TIMER_FFREW_8X_TIME_SCALE		(16000)		// 62.5ms cycle (16fps)
#define D_H264_DEC_TIMER_FFREW_8X_TIME_DELTA		(1000)

#define D_H264_DEC_SPECIAL_TRICK_VDPB_COEFFICIENT	(4)			// for Reverse Play.

#define D_H264_DEC_MAX_PTS							(0x7FFFFFFF)

#define D_H264_DEC_NOT_SET_PTS						(0xFFFFFFFFFFFFFFFF)

#define D_H264_DEC_FIRST_BUFFERING_COEFFICIENT		(10)		// Based on the "30fps is 3frame buffering". buffring_num = fps / coefficient.

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum{
	E_H264_DEC_PTS_SEARCH_MODE_SAME			= 0,	// ex.Use Normal Play
	E_H264_DEC_PTS_SEARCH_MODE_SMALL		= 1,	// ex.Use FF
 	E_H264_DEC_PTS_SEARCH_MODE_BIG			= 2,	// ex.Use REW
} E_H264_DEC_PTS_SEARCH_MODE;


typedef enum{
	E_H264_DEC_DISPLAY_DEVICE_LCD			= 0,	// LCD
	E_H264_DEC_DISPLAY_DEVICE_HDMI			= 1,	// HDMI
} E_H264_DEC_DISPLAY_DEVICE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* --- Decode Condition --------------------------------------------------- */
static	volatile	T_H264_IF_DECODER_PARAM		gDec_Param;
/* --- Decode control data ------------------------------------------------ */
static	volatile	T_H264_DEC_CTL				gDec_Ctrl;
/* --- Demux Control Data ------------------------------------------------- */
static	volatile	T_H264_DEC_DMUX_CTL			gDmux_Ctrl;
/* --- Display Control Data ------------------------------------------------- */
static	volatile	T_H264_DEC_DISP_CTL			gDisp_Ctrl;

static	volatile	UINT32						gDec_Serial_Num;		// for VCPB request


/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static	VOID				h264_decode_initialize( T_H264_IF_DECODER_PARAM* decoder_param, T_H264_IF_DISPLAY_INFO* display_info );
static	VOID				h264_decode_initialize_gdec_param( T_H264_IF_DECODER_PARAM* decoder_param );
static	VOID				h264_decode_initialize_gdec_control( VOID );
static	VOID				h264_decode_initialize_gdmux_control( VOID );
static	VOID				h264_decode_initialize_gdisp_control( T_H264_IF_DISPLAY_INFO* display_info );
static	INT32				h264_decode_init_param( VOID );
static	VOID				h264_decode_task_start( UINT32 task_id );
static	VOID				h264_decode_task_forced_stop( VOID );
static	E_H264_PLAY_MODE	h264_decode_flush( VOID );
static	E_H264_PLAY_MODE	h264_decode_play( VOID );
static	E_H264_PLAY_MODE	h264_decode_pause( VOID );
static	E_H264_PLAY_MODE	h264_decode_resume( VOID );
static	E_H264_PLAY_MODE	h264_decode_stop( VOID );
static	E_H264_PLAY_MODE	h264_decode_end( VOID );
static	INT32				h264_decode_set_vcpb( UINT32 video_index, UCHAR vdpb_use_num);
static	INT32				h264_decode_next_video_sample_get( UINT32* video_index );
static	INT32				h264_decode_next_video_sample_request( VOID );
static	INT32				h264_decode_get_vdpb( UINT32* bank_num, T_RB_INFOTABLE_VDPB* vdpb_info );
static	VOID				h264_decode_vdpb_free( E_H264_IF_PLAY_DIRECTION direction, T_H264_DEC_DISP_REFER_INDEX protect_index );
static	INT32				h264_decode_search_display_index( E_H264_DEC_PTS_SEARCH_MODE search_mode, UCHAR* display_index, T_H264_DEC_DISP_REFER_INDEX protect_index );
static	VOID				h264_decode_display_image( T_H264_IF_DISPLAY_PARAM* display_info, T_H264_MOVIE_ADDR* dec_addr, T_H264_MOVIE_ADDR* disp_addr, BOOL ribery_output );
static	INT32				h264_decode_top_image_get( ULLONG req_pts, T_H264_IF_VIDEO_YUV_INFO* decode_yuv_info );
static	VOID				h264_decode_flush_execute( UCHAR* disp_bank );

typedef E_H264_PLAY_MODE (*T_H264_DEC_PLAY_FUNC)(VOID);

const T_H264_DEC_PLAY_FUNC mp4dec_play_func[E_H264_PLAY_MODE_MAX] = {
	h264_decode_flush,
	h264_decode_play,
	h264_decode_pause,
	h264_decode_resume,
	h264_decode_stop,
	h264_decode_end
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
VOID H264_Decode_ENT_Main( VOID )
{
	INT32			ret;
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flgptn;
	OS_USER_FLGPTN	waiptn;
	UINT32			video_index;
	UINT32			vcpb_index;
	UINT32			next_free_pointer;
	BOOL			stop_req_flag	= FALSE;
	BOOL			stop_wait_flag	= FALSE;
#if 0	// for Debug
	OS_USER_SYSTIM					time_before, time_after;
#endif	// for Debug
	
	/* Report ENT Task Initailization End */
	OS_User_Set_Flg( FID_H264, FLG_H264_ENT_INIT_END );
	
	// Check Another Task Result
	waiptn = (FLG_H264_START | FLG_H264_STOP);
	OS_User_Wai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	
/* --- Video Stream Transfer to Ribery Loop Process --- */
	while(1){
		/* Loop Break by User STOP Request */
		if( stop_req_flag == TRUE ){
			break;
		}
		/* Check User Stop Request */
		ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_STOP, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			stop_req_flag = TRUE;
			H264_Info_Print(("+++ ENT: H264_STOP Event received\n" ));
			continue;
		}
		else if( ercd == D_OS_USER_E_TMOUT ) {
			/* No event */
			;
		}
		else{
			H264_Err_Print(("--- ENT: OS_User_Pol_Flg error!! error=%d\n", ercd ));
		}
		
		if( (gDec_Ctrl.error == TRUE) || (stop_wait_flag == TRUE) ){
			// In the case of Flush request, it will release the stop waiting.
			ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_FLUSH_OPS, D_OS_USER_TWF_ORW, &flgptn );
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
		ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_FLUSH_OPS, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			OS_User_Set_Flg( FID_H264, FLG_H264_ENT_STATE_CHANGE );
			
			// Initialize Stream Control.
			Sdram_Map_Movie_Play_H264_Stream_Ctrl_Initialize();
			
			// wait for start/stop
			OS_User_Wai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			// dispatch trigger
			OS_User_Dly_Tsk( 1 );
			continue;
		}
		
		ret = h264_decode_next_video_sample_get( &video_index );
		if( ret != H264_OK ){
			// Video Stream is nothing.
			// Wait until the stream is feed.
			
#if 0	// for Debug
			OS_User_Get_Tim(&time_before);
#endif	// for Debug
			
			OS_User_Wai_Flg( FID_H264, (FLG_H264_DEC_STREAM_RECEIVE | FLG_H264_FLUSH_OPS | FLG_H264_STOP), D_OS_USER_TWF_ORW, &flgptn );
			OS_User_Clr_Flg( FID_H264, ~FLG_H264_DEC_STREAM_RECEIVE );
			
#if 0	// for Debug
			OS_User_Get_Tim(&time_after);
			H264_Info_Print(("FeedWaitTime: %d\n", (UINT32)(time_after - time_before) ));
#endif
			continue;
		}
		
		if( gDmux_Ctrl.decode_video[video_index].pts > D_H264_DEC_MAX_PTS ){
			H264_Err_Print(("--- ENT: It is possible to have exceeded the PTS to be handled. %d\n", D_H264_DEC_MAX_PTS ));
			gDec_Ctrl.error = TRUE;
			break;
		}
		
		while(1){
			
			// VCPB_Info_Table Free
 			while(1){
				ret = H264_Ribery_Dec_VCPB_Free( &vcpb_index );
				if( (ret == H264_OK) || (ret == H264_OK_COMPLETE) ){
					// free vcpb index check.
					next_free_pointer = gDmux_Ctrl.free_pointer + 1;
					if( next_free_pointer >= D_H264_DEC_SAVE_SAMPLE_MAX_NUM ){
						next_free_pointer = 0;
					}
					if( vcpb_index == gDmux_Ctrl.decode_video[next_free_pointer].vcpb_index ){
						Sdram_Map_Movie_Play_H264_Release_Stream_Area( gDmux_Ctrl.decode_video[next_free_pointer].stream_address + gDmux_Ctrl.decode_video[next_free_pointer].stream_size );
					}
					else{
						// Since VCPB has not been released to the sequential, and will not be released in the stream area.
					}
					// free_pointer update
					gDmux_Ctrl.free_pointer = next_free_pointer;
				}
				// H264_OK will continue the loop processing because there remains a VCPB to release.
				if( ret != H264_OK ){
					break;
				}
			}
			if( ret == H264_ERR_SYSTEM ){
				H264_Err_Print(("--- ENT: H264_Ribery_Dec_VCPB_Free Error.\n" ));
				gDec_Ctrl.error = TRUE;
				break;
			}
			// VCPB_Infor_Table Set
			ret = h264_decode_set_vcpb( video_index, gDisp_Ctrl.vdpb_use_num );
			if( ret == H264_OK ){
				
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Vcpb:%lu", (ULONG)gDmux_Ctrl.decode_video[video_index].pts ));
#else
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Vcpb:%llu", gDmux_Ctrl.decode_video[video_index].pts ));
#endif
				
				if( (gDec_Ctrl.direction == E_H264_IF_PLAY_DIRECTION_RWD) && (gDec_Ctrl.iframe_only == FALSE) ){
					if( gDmux_Ctrl.decode_video[video_index].pts == 0 ){
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
						H264_Info_Print(("+++ ENT: Last Frame Detection.[pts:%lu]\n", (ULONG)gDmux_Ctrl.decode_video[video_index].pts ));
#else
						H264_Info_Print(("+++ ENT: Last Frame Detection.[pts:%llu]\n", gDmux_Ctrl.decode_video[video_index].pts ));
#endif
						gDec_Ctrl.last_display_pts = gDmux_Ctrl.decode_video[video_index].pts;
					}
					if( gDmux_Ctrl.decode_video[video_index].stream_end_flag == 1 ){
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
						H264_Info_Print(("+++ ENT: Last Stream Detection.[pts:%lu]\n", (ULONG)gDmux_Ctrl.decode_video[video_index].pts ));
#else
						H264_Info_Print(("+++ ENT: Last Stream Detection.[pts:%llu]\n", gDmux_Ctrl.decode_video[video_index].pts ));
#endif
						gDec_Ctrl.last_stream_pts = gDmux_Ctrl.decode_video[video_index].pts;
						stop_wait_flag = TRUE;
					}
				}
				else if( (gDec_Ctrl.direction == E_H264_IF_PLAY_DIRECTION_FWD) && (gDec_Ctrl.iframe_only == FALSE) ){
					if( gDmux_Ctrl.decode_video[video_index].stream_end_flag == 1 ){
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
						H264_Info_Print(("+++ ENT: Last Stream Detection.[pts:%lu]\n", (ULONG)gDmux_Ctrl.decode_video[video_index].pts ));
#else
						H264_Info_Print(("+++ ENT: Last Stream Detection.[pts:%llu]\n", gDmux_Ctrl.decode_video[video_index].pts ));
#endif
						gDec_Ctrl.last_stream_pts	= gDmux_Ctrl.decode_video[video_index].pts;
						stop_wait_flag = TRUE;
					}
				}
				else{
					// case of FWD/RWD I only.
					if( gDmux_Ctrl.decode_video[video_index].stream_end_flag == 1 ){
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
						H264_Info_Print(("+++ ENT: Last Frame/Stream Detection.[pts:%lu]\n", (ULONG)gDmux_Ctrl.decode_video[video_index].pts ));
#else
						H264_Info_Print(("+++ ENT: Last Frame/Stream Detection.[pts:%llu]\n", gDmux_Ctrl.decode_video[video_index].pts ));
#endif
						gDec_Ctrl.last_display_pts	= gDmux_Ctrl.decode_video[video_index].pts;
						gDec_Ctrl.last_stream_pts	= gDmux_Ctrl.decode_video[video_index].pts;
						stop_wait_flag = TRUE;
					}
				}
				break;
			}
			else{
				OS_User_Dly_Tsk( 1 );	// dispatch trigger
			}
			
			// FLUSH operation detect.
			ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_FLUSH_OPS, D_OS_USER_TWF_ORW, &flgptn );
			if( ercd == D_OS_USER_E_OK ) {
				break;
			}
			// Check User Stop Request
			ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_CC_STOP, D_OS_USER_TWF_ORW, &flgptn );
			if( ercd == D_OS_USER_E_OK ) {
				stop_req_flag = TRUE;
				H264_Info_Print(("+++ ENT: CC_STOP Event received\n" ));
				break;
			}
			else if( ercd == D_OS_USER_E_TMOUT ) {
				// No event
				;
			}
			else{
				H264_Err_Print(("--- ENT: OS_User_Pol_Flg error!! error=%d\n", ercd ));
			}
			if( gDec_Ctrl.error == TRUE ){
				break;
			}
		}
	}
	
	// Wait CC Task End
	OS_User_Wai_Flg( FID_H264, FLG_H264_CC_STOP, D_OS_USER_TWF_ORW, &flgptn );
	
	// Set ENT Task End
	OS_User_Set_Flg( FID_H264, FLG_H264_ENT_STOP );
	
	return;
}

/**
 *  @brief		Movie Decode Codec Core Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Decode_CC_Main( VOID )
{
	INT32				ret;
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn;
	UINT32				bank_num;
	T_RB_INFOTABLE_VDPB	vdpb_info;
#if 0	// for Debug
	OS_USER_SYSTIM					time_before, time_after;
#endif	// for Debug
	
	/* Report CCch Task Initailization End */
	OS_User_Set_Flg( FID_H264, FLG_H264_CC_INIT_END );
	
	// Check Another Task Result
	waiptn = (FLG_H264_START | FLG_H264_STOP);
	OS_User_Wai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	
	// Check Another Task Result
	waiptn = (FLG_H264_DEC_VDPB_COMP | FLG_H264_STOP | FLG_H264_DISP_STOP | FLG_H264_CODEC_FLUSH_COMP | FLG_H264_DEC_ERROR | FLG_H264_WDT );
#if 0	// for Debug
	OS_User_Get_Tim(&time_before);
#endif	// for Debug
	while(1){
		OS_User_Wai_Flg( FID_H264, waiptn,D_OS_USER_TWF_ORW, &flgptn );
		/* Received STOP Event */
		if( 	 0 != ( flgptn & FLG_H264_DISP_STOP	)){
			H264_Info_Print(("+++ CC: DISP_STOP Event received\n"	));
			break;
		}
		else if( 0 != ( flgptn & FLG_H264_STOP )){
			H264_Info_Print(("+++ CC: STOP Event received\n"		));
			OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_IDX_SET );
			OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_UPDATE );
			break;
		}
		else if( 0 != ( flgptn & FLG_H264_DEC_ERROR )){
			OS_User_Clr_Flg( FID_H264, ~FLG_H264_DEC_ERROR );
			H264_Info_Print(("+++ CC: Codec Error Event received\n"	));
			gDec_Ctrl.error = TRUE;
			continue;
		}
		else if( 0 != ( flgptn & FLG_H264_WDT )){
			OS_User_Clr_Flg( FID_H264, ~FLG_H264_WDT );
			H264_Info_Print(("+++ CC: WDT Event received\n"		));
			gDec_Ctrl.error = TRUE;
			continue;
		}
#if 0	// for Debug
		OS_User_Get_Tim(&time_after);
		
		H264_Info_Print(("CcTime: %d\n", (UINT32)(time_after - time_before) ));
		
		OS_User_Get_Tim(&time_before);
#endif
		
		OS_User_Clr_Flg( FID_H264, ~FLG_H264_DEC_VDPB_COMP );
		
		if( gDec_Ctrl.error == TRUE ){
			// wait for dispatch
			OS_User_Dly_Tsk( 10 );
			
			// It waits for a stop from the host.
			continue;
		}
		
		// FLUSH operation detect.
		if( flgptn & FLG_H264_CODEC_FLUSH_COMP){
			OS_User_Clr_Flg( FID_H264, ~FLG_H264_CODEC_FLUSH_COMP );
			OS_User_Set_Flg( FID_H264, FLG_H264_CC_STATE_CHANGE );
			
			// wait start/stop
			waiptn = (FLG_H264_START | FLG_H264_STOP);
			OS_User_Wai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			
			// wait pattern resetting
			waiptn = (FLG_H264_DEC_VDPB_COMP | FLG_H264_STOP | FLG_H264_DISP_STOP | FLG_H264_CODEC_FLUSH_COMP | FLG_H264_WDT );
			continue;
		}
		/* Get Bank Number and VDPB Information as Decode Result. */
		ret = h264_decode_get_vdpb( &bank_num, &vdpb_info );
		if( ret == H264_OK ){
			OS_User_Dly_Tsk(1);	// dispatch trigger
			OS_User_Set_Flg( FID_H264, FLG_H264_DEC_VDPB_COMP );
		}
		else if( ret == H264_OK_COMPLETE ){
			// only 1 request.
			;
		}
		else{
			// VDPB is nothing(continue).
			continue;
		}
		if( vdpb_info.err_code ){
			if( gDec_Param.conceal_error == E_H264_IF_CONCEAL_ERROR_STOP ){
				H264_Err_Print(("--- conceal error detected.(%lu)\n", vdpb_info.err_code ));
				gDec_Ctrl.error = TRUE;
				continue;
			}
		}
		
		/* It checks Bank Number */
		if( bank_num >= gDisp_Ctrl.vdpb_use_num ){
			H264_Info_Print(("--- h264_decode_get_vdpb: Bank Number is max over!! bank=%d\n", bank_num));
			gDec_Ctrl.task_result = H264_ERR_SYSTEM;
			OS_User_Set_Flg( FID_H264, FLG_H264_STOP );
			break;
		}
		/* Make Display Control Table */
		gDisp_Ctrl.inf[bank_num].PTS			= vdpb_info.PTS;
		gDisp_Ctrl.inf[bank_num].DTS			= vdpb_info.DTS;
		gDisp_Ctrl.inf[bank_num].release_flg	= FALSE;
		gDisp_Ctrl.buf_count++;
		gDisp_Ctrl.inf[bank_num].use_flg		= TRUE;
#if 0	// for Debug

#if 1	// MILB : Movie porting. printf 64bit Not Supported.
		H264_Info_Print(("decPTS: [%lu] [%lu] %lu\n", bank_num, gDisp_Ctrl.buf_count, (ULONG)gDisp_Ctrl.inf[bank_num].PTS ));
#else
		H264_Info_Print(("decPTS: [%lu] [%lu] %llu\n", bank_num, gDisp_Ctrl.buf_count, gDisp_Ctrl.inf[bank_num].PTS ));
#endif

#endif
		if( gDisp_Ctrl.inf[bank_num].PTS == gDec_Ctrl.last_stream_pts ){
			gDec_Ctrl.last_stream_dec_done = TRUE;
		}
		
		// Video Stream Request to HostFW.
		h264_decode_next_video_sample_request();
		
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Vdpb:%lu", (ULONG)gDisp_Ctrl.inf[bank_num].PTS ));
#else
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Vdpb:%llu", gDisp_Ctrl.inf[bank_num].PTS ));
#endif
	}
	
	// Wait DISP Task End
	OS_User_Wai_Flg( FID_H264, FLG_H264_DISP_STOP, D_OS_USER_TWF_ORW, &flgptn );
	
	/* Set ENT Task End */
	OS_User_Set_Flg( FID_H264, FLG_H264_CC_STOP );
	
	return;
}


/**
 *  @brief		Movie Decode Display Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Decode_DISP_Main( VOID )
{
	OS_USER_FLGPTN					flgptn;
	OS_USER_FLGPTN					waiptn;
	OS_USER_FLGPTN					pause_waiptn;
	INT32							ret;
	OS_USER_ER						ercd;
	BOOL							stop_wait_flag = FALSE;
	E_H264_PLAY_MODE				play_mode;
	UCHAR							first_buf_num		= 0;
	BOOL							first_play_flg		= TRUE;	/* Play/Replay: it need 4 banks image */
	BOOL							display_target_flg;
	UCHAR							detect_index = 0;
	UCHAR							skip_count=0;
	UCHAR							retry_cnt = 0;
	T_H264_DEC_DISP_REFER_INDEX		protect_index = { 0xFF, 0xFF, 0xFF, 0xFF };
	UINT32							thinning_counter = 0;
	UINT32							thinning_coefficient = 0;
	E_H264_DEC_PTS_SEARCH_MODE		pts_search_mode;
	
	waiptn			= 0;
	pause_waiptn	= 0;
	
	// Start LCD task.
	if( gDisp_Ctrl.device_info.lcd_info.valid_flag ){
		h264_decode_task_start( TID_H264_DISP_LCD );
		waiptn |= FLG_H264_DISP_LCD_INIT_END;
		pause_waiptn |= FLG_H264_DECODER_DISP_LCD_PAUSE;
	}
	// Start HDMI task.
	if( gDisp_Ctrl.device_info.hdmi_info.valid_flag ){
		h264_decode_task_start( TID_H264_DISP_HDMI );
		waiptn |= FLG_H264_DISP_HDMI_INIT_END;
		pause_waiptn |= FLG_H264_DECODER_DISP_HDMI_PAUSE;
	}
	
	// task init end wait.
	OS_User_Wai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
	
	/* Set Event for Task Start */
	OS_User_Set_Flg( FID_H264, FLG_H264_DISP_INIT_END );
	
	while( 1 ){
#ifdef CO_DEBUG_ON_PC
		OS_User_Dly_Tsk( 200 );
		OS_User_Set_Flg( FID_H264_SYNC, FLG_H264_SYNC_DEC );
#endif
		/* Wait Sync(Reload Timer) */
		OS_User_Wai_Flg( FID_H264_SYNC, FLG_H264_SYNC_DEC, D_OS_USER_TWF_ORW, &flgptn );
		OS_User_Clr_Flg( FID_H264_SYNC, ~FLG_H264_SYNC_DEC );
		
		ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_STOP, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_IDX_SET );
			OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_UPDATE );
			/* Received STOP Event */
			H264_Info_Print(("+++ DISP: H264_STOP Event received\n" ));
			break;
		}
		
		if( stop_wait_flag == TRUE ){
			// In the case of Flush request, it will release the stop waiting.
			ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_FLUSH_OPS, D_OS_USER_TWF_ORW, &flgptn );
			if( ercd == D_OS_USER_E_OK ) {
				stop_wait_flag = FALSE;
			}
			
			if( gDec_Ctrl.mode == E_H264_PLAY_MODE_PAUSE ){
				stop_wait_flag = FALSE;
				continue;
			}
			
			// wait for dispatch
			OS_User_Dly_Tsk( 10 );
			
			// It waits for a stop from the host.
			if( gDec_Ctrl.mode == E_H264_PLAY_MODE_STOP ){
				// Transition to the end processing (setting the FLG_H264_STOP)
				break;
			}
			continue;
		}
		// FLUSH operation detect.
		ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_FLUSH_OPS, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			// Check display task status.
			ercd = OS_User_Pol_Flg( FID_H264_DECODER, pause_waiptn, D_OS_USER_TWF_ANDW, &flgptn );
			if( ercd != D_OS_USER_E_OK ) {
				// It is not already in the pause state.
				OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_PAUSE_REQUEST );
				OS_User_Wai_Flg( FID_H264_DECODER, pause_waiptn, D_OS_USER_TWF_ANDW, &flgptn );
				OS_User_Clr_Flg( FID_H264_DECODER, ~FLG_H264_DECODER_DISP_START );
				
				first_play_flg = TRUE;
			}
			
			protect_index.lcd_decide		= gDisp_Ctrl.refer_index.lcd_decide;
			protect_index.lcd_displaying	= gDisp_Ctrl.refer_index.lcd_displaying;
			protect_index.hdmi_decide		= gDisp_Ctrl.refer_index.hdmi_decide;
			protect_index.hdmi_displaying	= gDisp_Ctrl.refer_index.hdmi_displaying;
			
			gDisp_Ctrl.inf[gDisp_Ctrl.display_index].release_flg = TRUE;
			OS_User_Set_Flg( FID_H264, FLG_H264_DISP_STATE_CHANGE );
			continue;
		}
		
		play_mode = gDec_Ctrl.mode;
		
		if( (gDec_Ctrl.pause == TRUE) && (first_play_flg == TRUE) ){
			play_mode = E_H264_PLAY_MODE_PLAY;
		}
		
		switch( play_mode ){
			case E_H264_PLAY_MODE_PLAY:
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
					
					switch( gDec_Ctrl.speed ){
						case E_H264_IF_PLAY_SPEED_1_2X:
							thinning_coefficient = 2;
							break;
						case E_H264_IF_PLAY_SPEED_1_4X:
							thinning_coefficient = 4;
							break;
						case E_H264_IF_PLAY_SPEED_1_8X:
							thinning_coefficient = 8;
							break;
						default:
							thinning_coefficient = 0;
							break;
					}
					
					// VDPB free
					if( gDec_Ctrl.iframe_only == FALSE ){
						h264_decode_vdpb_free( gDec_Ctrl.direction, protect_index );
					}
					
					if( (gDec_Ctrl.direction == E_H264_IF_PLAY_DIRECTION_RWD) && (gDec_Ctrl.iframe_only == FALSE) ){
						
						if( gDec_Ctrl.trick_limit == TRUE ){
							
							H264_Err_Print(( "--- Disp_Main : It has detected a Trick limit-breaking.\n" ));
							
							// Having detected an error, and shifts to the stop waiting state.
							gDec_Ctrl.error = TRUE;
							Host_H264_Notify_VideoDecodeCB( E_H264_IF_PLAY_DECODER_STATUS_NORMAL, gDisp_Ctrl.display_pts, gDec_Ctrl.error );
							stop_wait_flag = TRUE;
							continue;
						}
						
						first_buf_num = gDisp_Ctrl.vdpb_use_num;
					}else{
						if (User_IsDualStream()) {
							first_buf_num = 1;
						} else {
							if( (gDec_Param.time_scale / gDec_Param.time_delta) % D_H264_DEC_FIRST_BUFFERING_COEFFICIENT ){
								first_buf_num = ((gDec_Param.time_scale / gDec_Param.time_delta) / D_H264_DEC_FIRST_BUFFERING_COEFFICIENT) + 1;
							}else{
								first_buf_num = ((gDec_Param.time_scale / gDec_Param.time_delta) / D_H264_DEC_FIRST_BUFFERING_COEFFICIENT);
							}
							if( first_buf_num < 3 ){
								first_buf_num = 3;
							}
							if( first_buf_num > gDisp_Ctrl.vdpb_use_num ){
								first_buf_num = gDisp_Ctrl.vdpb_use_num;
							}
						}
					}
					if ( (gDisp_Ctrl.buf_count >= first_buf_num) || (gDec_Ctrl.last_stream_dec_done == TRUE) ){
						if( gDec_Ctrl.iframe_only == FALSE ){	// All frame decode/display
							// Decode waiting of a display picture.
							ret = h264_decode_search_display_index( E_H264_DEC_PTS_SEARCH_MODE_SAME, &detect_index, protect_index );
							if( ret == H264_OK ){
								display_target_flg = TRUE;
							}
						}else{	// I-frame only decode/display
							display_target_flg = TRUE;
						}
					}
				}
				else{
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
				
			case E_H264_PLAY_MODE_PAUSE:
				
				// Check display task status.
				ercd = OS_User_Pol_Flg( FID_H264_DECODER, pause_waiptn, D_OS_USER_TWF_ANDW, &flgptn );
				if( ercd != D_OS_USER_E_OK ) {
					// It is not already in the pause state.
					OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_PAUSE_REQUEST );
					OS_User_Wai_Flg( FID_H264_DECODER, pause_waiptn, D_OS_USER_TWF_ANDW, &flgptn );
					OS_User_Clr_Flg( FID_H264_DECODER, ~FLG_H264_DECODER_DISP_START );
					
					// change target pts
					gDisp_Ctrl.display_target_pts	= gDisp_Ctrl.display_pts;
					first_play_flg					= TRUE;
					gDec_Ctrl.pause					= FALSE;
					H264_If_Play_Notify_STATE_CHANGE_COMPLETE( H264_OK );
				}
				
				// VDPB free
				h264_decode_vdpb_free( gDec_Ctrl.direction, protect_index );
				display_target_flg = FALSE;
				break;
				
			default :	/* Unknown Case */
				display_target_flg = FALSE;
		}
		
		/* Stop Request End */
		if( play_mode == E_H264_PLAY_MODE_STOP ){
			break;
		}
		
		if( gDec_Ctrl.error == TRUE ){
			// Having detected an error, and shifts to the stop waiting state.
			stop_wait_flag = TRUE;
			continue;
		}
		
		/* It judge need to display image */
		if( display_target_flg == FALSE ){
			continue;
		}
		
		switch( play_mode ){
			case E_H264_PLAY_MODE_PLAY:		// Fallthrough
				
				if( gDec_Ctrl.iframe_only == FALSE ){	// All frame decode/display

					if (User_IsDualStream() &&	User_IsH265MainStream() && !H265_IsLastPTSSet_Or_Error()) {
						ercd = OS_User_Wai_Flg(FID_H265_DECODER, FLG_H265_DECODER_DISP_IDX_SET, D_OS_USER_TWF_ORW, &flgptn);
						if( ercd != D_OS_USER_E_OK ) H264_Err_Print(( "Disp_Main : OS_User_Wai_Flg [FLG_H265_DECODER_DISP_IDX_SET] NG\n" ));
						ercd = OS_User_Clr_Flg(FID_H265_DECODER, ~FLG_H265_DECODER_DISP_IDX_SET);
						if( ercd != D_OS_USER_E_OK ) H264_Err_Print(( "Disp_Main : OS_User_Clr_Flg [FLG_H265_DECODER_DISP_IDX_SET] NG\n" ));
					}

					if( (gDec_Ctrl.last_stream_dec_done == TRUE) && (gDec_Ctrl.last_display_pts == D_H264_DEC_NOT_SET_PTS) ){
						// From the decoded frame, to determine the last PTS.(Maximum of PTS)
						ret = h264_decode_search_display_index( E_H264_DEC_PTS_SEARCH_MODE_BIG, &detect_index, protect_index );
						H264_Info_Print(("+++ DISP: Last Frame Detection.[pts:%lu]\n", (ULONG)gDisp_Ctrl.inf[detect_index].PTS ));
						gDec_Ctrl.last_display_pts = gDisp_Ctrl.inf[detect_index].PTS;
					}
					
					// Search for an image to be displayed using the "display_target_pts".
					ret = h264_decode_search_display_index( E_H264_DEC_PTS_SEARCH_MODE_SAME, &detect_index, protect_index );
					if (ret == H264_OK) {
						if( gDec_Ctrl.direction == E_H264_IF_PLAY_DIRECTION_FWD ){
							// display_target_pts update
							if( gDec_Param.scan_mode == E_H264_IF_SCAN_MODE_PROGRESSIVE ){
								gDisp_Ctrl.display_target_pts += gDec_Param.time_delta;
							}else{
								gDisp_Ctrl.display_target_pts += (gDec_Param.time_delta * 2);
							}
						}else{
							// display_target_pts update
							if( gDisp_Ctrl.display_target_pts >= gDec_Param.time_delta ){
								if( gDec_Param.scan_mode == E_H264_IF_SCAN_MODE_PROGRESSIVE ){
									gDisp_Ctrl.display_target_pts -= gDec_Param.time_delta;
								}else{
									gDisp_Ctrl.display_target_pts -= (gDec_Param.time_delta * 2);
								}
							}else{}
						}
					}
				} else {	// I-frame only decode/display
					if( gDec_Ctrl.direction == E_H264_IF_PLAY_DIRECTION_FWD ){
						pts_search_mode = E_H264_DEC_PTS_SEARCH_MODE_SMALL;
					}else{
						pts_search_mode = E_H264_DEC_PTS_SEARCH_MODE_BIG;
					}
					
					ret = h264_decode_search_display_index( pts_search_mode, &detect_index, protect_index );
					gDisp_Ctrl.display_target_pts = gDisp_Ctrl.inf[detect_index].PTS;
				}
				// Check whether there is a display frame.
				if( ret != H264_OK ){
					/* Not detect sample. so, in this timing, system does not change image data */
					H264_Info_Print(("--- Not detect sample. --- target_pts:%ld\n", (LONG)gDisp_Ctrl.display_target_pts));
					M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Not Detect Sample PTS:%ld", (LONG)gDisp_Ctrl.display_target_pts));
#if 0				// Try 10 times to get that frame if over, break the decoder
					skip_count++;
					if( skip_count > 10 ){
						// Not detect sample. so, in this timing, FS read worst!?
						break;
					}
					continue;
#else				// ** Forcibly continue playing **
					// ** Forced to assign a target PTS because display_pts is not updated If not display even once. **
					retry_cnt++;
					if (retry_cnt > 10 && gDisp_Ctrl.display_target_pts != gDec_Ctrl.last_display_pts) {
						// Try to get next PTS, skip this frame of PTS which is not found
						if( gDec_Ctrl.iframe_only == FALSE && gDec_Ctrl.direction == E_H264_IF_PLAY_DIRECTION_FWD ){
							// display_target_pts resume to last one not detected
							if( gDec_Param.scan_mode == E_H264_IF_SCAN_MODE_PROGRESSIVE ){
								gDisp_Ctrl.display_target_pts += gDec_Param.time_delta;
							}else{
								gDisp_Ctrl.display_target_pts += (gDec_Param.time_delta * 2);
							}
						}
						retry_cnt = 0;
					}
					gDisp_Ctrl.display_pts = gDisp_Ctrl.display_target_pts;
					h264_decode_vdpb_free( gDec_Ctrl.direction, protect_index );
#endif
				} else {
					retry_cnt = 0;
					M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Disp:%lu", (ULONG)gDisp_Ctrl.inf[detect_index].PTS ));
				}
				break;
			default :	/* Not exist case */
				break;
		} // switch( play_mode ){
		
		if( skip_count > 10 ){			
			// Having detected an error, and shifts to the stop waiting state.
			gDec_Ctrl.error = TRUE;
			Host_H264_Notify_VideoDecodeCB( E_H264_IF_PLAY_DECODER_STATUS_NORMAL, gDisp_Ctrl.display_pts, gDec_Ctrl.error );
			stop_wait_flag = TRUE;
			continue;
		}else{
			// skip_count reset
			skip_count = 0;
		}
		
		ercd = OS_User_Twai_Sem(SID_H264, H264_DEC_SEMAPHORE_TIMEOUT);
		if( ercd != D_OS_USER_E_OK ){
			H264_Err_Print(( "--- Disp_Main : gDisp_Ctrl semaphore NG\n" ));
		}
		// information update
		gDisp_Ctrl.display_index		= detect_index;						// display target index
		
		if (detect_index != 0xFF) {
			OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_IDX_SET );
		}

		if (detect_index != 0xFF) {
			gDisp_Ctrl.display_pts			= gDisp_Ctrl.inf[detect_index].PTS;	// next   :Vsync display area (PTS keep)
			gDisp_Ctrl.display_dts			= gDisp_Ctrl.inf[detect_index].DTS;	//                            (DTS keep)
		}
		protect_index.lcd_decide		= gDisp_Ctrl.refer_index.lcd_decide;
		protect_index.lcd_displaying	= gDisp_Ctrl.refer_index.lcd_displaying;
		protect_index.hdmi_decide		= gDisp_Ctrl.refer_index.hdmi_decide;
		protect_index.hdmi_displaying	= gDisp_Ctrl.refer_index.hdmi_displaying;
		
		OS_User_Sig_Sem(SID_H264);
		
		if( first_play_flg == TRUE ){
			OS_User_Clr_Flg( FID_H264_DECODER, ~FLG_H264_DECODER_DISP_PAUSE_REQUEST );
			if (!User_IsDualStream()) {
				// Wait the first display.
				OS_User_Wai_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_START, D_OS_USER_TWF_ORW, &flgptn );
			}
		}
		
		if (User_IsDualStream() && detect_index != 0xFF) {
			OS_User_Wai_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_FRAME_DONE, D_OS_USER_TWF_ORW, &flgptn );
			OS_User_Clr_Flg(FID_H264_DECODER, ~FLG_H264_DECODER_DISP_FRAME_DONE);
		}

		first_play_flg = FALSE;
		
		if( (gDec_Ctrl.error == TRUE) || (gDisp_Ctrl.display_pts == gDec_Ctrl.last_display_pts) ){
			// Having detected an error, and shifts to the stop waiting state.
			stop_wait_flag = TRUE;
			
			if( gDec_Ctrl.pause == TRUE ){
				// In the case of the last frame display by the pause state, not to forcibly stop state.
				stop_wait_flag = FALSE;
			}
		}
		// VDPB free
		h264_decode_vdpb_free( gDec_Ctrl.direction, protect_index );
	} // while( 1 ){
	
	if (BF_H265_User_Is_Play_Stop()) {
		/* Reload Timer Stop */
		BF_Movie_Common_Timer_Stop( E_BF_MOVIE_COMMON_RTIMER_MODE_DEC );
	}
	
	// Set Stop
	OS_User_Set_Flg( FID_H264, FLG_H264_STOP );
	
	waiptn = 0;
	
	if( gDisp_Ctrl.device_info.lcd_info.valid_flag ){
		waiptn |= FLG_H264_DISP_LCD_STOP;
	}
	if( gDisp_Ctrl.device_info.hdmi_info.valid_flag ){
		waiptn |= FLG_H264_DISP_HDMI_STOP;
	}
	
	// LCD/HDMI task end wait.
	OS_User_Wai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
	
	/* Set Task End */
	OS_User_Set_Flg( FID_H264, FLG_H264_DISP_STOP );
	
	return;
}

/**
 *  @brief		Movie Decode Display Lcd Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Decode_DISP_LCD_Main( VOID )
{
	OS_USER_ER						ercd;
	OS_USER_FLGPTN					flgptn;
	BOOL							first_display_flag		= TRUE;
	UINT8							lcd_decide_index		= 0xFF;
	UINT8							lcd_displaying_index	= 0xFF;
	T_H264_MOVIE_ADDR				dec_addr;
	T_H264_MOVIE_ADDR				disp_addr;
	BOOL							stop_req_flag			= FALSE;
	BOOL							pause_complete_flag		= FALSE;
	BOOL							ribery_output;
	E_H264_IF_PLAY_DECODER_STATUS	decoder_status;
	BOOL							update = TRUE;
	USHORT							lcd_img_w;
	USHORT							lcd_img_l;
#if 0	// for Debug
	OS_USER_SYSTIM					time_before, time_after;
#endif	// for Debug
	
	lcd_img_w = gDisp_Ctrl.device_info.lcd_info.image_width;
	lcd_img_l = gDisp_Ctrl.device_info.lcd_info.image_lines;
	// To decide the input format to the DISP.
	if( (gDisp_Ctrl.device_info.lcd_info.image_width == gDec_Param.image_width) &&
		(gDisp_Ctrl.device_info.lcd_info.image_lines == gDec_Param.image_lines) &&
		!User_IsDualStream() ){
		
		if( (gDisp_Ctrl.device_info.lcd_info.rotate == E_H264_IF_DISP_ROTATE_90)  ||
			(gDisp_Ctrl.device_info.lcd_info.rotate == E_H264_IF_DISP_ROTATE_180) ||
			(gDisp_Ctrl.device_info.lcd_info.rotate == E_H264_IF_DISP_ROTATE_270) ){
			ribery_output = FALSE;
		}
		else{
			ribery_output = TRUE;
		}
	}
	else{
		ribery_output = FALSE;
	}
	
	/* Set Event for Task Start */
	OS_User_Set_Flg( FID_H264, FLG_H264_DISP_LCD_INIT_END );
	
	if (User_IsDualStream() && User_IsH265MainStream()) {
		update = FALSE;
	} else {
		update = TRUE;
	}
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "LCD" ));
	
	while(1){
		
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "LCD" ));
		
		H264_Movie_Lcd_Wait();
		
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "LCD" ));
		
		if( stop_req_flag == TRUE ){
			break;
		}
		
		// Disp task stop detect.
		ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_STOP, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_IDX_SET );
			OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_UPDATE );
			// update the last image before the end.
			stop_req_flag = TRUE;
			
			// If last_picture_keep is FALSE, it does black back setting.
			if( gDisp_Ctrl.last_picture_keep == FALSE ){
				FJ_Disp_Video_Disable( gDisp_Ctrl.device_info.lcd_info.disp_id );
			}
		}
		
		// PAUSE operation detect.
		ercd = OS_User_Pol_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_PAUSE_REQUEST, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			if( pause_complete_flag == FALSE ){
				// Set the TRUE to pause_complete_flag, and perform the image update.
				pause_complete_flag = TRUE;
			}
			else{
				OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_LCD_PAUSE );
				first_display_flag = TRUE;
				continue;
			}
		}
		else{
			OS_User_Clr_Flg( FID_H264_DECODER, ~FLG_H264_DECODER_DISP_LCD_PAUSE );
			pause_complete_flag = FALSE;
		}
		
		ercd = OS_User_Twai_Sem(SID_H264, H264_DEC_SEMAPHORE_TIMEOUT);
		if( ercd != D_OS_USER_E_OK ){
			H264_Err_Print(( "--- H264_Decode_DISP_LCD_Main : gDisp_Ctrl semaphore NG\n" ));
		}
		
		if( gDisp_Ctrl.display_index == 0xFF ){
			// Still there is no image to be displayed.
			OS_User_Sig_Sem(SID_H264);
			continue;
		}
		
		lcd_displaying_index	= lcd_decide_index;
		lcd_decide_index		= gDisp_Ctrl.display_index;
		
		gDisp_Ctrl.refer_index.lcd_decide		= lcd_decide_index;
		gDisp_Ctrl.refer_index.lcd_displaying	= lcd_displaying_index;
		
		if( pause_complete_flag == TRUE ){
			if( lcd_decide_index != lcd_displaying_index ){
				pause_complete_flag = FALSE;
			}
		}
		
		OS_User_Sig_Sem(SID_H264);
		
		if( (lcd_decide_index == lcd_displaying_index) && (first_display_flag == FALSE) ){
			// No need resize image.
			continue;
		}
		/* Get Decode Area Address */
		Sdram_Map_Movie_Play_H264_Get_Vdec( lcd_decide_index, &dec_addr.y_address, &dec_addr.c_address );

		if (User_IsStreamingProcess() && User_IsDualStream() &&	User_IsH265MainStream() && !H265_IsLastPTSDisplay_Or_Error()) {
			ercd = OS_User_Wai_Flg(FID_H265_DECODER, FLG_H265_DECODER_DISP_UPDATE, D_OS_USER_TWF_ORW, &flgptn);
			if( ercd != D_OS_USER_E_OK ) H264_Err_Print(( "Disp_Main : OS_User_Wai_Flg [FLG_H265_DECODER_DISP_UPDATE] NG\n" ));
			ercd = OS_User_Clr_Flg(FID_H265_DECODER, ~FLG_H265_DECODER_DISP_UPDATE);
			if( ercd != D_OS_USER_E_OK ) H264_Err_Print(( "Disp_Main : OS_User_Clr_Flg [FLG_H265_DECODER_DISP_UPDATE] NG\n" ));
		}

		if (User_IsStreamingProcess() && User_IsDualStream() &&	User_IsH265MainStream() &&
			H265_IsLastPTSDisplay_Or_Error() && (H265_GetLastPTS() != gDisp_Ctrl.inf[lcd_decide_index].PTS)) {
			update = TRUE;
			gDisp_Ctrl.device_info.lcd_info.image_width = lcd_img_w << 1;
			gDisp_Ctrl.device_info.lcd_info.image_lines = lcd_img_l << 1;
		}

		if (User_IsStreamingProcess() && User_IsDualStream() &&	H265_IsLastPTSDisplay_Or_Error() &&
			(H265_GetLastPTS() != gDisp_Ctrl.inf[lcd_decide_index].PTS)) {
			if( (gDisp_Ctrl.device_info.lcd_info.image_width == gDec_Param.image_width) &&
				(gDisp_Ctrl.device_info.lcd_info.image_lines == gDec_Param.image_lines) ){

				if( (gDisp_Ctrl.device_info.lcd_info.rotate == E_H264_IF_DISP_ROTATE_90)  ||
					(gDisp_Ctrl.device_info.lcd_info.rotate == E_H264_IF_DISP_ROTATE_180) ||
					(gDisp_Ctrl.device_info.lcd_info.rotate == E_H264_IF_DISP_ROTATE_270) ){
					ribery_output = FALSE;
				}else{
					gDisp_Ctrl.device_info.lcd_info.global_width = gDec_Param.image_bank_y_width;
					gDisp_Ctrl.device_info.lcd_info.global_lines = gDec_Param.image_bank_y_lines;
					ribery_output = TRUE;
				}
			}else{
				ribery_output = FALSE;
			}
		}

		/* Get Display Area address */
		H264_If_Play_Notify_LCD_UPDATE( &disp_addr.y_address, &disp_addr.c_address, update );

#if 0	// for Debug
		OS_User_Get_Tim(&time_before);
#endif	// for Debug

		h264_decode_display_image( (T_H264_IF_DISPLAY_PARAM*)&gDisp_Ctrl.device_info.lcd_info, &dec_addr, &disp_addr, ribery_output );
		
#if 0	// for Debug
		OS_User_Get_Tim(&time_after);
		H264_Info_Print(("resize_time: %d\n", (UINT32)(time_after - time_before) ));
#endif	// for Debug
		
// CO_DEBUG_RS_PLAY_MEDIA_OFF
		// [board check] ICE break and dump. Movie Play
// CO_DEBUG_RS_PLAY_MEDIA_OFF
		
		if (User_IsStreamingProcess() && User_IsDualStream() &&	!User_IsH265MainStream()  && !H265_IsLastPTSDisplay_Or_Error()) {
		} else {
			/* Set Image Data */
			H264_Movie_Lcd_Update( (T_H264_IF_DISPLAY_PARAM*)&gDisp_Ctrl.device_info.lcd_info, &disp_addr, ribery_output );
		}

		OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_UPDATE );
		OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_FRAME_DONE );

		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "DDisp:%lu, addr:%x", (ULONG)gDisp_Ctrl.inf[lcd_decide_index].PTS, disp_addr.y_address ));
		
		if( gDisp_Ctrl.inf[lcd_decide_index].PTS == gDec_Ctrl.last_display_pts ){
			decoder_status = E_H264_IF_PLAY_DECODER_STATUS_LAST;
			gDisp_Ctrl.last_pts_display = TRUE;
		}
		else{
			decoder_status = E_H264_IF_PLAY_DECODER_STATUS_NORMAL;
		}
		
		if( first_display_flag == TRUE ){
			// first display
			Host_H264_Notify_FirstFrameDisplayCB( decoder_status, gDisp_Ctrl.inf[lcd_decide_index].PTS, gDec_Ctrl.error );
			first_display_flag = FALSE;
			OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_START );
		}
		else{
			// normal display
			Host_H264_Notify_VideoDecodeCB( decoder_status, gDisp_Ctrl.inf[lcd_decide_index].PTS, gDec_Ctrl.error );
		}
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "LCD" ));
	
	/* Set Task End */
	OS_User_Set_Flg( FID_H264, FLG_H264_DISP_LCD_STOP );
	
	return;
}

/**
 *  @brief		Movie Decode Display Hdmi Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Decode_DISP_HDMI_Main( VOID )
{
	OS_USER_ER						ercd;
	OS_USER_FLGPTN					flgptn;
	BOOL							first_display_flag		= TRUE;
	UINT8							hdmi_decide_index		= 0xFF;
	UINT8							hdmi_displaying_index	= 0xFF;
	T_H264_MOVIE_ADDR				dec_addr;
	T_H264_MOVIE_ADDR				disp_addr;
	BOOL							stop_req_flag			= FALSE;
	BOOL							pause_complete_flag		= FALSE;
	BOOL							ribery_output;
	E_H264_IF_PLAY_DECODER_STATUS	decoder_status;
	BOOL							update = TRUE;
	USHORT							hdmi_img_w;
	USHORT							hdmi_img_l;
	
#if 0	// for Debug
	OS_USER_SYSTIM					time_before, time_after;
#endif	// for Debug
	
	hdmi_img_w = gDisp_Ctrl.device_info.hdmi_info.image_width;
	hdmi_img_l = gDisp_Ctrl.device_info.hdmi_info.image_lines;
	// To decide the input format to the DISP.
	if( (gDisp_Ctrl.device_info.hdmi_info.image_width == gDec_Param.image_width) &&
		(gDisp_Ctrl.device_info.hdmi_info.image_lines == gDec_Param.image_lines) &&
		 !User_IsDualStream() ){
		
		if( (gDisp_Ctrl.device_info.hdmi_info.rotate == E_H264_IF_DISP_ROTATE_90)  ||
			(gDisp_Ctrl.device_info.hdmi_info.rotate == E_H264_IF_DISP_ROTATE_180) ||
			(gDisp_Ctrl.device_info.hdmi_info.rotate == E_H264_IF_DISP_ROTATE_270) ){
			ribery_output = FALSE;
		}
		else{
			ribery_output = TRUE;
		}
	}
	else{
		ribery_output = FALSE;
	}
	
	/* Set Event for Task Start */
	OS_User_Set_Flg( FID_H264, FLG_H264_DISP_HDMI_INIT_END );

	if (User_IsDualStream() && User_IsH265MainStream()) {
		update = FALSE;
	} else {
		update = TRUE;
	}
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "HDMI" ));
	
	while(1){
		
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "HDMI" ));
		
		H264_Movie_Hdmi_Wait();
		//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "DD S"));
		
		if( stop_req_flag == TRUE ){
			break;
		}
		
		// Disp task stop detect.
		ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_STOP, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_IDX_SET );
			OS_User_Set_Flg( FID_H265_DECODER, FLG_H265_DECODER_DISP_UPDATE );
			// update the last image before the end.
			stop_req_flag = TRUE;
			
			// If last_picture_keep is FALSE, it does black back setting.
			if( gDisp_Ctrl.last_picture_keep == FALSE ){
				FJ_Disp_Video_Disable( gDisp_Ctrl.device_info.hdmi_info.disp_id );
			}
		}
		
		// PAUSE operation detect.
		ercd = OS_User_Pol_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_PAUSE_REQUEST, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			if( pause_complete_flag == FALSE ){
				// Set the TRUE to pause_complete_flag, and perform the image update.
				pause_complete_flag = TRUE;
			}
			else{
				OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_HDMI_PAUSE );
				first_display_flag = TRUE;
				continue;
			}
		}
		else{
			OS_User_Clr_Flg( FID_H264_DECODER, ~FLG_H264_DECODER_DISP_HDMI_PAUSE );
			pause_complete_flag = FALSE;
		}
		
		ercd = OS_User_Twai_Sem(SID_H264, H264_DEC_SEMAPHORE_TIMEOUT);
		if( ercd != D_OS_USER_E_OK ){
			H264_Err_Print(( "--- H264_Decode_DISP_HDMI_Main : gDisp_Ctrl semaphore NG\n" ));
		}
		
		if( gDisp_Ctrl.display_index == 0xFF ){
			// Still there is no image to be displayed.
			OS_User_Sig_Sem(SID_H264);
			continue;
		}
		
		hdmi_displaying_index	= hdmi_decide_index;
		hdmi_decide_index		= gDisp_Ctrl.display_index;
		
		gDisp_Ctrl.refer_index.hdmi_decide		= hdmi_decide_index;
		gDisp_Ctrl.refer_index.hdmi_displaying	= hdmi_displaying_index;
		
		if( pause_complete_flag == TRUE ){
			if( hdmi_decide_index != hdmi_displaying_index ){
				pause_complete_flag = FALSE;
			}
		}
		
		OS_User_Sig_Sem(SID_H264);
		
		if( (hdmi_decide_index == hdmi_displaying_index) && (first_display_flag == FALSE) ){
			// No need resize image.
			continue;
		}
		/* Get Decode Area Address */
		Sdram_Map_Movie_Play_H264_Get_Vdec( hdmi_decide_index, &dec_addr.y_address, &dec_addr.c_address );

		if (User_IsStreamingProcess() && User_IsDualStream() &&	User_IsH265MainStream() && !H265_IsLastPTSDisplay_Or_Error()) {
			//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "DD WS"));
			ercd = OS_User_Wai_Flg(FID_H265_DECODER, FLG_H265_DECODER_DISP_UPDATE, D_OS_USER_TWF_ORW, &flgptn);
			if( ercd != D_OS_USER_E_OK ) H264_Err_Print(( "Disp_Main : OS_User_Wai_Flg [FLG_H265_DECODER_DISP_UPDATE] NG\n" ));
			ercd = OS_User_Clr_Flg(FID_H265_DECODER, ~FLG_H265_DECODER_DISP_UPDATE);
			if( ercd != D_OS_USER_E_OK ) H264_Err_Print(( "Disp_Main : OS_User_Clr_Flg [FLG_H265_DECODER_DISP_UPDATE] NG\n" ));
			//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "DD WE"));
		}

		if (User_IsStreamingProcess() && User_IsDualStream() &&	User_IsH265MainStream() &&
			H265_IsLastPTSDisplay_Or_Error() && (H265_GetLastPTS() != gDisp_Ctrl.inf[hdmi_decide_index].PTS)) {
			update = TRUE;
			gDisp_Ctrl.device_info.hdmi_info.image_width = hdmi_img_w << 1;
			gDisp_Ctrl.device_info.hdmi_info.image_lines = hdmi_img_l << 1;
		}

		if (User_IsStreamingProcess() && User_IsDualStream() &&	H265_IsLastPTSDisplay_Or_Error() &&
			(H265_GetLastPTS() != gDisp_Ctrl.inf[hdmi_decide_index].PTS)) {
			if( (gDisp_Ctrl.device_info.hdmi_info.image_width == gDec_Param.image_width) &&
				(gDisp_Ctrl.device_info.hdmi_info.image_lines == gDec_Param.image_lines) ){

				if( (gDisp_Ctrl.device_info.hdmi_info.rotate == E_H264_IF_DISP_ROTATE_90)  ||
					(gDisp_Ctrl.device_info.hdmi_info.rotate == E_H264_IF_DISP_ROTATE_180) ||
					(gDisp_Ctrl.device_info.hdmi_info.rotate == E_H264_IF_DISP_ROTATE_270) ){
					ribery_output = FALSE;
				}else{
					gDisp_Ctrl.device_info.hdmi_info.global_width = gDec_Param.image_bank_y_width;
					gDisp_Ctrl.device_info.hdmi_info.global_lines = gDec_Param.image_bank_y_lines;
					ribery_output = TRUE;
				}
			}else{
				ribery_output = FALSE;
			}
		}

		/* Get Display Area address */
		H264_If_Play_Notify_HDMI_UPDATE( &disp_addr.y_address, &disp_addr.c_address, update );

#if 0	// for Debug
		OS_User_Get_Tim(&time_before);
#endif	// for Debug

		//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "DD C"));
		h264_decode_display_image( (T_H264_IF_DISPLAY_PARAM*)&gDisp_Ctrl.device_info.hdmi_info, &dec_addr, &disp_addr, ribery_output );
		//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "DD C"));
#if 0	// for Debug
		OS_User_Get_Tim(&time_after);
		H264_Info_Print(("resize_time: %d\n", (UINT32)(time_after - time_before) ));
#endif	// for Debug
		
		if (User_IsStreamingProcess() && User_IsDualStream() &&	!User_IsH265MainStream()  && !H265_IsLastPTSDisplay_Or_Error()) {
			Sdram_Map_Play_Free_HDMI_LockIdx();
		} else {
			/* Set Image Data */
			H264_Movie_Hdmi_Update( (T_H264_IF_DISPLAY_PARAM*)&gDisp_Ctrl.device_info.hdmi_info, &disp_addr, ribery_output );
		}

		OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_UPDATE );
		OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_FRAME_DONE );

		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "DDisp:%lu, addr:%x", (ULONG)gDisp_Ctrl.inf[hdmi_decide_index].PTS, disp_addr.y_address ));

		if( gDisp_Ctrl.device_info.lcd_info.valid_flag == FALSE ){
			if( gDisp_Ctrl.inf[hdmi_decide_index].PTS == gDec_Ctrl.last_display_pts ){
				decoder_status = E_H264_IF_PLAY_DECODER_STATUS_LAST;
				gDisp_Ctrl.last_pts_display = TRUE;
			}
			else{
				decoder_status = E_H264_IF_PLAY_DECODER_STATUS_NORMAL;
			}
			
			if( first_display_flag == TRUE ){
				// first display (If there is no LCD.)
				Host_H264_Notify_FirstFrameDisplayCB( decoder_status, gDisp_Ctrl.inf[hdmi_decide_index].PTS, gDec_Ctrl.error );
				first_display_flag = FALSE;
				OS_User_Set_Flg( FID_H264_DECODER, FLG_H264_DECODER_DISP_START );
			}
			else{
				// normal display (If there is no LCD.)
				Host_H264_Notify_VideoDecodeCB( decoder_status, gDisp_Ctrl.inf[hdmi_decide_index].PTS, gDec_Ctrl.error );
			}
		}
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "HDMI" ));
	
	/* Set Task End */
	OS_User_Set_Flg( FID_H264, FLG_H264_DISP_HDMI_STOP );
	
	return;
}


/**
 *  @brief		Decode Main Function.\n
 *	@param		file_id			i:File Identifier
 *	@param		play_index		i:Table Index
 *	@return		H264_OK					(0)Normal End
 *	@return		H264_ERR_INPUT_PARAM		(1)Input Parameter Error
 *	@return		H264_ERR_FILE_ACCESS		(8)File Access Error
 *	@return		H264_ERR_FILE_NO_SUPPORT	(9)Not Support File Error
 *	@return		H264_ERR_SYSTEM			(99)System Fault
 *	@note		None
 *	@attention	None
 */
INT32 H264_Decode_Main( VOID )
{

	INT32				ret;
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn;
	OS_USER_ER			ercd;
	UINT32				wait_count;
	UCHAR				main_skip_flg;
	
/* --- 3. Initialize/Change Ribery and Set condition --- */
	ret = h264_decode_init_param();
	if( ret != H264_OK ){
		H264_If_Play_Notify_STOP_COMPLETE();
		return ret;
	}
	
/* --- 7. Initialize Audio Process and Execute some tasks --- */
	/* ENTropy Task */
	h264_decode_task_start( TID_H264_ENT );
	/* CodecCore Task */
	h264_decode_task_start( TID_H264_CC );
	/* DISP Task */
	h264_decode_task_start( TID_H264_DISP );
	
	/* Confirm All of Task Status */
	waiptn = FLG_H264_ENT_INIT_END | FLG_H264_CC_INIT_END | FLG_H264_DISP_INIT_END;
	
	wait_count = 0;
	
	/* Wait Task Start Status */
	while(1){
		/* Check Task Status */
		ercd = OS_User_Pol_Flg( FID_H264, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			break;
		}
		else if( ercd == D_OS_USER_E_TMOUT ) {
			if( wait_count > 300 ){
				H264_Err_Print(("--- Main: It cannot change task status!! waiptn=%lx, flgptn=%lx\n", waiptn, flgptn ));
				h264_decode_task_forced_stop();
				H264_If_Play_Notify_STOP_COMPLETE();
				return H264_ERR_SYSTEM;
			}
		}
		else{	// error
			H264_Err_Print(("--- Main: Wait task start. OS_User_Pol_Flg ercd=%x\n", ercd ));
			h264_decode_task_forced_stop();
			H264_If_Play_Notify_STOP_COMPLETE();
			return H264_ERR_SYSTEM;
		}
		OS_User_Dly_Tsk(10);
		wait_count++;
	}
	
	/* Check Task Result Status before Recording */
	waiptn = FLG_H264_ENT_STOP | FLG_H264_CC_STOP | FLG_H264_DISP_STOP;
	
	ercd = OS_User_Pol_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	if( ercd == D_OS_USER_E_OK ) {
		H264_Err_Print(("--- Main: It found MOVIE Task End before Play Start. flgptn=%lx\n", flgptn ));
		OS_User_Set_Flg( FID_H264, FLG_H264_STOP );
		H264_If_Play_Notify_STOP_COMPLETE();
		return H264_ERR_SYSTEM;
	}
	
	H264_Info_Print(("\n"));
	H264_Info_Print(("+++ Main: MOVIE PLAY INIT ====\n"));
	
	/* It set "Initialization" to current status */
	H264_If_Play_Api_Set_Status( E_H264_IF_PLAY_STATUS_INIT );
	
	H264_If_Play_Notify_STATE_CHANGE_COMPLETE( H264_OK );
	
/* --- 9. Read Audio/Video Stream, and Check Event and File read end  --- */
	while(1){
		OS_User_Dly_Tsk(1);
		/* 8.1 Check Task Result */
		ercd = OS_User_Pol_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ){
			gDec_Ctrl.mode		= E_H264_PLAY_MODE_END;
			gDec_Ctrl.next_mode	= E_H264_PLAY_MODE_END;
			break;
		}
		else if( ercd == D_OS_USER_E_TMOUT ) {
			;
		}
		else{
			H264_Err_Print(("--- MP4: OS_User_Pol_Flg(8.1) ercd=%x\n", ercd ));
		}
		
		main_skip_flg = H264_OFF;
		
		if( gDec_Ctrl.next_mode == gDec_Ctrl.mode ){
			OS_User_Dly_Tsk(10);
			main_skip_flg = H264_ON;
		}
		
		/* According to Play Mode, it execute main part */
		if( main_skip_flg == H264_OFF ){
			gDec_Ctrl.mode = mp4dec_play_func[ gDec_Ctrl.mode ]();
		}
		
		/* Check User Request */
		if( gDec_Ctrl.mode != E_H264_PLAY_MODE_STOP ){
			if( gDec_Ctrl.mode != gDec_Ctrl.next_mode ){
				gDec_Ctrl.mode = gDec_Ctrl.next_mode;
			}
			else{
				gDec_Ctrl.next_mode = H264_Movie_Play_Event_Check( gDec_Ctrl.mode );
			}
		}
	}
	
	// dummy sync for top image display
	OS_User_Set_Flg( FID_H264_SYNC, FLG_H264_SYNC_DEC );
	
	waiptn = FLG_H264_ENT_STOP | FLG_H264_CC_STOP | FLG_H264_DISP_STOP;
/* --- 10. Wait Task End  --- */
	wait_count = 0;
	while(1){
		/* Check Task Status */
		ercd = OS_User_Pol_Flg( FID_H264, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			break;
		}
		else if( ercd == D_OS_USER_E_TMOUT ) {
			if( wait_count > 200 ){
				H264_Err_Print(("--- Main: Task End.\n"));
				h264_decode_task_forced_stop();
				break;
			}
		}
		else{
			H264_Err_Print(("--- MP4: Task End OS_User_Pol_Flg ercd=%x\n", ercd ));
			h264_decode_task_forced_stop();
			break;
		}
		OS_User_Dly_Tsk(10);
		wait_count++;
	}
	
/* --- 11. Stop Ribery Decoder	--- */
	H264_Ribery_Finalize( H264_MODE_DECODE );
	
/* --- 14. Report Decode End to U.I. */
	ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_FORCE_STOP, D_OS_USER_TWF_ANDW, &flgptn );
	if( ercd == D_OS_USER_E_OK ){
		// "H264_ERR_SYSTEM" because of "FORCE_STOP" it.
		gDec_Ctrl.task_result = H264_ERR_SYSTEM;
	}
	
	H264_If_Play_Notify_STOP_COMPLETE();
	
	H264_Info_Print(("+++ Main: MOVIE PLAY END!! ====\n"));
	
	return H264_OK;
}


/**
 *  @brief		Initialize Play Link Table and File Format Memory Table.\n
 *	@param		None
 *	@return		H264_OK						(0)Normal End
 *	@note		None
 *	@attention	None
 */
INT32 H264_Decode_Play_Start( ULLONG start_pts, E_H264_IF_PLAY_DIRECTION direction, E_H264_IF_PLAY_SPEED speed, BOOL pause )
{
	INT32	ret;
	UINT32	scan_mode;
	UINT32	timer_set_time_scale=0;
	UINT32	timer_set_time_delta=0;
	
	gDisp_Ctrl.display_target_pts	= start_pts;
	gDisp_Ctrl.display_pts			= start_pts;
	gDec_Ctrl.direction				= direction;
	gDec_Ctrl.speed					= speed;
	gDec_Ctrl.pause					= pause;
	
	switch( speed ){
		case E_H264_IF_PLAY_SPEED_1X:
			timer_set_time_scale	= gDec_Param.time_scale;
			timer_set_time_delta	= gDec_Param.time_delta;
			gDec_Ctrl.iframe_only	= FALSE;
			break;
		
		case E_H264_IF_PLAY_SPEED_2X:
			timer_set_time_scale	= D_H264_DEC_TIMER_FFREW_2X_TIME_SCALE;
			timer_set_time_delta	= D_H264_DEC_TIMER_FFREW_2X_TIME_DELTA;
			gDec_Ctrl.iframe_only	= TRUE;
			break;
		
		case E_H264_IF_PLAY_SPEED_4X:
			timer_set_time_scale	= D_H264_DEC_TIMER_FFREW_4X_TIME_SCALE;
			timer_set_time_delta	= D_H264_DEC_TIMER_FFREW_4X_TIME_DELTA;
			gDec_Ctrl.iframe_only	= TRUE;
			break;
		
		case E_H264_IF_PLAY_SPEED_8X:
			timer_set_time_scale	= D_H264_DEC_TIMER_FFREW_8X_TIME_SCALE;
			timer_set_time_delta	= D_H264_DEC_TIMER_FFREW_8X_TIME_DELTA;
			gDec_Ctrl.iframe_only	= TRUE;
			break;
		
		case E_H264_IF_PLAY_SPEED_1_2X:
			timer_set_time_scale	= gDec_Param.time_scale;
			timer_set_time_delta	= gDec_Param.time_delta;
			gDec_Ctrl.iframe_only	= FALSE;
			break;
		
		case E_H264_IF_PLAY_SPEED_1_4X:
			timer_set_time_scale	= gDec_Param.time_scale;
			timer_set_time_delta	= gDec_Param.time_delta;
			gDec_Ctrl.iframe_only	= FALSE;
			break;
		
		case E_H264_IF_PLAY_SPEED_1_8X:
			timer_set_time_scale	= gDec_Param.time_scale;
			timer_set_time_delta	= gDec_Param.time_delta;
			gDec_Ctrl.iframe_only	= FALSE;
			break;
		
		default :
			H264_Err_Print(("--- BF_Movie_Common_Timer_Start Error.\n"));
			break;
	}
	
	if( gDec_Param.scan_mode == E_H264_IF_SCAN_MODE_PROGRESSIVE ){
		scan_mode = D_BF_MOVIE_COMMON_SCAN_MODE_PROGRESSIVE;
	}
	else{
		scan_mode = D_BF_MOVIE_COMMON_SCAN_MODE_INTERLACE;
	}
	/* Decide Reload Timer */
	BF_Movie_Common_Timer_Calculate( timer_set_time_delta, timer_set_time_scale, scan_mode );
	
	/* Reload Timer Enable */
	ret = BF_Movie_Common_Timer_Start( E_BF_MOVIE_COMMON_RTIMER_MODE_DEC );

	if( ret != H264_OK ){
		H264_Err_Print(("--- BF_Movie_Common_Timer_Start Error.\n"));
		// Dont stop timer since dual decoding H264/H265 open timer twice
		/* Reload Timer Stop */
		//BF_Movie_Common_Timer_Stop( E_BF_MOVIE_COMMON_RTIMER_MODE_DEC );
		//return H264_NG;
	}

	/* Send Start Event */
	OS_User_Set_Flg( FID_H264, FLG_H264_START );
	
	return H264_OK;
}




/**
 *  @brief		Initialize Play Link Table and File Format Memory Table.\n
 *	@param		None
 *	@return		H264_OK						(0)Normal End
 *	@note		None
 *	@attention	None
 */
INT32 H264_Decode_Play_Initialize( VOID )
{
	return H264_OK;
}

/**
 *  @brief		Release Play Link Table.\n
 *	@param		None
 *	@return		H264_OK						(0)Normal End
 *	@note		None
 *	@attention	None
 */
INT32 H264_Decode_Play_Release( VOID )
{
	return H264_OK;
}


/**
 *  @brief		Delete Play Link Table.\n
 *	@param		i:play_index		sysmode_play Link Table Index
 *	@return		H264_OK						(0)Normal End
 *	@return		H264_ERR_INPUT_PARAM			(1)Input Parameter Error
 *	@note		None
 *	@attention	None
 */
INT32 H264_Decode_Play_Delete( UCHAR play_index )
{
	return H264_OK;
}


/**
 *  @brief		Delete Play Link Table.\n
 *	@param		i:play_info			Play Informaton
 *	@param		i:play_index		sysmode_play Link Table Index
 *	@return		H264_OK						(0)Normal End
 *	@return		H264_ERR_INPUT_PARAM			(1)Input Parameter Error
 *	@note		None
 *	@attention	None
 */
INT32 H264_Decode_Top_Init( T_H264_IF_DECODER_PARAM* decoder_param, T_H264_IF_DISPLAY_INFO* display_info )
{
	if( decoder_param == NULL){
		return H264_ERR_INPUT_PARAM;
	}
	if( display_info == NULL){
		return H264_ERR_INPUT_PARAM;
	}
	h264_decode_initialize( decoder_param, display_info );
	
	return 	H264_OK;
}


/**
 *  @brief		Top Image Display Main Processing.\n
 *	@param		i:play_info			Play Informaton
 *	@param		i:disp_size			Display size
 *	@param		i:va_adr_y			VRAM Y Address of Ribery Format
 *	@param		i:play_index		sysmode_play Link Table Index(Max.8)
 *	@param		i:dec_flag			decode execution flag\n
 *									H264_ON  : Decode Execution     -> (mov/mp4 free_atom only) info get & YCC420 get
 *									H264_OFF : Decode Not Execution -> (mov/mp4 free_atom only) spread atom 
 *	@return		H264_OK					(0)Normal End
 *	@return		H264_ERR_INPUT_PARAM		(1)Input Parameter Error
 *	@return		H264_ERR_FILE_ACCESS		(8)File Access Error
 *	@return		H264_ERR_FILE_NO_SUPPORT	(9)Not Support File Error
 *	@note		None
 *	@attention	None
 */
INT32 H264_Decode_Top_Main( ULLONG req_pts, T_H264_IF_VIDEO_YUV_INFO* decode_yuv_info )
{
	INT32 ret;
	
	ret = h264_decode_top_image_get( req_pts, decode_yuv_info );
	if( ret != H264_OK ){
		H264_Err_Print(("--- TOP: h264_decode_top_image_get Error=%d\n", ret ));
		return ret;
	}
	
	return 	H264_OK;
}

INT32 H264_Decode_Video_Stream_Set( UINT32 store_num, T_H264_IF_PARAM_SET_STREAM* video_stream_param )
{
	UINT32	next_write_pointer;
	UINT32	free_pointer;
	UINT32	stream_loop;
	
	// free area check
	next_write_pointer	= gDmux_Ctrl.write_pointer;
	free_pointer		= gDmux_Ctrl.free_pointer;
	
	for( stream_loop=0; stream_loop<store_num; stream_loop++){
		next_write_pointer++;
		
		if( next_write_pointer >= D_H264_DEC_SAVE_SAMPLE_MAX_NUM ){
			next_write_pointer = 0;
		}
		if( next_write_pointer == free_pointer ){
			// When passing through this route, there is a problem in the control of Stream request.
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
//			H264_Info_Print(("--- H264_Decode_Video_Stream_Set: The number of settings beyond the Stream management area. pts=%lu\n", (ULONG)video_stream_param[stream_loop].pts ));
#else
//			H264_Info_Print(("--- H264_Decode_Video_Stream_Set: The number of settings beyond the Stream management area. pts=%llu\n", video_stream_param[stream_loop].pts ));
#endif
			return H264_NG;
		}
	}
	
	next_write_pointer	= gDmux_Ctrl.write_pointer;
	free_pointer		= gDmux_Ctrl.free_pointer;
	
	for( stream_loop=0; stream_loop<store_num; stream_loop++){
		
		next_write_pointer++;
		
		if( next_write_pointer >= D_H264_DEC_SAVE_SAMPLE_MAX_NUM ){
			next_write_pointer = 0;
		}
		
		// set gDmux_Ctrl
		gDmux_Ctrl.decode_video[next_write_pointer].stream_end_flag	= video_stream_param[stream_loop].stream_end_flag;
		if( gDmux_Ctrl.decode_video[next_write_pointer].stream_end_flag == 1 ){
			gDmux_Ctrl.stream_end = TRUE;
		}
		gDmux_Ctrl.decode_video[next_write_pointer].stss			= video_stream_param[stream_loop].stss;
		gDmux_Ctrl.decode_video[next_write_pointer].stream_size		= video_stream_param[stream_loop].stream_size;
		gDmux_Ctrl.decode_video[next_write_pointer].stream_address	= video_stream_param[stream_loop].stream_address;
		gDmux_Ctrl.decode_video[next_write_pointer].pts				= video_stream_param[stream_loop].pts;
		gDmux_Ctrl.decode_video[next_write_pointer].dts				= video_stream_param[stream_loop].dts;
		gDmux_Ctrl.decode_video[next_write_pointer].vcpb_index		= 0xFFFFFFFF;
		
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "FeedRcv:%lu", (ULONG)gDmux_Ctrl.decode_video[next_write_pointer].pts ));
#else
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "FeedRcv:%llu", gDmux_Ctrl.decode_video[next_write_pointer].pts ));
#endif
	}
	
	// write_pointer update
	gDmux_Ctrl.write_pointer = next_write_pointer;
	OS_User_Set_Flg( FID_H264, FLG_H264_DEC_STREAM_RECEIVE );
	
	return H264_OK;
}


INT32 H264_Decode_Video_Display_Frame_Get( T_H264_IF_VIDEO_YUV_INFO* display_yuv_info )
{
	if( gDisp_Ctrl.display_index == 0xFF ){
		H264_Err_Print(("--- H264_Decode_Video_Display_Frame_Get: display_index is abnormal.\n" ));
		return H264_NG;
	}
	
	if( gDec_Ctrl.mode != E_H264_PLAY_MODE_PAUSE ){
		H264_Err_Print(("--- H264_Decode_Video_Display_Frame_Get: Video state is not a Pause.\n" ));
		return H264_NG;
	}
	
	
	Sdram_Map_Movie_Play_H264_Get_Vdec( gDisp_Ctrl.display_index, &display_yuv_info->y_addr, &display_yuv_info->cbcr_addr );
	
	display_yuv_info->g_width		= gDec_Ctrl.bank_y_width;
	display_yuv_info->g_lines		= gDec_Ctrl.bank_y_lines;
	display_yuv_info->width			= gDec_Param.image_width;
	display_yuv_info->lines			= gDec_Param.image_lines;
	display_yuv_info->yuv_format	= E_H264_IF_YUV_FORMAT_VIDEO;
	
	return H264_OK;
}



VOID H264_Decode_Stop_Request( BOOL last_picture_keep )
{
	gDisp_Ctrl.last_picture_keep = last_picture_keep;
	OS_User_Set_Flg( FID_H264, FLG_H264_STOP );
	OS_User_Set_Flg( FID_H264_SYNC, FLG_H264_SYNC_DEC );	// Put the dummy sync in order to stop processing the DISP task.
	
	return;
}

VOID H264_Decode_Flush_Request( VOID )
{
	INT32						ret;
	UCHAR						disp_bank = 0xFF;
	ULONG						y_ch;
	ULONG						c_ch;
	ULONG						y_adr;
	ULONG						c_adr;
	T_H264_RIBERY_DEC_PARAM		param;
	T_H264_RIBERY_BRIDGE_YC		bridge_yc;
	
	OS_User_Clr_Flg( FID_H264, ~FLG_H264_START );
	
	// Flush execute
	h264_decode_flush_execute( &disp_bank );
	
	if(disp_bank == 0xFF){
		H264_Err_Print(("+++ Not Found Disp_bank.\n"));
	}
	
	h264_decode_initialize_gdmux_control();
	
	// y_ch / y_adr
	if(      H264_Movie_Sdram_Ch_Check( SDRAM_ADR_MOVIE_PLAY_YCC ) == SDRAM_ADR_CH0_TOP ){ y_ch = RB_SDRAM_CH0; }
	else{																					 y_ch = RB_SDRAM_CH1; }
	y_adr		= SDRAM_ADR_MOVIE_PLAY_YCC;
	
	// c_ch / c_adr
	if(      H264_Movie_Sdram_Ch_Check( SDRAM_ADR_MOVIE_PLAY_YCC ) == SDRAM_ADR_CH0_TOP ){ c_ch = RB_SDRAM_CH0; }
	else{																					 c_ch = RB_SDRAM_CH1; }
	c_adr		= SDRAM_ADR_MOVIE_PLAY_YCC;
	
	// init param set
	param.size								= gDec_Param.image_size;
	param.frame_rate						= gDec_Param.frame_rate;
	param.trick								= H264_ON;
	param.keep_idx							= disp_bank;
	param.bank_num							= gDisp_Ctrl.vdpb_use_num;
	param.scan_mode							= gDec_Param.scan_mode;
	param.self_rec_play						= gDec_Param.self_rec;
	
	// bridge yc set
	bridge_yc.tb_flag						= H264_OFF;
	
	bridge_yc.map_yc_size.width				= gDec_Ctrl.bank_y_width;
	bridge_yc.map_yc_size.y_bank			= gDec_Ctrl.bank_y_size;
	bridge_yc.map_yc_size.c_bank			= gDec_Ctrl.bank_c_size;
	bridge_yc.map_yc_size.localdec_width	= 0;
	bridge_yc.map_yc_size.localdec_y_bank	= 0;
	bridge_yc.map_yc_size.localdec_c_bank	= 0;
	bridge_yc.map_yc_size.reduction_width	= 0;
	bridge_yc.map_yc_size.reduction_y_bank	= 0;
	bridge_yc.map_yc_size.reduction_c_bank	= 0;
	
	bridge_yc.map_yc_ch.y_ch				= y_ch;
	bridge_yc.map_yc_ch.c_ch				= c_ch;
	bridge_yc.map_yc_ch.localdec_y_ch		= 0;
	bridge_yc.map_yc_ch.localdec_c_ch		= 0;
	bridge_yc.map_yc_ch.reduction_y_ch		= 0;
	bridge_yc.map_yc_ch.reduction_c_ch		= 0;
	
	bridge_yc.map_yc_addr.y_addr			= y_adr;
	bridge_yc.map_yc_addr.c_addr			= c_adr;
	bridge_yc.map_yc_addr.localdec_y_addr	= 0;
	bridge_yc.map_yc_addr.localdec_c_addr	= 0;
	bridge_yc.map_yc_addr.reduction_y_addr	= 0;
	bridge_yc.map_yc_addr.reduction_c_addr	= 0;
	
	// init param & start
	H264_Ribery_Dec_InitParam_Set( &param, &bridge_yc );
	
	ret = H264_Ribery_Dec_Start();
	if( ret != H264_OK ){
		H264_Err_Print(("H264_Ribery_Dec_Start() NG.\n"));
	}
	
	// FLUSH operation release.
	OS_User_Clr_Flg( FID_H264, ~FLG_H264_FLUSH_OPS );
	
	H264_If_Play_Notify_STATE_CHANGE_COMPLETE( H264_OK );
	
	return;
}

BOOL H264_IsLastPTSSet_Or_Error(void)
{
	if (gDec_Ctrl.last_display_pts || gDec_Ctrl.error) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL H264_IsLastPTSDisplay_Or_Error(void)
{
	if (gDisp_Ctrl.last_pts_display || gDec_Ctrl.error) {
		return TRUE;
	} else {
		return FALSE;
	}
}

ULLONG H264_GetLastPTS(void)
{
	return gDec_Ctrl.last_display_pts;
}

/**
 *	@brief		Flush execute.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */

static VOID h264_decode_flush_execute( UCHAR* disp_bank )
{
	OS_USER_FLGPTN	flgptn;
	OS_USER_FLGPTN	waiptn = 0;
	UINT32			table_count;
	
	OS_User_Clr_Flg( FID_H264, ~FLG_H264_DISP_STATE_CHANGE );
	OS_User_Clr_Flg( FID_H264, ~FLG_H264_ENT_STATE_CHANGE );
	OS_User_Clr_Flg( FID_H264, ~FLG_H264_CC_STATE_CHANGE );
	
	// set FLUSH operation
	OS_User_Set_Flg( FID_H264, FLG_H264_FLUSH_OPS );
	
	// wait for DISP/ENT state change.
	waiptn = (FLG_H264_DISP_STATE_CHANGE | FLG_H264_ENT_STATE_CHANGE);
	
	OS_User_Wai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
	
	/* Reload Timer Stop */
	BF_Movie_Common_Timer_Stop( E_BF_MOVIE_COMMON_RTIMER_MODE_DEC );
	
	// FLUSH request
	H264_Ribery_Int_Request( E_H264_RIBERY_IRQ_REQUEST_DEC_STOP );
	
	// wait for CC state change.
	OS_User_Wai_Flg( FID_H264, FLG_H264_CC_STATE_CHANGE, D_OS_USER_TWF_ORW, &flgptn );
	
	// fifo,info init
	H264_Ribery_Dec_Vtable_Init();
	
	// Disp_Ctrl init
	for( table_count=0; table_count<gDisp_Ctrl.vdpb_use_num; table_count++ ){
		if( gDisp_Ctrl.inf[table_count].use_flg == TRUE ){
			if( table_count == gDisp_Ctrl.display_index ){
				// display bank is not initialize.
				*disp_bank = table_count;
			}
			else{
				gDisp_Ctrl.inf[table_count].use_flg = FALSE;
			}
		}
	}
	
// for Debug
#if 0
	// Check refer index
	if( (gDisp_Ctrl.refer_index.lcd_decide != 0xFF) &&
		(gDisp_Ctrl.refer_index.lcd_decide != gDisp_Ctrl.display_index) ){
		H264_Err_Print(("lcd_decide index is abnormal!!\n"));
	}
	if( (gDisp_Ctrl.refer_index.lcd_displaying != 0xFF) &&
		(gDisp_Ctrl.refer_index.lcd_displaying != gDisp_Ctrl.display_index) ){
		H264_Err_Print(("lcd_displaying index is abnormal!!\n"));
	}
	if( (gDisp_Ctrl.refer_index.hdmi_decide != 0xFF) &&
		(gDisp_Ctrl.refer_index.hdmi_decide != gDisp_Ctrl.display_index) ){
		H264_Err_Print(("hdmi_decide index is abnormal!!\n"));
	}
	if( (gDisp_Ctrl.refer_index.hdmi_displaying != 0xFF) &&
		(gDisp_Ctrl.refer_index.hdmi_displaying != gDisp_Ctrl.display_index) ){
		H264_Err_Print(("hdmi_displaying index is abnormal!!\n"));
	}
#endif
// for Debug
	
	gDec_Serial_Num					= 0;
	gDisp_Ctrl.use_count			= 1;
	gDisp_Ctrl.buf_count			= 1;
	gDec_Ctrl.last_display_pts		= D_H264_DEC_NOT_SET_PTS;
	gDec_Ctrl.last_stream_pts		= D_H264_DEC_NOT_SET_PTS;
	gDec_Ctrl.last_stream_dec_done	= FALSE;
	
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
static VOID h264_decode_task_start( UINT32 task_id )
{
	INT32	ercd;
	
	/* Create Task process */
	do{
		ercd = OS_User_Sta_Tsk( task_id, H264_MODE_PLAY );
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
 *	@return		H264_OK					(0) Normal End
 *	@note		None
 *	@attention	None
 */
static VOID h264_decode_initialize( T_H264_IF_DECODER_PARAM* decoder_param, T_H264_IF_DISPLAY_INFO* display_info )
{
	UINT32	total_bank_num;
	UINT32	frame_rate_multiply_100;
	
	/* Decode Condition (Default: Video/VGA 30fps High, Audio/48KHz,192bps)*/
	h264_decode_initialize_gdec_param( decoder_param );
	
	/* Decode Control Data */
	h264_decode_initialize_gdec_control();
	
	/* Initialize Demux Control Data */
	h264_decode_initialize_gdmux_control();
	
	/* Initialize Display Control Data */
	h264_decode_initialize_gdisp_control( display_info );
	
	// input image size setting
	gDec_Ctrl.bank_y_width	= decoder_param->image_bank_y_width;
	gDec_Ctrl.bank_y_lines	= decoder_param->image_bank_y_lines;
	gDec_Ctrl.bank_c_lines	= decoder_param->image_bank_c_lines;
	gDec_Ctrl.bank_y_size	= decoder_param->image_bank_y_size;
	gDec_Ctrl.bank_c_size	= decoder_param->image_bank_c_size;
	
	frame_rate_multiply_100 = (decoder_param->time_scale * 100) / decoder_param->time_delta;
	if( frame_rate_multiply_100 % 200 ){
		total_bank_num = (frame_rate_multiply_100 / 200) + 1;
	}
	else{
		total_bank_num = (frame_rate_multiply_100 / 200);
	}
	
	if( frame_rate_multiply_100 >= 9990 ){	// Special spec (over 100000/1001 fps). GOP limit less than 44. 20170721
		total_bank_num /= 2;
	}
	total_bank_num += D_H264_DEC_SPECIAL_TRICK_VDPB_COEFFICIENT;
	gDisp_Ctrl.vdpb_use_num = total_bank_num;
	
	if( total_bank_num < D_H264_DEC_VDPB_MIN_NUM ){
		total_bank_num			= D_H264_DEC_VDPB_MIN_NUM;
		gDisp_Ctrl.vdpb_use_num	= D_H264_DEC_VDPB_MIN_NUM;
	}
	
	if( decoder_param->trick == FALSE ){
		total_bank_num			= D_H264_RIBERY_VDPB_MIN_NUM;
	}
	
#if 0	// for Debug *Force 124 bank.*
	total_bank_num			= 124;
	gDisp_Ctrl.vdpb_use_num	= 124;
#endif	// for Debug  *Force 124 bank.*
	
	/* Initialize SDRAM area for Movie Play */
	Sdram_Map_Movie_Play_H264_Initialize( gDec_Ctrl.bank_y_size, gDec_Ctrl.bank_c_size, &total_bank_num );
	
	if( total_bank_num < gDisp_Ctrl.vdpb_use_num ){
		if( decoder_param->trick == FALSE ){
			H264_Warning_Print(("--- h264_decode_initialize : Trick limit mode is enabled.\n"));
		}
		else{
			H264_Warning_Print(("--- h264_decode_initialize : Image bank area is small and did not acquire a buffer for performing reverse playback.\n" ));
		}
		H264_Warning_Print(("total_num:%d use_max_num:%d\n", total_bank_num, gDisp_Ctrl.vdpb_use_num ));
		gDec_Ctrl.trick_limit	= TRUE;
		gDisp_Ctrl.vdpb_use_num	= total_bank_num;
	}
	
	return;
}


/**
 *	@brief		Initialize Decode Condition of gDec_Param.\n
 *	@param		file_format_type	i: file_format type
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_decode_initialize_gdec_param( T_H264_IF_DECODER_PARAM* decoder_param )
{
	gDec_Param = *decoder_param;
	
	return;
}


/**
 *	@brief		Initialize Decode Control Data of gDec_Ctrl.\n
 *	@param		play_index				play index data
 *	@return		H264_OK					(0) Normal End
 *	@note		None
 *	@attention	None
 */
static VOID h264_decode_initialize_gdec_control( VOID )
{
	gDec_Serial_Num = 0;
	gDec_Ctrl.mode						= E_H264_PLAY_MODE_PLAY;
	gDec_Ctrl.next_mode					= E_H264_PLAY_MODE_PLAY;
	gDec_Ctrl.task_result				= H264_OK;
	gDec_Ctrl.direction					= E_H264_IF_PLAY_DIRECTION_FWD;
	gDec_Ctrl.speed						= E_H264_IF_PLAY_SPEED_1X;
	gDec_Ctrl.trick_limit				= FALSE;
	gDec_Ctrl.pause						= FALSE;
	gDec_Ctrl.iframe_only				= FALSE;
	gDec_Ctrl.error						= FALSE;
	gDec_Ctrl.last_display_pts			= D_H264_DEC_NOT_SET_PTS;
	gDec_Ctrl.last_stream_pts			= D_H264_DEC_NOT_SET_PTS;
	gDec_Ctrl.last_stream_dec_done		= FALSE;
	
	return;
}


/**
 *	@brief		Initialize Stream Control Table of gDmux_Ctrl.\n
 *	@param		None
 *	@return		H264_OK					(0) Normal End
 *	@note		None
 *	@attention	None
 */
static VOID h264_decode_initialize_gdmux_control( VOID )
{
	H264_Movie_Memset( (VOID*)&gDmux_Ctrl, 0, sizeof(gDmux_Ctrl) );
	gDmux_Ctrl.stream_end = FALSE;
	return;
}


/**
 *	@brief		Initialize Display Control Data of gDisp_Ctrl.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_decode_initialize_gdisp_control( T_H264_IF_DISPLAY_INFO* display_info )
{
	H264_Movie_Memset( (VOID*)&gDisp_Ctrl, 0, sizeof(T_H264_DEC_DISP_CTL) );
	gDisp_Ctrl.last_picture_keep			= TRUE;
	gDisp_Ctrl.device_info					= *display_info;
	gDisp_Ctrl.display_index				= 0xFF;
	
	gDisp_Ctrl.refer_index.lcd_decide		= 0xFF;
	gDisp_Ctrl.refer_index.lcd_displaying	= 0xFF;
	gDisp_Ctrl.refer_index.hdmi_decide		= 0xFF;
	gDisp_Ctrl.refer_index.hdmi_displaying	= 0xFF;
	
	return;
}



/**
 *	@brief		Set Decode Condition and Ribery parameters from File Format Informations.\n
 *	@param		None
 *	@return		H264_OK						(0)Normal End
 *	@return		H264_ERR_INPUT_PARAM			(1)Input Parameter Error
 *	@return		H264_ERR_FILE_NO_SUPPORT		(9)Not Support File Error
 *	@note		None
 *	@attention	None
 */
static INT32 h264_decode_init_param( VOID )
{
	INT32						ret;
	UINT32						core_mode;
	
	ULONG						work0_ch;
	ULONG						work1_ch;
	ULONG						work2_ch;
	ULONG						strm_ch;
	ULONG						y_ch;
	ULONG						c_ch;
	
	ULONG						work0_adr;
	ULONG						work1_adr;
	ULONG						work2_adr;
	ULONG						strm_adr;
	ULONG						y_adr;
	ULONG						c_adr;
	
	T_H264_RIBERY_BRIDGE_COMMON	bridge_common;
	T_H264_RIBERY_BRIDGE_PORT	bridge_port;
	T_H264_RIBERY_DEC_PARAM		param;
	T_H264_RIBERY_BRIDGE_YC		bridge_yc;
	
/* --- 0. Initialize Event Flag --- */
	OS_User_Clr_Flg( FID_H264, 0x0000 );
	OS_User_Clr_Flg( FID_H264_SYNC, 0x0000 );
	OS_User_Clr_Flg( FID_H264_DECODER, 0x0000 );
	
/* --- 2. Initialize Ribery */
	H264_Movie_Ribery_Core_Mode_Get( &core_mode );
	
	// work0
	if(      H264_Movie_Sdram_Ch_Check( SDRAM_ADR_MOVIE_PLAY_H264_RIBERY0 ) == SDRAM_ADR_CH0_TOP ){ work0_ch = RB_SDRAM_CH0; }
	else{																							work0_ch = RB_SDRAM_CH1; }
	work0_adr	= SDRAM_ADR_MOVIE_PLAY_H264_RIBERY0;
	
	// work1
	if(      H264_Movie_Sdram_Ch_Check( SDRAM_ADR_MOVIE_PLAY_H264_RIBERY1 ) == SDRAM_ADR_CH0_TOP ){ work1_ch = RB_SDRAM_CH0; }
	else{																							work1_ch = RB_SDRAM_CH1; }
	work1_adr	= SDRAM_ADR_MOVIE_PLAY_H264_RIBERY1;
	
	// work2
	if(      H264_Movie_Sdram_Ch_Check( SDRAM_ADR_MOVIE_PLAY_H264_RIBERY2 ) == SDRAM_ADR_CH0_TOP ){ work2_ch = RB_SDRAM_CH0; }
	else{																							work2_ch = RB_SDRAM_CH1; }
	work2_adr	= SDRAM_ADR_MOVIE_PLAY_H264_RIBERY2;
	
	// stream ch
	if(      H264_Movie_Sdram_Ch_Check( SDRAM_ADR_MOVIE_PLAY_H264_STREAM ) == SDRAM_ADR_CH0_TOP ){ strm_ch = RB_SDRAM_CH0; }
	else{																						   strm_ch = RB_SDRAM_CH1; }
	strm_adr	= SDRAM_ADR_MOVIE_PLAY_H264_STREAM;
	
	bridge_common.ch0_top_addr						= SDRAM_ADR_CH0_TOP;
	bridge_common.ch1_top_addr						= SDRAM_ADR_CH1_TOP;
	
	bridge_common.strm_bank							= SDRAM_SIZ_MOVIE_PLAY_H264_STREAM + SDRAM_SIZ_MOVIE_PLAY_H264_STREAM_END;
	
	bridge_common.common_ch.work0_ch				= work0_ch;
	bridge_common.common_ch.work1_ch				= work1_ch;
	bridge_common.common_ch.work2_ch				= work2_ch;
	bridge_common.common_ch.stream_ch				= strm_ch;
	
	bridge_common.common_addr.work0_addr			= work0_adr;
	bridge_common.common_addr.work1_addr			= work1_adr;
	bridge_common.common_addr.work2_addr			= work2_adr;
	bridge_common.common_addr.stream_addr			= strm_adr;
	
#if 1	// MILB : Movie porting.  ** From SDRAM map is finalized, consider the allocation of port. **
	bridge_port.PME_Vector_Read						= RB_SDRAM_IF_CHB;
	bridge_port.PME_Vector_Write					= RB_SDRAM_IF_CHB;
	bridge_port.EDGE_Read							= RB_SDRAM_IF_CHB;
	bridge_port.EDGE_Write							= RB_SDRAM_IF_CHB;
	bridge_port.UBG_Read							= RB_SDRAM_IF_CHB;
	bridge_port.UBG_Write							= RB_SDRAM_IF_CHB;
	bridge_port.Activity_Read						= RB_SDRAM_IF_CHB;
	bridge_port.Activity_Write						= RB_SDRAM_IF_CHB;
	bridge_port.Stream0_Read_Acore					= RB_SDRAM_IF_CHB;
	bridge_port.Stream0_Write_Acore					= RB_SDRAM_IF_CHB;
	bridge_port.Stream1_Read_Acore					= RB_SDRAM_IF_CHB;
	bridge_port.Stream1_Write_Acore					= RB_SDRAM_IF_CHB;
	bridge_port.CME_Vector_Read_Acore				= RB_SDRAM_IF_CHB;
	bridge_port.CME_Vector_Write_Acore				= RB_SDRAM_IF_CHB;
	bridge_port.COEF_Read_Acore						= RB_SDRAM_IF_CHB;
	bridge_port.COEF_Write_Acore					= RB_SDRAM_IF_CHB;
	bridge_port.Stream0_Read_Bcore					= RB_SDRAM_IF_CHB;
	bridge_port.Stream0_Write_Bcore					= RB_SDRAM_IF_CHB;
	bridge_port.Stream1_Read_Bcore					= RB_SDRAM_IF_CHB;
	bridge_port.Stream1_Write_Bcore					= RB_SDRAM_IF_CHB;
	bridge_port.CME_Vector_Read_Bcore				= RB_SDRAM_IF_CHB;
	bridge_port.CME_Vector_Write_Bcore				= RB_SDRAM_IF_CHB;
	bridge_port.COEF_Read_Bcore						= RB_SDRAM_IF_CHB;
	bridge_port.COEF_Write_Bcore					= RB_SDRAM_IF_CHB;
	bridge_port.SEQ_Read							= RB_SDRAM_IF_CHB;
	bridge_port.SEQ_Write							= RB_SDRAM_IF_CHB;
	bridge_port.CBUS_Read							= RB_SDRAM_IF_CHB;
	bridge_port.CBUS_Write							= RB_SDRAM_IF_CHB;
	bridge_port.CC_1_1_Ref_Picture_Y_Read_Acore		= RB_SDRAM_IF_CHC;
	bridge_port.CC_1_1_Ref_Picture_C_Read_Acore		= RB_SDRAM_IF_CHA;
	bridge_port.CC_1_1_Ref_Picture_Y_Write_Acore	= RB_SDRAM_IF_CHA;
	bridge_port.CC_1_1_Ref_Picture_C_Write_Acore	= RB_SDRAM_IF_CHA;
	bridge_port.CC_1_1_Org_Picture_Y_Read_Acore		= RB_SDRAM_IF_CHC;
	bridge_port.CC_1_1_Org_Picture_C_Read_Acore		= RB_SDRAM_IF_CHA;
	bridge_port.PP_1_1_Org_Picture_Y_Read_Acore		= RB_SDRAM_IF_CHA;
	bridge_port.PP_1_1_Org_Picture_C_Read_Acore		= RB_SDRAM_IF_CHA;
	bridge_port.PIPP_Red_Picture_Y_Read_Acore		= RB_SDRAM_IF_CHA;
	bridge_port.PIPP_Red_Picture_C_Read_Acore		= RB_SDRAM_IF_CHA;
	bridge_port.PIPP_Red_Picture_Y_Write_Acore		= RB_SDRAM_IF_CHA;
	bridge_port.PIPP_Red_Picture_C_Write_Acore		= RB_SDRAM_IF_CHA;
	bridge_port.PME_1_16_Org_Picture_Y_Read_Acore	= RB_SDRAM_IF_CHA;
	bridge_port.PME_1_16_Org_Picture_C_Read_Acore	= RB_SDRAM_IF_CHA;
	bridge_port.PME_1_16_Ref_Picture_Y_Read_Acore	= RB_SDRAM_IF_CHA;
	bridge_port.PME_1_16_Ref_Picture_C_Read_Acore	= RB_SDRAM_IF_CHA;
	bridge_port.CC_1_1_Ref_Picture_Y_Read_Bcore		= RB_SDRAM_IF_CHC;
	bridge_port.CC_1_1_Ref_Picture_C_Read_Bcore		= RB_SDRAM_IF_CHA;
	bridge_port.CC_1_1_Ref_Picture_Y_Write_Bcore	= RB_SDRAM_IF_CHA;
	bridge_port.CC_1_1_Ref_Picture_C_Write_Bcore	= RB_SDRAM_IF_CHA;
	bridge_port.CC_1_1_Org_Picture_Y_Read_Bcore		= RB_SDRAM_IF_CHC;
	bridge_port.CC_1_1_Org_Picture_C_Read_Bcore		= RB_SDRAM_IF_CHA;
	bridge_port.PP_1_1_Org_Picture_Y_Read_Bcore		= RB_SDRAM_IF_CHA;
	bridge_port.PP_1_1_Org_Picture_C_Read_Bcore		= RB_SDRAM_IF_CHA;
	bridge_port.PIPP_Red_Picture_Y_Read_Bcore		= RB_SDRAM_IF_CHA;
	bridge_port.PIPP_Red_Picture_C_Read_Bcore		= RB_SDRAM_IF_CHA;
	bridge_port.PIPP_Red_Picture_Y_Write_Bcore		= RB_SDRAM_IF_CHA;
	bridge_port.PIPP_Red_Picture_C_Write_Bcore		= RB_SDRAM_IF_CHA;
	bridge_port.PME_1_16_Org_Picture_Y_Read_Bcore	= RB_SDRAM_IF_CHA;
	bridge_port.PME_1_16_Org_Picture_C_Read_Bcore	= RB_SDRAM_IF_CHA;
	bridge_port.PME_1_16_Ref_Picture_Y_Read_Bcore	= RB_SDRAM_IF_CHA;
	bridge_port.PME_1_16_Ref_Picture_C_Read_Bcore	= RB_SDRAM_IF_CHA;
#endif
	
	ret = H264_Ribery_Initialize( H264_MODE_DECODE, core_mode, &bridge_common, &bridge_port );
	if( ret != H264_OK ){
		return ret;
	}
	
	// info table/File
	H264_Ribery_Dec_Vtable_Init();
	
	// y_ch / y_adr
	if(      H264_Movie_Sdram_Ch_Check( SDRAM_ADR_MOVIE_PLAY_YCC ) == SDRAM_ADR_CH0_TOP ){ y_ch = RB_SDRAM_CH0; }
	else{																					 y_ch = RB_SDRAM_CH1; }
	y_adr		= SDRAM_ADR_MOVIE_PLAY_YCC;
	
	// c_ch / c_adr
	if(      H264_Movie_Sdram_Ch_Check( SDRAM_ADR_MOVIE_PLAY_YCC ) == SDRAM_ADR_CH0_TOP ){ c_ch = RB_SDRAM_CH0; }
	else{																					 c_ch = RB_SDRAM_CH1; }
	c_adr		= SDRAM_ADR_MOVIE_PLAY_YCC;
	
	// init param set
	param.size								= gDec_Param.image_size;
	param.frame_rate						= gDec_Param.frame_rate;
	param.trick								= H264_OFF;
	param.keep_idx							= 0;
	param.bank_num							= gDisp_Ctrl.vdpb_use_num;
	param.scan_mode							= gDec_Param.scan_mode;
	param.self_rec_play						= gDec_Param.self_rec;
	
	// bridge yc set
	bridge_yc.tb_flag						= H264_OFF;
	
	bridge_yc.map_yc_size.width				= gDec_Ctrl.bank_y_width;
	bridge_yc.map_yc_size.y_bank			= gDec_Ctrl.bank_y_size;
	bridge_yc.map_yc_size.c_bank			= gDec_Ctrl.bank_c_size;
	bridge_yc.map_yc_size.localdec_width	= 0;
	bridge_yc.map_yc_size.localdec_y_bank	= 0;
	bridge_yc.map_yc_size.localdec_c_bank	= 0;
	bridge_yc.map_yc_size.reduction_width	= 0;
	bridge_yc.map_yc_size.reduction_y_bank	= 0;
	bridge_yc.map_yc_size.reduction_c_bank	= 0;
	
	bridge_yc.map_yc_ch.y_ch				= y_ch;
	bridge_yc.map_yc_ch.c_ch				= c_ch;
	bridge_yc.map_yc_ch.localdec_y_ch		= 0;
	bridge_yc.map_yc_ch.localdec_c_ch		= 0;
	bridge_yc.map_yc_ch.reduction_y_ch		= 0;
	bridge_yc.map_yc_ch.reduction_c_ch		= 0;
	
	bridge_yc.map_yc_addr.y_addr			= y_adr;
	bridge_yc.map_yc_addr.c_addr			= c_adr;
	bridge_yc.map_yc_addr.localdec_y_addr	= 0;
	bridge_yc.map_yc_addr.localdec_c_addr	= 0;
	bridge_yc.map_yc_addr.reduction_y_addr	= 0;
	bridge_yc.map_yc_addr.reduction_c_addr	= 0;
	
	// init param & start
	H264_Ribery_Dec_InitParam_Set( &param, &bridge_yc );
	
	ret = H264_Ribery_Dec_Start();
	if( ret != H264_OK ){
		return ret;
	}
	return H264_OK;
}

/**
 *	@brief		FLUSH Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H264_PLAY_MODE h264_decode_flush( VOID )
{
	return E_H264_PLAY_MODE_FLUSH;
}

/**
 *	@brief		PLAY Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H264_PLAY_MODE h264_decode_play( VOID )
{
	return E_H264_PLAY_MODE_PLAY;
}

/**
 *	@brief		PAUSE Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H264_PLAY_MODE h264_decode_pause( VOID )
{
	return E_H264_PLAY_MODE_PAUSE;
}

/**
 *	@brief		RESUME Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H264_PLAY_MODE h264_decode_resume( VOID )
{
	return E_H264_PLAY_MODE_RESUME;
}

/**
 *	@brief		STOP Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H264_PLAY_MODE h264_decode_stop( VOID )
{
	return E_H264_PLAY_MODE_STOP;
}

/**
 *	@brief		END Function.\n
 *	@param		None
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
static E_H264_PLAY_MODE h264_decode_end( VOID )
{
	return E_H264_PLAY_MODE_END;
}


/**
 *	@brief		Forced Stop relational tasks.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_decode_task_forced_stop( VOID )
{
	OS_User_Ter_Tsk( TID_H264_ENT );
	OS_User_Ter_Tsk( TID_H264_CC );
	OS_User_Ter_Tsk( TID_H264_DISP );
	return;
}

/**
 *	@brief		Set VCPB Information Table & Execute Ribery Decoder
 *	@param		video_index		video chunk index
 *	@param		video_index		sample index
 *	@return		H264_OK		Normal End
 *	@return		H264_NG		It cannot get vacant bank of VCPB_INFO
 *	@note		None
 *	@attention	None
 */
static INT32 h264_decode_set_vcpb( UINT32 video_index, UCHAR vdpb_use_num)
{
	INT32				ret  = 0;
	UINT32				bank_num;
	T_RB_INFOTABLE_VCPB	vcpb_info;
	
	H264_Movie_Memset( &vcpb_info, 0, sizeof(T_RB_INFOTABLE_VCPB) );
	
	// Input Parameter Check
	if( video_index >= D_H264_DEC_SAVE_SAMPLE_MAX_NUM ){
		H264_Info_Print(("--- h264_decode_set_vcpb: Parameter Error. video_index=%d\n",video_index ));
		return H264_NG;
	}
	
	// It checks Number of Using VCPB/VDPB bank
	if( gDisp_Ctrl.use_count >= vdpb_use_num ){
		/* Full of VDPB Bank. So, it cannot register VCPB Information Table. */
		return H264_NG;
	}
	
	// Suppress the depletion of VCPB_info.
	if( (gDisp_Ctrl.use_count > gDisp_Ctrl.buf_count) && ((gDisp_Ctrl.use_count - gDisp_Ctrl.buf_count) >= D_H264_RIBERY_VCPB_EMPTY_FIFO_MAX_NUM) ){
		return H264_NG;
	}
	
	// It gets Bank Number of vacant
	ret = H264_Ribery_Dec_Vcpb_Empty_Field_Get( &bank_num );
	if(ret != H264_OK) {
		H264_Info_Print(("--- h264_decode_set_vcpb: Can't get empty bank!!(ret=%x)\n",ret));
		return H264_NG;
	}
	
	// It set VCPB_info_table
	vcpb_info.serial_num	= gDec_Serial_Num;
	vcpb_info.PTS			= gDmux_Ctrl.decode_video[video_index].pts;
	vcpb_info.DTS			= gDmux_Ctrl.decode_video[video_index].dts;
	vcpb_info.pic_size		= gDmux_Ctrl.decode_video[video_index].stream_size;
	vcpb_info.pic_addr		= gDmux_Ctrl.decode_video[video_index].stream_address;
	
	// save vcpb index
	gDmux_Ctrl.decode_video[video_index].vcpb_index = bank_num;
	
// for Debug
//printf("set idx(%d):vcpb_index=%d\n", video_index, gDmux_Ctrl.decode_video[video_index].vcpb_index);
// for Debug
	
	// It set VCPB_info_table, FIFO. And then, It request Decode Start
	H264_Ribery_Dec_VCPB_Set( bank_num, &vcpb_info);
	
	if( gDec_Param.scan_mode == E_H264_IF_SCAN_MODE_PROGRESSIVE ){
		// Increment Number of VDPB Bank
		gDisp_Ctrl.use_count++;
	}
	else{
		// Interlace : VCPB=2 -> 1Frame  gDec_Serial_Num == Odd number
		// user_count increment
		if( (gDec_Serial_Num % 2) ){
			// Increment Number of VDPB Bank
			gDisp_Ctrl.use_count++;
		}
	}
	
	gDec_Serial_Num++;
	
	return H264_OK;
}

/**
 *	@brief		Get Next sample and set Dt Table.\n
 *	@param		i:video_index		video sample index
 *	@return		H264_OK				(0)Normal End
 *	@return		H264_END			(1)Normal End( Data Read Completed )
 *	@note		None
 *	@attention	None
 */
static INT32 h264_decode_next_video_sample_get( UINT32* video_index )
{

	UINT32	read_pointer;
	UINT32	next_read_pointer;
	UINT32	write_pointer;
	
	write_pointer		= gDmux_Ctrl.write_pointer;
	read_pointer		= gDmux_Ctrl.read_pointer;
	next_read_pointer	= read_pointer + 1;
	if( next_read_pointer >= D_H264_DEC_SAVE_SAMPLE_MAX_NUM ){
		next_read_pointer = 0;
	}
	
	if( write_pointer ==  read_pointer ){
		// Video Stream is nothing.
		return H264_NG;
	}
	
	*video_index			= next_read_pointer;
	gDmux_Ctrl.read_pointer	= next_read_pointer;
	
	return H264_OK;
}

/**
 *	@brief		Get Next sample and set Dt Table.\n
 *	@param		i:video_index		video sample index
 *	@return		H264_OK				(0)Normal End
 *	@return		H264_END			(1)Normal End( Data Read Completed )
 *	@note		None
 *	@attention	None
 */
static INT32 h264_decode_next_video_sample_request( VOID )
{
	BOOL			host_ret;
	UINT32			next_write_pointer;
	UINT32			free_pointer;
	
	if( gDmux_Ctrl.stream_end == TRUE ){
		// Already Stream End.
		return H264_OK;
	}
	
	next_write_pointer	= gDmux_Ctrl.write_pointer + 1;
	free_pointer		= gDmux_Ctrl.free_pointer;
	
	if( next_write_pointer >= D_H264_DEC_SAVE_SAMPLE_MAX_NUM ){
		next_write_pointer = 0;
	}
	if( next_write_pointer == free_pointer ){
		// Don't request because management table is full.
		return H264_OK;
	}
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "FeedReq" ));
	
	host_ret = Host_H264_Notify_Video_Stream_Request();
	if( host_ret == FALSE ){
		H264_Err_Print(("--- h264_decode_next_video_sample_request:Host_H264_Notify_Video_Stream_Request Error\n"));
		return H264_NG;
	}
	
	return H264_OK;
}


/**
 *	@brief		Get VDPB_Information
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 h264_decode_get_vdpb( UINT32* bank_num, T_RB_INFOTABLE_VDPB* vdpb_info )
{
	INT32	ret;
	
	if( (bank_num == NULL) || (vdpb_info == NULL) ){
		return H264_ERR_INPUT_PARAM;
	}
	
	/* Get Numbaer of Bank and VDPB_info_table from FIFO. */
	ret = H264_Ribery_Dec_VDPB_Get( bank_num, vdpb_info );
	
	return ret;
}


/**
 *	@brief		Free not use VDPB bank.\n
 *	@param		i:direction			Play Direction
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_decode_vdpb_free( E_H264_IF_PLAY_DIRECTION direction, T_H264_DEC_DISP_REFER_INDEX protect_index )
{
	UINT32		table_count;
	
	for( table_count=0; table_count<gDisp_Ctrl.vdpb_use_num; table_count++ ){
		// display use bank is not target
		if( (protect_index.lcd_decide  == table_count) || (protect_index.lcd_displaying  == table_count) ||
			(protect_index.hdmi_decide == table_count) || (protect_index.hdmi_displaying == table_count) ||
			(gDisp_Ctrl.display_index == table_count )){
			continue;
		}
		if( gDisp_Ctrl.inf[table_count].use_flg == TRUE ){
			
			if( gDisp_Ctrl.inf[table_count].release_flg == FALSE ){
				
				if( direction == E_H264_IF_PLAY_DIRECTION_FWD ){
					
					if( gDisp_Ctrl.display_pts <= gDisp_Ctrl.inf[table_count].PTS ){
						// In larger PTS than "display PTS", nothing is done.
						continue;
					}
				}
				else{
					if( gDisp_Ctrl.display_pts >= gDisp_Ctrl.inf[table_count].PTS ){
						// In PTS smaller than "display PTS", nothing is done.
						continue;
					}
				}
			}
			/* Detect!! */
//			H264_Warning_Print(("free PTS = %d\n",gDisp_Ctrl.inf[table_count].PTS ));
			H264_Ribery_Dec_VDPB_Free(table_count);
			gDisp_Ctrl.inf[table_count].use_flg		= FALSE;
			gDisp_Ctrl.use_count--;	// (YC area bank) use count        *max 9 VDPB
			gDisp_Ctrl.buf_count--;	// (YC area bank) buffering count  *over 4 bank -> display start
		}
	}
	
	return;
}


/**
 *	@brief		Search display index.\n
 *	@param		i:display_index		Display Index
 *	@return		H264_OK				Normal End
 *	@note		None
 *	@attention	None
 */
static INT32 h264_decode_search_display_index( E_H264_DEC_PTS_SEARCH_MODE search_mode, UCHAR* display_index, T_H264_DEC_DISP_REFER_INDEX protect_index )
{
	UINT32		table_count;
	
	// Initialize display_index
	*display_index = 0xFF;
	
	for( table_count=0; table_count<gDisp_Ctrl.vdpb_use_num; table_count++ ){
		
		// display use bank is not target
		if( (protect_index.lcd_decide  == table_count) || (protect_index.lcd_displaying  == table_count) ||
			(protect_index.hdmi_decide == table_count) || (protect_index.hdmi_displaying == table_count) ){
			continue;
		}
		
		if( gDisp_Ctrl.inf[table_count].use_flg == TRUE ){
			
			if( gDisp_Ctrl.inf[table_count].release_flg == TRUE ){
				continue;
			}
			// The same "PTS" is searched.
			if( search_mode == E_H264_DEC_PTS_SEARCH_MODE_SAME ){
				if( gDisp_Ctrl.display_target_pts == gDisp_Ctrl.inf[table_count].PTS ){
					*display_index = table_count;
					break;
				}
			}
			// Smallest "PTS" in bank is searched. (Case of Fast Forward)
			else if( search_mode == E_H264_DEC_PTS_SEARCH_MODE_SMALL ){
				if( gDisp_Ctrl.display_index == table_count ){
					continue;
				}
				if( *display_index == 0xFF ){
					// found valid index. "display_index" renew.
					*display_index = table_count;
					continue;
				}
				if( gDisp_Ctrl.inf[*display_index].PTS > gDisp_Ctrl.inf[table_count].PTS ){
					*display_index = table_count;
				}
			}
			// Largest "PTS" in bank is searched. (Case of Fast Rewind)
			else{
				if( gDisp_Ctrl.display_index == table_count ){
					continue;
				}
				if( *display_index == 0xFF ){
					// found valid index. "display_index" renew.
					*display_index = table_count;
					continue;
				}
				if( gDisp_Ctrl.inf[*display_index].PTS < gDisp_Ctrl.inf[table_count].PTS ){
					*display_index = table_count;
				}
			}
		}
	}
	
	if( *display_index == 0xFF ){
		return H264_NG;
	}
	
	return H264_OK;
}


/**
 *	@brief		Make Display Image.\n
 *	@param		i:decode_addr		Decode Address
 *	@param		i:disp_addr			Display Address
 *	@param		i:fill_counter		Fill Counter
 *	@note		None
 *	@attention	None
 */
static VOID h264_decode_display_image( T_H264_IF_DISPLAY_PARAM* display_info, T_H264_MOVIE_ADDR* dec_addr, T_H264_MOVIE_ADDR* disp_addr, BOOL ribery_output )
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
	E_H264_IF_DISP_ROTATE			rotate;
	E_H264_RESIZE_MODE				resize_mode;
	
	T_H264_MOVIE_REC_TBL			rec_ctrl;
	
#if 0	// for Debug
	OS_USER_SYSTIM					time_before, time_after;
#endif	// for Debug
	
	H264_Movie_Memset( &rec_ctrl, 0, sizeof( rec_ctrl ) );
	
	if( ribery_output == TRUE ){
		disp_addr->y_address	= dec_addr->y_address;
		disp_addr->c_address	= dec_addr->c_address;
	}
	else{
		if( (display_info->rotate == E_H264_IF_DISP_ROTATE_90) ||
			(display_info->rotate == E_H264_IF_DISP_ROTATE_270) ){
			
			if( (display_info->image_lines <= SDRAM_WIDTH_MOVIE_PLAY_ROTATE_WORK ) &&	// Width and Lines are reversed during 90/270 rotation.
				(display_info->image_width <= SDRAM_LINES_MOVIE_PLAY_ROTATE_WORK ) ){	// Width and Lines are reversed during 90/270 rotation.
				
				rec_ctrl.src_width		= gDec_Param.image_width;
				rec_ctrl.src_lines		= gDec_Param.image_lines;
				rec_ctrl.g_src_width	= gDec_Ctrl.bank_y_width;
				rec_ctrl.g_src_lines	= gDec_Ctrl.bank_y_lines;
				rec_ctrl.src_addr[0]	= dec_addr->y_address;
				rec_ctrl.src_addr[1]	= dec_addr->c_address;
				
				rec_ctrl.dst_width		= display_info->image_lines;	// Width and Lines are reversed during 90/270 rotation.
				rec_ctrl.dst_lines		= display_info->image_width;	// Width and Lines are reversed during 90/270 rotation.
				rec_ctrl.g_dst_width	= SDRAM_WIDTH_MOVIE_PLAY_ROTATE_WORK;
				rec_ctrl.g_dst_lines	= SDRAM_LINES_MOVIE_PLAY_ROTATE_WORK;
				rec_ctrl.dst_addr[0]	= SDRAM_ADR_MOVIE_PLAY_ROTATE_WORK;
				rec_ctrl.dst_addr[1]	= SDRAM_ADR_MOVIE_PLAY_ROTATE_WORK + (SDRAM_WIDTH_MOVIE_PLAY_ROTATE_WORK * SDRAM_LINES_MOVIE_PLAY_ROTATE_WORK);
				
				rec_ctrl.ribery_mode	= TRUE;
				rec_ctrl.rotate			= E_H264_IF_DISP_ROTATE_0;
				rec_ctrl.resize_mode	= E_H264_RESIZE_MODE_NEAREST;
				
#if 0	// for Debug
		OS_User_Get_Tim(&time_before);
#endif	// for Debug
				// resize
				H264_Movie_Rec_Resize( &rec_ctrl );
#if 0	// for Debug
		OS_User_Get_Tim(&time_after);
		H264_Info_Print(("resize_time: %d\n", (UINT32)(time_after - time_before) ));
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
				src_img_width		= gDec_Param.image_width;
				src_img_lines		= gDec_Param.image_lines;
				src_g_img_width		= gDec_Ctrl.bank_y_width;
				src_g_img_lines		= gDec_Ctrl.bank_y_lines;
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
			resize_mode			= E_H264_RESIZE_MODE_NEAREST;
		}
		else{
			ribery_mode = TRUE;
			src_img_width		= gDec_Param.image_width;
			src_img_lines		= gDec_Param.image_lines;
			src_g_img_width		= gDec_Ctrl.bank_y_width;
			src_g_img_lines		= gDec_Ctrl.bank_y_lines;
			src_y_addr			= dec_addr->y_address;
			src_c_addr			= dec_addr->c_address;
			
			dst_img_width		= display_info->image_width;
			dst_img_lines		= display_info->image_lines;
			dst_g_img_width		= display_info->global_width;
			dst_g_img_lines		= display_info->global_lines;
			dst_y_addr			= disp_addr->y_address;
			dst_c_addr			= disp_addr->c_address;
			
			rotate				= display_info->rotate;
			resize_mode			= E_H264_RESIZE_MODE_BILINEAR;
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

		if (User_IsDualStream() && !User_IsH265MainStream()) {
			H264_Movie_SGDE_Resize(&rec_ctrl);
		} else {
			//resize & rotate
			H264_Movie_Rec_Resize( &rec_ctrl );
		}
#if 0	// for Debug
		OS_User_Get_Tim(&time_after);
		H264_Info_Print(("resize_rotate_time: %d\n", (UINT32)(time_after - time_before) ));
#endif	// for Debug
	}
	
	return;
}


/**
 *	@brief		Get video stream of top image.\n
 *	@param		i:file_id				File Identifier
 *	@param		i:video_stream_address	video stream address
 *	@param		i/o:video_stream_size	video stream size
 *	@return		H264_OK					(0)Normal End
 *	@return		H264_ERR_INPUT_PARAM		(1)Input Parameter Error
 *	@return		H264_ERR_FILE_ACCESS		(8)File Access Error
 *	@return		H264_ERR_FILE_NO_SUPPORT	(9)Not Support File Error
 *	@note		None
 *	@attention	None
 */
static INT32 h264_decode_top_image_get( ULLONG req_pts, T_H264_IF_VIDEO_YUV_INFO* decode_yuv_info )
{
	INT32					ret = H264_NG;
	OS_USER_ER				ercd = 0;
	OS_USER_FLGPTN			flgptn = 0;
	OS_USER_FLGPTN			waiptn = 0;
	UINT32					video_index = 0;
	UINT32					bank_num = 0;
	T_RB_INFOTABLE_VDPB		vdpb_info;
	
	while(1){
		
		ret = h264_decode_next_video_sample_get( &video_index );
		if( ret != H264_OK ){
			H264_Err_Print(("--- h264_decode_top_image_get : Video stream has not been set.\n"));
			ret = H264_NG;
			break;
		}
		
		ret = h264_decode_set_vcpb( video_index, gDisp_Ctrl.vdpb_use_num );
		if( ret != H264_OK ){
			H264_Err_Print(("--- top_image_get: vcpb setting error!!.\n"));
			break;
		}
		
		if ( gDmux_Ctrl.decode_video[video_index].pts == req_pts ){
			break;
		}
	}
	
	if( ret != H264_OK ){
		return ret;
	}
	
	waiptn = (FLG_H264_DEC_VDPB_COMP | FLG_H264_WDT );
	
	while(1){
		ercd = OS_User_Twai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn, 1000 );
		if( ercd != D_OS_USER_E_OK ){
			H264_Err_Print(("--- Can not get Top Image for Ribery(T/O).\n"));
			ret = H264_NG;
			break;
		}
		OS_User_Clr_Flg( FID_H264, ~FLG_H264_DEC_VDPB_COMP );
		
		ret = h264_decode_get_vdpb( &bank_num, &vdpb_info );
		if( ret == H264_OK ){
			OS_User_Set_Flg( FID_H264, FLG_H264_DEC_VDPB_COMP );
		}
		else if( ret == H264_OK_COMPLETE ){
			// only 1 request. 
			// Since it is a normal route, "OK" is put into a return value.
			ret = H264_OK;
		}
		else{
			H264_Info_Print(("--- h264_decode_get_vdpb error. ret=%d\n", ret ));
			break;
		}
		
		if( vdpb_info.PTS != req_pts ){
			continue;
		}
		
		Sdram_Map_Movie_Play_H264_Get_Vdec( bank_num, &decode_yuv_info->y_addr, &decode_yuv_info->cbcr_addr );
		
		decode_yuv_info->g_width	= gDec_Ctrl.bank_y_width;
		decode_yuv_info->g_lines	= gDec_Ctrl.bank_y_lines;
		decode_yuv_info->width		= gDec_Param.image_width;
		decode_yuv_info->lines		= gDec_Param.image_lines;
		decode_yuv_info->yuv_format	= E_H264_IF_YUV_FORMAT_VIDEO;
		break;
	}
	
	return ret;
}
