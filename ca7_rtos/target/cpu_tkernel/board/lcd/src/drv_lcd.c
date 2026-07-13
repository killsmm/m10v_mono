/**
 * @file		drv_lcd.c
 * @brief		LCD Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#if 1	// Porting by SNI start ----->
#include "drv_lcd.h"
#if defined D_LCD_DRV_ST7796S_RGB_DELTA
#include "drv_lcd_st7796s.h"
#elif defined (D_LCD_DRV_LPM030M369F_YUV_STRIPE)
extern GP_RET_CODE drv_lpm030m369f_init( struct drv_lcd_t *lcd_dev );
#endif

static struct drv_lcd_t lcd_drv;

//GP_RET_CODE drv_lcd_init( void )
unsigned int drv_lcd_init( void )
{
#if defined D_LCD_DRV_ST7796S_RGB_DELTA
	return drv_st7796s_init(&lcd_drv);
#elif defined (D_LCD_DRV_LPM030M369F_YUV_STRIPE)
	return drv_lpm030m369f_init(&lcd_drv);
#endif
}

GP_RET_CODE drv_lcd_event_init( void )
{
	return GP_RET_SUCCESS;
}

GP_RET_CODE drv_lcd_enable( void )
{
	if (lcd_drv.enable)
	{
		return lcd_drv.enable();
	}
	else
	{
		ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}
}

GP_RET_CODE drv_lcd_disable( void )
{
	if (lcd_drv.disable)
	{
		return lcd_drv.disable();
	}
	else
	{
		ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}
}

GP_RET_CODE drv_lcd_config( void )
{
	if (lcd_drv.config)
	{
		return lcd_drv.config();
	}
	else
	{
		ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}
}

GP_RET_CODE drv_lcd_post_config( void )
{
	if (lcd_drv.post_config)
	{
		return lcd_drv.post_config();
	}
	else
	{
		ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}
}

GP_RET_CODE drv_lcd_get_resolution( unsigned int *width, unsigned int *height, unsigned int *rotation )
{
	if (lcd_drv.get_resolution)
	{
		return lcd_drv.get_resolution(width, height, rotation);
	}
	else
	{
		ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}
}

GP_RET_CODE drv_lcd_set_backlight( void )
{
	if (lcd_drv.set_backlight)
	{
		return lcd_drv.set_backlight();
	}
	else
	{
		ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}
}

GP_RET_CODE drv_lcd_get_brightness( unsigned char *brightness )
{
	if (lcd_drv.set_brightness) 
	{
		return lcd_drv.get_brightness(brightness);
	}
	else
	{
		ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}
}

GP_RET_CODE drv_lcd_set_brightness( unsigned char brightness )
{
	if (lcd_drv.set_brightness)
	{
		return lcd_drv.set_brightness(brightness);
	}
	else
	{
		ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}
}

GP_RET_CODE drv_lcd_set_contrast( void )
{
 	if (lcd_drv.set_contrast)
 	{
 		return lcd_drv.set_contrast();
 	}
 	else
 	{
 		ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);		/* pgr0822 */
 		return GP_RET_FAILURE;
 	}
}

GP_RET_CODE drv_lcd_set_color_balance( void )
{
	if (lcd_drv.set_color_balance)
	{
		return lcd_drv.set_color_balance();
	}
	else
	{
		ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);		/* pgr0822 */
		return GP_RET_FAILURE;
	}
}

#if 0
bool drv_lcd_available( void )
{
	return true;
}
#endif

GP_RET_CODE drv_lcd_write_cmd( unsigned short cmd )
{
	GP_RET_CODE retval = GP_RET_FAILURE;

#if defined D_LCD_DRV_ST7796S_RGB_DELTA
	retval = drv_st7796s_write_cmd(cmd);
#elif defined (D_LCD_DRV_LPM030M369F_YUV_STRIPE)
#endif

	return retval;
}

GP_RET_CODE drv_lcd_write_reg_u8( unsigned char data )
{
	GP_RET_CODE retval = GP_RET_FAILURE;

#if defined D_LCD_DRV_ST7796S_RGB_DELTA
	retval = drv_st7796s_write_reg_u8(data);
#elif defined (D_LCD_DRV_LPM030M369F_YUV_STRIPE)
#endif

	return retval;
}

GP_RET_CODE drv_lcd_set_pattern( unsigned int x_start, unsigned int y_start,
                           unsigned int width, unsigned int height, unsigned int color )
{
	GP_RET_CODE retval = GP_RET_FAILURE;

	// TODO: need to add
#if defined D_LCD_DRV_ST7796S_RGB_DELTA
	retval = drv_st7796s_set_pattern(x_start, y_start, width, height, color);
#elif defined (D_LCD_DRV_LPM030M369F_YUV_STRIPE)
#endif

	return retval;
}



#else	// original code
#include <gp_error.h>
#include <gp_debug.h>
#include <gp_assert.h>

#include <rtos.h>

#include <platform.h>
#include <drv_config.h>
#include <drv_lcd.h>

#include <camera_drv_config.h>

#define DBG
#ifdef DBG
#define DBG_MSG gp_print
#else
#define DBG_MSG(...)
#endif

#define ERR_MSG gp_print_error

static struct drv_lcd_t lcd_drv = {
    .enable            = NULL,
    .disable           = NULL,
    .config            = NULL,
    .post_config       = NULL,
    .get_resolution    = NULL,
    .set_backlight     = NULL,
    .get_brightness    = NULL,
    .set_brightness    = NULL,
    .set_contrast      = NULL,
    .set_color_balance = NULL,
};

extern void test_drv_lcd_init(void);

gp_error drv_lcd_init(void)
{
    int retval = GP_FAILURE;
    struct drv_config_s * drv_config;

    printf("%s: ---- ENTER\n", __FUNCTION__);

    drv_config = drv_config_get();

    if ( drv_config->lcd_config.init != NULL )
    {
        retval = drv_config->lcd_config.init(&lcd_drv);
    }

    if (retval) {
        ERR_MSG("%s: Failed", __FUNCTION__);
    }
    test_drv_lcd_init();

    return retval;
}

gp_error drv_lcd_event_init(void)
{
    return GP_SUCCESS;
}

gp_error drv_lcd_enable(void)
{
    if (lcd_drv.enable) {
        return lcd_drv.enable();
    } else {
        ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);
        return GP_FAILURE;
    }
}

gp_error drv_lcd_disable(void)
{
    if (lcd_drv.disable) {
        return lcd_drv.disable();
    } else {
        ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);
        return GP_FAILURE;
    }
}

gp_error drv_lcd_config(void)
{
    if (lcd_drv.config) {
        return lcd_drv.config();
    } else {
        ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);
        return GP_FAILURE;
    }
}

gp_error drv_lcd_post_config(void)
{
    if (lcd_drv.post_config) {
        return lcd_drv.post_config();
    } else {
        ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);
        return GP_FAILURE;
    }
}

gp_error drv_lcd_get_resolution(uint32_t *width, uint32_t *height, int32_t *rotation)
{
    int retval = GP_FAILURE;

    if (lcd_drv.get_resolution) {
        retval = lcd_drv.get_resolution(width, height, rotation);
    } else {
        ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);
    }

    return retval;
}

gp_error drv_lcd_set_backlight(void)
{
    if (lcd_drv.set_backlight) {
        return lcd_drv.set_backlight();
    } else {
        ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);
        return GP_FAILURE;
    }
}

gp_error drv_lcd_get_brightness(uint8_t *brightness)
{
    int32_t retval = GP_FAILURE;

    if (lcd_drv.set_brightness) {
        retval = lcd_drv.get_brightness(brightness);
    } else {
        ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);
    }

    return retval;
}

gp_error drv_lcd_set_brightness(uint8_t brightness)
{
    int32_t retval = GP_FAILURE;

    if (lcd_drv.set_brightness) {
        retval = lcd_drv.set_brightness(brightness);
    } else {
        ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);
    }

    return retval;
}

gp_error drv_lcd_set_contrast(void)
{
    if (lcd_drv.set_contrast) {
        return lcd_drv.set_contrast();
    } else {
        ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);
        return GP_FAILURE;
    }
}

gp_error drv_lcd_set_color_balance(void)
{
    if (lcd_drv.set_color_balance) {
        return lcd_drv.set_color_balance();
    } else {
        ERR_MSG("%s: Function pointer not initialized", __FUNCTION__);
        return GP_FAILURE;
    }
}

bool drv_lcd_available(void)
{
#ifdef HAWAII_PLAT
    return target_is_backdoor();
#else
    return true;
#endif
}

gp_error drv_lcd_write_cmd(uint16_t cmd)
{
    gp_error retval = GP_FAILURE;
    struct drv_config_s * drv_config;
    drv_config = drv_config_get();
    retval = drv_config->lcd_config.write_cmd(cmd);

    if (retval) {
        //ERR_MSG("%s: Failed", __FUNCTION__);
    }
    return retval;
}

gp_error drv_lcd_write_reg_u8(uint8_t data)
{
    gp_error retval = GP_FAILURE;
    struct drv_config_s * drv_config;
    drv_config = drv_config_get();

    if ( drv_config->lcd_config.write_reg_u8 != NULL )
    {
        retval = drv_config->lcd_config.write_reg_u8(data);
        if (retval)
        {
            //ERR_MSG("%s: Failed", __FUNCTION__);
        }
    }
    else
    {
        ERR_MSG("%s: Failed due to NULL lcd_write_reg_u8 function pointer", __FUNCTION__);
        retval = GP_FAILURE;
    }

    return retval;
}

gp_error drv_lcd_set_pattern(uint32_t x_start, uint32_t y_start,
                             uint32_t width, uint32_t height, uint32_t color)
{

    gp_error retval = GP_FAILURE;
    struct drv_config_s * drv_config;
    drv_config = drv_config_get();
    retval = drv_config->lcd_config.set_pattern(x_start,y_start,width,height,color);
    if (retval) {
        ERR_MSG("%s: Failed", __FUNCTION__);
    }
    return retval;

}
#endif	// <----- Porting by SNI end
