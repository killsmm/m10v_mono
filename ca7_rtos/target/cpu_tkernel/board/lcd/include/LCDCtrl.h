/**
 * @file		LCDCtrl.h
 * @brief 		LCD controller
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _LCDCTRL_H
#define _LCDCTRL_H

#include "fj_peripheral.h"

extern UCHAR DUMMY_PORT; // Mean that no exsits or not support

/*--------------------------------------------------------------------------*/
/* Definition				 												*/
/*--------------------------------------------------------------------------*/
/* LCD ctrl port Compile Option */
#define D_LCDCTRL_WITH_GPIO			(TRUE)// true:control with GPIO port / false:control with serial(CSIO) port.


/* LCD Driver Type (Manufacture) */
#define D_LCDTYPE_NOTHING			(0x0000)	/* (not support)		*/
#define D_LCDTYPE_NT39018			(0x0001)	/* NOVATEK Driver		*/
#define D_LCDTYPE_A024CN02			(0x0002)	/* AUO Driver			*/
#define D_LCDTYPE_CASIO    			(0x0004)	/* CASIO 				*/
#define D_LCDTYPE_A027CN01			(0x0008)	/* Reserve4 			*/
#define D_LCDTYPE_RESERVE5			(0x0010)	/* Reserve5 			*/
#define D_LCDTYPE_RESERVE6			(0x0020)	/* Reserve6 			*/
#define D_LCDTYPE_RESERVE7			(0x0040)	/* Reserve7 			*/
#define D_LCDTYPE_RESERVE8			(0x0080)	/* Reserve8 			*/

/* LCD Mode */
#define D_LCDMODE_NOTHING			(0x0000)	/* (not support)		*/
#define D_LCDMODE_RGB_DELTA			(0x0001)	/* 8bit RGB				*/
#define D_LCDMODE_RGB_DUMMY_320		(0x0002)	/* 8bit RGB 320 Dummy	*/
#define D_LCDMODE_RGB_DUMMY_360		(0x0003)	/* 8bit RGB 360 Dummy	*/
#define D_LCDMODE_YUV640_NTSC		(0x0004)	/* YUV640(NTSC)		 	*/
#define D_LCDMODE_YUV640_PAL		(0x0005)	/* YUV640(PAL)		 	*/
#define D_LCDMODE_YUV720_NTSC		(0x0006)	/* YUV720(NTSC)			*/
#define D_LCDMODE_YUV720_PAL		(0x0007)	/* YUV720(PAL)			*/
#define D_LCDMODE_CCIR656			(0x0008)	/* CCIR656				*/

/* Available combination */
/* select option file    */
//#define D_LCD_DRV_NOVATEK_RGB_DELTA	(D_LCDTYPE_NT39018 | (D_LCDMODE_RGB_DELTA << 16))		/* NOVATEK RGB mode 		*/
//#define D_LCD_DRV_NOVATEK_RGB_DUMMY	(D_LCDTYPE_NT39018 | (D_LCDMODE_RGB_DUMMY_320 << 16))	/* NOVATEK RGB Dummy mode 	*/
//#define D_LCD_DRV_NOVATEK_YUV640		(D_LCDTYPE_NT39018 | (D_LCDMODE_YUV640_NTSC << 16))		/* NOVATEK YUV640 mode 		*/
//
//#define D_LCD_DRV_AUO_RGB_DELTA		(D_LCDTYPE_A024CN02 | (D_LCDMODE_RGB_DELTA << 16))		/* AUO RGB mode 			*/
//#define D_LCD_DRV_AUO_RGB_DUMMY		(D_LCDTYPE_A024CN02 | (D_LCDMODE_RGB_DUMMY_320 << 16))	/* AUO RGB Dummy mode 		*/
//#define D_LCD_DRV_AUO_YUV640			(D_LCDTYPE_A024CN02 | (D_LCDMODE_YUV640_NTSC << 16))	/* AUO YUV640 mode 			*/

#define DLCD_ENABLE_TYPE			(D_LCDTYPE_NT39018 | D_LCDTYPE_A024CN02|D_LCDTYPE_CASIO|D_LCDTYPE_A027CN01)				/* Supported LCD type		*/

/********************************************************************************************************/
/* actual mode : If you want to change the LCD type and mode, please select from Available combination. */
/* select option file                                                                                   */
/********************************************************************************************************/
//<----- for NOVATEK(NT39018) 
#ifdef D_LCD_DRV_NOVATEK_RGB_DELTA
#define _DLCD_MODE		(D_LCD_DRV_NOVATEK_RGB_DELTA)	// 1) RGB Delta
#endif
#ifdef D_LCD_DRV_NOVATEK_RGB_DUMMY
#define _DLCD_MODE		(D_LCD_DRV_NOVATEK_RGB_DUMMY)	// 2) RGB Dummy
#endif
#ifdef D_LCD_DRV_NOVATEK_YUV640
#define _DLCD_MODE		(D_LCD_DRV_NOVATEK_YUV640)		// 3) YUV640(NTSC)
#endif

//<----- for AUO(A024CN02) 
#ifdef D_LCD_DRV_AUO_RGB_DELTA
#define _DLCD_MODE		(D_LCD_DRV_AUO_RGB_DELTA)		// 4) RGB Delta
#endif
#ifdef D_LCD_DRV_AUO_RGB_DUMMY
#define _DLCD_MODE		(D_LCD_DRV_AUO_RGB_DUMMY)		// 5) RGB Dummy
#endif
#ifdef D_LCD_DRV_AUO_YUV640
#define _DLCD_MODE		(D_LCD_DRV_AUO_YUV640)			// 6) YUV640(NTSC)
#endif
//<----- for AUO(A024CN02) 
#ifdef D_LCD_DRV_CASIO_RGB_DELTA
#define _DLCD_MODE		(D_LCD_DRV_CASIO_RGB_DELTA)	// 7) RGB Delta(NTSC)
#endif
#ifdef D_LCD_DRV_AUO27_RGB_DELTA
#define _DLCD_MODE		(D_LCD_DRV_AUO27_RGB_DELTA)	// 8) RGB Delta(NTSC)
#endif
//<----- for Sitronix(ST7796S)
#ifdef D_LCD_DRV_ST7796S_RGB_DELTA
#define _DLCD_MODE		(D_LCD_DRV_ST7796S_RGB_DELTA)	// 9) RGB Delta
#endif

#ifdef D_LCD_DRV_LPM030M369F_YUV_STRIPE
#define _DLCD_MODE		(D_LCD_DRV_LPM030M369F_YUV_STRIPE)	// 10) YUV Stripe
#endif

#define LCD_TYPE_KIND	(_DLCD_MODE & 0x0000FFFF)	// LCD Driver Type
#define LCD_MODE_KIND	((_DLCD_MODE & 0xFFFF0000) >> 16)	// LCD Mode


//*******************************/
/* LCD control port definition	*/
//*******************************/
// Serial bus Enable port(CS)
#define LCD_SEN_DDR_GET(val)	Dd_Top_Get_Gpio_Direction(E_DD_TOP_GPIO_P80, &val)
#define LCD_SEN_DDR_SET(val)	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P80, (val))
#define LCD_SEN_PDR_GET(val)	Dd_Top_Get_Gpio_Status(E_DD_TOP_GPIO_P80, &val)
#define LCD_SEN_PDR_SET(val)	Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_P80, (val))
                                                                                 
// Serial bus Data port(SDA)                                                          
#define LCD_SDA_EPCR_GET(val)	Dd_Top_Get_Gpio_Function(E_DD_TOP_GPIO_P83, &val)
#define LCD_SDA_EPCR_SET(val)	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P83, (val))
#define LCD_SDA_DDR_GET(val)	Dd_Top_Get_Gpio_Direction(E_DD_TOP_GPIO_P83, &val)
#define LCD_SDA_DDR_SET(val)	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P83, (val))
#define LCD_SDA_PDR_GET(val)	Dd_Top_Get_Gpio_Status(E_DD_TOP_GPIO_P83, &val)
#define LCD_SDA_PDR_SET(val)	Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_P83, (val))
                                                                                 
// Serial bus Clock port(SCL)                                                         
#define LCD_SCK_EPCR_GET(val)	Dd_Top_Get_Gpio_Function(E_DD_TOP_GPIO_P85, &val);
#define LCD_SCK_EPCR_SET(val)	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P85, (val));
#define LCD_SCK_DDR_GET(val)	Dd_Top_Get_Gpio_Direction(E_DD_TOP_GPIO_P85, &val);
#define LCD_SCK_DDR_SET(val)	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P85, (val));
#define LCD_SCK_PDR_GET(val)	Dd_Top_Get_Gpio_Status(E_DD_TOP_GPIO_P85, &val);
#define LCD_SCK_PDR_SET(val)	Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_P85, (val));

// Backlight control port 
#define LCD_BLC_EPCR		(DUMMY_PORT)
#define LCD_BLC_DDR			(DUMMY_PORT)
#define LCD_BLC_PDR			(DUMMY_PORT)


/*--------------------------------------------------------------------------*/
/* Structure				 												*/
/*--------------------------------------------------------------------------*/


/* Error Code (unknown) */
typedef enum {
	LCD_ERR_OK				=  0,
	LCD_ERR_NG				= -1
} LCD_ERR_CODE;

/* Display mode of LCD. */
typedef enum {
	LCD_DISPLAY_NORMAL,            /**< Normal aspect ratio mode */
	LCD_DISPLAY_NARROW,            /**< Narrow aspect ratio mode */
	LCD_DISPLAY_WIDE               /**< Wide aspect ratio mode */
} LCD_DISPLAY_AR;

/* Register address definition */
typedef union {
	UCHAR byte;           /**< Byte of address */
	struct {
		UCHAR a7 : 1; /**< bit7 of address */
		UCHAR a6 : 1; /**< bit6 of address */
		UCHAR a5 : 1; /**< bit5 of address */
		UCHAR a4 : 1; /**< bit4 of address */
		UCHAR a3 : 1; /**< bit3 of address */
		UCHAR a2 : 1; /**< bit2 of address */
		UCHAR a1 : 1; /**< bit1 of address */
		UCHAR a0 : 1; /**< bit0 of address */
	} bit;
} lcd_reg_addr_t;

/**
 * Data structure contains service functions that need to be provided to the
 * lcd device manager by the device drivers.
 */
struct lcd_drv_s {
	CHAR name[64]; /**< Name of LCD device */
	/**
         * Enable/Disable the backlight of LCD.
         *
         * @param on_off - TRUE for enable, FALSE for disable.
         * @returns 0 for success or < 0 if error occured.
         */
	INT32 (*backlight_ctrl) (UCHAR on_off);
	/**
         * Configure the lcd device.
         *
         * @param mode - Signal input format.
         * @param ar - Display aspect ratio.
         * @returns 0 for success or < 0 if error occured.
         */
	INT32 (*config) ( INT32 mode, LCD_DISPLAY_AR ar);
	/**
         * Set brightness of LCD.
         *
         * @param brightness - Value of brightness.
         * @returns 0 for success or < 0 if error occured.
         */
	INT32 (*brightness) (UCHAR brightness);
	/**
         * Set contrast of LCD.
         *
         * @param contrast - Value of contrast.
         * @returns 0 for success or < 0 if error occured.
         */
	INT32 (*contrast) (UCHAR contrast);
	/**
         * Set contrast of R,B sub component.
         *
         * @param contrast - Value of contrast.
         * @returns 0 for success or < 0 if error occured.
         */
	INT32 (*rb_sub_contrast) (UCHAR contrast);
	/**
         * Set brightness of R,B sub component.
         *
         * @param brightness - Value of brightness.
         * @returns 0 for success or < 0 if error occured.
         */
	INT32 (*rb_sub_brightness) (UCHAR brightness);
	/**
         * Enable/Disable power mode.
         *
         * @param on_off - TRUE for normal mode, FALSE for standby mode.
         * @returns 0 for success or < 0 if error occured.
         */
	INT32 (*power_saving) (UCHAR on_off);
	/**
         * Set display aspect ratio of LCD.
         *
         * @param ar - Display mode of LCD.
         * @returns 0 for success or < 0 if error occured.
         */
	INT32 (*aspect_ratio) (LCD_DISPLAY_AR ar);
	/**
         * Set saturation of LCD.
         *
         * @param saturation - Value of saturation.
         * @returns 0 for success, otherwise failure.
         */
	INT32 (*saturation) (UCHAR saturation);
	/**
         * Set the Horizontal/Vertical scan direction setting.
         *
         * @param h_dir - Horizontal Scan Direction
         *                =0: Right to Left scan, =1: Left to Right scan(default)
         * @param v_dir - Vertical Scan Direction
         *                =0: Down to Up scan, =1: Up to Down scan(default)
         * @return 0 for success, otherwise failure.
         */
	INT32 (*display_direction) (UCHAR h_dir, unsigned char v_dir);
	/**
         * Set Hue of LCD.
         *
         * @param hue) - Value of Hue.
         * @returns 0 for success, otherwise failure.
         */
	INT32 (*hue) (UCHAR hue);
	/**
         * Write register of LCD.
         *
         * @param addr - Address of register.
         * @param data - Data to write in register.
         * @returns 0 for success, otherwise failure.
         */
//	int (*wr_reg)(lcd_reg_addr_t addr, unsigned char data);
	INT32 (*wr_reg)(UCHAR addr, UCHAR data);
	/**
         * Read register of LCD.
         *
         * @param addr - Address of register.
         * @param *data - Data pointer to read from register.
         * @returns 0 for success, otherwise failure.
         */
//	int (*rd_reg)(lcd_reg_addr_t addr, unsigned char *data);
	INT32 (*rd_reg)(UCHAR addr, UCHAR *data);
};

/* LCD type & mode */
typedef struct {
	USHORT type; /**< bit7 of address */
	USHORT mode; /**< bit6 of address */
} LCD_TYPE_MODE;

/*--------------------------------------------------------------------------*/
/* Prototype				 												*/
/*--------------------------------------------------------------------------*/
INT32	FJ_LCD_Drv_init( INT32 bl_ctrl );
INT32	FJ_LCD_Drv_Attach( struct lcd_drv_s *lcd) ;
INT32	FJ_LCD_Drv_Remove( VOID );
INT32	FJ_LCD_Drv_Config( LCD_DISPLAY_AR ar );
INT32	FJ_LCD_Drv_BackLight_Ctrl( UCHAR on_off );
INT32	FJ_LCD_Drv_Brightness( UCHAR brightness );
INT32	FJ_LCD_Drv_Contrast( UCHAR contrast );
INT32	FJ_LCD_Drv_RBsub_Contrast( UCHAR contrast );
INT32	FJ_LCD_Drv_RBsub_Brightness( UCHAR brightness );
INT32	FJ_LCD_Drv_Power( UCHAR on_off );
INT32	FJ_LCD_Drv_Aspect_Ratio( LCD_DISPLAY_AR ar );
INT32	FJ_LCD_Drv_Saturation( UCHAR saturation );
INT32	FJ_LCD_Drv_Display_Direction( UCHAR h_dir, UCHAR v_dir );
INT32	FJ_LCD_Drv_Hue( UCHAR hue );
INT32	FJ_LCD_Drv_Write_Reg( UCHAR addr, UCHAR data );
INT32	FJ_LCD_Drv_Read_Reg( UCHAR addr, UCHAR *data );

VOID	LCD_Init( INT32 bl_ctrl );
VOID	LCD_PowerUp( UCHAR pwr_saving );
VOID	LCD_PowerDown( UCHAR pwr_saving );
INT32	LCD_GetDrvType( VOID );
INT32	LCD_GetDrvMode( VOID );
INT32	LCD_GetVLDMode( INT32 re_set );
VOID	LCD_Set_Hook_Function(VOID);

INT32 lcd_init( INT32 bl_enable );

INT32	drv_lcd_init( void );

#endif	// _LCDCTRL_H

