/**
 * @file		lcd_casio.c
 * @brief 		LCD driver for CASIO
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "dd_top.h"
#include "LCDCtrl.h"
#include "debug.h"
#include "os_user_custom.h"

#if( LCD_TYPE_KIND == D_LCDTYPE_CASIO)
#define WAIT_CLK	(10)	// ### MILB_Porting

// define -----------------------------------------------------------
//#define _LCDC_PWR_ON	Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_PG7, 1);	// LCD_RST = 1;
//#define _LCDC_PWR_OFF	Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_PG7, 0);	// LCD_RST = 0;

#define _LCDC_ENABLE	LCD_SEN_PDR_SET(0);	/* LCDC Enable */
#define _LCDC_DISABLE	LCD_SEN_PDR_SET(1);	/* LCDC Disable */

#define _LCDC_DATA(val)		LCD_SDA_PDR_SET(val)			/* SDATA */
#define _LCDC_CLOCK_ON		LCD_SCK_PDR_SET(1);	/* SCLK ↑ */
#define _LCDC_CLOCK_OFF	LCD_SCK_PDR_SET(0);	/* SCLK ↓ */

#define _WAIT_NOP		{__asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");\
						 __asm__ volatile ("nop");}

/*CSIO channel number*/
#define CSIO_CH_1		(1)		/**<CSIO channel 1*/
#define CSIO_CH_2		(2)		/**<CSIO channel 2*/
#define CSIO_CH_3		(3)		/**<CSIO channel 3*/
#define CSIO_CH_4		(4)		/**<CSIO channel 4*/
#define CSIO_CH_5		(5)		/**<CSIO channel 5*/
#define CSIO_CH_6		(6)		/**<CSIO channel 6*/


// variable -----------------------------------------------------------
static UCHAR reg[][2] = {
//	addr	data
	{ 0x03,	0x01 },	// 垂直バックポーチ
	{ 0x04,	0x09 },	// 水平バックポーチ(8～514)。実際は＋３される
	{ 0x05,	0x0 },	// 水平バックポーチ
	{ 0x06,	128 },	// 水平期間クロック数。実際はx8が設定される
//	{ 0x07,	0x0 },	// RGB I/F1
	{ 0x07,	0x06 },	// RGB I/F1	// RK5のLCDが向きが逆のため上下反転させる
	{ 0x09,	0x0 },	// RGB I/F2
	{ 0x00,	0x0 },	// PANEL1
	{ 0x01,	0x21 },	// PANEL2(VCOM)
	{ 0x02,	0xab },	// PANEL3
	{ 0x0a,	0x08 },	// PANEL4
	{ 0x0b,	0x07 },	// PANEL5
	{ 0x0c,	0x0 },	// PANEL6
	{ 0x0d,	0x0 },	// PANEL7
	{ 0x10,	0x03 },	// gamma		GAMADJ=1
	{ 0x11,	0x00 },	// R gamma1
	{ 0x12,	0x40 },	// R gamma2
	{ 0x13,	0x32 },	// R gamma3
	{ 0x14,	0x20 },	// R gamma4
	{ 0x15,	0x7e },	// R gamma5
	{ 0x16,	0x0a },	// R gamma6
	{ 0x17,	0x0 },	// G gamma1
	{ 0x18,	0x36 },	// G gamma2
	{ 0x1a,	0x20 },	// G gamma3
	{ 0x1b,	0x69 },	// G gamma4
	{ 0x1c,	0x2e },	// G gamma5
	{ 0x1d,	0x0 },	// B gamma1
	{ 0x1f,	0x14 },	// B gamma2
	{ 0x20,	0x0a },	// B gamma3
	{ 0x21,	0x74 },	// B gamma4
	{ 0x22,	0x3e },	// B gamma5
	{ 0x23,	0x0 },	// image1		AUTO_IMAGE=0
	{ 0x24,	0x0 },	// image2
	{ 0x25,	0x0 },	// image3
	{ 0x26,	0x48 },	// image4
	{ 0x27,	0xbf },	// back light1
	{ 0x28,	0x01 },	// back light2
	{ 0x29,	0x0 },	// back light3	AUTO_BL=0
	{ 0x2a,	0x04 },	// auto BLC1
	{ 0x2b,	0x08 },	// auto BLC2
	{ 0x2c,	0x0c },	// auto BLC3
	{ 0x2d,	0x10 },	// auto BLC4
	{ 0x2e,	0x14 },	// auto BLC5
	{ 0x2f,	0x18 },	// auto BLC6
	{ 0x30,	0x1c },	// auto BLC7
	{ 0x31,	0x10 },	// auto BLC8
	{ 0x32,	0x24 },	// auto BLC9
	{ 0x33,	0x28 },	// auto BLC10
	{ 0x34,	0x2c },	// auto BLC11
	{ 0x35,	0x30 },	// auto BLC12
	{ 0x36,	0x34 },	// auto BLC13
	{ 0x37,	0x38 },	// auto BLC14
	{ 0x38,	0x3c },	// auto BLC15
	{ 0x39,	0x3f },	// auto BLC16
	{ 0x07,	0x16 },	// RGB I/F1	// RK5のLCDが向きが逆のため上下反転させる
	{ 0x00,	0x4c },	// // BRIGHT <- initial value
	{ 0xff,	0xff },	// stopper
};


// function -----------------------------------------------------------

/*==========================================================================
	SetCOM27T2M31U
		D-LCDCコマンドシリアル出力
==========================================================================*/
#if D_LCDCTRL_WITH_GPIO//control lcd with GPIO port
static VOID SetCOM27T2M31U( USHORT* data )
{
// address/dataで16bitのためSIOで送信できる
// 最初は下記にしておく
	
	INT32	i, j;
	UCHAR	bit;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	
	_LCDC_DATA(0);
	//Wait 20ns
	for(j=0;j<WAIT_CLK;j++){
		_WAIT_NOP
	}

	_LCDC_CLOCK_ON						/* SCLK=1 */

	//Wait 20ns
	for(j=0;j<WAIT_CLK;j++){
		_WAIT_NOP
	}

	_LCDC_ENABLE						/* LCDCイネーブル */
	//Wait 20ns
	for(j=0;j<WAIT_CLK;j++){
		_WAIT_NOP
	}
	
	for ( i = 0 ; i < 16 ; i++ ){	// address=8bit, data=8bit
		bit = ( *data >> i ) & 1 ;
		_LCDC_DATA( (bit & 0x01) );	/* SDAT Set */
		
		_LCDC_CLOCK_OFF;				/* SCLK=0 */
		
	//Wait 20ns
		for(j=0;j<WAIT_CLK;j++){
			_WAIT_NOP
		}
		
		_LCDC_CLOCK_ON;					/* SCLK=1 */
		
	//Wait 20ns
		for(j=0;j<WAIT_CLK;j++){
			_WAIT_NOP
		}
	}
	
	_LCDC_DISABLE						/* LCDCディゼーブル */
	//Wait 20ns
	for(j=0;j<WAIT_CLK;j++){
		_WAIT_NOP
	}
	
	Dd_ARM_Critical_Section_End( critseq );
}
#else//control lcd with CSIO
static VOID SetCOM27T2M31U( USHORT* data )
{
	FJ_SIO_CTRL csio_ctrl;
	FJ_SIO_CTRL_TRANSFER trans_ctrl;

	csio_ctrl.type = FJ_SIO_TYPE_MASTER;
	csio_ctrl.mode = FJ_SIO_MODE_NORMAL_SC_H;
	csio_ctrl.bit_direction = FJ_SIO_BIT_DIR_LSB_FIRST;//MSB first
	csio_ctrl.data_length = FJ_SIO_LENGTH_8;
	csio_ctrl.baudrate = 115200;
	csio_ctrl.pcallback = NULL; 

	/* Set LCD corresponding ports to external functionality */
	LCD_SDA_EPCR_SET(1);
	LCD_SCK_EPCR_SET(1);

	/* Set LCD corresponding ports to output direction */
	LCD_SCK_DDR_SET(1);
	LCD_SDA_DDR_SET(1);
	LCD_SEN_DDR_SET(1);

	/*Open and ctrl CSIO*/
	if(FJ_ERR_OK != FJ_SIO_Open(FJ_PERI_CH_3)){
		BF_Debug_Print_Error(("LCD control: FJ_SIO_Open:Error!!\n"));
		return;
	}

	/*CSIO settting*/
	if(FJ_ERR_OK != FJ_SIO_Ctrl(FJ_PERI_CH_3,&csio_ctrl)){
		FJ_SIO_Close(CSIO_CH_3);
		BF_Debug_Print_Error(("LCD control: FJ_SIO_Ctrl:Error!!\n"));
		return;
	}

	/* Enable serial port date transfer */
	_LCDC_ENABLE
	
	trans_ctrl.direction = FJ_SIO_DATA_DIR_SEND;
//	trans_ctrl.port = FJ_GPIO_PORT_MAX;
//	trans_ctrl.status = FJ_GPIO_STATUS_HIGH;
	trans_ctrl.data_ptr = data;
	trans_ctrl.data_count = 2;
	if(FJ_ERR_OK != FJ_SIO_Transfer( FJ_PERI_CH_3, &trans_ctrl, FJ_SIO_SEND_DMA_MODE_DEMAND )){
		BF_Debug_Print_Error(("LCD control: FJ_SIO_Send:NG!!\n"));
	}
	_LCDC_DISABLE

	/*Close and ctrl CSIO*/
	if(FJ_ERR_OK != FJ_SIO_Close(FJ_PERI_CH_3)){
		BF_Debug_Print_Error(("LCD control: FJ_SIO_Close:Error!!\n"));
	}	
}
#endif//LCD_CTRL_WITH_GPIO

VOID send_lcd_cmd(USHORT addr, USHORT data){
	USHORT write_data;
	write_data = (( data <<8) & 0xFF00) | (addr & 0x00FF);// address=8bit, data=8bit
	SetCOM27T2M31U( &write_data);
}
/**
 * Write register of com27t2m31u LCD.
 *
 * @param addr - Address of register.
 * @param data - Data to write in register.
 * @return (LCD_ERR_OK) for success, otherwise failure.
 */
static INT32 com27t2m31u_wr_reg( UCHAR addr, UCHAR data )
{
	send_lcd_cmd(addr,data);
	return (LCD_ERR_OK);
}
/**
 * Set brightness of LCD.
 *
 * @param brightness - FFh for dark, 50h for center, 00h for bright
 * @return (LCD_ERR_OK) for success, otherwise failure.
 */
static int brightness( UCHAR brightness_level )
{
	return com27t2m31u_wr_reg(0x00, brightness_level);
}

/*==========================================================================
	カシオ製D-LCD初期化
		COM27T2M31U
==========================================================================*/
void lcd_set_hook( void)
{
	struct lcd_drv_s lcd;
	strcpy(lcd.name, "COM27T2M31U");
	lcd.backlight_ctrl 		= NULL;
	lcd.config 				= NULL;
	lcd.brightness 			= brightness;
	lcd.contrast 			= NULL;
	lcd.rb_sub_contrast		= NULL;
	lcd.rb_sub_brightness 	= NULL;
	lcd.power_saving 		= NULL;
	lcd.aspect_ratio 		= NULL;
	lcd.saturation 	        = NULL;
	lcd.hue          	    = NULL;
	lcd.wr_reg				= com27t2m31u_wr_reg;
	lcd.rd_reg				= NULL;
	FJ_LCD_Drv_Attach(&lcd);
}
/**
 * @brief	LCD initialize
 * @param	none
 * @return	none
 * @note	set LCD stanby state
 */
INT32 lcd_init(INT32 bl_enable)
{
	INT32 i;
	
#if 1// Guarantee Seiral bus port setting
	/* Set LCD corresponding ports to GPIO functionality */
#if D_LCDCTRL_WITH_GPIO//control lcd with GPIO port
	LCD_SCK_EPCR_SET(0);
	LCD_SDA_EPCR_SET(0);
#else//control lcd with CSIO 
	LCD_SCK_EPCR_SET(1);
	LCD_SDA_EPCR_SET(1);
#endif//LCD_CTRL_WITH_GPIO

	/* Set LCD corresponding ports to output direction */
	LCD_SCK_DDR_SET(1);
	LCD_SDA_DDR_SET(1);
	LCD_SEN_DDR_SET(1);

	/* LCD signal initialization */
	LCD_SEN_PDR_SET(1);
	LCD_SDA_PDR_SET(0);
	LCD_SCK_PDR_SET(0);
#endif

	// Power On
//	_LCDC_PWR_ON
	LCD_BLC_PDR = 1;

	// Wait(1msec)
	OS_User_Dly_Tsk(2);

	// initialize setting
	for( i = 0 ; reg[i][0] != 0xFF ; i++ ) {
		send_lcd_cmd(reg[i][0], reg[i][1]);
	}
	
	OS_User_Dly_Tsk(10) ;
	lcd_set_hook();
	return (LCD_ERR_OK);
}


///**
// * @brief	Release Stanby
// * @param	none
// * @return	none
// * @note	none
// */
//void LCD_ReleaseStanby(void)
//{
//	// release stanby state
////	send_lcd_cmd(	7,	0x10);	// release stanby
//	send_lcd_cmd(	7,	0x16);	// release stanby	// RK5のLCDが向きが逆のため上下反転させる
//	send_lcd_cmd(	0,	0x4c);	// BRIGHT <- initial value
//	
//	// wait 5field
//	OS_dly_tsk(100);// 暫定処理
//}
#endif // LCD_TYPE_KIND == D_LCDTYPE_CASIO
