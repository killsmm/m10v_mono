/**
 * @file		drv_lcd_st7796s.h
 * @brief		LCD ST7796S Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _LCD_ST7796S_H_
#define _LCD_ST7796S_H_

#if 1	// Porting by SNI start ----->

#define LCD_LANDSCAPE_MODE

#define ST7796s_DEV_NAME       "st7796s lcd"

#define CMD_SW_RESET           0x01
#define CMD_SLEEP_IN           0x10
#define CMD_SLEEP_OUT          0x11
#define CMD_NORMAL_MODE_ON     0x13

#define CMD_INVERSION_ON       0x21
#define CMD_DISPLAY_OFF        0x28
#define CMD_DISPLAY_ON         0x29
#define CMD_COL_ADDR_SET       0x2A
#define CMD_ROW_ADDR_SET       0x2B

#define CMD_MEM_ACESS_CTRL     0x36
#define CMD_IDLE_MODE_OFF      0x38
#define CMD_PIXEL_FORMAT       0x3A

#define CMD_IF_MODE            0xB0
#define CMD_FRAME_RATE_CTRL    0xB1
#define CMD_PORCH_SETTING      0xB2
#define CMD_FR_CTRL1           0xB3
#define CMD_INVERSION_CTRL     0xB4
#define CMD_BLANK_PORCH_CTRL   0xB5
#define CMD_DISPLAY_FUNC       0xB6

#define CMD_ENTRY_MODE         0xB7
#define CMD_GATE_TIMING_ADJ    0xB8

#define CMD_DGM_EN             0xBA
#define CMD_VCOM_SETTING       0xBB

#define CMD_PWR_CTRL1          0xC0
#define CMD_PWR_CTRL2          0xC1
#define CMD_VDV_VRH_ENA        0xC2
#define CMD_VRH_SET            0xC3
#define CMD_VDV_SET            0xC4
#define CMD_VCOM_CTRL          0xC5
#define CMD_FR_CTRL            0xC6

#define CMD_VAPVAN_EN          0xD2

#define CMD_PV_GAMMA_CTRL      0xE0
#define CMD_NV_GAMMA_CTRL      0xE1
#define CMD_G_CTRL             0xE4
#define CMD_SPI2_EN            0xE7
#define CMD_DRV_SIGNAL_CTRL    0xE8
#define CMD_EQ_CTRL            0xE9
#define CMD_DISPLAY_CTRL       0xEB
#define CMD_PROM_CTRL          0xEC

#define CMD_TABLE2_ENABLE      0xF0
#define CMD_PROM_EN            0xFA
#define CMD_SPI_READ_ENABLE    0xFB
#define CMD_NVM_SETTING        0xFC
#define CMD_PROM_ACT           0xFE

#if defined (LCD_LANDSCAPE_MODE)
#define ST7796S_LCD_WIDTH       480
#define ST7796S_LCD_HEIGHT      320
#define ST7796S_LCD_ROTATION    (0)
#else
#define ST7796S_LCD_WIDTH       320
#define ST7796S_LCD_HEIGHT      480
#define ST7796S_LCD_ROTATION    (1)
#endif

GP_RET_CODE drv_st7796s_init( struct drv_lcd_t *lcd_dev );
GP_RET_CODE drv_st7796s_write_cmd( unsigned short cmd );
GP_RET_CODE drv_st7796s_write_reg_u8( unsigned char data );

GP_RET_CODE drv_st7796s_set_pattern( unsigned int x_start, unsigned int y_start,
                                     unsigned int width, unsigned int height, unsigned int color );

#else	// original code
#include <drv_config.h>

#define ST7796s_DEV_NAME       "st7796s lcd"

#define CMD_SW_RESET           0x01
#define CMD_SLEEP_IN           0x10
#define CMD_SLEEP_OUT          0x11
#define CMD_NORMAL_MODE_ON     0x13

#define CMD_INVERSION_ON       0x21
#define CMD_DISPLAY_OFF        0x28
#define CMD_DISPLAY_ON         0x29
#define CMD_COL_ADDR_SET       0x2A
#define CMD_ROW_ADDR_SET       0x2B

#define CMD_MEM_ACESS_CTRL     0x36
#define CMD_IDLE_MODE_OFF      0x38
#define CMD_PIXEL_FORMAT       0x3A

#define CMD_IF_MODE            0xB0
#define CMD_FRAME_RATE_CTRL    0xB1
#define CMD_PORCH_SETTING      0xB2
#define CMD_FR_CTRL1           0xB3
#define CMD_INVERSION_CTRL     0xB4
#define CMD_BLANK_PORCH_CTRL   0xB5
#define CMD_DISPLAY_FUNC       0xB6

#define CMD_ENTRY_MODE         0xB7
#define CMD_GATE_TIMING_ADJ    0xB8

#define CMD_DGM_EN             0xBA
#define CMD_VCOM_SETTING       0xBB

#define CMD_PWR_CTRL1          0xC0
#define CMD_PWR_CTRL2          0xC1
#define CMD_VDV_VRH_ENA        0xC2
#define CMD_VRH_SET            0xC3
#define CMD_VDV_SET            0xC4
#define CMD_VCOM_CTRL          0xC5
#define CMD_FR_CTRL            0xC6

#define CMD_VAPVAN_EN          0xD2

#define CMD_PV_GAMMA_CTRL      0xE0
#define CMD_NV_GAMMA_CTRL      0xE1
#define CMD_G_CTRL             0xE4
#define CMD_SPI2_EN            0xE7
#define CMD_DRV_SIGNAL_CTRL    0xE8
#define CMD_EQ_CTRL            0xE9
#define CMD_DISPLAY_CTRL       0xEB
#define CMD_PROM_CTRL          0xEC

#define CMD_TABLE2_ENABLE      0xF0
#define CMD_PROM_EN            0xFA
#define CMD_SPI_READ_ENABLE    0xFB
#define CMD_NVM_SETTING        0xFC
#define CMD_PROM_ACT           0xFE

#if defined (LCD_LANDSCAPE_MODE)
#define ST7796S_LCD_WIDTH       480
#define ST7796S_LCD_HEIGHT      320
#define ST7796S_LCD_ROTATION    (0)
#else
#define ST7796S_LCD_WIDTH       320
#define ST7796S_LCD_HEIGHT      480
#define ST7796S_LCD_ROTATION    (1)
#endif

gp_error drv_st7796s_init(struct drv_lcd_t *lcd_dev);
uint16_t drv_st7796s_write_cmd(uint16_t cmd);
uint16_t drv_st7796s_write_reg_u8(uint8_t data);

gp_error drv_st7796s_set_pattern(uint32_t x_start, uint32_t y_start,
                                uint32_t width, uint32_t height, uint32_t color);

#endif	// <----- Porting by SNI end

#endif /* _LCD_ST7796S_H_ */
