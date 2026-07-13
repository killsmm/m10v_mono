/**
 * @file		lcd_config.h
 * @brief		Definition of LCD Configuration
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _LCD_CONFIG_H
#define _LCD_CONFIG_H

#ifdef D_LCD_DRV_ST7796S_RGB_DELTA
////////////////////////////////////////////////////////////////////////
//////////////////// ST7796S      //////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------*/
/* LCD Clock information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_CLOCK				(16800)		// LCD clock[KHz]  FrameWidth x FrameHeight x 59.94 ( = 60/1.001)

/*----------------------------------------------------------------------*/
/* LCD polarity information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_POLARITY_CLOCK		(1)			// Output clock edge (0:Data changes near the falling edge, 1:Data changes near the rising edge)
#define D_LCD_CONFIG_POLARITY_VSYNC		(1)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_HSYNV		(1)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_VENABLE	(0)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_HENABLE	(0)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)

/*----------------------------------------------------------------------*/
/* LCD timing information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_SCAN_MODE			(0)			// LCD scan mode (0:Progressive, 1:Interlaced)
#define D_LCD_CONFIG_VCYC				(500)		// Vertical cycle[Line]
#define D_LCD_CONFIG_VCYC_SHIFT			(0)			// Vsync output 0.5H shift enable(interlace only)
#define D_LCD_CONFIG_HCYC				(560)		// Horizontal cycle[Clock]
#define D_LCD_CONFIG_VPW				(4)			// Vertical synchronous signal pulse width[Line]
#define D_LCD_CONFIG_HPW				(25)		// Horizontal synchronous signal pulse width[Clock]
#define D_LCD_CONFIG_VBLK				(8)			// Vertical blank period[Line]
#define D_LCD_CONFIG_HBLK				(50)		// Horizontal blank period[Clock]
#define D_LCD_CONFIG_VSIZE				(320)		// Output data vertical size[Line]
#define D_LCD_CONFIG_HSIZE				(480)		// Output data horizontal size[Clock]

/*----------------------------------------------------------------------*/
/* LCD format information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_OUTPUT_MODE		(0x0E)		// LCD output mode (0x00:RGB888 24bit, 0x04:RGB888 8bit 1, 0x05:RGB888 8bit 2, 0x06:RGB888 8bit 3, 0x0D:RGB565 16bit, 0x0E:RGB666 18bit, 0x40:YCbCr422 8bit, 0x41:YCbCr422 16bit)
#define D_LCD_CONFIG_OUTPUT_ORDER_1		(0)			// LCD data output order 1 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_2		(1)			// LCD data output order 2 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_3		(2)			// LCD data output order 3 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_4		(0)			// LCD data output order 4 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_5		(1)			// LCD data output order 5 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_6		(2)			// LCD data output order 6 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
//#define D_LCD_CONFIG_PIXEL_FORMAT		(0)			// LCD pixel format type (0:Delta, 1:Stripe)
#define D_LCD_CONFIG_PIXEL_FORMAT		(1)			// LCD pixel format type (0:Delta, 1:Stripe)

#endif // D_LCD_DRV_ST7796S_RGB_DELTA


////////////////////////////////////////////////////////////////////////
//////////////////// AUO A027DN01 //////////////////////////////////////
////////////////////////////////////////////////////////////////////////
#ifdef D_LCD_DRV_AUO27_RGB_DELTA
/*----------------------------------------------------------------------*/
/* LCD Clock information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_CLOCK				(27000)		// LCD clock[KHz]

/*----------------------------------------------------------------------*/
/* LCD polarity information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_POLARITY_CLOCK		(0)			// Output clock edge (0:Data changes near the falling edge, 1:Data changes near the rising edge)
#define D_LCD_CONFIG_POLARITY_VSYNC		(1)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_HSYNV		(1)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_VENABLE	(0)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_HENABLE	(0)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)

/*----------------------------------------------------------------------*/
/* LCD timing information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_SCAN_MODE			(1)			// LCD scan mode (0:Progressive, 1:Interlaced)
#define D_LCD_CONFIG_VCYC				(262)		// Vertical cycle[Line]
#define D_LCD_CONFIG_VCYC_SHIFT			(1)			// Vsync output 0.5H shift enable(interlace only)
#define D_LCD_CONFIG_HCYC				(1716)		// Horizontal cycle[Clock]
#define D_LCD_CONFIG_VPW				(1)			// Vertical synchronous signal pulse width[Line]
#define D_LCD_CONFIG_HPW				(1)			// Horizontal synchronous signal pulse width[Clock]
#define D_LCD_CONFIG_VBLK				(21)		// Vertical blank period[Line]
#define D_LCD_CONFIG_HBLK				(70)		// Horizontal blank period[Clock]
#define D_LCD_CONFIG_VSIZE				(480)		// Output data vertical size[Line]
#define D_LCD_CONFIG_HSIZE				(960)		// Output data horizontal size[Clock]

/*----------------------------------------------------------------------*/
/* LCD format information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_OUTPUT_MODE		(0x04)		// LCD output mode (0x00:RGB888 24bit, 0x04:RGB888 8bit 1, 0x05:RGB888 8bit 2, 0x06:RGB888 8bit 3, 0x40:YCbCr422 8bit, 0x41:YCbCr422 16bit)
#define D_LCD_CONFIG_OUTPUT_ORDER_1		(0)			// LCD data output order 1 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_2		(2)			// LCD data output order 2 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_3		(1)			// LCD data output order 3 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_4		(2)			// LCD data output order 4 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_5		(1)			// LCD data output order 5 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_6		(0)			// LCD data output order 6 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_PIXEL_FORMAT		(0)			// LCD pixel format type (0:Delta, 1:Stripe)

#endif	// D_LCD_DRV_AUO27_RGB_DELTA


////////////////////////////////////////////////////////////////////////
//////////////////// CASIO_COM27T2M31U /////////////////////////////////
////////////////////////////////////////////////////////////////////////
#ifdef D_LCD_DRV_CASIO_RGB_DELTA
/*----------------------------------------------------------------------*/
/* LCD Clock information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_CLOCK				(27000)		// LCD clock[KHz]

/*----------------------------------------------------------------------*/
/* LCD polarity information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_POLARITY_CLOCK		(0)			// Output clock edge (0:Data changes near the falling edge, 1:Data changes near the rising edge)
#define D_LCD_CONFIG_POLARITY_VSYNC		(0)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_HSYNV		(0)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_VENABLE	(0)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_HENABLE	(0)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)

/*----------------------------------------------------------------------*/
/* LCD timing information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_SCAN_MODE			(0)			// LCD scan mode (0:Progressive, 1:Interlaced)
#define D_LCD_CONFIG_VCYC				(482)		// Vertical cycle[Line]
#define D_LCD_CONFIG_VCYC_SHIFT			(0)			// Vsync output 0.5H shift enable(interlace only)
#define D_LCD_CONFIG_HCYC				(1024)		// Horizontal cycle[Clock]
#define D_LCD_CONFIG_VPW				(1)			// Vertical synchronous signal pulse width[Line]
#define D_LCD_CONFIG_HPW				(2)			// Horizontal synchronous signal pulse width[Clock]
#define D_LCD_CONFIG_VBLK				(1)			// Vertical blank period[Line]
#define D_LCD_CONFIG_HBLK				(12)		// Horizontal blank period[Clock]
#define D_LCD_CONFIG_VSIZE				(480)		// Output data vertical size[Line]
#define D_LCD_CONFIG_HSIZE				(960)		// Output data horizontal size[Clock]

/*----------------------------------------------------------------------*/
/* LCD format information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_OUTPUT_MODE		(0x04)		// LCD output mode (0x00:RGB888 24bit, 0x04:RGB888 8bit 1, 0x05:RGB888 8bit 2, 0x06:RGB888 8bit 3, 0x40:YCbCr422 8bit, 0x41:YCbCr422 16bit)
#define D_LCD_CONFIG_OUTPUT_ORDER_1		(0)			// LCD data output order 1 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_2		(2)			// LCD data output order 2 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_3		(1)			// LCD data output order 3 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_4		(2)			// LCD data output order 4 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_5		(1)			// LCD data output order 5 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_6		(0)			// LCD data output order 6 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_PIXEL_FORMAT		(0)			// LCD pixel format type (0:Delta, 1:Stripe)

#endif	// D_LCD_DRV_CASIO_RGB_DELTA

#ifdef D_LCD_DRV_LPM030M369F_YUV_STRIPE
////////////////////////////////////////////////////////////////////////
//////////////////// LPM030M369F  //////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------*/
/* LCD Clock information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_CLOCK				(27000)		// LCD clock[KHz]  FrameWidth x FrameHeight x 59.94 ( = 60/1.001)

/*----------------------------------------------------------------------*/
/* LCD polarity information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_POLARITY_CLOCK		(0)			// Output clock edge (0:Data changes near the falling edge, 1:Data changes near the rising edge)
#define D_LCD_CONFIG_POLARITY_VSYNC		(1)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_HSYNV		(1)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_VENABLE	(0)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)
#define D_LCD_CONFIG_POLARITY_HENABLE	(0)			// Vertical synchronous signal polarity (0:Positive, 1:Negative)

/*----------------------------------------------------------------------*/
/* LCD timing information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_SCAN_MODE			(0)			// LCD scan mode (0:Progressive, 1:Interlaced)
#define D_LCD_CONFIG_VCYC				(495)		// Vertical cycle[Line]
#define D_LCD_CONFIG_VCYC_SHIFT			(0)			// Vsync output 0.5H shift enable(interlace only)
#define D_LCD_CONFIG_HCYC				(910)		// Horizontal cycle[Clock]
#define D_LCD_CONFIG_VPW				(1)			// Vertical synchronous signal pulse width[Line]
#define D_LCD_CONFIG_HPW				(3)		// Horizontal synchronous signal pulse width[Clock]
#define D_LCD_CONFIG_VBLK				(8)			// Vertical blank period[Line]
#define D_LCD_CONFIG_HBLK				(30)		// Horizontal blank period[Clock]
#define D_LCD_CONFIG_VSIZE				(480)		// Output data vertical size[Line]
#define D_LCD_CONFIG_HSIZE				(720)		// Output data horizontal size[Clock]

/*----------------------------------------------------------------------*/
/* LCD format information												*/
/*----------------------------------------------------------------------*/
#define D_LCD_CONFIG_OUTPUT_MODE		(0x41)		// LCD output mode (0x00:RGB888 24bit, 0x04:RGB888 8bit 1, 0x05:RGB888 8bit 2, 0x06:RGB888 8bit 3, 0x0D:RGB565 16bit, 0x0E:RGB666 18bit, 0x40:YCbCr422 8bit, 0x41:YCbCr422 16bit)
#define D_LCD_CONFIG_OUTPUT_ORDER_1		(1)			// LCD data output order 1 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_2		(0)			// LCD data output order 2 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_3		(2)			// LCD data output order 3 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_4		(3)			// LCD data output order 4 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_5		(0)			// LCD data output order 5 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_OUTPUT_ORDER_6		(0)			// LCD data output order 6 (0:R or Y0, 1:G or CB, 2:B or CR, 3:0 or Y1)
#define D_LCD_CONFIG_PIXEL_FORMAT		(1)			// LCD pixel format type (0:Delta, 1:Stripe)

#endif // D_LCD_DRV_LPM030M369F_YUV_STRIPE

#endif	// _LCD_CONFIG_H
