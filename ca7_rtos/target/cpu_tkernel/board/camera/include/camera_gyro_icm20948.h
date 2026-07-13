/**
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 * @file		: camera_gyro_icm20948.h
 * @brief 		: Gyro driver for ICM-20948
 * @note		: None
 * @attention	: None
 */

#ifndef _CAMERA_GYRO_ICM20948_H_
#define _CAMERA_GYRO_ICM20948_H_

#include "ddim_typedef.h"
#include "fj_still.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_GYRO_SPI_READ_OP			( 1 << 7 )

#define DLPFCFG(v)					((v) << 3)

// for DMP : start
// flag pattern
#define FLG_GYRO_DMP_INT		(0x00000001)
#define FLG_GYRO_DMP_STOP_REQ	(0x00000002)
// for DMP : end

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/**
 * Definition of Register Addr
 */
typedef enum {
	/* User Bank 0 */
	WHO_AM_I			= 0x00,
	USER_CTRL			= 0x03,
	LP_CONFIG			= 0x05,
	PWR_MGMT_1			= 0x06,
	PWR_MGMT_2			= 0x07,
	INT_PIN_CFG 		= 0x0F,//0x10,
	INT_ENABLE			= 0x10,
	INT_ENABLE_1		= 0x11,
	ACCEL_OUT			= 0x2D,
	GYRO_OUT			= 0x33,
	TEMP_OUT			= 0x39,
	FIFO_EN_2			= 0x67,
	FIFO_MODE			= 0x69,
	FIFO_CFG			= 0x76,
	/* User Bank 2 */
	GYRO_SMPLRT_DIV		= 0x00,
	GYRO_CONFIG_1		= 0x01,
	ACCEL_SMPLRT_DIV_1	= 0x10,
	ACCEL_SMPLRT_DIV_2	= 0x11,
	ACCEL_INTEL_CTRL	= 0x12,
	ACCEL_WOM_THR		= 0x13,
	ACCEL_CONFIG		= 0x14,
	FSYNC_CONFIG		= 0x52,
	/* User Bank Select */
	REG_BNK_SEL			= 0x7F,
} E_GYRO_REG_ADDR;

/**
 * Definition of Register Data
 */
typedef enum {
	// USER_CTRL register
	I2C_IF_DIS			= (1 << 4),
	FIFO_EN				= (1 << 6),
	// LP_CONFIG register
	GYRO_CYCLE			= (1 << 4),
	ACCEL_CYCLE 		= (1 << 5),
	// INT_PIN_CFG register
	FSYNC_INT_MODE_EN	= (1 << 2), // FSYNC causes the interrupt
	ACTL_FSYNC_LOW		= (1 << 3), // FSYNC is active low
	ACTL_FSYNC_HIGH		= (0 << 3), // FSYNC is active high
	INT_ANYRD_2CLEAR	= (1 << 4), // any read clears interrupt status
	LATCH_INT_EN_LATCH	= (1 << 5), // level is held until interrupt status is cleared
	LATCH_INT_EN_PULSE	= (0 << 5), // interrupt is a 50us pulse
	OPEN				= (1 << 6), // INT pin is open drain
	OPEN_PUSH_PULL		= (0 << 6), // INT pin is push-pull
	ACTL_LOW 			= (1 << 7), // INT pin is active low
	ACTL_HIGH			= (0 << 7), // INT pin is active high
	// INT_ENABLE registers
	PLL_RDY_EN			= (1 << 2),
	WOM_INT_EN			= (1 << 3),
	REG_WOF_EN			= (1 << 7),
	// INT_ENABLE1 registers
	RAW_DATA_0_RDY_EN	= (1 << 0),
	// FIFO_EN_2 register
	TEMP_FIFO_EN		= (1 << 0),
	GYRO_X_FIFO_EN 		= (1 << 1),
	GYRO_Y_FIFO_EN 		= (1 << 2),
	GYRO_Z_FIFO_EN 		= (1 << 3),
	GYRO_FIFO_EN		= (GYRO_X_FIFO_EN | GYRO_Y_FIFO_EN | GYRO_Z_FIFO_EN),
	ACCEL_FIFO_EN		= (1 << 4),
	// FIFO_MODE register
	FIFO_MODE_STREAM	= (0 << 0),
	FIFO_MODE_SNAPSHOT	= (1 << 0),
	// FIFO_CFG register
	FIFO_CFG_INT_EN 	= (1 << 0),
	// PWR_MGMT_1 register
	CLKSEL1				= (1 << 0),		// CLKSEL=1 : Auto selects the best available clock source.
	SLEEP				= (1 << 6),
	DEVICE_RESET		= (1 << 7),
	// GYRO_CONFIG_1 register
	GYRO_FCHOICE		= (1 << 0),
	GYRO_FS_250			= (0 << 1),
	GYRO_FS_500			= (1 << 1),
	GYRO_FS_1000		= (2 << 1),
	GYRO_FS_2000		= (3 << 1),
	// ACCEL_INTEL_CTRL register
	ACCEL_INTEL_MODE_INT= (1 << 0),
	ACCEL_INTEL_EN		= (1 << 1),
	// ACCEL_CONFIG register
	ACCEL_FCHOICE		= (1 << 0),
	ACCEL_FS_2G			= (0 << 1),
	ACCEL_FS_4G			= (1 << 1),
	ACCEL_FS_8G			= (2 << 1),
	ACCEL_FS_16G		= (3 << 1),
	// FSYNC_CONFIG register
	FSYNC_TEMP			= (1 << 0),
	FSYNC_GYRO_X		= (2 << 0),
	FSYNC_GYRO_Y		= (3 << 0),
	FSYNC_GYRO_Z		= (4 << 0),
	FSYNC_ACCEL_X		= (5 << 0),
	FSYNC_ACCEL_Y		= (6 << 0),
	FSYNC_ACCEL_Z		= (7 << 0),
	// REG_BANK_SEL register
	USER_BANK_0			= (0 << 4),
	USER_BANK_1 		= (1 << 4),
	USER_BANK_2			= (2 << 4),
	USER_BANK_3			= (3 << 4),
} E_GYRO_REG_DATA;


/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Grobal Data															*/
/*----------------------------------------------------------------------*/
extern FJ_GYROIF_READ_CTRL gGyroCtrl_Data;
extern UINT32 gGyroCtrl_BufSize[E_CAMERA_GYRO_BUF_SIZE_MAX];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/** Write data */
VOID Gyro_Write( UCHAR addr, UCHAR data );

/** Write data */
VOID Gyro_Write_N( UCHAR addr, UCHAR* data, UCHAR write_bytes );

/** Read data */
VOID Gyro_Read( UCHAR addr, UCHAR* data );

/** Read data */
VOID Gyro_Read_N( UCHAR addr, UCHAR* data, UCHAR read_bytes );

/** Initialize Gyro Sensor */
E_CAMERA_ERR_CODE Gyro_Init( VOID );

/** Calc Gyro Biass */
E_CAMERA_ERR_CODE Gyro_Calc_Bias( ULONG time_ms, T_CAMERA_GYRO_MPU_DATA* bias );

/** Sleep Gyro Sensor */
E_CAMERA_ERR_CODE Gyro_Sleep( VOID );

E_CAMERA_ERR_CODE Gyro_Resume( VOID );

E_CAMERA_ERR_CODE Gyro_Get_Data(INT32 read_cnt, T_CAMERA_GYRO_MPU_DATA* data );

/** Convert Accel RAW data to gravity. */
FLOAT Gyro_AccelData2Gravity( const UCHAR data_h, const UCHAR data_l );

/** Convert Gyro RAW data to gravity. */
FLOAT Gyro_GyroData2Gravity( const UCHAR data_h, const UCHAR data_l );

/** Dump Accel, Templature and Gyro data register. */
VOID Gyro_Dump( VOID );

// for DMP : start
//INT32 inv_icm20948_get_sen_info(FLOAT* data, UINT8 sensor_type);
E_CAMERA_ERR_CODE Gyro_Init_DMP( VOID );
E_CAMERA_ERR_CODE Gyro_Sleep_DMP( VOID );
E_CAMERA_ERR_CODE Gyro_Resume_DMP( VOID );
E_CAMERA_ERR_CODE Gyro_Get_Data_DMP(INT32 read_cnt, T_CAMERA_GYRO_MPU_DATA* data );
VOID Gyro_Main_DMP( INT32 stacd );
// for DMP : end

//=========================GyroIF Start================================
extern E_CAMERA_ERR_CODE GyroIF_start_read( VOID );
extern E_CAMERA_ERR_CODE GyroIF_stop_read( VOID );
extern E_CAMERA_ERR_CODE GyroIF_get_read_count( UINT32* read_count );
//=========================GyroIF End================================

#endif	/* _CAMERA_GYRO_ICM20948_H_ */

