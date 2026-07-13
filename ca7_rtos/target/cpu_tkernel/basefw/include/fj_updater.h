/**
 * @file		fj_updater.h
 * @brief		Updater Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_UPDATER_H
#define _FJ_UPDATER_H

#include "ddim_typedef.h"
#include "fj_std.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/**
@weakgroup fj_updater_overview
@{
	The Updater controls can be achieved by using this API.<br>
 	This API can use IDLE mode only.<br>
 	Mount of external media is required in advance. 

@}*//* --- end of fj_updater_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_updater_sequence
@{
	- @ref updater_file_sequence
	- @ref updater_memory_sequence
	<hr>
	@section updater_file_sequence Updater file sequence
	@image html updater_sequence.png
	<hr>
	@section updater_memory_sequence Updater memory sequence
	@image html updater_memory_sequence.png
@}*//* --- end of fj_updater_sequence */

/** @weakgroup fj_updater_definition
@{*/
/*----------------------------------------------------------------------*/
/* Definition														    */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration														    */
/*----------------------------------------------------------------------*/
/**
Partition area
*/
typedef enum { 
  E_FJ_UPDATER_PARTITION_DATA0 = 0,			/**< Partition data 0 */ 
  E_FJ_UPDATER_PARTITION_DATA1 = 1,			/**< Partition data 1 */ 
  E_FJ_UPDATER_PARTITION_DATA2 = 2,			/**< Partition data 2 */ 
  E_FJ_UPDATER_PARTITION_DATA3 = 3,			/**< Partition data 3 */ 
  E_FJ_UPDATER_PARTITION_DATA4 = 4,			/**< Partition data 4 */ 
  E_FJ_UPDATER_PARTITION_DATA5 = 5,			/**< Partition data 5 */ 
  E_FJ_UPDATER_PARTITION_DATA6 = 6,			/**< Partition data 6 */ 
  E_FJ_UPDATER_PARTITION_DATA7 = 7,			/**< Partition data 7 */ 
  E_FJ_UPDATER_PARTITION_DATA8 = 8,			/**< Partition data 8 */ 
  E_FJ_UPDATER_PARTITION_DATA9 = 9,			/**< Partition data 9 */ 
  E_FJ_UPDATER_PARTITION_DATA10 = 10,		/**< Partition data 10 */ 
  E_FJ_UPDATER_PARTITION_DATA11 = 11,		/**< Partition data 11 */ 
  E_FJ_UPDATER_PARTITION_DATA12 = 12,		/**< Partition data 12 */ 
  E_FJ_UPDATER_PARTITION_DATA13 = 13,		/**< Partition data 13 */ 
  E_FJ_UPDATER_PARTITION_DATA14 = 14,		/**< Partition data 14 */ 
  E_FJ_UPDATER_PARTITION_DATA15 = 15		/**< Partition data 15 */ 
} E_FJ_UPDATER_PARTITION;

/*----------------------------------------------------------------------*/
/* Structure														    */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data														    */
/*----------------------------------------------------------------------*/
// Nothing Special
/*@} --- end of fj_updater_definition */

/**
@weakgroup fj_updater_api
@{*/
/*----------------------------------------------------------------------*/
/* Macro															    */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function															    */
/*----------------------------------------------------------------------*/
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function update the frimware on NandFlash/eMMC.(use the file data)
@param[in] filename Update file name.<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG  Error occurred.
@remarks
 This API can use IDLE mode only.<br>
 Mount of external media is required in advance. <br>
 Updated data will be used after system reboot.<br>
*/
FJ_ERR_CODE	FJ_Updater(UCHAR* filename);
FJ_ERR_CODE FJ_Updater_SecureUpdate(UCHAR* filename);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function update the partition data on NandFlash/eMMC.(use the memory data)
@param[in] area update partition area.<br>
@param[in] data_addr update data address.<br>
@param[in] size update data size.<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG  Error occurred.
@remarks
 This API can use IDLE mode only.<br>
 Updated data will be used after system reboot.<br>
*/
FJ_ERR_CODE	FJ_Updater_Memory(E_FJ_UPDATER_PARTITION area, UINT32 *data_addr, UINT32 size);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function update the partition data on NandFlash/eMMC.(use the memory data)
@param[in] data_addr update data address.<br>
@param[in] size update data size.<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG  Error occurred.
@remarks
 This API can use IDLE mode only.<br>
 Updated data will be used after system reboot.<br>
*/
FJ_ERR_CODE	FJ_Updater_Memory_All(UINT32 *data_addr, UINT32 size);
/*@} --- end of fj_updater_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_updater_message
@{*/
/*@} --- end of fj_updater_message */

///////////////////////////////////////////////////////////////////////////////
// callback
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_updater_callback
@{*/
/*@} --- end of fj_updater_callback */

#endif	// _FJ_UPDATER_H
