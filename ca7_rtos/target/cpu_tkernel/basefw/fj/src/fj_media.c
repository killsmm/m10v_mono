/**
 * @file		fj_media.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "media.h"
#include "mode.h"
// --- REMOVE_USB BEGIN ---
#include "filesystem.h"
// --- REMOVE_USB END ---
#ifndef CO_LINUX_VERSION_ON
#ifndef CO_TKERNEL_ONLY
#include "os_user_custom.h"
#include "sdram_map_define.h"
#endif /* CO_TKERNEL_ONLY */
#endif /* CO_LINUX_VERSION_ON */

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#ifndef CO_LINUX_VERSION_ON
#ifndef CO_TKERNEL_ONLY
#define D_BOOT_MAGIC_NUM_ADDR		( SDRAM_ADR_LINUX_RTOS_COMMON_IPCU_SYNC + 0x40 )
#endif /* CO_TKERNEL_ONLY */
#endif /* CO_LINUX_VERSION_ON */

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
//static INT32	gFormat_ret;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special
//static VOID media_format_callback(INT32 ret);

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * @brief 		Get media detected status.
 * @param		UCHAR ch
 * @param		FW_UCHAR*	detect : media detected state
 *							1 : media inserted
 *							0 : media not inserted
 * @return 		None
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_Set_MediaDetect(UCHAR ch, UCHAR time_per_sampling, UCHAR num_of_sampling)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_set_mediadetect(ch, time_per_sampling, num_of_sampling);
}

FJ_ERR_CODE FJ_Get_MediaDetect(UCHAR ch, UCHAR* detect)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	fj_get_mediadetect(ch, detect);

	return FJ_ERR_OK;
}

/**
 * @brief 		initialize the SD macro
 * @param		UCHAR ch
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SD_Init(UCHAR ch)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_init(ch);
}

/**
 * @brief 		Set limit of the SD drive strength, speed mode and clock
 * @param		UCHAR ch
 * @param		FJ_MEDIA_SD_DRIVE drive
 * @param		FJ_MEDIA_SD_MODE mode
 * @param		FJ_MEDIA_SD_CLOCK clock
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SD_Config(UCHAR ch, FJ_MEDIA_SD_DRIVE drive, FJ_MEDIA_SD_MODE mode, FJ_MEDIA_SD_CLOCK clock)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_config(ch, drive, mode, clock);
}

/**
 * @brief 		Set SD callback function
 * @param		UCHAR ch
 * @param		FJ_MEDIA_SD_CB_EVENT type
 * @param		SD_CALLBACK pcallback
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SD_Set_Event_CB(UCHAR ch, FJ_MEDIA_SD_CB_EVENT type, SD_CALLBACK pcallback)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_set_event_cb(ch, type, pcallback);
}

/**
 * @brief 		initialize the SD card
 * @param		UCHAR ch
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SD_Card_Init(UCHAR ch)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_card_init(ch);
}

/**
 * @brief 		initialize the SD card
 * @param		UCHAR ch
 * @param		INT32* mode
 * @param		ULONG* clock
 * @param		UCHAR* wp_status
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SD_GetCardInfo(UCHAR ch, INT32* mode, ULONG* clock, UCHAR* wp_status)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_getcardinfo(ch, mode, clock, wp_status);
}

/**
 * @brief 		Set hold clock of the SD
 * @param		UCHAR ch
 * @param		FJ_MEDIA_SD_HOLD_CLOCK hold
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SD_Set_Hold_Clock(UCHAR ch, FJ_MEDIA_SD_HOLD_CLOCK hold)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_set_hold_clock(ch, hold);
}

/**
 * @brief 		Set the SD Card Lock password
 * @param		UCHAR ch
 * @param		CHAR* password
 *				ULONG size
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SD_Set_CardLockPassword(UCHAR ch, CHAR* password, ULONG size)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_set_cardlockpassword(ch, password, size);;
}

/**
 * @brief 		Set the SD Card Lock operation
 * @param		UCHAR ch
 * @param		FJ_MEDIA_SD_LOCK_OPE operation
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SD_Set_CardLock(UCHAR ch, FJ_MEDIA_SD_LOCK_OPE operation)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_set_cardlock(ch, operation);
}

/**
 * @brief 		Set the SD Card power supply
 * @param		UCHAR ch
 * @param		UCHAR enable
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SD_Set_Card_Power(UCHAR ch, UCHAR enable)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_set_card_power(ch, enable);
}

/**
 * @brief		Set limit of the SD UHS2 mode, speed range and power limit
 * @param		UCHAR ch
 * @param		FJ_MEDIA_SD_UHS2_MODE mode
 * @param		FJ_MEDIA_SD_UHS2_RANGE range
 * @param		FJ_MEDIA_SD_UHS2_PWR power
 * @return		FJ_ERR_CODE : FJ_ERR_OK
 *                          : FJ_ERR_NG
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_SD_Set_UHS2_ModeSelect(UCHAR ch, FJ_MEDIA_SD_UHS2_MODE mode, FJ_MEDIA_SD_UHS2_RANGE range, FJ_MEDIA_SD_UHS2_PWR power)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_set_uhs2_modeselect(ch, mode, range, power);
}

/**
 * @brief		Get the SD UHS2 mode, speed range and power limit
 * @param		UCHAR ch
 * @param		UINT32* mode
 * @param		UINT32* range
 * @param		UINT32* power
 * @return		FJ_ERR_CODE : FJ_ERR_OK
 *                          : FJ_ERR_NG
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_SD_Get_UHS2_ModeSelect(UCHAR ch, UINT32* mode, UINT32* range, UINT32* power)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_get_uhs2_modeselect(ch, mode, range, power);
}

/**
 * @brief 		Set the SD UHS2 power saving mode
 * @param		UCHAR ch
 * @param		FJ_MEDIA_SD_UHS2_PMODE mode
 * @param		UCHAR enable
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SD_Set_UHS2_PowerSaving(UCHAR ch, FJ_MEDIA_SD_UHS2_PMODE mode, UCHAR enable)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_set_uhs2_powersaving(ch, mode, enable);
}

/**
 * @brief 		Set detection time of the SD UHS-II card
 * @param		UCHAR ch
 * @param		UINT32 time
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SD_Set_UHS2_DetectTime(UCHAR ch, UINT32 time)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_sd_set_uhs2_detecttime(ch, time);
}

/**
 * @brief 		Initialize the NandFlash macro.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Init(VOID)
{
#ifndef CO_LINUX_VERSION_ON
#ifndef CO_TKERNEL_ONLY
	// RTOS must wait until linux writes the magic number that indicates it has finished its hibernation processing.
	while( 1 ) {
		if( *(volatile unsigned int*)( D_BOOT_MAGIC_NUM_ADDR ) == 0xBEEFCAFE ) {
			/* NF available */
			break;
		}
		/* wait 10ms */
	    OS_User_Dly_Tsk(10);
	}
#endif /* CO_TKERNEL_ONLY */
#endif /* CO_LINUX_VERSION_ON */
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_init();
}

/**
 * @brief 		Set the attribute of the NandFlash.
 * @param		FJ_NF_ATTRIBUTE attr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Config(FJ_NF_ATTRIBUTE* attr)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_config(attr);
}

/**
 * @brief 		Initialize the NandFlash device.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Device_Init(VOID)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_device_init();
}

/**
 * @brief 		Gets the Manufacturer ID and Device ID of the NandFlash.
 * @param		UINT32* mid
 * @param		UINT32* did
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Get_MID_DID(UINT32* mid, UINT32* did)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_get_mid_did(mid, did);
}

/**
 * @brief 		Gets the device information of the NandFlash.
 * @param		FJ_NF_DEV_INFO* nf_dev_info
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Get_Device_Info(FJ_NF_DEV_INFO* nf_dev_info)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_get_device_info(nf_dev_info);
}

/**
 * @brief 		NF All Erase.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Erase(VOID)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_erase();
}

/**
 * @brief 		Writes the data of specified block of the NandFlash.
 * @param		ULONG phy_blk
 * @param		UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Write_Physical_Data(ULONG phy_blk, UINT32* data_addr)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_write_physical_data(phy_blk, data_addr);
}

/**
 * @brief 		Reads the data of specified block of the NandFlash.
 * @param		ULONG phy_blk
 * @param		UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Read_Physical_Data(ULONG phy_blk, UINT32* data_addr)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_read_physical_data(phy_blk, data_addr);
}

/**
 * @brief 		Write data to NandFlash Logical sector.
 * @param		ULONG  start_sec
 *				ULONG  write_sec_num
 *				UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Write_Logical_Sector(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_write_logical_sector(start_sec, write_sec_num, data_addr);
}

/**
 * @brief 		Read data from NandFlash Logical sector.
 * @param		ULONG  start_sec
 *				ULONG  read_sec_num
 *				UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NF_NO_ALLOCATE 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Read_Logical_Sector(ULONG start_sec, ULONG read_sec_num, UINT32* data_addr)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_read_logical_sector(start_sec, read_sec_num, data_addr);
}

/**
 * @brief 		Write data to NandFlash Logical sector(not use interrupt/OS).
 * @param		ULONG  start_sec
 *				ULONG  write_sec_num
 *				UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Write_Logical_Sector_PIO(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_write_logical_sector_pio(start_sec, write_sec_num, data_addr);
}

/**
 * @brief 		Partition of NandFlash.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Partition(VOID)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_partition();
}

/**
 * @brief 		Read partition information from the logical space of NandFlash.
 * @param		FJ_NF_PARTITION* part_info
 * @param		ULONG* part_num
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Partition_Read(FJ_NF_PARTITION* part_info, ULONG* part_num)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_partition_read(part_info, part_num);
}

/**
 * @brief 		Get the start sector number and size of the selected Partition.
 * @param		E_MEDIA_NF_PARTITION_NAME name
 * @param		ULONG* start_sec_num
 * @param		ULONG* data_size
 * @param		ULONG* total_size
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Get_Partition_Info(E_MEDIA_NF_PARTITION_NAME name, ULONG* start_sec_num, ULONG* data_size, ULONG* total_size)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_get_partition_info(name, start_sec_num, data_size, total_size);
}

/**
 * @brief 		Get number of badblock.
 * @param		ULONG limit
 * @param		FJ_MEDIA_NF_BB_SEARCH_AREA mode
 * @param		ULONG* num
 * @param		UINT32* buf
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Get_BadBlock_Num(ULONG limit, FJ_MEDIA_NF_BB_SEARCH_AREA mode, ULONG* num, UINT32* buf)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_nf_get_badblock_num(limit, (UCHAR)mode, num, buf);
}

/**
 * @brief 		Get parameter block info.
 * @param		ULONG* param_blk_top
 *				ULONG* param_blk_size
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Get_ParamBlk_Info(ULONG* param_blk_top, ULONG* param_blk_size)
{
	return fj_nf_get_paramblk_info(param_blk_top, param_blk_size);
}

/**
 * @brief 		Get exception log info.
 * @param		UINT32* exception_log_top
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_NF_Get_ExceptionLog_Info(UINT32* exception_log_top)
{
	return fj_nf_get_exceptionlog_info(exception_log_top);
}

/**
 * @brief 		Set retry count.
 * @param		UCHAR retry_cnt_read
 *				UCHAR retry_cnt_write
 * @return 		None
 * @note 		None
 * @attention 	None
 */
VOID FJ_NF_Set_Retry_Cnt(UCHAR retry_cnt_read, UCHAR retry_cnt_write)
{
	fj_nf_set_retry_cnt(retry_cnt_read, retry_cnt_write);

	return;
}

/**
 * @brief 		Get retry count.
 * @param		UCHAR* retry_cnt_read
 *				UCHAR* retry_cnt_write
 * @return 		None
 * @note 		None
 * @attention 	None
 */
VOID FJ_NF_Get_Retry_Cnt(UCHAR* retry_cnt_read, UCHAR* retry_cnt_write)
{
	fj_nf_get_retry_cnt(retry_cnt_read, retry_cnt_write);

	return;
}

/**
 * @brief 		Set timeout.
 * @param		FJ_MEDIA_NF_TIMEOUT type :
											FJ_MEDIA_NF_TIMEOUT_RESET	(0) : reset
											FJ_MEDIA_NF_TIMEOUT_DMA_W	(1) : DMA write
											FJ_MEDIA_NF_TIMEOUT_DMA_R	(2) : DMA read
											FJ_MEDIA_NF_TIMEOUT_ERASE	(3) : erase
											FJ_MEDIA_NF_TIMEOUT_COPYBACK(4) : copyback
											FJ_MEDIA_NF_TIMEOUT_XDMAC	(5) : XDMAC
											FJ_MEDIA_NF_TIMEOUT_NF_LP	(6) : NF LowPower
											FJ_MEDIA_NF_TIMEOUT_XDMAC_LP(7) : XDMAC LowPower
											FJ_MEDIA_NF_TIMEOUT_CMD_DESC(8) : CMD descriptor
 *				USHORT timeout : msec
 * @return 		None
 * @note 		None
 * @attention 	None
 */
VOID FJ_NF_Set_Timeout(FJ_MEDIA_NF_TIMEOUT type, USHORT timeout)
{
	fj_nf_set_timeout(type, timeout);

	return;
}

/**
 * @brief 		Get timeout.
 * @param		FJ_MEDIA_NF_TIMEOUT type :
											FJ_MEDIA_NF_TIMEOUT_RESET	(0) : reset
											FJ_MEDIA_NF_TIMEOUT_DMA_W	(1) : DMA write
											FJ_MEDIA_NF_TIMEOUT_DMA_R	(2) : DMA read
											FJ_MEDIA_NF_TIMEOUT_ERASE	(3) : erase
											FJ_MEDIA_NF_TIMEOUT_COPYBACK(4) : copyback
											FJ_MEDIA_NF_TIMEOUT_XDMAC	(5) : XDMAC
											FJ_MEDIA_NF_TIMEOUT_NF_LP	(6) : NF LowPower
											FJ_MEDIA_NF_TIMEOUT_XDMAC_LP(7) : XDMAC LowPower
											FJ_MEDIA_NF_TIMEOUT_CMD_DESC(8) : CMD descriptor
 *				USHORT* timeout : msec
 * @return 		None
 * @note 		None
 * @attention 	None
 */
VOID FJ_NF_Get_Timeout(FJ_MEDIA_NF_TIMEOUT type, USHORT* timeout)
{
	fj_nf_get_timeout(type, timeout);

	return;
}

/**
 * @brief 		Initialize the eMMC macro.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_EMMC_Init(VOID)
{
#ifndef CO_LINUX_VERSION_ON
#ifndef CO_TKERNEL_ONLY
	// RTOS must wait until linux writes the magic number that indicates it has finished its hibernation processing.
	while( 1 ) {
		if( *(volatile unsigned int*)( D_BOOT_MAGIC_NUM_ADDR ) == 0xBEEFCAFE ) {
			/* NF available */
			break;
		}
		/* wait 10ms */
	    OS_User_Dly_Tsk(10);
	}
#endif /* CO_TKERNEL_ONLY */
#endif /* CO_LINUX_VERSION_ON */
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_emmc_init();
}

/**
 * @brief 		Set limit of the eMMC access mode.
 * @param		E_MEDIA_EMMC_ACCESS_MODE mode
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_EMMC_Config(E_MEDIA_EMMC_ACCESS_MODE mode)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_emmc_config(mode);
}

/**
 * @brief 		Initialize the eMMC device.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_EMMC_Device_Init(VOID)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_emmc_device_init();
}

/**
 * @brief 		Gets the eMMC device information(access mode).
 * @param		INT32* mode
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_EMMC_Get_Device_Info(UINT32* mode)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_emmc_get_device_info(mode);
}

/**
 * @brief 		Write data to eMMC device.
 * @param		ULONG  start_sec
 *				ULONG  write_sec_num
 *				UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_EMMC_Write_Sector(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_emmc_write_sector(start_sec, write_sec_num, data_addr);
}

/**
 * @brief 		Read data from eMMC device.
 * @param		ULONG  start_sec
 *				ULONG  read_sec_num
 *				UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_EMMC_Read_Sector(ULONG start_sec, ULONG read_sec_num, UINT32* data_addr)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_emmc_read_sector(start_sec, read_sec_num, data_addr);
}

/**
 * @brief 		Partition of eMMC.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_EMMC_Partition(VOID)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_emmc_partition();
}

/**
 * @brief 		Read partition information from the storage area of eMMC.
 * @param		FJ_EMMC_PARTITION* part_info
 * @param		ULONG* part_num
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_EMMC_Partition_Read(FJ_EMMC_PARTITION* part_info, ULONG* part_num)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_emmc_partition_read(part_info, part_num);
}

/**
 * @brief 		Get the start sector number and size of the selected Partition of eMMC device.
 * @param		E_MEDIA_EMMC_PARTITION_NAME name
 * @param		ULONG* start_sec_num
 * @param		ULONG* data_size
 * @param		ULONG* total_size
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_EMMC_Get_Partition_Info(E_MEDIA_EMMC_PARTITION_NAME name, ULONG* start_sec_num, ULONG* data_size, ULONG* total_size)
{
	// romarea_last

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_emmc_get_partition_info(name, start_sec_num, data_size, total_size);
}

/**
 * @brief 		Get parameter block info.
 * @param		ULONG* param_blk_top
 *				ULONG* param_blk_size
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_EMMC_Get_ParamBlk_Info(ULONG* param_blk_top, ULONG* param_blk_size)
{
	return fj_emmc_get_paramblk_info(param_blk_top, param_blk_size);
}

/**
 * @brief 		Get exception log info.
 * @param		UINT32* exception_log_top
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_EMMC_Get_ExceptionLog_Info(UINT32* exception_log_top)
{
	return fj_emmc_get_exceptionlog_info(exception_log_top);
}

// --- REMOVE_USB BEGIN ---
/**
 * @brief 		Get Media(SD) WriteProtect Information.
 * @param		UCHAR*		pwWpInfo
 * @return 		FJ_ERR_CODE : FJ_ERR_OK Media Detect
 *                          : FJ_ERR_NG Media NonDetect
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_GetWriteProtectStatus(UCHAR* pwWpInfo)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_getwriteprotectstatus(pwWpInfo);
}

/**
 * @brief 		Get used media and format(filesystem) type.
 * @param		FJ_USED_MEDIA* media : used media type.
 *											FJ_USED_MEDIA_UNKNOWN(0x00) : unknown media
 *											FJ_USED_MEDIA_NF     (0x01) : Nand Flash
 *				FJ_FORMAT_TYPE* format_type : format(filesystem) type of the used media.
 *											FJ_FS_TYPE_UNKNOWN(0x00) : unknown filesystem
 *											FJ_FS_TYPE_FAT12  (0x01) : FAT12 format
 *											FJ_FS_TYPE_FAT16  (0x04) : FAT16 format (<= 32MB)
 *											FJ_FS_TYPE_FAT16E (0x06) : FAT16 format (>  32MB)
 *											FJ_FS_TYPE_EXFAT  (0x07) : exFAT format
 *											FJ_FS_TYPE_FAT32  (0x0B) : FAT32 format
 *											FJ_FS_TYPE_FAT32X (0x0C) : FAT32 (LBA) format
 *											FJ_FS_TYPE_FAT16X (0x0E) : FAT16 (LBA) format
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_Get_Media_Format_Type(UCHAR* media, UCHAR* format_type)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_get_media_format_type(media, format_type);
}
// --- REMOVE_USB END ---
