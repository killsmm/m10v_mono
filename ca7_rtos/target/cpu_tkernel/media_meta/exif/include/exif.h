/**
 * @file		exif.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _EXIF_H_
#define _EXIF_H_

#include "ddim_typedef.h"
#include "fj_custom.h"
#include "fj_memory.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define CO_BF_EXIF_DIRECT

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
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/

//////////////////////////////////
// EXIF API (romarea_boot)
//////////////////////////////////
//extern VOID BF_Exif_Reset_Index(VOID);
//extern FJ_ERR_CODE BF_Exif_Set_Mem_Location_Ptr( FJ_MEMORY_LOCATION Location, CHAR** Buffer, INT32 Size );

//////////////////////////////////
// EXIF API (romarea_pb)
//////////////////////////////////
#ifdef CO_BF_EXIF_DIRECT
extern FJ_ERR_CODE fj_getimageexif( LONG uwDcfIndex, USHORT uwIfdIndex, USHORT uwTagId, BYTE* puData, USHORT* pulCount );
#else // CO_BF_EXIF_DIRECT
extern FJ_ERR_CODE fj_getimageexif( USHORT uwDcfIndex, USHORT uwIfdIndex, USHORT uwTagId, USHORT* pulCount );
#endif // CO_BF_EXIF_DIRECT

//////////////////////////////////
// EXIF API (romarea_last)
//////////////////////////////////
extern FJ_ERR_CODE fj_setimageexif( USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data );
extern FJ_ERR_CODE fj_setimageexif_class1( USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data );
extern FJ_ERR_CODE fj_setimageexif_class2( USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data );
#ifdef FJ_DEBUG_PRINT_ON
extern INT32	BF_Exif_Get_Exif_Info_Jpeg_Name( CHAR* file_path );
#endif // FJ_DEBUG_PRINT_ON

#endif	// _EXIF_H_
