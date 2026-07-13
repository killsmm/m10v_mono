/**
 * @file		fj_awb.h
 * @brief		Auto White Balance Setting API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_AWB_H
#define _FJ_AWB_H

#include "fj_ae.h"
#include "fj_af.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_awb_overview
@{
	AWB module generates evaluation value of RAW(Detection) used to Auto white balance correction.<br>
	This module's structure is as following.
	@image html fj_awb_overview.png
	Each API's detailed, please refer to @ref fj_awb_api API section.<br>
	To get more detailed information of JDSPRO/F_B2Y, please refer to Hardware macro specifications.
	- Supported Function<br>
	. A setup of AWB detection.<br>
	. A White Barance Gain setup for Video/Still.<br>
@}*//* --- end of fj_awb_overview */


/******************************************************************************
 Fixed Define
******************************************************************************/
/* Nothing Special */

/******************************************************************************
 Enumeration
******************************************************************************/
/** @weakgroup fj_awb_definition
@{*/
/** PGAIN decimal point position
*/
typedef enum {
	E_FJ_AWB_PGAIN_DECI_POS_8 = 0,			/**<  5-bit integer part,8-bit fractional part	*/
	E_FJ_AWB_PGAIN_DECI_POS_7,				/**<  6-bit integer part,7-bit fractional part	*/
	E_FJ_AWB_PGAIN_DECI_POS_6,				/**<  7-bit integer part,6-bit fractional part	*/
	E_FJ_AWB_PGAIN_DECI_POS_5,				/**<  8-bit integer part,5-bit fractional part	*/
	E_FJ_AWB_PGAIN_DECI_POS_4,				/**<  9-bit integer part,4-bit fractional part	*/
	E_FJ_AWB_PGAIN_DECI_POS_3,				/**< 10-bit integer part,3-bit fractional part	*/
} E_FJ_AWB_PGAIN_DECI_POS;

/* @} */	// fj_awb_definition group

/******************************************************************************
 Structure
******************************************************************************/
/** @weakgroup fj_awb_definition
@{*/

// FJ_AWBSetVideoGain
/** AWB white balance information */
typedef struct {
	USHORT R_Gain;					/**< White barance gain R  pixel	value range :[ 0(0.0) ~ 2047(7.99609375) 1step = 1/256 ]	*/
	USHORT Gr_Gain;					/**< White barance gain Gr pixel	value range :[ 0(0.0) ~ 2047(7.99609375) 1step = 1/256 ]	*/
	USHORT Gb_Gain;					/**< White barance gain Gb pixel	value range :[ 0(0.0) ~ 2047(7.99609375) 1step = 1/256 ]	*/
	USHORT B_Gain;					/**< White barance gain B  pixel	value range :[ 0(0.0) ~ 2047(7.99609375) 1step = 1/256 ]	*/
} WB_Info;

// FJ_AWBSetLevelGate/FJ_SetFLCKLevelGate
/** AWB/FLCK level gate value */
typedef struct {
	UCHAR	Gate_Y;					/**< Y data gate value	value range:[ 0~255 ]	*/
	UCHAR	Gate_R;					/**< R data gate value	value range:[ 0~255 ]	*/
	UCHAR	Gate_G;					/**< G data gate value	value range:[ 0~255 ]	*/
	UCHAR	Gate_B;					/**< B data gate value	value range:[ 0~255 ]	*/
} Gatepix;

// FJ_AWBSetLevelGate
/** AWB level gate value */
typedef struct {
	Gatepix	AWBGateUpper;			/**< Level Gate Upper	*/
	Gatepix	AWBGateLower;			/**< Level Gate Lower	*/
} AWBLevelGate;

// FJ_AWBSetMtxLevelGate/FJ_SetFLCKMatrixLevelGate
/** AWB/FLCK Matrix Y/Cb/Cr Value */
typedef struct {
	UCHAR	Gate_Y;					/**< Matrix Y  data gate value */
	UCHAR	Gate_Cb;				/**< Matrix Cb data gate value */
	UCHAR	Gate_Cr;				/**< Matrix Cr data gate value */
} MtxGatepix;

// FJ_AWBSetMtxLevelGate
/** AWB Matrix level gate value */
typedef struct {
	MtxGatepix	AWBMtxGateUpper;			/**< Matrix Level Gate Upper	*/
	MtxGatepix	AWBMtxGateLower;			/**< Matrix Level Gate Lower	*/
	SHORT		AWBMtxValue[3][3];			/**< Matrix Value				*/
} AWBMtxLevelGate;

// FJ_AWBGetPGAIN
/** Bayer pixel adjust value */
typedef struct {
	USHORT					gain_r;					/**< Bayer gain value R  pix. value range : [ 0x0000 ~ 0x1FFF ]									*/
	USHORT					gain_gr;				/**< Bayer gain value GR pix. value range : [ 0x0000 ~ 0x1FFF ]									*/
	USHORT					gain_gb;				/**< Bayer gain value GB pix. value range : [ 0x0000 ~ 0x1FFF ]									*/
	USHORT					gain_b;					/**< Bayer gain value B  pix. value range : [ 0x0000 ~ 0x1FFF ]									*/
	SHORT					offset_r;				/**< Bayer offset value R  pix. value range : [ 0x1FFF(= -4095) ~ 0x0FFF(=+4095) 1step = 1 ]	*/
	SHORT					offset_gr;				/**< Bayer offset value GR pix. value range : [ 0x1FFF(= -4095) ~ 0x0FFF(=+4095) 1step = 1 ]	*/
	SHORT					offset_gb;				/**< Bayer offset value GB pix. value range : [ 0x1FFF(= -4095) ~ 0x0FFF(=+4095) 1step = 1 ]	*/
	SHORT					offset_b;				/**< Bayer offset value B  pix. value range : [ 0x1FFF(= -4095) ~ 0x0FFF(=+4095) 1step = 1 ]	*/
} BayAdjustInfo;

/** WB saturation correction value. (slope gain) */
typedef struct {
	BOOL	enable;			/**< WBHCEN register value. */
	USHORT	r;				/**< WBSLPRR register value. */
	USHORT	gr;				/**< WBSLPGR register value. */
	USHORT	gb;				/**< WBSLPGB register value. */
	USHORT	b;				/**< WBSLPBB register value. */
} T_FJ_AWB_SLP_GAIN;

/* @} */	// fj_awb_definition group

/******************************************************************************
 ProtoType
******************************************************************************/
/** @weakgroup fj_awb_api
@{*/

/********************** AWB ******************************/
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set White Balance Gain & CC matrix for Video mode. <br>
It assumes that an adjustment value is computed by AWB control of User.
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	wb			: White balance gain. See @ref WB_Info<br>
@param[in]	cc_matrix	: CC matrix.<br>
@param[in]	set_timming	: param set timming[0:ASYNC 1:SYNC].<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	White balance gain parameters are reflectd in the B2Y block.( Please refer to @ref fj_awb_overview overview )<br>
			<br>
			@image html fj_awb_ccmatrix.png Fig) CC Matrix
			
@code
			WB_Info wb;
			SHORT	cc[3][3];
			UCHAR demosaic_id = 0;
			
			wb.R_Gain  = 512;	//(= 2.0) gain/256
			wb.Gr_Gain = 256;	//(= 1.0)
			wb.Gb_Gain = 256;	//(= 1.0)
			wb.B_Gain  = 764;	//(= 1.49)
			
			cc[0][0] = 512;	//(=1.0) cc/512
			cc[0][1] = 0;
			cc[0][2] = 0;

			cc[0][0] = 0;
			cc[0][1] = 512;
			cc[0][2] = 0;

			cc[0][0] = 0;
			cc[0][1] = 0;
			cc[0][2] = 512;

			FJ_AWBSetVideoGain( demosaic_id, &wb, cc, 0 );
@endcode
*/
extern FJ_ERR_CODE FJ_AWBSetVideoGain( UCHAR demosaic_id, WB_Info* wb, SHORT* cc_matrix, UCHAR set_timming );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get White Balance Gain & CC matrix in Video mode.
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[out]	wb			: White balance gain. See @ref WB_Info<br>
@param[out]	cc_matrix	: CC matrix.<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@code
			WB_Info wb;
			SHORT	cc[3][3];
			UCHAR demosaic_id = 0;
			
			FJ_AWBGetVideoGain( demosaic_id, &wb, cc );
@endcode
*/
extern FJ_ERR_CODE FJ_AWBGetVideoGain( UCHAR demosaic_id, WB_Info* wb, SHORT* cc_matrix );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set White Balance Gain & CC matrix for Still mode. <br>
It assumes that an adjustment value is computed by AWB control of User.
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	wb			: White balance gain. See @ref WB_Info<br>
@param[in]	cc_matrix	: CC matrix.<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	White balance gain parameters are reflectd in the B2Y block.( Please refer to @ref fj_awb_overview overview )<br>
			<br>
			@image html fj_awb_ccmatrix.png Fig) CC Matrix
@code
			WB_Info wb;
			SHORT	cc[3][3];
			UCHAR demosaic_id = 0;
			
			wb.R_Gain  = 512;	//(= 2.0) gain/256
			wb.Gr_Gain = 256;	//(= 1.0)
			wb.Gb_Gain = 256;	//(= 1.0)
			wb.B_Gain  = 764;	//(= 1.49)
			
			cc[0][0] = 512;	//(=1.0) cc/512
			cc[0][1] = 0;
			cc[0][2] = 0;

			cc[0][0] = 0;
			cc[0][1] = 512;
			cc[0][2] = 0;

			cc[0][0] = 0;
			cc[0][1] = 0;
			cc[0][2] = 512;

			FJ_AWBSetStillGain( demosaic_id, &wb, cc );
@endcode
*/
extern FJ_ERR_CODE FJ_AWBSetStillGain( UCHAR demosaic_id, WB_Info* wb, SHORT* cc_matrix );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get White Balance Gain & CC matrix in Still mode. 
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[out]	wb			: White balance gain. See @ref WB_Info<br>
@param[out]	cc_matrix	: CC matrix.<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AWBGetStillGain( UCHAR demosaic_id, WB_Info* wb, SHORT* cc_matrix );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the levels to clip data after White Balance Gain operation for each color(R/Gb/Gr/B) in Video mode. 
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	wb	: White balance clip level.<br>
	USHORT xx_Gain...0~4095
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AWBSetVideoClipLevel( UCHAR demosaic_id, WB_Info* wb );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the levels to clip data after White Balance Gain operation in Video mode.
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[out]	wb			: White balance clip level.<br>
							USHORT xx_Gain...0~4095
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AWBGetVideoClipLevel( UCHAR demosaic_id, WB_Info* wb );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the levels to clip data after White Balance Gain operation for each color(R/Gb/Gr/B) in Still mode.
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	wb			: White balance clip level.<br>
							USHORT xx_Gain...0~4095
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AWBSetStillClipLevel( UCHAR demosaic_id, WB_Info* wb );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the levels to clip data after White Balance Gain operation in Still mode.
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[out]	wb	: White balance clip level.<br>
	USHORT xx_Gain...0~4095
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AWBGetStillClipLevel( UCHAR demosaic_id, WB_Info* wb );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API used to set RGB level gate threshold value in the process unit before AWB detection.<br>
The size of the threshold value is 8 bits, and this threshold value is compared with the valid upper 8 bits of the input data. 
@param[in]	sro_id	: SRO instance ID.<br>
@param[in]	gate	: AWB level gate value. See @ref AWBLevelGate<br>
					  This LevelGate value is compared with the valid upper 8 bits of the RAW pixels value.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	@image html fj_awb_rgb_levelgate.png Fig) RGB Level Gate
*/
extern FJ_ERR_CODE FJ_AWBSetLevelGate( UCHAR sro_id, AWBLevelGate* gate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AWB level gate threshold value.
@param[in]	sro_id	: SRO instance ID.<br>
@param[out]	gate	: AWB level gate value. See @ref AWBLevelGate<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AWBGetLevelGate( UCHAR sro_id, AWBLevelGate* gate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API used to set AWB matrix level gate threshold value in the process unit before AWB detection.<br>
The size of the threshold value is 8 bits, and this threshold value is compared with the matrix calculation result 8 bits.
@param[in]	sro_id		: SRO instance ID.<br>
@param[in]	mtx_gate	: AWB Matrix level gate value. See @ref AWBMtxLevelGate<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	@image html fj_awb_matrix_levelgate.png Fig) Matrix Level Gate
*/
extern FJ_ERR_CODE FJ_AWBSetMatrixLevelGate( UCHAR sro_id, AWBMtxLevelGate* mtx_gate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AWB matrix level gate threshold value.
@param[in]	sro_id		: SRO instance ID.<br>
@param[out]	mtx_gate	: AWB Matrix level gate value. See @ref AWBMtxLevelGate<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AWBGetMatrixLevelGate( UCHAR sro_id, AWBMtxLevelGate* mtx_gate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Bayer pixel color adjustment API .
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	block_num	: PGAIN block number [ 0 ~ 2 ] 
@param[in]	bay_adjust	: Bayer pixel adjust value. See @ref BayAdjustInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Bayer pixel adjust parameters are reflectd in the PRO block.( Please refer to @ref fj_awb_overview overview )<br>
*/
extern FJ_ERR_CODE FJ_AWBSetPGAIN( UCHAR demosaic_id, UCHAR block_num, BayAdjustInfo* bay_adjust );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get Bayer pixel color adjustment value .
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	block_num	: PGAIN block number [ 0 ~ 2 ] 
@param[out]	bay_adjust	: Bayer pixel adjust value. See @ref BayAdjustInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
extern FJ_ERR_CODE FJ_AWBGetPGAIN( UCHAR demosaic_id, UCHAR block_num, BayAdjustInfo* bay_adjust );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set PGAIN decimal point position.
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	block_num	: PGAIN block number [ 0 ~ 2 ] 
@param[in]	decimal_point_position : Decimal point position of bayer gain value. See @ref E_FJ_AWB_PGAIN_DECI_POS.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Bayer pixel adjust parameters are reflectd in the PRO block.( Please refer to @ref fj_awb_overview overview )<br>
*/
extern FJ_ERR_CODE FJ_AWBSetPGAIN_Decimal_Pos( UCHAR demosaic_id, UCHAR block_num, E_FJ_AWB_PGAIN_DECI_POS decimal_point_position );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get PGAIN decimal point position.
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	block_num	: PGAIN block number [ 0 ~ 2 ] 
@param[out]	decimal_point_position : Pointer of decimal point position of bayer gain value. See @ref E_FJ_AWB_PGAIN_DECI_POS.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	Bayer pixel adjust parameters are reflectd in the PRO block.( Please refer to @ref fj_awb_overview overview )<br>
*/
extern FJ_ERR_CODE FJ_AWBGetPGAIN_Decimal_Pos( UCHAR demosaic_id, UCHAR block_num, E_FJ_AWB_PGAIN_DECI_POS* decimal_point_position );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set AWB Data format type. 
It is API for Selecting the format of AWB output data.
@param[in]	sro_id		: SRO instance ID.<br>
@param[in] format_type	: AWB output data format type [ 0:R/G/B/AWBCnt 1:R/Gr/Gb/B ]
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	@image html fj_awb_dataformat.png Fig) AWB Data format type
*/
extern FJ_ERR_CODE FJ_AWBSetDataformat( UCHAR sro_id, UCHAR format_type );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get AWB Data format type. 
It is API for Selecting the format of AWB output data.
@param[in]	sro_id		: SRO instance ID.<br>
@param[out] format_type	: AWB output data format type [ 0:R/G/B/AWBCnt 1:R/Gr/Gb/B ]
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
@remarks	@image html fj_awb_dataformat.png Fig) AWB Data format type
*/
extern FJ_ERR_CODE FJ_AWBGetDataformat( UCHAR sro_id, UCHAR* format_type );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the slope gain after White Balance Gain operation for each color(R/Gr/Gb/B).
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	slp_gain	: Satulation correction value.(slope gain)<br>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
*/
extern FJ_ERR_CODE FJ_AWBSetSlopeGain( UCHAR demosaic_id, const T_FJ_AWB_SLP_GAIN* slp_gain );

// --- REMOVE_CAMERAIF BEGIN (simple tool) ---
extern FJ_ERR_CODE FJ_AWB_Loading_Complete( E_BF_AWB_BIN_IDX type, BYTE* addr, ULONG bytes );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get awb table sdram common.
 *
 *	@param[in]	awb_param_tbl		: Address of awb parameter table.
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 */
extern FJ_ERR_CODE FJ_AWB_Tbl_Get_Sdram_Common( VOID* awb_param_tbl );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Set awb tbl force reload.
 *
 *	@retval		FJ_ERR_OK : OK
 *	@retval		FJ_ERR_NG : Error
 */
extern FJ_ERR_CODE FJ_AWB_Tbl_Set_Force_Reload( VOID );
// --- REMOVE_CAMERAIF END (simple tool) ---

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set Calib Pgain from Calib.BIN.
@param[in]	demosaic_id	: Demosaic instance ID.
@param[in]	bay_adjust	: Bayer pixel adjust value. See @ref BayAdjustInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
*/
extern FJ_ERR_CODE FJ_AWBSetCalibPgain( UCHAR demosaic_id, BayAdjustInfo* bay_adjust );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get Calib Pgain from Calib.BIN.
@param[in]	demosaic_id	: Demosaic instance ID.
@param[out]	bay_adjust	: Bayer pixel adjust value. See @ref BayAdjustInfo
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
*/
extern FJ_ERR_CODE FJ_AWBGetCalibPgain( UCHAR demosaic_id, BayAdjustInfo* bay_adjust );

/* @} */	// fj_awb_api group
#endif
