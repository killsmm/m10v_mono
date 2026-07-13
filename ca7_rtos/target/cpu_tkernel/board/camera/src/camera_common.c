/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "camera_common.h"
#include "fj_peripheral.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define D_DBG_SPI_PRINT

#define D_SENSOR_XCE_GPIO		(E_DD_TOP_GPIO_PD3)

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
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

E_CAMERA_ERR_CODE Camera_Common_SPI_Write(FJ_PERI_CH ch, USHORT address,const  UCHAR* data, UCHAR size, BOOL is_cs_gpio)
{
	FJ_SPI_CTRL				spi_ctrl = {0};
	FJ_SPI_CTRL_TRANSFER	trans_ctrl;
	UCHAR					send_buf[16+3];
	UINT32					cnt;
	
	// error check
	if( size > D_CAMEARA_ACCESS_SIZE_MAX ){
		return E_CAMERA_ERR_NG;
	}
	
	// set send data
	send_buf[0] = 0x81;//chip_id
	send_buf[1] = (UCHAR)(address>>8);
	send_buf[2] = (UCHAR)(address&0xFF);
	for(cnt=0; cnt<size; cnt++){
		send_buf[3+cnt] = data[cnt];
	}
	
	#ifdef D_DBG_SPI_PRINT
	printf("\n SPI W Address:0x%04x, Size:%d", address, size);
	for(cnt=0; cnt<size; cnt++){
		printf(" Data[%d]:0x%02x", cnt, data[cnt]);
	}
	printf("\n");
	#endif
	
	// set driver
	CAMERA_PRINT_ERROR( FJ_SPI_Open( ch ) );
	
	spi_ctrl.type			= FJ_SPI_TYPE_MASTER;
	spi_ctrl.mode			= FJ_SPI_MODE_3_CPOL1_CPHA1;
	spi_ctrl.bit_direction	= FJ_SPI_BIT_DIR_LSB_FIRST;
	spi_ctrl.data_length	= FJ_SPI_LENGTH_8;
	spi_ctrl.baudrate		= 36000000;
	spi_ctrl.fifo_wmark_tx	= 128;
	spi_ctrl.fifo_wmark_rx	= 128;
	spi_ctrl.dma_en			= 0;
	spi_ctrl.dma_recv_to	= 0;
	spi_ctrl.pcallback		= NULL;
	CAMERA_PRINT_ERROR( FJ_SPI_Ctrl( ch, &spi_ctrl ) );
	
	// Wait for more than 400 nano sec.	// ### MILB_Porting 
	Dd_ARM_Wait_ns( 400 );

	// Transfer process
	trans_ctrl.direction			= FJ_SPI_DATA_DIR_SEND;
	trans_ctrl.data_ptr				= send_buf;
	trans_ctrl.data_count			= 3 + size;
	trans_ctrl.ss_info.cont_trans	= FJ_SPI_CONT_TRANS_ACT_FIFO_EMPTY;
	trans_ctrl.ss_info.ssout[0]		= 1;									// SPI1CS0X
	trans_ctrl.ss_info.sspol[0]		= 0;									// active low
	// does not use ssout[1~3]
	trans_ctrl.ss_info.ssout[1]		= 0;
	trans_ctrl.ss_info.ssout[2]		= 0;
	trans_ctrl.ss_info.ssout[3]		= 0;
	trans_ctrl.ss_info.sspol[1]		= 0;
	trans_ctrl.ss_info.sspol[2]		= 0;
	trans_ctrl.ss_info.sspol[3]		= 0;
	
	if( is_cs_gpio ){
		Dd_Top_Set_Gpio_Status(D_SENSOR_XCE_GPIO, D_DD_TOP_GPIO_STATUS_LOW);
	}
	
	CAMERA_PRINT_ERROR( FJ_SPI_Transfer(ch, &trans_ctrl, FJ_USIO_DMA_CH_NOT_USE) );
	
	if( is_cs_gpio ){
		Dd_Top_Set_Gpio_Status(D_SENSOR_XCE_GPIO, D_DD_TOP_GPIO_STATUS_HIGH);
	}
	
	CAMERA_PRINT_ERROR( FJ_SPI_Stop( ch ) );
	CAMERA_PRINT_ERROR( FJ_SPI_Close( ch ) );
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Camera_Common_SPI_Read(FJ_PERI_CH ch, USHORT address, UCHAR* data, UCHAR size, BOOL is_cs_gpio)
{
	FJ_SPI_CTRL					spi_ctrl = {0};
	FJ_SPI_CTRL_FULL_DUPLEX		trans_ctrl;
	UCHAR						send_buf[D_CAMEARA_ACCESS_SIZE_MAX+3];
	UCHAR						recv_buf[D_CAMEARA_ACCESS_SIZE_MAX+3];
	UINT32						cnt;
	
	// error check
	if( size > D_CAMEARA_ACCESS_SIZE_MAX ){
		return E_CAMERA_ERR_NG;
	}
	
	#ifdef D_DBG_SPI_PRINT
	printf("\n SPI R Address:0x%04x, Size:%d", address, size);
	#endif
	
	// set send data
	send_buf[0] = 0x82;	//chip_id
	send_buf[1] = (UCHAR)(address>>8);
	send_buf[2] = (UCHAR)(address&0xFF);
	
	// set driver
	CAMERA_PRINT_ERROR( FJ_SPI_Open( ch ) );
	
	spi_ctrl.type			= FJ_SPI_TYPE_MASTER;
	spi_ctrl.mode			= FJ_SPI_MODE_3_CPOL1_CPHA1;
	spi_ctrl.bit_direction	= FJ_SPI_BIT_DIR_LSB_FIRST;
	spi_ctrl.data_length	= FJ_SPI_LENGTH_8;
	//spi_ctrl.baudrate		= 36000000;						// Max=36MHz
	spi_ctrl.baudrate		= 18000000;						// Max=36MHz
	spi_ctrl.fifo_wmark_tx	= 128;
	spi_ctrl.fifo_wmark_rx	= 128;
	spi_ctrl.dma_en			= 0;
	spi_ctrl.dma_recv_to	= 0;
	spi_ctrl.pcallback		= NULL;
	CAMERA_PRINT_ERROR( FJ_SPI_Ctrl( ch, &spi_ctrl ) );
	
	// Wait for more than 400 nano sec.	// ### MILB_Porting 
	Dd_ARM_Wait_ns( 400 );

	// Transfer process
	trans_ctrl.send_ptr				= send_buf;
	trans_ctrl.recv_ptr				= recv_buf;
	trans_ctrl.data_count			= 3 + size;
	trans_ctrl.ss_info.cont_trans	= FJ_SPI_CONT_TRANS_ACT_FIFO_EMPTY;
	trans_ctrl.ss_info.ssout[0]		= 1;									// SPI1CS0X
	trans_ctrl.ss_info.sspol[0]		= 0;									// active low
	// does not use ssout[1~3]
	trans_ctrl.ss_info.ssout[1]		= 0;
	trans_ctrl.ss_info.ssout[2]		= 0;
	trans_ctrl.ss_info.ssout[3]		= 0;
	trans_ctrl.ss_info.sspol[1]		= 0;
	trans_ctrl.ss_info.sspol[2]		= 0;
	trans_ctrl.ss_info.sspol[3]		= 0;
	
	if( is_cs_gpio ){
		Dd_Top_Set_Gpio_Status(D_SENSOR_XCE_GPIO, D_DD_TOP_GPIO_STATUS_LOW);
	}
	
	CAMERA_PRINT_ERROR( FJ_SPI_Full_Duplex(ch, &trans_ctrl) );
	
	if( is_cs_gpio ){
		Dd_Top_Set_Gpio_Status(D_SENSOR_XCE_GPIO, D_DD_TOP_GPIO_STATUS_HIGH);
	}
	
	CAMERA_PRINT_ERROR( FJ_SPI_Stop( ch ) );
	CAMERA_PRINT_ERROR( FJ_SPI_Close( ch ) );
	
	// get data
	for(cnt=0; cnt<size; cnt++){
		data[cnt] = recv_buf[3+cnt];
	}

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Camera_Common_SPI_Write_Ex(UCHAR chipid, FJ_PERI_CH ch, UCHAR address, const  UCHAR* data, UCHAR size, BOOL is_cs_gpio)
{
	FJ_SPI_CTRL				spi_ctrl = {0};
	FJ_SPI_CTRL_TRANSFER	trans_ctrl;
	UCHAR					send_buf[16+2];
	UINT32					cnt;

	// error check
	if( size > D_CAMEARA_ACCESS_SIZE_MAX ){
		return E_CAMERA_ERR_NG;
	}

	// set send data
	send_buf[0] = chipid;
	send_buf[1] = address;
	for(cnt=0; cnt<size; cnt++){
		send_buf[2+cnt] = data[cnt];
	}

	#ifdef D_DBG_SPI_PRINT
	printf("\n SPI W Address:0x%04x, Size:%d", address, size);
	for(cnt=0; cnt<size; cnt++){
		printf(" Data[%d]:0x%02x", cnt, data[cnt]);
	}
	printf("\n");
	#endif

	// set driver
	CAMERA_PRINT_ERROR( FJ_SPI_Open( ch ) );

	spi_ctrl.type			= FJ_SPI_TYPE_MASTER;
	spi_ctrl.mode			= FJ_SPI_MODE_3_CPOL1_CPHA1;
	spi_ctrl.bit_direction	= FJ_SPI_BIT_DIR_LSB_FIRST;
	spi_ctrl.data_length	= FJ_SPI_LENGTH_8;
	spi_ctrl.baudrate		= 18000000;
	spi_ctrl.fifo_wmark_tx	= 128;
	spi_ctrl.fifo_wmark_rx	= 128;
	spi_ctrl.dma_en			= 0;
	spi_ctrl.dma_recv_to	= 0;
	spi_ctrl.pcallback		= NULL;
	CAMERA_PRINT_ERROR( FJ_SPI_Ctrl( ch, &spi_ctrl ) );

	// Wait for more than 400 nano sec.	// ### MILB_Porting
	Dd_ARM_Wait_ns( 400 );

	// Transfer process
	trans_ctrl.direction			= FJ_SPI_DATA_DIR_SEND;
	trans_ctrl.data_ptr				= send_buf;
	trans_ctrl.data_count			= 2 + size;
	trans_ctrl.ss_info.cont_trans	= FJ_SPI_CONT_TRANS_ACT_FIFO_EMPTY;
	trans_ctrl.ss_info.ssout[0]		= 1;									// SPI1CS0X
	trans_ctrl.ss_info.sspol[0]		= 0;									// active low
	// does not use ssout[1~3]
	trans_ctrl.ss_info.ssout[1]		= 0;
	trans_ctrl.ss_info.ssout[2]		= 0;
	trans_ctrl.ss_info.ssout[3]		= 0;
	trans_ctrl.ss_info.sspol[1]		= 0;
	trans_ctrl.ss_info.sspol[2]		= 0;
	trans_ctrl.ss_info.sspol[3]		= 0;

	if( is_cs_gpio ){
		Dd_Top_Set_Gpio_Status(D_SENSOR_XCE_GPIO, D_DD_TOP_GPIO_STATUS_LOW);
	}

	CAMERA_PRINT_ERROR( FJ_SPI_Transfer(ch, &trans_ctrl, FJ_USIO_DMA_CH_NOT_USE) );

	if( is_cs_gpio ){
		Dd_Top_Set_Gpio_Status(D_SENSOR_XCE_GPIO, D_DD_TOP_GPIO_STATUS_HIGH);
	}

	CAMERA_PRINT_ERROR( FJ_SPI_Stop( ch ) );
	CAMERA_PRINT_ERROR( FJ_SPI_Close( ch ) );

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Camera_Common_SPI_Read_Ex(UCHAR chipid, FJ_PERI_CH ch, UCHAR address, UCHAR* data, UCHAR size, BOOL is_cs_gpio)
{
	FJ_SPI_CTRL					spi_ctrl = {0};
	FJ_SPI_CTRL_FULL_DUPLEX		trans_ctrl;
	UCHAR						send_buf[D_CAMEARA_ACCESS_SIZE_MAX+2];
	UCHAR						recv_buf[D_CAMEARA_ACCESS_SIZE_MAX+2];
	UINT32						cnt;

	// error check
	if( size > D_CAMEARA_ACCESS_SIZE_MAX ){
		return E_CAMERA_ERR_NG;
	}

	#ifdef D_DBG_SPI_PRINT
	printf("\n SPI R Address:0x%04x, Size:%d", address, size);
	#endif

	// set send data
	send_buf[0] = chipid;
	send_buf[1] = address;

	// set driver
	CAMERA_PRINT_ERROR( FJ_SPI_Open( ch ) );

	spi_ctrl.type			= FJ_SPI_TYPE_MASTER;
	spi_ctrl.mode			= FJ_SPI_MODE_3_CPOL1_CPHA1;
	spi_ctrl.bit_direction	= FJ_SPI_BIT_DIR_LSB_FIRST;
	spi_ctrl.data_length	= FJ_SPI_LENGTH_8;
	//spi_ctrl.baudrate		= 36000000;						// Max=36MHz
	spi_ctrl.baudrate		= 18000000;						// Max=36MHz
	spi_ctrl.fifo_wmark_tx	= 128;
	spi_ctrl.fifo_wmark_rx	= 128;
	spi_ctrl.dma_en			= 0;
	spi_ctrl.dma_recv_to	= 0;
	spi_ctrl.pcallback		= NULL;
	CAMERA_PRINT_ERROR( FJ_SPI_Ctrl( ch, &spi_ctrl ) );

	// Wait for more than 400 nano sec.	// ### MILB_Porting
	Dd_ARM_Wait_ns( 400 );

	// Transfer process
	trans_ctrl.send_ptr				= send_buf;
	trans_ctrl.recv_ptr				= recv_buf;
	trans_ctrl.data_count			= 2 + size;
	trans_ctrl.ss_info.cont_trans	= FJ_SPI_CONT_TRANS_ACT_FIFO_EMPTY;
	trans_ctrl.ss_info.ssout[0]		= 1;									// SPI1CS0X
	trans_ctrl.ss_info.sspol[0]		= 0;									// active low
	// does not use ssout[1~3]
	trans_ctrl.ss_info.ssout[1]		= 0;
	trans_ctrl.ss_info.ssout[2]		= 0;
	trans_ctrl.ss_info.ssout[3]		= 0;
	trans_ctrl.ss_info.sspol[1]		= 0;
	trans_ctrl.ss_info.sspol[2]		= 0;
	trans_ctrl.ss_info.sspol[3]		= 0;

	if( is_cs_gpio ){
		Dd_Top_Set_Gpio_Status(D_SENSOR_XCE_GPIO, D_DD_TOP_GPIO_STATUS_LOW);
	}

	CAMERA_PRINT_ERROR( FJ_SPI_Full_Duplex(ch, &trans_ctrl) );

	if( is_cs_gpio ){
		Dd_Top_Set_Gpio_Status(D_SENSOR_XCE_GPIO, D_DD_TOP_GPIO_STATUS_HIGH);
	}

	CAMERA_PRINT_ERROR( FJ_SPI_Stop( ch ) );
	CAMERA_PRINT_ERROR( FJ_SPI_Close( ch ) );

	// get data
	for(cnt=0; cnt<size; cnt++){
		data[cnt] = recv_buf[2+cnt];
	}

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Camera_Common_I2C_Write(FJ_PERI_CH ch, UCHAR slave7bit, USHORT address, UCHAR* data, UCHAR size)
{
	FJ_I2C_CTRL				ctrl;
	FJ_I2C_TRANSFER_DATA	trans;
	UCHAR					address_buf[2];
	E_CAMERA_ERR_CODE		ret = E_CAMERA_ERR_OK;
	
	// error check
	if( size > D_CAMEARA_ACCESS_SIZE_MAX ){
		return E_CAMERA_ERR_NG;
	}
	
	CAMERA_PRINT_ERROR( FJ_I2C_Open(ch) );
	
	ctrl.type				= FJ_I2C_TYPE_MASTER;
	ctrl.speed				= FJ_I2C_SPEED_FAST;
	ctrl.address_format		= FJ_I2C_ADDRESS_FORMAT_7_BIT;
	ctrl.slave_address		= slave7bit;
	CAMERA_PRINT_ERROR( FJ_I2C_Ctrl(ch, ctrl, NULL) );
	
	address_buf[0] = (UCHAR)(address>>8);
	address_buf[1] = (UCHAR)(address&0xFF);
	trans.slave_register_address.data_length	= 2;
	trans.slave_register_address.data			= address_buf;
	trans.trn_data.data_length					= size;
	trans.trn_data.data							= data;
	trans.dirction								= 0;
	trans.timeout_threshold						= 3; //ms
	trans.retry_num								= 3;
	if( FJ_I2C_Transfer(ch, &trans, 1) != FJ_ERR_OK ){
		ret = E_CAMERA_ERR_NG;
	}
	
	CAMERA_PRINT_ERROR( FJ_I2C_Close(ch) );
	
	return ret;
}

E_CAMERA_ERR_CODE Camera_Common_I2C_Read(FJ_PERI_CH ch, UCHAR slave7bit, USHORT address, UCHAR* data, UCHAR size)
{
	FJ_I2C_CTRL				ctrl;
	FJ_I2C_TRANSFER_DATA	trans;
	UCHAR					address_buf[2];
	E_CAMERA_ERR_CODE		ret = E_CAMERA_ERR_OK;
	
	// error check
	if( size > D_CAMEARA_ACCESS_SIZE_MAX ){
		return E_CAMERA_ERR_NG;
	}
	
	CAMERA_PRINT_ERROR( FJ_I2C_Open(ch) );
	
	ctrl.type				= FJ_I2C_TYPE_MASTER;
	ctrl.speed				= FJ_I2C_SPEED_FAST;
	ctrl.address_format		= FJ_I2C_ADDRESS_FORMAT_7_BIT;
	ctrl.slave_address		= slave7bit;
	CAMERA_PRINT_ERROR( FJ_I2C_Ctrl(ch, ctrl, NULL) );
	
	address_buf[0] = (UCHAR)(address>>8);
	address_buf[1] = (UCHAR)(address&0xFF);
	trans.slave_register_address.data_length	= 2;
	trans.slave_register_address.data			= address_buf;
	trans.trn_data.data_length					= size;
	trans.trn_data.data							= data;
	trans.dirction								= 1;
	trans.timeout_threshold						= 3; //ms
	trans.retry_num								= 3;
	if( FJ_I2C_Transfer(ch, &trans, 1) != FJ_ERR_OK ){
		ret = E_CAMERA_ERR_NG;
	}
	
	CAMERA_PRINT_ERROR( FJ_I2C_Close(ch) );
	
	return ret;
}

E_CAMERA_ERR_CODE Camera_Common_I2C_Write_2(FJ_PERI_CH ch, UCHAR slave7bit, UCHAR address, UCHAR* data, UCHAR size)
{
	FJ_I2C_CTRL				ctrl;
	FJ_I2C_TRANSFER_DATA	trans;
	UCHAR					address_buf[2];
	E_CAMERA_ERR_CODE		ret = E_CAMERA_ERR_OK;
	
	// error check
	if( size > D_CAMEARA_ACCESS_SIZE_MAX ){
		return E_CAMERA_ERR_NG;
	}
	
	CAMERA_PRINT_ERROR( FJ_I2C_Open(ch) );
	
	ctrl.type				= FJ_I2C_TYPE_MASTER;
	ctrl.speed				= FJ_I2C_SPEED_FAST;
	ctrl.address_format		= FJ_I2C_ADDRESS_FORMAT_7_BIT;
	ctrl.slave_address		= slave7bit;
	CAMERA_PRINT_ERROR( FJ_I2C_Ctrl(ch, ctrl, NULL) );
	
	address_buf[0] = address;
	trans.slave_register_address.data_length	= 1;
	trans.slave_register_address.data			= address_buf;
	trans.trn_data.data_length					= size;
	trans.trn_data.data							= data;
	trans.dirction								= 0;
	trans.timeout_threshold						= 3; //ms
	trans.retry_num								= 3;
	if( FJ_I2C_Transfer(ch, &trans, 1) != FJ_ERR_OK ){
		ret = E_CAMERA_ERR_NG;
	}
	
	CAMERA_PRINT_ERROR( FJ_I2C_Close(ch) );
	
	return ret;
}

E_CAMERA_ERR_CODE Camera_Common_I2C_Read_2(FJ_PERI_CH ch, UCHAR slave7bit, UCHAR address, UCHAR* data, UCHAR size)
{
	FJ_I2C_CTRL				ctrl;
	FJ_I2C_TRANSFER_DATA	trans;
	UCHAR					address_buf[2];
	E_CAMERA_ERR_CODE		ret = E_CAMERA_ERR_OK;
	
	// error check
	if( size > D_CAMEARA_ACCESS_SIZE_MAX ){
		return E_CAMERA_ERR_NG;
	}
	
	CAMERA_PRINT_ERROR( FJ_I2C_Open(ch) );
	
	ctrl.type				= FJ_I2C_TYPE_MASTER;
	ctrl.speed				= FJ_I2C_SPEED_FAST;
	ctrl.address_format		= FJ_I2C_ADDRESS_FORMAT_7_BIT;
	ctrl.slave_address		= slave7bit;
	CAMERA_PRINT_ERROR( FJ_I2C_Ctrl(ch, ctrl, NULL) );
	
	address_buf[0] = address;
	trans.slave_register_address.data_length	= 1;
	trans.slave_register_address.data			= address_buf;
	trans.trn_data.data_length					= size;
	trans.trn_data.data							= data;
	trans.dirction								= 1;
	trans.timeout_threshold						= 3; //ms
	trans.retry_num								= 3;
	if( FJ_I2C_Transfer(ch, &trans, 1) != FJ_ERR_OK ){
		ret = E_CAMERA_ERR_NG;
	}
	
	CAMERA_PRINT_ERROR( FJ_I2C_Close(ch) );
	
	return ret;
}

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */
