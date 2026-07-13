/**
 * @file		h265_movie.h
 * @brief		H.265 Movie Function Control Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _H265_MOVIE_H_
#define _H265_MOVIE_H_

#include "h265_if.h"
#include "h265_if_common.h"
#include "debug.h"
#include "fs_if.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//----------------------
// Event Flag
//----------------------
/* FID_H265 */
/* Common Control */
#define FLG_H265_START						(0x00000001)
#define FLG_H265_STOP						(0x00000002)
#define FLG_H265_FLUSH_OPS					(0x00000004)
#define FLG_H265_WDT						(0x00000008)
#define FLG_H265_FORCE_STOP					(0x00000010)
#define FLG_H265_ENC_FREE_ALL_VSTREAM		(0x00000020)
#define FLG_H265_ENC_VEB_INFO_SET			(0x00000040)

#define FLG_H265_STOP_ENT_TSK				(0x00000100)
#define FLG_H265_STOP_ENT_TSK_CC2			(0x00000200)
#define FLG_H265_DEC_STREAM_RECEIVE			(0x00000400)

/* SIRIUS Control */
//#define FLG_H265_BOOT_COMP				(0x00000800)
//#define FLG_H265_CODEC_START_COMP0		(0x00001000)
//#define FLG_H265_CODEC_START_COMP1		(0x00002000)
#define FLG_H265_CODEC_STOP_COMP0			(0x00004000)
#define FLG_H265_CODEC_STOP_COMP1			(0x00008000)
#define FLG_H265_CODEC_FLUSH_COMP			(0x00010000)
#define FLG_H265_ENC_VSLICE_COMP0			(0x00020000)
#define FLG_H265_ENC_VSLICE_COMP1			(0x00040000)
#define FLG_H265_ENC_VFRAME_COMP0_TO_CC		(0x00080000)
#define FLG_H265_ENC_VFRAME_COMP0_TO_ENT	(0x00100000)
#define FLG_H265_ENC_VFRAME_COMP1_TO_CC		(0x00200000)
#define FLG_H265_ENC_VFRAME_COMP1_TO_ENT	(0x00400000)
#define FLG_H265_ENC_VSTRM_COMP0			(0x00800000)
#define FLG_H265_ENC_VSTRM_COMP1			(0x01000000)
#define FLG_H265_DEC_FRAMEEND_COMP			(0x02000000)
#define FLG_H265_DEC_ERROR					(0x04000000)
#define FLG_H265_DEC_STREAM_INPUT			(0x08000000)


/* FID_H265_TASK */
#define FLG_H265_TASK_ENT_INT_INIT_END		(0x00000001)
#define FLG_H265_TASK_ENT_INT_STOP			(0x00000002)
#define FLG_H265_TASK_ENT_INIT_END			(0x00000100)
#define FLG_H265_TASK_ENT_STATE_CHANGE		(0x00000200)
#define FLG_H265_TASK_ENT_STOP				(0x00000400)
#define FLG_H265_TASK_CC_INIT_END			(0x00000800)
#define FLG_H265_TASK_CC_STATE_CHANGE		(0x00001000)
#define FLG_H265_TASK_CC_STOP				(0x00002000)
#define FLG_H265_TASK_CC_2_INIT_END			(0x00004000)
#define FLG_H265_TASK_CC_2_STATE_CHANGE		(0x00008000)
#define FLG_H265_TASK_CC_2_STOP				(0x00010000)
#define FLG_H265_TASK_DISP_INIT_END			(0x00020000)
#define FLG_H265_TASK_DISP_STATE_CHANGE		(0x00040000)
#define FLG_H265_TASK_DISP_STOP				(0x00080000)
#define FLG_H265_TASK_DISP_LCD_INIT_END		(0x00100000)
#define FLG_H265_TASK_DISP_LCD_STOP			(0x00200000)
#define FLG_H265_TASK_DISP_HDMI_INIT_END	(0x00400000)
#define FLG_H265_TASK_DISP_HDMI_STOP		(0x00800000)
#define FLG_H265_TASK_CONTROLLER_STOP		(0x01000000)


/* FID_H265_DECODER */
#define FLG_H265_DECODER_DISP_START			(0x00000001)
#define FLG_H265_DECODER_DISP_PAUSE_REQUEST	(0x00000002)
#define FLG_H265_DECODER_DISP_LCD_PAUSE		(0x00000004)
#define FLG_H265_DECODER_DISP_HDMI_PAUSE	(0x00000008)
#define FLG_H265_DECODER_DISP_IDX_SET		(0x00000010)
#define FLG_H265_DECODER_DISP_UPDATE		(0x00000020)
#define FLG_H265_DECODER_DISP_FRAME_DONE	(0x00000040)


/* FID_H265_SYNC */
/* Sync Control */
#define FLG_H265_SYNC_ENC_MASTER			(0x00000001)
#define FLG_H265_SYNC_ENC_MASTER_2			(0x00000002)
#define FLG_H265_SYNC_ENC_CH0				(0x00000004)
#define FLG_H265_SYNC_ENC_CH1				(0x00000008)
#define FLG_H265_SYNC_DEC					(0x00000010)
/* Sync Stop Control */
#define FLG_H265_SYNC_STOP_ENC_CH0			(0x00000020)
#define FLG_H265_SYNC_STOP_ENC_CH1			(0x00000040)

#ifdef CO_H265_ENC_DEBUG_VIEW_DUMMY
#define FLG_H265_SYNC_ENC_DUMMY_CH0			(0x00000100)
#define FLG_H265_SYNC_ENC_DUMMY_CH1			(0x00000200)
#endif

/* SIRIUS Control(internal) */
#define FLG_SIRIUS_SYSTEM_BOOT_COMP			(0x00000001)
#define FLG_SIRIUS_VIDEO_BOOT_COMP			(0x00000002)
#define FLG_SIRIUS_CODEC_ENC_START_COMP0	(0x00000004)
#define FLG_SIRIUS_CODEC_ENC_START_COMP1	(0x00000008)
//#define FLG_SIRIUS_CODEC_ENC_STOP_COMP0	(0x00000010)
//#define FLG_SIRIUS_CODEC_ENC_STOP_COMP1	(0x00000020)
#define FLG_SIRIUS_CODEC_DEC_START_COMP		(0x00000040)
#define FLG_SIRIUS_CODEC_DEC_STOP_COMP		(0x00000080)

//----------------------
// Task Priority
//----------------------
/* Record */
#define H265_TASK_PRI_ENC_DISP			(24)		// View task (dummy)
#define H265_TASK_PRI_ENC_CC			(25)		// CC    task
#define H265_TASK_PRI_ENC_CC_2			(25)		// CC2   task
#define H265_TASK_PRI_ENC_ENT			(33)		// ENT   task
#define H265_TASK_PRI_ENC_ENT_INT		(9)			// ENT INT  task
#define H265_TASK_PRI_ENC_MAIN			(34)		// Main  task

/* Play */
#define H265_TASK_PRI_DEC_DISP_LCD		(23)		// Disp LCD task
#define H265_TASK_PRI_DEC_DISP_HDMI		(23)		// Disp HDMI task
#define H265_TASK_PRI_DEC_DISP			(24)		// Disp  task
#define H265_TASK_PRI_DEC_CC			(25)		// CC    task
#define H265_TASK_PRI_DEC_ENT			(26)		// ENT   task
#define H265_TASK_PRI_DEC_MAIN			(27)		// Main  task


//----------------------
// User Interface
//----------------------
/* Error Definition */
//		H265_OK							( 0)		// h265_if.h
//		H265_NG							(-1)		// h265_if.h
//		H265_PARAM_ERR					(-2)		// h265_if.h
//		H265_FATAL_ERR					(-3)		// h265_if.h
#define H265_OK_USER_REQ				( 1)		// User Request
#define H265_OK_MAX_TIME_END			( 2)		// Normal End(MaxTime)
#define H265_OK_COMPLETE				( 3)		// Complete End
#define H265_OK_STAP_OVER				( 4)		// Normal End(Start Point Over)
#define H265_OK_NORMAL_END				( 5)		// Normal End(User Request)
#define H265_ERR_INPUT_PARAM			(10)		// Input Parameter Error
#define H265_ERR_MEDIA_AREA_NOTHING		(11)		// Media Vacant Area Nothing
#define H265_ERR_FILE_CREATE			(12)		// Create File Error
#define H265_ERR_FILE_SIZE				(13)		// File Size Error
#define H265_ERR_MACRO					(14)		// Macro Error
#define H265_ERR_FILE_WRITE_TO			(15)		// FS_write Time Out
#define H265_ERR_FILE_WRITE				(16)		// FS_write Error
#define H265_ERR_FILE_ACCESS			(17)		// File Access Error
#define H265_ERR_FILE_NO_SUPPORT		(18)		// Not Support File Error
#define H265_ERR_FILE_READ				(19)		// Read Error
#define H265_ERR_FILE_BUFF_OVER			(20)		// FS buffer over Error
#define H265_ERR_FILE_STREAM			(21)		// Stream Error
#define H265_ERR_AUDIO_PAUSE			(22)		// MUX Error (Audio Pause)
#define H265_ERR_TTR_SIZE_OVER			(23)		// TTR size over
#define H265_ERR_MEDIA_CHANGE			(24)		// Media is changed
#define H265_ERR_SYSTEM					(99)		// System Error

/* ON/OFF */
//#define H265_OFF						(0)			// h265_if.h
//#define H265_ON						(1)			// h265_if.h

/* START/END */
#define H265_START						(0)
#define H265_END						(1)

/* Execution Mode */
#define H265_MODE_RECORD				(0)
#define H265_MODE_PLAY					(1)
#define H265_MODE_TRANSCODE				(2)
#define H265_MODE_COMMON				(3)

//----------------------
// Video (Ribery)
//----------------------
/* Core Mode */
#define H265_CORE_MODE_1				(1)
#define H265_CORE_MODE_2				(2)
#define H265_CORE_MODE_3				(3)

/* PTS base value(360KHz base) */
#define H265_PTS_BASE					(360000)	// 360 KHz

/* PTS base value(90KHz base) */
#define H265_PTS_90K_BASE				(90000)		// 90KHz

/* VOP Time Increment Resolution */
#define H265_TIMERESO_239_76			(240000)	// 239.76 fps
#define H265_TIMERESO_119_88			(120000)	// 119.88 fps
#define H265_TIMERESO_59_94				( 60000)	// 59.94 fps
#define H265_TIMERESO_29_97				( 30000)	// 29.97 fps
#define H265_TIMERESO_23_97				( 24000)	// 23.97 fps
#define H265_TIMERESO_14_98				( 15000)	// 14.98 fps

#define H265_TIMERESO_240_0				(240)	// 240 fps
#define H265_TIMERESO_120_0				(120)	// 120 fps
#define H265_TIMERESO_60_0				( 60)	// 60 fps
#define H265_TIMERESO_50_0				( 50)												
#define H265_TIMERESO_30_0				( 30)	// 30 fps
#define H265_TIMERESO_24_0				( 24)	// 24 fps
#define H265_TIMERESO_15_0				( 15)	// 15 fps

#define H265_TIMERESO_73_0				( 73)
#define H265_TIMERESO_72_0				( 72)
#define H265_TIMERESO_70_0				( 70)
#define H265_TIMERESO_29_0				( 29)
#define H265_TIMERESO_25_0				( 25)

/* VOP Time Increment */
#define H265_TIMEINC_239_76				(1001)		// 239.76 fps
#define H265_TIMEINC_119_88				(1001)		// 119.88 fps
#define H265_TIMEINC_59_94				(1001)		// 59.94 fps
#define H265_TIMEINC_29_97				(1001)		// 29.97 fps (90KHz/29.97frame)
#define H265_TIMEINC_23_97				(1001)		// 23.97 fps
#define H265_TIMEINC_14_98				(1001)		// 14.98 fps

#define H265_TIMEINC_240_0				(1)			// 240 fps
#define H265_TIMEINC_120_0				(1)			// 120 fps
#define H265_TIMEINC_60_0				(1)			// 60 fps
#define H265_TIMEINC_50_0				(1)
#define H265_TIMEINC_30_0				(1)			// 30 fps
#define H265_TIMEINC_24_0				(1)			// 24 fps
#define H265_TIMEINC_15_0				(1)			// 15 fps
#define H265_TIMEINC_73_0				(1)
#define H265_TIMEINC_72_0				(1)
#define H265_TIMEINC_70_0				(1)
#define H265_TIMEINC_29_0				(1)
#define H265_TIMEINC_25_0				(1)


//----------------------
// Print ON/OFF
//----------------------
//#define RS_printf printf
#define H265_PRINTF	RS_printf
#define H265_VA_PRINTER(arg) (H265_PRINTF arg)

#ifndef CO_H265_INFO_PRINT		// "info" print
#define H265_Info_Print(arg)
#else
#if 0
#define H265_Info_Print(arg) do{\
    H265_VA_PRINTER(arg);\
} while(0)
#endif
#define H265_Info_Print(arg)	BF_Debug_Print_Information(arg)
#endif

#ifndef CO_H265_WARNING_PRINT	// "warning" print
#define H265_Warning_Print(arg)
#else
#if 0
#define H265_Warning_Print(arg) do{\
    H265_VA_PRINTER(arg);\
} while(0)
#endif
#define H265_Warning_Print(arg)	BF_Debug_Print_Warning(arg)
#endif

#ifndef CO_H265_ERROR_PRINT	// "error" print
#define H265_Err_Print(arg)
#else
#if 0
#define H265_Err_Print(arg) do{\
    H265_VA_PRINTER(arg);\
} while(0)
#endif
#define H265_Err_Print(arg)	BF_Debug_Print_Error(arg)
#endif


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* User Event for Playback */
typedef enum{
	E_H265_EVENT_NONE			= 0,	// No Event
	E_H265_EVENT_PLAY			= 1,	// Play
	E_H265_EVENT_PAUSE			= 2,	// Pause
	E_H265_EVENT_REPLAY			= 3,	// Replay
	E_H265_EVENT_FLUSH			= 4,	// Flush
	E_H265_EVENT_STOP			= 5,	// Stop
} E_H265_EVENT;

/* Playing Status */
typedef enum{
	E_H265_PLAY_MODE_FLUSH		= 0,	// FLUSH
	E_H265_PLAY_MODE_PLAY		= 1,	// PLAY
	E_H265_PLAY_MODE_PAUSE		= 2,	// PAUSE
	E_H265_PLAY_MODE_RESUME		= 3,	// RESUME
	E_H265_PLAY_MODE_STOP		= 4,	// STOP
	E_H265_PLAY_MODE_END		= 5,	// Play End
	E_H265_PLAY_MODE_MAX		= 6		// Max.Value of Movie Play Status
} E_H265_PLAY_MODE;

/* IIP */
typedef enum{
	E_H265_RESIZE_MODE_NEAREST			= 0,
	E_H265_RESIZE_MODE_BILINEAR			= 1,
	E_H265_RESIZE_MODE_BICUBIC			= 2,
} E_H265_RESIZE_MODE;

// Sirius Core Block
typedef enum {
	E_H265_PMU_BLOCK_IPP = 0,
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	E_H265_PMU_BLOCK_PXF,
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif				
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	E_H265_PMU_BLOCK_WAVE,
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif				
// --- REMOVE_ES_COMPILE_OPT END ---
	E_H265_PMU_BLOCK_VDF,
} E_H265_PMU_BLOCK;

// Reset Request to Sirius
typedef enum {
	E_H265_TOP_RESET_ON = 0,
	E_H265_TOP_RESET_OFF

} E_H265_TOP_RESET;

// Clock Stop to Sirius
typedef enum {
	E_H265_TOP_CLOCK_STOP_ON = 0,
	E_H265_TOP_CLOCK_STOP_OFF
} E_H265_TOP_CLOCK_STOP;


// Setting to Sdramc Sirius
typedef enum {
	E_H265_SDRAMC_CH_0 = 0,						/**< Channel 0.				*/
	E_H265_SDRAMC_CH_1,							/**< Channel 1.				*/
} E_H265_SDRAMC_CH;

typedef enum {
	E_H265_SDRAMC_PORT_DS0 = 0,					/**< H.265 Common(DS0).		*/
	E_H265_SDRAMC_PORT_DS1,						/**< H.265 Common(DS1).		*/
	E_H265_SDRAMC_PORT_SDM,						/**< H.265 Common(SDM).		*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	E_H265_SDRAMC_PORT_VE0,						/**< H.265 Encode(VE0).		*/
	E_H265_SDRAMC_PORT_VE1,						/**< H.265 Encode(VE1).		*/
	E_H265_SDRAMC_PORT_VE2,						/**< H.265 Encode(VE2).		*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	E_H265_SDRAMC_PORT_VCOREW,					/**< H.265 Encode(VCOREW).	*/
	E_H265_SDRAMC_PORT_VCORER0,					/**< H.265 Encode(VCORER0).	*/
	E_H265_SDRAMC_PORT_VCORER1,					/**< H.265 Encode(VCORER1).	*/
	E_H265_SDRAMC_PORT_VCPU,					/**< H.265 Encode(VCPU).	*/
	E_H265_SDRAMC_PORT_VCOREM,					/**< H.265 Encode(VCOREM).	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	E_H265_SDRAMC_PORT_VD0,						/**< H.265 Decode(VD0).		*/
	E_H265_SDRAMC_PORT_VD1,						/**< H.265 Decode(VD1).		*/
	E_H265_SDRAMC_PORT_VD2,						/**< H.265 Decode(VD2).		*/
} E_H265_SDRAMC_PORT;

typedef enum {
	E_H265_SDRAMC_DATABUF_DS01 = 0,				/**< Data buffer 13(DS0/DS1).		*/
	E_H265_SDRAMC_DATABUF_SDM,					/**< Data buffer 14(SDM).			*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	E_H265_SDRAMC_DATABUF_VE012,				/**< Data buffer 15(VE0/VE1/VE2).	*/
	E_H265_SDRAMC_DATABUF_VD01,					/**< Data buffer 16(VD0/VD1).		*/
	E_H265_SDRAMC_DATABUF_VD2,					/**< Data buffer 17(VD2).			*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	E_H265_SDRAMC_DATABUF_VD01,					/**< Data buffer 16(VD0/VD1).		*/
	E_H265_SDRAMC_DATABUF_VCPU,					/**< Data buffer 18(VCPU).			*/
	E_H265_SDRAMC_DATABUF_VCOREWR0,				/**< Data buffer 19(VCOREW/VCORER0).*/
	E_H265_SDRAMC_DATABUF_VCORER1,				/**< Data buffer 20(VCORER1).		*/
	E_H265_SDRAMC_DATABUF_VCOREM,				/**< Data buffer 21(VCOREM).		*/
	E_H265_SDRAMC_DATABUF_VD2,					/**< Data buffer 22(VD2).			*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
} E_H265_SDRAMC_DATABUF;

typedef enum {
	E_H265_SDRAMC_CLOCK_EN_DISABLE = 0,			/**< disable.				*/
	E_H265_SDRAMC_CLOCK_EN_ENABLE				/**< enable.				*/
} E_H265_SDRAMC_CLOCK_EN;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct{
/* --- Video Part */
	T_H265_IF_PARAM_RECORD	codec_param[E_H265_IF_STREAM_CH_MAX];
	UINT32	scan_mode;					// progressive/interlace
} T_H265_MOVIE_UI;	// ** It requires scrutiny. **

typedef struct{
	UINT32	y_address;
	UINT32	c_address;
} T_H265_MOVIE_ADDR;


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
	E_H265_RESIZE_MODE		resize_mode;	// resize mode
	E_H265_IF_DISP_ROTATE	rotate;			// rotate(clockwise)
} T_H265_MOVIE_REC_TBL;

/* --- Bit Stream Control --- */
typedef struct{
	UCHAR* pointer;
	UCHAR bit_remain;
} T_H265_MOVIE_STREAM_CONTROL;

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
// A. H265 Control
extern	VOID	H265_CC_Process( INT32 stacd );
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern	VOID	H265_CC_2_Process( INT32 stacd );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
extern	VOID	H265_ENT_Process( INT32 stacd );
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern	VOID	H265_ENT_INT_Process( INT32 stacd );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif				
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
extern	VOID	H265_DISP_Process( INT32 stacd );

// C. H265 Decode
extern	INT32	H265_Movie_Play_Top_Get( T_H265_IF_DECODER_PARAM* decoder_param, ULLONG req_pts, UINT32 store_num, T_H265_IF_PARAM_SET_STREAM* video_stream_param, T_H265_IF_VIDEO_YUV_INFO* decode_yuv_info );
extern	INT32	H265_Movie_Play_Top_Set( T_H265_IF_DECODER_PARAM* decoder_param, T_H265_IF_DISPLAY_INFO* display_info );
extern	INT32	H265_Movie_Play_Start_Init( VOID );
extern	INT32	H265_Movie_Play_Start( ULLONG start_pts, E_H265_IF_PLAY_DIRECTION direction, E_H265_IF_PLAY_SPEED speed, BOOL pause );
extern	E_H265_PLAY_MODE	H265_Movie_Play_Event_Check( E_H265_PLAY_MODE current_mode );

// D. H265 Common Function
extern	INT32	H265_Movie_Convert_Framerate( UINT32 framerate, ULONG *vop_time_increment, ULONG *vop_time_inc_resol );
extern	INT32	H265_Movie_Convert_Endian( VOID *input_data, ULONG convert_size );
extern	INT32	H265_Movie_Rec_Resize( T_H265_MOVIE_REC_TBL* rec_tbl );
extern	VOID	H265_Movie_Lcd_Update( T_H265_IF_DISPLAY_PARAM* lcd_info, T_H265_MOVIE_ADDR* disp_addr, BOOL codec_format );
extern	VOID	H265_Movie_Lcd_Wait( VOID );
extern	VOID	H265_Movie_Tv_Update( T_H265_MOVIE_ADDR disp_addr, UCHAR* first_disp_flg, UCHAR color_matrix, UCHAR codec_format );
extern	VOID	H265_Movie_Tv_Wait( VOID );
extern	VOID	H265_Movie_Hdmi_Update( T_H265_IF_DISPLAY_PARAM* hdmi_info, T_H265_MOVIE_ADDR* disp_addr, BOOL codec_format );
extern	VOID	H265_Movie_Hdmi_Wait( VOID );

// H. DD Wrapper: Sirius
extern	ULONG	H265_Movie_Sirius_Phy_To_Log( ULONG addr );
extern	ULONG	H265_Movie_Sirius_Log_To_Phy( ULONG addr );
extern	VOID	H265_Movie_Sirius_IPP_ASYNC_Reset( E_H265_TOP_RESET reset );
extern	VOID	H265_Movie_Sirius_IPP_SYNC_Reset( E_H265_TOP_RESET reset );
extern	VOID	H265_Movie_Sirius_VDF_Reset( E_H265_TOP_RESET reset );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
extern	VOID	H265_Movie_Sirius_PXF_Reset( E_H265_TOP_RESET reset );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern	VOID	H265_Movie_Sirius_IPP_Clock_Stop( E_H265_TOP_CLOCK_STOP clk_stop );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
extern	VOID	H265_Movie_Sirius_PXF_Clock_Stop( E_H265_TOP_CLOCK_STOP clk_stop );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern	VOID	H265_Movie_Sirius_VDF_Clock_Stop( E_H265_TOP_CLOCK_STOP clk_stop );

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
extern	VOID	H265_Movie_HEVENC_Clock_Stop( E_H265_TOP_CLOCK_STOP clk_stop );
extern	VOID	H265_Movie_HEVENC_Reset( E_H265_TOP_RESET reset );
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---


extern	VOID	H265_Movie_PMU_Setting( E_H265_PMU_BLOCK block, INT32 pmu );
extern	VOID	H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH ch, E_H265_SDRAMC_PORT port, E_H265_SDRAMC_CLOCK_EN enable );
extern	VOID	H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH ch, E_H265_SDRAMC_DATABUF dbuf, E_H265_SDRAMC_CLOCK_EN enable );
extern	VOID	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH ch, E_H265_SDRAMC_PORT port, LONG address );


extern	ULONG	H265_Movie_Sdram_Ch_Check( ULONG check_address );
extern	VOID	H265_Movie_Arm_Sirius_Dsb_Poc( VOID );

// N. Standard Library Wrapper
extern	VOID	H265_Movie_Memset( VOID* ptr, UINT32 value,  ULONG size );
extern	VOID	H265_Movie_Memcpy( VOID* ptr1, VOID* ptr2,  ULONG size );
extern	VOID*	H265_Movie_Malloc( ULONG size );
extern	VOID	H265_Movie_Free( VOID* ptr );

// O. Bit Stream Control
extern	VOID	H265_Movie_Bs_Set_Pointer_Address( T_H265_MOVIE_STREAM_CONTROL* bit_stream, UCHAR* pointer_address );
extern	VOID	H265_Movie_Bs_Skip_Pointer_Address( T_H265_MOVIE_STREAM_CONTROL* bit_stream, ULONG skip_bytes );
extern	VOID	H265_Movie_Bs_Skip_Pointer_Scope_Bit( T_H265_MOVIE_STREAM_CONTROL* bit_stream, ULONG skip_bits );
extern	UCHAR*	H265_Movie_Bs_Get_Pointer( T_H265_MOVIE_STREAM_CONTROL* bit_stream );
extern	UCHAR	H265_Movie_Bs_Get_Remain_Bits( T_H265_MOVIE_STREAM_CONTROL* bit_stream );
extern	ULONG	H265_Movie_Bs_Read_U_1bit( T_H265_MOVIE_STREAM_CONTROL* bit_stream );
extern	ULONG	H265_Movie_Bs_Read_U_Nbit( T_H265_MOVIE_STREAM_CONTROL* bit_stream, UCHAR read_num );
extern	INT32	H265_Movie_Bs_Read_U_E( T_H265_MOVIE_STREAM_CONTROL* bit_stream );
extern	INT32	H265_Movie_Bs_Read_S_E( T_H265_MOVIE_STREAM_CONTROL* bit_stream );
extern	VOID	H265_Movie_Bs_Write_U_1bit( T_H265_MOVIE_STREAM_CONTROL* bit_stream, INT32 write_value );
extern	VOID	H265_Movie_Bs_Write_U_Nbit( T_H265_MOVIE_STREAM_CONTROL* bit_stream, UCHAR write_num, INT32 write_value );

extern	UINT32	H265_Movie_DI( VOID );
extern	VOID	H265_Movie_EI( UINT32 status );

// P. Flush & Clean
extern	VOID	H265_Movie_Cache_Clean( ULONG addr, ULONG size );
extern	VOID	H265_Movie_Cache_Flush( ULONG addr, ULONG size );
extern	VOID	H265_Movie_Cache_Clean_Flush( ULONG addr, ULONG size );
extern	VOID	H265_Movie_SGDE_Resize( T_H265_MOVIE_REC_TBL* rec_tbl );

#endif
