/**
 * @file		fj_updater.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_updater.h"
#include "updater.h"
#include "mode.h"

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
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/////////////////////////////////////////////////////
// romarea_boot
/////////////////////////////////////////////////////

/**
 * @brief		update the frimware on NandFlash.(use the file data)
 * @param		UCHAR*	filename
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Updater(UCHAR* filename)
{
	fj_updater_set_secure_enable(FALSE);
	return fj_updater(filename);
}

FJ_ERR_CODE FJ_Updater_SecureUpdate(UCHAR* filename)
{
	fj_updater_set_secure_enable(TRUE);
	return fj_updater(filename);
}

/**
 * @brief		update the partition data on NandFlash.(use the memory data)
 * @param		E_FJ_UPDATER_PARTITION area  : upadte partition area
 * 				UINT32* data_addr : update data address
 * 				UINT32 size : update file size
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Updater_Memory(E_FJ_UPDATER_PARTITION area, UINT32 *data_addr, UINT32 size)
{
	fj_updater_set_secure_enable(FALSE);
	return fj_updater_memory((UCHAR)area, data_addr, size);
}


/**
 * @brief		update the frimware on NandFlash.(use the memory data )
 * @param		UINT32* data_addr : Storage address of FW data to be updated. frimware data with header.
 * 				UINT32 size : update file size
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Updater_Memory_All(UINT32 *data_addr, UINT32 size)
{
	fj_updater_set_secure_enable(FALSE);
	return fj_updater_memory_all(data_addr, size);
}


