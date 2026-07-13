/**
 * @file		osd_config.c
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#ifndef INCLUDE_MADE
#include "osd_config.h"
#include "osd_icon_comm.h"

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
/** Use for output charactor by ASCII code */
static const USHORT* gOsd_Config_Ascii_Table = NULL;

/** gOsd_Config_Ascii_Table's size */
static USHORT  gOsd_Config_Ascii_Table_Size = 0;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
FJ_ERR_CODE BF_Osd_Config_Set_Ascii_Table(USHORT const* const asciiTbl, USHORT tblSize)
{
	// Processing is executable.
	gOsd_Config_Ascii_Table = asciiTbl;
	gOsd_Config_Ascii_Table_Size = tblSize;
	return FJ_ERR_OK;
}

USHORT BF_Osd_Config_Conv_Ascii_To_Icon_ID(UCHAR ascii)
{
	USHORT ret = 0xffff;

	if(gOsd_Config_Ascii_Table == NULL || gOsd_Config_Ascii_Table_Size == 0){
		//Use default convert
		ret = BF_Osd_Icon_Comm_Conv_Ascii_To_Icon_ID(ascii);
	}
	else{
		if(ascii < gOsd_Config_Ascii_Table_Size){
			//Can convert
			ret = gOsd_Config_Ascii_Table[ascii];
		}
	}
	return ret;
}

#endif //INCLUDE_MADE
