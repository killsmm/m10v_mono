/**
 * @file		emergency_log.c
 * @brief		Emergency Log dump
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include <string.h>
#include "fj_std.h"
#include "fj_emergency_log.h"
#include "fs_if.h"
#include "mwnf.h"
#include "mwnf_core.h"
#include "fj_media.h"
#include "em_if_ext.h"
#include "em_config.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static unsigned long g_bf_emrgency_log_top_sector  = 0;
static unsigned long g_bf_emrgency_log_save_sector = 0;
static unsigned long g_bf_emrgency_log_save_size  = 0;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static FJ_ERR_CODE emergencylog_store_memory_nf(ULONG startAddr, INT32 headerInfo);
static FJ_ERR_CODE emergencylog_save_nf(UCHAR* filename);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
static FJ_ERR_CODE emergencylog_store_memory_emmc(ULONG startAddr, INT32 headerInfo);
static FJ_ERR_CODE emergencylog_save_emmc(UCHAR* filename);
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---


/**
Stores the specified memory data to NandFlash.
@param[in] startAddr log data address
@param[in] headerInfo log header information<br>
						31-16bit : Log label ID<br>
						15-0 bit : Log data size<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
static FJ_ERR_CODE emergencylog_store_memory_nf(ULONG startAddr, INT32 headerInfo)
{
	int ercd;
	UINT32 size;
	UCHAR  top_sec_data[12] = {'M', 'I', 'L', 'B', 'E', 'A', 'U', 'T', 0, 0, 0, 0};
	UINT32 write_data[D_MWNF_SECTOR_SIZE/4];

	if(g_bf_emrgency_log_top_sector == 0){
		return FJ_ERR_NG;
	}

	if (g_bf_emrgency_log_save_size == 0) {
		g_bf_emrgency_log_save_sector = g_bf_emrgency_log_top_sector + 1;
	}

	size = (UINT32)(headerInfo & 0xFFFFFF);

	// header information
	memset(write_data, 0xFF, sizeof(write_data));
	write_data[0] = headerInfo;

	// write header information
	ercd = MWNF_Direct_Write(g_bf_emrgency_log_save_sector, 4, (unsigned long)write_data);
	if (ercd != 0) {
		// header information write error
		return FJ_ERR_NG;
	}
	g_bf_emrgency_log_save_sector++;
	g_bf_emrgency_log_save_size++;

	// write data
	ercd = MWNF_Direct_Write(g_bf_emrgency_log_save_sector, size, startAddr);
	if (ercd != 0) {
		// Log data write error
		return FJ_ERR_NG;
	}

	// To calculate write sector and write size
	g_bf_emrgency_log_save_sector += (size / D_MWNF_SECTOR_SIZE);
	g_bf_emrgency_log_save_size  += (size / D_MWNF_SECTOR_SIZE);
	if ((size % D_MWNF_SECTOR_SIZE) != 0) {
		g_bf_emrgency_log_save_sector++;
		g_bf_emrgency_log_save_size++;
	}

	// write keyword and total size
	memcpy(&top_sec_data[8], &g_bf_emrgency_log_save_size, 4);
	memcpy(&write_data, &top_sec_data, sizeof(top_sec_data));

	ercd = MWNF_Direct_Write(g_bf_emrgency_log_top_sector, 12, (unsigned long)top_sec_data);
	if (ercd != 0) {
		// Top sector write error
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
Stores the specified memory data to eMMC.
@param[in] startAddr log data address
@param[in] headerInfo log header information<br>
						31-16bit : Log label ID<br>
						15-0 bit : Log data size<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
static FJ_ERR_CODE emergencylog_store_memory_emmc(ULONG startAddr, INT32 headerInfo)
{
	int ercd;
	UINT32 size, sector;
	UCHAR  top_sec_data[12] = {'M', 'I', 'L', 'B', 'E', 'A', 'U', 'T', 0, 0, 0, 0};
	UINT32 write_data[EM_CONFIG_DEFAULT_BLOCK_SIZE/4];

	if(g_bf_emrgency_log_top_sector == 0){
		return FJ_ERR_NG;
	}

	if (g_bf_emrgency_log_save_size == 0) {
		g_bf_emrgency_log_save_sector = g_bf_emrgency_log_top_sector + 1;
	}

	// disable interrupt
	MWEM_Set_Interrupt_Enable(0);

	// header information
	memset(write_data, 0xFF, sizeof(write_data));
	write_data[0] = headerInfo;

	// write header information
	ercd = MWEM_DirectWrite(g_bf_emrgency_log_save_sector, 1, (char*)write_data);
	if (ercd != 0) {
		// header information write error
		MWEM_Set_Interrupt_Enable(1);
		return FJ_ERR_NG;
	}
	g_bf_emrgency_log_save_sector++;
	g_bf_emrgency_log_save_size++;

	// write data
	size = (UINT32)(headerInfo & 0xFFFFFF);
	if (size % EM_CONFIG_DEFAULT_BLOCK_SIZE) {
		sector = (size / EM_CONFIG_DEFAULT_BLOCK_SIZE + 1);
	}
	else {
		sector = (size / EM_CONFIG_DEFAULT_BLOCK_SIZE);
	}

	ercd = MWEM_DirectWrite(g_bf_emrgency_log_save_sector, sector, (char*)startAddr);
	if (ercd != 0) {
		// Log data write error
		MWEM_Set_Interrupt_Enable(1);
		return FJ_ERR_NG;
	}

	// To calculate write sector and write size
	g_bf_emrgency_log_save_sector += sector;
	g_bf_emrgency_log_save_size  += sector;

	// write keyword and total size
	memcpy(&top_sec_data[8], &g_bf_emrgency_log_save_size, 4);
	memcpy(&write_data, &top_sec_data, sizeof(top_sec_data));

	ercd = MWEM_DirectWrite(g_bf_emrgency_log_top_sector, 1, (char*)write_data);
	if (ercd != 0) {
		// Top sector write error
		MWEM_Set_Interrupt_Enable(1);
		return FJ_ERR_NG;
	}

	// enable interrupt
	MWEM_Set_Interrupt_Enable(1);

	return FJ_ERR_OK;
}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/**
Saves the log data stored at NandFlash to specified file on the media(SD).
@param[in] filename file name
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
static FJ_ERR_CODE emergencylog_save_nf(UCHAR* filename)
{
	int ercd, file_id;
	UCHAR tmp_buff[512];
	ULONG data_size;
	ULONG read_sec;
	FW_ULONG length;

	FJ_NF_Device_Init();
	read_sec = g_bf_emrgency_log_top_sector;
	// Information Sector read
	ercd = MWNF_BRead(NULL, read_sec, 1, (char*)tmp_buff);
	if (ercd != 0) {
		// read error
		return FJ_ERR_NG;
	}

	if (memcmp(tmp_buff, "MILBEAUT", 8) != 0) {
		// Signiture error
		return FJ_ERR_NG;
	}

	memcpy(&data_size, &tmp_buff[8], 4);
	read_sec = g_bf_emrgency_log_top_sector + 1;

	// log file create
	file_id = BF_Fs_If_Open((const FW_CHAR*)filename,(D_BF_FS_IF_O_RDWR | D_BF_FS_IF_O_CREAT | D_BF_FS_IF_O_TRUNC), D_BF_FS_IF_ATTR_NORMAL);
	if (file_id == 0) {
		// open error
		return FJ_ERR_NG;
	}

	// log data write to file
	while (data_size > 0) {
		ercd = MWNF_BRead(NULL, read_sec, 1, (char*)tmp_buff);
		if (ercd != 0) {
			BF_Fs_If_Close(file_id);
			return FJ_ERR_NG;
		}

		ercd = BF_Fs_If_Write(file_id, tmp_buff, 512, &length);
		if (ercd != 0) {
			BF_Fs_If_Close(file_id);
			return FJ_ERR_NG;
		}
		data_size--;
		read_sec++;
	}

	// file close
	ercd = BF_Fs_If_Close(file_id);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
Saves the log data stored at eMMC to specified file on the media(SD).
@param[in] filename file name
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
static FJ_ERR_CODE emergencylog_save_emmc(UCHAR* filename)
{
	int ercd, file_id;
	UCHAR tmp_buff[512];
	ULONG data_size;
	ULONG read_sec;
	FW_ULONG length;

	read_sec = g_bf_emrgency_log_top_sector;
	// Information Sector read
	ercd = MWEM_DirectRead(read_sec, 1, (char*)tmp_buff);
	if (ercd != 0) {
		// read error
		return FJ_ERR_NG;
	}

	if (memcmp(tmp_buff, "MILBEAUT", 8) != 0) {
		// Signiture error
		return FJ_ERR_NG;
	}

	memcpy(&data_size, &tmp_buff[8], 4);
	read_sec = g_bf_emrgency_log_top_sector + 1;

	// log file create
	file_id = BF_Fs_If_Open((const FW_CHAR*)filename,(D_BF_FS_IF_O_RDWR | D_BF_FS_IF_O_CREAT | D_BF_FS_IF_O_TRUNC), D_BF_FS_IF_ATTR_NORMAL);
	if (file_id == 0) {
		// open error
		return FJ_ERR_NG;
	}

	// log data write to file
	while (data_size > 0) {
		ercd = MWEM_DirectRead(read_sec, 1, (char*)tmp_buff);
		if (ercd != 0) {
			BF_Fs_If_Close(file_id);
			return FJ_ERR_NG;
		}

		ercd = BF_Fs_If_Write(file_id, tmp_buff, 512, &length);
		if (ercd != 0) {
			BF_Fs_If_Close(file_id);
			return FJ_ERR_NG;
		}
		data_size--;
		read_sec++;
	}

	// file close
	ercd = BF_Fs_If_Close(file_id);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
Set the number of log write start sector to external storage.
@param[in] sector start sector number
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE fj_emergencylog_set_log_start_sector(ULONG sector)
{
	g_bf_emrgency_log_top_sector = sector;

	return FJ_ERR_OK;
}


/**
Stores the specified memory data to external storage.
@param[in] startAddr log data address
@param[in] headerInfo log header information<br>
						31-16bit : Log label ID<br>
						15-0 bit : Log data size<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE fj_emergencylog_store_memory(ULONG startAddr, INT32 headerInfo)
{
	FJ_ERR_CODE ercd;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	CHAR	mdr = (*(( unsigned long * )0x1DFF4000) & 0x7);

	if (mdr == 1) {
		// eMMC
		ercd = emergencylog_store_memory_emmc(startAddr, headerInfo);
	}
	else {
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		// NAND Flash
		ercd = emergencylog_store_memory_nf(startAddr, headerInfo);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	return ercd;
}

/**
Saves the log data stored at external storage to specified file on the media(SD).
@param[in] filename file name
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE fj_emergencylog_save(UCHAR* filename)
{
	FJ_ERR_CODE ercd;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	CHAR	mdr = (*(( unsigned long * )0x1DFF4000) & 0x7);

	if (mdr == 1) {
		// eMMC
		ercd = emergencylog_save_emmc(filename);
	}
	else {
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		// NAND Flash
		ercd = emergencylog_save_nf(filename);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	return ercd;
}
/* File End */
