/**
 * @file		fj_ae.h
 * @brief		Auto Exposure Setting API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_AE_H
#define _FJ_AE_H

#include "fj_std.h"
#include "fj_custom.h"
#include "fj_still.h"

//#define AEAWB_COMPACT_STATISTICS	// output compact statistics data

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_ae_overview
@{
	AE module generates evaluation value of RAW(Detection) used to Auto Exposure function.<br>
	- Supported Function <br>
	. A setup of AE detection.<br>
@}*//* --- end of fj_ae_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_ae_sequence
@{
- @ref fj_still_3A_ae_sequence
	- @ref fj_still_3a_ae_sequence_section1
	- @ref fj_still_3a_ae_sequence_section2
	- @ref fj_still_3a_ae_sequence_section4
	- @ref fj_still_3a_ae_sequence_section5

<hr>
@section fj_still_3A_ae_sequence	3A Sequence
@subsection fj_still_3a_ae_sequence_section1 3A-1[Boot]
@image html fj_3a_boot.png

<hr>
@subsection fj_still_3a_ae_sequence_section2 3A-2[Video]
@image html fj_3a_video.png

<hr>
@subsection fj_still_3a_ae_sequence_section4 3A-3[Still]
@image html fj_3a_still_single.png

<hr>
@subsection fj_still_3a_ae_sequence_section5 3A-4[Burst]
@image html fj_3a_still_burst.png
@}*//* --- end of fj_ae_sequence */


/******************************************************************************
 Fixed Define
******************************************************************************/
/** @weakgroup fj_ae_definition
@{*/

/******************************************************************************
 Enumeration
******************************************************************************/
// --- REMOVE_CAMERAIF BEGIN (simple tool) ---
// AE BIN index
typedef enum {
	E_BF_AE_BIN_IDX_NULL = -1,
	E_BF_AE_BIN_IDX_COMMON= 0,
	E_BF_AE_BIN_IDX_STILL_CAP,
	E_BF_AE_BIN_IDX_STILL_MON,
	E_BF_AE_BIN_IDX_HD_VIDEO,
	E_BF_AE_BIN_IDX_FHD_VIDEO,
	E_BF_AE_BIN_IDX_4K_VIDEO,
	E_BF_AE_BIN_IDX_360_VIDEO,
	E_BF_AE_BIN_IDX_7,
	E_BF_AE_BIN_IDX_MAX
} E_BF_AE_BIN_IDX;

//AWB BIN index
typedef enum {
	E_BF_AWB_BIN_IDX_NULL = -1,
	E_BF_AWB_BIN_IDX_Common= 0,
	E_BF_AWB_BIN_IDX_MAX
} E_BF_AWB_BIN_IDX;
// --- REMOVE_CAMERAIF END (simple tool) ---
/******************************************************************************
 Structure
******************************************************************************/
// FJ_SetAEAWBWindow
/** AE/AWB Window Informaion	*/
typedef struct {
	USHORT	WindowBlockCntX;					/**< horizontal block count in the AE/AWB accumulation window. 													value range:[1~64 ]	*/
	USHORT	WindowBlockCntY;					/**< vertical block count in the AE/AWB accumulation window.													value range:[1~64 ]	*/
	USHORT	WindowStartX;						/**< horizontal start address of the AEAWB accumulation according to the valid HD edge reference.				value range:[0~8190, and the value must be a multiple of 2]	*/
	USHORT	WindowStartY;						/**< vertical start address of the AEAWB accumulation according to the valid VD edge reference.					value range:[0~8190, and the value must be a multiple of 2]	*/
	USHORT	WindowSizeX;						/**< horizontal pixel count of one block, a unit for AE/AWB accumulation. 										value range:[6~512 pixels and in units of 2 pixels. ]	*/
	USHORT	WindowSizeY;						/**< vertical line count of one block, a unit for AEAWB accumulation. 											value range:[2~256 lines and in units of 2 lines]	*/
	USHORT	WindowSizeLeftEdgeX;				/**< horizontal pixel count of the left edge block inside one block which is a unit for AE/AWB accumulation.	value range:[6~512 pixels and in units of 2 pixels]*/
	USHORT	WindowSizeRightEdgeX;				/**< horizontal pixel count of the right edge block inside one block which is a unit for AE/AWB accumulation.	value range:[6~512 pixels and in units of 2 pixels]*/
	USHORT	WindowSizeTopY;						/**< vertical line count of the top block inside one block which is a unit for AE/AWB accumulation.				value range:[2~256 lines and in units of 2 lines]	*/
	USHORT	WindowSizeBottomY;					/**< vertical line count of the bottom block inside one block which is a unit for AE/AWB accumulation.			value range:[2~256 lines and in units of 2 lines]	*/

	USHORT	AEWindowTargetSizeX;				/**< horizontal sizes of the actual AE detection accumulation target in one block								value range:[6~512 pixels and in units of 2 pixels. ]	*/
	USHORT	AEWindowTargetSizeY;				/**< vertical sizes of the actual AE detection accumulation target in one block									value range:[2~256 lines and in units of 2 lines]	*/
	USHORT	AEWindowTargetSizeLeftEdgeX;		/**< horizontal sizes of the actual AE detection accumulation target in left edge block of one block.			value range:[6~512 pixels and in units of 2 pixels]*/
	USHORT	AEWindowTargetSizeRightEdgeX;		/**< horizontal sizes of the actual AE detection accumulation target in right edge block of one block.			value range:[6~512 pixels and in units of 2 pixels]*/
	USHORT	AEWindowTargetSizeTopY;				/**< vertical sizes of the actual AE detection accumulation target in top block of one block.					value range:[2~256 lines and in units of 2 lines]	*/
	USHORT	AEWindowTargetSizeBottomY;			/**< vertical sizes of the actual AE detection accumulation target in bottom block of one block.				value range:[2~256 lines and in units of 2 lines]	*/

	USHORT	AWBWindowTargetSizeX;				/**< horizontal sizes of the actual AWB detection accumulation target in one block								value range:[6~512 pixels and in units of 2 pixels. ]	*/
	USHORT	AWBWindowTargetSizeY;				/**< vertical sizes of the actual AWB detection accumulation target in one block									value range:[2~256 lines and in units of 2 lines]	*/
	USHORT	AWBWindowTargetSizeLeftEdgeX;		/**< horizontal sizes of the actual AWB detection accumulation target in left edge block of one block.			value range:[6~512 pixels and in units of 2 pixels]*/
	USHORT	AWBWindowTargetSizeRightEdgeX;		/**< horizontal sizes of the actual AWB detection accumulation target in right edge block of one block.			value range:[6~512 pixels and in units of 2 pixels]*/
	USHORT	AWBWindowTargetSizeTopY;			/**< vertical sizes of the actual AWB detection accumulation target in top block of one block.					value range:[2~256 lines and in units of 2 lines]	*/
	USHORT	AWBWindowTargetSizeBottomY;			/**< vertical sizes of the actual AWB detection accumulation target in bottom block of one block.				value range:[2~256 lines and in units of 2 lines]	*/
} AEAWBWinInfo;

// FJ_AESetComputeValueRatio
/** AE compute value ratio	*/
typedef struct {
	SHORT	rRatio;					/**< R  pixel ratio.(step is 1/256)	*/
	SHORT	grRatio;				/**< Gr pixel ratio.(step is 1/256)	*/
	SHORT	gbRatio;				/**< Gb pixel ratio.(step is 1/256)	*/
	SHORT	bRatio;					/**< B  pixel ratio.(step is 1/256)	*/
} AAAComputeRatioInfo;

// @cond
/** Line Info */
typedef struct {
	USHORT		start_line[4];			/**< Line luminosity evaluation vertical address. 2line boundary	*/
} AEAWBLineInfo;
// @endcond

/* @} */	// fj_ae_definition group

/******************************************************************************
 ProtoType
******************************************************************************/
/** @weakgroup fj_ae_api
@{*/

/********************** AE  ******************************/
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set AE/AWB accumulation window. It can be set separately AE and AWB.
@param[in]	aeawb_id	: AEAWB ID. value range is [0..3]<br>
@param[in]	wininfo	: AE/AWB accumulation window information. See @ref AEAWBWinInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Schematic diagram of the AE/AWB size settings are as follows.<br>
			About WIndow mode detail, please refer to FJ_SetAEAWBWDWindowMode() remarks. <br>
			<br>
			- Fig) Window mode 0<br>
			@image html fj_ae_target_window_mode0.png Fig) AEAWB Window mode 0<br>
			- Fig) Window mode 1<br>
			@image html fj_ae_target_window_mode1.png Fig) AEAWB Window mode 1<br>
 *	Sample code is following.<br>
 *	@code
	FJ_ERR_CODE error_code;
	AEAWBWinInfo wininfo;
	UCHAR aeawb_id = 0;

	// Sensor driving mode 4248x2182( target: 4096x2160 )
	//                               4248
	//        +------------------------------------------------+
	//        |                                                |
	//        |   +-------------------------------------------+|
	//        |   |                  4096                      |
	//        |   |                                            |
	//        |   |                                            |
	//  2182  |   |2160                                        |
	//        |   |                                            |
	//        |   |                                            |
	//        |   |                                            |
	//        +---+--------------------------------------------+
	// 
	wininfo.WindowBlockCntX					= 32;
	wininfo.WindowBlockCntY					= 32;
	wininfo.WindowStartX					= 152;	// 4248 - 4096
	wininfo.WindowStartY					= 22;	// 2182 - 2160
	wininfo.WindowSizeX						= 4096 / wininfo.WindowBlockCntX;
	wininfo.WindowSizeY						= 2160 / wininfo.WindowBlockCntY;
	wininfo.WindowSizeLeftEdgeX				= 4096 / wininfo.WindowBlockCntX;
	wininfo.WindowSizeRightEdgeX			= 4096 / wininfo.WindowBlockCntX;
	wininfo.WindowSizeTopY					= 2160 / wininfo.WindowBlockCntY;
	wininfo.WindowSizeBottomY				= 2160 / wininfo.WindowBlockCntY;
            
	wininfo.AEWindowTargetSizeX				= 4096 / wininfo.WindowBlockCntX;
	wininfo.AEWindowTargetSizeY				= 2160 / wininfo.WindowBlockCntY;
	wininfo.AEWindowTargetSizeLeftEdgeX		= 4096 / wininfo.WindowBlockCntX;
	wininfo.AEWindowTargetSizeRightEdgeX	= 4096 / wininfo.WindowBlockCntX;
	wininfo.AEWindowTargetSizeTopY			= 2160 / wininfo.WindowBlockCntY;
	wininfo.AEWindowTargetSizeBottomY		= 2160 / wininfo.WindowBlockCntY;
            
	wininfo.AWBWindowTargetSizeX			= 4096 / wininfo.WindowBlockCntX;
	wininfo.AWBWindowTargetSizeY			= 2160 / wininfo.WindowBlockCntY;
	wininfo.AWBWindowTargetSizeLeftEdgeX	= 4096 / wininfo.WindowBlockCntX;
	wininfo.AWBWindowTargetSizeRightEdgeX	= 4096 / wininfo.WindowBlockCntX;
	wininfo.AWBWindowTargetSizeTopY			= 2160 / wininfo.WindowBlockCntY;
	wininfo.AWBWindowTargetSizeBottomY		= 2160 / wininfo.WindowBlockCntY;
	error_code = FJ_SetAEAWBWindow( 0, &wininfo );
 *	@endcode
*/
extern FJ_ERR_CODE FJ_SetAEAWBWindow( UCHAR aeawb_id, AEAWBWinInfo* wininfo );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AE/AWB accumulation window
@param[in]	aeawb_id	: AEAWB ID. value range is [0..3]<br>
@param[out]	wininfo	: AE/AWB window information. See @ref AEAWBWinInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	About detail, please refer to FJ_SetAEAWBWindow() remarks.<br>
*/
extern FJ_ERR_CODE FJ_GetAEAWBWindow( UCHAR aeawb_id, AEAWBWinInfo* wininfo );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API used to set the coefficients of luminance calculation equation used for AE detection.<br>
Available range of coefficients is as below:<br>
-256 ~ 255(-1~0.99609375): 1step = 1/256 = 0.00390625
@remarks	Use of this API is optional.<br>
			Unless Host uses this API, following default values to be used.<br>
@param[in]	aeawb_id	: AEAWB ID. value range is [0..3]<br>
@param[in]	aaacmp	: See @ref AAAComputeRatioInfo<br>
			<table border=0><tr><td>rRatio = 76</td><td>(76/256 = 0.3)</td></tr>
			<tr><td>grRatio = 74</td><td>(74/256 = 0.29)</td></tr>
			<tr><td>gbRatio = 74</td><td>(74/256 = 0.29)</td></tr>
			<tr><td>bRatio = 28</td><td>(28/256 = 0.11)</td></tr></table>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Fig) Image of Y generation:<br>
			@image html fj_ae_compratio.png Fig) Y generation<br>
*/
extern FJ_ERR_CODE FJ_3ASetComputeValueRatio( UCHAR aeawb_id, AAAComputeRatioInfo* aaacmp );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the coefficients of luminance calculation equation used for AE detection.<br>
Available range of coefficients is as below:<br>
-256 ~ 255(-1~0.99609375): 1step = 1/256 = 0.00390625
@remarks	Use of this API is optional.<br>
			Unless Host uses this API, following default values to be used.<br>
@param[in]	aeawb_id	: AEAWB ID. value range is [0..3]<br>
@param[out]	aaacmp	: See @ref AAAComputeRatioInfo<br>
			<table border=0><tr><td>rRatio = 76</td><td>(76/256 = 0.3)</td></tr>
			<tr><td>grRatio = 74</td><td>(74/256 = 0.29)</td></tr>
			<tr><td>gbRatio = 74</td><td>(74/256 = 0.29)</td></tr>
			<tr><td>bRatio = 28</td><td>(28/256 = 0.11)</td></tr></table>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_3AGetComputeValueRatio( UCHAR aeawb_id, AAAComputeRatioInfo* aaacmp );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set AE/AWB Window mode. 
It is API for Selecting the mode of AE/AWB Window.
@param[in]	aeawb_id		: AEAWB ID. value range is [0..3]<br>
@param[in] wb_win_mode	: AE/AWB Window mode.<br>
						  0:The detection block horizontal/vertical sizes and the in-block enable horizontal/ vertical sizes are all in the same size.<br>
						  1:The individual setting of detection block at top/bottom/left/right side is valid.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	<br>
			- Fig) Image of AE/AWB Window mode 0:<br>
			@image html fj_ae_window_mode0.png Fig) AE/AWB Window mode0<br>
			- Fig) Image of AE/AWB Window mode 1:<br>
			@image html fj_ae_window_mode1.png Fig) AE/AWB Window mode1<br>
*/
extern FJ_ERR_CODE FJ_SetAEAWBWDWindowMode( UCHAR aeawb_id, UCHAR wb_win_mode );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AE/AWB Window mode. 
It is API for Selecting the mode of AE/AWB Window.
@param[in]	aeawb_id		: AEAWB ID. value range is [0..3]<br>
@param[out] wb_win_mode	: AE/AWB Window mode.<br>
						  0:The detection block horizontal/vertical sizes and the in-block enable horizontal/ vertical sizes are all in the same size.<br>
						  1:The individual setting of detection block at top/bottom/left/right side is valid.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	About detail, please refer to FJ_SetAEAWBWDWindowMode() remarks.<br>
*/
extern FJ_ERR_CODE FJ_GetAEAWBWDWindowMode( UCHAR aeawb_id, UCHAR* wb_win_mode );

// --- REMOVE_CAMERAIF BEGIN (simple tool) ---
extern FJ_ERR_CODE FJ_AE_Loading_Complete( E_BF_AE_BIN_IDX type, BYTE* addr, ULONG bytes );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get ae table sdram common.
 *
 *	@param[in]	ae_param_tbl		: Address of ae parameter table.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 */
extern FJ_ERR_CODE FJ_AE_Tbl_Get_Sdram_Common( VOID* ae_param_tbl );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Set ae tbl force reload.
 *
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 */
extern FJ_ERR_CODE FJ_AE_Tbl_Set_Force_Reload( VOID );

// --- REMOVE_CAMERAIF END (simple tool) ---
/* @} */	// fj_ae_api group
#endif
