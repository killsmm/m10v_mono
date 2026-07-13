/**
 * @file		os_if.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _OS_IF_H_
#define _OS_IF_H_

#include "share.h"
#include "fj_encoder.h"
#include "dcf_if.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//Magic code
#define D_CPU_IF_MCODE_COMMAND_REQ			(0x5555AAAA)
#define D_CPU_IF_MCODE_COOMNAD_ACK			(0x6666BBBB)
#define D_CPU_IF_MCODE_EVENT_NOTIFY			(0x7777CCCC)
#define D_CPU_IF_MCODE_INFO_NOTIFY			(0x8888DDDD)
#define D_CPU_IF_MCODE_STREAM_SEND			(0x9999EEEE)
#define D_CPU_IF_MCODE_STREAM_RELESE		(0xAAAAFFFF)
#define D_CPU_IF_MCODE_GET_FROM_BF			(0xBBBB0000)

//Format version
 #define D_CPU_IF_COM_FORMAT_VERSION					(0x00000001)

 //Event Parameter Value
#define D_STATUS_REMOVED		(0x00000001)
#define D_STATUS_INSERTED		(0x00000000)
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

typedef enum {
	E_IPCU_CH67_ENCODE = 0,
	E_IPCU_CH67_DECODE = 1,
} E_IPCU_CH67_SWITCH;

typedef enum {
	eIpcuDecimal			= 0,
	eIpcuDecimalNag1		= 1, /* 1/10 */
	eIpcuDecimalNag2		= 2, /* 1/100 */
	eIpcuDecimalNag3		= 3, /* 1/1000 */
	eIpcuBrightness			= 4, /* Brightness*/
	eIpcuExpTime			= 5, /* Exposure time*/
} IPCU_DATA_FMT;

typedef enum {
	//STREAM_STR
  	E_CPU_IF_COMMAND_STREAM_VIDEO = 0x00000100,
	E_CPU_IF_COMMAND_STREAM_AUDIO = 0x00000200,
	E_CPU_IF_COMMAND_STREAM_RAW	  = 0x00000300,
	E_CPU_IF_COMMAND_STREAM_YUV = 0x00000400,
	E_CPU_IF_COMMAND_STREAM_JPG   = 0x00000500,
	E_CPU_IF_COMMAND_STREAM_META = 0x00010000,
} E_CPU_IF_COMMAND_STREAM;

typedef enum {
	E_CPU_IF_VSTRM_TYPE_H265 = 1,			/*!< H.265 */
	E_CPU_IF_VSTRM_TYPE_H264,				/*!< H.264 */
	E_CPU_IF_VSTRM_TYPE_JPEG,				/*!< MJPEG */
} E_CPU_IF_VSTRM_TYPE;

typedef enum {
	E_CPU_IF_VSTRM_FORMAT_ES = 1,			/*!< ES */
	E_CPU_IF_VSTRM_FORMAT_TS,				/*!< TS */
} E_CPU_IF_VSTRM_FROMAT;

typedef enum {
	E_CPU_IF_ASTRM_TYPE_ES = 1,				/*!< ES */
	E_CPU_IF_ASTRM_TYPE_TS,					/*!< TS */
} E_CPU_IF_ASTRM_FROMAT;

//Stream
typedef enum {
  	E_CPU_IF_COMMAND_SET_STREAM_STR = 0x00010100,
} E_CPU_IF_COMMAND_SET_STREAM;

typedef enum {
	E_CPU_IF_UVC_FORMAT_YUV = 0,
	E_CPU_IF_UVC_FORMAT_H264,
	E_CPU_IF_UVC_FORMAT_JPEG,
	E_CPU_IF_UVC_FORMAT_NV12,
	E_CPU_IF_UVC_FORMAT_MAX
} E_CPU_IF_UVC_FORMAT;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

typedef struct {
	ULONG	magic_code;
	ULONG	format_version;
	ULONG	cmd_set;
	ULONG	cmd;
} T_CPUCMD_HEADER;

typedef struct {
	UINT64 sof; // us
	UINT64 eof; // us
} T_CPUCMD_SENSOR_TIMESTAMP;

//Stream
typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG	stype;
	ULONG	sformat;
	ULONG	stream_id;
	ULONG	stream_end_flg;
	ULONG	area_index;
	ULONG	sample_size;
	ULONG	sample_address;
	//ULONG	rsv_ch;
	LLONG	sample_pts;
	ULONG	sample_type;
	ULONG	frame_of_GOP;
	ULONG	nal_unit_num;
	ULONG	nal_unit_size[32];
	ULONG	bitrate;
	ULONG	ave_bitrate;
	ULONG	width;
	ULONG	lines;
	ULLONG 	frame_no;
} T_CPUCMD_VIDEO_STREAM;

typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG    stype;            // H264 or H265
	ULONG    stream_end_flg;   // Is it a last frame?
	ULONG    area_index;       // frame number index
	ULONG    sample_size;      // frame size
	ULONG    sample_address;   // frame buffer address
	LLONG    sample_pts;       // frame presentation time stamp
} T_CPUCMD_VIDEO_DEC_STREAM;

typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG	stype;
	ULONG	sformat;
	ULONG	stream_id;
	ULONG	stream_end_flg;
	ULONG	area_index;
	ULONG	sample_size;
	ULONG	sample_address;
	ULONG	rsv_ch;
	ULLONG	time_stamp;
	ULONG	channel;
	ULONG	sampling_freq;
	ULONG	bit_rate;
	ULLONG	frame_no;
} T_CPUCMD_AUDIO_STREAM;

typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG	stype;
	ULONG	sformat;
	ULONG	stream_id;
	ULONG	stream_end_flg;
	ULONG	area_index;	
	ULONG	sample_size;
	ULONG	sample_address;
	ULONG	rsv_ch0;
	ULLONG 	time_stamp;
	ULONG	global_width;
	ULONG	width;
	ULONG	lines;
//	ULONG	pack_type; 
//	ULONG	first_pixel;
	ULONG	rsv_ch1;
	ULLONG	frame_no;
}T_CPUCMD_VRAW_STREAM;

typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG	stype;
	ULONG	sformat;
	ULONG	stream_id;
	ULONG	stream_end_flg;
	ULONG	area_index;	
	ULONG	sample_size;
	ULONG	sample_address;
	ULONG	rsv_ch;
	ULLONG	time_stamp;
	ULONG	width;
	ULONG	lines;
	ULLONG	frame_no;
}T_CPUCMD_YUV_STREAM;

typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG	board;
	ULONG	stype;
	ULONG	area_index;
	ULONG	sample_size;
	ULONG	sample_address;
	ULONG	stream_id;
	ULONG	stream_end_flg;
	T_CPUCMD_SENSOR_TIMESTAMP sensor_timestamp;
	ULONG	rsv_ul_0[9];
} T_CPUCMD_MOV_STREAM;


typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG	stype;
	ULONG	sformat;
	ULONG	stream_id;
	ULONG	stream_end_flg;
	ULONG	area_index;	
	ULONG	sample_size;
	ULONG	sample_address;
	//ULONG	rsv_ch;
	LLONG	sample_pts;
	ULONG	width;
	ULONG	lines;
	ULONG	y_offset;
	ULONG	y_size;
	ULONG	c_offset;
	ULONG	c_size;
	ULLONG	frame_no;
	T_CPUCMD_SENSOR_TIMESTAMP sensor_timestamp;
	T_BF_DCF_IF_EXIF_INFO exif;
} T_CPUCMD_JPG_STREAM;

typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG	board;
	ULONG	stype;
	UINT8	stream_end_flag;	
	UINT8	stss;				
	UINT32	stream_size;		
	UINT32	stream_address;		
	ULLONG	pts;				
	ULLONG	dts;				
	USHORT	width;				
	USHORT	height;				
	UINT32	nal_unit_num;		
	ULONG	nal_unit_size[8];	
	ULLONG	stream_serial_num;
	USHORT	counter_in_GOP;	
	ULONG	rsv_ch_0;	
	ULONG	rsv_ch_1;
	T_CPUCMD_SENSOR_TIMESTAMP sensor_timestamp;
} T_CPUCMD_SPVIDEO_STREAM;

typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG	board;
	ULONG	stype;
	ULONG	sample_size;
	ULONG	sample_address;
	ULONG	stream_end_flg;
	ULONG	ch_type;
	ULONG	sampling_freq;
	ULONG	bit_rate;
	ULONG	rsv_ul_1[8];
	LLONG	rsv_ll_0;
	ULLONG	time_stamp;
	ULLONG	rsv_ull_0[2];
	ULONG	stream_format;
	ULLONG	rsv_ll_1;
} T_CPUCMD_SPAUDIO_STREAM;

typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG	mtype;
	ULONG	sformat;
	ULONG	meta_id;
	ULONG	stream_end_flg;
	ULONG	area_index;
	ULONG	sample_size;
	ULONG	sample_address;
	ULONG	rsv_ch;
	ULLONG	frame_no;
} T_CPUCMD_META_STREAM;


typedef union {
	T_CPUCMD_HEADER	t_head;
	T_CPUCMD_VIDEO_STREAM vstream;
	T_CPUCMD_AUDIO_STREAM astream;
	T_CPUCMD_VRAW_STREAM  rawstream;
	T_CPUCMD_YUV_STREAM	 ystream;
	T_CPUCMD_MOV_STREAM mstream;
	T_CPUCMD_JPG_STREAM	jstream;
	T_CPUCMD_SPVIDEO_STREAM spvstream;
	T_CPUCMD_SPAUDIO_STREAM spastream;
	T_CPUCMD_META_STREAM metastream;
} T_CPUCMD_STREAM;

typedef enum {
	E_CPU_IF_YUV_TYPE_420= 1,			/*!< 420 */
	E_CPU_IF_YUV_TYPE_422,				/*!< 422 */
} E_CPU_IF_YUV_TYPE;

typedef enum {
	E_CPU_IF_YSTRM_FORMAT_PLANAR = 1,		/*!< PLANAR */
	E_CPU_IF_YSTRM_FORMAT_INTERLEAVED,		/*!< INTERLEAVED */
	E_CPU_IF_YSTRM_FORMAT_SEMI_PLANAR,		/*!< SEMI_PLANAR */
	E_CPU_IF_YSTRM_FORMAT_UYVY,				/*!< UYVY */
	E_CPU_IF_YSTRM_FORMAT_YUYV,				/*!< YUYV */
} E_CPU_IF_YSTRM_FORMAT;

typedef enum {
	E_CPU_IF_ASTRM_TYPE_PCM = 1,			/*!< ES */
	E_CPU_IF_ASTRM_TYPE_AAC,				/*!< TS */
} E_CPU_IF_ASTRM_TYPE;

//Notify
typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG			notify1;
	ULONG			notify2;
	ULONG			notify3;
	ULONG			notify4;
	CHAR			evt[128];
} T_CPUCMD_NOTIFY_EVENT;

//Notify_Event
typedef enum {
	E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_STATUS_CHANGED = 0x00010100,
	E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_PROC_FINISH = 0x00010200,
	E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_PROC_ERROR  = 0x00010300,
	E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_PANO_UPDATE = 0x00010500,
	E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_TIME_LAPSE_COUNT = 0x00010600,
	E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_ISSUE_CMD = 0x00010700,
} E_CPU_IF_COMMAND_SET_NOTIFY_EVENT;

//Notify_Event
typedef enum {
	//STATUS_CHANGED
	E_CPU_IF_COMMAND_NOTIFY_EVENT_HDMI_STATUS_CHANGED	= 0x00000100,
	E_CPU_IF_COMMAND_NOTIFY_EVENT_SD_STATUS_CHANGED		= 0x00000200,

	//PROC_FINISH
	E_CPU_IF_COMMAND_NOTIFY_EVENT_IMAGE_CAPTURE_FINISH	= 0x00000100,
	E_CPU_IF_COMMAND_NOTIFY_EVENT_VIDEO_CAPTURE_FINISH	= 0x00000200,
	E_CPU_IF_COMMAND_NOTIFY_EVENT_PLAY_FINISH			= 0x00000300,
	E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_SNAPSHOT_FINISH   = 0x00000600,
	
	//PROC_ERROR
	E_CPU_IF_COMMAND_NOTIFY_EVENT_PREVIEW_WIFI_ERROR	= 0x00000100,
	E_CPU_IF_COMMAND_NOTIFY_EVENT_IMAGE_CAPTURE_ERROR	= 0x00000200,
	E_CPU_IF_COMMAND_NOTIFY_EVENT_VIDEO_CAPTURE_ERROR	= 0x00000300,
	E_CPU_IF_COMMAND_NOTIFY_EVENT_PLAY_ERROR			= 0x00000400,

	// CAMERA FOCUS RANGE
	E_CPU_IF_COMMAND_NOTIFY_EVENT_CAMERA_FOCUS_RANGE_CHANGED	= 0x00000500,
	//PANORAMA_UPDATED
	E_CPU_IF_COMMAND_NOTIFY_EVENT_PANORAMA_PROGRESS		= 0x00000100,
	
	//TIME_LAPSE_COUNT
	E_CPU_IF_COMMAND_NOTIFY_EVENT_TIME_LAPSE_COUNT		= 0x00000100,
	E_CPU_IF_COMMAND_NOTIFY_EVENT_LINUX_CMD			= 0x00000900,
} E_CPU_IF_COMMAND_NOTIFY_EVENT;

typedef struct {
	T_CPUCMD_HEADER	t_head;
	ULONG			notify1;
	ULONG			notify2;
	ULONG			notify3;
	ULONG			notify4;
	ULONG			notify5;
	ULONG			notify6;
	ULONG			notify7;
	ULONG			notify8;
	ULONG			notify9;
	ULONG			notify10;
	ULONG			notify11;
	ULONG			notify12;
	ULONG			notify13;
	ULONG			notify14;
	ULONG			notify15;
	ULONG			notify16;
	ULONG			notify17;
	ULONG			notify18;
	ULONG			notify19;
	ULONG			notify20;
	ULONG			notify21;
	ULONG			notify22;
	ULONG			notify23;
	ULONG			notify24;
	ULONG			notify25;
	ULONG			notify26;
	ULONG			notify27;
	ULONG			notify28;
	ULONG			notify29;
	ULONG			notify30;
	ULONG			notify31;
	ULONG			notify32;
	ULONG			notify33;
	ULONG			notify34;
	ULONG			notify35;
	ULONG			notify36;
	ULONG			notify37;
	ULONG			notify38;
	ULONG			notify39;
	ULONG			notify40;
	ULONG			notify41;
	ULONG			notify42;
	ULONG			notify43;
	ULONG			notify44;
	ULONG			notify45;
	ULONG			notify46;
	ULONG			notify47;
	ULONG			notify48;
	ULONG			notify49;
	ULONG			notify50;
	ULONG			notify51;
	ULONG			notify52;
	ULONG			notify53;
	ULONG			notify54;
	ULONG			notify55;
	ULONG			notify56;
	ULONG			notify57;
} T_CPUCMD_NOTIFY_INFO;

typedef struct {
	ULONG				with_header;
	ULONG				header_size;
	ULONG				max_payload_size;
	E_CPU_IF_UVC_FORMAT	format;
} T_UVC_PARAM;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

void OS_IF_Init(void);
void OS_IF_Rtos_MJPEG_Stream_Send( FW_USHORT width, FW_USHORT lines, const ULLONG frame_no );
void OS_IF_Rtos_MJPEG_Stream_Send2( ULONG addr, ULONG size, FW_USHORT width, FW_USHORT lines, const ULLONG frame_no );
void OS_IF_Rtos_MJPEG_Stream_Send3(ULONG addr, ULONG size, T_BF_DCF_IF_EXIF_INFO *exif, FW_USHORT width, FW_USHORT lines, const ULLONG frame_no);
void OS_IF_Rtos_H264_Stream_Send( T_FJ_MOVIE_H264_STREAM_PARAM* stream_info, UCHAR video_enc_id, const ULLONG frame_no );
void OS_IF_Rtos_H265_Stream_Send( T_FJ_MOVIE_H265_STREAM_PARAM* stream_info, UCHAR video_enc_id, const ULLONG frame_no );
void OS_IF_Rtos_YUV_Stream_Send( ULONG addr, ULONG size, USHORT width, USHORT lines, const ULLONG frame_no );
void OS_IF_Rtos_RAW_Stream_Send( ULONG addr, ULONG size, USHORT width, USHORT lines, const ULLONG frame_no );
void OS_IF_Rtos_Audio_Stream_Send_AAC( UCHAR aid, T_FJ_AUDIO_AAC_STREAM_PARAM* cb_inf, const BOOL uac_en);
void OS_IF_Rtos_Audio_Stream_Send_PCM( UCHAR aid, T_FJ_AUDIO_PCM_STREAM_PARAM* cb_inf, const BOOL uac_en);
void OS_IF_Rtos_Event_Notify(const ULONG cmd_set, const ULONG cmd, const ULONG param[4]);
void OS_IF_Rtos_Linux_CMD( char * str, int size, UCHAR sync);
void OS_IF_Rtos_Stream_Ack(E_CPU_IF_COMMAND_STREAM sub, ULONG stype);
void OS_IF_Rtos_IPCU_CH67_Switch(E_IPCU_CH67_SWITCH sw);

void OS_IF_Rtos_UVC_Init( void );
void OS_IF_Rtos_UVC_Term( void );
BOOL OS_IF_Rtos_UVC_Is_Header( void );
INT32 OS_IF_Rtos_UVC_Format( void );
ULONG OS_IF_Rtos_UVC_Get_Image_Address( void );
void OS_IF_Rtos_UVC_YUV_Stream_Send( ULONG addr, ULONG size, FW_USHORT width, FW_USHORT lines, const ULLONG frame_no );
void OS_IF_Rtos_UVC_MJPEG_Stream_Send( FW_USHORT width, FW_USHORT lines, const ULLONG frame_no );
void OS_IF_Rtos_UVC_H264_Stream_Send( T_FJ_MOVIE_H264_STREAM_PARAM* stream_info, UCHAR video_enc_id, const ULLONG frame_no );


#ifdef __cplusplus
}
#endif

#endif	// _OS_IF_H_

