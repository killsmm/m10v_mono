/**
 * @file		fj_iq.h
 * @brief		Image Quality API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_IQ_H_
#define _FJ_IQ_H_

#include "fj_custom.h"
#include "fj_calibration.h"



///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_iq_overview
 *	@{
 *	This chapter describes API functions and other API issues related to Image Quality(IQ)
 *	of the DSC system. IQ API is able to improvement and the color adjustment of the
 *	image denoising and the resolution.<br>
 *	Set each IQ before changing to each mode (e.g. Video, Photo).<br>
 *	This setting is reflected by the change of mode.
 *	@}
 */


///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_iq_sequence
@{
- @ref fj_iq_sequence_initialize
- @ref fj_iq_sequence_iq_mode_change_for_preview_mode
- @ref fj_iq_sequence_iq_param_change_in_preview_mode
- @ref fj_iq_sequence_iq_mode_change_for_capture_mode
- @ref fj_iq_sequence_cc0_matrix_change_in_preview_mode
<br>
@section fj_iq_sequence_initialize	IQ Initialize
@image html fj_iq_sequence_initialize.png
@section fj_iq_sequence_iq_mode_change_for_preview_mode	IQ parameter select for Video mode
@image html fj_iq_sequence_iq_select_preview_param.png
@section fj_iq_sequence_iq_param_change_in_preview_mode	IQ parameter change in Video mode
@image html fj_iq_sequence_iq_mode_change.png
@section fj_iq_sequence_iq_mode_change_for_capture_mode	IQ parameter select for Photo mode
@image html fj_iq_sequence_iq_select_capture_param.png
@section fj_iq_sequence_cc0_matrix_change_in_preview_mode	CC0 matrix change in Video mode
@image html fj_iq_r2y_cc0_change.png
@} */



/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
/** @weakgroup fj_iq_definition
@{
@section fj_iq_iq_definition			Image Quality definitions
- @ref E_IQ_SCENE
- @ref E_FJ_IQ_BIN_TYPE
- @ref T_IQ_PRE_CNR_CC0_CTRL

@section fj_iq_badpix_definition		Bad Pixel calibration/compensation definitions
- @ref E_IQ_DETECT_ALGORITHM
- @ref T_CORNER_THRESHOLD_INFO
- @ref FJ_IQ_CORNER_THRESHOLD
*/

/**	IQ Still mode
 */
typedef enum {
	E_IQ_STILL_MODE_ERR = -1,		/**< API is not prepared. */
	E_IQ_STILL_MODE_IDLE = -1,		/**< API is not prepared. */
	E_IQ_STILL_MODE_TOP = 0,		/**< Top(minimum) value */
	E_IQ_STILL_MODE_0 = 0,			/**< mode0 */
	E_IQ_STILL_MODE_1,				/**< mode1 */
	E_IQ_STILL_MODE_2,				/**< mode2 */
	E_IQ_STILL_MODE_3,				/**< mode3 */
	E_IQ_STILL_MODE_4,				/**< mode4 */
	E_IQ_STILL_MODE_5,				/**< mode5 */
	E_IQ_STILL_MODE_6,				/**< mode6 */
	E_IQ_STILL_MODE_7,				/**< mode7 */
	E_IQ_STILL_MODE_8,				/**< mode8 */
	E_IQ_STILL_MODE_MAX
} E_IQ_STILL_MODE;

/**	IQ Scene mode
 */
typedef enum {
	E_IQ_SCENE_ERR = -1,	/**< API is not prepared. */
	E_IQ_SCENE_SYS_DEFAULT = -2,
	E_IQ_SCENE_TOP = 0,		/**< TOP of E_IQ_SCENE */
	E_IQ_SCENE_0 = 0,		/**< Scene No 0 */
	E_IQ_SCENE_1,			/**< Scene No 1 */
	E_IQ_SCENE_2,			/**< Scene No 2 */
	E_IQ_SCENE_3,			/**< Scene No 3 */
	E_IQ_SCENE_4,			/**< Scene No 4 */
	E_IQ_SCENE_5,			/**< Scene No 5 */
	E_IQ_SCENE_6,			/**< Scene No 6 */
	E_IQ_SCENE_7,			/**< Scene No 7 */
	E_IQ_SCENE_8,			/**< Scene No 8 */
	E_IQ_SCENE_9,			/**< Scene No 9 */
	E_IQ_SCENE_10,			/**< Scene No 10 */
	E_IQ_SCENE_11,			/**< Scene No 11 */
#if defined(CO_IQBIN_GENERAL) // --- REMOVE_IQBIN_GENERAL BEGIN ---
	E_IQ_SCENE_12,			/**< Scene No 12 */
	E_IQ_SCENE_13,			/**< Scene No 13 */
	E_IQ_SCENE_14,			/**< Scene No 14 */
	E_IQ_SCENE_15,			/**< Scene No 15 */
#endif // CO_IQBIN_GENERAL  --- REMOVE_IQBIN_GENERAL END ---
	E_IQ_SCENE_MAX
} E_IQ_SCENE;

/**	IQ binary type
 */
typedef enum {
	E_FJ_IQ_BIN_TYPE_TOP = E_IQ_STILL_MODE_TOP,			/**< Top(minimum) value */
	E_FJ_IQ_BIN_TYPE__0  = E_IQ_STILL_MODE_0,			/**< Mode0 parameter */
	E_FJ_IQ_BIN_TYPE__1  = E_IQ_STILL_MODE_1,			/**< Mode1 parameter */
	E_FJ_IQ_BIN_TYPE__2  = E_IQ_STILL_MODE_2,			/**< Mode2 parameter */
	E_FJ_IQ_BIN_TYPE__3  = E_IQ_STILL_MODE_3,			/**< Mode3 parameter */
	E_FJ_IQ_BIN_TYPE__4  = E_IQ_STILL_MODE_4,			/**< Mode4 parameter */
	E_FJ_IQ_BIN_TYPE__5  = E_IQ_STILL_MODE_5,			/**< Mode5 parameter */
	E_FJ_IQ_BIN_TYPE__6  = E_IQ_STILL_MODE_6,			/**< Mode6 parameter */
	E_FJ_IQ_BIN_TYPE__7  = E_IQ_STILL_MODE_7,			/**< Mode7 parameter */
	E_FJ_IQ_BIN_TYPE__8  = E_IQ_STILL_MODE_8,			/**< Mode8 parameter */
	E_FJ_IQ_BIN_TYPE_COMMON,											/**< Common parameter */
	E_FJ_IQ_BIN_TYPE_MAX
} E_FJ_IQ_BIN_TYPE;

/** IQ On/Off value
 */
typedef enum {
	E_IQ_ENABLE_OFF,	/**< Off */
	E_IQ_ENABLE_ON		/**< On */
} E_IQ_ENABLE;


/*==============================================================================*/
/* Enumeration(Bad Pixel Calibration)											*/
/*==============================================================================*/

/**	Bad pixel Calibration algorithm.<br>
 * Please refer detail FJ_IQ_CalibrationCreateBadpixTable().
 */
typedef enum {
	E_IQ_DETECT_ALGO_0,			/**< Algorithm 0 */
	E_IQ_DETECT_ALGO_1,			/**< Algorithm 1 */
	E_IQ_DETECT_ALGO_2,			/**< Algorithm 2 */
	E_IQ_DETECT_ALGO_3,			/**< Algorithm 3 */
	E_IQ_DETECT_ALGO_MAX
} E_IQ_DETECT_ALGORITHM;

/** Bad pixel line type.
 */
typedef enum {
	FJ_IQ_BADPIX_LINE_EVEN = 0,						/**< Even line. */
	FJ_IQ_BADPIX_LINE_ODD,							/**< Odd line. */
	FJ_IQ_BADPIX_LINE_MAX,							/**< Maximum value of @ref FJ_IQ_BADPIX_LINE. */
} FJ_IQ_BADPIX_LINE;

/** Bad pixel column type.
 */
typedef enum {
	FJ_IQ_BADPIX_COLUM_EVEN = 0,					/**< Even column. */
	FJ_IQ_BADPIX_COLUM_ODD,							/**< Odd column. */
	FJ_IQ_BADPIX_COLUM_MAX,							/**< Maximum value of @ref FJ_IQ_BADPIX_COLUM. */
} FJ_IQ_BADPIX_COLUM;


/*==============================================================================*/
/* Structure(Image Quality)														*/
/*==============================================================================*/

/**
 * IQ parameter selection information.
 */
typedef struct {
	E_IQ_STILL_MODE	still_mode;			/**< Still mode. (Select IQ binary.) */
	FLOAT			iso_idx;			/**< ISO number. (IQ parameter select index in IQ binary. 0.0~5.0(ISO100~ISO3200)) */
	E_IQ_SCENE		scene_no;			/**< Scene number. (IQ parameter select index in IQ binary.) */
	E_IQ_SCENE		scene_no_for_sharpness;	/**< Scene number. (IQ parameter select index in IQ binary.) */
	E_IQ_SCENE		scene_no_for_edge;		/**< Scene number. (IQ parameter select index in IQ binary.) */
#if defined(CO_IQBIN_GENERAL)	// --- REMOVE_IQBIN_GENERAL BEGIN ---
	UINT32			ozoom_idx;			/**< Optical zoom index. (IQ parameter select index in IQ binary. 0~9) */
	UINT32			lens_type_idx;		/**< Lens type index. (IQ parameter select index in IQ binary. 0~3) */
	FLOAT			color_temp_idx;		/**< Color temperature. (IQ parameter select index in IQ binary. 0.0~12.0(2000~10000)) */
	FLOAT			exposure_value_idx;	/**< Exposure value. (IQ parameter select index in IQ binary. 0.0~22.0(-5.0~16.0)) */
	FLOAT			time_value_idx;		/**< Time value. (IQ parameter select index in IQ binary. 0.0~18.0(-5.0~12.0)) */
#endif // CO_IQBIN_xxxx  --- REMOVE_IQBIN_GENERAL END ---
} T_FJ_IQ_SEL_INFO;

/**	CC Matrix control
 */
typedef struct {
	UCHAR				posi_dec;			/**< Position of decimals			*/ // @CCCTL
	USHORT				cc_matrix[3][3];	/**< CC-Matrix coefficient 3x3		*/ // @CC00~CC22
} T_IQ_PRE_CNR_CC0_CTRL;					/* CC0-Matrix control							*/

/**
 * Indivisual gamma table
 */
typedef struct {
	const USHORT* full;		/**< Full table. (256 elements array.) */
	const ULLONG* diff;		/**< Diff table. (256 elements array.) */
} T_FJ_IQ_GAMMA_TBL;

/**
 * Gamma table sets.
 */
typedef struct {
	T_FJ_IQ_GAMMA_TBL	rgb_tbl;	/**< RGB table. */
	T_FJ_IQ_GAMMA_TBL	r_tbl;		/**< R table. */
	T_FJ_IQ_GAMMA_TBL	g_tbl;		/**< G table. */
	T_FJ_IQ_GAMMA_TBL	b_tbl;		/**< B table. */
	T_FJ_IQ_GAMMA_TBL	yb_tbl;		/**< Yb table. */
	UCHAR				gamma_mode;	/**< Gamma mode. */
} T_FJ_IQ_GAMMA;

/** YC Convert control.
*/
typedef struct {
	SHORT				yc_coeff[3][3];		/**< YC : Coefficient of YC convert (9bits signed)			*/
	UCHAR				y_blend_ratio;		/**< YBLEND.YYBLND : Y blend ratio (6bits 0 ~ 32)			*/
	UCHAR				yb_blend_ratio;		/**< YBLEND.YBBLND : Yb blend ratio (6bits 0 ~ 32)			*/
} T_FJ_IQ_YCC;


/*==============================================================================*/
/* Structure(Dead Pixel Compensation)											*/
/*==============================================================================*/

/** Cornaer Threshold information structure
 */
typedef struct {
	USHORT us_h;							/**< horizontal threshold position */
	USHORT us_v;							/**< vertical threshold position */
	USHORT us_threshold_high;				/**< high threshold */
	USHORT us_threshold_low;				/**< low threshold */
} T_CORNER_THRESHOLD_INFO;

/** Cornaer Threshold parameters structure
 */
typedef struct {
	T_CORNER_THRESHOLD_INFO t_left_top;		/**< left top corner threshold info */
	T_CORNER_THRESHOLD_INFO t_right_top;	/**< right top corner threshold info */
	T_CORNER_THRESHOLD_INFO t_left_bottom;	/**< left bottom corner threshold info */
	T_CORNER_THRESHOLD_INFO t_right_bottom;	/**< right bottom corner threshold info */
} FJ_IQ_CORNER_THRESHOLD;

/** Pixel position
 */
typedef struct {
	SHORT x_pos;									/**< Position X. */
	SHORT y_pos;									/**< Position Y. */
} FJ_IQ_BADPIX_POS;

/** Rectangle
 */
typedef struct {
	USHORT width;									/**< Width. */
	USHORT lines;									/**< Lines. */
} FJ_IQ_BADPIX_RECT;

/** Bad pixel additional pixel position
 */
typedef struct {
	USHORT	y;										/**< Y position. */
	USHORT	x[20];									/**< X position(s). */
	USHORT	x_num;									/**< @ref x valid number. */
} FJ_IQ_BADPIX_ADD_POS;

/** Bad pixel additional pattern.
 */
typedef struct {
	FJ_IQ_BADPIX_ADD_POS	add_pos[20];			/**< Additional pixel position. */
	USHORT					add_pos_num;			/**< @ref add_pos valid number. */
	FJ_IQ_BADPIX_POS		cycle_start_pos;		/**< Additional pattern cycle start position. */
	FJ_IQ_BADPIX_RECT		cycle_rect;				/**< Additional pattern size. */
} FJ_IQ_BADPIX_ADD_PAT;

/** Bad pixel table convert parameter.
 */
typedef struct {
	FJ_IQ_BADPIX_ADD_PAT	add_pat[FJ_IQ_BADPIX_LINE_MAX][FJ_IQ_BADPIX_COLUM_MAX];	/**< Pixel additional pattern data. */
	FJ_IQ_BADPIX_POS		crop_pos_src;			/**< Cropping window start position. Must even number. */
	FJ_IQ_BADPIX_RECT		crop_rect_src;			/**< Cropping window size. Must even number. */
	FJ_IQ_BADPIX_POS		sen_window_pos_src;		/**< SEN window start position of source table. Must even number. */
	FJ_IQ_BADPIX_POS		sen_window_pos_dst;		/**< SEN window start position of destination table Must even number. */
} FJ_IQ_BADPIX_CONV_INFO;

/** Badpix table flip position parameter. (Parameter structure of FJ_IQ_Flip_BadpixTable().)
 */
typedef struct {
	FJ_IQ_BADPIX_POS		flip_pos_dst;			/**< After left-top offset position from VD and HD edge. Must even number. */
} FJ_IQ_BADPIX_FLIP_INFO;


/*==============================================================================*/
/* Structure(Shading Compensation)												*/
/*==============================================================================*/

/** Frame shading table conversion information from capture mode to view mode.<br>
*/
typedef struct {
	USHORT src_tbl_sta_x;						/**< source start left position (RAW pixels) */
	USHORT src_tbl_sta_y;						/**< source start top position (RAW pixels) */
	USHORT src_tbl_width;						/**< source width (RAW pixels) */
	USHORT src_tbl_lines;						/**< source lines (RAW pixels) */
	USHORT dst_tbl_width;						/**< destination width (RAW pixels) */
	USHORT dst_tbl_lines;						/**< destination lines (RAW pixels) */
} T_FJ_IQ_FSHD_CONVINFO;

/** Frame shading table selection information.<br>
*/
typedef struct {
	E_IQ_CALIB_TGT	calib_tgt;					/**< Calibration target.										*/
	USHORT			ozoom_idx;					/**< Optical zoom index number. (range:0x0000-0xFFFE)			*/
	USHORT			f_number;					/**< F number(integer 8bit, decimal 8bit) (range:0x0000-0xFFFE)	*/
} T_FJ_IQ_FSHD_SEL_INFO;

/** Frame shading saturation correction value. (slope gain) */
typedef struct {
	BOOL	enable;			/**< FSHCEN register value. */
	USHORT	r;				/**< FSSLPRR register value. */
	USHORT	gr;				/**< FSSLPGR register value. */
	USHORT	gb;				/**< FSSLPGB register value. */
	USHORT	b;				/**< FSSLPBB register value. */
} T_FJ_IQ_FSHD_SLP_GAIN;


/*==============================================================================*/
/* Structure(Shading L0 Compensation)											*/
/*==============================================================================*/

/** Frame shading L0 table information.<br>
*/
typedef struct {
	UCHAR	ffshgfmt;		/**< FFSHGFMT register value. */
	USHORT	ffshsubprh;		/**< FFSHSUBPRH register value. */
	USHORT	ffshsubprv;		/**< FFSHSUBPRV register value. */
	ULONG	tbl_addr[4];	/**< FSHDL table memory area address. [0]:R [1]:GR [2]:GB [3]:B */
	ULONG	tbl_g_width[4];	/**< FSHDL table global width bytes. [0]:R [1]:GR [2]:GB [3]:B */
} T_FJ_IQ_FSHDL_TBL;

/** Frame shading L0 parameter.<br>
*/
typedef struct {
	USHORT	ffshsubsph;		/**< FFSHSUBSPH register value. */
	USHORT	ffshsubspv;		/**< FFSHSUBSPV register value. */
	USHORT	ffshsubsth;		/**< FFSHSUBSTH register value. */
	USHORT	ffshsubstv;		/**< FFSHSUBSTV register value. */
} T_FJ_IQ_FSHDL_PARAM;

/** Frame shading L0 saturation correction value. (slope gain) */
typedef struct {
	BOOL	enable;			/**< FFSHCEN register value. */
	USHORT	r;				/**< FFSHLPRR register value. */
	USHORT	gr;				/**< FFSHLPGR register value. */
	USHORT	gb;				/**< FFSHLPGB register value. */
	USHORT	b;				/**< FFSHLPBB register value. */
} T_FJ_IQ_FSHDL_SLP_GAIN;


/*==============================================================================*/
/* Structure(CAG-FFC Compensation)												*/
/*==============================================================================*/

/** CAG-FFC table conversion information from source mode to destination mode.<br>
*/
typedef struct {
	USHORT src_tbl_sta_x;						/**< source start left position (RAW pixels) */
	USHORT src_tbl_sta_y;						/**< source start top position (RAW pixels) */
	USHORT src_tbl_width;						/**< source width (RAW pixels) */
	USHORT src_tbl_lines;						/**< source lines (RAW pixels) */
	USHORT dst_tbl_width;						/**< destination width (RAW pixels) */
	USHORT dst_tbl_lines;						/**< destination lines (RAW pixels) */
} T_FJ_IQ_CAG_CONVINFO;


/* @} */	// fj_iq_definition group


/*==============================================================================*/
/* Global variable declaration													*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/** @weakgroup fj_iq_api_basic
@{
*/

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change IQ Still mode for IQ category parameter selecting for SEN block.
 * Activating new IQ Still mode (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current_Sen() called or system mode changeed.
 *
 * Relativity of IQ Still mode and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	sen_id		: SEN instance ID.
 *	@param[in]	mode		: Next IQ Still mode.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *	@code
#include "fj_iq.h"

VOID change_iq_param( VOID )
{
	FJ_ERR_CODE fj_ercd;
	UCHAR sen_id;

	sen_id = 0;
	fj_ercd = FJ_IQ_Set_Next_Still_Mode_Sen( sen_id, E_IQ_STILL_MODE_2 );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Set_Next_IsoSpeed_Idx_Sen( sen_id, 0.0 );	// ISO100
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Set_Next_Scene_No_Sen( sen_id, E_IQ_SCENE_1 );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Update_Current_Sen( sen_id );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Set_Next_Still_Mode_Sen( UCHAR sen_id, E_IQ_STILL_MODE mode );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next IQ Still mode for SEN block.
 *
 *	@param[in]	sen_id		: SEN instance ID.
 *	@param[out]	curr_mode : Current IQ Still mode.
 *	@param[out]	next_mode : Next IQ Still mode.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *	@code
#include "fj_iq.h"

VOID get_iq_setting_sen( VOID )
{
	FJ_ERR_CODE fj_ercd;
	E_IQ_STILL_MODE curr_mode;
	E_IQ_STILL_MODE next_mode;
	FLOAT curr_iso_idx;
	FLOAT next_iso_idx;
	E_IQ_SCENE curr_scene;
	E_IQ_SCENE next_scene;
	UCHAR sen_id;

	sen_id = 0;
	fj_ercd = FJ_IQ_Get_Still_Mode_Sen( sen_id, &curr_mode, &next_mode );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Get_IsoSpeed_Idx_Sen( sen_id, &curr_iso_idx, &next_iso_idx );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Get_Scene_No_Sen( sen_id, &curr_scene, &next_scene );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Get_Still_Mode_Sen( UCHAR sen_id, E_IQ_STILL_MODE* curr_mode, E_IQ_STILL_MODE* next_mode );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change ISO speed axis index for IQ parameter selecting in IQ category parameter for SEN block.
 * Activating new ISO speed axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current_Sen() called or system mode changeed.
 *
 * Relativity of ISO speed axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	sen_id		: SEN instance ID.
 *	@param[in]	iso_idx		: Next ISO speed axis index. 0.0~5.0(ISO100~ISO3200).
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode_Sen().
 */
extern FJ_ERR_CODE	FJ_IQ_Set_Next_IsoSpeed_Idx_Sen( UCHAR sen_id, FLOAT iso_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next ISO speed axis index for SEN block.
 *
 *	@param[in]	sen_id			: SEN instance ID.
 *	@param[out]	curr_iso_idx	: Current ISO speed axis index.
 *	@param[out]	next_iso_idx	: Next ISO speed axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode_Sen().
 */
extern FJ_ERR_CODE	FJ_IQ_Get_IsoSpeed_Idx_Sen( UCHAR sen_id, FLOAT* curr_iso_idx, FLOAT* next_iso_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change Scene No axis index for IQ parameter selecting in IQ category parameter for SEN block.
 * Activating new Scene No axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current_Sen() called or system mode changeed.
 *
 * Relativity of Scene No axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	sen_id		: SEN instance ID.
 *	@param[in]	scene_idx	: Next Scene No axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode_Sen().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No_Sen( UCHAR sen_id, E_IQ_SCENE scene_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change Scene No axis index for IQ parameter selecting in IQ category parameter for SEN block.
 * Activating new Scene No axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current_Sen() called or system mode changeed.
 *
 * Relativity of Scene No axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	sen_id		: SEN instance ID.
 *	@param[in]	scene_idx	: Next Scene No axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode_Sen().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No_For_Sharpness_Sen( UCHAR sen_id, E_IQ_SCENE scene_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change Scene No axis index for IQ parameter selecting in IQ category parameter for SEN block.
 * Activating new Scene No axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current_Sen() called or system mode changeed.
 *
 * Relativity of Scene No axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	sen_id		: SEN instance ID.
 *	@param[in]	scene_idx	: Next Scene No axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode_Sen().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No_For_Edge_Sen( UCHAR sen_id, E_IQ_SCENE scene_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next Scene No axis index for SEN block.
 *
 *	@param[in]	sen_id			: SEN instance ID.
 *	@param[out]	curr_scene_idx	: Current Scene No axis index.
 *	@param[out]	next_scene_idx	: Next Scene No axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode_Sen().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Get_Scene_No_Sen( UCHAR sen_id, E_IQ_SCENE* curr_scene_idx, E_IQ_SCENE* next_scene_idx );

#if defined(CO_IQBIN_GENERAL)	// --- REMOVE_IQBIN_GENERAL BEGIN ---
/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 * Change Color temperature axis index for IQ parameter selecting in IQ binary for SEN block.
 * Activating new Color temperature axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current_Sen() called or system mode changeed.
 *
 * Relativity of Color temperature axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	sen_id			: SEN instance ID.
 *	@param[in]	color_temp_idx	: Next Color temperature axis index. 0.0~12.0(Color temperature 2000~10000)
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode_Sen().
 */
extern FJ_ERR_CODE	FJ_IQ_Set_Next_ColorTemp_Idx_Sen( UCHAR sen_id, FLOAT color_temp_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next Color temperature axis index for SEN block.
 *
 *	@param[in]	sen_id				: SEN instance ID.
 *	@param[out]	curr_color_temp_idx	: Current Color temperature axis index.
 *	@param[out]	next_color_temp_idx	: Next Color temperature axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode_Sen().
 */
extern FJ_ERR_CODE	FJ_IQ_Get_ColorTemp_Idx_Sen( UCHAR sen_id, FLOAT* curr_color_temp_idx, FLOAT* next_color_temp_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change Exposure value index for IQ parameter selecting in IQ category parameter for SEN block.
 * Activating new Exposure value index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current_Sen() called or system mode changeed.
 *
 * Relativity of Exposure value index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	sen_id		: SEN instance ID.
 *	@param[in]	exp_val_idx	: Next Exposure value index. 0.0~22.0(EV -5.0~16.0)
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode_Sen().
 */
extern FJ_ERR_CODE	FJ_IQ_Set_Next_ExposureValue_Idx_Sen( UCHAR sen_id, FLOAT exp_val_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next Exposure value index for SEN block.
 *
 *	@param[in]	sen_id				: SEN instance ID.
 *	@param[out]	curr_exp_val_idx	: Current Exposure value index.
 *	@param[out]	next_exp_val_idx	: Next Exposure value index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode_Sen().
 */
extern FJ_ERR_CODE	FJ_IQ_Get_ExposureValue_Idx_Sen( UCHAR sen_id, FLOAT* curr_exp_val_idx, FLOAT* next_exp_val_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change Time value index for IQ parameter selecting in IQ category parameter for SEN block.
 * Activating new Time value index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current_Sen() called or system mode changeed.
 *
 * Relativity of Time value index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	sen_id			: SEN instance ID.
 *	@param[in]	time_val_idx	: Next Time value index. 0.0~18.0(TV -5.0~12.0)
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode_Sen().
 */
extern FJ_ERR_CODE	FJ_IQ_Set_Next_TimeValue_Idx_Sen( UCHAR sen_id, FLOAT time_val_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next Time value index for SEN block.
 *
 *	@param[in]	sen_id				: SEN instance ID.
 *	@param[out]	curr_time_val_idx	: Current Time value index.
 *	@param[out]	next_time_val_idx	: Next Time value index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode_Sen().
 */
extern FJ_ERR_CODE	FJ_IQ_Get_TimeValue_Idx_Sen( UCHAR sen_id, FLOAT* curr_time_val_idx, FLOAT* next_time_val_idx );

#endif // CO_IQBIN_GENERAL  --- REMOVE_IQBIN_GENERAL END ---

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next IQ Still mode, ISO speed axis index and Scene No for SEN block.
 *
 *	@param[in]	sen_id			: SEN instance ID.
 *	@param[out]	curr_sel_info	: Current selection information.
 *	@param[out]	next_sel_info	: Next selection information.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@retval		FJ_ERR_OK : OK.
 *	@retval		FJ_ERR_NG : NG.
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Get_SelectInfo_Sen( UCHAR sen_id, T_FJ_IQ_SEL_INFO* curr_sel_info, T_FJ_IQ_SEL_INFO* next_sel_info );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Activate next IQ Still mode No, ISO speed axis index and Scene No for SEN block.
 *	IQ paramter value is reflected immediately when image pipeline is active.<br>
 *
 *	@param[in]	sen_id		: SEN instance ID.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode_Sen().
 */
extern FJ_ERR_CODE	FJ_IQ_Update_Current_Sen( UCHAR sen_id );


/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change IQ Still mode for IQ category parameter selecting for Demosaic block.
 * Activating new IQ Still mode (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current() called or system mode changeed.
 *
 * Relativity of IQ Still mode and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID.<br>
 *	@param[in]	mode		: Next IQ Still mode.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *	@code
#include "fj_iq.h"

VOID change_iq_param( VOID )
{
	FJ_ERR_CODE fj_ercd;
	UCHAR demosaic_id;

	demosaic_id = 0;
	fj_ercd = FJ_IQ_Set_Next_Still_Mode( demosaic_id, E_IQ_STILL_MODE_2 );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Set_Next_IsoSpeed_Idx( demosaic_id, 0.0 );	// ISO100
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Set_Next_Scene_No( demosaic_id, E_IQ_SCENE_1 );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Set_Next_OptZoom_Idx( demosaic_id, 3 );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Set_Next_LensType_Idx( demosaic_id, 2 );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Update_Current( demosaic_id );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Set_Next_Still_Mode( UCHAR demosaic_id, E_IQ_STILL_MODE mode );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next IQ Still mode for Demosaic block.
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID.<br>
 *	@param[out]	curr_mode : Current IQ Still mode.
 *	@param[out]	next_mode : Next IQ Still mode.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *	@code
#include "fj_iq.h"

VOID get_iq_setting( VOID )
{
	FJ_ERR_CODE fj_ercd;
	E_IQ_STILL_MODE curr_mode;
	E_IQ_STILL_MODE next_mode;
	FLOAT curr_iso_idx;
	FLOAT next_iso_idx;
	E_IQ_SCENE curr_scene;
	E_IQ_SCENE next_scene;
	UINT32 curr_opt_zoom_idx;
	UINT32 next_opt_zoom_idx;
	UINT32 curr_lens_type_idx;
	UINT32 next_lens_type_idx;
	UCHAR demosaic_id;

	demosaic_id = 0;
	fj_ercd = FJ_IQ_Get_Still_Mode( demosaic_id, &curr_mode, &next_mode );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Get_IsoSpeed_Idx( demosaic_id, &curr_iso_idx, &nn );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Get_Scene_No( demosaic_id, &curr_scene, &next_scene );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Get_OptZoom_Idx( demosaic_id, &curr_opt_zoom_idx, &next_opt_zoom_idx );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Get_LensType_Idx( demosaic_id, &curr_lens_type_idx, &next_lens_type_idx );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Get_Still_Mode( UCHAR demosaic_id, E_IQ_STILL_MODE* curr_mode, E_IQ_STILL_MODE* next_mode );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change ISO speed axis index for IQ parameter selecting in IQ category parameter for demosaic block.
 * Activating new ISO speed axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current() called or system mode changeed.
 *
 * Relativity of ISO speed axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	iso_idx		: Next ISO speed axis index. 0.0~5.0(ISO100~ISO3200).
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Set_Next_IsoSpeed_Idx( UCHAR demosaic_id, FLOAT iso_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next ISO speed axis index for Demosaic block.
 *
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[out]	curr_iso_idx	: Current ISO speed axis index.
 *	@param[out]	next_iso_idx	: Next ISO speed axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Get_IsoSpeed_Idx( UCHAR demosaic_id, FLOAT* curr_iso_idx, FLOAT* next_iso_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change Scene No axis index for IQ parameter selecting in IQ category parameter for Demosaic block.
 * Activating new Scene No axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current() called or system mode changeed.
 *
 * Relativity of Scene No axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	scene_idx	: Next Scene No axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No( UCHAR demosaic_id, E_IQ_SCENE scene_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change Scene No axis index for IQ parameter selecting in IQ category parameter for Demosaic block.
 * Activating new Scene No axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current() called or system mode changeed.
 *
 * Relativity of Scene No axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	scene_idx	: Next Scene No axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No_For_Sharpness( UCHAR demosaic_id, E_IQ_SCENE scene_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change Scene No axis index for IQ parameter selecting in IQ category parameter for Demosaic block.
 * Activating new Scene No axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current() called or system mode changeed.
 *
 * Relativity of Scene No axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	scene_idx	: Next Scene No axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No_For_Edge( UCHAR demosaic_id, E_IQ_SCENE scene_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next Scene No axis index for Demosaic block.
 *
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[out]	curr_scene_idx	: Current Scene No axis index.
 *	@param[out]	next_scene_idx	: Next Scene No axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Get_Scene_No( UCHAR demosaic_id, E_IQ_SCENE* curr_scene_idx, E_IQ_SCENE* next_scene_idx );

#if defined(CO_IQBIN_GENERAL)	// --- REMOVE_IQBIN_GENERAL BEGIN ---
/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 * Change Optical zoom axis index for IQ parameter selecting in IQ binary for Demosaic block.
 * Activating new Optical zoom axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current() called or system mode changeed.
 *
 * Relativity of Optical zoom axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[in]	opt_zoom_idx		: Next Optical zoom axis index. 0~9
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Set_Next_OptZoom_Idx( UCHAR demosaic_id, UINT32 opt_zoom_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next Optical zoom axis index for Demosaic block.
 *
 *	@param[in]	demosaic_id				: Demosaic instance ID.
 *	@param[out]	curr_opt_zoom_idx	: Current Optical zoom axis index.
 *	@param[out]	next_opt_zoom_idx	: Next Optical zoom axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Get_OptZoom_Idx( UCHAR demosaic_id, UINT32* curr_opt_zoom_idx, UINT32* next_opt_zoom_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 * Change Lens type axis index for IQ parameter selecting in IQ binary for Demosaic block.
 * Activating new Lens type axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current() called or system mode changeed.
 *
 * Relativity of Lens type axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[in]	lens_type_idx		: Next Lens type axis index. 0~9
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Set_Next_LensType_Idx( UCHAR demosaic_id, UINT32 lens_type_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next Lens type axis index for Demosaic block.
 *
 *	@param[in]	demosaic_id				: Demosaic instance ID.
 *	@param[out]	curr_lens_type_idx	: Current Lens type axis index.
 *	@param[out]	next_lens_type_idx	: Next Lens type axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Get_LensType_Idx( UCHAR demosaic_id, UINT32* curr_lens_type_idx, UINT32* next_lens_type_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 * Change Color temperature axis index for IQ parameter selecting in IQ binary for Demosaic block.
 * Activating new Color temperature axis index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current() called or system mode changeed.
 *
 * Relativity of Color temperature axis index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[in]	color_temp_idx	: Next Color temperature axis index. 0.0~12.0(Color temperature 2000~10000)
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Set_Next_ColorTemp_Idx( UCHAR demosaic_id, FLOAT color_temp_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next Color temperature axis index for Demosaic block.
 *
 *	@param[in]	demosaic_id				: Demosaic instance ID.
 *	@param[out]	curr_color_temp_idx	: Current Color temperature axis index.
 *	@param[out]	next_color_temp_idx	: Next Color temperature axis index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Get_ColorTemp_Idx( UCHAR demosaic_id, FLOAT* curr_color_temp_idx, FLOAT* next_color_temp_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change Exposure value index for IQ parameter selecting in IQ category parameter for Demosaic block.
 * Activating new Exposure value index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current() called or system mode changeed.
 *
 * Relativity of Exposure value index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	exp_val_idx	: Next Exposure value index. 0.0~22.0(EV -5.0~16.0)
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Set_Next_ExposureValue_Idx( UCHAR demosaic_id, FLOAT exp_val_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next Exposure value index for Demosaic block.
 *
 *	@param[in]	demosaic_id				: Demosaic instance ID.
 *	@param[out]	curr_exp_val_idx	: Current Exposure value index.
 *	@param[out]	next_exp_val_idx	: Next Exposure value index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Get_ExposureValue_Idx( UCHAR demosaic_id, FLOAT* curr_exp_val_idx, FLOAT* next_exp_val_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Change Time value index for IQ parameter selecting in IQ category parameter for Demosaic block.
 * Activating new Time value index (and new IQ parameter set to Milbeaut),
 * FJ_IQ_Update_Current() called or system mode changeed.
 *
 * Relativity of Time value index and IQ binary, Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[in]	time_val_idx	: Next Time value index. 0.0~18.0(TV -5.0~12.0)
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Set_Next_TimeValue_Idx( UCHAR demosaic_id, FLOAT time_val_idx );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next Time value index for Demosaic block.
 *
 *	@param[in]	demosaic_id				: Demosaic instance ID.
 *	@param[out]	curr_time_val_idx	: Current Time value index.
 *	@param[out]	next_time_val_idx	: Next Time value index.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Get_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Get_TimeValue_Idx( UCHAR demosaic_id, FLOAT* curr_time_val_idx, FLOAT* next_time_val_idx );
#endif // CO_IQBIN_xxxx  --- REMOVE_IQBIN_GENERAL END ---

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get current and next IQ Still mode, ISO speed axis index, Scene No, Optical zoom axis index and Lens type axis index for Demosaic block.
 *
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[out]	curr_sel_info	: Current selection information.
 *	@param[out]	next_sel_info	: Next selection information.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@retval		FJ_ERR_OK : OK.
 *	@retval		FJ_ERR_NG : NG.
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Get_SelectInfo( UCHAR demosaic_id, T_FJ_IQ_SEL_INFO* curr_sel_info, T_FJ_IQ_SEL_INFO* next_sel_info );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Activate next IQ Still mode No, ISO speed axis index, Scene No, Optical zoom axis index and Lens type axis index for Demosaic block.
 *	IQ paramter value is reflected immediately when image pipeline is active.<br>
 *
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_Next_Still_Mode().
 */
extern FJ_ERR_CODE	FJ_IQ_Update_Current( UCHAR demosaic_id );


/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				Set Color Correction 0(CC0) Matrix Coefficient parameters.
 *
 * 				The Color Correction 0(CC0) Matrix Coefficient parameters, Refer "F_R2Y6A Macro Specification" for detail.
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID.<br>
 *	@param[in]	pre_cnr_cc0_ctrl	: CC0 Matrix Coefficient parameters.
 *	@par Sample code:
 *	@code
#include <fj_iq.h>

FJ_ERR_CODE fj_ercd;
T_IQ_PRE_CNR_CC0_CTRL pre_cnr_cc0_ctrl;
UCHAR demosaic_id;

demosaic_id = 0;
pre_cnr_cc0_ctrl.posi_dec         = 0;		// 512
pre_cnr_cc0_ctrl.cc_matrix[0][0]  = 0x0499;	// CC00 =  2.30
pre_cnr_cc0_ctrl.cc_matrix[0][1]  = 0xfd67;	// CC01 = -1.30
pre_cnr_cc0_ctrl.cc_matrix[0][2]  = 0x0000;	// CC02 =  0.00
pre_cnr_cc0_ctrl.cc_matrix[1][0]  = 0xff66;	// CC10 = -0.30
pre_cnr_cc0_ctrl.cc_matrix[1][1]  = 0x0367;	// CC11 =  1.70
pre_cnr_cc0_ctrl.cc_matrix[1][2]  = 0xff33;	// CC12 = -0.40
pre_cnr_cc0_ctrl.cc_matrix[2][0]  = 0x0066;	// CC00 =  0.20
pre_cnr_cc0_ctrl.cc_matrix[2][1]  = 0xfdcd;	// CC01 = -1.10
pre_cnr_cc0_ctrl.cc_matrix[2][2]  = 0x03cd;	// CC02 =  1.90
pre_cnr_cc0_ctrl.cyc[0]           =  77;		// CCYC00 =  0.301
pre_cnr_cc0_ctrl.cyc[1]           = 150;		// CCYC01 =  0.586
pre_cnr_cc0_ctrl.cyc[2]           =  29;		// CCYC01 =  0.113

fj_ercd = FJ_IQ_Update_PreCnrCC0( demosaic_id, &pre_cnr_cc0_ctrl );
if( fj_ercd != FJ_ERR_OK ) {
	// error correction code here.
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Update_PreCnrCC0( UCHAR demosaic_id, T_IQ_PRE_CNR_CC0_CTRL* pre_cnr_cc0_ctrl );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				Get current Color Correction (CC) Matrix Coefficient parameters.
 *
 * 				The Color Correction (CC) Matrix Coefficient parameters, Refer "F_R2Y6A Macro Specification" for detail.
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID.<br>
 *	@param[in]	pre_cnr_cc0_ctrl	: CC Matrix Coefficient parameters.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Get_PreCnrCC0( UCHAR demosaic_id, T_IQ_PRE_CNR_CC0_CTRL* pre_cnr_cc0_ctrl );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get Gamma table from IQ binary on BaseFW internal memory area.
 *
 *	@param[out]	gamma_info	: Gamma table.
 *	@param[in]	sel_info	: Parameter selection information.
 *	@retval		FJ_ERR_OK : OK, return current gamma table.
 *	@retval		FJ_ERR_NG : Gamma table not loaded.
 *	@par Sample code:
 *	@code
#include "fj_iq.h"

VOID get_gamma_tbl( const UCHAR demosaic_id, T_FJ_IQ_GAMMA* const gamma_info )
{
	FJ_ERR_CODE			fj_ercd;
	T_FJ_IQ_SEL_INFO	sel_info;

	fj_ercd = FJ_IQ_Get_SelectInfo( demosaic_id, &sel_info, NULL );
	if ( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Get_GammaTbl( gamma_info, &sel_info );
	if ( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Get_GammaTbl( T_FJ_IQ_GAMMA* gamma_info, T_FJ_IQ_SEL_INFO* sel_info );

/** <table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get YC Conversion Matrix Coefficient table from IQ binary on BaseFW internal memory area.
 *
 * 	The YC Conversion Matrix Coefficient table, Refer "F_R2Y6A Macro Specification" for detail.
 *
 *	@param[out]	ycc_info	: YC Conversion Matrix Cefficient table.
 *	@param[in]	sel_info	: Parameter selection information.
 *	@retval		FJ_ERR_OK : OK, return current gamma table.
 *	@retval		FJ_ERR_NG : IQ binary not loaded.
 *	@par Sample code:
 *	@code
#include "fj_iq.h"

VOID get_ycc_tbl( const UCHAR demosaic_id, T_FJ_IQ_YCC* const ycc_info )
{
	FJ_ERR_CODE			fj_ercd;
	T_FJ_IQ_SEL_INFO	sel_info;

	fj_ercd = FJ_IQ_Get_SelectInfo( demosaic_id, &sel_info, NULL );
	if ( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Get_YccTbl( ycc_info, &sel_info );
	if ( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE	FJ_IQ_Get_YccTbl( T_FJ_IQ_YCC* ycc_info, T_FJ_IQ_SEL_INFO* sel_info );

/** <table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get enable/disable state of ELA from IQ binary on BaseFW internal memory area.<br>
 *
 *	@param[out]	on_off : TRUE:on FALSE:off
 *	@param[in]	sel_info : Parameter selection information.
 *	@retval		FJ_ERR_OK : OK.
 *	@retval		FJ_ERR_NG : NG.
 *	@par Sample code:
 *	@code
#include "fj_iq.h"

BOOL get_ela_onoff( const UCHAR demosaic_id )
{
	FJ_ERR_CODE			fj_ercd;
	T_FJ_IQ_SEL_INFO	sel_info;
	BOOL				onoff;

	fj_ercd = FJ_IQ_Get_SelectInfo( demosaic_id, &sel_info, NULL );
	if ( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Get_Ela_OnOff( &onoff, &sel_info );
	if ( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	return onoff;
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Get_Ela_OnOff( BOOL* on_off, T_FJ_IQ_SEL_INFO* sel_info );

/** <table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get enable/disable state of CNR from IQ binary on BaseFW internal memory area.<br>
 *
 *	@param[out]	on_off		: TRUE:on FALSE:off
 *	@param[in]	sel_info	: Parameter selection information.
 *	@retval		FJ_ERR_OK : OK.
 *	@retval		FJ_ERR_NG : NG.
 *	@par Sample code:
 *	@code
#include "fj_iq.h"

BOOL get_cnr_onoff( const UCHAR demosaic_id )
{
	FJ_ERR_CODE			fj_ercd;
	T_FJ_IQ_SEL_INFO	sel_info;
	BOOL				onoff;

	fj_ercd = FJ_IQ_Get_SelectInfo( demosaic_id, &sel_info, NULL );
	if ( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	fj_ercd = FJ_IQ_Get_Cnr_OnOff( &onoff, &sel_info );
	if ( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}

	return onoff;
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Get_Cnr_OnOff( BOOL* on_off, T_FJ_IQ_SEL_INFO* sel_info );


/** <table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 * Load IQ binary (indivisual parameter by still mode or common parameter) from SDRAM.
 * Category parameter file include following parameter.
 * - Sensor linearity compensation table
 * - Optical black decrease
 * - Edge enhance
 * - Noise reduction
 *   etc...
 *
 * Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	type	: IQ.bin type.
 *	@param[in]	addr	: SDRAM address.
 *	@param[in]	bytes	: Valid data bytes.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Notify_Loading_Complete( E_FJ_IQ_BIN_TYPE type, BYTE* addr, ULONG bytes );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 * Set IQ Category parameter (indivisual parameter by still mode) file path.
 * Category parameter file include following parameter.
 * - Sensor linearity compensation table
 * - Optical black decrease
 * - Edge enhance
 * - Noise reduction
 *   etc...
 *
 * Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	mode            : IQ Still mode
 *	@param[in]	param_file_path : parameter file path.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *	@code
#include "fj_iq.h"

VOID setup_iq( VOID )
{
	FJ_ERR_CODE fj_ercd;
	CHAR* file_path[E_IQ_STILL_MODE_MAX] = {
		(CHAR*)"A:\\IQ_CAP.BIN",	// E_IQ_STILL_MODE_CAPTURE
		(CHAR*)"A:\\IQ_V_SD.BIN",	// E_IQ_STILL_MODE_VIEW_SD
		(CHAR*)"A:\\IQ_V_HD.BIN",	// E_IQ_STILL_MODE_VIEW_HD720P
		(CHAR*)"A:\\IQ_M_SD.BIN",	// E_IQ_STILL_MODE_MOVIE_SD
		(CHAR*)"A:\\IQ_M_HD.BIN"	// E_IQ_STILL_MODE_MOVIE_HD720P
	};
	E_IQ_STILL_MODE still_mode;

	for( still_mode = E_IQ_STILL_MODE_TOP; still_mode < E_IQ_STILL_MODE_MAX; still_mode++ ) {
		fj_ercd = FJ_IQ_Set_IQ_FilePath( still_mode, file_path[still_mode] );
		if( fj_ercd != FJ_ERR_OK ) {
			// error correction code here.
		}
	}

	fj_ercd = FJ_IQ_Set_IQ_FilePath_common( (CHAR*)"A:\\IQ_COMM.BIN" );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
	}
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Set_IQ_FilePath( E_IQ_STILL_MODE mode, CHAR* param_file_path );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Set IQ Category parameter (common parameter for all still mode) file path.
 * Category parameter file include following parameter.
 * - Optical black decrease
 * - White Balance clip level
 *   etc...
 *
 * Refer "IQ binary memory specification" for detail.
 *
 *	@param[in]	param_file_path : parameter file path.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 *	@par Sample code:
 *				Please refer the sample code of FJ_IQ_Set_IQ_FilePath().
 */
extern FJ_ERR_CODE FJ_IQ_Set_IQ_FilePath_common( CHAR* param_file_path );


/*==============================================================================*/
/* Function declaration(CAG-FFC Compensation)									*/
/*==============================================================================*/

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This function convert CAG-FFC table from "src" to "dst".
 *
 *	@param[in]	dst_mode		: Destination still_mode.
 *	@param[in]	src_mode		: Source still_mode.
 *	@param[in]	conv_info		: Conversion information parameter from source mode to destination mode.
 *	@retval		FJ_ERR_OK : successful.
 *	@par Sample code:
 *	@code
#include "fj_iq.h"

VOID sample_cag_load_and_convert( BYTE* addr, ULONG bytes )
{
	FJ_ERR_CODE fj_ercd;
	static BOOL iqbin_is_loaded = FALSE;
	static T_FJ_IQ_CAG_CONVINFO cag_conv_info = {
		0,			// src_tbl_sta_x
		376,		// src_tbl_sta_y
		4640,		// src_tbl_width
		3104 -376,	// src_tbl_lines
		1152,		// dst_tbl_width
		666			// dst_tbl_lines
	};

	// Load full resolution cag table (in IQ.bin) to "E_IQ_STILL_MODE_0(E_FJ_IQ_BIN_TYPE__0)".
	if( iqbin_is_loaded == FALSE ) {
		fj_ercd = FJ_IQ_Notify_Loading_Complete( E_FJ_IQ_BIN_TYPE__0, iqbin_addr, iqbin_bytes );
		if( fj_ercd != FJ_ERR_OK ) {
			// error correction code here.
			return;
		}
		iqbin_is_loaded = TRUE;
	}

	// Set Vertical flip information.
	fj_ercd = FJ_IQ_Flip_CagTable( TRUE );	// Enable flipped.
//	fj_ercd = FJ_IQ_Flip_CagTable( FALSE );	// Disable flipped.
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
		return;
	}

	// Convert table from E_IQ_STILL_MODE_0(Full resolution) to E_IQ_STILL_MODE_1(Low resolution).
	fj_ercd = FJ_IQ_Conv_CagTable( E_IQ_STILL_MODE_0, E_IQ_STILL_MODE_1, &cag_conv_info );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
		return;
	}
}
 *	@endcode
 */
extern FJ_ERR_CODE FJ_IQ_Conv_CagTable( E_IQ_STILL_MODE dst_mode, E_IQ_STILL_MODE src_mode, const T_FJ_IQ_CAG_CONVINFO* conv_info );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				All CAG-FFC table flip vertical.<br>
 *				This API call in Stop the processing mode (FJ_StopMode()) or FJ_MODE_IDLE.<br>
 *				After this API called, need re-convert(crop/resize) using API FJ_IQ_Conv_CAG_Table().<br>
 *
 *	@param[in]	vflip_on_off    : TRUE: Flip on. FALSE Flip off.
 *	@retval		FJ_ERR_OK		OK (only return OK.)
 *	@par Sample code:
 *	@code
#include <fj_iq.h>
#include <fj_mode.h>

VOID sample_cag_flip( VOID )
{
	FJ_ERR_CODE fj_ercd;
	static T_FJ_IQ_CAG_CONVINFO cag_conv_info = {
		0,			// src_tbl_sta_x
		376,		// src_tbl_sta_y
		4640,		// src_tbl_width
		3104 -376,	// src_tbl_lines
		1152,		// dst_tbl_width
		666			// dst_tbl_lines
	};

	// need to load resolution cag table (in IQ.bin) to "E_IQ_STILL_MODE_0(E_FJ_IQ_BIN_TYPE__0)".


	// Stop current prossessing mode. (e.g. FJ_MODE_VIDEO)
	FJ_StopMode();
	FJ_WaitModeComp( FJ_MODE_VIDEO, 10, 100 );


	// Set Vertical flip information.
	fj_ercd = FJ_IQ_Flip_CagTable( TRUE );	// Enable flipped.
//	fj_ercd = FJ_IQ_Flip_CagTable( FALSE );	// Disable flipped.
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
		return;
	}

	// Convert table from E_IQ_STILL_MODE_0(Full resolution) to E_IQ_STILL_MODE_1(Low resolution).
	fj_ercd = FJ_IQ_Conv_CagTable( E_IQ_STILL_MODE_0, E_IQ_STILL_MODE_1, &cag_conv_info );
	if( fj_ercd != FJ_ERR_OK ) {
		// error correction code here.
		return;
	}
}
 *	@endcode
 */
extern FJ_ERR_CODE FJ_IQ_Flip_CagTable( BOOL vflip_on_off );


/* @} */


/** @weakgroup fj_iq_api_badpix
@{
*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function executes the bad pixel detection.<br>
Detects Dead Pixcel(DP) from bayer data  and create Dead Pixel Compensation(DPC) table.<br>
If DPC table creation is successful, DPC table set to enabled.<br>
If the number of bad pixels are more than ui_max_badpix_thresh, @ref FJ_ERR_IQ_BP_TOO_MUCH is returned.<br>
If bad pixel correction is impossible (over 4 bad pixel continuously is detected),<br>
The number of pixel impossible to correct is set to pui_over4succession_count and @ref FJ_ERR_OK<br>
is returned. This information is couned up when more than 4 continuous bad pixel is found.<br>
<br>
<hr>
<b>Data format of badpixel information</b><br>
<br>
@image html fj_iq_iq_api_badpix_information_data_format.png	Fig. Bad pixel information format<br>
<br>

<hr>
<b>Applicable threshold value and Range of corner</b><br>
<br>
@image html fj_iq_iq_api_badpix_applicable_threthold.png	Fig. Applicable threshold area on Full sensor window
<br>
<b>[Applicable threshold value]</b><br>
<TABLE>
<TR>
<TH>Area name</TH>
<TH>Applied argument of threshold</TH>
<TH>API parameter position</TH>
<TH>Range</TH>
</TR>
<TR>
<TD rowspan="2" align="center">TH</TD>
<TD>ui_high_thresh</TD>
<TD>The third argument of API</TD>
<TD rowspan="10">0 - 4096</TD>
</TR>
<TR>
<TD>us_low_thresh</TD>
<TD>The second argument of API</TD>
</TR>
<TR>
<TD rowspan="2" align="center">TH1</TD>
<TD>pt_corner_thresh->t_left_top.us_threshold_high</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD>pt_corner_thresh->t_left_top.us_threshold_low</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD rowspan="2" align="center">TH2</TD>
<TD>pt_corner_thresh->t_right_top.us_threshold_high</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD>pt_corner_thresh->t_right_top.us_threshold_low</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD rowspan="2" align="center">TH3</TD>
<TD>pt_corner_thresh->t_left_bottom.us_threshold_high</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD>pt_corner_thresh->t_left_bottom.us_threshold_low</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD rowspan="2" align="center">TH4</TD>
<TD>pt_corner_thresh->t_right_bottom.us_threshold_high</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD>pt_corner_thresh->t_right_bottom.us_threshold_low</TD>
<TD>The fifth argument of API</TD>
</TR>
</TABLE>

<b>[Range of corner]</b>
<TABLE>
<TR>
<TH>Area name</TH>
<TH>Applied argument of threshold</TH>
<TH>API parameter position</TH>
</TR>
<TR>
<TD align="center">x1</TD>
<TD>pt_corner_thresh->t_left_top.us_h</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD align="center">y1</TD>
<TD>pt_corner_thresh->t_left_top.us_v</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD align="center">x2</TD>
<TD>pt_corner_thresh->t_right_top.us_h</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD align="center">y2</TD>
<TD>pt_corner_thresh->t_right_top.us_v</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD align="center">x3</TD>
<TD>pt_corner_thresh->t_left_bottom.us_h</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD align="center">y3</TD>
<TD>pt_corner_thresh->t_left_bottom.us_v</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD align="center">x4</TD>
<TD>pt_corner_thresh->t_right_bottom.us_h</TD>
<TD>The fifth argument of API</TD>
</TR>
<TR>
<TD align="center">y4</TD>
<TD>pt_corner_thresh->t_right_bottom.us_v</TD>
<TD>The fifth argument of API</TD>
</TR>
</TABLE>
<br>

<hr>
<b>Bad pixel detection algorithm</b><br>
The detection algorithm is decided to the defect pixel by the first argument(e_detect_mode) of API<br>
<br>
@image html fj_iq_iq_api_badpix_bayerdata.png [BayerData]

 - e_detect_mode : @ref E_IQ_DETECT_ALGO_0<br>
The defect pixel is detected by the following patterns for algorithm 0.
@code
if( (LowThreshold > P) || (P > HighThreshold) ){
    P is Bad pixel;
}
@endcode
				
 - e_detect_mode : @ref E_IQ_DETECT_ALGO_1<br>
The defect pixel is detected by the following patterns for algorithm 1.
@code
if ((|X - P|) > HighThreshold) {
    P is Bad pixel;
}
X is last OK pixel.

The following values don't care for algorithm 1.
      us_low_thresh                                       The second argument of API
      pt_corner_thresh->t_left_top.us_threshold_low       The fifth argument of API
      pt_corner_thresh->t_right_top.us_threshold_low      The fifth argument of API
      pt_corner_thresh->t_left_bottom.us_threshold_low    The fifth argument of API
      pt_corner_thresh->t_right_bottom.us_threshold_low   The fifth argument of API
@endcode

 - e_detect_mode : @ref E_IQ_DETECT_ALGO_2<br>
The defect pixel is detected by the following patterns for algorithm 2.
@code
if ( (|X - P|) > LowThreshold) { 
    if ((|Average(A,B,C,D) - P|) > HighThreshold) {
        P is Bad pixel;
    }
}
X is last OK pixel.
@endcode

 - e_detect_mode : @ref E_IQ_DETECT_ALGO_3<br>
The defect pixel is detected by the following patterns for algorithm 3.
@code
if ( (|X - P|) > LowThreshold) { 
    if ((|(P * 100 / Average(A,B,C,D)) -100|) > HighThreshold) {
        P is Bad pixel;
    }
}
X is last OK pixel.
@endcode
<br>
<br>
Bad pixel table is maintained, and data is united at bad pixel detection.<br>
Bad pixel table is as follows.<br>
<br>

<hr>
<b>Bad pixel table</b><br>
<br>
There are table for each bad pixel collection of View, Moviw, Capture.<br>
The maximum of each table can accumulate 50,000 the defect pixels.<br>
The detection bad pixels are merged with the table in each row and column.<br>
<br>
<br>
Detection data merging image<br>
@image html fj_iq_iq_api_badpix_table_info.png	Table information can be output with FJ_IQ_CalibrationSaveBadpixTable() API

<hr>

@param[in]	sen_id	: SEN Instance ID.<br>
@param[in]	target	: calibration target.
@param[in]	tbl_idx	: Table index number. (0~4)
@param[in]	e_detect_mode				: defect pixel detection algorithm
@param[in]	us_low_thresh				: bad pixel low threshold value
@param[in]	us_high_thresh				: bad pixel high threshold value
@param[in]	ui_max_badpix_thresh		: Maximum number of detected bad pixel
@param[in]	pt_corner_thresh			: threshold information on corner
@param[out]	ui_over4succession_count	: The number of correction impossible pixels
@retval		FJ_ERR_OK						: Detection success. and create bad pixels table on memory.
@retval		FJ_ERR_NG_INPUT_PARAM			: Parameter error.
@retval		FJ_ERR_IQ_BP_DETECT_INTERNAL1	: BaseFW internal error.
@retval		FJ_ERR_IQ_BP_DETECT_INTERNAL2	: BaseFW internal error.
@retval		FJ_ERR_IQ_BP_MERGE_INTERNAL1	: BaseFW internal error.
@retval		FJ_ERR_IQ_BP_MERGE_INTERNAL2	: BaseFW internal error.
@retval		FJ_ERR_IQ_BP_CALIB_TGT			: The calibration target don't specified.  (please call FJ_IQ_CalibrationSetTarget() and retry.)
@retval		FJ_ERR_IQ_BP_THRESH				: Parameter error in pt_corner_thresh.
@retval		FJ_ERR_IQ_BP_FMT_PIX			: The format specified as E_IQ_CALIB_FMT_PIX_12PACK
@retval		FJ_ERR_IQ_BP_TOO_MUCH			: The number of bad pixels are more than max_badpix_thresh
@retval		FJ_ERR_IQ_BP_TOO_OVERFLOW		: The bad pixel table memory is overflow.
@par Sample code:
@code
#include "fj_iq.h"

VOID FJ_HostStillRawManipulation_sample( UCHAR inst_id, FJ_RAW_MANI_STILL mani_no, PSRAWINFO psInfo, USHORT* puwScalers )
{
	E_IQ_DETECT_ALGORITHM e_detect_mode = E_IQ_DETECT_ALGO_0;
	USHORT calib_lo_thresh = 0;
	USHORT calib_hi_thresh = 140;
	UINT32 calib_pixnum_thresh = 50000;
	FJ_IQ_CORNER_THRESHOLD corner_threth = {
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	};
	UINT32 calib_impossible_pixs = 0;
	UINT32 calib_num_of_badpix = 0xffffffff;
	FJ_ERR_CODE iq_ercd;
	E_IQ_CALIB_TGT photo_tgt = E_IQ_CALIB_TGT_0;
	BYTE* addr;
	UINT32 bytes;

	if( FJ_Get_Act_Sysmode() == FJ_MODE_CALIBRATION ) {
		if( mani_no == FJ_RAW_MANI_STILL_POST_SEN ) {
			FJ_IQ_CalibrationClearBadpixTable( photo_tgt );	// Clear exist badpix table for create new table by FJ_IQ_CalibrationCreateBadpixTable().

			iq_ercd = FJ_IQ_CalibrationCreateBadpixTable( inst_id,
														  photo_tgt,
														  e_detect_mode,
														  calib_lo_thresh,
														  calib_hi_thresh,
														  calib_pixnum_thresh,
														  &corner_threth,
														  &calib_impossible_pixs );
			if( iq_ercd != FJ_ERR_OK ) {
				// Error correction code here.
				return;
			}

			calib_num_of_badpix = FJ_IQ_CalibrationGetNumOfBadpix( photo_tgt );
			printf( "Number of bad pixel = %u\n", calib_num_of_badpix );

			iq_ercd = FJ_IQ_Get_BadpixTableAddr( photo_tgt, &addr, &bytes );
			if( iq_ercd != FJ_ERR_OK ) {
				// Error correction code here.
				return;
			}

			save_to_media( addr, bytes );	// User implementation function.
		}
	}
}
@endcode

*/
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_CalibrationCreateBadpixTable( UCHAR sen_id,
//											E_IQ_CALIB_TGT target,
//											UINT32 tbl_idx,
//											E_IQ_DETECT_ALGORITHM e_detect_mode,
//											USHORT us_low_thresh,
//											USHORT us_high_thresh,
//											UINT32 ui_max_badpix_thresh,
//											FJ_IQ_CORNER_THRESHOLD* pt_corner_thresh,
//											UINT32* ui_over4succession_count );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function returns the number of badpixels.
 *
 *	@param[in]	target	: calibration target.
 *	@param[in]	tbl_idx	: Table index number. (0~4)
 *	@return		Number of bad pixel for specified target
 *	@par Sample code:<br>
 *				Please refer the sample code of FJ_IQ_CalibrationCreateBadpixTable().
 */
// delete_basefw\fj\iq
//extern UINT32 FJ_IQ_CalibrationGetNumOfBadpix( E_IQ_CALIB_TGT target, UINT32 tbl_idx );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function clears the bad pixel table from Memory(SDRAM) for specified target.
 *
 *	@param[in]	target	: calibration target.
 *	@param[in]	tbl_idx	: Table index number. (0~4)
 *	@retval		TRUE : clear successful.
 *	@retval		FALSE : clear failer.
 *	@par Sample code:<br>
 *				Please refer the sample code of FJ_IQ_CalibrationCreateBadpixTable().
 */
// delete_basefw\fj\iq
//extern BOOL FJ_IQ_CalibrationClearBadpixTable( E_IQ_CALIB_TGT target, UINT32 tbl_idx );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function changes the validity of bad pixel correction processing.<br>
 *				The set information is changed when mode change.
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID.<br>
 *	@param[in]	b_onoff	: Dead pixel Compensation enable/disable
 *	@remarks		The bad pixel information which is not able to correct (4 continuous bad pixel) is copied to the
 *				calibration table.
 *	@par Sample code:<br>
 *				Please refer the sample code of FJ_IQ_Notify_Loading_BadpixTable_Complete().
 */
extern VOID FJ_IQ_CalibrationOnOffBadpixCompensation( UCHAR demosaic_id, BOOL b_onoff );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function saves the bad pixel table convert from Photo mode to Video mode with a image sensor pixel additional pattern.
 *
 *				<b>Outline of Convert Bad pixel table</b><br>
 *				<br>
 *				@image html fj_iq_iq_api_badpix_convert.png	Fig. Outline of Convert Bad pixel table.<br>
 *
 *	@param[in]	dst_target		: Calibration target for Video mode (destination bad pixel table).
 *	@param[in]	src_target		: Calibration target for Photo mode (source bad pix table).
 *	@param[in]	conv_info		: Image sensor pixel additional pattern.
 *	@retval		FJ_ERR_OK		OK
 *	@retval		FJ_IQ_TBL_FULL	Table memory is full.
 *	@retval		FJ_ERR_IQ_TBL_INVALID	Table is invalid(not ready).
 *	@par Sample code:<br>
 *				Please refer the sample code of FJ_IQ_Notify_Loading_BadpixTable_Complete().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_CalibrationConvertBadpixTable( E_IQ_CALIB_TGT dst_target, E_IQ_CALIB_TGT src_target, FJ_IQ_BADPIX_CONV_INFO* conv_info );

//@cond
/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function saves the bad pixel table to NF partition for specified target.
 *
 *	@param[in]	target			: calibration target.
 *	@param[in]	tbl_idx			: Table index number. (0~4)
 *	@param[in]	to_file_path	: save file absolute path.
 *	@retval		TRUE : save successful.
 *	@retval		FALSE : save failer.
 *	@par Sample code:<br>
 *				Please refer the sample code of FJ_IQ_CalibrationCreateBadpixTable().
 */
// delete_basefw\fj\iq
//extern BOOL FJ_IQ_CalibrationSaveBadpixTable( E_IQ_CALIB_TGT target, UINT32 tbl_idx, CHAR* to_file_path );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function loads the bad pixel table from NF partition for specified target.
 *
 *	@param[in]	target			: calibration target.
 *	@param[in]	tbl_idx			: Table index number. (0~4)
 *	@param[in]	from_file_path	: load file absolute path.
 *	@retval		TRUE : load successful.
 *	@retval		FALSE : load failer.
 *	@par Sample code:
 *	@code
In initialize function when bootup time:
#include <fj_iq.h>

FJ_ERR_CODE iq_ercd;

iq_ercd = FJ_IQ_CalibrationLoadBadpixTable( E_IQ_CALIB_TGT_0, 0, "A:\\BPC_CAP.BIN" );
if( iq_ercd != TRUE ) {
	// error correction code here.
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern BOOL FJ_IQ_CalibrationLoadBadpixTable( E_IQ_CALIB_TGT target, UINT32 tbl_idx, CHAR* from_file_path );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function deletes the bad pixel table from NF partition for specified target.
 *
 *	@param[in]	target			: calibration target.
 *	@param[in]	tbl_idx			: Table index number. (0~4)
 *	@param[in]	from_file_path	: Delete file absolute path.
 *	@retval		TRUE : delete successful.
 *	@retval		FALSE : delete failer.
 *	@par Sample code:
 *	@code
#include <fj_iq.h>

FJ_ERR_CODE iq_ercd;
UCHAR sen_id = 0;

iq_ercd = FJ_IQ_CalibrationDeleteBadpixTable( sen_id, E_IQ_CALIB_TGT_0, "A:\\BPC_CAP.BIN" );
if( iq_ercd != TRUE ) {
	// error correction code here.
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern BOOL FJ_IQ_CalibrationDeleteBadpixTable( E_IQ_CALIB_TGT target, UINT32 tbl_idx, CHAR* from_file_path );
// @endcond

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function loads the bad pixel table for specified target to BaseFW internal table memory area.
 *
 *	@param[in]	target	: calibration target.
 *	@param[in]	tbl_idx	: Table index number. (0~4)
 *	@param[in]	addr	: address of BadpixTable.
 *	@param[in]	bytes	: size of BadpixTable.
 *	@retval		TRUE : successful.
 *	@retval		FALSE : failer.
 *	@par Sample code:
 *	@code
In initialize function:
#include <fj_iq.h>

#define BADPIX_FILE_MAX_BYTES	(5000)
static BYTE		badpix_tbl_buf[BADPIX_FILE_MAX_BYTES];

VOID init_badpix( UCHAR demosaic_id )
{
	FJ_ERR_CODE		iq_ercd;
	ULONG			bytes;
	E_IQ_CALIB_TGT	photo_tgt			= E_IQ_CALIB_TGT_0;
	UINT32			photo_tbl_idx		= 0;
	E_IQ_CALIB_TGT	video_mode_n_tgt	= E_IQ_CALIB_TGT_1;
	FJ_IQ_BADPIX_CONV_INFO	conv_info_video_mode_n = {
		.add_pat[FJ_IQ_BADPIX_LINE_EVEN][FJ_IQ_BADPIX_COLUM_EVEN] = {
			.add_pos[0].y = 0,
			.add_pos[0].x = { 0, 2, 4 },
			.add_pos[0].x_num = 3,
			.add_pos[1].y = 2,
			.add_pos[1].x = { 0, 2, 4 },
			.add_pos[1].x_num = 3,
			.add_pos[2].y = 4,
			.add_pos[2].x = { 0, 2, 4 },
			.add_pos[2].x_num = 3,
			.add_pos_num = 3,	// Use add_pos[0] ~ [2]
			.cycle_start_pos = { .x_pos = 0, .y_pos = 0 },
			.cycle_rect      = { .width = 8, .lines = 8 },
		},
		.add_pat[FJ_IQ_BADPIX_LINE_EVEN][FJ_IQ_BADPIX_COLUM_ODD] = {
			.add_pos[0].y = 0,
			.add_pos[0].x = { 3, 5, 7 },
			.add_pos[0].x_num = 3,
			.add_pos[1].y = 2,
			.add_pos[1].x = { 3, 5, 7 },
			.add_pos[1].x_num = 3,
			.add_pos[2].y = 4,
			.add_pos[2].x = { 3, 5, 7 },
			.add_pos[2].x_num = 3,
			.add_pos_num = 3,	// Use add_pos[0] ~ [2]
			.cycle_start_pos = { .x_pos = 0, .y_pos = 0 },
			.cycle_rect      = { .width = 8, .lines = 8 },
		},
		.add_pat[FJ_IQ_BADPIX_LINE_ODD][FJ_IQ_BADPIX_COLUM_EVEN] = {
			.add_pos[0].y = 3,
			.add_pos[0].x = { 0, 2, 4 },
			.add_pos[0].x_num = 3,
			.add_pos[1].y = 5,
			.add_pos[1].x = { 0, 2, 4 },
			.add_pos[1].x_num = 3,
			.add_pos[2].y = 7,
			.add_pos[2].x = { 0, 2, 4 },
			.add_pos[2].x_num = 3,
			.add_pos_num = 3,	// Use add_pos[0] ~ [2]
			.cycle_start_pos = { .x_pos = 0, .y_pos = 0 },
			.cycle_rect      = { .width = 8, .lines = 8 },
		},
		.add_pat[FJ_IQ_BADPIX_LINE_ODD][FJ_IQ_BADPIX_COLUM_ODD] = {
			.add_pos[0].y = 3,
			.add_pos[0].x = { 3, 5, 7 },
			.add_pos[0].x_num = 3,
			.add_pos[1].y = 5,
			.add_pos[1].x = { 3, 5, 7 },
			.add_pos[1].x_num = 3,
			.add_pos[2].y = 7,
			.add_pos[2].x = { 3, 5, 7 },
			.add_pos[2].x_num = 3,
			.add_pos_num = 3,	// Use add_pos[0] ~ [2]
			.cycle_start_pos = { .x_pos = 0, .y_pos = 0 },
			.cycle_rect      = { .width = 8, .lines = 8 },
		},
		.crop_pos_src  = { .x_pos =    0, .y_pos =  376 },		// 16:9 Crop window with Photo. (Must adjust image sensor specification.)
		.crop_rect_src = { .width = 4072, .lines = 2290 },		// 16:9 Crop window with Photo. (Must adjust image sensor specification.)
		.sen_window_pos_src = { .x_pos = 36, .y_pos = 24 },		// SEN window with Photo.
		.sen_window_pos_dst = { .x_pos = 48, .y_pos =  4 },		// SEN window with Video.
	};


	bytes = load_bad_pixel_table_to_sdram( badpix_tbl_buf, BADPIX_FILE_MAX_BYTES );	// User implementation function.

	iq_ercd = FJ_IQ_Notify_Loading_BadpixTable_Complete( photo_tgt, photo_tbl_idx, badpix_tbl_buf, bytes );
	if( iq_ercd != TRUE ) {
		// error correction code here.
		return;
	}

	// Flip vertical if needed.
	FJ_IQ_Flip_BadpixTable( photo_tgt, &flip_info );


	iq_ercd = FJ_IQ_CalibrationConvertBadpixTable( video_mode_n_tgt, photo_tgt, &conv_info_video_mode_n );
	if( iq_ercd != TRUE ) {
		// error correction code here.
		return;
	}

	iq_ercd = FJ_IQ_Select_BadpixTable( demosaic_id, video_mode_n_tgt, photo_tbl_idx );
	if( iq_ercd != TRUE ) {
		// error correction code here.
		return;
	}

	FJ_IQ_CalibrationOnOffBadpixCompensation( demosaic_id, TRUE );
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern BOOL FJ_IQ_Notify_Loading_BadpixTable_Complete( E_IQ_CALIB_TGT target, UINT32 tbl_idx, BYTE* addr, ULONG bytes );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function get the bad pixel table for file write.
 *
 *	@param[in]	target	: calibration target.
 *	@param[in]	tbl_idx	: Table index number. (0~4)
 *	@param[in]	addr        : Pointer of Bad pixel table.
 *	@param[in]	bytes       : Bad pixel table size.
 *	@retval		FJ_ERR_OK					OK.
 *	@retval		FJ_ERR_IQ_TBL_INVALID		Bad pixel table not found specified "@ref target".
 *	@retval		FJ_ERR_NG					Other Error.
 *	@par Sample code:<br>
 *				Please refer the sample code of FJ_IQ_CalibrationCreateBadpixTable().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Get_BadpixTableAddr( E_IQ_CALIB_TGT target, UINT32 tbl_idx, BYTE** addr, UINT32* bytes );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function select the bad pixel table for compensation.
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID.<br>
 *	@param[in]	target	: calibration target select (Bad pixel table select).
 *	@param[in]	tbl_idx	: Table index number. (0~4)
 *	@retval		FJ_ERR_OK					OK.
 *	@retval		FJ_ERR_IQ_TBL_INVALID		Bad pixel table not found specified "@ref target".
 *	@retval		FJ_ERR_NG					Other Error.
 *	@par Sample code:<br>
 *				Please refer the sample code of FJ_IQ_Notify_Loading_BadpixTable_Complete().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Select_BadpixTable( UCHAR demosaic_id, E_IQ_CALIB_TGT target, UINT32 tbl_idx );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				The bad pixel tables specified argument \"target\" flip vertical.<br>
 *				This API call in Stop the processing mode (FJ_StopMode()) or FJ_MODE_IDLE.<br>
 *				After this API called, need re-convert(crop/resize) using API FJ_IQ_CalibrationConvertBadpixTable().<br>
 *
 *	@param[in]	target		: The calibration target select (Bad pixel table select).
 *	@param[in]	flip_info	: The flip position parameter.
 *	@retval		FJ_ERR_OK					Success.
 *	@retval		FJ_ERR_IQ_TBL_INVALID		Bad pixel table not found specified argument \"target\".
 *	@retval		FJ_ERR_IQ_FILE_TOO_SHORT	Source Bad pixel table is broken.
 *	@retval		FJ_ERR_IQ_TBL_FULL			Bad pixel table memory is full.
 *	@retval		FJ_ERR_NG					Internal error.
 *	@par Sample code:
 *	@code
#include <fj_iq.h>
#include <fj_mode.h>

VOID sample_badpix_flip( VOID )
{
	FJ_ERR_CODE fj_ercd;
	E_IQ_CALIB_TGT	photo_tgt			= E_IQ_CALIB_TGT_0;
	E_IQ_CALIB_TGT	video_mode_n_tgt	= E_IQ_CALIB_TGT_1;
	static FJ_IQ_BADPIX_FLIP_INFO flip_info = {
		{	96,		16 },	// flip_pos_dst
	};
	FJ_IQ_BADPIX_CONV_INFO	conv_info_video_mode_n = {
		.add_pat[FJ_IQ_BADPIX_LINE_EVEN][FJ_IQ_BADPIX_COLUM_EVEN] = {
			.add_pos[0].y = 0,
			.add_pos[0].x = { 0, 2, 4 },
			.add_pos[0].x_num = 3,
			.add_pos[1].y = 2,
			.add_pos[1].x = { 0, 2, 4 },
			.add_pos[1].x_num = 3,
			.add_pos[2].y = 4,
			.add_pos[2].x = { 0, 2, 4 },
			.add_pos[2].x_num = 3,
			.add_pos_num = 3,	// Use add_pos[0] ~ [2]
			.cycle_start_pos = { .x_pos = 0, .y_pos = 0 },
			.cycle_rect      = { .width = 8, .lines = 8 },
		},
		.add_pat[FJ_IQ_BADPIX_LINE_EVEN][FJ_IQ_BADPIX_COLUM_ODD] = {
			.add_pos[0].y = 0,
			.add_pos[0].x = { 3, 5, 7 },
			.add_pos[0].x_num = 3,
			.add_pos[1].y = 2,
			.add_pos[1].x = { 3, 5, 7 },
			.add_pos[1].x_num = 3,
			.add_pos[2].y = 4,
			.add_pos[2].x = { 3, 5, 7 },
			.add_pos[2].x_num = 3,
			.add_pos_num = 3,	// Use add_pos[0] ~ [2]
			.cycle_start_pos = { .x_pos = 0, .y_pos = 0 },
			.cycle_rect      = { .width = 8, .lines = 8 },
		},
		.add_pat[FJ_IQ_BADPIX_LINE_ODD][FJ_IQ_BADPIX_COLUM_EVEN] = {
			.add_pos[0].y = 3,
			.add_pos[0].x = { 0, 2, 4 },
			.add_pos[0].x_num = 3,
			.add_pos[1].y = 5,
			.add_pos[1].x = { 0, 2, 4 },
			.add_pos[1].x_num = 3,
			.add_pos[2].y = 7,
			.add_pos[2].x = { 0, 2, 4 },
			.add_pos[2].x_num = 3,
			.add_pos_num = 3,	// Use add_pos[0] ~ [2]
			.cycle_start_pos = { .x_pos = 0, .y_pos = 0 },
			.cycle_rect      = { .width = 8, .lines = 8 },
		},
		.add_pat[FJ_IQ_BADPIX_LINE_ODD][FJ_IQ_BADPIX_COLUM_ODD] = {
			.add_pos[0].y = 3,
			.add_pos[0].x = { 3, 5, 7 },
			.add_pos[0].x_num = 3,
			.add_pos[1].y = 5,
			.add_pos[1].x = { 3, 5, 7 },
			.add_pos[1].x_num = 3,
			.add_pos[2].y = 7,
			.add_pos[2].x = { 3, 5, 7 },
			.add_pos[2].x_num = 3,
			.add_pos_num = 3,	// Use add_pos[0] ~ [2]
			.cycle_start_pos = { .x_pos = 0, .y_pos = 0 },
			.cycle_rect      = { .width = 8, .lines = 8 },
		},
		.crop_pos_src  = { .x_pos =    0, .y_pos =  376 },		// 16:9 Crop window with Photo. (Must adjust image sensor specification.)
		.crop_rect_src = { .width = 4072, .lines = 2290 },		// 16:9 Crop window with Photo. (Must adjust image sensor specification.)
		.sen_window_pos_src = { .x_pos = 36, .y_pos = 24 },		// SEN window with Photo.
		.sen_window_pos_dst = { .x_pos = 48, .y_pos =  4 },		// SEN window with Video.
	};

	// Stop current prossessing mode.
	FJ_StopMode();
	FJ_WaitModeComp( FJ_MODE_xxx, 10, 100 );


	// Flip vertical.
	FJ_IQ_Flip_BadpixTable( photo_tgt, &flip_info );


	// Convert 
	iq_ercd = FJ_IQ_CalibrationConvertBadpixTable( video_mode_n_tgt, photo_tgt, &conv_info_video_mode_n );
	if( iq_ercd != TRUE ) {
		// error correction code here.
		return;
	}
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Flip_BadpixTable( E_IQ_CALIB_TGT target, FJ_IQ_BADPIX_FLIP_INFO* flip_info );

/* @} */


/** @weakgroup fj_iq_api_shading
@{
*/

/*==============================================================================*/
/* Function declaration(Frame-Shading Compensation)								*/
/*==============================================================================*/


/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This API changes the validity of Frame-shading correction.<br>
 *				Paramter value is reflected when system mode change.
 *
 *	@param[in]	demosaic_id	: Demosaic instance id.<br>
 *	@param[in]	onoff	: set Frame-Shading compensation enable/disable
 *	@retval		FJ_ERR_OK	OK
 *	@retval		FJ_ERR_NG	Error
 */
extern FJ_ERR_CODE FJ_IQ_OnOff_FrameShading( UCHAR demosaic_id, BOOL onoff );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API changes the Frame-shading correction table for Video and Still image pipeline.<br>
 *	Table is reflected immediately when image pipeline is active.<br>
 *	If table was not found indicate ozoom_idx and f_number, Calculate BI-Linear interpolation by near 4 tables.<br>
 *
 *	@param[in]	demosaic_id		: Demosaic instance id.<br>
 *	@param[in]	tbl_sel_info	: Frame-shading table selection information for compensation.
 *	@retval		FJ_ERR_OK	OK
 *	@retval		FJ_ERR_NG	Error
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Select_FrameShadingTable( UCHAR demosaic_id, T_FJ_IQ_FSHD_SEL_INFO* tbl_sel_info );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function create Frame-shading correction table from bayer readouted on @ref FJ_MODE_CALIBRATION.\n
 *				Please make calibration subject a 18 percent gray chart.\n
 *				This function can call in FJ_HostStillRawManipulation() on @ref FJ_MODE_CALIBRATION.\n
 *
 *				<b>Table calculate sequence:</b>
 *				- The brightness of more points by calibration picture is calculated.
 *				- Search maxium brightness.
 *				- The gains of each points calculate.\n
 *				  The value "corner_ratio" is ratio "brightnes of corner after compensation" / "maxium brightness".
 *				- The gain value is clipping to x3.999755859375. This is hardware specification.
 *
 *	@param[in]	sen_id          : SEN instance ID.
 *	@param[in]	tbl_sel_info	: Frame-shading table selection information for compensation.
 *	@param[in]	corner_ratio    : Ratio of corer luminence/center luminence. 1.0 means 100 percent.
 *	@retval		FJ_ERR_OK					OK
 *	@retval		FJ_ERR_IQ_TBL_INVALID		SDRAM for Shading table is full.
 *	@retval		FJ_ERR_IQ_GAIN_OVERFLOW		Calculated gain is overflow.\n
 *											The overfloted gain is clipping to hardware limit "3.999755859375" (0x3FFF / 0x1000) limit).
 *	@retval		FJ_ERR_NG					Other Error
 *	@par Sample code:
 *	@code
VOID FJ_HostStillRawManipulation( UCHAR inst_id, FJ_RAW_MANI_STILL mani_no, PSRAWINFO psInfo, USHORT* puwScalers )
{
	FJ_ERR_CODE fj_ercd;
	T_FJ_IQ_FSHD_SEL_INFO	tbl_sel_info = {
		.calib_tgt = E_IQ_CALIB_TGT_0,		// e.g. for STILL mode.
		.ozoom_idx = 0,
		.f_number = 0,
	};

	if( FJ_Get_Act_Sysmode() == FJ_MODE_CALIBRATION ) {
		if( mani_no == FJ_RAW_MANI_STILL_POST_SEN ) {
			fj_ercd = FJ_IQ_Calibration_FrameShading( inst_id, E_IQ_CALIB_TGT_0, 0.9 );
			if( iq_ercd != FJ_ERR_OK ) {
				// error correction code here.
				return;
			}
		}
	}
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Calibration_FrameShading( UCHAR sen_id, T_FJ_IQ_FSHD_SEL_INFO* tbl_sel_info, DOUBLE corner_ratio );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This API link the Frame-shading correction table.<br>
 *	@param[in]	to_tbl_sel_info		: Destination Frame-shading table selection information.
 *	@param[in]	from_tbl_sel_info	: Source Frame-shading table selection information.
 *	@retval		FJ_ERR_OK	OK
 *	@retval		FJ_ERR_NG	Error
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Link_FrameShadingTable( T_FJ_IQ_FSHD_SEL_INFO* to_tbl_sel_info, T_FJ_IQ_FSHD_SEL_INFO* from_tbl_sel_info );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This function convert All frame shading table from "calib_tgt_src" to "calib_tgt_dst".
 *
 *	@param[in]	calib_tgt_dst		: Destination Calibration target.
 *	@param[in]	calib_tgt_src		: Source Calibration target.
 *	@param[in]	conv_info			: Conversion information parameter from source mode to destination mode.
 *	@retval		FJ_ERR_OK : successful.
 *	@retval		FJ_ERR_NG_INPUT_PARAM : Input parameter error.
 *	@retval		FJ_ERR_IQ_TIMEOUT : Convert hardware is busy.
 *	@retval		FJ_ERR_IQ_DRIVER_ERR : Driver error occurred.
 *	@retval		FJ_ERR_IQ_TBL_INVALID : Source table is invalid.
 *	@par Sample code:
Ex: Convert Still (RAW 4640x3488) to Video (RAW 1152x666).<br>
@image html fj_iq_iq_api_fshd_conv.png <br>
 *	@code
#include "fj_iq.h"

VOID sample_fshd_convert( VOID )
{
	FJ_ERR_CODE fj_ercd;
	static T_FJ_IQ_FSHD_CONVINFO fshd_conv_info = {
		0,			// src_tbl_sta_x
		376,		// src_tbl_sta_y
		4640,		// src_tbl_width
		3104 -376,	// src_tbl_lines
		1152,		// dst_tbl_width
		666			// dst_tbl_lines
	};

	// need to load full resolution fshd table to "E_IQ_CALIB_TGT_0".

	// Convert table from E_IQ_CALIB_TGT_0(Full resolution) to E_IQ_CALIB_TGT_1(Low resolution).
	fj_ercd = FJ_IQ_Conv_FrameShadingTable( E_IQ_CALIB_TGT_0, E_IQ_CALIB_TGT_1, &fshd_conv_info );
	if( fj_ercd != FJ_ERR_OK ) {
		return;
	}
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Conv_FrameShadingTable( const E_IQ_CALIB_TGT calib_tgt_dst, const E_IQ_CALIB_TGT calib_tgt_src, T_FJ_IQ_FSHD_CONVINFO* conv_info );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				Load Frame-shading correction table to BaseFW interanal table memory area.
 *
 *				Please refer the sequence @ref fj_calibration_shading_compensation_sequence.
 *
 *	@param[in]	calib_tgt   : calibration target.
 *	@param[in]	addr        : Frame-Shading table address.
 *	@param[in]	bytes       : Frame-Shading table size.
 *	@retval		FJ_ERR_OK	OK
 *	@retval		FJ_ERR_IQ_FILE_FMT_NG		File format is unknown.
 *	@retval		FJ_ERR_IQ_FILE_TOO_SHORT	File size is small.
 *	@retval		FJ_ERR_IQ_FILE_TOO_LEARGE	File size is learge.
 *	@retval		FJ_ERR_IQ_TBL_INVALID		SDRAM for Shading table is full.
 *	@retval		FJ_ERR_NG					Other Error.
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Notify_Loading_FrameShadingTable_Complete( E_IQ_CALIB_TGT calib_tgt, BYTE* addr, ULONG bytes );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				Get Frame-shading correction table binary for file write.
 *
 *				Please refer the sequence @ref fj_calibration_shading_compensation_sequence.
 *
 *	@param[in]	tbl_sel_info: Frame-shading table selection information.
 *	@param[in]	addr        : Pointer of Frame-Shading table.
 *	@param[in]	bytes       : Frame-Shading table size.
 *	@retval		FJ_ERR_OK					OK.
 *	@retval		FJ_ERR_IQ_TBL_INVALID		Shading table not found specified tbl_sel_info.
 *	@retval		FJ_ERR_NG					Other Error.
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Get_FrameShadingTableAddr( T_FJ_IQ_FSHD_SEL_INFO* tbl_sel_info, BYTE** addr, UINT32* bytes );

// @cond
/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				Load Frame-shading correction table file from filesystem.
 *
 *				Please refer the sequence @ref fj_calibration_shading_compensation_sequence.
 *
 *	@param[in]	calib_tgt       : calibration target.
 *	@param[in]	filepath        : Frame-Shading table file path.
 *	@retval		FJ_ERR_OK	OK
 *	@retval		FJ_ERR_IQ_FILE_OPEN_NG		File open error.
 *	@retval		FJ_ERR_IQ_FILE_FMT_NG		File format is unknown.
 *	@retval		FJ_ERR_IQ_FILE_TOO_SHORT	File size is small.
 *	@retval		FJ_ERR_IQ_FILE_TOO_LEARGE	File size is learge.
 *	@retval		FJ_ERR_IQ_TBL_INVALID		SDRAM for Shading table is full.
 *	@retval		FJ_ERR_NG					Other Error.
 *	@par Sample code:
 *	@code
#include <fj_iq.h>
 
FJ_ERR_CODE fj_ercd;
BOOL iq_ercd;
UCHAR demosaic_id = 0;
 
// load Frame-shading tbl file
UCHAR *puc_frame_shading_tbl_filepath = "A:\\shd_tbl.bin";	// Table spec is Zoom0 F3.5
fj_ercd = FJ_IQ_LoadFile_FrameShadingTable( E_IQ_CALIB_TGT_0, puc_shading_tbl_filepath );
if( fj_ercd != FJ_ERR_OK ) {
	// error correction code here.
}
// select Frame-shading table.
fj_ercd = FJ_IQ_Select_FrameShadingTable( demosaic_id, 0, (USHORT)(0x100 * 3.5) );	// Zoom0, F3.5
if( fj_ercd != FJ_ERR_OK ) {
	// error correction code here.
}
// set to enable Frame-shading compansation
FJ_IQ_OnOff_FrameShading( demosaic_id, TRUE );
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_LoadFile_FrameShadingTable( E_IQ_CALIB_TGT calib_tgt, CHAR* filepath );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				Save Frame-shading correction table file to filesystem.
 *
 *	@param[in]	tbl_sel_info    : Frame-shading table selection information.
 *	@param[in]	filepath        : Frame-Shading table file path.
 *	@retval		FJ_ERR_OK					OK
 *	@retval		FJ_ERR_IQ_TBL_INVALID		Table data on SDRAM is invalid.
 *	@retval		FJ_ERR_IQ_FILE_OPEN_NG		File open error.
 *	@retval		FJ_ERR_IQ_FILE_TOO_LEARGE	Table data on SDRAM is invalid.
 *	@retval		FJ_ERR_NG					File write Error.
 *	@par Sample code:
 *	@code
#include <fj_iq.h>
 
FJ_ERR_CODE fj_ercd;
BOOL iq_ercd;
 
// save Frame-shading tbl file
UCHAR *puc_frame_shading_tbl_filepath = "A:\\shd_tbl.bin";
fj_ercd = FJ_IQ_SaveFile_FrameShadingTable( E_IQ_CALIB_TGT_0, puc_shading_tbl_filepath );
if( fj_ercd != FJ_ERR_OK ) {
	// error correction code here.
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_SaveFile_FrameShadingTable( T_FJ_IQ_FSHD_SEL_INFO* tbl_sel_info, CHAR* filepath );
// @endcond

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				Load Frame-shading correction table file from sdram.
 *
 *				Please refer the sequence @ref fj_calibration_shading_compensation_sequence.
 *
 *	@param[in]	calib_tgt       : calibration target(view/capture).
 *	@param[in]	tbl_addr        : Frame-Shading table address.
 *	@retval		FJ_ERR_OK	OK
 *	@retval		FJ_ERR_IQ_TBL_INVALID		SDRAM for Shading table is full.
 *	@retval		FJ_ERR_NG					Other Error.
 *	@par Sample code:
 *	@code
#include <fj_iq.h>
 
FJ_ERR_CODE fj_ercd;
BOOL iq_ercd;
 
// load Frame-shading tbl file
UCHAR *puc_shading_tbl_addr;

puc_shading_tbl_addr = (UCHAR*)RS_System_Factor_Draft_Calib_Get_Data_Addr();	// Table spec is Zoom0 F3.5
fj_ercd = FJ_IQ_LoadFile_FrameShadingTable_Sdram( E_IQ_CALIB_TGT_CAPTURE, puc_shading_tbl_addr );
if( fj_ercd != FJ_ERR_OK ) {
	// error correction code here.
}
// select Frame-shading table.
fj_ercd = FJ_IQ_Select_FrameShadingTable( 0, (USHORT)(0x100 * 3.5) );	// Zoom0, F3.5
if( fj_ercd != FJ_ERR_OK ) {
	// error correction code here.
}
// set to enable Frame-shading compansation
FJ_IQ_OnOff_FrameShading( TRUE );
 *	@endcode
 *	@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_LoadFile_FrameShadingTable_Sdram( E_IQ_CALIB_TGT calib_tgt, CHAR* tbl_addr );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				Save Frame-shading correction table file to sdram.
 *
 *	@param[in]	tbl_sel_info    : Frame-shading table selection information.
 *	@param[in]	tbl_addr        : Frame-Shading table address.
 *	@param[in]	tbl_size        : Frame-Shading table tbl_size.
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_SaveFile_FrameShadingTable_Sdram( T_FJ_IQ_FSHD_SEL_INFO* tbl_sel_info, CHAR* tbl_addr, ULONG* tbl_size );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				All Shading table flip vertical.<br>
 *				This API call in Stop the processing mode (FJ_StopMode()) or FJ_MODE_IDLE.<br>
 *				After this API called, need re-convert(crop/resize) using API FJ_IQ_Conv_FrameShadingTable().<br>
 *
 *	@param[in]	vflip_on_off    : TRUE: Flip on. FALSE Flip off.
 *	@retval		FJ_ERR_OK		OK (only return OK.)
 *	@par Sample code:
 *	@code
#include <fj_iq.h>
#include <fj_mode.h>

VOID sample_fshd_flip( VOID )
{
	FJ_ERR_CODE fj_ercd;
	static T_FJ_IQ_FSHD_CONVINFO fshd_conv_info = {
		0,			// src_tbl_sta_x
		376,		// src_tbl_sta_y
		4640,		// src_tbl_width
		3104 -376,	// src_tbl_lines
		1152,		// dst_tbl_width
		666			// dst_tbl_lines
	};

	// need to load full resolution fshd table to "E_IQ_CALIB_TGT_0".

	// Stop current prossessing mode.
	FJ_StopMode();
	FJ_WaitModeComp( FJ_MODE_xxx, 10, 100 );


	// Flip vertical.
	FJ_IQ_Flip_FrameShadingTable( TRUE );	// Enable flipped
//	FJ_IQ_Flip_FrameShadingTable( FALSE );	// Disable flipped.



	// (Re)Convert table from flipped E_IQ_CALIB_TGT_0(Full resolution) to E_IQ_CALIB_TGT_1(Low resolution).
	fj_ercd = FJ_IQ_Conv_FrameShadingTable( E_IQ_CALIB_TGT_0, E_IQ_CALIB_TGT_1, &fshd_conv_info );
	if( fj_ercd != FJ_ERR_OK ) {
		return;
	}
}
 *	@endcode
 */
extern FJ_ERR_CODE FJ_IQ_Flip_FrameShadingTable( BOOL vflip_on_off );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API changes the slope gain after Frame-shading correction Video and Still image pipeline.<br>
 *	The slope gain is reflected immediately when image pipeline is active.<br>
 *
 *	@param[in]	demosaic_id		: Demosaic instance id.<br>
 *	@param[in]	slp_gain		: Slope gain parameter.<br>
 *	@retval		FJ_ERR_OK	OK
 *	@retval		FJ_ERR_NG_INPUT_PARAM : Input parameter error.
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Set_FrameShadingSlopeGain( UCHAR demosaic_id, const T_FJ_IQ_FSHD_SLP_GAIN* slp_gain );


/*==============================================================================*/
/* Function declaration(Frame-Shading L0 Compensation)							*/
/*==============================================================================*/


/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This function convert frame shading L0 table from "src" to "dst".
 *
 *	@param[in]	dst		: Destination table information.
 *	@param[in]	src		: Source table information.
 *	@retval		FJ_ERR_OK : successful.
 *	@par Sample code:
 *	@code
#include "fj_iq.h"

// Sample of enable FSHDL0 and start Video mode.
VOID sample_fshdl0_start( VOID )
{
	T_FJ_IQ_FSHDL_TBL	src = {	// 22x22 grid table.
		.ffshgfmt = 0,
		.ffshsubprh = 44,
		.ffshsubprv = 44,
		.tbl_addr[0] = 0xB0000000,	// R src table memory area address.
		.tbl_addr[1] = 0xB1000000,	// GR src table memory area address.
		.tbl_addr[2] = 0xB2000000,	// GB src table memory area address.
		.tbl_addr[3] = 0xB3000000,	// B src table memory area address.
		.tbl_g_width[0] = sizeof(USHORT) * 22,
		.tbl_g_width[1] = sizeof(USHORT) * 22,
		.tbl_g_width[2] = sizeof(USHORT) * 22,
		.tbl_g_width[3] = sizeof(USHORT) * 22,
	};
	T_FJ_IQ_FSHDL_TBL	dst = {	// 11x11 grid table.
		.ffshgfmt = 0,
		.ffshsubprh = 22,
		.ffshsubprv = 22,
		.tbl_addr[0] = 0xB4000000,	// R dst table memory area address.
		.tbl_addr[1] = 0xB5000000,	// GR dst table memory area address.
		.tbl_addr[2] = 0xB6000000,	// GB dst table memory area address.
		.tbl_addr[3] = 0xB7000000,	// B dst table memory area address.
		.tbl_g_width[0] = sizeof(USHORT) * 11,
		.tbl_g_width[1] = sizeof(USHORT) * 11,
		.tbl_g_width[2] = sizeof(USHORT) * 11,
		.tbl_g_width[3] = sizeof(USHORT) * 11,
	};
	T_FJ_IQ_FSHDL_PARAM	dst_param = {
		.ffshsubsph = 64,	// (22 -1) x 64 = 640 pixels.
		.ffshsubspv = 48,	// (22 -1) x 48 = 480 pixels.
		.ffshsubsth = 0,
		.ffshsubstv = 0,
	};
	UCHAR	demosaic_id = 0;

	// Stop current Video mode.
	FJ_StopMode();
	FJ_WaitModeComp( FJ_MODE_VIDEO, 10, 100 );

	// Initialize source table gain here.
	user_func_init_tbl_gain( &src );

	// Convert FSHDL0 table.
	FJ_IQ_Conv_FSHDL0_Table( &dst, &src );

	// Set FSHDL0 talbe.
	FJ_IQ_Set_FSHDL0_Table( demosaic_id, &dst, &dst_param );

	// Enable FSHDL0.
	FJ_IQ_OnOff_FSHDL0( demosaic_id, TRUE );

	// Setting SEN and Demosaic instance parameter if needed.
	user_func_set_instance();

	// Start video mode.
	FJ_SetMode( FJ_MODE_VIDEO );
	FJ_StartMode();
	FJ_WaitModeComp( FJ_MODE_VIDEO, 10, 100 );
}

// Sample of update immediately FSHDL0 table to image pipeline.
VOID sample_fshdl0_update_immediately( VOID )
{
	T_FJ_IQ_FSHDL_TBL	src = {	// 22x22 grid table.
		.ffshgfmt = 0,
		.ffshsubprh = 44,
		.ffshsubprv = 44,
		.tbl_addr[0] = 0xB0000000,	// R src table memory area address.
		.tbl_addr[1] = 0xB1000000,	// GR src table memory area address.
		.tbl_addr[2] = 0xB2000000,	// GB src table memory area address.
		.tbl_addr[3] = 0xB3000000,	// B src table memory area address.
		.tbl_g_width[0] = sizeof(USHORT) * 22,
		.tbl_g_width[1] = sizeof(USHORT) * 22,
		.tbl_g_width[2] = sizeof(USHORT) * 22,
		.tbl_g_width[3] = sizeof(USHORT) * 22,
	};
	T_FJ_IQ_FSHDL_TBL	dst[2] = {	// 11x11 grid table.
		{
			.ffshgfmt = 0,
			.ffshsubprh = 22,
			.ffshsubprv = 22,
			.tbl_addr[0] = 0xB4000000,	// R dst table memory area address.
			.tbl_addr[1] = 0xB5000000,	// GR dst table memory area address.
			.tbl_addr[2] = 0xB6000000,	// GB dst table memory area address.
			.tbl_addr[3] = 0xB7000000,	// B dst table memory area address.
			.tbl_g_width[0] = sizeof(USHORT) * 11,
			.tbl_g_width[1] = sizeof(USHORT) * 11,
			.tbl_g_width[2] = sizeof(USHORT) * 11,
			.tbl_g_width[3] = sizeof(USHORT) * 11,
		},
		{
			.ffshgfmt = 0,
			.ffshsubprh = 22,
			.ffshsubprv = 22,
			.tbl_addr[0] = 0xB8000000,	// R dst table memory area address.
			.tbl_addr[1] = 0xB9000000,	// GR dst table memory area address.
			.tbl_addr[2] = 0xBA000000,	// GB dst table memory area address.
			.tbl_addr[3] = 0xBB000000,	// B dst table memory area address.
			.tbl_g_width[0] = sizeof(USHORT) * 11,
			.tbl_g_width[1] = sizeof(USHORT) * 11,
			.tbl_g_width[2] = sizeof(USHORT) * 11,
			.tbl_g_width[3] = sizeof(USHORT) * 11,
		},
	};
	T_FJ_IQ_FSHDL_PARAM	dst_param = {
		.ffshsubsph = 64,	// (11 -1) x 64 = 640 pixels.
		.ffshsubspv = 48,	// (11 -1) x 48 = 480 pixels.
		.ffshsubsth = 0,
		.ffshsubstv = 0,
	};
	UCHAR	demosaic_id = 0;
	static	UINT32	dst_tbl_idx = 0;

	// Change source table gain.
	user_func_change_tbl_gain( &src );

	// Change destination table index from current to reserved.
	dst_tbl_idx = (dst_tbl_idx +1) %2;

	// Convert and set FSHDL0 table to image pipeline immediately.
	FJ_IQ_Conv_FSHDL0_Table( &dst[dst_tbl_idx], &src );

	FJ_IQ_Set_FSHDL0_Table( demosaic_id, &dst[dst_tbl_idx], &dst_param );
}
 *	@endcode
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Conv_FSHDL0_Table( T_FJ_IQ_FSHDL_TBL* dst, const T_FJ_IQ_FSHDL_TBL* src );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API changes the Frame-shading L0 compensation table for Video and Still image pipeline.<br>
 *	Table is reflected immediately when image pipeline is active.<br>
 *
 *	@param[in]	demosaic_id		: Demosaic instance id.<br>
 *	@param[in]	tbl				: Frame-shading L0 table information.
 *	@param[in]	param			: Frame-shading L0 compensation parameter.
 *	@retval		FJ_ERR_OK	OK
 *	@par Sample code:<br>
 *				Please refer the sample code of FJ_IQ_Flip_FrameShadingTable().
 */
// delete_basefw\fj\iq
//extern FJ_ERR_CODE FJ_IQ_Set_FSHDL0_Table( UCHAR demosaic_id, const T_FJ_IQ_FSHDL_TBL* tbl, const T_FJ_IQ_FSHDL_PARAM* param );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This API changes the validity of Frame-shading L0 compensation.<br>
 *				Paramter value is reflected when system mode change.
 *
 *	@param[in]	demosaic_id	: Demosaic instance id.
 *	@param[in]	onoff	: set Frame-Shading L0 compensation enable/disable.
 *	@retval		FJ_ERR_OK	OK
 *	@retval		FJ_ERR_NG_INPUT_PARAM	Parameter error.
 *	@par Sample code:<br>
 *				Please refer the sample code of FJ_IQ_Flip_FrameShadingTable().
 */
extern FJ_ERR_CODE FJ_IQ_OnOff_FSHDL0( UCHAR demosaic_id, BOOL onoff );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API changes the slope gain after Frame-shading L0 correction Video and Still image pipeline.<br>
 *	The slope gain is reflected immediately when image pipeline is active.<br>
 *
 *	@param[in]	demosaic_id		: Demosaic instance id.<br>
 *	@param[in]	slp_gain		: Slope gain parameter.<br>
 *	@retval		FJ_ERR_OK	OK
 *	@retval		FJ_ERR_NG_INPUT_PARAM : Input parameter error.
 */
extern FJ_ERR_CODE FJ_IQ_Set_FSHDL0_SlopeGain( UCHAR demosaic_id, const T_FJ_IQ_FSHDL_SLP_GAIN* slp_gain );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Set iq table force reload.
 *
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 */
extern FJ_ERR_CODE FJ_IQ_Tbl_Set_Force_Reload( VOID );

/* @} */

#ifdef __cplusplus
}
#endif

#endif	// _FJ_IQ_H_

