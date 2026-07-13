/**
 * @file		ip_gpu.h
 * @brief		GPU rotate sample
 * @author		Hattori
 * @since		2016/07/14
 * @date		2016/07/14
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT 2016</I></B>
 */

// ****************************************************************************
//                          Include
// ****************************************************************************
#include "ddim_typedef.h"
#include "sdram_map_common.h"
// ****************************************************************************
//                          Define
// ****************************************************************************
#if 0
#define	D_IP_GPU_INPUT				0xA93D4000
#define	D_IP_GPU_OUTPUT_LCD_BANK0	0xA941F000
#define	D_IP_GPU_OUTPUT_LCD_BANK1	0xA946A000
#define	D_IP_GPU_HDMI_INPUT			0xAAD96000
#define	D_IP_GPU_OUTPUT_HDMI_BANK0	0xA962C000
#define	D_IP_GPU_OUTPUT_HDMI_BANK1	0xA98CF000

#define D_IP_GPU_HEAP				0xA9000000
#else
#define	D_IP_GPU_INPUT				SDRAM_ADR_HOST_LCD_OSD_FRAME_DATA_LOAD_AREA
#define	D_IP_GPU_OUTPUT_LCD_BANK0	(SDRAM_ADR_HOST_LCD_OSD_FRAME_DATA_AREA_0 + 0x4B000)
#define	D_IP_GPU_OUTPUT_LCD_BANK1	SDRAM_ADR_HOST_LCD_OSD_FRAME_DATA_AREA_1
#define	D_IP_GPU_HDMI_INPUT			SDRAM_ADR_HOST_HDMI_OSD_FRAME_DATA_LOAD_AREA
#define	D_IP_GPU_OUTPUT_HDMI_BANK0	SDRAM_ADR_HOST_HDMI_OSD_FRAME_DATA_AREA_0
#define	D_IP_GPU_OUTPUT_HDMI_BANK1	SDRAM_ADR_HOST_HDMI_OSD_FRAME_DATA_AREA_1

#define D_IP_GPU_HEAP				(SDRAM_ADR_RTOS_RAM + 0x5000000)
#endif

#define	D_IP_GPU_OK					(0)
#define	D_IP_GPU_NG					(-1)
// ****************************************************************************
//                           Structure
// ****************************************************************************

// ****************************************************************************
//                           Global Data
// ****************************************************************************

// ****************************************************************************
//                        External Method Definition
// ****************************************************************************
/******************************************************************************
 * @brief	GPU software reset
 * @param	None
 * @retval	None
 */
VOID IP_GPU_SoftReset(VOID);

/******************************************************************************
 * @brief	GPU open (set HEAP)
 * @param	None
 * @retval	None
 */
INT32 IP_GPU_Open( VOID );

/******************************************************************************
 * @brief	rotate HDMI
 * @param	None
 * @retval	None
 */
VOID IP_GPU_rotateHDMI();

/******************************************************************************
 * @brief	rotate LCD
 * @param	None
 * @retval	None
 */
VOID IP_GPU_rotateLCD();

/******************************************************************************
 * @brief	buffer setting for LCD
 * @param	None
 * @retval	None
 */
VOID IP_GPU_BufferSetLCD();

/******************************************************************************
 * @brief	buffer setting for HDMI
 * @param	None
 * @retval	None
 */
VOID IP_GPU_BufferSetHDMI();




