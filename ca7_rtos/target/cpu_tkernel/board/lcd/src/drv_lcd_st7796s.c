/**
 * @file		drv_lcd_st7796s.c
 * @brief		LCD ST7796S Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include <string.h>
#include "drv_lcd.h"
#include "drv_lcd_st7796s.h"
#include "dd_top.h"

#include "fj_peripheral.h"
#include "os_user_custom.h"

#define LCD_SPI_CH					FJ_PERI_CH_1
#define LCD_RESET					FJ_GPIO_PL5		// DPSD->LCD_SD->RESET
#define LCD_BL_EN					FJ_GPIO_PM4		// WPPG13_1->BL_EN->BL_C/A
#define LCD_PWM_CH					FJ_PERI_CH_13	// WPPG13_1

struct st7796s_t {
    //unsigned char panel_id;
    unsigned char brightness;
    unsigned int width;
    unsigned int height;
    unsigned int rotation;
};

static struct st7796s_t st7796s = {
    .brightness = 60,		// BD6072HFN Duty rate (5 - 95%)
    .width      = ST7796S_LCD_WIDTH,
    .height     = ST7796S_LCD_HEIGHT,
    .rotation   = ST7796S_LCD_ROTATION
};

#if 0
static LCD_ST7796S_CONFIG_s St7796s_Config[LCD_ST7796S_NUM_MODE] = {
    [LCD_ST7796S_960_480_60HZ] = {
        .Width          = 320,
        .Height         = 480,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 60000,
            .NumUnitsInTick = 1001,
        },
        .ScreenMode     = LCD_ST7796S_SCREEN_MODE_WIDE,
        .DeviceClock    = DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = DSP_VOUT_BT601_24B,              /* pixel format */ /* for 24-bit RGB, GPIO31(POC[23]) needs to be pulled low */
        .EvenLineColor  = DSP_VOUT_LCD_COLOR_RGB,          /* even line color order */
        .OddLineColor   = DSP_VOUT_LCD_COLOR_RGB,          /* odd line color order */
        .VideoTiming    = {
            .PixelClock         = 11880216, // FrameWidth x FrameHeight x 59.94 ( = 60/1.001)
            .PixelRepetition    = 1,
            .Htotal             = 400,      // 320 + hpw + hbp + hfp
            .Vtotal             = 496,      // 480 + vs + vbp + vfp
            .HsyncColStart      = 0,
            .HsyncColEnd        = 25,       // hpw
            .VsyncColStart      = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart      = 0,
            .VsyncRowEnd        = 4,        // vs
            .ActiveColStart     = 50,       // hpw + hbp
            .ActiveColWidth     = 320,
            .ActiveRowStart     = 8,        // vs + vbp
            .ActiveRowHeight    = 480
        }
    },
    [LCD_ST7796S_960_480_50HZ] = {
        .Width          = 960,
        .Height         = 480,
        .FrameRate      = {
            .Interlace  = 0,
            .TimeScale  = 50,
            .NumUnitsInTick = 1,
        },
        .ScreenMode     = LCD_ST7796S_SCREEN_MODE_WIDE,
        .DeviceClock    = DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,  /* clock frequency to LCD */
        .OutputMode     = DSP_VOUT_LCD_RGB565,             /* pixel format */
        .EvenLineColor  = DSP_VOUT_LCD_COLOR_RGB,          /* even line color order */
        .OddLineColor   = DSP_VOUT_LCD_COLOR_RGB,          /* odd line color order */
        .VideoTiming    = {
            .PixelClock         = 30030000,
            .PixelRepetition    = 1,
            .Htotal             = 1144,
            .Vtotal             = 525,
            .HsyncColStart      = 0,
            .HsyncColEnd        = 1,
            .VsyncColStart      = 0,
            .VsyncColEnd        = 0,
            .VsyncRowStart      = 0,
            .VsyncRowEnd        = 1,
            .ActiveColStart     = 32,
            .ActiveColWidth     = 960,
            .ActiveRowStart     = 42,
            .ActiveRowHeight    = 480
        }
    }
};
#endif

#if 0	// Unknown
static struct lcd_vout_config st7796s_vout_config = {
    .pixel_repetition = 1,
    .pixel_clk = 11880216,
    .display_timing = {
        .PixelClock           = 11880216,     //  FrameWidth x FrameHeight x 59.94 ( = 60/1.001)
        .FrameWidth           = 400,          // LCD_WIDTH + hpw(25) + hbp(25) + hfp(20)
        .FrameHeight          = 496,          // LCD_HEIGH + vs(4) + vbp(4) + vfp(8)
        .FrameActiveColStart  = 50,           // hpw(25) + hbp(25)
        .FrameActiveColWidth  = 320,          // LCD_WIDTH x COLORS_PER_DOT
        .FrameActiveRowStart  = 8,            // vs(4) + vbp(4)
        .FrameActiveRowHeight = 480,
        .Rotation = DSP_VOUT_ROTATE_NORMAL,
        .Interlace = 0,
    },
    .pixel_format = {
        .SyncCtrl = {
            .HSyncColStart   = 0,
            .HSyncColEnd     = 25,              // hpw
            .VSyncColStart   = 0,
            .VSyncColEnd     = 0,
            .VSyncRowStart   = 0,
            .VSyncRowEnd     = 4,               // vs
        },

        .OutputMode          = DSP_VOUT_BT601_24B,    /* for 24-bit RGB, GPIO31(POC[23]) needs to be pulled low */
        .DeviceClock         = DSP_VOUT_PIXEL_CLOCK_FULL_DCLK,    /* clock frequency to LCD */
        .EvenLineColor       = DSP_VOUT_LCD_COLOR_RGB,
        .OddLineColor        = DSP_VOUT_LCD_COLOR_RGB,
        .ClkSampleEdge       = DSP_VOUT_ACTIVE_HIGH,
        .LineSyncPolarity    = DSP_VOUT_ACTIVE_LOW,
        .FrameSyncPolarity   = DSP_VOUT_ACTIVE_LOW,
        .LineValidPolarity   = DSP_VOUT_ACTIVE_HIGH,
        .FrameValidPolarity  = DSP_VOUT_ACTIVE_HIGH,
        .Interlace           = 0,
        .RowTime             = 400
    }
};
#endif

static GP_RET_CODE drv_st7796s_set_backlight( void );
static GP_RET_CODE drv_st7796s_set_brightness( unsigned char brightness );

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
			ERR_MSG( "FJ_SPI_Transfer failed error=%d", err_code);
			retval = GP_RET_FAILURE;
		}
	}
	else
	{
		ERR_MSG( "FJ_SPI_Ctrl failed error=%d", err_code);
		retval = GP_RET_FAILURE;
    }
	
	err_code = FJ_SPI_Close(LCD_SPI_CH);
	if( err_code != FJ_ERR_OK ) 
	{
		ERR_MSG( "FJ_SPI_Close failed error=%d", err_code);
		retval = GP_RET_FAILURE;
	}

    return retval;
}

GP_RET_CODE drv_st7796s_write( unsigned short reg_offset, unsigned short data )
{
    GP_RET_CODE retval = GP_RET_FAILURE;

    unsigned short spi_cmd = (reg_offset << 9) | (data & 0xFF);

    retval = spi_write( spi_cmd );
    if( retval == GP_RET_FAILURE )
	{
		ERR_MSG("%s: [Error] I2C communication", __FUNCTION__);		/* pgr0822 */
	}

	return retval;
}

GP_RET_CODE drv_st7796s_write_cmd( unsigned short cmd )
{
    GP_RET_CODE retval = GP_RET_FAILURE;

    unsigned short spi_cmd = (cmd & 0xFF);

    retval = spi_write( spi_cmd );
    if( retval == GP_RET_FAILURE )
	{
		ERR_MSG("%s: [Error] I2C communication", __FUNCTION__);		/* pgr0822 */
	}

    return retval;
}

GP_RET_CODE drv_st7796s_write_reg_u8( unsigned char data )
{
    GP_RET_CODE retval = GP_RET_FAILURE;

    unsigned short spi_cmd = 0x0100 | (data & 0xFF);

    retval = spi_write( spi_cmd );
    if( retval == GP_RET_FAILURE )
	{
		ERR_MSG("%s: [Error] I2C communication", __FUNCTION__);		/* pgr0822 */
	}

    return retval;
}


static void drv_st7796s_hw_reset( void )
{
	FJ_ERR_CODE ret;

	ret = FJ_Gpio_Set_Function( LCD_RESET, 0 );	// set IO function;
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] HW setting", __FUNCTION__);		/* pgr0822 */
	}

	ret = FJ_Gpio_Set_Direction( LCD_RESET, 1 );	// set Output direction
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] IO Direction setting", __FUNCTION__);		/* pgr0822 */
	}

	delay_ms(20);
	ret = FJ_Gpio_Set_Status( LCD_RESET, 0 );		// set IO status for LCD_RST(Low)
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] IO Status settingT", __FUNCTION__);		/* pgr0822 */
	}

	delay_ms(20);
	ret = FJ_Gpio_Set_Status( LCD_RESET, 1 );		// set IO status for LCD_RST(High)
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] IO Status setting", __FUNCTION__);		/* pgr0822 */
	}

	delay_ms(20);
}

static void drv_st7796s_vendor_config( void )
{
//    drv_st7796s_write_cmd(CMD_VCOM_CTRL);      // VCOM control
//    drv_st7796s_write_reg_u8(0x3c);

    drv_st7796s_write_cmd(CMD_PV_GAMMA_CTRL);  // Positive Voltage Gamma Control
/*
    drv_st7796s_write_reg_u8(0xF0);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x02);
    drv_st7796s_write_reg_u8(0x0A);
    drv_st7796s_write_reg_u8(0x0D);
    drv_st7796s_write_reg_u8(0x1D);
    drv_st7796s_write_reg_u8(0x35);
    drv_st7796s_write_reg_u8(0x55);
    drv_st7796s_write_reg_u8(0x45);
    drv_st7796s_write_reg_u8(0x3C);
    drv_st7796s_write_reg_u8(0x17);
    drv_st7796s_write_reg_u8(0x17);
    drv_st7796s_write_reg_u8(0x18);
    drv_st7796s_write_reg_u8(0x1B);
*/
    drv_st7796s_write_reg_u8(0xF0);
    drv_st7796s_write_reg_u8(0x0D);
    drv_st7796s_write_reg_u8(0x15);
    drv_st7796s_write_reg_u8(0x0A);
    drv_st7796s_write_reg_u8(0x0B);
    drv_st7796s_write_reg_u8(0x06);
    drv_st7796s_write_reg_u8(0x3A);
    drv_st7796s_write_reg_u8(0x44);
    drv_st7796s_write_reg_u8(0x4F);
    drv_st7796s_write_reg_u8(0x0E);
    drv_st7796s_write_reg_u8(0x17);
    drv_st7796s_write_reg_u8(0x17);
    drv_st7796s_write_reg_u8(0x20);
    drv_st7796s_write_reg_u8(0x36);

    drv_st7796s_write_cmd(CMD_NV_GAMMA_CTRL);  // Negative Voltage Gamma Control
/*
    drv_st7796s_write_reg_u8(0xF0);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x02);
    drv_st7796s_write_reg_u8(0x07);
    drv_st7796s_write_reg_u8(0x06);
    drv_st7796s_write_reg_u8(0x04);
    drv_st7796s_write_reg_u8(0x2E);
    drv_st7796s_write_reg_u8(0x44);
    drv_st7796s_write_reg_u8(0x45);
    drv_st7796s_write_reg_u8(0x0B);
    drv_st7796s_write_reg_u8(0x17);
    drv_st7796s_write_reg_u8(0x16);
    drv_st7796s_write_reg_u8(0x18);
    drv_st7796s_write_reg_u8(0x1B);
*/
    drv_st7796s_write_reg_u8(0xF0);
    drv_st7796s_write_reg_u8(0x0D);
    drv_st7796s_write_reg_u8(0x15);
    drv_st7796s_write_reg_u8(0x0A);
    drv_st7796s_write_reg_u8(0x0B);
    drv_st7796s_write_reg_u8(0x06);
    drv_st7796s_write_reg_u8(0x3A);
    drv_st7796s_write_reg_u8(0x44);
    drv_st7796s_write_reg_u8(0x4F);
    drv_st7796s_write_reg_u8(0x0E);
    drv_st7796s_write_reg_u8(0x17);
    drv_st7796s_write_reg_u8(0x17);
    drv_st7796s_write_reg_u8(0x20);
    drv_st7796s_write_reg_u8(0x36);
}

static void drv_st7796s_init_setup( void )
{
#if defined (LCD_LANDSCAPE_MODE)
        UINT16 col = 480 - 1;
       UINT16 row = 320 - 1;
#else
        UINT16 col = 320 - 1;
        UINT16 row = 480 - 1;
#endif

    drv_st7796s_hw_reset();

    drv_st7796s_write_cmd(CMD_SW_RESET);        // software reset
    delay_ms(120);

    drv_st7796s_write_cmd(CMD_SLEEP_OUT);
    delay_ms(120);

/*
    drv_st7796s_write_cmd(0xF0);                // Command Set Control ??
    drv_st7796s_write_reg_u8(0xC3);

    drv_st7796s_write_cmd(0xF0);                // ??
    drv_st7796s_write_reg_u8(0x96);

    drv_st7796s_write_cmd(CMD_MEM_ACESS_CTRL);  // memory data access control
    drv_st7796s_write_reg_u8(0x08);

    drv_st7796s_write_cmd(CMD_IF_MODE);         // interface mode
    drv_st7796s_write_reg_u8(0x80);             // SPI DOUT pin is not used

    drv_st7796s_write_cmd(CMD_DISPLAY_FUNC);    // display function
    drv_st7796s_write_reg_u8(0xa0);             // non-memory & RGB mode
    drv_st7796s_write_reg_u8(0x02);             // default setting
    drv_st7796s_write_reg_u8(0x3b);             // NL = 480 / 8 - 1

    drv_st7796s_write_cmd(CMD_ENTRY_MODE);      // entry mode
    drv_st7796s_write_reg_u8(0xc6);             //

    drv_st7796s_write_cmd(CMD_DISPLAY_CTRL);    // Display Output Ctrl Adjust
    drv_st7796s_write_reg_u8(0x40);
    drv_st7796s_write_reg_u8(0x8A);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x28);
    drv_st7796s_write_reg_u8(0x0a);
    drv_st7796s_write_reg_u8(0x2e);
*/
    drv_st7796s_write_cmd(CMD_NORMAL_MODE_ON);
    drv_st7796s_write_cmd(CMD_INVERSION_ON);     // display inversion on

#ifdef LCD_LANDSCAPE_MODE
    drv_st7796s_write_cmd(CMD_MEM_ACESS_CTRL);   // memory data access control
    drv_st7796s_write_reg_u8(0x2C);              // Set display data latch order to from right to left
#else
    drv_st7796s_write_cmd(CMD_MEM_ACESS_CTRL);   // memory data access control
    drv_st7796s_write_reg_u8(0x0C);              // Set display data latch order to from right to left
#endif
    drv_st7796s_write_cmd(CMD_IDLE_MODE_OFF);    // Set IDLE MODE OFF
    drv_st7796s_write_cmd(CMD_PIXEL_FORMAT);
    drv_st7796s_write_reg_u8(0x66);

    drv_st7796s_write_cmd(CMD_TABLE2_ENABLE);
    drv_st7796s_write_reg_u8(0xC3);

    drv_st7796s_write_cmd(CMD_TABLE2_ENABLE);
    drv_st7796s_write_reg_u8(0x96);

    drv_st7796s_write_cmd(CMD_IF_MODE);          // interface mode
    drv_st7796s_write_reg_u8(0x80);              // SPI DOUT pin is not used

    drv_st7796s_write_cmd(CMD_FRAME_RATE_CTRL);
    drv_st7796s_write_reg_u8(0xA0);
    drv_st7796s_write_reg_u8(0x0B);

    drv_st7796s_write_cmd(CMD_INVERSION_CTRL);
    drv_st7796s_write_reg_u8(0x01);

    drv_st7796s_write_cmd(CMD_BLANK_PORCH_CTRL);
    drv_st7796s_write_reg_u8(0x08);
    drv_st7796s_write_reg_u8(0x04);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x19);

#ifdef LCD_LANDSCAPE_MODE
    drv_st7796s_write_cmd(CMD_DISPLAY_FUNC);     // display function
    drv_st7796s_write_reg_u8(0x20);              // non-memory & RGB mode
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x3b);              // NL = 480 / 8 - 1
#else
    drv_st7796s_write_cmd(CMD_DISPLAY_FUNC);     // display function
    drv_st7796s_write_reg_u8(0xa0);              // non-memory & RGB mode
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x3b);              // NL = 480 / 8 - 1
#endif
    drv_st7796s_write_cmd(CMD_ENTRY_MODE);       // entry mode
    drv_st7796s_write_reg_u8(0x06);              //

    drv_st7796s_write_cmd(0xB9);
    drv_st7796s_write_reg_u8(0x00);

    drv_st7796s_write_cmd(CMD_PWR_CTRL1);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x46);

    drv_st7796s_write_cmd(CMD_PWR_CTRL2);
    drv_st7796s_write_reg_u8(0x05);

    drv_st7796s_write_cmd(CMD_VCOM_CTRL);
    drv_st7796s_write_reg_u8(0x20);

    // Gamma settings
    drv_st7796s_vendor_config();

//    drv_st7796s_write_cmd(0xF0);                // Command Set Control ??
//    drv_st7796s_write_reg_u8(0xC3);

//    drv_st7796s_write_cmd(0xF0);                // ??
//    drv_st7796s_write_reg_u8(0x69);

    delay_ms(120);

#if defined (LCD_LANDSCAPE_MODE)
    // Colume address set
    drv_st7796s_write_cmd(CMD_COL_ADDR_SET);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8((col & 0xff00) >> 8);
    drv_st7796s_write_reg_u8((col & 0xff));

    // Row address set
    drv_st7796s_write_cmd(CMD_ROW_ADDR_SET);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8((row & 0xff00) >> 8);
    drv_st7796s_write_reg_u8(row & 0xff);
#endif

    drv_st7796s_write_cmd(CMD_DRV_SIGNAL_CTRL);
    drv_st7796s_write_reg_u8(0x40);
    drv_st7796s_write_reg_u8(0x8A);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x00);
    drv_st7796s_write_reg_u8(0x2B);
    drv_st7796s_write_reg_u8(0x10);
    drv_st7796s_write_reg_u8(0x36);

//    drv_st7796s_write_cmd(CMD_INVERSION_ON);    // display inversion on
    drv_st7796s_write_cmd(CMD_SPI_READ_ENABLE);     // Display Output Ctrl Adjust
    drv_st7796s_write_reg_u8(0x00);

    drv_st7796s_write_cmd(CMD_DISPLAY_ON);      // display on
}

static GP_RET_CODE drv_st7796s_enable( void )
{
	FJ_ERR_CODE ret;

#if 0	// unknown
    gp_print_green("%s", __FUNCTION__);		/* pgr0822 */
#endif

	ret = FJ_Gpio_Set_Status( LCD_RESET, 1 );		// set IO status for LCD_RST(High)
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] IO Status setting", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}
    delay_ms(20);

    //drv_st7796s_write_cmd(CMD_DISPLAY_ON);
//    drv_st7796s_init_setup();

    // Set BL front current to be 18mA by default
    drv_st7796s_set_brightness(st7796s.brightness);

	return GP_RET_SUCCESS;
}

static GP_RET_CODE drv_st7796s_disable( void )
{
	// TODO: to add to reset for power save
	drv_st7796s_write(0x0, (UINT16) 0x00);   /* Standby mode */

	FJ_PWM_Stop( LCD_PWM_CH );
	FJ_PWM_Close( LCD_PWM_CH );

	return GP_RET_SUCCESS;
}


static GP_RET_CODE drv_st7796s_config( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;

    return retval;
}

static GP_RET_CODE drv_st7796s_post_config( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;

    return retval;
}

static GP_RET_CODE drv_st7796s_get_resolution( unsigned int *width, unsigned int *height, unsigned int *rotation )
{
	GP_RET_CODE retval = GP_RET_SUCCESS;

    *width      = st7796s.width;
    *height     = st7796s.height;
    *rotation   = st7796s.rotation;

    return retval;
}

static GP_RET_CODE drv_st7796s_set_backlight( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;

    return retval;
}

static GP_RET_CODE drv_st7796s_get_brightness( unsigned char *brightness )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;

    return retval;
}

static GP_RET_CODE drv_st7796s_set_brightness( unsigned char brightness )
{
	FJ_ERR_CODE ret;
	FJ_PWM_CTRL	pwm_ctrl;
	UINT32		cycle_hz = 100;
	UINT32		duty_rate = brightness;

	Dd_Top_Set_PERSEL3_U1IDWP13( 1 );

	ret = FJ_PWM_Open( LCD_PWM_CH );
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] PWM : Open(%d)", __FUNCTION__, ret);		/* pgr0822 */
	}

	pwm_ctrl.ch				= LCD_PWM_CH;
	pwm_ctrl.mode			= FJ_PWM_MODE_CONTINUOUS;
	pwm_ctrl.polarity		= FJ_PWM_POLARITY_NORMAL;
	pwm_ctrl.restart_enable	= FJ_PWM_RESTART_ENABLE;
	pwm_ctrl.pcallback1		= NULL;
	pwm_ctrl.pcallback2		= NULL;

	ret = FJ_PWM_Set_By_Ratio( &pwm_ctrl, cycle_hz, duty_rate );
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] PWM : Ctrl(%d)", __FUNCTION__, ret);		/* pgr0822 */
	}

	ret = FJ_PWM_Start( LCD_PWM_CH );
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] PWM : Start(%d)", __FUNCTION__, ret);		/* pgr0822 */
	}

    return GP_RET_SUCCESS;
}

static GP_RET_CODE drv_st7796s_set_contrast( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;

	// TODO need to add input parameters

    return retval;
}

static GP_RET_CODE drv_st7796s_set_color_balance( void )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;
    return retval;
}

static void st7796s_write_display( unsigned char data )
{
    GP_RET_CODE retval = GP_RET_SUCCESS;

    unsigned short spi_cmd = 0x0100 | (data & 0xFC);

    retval = spi_write( spi_cmd );
    if( retval == GP_RET_FAILURE )
    {
		ERR_MSG("%s: [Error] I2C communication", __FUNCTION__);		/* pgr0822 */
	}
}

static void write_ram_data( unsigned char red, unsigned char green, unsigned char blue )
{
    st7796s_write_display(red);
    st7796s_write_display(green);
    st7796s_write_display(blue);
}


/* Clear a defined window with certain color */
void st7796s_clear_window( unsigned int x_start, unsigned int y_start,
                           unsigned int width, unsigned int height, unsigned int color )
{
    unsigned int i, j;
    unsigned char red, green, blue;

    drv_st7796s_write_cmd(CMD_COL_ADDR_SET);
    drv_st7796s_write_reg_u8(x_start >> 8);                 // column address start2
    drv_st7796s_write_reg_u8(x_start);                      // column address start1
    drv_st7796s_write_reg_u8((width + x_start - 1) >> 8);   // column address end2
    drv_st7796s_write_reg_u8((width + x_start - 1));        // column address end1

    drv_st7796s_write_cmd(CMD_ROW_ADDR_SET);
    drv_st7796s_write_reg_u8(y_start >> 8);                 // row address start2
    drv_st7796s_write_reg_u8(y_start);                      // row address start1
    drv_st7796s_write_reg_u8((height + y_start - 1) >> 8);  // row address end2
    drv_st7796s_write_reg_u8((height + y_start - 1));       // row address end1

    blue  = color;
    green = color >> 8;
    red   = color >> 16;

#if 0	// Unknown
    gp_print_yellow("===>red: %x, green: %x, blue: %x", red, green, blue);
#endif

    drv_st7796s_write_cmd(0x2C);                     // write data to RAM
    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            write_ram_data(red, green, blue);
        }
    }
}

void drv_st7796s_test_pattern_init( void )
{
    drv_st7796s_hw_reset();

    drv_st7796s_write_cmd(CMD_SW_RESET);        // software reset
    delay_ms(20);

    drv_st7796s_write_cmd(CMD_SLEEP_OUT);
    delay_ms(120);

    drv_st7796s_write_cmd(CMD_MEM_ACESS_CTRL);  // memory data access control

    drv_st7796s_write_reg_u8(0x00);


    drv_st7796s_write_cmd(CMD_PIXEL_FORMAT);    // interface pixel format
    drv_st7796s_write_reg_u8(0x66);             // 18 bit/pixel, 262K of RGB

    //Gamma settings
	drv_st7796s_write_cmd(0xB7);       // Gate Control
    drv_st7796s_write_reg_u8(0x54);             // VGH(v): 14.06, VGL(v): -9.6

    // Disable Digital Gamma setting
    drv_st7796s_write_cmd(CMD_DGM_EN);
    drv_st7796s_write_reg_u8(0x00);

    // Power setting
	drv_st7796s_write_cmd(CMD_VCOM_SETTING);    // VCOM setting
	drv_st7796s_write_reg_u8(0x29);             // VCOM(V) : 1.125

//	drv_st7796s_write_cmd(CMD_LCM_CTRL);        // LCM control
//	drv_st7796s_write_reg_u8(0x2c);

	drv_st7796s_write_cmd(CMD_VDV_VRH_ENA);     // VDV and VRH command enable
	drv_st7796s_write_reg_u8(0x00);             // VDV and VRH register value comes from comand write

	drv_st7796s_write_cmd(CMD_VRH_SET);         // VRH set
	drv_st7796s_write_reg_u8(0x00);             // 3.55 +( vcom + vcom offset + 0.5vdv )

	drv_st7796s_write_cmd(CMD_VDV_SET);         // VDV set
	drv_st7796s_write_reg_u8(0x20);             // VDV(V): 0

	drv_st7796s_write_cmd(CMD_FR_CTRL);         // frame rate control in normal mode
	drv_st7796s_write_reg_u8(0x0F);             // frame rate: 60Hz, dot inversion

	drv_st7796s_write_cmd(CMD_PWR_CTRL1);       // power control 1
	drv_st7796s_write_reg_u8(0xA4);
	drv_st7796s_write_reg_u8(0x01);             // AVDD(V): 6.4, AVCL(V): -4.4, VDDS(V): 2.3

	drv_st7796s_write_cmd(CMD_VAPVAN_EN);
	drv_st7796s_write_reg_u8(0x4C);

	drv_st7796s_write_cmd(0xDF);                    // This cmd does not appear on Sitronix Spec
	drv_st7796s_write_reg_u8(0x5A);
	drv_st7796s_write_reg_u8(0x69);
	drv_st7796s_write_reg_u8(0x02);
	drv_st7796s_write_reg_u8(0x00);

	drv_st7796s_write_cmd(CMD_PV_GAMMA_CTRL);   // positive voltage gamma control
	drv_st7796s_write_reg_u8(0xf0);
	drv_st7796s_write_reg_u8(0x06);
	drv_st7796s_write_reg_u8(0x0b);
	drv_st7796s_write_reg_u8(0x0a);
	drv_st7796s_write_reg_u8(0x0a);
	drv_st7796s_write_reg_u8(0x06);
	drv_st7796s_write_reg_u8(0x33);
	drv_st7796s_write_reg_u8(0x43);
	drv_st7796s_write_reg_u8(0x49);
	drv_st7796s_write_reg_u8(0x37);
	drv_st7796s_write_reg_u8(0x12);
	drv_st7796s_write_reg_u8(0x11);
	drv_st7796s_write_reg_u8(0x2d);
	drv_st7796s_write_reg_u8(0x32);

	drv_st7796s_write_cmd(CMD_NV_GAMMA_CTRL);   // negative voltage gamma control
	drv_st7796s_write_reg_u8(0xf0);
	drv_st7796s_write_reg_u8(0x06);
	drv_st7796s_write_reg_u8(0x0b);
	drv_st7796s_write_reg_u8(0x0a);
	drv_st7796s_write_reg_u8(0x0a);
	drv_st7796s_write_reg_u8(0x06);
	drv_st7796s_write_reg_u8(0x33);
	drv_st7796s_write_reg_u8(0x43);
	drv_st7796s_write_reg_u8(0x49);
	drv_st7796s_write_reg_u8(0x37);
	drv_st7796s_write_reg_u8(0x12);
	drv_st7796s_write_reg_u8(0x11);
	drv_st7796s_write_reg_u8(0x2d);
	drv_st7796s_write_reg_u8(0x32);

	drv_st7796s_write_cmd(CMD_G_CTRL);
	drv_st7796s_write_reg_u8(0x27);
	drv_st7796s_write_reg_u8(0x00);
	drv_st7796s_write_reg_u8(0x10);

	drv_st7796s_write_cmd(CMD_SPI2_EN);
	drv_st7796s_write_reg_u8(0x01);

	drv_st7796s_write_cmd(CMD_PWR_CTRL2);
	drv_st7796s_write_reg_u8(0x93);

	drv_st7796s_write_cmd(CMD_EQ_CTRL);         // Equalize time control
	drv_st7796s_write_reg_u8(0x02);
	drv_st7796s_write_reg_u8(0x02);
	drv_st7796s_write_reg_u8(0x00);

	drv_st7796s_write_cmd(CMD_PROM_CTRL);
	drv_st7796s_write_reg_u8(0x00);

	drv_st7796s_write_cmd(CMD_PROM_EN);
	drv_st7796s_write_reg_u8(0x5A);
	drv_st7796s_write_reg_u8(0x69);
	drv_st7796s_write_reg_u8(0xEE);
	drv_st7796s_write_reg_u8(0x00);

	drv_st7796s_write_cmd(CMD_NVM_SETTING);
	drv_st7796s_write_reg_u8(0x00);
	drv_st7796s_write_reg_u8(0x00);

	drv_st7796s_write_cmd(CMD_PROM_ACT);
	drv_st7796s_write_reg_u8(0x00);
	drv_st7796s_write_reg_u8(0x00);

	drv_st7796s_write_cmd(CMD_GATE_TIMING_ADJ); // gate on timing adjustment
	drv_st7796s_write_reg_u8(0x2A);
	drv_st7796s_write_reg_u8(0x2B);
	drv_st7796s_write_reg_u8(0x01);
	drv_st7796s_write_reg_u8(0xFF);                // GOFR[3:0] = 0x0F, GOF[0:3] = 0x0F

	// RGB interface control
//	drv_st7796s_write_cmd(CMD_RGB_CTRL);

//	drv_st7796s_write_reg_u8(0xc0);             // DE mode, with shift register

//	drv_st7796s_write_reg_u8(0x04);
//	drv_st7796s_write_reg_u8(0x0A);

	// Frame rate setting
	drv_st7796s_write_cmd(CMD_PORCH_SETTING);   // porch setting
	drv_st7796s_write_reg_u8(0x0c);             // back porch setting
	drv_st7796s_write_reg_u8(0x0c);             // front port setting
	drv_st7796s_write_reg_u8(0x00);             // disable seperate porch control
	drv_st7796s_write_reg_u8(0x33);             // back porch and front porch in idle mode
	drv_st7796s_write_reg_u8(0x33);             // back porch and front porch in partial mode

	drv_st7796s_write_cmd(CMD_FR_CTRL1);
	drv_st7796s_write_reg_u8(0x00);
	drv_st7796s_write_reg_u8(0x0F);
	drv_st7796s_write_reg_u8(0x0F);

	drv_st7796s_write_cmd(0xB0);
	drv_st7796s_write_reg_u8(0x11);             // 00: MCU interface; 11: RGB  interface
	//drv_st7796s_write_reg_u8(0x00);             // 00: MCU interface; 11: RGB  interface
	drv_st7796s_write_reg_u8(0xF4);             // 6-bits RGB interface bus width

    drv_st7796s_write_cmd(CMD_INVERSION_ON);    // display inversion on
    drv_st7796s_write_cmd(CMD_DISPLAY_ON);      // display on
}

GP_RET_CODE drv_st7796s_set_pattern( unsigned int x_start, unsigned int y_start,
                                     unsigned int width, unsigned int height, unsigned int color )
{
    drv_st7796s_test_pattern_init();

    // need to go out rgb mode and switch to mcu mode for this testing
    drv_st7796s_write_cmd(CMD_MEM_ACESS_CTRL);
    drv_st7796s_write_reg_u8(0x00);

//    drv_st7796s_write_cmd(CMD_RGB_CTRL);
//    drv_st7796s_write_reg_u8(0x00);             // DE mode, with RAM

    drv_st7796s_write_cmd(0xB0);
    drv_st7796s_write_reg_u8(0x00);             // 00: MCU interface; 11: RGB  interface

    st7796s_clear_window(x_start, y_start, width, height, color);

    return GP_RET_SUCCESS;
}

GP_RET_CODE drv_st7796s_init( struct drv_lcd_t *lcd_dev )
{
#if 0
	FJ_ERR_CODE ret;
#endif

	strcpy(lcd_dev->name , ST7796s_DEV_NAME);
#if 0	// Unknown
    gp_print_green("---------->  %s init", lcd_dev->name);
#endif

#if 0 // Maybe unnecessary..  P60:GYRO CS, P61:BIG_LCD_CS, P62:SMALL_LCD_CS for CP3
	// set GPIO_60
	ret = FJ_Gpio_Set_Function( FJ_GPIO_P60, 0 );	// set IO function;
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] HW setting : P60", __FUNCTION__);		/* pgr0822 */
	}

	ret = FJ_Gpio_Set_Direction( FJ_GPIO_P60, 1 );	// set Output direction
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] IO Direction setting : P60", __FUNCTION__);		/* pgr0822 */
	}
	ret = FJ_Gpio_Set_Status( FJ_GPIO_P60, 1 );
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] IO Status setting : P60", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}

	// set GPIO_61
	ret = FJ_Gpio_Set_Function( FJ_GPIO_P61, 0 );	// set IO function;
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] HW setting : P61", __FUNCTION__);		/* pgr0822 */
	}

	ret = FJ_Gpio_Set_Direction( FJ_GPIO_P61, 1 );	// set Output direction
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] IO Direction setting : P61", __FUNCTION__);		/* pgr0822 */
	}
	ret = FJ_Gpio_Set_Status( FJ_GPIO_P61, 1 );
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] IO Status setting : P61", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}

	// set GPIO_62
	ret = FJ_Gpio_Set_Function( FJ_GPIO_P62, 0 );	// set IO function;
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] HW setting : P62", __FUNCTION__);		/* pgr0822 */
	}

	ret = FJ_Gpio_Set_Direction( FJ_GPIO_P62, 1 );	// set Output direction
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] IO Direction setting : P62", __FUNCTION__);		/* pgr0822 */
	}
	ret = FJ_Gpio_Set_Status( FJ_GPIO_P62, 1 );
	if( ret != FJ_ERR_OK )
	{
		ERR_MSG("%s: [Error] IO Status setting : P62", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}
#endif

	lcd_dev->enable				= drv_st7796s_enable;
	lcd_dev->disable			= drv_st7796s_disable;
	lcd_dev->config				= drv_st7796s_config;
	lcd_dev->post_config		= drv_st7796s_post_config;
	lcd_dev->get_resolution		= drv_st7796s_get_resolution;
	lcd_dev->set_backlight		= drv_st7796s_set_backlight;
	lcd_dev->get_brightness		= drv_st7796s_get_brightness;
	lcd_dev->set_brightness		= drv_st7796s_set_brightness;
	lcd_dev->set_contrast		= drv_st7796s_set_contrast;
	lcd_dev->set_color_balance	= drv_st7796s_set_color_balance;

	// Display Setup
	drv_st7796s_init_setup();

	return GP_RET_SUCCESS;
}

