/**
 * @file		fj_std.h
 * @brief		FJ_API Error Code header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_STD_H
#define _FJ_STD_H

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_std_overview
@{
	In this chapter, it describes Standard Definition in Base F/W.<br>
@}*//* --- end of fj_std_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_std_sequence
@{
@}*//* --- end of fj_std_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/** @weakgroup fj_std_definition
@{*/
/* Definition of ON/OFF */
#define FJ_OFF				(0)			/**< ON */
#define FJ_ON				(1)			/**< OFF */

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Error Code */
typedef enum{
	/* Common part */
	FJ_ERR_OK						= 0x0000,	/**< OK */
	FJ_ERR_NG						= 0x0001,	/**< Error */
	FJ_ERR_NG_INPUT_PARAM			= 0x0002,	/**< Input Parameter error */
	FJ_ERR_NG_EXCEPTION				= 0x0003,	/**< Exception occurred */
	FJ_ERR_NG_CONDITION				= 0x0004,	/**< Condition error */
	FJ_ERR_NG_COMMUNICATON			= 0x0005,	/**< Communication error */
	FJ_ERR_NG_TMOUT					= 0x0006,	/**< Timeout error */
	FJ_ERR_NO_SUPPORT				= 0x0007,	/**< API cannot be supported on MILB_SDK */
	FJ_ERR_NG_FORMAT				= 0x0008,	/**< Format error */
	FJ_ERR_NG_MEMORY				= 0x0009,	/**< Memory error */
	FJ_ERR_OVERFLOW					= 0x000a,	/**< Overflow error */
	/* OSD Part */
	FJ_ERR_POSITION					= 0x0101,	/**< Can't draw at the position specified. */
	FJ_ERR_OSD_LAYER				= 0x0102,	/**< The layer that tried to be written by OSD is illegal. */
	FJ_ERR_OSD_LAYER_OR_POSITION	= FJ_ERR_POSITION |FJ_ERR_OSD_LAYER,/**< The layer or position error. */
	FJ_ERR_OSD_SEQUENCE				= 0x0104,	/**< The sequence error of drawing. */
	FJ_ERR_OSD_ASCII_DATA			= 0x0105,	/**< ASCII data define error. */
	FJ_ERR_OSD_ASCII_TABLE			= 0x0106,	/**< ASCII data define error. */
	FJ_ERR_OSD_FILE_ACCSESS			= 0x0107,	/**< OSD resource access error. */
	FJ_ERR_OSD_FILE_SIZE			= 0x0108,	/**< The data of the read file is too large. */
	FJ_ERR_OSD_ID_ERROR				= 0x0109,	/**< The ID specified is not exist. */
	FJ_ERR_OSD_CH_STATE_ERROR		= 0x010A,	/**< Channel state error. */
	FJ_ERR_OSD_SHOWING_AREA_ERROR	= 0x010B,	/**< Showing area setting error. */

	/* File System */
	FJ_ERR_DCF_DUPLICATE			= 0x0201,	/**< Repetition of file number.(Not supported) */
	FJ_ERR_NOT_FAT					= 0x0202,	/**< File Format of media that do Mount is not FAT. */
	FJ_ERR_NOT_MOUNT				= 0x0203,	/**< Not mount media */
	FJ_ERR_FREE_SPACE_OVER			= 0x0204,	/**< There is no free space to save the date database file. */
	FJ_ERR_WRITE_FAIL_TO_MEDIA		= 0x0205,	/**< Failed to write to the media. */
	FJ_ERR_READ_EOF					= 0x0206,	/**< Trying to read furthermore after reached to the end of the file. */
	FJ_ERR_READ_NF_BAD_BLOCK		= 0x0207,	/**< Failed to read to the NandFlash by bad block. */
	FJ_ERR_LAST_ENTRY				= 0x0208,	/**< Trying to get entry after reached to the last entry. */
	FJ_ERR_NF_NO_ALLOCATE			= 0x0209,	/**< NandFlash physical block is not allocated. */
	/* AV part */
	FJ_ERR_MOVIE_ALREADY_STOP_PLAY	= 0x0302,	/**< VideoPlayback is already stopped. */
	/* IQ Part */
	FJ_ERR_IQ_BP_DETECT_INTERNAL1	= 0x0401,	/**< Bad pixel BaseFW internal error. */
	FJ_ERR_IQ_BP_DETECT_INTERNAL2	= 0x0402,	/**< Bad pixel BaseFW internal error. */
	FJ_ERR_IQ_BP_MERGE_INTERNAL1	= 0x0403,	/**< Bad pixel BaseFW internal error. */
	FJ_ERR_IQ_BP_MERGE_INTERNAL2	= 0x0404,	/**< Bad pixel BaseFW internal error. */
	FJ_ERR_IQ_BP_HW_DETECT_ON		= 0x0405,	/**< Bad pixel FJ_IQ_CalibrationCreateBadpixTable() called by host, but hardware detection is enabled */
	FJ_ERR_IQ_BP_CALIB_TGT			= 0x0406,	/**< Bad pixel: The calibration target don't specified.  (please call FJ_IQ_CalibrationSetTarget() and retry.) */
	FJ_ERR_IQ_BP_THRESH				= 0x0407,	/**< Bad pixel: Parameter error in pt_corner_thresh (function FJ_IQ_CalibrationCreateBadpixTable()) */
	FJ_ERR_IQ_BP_FMT_PIX			= 0x0408,	/**< Bad pixel: The format specified as E_IQ_CALIB_FMT_PIX_12PACK */
	FJ_ERR_IQ_BP_TOO_MUCH			= 0x0409,	/**< Bad pixel: The number of bad pixels are more than max_badpix_thresh. (function FJ_IQ_CalibrationCreateBadpixTable()) */
	FJ_ERR_IQ_BP_TOO_OVERFLOW		= 0x040A,	/**< Bad pixel: The bad pixel table memory is overflow. */
	FJ_ERR_IQ_TBL_FULL				= 0x040B,	/**< Table memory is full */
	FJ_ERR_IQ_TBL_INVALID			= 0x040C,	/**< Table memory is invalid(not ready) */
	FJ_ERR_IQ_FILE_OPEN_NG			= 0x040D,	/**< Data file open NG */
	FJ_ERR_IQ_FILE_FMT_NG			= 0x040E,	/**< Data file format NG */
	FJ_ERR_IQ_FILE_TOO_LARGE		= 0x040F,	/**< Data file size too large */
	FJ_ERR_IQ_FILE_TOO_SHORT		= 0x0410,	/**< Data file size too short */
	FJ_ERR_IQ_GAIN_OVERFLOW			= 0x0411,	/**< Calculated gain data is overflow */
	FJ_ERR_IQ_TBL_EMPTY				= 0x0412,	/**< Table data is empty */
	FJ_ERR_IQ_TIMEOUT				= 0x0413,	/**< Wait time out */
	FJ_ERR_IQ_DRIVER_ERR			= 0x0414,	/**< error return from Im driver */
	/* Serial I/O Part */
	FJ_ERR_SIO_OVERRUN_ERROR		= 0x0501,	/**< Receive buffer overrun error */
	FJ_ERR_SIO_DMA_ERROR			= 0x0502,	/**< DMA error */
	/* SPI */
	FJ_ERR_SPI_OVERRUN_ERROR		= 0x0601,	/**< Receive buffer overrun error */
	FJ_ERR_SPI_DMA_ERROR			= 0x0602,	/**< DMA error */
	/* UART */
	FJ_ERR_UART_NO_SEND_DATA		= 0x0701,	/**< No send data */
	FJ_ERR_UART_MODE_UNMATCH		= 0x0702,	/**< Mode unmatch */
	FJ_ERR_UART_OVERFLOW_ERROR		= 0x0703,	/**< Overflow error */
	FJ_ERR_UART_OVERRUN_ERROR		= 0x0704,	/**< Overrun error */
	FJ_ERR_UART_FRAMING_ERROR		= 0x0705,	/**< Framing error */
	FJ_ERR_UART_PARITY_ERROR		= 0x0706,	/**< Parity error */
// --- REMOVE_USB BEGIN ---
	/* USB */
	FJ_ERR_USB_MEDIA_ERROR			= 0x0801,	/**< Media Error of USB Storage */
	FJ_ERR_USB_DISCONNECT_ERROR		= 0x0802,	/**< USB cable disconnect */
// --- REMOVE_USB END ---
} FJ_ERR_CODE;
/*@} --- end of fj_std_definition */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*+ @weakgroup fj_std_api
@{*/

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*@} --- end of fj_std_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_std_message
@{*/
/*@} --- end of fj_std_message */

#endif	/* _FJ_STD_H */
