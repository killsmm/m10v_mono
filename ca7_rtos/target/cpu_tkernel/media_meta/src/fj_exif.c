/**
 * @file		fj_exif.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_exif.h"
#include "dcf_if.h"
#include "mode.h"
#include "exif.h"

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
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/**
 * @brief		read EXIF header 
 * @param		INT32	uwDcfIndex
 *				INT32	uwIfdIndex
 *				INT32	uwTagID
 *				INT32	pulCount
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
#ifdef CO_BF_EXIF_DIRECT
FJ_ERR_CODE FJ_GetImageEXIF( LONG uwDcfIndex, USHORT uwIfdIndex, USHORT uwTagId, BYTE* puData, USHORT* pulCount )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getimageexif( uwDcfIndex, uwIfdIndex, uwTagId, puData, pulCount );

	return err_code;
}
	
#else
FJ_ERR_CODE FJ_GetImageEXIF( USHORT uwDcfIndex, USHORT uwIfdIndex, USHORT uwTagId, USHORT* pulCount )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getimageexif( uwDcfIndex, uwIfdIndex, uwTagId, pulCount );

	return err_code;
}
#endif


/**
 * @brief		updated EXIF header 
 * @param		INT32	IfdIndex
 *				INT32	TagId
 *				INT32	Type
 *				INT32	Count
 *				INT32	SupportLevel
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_SetImageEXIF( USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_setimageexif( IfdIndex, TagId, Type, Count, SupportLevel, Data );

	return err_code;
}

/**
 * @brief		updated EXIF header for MPF class1
 * @param		INT32	IfdIndex
 *				INT32	TagId
 *				INT32	Type
 *				INT32	Count
 *				INT32	SupportLevel
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_SetImageEXIF_Class1( USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_setimageexif_class1( IfdIndex, TagId, Type, Count, SupportLevel, Data );

	return err_code;
}

/**
 * @brief		updated EXIF header for MPF class2
 * @param		INT32	IfdIndex
 *				INT32	TagId
 *				INT32	Type
 *				INT32	Count
 *				INT32	SupportLevel
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_SetImageEXIF_Class2( USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_setimageexif_class2( IfdIndex, TagId, Type, Count, SupportLevel, Data );

	return err_code;
}

