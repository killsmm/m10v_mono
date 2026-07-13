/**
 * @file		media.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _MEDIA_H_
#define _MEDIA_H_

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_media.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define NF_MAX_DRV_NUM		7
#define EM_MAX_DRV_NUM		7

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
extern FJ_ERR_CODE fj_set_mediadetect(UCHAR ch, UCHAR time_per_sampling, UCHAR num_of_sampling);
extern FJ_ERR_CODE fj_get_mediadetectbypoll(UCHAR ch, UCHAR* detect, UCHAR time_per_sampling, UCHAR num_of_sampling);
extern FJ_ERR_CODE fj_get_mediadetect(UCHAR ch, UCHAR* detect);
extern FJ_ERR_CODE fj_getwriteprotectstatus(UCHAR* pwWpInfo);

extern FJ_ERR_CODE fj_sd_init(UCHAR ch);
extern FJ_ERR_CODE fj_sd_config(UCHAR ch, FJ_MEDIA_SD_DRIVE drive, FJ_MEDIA_SD_MODE mode, FJ_MEDIA_SD_CLOCK clock);
extern FJ_ERR_CODE fj_sd_set_event_cb(UCHAR ch, FJ_MEDIA_SD_CB_EVENT type, SD_CALLBACK pcallback);
extern FJ_ERR_CODE fj_sd_card_init(UCHAR ch);
extern FJ_ERR_CODE fj_sd_getcardinfo(UCHAR ch, INT32* mode, ULONG* clock, UCHAR* wp_status);
extern FJ_ERR_CODE fj_sd_set_transmode_select(UCHAR ch, FJ_MEDIA_SD_MODE mode, FJ_MEDIA_SD_CLOCK max_clock);
extern FJ_ERR_CODE fj_sd_get_transmode_select(UCHAR ch, INT32* mode, ULONG* sd_clk);
extern FJ_ERR_CODE fj_sd_set_hold_clock(UCHAR ch, FJ_MEDIA_SD_HOLD_CLOCK hold);
extern FJ_ERR_CODE fj_sd_set_cardlockpassword(UCHAR ch, CHAR* password, ULONG size);
extern FJ_ERR_CODE fj_sd_set_cardlock(UCHAR ch, FJ_MEDIA_SD_LOCK_OPE operation);
extern FJ_ERR_CODE fj_sd_set_card_power(UCHAR ch, UCHAR enable);
extern FJ_ERR_CODE fj_sd_set_uhs2_modeselect(UCHAR ch, FJ_MEDIA_SD_UHS2_MODE mode, FJ_MEDIA_SD_UHS2_RANGE range, FJ_MEDIA_SD_UHS2_PWR power);
extern FJ_ERR_CODE fj_sd_get_uhs2_modeselect(UCHAR ch, UINT32* mode, UINT32* range, UINT32* power);
extern FJ_ERR_CODE fj_sd_set_uhs2_powersaving(UCHAR ch, FJ_MEDIA_SD_UHS2_PMODE mode, UCHAR enable);
extern FJ_ERR_CODE fj_sd_set_uhs2_detecttime(UCHAR ch, UINT32 time);

extern FJ_ERR_CODE fj_nf_init(VOID);
extern FJ_ERR_CODE fj_nf_config(FJ_NF_ATTRIBUTE* attr);
extern FJ_ERR_CODE fj_nf_device_init(VOID);
extern FJ_ERR_CODE fj_nf_get_mid_did(UINT32* mid, UINT32* did);
extern FJ_ERR_CODE fj_nf_get_device_info(FJ_NF_DEV_INFO* nf_dev_info);
extern FJ_ERR_CODE fj_nf_erase(VOID);
extern FJ_ERR_CODE fj_nf_write_physical_data(ULONG phy_blk, UINT32* data_addr);
extern FJ_ERR_CODE fj_nf_read_physical_data(ULONG phy_blk, UINT32* data_addr);
extern FJ_ERR_CODE fj_nf_write_logical_sector(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr);
extern FJ_ERR_CODE fj_nf_read_logical_sector(ULONG start_sec, ULONG read_sec_num, UINT32* data_addr);
extern FJ_ERR_CODE fj_nf_write_logical_sector_pio(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr);
extern FJ_ERR_CODE fj_nf_partition(VOID);
extern FJ_ERR_CODE fj_nf_partition_read(FJ_NF_PARTITION* part_info, ULONG* part_num);
extern FJ_ERR_CODE fj_nf_get_partition_info(E_MEDIA_NF_PARTITION_NAME name, ULONG* start_sec_num, ULONG* data_size, ULONG* total_size);
extern FJ_ERR_CODE fj_nf_get_badblock_num(ULONG limit, UCHAR mode, ULONG* num, UINT32* buf);
extern FJ_ERR_CODE fj_nf_get_paramblk_info(ULONG* param_blk_top, ULONG* param_blk_size);
extern FJ_ERR_CODE fj_nf_get_exceptionlog_info(UINT32* exception_log_top);
extern VOID fj_nf_set_retry_cnt(UCHAR retry_cnt_read, UCHAR retry_cnt_write);
extern VOID fj_nf_get_retry_cnt(UCHAR* retry_cnt_read, UCHAR* retry_cnt_write);
extern VOID fj_nf_set_timeout(FJ_MEDIA_NF_TIMEOUT type, USHORT timeout);
extern VOID fj_nf_get_timeout(FJ_MEDIA_NF_TIMEOUT type, USHORT* timeout);

extern FJ_ERR_CODE fj_emmc_init(VOID);
extern FJ_ERR_CODE fj_emmc_config(E_MEDIA_EMMC_ACCESS_MODE mode);
extern FJ_ERR_CODE fj_emmc_device_init(VOID);
extern FJ_ERR_CODE fj_emmc_get_device_info(UINT32* mode);
extern FJ_ERR_CODE fj_emmc_write_sector(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr);
extern FJ_ERR_CODE fj_emmc_read_sector(ULONG start_sec, ULONG read_sec_num, UINT32* data_addr);
extern FJ_ERR_CODE fj_emmc_partition(VOID);
extern FJ_ERR_CODE fj_emmc_partition_read(FJ_EMMC_PARTITION* part_info, ULONG* part_num);
extern FJ_ERR_CODE fj_emmc_get_partition_info(E_MEDIA_EMMC_PARTITION_NAME name, ULONG* start_sec_num, ULONG* data_size, ULONG* total_size);
extern FJ_ERR_CODE fj_emmc_get_paramblk_info(ULONG* param_blk_top, ULONG* param_blk_size);
extern FJ_ERR_CODE fj_emmc_get_exceptionlog_info(UINT32* exception_log_top);

#endif	// _MEDIA_H_
