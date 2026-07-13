/**
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 * @file		: camera_gyro_mpu9250.h
 * @brief 		: Gyro driver for MPU9250
 * @note		: None
 * @attention	: None
 */

#ifndef _CAMERA_GYRO_MPU9250_H_
#define _CAMERA_GYRO_MPU9250_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_GYRO_SPI_READ_OP			( 1 << 7 )

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/**
 * Definition of Register Addr
 */
typedef enum {
	SELF_TEST_GYRO		= 0x00,
	SELF_TEST_ACCEL 	= 0x0D,
	G_OFFSET			= 0x13,
	SMPLRT_DIV			= 0x19,
	CONFIG				= 0x1A,
	GYRO_CONFIG 		= 0x1B,
	ACCEL_CONFIG		= 0x1C,
	ACCEL_CONFIG2		= 0x1D,
	LP_ACCEL_ODR		= 0x1E,
	WOM_THR 			= 0x1F,
	FIFO_EN 			= 0x23,
	I2C_MST_CTRL		= 0x24,
	I2C_MST_STATUS		= 0x36,
	INT_PIN_CFG 		= 0x37,
	INT_ENABLE			= 0x38,
	INT_STATUS			= 0x3A,
	ACCEL_OUT			= 0x3B,
	TEMP_OUT			= 0x41,
	GYRO_OUT			= 0x43,
	RESET				= 0x68,
	MOT_DETECT_CTRL 	= 0x69,
	USER_CTRL			= 0x6A,
	PWR_MGMT_1			= 0x6B,
	PWR_MGMT_2			= 0x6C,
	FIFO_COUNT			= 0x72,
	FIFO_RW 			= 0x74,
	WHO_AM_I			= 0x75,
	A_OFFSET			= 0x77,
} E_GYRO_REG_ADDR;

/**
 * Definition of Register Data
 */
typedef enum {
	// CONFIG register
	#define DLPF(v) 	((v) << 0)
	FSYNC_TEMP			= (1 << 3),
	FSYNC_GYRO_X		= (2 << 3),
	FSYNC_GYRO_Y		= (3 << 3),
	FSYNC_GYRO_Z		= (4 << 3),
	FSYNC_ACCEL_X		= (5 << 3),
	FSYNC_ACCEL_Y		= (6 << 3),
	FSYNC_ACCEL_Z		= (7 << 3),
	FIFO_DROP			= (1 << 6),
	FIFO_REPLACE		= (0 << 6),
	// GYRO_CONFIG register
	FCHOICE_x0			= (3 << 0),
	FCHOICE_01			= (2 << 0),
	FCHOICE_11			= (0 << 0),
	GYRO_FS_250 		= (0 << 3),
	GYRO_FS_500 		= (1 << 3),
	GYRO_FS_1000		= (2 << 3),
	GYRO_FS_2000		= (3 << 3),
	// ACCEL_CONFIG register
	ACCEL_FS_2G 		= (0 << 3),
	ACCEL_FS_4G 		= (1 << 3),
	ACCEL_FS_8G 		= (2 << 3),
	ACCEL_FS_16G		= (3 << 3),
	// ACCEL_CONFIG2 register
	AFCHOICE_0			= (1 << 3),
	AFCHOICE_1			= (0 << 3),
	// INT_PIN_CFG register
	INT_FSYNC_EN		= (1 << 2), // FSYNC causes the interrupt
	INT_FSYNC_LOW		= (1 << 3), // FSYNC is active low
	INT_FSYNC_HIGH		= (0 << 3), // FSYNC is active high
	INT_RD2CLEAR		= (1 << 4), // any read clears interrupt status
	INT_PULSE			= (0 << 5), // interrupt is a 50us pulse
	INT_LATCH			= (1 << 5), // level is held until interrupt status is cleared
	INT_OPEN			= (1 << 6), // INT pin is open drain
	INT_PUSHPULL		= (0 << 6), // INT pin is push-pull
	INT_HIGH			= (0 << 7), // INT pin is active high
	INT_LOW 			= (1 << 7), // INT pin is active low
	// INT_ENABLE, INT_STATUS registers
	INT_RAW_RDY 		= (1 << 0),
	INT_FSYNC			= (1 << 3),
	INT_OVERFLOW		= (1 << 4),
	INT_WOM 			= (1 << 6),
	// USER_CTRL register
	CTRL_SIG_RST		= (1 << 0),
	CTRL_FIFO_RST		= (1 << 2),
	CTRL_I2C_DIS		= (1 << 4),
	CTRL_FIFO_EN		= (1 << 6),
	// FIFO_EN register
	FIFO_ACCEL			= (1 << 3),
	FIFO_GYRO_Z 		= (1 << 4),
	FIFO_GYRO_Y 		= (1 << 5),
	FIFO_GYRO_X 		= (1 << 6),
	FIFO_GYRO			= (FIFO_GYRO_X | FIFO_GYRO_Y | FIFO_GYRO_Z),
	FIFO_TEMP			= (1 << 7),
	// PWR_MGMT_1 register
	PWR_CLK_OSC 		= (0 << 0),
	PWR_CLK_PLL1		= (1 << 0),
	PWR_SLEEP			= (1 << 6),
	PWR_RESET			= (1 << 7),
	// RESET register
	RESET_TEMP			= (1 << 0),
	RESET_ACCEL 		= (1 << 1),
	RESET_GYRO			= (1 << 2),
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
/** Write SPI data */
VOID Gyro_SPI_Write( UCHAR addr, UCHAR data );

/** Read SPI data */
VOID Gyro_SPI_Read( UCHAR addr, UCHAR* data );

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

VOID Gyro_Main_DMP( INT32 stacd ); // dummy

//=========================GyroIF Start================================
extern E_CAMERA_ERR_CODE GyroIF_start_read( VOID );
extern E_CAMERA_ERR_CODE GyroIF_stop_read( VOID );
extern E_CAMERA_ERR_CODE GyroIF_get_read_count( UINT32* read_count );
//=========================GyroIF End================================

#endif	/* _CAMERA_GYRO_MPU9250_H_ */

