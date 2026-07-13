/**
 * @file		fj_calibration.h
 * @brief		The Calibration API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef __FJ_CALIBRATION_H__
#define __FJ_CALIBRATION_H__

#include "fj_std.h"

#include "ddim_typedef.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_calibration_overview
@{
The content of this chapter is described calibration mode sequence and related APIs.
@}
*/


///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_calibration_sequence
@{
- @ref fj_calibration_badpix_detect_sequence
	- @ref fj_calibration_badpix_sw_capture_sequence_section
- @ref fj_calibration_badpix_compensation_sequence
	- @ref fj_calibration_badpix_compensation_sequence_section
- @ref fj_calibration_shading_calibration_sequence
	- @ref fj_calibration_shading_calibration_sequence_section_fshd
- @ref fj_calibration_shading_compensation_sequence
	- @ref fj_calibration_shading_compensation_sequence_section_fshd
<hr>

@section fj_calibration_badpix_detect_sequence	Badpix Detect Sequence
@subsection fj_calibration_badpix_sw_capture_sequence_section	SW Detect sequence
@image html fj_calib_dpc_detect_cap_sw.png

<hr>

@section fj_calibration_badpix_compensation_sequence	Badpix Compensation Sequence
@subsection fj_calibration_badpix_compensation_sequence_section	HW Compensation sequence
@image html fj_calib_dpc_correct.png
<hr>

@section fj_calibration_shading_calibration_sequence	Shading Calibration Sequence
@subsection fj_calibration_shading_calibration_sequence_section_fshd	Flame Shading Calibration sequence
@image html fj_calib_fshd_calibration.png
<hr>

@section fj_calibration_shading_compensation_sequence	Shading Compensation Sequence
@subsection fj_calibration_shading_compensation_sequence_section_fshd	Flame Shading Conmensation sequence
@image html fj_calib_fshd_correct.png
<hr>

@} */



/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
/** @weakgroup fj_calibration_definition
@{*/
// nothing special.

/* @} */	// fj_calibration_definition group

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
/** @weakgroup fj_calibration_definition
@{*/

//@cond
/**	Calibration bayer format
 */
typedef enum {
	E_IQ_CALIB_FMT_PIX_ERR = -1,	/**< API is not prepared. */
	E_IQ_CALIB_FMT_PIX_12PACK = 0,	/**< 12bit pack */
	E_IQ_CALIB_FMT_PIX_16NONPACK,	/**< 16bit non-pack */
	E_IQ_CALIB_FMT_PIX_8BIT,		/**< 8bit */
	E_IQ_CALIB_FMT_PIX_MAX
} E_IQ_CALIB_FMT_PIX;
// @endcond

/**	Calibration operate mode
 */
typedef enum {
	E_IQ_CALIB_OP_12M_CAPTURE = 1,	/**< 12M Capture */
	E_IQ_CALIB_OP_8M_CAPTURE,		/**< 8M/5M/2M Capture */
	E_IQ_CALIB_OP_4K_VIDEO,			/**< 4K Video */
	E_IQ_CALIB_OP_FHD_VIDEO,		/**< FHD Video */
	E_IQ_CALIB_OP_HD_VIDEO,			/**< HD Video */
	E_IQ_CALIB_OP_MAX
} E_IQ_CALIB_OP;

/**	Calibration target mode
 */
typedef enum {
	E_IQ_CALIB_TGT_ERR = -1,	/**< API is not prepared. */
	E_IQ_CALIB_TGT_INIT = 0,
	E_IQ_CALIB_TGT_0,			/**< Target 0 */
	E_IQ_CALIB_TGT_1,			/**< Target 1 */
	E_IQ_CALIB_TGT_2,			/**< Target 2 */
	E_IQ_CALIB_TGT_3,			/**< Target 3 */
	E_IQ_CALIB_TGT_4,			/**< Target 4 */
	E_IQ_CALIB_TGT_MAX
} E_IQ_CALIB_TGT;

/* @} */	// fj_calibration_definition group


/******************************************************************************
 Function
******************************************************************************/
// @cond
/** @weakgroup fj_calibration_api
@{*/

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function sets the format type of the bayer.\n
 *				\n
 *				supported calibration target.\n
 *				- E_IQ_CALIB_FMT_PIX_12PACK\n
 *				  - E_IQ_CALIB_TGT_1\n
 *				- E_IQ_CALIB_FMT_PIX_16NONPACK\n
 *				  - E_IQ_CALIB_TGT_0\n
 *				  - E_IQ_CALIB_TGT_1\n
 *				  - E_IQ_CALIB_TGT_2
 *
 *	@param[in]	e_format_pix : Format pixel. See @ref E_IQ_CALIB_FMT_PIX
 *	@remarks	The default value(=E_IQ_CALIB_FMT_PIX_16NONPACK) is set to the global valiable until this API is invoked.
 */
extern VOID FJ_IQ_CalibrationSetFormatPix( E_IQ_CALIB_FMT_PIX e_format_pix );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function returns the format type of the bayer.
 *	@remarks	The default value(=E_IQ_CALIB_FMT_PIX_16NONPACK) is set to the global valiable until this API is invoked.
 */
extern E_IQ_CALIB_FMT_PIX FJ_IQ_CalibrationGetFormatPix( VOID );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function sets the status of Raw save to the media.
 *	@param[in]	save : bayer save enable/disable. See BOOL
 *	@remarks	The default value(= FALSE) is set to the global valiable until this API is invoked.
 */
extern VOID FJ_IQ_CalibrationSetRawSaveToMedia( BOOL save );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function returns the status of Raw save to the media.
 *	@return		Raw data save status
 */
extern BOOL FJ_IQ_CalibrationGetRawSaveToMedia( VOID );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function sets the status of JPEG save to the media.\n
 *				\n
 *				supported calibration target:\n
 *				- E_IQ_CALIB_TGT_CAPTURE\n
 *
 *	@param[in]	save : jpeg save enable/disable. See BOOL
 *	@remarks	The default value(= FALSE) is set to the global valiable until this API is invoked.
 */
extern VOID FJ_IQ_CalibrationSetJpegSaveToMedia( BOOL save );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function returns the status of JPEG save to the media.
 *	@return		JPEG data save status
 */
extern BOOL FJ_IQ_CalibrationGetJpegSaveToMedia( VOID );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function sets the calibration target.
 *	@param[in]	target : calibration target(view/jpeg/capture). See @ref E_IQ_CALIB_TGT
 *	@remarks	The default value(= E_IQ_CALIB_TGT_INIT) is set to the global valiable until this API is invoked.
 */
extern VOID FJ_IQ_CalibrationSetTarget( E_IQ_CALIB_TGT target );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function returns the calibration target.
 *	@return		Calibration target
 */
extern VOID FJ_IQ_CalibrationGetTarget( E_IQ_CALIB_TGT* target );


/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function sets the calibration operate.
 *	@param[in]	operate : calibration operate. See @ref E_IQ_CALIB_OP
 */
extern VOID FJ_IQ_CalibrationSetOperate( E_IQ_CALIB_OP operate );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function returns the calibration operate.
 *	@return		Calibration operate
 */
extern E_IQ_CALIB_OP FJ_IQ_CalibrationGetOperate( VOID );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function set bayer for Calibration.
 *	@return		None
 */
extern VOID FJ_IQ_CalibrationSetBayer( const VOID* const bay_addr, const UINT32 bay_bytes, const UINT32 width, const UINT32 lines );

/* @} */	// fj_calibration_api group
// @endcond

#endif

