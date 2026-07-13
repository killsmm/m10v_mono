/**
 * @file		h264_movie.h
 * @brief		H.264 Movie Function Control Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _H264_MOVIE_H_
#define _H264_MOVIE_H_

#include "h264_if.h"
#include "h264_if_common.h"
#include "debug.h"
#include "fs_if.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//----------------------
// Event Flag
//----------------------
/* FID_H264 */
/* Common Control */
#define FLG_H264_START						(0x00000001)
#define FLG_H264_STOP						(0x00000002)
#define FLG_H264_FLUSH_OPS					(0x00000004)
#define FLG_H264_WDT						(0x00000008)
#define FLG_H264_FORCE_STOP					(0x00000010)
#define FLG_H264_ENC_FREE_ALL_VSTREAM		(0x00000020)
#define FLG_H264_ERROR_VRAW_MULTI			(0x00000040)
#define FLG_H264_ERROR_VSTRM_EMPTY			(0x00000080)

/* Task Control */
#define FLG_H264_ENT_INIT_END				(0x00000100)
#define FLG_H264_ENT_STATE_CHANGE			(0x00000200)
#define FLG_H264_ENT_STOP					(0x00000400)
#define FLG_H264_CC_INIT_END				(0x00000800)
#define FLG_H264_CC_STATE_CHANGE			(0x00001000)
#define FLG_H264_CC_STOP					(0x00002000)
#define FLG_H264_DISP_INIT_END				(0x00004000)
#define FLG_H264_DISP_STATE_CHANGE			(0x00008000)
#define FLG_H264_DISP_STOP					(0x00010000)
#define FLG_H264_DISP_LCD_INIT_END			(0x00020000)
#define FLG_H264_DISP_LCD_STATE_CHANGE		(0x00040000)
#define FLG_H264_DISP_LCD_STOP				(0x00080000)
#define FLG_H264_DISP_HDMI_INIT_END			(0x00100000)
#define FLG_H264_DISP_HDMI_STATE_CHANGE		(0x00200000)
#define FLG_H264_DISP_HDMI_STOP				(0x00400000)
#define FLG_H264_VECTOR_STOP				(0x00800000)
#define FLG_H264_CONTROLLER_STOP			(0x01000000)
#define FLG_H264_STOP_ENT_TSK				(0x02000000)
#define FLG_H264_DEC_STREAM_RECEIVE			(0x04000000)

/* Ribery Control */
#define FLG_H264_DEC_ERROR					(0x08000000)
#define FLG_H264_CODEC_FLUSH_COMP			(0x10000000)
#define FLG_H264_ENC_VSTRM_COMP				(0x20000000)
#define FLG_H264_DEC_VDPB_COMP				(0x40000000)
#define FLG_H264_ENC_Y_VEC_COMP				(0x80000000)

/* FID_H264_SYNC */
/* Sync Control */
#define FLG_H264_SYNC_ENC_MASTER			(0x00000001)
#define FLG_H264_SYNC_ENC_CH0				(0x00000002)
#define FLG_H264_SYNC_ENC_CH1				(0x00000004)
#define FLG_H264_SYNC_ENC_CH2				(0x00000008)
#define FLG_H264_SYNC_DEC					(0x00000010)
/* Sync Stop Control */
#define FLG_H264_SYNC_STOP_ENC_CH0			(0x00000020)
#define FLG_H264_SYNC_STOP_ENC_CH1			(0x00000040)
#define FLG_H264_SYNC_STOP_ENC_CH2			(0x00000080)

#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
#define FLG_H264_SYNC_ENC_DUMMY_CH0			(0x00000100)
#define FLG_H264_SYNC_ENC_DUMMY_CH1			(0x00000200)
#define FLG_H264_SYNC_ENC_DUMMY_CH2			(0x00000400)
#endif


/* FID_RIBERY */
/* Ribery Control(internal) */
#define FLG_RIBERY_BOOT_COMP				(0x00000001)
#define FLG_RIBERY_BOOT2_COMP				(0x00000002)
#define FLG_RIBERY_CODEC_START_COMP			(0x00000004)
#define FLG_RIBERY_CODEC_START2_COMP		(0x00000008)
#define FLG_RIBERY_CODEC_STOP_COMP			(0x00000010)
#define FLG_RIBERY_CODEC_STOP2_COMP			(0x00000020)
#define FLG_RIBERY_CODEC_FLASH_COMP			(0x00000040)
#define FLG_RIBERY_VSTRM_COMP				(0x00000080)
#define FLG_RIBERY_Y_VEC_COMP				(0x00000100)
#define FLG_RIBERY_VDPB_COMP				(0x00000200)
#define FLG_RIBERY_WDT						(0x00000400)
#define FLG_RIBERY_FORCE_STOP				(0x00000800)
#define FLG_RIBERY_FIFO_VRAW_ENCREQ_SET		(0x00001000)
#define FLG_RIBERY_FIFO_VSTRM_FREE_SET		(0x00002000)
#define FLG_RIBERY_FIFO_VRAW_FREE_GET		(0x00004000)
#define FLG_RIBERY_FIFO_VSTRM_ENCDONE_GET	(0x00008000)
#define FLG_RIBERY_FIFO_VCPB_DECREQ_SET		(0x00010000)
#define FLG_RIBERY_FIFO_VDPB_FREE_SET		(0x00020000)
#define FLG_RIBERY_FIFO_VCPB_FREE_GET		(0x00040000)
#define FLG_RIBERY_FIFO_VDPB_DECDONE_GET	(0x00080000)


/* FID_H264_DECODER */
#define FLG_H264_DECODER_DISP_START			(0x00000001)
#define FLG_H264_DECODER_DISP_PAUSE_REQUEST	(0x00000002)
#define FLG_H264_DECODER_DISP_LCD_PAUSE		(0x00000004)
#define FLG_H264_DECODER_DISP_HDMI_PAUSE	(0x00000008)
#define FLG_H264_DECODER_DISP_IDX_SET		(0x00000010)
#define FLG_H264_DECODER_DISP_UPDATE		(0x00000020)
#define FLG_H264_DECODER_DISP_FRAME_DONE	(0x00000040)

//----------------------
// Task Priority
//----------------------
/* Record */
#define H264_TASK_PRI_ENC_DISP			(24)		// View task (dummy)
#define H264_TASK_PRI_ENC_CC			(25)		// CC    task
#define H264_TASK_PRI_ENC_ENT			(33)		// ENT   task
#define H264_TASK_PRI_ENC_MAIN			(34)		// Main  task
#define H264_TASK_PRI_ENC_VECTOR		(33)		// Vector task

/* Play */
#define H264_TASK_PRI_DEC_DISP_LCD		(23)		// Disp LCD task
#define H264_TASK_PRI_DEC_DISP_HDMI		(23)		// Disp HDMI task
#define H264_TASK_PRI_DEC_DISP			(24)		// Disp  task
#define H264_TASK_PRI_DEC_CC			(25)		// CC    task
#define H264_TASK_PRI_DEC_ENT			(26)		// ENT   task
#define H264_TASK_PRI_DEC_MAIN			(27)		// Main  task


//----------------------
// User Interface
//----------------------
/* Error Definition */
//		H264_OK							( 0)		// h264_if.h
//		H264_NG							(-1)		// h264_if.h
//		H264_PARAM_ERR					(-2)		// h264_if.h
//		H264_FATAL_ERR					(-3)		// h264_if.h
#define H264_OK_USER_REQ				( 1)		// User Request
#define H264_OK_MAX_TIME_END			( 2)		// Normal End(MaxTime)
#define H264_OK_COMPLETE				( 3)		// Complete End
#define H264_OK_STAP_OVER				( 4)		// Normal End(Start Point Over)
#define H264_OK_NORMAL_END				( 5)		// Normal End(User Request)
#define H264_ERR_INPUT_PARAM			(10)		// Input Parameter Error
#define H264_ERR_MEDIA_AREA_NOTHING		(11)		// Media Vacant Area Nothing
#define H264_ERR_FILE_CREATE			(12)		// Create File Error
#define H264_ERR_FILE_SIZE				(13)		// File Size Error
#define H264_ERR_MACRO					(14)		// Macro Error
#define H264_ERR_FILE_WRITE_TO			(15)		// FS_write Time Out
#define H264_ERR_FILE_WRITE				(16)		// FS_write Error
#define H264_ERR_FILE_ACCESS			(17)		// File Access Error
#define H264_ERR_FILE_NO_SUPPORT		(18)		// Not Support File Error
#define H264_ERR_FILE_READ				(19)		// Read Error
#define H264_ERR_FILE_BUFF_OVER			(20)		// FS buffer over Error
#define H264_ERR_FILE_STREAM			(21)		// Stream Error
#define H264_ERR_AUDIO_PAUSE			(22)		// MUX Error (Audio Pause)
#define H264_ERR_TTR_SIZE_OVER			(23)		// TTR size over
#define H264_ERR_MEDIA_CHANGE			(24)		// Media is changed
#define H264_ERR_SYSTEM					(99)		// System Error

/* ON/OFF */
//#define H264_OFF						(0)			// h264_if.h
//#define H264_ON						(1)			// h264_if.h

/* START/END */
#define H264_START						(0)
#define H264_END						(1)

/* Execution Mode */
#define H264_MODE_RECORD				(0)
#define H264_MODE_PLAY					(1)

//----------------------
// Video (Ribery)
//----------------------
#define H264_MODE_ENCODE				(0)
#define H264_MODE_DECODE				(1)
#define H264_MODE_TRANSCODE				(2)

/* Core Mode */
#define H264_CORE_MODE_1				(1)
#define H264_CORE_MODE_2				(2)

/* PTS base value(360KHz base) */
#define H264_PTS_BASE					(360000)	// 360 KHz

/* PTS base value(90KHz base) */
#define H264_PTS_90K_BASE				(90000)		// 90KHz

/* VOP Time Increment Resolution */
#define H264_TIMERESO_239_76			(240000)	// 239.76 fps
#define H264_TIMERESO_119_88			(120000)	// 119.88 fps
#define H264_TIMERESO_59_94				( 60000)	// 59.94 fps
#define H264_TIMERESO_47_95				( 48000)	// 47.95 fps
#define H264_TIMERESO_29_97				( 30000)	// 29.97 fps
#define H264_TIMERESO_23_97				( 24000)	// 23.97 fps
#define H264_TIMERESO_14_98				( 15000)	// 14.98 fps
#define H264_TIMERESO_9_99				( 10000)	//  9.99 fps
#define H264_TIMERESO_4_99				(  5000)	//  4.99 fps
#define H264_TIMERESO_2_99				(  3000)	//  2.99 fps
#define H264_TIMERESO_1_99				(  2000)	//  1.99 fps
#define H264_TIMERESO_0_99				(  1000)	//  0.99 fps

#define H264_TIMERESO_240_0				(240)	// 240 fps
#define H264_TIMERESO_120_0				(120)	// 120 fps
#define H264_TIMERESO_60_0				( 60)	// 60 fps
#define H264_TIMERESO_50_0				( 50)	// 50 fps
#define H264_TIMERESO_48_0				( 48)	// 48 fps
#define H264_TIMERESO_30_0				( 30)	// 30 fps
#define H264_TIMERESO_25_0				( 25)	// 25 fps
#define H264_TIMERESO_24_0				( 24)	// 24 fps
#define H264_TIMERESO_15_0				( 15)	// 15 fps
#define H264_TIMERESO_12_5				(125)	// 12.5 fps
#define H264_TIMERESO_10_0				( 10)	// 10 fps
#define H264_TIMERESO_7_5				( 15)	// 7.5 fps (15/2fps)
#define H264_TIMERESO_5_0				(  5)	//  5 fps
#define H264_TIMERESO_3_0				(  3)	//  3 fps
#define H264_TIMERESO_2_0				(  2)	//  2 fps
#define H264_TIMERESO_1_0				(  1)	//  1 fps

/* VOP Time Increment */
#define H264_TIMEINC_239_76				(1001)		// 239.76 fps
#define H264_TIMEINC_119_88				(1001)		// 119.88 fps
#define H264_TIMEINC_59_94				(1001)		// 59.94 fps
#define H264_TIMEINC_47_95				(1001)		// 47.95 fps
#define H264_TIMEINC_29_97				(1001)		// 29.97 fps (90KHz/29.97frame)
#define H264_TIMEINC_23_97				(1001)		// 23.97 fps
#define H264_TIMEINC_14_98				(1001)		// 14.98 fps
#define H264_TIMEINC_9_99				(1001)		//  9.99 fps
#define H264_TIMEINC_4_99				(1001)		//  4.99 fps
#define H264_TIMEINC_2_99				(1001)		//  2.99 fps
#define H264_TIMEINC_1_99				(1001)		//  1.99 fps
#define H264_TIMEINC_0_99				(1001)		//  0.99 fps

#define H264_TIMEINC_240_0				(1)			// 240 fps
#define H264_TIMEINC_120_0				(1)			// 120 fps
#define H264_TIMEINC_60_0				(1)			// 60 fps
#define H264_TIMEINC_50_0				(1)			// 50 fps
#define H264_TIMEINC_48_0				(1)			// 48 fps
#define H264_TIMEINC_30_0				(1)			// 30 fps
#define H264_TIMEINC_25_0				(1)			// 25 fps
#define H264_TIMEINC_24_0				(1)			// 24 fps
#define H264_TIMEINC_15_0				(1)			// 15 fps
#define H264_TIMEINC_12_5				(10)		// 12.5 fps
#define H264_TIMEINC_10_0				(1)			// 10 fps
#define H264_TIMEINC_7_5				(2)			// 7.5 fps
#define H264_TIMEINC_5_0				(1)			// 5 fps
#define H264_TIMEINC_3_0				(1)			// 3 fps
#define H264_TIMEINC_2_0				(1)			// 2 fps
#define H264_TIMEINC_1_0				(1)			// 1 fps

/* Search Range Select Mode */
#define H264_SEARCH_RANGE_SEL_MODE0		(0)
#define H264_SEARCH_RANGE_SEL_MODE1		(1)
#define H264_SEARCH_RANGE_SEL_MODE2		(2)
#define H264_SEARCH_RANGE_SEL_MODE3		(3)

/* Image Quality Mode */
#define H264_IMAGE_QUALITY_MODE0		(0)
#define H264_IMAGE_QUALITY_MODE1		(1)

//----------------------
// Print ON/OFF
//----------------------
//#define RS_printf printf
#define H264_PRINTF	RS_printf
#define H264_VA_PRINTER(arg) (H264_PRINTF arg)

#ifndef CO_H264_INFO_PRINT		// "info" print
#define H264_Info_Print(arg)
#else
#if 0
#define H264_Info_Print(arg) do{\
    H264_VA_PRINTER(arg);\
} while(0)
#endif
#define H264_Info_Print(arg)	BF_Debug_Print_Information(arg)
#endif

#ifndef CO_H264_WARNING_PRINT	// "warning" print
#define H264_Warning_Print(arg)
#else
#if 0
#define H264_Warning_Print(arg) do{\
    H264_VA_PRINTER(arg);\
} while(0)
#endif
#define H264_Warning_Print(arg)	BF_Debug_Print_Warning(arg)
#endif

#ifndef CO_H264_ERROR_PRINT	// "error" print
#define H264_Err_Print(arg)
#else
#if 0
#define H264_Err_Print(arg) do{\
    H264_VA_PRINTER(arg);\
} while(0)
#endif
#define H264_Err_Print(arg)	BF_Debug_Print_Error(arg)
#endif


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* User Event for Playback */
typedef enum{
	E_H264_EVENT_NONE			= 0,	// No Event
	E_H264_EVENT_PLAY			= 1,	// Play
	E_H264_EVENT_PAUSE			= 2,	// Pause
	E_H264_EVENT_REPLAY			= 3,	// Replay
	E_H264_EVENT_FLUSH			= 4,	// Flush
	E_H264_EVENT_STOP			= 5,	// Stop
} E_H264_EVENT;

/* Playing Status */
typedef enum{
	E_H264_PLAY_MODE_FLUSH		= 0,	// FLUSH
	E_H264_PLAY_MODE_PLAY		= 1,	// PLAY
	E_H264_PLAY_MODE_PAUSE		= 2,	// PAUSE
	E_H264_PLAY_MODE_RESUME		= 3,	// RESUME
	E_H264_PLAY_MODE_STOP		= 4,	// STOP
	E_H264_PLAY_MODE_END		= 5,	// Play End
	E_H264_PLAY_MODE_MAX		= 6		// Max.Value of Movie Play Status
} E_H264_PLAY_MODE;

/* IIP */
typedef enum{
	E_H264_RESIZE_MODE_NEAREST			= 0,
	E_H264_RESIZE_MODE_BILINEAR			= 1,
	E_H264_RESIZE_MODE_BICUBIC			= 2,
} E_H264_RESIZE_MODE;

// Reset Request to Ribery
typedef enum {
	E_H264_TOP_RIBERY_RESET_ON = 0,
	E_H264_TOP_RIBERY_RESET_OFF
} E_H264_TOP_RIBERY_RESET;

// Clock Stop to Rebery
typedef enum {
	E_H264_TOP_RIBERY_CLOCK_STOP_ON = 0,
	E_H264_TOP_RIBERY_CLOCK_STOP_OFF
} E_H264_TOP_RIBERY_CLOCK_STOP;

// Clock Setting to Sdramc Ribery
typedef enum {
	E_H264_SDRAMC_RIBERY_CLOCK_CH_0 = 0,				/**< Channel 0.				*/
	E_H264_SDRAMC_RIBERY_CLOCK_CH_1,					/**< Channel 1.				*/
} E_H264_SDRAMC_RIBERY_CLOCK_CH;

typedef enum {
	E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB0 = 0,			/**< H.264(RB0).			*/
	E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB1,				/**< H.264(RB1).			*/
	E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB2,				/**< H.264(RB2).			*/
} E_H264_SDRAMC_RIBERY_CLOCK_PORT;

typedef enum {
	E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB0 = 0,			/**< Data buffer 10(RB0).			*/
	E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB1,				/**< Data buffer 11(RB1).			*/
	E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB2,				/**< Data buffer 12(RB2).			*/
} E_H264_SDRAMC_RIBERY_CLOCK_DATABUF;

typedef enum {
	E_H264_SDRAMC_RIBERY_CLOCK_EN_DISABLE = 0,			/**< disable.				*/
	E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE				/**< enable.				*/
} E_H264_SDRAMC_RIBERY_CLOCK_EN;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct{
/* --- Video Part */
	T_H264_IF_PARAM_RECORD	codec_param[E_H264_IF_STREAM_CH_MAX];
	UINT32					scan_mode;					// progressive/interlace
} T_H264_MOVIE_UI;	// ** It requires scrutiny. **

typedef struct{
	UINT32	y_address;
	UINT32	c_address;
} T_H264_MOVIE_ADDR;


/* --- Movie Rec Data --- */
typedef struct {
	ULONG					src_addr[2];	// src address
	ULONG 					dst_addr[2];	// dst address
	USHORT					g_src_width;	// global src width
	USHORT					g_src_lines;	// global src lines
	USHORT					src_width;		// src width
	USHORT					src_lines;		// src lines
	USHORT					g_dst_width;	// global dst width
	USHORT					g_dst_lines;	// global dst lines
	USHORT					dst_width;		// dst width
	USHORT					dst_lines;		// dst lines
	USHORT					ribery_mode;	// Ribery Mode
	E_H264_RESIZE_MODE		resize_mode;	// resize mode
	E_H264_IF_DISP_ROTATE	rotate;			// rotate(clockwise)
} T_H264_MOVIE_REC_TBL;

/* --- Bit Stream Control --- */
typedef struct{
	UCHAR* pointer;
	UCHAR bit_remain;
} T_H264_MOVIE_STREAM_CONTROL;

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
// A. H264 Control
extern	VOID	H264_CC_Process( INT32 stacd );
extern	VOID	H264_ENT_Process( INT32 stacd );
extern	VOID	H264_VECTOR_Process( INT32 stacd );
extern	VOID	H264_DISP_Process( INT32 stacd );

// C. H264 Decode
extern	INT32	H264_Movie_Play_Top_Get( T_H264_IF_DECODER_PARAM* decoder_param, ULLONG req_pts, UINT32 store_num, T_H264_IF_PARAM_SET_STREAM* video_stream_param, T_H264_IF_VIDEO_YUV_INFO* decode_yuv_info );
extern	INT32	H264_Movie_Play_Top_Set( T_H264_IF_DECODER_PARAM* decoder_param, T_H264_IF_DISPLAY_INFO* display_info );
extern	INT32	H264_Movie_Play_Start_Init( VOID );
extern	INT32	H264_Movie_Play_Start( ULLONG start_pts, E_H264_IF_PLAY_DIRECTION direction, E_H264_IF_PLAY_SPEED speed, BOOL pause );
extern	E_H264_PLAY_MODE	H264_Movie_Play_Event_Check( E_H264_PLAY_MODE current_mode );

// D. H264 Common Function
extern	INT32	H264_Movie_Convert_Framerate( UINT32 framerate, ULONG *vop_time_increment, ULONG *vop_time_inc_resol );
extern	INT32	H264_Movie_Convert_Endian( VOID *input_data, ULONG convert_size );
extern	INT32	H264_Movie_Rec_Resize( T_H264_MOVIE_REC_TBL* rec_tbl );
extern	VOID	H264_Movie_Lcd_Update( T_H264_IF_DISPLAY_PARAM* lcd_info, T_H264_MOVIE_ADDR* disp_addr, BOOL ribery_format );
extern	VOID	H264_Movie_Lcd_Wait( VOID );
extern	VOID	H264_Movie_Tv_Update( T_H264_MOVIE_ADDR disp_addr, UCHAR* first_disp_flg, UCHAR color_matrix, UCHAR ribery_format );
extern	VOID	H264_Movie_Tv_Wait( VOID );
extern	VOID	H264_Movie_Hdmi_Update( T_H264_IF_DISPLAY_PARAM* hdmi_info, T_H264_MOVIE_ADDR* disp_addr, BOOL ribery_format );
extern	VOID	H264_Movie_Hdmi_Wait( VOID );

// H. DD Wrapper: Ribery
extern	VOID	H264_Movie_Ribery_Reset( E_H264_TOP_RIBERY_RESET reset );
extern	VOID	H264_Movie_Ribery_Core_Mode_Set( UINT32 core_mode );
extern	VOID	H264_Movie_Ribery_Core_Mode_Get( UINT32* core_mode );
extern	VOID	H264_Movie_Ribery_PMU_Setting( INT32 pmu );
extern	VOID	H264_Movie_Ribery_Clock_Set( INT32 clock );
extern	VOID	H264_Movie_Ribery_Clock_Stop( E_H264_TOP_RIBERY_CLOCK_STOP flag );
extern	VOID	H264_Movie_Ribery_AHClock_Start( UCHAR* ahclk_count );
extern	VOID	H264_Movie_Ribery_AHClock_Stop( UCHAR* ahclk_count );
extern	ULONG	H264_Movie_Sdram_Ch_Check( ULONG check_address );
extern	VOID	H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH ch, E_H264_SDRAMC_RIBERY_CLOCK_PORT port, E_H264_SDRAMC_RIBERY_CLOCK_EN enable );
extern	VOID	H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH ch, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF dbuf, E_H264_SDRAMC_RIBERY_CLOCK_EN enable );
extern	VOID	H264_Movie_Arm_Ribery_Dsb_Poc( VOID );

// N. Standard Library Wrapper
extern	VOID	H264_Movie_Memset( VOID* ptr, UINT32 value,  ULONG size );
extern	VOID	H264_Movie_Memcpy( VOID* ptr1, VOID* ptr2,  ULONG size );
extern	VOID*	H264_Movie_Malloc( ULONG size );
extern	VOID	H264_Movie_Free( VOID* ptr );

// O. Bit Stream Control
extern	VOID	H264_Movie_Bs_Set_Pointer_Address( T_H264_MOVIE_STREAM_CONTROL* bit_stream, UCHAR* pointer_address );
extern	VOID	H264_Movie_Bs_Skip_Pointer_Address( T_H264_MOVIE_STREAM_CONTROL* bit_stream, ULONG skip_bytes );
extern	VOID	H264_Movie_Bs_Skip_Pointer_Scope_Bit( T_H264_MOVIE_STREAM_CONTROL* bit_stream, ULONG skip_bits );
extern	UCHAR*	H264_Movie_Bs_Get_Pointer( T_H264_MOVIE_STREAM_CONTROL* bit_stream );
extern	UCHAR	H264_Movie_Bs_Get_Remain_Bits( T_H264_MOVIE_STREAM_CONTROL* bit_stream );
extern	ULONG	H264_Movie_Bs_Read_U_1bit( T_H264_MOVIE_STREAM_CONTROL* bit_stream );
extern	ULONG	H264_Movie_Bs_Read_U_Nbit( T_H264_MOVIE_STREAM_CONTROL* bit_stream, UCHAR read_num );
extern	INT32	H264_Movie_Bs_Read_U_E( T_H264_MOVIE_STREAM_CONTROL* bit_stream );
extern	INT32	H264_Movie_Bs_Read_S_E( T_H264_MOVIE_STREAM_CONTROL* bit_stream );
extern	VOID	H264_Movie_Bs_Write_U_1bit( T_H264_MOVIE_STREAM_CONTROL* bit_stream, INT32 write_value );
extern	VOID	H264_Movie_Bs_Write_U_Nbit( T_H264_MOVIE_STREAM_CONTROL* bit_stream, UCHAR write_num, INT32 write_value );

// P. Flush & Clean
extern	VOID	H264_Movie_Cache_Clean( ULONG addr, ULONG size );
extern	VOID	H264_Movie_Cache_Flush( ULONG addr, ULONG size );
extern	VOID	H264_Movie_Cache_Clean_Flush( ULONG addr, ULONG size );

// Q. DI & EI
extern	INT32	H264_Movie_Ribery_DI( VOID );
extern	VOID	H264_Movie_Ribery_EI( INT32 status );
extern	VOID	H264_Movie_SGDE_Resize( T_H264_MOVIE_REC_TBL* rec_tbl );
#endif
