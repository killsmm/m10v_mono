/**
 * @file		fj_custom.h
 * @brief		Definition of custamization for user
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_CUSTOM_H
#define _FJ_CUSTOM_H
///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_custom_overview
@{
	Definition of custamization for user.
	If necessary, please modify contents of definition in fj_custom.h.
@}*//* --- end of fj_custom_overview */

#include "ddim_typedef.h"

// @cond
//-----------------------------------------------------------------------------
/**
@weakgroup fj_custom_definition_exif_dcf
@{*/
//#define FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN				/**< CPU Endian = BIG */
//#define FJ_CUSTOM_EXIF_DATA_BIG_ENDIAN			/**< Exif Endian = BIG */
//#define FJ_CUSTOM_CPU_BIG_ENDIAN					/**< CPU Endian = BIG */

#define FJ_CUSTOM_DCF_FILE_NAME			"MLBT"		/**< DCF File Name (it must be 4-characters) */
#define FJ_CUSTOM_DCF_DIR_NAME			"SOCIO"		/**< DCF Directory Name (it must be 5-characters) */
// --- REMOVE_USB BEGIN ---
#define FJ_CUSTOM_DCF_ROOT_DIR_NAME		"DCIM"		/**< DCF Root Directory Name */
// --- REMOVE_USB END ---

#define FJ_CUSTOM_DCF_ROOT_THM_DIR_NAME	"MISC"		/**< DCF Root Thumbnail Directory Name */
#define FJ_CUSTOM_DCF_THM_DIR_NAME		"THM"		/**< DCF Thumbnail Directory Name */

#define FJ_CUSTOM_DCFDB_MAX				(9999)		/**< The number of DCF DB max(1~10000) */

#define FJ_CUSTOM_DCFDB_DIRNUM_MIN		(100)		/**< The minimum directory number of DCF DB */
#define FJ_CUSTOM_DCFDB_DIRNUM_MAX		(999)		/**< The maximum directory number of DCF DB */
#define FJ_CUSTOM_DCFDB_FILENUM_MIN		(0)			/**< The minimum file number of DCF DB */
#define FJ_CUSTOM_DCFDB_FILENUM_MAX		(9999)		/**< The maximum file number of DCF DB */

#define FJ_CUSTOM_EXIF_IMG_DESCRIPTION_S		(32)								/**< Image Description size (4byte align) */
#define FJ_CUSTOM_EXIF_IMG_DESCRIPTION			"Fiytounlcount					"	/**< Image Description */
#define FJ_CUSTOM_EXIF_MAKE_S					(16)								/**< Maker Name size (4byte align) */
#define FJ_CUSTOM_EXIF_MAKE						"Fiytounlcount	"					/**< Maker Name */
#define FJ_CUSTOM_EXIF_MODEL_S					(12)								/**< Model Name size (4byte align) */
#define FJ_CUSTOM_EXIF_MODEL					"FT1-2635	"						/**< Model Name */
#define FJ_CUSTOM_EXIF_SOFT_S					(8)									/**< Software version size (4byte align) */
#define FJ_CUSTOM_EXIF_SOFT						"Ver 1.0"							/**< Software version */
#define FJ_CUSTOM_EXIF_COPYRIGHT_S				(16)								/**< Copyright size (4byte align) */
#define FJ_CUSTOM_EXIF_COPYRIGHT				"Copyright 2022 "					/**< Copyright */
#define FJ_CUSTOM_EXIF_PRINTIM_S				(28)								/**< PrintIM size (4byte align) */
#define FJ_CUSTOM_EXIF_PRINTIM					"PrintIM-0300               "		/**< PrintIM */
#define FJ_CUSTOM_EXIF_MAKERNOTE_S				(128)								/**< MakerNote size (128byte align) */
#define FJ_CUSTOM_EXIF_MAKERNOTE 				"\0"								/**< MakerNote */
#define FJ_CUSTOM_EXIF_MAKER1_S					(16)								/**< Maker Name size for IFD1 (4byte align) */
#define FJ_CUSTOM_EXIF_MAKER1					"Fiytounlcount	"					/**< Maker Name for IFD1 */
#define FJ_CUSTOM_EXIF_MODEL1_S					(12)								/**< Model Name size for IFD1 (4byte align) */
#define FJ_CUSTOM_EXIF_MODEL1					"FT1-2616	"						/**< Model Name for IFD1 */
#define FJ_CUSTOM_EXIF_MAKERNOTE_S_MAX			(0x4000)							/**< Maker Note Max size (16Kbyte) */

#define FJ_CUSTOM_DIRECTORY_DEPTH				(3)			/**< The number of directories opening out <- Recommendation (3) */
/*@} --- end of fj_custom_definition_exif_dcf */
// @endcond

//-----------------------------------------------------------------------------
/** @weakgroup fj_custom_definition_debug
@{*/
#define	FJ_DEBUG_PRINT_ON							/**< UART output function is On. */
#define	FJ_DEBUG_PRINT_DBG_ON						/**< UART output. Debug message output is On. */
#define	FJ_DEBUG_PRINT_INFO_ON						/**< UART output. Information message output is On. */
#define	FJ_DEBUG_PRINT_WRN_ON						/**< UART output. Warning message output is On. */
#define	FJ_DEBUG_PRINT_ERR_ON						/**< UART output. Error message output is On. */
/*@} --- end of fj_custom_definition_debug */

//-----------------------------------------------------------------------------
/** @weakgroup fj_custom_definition_message
@{*/
/**
Definition of size for HM.
@remarks This definition must accord with SID_HOST_MSG_COUNT on REALOS configuration.
*/
#define NUMBER_OF_HOST_MESSAGE_BUFFER	(4000)
/*@} --- end of fj_custom_definition_message */

//-----------------------------------------------------------------------------
/**
@if NOT_SUPPORTED_CUSTOMIZATION
@weakgroup fj_custom_definition_still
@{
@endif
*/
/* @if NOT_SUPPORTED_CUSTOMIZATION */
#if 0
#define FJ_CUSTOM_WIDTH_SCR				(WIDTH_QVGA)										/**< ScreenNail width					*/
#define FJ_CUSTOM_LINES_SCR				(LINES_QVGA)										/**< ScreenNail height					*/
#define FJ_CUSTOM_LINES_SCR_3_2			(LINES_QVGA_3_2)									/**< ScreenNail height for 3:2			*/
#define FJ_CUSTOM_LINES_SCR_16_9		(LINES_QVGA_16_9)									/**< ScreenNail height for 16:9			*/
#define	FJ_CUSTOM_LINES_SCR_FILL_3_2	(LINES_FILL_3_2_QVGA)								/**< ScreenNail fill height for 3:2		*/
#define	FJ_CUSTOM_LINES_SCR_FILL_16_9	(LINES_FILL_16_9_QVGA)								/**< ScreenNail fill height for 16:9	*/
#else
#define FJ_CUSTOM_WIDTH_SCR				(WIDTH_VGA)											/**< ScreenNail width					*/
#define FJ_CUSTOM_LINES_SCR				(LINES_VGA)											/**< ScreenNail height					*/
#define FJ_CUSTOM_LINES_SCR_3_2			(LINES_VGA_3_2)										/**< ScreenNail height for 3:2			*/
#define FJ_CUSTOM_LINES_SCR_16_9		(LINES_VGA_16_9)									/**< ScreenNail height for 16:9			*/
#define	FJ_CUSTOM_LINES_SCR_FILL_3_2	(LINES_FILL_3_2_VGA)								/**< ScreenNail fill height for 3:2		*/
#define	FJ_CUSTOM_LINES_SCR_FILL_16_9	(LINES_FILL_16_9_VGA)								/**< ScreenNail fill height for 16:9	*/
#endif
#define FJ_CUSTOM_PIX_SIZ_SCR			(FJ_CUSTOM_WIDTH_SCR * FJ_CUSTOM_LINES_SCR)			/**< ScreenNail Pixel size				*/
#define FJ_CUSTOM_PIX_SIZ_SCR_3_2		(FJ_CUSTOM_WIDTH_SCR * FJ_CUSTOM_LINES_SCR_3_2)		/**< ScreenNail Pixel size for 3:2		*/
#define FJ_CUSTOM_PIX_SIZ_SCR_16_9		(FJ_CUSTOM_WIDTH_SCR * FJ_CUSTOM_LINES_SCR_16_9)	/**< ScreenNail Pixel size for 16:9		*/

#define FJ_CUSTOM_LINES_SCR_PHOTOSTITCH_2PICS		(LINES_PHOTOSTITCH_2PICS_VGA)			/**< ScreenNail height for PhotoStitch(2PICS)		*/
#define FJ_CUSTOM_LINES_SCR_PHOTOSTITCH_3PICS		(LINES_PHOTOSTITCH_3PICS_VGA)			/**< ScreenNail height for PhotoStitch(3PICS)		*/
#define	FJ_CUSTOM_LINES_SCR_FILL_PHOTOSTITCH_2PICS	(LINES_FILL_PHOTOSTITCH_2PICS_VGA)		/**< ScreenNail fill height for PhotoStitch(2PICS)	*/
#define	FJ_CUSTOM_LINES_SCR_FILL_PHOTOSTITCH_3PICS	(LINES_FILL_PHOTOSTITCH_3PICS_VGA)		/**< ScreenNail fill height for PhotoStitch(3PICS)	*/

#define	FJ_CUSTOM_EVTT_ADI_TG_REG_ADDR		(0x0000)	/**< TG(SUB) register Address */
#define	FJ_CUSTOM_GLTT_ADI_VGA_REG_ADDR		(0x0000)	/**< VGA Register Address */
#define	FJ_CUSTOM_GLTT_SIZE					(128)		/**< Number of entries in GLTT size */
#define	FJ_CUSTOM_EVTT_SIZE					(22)		/**< Number of entries in EVTT size */

extern const USHORT FJ_Custom_GLTT_Preview[];	/**< VGA_Gain data info for Preview	*/
extern const USHORT FJ_Custom_GLTT_Capture[];	/**< VGA_Gain data info for Capture	*/
extern const USHORT FJ_Custom_EVTT_Preview[];	/**< SUB data info for Preview	*/
extern const USHORT FJ_Custom_EVTT_Capture[];	/**< SUB data info for Capture	*/

// --- REMOVE_USB BEGIN ---
#define FJ_CUSTOM_USB_PARTITION_ON					/**< USB Storage Partition On/Off */

/* USB PTP/MTP Description */
#define FJ_CUSTOM_USB_PTP_DESCRIPTION_LEN_NF	(0)					/**< USB PTP/MTP Description length for NF */
#define FJ_CUSTOM_USB_PTP_DESCRIPTION_LEN_MS	(0)					/**< USB PTP/MTP Description length for MS */
#define FJ_CUSTOM_USB_PTP_DESCRIPTION_LEN_SD	(0)					/**< USB PTP/MTP Description length for SD */

#define FJ_CUSTOM_USB_PTP_MANUFACTURER_LEN		(9)					/**< USB PTP/MTP Manufacturer string length */
#define FJ_CUSTOM_USB_PTP_MANUFACTURER_STR		{'c', 0, 'u', 0, 's', 0, 't', 0, 'o', 0, 'm', 0, 'e', 0, 'r', 0, 0, 0}	/**< USB PTP Manufacrurer string */
#define FJ_CUSTOM_USB_MTP_MANUFACTURER_STR		{'c', 0, 'u', 0, 's', 0, 't', 0, 'o', 0, 'm', 0, 'e', 0, 'r', 0, 0, 0}	/**< USB MTP Manufacrurer string */

#define FJ_CUSTOM_USB_PTP_MODEL_LEN				(13)				/**< USB PTP/MTP Model string length */
#define FJ_CUSTOM_USB_PTP_MODEL_STR				{'c', 0, 'u', 0, 's', 0, 't', 0, 'o', 0, 'm', 0, 'e', 0, 'r', 0, ' ', 0, 'P', 0, 'T', 0, 'P', 0, 0, 0}	/**< USB PTP Model string */
#define FJ_CUSTOM_USB_MTP_MODEL_STR				{'c', 0, 'u', 0, 's', 0, 't', 0, 'o', 0, 'm', 0, 'e', 0, 'r', 0, ' ', 0, 'M', 0, 'T', 0, 'P', 0, 0, 0}	/**< USB MTP Model string */
// --- REMOVE_USB END ---

/*@} --- end of fj_custom_definition_still */
/* @endif */

//-----------------------------------------------------------------------------
#define FJ_CUSTOM_BEEP_DATA_CNT	(30)						/**< data buffer count for BEEP sound */
//-----------------------------------------------------------------------------

#endif
