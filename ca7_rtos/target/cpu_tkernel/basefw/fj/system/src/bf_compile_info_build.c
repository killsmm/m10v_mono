/**
 * @file		bf_compile_info_build.c
 * @brief		build/update compile information.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "bf_compile_info_build.h"
#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static const CHAR g_bf_compile_info_build_datetime_str[] = "2022/12/29 11:47:47    ";
static const CHAR g_bf_compile_info_build_opt_str[] = "-c -pipe -g3 -gdwarf-2 -std=gnu99 -fno-builtin -ffreestanding -mtune=cortex-a7 -marm -Ofast -march=armv7-a -mfloat-abi=hard -mfpu=neon-vfpv4 -mthumb-interwork -mstructure-size-boundary=8 -ffreestanding -fsigned-char -Wall -D CO_ES3_HARDWARE -D CO_MEMORY_2GB -D CO_RTOS_CPUx2 -D BOARD_TYPE_SNI_SMALL -D ETHER_EN -D MPS_PMIC_DIS -D CO_SD_AUTO_MOUNT_FOR_TKERNEL_VERSION -D CO_LCD_ENABLE -D CO_LENS_NOT_DRIVE -D CO_AUTO_MODE_CHANGE_ON -D CO_WAIT_FOR_INTERRUPT_ON -D \"UART_CTRL_CH=2\" -D CO_BF_STARTUP_ENABLE_ERROR_INT -D CO_MEASURE_TIMER -D CO_DEBUG_MBALOG_ON -D CO_BF_MBALOG_ON -D CO_DEBUG_MBALOG_OVERWRITE -D CO_PMU_MONITOR_ON -D CO_SDRAMC_MONITOR_ON -D CO_MPROFILE_ON -D CO_4K2K_QFHD -D CO_REALOS_FILE_DEBUG_ON -D CO_DCF_EXIF_ON -D CO_GYRO_SAMPLERATE_8KHZ -D CO_SUPPORT_30FPS -D CO_SUPPORT_60FPS -D CO_IQBIN_GENERAL -D CO_IQBIN_NUM_LIMIT -D D_COM_BUF_FOR_TERMINAL -D CO_HOST_PCIE_EP -D CO_HOST_PCIE_RC -D \"CO_MOVIE_FF_AUDIO_PATTERN=0\" -D \"D_LCD_DRV_LPM030M369F_YUV_STRIPE=0x00010020\" -D CO_TKERNEL_LINUX_COOP -D D_DRV_SENSOR_IMX477 -D D_DRV_SENSOR_IMX477_SENSOR_MASTER -D CO_SD0_DRIVER_ENABLE -D CO_SD1_DRIVER_ENABLE -D CO_NF_DRIVER_ENABLE -D CO_EMMC_DRIVER_ENABLE -D CO_BOARD_TYPE_RK -D CO_FS_USED_PRFILE2 -D \"CO_UI_TYPE=0\" -D \"CO_IVE_QR_MARKER_ON=0\" -Wno-format-zero-length";


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

const CHAR* BF_CompileInfoBuild_Get_DataTimeStr( VOID )
{
	return g_bf_compile_info_build_datetime_str;
}

const CHAR* BF_CompileInfoBuild_Get_OptStr( VOID )
{
	return g_bf_compile_info_build_opt_str;
}

