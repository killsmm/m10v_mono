/**
 * @file		fj_playback.h
 * @brief		PlayBack Controller
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_PLAYBACK_H
#define _FJ_PLAYBACK_H

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_still.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_playback_overview
@{
	This chapter describes about playback function.<br>
	<br>
	- Functions<br>
		Playback provides the following functions.<br>
		- Decode jpeg data.<br>
		- Decode MP4 data.<br>
		- The following image conversion.<br>
			- Resize<br>
			- Rotate<br>
			- Convert YUV data to RGBA data<br>
	<br>
	- Limitations<br>
		- JPEG decording<br>
			- Max file size is up to 36,152,320(7360*4912)bytes. [Note: There is also a memory map size limit]<br>
			- The file must be a Exif standard-compliant JPEG file.<br>
			- Output data format is Semi-Planer( Y: Plane, CbCr: Dot) YCC422 only.<br>
			  Please specify YCC422 to the output data format.<br>
		- Image Conversion (Resize, Rotate, Convert YUV data to RGBA data)<br>
			- Max file size is up to 36,152,320(7360x4912)bytes. [Note: There is also a memory map size limit]<br>
			- Input YUV data format is YCC422 or YCC420 only.<br>
			- Output data format is YCC422, YCC420 and RGBA8888 only.<br>
			- Global width is must be 16pixels allignment.<br>
			- Height is must be even allignment.<br>
	<br>
	- Buffers<br>
		- HostFW management buffer<br>
			- Please manage JPEG read buffer and MP4 read/parse buffer by HostFW side.<br>
			- HostFW can get memory information of HostFW management buffers by FJ_GetMem().<br>
		- BaseFW management buffer<br>
			- HostFW can get memory information of these buffers by FJ_StillPlayback_GetMemInfo().<br>
			- Memory Description
			<table>
			<tr>
				<th>Buffer Name</th>
				<th>Description</th>
				<th>Buffer computation Formula</th>
				<th>Buffer Calculation</th>
				<th>Total Size (bytes)</th>
				<th>Remarks</th>
			</tr>
			<tr>
				<td>decoded_output_yuv_main</td>
				<td>Decoded YUV Buffer for JPEG Main</td>
				<td>FULL_RES_W*FULL_RES_H*YUV_BYTES_PER_PIXEL</td>
				<td>4800*3600*2</td>
				<td>34,560,000</td>
				<td></td>
			</tr>
			<tr>
				<td>decoded_output_yuv_thm</td>
				<td>Decoded YUV Buffer for JPEG Thumbnail</td>
				<td>MAX_THM_W*MAX_THM_H*YUV_BYTES_PER_PIXEL</td>
				<td>320*240*2</td>
				<td>153,600</td>
				<td></td>
			</tr>
			<tr>
				<td>decoded_resize_yuv</td>
				<td>Decoded Resize YUV Buffer for JPEG</td>
				<td>MAX_RESIZE_W*MAX_RESIZE_H*YUV_BYTES_PER_PIXEL*NUM_BUFFERS</td>
				<td>960*720*2*5</td>
				<td>6,912,000</td>
				<td></td>
			</tr>
			<tr>
				<td>decoded_rgb</td>
				<td>RGB Buffer for Screennail</td>
				<td>MAX_SCREEN_W*MAX_SCREEN_H*RGB_BYTES_PER_PIXEL</td>
				<td>4096*2160*4</td>
				<td>35,389,440</td>
				<td></td>
			</tr>
			<tr>
				<td>thm_rgb</td>
				<td>RGB Buffer for Thumbnail</td>
				<td>MAX_THM_W*MAX_THM_H*RGB_BYTES_PER_PIXEL*NUM_BUFFERS</td>
				<td>320*240*4*80</td>
				<td>307,200</td>
				<td>Depending on the model, some cases can not be secured 80.</td>
			</tr>
			<tr>
				<td>disp_yuv_lcd</td>
				<td>Display YUV Buffer on Disp Image Layer for LCD</td>
				<td>LCD_W*LCD_H*YUV_BYTES_PER_PIXEL*NUM_BUFFERS</td>
				<td>640*480*1.5*3</td>
				<td>1,382,40</td>
				<td></td>
			</tr>
			<tr>
				<td>disp_yuv_hdmi</td>
				<td>Display YUV Buffer on Disp Image Layer for HDMI</td>
				<td>HDMI_W*HDMI_H*YUV_BYTES_PER_PIXEL*NUM_BUFFERS</td>
				<td>4096*2160*1.5*3</td>
				<td>39,813,120</td>
				<td></td>
			</tr>
	    	</table>
@cond
   The content of this chapter is described control playback process sequence and related APIs.<br>
	<br>
	There are the following limitations of image size for be able to playback display.<br>
	<br>
	[ Size alignment ]<br>
	Width is 16 pixels aligned.<br>
	Height is Even pixels aligned.<br>
	<br>
	[ Maximum Size ]<br>
	The multiplier value of width and height should be within 36,152,320 (= 7360 * 4912) pixcels.<br>
@endcond
   
@}*//* --- end of fj_playback_overview */


///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_playback_sequence
@{
- @ref fj_playback_sequence_section0
- @ref fj_playback_sequence_section1
- @ref fj_playback_sequence_section2
- @ref fj_playback_sequence_section3
- @ref fj_playback_sequence_section4
- @ref fj_playback_sequence_section5
- @ref fj_playback_sequence_section6
- @ref fj_playback_sequence_section7
<br>
@section fj_playback_sequence_section0	Mode Change(IDLE->Playback)
@image html fj_playback_sq00_mode_change.png
@section fj_playback_sequence_section1	Screennail and Thumbnail Playback of JPEG on OSD Layer
@image html fj_playback_sq01_screennail_and_thumbnail_playback_of_jpeg_on_OSD_layer.png
@section fj_playback_sequence_section2	Screennail Playback of JPEG on Image Layer
@image html fj_playback_sq02_screennail_playback_of_jpeg_on_Image_layer.png
@section fj_playback_sequence_section3	Screennail Playback of MP4 on OSD Layer
@image html fj_playback_sq03_screennail_playback_of_MP4_on_OSD_layer.png
@section fj_playback_sequence_section4	Screennail Playback of MP4 on Image Layer (Display by HostFW)
@image html fj_playback_sq04_screennail_playback_of_MP4_on_Image_layer_Display_by_HostFW.png
@section fj_playback_sequence_section5	Screennail Playback of MP4 on Image Layer (Display by BaseFW)
@image html fj_playback_sq05_screennail_playback_of_MP4_on_Image_layer_Display_by_BaseFW.png
@section fj_playback_sequence_section6	RGB Frame of the Paused Screen during Video Playback
@image html fj_playback_sq06_RGB_Frame_Playback_of_MP4_Paused_Screen_on_OSD_Layer.png
@section fj_playback_sequence_section7	Mode Close Process
@image html fj_playback_sq07_Mode_Close_Process.png

@cond
- @ref fj_playback_sequence_section1
- @ref fj_playback_sequence_section2
<br>
@section fj_playback_sequence_section1	Single display.
@image html fj_playback_sq01_single_display.png
@section fj_playback_sequence_section2	Thumbnail index display.
@image html fj_playback_sq02_thumb_index_display.png
@endcond
   
@}*//* --- end of fj_playback_sequence */

/** @weakgroup fj_playback_definition
@{*/
///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/*--------------------------------------------------------------*/
/* Enumeration													*/
/*--------------------------------------------------------------*/
/** Playback format */
typedef enum {
	E_FJ_PLAYBACK_FMT_YCC422	= 0x00,		/**< YCC422		*/
	E_FJ_PLAYBACK_FMT_YCC420	= 0x01,		/**< YCC420 (Not supported in JPEG Decording.)	*/
} E_FJ_PLAYBACK_FMT;

/*--------------------------------------------------------------*/
/* Structure													*/
/*--------------------------------------------------------------*/
/** Playback memory information */
typedef struct {
	UINT32	top_addr;			/**< buffer top address		*/
	UINT32	buff_size;			/**< buffer size			*/
} T_FJ_PLAYBACK_BUFF_INFO;

/** Playback buffer information */

typedef struct {
	T_FJ_PLAYBACK_BUFF_INFO		decoded_output_yuv_main;/**< Decoded YUV Buffer for JPEG Main. <br>
														   Buffer size is FULL_RES_W*FULL_RES_H*YUV_BYTES_PER_PIXEL = 4800*3600*2 = 34,560,000 (bytes) */
	T_FJ_PLAYBACK_BUFF_INFO		decoded_output_yuv_thm;	/**< Decoded YUV Buffer for JPEG Thumbnail. <br>
														   Buffer size is MAX_THM_W*MAX_THM_H*YUV_BYTES_PER_PIXEL = 320*240*2 = 153,600 (bytes) */
	T_FJ_PLAYBACK_BUFF_INFO		decoded_resize_yuv;		/**< Decoded Resize YUV Buffer for JPEG.<br>
														   Buffer size is MAX_RESIZE_W*MAX_RESIZE_H*YUV_BYTES_PER_PIXEL*NUM_BUFFERS = 960*720*2*5 = 6,912,000 (bytes) */
	T_FJ_PLAYBACK_BUFF_INFO		decoded_rgb;			/**< RGB Buffer for Screennail.<br>
														   Buffer size is MAX_SCREEN_W*MAX_SCREEN_H*RGB_BYTES_PER_PIXEL = 4096*2160*4 = 35,389,440 (bytes) */
	T_FJ_PLAYBACK_BUFF_INFO		thm_rgb;				/**< RGB Buffer for Thumbnail.<br>
														   Buffer size is MAX_THM_W*MAX_THM_H*RGB_BYTES_PER_PIXEL*NUM_BUFFERS = 320*240*4*80 = 307,200 (bytes)<br>
														   Depending on the model, some cases can not be secured 80. */
	T_FJ_PLAYBACK_BUFF_INFO		disp_yuv_lcd;			/**< Display YUV Buffer for LCD.<br>
														   Buffer size is LCD_W*LCD_H*YUV_BYTES_PER_PIXEL*NUM_BUFFERS = 640*480*1.5*3 = 1,382,400 (bytes) */
	T_FJ_PLAYBACK_BUFF_INFO		disp_yuv_hdmi;			/**< Display YUV Buffer for HDMI.<br>
														   Buffer size is HDMI_W*HDMI_H*YUV_BYTES_PER_PIXEL*NUM_BUFFERS = 4096*2160*1.5*3 = 39,813,120 (bytes) */
} T_FJ_PLAYBACK_MEMINFO;

/** Playback decode information */
typedef struct {
	UINT32				src_addr;			/**< source addres(JPEG)		*/
	UINT32				src_size;			/**< source size(JPEG) 			*/
	UINT32				src_width;			/**< source width(JPEG) 		*/
	UINT32				src_lines;			/**< source lines(JPEG) 		*/
	UINT32				dst_addr;			/**< destination address(YUV)	*/
	E_FJ_PLAYBACK_FMT	dst_fmt;			/**< destination format(YUV)	*/
} T_FJ_PLAYBACK_DECODE;

/** Playback YUV parameter */
typedef struct{
	UINT32				y_addr;				/**< Y    data stored address.	*/
	UINT32				cbcr_addr;			/**< CbCr data stored address.	*/
	UINT32				g_width;			/**< global horizontal size. (16pix aligned)	*/
	UINT32				g_lines;			/**< global vertical size. (8pix aligned)		*/
	UINT32				width;				/**< horizontal size.			*/
	UINT32				lines;				/**< vertical size.				*/
	E_FJ_PLAYBACK_FMT	yuv_format;			/**< YUV format.				*/
} T_FJ_PLAYBACK_YUV_INFO;

/** Playback convert information */
typedef struct {
	FJ_IMG_INFO				src_img;		/**< source image information	 	*/
	FJ_IMG_INFO				dst_img;		/**< destination image information	*/
	FJ_IMG_RESIZE_MODE		resize_mode;	/**< Interpolation mode select		*/
	FJ_IMG_ROTATE_DEGREE	rotate_deg;		/**< Rotate degree (clockwise) 		*/
} T_FJ_PLAYBACK_CONVERT;


/*********************************/
/* playback common function      */
/*********************************/
#define ATT_FILE_NAME_FULLPATH			/* on: full path, off: dos name */

#ifdef ATT_FILE_NAME_FULLPATH
#define ATT_FILE_NAME_SIZE		(260)	/* length of full path */
#else
#define ATT_FILE_NAME_SIZE 		(13)	/* length of dos name */
#endif

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/*--------------------------------------------------------------*/
/* Enumeration													*/
/*--------------------------------------------------------------*/
/*********************************/
/* playback common function      */
/*********************************/
/* ycc format information of displayed image */
typedef enum {
	E_VGA_FORMAT_YCC420 = 0,		/*< YCC420 */
	E_VGA_FORMAT_YCC422,			/*< YCC422 */
} E_VGA_FORMAT;

/* display window setting type */
typedef enum {
	E_PB_DISP_WIN_TYPE_LCD = 0,		/*< setting for LCD */
	E_PB_DISP_WIN_TYPE_HDMI,		/*< setting for HDMI   */
} E_PB_DISP_WIN_TYPE;

/* display device */
typedef enum {
	E_PB_DISP_DEVICE_LCD = 0,		/*< LCD display  */
	E_PB_DISP_DEVICE_HDMI			/*< HDMI display */
} E_PB_DISP_DEVICE;

/* automatic display */
typedef enum {
	E_PB_AUTODISP_OFF = 0,			/*< disable auto display */
	E_PB_AUTODISP_ON				/*< enable auto display */
} E_PB_AUTODISP;

/* H.264 GOP struct */
typedef enum {
	FJ_H264_GOP_STRUCT_IIII = 0,	/*< use "I" frame only */
	FJ_H264_GOP_STRUCT_IPPP,		/*< use "I" and "P" frame */
	FJ_H264_GOP_STRUCT_IBBP			/*< use "I", "P" and "B" frame */
} FJ_H264_GOP_STRUCT;

/* H.264 Scan Mode */
typedef enum {
	FJ_H264_SCAN_MODE_PROGRESSIVE = 0,	/*< Progressive */
	FJ_H264_SCAN_MODE_INTERLACE,		/*< Interlace */
} FJ_H264_SCAN_MODE;


/*********************************/
/* thumbnail index function      */
/*********************************/
/* thumbnail index display configuration */
typedef enum {
	UINT3_0 = (CHAR)0x00,			/*< number of thumbnails per line/column 0(not supported) */
	UINT3_1 = (CHAR)0x01,			/*< number of thumbnails per line/column 1 */
	UINT3_2 = (CHAR)0x02,			/*< number of thumbnails per line/column 2 */
	UINT3_3 = (CHAR)0x03,			/*< number of thumbnails per line/column 3 */
	UINT3_4 = (CHAR)0x04,			/*< number of thumbnails per line/column 4 */
	UINT3_5 = (CHAR)0x05,			/*< number of thumbnails per line/column 5 */
	UINT3_6 = (CHAR)0x06,			/*< number of thumbnails per line/column 6(not supported) */
	UINT3_7 = (CHAR)0x07			/*< number of thumbnails per line/column 7(not supported) */
} UINT3;


/* source image select */
typedef enum {
	FJ_PLAYBACK_IMAGE_SEL_THM=0,	/*< thumbnail image    */
	FJ_PLAYBACK_IMAGE_SEL_CLS1,		/*< class1 image       */
	FJ_PLAYBACK_IMAGE_SEL_CLS2,		/*< class2 image       */
	FJ_PLAYBACK_IMAGE_SEL_MAIN,		/*< main image	        */
	FJ_PLAYBACK_IMAGE_SEL_END		/*< End of enumeration */
} FJ_PLAYBACK_IMAGE_SEL;

/*--------------------------------------------------------------*/
/* Structure													*/
/*--------------------------------------------------------------*/
/*********************************/
/* playback common function      */
/*********************************/
/* image attribute flags information */
typedef struct {
	USHORT	pict_lock	: 1;		/*< picture locked  (0: not locked, 1: locked) */
	USHORT	exists		: 1;		/*< audio exists  (0: no, 1: yes) */
	USHORT	pudio_lock	: 1;		/*< audio locked  (0: no, 1: yes) */
	USHORT	flash		: 1;		/*< image was captured with flash  (0: no, 1: yes) */
	USHORT	thumb_comp	: 1;		/*< thumbnail compressed  (0: no, 1: yes) */
	USHORT	base_fw		: 1;		/*< captured by BaseFW  (0: no, 1: yes) */
	USHORT	sampling	: 1;		/*< sub-sampling 4:2:2  (0: no, 1: yes) */
	USHORT	audio_format: 1;		/*< audio format supported  (0: no, 1: yes) (currently supported for MJPEG only) */
	USHORT	bits_bixel	: 8;		/*< image compressed bits/pixel */
	USHORT	app11_flg	: 1;		/*< app11 exists flag (0: no, 1: yes) */
} T_FLAGS;

/* image attribute information */
typedef struct {
	T_FLAGS 				wFlags;					/*< flags */
	USHORT					wImageHSize;			/*< main body image holizontal size */
	USHORT					wImageVSize;			/*< main body image vertical size */
	USHORT					wDisplayImageHSize;		/*< display image holizontal size */
	USHORT					wDisplayImageVSize;		/*< display image vertical size */
	USHORT					wThumbnailHSize;		/*< thumbnail holizontal size */
	USHORT					wThumbnailVSize;		/*< thumbnail vertical size */
	USHORT					wClass1HSize;			/*< MPF image (monitor class1) holizontal size */
	USHORT					wClass1VSize;			/*< MPF image (monitor class1) vertical size */
	USHORT					wClass1ThumHSize;		/*< MPF image (monitor class1 thumbnail) holizontal size */
	USHORT					wClass1ThumVSize;		/*< MPF image (monitor class1 thumbnail) vertical size */
	USHORT					wClass2HSize;			/*< MPF image (monitor class2) holizontal size */
	USHORT					wClass2VSize;			/*< MPF image (monitor class2) vertical size */
	USHORT					wClass2ThumHSize;		/*< MPF image (monitor class2 thumbnail) holizontal size */
	USHORT					wClass2ThumVSize;		/*< MPF image (monitor class2 thumbnail) vertical size */
	UINT32					ulCreTimeStamp;			/*< (not use) */
	UINT32					ulModTimeStamp;			/*< (not use) */
	UINT32					ulClipTime; 			/*< movie file time length (in case of JPEG file will be set to "0", ) */
	ULONG					ulTotalFrames;			/*< movie file total frames (in case of JPEG file will be set to "0", ) */
	ULONG					ulTimeScale;			/*< Time Scale (in case of JPEG file will be set to "0", ) */
	ULONG					ulTimeIncrement;		/*< Time Increment (in case of JPEG file will be set to "0", ) */
	ULONG					ulBitRate;				/*< Bit Rate (not used)*/
	USHORT					wPixelAspectRatioH;		/*< Pixel Aspect Ratio (holizontal) (not used) */
	USHORT					wPixelAspectRatioV;		/*< Pixel Aspect Ratio (vertical) (not used) */
	FJ_H264_GOP_STRUCT		GopStruct;				/*< H.264 GOP Struct (in case of JPEG file will be set to "0", ) */
	FJ_H264_SCAN_MODE		VideoScanMode;			/*< H.264 Scan Mode (Progressive or Interlace) */
} IMAGE_ATTRIBUTES;

/* file attribute information */
typedef struct {
	CHAR	name[ATT_FILE_NAME_SIZE];	/*< file name */
	USHORT	attributes;					/*< file attribute */
	LONG	fileSize;					/*< file size */
	USHORT	updateTime;					/*< file update time */
	USHORT	updateDate;					/*< file update date */
} FILE_ATTRIB;

#ifndef _IMAGE_INFO_H_
/* image size */
typedef struct {
	USHORT	width;					/*< image width */
	USHORT	lines;					/*< image height */
} T_IMAGE_SIZE;
#endif	/* _IMAGE_INFO_H_ */

/* image YCC address */
typedef struct {
	ULONG	y_addr;						/*< image Y address */
	ULONG	cb_addr;					/*< image Cb address */
	ULONG	cr_addr;					/*< image Cr address */
} T_YCC_ADDR;

/* image size (include field width) */
typedef struct {
	USHORT	width;					/*< image width */
	USHORT	gl_width;				/*< image global width */
	USHORT	lines;					/*< image height */
} T_PB_IMAGE_SIZE;

/* image size & address information */
typedef struct {
	T_YCC_ADDR		addr;			/*< image address */
	T_PB_IMAGE_SIZE	size;			/*< image size */
	E_VGA_FORMAT	ycc_type;		/*< image ycc type */
} FJ_T_PLAYBACK_SIZE_ADDR;


/* display window parameter */
typedef struct {
	USHORT	pos_x;				/*< window x position */
	USHORT	pos_y;				/*< window y position */
	USHORT	width;				/*< window holizontal size */
	USHORT	lines;				/*< window vartinal size */
	BOOL	ar_flg;				/*< ar flag */
} T_PB_DISP_WINDOW;

/* image kind for playback display */
typedef enum {
	DISP_IMAGE_NON = 0,			/*< display no image */
	DISP_IMAGE_MAIN,			/*< display main body image */
	DISP_IMAGE_THUMB,			/*< display thumbnail image */
	DISP_IMAGE_MPCLS1,			/*< display MPF monitor class1 image */
	DISP_IMAGE_MPCLS1_THUMB,	/*< display MPF monitor class1 image */
	DISP_IMAGE_MPCLS2,			/*< display MPF monitor class2 image */
	DISP_IMAGE_MPCLS2_THUMB		/*< display MPF monitor class2 image */
} E_PB_DISP_IMAGE;


/* image window parameter */
typedef struct {
	USHORT	pos_x;				/*< window x position */
	USHORT	pos_y;				/*< window y position */
	USHORT	width;				/*< window holizontal size */
	USHORT	lines;				/*< window vartinal size */
} T_PB_IMAGE_WINDOW;

/*********************************/
/* thumbnail index function      */
/*********************************/
/* thumbnail index image configuration */
typedef struct {
	UINT3	bThumbPerLine;			/*< number of thumbnails per line */
	UINT3	bThumbPerColumn;		/*< number of thumbnails per column */
	USHORT	bFirstHGap;				/*< horizontal gap size to the first thumbnail for LCD/TV */
	USHORT	bHGap;					/*< horizontal gap size between thumbnails for LCD/TV */
	USHORT	bLastHGap;				/*< horizontal gap size after the last thumbnail for LCD/TV (not use) */
	USHORT	bFirstVGap;				/*< vertical gap size to the first thumbnail for LCD/TV */
	USHORT	bVGap;					/*< vertical gap between thumbnails for LCD/TV */
	USHORT	bLastVGap;				/*< vertical gap size after the last thumbnail for LCD/TV (not use) */
	USHORT	bFirstHGap_HD;			/*< horizontal gap size to the first thumbnail for HDMI */
	USHORT	bHGap_HD;				/*< horizontal gap size between thumbnails for HDMI */
	USHORT	bLastHGap_HD;			/*< horizontal gap size after the last thumbnail for HDMI (not use) */
	USHORT	bFirstVGap_HD;			/*< vertical gap size to the first thumbnail for HDMI */
	USHORT	bVGap_HD;				/*< vertical gap between thumbnails for HDMI */
	USHORT	bLastVGap_HD;			/*< vertical gap size after the last thumbnail for HDMI (not use) */
} THUMBNAIL_CONFIGURATION;

/* thumbnail index error information */
typedef struct {
	INT32	thumb_index;			/*< image index */
	CHAR	thumb_error;			/*< image error info (0: OK, 1: NG) */
} FJ_T_PLAYBACK_THUMBNAILINDEX_INFO;

/*@} --- end of fj_playback_definition */

///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_playback_api
@{*/

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
// PlayBack

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get memory info for still playback.
@param [in] mem_info	BaseFW memory information
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@remarks	BaseFW returns top address and size of each memory buffer.<br>
   For the detail of buffers, please refer to the overview or the description of each member of the structure @ref T_FJ_PLAYBACK_MEMINFO.
*/
extern FJ_ERR_CODE FJ_StillPlayback_GetMemInfo( T_FJ_PLAYBACK_MEMINFO* mem_info );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Decode jpeg data.
@param [in] dec_info	decode information
@param [out] decode_yuv_info	YUV information
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@remarks	Please specify the address got by FJ_StillPlayback_GetMemInfo() to the decoding destination address.
		- Limitations
			- Max file size is up to 36,152,320(7360*4912)bytes. [Note: There is also a memory map size limit]
			- The file must be a Exif standard-compliant JPEG file.
		- Notice
			- Output YUV format is Semi-Planer( Y: Plane, CbCr: Dot).
			- Please specify YCC422 format to the destination image format.
*/
extern FJ_ERR_CODE FJ_StillPlayback_ImageDecode( T_FJ_PLAYBACK_DECODE dec_info, T_FJ_PLAYBACK_YUV_INFO* decode_yuv_info );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function achieves the following conversion.
- YUV data resize
- Rotate
- Convert image format YUV to RGBA
   
@param [in] conv_info	convert information
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@remarks	Please specify the address got by FJ_StillPlayback_GetMemInfo() to the decoding destination address.
		- Limitations
			- Max file size is up to 36,152,320(7360x4912)bytes. [Note: There is also a memory map size limit]
			- Input YUV data format is YCC422 or YCC420 only.
			- Output RGB data format is RGBA8888 only.
			- Global width is must be 16pixels allignment.
			- Height is must be even allignment.
		- Notice
			- Please specify Semi-Planer( Y: Plane, CbCr: Dot) to the output YUV format.
*/
extern FJ_ERR_CODE FJ_StillPlayback_ImageConvert( T_FJ_PLAYBACK_CONVERT conv_info );

/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set image index as current.
@param [in] ImageIndex	image index (includes "directory index" and "object index" as follow)<br>
   <table>
   <tr>
      <th colspan=2></th><th>meaning</th></tr>
   <tr><th rowspan=2 align=center>bit fields</th>
      <td align=center>31 - 16</td><td>directory index (1 - 900)</td></tr>
      <tr>
      <td align=center>15 - 0</td><td>object index (1 - 9999)</td></tr>
   </table>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_SetCurrentImage( INT32 ImageIndex );


/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get current image index.
@retval INT32	image index (includes "directory index" and "object index" as follow)<br>
   <table>
   <tr>
      <th colspan=2></th><th>meaning</th></tr>
   <tr><th rowspan=2 align=center>bit fields</th>
      <td align=center>31 - 16</td><td>directory index (1 - 900)</td></tr>
      <tr>
      <td align=center>15 - 0</td><td>object index (1 - 9999)</td></tr>
   </table>
@endcond
*/
extern INT32 FJ_GetCurrentImage( VOID );


/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets file type as current.
@param [in] FileType	file type<br>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@remarks refer followed definitions about file type<br>
   @ref FJ_FILE_TYPE_JPEG<br>
   @ref FJ_FILE_TYPE_TIFF<br>
   @ref FJ_FILE_TYPE_WAV<br>
   @ref FJ_FILE_TYPE_AVI<br>
   @ref FJ_FILE_TYPE_MPEG<br>
   @ref FJ_FILE_TYPE_MP4<br>
   @ref FJ_FILE_TYPE_ASF<br>
   @ref FJ_FILE_TYPE_WMV<br>
   @ref FJ_FILE_TYPE_WMA<br>
   @ref FJ_FILE_TYPE_MOV<br>
   @ref FJ_FILE_TYPE_TXT<br>
@endcond
*/
extern FJ_ERR_CODE FJ_SetFileType( USHORT FileType );


/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get file attribute informations of current image index file.
@param [out] Attribute	file attribute information<br>
   <table>
   <tr>
      <th>Attribute</th><th>meaning</th></tr>
   <tr>
      <td>name</td>
      <td>file name</td></tr>
   <tr>
      <td>attribute</td>
      <td>file attributes</td></tr>
   <tr>
      <td>fileSize</td>
      <td>file size</td></tr>
   <tr>
      <td>updateTime</td>
      <td>update time<br>
                     <ul>
                     <li>bit[0-4]   seconds devided by 2 (0-29)<br>
                     <li>bit[5-10]  minutes (0-59)<br>
                     <li>bit[11-15] hours (0-23)</ul></td></tr>
   <tr>
      <td>updateDate</td>
      <td>update date<br>
                     <ul>
                     <li>bit[0-4]  day of month (1-31)<br>
                     <li>bit[5-8]  month (1-12)<br>
                     <li>bit[9-15] year relative to 1980</ul></td></tr>
   </table>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_GetFileAttributes( FILE_ATTRIB* Attribute );


/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get image attribute information of current image index file.
@param [out] ImageAttr	image attribute information<br>
   <table>
   <tr>
      <th colspan=2 rowspan=2>ImageAttr</th>
	  <th rowspan=2>meaning</th>
	  <th colspan=2>File Type</th></tr>
   <tr>
	  <th width=80>JPEG</th>
	  <th width=80>MP4</th></tr>
   <tr>
      <td rowspan=9 align=left>wFlags</td>
      <td align=left>pict_lock</td>
	  <td align=left>Picture locked. (0: not locked, 1: locked)</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td align=left>exists</td>
	  <td align=left>Audio Exists. (0: no, 1: yes)</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td align=left>pudio_lock</td>
	  <td align=left>Audio locked. (0: no, 1: yes)</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td align=left>flash</td>
	  <td align=left>Image was captured with flash. (0: no, 1: yes)</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td align=left>thumb_comp</td>
	  <td align=left>Thumbnail compressed. (0: no, 1: yes)</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td align=left>base_fw</td>
	  <td align=left>Captured by BaseFW. (0: no, 1: yes)</td>
	  <td align=center>v</td>
	  <td align=center>v</td></tr>
   <tr>
      <td align=left>sampling</td>
	  <td align=left>Sub-sampling 4:2:2. (0: no, 1: yes)</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td align=left>audio_format</td>
	  <td align=left>Audio format supported. (0: no, 1: yes)</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   <tr>
      <td align=left>bits_bixel</td>
	  <td align=left>Image compressed bits/pixel.</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr></tr>
   <tr>
      <td colspan=2>wImageHSize</td>
	  <td align=left>Captured image horizontal size.</td>
	  <td align=center>v</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>wImageVSize</td>
	  <td align=left>Captured image vertical size.</td>
	  <td align=center>v</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>wDisplayImageHSize</td>
	  <td align=left>Play Display image horizontal size.</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>wDisplayImageVSize</td>
	  <td align=left>Play Display image vertical size.</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>wThumbnailHSize</td>
	  <td align=left>Thumbnail horizontal size.</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>wThumbnailVSize</td>
	  <td align=left>Thumbnail vertical size.</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>wClass1HSize</td>
	  <td align=left>MPF image (monitor class1) holizontal size.</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>wClass1VSize</td>
	  <td align=left>MPF image (monitor class1) vertical size.</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>wClass1ThumHSize</td>
	  <td align=left>MPF image (monitor class1 thumbnail) holizontal size.</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>wClass1ThumVSize</td>
	  <td align=left>MPF image (monitor class1 thumbnail) vertical size.</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>wClass2HSize</td>
	  <td align=left>MPF image (monitor class2) holizontal size.</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>wClass2VSize</td>
	  <td align=left>MPF image (monitor class2) vertical size.</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>wClass2ThumHSize</td>
	  <td align=left>MPF image (monitor class2 thumbnail) holizontal size.</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>wClass2ThumVSize</td>
	  <td align=left>MPF image (monitor class2 thumbnail) vertical size.</td>
	  <td align=center>v</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>ulCreTimeStamp</td>
	  <td align=left>(not used)</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>ulModTimeStamp</td>
	  <td align=left>(not used)</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>&nbsp;</td></tr>
   <tr>
      <td colspan=2>ulClipTime</td>
	  <td align=left>Movie file time length. In case of JPEG will be set to 0.</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>ulTotalFrames</td>
	  <td align=left>Movie file total frames. In case of JPEG will be set to 0.</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>ulTimeScale</td>
	  <td align=left>Time Scale. In case of JPEG will be set to 0.</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>ulTimeIncrement</td>
	  <td align=left>Time Increment. In case of JPEG will be set to 0.<br>
	                 When ulTimeScale is divided with ulTimeIncrement, the number of samples per one second is gotten.</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>ulBitRate</td>
	  <td align=left>Bit Rate. (not used)</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>wPixelAspectRatioH</td>
	  <td align=left>Pixel Aspect Ratio (holizontal). (not used)</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>wPixelAspectRatioV</td>
	  <td align=left>Pixel Aspect Ratio (vertical). (not used)</td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>GopStruct</td>
	  <td align=left>H.264 GOP struct type. In case of JPEG will be set to 0. </td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   <tr>
      <td colspan=2>VideoScanMode</td>
	  <td align=left>H.264 Scan Mode (Progressive or Interlace). In case of JPEG will be set to 0. </td>
	  <td align=center>&nbsp;</td>
	  <td align=center>v</td></tr>
   </table>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@remarks Please do not acquire information on AVI file in the View mode.<br>
         Please refer to the FileType row for attribute information on each FileType. 
@endcond
*/
extern FJ_ERR_CODE FJ_GetImageAttributes( IMAGE_ATTRIBUTES* ImageAttr );

/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function Size of Histogram set with PlayBack is returned.
Please call after Main or Class1_Main or Class2_Main of SingleDisplay. 
@param [out] width	parameter of width<br>
@param [out] lines	parameter of lines<br>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_Get_Pb_Histogram_Size( USHORT* width, USHORT* lines );

/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set automatic playback display by set mode.
Please call before playback display.
@param [in]  auto_disp	automatic display status<br>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_Set_Pb_AutoDisplay( E_PB_AUTODISP auto_disp );

/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get current automatic playback display status by set mode.
@param [out]  auto_disp	automatic display status<br>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_Get_Pb_AutoDisplay( E_PB_AUTODISP* auto_disp );

/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function show playback displayed image.
Please call after playback display.
@param [in] disp_device	display device<br>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_Show_Pb_DispImage( E_PB_DISP_DEVICE disp_device );

/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set of window position and size for playback display.
@param [in] window_type		window size type of display<br>
@param [in] disp_window		parameter of display window<br>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_SetDisplayWindow( E_PB_DISP_WIN_TYPE window_type, T_PB_DISP_WINDOW* disp_window );

/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set of image window position and size for playback display.
@param [in] disp_image		kind of image for image window<br>
@param [in] image_window	parameter of image window<br>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_SetImageWindow( E_PB_DISP_IMAGE disp_image, T_PB_IMAGE_WINDOW* image_window );

/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function cancel of image window position and size for playback display.
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_CancelImageWindow( VOID );

/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets an display image of ThumbnailIndex.
@param [in] Configuration	parameter of ThumbnailIndex<br>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_ConfigThumbIndex( THUMBNAIL_CONFIGURATION* Configuration );

/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function Thumbnail image display process for Thumbnail.
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_Playback_ThumbnailIndex( VOID );

/**
@cond
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function Single display process to Play mode.
@param [in] screen_mode		display screen mode<br>
							FJ_MODE_DISP_IMAGE_MAIN:	display main image<br>
							FJ_MODE_DISP_IMAGE_THUMB:	display thumbnail image<br>
							FJ_MODE_DISP_MONCLS1_MAIN:	display VGAnail image<br>
							FJ_MODE_DISP_MONCLS2_MAIN:	display FHDnail image<br>
@retval FJ_ERR_OK	Normal end.
@retval FJ_ERR_NG	Error occurred.
@endcond
*/
extern FJ_ERR_CODE FJ_Playback_SingleDisplay( FJ_MODE screen_mode );

/*@} --- end of fj_playback_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_playback_message
@{*/
/**
@cond
@def FJ_HM_VALID_IMAGE
File restore process completion message.
@endcond
*/
/*@} --- end of fj_boot_message */
#endif
