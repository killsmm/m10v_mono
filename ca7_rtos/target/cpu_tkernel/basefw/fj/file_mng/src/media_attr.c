/**
 * @file		media_attr.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include <string.h>
#include "media_attr.h"
#include "fs_user_custom.h"
#include "debug.h"
#include "fs_if.h"
#include "media_drv_wrapper.h"


/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
//#define CO_BF_MEDIA_ATTR_EXT_MEDIA_DEFAULT_SD

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static volatile T_BF_MEDIA_ATTR g_bf_media_attr = {
	0,						/* sync */
	0,						/* volume_label */
	30,						/* time_out */
	500 * 1024,				/* dpof_size */
	{ "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0", },	/* sz_vol_label[] */
	E_BF_MEDIA_ATTR_EXT_SD		/* ext_media */
};

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
VOID             BF_Media_Attr_Set_Attr_Sync(USHORT sync)
{
	g_bf_media_attr.sync = sync;
}

USHORT           BF_Media_Attr_Get_Attr_Sync(VOID)
{
	return g_bf_media_attr.sync;
}

VOID             BF_Media_Attr_Set_Attr_Volume_Label(USHORT volume_label)
{
	g_bf_media_attr.volume_label = volume_label;
}

USHORT           BF_Media_Attr_Get_Attr_Volume_Label(VOID)
{
	return g_bf_media_attr.volume_label;
}

VOID             BF_Media_Attr_Set_Attr_Time_Out(USHORT time_out)
{
	g_bf_media_attr.time_out = time_out;
}

USHORT           BF_Media_Attr_Get_Attr_Time_Out(VOID)
{
	return g_bf_media_attr.time_out;
}

VOID             BF_Media_Attr_Set_Attr_Dpof_Size(ULONG dpof_size)
{
	g_bf_media_attr.dpof_size = dpof_size;
}

ULONG            BF_Media_Attr_Get_Attr_Dpof_Size(VOID)
{
	return g_bf_media_attr.dpof_size;
}

VOID             BF_Media_Attr_Set_Attr_Sz_Vol_Label(FW_INT32 media_id, UCHAR* sz_vol_label)
{
	strncpy((char*)g_bf_media_attr.sz_vol_label[media_id], (const char*)sz_vol_label, sizeof(g_bf_media_attr.sz_vol_label[0]));
}

UCHAR*           BF_Media_Attr_Get_Attr_Sz_Vol_Label(FW_INT32 media_id)
{
	return (UCHAR*)g_bf_media_attr.sz_vol_label[media_id];
}

INT32            BF_Media_Attr_Set_Attr_Ext_Media(E_BF_MEDIA_ATTR_EXT ext_media)
{
	if (ext_media == E_BF_MEDIA_ATTR_EXT_SD) {
		BF_Debug_Print_Information(("SD-Card ch0 mode\n"));
	}
	else if (ext_media == E_BF_MEDIA_ATTR_EXT_SD1) {
		BF_Debug_Print_Information(("SD-Card ch1 mode\n"));
	}
	else {
		BF_Debug_Print_Error(("parameter error.\n"));
		return -1;
	}

	g_bf_media_attr.ext_media = ext_media;
	return 0;
}

E_BF_MEDIA_ATTR_EXT BF_Media_Attr_Get_Attr_Ext_Media(VOID)
{
	return g_bf_media_attr.ext_media;
}
