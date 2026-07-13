/**
 * @file		drv_lcd_lpm030m369f.c
 * @brief		LCD LPM030M369F Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include <string.h>
#include "drv_lcd.h"
#include "dd_top.h"

#include "fj_peripheral.h"
#include "os_user_custom.h"

#include "share.h"

#define _ERR_MSG_ UPRINTF

#define LCD_SPI_CH					FJ_PERI_CH_1
#define LCD_RESET					E_DD_TOP_GPIO_CM0_P23

struct lpm030m369f_t {
    //unsigned char panel_id;
    //unsigned char brightness;
    unsigned int width;
    unsigned int height;
    unsigned int rotation;
};

static struct lpm030m369f_t lpm030m369f = {
    //.brightness = 100,
    .width      = 720,
    .height     = 480,
    .rotation   = 0
};


static GP_RET_CODE drv_lpm030m369f_set_backlight( void );
static GP_RET_CODE drv_lpm030m369f_set_brightness( unsigned char brightness );

static void delay_ms( unsigned short n )
{
    OS_User_Dly_Tsk(n);
}

static GP_RET_CODE spi_write( unsigned short cmd_data )
{
	GP_RET_CODE				retval = GP_RET_SUCCESS;
	FJ_ERR_CODE				err_code;
	FJ_SPI_CTRL				spi_control;
	FJ_SPI_CTRL_TRANSFER	transfer;

	err_code = FJ_SPI_Open(LCD_SPI_CH);
	if( err_code != FJ_ERR_OK )
	{
		ERR_MSG( "FJ_SPI_Open failed error=%d", err_code );
		return GP_RET_FAILURE;
	}

	// Control
	spi_control.type			= FJ_SPI_TYPE_MASTER;
	spi_control.mode			= FJ_SPI_MODE_0_CPOL0_CPHA0;
	spi_control.bit_direction	= FJ_SPI_BIT_DIR_MSB_FIRST;
	spi_control.data_length		= FJ_SPI_LENGTH_9;
	// cmd,u8
	spi_control.baudrate		= 10000000;
//	// write
//	spi_control.baudrate  = 500000;
	spi_control.fifo_wmark_tx	= 32;
	spi_control.fifo_wmark_rx	= 32;
	spi_control.dma_en			= 0;
	spi_control.dma_recv_to		= 0;
	spi_control.pcallback		= NULL;

	err_code = FJ_SPI_Ctrl(LCD_SPI_CH, &spi_control);
	
	// resutl
	if( err_code == FJ_ERR_OK )
	{
		// Transfer
		transfer.data_count	= 1;
		transfer.direction  = FJ_SPI_DATA_DIR_SEND;
		transfer.data_ptr   = &cmd_data;
		transfer.ss_info.cont_trans	= FJ_SPI_CONT_TRANS_INACT_FIFO_EMPTY;
		transfer.ss_info.ssout[0]	= 0;
		transfer.ss_info.ssout[1]	= 1;
		transfer.ss_info.ssout[2]	= 0;
		transfer.ss_info.ssout[3]	= 0;
		transfer.ss_info.sspol[0]	= 0;
		transfer.ss_info.sspol[1]	= 0;
		transfer.ss_info.sspol[2]	= 0;
		transfer.ss_info.sspol[3]	= 0;

		err_code = FJ_SPI_Transfer(LCD_SPI_CH, &transfer, FJ_USIO_DMA_CH_NOT_USE);
		if( err_code != FJ_ERR_OK )
		{
			_ERR_MSG_( "FJ_SPI_Transfer failed error=%d", err_code);
			retval = GP_RET_FAILURE;
		}
	}
	else
	{
		_ERR_MSG_( "FJ_SPI_Ctrl failed error=%d", err_code);
		retval = GP_RET_FAILURE;
    }
	
	err_code = FJ_SPI_Close(LCD_SPI_CH);
	if( err_code != FJ_ERR_OK ) 
	{
		_ERR_MSG_( "FJ_SPI_Close failed error=%d", err_code);
		retval = GP_RET_FAILURE;
	}

    return retval;
}

GP_RET_CODE drv_lpm030m369f_write( unsigned short reg_offset, unsigned short data )
{
    GP_RET_CODE retval = GP_RET_FAILURE;

    unsigned short spi_cmd = (reg_offset << 9) | (data & 0xFF);

    retval = spi_write( spi_cmd );
    if( retval == GP_RET_FAILURE )
	{
    	_ERR_MSG_("%s: [Error] SPI communication", __FUNCTION__);		/* pgr0822 */
	}

	return retval;
}

GP_RET_CODE drv_lpm030m369f_write_cmd( unsigned short cmd )
{
    GP_RET_CODE retval = GP_RET_FAILURE;

    unsigned short spi_cmd = (cmd & 0xFF);

    retval = spi_write( spi_cmd );
    if( retval == GP_RET_FAILURE )
	{
    	_ERR_MSG_("%s: [Error] SPI communication", __FUNCTION__);		/* pgr0822 */
	}

    return retval;
}

GP_RET_CODE drv_lpm030m369f_write_reg_u8( unsigned char data )
{
    GP_RET_CODE retval = GP_RET_FAILURE;

    unsigned short spi_cmd = 0x0100 | (data & 0xFF);

    retval = spi_write( spi_cmd );
    if( retval == GP_RET_FAILURE )
	{
    	_ERR_MSG_("%s: [Error] SPI communication", __FUNCTION__);		/* pgr0822 */
	}

    return retval;
}

static void drv_lpm030m369f_hw_reset( void )
{
	FJ_ERR_CODE ret;

	//ret = FJ_Gpio_Set_Function( LCD_RESET, 0 );	// set IO function;
	ret = Dd_Top_Set_Gpio_CM0_Function(LCD_RESET, 0);
	if( ret != FJ_ERR_OK )
	{
		_ERR_MSG_("%s: [Error] HW setting", __FUNCTION__);		/* pgr0822 */
	}

	//ret = FJ_Gpio_Set_Direction( LCD_RESET, 1 );	// set Output direction
	ret = Dd_Top_Set_Gpio_CM0_Direction(LCD_RESET, 1);
	if( ret != FJ_ERR_OK )
	{
		_ERR_MSG_("%s: [Error] IO Direction setting", __FUNCTION__);		/* pgr0822 */
	}

	delay_ms(10);
	//ret = FJ_Gpio_Set_Status( LCD_RESET, 0 );		// set IO status for LCD_RST(Low)
	ret = Dd_Top_Set_Gpio_CM0_Status(LCD_RESET, 0);
	if( ret != FJ_ERR_OK )
	{
		_ERR_MSG_("%s: [Error] IO Status setting", __FUNCTION__);		/* pgr0822 */
	}

	//ret = FJ_Gpio_Set_Status( LCD_RESET, 1 );		// set IO status for LCD_RST(High)
	ret = Dd_Top_Set_Gpio_CM0_Status(LCD_RESET, 1);
	if( ret != FJ_ERR_OK )
	{
		_ERR_MSG_("%s: [Error] IO Status setting", __FUNCTION__);		/* pgr0822 */
	}

	delay_ms(5);
}

static void drv_lpm030m369f_init_setup( void )
{
	UPRINTF("%s(%d) \n", __func__, __LINE__);

	drv_lpm030m369f_hw_reset();

	//Manufacturer Command Access Protect
    drv_lpm030m369f_write_cmd(0xB0);
    drv_lpm030m369f_write_reg_u8(0x04);
    // WM setting
    drv_lpm030m369f_write_cmd(0x90);
    drv_lpm030m369f_write_reg_u8(0xFF); //?
    drv_lpm030m369f_write_cmd(0x91);
    drv_lpm030m369f_write_reg_u8(0x78);
    drv_lpm030m369f_write_reg_u8(0xff);
    drv_lpm030m369f_write_reg_u8(0x5e);
    drv_lpm030m369f_write_reg_u8(0x00);
    drv_lpm030m369f_write_cmd(0x92);
    drv_lpm030m369f_write_reg_u8(0x10);
    drv_lpm030m369f_write_reg_u8(0xb0);
    drv_lpm030m369f_write_reg_u8(0x00);
    drv_lpm030m369f_write_reg_u8(0xa2);
    drv_lpm030m369f_write_reg_u8(0x20);
    drv_lpm030m369f_write_cmd(0x93);
    drv_lpm030m369f_write_reg_u8(0x68);
    drv_lpm030m369f_write_reg_u8(0xf2);
    drv_lpm030m369f_write_reg_u8(0xc3);
    drv_lpm030m369f_write_reg_u8(0x01);
    drv_lpm030m369f_write_cmd(0x94);
    drv_lpm030m369f_write_reg_u8(0xc4);
    drv_lpm030m369f_write_reg_u8(0x80);
    drv_lpm030m369f_write_reg_u8(0x25);
    drv_lpm030m369f_write_reg_u8(0x86);
    drv_lpm030m369f_write_cmd(0x95);
    drv_lpm030m369f_write_reg_u8(0x30);
    drv_lpm030m369f_write_reg_u8(0x00);
    drv_lpm030m369f_write_reg_u8(0xff);
    drv_lpm030m369f_write_reg_u8(0x00);
    drv_lpm030m369f_write_reg_u8(0xff);
    drv_lpm030m369f_write_cmd(0xd9);
    drv_lpm030m369f_write_reg_u8(0x95);
    drv_lpm030m369f_write_reg_u8(0x00);
    drv_lpm030m369f_write_reg_u8(0x00);
    drv_lpm030m369f_write_reg_u8(0x00);
    // set_address_mode
    drv_lpm030m369f_write_cmd(0x36);
    drv_lpm030m369f_write_reg_u8(0x00);
    // set_display_on
    drv_lpm030m369f_write_cmd(0x29);
    // exit_sleep_mode
    drv_lpm030m369f_write_cmd(0x11);

    // wait 5 vsync
    delay_ms(5*17);

    // LEDPWM frequency setting
    drv_lpm030m369f_write_cmd(0xc4);
    drv_lpm030m369f_write_reg_u8(0x30);
    drv_lpm030m369f_write_reg_u8(0x04); // ?
    // Black auto-run setting (LCD)
    drv_lpm030m369f_write_cmd(0xde);
    drv_lpm030m369f_write_reg_u8(0x00); // ?
    // Manufacturer Command Access Protect
    drv_lpm030m369f_write_cmd(0xb0);
    drv_lpm030m369f_write_reg_u8(0x03);
}

static GP_RET_CODE drv_lpm030m369f_enable( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;
    UPRINTF("%s(%d) \n", __func__, __LINE__);

    drv_lpm030m369f_set_brightness(60);

    return retval;
}

static GP_RET_CODE drv_lpm030m369f_disable( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;
    UPRINTF("%s(%d) \n", __func__, __LINE__);
    return retval;
}


static GP_RET_CODE drv_lpm030m369f_config( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;
    UPRINTF("%s(%d) \n", __func__, __LINE__);
    return retval;
}

static GP_RET_CODE drv_lpm030m369f_post_config( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;
    UPRINTF("%s(%d) \n", __func__, __LINE__);
    return retval;
}

static GP_RET_CODE drv_lpm030m369f_get_resolution( unsigned int *width, unsigned int *height, unsigned int *rotation )
{
	GP_RET_CODE retval = GP_RET_SUCCESS;
//	UPRINTF("%s(%d) \n", __func__, __LINE__);

	*width = lpm030m369f.width;
	*height = lpm030m369f.height;
	*rotation = lpm030m369f.rotation;

    return retval;
}

static GP_RET_CODE drv_lpm030m369f_set_backlight( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;
    UPRINTF("%s(%d) \n", __func__, __LINE__);
    return retval;
}

static GP_RET_CODE drv_lpm030m369f_get_brightness( unsigned char *brightness )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;
    UPRINTF("%s(%d) \n", __func__, __LINE__);
    return retval;
}

static GP_RET_CODE drv_lpm030m369f_set_brightness( unsigned char brightness )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;

    UPRINTF("%s(%d) \n", __func__, __LINE__);
    return retval;
}

static GP_RET_CODE drv_lpm030m369f_set_contrast( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;
    UPRINTF("%s(%d) \n", __func__, __LINE__);
    return retval;
}

static GP_RET_CODE drv_lpm030m369f_set_color_balance( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;
    UPRINTF("%s(%d) \n", __func__, __LINE__);
    return retval;
}

GP_RET_CODE drv_lpm030m369f_init( struct drv_lcd_t *lcd_dev )
{
	lcd_dev->enable				= drv_lpm030m369f_enable;
	lcd_dev->disable			= drv_lpm030m369f_disable;
	lcd_dev->config				= drv_lpm030m369f_config;
	lcd_dev->post_config		= drv_lpm030m369f_post_config;
	lcd_dev->get_resolution		= drv_lpm030m369f_get_resolution;
	lcd_dev->set_backlight		= drv_lpm030m369f_set_backlight;
	lcd_dev->get_brightness		= drv_lpm030m369f_get_brightness;
	lcd_dev->set_brightness		= drv_lpm030m369f_set_brightness;
	lcd_dev->set_contrast		= drv_lpm030m369f_set_contrast;
	lcd_dev->set_color_balance	= drv_lpm030m369f_set_color_balance;

	// Display Setup
	drv_lpm030m369f_init_setup();

	return GP_RET_SUCCESS;
}

