/**
 * @file		fj_param.h
 * @brief		parameter header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_PARAM_H
#define _FJ_PARAM_H

#include "ddim_typedef.h"
#include "fj_std.h"
///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
//@cond
/** @weakgroup fj_param_overview
@{
	There are SocioNext's paramer setting.<br>
@}*//* --- end of fj_param_overview */
//@endcond

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
//@cond
/** @weakgroup fj_param_definition
@{*/
/*----------------------------------------------------------------------------
		Parameter ID
----------------------------------------------------------------------------*/
//@cond
/* --- System --- */
/* Interrupt */
#define FJ_PARA_HOST_MSG_BLOCK							0x1001	/**< Blocking the HOST Message (HM) */
#define FJ_PARA_HOST_MSG_UNBLOCK						0x1002	/**< Unblocking the HOST Message */
/* Memory Manupulate */
#define FJ_PARA_TL_TOO_MANY_BAD_BLOCKS_CALLBACK_FUNC	0x1003	/**< NAND TL too many bad blocks callback function (Not supprt) */
#define FJ_PARA_TL_SYSTEM_DATA_ERROR_CALLBACK_FUNC		0x1004	/**< NAND TL system data error callback function (Not supprt) */
#define FJ_PARA_TL_ERROR_STATUS							0x1005	/**< NAND TL error status (Not supprt) */
/* General */
#define FJ_PARA_MEM_FILE_NAME							0x1006	/**< Memory file name (Not supprt) */

/* --- Preview, Capture --- */
/* Still Capture */
#define FJ_PARA_JPEG_CUT_THRESHOLD						0x3001	/**< % (Default:30%) */

#define FJ_PARA_VIEW_YUV_1_SET_OUTPUT_SIZE				0x3020	/**< Image size of FJ_HostVideoYUVManipulation(Pre-CNR) */
#define FJ_PARA_VIEW_YUV_2_SET_OUTPUT_SIZE				0x3021	/**< Image size of FJ_HostVideoYUVManipulation(Post-CNR) */
#define FJ_PARA_VIEW_YUV_1_SET_OUTPUT_AREA				0x3022	/**< Output Area of FJ_HostVideoYUVManipulation(Pre-CNR) */
#define FJ_PARA_VIEW_YUV_2_SET_OUTPUT_AREA				0x3023	/**< Output Area of FJ_HostVideoYUVManipulation(Post-CNR) */
#define FJ_PARA_CAPTURE_YUV_1_SET_OUTPUT_SIZE			0x3024	/**< Image size of FJ_HostStillYUVManipulation(Pre-CNR) */
#define FJ_PARA_CAPTURE_YUV_2_SET_OUTPUT_SIZE			0x3025	/**< Image size of FJ_HostStillYUVManipulation(Post-CNR) */
#define FJ_PARA_CAPTURE_YUV_3_SET_OUTPUT_SIZE			0x3026	/**< Image size of FJ_HostStillYUVManipulation(Post-WARP) */
#define FJ_PARA_CAPTURE_YUV_4_SET_OUTPUT_SIZE			0x3027	/**< Image size of FJ_HostStillYUVManipulation(Post-AFN) */
#define FJ_PARA_CAPTURE_YUV_1_SET_OUTPUT_AREA			0x3028	/**< Output Area of FJ_HostStillYUVManipulation(Pre-CNR) */
#define FJ_PARA_CAPTURE_YUV_2_SET_OUTPUT_AREA			0x3029	/**< Output Area of FJ_HostStillYUVManipulation(Post-CNR) */
#define FJ_PARA_CAPTURE_YUV_3_SET_OUTPUT_AREA			0x302A	/**< Output Area of FJ_HostStillYUVManipulation(Post-WARP) */
#define FJ_PARA_CAPTURE_YUV_4_SET_OUTPUT_AREA			0x302B	/**< Output Area of FJ_HostStillYUVManipulation(Post-AFN) */

/* --- ChipTest --- */
#define FJ_PARA_CHIPTEST_ADDR							0x8001	/**< ChipTest reserved address */
//@endcond
/* --- Movie --- */
#define FJ_PARA_MOVIE_REC_SYNC							0x9001	/**< The return of FJ_CaptureClipCmd() shows synchronization or the asynchronization. */
#define FJ_PARA_MOVIE_PLAY_SYNC							0x9002	/**< The return of FJ_PlaybackClipCmd() shows synchronization or the asynchronization. */
//@cond
#define FJ_PARA_MOVIE_SCR_CLASS1_HSIZE					0x9003	/**< Width of Class 1(MPF) for Single Trimming of Movie Play. */
#define FJ_PARA_MOVIE_SCR_CLASS1_VSIZE					0x9004	/**< Lines of Class 1(MPF) for Single Trimming of Movie Play. */
#define FJ_PARA_MOVIE_SCR_CLASS2_HSIZE					0x9005	/**< Width of Class 2(MPF) for Single Trimming of Movie Play. */
#define FJ_PARA_MOVIE_SCR_CLASS2_VSIZE					0x9006	/**< Lines of Class 2(MPF) for Single Trimming of Movie Play. */

/* --- File System --- */
/* MEDIA */
#define FJ_PARA_ASYNC_MOUNT								0x2001	/**< 1:Async, 0:Sync */
#define FJ_PARA_SET_VOLUME_LABEL						0x2002	/**< 0:Disable, 1:Enable */
#define FJ_PARA_FCU_OPERATION_TIME_OUT					0x2003	/**< Media Mount T.O. time(msec) */
#define FJ_PARA_EXPECTED_DPOF_FILE_SIZE					0x2004	/**< DPOF file size */
#define FJ_PARA_EXTERNAL_MEDIA							0x2005	/**< External Media (0:MS, 1:SD) */
/* Exif */
#define FJ_PARA_MAKER_NOTE_SIZE							0x2011	/**< maker note size */
#define FJ_PARA_EXIF_TAG_AUTO							0x2012	/**< 1:Yes, 0:No */
#define FJ_PARA_MAX_CUSTOM_MARKER_SIZE					0x2013	/**< custom marker note size */
#define FJ_PARA_APPN_SIZE								0x2014	/**< fill size */
#define FJ_PARA_SCR_CLASS1_HSIZE						0x2015	/**< screennail class1 width (Default:640) */
#define FJ_PARA_SCR_CLASS1_VSIZE						0x2016	/**< screennail class1 height (Default:480) */
#define FJ_PARA_SCR_CLASS2_HSIZE						0x2017	/**< screennail class2 width (Default:1440) */
#define FJ_PARA_SCR_CLASS2_VSIZE						0x2018	/**< screennail class2 height (Default:1080) */
/* DCF */
#define FJ_PARA_DCF_LAST_KEY							0x2021	/**< latest number */
#define	FJ_PARA_DCF_DUPLICATE							0x2022	/**< duplicate number */
#define	FJ_PARA_DCF_HIDDEN								0x2023	/**< hidden number */
#define	FJ_PARA_DCF_ASSIGN_LAST_KEY						0x2024	/**< latest number assign */
#define	FJ_PARA_DCF_ASSIGN_DUPLICATE					0x2025	/**< duplicate number assign */
#define	FJ_PARA_DCF_ASSIGN_HIDDEN						0x2026	/**< hidden number assign */
#define FJ_PARA_DCF_DUPLICATE_OPERATION					0x2027	/**< 0:The file (And, directory) with the same number is registered in DCF-DB.<br>																	 1:The file (And, directory) with the same number is not registered in DCF-DB. */
#define FJ_PARA_DCF_EDIT_FILE_NOT_ADD_DATE_DB_			0x2028	/**< Edit file is not add to date DB. (0:Add to DB, 1:Not Add to DB)*/
/* --- PlayBack --- */
/* Thumbnail draw */
#define FJ_PARA_THUMB_DRAW_WIDTH_LCD_TV					0x4001	/**< ThumbnailIndex Draw Width for LCD & TV */
#define FJ_PARA_THUMB_DRAW_LINES_LCD_TV					0x4002	/**< ThumbnailIndex Draw Lines for LCD & TV */
#define FJ_PARA_THUMB_DRAW_WIDTH_HD						0x4003	/**< ThumbnailIndex Draw Width for HDMI */
#define FJ_PARA_THUMB_DRAW_LINES_HD						0x4004	/**< ThumbnailIndex Draw Lines for HDMI */
//@endcond
/*@} --- end of fj_param_definition */
//@endcond

//@cond
/** @weakgroup fj_param_api
@{*/
///////////////////////////////////////////////////////////////////////////////
// APIs
///////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
		Declare of API Prototype 
----------------------------------------------------------------------------*/
/* ------------- */
/* Common        */
/* ------------- */
/**
//@cond
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Get parameter value according to parameter ID.<br>
@param [in]  wParamID	paramer ID<br>
@param [out] pwValue	pointer of getted parameter<br>

@retval FJ_ERR_OK	normal end
@retval other : see @ref FJ_ERR_CODE
@remarks
<table>
    <tr>
      <th>wParamID</th>
      <th>Meaning of wParamID</th>
      <th>Meaning of wValue</th>
    </tr>
    <tr>
      <td>@ref FJ_PARA_DCF_DUPLICATE_OPERATION</td>
      <td>DCF-DB same file number is registerd</td>
      <td>0:same number register, 1:same number not register</td>
    </tr>
</table>
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
extern FJ_ERR_CODE FJ_GetParam( USHORT wParamID, USHORT* pwValue );
//@endcond

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Select parameter from host
@param[in] wParamID paramer ID.<br>
@param[in] wValue paramer value.<br>
@retval FJ_ERR_OK	normal end.
@retval other : see @ref FJ_ERR_CODE .
@remarks
<table>
    <tr>
      <th>wParamID</th>
      <th>Meaning of wParamID</th>
      <th>Meaning of wValue</th>
    </tr>
//@cond
   <tr>
      <td>@ref FJ_PARA_HOST_MSG_BLOCK</td>
      <td>Not used</td>
      <td>Not used</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_HOST_MSG_UNBLOCK</td>
      <td>Not used</td>
      <td>Not used</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_TL_TOO_MANY_BAD_BLOCKS_CALLBACK_FUNC</td>
      <td>Not used</td>
      <td>Not used</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_TL_SYSTEM_DATA_ERROR_CALLBACK_FUNC</td>
      <td>Not used</td>
      <td>Not used</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_TL_ERROR_STATUS</td>
      <td>Not used</td>
      <td>Not used</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_MEM_FILE_NAME</td>
      <td>Not used</td>
      <td>Not used</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_ASYNC_MOUNT</td>
      <td>Operation of Mount Media</td>
      <td>1 : Async, 0 : Sync</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_SET_VOLUME_LABEL</td>
      <td>Set volume label of "I" drive in after format</td>
      <td>0:Disable, 1:Enable</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_FCU_OPERATION_TIME_OUT</td>
      <td>Not used</td>
      <td>Not used</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_EXPECTED_DPOF_FILE_SIZE</td>
      <td>DPOF file size</td>
      <td>Default : 500 (Kbyte)</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_EXTERNAL_MEDIA</td>
      <td>Use external media</td>
      <td>External Media (0:MS, 1:SD)</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_MAKER_NOTE_SIZE</td>
      <td>MakerNote size</td>
      <td>Please sets before Capture sequence<br>Default : 40 (byte)</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_EXIF_TAG_AUTO</td>
      <td>Not used</td>
      <td>Not used</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_MAX_CUSTOM_MARKER_SIZE</td>
      <td>Not used</td>
      <td>Not used</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_APPN_SIZE</td>
      <td>Not used</td>
      <td>Not used</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_SCR_CLASS1_HSIZE</td>
      <td>Screen nail class1 width size</td>
      <td>Default : 640</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_SCR_CLASS1_VSIZE</td>
      <td>Screen nail class1 lines size</td>
      <td>Default : 480</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_SCR_CLASS2_HSIZE</td>
      <td>Screen nail class2 width size</td>
      <td>Default : 1440</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_SCR_CLASS2_VSIZE</td>
      <td>Screen nail class2 lines size</td>
      <td>Default : 1080</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_DCF_LAST_KEY</td>
      <td>Use of FJ_GetParamLong()</td>
      <td>Get Last index file number and directory number</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_DCF_DUPLICATE_OPERATION</td>
      <td>DCF-DB same file number is registerd</td>
      <td>0:same number register, 1:same number not register</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_DCF_EDIT_FILE_NOT_ADD_DATE_DB_</td>
      <td>Edit file is not add to date DB</td>
      <td>0:Add to DB, 1:Not Add to DB</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_THUMB_DRAW_WIDTH_HD</td>
      <td>Thumbnail draw width for HD</td>
      <td>Value of Width</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_THUMB_DRAW_LINES_HD</td>
      <td>Thumbnail draw lines for HD</td>
      <td>Value of Lines</td>
    </tr>
	<tr>
      <td>@ref FJ_PARA_JPEG_CUT_THRESHOLD</td>
      <td>-</td>
      <td>Parcentage ( Default: 30% )</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_VIEW_YUV_1_SET_OUTPUT_SIZE</td>
      <td>-</td>
      <td>Output Size<br>
          0 : None ( FJ_HostVideoYUVManipulation(Pre-CNR) isn't image )<br>
          1 : QVGA YUV420 format image<br>
 		  2 : QVGA YUV422 format image<br>
		  3 : VGA YUV420 format image<br>
		  4 : VGA YUV422 format image<br>
	  </td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_VIEW_YUV_2_SET_OUTPUT_SIZE</td>
      <td>-</td>
      <td>Output Size<br>
          0 : None ( FJ_HostVideoYUVManipulation(Pre-CNR) isn't image )<br>
          1 : QVGA YUV422 format image<br>
 		  2 : QVGA YUV420 format image<br>
		  3 : VGA YUV422 format image<br>
		  4 : VGA YUV420 format image<br>
	  </td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_VIEW_YUV_1_SET_OUTPUT_AREA</td>
      <td>-</td>
      <td>Output Address<br>
          0 : Output area to 0 of the whole 3<br>
          1 : Output area to 1 of the whole 3<br>
 		  2 : Output area to 2 of the whole 3<br>
	  </td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_VIEW_YUV_2_SET_OUTPUT_AREA</td>
      <td>-</td>
      <td>Output Address<br>
          0 : Output area to 0 of the whole 3<br>
          1 : Output area to 1 of the whole 3<br>
 		  2 : Output area to 2 of the whole 3<br>
	  </td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_CAPTURE_YUV_1_SET_OUTPUT_SIZE</td>
      <td>-</td>
      <td>Output Size<br>
          0 : None ( FJ_HostStillYUVManipulation(Pre-CNR) isn't image )<br>
          1 : QVGA YUV420 format image<br>
 		  2 : QVGA YUV422 format image<br>
		  3 : VGA YUV420 format image<br>
		  4 : VGA YUV422 format image<br>
	  </td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_CAPTURE_YUV_2_SET_OUTPUT_SIZE</td>
      <td>-</td>
      <td>Output Size<br>
          0 : None ( FJ_HostStillYUVManipulation(Post-CNR) isn't image )<br>
          1 : QVGA YUV422 format image<br>
 		  2 : QVGA YUV420 format image<br>
		  3 : VGA YUV422 format image<br>
		  4 : VGA YUV420 format image<br>
	  </td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_CAPTURE_YUV_3_SET_OUTPUT_SIZE</td>
      <td>-</td>
      <td>Output Size<br>
          0 : None ( FJ_HostStillYUVManipulation(Post-WARP) isn't image )<br>
          1 : QVGA YUV420 format image<br>
 		  2 : QVGA YUV422 format image<br>
		  3 : VGA YUV420 format image<br>
		  4 : VGA YUV422 format image<br>
	  </td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_CAPTURE_YUV_3_SET_OUTPUT_SIZE</td>
      <td>-</td>
      <td>Output Size<br>
          0 : None ( FJ_HostStillYUVManipulation(Post-AFN) isn't image )<br>
          1 : QVGA YUV422 format image<br>
 		  2 : QVGA YUV420 format image<br>
		  3 : VGA YUV422 format image<br>
		  4 : VGA YUV420 format image<br>
	  </td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_CAPTURE_YUV_1_SET_OUTPUT_AREA</td>
      <td>-</td>
      <td>Output Address<br>
          0 : Output area to 0 of the whole 3<br>
          1 : Output area to 1 of the whole 3<br>
 		  2 : Output area to 2 of the whole 3<br>
	  </td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_CAPTURE_YUV_2_SET_OUTPUT_AREA</td>
      <td>-</td>
      <td>Output Address<br>
          0 : Output area to 0 of the whole 3<br>
          1 : Output area to 1 of the whole 3<br>
 		  2 : Output area to 2 of the whole 3<br>
	  </td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_CAPTURE_YUV_3_SET_OUTPUT_AREA</td>
      <td>-</td>
      <td>Output Address<br>
          0 : Output area to 0 of the whole 3<br>
          1 : Output area to 1 of the whole 3<br>
 		  2 : Output area to 2 of the whole 3<br>
	  </td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_CAPTURE_YUV_4_SET_OUTPUT_AREA</td>
      <td>-</td>
      <td>Output Address<br>
          0 : Output area to 0 of the whole 3<br>
          1 : Output area to 1 of the whole 3<br>
 		  2 : Output area to 2 of the whole 3<br>
	  </td>
    </tr>
//@endcond
   <tr>
      <td>@ref FJ_PARA_MOVIE_REC_SYNC</td>
      <td>The return of FJ_CaptureClipCmd() shows synchronization or the asynchronization.</td>
      <td>1(=synchronization) / 0(=asynchronization)</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_MOVIE_PLAY_SYNC</td>
      <td>The return of FJ_PlaybackClipCmd() shows synchronization or the asynchronization.</td>
      <td>1(=synchronization) / 0(=asynchronization)</td>
    </tr>
//@cond
   <tr>
      <td>@ref FJ_PARA_MOVIE_SCR_CLASS1_HSIZE</td>
      <td>Width of Class 1 at Single Trimming is set.</td>
      <td>Value of Width</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_MOVIE_SCR_CLASS1_VSIZE</td>
      <td>Lines of Class 1 at Single Trimming is set.</td>
      <td>Value of Lines</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_MOVIE_SCR_CLASS2_HSIZE</td>
      <td>Width of Class 2 at Single Trimming is set.</td>
      <td>Value of Width</td>
    </tr>
    <tr>
      <td>@ref FJ_PARA_MOVIE_SCR_CLASS2_VSIZE</td>
      <td>Lines of Class 2 at Single Trimming is set.</td>
      <td>Value of Lines</td>
    </tr>
//@endcond
</table>
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
extern FJ_ERR_CODE FJ_SetParam( USHORT wParamID, USHORT wValue );

//@cond
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get parameter value according to parameter ID.<br>
@param[in] wParamID The operation ID of this function<br>
@param[out] pwValue pointer of parameter value<br>
@retval FJ_ERR_OK
@retval FJ_ERR_NG
@remarks
Function operation
   <table>
   <tr>
      <th>wParamID</th>
      <th>Function Operation</th></tr>
   <tr>
      <td>@ref FJ_PARA_DCF_LAST_KEY</td>
      <td align=left>Get number of last index file and directory.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCF_DUPLICATE</td>
      <td align=left>Get last duplicate number in all DCF object.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCF_HIDDEN</td>
      <td align=left>Get last hidden number in all DCF object.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCF_ASSIGN_LAST_KEY</td>
      <td align=left>Get last number in only in DCF object of charctor set in FJ_SetDCFFreeChars().</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCF_ASSIGN_DUPLICATE</td>
      <td align=left>Get last duplicate number in only in DCF object of charctor set in FJ_SetDCFFreeChars().</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCF_ASSIGN_HIDDEN</td>
      <td align=left>Get last hidden number in only in DCF object of charctor set in FJ_SetDCFFreeChars().</td></tr>
   </table>
<br>
<br>
The meaning to have of bit of the "pwValue"
   <table>
   <tr>
      <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0-13</td>
      <td align=left>File numeber</td></tr>
   <tr>
      <td>14-31</td>
      <td align=left>Directory number</td></tr>
   </table>
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
extern FJ_ERR_CODE	FJ_GetParamLong(USHORT wParamID, LONG* pwValue);
//@endcond
/*@} --- end of fj_param_api */
//@endcond

#endif
