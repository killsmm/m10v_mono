/**
 * @file		exif_attr.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_custom.h"
#include "exif_attr.h"
#include "sdram_map_common.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

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
static volatile T_BF_EXIF_ATTR g_bf_exif_attr = {
	40,						/* maker_note_size */
//	0,						/* tag_auto */
	0,						/* appn_size */
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
VOID   BF_Exif_Attr_Set_Attr_Maker_Note_Size(USHORT maker_note_size)
{
	g_bf_exif_attr.maker_note_size = maker_note_size;
}

USHORT BF_Exif_Attr_Get_Attr_Maker_Note_Size(VOID)
{
	return g_bf_exif_attr.maker_note_size;
}

#if 0
VOID BF_Exif_Attr_Set_Attr_Tag_Auto(USHORT tag_auto)
{
	g_bf_exif_attr.tag_auto = tag_auto;
}

USHORT BF_Exif_Attr_Get_Attr_Tag_Auto(VOID)
{
	return g_bf_exif_attr.tag_auto;
}
#endif
VOID   BF_Exif_Attr_Set_Attr_Appn_Size(USHORT appn_size)
{
	g_bf_exif_attr.appn_size = appn_size;
}

USHORT BF_Exif_Attr_Get_Attr_Appn_Size(VOID)
{
	return g_bf_exif_attr.appn_size;
}
