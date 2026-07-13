/**
 * @file		h265_decode.h
 * @brief		H.265 Decode function Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _H265_DECODE_H_
#define _H265_DECODE_H_

#include "movie_common.h"
#include "h265_movie.h"
#include "h265_sirius.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_H265_DEC_SAVE_SAMPLE_MAX_NUM		(500)	/* ** tentative value */

// Number of VOP to Lch-Decode
#define D_H265_DEC_VOP_TOP_NUM				(1)
#define D_H265_DEC_VOP_MAX_NUM				(16)
#define D_H265_DEC_VOP_TRICK_NUM			(2)

// Stop Request
#define D_H265_DEC_REQ_NONE					(0)
#define D_H265_DEC_REQ_STOP					(1)
#define D_H265_DEC_REQ_FORCE_STOP			(2)
#define D_H265_DEC_REQ_TTR_END_POINT		(3)

// End Cause
#define D_H265_DEC_END_CAUSE_NONE			(0)
#define D_H265_DEC_END_CAUSE_STREAM_ERR		(1)
#define D_H265_DEC_END_CAUSE_FATAL_ERR		(2)


// Kind of Video Chunk
#define D_H265_DEC_IP_VOP					(0)
#define D_H265_DEC_I_VOP					(1)

// Video Chunk for Sync Flag
#define D_H265_DEC_SYNC_OFF					(0)
#define D_H265_DEC_SYNC_ON					(1)

// User Data Size
#define D_H265_DEC_USER_DATA_SIZE			(6)

// semaphore wait time
#define D_H265_DEC_SEMAPHORE_TIMEOUT		(5000)

// VDPB Num
#define D_H265_DEC_VDPB_MAX_NUM				(135)
#define D_H265_DEC_VDPB_MIN_NUM				(9)



/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum{
	E_H265_DEC_PLAY_START_TRICK_NORMAL		= 0,
	E_H265_DEC_PLAY_START_TRICK_FF			= 1,
	E_H265_DEC_PLAY_START_TRICK_REW			= 2,
	E_H265_DEC_PLAY_START_TRICK_SLOW_FWD	= 3,
	E_H265_DEC_PLAY_START_TRICK_SLOW_RWD	= 4,
} E_H265_DEC_PLAY_START_TRICK;

typedef enum{
	E_H265_DEC_FRAME_PLAY_MODE_NONE			= 0,	// No frame play
	E_H265_DEC_FRAME_PLAY_MODE_NFRM			= 1,	// Forward: Next frame sending
	E_H265_DEC_FRAME_PLAY_MODE_PFRM			= 2,	// Backward: Previous frame returning
	E_H265_DEC_FRAME_PLAY_MODE_NFRM_SYNC	= 3,	// Forward: Next sync frame sending
	E_H265_DEC_FRAME_PLAY_MODE_PFRM_SYNC	= 4,	// Backward: Previous sync frame returning
} E_H265_DEC_FRAME_PLAY_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Play Control Data */
typedef struct {
	E_H265_PLAY_MODE			mode;				// PLAY Mode
	E_H265_PLAY_MODE			next_mode;			// PLAY Mode (Next)
	E_H265_IF_PLAY_DIRECTION	direction;			// Play direction
	E_H265_IF_PLAY_SPEED		speed;				// Play speed
	BOOL						trick_limit;		// Trick limit mode.
	BOOL						pause;				// Play pause
	BOOL						iframe_only;		// I frame only
	UINT8						task_result;		// Result of Task Control Process
	BOOL						error;				// Error is occurred?
	ULLONG						last_display_pts;
	ULLONG						last_stream_pts;
	BOOL						last_stream_dec_done;
	UINT32						bank_y_width;
	UINT32						bank_y_lines;
	UINT32						bank_c_lines;
	UINT32						bank_y_size;
	UINT32						bank_c_size;
} T_H265_DEC_CTL;

typedef struct {
	UINT8				stream_end_flag;	/**< Last stream = 1, Otherwise = 0. <br> */
	UINT8				stss;				/**< It exists in MP4 of stss atom = 1, Not exist = 0. <br> */
	UINT32				stream_size;		/**< Stream size. <br> */
	UINT32				stream_address;		/**< Stored stream address. <br> */
	ULLONG				pts;				/**< Presentation Time Stamp (ex. 1001) <br> */
	ULLONG				dts;				/**< Decoding Time Stamp (ex. 1001) <br> */
} T_H265_DEC_SAMPLE;

typedef struct {
	BOOL				stream_end;
	T_H265_DEC_SAMPLE	decode_video[D_H265_DEC_SAVE_SAMPLE_MAX_NUM];
	UINT32				read_pointer;
	UINT32				write_pointer;
	UINT32				free_pointer;
} T_H265_DEC_DMUX_CTL;



typedef struct{
	BOOL				use_flg;			// Use flag (as Decoder use)
	BOOL				release_flg;		// Release flag
	ULLONG				PTS;				// PTS: Picture Time Stamp
	ULLONG				DTS;				// DTS: Decoder Time Stamp
} T_H265_DEC_DISP_INF;

typedef struct{
	UINT8	lcd_decide;
	UINT8	lcd_displaying;
	UINT8	hdmi_decide;
	UINT8	hdmi_displaying;
} T_H265_DEC_DISP_REFER_INDEX;

typedef struct{
	T_H265_IF_DISPLAY_INFO		device_info;
	T_H265_DEC_DISP_REFER_INDEX	refer_index;
	BOOL						last_picture_keep;
	ULLONG						display_target_pts;		// display target pts
	ULLONG						display_pts;			// current pts
	ULLONG						display_dts;			// current dts
	UINT32						display_index;			// (YC area bank) display index
	UINT32						use_count;				// (YC area bank) use count
	UINT32						buf_count;				// (YC area bank) image data buffering count
	UINT32						vdpb_use_num;
	BOOL						last_pts_display;
	T_H265_DEC_DISP_INF			inf[D_H265_DEC_VDPB_MAX_NUM];
} T_H265_DEC_DISP_CTL;

/* Time Trimming Function */
typedef struct {
	ULONG				start_dt;			/* (1)Time Trimming Start DT */
	ULONG				end_dt;				/* (1)Time Trimming End DT */
	UINT32				media_vc_size;		/* (1)MEDIA Vacant Size */
	ULONG				video_total_dur;	/* (3)total duration - during file creation */
	ULONG				audio_start_dt;		/* (3)Audio Start Duration */
	ULONG				ttr_max_time;		/* (1)Trimming Total Time */
	ULONG				ttr_start_time;		/* (1)Trimming Start Time */
	ULONG				video_time;			/* (3)Video Time - during file creation */
	ULONG				audio_time;			/* (3)Audio Time - during file creation */
	UCHAR				dummy1;
	USHORT				dummy2;
	ULONG				udta_atom_size;
	ULONG				free_atom_size;
	ULONG				size_copyed;
	ULONG				size_total;
} T_H265_DEC_TTR_CTL;

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern	VOID	H265_Decode_ENT_Main( VOID );
extern	VOID	H265_Decode_CC_Main( VOID );
extern	VOID	H265_Decode_DISP_Main( VOID );
extern	VOID	H265_Decode_DISP_LCD_Main( VOID );
extern	VOID	H265_Decode_DISP_HDMI_Main( VOID );
extern	INT32	H265_Decode_Main( VOID );
extern	INT32	H265_Decode_Play_Start( ULLONG start_pts, E_H265_IF_PLAY_DIRECTION direction, E_H265_IF_PLAY_SPEED speed, BOOL pause );
extern	INT32	H265_Decode_Play_Initialize( VOID );
extern	INT32	H265_Decode_Play_Release( VOID );
extern	INT32	H265_Decode_Play_Delete( UCHAR play_index );
extern	INT32	H265_Decode_Top_Init( T_H265_IF_DECODER_PARAM* decoder_param, T_H265_IF_DISPLAY_INFO* display_info );
extern	INT32	H265_Decode_Top_Main( ULLONG req_pts, T_H265_IF_VIDEO_YUV_INFO* decode_yuv_info );
extern	INT32	H265_Decode_Video_Stream_Set( UINT32 store_num, T_H265_IF_PARAM_SET_STREAM* video_stream_param );
extern	INT32	H265_Decode_Video_Display_Frame_Get( T_H265_IF_VIDEO_YUV_INFO* display_yuv_info );
extern	VOID	H265_Decode_Stop_Request( BOOL last_picture_keep );
extern	VOID	H265_Decode_Flush_Request( VOID );
extern  BOOL	H265_IsLastPTSSet_Or_Error(void);
extern	BOOL	H265_IsLastPTSDisplay_Or_Error(void);
extern	ULLONG	H265_GetLastPTS(void);

#endif
