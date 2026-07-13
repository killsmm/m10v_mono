/**
 * @file		media_attr.h
 * @brief		SocioNext Media Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_MEDIA_ATTR_H
#define _FJ_MEDIA_ATTR_H

#include "ddim_typedef.h"
#include "fs_controller.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
typedef enum {
	E_BF_MEDIA_ATTR_EXT_MS = 0,
	E_BF_MEDIA_ATTR_EXT_SD = 1,
	E_BF_MEDIA_ATTR_EXT_SD1 = 2
} E_BF_MEDIA_ATTR_EXT;

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// MEDIA Attribute Data
typedef struct {
	USHORT				sync;
	USHORT				volume_label;
	USHORT				time_out;
	ULONG				dpof_size;
	UCHAR				sz_vol_label[D_BF_FS_CONTROLLER_MEDIA_ID_NUM][12];
	E_BF_MEDIA_ATTR_EXT	ext_media;
} T_BF_MEDIA_ATTR;

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
VOID             BF_Media_Attr_Set_Attr_Sync(USHORT sync);
USHORT           BF_Media_Attr_Get_Attr_Sync(VOID);
VOID             BF_Media_Attr_Set_Attr_Volume_Label(USHORT volume_label);
USHORT           BF_Media_Attr_Get_Attr_Volume_Label(VOID);
VOID             BF_Media_Attr_Set_Attr_Time_Out(USHORT time_out);
USHORT           BF_Media_Attr_Get_Attr_Time_Out(VOID);
VOID             BF_Media_Attr_Set_Attr_Dpof_Size(ULONG dpof_size);
ULONG            BF_Media_Attr_Get_Attr_Dpof_Size(VOID);
VOID             BF_Media_Attr_Set_Attr_Sz_Vol_Label(FW_INT32 media_id, UCHAR* sz_vol_label);
UCHAR*           BF_Media_Attr_Get_Attr_Sz_Vol_Label(FW_INT32 media_id);
INT32            BF_Media_Attr_Set_Attr_Ext_Media(E_BF_MEDIA_ATTR_EXT ext_media);
E_BF_MEDIA_ATTR_EXT BF_Media_Attr_Get_Attr_Ext_Media(VOID);


#endif
