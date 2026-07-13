/**
 * @file		fj_media.h
 * @brief		Media Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_MEDIA_H
#define _FJ_MEDIA_H

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_custom.h"
///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_media_overview
@{
<span style="font-weight: bold;">Introduction</span><br>
&nbsp; <br>
	The Media and Memory controls can be achieved by using this API.<br>
	The Supported storage media is NandFlash, SD card and eMMC.<br>
<br>
<span style="font-weight: bold;">NandFlash interface</span><br>
<br>
	Build-in 1ch NandFlash interface.<br>
<br>
<span style="font-weight: bold;">SD card interface</span><br>
<br>
	Build-in 2ch SD card interface.<br>
	SD ch0 supports the Default speed / High speed / UHS-I.<br>
	SD ch0 is controlled by SD4 of SD macro.<br>
	SD ch1 supports the Default speed / High speed / UHS-I / UHS-II.<br>
	SD ch1 is controlled by SD3 of SD macro.<br>
<br>
<span style="font-weight: bold;">eMMC interface</span><br>
<br>
	Build-in 1ch eMMC interface.<br>
<br>
@}*//* --- end of fj_media_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_media_sequence
@{
	- @ref media_sequence_sd_init
	- @ref media_sequence_sd_insert
	- @ref media_sequence_sd_eject
	- @ref media_sequence_nf_init
	- @ref media_sequence_nf_physical_access
	- @ref media_sequence_nf_logical_access
	- @ref media_sequence_nf_partition
	- @ref media_sequence_nf_partition_read
	- @ref media_sequence_emmc_init
	- @ref media_sequence_emmc_access
	- @ref media_sequence_emmc_partition
	- @ref media_sequence_emmc_partition_read

	<hr>
	@section media_sequence_sd_init SD Card Initialize
	This is SD card initialize sequence.<br>
	@image html media_sequence_sd_init.png
	@code
		int  ercd;
		unsigned char  wp_status;
		unsigned int   mode;
		unsigned long  clock;

		ercd = FJ_SD_Init(1);
		if (ercd != FJ_ERR_OK) {
			// error
		}
		ercd = FJ_SD_Config(1, FJ_MEDIA_SD_TYPE_B, FJ_MEDIA_SD_MODE_SDR50, FJ_MEDIA_SD_CLOCK_100M);
		if (ercd != FJ_ERR_OK) {
			// error
		}
		ercd = FJ_SD_Set_UHS2_ModeSelect(1, FJ_MEDIA_SD_UHS2_MODE_FD, FJ_MEDIA_SD_UHS2_RANGE_B, FJ_MEDIA_SD_UHS2_PWR_1_44W);
		if (ercd != FJ_ERR_OK) {
			// error
		}
		ercd = FJ_Get_MediaDetect(1, &detect);
		if (ercd != FJ_ERR_OK) {
			// error
		}
		ercd = FJ_SD_Set_Event_CB(1, FJ_MEDIA_SD_CB_INSERTED, sd_insert_callback);
		if (ercd != FJ_ERR_OK) {
			// error
		}
		ercd = FJ_SD_Set_Event_CB(1, FJ_MEDIA_SD_CB_EJECTED, sd_eject_callback);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		if (detect == 1) {
			ercd = FJ_SD_Card_Init(1);
			if (ercd != FJ_ERR_OK) {
				// error
			}
			ercd = FJ_SD_GetCardInfo(1, &mode, &clock, &wp_status);
			if (ercd != FJ_ERR_OK) {
				// error
			}
			if (mode == FJ_MEDIA_SD_MODE_UHS2) {
				ercd = FJ_SD_Get_UHS2_ModeSelect(1, &u2mode, &range, &power);
				if (ercd != FJ_ERR_OK) {
					// error
				}
			}
			ercd = FJ_Mount(FJ_MEDIA_SD1, "I");
			if (ercd != FJ_ERR_OK) {
				// error
			}
		}
	@endcode
	

	<hr>
	@section media_sequence_sd_insert SD Card Detect (Insert)
	This is SD card detect(insert) sequence.<br>
	@image html media_sequence_sd_insert.png

	<hr>
	@section media_sequence_sd_eject SD Card Detect (Eject)
	This is SD card detect(eject) sequence.<br>
	@image html media_sequence_sd_eject.png

	<hr>
	@section media_sequence_nf_init NandFlash Initialize
	This is NandFlash initialize sequence.<br>
	@image html media_sequence_nf_init.png
	@code
		int  ercd;
		unsigned int   mid, did;
		FJ_NF_ATTRIBUTE attr;

		ercd = FJ_NF_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Get_MID_DID(&mid, &did);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		attr.ce_num                = 1;
		attr.max_bad_block_num     = 48
		attr.device_block_num      = 2048;
		attr.bad_block_search_page = 63;
		attr.drivability0 = E_MEDIA_NF_DRIVABILITY_1;
		attr.drivability1 = E_MEDIA_NF_DRIVABILITY_1;
		attr.drivability2 = E_MEDIA_NF_DRIVABILITY_1;

		ercd = FJ_NF_Config(&attr);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Device_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_Mount(FJ_MEDIA_NF_STORAGE0, "J");
		if (ercd != FJ_ERR_OK) {
			// error
		}
	@endcode

	<hr>
	@section media_sequence_nf_physical_access NandFlash Physical Access
	This is NandFlash physical access sequence.<br>
	@image html media_sequence_nf_physical_access.png
	@code
		int  ercd;
		unsigned int   mid, did;
		FJ_NF_ATTRIBUTE attr;

		ercd = FJ_NF_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Get_MID_DID(&mid, &did);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		attr.ce_num                = 1;
		attr.max_bad_block_num     = 48
		attr.device_block_num      = 2048;
		attr.bad_block_search_page = 63;
		attr.drivability0 = E_MEDIA_NF_DRIVABILITY_1;
		attr.drivability1 = E_MEDIA_NF_DRIVABILITY_1;
		attr.drivability2 = E_MEDIA_NF_DRIVABILITY_1;

		ercd = FJ_NF_Config(&attr);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Device_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Read_Physical_Data(0x1000, 0x70000000);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Write_Physical_Data(0x1000, 0x71000000);
		if (ercd != FJ_ERR_OK) {
			// error
		}
	@endcode

	<hr>
	@section media_sequence_nf_logical_access NandFlash Logical Access
	This is NandFlash logical access sequence.<br>
	@image html media_sequence_nf_logical_access.png
	@code
		int  ercd;
		unsigned int   mid, did;
		FJ_NF_ATTRIBUTE attr;

		ercd = FJ_NF_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Get_MID_DID(&mid, &did);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		attr.ce_num                = 1;
		attr.max_bad_block_num     = 48
		attr.device_block_num      = 2048;
		attr.bad_block_search_page = 63;
		attr.drivability0 = E_MEDIA_NF_DRIVABILITY_1;
		attr.drivability1 = E_MEDIA_NF_DRIVABILITY_1;
		attr.drivability2 = E_MEDIA_NF_DRIVABILITY_1;

		ercd = FJ_NF_Config(&attr);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Device_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Read_Logical_Sector(2000, 1, &buf);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Write_Logical_Sector(2000, 1, &buf);
		if (ercd != FJ_ERR_OK) {
			// error
		}
	@endcode

	<hr>
	@section media_sequence_nf_partition NandFlash Partition (Storage area)
	This is sequence of the partition of NandFlash(Storage area).<br>
	@image html media_sequence_nf_partition.png
	@code
		int  ercd;
		unsigned int   mid, did;
		FJ_NF_ATTRIBUTE attr;
		FJ_NF_PARTITION part_info[8]={0};
		unsigned long part_num;

		ercd = FJ_NF_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Get_MID_DID(&mid, &did);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		attr.ce_num                = 1;
		attr.max_bad_block_num     = 48
		attr.device_block_num      = 2048;
		attr.bad_block_search_page = 63;
		attr.drivability0 = E_MEDIA_NF_DRIVABILITY_1;
		attr.drivability1 = E_MEDIA_NF_DRIVABILITY_1;
		attr.drivability2 = E_MEDIA_NF_DRIVABILITY_1;

		ercd = FJ_NF_Config(&attr);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Device_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Partition_Read(part_info, &part_num);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		if (part_num == 0) {
			ercd = FJ_NF_Partition();
			if (ercd != FJ_ERR_OK) {
				// error
			}
		}
	@endcode

	<hr>
	@section media_sequence_nf_partition_read NandFlash Partition Read (Partition data area)
	This is a sequence to read the partition data of NandFlash(Partition data area).<br>
	@image html media_sequence_nf_partition_read.png
	@code
		int  ercd;
		unsigned int   mid, did;
		unsigned long  start_sec_num, data_size, total_size, phy_blk;
		unsigned char  buf[];
		FJ_NF_ATTRIBUTE attr;
		FJ_NF_DEV_INFO dev_info;

		ercd = FJ_NF_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Get_MID_DID(&mid, &did);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		attr.ce_num                = 1;
		attr.max_bad_block_num     = 48
		attr.device_block_num      = 2048;
		attr.bad_block_search_page = 63;
		attr.drivability0 = E_MEDIA_NF_DRIVABILITY_1;
		attr.drivability1 = E_MEDIA_NF_DRIVABILITY_1;
		attr.drivability2 = E_MEDIA_NF_DRIVABILITY_1;

		ercd = FJ_NF_Config(&attr);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Device_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Get_Device_Info(&dev_info);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_NF_Get_Partition_Info(E_MEDIA_NF_PARTITION_DATA2, &start_sec_num, &data_size, &total_size);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		phy_blk = (start_sec_num / dev_info.sector_per_page / dev_info.page_per_blk);

		while(phy_blk < 10) {
			ercd = FJ_NF_Read_Physical_Data(phy_blk, &buf);
			if ((ercd != FJ_ERR_OK) && (ercd != FJ_ERR_READ_NF_BAD_BLOCK)) {
				// error end
				break;
			}
			phy_blk++;
			buf += (dev_info.page_size * dev_info.page_per_blk)
		}
	@endcode

	<hr>
	@section media_sequence_emmc_init eMMC Initialize
	This is eMMC initialize sequence.<br>
	@image html media_sequence_emmc_init.png
	@code
		int  ercd;

		ercd = FJ_EMMC_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Config(E_MEDIA_EMMC_ACCESS_MODE_HS_200);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Device_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_Mount(FJ_MEDIA_EMMC, "E");
		if (ercd != FJ_ERR_OK) {
			// error
		}
	@endcode

	<hr>
	@section media_sequence_emmc_access eMMC Access
	This is eMMC access sequence.<br>
	@image html media_sequence_emmc_access.png
	@code
		int  ercd;

		ercd = FJ_EMMC_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Config(E_MEDIA_EMMC_ACCESS_MODE_HS_200);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Device_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_Mount(FJ_MEDIA_EMMC, "E");
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Read_Sector(2000, 1, &buf);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Write_Sector(2000, 1, &buf);
		if (ercd != FJ_ERR_OK) {
			// error
		}
	@endcode

	<hr>
	@section media_sequence_emmc_partition eMMC Partition (Storage area)
	This is sequence of the partition of eMMC(Storage area).<br>
	@image html media_sequence_emmc_partition.png
	@code
		int  ercd;
		FJ_EMMC_PARTITION part_info[8]={0};
		unsigned long part_num;

		ercd = FJ_EMMC_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Config(E_MEDIA_EMMC_ACCESS_MODE_HS_200);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Device_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_Mount(FJ_MEDIA_EMMC, "E");
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Partition_Read(part_info, &part_num);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		if (part_num == 0) {
			ercd = FJ_EMMC_Partition();
			if (ercd != FJ_ERR_OK) {
				// error
			}
		}
	@endcode

	<hr>
	@section media_sequence_emmc_partition_read eMMC Partition Read (Partition data area)
	This is a sequence to read the partition data of eMMC(Partition data area).<br>
	@image html media_sequence_emmc_partition_read.png
	@code
		int  ercd;
		unsigned long  start_sec_num, data_size, total_size;
		unsigned char  buf[];

		ercd = FJ_EMMC_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Config(E_MEDIA_EMMC_ACCESS_MODE_HS_200);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Device_Init();
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_Mount(FJ_MEDIA_EMMC, "E");
		if (ercd != FJ_ERR_OK) {
			// error
		}

		ercd = FJ_EMMC_Get_Partition_Info(E_MEDIA_EMMC_PARTITION_DATA2, &start_sec_num, &data_size, &total_size);
		if (ercd != FJ_ERR_OK) {
			// error
		}

		// Calculate the read sector number from the data size.
		read_sec_num = ((data_size - 1) / 512) + 1;

		ercd = FJ_EMMC_Read_Sector(start_sec_num, read_sec_num, &buf);
		if (ercd != FJ_ERR_OK) {
			// error
		}
	@endcode
@}*//* --- end of fj_media_sequence */

/*----------------------------------------------------------------------*/
/* Definition														    */
/*----------------------------------------------------------------------*/
/** @weakgroup fj_media_definition
@{*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration														    */
/*----------------------------------------------------------------------*/
// Write Protect Status
/**
Write Protect Status
*/
typedef enum {
	FJ_MEDIA_WP					= 0,		/**< Write Protect ON */
	FJ_MEDIA_NON_WP				= 1			/**< Write Protect OFF */
} FJ_MEDIA_WRITE_PROTECT;

/**
SD Driver Type
*/
typedef enum {
	FJ_MEDIA_SD_TYPE_B			= 0,		/**< Driver Type B */
	FJ_MEDIA_SD_TYPE_A			= 1,		/**< Driver Type A */
	FJ_MEDIA_SD_TYPE_C			= 2,		/**< Driver Type C */
	FJ_MEDIA_SD_TYPE_D			= 3			/**< Driver Type D */
} FJ_MEDIA_SD_DRIVE;

/**
SD Clock
*/
typedef enum {
	FJ_MEDIA_SD_CLOCK_25M		= 0,		/**< 25MHz */
	FJ_MEDIA_SD_CLOCK_50M		= 1,		/**< 50MHz */
	FJ_MEDIA_SD_CLOCK_100M		= 2,		/**< 100MHz */
	FJ_MEDIA_SD_CLOCK_208M		= 3,		/**< (ch0)175MHz (ch1)200MHz */
	FJ_MEDIA_SD_CLOCK_UHS2		= 4			/**< 51.851MHz */
} FJ_MEDIA_SD_CLOCK;

/**
SD Speed mode
*/
typedef enum {
	FJ_MEDIA_SD_MODE_DS			= 0,		/**< Access Mode : Default Speed  */
	FJ_MEDIA_SD_MODE_HS			= 1,		/**< Access Mode : High Speed     */
	FJ_MEDIA_SD_MODE_SDR12		= 2,		/**< Access Mode : UHS-I(SDR12)   */
	FJ_MEDIA_SD_MODE_SDR25		= 3,		/**< Access Mode : UHS-I(SDR25)   */
	FJ_MEDIA_SD_MODE_SDR50		= 4,		/**< Access Mode : UHS-I(SDR50)   */
	FJ_MEDIA_SD_MODE_SDR104		= 5,		/**< Access Mode : UHS-I(SDR104)  */
	FJ_MEDIA_SD_MODE_UHS2		= 6			/**< Access Mode : UHS-II         */
} FJ_MEDIA_SD_MODE;

/**
SD UHS2 mode
*/
typedef enum {
	FJ_MEDIA_SD_UHS2_MODE_FD	= 0,		/**< UHS-II Mode : Full Duplex(FD) mode    */
	FJ_MEDIA_SD_UHS2_MODE_2LHD	= 1			/**< UHS-II Mode : 2Line-Half Duplex(2L-HD) mode (not supported) */
} FJ_MEDIA_SD_UHS2_MODE;

/**
SD UHS2 Range
*/
typedef enum {
	FJ_MEDIA_SD_UHS2_RANGE_A	= 0,		/**< UHS-II Speed range : A(x15) */
	FJ_MEDIA_SD_UHS2_RANGE_B	= 1			/**< UHS-II Speed range : B(x30) */
} FJ_MEDIA_SD_UHS2_RANGE;

/**
SD UHS2 Power Limit
*/
typedef enum {
	FJ_MEDIA_SD_UHS2_PWR_0_72W	= 0,		/**< UHS-II Power Limit : Max 0.72W */
	FJ_MEDIA_SD_UHS2_PWR_1_44W	= 1,		/**< UHS-II Power Limit : Max 1.44W */
	FJ_MEDIA_SD_UHS2_PWR_2_16W	= 2,		/**< UHS-II Power Limit : Max 2.16W */
	FJ_MEDIA_SD_UHS2_PWR_2_88W	= 3,		/**< UHS-II Power Limit : Max 2.88W */
	FJ_MEDIA_SD_UHS2_PWR_1_80W	= 4			/**< UHS-II Power Limit : Max 1.80W */
} FJ_MEDIA_SD_UHS2_PWR;

/**
SD UHS2 mode
*/
typedef enum {
	FJ_MEDIA_SD_UHS2_PMODE_DORMANT	 = 0,	/**< UHS-II Power saving : Dormant state mode   */
	FJ_MEDIA_SD_UHS2_PMODE_HIBERNATE = 1	/**< UHS-II Power saving : Hibernate state mode */
} FJ_MEDIA_SD_UHS2_PMODE;

/**
SD Hold clock
*/
typedef enum {
	FJ_MEDIA_SD_HOLD_CLOCK_1_4	= 0x0,		/**< SD_BCLK_I * 1/4 */
	FJ_MEDIA_SD_HOLD_CLOCK_2_4	= 0xC,		/**< SD_BCLK_I * 2/4 */
	FJ_MEDIA_SD_HOLD_CLOCK_3_4	= 0xF		/**< SD_BCLK_I * 3/4 */
} FJ_MEDIA_SD_HOLD_CLOCK;

/**
SD Lock/Unlock operation
*/
typedef enum {
	FJ_MEDIA_SD_UNLOCK			= 0x00,		/**< Unlock  */
	FJ_MEDIA_SD_SET_PASSWORD	= 0x01,		/**< Set Password  */
	FJ_MEDIA_SD_CLR_PASSWORD	= 0x02,		/**< Clear Password  */
	FJ_MEDIA_SD_LOCK			= 0x04,		/**< Lock  */
	FJ_MEDIA_SD_ERACE			= 0x08		/**< Erace  */
} FJ_MEDIA_SD_LOCK_OPE;

/**
SD Callback event type
*/
typedef enum {
	FJ_MEDIA_SD_CB_INSERTED		= 0,		/**< Callback event type : Inserted  */
	FJ_MEDIA_SD_CB_EJECTED		= 1			/**< Callback event type : Ejected  */
} FJ_MEDIA_SD_CB_EVENT;

/**
NandFlash Timeout type
*/
typedef enum {
	FJ_MEDIA_NF_TIMEOUT_RESET		= 0,	/**< reset (Default:5000ms) */
	FJ_MEDIA_NF_TIMEOUT_DMA_W		= 1,	/**< DMA write (Default:5000ms) */
	FJ_MEDIA_NF_TIMEOUT_DMA_R		= 2,	/**< DMA read  (Default:5000ms) */
	FJ_MEDIA_NF_TIMEOUT_ERASE		= 3,	/**< erase (Default:5000ms) */
	FJ_MEDIA_NF_TIMEOUT_COPYBACK	= 4,	/**< copyback (Default:5000ms) */
	FJ_MEDIA_NF_TIMEOUT_XDMAC		= 5,	/**< XDMAC (Default:5000ms) */
	FJ_MEDIA_NF_TIMEOUT_NF_LP		= 6,	/**< NF LowPower (Default:3000ms) */
	FJ_MEDIA_NF_TIMEOUT_XDMAC_LP	= 7,	/**< XDMAC LowPower (Default:3000ms) */
	FJ_MEDIA_NF_TIMEOUT_CMD_DESC	= 8		/**< CMD descriptor (Default:30ms) */
} FJ_MEDIA_NF_TIMEOUT;

/**
NandFlash BadBlock search area
*/
typedef enum {
	FJ_MEDIA_NF_CODE_AREA_DISABLE		= 0,	/**< not include code area */
	FJ_MEDIA_NF_CODE_AREA_ENABLE		= 1		/**< include code area */
} FJ_MEDIA_NF_BB_SEARCH_AREA;

/**
NandFlash Access cycle
*/
typedef enum {
	E_MEDIA_NF_ACCESS_CYC_20NSEC = 0,		/**< 20 nsec cycle */
	E_MEDIA_NF_ACCESS_CYC_25NSEC = 1,		/**< 25 nsec cycle */
	E_MEDIA_NF_ACCESS_CYC_30NSEC = 2,		/**< 30 nsec cycle */
	E_MEDIA_NF_ACCESS_CYC_50NSEC = 3		/**< 50 nsec cycle */
} E_MEDIA_NF_ACCESS_CYC;

/**
NandFlash Bus I/F Drivability control
*/
typedef enum {
	E_MEDIA_NF_DRIVABILITY_0 = 0,			/**< Driver Type D */
	E_MEDIA_NF_DRIVABILITY_1 = 1,			/**< Driver Type B */
	E_MEDIA_NF_DRIVABILITY_2 = 2,			/**< Driver Type C */
	E_MEDIA_NF_DRIVABILITY_3 = 3			/**< Driver Type A */
} E_MEDIA_NF_DRIVABILITY;

/**
NandFlash Partition area
*/
typedef enum { 
  E_MEDIA_NF_PARTITION_DATA0 = 0,			/**< Partition data 0 */
  E_MEDIA_NF_PARTITION_DATA1 = 1,			/**< Partition data 1 */
  E_MEDIA_NF_PARTITION_DATA2 = 2,			/**< Partition data 2 */
  E_MEDIA_NF_PARTITION_DATA3 = 3,			/**< Partition data 3 */
  E_MEDIA_NF_PARTITION_DATA4 = 4,			/**< Partition data 4 */
  E_MEDIA_NF_PARTITION_DATA5 = 5,			/**< Partition data 5 */
  E_MEDIA_NF_PARTITION_DATA6 = 6,			/**< Partition data 6 */
  E_MEDIA_NF_PARTITION_DATA7 = 7,			/**< Partition data 7 */
  E_MEDIA_NF_PARTITION_DATA8 = 8,			/**< Partition data 8 */
  E_MEDIA_NF_PARTITION_DATA9 = 9,			/**< Partition data 9 */
  E_MEDIA_NF_PARTITION_DATA10 = 10,			/**< Partition data 10 */
  E_MEDIA_NF_PARTITION_DATA11 = 11,			/**< Partition data 11 */
  E_MEDIA_NF_PARTITION_DATA12 = 12,			/**< Partition data 12 */
  E_MEDIA_NF_PARTITION_DATA13 = 13,			/**< Partition data 13 */
  E_MEDIA_NF_PARTITION_DATA14 = 14,			/**< Partition data 14 */
  E_MEDIA_NF_PARTITION_DATA15 = 15			/**< Partition data 15 */
} E_MEDIA_NF_PARTITION_NAME;

/**
eMMC Access mode
*/
typedef enum {
	E_MEDIA_EMMC_ACCESS_MODE_LEGACY = 0,		/**< legacy MMC */
	E_MEDIA_EMMC_ACCESS_MODE_HS_SDR = 1,		/**< High Speed SDR */
	E_MEDIA_EMMC_ACCESS_MODE_HS_200 = 2,		/**< HS200 */
	E_MEDIA_EMMC_ACCESS_MODE_HS_400 = 3			/**< HS400 */
} E_MEDIA_EMMC_ACCESS_MODE;

/**
eMMC Partition area
*/
typedef enum { 
  E_MEDIA_EMMC_PARTITION_DATA0 = 0,			/**< Partition data 0 */
  E_MEDIA_EMMC_PARTITION_DATA1 = 1,			/**< Partition data 1 */
  E_MEDIA_EMMC_PARTITION_DATA2 = 2,			/**< Partition data 2 */
  E_MEDIA_EMMC_PARTITION_DATA3 = 3,			/**< Partition data 3 */
  E_MEDIA_EMMC_PARTITION_DATA4 = 4,			/**< Partition data 4 */
  E_MEDIA_EMMC_PARTITION_DATA5 = 5,			/**< Partition data 5 */
  E_MEDIA_EMMC_PARTITION_DATA6 = 6,			/**< Partition data 6 */
  E_MEDIA_EMMC_PARTITION_DATA7 = 7,			/**< Partition data 7 */
  E_MEDIA_EMMC_PARTITION_DATA8 = 8,			/**< Partition data 8 */
  E_MEDIA_EMMC_PARTITION_DATA9 = 9,			/**< Partition data 9 */
  E_MEDIA_EMMC_PARTITION_DATA10 = 10,		/**< Partition data 10 */
  E_MEDIA_EMMC_PARTITION_DATA11 = 11,		/**< Partition data 11 */
  E_MEDIA_EMMC_PARTITION_DATA12 = 12,		/**< Partition data 12 */
  E_MEDIA_EMMC_PARTITION_DATA13 = 13,		/**< Partition data 13 */
  E_MEDIA_EMMC_PARTITION_DATA14 = 14,		/**< Partition data 14 */
  E_MEDIA_EMMC_PARTITION_DATA15 = 15		/**< Partition data 15 */
} E_MEDIA_EMMC_PARTITION_NAME;


/*----------------------------------------------------------------------*/
/* Structure														    */
/*----------------------------------------------------------------------*/
/**
NandFlash Device Information
*/
typedef struct {
	ULONG	phy_blk_num;				/**< Number of physical blocks */
	USHORT	page_per_blk;				/**< Number of pages in 1block */
	USHORT	sector_per_page;			/**< Number of sectors in 1page */
	USHORT	page_size;					/**< page data size (byte) */
	UCHAR	device_kind;				/**< 1:Large 0:Small */
	ULONG	bad_blk_num;				/**< Number of bad blocks */
	ULONG	physical_free_size;			/**< Free size of physical area */
	ULONG	logical_free_size;			/**< Free size of logical area */
} FJ_NF_DEV_INFO;

/**
NandFlash Device Attribute
*/
typedef struct {
	UINT32 ce_num;							/**< Number of chip enable */
	UINT32 max_bad_block_num;				/**< Number of max bad block */
	UINT32 device_block_num;				/**< Number of device block num */
	UINT32 bad_block_search_page;			/**< Page number to be used in the search of the bad block (page0 is check at fixed.) */
	E_MEDIA_NF_DRIVABILITY drivability0;	/**< NandFlash I/F Drivability control(NFREX) <br>
												 Limit of the set value : 10mA(3.3V) / 6mA(1.8V)
											*/
	E_MEDIA_NF_DRIVABILITY drivability1;	/**< NandFlash I/F Drivability control(NFREX) <br>
												 Limit of the set value : 10mA(3.3V) / 6mA(1.8V)
											*/
	E_MEDIA_NF_DRIVABILITY drivability2;	/**< NandFlash I/F Drivability control(NFWEX) <br>
												 Limit of the set value : 8mA(3.3V) / 6mA(1.8V)
											*/
} FJ_NF_ATTRIBUTE;

/**
NandFlash Partition Information
*/
typedef struct {
	UINT32 part_type;						/**< Partition type. <br>
												  = 0x01 : FAT12<br>
												  = 0x04 : FAT16  (<32MB)<br>
												  = 0x06 : FAT16E (>=32MB)<br>
												  = 0x0B : FAT32<br>
												  = 0x07 : exFAT<br>
											*/
	UINT32 start_lba;						/**< LBA start sector. */
	UINT32 part_size;						/**< Partition size. */
} FJ_NF_PARTITION;

/**
eMMC Partition Information
*/
typedef struct {
	UINT32 part_type;						/**< Partition type. <br>
												  = 0x01 : FAT12<br>
												  = 0x04 : FAT16  (<32MB)<br>
												  = 0x06 : FAT16E (>=32MB)<br>
												  = 0x0B : FAT32<br>
												  = 0x07 : exFAT<br>
											*/
	UINT32 start_lba;						/**< LBA start sector. */
	UINT32 part_size;						/**< Partition size. */
} FJ_EMMC_PARTITION;

/** 
Type is defined to Callback function pointer for SD 
*/
typedef VOID (*SD_CALLBACK)(UCHAR ch);

/*----------------------------------------------------------------------*/
/* Global Data														    */
/*----------------------------------------------------------------------*/
// Nothing Special
/*@} --- end of fj_media_definition */

/*----------------------------------------------------------------------*/
/* Macro															    */
/*----------------------------------------------------------------------*/
/** @weakgroup fj_media_api
@{*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function															    */
/*----------------------------------------------------------------------*/
// get Media insert status function
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the sampling time and number for FJ_Get_MediaDetect() API.
@param[in] ch SD Channel number. (0 or 1)<br>
@param[in] time_per_sampling The sampling time of Media Insert Signal for FJ_Get_MediaDetect() API.
@param[in] num_of_sampling The sampling count of Media Insert Signal for FJ_Get_MediaDetect() API.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks
The sampling time and numbers set by this function is used by only FJ_Get_MediaDetect() API.<br>
Default settings of sampling time is 0ms, sampling number is 0.<br>
*/
FJ_ERR_CODE FJ_Set_MediaDetect(UCHAR ch, UCHAR time_per_sampling, UCHAR num_of_sampling);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get the status of SD media insert.
@param[in] ch SD Channel number. (0 or 1)<br>
@param[out] detect Status of the media insert.<br>
					= 1 : media was inserted.<br>
					= 0 : media was not inserted.<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks
The sampling number and sampling time can be set by FJ_Set_MediaDetect() API.<br>
Before sets by FJ_Set_MediaDetect() API, default settings of sampling number is 0.<br>
If the sampling number is 0, then this function gets the status of MediaInsertSignal at once.<br>
*/
FJ_ERR_CODE FJ_Get_MediaDetect(UCHAR ch, UCHAR* detect);

// sd card interface
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function initialize the SD macro.
@param[in] ch SD Channel number. (0 or 1)<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
FJ_ERR_CODE FJ_SD_Init(UCHAR ch);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets limit of the SD drive strength, speed mode and clock.<br>
In the initialization of the SD card, this value is the upper limit.
@param[in] ch SD Channel number. (0 or 1)<br>
@param[in] drive drive strength.<br>
					= @ref FJ_MEDIA_SD_TYPE_B : Driver Type B<br>
					= @ref FJ_MEDIA_SD_TYPE_A : Driver Type A<br>
					= @ref FJ_MEDIA_SD_TYPE_C : Driver Type C<br>
					= @ref FJ_MEDIA_SD_TYPE_D : Driver Type D<br>
@param[in] mode speed mode.<br>
					= @ref FJ_MEDIA_SD_MODE_DS : DefaultSpeed<br>
					= @ref FJ_MEDIA_SD_MODE_HS : HighSpeed<br>
					= @ref FJ_MEDIA_SD_MODE_SDR12 : UHS-I(SDR12) (ch1 is not supported)<br>
					= @ref FJ_MEDIA_SD_MODE_SDR25 : UHS-I(SDR25) (ch1 is not supported)<br>
					= @ref FJ_MEDIA_SD_MODE_SDR50 : UHS-I(SDR50)<br>
					= @ref FJ_MEDIA_SD_MODE_SDR104 : UHS-I(SDR104)<br>
					= @ref FJ_MEDIA_SD_MODE_UHS2 : UHS-II (ch0 is not supported)<br>
@param[in] clock Max clock.<br>
					= @ref FJ_MEDIA_SD_CLOCK_25M : 25MHz<br>
					= @ref FJ_MEDIA_SD_CLOCK_50M : 50MHz<br>
					= @ref FJ_MEDIA_SD_CLOCK_100M : 100MHz<br>
					= @ref FJ_MEDIA_SD_CLOCK_208M : (ch0)175MHz (ch1)200MHz<br>
					= @ref FJ_MEDIA_SD_CLOCK_UHS2 : 51.851MHz (ch0 is not supported)<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks
	</table>
	Table of SD card(ch0). Combination of speed mode and frequency can be selected.<br>
	- O:can use
	- X:cannot use
	.
	<table>
	<tr>
		<th rowspan=2 style="text-align:center;">Speed Mode</th>
		<th colspan=14 style="text-align:center;">Clock</th>
	</tr>
	<tr>
		<td colspan=1 style="font-size:75%;">FJ_MEDIA_SD_CLOCK_25M</td>
		<td colspan=1 style="font-size:75%;">FJ_MEDIA_SD_CLOCK_50M</td>
		<td colspan=1 style="font-size:75%;">FJ_MEDIA_SD_CLOCK_100M</td>
		<td colspan=1 style="font-size:75%;">FJ_MEDIA_SD_CLOCK_208M</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_MEDIA_SD_MODE_DS</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_MEDIA_SD_MODE_HS</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_MEDIA_SD_MODE_SDR12</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_MEDIA_SD_MODE_SDR25</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_MEDIA_SD_MODE_SDR50</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_MEDIA_SD_MODE_SDR104</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	</tr>
	</table>
<br>
	Table of SD card(ch1). Combination of speed mode and frequency can be selected.<br>
	- O:can use
	- X:cannot use
	.
	<table>
	<tr>
		<th rowspan=2 style="text-align:center;">Speed Mode</th>
		<th colspan=14 style="text-align:center;">Clock</th>
	</tr>
	<tr>
		<td colspan=1 style="font-size:75%;">FJ_MEDIA_SD_CLOCK_25M</td>
		<td colspan=1 style="font-size:75%;">FJ_MEDIA_SD_CLOCK_50M</td>
		<td colspan=1 style="font-size:75%;">FJ_MEDIA_SD_CLOCK_100M</td>
		<td colspan=1 style="font-size:75%;">FJ_MEDIA_SD_CLOCK_208M</td>
		<td colspan=1 style="font-size:75%;">FJ_MEDIA_SD_CLOCK_UHS2</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_MEDIA_SD_MODE_DS</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_MEDIA_SD_MODE_HS</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_MEDIA_SD_MODE_SDR50</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_MEDIA_SD_MODE_SDR104</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_MEDIA_SD_MODE_UHS2</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	  <td align=center>O</td>
	</tr>
	</table>

Is set as the initial value as follows:<br>
SD card(ch0):<br>
@ref FJ_MEDIA_SD_MODE_SDR104 : UHS-I(SDR104)<br>
@ref FJ_MEDIA_SD_CLOCK_208M : 175MHz<br>
SD card(ch1):<br>
@ref FJ_MEDIA_SD_MODE_UHS2 : UHS-II<br>
@ref FJ_MEDIA_SD_CLOCK_UHS2 : 51.851MHz<br>
*/
FJ_ERR_CODE FJ_SD_Config(UCHAR ch, FJ_MEDIA_SD_DRIVE drive, FJ_MEDIA_SD_MODE mode, FJ_MEDIA_SD_CLOCK clock);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the callback function of the card detection.
@param[in] ch SD Channel number. (0 or 1)<br>
@param[in] type Callback event type.<br>
					= @ref FJ_MEDIA_SD_CB_INSERTED : Inserted<br>
					= @ref FJ_MEDIA_SD_CB_EJECTED  : Ejected<br>
@param[in] pcallback Callback function pointer.<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
FJ_ERR_CODE FJ_SD_Set_Event_CB(UCHAR ch, FJ_MEDIA_SD_CB_EVENT type, SD_CALLBACK pcallback);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function initialize the SD card.
@param[in] ch SD Channel number. (0 or 1)<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
FJ_ERR_CODE FJ_SD_Card_Init(UCHAR ch);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the SD Card information(speed mode, clock, write protect status).
@param[in] ch SD Channel number. (0 or 1)<br>
@param[out] mode speed mode.<br>
					= @ref FJ_MEDIA_SD_MODE_DS : DefaultSpeed<br>
					= @ref FJ_MEDIA_SD_MODE_HS : HighSpeed<br>
					= @ref FJ_MEDIA_SD_MODE_SDR12 : UHS-I(SDR12)<br>
					= @ref FJ_MEDIA_SD_MODE_SDR25 : UHS-I(SDR25)<br>
					= @ref FJ_MEDIA_SD_MODE_SDR50 : UHS-I(SDR50)<br>
					= @ref FJ_MEDIA_SD_MODE_SDR104 : UHS-I(SDR104)<br>
					= @ref FJ_MEDIA_SD_MODE_UHS2 : UHS-II<br>
@param[out] clock clock frequency. (KHz)<br>
@param[out] wp_status  Media Write Protect Status.<br>
					= @ref FJ_MEDIA_WP : Write Protect ON<br>
					= @ref FJ_MEDIA_NON_WP : Write Protect OFF<br>
*/
FJ_ERR_CODE FJ_SD_GetCardInfo(UCHAR ch, INT32* mode, ULONG* clock, UCHAR* wp_status);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets hold clock of the SD command line.
@param[in] ch SD Channel number. (0 or 1)<br>
@param[in] hold hold_clock.<br>
					= @ref FJ_MEDIA_SD_HOLD_CLOCK_1_4 : SD_BCLK_I * 1/4<br>
					= @ref FJ_MEDIA_SD_HOLD_CLOCK_2_4 : SD_BCLK_I * 2/4<br>
					= @ref FJ_MEDIA_SD_HOLD_CLOCK_3_4 : SD_BCLK_I * 3/4<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
FJ_ERR_CODE FJ_SD_Set_Hold_Clock(UCHAR ch, FJ_MEDIA_SD_HOLD_CLOCK hold);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the SD Card Lock password.
@param[in] ch SD Channel number. (0 or 1)<br>
@param[in] password SD card lock password.
@param[in] size SD card lock password size.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
FJ_ERR_CODE FJ_SD_Set_CardLockPassword(UCHAR ch, CHAR* password, ULONG size);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the SD Card Lock operation.
@param[in] ch SD Channel number. (0 or 1)<br>
@param[in] operation SD card lock operation.<br>
					= @ref FJ_MEDIA_SD_UNLOCK : Unlock the card<br>
					= @ref FJ_MEDIA_SD_SET_PASSWORD : Sets password<br>
					= @ref FJ_MEDIA_SD_CLR_PASSWORD : Clears password<br>
					= @ref FJ_MEDIA_SD_LOCK : Locks the card<br>
					= @ref FJ_MEDIA_SD_ERACE : Forced Erace operation<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
FJ_ERR_CODE FJ_SD_Set_CardLock(UCHAR ch, FJ_MEDIA_SD_LOCK_OPE operation);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the SD Card power supply.
@param[in] ch SD Channel number. (0 or 1)<br>
@param[in] enable SD card power supply.<br>
					= 1 : power supply enbale.<br>
					= 0 : power supply disable.<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
FJ_ERR_CODE FJ_SD_Set_Card_Power(UCHAR ch, UCHAR enable);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets limit of the SD UHS-II mode, speed range and power limit.
@param[in] ch SD Channel number. (1 only)<br>
@param[in] mode UHS-II mode.<br>
					= @ref FJ_MEDIA_SD_UHS2_MODE_FD : Full Duplex(FD) mode<br>
					= @ref FJ_MEDIA_SD_UHS2_MODE_2LHD : 2Line-Half Duplex(2L-HD) mode (not supported)<br>
@param[in] range Speed range.<br>
					= @ref FJ_MEDIA_SD_UHS2_RANGE_A : UHS-II Speed range A(x15)<br>
					= @ref FJ_MEDIA_SD_UHS2_RANGE_B : UHS-II Speed range B(x30)<br>
@param[in] power Power limit.<br>
					= @ref FJ_MEDIA_SD_UHS2_PWR_0_72W : UHS-II Power Limit : Max 0.72W<br>
					= @ref FJ_MEDIA_SD_UHS2_PWR_1_44W : UHS-II Power Limit : Max 1.44W<br>
					= @ref FJ_MEDIA_SD_UHS2_PWR_2_16W : UHS-II Power Limit : Max 2.16W<br>
					= @ref FJ_MEDIA_SD_UHS2_PWR_2_88W : UHS-II Power Limit : Max 2.88W<br>
					= @ref FJ_MEDIA_SD_UHS2_PWR_1_80W : UHS-II Power Limit : Max 1.80W<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks
Is set as the initial value as follows:<br>
@ref FJ_MEDIA_SD_UHS2_MODE_FD : Full Duplex(FD) mode<br>
@ref FJ_MEDIA_SD_UHS2_RANGE_B : UHS-II Speed range B(x30)<br>
@ref FJ_MEDIA_SD_UHS2_PWR_1_44W : UHS-II Power Limit : Max 1.44W<br>
*/
FJ_ERR_CODE FJ_SD_Set_UHS2_ModeSelect(UCHAR ch, FJ_MEDIA_SD_UHS2_MODE mode, FJ_MEDIA_SD_UHS2_RANGE range, FJ_MEDIA_SD_UHS2_PWR power);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the SD UHS-II mode, speed range and power limit.
@param[in] ch SD Channel number. (1 only)<br>
@param[out] mode UHS-II mode.<br>
					= @ref FJ_MEDIA_SD_UHS2_MODE_FD : Full Duplex(FD) mode<br>
					= @ref FJ_MEDIA_SD_UHS2_MODE_2LHD : 2Line-Half Duplex(2L-HD) mode<br>
@param[out] range Speed range.<br>
					= @ref FJ_MEDIA_SD_UHS2_RANGE_A : UHS-II Speed range A(x15)<br>
					= @ref FJ_MEDIA_SD_UHS2_RANGE_B : UHS-II Speed range B(x30)<br>
@param[out] power Power limit.<br>
					= @ref FJ_MEDIA_SD_UHS2_PWR_0_72W : UHS-II Power Limit : Max 0.72W<br>
					= @ref FJ_MEDIA_SD_UHS2_PWR_1_44W : UHS-II Power Limit : Max 1.44W<br>
					= @ref FJ_MEDIA_SD_UHS2_PWR_2_16W : UHS-II Power Limit : Max 2.16W<br>
					= @ref FJ_MEDIA_SD_UHS2_PWR_2_88W : UHS-II Power Limit : Max 2.88W<br>
					= @ref FJ_MEDIA_SD_UHS2_PWR_1_80W : UHS-II Power Limit : Max 1.80W<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
FJ_ERR_CODE FJ_SD_Get_UHS2_ModeSelect(UCHAR ch, UINT32* mode, UINT32* range, UINT32* power);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the SD UHS2 Card power saving mode.
@param[in] ch SD Channel number. (1 only)<br>
@param[in] mode Power saving mode.<br>
					= @ref FJ_MEDIA_SD_UHS2_PMODE_DORMANT : Dormant state mode<br>
					= @ref FJ_MEDIA_SD_UHS2_PMODE_HIBERNATE : Hibernate state mode<br>
@param[in] enable Power saving enable.<br>
					= 1 : power saving enbale.<br>
					= 0 : power saving disable.<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
FJ_ERR_CODE FJ_SD_Set_UHS2_PowerSaving(UCHAR ch, FJ_MEDIA_SD_UHS2_PMODE mode, UCHAR enable);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the detection time of the SD UHS-II card.
@param[in] ch SD Channel number. (1 only)<br>
@param[in] time max detection time (ms).<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks
Initial value of detection time is 1ms.<br>
Although it meets SD standard with detection time of 1ms, but if there are cards that can not be detected in that time, it can be adjusted with this API.<br>
If UHS-II card is detected before max detection time, it ends there.<br>
However, in case of UHS-I card, the initialization time will be longer by the set detection time.<br>
*/
FJ_ERR_CODE FJ_SD_Set_UHS2_DetectTime(UCHAR ch, UINT32 time);

// nand flash interface
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function initialize the NandFlash macro.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Init(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set the attribute of the NandFlash.<br>
Initialize the NandFlash using this attribute.
@param[in] attr NandFlash attribute information.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Config(FJ_NF_ATTRIBUTE* attr);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function initialize the NandFlash device.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Device_Init(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the Manufacturer ID and Device ID of the NandFlash.
@param[out] mid Manufacturer ID.
@param[out] did Device ID.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Get_MID_DID(UINT32* mid, UINT32* did);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the device information of the NandFlash.<br>
Device information see @ref FJ_NF_DEV_INFO.
@param[out] nf_dev_info NandFlash device information.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Get_Device_Info(FJ_NF_DEV_INFO* nf_dev_info);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function erase the NandFlash all area.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Erase(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function writes the data of specified block of the NandFlash.
@param[in] phy_blk physical block number.
@param[in] data_addr The buffer address for writing into data.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The initial value is following<br>
In the case of physical block that is mapped to the logical space, it return an FJ_ERR_NG.
*/
FJ_ERR_CODE FJ_NF_Write_Physical_Data(ULONG phy_blk, UINT32* data_addr);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function reads the data of specified block of the NandFlash.
@param[in] phy_blk physical block number.
@param[out] data_addr The buffer address for reading into data.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_READ_NF_BAD_BLOCK Read error by bad block.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Read_Physical_Data(ULONG phy_blk, UINT32* data_addr);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function write data to NandFlash Logical sector.
@param[in] start_sec start sector number (LBA).
@param[in] write_sec_num number of write sectors. (1sector = 512byte)
@param[in] data_addr address of write data.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Write_Logical_Sector(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function read data from NandFlash Logical sector.
@param[in] start_sec start sector number (LBA).
@param[in] read_sec_num number of read sectors. (1sector = 512byte)
@param[out] data_addr address of read data.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NF_NO_ALLOCATE Physical block not allocated.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Read_Logical_Sector(ULONG start_sec, ULONG read_sec_num, UINT32* data_addr);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function write data to NandFlash Logical sector(not use interrupt/OS).
@param[in] start_sec start sector number (LBA).
@param[in] write_sec_num number of write sectors. (1sector = 512byte)
@param[in] data_addr address of write data.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
Please this function is used in writing of when an exception occurs.
*/
FJ_ERR_CODE FJ_NF_Write_Logical_Sector_PIO(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function partition the logical space of NandFlash. 
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Partition(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function read partition information from the logical space of NandFlash.
@param[out] part_info Partition information (array[8]).
@param[out] part_num Number of partition information.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
This function reads the information of up to eight FJ_NF_PARTITION structures.<br>
Please pass the array address of area for eight FJ_NF_PARTITION structure to part_info.<br>
*/
FJ_ERR_CODE FJ_NF_Partition_Read(FJ_NF_PARTITION* part_info, ULONG* part_num);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get the start sector number and size of the specified partition of physical space.
@param[in]  name name of partition table. 
@param[out] start_sec_num start physical sector number.
@param[out] data_size used data size(byte).
@param[out] total_size total size.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
Total size includes the bad blocks.
*/
FJ_ERR_CODE FJ_NF_Get_Partition_Info(E_MEDIA_NF_PARTITION_NAME name, ULONG* start_sec_num, ULONG* data_size, ULONG* total_size);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get number of badblock.
@param[in] limit max bad block number for storing in a buffer.
@param[in] mode badblock search area.<br>
					= @ref FJ_MEDIA_NF_CODE_AREA_DISABLE : Not include code area<br>
					= @ref FJ_MEDIA_NF_CODE_AREA_ENABLE : Include code area<br>
@param[out] num pointer of bad block number.
@param[out] buf pointer of buffer for storing bad block.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Get_BadBlock_Num(ULONG limit, FJ_MEDIA_NF_BB_SEARCH_AREA mode, ULONG* num, UINT32* buf);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function get NAND Flash Translation Layer(NFTL) information.
@param[out] param_blk_top NFTL top sector number.
@param[out] param_blk_size number of NFTL sectors. (1sector = 512byte)
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_NF_Get_ParamBlk_Info(ULONG* param_blk_top, ULONG* param_blk_size);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function get exception log info.
@param[out] exception_log_top exception log top information.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
This function is used in SDK.
*/
FJ_ERR_CODE FJ_NF_Get_ExceptionLog_Info(UINT32* exception_log_top);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function sets retry count for read/write failure.
@param[in] retry_cnt_read retry count for read.
@param[in] retry_cnt_write retry count for write.
@retval None.
@remarks
The initial value is following<br>
read retry count : 1<br>
write retry count : 1<br>
*/
VOID FJ_NF_Set_Retry_Cnt(UCHAR retry_cnt_read, UCHAR retry_cnt_write);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function gets retry count for read/write failure.
@param[out] retry_cnt_read retry count for read.
@param[out] retry_cnt_write retry count for write.
@retval None.
*/
VOID FJ_NF_Get_Retry_Cnt(UCHAR* retry_cnt_read, UCHAR* retry_cnt_write);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function set timeout.<br>
Timeout type see @ref FJ_MEDIA_NF_TIMEOUT.
@param[in] type timeout type.
@param[in] timeout timeout(msec).
@retval None.
*/
VOID FJ_NF_Set_Timeout(FJ_MEDIA_NF_TIMEOUT type, USHORT timeout);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function get timeout.<br>
Timeout type see @ref FJ_MEDIA_NF_TIMEOUT.
@param[in] type timeout type.
@param[out] timeout timeout(msec).
@retval None.
*/
VOID FJ_NF_Get_Timeout(FJ_MEDIA_NF_TIMEOUT type, USHORT* timeout);

// eMMC interface
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function initialize the eMMC macro.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_EMMC_Init(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets limit of the eMMC access mode.<br>
In the initialization of the eMMC, this value is the upper limit.
@param[in] mode access mode.<br>
					= @ref E_MEDIA_EMMC_ACCESS_MODE_LEGACY : legacy MMC<br>
					= @ref E_MEDIA_EMMC_ACCESS_MODE_HS_SDR : High Speed SDR<br>
					= @ref E_MEDIA_EMMC_ACCESS_MODE_HS_200 : HS200<br>
					= @ref E_MEDIA_EMMC_ACCESS_MODE_HS_400 : HS400<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
Initial value of access mode is HS400.<br>
If you want to change the access mode, please call this API.<br>
*/
FJ_ERR_CODE FJ_EMMC_Config(E_MEDIA_EMMC_ACCESS_MODE mode);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function initialize the eMMC device.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_EMMC_Device_Init(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the eMMC device information(access mode).<br>
@param[out] mode speed mode.<br>
					= @ref E_MEDIA_EMMC_ACCESS_MODE_LEGACY : legacy MMC<br>
					= @ref E_MEDIA_EMMC_ACCESS_MODE_HS_SDR : High Speed SDR<br>
					= @ref E_MEDIA_EMMC_ACCESS_MODE_HS_200 : HS200<br>
					= @ref E_MEDIA_EMMC_ACCESS_MODE_HS_400 : HS400<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_EMMC_Get_Device_Info(UINT32* mode);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function write data to eMMC device.
@param[in] start_sec start sector number.
@param[in] write_sec_num number of write sectors. (1sector = 512byte)
@param[in] data_addr address of write data.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_EMMC_Write_Sector(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function read data from eMMC device.
@param[in] start_sec start sector number.
@param[in] read_sec_num number of read sectors. (1sector = 512byte)
@param[out] data_addr address of read data.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_EMMC_Read_Sector(ULONG start_sec, ULONG read_sec_num, UINT32* data_addr);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function partition the storage area of eMMC. 
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
 */
FJ_ERR_CODE FJ_EMMC_Partition(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function read partition information from the storage area of eMMC.
@param[out] part_info Partition information (array[8]).
@param[out] part_num Number of partition information.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
This function reads the information of up to eight FJ_EMMC_PARTITION structures.<br>
Please pass the array address of area for eight FJ_EMMC_PARTITION structure to part_info.<br>
 */
FJ_ERR_CODE FJ_EMMC_Partition_Read(FJ_EMMC_PARTITION* part_info, ULONG* part_num);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get the start sector number and size of the specified partition of eMMC.
@param[in]  name name of partition table. 
@param[out] start_sec_num start physical sector number.
@param[out] data_size used data size(byte).
@param[out] total_size total size.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_EMMC_Get_Partition_Info(E_MEDIA_EMMC_PARTITION_NAME name, ULONG* start_sec_num, ULONG* data_size, ULONG* total_size);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function get eMMC parameter area information.
@param[out] param_blk_top parameter area top sector number.
@param[out] param_blk_size number of parameter area sectors. (1sector = 512byte)
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
 */
FJ_ERR_CODE FJ_EMMC_Get_ParamBlk_Info(ULONG* param_blk_top, ULONG* param_blk_size);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function get eMMC exception log info.
@param[out] exception_log_top exception log top information.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
This function is used in SDK.
 */
FJ_ERR_CODE FJ_EMMC_Get_ExceptionLog_Info(UINT32* exception_log_top);

// --- REMOVE_USB BEGIN ---
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get Media(SD) Write Protect Status.<br>
@param[out] pwWpInfo  Media Write Protect Status.<br>
						= @ref FJ_MEDIA_WRITE_PROTECT <br>
@retval FJ_ERR_OK(0) Normal end.
@retval FJ_ERR_NG(-1) Error (Non Detect Media).
@retval FJ_ERR_NOT_MOUNT(0x0207) SD is not mount.
@remarks Please mount media(SD) before calling this API.
*/
FJ_ERR_CODE	FJ_GetWriteProtectStatus(UCHAR* pwWpInfo);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the media type and formatted type of the using media.
@param[out] media The Media Type of using.<br>
					= @ref FJ_USED_MEDIA_UNKNOWN(0x00) : unknown media<br>
					= @ref FJ_USED_MEDIA_NF(0x01) : Nand Flash<br>
					= @ref FJ_USED_MEDIA_SD(0x02) : SD card<br>
					= @ref FJ_USED_MEDIA_SDHC(0x03) : SDHC card<br>
					= @ref FJ_USED_MEDIA_SDXC(0x04) : SDXC card<br>
@param[out] format_type The Format(filesystem) Type of the using media.<br>
					= @ref FJ_FS_TYPE_UNKNOWN(0x00) : unknown filesystem<br>
					= @ref FJ_FS_TYPE_FAT12(0x02) : FAT12 format<br>
					= @ref FJ_FS_TYPE_FAT16(0x04) : FAT16 format (<= 32MB)<br>
					= @ref FJ_FS_TYPE_FAT16E(0x06) : FAT16 format (>  32MB)<br>
					= @ref FJ_FS_TYPE_EXFAT(0x07) : exFAT *1 format<br>
					= @ref FJ_FS_TYPE_FAT32(0x0B) : FAT32 format<br>
					= @ref FJ_FS_TYPE_FAT32X(0x0C) : FAT32 (LBA) format<br>

@retval FJ_ERR_OK(0) Normal end.
@retval FJ_ERR_NG(-1) Error occurred.
@retval FJ_ERR_NOT_MOUNT(0x0203) Media is not mount.
@pre @ref FJ_Mount is called before this API.
@remarks
	Table. Combination of Media Type and Format Type to be able to use the Quick/Full-Format.<br>
	- O:can use
	- X:cannot use
	.
	<table>
	<tr>
		<th rowspan=3 style="text-align:center;">Format Type</th>
		<th colspan=14 style="text-align:center;">Media Type</th>
	</tr>
	<tr>
		<td colspan=2 style="font-size:75%;">FJ_USED_MEDIA_NF(0x01)</td>
		<td colspan=2 style="font-size:75%;">FJ_USED_MEDIA_SD(0x02)</td>
		<td colspan=2 style="font-size:75%;">FJ_USED_MEDIA_SDHC(0x03)</td>
		<td colspan=2 style="font-size:75%;">FJ_USED_MEDIA_SDXC(0x04)</td>
	</tr>
	<tr>
		<th style="font-size:75%;text-align:center;">Quick</th>
		<th style="font-size:75%;text-align:center;">Full</th>
		<th style="font-size:75%;text-align:center;">Quick</th>
		<th style="font-size:75%;text-align:center;">Full</th>
		<th style="font-size:75%;text-align:center;">Quick</th>
		<th style="font-size:75%;text-align:center;">Full</th>
		<th style="font-size:75%;text-align:center;">Quick</th>
		<th style="font-size:75%;text-align:center;">Full</th>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_FS_TYPE_UNKNOWN(0x00)</td>
	  <td align=center>X</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_FS_TYPE_FAT12(0x01)</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_FS_TYPE_FAT16(0x04)</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_FS_TYPE_FAT16E(0x06)</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_FS_TYPE_EXFAT(0x07)</td>
	  <td align=center>X</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_FS_TYPE_FAT32(0x0B)</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	<tr>
	  <td style="font-size:75%;">FJ_FS_TYPE_FAT32X(0x0C)</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>O</td>
	  <td align=center>X</td>
	  <td align=center>X</td>
	</tr>
	</table>
<br>
 * *1 To use the exFAT is a license required.
*/
FJ_ERR_CODE	FJ_Get_Media_Format_Type(UCHAR* media, UCHAR* format_type);
// --- REMOVE_USB END ---

/*@} --- end of fj_media_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_media_message
@{*/

/**
@def FJ_HM_FLASH_CARD_DETECT
This message is sent when flash card detect.<br>
This message is used in SDK.
@param 0x00 Card removed.
@param 0x01 Card inserted.
*/

/**
@def FJ_HM_NOT_ENOUGH_FLASH_MEMORY
This message is sent when not enugh flash card memory.<br>
This message is used in SDK.
*/

/**
@def FJ_HM_MOUNT_COMPLETED
This message is sent after end media mount process.<br>
This message is used in SDK.
@param 0x00 Normal end.
@param 0x01 Error. Terminate process.
@param 0x02 Error. Media format is not FAT filesystem.
*/

/**
@def FJ_HM_MOUNT_DUPLICATED_FOUND
This message is sent when DCF duplication found.<br>
This message is used in SDK.
@param 0x00 No Duplication.
@param 0x01 Dirs are duplicated.
@param 0x02 Files are duplicated.
@param 0x03 Both Dirs and Files are duplicated.
*/

/**
@def FJ_HM_GET_MEDIA_STATUS_COMPLETED
This message is sent after end get media status process.<br>
This message is used in SDK.
*/

/**
@def FJ_HM_NOT_ENOUGH_DCFDB
This message is sent when DCF-DB not enough.<br>
This message is used in SDK.
*/

/**
@def FJ_HM_MAX_FILENUMBER_DCFDB
This message is sent when DCF-DB Max File Number over.<br>
This message is used in SDK.
*/

/**
@def FJ_HM_SLOW_MEDIA_ERROR
This message is sent when slow media error occured.<br>
This message is used in SDK.
*/
/*@} --- end of fj_media_message */
#endif	// _FJ_MEDIA_H
