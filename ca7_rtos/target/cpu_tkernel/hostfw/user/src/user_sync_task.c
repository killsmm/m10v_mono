/**
 * @file		user_sync_task.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "TKernel_id.h"
#include "os_user_custom.h"
#include "user_sync_task.h"
#include "sdram_map_define.h"
#include "user_utility_jpeg.h"
#include "user_utility_dump_rawyuv.h"
#include "user_utility_ip.h"
#include "user_utility_disp.h"
#include "sdram_map_common.h"
#include "fj_encoder.h"
#include "audio_encoder.h"
#include "fj_filesystem.h"
#include "user_streaming.h"
#include "dcf_if.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define DUMP_RAW_DIVIDED		0
#define DUMP_RAW_ONE_SENSOR		1

#define DOL_YUV E_SHARE_YUV_OUT_MAX

#define THUMB_ENC_SIZE		(0xA000)

#define D_JPEG_Q_MAX			32	// temporary
#define D_JPEG_Q_MEMBER_BUF		32	// temporary

#define D_RING_MAX				D_JPEG_Q_MAX
/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#ifndef align_up
#define align_up(num, align) \
	(((num) + ((align) - 1)) & ~((align) - 1))
#endif

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
typedef enum {
	E_JPEG_STATE_Idle = 0,
	
	E_JPEG_STATE_Run_Main,
	E_JPEG_STATE_Run_Thumb,
	
	// for Start Thumb Event while Running Main
	E_JPEG_STATE_Wait_Main,
	E_JPEG_STATE_Restart_Thumb,
	
	// for Start Main Event while Running Main/Thumb
	E_JPEG_STATE_Wait_Main_withThumbMain,
	E_JPEG_STATE_Restart_Thumb_withThumbMain,
	E_JPEG_STATE_Run_Thumb_withThumbMain,
	E_JPEG_STATE_Restart_Main_withThumbMain,
	E_JPEG_STATE_Run_Main_withThumbMain,
	
	// for Start Main Event while Main running ( Thumb run with Start Main Event )
	E_JPEG_STATE_Wait_Main2,
	E_JPEG_STATE_Restart_Main2,
	
	E_JPEG_STATE_MAX,
} E_JPEG_STATE;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	BOOL				enable;
	BOOL				run;
	BOOL				media_save;
	UINT32				head;
	UINT32				num;
	UINT32				max_num;
	UINT32				req_frame_num;
	UINT32				cur_frame_cnt;
	UINT32				cur_frame_enc_cnt;
	ULLONG				frame_no[D_JPEG_Q_MAX];
} T_JPEG_Q;

typedef struct {
	UCHAR	index;
	UCHAR	buf_num;
	ULONG	buf_size;
	ULONG	buf_addr[D_RING_MAX];
	UCHAR	sdram_ch;
} T_RING;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_SHARE_SYSTEM_PARAM g_system_param;
static T_SHARE_YUV g_share_yuv;
static T_SHARE_YUV g_share_sgde_yuv;
static T_SHARE_SYSTEM_CONFIG g_system_config;
static T_SHARE_MOVIE_REC_CONFIG g_movie_rec_cfg;
static ULLONG g_jpeg_frame_no;
static BOOL g_pipe_yuv_addr_lock = FALSE;
ULONG g_thumb_jpeg_addr = 0;
ULONG g_thumb_jpeg_size = 0;
ULONG g_thumb_offset = 0;

static E_JPEG_STATE g_jpeg_state = E_JPEG_STATE_Idle;
static ULLONG g_jpeg_state_bk_frame_no_thumb = 0;
static ULLONG g_jpeg_state_bk_frame_no_main = 0;

static ULONG g_jpeg_in_frame_num = 0;

static T_JPEG_Q				g_jpeg_q;
static T_JPEG_SAVE_FILE2	g_jpeg_q_member[D_JPEG_Q_MEMBER_BUF];
static BOOL					g_jpeg_q_member_enc_end[D_JPEG_Q_MEMBER_BUF];

static T_RING				g_ring;

static BOOL g_postview_enable = FALSE;
static BOOL g_postview_quick= FALSE;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 User_Sync_Rcv_Msg( T_USER_SYNC_MSG* msg );
static VOID postview( ULLONG frame_no, BOOL is_from_qview );
static BOOL jpeg_state_machine( E_USER_SYNC_EVENT event, ULLONG frame_no );
static INT32 jpeg_q_init( BOOL enable );
static INT32 jpeg_q_del_member( ULLONG frame_no );
static BOOL jpeg_q_full( void );
static ULLONG jpeg_q_head_frame_no( void );
static INT32 jpeg_q_enqueue( const ULLONG* frame_no );
static INT32 jpeg_q_dequeue( ULLONG* frame_no );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void User_Sync_Task( INT32 stacd )
{
	static ULONG jpeg_width = 0, jpeg_lines = 0;
	static BOOL fd_draw_status = FALSE;
	T_SHARE_SYSTEM_CONFIG config;
	T_USER_SYNC_MSG msg;
	OS_USER_ER ercd = 0;
	ULONG thumb_yuv_addr;
	ULONG thumb_jpeg_addr, thumb_jpeg_size;
	ULONG offset;
	T_JPEG_ENC in;
	#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	T_JPEG_MPF_DATA mpf_data;
	#endif
	T_SHARE_LAYOUT *yuv_layout = NULL;
	int i;
	UINT32 align_addr = 0;

	while (1) {
		ercd = User_Sync_Rcv_Msg( &msg );
		UPRINTF_ERR(ercd);

		switch( msg.event ) {
		case E_USER_SYNC_EVENT_VIEW_START:
			g_postview_enable = FALSE;
			g_postview_quick  = FALSE;
			g_jpeg_state = E_JPEG_STATE_Idle;
			jpeg_q_init( FALSE );
			break;
		case E_USER_SYNC_EVENT_CAP_START:
			g_postview_enable = TRUE;
			g_postview_quick  = FALSE;
			g_jpeg_state = E_JPEG_STATE_Idle;
			jpeg_q_init( TRUE );
			break;
		case E_USER_SYNC_EVENT_LINUX_END_RAW:
			break;
		case E_USER_SYNC_EVENT_LINUX_END_YUV:
			break;
		case E_USER_SYNC_EVENT_LINUX_END_JPG:
			break;
		case E_USER_SYNC_EVENT_SAVE_RAW:
			if( (UCHAR)msg.param == DUMP_RAW_DIVIDED ) {
				User_Utility_SaveFile_RAW_Divided();
			} else {
				User_Utility_Dump_RAW_One_Sensor(msg.frame_no, User_Utility_is_IPCU_RAW_Enable() ? E_DUMP_IPCU : E_DUMP_STORAGE);
			}
			break;
		case E_USER_SYNC_EVENT_SAVE_YUV:
			if( (UCHAR)msg.param == DOL_YUV ) {
				User_Utility_SaveFile_DOL_YUV();
			} else {
				if( OS_IF_Rtos_UVC_Format() == E_CPU_IF_UVC_FORMAT_NV12 )
				{
					ULONG addr, size;
					E_SHARE_YUV_OUT yuv_out = (E_SHARE_YUV_OUT)msg.param;
					
					Get_Frame_Share_YUV( &g_share_yuv, msg.frame_no );
					addr = g_share_yuv.layout[yuv_out].addr_y;
					size = (g_share_yuv.layout[yuv_out].h_size * g_share_yuv.layout[yuv_out].v_size * 3) / 2;
					
					if( OS_IF_Rtos_UVC_Is_Header() ){
						OS_IF_Rtos_UVC_YUV_Stream_Send(addr, 
													   size, 
													   g_share_yuv.layout[yuv_out].h_size,
													   g_share_yuv.layout[yuv_out].v_size,
													   msg.frame_no );
					}else{
						OS_IF_Rtos_YUV_Stream_Send(addr, 
												   size, 
												   g_share_yuv.layout[yuv_out].h_size,
												   g_share_yuv.layout[yuv_out].v_size,
												   msg.frame_no );
					}
				}else{
					User_Utility_Dump_YUV((E_SHARE_YUV_OUT)msg.param, msg.frame_no, User_Utility_is_IPCU_YUV_Enable() ? E_DUMP_IPCU : E_DUMP_STORAGE);
				}
			}
			break;
		case E_USER_SYNC_EVENT_SAVE_RAW_YUV:
			if (User_Utility_is_IPCU_YUV_Enable() && User_Utility_is_IPCU_RAW_Enable()) {
				UPRINTF_ERR(-1); // Not enough memory size for transferring captured RAW+YUV
				break;
			}
			if (User_Utility_is_IPCU_YUV_Enable() || User_Utility_is_IPCU_RAW_Enable()) {
				User_Utility_Dump_RAW_YUV(msg.param1,
										  msg.frame_no,
										  User_Utility_is_IPCU_YUV_Enable(),
										  User_Utility_is_IPCU_RAW_Enable());
			} else {
				User_Utility_Dump_RAW_YUV(msg.param1,
										  msg.frame_no,
										  E_DUMP_STORAGE,
										  E_DUMP_STORAGE);
			}
			break;
		case E_USER_SYNC_EVENT_ENC_THUMB:
			
			if( (g_jpeg_q.enable && jpeg_q_full()) || (g_jpeg_q.enable && !g_jpeg_q.run) ){
				break;
			}
			
			if( g_jpeg_q.enable && g_jpeg_q.req_frame_num != 0 && g_jpeg_q.cur_frame_enc_cnt >= g_jpeg_q.req_frame_num ){
				break;
			}
			
			if( !jpeg_state_machine( msg.event, msg.frame_no ) ){
				break;
			}
			
			Get_Frame_Share_QVIEW_YUV( &g_share_yuv, msg.frame_no );
			Get_Share_System_Config(&g_system_config);
			
			postview( msg.frame_no, TRUE );
			
			Get_Share_System_Param(&g_system_param);
			g_system_param.lock_ipu_address_y = g_share_yuv.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
			g_system_param.lock_ipu_address_c = g_share_yuv.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
			Set_Share_System_Param( &g_system_param );
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "Thumb Enc"));
			if (User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_STORAGE && msg.param == E_NORMAL_JPEG){
				align_addr = align_up(g_ring.buf_addr[g_ring.index], 8);
				thumb_jpeg_addr = align_addr;
				thumb_jpeg_size = WIDTH_THUMB * LINES_THUMB * 2; // YUV422
				thumb_yuv_addr = thumb_jpeg_addr + thumb_jpeg_size;
			
				User_Utility_JPEG_Enc_Thumb(msg.frame_no,
											&g_share_yuv,
											E_SHARE_YUV_OUT_USER_1,
											thumb_jpeg_addr,
											thumb_jpeg_size,
											thumb_yuv_addr,
											g_system_config.jpeg_quality);
				User_Utility_JPEG_Get_Result(&thumb_jpeg_addr, &thumb_jpeg_size);
				offset = THUMB_ENC_SIZE + (align_addr - g_ring.buf_addr[g_ring.index]);
							
#if 1 // padding 4
				{
					// temporary for File System with EMMC.
					// force align 4
					UINT32 thumb_padding;
					thumb_padding = thumb_jpeg_size % 4;
					UPRINTF("thumb_padding = %d\n", thumb_padding);
					if( thumb_padding ){
						thumb_padding = 4 - thumb_padding;
						memset( (VOID*)(thumb_jpeg_addr + thumb_jpeg_size), 0xFF, thumb_padding);
						thumb_jpeg_size += thumb_padding;
						UPRINTF( "Thumb Padding = %d\n", (INT32)thumb_padding );
					}
				}
#endif
				if( g_jpeg_q.enable ){
					// jpeg_q_enqueue at main
					g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ].thumb_addr = thumb_jpeg_addr;
					g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ].thumb_size = thumb_jpeg_size;
				}
			} else {
				thumb_jpeg_addr = 0;
				thumb_jpeg_size = 0;
				offset = 0;
			}
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "Thumb Enc"));

			g_thumb_jpeg_addr = thumb_jpeg_addr;
			g_thumb_jpeg_size = thumb_jpeg_size;
			g_thumb_offset = offset;
			
			Get_Share_System_Param(&g_system_param);
			g_system_param.lock_ipu_address_y = D_ADDR_UNLOCK;
			g_system_param.lock_ipu_address_c = D_ADDR_UNLOCK;
			Set_Share_System_Param( &g_system_param );
			
			break;
		case E_USER_SYNC_EVENT_SAVE_JPG:
			if( (g_jpeg_q.enable && jpeg_q_full()) || (g_jpeg_q.enable && !g_jpeg_q.run) ){
				break;
			}
			if( g_jpeg_q.enable && g_jpeg_q.req_frame_num != 0 && g_jpeg_q.cur_frame_enc_cnt >= g_jpeg_q.req_frame_num ){
				break;
			}
			if( !jpeg_state_machine( msg.event, msg.frame_no ) ){
				break;
			}
			Get_Share_System_Config(&g_system_config);
			Get_Frame_Share_YUV( &g_share_yuv, msg.frame_no );
			// Check image on USER_0
			if(g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y == 0 || g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c == 0) {
				UPRINTF_ERR(1);
				break;
			}
			
			postview( msg.frame_no, FALSE );
			
			if( ( User_Utility_Record_H264_Is_Running() == TRUE ) || ( User_Utility_Record_H265_Is_Running() == TRUE ) ) {
				FJ_Top_Set_Clock_Select_JPEG(FJ_TOP_JPG_CLKSEL_100MHZ);
			}
			else {
				FJ_Top_Set_Clock_Select_JPEG(FJ_TOP_JPG_CLKSEL_200MHZ);
			}

			if(msg.param == E_STITCH_CALIB_JPEG) {
				g_jpeg_frame_no = msg.frame_no;
				Get_Share_System_Param(&g_system_param);
				Get_Frame_Share_YUV( &g_share_yuv, msg.frame_no );
				g_system_param.lock_address_y = g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
				g_system_param.lock_address_c = g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
				Set_Share_System_Param( &g_system_param );
				g_pipe_yuv_addr_lock = TRUE;

				jpeg_width 		= g_share_yuv.sensor[msg.param1].yuv[E_SHARE_YUV_OUT_USER_0].h_size;
				jpeg_lines 		= g_share_yuv.sensor[msg.param1].yuv[E_SHARE_YUV_OUT_USER_0].v_size;
				in.format		= g_share_yuv.sensor[msg.param1].yuv[E_SHARE_YUV_OUT_USER_0].format;
				in.width		= g_share_yuv.sensor[msg.param1].yuv[E_SHARE_YUV_OUT_USER_0].h_size;
				in.lines		= g_share_yuv.sensor[msg.param1].yuv[E_SHARE_YUV_OUT_USER_0].v_size;
				in.g_y_width	= g_share_yuv.sensor[msg.param1].yuv[E_SHARE_YUV_OUT_USER_0].h_byte;
				in.g_c_width	= g_share_yuv.sensor[msg.param1].yuv[E_SHARE_YUV_OUT_USER_0].h_byte;
				in.addr_y 		= g_share_yuv.sensor[msg.param1].yuv[E_SHARE_YUV_OUT_USER_0].addr_y;
				in.addr_c 		= g_share_yuv.sensor[msg.param1].yuv[E_SHARE_YUV_OUT_USER_0].addr_c;
				in.addr_code 	= align_up(g_ring.buf_addr[g_ring.index], 8);
				in.limit_size 	= g_ring.buf_size - in.addr_code - g_ring.buf_addr[g_ring.index];
				in.quality 		= 95;
				User_Utility_JPEG_Enc(msg.frame_no, &in, 0, 0);
				// no need q // if( g_jpeg_q.enable ){}
				g_ring.index = g_ring.index + 1 >= g_ring.buf_num ? 0 : g_ring.index + 1;
				break;
			}

			g_jpeg_frame_no = msg.frame_no;
			Get_Share_System_Param(&g_system_param);
			g_system_param.lock_ipu_address_y = g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
			g_system_param.lock_ipu_address_c = g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
			Set_Share_System_Param( &g_system_param );
			
			if((g_thumb_jpeg_addr == 0) || (g_thumb_jpeg_size == 0) || (g_thumb_offset == 0)){
				if (User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_STORAGE && msg.param == E_NORMAL_JPEG){
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "JPEG-0 Enc"));
					align_addr = align_up(g_ring.buf_addr[g_ring.index], 8);
					thumb_jpeg_addr = align_addr;
					thumb_jpeg_size = WIDTH_THUMB * LINES_THUMB * 2; // YUV422
					thumb_yuv_addr = thumb_jpeg_addr + thumb_jpeg_size;

					User_Utility_JPEG_Enc_Thumb(msg.frame_no,
												&g_share_yuv,
												E_SHARE_YUV_OUT_USER_0,
												thumb_jpeg_addr,
												thumb_jpeg_size,
												thumb_yuv_addr,
												g_system_config.jpeg_quality);
					User_Utility_JPEG_Get_Result(&thumb_jpeg_addr, &thumb_jpeg_size);
					offset = THUMB_ENC_SIZE + (align_addr - g_ring.buf_addr[g_ring.index]);
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "JPEG-0 Enc"));
					
#if 1 // padding 4
					{
						// temporary for File System with EMMC.
						// force align 4
						UINT32 thumb_padding;
						thumb_padding = thumb_jpeg_size % 4;
					
						if( thumb_padding ){
							thumb_padding = 4 - thumb_padding;
							memset( (VOID*)(thumb_jpeg_addr + thumb_jpeg_size), 0xFF, thumb_padding);
							thumb_jpeg_size += thumb_padding;
							UPRINTF( "Thumb Padding = %d\n", (INT32)thumb_padding );
						}
					}
#endif
					if( g_jpeg_q.enable ){
						// jpeg_q_enqueue at main
						g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ].thumb_addr = thumb_jpeg_addr;
						g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ].thumb_size = thumb_jpeg_size;
					}
				} else {
					thumb_jpeg_addr = 0;
					thumb_jpeg_size = 0;
					offset = 0;
				}
			}else{
				thumb_jpeg_addr = g_thumb_jpeg_addr;
				thumb_jpeg_size = g_thumb_jpeg_size;
				offset = g_thumb_offset;
				
				g_thumb_jpeg_addr = 0;
				g_thumb_jpeg_size = 0;
				g_thumb_offset = 0; 		
			}
#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
			if (User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_STORAGE && msg.param == E_NORMAL_JPEG){

				memset( &mpf_data, 0, sizeof( T_JPEG_MPF_DATA ) );
				// Make MPF1
				if(g_share_yuv.layout[E_SHARE_YUV_OUT_H264_0].addr_y != 0){
					Get_Share_System_Param(&g_system_param);
					g_system_param.lock_ipu_address_y = g_share_yuv.layout[E_SHARE_YUV_OUT_H264_0].addr_y;
					g_system_param.lock_ipu_address_c = g_share_yuv.layout[E_SHARE_YUV_OUT_H264_0].addr_c;
					Set_Share_System_Param( &g_system_param );
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "MP1 Enc"));

					align_addr = align_up(g_ring.buf_addr[g_ring.index] + offset, 8);
					mpf_data.mpf_enc[0].addr = align_addr;
					mpf_data.mpf_enc[0].size	= g_share_yuv.layout[E_SHARE_YUV_OUT_H264_0].h_byte * g_share_yuv.layout[E_SHARE_YUV_OUT_H264_0].v_size * 2; // YUV422
					
					User_Utility_JPEG_Enc_MPF(msg.frame_no,
												&g_share_yuv,
												E_SHARE_YUV_OUT_H264_0,
												mpf_data.mpf_enc[0].addr,
												mpf_data.mpf_enc[0].size,
												g_system_config.jpeg_quality);
					User_Utility_JPEG_Get_Result(&mpf_data.mpf_enc[0].addr, &mpf_data.mpf_enc[0].size);
					offset = offset + mpf_data.mpf_enc[0].size + (align_addr - (g_ring.buf_addr[g_ring.index] + offset));

					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "MP1 Enc"));
					Get_Share_System_Param(&g_system_param);
					g_system_param.lock_ipu_address_y = g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
					g_system_param.lock_ipu_address_c = g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
					Set_Share_System_Param( &g_system_param );
				}
				// Make MPF2
				if(g_share_yuv.layout[E_SHARE_YUV_OUT_H264_1].addr_y != 0){
					Get_Share_System_Param(&g_system_param);
					g_system_param.lock_ipu_address_y = g_share_yuv.layout[E_SHARE_YUV_OUT_H264_1].addr_y;
					g_system_param.lock_ipu_address_c = g_share_yuv.layout[E_SHARE_YUV_OUT_H264_1].addr_c;
					Set_Share_System_Param( &g_system_param );
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "MP2 Enc"));

					align_addr = align_up(g_ring.buf_addr[g_ring.index] + offset, 8);
					mpf_data.mpf_enc[1].addr	= align_addr;
					mpf_data.mpf_enc[1].size	= g_share_yuv.layout[E_SHARE_YUV_OUT_H264_1].h_byte * g_share_yuv.layout[E_SHARE_YUV_OUT_H264_1].v_size * 2; // YUV422
								
					User_Utility_JPEG_Enc_MPF(msg.frame_no,
												&g_share_yuv,
												E_SHARE_YUV_OUT_H264_1,
												mpf_data.mpf_enc[1].addr,
												mpf_data.mpf_enc[1].size,
												g_system_config.jpeg_quality);
					User_Utility_JPEG_Get_Result(&mpf_data.mpf_enc[1].addr, &mpf_data.mpf_enc[1].size);
					offset = offset + mpf_data.mpf_enc[1].size + (align_addr - (g_ring.buf_addr[g_ring.index] + offset));

					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "MP2 Enc"));
					Get_Share_System_Param(&g_system_param);
					g_system_param.lock_ipu_address_y = g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
					g_system_param.lock_ipu_address_c = g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
					Set_Share_System_Param( &g_system_param );
				}
				//Set MPF Info
				User_Utility_JPEG_Set_MPF(&mpf_data);
				if( g_jpeg_q.enable ){
					// jpeg_q_enqueue at main
					g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ].mpf = mpf_data;
				}
			}
			
#endif

			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "JPEG Main"));
			jpeg_width 		= g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_size;
			jpeg_lines 		= g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].v_size;
			in.format		= g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].format;
			in.width		= g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_size;
			in.lines		= g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].v_size;
			in.g_y_width	= g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_byte;
			in.g_c_width	= g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_byte;
			in.addr_y 		= g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
			in.addr_c 		= g_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
			align_addr		= align_up(g_ring.buf_addr[g_ring.index] + offset, 8);
			in.addr_code 	= align_addr;
			in.limit_size 	= g_ring.buf_size - offset - (align_addr - (g_ring.buf_addr[g_ring.index] + offset));
			in.limit_size   = g_system_config.jpeg_limited_size < in.limit_size ? g_system_config.jpeg_limited_size : in.limit_size;
			in.quality 		= g_system_config.jpeg_quality;
			UPRINTF("JPEG qulity = %d\n", in.quality);
			User_Utility_JPEG_Enc(msg.frame_no, &in, thumb_jpeg_addr, thumb_jpeg_size);
			if( g_jpeg_q.enable ){
				if( jpeg_q_enqueue( &msg.frame_no ) ){
					UPRINTF_ERR(1); // not expected
				}
				g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ].main_addr = in.addr_code;
				g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ].main_size = in.limit_size;
				g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ].main_width = jpeg_width;
				g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ].main_lines = jpeg_lines;
				User_Utility_JPEG_Get_Exif( msg.frame_no, &g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ] );
			}
			g_ring.index = g_ring.index + 1 >= g_ring.buf_num ? 0 : g_ring.index + 1;
			break;
		case E_USER_SYNC_EVENT_JPG_ENC_END:
			
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_ENC, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "JPEG Main"));
			Get_Share_System_Param(&g_system_param);
			g_system_param.lock_ipu_address_y = D_ADDR_UNLOCK;
			g_system_param.lock_ipu_address_c = D_ADDR_UNLOCK;
			if(g_pipe_yuv_addr_lock) {
				g_system_param.lock_address_y = D_ADDR_UNLOCK;
				g_system_param.lock_address_c = D_ADDR_UNLOCK;
				g_pipe_yuv_addr_lock = FALSE;
			}
			Set_Share_System_Param( &g_system_param );
			
			if( !jpeg_state_machine( msg.event, msg.frame_no ) ){
				break;
			}
			
			if( g_jpeg_q.enable && !g_jpeg_q.run ){
				break;
			}
			
			if( g_jpeg_q.enable ){
				
				g_jpeg_q.cur_frame_enc_cnt++;
				
				g_jpeg_q_member_enc_end[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ] = TRUE;
				
				if( !g_jpeg_q.media_save ){
					// use same func as q disable
					jpeg_q_dequeue( NULL );
					User_Utility_JPEG_ENC(jpeg_width, jpeg_lines, g_jpeg_frame_no);
					
					g_jpeg_q.cur_frame_cnt++;
					if( g_jpeg_q.req_frame_num != 0 && g_jpeg_q.cur_frame_cnt >= g_jpeg_q.req_frame_num ){
						g_jpeg_q.run = FALSE;
						Snd_Event_to_User( E_USER_EVENT_JPG_REQ_NUM_END, 0 );
						break;
					}
				}
				else{
					// update real size
					User_Utility_JPEG_Get_Result( &g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ].main_addr,
												  &g_jpeg_q_member[ msg.frame_no % D_JPEG_Q_MEMBER_BUF ].main_size );
					
					if( g_jpeg_q.num == 1 ){
						if( g_jpeg_q_member_enc_end[ jpeg_q_head_frame_no() % D_JPEG_Q_MEMBER_BUF ] ){
							User_Utility_JPEG_Save_File2( &g_jpeg_q_member[ jpeg_q_head_frame_no() % D_JPEG_Q_MEMBER_BUF ] );
						}
					}else{
						break;// skip
					}
				}
			}else{

				User_Utility_JPEG_ENC(jpeg_width, jpeg_lines, g_jpeg_frame_no);
			}
			break;
		case E_USER_SYNC_EVENT_JPG_SAVE_END:
			if( g_jpeg_q.enable ){
				ULLONG tmp_frame_no = 0;
				if( g_jpeg_q.media_save ){
					g_jpeg_q.cur_frame_cnt++;
					if( g_jpeg_q.req_frame_num != 0 && g_jpeg_q.cur_frame_cnt >= g_jpeg_q.req_frame_num ){
						g_jpeg_q.run = FALSE;
						Snd_Event_to_User( E_USER_EVENT_JPG_REQ_NUM_END, 0 );
						break;
					}
				}
				if( !jpeg_q_dequeue( &tmp_frame_no ) ){
					jpeg_q_del_member( tmp_frame_no );
					if( g_jpeg_q.num != 0 ){
						if( g_jpeg_q_member_enc_end[ jpeg_q_head_frame_no() % D_JPEG_Q_MEMBER_BUF ] ){							
							User_Utility_JPEG_Save_File2( &g_jpeg_q_member[ jpeg_q_head_frame_no() % D_JPEG_Q_MEMBER_BUF ] );
						}
					}
				}
			}
			break;
		case E_USER_SYNC_EVENT_RECORD_START_AGAIN:
		case E_USER_SYNC_EVENT_RECORD_START:
			Get_Share_Movie_Rec_Config(&g_movie_rec_cfg);

			// Only check audio stream0 since we dont support diff compress tpye for stream0/1
			if( g_movie_rec_cfg.audio[0].enable ) {
				if( g_movie_rec_cfg.audio[0].codec_type == FJ_COMPRESS_TYPE_PCM ) {
					BF_AudioEnc_SetTestMode(1);
				} else {
					BF_AudioEnc_SetTestMode(0);
				}
			}

			if(User_Utility_Record_H265_Is_Cache_Running() == FALSE && User_Utility_Record_H264_Is_Cache_Running() == FALSE) {
				User_Utility_Record_Mux_Initial();
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE) {
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_0)
					User_Utility_Record_H265_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265], g_movie_rec_cfg.base_fps, E_SHARE_YUV_OUT_H265);
				else
					User_Utility_Record_H264_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265], g_movie_rec_cfg.base_fps);
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) {
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_0)
					User_Utility_Record_H265_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0], g_movie_rec_cfg.base_fps, E_SHARE_YUV_OUT_H264_0);
				else if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_1)
					User_Utility_Record_H265_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0], g_movie_rec_cfg.base_fps, E_SHARE_YUV_OUT_H264_0);
				else
				User_Utility_Record_H264_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0], g_movie_rec_cfg.base_fps);
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) {
				User_Utility_Record_H264_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1], g_movie_rec_cfg.base_fps);
			}

			for(i = 0; i < 2; i++) {
				if( g_movie_rec_cfg.audio[i].stream_format == FJ_MOVIE_STREAM_FORMAT_ES_HEADER && g_movie_rec_cfg.audio[i].enable ) {
					User_Utility_Record_Audio_Start(i, &g_movie_rec_cfg.audio[i]);
				} else if( g_movie_rec_cfg.audio[i].stream_format == FJ_MOVIE_STREAM_FORMAT_ES && g_movie_rec_cfg.audio[i].enable ) {
					User_Utility_Aud_Mux_Config(i, &g_movie_rec_cfg.audio[i]);
				}
			}

			if(User_Utility_Record_H265_Is_Cache_Running() == FALSE && User_Utility_Record_H264_Is_Cache_Running() == FALSE) {
				User_Utility_Record_Mux_Start(&g_movie_rec_cfg.audio[0]);
			} else {
				User_Utility_Record_Mux_Cache_Start();
			}

			Snd_Event_to_User( E_USER_EVENT_RECORD_START_COMPLETE, 0LL );
			break;

		case E_USER_SYNC_EVENT_RECORD_CACHE_START_AGAIN:
			Get_Share_Movie_Rec_Config(&g_movie_rec_cfg);
			User_Utility_Record_Mux_Initial();
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_en == TRUE) {
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_0)
					User_Utility_Record_H265_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265], g_movie_rec_cfg.base_fps, E_SHARE_YUV_OUT_H265);
				else
					User_Utility_Record_H264_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265], g_movie_rec_cfg.base_fps);
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_en == TRUE) {
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_0)
					User_Utility_Record_H265_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0], g_movie_rec_cfg.base_fps, E_SHARE_YUV_OUT_H264_0);
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_1)
					User_Utility_Record_H265_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0], g_movie_rec_cfg.base_fps, E_SHARE_YUV_OUT_H264_0);
				else
				User_Utility_Record_H264_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0], g_movie_rec_cfg.base_fps);
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_en == TRUE) {
				User_Utility_Record_H264_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1], g_movie_rec_cfg.base_fps);
			}
			break;
		case E_USER_SYNC_EVENT_RECORD_CACHE_START:
			Get_Share_Movie_Rec_Config(&g_movie_rec_cfg);
			User_Utility_Record_Mux_Initial();
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_en == TRUE) {
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_0)
					User_Utility_Record_H265_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265], g_movie_rec_cfg.base_fps, E_SHARE_YUV_OUT_H265);
				else
					User_Utility_Record_H264_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265], g_movie_rec_cfg.base_fps);
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_en == TRUE) {
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_0)
					User_Utility_Record_H265_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0], g_movie_rec_cfg.base_fps, E_SHARE_YUV_OUT_H264_0);
				else if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_1)
					User_Utility_Record_H265_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0], g_movie_rec_cfg.base_fps, E_SHARE_YUV_OUT_H264_0);
				else
				User_Utility_Record_H264_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0], g_movie_rec_cfg.base_fps);
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_en == TRUE) {
				User_Utility_Record_H264_Cache_Start(&g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1], g_movie_rec_cfg.base_fps);
			}
			break;
		case E_USER_SYNC_EVENT_RECORD_CACHE_H265_STREAM_CB:
			if(msg.param1 == E_FJ_VIDEO_ENC_ENUM_H265_0) {
				User_Utility_Record_H265_Cache_StreamCB( msg.param );
			} else if(msg.param1 == E_FJ_VIDEO_ENC_ENUM_H265_1) {
				User_Utility_Record_H265_Cache_StreamCB( msg.param );
			}
			break;
		case E_USER_SYNC_EVENT_RECORD_CACHE_H264_STREAM_CB:
			User_Utility_Record_H264_Cache_StreamCB( msg.param );
			break;
		case E_USER_SYNC_EVENT_RECORD_STOP:
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE) {
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_0)
					User_Utility_Record_H265_Stop(E_SHARE_YUV_OUT_H265);
				else
					User_Utility_Record_H264_Stop( E_SHARE_YUV_OUT_H265 );
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) {
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_0)
					User_Utility_Record_H265_Stop( E_SHARE_YUV_OUT_H264_0 );
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_1)
					User_Utility_Record_H265_Stop( E_SHARE_YUV_OUT_H264_0 );
				else
				User_Utility_Record_H264_Stop( E_SHARE_YUV_OUT_H264_0 );
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) {
				User_Utility_Record_H264_Stop( E_SHARE_YUV_OUT_H264_1 );
			}
            for(i = 0; i < 2; i++) {
				if (g_movie_rec_cfg.audio[i].enable) User_Utility_Record_Audio_Stop(i);
            }
			break;
		case E_USER_SYNC_EVENT_FACE_DETECTION:
			Get_Share_System_Config(&g_system_config);
			Get_Frame_Share_YUV( &g_share_yuv, msg.frame_no );
			Get_Frame_Share_SGDE_YUV( &g_share_sgde_yuv, msg.frame_no );
			yuv_layout = &g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0];
			Get_Share_System_Config(&config);
			User_Utility_FD_Detect_One_Frame(yuv_layout);
			Get_Share_System_Param(&g_system_param);
			if( ((g_system_param.auto_framing_log & 0x08) || (g_system_param.auto_framing == 0)) &&
					g_system_param.fd_info && g_share_yuv.layout[E_SHARE_YUV_OUT_USER_1].h_size != 0 && g_share_yuv.layout[E_SHARE_YUV_OUT_USER_1].v_size != 0) {
				User_Utility_FD_Draw(&g_share_yuv,
									 yuv_layout,
									 config.disp_if == E_SHARE_DISP_IF_HDMI ? E_SHARE_DISP_IF_HDMI : E_SHARE_DISP_IF_LCD,
									 g_system_param.fd_info,
									 g_system_param.num_of_face);
				fd_draw_status = TRUE;
			}
			else{
				if(fd_draw_status){
					// Clear FD rectangle
					User_Utility_FD_Draw(&g_share_yuv,
										 yuv_layout,
										 config.disp_if == E_SHARE_DISP_IF_HDMI ? E_SHARE_DISP_IF_HDMI : E_SHARE_DISP_IF_LCD,
										 NULL,
										 0);
				}
				fd_draw_status = FALSE;
			}
			break;
		case E_USER_SYNC_EVENT_PERSON_DETECTION:
			Get_Share_System_Config(&g_system_config);
			Get_Frame_Share_YUV( &g_share_yuv, msg.frame_no );
            Get_Frame_Share_SGDE_YUV( &g_share_sgde_yuv, msg.frame_no );
			Get_Share_System_Config(&config);
			yuv_layout = &g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0];
			User_Utility_PD_Detect_One_Frame(yuv_layout);
			Get_Share_System_Param(&g_system_param);
			if(  g_system_param.auto_framing == 0 &&
					g_system_param.pd_info && g_share_yuv.layout[E_SHARE_YUV_OUT_USER_1].h_size != 0 && g_share_yuv.layout[E_SHARE_YUV_OUT_USER_1].v_size != 0) {
				User_Utility_PD_Draw(&g_share_yuv,
									 yuv_layout,
									 config.disp_if == E_SHARE_DISP_IF_HDMI ? E_SHARE_DISP_IF_HDMI : E_SHARE_DISP_IF_LCD,
									 g_system_param.pd_info,
									 (g_system_param.roi_en == 1 || g_system_param.zoom_ratio != 1.0) ? 0 : g_system_param.num_of_person);
			}
			break;
		case E_USER_SYNC_EVENT_FEED_STREAM:
#if defined(D_STREAMING_FROM_FILE)
			User_StreamingGetFrameSync(msg.param, (UCHAR)msg.frame_no);
#endif
			break;
		#if 1
		case E_USER_SYNC_EVENT_SAVE_FILE:
			{
				ULONG fid;
				ULONG addr;
				ULONG ret_size;
				static int file_cnt = 0;
				CHAR str[32];

				addr = (ULONG)msg.frame_no;
				sprintf(str, "I:/test%d.yuv", file_cnt++);
				fid = FJ_Fs_Open(str, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC));
				FJ_Fs_Write(fid, (UINT32 *)addr, msg.param * msg.param1, &ret_size);
				FJ_Fs_Close( fid );
				break;
			}
		#endif
		default:
			break;
		}
	}
}

INT32 Snd_Msg_To_User_Sync(const T_USER_SYNC_MSG* msg)
{
	OS_USER_ER	ercd = 0;
	T_USER_SYNC_MBOX *mbox = NULL;

	ercd = OS_User_Get_Mpf( MPFID_USER_SYNC, (OS_USER_VP*)&mbox );
	UPRINTF_ERR(ercd);

	mbox->blf_addr = mbox;

	memcpy( mbox->msg, msg, sizeof(T_USER_SYNC_MSG) );

	ercd = OS_User_Snd_Mbx( MID_USER_SYNC, (T_OS_USER_MSG*)mbox );
	UPRINTF_ERR(ercd);

	return (ercd);
}

ULONG User_Sync_HSCap_Get_Jpeg_Bank_Num(void)
{
	return g_ring.buf_num;
}

INT32 User_Sync_Set_Frame_Num( ULONG frame_num )
{
	g_jpeg_in_frame_num = frame_num;
	return OK;
}

INT32 User_Sync_Create_Ring(const ULONG size, const ULONG yuv_size_byte, const UCHAR ratio)
{
	T_SHARE_SYSTEM_CONFIG cfg;
	int i;
	int rval = 0;
	UPRINTF("User_Sync_Create_Ring size = %x, yuv_size_byte = %x, ratio = %d\n",
		size, yuv_size_byte, ratio);
	memset(&g_ring, 0, sizeof(g_ring));

	Get_Share_System_Config(&cfg);
	if ( size <= (cfg.end_available_memory_addr_ch0 - Get_Share_Available_Memory(0) + 1)) {
		UPRINTF("req_size:%8x, ch0_size:%8x\n", (UINT32)size, (UINT32)(cfg.end_available_memory_addr_ch0 - Get_Share_Available_Memory(0) + 1));
		g_ring.sdram_ch = 0;
	} else if ( size <= (cfg.end_available_memory_addr_ch1 - Get_Share_Available_Memory(1) + 1)) {
		UPRINTF("req_size:%8x, ch1_size:%8x\n", (UINT32)size, (UINT32)(cfg.end_available_memory_addr_ch1 - Get_Share_Available_Memory(1) + 1));
		g_ring.sdram_ch = 1;
	} else {
		UPRINTF("req_size:%8x, ch0_size:%8x\n", (UINT32)size, (UINT32)(cfg.end_available_memory_addr_ch0 - Get_Share_Available_Memory(0) + 1));
		UPRINTF("req_size:%8x, ch1_size:%8x\n", (UINT32)size, (UINT32)(cfg.end_available_memory_addr_ch1 - Get_Share_Available_Memory(1) + 1));
		rval = -1;
		UPRINTF_ERR(rval);
		return rval;
	}

	g_ring.buf_size = align_up(yuv_size_byte / ratio, 4);
	g_ring.buf_num = size / g_ring.buf_size;
	g_ring.buf_num = g_ring.buf_num > D_RING_MAX ? D_RING_MAX : g_ring.buf_num;

	for (i = 0; i < g_ring.buf_num; i++) {
		g_ring.buf_addr[i] = (ULONG)User_Utility_MM_alloc(g_ring.buf_size, g_ring.sdram_ch);
		if (g_ring.buf_addr[i] == 0) {
			rval = -1;
			UPRINTF_ERR(rval);
			UPRINTF("ERR, ring - buf_size:%lx, buf_num:%d\n", g_ring.buf_size, g_ring.buf_num);
			break;
		}
	}

	if (rval) {
		for (i = 0; i < g_ring.buf_num; i++) {
			if (g_ring.buf_addr[i]) {
				User_Utility_MM_free((void*)g_ring.buf_addr[i], g_ring.sdram_ch);
			}
		}
		memset(&g_ring, 0, sizeof(g_ring));
	} else {
		Set_Share_Media_Bank(g_ring.buf_num);
		g_jpeg_q.max_num = g_ring.buf_num;
	}

	UPRINTF("ring - buf_size:%lx, buf_num:%d\n", g_ring.buf_size, g_ring.buf_num);
	return rval;
}

INT32 User_Sync_Destroy_Ring(void)
{
	int i;
	int rval = 0;

	for (i = 0; i < g_ring.buf_num; i++) {
		if (g_ring.buf_addr[i]) {
			User_Utility_MM_free((void*)g_ring.buf_addr[i], g_ring.sdram_ch);
		}
	}

	memset(&g_ring, 0, sizeof(g_ring));

	return rval;
}

BOOL User_Sync_Is_Ring_Created(void)
{
	return g_ring.buf_size ? TRUE : FALSE;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static INT32 User_Sync_Rcv_Msg( T_USER_SYNC_MSG* msg )
{
	OS_USER_ER	ercd = 0;
	T_USER_SYNC_MBOX *mbox = NULL;
	
	/* [mbox] */
	ercd = OS_User_Rcv_Mbx( MID_USER_SYNC, (T_OS_USER_MSG**)&mbox );
	if( ercd != D_OS_USER_E_OK ){
		UPRINTF_ERR(ercd);
		return ( ercd );
	}

	memcpy( msg, mbox->msg, sizeof(T_USER_SYNC_MSG) );

	ercd = OS_User_Rel_Mpf( MPFID_USER_SYNC, (OS_USER_VP)mbox->blf_addr );
	UPRINTF_ERR(ercd);
	return (ercd);
}

static VOID postview( ULLONG frame_no, BOOL is_from_qview )
{
	E_SHARE_YUV_OUT			yuv_out = E_SHARE_YUV_OUT_USER_0;
	
	if( !g_postview_enable ){
		return;
	}
	
	// Get_Share_System_Config( &g_system_config ); // called before this func
	
	if( is_from_qview ){
		g_postview_quick = TRUE;
		// Get_Frame_Share_QVIEW_YUV( &g_share_yuv, frame_no ); // called before this func
		yuv_out = E_SHARE_YUV_OUT_USER_1;
	}else{
		if( g_postview_quick ){
			return;
		}else{
			// Get_Frame_Share_YUV( &g_share_yuv, frame_no ); // called before this func
			
			switch( g_system_config.op_ipu_mode ){
				case E_SHARE_OP_IPU_MODE_PIPE_ONLY:
					yuv_out = E_SHARE_YUV_OUT_USER_0;
					break;
				case E_SHARE_OP_IPU_MODE_NORMAL:
				case E_SHARE_OP_IPU_MODE_IPU_ONLY:
					yuv_out = E_SHARE_YUV_OUT_USER_0;
					break;
				default:
					yuv_out = E_SHARE_YUV_OUT_USER_1;
					break;
			}
		}
	}
	
	switch ( g_system_config.disp_if ) {
		case E_SHARE_DISP_IF_LCD:
			User_Utility_LCD_Yuv( &g_share_yuv.layout[ yuv_out ]);
		break;
		case E_SHARE_DISP_IF_HDMI:
			User_Utility_HDMI_Yuv( &g_share_yuv.layout[ yuv_out ] );
		break;
		default:
			User_Utility_HDMI_Yuv( &g_share_yuv.layout[ yuv_out ] );
		break;
	}

}

static void jpeg_state_machine_restart_thumb( void )
{
	T_USER_SYNC_MSG user_sync_msg;
	OS_USER_ER os_err = 0;
	
	user_sync_msg.event		= E_USER_SYNC_EVENT_ENC_THUMB;
	user_sync_msg.param		= E_NORMAL_JPEG;
	user_sync_msg.frame_no	= g_jpeg_state_bk_frame_no_thumb;
	os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
	UPRINTF_ERR(os_err);
}

static void jpeg_state_machine_restart_main( void )
{
	T_USER_SYNC_MSG user_sync_msg;
	OS_USER_ER os_err = 0;
	
	user_sync_msg.event		= E_USER_SYNC_EVENT_SAVE_JPG;
	user_sync_msg.param		= E_NORMAL_JPEG;
	user_sync_msg.frame_no	= g_jpeg_state_bk_frame_no_main;
	os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
	UPRINTF_ERR(os_err);
}

static BOOL jpeg_state_machine( E_USER_SYNC_EVENT event, ULLONG frame_no )
{
	BOOL ret = FALSE;
	
#if 1 // Log
	switch( event ){
		case E_USER_SYNC_EVENT_ENC_THUMB:
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_HOST, FJ_MBALOG_TYPE_POINT, "ThumbStart"));
			break;
		case E_USER_SYNC_EVENT_SAVE_JPG:
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_HOST, FJ_MBALOG_TYPE_POINT, "MainStart"));
			break;
		case E_USER_SYNC_EVENT_JPG_ENC_END:
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_HOST, FJ_MBALOG_TYPE_POINT, "MainEnd"));
			break;
		default:
			break;
	}
#endif
	
	switch( g_jpeg_state ){
		case E_JPEG_STATE_Idle:
			switch( event ){
				case E_USER_SYNC_EVENT_ENC_THUMB:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "Idle -> RunThumb"));
					g_jpeg_state = E_JPEG_STATE_Run_Thumb;
					g_jpeg_state = E_JPEG_STATE_Idle; // Auto because Sync
					ret = TRUE;
					break;
				case E_USER_SYNC_EVENT_SAVE_JPG:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "Idle -> RunMain"));
					g_jpeg_state = E_JPEG_STATE_Run_Main;
					ret = TRUE;
					break;
				case E_USER_SYNC_EVENT_JPG_ENC_END:
					break;
				default:
					break;
			}
		break;
		
		case E_JPEG_STATE_Run_Main:
			switch( event ){
				case E_USER_SYNC_EVENT_ENC_THUMB:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "RunMain -> WaitMain"));
					g_jpeg_state = E_JPEG_STATE_Wait_Main;
					g_jpeg_state_bk_frame_no_thumb = frame_no;
					//ret = TRUE;
					break;
				case E_USER_SYNC_EVENT_SAVE_JPG:
					// for make Thumb with E_USER_SYNC_EVENT_SAVE_JPG
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "RunMain -> WaitMain2"));
					g_jpeg_state = E_JPEG_STATE_Wait_Main2;
					g_jpeg_state_bk_frame_no_main = frame_no;
					//ret = TRUE;
					break;
				case E_USER_SYNC_EVENT_JPG_ENC_END:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "RunMain -> Idle"));
					g_jpeg_state = E_JPEG_STATE_Idle;
					ret = TRUE;
					break;
				default:
					break;
			}
		break;
	
		case E_JPEG_STATE_Wait_Main:
			switch( event ){
				case E_USER_SYNC_EVENT_ENC_THUMB:
					break;
				case E_USER_SYNC_EVENT_SAVE_JPG:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "WaitMain -> WaitMainWith"));
					g_jpeg_state = E_JPEG_STATE_Wait_Main_withThumbMain;
					g_jpeg_state_bk_frame_no_main = frame_no;
					// ret = TRUE
					break;
				case E_USER_SYNC_EVENT_JPG_ENC_END:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "WaitMain -> RestartThumb"));
					g_jpeg_state = E_JPEG_STATE_Restart_Thumb;
					ret = TRUE;
					jpeg_state_machine_restart_thumb();
					break;
				default:
					break;
			}
		break;
		
		case E_JPEG_STATE_Restart_Thumb:
			switch( event ){
				case E_USER_SYNC_EVENT_ENC_THUMB:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "RestartThumb -> RunThumb"));
					g_jpeg_state = E_JPEG_STATE_Run_Thumb;
					g_jpeg_state = E_JPEG_STATE_Idle; // Auto because Sync
					ret = TRUE;
					break;
				case E_USER_SYNC_EVENT_JPG_ENC_END:
					break;
				case E_USER_SYNC_EVENT_SAVE_JPG:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "RestartThumb -> RestartThumbWith"));
					g_jpeg_state = E_JPEG_STATE_Restart_Thumb_withThumbMain;
					g_jpeg_state_bk_frame_no_main = frame_no;
					//ret = TRUE;
					break;
				default:
					break;
			}
		break;
		
		case E_JPEG_STATE_Wait_Main_withThumbMain:
			switch( event ){
				case E_USER_SYNC_EVENT_ENC_THUMB:
					break;
				case E_USER_SYNC_EVENT_SAVE_JPG:
					break;
				case E_USER_SYNC_EVENT_JPG_ENC_END:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "WaitMainWith -> RestartThumbWith"));
					g_jpeg_state = E_JPEG_STATE_Restart_Thumb_withThumbMain;
					jpeg_state_machine_restart_thumb();
					ret = TRUE;
					break;
				default:
					break;
			}
		break;
		
		case E_JPEG_STATE_Restart_Thumb_withThumbMain:
			switch( event ){
				case E_USER_SYNC_EVENT_ENC_THUMB:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "RestartThumbWith -> RunThumbWith"));
					g_jpeg_state = E_JPEG_STATE_Run_Thumb_withThumbMain;
					g_jpeg_state = E_JPEG_STATE_Restart_Main_withThumbMain; // Auto because Sync
					jpeg_state_machine_restart_main();
					ret = TRUE;
					break;
				case E_USER_SYNC_EVENT_SAVE_JPG:
					break;
				case E_USER_SYNC_EVENT_JPG_ENC_END:
					break;
				default:
					break;
			}
		break;
		
		case E_JPEG_STATE_Restart_Main_withThumbMain:
			switch( event ){
				case E_USER_SYNC_EVENT_ENC_THUMB:
					break;
				case E_USER_SYNC_EVENT_SAVE_JPG:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "RestartMainWith -> RunMainWith"));
					g_jpeg_state = E_JPEG_STATE_Run_Main_withThumbMain;
					ret = TRUE;
					break;
				case E_USER_SYNC_EVENT_JPG_ENC_END:
					break;
				default:
					break;
			}
		break;
		
		case E_JPEG_STATE_Run_Main_withThumbMain:
			switch( event ){
				case E_USER_SYNC_EVENT_ENC_THUMB:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "RunMainWith -> WaitMainWith"));
					g_jpeg_state = E_JPEG_STATE_Wait_Main_withThumbMain;
					g_jpeg_state_bk_frame_no_thumb = frame_no;
					g_jpeg_state_bk_frame_no_main = frame_no; // same frame_no as thumb
					//ret = TRUE;
					break;
				case E_USER_SYNC_EVENT_SAVE_JPG:
					break;
				case E_USER_SYNC_EVENT_JPG_ENC_END:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "RunMainWith -> Idle"));
					g_jpeg_state = E_JPEG_STATE_Idle;
					ret = TRUE;
					break;
				default:
					break;
			}
		break;
		
		case E_JPEG_STATE_Wait_Main2:
			switch( event ){
				case E_USER_SYNC_EVENT_ENC_THUMB:
					break;
				case E_USER_SYNC_EVENT_SAVE_JPG:
					break;
				case E_USER_SYNC_EVENT_JPG_ENC_END:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "WaitMain2 -> RestartMain2"));
					g_jpeg_state = E_JPEG_STATE_Restart_Main2;
					jpeg_state_machine_restart_main();
					ret = TRUE;
					break;
				default:
					break;
			}
		break;
	
		case E_JPEG_STATE_Restart_Main2:
			switch( event ){
				case E_USER_SYNC_EVENT_ENC_THUMB:
					break;
				case E_USER_SYNC_EVENT_SAVE_JPG:
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_CTRL, FJ_MBALOG_FUNC_ID_JPEG, FJ_MBALOG_TYPE_POINT, "RestartMain2 -> RunMain"));
					g_jpeg_state = E_JPEG_STATE_Run_Main;
					ret = TRUE;
					break;
				case E_USER_SYNC_EVENT_JPG_ENC_END:
					break;
				default:
					break;
			}
		break;
	
		
		default:
		break;
	}
	
	
	return ret;
}

static INT32 jpeg_q_init( BOOL enable )
{
	memset( &g_jpeg_q, 0, sizeof( g_jpeg_q ) );
	memset( &g_jpeg_q_member, 0, sizeof( g_jpeg_q_member ) );
	memset( &g_jpeg_q_member_enc_end, 0, sizeof( g_jpeg_q_member_enc_end ) );
	g_jpeg_q.enable = enable;	// assume enable if capture && dump storage
	g_jpeg_q.run = enable;
	g_jpeg_q.max_num = g_ring.buf_num;

	g_jpeg_q.req_frame_num = g_jpeg_in_frame_num;
	if( User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_STORAGE ){
		g_jpeg_q.media_save = TRUE;
	}

	return OK;
}

static INT32 jpeg_q_del_member( ULLONG frame_no )
{
	memset( &g_jpeg_q_member[ frame_no % D_JPEG_Q_MEMBER_BUF ], 0, sizeof( T_JPEG_SAVE_FILE2 ) );
	g_jpeg_q_member_enc_end[ frame_no % D_JPEG_Q_MEMBER_BUF ] = FALSE;
	return OK;
}

static BOOL jpeg_q_full( void )
{
	return ( g_jpeg_q.num >= g_jpeg_q.max_num );
}

static ULLONG jpeg_q_head_frame_no( void )
{
	// no check empty, please check call side
	return g_jpeg_q.frame_no[ g_jpeg_q.head ];
}

static INT32 jpeg_q_enqueue( const ULLONG* frame_no )
{
	UINT32 index;
	if( g_jpeg_q.num < g_jpeg_q.max_num ){
		index = ( g_jpeg_q.head + g_jpeg_q.num ) % g_jpeg_q.max_num;
		g_jpeg_q.frame_no[ index ] = *frame_no;
		g_jpeg_q.num ++;
		return OK;
	}else{
		return NG; // FULL
	}
}

static INT32 jpeg_q_dequeue( ULLONG* frame_no )
{
	if( g_jpeg_q.num > 0 ){
		if( frame_no != NULL ){
			*frame_no = g_jpeg_q.frame_no[ g_jpeg_q.head ];
		}
		g_jpeg_q.head = ( g_jpeg_q.head + 1 ) % g_jpeg_q.max_num;
		g_jpeg_q.num --;
		return OK;
	}else{
		return NG; // Empty
	}
}
