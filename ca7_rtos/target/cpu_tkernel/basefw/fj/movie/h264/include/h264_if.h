/**
 * @file		h264_if.h
 * @brief		H.264 movie function Host I/F header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _H264_IF_H_
#define _H264_IF_H_


// --- CO_H264_PORTING_BLOCK
#include "os_user_custom.h"
#include "ddim_typedef.h"		// "typedef" VOID,INT32,UCHAR...
#include <string.h>			// memcpy, memset...
// ---


/*----------------------------------------------------------------------*/
/* Compile Option														*/
/*----------------------------------------------------------------------*/
/* General */
//#define CO_H264_SECTION_ON				// section definition enable
#define CO_H264_INFO_PRINT				// Information print enable
#define CO_H264_WARNING_PRINT			// Warning     print enable
#define CO_H264_ERROR_PRINT				// Error       print enable

/* Debug */
//#define CO_H264_PORTING_BLOCK			// sytem porting block  (porting done = comment out)
//#define CO_H264_ENC_DEBUG_VIEW_DUMMY	// Record: "View(Disp) task" dummy

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* result */
#define H264_OK				( 0)		// OK (normal end)
#define H264_NG				(-1)		// NG (error)
#define H264_PARAM_ERR		(-2)		// parameter error
#define H264_FATAL_ERR		(-3)		// fatal error

/* ON/OFF */
#define H264_OFF			(0)
#define H264_ON				(1)

/* NAL unit size */
#define	H264_NAL_UNIT_MAX_NUM					(11)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_H264_IF_ID_RECORD		=	0x00010000,
    E_H264_IF_ID_PLAY		=	0x00020000,
} E_H264_IF_ID;


typedef enum {
	E_H264_IF_TYPE_NO_REQUEST			= 0,
	
	E_H264_IF_TYPE_RECORD_INITIALIZE	= ( E_H264_IF_ID_RECORD  | 0x00000001 ),
	E_H264_IF_TYPE_RECORD_FINALIZE		= ( E_H264_IF_ID_RECORD  | 0x00000002 ),
	E_H264_IF_TYPE_RECORD_START_VIDEO	= ( E_H264_IF_ID_RECORD  | 0x00000003 ),
	E_H264_IF_TYPE_RECORD_STOP			= ( E_H264_IF_ID_RECORD  | 0x00000004 ),
	E_H264_IF_TYPE_RECORD_LAST			= ( E_H264_IF_ID_RECORD  | 0x00000005 ),
	
	E_H264_IF_TYPE_PLAY_INITIALIZE		= ( E_H264_IF_ID_PLAY    | 0x00000001 ),
	E_H264_IF_TYPE_PLAY_FINALIZE		= ( E_H264_IF_ID_PLAY    | 0x00000002 ),
	E_H264_IF_TYPE_PLAY_START			= ( E_H264_IF_ID_PLAY    | 0x00000003 ),
	E_H264_IF_TYPE_PLAY_RESUME			= ( E_H264_IF_ID_PLAY    | 0x00000004 ),
	E_H264_IF_TYPE_PLAY_STOP			= ( E_H264_IF_ID_PLAY    | 0x00000005 ),
	E_H264_IF_TYPE_PLAY_PAUSE			= ( E_H264_IF_ID_PLAY    | 0x00000006 ),
	E_H264_IF_TYPE_PLAY_FLUSH			= ( E_H264_IF_ID_PLAY    | 0x00000007 ),
	E_H264_IF_TYPE_PLAY_TOPIMAGE		= ( E_H264_IF_ID_PLAY    | 0x00000008 ),
	E_H264_IF_TYPE_PLAY_LAST			= ( E_H264_IF_ID_PLAY    | 0x00000009 )
}E_H264_IF_TYPE;

/* Stop Nortify */
typedef enum{
	E_H264_IF_STOP_NOTIFY_NORMAL_END		= 0,	// normal end. (stop command)
	E_H264_IF_STOP_NOTIFY_VRAW_MULTI_ERR	= 1,	// Error!! (vraw multi err)
	E_H264_IF_STOP_NOTIFY_VSTRM_EMPTY_ERR	= 2,	// Error!! (vstrm empty err)
	E_H264_IF_STOP_NOTIFY_FATAL_ERR			= 3		// Error!! (fatal err)
} E_H264_IF_STOP_NOTIFY;

/* msg wait type */
typedef enum {
	E_H264_IF_MSG_WAIT_BLOCK	= 1,		// blocking wait
	E_H264_IF_MSG_WAIT_NONBLOCK	= 2			// non-blocking wait
} E_H264_IF_MSG_WAIT;


// [common]
/* Video Ribery CH (for multi encoding) */
typedef enum {
	E_H264_IF_STREAM_CH_0	= 0,			/**< H264 Stream CH0 */
	E_H264_IF_STREAM_CH_1,					/**< H264 Stream CH1 */
	E_H264_IF_STREAM_CH_2,					/**< H264 Stream CH2 */
	E_H264_IF_STREAM_CH_MAX					/**< H264 Stream CH MAX */
} E_H264_IF_STREAM_CH;

/* Video Image Size */
typedef enum{
	E_H264_IF_IMAGE_SIZE_4096_2304 = 0,		// 4096*2304
	E_H264_IF_IMAGE_SIZE_4096_2160,			// 4096*2160
	E_H264_IF_IMAGE_SIZE_4096_2048,			// 4096*2048
	E_H264_IF_IMAGE_SIZE_3840_2160,			// 3840*2160
	E_H264_IF_IMAGE_SIZE_3840_1920,			// 3840*1920
	E_H264_IF_IMAGE_SIZE_3000_3000,			// 3000*3000
	E_H264_IF_IMAGE_SIZE_2704_2028,			// 2704*2028
	E_H264_IF_IMAGE_SIZE_2704_1520,			// 2704*1520
	E_H264_IF_IMAGE_SIZE_2560_1440,			// 2560*1440
	E_H264_IF_IMAGE_SIZE_1920_1440,			// 1920*1440
	E_H264_IF_IMAGE_SIZE_1920_1080,			// 1920*1080
	E_H264_IF_IMAGE_SIZE_1920_960,			// 1920*960
	E_H264_IF_IMAGE_SIZE_1440_1080,			// 1440*1080
	E_H264_IF_IMAGE_SIZE_1440_720,			// 1440* 720
	E_H264_IF_IMAGE_SIZE_1280_1024,
	E_H264_IF_IMAGE_SIZE_1280_960,			// 1280* 960
	E_H264_IF_IMAGE_SIZE_1280_768,
	E_H264_IF_IMAGE_SIZE_1280_720,			// 1280* 720
	E_H264_IF_IMAGE_SIZE_1024_768,
	E_H264_IF_IMAGE_SIZE_960_540,
	E_H264_IF_IMAGE_SIZE_960_480,			//  960* 480
	E_H264_IF_IMAGE_SIZE_800_600,
	E_H264_IF_IMAGE_SIZE_864_480,			//  864* 480
	E_H264_IF_IMAGE_SIZE_848_480,			//  848* 480
	E_H264_IF_IMAGE_SIZE_720_576,			//  720* 576
	E_H264_IF_IMAGE_SIZE_720_480,			//  720* 480
	E_H264_IF_IMAGE_SIZE_640_480,			//  640* 480
	E_H264_IF_IMAGE_SIZE_480_270,
	E_H264_IF_IMAGE_SIZE_640_360,			//  640* 360
	E_H264_IF_IMAGE_SIZE_432_240,			//  432* 240
	E_H264_IF_IMAGE_SIZE_320_240,			//  320* 240
	E_H264_IF_IMAGE_SIZE_1600_896,
	E_H264_IF_IMAGE_SIZE_1024_576,
	E_H264_IF_IMAGE_SIZE_800_448,
	E_H264_IF_IMAGE_SIZE_352_288,
	E_H264_IF_IMAGE_SIZE_2592_1944,
	E_H264_IF_IMAGE_SIZE_2304_1296,
	E_H264_IF_IMAGE_SIZE_1920_1280,
	E_H264_IF_IMAGE_SIZE_640_512,
	E_H264_IF_IMAGE_SIZE_384_288,
	E_H264_IF_IMAGE_SIZE_MAX,				//  Stopper
} E_H264_IF_IMAGE_SIZE;


/* Stream Format */
typedef enum{
	E_H264_IF_STREAM_FORMAT_ES = 0,		// for MP4
	E_H264_IF_STREAM_FORMAT_PES,		// for TS
	E_H264_IF_STREAM_FORMAT_ES_HEADER,	// for Both
} E_H264_IF_STREAM_FORMAT;


/* Video Frame Rate */
typedef enum {
	E_H264_IF_FRAME_RATE_239_76	= 23976,	// 239.76fps
	E_H264_IF_FRAME_RATE_119_88	= 11988,	// 119.88fps
	E_H264_IF_FRAME_RATE_59_94	= 5994,		// 59.94fps
	E_H264_IF_FRAME_RATE_47_95	= 4795,		// 47.95fps
	E_H264_IF_FRAME_RATE_29_97	= 2997,		// 29.97fps
	E_H264_IF_FRAME_RATE_23_97	= 2397,		// 23.97fps
	E_H264_IF_FRAME_RATE_14_98	= 1498,		// 14.98fps
	E_H264_IF_FRAME_RATE_9_99	= 999,		// 9.99fps
	E_H264_IF_FRAME_RATE_4_99	= 499,		// 4.99fps
	E_H264_IF_FRAME_RATE_2_99	= 299,		// 2.99fps
	E_H264_IF_FRAME_RATE_1_99	= 199,		// 1.99fps
	E_H264_IF_FRAME_RATE_0_99	= 99,		// 0.99fps
	
	E_H264_IF_FRAME_RATE_240_0	= 24000,	// 240.0fps
	E_H264_IF_FRAME_RATE_120_0	= 12000,	// 120.0fps
	E_H264_IF_FRAME_RATE_60_0	= 6000,		// 60.0fps
	E_H264_IF_FRAME_RATE_50_0	= 5000,		// 50.0fps
	E_H264_IF_FRAME_RATE_48_0	= 4800,		// 48.0fps
	E_H264_IF_FRAME_RATE_30_0	= 3000,		// 30.0fps
	E_H264_IF_FRAME_RATE_25_0	= 2500,		// 25.0fps
	E_H264_IF_FRAME_RATE_24_0	= 2400,		// 24.0fps
	E_H264_IF_FRAME_RATE_15_0	= 1500,		// 15.0fps
	E_H264_IF_FRAME_RATE_12_5	= 1250,		// 12.5fps
	E_H264_IF_FRAME_RATE_10_0	= 1000,		// 10.0fps
	E_H264_IF_FRAME_RATE_7_5	= 750,		// 7.5fps
	E_H264_IF_FRAME_RATE_5_0	= 500,		// 5.0fps
	E_H264_IF_FRAME_RATE_3_0	= 300,		// 3.0fps
	E_H264_IF_FRAME_RATE_2_0	= 200,		// 2.0fps
	E_H264_IF_FRAME_RATE_1_0	= 100,		// 1.0fps
} E_H264_IF_FRAME_RATE;

// [Record]
/* Bitrate Algorithm */
typedef enum {
	E_H264_IF_RATE_MODE_CBR = 0,			// CBR
	E_H264_IF_RATE_MODE_VBR,				// VBR
	E_H264_IF_RATE_MODE_FIXED_QP,		// FIXED_QP
} E_H264_IF_RATE_MODE;

/* Profile */
typedef enum {
	E_H264_IF_PROFILE_HIGH = 0,				// High
	E_H264_IF_PROFILE_MAIN,					// Main
	E_H264_IF_PROFILE_CONSTRAINED_BASELINE, // Constrained Baseline
} E_H264_IF_PROFILE;

/* Level */
typedef enum {
	E_H264_IF_LEVEL_1_3 = 13,			// LEVEL : "1.3"
	E_H264_IF_LEVEL_2   = 20,			// LEVEL : "2"
	E_H264_IF_LEVEL_2_1 = 21,			// LEVEL : "2.1"
	E_H264_IF_LEVEL_2_2 = 22,			// LEVEL : "2.2"
	E_H264_IF_LEVEL_3   = 30,			// LEVEL : "3"
	E_H264_IF_LEVEL_3_1 = 31,			// LEVEL : "3.1"
	E_H264_IF_LEVEL_3_2 = 32,			// LEVEL : "3.2"
	E_H264_IF_LEVEL_4   = 40,			// LEVEL : "4"
	E_H264_IF_LEVEL_4_1 = 41,			// LEVEL : "4.1"
	E_H264_IF_LEVEL_4_2 = 42,			// LEVEL : "4.2"
	E_H264_IF_LEVEL_5   = 50,			// LEVEL : "5"
	E_H264_IF_LEVEL_5_1 = 51,			// LEVEL : "5.1"
	E_H264_IF_LEVEL_5_2 = 52,			// LEVEL : "5.2"
} E_H264_IF_LEVEL;

/* VLC Mode */
typedef enum {
	E_H264_IF_VLC_MODE_CABAC = 0,			// CABAC
	E_H264_IF_VLC_MODE_CAVLC,				// CAVLC
} E_H264_IF_VLC_MODE;

/* Scan Mode */
typedef enum {
	E_H264_IF_SCAN_MODE_PROGRESSIVE = 0,	// PROGRESSIVE
	E_H264_IF_SCAN_MODE_INTERLACE,			// INTERLACE
} E_H264_IF_SCAN_MODE;

/* Video Picture Type */
typedef enum {
	E_H264_IF_PIC_TYPE_IDR = 0,			// IDR Picture
	E_H264_IF_PIC_TYPE_I,				// I Picture
	E_H264_IF_PIC_TYPE_P,				// P Picture
	E_H264_IF_PIC_TYPE_B,				// B Picture
} E_H264_IF_PIC_TYPE;

/* Video Picture Structure Type */
typedef enum {
	E_H264_IF_GOP_STR_IIII = 0,				// IIIIIIIIIIIIIII
	E_H264_IF_GOP_STR_IPPP,					// IPPPPPPPPPPPPPP
	E_H264_IF_GOP_STR_IBBP,					// IBBPBBPBBPBBPBB
	E_H264_IF_GOP_STR_IBP,					// IBPBPBPBPBPBPBP
} E_H264_IF_GOP_STR;

/* Inter/Intra Weight Control */
typedef enum {
	E_H264_IF_WEIGHT_CTRL_INTRA_3 = 0,		// INTRA WEIGHT 3
	E_H264_IF_WEIGHT_CTRL_INTRA_2,			// INTRA WEIGHT 2
	E_H264_IF_WEIGHT_CTRL_INTRA_1,			// INTRA WEIGHT 1
	E_H264_IF_WEIGHT_CTRL_DEFAULT,			// Default
	E_H264_IF_WEIGHT_CTRL_INTER_1,			// INTER WEIGHT 1
	E_H264_IF_WEIGHT_CTRL_INTER_2,			// INTER WEIGHT 2
	E_H264_IF_WEIGHT_CTRL_INTER_3,			// INTER WEIGHT 3
} E_H264_IF_WEIGHT_CTRL;

// [Play]

/** For speed ing FJ_VideoPlayback_SetSpeedDirection() */
typedef enum {
	E_H264_IF_PLAY_SPEED_1X			= 0,	/**< Normal speed */
	E_H264_IF_PLAY_SPEED_2X			= 1,	/**<     2x speed */
	E_H264_IF_PLAY_SPEED_4X			= 2,	/**<     4x speed */
	E_H264_IF_PLAY_SPEED_8X			= 3,	/**<     8x speed */
	E_H264_IF_PLAY_SPEED_1_2X		= 4,	/**<   1/2x speed */
	E_H264_IF_PLAY_SPEED_1_4X		= 5,	/**<   1/4x speed */
	E_H264_IF_PLAY_SPEED_1_8X		= 6,	/**<   1/8x speed */
} E_H264_IF_PLAY_SPEED;

/** For direction in FJ_VideoPlayback_SetSpeedDirection() */
typedef enum {
	E_H264_IF_PLAY_DIRECTION_FWD	= 0,	/**< Direction is Forward */
	E_H264_IF_PLAY_DIRECTION_RWD	= 1,	/**< Direction is Rewind */
} E_H264_IF_PLAY_DIRECTION;

/** Decoder Status */
typedef enum {
	E_H264_IF_PLAY_DECODER_STATUS_IDLE		= 0,	/**< Decoder status is idle */
	E_H264_IF_PLAY_DECODER_STATUS_NORMAL	= 1,	/**< Decoder status is normal */
	E_H264_IF_PLAY_DECODER_STATUS_LAST		= 2,	/**< Decoder status is last stream decoded */
} E_H264_IF_PLAY_DECODER_STATUS;

/* Movie Play display video format */
typedef enum {
	E_H264_IF_VIDEO_FORMAT_NONE = 0,	// none.
	E_H264_IF_VIDEO_FORMAT_NTSC,		// NTSC
	E_H264_IF_VIDEO_FORMAT_PAL,			// PAL
} E_H264_IF_VIDEO_FORMAT;

/* Movie Play point type */
typedef enum {
	E_H264_IF_POINT_TYPE_FRAME = 0,		// frame number
	E_H264_IF_POINT_TYPE_TIME,			// time
} E_H264_IF_POINT_TYPE;

/* Movie Play Conceal Error */
typedef enum {
	E_H264_IF_CONCEAL_ERROR_STOP = 0,		// play stop
	E_H264_IF_CONCEAL_ERROR_SKIP,			// frame skip
} E_H264_IF_CONCEAL_ERROR;

/* YUV format */
typedef enum {
	E_H264_IF_YUV_FORMAT_VIDEO		= 0,	/**< Video format(420 fixed) */
	E_H264_IF_YUV_FORMAT_YUV420		= 1,	/**< YUV420 format (Currently not used.) */
} E_H264_IF_YUV_FORMAT;

/* Rotation (clockwise) */
typedef enum {
	E_H264_IF_DISP_ROTATE_0 = 0,	/**< not rotate						*/
	E_H264_IF_DISP_ROTATE_90,		/**< rotate 90 degree (clockwise)	*/
	E_H264_IF_DISP_ROTATE_180,		/**< rotate 180 degree (clockwise)	*/
	E_H264_IF_DISP_ROTATE_270,		/**< rotate 270 degree (clockwise)	*/
} E_H264_IF_DISP_ROTATE;

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
// [common]

// [Record]
/* VUI Information */
typedef struct {
	UCHAR				video_signal_type_present_flag;		// OFF(0)/ON(1)
	UCHAR				video_format;						// 3bit
	UCHAR				video_full_range_flag;				// OFF(0)/ON(1)
	UCHAR				colour_description_present_flag;	// OFF(0)/ON(1)
	UCHAR				colour_primaries;					// 8bit
	UCHAR				transfer_characteristics;			// 8bit
	UCHAR				matrix_coefficients;				// 8bit
	UCHAR				aspect_ratio_info_present_flag;		// OFF(0)/ON(1)
	UCHAR				aspect_ratio_idc;					// 8bit
	USHORT				sar_width;							// 16bit
	USHORT				sar_height;							// 16bit
	UCHAR				chroma_loc_info_present_flag;		
	UCHAR				chroma_sample_loc_top_field;		
	UCHAR				chroma_sample_loc_bottom_field;		
} T_H264_IF_PARAM_VUI;

/** ScalingList Initial Information */
typedef struct {
	INT32				scaling_list_flg;			/**< Flag to Enable ScalingList */
	INT32				scaling_list_4x4_Y1[16];	/**< ScalingList 4x4 for Y1 */
	INT32				scaling_list_4x4_Cb1[16];	/**< ScalingList 4x4 for Cb1 */
	INT32				scaling_list_4x4_Cr1[16];	/**< ScalingList 4x4 for Cr1 */
	INT32				scaling_list_4x4_Y2[16];	/**< ScalingList 4x4 for Y2 */
	INT32				scaling_list_4x4_Cb2[16];	/**< ScalingList 4x4 for Cb2 */
	INT32				scaling_list_4x4_Cr2[16];	/**< ScalingList 4x4 for Cr2 */
	INT32				scaling_list_8x8_Y1[64];	/**< ScalingList 8x8 for Y1 */
	INT32				scaling_list_8x8_Y2[64];	/**< ScalingList 8x8 for Y2 */
} T_H264_INITIAL_SCALINGLIST;

/** Initialize Parameter */
typedef struct {
	E_H264_IF_STREAM_CH			stream_ch;					// stream ch
	UCHAR						enable_flg;					// Enable Stream Flg
	E_H264_IF_SCAN_MODE			scan_mode;					// Scan Mode
	E_H264_IF_STREAM_FORMAT		stream_format;				// stream format
	E_H264_IF_IMAGE_SIZE		size;						// Image Size
	E_H264_IF_IMAGE_SIZE		display_size;				// Display Image Size
	UINT32						width;						// Sample Input Width Size
	UINT32						lines;						// Sample Input Lines Size
	E_H264_IF_FRAME_RATE		frame_rate;					// Frame Rate
	E_H264_IF_RATE_MODE			rate_mode;					// Bitrate Algorithm	CBR(0)/VBR(1)
	UCHAR						level;						// H.264 Level
	ULONG						bitrate;					// Video Bit Rate(bps)	(Ex.24Mbps => D'24,000,000)
	ULONG 						average_bitrate;			// Average Bit Rate		(Ex.24Mbps => D'24,000,000)
	E_H264_IF_GOP_STR			gop_struct;					// GOP struct
	UINT32						gop_num;					// vop num in gop
	UINT32						gop_num_idr_to_idr;			// GOP number of IDR to IDR
	E_H264_IF_PROFILE			profile;					// Profile				HIGH(0)/MAIN(1)
	E_H264_IF_VLC_MODE			vlc_mode;					// VLC mode				CABAC(0)/CAVLC(1)
	UCHAR						vis;						// Video Stabilizer		OFF(0)/ON(1)
	UCHAR						closed_gop;					// Closed GOP			OFF(0)/ON(1)
	T_H264_IF_PARAM_VUI			vui;						// VUI parameter
	UCHAR						pictstruct_flag;			// Flag to Pictstruct in PictureTiming SEI	OFF(0)/ON(1)
	UCHAR						buffer_ctrl_flag;			// Flag to Insert hrd_parameters in VUI/BuffeingPeriodSEI/removal_delay in PictureTimingSEI	OFF(0)/ON(1)
	UCHAR						recovery_point_sei_flag;	// Flag to Recovery Point SEI	OFF(0)/ON(1)
	UCHAR						end_of_stream_flag;			// Flag to End Of Stream 		OFF(0)/ON(1)
	UCHAR						end_of_sequence_flag;		// Flag to End Of Sequence		OFF(0)/ON(1)
	UCHAR						CPB_allow_flg;				// Flag to Allow CPB Buffer Over	OFF(0)/ON(1)
	UCHAR						next_frame_comp_flg;		// Flag to Enable Next Frame Compare Mode	OFF(0)/ON(1)
	UCHAR						time_lapse_flag;			// Flag to TimeLapse	OFF(0)/ON(1)
	UCHAR						filler_flag;				// Flag to Insert Filler Data	OFF(0)/ON(1)
	UCHAR						max_qp;						// Maximum QP (0-51 : 51=OFF)
	UCHAR						min_qp;						// Minimum QP (0-51 : 0=OFF)
	UCHAR						vector_info_flag;			// Flag to Notify the Vector/Brightness Information
	UINT32						frame_crop_top_offset;		// Frame crop top offset
	UINT32						frame_crop_bottom_offset;	// Frame crop bottom offset
	UINT32						frame_crop_right_offset;	// Frame crop right offset
	UINT32						frame_crop_left_offset;		// Frame crop left offset
	ULONG						CPB_removal_delay;			// Value for initial_cpb_removal_delay(msec)
	T_H264_INITIAL_SCALINGLIST	initial_scalinglist_info;	// Initial ScalingList Information
	E_H264_IF_WEIGHT_CTRL		inter_intra_weight_ctrl;	// Inter/Intra Weight Control
	UCHAR						slice_encode_flag;			// Slice Encode Flag
// --- REMOVE_H264_IQ_ENABLE BEGIN ---
	UCHAR						prefetch_bypath;			// Prefetch Bypath Flag
// --- REMOVE_H264_IQ_ENABLE END ---
	UCHAR						delay_notify_flag;			// delay notify to host for stable interval
} T_H264_IF_PARAM_RECORD;

/** Dynamic Parameter */
typedef struct {
	UINT32						bitrate;					// Video Bit Rate(bps)	(Ex.24Mbps => D'24,000,000)
	UINT8						gop_struct;					// GOP Struct
	UINT16						frame_num_of_gop;			// GOP Frame num
	UINT8						closed_gop;					// Closed GOP			OFF(0)/ON(1)
	UINT8						gop_num_idr_to_idr;			// GOP number of IDR to IDR
} T_H264_IF_DYNAMIC_PARAM_RECORD;

/* --- Stream Information Send to HOST --- */
/* Video stream informations for HOST */
typedef struct{
	UINT32		stream_ch;				/**< Stream ID */
	ULLONG		frame_no;				/**< Frame Number */
	UINT32		width;					/**< Sample Input Width Size */
	UINT32		lines;					/**< Sample Input Lines Size */
	ULLONG		stream_serial_num;		/**< Stream Serial Num */
	UCHAR		stream_end_flg;			/**< Each Sample End Flag */
	UINT32		vstream_index;			/**< Stream Index for Free */
	UCHAR*		sample_address;			/**< Sample address */
	UINT32		sample_size;			/**< Sample size */
	UINT32		sample_type;			/**< Sample type (IDR=0,I=1,P=2,B=3) */
	LONG		sample_poc;				/**< Sample Picture Order Count */
	UINT32		frame_of_GOP;			/**< Frame Number of GOP */
	UINT32		VBV_buffer;				/**< VBV Buffer Value */
	UINT32		nal_unit_num;			/**< NUL unit number in 1sample */
	ULONG		nal_unit_size[11];		/**< Each NAL unit size in 1sample */
	LLONG		sample_pts;				/**< Sample PTS */
	LLONG		sample_dts;				/**< Sample DTS */
	UINT32		bitrate;				/**< Current Bitrate */
	UINT32		ave_bitrate;			/**< Current Average Bitrate */
	BOOL		error_flg;				/**< Error Flag */
	UINT32		ave_qp;					/**< Average QP */
	UINT32		cpb_over_flag;			/**< CPB Buffer Over flag (0:not over, 1:over) */
	ULONG		stream_buffer0_limit;	/**< Limit of Stream Buffer0 */
	ULONG		stream_buffer1_limit;	/**< Limit of Stream Buffer1 */
} T_H264_IF_VIDEO_STREAM_PARAM;

/** H264 Vector information parameter */
typedef struct{
	UINT32		stream_ch;				/**< Stream Channel */
	ULLONG		frame_no;				/**< Frame Number */
	UINT32		width;					/**< Sample Input Width Size */
	UINT32		lines;					/**< Sample Input Lines Size */
	UINT32		sample_type;			/**< Sample type (IDR=0,I=1,P=2,B=3) */
	E_H264_IF_GOP_STR	gop_struct;		/**< GOP struct */
	UINT32		gop_num;				/**< vop num in gop */
	LLONG		sample_pts;				/**< Sample PTS */
	LLONG		sample_dts;				/**< Sample DTS */
	UINT32		bitrate;				/**< Bitrate */
	UINT32		ave_bitrate;			/**< Average Bitrate */
	UINT32*		vector_addr;			/**< Vector Data Address */
	UINT32		vector_size;			/**< Vector Data Size */
	UINT32*		brightness_addr;		/**< Brightness Data Address */
	UINT32		brightness_size;		/**< Brightness Data Size */
}T_H264_IF_VIDEO_VECTOR_PARAM;

/** ScalingList Information */
typedef struct {
	INT32				seq_scaling_matrix_present_flag;	/**< seq_scaling_matrix_present_flag */
	INT32				pic_scaling_matrix_present_flag;	/**< pic_scaling_matrix_present_flag */
	INT32				scaling_list_Y_select;				/**< ScalingList Select for Y */
	INT32				scaling_list_C_select;				/**< ScalingList Select for Cb/Cr */
} T_H264_IF_SCALINGLIST;

/** Specific Field Information */
typedef struct {
	INT32				start_pos;			/**< StartPosition(16 align) */
	INT32				width_size;			/**< Width (Number of MacroBlock) */
	INT32				height_size;		/**< Height (Number of MacroBlock) */
	INT32				qp_down_range;		/**< Down Range of QP */
	INT32				qp_min;				/**< Minimum of QP */
	INT32				adptation_qp_flg;	/**< Flag of Adaptation QP */
} T_H264_IF_SPECIFIC_FIELD;

/** Select a Specific Field of QP */
typedef struct {
	INT32							field_num;					/**< Specific Field Number */
	T_H264_IF_SPECIFIC_FIELD		field[2];					/**< Field1 StartPosition(16 align) */
	INT32							out_field_qp_up_range;		/**< Another Field Up Range of QP */
	INT32							out_field_qp_max;			/**< Another Field Maximum of QP */
	INT32							out_field_adptation_qp_flg;	/**< Another Field Flag of Adaptation QP */
} T_H264_IF_SPECIFIC_FIELD_QP;

/** Get ImageAddress Parameter */
typedef struct{
	ULONG		y_addr;
	ULONG		c_addr;
	USHORT		g_width;
	BOOL		dummy_flg;
} T_H264_IF_GET_IMAGE_ADDR_PARAM;


// [Play]
// Decoder param
typedef struct {
	UINT32						image_width;		/**< Image Size(width). */
	UINT32						image_lines;		/**< Image Size(lines). */
	UINT32						image_bank_y_width;	/**< Image Bank Size(Y width). */
	UINT32						image_bank_y_lines;	/**< Image Bank Size(Y lines). */
	UINT32						image_bank_c_lines;	/**< Image Bank Size(C lines). */
	UINT32						image_bank_y_size;	/**< Image Bank Y Size. */
	UINT32						image_bank_c_size;	/**< Image Bank C Size. */
	
	E_H264_IF_IMAGE_SIZE		image_size;			/**< Image Size. */
	E_H264_IF_SCAN_MODE			scan_mode;			/**< scan mode. */
	E_H264_IF_CONCEAL_ERROR		conceal_error;		/**< operation at the time of the error. */
	E_H264_IF_FRAME_RATE		frame_rate;			/**< Frame Rate. */
	UINT32						time_scale;			/**< Time Scale. (ex. 29.97fps is 30000) */
	UINT32						time_delta;			/**< Time Delta. (ex. 29.97fps is 1001) */
	BOOL						trick;				/**< Whether the trick play. */
	UINT8						self_rec;			/**< Self-recording Stream Mode */
}T_H264_IF_DECODER_PARAM;

/* Display Information */
typedef struct {
	BOOL						valid_flag;					// Valid Flag
	UCHAR						disp_id;					// Connected disp_id.
	USHORT						image_width;				// Display Image Size Width
	USHORT						image_lines;				// Display Image Size Lines
	USHORT						global_width;				// Display Area Size Width
	USHORT						global_lines;				// Display Area Size Lines
	UINT32						frame_rate;					// Frame Rate
	E_H264_IF_SCAN_MODE			scan_mode;					// Scan Mode
	E_H264_IF_DISP_ROTATE		rotate;						// Rotate(clockwise)
} T_H264_IF_DISPLAY_PARAM;


typedef struct {
	T_H264_IF_DISPLAY_PARAM		lcd_info;					// LCD information
	T_H264_IF_DISPLAY_PARAM		hdmi_info;					// HDMI information
} T_H264_IF_DISPLAY_INFO;


/* Movie Initialize Parameters (INITIALIZE request) */
typedef struct {
	T_H264_IF_DECODER_PARAM		decoder_param;				// Decoder parameter
	T_H264_IF_DISPLAY_INFO		display_info;				// Display information
} T_H264_IF_PARAM_INIT;


/* Movie Play Parameters (PLAY request) */
typedef struct {
	ULLONG						start_pts;					// Play Start PTS
	E_H264_IF_PLAY_DIRECTION	direction;					// Play Direction
	E_H264_IF_PLAY_SPEED		speed;						// Play Speed
	BOOL						pause;						// Force transition to Pause state
} T_H264_IF_PARAM_PLAY;


/* Movie Play Stop Parameters (STOP request) */
typedef struct {
	BOOL						last_picture_keep;			// Last Picture Keep(TRUE/FALSE).
} T_H264_IF_PARAM_PLAY_STOP;


/* Movie Play Parameters (PLAY request) */
typedef struct {
	UINT8						stream_end_flag;	/**< Last stream = 1, Otherwise = 0. <br> */
	UINT8						stss;				/**< It exists in MP4 of stss atom = 1, Not exist = 0. <br> */
	UINT32						stream_size;		/**< Stream size. <br> */
	UINT32						stream_address;		/**< Stored stream address. <br> */
	ULLONG						pts;				/**< Presentation Time Stamp (ex. 1001) <br> */
	ULLONG						dts;				/**< Decoding Time Stamp (ex. 1001) <br> */
} T_H264_IF_PARAM_SET_STREAM;


/* Movie Play Parameters (FF/REW/SEEK request) */
typedef struct {
	E_H264_IF_PLAY_SPEED		speed;					// FF/REW speed
	ULONG						seek_frame;				// Seek Frame
} T_H264_IF_PARAM_TRICK;


/* YUV parameter */
typedef struct{
	UINT32						y_addr;			/**< Y    data stored address. <br> */
	UINT32						cbcr_addr;		/**< CbCr data stored address. <br> */
	UINT32						g_width;		/**< global horizontal size. <br> */
	UINT32						g_lines;		/**< global vertical size. <br> */
	UINT32						width;			/**< horizontal size. <br> */
	UINT32						lines;			/**< vertical size. <br> */
	E_H264_IF_YUV_FORMAT		yuv_format;		/**< YUV format. <br> */
} T_H264_IF_VIDEO_YUV_INFO;


// [TopImage/Attribute]
/* Movie Play Parameters (Topimage request) */
typedef struct {
	T_H264_IF_DECODER_PARAM*	decoder_param;				// Decoder parameter
	ULLONG						req_pts;					// PTS of request frame
	UINT32						store_num;					// Video Stream Num
	T_H264_IF_PARAM_SET_STREAM*	video_stream_param;			// Video Stream Info
	T_H264_IF_VIDEO_YUV_INFO*	decode_yuv_info;			// YUV information after decoding
} T_H264_IF_PARAM_TOPIMAGE;

/* Movie Encode/Decode Task Message Interface Definition (mbox) */
typedef union {
	T_H264_IF_PARAM_RECORD			*pRecordParam;
	T_H264_IF_PARAM_INIT			*pInitParam;
	T_H264_IF_PARAM_PLAY			*pPlayParam;
	T_H264_IF_PARAM_PLAY_STOP		*pPlayStopParam;
	T_H264_IF_PARAM_TOPIMAGE		*pTopimageParam;
	ULONG						info;
} U_H264_IF_PARAM;

/* msg if */
typedef struct {
	E_H264_IF_TYPE		type;
	U_H264_IF_PARAM		data;
	VOID ( *cbfunc )( int, int );
} T_H264_IF_MSG;

/* Movie Encode/Decode Handller structure */
typedef struct {
	E_H264_IF_TYPE	type;
	INT32	(*handler)(T_H264_IF_MSG*);
	CHAR	sztype[50];
	INT32	bcallback;
} T_H264_IF_HANDLER;

/* mbox structure */
typedef struct {
	OS_USER_VP		_os_reserve[2];	// OS reserve
	OS_USER_VP		blf_addr;		// blf address
	OS_USER_VB		msg[16];		// message
} T_H264_IF_MBOX;


/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
// [H264 controller [main TASK]]
extern	VOID	H264_Controller( INT32 stacd );

// [H264 Task Interface (MailBOX)]
extern	INT32	H264_If_Set_Msg( T_H264_IF_MSG* msg, E_H264_IF_MSG_WAIT wait);
extern	INT32	H264_If_Get_Msg( T_H264_IF_MSG* msg, E_H264_IF_MSG_WAIT wait);
extern	INT32	H264_If_Clear_Msg( VOID );


// [Function call IF]
/* (Record) */
/*  -Input YC Image setting Address get */
extern	INT32	H264_If_Get_Record_Image_Address(
					E_H264_IF_STREAM_CH		stream_ch,
					E_H264_IF_IMAGE_SIZE	size,
					ULLONG					frame_no, 
					T_H264_IF_GET_IMAGE_ADDR_PARAM* addr_param
					);
extern	VOID	H264_If_Decide_Record_Image_Address( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg );
extern	VOID	H264_If_Record_Frame_Enabled( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg, BOOL enabled );
extern	VOID	H264_If_Record_Valid_Image_Address( E_H264_IF_STREAM_CH stream_ch );
extern	VOID	H264_If_Record_Dynamic_Param_Set( E_H264_IF_STREAM_CH stream_ch, T_H264_IF_DYNAMIC_PARAM_RECORD* dynamic_param );
extern	VOID	H264_If_Record_Insert_IDR_Trigger_Set( E_H264_IF_STREAM_CH stream_ch );
/*  -Free video stream */
extern	INT32	H264_If_Record_Free_Video_Index( E_H264_IF_STREAM_CH stream_ch, UINT32 free_vstream_index, BOOL last_free_flg );


/* (Play)  */
/*  -Total Play Time get */
extern	INT32	H264_If_Play_Set_Video_Stream( UINT32 store_num, T_H264_IF_PARAM_SET_STREAM* video_stream_param );
/*  -Current Display Frame get */
extern	INT32	H264_If_Play_Get_Display_Frame( T_H264_IF_VIDEO_YUV_INFO* display_yuv_info );

/*  -Top Image get */
extern	INT32	H264_If_Get_Play_Topimage( T_H264_IF_MSG* msg );	// I: messange

// [Host definition Function]
/* (Common) */
extern	VOID	H264_If_Set_Codec_CoreMode( UINT32 core_mode );

extern	VOID	Host_H264_Notify_Stop_Trigger( E_H264_IF_STOP_NOTIFY param );
extern	VOID	Host_H264_Notify_Stop_Complete( VOID );
extern	VOID	Host_H264_Notify_Vstrm_Free_Timeout( VOID );
extern	VOID	Host_H264_Notify_State_Change_Complete( INT32 result );
/* (Record) */
extern	BOOL	Host_H264_Notify_Video_Stream(T_H264_IF_VIDEO_STREAM_PARAM* vstream_info);
extern	BOOL	Host_H264_Notify_Vector_Info(T_H264_IF_VIDEO_VECTOR_PARAM* vector_info);
extern	BOOL	Host_H264_Notify_PreEncode( E_H264_IF_STREAM_CH stream_ch );
extern	BOOL	Host_H264_Notify_RepeatFrame( E_H264_IF_STREAM_CH stream_ch );
/* (Play) */
extern	BOOL	Host_H264_Notify_Video_Stream_Request( VOID );
extern	BOOL	Host_H264_Notify_FirstFrameDisplayCB( E_H264_IF_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error );
extern	BOOL	Host_H264_Notify_VideoDecodeCB( E_H264_IF_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error );
extern	VOID	Host_H264_Notify_Lcd_Update( UINT32* y_addr, UINT32* c_addr, BOOL update );
extern	VOID	Host_H264_Notify_Tv_Update( ULONG* y_addr, ULONG* c_addr );
extern	VOID	Host_H264_Notify_Hdmi_Update( UINT32* y_addr, UINT32* c_addr, BOOL update );

#endif
