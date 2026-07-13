/**
 * @file		osd_image_comm.h
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _OSD_IMAGE_COMM_H_
#define _OSD_IMAGE_COMM_H_

#include "osd_draw_comm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_BF_OSD_IMAGE_COMM_PHOTOFRAME_FNAME_MAX_LEN	(46)	/**< File name length */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Image file kind */
typedef enum {
	E_BF_OSD_IMAGE_COMM_FILE_KIND_PHOTFRAME = 0,	/**< Photoframe */
	E_BF_OSD_IMAGE_COMM_FILE_KIND_JPEG,				/**< Jpeg */
	E_BF_OSD_IMAGE_COMM_FILE_KIND_ERROR				/**< Error */
} E_BF_OSD_IMAGE_COMM_FILE_KIND;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

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
/**
Draw a compressed file of RGBA8888/4444 to VGR
@param[in]	disp_no_data	layer ID
@param[in]	wFileType	file type
@param[in]	file_name	file name
@param[in]	start_x	Draw X position
@param[in]	start_y	Draw Y position
@param[in]	width	Draw width
@param[in]	hight	Draw hight
@retval		FJ_ERR_OK	OK
@retval		other		NG
*/
extern FJ_ERR_CODE BF_Osd_Image_Comm_Draw_File(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data,FJ_GR_FILE_TYPE wFileType,
								const CHAR* file_name, USHORT start_x, USHORT start_y, USHORT width, USHORT hight);

/**
Check jpeg file name. Set 320*240 file name if it's a photoframe
@param[in]	FileName	photoframe file name
@param[in]	photoframe_attribute_file_name	attribute file name
@param[in]	photoframeDispFileName	display file name
@retval		E_BF_OSD_IMAGE_COMM_FILE_KIND_JPEG		JPEG
@retval		E_BF_OSD_IMAGE_COMM_FILE_KIND_PHOTFRAME	PHOTFRAME
@retval		E_BF_OSD_IMAGE_COMM_FILE_KIND_ERROR		ERROR
*/
extern E_BF_OSD_IMAGE_COMM_FILE_KIND BF_Osd_Image_Comm_Check_Jpeg_File_Name(const char** FileName, char* photoframe_attribute_file_name, char* photoframeDispFileName);

/**
Draw a jpeg file to HD
@param[in]	wFileType	file type
@param[in]	file_name	file name
@param[in]	start_x	Draw X position
@param[in]	start_y	Draw Y position
@param[in]	width	Draw width
@param[in]	hight	Draw hight
@retval		FJ_ERR_OK	OK
@retval		other		NG
*/
extern FJ_ERR_CODE BF_Osd_Image_Comm_Draw_Jpeg_To_HD(FJ_GR_FILE_TYPE wFileType, CHAR* file_name, 
									  USHORT start_x, USHORT start_y, USHORT width, USHORT hight);

/**
Display image file (OSD layer)
@param[in]	layerID		layer ID
@param[in]	wFileType	file type
@param[in]	FileName	file name
@param[in]	uStartX	Draw X position
@param[in]	uStartY	Draw Y position
@param[in]	uWidth	Draw width
@param[in]	uHight	Draw hight
@param[in]	drawAreaNum	Draw area number
@retval		FJ_ERR_OK	OK
@retval		other		NG
*/
extern FJ_ERR_CODE BF_Osd_Drawfile(E_OSDLayer layerID, FJ_GR_FILE_TYPE wFileType, CHAR* FileName,
						USHORT uStartX, USHORT uStartY, USHORT uWidth, USHORT uHight, BYTE drawAreaNum);


#endif // _OSD_IMAGE_COMM_H_
