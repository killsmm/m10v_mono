/**
 * @file		h264_encode.c
 * @brief		H.264 Encode Process.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h264_encode.h"
#include "mba_if.h"
#include "h264_user.h"

#ifdef CO_H264_SECTION_ON
//#pragma arm section code="H264_CODE"
//#pragma arm section rodata="H264_CONST"
//#pragma arm section rwdata="H264_INIT"
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// vector size define (MB = MacroBlock)
#define D_H264_ENCODE_VECTOR_SIZE_4096_2304			(0x48000)	// (256*144)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_4096_2160			(0x43800)	// (256*135)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_4096_2048			(0x40000)	// (256*128)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_3840_2160			(0x6E280)	// (240*235)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_3840_1920			(0x38400)	// (240*120)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_3000_3000			(0x45080)	// (188*188)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_2704_2028			(0x29EB8)	// (169*127)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_2704_1520			(0x1F5B8)	// (169* 95)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_2560_1440			(0x1C200)	// (160* 90)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1920_1440			(0x15180)	// (120* 90)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1920_1080			(0xFF00)	// (120* 68)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1920_960			(0xE100)	// (120* 60)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1440_1080			(0xBF40)	// ( 90* 68)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1440_720			(0x7E90)	// ( 90* 45)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1280_1024			(0xA000)	// ( 80* 64)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1280_960			(0x9600)	// ( 80* 60)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1280_720			(0x7080)	// ( 80* 45)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1280_768			(0x7800)	// ( 80* 48)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1024_768			(0x6000)	// ( 64* 48)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_960_540			(0x3FC0)	// ( 60* 34)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_960_480			(0x3840)	// ( 60* 30)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_800_600			(0x3860)	// ( 50* 38)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_864_480			(0x32A0)	// ( 54* 30)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_848_480			(0x31B0)	// ( 53* 30)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_720_576			(0x32A0)	// ( 45* 36)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_720_480			(0x2A30)	// ( 45* 30)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_640_480			(0x2580)	// ( 40* 30)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_640_360			(0x1CC0)	// ( 40* 23)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_480_270			(0xFF0)		// ( 30* 17)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_432_240			(0xCA8)		// ( 27* 15)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_320_240			(0x960)		// ( 20* 15)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1600_896			(0xAF00)	// ( 100* 56)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1024_576			(0x4800)	// ( 64* 36)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_800_448			(0x2BC0)	// ( 50* 28)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_352_288			(0xC60)		// ( 22* 18)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_2592_1944			(0x269A0)	// (162*122)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_2304_1296			(0x16C80)	// (144* 81)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_1920_1280			(0x12C00)	// (120* 80)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_640_512			(0x2800)	// ( 40* 32)MB * 8byte
#define D_H264_ENCODE_VECTOR_SIZE_384_288			(0xD80)		// ( 24* 18)MB * 8byte

// brightness size define
#define D_H264_ENCODE_BRIGHTNESS_SIZE_4096_2304		(0xD8000)	// (256*144)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_4096_2160		(0xCA800)	// (256*135)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_4096_2048		(0xC0000)	// (256*128)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_3840_2160		(0xBDD80)	// (240*235)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_3840_1920		(0xA8C00)	// (240*120)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_3000_3000		(0xCF180)	// (188*188)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_2704_2028		(0x7DC28)	// (169*127)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_2704_1520		(0x5E128)	// (169* 95)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_2560_1440		(0x54600)	// (160* 90)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1920_1440		(0x3F480)	// (120* 90)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1920_1080		(0x2FD00)	// (120* 68)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1920_960		(0x2A300)	// (120* 60)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1440_1080		(0x23DC0)	// ( 90* 68)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1440_720		(0x17BB0)	// ( 90* 45)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1280_1024		(0x16800)	// ( 80* 64)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1280_960		(0x1C200)	// ( 80* 60)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1280_720		(0x15180)	// ( 80* 45)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1280_768		(0x15180)	// ( 80* 48)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1024_768		(0x12000)	// ( 64* 48)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_960_540		(0xBF40)	// ( 60* 34)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_960_480		(0xA8C0)	// ( 60* 30)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_800_600		(0xB220)	// ( 50* 38)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_864_480		(0x97E0)	// ( 54* 30)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_848_480		(0x9510)	// ( 53* 30)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_720_576		(0x97E0)	// ( 45* 36)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_720_480		(0x7E90)	// ( 45* 30)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_640_480		(0x7080)	// ( 40* 30)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_640_360		(0x5640)	// ( 40* 23)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_480_270		(0x2FD0)	// ( 30* 17)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_432_240		(0x25F8)	// ( 27* 15)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_320_240		(0x1C20)	// ( 20* 15)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1600_896		(0x20D00)	// ( 100* 56)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1024_576		(0xD800)	// ( 64* 36)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_800_448		(0x8340)	// ( 50* 28)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_352_288		(0x2520)	// ( 22* 18)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_2592_1944		(0x73CE0)	// (162*122)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_2304_1296		(0x44580)	// (144* 81)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_1920_1280		(0x38400)	// (120* 80)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_640_512		(0x7800)	// ( 40* 32)MB * 24byte
#define D_H264_ENCODE_BRIGHTNESS_SIZE_384_288		(0x2880)	// ( 24* 18)MB * 24byte

#define D_H264_ENCODE_LATER_THRESHOLD		(30)

// for Video PES
#define D_H264_ENCODE_PES_HEADER_VIDEO_SIZE	(19)	// PES header size

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
static	volatile	T_H264_MOVIE_UI					gH264_Rec_Param;

// Encode Control Data
static	volatile	T_H264_ENC_CTL					gH264_Enc_Ctrl = {0};

static	volatile	ULONG							gH264_Stream_Stop_Flg[E_H264_IF_STREAM_CH_MAX] = {FLG_H264_SYNC_STOP_ENC_CH0, FLG_H264_SYNC_STOP_ENC_CH1, FLG_H264_SYNC_STOP_ENC_CH2};
static	volatile	ULONG							gH264_Stream_TimeLapse_Sync_Flg[E_H264_IF_STREAM_CH_MAX] = {FLG_H264_SYNC_ENC_CH0, FLG_H264_SYNC_ENC_CH1, FLG_H264_SYNC_ENC_CH2};

static	volatile	ULONG							gH264_ENT_Enable_Flg[E_H264_IF_STREAM_CH_MAX] = {0, 0, 0};
static	volatile	T_H264_ENC_VIN_ADDRESS_CONTROL	gH264_Vin_Adr_Ctl[E_H264_IF_STREAM_CH_MAX];
static	volatile	INT32							gH264_Vin_Adr_Emp_Over_Flg[E_H264_IF_STREAM_CH_MAX];
static	volatile	UINT32							gH264_Enc_Serial_Num[E_H264_IF_STREAM_CH_MAX] = {0, 0, 0};
static	volatile	UINT32							gH264_Map_Enable_Flg[E_H264_IF_STREAM_CH_MAX] = {0, 0, 0};
static	volatile	UINT32							gH264_ViewLater_Count = 0;
static	volatile	UINT32							gH264_MovieLater_Count = 0;
static	volatile	UINT32							gH264_ViewLater_Total_Count = 0;
static	volatile	UINT32							gH264_MovieLater_Total_Count = 0;
static	volatile	UINT32							gH264_CC_Stop_Flg = H264_OFF;

// Dynamic Param
static	volatile	T_H264_IF_DYNAMIC_PARAM_RECORD	gH264_Dynamic_Param[E_H264_IF_STREAM_CH_MAX];
static	volatile	UINT32							gH264_Dynamic_Change_Flag[E_H264_IF_STREAM_CH_MAX];

// Insert IDR Picture
static	volatile	UINT32							gH264_Insert_IDR_Flag[E_H264_IF_STREAM_CH_MAX];
static	volatile	BOOL							gH264_First_IDR_Flag[E_H264_IF_STREAM_CH_MAX];

// Free Stream
static	volatile	UINT32							gH264_Free_Vstream_End_Flg[E_H264_IF_STREAM_CH_MAX] = { H264_ON, H264_ON, H264_ON };	/**< Free Video Stream End Flag */
static	volatile	UINT32							gH264_Free_Vstream_Count = 0;										/**< Free Video Stream Count */
static	volatile	T_RB_FIFO_INDEX					gH264_Free_Vstream_Param[RB_VSTRM_INFO_TABLE_NUM];				/**< Free Video Stream StreamID/Index */

static	volatile	ULONG							gH264_Stream_Buffer0_Limit = 0;										/**< Stream Buffer0 Limit */
static	volatile	ULONG							gH264_Stream_Buffer1_Limit = 0;										/**< Stream Buffer1 Limit */
static  T_H264_IF_VIDEO_STREAM_PARAM g_video_stream_info[3];
/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static	INT32	h264_encode_task_create( VOID );
static	INT32	h264_encode_vector_task_create( VOID );
static	INT32	h264_encode_init_codec( VOID );
static	INT32	h264_encode_set_param_codec( E_H264_IF_STREAM_CH stream_ch );
static	VOID	h264_encode_task_start( UINT32 task_id );
static	INT32	h264_encode_ribery_vraw_param_set(
					E_H264_IF_STREAM_CH		stream_ch,
					T_RB_INFOTABLE_VRAW		*vraw_info,
					T_RB_FIFO_INDEX			*vraw_index,
					ULLONG					*frame_no,
					UINT32					stop_req_flag,
					USHORT					*vop_cnt,
					USHORT					*gop_cnt
					);
static	INT32	h264_encode_ribery_vraw_set( T_RB_FIFO_INDEX vraw_index, T_RB_INFOTABLE_VRAW *vraw_info );
static	INT32	h264_encode_ribery_start( UINT32 count, T_RB_FIFO_INDEX *vraw_index );
static	INT32	h264_encode_encoder_result_get( T_H264_ENC_VIDEO_INFO* video_info, UINT32* info_count );
static	VOID	h264_encode_stream_around_check( T_H264_ENC_SAMPLE_PARAM* sample_param );
static	VOID	h264_encode_pes_header_set( ULONG stream_ch, ULONG addr, ULONG size, ULLONG pts, ULLONG dts );
static	VOID	h264_encode_illagal( T_H264_ENC_VIDEO_INFO* video_info, UINT32 info_count, ULLONG total_duration );
static	VOID	h264_encode_ch_map_reset( E_H264_IF_STREAM_CH stream_ch );

static	VOID	h264_encode_Vin_initialize( VOID );
static	INT32	h264_encode_Vin_get_address( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no, E_H264_IF_IMAGE_SIZE size, T_H264_IF_GET_IMAGE_ADDR_PARAM* addr_param );
static	VOID	h264_encode_Vin_decide_address( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no );

static	VOID	h264_encode_Vin_set_decide_bank( E_H264_IF_STREAM_CH stream_ch, INT32 bank, ULLONG frame_no );
static	INT32	h264_encode_Vin_get_decide_bank( E_H264_IF_STREAM_CH stream_ch, INT32* bank, ULLONG* frame_no );
static	VOID	h264_encode_Vin_set_empty_bank( E_H264_IF_STREAM_CH stream_ch, INT32 bank );
static	INT32	h264_encode_Vin_get_empty_bank( E_H264_IF_STREAM_CH stream_ch, INT32* bank );

static	VOID	h264_encode_video_stream_param_set(T_H264_ENC_VIDEO_INFO* video_info, UINT32 sample_index, T_H264_IF_VIDEO_STREAM_PARAM* video_stream_info, ULLONG* total_duration);
static	VOID	h264_encode_result_notify( INT32 enc_result );
static	VOID	h264_encode_free_vstream( VOID );

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
INT32 H264_Encode_Main_Ready( E_H264_IF_STREAM_CH stream_ch, T_H264_MOVIE_UI* record_info )
{
	INT32	ret;
	
	if( record_info == NULL ){
		return H264_ERR_INPUT_PARAM;
	}
	
	// Initialize Control Data(Global Data & SDRAM)
	ret = H264_Encode_Initialize( stream_ch, record_info );
	if( ret != H264_OK ){
		if( gH264_Enc_Ctrl.rec_status != H264_ENC_RECORDING ){
			H264_Encode_Finalize();
		}
		return ret;
	}
	if( gH264_Enc_Ctrl.rec_status != H264_ENC_RECORDING ){
		// Initialize Ribery Macro
		// Ribery F/W DL, Start Encoder
		ret = h264_encode_init_codec();
		if( ret != H264_OK ){
			// Free MP4 Memory, and Close the file
			H264_Encode_Finalize();
			return ret;
		}
	}
	
	// Ribery Parameter Setting
	ret = h264_encode_set_param_codec( stream_ch );
	if( ret != H264_OK ){
		// Free MP4 Memory, and Close the file
		H264_Encode_Finalize();
		return ret;
	}
	
	if( gH264_Enc_Ctrl.rec_status != H264_ENC_RECORDING ){
		// Create CC, ENT task
		ret = h264_encode_task_create();
		if( ret != H264_OK ){
			OS_User_Set_Flg( FID_H264, FLG_H264_STOP );
			H264_Ribery_Finalize( H264_MODE_ENCODE );
			H264_Encode_Finalize();
			return ret;
		}
		gH264_Enc_Ctrl.rec_status = H264_ENC_RECORDING;
	}
	if( (gH264_Enc_Ctrl.vector_status == H264_OFF) &&
		(record_info->codec_param[stream_ch].vector_info_flag == H264_ON) ){
		ret = h264_encode_vector_task_create();
		if( ret != H264_OK ){
			OS_User_Set_Flg( FID_H264, FLG_H264_STOP );
			H264_Ribery_Finalize( H264_MODE_ENCODE );
			H264_Encode_Finalize();
			return ret;
		}
		gH264_Enc_Ctrl.vector_status = H264_ON;
	}
	
	return H264_OK;
}

/**
 *  @brief		Movie Encode Check Result.\n
 *	@param		None
 *	@return		End Result		End Result\n
 *								H264_OK   : Not End
 *								anything : Not End(Continue)
 *	@note		None
 *	@attention	None
 */
INT32 H264_Encode_Main_Check_End( T_H264_MOVIE_UI* record_info )
{
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn;
	OS_USER_ER			ercd;
	
	if( gH264_Enc_Ctrl.vector_status != H264_OFF ){
		waiptn = FLG_H264_ENT_STOP | FLG_H264_CC_STOP | FLG_H264_VECTOR_STOP;
	}
	else{
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
		waiptn = FLG_H264_ENT_STOP | FLG_H264_CC_STOP | FLG_H264_DISP_STOP;
#else
		waiptn = FLG_H264_ENT_STOP | FLG_H264_CC_STOP;
#endif
	}
	ercd = OS_User_Pol_Flg( FID_H264, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
	if( ercd == D_OS_USER_E_OK ){
		H264_Info_Print(("--- H264: STOP by Task End. (flg=%lx)\n",flgptn ));
		gH264_Enc_Ctrl.vector_status = H264_OFF;
		return gH264_Enc_Ctrl.task_result;
	}
	else if( ercd == D_OS_USER_E_TMOUT ) {
		;
	}
	else{
		H264_Err_Print(("--- pol_flg(6.3) ercd=%x\n", ercd ));
	}
	
	return H264_OK;
}

/**
 *  @brief		Encode Process End.\n
 *	@param		None
 *	@return		End Result		End Result\n
 *								H264_OK  : Not End
 *								H264_OFF : Not End(Continue)
 *	@note		None
 *	@attention	None
 */
VOID H264_Encode_Main_End( E_H264_IF_STREAM_CH stream_ch, T_H264_MOVIE_UI* record_info )
{
	if( gH264_Rec_Param.codec_param[stream_ch].enable_flg == H264_ON ){
		OS_User_Set_Flg( FID_H264_SYNC, gH264_Stream_Stop_Flg[stream_ch] );
		
#if 1	// This is decide customer specification. This case is self call FrameEnabled, and last frame is dummy.
		if( gH264_Rec_Param.codec_param[stream_ch].time_lapse_flag != H264_OFF ){
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u DummySync", stream_ch ));
			OS_User_Set_Flg( FID_H264_SYNC, FLG_H264_SYNC_ENC_MASTER );
			OS_User_Set_Flg( FID_H264_SYNC, gH264_Stream_TimeLapse_Sync_Flg[stream_ch] );
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
VOID H264_Encode_CC_Main( VOID )
{
	OS_USER_FLGPTN		flgptn;
	OS_USER_ER			ercd;
	INT32				ret;
	ULLONG				frame_no = 0;
	T_RB_FIFO_INDEX		vraw_index[D_H264_ENC_FIFO_REQ_COUNT];
	T_RB_INFOTABLE_VRAW	vraw_info;
	UCHAR				vraw_fifo_count = 0;
	USHORT				vop_cnt[E_H264_IF_STREAM_CH_MAX] = {0, 0, 0};
	USHORT				gop_cnt[E_H264_IF_STREAM_CH_MAX] = {1, 1, 1};
	UCHAR				first_flag[E_H264_IF_STREAM_CH_MAX] = {0, 0, 0};
	//UINT32				waitcnt[E_H264_IF_STREAM_CH_MAX] = {0, 0, 0};
	UCHAR				stop_req_flag[E_H264_IF_STREAM_CH_MAX] = {0, 0, 0};
	OS_USER_FLGPTN		waiptn;
	E_H264_IF_STREAM_CH	stream_ch;
	UINT32				free_count, free_loop;
	T_RB_FIFO_INDEX		free_index[D_H264_ENC_FIFO_REQ_COUNT];
	UINT32				free_bank[D_H264_ENC_FIFO_REQ_COUNT];
	UCHAR				detect_h264_stop = FALSE;
	T_H264_IF_MSG		msg;

	static T_RB_INFOTABLE_SCALINGLIST		scalinglist = {0, 0, 0};
	
	// Initilize control data
	H264_Movie_Memset( &vraw_info, 0, sizeof(T_RB_INFOTABLE_VRAW) );

	for( stream_ch=E_H264_IF_STREAM_CH_0; stream_ch<E_H264_IF_STREAM_CH_MAX; stream_ch++ ){
		if( gH264_Rec_Param.codec_param[stream_ch].enable_flg == H264_ON ){
			if(gH264_Rec_Param.codec_param[stream_ch].initial_scalinglist_info.scaling_list_flg >= H264_ON){
				scalinglist.pic_scaling_matrix_present_flag = RB_ON;
				scalinglist.scalinglist_Y_sel = gH264_Rec_Param.codec_param[stream_ch].initial_scalinglist_info.scaling_list_flg;
				scalinglist.scalinglist_C_sel = gH264_Rec_Param.codec_param[stream_ch].initial_scalinglist_info.scaling_list_flg;
				vraw_info.scalinglist = (T_RB_INFOTABLE_SCALINGLIST *)&scalinglist;
				break;
			}
		}
	}
	
	// Set Event for REC Task Start
	OS_User_Set_Flg( FID_H264, FLG_H264_CC_INIT_END );
	
	//wait start video process.
	waiptn = (FLG_H264_WDT | FLG_H264_FORCE_STOP | FLG_H264_STOP | FLG_H264_START);
	OS_User_Wai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	
	//Process FLG_H264_WDT & FLG_H264_FORCE_STOP  in next "CC Loop Process".
	if( 0 != (flgptn & FLG_H264_STOP) ){
		detect_h264_stop = TRUE;
		OS_User_Set_Flg( FID_H264, FLG_H264_STOP_ENT_TSK );
		H264_Info_Print(("+++ H264_CC: Request Stop\n"));
	}
	OS_User_Clr_Flg( FID_H264, ~FLG_H264_START );
	
	if (detect_h264_stop == FALSE){
		// CC Loop Process start
		while(1){
			waiptn = (FLG_H264_WDT | FLG_H264_FORCE_STOP);
			// received wdt time out msg from Ribery or Record Froce Stop
			ercd = OS_User_Pol_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			if( ercd == D_OS_USER_E_OK ){
				if( 0 != (flgptn & FLG_H264_WDT) ){
					// Received WDT T.O Event
					H264_Err_Print(("+++ H264_CC: WDT Event Received\n"));
				}
				else{ //0 != (flgptn & FLG_H264_FORCE_STOP)
					// Received Force Stop Event
					H264_Err_Print(("+++ H264_CC: Force Stop\n"));
				}
				
				waiptn = 0;
				if( gH264_Rec_Param.codec_param[E_H264_IF_STREAM_CH_0].enable_flg == H264_ON ){
					waiptn |= FLG_H264_SYNC_STOP_ENC_CH0;
				}
				if( gH264_Rec_Param.codec_param[E_H264_IF_STREAM_CH_1].enable_flg == H264_ON ){
					waiptn |= FLG_H264_SYNC_STOP_ENC_CH1;
				}
				if( gH264_Rec_Param.codec_param[E_H264_IF_STREAM_CH_2].enable_flg == H264_ON ){
					waiptn |= FLG_H264_SYNC_STOP_ENC_CH2;
				}
				
				OS_User_Wai_Flg( FID_H264_SYNC, waiptn, D_OS_USER_TWF_ANDW, &flgptn ); 
				OS_User_Clr_Flg( FID_H264_SYNC, ~flgptn );
				H264_Info_Print(("--- H264_CC: all stop request\n" ));
				break;
			}
			else if( ercd == D_OS_USER_E_TMOUT ) {
				;
			}
			else{
				H264_Err_Print(("--- H264_CC: pol_flg ercd1=%x\n", ercd ));
			}
			
			waiptn = FLG_H264_SYNC_ENC_MASTER | FLG_H264_SYNC_ENC_CH0 | FLG_H264_SYNC_ENC_CH1 | FLG_H264_SYNC_ENC_CH2;
			
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "CC E" ));
			
			OS_User_Wai_Flg( FID_H264_SYNC, waiptn, D_OS_USER_TWF_ORW, &flgptn );
			
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "CC S %x", (UINT32)flgptn));
			
			if( 0 != (flgptn & FLG_H264_SYNC_ENC_MASTER) ){
				OS_User_Clr_Flg( FID_H264_SYNC, ~FLG_H264_SYNC_ENC_MASTER );
				for( stream_ch=E_H264_IF_STREAM_CH_0; stream_ch<E_H264_IF_STREAM_CH_MAX; stream_ch++ ){
					if( gH264_Rec_Param.codec_param[stream_ch].enable_flg == H264_ON ){
						if( gH264_Map_Enable_Flg[stream_ch] == H264_ON ){
							if( (gH264_Enc_Serial_Num[stream_ch] == 0) && (first_flag[stream_ch] == H264_OFF) ){
#if 0
								if( gH264_Vin_Adr_Ctl[stream_ch].hold_max_no < 1 ){
									waitcnt[stream_ch]++;
		//							printf("waitcnt[%d][%d]\n",stream_ch, waitcnt[stream_ch] );
									if( waitcnt[stream_ch] > 1200 ){
										H264_Err_Print(("+++ H264_CC: View Task Image Update Nothing!!(Between 150VD) %d\n", stream_ch ));
										OS_User_Set_Flg( FID_H264, FLG_H264_FORCE_STOP );
										break;
									}
								}
								else{
									// "total_time" is taken as internal time. (It is from 0 after a stop.) 
									BF_Movie_Common_Sync_Start_H264( stream_ch );
									first_flag[stream_ch] = H264_ON;
								}
#else
								BF_Movie_Common_Sync_Start_H264( stream_ch );
								first_flag[stream_ch] = H264_ON;
#endif
								OS_User_Set_Flg( FID_ENCODER, D_BF_FLG_START_SYNC_H264 );
								H264_Info_Print(( "H264 sync Start ch[%d]\n", stream_ch ));
							}
						}
					}
				}
				continue;
			}
			else if( 0 != (flgptn & FLG_H264_SYNC_ENC_CH0) ){
				OS_User_Clr_Flg( FID_H264_SYNC, ~FLG_H264_SYNC_ENC_CH0 );
				stream_ch = E_H264_IF_STREAM_CH_0;
				waiptn = FLG_H264_SYNC_STOP_ENC_CH0;
			}
			else if( 0 != (flgptn & FLG_H264_SYNC_ENC_CH1) ){
				OS_User_Clr_Flg( FID_H264_SYNC, ~FLG_H264_SYNC_ENC_CH1 );
				stream_ch = E_H264_IF_STREAM_CH_1;
				waiptn = FLG_H264_SYNC_STOP_ENC_CH1;
			}
			else if( 0 != (flgptn & FLG_H264_SYNC_ENC_CH2) ){
				OS_User_Clr_Flg( FID_H264_SYNC, ~FLG_H264_SYNC_ENC_CH2 );
				stream_ch = E_H264_IF_STREAM_CH_2;
				waiptn = FLG_H264_SYNC_STOP_ENC_CH2;
			}
			else{
				H264_Err_Print(("+++ H264_CC: Sync Wait: The route which is not assumed.\n" ));
				stream_ch = E_H264_IF_STREAM_CH_0;
			}
			// VRAW_Info_Table Free
			if( vraw_fifo_count == 0 ){
				while(1){
					ret = H264_NG;
					ercd = OS_User_Twai_Sem(SID_H264, H264_ENC_SEMAPHORE_TIMEOUT);
					if( ercd != D_OS_USER_E_OK ){
						H264_Err_Print(( "--- H264_CC: Vraw semaphore NG\n" ));
						OS_User_Set_Flg( FID_H264, FLG_H264_FORCE_STOP );
					}
					ret = H264_Ribery_Enc_VRAW_Free( &free_count, &free_index[0], &free_bank[0] );
					if( (ret == H264_OK) || (ret == H264_OK_COMPLETE) ){
						for( free_loop=0; free_loop<free_count; free_loop++ ){
							h264_encode_Vin_set_empty_bank( (E_H264_IF_STREAM_CH)free_index[free_loop].ch, free_bank[free_loop] );
						}
					}
					OS_User_Sig_Sem(SID_H264);
					if( ret != H264_OK ){
						break;
					}
				}
			}
			
			// It updates vop_cnt
			vop_cnt[stream_ch]++;
			if( vop_cnt[stream_ch] > gH264_Rec_Param.codec_param[stream_ch].gop_num ){
				vop_cnt[stream_ch] = 1;
				gop_cnt[stream_ch]++;
				if( gop_cnt[stream_ch] > gH264_Rec_Param.codec_param[stream_ch].gop_num_idr_to_idr ){
					gop_cnt[stream_ch] = 1;
				}
			}
			// Check Stop Request
			if( stop_req_flag[stream_ch] == H264_OFF ){
				ercd = OS_User_Pol_Flg( FID_H264_SYNC, waiptn, D_OS_USER_TWF_ORW, &flgptn );
				if( ercd == D_OS_USER_E_OK ){
					if( gH264_Rec_Param.codec_param[stream_ch].gop_struct == E_H264_IF_GOP_STR_IBBP ){	// for IBBP struct
						if( (vop_cnt[stream_ch]%3) == 0 ){
							H264_Info_Print(("--- H264_CC: stop_req_flag stream_ch =%d\n", stream_ch));
							// recv STOP new image not store
							stop_req_flag[stream_ch] = H264_ON;
							gH264_Rec_Param.codec_param[stream_ch].enable_flg = H264_OFF;
							gH264_Map_Enable_Flg[stream_ch] = H264_OFF;
							h264_encode_ch_map_reset( stream_ch );
						}
					}
					else if( gH264_Rec_Param.codec_param[stream_ch].gop_struct == E_H264_IF_GOP_STR_IBP ){	// for IBP struct
						if( (vop_cnt[stream_ch]%2) == 0 ){
							H264_Info_Print(("--- H264_CC: stop_req_flag stream_ch =%d\n", stream_ch));
							// recv STOP new image not store
							stop_req_flag[stream_ch] = H264_ON;
							gH264_Rec_Param.codec_param[stream_ch].enable_flg = H264_OFF;
							gH264_Map_Enable_Flg[stream_ch] = H264_OFF;
							h264_encode_ch_map_reset( stream_ch );
						}
					}
					else{																// for IPPP/IIII struct
							H264_Info_Print(("--- H264_CC: stop_req_flag stream_ch =%d\n", stream_ch));
							// recv STOP new image not store
							stop_req_flag[stream_ch] = H264_ON;
							gH264_Rec_Param.codec_param[stream_ch].enable_flg = H264_OFF;
							gH264_Map_Enable_Flg[stream_ch] = H264_OFF;
							h264_encode_ch_map_reset( stream_ch );
					}
				}
				else if( ercd == D_OS_USER_E_TMOUT ){
					;
				}
				else{
					H264_Err_Print(("--- H264_CC: pol_flg ercd2=%x\n", ercd ));
				}
			}
			else{
				// This CH is allready STOP req
				continue;
			}
			
			if( vop_cnt[stream_ch] == 1 ){
				H264_If_Record_Notify_PreEncode( stream_ch );
			}
			
			// H.264 Encode process
			// Parameter Setting (VRAW_INFO getting)
			ret = h264_encode_ribery_vraw_param_set(
						stream_ch,
						&vraw_info,
						&vraw_index[vraw_fifo_count],
						&frame_no,
						 stop_req_flag[stream_ch],
						&vop_cnt[stream_ch],
						&gop_cnt[stream_ch]
						 );
			if( ret != H264_OK ){
				H264_Err_Print(("+++ h264_encode_ribery_vraw_param_set ERR\n"));
				OS_User_Set_Flg( FID_H264, FLG_H264_FORCE_STOP );
				break;
			}
			
			// Encode Request to Ribery Macro.
			ret = h264_encode_ribery_vraw_set( vraw_index[vraw_fifo_count], &vraw_info );
			if( ret != H264_OK ){
				H264_Err_Print(("+++ H264_CC: Encode Parameter Error=%d\n", ret ));
				OS_User_Set_Flg( FID_H264, FLG_H264_FORCE_STOP );
				break;
			}
			vraw_fifo_count++;
			
			ret = h264_encode_ribery_start( vraw_fifo_count, &vraw_index[0] );
			if( ret != H264_OK ){
				H264_Err_Print(("+++ H264_CC: Encode Request Error=%d\n", ret ));
				OS_User_Set_Flg( FID_H264, FLG_H264_FORCE_STOP );
				break;
			}
			vraw_fifo_count = 0;
			
			// Recording Time Update
			gH264_Enc_Ctrl.video[stream_ch].total_time_increment += gH264_Enc_Ctrl.video[stream_ch].time_increment;
			
//			printf("incr:[%d][%d]\n", stream_ch, gH264_Enc_Ctrl.video[stream_ch].total_time_increment);
			
			// Stop Complete Check
			if( (gH264_Rec_Param.codec_param[E_H264_IF_STREAM_CH_0].enable_flg != H264_ON) &&
				(gH264_Rec_Param.codec_param[E_H264_IF_STREAM_CH_1].enable_flg != H264_ON) &&
				(gH264_Rec_Param.codec_param[E_H264_IF_STREAM_CH_2].enable_flg != H264_ON) ){
				gH264_CC_Stop_Flg = H264_ON;
				break;
			}
		}	// CC loop End
	}
	
	// for Debug
	if( (gH264_ViewLater_Total_Count != 0) || (gH264_MovieLater_Total_Count != 0) ){
		H264_Warning_Print(("gH264_ViewLater_Total_Count is %d\n", gH264_ViewLater_Total_Count ));
		H264_Warning_Print(("gH264_MovieLater_Total_Count is %d\n", gH264_MovieLater_Total_Count ));
	}
	
//	H264_Info_Print(("+++ H264_CC: CC Task End Complete!!\n"));
	
	// Set CC Task Stop Event
	OS_User_Set_Flg( FID_H264, FLG_H264_CC_STOP );
	
	msg.type = E_H264_IF_TYPE_NO_REQUEST;
	ret = H264_If_Set_Msg( &msg, E_H264_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		H264_Err_Print(("--- H264_CC End Msg ERR. ret=%x\n", ret ));
	}
	
	return;
}

/**
 *  @brief		Encode ENT Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Encode_ENT_Main( VOID )
{
	INT32					ret;
	ULLONG					total_duration;
	OS_USER_FLGPTN			flgptn;
	OS_USER_FLGPTN			waiptn;
	T_H264_ENC_VIDEO_INFO	video_info;
	UINT32					sample_index;
	UINT32					info_count = 0;
	T_OS_USER_RTSK			pk_rtsk;
	OS_USER_ER				ercd;
	T_H264_IF_VIDEO_STREAM_PARAM video_stream_info;
	LLONG					pes_set_pts;
	LLONG					pes_set_dts;
	UINT32					free_count, free_loop;
	T_RB_FIFO_INDEX			free_index[D_H264_ENC_FIFO_REQ_COUNT];
	UINT32					free_bank[D_H264_ENC_FIFO_REQ_COUNT];
#ifndef CO_DEBUG_ON_PC
	UCHAR					flag_first_proc = H264_ON;
#endif
	T_H264_IF_MSG			msg;
	
	// 0. Initialize Local Data
	H264_Movie_Memset( &video_info, 0, sizeof(T_H264_ENC_VIDEO_INFO) );
	H264_Movie_Memset( &video_stream_info, 0, sizeof(T_H264_IF_VIDEO_STREAM_PARAM) );
	video_info.sample_num = 0;
	total_duration = 0;
	ret = H264_OK;
	
	// 2. Report ENT Task Start
	OS_User_Set_Flg( FID_H264, FLG_H264_ENT_INIT_END );
	
	waiptn = (FLG_H264_ENC_VSTRM_COMP | FLG_H264_WDT | FLG_H264_FORCE_STOP | FLG_H264_STOP_ENT_TSK);
	
	// 3. ENT Task Loop Process
	while(1){
		
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "ENT E" ));
		
		ercd = OS_User_Wai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		OS_User_Clr_Flg( FID_H264, ~FLG_H264_ENC_VSTRM_COMP );
		
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "ENT S:%x", (UINT32)flgptn));
		
		// Received WDT T.O Event
		if( 0 != (flgptn & FLG_H264_WDT) ){
			H264_Err_Print(("+++ H264_ENT: WDT Event Received\n"));
			// Set Record End Status
			gH264_Enc_Ctrl.task_result = H264_ERR_SYSTEM;
			h264_encode_illagal( &video_info, info_count, total_duration );
			
			return;
		}
		// Received Force Stop Event
		if( 0 != (flgptn & FLG_H264_FORCE_STOP) ){
			H264_Err_Print(("+++ H264_ENT: Force Stop\n"));
			// Set Record End Status
			gH264_Enc_Ctrl.task_result = H264_ERR_SYSTEM;
			h264_encode_illagal( &video_info, info_count, total_duration );
			
			return;
		}		
		// Received Task Stop Event
		if (0 != (flgptn & FLG_H264_STOP_ENT_TSK)){
			H264_Info_Print(("+++ H264_ENT: Task Request Stop\n"));
			break;
		}
#ifndef CO_DEBUG_ON_PC
		flag_first_proc = H264_OFF;
#endif
		// Free Video Stream
		h264_encode_free_vstream();
		
		if( gH264_CC_Stop_Flg == H264_ON ){
			OS_User_Set_Flg( FID_H264, FLG_H264_ENC_VSTRM_COMP );
			if( video_info.sample_num == info_count ){
				OS_User_Tslp_Tsk( 10 );
			}
		}
		// 3.1. Get Encoder Result
		ret = h264_encode_encoder_result_get( &video_info, &info_count );
		if( (ret != H264_OK) && (ret != H264_OK_COMPLETE) ){
			if( ret == H264_NG ){
				// No Sample
				;
			}
			else{
				// PARAM NG
				H264_Err_Print(("+++ H264_ENT: h264_encode_encoder_result_get. ret=%d\n", ret ));
			}
			continue;
		}
		
		while( video_info.sample_num < info_count ){
			video_info.sample_num++;
			sample_index = video_info.sample_num-1;

			if (gH264_Rec_Param.codec_param[video_info.stream_ch[sample_index]].delay_notify_flag == TRUE && video_info.first_param[sample_index].sample_pts > 0) {
				ret = H264_If_Record_Notify_Video_Stream( &g_video_stream_info[video_info.stream_ch[sample_index]] );
				if( !ret ){
					H264_Err_Print(("+++ H264_ENT: H264_If_Record_Notify_Video_Stream Error.ret=%d\n",ret ));

					// Set Record End Status
					gH264_Enc_Ctrl.task_result = H264_ERR_SYSTEM;
					h264_encode_illagal( &video_info, info_count, total_duration );

					return;
				}
			}

			h264_encode_stream_around_check( &video_info.first_param[sample_index] );
			
			if( gH264_Rec_Param.codec_param[ video_info.stream_ch[sample_index] ].stream_format == E_H264_IF_STREAM_FORMAT_PES ){
				// ((PTS*90KHz)/time_scale) (PTS is 90KHz)
				pes_set_pts = (((LLONG)video_info.first_param[sample_index].sample_pts * H264_PTS_90K_BASE) / gH264_Enc_Ctrl.video[ video_info.stream_ch[sample_index] ].time_scale);
				// ((DTS*90KHz)/time_scale) (DTS is 90KHz)
				pes_set_dts = (((LLONG)video_info.first_param[sample_index].sample_dts * H264_PTS_90K_BASE) / gH264_Enc_Ctrl.video[ video_info.stream_ch[sample_index] ].time_scale);
				// For making time of "H.264" into a positive value. 1sec in 90KHz terms.
				
				h264_encode_pes_header_set( video_info.stream_ch[sample_index],
											(ULONG)video_info.first_param[sample_index].sample_address,
											video_info.first_param[sample_index].sample_size,
											pes_set_pts,
											pes_set_dts );
				
				video_info.first_param[sample_index].sample_address -= D_H264_ENCODE_PES_HEADER_VIDEO_SIZE;
				video_info.first_param[sample_index].sample_size += D_H264_ENCODE_PES_HEADER_VIDEO_SIZE;
			}
			else{
				// stream_format is H264_STREAM_FORMAT_MP4
				// do nothing.
			}
			
			if( video_info.stream_end_flg[sample_index] == H264_ON ){
				H264_Info_Print(("+++ H264_ENT: [ch%lu]Received Last Stream.\n", video_info.stream_ch[sample_index] ));
				gH264_ENT_Enable_Flg[video_info.stream_ch[sample_index]] = H264_OFF;
			}
			h264_encode_video_stream_param_set( &video_info, sample_index, &video_stream_info, &total_duration );
			video_stream_info.error_flg = FALSE;

			if (gH264_Rec_Param.codec_param[video_info.stream_ch[sample_index]].delay_notify_flag == TRUE) {
				// Avoid wrap-around process delay, send previous frame info to post-process CB
				if (video_info.stream_end_flg[sample_index] == H264_ON) {
					ret = H264_If_Record_Notify_Video_Stream( &video_stream_info );
					if( !ret ){
						H264_Err_Print(("+++ H264_ENT: H264_If_Record_Notify_Video_Stream Error.ret=%d\n",ret ));

						// Set Record End Status
						gH264_Enc_Ctrl.task_result = H264_ERR_SYSTEM;
						h264_encode_illagal( &video_info, info_count, total_duration );

						return;
					}
				} else {
					memcpy(&g_video_stream_info[video_stream_info.stream_ch], &video_stream_info, sizeof(T_H264_IF_VIDEO_STREAM_PARAM));
				}
			} else {
				ret = H264_If_Record_Notify_Video_Stream( &video_stream_info );
				if( !ret ){
					H264_Err_Print(("+++ H264_ENT: H264_If_Record_Notify_Video_Stream Error.ret=%d\n",ret ));

					// Set Record End Status
					gH264_Enc_Ctrl.task_result = H264_ERR_SYSTEM;
					h264_encode_illagal( &video_info, info_count, total_duration );

					return;
				}
			}
		}
		
		// Check End of All Video Stream
		if( (gH264_ENT_Enable_Flg[E_H264_IF_STREAM_CH_0] != H264_ON) &&
			(gH264_ENT_Enable_Flg[E_H264_IF_STREAM_CH_1] != H264_ON) &&
			(gH264_ENT_Enable_Flg[E_H264_IF_STREAM_CH_2] != H264_ON) ){
#ifdef CO_DEBUG_ON_PC
				OS_User_Tslp_Tsk(100);
#endif
			break;
		}
	}
#ifndef CO_DEBUG_ON_PC
	if (flag_first_proc == H264_OFF){
		// Free all video stream
		OS_User_Twai_Flg( FID_H264, FLG_H264_ENC_FREE_ALL_VSTREAM, D_OS_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_FOREVER );
		OS_User_Clr_Flg( FID_H264, ~FLG_H264_ENC_FREE_ALL_VSTREAM );
		h264_encode_free_vstream();
	}
#endif
	
	// 4. End Process
	// last Vstream rcv end
	OS_User_Ref_Tsk( TID_H264_CC, &pk_rtsk );
	if( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
	}
	else{
		// VRAW_Info_Table Free
		while(1){
			ret = H264_Ribery_Enc_VRAW_Free( &free_count, &free_index[0], &free_bank[0] );
			if( (ret == H264_OK) || (ret == H264_OK_COMPLETE) ){
				for( free_loop=0; free_loop<free_count; free_loop++ ){
					h264_encode_Vin_set_empty_bank( (E_H264_IF_STREAM_CH)free_index[free_loop].ch, free_bank[free_loop] );
				}
			}
			if( ret != H264_OK ) { break; }
		}
	}
	
	gH264_Enc_Ctrl.task_result = H264_OK_NORMAL_END;
	
	waiptn = FLG_H264_CC_STOP;
	ercd = OS_User_Twai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ANDW, &flgptn, 1000 );
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(("+++ H264_ENT End: Time out ercd=%x\n", ercd ));
	}
	
	// ENT Task End Process
	h264_encode_result_notify( gH264_Enc_Ctrl.task_result );
	H264_Ribery_Finalize( H264_MODE_ENCODE );
	
//	H264_Info_Print(("+++ H264_ENT: ENT Task End Complete!!\n"));
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "ENT" ));
	
	// 4. Set ENT Task Stop Event 
	OS_User_Set_Flg( FID_H264, FLG_H264_ENT_STOP );
	
	msg.type = E_H264_IF_TYPE_NO_REQUEST;
	ret = H264_If_Set_Msg( &msg, E_H264_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		H264_Err_Print(("--- H264_ENT End Msg ERR. ret=%x\n", ret ));
	}
	
	return;
}

/**
 *  @brief		Encode VECTOR Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Encode_VECTOR_Main( VOID )
{
	T_RB_INFOTABLE_Y_VEC rb_vector_info;
	T_H264_IF_VIDEO_VECTOR_PARAM vector_info;
	INT32 ret;
	UINT32 index;
	OS_USER_FLGPTN	flgptn;
	OS_USER_FLGPTN	waiptn;
	T_H264_IF_MSG	msg;
//	ULLONG offset;
	
	// 0. Initialize Local Data
	H264_Movie_Memset( &vector_info, 0, sizeof(T_H264_IF_VIDEO_VECTOR_PARAM) );
	
	waiptn = (FLG_H264_ENC_Y_VEC_COMP | FLG_H264_WDT | FLG_H264_FORCE_STOP | FLG_H264_ENT_STOP );
	
	while(1){
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "Vec" ));
		
		OS_User_Wai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		if( (0 != (flgptn & FLG_H264_WDT)) || (0 != (flgptn & FLG_H264_FORCE_STOP)) || (0 != (flgptn & FLG_H264_ENT_STOP)) ){
			// Vector Task stop
			break;
		}
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "Vec" ));
		
		OS_User_Clr_Flg( FID_H264, ~FLG_H264_ENC_Y_VEC_COMP );
		
		while(1){
			ret = H264_Ribery_Enc_Y_VEC_Get( &index, &rb_vector_info );
			if( ret == H264_NG ){
				break;
			}
			
			vector_info.stream_ch		= rb_vector_info.ch;
			vector_info.frame_no		= rb_vector_info.vraw_serial_num;
			vector_info.sample_type		= rb_vector_info.pic_type;
			vector_info.vector_addr		= (UINT32*)rb_vector_info.vec_info_bank_addr;
			vector_info.brightness_addr	= (UINT32*)rb_vector_info.y_info_bank_addr;
			
			// 360KHz -> 90KHz
			vector_info.sample_pts = rb_vector_info.PTS * H264_PTS_90K_BASE / H264_PTS_BASE;
			vector_info.sample_dts = rb_vector_info.DTS * H264_PTS_90K_BASE / H264_PTS_BASE;
			
#ifdef CO_MOVIE_DSC_MODE
			//*********  DTS of Top field/frame to unify to 0  **********//
			// Ribery put out Top frame/field DTS is follows.
			//  IBBP Interlace   = (-)time increment*2
			//  IBBP Progressive = (-)time increment
			//  IPPP Interlace   = 0
			//  IPPP Progressice = 0
			if( gH264_Rec_Param.codec_param[rb_vector_info.ch].gop_struct == E_H264_IF_GOP_STR_IBBP ){
				if( gH264_Rec_Param.scan_mode == E_H264_IF_SCAN_MODE_INTERLACE ){
					vector_info.sample_pts += (H264_PTS_90K_BASE / gH264_Enc_Ctrl.video[rb_vector_info.ch].time_scale) * (gH264_Enc_Ctrl.video[rb_vector_info.ch].time_increment*2);
					vector_info.sample_dts += (H264_PTS_90K_BASE / gH264_Enc_Ctrl.video[rb_vector_info.ch].time_scale) * (gH264_Enc_Ctrl.video[rb_vector_info.ch].time_increment*2);
				}
				else{
					vector_info.sample_pts += (H264_PTS_90K_BASE / gH264_Enc_Ctrl.video[rb_vector_info.ch].time_scale) * gH264_Enc_Ctrl.video[rb_vector_info.ch].time_increment;
					vector_info.sample_dts += (H264_PTS_90K_BASE / gH264_Enc_Ctrl.video[rb_vector_info.ch].time_scale) * gH264_Enc_Ctrl.video[rb_vector_info.ch].time_increment;
				}
			}
#endif
			
			vector_info.bitrate		= rb_vector_info.bitrate;
			vector_info.ave_bitrate	= rb_vector_info.avebitrate;
			
			vector_info.gop_struct = gH264_Rec_Param.codec_param[rb_vector_info.ch].gop_struct;
			vector_info.gop_num = gH264_Rec_Param.codec_param[rb_vector_info.ch].gop_num;
			switch( gH264_Rec_Param.codec_param[rb_vector_info.ch].size ){
				case E_H264_IF_IMAGE_SIZE_4096_2304 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_4096_2304;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_4096_2304;
					vector_info.width = 4096;
					vector_info.lines = 2304;
					break;
				case E_H264_IF_IMAGE_SIZE_4096_2160 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_4096_2160;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_4096_2160;
					vector_info.width = 4096;
					vector_info.lines = 2160;
					break;
				case E_H264_IF_IMAGE_SIZE_4096_2048 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_4096_2048;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_4096_2048;
					vector_info.width = 4096;
					vector_info.lines = 2048;
					break;
				case E_H264_IF_IMAGE_SIZE_3840_2160 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_3840_2160;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_3840_2160;
					vector_info.width = 3840;
					vector_info.lines = 2160;
					break;
                case E_H264_IF_IMAGE_SIZE_3840_1920 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_3840_1920;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_3840_1920;
					vector_info.width = 3840;
					vector_info.lines = 1920;
					break;
				case E_H264_IF_IMAGE_SIZE_3000_3000 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_3000_3000;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_3000_3000;
					vector_info.width = 3008;
					vector_info.lines = 3008;
					break;					
				case E_H264_IF_IMAGE_SIZE_2704_2028 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_2704_2028;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_2704_2028;
					vector_info.width = 2704;
					vector_info.lines = 2028;
					break;
				case E_H264_IF_IMAGE_SIZE_2704_1520 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_2704_1520;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_2704_1520;
					vector_info.width = 2704;
					vector_info.lines = 1520;
					break;
				case E_H264_IF_IMAGE_SIZE_2560_1440 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_2560_1440;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_2560_1440;
					vector_info.width = 2560;
					vector_info.lines = 1440;
					break;
				case E_H264_IF_IMAGE_SIZE_1920_1440 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1920_1440;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1920_1440;
					vector_info.width = 1920;
					vector_info.lines = 1440;
					break;
				case E_H264_IF_IMAGE_SIZE_1920_1080 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1920_1080;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1920_1080;
					vector_info.width = WIDTH_FHD;
					vector_info.lines = LINES_FHD;
					break;
				case E_H264_IF_IMAGE_SIZE_1920_960 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1920_960;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1920_960;
					vector_info.width = 1920;
					vector_info.lines = 960;
					break;
				case E_H264_IF_IMAGE_SIZE_1440_1080 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1440_1080;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1440_1080;
					vector_info.width = WIDTH_1440;
					vector_info.lines = LINES_1440;
					break;
				case E_H264_IF_IMAGE_SIZE_1440_720 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1440_720;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1440_720;
					vector_info.width = WIDTH_1440;
					vector_info.lines = 720;
					break;
				case E_H264_IF_IMAGE_SIZE_1280_1024 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1280_1024;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1280_1024;
					vector_info.width = 1280;
					vector_info.lines = 1024;
					break;
				case E_H264_IF_IMAGE_SIZE_1280_960 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1280_960;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1280_960;
					vector_info.width = 1280;
					vector_info.lines = 960;
					break;
				case E_H264_IF_IMAGE_SIZE_1280_720 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1280_720;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1280_720;
					vector_info.width = 1280;
					vector_info.lines = 720;
					break;
				case E_H264_IF_IMAGE_SIZE_960_540 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_960_540;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_960_540;
					vector_info.width = 960;
					vector_info.lines = 540;
					break;
				case E_H264_IF_IMAGE_SIZE_1280_768 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1280_768;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1280_768;
					vector_info.width = 1280;
					vector_info.lines = 768;
					break;
				case E_H264_IF_IMAGE_SIZE_1024_768 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1024_768;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1024_768;
					vector_info.width = 1024;
					vector_info.lines = 768;
					break;
				case E_H264_IF_IMAGE_SIZE_960_480 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_960_480;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_960_480;
					vector_info.width = 960;
					vector_info.lines = 480;
					break;
				case E_H264_IF_IMAGE_SIZE_800_600 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_800_600;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_800_600;
					vector_info.width = 800;
					vector_info.lines = 600;
				case E_H264_IF_IMAGE_SIZE_864_480 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_864_480;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_864_480;
					vector_info.width = 864;
					vector_info.lines = 480;
					break;
				case E_H264_IF_IMAGE_SIZE_848_480 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_848_480;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_848_480;
					vector_info.width = 848;
					vector_info.lines = 480;
					break;
				case E_H264_IF_IMAGE_SIZE_720_576 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_720_576;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_720_576;
					vector_info.width = 720;
					vector_info.lines = 576;
					break;
				case E_H264_IF_IMAGE_SIZE_720_480 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_720_480;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_720_480;
					vector_info.width = 720;
					vector_info.lines = 480;
					break;
				case E_H264_IF_IMAGE_SIZE_640_480 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_640_480;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_640_480;
					vector_info.width = WIDTH_VGA;
					vector_info.lines = LINES_VGA;
					break;
				case E_H264_IF_IMAGE_SIZE_480_270 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_480_270;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_480_270;
					vector_info.width = 480;
					vector_info.lines = 270;
					break;
				case E_H264_IF_IMAGE_SIZE_640_360 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_640_360;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_640_360;
					vector_info.width = WIDTH_HVGAW;
					vector_info.lines = LINES_HVGAW;
					break;
				case E_H264_IF_IMAGE_SIZE_432_240 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_432_240;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_432_240;
					vector_info.width = 432;
					vector_info.lines = 240;
					break;
				case E_H264_IF_IMAGE_SIZE_320_240 :
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_320_240;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_320_240;
					vector_info.width = WIDTH_QVGA;
					vector_info.lines = LINES_QVGA;
					break;
				case E_H264_IF_IMAGE_SIZE_1600_896:
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1600_896;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1600_896;
					vector_info.width = 1600;
					vector_info.lines = 896;
					break;
				case E_H264_IF_IMAGE_SIZE_1024_576:
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1024_576;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1024_576;
					vector_info.width = 1024;
					vector_info.lines = 576;
					break;
				case E_H264_IF_IMAGE_SIZE_800_448:
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_800_448;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_800_448;
					vector_info.width = 800;
					vector_info.lines = 448;
					break;
				case E_H264_IF_IMAGE_SIZE_352_288:
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_352_288;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_352_288;
					vector_info.width = 352;
					vector_info.lines = 288;
					break;
				case E_H264_IF_IMAGE_SIZE_2592_1944:
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_2592_1944;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_2592_1944;
					vector_info.width = 2592;
					vector_info.lines = 1944;
					break;
				case E_H264_IF_IMAGE_SIZE_2304_1296:
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_2304_1296;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_2304_1296;
					vector_info.width = 2304;
					vector_info.lines = 1296;
					break;
				case E_H264_IF_IMAGE_SIZE_1920_1280:
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_1920_1280;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_1920_1280;
					vector_info.width = 1920;
					vector_info.lines = 1280;
					break;
				case E_H264_IF_IMAGE_SIZE_640_512:
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_640_512;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_640_512;
					vector_info.width = 640;
					vector_info.lines = 512;
					break;
				case E_H264_IF_IMAGE_SIZE_384_288:
					vector_info.vector_size = D_H264_ENCODE_VECTOR_SIZE_384_288;
					vector_info.brightness_size = D_H264_ENCODE_BRIGHTNESS_SIZE_384_288;
					vector_info.width = 384;
					vector_info.lines = 288;
					break;
				default :
					H264_Err_Print(("--- Size Error!! size=%d\n", gH264_Rec_Param.codec_param[rb_vector_info.ch].size ));
					break;
			}
			H264_If_Record_Notify_Vector_Info(&vector_info);
			if( ret == H264_OK_COMPLETE ){
				break;
			}
		}
	}
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "Vec" ));
	
//	H264_Info_Print(("+++ H264_VECTOR: Task End Complete!!\n"));
	OS_User_Set_Flg( FID_H264, FLG_H264_VECTOR_STOP );
	
	msg.type = E_H264_IF_TYPE_NO_REQUEST;
	ret = H264_If_Set_Msg( &msg, E_H264_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		H264_Err_Print(("--- H264_VECTOR End Msg ERR. ret=%x\n", ret ));
	}
	
	return;
}

/**
 *	@brief		H264 Encode Initilize control data.\n
 *				And then this method will check media size.\n
 *				1) Global Data\n
 *				2) SDRAM\n
 *				3) MEDIA vacant area check.\n
 *	@param		record_info						Record Condition\n
 *	@return		H264_OK						(0) Normal End
 *	@return 	H264_ERR_MEDIA_AREA_NOTHING	(2) MEDIA Size Error
 *	@note		None
 *	@attention	None
 */
INT32 H264_Encode_Initialize( E_H264_IF_STREAM_CH stream_ch, T_H264_MOVIE_UI* record_info )
{
	INT32 ret;
	
	if( record_info == NULL ){
		return H264_ERR_SYSTEM;
	}
	
	if( gH264_Enc_Ctrl.rec_status != H264_ENC_RECORDING ){
// --- 0. Initialize Event Flag ---
		OS_User_Clr_Flg( FID_H264, 0x0000 );
		OS_User_Clr_Flg( FID_H264_SYNC, 0x0000 );
		
// --- 1. Initialize Global Data ---
		// gH264_Rec_Param
		gH264_Rec_Param = *record_info;
		
		// gH264_Enc_Ctrl
		gH264_Enc_Ctrl.task_result		= H264_OK;
		gH264_Enc_Ctrl.vector_status	= H264_OFF;
		
		// Global Reset
		H264_Movie_Memset( (VOID*)gH264_Enc_Serial_Num, 0, sizeof(gH264_Enc_Serial_Num) );
		
		gH264_CC_Stop_Flg = H264_OFF;
		gH264_Stream_Buffer0_Limit = Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_End_Address() - Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_Start_Address();
		gH264_Stream_Buffer1_Limit = Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_End_Address() - Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_Start_Address();
		
		gH264_ViewLater_Count = 0;
		gH264_MovieLater_Count = 0;
		gH264_ViewLater_Total_Count = 0;
		gH264_MovieLater_Total_Count = 0;
		gH264_Free_Vstream_End_Flg[E_H264_IF_STREAM_CH_0] = H264_ON;
		gH264_Free_Vstream_End_Flg[E_H264_IF_STREAM_CH_1] = H264_ON;
		gH264_Free_Vstream_End_Flg[E_H264_IF_STREAM_CH_2] = H264_ON;
		gH264_Free_Vstream_Count = 0;
		H264_Movie_Memset( (VOID*)&gH264_Free_Vstream_Param[0], 0, sizeof(T_RB_FIFO_INDEX)*RB_VSTRM_INFO_TABLE_NUM );
		
		gH264_Dynamic_Change_Flag[E_H264_IF_STREAM_CH_0] = H264_OFF;
		gH264_Dynamic_Change_Flag[E_H264_IF_STREAM_CH_1] = H264_OFF;
		gH264_Dynamic_Change_Flag[E_H264_IF_STREAM_CH_2] = H264_OFF;

		gH264_Insert_IDR_Flag[E_H264_IF_STREAM_CH_0] = H264_OFF;
		gH264_Insert_IDR_Flag[E_H264_IF_STREAM_CH_1] = H264_OFF;
		gH264_Insert_IDR_Flag[E_H264_IF_STREAM_CH_2] = H264_OFF;

		gH264_First_IDR_Flag[E_H264_IF_STREAM_CH_0] = FALSE;
		gH264_First_IDR_Flag[E_H264_IF_STREAM_CH_1] = FALSE;
		gH264_First_IDR_Flag[E_H264_IF_STREAM_CH_2] = FALSE;
	}
	else{
		gH264_Rec_Param.codec_param[stream_ch] = record_info->codec_param[stream_ch];
	}
	
	// video
	switch( gH264_Rec_Param.codec_param[stream_ch].frame_rate ){
		case E_H264_IF_FRAME_RATE_239_76 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_239_76);
			break;
		case E_H264_IF_FRAME_RATE_119_88 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_119_88);
			break;
		case E_H264_IF_FRAME_RATE_59_94 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_59_94);
			break;
		case E_H264_IF_FRAME_RATE_47_95 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_47_95);
			break;
		case E_H264_IF_FRAME_RATE_29_97 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_29_97);
			break;
		case E_H264_IF_FRAME_RATE_23_97 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_23_97);
			break;
		case E_H264_IF_FRAME_RATE_14_98 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_14_98);
			break;
		case E_H264_IF_FRAME_RATE_9_99 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_9_99);
			break;
		case E_H264_IF_FRAME_RATE_4_99 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_4_99);
			break;
		case E_H264_IF_FRAME_RATE_2_99 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_2_99);
			break;
		case E_H264_IF_FRAME_RATE_1_99 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_1_99);
			break;
		case E_H264_IF_FRAME_RATE_0_99 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_0_99);
			break;
		
		case E_H264_IF_FRAME_RATE_240_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_240_0);
			break;
		case E_H264_IF_FRAME_RATE_120_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_120_0);
			break;
		case E_H264_IF_FRAME_RATE_60_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_60_0);
			break;
		case E_H264_IF_FRAME_RATE_50_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_50_0);
			break;
		case E_H264_IF_FRAME_RATE_48_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_48_0);
			break;
		case E_H264_IF_FRAME_RATE_30_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_30_0);
			break;
		case E_H264_IF_FRAME_RATE_25_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_25_0);
			break;
		case E_H264_IF_FRAME_RATE_24_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_24_0);
			break;
		case E_H264_IF_FRAME_RATE_15_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_15_0);
			break;
		case E_H264_IF_FRAME_RATE_12_5 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_12_5);
			break;
		case E_H264_IF_FRAME_RATE_10_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_10_0);
			break;
		case E_H264_IF_FRAME_RATE_7_5 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_7_5);
			break;
		case E_H264_IF_FRAME_RATE_5_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_5_0);
			break;
		case E_H264_IF_FRAME_RATE_3_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_3_0);
			break;
		case E_H264_IF_FRAME_RATE_2_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_2_0);
			break;
		case E_H264_IF_FRAME_RATE_1_0 :
			gH264_Enc_Ctrl.video[stream_ch].coefficient = ((DOUBLE)H264_PTS_BASE/H264_TIMERESO_1_0);
			break;
		
		default :
			H264_Err_Print(( "H264_Encode_Initialize: frame_rate NG\n" ));
			return H264_ERR_INPUT_PARAM;
	}
	
	// Set Video track information
	ret = H264_Movie_Convert_Framerate( gH264_Rec_Param.codec_param[stream_ch].frame_rate, (ULONG*)&gH264_Enc_Ctrl.video[stream_ch].time_increment, (ULONG*)&gH264_Enc_Ctrl.video[stream_ch].time_scale );
	if( ret != H264_OK ){
		H264_Err_Print(("+++ H264_Movie_Convert_Framerate: NG!!\n"));
		return H264_ERR_INPUT_PARAM;
	}
	gH264_Enc_Ctrl.video[stream_ch].total_time_increment	= 0;
	
	gH264_Free_Vstream_End_Flg[stream_ch] = H264_OFF;
	gH264_ENT_Enable_Flg[stream_ch] = H264_ON;
	
	return H264_OK;
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
 *	@return		H264_OK					(0) Normal End
 *	@return		H264_ERR_INPUT_PARAM	(1) Input Parameter Error
 *	@return		H264_ERR_SYSTEM			(99) System Error
 *	@note		None
 *	@attention	None
 */
INT32 H264_Encode_Image_Address_Get( E_H264_IF_STREAM_CH stream_ch, E_H264_IF_IMAGE_SIZE size, ULLONG frame_no, T_H264_IF_GET_IMAGE_ADDR_PARAM* addr_param )
{
	INT32 ret;
	
	addr_param->dummy_flg = FALSE;
	
	if( gH264_Map_Enable_Flg[stream_ch] != H264_ON ){
		Sdram_Map_Movie_Record_Get_H264_Vin_Dummy( stream_ch, size, addr_param );
//		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-%llu-%u Vget", stream_ch, frame_no, *dummy_flg ));
		return H264_OK;
	}
	
	ret = h264_encode_Vin_get_address( stream_ch, frame_no, size, addr_param );
	if( ret != H264_OK ){
		addr_param->dummy_flg = TRUE;
	}
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-%llu-%u Vget", stream_ch, frame_no, *dummy_flg ));
	return H264_OK;
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
VOID H264_Encode_Image_Address_Decide( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg )
{
	if( !dummy_flg ){
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-%llu-%u Vdec", stream_ch, frame_no, dummy_flg ));
		h264_encode_Vin_decide_address( stream_ch, frame_no );
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
VOID H264_Encode_Frame_Enabled( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg, BOOL enabled )
{
	INT32			ret = 0;
	INT32			erace_bank;
	ULLONG			erace_frame_no;
	
	// If timelapse flag off, nothing special.
	if( (gH264_Rec_Param.codec_param[stream_ch].time_lapse_flag != H264_OFF) && (gH264_Map_Enable_Flg[stream_ch] == H264_ON) ){
		if( !dummy_flg ){
			if( enabled ){
				// TRUE
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-%llu-%u Venb", stream_ch, frame_no, dummy_flg ));
				OS_User_Set_Flg( FID_H264_SYNC, FLG_H264_SYNC_ENC_MASTER );
				OS_User_Set_Flg( FID_H264_SYNC, gH264_Stream_TimeLapse_Sync_Flg[stream_ch] );
			}
			else{
				// FALSE
				// Free to unnecessary Bank 
				OS_User_Wai_Sem(SID_H264);
//				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-%llu-%u Vfal", stream_ch, frame_no, dummy_flg ));
				ret = h264_encode_Vin_get_decide_bank( stream_ch, &erace_bank, &erace_frame_no );
				if( ret == H264_ENC_VIN_CTL_NONEDATA ){
					H264_Err_Print(( "H264 Enabled frame nondata\n" ));
					OS_User_Sig_Sem(SID_H264);
					return;
				}
				if( erace_frame_no != frame_no ){
//					H264_Err_Print(( "H264 Enabled is Fraem Overlap\n" ));  // frame no meaningless?
				}
				h264_encode_Vin_set_empty_bank( stream_ch, erace_bank );
				OS_User_Sig_Sem(SID_H264);
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
VOID H264_Encode_Finalize( VOID )
{
	gH264_Map_Enable_Flg[E_H264_IF_STREAM_CH_0] = H264_OFF;
	gH264_Map_Enable_Flg[E_H264_IF_STREAM_CH_1] = H264_OFF;
	gH264_Map_Enable_Flg[E_H264_IF_STREAM_CH_2] = H264_OFF;
	gH264_Enc_Ctrl.vector_status = H264_OFF;
	gH264_Enc_Ctrl.rec_status = H264_ENC_STOP_END;
	return;
}

/**
 *  @brief		Free video stream.
 *	@param		i:vstream_free_info		Free video stream information
 *	@return		H264_OK	(0)Normal End
 *	@return		H264_NG	(1)Error
 *	@note		None
 *	@attention	None
 */
INT32 H264_Encode_Free_Video_Index( E_H264_IF_STREAM_CH stream_ch, UINT32 free_vstream_index, BOOL last_free_flg )
{
	OS_USER_FLGPTN		flgptn ;
	
	OS_User_Wai_Sem(SID_H264_STREAM);
	gH264_Free_Vstream_Param[gH264_Free_Vstream_Count].ch = stream_ch;
	gH264_Free_Vstream_Param[gH264_Free_Vstream_Count].index = free_vstream_index;
	if( last_free_flg == H264_ON ){
		gH264_Free_Vstream_End_Flg[stream_ch] = H264_ON;
	}
	gH264_Free_Vstream_Count++;
	if( gH264_Free_Vstream_Count >= RB_VSTRM_INFO_TABLE_NUM ){
		H264_Err_Print(("--- H264 VSTRM free table overflow\n" ));
	}
	OS_User_Sig_Sem(SID_H264_STREAM);
	
	if( (gH264_Free_Vstream_End_Flg[E_H264_IF_STREAM_CH_0] == H264_ON) &&
		(gH264_Free_Vstream_End_Flg[E_H264_IF_STREAM_CH_1] == H264_ON) &&
		(gH264_Free_Vstream_End_Flg[E_H264_IF_STREAM_CH_2] == H264_ON) ){
		// All VSTRM Table Free Complete 
		OS_User_Set_Flg( FID_H264, FLG_H264_ENC_FREE_ALL_VSTREAM );
		
		// wait all task stop
		OS_User_Wai_Flg( FID_H264, FLG_H264_CONTROLLER_STOP, D_OS_USER_TWF_ORW, &flgptn );
		H264_If_Record_Notify_STOP_COMPLETE();
	}
	
	return H264_OK;
}

/**
 *	@brief		Encode Task Create.\n
 *	@param		None
 *	@return		H264_OK				(0)  Normal End
 *	@return 	H264_ERR_SYSTEM		(99) It Cannot create task
 *	@note		None
 *	@attention	None
 */
static INT32 h264_encode_task_create( VOID )
{
	ULONG				frameCnt = 0;
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn;
	OS_USER_ER			ercd;
	
	// Video CC Task (create file)
	h264_encode_task_start( TID_H264_CC );
	// Video ENT Task
	h264_encode_task_start( TID_H264_ENT );
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
	// Video DISP Task (Not used on Record Mode)
	h264_encode_task_start( TID_H264_DISP );
#endif
	// Confirm All of Task Status
	waiptn = FLG_H264_ENT_INIT_END | FLG_H264_CC_INIT_END;
	frameCnt = 0;
	
	while(1){
		OS_User_Tslp_Tsk( 5 );
		// Check Task Status
		ercd = OS_User_Pol_Flg( FID_H264, waiptn, D_OS_USER_TWF_ANDW, &flgptn );
		if( ercd == D_OS_USER_E_OK ) {
			break;
		}
		else if( ercd == D_OS_USER_E_TMOUT ) {
			if( frameCnt > 400 ){
				H264_Err_Print(("--- H264 It cannot change task status!! flgptn=%lx\n", flgptn ));
				OS_User_Ter_Tsk( TID_H264_ENT );
				OS_User_Ter_Tsk( TID_H264_CC );
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
				OS_User_Ter_Tsk( TID_H264_DISP );
#endif
				return H264_ERR_SYSTEM;
			}
		}
		else{
			H264_Err_Print(("--- H264 Wait task start. pol_flg ercd=%x\n", ercd ));
			OS_User_Ter_Tsk( TID_H264_ENT );
			OS_User_Ter_Tsk( TID_H264_CC );
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
			OS_User_Ter_Tsk( TID_H264_DISP );
#endif
			return H264_ERR_SYSTEM;
		}
		frameCnt++;
	}
	// Check Task Result Status before Recording
	waiptn = FLG_H264_ENT_STOP | FLG_H264_CC_STOP;
	
	ercd = OS_User_Pol_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	if( ercd == D_OS_USER_E_OK ) {
		H264_Err_Print(("--- H264 CAN'T RECORD by TASK. flg=%lx\n", flgptn ));
		OS_User_Ter_Tsk( TID_H264_ENT );
		OS_User_Ter_Tsk( TID_H264_CC );
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
		OS_User_Ter_Tsk( TID_H264_DISP );
#endif
		return H264_ERR_SYSTEM;
	}
	
	return H264_OK;
}

/**
 *	@brief		Encode Vector Task Create.\n
 *	@param		None
 *	@return		H264_OK				(0)  Normal End
 *	@return 	H264_ERR_SYSTEM		(99) It Cannot create task
 *	@note		None
 *	@attention	None
 */
static INT32 h264_encode_vector_task_create( VOID )
{
	OS_USER_FLGPTN		flgptn;
	OS_USER_FLGPTN		waiptn;
	OS_USER_ER			ercd;
	
	//Video VECTOR Task
	h264_encode_task_start( TID_H264_VECTOR );
	
	// Check Task Result Status before Recording
	waiptn = FLG_H264_VECTOR_STOP;
	
	ercd = OS_User_Pol_Flg( FID_H264, waiptn, D_OS_USER_TWF_ORW, &flgptn );
	if( ercd == D_OS_USER_E_OK ) {
		H264_Err_Print(("--- H264 CAN'T RECORD by Vecotr TASK. flg=%lx\n", flgptn ));
		OS_User_Ter_Tsk( TID_H264_VECTOR );
		return H264_ERR_SYSTEM;
	}
	
	return H264_OK;
}

/**
 *	@brief		Initialize codec.\n
 *	@param		None
 *	@return		H264_OK					(0) Normal End
 *	@return		H264_ERR_MACRO			(5) Macro Error
 *	@note		None
 *	@attention	None
 */
static INT32 h264_encode_init_codec( VOID )
{
	INT32						ret;
	UINT32						core_mode;
	E_H264_IF_STREAM_CH			stream_ch;
	T_H264_RIBERY_BRIDGE_COMMON	bridge_common;
	T_H264_RIBERY_BRIDGE_PORT	bridge_port;
	
	memset( &bridge_common, 0, sizeof(bridge_common) );
	
	H264_Movie_Ribery_Core_Mode_Get( &core_mode );
	
	// RIBERY macro (DL & ResetCancelation)
	// set bridge param
	bridge_common.ch0_top_addr	= SDRAM_ADR_CH0_TOP;
	bridge_common.ch1_top_addr	= SDRAM_ADR_CH1_TOP;
	bridge_common.strm_bank		= Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_End_Address() - Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_Start_Address() + 8;
	
	// Common
	// Ribery_Work0 CH
	if(      H264_Movie_Sdram_Ch_Check( Sdram_Map_Movie_Record_Get_H264_Ribery_Work0() ) == SDRAM_ADR_CH0_TOP ){
		bridge_common.common_ch.work0_ch	= RB_SDRAM_CH0;
	}
	else{
		bridge_common.common_ch.work0_ch	= RB_SDRAM_CH1;
	}
	// Ribery_Work1 CH
	if(      H264_Movie_Sdram_Ch_Check( Sdram_Map_Movie_Record_Get_H264_Ribery_Work1() ) == SDRAM_ADR_CH0_TOP ){
		bridge_common.common_ch.work1_ch	= RB_SDRAM_CH0;
	}
	else{
		bridge_common.common_ch.work1_ch	= RB_SDRAM_CH1;
	}
	// Ribery_Work2 CH
	if(      H264_Movie_Sdram_Ch_Check( Sdram_Map_Movie_Record_Get_H264_Ribery_Work2() ) == SDRAM_ADR_CH0_TOP ){
		bridge_common.common_ch.work2_ch	= RB_SDRAM_CH0;
	}
	else{
		bridge_common.common_ch.work2_ch	= RB_SDRAM_CH1;
	}
	// Ribery_Stream CH
	if(      H264_Movie_Sdram_Ch_Check( Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_Start_Address() ) == SDRAM_ADR_CH0_TOP ){
		bridge_common.common_ch.stream_ch	= RB_SDRAM_CH0;
	}
	else{
		bridge_common.common_ch.stream_ch	= RB_SDRAM_CH1;
	}
	bridge_common.common_addr.work0_addr	= Sdram_Map_Movie_Record_Get_H264_Ribery_Work0();
	bridge_common.common_addr.work1_addr	= Sdram_Map_Movie_Record_Get_H264_Ribery_Work1();
	bridge_common.common_addr.work2_addr	= Sdram_Map_Movie_Record_Get_H264_Ribery_Work2();
	bridge_common.common_addr.stream_addr	= Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_Start_Address();
	
	// Port
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
	
	ret = H264_Ribery_Initialize( H264_MODE_ENCODE, core_mode, &bridge_common, &bridge_port );
	if( ret != H264_OK ){
		H264_Err_Print(("--- Ribery Enc Initialize Err.\n"));
		return H264_NG;
	}
	
	// Information table and FIFO for Ribery
	H264_Ribery_Enc_Vtable_Init();
	
	// FIFO table init
	for( stream_ch=E_H264_IF_STREAM_CH_0; stream_ch<E_H264_IF_STREAM_CH_MAX; stream_ch++ ){
		H264_Ribery_Enc_Vraw_Empty_Init( stream_ch, stream_ch*RB_MAP_BNK_ENC_YCC );
	}
	
	// Initialize Vin Address control Data
	h264_encode_Vin_initialize();
	
	return H264_OK;
}

/**
 *	@brief		Initialize codec Param.\n
 *	@param		i:stream_ch				Stream CH
 *	@return		H264_OK					(0) Normal End
 *	@return		H264_NG					(-1) Macro Error
 *	@note		None
 *	@attention	None
 */
static INT32 h264_encode_set_param_codec( E_H264_IF_STREAM_CH stream_ch )
{
	INT32		ret;
	T_H264_RIBERY_ENC_PARAM			param;
	T_H264_RIBERY_ENC_VUI			vui;
	T_H264_RIBERY_ENC_SCALINGLIST	scalinglist;
	T_H264_RIBERY_BRIDGE_YC			bridge_yc;
	
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_YC			yc;
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_LOCALDEC	local_dec;
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_REDUCTION	reduction;
	
	if( gH264_Rec_Param.scan_mode == E_H264_IF_SCAN_MODE_INTERLACE ){
		bridge_yc.tb_flag = H264_ON;
	}
	else{
		bridge_yc.tb_flag = H264_OFF;
	}
	
	if( gH264_Rec_Param.codec_param[stream_ch].enable_flg == H264_ON ){
		Sdram_Map_Movie_Record_Get_H264_Map_YC_Information( stream_ch, &yc );
		
		// Ribery YC Size
		bridge_yc.map_yc_size.width		= yc.g_width;
		bridge_yc.map_yc_size.y_bank	= yc.y_bank;
		bridge_yc.map_yc_size.c_bank	= yc.c_bank;
		
		// Ribery YC Address
		bridge_yc.map_yc_addr.y_addr	= yc.addr[0].y;
		bridge_yc.map_yc_addr.c_addr	= yc.addr[0].c;
		
		// Ribery YC CH
		if( H264_Movie_Sdram_Ch_Check( bridge_yc.map_yc_addr.y_addr ) == SDRAM_ADR_CH0_TOP ){
			bridge_yc.map_yc_ch.y_ch	= RB_SDRAM_CH0;
		}
		else{
			bridge_yc.map_yc_ch.y_ch	= RB_SDRAM_CH1;
		}
		if( H264_Movie_Sdram_Ch_Check( bridge_yc.map_yc_addr.c_addr ) == SDRAM_ADR_CH0_TOP ){
			bridge_yc.map_yc_ch.c_ch	= RB_SDRAM_CH0;
		}
		else{
			bridge_yc.map_yc_ch.c_ch	= RB_SDRAM_CH1;
		}
		
		
		Sdram_Map_Movie_Record_Get_H264_Map_Local_Dec_Information( stream_ch, &local_dec );
		
		// Ribery Local Decode Area Size
		bridge_yc.map_yc_size.localdec_width	= local_dec.g_width;
		bridge_yc.map_yc_size.localdec_y_bank	= local_dec.y_bank;
		bridge_yc.map_yc_size.localdec_c_bank	= local_dec.c_bank;
		
		// Ribery Local Decode Area Address
		bridge_yc.map_yc_addr.localdec_y_addr	= local_dec.y_addr;
		bridge_yc.map_yc_addr.localdec_c_addr	= local_dec.c_addr;
		
		// Ribery Local Decode Area CH
		if( H264_Movie_Sdram_Ch_Check( bridge_yc.map_yc_addr.localdec_y_addr ) == SDRAM_ADR_CH0_TOP ){
			bridge_yc.map_yc_ch.localdec_y_ch	= RB_SDRAM_CH0;
		}
		else{
			bridge_yc.map_yc_ch.localdec_y_ch	= RB_SDRAM_CH1;
		}
		if( H264_Movie_Sdram_Ch_Check( bridge_yc.map_yc_addr.localdec_c_addr ) == SDRAM_ADR_CH0_TOP ){
			bridge_yc.map_yc_ch.localdec_c_ch	= RB_SDRAM_CH0;
		}
		else{
			bridge_yc.map_yc_ch.localdec_c_ch	= RB_SDRAM_CH1;
		}
		
		
		Sdram_Map_Movie_Record_Get_H264_Map_Reduction_Information( stream_ch, &reduction );
		
		// Ribery Reduction Area Size
		bridge_yc.map_yc_size.reduction_width	= reduction.g_width;
		bridge_yc.map_yc_size.reduction_y_bank	= reduction.y_bank;
		bridge_yc.map_yc_size.reduction_c_bank	= reduction.c_bank;
		
		// Ribery Reduction Area Address
		bridge_yc.map_yc_addr.reduction_y_addr	= reduction.y_addr;
		bridge_yc.map_yc_addr.reduction_c_addr	= reduction.c_addr;
		
		// Ribery Reduction Area CH
		if( H264_Movie_Sdram_Ch_Check( bridge_yc.map_yc_addr.reduction_y_addr ) == SDRAM_ADR_CH0_TOP ){
			bridge_yc.map_yc_ch.reduction_y_ch	= RB_SDRAM_CH0;
		}
		else{
			bridge_yc.map_yc_ch.reduction_y_ch	= RB_SDRAM_CH1;
		}
		if( H264_Movie_Sdram_Ch_Check( bridge_yc.map_yc_addr.reduction_y_addr ) == SDRAM_ADR_CH0_TOP ){
			bridge_yc.map_yc_ch.reduction_c_ch	= RB_SDRAM_CH0;
		}
		else{
			bridge_yc.map_yc_ch.reduction_c_ch	= RB_SDRAM_CH1;
		}
		
		// init param setting
		param.size					= gH264_Rec_Param.codec_param[stream_ch].size;
		param.frame_rate			= gH264_Rec_Param.codec_param[stream_ch].frame_rate;
		param.enc_mode				= gH264_Rec_Param.codec_param[stream_ch].stream_format;
		param.vrawinfo_interval		= 0;
		param.vstrminfo_interval	= 0;
		param.noirq_mode			= H264_OFF;	// IRQ_MODE
		
		if( gH264_Rec_Param.codec_param[stream_ch].gop_struct == E_H264_IF_GOP_STR_IIII ){
			param.gop_struct		= E_H264_IF_GOP_STR_IPPP;
			param.gop_size			= 1;
		}
		else{
			param.gop_struct		= gH264_Rec_Param.codec_param[stream_ch].gop_struct;
			param.gop_size			= BF_H264_User_Record_Get_Gop_Size(stream_ch);
		}
		
		param.vlcmode				= gH264_Rec_Param.codec_param[stream_ch].vlc_mode;
		
		if( gH264_Rec_Param.codec_param[stream_ch].rate_mode == E_H264_IF_RATE_MODE_VBR ){
			param.rate_mode		= E_H264_IF_RATE_MODE_VBR;
			param.bitrate		= gH264_Rec_Param.codec_param[stream_ch].bitrate;
			param.avebitrate	= gH264_Rec_Param.codec_param[stream_ch].average_bitrate;
		}
		else if( gH264_Rec_Param.codec_param[stream_ch].rate_mode == E_H264_IF_RATE_MODE_CBR ){
			param.rate_mode		= E_H264_IF_RATE_MODE_CBR;
			param.bitrate		= gH264_Rec_Param.codec_param[stream_ch].bitrate;
			param.avebitrate	= 0;	// don't care
		}
		else{
			param.rate_mode		= E_H264_IF_RATE_MODE_FIXED_QP;
			param.bitrate		= gH264_Rec_Param.codec_param[stream_ch].bitrate;
			param.avebitrate	= gH264_Rec_Param.codec_param[stream_ch].average_bitrate;
		}
		
		param.cl_gop			= gH264_Rec_Param.codec_param[stream_ch].closed_gop;
		param.profile			= gH264_Rec_Param.codec_param[stream_ch].profile;
		param.level				= gH264_Rec_Param.codec_param[stream_ch].level;
		param.scan_mode			= gH264_Rec_Param.scan_mode;
		
		param.frame_crop_top_offset		= gH264_Rec_Param.codec_param[stream_ch].frame_crop_top_offset;
		param.frame_crop_bottom_offset	= gH264_Rec_Param.codec_param[stream_ch].frame_crop_bottom_offset;
		param.frame_crop_right_offset	= gH264_Rec_Param.codec_param[stream_ch].frame_crop_right_offset;
		param.frame_crop_left_offset	= gH264_Rec_Param.codec_param[stream_ch].frame_crop_left_offset;
		
// --- REMOVE_H264_IQ_ENABLE BEGIN ---
		if( gH264_Rec_Param.codec_param[stream_ch].prefetch_bypath == H264_ON ){
			param.search_range_sel			= H264_SEARCH_RANGE_SEL_MODE3;
		}
		else{
// --- REMOVE_H264_IQ_ENABLE END ---
			param.search_range_sel			= H264_SEARCH_RANGE_SEL_MODE0;
// --- REMOVE_H264_IQ_ENABLE BEGIN ---
		}
// --- REMOVE_H264_IQ_ENABLE END ---
		param.image_quality_mode		= H264_IMAGE_QUALITY_MODE1;			// Fixed
		param.qp_lambda_offset			= -15;								// Fixed
		param.inter_intra_weight_ctrl	= E_H264_IF_WEIGHT_CTRL_INTER_1;	// Fixed
		#ifdef CO_RB_SLICE_ON
		param.slice			 			= gH264_Rec_Param.codec_param[stream_ch].slice_encode_flag;
		#endif
		param.mb_light_mode				= H264_ON;							// Fixed

		if(param.rate_mode == E_H264_IF_RATE_MODE_FIXED_QP){
			param.fixqp_en				= H264_ON;							// Fixed
		}
		else{
			param.fixqp_en				= H264_OFF;							// Fixed
		}
		
		param.insert_pic_struct			= gH264_Rec_Param.codec_param[stream_ch].pictstruct_flag;
		param.insert_buf_ctrl_param		= gH264_Rec_Param.codec_param[stream_ch].buffer_ctrl_flag;
		param.insert_recovery_point_sei	= gH264_Rec_Param.codec_param[stream_ch].recovery_point_sei_flag;
		param.end_of_stream_en			= gH264_Rec_Param.codec_param[stream_ch].end_of_stream_flag;
		param.end_of_sequence_en		= gH264_Rec_Param.codec_param[stream_ch].end_of_sequence_flag;
		param.cpb_over_en				= gH264_Rec_Param.codec_param[stream_ch].CPB_allow_flg;
		param.ubg_en					= gH264_Rec_Param.codec_param[stream_ch].next_frame_comp_flg;
		param.filler_en					= gH264_Rec_Param.codec_param[stream_ch].filler_flag;
		param.max_qp					= gH264_Rec_Param.codec_param[stream_ch].max_qp;
		param.min_qp					= gH264_Rec_Param.codec_param[stream_ch].min_qp;
		param.initial_cpb_removal_delay	= gH264_Rec_Param.codec_param[stream_ch].CPB_removal_delay;
		param.vec_info_en				= gH264_Rec_Param.codec_param[stream_ch].vector_info_flag;
		
		// VUI setteing
		vui.video_signal_type_present_flag	= gH264_Rec_Param.codec_param[stream_ch].vui.video_signal_type_present_flag;
		vui.video_format					= gH264_Rec_Param.codec_param[stream_ch].vui.video_format;
		vui.video_full_range_flag			= gH264_Rec_Param.codec_param[stream_ch].vui.video_full_range_flag;
		vui.colour_description_present_flag	= gH264_Rec_Param.codec_param[stream_ch].vui.colour_description_present_flag;
		vui.colour_primaries				= gH264_Rec_Param.codec_param[stream_ch].vui.colour_primaries;
		vui.transfer_characteristics		= gH264_Rec_Param.codec_param[stream_ch].vui.transfer_characteristics;
		vui.matrix_coefficients				= gH264_Rec_Param.codec_param[stream_ch].vui.matrix_coefficients;
		vui.aspect_ratio_info_present_flag	= gH264_Rec_Param.codec_param[stream_ch].vui.aspect_ratio_info_present_flag;
		vui.aspect_ratio_idc				= gH264_Rec_Param.codec_param[stream_ch].vui.aspect_ratio_idc;
		vui.sar_width						= gH264_Rec_Param.codec_param[stream_ch].vui.sar_width;
		vui.sar_height						= gH264_Rec_Param.codec_param[stream_ch].vui.sar_height;
		vui.chroma_loc_info_present_flag	= gH264_Rec_Param.codec_param[stream_ch].vui.chroma_loc_info_present_flag;
		vui.chroma_sample_loc_top_field		= gH264_Rec_Param.codec_param[stream_ch].vui.chroma_sample_loc_top_field;
		vui.chroma_sample_loc_bottom_field	= gH264_Rec_Param.codec_param[stream_ch].vui.chroma_sample_loc_bottom_field;
		
		memcpy( &scalinglist, (VOID*)&gH264_Rec_Param.codec_param[stream_ch].initial_scalinglist_info, sizeof(T_RB_INITPARAM_SCALINGLIST) );
		if(gH264_Rec_Param.codec_param[stream_ch].initial_scalinglist_info.scaling_list_flg >= H264_ON){
			scalinglist.ScalingListFlag = H264_ON;
		}
		
		param.vui			= &vui;
		param.scalinglist	= &scalinglist;
		
		H264_Ribery_Enc_InitParam_Set( (UINT32)stream_ch, &param, &bridge_yc );
		
		ret = H264_Ribery_Enc_Start( (UINT32)stream_ch );
		if( ret != H264_OK ){
			H264_Err_Print(("--- Ribery Enc Start Err ch=%d\n", stream_ch));
			return ret;
		}
	}
	
	return H264_OK;
}

/**
 *	@brief		Start Task Process.\n
 *				In case of Audio Task, this method initialize audio process.
 *	@param		i:task_id	Task Identifier
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_encode_task_start( UINT32 task_id )
{
	OS_USER_ER			ercd, ercd2;
	T_OS_USER_RTSK		rtsk;	// Task Information
	
	OS_User_Ter_Tsk(task_id);
	// Create Task process
	do{
		ercd = OS_User_Sta_Tsk( task_id, H264_MODE_RECORD );
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

/**
 *	@brief		It sets Ribery Parameters for Frame Encode
 *	@param		stream_ch		Stream CH(Ribery CH)
 *	@param		vraw_info		VRAW_INFO table pointer
 *	@param		index			VRAW_INFO table index
  *	@param		frame_no		frame number
 *	@param		stop_req_flag	stop request flag
 *	@param		vop_cnt			vop counter for Picture Type
 *	@param		stop_comp_flag	Stop Completed Flag
 *	@return		H264_OK			Normal End
 *	@return		H264_OK			Abnormal End
 *	@note		None
 *	@attention	None
 */
static INT32 h264_encode_ribery_vraw_param_set(
				E_H264_IF_STREAM_CH		stream_ch,
				T_RB_INFOTABLE_VRAW		*vraw_info,
				T_RB_FIFO_INDEX			*vraw_index,
				ULLONG					*frame_no,
				UINT32					stop_req_flag,
				USHORT					*vop_cnt,
				USHORT					*gop_cnt
				)
{
	INT32	ret;
	
	// input parameter check
	if( (vraw_info == NULL) || (vraw_index == NULL) || (frame_no == NULL) ){
		H264_Err_Print(("---  h264_encode_ribery_vraw_param_set: NULL\n"));
		return H264_NG;
	}
	
	vraw_info->end_flag = H264_OFF;
	vraw_index->ch = stream_ch;
	
	OS_User_Wai_Sem(SID_H264);
	ret = H264_Ribery_Enc_Vraw_Empty_Field_Get( stream_ch, &vraw_index->index );
	if( ret != H264_OK ){
		H264_Err_Print(( "Vraw_Empty_Field_Get NG\n" ));
		OS_User_Sig_Sem(SID_H264);
		return H264_NG;
	}
	ret = h264_encode_Vin_get_decide_bank( stream_ch, (INT32*)&vraw_info->physical_bank_idx, frame_no );
	if( ret == H264_ENC_VIN_CTL_NONEDATA ){
		if( gH264_ViewLater_Count == 0 ){
			H264_Warning_Print(( "H264 View task later!! ch[%d] (timming NG) \n", stream_ch ));
		}
		gH264_ViewLater_Count++;
		gH264_ViewLater_Total_Count++;
		if( gH264_ViewLater_Count > D_H264_ENCODE_LATER_THRESHOLD ){
			gH264_ViewLater_Count = 0;
		}
		if( gH264_Vin_Adr_Ctl[stream_ch].decide_read_point == 0 ){
			h264_encode_Vin_set_decide_bank( stream_ch, gH264_Vin_Adr_Ctl[stream_ch].decide_bank[ RB_MAP_BNK_ENC_YCC - 1 ], gH264_Vin_Adr_Ctl[stream_ch].decide_frame_no[ RB_MAP_BNK_ENC_YCC - 1 ] );
		}
		else{
			h264_encode_Vin_set_decide_bank( stream_ch, gH264_Vin_Adr_Ctl[stream_ch].decide_bank[ gH264_Vin_Adr_Ctl[stream_ch].decide_read_point - 1 ], gH264_Vin_Adr_Ctl[stream_ch].decide_frame_no[ gH264_Vin_Adr_Ctl[stream_ch].decide_read_point - 1 ] );
		}
		h264_encode_Vin_get_decide_bank( stream_ch, (INT32*)&vraw_info->physical_bank_idx, frame_no );
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "View later %u-0x%lX", stream_ch, (ULONG)*frame_no ));
		
		H264_If_Record_Notify_RepeatFrame( stream_ch );
		
		gH264_Vin_Adr_Ctl[stream_ch].duplicate_check_tbl[vraw_info->physical_bank_idx]++;
	}
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-0x%lX VRAW", stream_ch, (ULONG)*frame_no ));
	
	if( stop_req_flag == H264_ON ){
		H264_Info_Print(( "H264 Last Vraw!! ch = %d\n", stream_ch ));
		vraw_info->end_flag = H264_ON;
	}
	OS_User_Sig_Sem(SID_H264);
	
	// Serial Number
	gH264_Enc_Serial_Num[stream_ch]++;
	vraw_info->serial_num = *frame_no;
	
	// PTS to 360KHz change.
	vraw_info->PTS = ((H264_PTS_BASE * 10) / gH264_Enc_Ctrl.video[stream_ch].time_scale) * gH264_Enc_Ctrl.video[stream_ch].total_time_increment;
	vraw_info->PTS /= 10;
	
	if( (*vop_cnt == 1) && (gH264_Dynamic_Change_Flag[stream_ch] == H264_ON) ){
		if( gH264_Rec_Param.codec_param[stream_ch].rate_mode == E_H264_IF_RATE_MODE_CBR ){
			gH264_Rec_Param.codec_param[stream_ch].bitrate = gH264_Dynamic_Param[stream_ch].bitrate;
		}
		else if( gH264_Rec_Param.codec_param[stream_ch].rate_mode == E_H264_IF_RATE_MODE_VBR ){
			if( gH264_Rec_Param.codec_param[stream_ch].bitrate >= gH264_Dynamic_Param[stream_ch].bitrate ){
				gH264_Rec_Param.codec_param[stream_ch].average_bitrate = gH264_Dynamic_Param[stream_ch].bitrate;
			}
			else{
				H264_Warning_Print(("+++ H264 Dynamic parameter bitrat is over MAX bitrate.\n" ));
			}
		}
		else if( gH264_Rec_Param.codec_param[stream_ch].rate_mode == E_H264_IF_RATE_MODE_FIXED_QP ){
			gH264_Rec_Param.codec_param[stream_ch].bitrate = gH264_Dynamic_Param[stream_ch].bitrate;
		}
		
		gH264_Rec_Param.codec_param[stream_ch].gop_num				= gH264_Dynamic_Param[stream_ch].frame_num_of_gop;
		gH264_Rec_Param.codec_param[stream_ch].gop_struct			= gH264_Dynamic_Param[stream_ch].gop_struct;
		gH264_Rec_Param.codec_param[stream_ch].gop_num_idr_to_idr	= gH264_Dynamic_Param[stream_ch].gop_num_idr_to_idr;
		gH264_Rec_Param.codec_param[stream_ch].closed_gop			= gH264_Dynamic_Param[stream_ch].closed_gop;
		
		*gop_cnt = 1;	// GOP reset
		
		gH264_Dynamic_Change_Flag[stream_ch] = H264_OFF;
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "change" ));
	}
	
	// Picture Type and Stream End Flag
	if( gH264_Insert_IDR_Flag[stream_ch] == H264_ON ){	// Insert IDR Picture
		H264_Info_Print(("--- !! [H264] Insert IDR Picture [%d] (vop_cnt is %02d) !!\n", stream_ch, *vop_cnt ));
		switch( gH264_Rec_Param.codec_param[stream_ch].gop_struct ){
			case E_H264_IF_GOP_STR_IIII :	// I(IDR)III III(0x00)
			case E_H264_IF_GOP_STR_IPPP :	// I(IDR)PPP PPP(0x01)
				*vop_cnt = 1;
				*gop_cnt = 1;
				gH264_Insert_IDR_Flag[stream_ch] = H264_OFF;
				break;
			case E_H264_IF_GOP_STR_IBBP :	// I(IDR)BBP BBP(0x02)
				if( (*vop_cnt % 3) == 1 ){	// B1 Frane
					*vop_cnt = 1;
					*gop_cnt = 1;
					gH264_Insert_IDR_Flag[stream_ch] = H264_OFF;
				}
				break;
			case E_H264_IF_GOP_STR_IBP :	// I(IDR)BP BP BP BP(0x03)
			default:
				if( (*vop_cnt % 2) == 1 ){	// B1 Frane
					*vop_cnt = 1;
					*gop_cnt = 1;
					gH264_Insert_IDR_Flag[stream_ch] = H264_OFF;
				}
				break;
		}
	}

	switch( gH264_Rec_Param.codec_param[stream_ch].gop_struct ){
		case E_H264_IF_GOP_STR_IIII :					// I(IDR)III III(0x00)
			if( *vop_cnt == 1 ){
				vraw_info->pic_type = RB_PIC_IDR;		// IDR(0x00)
			}
			else{
				vraw_info->pic_type = RB_PIC_I;			// I-PIC(0x01)
			}
			vraw_info->gop_struct = RB_GOP_STR_IPPP;
			break;
		case E_H264_IF_GOP_STR_IPPP :					// I(IDR)PPP PPP(0x01)
			if( *vop_cnt == 1 ){
				if( *gop_cnt == 1 ){
					vraw_info->pic_type = RB_PIC_IDR;	// IDR(0x00)
				}
				else{
					vraw_info->pic_type = RB_PIC_I;		// I-PIC(0x01)
				}
			}
			else{
				vraw_info->pic_type = RB_PIC_P;			// P-PIC(0x02)
			}
			vraw_info->gop_struct = RB_GOP_STR_IPPP;
			break;
		case E_H264_IF_GOP_STR_IBBP :					// I(IDR)BBP BBP(0x02)
			if( *vop_cnt == 3 ){
				if( *gop_cnt == 1 ){
					vraw_info->pic_type = RB_PIC_IDR;	// IDR(0x00)
				}
				else{
					vraw_info->pic_type = RB_PIC_I;		// I-PIC(0x01)
				}
			}
			else{
				if( (*vop_cnt % 3) == 0 ){
					vraw_info->pic_type = RB_PIC_P;		// P-PIC(0x02)
				}
				else {
					vraw_info->pic_type = RB_PIC_B;		// B-PIC(0x03)
				}
			}
			vraw_info->gop_struct = RB_GOP_STR_IBBP;
			break;
		case E_H264_IF_GOP_STR_IBP :					// I(IDR)BBP BBP(0x02)
			if( *vop_cnt == 2 ){
				if( *gop_cnt == 1 ){
					vraw_info->pic_type = RB_PIC_IDR;	// IDR(0x00)
				}
				else{
					vraw_info->pic_type = RB_PIC_I;		// I-PIC(0x01)
				}
			}
			else{
				if( (*vop_cnt % 2) == 0 ){
					vraw_info->pic_type = RB_PIC_P;		// P-PIC(0x02)
				}
				else {
					vraw_info->pic_type = RB_PIC_B;		// B-PIC(0x03)
				}
			}
			vraw_info->gop_struct = RB_GOP_STR_IBPB;
			break;
		default :
			break;
	}

	if((vraw_info->pic_type == RB_PIC_I) || vraw_info->pic_type == RB_PIC_IDR){
		if(gH264_First_IDR_Flag[stream_ch] == FALSE){
			gH264_Insert_IDR_Flag[stream_ch] = H264_ON;

			gH264_First_IDR_Flag[stream_ch] = TRUE;
		}
	}
	
	// Frame Count in GOP
	vraw_info->frame_count_in_gop	= *vop_cnt - 1;	// BaseFW is vop is 1 start. but ribery is 0 start.
	
	vraw_info->gop_size	   = BF_H264_User_Record_Get_Gop_Size(stream_ch);
	vraw_info->bitrate     = gH264_Rec_Param.codec_param[stream_ch].bitrate / 1000;			// bps -> Kbps
	vraw_info->avebitrate  = gH264_Rec_Param.codec_param[stream_ch].average_bitrate / 1000;	// bps -> Kbps
	vraw_info->constant_qp = gH264_Rec_Param.codec_param[stream_ch].bitrate / 1000000;
	
	return H264_OK;
}

/**
 *	@brief		It executes Ribery Encoder.
 *	@param		vraw_index			VRAW_INFO table index
 *	@param		vraw_info			VRAW_INFO table pointer
 *	@return		H264_OK			Normal End
 *	@return		H264_NG			Abnormal End
 *	@note		None
 *	@attention	None
 */
static INT32 h264_encode_ribery_vraw_set(
						T_RB_FIFO_INDEX			vraw_index,
						T_RB_INFOTABLE_VRAW		*vraw_info
						)
{
	INT32	ret = 0;
	
	if( vraw_info == NULL ){
		H264_Err_Print(("---  h264_encode_ribery_vraw_set: NULL\n"));
		return( H264_NG );
	}
	
	// H.264 Encode Start
	ret = H264_Ribery_Enc_VRAW_Set( vraw_index, vraw_info );
	
	return ret;
}

/**
 *	@brief		It executes Ribery Encoder.
 *	@param		vraw_index			VRAW_INFO table index
 *	@param		vraw_info			VRAW_INFO table pointer
 *	@return		H264_OK			Normal End
 *	@return		H264_NG			Abnormal End
 *	@note		None
 *	@attention	None
 */
static INT32 h264_encode_ribery_start(
						UINT32					count,
						T_RB_FIFO_INDEX			*vraw_index
						)
{
	INT32	ret = 0;
	
	if( vraw_index == NULL ){
		H264_Err_Print(("---  h264_encode_ribery_start: NULL\n"));
		return( H264_NG );
	}
	
	// H.264 Encode Start
	ret = H264_Ribery_Enc_VRAW_Req( count, vraw_index );
	
	return ret;
}

/**
 *	@brief		Get Encoder Result.\n
 *	@param		i/o:video_info		Video Informations
 *	@return		H264_OK				OK - Next Exist
 *				H264_OK_COMPLETE		OK - Read Completed
 *	@return		H264_ERR_INPUT_PARAM	Input Parameter Error
 *	@return		H264_NG				No Sample
 *	@note		None
 *	@attention	None
 */
static INT32 h264_encode_encoder_result_get( T_H264_ENC_VIDEO_INFO* video_info, UINT32* info_count )
{
	INT32					ret = 0;
	T_RB_FIFO_INDEX			vstrm_index[D_H264_ENC_FIFO_REQ_COUNT];
	T_RB_INFOTABLE_VSTRM	vstrm_info[D_H264_ENC_FIFO_REQ_COUNT];
	UINT32					count;
	UINT32					loop_cnt;
	DOUBLE					temp_pts, temp_dts;
	ULONG					stream_ch;
	ULONG					tmp_latest_stream_addr;
	ULONG					tmp_over_stream_size;
	
	if( (video_info == NULL) || (info_count == NULL) ){
		return H264_ERR_INPUT_PARAM;
	}
	
	video_info->sample_num = 0;
	
	// It gets Index and VSTREAM_INFO from FIFO
	ret = H264_Ribery_Enc_VSTREAM_Get( info_count, &vstrm_index[0], &vstrm_info[0] );
	if( ret == H264_NG ){
		H264_Info_Print(("--- H264_Ribery_Enc_VSTREAM_Get Complete\n" ));
		return H264_NG;
	}
	if( *info_count == 0 ){
		return H264_NG;
	}
	
	for( count=0; count<(*info_count); count++ ){
		stream_ch = vstrm_index[count].ch;
		
		// First stream
		video_info->first_param[count].sample_type    = vstrm_info[count].pic_type;
		video_info->first_param[count].sample_address = (UCHAR*)(vstrm_info[count].first_stream_start_addr);
		video_info->first_param[count].nal_unit_num   = vstrm_info[count].first_nal_unit_num;
		video_info->first_param[count].VBV_buffer     = vstrm_info[count].first_VBV_boc;
		video_info->first_param[count].sample_size    = vstrm_info[count].first_stream_size;
		video_info->first_param[count].ave_qp         = vstrm_info[count].first_ave_qp;
		video_info->first_param[count].bitrate        = vstrm_info[count].first_bitrate;
		video_info->first_param[count].cpb_over_flag  = vstrm_info[count].first_cpb_over_flag;
		
		for( loop_cnt = 0; loop_cnt < H264_NAL_UNIT_MAX_NUM; loop_cnt++ ){
			video_info->first_param[count].nal_unit_size[loop_cnt] = vstrm_info[count].first_rep_tbl_buf_num[loop_cnt];
		}
		
		// PTS/DTS
		temp_pts = (DOUBLE)vstrm_info[count].PTS / gH264_Enc_Ctrl.video[stream_ch].coefficient;
		temp_dts = (DOUBLE)vstrm_info[count].DTS / gH264_Enc_Ctrl.video[stream_ch].coefficient;
		
		video_info->first_param[count].sample_pts = (LLONG)temp_pts;
		video_info->first_param[count].sample_dts = (LLONG)temp_dts;
		
		// Second stream
		if( gH264_Rec_Param.scan_mode == E_H264_IF_SCAN_MODE_INTERLACE ){
			if( vstrm_info[count].pic_type == RB_PIC_B ){
				video_info->second_param[count].sample_type    = E_H264_IF_PIC_TYPE_B;
			}
			else{
				video_info->second_param[count].sample_type    = E_H264_IF_PIC_TYPE_P;
			}
			video_info->second_param[count].sample_address = (UCHAR*)(vstrm_info[count].second_stream_start_addr);
			video_info->second_param[count].nal_unit_num   = vstrm_info[count].second_nal_unit_num;
			video_info->second_param[count].VBV_buffer     = vstrm_info[count].second_VBV_boc;
			video_info->second_param[count].sample_size    = vstrm_info[count].second_stream_size;
			video_info->second_param[count].ave_qp         = vstrm_info[count].second_ave_qp;
			video_info->second_param[count].bitrate        = vstrm_info[count].second_bitrate;
			video_info->second_param[count].cpb_over_flag  = vstrm_info[count].second_cpb_over_flag;
			
			for( loop_cnt = 0; loop_cnt < H264_NAL_UNIT_MAX_NUM; loop_cnt++ ){
				video_info->second_param[count].nal_unit_size[loop_cnt] = vstrm_info[count].second_rep_tbl_buf_num[loop_cnt];
			}
			
			// PTS/DTS
			video_info->second_param[count].sample_pts  = video_info->first_param[count].sample_pts;
			video_info->second_param[count].sample_dts  = video_info->first_param[count].sample_dts;
			
			// first param re-set PTS/DTS
			video_info->first_param[count].sample_pts  = video_info->second_param[count].sample_pts - gH264_Enc_Ctrl.video[stream_ch].time_increment;
			video_info->first_param[count].sample_dts  = video_info->second_param[count].sample_dts - gH264_Enc_Ctrl.video[stream_ch].time_increment;
		}
		
#ifdef CO_MOVIE_DSC_MODE
		//*********  DTS of Top field/frame to unify to 0  **********//
		// Ribery put out Top frame/field DTS is follows.
		//  IBBP Interlace   = (-)time increment*2
		//  IBBP Progressive = (-)time increment
		//  IPPP Interlace   = 0
		//  IPPP Progressice = 0
		if( gH264_Rec_Param.codec_param[stream_ch].gop_struct == E_H264_IF_GOP_STR_IBBP ){
			if( gH264_Rec_Param.scan_mode == E_H264_IF_SCAN_MODE_INTERLACE ){
				video_info->first_param[count].sample_pts  += gH264_Enc_Ctrl.video[stream_ch].time_increment*2;
				video_info->first_param[count].sample_dts  += gH264_Enc_Ctrl.video[stream_ch].time_increment*2;
				video_info->second_param[count].sample_pts += gH264_Enc_Ctrl.video[stream_ch].time_increment*2;
				video_info->second_param[count].sample_dts += gH264_Enc_Ctrl.video[stream_ch].time_increment*2;
			}
			else{
				video_info->first_param[count].sample_pts  += gH264_Enc_Ctrl.video[stream_ch].time_increment;
				video_info->first_param[count].sample_dts  += gH264_Enc_Ctrl.video[stream_ch].time_increment;
				video_info->second_param[count].sample_pts += gH264_Enc_Ctrl.video[stream_ch].time_increment;
				video_info->second_param[count].sample_dts += gH264_Enc_Ctrl.video[stream_ch].time_increment;
			}
		}
#endif
		// Common
		video_info->frame_no[count]              = vstrm_info[count].VRAW_serial_num;
		video_info->vstrem_serial_num[count]     = vstrm_info[count].serial_num;
		video_info->frame_count_in_gop[count]    = vstrm_info[count].VRAW_frame_count_in_gop + 1;		// BaseFW is vop is 1 start. but ribery is 0 start.
		video_info->poc[count]                   = vstrm_info[count].poc;
		video_info->ave_bitrate[count]           = vstrm_info[count].avebitrate;
		video_info->stream_end_flg[count]        = vstrm_info[count].end_flag;
		video_info->vstream_free_index[count]    = vstrm_index[count].index;
		video_info->stream_ch[count]             = stream_ch;
		
		// Buffer Limit Calculate
		tmp_latest_stream_addr = (ULONG)video_info->first_param[count].sample_address + video_info->first_param[count].sample_size;
		if( (ULONG)video_info->first_param[count].sample_address < Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_End_Address() )
		{
			// Stream Buffer 0
			if( tmp_latest_stream_addr < Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_End_Address() ){
				if( tmp_latest_stream_addr > vstrm_info[count].a_streambuf_write_limit_ptr ){
					gH264_Stream_Buffer0_Limit = (Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_End_Address() - tmp_latest_stream_addr) +
					                        (vstrm_info[count].a_streambuf_write_limit_ptr - Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_Start_Address());
				}
				else{
					gH264_Stream_Buffer0_Limit = vstrm_info[count].a_streambuf_write_limit_ptr - tmp_latest_stream_addr;
				}
			}
			else{
				tmp_over_stream_size = tmp_latest_stream_addr - Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_End_Address();
				
				gH264_Stream_Buffer0_Limit = vstrm_info[count].a_streambuf_write_limit_ptr - Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_Start_Address() + tmp_over_stream_size;
			}
		}
		else{
			// Stream Buffer 1
			if( tmp_latest_stream_addr < Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_End_Address() ){
				if( tmp_latest_stream_addr > vstrm_info[count].b_streambuf_write_limit_ptr ){
					gH264_Stream_Buffer1_Limit = (Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_End_Address() - tmp_latest_stream_addr) +
					                        (vstrm_info[count].b_streambuf_write_limit_ptr - Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_Start_Address());
				}
				else{
					gH264_Stream_Buffer1_Limit = vstrm_info[count].b_streambuf_write_limit_ptr - tmp_latest_stream_addr;
				}
			}
			else{
				tmp_over_stream_size = tmp_latest_stream_addr - Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_End_Address();
				
				gH264_Stream_Buffer1_Limit = vstrm_info[count].b_streambuf_write_limit_ptr - Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_Start_Address() + tmp_over_stream_size;
			}
		}
		video_info->stream_buffer0_limit[count] = gH264_Stream_Buffer0_Limit;
		video_info->stream_buffer1_limit[count] = gH264_Stream_Buffer1_Limit;
	}
	
	return ret;
}


/**
 *	@brief		Checked whether Stream Area of Ribery went around.\n
 *	@param		i/o:video_info		Video Informations\n
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_encode_stream_around_check( T_H264_ENC_SAMPLE_PARAM* sample_param )
{
#ifndef CO_DEBUG_ON_PC
	UINT32				sample_address;
	UINT32				sample_size;
	UINT32				size_before;
	UINT32				size_after;
	UINT32				strm_start_address;
	UINT32				strm_end_address;
	UINT32				stream_work_address;
	
	sample_address		= (UINT32)sample_param->sample_address;
	sample_size			= sample_param->sample_size;
	
	// Init address
	strm_start_address	= Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_Start_Address();
	strm_end_address	= Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_End_Address();
	stream_work_address	= Sdram_Map_Movie_Record_Get_H264_Stream_Work0() + 64;	// PES Header Area 64byte
	
	if( (strm_start_address <= sample_address) && (sample_address < strm_end_address) ){
		// bank0
		// do nothing.
	}
	else{
		// bank1
		strm_start_address	= Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_Start_Address();
		strm_end_address	= Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_End_Address();
		stream_work_address	= Sdram_Map_Movie_Record_Get_H264_Stream_Work1() + 64;	// PES Header Area 64byte
	}
	
	if( (sample_address + sample_size) > strm_end_address ){
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "around" ));
		size_before = strm_end_address - sample_address;
		H264_Movie_Memcpy( (UCHAR*)stream_work_address, (UCHAR*)sample_address, size_before );
		
		size_after = sample_size - size_before;
		H264_Movie_Memcpy( (UCHAR*)(stream_work_address + size_before), (UCHAR*)strm_start_address, size_after );
		
		sample_param->sample_address = (UCHAR*)stream_work_address;
		
		H264_Movie_Cache_Clean( stream_work_address, sample_size );
		
		if(sample_size > Sdram_Map_Movie_Record_Get_H264_Stream_Work_Size() ){
			H264_Info_Print(("---  H264_ENT: STRM_WORK_AREA sample_size=%x\n",sample_size));
		}
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "around" ));
	}
#endif
}

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
VOID h264_encode_pes_header_set( ULONG stream_ch, ULONG addr, ULONG size, ULLONG pts, ULLONG dts )
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
	*(USHORT*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE)     = 0x0000;			// start_code					:23-8bit
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 2)  = 0x01;				// start_code					:7-0bit
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 3)  = 0xE0 | stream_ch;	// stream_ch					:8bit	:1110xxxxb. (Sets the value of the Channel to the xxxx.)
	*(USHORT*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 4) = 0x0000;			// packet_length				:16bit	:0 fixed.(PES length undefined)
	*(USHORT*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 6) = 0xC081;			// sync_code					:2bit	:(10b)
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
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 8)  = 0x0A;				// PES_header_data_length		:8bit	:PTS:5 + DTS:5 = 10byte
	
	// PTS
	tmp_pts = (pts & 0x00000001C0000000) >> 29;
	tmp_pts |= 0x31;	// "0011" & marker_bit
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 9)  = tmp_pts;
	tmp_pts = (pts & 0x000000003FC00000) >> 22;
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 10) = tmp_pts;
	tmp_pts = (pts & 0x00000000003F8000) >> 14;
	tmp_pts |= 0x01;
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 11) = tmp_pts;
	tmp_pts = (pts & 0x0000000000007F80) >> 7;
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 12) = tmp_pts;
	tmp_pts = (pts & 0x000000000000007F) << 1;
	tmp_pts |= 0x01;
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 13) = tmp_pts;
	
	// DTS
	tmp_dts = (dts & 0x00000001C0000000) >> 29;
	tmp_dts |= 0x11;
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 14) = tmp_dts;
	tmp_dts = (dts & 0x000000003FC00000) >> 22;
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 15) = tmp_dts;
	tmp_dts = (dts & 0x00000000003F8000) >> 14;
	tmp_dts |= 0x01;
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 16) = tmp_dts;
	tmp_dts = (dts & 0x0000000000007F80) >> 7;
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 17) = tmp_dts;
	tmp_dts = (dts & 0x000000000000007F) << 1;
	tmp_dts |= 0x01;
	*(UCHAR*)(addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE + 18) = tmp_dts;
	
	H264_Movie_Cache_Clean_Flush( (addr - D_H264_ENCODE_PES_HEADER_VIDEO_SIZE), D_H264_ENCODE_PES_HEADER_VIDEO_SIZE );
#endif
}

/**
 *	@brief		ENT Task Illegal end.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_encode_illagal( T_H264_ENC_VIDEO_INFO* video_info, UINT32 info_count, ULLONG total_duration )
{
	INT32			ret;
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	waiptn;
	OS_USER_FLGPTN	flgptn;
	T_H264_IF_VIDEO_STREAM_PARAM video_stream_info;
	UINT32				free_count, free_loop;
	T_RB_FIFO_INDEX		free_index[D_H264_ENC_FIFO_REQ_COUNT];
	UINT32				free_bank[D_H264_ENC_FIFO_REQ_COUNT];
	E_H264_IF_STREAM_CH	notify_loop;
	T_H264_IF_MSG		msg;
	
	// CC_Task End wait
	OS_User_Set_Flg( FID_H264, FLG_H264_FORCE_STOP );
	
	for( notify_loop=E_H264_IF_STREAM_CH_0; notify_loop < E_H264_IF_STREAM_CH_MAX; notify_loop++ ){
		if( gH264_Rec_Param.codec_param[notify_loop].enable_flg == H264_ON ){
			video_stream_info.stream_ch = notify_loop;
			video_stream_info.error_flg = TRUE;
			H264_If_Record_Notify_Video_Stream( &video_stream_info );
			h264_encode_ch_map_reset( notify_loop );
		}
	}
	
	waiptn = FLG_H264_CC_STOP;
	ercd = OS_User_Twai_Flg( FID_H264, waiptn, D_OS_USER_TWF_ANDW, &flgptn, 1000 );
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(("+++ H264_ENT Illigal: Time out ercd=%x\n", ercd ));
	}
	
	h264_encode_result_notify( gH264_Enc_Ctrl.task_result );
	
	// wait free all flag
	OS_User_Twai_Flg( FID_H264, FLG_H264_ENC_FREE_ALL_VSTREAM, D_OS_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_FOREVER );
	OS_User_Clr_Flg( FID_H264, ~FLG_H264_ENC_FREE_ALL_VSTREAM );
	
	H264_Err_Print(("+++ h264_encode_illagal\n"));
	// VRAW_Info_Table Free
	while(1){
		ret = H264_Ribery_Enc_VRAW_Free( &free_count, &free_index[0], &free_bank[0] );
		if( (ret == H264_OK) || (ret == H264_OK_COMPLETE) ){
			for( free_loop=0; free_loop<free_count; free_loop++ ){
				h264_encode_Vin_set_empty_bank( (E_H264_IF_STREAM_CH)free_index[free_loop].ch, free_bank[free_loop] );
			}
		}
		if( ret != H264_OK ) { break; }
	}
	
	H264_Ribery_Finalize( H264_MODE_ENCODE );
	
	// ENT Task End Flag Set
	OS_User_Set_Flg( FID_H264, FLG_H264_ENT_STOP );
	
	msg.type = E_H264_IF_TYPE_NO_REQUEST;
	ret = H264_If_Set_Msg( &msg, E_H264_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		H264_Err_Print(("--- H264_ENT End Msg ERR. ret=%x\n", ret ));
	}
	
	return;
}

/**
 *  @brief		Reset Channel Map Information \n
 *	@param		sream_ch
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID h264_encode_ch_map_reset( E_H264_IF_STREAM_CH stream_ch )
{
	E_H264_IF_STREAM_CH							first_stream_ch = E_H264_IF_STREAM_CH_MAX;
	E_H264_IF_STREAM_CH							second_stream_ch = E_H264_IF_STREAM_CH_MAX;
	UINT32										i;
	UINT32										free1, free2, free3;
	T_SDRAM_MAP_MOVIE_RECORD_BASE_INFORMATION	map_base_info;
	T_SDRAM_MAP_MOVIE_RECORD_INFORMATION		map_info;
	
	Sdram_Map_Movie_Record_Get_Map_Information( &map_info );
	Sdram_Map_Movie_Record_Get_Map_Base_Information( &map_base_info );
	
	if( map_info.ribery_map_count == 0 ){
		;	// nothing (for Error route)
	}
	else if( map_info.ribery_map_count == 1 ){
		map_info.ribery_map_reduction_use		= map_base_info.ribery_reduction_top_addr;
		map_info.ribery_map_reduction_free_size	= map_base_info.ribery_reduction_bottom_addr - map_base_info.ribery_reduction_top_addr;
		map_info.ribery_map_localdec_use		= map_base_info.ribery_localdec_top_addr;
		map_info.ribery_map_localdec_free_size	= map_base_info.ribery_localdec_bottom_addr - map_base_info.ribery_localdec_top_addr;
		map_info.ribery_map_yc_use				= map_base_info.ribery_yc_top_addr;;
		map_info.ribery_map_yc_free_size		= map_base_info.ribery_yc_bottom_addr - map_base_info.ribery_yc_top_addr;
	}
	else if( map_info.ribery_map_count == 2 ){
		for( i=0; i<E_H264_IF_STREAM_CH_MAX; i++ ){
			if( i != stream_ch ){
				if( map_info.ribery_map_size[i] != E_H264_IF_IMAGE_SIZE_MAX ){
					first_stream_ch = i;
					break;
				}
			}
		}
		if( first_stream_ch == E_H264_IF_STREAM_CH_MAX ){
			H264_Err_Print(("h264_encode_ch_map_reset : stream ch error! 1\n"));		// This route does not pass through.
		}
		
		free1 = map_info.ribery_map_reduction_top_addr[first_stream_ch] - map_base_info.ribery_reduction_top_addr;
		free2 = map_base_info.ribery_reduction_bottom_addr - map_info.ribery_map_reduction_bottom_addr[first_stream_ch];
		
		if( free1 > free2 ){
			map_info.ribery_map_reduction_use		= map_base_info.ribery_reduction_top_addr;
			map_info.ribery_map_reduction_free_size	= free1;
		}
		else{
			map_info.ribery_map_reduction_use		= map_info.ribery_map_reduction_bottom_addr[first_stream_ch];
			map_info.ribery_map_reduction_free_size	= free2;
		}
		
		free1 = map_info.ribery_map_yc_top_addr[first_stream_ch] - map_base_info.ribery_yc_top_addr;
		free2 = map_base_info.ribery_yc_bottom_addr - map_info.ribery_map_yc_bottom_addr[first_stream_ch];
		
		if( free1 > free2 ){
			map_info.ribery_map_localdec_use		= map_base_info.ribery_localdec_top_addr;
			map_info.ribery_map_localdec_free_size	= map_info.ribery_map_localdec_top_addr[first_stream_ch] - map_base_info.ribery_localdec_top_addr;
			map_info.ribery_map_yc_use				= map_base_info.ribery_yc_top_addr;
			map_info.ribery_map_yc_free_size		= free1;
		}
		else{
			map_info.ribery_map_localdec_use		= map_info.ribery_map_localdec_bottom_addr[first_stream_ch];
			map_info.ribery_map_localdec_free_size	= map_base_info.ribery_localdec_bottom_addr - map_info.ribery_map_localdec_bottom_addr[first_stream_ch];
			map_info.ribery_map_yc_use				= map_info.ribery_map_yc_bottom_addr[first_stream_ch];
			map_info.ribery_map_yc_free_size		= free2;
		}
	}
	else{
		for( i=0; i<E_H264_IF_STREAM_CH_MAX; i++ ){
			if( i != stream_ch ){
				if( map_info.ribery_map_size[i] != E_H264_IF_IMAGE_SIZE_MAX ){
					if( first_stream_ch == E_H264_IF_STREAM_CH_MAX ){
						first_stream_ch = i;
					}
					else{
						if( map_info.ribery_map_reduction_top_addr[first_stream_ch] > map_info.ribery_map_reduction_top_addr[i] ){
							second_stream_ch = first_stream_ch;
							first_stream_ch = i;
						}
						else{
							second_stream_ch = i;
						}
						break;
					}
				}
			}
		}
		if( (first_stream_ch == E_H264_IF_STREAM_CH_MAX) ||
			(second_stream_ch == E_H264_IF_STREAM_CH_MAX) ){
			H264_Err_Print(("h264_encode_ch_map_reset : stream ch error! 2\n"));		// This route does not pass through.
		}
		
		free1 = map_info.ribery_map_reduction_top_addr[first_stream_ch] - map_base_info.ribery_reduction_top_addr;
		free2 = map_info.ribery_map_reduction_top_addr[second_stream_ch] - map_info.ribery_map_reduction_bottom_addr[first_stream_ch];
		free3 = map_base_info.ribery_reduction_bottom_addr - map_info.ribery_map_reduction_bottom_addr[second_stream_ch];
		
		if( (free1 > free2) && (free1 > free3) ){
			// free1 case
			map_info.ribery_map_reduction_use		= map_base_info.ribery_reduction_top_addr;
			map_info.ribery_map_reduction_free_size	= free1;
		}
		else if( (free2 > free1) && (free2 > free3) ){
			// free2 case
			map_info.ribery_map_reduction_use		= map_info.ribery_map_reduction_bottom_addr[first_stream_ch];
			map_info.ribery_map_reduction_free_size	= free2;
		}
		else{
			// free3 case
			map_info.ribery_map_reduction_use		= map_info.ribery_map_reduction_bottom_addr[second_stream_ch];
			map_info.ribery_map_reduction_free_size	= free3;
		}
		
		for( i=0; i<E_H264_IF_STREAM_CH_MAX; i++ ){
			if( i != stream_ch ){
				if( map_info.ribery_map_size[i] != E_H264_IF_IMAGE_SIZE_MAX ){
					if( first_stream_ch == E_H264_IF_STREAM_CH_MAX ){
						first_stream_ch = i;
					}
					else{
						if( map_info.ribery_map_yc_top_addr[first_stream_ch] > map_info.ribery_map_yc_top_addr[i] ){
							second_stream_ch = first_stream_ch;
							first_stream_ch = i;
						}
						else{
							second_stream_ch = i;
						}
						break;
					}
				}
			}
		}
		if( (first_stream_ch == E_H264_IF_STREAM_CH_MAX) ||
			(second_stream_ch == E_H264_IF_STREAM_CH_MAX) ){
			H264_Err_Print(("h264_encode_ch_map_reset : stream ch error! 3\n"));		// This route does not pass through.
		}
		
		free1 = map_info.ribery_map_yc_top_addr[first_stream_ch] - map_base_info.ribery_yc_top_addr;
		free2 = map_info.ribery_map_yc_top_addr[second_stream_ch] - map_info.ribery_map_yc_bottom_addr[first_stream_ch];
		free3 = map_base_info.ribery_yc_bottom_addr - map_info.ribery_map_yc_bottom_addr[second_stream_ch];
		
		if( (free1 > free2) && (free1 > free3) ){
			// free1 case
			map_info.ribery_map_localdec_use		= map_base_info.ribery_localdec_top_addr;
			map_info.ribery_map_localdec_free_size	= map_info.ribery_map_localdec_top_addr[first_stream_ch] - map_base_info.ribery_localdec_top_addr;
			map_info.ribery_map_yc_use				= map_base_info.ribery_yc_top_addr;
			map_info.ribery_map_yc_free_size		= free1;
		}
		else if( (free2 > free1) && (free2 > free3) ){
			// free2 case
			map_info.ribery_map_localdec_use		= map_info.ribery_map_localdec_bottom_addr[first_stream_ch];
			map_info.ribery_map_localdec_free_size	= map_info.ribery_map_localdec_top_addr[second_stream_ch] - map_info.ribery_map_localdec_bottom_addr[first_stream_ch];
			map_info.ribery_map_yc_use				= map_info.ribery_map_yc_bottom_addr[first_stream_ch];
			map_info.ribery_map_yc_free_size		= free2;
		}
		else{
			// free3 case
			map_info.ribery_map_localdec_use		= map_info.ribery_map_localdec_bottom_addr[second_stream_ch];
			map_info.ribery_map_localdec_free_size	= map_base_info.ribery_localdec_bottom_addr - map_info.ribery_map_localdec_bottom_addr[second_stream_ch];
			map_info.ribery_map_yc_use				= map_info.ribery_map_yc_bottom_addr[second_stream_ch];
			map_info.ribery_map_yc_free_size		= free3;
		}
	}
	
	map_info.ribery_map_size[stream_ch]						=  E_H264_IF_IMAGE_SIZE_MAX;
	map_info.ribery_map_reduction_total_size[stream_ch]		=  0;
	map_info.ribery_map_reduction_top_addr[stream_ch]		=  0;
	map_info.ribery_map_reduction_bottom_addr[stream_ch]	=  0;
	map_info.ribery_map_localdec_total_size[stream_ch]		=  0;
	map_info.ribery_map_localdec_top_addr[stream_ch]		=  0;
	map_info.ribery_map_localdec_bottom_addr[stream_ch]		=  0;
	map_info.ribery_map_yc_total_size[stream_ch]			=  0;
	map_info.ribery_map_yc_top_addr[stream_ch]				=  0;
	map_info.ribery_map_yc_bottom_addr[stream_ch]			=  0;
	map_info.ribery_map_count--;
	
	Sdram_Map_Movie_Record_Set_H264_Map_Information( &map_info );
	
	return;
}

/**
 *  @brief		Initialize Vin Address Ctl Data.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_encode_Vin_initialize( VOID )
{
	E_H264_IF_STREAM_CH stream_ch;
	INT32 i;
	
	for (stream_ch = E_H264_IF_STREAM_CH_0; stream_ch < E_H264_IF_STREAM_CH_MAX; stream_ch++){
		gH264_Vin_Adr_Emp_Over_Flg[stream_ch] = H264_OFF;
		
		gH264_Vin_Adr_Ctl[stream_ch].write_point = 0;
		gH264_Vin_Adr_Ctl[stream_ch].read_point = 0;
		gH264_Vin_Adr_Ctl[stream_ch].decide_write_point = 0;
		gH264_Vin_Adr_Ctl[stream_ch].decide_read_point = 0;
		gH264_Vin_Adr_Ctl[stream_ch].hold_max_no = 0;
		gH264_Vin_Adr_Ctl[stream_ch].next_decide_write_point = 0;
		gH264_Vin_Adr_Ctl[stream_ch].next_decide_read_point = 0;
		
		for( i=0; i<D_H264_ENCODE_DECIDE_BANK_FIFO_MAX; i++ ){
			gH264_Vin_Adr_Ctl[stream_ch].next_decide_bank[i] = H264_ENC_VIN_IDX_NODATA;
			gH264_Vin_Adr_Ctl[stream_ch].next_decide_frame_no[i] = 0;
		}
		for( i=0; i<RB_MAP_BNK_ENC_YCC; i++ ){
			gH264_Vin_Adr_Ctl[stream_ch].free_bank_tbl[i] = i;
			gH264_Vin_Adr_Ctl[stream_ch].decide_bank[i] = 0;
			gH264_Vin_Adr_Ctl[stream_ch].decide_frame_no[i] = 0;
			gH264_Vin_Adr_Ctl[stream_ch].duplicate_check_tbl[i] = 0;
		}
	}
	
	return;
}

/**
 *	@brief		Set Ribery input area.\n
 *	@param		i:stream_ch		Stream CH (Ribery CH)
 *	@param		i:bank			Bank Number
 *	@param		i:frame_no		Frame Number
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_encode_Vin_set_decide_bank( E_H264_IF_STREAM_CH stream_ch, INT32 bank, ULLONG frame_no )
{
	if( bank != H264_ENC_VIN_IDX_NODATA ){
		gH264_Vin_Adr_Ctl[stream_ch].decide_bank[ gH264_Vin_Adr_Ctl[stream_ch].decide_write_point ] = bank;
		gH264_Vin_Adr_Ctl[stream_ch].decide_frame_no[ gH264_Vin_Adr_Ctl[stream_ch].decide_write_point ] = frame_no;
		gH264_Vin_Adr_Ctl[stream_ch].decide_write_point++;
		if( gH264_Vin_Adr_Ctl[stream_ch].decide_write_point >= RB_MAP_BNK_ENC_YCC ){
			gH264_Vin_Adr_Ctl[stream_ch].decide_write_point = 0;
		}
		gH264_Vin_Adr_Ctl[stream_ch].hold_max_no++;				// Hold Max Num 3
		//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "ch:%d hold:%d",stream_ch,  gH264_Vin_Adr_Ctl[stream_ch].hold_max_no ));
#if 0	// debug
		H264_Info_Print(( "vsd=  [%d]\n", bank ));
#endif
	}
	return;
}

/**
 *	@brief		Get Ribery input Area.\n
 *	@param		i:stream_ch		Stream CH (Ribery CH)
 *	@param		o:bank			Bank Number
 *	@param		o:frame_no		Frame Number
 *	@return		H264_OK			Normal End
 *	@return		H264_NG			Abnormal End
 *	@note		None
 *	@attention	None
 */
static INT32 h264_encode_Vin_get_decide_bank( E_H264_IF_STREAM_CH stream_ch, INT32* bank, ULLONG* frame_no )
{
	if( gH264_Vin_Adr_Ctl[stream_ch].hold_max_no == 0 ){
		return H264_ENC_VIN_CTL_NONEDATA;
	}
	*bank = gH264_Vin_Adr_Ctl[stream_ch].decide_bank[ gH264_Vin_Adr_Ctl[stream_ch].decide_read_point ];
	*frame_no = gH264_Vin_Adr_Ctl[stream_ch].decide_frame_no[ gH264_Vin_Adr_Ctl[stream_ch].decide_read_point ];
	
#if 0	// debug
	H264_Info_Print(( "vgd=[%d]\n", *bank ));
#endif
	
	gH264_Vin_Adr_Ctl[stream_ch].decide_read_point++;
	if( gH264_Vin_Adr_Ctl[stream_ch].decide_read_point >= RB_MAP_BNK_ENC_YCC ){
		gH264_Vin_Adr_Ctl[stream_ch].decide_read_point = 0;
	}
	gH264_Vin_Adr_Ctl[stream_ch].hold_max_no--;
	//M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "ch:%d hold:%d",stream_ch,  gH264_Vin_Adr_Ctl[stream_ch].hold_max_no ));
	if( gH264_Vin_Adr_Ctl[stream_ch].hold_max_no == 0 ){
		return H264_ENC_VIN_CTL_LASTDATA;
	}
	
	return H264_OK;
}

/**
 *	@brief		Set Empty Ribery input area.\n
 *	@param		i:stream_ch		Stream CH (Ribery CH)
 *	@param		i:bank			Empty Bank Number
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_encode_Vin_set_empty_bank( E_H264_IF_STREAM_CH stream_ch, INT32 bank )
{
	if( gH264_Vin_Adr_Ctl[stream_ch].duplicate_check_tbl[bank] > 0 ){
		gH264_Vin_Adr_Ctl[stream_ch].duplicate_check_tbl[bank]--;
#if 0	// debug
		H264_Info_Print(( "vse_d=[%d]\n", bank ));
#endif
		return;
	}
	gH264_Vin_Adr_Ctl[stream_ch].free_bank_tbl[ gH264_Vin_Adr_Ctl[stream_ch].write_point ] = bank;
	gH264_Vin_Adr_Ctl[stream_ch].write_point++;
	if( gH264_Vin_Adr_Ctl[stream_ch].write_point >= RB_MAP_BNK_ENC_YCC ){
		gH264_Vin_Adr_Ctl[stream_ch].write_point = 0;
	}
	
	gH264_Vin_Adr_Emp_Over_Flg[stream_ch] = H264_OFF;
	
#if 0	// debug
	H264_Info_Print(( "vse  =[%d]\n", bank ));
#endif
	return;
}

/**
 *	@brief		Get Empty Ribery input area.\n
 *	@param		i:stream_ch		Stream CH (Ribery CH)
 *	@param		o:bank			Empty Bank Number
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static INT32 h264_encode_Vin_get_empty_bank( E_H264_IF_STREAM_CH stream_ch, INT32* bank )
{
	if( gH264_Vin_Adr_Emp_Over_Flg[stream_ch] == H264_ON ){
		H264_Err_Print(("H264 Image Bank Over Flow! ch=%d\n",stream_ch));
		return H264_NG;
	}
	
	*bank = gH264_Vin_Adr_Ctl[stream_ch].free_bank_tbl[ gH264_Vin_Adr_Ctl[stream_ch].read_point ];
	gH264_Vin_Adr_Ctl[stream_ch].read_point++;
	if( gH264_Vin_Adr_Ctl[stream_ch].read_point >= RB_MAP_BNK_ENC_YCC ){
		gH264_Vin_Adr_Ctl[stream_ch].read_point = 0;
	}
	
	if( gH264_Vin_Adr_Ctl[stream_ch].write_point == gH264_Vin_Adr_Ctl[stream_ch].read_point ){
		gH264_Vin_Adr_Emp_Over_Flg[stream_ch] = H264_ON;
	}
#if 0	// debug
	H264_Info_Print(( "vge=  [%d]\n", *bank ));
#endif
	return H264_OK;
}

/**
 *	@brief		Get Ribery input address.\n
 *	@param		i:stream_ch		Encode Stream Channel.
 *	@param		i:frame_no		Encode Frame Number.
 *	@param		i:size			Encode Size.
 *	@param		o:y_addr		Y Address Pointer.
 *	@param		o:c_addr		C Address Pointer.
 *	@param		o:g_width		Global Width for Encode Bank.
 *	@return		H264_OK			Normal End
 *	@return		H264_NG			Abnormal End (Return Dummy Address)
 *	@note		None
 *	@attention	None
 */
static INT32 h264_encode_Vin_get_address( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no, E_H264_IF_IMAGE_SIZE size, T_H264_IF_GET_IMAGE_ADDR_PARAM* addr_param )
{
	INT32			bank = 0;
	INT32			ret;
	OS_USER_ER		ercd;
	
	ercd = OS_User_Twai_Sem(SID_H264, H264_ENC_SEMAPHORE_TIMEOUT);
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(( "h264_encode_Vin_get_address: semaphore NG\n" ));
		Sdram_Map_Movie_Record_Get_H264_Vin_Dummy( stream_ch, size, addr_param );
		OS_User_Sig_Sem(SID_H264);
		return H264_NG;
	}
	
	ret = h264_encode_Vin_get_empty_bank( stream_ch, &bank );
	if( ret != H264_OK ){
		Sdram_Map_Movie_Record_Get_H264_Vin_Dummy( stream_ch, size, addr_param );
		FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "over" ));
		OS_User_Sig_Sem(SID_H264);
		return H264_NG;
	}
	
	gH264_Vin_Adr_Ctl[stream_ch].next_decide_bank[gH264_Vin_Adr_Ctl[stream_ch].next_decide_write_point] = bank;
	gH264_Vin_Adr_Ctl[stream_ch].next_decide_frame_no[gH264_Vin_Adr_Ctl[stream_ch].next_decide_write_point] = frame_no;
	
	gH264_Vin_Adr_Ctl[stream_ch].next_decide_write_point++;
	if( gH264_Vin_Adr_Ctl[stream_ch].next_decide_write_point >= D_H264_ENCODE_DECIDE_BANK_FIFO_MAX ){
		gH264_Vin_Adr_Ctl[stream_ch].next_decide_write_point = 0;
	}
	if( gH264_Vin_Adr_Ctl[stream_ch].next_decide_write_point == gH264_Vin_Adr_Ctl[stream_ch].next_decide_read_point ){
		H264_Err_Print(("H264 Address Get Duplicate!\n"));
		Sdram_Map_Movie_Record_Get_H264_Vin_Dummy( stream_ch, size, addr_param );
		OS_User_Sig_Sem(SID_H264);
		return H264_NG;
	}
	
	OS_User_Sig_Sem(SID_H264);
	
	Sdram_Map_Movie_Record_Get_H264_Vin( stream_ch, bank, addr_param );
	return H264_OK;
}

/**
 *	@brief		Decide Ribery input address.\n
 *	@param		i:stream_ch		Encode Stream Channel.
 *	@param		i:frame_no		Encode Frame Number.
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_encode_Vin_decide_address( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no )
{
	INT32			ret = 0;
	OS_USER_ER		ercd;
	INT32			erace_bank;
	ULLONG			erace_frame_no;
	
	ercd = OS_User_Twai_Sem(SID_H264, H264_ENC_SEMAPHORE_TIMEOUT);
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(( "h264_encode_Vin_decide_address: semaphore NG\n" ));
		OS_User_Sig_Sem(SID_H264);
		return;
	}
	
	if( gH264_Vin_Adr_Ctl[stream_ch].next_decide_frame_no[gH264_Vin_Adr_Ctl[stream_ch].next_decide_read_point] != frame_no ){
		H264_Err_Print(("H264 frameNo different!\n"));
	}
	h264_encode_Vin_set_decide_bank( stream_ch,
									 gH264_Vin_Adr_Ctl[stream_ch].next_decide_bank[gH264_Vin_Adr_Ctl[stream_ch].next_decide_read_point],
									 gH264_Vin_Adr_Ctl[stream_ch].next_decide_frame_no[gH264_Vin_Adr_Ctl[stream_ch].next_decide_read_point] );
	
	gH264_Vin_Adr_Ctl[stream_ch].next_decide_read_point++;
	if( gH264_Vin_Adr_Ctl[stream_ch].next_decide_read_point >= D_H264_ENCODE_DECIDE_BANK_FIFO_MAX ){
		gH264_Vin_Adr_Ctl[stream_ch].next_decide_read_point = 0;
	}	
	
	if( gH264_Vin_Adr_Ctl[stream_ch].hold_max_no > 2 ){
		// Free to oldest Bank 
		if( E_H264_IF_RECORD_STATUS_RUNNING == H264_If_Record_Api_Get_Status() ){
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H264 Movie later %u-0x%lX", stream_ch, (ULONG)frame_no ));
			if( gH264_MovieLater_Count == 0 ){
				H264_Warning_Print(( "H264 Movie task later!! ch[%d] (timming NG) \n", stream_ch ));
			}
			gH264_MovieLater_Count++;
			gH264_MovieLater_Total_Count++;
			if( gH264_MovieLater_Count > D_H264_ENCODE_LATER_THRESHOLD ){
				gH264_MovieLater_Count = 0;
			}
		}
		ret = h264_encode_Vin_get_decide_bank( stream_ch, &erace_bank, &erace_frame_no );
		if( ret != H264_OK ){
			H264_Err_Print(( "H264 Vin decide NG1\n" ));
			OS_User_Sig_Sem(SID_H264);
			return;
		}
		h264_encode_Vin_set_empty_bank( stream_ch, erace_bank );
	}
	OS_User_Sig_Sem(SID_H264);
	return;
}

/**
 *	@brief		Set video stream information parameter.\n
 *	@param		i:T_H264_ENC_VIDEO_INFO			video_info
 *	@param		o:T_H264_IF_VIDEO_STREAM_PARAM*	video_stream_info
 *	@param		o:ULLONG* 						total_duration
 *	@return		none
 *	@note		None
 *	@attention	None
 */
static VOID h264_encode_video_stream_param_set(T_H264_ENC_VIDEO_INFO* video_info, UINT32 sample_index, T_H264_IF_VIDEO_STREAM_PARAM* video_stream_info, ULLONG* total_duration)
{
	USHORT nal_cnt;
	
	video_stream_info->stream_ch		= video_info->stream_ch[sample_index];
	video_stream_info->frame_no			= video_info->frame_no[sample_index];
	
	switch( gH264_Rec_Param.codec_param[video_info->stream_ch[sample_index]].size ){
		case E_H264_IF_IMAGE_SIZE_4096_2304 :
			video_stream_info->width = 4096;
			video_stream_info->lines = 2304;
			break;
		case E_H264_IF_IMAGE_SIZE_4096_2160 :
			video_stream_info->width = 4096;
			video_stream_info->lines = 2160;
			break;
		case E_H264_IF_IMAGE_SIZE_4096_2048 :
			video_stream_info->width = 4096;
			video_stream_info->lines = 2048;
			break;
		case E_H264_IF_IMAGE_SIZE_3840_2160 :
			video_stream_info->width = 3840;
			video_stream_info->lines = 2160;
			break;
		case E_H264_IF_IMAGE_SIZE_3840_1920 :
			video_stream_info->width = 3840;
			video_stream_info->lines = 1920;
			break;
		case E_H264_IF_IMAGE_SIZE_3000_3000 :
			video_stream_info->width = 3008;
			video_stream_info->lines = 3008;
			break;			
		case E_H264_IF_IMAGE_SIZE_2704_2028 :
			video_stream_info->width = 2704;
			video_stream_info->lines = 2028;
			break;
		case E_H264_IF_IMAGE_SIZE_2704_1520 :
			video_stream_info->width = 2704;
			video_stream_info->lines = 1520;
			break;
		case E_H264_IF_IMAGE_SIZE_2560_1440 :
			video_stream_info->width = 2560;
			video_stream_info->lines = 1440;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_1440 :
			video_stream_info->width = 1920;
			video_stream_info->lines = 1440;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_1080 :
			video_stream_info->width = WIDTH_FHD;
			video_stream_info->lines = LINES_FHD;
			break;
		case E_H264_IF_IMAGE_SIZE_1920_960 :
			video_stream_info->width = 1920;
			video_stream_info->lines = 960;
			break;
		case E_H264_IF_IMAGE_SIZE_1440_1080 :
			video_stream_info->width = WIDTH_1440;
			video_stream_info->lines = LINES_1440;
			break;
		case E_H264_IF_IMAGE_SIZE_1440_720 :
			video_stream_info->width = WIDTH_1440;
			video_stream_info->lines = 720;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_1024 :
			video_stream_info->width = 1280;
			video_stream_info->lines = 1024;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_960 :
			video_stream_info->width = 1280;
			video_stream_info->lines = 960;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_720 :
			video_stream_info->width = 1280;
			video_stream_info->lines = 720;
			break;
		case E_H264_IF_IMAGE_SIZE_1280_768 :
			video_stream_info->width = 1280;
			video_stream_info->lines = 768;
			break;
		case E_H264_IF_IMAGE_SIZE_1024_768 :
			video_stream_info->width = 1024;
			video_stream_info->lines = 768;
			break;
		case E_H264_IF_IMAGE_SIZE_960_540 :
			video_stream_info->width = 960;
			video_stream_info->lines = 540;
			break;
		case E_H264_IF_IMAGE_SIZE_960_480 :
			video_stream_info->width = 960;
			video_stream_info->lines = 480;
			break;
		case E_H264_IF_IMAGE_SIZE_800_600 :
			video_stream_info->width = 800;
			video_stream_info->lines = 600;
			break;
		case E_H264_IF_IMAGE_SIZE_864_480 :
			video_stream_info->width = 864;
			video_stream_info->lines = 480;
			break;
		case E_H264_IF_IMAGE_SIZE_848_480 :
			video_stream_info->width = 848;
			video_stream_info->lines = 480;
			break;
		case E_H264_IF_IMAGE_SIZE_720_576 :
			video_stream_info->width = 720;
			video_stream_info->lines = 576;
			break;
		case E_H264_IF_IMAGE_SIZE_720_480 :
			video_stream_info->width = 720;
			video_stream_info->lines = 480;
			break;
		case E_H264_IF_IMAGE_SIZE_640_480 :
			video_stream_info->width = WIDTH_VGA;
			video_stream_info->lines = LINES_VGA;
			break;
		case E_H264_IF_IMAGE_SIZE_640_360 :
			video_stream_info->width = WIDTH_HVGAW;
			video_stream_info->lines = LINES_HVGAW;
			break;
		case E_H264_IF_IMAGE_SIZE_480_270 :
			video_stream_info->width = 480;
			video_stream_info->lines = 270;
			break;
		case E_H264_IF_IMAGE_SIZE_432_240 :
			video_stream_info->width = 432;
			video_stream_info->lines = 240;
			break;
		case E_H264_IF_IMAGE_SIZE_320_240 :
			video_stream_info->width = WIDTH_QVGA;
			video_stream_info->lines = LINES_QVGA;
			break;
		case E_H264_IF_IMAGE_SIZE_1600_896:
			video_stream_info->width = 1600;
			video_stream_info->lines = 896;
			break;
		case E_H264_IF_IMAGE_SIZE_1024_576:
			video_stream_info->width = 1024;
			video_stream_info->lines = 576;
			break;
		case E_H264_IF_IMAGE_SIZE_800_448:
			video_stream_info->width = 800;
			video_stream_info->lines = 448;
			break;
		case E_H264_IF_IMAGE_SIZE_352_288:
			video_stream_info->width = 352;
			video_stream_info->lines = 288;
			break;
		case E_H264_IF_IMAGE_SIZE_2592_1944:
			video_stream_info->width = 2592;
			video_stream_info->lines = 1944;
			break;
		case E_H264_IF_IMAGE_SIZE_2304_1296:
			video_stream_info->width = 2304;
			video_stream_info->lines = 1296;
			break;
	  	case E_H264_IF_IMAGE_SIZE_1920_1280:
			video_stream_info->width = 1920;
			video_stream_info->lines = 1280;
			break;
	   	case E_H264_IF_IMAGE_SIZE_640_512:
			video_stream_info->width = 640;
			video_stream_info->lines = 512;
			break;
		case E_H264_IF_IMAGE_SIZE_384_288:
			video_stream_info->width = 384;
			video_stream_info->lines = 288;
			break;
		default :
			H264_Err_Print(("--- H264 Size Error!! size=%d\n", gH264_Rec_Param.codec_param[video_info->stream_ch[sample_index]].size ));
			break;
	}
	video_stream_info->stream_serial_num		= video_info->vstrem_serial_num[sample_index];
	video_stream_info->stream_end_flg			= video_info->stream_end_flg[sample_index];
	video_stream_info->vstream_index			= video_info->vstream_free_index[sample_index];
	video_stream_info->sample_address			= video_info->first_param[sample_index].sample_address;
	video_stream_info->sample_size				= video_info->first_param[sample_index].sample_size;
	video_stream_info->sample_type				= video_info->first_param[sample_index].sample_type;
	video_stream_info->sample_poc				= video_info->poc[sample_index];
	video_stream_info->frame_of_GOP				= video_info->frame_count_in_gop[sample_index];
	video_stream_info->VBV_buffer				= video_info->first_param[sample_index].VBV_buffer;
	video_stream_info->nal_unit_num				= video_info->first_param[sample_index].nal_unit_num;
	for (nal_cnt = 0; nal_cnt < H264_NAL_UNIT_MAX_NUM; nal_cnt++) {
		video_stream_info->nal_unit_size[nal_cnt] = video_info->first_param[sample_index].nal_unit_size[nal_cnt];
	}
	// time_increment -> 90KHz
	video_stream_info->sample_pts				= ((LLONG)(video_info->first_param[sample_index].sample_pts * H264_PTS_90K_BASE) / gH264_Enc_Ctrl.video[ video_info->stream_ch[sample_index] ].time_scale);
	video_stream_info->sample_dts				= ((LLONG)(video_info->first_param[sample_index].sample_dts * H264_PTS_90K_BASE) / gH264_Enc_Ctrl.video[ video_info->stream_ch[sample_index] ].time_scale);
	video_stream_info->bitrate					= video_info->first_param[sample_index].bitrate;
	video_stream_info->ave_bitrate				= video_info->ave_bitrate[sample_index];
	video_stream_info->ave_qp					= video_info->first_param[sample_index].ave_qp;
	video_stream_info->cpb_over_flag			= video_info->first_param[sample_index].cpb_over_flag;
	video_stream_info->stream_buffer0_limit		= video_info->stream_buffer0_limit[sample_index];
	video_stream_info->stream_buffer1_limit		= video_info->stream_buffer1_limit[sample_index];
	
	*total_duration += gH264_Enc_Ctrl.video[ video_info->stream_ch[sample_index] ].time_increment;
	
	return;
}

/**
 *	@brief		Encode Task End process.\n
 *	@param		i:enc_result		Encode Result
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_encode_result_notify( INT32 enc_result )
{
	INT32 ercd;
	OS_USER_FLGPTN flgptn;
	
	// Ribery area over write this timming Finalize
	
	// befor File write STOP trigger send
	if( enc_result == H264_OK_NORMAL_END ){
		H264_If_Record_Notify_NORMAL_END();
	}
	else{
		ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_ERROR_VRAW_MULTI | FLG_H264_ERROR_VSTRM_EMPTY, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ){
			if( 0 != (flgptn & FLG_H264_ERROR_VRAW_MULTI) ){
				H264_If_Record_Notify_VRAW_MULTI_ERR();
			}
			else if( 0 != (flgptn & FLG_H264_ERROR_VSTRM_EMPTY) ){
				H264_If_Record_Notify_VSTRM_EMPTY_ERR();
			}
		}
		else{
			H264_If_Record_Notify_FATAL_ERR();
		}
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
static VOID h264_encode_free_vstream( VOID )
{
	USHORT i;
	USHORT loop;
	USHORT comp_count = 0;
	
	OS_User_Wai_Sem(SID_H264_STREAM);
	
	if( gH264_Free_Vstream_Count == 0 ){
		OS_User_Sig_Sem(SID_H264_STREAM);
		return;
	}
	
	loop = gH264_Free_Vstream_Count/RB_FIFO_MAX_COUNT_NUM + 1;
	for(i = 0; i < loop; i++){
		if( gH264_Free_Vstream_Count < RB_FIFO_MAX_COUNT_NUM ){
			H264_Ribery_Enc_VSTREAM_Free( gH264_Free_Vstream_Count, (T_RB_FIFO_INDEX*)&gH264_Free_Vstream_Param[comp_count] );
			gH264_Free_Vstream_Count = 0;
		}
		else{
			H264_Ribery_Enc_VSTREAM_Free( RB_FIFO_MAX_COUNT_NUM, (T_RB_FIFO_INDEX*)&gH264_Free_Vstream_Param[comp_count] );
			gH264_Free_Vstream_Count -= RB_FIFO_MAX_COUNT_NUM;
			comp_count += RB_FIFO_MAX_COUNT_NUM;
		}
	}
	OS_User_Sig_Sem(SID_H264_STREAM);
	
	return;
}

#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
/* for Debug (view task dummy) */
/**
 *  @brief		Encode DISP Main Processing.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Encode_DISP_Main( VOID )
{
	OS_USER_FLGPTN			flgptn;
	OS_USER_ER				ercd;
	OS_USER_FLGPTN			waiptn;
	E_H264_IF_STREAM_CH		stream_ch;
	ULONG					frame_no[E_H264_IF_STREAM_CH_MAX] = {0,0,0};
	ULONG					frame_PTS[E_H264_IF_STREAM_CH_MAX] = {0,0,0};
	T_H264_IF_GET_IMAGE_ADDR_PARAM addr_param;
	
	waiptn = FLG_H264_SYNC_ENC_DUMMY_CH0 | FLG_H264_SYNC_ENC_DUMMY_CH1 | FLG_H264_SYNC_ENC_DUMMY_CH2;
	
	while(1){
		ercd = OS_User_Wai_Flg( FID_H264_SYNC, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd != D_OS_USER_E_OK ){
			H264_Info_Print(("+++ H264_DISP: wai_flg ercd=%x\n", ercd ));
		}
		
		if( 0 != (flgptn & FLG_H264_SYNC_ENC_DUMMY_CH0) ){
			OS_User_Clr_Flg( FID_H264_SYNC, ~FLG_H264_SYNC_ENC_DUMMY_CH0 );
			stream_ch = E_H264_IF_STREAM_CH_0;
//			printf("disp:sync ch0\n");
		}
		else if( 0 != (flgptn & FLG_H264_SYNC_ENC_DUMMY_CH1) ){
			OS_User_Clr_Flg( FID_H264_SYNC, ~FLG_H264_SYNC_ENC_DUMMY_CH1 );
			stream_ch = E_H264_IF_STREAM_CH_1;
//			printf("disp:sync ch1\n");
		}
		else if( 0 != (flgptn & FLG_H264_SYNC_ENC_DUMMY_CH2) ){
			OS_User_Clr_Flg( FID_H264_SYNC, ~FLG_H264_SYNC_ENC_DUMMY_CH2 );
			stream_ch = E_H264_IF_STREAM_CH_2;
//			printf("disp:sync ch2\n");
		}
		else{
			H264_Err_Print(("+++ H264_DISP: Sync Wait: The route which is not assumed.\n" ));
			stream_ch = E_H264_IF_STREAM_CH_0;
		}
		
		if( gH264_Rec_Param.codec_param[stream_ch].enable_flg == H264_ON ){
			H264_Encode_Image_Address_Get(stream_ch, gH264_Rec_Param.codec_param[stream_ch].size, frame_no[stream_ch], &addr_param );
			H264_Encode_Image_Address_Decide( stream_ch, frame_no[stream_ch], addr_param.dummy_flg );
			frame_no[stream_ch]++;
			frame_PTS[stream_ch] += gH264_Enc_Ctrl.video[stream_ch].time_increment;
		}
		else{
			H264_Info_Print(("+++ H264_DISP: Sync of the invalid channel occurred.\n" ));
		}
		
		ercd = OS_User_Pol_Flg( FID_H264, FLG_H264_CC_STOP, D_OS_USER_TWF_ORW, &flgptn );
		if( ercd == D_OS_USER_E_OK ){
			break;
		}
		else if( ercd == D_OS_USER_E_TMOUT ) {
			;
		}
		else{
			H264_Err_Print(("--- H264_DISP: pol_flg ercd=%x\n", ercd ));
		}
	}
	// Set DISP Task Stop Event 
	OS_User_Set_Flg( FID_H264, FLG_H264_DISP_STOP );
	
	return;
}
#endif

/**
 * Valid Image Area Address on Movie Mode
 * @param		stream_ch		stream channel
 * @return		None
 * @attention	None
 */
VOID H264_Encode_Vin_Address_Valid( E_H264_IF_STREAM_CH stream_ch )
{
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "MapEnable ON[%u]", stream_ch ));
	gH264_Map_Enable_Flg[stream_ch] = H264_ON;
//	printf("gH264_Map_Enable_Flg is ON!!\n");
	return;
}

/**
 * Set Dynamic Parameter  on Movie Mode
 * @param		stream_ch				stream channel
 * @param		dynamic_param			dynamic parameter
 * @return		None
 * @attention	None
 */
VOID H264_Encode_Dynamic_Parameter_Set( E_H264_IF_STREAM_CH stream_ch, T_H264_IF_DYNAMIC_PARAM_RECORD* dynamic_param )
{
	gH264_Dynamic_Param[stream_ch].bitrate				= dynamic_param->bitrate;
	gH264_Dynamic_Param[stream_ch].frame_num_of_gop		= dynamic_param->frame_num_of_gop;
	gH264_Dynamic_Param[stream_ch].gop_struct			= dynamic_param->gop_struct;
	gH264_Dynamic_Param[stream_ch].gop_num_idr_to_idr	= dynamic_param->gop_num_idr_to_idr;
	gH264_Dynamic_Param[stream_ch].closed_gop			= dynamic_param->closed_gop;
	
	gH264_Dynamic_Change_Flag[stream_ch] = H264_ON;
	
	return;
}

/**
 * Set Insert IDR Trriger  on Movie Mode
 * @param		stream_ch				stream channel
 * @return		None
 * @attention	None
 */
VOID H264_Encode_Insert_IDR_Trriger_Set( E_H264_IF_STREAM_CH stream_ch )
{
	gH264_Insert_IDR_Flag[stream_ch] = H264_ON;
//	printf("[H264]Insert_IDR_Flag[%d] is ON!!\n", stream_ch);
	return;
}

