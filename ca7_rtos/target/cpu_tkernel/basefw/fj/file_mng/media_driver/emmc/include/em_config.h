// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
* @file em_config.h
* @brief environment parameter definition
* @note None
* @attention None
*
*/
/*****************************************************************************/

#ifndef EM_CONFIG_H_INCLUDE
#define EM_CONFIG_H_INCLUDE

/// Soft Reset retry count
#define EM_CONFIG_RESET_COUNT					  2
/// Status wait time(ms)
#define EM_CONFIG_WAIT_VALUE_TIME				  2000uL
/// CMD1 wait time(ms)
#define EM_CONFIG_CMD1_WAIT_VALUE_TIME			  2000uL
/// Rampup time(ms)
#define EM_CONFIG_RAMPUP_TIME_MS				  2000uL
/// Data transfer timeout(ms)
#define EM_CONFIG_DATA_TIMEOUT					  16000uL
/// eMMC Register base address
#define EM_CONFIG_REGISTER_BASE_ADDRESS			  ( unsigned long )&IO_EMMC
/// Slot num
#define EM_CONFIG_SLOT_COUNT					  1
/// Device per Slot
#define EM_CONFIG_MAX_DEV_PER_SLOT				  1
/// Default block size
#define EM_CONFIG_DEFAULT_BLOCK_SIZE			  512
/// Max training count(Division valuex2(Except for 400KHz))
#define EM_CONFIG_TRAINING_MAX_CNT				  16
/// Max tuning count
#define EM_CONFIG_TUNING_MAX_CNT				  40
/// step of SDCLK Delay Register
#define EM_CONFIG_PHY_SDCLK_DELAY_STEP			  1

/// ADMA descriptor sub buffer count
#define EM_CONFIG_SUB_BUFFERS_COUNT				  512
/// ADMA descriptor sub buffer size
#define EM_CONFIG_SUB_BUFFERS_SIZE				  65536uL
/// ADMA descriptor buffer size
#define EM_CONFIG_MAX_DESCR_BUFF_SIZE			  (8 * EM_CONFIG_SUB_BUFFERS_COUNT)
/// ADMA descriptor max transfer size
#define EM_CONFIG_MAX_TRANSFER_SIZE				  (EM_CONFIG_SUB_BUFFERS_SIZE * EM_CONFIG_SUB_BUFFERS_COUNT)
/// ADMA descriptor max sector count
#define EM_CONFIG_MAX_SECTOR_COUNT				  (EM_CONFIG_MAX_TRANSFER_SIZE / EM_CONFIG_DEFAULT_BLOCK_SIZE - 1)

#endif
