/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "camera_config.h"
#include "camera_use.h"
#include "os_user_custom.h"

#if USE_SENSOR == SENSOR_IMX477
	#include "camera_sensor_imx477.h"
#elif USE_SENSOR == SENSOR_IMX277
	#include "camera_sensor_imx277.h"
#elif USE_SENSOR == SENSOR_IMX383
	#include "camera_sensor_imx383.h"
#elif USE_SENSOR == SENSOR_IMX533
	#include "camera_sensor_imx533.h"
#elif USE_SENSOR == SENSOR_IMX183
	#include "camera_sensor_imx183.h"
#elif USE_SENSOR == SENSOR_IMX363
	#include "camera_sensor_imx363.h"
#elif USE_SENSOR == SENSOR_IMX290
	#include "camera_sensor_imx290.h"
#elif USE_SENSOR == SENSOR_OS08A10
	#include "camera_sensor_OS08A10.h"
#elif USE_SENSOR == SENSOR_AR0430
	#include "camera_sensor_AR0430.h"
#elif USE_SENSOR == SENSOR_IMX377
	#include "camera_sensor_imx377.h"
#elif USE_SENSOR == SENSOR_OV12895
	#include "camera_sensor_ov12895.h"
#elif USE_SENSOR == SENSOR_IMX477_IMX183
	#include "camera_sensor_imx477_imx183.h"
#elif USE_SENSOR == SENSOR_IMX577
	#include "camera_sensor_imx577.h"
#elif USE_SENSOR == SENSOR_IMX269
	#include "camera_sensor_imx269.h"
#elif USE_SENSOR == SENSOR_PSEUDO
	#include "camera_sensor_pseudo.h"
#elif USE_SENSOR == SENSOR_IMX571
	#include "camera_sensor_imx571.h"
#endif

#if USE_GYRO == GYRO_ICM20948
	#include "camera_gyro_icm20948.h"
#elif USE_GYRO == GYRO_MPU9250
	#include "camera_gyro_mpu9250.h"
#elif USE_GYRO == GYRO_ICM20602
	#include "camera_gyro_icm20602.h"
#endif
//#define USE_FOCUS FOCUS_F103T
#if USE_FOCUS == FOCUS_ZC525
	#include "camera_focus_zc525.h"
#elif USE_FOCUS == FOCUS_M43
	#include "camera_focus_m43.h"
#elif USE_FOCUS == FOCUS_F103T
	#include "camera_focus_f103t.h"	
#endif


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
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

//------------------------------------------------------------------------
// CAMERA
//------------------------------------------------------------------------
const T_CAMERA CAMERA = 
{
#if USE_SENSOR == SENSOR_IMX477
	
	.sensor_num				= 4,// 4 or 2 or 1
	.Func_Clock_GPIO_Init	= Sensor_IMX477_Clock_Gpio_Init,
	.sensor_str				= (char *)"IMX477",
	.start_frame_num		= 0,

	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX477_BASIC,
	.sensor[E_CAMERA_ID_0].mode			= IMX477_MODE,
	.sensor[E_CAMERA_ID_0].mode_num		= E_SENSOR_IMX477_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX477_MODE_Full_4056x3040_RAW12_40fps,
	
	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_0].F_number		= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit		= 256, // ob is overwritten by IQ bin
	
	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby			= Sensor_IMX477_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX477_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX477_Mode,
	.sensor[E_CAMERA_ID_0].Func_Pdaf_Mode		= NULL, // Not support in SDK
	.sensor[E_CAMERA_ID_0].Func_Multi			= Sensor_IMX477_Multi,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Stream			= Sensor_IMX477_Stream,

	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX477_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp			= Sensor_IMX477_Set_Exp,
	
	.sensor[E_CAMERA_ID_0].Func_DOL_Calc_Exp	= Sensor_IMX477_DOL_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_DOL_Set_Exp		= Sensor_IMX477_DOL_Set_Exp,
	
	.sensor[E_CAMERA_ID_0].Func_Get_PdInfo		= NULL, // Not support in SDK
	
	// sensor 1
	.sensor[E_CAMERA_ID_1].basic		= &IMX477_BASIC,
	.sensor[E_CAMERA_ID_1].mode			= IMX477_MODE,
	.sensor[E_CAMERA_ID_1].mode_num		= E_SENSOR_IMX477_MODE_MAX,
	.sensor[E_CAMERA_ID_1].mode_full	= E_SENSOR_IMX477_MODE_Full_4056x3040_RAW12_40fps,
	
	.sensor[E_CAMERA_ID_1].port_xclr	= E_DD_TOP_GPIO_P81,
#if defined(ETHER_EN)
    // PG3/4/5 are used for Ethernet, cannot configure here, using sensor 0 PG0/1/2 since sensor cannot be used when ETHER_EN is enabled
	.sensor[E_CAMERA_ID_1].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_1].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_1].port_inck	= E_DD_TOP_GPIO_PG2,
#else
    .sensor[E_CAMERA_ID_1].port_xhs		= E_DD_TOP_GPIO_PG3,
	.sensor[E_CAMERA_ID_1].port_xvs		= E_DD_TOP_GPIO_PG4,
	.sensor[E_CAMERA_ID_1].port_inck	= E_DD_TOP_GPIO_PG5,
#endif
	.sensor[E_CAMERA_ID_1].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_1].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_1].i2c_ch		= FJ_PERI_CH_1,
	.sensor[E_CAMERA_ID_1].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_1].F_number		= 1.8,
	.sensor[E_CAMERA_ID_1].ob_12bit		= 256, // ob is overwritten by IQ bin
	
	.sensor[E_CAMERA_ID_1].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_1].Func_Standby			= Sensor_IMX477_Standby,
	.sensor[E_CAMERA_ID_1].Func_CancelStandby	= Sensor_IMX477_CancelStandby,
	.sensor[E_CAMERA_ID_1].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_1].Func_Mode			= Sensor_IMX477_Mode,
	.sensor[E_CAMERA_ID_1].Func_Multi			= Sensor_IMX477_Multi,
	.sensor[E_CAMERA_ID_1].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_1].Func_Stream			= Sensor_IMX477_Stream,

	.sensor[E_CAMERA_ID_1].Func_Calc_Exp		= Sensor_IMX477_Calc_Exp,
	.sensor[E_CAMERA_ID_1].Func_Set_Exp			= Sensor_IMX477_Set_Exp,
	
	// sensor 2
	.sensor[E_CAMERA_ID_2].basic		= &IMX477_BASIC,
	.sensor[E_CAMERA_ID_2].mode			= IMX477_MODE,
	.sensor[E_CAMERA_ID_2].mode_num		= E_SENSOR_IMX477_MODE_MAX,
	.sensor[E_CAMERA_ID_2].mode_full	= E_SENSOR_IMX477_MODE_Full_4056x3040_RAW12_40fps,
	
	.sensor[E_CAMERA_ID_2].port_xclr	= E_DD_TOP_GPIO_P82,
	.sensor[E_CAMERA_ID_2].port_xhs		= E_DD_TOP_GPIO_PG6,
	.sensor[E_CAMERA_ID_2].port_xvs		= E_DD_TOP_GPIO_PG7,
	.sensor[E_CAMERA_ID_2].port_inck	= E_DD_TOP_GPIO_PH0,
	.sensor[E_CAMERA_ID_2].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_2].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_2].i2c_ch		= FJ_PERI_CH_1,
	.sensor[E_CAMERA_ID_2].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT2,
	.sensor[E_CAMERA_ID_2].F_number		= 1.8,
	.sensor[E_CAMERA_ID_2].ob_12bit		= 256, // ob is overwritten by IQ bin
	
	.sensor[E_CAMERA_ID_2].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_2].Func_Standby			= Sensor_IMX477_Standby,
	.sensor[E_CAMERA_ID_2].Func_CancelStandby	= Sensor_IMX477_CancelStandby,
	.sensor[E_CAMERA_ID_2].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_2].Func_Mode			= Sensor_IMX477_Mode,
	.sensor[E_CAMERA_ID_2].Func_Multi			= Sensor_IMX477_Multi,
	.sensor[E_CAMERA_ID_2].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_2].Func_Stream			= Sensor_IMX477_Stream,
	
	.sensor[E_CAMERA_ID_2].Func_Calc_Exp		= Sensor_IMX477_Calc_Exp,
	.sensor[E_CAMERA_ID_2].Func_Set_Exp			= Sensor_IMX477_Set_Exp,
	
	// sensor 3
	.sensor[E_CAMERA_ID_3].basic		= &IMX477_BASIC,
	.sensor[E_CAMERA_ID_3].mode			= IMX477_MODE,
	.sensor[E_CAMERA_ID_3].mode_num		= E_SENSOR_IMX477_MODE_MAX,
	.sensor[E_CAMERA_ID_3].mode_full	= E_SENSOR_IMX477_MODE_Full_4056x3040_RAW12_40fps,
	
	.sensor[E_CAMERA_ID_3].port_xclr	= E_DD_TOP_GPIO_P83,
	.sensor[E_CAMERA_ID_3].port_xhs		= E_DD_TOP_GPIO_PH1,
	.sensor[E_CAMERA_ID_3].port_xvs		= E_DD_TOP_GPIO_PH2,
	.sensor[E_CAMERA_ID_3].port_inck	= E_DD_TOP_GPIO_PH3,
	.sensor[E_CAMERA_ID_3].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_3].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_3].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_3].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT2,
	.sensor[E_CAMERA_ID_3].F_number		= 1.8,
	.sensor[E_CAMERA_ID_3].ob_12bit		= 256, // ob is overwritten by IQ bin

	.sensor[E_CAMERA_ID_3].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_3].Func_Standby			= Sensor_IMX477_Standby,
	.sensor[E_CAMERA_ID_3].Func_CancelStandby	= Sensor_IMX477_CancelStandby,
	.sensor[E_CAMERA_ID_3].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_3].Func_Mode			= Sensor_IMX477_Mode,
	.sensor[E_CAMERA_ID_3].Func_Multi			= Sensor_IMX477_Multi,

	.sensor[E_CAMERA_ID_3].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_3].Func_Stream			= Sensor_IMX477_Stream,

	.sensor[E_CAMERA_ID_3].Func_Calc_Exp		= Sensor_IMX477_Calc_Exp,
	.sensor[E_CAMERA_ID_3].Func_Set_Exp			= Sensor_IMX477_Set_Exp,

#elif USE_SENSOR == SENSOR_IMX577
	
	.sensor_num				= 4,// 4 or 2 or 1
	.Func_Clock_GPIO_Init	= Sensor_IMX577_Clock_Gpio_Init,
	.sensor_str				= (char *)"IMX577",
	.start_frame_num		= 0,

	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX577_BASIC,
	.sensor[E_CAMERA_ID_0].mode			= IMX577_MODE,
	.sensor[E_CAMERA_ID_0].mode_num		= E_SENSOR_IMX577_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX577_MODE_Full_4056x3040_RAW12_40fps,
	
	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_0].F_number		= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit		= 256, // ob is overwritten by IQ bin
	
	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby			= Sensor_IMX577_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX577_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX577_Mode,
	.sensor[E_CAMERA_ID_0].Func_Pdaf_Mode		= NULL, // Not support in SDK
	.sensor[E_CAMERA_ID_0].Func_Multi			= Sensor_IMX577_Multi,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Stream			= Sensor_IMX577_Stream,

	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX577_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp			= Sensor_IMX577_Set_Exp,
	
	.sensor[E_CAMERA_ID_0].Func_DOL_Calc_Exp	= Sensor_IMX577_DOL_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_DOL_Set_Exp		= Sensor_IMX577_DOL_Set_Exp,
	
	.sensor[E_CAMERA_ID_0].Func_Get_PdInfo		= NULL, // Not support in SDK
	
	// sensor 1
	.sensor[E_CAMERA_ID_1].basic		= &IMX577_BASIC,
	.sensor[E_CAMERA_ID_1].mode			= IMX577_MODE,
	.sensor[E_CAMERA_ID_1].mode_num		= E_SENSOR_IMX577_MODE_MAX,
	.sensor[E_CAMERA_ID_1].mode_full	= E_SENSOR_IMX577_MODE_Full_4056x3040_RAW12_40fps,
	
	.sensor[E_CAMERA_ID_1].port_xclr	= E_DD_TOP_GPIO_P81,
#if defined(ETHER_EN)
    // PG3/4/5 are used for Ethernet, cannot configure here, using sensor 0 PG0/1/2 since sensor cannot be used when ETHER_EN is enabled
	.sensor[E_CAMERA_ID_1].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_1].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_1].port_inck	= E_DD_TOP_GPIO_PG2,
#else
    .sensor[E_CAMERA_ID_1].port_xhs		= E_DD_TOP_GPIO_PG3,
	.sensor[E_CAMERA_ID_1].port_xvs		= E_DD_TOP_GPIO_PG4,
	.sensor[E_CAMERA_ID_1].port_inck	= E_DD_TOP_GPIO_PG5,
#endif
	.sensor[E_CAMERA_ID_1].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_1].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_1].i2c_ch		= FJ_PERI_CH_1,
	.sensor[E_CAMERA_ID_1].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_1].F_number		= 1.8,
	.sensor[E_CAMERA_ID_1].ob_12bit		= 256, // ob is overwritten by IQ bin
	
	.sensor[E_CAMERA_ID_1].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_1].Func_Standby			= Sensor_IMX577_Standby,
	.sensor[E_CAMERA_ID_1].Func_CancelStandby	= Sensor_IMX577_CancelStandby,
	.sensor[E_CAMERA_ID_1].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_1].Func_Mode			= Sensor_IMX577_Mode,
	.sensor[E_CAMERA_ID_1].Func_Multi			= Sensor_IMX577_Multi,
	.sensor[E_CAMERA_ID_1].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_1].Func_Stream			= Sensor_IMX577_Stream,

	.sensor[E_CAMERA_ID_1].Func_Calc_Exp		= Sensor_IMX577_Calc_Exp,
	.sensor[E_CAMERA_ID_1].Func_Set_Exp			= Sensor_IMX577_Set_Exp,
	
	// sensor 2
	.sensor[E_CAMERA_ID_2].basic		= &IMX577_BASIC,
	.sensor[E_CAMERA_ID_2].mode			= IMX577_MODE,
	.sensor[E_CAMERA_ID_2].mode_num		= E_SENSOR_IMX577_MODE_MAX,
	.sensor[E_CAMERA_ID_2].mode_full	= E_SENSOR_IMX577_MODE_Full_4056x3040_RAW12_40fps,
	
	.sensor[E_CAMERA_ID_2].port_xclr	= E_DD_TOP_GPIO_P82,
	.sensor[E_CAMERA_ID_2].port_xhs		= E_DD_TOP_GPIO_PG6,
	.sensor[E_CAMERA_ID_2].port_xvs		= E_DD_TOP_GPIO_PG7,
	.sensor[E_CAMERA_ID_2].port_inck	= E_DD_TOP_GPIO_PH0,
	.sensor[E_CAMERA_ID_2].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_2].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_2].i2c_ch		= FJ_PERI_CH_1,
	.sensor[E_CAMERA_ID_2].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT2,
	.sensor[E_CAMERA_ID_2].F_number		= 1.8,
	.sensor[E_CAMERA_ID_2].ob_12bit		= 256, // ob is overwritten by IQ bin
	
	.sensor[E_CAMERA_ID_2].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_2].Func_Standby			= Sensor_IMX577_Standby,
	.sensor[E_CAMERA_ID_2].Func_CancelStandby	= Sensor_IMX577_CancelStandby,
	.sensor[E_CAMERA_ID_2].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_2].Func_Mode			= Sensor_IMX577_Mode,
	.sensor[E_CAMERA_ID_2].Func_Multi			= Sensor_IMX577_Multi,
	.sensor[E_CAMERA_ID_2].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_2].Func_Stream			= Sensor_IMX577_Stream,
	
	.sensor[E_CAMERA_ID_2].Func_Calc_Exp		= Sensor_IMX577_Calc_Exp,
	.sensor[E_CAMERA_ID_2].Func_Set_Exp			= Sensor_IMX577_Set_Exp,
	
	// sensor 3
	.sensor[E_CAMERA_ID_3].basic		= &IMX577_BASIC,
	.sensor[E_CAMERA_ID_3].mode			= IMX577_MODE,
	.sensor[E_CAMERA_ID_3].mode_num		= E_SENSOR_IMX577_MODE_MAX,
	.sensor[E_CAMERA_ID_3].mode_full	= E_SENSOR_IMX577_MODE_Full_4056x3040_RAW12_40fps,
	
	.sensor[E_CAMERA_ID_3].port_xclr	= E_DD_TOP_GPIO_P83,
	.sensor[E_CAMERA_ID_3].port_xhs		= E_DD_TOP_GPIO_PH1,
	.sensor[E_CAMERA_ID_3].port_xvs		= E_DD_TOP_GPIO_PH2,
	.sensor[E_CAMERA_ID_3].port_inck	= E_DD_TOP_GPIO_PH3,
	.sensor[E_CAMERA_ID_3].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_3].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_3].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_3].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT2,
	.sensor[E_CAMERA_ID_3].F_number		= 1.8,
	.sensor[E_CAMERA_ID_3].ob_12bit		= 256, // ob is overwritten by IQ bin

	.sensor[E_CAMERA_ID_3].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_3].Func_Standby			= Sensor_IMX577_Standby,
	.sensor[E_CAMERA_ID_3].Func_CancelStandby	= Sensor_IMX577_CancelStandby,
	.sensor[E_CAMERA_ID_3].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_3].Func_Mode			= Sensor_IMX577_Mode,
	.sensor[E_CAMERA_ID_3].Func_Multi			= Sensor_IMX577_Multi,

	.sensor[E_CAMERA_ID_3].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_3].Func_Stream			= Sensor_IMX577_Stream,

	.sensor[E_CAMERA_ID_3].Func_Calc_Exp		= Sensor_IMX577_Calc_Exp,
	.sensor[E_CAMERA_ID_3].Func_Set_Exp			= Sensor_IMX577_Set_Exp,

#elif USE_SENSOR == SENSOR_IMX277
	
	.sensor_num				= 1,
	.Func_Clock_GPIO_Init	= Sensor_IMX277_Clock_Gpio_Init,
	.sensor_str				= (char *)"IMX277",
	.start_frame_num		= 0,

	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX277_BASIC,
	.sensor[E_CAMERA_ID_0].mode			= IMX277_MODE,
	.sensor[E_CAMERA_ID_0].mode_num		= E_SENSOR_IMX277_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX277_MODE_H4_V3_NO_1,
	
	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_0].F_number		= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit		= 256, // ob is overwritten by IQ bin
	
	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby			= Sensor_IMX277_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX277_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX277_Mode,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= Sensor_IMX277_SgVHD,
	.sensor[E_CAMERA_ID_0].Func_Stream			= NULL,

	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX277_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp			= Sensor_IMX277_Set_Exp,

#elif USE_SENSOR == SENSOR_IMX383

	.sensor_num				= 1,
	.Func_Clock_GPIO_Init	= Sensor_IMX383_Clock_Gpio_Init,
	.sensor_str				= (char *)"IMX383",
	.start_frame_num		= 0,
	
	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX383_BASIC,
	.sensor[E_CAMERA_ID_0].mode			= IMX383_MODE,
	.sensor[E_CAMERA_ID_0].mode_num		= E_SENSOR_IMX383_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX383_MODE_H4_V3_NO_1,

	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= TRUE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_0].F_number		= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit		= 256, // ob is overwritten by IQ bin
	
	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby			= Sensor_IMX383_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX383_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX383_Mode,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= Sensor_IMX383_SgVHD,
	.sensor[E_CAMERA_ID_0].Func_Stream			= NULL,

	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX383_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp			= Sensor_IMX383_Set_Exp,

#elif USE_SENSOR == SENSOR_IMX533
		
	.sensor_num 			= 1,
	.Func_Clock_GPIO_Init	= Sensor_IMX533_Clock_Gpio_Init,
	.sensor_str				= (char *)"IMX533",
	.start_frame_num		= 0,
	
	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX533_BASIC,
	.sensor[E_CAMERA_ID_0].mode 		= IMX533_MODE,
	.sensor[E_CAMERA_ID_0].mode_num 	= E_SENSOR_IMX533_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX533_MODE_H1_V1_NO_1,
		
	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs 	= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs 	= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= TRUE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_0].F_number 	= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit 	= 256, // ob is overwritten by IQ bin
		
	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby 		= Sensor_IMX533_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX533_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX533_Mode,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= Sensor_IMX533_SgVHD,
	.sensor[E_CAMERA_ID_0].Func_Stream			= NULL,
	
	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX533_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp 		= Sensor_IMX533_Set_Exp,
	
#elif USE_SENSOR == SENSOR_IMX183
		
	.sensor_num 			= 1,
	.Func_Clock_GPIO_Init	= Sensor_IMX183_Clock_Gpio_Init,
	.sensor_str				= (char *)"IMX183",
	.start_frame_num		= 1,
	
	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX183_BASIC,
	.sensor[E_CAMERA_ID_0].mode 		= IMX183_MODE,
	.sensor[E_CAMERA_ID_0].mode_num 	= E_SENSOR_IMX183_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX183_MODE_TYPE_1_NO_1,
		
	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs 	= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs 	= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= TRUE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_0].F_number 	= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit 	= 256, // ob is overwritten by IQ bin
		
	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby 		= Sensor_IMX183_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX183_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= Sensor_IMX183_GlobalShutter,
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX183_Mode,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= Sensor_IMX183_SgVHD,
	.sensor[E_CAMERA_ID_0].Func_Stream			= NULL,
	
	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX183_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp 		= Sensor_IMX183_Set_Exp,
	
#elif USE_SENSOR == SENSOR_IMX363
	
	.sensor_num				= 1,
	.Func_Clock_GPIO_Init	= Sensor_IMX363_Clock_Gpio_Init,
	.sensor_str				= (char *)"IMX363",
	.start_frame_num		= 0,

	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX363_BASIC,
	.sensor[E_CAMERA_ID_0].mode			= IMX363_MODE,
	.sensor[E_CAMERA_ID_0].mode_num		= E_SENSOR_IMX363_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX363_MODE4_Fullp30_4032x3024_pd_4032x756,
	
	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT2,
	.sensor[E_CAMERA_ID_0].F_number		= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit		= 256, // ob is overwritten by IQ bin
	
	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby			= Sensor_IMX363_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX363_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX363_Mode,
	.sensor[E_CAMERA_ID_0].Func_Pdaf_Mode		= Sensor_IMX363_Pdaf_Mode,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Stream			= Sensor_IMX363_Stream,

	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX363_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp			= Sensor_IMX363_Set_Exp,

#elif USE_SENSOR == SENSOR_IMX290

	.sensor_num				= 1,
	.Func_Clock_GPIO_Init	= Sensor_IMX290_Clock_Gpio_Init,
	.sensor_str				= (char *)"IMX290",
	.start_frame_num		= 0,

	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX290_BASIC,
	.sensor[E_CAMERA_ID_0].mode			= IMX290_MODE,
	.sensor[E_CAMERA_ID_0].mode_num		= E_SENSOR_IMX290_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX290_MODE_FUll_1920x1080_RAW12_60FPS,

	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_0].F_number		= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit		= 256, // ob is overwritten by IQ bin

	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby			= Sensor_IMX290_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX290_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX290_Mode,
	.sensor[E_CAMERA_ID_0].Func_Multi			= NULL,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Stream			= Sensor_IMX290_Stream,

	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX290_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp			= Sensor_IMX290_Set_Exp,

	.sensor[E_CAMERA_ID_0].Func_DOL_Calc_Exp	= NULL,
	.sensor[E_CAMERA_ID_0].Func_DOL_Set_Exp		= NULL,
	
#elif USE_SENSOR == SENSOR_OS08A10

	.sensor_num				= 1,
	.Func_Clock_GPIO_Init	= Sensor_OS08A10_Clock_Gpio_Init,
	.sensor_str				= (char *)"Ominivision OS08A10",
	.start_frame_num		= 0,

	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &OS08A10_BASIC,
	.sensor[E_CAMERA_ID_0].mode			= OS08A10_MODE,
	.sensor[E_CAMERA_ID_0].mode_num		= E_SENSOR_OS08A10_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_OS08A10_MODE_FUll_3840x2160_RAW12_30FPS,

	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_OV_SENSOR_SLAVE7BIT, // If SID pin is low. check spec. p.34
	.sensor[E_CAMERA_ID_0].F_number		= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit		= 256, // ob is overwritten by IQ bin

	.sensor[E_CAMERA_ID_0].Func_Init                = NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby             = Sensor_OS08A10_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby       = Sensor_OS08A10_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter       = NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode                = Sensor_OS08A10_Mode,
	.sensor[E_CAMERA_ID_0].Func_Multi               = NULL,
	.sensor[E_CAMERA_ID_0].Func_SgVHD               = NULL,
	.sensor[E_CAMERA_ID_0].Func_Stream              = Sensor_OS08A10_Stream,

	.sensor[E_CAMERA_ID_0].Func_Calc_Exp            = Sensor_OS08A10_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp             = Sensor_OS08A10_Set_Exp,

	.sensor[E_CAMERA_ID_0].Func_DOL_Calc_Exp	= NULL,
	.sensor[E_CAMERA_ID_0].Func_DOL_Set_Exp		= NULL,

#elif USE_SENSOR == SENSOR_AR0430

	.sensor_num				= 1,
	.Func_Clock_GPIO_Init	= Sensor_AR0430_Clock_Gpio_Init,
	.sensor_str				= (char *)"ONsemi AR0430",
	.start_frame_num		= 0,

	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &AR0430_BASIC,
	.sensor[E_CAMERA_ID_0].mode			= AR0430_MODE,
	.sensor[E_CAMERA_ID_0].mode_num		= E_SENSOR_AR0430_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_AR0430_MODE_FUll_2316x1746_RAW10_30FPS,

	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80, //reset opin to XSHUTDOWN
	.sensor[E_CAMERA_ID_0].port_xhs		= E_DD_TOP_GPIO_PORT_MAX, //E_DD_TOP_GPIO_PG0, mipi no need xhs
	.sensor[E_CAMERA_ID_0].port_xvs		= E_DD_TOP_GPIO_PORT_MAX, //E_DD_TOP_GPIO_PG1, mipi no need vhs
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_ONSEMI_SENSOR_SLAVE7BIT, //Slave Address. DS. p.9
	.sensor[E_CAMERA_ID_0].F_number		= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit		= 256, // ob is overwritten by IQ bin

	.sensor[E_CAMERA_ID_0].Func_Init                = Sensor_AR0430_Init,
	.sensor[E_CAMERA_ID_0].Func_Standby             = Sensor_AR0430_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby       = NULL,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter       = NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode                = Sensor_AR0430_Mode,
	.sensor[E_CAMERA_ID_0].Func_Multi               = NULL,
	.sensor[E_CAMERA_ID_0].Func_SgVHD               = NULL,
	.sensor[E_CAMERA_ID_0].Func_Stream              = Sensor_AR0430_Stream,

	.sensor[E_CAMERA_ID_0].Func_Calc_Exp            = Sensor_AR0430_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp             = Sensor_AR0430_Set_Exp,

	.sensor[E_CAMERA_ID_0].Func_DOL_Calc_Exp	= NULL,
	.sensor[E_CAMERA_ID_0].Func_DOL_Set_Exp		= NULL,
	
#elif USE_SENSOR == SENSOR_IMX377
		
		.sensor_num 			= 1,
		.Func_Clock_GPIO_Init	= Sensor_IMX377_Clock_Gpio_Init,
		.sensor_str 			= (char *)"IMX377",
		.start_frame_num		= 0,
	
		// sensor 0
		.sensor[E_CAMERA_ID_0].basic		= &IMX377_BASIC,
		.sensor[E_CAMERA_ID_0].mode 		= IMX377_MODE,
		.sensor[E_CAMERA_ID_0].mode_num 	= E_SENSOR_IMX377_MODE_MAX,
		.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX377_9M_MODE2_2048x1080,
		
		.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
		.sensor[E_CAMERA_ID_0].port_xhs 	= E_DD_TOP_GPIO_PG0,
		.sensor[E_CAMERA_ID_0].port_xvs 	= E_DD_TOP_GPIO_PG1,
		.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
		.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
		.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
		.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
		.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,	// slave address = 34/35 >> 0x1A
		.sensor[E_CAMERA_ID_0].F_number 	= 1.8,
		.sensor[E_CAMERA_ID_0].ob_12bit 	= 256, // ob is overwritten by IQ bin
		
		.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
		.sensor[E_CAMERA_ID_0].Func_Standby 		= Sensor_IMX377_Standby,
		.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX377_CancelStandby,
		.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
		.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX377_Mode,
		.sensor[E_CAMERA_ID_0].Func_Pdaf_Mode		= NULL,
		.sensor[E_CAMERA_ID_0].Func_SgVHD			= NULL,
		.sensor[E_CAMERA_ID_0].Func_Stream			= Sensor_IMX377_Stream,
	
		.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX377_Calc_Exp,
		.sensor[E_CAMERA_ID_0].Func_Set_Exp 		= Sensor_IMX377_Set_Exp,

#elif USE_SENSOR == SENSOR_OV12895
		
		.sensor_num 			= 1,
		.Func_Clock_GPIO_Init	= Sensor_OV12895_Clock_Gpio_Init,
		.sensor_str 			= (char *)"OV12895",
		.start_frame_num		= 0,
	
		// sensor 0
		.sensor[E_CAMERA_ID_0].basic		= &OV12895_BASIC,
		.sensor[E_CAMERA_ID_0].mode 		= OV12895_MODE,
		.sensor[E_CAMERA_ID_0].mode_num 	= E_SENSOR_OV12895_MODE_MAX,
		.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_OV12895_MODE_FUll_3840x2160_RAW10_60FPS,
		
		.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
		.sensor[E_CAMERA_ID_0].port_xhs 	= E_DD_TOP_GPIO_PG0,
		.sensor[E_CAMERA_ID_0].port_xvs 	= E_DD_TOP_GPIO_PG1,
		.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
		.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
		.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
		.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
		.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_OV_SENSOR_SLAVE7BIT,
		.sensor[E_CAMERA_ID_0].F_number 	= 1.8,
		.sensor[E_CAMERA_ID_0].ob_12bit 	= 256, // ob is overwritten by IQ bin
		
		.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
		.sensor[E_CAMERA_ID_0].Func_Standby 		= Sensor_OV12895_Standby,
		.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_OV12895_CancelStandby,
		.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
		.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_OV12895_Mode,
		.sensor[E_CAMERA_ID_0].Func_Pdaf_Mode		= NULL,
		.sensor[E_CAMERA_ID_0].Func_SgVHD			= NULL,
		.sensor[E_CAMERA_ID_0].Func_Stream			= Sensor_OV12895_Stream,
	
		.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_OV12895_Calc_Exp,
		.sensor[E_CAMERA_ID_0].Func_Set_Exp 		= Sensor_OV12895_Set_Exp,

#elif USE_SENSOR == SENSOR_IMX477_IMX183
				
	#if 1
	.sensor_num 			= 2,
	.Func_Clock_GPIO_Init	= Sensor_IMX477_IMX183_Clock_Gpio_Init,
	.sensor_str 			= (char *)"IMX477+IMX183",
	.start_frame_num		= 1,
			
	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX477_IMX183_0_BASIC,
	.sensor[E_CAMERA_ID_0].mode 		= IMX477_MODE,
	.sensor[E_CAMERA_ID_0].mode_num 	= E_SENSOR_IMX477_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX477_MODE_Full_4056x3040_RAW12_40fps,
			
	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P81,
	.sensor[E_CAMERA_ID_0].port_xhs 	= E_DD_TOP_GPIO_PG3,
	.sensor[E_CAMERA_ID_0].port_xvs 	= E_DD_TOP_GPIO_PG4,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG5,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_0].F_number 	= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit 	= 256, // ob is overwritten by IQ bin
			
	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby 		= Sensor_IMX477_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX477_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX477_Mode,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Stream			= Sensor_IMX477_Stream,
			
	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= NULL,//Sensor_IMX477_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp 		= NULL,//Sensor_IMX477_Set_Exp,
			
	// sensor 1
	.sensor[E_CAMERA_ID_1].basic		= &IMX477_IMX183_1_BASIC,
	.sensor[E_CAMERA_ID_1].mode 		= IMX183_MODE,
	.sensor[E_CAMERA_ID_1].mode_num 	= E_SENSOR_IMX183_MODE_MAX,
	.sensor[E_CAMERA_ID_1].mode_full	= E_SENSOR_IMX183_MODE_TYPE_1_NO_1,
				
	.sensor[E_CAMERA_ID_1].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_1].port_xhs 	= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_1].port_xvs 	= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_1].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_1].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_1].is_spics_gpio= TRUE,
	.sensor[E_CAMERA_ID_1].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_1].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_1].F_number 	= 1.8,
	.sensor[E_CAMERA_ID_1].ob_12bit 	= 256, // ob is overwritten by IQ bin
				
	.sensor[E_CAMERA_ID_1].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_1].Func_Standby 		= Sensor_IMX183_Standby,
	.sensor[E_CAMERA_ID_1].Func_CancelStandby	= Sensor_IMX183_CancelStandby,
	.sensor[E_CAMERA_ID_1].Func_GlobalShutter	= Sensor_IMX183_GlobalShutter,
	.sensor[E_CAMERA_ID_1].Func_Mode			= Sensor_IMX183_Mode,
	.sensor[E_CAMERA_ID_1].Func_SgVHD			= Sensor_IMX183_SgVHD,
	.sensor[E_CAMERA_ID_1].Func_Stream			= NULL,
			
	.sensor[E_CAMERA_ID_1].Func_Calc_Exp		= Sensor_IMX183_Calc_Exp,
	.sensor[E_CAMERA_ID_1].Func_Set_Exp 		= Sensor_IMX183_Set_Exp,
	#else
	.sensor_num 			= 1,
	.Func_Clock_GPIO_Init	= Sensor_IMX477_IMX183_Clock_Gpio_Init,
	.sensor_str 			= (char *)"IMX477+IMX183",
	.start_frame_num		= 1,
			
	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX477_IMX183_1_BASIC,
	.sensor[E_CAMERA_ID_0].mode 		= IMX183_MODE,
	.sensor[E_CAMERA_ID_0].mode_num 	= E_SENSOR_IMX183_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX183_MODE_TYPE_1_NO_1,
				
	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs 	= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs 	= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= TRUE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_0].F_number 	= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit 	= 256, // ob is overwritten by IQ bin
				
	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby 		= Sensor_IMX183_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX183_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= Sensor_IMX183_GlobalShutter,
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX183_Mode,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= Sensor_IMX183_SgVHD,
	.sensor[E_CAMERA_ID_0].Func_Stream			= NULL,
			
	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX183_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp 		= Sensor_IMX183_Set_Exp,
	#endif
#elif USE_SENSOR == SENSOR_IMX269
	.sensor_num				= 1,
	.Func_Clock_GPIO_Init	= Sensor_IMX269_Clock_Gpio_Init,
	.sensor_str				= (char *)"IMX269",
	.start_frame_num		= 1,
	
	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX269_BASIC,
	.sensor[E_CAMERA_ID_0].mode			= IMX269_MODE,
	.sensor[E_CAMERA_ID_0].mode_num		= E_SENSOR_IMX269_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX269_MODE1_5280x3956_RAW12_18_92FPS,

	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= 0,
	.sensor[E_CAMERA_ID_0].F_number		= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit		= 256, // ob is overwritten by IQ bin

	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby			= Sensor_IMX269_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX269_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX269_Mode,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= Sensor_IMX269_SgVHD,
	.sensor[E_CAMERA_ID_0].Func_Stream			= NULL,

	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX269_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp			= Sensor_IMX269_Set_Exp,


#elif USE_SENSOR == SENSOR_IMX571

	.sensor_num				= 1,
	.Func_Clock_GPIO_Init	= Sensor_IMX571_Clock_Gpio_Init,
	.sensor_str				= (char *)"IMX571",
	.start_frame_num		= 0,
	
	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &IMX571_BASIC,
	.sensor[E_CAMERA_ID_0].mode			= IMX571_MODE,
	.sensor[E_CAMERA_ID_0].mode_num		= E_SENSOR_IMX571_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_IMX571_MODE0C_V1H1,

	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= TRUE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_0].F_number		= 5.6,
	.sensor[E_CAMERA_ID_0].ob_12bit		= 256, // ob is overwritten by IQ bin
	
	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby			= Sensor_IMX571_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= Sensor_IMX571_CancelStandby,
	.sensor[E_CAMERA_ID_0].Func_Callback        = Sensor_IMX571_CALLBACK,
#ifdef IMX571_GLOBAL_SHUTTER
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= Sensor_IMX571_GlobalShutter,
#else
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
#endif
	.sensor[E_CAMERA_ID_0].Func_Mode			= Sensor_IMX571_Mode,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= Sensor_IMX571_SgVHD,
	.sensor[E_CAMERA_ID_0].Func_Stream			= NULL,

	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_IMX571_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp			= Sensor_IMX571_Set_Exp,

	


#elif USE_SENSOR == SENSOR_PSEUDO
	.sensor_num				= 1,
	.Func_Clock_GPIO_Init	= Sensor_PSEUDO_Clock_Gpio_Init,
	.sensor_str				= (char *)"PSEUDO",
	.start_frame_num		= 10,

	// sensor 0
	.sensor[E_CAMERA_ID_0].basic		= &PSEUDO_BASIC,
	.sensor[E_CAMERA_ID_0].mode			= PSEUDO_MODE,
	.sensor[E_CAMERA_ID_0].mode_num		= E_SENSOR_PSEUDO_MODE_MAX,
	.sensor[E_CAMERA_ID_0].mode_full	= E_SENSOR_PSEUDO_MODE_Full_3840x2160_RAW10_30fps,
	
	.sensor[E_CAMERA_ID_0].port_xclr	= E_DD_TOP_GPIO_P80,
	.sensor[E_CAMERA_ID_0].port_xhs		= E_DD_TOP_GPIO_PG0,
	.sensor[E_CAMERA_ID_0].port_xvs		= E_DD_TOP_GPIO_PG1,
	.sensor[E_CAMERA_ID_0].port_inck	= E_DD_TOP_GPIO_PG2,
	.sensor[E_CAMERA_ID_0].spi_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].is_spics_gpio= FALSE,
	.sensor[E_CAMERA_ID_0].i2c_ch		= FJ_PERI_CH_0,
	.sensor[E_CAMERA_ID_0].i2c_slave7bit= D_IMX_SENSOR_SLAVE7BIT,
	.sensor[E_CAMERA_ID_0].F_number		= 1.8,
	.sensor[E_CAMERA_ID_0].ob_12bit		= 256, // ob is overwritten by IQ bin
	
	.sensor[E_CAMERA_ID_0].Func_Init			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Standby			= Sensor_PSEUDO_Standby,
	.sensor[E_CAMERA_ID_0].Func_CancelStandby	= NULL,
	.sensor[E_CAMERA_ID_0].Func_GlobalShutter	= NULL,
	.sensor[E_CAMERA_ID_0].Func_Mode			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Pdaf_Mode		= NULL,
	.sensor[E_CAMERA_ID_0].Func_SgVHD			= NULL,
	.sensor[E_CAMERA_ID_0].Func_Stream			= Sensor_PSEUDO_Stream,

	.sensor[E_CAMERA_ID_0].Func_Calc_Exp		= Sensor_PSEUDO_Calc_Exp,
	.sensor[E_CAMERA_ID_0].Func_Set_Exp			= NULL,

#endif

#if USE_GYRO == GYRO_ICM20948
	.gyro.buf_size				= &gGyroCtrl_BufSize[0],
	.gyro.ctrl					= &gGyroCtrl_Data,
	.gyro.Sampling_Hz_Gyro		= 1125.0,
	.gyro.Sampling_Hz_Accel		= 1125.0,
	.gyro.Func_Init				= Gyro_Init, //Gyro_Init_DMP,
	.gyro.Func_Standby			= Gyro_Sleep, //Gyro_Sleep_DMP,
	.gyro.Func_Calc_Bias		= Gyro_Calc_Bias,
	.gyro.Func_CancelStandby	= Gyro_Resume, //Gyro_Resume_DMP,
	.gyro.Func_GetData			= Gyro_Get_Data, //Gyro_Get_Data_DMP,
	.gyro.Func_StartRead		= GyroIF_start_read,
	.gyro.Func_StopRead			= GyroIF_stop_read,
	.gyro.Func_GetReadCount		= GyroIF_get_read_count,
	.gyro.Func_Accel2Gravity	= Gyro_AccelData2Gravity,
	.gyro.Func_Gyro2Gravity		= Gyro_GyroData2Gravity,
	.gyro.Func_DebugDump		= Gyro_Dump,
#elif USE_GYRO == GYRO_MPU9250
	.gyro.buf_size				= &gGyroCtrl_BufSize[0],
	.gyro.ctrl					= &gGyroCtrl_Data,
	.gyro.Sampling_Hz_Gyro		= 8000.0,
	.gyro.Sampling_Hz_Accel		= 1000.0,
	.gyro.Func_Init 			= Gyro_Init,
	.gyro.Func_Standby			= Gyro_Sleep,
	.gyro.Func_Calc_Bias		= Gyro_Calc_Bias,
	.gyro.Func_CancelStandby	= Gyro_Resume,
	.gyro.Func_GetData			= Gyro_Get_Data,
	.gyro.Func_StartRead		= GyroIF_start_read,
	.gyro.Func_StopRead 		= GyroIF_stop_read,
	.gyro.Func_GetReadCount 	= GyroIF_get_read_count,
	.gyro.Func_Accel2Gravity	= Gyro_AccelData2Gravity,
	.gyro.Func_Gyro2Gravity 	= Gyro_GyroData2Gravity,
	.gyro.Func_DebugDump		= Gyro_Dump,
#elif USE_GYRO == GYRO_ICM20602
	.gyro.buf_size				= &gGyroCtrl_BufSize[0],
	.gyro.ctrl					= &gGyroCtrl_Data,
	.gyro.Sampling_Hz_Gyro		= 8000.0,
	.gyro.Sampling_Hz_Accel		= 1000.0,
	.gyro.Func_Init 			= Gyro_Init,
	.gyro.Func_Standby			= Gyro_Sleep,
	.gyro.Func_Calc_Bias		= Gyro_Calc_Bias,
	.gyro.Func_CancelStandby	= Gyro_Resume,
	.gyro.Func_GetData			= Gyro_Get_Data,
	.gyro.Func_StartRead		= GyroIF_start_read,
	.gyro.Func_StopRead 		= GyroIF_stop_read,
	.gyro.Func_GetReadCount 	= GyroIF_get_read_count,
	.gyro.Func_Accel2Gravity	= Gyro_AccelData2Gravity,
	.gyro.Func_Gyro2Gravity 	= Gyro_GyroData2Gravity,
	.gyro.Func_DebugDump		= Gyro_Dump,
#endif	

	
#if USE_FOCUS == FOCUS_ZC525
	.focus[E_CAMERA_ID_0].basic				= &ZC525_BASIC,
	.focus[E_CAMERA_ID_0].dof				= Z525_DOF,
	.focus[E_CAMERA_ID_0].Func_VCM_Init		= &Focus_VCM_ZC525_Init,
	.focus[E_CAMERA_ID_0].Func_VCM_SetDAC	= &Focus_VCM_ZC525_SetDAC,
	.focus[E_CAMERA_ID_0].Func_VCM_GetDAC	= &Focus_VCM_ZC525_GetDAC,
	.focus[E_CAMERA_ID_0].Func_VCM_Move		= &Focus_VCM_ZC525_Move,
	.focus[E_CAMERA_ID_0].Func_VCM_CalcDAC	= &Focus_VCM_ZC525_CalcDAC,
#endif	

#if USE_FOCUS == FOCUS_M43
	.focus[E_CAMERA_ID_0].basic				= &M43_BASIC,
	.focus[E_CAMERA_ID_0].dof				= M43_DOF,
	.focus[E_CAMERA_ID_0].Func_VCM_Init		= &Focus_VCM_M43_Init,
	.focus[E_CAMERA_ID_0].Func_VCM_SetDAC	= &Focus_VCM_M43_SetDAC,
	.focus[E_CAMERA_ID_0].Func_VCM_GetDAC	= &Focus_VCM_M43_GetDAC,
	.focus[E_CAMERA_ID_0].Func_VCM_Move		= &Focus_VCM_M43_Move,
	.focus[E_CAMERA_ID_0].Func_VCM_CalcDAC	= &Focus_VCM_M43_CalcDAC,
#endif

#if USE_FOCUS == FOCUS_F103T
	.focus[E_CAMERA_ID_0].basic				= &F103T_BASIC,
	.focus[E_CAMERA_ID_0].dof				= F103T_DOF,
	.focus[E_CAMERA_ID_0].Func_VCM_Init		= &Focus_VCM_F103T_Init,
	.focus[E_CAMERA_ID_0].Func_VCM_SetDAC	= &Focus_VCM_F103T_SetDAC,
	.focus[E_CAMERA_ID_0].Func_VCM_GetDAC	= &Focus_VCM_F103T_GetDAC,
	.focus[E_CAMERA_ID_0].Func_VCM_Move		= &Focus_VCM_F103T_Move,
	.focus[E_CAMERA_ID_0].Func_VCM_CalcDAC	= &Focus_VCM_F103T_CalcDAC,
	.focus[E_CAMERA_ID_0].Func_VCM_Get_Cur_Focus_Value = & Focus_VCM_F103T_Get_Cur_Focus_Value,
#endif
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

char* Camera_Get_SensorType(void)
{
	return CAMERA.sensor_str;
}

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

