/**
 * @file		drv_lcd.h
 * @brief		LCD Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _DRV_LCD_H_
#define _DRV_LCD_H_

#if 1	// Porting by SNI start ----->

//#include <stdbool.h>
#include <stdio.h>

#define ERR_MSG printf

#define RTOS_MAX_NAME_LENGTH	12

typedef enum{
	GP_RET_SUCCESS = 0x0000,	// OK    
	GP_RET_FAILURE				// Error 
} GP_RET_CODE;

struct drv_lcd_t {
    char name[RTOS_MAX_NAME_LENGTH];
    GP_RET_CODE (*enable)( void );
    GP_RET_CODE (*disable)( void );
    GP_RET_CODE (*config)( void );
    GP_RET_CODE (*post_config)( void );
    GP_RET_CODE (*get_resolution)( unsigned int *width, unsigned int *height, unsigned int *rotation );
    GP_RET_CODE (*set_backlight)( void );
    GP_RET_CODE (*get_brightness)( unsigned char *brightness );
    GP_RET_CODE (*set_brightness)( unsigned char brightness );
    GP_RET_CODE (*set_contrast)( void );
    GP_RET_CODE (*set_color_balance)( void );
};

#if 0	// Unknown
struct lcd_vout_config {
    unsigned int pixel_clk;
    unsigned char pixel_repetition;
    DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s pixel_format;
    DSP_VOUT_DISPLAY_TIMING_CONFIG_s display_timing;
};
#endif

unsigned int drv_lcd_init( void );
GP_RET_CODE drv_lcd_event_init( void );

GP_RET_CODE drv_lcd_enable( void );
GP_RET_CODE drv_lcd_disable( void );
GP_RET_CODE drv_lcd_config( void );
GP_RET_CODE drv_lcd_post_config( void );
GP_RET_CODE drv_lcd_get_resolution( unsigned int *width, unsigned int *height, unsigned int *rotation );
GP_RET_CODE drv_lcd_set_backlight( void );
GP_RET_CODE drv_lcd_get_brightness( unsigned char *brightness );
GP_RET_CODE drv_lcd_set_brightness( unsigned char brightness );
GP_RET_CODE drv_lcd_set_contrast( void );
GP_RET_CODE drv_lcd_set_color_balance( void );
GP_RET_CODE drv_lcd_write_cmd( unsigned short cmd );
GP_RET_CODE drv_lcd_write_reg_u8( unsigned char data );
GP_RET_CODE drv_lcd_set_pattern( unsigned int x_start, unsigned int y_start,
                                 unsigned int width, unsigned int height, unsigned int color );
#if 0	// Unknown
bool drv_lcd_available( void );
#endif



#else	// original
#include <rtos.h>
#include <drv_config.h>

struct drv_lcd_t {
    char name[RTOS_MAX_NAME_LENGTH];
    gp_error (*enable)(void);
    gp_error (*disable)(void);
    gp_error (*config)(void);
    gp_error (*post_config)(void);
    gp_error (*get_resolution)(uint32_t *width, uint32_t *height, int32_t *rotation);
    gp_error (*set_backlight)(void);
    gp_error (*get_brightness)(uint8_t *brightness);
    gp_error (*set_brightness)(uint8_t brightness);
    gp_error (*set_contrast)(void);
    gp_error (*set_color_balance)(void);
};

struct lcd_vout_config {
    unsigned int pixel_clk;
    unsigned char pixel_repetition;
    DSP_VOUT_DISPLAY_DIGITAL_CONFIG_s pixel_format;
    DSP_VOUT_DISPLAY_TIMING_CONFIG_s display_timing;
};

gp_error drv_lcd_init(void);
gp_error drv_lcd_event_init(void);

gp_error drv_lcd_enable(void);
gp_error drv_lcd_disable(void);
gp_error drv_lcd_config(void);
gp_error drv_lcd_post_config(void);
gp_error drv_lcd_get_resolution(uint32_t *width, uint32_t *height, int32_t *rotation);
gp_error drv_lcd_set_backlight(void);
gp_error drv_lcd_get_brightness(uint8_t *brightness);
gp_error drv_lcd_set_brightness(uint8_t brightness);
gp_error drv_lcd_set_contrast(void);
gp_error drv_lcd_set_color_balance(void);
bool drv_lcd_available(void);

gp_error drv_lcd_write_cmd(uint16_t cmd);
gp_error drv_lcd_write_reg_u8(uint8_t data);
gp_error drv_lcd_set_pattern(uint32_t x_start, uint32_t y_start,
                             uint32_t width, uint32_t height, uint32_t color);

#endif	// <----- Porting by SNI end

#endif /* _DRV_LCD_H_ */
