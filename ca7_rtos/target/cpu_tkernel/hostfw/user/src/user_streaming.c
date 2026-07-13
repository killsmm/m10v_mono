/**
 * @file		user_streaming.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "user.h"
#include "arb.h"
#include "category_parameter.h"
#include "share_data.h"
#include "user_utility_disp.h"
#include "user_streaming.h"
#include "fj_disp.h"
#include "fj_decoder.h"
#include "fj_filesystem.h"
#include "fj_memory.h"
#include "user_sync_task.h"
#include "sdram_map_define.h"
#include "os_user_custom.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_STREAMING_MAX_SAMPLE 120
#define D_STREAMING_MBX_MAX	D_STREAMING_MAX_SAMPLE

#define ERROR_HANDLE(rval) \
	UPRINTF_ERR(rval)\
	if (rval) goto end;

#define D_STACK_SIZE 0x200
/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	OS_USER_VP	_os_reserve[2];	// OS reserve
	T_CPUCMD_VIDEO_DEC_STREAM str_info;
} T_STREAMING_MBX;

typedef struct {
	UINT32				head;
	UINT32				num;
	T_STREAMING_MBX*	data[D_STREAMING_MBX_MAX];
} T_STREAMING_MBX_DTQ;

typedef struct {
	UCHAR	vid_id;
	ULLONG	frame_cnt;
	BOOL	streaming_decode;
	BOOL	end_frame;
} T_STREAMING_STAT;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static volatile T_STREAMING_MBX		g_str_mbx[D_STREAMING_MBX_MAX] __attribute__((section(".LOCK_SECTION"), aligned(64)));
static volatile T_STREAMING_MBX_DTQ	g_str_mbx_empty_dtq __attribute__((section(".LOCK_SECTION"), aligned(64)));
static volatile ULONG				g_str_spin_lock	__attribute__((section(".LOCK_SECTION"), aligned(64)));

static E_USER_OP_STATE g_user_streaming_op_state = E_USER_OP_STR_DEC_IDLE;
static T_SHARE_STREAM_INFO g_stream_info[E_MAX_STREAM];
static T_STREAMING_STAT g_stat[E_MAX_STREAM];
static T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM g_stream_param[E_MAX_STREAM][D_STREAMING_MAX_SAMPLE];
static OS_USER_ID g_task_id;
static UCHAR g_task_stack[D_STACK_SIZE];

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void rcv_event( T_CPUCMD_VIDEO_DEC_STREAM* str_info);
static INT32 enqueue( T_STREAMING_MBX* data );
static INT32 dequeue( T_STREAMING_MBX** data );

static int user_streaming_start(T_SHARE_STREAM_INFO *info);
static void stream_recv_ipcu_task(void);

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
E_USER_STATE User_Streaming( E_USER_EVENT event, ULLONG frame_no )
{
	T_OS_USER_CTSK 		ctsk;
	E_USER_STATE state	= E_USER_STATE_BUSY;
	INT32 rval 			= NG;
	int	i;

	switch( event )	{
		case E_USER_EVENT_STREAMING:
			g_stat[E_MAIN_STREAM].streaming_decode = TRUE;
			g_stat[E_SUB_STREAM].streaming_decode = TRUE;
			Set_Arb_MXIC(0);
			Set_Arb_SDRAMC(2);

			OS_IF_Rtos_IPCU_CH67_Switch(E_IPCU_CH67_DECODE);
			User_Utility_disp_switch(Cate_Get_Disp_If());
			g_user_streaming_op_state = E_USER_OP_STR_SETUP_DONE;
		break;

		case E_USER_EVENT_STREAM_DEC_START:
			Cate_Get_Stream_Info(g_stream_info);

			g_stat[E_MAIN_STREAM].frame_cnt = 0;
			g_stat[E_SUB_STREAM].frame_cnt = 0;
			g_stat[E_MAIN_STREAM].end_frame = FALSE;
			g_stat[E_SUB_STREAM].end_frame = FALSE;
			g_stat[E_MAIN_STREAM].vid_id = 0xFF;
			g_stat[E_SUB_STREAM].vid_id = 0xFF;

			// reset mailbox queue
			g_str_mbx_empty_dtq.head = 0;
			g_str_mbx_empty_dtq.num = 0;
			memset( (void*)g_str_mbx, 0, sizeof(g_str_mbx) );
			for (i = 0; i < D_STREAMING_MBX_MAX; i++) {
				enqueue( (T_STREAMING_MBX*)&g_str_mbx[i] );
			}

			ctsk.tskatr  = 0;
			ctsk.exinf   = (OS_USER_VP_INT)0;
			ctsk.task    = (OS_USER_FP)stream_recv_ipcu_task;
			ctsk.itskpri = (OS_USER_PRI)25;
			ctsk.stksz   = (OS_USER_SIZE)D_STACK_SIZE;
			ctsk.stk     = (OS_USER_VP)g_task_stack;
#if D_DDIM_USER_KERNEL
			ctsk.AssignProcessor  = (OS_USER_UW)1;
#endif
			g_task_id = OS_User_Acre_Tsk(&ctsk);
			if (g_task_id < 0) {
				UPRINTF_ERR(g_task_id);
				rval = -1;
			} else {
				rval = OS_User_Sta_Tsk(g_task_id, 0);
				UPRINTF_ERR(rval);
			}

			if (rval == OK)
				rval = user_streaming_start(&g_stream_info[0]);

			if (rval == OK) {
				g_user_streaming_op_state = E_USER_OP_STR_DEC_RUN;
			}
			break;

		case E_USER_EVENT_STREAM_DEC_STOP:
			rval = User_StreamingStop((E_FJ_VIDEO_DEC_ENUM)frame_no);

			if ((rval == OK && g_stream_info[E_SUB_STREAM].type == E_FJ_VIDEO_DEC_NONE) ||
				(rval == OK && g_stream_info[E_SUB_STREAM].type == (E_FJ_VIDEO_DEC_ENUM)frame_no)) {
				rval = OS_User_Ter_Tsk(g_task_id);
				rval |= OS_User_Del_Tsk(g_task_id);
				UPRINTF_ERR(rval);
				g_user_streaming_op_state = E_USER_OP_STR_DEC_IDLE;
			}
			break;

		case E_USER_EVENT_STANDBY:
			OS_IF_Rtos_IPCU_CH67_Switch(E_IPCU_CH67_ENCODE);
			g_stat[E_MAIN_STREAM].streaming_decode = FALSE;
			g_stat[E_SUB_STREAM].streaming_decode = FALSE;
			g_user_streaming_op_state = E_USER_OP_NONE;
			state = E_USER_STATE_IDLE;
		break;

		default:
		break;
	}

	return state;
}

E_USER_OP_STATE User_Streaming_Get_OP_Stat( VOID )
{
	return g_user_streaming_op_state;
}

VOID User_Streaming_Clear_OP_Stat( VOID )
{
	g_user_streaming_op_state = E_USER_OP_NONE;
}

BOOL User_IsStreamingProcess(void)
{
	return g_stat[E_MAIN_STREAM].streaming_decode;
}

BOOL User_IsH265MainStream(void)
{
	if (g_stat[E_MAIN_STREAM].streaming_decode &&
		g_stream_info[E_MAIN_STREAM].type == E_FJ_VIDEO_DEC_H265) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL User_IsDualStream(void)
{
	if (g_stat[E_MAIN_STREAM].streaming_decode &&
		g_stream_info[E_MAIN_STREAM].type != E_FJ_VIDEO_DEC_NONE &&
		g_stream_info[E_SUB_STREAM].type != E_FJ_VIDEO_DEC_NONE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int User_StreamingStop(E_FJ_VIDEO_DEC_ENUM codec)
{
	int rval = 0;

	rval = FJ_VideoPlayback_StopVideoDecode(codec, E_FJ_MOVIE_PLAY_LAST_PICTURE_KEEP_STOP);
	ERROR_HANDLE(rval);

	rval = FJ_VideoDecoder_Disconnect((UCHAR)codec, Cate_Get_Disp_If(), g_stat[E_SUB_STREAM].vid_id == codec ? TRUE : FALSE);
	ERROR_HANDLE(rval);

	rval = FJ_VideoDecoder_Close((UCHAR)codec);
	ERROR_HANDLE(rval);

end:
	return rval;
}

#if defined(D_STREAMING_FROM_FILE)
int User_StreamingGetFrameASync(E_FJ_VIDEO_DEC_ENUM codec, unsigned char num_frame)
{
	T_USER_SYNC_MSG msg;
	OS_USER_ER os_err = 0;

	msg.event		= E_USER_SYNC_EVENT_FEED_STREAM;
	msg.frame_no	= (ULLONG)num_frame;
	msg.param		= (ULONG)codec;
	os_err = Snd_Msg_To_User_Sync(&msg);
	UPRINTF_ERR(os_err);

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "S - codec:%d ", codec));
	return os_err;
}
#endif

#if defined(D_STREAMING_FROM_FILE)
int User_StreamingGetFrameSync(E_FJ_VIDEO_DEC_ENUM codec, unsigned char num_frame)
{
	E_STREAM_NUM stream_idx = E_MAIN_STREAM;
	ULONG frame_addr;
	int rval = 0;
	int i;
	CHAR dname[16] = "I:\\stream";
	CHAR fname[32];
	FJ_FS_STAT f_stat;
	ULONG read_size;
	ULONG fid;

	//memset(&g_stream_param, 0, sizeof(g_stream_param));

	stream_idx = codec == g_stat[E_MAIN_STREAM].vid_id ? E_MAIN_STREAM : E_SUB_STREAM;

	for (i = 0; i < num_frame && g_stat[stream_idx].end_frame == FALSE; i++) {

		if (codec == E_FJ_VIDEO_DEC_H264) {
			sprintf( fname, "%s\\H264%04d", dname, (INT32)g_stat[stream_idx].frame_cnt );
		} else {
			sprintf( fname, "%s\\H265%04d", dname, (INT32)g_stat[stream_idx].frame_cnt );
		}

		rval = FJ_Fs_Stat(fname, &f_stat);
		ERROR_HANDLE(rval);

		fid = FJ_Fs_Open ( fname, (FJ_FS_OPEN_RDONLY) );
		if (fid == 0) {
			rval = -1;
			ERROR_HANDLE(rval);
		}

		rval = FJ_VideoPlayback_MemAlloc( codec, (UINT32*)&f_stat.size, (UINT32*)&frame_addr );
		ERROR_HANDLE(rval);

		rval = FJ_Fs_Read(fid, (UINT32*)frame_addr, f_stat.size, &read_size);
		ERROR_HANDLE(rval);

		FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (ULONG)frame_addr, f_stat.size);

		g_stream_param[stream_idx][i].dts = g_stat[stream_idx].frame_cnt * g_stream_info[stream_idx].t_delta;
		g_stream_param[stream_idx][i].pts = g_stat[stream_idx].frame_cnt * g_stream_info[stream_idx].t_delta;
		g_stream_param[stream_idx][i].stream_address = frame_addr;
		g_stream_param[stream_idx][i].stream_size = f_stat.size;
		g_stream_param[stream_idx][i].stss = 0;
		g_stream_param[stream_idx][i].stream_end_flag = g_stat[stream_idx].frame_cnt == (g_stream_info[stream_idx].total_frame_num - 1) ? 1 : 0;

		FJ_Fs_Close( fid );
		g_stat[stream_idx].frame_cnt++;
		if (g_stream_param[stream_idx][i].stream_end_flag) g_stat[stream_idx].end_frame = TRUE;
	}

	if (i > 0) {
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "E - codec:%d PTS:%lu", codec, (ULONG)(g_stat[stream_idx].frame_cnt - 1) * g_stream_info[stream_idx].t_delta));
		rval = FJ_VideoPlayback_SetVideoStream( codec, i, &g_stream_param[stream_idx][0] );
		ERROR_HANDLE(rval);
	}
end:
	return rval;
}
#endif

INT32 User_StreamingDecodeOneFrameDoneCB( E_FJ_VIDEO_DEC_ENUM codec, E_FJ_MOVIE_PLAY_DECODER_STATUS status, ULLONG pts, BOOL err )
{
	int rval = 0;

	if (status == E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST || err) {
		UPRINTF_ERR(err);
		Snd_Event_to_User( E_USER_EVENT_STREAM_DEC_STOP, (ULLONG)codec );
	}

	return rval;
}

INT32 User_StreamingFirstFrameDisplayCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error )
{
	INT32 rval = 0;

	rval = User_Utility_Disp_Enable(Cate_Get_Disp_If());
	UPRINTF_ERR(rval);

	return rval;
}

INT32 User_SendStreamToReceiver(T_CPUCMD_VIDEO_DEC_STREAM str_info)
{
	OS_USER_ER			ercd = 0;
	T_STREAMING_MBX*	mbx;

	// Get Memory
	Dd_ARM_Critical_Section_Start( g_str_spin_lock );
	ercd = dequeue( &mbx );
	Dd_ARM_Critical_Section_End( g_str_spin_lock );

	// Error Check
	if( ercd ){
		UPRINTF_ERR(ercd);
		return ercd;
	}

	// Set Data
	mbx->str_info = str_info;

	// Send Memory
	ercd = OS_User_Snd_Mbx( MID_STREAMING, (T_OS_USER_MSG*)mbx );
	UPRINTF_ERR(ercd);
	return ercd;
}

const T_SHARE_STREAM_INFO* User_GetStreamType(VOID)
{
	return &g_stream_info[0];
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void stream_recv_ipcu_task(void)
{
	T_CPUCMD_VIDEO_DEC_STREAM str_info;
	E_STREAM_NUM stream_idx = E_MAIN_STREAM;
	UINT32 frame_addr;
	int rval = 0;

	for (;;) {

		memset(&str_info, 0, sizeof(str_info));
		rcv_event(&str_info);

		if (str_info.t_head.magic_code != D_CPU_IF_MCODE_STREAM_SEND ||
			str_info.t_head.format_version != D_CPU_IF_COM_FORMAT_VERSION ||
			str_info.t_head.cmd_set != E_CPU_IF_COMMAND_SET_STREAM_STR ||
			str_info.t_head.cmd != E_CPU_IF_COMMAND_STREAM_VIDEO) {
			rval = -1;
			ERROR_HANDLE(rval);
		}

		if (g_stat[E_MAIN_STREAM].vid_id == str_info.stype && g_stat[E_MAIN_STREAM].end_frame) goto end;
		if (g_stat[E_SUB_STREAM].vid_id == str_info.stype && g_stat[E_SUB_STREAM].end_frame) goto end;

		if (str_info.sample_size) {
			rval = FJ_VideoPlayback_MemAlloc( str_info.stype, (UINT32*)&str_info.sample_size, (UINT32*)&frame_addr );
			ERROR_HANDLE(rval);
		}

		if (str_info.sample_address < SDRAM_ADR_CH0_TOP || str_info.sample_address > SDRAM_ADR_CH1_END) {
			rval = -1;
			ERROR_HANDLE(rval);
		}
		if (str_info.sample_address > SDRAM_ADR_CH0_END && str_info.sample_address < SDRAM_ADR_CH1_TOP) {
			rval = -1;
			ERROR_HANDLE(rval);
		}

		if (str_info.sample_address >= SDRAM_ADR_CH0_TOP && str_info.sample_address < SDRAM_ADR_CH0_END &&
			(str_info.sample_address + str_info.sample_size >  SDRAM_ADR_CH0_END)) {
			rval = -1;
			ERROR_HANDLE(rval);
		}
		if (str_info.sample_address >= SDRAM_ADR_CH1_TOP && str_info.sample_address < SDRAM_ADR_CH1_END &&
			(str_info.sample_address + str_info.sample_size >  SDRAM_ADR_CH1_END)) {
			rval = -1;
			ERROR_HANDLE(rval);
		}

		memcpy((void*)frame_addr, (void*)str_info.sample_address, str_info.sample_size);
		FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (ULONG)frame_addr, str_info.sample_size);

		OS_IF_Rtos_Stream_Ack(E_CPU_IF_COMMAND_STREAM_VIDEO, str_info.stype);

		stream_idx = str_info.stype == g_stat[E_MAIN_STREAM].vid_id ? E_MAIN_STREAM : E_SUB_STREAM;
		g_stream_param[stream_idx][0].dts = str_info.area_index * g_stream_info[stream_idx].t_delta;
		// if sample_pts is presented, just assign it, otherwise, using frame index * delta
		// IBBP PTS is necessary, IPPP is not necessary
		g_stream_param[stream_idx][0].pts = str_info.sample_pts == 0 ? str_info.area_index * g_stream_info[stream_idx].t_delta : str_info.sample_pts;
		g_stream_param[stream_idx][0].stream_address = frame_addr;
		g_stream_param[stream_idx][0].stream_size = str_info.sample_size;
		g_stream_param[stream_idx][0].stss = 0;
		g_stream_param[stream_idx][0].stream_end_flag = str_info.stream_end_flg;

		if (g_stream_param[stream_idx][0].stream_end_flag) g_stat[stream_idx].end_frame = TRUE;

		g_stat[stream_idx].frame_cnt++;
		rval = FJ_VideoPlayback_SetVideoStream( str_info.stype, 1, &g_stream_param[stream_idx][0] );
		ERROR_HANDLE(rval);
end:
		continue;
	}
}

static int user_streaming_start(T_SHARE_STREAM_INFO *info)
{
	T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM	cfg;
	FJ_DISP_TYPE disp_id = FJ_DISP_TYPE_HDMI;
	T_SHARE_STREAM_INFO *p_info = NULL;
	unsigned int fps_100 = 3000;
	unsigned char frame_no = 0;
	int rval = 0;
	int i;

	disp_id = Cate_Get_Disp_If();

	for (i = 0, p_info = info; i < E_MAX_STREAM && p_info->type != E_FJ_VIDEO_DEC_NONE; i++, p_info++) {
		rval = FJ_VideoDecoder_Open(p_info->type, &g_stat[i].vid_id);
		ERROR_HANDLE(rval);

		if (i == E_MAIN_STREAM) {
			rval = User_Utility_Disp_Set_Window((E_SHARE_DISP_IF)disp_id, (UINT32)p_info->w, (UINT32)p_info->h, FJ_DISP_ROTATE_DEGREE_0 );
			ERROR_HANDLE(rval);
		}

		rval = FJ_VideoDecoder_Connect(g_stat[i].vid_id, (UCHAR)disp_id, i == E_SUB_STREAM ? TRUE : FALSE);
		ERROR_HANDLE(rval);

		switch(p_info->w + p_info->h) {
			case (4000+3000):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_4000_3000;  break;
			case (4096+2304):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2304;  break;
			case (4096+2160):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2160;  break;
			case (4096+2048):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2048;  break;
			case (3840+1920):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_3840_1920;  break;
			case (2704+2028):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_2028;  break;
			case (2704+1520):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_1520;  break;
			case (2560+1920):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_2560_1920;  break;
			case (2560+1440):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_2560_1440;  break;
			case (1920+1440):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1440;  break;
			case (1920+1080):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1080;  break;
			case (1920+960):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_960;   break;
			case (1504+1504):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1504_1504;  break;
			case (1440+1080):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_1080;  break;
			case (1440+720):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_720;   break;
			case (1280+960):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_960;   break;
			case (1280+720):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_720;   break;
			case (960+540):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_960_540;    break;
			case (960+480):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_960_480;    break;
			case (864+480):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_864_480;    break;
			case (848+480):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_848_480;    break;
			case (720+480):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_720_480;    break;
			case (640+480):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_480;    break;
			case (640+360):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_360;    break;
			case (480+270):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_480_270;    break;
			case (432+240):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_432_240;    break;
			case (320+240):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_320_240;    break;
			case (720+576):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_720_576;    break;
			case (1024+768):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1024_768;   break;
			case (1280+768):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_768;   break;
			case (1280+1024):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_1024;  break;
			case (800+600):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_800_600;    break;
			case (6000):
				if (p_info->w == 3000) {
					cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_3000_3000;
				} else {
					cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_3840_2160;
				}
			break;
			case (1600+896):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1600_896;    break;
			case (1024+576):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1024_576;    break;
			case (800+448):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_800_448;    break;
			case (352+288):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_352_288;    break;
			case (2592+1944):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_2592_1944;    break;
			case (2304+1296):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_2304_1296;    break;
			case (1920+1280):	cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1280;    break;
			case (640+512):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_512;    break;
			//case (384+288):		cfg.image_size = E_FJ_MOVIE_PLAY_VIDEO_SIZE_384_288;    break;
			default:
				rval = -1;
				ERROR_HANDLE(rval);
				break;
		}

		cfg.codec_type			= p_info->type;
		cfg.compatibility_mode	= FJ_MOVIE_PLAY_OFF;
		cfg.conceal_error		= FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_SKIP;
		cfg.scan_mode			= FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_PROGRESSIVE;
		cfg.time_delta			= p_info->t_delta;
		cfg.time_scale			= p_info->t_scale;
		cfg.trick				= TRUE;
		rval = FJ_VideoPlayback_SetVideoConfig(&cfg);
		ERROR_HANDLE(rval);

		fps_100 = p_info->t_scale * 100 / p_info->t_delta;
		frame_no = ((fps_100 % 100) == 0) ? fps_100 / 100 / 2 : (fps_100 / 100 + 1) / 2;
		frame_no = frame_no == 0 ? 1 : frame_no;

#if defined(D_STREAMING_FROM_FILE)
		rval = User_StreamingGetFrameSync(p_info->type, frame_no);
		ERROR_HANDLE(rval);
#else
		for (;g_stat[i].frame_cnt < frame_no;) OS_User_Dly_Tsk(100);
#endif
	}

	rval = FJ_Disp_Video_Disable((UCHAR)disp_id);
	ERROR_HANDLE(rval);

	rval = FJ_VideoPlayback_SetStartPTS(0);
	ERROR_HANDLE(rval);

	rval = FJ_VideoPlayback_SetSpeedDirection(E_FJ_MOVIE_PLAY_SPEED_1X, E_FJ_MOVIE_PLAY_DIRECTION_FWD, FALSE);
	ERROR_HANDLE(rval);

	for (i = 0, p_info = info; i < E_MAX_STREAM && p_info->type != E_FJ_VIDEO_DEC_NONE; i++, p_info++) {
		rval = FJ_VideoPlayback_StartVideoDecode(p_info->type);
		ERROR_HANDLE(rval);
	}

end:
	if (rval) {
		for (i = 0, p_info = info; i < E_MAX_STREAM && p_info->type != E_FJ_VIDEO_DEC_NONE; i++, p_info++) {
			FJ_VideoDecoder_Close(p_info->type);
		}
	}
	return rval;
}

static void rcv_event( T_CPUCMD_VIDEO_DEC_STREAM* str_info )
{
	OS_USER_ER			ercd;
	T_STREAMING_MBX*	mbx;

	// Get Memory
	ercd = OS_User_Rcv_Mbx( MID_STREAMING, (T_OS_USER_MSG**)&mbx );
	if( ercd ){
		UPRINTF_ERR(ercd);
		return;
	}

	// Set from Memory
	*str_info = mbx->str_info;

	// Update Memory
	Dd_ARM_Critical_Section_Start( g_str_spin_lock );
	ercd = enqueue( (T_STREAMING_MBX*)mbx );
	Dd_ARM_Critical_Section_End( g_str_spin_lock );

	UPRINTF_ERR(ercd);
}


static INT32 enqueue( T_STREAMING_MBX* data )
{
	if( g_str_mbx_empty_dtq.num < D_STREAMING_MBX_MAX ){
		g_str_mbx_empty_dtq.data[(g_str_mbx_empty_dtq.head+g_str_mbx_empty_dtq.num)%D_STREAMING_MBX_MAX] = data;
		g_str_mbx_empty_dtq.num++;
		return D_DDIM_USER_E_OK;
	}
	else{
		return D_DDIM_USER_E_SYS; // full
	}
}


static INT32 dequeue( T_STREAMING_MBX** data )
{
	if( g_str_mbx_empty_dtq.num > 0 ){
		*data = g_str_mbx_empty_dtq.data[g_str_mbx_empty_dtq.head];
		g_str_mbx_empty_dtq.head = (g_str_mbx_empty_dtq.head+1)%D_STREAMING_MBX_MAX;
		g_str_mbx_empty_dtq.num--;
		return D_DDIM_USER_E_OK;
	}
	else{
		return D_DDIM_USER_E_QOVR; // empty
	}
}
