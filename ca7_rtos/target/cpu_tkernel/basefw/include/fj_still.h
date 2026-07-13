/**
 * @file		fj_still.h
 * @brief		Still and Video API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef __FJ_STILL_H__
#define __FJ_STILL_H__

#include "ddim_typedef.h"
#include "fj_top.h"
#include "fj_iq.h"
#include "fj_custom.h"
#include "fj_mode.h"
#include "image_info.h"
#include "fj_disp.h"	// ### temporary solution to pass compile
#include "ip_eis.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_still_overview
@{
	<br>
	This chapter describes about Video record, Still capture, and those related functions.<br>
	- @ref fj_still_api_configuration
		- @ref fj_still_api_configuration_window
		- @ref fj_still_api_configuration_connectivity_overview
			- @ref fj_still_api_configuration_connectivity_video_rec
			- @ref fj_still_api_configuration_connectivity_still_cap
			- @ref fj_still_api_configuration_connectivity_piv
			- @ref fj_still_api_configuration_connectivity_sensor2hdmi_bypass
			- @ref fj_still_api_configuration_connectivity_stitch
			- @ref fj_still_api_configuration_connectivity_zsl
		- @ref fj_still_imagemacromode_overview
			- @ref fj_still_image_macro_mode_00
			- @ref fj_still_image_macro_mode_01
			- @ref fj_still_image_macro_mode_02
			- @ref fj_still_image_macro_mode_03
			- @ref fj_still_image_macro_mode_04
			- @ref fj_still_image_macro_mode_05
			- @ref fj_still_image_macro_mode_06
			- @ref fj_still_image_macro_mode_07
			- @ref fj_still_image_macro_mode_08
			- @ref fj_still_image_macro_mode_09
			- @ref fj_still_image_macro_mode_0A
			- @ref fj_still_image_macro_mode_0B
			- @ref fj_still_image_macro_mode_0C
			- @ref fj_still_image_macro_mode_0D
			- @ref fj_still_image_macro_mode_0E
			- @ref fj_still_image_macro_mode_0F

	<hr>
	@section fj_still_api_configuration	Still Configuration
	Base Firmware supports multi hardware devices connection and flexible processing combinations.<br>
	(e.g. Multi-image sensor devices connection, Multi-video encoder connection, Multi-display devices connection...)<br>
	Base Firmware manages image processing configurations as follows.<br>
	<table>
		<tr>
			<th>Category</th>
			<th>Description</th>
			<th>Window Size</th>
			<th>Open/Close</th>
			<th>Connect/Disconnect</th>
			<th>Optional</th>
		</tr>
		<tr>
			<td bgcolor="#d3d3d3">Sensor</td>
			<td bgcolor="#d3d3d3">Configurations of<br>the sensor device image processing</td>
			<td>FJ_Set_Sensor_Window()</td>
			<td>-</td>
			<td>FJ_Sensor_Connect()<br>FJ_Sensor_Disconnect()</td>
			<td>-</td>
		</tr>
		<tr>
			<td bgcolor="#d3d3d3">SEN</td>
			<td bgcolor="#d3d3d3">Configurations of<br>the sensor readout image processing</td>
			<td>FJ_Set_Sen_Window()</td>
			<td>FJ_Sen_Open()<br>FJ_Sen_Close()</td>
			<td>FJ_Sen_Connect()<br>FJ_Sen_Connect_Display()<br>FJ_Sen_Disconnect()</td>
			<td>-</td>
		</tr>
		<tr>
			<td bgcolor="#d3d3d3">Demosaic</td>
			<td bgcolor="#d3d3d3">Configurations of<br>the image processing of <br>converting the RAW to YUV</td>
			<td>FJ_Set_Demosaic_Window()</td>
			<td>FJ_Demosaic_Open()<br>FJ_Demosaic_Close()</td>
			<td rowspan="4">FJ_Demosaic_Connect_Encode()<br>FJ_Demosaic_Disconnect_Encode()<br>
							FJ_Demosaic_Connect_Display()<br>FJ_Demosaic_Disconnect_Display()<br>
							FJ_Demosaic_Connect_Stitch()<br>FJ_Demosaic_Disconnect_Stitch()<br>
							FJ_Stitch_Connect_Encode()<br>FJ_Stitch_Disconnect_Encode()<br>
							FJ_Stitch_Connect_Display()<br>FJ_Stitch_Disconnect_Display()</td>
			<td>FJ_SetDemosaicInputControl()<br>FJ_SetDemosaicOutputControl()</td>
		</tr>
		<tr>
			<td bgcolor="#d3d3d3">Stitch</td>
			<td bgcolor="#d3d3d3">Configurations of <br>the stitching image processing</td>
			<td>FJ_Set_Stitch_Window()</td>
			<td>FJ_Stitch_Open()<br>FJ_Stitch_Close()</td>
			<td>-</td>
		</tr>
		<tr>
			<td bgcolor="#d3d3d3">Encode</td>
			<td bgcolor="#d3d3d3">Configurations of <br>the encoder image processing</td>
			<td>Video Encoder : <br>
				FJ_VideoEncoder_Config()<br>
				<hr>
				JPEG Encoder  : <br>
				FJ_Set_Encode_Window()</td>
			<td>Video Encoder : <br>
				FJ_VideoEncoder_Open()<br>FJ_VideoEncoder_Close()<br>
				<hr>
				JPEG Encoder  : <br>
				FJ_StillEncoder_Open()<br>FJ_StillEncoder_Close()<br></td>
			<td>-</td>
		</tr>
		<tr>
			<td bgcolor="#d3d3d3">Display</td>
			<td bgcolor="#d3d3d3">Configurations of <br>the displaying image processing</td>
			<td>FJ_Set_Display_Window()</td>
			<td>FJ_Disp_Open()<br>FJ_Disp_Close()</td>
			<td>-</td>
		</tr>
	</table>
	About Window Size, please refer to @ref fj_still_api_configuration_window .<br>
	About Open/Close & Connect/Disconnect, please refer to @ref fj_still_api_configuration_connectivity_overview .<br>

	<hr>
	@subsection fj_still_api_configuration_window					Image Window Configuration
	Each image processing window size details is following.<br>
	@image html fj_still_window_setting.png

	<hr>
	@subsection fj_still_api_configuration_connectivity_overview	Image Connectivity Overview
	Base Firmware provides flexible connectivity of hardware module and Software.<br>
	It can connect multi hardware module to multi image processing.<br>
	Each connect configuration API is following.<br>
	For more detailed of configuration APIs, please refer to description of each configuration APIs.<br>
	@image html fj_still_connectivity_overview.png

	<hr>
	@subsection fj_still_api_configuration_connectivity_video_rec	Image Connectivity Video Record Sample
	Sample connection setting of Video recording is as following.<br>
	@image html fj_still_connectivity_video_rec_sample.png

	<hr>
	@subsection fj_still_api_configuration_connectivity_still_cap	Image Connectivity Still Sample
	Sample connection setting of Still is as following.<br>
	@image html fj_still_connectivity_still_cap_sample.png

	<hr>
	@subsection fj_still_api_configuration_connectivity_piv	Image Connectivity PIV Sample
	Sample connection setting of PIV is as following.<br>
	@image html fj_still_connectivity_piv_sample.png

	<hr>
	@subsection fj_still_api_configuration_connectivity_sensor2hdmi_bypass	Image Connectivity Sensor to HDMI Bypass Sample
	Sample connection setting of Sensor to HDMI Bypass is as following.<br>
	@image html fj_still_connectivity_sensor2hdmi_bypass_sample.png

	<hr>
	@subsection fj_still_api_configuration_connectivity_stitch	Image Connectivity Stitch Sample
	Sample connection setting of Stitch is as following.<br>
	@image html fj_still_connectivity_stitch_sample.png

	<hr>
	@subsection fj_still_api_configuration_connectivity_zsl	Image Connectivity Zero Shutter Lag Sample
	Sample connection setting of Zero Shutter Lag is as following.<br>
	@image html fj_still_connectivity_zsl_sample.png

	<hr>
	@subsection fj_still_imagemacromode_overview			Image Macro Mode
	Image Macro Mode indicates a behavior of Image processing to BaseFW.<br>
	This mode related to each instances connection conditions.<br>
	Please refer to the @ref fj_still_api_configuration_connectivity_overview and following figure.
	<hr>
	@subsection fj_still_image_macro_mode_00	Image Macro Mode 00 detailed( for Still/Burst/Continuous capture :1pipe )
	@image html fj_still_image_macro_mode_00.png

	<hr>
	@subsection fj_still_image_macro_mode_01	Image Macro Mode 01 detailed( for Video loop/record :1pipe )
	@image html fj_still_image_macro_mode_01.png

	<hr>
	@subsection fj_still_image_macro_mode_02	Image Macro Mode 02 detailed( for 2 sensor usecase w/ stitching )
	@image html fj_still_image_macro_mode_02.png

	<hr>
	@subsection fj_still_image_macro_mode_03	Image Macro Mode 03 detailed( for 4 sensor )
	@image html fj_still_image_macro_mode_03.png

	<hr>
	@subsection fj_still_image_macro_mode_04	Image Macro Mode 04 detailed( for Video loop/record (High power mode :2pipe) )
	@image html fj_still_image_macro_mode_04.png

	<hr>
	@subsection fj_still_image_macro_mode_05	Image Macro Mode 05 detailed( for Video loop/record (Low power mode :1pipe) )
	@image html fj_still_image_macro_mode_05.png

	<hr>
	@subsection fj_still_image_macro_mode_06	Image Macro Mode 06 detailed( for Clean bayer output (SEN out) )
	@image html fj_still_image_macro_mode_06.png

	<hr>
	@subsection fj_still_image_macro_mode_07	Image Macro Mode 07 detailed( for Clean bayer output (SEN-Demosaic direct out /w BA ) )
	@image html fj_still_image_macro_mode_07.png

	<hr>
	@subsection fj_still_image_macro_mode_08	Image Macro Mode 08 detailed( for Clean bayer output (SEN-Demosaic in-direct out) )
	@image html fj_still_image_macro_mode_08.png

	<hr>
	@subsection fj_still_image_macro_mode_09	Image Macro Mode 09 detailed( for Video record w/ PIV )
	@image html fj_still_image_macro_mode_09.png

	<hr>
	@subsection fj_still_image_macro_mode_0A	Image Macro Mode 0A detailed
	Reserved.

	<hr>
	@subsection fj_still_image_macro_mode_0B	Image Macro Mode 0B detailed
	Reserved.

	<hr>
	@subsection fj_still_image_macro_mode_0C	Image Macro Mode 0C detailed
	Reserved.

	<hr>
	@subsection fj_still_image_macro_mode_0D	Image Macro Mode 0D detailed
	Reserved.

	<hr>
	@subsection fj_still_image_macro_mode_0E	Image Macro Mode 0E detailed
	Reserved.

	<hr>
	@subsection fj_still_image_macro_mode_0F	Image Macro Mode 0F detailed
	Reserved.
@}*//* --- end of fj_still_overview */


///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_still_sequence
@{
- @ref fj_still_capture_sequence
    - @ref fj_still_capture_sequence_section1
	- @ref fj_still_capture_sequence_section2
	- @ref fj_still_capture_sequence_section3
	- @ref fj_still_capture_sequence_section4
	- @ref fj_still_capture_sequence_section5
	- @ref fj_still_capture_sequence_section13
	- @ref fj_still_capture_sequence_section12
	- @ref fj_still_capture_sequence_section6
	- @ref fj_still_capture_sequence_section7
	- @ref fj_still_capture_sequence_section8
	- @ref fj_still_capture_sequence_section9
	- @ref fj_still_capture_sequence_section10
	- @ref fj_still_capture_sequence_section11
	- @ref fj_still_capture_sequence_section14

<hr>
@section fj_still_capture_sequence	Still Sequence
@subsection fj_still_capture_sequence_section1	Video Loop
@image html fj_video_loop.png

<hr>
@subsection fj_still_capture_sequence_section2	Video Loop in Zero Shutter Lag
@image html fj_video_loop_zsl.png

<hr>
@subsection fj_still_capture_sequence_section3	PIV( Picture In Video )
@image html fj_still_capture_in_video_record.png

<hr>
@subsection fj_still_capture_sequence_section4	Stitching
@image html fj_video_loop_stitch.png

<hr>
@subsection fj_still_capture_sequence_section5	Still Capture / Burst Capture
@image html fj_still_capture.png

<hr>
@subsection fj_still_capture_sequence_section13	Clean Bayer
@image html fj_still_capture_clean_bayer.png

<hr>
@subsection fj_still_capture_sequence_section12	Sensor to HDMI Bypass
@image html fj_video_loop_sensor2hdmi.png

<hr>
@subsection fj_still_capture_sequence_section6	Still HDR
@image html fj_still_shdr.png

<hr>
@subsection fj_still_capture_sequence_section7	Sensor Mode Changing During Video Record
@image html fj_reconfigure_sensor.png

<hr>
@subsection fj_still_capture_sequence_section8	Mode Close Process
@image html fj_still_close.png

<hr>
@subsection fj_still_capture_sequence_section9	Imaging Process(SEN-SRO-B2B-B2R-LTM-R2Y-CNR)
@image html fj_still_sen2cnr.png

<hr>
@subsection fj_still_capture_sequence_section10	Imaging Process(SRO-B2B-B2R-LTM-R2Y-CNR)
@image html fj_still_sro2cnr.png

<hr>
@subsection fj_still_capture_sequence_section11	(Socionext SDK only)File Save Process
@image html sociosdk_opt_file_save.png

<hr>
@subsection fj_still_capture_sequence_section14	Panorama
@image html fj_still_panorama.png
@} */	// fj_still_sequence group


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/** @weakgroup fj_still_definition
@{*/

/** for bConfig in FJ_ConfigCapture() */
//#define FJ_CC_DATA_STAMP						(0x01)		/**< Data stamp enable/disable			*/
//#define FJ_CC_SCR_VGA_EN						(0x02)		/**< ScreenNail VGA enable/disable		*/
//#define FJ_CC_SCR_HD_EN							(0x03)		/**< ScreenNail HD enable/disable		*/
#define FJ_CC_RECORD_TYPE						(0x04)		/*+< Record type						*/
#define	FJ_CC_ZOOM_MODE							(0x05)		/**< Zoom mode							*/
#define FJ_CC_BAYER_TYPE						(0x09)		/**< Bayer Format Type					*/
#define FJ_CC_CAP_RESIZE_MODE					(0x0C)		/**< Resize mode ex)bi-linear, bi-cubic	*/
#define	FJ_CC_SHUTTER_MODE						(0x0E)		/**< Shutter Mode for still				*/
#define FJ_CC_RAW_SAVE_EN						(0x0F)		/**< RAW save enable/disable			*/
#define FJ_CC_OBD_MODE_EN						(0x11)		/**< OBD Mode Select					*/
#define FJ_CC_OBT_MODE_EN						(0x12)		/**< OBT Mode Select					*/
#define FJ_CC_RAW_COMPENSATION_EN				(0x16)		/**< RAW compensation enable/disable	*/

#define FJ_CC_PRE_CNR_HISTOGRAM_EN				(0x17)		/**< Pre-CNR histogram enable/disable	*/
#define FJ_CC_STILL_COUNT						(0x18)		/**< still count setting				*/
//#define FJ_CC_PIV_ROW_DIVISION_NUM				(0x19)		/**< Row division number of still processing in PIV mode	*/
//#define FJ_CC_PIV_COLUMN_DIVISION_NUM			(0x1A)		/**< Column division number of still processing in PIV mode	*/
#define FJ_CC_BTC_THUMBNAIL_EN					(0x1B)		/**< Before TONE Thumbnail enable/disable	*/
#define FJ_CC_BTC_HISTOGRAM_EN					(0x1C)		/**< Before TONE histogram enable/disable	*/
#define FJ_CC_RAW_INSERTION_ONOFF				(0x1D)		/**< RAW Insertion On/Off				*/
#define FJ_CC_MFNR_ONOFF						(0x1E)		/**< MFNR On/Off						*/
#define FJ_CC_TONE_AUTO_MANUAL					(0x1F)		/**< Tone auto/manual(for Global Tone Mapping	*/


/** for bConfig in FJ_ConfigCapture()	(Not used) */
#define FJ_JPEG									(0x00)		/** (Not used)			*/
#define FJ_TIFF									(0x01)		/** (Not used)			*/

#define HOST_MANIPULATION_CONTINUE				(0x00)		/**< N in 1 mode wasn't initialized or intermediate
																 N in 1 images have to be saved	*/
#define HOST_MANIPULATION_DONT_STORE			(0x01)		/**< The captured image shouldn't be stored	*/

/**
@def SENSOR_FIRST_PIXEL_RG
Sensor first pixel RG
*/
/**
@def SENSOR_FIRST_PIXEL_GR
Sensor first pixel GR
*/
/**
@def SENSOR_FIRST_PIXEL_GB
Sensor first pixel GB
*/
/**
@def SENSOR_FIRST_PIXEL_BG
Sensor first pixel BG
*/

/** Stream Type for Stream ID
 */
#define FJ_STREAM_TYPE_RAW						(0x0100)	/**< Stream Type : RAW	*/
#define FJ_STREAM_TYPE_YUV						(0x0200)	/**< Stream Type : YUV	*/
#define FJ_STREAM_TYPE_H264						(0x0300)	/**< Stream Type : H264	*/
#define FJ_STREAM_TYPE_JPEG						(0x0400)	/**< Stream Type : JPEG	*/
#define FJ_STREAM_TYPE_AAC						(0x0500)	/**< Stream Type : AAC	*/
#define FJ_STREAM_TYPE_G711						(0x0600)	/**< Stream Type : G711	*/

#define D_FJ_OSD_DRAW_RECT_NUM					(1000)		/**< The limit of rects on OSD	*/

// FJ_STILL_SNR_ELEM_FUNC's return code
#define FJ_STILL_SEQ_ERR_OK					(0)				/**< Imaging scenario element was successful. */
#define FJ_STILL_SEQ_ERR_SKIP				(1)				/**< Imaging scenario element was skip. */
#define FJ_STILL_SEQ_ERR_MAP_BUSY			(2)				/**< Imaging scenario element was waiting memory map. Retry current element after timming. */
#define FJ_STILL_SEQ_ERR_WORKER_BUSY		(3)				/**< Reserved. */
#define FJ_STILL_SEQ_ERR_WAITING			(4)				/**< Imaging scenario element was pausing. */
#define FJ_STILL_SEQ_ERR_DDIM_ERR			(5)				/**< Imaging scenario element was detected Driver error. */
#define FJ_STILL_SEQ_ERR_FJ_ERR				(6)				/**< Imaging scenario element was detected FJ_API error. */
#define FJ_STILL_SEQ_ERR_OS_ERR				(7)				/**< Imaging scenario element was detected OS service call error. */
#define FJ_STILL_SEQ_ERR_PARAM_ERR			(8)				/**< Imaging scenario element was detected parameter error. */
#define FJ_STILL_SEQ_ERR_SNR_ERR			(9)				/**< Reserved. */
#define FJ_STILL_SEQ_ERR_MSG_ERR			(10)			/**< Imaging scenario element function pointer is NULL. */
#define FJ_STILL_SEQ_ERR_INTERNAL_ERR		(11)			/**< BaseFW internal use only. (Internal error of Imaging scenario sequencer.) */
#define FJ_STILL_SEQ_ERR_TERMINATING_ERR	(12)			/**< Imaging scenario sequencer goto terminating. */

#define FJ_STILL_SEQ_TIMEOUT_MSEC			(1000)			/**< Imaging scenario sequencer timeout time. (msec). The zero mean forever. */

#define FJ_STILL_IMG_SRAM_BANK_NUM_MAX		(8)				/**< Image SRAM Bank MAX number					*/

/** Sensor instance ID
 */
#define FJ_IMAGE_SENSOR_ID_0				(0x01)			/**< Sensor instance ID 0 */
#define FJ_IMAGE_SENSOR_ID_1				(0x02)			/**< Sensor instance ID 1 */
#define FJ_IMAGE_SENSOR_ID_2				(0x03)			/**< Sensor instance ID 2 */
#define FJ_IMAGE_SENSOR_ID_3				(0x04)			/**< Sensor instance ID 3 */

/** SEN instance ID
 */
#define FJ_IMAGE_SEN_ID_0					(0x01)			/**< SEN instance ID 0 */
#define FJ_IMAGE_SEN_ID_1					(0x02)			/**< SEN instance ID 1 */
#define FJ_IMAGE_SEN_ID_2					(0x03)			/**< SEN instance ID 2 */
#define FJ_IMAGE_SEN_ID_3					(0x04)			/**< SEN instance ID 3 */

/** Demosaic instance ID
 */
#define FJ_IMAGE_DEMOSAIC_ID_0				(0x01)			/**< Demosaic instance ID 0 */
#define FJ_IMAGE_DEMOSAIC_ID_1				(0x02)			/**< Demosaic instance ID 1 */
#define FJ_IMAGE_DEMOSAIC_ID_2				(0x03)			/**< Demosaic instance ID 2 */
#define FJ_IMAGE_DEMOSAIC_ID_3				(0x04)			/**< Demosaic instance ID 3 */
#define FJ_IMAGE_DEMOSAIC_ID_4				(0x05)			/**< Demosaic instance ID 4 */
#define FJ_IMAGE_DEMOSAIC_ID_5				(0x06)			/**< Demosaic instance ID 5 */
#define FJ_IMAGE_DEMOSAIC_ID_6				(0x07)			/**< Demosaic instance ID 6 */
#define FJ_IMAGE_DEMOSAIC_ID_7				(0x08)			/**< Demosaic instance ID 7 */
#define FJ_IMAGE_DEMOSAIC_ID_8				(0x09)			/**< Demosaic instance ID 8 */
#define FJ_IMAGE_DEMOSAIC_ID_9				(0x0A)			/**< Demosaic instance ID 9 */

/** Encode instance ID
 */
#define FJ_IMAGE_ENCODE_ID_0				(0x01)			/**< Encode instance ID 0 */
#define FJ_IMAGE_ENCODE_ID_1				(0x02)			/**< Encode instance ID 1 */
#define FJ_IMAGE_ENCODE_ID_2				(0x03)			/**< Encode instance ID 2 */
#define FJ_IMAGE_ENCODE_ID_3				(0x04)			/**< Encode instance ID 3 */
#define FJ_IMAGE_ENCODE_ID_4				(0x05)			/**< Encode instance ID 4 */
#define FJ_IMAGE_ENCODE_ID_5				(0x06)			/**< Encode instance ID 5 */
#define FJ_IMAGE_ENCODE_ID_6				(0x07)			/**< Encode instance ID 6 */
#define FJ_IMAGE_ENCODE_ID_7				(0x08)			/**< Encode instance ID 7 */
#define FJ_IMAGE_ENCODE_ID_8				(0x09)			/**< Encode instance ID 8 */
#define FJ_IMAGE_ENCODE_ID_9				(0x0A)			/**< Encode instance ID 9 */

/** JPEG instance ID
 */
#define FJ_IMAGE_JPEG_ID_0					(0x01)			/**< JPEG instance ID 0 */
#define FJ_IMAGE_JPEG_ID_1					(0x02)			/**< JPEG instance ID 1 */
#define FJ_IMAGE_JPEG_ID_2					(0x03)			/**< JPEG instance ID 2 */
#define FJ_IMAGE_JPEG_ID_3					(0x04)			/**< JPEG instance ID 3 */
#define FJ_IMAGE_JPEG_ID_4					(0x05)			/**< JPEG instance ID 4 */
#define FJ_IMAGE_JPEG_ID_5					(0x06)			/**< JPEG instance ID 5 */

/** Disp instance ID
 */
#define FJ_IMAGE_DISP_ID_0					(0x01)			/**< DISP instance ID 0 */
#define FJ_IMAGE_DISP_ID_1					(0x02)			/**< DISP instance ID 1 */

/** LCD instance ID
 */
#define FJ_IMAGE_LCD_ID_0					(0x01)			/**< LCD instance ID 0 */

/** HDMI instance ID
 */
#define FJ_IMAGE_HDMI_ID_0					(0x01)			/**< HDMI instance ID 0 */

/** MAX number
 */
#define FJ_DEMOSAIC_OUTPUT_NUM_MAX			(6)				/**< Max number of Demosaic output */
#define FJ_STITCH_OUTPUT_NUM_MAX			(3)				/**< Max number of Stitch output */

/** Raw output part
 */
#define FJ_STILL_RAW_OUTPUT_PART_NONE	(0x00)		/**< Raw does not out	*/
#define FJ_STILL_RAW_OUTPUT_PART_SEN	(0x01)		/**< Raw out from SEN	*/
#define FJ_STILL_RAW_OUTPUT_PART_SRO	(0x02)		/**< Raw out from SRO	*/
#define FJ_STILL_RAW_OUTPUT_PART_B2B	(0x04)		/**< Raw out from B2B	*/


#define FJ_CONFIG_CAPTURE_COUNT_STILL_TIME_LAPSE	(0xFF)	/**< Config capture count of Still Time Lapse */


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Jpeg quality kind */
typedef enum {
	FJ_JPEG_QUALITY_KIND_Q,					/**< Quality */
	FJ_JPEG_QUALITY_KIND_CR,				/**< Compression Ratio */
} FJ_JPEG_QUALITY_KIND;

/** compression ratio */
typedef enum {
	FJ_CR_4_0	= (short)0x0000,			/**< CR 4.0  */
	FJ_CR_4_5	= (short)0x0001,			/**< CR 4.5  */
	FJ_CR_5_0	= (short)0x0002,			/**< CR 5.0  */
	FJ_CR_5_5	= (short)0x0003,			/**< CR 5.5  */
	FJ_CR_6_0	= (short)0x0004,			/**< CR 6.0  */
	FJ_CR_6_5	= (short)0x0005,			/**< CR 6.5  */
	FJ_CR_7_0	= (short)0x0006,			/**< CR 7.0  */
	FJ_CR_7_5	= (short)0x0007,			/**< CR 7.5  */
	FJ_CR_8_0	= (short)0x0008,			/**< CR 8.0  */
	FJ_CR_8_5	= (short)0x0009,			/**< CR 8.5  */
	FJ_CR_9_0	= (short)0x000A,			/**< CR 9.0  */
	FJ_CR_9_5	= (short)0x000B,			/**< CR 9.5  */
	FJ_CR_10_0	= (short)0x000C,			/**< CR 10.0 */
	FJ_CR_12_0	= (short)0x000D,			/**< CR 12.0 */
	FJ_CR_14_0	= (short)0x000E,			/**< CR 14.0 */
	FJ_CR_15_0  = (short)0x000F,			/**< CR 15.0 */
	FJ_CR_16_0  = (short)0x0010,			/**< CR 16.0 */
	FJ_CR_17_0  = (short)0x0011,			/**< CR 17.0 */
	FJ_CR_18_0  = (short)0x0012,			/**< CR 18.0 */
	FJ_CR_19_0  = (short)0x0013,			/**< CR 19.0 */
	FJ_CR_20_0  = (short)0x0014,			/**< CR 20.0 */
	FJ_CR_21_0  = (short)0x0015,			/**< CR 21.0 */
	FJ_CR_22_0  = (short)0x0016,			/**< CR 22.0 */
	FJ_CR_23_0  = (short)0x0017,			/**< CR 23.0 */
	FJ_CR_24_0  = (short)0x0018,			/**< CR 24.0 */
	FJ_CR_25_0  = (short)0x0019,			/**< CR 25.0 */
	FJ_CR_26_0  = (short)0x001A,			/**< CR 26.0 */
	FJ_CR_27_0  = (short)0x001B,			/**< CR 27.0 */
	FJ_CR_28_0  = (short)0x001C,			/**< CR 28.0 */
	FJ_CR_29_0  = (short)0x001D,			/**< CR 29.0 */
	FJ_CR_30_0  = (short)0x001E,			/**< CR 30.0 */
	FJ_CR_31_0  = (short)0x001F,			/**< CR 31.0 */
	FJ_CR_32_0  = (short)0x0020,			/**< CR 32.0 */
	FJ_CR_33_0  = (short)0x0021,			/**< CR 33.0 */
	FJ_CR_34_0  = (short)0x0022,			/**< CR 34.0 */
	FJ_CR_35_0  = (short)0x0023,			/**< CR 35.0 */
	FJ_CR_36_0  = (short)0x0024,			/**< CR 36.0 */
	FJ_CR_37_0  = (short)0x0025,			/**< CR 37.0 */
	FJ_CR_38_0  = (short)0x0026,			/**< CR 38.0 */
	FJ_CR_39_0  = (short)0x0027,			/**< CR 39.0 */
	FJ_CR_40_0  = (short)0x0028,			/**< CR 40.0 */
	FJ_CR_3_0   = (short)0x0029,			/**< CR 3.0	*/
	FJ_CR_3_5   = (short)0x002A,			/**< CR 3.5	*/
	FJ_CR_2_0   = (short)0x002B,			/**< CR 2.0	*/
	FJ_CR_2_5   = (short)0x002C,			/**< CR 2.5	*/
	FJ_CR_2_7   = (short)0x002D				/**< CR 2.7	*/
} FJ_COMPRESSION_RATIO;

/** LTM map control */
typedef enum {
	FJ_LTM_MAP_CTRL_AUTO,					/**< Auto (Hard control)	*/
	FJ_LTM_MAP_CTRL_MANUAL,					/**< Manual (Host control)	*/
} FJ_LTM_MAP_CTRL;

/** Effect Type */
typedef enum {
	FJ_CAP_EFFECT_NORMAL	= 0,			/**< Normal			*/
	FJ_CAP_EFFECT_BW		= 1,			/**< Black & White	*/
	FJ_CAP_EFFECT_SEPIA		= 2,			/**< Sepia			*/
	FJ_CAP_EFFECT_SUNSET	= 3,			/**< Sunset			*/
	FJ_CAP_EFFECT_NEGA		= 5,			/**< Nega			*/
	FJ_CAP_EFFECT_RED		= 6,			/**< Red			*/
	FJ_CAP_EFFECT_GREEN		= 7,			/**< Green			*/
	FJ_CAP_EFFECT_BLUE		= 8,			/**< Blue			*/
	FJ_CAP_EFFECT_MAX		= 9				/**< Max			*/
} FJ_CAP_EFFECT;

/** View Display Mute/Freeze mode
*/
typedef enum {
	FJ_DISP_PAUSE_ID_MUTE = 0,				/**< Mute controller	*/
	FJ_DISP_PAUSE_ID_FREEZE,				/**< Freeze controller	*/
	FJ_DISP_PAUSE_ID_MAX
} FJ_DISP_PAUSE_ID;

/** View Display Mute/Freeze mode
*/
typedef enum {
	FJ_DISP_PAUSE_FUNC_OFF = 0,				/**< Function cancel/resumel	*/
	FJ_DISP_PAUSE_FUNC_ON,					/**< Fucntion execute			*/
	FJ_DISP_PAUSE_FUNC_MAX
} FJ_DISP_PAUSE_FUNC;

/** YCC Format Type */
typedef enum {
	E_YCC_FORMAT_TYPE_NONE		= 0,		/**< Not Supported	*/
	E_YCC_FORMAT_TYPE_YCC444	= 1,		/**< YCC444			*/
	E_YCC_FORMAT_TYPE_YCC422	= 2,		/**< YCC422			*/
	E_YCC_FORMAT_TYPE_YCC420	= 3,		/**< YCC420			*/
	E_YCC_FORMAT_TYPE_RGB888	= 4,		/**< RGB888			*/
} E_YCC_FORMAT_TYPE;

/** View Shading mode */
typedef enum {
	FJ_VIEW_SDHD_MODE_SD_VGA = 0,			/**< VGA View			*/
	FJ_VIEW_SDHD_MODE_HD_720P,				/**< HD View			*/
	FJ_VIEW_SDHD_MODE_MAX,					/**< Maximum value		*/
	FJ_VIEW_SDHD_MODE_UNDEF					/**< Undefined value	*/
} FJ_VIEW_SDHD_MODE;

/** Sensor Drive mode
*/
typedef enum {
	FJ_SENSOR_DRIVE_MODE_VIEW = 0,			/**< Video mode							*/
	FJ_SENSOR_DRIVE_MODE_AF,				/**< AF Search mode						*/
	FJ_SENSOR_DRIVE_MODE_CAPTURE,			/**< still mode							*/
	FJ_SENSOR_DRIVE_MODE_MOVIE,				/**< Move rec mode						*/
	FJ_SENSOR_DRIVE_MODE_MOVIE_FHD,			/**< Move rec mode(FULL HD-30fps)		*/
	FJ_SENSOR_DRIVE_MODE_MOVIE_4K2K_30,		/**< Move rec mode(4K2K-30fps)			*/
	FJ_SENSOR_DRIVE_MODE_MOVIE_4K2K_15,		/**< Move rec mode(4K2K-15fps)			*/
	FJ_SENSOR_DRIVE_MODE_MOVIE_4K2K_7_5,	/**< Move rec mode(4K2K-7.5fps)			*/
	FJ_SENSOR_DRIVE_MODE_MOVIE_FHD_60,		/**< Move rec mode(FULL HD-60fps) # 60i	*/
	FJ_SENSOR_DRIVE_MODE_MAX
} FJ_SENSOR_DRIVE_MODE;

/** Interpolation mode for the image resize */
typedef enum {
	FJ_IMG_RESIZE_MODE_BILINEAR = 0,		/**< bi-linear mode	*/
	FJ_IMG_RESIZE_MODE_BICUBIC,				/**< bi-cubic mode	*/
} FJ_IMG_RESIZE_MODE;

/** image pixel format settings */
typedef enum {
	FJ_IMG_PFMT_YCC422 = 0,					/**< YCC422			*/
	FJ_IMG_PFMT_YCC420,						/**< YCC420			*/
	FJ_IMG_PFMT_YCC444,						/**< YCC444			*/
	FJ_IMG_PFMT_YCC400,						/**< YCC400			*/
	FJ_IMG_PFMT_VIDEO,						/**< VIDEO			*/
	FJ_IMG_PFMT_RGBA8888,					/**< RGBA8888		*/
	FJ_IMG_PFMT_RGBA4444,					/**< RGBA4444		*/
} FJ_IMG_PFMT;

/** Rotation Degree (clockwise) */
typedef enum {
  FJ_IMG_ROTATE_DEGREE_0 = 0,	/**< not rotate */
  FJ_IMG_ROTATE_DEGREE_90,		/**< rotate  90 degree (clockwise) */
  FJ_IMG_ROTATE_DEGREE_180,		/**< rotate 180 degree (clockwise) */
  FJ_IMG_ROTATE_DEGREE_270,		/**< rotate 270 degree (clockwise) */
} FJ_IMG_ROTATE_DEGREE;

/** image out display */
typedef enum {
	FJ_IMG_DISPLAY_LCD = 0,					/**< LCD out			*/
	FJ_IMG_DISPLAY_HDMI						/**< HDMI out			*/
} FJ_IMG_DISPLAY;

/** Imaging scenario type. (sync/async/end) */
typedef enum {
	FJ_STILL_SNR_SYNTYP_SYNC = 0,			/**< Imaging scenario element function call sync. (Not from worker task). */
	FJ_STILL_SNR_SYNTYP_ASYNC,				/**< Imaging scenario element function call async. (From worker task). */
	FJ_STILL_SNR_SYNTYP_END,				/**< End marker. */
	FJ_STILL_SNR_SYNTYP_MAX,				/**< Maximum value of @ref FJ_STILL_SNR_SYNTYP. */
} FJ_STILL_SNR_SYNTYP;

/** Image pixel data depth. */
typedef enum {
	FJ_IMG_DEPTH_8BIT = 0,					/**< 8bits. */
	FJ_IMG_DEPTH_12BIT,						/**< 12bits. (packed format) */
	FJ_IMG_DEPTH_16BIT,						/**< 16bits. */
	FJ_IMG_DEPTH_MAX,						/**< Maximum value of @ref FJ_IMG_DEPTH. */
} FJ_IMG_DEPTH;

/**
Stream image process type.
*/
typedef enum {
	E_FJ_STILL_STERAM_PROC_TYPE_THROUGH = 0,			/**< Through image. */
	E_FJ_STILL_STERAM_PROC_TYPE_IIP_SCALLER,			/**< IIP Scaller. */
	E_FJ_STILL_STERAM_PROC_TYPE_IIP_SCALLER_LPF,		/**< IIP Scaller and LPF. */
	E_FJ_STILL_STERAM_PROC_TYPE_IIP_DZOOM,				/**< IIP Digital-zoom. */
	E_FJ_STILL_STERAM_PROC_TYPE_COPY,					/**< Imge copy. */
	E_FJ_STILL_STERAM_PROC_TYPE_IIP_SPLIT_DZOOM,		/**< 4K2K path. */

	E_FJ_STILL_STERAM_PROC_TYPE_IIP_FHD__IIP_SCALLER,	/**< IIP(Shared)->IIP Scaller.
															 "IIP(Shared)" mean Pre-resize to FullHD. Pre-resizer shared between following type.
																- @ref E_FJ_STILL_STERAM_PROC_TYPE_IIP_FHD__IIP_SCALLER
																- @ref E_FJ_STILL_STERAM_PROC_TYPE_IIP_FHD__IIP_SCALLER_LPF
															*/
	E_FJ_STILL_STERAM_PROC_TYPE_IIP_FHD__IIP_SCALLER_LPF,	/**< IIP(Shared)->IIP Scaller and LPF.
															 "IIP(Shared)" mean Pre-resize to FullHD. Pre-resizer shared between following type.
																- @ref E_FJ_STILL_STERAM_PROC_TYPE_IIP_FHD__IIP_SCALLER
																- @ref E_FJ_STILL_STERAM_PROC_TYPE_IIP_FHD__IIP_SCALLER_LPF
															*/
	E_FJ_STILL_STERAM_PROC_TYPE_JPG_ENCODE,					/**< JPEG Encode. */
	E_FJ_STILL_STERAM_PROC_TYPE_MAX,					/**< Maximum value of @ref E_FJ_STILL_STERAM_PROC_TYPE. */
} E_FJ_STILL_STERAM_PROC_TYPE;

/** Image procedure status. */
typedef enum {
	E_FJ_IMGPROC_STAT_BEGIN = 0,			/**< Image procedure begin frame processing. */
	E_FJ_IMGPROC_STAT_GETADDR_ALL_STRM,		/**< Image procedure get image memory from movie codec (All streams get image memory same frame). */
	E_FJ_IMGPROC_STAT_GETADDR,				/**< Image procedure get image memory from movie codec. */
	E_FJ_IMGPROC_STAT_FINISH,				/**< Image procedure finish frame processing. */
	E_FJ_IMGPROC_STAT_MAX,					/**< Maximum value of this enum. */
} E_FJ_IMGPROC_STAT;

/** Image form.
*/
typedef enum {
	FJ_STILL_IP_IMAGE_FORM_420P		= 0,	/**< YCC420 plane		*/
	FJ_STILL_IP_IMAGE_FORM_422P		= 1,	/**< YCC422 plane		*/
	FJ_STILL_IP_IMAGE_FORM_444P		= 2,	/**< YCC424 plane		*/
	FJ_STILL_IP_IMAGE_FORM_420SP	= 3,	/**< YCC420 semi plane	*/
	FJ_STILL_IP_IMAGE_FORM_422SP	= 4,	/**< YCC422 semi plane	*/
	FJ_STILL_IP_IMAGE_FORM_444SP	= 5,	/**< YCC424 semi plane	*/
	FJ_STILL_IP_IMAGE_FORM_GRAY		= 6,	/**< Y only				*/
	FJ_STILL_IP_IMAGE_FORM_RGB		= 7,	/**< RGB				*/
	FJ_STILL_IP_IMAGE_FORM_BAY		= 8,	/**< Bayer				*/
	FJ_STILL_IP_IMAGE_FORM_CBCR_P	= 9,	/**< CbCr plane			*/
	FJ_STILL_IP_IMAGE_FORM_CBCR_C 	= 10,	/**< CbCr chunky		*/
	FJ_STILL_IP_IMAGE_FORM_RGB_P	= 11,	/**< RGB plane			*/
	FJ_STILL_IP_IMAGE_FORM_RGB_C	= 12 	/**< RGB chunky			*/
} FJ_STILL_IP_IMAGE_FORM;

/** Image data form.
*/
typedef enum {
	FJ_STILL_IP_IMAGE_DATA_FORM_PACK	= 0,	/**< Pack	*/
	FJ_STILL_IP_IMAGE_DATA_FORM_UNPACK	= 1		/**< Unpack	*/
} FJ_STILL_IP_IMAGE_DATA_FORM;

// @cond
/** Sensor Enumelation
*/
typedef enum {
	FJ_SENSOR_ENUM_0 = 0,	/**< SENSOR_0 */
	FJ_SENSOR_ENUM_1 = 1,	/**< SENSOR_1 */
	FJ_SENSOR_ENUM_2 = 2,	/**< SENSOR_2 */
	FJ_SENSOR_ENUM_3 = 3,	/**< SENSOR_3 */
	FJ_SENSOR_ENUM_MAX		/**< Stopper */
} FJ_SENSOR_ENUM;
// @endcond

/** SEN Enumelation
*/
typedef enum {
	FJ_SEN_ENUM_0 = 0,		/**< SEN_0 */
	FJ_SEN_ENUM_1 = 1,		/**< SEN_1 */
	FJ_SEN_ENUM_2 = 2,		/**< SEN_2 */
	FJ_SEN_ENUM_3 = 3,		/**< SEN_3 */
	FJ_SEN_ENUM_4 = 4,		/**< SEN_4 */
	FJ_SEN_ENUM_5 = 5,		/**< SEN_5 */
	FJ_SEN_ENUM_6 = 6,		/**< SEN_6 */
	FJ_SEN_ENUM_7 = 7,		/**< SEN_7 */
	FJ_SEN_ENUM_8 = 8,		/**< SEN_8 */
	FJ_SEN_ENUM_9 = 9,		/**< SEN_9 */
	FJ_SEN_ENUM_MAX			/**< Stopper */
} FJ_SEN_ENUM;

/** Demosaic Enumelation
*/
typedef enum {
	FJ_DEMOSAIC_ENUM_0 = 0,		/**< DEMOSAIC_0 (For Video Mode) */
	FJ_DEMOSAIC_ENUM_1 = 1,		/**< DEMOSAIC_1 (For Still Mode) */
	FJ_DEMOSAIC_ENUM_2 = 2,		/**< DEMOSAIC_2 (For PIV) */
	FJ_DEMOSAIC_ENUM_3 = 3,		/**< DEMOSAIC_3 (Reserved) */
	FJ_DEMOSAIC_ENUM_4 = 4,		/**< DEMOSAIC_4 (Reserved) */
	FJ_DEMOSAIC_ENUM_5 = 5,		/**< DEMOSAIC_5 (Reserved) */
	FJ_DEMOSAIC_ENUM_6 = 6,		/**< DEMOSAIC_6 (Reserved) */
	FJ_DEMOSAIC_ENUM_7 = 7,		/**< DEMOSAIC_7 (Reserved) */
	FJ_DEMOSAIC_ENUM_8 = 8,		/**< DEMOSAIC_8 (Reserved) */
	FJ_DEMOSAIC_ENUM_9 = 9,		/**< DEMOSAIC_9 (Reserved) */
	FJ_DEMOSAIC_ENUM_MAX		/**< Stopper */
} FJ_DEMOSAIC_ENUM;

/** Stitch Enumelation
*/
typedef enum {
	FJ_STITCH_ENUM_0 = 0,		/**< STITCH_0 */
	FJ_STITCH_ENUM_1 = 1,		/**< STITCH_1 */
	FJ_STITCH_ENUM_2 = 2,		/**< STITCH_2 */
	FJ_STITCH_ENUM_MAX			/**< Stopper */
} FJ_STITCH_ENUM;

/** Image Macro Mode.<br>
	About more detaild information of mode, Please refer to @ref FJ_SetImageMacroMode() description. 
*/
typedef enum {
	FJ_STILL_IMAGE_MACRO_MODE_00	= 0,	/**< Mode 00	*/
	FJ_STILL_IMAGE_MACRO_MODE_01,			/**< Mode 01	*/
	FJ_STILL_IMAGE_MACRO_MODE_02,			/**< Mode 02	*/
	FJ_STILL_IMAGE_MACRO_MODE_03,			/**< Mode 03	*/
	FJ_STILL_IMAGE_MACRO_MODE_04,			/**< Mode 04	*/
	FJ_STILL_IMAGE_MACRO_MODE_05,			/**< Mode 05	*/
	FJ_STILL_IMAGE_MACRO_MODE_06,			/**< Mode 06	*/
	FJ_STILL_IMAGE_MACRO_MODE_07,			/**< Mode 07	*/
	FJ_STILL_IMAGE_MACRO_MODE_08,			/**< Mode 08	*/
	FJ_STILL_IMAGE_MACRO_MODE_09,			/**< Mode 09	*/
	FJ_STILL_IMAGE_MACRO_MODE_0A,			/**< Mode 0A	*/
	FJ_STILL_IMAGE_MACRO_MODE_0B,			/**< Mode 0B	*/
	FJ_STILL_IMAGE_MACRO_MODE_0C,			/**< Mode 0C	*/
	FJ_STILL_IMAGE_MACRO_MODE_0D,			/**< Mode 0D	*/
	FJ_STILL_IMAGE_MACRO_MODE_0E,			/**< Mode 0E	*/
	FJ_STILL_IMAGE_MACRO_MODE_0F,			/**< Mode 0F	*/
	FJ_STILL_IMAGE_MACRO_MODE_NONE,			/**< None	*/
	FJ_STILL_IMAGE_MACRO_MODE_MAX,			/**< Maximum value of FJ_STILL_IMAGE_MACRO_MODE		*/
} FJ_STILL_IMAGE_MACRO_MODE;

/** SEN input format.
*/
typedef enum {
	FJ_STILL_SEN_INPUT_FORMAT_INTERLACE	= 0,	/**< Interlace		*/
	FJ_STILL_SEN_INPUT_FORMAT_PROGRESSIVE,		/**< Progressive	*/
	FJ_STILL_SEN_INPUT_FORMAT_MAX,				/**< Maximum value of FJ_STILL_SEN_INPUT_FORMAT	*/
} FJ_STILL_SEN_INPUT_FORMAT;

/** SEN bit width.
*/
typedef enum {
	FJ_STILL_SEN_BIT_WIDTH_10	= 0,		/**< 10bit	*/
	FJ_STILL_SEN_BIT_WIDTH_12,				/**< 12bit	*/
	FJ_STILL_SEN_BIT_WIDTH_14,				/**< 14bit	*/
	FJ_STILL_SEN_BIT_WIDTH_16,				/**< 16bit	*/
	FJ_STILL_SEN_BIT_WIDTH_MAX				/**< Maximum value of FJ_STILL_SEN_BIT_WIDTH	*/
} FJ_STILL_SEN_BIT_WIDTH;

/** Sensor interface.
*/
typedef enum {
	FJ_STILL_SEN_SENSOR_INTERFACE_SUB_LVDS	= 0,	/**< Sub-LVDS		*/
	FJ_STILL_SEN_SENSOR_INTERFACE_SLVS_EC,			/**< SLVS-EC		*/
	FJ_STILL_SEN_SENSOR_INTERFACE_MIPI_D,			/**< MIPI D-PHY		*/
	FJ_STILL_SEN_SENSOR_INTERFACE_MIPI_C,			/**< MIPI C-PHY		*/
} FJ_STILL_SEN_SENSOR_INTERFACE;

/** Sensor Low-Power control select.
*/
typedef enum {
	FJ_STILL_SEN_SENSOR_LP_DIS = 0,		/**< Sensor Low-power(Intermittent) control disabled.	*/
	FJ_STILL_SEN_SENSOR_LP_EN,			/**< Sensor Low-power(Intermittent) control enabled.	*/
} FJ_STILL_SEN_SENSOR_LP;

#if 0	// not use?
/** Sensor Low-Power Control.
*/
typedef enum {
	FJ_STILL_SEN_SENSOR_LP_CTRL_DIS = 0,	/**< Sensor Low-power(Intermittent) control disabled.	*/
	FJ_STILL_SEN_SENSOR_LP_CTRL_EN,			/**< Sensor Low-power(Intermittent) control enabled.	*/
} FJ_STILL_SEN_SENSOR_LP_CTRL;
#endif

/** Initiate cycle(Demosaic output).
*/
typedef enum {
	FJ_DEMOSAIC_OUTPUT_DISABLE	= 0,			/**< Disable			*/
	FJ_DEMOSAIC_OUTPUT_MONO_SHOT,				/**< Mono-Shot			*/
	FJ_DEMOSAIC_OUTPUT_CYCLE_SHOT,				/**< Cycle-Shot(1/n)	*/
	FJ_DEMOSAIC_OUTPUT_MAX,						/**< Maximum value of FJ_DEMOSAIC_OUTPUT	*/
} FJ_DEMOSAIC_OUTPUT;

/** Demosaic output image format */
typedef enum {
	FJ_DEMOSAIC_OUTPUT_IMG_PFMT_VIDEO = 0,	/**< Video format for Video Record(Encoder input) image generation.		*/
	FJ_DEMOSAIC_OUTPUT_IMG_PFMT_YCC422,		/**< YCC422 format for Still image generation and other usecases.		*/
	FJ_DEMOSAIC_OUTPUT_IMG_PFMT_YCC420,		/**< YCC420	format for Display image generation and other usecases.		*/
	FJ_DEMOSAIC_OUTPUT_IMG_PFMT_MAX,		/**< Maximum value of FJ_DEMOSAIC_OUTPUT_IMG_PFMT.						*/
} FJ_DEMOSAIC_OUTPUT_IMG_PFMT;

/** 3DNR mode that input size indicate automatic setting or manually setting.
*/
typedef enum {
	FJ_STILL_3DNR_MODE_AUTO	= 0,		/**< To automatic setting of the input size of 3DNR by the BaseFW.	*/
	FJ_STILL_3DNR_MODE_MANUAL,			/**< To manual setting of the input size of 3DNR by the FJ_STILL_3DNR_CONFIG parameters.	*/
	FJ_STILL_3DNR_MODE_MAX,				/**< Maximum value of FJ_STILL_3DNR_MODE	*/
} FJ_STILL_3DNR_MODE;

/** image fill type.
*/
typedef enum {
	FJ_IMG_FILL_TYPE_Y	= 0,	/**<  8bit/pixel (Y data)			*/
	FJ_IMG_FILL_TYPE_CbCr,		/**< 16bit/Pixel (Cb/Cr data)		*/
	FJ_IMG_FILL_TYPE_RGB,		/**< 24bit/Pixel (RGB data)			*/
	FJ_IMG_FILL_TYPE_RGBA8888,	/**< 32bit/Pixel (RGBA8888 data)	*/
	FJ_IMG_FILL_TYPE_RGBA4444,	/**< 16bit/Pixel (RGBA4444 data)	*/
	FJ_IMG_FILL_TYPE_MAX
} FJ_IMG_FILL_TYPE;

/** image fill data.
*/
typedef struct {
	UCHAR	fill_0;			/**< Fill data for Y/Cb/R	*/
	UCHAR	fill_1;			/**< Fill data for Cr/G		*/
	UCHAR	fill_2;			/**< Fill data for B		*/
	UCHAR	fill_3;			/**< Fill data for A		*/
} FJ_IMG_FILL_DATA;

/** LVDS channel number.
*/
typedef enum {
	FJ_STILL_LVDS_CH0 = 0,		/**< LVDS ch0	*/
	FJ_STILL_LVDS_CH1,			/**< LVDS ch1	*/
	FJ_STILL_LVDS_CH2,			/**< LVDS ch1	*/
	FJ_STILL_LVDS_CH3,			/**< LVDS ch1	*/
} FJ_STILL_LVDS_CH;

/** MIPI-DPHY channel number.
*/
typedef enum {
	FJ_STILL_DMIPI_CH0 = 0,		/**< MIPI-DPHY0 */
	FJ_STILL_DMIPI_CH1,			/**< MIPI-DPHY1 */
	FJ_STILL_DMIPI_CH2,			/**< MIPI-DPHY2 */
	FJ_STILL_DMIPI_CH3,			/**< MIPI-DPHY3 */
} FJ_STILL_DMIPI_CH;

/** Number of input lanes selection.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_LANE_1 = 0,	/**< 1-lane input */
	FJ_PRO_CTRL_DMIPI_LANE_2,		/**< 2-lane input */
	FJ_PRO_CTRL_DMIPI_LANE_4 = 3,	/**< 4-lane input */
} FJ_PRO_CTRL_DMIPI_LANE;

/** select the Virtual Channel value for the packets output to the OUT0/1 side.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_VC_0 = 0,	/**< Virtual Channel0 */
	FJ_PRO_CTRL_DMIPI_VC_1,		/**< Virtual Channel1 */
	FJ_PRO_CTRL_DMIPI_VC_2,		/**< Virtual Channel2 */
	FJ_PRO_CTRL_DMIPI_VC_3,		/**< Virtual Channel3 */
} FJ_PRO_CTRL_DMIPI_VC;

/** select the input data mode.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_IN_DATA_MODE_8BIT = 0,				/**< RAW 8-bit mode */
	FJ_PRO_CTRL_DMIPI_IN_DATA_MODE_10BIT,					/**< RAW 10-bit mode */
	FJ_PRO_CTRL_DMIPI_IN_DATA_MODE_12BIT,					/**< RAW 12-bit mode */
	FJ_PRO_CTRL_DMIPI_IN_DATA_MODE_14BIT,					/**< RAW 14-bit mode */
	FJ_PRO_CTRL_DMIPI_IN_DATA_MODE_LEGACY_YUV420,			/**< Legacy YUV420 8-bit  */
	FJ_PRO_CTRL_DMIPI_IN_DATA_MODE_YUV422,					/**< YUV422 8-bit */
	FJ_PRO_CTRL_DMIPI_IN_DATA_MODE_USER_DEF,				/**< User Defined/Embedded Data */
} FJ_PRO_CTRL_DMIPI_IN_DATA_MODE;

/** Output data mode selection.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_OUT_MODE_NO_OUTPUT = 0,				/**< No output */
	FJ_PRO_CTRL_DMIPI_OUT_MODE_1LINE = 2,					/**< 1-line output mode */
	FJ_PRO_CTRL_DMIPI_OUT_MODE_2POINT,						/**< 2-point output mode */
	FJ_PRO_CTRL_DMIPI_OUT_MODE_4POINT,						/**< 4-point output mode */
	FJ_PRO_CTRL_DMIPI_OUT_MODE_8POINT,						/**< 8-point output mode */
} FJ_PRO_CTRL_DMIPI_OUT_MODE;

/** select the DataType mode for the packets output.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_DT_OUT_DIS = 0,						/**< DT selection mode is off */
	FJ_PRO_CTRL_DMIPI_DT_OUT_EN,							/**< DT selection mode is on */
} FJ_PRO_CTRL_DMIPI_DT_OUT;

/** RxSyncMode value setting.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_RXSYNCMODE_PERFECTLY_MATCHED = 0,	/**< Only perfectly-matched sync code is allowed. */
	FJ_PRO_CTRL_DMIPI_RXSYNCMODE_1BIT_DISCREPANCY,			/**< One bit of discrepancy between sync codes is allowed. */
} FJ_PRO_CTRL_DMIPI_RXSYNCMODE;

/** RxEnable value setting.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_RXENABLE_ALL_OUT0 = 0,				/**< All RxEnable0~3 signals output "0". */
	FJ_PRO_CTRL_DMIPI_RXENABLE_OUT1,						/**< The corresponding RxEnable signal outputs "1". */
} FJ_PRO_CTRL_DMIPI_RXENABLE;

/**  select the cycle between horizontal detect pulse and output pixel data.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_SEL_CYCLE_0 = 0,						/**< The cycle between HDP and pixel data is 0 cycle. */
	FJ_PRO_CTRL_DMIPI_SEL_CYCLE_2,							/**< The cycle between HDP and pixel data is 2 cycle. */
} FJ_PRO_CTRL_DMIPI_SEL_CYCLE;

/** Line division processing multi-CH combination mode setting.<br>
	set the multi-CH combination mode of the line division processing.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_MULTI_CH_NO_COMBI = 0,		/**< No combination	*/
	FJ_PRO_CTRL_DMIPI_MULTI_CH_2_COMBI,				/**< 2 combination	*/
	FJ_PRO_CTRL_DMIPI_MULTI_CH_4_COMBI,				/**< 4 combination	*/
} FJ_PRO_CTRL_DMIPI_MULTI_CH;

/** MIPI-CPHY channel number.
*/
typedef enum {
	FJ_STILL_CMIPI_CH0 = 0,			/**< MIPI-CPHY0 */
	FJ_STILL_CMIPI_CH1,				/**< MIPI-CPHY1 */
	FJ_STILL_CMIPI_CH2,				/**< MIPI-CPHY2 */
	FJ_STILL_CMIPI_CH3,				/**< MIPI-CPHY3 */
} FJ_STILL_CMIPI_CH;

/** Number of input lanes selection.
*/
typedef enum {
	FJ_PRO_CTRL_CMIPI_LANE_1 = 0,	/**< 1-lane input */
	FJ_PRO_CTRL_CMIPI_LANE_2,		/**< 2-lane input */
	FJ_PRO_CTRL_CMIPI_LANE_4 = 3,	/**< 4-lane input */
} FJ_PRO_CTRL_CMIPI_LANE;

/** select the Virtual Channel value for the packets output to the OUT0/1 side.
*/
typedef enum {
	FJ_PRO_CTRL_CMIPI_VC_0 = 0,		/**< Virtual Channel0 */
	FJ_PRO_CTRL_CMIPI_VC_1,			/**< Virtual Channel1 */
	FJ_PRO_CTRL_CMIPI_VC_2,			/**< Virtual Channel2 */
	FJ_PRO_CTRL_CMIPI_VC_3,			/**< Virtual Channel3 */
	FJ_PRO_CTRL_CMIPI_VC_INVALID,	/**< Virtual Channel invalid ( All output ) */
} FJ_PRO_CTRL_CMIPI_VC;

/** select the input data mode.
*/
typedef enum {
	FJ_PRO_CTRL_CMIPI_IN_DATA_MODE_8BIT = 0,				/**< RAW 8-bit mode */
	FJ_PRO_CTRL_CMIPI_IN_DATA_MODE_10BIT,					/**< RAW 10-bit mode */
	FJ_PRO_CTRL_CMIPI_IN_DATA_MODE_12BIT,					/**< RAW 12-bit mode */
	FJ_PRO_CTRL_CMIPI_IN_DATA_MODE_14BIT,					/**< RAW 14-bit mode */
	FJ_PRO_CTRL_CMIPI_IN_DATA_MODE_LEGACY_YUV420,			/**< Legacy YUV420 8-bit  */
	FJ_PRO_CTRL_CMIPI_IN_DATA_MODE_YUV422,					/**< YUV422 8-bit */
	FJ_PRO_CTRL_CMIPI_IN_DATA_MODE_USER_DEF,				/**< User Defined/Embedded Data */
} FJ_PRO_CTRL_CMIPI_IN_DATA_MODE;

/** Output data mode selection.
*/
typedef enum {
	FJ_PRO_CTRL_CMIPI_OUT_MODE_NO_OUTPUT = 0,				/**< No output */
	FJ_PRO_CTRL_CMIPI_OUT_MODE_1LINE = 2,					/**< 1-line output mode */
	FJ_PRO_CTRL_CMIPI_OUT_MODE_2POINT,						/**< 2-point output mode */
	FJ_PRO_CTRL_CMIPI_OUT_MODE_4POINT,						/**< 4-point output mode */
	FJ_PRO_CTRL_CMIPI_OUT_MODE_8POINT,						/**< 8-point output mode */
} FJ_PRO_CTRL_CMIPI_OUT_MODE;

/** select the DataType mode for the packets output.
*/
typedef enum {
	FJ_PRO_CTRL_CMIPI_DT_OUT_DIS = 0,						/**< DT selection mode is off */
	FJ_PRO_CTRL_CMIPI_DT_OUT_EN,							/**< DT selection mode is on */
} FJ_PRO_CTRL_CMIPI_DT_OUT;


/**  select the cycle between horizontal detect pulse and output pixel data.
*/
typedef enum {
	FJ_PRO_CTRL_CMIPI_SEL_CYCLE_0 = 0,						/**< The cycle between HDP and pixel data is 0 cycle. */
	FJ_PRO_CTRL_CMIPI_SEL_CYCLE_2,							/**< The cycle between HDP and pixel data is 2 cycle. */
} FJ_PRO_CTRL_CMIPI_SEL_CYCLE;

/** RxEnable value setting.
*/
typedef enum {
	FJ_PRO_CTRL_CMIPI_RXENABLE_ALL_OUT0 = 0,				/**< All RxEnable0~3 signals output "0". */
	FJ_PRO_CTRL_CMIPI_RXENABLE_OUT1,						/**< The corresponding RxEnable signal outputs "1". */
} FJ_PRO_CTRL_CMIPI_RXENABLE;

/** Line division processing multi-CH combination mode setting.<br>
	set the multi-CH combination mode of the line division processing.
*/
typedef enum {
	FJ_PRO_CTRL_CMIPI_MULTI_CH_NO_COMBI = 0,		/**< No combination	*/
	FJ_PRO_CTRL_CMIPI_MULTI_CH_2_COMBI,				/**< 2 combination	*/
	FJ_PRO_CTRL_CMIPI_MULTI_CH_4_COMBI,				/**< 4 combination	*/
} FJ_PRO_CTRL_CMIPI_MULTI_CH;

/** RxEnable value setting.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_RXMODE_UNDER_1P5_GBPS = 0,			/**< D-PHY under 1.5Gpbs. */
	FJ_PRO_CTRL_DMIPI_RXMODE_2P5_GBPS_W_DESKEW,				/**< D-PHY 1.5Gbps to 2.5Gbps with deskew. */
} FJ_PRO_CTRL_DMIPI_RXMODE;

/** RxSpeed value setting.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_RXSPEED_NOT_USED 			= 0,		/**< D-PHY Delay Line Mode Select signal belong Data Rate:delay Line not used */
	FJ_PRO_CTRL_DMIPI_RXSPEED_1P5_TO_1P8_GBPS	= 4,		/**< D-PHY Delay Line Mode Select signal belong Data Rate:1.5Gbps to 1.8Gbps */
	FJ_PRO_CTRL_DMIPI_RXSPEED_1P8_TO_2P1_GBPS	= 5,		/**< D-PHY Delay Line Mode Select signal belong Data Rate:1.8Gbps to 2.1Gbps */
	FJ_PRO_CTRL_DMIPI_RXSPEED_2P1_TO_2P4_GBPS	= 6,		/**< D-PHY Delay Line Mode Select signal belong Data Rate:2.1Gbps to 2.4Gbps */
	FJ_PRO_CTRL_DMIPI_RXSPEED_2P4_TO_2P5_GBPS	= 7,		/**< D-PHY Delay Line Mode Select signal belong Data Rate:2.4Gbps to 2.5Gbps */
} FJ_PRO_CTRL_DMIPI_RXSPEED;

/** Periodic De-skew code search width setting signals.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_DESKEW_CODE_SEARCH_WIDTH_0CODE = 0,	/**< current code only		*/
	FJ_PRO_CTRL_DMIPI_DESKEW_CODE_SEARCH_WIDTH_1CODE,		/**< current code +/- 1code	*/
	FJ_PRO_CTRL_DMIPI_DESKEW_CODE_SEARCH_WIDTH_2CODE,		/**< current code +/- 2code	*/
	FJ_PRO_CTRL_DMIPI_DESKEW_CODE_SEARCH_WIDTH_3CODE,		/**< current code +/- 3code	*/
	FJ_PRO_CTRL_DMIPI_DESKEW_CODE_SEARCH_WIDTH_4CODE,		/**< current code +/- 4code	*/
	FJ_PRO_CTRL_DMIPI_DESKEW_CODE_SEARCH_WIDTH_5CODE,		/**< current code +/- 5code	*/
	FJ_PRO_CTRL_DMIPI_DESKEW_CODE_SEARCH_WIDTH_6CODE,		/**< current code +/- 6code	*/
	FJ_PRO_CTRL_DMIPI_DESKEW_CODE_SEARCH_WIDTH_7CODE,		/**< current code +/- 7code	*/
} FJ_PRO_CTRL_DMIPI_DESKEW_CODE_SEARCH_WIDTH;

/** Minimum valid width setting signals.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_MIN_VALID_WIDTH_1CODE = 0,			/**< 1 code in valid window	*/
	FJ_PRO_CTRL_DMIPI_MIN_VALID_WIDTH_2CODE,				/**< 2 code in valid window	*/
	FJ_PRO_CTRL_DMIPI_MIN_VALID_WIDTH_3CODE,				/**< 3 code in valid window	*/
	FJ_PRO_CTRL_DMIPI_MIN_VALID_WIDTH_4CODE,				/**< 4 code in valid window	*/
	FJ_PRO_CTRL_DMIPI_MIN_VALID_WIDTH_5CODE,				/**< 5 code in valid window	*/
	FJ_PRO_CTRL_DMIPI_MIN_VALID_WIDTH_6CODE,				/**< 6 code in valid window	*/
	FJ_PRO_CTRL_DMIPI_MIN_VALID_WIDTH_8CODE,				/**< 8 code in valid window	*/
} FJ_PRO_CTRL_DMIPI_MIN_VALID_WIDTH;

/** Sampling number setting of each DelayLine code in Deskew sequence.
*/
typedef enum {
	FJ_PRO_CTRL_DMIPI_SAMPLING_NUM_1BYTE = 0,				/**< 1byte data		*/
	FJ_PRO_CTRL_DMIPI_SAMPLING_NUM_2BYTE,					/**< 2byte data		*/
	FJ_PRO_CTRL_DMIPI_SAMPLING_NUM_4BYTE,					/**< 4byte data		*/
	FJ_PRO_CTRL_DMIPI_SAMPLING_NUM_16BYTE,					/**< 16byte data	*/
} FJ_PRO_CTRL_DMIPI_SAMPLING_NUM;

/** SLVS-EC stream type.
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_STREAM_TYPE_A = 0,		/**< Stream A		*/
	FJ_PRO_CTRL_SLVS_STREAM_TYPE_B,			/**< Stream B		*/
	FJ_PRO_CTRL_SLVS_STREAM_TYPE_BOTH,		/**< Stream A & B	*/
} FJ_PRO_CTRL_SLVS_STREAM_TYPE;

/** SLVS-EC Multiplication factor control.
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_MULTIPLICATION_FACTOR_REF_CLK_72MHZ = 0,	/**< Reference Clock 72MHz Setting for (Data Rate 2.304Gbps)	*/
	FJ_PRO_CTRL_SLVS_MULTIPLICATION_FACTOR_RESERVED				/**< Reserved	*/
} FJ_PRO_CTRL_SLVS_MULTIPLICATION_FACTOR;

/** SLVS input differential data value inversion setting.
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_NORMAL = 0,			/**< Normal	*/
	FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_INVERT				/**< Invert	*/
} FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION;

/** SLVS VDOUT control when frame stop is set
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_VDOUT_CTRL_NO_OUTPUT = 0,					/**< Next VDOUT after frame stop is set is not output.	*/
	FJ_PRO_CTRL_SLVS_VDOUT_CTRL_OUTPUT,							/**< Next VDOUT after frame stop is set is output.	*/
} FJ_PRO_CTRL_SLVS_VDOUT_CTRL;

/** SLVS ECC option / CRC Option of payload.
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_ECCCRC_OPT_WO_ECC_CRC = 0,					/**< Without ECC/CRC.	*/
	FJ_PRO_CTRL_SLVS_ECCCRC_OPT_W_CRC,							/**< With CRC.	*/
	FJ_PRO_CTRL_SLVS_ECCCRC_OPT_W_ECC_2BYTE_PARITY,				/**< With ECC(2byte parity).	*/
	FJ_PRO_CTRL_SLVS_ECCCRC_OPT_W_ECC_4BYTE_PARITY = 4,			/**< With ECC(4byte parity).	*/
	FJ_PRO_CTRL_SLVS_ECCCRC_OPT_MAX,							/**< ECC/CRC option select Max value	*/
} FJ_PRO_CTRL_SLVS_ECCCRC_OPT;

/** SLVS expected value when Line Number Error of the line next to the state the Line Number extracted from the packet header reaches the MAX value (13’h1FFF) is detected.
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_EXPECT_VAL_H_0000 = 0,						/**< expected value when Line Number Error occurred is 0x0000.	*/
	FJ_PRO_CTRL_SLVS_EXPECT_VAL_H_1FFF,							/**< expected value when Line Number Error occurred is 0x1FFF.	*/
} FJ_PRO_CTRL_SLVS_EXPECT_VAL;

/** SLVS symbol type
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_SYMBOL_TYPE_DATA = 0,			/**< code type is Data.	*/
	FJ_PRO_CTRL_SLVS_SYMBOL_TYPE_K_CODE,			/**< code type is K Code.	*/
} FJ_PRO_CTRL_SLVS_SYMBOL_TYPE;

/** SLVS number of bits per pixel
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_PIXEL_BIT_8BIT = 0,	/**< 8bit	*/
	FJ_PRO_CTRL_SLVS_PIXEL_BIT_10BIT,		/**< 10bit	*/
	FJ_PRO_CTRL_SLVS_PIXEL_BIT_12BIT,		/**< 12bit	*/
	FJ_PRO_CTRL_SLVS_PIXEL_BIT_14BIT,		/**< 14bit	*/
	FJ_PRO_CTRL_SLVS_PIXEL_BIT_16BIT,		/**< 16bit	*/
	FJ_PRO_CTRL_SLVS_PIXEL_BIT_MAX			/**< number of bits per pixel Max value	*/
} FJ_PRO_CTRL_SLVS_PIXEL_BIT;

/** SLVS number of valid Lanes
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_LANE_NUM_1LANE = 0,	/**< 1Lane	*/
	FJ_PRO_CTRL_SLVS_LANE_NUM_2LANE,		/**< 2Lane	*/
	FJ_PRO_CTRL_SLVS_LANE_NUM_4LANE,		/**< 4Lane	*/
	FJ_PRO_CTRL_SLVS_LANE_NUM_6LANE,		/**< 6Lane	*/
	FJ_PRO_CTRL_SLVS_LANE_NUM_8LANE,		/**< 8Lane	*/
	FJ_PRO_CTRL_SLVS_LANE_NUM_MAX			/**< number of valid Lanes Max value	*/
} FJ_PRO_CTRL_SLVS_LANE_NUM;

/** SLVS output format of the image data interface.
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_OUT_FORMAT_1POINT = 0,	/**< 1-point output	*/
	FJ_PRO_CTRL_SLVS_OUT_FORMAT_2POINT,		/**< 2-point output	*/
	FJ_PRO_CTRL_SLVS_OUT_FORMAT_4POINT,		/**< 4-point output	*/
	FJ_PRO_CTRL_SLVS_OUT_FORMAT_8POINT,		/**< 8-point output	*/
	FJ_PRO_CTRL_SLVS_OUT_FORMAT_MAX			/**< output format Max value	*/
} FJ_PRO_CTRL_SLVS_OUT_FORMAT;

/** SLVS input port select for Lane swap.
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL_0 = 0,	/**< L0IN	*/
	FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL_1,		/**< L1IN	*/
	FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL_2,		/**< L2IN	*/
	FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL_3,		/**< L3IN	*/
	FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL_4,		/**< L4IN	*/
	FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL_5,		/**< L5IN	*/
	FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL_6,		/**< L6IN	*/
	FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL_7,		/**< L7IN	*/
	FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL_MAX		/**< input port select Max value	*/
} FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL;

/** SLVS Lane Enable/Disable
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_LANE_ENABLE_DIS = 0,	/**< Disable	*/
	FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN			/**< Enable		*/
} FJ_PRO_CTRL_SLVS_LANE_ENABLE;

/** SLVS trimming Enable/Disable
*/
typedef enum {
	FJ_PRO_CTRL_SLVS_TRIM_DIS = 0,			/**< Disable	*/
	FJ_PRO_CTRL_SLVS_TRIM_EN				/**< Enable		*/
} FJ_PRO_CTRL_SLVS_TRIM_ENABLE;

/** RGB/Y table type(for Histogram before TC).
*/
typedef enum {
	E_FJ_STILL_BTC_HIST_TBL_R = 0,		/**< R component data table. */
	E_FJ_STILL_BTC_HIST_TBL_G,			/**< G component data table. */
	E_FJ_STILL_BTC_HIST_TBL_B,			/**< B component data table. */
	E_FJ_STILL_BTC_HIST_TBL_Y,			/**< Y component data table. */
	E_FJ_STILL_BTC_HIST_TBL_MAX			/**< Maximum value. */
} E_FJ_STILL_BTC_HIST_TBL;

/** TCHS horizontal sampling interval.
*/
typedef enum {
	E_FJ_STILL_TCHS_HIST_HCYC_4 = 0,		/**< 4pixel			*/
	E_FJ_STILL_TCHS_HIST_HCYC_8,			/**< 8pixel			*/
	E_FJ_STILL_TCHS_HIST_HCYC_16,			/**< 16pixel		*/
	E_FJ_STILL_TCHS_HIST_HCYC_MAX			/**< Maximum value.	*/
} E_FJ_STILL_TCHS_HIST_HCYC;

/** TCHS vertical sampling interval.
*/
typedef enum {
	E_FJ_STILL_TCHS_HIST_VCYC_1 = 0,		/**< 1pixel			*/
	E_FJ_STILL_TCHS_HIST_VCYC_2,			/**< 2pixel			*/
	E_FJ_STILL_TCHS_HIST_VCYC_4,			/**< 4pixel			*/
	E_FJ_STILL_TCHS_HIST_VCYC_8,			/**< 8pixel			*/
	E_FJ_STILL_TCHS_HIST_VCYC_16,			/**< 16pixel		*/
	E_FJ_STILL_TCHS_HIST_VCYC_MAX			/**< Maximum value.	*/
} E_FJ_STILL_TCHS_HIST_VCYC;

/** TCHS RGB histogram mode.
*/
typedef enum {
	E_FJ_STILL_TCHS_HIST_BIT_WIDTH_7_0 = 0,		/**< Detection bit range[7:0]	Detection maximum value:0xFF	*/
	E_FJ_STILL_TCHS_HIST_BIT_WIDTH_8_1,			/**< Detection bit range[8:1]	Detection maximum value:0x1FF	*/
	E_FJ_STILL_TCHS_HIST_BIT_WIDTH_9_2,			/**< Detection bit range[9:2]	Detection maximum value:0x3FF	*/
	E_FJ_STILL_TCHS_HIST_BIT_WIDTH_10_3,		/**< Detection bit range[10:3]	Detection maximum value:0x7FF	*/
	E_FJ_STILL_TCHS_HIST_BIT_WIDTH_11_4,		/**< Detection bit range[11:4]	Detection maximum value:0xFFF	*/
	E_FJ_STILL_TCHS_HIST_BIT_WIDTH_MAX			/**< Maximum value.	*/
} E_FJ_STILL_TCHS_HIST_BIT_WIDTH;

/** TC table reference resolution.
*/
typedef enum {
	E_FJ_STILL_TC_REF_RESOLUTION_1024 = 0,		/**< 1024 gradations / 1plane			*/
	E_FJ_STILL_TC_REF_RESOLUTION_512,			/**<  512 gradations / 2planes			*/
	E_FJ_STILL_TC_REF_RESOLUTION_256,			/**<  256 gradations / 4planes			*/
	E_FJ_STILL_TC_REF_RESOLUTION_MAX			/**< Maximum value.	*/
} E_FJ_STILL_TC_REF_RESOLUTION;

/** TC YTc output selection.
*/
typedef enum {
	E_FJ_STILL_YTC_OUT_Y = 0,		/**< Y  is a luminance value calculated from RGB, which has not been input to TC block, and coefficient "TCYC"	*/
	E_FJ_STILL_YTC_OUT_YB,			/**< Yb is a luminance value calculated from RGB, which has not been input to color correction 0 block, and coefficient "CCYC"	*/
	E_FJ_STILL_YTC_OUT_MAX			/**< Maximum value.	*/
} E_FJ_STILL_YTC_OUT;

/** TC data integral part selection.
*/
typedef enum {
	E_FJ_STILL_TC_INTBIT_15_14 = 0,		/**< XX.YYYYY_YYYY_YYYY(X:Integer part / Y:Decimal part)	*/
	E_FJ_STILL_TC_INTBIT_15_13,			/**< XXX.YYYY_YYYY_YYYY(X:Integer part / Y:Decimal part)	*/
	E_FJ_STILL_TC_INTBIT_15_12,			/**< XXXX.YYY_YYYY_YYYY(X:Integer part / Y:Decimal part)	*/
	E_FJ_STILL_TC_INTBIT_15_11,			/**< XXXXX.YY_YYYY_YYYY(X:Integer part / Y:Decimal part)	*/
	E_FJ_STILL_TC_INTBIT_MAX			/**< Maximum value.	*/
} E_FJ_STILL_TC_INTBIT;

/** Adaptive Auto Exposure Mode
*/
typedef enum {
	FJ_STILL_ADAPTIVE_AE_MODE_AUTO = 0,			/**< Adaptive Auto Exposure Auto	*/
	FJ_STILL_ADAPTIVE_AE_MODE_LOCK,				/**< Adaptive Auto Exposure Lock	*/
	FJ_STILL_ADAPTIVE_AE_MODE_DISABLE,			/**< Adaptive Auto Exposure Disable	*/
	FJ_STILL_ADAPTIVE_AE_MODE_MAX,			/**< Adaptive Auto Exposure Disable	*/
} FJ_STILL_ADAPTIVE_AE_MODE;

/** Raw scaler selection.
*/
typedef enum {
	FJ_STILL_RAW_SCALER_SEL_DIS = 0,	/**< Raw scaler disabled	*/
	FJ_STILL_RAW_SCALER_SEL_SNI,		/**< SNI Raw scaler enabled	*/
	FJ_STILL_RAW_SCALER_SEL_EXTERNAL,	/**< External Raw scaler enabled */
} FJ_STILL_RAW_SCALER_SEL;

/** Stitch mode selection.
*/
typedef enum {
	FJ_STILL_STITCH_TYPE_VIDEO = 0,
	FJ_STILL_STITCH_TYPE_PHOTO,
	FJ_STILL_STITCH_TYPE_AEB,
	FJ_STILL_STITCH_TYPE_CALIBTION,
// --- REMOVE_AHA BEGIN ---
	FJ_STILL_STITCH_TYPE_VIDEO_AHA,
// --- REMOVE_AHA END ---
	FJ_STILL_STITCH_TYPE_MAX,
} FJ_STILL_STITCH_TYPE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/** YCC address */
typedef struct {
	UINT32					AddressY;							/**< Y Addr		*/
	UINT32					AddressCb;							/**< Cb Addr	*/
	UINT32					AddressCr;							/**< Cr Addr	*/
} FJ_T_ADDR_YCC;

/** RGB address */
typedef struct {
	UINT32					AddressR;							/**< R Addr		*/
	UINT32					AddressG;							/**< G Addr		*/
	UINT32					AddressB;							/**< B Addr		*/
} FJ_T_ADDR_RGB;

/** YCC information */
typedef struct {
	FJ_T_ADDR_YCC		addr;				/**< address				*/
	USHORT				g_y_width;			/**< horizontal global size	*/
	USHORT				g_c_width;			/**< horizontal global size	*/
	USHORT				width;				/**< horizontal size		*/
	USHORT				lines;				/**< vertical size			*/
	FJ_IMG_PFMT			pfmt;				/**< pixel format			*/
} FJ_IMG_INFO;

/** RGB information */
typedef struct {
	FJ_IMG_DEPTH		depth;				/**< depth					*/
	FJ_T_ADDR_RGB		addr;				/**< address				*/
	USHORT				g_width;			/**< horizontal global size	*/
	USHORT				width;				/**< horizontal size		*/
	USHORT				lines;				/**< vertical size			*/
} FJ_IMG_INFO_RGB;

/** Image address */
typedef struct {
	UINT32				Y_G;				/**< Beginning address of Global Y(G) image.
												 Should be aligned on 8.						*/
	UINT32				Cb_B;				/**< Beginning address of Global Cb(B) image.
												 Should be aligned on 8.						*/
	UINT32				Cr_R;				/**< Beginning address of Global Cr(R) image.
												 Should be aligned on 8.						*/
	UINT32				Alpha;				/**< Beginning address of Global Alpha image.
												 Should be aligned on 8.						*/
} FJ_T_ADDR_IMG;

/** The size of byte of global image 1 line */
typedef struct {
	INT32				Y_G;				/**< The size of byte of Global Y(G) image 1 line.
												 Should be aligned on 8.						*/
	INT32				Cb_B;				/**< The size of byte of Global Cb(B) image 1 line.
												 Should be aligned on 8.						*/
	INT32				Cr_R;				/**< The size of byte of Global Cr(R) image 1 line.
												 Should be aligned on 8.						*/
	INT32				Alpha;				/**< The size of byte of Global Alpha image 1 line.
												 Should be aligned on 8.						*/
} FJ_T_LINE_BYTES;

/** Image convert information. */
typedef struct {
	FJ_T_ADDR_IMG		addr;				/**< address.										*/
	USHORT				g_y_g_width;		/**< The number of horizontal global pixel Y(G).	*/
	USHORT				g_cb_b_width;		/**< The number of horizontal global pixel Cb(B).	*/
	USHORT				g_cr_r_width;		/**< The number of horizontal global pixel Cr(R).	*/
	USHORT				g_alpha_width;		/**< The number of horizontal global pixel Alpha.	*/
	USHORT				width;				/**< The number of horizontal pixel.				*/
	USHORT				lines;				/**< The number of vertical pixel. 					*/
	FJ_T_LINE_BYTES		line_bytes;			/**< The size of byte of global image 1 line.		*/
	FJ_IMG_PFMT			pfmt;				/**< pixel format.									*/
} FJ_IMG_INFO_CONV;

/** YUV buffer information */
typedef struct {
	UINT32	uiRecordSizeX;					/**< Number of pixels per image line for Record.	*/
	UINT32	uiRecordSizeY;					/**< Number of image lines for Record.				*/
	UINT32	uiSizeX;						/**< Number of pixels per image line.				*/
	UINT32	uiSizeY;						/**< Number of image lines.							*/
	UINT32	uiGlobalSizeX;					/**< Number of pixels per image line.				*/
	CHAR*	pbyBuffer_Y;					/**< Address of the YUV data buffer.				*/
	CHAR*	pbyBuffer_Cb;					/**< Address of the YUV data buffer.				*/
	CHAR*	pbyBuffer_Cr;					/**< Address of the YUV data buffer.				*/
	UCHAR	Direction;						/**< Direction of image.							*/
} SPANOINFO;
typedef SPANOINFO* PSPANOINFO;

/** A structure containing information about the jpeg stream and YUV 4:2:2 data frame */
typedef struct {
	UCHAR*		pbyYuvBuffer;				/**< YUV buffer address				*/
	UINT32		uiYuvLineJumpInBytes;		/**< YUV global horizontal size		*/
	UINT32		uiYuvSizeX;					/**< YUV width						*/
	UINT32		uiYuvSizeY;					/**< YUV height						*/
	UCHAR*		pbyJpg;						/**< JPEG Main buffer address		*/
	UCHAR*		pbyJpgScreen;				/**< JPEG Screennail buffer address	*/
	UCHAR*		pbyJpgThumb;				/**< JPEG Thumbnail buffer address	*/
	UINT32		uiMaxJpgSize;				/**< JPEG max size					*/
	UINT32*		puiActJpgSize;				/**< JPEG real size					*/
	UINT32*		puiActJpgScreenSize;		/**< JPEG Screennail real size		*/
	UINT32*		puiActJpgThumbSize;			/**< JPEG Thumbnail real size		*/
	UINT32		uiJpgIdx;					/**< JPEG index						*/
	BOOL		blLastFrameFlag;			/**< Last frame flag				*/
} JPGINFO;
typedef JPGINFO* PSJPGINFO;

/** Image crop parameter
*/
typedef struct {
	DOUBLE	left;							/**< Start position of X.	*/
	DOUBLE	top;							/**< Start position of Y.	*/
	DOUBLE	width;							/**< Crop width.			*/
	DOUBLE	lines;							/**< Crop lines.			*/
} FJ_IMG_CROP_RECT;

/** YUV buffer information */
typedef struct {
	UINT32		uiSizeX;						/**< Number of pixels per image line.															*/
	UINT32		uiSizeY;						/**< Number of image lines.																		*/
	UINT32		uiLineJumpInBytes;				/**< Number of bytes to jump from beginning of line N to beginning of line N+1					*/
	CHAR*		pbyBuffer;						/**< Address of the YCC data buffer.															*/
	CHAR*		pbyBufferY;						/**< Address of the Y data buffer.	(The editable by Host)										*/
	CHAR*		pbyBufferCb;					/**< Address of the Cb data buffer.	(The editable by Host)										*/
	CHAR*		pbyBufferCr;					/**< Address of the Cr data buffer.	(The editable by Host)										*/
	PBYTE		pbyOptionalBuffer;				/**< OPTIONAL BUFFER INFO																		*/
	UINT32		uiOptionalSizeX;				/**< Number of pixels per image line in Optional buffer.										*/
	UINT32		uiOptionalSizeY;				/**< Number of image lines in Optional buffer.													*/
	ULLONG 		ulFrameNo;						/**< Corresponding number of readout frame.														*/
	USHORT		usStreamID;						/**< Stream ID.																					*/
	USHORT		usStreamType;					/**< Stream Type. @ref FJ_STREAM_TYPE_H264 or @ref FJ_STREAM_TYPE_YUV.							*/
	FJ_IMG_PFMT	tPfmt;							/**< YCC Image format.																			*/
	ULONG*		pHistogramBuffer;				/**< The pointer of Histogram data.																*/
	UINT32		uiEditHost;						/**< Edited Flag of by the Host.<br>
												 If you edit pbyBufferY, pbyBufferCb, pbyBufferCr in the Host process, you please set 1.	*/
} SYUVINFO;
typedef SYUVINFO* PSYUVINFO;

/** RGB buffer information */
typedef struct {
	UINT32		uiSizeX;						/**< Number of pixels per image line.															*/
	UINT32		uiSizeY;						/**< Number of image lines.																		*/
	UINT32		uiLineJumpInBytes;				/**< Number of bytes to jump from beginning of line N to beginning of line N+1					*/
	CHAR*		pbyBuffer;						/**< Address of the RGB data buffer.															*/
	CHAR*		pbyBufferG;						/**< Address of the G data buffer.		(The editable by Host)										*/
	CHAR*		pbyBufferB;						/**< Address of the B data buffer.		(The editable by Host)										*/
	CHAR*		pbyBufferR;						/**< Address of the R data buffer.		(The editable by Host)										*/
	CHAR*		pbyBufferA;						/**< Address of the Alpha data buffer.	(The editable by Host)									*/
	PBYTE		pbyOptionalBuffer;				/**< OPTIONAL BUFFER INFO																		*/
	UINT32		uiOptionalSizeX;				/**< Number of pixels per image line in Optional buffer.										*/
	UINT32		uiOptionalSizeY;				/**< Number of image lines in Optional buffer.													*/
	FJ_IMG_PFMT	tPfmt;							/**< Image format.																			*/
	ULONG*		pHistogramBuffer;				/**< The pointer of Histogram data.																*/
	UINT32		uiEditHost;						/**< Edited Flag of by the Host.<br>
												 If you edit pbyBufferG, pbyBufferB, pbyBufferR, pbyBufferA in the Host process, you please set 1.	*/
} SRGBINFO;
typedef SRGBINFO* PSRGBINFO;

/** Stitch data information */
typedef struct {
	FJ_T_ADDR_YCC			top_addr;				/**< Top address.																			*/
	UINT32					top_SizeX;				/**< Top number of pixels per image line.													*/
	UINT32					top_SizeY;				/**< Top number of image lines.																*/
	UINT32					top_LineJumpInBytes;	/**< Top number of bytes to jump from beginning of line N to beginning of line N+1.			*/
	FJ_T_ADDR_YCC			left_addr;				/**< Left address of left data include overwrap area.										*/
	UINT32					left_SizeX;				/**< Left number of pixels per image line include overwrap area.							*/
	UINT32					left_SizeY;				/**< Left number of image lines.															*/
	UINT32					left_LineJumpInBytes;	/**< Left number of bytes to jump from beginning of line N to beginning of line N+1.		*/
	FJ_T_ADDR_YCC			right_addr;				/**< Right address of right data include overwrap area.										*/
	UINT32					right_SizeX;			/**< Right number of pixels per image line include overwrap area.							*/
	UINT32					right_SizeY;			/**< Right number of image lines.(source right)												*/
	UINT32					right_LineJumpInBytes;	/**< Right number of bytes to jump from beginning of line N to beginning of line N+1.		*/
	FJ_T_ADDR_YCC			ovr_addr[2];			/**< Overwrap address of Overwrap data.														*/
	UINT32					ovr_SizeX[2];			/**< Overwrap area number of pixels per image line.											*/
	UINT32					ovr_SizeY[2];			/**< Overwrap number of image lines.														*/
	UINT32					ovr_LineJumpInBytes[2];	/**< Overwrap number of bytes to jump from beginning of line N to beginning of line N+1.	*/
	FJ_IMG_PFMT				pfmt;					/**< Pixel format.																			*/
} SSTITCHINFO;
typedef SSTITCHINFO* PSSTITCHINFO;

/** A structure containing the jpeg offset info */
typedef struct {
	USHORT		up;							/**< top	*/
	USHORT		bottom;						/**< bottm	*/
	USHORT		right;						/**< right	*/
	USHORT		left;						/**< left	*/
} T_OFFSET;

/** call back information */
typedef struct {
	UINT32		uiSizeX;					/**< Number of pixels per image line.													*/
	UINT32		uiSizeY;					/**< Number of image lines.																*/
	UINT32		uiLineJumpInBytes;			/**< Number of bytes to jump from beginning of line N to beginning of line N+1			*/
	PBYTE		pbyBuffer;					/**< Address of the raw data buffer.													*/
	PBYTE		pbyAllocatedAddress;		/**< Address of the originally allocated buffer (pbyBuffer points inside)				*/
	UINT32		uiFirstPixelColor;			/**< First pixel color																	*/
	UINT32*		puiBleArr;					/**< Array of BLE values (RGB) for all the fields										*/
	UINT32		uiBleArrSize;				/**< Number of entries in puiBleArr array												*/
	ULLONG		ulFrameNo;					/**< Number of readout frame.															*/
	ULONG		ulFrameType;				/**< Don't care																			*/
	UINT32		uiExtraTopRawBytes;			/**< 																					*/
	INT32		iBufferOffset;				/**< 																					*/
	T_OFFSET	offset;						/**< Offset of effective image area														*/
	UCHAR		setup_value;				/**< Don't care																			*/
} SRAWINFO;
typedef SRAWINFO* PSRAWINFO;

/** knee and deknee table */
typedef struct {
	UCHAR*		pp2m_tbl;					/**< knee table address pointer for PKNETBL													*/
	USHORT		knee_tbl_size;				/**< set knee table size for PKNETBL<br> value range :257 fixed number of array				*/
	SHORT*		pm2p_tbl;					/**< deknee table address pointer for M2P0DKNTBL											*/
	USHORT		deknee_tbl_size;			/**< set deknee table table size for M2P0DKNTBL<br> value range :256 fixed number of array	*/
} FJ_T_KNEE_DKNEE_TBL;

/** LVDS Sync Code Information */
typedef struct {
	USHORT	sav[4];							/**< Effective pixel start(SAV)<br>
												value range  :[0x0000 - 0x3FFF]<br>
												target registor  :@@SAV_1st - @@SAV_4th */
	USHORT	eav[4];							/**< Effective pixel end(EAV)<br>
												value range  :[0x0000 - 0x3FFF]<br>
												target registor  :@@EAV_1st - @@EAV_4th */
	USHORT	sab[4];							/**< V blank start(SAB)<br>
												value range  :[0x0000 - 0x3FFF]<br>
												target registor  :@@SAB_1st - @@SAB_4th */
	USHORT	eab[4];							/**< V blank end(EAB)<br>
												value range  :[0x0000 - 0x3FFF]<br>
												target registor  :@@EAB_1st - @@EAB_4th */
} FJ_PRO_LVDS_SYNC_CODE;

/** LVDS Sync Code Mask Information */
typedef struct {
	USHORT	msav[4];						/**< SAV Mask bit<br>
												value range  :[0x0000 - 0x3FFF]<br>
												target registor  :@@MSAV_1st - @@MSAV_4th */
	USHORT	meav[4];						/**< EAV Mask bit<br>
												value range  :[0x0000 - 0x3FFF]<br>
												target registor  :@@MEAV_1st - @@MEAV_4th */
	USHORT	msab[4];						/**< SAB Mask bit<br>
												value range  :[0x0000 - 0x3FFF]<br>
												target registor  :@@MSAB_1st - @@MSAB_4th */
	USHORT	meab[4];						/**< EAB Mask bit<br>
												value range  :[0x0000 - 0x3FFF]<br>
												target registor  :@@MEAB_1st - @@MEAB_4th */
} FJ_PRO_LVDS_SYNC_MASK;

/** LVDS Lane select information */
typedef struct {
	USHORT	ptn_a:3;						/**< LVDS output line A<br>
												value range :[0 - 7]<br>
												target registor :@@SELA */
	USHORT	ptn_b:3;						/**< LVDS output line B<br>
												value range :[0 - 7]<br>
												target registor :@@SELB */
	USHORT	ptn_c:3;						/**< LVDS output line C<br>
												value range :[0 - 7]<br>
												target registor :@@SELC */
	USHORT	ptn_d:3;						/**< LVDS output line D<br>
												value range :[0 - 7]<br>
												target registor :@@SELD */
	USHORT	ptn_e:3;						/**< LVDS output line E<br>
												value range :[0 - 7]<br>
												target registor :@@SELE */
	USHORT	dummy01:1;						/**< Not used */
	USHORT	ptn_f:3;						/**< LVDS output line F
												value range :[0 - 7]<br>
												target registor :@@SELF */
	USHORT	ptn_g:3;						/**< LVDS output line G 
												value range :[0 - 7]<br>
												target registor :@@SELG */
	USHORT	ptn_h:3;						/**< LVDS output line H<br>
												value range :[0 - 7]<br>
												target registor :@@SELH */
	USHORT	ptn_i:3;						/**< LVDS output line I<br>
												value range :[0 - 7]<br>
												target registor :@@SELI */
	USHORT	ptn_j:3;						/**< LVDS output line J<br>
												value range :[0 - 7]<br>
												target registor :@@SELJ */
	USHORT	dummy02:1;						/**< Not used */
	USHORT	ptn_k:3;						/**< LVDS output line K<br>
												value range :[0 - 7]<br>
												target registor :@@SELK */
	USHORT	ptn_l:3;						/**< LVDS output line L<br>
												value range :[0 - 7]<br>
												target registor :@@SELL */
	USHORT	ptn_m:3;						/**< LVDS output line M<br>
												value range :[0 - 7]<br>
												target registor :@@SELM */
	USHORT	ptn_n:3;						/**< LVDS output line N<br>
												value range :[0 - 7]<br>
												target registor :@@SELN */
	USHORT	ptn_o:3;						/**< LVDS output line O<br>
												value range :[0 - 7]<br>
												target registor :@@SELO */
	USHORT	dummy03:1;						/**< Not used */
	USHORT	ptn_p:3;						/**< LVDS output line P<br>
												value range :[0 - 7]<br>
												target registor :@@SELP */
	USHORT	dummy04:13;						/**< Not used */
} FJ_PRO_LVDS_PIXEL_ORDER;


/** LVDS control table
*/
typedef	struct {
	UCHAR						macro_enable_flg;	/**< 0:disable 1:enable */
	UCHAR						trans_mode;			/**< LVDS input mode<br>
														target registor	:@@LMD */
	UCHAR						lsft;				/**< LVDS Output data right 2bit shift permission<br>
														value range	:[0:no shift 1:2bit shift]<br>
														target registor	:@@LSFT<br>
														remarks	:effective bit is 14 => 12bit out<br>
																 effective bit is 12 => 10bit out<br>
																 effective bit is 10 => 8bit out */
	UCHAR						sync_type;			/**< LVDS synchronous code detection type<br>
														value range	:[0:type0(SAV/EAV/SAB/EAB) 1:type1(SOF/SOL/EOF/EOL)<br>
														target registor	:@@SYNT */
	UCHAR						data_input_type;	/**< LVDS Input size type.<br>
														value range	:[0:A synchronous code is followed. 1:LHSIZE/LHSADD is followed.] <br>
														target registor	:@@SZEN */
	UCHAR						lane;				/**< LVDS use lane number<br>
														value range	:[0x01 - 0x08, 0x0A - 0x0C]<br>
														target registor	:@@LANE */
	UCHAR						output_ch_no;		/**< LVDS sensor number of output channel<br>
														value range	:[0x00: 1ch sensor, 0x01: 2ch sensor, 0x02: 3ch sensor, 0x03: 4ch sensor]<br>
														target registor	:@@LCH */
	UCHAR						data_bit_width;		/**< LVDS output data bit width<br>
														value range	:[0x00: RAW 16bit, 0x01: RAW 14bit, 0x02: RAW 12bit, 0x03: RAW 10bit]<br>
														target registor	:@@LDW */
	UCHAR						sync_bit_fix;		/**< LVDS Sync code bit 16bit fix<br>
														value range	:[0x00: same data_bit_length, 0x01: 16bit fix]<br>
														target registor	:@@SYN16 */
	UCHAR						divide_output;			/**< LVDS divide output for use 1ch sensor<br>
														value range	:[0x00: not divide output, 0x01: divide output]<br>
														target registor	:@@DIVOT */
	UCHAR						Inverted_output;	/**< LVDS Inverted output for use 1ch sensor<br>
														value range	:[0x00: not Inverted output, 0x01: Inverted output]<br>
														target registor	:@@LMIR */
	UCHAR						clock_select_sp0;	/**< LVDS select serial clock for SP0<br>
														value range	:[0x00: stop, 0x01: SNCLK0, 0x02: SNCLK1, 0x03: SNCLK2]<br>
														target registor	:@@LVDSCLKS */
	UCHAR						clock_select_sp1;	/**< LVDS select serial clock for SP1<br>
														value range	:[0x00: stop, 0x01: SNCLK0, 0x02: SNCLK1, 0x03: SNCLK2]<br>
														target registor	:@@LVDSCLKS */
	UCHAR						clock_select_sp2;	/**< LVDS select serial clock for SP2<br>
														value range	:[0x00: stop, 0x01: SNCLK0, 0x02: SNCLK1, 0x03: SNCLK2]<br>
														target registor	:@@LVDSCLKS */
	UCHAR						clock_select_sp3;	/**< LVDS select serial clock for SP3<br>
														value range	:[0x00: stop, 0x01: SNCLK0, 0x02: SNCLK1, 0x03: SNCLK2]<br>
														target registor	:@@LVDSCLKS */
	UCHAR						clock_select_sp4;	/**< LVDS select serial clock for SP4<br>
														value range	:[0x00: stop, 0x01: SNCLK0, 0x02: SNCLK1, 0x03: SNCLK2]<br>
														target registor	:@@LVDSCLKS */
	UCHAR						clock_select_sp5;	/**< LVDS select serial clock for SP5<br>
														value range	:[0x00: stop, 0x01: SNCLK0, 0x02: SNCLK1, 0x03: SNCLK2]<br>
														target registor	:@@LVDSCLKS */
	UCHAR						data_select_sp0_0;	/**< LVDS select serial data for SP0 data 0<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						data_select_sp0_1;	/**< LVDS select serial data for SP0 data 1<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						data_select_sp1_0;	/**< LVDS select serial data for SP1 data 0<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						data_select_sp1_1;	/**< LVDS select serial data for SP1 data 1<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						data_select_sp2_0;	/**< LVDS select serial data for SP2 data 0<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						data_select_sp2_1;	/**< LVDS select serial data for SP2 data 1<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						data_select_sp3_0;	/**< LVDS select serial data for SP3 data 0<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						data_select_sp3_1;	/**< LVDS select serial data for SP3 data 1<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						data_select_sp4_0;	/**< LVDS select serial data for SP4 data 0<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						data_select_sp4_1;	/**< LVDS select serial data for SP4 data 1<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						data_select_sp5_0;	/**< LVDS select serial data for SP5 data 0<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						data_select_sp5_1;	/**< LVDS select serial data for SP5 data 1<br>
														value range	:[0x00: SNDDATA0, 0x01: SNDDATA1, 0x02: SNDDATA2, 0x03: SNDDATA3
														0x04: SNDDATA4, 0x05: SNDDATA5, 0x06: SNDDATA6, 0x07: SNDDATA7
														0x08: SNDDATA8, 0x09: SNDDATA9, 0x0A: SNDDATA10, 0x0B: SNDDATA11]<br>
														target registor	:@@LVDSDATS */
	UCHAR						lane_select;		/**< LVDS sync code lane select<br>
														value range	:[0x00 - 0x3F] 0: Enable eetect side D0 of SPx, 0: Enable eetect side D1 of SPx<br>
														target registor	:@@SYNCSEL */
	USHORT						lane_mask;			/**< LVDS sync code mask setting<br>
														value range	:[0x00 - 0xFFFF]<br>
														target registor	:@@LMSK */
	USHORT						input_pos;			/**< LVDS input position<br>
														value range:[0 - 12288pixel] 4pixel boundary<br>
														target registor:@@LHSADD */
	USHORT						input_size;			/**< LVDS input size<br>
														value range:[4 - 4480pixel] 4pixel boundary<br>
														target registor:@@LHSIZE */

	FJ_PRO_LVDS_SYNC_CODE		sync_code;
	FJ_PRO_LVDS_SYNC_MASK		sync_mask;
	FJ_PRO_LVDS_PIXEL_ORDER		pixel_order;
} FJ_PRO_CTRL_LVDS;

/** SLVS-EC Input differential data value inversion setting. */
typedef struct {
	FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION	lane0:1;	/**< Lane0 */
	FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION	lane1:1;	/**< Lane1 */
	FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION	lane2:1;	/**< Lane2 */
	FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION	lane3:1;	/**< Lane3 */
	FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION	lane4:1;	/**< Lane4 */
	FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION	lane5:1;	/**< Lane5 */
	FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION	lane6:1;	/**< Lane6 */
	FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION	lane7:1;	/**< Lane7 */
} FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_SEL;

/** Valid Lane select information */
typedef struct {
	FJ_PRO_CTRL_SLVS_LANE_ENABLE	lane0:1;	/**< Lane0 */
	FJ_PRO_CTRL_SLVS_LANE_ENABLE	lane1:1;	/**< Lane1 */
	FJ_PRO_CTRL_SLVS_LANE_ENABLE	lane2:1;	/**< Lane2 */
	FJ_PRO_CTRL_SLVS_LANE_ENABLE	lane3:1;	/**< Lane3 */
	FJ_PRO_CTRL_SLVS_LANE_ENABLE	lane4:1;	/**< Lane4 */
	FJ_PRO_CTRL_SLVS_LANE_ENABLE	lane5:1;	/**< Lane5 */
	FJ_PRO_CTRL_SLVS_LANE_ENABLE	lane6:1;	/**< Lane6 */
	FJ_PRO_CTRL_SLVS_LANE_ENABLE	lane7:1;	/**< Lane7 */
	ULONG							dummy:24;	/**< Not used */
} FJ_PRO_CTRL_SLVS_VALID_LANE_SEL;

/** SLVS-EC Common Configuration parameters
*/
typedef struct {
	FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_SEL	InversionSel;		/**< Input differential data value inversion setting. */
	FJ_PRO_CTRL_SLVS_VALID_LANE_SEL				ValidLaneSel;		/**< Valid Lane select information. */
	UCHAR										StandbyLength;		/**< transfer count of Standby Code in Standby Sequence in the range of 18~255. */
	FJ_PRO_CTRL_SLVS_SYMBOL_TYPE				StandbySymbolType;	/**< Symbol code type (D/K) following Comma Symbol inside the Standby Code. */
	UCHAR										StandbySymbol;		/**< Symbol following Comma Symbol inside the Standby Code. */
	UCHAR										DeskewInterval;		/**< Deskew Interval (Idle Code continuous transfer count between Deskew Code in Training Sequence) in the range of 20~255. */
	UCHAR										DeskewLength;		/**< Deskew Length (transfer count of Deskew Code in Training Sequence) in the range of 3~255. */
	FJ_PRO_CTRL_SLVS_SYMBOL_TYPE				DeskewSymbolType;	/**< Code type (D/K) of the Symbol following Comma Symbol inside the Deskew Code. */
	UCHAR										DeskewSymbol;		/**< Symbol following Comma Symbol inside the Deskew Code. */
	FJ_PRO_CTRL_SLVS_SYMBOL_TYPE				IdleSymbolType1;	/**< Code type (D/K) of the Symbol(2nd) of the Idle Code. */
	UCHAR										IdleSymbol1;		/**< Symbol(2nd) of idle code.  */
	FJ_PRO_CTRL_SLVS_SYMBOL_TYPE				IdleSymbolType0;	/**< Code type (D/K) of the Symbol(1st) of the Idle Code. */
	UCHAR										IdleSymbol0;		/**< Symbol(1st) of Idle Code */
	FJ_PRO_CTRL_SLVS_SYMBOL_TYPE				IdleSymbolType3;	/**< Code type (D/K) of the Symbol (4th) of the Idle Code. */
	UCHAR										IdleSymbol3;		/**< Symbol(4th) of idle code. */
	FJ_PRO_CTRL_SLVS_SYMBOL_TYPE				IdleSymbolType2;	/**< Code type (D/K) of the Symbol (3rd) of the Idle Code. */
	UCHAR										IdleSymbol2;		/**< Symbol (3rd) of Idle Code */
} FJ_PRO_CTRL_SLVS_COMMON_CFG;


/** Control the ICPCODE port of PMA macro (common to each PMA) and the REFSEL port of the termination resistor adjustment circuit.<br>
	*Caution* Changing the setting value in normal use is prohibited. */
typedef struct {
	UCHAR	enabled;								/**< Optional parameter setting enabled.[0: disabled  other: enabled]<br>
														If this parameter set disabled, H/W initial value applied to PHYCR0@@REFSEL and PHYCR0@@ICPCODE. */
	UCHAR	refsel;									/**< Termination resistor adjustment circuit mode select. PHYCR0@@REFSEL */
	UCHAR	icpcode;								/**< Charge pump control code PHYCR0@@ICPCODE */
} FJ_PRO_CTRL_SLVS_PMA_INIT_OPT;

/** SLVS-EC Control Information
*/
typedef struct {
	FJ_PRO_CTRL_SLVS_VDOUT_CTRL	vdout_ctrl;			/**< Control the VDOUT output when frame stop is set. TRGR@@VDEN	*/
	FJ_PRO_CTRL_SLVS_ECCCRC_OPT	ecccrc_opt;			/**< ECC option / CRC Option of payload. CFGR0@@ECCCRC	*/
	FJ_PRO_CTRL_SLVS_EXPECT_VAL	expect_val;			/**< Expected value when Line Number Error occurred. MMODE@@LNMAXCLP	*/
} FJ_PRO_CTRL_SLVS_CTRL;

/** SLVS-EC Mode Trimming parameters
*/
typedef struct {
	FJ_PRO_CTRL_SLVS_TRIM_ENABLE	enabled;		/**< SLVS trimming mode enabled. OUTMD@@TRMG */
	USHORT							start_x;		/**< SLVS trimming start X position(16 pixel boundary). HSADD@@HSADD */
	USHORT							width;			/**< SLVS trimming width(16 pixel boundary). HSIZE@@HSIZE */
} FJ_PRO_CTRL_SLVS_TRIM_CFG;

/** Mode Configuration parameters
*/
typedef struct {
	FJ_PRO_CTRL_SLVS_PIXEL_BIT					pixelbit;													/**< SLVS number of bits per pixel. CFGR0@@PixelBit */
	FJ_PRO_CTRL_SLVS_LANE_NUM					lanenum;													/**< SLVS number of valid Lanes. CFGR0@@LaneNum */
	USHORT										linelength;													/**< SLVS number of bits per pixel(4 pixel boundary). CFGR1@@LineLength */
	FJ_PRO_CTRL_SLVS_OUT_FORMAT					output_format;												/**< SLVS output format of the image data interface. OUTMD@@OUTMD */
	FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL	swap_lane[FJ_PRO_CTRL_SLVS_LANE_SWAP_INPUT_PORT_SEL_MAX];	/**< SLVS swap lane configuration. LSWAP@@LxOUT<br>
																												@image html fj_still_slvs_swap.png	*/
	FJ_PRO_CTRL_SLVS_VALID_LANE_SEL				valid_lane;													/**< SLVS valid Lane select. VLANE@@LANEx */
} FJ_PRO_CTRL_SLVS_MODE_CFG;

/** Stream Configuration parameters
*/
typedef struct {
	BOOL							enabled;			/**< Enable/Disable. */
	FJ_PRO_CTRL_SLVS_CTRL			ctrl_cfg;			/**< SLVS-EC Control configuration. */
	FJ_PRO_CTRL_SLVS_TRIM_CFG		trim_cfg;			/**< SLVS-EC Trimming configuration. */
	FJ_PRO_CTRL_SLVS_MODE_CFG		mode_cfg;			/**< SLVS-EC Mode configuration. */
} FJ_PRO_CTRL_SLVS_STREAM_CFG;

/** SLVS control table
*/
typedef	struct {
	FJ_PRO_CTRL_SLVS_COMMON_CFG		common_cfg;										/**< SLVS-EC Common configuration. */
	FJ_PRO_CTRL_SLVS_PMA_INIT_OPT	pma_opt_cfg;									/**< SLVS-EC optional configuration of PMA(Physical Media Attachment) macro. */
	FJ_PRO_CTRL_SLVS_STREAM_CFG		stream_cfg[FJ_PRO_CTRL_SLVS_STREAM_TYPE_BOTH];	/**< SLVS-EC Stream configuration. */
} FJ_PRO_CTRL_SLVS;

/** MIPI-DPHY Information.
*/
typedef struct {
	FJ_PRO_CTRL_DMIPI_RXSYNCMODE				rxsyncmode;				/**< RxSyncMode value setting.<br>
																			target registor	:@@DPSM	*/
	UCHAR										rxsmt_d_val;			/**< value of output data RxSMT_D. Set DPSMTD to 4'h7<br>
																			RxSMT_D is an output signal for setting the same phase input mask in the data lane of D-PHY.<br>
																			target registor	:@@DPSMTD	*/
	UCHAR										rxsmt_c_val;			/**< value of output data RxSMT_C. Set DPSMTD to 4'hE<br>
																			RxSMT_C is an output signal for setting the same phase input mask in the clock lane of D-PHY.<br>
																			target registor	:@@DPSMTC	*/
	FJ_PRO_CTRL_DMIPI_RXENABLE					rxenable;				/**< RxEnable value setting.<br>
																			target registor	:@@DPEN	*/
	FJ_PRO_CTRL_DMIPI_RXMODE					rxmode;					/**< RxMode select value.														*/
	FJ_PRO_CTRL_DMIPI_RXSPEED					rxspeed;				/**< Delay Line Mode Select signal belong Data Rate value.						*/
	FJ_PRO_CTRL_DMIPI_DESKEW_CODE_SEARCH_WIDTH	code_search_width;		/**< Periodic De-skew code search width setting signals.						*/
	FJ_PRO_CTRL_DMIPI_MIN_VALID_WIDTH			min_valid_width;		/**< Minimum valid width setting signals.										*/
	FJ_PRO_CTRL_DMIPI_SAMPLING_NUM				sampling_num;			/**< Sampling number setting of each DelayLine code in Deskew sequence.			*/
} FJ_PRO_CTRL_DMIPI_PHY_CTRL;

/** MIPI-DPHY Configuration.
*/
typedef struct {
	FJ_PRO_CTRL_DMIPI_LANE			lane_sel;				/**< Number of input lanes selection.<br>
																target registor	:@@LANE	*/
	FJ_PRO_CTRL_DMIPI_VC			vc_mode_sel[2];			/**< select the Virtual Channel value for the packets output to the OUT0/1 side.<br>
																target registor	:@@VCOUT0/1	*/
	FJ_PRO_CTRL_DMIPI_IN_DATA_MODE	input_data_mode[2];		/**< select the input data mode.<br>
																target registor	:@@DTMD0/1	*/
	FJ_PRO_CTRL_DMIPI_OUT_MODE		output_data_mode[2];	/**< Output data mode selection.<br>
																target registor	:@@OUTMD0/1	*/
	FJ_PRO_CTRL_DMIPI_DT_OUT		output_dt_sel[2];		/**< Output DT selection mode.<br>
																target registor	:@@DTCOMD0/1	*/
	UCHAR							output_dt_setting[2];	/**< DataType value for the packets output to the OUT0/1 side.<br>
																target registor	:@@DTCO0/1	*/
	FJ_PRO_CTRL_DMIPI_PHY_CTRL		phy_ctrl;				/**< MIPI D-PHY control. */
	FJ_PRO_CTRL_DMIPI_SEL_CYCLE		out_cycle[2];			/**< Setting the cycle for OUT0/1 side.<br>
																target registor	:@@HDPDSEL0/1	*/
	FJ_PRO_CTRL_DMIPI_MULTI_CH		multi_ch_mode;			/**< Line division processing multi-CH combination mode setting.<br>
																target registor	:LDIVCTL@@LDMCH */
} FJ_PRO_CTRL_DMIPI_CTRL;

/** MIPI D-PHY control table
*/
typedef	struct {
	BOOL						enabled;			/**< Enable/Disable. */
	FJ_PRO_CTRL_DMIPI_CTRL		ctrl_cfg;			/**< MIPI D-PHY Control configuration. */
} FJ_PRO_CTRL_DMIPI;

/** MIPI-CPHY Information.<br>
	detail of this setting value, please refer to C-PHY macro specifications.
*/
typedef struct {
	FJ_PRO_CTRL_CMIPI_RXENABLE					rxenable;				/**< RxEnable value setting.				target registor	:@@CPEN	*/
	UCHAR										hs_settlecnt;			/**< Settle count setting.					target registor	:@@CP_HS_SETTLECNT	*/
	UCHAR										hs_stopcnt;				/**< Stop count setting.					target registor	:@@CP_HS_STOPCNT	*/
	UCHAR										hs_syncmode;			/**< Sync detection mode setting.			target registor	:@@CP_HS_SYNCMODE	*/
	UCHAR										rlr_lconfig;			/**< Lane configuration.					target registor	:@@CP_RLR_LCONFIG	*/
	UCHAR										rlr_tpsel;				/**< Test pattern select.					target registor	:@@CP_RLR_TPSEL		*/
	UCHAR										rlr_prbssp;				/**< PRBS seed value.						target registor	:@@CP_RLR_PRBSSP	*/
	ULLONG										tgr_progseq;			/**< Preamble configuration sequence value.	target registor	:@@CP_TGR_PROGSEQ	*/
	UCHAR										forcerxmode2;			/**< ForceRxmode(lane2) value.				target registor	:@@CP_FORCERXMODE2	*/
	UCHAR										forcerxmode1;			/**< ForceRxmode(lane1) value.				target registor	:@@CP_FORCERXMODE1	*/
	UCHAR										forcerxmode0;			/**< ForceRxmode(lane0) value.				target registor	:@@CP_FORCERXMODE0	*/
} FJ_PRO_CTRL_CMIPI_PHY_CTRL;

/** MIPI-CPHY Configuration.
*/
typedef struct {
	FJ_PRO_CTRL_CMIPI_LANE			lane_sel;				/**< Number of input lanes selection.<br>
																target registor	:@@LANE	*/
	FJ_PRO_CTRL_CMIPI_VC			vc_mode_sel[2];			/**< select the Virtual Channel value for the packets output to the OUT0/1 side.<br>
																target registor	:@@VCOUT0/1	*/
	FJ_PRO_CTRL_CMIPI_IN_DATA_MODE	input_data_mode[2];		/**< select the input data mode.<br>
																target registor	:@@DTMD0/1	*/
	FJ_PRO_CTRL_CMIPI_OUT_MODE		output_data_mode[2];	/**< Output data mode selection.<br>
																target registor	:@@OUTMD0/1	*/
	FJ_PRO_CTRL_CMIPI_DT_OUT		output_dt_sel[2];		/**< Output DT selection mode.<br>
																target registor	:@@DTCOMD0/1	*/
	UCHAR							output_dt_setting[2];	/**< DataType value for the packets output to the OUT0/1 side.<br>
																target registor	:@@DTCO0/1	*/
	FJ_PRO_CTRL_CMIPI_SEL_CYCLE		out_cycle[2];			/**< Setting the cycle for OUT0/1 side.<br>
																target registor	:@@HDPDSEL0/1	*/
	FJ_PRO_CTRL_CMIPI_PHY_CTRL		phy_ctrl;				/**< MIPI C-PHY control. */
	FJ_PRO_CTRL_CMIPI_MULTI_CH		multi_ch_mode;			/**< Line division processing multi-CH combination mode setting.<br>
																target registor	:LDIVCTL@@LDMCH */
} FJ_PRO_CTRL_CMIPI_CTRL;

/** MIPI C-PHY control table
*/
typedef	struct {
	BOOL						enabled;			/**< Enable/Disable. */
	FJ_PRO_CTRL_CMIPI_CTRL		ctrl_cfg;			/**< MIPI C-PHY Control configuration. */
} FJ_PRO_CTRL_CMIPI;

/** PRO Input Data first pixel
*/
typedef enum {
	FJ_PRO_CTRL_FIRST_PIX_COLOR_R = 0,			/**< R pixel standard	*/
	FJ_PRO_CTRL_FIRST_PIX_COLOR_B,				/**< B pixel standard	*/
	FJ_PRO_CTRL_FIRST_PIX_COLOR_GR,				/**< Gr pixel standard	*/
	FJ_PRO_CTRL_FIRST_PIX_COLOR_GB				/**< Gb pixel standard	*/
} FJ_PRO_CTRL_FIRST_PIX_COLOR;

/** Sensor Readout sampling count.
*/
typedef enum {
	FJ_PRO_CTRL_READOUT_SAMPLING_CNT_1 = 0,			/**< No subsampling	*/
	FJ_PRO_CTRL_READOUT_SAMPLING_CNT_2,				/**< 1/2 subsampling	*/
	FJ_PRO_CTRL_READOUT_SAMPLING_CNT_3,				/**< 1/3 subsampling	*/
	FJ_PRO_CTRL_READOUT_SAMPLING_CNT_4				/**< 1/4 subsampling	*/
} FJ_PRO_CTRL_READOUT_SAMPLING_CNT;

/** SG control table
*/
typedef struct {
	UCHAR		vhdo;						/**< VD/HD I/O control.	<br>
												value range		:[0:Input 1:Output]<br>
												target registor	:@@VHDO	*/
	UCHAR		vdot;						/**< VD output timming.<br>
												value range		:[0:HD0 2cycle 1:HD0 1cycle]<br>
												target registor @@VDOT				*/
	USHORT		vdinv;						/**< VD Reversal output.<br>
												value range		:[0:L out 1:H out]<br>
												target registor	:@@VDINV	*/
	UCHAR		hdinv;						/**< HD Reversal output.<br>
												value range		:[0:L out 1:H out]<br>
												target registor @@HDINV				*/
	UCHAR		hdchg;						/**< Switch of Last HD output.<br>
												value range		:[0:Regularity 1:Lastline onry]<br>
												target registor @@HDCHG			*/
	UCHAR		vddly;						/**< VD output delay.<br>
												value range		:[0-255]<br>
												target registor @@VDDLY	*/
	ULONG		vdocyc;						/**< VD0 output cycle.<br>
												value range		:[0(=0x4000)  0x0004 - 0x3FFF]<br>
												target registor	@@VDOCYC		*/
	USHORT		vdow;						/**< VD0 output width.<br>
												value range		:[0(=512)  1 - 511]<br>
												target registor	@@VDOW			*/
	USHORT		hdocyc;						/**< HD0 output cycle.<br>
												value range		:[0(=0x4000)  0x0001 - 0x3FFF]<br>
												target registor	@@HDOCYC		*/
	USHORT		hdow;						/**< HD0 output width.<br>
												value range		:[0(=512)  1 - 511]<br>
												target registor	@@HDOW			*/
	USHORT		hdocyc2;					/**< HD0 output cycle2.<br>
												value range		:[0(=0x4000)  0x0001 - 0x3FFF]<br>
												target registor	@@HDOCYC2			*/
	USHORT		hdow2;						/**< VD0 output width2.<br>
												value range		:[0(=512)  1 - 511]<br>
												target registor	@@HDOW2				*/
} FJ_SG_CTRL;

/** Crop rectangle information
*/
typedef struct {
	USHORT		sta_x;						/** First pixel of the input image for created output image. (ex. D-Zoom rectangle.)	*/
	USHORT		sta_y;						/** First line of the input image for created output image. (ex. D-Zoom rectangle.)		*/
	USHORT		width;						/**< Number of pixels per line in the input image for created output image.				*/
	USHORT		lines;						/**< Number of lines per input image for created output image.							*/
} FJ_CROP_INFO;

/** Blend control
*/
typedef struct {
	FJ_IMG_INFO			src_img;			/**< source image							*/
	FJ_IMG_INFO			osd_img;			/**< OSD    image 							*/
	UINT32				alpha_addr;			/**< alpha lane address.					*/
	USHORT				sta_x;				/**< blend x position of source image		*/
	USHORT				sta_y;				/**< blend y position of source image		*/
	INT32				timeout;			/**< timeout(msec). -1:wait forever.		*/
} FJ_IMG_BLEND_CTRL;

/**
Structure(Union) defining Color.<br>
It is applicable for all kinds of color.
*/
typedef struct {
	BYTE Y;									/**< Y		*/
	BYTE Cb;								/**< Cb		*/
	BYTE Cr;								/**< Cr		*/
} FJ_IMG_COLOR;

/** Rect Frame Information */
typedef struct {
	T_Rect			pos;					/**< The start of coordinate and size.( 2pixel boundary )	*/
	FJ_IMG_COLOR	color_ycc;				/**< Fill color.											*/
} FJ_RECT_FRAME_EACH_SET;

/** Face Frame Information */
typedef struct {
	FJ_RECT_FRAME_EACH_SET	rect[D_FJ_OSD_DRAW_RECT_NUM];	/**<rect frame position and color */
	USHORT					rect_count;						/**< Number of rect frame. Erased, except for the specified rect.<br>
																(rect frame 0 ~ rect frame max(@ref D_FJ_OSD_DRAW_RECT_NUM)-1).<br>		*/
} FJ_RECT_FRAME_SET;


/** Image Convert control
*/
typedef struct {
	FJ_IMG_INFO_CONV		src_img;			/**< source image information	 	*/
	FJ_IMG_INFO_CONV		dst_img;			/**< destination image information	*/
	FJ_IMG_RESIZE_MODE		resize_mode;		/**< Interpolation mode select		*/
	FJ_IMG_ROTATE_DEGREE	rotate_deg;			/**< Rotate degree (clockwise) */  
	UCHAR					fill_enable;		/**< Fill function enable			*/
	INT32					timeout;			/**< timeout(msec). -1:wait forever.*/
} FJ_IMG_CONV_CTRL;


/** Image Crop control
*/
typedef struct {
	FJ_IMG_CROP_RECT		src_rect;			/**< source image crop rectangle	*/
	FJ_IMG_INFO				src_img;			/**< source image information	 	*/
	FJ_IMG_INFO				dst_img;			/**< destination image information	*/
	FJ_IMG_RESIZE_MODE		resize_mode;		/**< Interpolation mode select		*/
	UCHAR					fill_enable;		/**< Fill function enable			*/
	INT32					timeout;			/**< timeout(msec). -1:wait forever.*/
} FJ_IMG_CROP_CTRL;

/** Image cropping parameter.
*/
typedef struct {
	FJ_IMG_CROP_CTRL crop_ctrl;
	FJ_IMG_INFO		 display_img;			/**< source image information		*/
	UCHAR			 fill_enable;			/**< Fill function enable			*/
	UCHAR			 rotation;
} FJ_IMG_CROP_CTRL_SPLIT;

/** Image Fill control
*/
typedef struct {
	FJ_IMG_INFO				dst_img;			/**< destination image information	*/
	UCHAR					dst_mif_sel;		/**< Master IF select<br>
													0: Master IF0<br>
													1: Master IF1<br>
													2: Master IF2<br>
												*/
	INT32					timeout;			/**< timeout(msec). -1:wait forever.*/
} FJ_IMG_FILL_CTRL;

/** Image free rectangle table information */
typedef struct {
	UINT32	addr;							/**< Free rectangle table address */
	USHORT	h_num;							/**< Free rectangle table horizontal parameter number */
	USHORT	v_num;							/**< Free rectangle table vertical parameter number */
} FJ_IMG_FRECT_TABLE_INFO;

/** Image free rectangle convert control */
typedef struct {
	FJ_IMG_INFO				src_img;		/**< source image information */
	FJ_IMG_INFO				dst_img;		/**< destination image information */
	FJ_IMG_FRECT_TABLE_INFO	table_info;		/**< Image free rectangle table information */
	FJ_IMG_RESIZE_MODE		resize_mode;	/**< Interpolation mode select */
	UCHAR					unit_num;		/**< The number of the unit used by free rectangle<br>
												0: Single<br>
												1: Double<br>
											*/
	UCHAR					fill_enable;	/**< Fill function enable */
	INT32					timeout;		/**< timeout(msec). -1:wait forever.*/
} FJ_IMG_FRECT_CTRL;

/** Image Jpeg encode control
*/
typedef struct {
	FJ_IMG_INFO				src_img;			/**< source image information	 */
	UINT32					dst_addr;			/**< destination jpeg address	*/
	ULONG					enc_size;			/**< result size of Jpeg encoded	*/
	UINT32					upper_limit_size;	/**< Upper limit size of Jpeg encoding	*/
} FJ_IMG_JPEG_CTRL;


/** Typedef of scenario element structure. */
typedef struct fj_still_snr_elem FJ_STILL_SNR_ELEM;

/** Imaging scenario element/control function pointer. */
typedef INT32 (*FJ_STILL_SNR_ELEM_FUNC)( VOID* instance, ULONG arg );
/** Imaging scenario "if" statement pointer. */
typedef BOOL (*FJ_STILL_SNR_COND_FUNC)( VOID* instance, ULONG arg );
/** Imaging scenario instance each function pointer. */
typedef VOID (*FJ_STILL_SNR_EACH_FUNC)( VOID* instance, ULONG func_arg );

/** Scenario element structure. */
struct fj_still_snr_elem {
	FJ_STILL_SNR_SYNTYP				sync_type;			/**< Synchronization type. (must) */
	FJ_STILL_SNR_ELEM_FUNC			elem_pre_func;		/**< Element pre function pointer. Before execute elem_post_func. Map hunt etc. (may) */
	ULONG							elem_pre_arg;		/**< Element pre function argument. Before execute elem_post_func. Map hunt etc. (may) */
	FJ_STILL_SNR_ELEM_FUNC			elem_post_func;		/**< Element post function pointer. Execute on worker when FJ_STILL_SNR_SYNTYP_ASYNC. (must) */
	ULONG							elem_post_arg;		/**< Element post function argument. Execute on worker when FJ_STILL_SNR_SYNTYP_SYNC. (may) */
	FJ_STILL_SNR_COND_FUNC			cond_func;			/**< Condition function pointer. (may. When NULL is elem_pre_func and elem_post_func always run.) */
	ULONG							cond_arg;			/**< Condition function argument. */
	INT32							task_prio_ofs;		/**< Element task priority offset. */
};

/** Scenario dump callback. */
typedef VOID (*FJ_STILL_SNR_DUMP_INST_CB)( VOID* instance, CHAR msgbuf[], UINT32 msgbuf_len );

/** Scenario structure. */
typedef struct {
	UINT32							no;					/**< Scenario number. (User defined.) */
	const CHAR*						name;				/**< Scenario name. (User defined.) */
	const FJ_STILL_SNR_ELEM*		elem;				/**< Scenario element array pointer. sync_type of last element's must be set FJ_STILL_SNR_SYNTYP_END. */
	FJ_STILL_SNR_DUMP_INST_CB		funcptr;			/**< Scenario dump callback. (may NULL) */
} FJ_STILL_SEQ_SNR;

/** IQ Parameter structure of Video/Still callback APIs. */
typedef struct {
	T_FJ_IQ_SEL_INFO	sel_info_sen;	/**< IQ parameter selection information for sen. (Select IQ binary.) */
	T_FJ_IQ_SEL_INFO	sel_info;		/**< IQ parameter selection information for demosaic. (Select IQ binary.) */

	BOOL				ela_onoff;		/**< ELA On/Off status. May overwrite. (Default: IQ binary.) */
	BOOL				cnr_onoff;		/**< CNR On/Off status. May overwrite. (Default: IQ binary.) */
} T_FJ_STILL_IQ_PARAM;

/**
Stream information for view process.
*/
typedef struct {
	BOOL								enable;				/**< Enable/Disable. */
	E_FJ_STILL_STERAM_PROC_TYPE			process_type;		/**< Scenario type. */
	UINT32								stream_type;		/**< Stream type.
															  <table>
															  <tr><td>stream_type</td><td>pfmt</td></tr>
															  <tr><td>FJ_STREAM_TYPE_H264</td><td>FJ_IMG_PFMT_VIDEO</td></tr>
															  <tr><td>FJ_STREAM_TYPE_JPEG</td><td>FJ_IMG_PFMT_YCC420<br>FJ_IMG_PFMT_YCC422</td></tr>
															  </table>
															  */
	FJ_IMG_PFMT							pfmt;				/**< YCC Pixel format. */
	USHORT								fps;				/**< Fps (ex. 60=>60fps) */
	USHORT								width;				/**< Width */
	USHORT								lines;				/**< Lines */
	FJ_IMG_DISPLAY						display;			/**< display out. */
} FJ_STILL_STREAM_INFO;

/**
SPF kernel.
*/
typedef struct {
	SHORT	kernel_y[17];						/** SPF kernel value for Y.<br>
												Valid bits is 13bits signed. (Bit format: Sxxxx.xxxxx_xxxx. S=signed bit.)<br>
												<br>
												Calculate formula.<br>
												<br>
												Output value = <br>
												(pix[0][0] + pix[4][4]) * K9 +<br>
												(pix[0][1] + pix[4][3]) * K10 +<br>
												(pix[0][2] + pix[4][2]) * K11 +<br>
												(pix[0][3] + pix[4][1]) * K12 +<br>
												(pix[0][4] + pix[4][0]) * K13 +<br>
												(pix[1][0] + pix[3][4]) * K14 +<br>
												(pix[1][4] + pix[3][0]) * K15 +<br>
												(pix[2][0] + pix[2][4]) * K16 +<br>
												(pix[1][1] *K0 ) + (pix[1][2] *K1 ) + (pix[1][3] *K2 ) +<br>
												(pix[2][1] *K3 ) + (pix[2][2] *K4 ) + (pix[2][3] *K5 ) +<br>
												(pix[3][1] *K6 ) + (pix[3][2] *K7 ) + (pix[3][3] *K8 )<br>
												LPF kernel value matrix:<br>
												<TABLE>
												<TR><TD>K9</TD><TD>K10</TD><TD>K11</TD><TD>K12</TD><TD>K13</TD></TR>
												<TR><TD>K14</TD><TD>K0</TD><TD>K1</TD><TD>K2</TD><TD>K15</TD></TR>
												<TR><TD>K16</TD><TD>K3</TD><TD>K4</TD><TD>K5</TD><TD>K16</TD></TR>
												<TR><TD>K15</TD><TD>K6</TD><TD>K7</TD><TD>K8</TD><TD>K14</TD></TR>
												<TR><TD>K13</TD><TD>K12</TD><TD>K11</TD><TD>K10</TD><TD>K9</TD></TR>
												</TABLE>
												<br>
												Pixel:<br>
												<TABLE>
												<TR><TD>pix[0][0]</TD><TD>pix[0][1]</TD><TD>pix[0][2]</TD><TD>pix[0][3]</TD><TD>pix[0][4]</TD></TR>
												<TR><TD>pix[1][0]</TD><TD>pix[1][1]</TD><TD>pix[1][2]</TD><TD>pix[1][3]</TD><TD>pix[1][4]</TD></TR>
												<TR><TD>pix[2][0]</TD><TD>pix[2][1]</TD><TD>pix[2][2] - Target pixel</TD><TD>pix[2][3]</TD><TD>pix[2][4]</TD></TR>
												<TR><TD>pix[3][0]</TD><TD>pix[3][1]</TD><TD>pix[3][2]</TD><TD>pix[3][3]</TD><TD>pix[3][4]</TD></TR>
												<TR><TD>pix[4][0]</TD><TD>pix[4][1]</TD><TD>pix[4][2]</TD><TD>pix[4][3]</TD><TD>pix[4][4]</TD></TR>
												</TABLE>
												*/
	SHORT	kernel_c[17];					/** LPF kernel value for C.
												Calculate formula ando kernel value matrix refer @ref lpf_kernel_y.<br>
												Pixel position is YCC444. When YCC422 and YCC420, Imaginate convert to YCC444.
												*/
} FJ_IMG_SPF_KERNEL;

/** SPF control
*/
typedef struct {
	FJ_IMG_INFO			src_img;			/**< source image							*/
	FJ_IMG_INFO			dst_img;			/**< destination image 						*/
	FJ_IMG_SPF_KERNEL	spf_kernel;			/**< SPF kernel value.						*/
	INT32				timeout;			/**< timeout(msec). -1:wait forever.		*/
} FJ_IMG_SPF_CTRL;


typedef struct {
	UINT32		sram_bank_address;
	UINT32		sram_bank_size;
} FJ_STILL_IMG_SRAM_BANK_INFO;

/** Adaptive AE parameter */
typedef struct {
	FJ_STILL_ADAPTIVE_AE_MODE	adaptive_ae_mode;
	UCHAR						fix_tone_no_fg;
	UCHAR						fix_tone_no_ae;
} FJ_STILL_ADAPTIVE_AE_PARAM;

/* Image information.
*/
typedef struct {
	VOID*		pImgAdrs;
	UINT32		gImgSize;
	UINT32		wImgSize;
	UINT32		hImgSize;
} FJ_STILL_IP_IMAGE_INFO;

/* Image parameter. */
typedef struct {
	FJ_STILL_IP_IMAGE_FORM			imgForm;
	FJ_STILL_IP_IMAGE_INFO			imgInfo[ 3 ];
	UINT32							bitDepth;
	FJ_STILL_IP_IMAGE_DATA_FORM		dataForm;
} FJ_STILL_IP_IMAGE;

/** still ip image information. */
typedef struct {
	UINT32						wSize;
	UINT32						hSize;
	FJ_STILL_IP_IMAGE_FORM		imgForm;
} FJ_STILL_IP_IMG_INFO_PRM;

/** Sensor window information.
*/
typedef struct {
	UINT32		global_width;						/**< Global input width	*/
	UINT32		global_lines;						/**< Global input lines	*/
	UINT32		ob_x_pos;							/**< OB area X offset	*/
	UINT32		ob_y_pos;							/**< OB area Y offset	*/
	UINT32		ob_in_width;						/**< OB area width		*/
	UINT32		ob_in_lines;						/**< OB area lines		*/
} FJ_SENSOR_WINDOW_CONFIG;

/** SEN window config.
*/
typedef struct {
	UINT32		x_pos;								/**< Input X offset		*/
	UINT32		y_pos;								/**< Input Y offset		*/
	UINT32		in_width;							/**< Input width		*/
	UINT32		in_lines;							/**< Input lines		*/
	UINT32		out_width;							/**< Output width		*/
	UINT32		out_lines;							/**< Output lines		*/
} FJ_SEN_WINDOW_CONFIG;

/** Demosaic scaler config.
*/
typedef struct {
	UINT32		width;								/**< Scaler width		*/
	UINT32		lines;								/**< Scaler lines		*/
} FJ_DEMOSAIC_SCALER_CONFIG;

/** Demosaic output window config.
*/
typedef struct {
	UINT32		width;								/**< Output width		*/
	UINT32		lines;								/**< Output lines		*/
} FJ_DEMOSAIC_OUTPUT_CONFIG;

/** Stitch output window config.
*/
typedef struct {
	UINT32		width;								/**< Output width		*/
	UINT32		lines;								/**< Output lines		*/
} FJ_STITCH_OUTPUT_CONFIG;

/** Demosaic window config.
*/
typedef struct {
	UINT32						x_pos;				/**< Input X offset			*/
	UINT32						y_pos;				/**< Input Y offset			*/
	UINT32						in_width;			/**< Input width			*/
	UINT32						in_lines;			/**< Input lines			*/
	FJ_DEMOSAIC_SCALER_CONFIG	scaler_in;			/**< Scaler Input config	*/
	FJ_DEMOSAIC_SCALER_CONFIG	scaler_out;			/**< Scaler Output config	*/
	FJ_DEMOSAIC_OUTPUT_CONFIG	output[FJ_DEMOSAIC_OUTPUT_NUM_MAX];
													/**< Output config array	*/
} FJ_DEMOSAIC_WINDOW_CONFIG;

/** Stitch input window config.
*/
typedef struct {
	UINT32						in_width;			/**< Input width. (Must multiple of 8 pixel.)		*/
	UINT32						in_lines;			/**< Input lines. (Must multiple of 8 pixel.)		*/
} FJ_STITCH_INPUT_WINDOW_CONFIG;

/** Stitch window config.
*/
typedef struct {
	FJ_STITCH_INPUT_WINDOW_CONFIG	in_left;		/**< Input Config(Left).							*/
	FJ_STITCH_INPUT_WINDOW_CONFIG	in_right;		/**< Input Config(Right).							*/
	UINT32							ovr_width[2];	/**< Overwrap width. (Must multiple of 8 pixel.)	*/
	UINT32							img_width_div[2];	/**< Devided image width. */
	FJ_STITCH_OUTPUT_CONFIG		output[FJ_STITCH_OUTPUT_NUM_MAX];
													/**< Output config array	*/
} FJ_STITCH_WINDOW_CONFIG;

/** Encode output window config.
*/
typedef struct {
	UINT32		x_pos;								/**< Input X offset		*/
	UINT32		y_pos;								/**< Input Y offset		*/
	UINT32		in_width;							/**< Input width		*/
	UINT32		in_lines;							/**< Input lines		*/
	UINT32		out_width;							/**< Output width		*/
	UINT32		out_lines;							/**< Output lines		*/
} FJ_ENCODE_OUTPUT_CONFIG;

/** Encode window config.
*/
typedef struct {
	FJ_ENCODE_OUTPUT_CONFIG		main;				/**< Main				*/
	FJ_ENCODE_OUTPUT_CONFIG		screen;				/**< Screennail			*/
	FJ_ENCODE_OUTPUT_CONFIG		thumb;				/**< Thumbnail			*/
} FJ_ENCODE_WINDOW_CONFIG;

/** Sensor I/F information.
*/
typedef struct {
	FJ_PRO_CTRL_SLVS	slvs;							/**< SLVS-EC control	*/
	FJ_PRO_CTRL_LVDS	lvds[FJ_STILL_LVDS_CH3 + 1];	/**< LVDS control	*/
	FJ_PRO_CTRL_DMIPI	dmipi[FJ_STILL_DMIPI_CH3 + 1];	/**< MIPI-DPHY control	*/
	FJ_PRO_CTRL_CMIPI	cmipi[FJ_STILL_CMIPI_CH3 + 1];	/**< MIPI-CPHY control	*/
} FJ_STILL_SEN_SENSOR_IF;

/** FJ_STILL_SEN_OB_MODE.
*/
typedef enum {
	FJ_STILL_SEN_OB_MODE_IQ = 0,	/*< use IQ value	*/
	FJ_STILL_SEN_OB_MODE_AUTO		/*< detect and substruct	*/
} FJ_STILL_SEN_OB_MODE;

/** Sensor I/F information.
*/
typedef struct {
	FJ_STILL_SEN_SENSOR_LP	lp_enabled;				/**< Sensor Low-Power control select.	*/
	USHORT					sensor_vd_sampling_cnt;	/**< Sensor VD sampling count.	*/
} FJ_STILL_SEN_SENSOR_LP_CTRL;

/** SEN configuration.
*/
typedef struct {
	FJ_STILL_SEN_INPUT_FORMAT			input_format;		/**< Input format						*/
	FJ_STILL_SEN_BIT_WIDTH				input_bit_width;	/**< Input bit width					*/
	FJ_STILL_SEN_SENSOR_INTERFACE		interface_sel;		/**< Interface select					*/
	FJ_STILL_SEN_SENSOR_IF				sensor_if;			/**< Sensor I/F configration			*/
	FJ_PRO_CTRL_FIRST_PIX_COLOR			first_pix_color;	/**< RAW first pixel color				*/
	FJ_PRO_CTRL_READOUT_SAMPLING_CNT	sampling_count;		/**< Sensor Readout sampling count.<br>
																e.g.<br>
																	Original sensor readout fps :120<br>
																	@ref FJ_PRO_CTRL_READOUT_SAMPLING_CNT_1 :readout fps:120<br>
																	@ref FJ_PRO_CTRL_READOUT_SAMPLING_CNT_2 :readout fps: 60<br>
																	@ref FJ_PRO_CTRL_READOUT_SAMPLING_CNT_3 :readout fps: 40<br>
																	@ref FJ_PRO_CTRL_READOUT_SAMPLING_CNT_4 :readout fps: 30 */
	FJ_STILL_SEN_OB_MODE				ob_mode;			/**< ob mode */
	FJ_STILL_SEN_SENSOR_LP_CTRL			lp_ctrl;			/**< Sensor Low-Power Control			*/
} FJ_STILL_SEN_CONFIG;

/** SHDR configuration.
*/
typedef struct {
	UINT32							fptParams;			/**< Set FPT(Global motion detection) parameter. (T.B.D.)			*/
	UINT32							exposureDiff;		/**< Set exposure differnce between two frames. 1EV=256.<br>
															ex:-1EV,0EV,+1EV=256, -2EV,0EV,+2EV=512, -3EV,0EV,+3EV=768.		*/
	UINT32							isoValue[3];		/**< Set ISO value each image.										*/
	UINT32							ltmParams;			/**< Set LTM parameter. (T.B.D.)									*/
} FJ_STILL_SHDR_CONFIG;

/** 3DNR configuration.
*/
typedef struct {
	FJ_STILL_3DNR_MODE			mode;					/**< 3DNR mode that input size indicate automatic setting or manually setting.	*/
	UINT32						x_pos;					/**< Input X offset of 3DNR. (If automatic setting mode, this parameter is don't care.)	*/
	UINT32						y_pos;					/**< Input Y offset of 3DNR. (If automatic setting mode, this parameter is don't care.)	*/
	UINT32						in_width;				/**< Input width of 3DNR. (If automatic setting mode, this parameter is don't care.)	*/
	UINT32						in_lines;				/**< Input lines of 3DNR. (If automatic setting mode, this parameter is don't care.)	*/
	BOOL						compression_enable;		/**< Frame Compression Enable Flag. */
	BOOL						bypass_enable;			/**< Bypass mode Enable Flag. */
	BOOL						mc_enable;				/**< Mc mode Enable Flag. */
} FJ_STILL_3DNR_CONFIG;

/** OB value
*/
typedef struct {
	SHORT		R;		/**< R		*/
	SHORT		Gr;		/**< Gr		*/
	SHORT		Gb;		/**< Gb		*/
	SHORT		B;		/**< B		*/
} FJ_STILL_OB_VALUE;

/** SGDE table information
 */
typedef struct {
    FLOAT				u;			/**< Horizontal position in pixels on the input image. */
    FLOAT				v;			/**< Vertical position in pixels on the input image. */
} FJ_GE_CELL;

/** SGDE table information
 */
typedef struct {
	FJ_GE_CELL*	ge_tbl_addr;		/**< Pointer of Calculated SGDE table output memory. */
	ULONG		ge_tbl_max_bytes;	/**< Area size of SGDE table output memory. */
	ULONG		ge_tbl_bytes;		/**< Size of Calculated SGDE table output memory. */
	UINT32		width;				/**< Width of calculated SGDE table. */
	UINT32		height;				/**< Height of calculated SGDE table. */
	UINT32		pitchX;				/**< Cell width of calculated SGDE table. */
	UINT32		pitchY;				/**< Cell height of calculated SGDE table. */
} FJ_GE_TBL_INFO;

/** Warping table calculation parameter.
 */
typedef struct {
	USHORT				crop_sta_x;			/**< Crop start left position. (rectangle is input image) */
	USHORT				crop_sta_y;			/**< Crop start top position. (rectangle is input image) */
	USHORT				crop_width;			/**< Crop width. (rectangle is input image) */
	USHORT				crop_lines;			/**< Crop lines. (rectangle is input image) */
	USHORT				in_img_width;		/**< Input image width. */
	USHORT				in_img_lines;		/**< Input image lines. */
	USHORT				out_img_width;		/**< Output image width. */
	USHORT				out_img_lines;		/**< Output image lines. */
} FJ_WARP_CALC_PARAM;

/** Image fill area parameter.
*/
typedef struct {
	UINT32				addr;			/**< Start address of target buffer								*/
	USHORT				g_width;		/**< Global horizontal size (Global width:set 16byte unit)		*/
	USHORT				width;			/**< Horizontal size (Width of Image size)						*/
	USHORT				lines;			/**< Vertical size (Lines)										*/
	FJ_IMG_FILL_TYPE	fill_type;		/**< Fill type selection										*/
	FJ_IMG_FILL_DATA 	fill_data;		/**< Fill data (Range of each data: 0x00-0xFF)					*/
} FJ_IMG_FILL_INFO;

/** RGB offset.
*/
typedef struct {
	SHORT				offset_r;		/**< Offset R correction value before TC	*/
	SHORT				offset_g;		/**< Offset G correction value before TC	*/
	SHORT				offset_b;		/**< Offset B correction value before TC	*/
	SHORT				offset_yb;		/**< Offset Yb correction value before TC	*/
} T_FJ_STILL_RGB_OFFSET;

/** TCT control parameter.
*/
typedef struct {
	USHORT				start_x;		/**< start X position		*/
	USHORT				start_y;		/**< start Y position		*/
	UCHAR				block_hsiz;		/**< Horizontal block size	*/
	UCHAR				block_vsiz;		/**< Vertical block size	*/
	UCHAR				block_hnum;		/**< Horizontal block count	*/
	UCHAR				block_vnum;		/**< Vertical block count	*/
} T_FJ_STILL_TCT_PARAM;

/** TCHS control parameter.
*/
typedef struct {
	E_FJ_STILL_TCHS_HIST_HCYC		sampling_hcycle;			/**< Horizontal sampling interval setting	*/
	E_FJ_STILL_TCHS_HIST_VCYC		sampling_vcycle;			/**< Vertical sampling interval setting		*/
	E_FJ_STILL_TCHS_HIST_BIT_WIDTH	histogram_mode;				/**< RGB histogram mode						*/
	UCHAR							histogram_minus_mode;		/**< RGB histogram minus value mode<br>
																	0 : Does not count minus value.<br>
																	1 : Counts minus value as pixel value equals "0".*/
	USHORT							start_x;					/**< start X position						*/
	USHORT							start_y;					/**< start Y position						*/
	USHORT							hsiz;						/**< Horizontal size						*/
	USHORT							vsiz;						/**< Vertical size							*/
} T_FJ_STILL_TCHS_PARAM;

/** TONE control parameter.
*/
typedef struct {
	E_FJ_STILL_YTC_OUT				ytc_out_sel;			/**< TC YTc output selection					*/
	E_FJ_STILL_TC_INTBIT			int_part_sel;			/**< TC data integral part selection.			*/
	UCHAR							tbl_blend_en;			/**< TC table blend enable						*/
	UCHAR							tbl_blend_ratio;		/**< TC table blend ratio<br>
																 This parameter is the parameter used to perform blend processing on the reference value 1 of TC table 1
																 and the reference value 2 of TC table 2 to generate the reference value which is applied to tone control.<br>
																 Blend equation is defined as follows:
																 Reference value = ((32 - tbl_blend_ratio) * Reference value 1 + tbl_blend_ratio * Reference value 2) / 32	*/
	UCHAR							yc_matrix[3];			/**< TC Y conversion coefficient				*/
	USHORT							clip_p_r;				/**< R component after TC positive direction clip(12bit)	*/
	USHORT							clip_m_r;				/**< R component after TC negative direction clip(12bit)	*/
	USHORT							clip_p_g;				/**< G component after TC positive direction clip(12bit)	*/
	USHORT							clip_m_g;				/**< G component after TC negative direction clip(12bit)	*/
	USHORT							clip_p_b;				/**< B component after TC positive direction clip(12bit)	*/
	USHORT							clip_m_b;				/**< B component after TC negative direction clip(12bit)	*/
} T_FJ_STILL_TONE_PARAM;

/** Gyro config parameter.
*/
typedef struct {
	FJ_CROP_INFO					stab_limits;				/**< The rectangle in pixels representing boundaries within which the output_size is moved in order to provide stabilisation effect.<br>
																	 Typically those boundaries corellate with the boundaries of the original image.			*/
	FLOAT							min_strength;				/**< The min_strength and max_strength parameters determine the range of the stabilisation strength within which the stabilisation filter parameters will be adapted.<br>
																	 Strength 0 means no stabilisation and strength 1 means full stabilisation (image will not move until stabilisation margin is reached).	*/
	FLOAT							max_strength;				/**< See min_strength						*/
	UINT32							frame_interval;				/**< Frame interval (usec)					*/
	UINT32							gyro_interval;				/**< Gyro interval (usec)					*/
	UINT32							accel_interval;				/**< Accel interval (usec)					*/
	UINT32							magnet_interval;			/**< Magnet interval (usec)					*/
	T_IP_EIS_CAMERA_INFO 			camera_info;				/**< Camera Info							*/
} FJ_GYRO_CONFIG;

/** Gyro data sample.
*/
typedef struct {
	FLOAT							x;							/**< X-axis value							*/
	FLOAT							y;							/**< Y-axis value							*/
	FLOAT							z;							/**< Z-axis value							*/
} FJ_GYRO_DATA_SAMPLE;


/** Gyro data.
*/
typedef struct {
	UINT32							fsync;						/**< Frame sync flag (0:ON, 1:OFF)				*/
	UINT32							time_stamp;					/**< Time stamp (usec)							*/
	UINT32							exposure_time;				/**< Current image sensor exposure time (usec)	*/
	FJ_GYRO_DATA_SAMPLE				*gyro;						/**< The pointer to the vector containing gyroscope rates in rad/s. Can be NULL if not used	*/
	FJ_GYRO_DATA_SAMPLE				*accel;						/**< The pointer to the linear acceleration vector in G. Can be NULL if not used			*/
	FJ_GYRO_DATA_SAMPLE				*magnet;					/**< The pointer to the magnetic field vector in uT. Can be NULL if not used				*/
} FJ_GYRO_DATA;

/** Raw window information.
*/
typedef struct {
	USHORT	pre_sclr_left_sta_x;	/**< [Pre-Scaler]  Offset x of target image from sensor window. */
	USHORT	pre_sclr_left_sta_y;	/**< [Pre-Scaler]  Offset y of target image from sensor window. */
	USHORT	pre_sclr_left_width;	/**< [Pre-Scaler]  left image width. */
	USHORT	pre_sclr_right_sta_x;	/**< [Pre-Scaler]  Offset x of target image from sensor window. */
	USHORT	pre_sclr_right_sta_y;	/**< [Pre-Scaler]  Offset y of target image from sensor window. */
	USHORT	pre_sclr_right_width;	/**< [Pre-Scaler]  right image width. */
	USHORT	pre_sclr_overlap;		/**< [Pre-Scaler]  Overlap width. */
	USHORT	pre_sclr_right_blank;	/**< [Pre-Scaler]  right blanking width. */
	USHORT	pre_sclr_lines; 		/**< [Pre-Scaler]  lines. */
	USHORT	post_sclr_left_sta_x;	/**< [Post-Scaler] Offset x of target image from sensor window. */
	USHORT	post_sclr_left_sta_y;	/**< [Post-Scaler] Offset y of target image from sensor window. */
	USHORT	post_sclr_left_width;	/**< [Post-Scaler] left image width. */
	USHORT	post_sclr_right_sta_x;	/**< [Post-Scaler] Offset x of target image from sensor window. */
	USHORT	post_sclr_right_sta_y;	/**< [Post-Scaler] Offset y of target image from sensor window. */
	USHORT	post_sclr_right_width;	/**< [Post-Scaler] right image width. */
	USHORT	post_sclr_overlap;		/**< [Post-Scaler] Overlap width. */
	USHORT	post_sclr_right_blank;	/**< [Post-Scaler] right blanking width. */
	USHORT	post_sclr_lines;		/**< [Post-Scaler] lines. */
} FJ_STILL_RAW_WINDOW_CONFIG;

/** Raw readout timestamp information.
*/
typedef struct {
	UINT64	sof;				/**< Start Of Frame timestamp (per unit of time:usec  1ms = 1000) */
	UINT64	eof;				/**< End Of Frame timestamp (per unit of time:usec  1ms = 1000) */
	UINT64	readout_timestamp;	/**< Raw readout timestamp (per unit of time:usec  1ms = 1000) */
} FJ_STILL_READOUT_TIMESTAMP;

/* @} */	// fj_still_definition group

/** @weakgroup fj_stat_definition
@{*/

/** STAT location type.
*/
typedef enum{
	FJ_STAT_LOCATION_TYPE_NONE = 0,		/**< Not locate */
	FJ_STAT_LOCATION_TYPE_SEN_0,		/**< locate to SEN core0 */
	FJ_STAT_LOCATION_TYPE_SEN_1,		/**< locate to SEN core1 */
	FJ_STAT_LOCATION_TYPE_SEN_2,		/**< locate to SEN core2 */
	FJ_STAT_LOCATION_TYPE_SEN_3,		/**< locate to SEN core3 */
	FJ_STAT_LOCATION_TYPE_SRO_0,		/**< locate to SRO core0 */
	FJ_STAT_LOCATION_TYPE_SRO_1,		/**< locate to SRO core1 */
	FJ_STAT_LOCATION_TYPE_B2B_0,		/**< locate to B2B core0 */
	FJ_STAT_LOCATION_TYPE_B2B_1,		/**< locate to B2B core1 */
	FJ_STAT_LOCATION_TYPE_MAX			// Stopper
} FJ_STAT_LOCATION_TYPE;

/** 3A callback interface	*/
typedef struct {
	USHORT		AeWin_Block_NumH;				/**< AE statistics Window horizontal block number			*/
	USHORT		AeWin_Block_NumV;				/**< AE statistics Window vertical block number				*/
	USHORT		AeWin_Block_SizeH;				/**< AE statistics Window horizontal block size				*/
	USHORT		AeWin_Block_SizeV;				/**< AE statistics Window vertical block size				*/
	USHORT		CurrentFrameNumber;				/**< Virtual number for frame number management of HOST. (associated with specified Statistic_Info member of StartFrameNumber in FJ_SetStatisticPeriod().)<br>
													 This number is incremented from StartFrameNumber by sensr VD.<br>
													 and, this number intializes StartFrameNumber at the timing of 3A processing cycle 	*/
	UCHAR		Current_Pattern;				/**< Current PatternData.( Please refer to Statistic_Info 's PatternData description.)				*/
} AAA_Common_Info;

// FJ_SetStatisticPeriod
/** Statistics Informaion	*/
typedef struct {
	USHORT	StartFrameNumber;		/**< Virtual number for frame number management of HOST.<br>
										 StartID is really a starting frame number that is returned in the AAA_Common_Info struct	*/
	UCHAR*	PatternData;			/**< 3A process pattern character string.<br>
										 About sample setting & operation sequence, please refer to FJ_SetStatisticPeriod().
											<table>
												<tr><th>PatternData</th><th>Description</th></tr>
												<tr><td align="center">"c"</td><td>Cycle : Specification of Cycle operation.</td></tr>
												<tr><td align="center">"d"</td><td>Dummy : Ignore specified frame.</td></tr>
												<tr><td align="center">"e"</td><td>AE : returning FJ_HostVideoAECB() with AE statistic data information.</td></tr>
												<tr><td align="center">"w"</td><td>AWB : returning FJ_HostVideoAWBCB() with AWB statistic data information.</td></tr>
												<tr><td align="center">"f"</td><td>AF : returning FJ_HostAFCB() with AF statistic data information.</td></tr>
												<tr><td align="center">"l"</td><td>FLCK : returning FJ_HostFLCKCB() with Flicker statistic data information.</td></tr>
												<tr><td align="center">"+"</td><td>Multiple pattern specifies in 1 sensor VD.</td></tr>
											</table>
										 
										 	*/
} Statistic_Info;

/**
 * @brief date structure for date
 * @note none
 * @attention none
 */
typedef struct {
	USHORT ad_year;		/* date A.D. year */
	UCHAR month;		/* date month */
	UCHAR day;			/* date day */
	UCHAR hour;			/* date hour */
	UCHAR min;			/* date minute */
	UCHAR sec;			/* date second */
} FJ_STILL_DATE;

/**
 * @brief fraction structure for fraction
 * @note none
 * @attention none
 */
typedef struct {
	ULONG nume;			/**< numerator */
	ULONG denomi;		/**< denominator */
} FJ_STILL_FRACTION;

/**
 * @brief date structure for GPS
 * @note none
 * @attention none
 */
typedef struct {
	UCHAR latitude_ref;
	FJ_STILL_FRACTION latitude[3];
	UCHAR longitude_ref;
	FJ_STILL_FRACTION longitude[3];
	UCHAR img_direction_ref;
	FJ_STILL_FRACTION img_direction;
} FJ_STILL_GPS;

/**
 * @brief date structure for camera information
 * @note none
 * @attention none
 */
typedef struct {
	FJ_STILL_DATE date;
	FJ_STILL_GPS gps;
} FJ_STILL_CAMERA_INFO;



/* @} */	// fj_stat_definition group

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
/** @weakgroup fj_still_api_basic
@{
*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set configuration of Still Capture and Burst Capture.<br>
@param[in]	bConfig		: Configration kinds<br>
@param[in]	wOperand	: Configration operands<br>
							The details are cf. Remarks.
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Fail
@remarks	<strong>Table Relation of bConfig and wOperand</strong>
			<table border=1>
			<tr bgcolor=darkgray><td>bConfig</td><td>Meaning of wOperand</td></tr>
			<tr><td>@ref FJ_CC_STILL_COUNT</td><td>still count<br>
				1    : Single Capture / Still Time Lapse(Switch to Live-View) in @ref FJ_MODE_PHOTO or @ref FJ_MODE_STILL_HDR<br>
				1-30 : Burst Capture in @ref FJ_MODE_BURST_CAPTURE. 31-255 is handled with 30. In case of MFNR, count is 2-6.<br>
				255  : Still Time Lapse(Not switch to Live-View) in @ref FJ_MODE_CONTI_CAPTURE<br>
			</td></tr>
			</table>
*/
extern	FJ_ERR_CODE	FJ_ConfigCapture( UCHAR bConfig, USHORT wOperand );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function specifies one-shot raw outputting during video mode for Debug.
@remarks	After outputting RAW data, BaseFW returns data size and stored address with FJ_HostVideoRawManipulation() function.<br>
@param[in]	output_part	: raw output part. please refer @ref FJ_STILL_RAW_OUTPUT_PART_NONE to @ref FJ_STILL_RAW_OUTPUT_PART_B2B <br>
@param[in]	instance_id	: Instance ID.(sen_id or demosaic_id)<br>
@param[in]	enable		: 1:Enable / 0:Disable
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : (FJ error code according to the specific error)
 */
extern	FJ_ERR_CODE	FJ_SaveRawVideo( UCHAR output_part, UCHAR instance_id, UCHAR enable );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
SG cycle setting API
@param[in]	sen_id	: SEN instance ID.<br>
@param[in]	sg_ctrl	: Pointer to a sg control structure. See @ref FJ_SG_CTRL.
@retval	FJ_ERR_OK : success
@retval	FJ_ERR_NG : fail - FJ error code according to the specific error.
*/
extern	FJ_ERR_CODE	FJ_SetSg( UCHAR sen_id, FJ_SG_CTRL* sg_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
SG Trigger Enable API
@param[in]	sen_id	: SEN instance ID.<br>
@retval	FJ_ERR_OK : success
@retval	FJ_ERR_NG : fail - FJ error code according to the specific error.
*/
extern	FJ_ERR_CODE	FJ_SgStart( UCHAR sen_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
SG Trigger Disable API
@param[in]	sen_id		: SEN instance ID.<br>
@param[in]	stop_mode	: 0:normal stop 1:forced stop
@retval	FJ_ERR_OK : success
@retval	FJ_ERR_NG : fail - FJ error code according to the specific error.
*/
extern	FJ_ERR_CODE	FJ_SgStop( UCHAR sen_id, UCHAR stop_mode );

/** <table><tr><td>Task-independent portion</td><td>o</td></tr></table>
 *	This function get non-liner histogram data( from R2Y ).<br>
 *	@param[in]	demosaic_id		: Demosaic instance ID.<br>
 *	@param[out]	pHistogramBuff	: Used to collect Histogram buffer. The buffer size must be greater than or equal to size(USHORT) * 128.
 *	@retval FJ_ERR_OK : success.
 *	@retval FJ_ERR_NG : fail.
 */
extern	FJ_ERR_CODE	FJ_GetNonLinerHistogram( UCHAR demosaic_id, USHORT* pHistogramBuff );

/** <table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 * This function sets Adaptive Auto Exposure function control parameter.
 * @param[in]	param		: Adaptive Auto Exposure param
 * @attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
 */
extern	FJ_ERR_CODE FJ_Still_Set_Adaptive_Ae_Param( const FJ_STILL_ADAPTIVE_AE_PARAM* const param );

// @cond
/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API opens the Sensor HW (get instance).
 *
 *	@param[in]	sensor_type		: Sensor H/W type. see @ref FJ_SENSOR_ENUM.
 *	@param[out]	sensor_id		: Sensor instance ID.
 *	@retval FJ_ERR_OK : success.
 *	@retval FJ_ERR_NG : fail.
 *	@remarks	Please refer to Open/Close API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Sensor_Open( FJ_SENSOR_ENUM sensor_enum, UCHAR* sensor_id );
// @endcond

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API opens the SEN HW (get instance).
 *
 *	@param[in]	sen_enum	: SEN H/W type. see @ref FJ_SEN_ENUM.
 *	@param[out]	sen_id		: SEN instance ID.
 *	@retval FJ_ERR_OK : success.
 *	@retval FJ_ERR_NG : fail(sen_id is full).
 *	@remarks	Please refer to Open/Close API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Sen_Open( FJ_SEN_ENUM sen_enum, UCHAR* sen_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API closes the SEN HW (release instance).
 *
 *	@param[in]	sen_id		: SEN instance ID.
 *	@retval FJ_ERR_OK : success.
 *	@retval FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	Please refer to Open/Close API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Sen_Close( UCHAR sen_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API initializes the SEN instance.
 *
 *	@param[in]	sen_enum	: SEN H/W type. see @ref FJ_SEN_ENUM.
 *	@retval FJ_ERR_OK : success.
 */
extern	FJ_ERR_CODE FJ_Sen_Init( FJ_SEN_ENUM sen_enum );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API suspends the SEN instance.
 *
 *	@param[in]	sen_id		: SEN instance ID.
 *	@retval FJ_ERR_OK : success.
 *	@retval FJ_ERR_NG : fail(sen_id is not opened).
 *	@retval FJ_ERR_NG_INPUT_PARAM : param err.
 */
extern	FJ_ERR_CODE FJ_Sen_Suspend( UCHAR sen_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API resumes the SEN instance.
 *
 *	@param[in]	sen_id		: SEN instance ID.
 *	@retval FJ_ERR_OK : success.
 *	@retval FJ_ERR_NG : fail(sen_id is not opened).
 *	@retval FJ_ERR_NG_INPUT_PARAM : param err.
 */
extern	FJ_ERR_CODE FJ_Sen_Resume( UCHAR sen_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API opens the Demosaic HW (get instance).
 *
 *	@param[in]	demosaic_enum	: Demosaic H/W type. see @ref FJ_DEMOSAIC_ENUM.
 *	@param[out]	demosaic_id		: Demosaic instance ID.
 *	@retval FJ_ERR_OK : success.
 *	@retval FJ_ERR_NG : fail(demosaic_id is full).
 *	@remarks	Please refer to Open/Close API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Demosaic_Open( FJ_DEMOSAIC_ENUM demosaic_enum, UCHAR* demosaic_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API closes the Demosaic HW (release instance).
 *
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	Please refer to Open/Close API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Demosaic_Close( UCHAR demosaic_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API initializes the Demosaic instance.
 *
 *	@param[in]	demosaic_enum	: Demosaic H/W type. see @ref FJ_DEMOSAIC_ENUM.
 *	@retval		FJ_ERR_OK : success.
 */
extern	FJ_ERR_CODE FJ_Demosaic_Init( FJ_DEMOSAIC_ENUM demosaic_enum );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API suspends the Demosaic instance.
 *
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 */
extern	FJ_ERR_CODE FJ_Demosaic_Suspend( UCHAR demosaic_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API resumes the Demosaic instance.
 *
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 */
extern	FJ_ERR_CODE FJ_Demosaic_Resume( UCHAR demosaic_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API opens the Stitch HW (get instance).
 *
 *	@param[in]	stitch_enum	: Stitch H/W type. see @ref FJ_STITCH_ENUM.
 *	@param[out]	stitch_id		: Stitch instance ID.
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(stitch_id is full).
 *	@remarks	Please refer to Open/Close API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Stitch_Open( FJ_STITCH_ENUM stitch_enum, UCHAR* stitch_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API closes the Stitch HW (release instance).
 *
 *	@param[in]	stitch_id		: Stitch instance ID.
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(stitch_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	Please refer to Open/Close API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Stitch_Close( UCHAR stitch_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API initializes the Stitch instance.
 *
 *	@param[in]	stitch_enum	: Stitch H/W type. see @ref FJ_STITCH_ENUM.
 *	@retval		FJ_ERR_OK : success.
 */
extern	FJ_ERR_CODE FJ_Stitch_Init( FJ_STITCH_ENUM stitch_enum );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API suspends the Stitch instance.
 *
 *	@param[in]	stitch_id		: Stitch instance ID.
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(stitch_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 */
extern	FJ_ERR_CODE FJ_Stitch_Suspend( UCHAR stitch_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API resumes the Stitch instance.
 *
 *	@param[in]	stitch_id		: Stitch instance ID.
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(stitch_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 */
extern	FJ_ERR_CODE FJ_Stitch_Resume( UCHAR stitch_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API is sets the Image Macro Mode.

	<table>
		<tr>
			<th>Mode No.</th>
			<th>Use Case</th>
			<th>Feature of Instance Driving</th>
			<th>Notes</th>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_00</td>
			<td>Still/Burst/Continuous capture</td>
			<td>Please refer to <br>@ref fj_still_image_macro_mode_00</td>
			<td>None</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_01</td>
			<td>Video preview/record<br>(Normal mode)</td>
			<td>Please refer to <br>@ref fj_still_image_macro_mode_01.</td>
			<td>None</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_02</td>
			<td>2 Sensor Connect</td>
			<td>Please refer to <br>@ref fj_still_image_macro_mode_02.</td>
			<td>Stitching 2sensor output images.</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_03</td>
			<td>4 Sensor Connect</td>
			<td>Please refer to <br>@ref fj_still_image_macro_mode_03.</td>
			<td>Recieved 4 Sensor outputs.</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_04</td>
			<td>Video preview/record<br>(High power mode)</td>
			<td>Please refer to <br>@ref fj_still_image_macro_mode_04.</td>
			<td>Using huge memory and high-power processing.</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_05</td>
			<td>Video preview/record<br>(Low power mode)</td>
			<td>Please refer to <br>@ref fj_still_image_macro_mode_05.</td>
			<td>Using tiny memory and low-power processing.</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_06</td>
			<td>(SEN out)<br>- Clean bayer output<br>- Sensor to HDMI Bypass</td>
			<td>Please refer to <br>@ref fj_still_image_macro_mode_06.</td>
			<td>Raw bayer output without any bayer image corrections.</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_07</td>
			<td>(SEN-Demosaic direct out /w BA)<br>- Clean bayer output<br>- Sensor to HDMI Bypass</td>
			<td>Please refer to <br>@ref fj_still_image_macro_mode_07.</td>
			<td>Raw bayer output with bayer image corrections and Bayer Analyzer by PRO and BA via DDR output.</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_08</td>
			<td>(SEN-Demosaic in-direct out)<br>- Clean bayer output<br>- Sensor to HDMI Bypass</td>
			<td>Please refer to <br>@ref fj_still_image_macro_mode_08.</td>
			<td>Raw bayer output with bayer image corrections by PRO via DDR output.</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_09</td>
			<td>Video record w/ PIV</td>
			<td>Please refer to <br>@ref fj_still_image_macro_mode_09.</td>
			<td>Picture In Video mode driven.</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_0A</td>
			<td>Reserved</td>
			<td>-</td>
			<td>-</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_0B</td>
			<td>Reserved</td>
			<td>-</td>
			<td>-</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_0C</td>
			<td>Reserved</td>
			<td>-</td>
			<td>-</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_0D</td>
			<td>Reserved</td>
			<td>-</td>
			<td>-</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_0E</td>
			<td>Reserved</td>
			<td>-</td>
			<td>-</td>
		</tr>
		<tr>
			<td>@ref FJ_STILL_IMAGE_MACRO_MODE_0F</td>
			<td>Reserved</td>
			<td>-</td>
			<td>-</td>
		</tr>
	</table>
 *
 *	@param[in]	macro_mode		: Image Macro Mode see @ref FJ_STILL_IMAGE_MACRO_MODE.
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 */
extern	FJ_ERR_CODE FJ_SetImageMacroMode( FJ_STILL_IMAGE_MACRO_MODE macro_mode );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API connect Sensor to SEN (sensor instance <-> SEN instance).
 *
 *	@param[in]	sensor_id	: Sensor instance ID(source).
 *	@param[in]	sen_id		: SEN instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(sen_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Sensor_Connect( UCHAR sensor_id, UCHAR sen_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API connect SEN to Demosaic(SEN instance <-> Demosaic instance).
 *
 *	@param[in]	sen_id		: SEN instance ID(source).
 *	@param[in]	demosaic_id	: Demosaic instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(sen_id/demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Sen_Connect( UCHAR sen_id, UCHAR demosaic_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API connect SEN to Display(SEN instance <-> Display instance).
 *
 *	@param[in]	sen_id		: SEN instance ID(source).
 *	@param[in]	disp_id		: Disp instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(sen_id/disp_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Sen_Connect_Display( UCHAR sen_id, UCHAR disp_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API conncect Demosaic to Encode(Demosaic instance-output_ch <-> Encode instance).
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID(source).
 *	@param[in]	output_id	: Output channel of 'demossaic_id'.
 *	@param[in]	encode_id	: Encode instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(demosaic_id/encode_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Demosaic_Connect_Encode( UCHAR demosaic_id, UCHAR output_id, UCHAR encode_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API conncect Demosaic to Display(Demosaic instance-outpu_ch <-> Display instance).
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID(source).
 *	@param[in]	output_id	: Output channel of 'demossaic_id'.
 *	@param[in]	disp_id	: Display instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(demosaic_id/disp_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Demosaic_Connect_Display( UCHAR demosaic_id, UCHAR output_id, UCHAR disp_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API conncect Demosaic to Stitch(Demosaic instance-outpu_ch <-> Stitch instance).
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID(source).
 *	@param[in]	output_id	: Output channel of 'demossaic_id'.
 *	@param[in]	stitch_id	: Stitch instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(demosaic_id/stitch_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Demosaic_Connect_Stitch( UCHAR demosaic_id, UCHAR output_id, UCHAR stitch_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API conncect Stitch to Encode(Stitch instance-output_ch <-> Encode instance).
 *
 *	@param[in]	stitch_id	: Stitch instance ID(source).
 *	@param[in]	video_encode_id1	: Encode instance ID(sink). 0xFF is disable.
 *	@param[in]	video_encode_id2	: Encode instance ID(sink). 0xFF is disable.
 *	@param[in]	video_encode_id3	: Encode instance ID(sink). 0xFF is disable.
 *	@param[in]	still_encode_id	: Encode instance ID(sink). 0xFF is disable.
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(stitch_id/video_encode_id/still_encode_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval	FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Stitch_Connect_Encode( UCHAR stitch_id, UCHAR video_encode_id1, UCHAR video_encode_id2, UCHAR video_encode_id3, UCHAR still_encode_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API conncect Stitch to Display(Stitch instance-outpu_ch <-> Display instance).
 *
 *	@param[in]	stitch_id	: Stitch instance ID(source).
 *	@param[in]	disp_id		: Display instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(stitch_id/disp_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Stitch_Connect_Display( UCHAR stitch_id, UCHAR disp_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API disconnect Sensor from SEN (sensor instance <-> SEN instance).
 *
 *	@param[in]	sensor_id	: Sensor instance ID(source).
 *	@param[in]	sen_id	: SEN instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(sen_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Sensor_Disconnect( UCHAR sensor_id, UCHAR sen_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API disconnect SEN from Demosaic(SEN instance <-> Demosaic instance).
 *
 *	@param[in]	sen_id		: SEN instance ID(source).
 *	@param[in]	demosaic_id	: Demosaic instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(sen_id/demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Sen_Disconnect( UCHAR sen_id, UCHAR demosaic_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API disconnect SEN from Encode(SEN instance <-> Disp instance).
 *
 *	@param[in]	sen_id		: SEN instance ID(source).
 *	@param[in]	disp_id		: Disp instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(sen_id/disp_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Sen_Disconnect_Display( UCHAR sen_id, UCHAR disp_id );

extern  FJ_ERR_CODE FJ_Set_Capture_Stitch_Status( FJ_STILL_STITCH_TYPE stitch_type );
extern  FJ_ERR_CODE FJ_Get_Capture_Stitch_Status( FJ_STILL_STITCH_TYPE * stitch_type );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API disconncect Demosaic from Encode(Demosaic instance-output_ch <-> Encode instance).
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID(source).
 *	@param[in]	output_id	: output ID of demosaic instance(source).
 *	@param[in]	encode_id	: Encode instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(demosaic_id/encode_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Demosaic_Disconnect_Encode( UCHAR demosaic_id, UCHAR output_id, UCHAR encode_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API disconncect Demosaic from Display(Demosaic instance-output_ch <-> Display instance).
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID(source).
 *	@param[in]	output_id	: output ID of demosaic instance(source).
 *	@param[in]	disp_id		: Display instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(demosaic_id/disp_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Demosaic_Disconnect_Display( UCHAR demosaic_id, UCHAR output_id, UCHAR disp_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API disconncect Demosaic from Stitch(Demosaic instance-output_ch <-> Stitch instance).
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID(source).
 *	@param[in]	output_id	: output ID of demosaic instance(source).
 *	@param[in]	stitch_id	: Stitch instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(demosaic_id/stitch_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Demosaic_Disconnect_Stitch( UCHAR demosaic_id, UCHAR output_id, UCHAR stitch_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API disconncect Stitch from Encode(Stitch instance-output_ch <-> Encode instance).
 *
 *	@param[in]	stitch_id	: Stitch instance ID(source).
 *	@param[in]	video_encode_id1	: Encode instance ID(sink). 0xFF is disable.
 *	@param[in]	video_encode_id2	: Encode instance ID(sink). 0xFF is disable.
 *	@param[in]	video_encode_id3	: Encode instance ID(sink). 0xFF is disable.
 *	@param[in]	still_encode_id	: Encode instance ID(sink). 0xFF is disable.
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(stitch_id/video_encode_id/still_encode_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Stitch_Disconnect_Encode( UCHAR stitch_id, UCHAR video_encode_id1, UCHAR video_encode_id2, UCHAR video_encode_id3, UCHAR still_encode_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API disconncect Stitch from Display(Stitch instance-output_ch <-> Display instance).
 *
 *	@param[in]	stitch_id	: Stitch instance ID(source).
 *	@param[in]	disp_id		: Display instance ID(sink).
 *	@retval		FJ_ERR_OK : success.
 *	@retval		FJ_ERR_NG : fail(stitch_id/disp_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@retval		FJ_ERR_NG_CONDITION : connection unmatch.
 *	@remarks	Please refer to Connect/Disconnect API from @ref fj_still_api_configuration_connectivity_overview.
 */
extern	FJ_ERR_CODE FJ_Stitch_Disconnect_Display( UCHAR stitch_id, UCHAR disp_id );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets the sensor instance window setting.
 *	@param[in]	sensor_id		: Sensor instance ID.
 *	@param[in]	sensor_info		: Pointer to a sensor information structure.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(sensor_id is not opened).
 *	@remarks	Please refer to @ref fj_still_api_configuration_window.
*/
extern	FJ_ERR_CODE	FJ_Set_Sensor_Window( UCHAR sensor_id, FJ_SENSOR_WINDOW_CONFIG* sensor_info );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets the sensor instance window setting.
 *	@param[in]	sensor_id		: Sensor instance ID.
 *	@param[out]	sensor_info		: Pointer to a sensor information structure.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(sensor_id is not opened).
 *	@remarks	Please refer to @ref fj_still_api_configuration_window.
*/
extern	FJ_ERR_CODE	FJ_Get_Sensor_Window( UCHAR sensor_id, FJ_SENSOR_WINDOW_CONFIG* sensor_info );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets the SEN instance window setting.
 *	@param[in]	sen_id			: SEN instance ID.
 *	@param[in]	window_config	: Pointer to a window configuration structure.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(sen_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	Please refer to @ref fj_still_api_configuration_window.
*/
extern	FJ_ERR_CODE	FJ_Set_Sen_Window( UCHAR sen_id, FJ_SEN_WINDOW_CONFIG* window_config );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets the SEN instance window setting.
 *	@param[in]	sen_id			: SEN instance ID.
 *	@param[out]	window_config	: Pointer to a window configuration structure.
 *	@retval		FJ_ERR_NG : fail(sen_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	Please refer to @ref fj_still_api_configuration_window.
*/
extern	FJ_ERR_CODE	FJ_Get_Sen_Window( UCHAR sen_id, FJ_SEN_WINDOW_CONFIG* window_config );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets the Demosaic instance window setting.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	window_config	: Pointer to a window configuration structure.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	Please refer to @ref fj_still_api_configuration_window.
*/
extern	FJ_ERR_CODE	FJ_Set_Demosaic_Window( UCHAR demosaic_id, FJ_DEMOSAIC_WINDOW_CONFIG* window_config );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets the Demosaic instance window setting.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[out]	window_config	: Pointer to a window configuration structure.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	Please refer to @ref fj_still_api_configuration_window.
*/
extern	FJ_ERR_CODE	FJ_Get_Demosaic_Window( UCHAR demosaic_id, FJ_DEMOSAIC_WINDOW_CONFIG* window_config );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets the Stitch instance window setting.
 *	@param[in]	stitch_id		: Stitch instance ID.
 *	@param[in]	window_config	: Pointer to a window configuration structure.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(stitch_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	For each parameter, please refer to following images.<br>
 *				@image html fj_still_stitch_config.png
*/
extern	FJ_ERR_CODE	FJ_Set_Stitch_Window( UCHAR stitch_id, FJ_STITCH_WINDOW_CONFIG* window_config );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets the Stitch instance window setting.
 *	@param[in]	stitch_id		: Stitch instance ID.
 *	@param[out]	window_config	: Pointer to a window configuration structure.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(stitch_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	For each parameter, please refer to following images.<br>
 *				@image html fj_still_stitch_config.png
*/
extern	FJ_ERR_CODE	FJ_Get_Stitch_Window( UCHAR stitch_id, FJ_STITCH_WINDOW_CONFIG* window_config );


// Moved to ENCODE Module start..
/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets the Encode instance window setting.
 *	@param[in]	encode_id		: Encode instance ID.
 *	@param[in]	window_config	: Pointer to a window configuration structure.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(encode_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	Please refer to @ref fj_still_api_configuration_window.
*/
extern	FJ_ERR_CODE	FJ_Set_Encode_Window( UCHAR encode_id, FJ_ENCODE_WINDOW_CONFIG* window_config );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets the Encode instance window setting.
 *	@param[in]	encode_id		: Encode instance ID.
 *	@param[out]	window_config	: Pointer to a window configuration structure.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(encode_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	Please refer to @ref fj_still_api_configuration_window.
*/
extern	FJ_ERR_CODE	FJ_Get_Encode_Window( UCHAR encode_id, FJ_ENCODE_WINDOW_CONFIG* window_config );
// Moved to ENCODE Module ..end

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets the Demosaic instance input control configuration.
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[in]	row_division_num	: Row division number of still processing in PIV mode.(1 - 8)
 *	@param[in]	column_division_num	: Column division number of still processing in PIV mode.(1 - 8)
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	In PIV mode only, can be specified division processing count of demosaic.<br>
 				Please set a total division number( row_division_num * column_division_num ) less than or equal to 32.<br>
				About division processing, please refer to @ref fj_still_api_configuration_window and following images.<br>
				@image html fj_still_connectivity_demosaic_input_control.png
*/
extern	FJ_ERR_CODE	FJ_SetDemosaicInputControl( UCHAR demosaic_id, UCHAR row_division_num, UCHAR column_division_num );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets the Demosaic instance input control configuration.
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[out]	row_division_num	: Row division number of still processing in PIV mode.
 *	@param[out]	column_division_num	: Column division number of still processing in PIV mode.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GetDemosaicInputControl( UCHAR demosaic_id, UCHAR* row_division_num, UCHAR* column_division_num );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets the Demosaic instance output control configuration. <br>
 *  
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[in]	output_id			: Demosaic output ID.
 *	@param[in]	initiate_cycle		: Initiate cycle. See @ref FJ_DEMOSAIC_OUTPUT.
 *	@param[in]	sampling_count		: Sampling count for cycle shot(1/n).
 *	@param[in]	image_format		: Demosaic output image format. See @ref FJ_DEMOSAIC_OUTPUT_IMG_PFMT.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	Please refer to Demosaic Output Control API from @ref fj_still_api_configuration_connectivity_overview.
*/
extern	FJ_ERR_CODE	FJ_SetDemosaicOutputControl( UCHAR demosaic_id, UCHAR output_id, FJ_DEMOSAIC_OUTPUT initiate_cycle, UINT32 sampling_count, FJ_DEMOSAIC_OUTPUT_IMG_PFMT image_format );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets the Demosaic instance output control configuration. <br>
 *  
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[in]	output_id			: Demosaic output ID.
 *	@param[out]	initiate_cycle		: Initiate cycle. See @ref FJ_DEMOSAIC_OUTPUT.
 *	@param[out]	sampling_count		: Sampling count for cycle shot(1/n).
 *	@param[out]	image_format		: Demosaic output image format. See @ref FJ_DEMOSAIC_OUTPUT_IMG_PFMT.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	Please refer to Demosaic Output Control API from @ref fj_still_api_configuration_connectivity_overview.
*/
extern	FJ_ERR_CODE	FJ_GetDemosaicOutputControl( UCHAR demosaic_id, UCHAR output_id, FJ_DEMOSAIC_OUTPUT* initiate_cycle, UINT32* sampling_count, FJ_DEMOSAIC_OUTPUT_IMG_PFMT* image_format );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets whether 3DNR processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	enabled			: TRUE: Enable   FALSE: Disable
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_SetDemosaic3dnrEnabled( UCHAR demosaic_id, BOOL enabled );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets whether 3DNR processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[out]	enabled			: TRUE: Enable   FALSE: Disable
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GetDemosaic3dnrEnabled( UCHAR demosaic_id, BOOL* enabled );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets 3DNR configuration in demosaic block.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	config			: Pointer to a 3DNR configuration structure. see @ref FJ_STILL_3DNR_CONFIG.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	About effective block of these settings,<br>
 				please refer to @ref fj_still_api_configuration_connectivity_overview and following images.<br>
				@image html fj_still_3dnr_config.png
				Automatic mode(FJ_STILL_3DNR_MODE_AUTO) is used when EIS is enabled.
*/
extern	FJ_ERR_CODE	FJ_SetDemosaic3dnrConfig( UCHAR demosaic_id, FJ_STILL_3DNR_CONFIG* config );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets 3DNR configuration in demosaic block.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[out]	config			: Pointer to a 3DNR configuration structure. see @ref FJ_STILL_3DNR_CONFIG.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GetDemosaic3dnrConfig( UCHAR demosaic_id, FJ_STILL_3DNR_CONFIG* config );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets whether LTM processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	enabled			: TRUE: Enable   FALSE: Disable
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_SetDemosaicLtmEnabled( UCHAR demosaic_id, BOOL enabled );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets whether LTM processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[out]	enabled			: TRUE: Enable   FALSE: Disable
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GetDemosaicLtmEnabled( UCHAR demosaic_id, BOOL* enabled );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets whether SHDR processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	enabled			: TRUE: Enable   FALSE: Disable
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_SetDemosaicShdrEnabled( UCHAR demosaic_id, BOOL enabled );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets whether SHDR processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[out]	enabled			: TRUE: Enable   FALSE: Disable
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GetDemosaicShdrEnabled( UCHAR demosaic_id, BOOL* enabled );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets the SHDR configuration.
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[in]	config		: Pointer to a configuration structure. See @ref FJ_STILL_SHDR_CONFIG.
 *	@retval		FJ_ERR_OK	: success
 *	@retval		FJ_ERR_NG	: fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_SetDemosaicShdrConfig( UCHAR demosaic_id, FJ_STILL_SHDR_CONFIG* config );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets the SHDR configuration.
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[out]	config		: Pointer to a configuration structure. See @ref FJ_STILL_SHDR_CONFIG.
 *	@retval		FJ_ERR_OK	: success
 *	@retval		FJ_ERR_NG	: fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GetDemosaicShdrConfig( UCHAR demosaic_id, FJ_STILL_SHDR_CONFIG* config );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets whether Ahip processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	enabled			: TRUE: Enable   FALSE: Disable
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_SetDemosaicAhipEnabled( UCHAR demosaic_id, BOOL enabled );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets whether Ahip processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[out]	enabled			: TRUE: Enable   FALSE: Disable
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GetDemosaicAhipEnabled( UCHAR demosaic_id, BOOL* enabled );

/**	<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
 *	This API sets whether Warp processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	enabled			: TRUE: Enable   FALSE: Disable
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
FJ_ERR_CODE	FJ_SetDemosaicWarpEnabled( UCHAR demosaic_id, BOOL enabled );

/**	<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
 *	This API gets whether Warp processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[out]	enabled			: TRUE: Enable   FALSE: Disable
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
FJ_ERR_CODE	FJ_GetDemosaicWarpEnabled( UCHAR demosaic_id, BOOL* enabled );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API loads the warping table binary to BaseFW internal table memory area.
 *
 *	@param[in]	tbl_idx	: Warping table index number.
 *	@param[in]	addr	: Pointer of Warping table binary data.
 *	@param[in]	bytes	: Size of Warping table binary data.
 *	@retval		FJ_ERR_OK			Load successful.
 *	@retval		FJ_ERR_NG_FORMAT	Binary data format is unknown.
 *	@retval		FJ_ERR_OVERFLOW		Binary data size is learge.
 *	@retval		FJ_ERR_NG_MEMORY	BaseFW internal table memory area is full.
 *	@par Sample code:
 *				Please refer the sample code of FJ_Warp_Calc_GeTable().
 */
extern FJ_ERR_CODE FJ_Notify_Loading_WarpTable_Complete( UINT32 tbl_idx, const BYTE* addr, ULONG bytes );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API calculate SGDE table from warping table binary.
 *
 *	@param[in]		tbl_idx		: Warping table index number.
 *	@param[in,out]	ge_tbl_info	: Pointer of to Calculated SGDE table information.
 *	@param[in]		calc_param	: Rectangle parameter.
 *	@retval		FJ_ERR_OK				Calculated successful.
 *	@retval		FJ_ERR_OVERFLOW			SGDE table memory is small.
 *	@retval		FJ_ERR_NG_INPUT_PARAM	Not loaded Warping table specified tbl_idx.
 *	@par Sample code:
 *	@code
#include "fj_still.h"
#include <stdio.h>

#define GE_TBL_ARRAY_NUM	(2500)
FJ_GE_CELL		ge_tbl[GE_TBL_ARRAY_NUM];
FJ_GE_TBL_INFO	ge_tbl_info;

VOID calc_sgde_table( VOID )
{
	FJ_ERR_CODE	fj_ercd;
	UINT32		sv_tbl_idx = 0;
	FJ_WARP_CALC_PARAM calc_param = {
		.crop_sta_x = 0,
		.crop_sta_y = 0,
		.crop_width = 1440,
		.crop_lines = 1080,
		.in_img_width = 1440,
		.in_img_lines = 1080,
		.out_img_width = 1920,
		.out_img_lines = 1080,
	};

	user_func_warping_tbl_load_to_sdram( addr, &bytes );

	fj_ercd = FJ_Notify_Loading_WarpTable_Complete( sv_tbl_idx, addr, bytes );
	if( fj_ercd != FJ_ERR_OK ) {
		// Error correction code here.
		return;
	}

	ge_tbl_info.ge_tbl_addr = &ge_tbl[0];
	ge_tbl_info.ge_tbl_max_bytes = sizeof(FJ_GE_CELL) * GE_TBL_ARRAY_NUM;

	fj_ercd = FJ_Warp_Calc_GeTable( sv_tbl_idx, &ge_tbl_info, &calc_param );
	if( fj_ercd != FJ_ERR_OK ) {
		// Error correction code here.
		return;
	}

	printf( "SGDE tbl bytes = %u\n", ge_tbl_info.ge_tbl_bytes );
	printf( "SGDE tbl num = %ux%u\n", ge_tbl_info.width, ge_tbl_info.lines );
	printf( "SGDE tbl cell size = %ux%u\n", ge_tbl_info.pitchX, ge_tbl_info.pitchY );
}
 *	@endcode
 */
extern FJ_ERR_CODE FJ_Warp_Calc_GeTable( UINT32 tbl_idx, FJ_GE_TBL_INFO* ge_tbl_info, const FJ_WARP_CALC_PARAM* calc_param );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets the Raw output in the Demosaic.
 *	@param[in]	demosaic_id		: demosaic instance ID.
 *	@param[in]	raw_out_part	: Raw Output Part. <br>
 * 		@ref FJ_STILL_RAW_OUTPUT_PART_NONE : not output <br>
 * 		@ref FJ_STILL_RAW_OUTPUT_PART_SRO  : output from SRO <br>
 * 		@ref FJ_STILL_RAW_OUTPUT_PART_B2B  : output from B2B <br>
 *		It is possible to set like "FJ_STILL_RAW_OUTPUT_PART_SRO | FJ_STILL_RAW_OUTPUT_PART_B2B" <br>
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_SetDemosaicRawOutPart( UCHAR demosaic_id, UCHAR raw_out_part );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets the Raw output in the Demosaic.
 *	@param[in]	demosaic_id		: demosaic instance ID.
 *	@param[out]	raw_out_part	: Raw Output Part. <br>
 * 		@ref FJ_STILL_RAW_OUTPUT_PART_NONE : not output <br>
 * 		@ref FJ_STILL_RAW_OUTPUT_PART_SRO  : output from SRO <br>
 * 		@ref FJ_STILL_RAW_OUTPUT_PART_B2B  : output from B2B <br>
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GetDemosaicRawOutPart( UCHAR demosaic_id, UCHAR* raw_out_part );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets the SEN configuration.
 *	@param[in]	sen_id		: SEN instance ID.
 *	@param[in]	sen_config	: Pointer to a configuration structure. See @ref FJ_STILL_SEN_CONFIG.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(sen_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_SetSenConfig( UCHAR sen_id, FJ_STILL_SEN_CONFIG* sen_config );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets the SEN configuration.
 *	@param[in]	sen_id		: SEN instance ID.
 *	@param[out]	sen_config	: Pointer to a configuration structure. See @ref FJ_STILL_SEN_CONFIG.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(sen_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GetSenConfig( UCHAR sen_id, FJ_STILL_SEN_CONFIG* sen_config );

// Moved to ENCODE Module
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Set Jpeg Encode Quality for the next captured image.<br>
 *	@remarks	<strong>Table Relation of bQuality and wOperand</strong>
				<table border=1>
					<tr bgcolor=darkgray><td>bQuality</td><td>Meaning of wOperand</td></tr>
					<tr><td>FJ_JPEG_QUALITY_KIND_Q </td><td>Quality          <br>Quality value [0..100]<br></td></tr>
					<tr><td>FJ_JPEG_QUALITY_KIND_CR</td><td>Compression Ratio<br>See @ref FJ_COMPRESSION_RATIO<br></td></tr>
				</table>
 *	@param[in]	encode_id	: encode instance id<br>
 *	@param[in]	bQuality	: Quality kinds. See @ref FJ_JPEG_QUALITY_KIND<br>
 *	@param[in]	wOperand	: Quality operands. The details are cf. Remarks.<br>
 *	@retval		FJ_ERR_OK	: success.
 *	@retval		FJ_ERR_NG	: fail(encode_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_SetJpegEncodeQuality( UCHAR encode_id, FJ_JPEG_QUALITY_KIND bQuality, USHORT wOperand );

// Moved to ENCODE Module
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Get Jpeg Encode Quality for the next captured image.<br>
 *	@param[in]	encode_id	: encode instance id<br>
 *	@param[out]	bQuality	: Quality kinds. See @ref FJ_JPEG_QUALITY_KIND<br>
 *	@param[out]	wOperand	: Quality operands. The details are cf. Remarks.<br>
 *	@retval		FJ_ERR_OK	: success.
 *	@retval		FJ_ERR_NG	: fail(encode_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks	<strong>Table Relation of bQuality and wOperand</strong>
				<table border=1>
					<tr bgcolor=darkgray><td>bQuality</td><td>Meaning of wOperand</td></tr>
					<tr><td>FJ_JPEG_QUALITY_KIND_Q </td><td>Quality          <br>Quality value [0..100]. 101- is handled with 100.<br></td></tr>
					<tr><td>FJ_JPEG_QUALITY_KIND_CR</td><td>Compression Ratio<br>See @ref FJ_COMPRESSION_RATIO<br></td></tr>
				</table>
*/
extern	FJ_ERR_CODE	FJ_GetJpegEncodeQuality( UCHAR encode_id, FJ_JPEG_QUALITY_KIND* bQuality, USHORT* wOperand );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API loads the LTM map.
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[in]	map_ctrl	: LTM map control. See @ref FJ_LTM_MAP_CTRL
 *	@param[in]	map_addr	: LTM map address. if map_ctrl is FJ_LTM_MAP_CTRL_AUTO, this parameter is invalid.
 *	@retval		FJ_ERR_OK	: success
 *	@retval		FJ_ERR_NG	: fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_LoadLtmMap( UCHAR demosaic_id, FJ_LTM_MAP_CTRL map_ctrl, UINT32 map_addr );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API set genarate the JPEG file for normal image in Still HDR.
 *	@param[in]	demosaic_id				: Demosaic instance ID.
 *	@param[in]	normal_img_jpeg_onoff	: Normal image jpeg for create On/Off status.
 *	@retval		FJ_ERR_OK	: success
 *	@retval		FJ_ERR_NG	: fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GenerateJpegShdr( UCHAR demosaic_id, BOOL normal_img_jpeg_onoff );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API indicate sensor reconfiguration to BaseFW.<br>
 *	and it can use during video mode.<br>
 *	About sequence of sensor reconfiguration, please refer to @ref fj_still_capture_sequence_section7 & @ref fj_still_capture_sequence_section1.
 *	@param[in]	sen_id          : SEN instance ID.
 *	@param[in]	pre_frame_rate  : Frame rate of before sensor mode changed.(ex. 29.97fps -> D'2997, 30fps -> D'3000)
 *	@param[in]	post_frame_rate : Frame rate of after sensor mode changed.(ex. 29.97fps -> D'2997, 30fps -> D'3000)
 *	@retval		FJ_ERR_OK	: success
 *	@retval		FJ_ERR_NG	: fail(sen_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
 *	@remarks Image processing timming chart is as following.<br>
		@image html fj_reconfigure_sensor_timing_chart.png<br>
*/
extern	FJ_ERR_CODE	FJ_Reconfigure_Sensor( UCHAR sen_id, UINT32 pre_frame_rate, UINT32 post_frame_rate );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets whether Demosaic Flipping and Mirroring are enabled.
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[in]	flip_enabled	: TRUE: flip  FALSE: not flip
 *	@param[in]	mirror_enabled	: TRUE: mirror  FALSE: not mirror
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern FJ_ERR_CODE	FJ_SetDemosaicFlipMirrorEnabled( UCHAR demosaic_id, BOOL flip_enabled, BOOL mirror_enabled );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets whether Demosaic Flipping and Mirroring are enabled.
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[out]	flip_enabled	: TRUE: flip  FALSE: not flip
 *	@param[out]	mirror_enabled	: TRUE: mirror  FALSE: not mirror
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern FJ_ERR_CODE	FJ_GetDemosaicFlipMirrorEnabled( UCHAR demosaic_id, BOOL* flip_enabled, BOOL* mirror_enabled );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This function get a OB wave detect value of latest readout frame.
 *	@param[in]	sen_id  : SEN instance ID.
 *	@param[out]	obdata : OB wavedetect value.
 *	@retval FJ_ERR_OK : success.<br>
 *	@retval FJ_ERR_NG : fail.<br>
 *	Sample code is following.<br>
 *	@code
#include "fj_still.h"

// for Photo/Calibration mode.
VOID FJ_HostStillRawManipulation( UCHAR inst_id, FJ_RAW_MANI_STILL mani_no, PSRAWINFO psInfo, USHORT* puwScalers )
{
	FJ_STILL_OB_VALUE obdata;
	FJ_ERR_CODE fj_ercd;

	// Get OB wave detect value.
	if( mani_no == FJ_RAW_MANI_STILL_POST_SEN ) {
		fj_ercd = FJ_Still_Get_ObWaveDetectValue( inst_id, &obdata );
		if( fj_ercd != FJ_ERR_OK ) {
			// Error correction code here.
		}
		else {
			printf( "OB R=%u Gr=%u Gb=%u B=%u\n",
				obdata.R,
				obdata.Gr,
				obdata.Gb,
				obdata.B
			);
		}
	}
}

// for Video mode.
VOID FJ_HostVideoYUVManipulation( UCHAR demosaic_id, UCHAR output_id, FJ_YUV_MANI_VIDEO mani_no, PSYUVINFO psInfo )
{
	FJ_STILL_OB_VALUE obdata;
	FJ_ERR_CODE fj_ercd;
	UCHAR sen_id = 0;

	// Get OB wave detect value of latest readouting frame.
	fj_ercd = FJ_Still_Get_ObWaveDetectValue( sen_id, &obdata );
	if( fj_ercd != FJ_ERR_OK ) {
		// Error correction code here.
	}
	else {
		printf( "OB R=%u Gr=%u Gb=%u B=%u\n",
			obdata.R,
			obdata.Gr,
			obdata.Gb,
			obdata.B
		);
	}
}
 *	@endcode
 */
extern	FJ_ERR_CODE	FJ_Still_Get_ObWaveDetectValue( UCHAR sen_id, FJ_STILL_OB_VALUE* const obdata );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets histogram(before ToneControl) buffer's top address.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	data_type		: type. @ref E_FJ_STILL_BTC_HIST_TBL.
 *	@param[out]	hist_buffer		: Histogram buffer
 *	@param[out]	overflow_flg	: Overflow flg
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern FJ_ERR_CODE	FJ_Still_GetBeforeTCHistogram( UCHAR demosaic_id, E_FJ_STILL_BTC_HIST_TBL data_type, USHORT* hist_buffer, UCHAR* overflow_flg );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets luminance evaluation(before ToneControl) buffer's top address.
 *	@param[in]	demosaic_id			: Demosaic instance ID.
 *	@param[out]	luminance_buffer	: Luminance evaluation buffer
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern FJ_ERR_CODE	FJ_Still_GetBeforeTCLuminanceEvaluation( UCHAR demosaic_id, UINT32* luminance_buffer );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets Tone control table.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	tone_tbl		: Pointer of TONE table data
 *	@param[in]	table_resol		: TONE table reference resolution
 *	@param[in]	table_end_point	: TONE table End point
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern FJ_ERR_CODE	FJ_Still_SetToneTable( UCHAR demosaic_id, USHORT* tone_tbl, E_FJ_STILL_TC_REF_RESOLUTION table_resol, USHORT table_end_point );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets RGB offset.
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[out]	rgb_offset	: RGB offset. @ref T_FJ_STILL_RGB_OFFSET.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern FJ_ERR_CODE	FJ_Still_SetRgbOffset( UCHAR demosaic_id, T_FJ_STILL_RGB_OFFSET* rgb_offset );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets TCT parameter.
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[out]	tct_param	: TCT control parameter. @ref T_FJ_STILL_TCT_PARAM.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern FJ_ERR_CODE	FJ_Still_SetTctParam( UCHAR demosaic_id, T_FJ_STILL_TCT_PARAM* tct_param );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets TCHS parameter.
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[out]	tchs_param	: TCHS control parameter. @ref T_FJ_STILL_TCHS_PARAM.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern FJ_ERR_CODE	FJ_Still_SetTchsParam( UCHAR demosaic_id, T_FJ_STILL_TCHS_PARAM* tchs_param );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets TONE parameter.
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[out]	tone_param	: TONE control parameter. @ref T_FJ_STILL_TONE_PARAM.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern FJ_ERR_CODE	FJ_Still_SetToneParam( UCHAR demosaic_id, T_FJ_STILL_TONE_PARAM* tone_param );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets global window size for TCT/TCHS.
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[out]	global_width	: Global width.
 *	@param[out]	global_lines	: Global lines.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern FJ_ERR_CODE	FJ_Still_GetTctTchsGlobalWindowSize( UCHAR demosaic_id, USHORT* global_width, USHORT* global_lines );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API sets whether RawScaler processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[in]	scaler_sel		: Raw Scaler selection parameter.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_SetDemosaicRawScalerEnabled( UCHAR demosaic_id, FJ_STILL_RAW_SCALER_SEL scaler_sel );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This API gets whether RawScaler processing is enabled.
 *	@param[in]	demosaic_id		: Demosaic instance ID.
 *	@param[out]	scaler_sel		: Raw Scaler selection parameter.
 *	@retval		FJ_ERR_OK : success
 *	@retval		FJ_ERR_NG : fail(demosaic_id is not opened).
 *	@retval		FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GetDemosaicRawScalerEnabled( UCHAR demosaic_id, FJ_STILL_RAW_SCALER_SEL* scaler_sel );

/** <table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *  This API gets the Raw window configuration.
 *  @param[in]  sen_id          : SEN instance ID.
 *  @param[out] raw_window_cfg  : Pointer to a configuration structure. See @ref FJ_STILL_RAW_WINDOW_CONFIG.
 *  @retval     FJ_ERR_OK : success
 *  @retval     FJ_ERR_NG : fail(sen_id is not opened).
 *  @retval     FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_GetRawWindowConfig ( UCHAR sen_id, FJ_STILL_RAW_WINDOW_CONFIG* raw_window_cfg );

/** <table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *  This API gets the sensor readout timestamp.
 *  @param[in]  sen_id            : SEN instance ID.
 *  @param[in]  frame_no          : Frame no.
 *  @param[out] readout_timestamp : Pointer to a timestamp variable. (per unit of time:usec  1ms = 1000)
 *  @retval     FJ_ERR_OK : success
 *  @retval     FJ_ERR_NG : fail(sen_id is not opened).
 *  @retval     FJ_ERR_NG_INPUT_PARAM : param err.
*/
extern	FJ_ERR_CODE	FJ_Still_GetReadoutTimestamp( UCHAR sen_id, UINT64 frame_no, FJ_STILL_READOUT_TIMESTAMP* readout_timestamp );

/* @} */	// fj_still_api_basic group

/** @weakgroup fj_stat_api
@{
*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set 3A statistics sequence in video mode.<br>
By this API, Host can define the types of statistics is wants to receive from Base FW .<br>
Host can define the sequence of statistics data type to be served.<br>
Host can also define the OS_USER_ID number for the first statistics defined the sequence.<br>
Each following statistics slot will be accompanied with an OS_USER_ID number one higher than slot preceding it.<br>
@image html fj_ae_3A_statistic_sequence-1.png "Fig. statistics sequence example-1"
@image html fj_ae_3A_statistic_sequence-2.png "Fig. statistics sequence example-2"
@image html fj_ae_3A_statistic_sequence-3.png "Fig. statistics sequence example-3"
@remarks	3A system has AE and AWB statistics data of latest four frames.
@param[in]	inst_id		: ID of instance to configure.<br>
						  stat_locate is 'FJ_STAT_LOCATION_TYPE_SEN', inst_id should be 'SEN instance ID',<br>
						  stat_locate is other, inst_id should be 'Demosaic instance ID'.
@param[in]	stat_locate	: STAT location type. see @ref FJ_STAT_LOCATION_TYPE.
@param[in]	SchData	: Statistics information. See @ref Statistic_Info
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM : Input Parameter error
 */
extern FJ_ERR_CODE FJ_SetStatisticPeriod( UCHAR inst_id, FJ_STAT_LOCATION_TYPE stat_locate, Statistic_Info SchData );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function configure AEAWB in STAT.
@param[in]	aeawb_id	: AEAWB ID. value range is [0..3]
@param[in]	inst_id		: ID of instance to configure.<br>
						  stat_locate is 'FJ_STAT_LOCATION_TYPE_SEN', inst_id should be 'SEN instance ID',<br>
						  stat_locate is other, inst_id should be 'Demosaic instance ID'.
@param[in]	stat_locate	: STAT location type. see @ref FJ_STAT_LOCATION_TYPE.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
*/
extern FJ_ERR_CODE FJ_SetStatAeAwbConfig( UCHAR aeawb_id, UCHAR inst_id, FJ_STAT_LOCATION_TYPE stat_locate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get configuration of AEAWB in STAT.
@param[in]	aeawb_id	: AEAWB ID. value range is [0..3]
@param[out]	inst_id		: ID of instance to configure.<br>
						  stat_locate is 'FJ_STAT_LOCATION_TYPE_SEN', inst_id should be 'SEN instance ID',<br>
						  stat_locate is other, inst_id should be 'Demosaic instance ID'.
@param[out]	stat_locate	: STAT location type. see @ref FJ_STAT_LOCATION_TYPE.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
*/
extern FJ_ERR_CODE FJ_GetStatAeAwbConfig( UCHAR aeawb_id, UCHAR* inst_id, FJ_STAT_LOCATION_TYPE* stat_locate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function configure AF in STAT.
@param[in]	af_id		: AF ID. value range is [0..1]
@param[in]	inst_id		: ID of instance to configure.<br>
						  stat_locate is 'FJ_STAT_LOCATION_TYPE_SEN', inst_id should be 'SEN instance ID',<br>
						  stat_locate is other, inst_id should be 'Demosaic instance ID'.
@param[in]	stat_locate : STAT location type. see @ref FJ_STAT_LOCATION_TYPE.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
*/
extern FJ_ERR_CODE FJ_SetStatAfConfig( UCHAR af_id, UCHAR inst_id, FJ_STAT_LOCATION_TYPE stat_locate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get configuration of AF in STAT.
@param[in]	af_id		: AF ID. value range is [0..1]
@param[out]	inst_id		: ID of instance to configure.<br>
						  stat_locate is 'FJ_STAT_LOCATION_TYPE_SEN', inst_id should be 'SEN instance ID',<br>
						  stat_locate is other, inst_id should be 'Demosaic instance ID'.
@param[out]	stat_locate : STAT location type. see @ref FJ_STAT_LOCATION_TYPE.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
*/
extern FJ_ERR_CODE FJ_GetStatAfConfig( UCHAR af_id, UCHAR* inst_id, FJ_STAT_LOCATION_TYPE* stat_locate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function configure FLCK in STAT.
@param[in]	flck_id		: FLCK ID. value range is [0..3]
@param[in]	inst_id		: ID of instance to configure.<br>
						  stat_locate is 'FJ_STAT_LOCATION_TYPE_SEN', inst_id should be 'SEN instance ID',<br>
						  stat_locate is other, inst_id should be 'Demosaic instance ID'.
@param[in]	stat_locate	: STAT location type. see @ref FJ_STAT_LOCATION_TYPE.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
*/
extern FJ_ERR_CODE FJ_SetStatFlckConfig( UCHAR flck_id, UCHAR inst_id, FJ_STAT_LOCATION_TYPE stat_locate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get configuration of FLCK in STAT.
@param[in]	flck_id		: FLCK ID. value range is [0..3]
@param[out]	inst_id		: ID of instance to configure.<br>
						  stat_locate is 'FJ_STAT_LOCATION_TYPE_SEN', inst_id should be 'SEN instance ID',<br>
						  stat_locate is other, inst_id should be 'Demosaic instance ID'.
@param[out]	stat_locate	: STAT location type. see @ref FJ_STAT_LOCATION_TYPE.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
*/
extern FJ_ERR_CODE FJ_GetStatFlckConfig( UCHAR flck_id, FJ_STAT_LOCATION_TYPE* stat_locate, UCHAR* inst_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function configure HIST in STAT.
@param[in]	hist_id		: HIST ID. value range is [0..1]
@param[in]	inst_id		: ID of instance to configure.<br>
						  stat_locate is 'FJ_STAT_LOCATION_TYPE_SEN', inst_id should be 'SEN instance ID',<br>
						  stat_locate is other, inst_id should be 'Demosaic instance ID'.
@param[in]	stat_locate	: STAT location type. see @ref FJ_STAT_LOCATION_TYPE.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
*/
extern FJ_ERR_CODE FJ_SetStatHistConfig( UCHAR hist_id, UCHAR inst_id, FJ_STAT_LOCATION_TYPE stat_locate );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get configuration of HIST in STAT.
@param[in]	hist_id		: HIST ID. value range is [0..1]
@param[out]	inst_id		: ID of instance to configure.<br>
						  stat_locate is 'FJ_STAT_LOCATION_TYPE_SEN', inst_id should be 'SEN instance ID',<br>
						  stat_locate is other, inst_id should be 'Demosaic instance ID'.
@param[out]	stat_locate	: STAT location type. see @ref FJ_STAT_LOCATION_TYPE.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
*/
extern FJ_ERR_CODE FJ_GetStatHistConfig( UCHAR hist_id, FJ_STAT_LOCATION_TYPE* stat_locate, UCHAR* inst_id );

/* @} */	// fj_stat_api group

/** @weakgroup fj_still_api_utility
@{*/
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function execute an SPecial Filter(=SPF) function.
@param[in]	spf_ctrl_info	: SPF control parameter. see @ref FJ_IMG_SPF_CTRL.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
*/
extern	FJ_ERR_CODE	FJ_ImageSPF( FJ_IMG_SPF_CTRL* spf_ctrl_info );

// @cond
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function copy an image using Xch.
@param[in]	src_addr	: src image top address
@param[in]	dst_addr	: dst image top address
@param[in]	width		: image width
@param[in]	lines		: image lines
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
@remarks This function has following limirations.<br>
		 - This function only works in FJ_HostPanoramaManipulation()<br>
		 - This function only works on PanoramaRec mode<br>
		 - <br>
 */
extern	FJ_ERR_CODE	FJ_ImageCopy( UINT32 src_addr, UINT32 dst_addr, UINT32 width, UINT32 lines );
// @endcond

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function copy an YCC Image using Xch
@param[in]	src	: src image information
@param[in]	dst	: dst image information
@retval	FJ_ERR_OK : success
@retval	FJ_ERR_NG : fail
 */
extern	FJ_ERR_CODE	FJ_ImageCopyYcc( FJ_IMG_INFO* src, FJ_IMG_INFO* dst );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function convert an image format, expand and reduct an image.
@param[in]	conv_info	: Image convert parameter. See @ref FJ_IMG_CONV_CTRL.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
@remarks	If you have enabled the fill function, Retain the aspect ratio of the input image, <br>
			and  fill in black the outside of effective image. <br>
			Supported image format is the following. <br>
			- the Y Planar and CbCr Interleaved format <br>
			- H.264 format <br>
			- RGBA8888 and RGBA4444.<br>
 */
extern	FJ_ERR_CODE	FJ_ImageConvert( FJ_IMG_CONV_CTRL* conv_info );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function format conversion an image format, To convert the format of the image.
@param[in]	src	: Input image parameter. See @ref FJ_IMG_INFO.
@param[in]	dst	: Output image parameter. See @ref FJ_IMG_INFO.
@param[in]	planer_select	: 0:To Planar format. other:To semi planer format.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
@remarks	Supported image format is the following. <br>
			- the Y Planar and CbCr Interleaved format <br>
			- the Y Planar and CbCr Planar format <br>
 */
extern	FJ_ERR_CODE	FJ_ImageFormatConvert( FJ_IMG_INFO* src, FJ_IMG_INFO* dst, UCHAR planer_select );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function crop and enlarge an image.
@param[in]	crop_info	: Image cropping parameter. See @ref FJ_IMG_CROP_CTRL.
@retval		FJ_ERR_OK	: success.
@retval		FJ_ERR_NG	: fail.
@remarks	This function can enlargement the image. ( ex. D-Zoom ) <br>
			Supported image format is the following. <br>
			- the Y Planar and CbCr Interleaved format <br>
			- H.264 format <br>
 */
extern	FJ_ERR_CODE	FJ_ImageCrop( FJ_IMG_CROP_CTRL* crop_info );

// @cond
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function crop an image on 4k usecase.<br>
This function handle AFNx2(for 4k movie) and AFNx1(for FHD display) in parallel.<br>
@param[in]	crop_info				: Image cropping parameter. See @ref FJ_IMG_CROP_CTRL_SPLIT.
@retval		FJ_ERR_OK				: success.
@retval		FJ_ERR_NG				: fail.
@retval		FJ_ERR_NG_INPUT_PARAM	: input parameter error.
@remarks	This function can enlargement the image.	( ex. D-Zoom ) <br>
			Supported image format is the following. <br>
			- the Y Planar and CbCr Interleaved format <br>
			- H.264 format <br>
 */
extern FJ_ERR_CODE	FJ_ImageCropSplitWithDisplay( FJ_IMG_CROP_CTRL_SPLIT* crop_info );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function black fill an outside of H.264 format image.<br>
X axis: from width to to roundup to 64 pixels.<br>
Y axis: from lines to to roundup to 32 pixels. (16 pixels of CbCr.)<br>
@image html fj_still_FJ_ImageFillVideo.png
@param[in]	fill_info : Fill parameter. See @ref FJ_IMG_FILL_CTRL.
@remarks	Supported image format is the following. <br>
			- H.264 format <br>
 */
extern	FJ_ERR_CODE	FJ_ImageFillVideo( FJ_IMG_FILL_CTRL* fill_info );
// @endcond

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function convert an image into a free rectangle.
@param[in]	frect_ctrl : Free rectangle control parameter. See @ref FJ_IMG_FRECT_CTRL.
@retval		FJ_ERR_OK				: success.
@retval		FJ_ERR_NG				: fail.
@retval		FJ_ERR_NG_INPUT_PARAM	: input parameter error.
@retval		FJ_ERR_IQ_TBL_INVALID	: Table memory is invalid(not ready).
@retval		FJ_ERR_IQ_TBL_EMPTY		: Table data is empty.
@remarks	If you have enabled the fill function, Retain the aspect ratio of the input image, <br>
			and  fill in black the outside of effective image. <br>
			Supported image format is the following. <br>
			- the Y Planar and CbCr Interleaved format <br>
			- H.264 format <br>
			- CAN NOT support image format with 'A' plane.<br>
 */
extern FJ_ERR_CODE FJ_ImageFreeRect( FJ_IMG_FRECT_CTRL* frect_ctrl );

// @cond
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
H/W support Blend function.<br>
@param[in]		blend_info : blending parameter. See @ref FJ_IMG_BLEND_CTRL.
@return 		FJ_ERR_CODE
*/
//extern	FJ_ERR_CODE	FJ_ImageBlend( FJ_IMG_BLEND_CTRL* blend_info );
// @endcond

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function blend a source image and a osd image with H/W support.<br>
@param[in]		osd_blend_info : OSD blending parameter. See @ref FJ_IMG_BLEND_CTRL.
@retval			FJ_ERR_OK				: success.
@retval			FJ_ERR_NG				: fail.
@retval			FJ_ERR_NG_INPUT_PARAM	: input parameter error.
@attention 		If the pixel format of target image is "Video format"(osd_blend_info->src_img.pfmt == @ref FJ_IMG_PFMT_VIDEO),<br>
				x position and width must be set '64' alignmented value and y position and lines must be set '16' alignmented value.<br>
*/
extern	FJ_ERR_CODE	FJ_ImageBlend( FJ_IMG_BLEND_CTRL* osd_blend_info );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Draw "Face-Frame" function by CPU (There is NO HW-Support).<br>
@param[in]		src_frame      : target image information. See @ref FJ_IMG_INFO.
@param[in]		face_pos_array : frame information of each. See @ref T_FACE_FRAME_EACH_SET. size, position and thickness need 2pixel boundary.
@param[in]		frame_set      : frame set. See @ref T_FACE_FRAME_SET.
@retval			FJ_ERR_OK      : success.
@retval			FJ_ERR_NG      : fail.
*/
extern	FJ_ERR_CODE	FJ_ImageDrawFaceFrame( FJ_IMG_INFO* src_frame, T_FACE_FRAME_EACH_SET* face_pos_array, T_FACE_FRAME_SET* frame_set );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Draw Rectangle function by CPU (There is NO HW-Support).<br>
@param[in]		src_frame : target image information. See @ref FJ_IMG_INFO.
@param[in]		frame_set : rectangle set. See @ref FJ_RECT_FRAME_SET. size and position need 2pixel boundary.
@retval			FJ_ERR_OK				: success.
@retval			FJ_ERR_NG				: fail.
@retval			FJ_ERR_NG_INPUT_PARAM	: input parameter error.
*/
extern	FJ_ERR_CODE	FJ_ImageDrawRect( FJ_IMG_INFO* src_frame, FJ_RECT_FRAME_SET* frame_set );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function would fill the specified area with specified data pattern using the Xch HW assist.<br>
The pattern of "fill data" is made by FJ_IMG_FILL_TYPE and  FJ_IMG_FILL_DATA combination.<br>
Besides, The FILL mode of Xch function can be used if parameters of all FJ_IMG_FILL_DATA is same. <br>
@param[in]		fill_info : target image information. See @ref FJ_IMG_FILL_INFO.
@retval			FJ_ERR_OK				: success.
@retval			FJ_ERR_NG_INPUT_PARAM	: input parameter error.
@attention		The g_width”parameter should set as 16Byte unit, because Xch has an alignment limitation regarding the horizontal size. <br>
@remarks		The processing image by each parameters are as follows.<br>
				@image html fj_still_image_fill.png
*/
extern	FJ_ERR_CODE FJ_ImageFill( FJ_IMG_FILL_INFO* fill_info );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function is an API for determining the availability of FJ_ImageFill().<br>
@retval			FJ_ERR_OK				: Available.
@retval			FJ_ERR_NG_CONDITION		: Usage prohibited.
@remarks		Please the results by calling before you run the FJ_ImageFill().<br>
*/
extern	FJ_ERR_CODE FJ_IsBufferFillAvailable( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function configurate gyro setting.<br>
@param[in]		gyro_config				: Gyro configuration setting.
@retval			FJ_ERR_NG				: fail.
@retval			FJ_ERR_NG_INPUT_PARAM	: input parameter error.
*/
extern	FJ_ERR_CODE FJ_ConfigGyro( FJ_GYRO_CONFIG* gyro_config );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function add gyro data and timestamp to EIS library.<br>
@param[in]		frame_no   : Frame no.
@param[in]		*gyro_data : Gyro data.
@retval			FJ_ERR_OK				: Available.
@retval			FJ_ERR_NG				: fail.
@retval			FJ_ERR_NG_INPUT_PARAM	: input parameter error.
*/
extern	FJ_ERR_CODE FJ_AddGyroData( ULLONG frame_no, FJ_GYRO_DATA* gyro_data );

// Moved to ENCODE Module
/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	This function encode an image in JPEG.
 *	@param[in]	enc_info : structure member of dst_addr is destination jpeg address. See @ref FJ_IMG_JPEG_CTRL.
 *	@param[in]	enc_info : structure member of upper_limit_size is Upper limit size of Jpeg encoding. See @ref FJ_IMG_JPEG_CTRL.
 *	@param[out]	enc_info : structure member of enc_size is result size of Jpeg encoded. See @ref FJ_IMG_JPEG_CTRL.
 *	@retval FJ_ERR_OK : success.<br>
 *	@retval FJ_ERR_NG : fail.<br>
 *	Sample code is following.<br>
 *	@code
	FJ_ERR_CODE error_code;
	FJ_IMG_JPEG_CTRL jpeg_ctrl;

	jpeg_ctrl.src_img.addr.AddressY		= 0x40000000;			// address of Y
	jpeg_ctrl.src_img.addr.AddressCb	= 0x41000000;			// address of CbCr (semi plane format)
	jpeg_ctrl.src_img.addr.AddressCr	= 0;					// don't care
	jpeg_ctrl.src_img.g_y_width			= 4000;					// image size global width of Y
	jpeg_ctrl.src_img.g_c_width			= 4000;					// image size global width of CbCr
	jpeg_ctrl.src_img.width				= 4000;					// image size width
	jpeg_ctrl.src_img.lines				= 3000;					// image size width
	jpeg_ctrl.src_img.pfmt				= FJ_IMG_PFMT_YCC422;	// YCC422 semi plane format
	jpeg_ctrl.dst_addr					= 0x43000000			// destination address.
	jpeg_ctrl.enc_size					= 0;					// initialization of encode size.
	jpeg_ctrl.upper_limit_size			= 8 * 1024 * 1024;		// Upper limit size of Jpeg encoding. (8Mbyte)

	error_code = FJ_ImageJpegEncode( &jpeg_ctrl);

	fs_write(jpeg_ctrl.dst_addr, jpeg_ctrl.enc_size);		// UserFunction to jpeg file save
 *	@endcode
 */
extern	FJ_ERR_CODE	FJ_ImageJpegEncode( FJ_IMG_JPEG_CTRL* enc_info );

/* @} */	// fj_still_api_utility group


///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_still_message
@{*/
/**
@def FJ_HM_STILL_RAW_READY
Ready for RAW data in still process.
@param[in]	0	None. Always 0
@remarks	This message indicates the VQ main that the captured RAW data was grabbed from the sensor.
			Follow this message the VQ main can re-open the mechanical shutter.
*/

/**
@def FJ_HM_SEND_CONFIRM_STORE
Confirm to host for save image to media in still still process.
@param[in]	0	None. Always 0
@remarks	Q.View is ready to show this message.
*/

/**
@def FJ_HM_STORE_FINISHED
Confirm to host for save image to media in still still process.
@param[in]	0 : Successful store, other : error id
@remarks	Q.View is ready to show this message.
*/

/**
@def FJ_HM_BKG_STORE_COMPLETE
Report to host still process is completed.
@param[in]	0	None. Always 0
@remarks	This message is sent when the background store process 
			finishes and there are no more images in queue to store.
*/

/**
@def FJ_HM_QVIEW_START
Report to host quick view process is started.
@param[in]	0	None. Always 0
@remarks	This message is sent When indication time except 0 was set.
*/

/**
@def FJ_HM_QVIEW_END
Report to host quick view process is completed.
@param[in]	0	None. Always 0
@remarks	This message is sent When indication time except 0 was set.
			Please do blackback indication if view is not finished.
*/

/**
@def FJ_HM_VIDEO_RAW_READY
Ready for RAW data in video calibration process.
@param[in]	0	None. Always 0
@remarks	This message indicates the VQ main that RAW data in video was grabbed from the sensor.
			Follow this message the VQ main can re-open the mechanical shutter.
			This message is send only in video calibration process.
*/

/**
@def FJ_HM_VIDEO_MODE_READY
Report the timing which returns to a view mode to host.
@param[in]	0	None. Always 0
@remarks	In last frame of Still, BaseFW notify this message to Host.
*/

/* @} */	// fj_still_message group

#endif
