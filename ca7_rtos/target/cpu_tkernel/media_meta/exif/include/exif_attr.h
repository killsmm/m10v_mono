/**
 * @file		exif_attr.h
 * @brief		SocioNext Exif Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_EXIF_ATTR_H
#define _FJ_EXIF_ATTR_H

#include "ddim_typedef.h"

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
// EXIF Attribute Data
typedef struct {
	USHORT	maker_note_size;
//	USHORT	tag_auto;
	USHORT	appn_size;
} T_BF_EXIF_ATTR;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
VOID   BF_Exif_Attr_Set_Attr_Maker_Note_Size(USHORT maker_note_size);
USHORT BF_Exif_Attr_Get_Attr_Maker_Note_Size(VOID);
//VOID   BF_Exif_Attr_Set_Attr_Tag_Auto(USHORT tag_auto);
//USHORT BF_Exif_Attr_Get_Attr_Tag_Auto(VOID);
VOID   BF_Exif_Attr_Set_Attr_Appn_Size(USHORT appn_size);
USHORT BF_Exif_Attr_Get_Attr_Appn_Size(VOID);


#endif
