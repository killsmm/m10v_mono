/**
 * @file		instance.h
 * @brief		Instance common header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include "ddim_typedef.h"
#include "fj_still.h"
#include "fj_encoder.h"
#include "fj_decoder.h"
#include "fj_disp.h"
#include "fj_audio.h"
#include "fj_decoder.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Instance number max.
#define D_BF_INSTANCE_NUM_MAX_SENSOR			(4)		// 4 Sensor.

// Instance ID max.
#define D_BF_INSTANCE_ID_MAX_SEN				(10)	// Control of BaseFw(=Instance number max).
#define D_BF_INSTANCE_ID_MAX_DEMOSAIC			(10)	// Control of BaseFw(=Instance number max).
#define D_BF_INSTANCE_ID_MAX_VIDEO_ENCODE		(5)		// Control of BaseFw(=Instance number max). (H.264=3, H.265=2)
#define D_BF_INSTANCE_ID_MAX_VIDEO_DECODE		(2)		// Control of BaseFw(=Instance number max). (H.264=1, H.265=1)
#define D_BF_INSTANCE_ID_MAX_STILL_ENCODE		(10)	// Control of BaseFw(=Instance number max).
#define D_BF_INSTANCE_ID_MAX_DISPLAY			(2)		// Control of BaseFw(=Instance number max).
#define D_BF_INSTANCE_ID_MAX_STITCH				(10)	// Control of BaseFw(=Instance number max).
#define D_BF_INSTANCE_ID_MAX_AUDIO_SRC			(3)		// Control of BaseFw(=Instance number max).
#define D_BF_INSTANCE_ID_MAX_AUDIO_CAP			(2)		// Control of BaseFw(=Instance number max).
#define D_BF_INSTANCE_ID_MAX_AUDIO_OUT			(2)		// Control of BaseFw(=Instance number max).
#define D_BF_INSTANCE_ID_MAX_AUDIO_ENC			(2)		// Control of BaseFw(=Instance number max).
#define D_BF_INSTANCE_ID_MAX_AUDIO_DEC			(1)		// Control of BaseFw(=Instance number max).

// Output max.
#define D_BF_INSTANCE_OUTPUT_MAX_SEN			(2)
#define D_BF_INSTANCE_OUTPUT_MAX_DEMOSAIC		(FJ_DEMOSAIC_OUTPUT_NUM_MAX)
#define D_BF_INSTANCE_OUTPUT_MAX_STITCH			(3)
#define D_BF_INSTANCE_OUTPUT_MAX_VIDEO_DEC		(2)

// Input max.
#define D_BF_INSTANCE_INPUT_MAX_STITCH			(2)		// 0:VideoEncoder 1:StillEncoder(thumbnail)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Flag.
typedef enum{
	E_BF_INSTANCE_FLAG_UNUSE,						// Unuse.
	E_BF_INSTANCE_FLAG_USE,							// Use.
} E_BF_INSTANCE_FLAG;

// State.
typedef enum{
	E_BF_INSTANCE_STATE_NORMAL,						// Normal.
	E_BF_INSTANCE_STATE_SUSPEND,					// Suspend.
} E_BF_INSTANCE_STATE;

// LCD/HDMI Connect State.
typedef enum{
	E_BF_INSTANCE_CONNECT_STATE_ON,					// Connect LCD/HDMI.
	E_BF_INSTANCE_CONNECT_STATE_OFF,				// Disconnect LCD/HDMI.
} E_BF_INSTANCE_CONNECT_STATE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Connection.
typedef struct {
	UCHAR							instance_id;									// instance ID.
	UCHAR*							instance_p;										// instance pointer.
} T_BF_INSTANCE_CONNECT;

// Sensor connection.
typedef struct {
	T_BF_INSTANCE_CONNECT			sen;											// SEN connection.
} T_BF_INSTANCE_CONNECT_SENSOR;

// SEN before connection.
typedef struct {
	T_BF_INSTANCE_CONNECT			sensor;											// Sensor connection.
} T_BF_INSTANCE_CONNECT_SEN_BEFORE;

// SEN next connection.
typedef struct {
	T_BF_INSTANCE_CONNECT			demosaic[D_BF_INSTANCE_OUTPUT_MAX_SEN];			// Demosaic connection. [0]:Video(Rec), [1]:PIV.
	T_BF_INSTANCE_CONNECT			display;										// Display connection.
} T_BF_INSTANCE_CONNECT_SEN_NEXT;

// SEN connection.
typedef struct {
	T_BF_INSTANCE_CONNECT_SEN_BEFORE	before;										// SEN before connection.
	T_BF_INSTANCE_CONNECT_SEN_NEXT		next;										// SEN next connection.
} T_BF_INSTANCE_CONNECT_SEN;

// Demosaic before connection.
typedef struct {
	T_BF_INSTANCE_CONNECT			sen;											// SEN connection.
} T_BF_INSTANCE_CONNECT_DEMOSAIC_BEFORE;

// Demosaic next connection.
typedef struct {
	T_BF_INSTANCE_CONNECT			encode;											// Encode connection.
	T_BF_INSTANCE_CONNECT			display;										// Display connection.
	T_BF_INSTANCE_CONNECT			stitch;											// Stitch connection.
} T_BF_INSTANCE_CONNECT_DEMOSAIC_NEXT;

// Demosaic connection.
typedef struct {
	T_BF_INSTANCE_CONNECT_DEMOSAIC_BEFORE	before;										// Demosaic before connection.
	T_BF_INSTANCE_CONNECT_DEMOSAIC_NEXT		next[D_BF_INSTANCE_OUTPUT_MAX_DEMOSAIC];	// Demosaic next connection.
} T_BF_INSTANCE_CONNECT_DEMOSAIC;

// Stitch connection.
typedef struct {
	T_BF_INSTANCE_CONNECT			encode[D_BF_INSTANCE_OUTPUT_MAX_STITCH+1];		// Encode connection.
	T_BF_INSTANCE_CONNECT			display;										// Display connection.
} T_BF_INSTANCE_CONNECT_STITCH;

// VideoDecoder connection.
typedef struct {
	T_BF_INSTANCE_CONNECT			display[D_BF_INSTANCE_OUTPUT_MAX_VIDEO_DEC];	// Display connection.
} T_BF_INSTANCE_CONNECT_VIDEO_DEC;

// AudioCapture connection.
typedef struct {
	T_BF_INSTANCE_CONNECT			audio_encoder;									// AudioEncoder connection.
	T_BF_INSTANCE_CONNECT			audio_out;										// AudioOut connection.
} T_BF_INSTANCE_CONNECT_AUDIO_CAP;

// AudioDecoder connection.
typedef struct {
	T_BF_INSTANCE_CONNECT			audio_out;										// AudioOut connection.
} T_BF_INSTANCE_CONNECT_AUDIO_DEC;

// Basic size.
typedef struct {
	UINT32							width;											// Width.
	UINT32							lines;											// Lines.
} T_BF_INSTANCE_SIZE;

// Basic window.
typedef struct {
	UINT32							x_pos;											// Input  X offset.
	UINT32							y_pos;											// Input  Y offset.
	T_BF_INSTANCE_SIZE				input;											// Input  size.
	T_BF_INSTANCE_SIZE				output;											// Output size.
} T_BF_INSTANCE_WINDOW;

// Sensor window.
typedef struct {
	T_BF_INSTANCE_SIZE				g_input;										// Global iutput size.
	UINT32							ob_x_pos;										// OB area X offset.
	UINT32							ob_y_pos;										// OB area Y offset.
	T_BF_INSTANCE_SIZE				ob_input;										// OB area size.
} T_BF_INSTANCE_WINDOW_SENSOR;

// Demosaic window.
typedef struct {
	UINT32							x_pos;											// Input X offset.
	UINT32							y_pos;											// Input Y offset.
	T_BF_INSTANCE_SIZE				input;											// Input size.
	T_BF_INSTANCE_SIZE				scaler_input;									// Scaler input size.
	T_BF_INSTANCE_SIZE				scaler_output;									// Scaler output size.
	T_BF_INSTANCE_SIZE				output[D_BF_INSTANCE_OUTPUT_MAX_DEMOSAIC];		// Output size.
} T_BF_INSTANCE_WINDOW_DEMOSAIC;

// Stitch window.
typedef struct {
	T_BF_INSTANCE_SIZE				input[D_BF_INSTANCE_INPUT_MAX_STITCH];			// Input size.
	UINT32							ovr_width[2];									// Overwrap width.
	UINT32							img_width_div[2];								// Divided image width.
	T_BF_INSTANCE_SIZE				output[D_BF_INSTANCE_OUTPUT_MAX_STITCH];		// Output size.
} T_BF_INSTANCE_WINDOW_STITCH;

// Basic window.
typedef struct {
	UINT32							in_x_pos;										// Input  X offset.
	UINT32							in_y_pos;										// Input  Y offset.
	UINT32							out_x_pos;										// Output  X offset.
	UINT32							out_y_pos;										// Output  Y offset.
	T_BF_INSTANCE_SIZE				input;											// Input  size.
	T_BF_INSTANCE_SIZE				output;											// Output size.
} T_BF_INSTANCE_WINDOW_DISPLAY;

// Demosaic control input(for PIV).
typedef struct {
	UCHAR							row_division_num;								// Row division number of still processing.
	UCHAR							column_division_num;							// Column division number of still processing.
	UCHAR							internal_division_num;
} T_BF_INSTANCE_CONTROL_DEMOSAIC_INPUT;

// Demosaic control output.
typedef struct {
	FJ_DEMOSAIC_OUTPUT				initiate_cycle;									// Initiate cycle.
	UINT32							sampling_count;									// Sampling count for cycle shot(1/n).
	FJ_DEMOSAIC_OUTPUT_IMG_PFMT		image_format;									// Image format
} T_BF_INSTANCE_CONTROL_DEMOSAIC_OUTPUT;

// Demosaic control.
typedef struct {
	T_BF_INSTANCE_CONTROL_DEMOSAIC_INPUT	input;										// Input control.
	T_BF_INSTANCE_CONTROL_DEMOSAIC_OUTPUT	output[D_BF_INSTANCE_OUTPUT_MAX_DEMOSAIC];	// Output control.
} T_BF_INSTANCE_CONTROL_DEMOSAIC;

// Jpeg quality.
typedef struct {
	FJ_JPEG_QUALITY_KIND			kind;							// Kind.
	USHORT							operand;						// Operand.
} T_BF_INSTANCE_JPEG_QUALITY;

// SEN DZoom /w sensor mode change config
typedef struct {
	UINT32	pre_frame_rate;
	UINT32	post_frame_rate;
} T_BF_INSTANCE_SEN_DZOOM_CONFIG;

// Sensor instance.
typedef struct {
	UCHAR							sensor_id;						// Sensor ID.
	T_BF_INSTANCE_CONNECT_SENSOR	connect;						// Connection setting.
	T_BF_INSTANCE_WINDOW_SENSOR		window;							// Window setting.
} T_BF_INSTANCE_SENSOR;

// SEN instance.
typedef struct {
	E_BF_INSTANCE_FLAG				flag;							// Use flag.
	FJ_SEN_ENUM						type;							// Instance type.
	E_BF_INSTANCE_STATE				state;							// Instance state.
	T_BF_INSTANCE_CONNECT_SEN		connect;						// Connection setting.
	T_BF_INSTANCE_WINDOW			window;							// Window setting.
	FJ_STILL_SEN_CONFIG				sen_config;						// SEN config.
	T_BF_INSTANCE_SEN_DZOOM_CONFIG	dzoom_config;					// SEN DZoom /w sensor mode change config
	FJ_STILL_RAW_WINDOW_CONFIG		raw_window;						// Raw window information
} T_BF_INSTANCE_SEN;

// Demosaic instance.
typedef struct {
	E_BF_INSTANCE_FLAG				flag;							// Use flag.
	FJ_DEMOSAIC_ENUM				type;							// Instance type.
	E_BF_INSTANCE_STATE				state;							// Instance state.
	T_BF_INSTANCE_CONNECT_DEMOSAIC	connect;						// Connection setting.
	T_BF_INSTANCE_WINDOW_DEMOSAIC	window;							// Window setting.
	T_BF_INSTANCE_CONTROL_DEMOSAIC	control;						// Control setting.
	FJ_STILL_3DNR_CONFIG 			dnr_config;						// 3DNR config.
	BOOL							dnr_enabled;					// 3DNR enable.
	BOOL							ltm_enabled;					// LTM enable.
	FJ_STILL_SHDR_CONFIG			shdr_config;					// SHDR config.
	BOOL							shdr_enabled;					// SHDR enable.
	BOOL							ahip_enabled;					// AHIP enable.
	BOOL							mirror_enabled;					// Demosaic mirror enable.
	BOOL							flip_enabled;					// Demosaic flip enable.
	BOOL							warp_enabled;					// Warping enable.
	FJ_STILL_RAW_SCALER_SEL			raw_scaler_sel;					// Raw Scaler enable.
} T_BF_INSTANCE_DEMOSAIC;

// Video encode instance.
typedef struct {
	E_BF_INSTANCE_FLAG				flag;							// Use flag.
	E_FJ_VIDEO_ENC_ENUM				type;							// Instance type.
	E_BF_INSTANCE_STATE				state;							// Instance state.
	T_BF_INSTANCE_WINDOW			window;							// Window setting.
} T_BF_INSTANCE_VIDEO_ENCODE;

// Video decode instance.
typedef struct {
	E_BF_INSTANCE_FLAG				flag;							// Use flag.
	E_FJ_VIDEO_DEC_ENUM				type;							// Instance type.
	E_BF_INSTANCE_STATE				state;							// Instance state.
	T_BF_INSTANCE_CONNECT_VIDEO_DEC	connect;						// Connection setting.
} T_BF_INSTANCE_VIDEO_DECODE;

// Still encode instance.
typedef struct {
	E_BF_INSTANCE_FLAG				flag;							// Use flag.
	E_FJ_STILL_ENC_ENUM				type;							// Instance type.
	E_BF_INSTANCE_STATE				state;							// Instance state.
	T_BF_INSTANCE_WINDOW			window[3];						// Window setting(main/screen/thumb).
	T_BF_INSTANCE_JPEG_QUALITY		quality;						// Jpeg quality.
	BOOL							enabled;						// Still encode enable.
} T_BF_INSTANCE_STILL_ENCODE;

// Display instance.
typedef struct {
	E_BF_INSTANCE_FLAG				flag;							// Use flag.
	FJ_DISP_TYPE					type;							// Instance type.
	E_BF_INSTANCE_STATE				state;							// Instance state.
	E_BF_INSTANCE_CONNECT_STATE		connect_state;					// LCD/HDMI Connect state.
	FJ_DISP_ROTATE_DEGREE			rotate_degree;					// Rotate degree. (clockwise)
	T_BF_INSTANCE_WINDOW_DISPLAY	window;							// Window setting.
} T_BF_INSTANCE_DISPLAY;

// Stitch instance.
typedef struct {
	E_BF_INSTANCE_FLAG				flag;							// Use flag.
	FJ_STITCH_ENUM					type;							// Instance type.
	E_BF_INSTANCE_STATE				state;							// Instance state.
	T_BF_INSTANCE_CONNECT_STITCH	connect;						// Connection setting.
	T_BF_INSTANCE_WINDOW_STITCH		window;							// Window setting.
} T_BF_INSTANCE_STITCH;

// AudioSRC instance.
typedef struct {
	E_BF_INSTANCE_FLAG				flag;							// Use flag.
	E_FJ_AUDIO_SRC_ENUM				type;							// Instance type.
	E_BF_INSTANCE_STATE				state;							// Instance state.
} T_BF_INSTANCE_AUDIO_SRC;

// AudioCapture instance.
typedef struct {
	E_BF_INSTANCE_FLAG					flag;						// Use flag.
	E_FJ_AUDIO_CAPTURE_ENUM				type;						// Instance type.
	E_BF_INSTANCE_STATE					state;						// Instance state.
	T_BF_INSTANCE_CONNECT_AUDIO_CAP		connect;					// Connection setting.
} T_BF_INSTANCE_AUDIO_CAP;

// AudioOut instance.
typedef struct {
	E_BF_INSTANCE_FLAG					flag;						// Use flag.
	E_FJ_AUDIO_OUT_ENUM					type;						// Instance type.
	E_BF_INSTANCE_STATE					state;						// Instance state.
} T_BF_INSTANCE_AUDIO_OUT;

// AudioEncoder instance.
typedef struct {
	E_BF_INSTANCE_FLAG					flag;						// Use flag.
	E_FJ_AUDIO_ENC_ENUM					type;						// Instance type.
	E_BF_INSTANCE_STATE					state;						// Instance state.
} T_BF_INSTANCE_AUDIO_ENC;

// AudioDecoder instance.
typedef struct {
	E_BF_INSTANCE_FLAG					flag;						// Use flag.
	E_FJ_AUDIO_DEC_ENUM					type;						// Instance type.
	E_BF_INSTANCE_STATE					state;						// Instance state.
	T_BF_INSTANCE_CONNECT_AUDIO_DEC		connect;					// Connection setting.
} T_BF_INSTANCE_AUDIO_DEC;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
// Still
extern T_BF_INSTANCE_DEMOSAIC*		BF_Instance_Get_Demosaic( UCHAR demosaic_id );
extern T_BF_INSTANCE_STILL_ENCODE*	BF_Instance_Get_StillEncode( UCHAR still_encode_id );
extern T_BF_INSTANCE_STITCH*		BF_Instance_Get_Stitch( UCHAR stitch_id );

extern VOID BF_Instance_Get_Sen_Id_Info( UCHAR* sen_id_cnt, UCHAR* sen_id_array_pointer );
extern UCHAR BF_Instance_Get_Sensor_Id_From_Sen( UCHAR sen_id );

// Movie
extern T_BF_INSTANCE_VIDEO_ENCODE*	BF_Instance_Get_VideoEncode( UCHAR video_encode_id );
extern T_BF_INSTANCE_VIDEO_DECODE*	BF_Instance_Get_VideoDecode( UCHAR video_decode_id );

// Display
extern T_BF_INSTANCE_DISPLAY*		BF_Instance_Get_Display( UCHAR disp_id );

// Audio
extern T_BF_INSTANCE_AUDIO_SRC*	BF_Instance_Get_AudioSrc( UCHAR audio_src_id );
extern T_BF_INSTANCE_AUDIO_CAP*	BF_Instance_Get_AudioCapture( UCHAR audio_capture_id );
extern T_BF_INSTANCE_AUDIO_OUT*	BF_Instance_Get_AudioOut( UCHAR audio_out_id );
extern T_BF_INSTANCE_AUDIO_ENC*	BF_Instance_Get_AudioEncoder( UCHAR audio_enc_id );
extern T_BF_INSTANCE_AUDIO_DEC*	BF_Instance_Get_AudioDecoder( UCHAR audio_dec_id );


#endif	// _INSTANCE_H_
