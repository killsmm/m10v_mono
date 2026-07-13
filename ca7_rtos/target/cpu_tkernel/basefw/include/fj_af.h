/**
 * @file		fj_af.h
 * @brief		Auto Focus Setting API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_AF_H
#define _FJ_AF_H

#include "fj_ae.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_af_overview
@{
	AF module generates evaluation value of RAW(Detection) used to Auto Focus function.<br>
 	AF detection function consists of the following 6 blocks.<br>
 	- Function<br>
	. AF Main Control Block<br> 
	. AF Luminance Calculation Block<br> 
	. AF Vertical Window Block<br>
	. AF Horizontal Calculation Block<br>
	. AF Window Block<br>
	@image html fj_af_all_block.png

@}*//* --- end of fj_af_overview */

/******************************************************************************
 Fixed Define
******************************************************************************/
/** @weakgroup fj_af_definition
@{*/
/** AF Window Number
 */
#define D_AF_WINDOW_MAX	(14)
#define D_AF_LUMINANCE_INFO_MAX	(2)
#define D_AF_HORIZONTAL_TAP_MAX	(2)

/******************************************************************************
 Enumeration
******************************************************************************/
/**	AF data type
 */
typedef enum {
	FJ_AF_DATA_TYPE_H_WD_0G_A = 0,		/**< AF Horizontal calculation block0 group A output data */
	FJ_AF_DATA_TYPE_H_WD_0G_B,			/**< AF Horizontal calculation block0 group B output data */
	FJ_AF_DATA_TYPE_H_WD_1G_A,			/**< AF Horizontal calculation block1 group A output data */
	FJ_AF_DATA_TYPE_H_WD_1G_B,			/**< AF Horizontal calculation block1 group B output data */
	FJ_AF_DATA_TYPE_H_WD_2G_A,			/**< AF Horizontal calculation block2 group A output data */
	FJ_AF_DATA_TYPE_H_WD_2G_B,			/**< AF Horizontal calculation block2 group B output data */
	FJ_AF_DATA_TYPE_V_WD,				/**< AF Vertical Window output data						  */
	FJ_AF_DATA_TYPE_LUMI_0,				/**< AF Luminance calculation block0 output data		  */
	FJ_AF_DATA_TYPE_LUMI_1,				/**< AF Luminance calculation block1 output data		  */
	FJ_AF_DATA_TYPE_LUMI_2,				/**< AF Luminance calculation block2 output data		  */
	FJ_AF_DATA_TYPE_MAX					/**< Stopper											  */
} FJ_AF_DATA_TYPE;


/******************************************************************************
 Structure
******************************************************************************/
/** Window size information */
typedef struct {
	USHORT	width;					/**< AF Window horizontal size.		value range:[2 - 2496]  2pixel boundary	*/
	USHORT	height;					/**< AF Window vertical size.		value range:[2 - 16382] 2pixel boundary	*/
	USHORT	pos_x;					/**< AF Window start position X.	value range:[0 - 8190]  (Multiples of 2)	*/
	USHORT	pos_y;					/**< AF Window start position Y.	value range:[0 - 16382] (Multiples of 2)	*/
} AFWinRect;

/** AF Window information structure */
typedef struct {
	FJ_AF_DATA_TYPE		horizontal_data_input_select[3];	/**< AF Window input data select<br>
																 value range:<br>
																 [ FJ_AF_DATA_TYPE_H_WD_0G_A<br>
																   FJ_AF_DATA_TYPE_H_WD_0G_B<br>
																   FJ_AF_DATA_TYPE_H_WD_1G_A<br>
																   FJ_AF_DATA_TYPE_H_WD_1G_B<br>
																   FJ_AF_DATA_TYPE_H_WD_2G_A<br>
																   FJ_AF_DATA_TYPE_H_WD_2G_B ]	*/
 	FJ_AF_DATA_TYPE		max_min_data_input_select;			/**< AF Max/Min data type select<br>
																 value range:<br>
																 [ FJ_AF_DATA_TYPE_H_WD_0G_A<br>
																   FJ_AF_DATA_TYPE_H_WD_0G_B<br>
																   FJ_AF_DATA_TYPE_H_WD_1G_A<br>
																   FJ_AF_DATA_TYPE_H_WD_1G_B<br>
																   FJ_AF_DATA_TYPE_H_WD_2G_A<br>
																   FJ_AF_DATA_TYPE_H_WD_2G_B<br>
																   FJ_AF_DATA_TYPE_V_WD ]	*/
	FJ_AF_DATA_TYPE		luminance_data_input_select;		/**< AF Window input luminance data select<br>
																 value range:<br>
																 [ FJ_AF_DATA_TYPE_LUMI_0<br>
																   FJ_AF_DATA_TYPE_LUMI_1<br>
																   FJ_AF_DATA_TYPE_LUMI_2 ]	*/
	AFWinRect			rect;								/**< AF Window size	*/
	USHORT				bright_threshold;					/**< AF Luminance data threshold(bright) value range:[ 0 ~ 4095 ]	*/
	USHORT				dark_threshold;						/**< AF Luminance data threshold(dark)   value range:[ 0 ~ 4095 ]	*/
} AFWinInfo;

/** AF Luminance calculation block information structure */
typedef struct {
	USHORT	matrix_value[4];			/**< AF Luminance matrix value R[0]/Gr[1]/Gb[2]/B[3]. value range:[ 0x0200(-2.0) ~ 0x01FF(+1.99609375) 1step = 1/256]	*/
	USHORT	knee_tbl_border[7];			/**< Knee table border value. value range:[ 0 ~ 16383 ]	*/
	USHORT	knee_tbl_inc[8];			/**< Knee table inclination value. value range:[ 0x0000(0.0) ~ 0x03FF(3.99609375) 1step = 1/256 ]	*/
	USHORT	knee_tbl_offset[8];			/**< Knee table offset value. value range:[ 0 ~ 4095 ]	*/
	UCHAR	median_filter_enable;		/**< Median filter enable. 0:disable 1:enable	*/
	UCHAR	knee_enable;				/**< Knee enable. 0:disable 1:enable	*/
} AFLumiInfo;

/** AF Vertical Window information structure */
typedef struct {
	FJ_AF_DATA_TYPE	vertical_data_input_select;		/**< Vertical window input data select<br>
														 value range:<br>
														 [ FJ_AF_DATA_TYPE_LUMI_0<br>
														   FJ_AF_DATA_TYPE_LUMI_1<br>
														   FJ_AF_DATA_TYPE_LUMI_2 ]	*/
	USHORT			horizontal_start_pos;			/**< Vertical window start position X. value range:[ 0,2,4, ~ 6270 ] 1step = 2	*/
	USHORT			horizontal_width;				/**< Vertical window horizontal width. value range:[ 2,4,6, ~ 784 ] 1step = 2	*/
	USHORT			iir_filter_coring_threshold;	/**< IIR Filter coring threshold value. value range:[ 0 ~ 4095 ]	*/
	USHORT			iir_filter_coring_level;		/**< IIR Filter coring level. value range:[ 0 ~ 4095 ]	*/
	INT16			iir_gain_coefficient;			/**< IIR Gain coefficient. value range:[ 0x4000(-64.0) ~ 0x3FFF(63.99609375) 1step = 1/256 ]	*/
	INT8			iir_filter_a_value[3];			/**< IIR Filter A0[0]/A1[1]/A2[2] coefficient.<br>
														 A0 value range:[ 0x80(-4.0) ~ 0x7F(3.96875) 1step = 1/32 ]<br>
														 A1 value range:[ 0x80(-4.0) ~ 0x7F(3.96875) 1step = 1/32 ]<br>
														 A2 value range:[ 0x80(-4.0) ~ 0x7F(3.96875) 1step = 1/32 ]	*/
	INT16			iir_filter_b_value[2];			/**< IIR Filter B1[0]/B2[1] coefficient.<br>
														 B1 value range:[ 0x4000(-64.0) ~ 0x3FFF(63.99609375) 1step = 1/256 ]<br>
														 B2 value range:[ 0x4000(-64.0) ~ 0x3FFF(63.99609375) 1step = 1/256 ]	*/
	SHORT			fir_filter_value[3];			/**< FIR Filter coefficient Tap1[0],Tap2[1],Tap3[2]<br>
														 value range:[ 0x80(-2.0) ~ 0x7F(+1.984375) 1step = 1/64 ]	*/
	UCHAR			horizontal_update_cycle;		/**< Horizontal update cycle. value range:[ 0(=16), 1 ~ 15 ]	*/
} AFVWinInfo;

/** AF Horizontal group A/B */
typedef struct {
	FJ_AF_DATA_TYPE	horizontal_data_input_select;	/**< Horizontal calculation block input data type select<br>
														 value range:<br>
														 [ FJ_AF_DATA_TYPE_LUMI_0<br>
														   FJ_AF_DATA_TYPE_LUMI_1<br>
														   FJ_AF_DATA_TYPE_LUMI_2 ]	*/
	USHORT			fir_filter_coring_level;		/**< FIR Filter coring level. value range:[ 0 ~ 4095 ]	*/
	USHORT			coring_tbl_border[3];			/**< FIR Filter coring table border value. value range:[ 0 ~ 4095 ]	*/
	USHORT			coring_tbl_offset[4];			/**< FIR Filter coring table offset value. value range:[ 0 ~ 4095 ]	*/
	INT16			coring_tbl_inc[4];				/**< FIR Filter coring table inclination value. value range:[ 0x0800(-2.0) ~ 0x07FF(+1.9990234375) 1step = 1/1024 ]	*/
	UCHAR			iir_filter_enable;				/**< IIR Filter enable. 0:disable 1:enable	*/
	INT16			iir_gain_coefficient;			/**< IIR Gain coefficient. value range:[ 0x4000(-64.0) ~ 0x3FFF(63.99609375) 1step = 1/256 ]	*/
	INT8			iir_filter_a_value[3];			/**< IIR Filter A0[0]/A1[1]/A2[2] coefficient.<br>
														 A0 value range:[ 0x80(-4.0) ~ 0x7F(3.96875) 1step = 1/32 ]<br>
														 A1 value range:[ 0x80(-4.0) ~ 0x7F(3.96875) 1step = 1/32 ]<br>
														 A2 value range:[ 0x80(-4.0) ~ 0x7F(3.96875) 1step = 1/32 ]	*/
	INT16			iir_filter_b_value[2];			/**< IIR Filter B1[0]/B2[1] coefficient.<br>
														 B1 value range:[ 0x4000(-64.0) ~ 0x3FFF(63.99609375) 1step 1/256 ]<br>
														 B2 value range:[ 0x4000(-64.0) ~ 0x3FFF(63.99609375) 1step 1/256 ]	*/

	UCHAR			fir_filter_enable;				/**< FIR Filter enable. 0:disable 1:enable */
	INT8			fir_filter_value[5];			/**< FIR Filter 1~5tap coefficient. value range:[ 0x80(-2.0) ~ 0x7F(+1.984375) 1step = 1/64 ]	*/
	UCHAR			fir_filter_gain;				/**< FIR Filter gain value. value range:[ 0x00(0.0) ~ 0xFF(3.984375) 1step = 1/64 ]	*/
} AFHGroup;

/** AF Horizontal calculation block information structure */
typedef struct {
	AFHGroup	afh_gp[2];	/**< Horizontal Group A/B */
} AFHInfo;

/** AF block power status */
typedef struct {
	UCHAR	lumi_block[3];			/**< AF Luminance calculation block 0~2tap power enable. 0:disable 1:enable	*/
	UCHAR	vertical_block;			/**< AF Vertical Window power enable. 0:disable 1:enable	*/
	UCHAR	horizon_block[3];		/**< AF Horizontal calculation block 0~2tap power enable. 0:disable 1:enable	*/
	UCHAR	afwin[D_AF_WINDOW_MAX+1];	/**< AF Window( 0 ~ 14 ) power enable. 0:disable 1:enable	*/
	UCHAR	globalwin;				/**< AF Global Window power enable. 0:disable 1:enable	*/
} AFPowerState;

/* @} */	// fj_af_definition group

/******************************************************************************
 ProtoType
******************************************************************************/
///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_af_api
@{*/
/********************** AF  ******************************/
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set AF Window.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: Target AF Window number. value range:[0 ~ 14]
@param[in]	af_wininfo	: AF Window information. See @ref AFWinInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig.1) Image of AF window setting is shown below:<br>
			@image html fj_af_window.png Fig.1) AF Window<br>
			Fig.2) AF Window Block details.<br>
			@image html fj_af_window_block.png Fig.2) AF Window Block details<br>
			Fig.3) AFWinInfo setting sample code<br>
			@image html fj_af_window_block_sample.png Fig.3) AF Window Block details sample<br>
@code
			AFWinInfo	af_wininfo;
			UCHAR af_id = 0;

			af_wininfo.horizontal_data_input_select[0] = FJ_AF_DATA_TYPE_H_WD_0G_B;
			af_wininfo.horizontal_data_input_select[1] = FJ_AF_DATA_TYPE_H_WD_1G_B;
			af_wininfo.horizontal_data_input_select[2] = FJ_AF_DATA_TYPE_H_WD_2G_B;
			af_wininfo.luminance_data_input_select     = FJ_AF_DATA_TYPE_LUMI_1;
			af_wininfo.max_min_data_input_data_select  = FJ_AF_DATA_TYPE_V_WD;
			af_wininfo.bright_threshold = 4000;
			af_wininfo.dark_threshold   = 100;
			af_wininfo.rect.pos_x  = 120;
			af_wininfo.rect.pos_y  = 120;
			af_wininfo.rect.width  = 320;
			af_wininfo.rect.height = 240;
			
			FJ_AFSetWindow( af_id, 0, &af_wininfo );
@endcode
*/
extern FJ_ERR_CODE FJ_AFSetWindow( UCHAR af_id, UCHAR af_win_no, AFWinInfo* af_wininfo );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set AF Global Window.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	afg_window	: AF Global Window information See @ref AFWinRect
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig) Image of AF global window setting is shown below:<br>
			@image html fj_af_global_window.png Fig) AF Global Window<br>
*/
extern FJ_ERR_CODE FJ_AFSetGlobalWindow( UCHAR af_id, AFWinRect* afg_window );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set AF Luminance calculation block.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	tap_no		: Luminance calculation block no. value range:[0~2]
@param[in]	af_lumiinfo	: Luminance calculaton block information. See @ref AFLumiInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig.1) Image of AF Luminance calculation block detail:<br>
			@image html fj_af_luminance_block.png Fig.1) AF Luminance Calculation Block<br>
			Fig.2) Knee Table image:<br>
			@image html fj_af_luminance_block_knee_tbl.png Fig.2) Knee Table<br>
*/
extern FJ_ERR_CODE FJ_AFSetLuminanceCalc( UCHAR af_id, UCHAR tap_no, AFLumiInfo* af_lumiinfo );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set AF Vertical Window.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	afv_wininfo	: Vertical Window information. See @ref AFVWinInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig.1) Image of AF Vertical window setting is shown below:<br>
			@image html fj_af_vertical_window.png Fig.1) AF Vertical Window<br>
			Fig.2) Image of AF Vertical window block details:<br>
			@image html fj_af_vertical_window_block.png Fig.2) AF Vertical Window Block<br>
*/
extern FJ_ERR_CODE FJ_AFSetVerticalWindow( UCHAR af_id, AFVWinInfo* afv_wininfo );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set AF Horizontal calculation block.
@param[in]	af_id			: AF ID. value range is [0..1]<br>
@param[in]	tap_no			: Horizontal calculation block no. value range:[0~2]
@param[in]	af_horizoninfo	: Horizontal calculaton block information. See @ref AFHInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig.1) Image of AF Horizontal calculation block:<br>
			@image html fj_af_horizontal_block.png Fig.1) AF Horizontal Calculation Block<br>
			Fig.2) Image of Coring table:<br>
			@image html fj_af_horizontal_coring_tbl.png Fig.2) Coring Table<br>
@code
			AFHInfo af_horizoninfo;
			UCHAR af_id = 0;
			
			// AF Horizontal Block Group A( Tap = 0 )
			af_horizoninfo.afh_gp[0].horizontal_data_input_select = FJ_AF_DATA_TYPE_LUMI_0;
			af_horizoninfo.afh_gp[0].fir_filter_coring_level	= 200;
			
			af_horizoninfo.afh_gp[0].coring_tbl_border[0]		= 300;
			af_horizoninfo.afh_gp[0].coring_tbl_border[1]		= 2048;
			af_horizoninfo.afh_gp[0].coring_tbl_border[2]		= 4095;
			
			af_horizoninfo.afh_gp[0].coring_tbl_offset[0]		= 20;
			af_horizoninfo.afh_gp[0].coring_tbl_offset[1]		= 1800;
			af_horizoninfo.afh_gp[0].coring_tbl_offset[2]		= 2200;
			af_horizoninfo.afh_gp[0].coring_tbl_offset[3]		= 4095;
			
			af_horizoninfo.afh_gp[0].coring_tbl_inc[0]			= 0x0460;	// (=1.09375)
			af_horizoninfo.afh_gp[0].coring_tbl_inc[1]			= 0x0500;	// (=1.25)
			af_horizoninfo.afh_gp[0].coring_tbl_inc[2]			= 0x0140;	// (=0.3125)
			af_horizoninfo.afh_gp[0].coring_tbl_inc[3]			= 0x0600;	// (=1.5)

			af_horizoninfo.afh_gp[0].iir_filter_enable 			= 1;
			af_horizoninfo.afh_gp[0].iir_gain_coefficient		= 0x4F00;	// (=-49.0)
			af_horizoninfo.afh_gp[0].iir_filter_a_value[0]		= 0xFF;		// (=-0.3125)
			af_horizoninfo.afh_gp[0].iir_filter_a_value[1]		= 0x18;		// (=0.75)
			af_horizoninfo.afh_gp[0].iir_filter_a_value[2]		= 0x60;		// (=3.0)
			af_horizoninfo.afh_gp[0].iir_filter_b_value[0]		= 0x4000;	// (=-64.0)
			af_horizoninfo.afh_gp[0].iir_filter_b_value[1]		= 0x00FF;	// (=0.99609375)
			
			af_horizoninfo.afh_gp[0].fir_filter_enable 			= 1;
			af_horizoninfo.afh_gp[0].fir_filter_value[0] 		= 0x40;	// (=1.0)
			af_horizoninfo.afh_gp[0].fir_filter_value[1]		= 0x45;	// (=1.078125)
			af_horizoninfo.afh_gp[0].fir_filter_value[2]		= 0x50;	// (=1.25)
			af_horizoninfo.afh_gp[0].fir_filter_value[3]		= 0x55;	// (=1.328125)
			af_horizoninfo.afh_gp[0].fir_filter_value[4]		= 0x60;	// (=1.5)
			
			af_horizoninfo.afh_gp[0].fir_filter_gain			= 0x40;	// (=1.0)

			// AF Horizontal Block Group B( Tap = 0 )
			memcpy( &af_horizoninfo.afh_gp[1], &af_horizoninfo.afh_gp[0], sizeof(AFHGroup) );
			af_horizoninfo.afh_gp[1].horizontal_data_input_select = FJ_AF_DATA_TYPE_LUMI_2;
			
			FJ_AFSetHorizontalCalc( af_id, 0, &af_horizoninfo );
@endcode
*/
extern FJ_ERR_CODE FJ_AFSetHorizontalCalc( UCHAR af_id, UCHAR tap_no, AFHInfo* af_horizoninfo );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set AF Block Power enable.
@param[in]	af_id			: AF ID. value range is [0..1]<br>
@param[in]	af_pow_state	: AF Block power status. See @ref AFPowerState
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@attention  By PRO(AF) macro specifications, af_pow_state->lumi_block[0] and af_pow_state->horizon_block[0] setting will be enabled forcibly.<br><br>
*/
extern FJ_ERR_CODE FJ_AFSetPowerEnable( UCHAR af_id, AFPowerState* af_pow_state );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Window Luminance data.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: Luminance data
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetLuminanceData( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Window Luminance bright count.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: Luminance bright count.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetLuminanceBrightCount( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Window Luminance dark count.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: Luminance dark count.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetLuminanceDarkCount( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Window Horizontal data0.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: Horizontal data0.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetHorizontalData0( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Window Horizontal data1.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: Horizontal data1.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetHorizontalData1( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Window Horizontal data2.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: Horizontal data2.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetHorizontalData2( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Window Vertical data.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: Vertical data.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetVerticalData( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Window Luminance data max value.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: Luminance data max value.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetLuminanceDataMaxValue( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Window Luminance data min value.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: Luminance data min value.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetLuminanceDataMinValue( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Window Evaluation data max value.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: Evaluation data max value.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetDataMaxValue( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Window Evaluation data min value.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: Evaluation data min value.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetDataMinValue( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Detection Window Horizontal0 Detection Each Line Maximum Value Accumulation result.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: AF Detection Window Horizontal0 Detection Each Line Maximum Value Accumulation result.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetHorizontalLineAccumMaxData0( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Detection Window Horizontal1 Detection Each Line Maximum Value Accumulation result.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: AF Detection Window Horizontal1 Detection Each Line Maximum Value Accumulation result.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetHorizontalLineAccumMaxData1( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AF Detection Window Horizontal2 Detection Each Line Maximum Value Accumulation result.
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	af_win_no	: AF Window number value range:[ 0 ~ 14 ]
@param[out]	data		: AF Detection Window Horizontal2 Detection Each Line Maximum Value Accumulation result.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFGetHorizontalLineAccumMaxData2( UCHAR af_id, UCHAR af_win_no, ULONG* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Calculation of AF statistics is controlled by this API when NOT specified "f" by FJ_SetStatisticPeriod().<br>
If AF statistics is enabled by this API,Base FW calculate AF statistics every frame,<br>
and callback function FJ_HostAFCB() every frame.<br><br>
@param[in]	af_id		: AF ID. value range is [0..1]<br>
@param[in]	on_off	: AF statistics calculation switch<br>
			0:Disabling AF Statistics<br>
			1:Enabling AF Statistics<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AFSetCalcDataReady( UCHAR af_id, UCHAR on_off );

/* @} */	// fj_af_api group
#endif
