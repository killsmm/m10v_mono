/**
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 * @file		: camera_gyro_icm20602.h
 * @brief 		: Gyro driver for ICM-20602
 * @note		: None
 * @attention	: None
 */

#ifndef _CAMERA_GYRO_ICM20602_H_
#define _CAMERA_GYRO_ICM20602_H_

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
	G_OFFS_TC			= 0x04,	// GYROSCOPE LOW NOISE TO LOW POWER OFFSET SHIFT AND GYROSCOPE OFFSET TEMPERATURE COMPENSATION (TC) REGISTER
	SELF_TEST_ACCEL		= 0x0D,	// ACCELEROMETER SELF-TEST REGISTERS
	G_OFFS_USR			= 0x13,	// GYRO OFFSET ADJUSTMENT REGISTER
	SMPLRT_DIV			= 0x19,	// SAMPLE RATE DIVIDER
	CONFIG				= 0x1A,	// CONFIGURATION
	GYRO_CONFIG			= 0x1B, // GYROSCOPE CONFIGURATION
	ACCEL_CONFIG		= 0x1C,	// ACCELEROMETER CONFIGURATION
	ACCEL_CONFIG2		= 0x1D,
	LP_MODE_CFG			= 0x1E,	// GYROSCOPE LOW POWER MODE CONFIGURATION
	ACCEL_WOM_THR		= 0x20,	// WAKE-ON MOTION THRESHOLD
	FIFO_EN				= 0x23,	// FIFO ENABLE
	FSYNC_INT			= 0x36,	// FSYNC INTERRUPT STATUS
	INT_PIN_CFG			= 0x37,	// INT/DRDY PIN / BYPASS ENABLE CONFIGURATION
	INT_ENABLE			= 0x38,
	FIFO_WM_INT_STATUS	= 0x39,	// FIFO WATERMARK INTERRUPT STATUS
	INT_STATUS			= 0x3A,	// INTERRUPT STATUS
	ACCEL_OUT			= 0x3B,	// ACCELEROMETER MEASUREMENTS
	TEMP_OUT			= 0x41,	// TEMPERATURE MEASUREMENT
	GYRO_OUT			= 0x43,	// GYROSCOPE MEASUREMENT
	SELF_TEST_GYRO		= 0x50,	// GYROSCOPE SELF-TEST REGISTERS
	FIFO_WM_TH1			= 0x60,	// FIFO WATERMARK THRESHOLD IN NUMBER OF BYTES
	FIFO_WM_TH2			= 0x61,
	SIGNAL_PATH_RESET	= 0x68,	// SIGNAL PATH RESET
	ACCEL_INTEL_CTRL	= 0x69,	// ACCELEROMETER INTELLIGENCE CONTROL
	USER_CTRL			= 0x6A,	// USER CONTROL
	PWR_MGMT_1			= 0x6B,	// POWER MANAGEMENT
	PWR_MGMT_2			= 0x6C,
	I2C_IF				= 0x70,	// I2C INTERFACE
	FIFO_COUNT			= 0x72,	// FIFO COUNT REGISTERS
	FIFO_RW				= 0x74,	// FIFO READ WRITE
	WHO_AM_I			= 0x75,	// WHO AM I
	A_OFFSET			= 0x77,	// ACCELEROMETER OFFSET REGISTERS
} E_GYRO_REG_ADDR;

/**
 * Definition of Register Data
 */
typedef enum {
	// CONFIG register
	#define DLPF(v) 	((v) << 0)	// For the DLPF to be used, FCHOICE_B[1:0] is 2’b00.
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
	FCHOICE_B_x1		= (1 << 0),
	FCHOICE_B_10		= (2 << 0),
	FCHOICE_B_00		= (0 << 0),
	GYRO_FS_250 		= (0 << 3),
	GYRO_FS_500 		= (1 << 3),
	GYRO_FS_1000		= (2 << 3),
	GYRO_FS_2000		= (3 << 3),
	// ACCEL_CONFIG register
	ACCEL_FS_2G 		= (0 << 3),
	ACCEL_FS_4G 		= (1 << 3),
	ACCEL_FS_8G 		= (2 << 3),
	ACCEL_FS_16G		= (3 << 3),
	ACCEL_DEC2_4		= (0 << 4),
	ACCEL_DEC2_8		= (1 << 4),
	ACCEL_DEC2_16		= (2 << 4),
	ACCEL_DEC2_32		= (3 << 4),
	// ACCEL_CONFIG2 register
	ACCEL_FCHOICE_B_1	= (1 << 3),
	ACCEL_FCHOICE_B_0	= (0 << 3),
	// INT_PIN_CFG register
	INT_FSYNC_EN		= (1 << 2), // FSYNC causes the interrupt
	INT_FSYNC_LOW		= (1 << 3), // FSYNC is active low
	INT_FSYNC_HIGH		= (0 << 3), // FSYNC is active high
	INT_RD2CLEAR		= (1 << 4), // any read clears interrupt status
	INT_PULSE			= (0 << 5), // interrupt is a 50us pulse
	INT_LATCH			= (1 << 5), // level is held until interrupt status is cleared
	INT_PUSHPULL		= (0 << 6), // INT pin is push-pull
	INT_OPEN			= (1 << 6), // INT pin is open drain
	INT_HIGH			= (0 << 7), // INT pin is active high
	INT_LOW 			= (1 << 7), // INT pin is active low
	// INT_ENABLE, INT_STATUS registers
	INT_RAW_RDY 		= (1 << 0),
	INT_GDRIVE			= (1 << 2),
	INT_FSYNC			= (1 << 3),
	INT_OVERFLOW		= (1 << 4),
	INT_WOM_Z			= (1 << 5),
	INT_WOM_Y			= (1 << 6),
	INT_WOM_X			= (1 << 7),
	INT_WOM 			= (INT_WOM_X | INT_WOM_Y | INT_WOM_Z),
	// USER_CTRL register
	CTRL_SIG_RST		= (1 << 0),
	CTRL_FIFO_RST		= (1 << 2),
	CTRL_FIFO_EN		= (1 << 6),
	// FIFO_EN register
	FIFO_ACCEL			= (1 << 3),
	FIFO_GYRO	 		= (1 << 4),
	// PWR_MGMT_1 register
	PWR_CLK_OSC 		= (0 << 0),
	PWR_CLK_PLL1		= (1 << 0),
	PWR_SLEEP			= (1 << 6),
	PWR_RESET			= (1 << 7),
	// SIGNAL_PATH_RESET register
	RESET_TEMP			= (1 << 0),
	RESET_ACCEL 		= (1 << 1),
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

#endif	/* _CAMERA_GYRO_ICM20602_H_ */

