/**
 * @file		fj_flck.h
 * @brief		Flicker Setting API.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_FLCK_H
#define _FJ_FLCK_H

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_awb.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_flck_overview
@{
	FLCK module generates evaluation value of RAW(Detection) used to Flicker cancel correction.<br>
	- Supported Function<br>
	. A setup of FLCK detection.<br>
@}*//* --- end of fj_flck_overview */


/******************************************************************************
 Fixed Define
******************************************************************************/
/* Nothing Special */

/******************************************************************************
 Enumeration
******************************************************************************/
/* Nothing Special */

/******************************************************************************
 Structure
******************************************************************************/
/** @weakgroup fj_flck_definition
@{*/

// FJ_SetFLCKWindow
/** FLCK Window Informaion	*/
typedef struct {
	USHORT	pos_x;					/**< FLCK Window start position X							value range:[0 - 8190](Multiples of 2)	*/
	USHORT	pos_y;					/**< FLCK Window start position Y							value range:[0 - 8190](Multiples of 2)<br>	Please set up pos_y + height * blk_num not to exceed 8192. 	*/
	USHORT	width;					/**< FLCK Window horizontal size							value range:[6 - 8192] 2pixel boundary<br>	*/
	USHORT	lines;					/**< FLCK Window vertical size								value range:[2 - 1024] 2pixel boundary(wd_mode=0)/4pixel boundary(wd_mode=1)<br>	Please set up pos_y + height * blk_num not to exceed 8192.	*/
	USHORT	effective_lines;		/**< FLCK Window effective vertical size(for integration)	value range:[2 - 1024] 2pixel boundary(wd_mode=0)/4pixel boundary(wd_mode=1)<br>	Please set up pos_y + height * blk_num not to exceed 8192.	*/
	USHORT	blk_num;				/**< FLCK Window Window block number						value range:[1 - 4096]<br>	Please set up pos_y + height * blk_num not to exceed 8192.					*/
} FLCKWinInfo;

// FJ_SetFLCKLevelGate
/** FLCK level gate value */
typedef struct {
	Gatepix	FLCKGateUpper;			/**< Level Gate Upper	*/
	Gatepix	FLCKGateLower;			/**< Level Gate Lower	*/
} FLCKLevelGate;

// FJ_SetFLCKMatrixLevelGate
/** FLCK Matrix level gate value */
typedef struct {
	MtxGatepix	FLCKMtxGateUpper;			/**< Matrix Level Gate Upper	*/
	MtxGatepix	FLCKMtxGateLower;			/**< Matrix Level Gate Lower	*/
	SHORT		FLCKMtxValue[3][3];			/**< Matrix Value				*/
} FLCKMtxLevelGate;

// FJ_SetFLCKComputeValueRatio
/** FLCK compute value ratio	*/
typedef struct {
	SHORT	rRatio;					/**< R  pixel ratio.(step is 1/256)	*/
	SHORT	grRatio;				/**< Gr pixel ratio.(step is 1/256)	*/
	SHORT	gbRatio;				/**< Gb pixel ratio.(step is 1/256)	*/
	SHORT	bRatio;					/**< B  pixel ratio.(step is 1/256)	*/
} FLCKComputeRationInfo;

/* @} */	// fj_flck_definition group

/******************************************************************************
 ProtoType
******************************************************************************/
/** @weakgroup fj_flck_api
@{*/

/********************** FLCK ******************************/
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set FLCK Window
@remarks	Sample image of FLCK window setting is shown below.
@param[in]	flck_id	: FLCK ID. value range is [0..1]<br>
@param[in]	wininfo	: FLCK window information. See @ref FLCKWinInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig) Image of FLCK window setting is shown below:<br>
			@image html fj_flck_window.png Fig) FLCK Window<br>
*/
extern FJ_ERR_CODE FJ_SetFLCKWindow( UCHAR flck_id, FLCKWinInfo* wininfo );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get FLCK Window
@param[in]	flck_id	: FLCK ID. value range is [0..1]<br>
@param[out]	wininfo	: FLCK window information. See @ref FLCKWinInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_GetFLCKWindow( UCHAR flck_id, FLCKWinInfo* wininfo );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set FLCK Data format type. 
It is API for Selecting the format of FLCK output data.
@param[in]	flck_id		: FLCK ID. value range is [0..1]<br>
@param[in] format_type	: FLCK output data format type [ 0:detecting RGB and count 1:detecting Y/Gr/Gb and count ]
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig) Image of FLCK Data format type:<br>
			@image html fj_flck_dataformat.png Fig) FLCK Data format type
*/
extern FJ_ERR_CODE FJ_SetFLCKDataformat( UCHAR flck_id, UCHAR format_type );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get FLCK Data format type. 
It is API for getting the current format of FLCK output data.
@param[in]	flck_id		: FLCK ID. value range is [0..1]<br>
@param[out] format_type	: FLCK output data format type [ 0:detecting RGB and count 1:detecting Y/Gr/Gb and count ]
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_GetFLCKDataformat( UCHAR flck_id, UCHAR* format_type );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set FLCK Detection mode. 
It is API for Selecting the Detection mode of FLCK.
@param[in]	flck_id	: FLCK ID. value range is [0..1]<br>
@param[in] wd_mode	: FLCK Detection mode [ 0:Normal detection 1:Even / Odd line distinction detection ]
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig) Image of FLCK Detection mode:<br>
			@image html fj_flck_detectionmode.png Fig) FLCK Detection mode
*/
extern FJ_ERR_CODE FJ_SetFLCKWDMode( UCHAR flck_id, UCHAR wd_mode );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get FLCK Detection mode. 
It is API for getting the current Detection mode of FLCK.
@param[in]	flck_id	: FLCK ID. value range is [0..1]<br>
@param[out] wd_mode	: FLCK Detection mode [ 0:Normal detection 1:Even / Odd line distinction detection ]
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_GetFLCKWDMode( UCHAR flck_id, UCHAR* wd_mode );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API used to set RGB level gate threshold value in the process unit before FLCK detection.<br>
The threshold value is 8 bits, and compared with the upper 8 bits of the input data.
@param[in]	flck_id	: FLCK ID. value range is [0..1]<br>
@param[in]	gate	: FLCK level gate value. See @ref FLCKLevelGate<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig) Image of RGB Level Gate:<br>
			@image html fj_flck_rgb_levelgate.png Fig) RGB Level Gate
*/
extern FJ_ERR_CODE FJ_SetFLCKLevelGate( UCHAR flck_id, FLCKLevelGate* gate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get RGB level gate threshold value in the process unit before FLCK detection.
@param[in]	flck_id	: FLCK ID. value range is [0..1]<br>
@param[out]	gate	: FLCK level gate value. See @ref FLCKLevelGate<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_GetFLCKLevelGate( UCHAR flck_id, FLCKLevelGate* gate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API used to set FLCK matrix level gate threshold value in the process unit before FLCK detection.<br>
The threshold value is 8 bits, and compared with the matrix calculation result 8 bits.
@param[in]	flck_id		: FLCK ID. value range is [0..1]<br>
@param[in]	mtx_gate	: FLCK Matrix level gate value. See @ref FLCKMtxLevelGate<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig) Image of Matrix Level Gate:<br>
			@image html fj_flck_matrix_levelgate.png Fig) Matrix Level Gate
*/
extern FJ_ERR_CODE FJ_SetFLCKMatrixLevelGate( UCHAR flck_id, FLCKMtxLevelGate* mtx_gate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get FLCK matrix level gate threshold value in the process unit before FLCK detection.
@param[in]	flck_id		: FLCK ID. value range is [0..1]<br>
@param[out]	mtx_gate	: FLCK Matrix level gate value. See @ref FLCKMtxLevelGate<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_GetFLCKMatrixLevelGate( UCHAR flck_id, FLCKMtxLevelGate* mtx_gate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API used to set the coefficients of luminance calculation equation used for FLCK detection.<br>
Available range of coefficients is as below:<br>
-256 ~ 255(-1~0.99609375): 1step = 1/256 = 0.00390625
@remarks	Use of this API is optional.<br>
			Unless Host uses this API, following default values to be used.<br>
@param[in]	flck_id	: FLCK ID. value range is [0..1]<br>
@param[in]	flckcmp	: See @ref FLCKComputeRationInfo<br>
			<table border=0><tr><td>rRatio = 76</td><td>(76/256 = 0.3)</td></tr>
			<tr><td>grRatio = 74</td><td>(74/256 = 0.29)</td></tr>
			<tr><td>gbRatio = 74</td><td>(74/256 = 0.29)</td></tr>
			<tr><td>bRatio = 28</td><td>(28/256 = 0.11)</td></tr></table>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig) Image of Y generation:<br>
			@image html fj_flck_compratio.png Fig) Y generation<br>
*/
extern FJ_ERR_CODE FJ_SetFLCKComputeValueRatio( UCHAR flck_id, FLCKComputeRationInfo* flckcmp );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the coefficients of luminance calculation equation used for FLCK detection.<br>
Available range of coefficients is as below:<br>
-256 ~ 255(-1~0.99609375): 1step = 1/256 = 0.00390625
@remarks	Use of this API is optional.<br>
			Unless Host uses this API, following default values to be used.<br>
@param[in]	flck_id	: FLCK ID. value range is [0..1]<br>
@param[out]	flckcmp	: See @ref FLCKComputeRationInfo<br>
			<table border=0><tr><td>rRatio = 76</td><td>(76/256 = 0.3)</td></tr>
			<tr><td>grRatio = 74</td><td>(74/256 = 0.29)</td></tr>
			<tr><td>gbRatio = 74</td><td>(74/256 = 0.29)</td></tr>
			<tr><td>bRatio = 28</td><td>(28/256 = 0.11)</td></tr></table>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_GetFLCKComputeValueRatio( UCHAR flck_id, FLCKComputeRationInfo* flckcmp );

/* @} */	// fj_flck_api group
#endif
