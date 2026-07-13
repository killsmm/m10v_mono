/**
 * @file		osd_config.h
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _OSD_CONFIG_H_
#define _OSD_CONFIG_H_

#ifndef INCLUDE_MADE
#include "osd_config_comm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

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
The translation table from ASCII code to Font-OS_USER_ID.<br>
Set for the following functions.<br>
	- @ref FJ_OSDTextOut<br>
@param[in] asciiTbl	The translation table from ASCII code to Font-OS_USER_ID.<br>
					Please specify the pointer of the array of USHORT.<br>
					Converted FONT-OS_USER_ID is specified for the array element corresponding to ASCII code.<br>
					The element is a font OS_USER_ID and the index is ASCII code - 0x20.<br>
@param[in] tblSize	Size of translation table.<br>
@retval FJ_ERR_OK	Normal End.
*/
FJ_ERR_CODE BF_Osd_Config_Set_Ascii_Table(USHORT const* const asciiTbl, USHORT tblSize);

/**
Get String OS_USER_ID from ASCII code
@param ascii[in] ASCII code
@retval 0xffff	Error.
@retval Other	Font OS_USER_ID.
@note	it can be convertd Latin 1 only
*/
USHORT BF_Osd_Config_Conv_Ascii_To_Icon_ID(UCHAR ascii);


#endif // INCLUDE_MADE
#endif // _OSD_CONFIG_H_
