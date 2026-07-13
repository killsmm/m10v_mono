/**
 * @file		fs_user_func.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FS_USER_FUNC_H_
#define _FS_USER_FUNC_H_

#include "fj_filesystem.h"
#ifdef CO_DCF_EXIF_ON
#include "fj_exif.h"
#endif // CO_DCF_EXIF_ON
#include "fj_memory.h"

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
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
// media user function
extern FJ_ERR_CODE BF_Media_Mount(FJ_MEDIA_ID target);

// file user function
extern FJ_ERR_CODE BF_Filesystem_File_Reset_Directory(VOID);
extern FJ_ERR_CODE BF_Filesystem_File_Reset_Directory_Number(VOID);

// exif user function
extern VOID			BF_Exif_Reset_Index(VOID);
extern FJ_ERR_CODE	BF_Exif_Set_Mem_Location_Ptr(FJ_MEMORY_LOCATION Location, CHAR** Buffer, INT32 Size);
extern FJ_ERR_CODE	BF_Exif_Unset_Mem_Location_Ptr( FJ_MEMORY_LOCATION Location );

// dcf user function
// dpof user function


#endif	// _FS_USER_FUNC_H_
