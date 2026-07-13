/**
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 * @file		: camera_gyro_icm20948.c
 * @brief 		: Gyro driver for ICM-20948
 * @note		: None
 * @attention	: None
 */

#include "camera_config.h"
#include "camera_use.h"
#if USE_GYRO == GYRO_ICM20948

#include <stdio.h>
#include <string.h>
#include "camera_gyro_icm20948.h"
#include "fj_peripheral.h"
#include "os_user_custom.h"

// for DMP : start
#include <stdint.h>
#include <stdlib.h>
#include "Icm20948.h"
#include "Icm20948MPUFifoControl.h"
#include "SensorTypes.h"
#include "dd_arm.h"
#include "mba_if.h"
// for DMP : end

//=========================GyroIF Start================================
#include "peripheral.h"
#include "chiptop.h"
#include "../../../MILB_API/MILB_Header/include/spi.h"
#include "dd_spi.h"
#include "dd_tmr64.h"
#include "dd_hdmac1.h"
#include "debug.h"
#include "os_user_custom.h"
#include "sdram_map_cache_define.h"
#include "l1l2cache.h"
#include "fj_memory.h"
//=========================GyroIF End================================

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_GYRO_CHIPID				( 0xEA ) // WHOAMI value for ICM20948 or derivative

#define D_GYRO_SPI_CH				( 2 )
#define D_GYRO_SPI_PORT_SCK			( FJ_GPIO_PB0 )
#define D_GYRO_SPI_PORT_CSX			( FJ_GPIO_PB1 )
#define D_GYRO_SPI_PORT_MOSI		( FJ_GPIO_PB2 )
#define D_GYRO_SPI_PORT_MISO		( FJ_GPIO_PB3 )
#define D_GYRO_SPI_PORT_SPIRLTRG	( FJ_GPIO_PB4 )

// for DMP : start
#define D_GYRO_DMP_OFF
//#define D_GYRO_DMP_MBA_ON
//#define D_GYRO_DMP_PRINT_ON

#define D_GYRO_AK_I2C_ADDR  		( 0x0C ) /* I2C address for Ak09916 */
// for DMP : end

//=========================GyroIF Start================================
#define D_BF_GYROIF_SPI_CH					(FJ_PERI_CH_2)

#define D_BF_GYROIF_HDMAC1_CH_TMR64			(FJ_USIO_DMA_CH_0)
#define D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER1	(FJ_USIO_DMA_CH_1)
#define D_BF_GYROIF_HDMAC1_CH_SPI_SEND		(FJ_USIO_DMA_CH_2)
#define D_BF_GYROIF_HDMAC1_CH_SPI_RECV		(FJ_USIO_DMA_CH_3)
#define D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER0	(FJ_USIO_DMA_CH_4)

//#define D_GYROIF_TIMESTAMP_DMA
//#define D_GYROIF_DEBUG
//=========================GyroIF End================================
//=========================GyroCtrl Start================================
#define D_GYROCTRL_DATA_NUM		(15)		// Register address(1byte) + Gyro(2byte * 3) + Temp(2byte * 1) + Accel(2byte * 3)
#define D_GYROCTRL_BUFF_NUM		(65536)		// Modify for DMP
//=========================GyroCtrl End================================

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
static FJ_SPI_CTRL gGyro_SPI_Ctrl = {
	.type				= FJ_SPI_TYPE_MASTER,
	.mode				= FJ_SPI_MODE_3_CPOL1_CPHA1,
	.bit_direction 		= FJ_SPI_BIT_DIR_MSB_FIRST,
	.data_length		= FJ_SPI_LENGTH_8,
	.baudrate			= 1000000,	// 1MHz
	.fifo_wmark_tx		= 8,
	.fifo_wmark_rx		= 8,
	.dma_en				= 0,
	.dma_recv_to		= 0,
	.pcallback			= NULL,
};

static FJ_SPI_CTRL_TRANSFER gGyro_SPI_Trans = {
	.direction			= FJ_SPI_DATA_DIR_SEND,
	.ss_info.cont_trans	= FJ_SPI_CONT_TRANS_INACT_FIFO_EMPTY,
	.ss_info.ssout[0]	= 1,
	.ss_info.ssout[1]	= 0,
	.ss_info.ssout[2]	= 0,
	.ss_info.ssout[3]	= 0,
	.ss_info.sspol[0]	= 0,
	.ss_info.sspol[1]	= 0,
	.ss_info.sspol[2]	= 0,
	.ss_info.sspol[3]	= 0,
	.data_ptr			= NULL,
	.data_count			= 0,
};

static FJ_SPI_CTRL_FULL_DUPLEX gGyro_SPI_Trans_Full = {
	.ss_info.cont_trans	= FJ_SPI_CONT_TRANS_ACT_FIFO_EMPTY,
	.ss_info.ssout[0]	= 1,
	.ss_info.ssout[1]	= 0,
	.ss_info.ssout[2]	= 0,
	.ss_info.ssout[3]	= 0,
	.ss_info.sspol[0]	= 0,
	.ss_info.sspol[1]	= 0,
	.ss_info.sspol[2]	= 0,
	.ss_info.sspol[3]	= 0,
	.send_ptr			= NULL,
	.recv_ptr			= NULL,
	.data_count			= 0,
};

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static const UCHAR	g_gyro_icm20948_accel_fs_sel = ACCEL_FS_2G;
static const UCHAR	g_gyro_icm20948_gyro_fs_sel = GYRO_FS_1000;


// for DMP : start
static inv_icm20948_t g_gyro_icm_device;

static UINT8 g_gyro_icm_convert_to_generic_ids[INV_ICM20948_SENSOR_MAX] = {
	INV_SENSOR_TYPE_ACCELEROMETER,
	INV_SENSOR_TYPE_GYROSCOPE,
	INV_SENSOR_TYPE_RAW_ACCELEROMETER,
	INV_SENSOR_TYPE_RAW_GYROSCOPE,
	INV_SENSOR_TYPE_UNCAL_MAGNETOMETER,
	INV_SENSOR_TYPE_UNCAL_GYROSCOPE,
	INV_SENSOR_TYPE_BAC,
	INV_SENSOR_TYPE_STEP_DETECTOR,
	INV_SENSOR_TYPE_STEP_COUNTER,
	INV_SENSOR_TYPE_GAME_ROTATION_VECTOR,
	INV_SENSOR_TYPE_ROTATION_VECTOR,
	INV_SENSOR_TYPE_GEOMAG_ROTATION_VECTOR,
	INV_SENSOR_TYPE_MAGNETOMETER,
	INV_SENSOR_TYPE_SMD,
	INV_SENSOR_TYPE_PICK_UP_GESTURE,
	INV_SENSOR_TYPE_TILT_DETECTOR,
	INV_SENSOR_TYPE_GRAVITY,
	INV_SENSOR_TYPE_LINEAR_ACCELERATION,
	INV_SENSOR_TYPE_ORIENTATION,
	INV_SENSOR_TYPE_B2S
};

static const UINT8 g_gyro_icm_dmp3_image[] = {
	#include "icm20948_img.dmp3a.h"
};
/* 
 * Mounting matrix configuration applied for both Accel and Gyro 
 * The coefficient values are coded in integer q30
 */
static const FLOAT g_gyro_icm_cfg_mounting_matrix[9]= {1.f*(1<<30), 0,           0,
 													   0,           1.f*(1<<30), 0,
 													   0,           0,           1.f*(1<<30) };

static INT32 g_gyro_icm_cfg_acc_fsr = 4; // Default = +/- 4g. Valid ranges: 2, 4, 8, 16
static INT32 g_gyro_icm_cfg_gyr_fsr = 2000; // Default = +/- 2000dps. Valid ranges: 250, 500, 1000, 2000


static T_CAMERA_GYRO_MPU_DATA gGyro_Dmp_Raw_Data;
// for DMP : end

//=========================GyroIF Start================================
static volatile FJ_GYROIF_READ_CTRL		gGyroIF_Read_Ctrl;
static volatile BOOL					gGyroIF_Stop_Flag;
static volatile union io_spi_auxctrl	gAUX_CTRL_valu_1 __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));	// D-Cache line aligned.;
static volatile union io_spi_auxctrl	gAUX_CTRL_valu_2 __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));	// D-Cache line aligned.;

#ifdef D_GYRO_DMP_OFF	// for DMP : start
static ULONG gBF_GyroIF_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));
#endif	// for DMP : end
//=========================GyroIF End================================
//=========================GyroCtrl Start================================
static VOID gyroctrl_data_read_callbak( FJ_ERR_CODE result );
static VOID gyroctrl_timestamp_callbak( FJ_ERR_CODE result );

// SPI Send buffer. Size is same as data_count. (register address and dummy data for data receive.)
UCHAR gGyroCtrl_Send_Data[D_GYROCTRL_DATA_NUM] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));	// D-Cache line aligned.
// SPI Receive buffer.
UCHAR gGyroCtrl_Recv_Data[D_GYROCTRL_BUFF_NUM][D_GYROCTRL_DATA_NUM] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));	// D-Cache line aligned.
// TimeStamp buffer. Size is Same as element count of Gyro data.
ULONG gGyroCtrl_TimeStamp[D_GYROCTRL_BUFF_NUM * 2] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));	// D-Cache line aligned.

FJ_GYROIF_READ_CTRL gGyroCtrl_Data =
{
	.data_ctrl.mode					= FJ_SPI_MODE_3_CPOL1_CPHA1,			// Clock Polarity=1, Clock Phase=1
	.data_ctrl.bit_direction		= FJ_SPI_BIT_DIR_MSB_FIRST, 			// MSB First
	.data_ctrl.data_length 			= FJ_SPI_LENGTH_8,						// 8bit
	.data_ctrl.baudrate				= 7000000,								// 7MHz (ch2 is max 50MHz. when SPICLK=216MHz, ICM20948 : 7MHz)
	.data_ctrl.fifo_wmark_tx		= 8,									// TX FIFO Watermark (ch2 Tx FIFO size is 16)
	.data_ctrl.fifo_wmark_rx		= D_GYROCTRL_DATA_NUM,					// RX FIFO Watermark (ch2 Rx FIFO size is 16)
	.data_ctrl.ss_info.cont_trans	= FJ_SPI_CONT_TRANS_INACT_FIFO_EMPTY,	// ssOut become inactive when the TX FIFO is empty.
	.data_ctrl.ss_info.ssout[0]		= 1,									// Connected to the device
	.data_ctrl.ss_info.ssout[1]		= 0,									// Not Connected
	.data_ctrl.ss_info.ssout[2]		= 0,									// Not Connected
	.data_ctrl.ss_info.ssout[3]		= 0,									// Not Connected
	.data_ctrl.ss_info.sspol[0]		= 0,									// Active Low
	.data_ctrl.ss_info.sspol[1]		= 0,									// Not Connected
	.data_ctrl.ss_info.sspol[2]		= 0,									// Not Connected
	.data_ctrl.ss_info.sspol[3]		= 0,									// Not Connected
	.data_ctrl.send_addr			= (ULONG)gGyroCtrl_Send_Data,			// Send buffer address
	.data_ctrl.recv_addr			= (ULONG)gGyroCtrl_Recv_Data,			// Receive buffer address
	.data_ctrl.data_count			= D_GYROCTRL_DATA_NUM,					// gyro/accel (2 x 3Byte x 12bit)
	.data_ctrl.total_data_count		= D_GYROCTRL_BUFF_NUM,					// Count of receive buffer
	.data_ctrl.dma_recv_to 			= 16777215, 							// Should be set Max value
	.data_ctrl.pcallback			= gyroctrl_data_read_callbak,			// Callback function pointer
	.stamp_ctrl.timer_clk			= FJ_GYROIF_TMR_CLK_DIV_2,				// 64bit Timer is 2 dividing frequency
	.stamp_ctrl.buff_addr			= (ULONG)gGyroCtrl_TimeStamp,			// TimeStamp buffer address
	.stamp_ctrl.total_data_count	= D_GYROCTRL_BUFF_NUM,					// Count of TimeStamp buffer (Same as Gyro data total_data_count)
	.stamp_ctrl.pcallback			= gyroctrl_timestamp_callbak,			// Callback function pointer
};

UINT32 gGyroCtrl_BufSize[E_CAMERA_GYRO_BUF_SIZE_MAX] =
{
	sizeof(gGyroCtrl_Send_Data),
	sizeof(gGyroCtrl_Recv_Data),
	sizeof(gGyroCtrl_TimeStamp),
};

//=========================GyroCtrl End================================

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
int gyro_write_reg( void * context, uint8_t reg, const uint8_t * wbuffer, uint32_t wlen )
{
	(VOID)context;
	Gyro_Write_N( reg, (UCHAR*)wbuffer, wlen );
	return 0;
}

int gyro_read_reg( void * context, uint8_t reg, uint8_t * rbuffer, uint32_t rlen )
{
	(VOID)context;
	UCHAR temp_data[20] = {0};
	
	Gyro_Read_N( reg, &temp_data[0], (rlen+1) );
	
	memcpy((VOID*)&rbuffer[0], (VOID*)&temp_data[1], rlen );
	
	return 0;
}

static VOID gyro_spi_set( VOID )
{
	FJ_ERR_CODE		ret;

	// Set SPI Port
	ret = FJ_Gpio_Set_Function( D_GYRO_SPI_PORT_SCK, 1 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Function ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Function( D_GYRO_SPI_PORT_CSX, 1 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Function ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Function( D_GYRO_SPI_PORT_MOSI, 1 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Function ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Function( D_GYRO_SPI_PORT_MISO, 1 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Function ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Function( D_GYRO_SPI_PORT_SPIRLTRG, 1 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Function ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Pull_Up_Down_Ctrl( D_GYRO_SPI_PORT_SCK, 1 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Pull_Up_Down_Ctrl ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Pull_Up_Down_Ctrl( D_GYRO_SPI_PORT_CSX, 1 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Pull_Up_Down_Ctrl ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Pull_Up_Down_Ctrl( D_GYRO_SPI_PORT_MOSI, 0 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Pull_Up_Down_Ctrl ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Pull_Up_Down_Ctrl( D_GYRO_SPI_PORT_MISO, 0 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Pull_Up_Down_Ctrl ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Pull_Up_Down_Enable( D_GYRO_SPI_PORT_SCK, 1 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Pull_Up_Down_Enable ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Pull_Up_Down_Enable( D_GYRO_SPI_PORT_CSX, 1 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Pull_Up_Down_Enable ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Pull_Up_Down_Enable( D_GYRO_SPI_PORT_MOSI, 1 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Pull_Up_Down_Enable ERROR! : %d\n", __LINE__);
	}

	ret = FJ_Gpio_Set_Pull_Up_Down_Enable( D_GYRO_SPI_PORT_MISO, 1 );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_Gpio_Set_Pull_Up_Down_Enable ERROR! : %d\n", __LINE__);
	}

	// Open SPI
	ret = FJ_SPI_Open( D_GYRO_SPI_CH );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_SPI_Open ERROR! : %d\n", __LINE__);
	}

	// Set SPI Control
	ret = FJ_SPI_Ctrl( D_GYRO_SPI_CH, &gGyro_SPI_Ctrl );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_SPI_Ctrl ERROR! : %d\n", __LINE__);
	}
}

static VOID gyro_spi_close( VOID )
{
	FJ_ERR_CODE		ret;

	// Close SPI
	ret = FJ_SPI_Close( D_GYRO_SPI_CH );
	if( ret != FJ_ERR_OK ) {
		printf("FJ_SPI_Close ERROR! : %d\n", __LINE__);
	}
}

// Write SPI data
static VOID gyro_spi_write( UCHAR addr, UCHAR data )
{
	FJ_ERR_CODE	ret;
	UCHAR		send_data[2];

	send_data[0] = addr;
	send_data[1] = data;

	gGyro_SPI_Trans.data_ptr	= send_data;
	gGyro_SPI_Trans.data_count	= sizeof(send_data);

	// Start send (CPU)
	ret = FJ_SPI_Transfer( D_GYRO_SPI_CH, &gGyro_SPI_Trans, FJ_USIO_DMA_CH_NOT_USE );
	if( ret != FJ_ERR_OK ){
		printf("FJ_SPI_Transfer ERROR! : %d\n", __LINE__);
	}
}

// Write SPI data
static VOID gyro_spi_write_n( UCHAR addr, UCHAR* data, UCHAR write_bytes )
{
	FJ_ERR_CODE	ret;
	UCHAR		send_data[20];

	if( write_bytes >= sizeof(send_data) ){
		printf("%s() ERROR! : %d\n", __func__, __LINE__);	/* pgr0822 */
		return;
	}

	send_data[0] = addr;
	memcpy( (VOID*)&send_data[1], (VOID*)data, write_bytes );
	
	gGyro_SPI_Trans.data_ptr	= send_data;
	gGyro_SPI_Trans.data_count	= write_bytes+1;	// data size + addr

	// Start send (CPU)
	ret = FJ_SPI_Transfer( D_GYRO_SPI_CH, &gGyro_SPI_Trans, FJ_USIO_DMA_CH_NOT_USE );
	if( ret != FJ_ERR_OK ){
		printf("FJ_SPI_Transfer ERROR! : %d\n", __LINE__);
	}
}

// Read SPI data
static VOID gyro_spi_read( UCHAR addr, UCHAR* data )
{
	FJ_ERR_CODE	ret;
	UCHAR		send_data[2] = {0};
	UCHAR		recv_data[2] = {0};

	send_data[0] = D_GYRO_SPI_READ_OP | addr;

	gGyro_SPI_Trans_Full.send_ptr	= send_data;
	gGyro_SPI_Trans_Full.recv_ptr	= recv_data;
	gGyro_SPI_Trans_Full.data_count	= sizeof(recv_data);

	// Start fullduplex (CPU)
	ret = FJ_SPI_Full_Duplex( D_GYRO_SPI_CH, &gGyro_SPI_Trans_Full );
	if( ret != FJ_ERR_OK ){
		printf("FJ_SPI_Full_Duplex ERROR! : %d\n", __LINE__);
	}

	*data = recv_data[1];
}

// Read SPI data
static VOID gyro_spi_read_n( UCHAR addr, UCHAR* data, UCHAR read_bytes )
{
	FJ_ERR_CODE	ret;
	UCHAR		send_data[64] = {0};

	if( read_bytes >= (sizeof(send_data) / sizeof(send_data[0])) ) {
		printf("%s() ERROR! : %d\n", __func__, __LINE__);	/* pgr0822 */
		return;
	}

	send_data[0] = D_GYRO_SPI_READ_OP | addr;

	gGyro_SPI_Trans_Full.send_ptr	= send_data;
	gGyro_SPI_Trans_Full.recv_ptr	= data;
	gGyro_SPI_Trans_Full.data_count	= read_bytes;

	// Start fullduplex (CPU)
	ret = FJ_SPI_Full_Duplex( D_GYRO_SPI_CH, &gGyro_SPI_Trans_Full );
	if( ret != FJ_ERR_OK ){
		printf("FJ_SPI_Full_Duplex ERROR! : %d\n", __LINE__);
	}
}

static FLOAT gyro_data2float( const UCHAR data_h, const UCHAR data_l )
{
	union {
		SHORT data_short;
		USHORT data_ushort;
	} us2s;

	us2s.data_ushort = (((USHORT)data_h) << 8) | data_l;

	return (FLOAT)us2s.data_short;	/* pgr0872 */
}

// for DMP : start

// User Function from ICM20948 DMP Driver
void inv_icm20948_sleep(int ms)
{
	OS_User_Dly_Tsk( ms );
}

void inv_icm20948_sleep_us(int us)
{
	INT32 ms;
	if( us < 1000 ){
		ms = 1;
	}
	else{
		ms = us / 1000;
		if( (us % 1000) != 0 ){
			ms += 1;
		} 
	}
	OS_User_Dly_Tsk( ms );
}

ULLONG inv_icm20948_get_time_us(void)
{
	ULLONG timestamp;
	OS_USER_SYSTIM time;
	
	OS_User_Get_Tim(&time);
	
	timestamp = (ULLONG)(time*1000);	// ms->us
	return timestamp;
}

static inline void inv_icm20948_reset_states(struct inv_icm20948 * s, const struct inv_icm20948_serif * serif)
{
	memset(s, 0, sizeof(*s));
	s->serif = *serif;
	icm20948_instance = s;
}

VOID inv_icm20948_build_sensor_event_data(VOID * context, UINT8 sensortype, uint64_t timestamp, const VOID * data, const VOID *arg)
{
	UINT8 sensor_id = g_gyro_icm_convert_to_generic_ids[sensortype];
	float f_data[6];
#ifdef D_GYRO_DMP_PRINT_ON
	static INT32 a_count = 0;
	static INT32 g_count = 0;
	static INT32 m_count = 0;
	static INT32 q_count = 0;
	static INT32 o_count = 0;
#endif
	
	switch(sensor_id){
		case INV_SENSOR_TYPE_ACCELEROMETER:
			memcpy(f_data, data, sizeof(f_data));
			gGyro_Dmp_Raw_Data.accel[0] = f_data[0];
			gGyro_Dmp_Raw_Data.accel[1] = f_data[1];
			gGyro_Dmp_Raw_Data.accel[2] = f_data[2];
#ifdef D_GYRO_DMP_MBA_ON
			MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_POINT, "ACCEL"));
#endif
#ifdef D_GYRO_DMP_PRINT_ON
			a_count++;
			if( a_count > 10 ){
				printf("ax:%f, ay:%f, az:%f \n", f_data[0], f_data[1], f_data[2] );
				a_count = 0;
			}
#endif
			break;
		case INV_SENSOR_TYPE_RAW_GYROSCOPE:
		case INV_SENSOR_TYPE_GYROSCOPE:
			memcpy(f_data, data, sizeof(f_data));
			gGyro_Dmp_Raw_Data.gyro[0] = f_data[0];
			gGyro_Dmp_Raw_Data.gyro[1] = f_data[1];
			gGyro_Dmp_Raw_Data.gyro[2] = f_data[2];
#ifdef D_GYRO_DMP_MBA_ON
			MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_POINT, "GYRO"));
#endif
#ifdef D_GYRO_DMP_PRINT_ON
			g_count++;
			if( g_count > 10 ){
				printf("gx:%f, gy:%f, gz:%f \n", f_data[0], f_data[1], f_data[2] );
				g_count = 0;
			}
#endif
			break;
		case INV_SENSOR_TYPE_MAGNETOMETER:
			memcpy(f_data, data, sizeof(f_data));
			gGyro_Dmp_Raw_Data.compass[0] = f_data[0];
			gGyro_Dmp_Raw_Data.compass[1] = f_data[1];
			gGyro_Dmp_Raw_Data.compass[2] = f_data[2];
#ifdef D_GYRO_DMP_MBA_ON
			MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_POINT, "MAG"));
#endif
#ifdef D_GYRO_DMP_PRINT_ON
			m_count++;
			if( m_count > 10 ){
				printf("mx:%f, my:%f, mz:%f \n", f_data[0], f_data[1], f_data[2] );
				m_count = 0;
			}
#endif
			break;
		case INV_SENSOR_TYPE_ORIENTATION:
			memcpy(f_data, data, sizeof(f_data));
			gGyro_Dmp_Raw_Data.orientation[0] = f_data[0];
			gGyro_Dmp_Raw_Data.orientation[1] = f_data[1];
			gGyro_Dmp_Raw_Data.orientation[2] = f_data[2];
#ifdef D_GYRO_DMP_MBA_ON
			MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_POINT, "ORIENT"));
#endif
#ifdef D_GYRO_DMP_PRINT_ON
			o_count++;
			if( o_count > 100 ){
				printf("yaw:%f, pitch:%f, roll:%f \n", f_data[0], f_data[1], f_data[2] );
				o_count = 0;
			}
#endif
			break;
		case INV_SENSOR_TYPE_ROTATION_VECTOR:
			memcpy(f_data, data, sizeof(f_data));
			gGyro_Dmp_Raw_Data.quat[0] = f_data[0];
			gGyro_Dmp_Raw_Data.quat[1] = f_data[1];
			gGyro_Dmp_Raw_Data.quat[2] = f_data[2];
			gGyro_Dmp_Raw_Data.quat[3] = f_data[3];
#ifdef D_GYRO_DMP_MBA_ON
			MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_POINT, "QUAT"));
#endif
#ifdef D_GYRO_DMP_PRINT_ON
			q_count++;
			if( q_count > 100 ){
				printf("q0:%f, q1:%f, q2:%f, q3:%f \n", f_data[0], f_data[1], f_data[2], f_data[3] );
				q_count = 0;
			}
#endif
			break;
		default:
			// Not Support
			break;
	}
}
// for DMP : end

//=========================GyroIF Start================================
/**
Callback function from TMR64 DMA interrupt.
*/
static VOID gyroif_tmr64_callback( UCHAR dma_ch, USHORT* const status )
{
	
	if( *status != D_DD_HDMAC1_SS_NORMAL_END ){
		if( *status != D_DD_HDMAC1_SS_STOP_REQ ){
			printf("FJ_GyroIF: TMR64 HDMAC[%d] stop status error!! : line=%d, status=%d\n", dma_ch, __LINE__, *status);
			if( gGyroIF_Read_Ctrl.stamp_ctrl.pcallback != NULL ) {
				(*gGyroIF_Read_Ctrl.stamp_ctrl.pcallback)( FJ_ERR_NG );
			}
		}
		else {
			printf("FJ_GyroIF: TMR64 HDMAC[%d] stop end : line=%d, status=%d\n", dma_ch, __LINE__, *status);
		}
	}
#ifndef D_GYRO_DMP_OFF	// for DMP : start
	IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_TMR64].DMACA.bit.EB = 1;
	OS_User_Set_Flg( FID_GYRO_DMP, FLG_GYRO_DMP_INT );
#endif	// for DMP : end
}

#ifndef TIMER64_START_FROM_BOOT
/**
Start TMR64.
*/
static FJ_ERR_CODE gyroif_start_tmr64( FJ_GYROIF_STAMP_CTRL const* const stamp_ctrl )
{
	INT32			ret;
	T_DD_TMR64_CTRL	tmr64_ctrl;

	// Open TMR64
	ret = Dd_TMR64_Open( D_DDIM_USER_SEM_WAIT_POL );
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_TMR64_Open Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		return FJ_ERR_NG;
	}

	// Ctrl TMR64
	tmr64_ctrl.csr1.word	= 0;
	tmr64_ctrl.csr1.bit.csl	= stamp_ctrl->timer_clk;
	tmr64_ctrl.csr2.word	= 0;

	ret = Dd_TMR64_Ctrl( &tmr64_ctrl );
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_TMR64_Ctrl Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		Dd_TMR64_Close();
		return FJ_ERR_NG;
	}

	// Start TMR64
	ret = Dd_TMR64_Start( FALSE );
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_TMR64_Start Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		Dd_TMR64_Close();
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}
#endif

/**
Stop TMR64.
*/
static FJ_ERR_CODE gyroif_stop_tmr64( VOID )
{
	FJ_ERR_CODE	err_code = FJ_ERR_OK;
#ifndef TIMER64_START_FROM_BOOT
	INT32		ret;

	// Start TMR64
	ret = Dd_TMR64_Stop();
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_TMR64_Stop Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		err_code = FJ_ERR_NG;
	}

	// Open TMR64
	ret = Dd_TMR64_Close();
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_TMR64_Close Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		err_code = FJ_ERR_NG;
	}
#endif
	return err_code;
}

/**
Start DMA for TMR64.
*/
static FJ_ERR_CODE gyroif_start_dma_tmr64( FJ_GYROIF_STAMP_CTRL const* const stamp_ctrl )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	UCHAR				dma_ch = D_BF_GYROIF_HDMAC1_CH_TMR64;

	// Open TMR64 DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_SEM_WAIT_POL );
	if( ret != D_DDIM_OK ){
		printf("FJ_GyroIF: Dd_HDMAC1_Open Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch);
		return FJ_ERR_NG;
	}

	// Set TMR64 DMA Ctrl.
	hdmac1_ctrl.config_a.word	= 0;
	hdmac1_ctrl.config_b.word	= 0;
	hdmac1_ctrl.config_a.bit.is	= D_DD_HDMAC1_IS_IDREQ_24;
	hdmac1_ctrl.config_a.bit.bt	= D_DD_HDMAC1_BT_NORMAL;
	hdmac1_ctrl.config_a.bit.bc	= 2 - 1;
	hdmac1_ctrl.config_a.bit.tc	= stamp_ctrl->total_data_count - 1;
//	hdmac1_ctrl.config_b.bit.tt	= 0;
	hdmac1_ctrl.config_b.bit.ms	= D_DD_HDMAC1_MS_BLK;
	hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_WORD;
	hdmac1_ctrl.config_b.bit.fs	= D_DD_HDMAC1_FS_FIX;
	hdmac1_ctrl.config_b.bit.fd	= D_DD_HDMAC1_FD_INCR;
	hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
	hdmac1_ctrl.config_b.bit.rs	= D_DD_HDMAC1_RS_DISABLE;
	hdmac1_ctrl.config_b.bit.rd	= D_DD_HDMAC1_RD_ENABLE;
//	hdmac1_ctrl.config_b.bit.ei = 1;
//	hdmac1_ctrl.config_b.bit.ci = 1;
//	hdmac1_ctrl.config_b.bit.ss = 0;
	hdmac1_ctrl.config_b.bit.sp = 8;
	hdmac1_ctrl.config_b.bit.dp = 8;
	hdmac1_ctrl.src_addr		= (ULONG)&(IO_PERI.TMR64.GSTMP);
	hdmac1_ctrl.dst_addr		= stamp_ctrl->buff_addr;
	hdmac1_ctrl.int_handler		= (VP_CALLBACK)gyroif_tmr64_callback;

	// Ctrl TMR64 DMA.
	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, &hdmac1_ctrl );
	if( ret != D_DDIM_OK ){
		printf("FJ_GyroIF: Dd_HDMAC1_Ctrl_Common Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch);
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	// Start TMR64 DMA.
	ret = Dd_HDMAC1_Start_Async( dma_ch );
	if( ret != D_DDIM_OK ){
		printf("FJ_GyroIF: Dd_HDMAC1_Start_Async Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch);
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

// for DMP : start
	IO_HDMAC1.DMAC[dma_ch].DMACB.bit.CI = 0;
// for DMP : end

	return FJ_ERR_OK;
}

/**
Stop DMA.
*/
static FJ_ERR_CODE gyroif_stop_dma( UCHAR dma_ch )
{
	FJ_ERR_CODE	err_code = FJ_ERR_OK;
	INT32		ret;

	// Stop HDMAC
	ret = Dd_HDMAC1_Stop( dma_ch );
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_HDMAC1_Stop Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		err_code = FJ_ERR_NG;
	}

	// Close HDMAC
	ret = Dd_HDMAC1_Close( dma_ch );
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_HDMAC1_Close Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		err_code = FJ_ERR_NG;
	}

	return err_code;
}

/**
Stop SPI.
*/
static FJ_ERR_CODE gyroif_stop_spi( VOID )
{
	FJ_ERR_CODE	err_code = FJ_ERR_OK;
	INT32		ret;
	UCHAR		spi_ch = D_BF_GYROIF_SPI_CH;

	// Stop SPI
	ret = Dd_SPI_Stop( spi_ch );
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_SPI_Stop Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		err_code = FJ_ERR_NG;
	}

	// Close SPI
	ret = Dd_SPI_Close( spi_ch );
	if( ret != D_DDIM_OK ) {
		printf("FJ_GyroIF: Dd_SPI_Close Error!! : line=%d, ret=0x%08x\n", __LINE__, ret);
		err_code = FJ_ERR_NG;
	}

	return err_code;
}

#ifdef D_GYRO_DMP_OFF	// for DMP : start

/**
Callback function from SPI send DMA interrupt.
*/
static VOID gyroif_spi_send_callback( UCHAR dma_ch, USHORT* const status )
{
	
	if( *status != D_DD_HDMAC1_SS_NORMAL_END ){
		if( *status != D_DD_HDMAC1_SS_STOP_REQ ){
			BF_Debug_Print_Warning(("FJ_GyroIF: SPI Send HDMAC[%d] stop status error!! : line=%d, status=%d\n", dma_ch, __LINE__, *status));
			if( gGyroIF_Read_Ctrl.data_ctrl.pcallback != NULL ) {
				(*gGyroIF_Read_Ctrl.data_ctrl.pcallback)( FJ_ERR_NG );
			}
		}
		else {
			BF_Debug_Print_Information(("FJ_GyroIF: SPI Send HDMAC[%d] stop end : line=%d, status=%d\n", dma_ch, __LINE__, *status));
		}
	}
}

/**
Callback function from SPI receive DMA interrupt.
*/
static VOID gyroif_spi_recv_callback( UCHAR dma_ch, USHORT* const status )
{
	
	if( *status != D_DD_HDMAC1_SS_NORMAL_END ){
		if( *status != D_DD_HDMAC1_SS_STOP_REQ ){
			BF_Debug_Print_Warning(("FJ_GyroIF: SPI Recv HDMAC[%d] stop status error!! : line=%d, status=%d\n", dma_ch, __LINE__, *status));
			if( gGyroIF_Read_Ctrl.data_ctrl.pcallback != NULL ) {
				(*gGyroIF_Read_Ctrl.data_ctrl.pcallback)( FJ_ERR_NG );
			}
		}
		else {
			BF_Debug_Print_Information(("FJ_GyroIF: SPI Recv HDMAC[%d] stop end : line=%d, status=%d\n", dma_ch, __LINE__, *status));
		}
	}
}

/**
Callback function from SPI contXferExtend=1 DMA interrupt.
*/
static VOID gyroif_spi_contxfer1_callback( UCHAR dma_ch, USHORT* const status )
{
	
	if( *status != D_DD_HDMAC1_SS_NORMAL_END ){
		if( *status != D_DD_HDMAC1_SS_STOP_REQ ){
			BF_Debug_Print_Warning(("FJ_GyroIF: SPI Cont1 HDMAC[%d] stop status error!! : line=%d, status=%d\n", dma_ch, __LINE__, *status));
			if( gGyroIF_Read_Ctrl.data_ctrl.pcallback != NULL ) {
				(*gGyroIF_Read_Ctrl.data_ctrl.pcallback)( FJ_ERR_NG );
			}
		}
		else {
			BF_Debug_Print_Information(("FJ_GyroIF: SPI Cont1 HDMAC[%d] stop end : line=%d, status=%d\n", dma_ch, __LINE__, *status));
		}
	}
}

/**
Callback function from SPI contXferExtend=0 DMA interrupt.
*/
static VOID gyroif_spi_contxfer0_callback( UCHAR dma_ch, USHORT* const status )
{
	
#ifdef D_GYROIF_DEBUG
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_START, "GyroIF CB"));
#endif

#ifdef D_GYROIF_DEBUG
	IO_CHIPTOP.PDR.word[0] = 0x20002000;
	Dd_ARM_Dsb_Pou();
#endif

	if( *status != D_DD_HDMAC1_SS_NORMAL_END ){
		if( *status != D_DD_HDMAC1_SS_STOP_REQ ){
			BF_Debug_Print_Warning(("FJ_GyroIF: SPI Cont0 HDMAC[%d] stop status error!! : line=%d, status=%d\n", dma_ch, __LINE__, *status));
			if( gGyroIF_Read_Ctrl.data_ctrl.pcallback != NULL ) {
				(*gGyroIF_Read_Ctrl.data_ctrl.pcallback)( FJ_ERR_NG );
			}
		}
		else {
			BF_Debug_Print_Information(("FJ_GyroIF: SPI Cont0 HDMAC[%d] stop end : line=%d, status=%d\n", dma_ch, __LINE__, *status));
		}
	}
	else {
		Dd_ARM_Critical_Section_Start(gBF_GyroIF_Spin_Lock);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		// Disable input Gyro INT
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PB4, 0);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_TMR64].DMACA.bit.EB = 1;
		IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER1].DMACA.bit.EB = 1;
		IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_SPI_SEND].DMACA.bit.EB = 1;
		IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_SPI_RECV].DMACA.bit.EB = 1;
		IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER0].DMACA.bit.EB = 1;
		Dd_ARM_Dsb_Pou();
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		// Enable input Gyro INT
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PB4, 1);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		Dd_ARM_Critical_Section_End(gBF_GyroIF_Spin_Lock);
	}

#ifdef D_GYROIF_DEBUG
	IO_CHIPTOP.PDR.word[0] = 0x20000000;
	Dd_ARM_Dsb_Pou();
	BF_Debug_Print_Information(("-GyroIF CB-\n"));
#endif

#ifdef D_GYROIF_DEBUG
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_END, "GyroIF CB"));
#endif
}

/**
Start DMA for SPI send.
*/
static FJ_ERR_CODE gyroif_start_dma_spi_send( FJ_GYROIF_DATA_CTRL const* const data_ctrl )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	UCHAR				spi_ch = D_BF_GYROIF_SPI_CH;
	UCHAR				dma_ch = D_BF_GYROIF_HDMAC1_CH_SPI_SEND;

	// Open SPI send DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_SEM_WAIT_POL );
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Open Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		return FJ_ERR_NG;
	}

	// Set SPI send DMA Ctrl.
	hdmac1_ctrl.config_a.word		= 0;
	hdmac1_ctrl.config_b.word		= 0;
	hdmac1_ctrl.config_a.bit.is		= D_DD_HDMAC1_IS_IDREQ_24;
	hdmac1_ctrl.config_a.bit.bt		= D_DD_HDMAC1_BT_NORMAL;
	hdmac1_ctrl.config_a.bit.bt		= D_DD_HDMAC1_BT_INCR;
	hdmac1_ctrl.config_a.bit.bc		= data_ctrl->data_count - 1;
	hdmac1_ctrl.config_a.bit.tc		= data_ctrl->total_data_count - 1;
//	hdmac1_ctrl.config_b.bit.tt		= 0;
	hdmac1_ctrl.config_b.bit.ms		= D_DD_HDMAC1_MS_BLK;
	if( data_ctrl->data_length > FJ_SPI_LENGTH_8 ){
		hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_HWORD;
	}
	else{
		hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_BYTE;
	}
	hdmac1_ctrl.config_b.bit.fs		= D_DD_HDMAC1_FS_FIX;
	hdmac1_ctrl.config_b.bit.fd		= D_DD_HDMAC1_FD_FIX;
	hdmac1_ctrl.config_b.bit.rc		= D_DD_HDMAC1_RC_ENABLE;
	hdmac1_ctrl.config_b.bit.rs		= D_DD_HDMAC1_RS_ENABLE;
	hdmac1_ctrl.config_b.bit.rd		= D_DD_HDMAC1_RD_DISABLE;
//	hdmac1_ctrl.config_b.bit.ei		= 1;
//	hdmac1_ctrl.config_b.bit.ci		= 1;
//	hdmac1_ctrl.config_b.bit.ss		= 0;
	hdmac1_ctrl.config_b.bit.sp		= 8;
	hdmac1_ctrl.config_b.bit.dp		= 8;
	hdmac1_ctrl.src_addr			= data_ctrl->send_addr;
	hdmac1_ctrl.dst_addr			= (ULONG)&(IO_SPI[spi_ch].TXDATA);
	hdmac1_ctrl.int_handler			= (VP_CALLBACK)gyroif_spi_send_callback;

	// Ctrl SPI send DMA.
	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, &hdmac1_ctrl );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Ctrl_Common Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	// Start SPI send DMA.
	ret = Dd_HDMAC1_Start_Async( dma_ch );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Start_Async Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	IO_HDMAC1.DMAC[dma_ch].DMACB.bit.CI = 0;

	return FJ_ERR_OK;
}

/**
Start DMA for SPI receive.
*/
static FJ_ERR_CODE gyroif_start_dma_spi_recv( FJ_GYROIF_DATA_CTRL const* const data_ctrl )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	UCHAR				spi_ch = D_BF_GYROIF_SPI_CH;
	UCHAR				dma_ch = D_BF_GYROIF_HDMAC1_CH_SPI_RECV;

	// Open SPI receive DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_SEM_WAIT_POL );
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Open Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		return FJ_ERR_NG;
	}

	// Set SPI receive DMA Ctrl.
	hdmac1_ctrl.config_a.word		= 0;
	hdmac1_ctrl.config_b.word		= 0;
	hdmac1_ctrl.config_a.bit.is		= D_DD_HDMAC1_IS_IDREQ_20;
	hdmac1_ctrl.config_a.bit.bt		= D_DD_HDMAC1_BT_INCR;
	hdmac1_ctrl.config_a.bit.bc		= data_ctrl->data_count - 1;
	hdmac1_ctrl.config_a.bit.tc		= data_ctrl->total_data_count - 1;
//	hdmac1_ctrl.config_b.bit.tt		= 0;
	hdmac1_ctrl.config_b.bit.ms		= D_DD_HDMAC1_MS_BLK;
	if( data_ctrl->data_length > FJ_SPI_LENGTH_8 ){
		hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_HWORD;
	}
	else{
		hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_BYTE;
	}
	hdmac1_ctrl.config_b.bit.fs		= D_DD_HDMAC1_FS_FIX;
	hdmac1_ctrl.config_b.bit.fd		= D_DD_HDMAC1_FD_INCR;
	hdmac1_ctrl.config_b.bit.rc		= D_DD_HDMAC1_RC_ENABLE;
	hdmac1_ctrl.config_b.bit.rs		= D_DD_HDMAC1_RS_DISABLE;
	hdmac1_ctrl.config_b.bit.rd 	= D_DD_HDMAC1_RD_ENABLE;
//	hdmac1_ctrl.config_b.bit.ei		= 1;
//	hdmac1_ctrl.config_b.bit.ci		= 1;
//	hdmac1_ctrl.config_b.bit.ss		= 0;
	hdmac1_ctrl.config_b.bit.sp		= 8;
	hdmac1_ctrl.config_b.bit.dp		= 8;
	hdmac1_ctrl.src_addr			= (ULONG)&(IO_SPI[spi_ch].RXDATA);
	hdmac1_ctrl.dst_addr			= data_ctrl->recv_addr;
	hdmac1_ctrl.int_handler			= (VP_CALLBACK)gyroif_spi_recv_callback;

	// Ctrl SPI receive DMA.
	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, &hdmac1_ctrl );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Ctrl_Common Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	// Start SPI receive DMA.
	ret = Dd_HDMAC1_Start_Async( dma_ch );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Start_Async Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	IO_HDMAC1.DMAC[dma_ch].DMACB.bit.CI = 0;

	return FJ_ERR_OK;
}

/**
Start DMA for SPI contXferExtend=1 set.
*/
static FJ_ERR_CODE gyroif_start_dma_spi_contxfer1( FJ_GYROIF_DATA_CTRL const* const data_ctrl )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	UCHAR				spi_ch = D_BF_GYROIF_SPI_CH;
	UCHAR				dma_ch = D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER1;

	// Open CS control1 DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_SEM_WAIT_POL );
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Open Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		return FJ_ERR_NG;
	}

	// Set CS control1 DMA Ctrl.
	hdmac1_ctrl.config_a.word		= 0;
	hdmac1_ctrl.config_b.word		= 0;
	hdmac1_ctrl.config_a.bit.is		= D_DD_HDMAC1_IS_IDREQ_24;
	hdmac1_ctrl.config_a.bit.bt		= D_DD_HDMAC1_BT_NORMAL;
	hdmac1_ctrl.config_a.bit.bc		= 0;
	hdmac1_ctrl.config_a.bit.tc		= data_ctrl->total_data_count - 1;
//	hdmac1_ctrl.config_b.bit.tt		= 0;
	hdmac1_ctrl.config_b.bit.ms		= D_DD_HDMAC1_MS_BLK;
	hdmac1_ctrl.config_b.bit.tw		= D_DD_HDMAC1_TW_WORD;
	hdmac1_ctrl.config_b.bit.fs		= D_DD_HDMAC1_FS_FIX;
	hdmac1_ctrl.config_b.bit.fd		= D_DD_HDMAC1_FD_FIX;
	hdmac1_ctrl.config_b.bit.rc		= D_DD_HDMAC1_RC_ENABLE;
	hdmac1_ctrl.config_b.bit.rs		= D_DD_HDMAC1_RS_DISABLE;
	hdmac1_ctrl.config_b.bit.rd 	= D_DD_HDMAC1_RD_DISABLE;
//	hdmac1_ctrl.config_b.bit.ei		= 1;
//	hdmac1_ctrl.config_b.bit.ci		= 1;
//	hdmac1_ctrl.config_b.bit.ss		= 0;
	hdmac1_ctrl.config_b.bit.sp		= 8;
	hdmac1_ctrl.config_b.bit.dp		= 8;
	hdmac1_ctrl.src_addr			= (ULONG)&(gAUX_CTRL_valu_1);
	hdmac1_ctrl.dst_addr			= (ULONG)&(IO_SPI[spi_ch].AUX_CTRL);
	hdmac1_ctrl.int_handler			= (VP_CALLBACK)gyroif_spi_contxfer1_callback;

	// Ctrl CS control1 DMA.
	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, &hdmac1_ctrl );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Ctrl_Common Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	// Start CS control1 DMA.
	ret = Dd_HDMAC1_Start_Async( dma_ch );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Start_Async Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	IO_HDMAC1.DMAC[dma_ch].DMACB.bit.CI = 0;

	return FJ_ERR_OK;
}

/**
Start DMA for SPI contXferExtend=0 set.
*/
static FJ_ERR_CODE gyroif_start_dma_spi_contxfer0( FJ_GYROIF_DATA_CTRL const* const data_ctrl )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	UCHAR				spi_ch = D_BF_GYROIF_SPI_CH;
	UCHAR				dma_ch = D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER0;

	// Open CS control2 DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_SEM_WAIT_POL );
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Open Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		return FJ_ERR_NG;
	}

	// Set CS control2 DMA Ctrl.
	hdmac1_ctrl.config_a.word		= 0;
	hdmac1_ctrl.config_b.word		= 0;
	hdmac1_ctrl.config_a.bit.is		= D_DD_HDMAC1_IS_IDREQ_20;
	hdmac1_ctrl.config_a.bit.bt		= D_DD_HDMAC1_BT_NORMAL;
	hdmac1_ctrl.config_a.bit.bc		= 0;
	hdmac1_ctrl.config_a.bit.tc		= data_ctrl->total_data_count - 1;
//	hdmac1_ctrl.config_b.bit.tt		= 0;
	hdmac1_ctrl.config_b.bit.ms		= D_DD_HDMAC1_MS_BLK;
	hdmac1_ctrl.config_b.bit.tw		= D_DD_HDMAC1_TW_WORD;
	hdmac1_ctrl.config_b.bit.fs		= D_DD_HDMAC1_FS_FIX;
	hdmac1_ctrl.config_b.bit.fd		= D_DD_HDMAC1_FD_FIX;
	hdmac1_ctrl.config_b.bit.rc		= D_DD_HDMAC1_RC_ENABLE;
	hdmac1_ctrl.config_b.bit.rs		= D_DD_HDMAC1_RS_DISABLE;
	hdmac1_ctrl.config_b.bit.rd 	= D_DD_HDMAC1_RD_DISABLE;
//	hdmac1_ctrl.config_b.bit.ei		= 1;
//	hdmac1_ctrl.config_b.bit.ci		= 1;
//	hdmac1_ctrl.config_b.bit.ss		= 0;
	hdmac1_ctrl.config_b.bit.sp		= 8;
	hdmac1_ctrl.config_b.bit.dp		= 8;
	hdmac1_ctrl.src_addr			= (ULONG)&(gAUX_CTRL_valu_2);
	hdmac1_ctrl.dst_addr			= (ULONG)&(IO_SPI[spi_ch].AUX_CTRL);
	hdmac1_ctrl.int_handler			= (VP_CALLBACK)gyroif_spi_contxfer0_callback;

	// Ctrl CS control2 DMA.
	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, &hdmac1_ctrl );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Ctrl_Common Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	// Start CS control2 DMA.
	ret = Dd_HDMAC1_Start_Async( dma_ch );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Start_Async Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	gAUX_CTRL_valu_1.word				= IO_SPI[spi_ch].AUX_CTRL.word;
	gAUX_CTRL_valu_1.bit.CONTXFEREXTEND	= 1;
	gAUX_CTRL_valu_2.word				= gAUX_CTRL_valu_1.word;
	gAUX_CTRL_valu_2.bit.CONTXFEREXTEND	= 0;

	BF_L1l2cache_Clean_Flush_Addr( (UINT32)&gAUX_CTRL_valu_1.word, sizeof(gAUX_CTRL_valu_1.word) );
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)&gAUX_CTRL_valu_2.word, sizeof(gAUX_CTRL_valu_2.word) );

	return FJ_ERR_OK;
}

/**
Set SPI Contorl.
*/
static FJ_ERR_CODE gyroif_set_spi( FJ_GYROIF_DATA_CTRL const* const data_ctrl, E_DD_SPI_ENABLE_SIG enable_sig )
{
	INT32			ret;
	T_DD_SPI_CTRL	spi_ctrl;
	UCHAR			spi_ch = D_BF_GYROIF_SPI_CH;
	UCHAR			clk_div;

	// Open SPI
	ret = Dd_SPI_Open( spi_ch, D_DDIM_SEM_WAIT_POL );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_SPI_Open Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		return FJ_ERR_NG;
	}

	// Calculate SPI baudrate
	ret = Dd_SPI_Calculate( data_ctrl->baudrate, &clk_div );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_SPI_Calculate Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		Dd_SPI_Close( spi_ch );
		return FJ_ERR_NG;
	}

	// Set SPI Ctrl
	spi_ctrl.type				= E_DD_SPI_TYPE_MASTER;
	spi_ctrl.mode				= data_ctrl->mode;
	spi_ctrl.enable_sig			= enable_sig;
	spi_ctrl.bit_direction 		= data_ctrl->bit_direction;
	spi_ctrl.bit_length			= data_ctrl->data_length;
	spi_ctrl.clk_div			= clk_div;
	spi_ctrl.delay_en			= 0;
	spi_ctrl.delay_val			= 0;
	spi_ctrl.rx_inhibit_en		= 0;
	spi_ctrl.fifo_wmark_tx		= data_ctrl->fifo_wmark_tx;
	spi_ctrl.fifo_wmark_rx		= data_ctrl->fifo_wmark_rx;
	spi_ctrl.dma_en				= 1;
	spi_ctrl.dma_recv_to		= data_ctrl->dma_recv_to;
	spi_ctrl.ss_info.cont_trans	= data_ctrl->ss_info.cont_trans;
	spi_ctrl.ss_info.ssout[0]	= data_ctrl->ss_info.ssout[0];
	spi_ctrl.ss_info.ssout[1]	= data_ctrl->ss_info.ssout[1];
	spi_ctrl.ss_info.ssout[2]	= data_ctrl->ss_info.ssout[2];
	spi_ctrl.ss_info.ssout[3]	= data_ctrl->ss_info.ssout[3];
	spi_ctrl.ss_info.sspol[0]	= data_ctrl->ss_info.sspol[0];
	spi_ctrl.ss_info.sspol[1]	= data_ctrl->ss_info.sspol[1];
	spi_ctrl.ss_info.sspol[2]	= data_ctrl->ss_info.sspol[2];
	spi_ctrl.ss_info.sspol[3]	= data_ctrl->ss_info.sspol[3];
	spi_ctrl.pcallback			= NULL;
	spi_ctrl.pcallback_ss		= NULL;

	// Call SPI driver
	ret = Dd_SPI_Ctrl( spi_ch, &spi_ctrl );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_SPI_Ctrl Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		Dd_SPI_Close( spi_ch );
		return FJ_ERR_NG;
	}

	gAUX_CTRL_valu_1.word				= IO_SPI[spi_ch].AUX_CTRL.word;
	gAUX_CTRL_valu_1.bit.CONTXFEREXTEND	= 1;
	gAUX_CTRL_valu_2.word				= gAUX_CTRL_valu_1.word;
	gAUX_CTRL_valu_2.bit.CONTXFEREXTEND	= 0;

	BF_L1l2cache_Clean_Flush_Addr( (UINT32)&gAUX_CTRL_valu_1.word, sizeof(gAUX_CTRL_valu_1.word) );
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)&gAUX_CTRL_valu_2.word, sizeof(gAUX_CTRL_valu_2.word) );

	return FJ_ERR_OK;
}

#endif	// for DMP : end

//=========================GyroIF End================================

//=========================GyroCtrl Start================================
// Gyro Data Read Callbak
static VOID gyroctrl_data_read_callbak( FJ_ERR_CODE result )
{
	BF_Debug_Print_Information(("gyroctrl_data_read_callbak\n"));

	if( result != FJ_ERR_OK ){
		GyroIF_stop_read();
	}
}

// Gyro TimeStamp Callbak
static VOID gyroctrl_timestamp_callbak( FJ_ERR_CODE result )
{
	BF_Debug_Print_Information(("gyroctrl_timestamp_callbak\n"));

	if( result != FJ_ERR_OK ){
		GyroIF_stop_read();
	}
}

//=========================GyroCtrl End================================

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
// Write data
VOID Gyro_Write( UCHAR addr, UCHAR data )
{
	gyro_spi_write( addr, data );
}

// Write data
VOID Gyro_Write_N( UCHAR addr, UCHAR* data, UCHAR write_bytes )
{
	gyro_spi_write_n( addr, data, write_bytes );
}

// Read data
VOID Gyro_Read( UCHAR addr, UCHAR* data )
{
	gyro_spi_read( addr, data );
}

// Read data
VOID Gyro_Read_N( UCHAR addr, UCHAR* data, UCHAR read_bytes )
{
	gyro_spi_read_n( addr, data, read_bytes );
}

// Initialize Gyro Sensor
E_CAMERA_ERR_CODE Gyro_Init( VOID )
{
	UCHAR read_data = 0;

#ifdef TIMER64_START_FROM_BOOT
	Dd_TMR64_Time_Stamp_Enable(FALSE);
#endif

	printf("---Gyro Sensor Init Start---\n");

	gyro_spi_set();
	
	// set user bank 0
	Gyro_Write( REG_BNK_SEL, USER_BANK_0 );

	// check device ID
	Gyro_Read( WHO_AM_I, &read_data );
	if( read_data  != D_GYRO_CHIPID ){
		printf("WHO_AM_I[0x%02X] ERROR! : %d\n", read_data, __LINE__);
	}
	else {
		printf("WHO_AM_I[0x%02X]\n", read_data);
	}

	// reset device
	Gyro_Write( PWR_MGMT_1, DEVICE_RESET );

	OS_User_Dly_Tsk( 100 );	// 100ms
	
	// clear sleep mode bit (6), set clock source
	Gyro_Write( PWR_MGMT_1, CLKSEL1 );

	// set user bank 2
	Gyro_Write( REG_BNK_SEL, USER_BANK_2 );
	
	// set gyro sample rate divider to 1
	Gyro_Write( GYRO_SMPLRT_DIV, 0 );

	// set gyro sample rate : 1.125kHz, 3DB BW : 361.4Hz, NBW : 376.5Hz
	Gyro_Write( GYRO_CONFIG_1, DLPFCFG(7) | g_gyro_icm20948_gyro_fs_sel | GYRO_FCHOICE );
	
	// set accel sample rate divider to 1
	Gyro_Write( ACCEL_SMPLRT_DIV_1, 0 );
	Gyro_Write( ACCEL_SMPLRT_DIV_2, 0 );
	
	// enable the WOM logic, WOM algorithm is "compare the previous sample".
	Gyro_Write( ACCEL_INTEL_CTRL, ACCEL_INTEL_MODE_INT | ACCEL_INTEL_EN );
	
	// WOM threshold value : 0mg
	Gyro_Write( ACCEL_WOM_THR, 0 );
	
	// set accel sample rate : 1.125kHz, 3DB BW : 50.4Hz, NBW : 68.8Hz
//	Gyro_Write( ACCEL_CONFIG, DLPFCFG(3) | g_gyro_icm20948_accel_fs_sel | ACCEL_FCHOICE );
	Gyro_Write( ACCEL_CONFIG, DLPFCFG(7) | g_gyro_icm20948_accel_fs_sel | ACCEL_FCHOICE );
	
	// set FSYNC pin : GYRO_XOUT
	Gyro_Write( FSYNC_CONFIG, FSYNC_GYRO_X );


	// set user bank 0
	Gyro_Write( REG_BNK_SEL, USER_BANK_0 );
	
	// set ACCEL and GYRO to duty cycle mode
	Gyro_Write( LP_CONFIG, GYRO_CYCLE | ACCEL_CYCLE );
	
	// clear sleep mode bit (6), set clock source
//	Gyro_Write( PWR_MGMT_1, CLKSEL1 );

	// set FIFO stream mode
	Gyro_Write( FIFO_MODE, FIFO_MODE_STREAM );
	
	// set FIFO configuration
	Gyro_Write( FIFO_CFG, FIFO_CFG_INT_EN );
	
	// trigger interupt when WOM_INT_EN, PLL_RDY_EN appears
	Gyro_Write( INT_ENABLE, WOM_INT_EN | PLL_RDY_EN );
	
	// trigger interupt when INT_RAW_RDY appears
	Gyro_Write( INT_ENABLE_1, RAW_DATA_0_RDY_EN );
	
	// INT: clear on any read and signal FSYNC by interrupt
//	Gyro_Write( INT_PIN_CFG, FSYNC_INT_MODE_EN | INT_ANYRD_2CLEAR | LATCH_INT_EN_PULSE | ACTL_HIGH );
	Gyro_Write( INT_PIN_CFG, INT_ANYRD_2CLEAR | LATCH_INT_EN_PULSE | ACTL_HIGH );
	
	// disable FIFO output
	Gyro_Write( FIFO_EN_2, 0 );

	// enable FIFO operation mode
	Gyro_Write( USER_CTRL, FIFO_EN );

#if 1
	//
	// Read data
	//

	// clear sleep mode bit (6), set clock source
	Gyro_Read( PWR_MGMT_1, &read_data );
	printf("PWR_MGMT_1[0x%02X]\n", read_data);
	
	// set ACCEL and GYRO to duty cycle mode
	Gyro_Read( LP_CONFIG, &read_data );
	printf("LP_CONFIG[0x%02X]\n", read_data);
	
	// clear sleep mode bit (6), set clock source
	Gyro_Read( PWR_MGMT_1, &read_data );
	printf("PWR_MGMT_1[0x%02X]\n", read_data);
	
	// INT: clear on any read by interrupt
	Gyro_Read( INT_PIN_CFG, &read_data );
	printf("INT_PIN_CFG[0x%02X]\n", read_data);
	
	// trigger interupt when WOM_INT_EN, PLL_RDY_EN appears
	Gyro_Read( INT_ENABLE, &read_data );
	printf("INT_ENABLE[0x%02X]\n", read_data);
	
	// trigger interupt when INT_RAW_RDY appears
	Gyro_Read( INT_ENABLE_1, &read_data );
	printf("INT_ENABLE_1[0x%02X]\n", read_data);
	
	// set FIFO stream mode
	Gyro_Read( FIFO_MODE, &read_data );
	printf("FIFO_MODE[0x%02X]\n", read_data);
	
	// set FIFO configuration
	Gyro_Read( FIFO_CFG, &read_data );
	printf("FIFO_CFG[0x%02X]\n", read_data);


	// set user bank 2
	Gyro_Write( REG_BNK_SEL, USER_BANK_2 );
	
	// set gyro sample rate divider to 1
	Gyro_Read( GYRO_SMPLRT_DIV, &read_data );
	printf("GYRO_SMPLRT_DIV[0x%02X]\n", read_data);
	
	// set gyro sample rate : 1.125kHz, 3DB BW : 361.4Hz, NBW : 376.5Hz
	Gyro_Read( GYRO_CONFIG_1, &read_data );
	printf("GYRO_CONFIG_1[0x%02X]\n", read_data);
	
	// enable the WOM logic, WOM algorithm is "compare the previous sample".
	Gyro_Read( ACCEL_INTEL_CTRL, &read_data );
	printf("ACCEL_INTEL_CTRL[0x%02X]\n", read_data);
	
	// WOM threshold value : 4mg
	Gyro_Read( ACCEL_WOM_THR, &read_data );
	printf("ACCEL_WOM_THR[0x%02X]\n", read_data);
	
	// set accel sample rate divider to 1
	Gyro_Read( ACCEL_SMPLRT_DIV_1, &read_data );
	printf("ACCEL_SMPLRT_DIV_1[0x%02X]\n", read_data);

	Gyro_Read( ACCEL_SMPLRT_DIV_2, &read_data );
	printf("ACCEL_SMPLRT_DIV_2[0x%02X]\n", read_data);
	
	// set accel sample rate : 1.125kHz, 3DB BW : 50.4Hz, NBW : 68.8Hz
	Gyro_Read( ACCEL_CONFIG, &read_data );
	printf("ACCEL_CONFIG[0x%02X]\n", read_data);
	
	// set FSYNC pin : GYRO_XOUT
	Gyro_Read( FSYNC_CONFIG, &read_data );
	printf("FSYNC_CONFIG[0x%02X]\n", read_data);
#endif
	// set user bank 0
	Gyro_Write( REG_BNK_SEL, USER_BANK_0 );

	// enable gyro and accelerometer to output to FIFO
	Gyro_Write( FIFO_EN_2, ACCEL_FIFO_EN | GYRO_FIFO_EN );

	gyro_spi_close();

	printf("---Gyro Sensor Init End---\n");

	return E_CAMERA_ERR_OK;
}

/** Calc Gyro Biass */
E_CAMERA_ERR_CODE Gyro_Calc_Bias( ULONG time_ms, T_CAMERA_GYRO_MPU_DATA* bias )
{
	UCHAR	read_data[15] = {0};
	FLOAT	sum_a[3] = {0.0, 0.0, 0.0};
	FLOAT	sum_g[3] = {0.0, 0.0, 0.0};
	
	INT32 x = EIS_X;
	INT32 y = EIS_Y;
	INT32 z = EIS_Z;
	FLOAT sx = 1.0;
	FLOAT sy = 1.0;
	FLOAT sz = 1.0;
	
	INT32	loop;
	INT32	i;
	
	printf("---Gyro Sensor Calc Init Start---\n");
	gyro_spi_set();
	
	if( time_ms ){
		loop = time_ms;
	}else{
		loop = 1;
	}
	// Calc
	for( i = 0; i < loop; i ++ ){
		Gyro_Read_N( ACCEL_OUT, read_data, (sizeof(read_data) / sizeof(read_data[0])) );
		sum_a[0] += Gyro_AccelData2Gravity( read_data[ 1], read_data[ 2] );
		sum_a[1] += Gyro_AccelData2Gravity( read_data[ 3], read_data[ 4] );
		sum_a[2] += Gyro_AccelData2Gravity( read_data[ 5], read_data[ 6] );
		sum_g[0] += Gyro_GyroData2Gravity( read_data[ 7], read_data[ 8] );
		sum_g[1] += Gyro_GyroData2Gravity( read_data[ 9], read_data[10] );
		sum_g[2] += Gyro_GyroData2Gravity( read_data[11], read_data[12] );
		OS_User_Dly_Tsk(1);
	}
	for( i = 0; i < 3; i ++ ){
		sum_g[i] /= loop;
		sum_g[i] *= ( M_PI / 180 );
		sum_a[i] /= loop;
	}
	printf("ACCEL_XOUT[%9.4fG]\n", sum_a[0] );
	printf("ACCEL_YOUT[%9.4fG]\n", sum_a[1] );
	printf("ACCEL_ZOUT[%9.4fG]\n", sum_a[2] );
	printf("GYRO_XOUT [%9.4frad/sec]\n",  sum_g[0] );
	printf("GYRO_YOUT [%9.4frad/sec]\n",  sum_g[1] );
	printf("GYRO_ZOUT [%9.4frad/sec]\n",  sum_g[2] );
	
	// Output
	if( x < 1 ){
		x *= -1;
		sx *= -1.0;
	}
	if( y < 1 ){
		y *= -1;
		sy *= -1.0;
	}
	if( z < 1 ){
		z *= -1;
		sz *= -1.0;
	}
	bias->accel[x-1] = sx * sum_a[0];
	bias->accel[y-1] = sy * sum_a[1];
	bias->accel[z-1] = sz * sum_a[2];
	bias->gyro[x-1] = sx * sum_g[0];
	bias->gyro[y-1] = sy * sum_g[1];
	bias->gyro[z-1] = sz * sum_g[2];
	
	gyro_spi_close();
	printf("---Gyro Sensor Calc Bias End---\n");

	return E_CAMERA_ERR_OK;
}

// Sleep Gyro Sensor
E_CAMERA_ERR_CODE Gyro_Sleep( VOID )
{
	gyro_spi_set();

	// disable interupt
	Gyro_Write( INT_ENABLE_1, 0 );

	// sleep device
	Gyro_Write( PWR_MGMT_1, SLEEP );

	gyro_spi_close();

#ifdef TIMER64_START_FROM_BOOT
	Dd_TMR64_Time_Stamp_Enable(TRUE);
#endif

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Gyro_Resume( VOID )
{
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Gyro_Get_Data(INT32 read_cnt, T_CAMERA_GYRO_MPU_DATA* data )
{
	T_CAMERA_GYRO_MPU_DATA mpu_data;
	INT32 x = EIS_X;
	INT32 y = EIS_Y;
	INT32 z = EIS_Z;
	FLOAT sx = 1.0;
	FLOAT sy = 1.0;
	FLOAT sz = 1.0;

	memset( (VOID*)&mpu_data, 0, (sizeof(T_CAMERA_GYRO_MPU_DATA)) );
	if(read_cnt >= D_GYROCTRL_BUFF_NUM) read_cnt = 0;
	
	if( x < 1 ){
		x *= -1;
		sx *= -1.0;
	}
	if( y < 1 ){
		y *= -1;
		sy *= -1.0;
	}
	if( z < 1 ){
		z *= -1;
		sz *= -1.0;
	}
	
	mpu_data.accel[x-1] = sx * Gyro_AccelData2Gravity( gGyroCtrl_Recv_Data[ read_cnt ][ 1], gGyroCtrl_Recv_Data[ read_cnt ][ 2] );
	mpu_data.accel[y-1] = sy * Gyro_AccelData2Gravity( gGyroCtrl_Recv_Data[ read_cnt ][ 3], gGyroCtrl_Recv_Data[ read_cnt ][ 4] );
	mpu_data.accel[z-1] = sz * Gyro_AccelData2Gravity( gGyroCtrl_Recv_Data[ read_cnt ][ 5], gGyroCtrl_Recv_Data[ read_cnt ][ 6] );
	mpu_data.gyro[x-1] = sx * Gyro_GyroData2Gravity( gGyroCtrl_Recv_Data[ read_cnt ][ 7], gGyroCtrl_Recv_Data[ read_cnt ][ 8] ) * ( M_PI / 180 );
	mpu_data.gyro[y-1] = sy * Gyro_GyroData2Gravity( gGyroCtrl_Recv_Data[ read_cnt ][ 9], gGyroCtrl_Recv_Data[ read_cnt ][10] ) * ( M_PI / 180 );
	mpu_data.gyro[z-1] = sz * Gyro_GyroData2Gravity( gGyroCtrl_Recv_Data[ read_cnt ][11], gGyroCtrl_Recv_Data[ read_cnt ][12] ) * ( M_PI / 180 );

	memcpy( (VOID*)data, &mpu_data, (sizeof(T_CAMERA_GYRO_MPU_DATA)) );

	return E_CAMERA_ERR_OK;
}

FLOAT Gyro_AccelData2Gravity( const UCHAR data_h, const UCHAR data_l )
{
	FLOAT f_val;
	FLOAT accel_1g_count;

	switch( g_gyro_icm20948_accel_fs_sel ) {
		case ACCEL_FS_2G:	// +-2g Full scale.
			accel_1g_count = 16384.0;	// typ.
			break;
		case ACCEL_FS_4G:	// +-4g Full scale.
			accel_1g_count = 8192.0;	// typ.
			break;
		case ACCEL_FS_8G:	// +-8g Full scale.
			accel_1g_count = 4096.0;	// typ.
			break;
		case ACCEL_FS_16G:	// +-16g Full scale.
			accel_1g_count = 2048.0;	// typ.
			break;
		default:
			printf( "%s(): unknown ACCEL_FS_SEL. 0x%x\n", __func__, g_gyro_icm20948_accel_fs_sel );	/* pgr0822 */
			accel_1g_count = 16384.0;	// fail safe
	}

	f_val = gyro_data2float( data_h, data_l );

	return (f_val / accel_1g_count);
}

FLOAT Gyro_GyroData2Gravity( const UCHAR data_h, const UCHAR data_l )
{
	FLOAT f_val;
	FLOAT gyro_1degree_count;

	switch( g_gyro_icm20948_gyro_fs_sel ) {
		case GYRO_FS_250:	// +-250 degree/sec Full scale.
			gyro_1degree_count = 131.0;	// typ.
			break;
		case GYRO_FS_500:	// +-500 degree/sec Full scale.
			gyro_1degree_count = 65.5;	// typ.
			break;
		case GYRO_FS_1000:	// +-1000 degree/sec Full scale.
			gyro_1degree_count = 32.8;	// typ.
			break;
		case GYRO_FS_2000:	// +-2000 degree/sec Full scale.
			gyro_1degree_count = 16.4;	// typ.
			break;
		default:
			printf( "%s(): unknown GYRO_FS_SEL. 0x%x\n", __func__, g_gyro_icm20948_gyro_fs_sel );	/* pgr0822 */
			gyro_1degree_count = 131.0;	// fail safe
	}

	f_val = gyro_data2float( data_h, data_l );

	return (f_val / gyro_1degree_count);
}

VOID Gyro_Dump( VOID )
{
	UCHAR read_data[15] = {0};
	FLOAT data_f;

	// set user bank 0
	Gyro_Write( REG_BNK_SEL, USER_BANK_0 );

	// get ACCEL & GYRO data
	Gyro_Read_N( ACCEL_OUT, read_data, (sizeof(read_data) / sizeof(read_data[0])) );

	data_f = Gyro_AccelData2Gravity( read_data[ 1], read_data[ 2] );
	printf("ACCEL_XOUT[0x%02X%02X=%9.4fG]\n", read_data[ 1], read_data[ 2], data_f );

	data_f = Gyro_AccelData2Gravity( read_data[ 3], read_data[ 4] );
	printf("ACCEL_YOUT[0x%02X%02X=%9.4fG]\n", read_data[ 3], read_data[ 4], data_f );

	data_f = Gyro_AccelData2Gravity( read_data[ 5], read_data[ 6] );
	printf("ACCEL_ZOUT[0x%02X%02X=%9.4fG]\n", read_data[ 5], read_data[ 6], data_f );

	data_f = Gyro_GyroData2Gravity( read_data[ 7], read_data[8] );
	printf("GYRO_XOUT [0x%02X%02X=%9.4fdeg/sec]\n", read_data[ 7], read_data[8], data_f );

	data_f = Gyro_GyroData2Gravity( read_data[9], read_data[10] );
	printf("GYRO_YOUT [0x%02X%02X=%9.4fdeg/sec]\n", read_data[9], read_data[10], data_f );

	data_f = Gyro_GyroData2Gravity( read_data[11], read_data[12] );
	printf("GYRO_ZOUT [0x%02X%02X=%9.4fdeg/sec]\n", read_data[11], read_data[12], data_f );

	printf("TEMP_OUT  [0x%02X%02X]\n", read_data[13], read_data[14] );

}

// for DMP : start
E_CAMERA_ERR_CODE Gyro_Init_DMP( VOID )
{
	E_CAMERA_ERR_CODE ret = E_CAMERA_ERR_OK;
	INT32 rc;
	INT32 ii;
	UINT8 whoami = 0xff;
	struct inv_icm20948_serif icm20948_serif;
	
	printf("---Gyro Sensor Init DMP Start---\n");
#ifdef D_GYRO_DMP_MBA_ON
	MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_START, "Gyro_init"));
#endif
	
	OS_User_Clr_Flg( FID_GYRO_DMP, 0 );
	
	gyro_spi_set();
	
	/* 
	 * Initialize icm20948 serif structure 
	 */
	icm20948_serif.context   = 0; /* no need */
	icm20948_serif.read_reg  = gyro_read_reg;
	icm20948_serif.write_reg = gyro_write_reg;
	icm20948_serif.max_read  = 20; /* maximum number of bytes allowed per serial read */
	icm20948_serif.max_write = 20; /* maximum number of bytes allowed per serial write */
	icm20948_serif.is_spi    = 1;
	
	/* 
	 * Reset icm20948 driver states
	 */
	inv_icm20948_reset_states(&g_gyro_icm_device, &icm20948_serif);
	inv_icm20948_register_aux_compass(&g_gyro_icm_device, INV_ICM20948_COMPASS_ID_AK09911, D_GYRO_AK_I2C_ADDR);
	
	/*
	 * Setup the icm20948 device
	 */
	/*
	 * Just get the whoami
	 */
	rc = inv_icm20948_get_whoami(&g_gyro_icm_device, &whoami);
	if( rc ){
		printf("GYRO READ ERROR![%d] : %d\n", rc, __LINE__);
		ret = E_CAMERA_ERR_NG;
	}
	
	/*
	 * Check if WHOAMI value corresponds to any value from D_GYRO_CHIPID array
	 */
	if( whoami != D_GYRO_CHIPID ){
		printf("WHO_AM_I[0x%02X] ERROR! : %d\n", whoami, __LINE__);
		ret = E_CAMERA_ERR_NG;
	}
	
	/* Setup accel and gyro mounting matrix and associated angle for current board */
	inv_icm20948_init_matrix(&g_gyro_icm_device);
	
	for( ii=0; ii<INV_ICM20948_SENSOR_MAX; ii++ ){
		inv_icm20948_set_matrix(&g_gyro_icm_device, g_gyro_icm_cfg_mounting_matrix, ii);
	}
	/* set default power mode */
	rc = inv_icm20948_initialize(&g_gyro_icm_device, g_gyro_icm_dmp3_image, sizeof(g_gyro_icm_dmp3_image));
	if( rc ){
		printf("GYRO DMP INITIALIZE ERROR![%d] : %d\n", rc, __LINE__);
		ret = E_CAMERA_ERR_NG;
	}
	
	/* Initialize auxiliary sensors */
	inv_icm20948_register_aux_compass( &g_gyro_icm_device, INV_ICM20948_COMPASS_ID_AK09916, D_GYRO_AK_I2C_ADDR );
	inv_icm20948_initialize_auxiliary( &g_gyro_icm_device );
	
	inv_icm20948_set_fsr(&g_gyro_icm_device, INV_ICM20948_SENSOR_RAW_ACCELEROMETER, (const VOID *)&g_gyro_icm_cfg_acc_fsr);
	inv_icm20948_set_fsr(&g_gyro_icm_device, INV_ICM20948_SENSOR_ACCELEROMETER, (const VOID *)&g_gyro_icm_cfg_acc_fsr);
	inv_icm20948_set_fsr(&g_gyro_icm_device, INV_ICM20948_SENSOR_RAW_GYROSCOPE, (const VOID *)&g_gyro_icm_cfg_gyr_fsr);
	inv_icm20948_set_fsr(&g_gyro_icm_device, INV_ICM20948_SENSOR_GYROSCOPE, (const VOID *)&g_gyro_icm_cfg_gyr_fsr);
	inv_icm20948_set_fsr(&g_gyro_icm_device, INV_ICM20948_SENSOR_GYROSCOPE_UNCALIBRATED, (const VOID *)&g_gyro_icm_cfg_gyr_fsr);
	
	/* re-initialize base state structure */
	inv_icm20948_init_structure(&g_gyro_icm_device);
	
#ifdef D_GYRO_DMP_MBA_ON
	MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_END, "Gyro_init"));
	MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_START, "DMP_Load"));
#endif
	/*
	 * Now that Icm20948 device was initialized, we can proceed with DMP image loading
	 * This step is mandatory as DMP image are not store in non volatile memory
	 */
	rc = inv_icm20948_load(&g_gyro_icm_device, g_gyro_icm_dmp3_image, sizeof(g_gyro_icm_dmp3_image));
	if( rc ){
		printf("GYRO DMP DOWNLOAD ERROR![%d] : %d\n", rc, __LINE__);
		ret = E_CAMERA_ERR_NG;
	}
	
#ifdef D_GYRO_DMP_MBA_ON
	MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_END, "DMP_Load"));
#endif
	/*
	 * Sensor start
	 */
	inv_icm20948_enable_sensor(&g_gyro_icm_device, INV_ICM20948_SENSOR_ROTATION_VECTOR, 1);		// quaternion
	inv_icm20948_enable_sensor(&g_gyro_icm_device, INV_ICM20948_SENSOR_ORIENTATION, 1);			// 
	inv_icm20948_enable_sensor(&g_gyro_icm_device, INV_ICM20948_SENSOR_ACCELEROMETER, 1);		// accel
	inv_icm20948_enable_sensor(&g_gyro_icm_device, INV_ICM20948_SENSOR_GYROSCOPE, 1);			// gyro
	inv_icm20948_enable_sensor(&g_gyro_icm_device, INV_ICM20948_SENSOR_GEOMAGNETIC_FIELD, 1);	// mag
	
	OS_User_Sta_Tsk(TID_GYRO_DMP, 0);
	
	gyro_spi_close();
	
	printf("---Gyro Sensor Init DMP End---\n");
	return ret;
}

E_CAMERA_ERR_CODE Gyro_Sleep_DMP( VOID )
{
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Gyro_Resume_DMP( VOID )
{
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Gyro_Get_Data_DMP(INT32 read_cnt, T_CAMERA_GYRO_MPU_DATA* data )
{
#ifdef D_GYRO_DMP_MBA_ON
	MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_START, "DMP_G"));
#endif
	memcpy( (VOID*)data, &gGyro_Dmp_Raw_Data, (sizeof(T_CAMERA_GYRO_MPU_DATA)) );
#ifdef D_GYRO_DMP_MBA_ON
	MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_END, "DMP_G"));
#endif
	return E_CAMERA_ERR_OK;
}

VOID Gyro_Main_DMP( INT32 stacd )
{
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flgptn = 0;
	
	while(1){
		ercd = OS_User_Wai_Flg( FID_GYRO_DMP, (FLG_GYRO_DMP_INT|FLG_GYRO_DMP_STOP_REQ), D_OS_USER_TWF_ORW, &flgptn );
		if( ercd != D_OS_USER_E_OK ){
			printf("DMP Task Wai_Flg error %d\n", ercd);
		}
		if( flgptn & FLG_GYRO_DMP_STOP_REQ ){
#ifdef D_GYRO_DMP_MBA_ON
			MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_POINT, "DMP_STOP"));
#endif
			break;
		}
		OS_User_Clr_Flg( FID_GYRO_DMP, ~FLG_GYRO_DMP_INT );
		
#ifdef D_GYRO_DMP_MBA_ON
		MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_START, "DMP"));
#endif
		
		inv_icm20948_poll_sensor(&g_gyro_icm_device, (void *)0, inv_icm20948_build_sensor_event_data);
		
#ifdef D_GYRO_DMP_MBA_ON
		MPRINTF((D_MBA_KIND_HOST_DEBUG, FJ_MBALOG_LINE_ID_GYRO, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_END, "DMP"));
#endif
	}
	
	OS_User_Ext_Tsk();
}

// for DMP : end

//=========================GyroIF Start================================
/**
Start Gyro data read.
*/
E_CAMERA_ERR_CODE GyroIF_start_read( VOID )
{
	FJ_ERR_CODE	err_code = FJ_ERR_OK;

	gGyroCtrl_Send_Data[0] = D_GYRO_SPI_READ_OP | ACCEL_OUT;	// R/W bit and Register address for Gyro read.
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN, (UINT32)gGyroCtrl_Send_Data, gGyroCtrl_BufSize[E_CAMERA_GYRO_BUF_SIZE_SEND] );
	
	gGyroIF_Read_Ctrl = gGyroCtrl_Data;

	IO_HDMAC1.DMACR.bit.PR = 0;		// If Block Transfer Mode and set the contXferExtend, PR = 0 is mandatory setting.

#ifdef D_GYROIF_DEBUG
	IO_CHIPTOP.DDR.bit.P75 = 1;
	IO_CHIPTOP.PDR.word[0] = 0x20000000;
#endif

#ifndef TIMER64_START_FROM_BOOT
	// Start TMR64
	err_code = gyroif_start_tmr64( &gGyroCtrl_Data.stamp_ctrl );
	if( err_code != FJ_ERR_OK ){
		printf("GyroIF_start_read gyroif_start_tmr64 error %d\n", err_code);
		return E_CAMERA_ERR_NG;
	}
#endif

	// Start DMA for TMR64
	err_code = gyroif_start_dma_tmr64( &gGyroCtrl_Data.stamp_ctrl );
	if( err_code != FJ_ERR_OK ){
		gyroif_stop_tmr64();
		gyroif_stop_spi();
		printf("GyroIF_start_read gyroif_start_dma_tmr64 error %d\n", err_code);
		return E_CAMERA_ERR_NG;
	}

#ifdef D_GYRO_DMP_OFF	// for DMP : start
		// Set SPI Control(EXT0).
		err_code = gyroif_set_spi( &gGyroCtrl_Data.data_ctrl, E_DD_SPI_ENABLE_SIG_CPU );
		if( err_code != FJ_ERR_OK ){
			gyroif_stop_tmr64();
			return err_code;
		}
	
		// Start DMA for SPI receive.
		err_code = gyroif_start_dma_spi_recv( &gGyroCtrl_Data.data_ctrl );
		if( err_code != FJ_ERR_OK ){
			gyroif_stop_tmr64();
			gyroif_stop_spi();
			gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_TMR64 );
			gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_SEND );
			return err_code;
		}
	
		// Start DMA for SPI send.
		err_code = gyroif_start_dma_spi_send( &gGyroCtrl_Data.data_ctrl );
		if( err_code != FJ_ERR_OK ){
			gyroif_stop_tmr64();
			gyroif_stop_spi();
			gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_TMR64 );
			return err_code;
		}
	
		// Start DMA for SPI contXferExtend=1 set.
		err_code = gyroif_start_dma_spi_contxfer1( &gGyroCtrl_Data.data_ctrl );
		if( err_code != FJ_ERR_OK ){
			gyroif_stop_tmr64();
			gyroif_stop_spi();
			gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_TMR64 );
			gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_SEND );
			gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_RECV );
			return err_code;
		}
	
		// Start DMA for SPI CS contXferExtend=0 set.
		err_code = gyroif_start_dma_spi_contxfer0( &gGyroCtrl_Data.data_ctrl );
		if( err_code != FJ_ERR_OK ){
			gyroif_stop_tmr64();
			gyroif_stop_spi();
			gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_TMR64 );
			gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_SEND );
			gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_RECV );
			gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER1 );
			return err_code;
		}
#endif	// for DMP : end

	gGyroIF_Stop_Flag = FALSE;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	// Disable input Gyro INT
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PB4, 0);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	// Enable SPI
	Dd_SPI_Set_SPI_Enable( D_BF_GYROIF_SPI_CH, E_DD_SPI_ENABLE_SIG_CPU, TRUE );
	// Enable Timestamp
	Dd_TMR64_Time_Stamp_Enable(TRUE);

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	// Enable input Gyro INT
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PB4, 1);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	return E_CAMERA_ERR_OK;
}

/**
Stopt Gyro data read.
*/
E_CAMERA_ERR_CODE GyroIF_stop_read( VOID )
{
	FJ_ERR_CODE err_code = FJ_ERR_OK;
	E_CAMERA_ERR_CODE ret_code = E_CAMERA_ERR_OK;

	gGyroIF_Stop_Flag = TRUE;

#ifdef TIMER64_START_FROM_BOOT
	Dd_TMR64_Time_Stamp_Enable(FALSE);
#else
	err_code = gyroif_stop_tmr64();
	if( err_code != FJ_ERR_OK ){
		ret_code = E_CAMERA_ERR_NG;
	}
#endif

	err_code = gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_TMR64 );
	if( err_code != FJ_ERR_OK ){
		ret_code = E_CAMERA_ERR_NG;
	}

#ifdef D_GYRO_DMP_OFF	// for DMP : start
	err_code = gyroif_stop_spi();
	if( err_code != FJ_ERR_OK ){
		ret_code = E_CAMERA_ERR_NG;
	}

	err_code = gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_SEND );
	if( err_code != FJ_ERR_OK ){
		ret_code = E_CAMERA_ERR_NG;
	}

	err_code = gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_RECV );
	if( err_code != FJ_ERR_OK ){
		ret_code = E_CAMERA_ERR_NG;
	}

	err_code = gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER1 );
	if( err_code != FJ_ERR_OK ){
		ret_code = E_CAMERA_ERR_NG;
	}

	err_code = gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER0 );
	if( err_code != FJ_ERR_OK ){
		ret_code = E_CAMERA_ERR_NG;
	}
#endif	// for DMP : end

	return ret_code;
}

/**
Get Gyro data read count.
*/
E_CAMERA_ERR_CODE GyroIF_get_read_count( UINT32* read_count )
{
#if 1
	INT32	ret;
#ifdef D_GYROIF_DEBUG
	ULONG	tc_recv, tc_send, tc_stmp;
#else
	ULONG	tc_recv;
#endif

	// Get TC of receive SPI DMA
	ret = Dd_HDMAC1_Get_Trans_Count( D_BF_GYROIF_HDMAC1_CH_SPI_RECV, &tc_recv );
	if( ret != D_DDIM_OK ) {
		return E_CAMERA_ERR_NG;
	}

	*read_count = gGyroIF_Read_Ctrl.data_ctrl.total_data_count - (tc_recv + 1);

#ifdef D_GYROIF_DEBUG
	tc_send = IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_SPI_SEND].DMACA.bit.TC;
	tc_stmp = IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_TMR64].DMACA.bit.TC;
	if( (tc_recv != tc_send) || (tc_recv != tc_stmp) || (tc_send != tc_stmp) ){
		printf("tc_recv=%d, tc_send=%d, tc_stmp=%d\n", tc_recv, tc_send, tc_stmp);
	}
#endif
#else
	*read_count = gBF_GyroIF_SPI_Recv_Count;
#endif

	return E_CAMERA_ERR_OK;
}

//=========================GyroIF End================================

#endif	/* GYRO_ICM20948 */

