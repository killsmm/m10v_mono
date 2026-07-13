/**
 * @file		LCDCtrl.c
 * @brief 		LCD controller
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "LCDCtrl.h"
#include "dd_top.h"
#include "debug.h"
#define DDR_OUTPUT_DIR (1)	//Set DDR(Data Direction Register) to output
#define DDR_INPUT_DIR  (0)	//Set DDR(Data Direction Register) to input

#define EPCR_OUT_SIGNAL (1)	//Set EPCR（External Port Control Register) to out signal
#define EPCR_GPIO		(0) //Set EPCR（External Port Control Register) to GPIO
#define INTERNAL_CLOCK   (0) //Set internal clock
#define EXTERIOR_CLOCK   (1) //Set exterior clock
/**
 * Global information for LCD type and mode 
 */
static LCD_TYPE_MODE gLCD_drv = { 0 };
VOID lcd_set_hook(VOID);

VOID LCD_Set_Hook_Function(VOID)
{
#if !defined (D_LCD_DRV_ST7796S_RGB_DELTA) && !defined (D_LCD_DRV_LPM030M369F_YUV_STRIPE)
	lcd_set_hook();
#endif
}

VOID LCD_Init( INT32 bl_ctrl )
{
	INT32 ret_code;

	gLCD_drv.type = LCD_TYPE_KIND;
	gLCD_drv.mode = LCD_MODE_KIND;

	ret_code = FJ_LCD_Drv_init( bl_ctrl );
	if (ret_code != LCD_ERR_OK) {
		BF_Debug_Print_Error(("LCD Initialization failed in driver_init.\n"));
	}
	else{

	#ifndef D_LCD_DRV_CASIO_RGB_DELTA
		ret_code = FJ_LCD_Drv_Config( LCD_DISPLAY_NORMAL );
		if (ret_code != LCD_ERR_OK) {
			BF_Debug_Print_Error(("LCD Initialization failed in driver_config.\n"));
		}
	#endif
	}
}

VOID LCD_PowerUp( UCHAR pwr_saving )
{
	if (pwr_saving) FJ_LCD_Drv_Power(1);
	FJ_LCD_Drv_BackLight_Ctrl(1);
}

VOID LCD_PowerDown( UCHAR pwr_saving )
{
	FJ_LCD_Drv_BackLight_Ctrl(0);
	if (pwr_saving) FJ_LCD_Drv_Power(0);
}

INT32 LCD_GetDrvType( VOID )
{
	return (gLCD_drv.type);
}

INT32 LCD_GetDrvMode( VOID )
{
	return (gLCD_drv.mode);
}

/********************************************************************************/
/* Attention: Basically Don't call this API before called LCD_Init()			*/
/* ## if you need it before LCD_Init(), please set the TRUE as input parameter	*/
/********************************************************************************/
INT32 LCD_GetVLDMode( INT32 re_set )
{
	INT32 vld_mode;

	if (re_set) {
		gLCD_drv.type = LCD_TYPE_KIND;
		gLCD_drv.mode = LCD_MODE_KIND;
	}
#if(LCD_MODE_KIND == D_LCDMODE_RGB_DELTA)
		vld_mode = 8;	// MODE8
#elif(LCD_MODE_KIND == D_LCDMODE_RGB_DUMMY_320)
		vld_mode = 1;	// MODE1
#elif(LCD_MODE_KIND == D_LCDMODE_YUV640_NTSC)
		vld_mode = 0;	// MODE0
#elif(LCD_MODE_KIND == D_LCDMODE_RGB_DUMMY_360 || LCD_MODE_KIND == D_LCDMODE_YUV640_PAL || LCD_MODE_KIND == D_LCDMODE_YUV720_NTSC || LCD_MODE_KIND == D_LCDMODE_YUV720_PAL || LCD_MODE_KIND == D_LCDMODE_CCIR656)
#else
		vld_mode = LCD_ERR_NG;	// Not support at this time
		BF_Debug_Print_Error((" LCD mode get failed! :Not suppot VLD mode=%x\n", __LINE__, vld_mode));
#endif
	return (vld_mode);
}

/** 
 * Global structure variable of LCD
 */ 
static struct lcd_drv_s g_lcd_drv;

/**
 * Initilzation of LCD device driver at the system bootup.
 *
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_init( INT32 bl_ctrl )
{
	INT32 rval;
	memset( &g_lcd_drv, 0, sizeof(struct lcd_drv_s) );

#if defined (D_LCD_DRV_ST7796S_RGB_DELTA) || defined (D_LCD_DRV_LPM030M369F_YUV_STRIPE)
	rval = drv_lcd_init();
#else
	rval = lcd_init( bl_ctrl );
#endif
	return rval;
}

/**
 * Attach the LCD device driver.
 *
 * @param lcd - Context of LCD device driver.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_Attach( struct lcd_drv_s *lcd )
{
	if (lcd == NULL) return (LCD_ERR_NG);
	memcpy(&g_lcd_drv, lcd, sizeof(struct lcd_drv_s));
	return 0;
}

/**
 * Remove the LCD device driver.
 *
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_Remove( VOID )
{
 	memset( &g_lcd_drv, 0, sizeof(struct lcd_drv_s) );
	return 0;
}

/**
 * Configure the lcd device.
 *
 * @param mode - Signal input format.
 * @param ar - Display aspect ratio.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_Config( LCD_DISPLAY_AR ar )
{
	if (g_lcd_drv.config != NULL)
		return g_lcd_drv.config( gLCD_drv.mode, ar );
	else
		return (LCD_ERR_NG);
}

/**
 * Enable/Disable the backlight of LCD.
 *
 * @param on_off - TRUE for enable, FALSE for disable.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_BackLight_Ctrl( UCHAR on_off )
{
	if ( g_lcd_drv.backlight_ctrl != NULL )
		return g_lcd_drv.backlight_ctrl( on_off );
	else
		return (LCD_ERR_NG);
}

/**
 * Set brightness of LCD.
 *
 * @param brightness - Value of brightness.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_Brightness( UCHAR brightness )
{
	if (g_lcd_drv.brightness != NULL)
		return g_lcd_drv.brightness(brightness);
	else
		return (LCD_ERR_NG);
}

/**
 * Set contrast of LCD.
 *
 * @param contrast - Value of contrast.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_Contrast( UCHAR contrast )
{
	if (g_lcd_drv.contrast != NULL)
		return g_lcd_drv.contrast(contrast);
	else
		return (LCD_ERR_NG);
}

/**
 * Set contrast of R,B sub component.
 *
 * @param contrast - Value of contrast.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_RBsub_Contrast( UCHAR contrast )
{
	if (g_lcd_drv.rb_sub_contrast != NULL)
		return g_lcd_drv.rb_sub_contrast(contrast);
	else
		return (LCD_ERR_NG);
}

/**
 * Set brightness of R,B sub component.
 *
 * @param brightness - Value of brightness.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_RBsub_Brightness( UCHAR brightness )
{
	if (g_lcd_drv.rb_sub_brightness != NULL)
		return g_lcd_drv.rb_sub_brightness(brightness);
	else
		return (LCD_ERR_NG);
}

/**
 * Enable/Disable power mode.
 *
 * @param on_off - TRUE for normal mode, FALSE for standby mode.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_Power( UCHAR on_off )
{
	if (g_lcd_drv.power_saving != NULL)
		return g_lcd_drv.power_saving(on_off);
	else
		return (LCD_ERR_NG);
}

/**
 * Set display aspect ratio of LCD.
 *
 * @param ar - Display mode of LCD.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_Aspect_Ratio( LCD_DISPLAY_AR ar )
{
	if (g_lcd_drv.aspect_ratio != NULL)
		return g_lcd_drv.aspect_ratio(ar);
	else
		return (LCD_ERR_NG);
}

/**
 * Set saturation of LCD.
 *
 * @param saturation - Saturation of LCD.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_Saturation( UCHAR saturation )
{
	if (g_lcd_drv.saturation != NULL)
		return g_lcd_drv.saturation(saturation);
	else
		return (LCD_ERR_NG);
}

/**
 * Set hue of LCD.
 *
 * @param hue - Hue of LCD.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_Hue( UCHAR hue )
{
	if (g_lcd_drv.hue != NULL)
		return g_lcd_drv.hue(hue);
	else
		return (LCD_ERR_NG);
}

/**
 * Switch display direction.
 *
 * @param hue - Hue of LCD.
 * @returns 0 for success or < 0 if error occured.
 */
INT32 FJ_LCD_Drv_Display_Direction( UCHAR h_dir, UCHAR v_dir )
{
	if (g_lcd_drv.display_direction != NULL)
		return g_lcd_drv.display_direction( h_dir, v_dir );
	else
		return (LCD_ERR_NG);
}
/**
 * Write register of LCD.
 *
 * @param addr - Address of register.
 * @param data - Data to write in register.
 * @returns 0 for success, otherwise failure.
 */
INT32 FJ_LCD_Drv_Write_Reg( UCHAR addr, UCHAR data )
{
	if (g_lcd_drv.wr_reg != NULL)
		return g_lcd_drv.wr_reg(addr, data);
	else
		return (LCD_ERR_NG);
}

/**
 * Read register of LCD.
 *
 * @param addr - Address of register.
 * @param *data - Data pointer to read from register.
 * @returns 0 for success, otherwise failure.
 */
INT32 FJ_LCD_Drv_Read_Reg( UCHAR addr, UCHAR *data )
{
	if ((g_lcd_drv.rd_reg != NULL) && (data != NULL)) {
		*data = 0x00;	// Initialization
		return g_lcd_drv.rd_reg(addr, data);
	}
	else {
		return (LCD_ERR_NG);
	}
}

///**
// * @brief       Set LCD pin data
// * @param[in]   None
// * @param[out]  None
// * @return      None
// * @note        Set CHIP TOP data for LCD and Bord connection
// * @attention   None
// */
//void FJ_LCD_Set_Pin()
//{
//	// Serial bus port setting
//	D_EPR_LCD_SIO_SEN	= 
//	D_EPR_LCD_SIO_SDA	= 
//	D_EPR_LCD_SIO_SCK	= EPCR_GPIO;		// GPIO use
//
//	D_PDR_LCD_SIO_SEN	= 
//	D_PDR_LCD_SIO_SDA	= 
//	D_PDR_LCD_SIO_SCK	= DDR_OUTPUT_DIR;	// OUTPUT setting
//}
