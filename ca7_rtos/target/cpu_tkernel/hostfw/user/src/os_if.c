/**
 * @file		os_if.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "os_if.h"
#include "fj_arm.h"
#include "os_user_custom.h"
#include "category_parameter.h"
#include "user.h"
#include "user_utility_jpeg.h"
#include "share_data.h"
#include "user_sync_task.h"
#include "ipcu.h"
#include "fj_filesystem.h"
#include "fj_memory.h"
#include "user_streaming.h"
#include "user_record_task.h"
#include "sdram_map_movie_record.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define ROUNDUP_8(val)	(((UINT32)(val) +7) & 0xFFFFFFF8)

#define FLG_OS_IF_RTOS_CMD_ACK	(0x00000001)
#define FLG_OS_IF_RTOS_CMD_DBG	(0x00000002)
#define FLG_OS_IF_WAITPTN		(FLG_OS_IF_RTOS_CMD_ACK | FLG_OS_IF_RTOS_CMD_DBG)

#define D_AUDIO_STREAM_REC_AAC_ES_OUT_DATA_OFFSET	(8)		// output data adress offset
//#define VIDEO_DEBUG_FILE_SAVE
//#define AUDIO_DEBUG_FILE_SAVE

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define IS_FLGPTN_EQUAL(flgptn, FLG)	((((flgptn) & (FLG)) == (FLG)) ? TRUE : FALSE)

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
typedef enum {
	E_OS_IF_RTOS_CMD_ACK			= 0,
	E_OS_IF_LINUX_CMD_REQ			= 1,
	
	E_OS_IF_RTOS_EVENT_NOTIFY		= 4,
	E_OS_IF_LINUX_INFO_NOTIFY		= 5,

	E_OS_IF_RTOS_STREAM_SEND		= 6,
	E_OS_IF_LINUX_STREAM_RELEASE	= 7,
	
	E_OS_IF_RTOS_STREAM_REL			= 6,
	E_OS_IF_LINUX_STREAM_SEND		= 7,

	E_OS_IF_MAX						= 15, // IPCU Unit0 has 8 mailbox, IPCU Unit1 has 8 mailbox
} E_OS_IF;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	ULONG	magic;
	ULONG	fmt;
	ULONG	cmd;
	ULONG	sub;
	ULONG	prm1;
	ULONG	prm2;
	ULONG	prm3;
	ULONG	prm4;
	ULONG	dec_pos;
	int	exp_time;
} T_OS_IF_CMD;

typedef struct {
	ULONG	magic;
	ULONG	fmt;
	ULONG	cmd;
	ULONG	sub;
	ULONG	prm1;
	ULONG	prm2;
	ULONG	prm3;
	ULONG	prm4;
	ULONG	area;
} T_OS_IF_REL;

typedef enum {
	E_STREAM_TYPE_H264 = 0,
	E_STREAM_TYPE_H265,
	E_STREAM_TYPE_JPEG,
	E_STREAM_TYPE_YUV,
	E_STREAM_TYPE_AAC,
	E_STREAM_TYPE_PCM,
} E_STREAM_TYPE;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static UCHAR g_mbox[E_OS_IF_MAX];
static T_OS_IF_CMD g_cmd_ack;

static UINT32 g_audio_idx[2][SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM];
static UINT32 g_audio_idx_cnt[2] = {0, 0};
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void linux_cmd_req_from_IPCU_Task( UCHAR mailbox_id, UINT32 data_addr, UINT32 data_size, UINT32 continue_flag, UINT32 total_size );
static void linux_stream_release_from_IPCU_Task( UCHAR mailbox_id, UINT32 data_addr, UINT32 data_size, UINT32 continue_flag, UINT32 total_size );
#if 0
static ULONG audio_stream_rec_set_es_stream_size( ULONG addr, ULONG size);
#endif

static void linux_stream_send( UCHAR mailbox_id, UINT32 data_addr, UINT32 data_size, UINT32 continue_flag, UINT32 total_size );
/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

#if defined(VIDEO_DEBUG_FILE_SAVE) || defined(AUDIO_DEBUG_FILE_SAVE)

#define D_USER_MBX_MAX	(1024)

typedef struct {
	E_STREAM_TYPE	type;
	ULLONG			frame_no;
	BOOL			is_video;
	ULONG			addr;
	ULONG			size;
} T_USER_MBX;

typedef struct {
	UINT32			head;
	UINT32			num;
	T_USER_MBX		data[D_USER_MBX_MAX];
} T_USER_MBX_DTQ;

static volatile T_USER_MBX_DTQ	g_user_mbx_empty_dtq __attribute__((section(".LOCK_SECTION"), aligned(64)));

static INT32 enqueue( T_USER_MBX* data )
{
	if( g_user_mbx_empty_dtq.num < D_USER_MBX_MAX ){
		g_user_mbx_empty_dtq.data[(g_user_mbx_empty_dtq.head+g_user_mbx_empty_dtq.num)%D_USER_MBX_MAX] = *data;
		g_user_mbx_empty_dtq.num++;
		return D_DDIM_USER_E_OK;
	}
	else{
		return D_DDIM_USER_E_SYS; // full
	}
}

static INT32 dequeue( T_USER_MBX* data )
{
	if( g_user_mbx_empty_dtq.num > 0 ){
		*data = g_user_mbx_empty_dtq.data[g_user_mbx_empty_dtq.head];
		g_user_mbx_empty_dtq.head = (g_user_mbx_empty_dtq.head+1)%D_USER_MBX_MAX;
		g_user_mbx_empty_dtq.num--;
		return D_DDIM_USER_E_OK;
	}
	else{
		return D_DDIM_USER_E_QOVR; // empty
	}
}

static void call_os_if_task( E_STREAM_TYPE type, ULLONG frame_no, ULONG addr, ULONG size, BOOL is_video )
{
	T_USER_MBX	mbx;
	
	mbx.type = type;
	mbx.frame_no = frame_no;
	mbx.addr = addr;
	mbx.size = size;
	mbx.is_video = is_video;
	if( enqueue( &mbx ) == D_DDIM_USER_E_OK ){
		OS_User_Set_Flg( FID_OS_IF, FLG_OS_IF_RTOS_CMD_DBG );
	}else{
		UPRINTF_ERR(1);
		printf( "!!!! enqueue error !!!!\n" );
	}
}
#endif

VOID OS_IF_Task( INT32 stacd )
{
	OS_USER_FLGPTN	flgptn = 0;
	
	while(1)
	{
		// wait event
		OS_User_Twai_Flg( FID_OS_IF, FLG_OS_IF_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( FID_OS_IF, ~FLG_OS_IF_WAITPTN );
		
		// Sensor Update
		if( IS_FLGPTN_EQUAL( flgptn, FLG_OS_IF_RTOS_CMD_ACK ) ){
			MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_CMD_REL, FJ_MBALOG_FUNC_ID_LINUX_CMD, FJ_MBALOG_TYPE_POINT, "" ));
			FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_CMD_ACK], (UINT32)&g_cmd_ack, sizeof(T_OS_IF_CMD), 1);
#if 0
			UPRINTF( "RTOS to Linux\n" );
			UPRINTF( "  magic : 0x%08x\n", (INT32)g_cmd_ack.magic);
			UPRINTF( "  fmt   : 0x%08x\n", (INT32)g_cmd_ack.fmt);
			UPRINTF( "  cmd   : 0x%08x\n", (INT32)g_cmd_ack.cmd);
			UPRINTF( "  sub   : 0x%08x\n", (INT32)g_cmd_ack.sub);
			UPRINTF( "  prm1  : 0x%08x\n", (INT32)g_cmd_ack.prm1);
			UPRINTF( "  prm2  : 0x%08x\n", (INT32)g_cmd_ack.prm2);
			UPRINTF( "  prm3  : 0x%08x\n", (INT32)g_cmd_ack.prm3);
			UPRINTF( "  prm4  : 0x%08x\n", (INT32)g_cmd_ack.prm4);
#endif
		}
#if defined(VIDEO_DEBUG_FILE_SAVE) || defined(AUDIO_DEBUG_FILE_SAVE)
		if( IS_FLGPTN_EQUAL( flgptn, FLG_OS_IF_RTOS_CMD_DBG ) )
		{
			static ULLONG	check_frame_no = 0;
			static BOOL		execute_flg = FALSE;
			FJ_ERR_CODE		err;
			CHAR			dname[16] = "I:\\stream";
			CHAR			fname[32];
			ULONG			fid;
			ULONG			ret_size;
			UINT32			attr;
			T_USER_MBX		mbx;
			
			while( dequeue( &mbx ) == D_DDIM_USER_E_OK ){
				
				// video
				if( mbx.is_video ){
					// init at 1st frame
					if( mbx.type == E_STREAM_TYPE_H264 && mbx.frame_no == 0){
						execute_flg = FALSE;
						check_frame_no = mbx.frame_no;
						err = FJ_Fs_GetAttribute( dname, &attr );
						if ( err != FJ_ERR_OK ) {
							FJ_Fs_Mkdir( dname );
						}
					}

					if( mbx.type == E_STREAM_TYPE_H265 && mbx.frame_no == 1){
						execute_flg = FALSE;
						check_frame_no = mbx.frame_no;
						err = FJ_Fs_GetAttribute( dname, &attr );
						if ( err != FJ_ERR_OK ) {
							FJ_Fs_Mkdir( dname );
						}
					}
					// execute?
					if( execute_flg ){
						break;
					}
					// reliability ok?
					if( check_frame_no != mbx.frame_no ){
						UPRINTF_ERR(1);
						printf( "    mbx.frame_no = %d, check_frame_no = %d\n", (INT32)mbx.frame_no, (INT32)check_frame_no );
					
						printf( "    This Debug is no longer executed, since reliability is lost\n" );
						execute_flg = TRUE;
						break;
					}
					// file save
					if (mbx.type == E_STREAM_TYPE_H265) {
						sprintf( fname, "%s\\H265%04d", dname, (INT32)mbx.frame_no - 1);
					} else {
						sprintf( fname, "%s\\H264%04d", dname, (INT32)mbx.frame_no );
					}
					fid = FJ_Fs_Open ( fname, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
					FJ_Fs_Write( fid, (UINT32*)mbx.addr, mbx.size, &ret_size );
					FJ_Fs_Close( fid );
					// guess next frame
					check_frame_no = mbx.frame_no + 1;
				}
				// audio
				else{
					sprintf( fname, "I:\\test.aac" );
					fid = FJ_Fs_Open ( fname, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_APPEND) );
					FJ_Fs_Write( fid, (UINT32*)mbx.addr, mbx.size, &ret_size );
					FJ_Fs_Close( fid );
				}
			}
		}
#endif
	}
}

void OS_IF_Init(void)
{
	FJ_ERR_CODE ercd = 0;

	// to communicate Linux and CM0
	OS_User_Sta_Tsk( TID_IPCU_TASK0, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK1, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK2, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK3, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK4, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK5, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK6, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK7, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK8, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK9, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK10, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK11, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK12, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK13, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK14, 0 );
	OS_User_Sta_Tsk( TID_IPCU_TASK15, 0 );
	
	OS_User_Clr_Flg( FID_OS_IF, 0 );
	OS_User_Sta_Tsk( TID_OS_IF, 0 );
	
	// IPCU Command Request from Linux
	ercd = FJ_IPCU_Open(E_OS_IF_LINUX_CMD_REQ, &g_mbox[E_OS_IF_LINUX_CMD_REQ]);
	UPRINTF_ERR(ercd);
	ercd = FJ_IPCU_SetReceiverCB(g_mbox[E_OS_IF_LINUX_CMD_REQ], linux_cmd_req_from_IPCU_Task);
	UPRINTF_ERR(ercd);
	// IPCU Command Ack to Linux
	ercd = FJ_IPCU_Open(E_OS_IF_RTOS_CMD_ACK, &g_mbox[E_OS_IF_RTOS_CMD_ACK]);
	UPRINTF_ERR(ercd);

	// IPCU RTOS Event Notify to Linux
	ercd = FJ_IPCU_Open(E_OS_IF_RTOS_EVENT_NOTIFY, &g_mbox[E_OS_IF_RTOS_EVENT_NOTIFY]);
	UPRINTF_ERR(ercd);
	// IPCU Information Notify from Linux
	ercd = FJ_IPCU_Open(E_OS_IF_LINUX_INFO_NOTIFY, &g_mbox[E_OS_IF_LINUX_INFO_NOTIFY]);
	UPRINTF_ERR(ercd);
	ercd = FJ_IPCU_SetReceiverCB(g_mbox[E_OS_IF_LINUX_INFO_NOTIFY], NULL);
	UPRINTF_ERR(ercd);

	// IPCU Stream Send to Linux
	ercd = FJ_IPCU_Open(E_OS_IF_RTOS_STREAM_SEND, &g_mbox[E_OS_IF_RTOS_STREAM_SEND]);
	UPRINTF_ERR(ercd);
	// IPCU Stream Release from Linux
	ercd = FJ_IPCU_Open(E_OS_IF_LINUX_STREAM_RELEASE, &g_mbox[E_OS_IF_LINUX_STREAM_RELEASE]);
	UPRINTF_ERR(ercd);
	ercd = FJ_IPCU_SetReceiverCB(g_mbox[E_OS_IF_LINUX_STREAM_RELEASE], linux_stream_release_from_IPCU_Task);
	UPRINTF_ERR(ercd);

	// for Linux to communicate CM0
	BF_IPCU_Set_Sync_CM0();
}

void OS_IF_Rtos_Event_Notify(const ULONG cmd_set, const ULONG cmd, const ULONG param[4])
{
	T_CPUCMD_NOTIFY_EVENT notify;

	notify.t_head.magic_code = D_CPU_IF_MCODE_EVENT_NOTIFY;
	notify.t_head.format_version = D_CPU_IF_COM_FORMAT_VERSION;
	notify.t_head.cmd_set = cmd_set;
	notify.t_head.cmd = cmd;

	notify.notify1 = param[0];
	notify.notify2 = param[1];
	notify.notify3 = param[2];
	notify.notify4 = param[3];

	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_NOTIFY, FJ_MBALOG_FUNC_ID_LINUX_EVENT_NOTIFY, FJ_MBALOG_TYPE_POINT, ""));
	FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_EVENT_NOTIFY], (UINT32)&notify, sizeof(T_CPUCMD_NOTIFY_EVENT), 0);
}
void OS_IF_Rtos_Linux_CMD( char * str, int size, UCHAR sync)
{
	T_CPUCMD_NOTIFY_EVENT notify;
	UPRINTF( "OS_IF_Rtos_Linux_CMD:[%s], size:%d\n", str, size);

	notify.t_head.magic_code = D_CPU_IF_MCODE_EVENT_NOTIFY;
	notify.t_head.format_version = D_CPU_IF_COM_FORMAT_VERSION;
	notify.t_head.cmd_set = E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_ISSUE_CMD;
	notify.t_head.cmd = E_CPU_IF_COMMAND_NOTIFY_EVENT_LINUX_CMD;
	memcpy(notify.evt, str, size);
	FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_EVENT_NOTIFY], (UINT32)&notify, sizeof(T_CPUCMD_NOTIFY_EVENT), sync);

	if(sync == 0){
		OS_User_Clr_Flg(FID_LINUX_CMD, 0x0 );
	}
}

void OS_IF_Rtos_MJPEG_Stream_Send( FW_USHORT width, FW_USHORT lines, const ULLONG frame_no )
{
	T_SHARE_FRAME_TIMESTAMP stamp;
	T_CPUCMD_STREAM stream_if_info;
	ULONG addr, size;
	FJ_ERR_CODE ercd = 0;
	//static UINT32 eof = 0, sof=0;
	
	User_Utility_JPEG_Get_Result( &addr, &size );

	Get_Frame_Share_FrameStamp( &stamp, frame_no);

	stream_if_info.jstream.t_head.magic_code		= 0x9999EEEE;
	stream_if_info.jstream.t_head.format_version	= D_CPU_IF_COM_FORMAT_VERSION;
	stream_if_info.jstream.t_head.cmd_set			= E_CPU_IF_COMMAND_SET_STREAM_STR;
	stream_if_info.jstream.t_head.cmd				= E_CPU_IF_COMMAND_STREAM_JPG;
	stream_if_info.jstream.stype					= 0;
	stream_if_info.jstream.sformat 				= 0;
	stream_if_info.jstream.stream_id				= 16; //video_enc_id;
	stream_if_info.jstream.stream_end_flg			= 0;//stream_info->stream_end_flg;
	stream_if_info.jstream.area_index				= 1;//stream_info->vstream_index;
	stream_if_info.jstream.sample_size 			= size; //stream_info->sample_size;
	stream_if_info.jstream.sample_address			= addr;//(ULONG)stream_info->sample_address;
	stream_if_info.jstream.sample_pts				= 1;//stream_info->sample_pts;
	stream_if_info.jstream.width					= width;//stream_info->width;
	stream_if_info.jstream.lines					= lines;//stream_info->lines;

	stream_if_info.jstream.y_offset				= 0;
	stream_if_info.jstream.c_offset				= 70;
	stream_if_info.jstream.y_size					= 64;
	stream_if_info.jstream.c_size					= 64;

	stream_if_info.jstream.frame_no				= frame_no;

	ercd = OS_User_Twai_Sem(SID_IPCU6A, 500);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(( "--- IPCU6A: Twai semaphore NG\n" ));
	}

	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REQ, FJ_MBALOG_FUNC_ID_LINUX_STR_MJPG, FJ_MBALOG_TYPE_POINT, ""));
	ercd = FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_STREAM_SEND], (UINT32)&stream_if_info, sizeof(T_CPUCMD_STREAM), 1);
	OS_User_Sig_Sem(SID_IPCU6A);
}

void OS_IF_Rtos_MJPEG_Stream_Send2( ULONG addr, ULONG size, FW_USHORT width, FW_USHORT lines, const ULLONG frame_no )
{
	T_SHARE_FRAME_TIMESTAMP stamp;
	T_CPUCMD_STREAM stream_if_info;
	FJ_ERR_CODE ercd = 0;
	
	Get_Frame_Share_FrameStamp( &stamp, frame_no);

	stream_if_info.jstream.t_head.magic_code		= 0x9999EEEE;
	stream_if_info.jstream.t_head.format_version	= D_CPU_IF_COM_FORMAT_VERSION;
	stream_if_info.jstream.t_head.cmd_set			= E_CPU_IF_COMMAND_SET_STREAM_STR;
	stream_if_info.jstream.t_head.cmd				= E_CPU_IF_COMMAND_STREAM_JPG;
	stream_if_info.jstream.stype					= 0;
	stream_if_info.jstream.sformat 				= 0;
	stream_if_info.jstream.stream_id				= 16; //video_enc_id;
	stream_if_info.jstream.stream_end_flg			= 0;//stream_info->stream_end_flg;
	stream_if_info.jstream.area_index				= 1;//stream_info->vstream_index;
	stream_if_info.jstream.sample_size 			= size; //stream_info->sample_size;
	stream_if_info.jstream.sample_address			= addr;//(ULONG)stream_info->sample_address;
	stream_if_info.jstream.sample_pts				= 1;//stream_info->sample_pts;
	stream_if_info.jstream.width					= width;//stream_info->width;
	stream_if_info.jstream.lines					= lines;//stream_info->lines;

	stream_if_info.jstream.y_offset				= 0;
	stream_if_info.jstream.c_offset				= 70;
	stream_if_info.jstream.y_size					= 64;
	stream_if_info.jstream.c_size					= 64;


	stream_if_info.jstream.frame_no				= frame_no;

	ercd = OS_User_Twai_Sem(SID_IPCU6A, 500);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(( "--- IPCU6A: Twai semaphore NG\n" ));
	}

	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REQ, FJ_MBALOG_FUNC_ID_LINUX_STR_MJPG, FJ_MBALOG_TYPE_POINT, ""));
	ercd = FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_STREAM_SEND], (UINT32)&stream_if_info, sizeof(T_CPUCMD_STREAM), 1);
	OS_User_Sig_Sem(SID_IPCU6A);
}

void OS_IF_Rtos_MJPEG_Stream_Send3( ULONG addr, ULONG size, T_BF_DCF_IF_EXIF_INFO *exif, FW_USHORT width, FW_USHORT lines, const ULLONG frame_no )
{
	T_SHARE_FRAME_TIMESTAMP stamp;
	T_CPUCMD_STREAM stream_if_info;
	FJ_ERR_CODE ercd = 0;
	
	Get_Frame_Share_FrameStamp( &stamp, frame_no);

	stream_if_info.jstream.t_head.magic_code		= 0x9999EEEE;
	stream_if_info.jstream.t_head.format_version	= D_CPU_IF_COM_FORMAT_VERSION;
	stream_if_info.jstream.t_head.cmd_set			= E_CPU_IF_COMMAND_SET_STREAM_STR;
	stream_if_info.jstream.t_head.cmd				= E_CPU_IF_COMMAND_STREAM_JPG;
	stream_if_info.jstream.stype					= 0;
	stream_if_info.jstream.sformat 				= 0;
	stream_if_info.jstream.stream_id				= 16; //video_enc_id;
	stream_if_info.jstream.stream_end_flg			= 0;//stream_info->stream_end_flg;
	stream_if_info.jstream.area_index				= 1;//stream_info->vstream_index;
	stream_if_info.jstream.sample_size 			= size; //stream_info->sample_size;
	stream_if_info.jstream.sample_address			= addr;//(ULONG)stream_info->sample_address;
	stream_if_info.jstream.sample_pts				= 1;//stream_info->sample_pts;
	stream_if_info.jstream.width					= width;//stream_info->width;
	stream_if_info.jstream.lines					= lines;//stream_info->lines;

	stream_if_info.jstream.y_offset				= 0;
	stream_if_info.jstream.c_offset				= 70;
	stream_if_info.jstream.y_size					= 64;
	stream_if_info.jstream.c_size					= 64;

	stream_if_info.jstream.frame_no				= frame_no;
	stream_if_info.jstream.exif 				= *exif;

	UPRINTF("iso = %d, exp_time = %d/%d\n", stream_if_info.jstream.exif.iso_value, stream_if_info.jstream.exif.exposure_time.nume, 
										stream_if_info.jstream.exif.exposure_time.denomi);

	UPRINTF("%d/%d/%d, %d:%d:%d\n", 
						stream_if_info.jstream.exif.date.ad_year, 
						stream_if_info.jstream.exif.date.month, 
						stream_if_info.jstream.exif.date.day, 
						stream_if_info.jstream.exif.date.hour, 
						stream_if_info.jstream.exif.date.min, 
						stream_if_info.jstream.exif.date.sec);

	ercd = OS_User_Twai_Sem(SID_IPCU6A, 500);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(( "--- IPCU6A: Twai semaphore NG\n" ));
	}

	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REQ, FJ_MBALOG_FUNC_ID_LINUX_STR_MJPG, FJ_MBALOG_TYPE_POINT, ""));
	ercd = FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_STREAM_SEND], (UINT32)&stream_if_info, sizeof(T_CPUCMD_STREAM), 1);
	OS_User_Sig_Sem(SID_IPCU6A);
}


void OS_IF_Rtos_H264_Stream_Send( T_FJ_MOVIE_H264_STREAM_PARAM* stream_info, UCHAR video_enc_id, const ULLONG frame_no )
{
	T_SHARE_FRAME_TIMESTAMP stamp;
	T_CPUCMD_STREAM stream_if_info;
	FJ_ERR_CODE ercd = 0;
	//static UINT32 eof = 0, sof=0;

	Get_Frame_Share_FrameStamp( &stamp, frame_no);

	stream_if_info.vstream.t_head.magic_code		= D_CPU_IF_MCODE_STREAM_SEND;
	stream_if_info.vstream.t_head.format_version	= D_CPU_IF_COM_FORMAT_VERSION;
	stream_if_info.vstream.t_head.cmd_set			= E_CPU_IF_COMMAND_SET_STREAM_STR;
	stream_if_info.vstream.t_head.cmd				= E_CPU_IF_COMMAND_STREAM_VIDEO; // for video stream 
		
	stream_if_info.vstream.stype					= E_CPU_IF_VSTRM_TYPE_H264; // for h264
	stream_if_info.vstream.sformat 					= E_CPU_IF_VSTRM_FORMAT_ES; // for es
	stream_if_info.vstream.stream_id				= video_enc_id;
	stream_if_info.vstream.stream_end_flg			= stream_info->stream_end_flg;
	stream_if_info.vstream.area_index				= stream_info->vstream_index;
	stream_if_info.vstream.sample_size 				= stream_info->stream_size;
	stream_if_info.vstream.sample_address			= stream_info->stream_address;
	stream_if_info.vstream.sample_pts				= stream_info->stream_pts;
	stream_if_info.vstream.sample_type 				= stream_info->frame_type;
	stream_if_info.vstream.frame_of_GOP				= stream_info->counter_in_gop;
	stream_if_info.vstream.nal_unit_num				= stream_info->nal_unit_num;
	memcpy( stream_if_info.vstream.nal_unit_size, stream_info->nal_unit_size, sizeof( stream_info->nal_unit_size ) );
	stream_if_info.vstream.bitrate 					= stream_info->bitrate;
	stream_if_info.vstream.ave_bitrate 				= stream_info->ave_bitrate;
	stream_if_info.vstream.width					= stream_info->width;
	stream_if_info.vstream.lines					= stream_info->lines;
	stream_if_info.vstream.frame_no					= stream_info->frame_no;

	ercd = OS_User_Twai_Sem(SID_IPCU6A, 500);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(( "--- IPCU6A: Twai semaphore NG\n" ));
	}
	if( video_enc_id == 0 ){
		MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REQ, FJ_MBALOG_FUNC_ID_LINUX_STR_264_0, FJ_MBALOG_TYPE_POINT, ""));
	}else if( video_enc_id == 1 ){
		MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REQ, FJ_MBALOG_FUNC_ID_LINUX_STR_264_1, FJ_MBALOG_TYPE_POINT, ""));
	}
#if defined(VIDEO_DEBUG_FILE_SAVE)
	call_os_if_task( E_STREAM_TYPE_H264, stream_info->stream_serial_num, stream_if_info.vstream.sample_address, stream_if_info.vstream.sample_size, TRUE  );
#else
	ercd = FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_STREAM_SEND], (UINT32)&stream_if_info, sizeof(T_CPUCMD_STREAM), 1);
	UPRINTF_ERR(ercd);
#endif
	OS_User_Sig_Sem(SID_IPCU6A);
}

void OS_IF_Rtos_H265_Stream_Send( T_FJ_MOVIE_H265_STREAM_PARAM* stream_info, UCHAR video_enc_id, const ULLONG frame_no )
{
	//FJ_ERR_CODE err;
	T_SHARE_FRAME_TIMESTAMP stamp;
	T_CPUCMD_STREAM stream_if_info;
	OS_USER_ER ercd = 0;
	static BOOL begin = TRUE;

	//static UINT32 eof = 0, sof=0;
	if (begin) {
		UPRINTF( "uvc265, id:%d addr:0x%08x, size:0x%08x\n", video_enc_id, (UINT32)stream_info->stream_address, (UINT32)stream_info->stream_size);
		begin = FALSE;
	}

	if (stream_info->stream_end_flg) {
		begin = TRUE;
	}

	Get_Frame_Share_FrameStamp( &stamp, frame_no);

	stream_if_info.vstream.t_head.magic_code		= D_CPU_IF_MCODE_STREAM_SEND;
	stream_if_info.vstream.t_head.format_version	= D_CPU_IF_COM_FORMAT_VERSION;
	stream_if_info.vstream.t_head.cmd_set			= E_CPU_IF_COMMAND_SET_STREAM_STR;
	stream_if_info.vstream.t_head.cmd				= E_CPU_IF_COMMAND_STREAM_VIDEO; // for video stream
		
	stream_if_info.vstream.stype					= E_CPU_IF_VSTRM_TYPE_H265; // for h265
	stream_if_info.vstream.sformat 					= E_CPU_IF_VSTRM_FORMAT_ES; // for es
	stream_if_info.vstream.stream_id				= video_enc_id + 5;
	stream_if_info.vstream.stream_end_flg			= stream_info->stream_end_flg;
	stream_if_info.vstream.area_index				= stream_info->vstream_index;
	stream_if_info.vstream.sample_size 				= stream_info->stream_size;
	stream_if_info.vstream.sample_address			= stream_info->stream_address;
	stream_if_info.vstream.sample_pts				= stream_info->stream_pts;
	stream_if_info.vstream.sample_type 				= stream_info->frame_type;
	stream_if_info.vstream.frame_of_GOP				= stream_info->counter_in_gop;
	stream_if_info.vstream.nal_unit_num				= stream_info->nal_unit_num;
	memcpy( stream_if_info.vstream.nal_unit_size, stream_info->nal_unit_size, sizeof( stream_info->nal_unit_size ) );
	stream_if_info.vstream.bitrate 					= stream_info->bitrate;
	stream_if_info.vstream.ave_bitrate 				= stream_info->ave_bitrate;
	stream_if_info.vstream.width					= stream_info->width;
	stream_if_info.vstream.lines					= stream_info->lines;
	stream_if_info.vstream.frame_no					= stream_info->frame_no;

	ercd = OS_User_Twai_Sem(SID_IPCU6A, 500);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(( "--- IPCU6A: Twai semaphore NG\n" ));
	}

	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REQ, FJ_MBALOG_FUNC_ID_LINUX_STR_265, FJ_MBALOG_TYPE_POINT, "IPCU Send"));

#if defined(VIDEO_DEBUG_FILE_SAVE)
	call_os_if_task( E_STREAM_TYPE_H265, stream_info->stream_serial_num, stream_if_info.vstream.sample_address, stream_if_info.vstream.sample_size, TRUE  );
#else
	ercd = FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_STREAM_SEND], (UINT32)&stream_if_info, sizeof(T_CPUCMD_STREAM), 1);
#endif
	OS_User_Sig_Sem(SID_IPCU6A);
}

void OS_IF_Rtos_YUV_Stream_Send( ULONG addr, ULONG size, FW_USHORT width, FW_USHORT lines, const ULLONG frame_no )
{
	T_SHARE_FRAME_TIMESTAMP stamp;
	T_CPUCMD_STREAM stream_if_info;
	FJ_ERR_CODE ercd = 0;

	Get_Frame_Share_FrameStamp( &stamp, frame_no);

	stream_if_info.ystream.t_head.magic_code		= D_CPU_IF_MCODE_STREAM_SEND;
	stream_if_info.ystream.t_head.format_version	= D_CPU_IF_COM_FORMAT_VERSION;
	stream_if_info.ystream.t_head.cmd_set			= E_CPU_IF_COMMAND_SET_STREAM_STR;
	stream_if_info.ystream.t_head.cmd				= E_CPU_IF_COMMAND_STREAM_YUV;
	stream_if_info.ystream.stype					= E_CPU_IF_YUV_TYPE_422; // 1:YUV420, 2:YUV422
	stream_if_info.ystream.sformat 					= E_CPU_IF_YSTRM_FORMAT_YUYV; // 5:YUYV
	stream_if_info.ystream.stream_id				= 1; //video_enc_id;
	stream_if_info.ystream.stream_end_flg			= 0;//stream_info->stream_end_flg;
	stream_if_info.ystream.area_index				= 1;//stream_info->vstream_index;
	stream_if_info.ystream.sample_size 				= size; //stream_info->sample_size;
	stream_if_info.ystream.sample_address			= addr;//(ULONG)stream_info->sample_address;
	stream_if_info.ystream.time_stamp				= 0;
	stream_if_info.ystream.width					= width;//stream_info->width;
	stream_if_info.ystream.lines					= lines;//stream_info->lines;
	stream_if_info.ystream.frame_no					= frame_no;

	ercd = OS_User_Twai_Sem(SID_IPCU6A, 500);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(( "--- IPCU6A: Twai semaphore NG\n" ));
	}
	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REQ, FJ_MBALOG_FUNC_ID_LINUX_STR_YUV, FJ_MBALOG_TYPE_POINT, ""));
	ercd = FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_STREAM_SEND], (UINT32)&stream_if_info, sizeof(T_CPUCMD_STREAM), 1);
	OS_User_Sig_Sem(SID_IPCU6A);
}

void OS_IF_Rtos_RAW_Stream_Send( ULONG addr, ULONG size, USHORT width, USHORT lines, const ULLONG frame_no )
{
	T_SHARE_FRAME_TIMESTAMP stamp;
	T_CPUCMD_STREAM stream_if_info;
	FJ_ERR_CODE ercd = 0;

	Get_Frame_Share_FrameStamp( &stamp, frame_no);

	stream_if_info.rawstream.t_head.magic_code		= D_CPU_IF_MCODE_STREAM_SEND;
	stream_if_info.rawstream.t_head.format_version	= D_CPU_IF_COM_FORMAT_VERSION;
	stream_if_info.rawstream.t_head.cmd_set			= E_CPU_IF_COMMAND_SET_STREAM_STR;
	stream_if_info.rawstream.t_head.cmd				= E_CPU_IF_COMMAND_STREAM_RAW;
	stream_if_info.rawstream.stype					= 0; // 0: 14 bits unpack
	stream_if_info.rawstream.sformat 				= 0; // first pixel 0:R 1:B 2:Gr 3:Gb
	stream_if_info.rawstream.stream_id				= 1;
	stream_if_info.rawstream.stream_end_flg			= 0;
	stream_if_info.rawstream.area_index				= 1;
	stream_if_info.rawstream.sample_size 			= size;
	stream_if_info.rawstream.sample_address			= addr;
	stream_if_info.rawstream.time_stamp				= 0;

	stream_if_info.rawstream.global_width			= 0;
	stream_if_info.rawstream.width					= width;
	stream_if_info.rawstream.lines					= lines;
	stream_if_info.rawstream.frame_no				= frame_no;

	ercd = OS_User_Twai_Sem(SID_IPCU6A, 500);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(( "--- IPCU6A: Twai semaphore NG\n" ));
	}
	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REQ, FJ_MBALOG_FUNC_ID_LINUX_STR_RAW, FJ_MBALOG_TYPE_POINT, ""));
	ercd = FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_STREAM_SEND], (UINT32)&stream_if_info, sizeof(T_CPUCMD_STREAM), 1);
	OS_User_Sig_Sem(SID_IPCU6A);
}

void OS_IF_Rtos_Audio_Stream_Send_AAC( UCHAR aid, T_FJ_AUDIO_AAC_STREAM_PARAM* cb_inf, const BOOL uac_en)
{
	T_CPUCMD_STREAM sif_info;
	OS_USER_ER ercd = 0;
	BOOL end_flg = FALSE;

	sif_info.astream.t_head.magic_code		= D_CPU_IF_MCODE_STREAM_SEND;
	sif_info.astream.t_head.format_version	= D_CPU_IF_COM_FORMAT_VERSION;
	sif_info.astream.t_head.cmd_set			= E_CPU_IF_COMMAND_SET_STREAM_STR;
	sif_info.astream.t_head.cmd				= E_CPU_IF_COMMAND_STREAM_AUDIO;
	sif_info.astream.t_head.magic_code		= D_CPU_IF_MCODE_STREAM_SEND;
	sif_info.astream.t_head.format_version	= D_CPU_IF_COM_FORMAT_VERSION;
	sif_info.astream.t_head.cmd_set			= E_CPU_IF_COMMAND_SET_STREAM_STR;
	sif_info.astream.t_head.cmd				= E_CPU_IF_COMMAND_STREAM_AUDIO;

	sif_info.astream.stype				= E_CPU_IF_ASTRM_TYPE_AAC;
	sif_info.astream.sformat			= E_CPU_IF_ASTRM_TYPE_ES;
	sif_info.astream.stream_id			= aid;
	sif_info.astream.stream_end_flg		= cb_inf->stream_end_flag;
	sif_info.astream.area_index			= cb_inf->area_index;
	if( uac_en ) {
		sif_info.astream.sample_size 	= (ULONG)cb_inf->uac_size;
		sif_info.astream.sample_address	= (ULONG)cb_inf->uac_data;
	} else {
		sif_info.astream.sample_address = (ULONG)cb_inf->adts_data;
		sif_info.astream.sample_size 	= (ULONG)cb_inf->adts_size;
	}
	sif_info.astream.time_stamp			= cb_inf->time_stamp;
	sif_info.astream.channel			= cb_inf->ch_type;
	sif_info.astream.sampling_freq		= cb_inf->sampling_freq;
	sif_info.astream.bit_rate			= cb_inf->bit_rate;
	sif_info.astream.frame_no			= cb_inf->frame_no;

	//	sif_info.astream.num_remain_area		= stream_info->num_remain_area;
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, sif_info.astream.sample_address, sif_info.astream.sample_size);

	ercd = OS_User_Twai_Sem(SID_IPCU6A, 500);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(( "--- IPCU6A: Twai semaphore NG\n" ));
	}

	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REQ, FJ_MBALOG_FUNC_ID_LINUX_STR_AUDIO, FJ_MBALOG_TYPE_POINT, ""));
#if defined(AUDIO_DEBUG_FILE_SAVE)
	call_os_if_task( E_STREAM_TYPE_AAC, sif_info.astream.frame_no, sif_info.astream.sample_address, sif_info.astream.sample_size, FALSE  );
#else
	ercd = FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_STREAM_SEND], (UINT32)&sif_info, sizeof(T_CPUCMD_STREAM), 1);
#endif
	OS_User_Sig_Sem(SID_IPCU6A);

	if (cb_inf->frame_no == 0) {
		g_audio_idx_cnt[aid-1] = 0;
	}

	g_audio_idx[aid-1][g_audio_idx_cnt[aid-1]] = cb_inf->area_index;
	if (g_audio_idx_cnt[aid-1] == (cb_inf->sampling_freq / 1024) || cb_inf->stream_end_flag) {
		for (int i = 0; i <= g_audio_idx_cnt[aid-1]; i++) {
			if (cb_inf->stream_end_flag && i == g_audio_idx_cnt[aid-1]) {
				end_flg = TRUE;
			} else {
				end_flg = FALSE;
			}
			User_Utility_Record_Audio_Free_Index(aid - 1, g_audio_idx[aid-1][i], end_flg);
		}
		g_audio_idx_cnt[aid-1] = 0;
	} else {
		g_audio_idx_cnt[aid-1] = ((g_audio_idx_cnt[aid-1] + 1) == SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM) ? 0 : g_audio_idx_cnt[aid-1] + 1;
	}
}

void OS_IF_Rtos_Audio_Stream_Send_PCM( UCHAR aid, T_FJ_AUDIO_PCM_STREAM_PARAM* cb_inf, const BOOL uac_en)
{
	T_CPUCMD_STREAM sif_info;
	FJ_ERR_CODE ercd = 0;
	BOOL end_flg = FALSE;

	sif_info.astream.t_head.magic_code		= D_CPU_IF_MCODE_STREAM_SEND;
	sif_info.astream.t_head.format_version	= D_CPU_IF_COM_FORMAT_VERSION;
	sif_info.astream.t_head.cmd_set			= E_CPU_IF_COMMAND_SET_STREAM_STR;
	sif_info.astream.t_head.cmd				= E_CPU_IF_COMMAND_STREAM_AUDIO;

	sif_info.astream.stype				= E_CPU_IF_ASTRM_TYPE_PCM;
	sif_info.astream.sformat			= E_CPU_IF_ASTRM_TYPE_ES;
	sif_info.astream.stream_id			= aid;
	sif_info.astream.stream_end_flg		= cb_inf->stream_end_flag;
	sif_info.astream.area_index			= cb_inf->area_index;
	if( uac_en ) {
		sif_info.astream.sample_size 	= (ULONG)cb_inf->uac_size;
		sif_info.astream.sample_address	= (ULONG)cb_inf->uac_data;
	} else {
		sif_info.astream.sample_size 	= (ULONG)cb_inf->pcm_size;
		sif_info.astream.sample_address	= (ULONG)cb_inf->pcm_data;
	}
	sif_info.astream.time_stamp			= cb_inf->time_stamp;
	sif_info.astream.channel			= cb_inf->ch_type;
	sif_info.astream.sampling_freq		= cb_inf->sampling_freq;
	sif_info.astream.bit_rate			= cb_inf->bit_rate;
	sif_info.astream.frame_no			= cb_inf->frame_no;

//	sif_info.astream.num_remain_area		= stream_info->num_remain_area;						
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, sif_info.astream.sample_address, sif_info.astream.sample_size);

	ercd = OS_User_Twai_Sem(SID_IPCU6A, 500);
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(( "--- IPCU6A: Twai semaphore NG\n" ));
	}
	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REQ, FJ_MBALOG_FUNC_ID_LINUX_STR_AUDIO, FJ_MBALOG_TYPE_POINT, ""));
#if defined(AUDIO_DEBUG_FILE_SAVE)
	call_os_if_task( E_STREAM_TYPE_PCM, sif_info.astream.frame_no, sif_info.astream.sample_address, sif_info.astream.sample_size, FALSE  );
#else
	ercd = FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_STREAM_SEND], (UINT32)&sif_info, sizeof(T_CPUCMD_STREAM), 1);
#endif
	OS_User_Sig_Sem(SID_IPCU6A);

	if (cb_inf->frame_no == 0) {
		g_audio_idx_cnt[aid-1] = 0;
	}

	g_audio_idx[aid-1][g_audio_idx_cnt[aid-1]] = cb_inf->area_index;
	if (g_audio_idx_cnt[aid-1] == (cb_inf->sampling_freq / 1024) || cb_inf->stream_end_flag) {
		for (int i = 0; i <= g_audio_idx_cnt[aid-1]; i++) {
			if (cb_inf->stream_end_flag && i == g_audio_idx_cnt[aid-1]) {
				end_flg = TRUE;
			} else {
				end_flg = FALSE;
			}
			User_Utility_Record_Audio_Free_Index(aid - 1, g_audio_idx[aid-1][i], end_flg);
		}
		g_audio_idx_cnt[aid-1] = 0;
	} else {
		g_audio_idx_cnt[aid-1] = ((g_audio_idx_cnt[aid-1] + 1) == SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM) ? 0 : g_audio_idx_cnt[aid-1] + 1;
	}
}

void OS_IF_Rtos_Stream_Ack(E_CPU_IF_COMMAND_STREAM sub, ULONG stype)
{
	T_OS_IF_REL rel;

	memset(&rel, 0, sizeof(rel));
	rel.magic = D_CPU_IF_MCODE_STREAM_RELESE;
	rel.cmd = E_CPU_IF_COMMAND_SET_STREAM_STR;
	rel.sub = sub;
	rel.prm1 = stype;
	FJ_IPCU_Send(g_mbox[E_OS_IF_RTOS_STREAM_REL], (UINT32)&rel, sizeof(T_OS_IF_REL), 0);

	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REL, FJ_MBALOG_FUNC_ID_LINUX_REL_264_0, FJ_MBALOG_TYPE_POINT, ""));
}

void OS_IF_Rtos_IPCU_CH67_Switch(E_IPCU_CH67_SWITCH sw)
{
	FJ_ERR_CODE err = FJ_ERR_OK;

	if (sw == E_IPCU_CH67_ENCODE) {
		err = FJ_IPCU_Close(E_OS_IF_RTOS_STREAM_REL);
		UPRINTF_ERR(err);
		err = FJ_IPCU_Close(E_OS_IF_LINUX_STREAM_SEND);
		UPRINTF_ERR(err);

		// IPCU Stream Send to Linux
		err = FJ_IPCU_Open(E_OS_IF_RTOS_STREAM_SEND, &g_mbox[E_OS_IF_RTOS_STREAM_SEND]);
		UPRINTF_ERR(err);
		// IPCU Stream Release from Linux
		err = FJ_IPCU_Open(E_OS_IF_LINUX_STREAM_RELEASE, &g_mbox[E_OS_IF_LINUX_STREAM_RELEASE]);
		UPRINTF_ERR(err);
		err = FJ_IPCU_SetReceiverCB(g_mbox[E_OS_IF_LINUX_STREAM_RELEASE], linux_stream_release_from_IPCU_Task);
		UPRINTF_ERR(err);
	} else {
		err = FJ_IPCU_Close(E_OS_IF_RTOS_STREAM_SEND);
		UPRINTF_ERR(err);
		err = FJ_IPCU_Close(E_OS_IF_LINUX_STREAM_RELEASE);
		UPRINTF_ERR(err);

		// IPCU Stream Send from Linux to RTOS
		err = FJ_IPCU_Open(E_OS_IF_LINUX_STREAM_SEND, &g_mbox[E_OS_IF_LINUX_STREAM_SEND]);
		UPRINTF_ERR(err);
		// IPCU Stream Release from RTOS to Linux
		err = FJ_IPCU_Open(E_OS_IF_RTOS_STREAM_REL, &g_mbox[E_OS_IF_RTOS_STREAM_REL]);
		UPRINTF_ERR(err);
		err = FJ_IPCU_SetReceiverCB(g_mbox[E_OS_IF_LINUX_STREAM_SEND], linux_stream_send);
		UPRINTF_ERR(err);
	}
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void linux_cmd_req_from_IPCU_Task( UCHAR mailbox_id, UINT32 data_addr, UINT32 data_size, UINT32 continue_flag, UINT32 total_size )
{
	T_OS_IF_CMD		ack;
	T_OS_IF_CMD*	req;
	UCHAR			cate;
	UCHAR			index;
	UCHAR			size;
	ULONG			data[4]; // why 4? because T_OS_IF_CMD has only 4
	BOOL			is_read;
	FLOAT 			value;
	INT32			ret = 0;
	// Req from Linux
	req = (T_OS_IF_CMD*)data_addr;
	
	// set fixed
	ack.magic = 0x6666BBBB; // fixed
	ack.fmt   = 0x00000001; // fixed
	ack.cmd   = req->cmd;
	ack.sub   = req->sub;
	
	// camera if -> cate
	if( req->cmd & 0x80000000 ){
		is_read = TRUE;
	}else{
		is_read = FALSE;
	}
	cate = req->cmd & 0xFF;
	index = req->sub & 0xFF;
	//size = (req->cmd >> 24) & 0xFF;

	/** fmt translate here **/
	switch (req->dec_pos) {
		case eIpcuDecimalNag1:
			value = req->prm1 /10.0;
			memcpy(&data[0], &value, sizeof(value));
			UPRINTF( "req->prm1:%lx, value:%.4f, data[0]:%lx\n", req->prm1, value, data[0]);
		break;
		case eIpcuDecimalNag2:
			value = req->prm1 /100.0;
			memcpy(&data[0], &value, sizeof(value));
			UPRINTF( "req->prm1:%lx, value:%.4f, data[0]:%lx\n", req->prm1, value, data[0]);
		break;
		case eIpcuDecimalNag3:
			value = req->prm1 /1000.0;
			memcpy(&data[0], &value, sizeof(value));
			UPRINTF( "req->prm1:%lx, value:%.4f, data[0]:%lx\n", req->prm1, value, data[0]);

		break;
		case eIpcuExpTime:
			value = req->exp_time / 1000.0;
			memcpy(&data[0], &value, sizeof(value));
			UPRINTF( "req->exp_time:%d, value:%.4f, data[0]:%lx\n", req->exp_time, value, data[0]);
			
		break;
		case eIpcuBrightness:
			UPRINTF( "eIpcuBrightness req->prm1 1: %lx\n", req->prm1);
			req->prm1 -= 3;
			memcpy(&data[0], &req->prm1, sizeof(req->prm1));
		break;
		
		default:
			data[0] = req->prm1;
		break;

	}
	data[1] = req->prm2;
	data[2] = req->prm3;
	data[3] = req->prm4;

	// fix size as 1 TODO
	size = 1;

	// set camera
	if( is_read ){
		// Linux want to read
		MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_CMD_REQ, FJ_MBALOG_FUNC_ID_LINUX_CMD, FJ_MBALOG_TYPE_POINT, "R 0x%x/0x%x : 0x%x", cate, index, (UINT32)data[0] ));
		Cate_Get( cate, index, size, data );
		ack.prm1 = data[0];
		ack.prm2 = data[1];
		ack.prm3 = data[2];
		ack.prm4 = data[3];
	}
	else{
		// Linux want to write
		MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_CMD_REQ, FJ_MBALOG_FUNC_ID_LINUX_CMD, FJ_MBALOG_TYPE_POINT, "W 0x%x/0x%x = 0x%x", cate, index, (UINT32)data[0] ));
		ret = Cate_Set( cate, index, size, data );
		// Linux want to know OK(0) or NG(1) or Not Support(2)
		ack.prm1 = ret;
		ack.prm2 = 0xFACEFACE; // ?V??
		ack.prm3 = 0xBEAFBEAF; // ?V??
		ack.prm4 = 0xFACEFACE; // ?V??
	}
	
	// Ack to Linux
	g_cmd_ack = ack;
	OS_User_Set_Flg( FID_OS_IF, FLG_OS_IF_RTOS_CMD_ACK );
	
#if 0 // debug
	UPRINTF( "Linux to RTOS\n" );
	UPRINTF( "  magic : 0x%08x\n", (INT32)req->magic);
	UPRINTF( "  fmt   : 0x%08x\n", (INT32)req->fmt);
	UPRINTF( "  cmd   : 0x%08x\n", (INT32)req->cmd);
	UPRINTF( "  sub   : 0x%08x\n", (INT32)req->sub);
	UPRINTF( "  prm1  : 0x%08x\n", (INT32)req->prm1);
	UPRINTF( "  prm2  : 0x%08x\n", (INT32)req->prm2);
	UPRINTF( "  prm3  : 0x%08x\n", (INT32)req->prm3);
	UPRINTF( "  prm4  : 0x%08x\n", (INT32)req->prm4);
	UPRINTF( "\n" );
#endif	

}

static void linux_stream_release_from_IPCU_Task( UCHAR mailbox_id, UINT32 data_addr, UINT32 data_size, UINT32 continue_flag, UINT32 total_size )
{
	T_OS_IF_REL*	req;
	T_USER_SYNC_MSG user_sync_msg = {0};
	OS_USER_ER os_err = 0;
	//T_USER_RECORD_MSG msg;
	//int ret = 0;
	
	req = (T_OS_IF_REL*)data_addr;
	
	switch( req->sub ){
	case E_CPU_IF_COMMAND_STREAM_VIDEO://video
		if( req->prm1 == E_CPU_IF_VSTRM_TYPE_H265 ){		// 265
			MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REL, FJ_MBALOG_FUNC_ID_LINUX_REL_265, FJ_MBALOG_TYPE_POINT, ""));
		}else if( req->prm1 == E_CPU_IF_VSTRM_TYPE_H264 ){	// 264
			if( req->prm3 == 0 ){		// stream 0
				MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REL, FJ_MBALOG_FUNC_ID_LINUX_REL_264_0, FJ_MBALOG_TYPE_POINT, ""));
				//msg.event		= E_USER_RECORD_EVENT_RECORD_H264_FREE_INDEX_0;
				//msg.frame_no	= g_frame_no[E_VIDEO_ENOCDE_ID_H264_CH0];
				//ret = Snd_Msg_To_User_Record( &msg );
				//UPRINTF_ERR(ret)
			} else if( req->prm3 == 1 ){	// stream 1
				MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REL, FJ_MBALOG_FUNC_ID_LINUX_REL_264_1, FJ_MBALOG_TYPE_POINT, ""));
				//msg.event		= E_USER_RECORD_EVENT_RECORD_H264_FREE_INDEX_1;
				//msg.frame_no	= g_frame_no[E_VIDEO_ENOCDE_ID_H264_CH1];
				//ret = Snd_Msg_To_User_Record( &msg );
			} else if( req->prm3 == 2 ){	// stream 2
				MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REL, FJ_MBALOG_FUNC_ID_LINUX_REL_265, FJ_MBALOG_TYPE_POINT, ""));
				//msg.event		= E_USER_RECORD_EVENT_RECORD_H265_FREE_INDEX_0;
				//msg.frame_no	= g_frame_no[E_VIDEO_ENOCDE_ID_H264_CH2];
				//ret = Snd_Msg_To_User_Record( &msg );
			}
		}
		break;
	case E_CPU_IF_COMMAND_STREAM_RAW://raw
		MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REL, FJ_MBALOG_FUNC_ID_LINUX_REL_RAW, FJ_MBALOG_TYPE_POINT, ""));
		user_sync_msg.event = E_USER_SYNC_EVENT_LINUX_END_RAW;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);
		break;
	case E_CPU_IF_COMMAND_STREAM_YUV://yuv
		MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REL, FJ_MBALOG_FUNC_ID_LINUX_REL_YUV, FJ_MBALOG_TYPE_POINT, ""));
		user_sync_msg.event = E_USER_SYNC_EVENT_LINUX_END_YUV;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);
		break;
	case E_CPU_IF_COMMAND_STREAM_JPG://jpg
		MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REL, FJ_MBALOG_FUNC_ID_LINUX_REL_MJPG, FJ_MBALOG_TYPE_POINT, ""));
		user_sync_msg.event = E_USER_SYNC_EVENT_LINUX_END_JPG;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);
		break;
	case E_CPU_IF_COMMAND_STREAM_AUDIO:// audio
		//FJ_AudioEncoder_FreeIndex(req->prm3, req->area, FALSE);
		MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REL, FJ_MBALOG_FUNC_ID_LINUX_REL_AUDIO, FJ_MBALOG_TYPE_POINT, ""));
		break;
	default:
		break;
	}
	
#if 0 // debug	
	UPRINTF( "Linux to RTOS\n" );
	UPRINTF( "  magic : 0x%08x\n", (INT32)req->magic);
	UPRINTF( "  fmt   : 0x%08x\n", (INT32)req->fmt);
	UPRINTF( "  cmd   : 0x%08x\n", (INT32)req->cmd);
	UPRINTF( "  sub   : 0x%08x\n", (INT32)req->sub);
	UPRINTF( "  prm1   : 0x%08x\n", (INT32)req->prm1);
	UPRINTF( "  prm2   : 0x%08x\n", (INT32)req->prm2);
	UPRINTF( "  prm3   : 0x%08x\n", (INT32)req->prm3);
	UPRINTF( "  prm4   : 0x%08x\n", (INT32)req->prm4);
	UPRINTF( "\n" );
#endif
}
#if 0
static ULONG audio_stream_rec_set_es_stream_size( ULONG addr, ULONG size)
{
	UCHAR * temp_size;
	ULONG start_adress;
	UCHAR size_area;
	UCHAR loop;
	
	if (size >= (D_AUDIO_STREAM_REC_AAC_ES_OUT_DATA_OFFSET * 255)){
		// BF_Debug_Print_Error(("[AUDIO_STREAM_REC]ES AAC size Over. size = %lx\n", size));
		return 0;
	}
	
	temp_size = (UCHAR*)addr;
	
	temp_size[7] = size % 255;
	
	size_area = (size - temp_size[7]) / 255;
	
	for (loop = 0; loop < size_area; loop++){
		temp_size[6 - loop] = 255;
	}
	
	start_adress = addr + (D_AUDIO_STREAM_REC_AAC_ES_OUT_DATA_OFFSET - loop - 1);

//	printf("[ES Stream Size] addr=%lx, size = %lx, start=%lx, start_data1=%lx, start_data2=%lx\n", addr, size, start_adress, *(ULONG*)addr, *(ULONG*)(addr+4));
	
	return start_adress;
}
#endif

static void linux_stream_send( UCHAR mailbox_id, UINT32 data_addr, UINT32 data_size, UINT32 continue_flag, UINT32 total_size )
{
	T_CPUCMD_VIDEO_DEC_STREAM* str_info;

	str_info = (T_CPUCMD_VIDEO_DEC_STREAM*)data_addr;

	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_STR_REQ, FJ_MBALOG_FUNC_ID_LINUX_STR_264_0, FJ_MBALOG_TYPE_POINT, ""));

	User_SendStreamToReceiver(*str_info);
}


/*==============================================================================*/
/* for UVC Copy																	*/
/*==============================================================================*/
#include "im_iip.h"
#include "im_xch.h"
#define FLG_UVC_COPY_TRIG				0x00000001
#define FLG_UVC_COPY_EXIT				0x80000000
#define FLG_UVC_COPY_WAITPTN			(FLG_UVC_COPY_TRIG | FLG_UVC_COPY_EXIT)

#define UVC_COPY_BANK_MAX				3
#define UVC_COPY_XCH_MAX				0x8000

typedef struct{
	ULONG	addr;
	ULONG	size;
	USHORT	width;
	USHORT	lines;
	ULLONG	frame_no;
	T_FJ_MOVIE_H264_STREAM_PARAM	stream_info;
	UCHAR							video_enc_id;
} T_UVC_DATA;

typedef struct{
	ULONG		payload_size;
	ULONG		payload_num;
	ULONG		header_size;
	
	ULONG		uvc_bank_num;
	ULONG		uvc_bank_index;
	ULONG		uvc_bank_data_size;
	ULONG		uvc_bank_one_size;
	ULONG		uvc_bank[UVC_COPY_BANK_MAX];
	void*		uvc_buf;

	ULONG		ipcu_bank_num;
	ULONG		ipcu_bank_index;
	ULONG		ipcu_bank_data_size;
	ULONG		ipcu_bank_one_size;
	ULONG		ipcu_bank[UVC_COPY_BANK_MAX];
	void*		ipcu_buf;
	
	ULONG		image_bank_num;
	ULONG		image_bank_index;
	ULONG		image_bank_data_size;
	ULONG		image_bank_one_size;
	ULONG		image_bank[UVC_COPY_BANK_MAX];
	void*		image_buf;
	
	T_UVC_DATA	data;
	
} T_UVC_COPY;

static ULONG uvc_copy( ULONG img_addr, ULONG img_size );
static void uvc_copy_xch( T_IM_XCH_CTRL_CMN *cmn0, T_IM_XCH_CTRL_COPY* copy0, T_IM_XCH_CTRL_CMN* cmn1, T_IM_XCH_CTRL_COPY* copy1 );
static int uvc_copy_iip(const ULONG src_addr, const ULONG dst_addr, const ULONG src_h_byte, const ULONG dst_h_byte, const USHORT h_size, const USHORT v_size);

static T_UVC_PARAM	g_uvc_param;
static T_UVC_COPY	g_uvc_copy;
static UCHAR		g_uvc_buf_ch = 0;

#define ROUNDUPN( val, n )		((((val) + (n) - 1) / (n)) * (n))

void OS_IF_UVC_Copy_Task(INT32 stacd )
{
	OS_USER_FLGPTN flgptn = 0;
	T_UVC_DATA uvc_data;
	ULONG addr;
	
	while(1) {
		// wait event
		OS_User_Twai_Flg( FID_UVC_COPY, FLG_UVC_COPY_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( FID_UVC_COPY, ~FLG_UVC_COPY_WAITPTN );

		if( IS_FLGPTN_EQUAL( flgptn, FLG_UVC_COPY_TRIG ) ){
			MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_57, FJ_MBALOG_FUNC_ID_LINUX_STR_YUV, FJ_MBALOG_TYPE_START, "copy"));
			uvc_data = g_uvc_copy.data;
			FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (ULONG)uvc_data.addr, (ULONG)uvc_data.size); // For safety
			addr = uvc_copy( uvc_data.addr, uvc_data.size );
			switch( g_uvc_param.format ){
				case E_CPU_IF_UVC_FORMAT_YUV:
				case E_CPU_IF_UVC_FORMAT_NV12:
					OS_IF_Rtos_YUV_Stream_Send( addr, uvc_data.size, uvc_data.width, uvc_data.lines, uvc_data.frame_no );
					break;
				case E_CPU_IF_UVC_FORMAT_JPEG:
					OS_IF_Rtos_MJPEG_Stream_Send2( addr, uvc_data.size, uvc_data.width, uvc_data.lines, uvc_data.frame_no );
					break;
				case E_CPU_IF_UVC_FORMAT_H264:
					uvc_data.stream_info.stream_address = addr;
					OS_IF_Rtos_H264_Stream_Send( &uvc_data.stream_info, uvc_data.video_enc_id, uvc_data.frame_no );
					break;
				default:
					UPRINTF_ERR(1);
					break;
			}
			MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_57, FJ_MBALOG_FUNC_ID_LINUX_STR_YUV, FJ_MBALOG_TYPE_END, ""));
		}

		// Exit Task
		if( IS_FLGPTN_EQUAL( flgptn, FLG_UVC_COPY_EXIT ) ){
			break;
		}
	}

	if( g_uvc_param.format == E_CPU_IF_UVC_FORMAT_YUV ){
		User_Utility_MM_free(g_uvc_copy.image_buf, 1);
	}
	User_Utility_MM_free(g_uvc_copy.uvc_buf, g_uvc_buf_ch);
	User_Utility_MM_free(g_uvc_copy.ipcu_buf, g_uvc_buf_ch);
	
	OS_User_Ext_Tsk();
}

void OS_IF_Rtos_UVC_Init( void )
{
	T_SHARE_SYSTEM_CONFIG cfg;
	T_SHARE_MOVIE_REC_CONFIG mcfg;
	ULONG image_bytesize = 0;
	ULONG align = 16;	// MM_alloc may not return address with alignment(ex 16n), so use "align" to adjust alignment.
	INT32 i;
	UCHAR	bank_num = 3;
	
	memset( &g_uvc_copy, 0, sizeof(g_uvc_copy) );
	Cate_Get_UVC_Param( &g_uvc_param );
	
	if( !g_uvc_param.with_header ){
		return;
	}

	#ifdef D_TEAMS_CERTIFICATION
	{
		ULONG	data_tmp;
		
		Cate_Get( 0x01, 0x28, 0x01, &data_tmp );
		if(data_tmp == E_SHARE_OP_IPU_MODE_IPU_ONLY)
		{
			// FPS=30 for YUV, FPS=40 for MJPEG
			if((g_uvc_param.format == E_CPU_IF_UVC_FORMAT_YUV) || (g_uvc_param.format == E_CPU_IF_UVC_FORMAT_NV12)){
				data_tmp = 0x41f00000; // VIEW_FPS = 30fps
			}
			else{
				data_tmp = 0x41f00000; // VIEW_FPS = 30fps
				//data_tmp = 0x42200000; // VIEW_FPS = 40fps
				//data_tmp = 0x42340000; // VIEW_FPS = 45fps
				//data_tmp = 0x42700000; // VIEW_FPS = 60fps
			}
			Cate_Set( 0x01, 0x16, 0x01, &data_tmp );
			//data_tmp = 0; // AE_FLICKER = Off
			//Cate_Set( 0x03, 0x05, 0x01, &data_tmp );
			data_tmp = 3; // EVP_VIEW = 3
			Cate_Set( 0x03, 0x10, 0x01, &data_tmp );
		}
	}
	#endif
	
	Get_Share_System_Config( &cfg );
	Get_Share_Movie_Rec_Config(&mcfg);
	switch( g_uvc_param.format ){
		case E_CPU_IF_UVC_FORMAT_YUV:
			image_bytesize = cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size * cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size * 2; // YUV
			UPRINTF("[UVC COPY] YUY2 size for uvc_copy is 0x%x\n", (UINT32)image_bytesize );
			g_uvc_buf_ch = 1;
			break;
		case E_CPU_IF_UVC_FORMAT_NV12:
			image_bytesize = (cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size * cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size * 3) / 2; // YUV
			UPRINTF("[UVC COPY] NV12 size for uvc_copy is 0x%x\n", (UINT32)image_bytesize );
			g_uvc_buf_ch = 1;
			break;
		case E_CPU_IF_UVC_FORMAT_JPEG:
			image_bytesize = cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size * cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size;
			if( cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_0].format == E_SHARE_YUV_FORMAT_422_SEMI ){
				image_bytesize *= 2;
			}else{
				image_bytesize *= 3;
				image_bytesize /= 2;
			}
			// almost same calculation as User_Sync_Create_Ring
			image_bytesize /= cfg.yuv_jpeg_ratio;
			image_bytesize = ROUNDUPN( image_bytesize, 16 ); // 4 align at User_Sync_Create_Ring, 16n for xch
			UPRINTF("[UVC COPY] JPEG size for uvc_copy is 0x%x\n", (UINT32)image_bytesize );
			g_uvc_buf_ch = 1;
			break;
		case E_CPU_IF_UVC_FORMAT_H264:
//			image_bytesize = mcfg.video[E_SHARE_YUV_OUT_H264_0].bitrate / 8; // bps -> Byte/s
			image_bytesize = 30000000 / 8; // 30Mbps -> Byte/s (reserve bigger size)
			image_bytesize /= 2; // temporary
			image_bytesize = ROUNDUPN( image_bytesize, 16 ); // 16n for xch
			UPRINTF("[UVC COPY] H264 size for uvc_copy is 0x%x\n", (UINT32)image_bytesize );
			g_uvc_buf_ch = 1;
			break;
		default:
			UPRINTF_ERR(1);
			return;//break;
	}
	
	g_uvc_copy.header_size		= g_uvc_param.header_size;
	g_uvc_copy.payload_size		= g_uvc_param.max_payload_size;
	g_uvc_copy.payload_num		= ( image_bytesize + (g_uvc_copy.payload_size - g_uvc_copy.header_size - 1) ) / (g_uvc_copy.payload_size - g_uvc_copy.header_size);
	UPRINTF("[UVC COPY] header_size:%d payload_size:%d payload_num:%d\n", (UINT32)g_uvc_copy.header_size, (UINT32)g_uvc_copy.payload_size, (UINT32)g_uvc_copy.payload_num );
	
	if( g_uvc_param.format == E_CPU_IF_UVC_FORMAT_YUV ){ // prepare for IIP
		g_uvc_copy.image_bank_num		= 2;	// for HW Macro to write, and for Copy Process to read
		g_uvc_copy.image_bank_index		= 0;
		g_uvc_copy.image_bank_data_size	= image_bytesize;
		g_uvc_copy.image_bank_one_size	= g_uvc_copy.image_bank_data_size + align;
		
		g_uvc_copy.image_buf = User_Utility_MM_alloc( g_uvc_copy.image_bank_one_size * g_uvc_copy.image_bank_num,  1 );
		UPRINTF("[UVC COPY] image_buf:0x%08x size:%d = %d * %d\n", (UINT32)g_uvc_copy.image_buf, (UINT32)(g_uvc_copy.image_bank_one_size * g_uvc_copy.image_bank_num), (UINT32)g_uvc_copy.image_bank_one_size, (UINT32)g_uvc_copy.image_bank_num );
		if( g_uvc_copy.image_buf == 0 ){
			UPRINTF_ERR(1);
			return;
		}
		for( i = 0; i < g_uvc_copy.image_bank_num; i ++ ){
			g_uvc_copy.image_bank[i] = ROUNDUPN( (ULONG)g_uvc_copy.image_buf, align ) + g_uvc_copy.image_bank_one_size * i;
		}
	}
	g_uvc_copy.uvc_bank_num		= bank_num;	// for Copy Process to write, and for Linux Process to read ( may takes 2frames )
	g_uvc_copy.uvc_bank_index	= 0;
	g_uvc_copy.uvc_bank_data_size = g_uvc_copy.payload_num * g_uvc_copy.payload_size;
	g_uvc_copy.uvc_bank_one_size = g_uvc_copy.uvc_bank_data_size + align;
	
	g_uvc_copy.ipcu_bank_num	= bank_num;	// for Copy Process to write, and for Linux Process to read ( may takes 2frames )
	g_uvc_copy.ipcu_bank_index	= 0;
	g_uvc_copy.ipcu_bank_data_size = 16;
	g_uvc_copy.ipcu_bank_one_size = g_uvc_copy.ipcu_bank_data_size + align;
	
	g_uvc_copy.uvc_buf   = User_Utility_MM_alloc( g_uvc_copy.uvc_bank_one_size   * g_uvc_copy.uvc_bank_num,    g_uvc_buf_ch );
	g_uvc_copy.ipcu_buf  = User_Utility_MM_alloc( g_uvc_copy.ipcu_bank_one_size  * g_uvc_copy.ipcu_bank_num,   g_uvc_buf_ch );
	UPRINTF("[UVC COPY] uvc_buf:0x%08x size:%d = %d * %d\n", (UINT32)g_uvc_copy.uvc_buf, (UINT32)(g_uvc_copy.uvc_bank_one_size * g_uvc_copy.uvc_bank_num), (UINT32)g_uvc_copy.uvc_bank_one_size, (UINT32)g_uvc_copy.uvc_bank_num );
	UPRINTF("[UVC COPY] ipcu_buf:0x%08x size:%d = %d * %d\n", (UINT32)g_uvc_copy.ipcu_buf, (UINT32)(g_uvc_copy.ipcu_bank_one_size * g_uvc_copy.ipcu_bank_num), (UINT32)g_uvc_copy.ipcu_bank_one_size, (UINT32)g_uvc_copy.ipcu_bank_num );
	
	if( g_uvc_copy.uvc_buf == 0 || g_uvc_copy.ipcu_buf == 0 ){
		UPRINTF_ERR(1);
		return;
	}
	
	for( i = 0; i < g_uvc_copy.uvc_bank_num; i ++ ){
		g_uvc_copy.uvc_bank[i] = ROUNDUPN( (ULONG)g_uvc_copy.uvc_buf, align ) + g_uvc_copy.uvc_bank_one_size * i;
	}
	for( i = 0; i < g_uvc_copy.ipcu_bank_num; i ++ ){
		g_uvc_copy.ipcu_bank[i] = ROUNDUPN( (ULONG)g_uvc_copy.ipcu_buf, align ) + g_uvc_copy.ipcu_bank_one_size * i;
	}
	
	OS_User_Clr_Flg( FID_UVC_COPY, 0 );
	OS_User_Sta_Tsk( TID_UVC_COPY, 0 );
}


void OS_IF_Rtos_UVC_Term( void )
{
//	T_OS_USER_RTSK pk_rtsk;
	
	if( !g_uvc_param.with_header ){
		return;
	}
	OS_User_Set_Flg( FID_UVC_COPY, FLG_UVC_COPY_EXIT );
	
//	OS_User_Ref_Tsk( TID_UVC_COPY, &pk_rtsk );
//	while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
//		OS_User_Dly_Tsk( 1 );
//		OS_User_Ref_Tsk( TID_UVC_COPY, &pk_rtsk );
//	}
}


BOOL OS_IF_Rtos_UVC_Is_Header( void )
{
	return g_uvc_param.with_header;
}


INT32 OS_IF_Rtos_UVC_Format( void )
{
	return g_uvc_param.format;
}


ULONG OS_IF_Rtos_UVC_Get_Image_Address( void )
{
	if( !g_uvc_param.with_header ){
		return 0;
	}
	return g_uvc_copy.image_bank[ g_uvc_copy.image_bank_index++ % g_uvc_copy.image_bank_num ];
}


void OS_IF_Rtos_UVC_YUV_Stream_Send( ULONG addr, ULONG size, FW_USHORT width, FW_USHORT lines, const ULLONG frame_no )
{
	if( !g_uvc_param.with_header ){
		OS_IF_Rtos_YUV_Stream_Send( addr, size, width, lines, frame_no );
	}else{
		g_uvc_copy.data.addr = addr;
		g_uvc_copy.data.size = size; // size has to be 16n
		g_uvc_copy.data.width = width;
		g_uvc_copy.data.lines = lines;
		g_uvc_copy.data.frame_no = frame_no;
		OS_User_Set_Flg( FID_UVC_COPY, FLG_UVC_COPY_TRIG );
	}
}


void OS_IF_Rtos_UVC_MJPEG_Stream_Send( FW_USHORT width, FW_USHORT lines, const ULLONG frame_no )
{
	if( !g_uvc_param.with_header ){
		OS_IF_Rtos_MJPEG_Stream_Send( width, lines, frame_no );
	}else{
		ULONG addr, size;
		User_Utility_JPEG_Get_Result( &addr, &size );
		size = ROUNDUPN( size, 16 );
		g_uvc_copy.data.addr = addr;
		g_uvc_copy.data.size = size;
		g_uvc_copy.data.width = width;
		g_uvc_copy.data.lines = lines;
		g_uvc_copy.data.frame_no = frame_no;
		OS_User_Set_Flg( FID_UVC_COPY, FLG_UVC_COPY_TRIG );
	}
}


void OS_IF_Rtos_UVC_H264_Stream_Send( T_FJ_MOVIE_H264_STREAM_PARAM* stream_info, UCHAR video_enc_id, const ULLONG frame_no )
{
	if( !g_uvc_param.with_header ){
		OS_IF_Rtos_H264_Stream_Send( stream_info, video_enc_id, frame_no );
	}else{
		g_uvc_copy.data.stream_info = *stream_info;
		g_uvc_copy.data.stream_info.stream_size = ROUNDUPN( g_uvc_copy.data.stream_info.stream_size, 16 );
		g_uvc_copy.data.video_enc_id = video_enc_id;
		g_uvc_copy.data.addr = g_uvc_copy.data.stream_info.stream_address;
		g_uvc_copy.data.size = g_uvc_copy.data.stream_info.stream_size;
		g_uvc_copy.data.frame_no = frame_no;
		OS_User_Set_Flg( FID_UVC_COPY, FLG_UVC_COPY_TRIG );
	}
}


static ULONG uvc_copy( ULONG img_addr, ULONG img_size )
{
	static UCHAR fid = 0;
	
	UINT32	payload_size	= g_uvc_copy.payload_size;
	UINT32	header_size		= g_uvc_copy.header_size;
	UINT32	data_size		= payload_size - header_size;
	UINT32	payload_num		= g_uvc_copy.payload_num;
	UINT32	header_remain	= header_size % 16;
	
	UINT32	dst_bk;
	UCHAR	*src, *dst;
	
	UINT32	*ipcu_dst;
	INT32	i;
	
	// Check
	if( payload_size & 0xF ){
		UPRINTF_ERR(1);
		UPRINTF( "payload_size = 0x%x -> should be 16n\n", (UINT32)payload_size );
	}
	if( header_size & 0x7 ){
		UPRINTF_ERR(1);
		UPRINTF( "header_size = 0x%x -> should be 8n\n", (UINT32)header_size );// TODO...
	}
	
	// Prepare Copy
	dst = (UCHAR*)g_uvc_copy.uvc_bank[ g_uvc_copy.uvc_bank_index++ % g_uvc_copy.uvc_bank_num ];
	src = (UCHAR*)img_addr;
	dst_bk = (UINT32)dst;
	
	// Prepare IPCU
	ipcu_dst = (UINT32*)( g_uvc_copy.ipcu_bank[ g_uvc_copy.ipcu_bank_index++ % g_uvc_copy.ipcu_bank_num ] );
	ipcu_dst[0] = dst_bk;
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (ULONG)ipcu_dst, (ULONG)g_uvc_copy.ipcu_bank_data_size );
	
	// Make Header
	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_LINUX_STR_YUV, FJ_MBALOG_TYPE_START, "header"));
	for( i = 0; i < payload_num; i ++ ){
		memset( dst, 0, header_size );
		dst[0] = header_size;
		dst[1] = 0x80 | fid;	// EOH | fid
		if( i == (payload_num-1) ){
			dst[1] |= 0x02;		// EOF
		}
		FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (ULONG)dst, (ULONG)header_size);
		dst += payload_size;
	}
	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_LINUX_STR_YUV, FJ_MBALOG_TYPE_END, ""));
	
	// Data Copy
	Im_Xch_Open( E_IM_XCH_CH_SEL_0, D_DDIM_WAIT_END_TIME );
	Im_Xch_Open( E_IM_XCH_CH_SEL_1, D_DDIM_WAIT_END_TIME );
	
	dst = (UCHAR*)dst_bk;
	dst += header_size;
	
	{
		T_IM_XCH_CTRL_CMN	xch_ctrl_cmn0  = {0};
		T_IM_XCH_CTRL_COPY	xch_ctrl_copy0 = {0};
		T_IM_XCH_CTRL_CMN	xch_ctrl_cmn1  = {0};
		T_IM_XCH_CTRL_COPY	xch_ctrl_copy1 = {0};
		INT32				copied_w, remain_w;
		UINT32				cnv_width, cnv_height, cnv_remain;
		
		// Set Xch (fix value)
		xch_ctrl_cmn0.mode		= D_IM_XCH_MD_COPY;
		xch_ctrl_cmn0.pCallBack	= NULL;
		xch_ctrl_copy0.vcyc		= 0;
		xch_ctrl_copy0.ven		= 0xFFFFFFFF;
		
		xch_ctrl_cmn1.mode		= D_IM_XCH_MD_COPY;
		xch_ctrl_cmn1.pCallBack	= NULL;
		xch_ctrl_copy1.vcyc		= 0;
		xch_ctrl_copy1.ven		= 0xFFFFFFFF;
		
		// convert img_size -> data_size(width) x height + remain
		cnv_width  = data_size;
		cnv_height = img_size / cnv_width;
		cnv_remain = img_size % cnv_width;
		
#if 0
		{
			static UINT32 max = 0;
			if ( max < img_size ){
				max = img_size;
				UPRINTF( "max = 0x%x\n", (UINT32)max );
			}
		}
#endif
		if( g_uvc_copy.uvc_bank_data_size < img_size ){
			UPRINTF_ERR(1);
			UPRINTF( "img_size (0x%x) > bank_data_size (0x%x)\n", (UINT32)img_size, (UINT32)g_uvc_copy.uvc_bank_data_size );
			img_size = g_uvc_copy.uvc_bank_data_size;
		}
		
		if((header_remain) && ( payload_size <= (UVC_COPY_XCH_MAX*2-16) )){
			if((header_remain == 8) && ( cnv_height > 1 )){
				// Copy by IIP(even lines) + CPU by plane
				if(cnv_height % 2){
					uvc_copy_iip((ULONG)src, (ULONG)dst, data_size, payload_size, cnv_width, (cnv_height - 1));
					memcpy((UCHAR *)(dst + (payload_size * (cnv_height - 1))), (UCHAR *)(src + (data_size * (cnv_height - 1))), cnv_width);
					FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (ULONG)(dst + (payload_size * (cnv_height - 1))), (ULONG)cnv_width);
				}
				else{
					uvc_copy_iip((ULONG)src, (ULONG)dst, data_size, payload_size, cnv_width, cnv_height);
				}
				
				if(cnv_remain){
					memcpy((UCHAR *)(dst + (payload_size * cnv_height)), (UCHAR *)(src + (data_size * cnv_height)), cnv_remain);
					FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (ULONG)(dst + (payload_size * cnv_height)), (ULONG)cnv_remain);
				}
			}
			else{
				// Copy by CPU by each line
				for( i = 0; i < (cnv_height + 1); i ++ ){
					if( i == cnv_height && cnv_remain ){
						remain_w = cnv_remain;
					}else{
						remain_w = cnv_width;
					}
					memcpy((UCHAR *)(dst + (i * payload_size)), (UCHAR *)(src + (i * data_size)), remain_w);
					FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (ULONG)(dst + (i * payload_size)), (ULONG)remain_w);
				}
			}
			*(UCHAR *)(dst + (cnv_height * payload_size) - (header_size - 1)) |= 0x02;		// EOF
		}
		else if( payload_size <= (UVC_COPY_XCH_MAX*2-16) ){
			// XSHW ( Horizontal Src Global can cover payload )
			// Copy by horizontal rect
			for( i = 0; i < 2; i ++ ){
				if( i == 0 ){
					if( cnv_height == 0 ){
						continue;
					}
					xch_ctrl_cmn0.d_vsize	= cnv_height;
					xch_ctrl_cmn0.d_hsize	= (cnv_width/2) & 0xFFFFFFF0;
					xch_ctrl_cmn1.d_vsize	= cnv_height;
					xch_ctrl_cmn1.d_hsize	= cnv_width - xch_ctrl_cmn0.d_hsize;
					xch_ctrl_cmn0.dst_addr	= (UINT32)dst;
					xch_ctrl_copy0.src_addr	= (UINT32)src;
				
				}else{
					xch_ctrl_cmn0.d_vsize	= 1;
					xch_ctrl_cmn0.d_hsize	= (cnv_remain/2) & 0xFFFFFFF0;
					xch_ctrl_cmn1.d_vsize	= 1;
					xch_ctrl_cmn1.d_hsize	= cnv_remain - xch_ctrl_cmn0.d_hsize;
					xch_ctrl_cmn0.dst_addr	= (UINT32)dst + (payload_size * cnv_height);
					xch_ctrl_copy0.src_addr	= (UINT32)src + (data_size    * cnv_height);
				}
			
				xch_ctrl_cmn0.g_hsize	= payload_size;
				xch_ctrl_copy0.s_hsize	= data_size;
					
				xch_ctrl_cmn1.g_hsize	= payload_size;
				xch_ctrl_copy1.s_hsize	= data_size;
				xch_ctrl_cmn1.dst_addr	= xch_ctrl_cmn0.dst_addr  + (xch_ctrl_cmn0.d_hsize);
				xch_ctrl_copy1.src_addr	= xch_ctrl_copy0.src_addr + (xch_ctrl_cmn0.d_hsize);
			
				uvc_copy_xch( &xch_ctrl_cmn0, &xch_ctrl_copy0, &xch_ctrl_cmn1, &xch_ctrl_copy1 );
			}
			*(UCHAR *)(dst + (cnv_height * payload_size) - (header_size - 1)) |= 0x02;		// EOF
		}
		else{
			// Copy by each line
			for( i = 0; i < payload_num; i ++ ){
				if( i == cnv_height && cnv_remain ){ // last
					remain_w = cnv_remain;
					copied_w = 0;
				}else{
					remain_w = cnv_width;
					copied_w = 0;
				}
				while( remain_w > 0 ){
					if( remain_w >= (UVC_COPY_XCH_MAX * 2) ){
						xch_ctrl_cmn0.d_vsize	= remain_w / (UVC_COPY_XCH_MAX * 2);
						xch_ctrl_cmn0.d_hsize	= UVC_COPY_XCH_MAX;
						xch_ctrl_cmn1.d_vsize	= xch_ctrl_cmn0.d_vsize;
						xch_ctrl_cmn1.d_hsize	= xch_ctrl_cmn0.d_hsize;
					}else{
						xch_ctrl_cmn0.d_vsize	= 1;
						xch_ctrl_cmn0.d_hsize	= (remain_w / 2 ) & 0xFFFFFFF0;;
						xch_ctrl_cmn1.d_vsize	= xch_ctrl_cmn0.d_vsize;
						xch_ctrl_cmn1.d_hsize	= remain_w - xch_ctrl_cmn0.d_hsize;
						
					}
					xch_ctrl_cmn0.g_hsize	= xch_ctrl_cmn0.d_hsize;
					xch_ctrl_copy0.s_hsize	= xch_ctrl_cmn0.d_hsize;
					xch_ctrl_cmn0.dst_addr	= (UINT32)dst + (i * payload_size) + copied_w;
					xch_ctrl_copy0.src_addr	= (UINT32)src + (i * data_size)    + copied_w;
					
					xch_ctrl_cmn1.g_hsize	= xch_ctrl_cmn1.d_hsize;
					xch_ctrl_copy1.s_hsize	= xch_ctrl_cmn1.d_hsize;
					xch_ctrl_cmn1.dst_addr	= xch_ctrl_cmn0.dst_addr  + (xch_ctrl_cmn0.d_hsize * xch_ctrl_cmn0.d_vsize);
					xch_ctrl_copy1.src_addr	= xch_ctrl_copy0.src_addr + (xch_ctrl_cmn0.d_hsize * xch_ctrl_cmn0.d_vsize);
					
					copied_w += (xch_ctrl_cmn0.d_hsize * xch_ctrl_cmn0.d_vsize) + (xch_ctrl_cmn1.d_hsize * xch_ctrl_cmn1.d_vsize);
					remain_w -= (xch_ctrl_cmn0.d_hsize * xch_ctrl_cmn0.d_vsize) + (xch_ctrl_cmn1.d_hsize * xch_ctrl_cmn1.d_vsize);
					uvc_copy_xch( &xch_ctrl_cmn0, &xch_ctrl_copy0, &xch_ctrl_cmn1, &xch_ctrl_copy1 );
				}
			}
		}
	}
	
	fid ^= 0x01; // FID
	
	Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
	Im_Xch_Close( E_IM_XCH_CH_SEL_1 );
	
	return (ULONG)ipcu_dst;

}

static void uvc_copy_xch( T_IM_XCH_CTRL_CMN *cmn0, T_IM_XCH_CTRL_COPY* copy0, T_IM_XCH_CTRL_CMN* cmn1, T_IM_XCH_CTRL_COPY* copy1 )
{
	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_XCH, FJ_MBALOG_TYPE_START, "xch"));
	// Set
	Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_0, cmn0 );
	Im_Xch_Ctrl_Copy( E_IM_XCH_CH_SEL_0, copy0 );
	Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_1, cmn1 );
	Im_Xch_Ctrl_Copy( E_IM_XCH_CH_SEL_1, copy1 );
	// Start
	Im_Xch_Start_Async( E_IM_XCH_CH_SEL_0 );
	Im_Xch_Start_Async( E_IM_XCH_CH_SEL_1 );
	// Wait
	Im_Xch_Wait_End( E_IM_XCH_CH_SEL_0 );
	Im_Xch_Wait_End( E_IM_XCH_CH_SEL_1 );
	
	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_XCH, FJ_MBALOG_TYPE_END, ""));
}

static int uvc_copy_iip(const ULONG src_addr, const ULONG dst_addr, const ULONG src_h_byte, const ULONG dst_h_byte, const USHORT h_size, const USHORT v_size)
{
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES];
	ULONG param_buffer_addr = ROUNDUP_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 1000;
	T_IM_IIP_UTIL_RR iip_util_rr0;
	int rval = 0;

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_07, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "IIP memcpy"));

	memset(&iip_util_rr0, 0, sizeof(T_IM_IIP_UTIL_RR));

	iip_util_rr0.afn_unitid = E_IM_IIP_UNIT_ID_AFN0;
	iip_util_rr0.sl_unitid = E_IM_IIP_UNIT_ID_SL2;
	iip_util_rr0.src_pixid = E_IM_IIP_PIXID_4;
	iip_util_rr0.dst_pixid = E_IM_IIP_PIXID_5;

	iip_util_rr0.src.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_rr0.src.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
	iip_util_rr0.src.gbl.pix_format = E_IM_IIP_PFMT_BAYER;
	iip_util_rr0.src.gbl.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.src.gbl.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.src.gbl.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.src.gbl.masterIF_Alpha = D_IM_IIP_MASTER_IF1;

	iip_util_rr0.dst.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_rr0.dst.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
	iip_util_rr0.dst.gbl.pix_format = E_IM_IIP_PFMT_BAYER;
	iip_util_rr0.dst.gbl.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Alpha = D_IM_IIP_MASTER_IF1;

	iip_util_rr0.afn_cache_select = E_IM_IIP_PARAM_CSEL_0;
	iip_util_rr0.pix_depth = D_IM_IIP_PDEPTH_8BITS;
	iip_util_rr0.rotation = E_IM_IIP_UTIL_ROTATE_000;
	iip_util_rr0.flip = E_IM_IIP_UTIL_FLIP_OFF;
	iip_util_rr0.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;

	iip_util_rr0.src.rect.width				= h_size;
	iip_util_rr0.src.rect.lines				= v_size;
	iip_util_rr0.src.gbl.pix_format			= E_IM_IIP_PFMT_BAYER;
	iip_util_rr0.src.gbl.width				= src_h_byte;
	iip_util_rr0.src.gbl.lines				= v_size;
	iip_util_rr0.src.gbl.line_bytes.Y_G		= src_h_byte;
	iip_util_rr0.src.gbl.line_bytes.Cb_B	= 0;
	iip_util_rr0.src.gbl.line_bytes.Cr_R	= 0;
	iip_util_rr0.src.gbl.line_bytes.Alpha	= 0;
	iip_util_rr0.src.gbl.addr.Y_G			= src_addr;
	iip_util_rr0.src.gbl.addr.Cb_B			= 0;
	iip_util_rr0.src.gbl.addr.Cr_R			= 0;
	iip_util_rr0.src.gbl.addr.Alpha			= 0;

	iip_util_rr0.dst.rect.width				= h_size;
	iip_util_rr0.dst.rect.lines				= v_size;
	iip_util_rr0.dst.gbl.pix_format			= E_IM_IIP_PFMT_BAYER;
	iip_util_rr0.dst.gbl.width				= dst_h_byte;
	iip_util_rr0.dst.gbl.lines				= v_size;
	iip_util_rr0.dst.gbl.line_bytes.Y_G		= dst_h_byte;
	iip_util_rr0.dst.gbl.line_bytes.Cb_B	= 0;
	iip_util_rr0.dst.gbl.line_bytes.Cr_R	= 0;
	iip_util_rr0.dst.gbl.line_bytes.Alpha	= 0;
	iip_util_rr0.dst.gbl.addr.Y_G			= dst_addr;
	iip_util_rr0.dst.gbl.addr.Cb_B			= 0;
	iip_util_rr0.dst.gbl.addr.Cr_R			= 0;
	iip_util_rr0.dst.gbl.addr.Alpha			= 0;

	rval = IM_IIP_Utility_ResizeRotate( &iip_util_rr0,
										NULL,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	UPRINTF_ERR(rval);

	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_07, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "IIP memcpy"));

	return rval;
}



