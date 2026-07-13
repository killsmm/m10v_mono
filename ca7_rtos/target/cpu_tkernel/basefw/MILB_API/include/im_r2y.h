/**
 * @file		im_r2y.h
 * @brief		Bayer to YUV comversion
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyyright 2015 Socionext Inc.</I></B>
 */

/**
@weakgroup im_r2y
@{
	@defgroup im_r2y_iq IQ
*/
/** @} */

/**
@weakgroup im_r2y
@{
	- @ref im_r2y_sequence
	  - @ref R2Y_Init
	  - @ref R2Y_Set_Photo_Sample
	  - @ref R2Y_Set_Video_Sample
	  - @ref R2Y_Ctrl
	  - @ref R2Y_Resize_Rect_Ctrl
	  - @ref R2Y_Int_Handler

	- @ref im_r2y_sample
	  - @ref im_r2y_sample_section1
	  - @ref im_r2y_sample_section2
	  - @ref im_r2y_sample_section3

	<hr>
	@section im_r2y_sequence Sequence

	@section R2Y_Init						Sequence of R2Y initialization
	@image html im_r2y_init.png

	@section R2Y_Set_Photo_Sample			Sequence of R2Y set photo sample sequence
	@image html im_r2y_set_photo_sample.png

	@section R2Y_Set_Video_Sample			Sequence of R2Y set video sample sequence
	@image html im_r2y_set_video_sample.png

	@section R2Y_Ctrl						Sequence of R2Y control (each mode common control)
	@image html im_r2y_ctrl.png

	@section R2Y_Resize_Rect_Ctrl			Sequence of R2Y resize rectangle Control
	@image html im_r2y_resize_rect_ctrl.png

	@section R2Y_Int_Handler				Sequence of R2Y interrupt handler (VOID type)
	@image html im_r2y_int_handler.png
@}
*/

/**
@weakgroup im_r2y_iq
@{
	- @ref im_r2y_iq_sequence
	  - @ref R2Y_Set_Edge_Tbl
	  - @ref R2Y_Set_Gamma_Tbl

	<hr>
	@section im_r2y_iq_sequence Sequence

	@section R2Y_Set_Edge_Tbl			Sequence of R2Y set edge enhancement table
	@image html im_r2y_set_edge_tbl.png

	@section R2Y_Set_Gamma_Tbl			Sequence of R2Y set gamma table
	@image html im_r2y_set_gamma_tbl.png

	- @ref im_r2y_iq_sample
	  - @ref im_r2y_iq_sample_section1
	  - @ref im_r2y_iq_sample_section2
	  - @ref im_r2y_iq_sample_section3
	  - @ref im_r2y_iq_sample_section4

@}
*/

#ifndef _IM_R2Y_H_
#define _IM_R2Y_H_


#include "driver_common.h"
#include "ddim_user_custom.h"
#include "jdsr2y.h"

#include "ddim_typedef.h"

#if 1
#ifdef CO_DEBUG_PRINT_XX_YYY
#ifndef IM_R2Y_STATUS_PRINT
#define CO_DEBUG_PRINT_IM_R2Y	/**< DEBUG */
#endif
#endif
#endif
#ifdef CO_DEBUG_PRINT_IM_R2Y
#ifndef IM_R2Y_STATUS_PRINT
#define IM_R2Y_STATUS_PRINT		/**< DEBUG */
#endif
#endif


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

/** @weakgroup im_r2y
@{
*/

/* Return value of processing result */
//	D_DDIM_OK is defined with driver_common.h
#define	D_IM_R2Y_PARAM_ERROR					(D_IM_R2Y | D_DDIM_INPUT_PARAM_ERROR)	/**< parameter error		*/
#define	D_IM_R2Y_TIMEOUT						(D_IM_R2Y | D_DDIM_TIMEOUT)				/**< timeout				*/
#define	D_IM_R2Y_SYSTEM_ERROR					(D_IM_R2Y | D_DDIM_SYSTEM_ERROR)		/**< System call error		*/
#define	D_IM_R2Y_MACRO_BUSY						(D_IM_R2Y | D_DDIM_MACRO_BUSY_NG)		/**< Macro busy error		*/

/** Function Enable
*/
#define	D_IM_R2Y_ENABLE_OFF						(0)						/**< Disable						*/
#define	D_IM_R2Y_ENABLE_ON						(1)						/**< Enable							*/

/** Wait Enable
*/
#define	D_IM_R2Y_WAIT_OFF						(0)						/**< Wait disable.					*/
#define	D_IM_R2Y_WAIT_ON						(1)						/**< Wait enable.					*/

/** Pipe No
*/
#define	D_IM_R2Y_PIPE1							(0)						/**< Pipe1.							*/
#define	D_IM_R2Y_PIPE2							(1)						/**< Pipe2.							*/
#define	D_IM_R2Y_PIPE12							(2)						/**< Pipe1&2						*/
#define	D_IM_R2Y_PIPE_MAX						(3)						/**< Max value						*/

/** Interrupt status(bit field)
*/
#define	D_IM_R2Y1_INT_STATE_YYR_END				(0x00000001)			/**< YYR: Normal end				*/
#define	D_IM_R2Y1_INT_STATE_YYWALL_END			(0x00000002)			/**< YYW0,0a,1 and 2: Normal end	*/
#define	D_IM_R2Y1_INT_STATE_YYW0_END			(0x00000004)			/**< YYW0: Normal end				*/
#define	D_IM_R2Y1_INT_STATE_YYW0A_END			(0x00000008)			/**< YYW0a: Normal end				*/
#define	D_IM_R2Y1_INT_STATE_YYW1_END			(0x00000010)			/**< YYW1: Normal end				*/
#define	D_IM_R2Y1_INT_STATE_YYW2_END			(0x00000020)			/**< YYW2: Normal end				*/
#define	D_IM_R2Y1_INT_STATE_LINE0_END			(0x00000040)			/**< YYW Line transferd 0			*/
#define	D_IM_R2Y1_INT_STATE_LINE1_END			(0x00000080)			/**< YYW Line transferd 1			*/
#define	D_IM_R2Y1_INT_STATE_LINE2_END			(0x00000100)			/**< YYW Line transferd 2			*/
#define	D_IM_R2Y1_INT_STATE_YYR_ERR				(0x00000200)			/**< YYR: Transfer error			*/
#define	D_IM_R2Y1_INT_STATE_YYW_ERR				(0x00000400)			/**< YYW0,0a,1 or 2: Transfer error	*/
#define	D_IM_R2Y1_INT_STATE_AXR_ERR				(0x00000800)			/**< AXI Read channel error			*/
#define	D_IM_R2Y1_INT_STATE_AXW_ERR				(0x00001000)			/**< AXI Write channel error		*/
#define	D_IM_R2Y1_INT_STATE_YCFERR_ERR			(0x00002000)			/**< F_R2Y YCFERR					*/
#define	D_IM_R2Y1_INT_STATE_TCT_END				(0x00004000)			/**< F_R2Y TCT Normal end			*/
#define	D_IM_R2Y1_INT_STATE_TCHS_END			(0x00008000)			/**< F_R2Y TCHS Normal end			*/
#define	D_IM_R2Y2_INT_STATE_YYR_END				(0x00010000)			/**< YYR: Normal end				*/
#define	D_IM_R2Y2_INT_STATE_YYWALL_END			(0x00020000)			/**< YYW0,0a,1 and 2: Normal end	*/
#define	D_IM_R2Y2_INT_STATE_YYW0_END			(0x00040000)			/**< YYW0: Normal end				*/
#define	D_IM_R2Y2_INT_STATE_YYW0A_END			(0x00080000)			/**< YYW0a: Normal end				*/
#define	D_IM_R2Y2_INT_STATE_YYW1_END			(0x00100000)			/**< YYW1: Normal end				*/
#define	D_IM_R2Y2_INT_STATE_YYW2_END			(0x00200000)			/**< YYW2: Normal end				*/
#define	D_IM_R2Y2_INT_STATE_LINE0_END			(0x00400000)			/**< YYW Line transferd 0			*/
#define	D_IM_R2Y2_INT_STATE_LINE1_END			(0x00800000)			/**< YYW Line transferd 1			*/
#define	D_IM_R2Y2_INT_STATE_LINE2_END			(0x01000000)			/**< YYW Line transferd 2			*/
#define	D_IM_R2Y2_INT_STATE_YYR_ERR				(0x02000000)			/**< YYR: Transfer error			*/
#define	D_IM_R2Y2_INT_STATE_YYW_ERR				(0x04000000)			/**< YYW0,0a,1 or 2: Transfer error	*/
#define	D_IM_R2Y2_INT_STATE_AXR_ERR				(0x08000000)			/**< AXI Read channel error			*/
#define	D_IM_R2Y2_INT_STATE_AXW_ERR				(0x10000000)			/**< AXI Write channel error		*/
#define	D_IM_R2Y2_INT_STATE_YCFERR_ERR			(0x20000000)			/**< F_R2Y YCFERR					*/
#define	D_IM_R2Y2_INT_STATE_TCT_END				(0x40000000)			/**< F_R2Y TCT Normal end			*/
#define	D_IM_R2Y2_INT_STATE_TCHS_END			(0x80000000)			/**< F_R2Y TCHS Normal end			*/

/** Interrupt flag for Im_R2Y_WaitEnd() (bit field)
*/
#define	D_IM_R2Y1_INT_FLG_YYR_END				(0x00000001)			/**< YYR: Normal end				*/
#define	D_IM_R2Y1_INT_FLG_YYWALL_END			(0x00000002)			/**< YYW0,0a,1 and 2: Normal end	*/
#define	D_IM_R2Y1_INT_FLG_YYW0_END				(0x00000004)			/**< YYW0: Normal end				*/
#define	D_IM_R2Y1_INT_FLG_YYW0A_END				(0x00000008)			/**< YYW0a: Normal end				*/
#define	D_IM_R2Y1_INT_FLG_YYW1_END				(0x00000010)			/**< YYW1: Normal end				*/
#define	D_IM_R2Y1_INT_FLG_YYW2_END				(0x00000020)			/**< YYW2: Normal end				*/
#define	D_IM_R2Y1_INT_FLG_LINE0_END				(0x00000040)			/**< YYW Line transferd 0			*/
#define	D_IM_R2Y1_INT_FLG_LINE1_END				(0x00000080)			/**< YYW Line transferd 1			*/
#define	D_IM_R2Y1_INT_FLG_LINE2_END				(0x00000100)			/**< YYW Line transferd 2			*/
#define	D_IM_R2Y1_INT_FLG_YYR_ERR				(0x00000200)			/**< YYR: Transfer error			*/
#define	D_IM_R2Y1_INT_FLG_YYW_ERR				(0x00000400)			/**< YYW0,0a,1 or 2: Transfer error	*/
#define	D_IM_R2Y1_INT_FLG_AXR_ERR				(0x00000800)			/**< AXI Read channel error			*/
#define	D_IM_R2Y1_INT_FLG_AXW_ERR				(0x00001000)			/**< AXI Write channel error		*/
#define	D_IM_R2Y1_INT_FLG_YCFERR_ERR			(0x00002000)			/**< F_R2Y YCFERR					*/
#define	D_IM_R2Y1_INT_FLG_TCT_END				(0x00004000)			/**< F_R2Y TCT Normal end			*/
#define	D_IM_R2Y1_INT_FLG_TCHS_END				(0x00008000)			/**< F_R2Y TCHS Normal end			*/
#define	D_IM_R2Y1_INT_FLG_ALL					(0x0000FFFF)			/**< All flags(PIPE1)				*/
#define	D_IM_R2Y2_INT_FLG_YYR_END				(0x00010000)			/**< YYR: Normal end				*/
#define	D_IM_R2Y2_INT_FLG_YYWALL_END			(0x00020000)			/**< YYW0,0a,1 and 2: Normal end	*/
#define	D_IM_R2Y2_INT_FLG_YYW0_END				(0x00040000)			/**< YYW0: Normal end				*/
#define	D_IM_R2Y2_INT_FLG_YYW0A_END				(0x00080000)			/**< YYW0a: Normal end				*/
#define	D_IM_R2Y2_INT_FLG_YYW1_END				(0x00100000)			/**< YYW1: Normal end				*/
#define	D_IM_R2Y2_INT_FLG_YYW2_END				(0x00200000)			/**< YYW2: Normal end				*/
#define	D_IM_R2Y2_INT_FLG_LINE0_END				(0x00400000)			/**< YYW Line transferd 0			*/
#define	D_IM_R2Y2_INT_FLG_LINE1_END				(0x00800000)			/**< YYW Line transferd 1			*/
#define	D_IM_R2Y2_INT_FLG_LINE2_END				(0x01000000)			/**< YYW Line transferd 2			*/
#define	D_IM_R2Y2_INT_FLG_YYR_ERR				(0x02000000)			/**< YYR: Transfer error			*/
#define	D_IM_R2Y2_INT_FLG_YYW_ERR				(0x04000000)			/**< YYW0,0a,1 or 2: Transfer error	*/
#define	D_IM_R2Y2_INT_FLG_AXR_ERR				(0x08000000)			/**< AXI Read channel error			*/
#define	D_IM_R2Y2_INT_FLG_AXW_ERR				(0x10000000)			/**< AXI Write channel error		*/
#define	D_IM_R2Y2_INT_FLG_YCFERR_ERR			(0x20000000)			/**< F_R2Y YCFERR					*/
#define	D_IM_R2Y2_INT_FLG_TCT_END				(0x40000000)			/**< F_R2Y TCT Normal end			*/
#define	D_IM_R2Y2_INT_FLG_TCHS_END				(0x80000000)			/**< F_R2Y TCHS Normal end			*/
#define	D_IM_R2Y2_INT_FLG_ALL					(0xFFFF0000)			/**< All flags(PIPE2)				*/
#define	D_IM_R2Y_INT_FLG_ALL					(0xFFFFFFFF)			/**< All flags						*/

/** Reading data type
*/
#define	D_IM_R2Y_READ_DTYP_PACK8				(0)						/**< 8bit							*/
#define	D_IM_R2Y_READ_DTYP_PACK12				(2)						/**< 12bit PACK						*/
#define	D_IM_R2Y_READ_DTYP_PACK16				(3)						/**< 16bit							*/
																		/* @YYRCTL.YYRDTYP	*/

/** Writing data type
*/
#define	D_IM_R2Y_WRITE_DTYP_PACK8				(0)						/**< 8bit							*/
#define	D_IM_R2Y_WRITE_DTYP_PACK10				(1)						/**< 10bit PACK						*/
#define	D_IM_R2Y_WRITE_DTYP_PACK12				(2)						/**< 12bit PACK						*/
#define	D_IM_R2Y_WRITE_DTYP_PACK16				(3)						/**< 16bit							*/
																		/* @YYW1MD.DTY1SL	*/

/** RGB data left shift
*/
#define	D_IM_R2Y_STL_RGB_LEFT_SHIFT_NONE		(0x0)					/**< Non shift.						*/
#define	D_IM_R2Y_STL_RGB_LEFT_SHIFT_1BIT		(0x1)					/**< Left shift 1bit.				*/
#define	D_IM_R2Y_STL_RGB_LEFT_SHIFT_2BIT		(0x2)					/**< Left shift 2bits.				*/
#define	D_IM_R2Y_STL_RGB_LEFT_SHIFT_4BIT		(0x3)					/**< Left shift 4bits.				*/
																		/* @YYRCTL.RGBLSFT	*/

/** Resize mode
*/
#define	D_IM_R2Y_RSZ_BILINEAR					(0)						/**< Bilinear interpolation method	*/
#define	D_IM_R2Y_RSZ_BICUBIC					(1)						/**< Bi-cubic interpolation method	*/
																		/* @YYW0MD.RSZ0MD, YYW1MD.RSZ1MD	*/

/** Burst length
*/
#define	D_IM_R2Y_BRST_512						(0)						/**< 512bytes burst					*/
#define	D_IM_R2Y_BRST_256						(1)						/**< 256bytes burst					*/
#define	D_IM_R2Y_BRST_128						(2)						/**< 128bytes burst					*/
																		/* @YYRAXMD.YYRBL, YYW0AXMD.YYW0BL, YYWAAXMD.YYWABL, YYW1AXMD.YYW1BL, YYW2AXMD.YYW2BL	*/

/** YCC CbCr sampling select
*/
#define	D_IM_R2Y_THIN_OUT_YCC444				(0x0)					/**< YCC444 output mode					*/
#define	D_IM_R2Y_THIN_OUT_YCC422_ODD			(0x8)					/**< YCC422 odd sampling output mode	*/
#define	D_IM_R2Y_THIN_OUT_YCC422_CENTER			(0xB)					/**< YCC422 center sampling output mode	*/
#define	D_IM_R2Y_THIN_OUT_YCC420_ODD			(0xC)					/**< YCC420 odd sampling output mode	*/
#define	D_IM_R2Y_THIN_OUT_YCC420_CENTER_LEFT	(0xD)					/**< YCC420 left columned center sampling output mode	*/
#define	D_IM_R2Y_THIN_OUT_YCC420_CENTER			(0xF)					/**< YCC420 center sampling output mode	*/
																		/* @YYW0MD.CSS0SL, YYW1MD.CSS1SL		*/

/** Planar/Dotter output format select
 */
#define	D_IM_R2Y_PORT_PLANAR					(0)						/**< Planar format						*/
#define	D_IM_R2Y_PORT_INTERLEAVED				(1)						/**< Color Interleaved format			*/
																		/* @YYW0MD.SPL0EN, YYW1MD.SPL1EN	*/

/** Output format(Video)
*/
#define	D_IM_R2Y_VFM_NORMAL						(0)						/**< Outout in normal format	*/
#define	D_IM_R2Y_VFM_VIDEO						(1)						/**< Outout in video format	*/
																		/* @YYW0MD.CH0VFM, YYW0MD.CHAVFM	*/

/** Output mode(ch0a)
*/
#define	D_IM_R2Y_YYW0A_OUTPUT_MODE_STOP			(0)						/**< YYW/ch0a stops	*/
#define	D_IM_R2Y_YYW0A_OUTPUT_MODE_DIV_4_Y		(1)						/**< After 1/4 reduction, only Y data is output	*/
#define	D_IM_R2Y_YYW0A_OUTPUT_MODE_DIV_8_Y		(2)						/**< After 1/8 reduction, only Y data is output	*/
#define	D_IM_R2Y_YYW0A_OUTPUT_MODE_DIV_32_SP	(3)						/**< After 1/32 reduction, YCbCr data is output by semi-planar	*/
																		/* @YYW0MD.CH0AMD	*/

/** Output format(ch1)
*/
#define	D_IM_R2Y_DATA_FORMAT_8					(0)						/**< 8bit data format	*/
#define	D_IM_R2Y_DATA_FORMAT_10					(1)						/**< 10bit data format	*/
																		/* @YYW1MD.DFM1SL	*/

/** MCC select
*/
#define	D_IM_R2Y_MCC_AFTER_CC0					(0)						/**< MCC is performed after CC0	*/
#define	D_IM_R2Y_MCC_AFTER_GAMMA				(1)						/**< MCC is performed after gamma	*/
																		/* @R2YMODE.MCCSL	*/

/** YYW Master I/F select
*/
#define	D_IM_R2Y_MASTER_IF0_PORT0				(0x00)					/**< YYW Ch0(Port0) Master I/F 0 select			*/
#define	D_IM_R2Y_MASTER_IF0_PORT1				(0x00)					/**< YYW Ch1(Port1) Master I/F 0 select			*/
#define	D_IM_R2Y_MASTER_IF0_PORT2				(0x00)					/**< YYW Ch2(Port2) Master I/F 0 select			*/
#define	D_IM_R2Y_MASTER_IF1_PORT0				(0x01)					/**< YYW Ch0(Port0) Master I/F 1 select			*/
#define	D_IM_R2Y_MASTER_IF1_PORT1				(0x04)					/**< YYW Ch1(Port1) Master I/F 1 select			*/
#define	D_IM_R2Y_MASTER_IF1_PORT2				(0x10)					/**< YYW Ch2(Port2) Master I/F 1 select			*/
#define	D_IM_R2Y_MASTER_IF2_PORT0				(0x02)					/**< YYW Ch0(Port0) Master I/F 2 select			*/
#define	D_IM_R2Y_MASTER_IF2_PORT1				(0x08)					/**< YYW Ch1(Port1) Master I/F 2 select			*/
#define	D_IM_R2Y_MASTER_IF2_PORT2				(0x20)					/**< YYW Ch2(Port2) Master I/F 2 select			*/
#define	D_IM_R2Y_MASTER_IF3_PORT0				(0x03)					/**< YYW Ch0(Port0) Master I/F 3 select			*/
#define	D_IM_R2Y_MASTER_IF3_PORT1				(0x0C)					/**< YYW Ch1(Port1) Master I/F 3 select			*/
#define	D_IM_R2Y_MASTER_IF3_PORT2				(0x30)					/**< YYW Ch2(Port2) Master I/F 3 select			*/
																		/* @YYW0MSL, YYWAMSL(PORT0&1), YYW1MSL, YYW2MSL(PORT0 only) */

/** Maximum of Table size
*/
// F_R2Y
#define D_IM_R2Y_TABLE_MAX_TONE					(1024)					/**< Tone Control table array number					*/
#define D_IM_R2Y_TABLE_MAX_TCT					(1200)					/**< Luminance evaluation value table array number		*/
#define D_IM_R2Y_TABLE_MAX_BTC_HISTOGRAM_Y		(4096)					/**< Histogram data(Y) table array number(before TC)	*/
#define D_IM_R2Y_TABLE_MAX_BTC_HISTOGRAM_RGB	(256)					/**< Histogram data(RGB) table array number(before TC)	*/
#define D_IM_R2Y_TABLE_MAX_GAMMA				(256)					/**< Gamma table array number 							*/
#define D_IM_R2Y_TABLE_MAX_EDGE_SCALE_HI		(512)					/**< High frequency Edge Scale table array number		*/
#define D_IM_R2Y_TABLE_MAX_EDGE_TC_HI			(256)					/**< High frequency Edge Tc table array number			*/
#define D_IM_R2Y_TABLE_MAX_EDGE_SCALE_MEDIUM	(512)					/**< Medium frequency Edge Scale table array number		*/
#define D_IM_R2Y_TABLE_MAX_EDGE_TC_MEDIUM		(256)					/**< Medium frequency Edge Tc table array number		*/
#define D_IM_R2Y_TABLE_MAX_EDGE_SCALE_LO		(512)					/**< Low frequency edge Scale table array number		*/
#define D_IM_R2Y_TABLE_MAX_EDGE_TC_LO			(256)					/**< Low frequency edge Tc table array number			*/
#define D_IM_R2Y_TABLE_MAX_MAPSCL				(221)					/**< Map Scale table array number (13 x 17)				*/

// JDSR2Y
#define D_IM_R2Y_TABLE_MAX_HISTOGRAM			(128)					/**< Histogram data table array number				*/
#define D_IM_R2Y_TABLE_MAX_RGB_DEKNEE			(256)					/**< JDSR2Y RGB Deknee table array number			*/

/** YCbCr/RGB component to be processed
*/
#define D_IM_R2Y_PORT_0							(0)						/**< Port0						*/
#define D_IM_R2Y_PORT_1							(1)						/**< Port1						*/
#define D_IM_R2Y_PORT_2							(2)						/**< Port2						*/
#define D_IM_R2Y_PORT_MAX						(3)						/**< The maximum values of YCbCr/RGB component to be processed */
// YCbCr(planar)
#define	D_IM_R2Y_PORT_Y							(D_IM_R2Y_PORT_0)		/**< Y data						*/
#define	D_IM_R2Y_PORT_CB						(D_IM_R2Y_PORT_1)		/**< Cb data (planar)			*/
#define	D_IM_R2Y_PORT_CR						(D_IM_R2Y_PORT_2)		/**< Cr data (planar)			*/
// YCbCr(semi-planar)
#define	D_IM_R2Y_PORT_CBCR						(D_IM_R2Y_PORT_1)		/**< CbCr data (semi-planar)	*/
// RGB
#define	D_IM_R2Y_PORT_R							(D_IM_R2Y_PORT_0)		/**< R data						*/
#define	D_IM_R2Y_PORT_G							(D_IM_R2Y_PORT_1)		/**< G data						*/
#define	D_IM_R2Y_PORT_B							(D_IM_R2Y_PORT_2)		/**< B data						*/
#define	D_IM_R2Y_PORT_GRAY						(D_IM_R2Y_PORT_0)		/**< Gray data					*/

/** YYW channel number
*/
#define D_IM_R2Y_YYW_CH_0						(0)						/**< YYW channel index number 0 */
#define D_IM_R2Y_YYW_CH_1						(1)						/**< YYW channel index number 1 */
#define D_IM_R2Y_YYW_CH_2						(2)						/**< YYW channel index number 2 */
#define D_IM_R2Y_YYW_CH_0A						(3)						/**< YYW channel index number 3 */
#define D_IM_R2Y_YYW_CH_MAX						(4)						/**< Maximum channel index number of YYW */

/** Line interrupt channel number
 */
#define D_IM_R2Y_LINE_INT_0						(0)						/**< Line interrupt index number 0 */
#define D_IM_R2Y_LINE_INT_1						(1)						/**< Line interrupt index number 1 */
#define D_IM_R2Y_LINE_INT_2						(2)						/**< Line interrupt index number 2 */
#define D_IM_R2Y_LINE_INT_MAX					(3)						/**< Maximum channel index number of Line interrupt */

/** YYW output bank number
*/
#define D_IM_R2Y_YYW_BANK_0						(0)						/**< YYW output bank index number 0 */
#define D_IM_R2Y_YYW_BANK_1						(1)						/**< YYW output bank index number 1 */
#define D_IM_R2Y_YYW_BANK_2						(2)						/**< YYW output bank index number 2 */
#define D_IM_R2Y_YYW_BANK_3						(3)						/**< YYW output bank index number 3 */
#define D_IM_R2Y_YYW_BANK_MAX					(4)						/**< Maximum output bank index number of YYW */

/** Histogram sampling pitch
 */
#define	D_IM_R2Y_HIST_PIT_2X2					(0)						/**< 1pixel in 2x2 area			*/
#define	D_IM_R2Y_HIST_PIT_4X4					(1)						/**< 1pixel in 4x4 area			*/
#define	D_IM_R2Y_HIST_PIT_8X8					(2)						/**< 1pixel in 8x8 area			*/
#define	D_IM_R2Y_HIST_PIT_16X16					(3)						/**< 1pixel in 16x16 area		*/
																		/* @HSTMD.PIT_M */

/** Transferred subject to the interrupt line number
*/
#define	D_IM_R2Y_LINT_YYW_CH0					(0)						/**< Monitoring of the number of lines transferred the YYW/ch0	*/
#define	D_IM_R2Y_LINT_YYW_CH1					(1)						/**< Monitoring of the number of lines transferred the YYW/ch1	*/
																		/* @LINTSEL.LINTS */

/** Transferred subject to the interrupt line number
*/
#define	D_IM_R2Y_RDC_MODE_DIV_2					(0)						/**< Reduction 1/2 -> H:2 x V:2 = Find the average of the 4 pixels	*/
#define	D_IM_R2Y_RDC_MODE_DIV_4					(1)						/**< Reduction 1/4 -> H:4 x V:4 = Find the average of the 16 pixels	*/
#define	D_IM_R2Y_RDC_MODE_DIV_8					(2)						/**< Reduction 1/8 -> H:8 x V:8 = Find the average of the 64 pixels	*/
																		/* @YYW0MD.RDC0MD, YYW1MD.RDC1MD, YYW2MD.RDC2MD */
#define	D_IM_R2Y_RDC_MODE_DIV_DIS				(3)						/**< Reduction disable	*/

/** Gamma correction mode
*/
#define	D_IM_R2Y_GAMMA_MODE_12					(0)						/**< 12bit input difference type gamma correction mode	*/
#define	D_IM_R2Y_GAMMA_MODE_10					(1)						/**< 10bit input gamma correction mode	*/
																		/* @GMCTL.GMMD */

#define D_IM_R2Y_TABLE_MAX_VAL					(32767)					/**< Maximum value of Histogram data */

// Output Address array maximum number
#define D_IM_R2Y_ADDR_INDEX_MAX					(D_IM_R2Y_YYW_BANK_MAX)	/**< Maximum number of R2Y output address information(bank0 ~ 3)	*/

// RSZ0HSTA, RSZ0HPIT, RSZ0VSTA, RSZ0VPIT, RSZ1HSTA, RSZ1HPIT, RSZ1VSTA, RSZ1VPIT, RSZ2HSTA, RSZ2HPIT, RSZ2VSTA, RSZ2VPIT register value mean "1.0"
#define D_IM_R2Y_STA_PIT_1_0					(0x4000)				/**< RSZxxSTA, RSZxxPIT register value mean "1.0" */

/** Histogram horizontal sampling interval
 */
#define	D_IM_R2Y_HIST_HCYC_4					(0)						/**< 4pixel				*/
#define	D_IM_R2Y_HIST_HCYC_8					(1)						/**< 8pixel				*/
#define	D_IM_R2Y_HIST_HCYC_16					(2)						/**< 16pixel			*/
																		/* @TCHSCTL.TCHSHCYC */

/** Histogram vertical sampling interval
 */
#define	D_IM_R2Y_HIST_VCYC_1					(0)						/**< 1lines				*/
#define	D_IM_R2Y_HIST_VCYC_2					(1)						/**< 2lines				*/
#define	D_IM_R2Y_HIST_VCYC_4					(2)						/**< 4lines				*/
#define	D_IM_R2Y_HIST_VCYC_8					(3)						/**< 8lines				*/
#define	D_IM_R2Y_HIST_VCYC_16					(4)						/**< 16lines			*/
																		/* @TCHSCTL.TCHSVCYC */

/** RGB Histogram mode
 */
#define	D_IM_R2Y_HIST_BIT_WIDTH_7_0				(0)						/**< Detection bit range[7:0]	Detection maximum value:0xFF		*/
#define	D_IM_R2Y_HIST_BIT_WIDTH_8_1				(1)						/**< Detection bit range[8:1]	Detection maximum value:0x1FF		*/
#define	D_IM_R2Y_HIST_BIT_WIDTH_9_2				(2)						/**< Detection bit range[9:2]	Detection maximum value:0x3FF		*/
#define	D_IM_R2Y_HIST_BIT_WIDTH_10_3			(3)						/**< Detection bit range[10:3]	Detection maximum value:0x7FF		*/
#define	D_IM_R2Y_HIST_BIT_WIDTH_11_4			(4)						/**< Detection bit range[11:4]	Detection maximum value:0xFFF		*/
																		/* @TCHSCTL.TCHSRGBMD */

/** TC table reference resolution
 */
#define	D_IM_R2Y_TC_RESOLUTION_1024				(0)						/**< 1024 gradations				*/
#define	D_IM_R2Y_TC_RESOLUTION_512				(1)						/**<  512 gradations				*/
#define	D_IM_R2Y_TC_RESOLUTION_256				(2)						/**<  256 gradations				*/
																		/* @TCCTL.TCRES */

/** Edge enhancement edge noise reduction mode
 */
#define	D_IM_R2Y_EGSMT_VERSION1					(0)						/**< Edge noise reduction ver.1		*/
#define	D_IM_R2Y_EGSMT_VERSION2					(1)						/**< Edge noise reduction ver.2		*/
																		/* @EGSMCTL.EGSMT */


/* @} */	// im_r2y group

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

/** RGB table type(for Deknee).
 */
typedef enum {
	E_R2Y_DKN_RGBTBL_R = 0,			/**< R color table. */
	E_R2Y_DKN_RGBTBL_G,				/**< G color table. */
	E_R2Y_DKN_RGBTBL_B,				/**< B color table. */
	E_R2Y_DKN_RGBTBL_MAX,			/**< Maximum value. */
} E_R2Y_DKN_RGBTBL;

/** RGB table type(for Histogram before TC).
 */
typedef enum {
	E_R2Y_BTC_HIST_RGBTBL_R = 0,	/**< R component data table. */
	E_R2Y_BTC_HIST_RGBTBL_G,		/**< G component data table. */
	E_R2Y_BTC_HIST_RGBTBL_B,		/**< B component data table. */
	E_R2Y_BTC_HIST_RGBTBL_Y,		/**< Y component data table. */
	E_R2Y_BTC_HIST_RGBTBL_MAX,		/**< Maximum value. */
} E_R2Y_BTC_HIST_RGBTBL;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
//
/** @weakgroup im_r2y
@{
*/

/** AXI bus I/F control
*/
typedef struct {
	UCHAR					cache_type;								/**< cache type (4bits)				*/
	UCHAR					protect_type;							/**< protect type (3bits)			*/
} T_IM_R2Y_CTRL_AXI_TYPE;

/** AXI bus I/F control
*/
typedef struct {
	T_IM_R2Y_CTRL_AXI_TYPE	yyr[D_IM_R2Y_PORT_MAX];							/**< YYRAXCTL.YYRCACHE_x, YYRAXCTL.YYRPROT_x YYR Read channel types.	*/
	T_IM_R2Y_CTRL_AXI_TYPE	yyw[D_IM_R2Y_YYW_CH_MAX][D_IM_R2Y_PORT_MAX];	/**< YYW0AXCTL.YYW0CACHE[3], YYW0AXCTL.YYW0PROT[3],<br>
																				 YYW1AXCTL.YYW1CACHE[3], YYW1AXCTL.YYW1PROT[3],<br>
																				 YYW2AXCTL.YYW2CACHE, YYW2AXCTL.YYW2PROT,<br>
																				 YYWAAXCTL.YYWACACHE[2], YYWAAXCTL.YYWAPROT[2]<br>
																				 YYW Write channel types.											*/
} T_IM_R2Y_CTRL_AXI;

/** AXI bus read channel status
*/
typedef struct {
	UCHAR					yyr_axi_stat[D_IM_R2Y_PORT_MAX];		/**< YYRAXSTS.YYRRESP_x : Read channel status (2bits)	*/
} T_IM_R2Y_AXI_YYR_STAT;

/** AXI bus write channel status
*/
typedef struct {
	UCHAR					yyw0_axi_stat[D_IM_R2Y_PORT_MAX];		/**< YYW0AXSTS.YY0BRESP[3] : Write channel status (2bits)	*/
	UCHAR					yyw0a_axi_stat[D_IM_R2Y_PORT_MAX-1];	/**< YYWAAXSTS.YYABRESP[2] : Write channel status (2bits)	*/
	UCHAR					yyw1_axi_stat[D_IM_R2Y_PORT_MAX];		/**< YYW1AXSTS.YY1BRESP[3] : Write channel status (2bits)	*/
	UCHAR					yyw2_axi_stat;							/**< YYW2AXSTS.YY2BRESP : Write channel status (2bits)		*/
} T_IM_R2Y_AXI_YYW_STAT;

/** AXI bus write channel status
*/
typedef struct {
	UCHAR					master_if_select;						/**< Master I/F select.<br>
																		YYW0AXMD.YYW0MSL, YYW1AXMD.YYW1MSL : (6bits)<br>
																		Set bit-OR parameter between YYW0/YYW1 Ch0-Ch2.<br>
																		YYWAAXMD.YYWAMSL : (4bits)<br>
																		Set bit-OR parameter between YYW0a Ch0-Ch1.<br>
																		YYW2AXMD.YYW2MSL : (2bits)<br>
																		Set bit-OR parameter between YYW2 Ch0.<br>
																		YYW0/YYW0a/YYW1/YYW2 Ch0(Port0):<br>
																	<ul><li>@ref D_IM_R2Y_MASTER_IF0_PORT0
																		<li>@ref D_IM_R2Y_MASTER_IF1_PORT0
																		<li>@ref D_IM_R2Y_MASTER_IF2_PORT0
																		<li>@ref D_IM_R2Y_MASTER_IF3_PORT0</ul>
																	YYW0/YYW0a/YYW1 Ch1(Port1):<br>
																	<ul><li>@ref D_IM_R2Y_MASTER_IF0_PORT1
																		<li>@ref D_IM_R2Y_MASTER_IF1_PORT1
																		<li>@ref D_IM_R2Y_MASTER_IF2_PORT1
																		<li>@ref D_IM_R2Y_MASTER_IF3_PORT1</ul>
																	YYW0/YYW1 Ch2(Port2):<br>
																	<ul><li>@ref D_IM_R2Y_MASTER_IF0_PORT2
																		<li>@ref D_IM_R2Y_MASTER_IF1_PORT2
																		<li>@ref D_IM_R2Y_MASTER_IF2_PORT2
																		<li>@ref D_IM_R2Y_MASTER_IF3_PORT2</ul>										*/
	UCHAR					burst_length;							/**< YYW0AXMD.YYW0BL, YYWAAXMD.YYWABL,<br>
																		 YYW1AXMD.YYW1BL, YYW2AXMD.YYW2BL : Transfer burst length (2bits)
																	<ul><li>@ref D_IM_R2Y_BRST_512
																		<li>@ref D_IM_R2Y_BRST_256
																		<li>@ref D_IM_R2Y_BRST_128</ul>												*/
	UCHAR					out_enable[D_IM_R2Y_PORT_MAX];			/**< Set of componentsto be output to the external memory.<br>
																		 YYW0AXMD.YYW0DOE, YYW1AXMD.YYW1DOE : Y/Cb/Cr(planar) or Y/CbCr(semi-planar) (3bits)
																		 YYWAAXMD.YYWADOE					: Y/CbCr(semi-planar) (2bits)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not transfer component data to master I/F
																		<li>@ref D_IM_R2Y_ENABLE_ON : Transfers component data to master I/F</ul>	*/
} T_IM_R2Y_AXI_YYW_MODE;

/** Rectangle of image
*/
typedef struct {
	USHORT					img_top;								/**< Vertical start position		*/
	USHORT					img_left;								/**< Horizontal start position		*/
	USHORT					img_width;								/**< Horizontal size				*/
	USHORT					img_lines;								/**< Vertical size					*/
} T_IM_R2Y_RECT;								/* Rectangle control data	*/

/** R2Y Window size
*/
typedef struct {
	USHORT					yyw_width;								/**< image width(YYW0/1/2)																		*/
	USHORT					yyw_lines;								/**< image lines(YYW0/1/2)																		*/
	INT32					output_global_w[D_IM_R2Y_PORT_MAX];		/**< YYW0DEF[2], YYWADEF, YYW1DEF[2], YYW2DEF : Output Horizontal global bytes (17 bits signed)	*/
} T_IM_R2Y_WINDOW_SIZE;

/** R2Y Pitch
*/
typedef struct {
	UINT32					output_x_start_pos;						/**< resize horizontal start pos<br>
																		 RSZ0HSTA, RSZ1HSTA : Bilinear (0 ~ 63 + 16383/16384)<br>
																							  Bi-cubic (1 ~ 63 + 16383/16384)<br>
																		 RSZ2HSTA : 		  (0 ~ 63 + 16383/16384)												*/
	UINT32					output_x_pitch;							/**< RSZ0HPIT, RSZ1HPIT, RSZ2HPIT : resize horizontal pitch (2048/16384 ~ 32768/16384)			*/
	UINT32					output_y_start_pos;						/**< resize vertical start pos
																		 RSZ0VSTA, RSZ1VSTA : Bilinear (0 ~ 63 + 16383/16384)<br>
																							  Bi-cubic (1 ~ 63 + 16383/16384)<br>
																		 RSZ2VSTA : 		  (0 ~ 63 + 16383/16384)												*/
	UINT32					output_y_pitch;							/**< RSZ0VPIT, RSZ1VPIT, RSZ2VPIT : resize vertical pitch (2048/16384 ~ 32768/16384)			*/
	INT32					output_global_w[D_IM_R2Y_PORT_MAX];		/**< YYW0DEF[2], YYW1DEF[2], YYW2DEF, YYWADEF : Output Horizontal global bytes (17 bits signed)	*/
} T_IM_R2Y_PITCH;

/** YYW control
*/
typedef struct {
	T_IM_R2Y_AXI_YYW_MODE	axi_write_mode;							/**< AXI bus write channel control parameters										*/
	USHORT					write_request_mask[D_IM_R2Y_PORT_MAX];	/**< YYW0REQMSK.YYW0REQMSK[3], YYWAREQMSK.YYWAREQMSK[2],<br>
																		 YYW1REQMSK.YYW1REQMSK[3], YYW2REQMSK.YYW2REQMSK :
																		 YYW transfer request mask (12bits)<br>
																		<p>Please set number of cycles, the time until the next access request.<br>
																		   If you set the "000h" you is disabled.										*/
	UCHAR					resize_mode;							/**< YYW0MD.RSZ0MD, YYW1MD.RSZ1MD : Resize mode selection (1bit)
																	<ul><li>@ref D_IM_R2Y_RSZ_BILINEAR
																		<li>@ref D_IM_R2Y_RSZ_BICUBIC</ul>												*/
	UCHAR					pixel_avg_reduction_mode;				/**< YYW0MD.RDC0MD, YYW1MD.RDC1MD, YYW2MD.RDC2MD : Pixel average reduction mode (2bits)
																	<ul><li>@ref D_IM_R2Y_RDC_MODE_DIV_2
																		<li>@ref D_IM_R2Y_RDC_MODE_DIV_4
																		<li>@ref D_IM_R2Y_RDC_MODE_DIV_8</ul>	*/
	UCHAR					pixel_avg_reduction_enable;				/**< YYW0MD.RDC0EN, YYW1MD.RDC1EN, YYW2MD.RDC2EN : Pixel average reduction enable
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Pixel average reduction function is not used
																		<li>@ref D_IM_R2Y_ENABLE_ON : Pixel average reduction function is used</ul>		*/
	UCHAR					ycc_cc_thin_select;						/**< YYW0MD.CSS0SL, YYW1MD.CSS1SL : YCCC444, YCC422 and YCC420 format (4bits)
																	<ul><li>@ref D_IM_R2Y_THIN_OUT_YCC444
																		<li>@ref D_IM_R2Y_THIN_OUT_YCC422_ODD
																		<li>@ref D_IM_R2Y_THIN_OUT_YCC422_CENTER
																		<li>@ref D_IM_R2Y_THIN_OUT_YCC420_ODD
																		<li>@ref D_IM_R2Y_THIN_OUT_YCC420_CENTER_LEFT
																		<li>@ref D_IM_R2Y_THIN_OUT_YCC420_CENTER</ul>									*/
	UCHAR					trim_out_enable;						/**< YYW0MD.TRM0EN, YYW1MD.TRM1EN, YYW2MD.TRM2EN : Trimming output enable
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Do not do the trimming.
																		<li>@ref D_IM_R2Y_ENABLE_ON : Trimming.</ul>									*/
	UCHAR					planar_interleaved_mode;				/**< YYW0MD.SPL0EN, YYW1MD.SPL1EN : Planar or Color Interleaved store mode
																	<ul><li>@ref D_IM_R2Y_PORT_PLANAR
																		<li>@ref D_IM_R2Y_PORT_INTERLEAVED</ul>											*/
} T_IM_R2Y_YYW_CTRL;

/** YYW Line interrupt control
*/
typedef struct {
	USHORT					level;									/**< LINTLV[3] : YYW Line interrupt level. (14bits)
																		<p>We generate the interrupt request flag when the set value of this register,
																		   the number of lines of ch transferred was selected by line_intr_select arrives.
																		   Setting of this interrupt can be three places,
																		   also can be set in each interrupt request.
																		<p>This interrupt request is required that you have chosen is ch operation is enabled,
																		   also is effective SDRAM power of that ch, and the operation is enabled.
																		<p>This interrupt request, can be disabled by setting the "0000" h.							*/
	UCHAR					yyw_select;								/**< LINTSEL.LINTS[3] : Line interrupt select.
																	<ul><li>@ref D_IM_R2Y_YYW_CH_0 : Monitoring of the number of lines of YYW/ch0 transferred.
																		<li>@ref D_IM_R2Y_YYW_CH_1 : Monitoring of the number of lines of YYW/ch1 transferred.</ul>	*/
} T_IM_R2Y_CTRL_LINE_INTR;

/** R2Y control
*/
typedef struct {
	// YYW write channel control
	T_IM_R2Y_YYW_CTRL		yyw[D_IM_R2Y_YYW_CH_MAX];				/**< YYW0/YYW1/YYW2/YYW0a control parameters										*/
	T_IM_R2Y_CTRL_LINE_INTR	line_intr[D_IM_R2Y_LINE_INT_MAX];		/**< YYW Line interrupt control parameters											*/
	UCHAR					yyw_enable[D_IM_R2Y_YYW_CH_MAX - 1];	/**< YYWCTL.YYWMODE[0] : YYW enable(ch0(+ch0a)) (1bit)<br>
																		 YYWCTL.YYWMODE[1] : YYW enable(ch1) (1bit)<br>
																		 YYWCTL.YYWMODE[2] : YYW enable(ch2) (1bit)<br>
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Stop
																		<li>@ref D_IM_R2Y_ENABLE_ON : Transfer is enabled</ul>							*/
	USHORT					line_transfer_cycle;					/**< YYW0LTT : 1 Line transfer clock cycle (14bits)
																		<p>We adjust the amount of time between the beginning of the current line
																		   from the output data, and outputs the data to the beginning of the next line.
																		<p>Set minimum number of cycles per 2-cycles are required YYW to output data for one line.<br>
																		   If the number of cycles for the number of cycles that have been set in this register,
																		   took to the transfer of one line is below, we will interrupt the output line of the
																		   following to stop the transfer operation of YYW only time of the difference.<br>
																		   YYW transfer operation does not stop if the number of cycles needed to transfer one line,
																		   this setting is exceeded, we make the following output line.<br>
																		   If you set the 0d, this function is disabled.								*/
	UCHAR					yyw_continue_start_enable;				/**< YYWCTL.YYWCSE : YYW continue start enable (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Frame processing one-shot.
																			(YYWTRG will need to restart each process one frame.)
																		<li>@ref D_IM_R2Y_ENABLE_ON : Continuous frame processing.
																			(I continue to frame processing writing "00" to YYWTRG.)</ul>				*/
	UCHAR					yyw_horizontal_flip;					/**< YYWCTL.YYWHRV : YYW horizonta flip (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Horizontal flip disable.
																		<li>@ref D_IM_R2Y_ENABLE_ON : Horizontal flip enable.</ul>						*/
	UCHAR					video_format_out_select_0;				/**< YYW0MD.CH0VFM : Video format output selection(ch0).
																	<ul><li>@ref D_IM_R2Y_VFM_NORMAL
																		<li>@ref D_IM_R2Y_VFM_VIDEO</ul>												*/
	UCHAR					ipu_macro_output_enable;				/**< YYW0MD.EXTBEN : External I/F output B enable(IPU macro).
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not output.
																		<li>@ref D_IM_R2Y_ENABLE_ON : Output is enabled.</ul>							*/
	UCHAR					ipu_macro_trimming_enable;				/**< YYW0MD.TRMBEN : External I/F trimming B enable(IPU macro).
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Trimming is disabled.
																		<li>@ref D_IM_R2Y_ENABLE_ON : Trimming is enabled.</ul>							*/
	UCHAR					cnr_macro_output_enable;				/**< YYW0MD.EXTCEN : External I/F output C enable(CNR macro).
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not output.
																		<li>@ref D_IM_R2Y_ENABLE_ON : Output is enabled.</ul>							*/
	UCHAR					cnr_macro_trimming_enable;				/**< YYW0MD.TRMCEN : External I/F trimming C enable(CNR macro).
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Trimming is disabled.
																		<li>@ref D_IM_R2Y_ENABLE_ON : Trimming is enabled.</ul>							*/
	UCHAR					output_mode_0a;							/**< YYW0MD.CH0AMD : Output channel 0a mode.
																	<ul><li>@ref D_IM_R2Y_YYW0A_OUTPUT_MODE_STOP
																		<li>@ref D_IM_R2Y_YYW0A_OUTPUT_MODE_DIV_4_Y
																		<li>@ref D_IM_R2Y_YYW0A_OUTPUT_MODE_DIV_8_Y
																		<li>@ref D_IM_R2Y_YYW0A_OUTPUT_MODE_DIV_32_SP</ul>								*/
	UCHAR					video_format_out_select_0a;				/**< YYW0MD.CHAVFM : Video format output selection(ch0a).
																	<ul><li>@ref D_IM_R2Y_VFM_NORMAL
																		<li>@ref D_IM_R2Y_VFM_VIDEO	 </ul>												*/
	UCHAR					output_format_sel1;						/**< YYW1MD.DFM1SL : YCbCr data format selection(ch1)
																	<ul><li>@ref D_IM_R2Y_DATA_FORMAT_8
																		<li>@ref D_IM_R2Y_DATA_FORMAT_10</ul>											*/
	UCHAR					output_type_sel1;						/**< YYW1MD.DTY1SL : Transfer data type selection(ch1)
																	<ul><li>@ref D_IM_R2Y_WRITE_DTYP_PACK8
																		<li>@ref D_IM_R2Y_WRITE_DTYP_PACK10
																		<li>@ref D_IM_R2Y_WRITE_DTYP_PACK12
																		<li>@ref D_IM_R2Y_WRITE_DTYP_PACK16</ul>										*/

	/* R2Y control */
	UCHAR					ycf_bypass;								/**< R2YMODE.YCFBYP : YCF bypass mode of the F_R2Y (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : YCF block of F_R2Y is set to normal mode.
																		<li>@ref D_IM_R2Y_ENABLE_ON : YCF block of F_R2Y is set to bypass mode.</ul>	*/
	UCHAR					ycf_padding;							/**< R2YMODE.YCFPDD : YCF padding mode of the F_R2Y (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform ring pixel padding.
																		<li>@ref D_IM_R2Y_ENABLE_ON : Perform ring pixel padding.</ul>					*/
	UCHAR					mcc_select;								/**< R2YMODE.MCCSL : Multi-axis color correction processing selection (1bit)
																	<ul><li>@ref D_IM_R2Y_MCC_AFTER_CC0
																		<li>@ref D_IM_R2Y_MCC_AFTER_GAMMA</ul>											*/
	UCHAR					mcc_bit_shift;							/**< R2YMODE.MCC1BM : MCCSL=1 selection bit shift mode (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Saturation compensation is disabled.
																		<li>@ref D_IM_R2Y_ENABLE_ON : Saturation compensation is enabled.</ul>			*/

	VOID					(*r2y_user_handler)(UINT32* int_status, UINT32 user_param);			/**< Interrupt Handler																*/
	UINT32					user_param;								/**< This set value will return to callback argument value when interrupt occurs.	*/
} T_IM_R2Y_CTRL;								/* R2Y control	*/

/** R2Y SDRAM input mode control
*/
typedef struct {
	UCHAR					burst_length;							/**< YYRAXMD.YYRBL : Transfer burst length of the read channel. (2bits)
																	<ul><li>@ref D_IM_R2Y_BRST_512
																		<li>@ref D_IM_R2Y_BRST_256
																		<li>@ref D_IM_R2Y_BRST_128</ul>													*/
	USHORT					read_request_mask[D_IM_R2Y_PORT_MAX];	/**< YYRREQMSK.YYRREQMSK[3] : Mask transfer request(12bits)<br>
																	<p>Mask setting of the transfer request from the read to do YYR.<br>
																	   Please set number of cycles, the time until the next access request.<br>
																	   If you set the 000h you is disabled.												*/
	UCHAR					input_dtype;							/**< YYRCTL.YYRDTYP : Input data type (2bits)
																	<ul><li>@ref D_IM_R2Y_READ_DTYP_PACK8
																		<li>@ref D_IM_R2Y_READ_DTYP_PACK12
																		<li>@ref D_IM_R2Y_READ_DTYP_PACK16</ul>											*/
	UCHAR					mono_ebable;							/**< YYRCTL.MONOEN : Single component processing enable (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Input the RGB data.
																		<li>@ref D_IM_R2Y_ENABLE_ON : Input only single component data.</ul>			*/
	UCHAR					rgb_deknee_enable;						/**< YYRCTL.DKNEN : Deknee enable for RGB data. (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Do not process Deknee
																		<li>@ref D_IM_R2Y_ENABLE_ON : Processing Deknee</ul>							*/
	UCHAR					rgb_left_shift;							/**< YYRCTL.RGBLSFT : Left shift for RGB data. (2bits)
																	<ul><li>@ref D_IM_R2Y_STL_RGB_LEFT_SHIFT_NONE
																		<li>@ref D_IM_R2Y_STL_RGB_LEFT_SHIFT_1BIT
																		<li>@ref D_IM_R2Y_STL_RGB_LEFT_SHIFT_2BIT
																		<li>@ref D_IM_R2Y_STL_RGB_LEFT_SHIFT_4BIT</ul>									*/
	UCHAR					rgb_saturation_mode;					/**< YYRCTL.RGBLSM : Saturation mode when left shift. (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Do not process Saturation
																		<li>@ref D_IM_R2Y_ENABLE_ON : Processing Saturation</ul>						*/
	UCHAR					top_offset[D_IM_R2Y_PORT_MAX];			/**< YYROFS[3] : Setting top bit input data area (3bits)<br>
																		<p>Set the first bit of the data area is read YYR.
																		This setting can be made for each individual component data.
																		Set the position of the LSB bit in bit position at the time of each data format.<br>
																		However, when you select 8-bit / 16-bit in input_dtype(YYRDTYP) is always 000b.
																		Only when you select a 12-bit pack, please select the setting of 000b or 100b.
																		Only when you select a 10-bit pack, please select the setting of 000b or 010b or 100b or 110b.
																		<p>This setting is valid only when the input mode SDRAM.
																		<p>exp)<br>
																		I would like the following figure, data and each are stored in 12-bit pack.<br>
																		Assuming that the first pixel data3 at this time,
																		since the position of the LSB and bit 4 data3 address, I set 100b.
																		@image html im_r2y_yyrofs_exp.png												*/
	USHORT					input_global;							/**< YYRDEF : Input horizontal Gobal area size (16bits, multiple of 16)				*/
} T_IM_R2Y_CTRL_SDRAM_INPUT;						/* R2Y SDRAM input mode control	*/

/** R2Y B2R/LTM direct connection mode control
*/
typedef struct {
	UCHAR					frame_stop;								/**< DINCTL.FRMSTP : R2Y stop when next frame<br>
																	<p>If you want to stop at the frame boundaries of YYR transfer process is set to "1".	*/
} T_IM_R2Y_CTRL_DIRECT;								/* R2Y B2R/LTM direct connection mode control	*/

/** Resize Rectangle control
*/
typedef struct {
	T_IM_R2Y_RECT			input_size;								/**< SDRAM input mode : input image size(and trimming)<br>
																							Please set the start position and size<br>
																		 Direct input mode : input image size<br>
																							 Please set output size of connection origin.<br>
																							 (Please set (0,0) to start position)				*/
	T_IM_R2Y_WINDOW_SIZE	output_size[D_IM_R2Y_YYW_CH_MAX];		/**< output image size(YYW0/1/2/0a)											*/
} T_IM_R2Y_RESIZE_RECT;

/** Resize Pitch control
*/
typedef struct {
	T_IM_R2Y_RECT			input_size;								/**< input image size(and trimming)	*/
	T_IM_R2Y_PITCH			output_pitch[D_IM_R2Y_YYW_CH_MAX];		/**< output image pitch(YYW0/1/2/0a)*/
} T_IM_R2Y_RESIZE_PITCH;

/** Resize control information
*/
typedef struct {
	UINT32					resize_select;							/**< 0:invalidate(initial value or parameter changing)<br>
																		1:validate r2y_resize_rect<br>
																		2:validate r2y_resize_pitch */
	T_IM_R2Y_RESIZE_RECT	r2y_resize_rect;						/**< rectangle information. (refer Im_R2Y_Set_Resize_Rect()) */
	T_IM_R2Y_RESIZE_PITCH	r2y_resize_pitch;						/**< pitch information. (refer Im_R2Y_Set_Resize_Pitch()) */
} T_IM_R2Y_RESIZE_PARAM;

/** YCC address information
*/
typedef struct {
	VOID*					addr_Y;									/**< Y address		*/
	VOID*					addr_Cb;								/**< Cb address		*/
	VOID*					addr_Cr;								/**< Cr address		*/
} T_IM_R2Y_ADDR_YCC_INFO;

/** RGB address information
*/
typedef struct {
	VOID*					addr_R;									/**< R address		*/
	VOID*					addr_G;									/**< G address		*/
	VOID*					addr_B;									/**< B address		*/
} T_IM_R2Y_ADDR_RGB_INFO;

/** YYR input address information
*/
typedef union {
	T_IM_R2Y_ADDR_RGB_INFO	rgb;									/**< RGB address information	*/
} T_IM_R2Y_INADDR_INFO;

/** YYW output address information
*/
typedef union {
	T_IM_R2Y_ADDR_YCC_INFO	ycc;									/**< YCC address information	*/
} T_IM_R2Y_OUTADDR_INFO;

/** YYW output bank control
*/
typedef struct {
	UCHAR					bank_initial_position;					/**< YYWBANK.BANK0PS, YYWBANK.BANKAPS, YYWBANK.BANK1PS, YYWBANK.BANK2PS : Bank initial position
																	<ul><li>@ref D_IM_R2Y_YYW_BANK_0
																		<li>@ref D_IM_R2Y_YYW_BANK_1
																		<li>@ref D_IM_R2Y_YYW_BANK_2
																		<li>@ref D_IM_R2Y_YYW_BANK_3</ul>				*/
	UCHAR					use_bank_num;							/**< YYWCTL.USEBANK0, YYWCTL.USEBANKA, YYWCTL.USEBANK1, YYWCTL.USEBANK2 : Use bank number<br>
																	Use only one bank in the setting of 0 (d),
																	and use of the four banks in the setting of 3 (d).	*/
	T_IM_R2Y_OUTADDR_INFO	output_addr[D_IM_R2Y_YYW_BANK_MAX];		/**< Output bank address information				*/
} T_IM_R2Y_OUTBANK_INFO;

/** YYW output right side offset(for video format)
*/
typedef struct {
	UINT32					offset_byte[D_IM_R2Y_YYW_BANK_MAX];		/**< YYW0OFSX(multiple of 8), YYWAOFSX(multiple of 2) : Offset byte(12bits)	*/
} T_IM_R2Y_RIGHTSIDE_OFFSET;

/** Trimming control
*/
typedef struct {
	UCHAR					trimming_enable;						/**< YYW0MD.TRM0EN, YYW1MD.TRM1EN, YYW2MD.TRM2EN : Trimming enable (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Do not do the trimming
																		<li>@ref D_IM_R2Y_ENABLE_ON : Trimming	</ul> 							*/
	T_IM_R2Y_RECT			trim_window;							/**< TRM0H, TRM1H, TRM2H : start X position (13bits, multiple of 2)<br>
																		 TRM0V, TRM1V, TRM2V : start Y position (14bits, multiple of 2)			*/
} T_IM_R2Y_CTRL_TRIMMING;

/** Trimming control
*/
typedef struct {
	UCHAR					trimming_enable_b;						/**< YYW0MD.TRMBEN : Trimming enable (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Do not do the trimming
																		<li>@ref D_IM_R2Y_ENABLE_ON : Trimming	</ul> 							*/
	UCHAR					trimming_enable_c;						/**< YYW0MD.TRMCEN : Trimming enable (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Do not do the trimming
																		<li>@ref D_IM_R2Y_ENABLE_ON : Trimming	</ul> 							*/
	T_IM_R2Y_RECT			trim_window_b;							/**< TRMBH : start X position (13bits, multiple of 2)<br>
																		 TRMBV : start Y position (14bits, multiple of 2)						*/
	T_IM_R2Y_RECT			trim_window_c;							/**< TRMCH : start X position (13bits, multiple of 2)<br>
																		 TRMCV : start Y position (14bits, multiple of 2)						*/
} T_IM_R2Y_CTRL_TRIMMING_EXT;

/** Histogram control
*/
typedef struct {
	UCHAR					enable;									/**< HSTMD.HSTEN : Histogram enable (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Do not get the histogram
																		<li>@ref D_IM_R2Y_ENABLE_ON : Get the histogram</ul>						*/
	UCHAR					yyw_no;									/**< HSTMD.HSTSEL : Select a ch of YYW that you want to get the histogram. (1bit)
																	<ul><li>@ref D_IM_R2Y_YYW_CH_0
																		<li>@ref D_IM_R2Y_YYW_CH_1</ul>												*/
	UCHAR					sampling_pitch;							/**< HSTMD.PIT_M : Sampling pixel pitch (2bits)
																	<ul><li>@ref D_IM_R2Y_HIST_PIT_2X2
																		<li>@ref D_IM_R2Y_HIST_PIT_4X4
																		<li>@ref D_IM_R2Y_HIST_PIT_8X8
																		<li>@ref D_IM_R2Y_HIST_PIT_16X16</ul>										*/
	T_IM_R2Y_RECT			histogram_area;							/**< HSTH (13bits, multiple of 2), HSTV(14bits, multiple of 2) : Histogram area	*/
} T_IM_R2Y_CTRL_HISTOGRAM;

/** Histogram address
*/
typedef struct {
	USHORT*					y_buf;									/**< Y Histogram data copy destination address (USHORT x D_IM_R2Y_TABLE_MAX_HISTOGRAM) */
} T_IM_R2Y_HISTOGRAM_ADDR;

/** Histogram status
*/
typedef struct {
	UCHAR					y_overflow;								/**< HSTMD.YHSOVF : Y data overflow stauts (1bit)<br>
																	<p>When the acquisition time of the histogram for the Y data,
																	   the number of gray scale appearance of the same exceeds 32767d displays "1".
																	   Gradation appearance is greater than the number 32767d, you clip in 32767d.	*/
	UCHAR					running_status;							/**< HSTMD.HSTSTS : Histogram running status (1bit)<br>
																	<p>While trying to get the histogram to display "1".<br>
																	   When this bit is "1", the result of the histogram can not be read.			*/
} T_IM_R2Y_HISTOGRAM_STAT;

/** Partial color extraction information
*/
typedef struct {
	UCHAR					extract_enable;							/**< PCEMD.PCEEN : Partial color extraction enable (1bit)
																	<ul><li>@ref D_IM_R2Y_ENABLE_OFF
																		<li>@ref D_IM_R2Y_ENABLE_ON</ul>										*/
	SHORT					ref_axis_x;								/**< PCEXY.PCEX : Axis of the color you want to extract (X) (9bits signed)	*/
	SHORT					ref_axis_y;								/**< PCEXY.PCEY : Axis of the color you want to extract (Y) (9bits signed)	*/
} T_IM_R2Y_COLOR_EXTRACT;

/** Chroma scale value
*/
typedef struct {
	UCHAR					chroma_offset;							/**< PCEC.PCECOF : Offset chroma scale value (8bits 0 ~ 128)	*/
	USHORT					chroma_gain;							/**< PCEC.PCECGA : Gain chroma scale value (10bits)				*/
} T_IM_R2Y_CHROMA_SCALE;

/** Hue scale value
*/
typedef struct {
	UCHAR					hue_offset;								/**< PCEH.PCEHOF : Offset hue scale value (8bits 0 ~ 128)	*/
	USHORT					hue_gain;								/**< PCEH.PCEHGA : Gain hue scale value (10bits)			*/
} T_IM_R2Y_HUE_SCALE;

/** Post-resize edge enhancement0 control information
*/
typedef struct {
	UCHAR					edge_enable;							/**< EE0CTL.EE0EN : Post-resize edge enhancement 0 enable (1bit)					*/
	UCHAR					gradation_enable;						/**< EE0CTL.EE0TC : Gradation adjustment enable (1bit)								*/
	SHORT					hpf_k[6];								/**< EE0HPFK : High pass filter coefficient K0-K5.<br>
																					 EE0HPFK0(10bits unsigned), EE0HPFK1-EE0HPFK5(8bits signed).		*/
	UCHAR					strength;								/**< EE0NRLV : Noise reduction strength (4bits 0~8)									*/
	UCHAR					coring_p_offset[4];						/**< EE0CORPOF 0-3 : Coring positive direction offset (8bits)						*/
	INT8					coring_p_gain[4];						/**< EE0CORPGA 0-3 : Coring positive direction gain (8bits signed)					*/
	UCHAR					coring_p_border[3];						/**< EE0CORPBD 1-3 : Coring positive direction area boundary (8bits)				*/
	UCHAR					coring_m_offset[4];						/**< EE0CORMOF 0-3 : Coring negative direction offset (8bits)						*/
	INT8					coring_m_gain[4];						/**< EE0CORMGA 0-3 : Coring negative direction gain (8bits signed)					*/
	UCHAR					coring_m_border[3];						/**< EE0CORMBD 1-3 : Coring negative direction area boundary (8bits)				*/
	UCHAR					scale_reduct_coef;						/**< EE0SCLSUP : Scale reduction coefficient (6bits)								*/
	UCHAR					scale_p_offset[10];						/**< EE0SCLPOF 0-9 : Scale positive direction offset (8bits)						*/
	SHORT					scale_p_gain[10];						/**< EE0SCLPGA 0-9 : Scale positive direction gain (13bits signed)					*/
	UCHAR					scale_p_border[9];						/**< EE0SCLPBD 1-9 : Scale positive direction area boundary (8bits)					*/
	UCHAR					scale_m_offset[10];						/**< EE0SCLMOF 0-9 : Scale negative direction offset (8bits)						*/
	SHORT					scale_m_gain[10];						/**< EE0SCLMGA 0-9 : Scale negative direction gain (13bits signed)					*/
	UCHAR					scale_m_border[9];						/**< EE0SCLMBD 1-9 : Scale negative direction area boundary (8bits)					*/
	UCHAR					gradation_p_offset[5];					/**< EE0TONPOF 0-4 : Gradation adjustment positive direction offset (7bits)			*/
	SHORT					gradation_p_gain[5];					/**< EE0TONPGA 0-4 : Gradation adjustment positive direction gain (9bits signed)	*/
	UCHAR					gradation_p_border[4];					/**< EE0TONPBD 1-4 : Gradation adjustment positive direction area boundary (7bits)	*/
	UCHAR					gradation_m_offset[5];					/**< EE0TONMOF 0-4 : Gradation adjustment negative direction offset (7bits)			*/
	SHORT					gradation_m_gain[5];					/**< EE0TONMGA 0-4 : Gradation adjustment negative direction gain (9bits signed)	*/
	UCHAR					gradation_m_border[4];					/**< EE0TONMBD 1-4 : Gradation adjustment negative direction area boundary (7bits)	*/
	UCHAR					level_clip_p_offset[3];					/**< EE0CLPPOF 0-2 : Level clip positive direction offset (7bits)					*/
	INT8					level_clip_p_gain[3];					/**< EE0CLPPGA 0-2 : Level clip positive direction gain (8bits signed)				*/
	UCHAR					level_clip_p_border[2];					/**< EE0CLPPBD 1-2 : Level clip positive direction area boundary (8bits)			*/
	UCHAR					level_clip_m_offset[3];					/**< EE0CLPMOF 0-2 : Level clip negative direction offset (7bits)					*/
	INT8					level_clip_m_gain[3];					/**< EE0CLPMGA 0-2 : Level clip negative direction gain (8bits signed)				*/
	UCHAR					level_clip_m_border[2];					/**< EE0CLPMBD 1-2 : Level clip negative direction area boundary (8bits)			*/
} T_IM_R2Y_POST_RESIZE0_PARAM;

/** Post-resize edge enhancement1 control information
*/
typedef struct {
	UCHAR					edge_enable;							/**< EE1CTL.EE1EN : Post-resize edge enhancement 1 enable (1bit)					*/
	UCHAR					gradation_enable;						/**< EE1CTL.EE1TC : Gradation adjustment enable (1bit)								*/
	SHORT					hpf_k[6];								/**< EE1HPFK : High pass filter coefficient K0-K5.<br>
																					 EE0HPFK0(10bits unsigned), EE0HPFK1-EE0HPFK5(8bits signed).		*/
	UCHAR					strength;								/**< EE1NRLV : Noise reduction strength (4bits 0~8)									*/
	USHORT					coring_p_offset[4];						/**< EE1CORPOF 0-3 : Coring positive direction offset (10bits)						*/
	INT8					coring_p_gain[4];						/**< EE1CORPGA 0-3 : Coring positive direction gain (8bits signed)					*/
	USHORT					coring_p_border[3];						/**< EE1CORPBD 1-3 : Coring positive direction area boundary (10bits)				*/
	USHORT					coring_m_offset[4];						/**< EE1CORMOF 0-3 : Coring negative direction offset (10bits)						*/
	INT8					coring_m_gain[4];						/**< EE1CORMGA 0-3 : Coring negative direction gain (8bits signed)					*/
	USHORT					coring_m_border[3];						/**< EE1CORMBD 1-3 : Coring negative direction area boundary (10bits)				*/
	UCHAR					scale_reduct_coef;						/**< EE1SCLSUP : Scale reduction coefficient (6bits)								*/
	UCHAR					scale_p_offset[10];						/**< EE1SCLPOF 0-9 : Scale positive direction offset (8bits)						*/
	SHORT					scale_p_gain[10];						/**< EE1SCLPGA 0-9 : Scale positive direction gain (13bits signed)					*/
	USHORT					scale_p_border[9];						/**< EE1SCLPBD 1-9 : Scale positive direction area boundary (10bits)				*/
	UCHAR					scale_m_offset[10];						/**< EE1SCLMOF 0-9 : Scale negative direction offset (8bits)						*/
	SHORT					scale_m_gain[10];						/**< EE1SCLMGA 0-9 : Scale negative direction gain (13bits signed)					*/
	USHORT					scale_m_border[9];						/**< EE1SCLMBD 1-9 : Scale negative direction area boundary (10bits)				*/
	USHORT					gradation_p_offset[5];					/**< EE1TONPOF 0-4 : Gradation adjustment positive direction offset (9bits)			*/
	SHORT					gradation_p_gain[5];					/**< EE1TONPGA 0-4 : Gradation adjustment positive direction gain (9bits signed)	*/
	USHORT					gradation_p_border[4];					/**< EE1TONPOF 1-4 : Gradation adjustment positive direction area boundary (9bits)	*/
	USHORT					gradation_m_offset[5];					/**< EE1TONMOF 0-4 : Gradation adjustment negative direction offset (9bits)			*/
	SHORT					gradation_m_gain[5];					/**< EE1TONMGA 0-4 : Gradation adjustment negative direction gain (9bits signed)	*/
	USHORT					gradation_m_border[4];					/**< EE1TONMOF 1-4 : Gradation adjustment negative direction area boundary (9bits)	*/
	USHORT					level_clip_p_offset[3];					/**< EE1CLPPOF 0-2 : Level clip positive direction offset (9bits)					*/
	INT8					level_clip_p_gain[3];					/**< EE1CLPPGA 0-2 : Level clip positive direction gain (8bits signed)				*/
	USHORT					level_clip_p_border[2];					/**< EE1CLPPBD 1-2 : Level clip positive direction area boundary (10bits)			*/
	USHORT					level_clip_m_offset[3];					/**< EE1CLPMOF 0-2 : Level clip negative direction offset (9bits)					*/
	INT8					level_clip_m_gain[3];					/**< EE1CLPMGA 0-2 : Level clip negative direction gain (8bits signed)				*/
	USHORT					level_clip_m_border[2];					/**< EE1CLPMBD 1-2 : Level clip negative direction area boundary (10bits)			*/
} T_IM_R2Y_POST_RESIZE1_PARAM;

/* @} */	// im_r2y group


/** @weakgroup im_r2y_iq
@{
*/


/** Offset control of RGB data
*/
typedef struct {
	SHORT			R;							/**< OFSTR : Control of R pixel (10bits signed)	*/
	SHORT			G;							/**< OFSTG : Control of G pixel (10bits signed)	*/
	SHORT			B;							/**< OFSTB : Control of B pixel (10bits signed)	*/
} T_IM_R2Y_OFS;									/* Offset control data	*/

/** Color control of RGB data
*/
typedef struct {
	USHORT			R;							/**< WBGAR (11bits) / WBLVR (12bits) : Control of R pixel	*/
	USHORT			G;							/**< WBGAG (11bits) / WBLVG (12bits) : Control of G pixel	*/
	USHORT			B;							/**< WBGAB (11bits) / WBLVB (12bits) : Control of B pixel	*/
} T_IM_R2Y_RGB_COLOR;							/* RGB array control data	*/

/** CC0 Matrix control
*/
typedef struct {
	UCHAR			posi_dec;					/**< CCCTL.CC0DP : CC0 matrix decimal point position (2bits)			*/
	SHORT			cc_matrix[3][3];			/**< CC0K : CC-Matrix coefficient 3x3 (12bits signed)					*/
	USHORT			cybof[3];					/**< CC0YBOF : Blend luminance offset color correction (11bits)			*/
	SHORT			cybga[3];					/**< CC0YBGA : Blend color correction brightness gain (9bits signed)	*/
	USHORT			cybbd[2];					/**< CC0YBBD : Boundary blend color correction brightness (12bits)		*/
	UCHAR			cyc[3];						/**< CCYC : Coefficient of Yb convert (8bits)							*/
} T_IM_R2Y_CTRL_CC0;							/* CC-Matrix control	*/

/** Multi Axis control
*/
typedef struct {
	SHORT			cyc_coeff[9];				/**< MCYC : Coefficient of YC convert (9bits signed)				*/
	USHORT			boundary[16];				/**< MCB1A~MCB4D : Boundary of color correction (12bits)			*/
	UCHAR			area_index[4][5];			/**< MCID1A~MCID4E : Index value of correction area (4bits 0 ~ 11)	*/
	SHORT			area_a_k[5][3][3];			/**< MCKA_0_00~MCKA_4_22 : Convert data of area-A (12bits signed)	*/
	SHORT			area_b_k[5][3][3];			/**< MCKB_0_00~MCKB_4_22 : Convert data of area-B (12bits signed)	*/
	SHORT			area_c_k[5][3][3];			/**< MCKC_0_00~MCKC_4_22 : Convert data of area-C (12bits signed)	*/
	SHORT			area_d_k[5][3][3];			/**< MCKD_0_00~MCKD_4_22 : Convert data of area-D (12bits signed)	*/
	SHORT			area_e_k[5][3][3];			/**< MCKE_0_00~MCKE_4_22 : Convert data of area-E (12bits signed)	*/
	SHORT			area_f_k[5][3][3];			/**< MCKF_0_00~MCKF_4_22 : Convert data of area-F (12bits signed)	*/
	SHORT			area_g_k[5][3][3];			/**< MCKG_0_00~MCKG_4_22 : Convert data of area-G (12bits signed)	*/
	SHORT			area_h_k[5][3][3];			/**< MCKH_0_00~MCKH_4_22 : Convert data of area-H (12bits signed)	*/
	SHORT			area_i_k[5][3][3];			/**< MCKI_0_00~MCKI_4_22 : Convert data of area-I (12bits signed)	*/
	SHORT			area_j_k[5][3][3];			/**< MCKJ_0_00~MCKJ_4_22 : Convert data of area-J (12bits signed)	*/
	SHORT			area_k_k[5][3][3];			/**< MCKK_0_00~MCKK_4_22 : Convert data of area-K (12bits signed)	*/
	SHORT			area_l_k[5][3][3];			/**< MCKL_0_00~MCKL_4_22 : Convert data of area-L (12bits signed)	*/
	SHORT			area_a_l[5][3][2];			/**< MCLA_0_00~MCLA_4_21 : Convert data of area-A (10bits signed)	*/
	SHORT			area_b_l[5][3][2];			/**< MCLB_0_00~MCLB_4_21 : Convert data of area-B (10bits signed)	*/
	SHORT			area_c_l[5][3][2];			/**< MCLC_0_00~MCLC_4_21 : Convert data of area-C (10bits signed)	*/
	SHORT			area_d_l[5][3][2];			/**< MCLD_0_00~MCLD_4_21 : Convert data of area-D (10bits signed)	*/
	SHORT			area_e_l[5][3][2];			/**< MCLE_0_00~MCLE_4_21 : Convert data of area-E (10bits signed)	*/
	SHORT			area_f_l[5][3][2];			/**< MCLF_0_00~MCLF_4_21 : Convert data of area-F (10bits signed)	*/
	SHORT			area_g_l[5][3][2];			/**< MCLG_0_00~MCLG_4_21 : Convert data of area-G (10bits signed)	*/
	SHORT			area_h_l[5][3][2];			/**< MCLH_0_00~MCLH_4_21 : Convert data of area-H (10bits signed)	*/
	SHORT			area_i_l[5][3][2];			/**< MCLI_0_00~MCLI_4_21 : Convert data of area-I (10bits signed)	*/
	SHORT			area_j_l[5][3][2];			/**< MCLJ_0_00~MCLJ_4_21 : Convert data of area-J (10bits signed)	*/
	SHORT			area_k_l[5][3][2];			/**< MCLK_0_00~MCLK_4_21 : Convert data of area-K (10bits signed)	*/
	SHORT			area_l_l[5][3][2];			/**< MCLL_0_00~MCLL_4_21 : Convert data of area-L (10bits signed)	*/
	UCHAR			cyc_alpha_blend;			/**< MCYCBALP.MCYCBALP : CYC Alpha blend value (6bits)				*/
	USHORT			cyc_blend_gain[4];			/**< MCYCBGA : CYC blend gain (12bits)								*/
	USHORT			cyc_blend_border[4];		/**< MCYCBBD : CYC blend border (12bits)							*/
	UCHAR			cba_alpha_blend;			/**< MCBABALP.MCBABALP : Alpha blend value (6bits)					*/
	USHORT			cba_blend_offset[4];		/**< MCBABOF : CBA blend offset (11bits)							*/
	SHORT			cba_blend_gain[4];			/**< MCBABGA : CBA blend gain (9bits signed)						*/
	USHORT			cba_blend_border[3];		/**< MCBABBD : CBA blend border (12bits)							*/
} T_IM_R2Y_CTRL_MULTI_AXIS;						/* Multi Axis control	*/

/** Offset before TC data
*/
typedef struct {
	SHORT			R;							/**< TCOFR : Offset R correction value before TC(10bits signed)		*/
	SHORT			G;							/**< TCOFG : Offset G correction value before TC(10bits signed)		*/
	SHORT			B;							/**< TCOFB : Offset B correction value before TC(10bits signed)		*/
	SHORT			Yb;							/**< TCOFYB : Offset Yb correction value before TC(10bits signed)	*/
} T_IM_R2Y_TCOF;								/* Offset before TC data	*/

/** Luminance Evaluation before TC data
*/
typedef struct {
	UCHAR			tct_enable;					/**< TCTCTL.TCTEN : Luminance evaluation acquisition enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not acquire luminance evaluation value.
													<li>@ref D_IM_R2Y_ENABLE_ON : Acquires luminance evaluation value.</ul>	*/
	USHORT			start_x;					/**< TCTSTA.TCTHSTA : start X position (12bits, multiple of 2)				*/
	USHORT			start_y;					/**< TCTSTA.TCTVSTA : start Y position (14bits)								*/
	UCHAR			block_hsiz;					/**< TCTB.TCTBHSIZ : Horizontal size (8bits, multiple of 2, 8~128)			*/
	UCHAR			block_vsiz;					/**< TCTB.TCTBVSIZ : Vertical size (8bits, 4~128)							*/
	UCHAR			block_hnum;					/**< TCTB.TCTBHNUM : Block count(Horizontal) (6bits, 2~40)					*/
	UCHAR			block_vnum;					/**< TCTB.TCTBVNUM : Block count(Vertical) (5bits, 2~30)					*/
} T_IM_R2Y_TCT;									/* Luminance Evaluation before TC data	*/

/** histogram before TC data
*/
typedef struct {
	UCHAR			hist_enable;				/**< TCHSCTL.TCHSEN : Histogram acquisition enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not acquire histogram.
													<li>@ref D_IM_R2Y_ENABLE_ON : Acquires histogram.</ul>								*/
	UCHAR			sampling_hcyc;				/**< TCHSCTL.TCHSHCYC : Horizontal sampling interval setting (2bit)
												<ul><li>@ref D_IM_R2Y_HIST_HCYC_4
													<li>@ref D_IM_R2Y_HIST_HCYC_8
													<li>@ref D_IM_R2Y_HIST_HCYC_16</ul>													*/
	UCHAR			sampling_vcyc;				/**< TCHSCTL.TCHSVCYC : Vertical sampling interval setting (3bit)
												<ul><li>@ref D_IM_R2Y_HIST_VCYC_1
													<li>@ref D_IM_R2Y_HIST_VCYC_2
													<li>@ref D_IM_R2Y_HIST_VCYC_4
													<li>@ref D_IM_R2Y_HIST_VCYC_8
													<li>@ref D_IM_R2Y_HIST_VCYC_16</ul>													*/
	UCHAR			histogram_mode;				/**< TCHSCTL.TCHSRGBMD : RGB histogram mode (3bit)
												<ul><li>@ref D_IM_R2Y_HIST_BIT_WIDTH_7_0
													<li>@ref D_IM_R2Y_HIST_BIT_WIDTH_8_1
													<li>@ref D_IM_R2Y_HIST_BIT_WIDTH_9_2
													<li>@ref D_IM_R2Y_HIST_BIT_WIDTH_10_3
													<li>@ref D_IM_R2Y_HIST_BIT_WIDTH_11_4</ul>											*/
	UCHAR			hist_minus_mode;			/**< TCHSCTL.TCHSMN : RGB histogram minus value mode (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not count minus value.
													<li>@ref D_IM_R2Y_ENABLE_ON : Counts minus value as pixel value equals "0".</ul>	*/
	T_IM_R2Y_RECT	tchs_window;				/**< TCHSSTA.TCHSHSTA : Horizontal start position (12bits, multiple of 2)<br>
													 TCHSSTA.TCHSVSTA : Vertical start position (14bits)<br>
													 TCHSSIZ.TCHSHSIZ : Horizontal size (12bits, multiple of 2, 2~2360)<br>
													 TCHSSIZ.TCHSVSIZ : Vertical size (14bits, 1~8800)									*/
} T_IM_R2Y_TCHS;								/* Histogram before TC data	*/

/** Histogram overflow flag before TC data
*/
typedef struct {
	UCHAR			Y;							/**< TCHYOV : Histogram overflow flag Y component	*/
	UCHAR			R;							/**< TCHROV : Histogram overflow flag R component	*/
	UCHAR			G;							/**< TCHGOV : Histogram overflow flag G component	*/
	UCHAR			B;							/**< TCHBOV : Histogram overflow flag B component	*/
} T_IM_R2Y_TCHS_FLG;							/* Offset before TC data	*/

/** Tone control
*/
typedef struct {
	UCHAR			tone_enable;				/**< TCCTL.TCEN : TC RGB enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform TC to RGB.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs TC to RGB.</ul>					*/
	UCHAR			tone_yb_enable;				/**< TCCTL.TCYBEN : TC luminance Yb enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform TC to luminance Yb.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs TC to luminance Yb.</ul>			*/
	UCHAR			table_blend_enable;			/**< TCCTL.TCBLEN : TC table blend enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform blend.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs blend.</ul>						*/
	UCHAR			table_resol;				/**< TCCTL.TCRES : TC table reference resolution (2bits 0 ~ 2)
												<ul><li>@ref D_IM_R2Y_TC_RESOLUTION_1024
													<li>@ref D_IM_R2Y_TC_RESOLUTION_512
													<li>@ref D_IM_R2Y_TC_RESOLUTION_256</ul>								*/
	UCHAR			table_select;				/**< TCCTL.TCTBL : Tone control table select (2bits 0 ~ 3)					*/
	UCHAR			ytc_out;					/**< TCCTL.TCYOUT : TC YTc output selection (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : YTc = Y(coefficient "TCYC").
													<li>@ref D_IM_R2Y_ENABLE_ON : YTc = Yb(coefficient "CCYC").</ul>		*/
	UCHAR			int_bit;					/**< TCCTL.TCINTBIT : TC data integral part selection(used to set the format of TC and TCEP). (2bits)	*/
	UCHAR			table_blend_ratio;			/**< TCTBLCTL.TCBLND : TC table blend ratio (6bits 0 ~ 31)					*/
	UCHAR			yc_matrix[3];				/**< TCYC : Tone control YC matrix (8bits)									*/
	USHORT			table_endp[4];				/**< TCEP : Tone control end point table (15bits)							*/
	USHORT			clip_p_r;					/**< TCCLPR.TCCLPRP : R component after TC positive direction clip(12bit)	*/
	USHORT			clip_m_r;					/**< TCCLPR.TCCLPRM : R component after TC negative direction clip(12bit)	*/
	USHORT			clip_p_g;					/**< TCCLPG.TCCLPGP : G component after TC positive direction clip(12bit)	*/
	USHORT			clip_m_g;					/**< TCCLPG.TCCLPGM : G component after TC negative direction clip(12bit)	*/
	USHORT			clip_p_b;					/**< TCCLPB.TCCLPBP : B component after TC positive direction clip(12bit)	*/
	USHORT			clip_m_b;					/**< TCCLPB.TCCLPBM : B component after TC negative direction clip(12bit)	*/
} T_IM_R2Y_CTRL_TONE;

/** Gamma correction control
*/
typedef struct {
	UCHAR			gamma_enable;				/**< GMCTL.GMEN : Gamma correction enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform gamma correction.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs gamma correction.</ul>							*/
	UCHAR			gamma_mode;					/**< GMCTL.GMMD : Gamma correction mode (1bit)
												<ul><li>@ref D_IM_R2Y_GAMMA_MODE_12
													<li>@ref D_IM_R2Y_GAMMA_MODE_10</ul>													*/
	UCHAR			gamma_yb_tbl_simul;			/**< TGCTL.GAMSW : Gamma correction Yb Table simultaneous writing selection (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Simultaneous writing Yb table can be performed.
													<li>@ref D_IM_R2Y_ENABLE_ON : Simultaneous writing Yb table can't be performed.</ul>	*/
} T_IM_R2Y_CTRL_GAMMA;							/* Gamma correction control	*/

/** CC1 Matrix control
*/
typedef struct {
	UCHAR			posi_dec;					/**< CCCTL.CC1DP : CC1 matrix decimal point position (2bits)					*/
	SHORT			cc_matrix[3][3];			/**< CC1K : CC-Matrix coefficient 3x3 (12bits signed)							*/
	USHORT			clip_p_r;					/**< CC1CLPR.CC1CLPRP : R component after CC1 positive direction clip(12bit)	*/
	USHORT			clip_m_r;					/**< CC1CLPR.CC1CLPRM : R component after CC1 negative direction clip(12bit)	*/
	USHORT			clip_p_g;					/**< CC1CLPG.CC1CLPGP : G component after CC1 positive direction clip(12bit)	*/
	USHORT			clip_m_g;					/**< CC1CLPG.CC1CLPGM : G component after CC1 negative direction clip(12bit)	*/
	USHORT			clip_p_b;					/**< CC1CLPB.CC1CLPBP : B component after CC1 positive direction clip(12bit)	*/
	USHORT			clip_m_b;					/**< CC1CLPB.CC1CLPBM : B component after CC1 negative direction clip(12bit)	*/
} T_IM_R2Y_CTRL_CC1;							/* CC-Matrix control	*/

/** YC Convert control
*/
typedef struct {
	SHORT			yc_coeff[3][3];				/**< YC : YC convesion matrix coefficient (9bits signed)		*/
	UCHAR			y_blend_ratio;				/**< YBLEND.YYBLND : Luminance Y blend ratio (6bits 0 ~ 32)		*/
	UCHAR			yb_blend_ratio;				/**< YBLEND.YBBLND : Luminance Yb blend ratio (6bits 0 ~ 32)	*/
} T_IM_R2Y_CTRL_YCC;							/* YC convert control	*/

/** Y(Luminance) Noise Reduction control
*/
typedef struct {
	UCHAR			nr_mode;					/**< NRCTL.NRMD : Luminance noise reduction mode. (2bits 0~2)								*/
	UCHAR			blend_ratio;				/**< NRCTL.NRBLEND : Luminance blend before and after noise reduction (3bits 0~7)			*/
	USHORT			offset[4];					/**< NROF : Luminance noise reduction noise characteristic offset/LPF coefficient (10bits)	*/
	SHORT			gain[4];					/**< NRGA : Luminance noise reduction noise characteristic gain (11bits signed)				*/
	USHORT			border[3];					/**< NRBD : Luminance noise reduction noise characteristic area boundary (10bits)			*/
} T_IM_R2Y_CTRL_YNR;							/* Y-Noise Reduction control	*/

/** High/Medium frequency edge enhancement common control
*/
typedef struct {
	UCHAR			reduction_mode;				/**< EGSMCTL.EGSMT : Edge enhancement edge noise reduction mode (1bit)
												<ul><li>@ref D_IM_R2Y_EGSMT_VERSION1
													<li>@ref D_IM_R2Y_EGSMT_VERSION2</ul>										*/
	USHORT			threshold;					/**< EGSMTT.EGSMTTH : Edge noise reduction applying threshold value (10bits)	*/
	UCHAR			texture_gain;				/**< EGSMTT.EGSMTTXGA : Edge noise reduction texture gain (7bits 0~64)			*/
} T_IM_R2Y_CTRL_EDGE_CMN;						/* High/Medium frequency edge enhancement common control	*/

/** High frequency edge enhancement control
*/
typedef struct {
	UCHAR			edge_enable;				/**< EGHWCTL.EGHWEN : Edge enhancement enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform high-frequency edge enhancement.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs high-frequency edge enhancement</ul>				*/
	UCHAR			scale_enable;				/**< EGHWCTL.EGHWMP : Coordinate dependent scale enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform coordinate dependent scale adjustment.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs coordinate dependent scale adjustment.</ul>		*/
	UCHAR			table_clip_select;			/**< EGHWCTL.EGHWTC : Gradation adjustment table enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not refer to gradation table.
													<li>@ref D_IM_R2Y_ENABLE_ON : Refers to gradation table.</ul>							*/
	UCHAR			scale_table_resol;			/**< EGHWCTL.EGHWSCLRES : Scale table reference resolution (2bits 0 ~ 2)					*/
	UCHAR			scale_table_select;			/**< EGHWCTL.EGHWSCLTBL : Scale table selection (2bits)										*/
	UCHAR			gradation_table_resol;		/**< EGHWCTL.EGHWTCRES : Gradation adjustment table reference resolution (2bits 0 ~ 2)		*/
	UCHAR			gradation_table_select;		/**< EGHWCTL.EGHWTCTBL : Gradation adjustment table selection (2bits)						*/
	UCHAR			nr_level;					/**< EGHWCTL.EGHWNRLV : Noise reduction strength (4bits 0 ~ 8)								*/

	SHORT			hpf_k[6];					/**< EGHWHPFK : High pass filter coefficient K0-K5.<br>
													 EGHWHPFK0(10bits unsigned), EGHWHPFK1-EGHWHPFK5(8bits signed).							*/
	USHORT			corp_offset[4];				/**< EGHWCORPOF : Coring positive direction offset 0-3 (10bits)								*/
	INT8			corp_gain[4];				/**< EGHWCORPGA : Coring positive direction gain 0-3 (8bits signed)							*/
	USHORT			corp_border[3];				/**< EGHWCORPBD : Coring positive direction area boundary 1-3 (10bits)						*/
	USHORT			corm_offset[4];				/**< EGHWCORMOF : Coring negative direction offset 0-3 (10bits)								*/
	INT8			corm_gain[4];				/**< EGHWCORMGA : Coring negative direction gain 0-3 (8bits signed)							*/
	USHORT			corm_border[3];				/**< EGHWCORMBD : Coring negative direction area boundary 1-3 (10bits)						*/
	UCHAR			sup_scl;					/**< EGHWSCLSUP : Scale reduction coefficient (6bits)										*/
	USHORT			level_clip_p_offset[3];		/**< EGHWCLPPOF : Level clip positive direction offset (9bits)								*/
	INT8			level_clip_p_gain[3];		/**< EGHWCLPPGA : Level clip positive direction gain (8bits signed)							*/
	USHORT			level_clip_p_border[2];		/**< EGHWCLPPBD : Level clip positive direction area boundary (10bits)						*/
	USHORT			level_clip_m_offset[3];		/**< EGHWCLPMOF : Level clip negative direction offset (9bits)								*/
	INT8			level_clip_m_gain[3];		/**< EGHWCLPMGA : Level clip negative direction gain (8bits signed)							*/
	USHORT			level_clip_m_border[2];		/**< EGHWCLPMBD : Level clip negative direction area boundary (10bits)						*/
} T_IM_R2Y_CTRL_HEDGE;							/* High frequency edge enhancement control	*/

/** Medium frequency edge enhancement control
*/
typedef struct {
	UCHAR			edge_enable;				/**< EGMWCTL.EGMWEN : Edge enhancement enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform medium-frequency edge enhancement.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs medium-frequency edge enhancement</ul>			*/
	UCHAR			scale_enable;				/**< EGMWCTL.EGMWMP : Coordinate dependent scale enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform coordinate dependent scale adjustment.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs coordinate dependent scale adjustment.</ul>		*/
	UCHAR			table_clip_select;			/**< EGMWCTL.EGMWTC : Gradation adjustment table enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not refer to gradation table.
													<li>@ref D_IM_R2Y_ENABLE_ON : Refers to gradation table.</ul>							*/
	UCHAR			scale_table_resol;			/**< EGMWCTL.EGMWSCLRES : Scale table reference resolution (2bits 0 ~ 2)					*/
	UCHAR			scale_table_select;			/**< EGMWCTL.EGMWSCLTBL : Scale table selection (2bits)										*/
	UCHAR			gradation_table_resol;		/**< EGMWCTL.EGMWTCRES : Gradation adjustment table reference resolution (2bits 0 ~ 2)		*/
	UCHAR			gradation_table_select;		/**< EGMWCTL.EGMWTCTBL : Gradation adjustment table selection (2bits)						*/
	UCHAR			nr_level;					/**< EGMWCTL.EGMWNRLV : Noise reduction strength (4bits 0 ~ 8)								*/

	SHORT			hpf_k[6];					/**< EGMWHPFK : High pass filter coefficient K0-K5.<br>
													 EGMWHPFK0(10bits unsigned), EGMWHPFK1-EGMWHPFK5(8bits signed).							*/
	USHORT			corp_offset[4];				/**< EGMWCORPOF : Coring positive direction offset 0-3 (10bits)								*/
	INT8			corp_gain[4];				/**< EGMWCORPGA : Coring positive direction gain 0-3 (8bits signed)							*/
	USHORT			corp_border[3];				/**< EGMWCORPBD : Coring positive direction area boundary 1-3 (10bits)						*/
	USHORT			corm_offset[4];				/**< EGMWCORMOF : Coring negative direction offset 0-3 (10bits)								*/
	INT8			corm_gain[4];				/**< EGMWCORMGA : Coring negative direction gain 0-3 (8bits signed)							*/
	USHORT			corm_border[3];				/**< EGMWCORMBD : Coring negative direction area boundary 1-3 (10bits)						*/
	UCHAR			sup_scl;					/**< EGMWSCLSUP : Scale reduction coefficient (6bits)										*/
	USHORT			level_clip_p_offset[3];		/**< EGMWCLPPOF : Level clip positive direction offset (9bits)								*/
	INT8			level_clip_p_gain[3];		/**< EGMWCLPPGA : Level clip positive direction gain (8bits signed)							*/
	USHORT			level_clip_p_border[2];		/**< EGMWCLPPBD : Level clip positive direction area boundary (10bits)						*/
	USHORT			level_clip_m_offset[3];		/**< EGMWCLPMOF : Level clip negative direction offset (9bits)								*/
	INT8			level_clip_m_gain[3];		/**< EGMWCLPMGA : Level clip negative direction gain (8bits signed)							*/
	USHORT			level_clip_m_border[2];		/**< EGMWCLPMBD : Level clip negative direction area boundary (10bits)						*/
} T_IM_R2Y_CTRL_MEDGE;							/* Medium frequency edge enhancement control	*/

/** Low frequency edge enhancement control
*/
typedef struct {
	UCHAR			edge_enable;				/**< EGLWCTL.EGLWEN : Edge enhancement enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform low-frequency edge enhancement
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs low-frequency edge enhancement</ul>				*/
	UCHAR			scale_enable;				/**< EGLWCTL.EGLWMP : Coordinate dependent scale enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform coordinate dependent scale adjustment.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs coordinate dependent scale adjustment.</ul>		*/
	UCHAR			table_clip_select;			/**< EGLWCTL.EGLWTC : Gradation adjustment table enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not refer to gradation table.
													<li>@ref D_IM_R2Y_ENABLE_ON : Refers to gradation table.</ul>							*/
	UCHAR			scale_table_resol;			/**< EGLWCTL.EGLWSCLRES : Scale table reference resolution (2bits 0 ~ 2)	*/
	UCHAR			scale_table_select;			/**< EGLWCTL.EGLWSCLTBL : Scale table selection (2bits)					*/
	UCHAR			gradation_table_resol;		/**< EGLWCTL.EGLWTCRES : Gradation adjustment table reference resolution (2bits 0 ~ 2)		*/
	UCHAR			gradation_table_select;		/**< EGLWCTL.EGLWTCTBL : Gradation adjustment table selection (2bits)						*/

	SHORT			hpf_k[10];					/**< EGLWHPFK : High pass filter coefficient K0-K9.<br>
													 EGLWHPFK0(10bits unsigned), EGLWHPFK1-EGLWHPFK9(8bits signed).							*/
	USHORT			corp_offset[4];				/**< EGLWCORPOF : Coring positive direction offset 0-3 (10bits)								*/
	INT8			corp_gain[4];				/**< EGLWCORPGA : Coring positive direction gain 0-3 (8bits signed)							*/
	USHORT			corp_border[3];				/**< EGLWCORPBD : Coring positive direction boundary 1-3 (10bits)							*/
	USHORT			corm_offset[4];				/**< EGLWCORMOF : Coring negative direction offset 0-3 (10bits)								*/
	INT8			corm_gain[4];				/**< EGLWCORMGA : Coring negative direction gain 0-3 (8bits signed)							*/
	USHORT			corm_border[3];				/**< EGLWCORMBD : Coring negative direction boundary 1-3 (10bits)							*/
	UCHAR			sup_scl;					/**< EGLWSCLSUP : Scale reduction coefficient (6bits)										*/
	USHORT			level_clip_p_offset[3];		/**< EGLWCLPPOF : Level clip positive direction offset (9bits)								*/
	INT8			level_clip_p_gain[3];		/**< EGLWCLPPGA : Level clip positive direction gain (8bits signed)							*/
	USHORT			level_clip_p_border[2];		/**< EGLWCLPPBD : Level clip positive direction border (10bits)								*/
	USHORT			level_clip_m_offset[3];		/**< EGLWCLPMOF : Level clip negative direction offset (9bits)								*/
	INT8			level_clip_m_gain[3];		/**< EGLWCLPMGA : Level clip negative direction gain (8bits signed)							*/
	USHORT			level_clip_m_border[2];		/**< EGLWCLPMBD : Level clip negative direction border (10bits)								*/
} T_IM_R2Y_CTRL_LEDGE;							/* Low frequency edge enhancement control	*/

/** Edge dot noise correction control
*/
typedef struct {
	UCHAR			hf_enable;					/**< EGDTCTL.EGDTHW : High frequency edge dot noise correction enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform high-frequency edge dot noise correction.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs high-frequency edge dot noise correction.</ul>		*/
	UCHAR			mf_enable;					/**< EGDTCTL.EGDTMW : Medium frequency edge dot noise correction enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform medium-frequency edge dot noise correction.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs medium-frequency edge dot noise correction.</ul>		*/
	USHORT			hf_p_threshold;				/**< EGDTHWTH.EGDTHWTHP : Threshold for high-frequency edge dot noise detection(+) (10bits)		*/
	USHORT			hf_m_threshold;				/**< EGDTHWTH.EGDTHWTHM : Threshold for high-frequency edge dot noise detection(-) (10bits)		*/
	UCHAR			hf_p_coef;					/**< EGDTHWK.EGDTHWKP : High-frequency edge dot noise correction coefficient(+) (4bits 0~8)		*/
	UCHAR			hf_m_coef;					/**< EGDTHWK.EGDTHWKM : High-frequency edge dot noise correction coefficient(-) (4bits 0~8)		*/
	USHORT			mf_p_threshold;				/**< EGDTMWTH.EGDTMWTHP : Threshold for Medium-frequency edge dot noise detection(+) (10bits)	*/
	USHORT			mf_m_threshold;				/**< EGDTMWTH.EGDTMWTHM : Threshold for Medium-frequency edge dot noise detection(-) (10bits)	*/
	UCHAR			mf_p_coef;					/**< EGDTMWK.EGDTMWKP : Medium-frequency edge dot noise correction coefficient(+) (4bits 0~8)	*/
	UCHAR			mf_m_coef;					/**< EGDTMWK.EGDTMWKM : Medium-frequency edge dot noise correction coefficient(-) (4bits 0~8)	*/
} T_IM_R2Y_CTRL_EDGE_DOT_NOISE;					/* Edge dot noise correction control	*/

/** Map scale control
*/
typedef struct {
	USHORT			block_size_h;				/**< EGMPSIZ.EGMPSIZH : Horizontal block size (10bits 16 ~ 1022)									*/
	USHORT			block_size_v;				/**< EGMPSIZ.EGMPSIZV : Vertical block size (10bits 16 ~ 1023)										*/
	USHORT			recip_multipli_h;			/**< EGMPSIZHINV.EGMPINVH : Horizontal reciprocal multiplication value (14bits)						*/
	UCHAR			recip_multipli_shift_h;		/**< EGMPSIZHINV.EGMPINVSFTH : Amount multiplied by the reciprocal horizontal shift (3bits 0 ~ 6)	*/
	USHORT			recip_multipli_v;			/**< EGMPSIZVINV.EGMPINVV : Vertical reciprocal multiplication value (14bits)						*/
	UCHAR			recip_multipli_shift_v;		/**< EGMPSIZVINV.EGMPINVSFTV : Amount multiplied by the reciprocal vertical shift (3bits 0 ~ 6)		*/
	UCHAR			block_start_h;				/**< EGMPSTAPH.EGMPSTABH : Block horizontal start (4bits 0 ~ 15)									*/
	USHORT			block_start_coord_h;		/**< EGMPSTAPH.EGMPSTACH : Horizontal start coordinate in the block (10bits 0 ~ 1022)				*/
	UCHAR			block_start_v;				/**< EGMPSTAPV.EGMPSTAPV : Block horizontal start (4bits 0 ~ 11)									*/
	USHORT			block_start_coord_v;		/**< EGMPSTAPV.EGMPSTACV : Horizontal start coordinate in the block (10bits 0 ~ 1023)				*/
} T_IM_R2Y_CTRL_MAPSCL;							/* Map scale control	*/

/** Edge blend control
*/
typedef struct {
	UCHAR			blend_type;					/**< EGCMPCTL.EGCMPS : Edge blend type select (1bits)								*/
	USHORT			border[3];					/**< EGCMPBD : Edge synthesis blend coefficient area boundary 1-3 (10bits)			*/
	UCHAR			alpha_offset[4];			/**< EGCMPALPOF : Edge synthesis blend coefficient alpha offset 0-3 (8bits 0 ~ 128)	*/
	SHORT			alpha_gain[4];				/**< EGCMPALPGA : Edge synthesis blend coefficient alpha gain 0-3 (10bits signed)	*/
	UCHAR			beta_offset[4];				/**< EGCMPBTAOF : Edge synthesis blend coefficient beta offset 0-3 (8bits 0 ~ 128)	*/
	SHORT			beta_gain[4];				/**< EGCMPBTAGA : Edge synthesis blend coefficient beta gain 0-3 (10bits signed)	*/
	USHORT			level_clip_p;				/**< EGCMPCLP.EGCMPCLPP : Edge synthesis positive direction level clip (9bits)		*/
	USHORT			level_clip_m;				/**< EGCMPCLP.EGCMPCLPM : Edge synthesis negative direction level clip (9bits)		*/
} T_IM_R2Y_CTRL_EDGE_BLEND;						/* Edge blend control	*/

/** Chroma referenced edge and texture adjustment control common
 */
typedef struct {
	INT8			cb_a_focus1_pos;			/**< CRVAF.CRVAFX1 : Ellipse focus 1 Cb coordinate of color difference reference correction area A (8bits signed) */
	INT8			cr_a_focus1_pos;			/**< CRVAF.CRVAFY1 : Ellipse focus 1 Cr coordinate of color difference reference correction area A (8bits signed) */
	INT8			cb_a_focus2_pos;			/**< CRVAF.CRVAFX2 : Ellipse focus 2 Cb coordinate of color difference reference correction area A (8bits signed) */
	INT8			cr_a_focus2_pos;			/**< CRVAF.CRVAFY2 : Ellipse focus 2 Cr coordinate of color difference reference correction area A (8bits signed) */
	INT8			cb_b_focus1_pos;			/**< CRVBF.CRVBFX1 : Ellipse focus 1 Cb coordinate of color difference reference correction area B (8bits signed) */
	INT8			cr_b_focus1_pos;			/**< CRVBF.CRVBFY1 : Ellipse focus 1 Cr coordinate of color difference reference correction area B (8bits signed) */
	INT8			cb_b_focus2_pos;			/**< CRVBF.CRVBFX2 : Ellipse focus 2 Cb coordinate of color difference reference correction area B (8bits signed) */
	INT8			cr_b_focus2_pos;			/**< CRVBF.CRVBFY2 : Ellipse focus 2 Cr coordinate of color difference reference correction area B (8bits signed) */
} T_IM_R2Y_CTRL_C_REF_EDGE_TEXTURE_ADJ_COMMON;

/** Chroma referenced edge adjustment control
 */
typedef struct {
	UCHAR			area_a_enable;				/**< EGCRVCTL.EGCRAE : Set whether color difference reference edge correction is
													 performed or not by using correction area A. (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF :
														Does not perform color difference regerence edge correction by using correction area A.
													<li>@ref D_IM_R2Y_ENABLE_ON :
														Performs color difference regerence edge correction by using correction area A.</ul>		*/
	UCHAR			area_b_enable;				/**< EGCRVCTL.EGCRBE : Set whether color difference reference edge correction is
													 performed or not by using correction area A. (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF :
														Does not perform color difference regerence edge correction by using correction area B.
													<li>@ref D_IM_R2Y_ENABLE_ON :
														Performs color difference regerence edge correction by using correction area B.</ul>		*/
	UCHAR			merge_method;				/**< EGCRVCTL.EGCRVMRG : Color difference reference edge correction area AB merge method (2bits 0~2)*/
	UCHAR			area_a_scale_offset;		/**< EGCRVASCLOF : Area A scale offset (8bits)														*/
	SHORT			area_a_scale_gain[2];		/**< EGCRVASCLGA : Area A scale gain (11bits signed)												*/
	UCHAR			area_a_scale_border;		/**< EGCRVASCLBD : Area A scale border (7bits)														*/
	UCHAR			area_a_scale_clip_lo;		/**< EGCRVASCLCP.EGCRVASCLCPL : Area A scale lower clip (8bits)										*/
	UCHAR			area_a_scale_clip_hi;		/**< EGCRVASCLCP.EGCRVASCLCPH : Area A scale upper clip (8bits)										*/
	UCHAR			area_a_y_scale_gain_p;		/**< EGYASCLGA.EGYASCLGA0 : Area A luminance scale plus gain0 (8bits)								*/
	UCHAR			area_a_y_scale_gain_m;		/**< EGYASCLGA.EGYASCLGA1 : Area A luminance scale minus gain1 (8bits)								*/
	UCHAR			area_a_y_scale_border[2];	/**< EGYASCLBD : Area A luminance scale border (8bits)												*/
	USHORT			area_a_y_scale_clip_lo;		/**< EGYASCLCLP.EGYASCLCLPL : Area A luminance scale lower clip (9bits)								*/
	USHORT			area_a_y_scale_clip_hi;		/**< EGYASCLCLP.EGYASCLCLPH : Area A luminance scale upper clip (9bits)								*/
	UCHAR			area_b_scale_offset;		/**< EGCRVBSCLOF : Area B scale offset (8bits)														*/
	SHORT			area_b_scale_gain[2];		/**< EGCRVBSCLGA : Area B scale gain (11bits signed)												*/
	UCHAR			area_b_scale_border;		/**< EGCRVBSCLBD : Area B scale border (7bits)														*/
	UCHAR			area_b_scale_clip_lo;		/**< EGCRVBSCLCP.EGCRVBSCLCPL : Area B scale lower clip (8bits)										*/
	UCHAR			area_b_scale_clip_hi;		/**< EGCRVBSCLCP.EGCRVBSCLCPH : Area B scale upper clip (8bits)										*/
	UCHAR			area_b_y_scale_gain_p;		/**< EGYBSCLGA.EGYBSCLGA0 : Area B luminance scale plus gain0 (8bits)								*/
	UCHAR			area_b_y_scale_gain_m;		/**< EGYBSCLGA.EGYBSCLGA1 : Area B luminance scale minus gain1 (8bits)								*/
	UCHAR			area_b_y_scale_border[2];	/**< EGYBSCLBD : Area B luminance scale border (8bits)												*/
	USHORT			area_b_y_scale_clip_lo;		/**< EGYBSCLCLP.EGYBSCLCLPL : Area B luminance scale lower clip (9bits)								*/
	USHORT			area_b_y_scale_clip_hi;		/**< EGYBSCLCLP.EGYBSCLCLPH : Area B luminance scale upper clip (9bits)								*/
	USHORT			level_clip_p;				/**< EGCRVCLP.EGCRVCLPP : Level clip plus (9bits)													*/
	USHORT			level_clip_m;				/**< EGCRVCLP.EGCRVCLPM : Level clip minus (9bits)													*/
} T_IM_R2Y_CTRL_C_REF_EDGE_ADJ;

/** Chroma referenced luminance blend control common
 */
typedef struct {
	UCHAR			area_a_enable;				/**< YBCRVCTL.YBCRAE : Set whether color difference regerence luminance blend is
													 performed or not by using correction area A. (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF :
														Does not perform color difference regerence luminance blend by using correction area A.
													<li>@ref D_IM_R2Y_ENABLE_ON :
														Performs color difference regerence luminance blend by using correction area A.</ul>	*/
	UCHAR			area_b_enable;				/**< YBCRVCTL.YBCRBE : Set whether color difference regerence luminance blend is
													 performed or not by using correction area B. (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF :
														Does not perform color difference regerence luminance blend by using correction area B.
													<li>@ref D_IM_R2Y_ENABLE_ON :
														Performs color difference regerence luminance blend by using correction area B.</ul>	*/
	UCHAR			area_a_offset;				/**< YBCRVAALPOF : offset value alpha in area A (8bits)											*/
	SHORT			area_a_gain[2];				/**< YBCRVAALPGA : gain value alpha in area A (11bits signed)									*/
	UCHAR			area_a_border;				/**< YBCRVAALPBD : border value alpha in area A (7bits)											*/
	UCHAR			area_a_scale_clip_lo;		/**< YBCRVAALPCP.YBCRVAALPCPL : lower clip in area A (8bits)									*/
	UCHAR			area_a_scale_clip_hi;		/**< YBCRVAALPCP.YBCRVAALPCPH : upper clip in area A (8bits)									*/
	UCHAR			area_a_correct_gain_p;		/**< YBYAALPGA.YBYAALPGA0 : Area A alpha value correction plus gain (8bits)						*/
	UCHAR			area_a_correct_gain_m;		/**< YBYAALPGA.YBYAALPGA1 : Area A alpha value correction minus gain (8bits)					*/
	UCHAR			area_a_correct_border[2];	/**< YBYAALPBD : Area A alpha value correction boudary (8bits)									*/
	UCHAR			area_a_correct_clip_lo;		/**< YBYAALPCLP.YBYAALPCLPL : Area A alpha value correction lower clip (8bits)					*/
	UCHAR			area_a_correct_clip_hi;		/**< YBYAALPCLP.YBYAALPCLPH : Area A alpha value correction upper clip (8bits)					*/
	UCHAR			area_b_offset;				/**< YBCRVBALPOF : offset value alpha in area B (8bits)											*/
	SHORT			area_b_gain[2];				/**< YBCRVBALPGA : gain value alpha in area B (11bits signed)									*/
	UCHAR			area_b_border;				/**< YBCRVBALPBD : border value alpha in area B (7bits)											*/
	UCHAR			area_b_scale_clip_lo;		/**< YBCRVBALPCP.YBCRVAALPCPL : lower clip in area B (8bits)									*/
	UCHAR			area_b_scale_clip_hi;		/**< YBCRVBALPCP.YBCRVAALPCPH : upper clip in area B (8bits)									*/
	UCHAR			area_b_correct_gain_p;		/**< YBYBALPGA.YBYBALPGA0 : Area B alpha value correction plus gain (8bits)						*/
	UCHAR			area_b_correct_gain_m;		/**< YBYBALPGA.YBYBALPGA1 : Area B alpha value correction minus gain (8bits)					*/
	UCHAR			area_b_correct_border[2];	/**< YBYBALPBD : Area B alpha value correction boudary (8bits)									*/
	UCHAR			area_b_correct_clip_lo;		/**< YBYBALPCLP.YBYBALPCLPL : Area B alpha value correction lower clip (8bits)					*/
	UCHAR			area_b_correct_clip_hi;		/**< YBYBALPCLP.YBYBALPCLPH : Area B alpha value correction upper clip (8bits)					*/
} T_IM_R2Y_CTRL_C_REF_YB_BLEND;

/** C(Chroma) Noise Reduction control
*/
typedef struct {
	UCHAR			clpf_enable;				/**< CLPFCTL.CLPFEN : Color Noise Reduction enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : Does not perform color noise reduction.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs color noise reduction.</ul>							*/
	UCHAR			interlock_blend_enable;		/**< CLPFCTL.CLPFYA : Color difference component LPF luminance cooperating blend enable (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF :
														Does not perform color difference component LPF luminance cooperating blend.
													<li>@ref D_IM_R2Y_ENABLE_ON :
														Performs color difference component LPF luminance cooperating blend.</ul>				*/
	SHORT			lpf_k[9];					/**< CLPFK: LPF kernel parameter (9bits signed)													*/
	UCHAR			lpf_ib_offset[4];			/**< CLPFOF: LPF kernel Interlock blend offset (8bits 0 ~ 128)									*/
	SHORT			lpf_ib_gain[4];				/**< CLPFGA: LPF kernel Interlock blend gain (10bits signed)									*/
	USHORT			lpf_ib_border[3];			/**< CLPFBD: LPF kernel Interlock blend border (10bits)											*/
} T_IM_R2Y_CTRL_CLPF;

/** Chroma Suppress control
*/
typedef struct {
	UCHAR			csy_enable;					/**< CSYCTL.CSYEN : Set whether reduction function by luminance/chroma
													 reference color difference reduction table is used or not. (1bit)
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF :
														Does not peform color difference reduction by luminance/chroma reference.
													<li>@ref D_IM_R2Y_ENABLE_ON :
														Peforms color difference reduction by luminance/chroma reference.</ul>		*/
	UCHAR			csy_mix_ratio;				/**< CSYCTL.CSYKY : Mixing ratio of Luminannce & Chroma (4bits 0 ~ 8)				*/
	UCHAR			csy_select_table;			/**< CSYCTL.CSYTBL : Selection of Chroma suppress table (1bit)						*/
	USHORT			csy_offset[4];				/**< CSYOF : offset (10bits)														*/
	SHORT			csy_gain[4];				/**< CSYGA : gain (11bits signed)													*/
	USHORT			csy_border[3];				/**< CSYBD : border (10bits)														*/
	UCHAR			y_rev_enable;				/**< YCRVFX.YRV : Enable of Y data reverse (1bit) <br>
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : No operation.
													<li>@ref D_IM_R2Y_ENABLE_ON : Sets luminance to 1023d-Y.</ul>					*/
	UCHAR			c_rev_enable;				/**< YCRVFX.CRV : Enable of Chroma data reverse (1bit) <br>
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : No operation.
													<li>@ref D_IM_R2Y_ENABLE_ON : Performs bit inversion to color difference.</ul>	*/
	UCHAR			c_fixed_enable;				/**< YCRVFX.CFIXEN : Enable of Chroma fixed control (1bit) <br>
												<ul><li>@ref D_IM_R2Y_ENABLE_OFF : No operation.
													<li>@ref D_IM_R2Y_ENABLE_ON : Sets color difference fixing enable.</ul>			*/
	SHORT			cb_fixed;					/**< CFIX.CFIXB : When enabled, c_fixed_enable is the value
																  that you set the component Cb. (10bits signed)					*/
	SHORT			cr_fixed;					/**< CFIX.CFIXR : When enabled, c_fixed_enable is the value
																  that you set the component Cr. (10bits signed)					*/
	SHORT			y_offset;					/**< YCOF.YOF : YOF is added to Y component processed
																by color difference reduction. (11bits signed)						*/
	SHORT			cb_offset;					/**< YCOF.COFB : (512d + COFB setting value) is added to Cb component processed
																 by color difference reduction. (11bits signed)						*/
	SHORT			cr_offset;					/**< YCOF.COFR : (512d + COFR setting value) is added to Cr component processed
																 by color difference reduction. (11bits signed)						*/
} T_IM_R2Y_CTRL_CS;								/* Chroma Suppress control	*/

/* @} */	// im_r2y_iq group

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
//------------ for RDMA ----------------
/** Post-resize edge enhancement0 control register address info (for RDMA)
*/
typedef struct {
	ULONG	EE0CTL;					/**< 2841_(C400 - C403h) */
	ULONG	EE0HPFK1;				/**< 2841_(C408 - C40Bh) */
	ULONG	EE0HPFK2;				/**< 2841_(C40C - C40Fh) */
	ULONG	EE0NRLV;				/**< 2841_(C410 - C413h) */
	ULONG	EE0CORPOF;				/**< 2841_(C414 - C417h) */
	ULONG	EE0CORPGA;				/**< 2841_(C418 - C41Bh) */
	ULONG	EE0CORPBD;				/**< 2841_(C41C - C41Fh) */
	ULONG	EE0CORMOF;				/**< 2841_(C420 - C423h) */
	ULONG	EE0CORMGA;				/**< 2841_(C424 - C427h) */
	ULONG	EE0CORMBD;				/**< 2841_(C428 - C42Bh) */
	ULONG	EE0SCLSUP;				/**< 2841_(C42C - C42Fh) */
	ULONG	EE0SCLPOF1;				/**< 2841_(C430 - C433h) */
	ULONG	EE0SCLPOF2;				/**< 2841_(C434 - C437h) */
	ULONG	EE0SCLPOF3;				/**< 2841_(C438 - C43Bh) */
	ULONG	EE0SCLPGA1;				/**< 2841_(C440 - C443h) */
	ULONG	EE0SCLPGA2;				/**< 2841_(C444 - C447h) */
	ULONG	EE0SCLPGA3;				/**< 2841_(C448 - C44Bh) */
	ULONG	EE0SCLPGA4;				/**< 2841_(C44C - C44Fh) */
	ULONG	EE0SCLPGA5;				/**< 2841_(C450 - C453h) */
	ULONG	EE0SCLPBD1;				/**< 2841_(C460 - C463h) */
	ULONG	EE0SCLPBD2;				/**< 2841_(C464 - C467h) */
	ULONG	EE0SCLPBD3;				/**< 2841_(C468 - C46Bh) */
	ULONG	EE0SCLMOF1;				/**< 2841_(C470 - C473h) */
	ULONG	EE0SCLMOF2;				/**< 2841_(C474 - C477h) */
	ULONG	EE0SCLMOF3;				/**< 2841_(C478 - C47Bh) */
	ULONG	EE0SCLMGA1;				/**< 2841_(C480 - C483h) */
	ULONG	EE0SCLMGA2;				/**< 2841_(C484 - C487h) */
	ULONG	EE0SCLMGA3;				/**< 2841_(C488 - C48Bh) */
	ULONG	EE0SCLMGA4;				/**< 2841_(C48C - C48Fh) */
	ULONG	EE0SCLMGA5;				/**< 2841_(C490 - C493h) */
	ULONG	EE0SCLMBD1;				/**< 2841_(C4A0 - C4A3h) */
	ULONG	EE0SCLMBD2;				/**< 2841_(C4A4 - C4A7h) */
	ULONG	EE0SCLMBD3;				/**< 2841_(C4A8 - C4ABh) */
	ULONG	EE0TONPOF1;				/**< 2841_(C4B0 - C4B3h) */
	ULONG	EE0TONPOF2;				/**< 2841_(C4B4 - C4B7h) */
	ULONG	EE0TONPGA1;				/**< 2841_(C4C0 - C4C3h) */
	ULONG	EE0TONPGA2;				/**< 2841_(C4C4 - C4C7h) */
	ULONG	EE0TONPGA3;				/**< 2841_(C4C8 - C4CBh) */
	ULONG	EE0TONPBD;				/**< 2841_(C4D0 - C4D3h) */
	ULONG	EE0TONMOF1;				/**< 2841_(C4D8 - C4DBh) */
	ULONG	EE0TONMOF2;				/**< 2841_(C4DC - C4DFh) */
	ULONG	EE0TONMGA1;				/**< 2841_(C4E0 - C4E3h) */
	ULONG	EE0TONMGA2;				/**< 2841_(C4E4 - C4E7h) */
	ULONG	EE0TONMGA3;				/**< 2841_(C4E8 - C4EBh) */
	ULONG	EE0TONMBD;				/**< 2841_(C4F0 - C4F3h) */
	ULONG	EE0CLPPOF;				/**< 2841_(C4F4 - C4F7h) */
	ULONG	EE0CLPPGA;				/**< 2841_(C4F8 - C4FBh) */
	ULONG	EE0CLPPBD;				/**< 2841_(C4FC - C4FFh) */
	ULONG	EE0CLPMOF;				/**< 2841_(C500 - C503h) */
	ULONG	EE0CLPMGA;				/**< 2841_(C504 - C507h) */
	ULONG	EE0CLPMBD;				/**< 2841_(C508 - C50Bh) */
} T_IM_R2Y_CTRL_RDMA_EE0_ADDR;

/** Post-resize edge enhancement1 control register address info (for RDMA)
*/
typedef struct {
	ULONG	EE1CTL;					/**< 2841_(C600 - C603h) */
	ULONG	EE1HPFK1;				/**< 2841_(C608 - C60Bh) */
	ULONG	EE1HPFK2;				/**< 2841_(C60C - C60Fh) */
	ULONG	EE1NRLV;				/**< 2841_(C610 - C613h) */
	ULONG	EE1CORPOF1;				/**< 2841_(C618 - C61Bh) */
	ULONG	EE1CORPOF2;				/**< 2841_(C61C - C61Fh) */
	ULONG	EE1CORPGA;				/**< 2841_(C620 - C623h) */
	ULONG	EE1CORPBD1;				/**< 2841_(C628 - C62Bh) */
	ULONG	EE1CORPBD2;				/**< 2841_(C62C - C62Fh) */
	ULONG	EE1CORMOF1;				/**< 2841_(C630 - C633h) */
	ULONG	EE1CORMOF2;				/**< 2841_(C634 - C637h) */
	ULONG	EE1CORMGA;				/**< 2841_(C638 - C63Bh) */
	ULONG	EE1CORMBD1;				/**< 2841_(C640 - C643h) */
	ULONG	EE1CORMBD2;				/**< 2841_(C644 - C647h) */
	ULONG	EE1SCLSUP;				/**< 2841_(C648 - C64Bh) */
	ULONG	EE1SCLPOF1;				/**< 2841_(C650 - C653h) */
	ULONG	EE1SCLPOF2;				/**< 2841_(C654 - C657h) */
	ULONG	EE1SCLPOF3;				/**< 2841_(C658 - C65Bh) */
	ULONG	EE1SCLPGA1;				/**< 2841_(C660 - C663h) */
	ULONG	EE1SCLPGA2;				/**< 2841_(C664 - C667h) */
	ULONG	EE1SCLPGA3;				/**< 2841_(C668 - C66Bh) */
	ULONG	EE1SCLPGA4;				/**< 2841_(C66C - C66Fh) */
	ULONG	EE1SCLPGA5;				/**< 2841_(C670 - C673h) */
	ULONG	EE1SCLPBD1;				/**< 2841_(C680 - C683h) */
	ULONG	EE1SCLPBD2;				/**< 2841_(C684 - C687h) */
	ULONG	EE1SCLPBD3;				/**< 2841_(C688 - C68Bh) */
	ULONG	EE1SCLPBD4;				/**< 2841_(C68C - C68Fh) */
	ULONG	EE1SCLPBD5;				/**< 2841_(C690 - C693h) */
	ULONG	EE1SCLMOF1;				/**< 2841_(C6A0 - C6A3h) */
	ULONG	EE1SCLMOF2;				/**< 2841_(C6A4 - C6A7h) */
	ULONG	EE1SCLMOF3;				/**< 2841_(C6A8 - C6ABh) */
	ULONG	EE1SCLMGA1;				/**< 2841_(C6C0 - C6C3h) */
	ULONG	EE1SCLMGA2;				/**< 2841_(C6C4 - C6C7h) */
	ULONG	EE1SCLMGA3;				/**< 2841_(C6C8 - C6CBh) */
	ULONG	EE1SCLMGA4;				/**< 2841_(C6CC - C6CFh) */
	ULONG	EE1SCLMGA5;				/**< 2841_(C6D0 - C6D3h) */
	ULONG	EE1SCLMBD1;				/**< 2841_(C6E0 - C6E3h) */
	ULONG	EE1SCLMBD2;				/**< 2841_(C6E4 - C6E7h) */
	ULONG	EE1SCLMBD3;				/**< 2841_(C6E8 - C6EBh) */
	ULONG	EE1SCLMBD4;				/**< 2841_(C6EC - C6EFh) */
	ULONG	EE1SCLMBD5;				/**< 2841_(C6F0 - C6F3h) */
	ULONG	EE1TONPOF1;				/**< 2841_(C700 - C703h) */
	ULONG	EE1TONPOF2;				/**< 2841_(C704 - C707h) */
	ULONG	EE1TONPOF3;				/**< 2841_(C708 - C70Bh) */
	ULONG	EE1TONPGA1;				/**< 2841_(C710 - C713h) */
	ULONG	EE1TONPGA2;				/**< 2841_(C714 - C717h) */
	ULONG	EE1TONPGA3;				/**< 2841_(C718 - C71Bh) */
	ULONG	EE1TONPBD1;				/**< 2841_(C720 - C723h) */
	ULONG	EE1TONPBD2;				/**< 2841_(C724 - C727h) */
	ULONG	EE1TONMOF1;				/**< 2841_(C730 - C733h) */
	ULONG	EE1TONMOF2;				/**< 2841_(C734 - C737h) */
	ULONG	EE1TONMOF3;				/**< 2841_(C738 - C73Bh) */
	ULONG	EE1TONMGA1;				/**< 2841_(C740 - C743h) */
	ULONG	EE1TONMGA2;				/**< 2841_(C744 - C747h) */
	ULONG	EE1TONMGA3;				/**< 2841_(C748 - C74Bh) */
	ULONG	EE1TONMBD1;				/**< 2841_(C750 - C753h) */
	ULONG	EE1TONMBD2;				/**< 2841_(C754 - C757h) */
	ULONG	EE1CLPPOF1;				/**< 2841_(C758 - C75Bh) */
	ULONG	EE1CLPPOF2;				/**< 2841_(C75C - C75Fh) */
	ULONG	EE1CLPPGA;				/**< 2841_(C760 - C763h) */
	ULONG	EE1CLPPBD;				/**< 2841_(C764 - C767h) */
	ULONG	EE1CLPMOF1;				/**< 2841_(C768 - C76Bh) */
	ULONG	EE1CLPMOF2;				/**< 2841_(C76C - C76Fh) */
	ULONG	EE1CLPMGA;				/**< 2841_(C770 - C773h) */
	ULONG	EE1CLPMBD;				/**< 2841_(C774 - C777h) */
} T_IM_R2Y_CTRL_RDMA_EE1_ADDR;

/** Deknee Table(R/G/B) register address info (for RDMA)
*/
typedef struct {
	ULONG	DKNTBL[128];			/**< 294A_(E000 - E1FFh) */
									/**< 294A_(E200 - E3FFh) */
									/**< 294A_(E400 - E5FFh) */
} T_IM_R2Y_CTRL_RDMA_DEKNEE_TBL_ADDR;

/** Offset control register address info (for RDMA)
*/
typedef struct {
	ULONG	OFST1;					/**< 2841_(8020 - 8023h) */
	ULONG	OFST2;					/**< 2841_(8024 - 8027h) */
} T_IM_R2Y_CTRL_RDMA_OFST_ADDR;

/** WB clip control register address info (for RDMA)
*/
typedef struct {
	ULONG	WBLV1;					/**< 2841_(8030 - 8033h) */
	ULONG	WBLV2;					/**< 2841_(8034 - 8037h) */
} T_IM_R2Y_CTRL_RDMA_WB_CLIP_ADDR;

/** CC0 matrix control register address info (for RDMA)
*/
typedef struct {
	ULONG	CC0CTL;					/**< 2841_(8080 - 8083h) */
	ULONG	CC0K1;					/**< 2841_(80A0 - 80A3h) */
	ULONG	CC0K2;					/**< 2841_(80A4 - 80A7h) */
	ULONG	CC0K3;					/**< 2841_(80A8 - 80ABh) */
	ULONG	CC0K4;					/**< 2841_(80AC - 80AFh) */
	ULONG	CC0K5;					/**< 2841_(80B0 - 80B3h) */
	ULONG	CC0YBOF1;				/**< 2841_(80C0 - 80C3h) */
	ULONG	CC0YBOF2;				/**< 2841_(80C4 - 80C7h) */
	ULONG	CC0YBGA1;				/**< 2841_(80C8 - 80CBh) */
	ULONG	CC0YBGA2;				/**< 2841_(80CC - 80CFh) */
	ULONG	CC0YBBD;				/**< 2841_(80D0 - 80D3h) */
	ULONG	CCYC;					/**< 2841_(80D4 - 80D7h) */
} T_IM_R2Y_CTRL_RDMA_CC0_ADDR;

/** CC0 matrix coefficient control register address info (for RDMA)
*/
typedef struct {
	ULONG	CC0K1;					/**< 2841_(80A0 - 80A3h) */
	ULONG	CC0K2;					/**< 2841_(80A4 - 80A7h) */
	ULONG	CC0K3;					/**< 2841_(80A8 - 80ABh) */
	ULONG	CC0K4;					/**< 2841_(80AC - 80AFh) */
	ULONG	CC0K5;					/**< 2841_(80B0 - 80B3h) */
} T_IM_R2Y_CTRL_RDMA_CC0_COEF_ADDR;

/** Multi Axis control register address info (for RDMA)
*/
typedef struct {
	ULONG	MCYC1;					/**< 2841_(9000 - 9003h) */
	ULONG	MCYC2;					/**< 2841_(9004 - 9007h) */
	ULONG	MCYC3;					/**< 2841_(9008 - 900Bh) */
	ULONG	MCYC4;					/**< 2841_(900C - 900Fh) */
	ULONG	MCYC5;					/**< 2841_(9000 - 9013h) */
	ULONG	MCB1AB;					/**< 2841_(9020 - 9023h) */
	ULONG	MCB1CD;					/**< 2841_(9024 - 9027h) */
	ULONG	MCB2AB;					/**< 2841_(9028 - 902Bh) */
	ULONG	MCB2CD;					/**< 2841_(902C - 902Fh) */
	ULONG	MCB3AB;					/**< 2841_(9030 - 9033h) */
	ULONG	MCB3CD;					/**< 2841_(9034 - 9037h) */
	ULONG	MCB4AB;					/**< 2841_(9038 - 903Bh) */
	ULONG	MCB4CD;					/**< 2841_(903C - 903Fh) */
	ULONG	MCID1;					/**< 2841_(9040 - 9043h) */
	ULONG	MCID2;					/**< 2841_(9044 - 9047h) */
	ULONG	MCID3;					/**< 2841_(9048 - 904Bh) */
	ULONG	MCID4;					/**< 2841_(904C - 904Fh) */
	ULONG	MCKA1;					/**< 2841_(9080 - 9083h) */
	ULONG	MCKA2;					/**< 2841_(9084 - 9087h) */
	ULONG	MCKA3;					/**< 2841_(9088 - 908Bh) */
	ULONG	MCKA4;					/**< 2841_(908C - 908Fh) */
	ULONG	MCKA5;					/**< 2841_(9090 - 9093h) */
	ULONG	MCKA6;					/**< 2841_(9094 - 9097h) */
	ULONG	MCKA7;					/**< 2841_(9098 - 909Bh) */
	ULONG	MCKA8;					/**< 2841_(909C - 909Fh) */
	ULONG	MCKA9;					/**< 2841_(90A0 - 90A3h) */
	ULONG	MCKA10;					/**< 2841_(90A4 - 90A7h) */
	ULONG	MCKA11;					/**< 2841_(90A8 - 90ABh) */
	ULONG	MCKA12;					/**< 2841_(90AC - 90AFh) */
	ULONG	MCKA13;					/**< 2841_(90B0 - 90B3h) */
	ULONG	MCKA14;					/**< 2841_(90B4 - 90B7h) */
	ULONG	MCKA15;					/**< 2841_(90B8 - 90BBh) */
	ULONG	MCKA16;					/**< 2841_(90BC - 90BFh) */
	ULONG	MCKA17;					/**< 2841_(90C0 - 90C3h) */
	ULONG	MCKA18;					/**< 2841_(90C4 - 90C7h) */
	ULONG	MCKA19;					/**< 2841_(90C8 - 90CBh) */
	ULONG	MCKA20;					/**< 2841_(90CC - 90CFh) */
	ULONG	MCKA21;					/**< 2841_(90D0 - 90D3h) */
	ULONG	MCKA22;					/**< 2841_(90D4 - 90D7h) */
	ULONG	MCKA23;					/**< 2841_(90D8 - 90DBh) */
	ULONG	MCKA24;					/**< 2841_(90DC - 90DFh) */
	ULONG	MCKA25;					/**< 2841_(90E0 - 90E3h) */
	ULONG	MCKB1;					/**< 2841_(9100 - 9103h) */
	ULONG	MCKB2;					/**< 2841_(9104 - 9107h) */
	ULONG	MCKB3;					/**< 2841_(9108 - 910Bh) */
	ULONG	MCKB4;					/**< 2841_(910C - 910Fh) */
	ULONG	MCKB5;					/**< 2841_(9110 - 9113h) */
	ULONG	MCKB6;					/**< 2841_(9114 - 9117h) */
	ULONG	MCKB7;					/**< 2841_(9118 - 911Bh) */
	ULONG	MCKB8;					/**< 2841_(911C - 911Fh) */
	ULONG	MCKB9;					/**< 2841_(9120 - 9123h) */
	ULONG	MCKB10;					/**< 2841_(9124 - 9127h) */
	ULONG	MCKB11;					/**< 2841_(9128 - 912Bh) */
	ULONG	MCKB12;					/**< 2841_(912C - 912Fh) */
	ULONG	MCKB13;					/**< 2841_(9130 - 9133h) */
	ULONG	MCKB14;					/**< 2841_(9134 - 9137h) */
	ULONG	MCKB15;					/**< 2841_(9138 - 913Bh) */
	ULONG	MCKB16;					/**< 2841_(913C - 913Fh) */
	ULONG	MCKB17;					/**< 2841_(9140 - 9143h) */
	ULONG	MCKB18;					/**< 2841_(9144 - 9147h) */
	ULONG	MCKB19;					/**< 2841_(9148 - 914Bh) */
	ULONG	MCKB20;					/**< 2841_(914C - 914Fh) */
	ULONG	MCKB21;					/**< 2841_(9150 - 9153h) */
	ULONG	MCKB22;					/**< 2841_(9154 - 9157h) */
	ULONG	MCKB23;					/**< 2841_(9158 - 915Bh) */
	ULONG	MCKB24;					/**< 2841_(915C - 915Fh) */
	ULONG	MCKB25;					/**< 2841_(9160 - 9163h) */
	ULONG	MCKC1;					/**< 2841_(9180 - 9183h) */
	ULONG	MCKC2;					/**< 2841_(9184 - 9187h) */
	ULONG	MCKC3;					/**< 2841_(9188 - 918Bh) */
	ULONG	MCKC4;					/**< 2841_(918C - 918Fh) */
	ULONG	MCKC5;					/**< 2841_(9190 - 9193h) */
	ULONG	MCKC6;					/**< 2841_(9194 - 9197h) */
	ULONG	MCKC7;					/**< 2841_(9198 - 919Bh) */
	ULONG	MCKC8;					/**< 2841_(919C - 919Fh) */
	ULONG	MCKC9;					/**< 2841_(91A0 - 91A3h) */
	ULONG	MCKC10;					/**< 2841_(91A4 - 91A7h) */
	ULONG	MCKC11;					/**< 2841_(91A8 - 91ABh) */
	ULONG	MCKC12;					/**< 2841_(91AC - 91AFh) */
	ULONG	MCKC13;					/**< 2841_(91B0 - 91B3h) */
	ULONG	MCKC14;					/**< 2841_(91B4 - 91B7h) */
	ULONG	MCKC15;					/**< 2841_(91B8 - 91BBh) */
	ULONG	MCKC16;					/**< 2841_(91BC - 91BFh) */
	ULONG	MCKC17;					/**< 2841_(91C0 - 91C3h) */
	ULONG	MCKC18;					/**< 2841_(91C4 - 91C7h) */
	ULONG	MCKC19;					/**< 2841_(91C8 - 91CBh) */
	ULONG	MCKC20;					/**< 2841_(91CC - 91CFh) */
	ULONG	MCKC21;					/**< 2841_(91D0 - 91D3h) */
	ULONG	MCKC22;					/**< 2841_(91D4 - 91D7h) */
	ULONG	MCKC23;					/**< 2841_(91D8 - 91DBh) */
	ULONG	MCKC24;					/**< 2841_(91DC - 91DFh) */
	ULONG	MCKC25;					/**< 2841_(91E0 - 91E3h) */
	ULONG	MCKD1;					/**< 2841_(9200 - 9203h) */
	ULONG	MCKD2;					/**< 2841_(9204 - 9207h) */
	ULONG	MCKD3;					/**< 2841_(9208 - 920Bh) */
	ULONG	MCKD4;					/**< 2841_(920C - 920Fh) */
	ULONG	MCKD5;					/**< 2841_(9210 - 9213h) */
	ULONG	MCKD6;					/**< 2841_(9214 - 9217h) */
	ULONG	MCKD7;					/**< 2841_(9218 - 921Bh) */
	ULONG	MCKD8;					/**< 2841_(921C - 921Fh) */
	ULONG	MCKD9;					/**< 2841_(9220 - 9223h) */
	ULONG	MCKD10;					/**< 2841_(9224 - 9227h) */
	ULONG	MCKD11;					/**< 2841_(9228 - 922Bh) */
	ULONG	MCKD12;					/**< 2841_(922C - 922Fh) */
	ULONG	MCKD13;					/**< 2841_(9230 - 9233h) */
	ULONG	MCKD14;					/**< 2841_(9234 - 9237h) */
	ULONG	MCKD15;					/**< 2841_(9238 - 923Bh) */
	ULONG	MCKD16;					/**< 2841_(923C - 923Fh) */
	ULONG	MCKD17;					/**< 2841_(9240 - 9243h) */
	ULONG	MCKD18;					/**< 2841_(9244 - 9247h) */
	ULONG	MCKD19;					/**< 2841_(9248 - 924Bh) */
	ULONG	MCKD20;					/**< 2841_(924C - 924Fh) */
	ULONG	MCKD21;					/**< 2841_(9250 - 9253h) */
	ULONG	MCKD22;					/**< 2841_(9254 - 9257h) */
	ULONG	MCKD23;					/**< 2841_(9258 - 925Bh) */
	ULONG	MCKD24;					/**< 2841_(925C - 925Fh) */
	ULONG	MCKD25;					/**< 2841_(9260 - 9263h) */
	ULONG	MCKE1;					/**< 2841_(9280 - 9283h) */
	ULONG	MCKE2;					/**< 2841_(9284 - 9287h) */
	ULONG	MCKE3;					/**< 2841_(9288 - 928Bh) */
	ULONG	MCKE4;					/**< 2841_(928C - 928Fh) */
	ULONG	MCKE5;					/**< 2841_(9290 - 9293h) */
	ULONG	MCKE6;					/**< 2841_(9294 - 9297h) */
	ULONG	MCKE7;					/**< 2841_(9298 - 929Bh) */
	ULONG	MCKE8;					/**< 2841_(929C - 929Fh) */
	ULONG	MCKE9;					/**< 2841_(92A0 - 92A3h) */
	ULONG	MCKE10;					/**< 2841_(92A4 - 92A7h) */
	ULONG	MCKE11;					/**< 2841_(92A8 - 92ABh) */
	ULONG	MCKE12;					/**< 2841_(92AC - 92AFh) */
	ULONG	MCKE13;					/**< 2841_(92B0 - 92B3h) */
	ULONG	MCKE14;					/**< 2841_(92B4 - 92B7h) */
	ULONG	MCKE15;					/**< 2841_(92B8 - 92BBh) */
	ULONG	MCKE16;					/**< 2841_(92BC - 92BFh) */
	ULONG	MCKE17;					/**< 2841_(92C0 - 92C3h) */
	ULONG	MCKE18;					/**< 2841_(92C4 - 92C7h) */
	ULONG	MCKE19;					/**< 2841_(92C8 - 92CBh) */
	ULONG	MCKE20;					/**< 2841_(92CC - 92CFh) */
	ULONG	MCKE21;					/**< 2841_(92D0 - 92D3h) */
	ULONG	MCKE22;					/**< 2841_(92D4 - 92D7h) */
	ULONG	MCKE23;					/**< 2841_(92D8 - 92DBh) */
	ULONG	MCKE24;					/**< 2841_(92DC - 92DFh) */
	ULONG	MCKE25;					/**< 2841_(92E0 - 92E3h) */
	ULONG	MCKF1;					/**< 2841_(9300 - 9303h) */
	ULONG	MCKF2;					/**< 2841_(9304 - 9307h) */
	ULONG	MCKF3;					/**< 2841_(9308 - 930Bh) */
	ULONG	MCKF4;					/**< 2841_(930C - 930Fh) */
	ULONG	MCKF5;					/**< 2841_(9310 - 9313h) */
	ULONG	MCKF6;					/**< 2841_(9314 - 9317h) */
	ULONG	MCKF7;					/**< 2841_(9318 - 931Bh) */
	ULONG	MCKF8;					/**< 2841_(931C - 931Fh) */
	ULONG	MCKF9;					/**< 2841_(9320 - 9323h) */
	ULONG	MCKF10;					/**< 2841_(9324 - 9327h) */
	ULONG	MCKF11;					/**< 2841_(9328 - 932Bh) */
	ULONG	MCKF12;					/**< 2841_(932C - 932Fh) */
	ULONG	MCKF13;					/**< 2841_(9330 - 9333h) */
	ULONG	MCKF14;					/**< 2841_(9334 - 9337h) */
	ULONG	MCKF15;					/**< 2841_(9338 - 933Bh) */
	ULONG	MCKF16;					/**< 2841_(933C - 933Fh) */
	ULONG	MCKF17;					/**< 2841_(9340 - 9343h) */
	ULONG	MCKF18;					/**< 2841_(9344 - 9347h) */
	ULONG	MCKF19;					/**< 2841_(9348 - 934Bh) */
	ULONG	MCKF20;					/**< 2841_(934C - 934Fh) */
	ULONG	MCKF21;					/**< 2841_(9350 - 9353h) */
	ULONG	MCKF22;					/**< 2841_(9354 - 9357h) */
	ULONG	MCKF23;					/**< 2841_(9358 - 935Bh) */
	ULONG	MCKF24;					/**< 2841_(935C - 935Fh) */
	ULONG	MCKF25;					/**< 2841_(9360 - 9363h) */
	ULONG	MCKG1;					/**< 2841_(9380 - 9383h) */
	ULONG	MCKG2;					/**< 2841_(9384 - 9387h) */
	ULONG	MCKG3;					/**< 2841_(9388 - 938Bh) */
	ULONG	MCKG4;					/**< 2841_(938C - 938Fh) */
	ULONG	MCKG5;					/**< 2841_(9390 - 9393h) */
	ULONG	MCKG6;					/**< 2841_(9394 - 9397h) */
	ULONG	MCKG7;					/**< 2841_(9398 - 939Bh) */
	ULONG	MCKG8;					/**< 2841_(939C - 939Fh) */
	ULONG	MCKG9;					/**< 2841_(93A0 - 93A3h) */
	ULONG	MCKG10;					/**< 2841_(93A4 - 93A7h) */
	ULONG	MCKG11;					/**< 2841_(93A8 - 93ABh) */
	ULONG	MCKG12;					/**< 2841_(93AC - 93AFh) */
	ULONG	MCKG13;					/**< 2841_(93B0 - 93B3h) */
	ULONG	MCKG14;					/**< 2841_(93B4 - 93B7h) */
	ULONG	MCKG15;					/**< 2841_(93B8 - 93BBh) */
	ULONG	MCKG16;					/**< 2841_(93BC - 93BFh) */
	ULONG	MCKG17;					/**< 2841_(93C0 - 93C3h) */
	ULONG	MCKG18;					/**< 2841_(93C4 - 93C7h) */
	ULONG	MCKG19;					/**< 2841_(93C8 - 93CBh) */
	ULONG	MCKG20;					/**< 2841_(93CC - 93CFh) */
	ULONG	MCKG21;					/**< 2841_(93D0 - 93D3h) */
	ULONG	MCKG22;					/**< 2841_(93D4 - 93D7h) */
	ULONG	MCKG23;					/**< 2841_(93D8 - 93DBh) */
	ULONG	MCKG24;					/**< 2841_(93DC - 93DFh) */
	ULONG	MCKG25;					/**< 2841_(93E0 - 93E3h) */
	ULONG	MCKH1;					/**< 2841_(9400 - 9403h) */
	ULONG	MCKH2;					/**< 2841_(9404 - 9407h) */
	ULONG	MCKH3;					/**< 2841_(9408 - 940Bh) */
	ULONG	MCKH4;					/**< 2841_(940C - 940Fh) */
	ULONG	MCKH5;					/**< 2841_(9410 - 9413h) */
	ULONG	MCKH6;					/**< 2841_(9414 - 9417h) */
	ULONG	MCKH7;					/**< 2841_(9418 - 941Bh) */
	ULONG	MCKH8;					/**< 2841_(941C - 941Fh) */
	ULONG	MCKH9;					/**< 2841_(9420 - 9423h) */
	ULONG	MCKH10;					/**< 2841_(9424 - 9427h) */
	ULONG	MCKH11;					/**< 2841_(9428 - 942Bh) */
	ULONG	MCKH12;					/**< 2841_(942C - 942Fh) */
	ULONG	MCKH13;					/**< 2841_(9430 - 9433h) */
	ULONG	MCKH14;					/**< 2841_(9434 - 9437h) */
	ULONG	MCKH15;					/**< 2841_(9438 - 943Bh) */
	ULONG	MCKH16;					/**< 2841_(943C - 943Fh) */
	ULONG	MCKH17;					/**< 2841_(9440 - 9443h) */
	ULONG	MCKH18;					/**< 2841_(9444 - 9447h) */
	ULONG	MCKH19;					/**< 2841_(9448 - 944Bh) */
	ULONG	MCKH20;					/**< 2841_(944C - 944Fh) */
	ULONG	MCKH21;					/**< 2841_(9450 - 9453h) */
	ULONG	MCKH22;					/**< 2841_(9454 - 9457h) */
	ULONG	MCKH23;					/**< 2841_(9458 - 945Bh) */
	ULONG	MCKH24;					/**< 2841_(945C - 945Fh) */
	ULONG	MCKH25;					/**< 2841_(9460 - 9463h) */
	ULONG	MCKI1;					/**< 2841_(9480 - 9483h) */
	ULONG	MCKI2;					/**< 2841_(9484 - 9487h) */
	ULONG	MCKI3;					/**< 2841_(9488 - 948Bh) */
	ULONG	MCKI4;					/**< 2841_(948C - 948Fh) */
	ULONG	MCKI5;					/**< 2841_(9490 - 9493h) */
	ULONG	MCKI6;					/**< 2841_(9494 - 9497h) */
	ULONG	MCKI7;					/**< 2841_(9498 - 949Bh) */
	ULONG	MCKI8;					/**< 2841_(949C - 949Fh) */
	ULONG	MCKI9;					/**< 2841_(94A0 - 94A3h) */
	ULONG	MCKI10;					/**< 2841_(94A4 - 94A7h) */
	ULONG	MCKI11;					/**< 2841_(94A8 - 94ABh) */
	ULONG	MCKI12;					/**< 2841_(94AC - 94AFh) */
	ULONG	MCKI13;					/**< 2841_(94B0 - 94B3h) */
	ULONG	MCKI14;					/**< 2841_(94B4 - 94B7h) */
	ULONG	MCKI15;					/**< 2841_(94B8 - 94BBh) */
	ULONG	MCKI16;					/**< 2841_(94BC - 94BFh) */
	ULONG	MCKI17;					/**< 2841_(94C0 - 94C3h) */
	ULONG	MCKI18;					/**< 2841_(94C4 - 94C7h) */
	ULONG	MCKI19;					/**< 2841_(94C8 - 94CBh) */
	ULONG	MCKI20;					/**< 2841_(94CC - 94CFh) */
	ULONG	MCKI21;					/**< 2841_(94D0 - 94D3h) */
	ULONG	MCKI22;					/**< 2841_(94D4 - 94D7h) */
	ULONG	MCKI23;					/**< 2841_(94D8 - 94DBh) */
	ULONG	MCKI24;					/**< 2841_(94DC - 94DFh) */
	ULONG	MCKI25;					/**< 2841_(94E0 - 94E3h) */
	ULONG	MCKJ1;					/**< 2841_(9500 - 9503h) */
	ULONG	MCKJ2;					/**< 2841_(9504 - 9507h) */
	ULONG	MCKJ3;					/**< 2841_(9508 - 950Bh) */
	ULONG	MCKJ4;					/**< 2841_(950C - 950Fh) */
	ULONG	MCKJ5;					/**< 2841_(9510 - 9513h) */
	ULONG	MCKJ6;					/**< 2841_(9514 - 9517h) */
	ULONG	MCKJ7;					/**< 2841_(9518 - 951Bh) */
	ULONG	MCKJ8;					/**< 2841_(951C - 951Fh) */
	ULONG	MCKJ9;					/**< 2841_(9520 - 9523h) */
	ULONG	MCKJ10;					/**< 2841_(9524 - 9527h) */
	ULONG	MCKJ11;					/**< 2841_(9528 - 952Bh) */
	ULONG	MCKJ12;					/**< 2841_(952C - 952Fh) */
	ULONG	MCKJ13;					/**< 2841_(9530 - 9533h) */
	ULONG	MCKJ14;					/**< 2841_(9534 - 9537h) */
	ULONG	MCKJ15;					/**< 2841_(9538 - 953Bh) */
	ULONG	MCKJ16;					/**< 2841_(953C - 953Fh) */
	ULONG	MCKJ17;					/**< 2841_(9540 - 9543h) */
	ULONG	MCKJ18;					/**< 2841_(9544 - 9547h) */
	ULONG	MCKJ19;					/**< 2841_(9548 - 954Bh) */
	ULONG	MCKJ20;					/**< 2841_(954C - 954Fh) */
	ULONG	MCKJ21;					/**< 2841_(9550 - 9553h) */
	ULONG	MCKJ22;					/**< 2841_(9554 - 9557h) */
	ULONG	MCKJ23;					/**< 2841_(9558 - 955Bh) */
	ULONG	MCKJ24;					/**< 2841_(955C - 955Fh) */
	ULONG	MCKJ25;					/**< 2841_(9560 - 9563h) */
	ULONG	MCKK1;					/**< 2841_(9580 - 9583h) */
	ULONG	MCKK2;					/**< 2841_(9584 - 9587h) */
	ULONG	MCKK3;					/**< 2841_(9588 - 958Bh) */
	ULONG	MCKK4;					/**< 2841_(958C - 958Fh) */
	ULONG	MCKK5;					/**< 2841_(9590 - 9593h) */
	ULONG	MCKK6;					/**< 2841_(9594 - 9597h) */
	ULONG	MCKK7;					/**< 2841_(9598 - 959Bh) */
	ULONG	MCKK8;					/**< 2841_(959C - 959Fh) */
	ULONG	MCKK9;					/**< 2841_(95A0 - 95A3h) */
	ULONG	MCKK10;					/**< 2841_(95A4 - 95A7h) */
	ULONG	MCKK11;					/**< 2841_(95A8 - 95ABh) */
	ULONG	MCKK12;					/**< 2841_(95AC - 95AFh) */
	ULONG	MCKK13;					/**< 2841_(95B0 - 95B3h) */
	ULONG	MCKK14;					/**< 2841_(95B4 - 95B7h) */
	ULONG	MCKK15;					/**< 2841_(95B8 - 95BBh) */
	ULONG	MCKK16;					/**< 2841_(95BC - 95BFh) */
	ULONG	MCKK17;					/**< 2841_(95C0 - 95C3h) */
	ULONG	MCKK18;					/**< 2841_(95C4 - 95C7h) */
	ULONG	MCKK19;					/**< 2841_(95C8 - 95CBh) */
	ULONG	MCKK20;					/**< 2841_(95CC - 95CFh) */
	ULONG	MCKK21;					/**< 2841_(95D0 - 95D3h) */
	ULONG	MCKK22;					/**< 2841_(95D4 - 95D7h) */
	ULONG	MCKK23;					/**< 2841_(95D8 - 95DBh) */
	ULONG	MCKK24;					/**< 2841_(95DC - 95DFh) */
	ULONG	MCKK25;					/**< 2841_(95E0 - 95E3h) */
	ULONG	MCKL1;					/**< 2841_(9600 - 9603h) */
	ULONG	MCKL2;					/**< 2841_(9604 - 9607h) */
	ULONG	MCKL3;					/**< 2841_(9608 - 960Bh) */
	ULONG	MCKL4;					/**< 2841_(960C - 960Fh) */
	ULONG	MCKL5;					/**< 2841_(9610 - 9613h) */
	ULONG	MCKL6;					/**< 2841_(9614 - 9617h) */
	ULONG	MCKL7;					/**< 2841_(9618 - 961Bh) */
	ULONG	MCKL8;					/**< 2841_(961C - 961Fh) */
	ULONG	MCKL9;					/**< 2841_(9620 - 9623h) */
	ULONG	MCKL10;					/**< 2841_(9624 - 9627h) */
	ULONG	MCKL11;					/**< 2841_(9628 - 962Bh) */
	ULONG	MCKL12;					/**< 2841_(962C - 962Fh) */
	ULONG	MCKL13;					/**< 2841_(9630 - 9633h) */
	ULONG	MCKL14;					/**< 2841_(9634 - 9637h) */
	ULONG	MCKL15;					/**< 2841_(9638 - 963Bh) */
	ULONG	MCKL16;					/**< 2841_(963C - 963Fh) */
	ULONG	MCKL17;					/**< 2841_(9640 - 9643h) */
	ULONG	MCKL18;					/**< 2841_(9644 - 9647h) */
	ULONG	MCKL19;					/**< 2841_(9648 - 964Bh) */
	ULONG	MCKL20;					/**< 2841_(964C - 964Fh) */
	ULONG	MCKL21;					/**< 2841_(9650 - 9653h) */
	ULONG	MCKL22;					/**< 2841_(9654 - 9657h) */
	ULONG	MCKL23;					/**< 2841_(9658 - 965Bh) */
	ULONG	MCKL24;					/**< 2841_(965C - 965Fh) */
	ULONG	MCKL25;					/**< 2841_(9660 - 9663h) */
	ULONG	MCLA1;					/**< 2841_(9680 - 9683h) */
	ULONG	MCLA2;					/**< 2841_(9684 - 9687h) */
	ULONG	MCLA3;					/**< 2841_(9688 - 968Bh) */
	ULONG	MCLA4;					/**< 2841_(968C - 96BFh) */
	ULONG	MCLA5;					/**< 2841_(9690 - 9693h) */
	ULONG	MCLA6;					/**< 2841_(9694 - 9697h) */
	ULONG	MCLA7;					/**< 2841_(9698 - 969Bh) */
	ULONG	MCLA8;					/**< 2841_(969C - 969Fh) */
	ULONG	MCLA9;					/**< 2841_(96A0 - 96A3h) */
	ULONG	MCLA10;					/**< 2841_(96A4 - 96A7h) */
	ULONG	MCLA11;					/**< 2841_(96A8 - 96ABh) */
	ULONG	MCLA12;					/**< 2841_(96AC - 96AFh) */
	ULONG	MCLA13;					/**< 2841_(96B0 - 96B3h) */
	ULONG	MCLA14;					/**< 2841_(96B4 - 96B7h) */
	ULONG	MCLA15;					/**< 2841_(96B8 - 96BBh) */
	ULONG	MCLB1;					/**< 2841_(96C0 - 96C3h) */
	ULONG	MCLB2;					/**< 2841_(96C4 - 96C7h) */
	ULONG	MCLB3;					/**< 2841_(96C8 - 96CBh) */
	ULONG	MCLB4;					/**< 2841_(96CC - 96CFh) */
	ULONG	MCLB5;					/**< 2841_(96D0 - 96D3h) */
	ULONG	MCLB6;					/**< 2841_(96D4 - 96D7h) */
	ULONG	MCLB7;					/**< 2841_(96D8 - 96DBh) */
	ULONG	MCLB8;					/**< 2841_(96DC - 96DFh) */
	ULONG	MCLB9;					/**< 2841_(96E0 - 96E3h) */
	ULONG	MCLB10;					/**< 2841_(96E4 - 96E7h) */
	ULONG	MCLB11;					/**< 2841_(96E8 - 96EBh) */
	ULONG	MCLB12;					/**< 2841_(96EC - 96EFh) */
	ULONG	MCLB13;					/**< 2841_(96F0 - 96F3h) */
	ULONG	MCLB14;					/**< 2841_(96F4 - 96F7h) */
	ULONG	MCLB15;					/**< 2841_(96F8 - 96FBh) */
	ULONG	MCLC1;					/**< 2841_(9700 - 9703h) */
	ULONG	MCLC2;					/**< 2841_(9704 - 9707h) */
	ULONG	MCLC3;					/**< 2841_(9708 - 970Bh) */
	ULONG	MCLC4;					/**< 2841_(970C - 970Fh) */
	ULONG	MCLC5;					/**< 2841_(9710 - 9713h) */
	ULONG	MCLC6;					/**< 2841_(9714 - 9717h) */
	ULONG	MCLC7;					/**< 2841_(9718 - 971Bh) */
	ULONG	MCLC8;					/**< 2841_(971C - 971Fh) */
	ULONG	MCLC9;					/**< 2841_(9720 - 9723h) */
	ULONG	MCLC10;					/**< 2841_(9724 - 9727h) */
	ULONG	MCLC11;					/**< 2841_(9728 - 972Bh) */
	ULONG	MCLC12;					/**< 2841_(972C - 972Fh) */
	ULONG	MCLC13;					/**< 2841_(9730 - 9733h) */
	ULONG	MCLC14;					/**< 2841_(9734 - 9737h) */
	ULONG	MCLC15;					/**< 2841_(9738 - 973Bh) */
	ULONG	MCLD1;					/**< 2841_(9740 - 9743h) */
	ULONG	MCLD2;					/**< 2841_(9744 - 9747h) */
	ULONG	MCLD3;					/**< 2841_(9748 - 974Bh) */
	ULONG	MCLD4;					/**< 2841_(974C - 974Fh) */
	ULONG	MCLD5;					/**< 2841_(9750 - 9753h) */
	ULONG	MCLD6;					/**< 2841_(9754 - 9757h) */
	ULONG	MCLD7;					/**< 2841_(9758 - 975Bh) */
	ULONG	MCLD8;					/**< 2841_(975C - 975Fh) */
	ULONG	MCLD9;					/**< 2841_(9760 - 9763h) */
	ULONG	MCLD10;					/**< 2841_(9764 - 9767h) */
	ULONG	MCLD11;					/**< 2841_(9768 - 976Bh) */
	ULONG	MCLD12;					/**< 2841_(976C - 976Fh) */
	ULONG	MCLD13;					/**< 2841_(9770 - 9773h) */
	ULONG	MCLD14;					/**< 2841_(9774 - 9777h) */
	ULONG	MCLD15;					/**< 2841_(9778 - 977Bh) */
	ULONG	MCLE1;					/**< 2841_(9780 - 9783h) */
	ULONG	MCLE2;					/**< 2841_(9784 - 9787h) */
	ULONG	MCLE3;					/**< 2841_(9788 - 978Bh) */
	ULONG	MCLE4;					/**< 2841_(978C - 97BFh) */
	ULONG	MCLE5;					/**< 2841_(9790 - 9793h) */
	ULONG	MCLE6;					/**< 2841_(9794 - 9797h) */
	ULONG	MCLE7;					/**< 2841_(9798 - 979Bh) */
	ULONG	MCLE8;					/**< 2841_(979C - 979Fh) */
	ULONG	MCLE9;					/**< 2841_(97A0 - 97A3h) */
	ULONG	MCLE10;					/**< 2841_(97A4 - 97A7h) */
	ULONG	MCLE11;					/**< 2841_(97A8 - 97ABh) */
	ULONG	MCLE12;					/**< 2841_(97AC - 97AFh) */
	ULONG	MCLE13;					/**< 2841_(97B0 - 97B3h) */
	ULONG	MCLE14;					/**< 2841_(97B4 - 97B7h) */
	ULONG	MCLE15;					/**< 2841_(97B8 - 97BBh) */
	ULONG	MCLF1;					/**< 2841_(97C0 - 97C3h) */
	ULONG	MCLF2;					/**< 2841_(97C4 - 97C7h) */
	ULONG	MCLF3;					/**< 2841_(97C8 - 97CBh) */
	ULONG	MCLF4;					/**< 2841_(97CC - 97CFh) */
	ULONG	MCLF5;					/**< 2841_(97D0 - 97D3h) */
	ULONG	MCLF6;					/**< 2841_(97D4 - 97D7h) */
	ULONG	MCLF7;					/**< 2841_(97D8 - 97DBh) */
	ULONG	MCLF8;					/**< 2841_(97DC - 97DFh) */
	ULONG	MCLF9;					/**< 2841_(97E0 - 97E3h) */
	ULONG	MCLF10;					/**< 2841_(97E4 - 97E7h) */
	ULONG	MCLF11;					/**< 2841_(97E8 - 97EBh) */
	ULONG	MCLF12;					/**< 2841_(97EC - 97EFh) */
	ULONG	MCLF13;					/**< 2841_(97F0 - 97F3h) */
	ULONG	MCLF14;					/**< 2841_(97F4 - 97F7h) */
	ULONG	MCLF15;					/**< 2841_(97F8 - 97FBh) */
	ULONG	MCLG1;					/**< 2841_(9800 - 9803h) */
	ULONG	MCLG2;					/**< 2841_(9804 - 9807h) */
	ULONG	MCLG3;					/**< 2841_(9808 - 980Bh) */
	ULONG	MCLG4;					/**< 2841_(980C - 980Fh) */
	ULONG	MCLG5;					/**< 2841_(9810 - 9813h) */
	ULONG	MCLG6;					/**< 2841_(9814 - 9817h) */
	ULONG	MCLG7;					/**< 2841_(9818 - 981Bh) */
	ULONG	MCLG8;					/**< 2841_(981C - 981Fh) */
	ULONG	MCLG9;					/**< 2841_(9820 - 9823h) */
	ULONG	MCLG10;					/**< 2841_(9824 - 9827h) */
	ULONG	MCLG11;					/**< 2841_(9828 - 982Bh) */
	ULONG	MCLG12;					/**< 2841_(982C - 982Fh) */
	ULONG	MCLG13;					/**< 2841_(9830 - 9833h) */
	ULONG	MCLG14;					/**< 2841_(9834 - 9837h) */
	ULONG	MCLG15;					/**< 2841_(9838 - 983Bh) */
	ULONG	MCLH1;					/**< 2841_(9840 - 9843h) */
	ULONG	MCLH2;					/**< 2841_(9844 - 9847h) */
	ULONG	MCLH3;					/**< 2841_(9848 - 984Bh) */
	ULONG	MCLH4;					/**< 2841_(984C - 984Fh) */
	ULONG	MCLH5;					/**< 2841_(9850 - 9853h) */
	ULONG	MCLH6;					/**< 2841_(9854 - 9857h) */
	ULONG	MCLH7;					/**< 2841_(9858 - 985Bh) */
	ULONG	MCLH8;					/**< 2841_(985C - 985Fh) */
	ULONG	MCLH9;					/**< 2841_(9860 - 9863h) */
	ULONG	MCLH10;					/**< 2841_(9864 - 9867h) */
	ULONG	MCLH11;					/**< 2841_(9868 - 986Bh) */
	ULONG	MCLH12;					/**< 2841_(986C - 986Fh) */
	ULONG	MCLH13;					/**< 2841_(9870 - 9873h) */
	ULONG	MCLH14;					/**< 2841_(9874 - 9877h) */
	ULONG	MCLH15;					/**< 2841_(9878 - 987Bh) */
	ULONG	MCLI1;					/**< 2841_(9880 - 9883h) */
	ULONG	MCLI2;					/**< 2841_(9884 - 9887h) */
	ULONG	MCLI3;					/**< 2841_(9888 - 988Bh) */
	ULONG	MCLI4;					/**< 2841_(988C - 98BFh) */
	ULONG	MCLI5;					/**< 2841_(9890 - 9893h) */
	ULONG	MCLI6;					/**< 2841_(9894 - 9897h) */
	ULONG	MCLI7;					/**< 2841_(9898 - 989Bh) */
	ULONG	MCLI8;					/**< 2841_(989C - 989Fh) */
	ULONG	MCLI9;					/**< 2841_(98A0 - 98A3h) */
	ULONG	MCLI10;					/**< 2841_(98A4 - 98A7h) */
	ULONG	MCLI11;					/**< 2841_(98A8 - 98ABh) */
	ULONG	MCLI12;					/**< 2841_(98AC - 98AFh) */
	ULONG	MCLI13;					/**< 2841_(98B0 - 98B3h) */
	ULONG	MCLI14;					/**< 2841_(98B4 - 98B7h) */
	ULONG	MCLI15;					/**< 2841_(98B8 - 98BBh) */
	ULONG	MCLJ1;					/**< 2841_(98C0 - 98C3h) */
	ULONG	MCLJ2;					/**< 2841_(98C4 - 98C7h) */
	ULONG	MCLJ3;					/**< 2841_(98C8 - 98CBh) */
	ULONG	MCLJ4;					/**< 2841_(98CC - 98CFh) */
	ULONG	MCLJ5;					/**< 2841_(98D0 - 98D3h) */
	ULONG	MCLJ6;					/**< 2841_(98D4 - 98D7h) */
	ULONG	MCLJ7;					/**< 2841_(98D8 - 98DBh) */
	ULONG	MCLJ8;					/**< 2841_(98DC - 98DFh) */
	ULONG	MCLJ9;					/**< 2841_(98E0 - 98E3h) */
	ULONG	MCLJ10;					/**< 2841_(98E4 - 98E7h) */
	ULONG	MCLJ11;					/**< 2841_(98E8 - 98EBh) */
	ULONG	MCLJ12;					/**< 2841_(98EC - 98EFh) */
	ULONG	MCLJ13;					/**< 2841_(98F0 - 98F3h) */
	ULONG	MCLJ14;					/**< 2841_(98F4 - 98F7h) */
	ULONG	MCLJ15;					/**< 2841_(98F8 - 98FBh) */
	ULONG	MCLK1;					/**< 2841_(9900 - 9903h) */
	ULONG	MCLK2;					/**< 2841_(9904 - 9907h) */
	ULONG	MCLK3;					/**< 2841_(9908 - 990Bh) */
	ULONG	MCLK4;					/**< 2841_(990C - 990Fh) */
	ULONG	MCLK5;					/**< 2841_(9910 - 9913h) */
	ULONG	MCLK6;					/**< 2841_(9914 - 9917h) */
	ULONG	MCLK7;					/**< 2841_(9918 - 991Bh) */
	ULONG	MCLK8;					/**< 2841_(991C - 991Fh) */
	ULONG	MCLK9;					/**< 2841_(9920 - 9923h) */
	ULONG	MCLK10;					/**< 2841_(9924 - 9927h) */
	ULONG	MCLK11;					/**< 2841_(9928 - 992Bh) */
	ULONG	MCLK12;					/**< 2841_(992C - 992Fh) */
	ULONG	MCLK13;					/**< 2841_(9930 - 9933h) */
	ULONG	MCLK14;					/**< 2841_(9934 - 9937h) */
	ULONG	MCLK15;					/**< 2841_(9938 - 993Bh) */
	ULONG	MCLL1;					/**< 2841_(9940 - 9943h) */
	ULONG	MCLL2;					/**< 2841_(9944 - 9947h) */
	ULONG	MCLL3;					/**< 2841_(9948 - 994Bh) */
	ULONG	MCLL4;					/**< 2841_(994C - 994Fh) */
	ULONG	MCLL5;					/**< 2841_(9950 - 9953h) */
	ULONG	MCLL6;					/**< 2841_(9954 - 9957h) */
	ULONG	MCLL7;					/**< 2841_(9958 - 995Bh) */
	ULONG	MCLL8;					/**< 2841_(995C - 995Fh) */
	ULONG	MCLL9;					/**< 2841_(9960 - 9963h) */
	ULONG	MCLL10;					/**< 2841_(9964 - 9967h) */
	ULONG	MCLL11;					/**< 2841_(9968 - 996Bh) */
	ULONG	MCLL12;					/**< 2841_(996C - 996Fh) */
	ULONG	MCLL13;					/**< 2841_(9970 - 9973h) */
	ULONG	MCLL14;					/**< 2841_(9974 - 9977h) */
	ULONG	MCLL15;					/**< 2841_(9978 - 997Bh) */
	ULONG	MCYCBALP;				/**< 2841_(9980 - 9983h) */
	ULONG	MCYCBGA1;				/**< 2841_(9988 - 998Bh) */
	ULONG	MCYCBGA2;				/**< 2841_(998C - 998Fh) */
	ULONG	MCYCBBD1;				/**< 2841_(9990 - 9993h) */
	ULONG	MCYCBBD2;				/**< 2841_(9994 - 9997h) */
	ULONG	MCBABALP;				/**< 2841_(9998 - 999Bh) */
	ULONG	MCBABOF1;				/**< 2841_(99A0 - 99A3h) */
	ULONG	MCBABOF2;				/**< 2841_(99A4 - 99A7h) */
	ULONG	MCBABGA1;				/**< 2841_(99A8 - 99ABh) */
	ULONG	MCBABGA2;				/**< 2841_(99AC - 99AFh) */
	ULONG	MCBABBD1;				/**< 2841_(99B0 - 99B3h) */
	ULONG	MCBABBD2;				/**< 2841_(99B4 - 99B7h) */
} T_IM_R2Y_CTRL_RDMA_MCYC_ADDR;

/** RGB Offset before TC control register address info (for RDMA)
*/
typedef struct {
	ULONG	TCOF1;					/**< 2841_(A000 - A003h) */
	ULONG	TCOF2;					/**< 2841_(A004 - A007h) */
} T_IM_R2Y_CTRL_RDMA_BTC_TCOF_ADDR;

/** Luminance Evaluation TC control register address info (for RDMA)
*/
typedef struct {
	ULONG	TCTCTL;					/**< 2841_(A008 - A00Bh) */
	ULONG	TCTSTA;					/**< 2841_(A00C - A00Fh) */
	ULONG	TCTB;					/**< 2841_(A010 - A013h) */
} T_IM_R2Y_CTRL_RDMA_BTC_TCT_ADDR;

/** Histogram before TC control register address info (for RDMA)
*/
typedef struct {
	ULONG	TCHSCTL;				/**< 2841_(A014 - A017h) */
	ULONG	TCHSSTA;				/**< 2841_(A018 - A01Bh) */
	ULONG	TCHSSIZ;				/**< 2841_(A01C - A01Fh) */
} T_IM_R2Y_CTRL_RDMA_BTC_TCHS_ADDR;

/** Tone control register address info (for RDMA)
*/
typedef struct {
	ULONG	TCCTL;					/**< 2841_(A040 - A043h) */
	ULONG	TCYC;					/**< 2841_(A044 - A047h) */
	ULONG	TCEP1;					/**< 2841_(A048 - A04Bh) */
	ULONG	TCEP2;					/**< 2841_(A04C - A04Fh) */
	ULONG	TCCLPR;					/**< 2841_(A050 - A053h) */
	ULONG	TCCLPG;					/**< 2841_(A054 - A057h) */
	ULONG	TCCLPB;					/**< 2841_(A058 - A05Bh) */
} T_IM_R2Y_CTRL_RDMA_TONE_ADDR;

/** Gamma control register address info (for RDMA)
*/
typedef struct {
	ULONG	GMCTL;					/**< 2841_(A060 - A063h) */
} T_IM_R2Y_CTRL_RDMA_GAMMA_ADDR;

/** CC1 matrix control register address info (for RDMA)
*/
typedef struct {
	ULONG	CC1CTL;					/**< 2841_(A080 - A083h) */
	ULONG	CC1K1;					/**< 2841_(A0A0 - A0A3h) */
	ULONG	CC1K2;					/**< 2841_(A0A4 - A0A7h) */
	ULONG	CC1K3;					/**< 2841_(A0A8 - A0ABh) */
	ULONG	CC1K4;					/**< 2841_(A0AC - A0AFh) */
	ULONG	CC1K5;					/**< 2841_(A0B0 - A0B3h) */
	ULONG	CC1CLPR;				/**< 2841_(A0C0 - A0C3h) */
	ULONG	CC1CLPG;				/**< 2841_(A0C4 - A0C7h) */
	ULONG	CC1CLPB;				/**< 2841_(A0C8 - A0CBh) */
} T_IM_R2Y_CTRL_RDMA_CC1_ADDR;

/** CC1 matrix coefficient control register address info (for RDMA)
*/
typedef struct {
	ULONG	CC1K1;					/**< 2841_(A0A0 - A0A3h) */
	ULONG	CC1K2;					/**< 2841_(A0A4 - A0A7h) */
	ULONG	CC1K3;					/**< 2841_(A0A8 - A0ABh) */
	ULONG	CC1K4;					/**< 2841_(A0AC - A0AFh) */
	ULONG	CC1K5;					/**< 2841_(A0B0 - A0B3h) */
} T_IM_R2Y_CTRL_RDMA_CC1_COEF_ADDR;

/** Yc convert control register address info (for RDMA)
*/
typedef struct {
	ULONG	YC1;					/**< 2841_(A100 - A103h) */
	ULONG	YC2;					/**< 2841_(A104 - A107h) */
	ULONG	YC3;					/**< 2841_(A108 - A10Bh) */
	ULONG	YC4;					/**< 2841_(A10C - A110h) */
	ULONG	YC5;					/**< 2841_(A110 - A113h) */
	ULONG	YBLEND;					/**< 2841_(A120 - A123h) */
} T_IM_R2Y_CTRL_RDMA_YCC_ADDR;

/** YNR control register address info (for RDMA)
*/
typedef struct {
	ULONG	NRCTL;					/**< 2841_(A140 - A143h) */
	ULONG	NROF1;					/**< 2841_(A148 - A14Bh) */
	ULONG	NROF2;					/**< 2841_(A14C - A14Fh) */
	ULONG	NRGA1;					/**< 2841_(A150 - A153h) */
	ULONG	NRGA2;					/**< 2841_(A154 - A157h) */
	ULONG	NRBD1;					/**< 2841_(A158 - A15Bh) */
	ULONG	NRBD2;					/**< 2841_(A15C - A15Fh) */
} T_IM_R2Y_CTRL_RDMA_YNR_ADDR;

/** Edge enhancement noise reduction control register address info (for RDMA)
*/
typedef struct {
	ULONG	EGSMCTL;				/**< 2841_(A180 - A183h) */
	ULONG	EGSMTT;					/**< 2841_(A184 - A187h) */
} T_IM_R2Y_CTRL_RDMA_EENR_ADDR;

/** High frequency edge enhancement control register address info (for RDMA)
*/
typedef struct {
	ULONG	EGHWCTL;				/**< 2841_(A200 - A203h) */
	ULONG	EGHWHPFK1;				/**< 2841_(A208 - A20Bh) */
	ULONG	EGHWHPFK2;				/**< 2841_(A20C - A20Fh) */
	ULONG	EGHWCORPOF1;			/**< 2841_(A210 - A213h) */
	ULONG	EGHWCORPOF2;			/**< 2841_(A214 - A217h) */
	ULONG	EGHWCORPGA;				/**< 2841_(A218 - A21Bh) */
	ULONG	EGHWCORPBD1;			/**< 2841_(A220 - A223h) */
	ULONG	EGHWCORPBD2;			/**< 2841_(A224 - A227h) */
	ULONG	EGHWCORMOF1;			/**< 2841_(A228 - A22Bh) */
	ULONG	EGHWCORMOF2;			/**< 2841_(A22C - A22Fh) */
	ULONG	EGHWCORMGA;				/**< 2841_(A230 - A233h) */
	ULONG	EGHWCORMBD1;			/**< 2841_(A238 - A23Bh) */
	ULONG	EGHWCORMBD2;			/**< 2841_(A23C - A23Fh) */
	ULONG	EGHWSCLSUP;				/**< 2841_(A240 - A243h) */
	ULONG	EGHWCLPPOF1;			/**< 2841_(A248 - A24Bh) */
	ULONG	EGHWCLPPOF2;			/**< 2841_(A24C - A24Fh) */
	ULONG	EGHWCLPPGA;				/**< 2841_(A250 - A253h) */
	ULONG	EGHWCLPPBD;				/**< 2841_(A254 - A257h) */
	ULONG	EGHWCLPMOF1;			/**< 2841_(A258 - A25Bh) */
	ULONG	EGHWCLPMOF2;			/**< 2841_(A25C - A25Fh) */
	ULONG	EGHWCLPMGA;				/**< 2841_(A260 - A263h) */
	ULONG	EGHWCLPMBD;				/**< 2841_(A264 - A267h) */
} T_IM_R2Y_CTRL_RDMA_EGHW_ADDR;

/** Medium frequency edge enhancement control register address info (for RDMA)
*/
typedef struct {
	ULONG	EGMWCTL;				/**< 2841_(A280 - A283h) */
	ULONG	EGMWHPFK1;				/**< 2841_(A288 - A28Bh) */
	ULONG	EGMWHPFK2;				/**< 2841_(A28C - A28Fh) */
	ULONG	EGMWCORPOF1;			/**< 2841_(A290 - A293h) */
	ULONG	EGMWCORPOF2;			/**< 2841_(A294 - A297h) */
	ULONG	EGMWCORPGA;				/**< 2841_(A298 - A29Bh) */
	ULONG	EGMWCORPBD1;			/**< 2841_(A2A0 - A2A3h) */
	ULONG	EGMWCORPBD2;			/**< 2841_(A2A4 - A2A7h) */
	ULONG	EGMWCORMOF1;			/**< 2841_(A2A8 - A2ABh) */
	ULONG	EGMWCORMOF2;			/**< 2841_(A2AC - A2AFh) */
	ULONG	EGMWCORMGA;				/**< 2841_(A2B0 - A2B3h) */
	ULONG	EGMWCORMBD1;			/**< 2841_(A2B8 - A2BBh) */
	ULONG	EGMWCORMBD2;			/**< 2841_(A2BC - A2BFh) */
	ULONG	EGMWSCLSUP;				/**< 2841_(A2C0 - A2C3h) */
	ULONG	EGMWCLPPOF1;			/**< 2841_(A2C8 - A2CBh) */
	ULONG	EGMWCLPPOF2;			/**< 2841_(A2CC - A2CFh) */
	ULONG	EGMWCLPPGA;				/**< 2841_(A2D0 - A2D3h) */
	ULONG	EGMWCLPPBD;				/**< 2841_(A2D4 - A2D7h) */
	ULONG	EGMWCLPMOF1;			/**< 2841_(A2D8 - A2DBh) */
	ULONG	EGMWCLPMOF2;			/**< 2841_(A2DC - A2DFh) */
	ULONG	EGMWCLPMGA;				/**< 2841_(A2E0 - A2E3h) */
	ULONG	EGMWCLPMBD;				/**< 2841_(A2E4 - A2E7h) */
} T_IM_R2Y_CTRL_RDMA_EGMW_ADDR;

/** Low frequency edge enhancement control register address info (for RDMA)
*/
typedef struct {
	ULONG	EGLWCTL;				/**< 2841_(A300 - A303h) */
	ULONG	EGLWHPFK1;				/**< 2841_(A310 - A313h) */
	ULONG	EGLWHPFK2;				/**< 2841_(A314 - A317h) */
	ULONG	EGLWHPFK3;				/**< 2841_(A318 - A31Bh) */
	ULONG	EGLWCORPOF1;			/**< 2841_(A320 - A323h) */
	ULONG	EGLWCORPOF2;			/**< 2841_(A324 - A327h) */
	ULONG	EGLWCORPGA;				/**< 2841_(A328 - A32Bh) */
	ULONG	EGLWCORPBD1;			/**< 2841_(A330 - A333h) */
	ULONG	EGLWCORPBD2;			/**< 2841_(A334 - A337h) */
	ULONG	EGLWCORMOF1;			/**< 2841_(A338 - A33Bh) */
	ULONG	EGLWCORMOF2;			/**< 2841_(A33C - A33Fh) */
	ULONG	EGLWCORMGA;				/**< 2841_(A340 - A343h) */
	ULONG	EGLWCORMBD1;			/**< 2841_(A348 - A34Bh) */
	ULONG	EGLWCORMBD2;			/**< 2841_(A34C - A34Fh) */
	ULONG	EGLWSCLSUP;				/**< 2841_(A350 - A353h) */
	ULONG	EGLWCLPPOF1;			/**< 2841_(A358 - A35Bh) */
	ULONG	EGLWCLPPOF2;			/**< 2841_(A35C - A35Fh) */
	ULONG	EGLWCLPPGA;				/**< 2841_(A360 - A363h) */
	ULONG	EGLWCLPPBD;				/**< 2841_(A364 - A367h) */
	ULONG	EGLWCLPMOF1;			/**< 2841_(A368 - A36Bh) */
	ULONG	EGLWCLPMOF2;			/**< 2841_(A36C - A36Fh) */
	ULONG	EGLWCLPMGA;				/**< 2841_(A370 - A373h) */
	ULONG	EGLWCLPMBD;				/**< 2841_(A374 - A377h) */
} T_IM_R2Y_CTRL_RDMA_EGLW_ADDR;

/** Edge dot noise correction control register address info (for RDMA)
*/
typedef struct {
	ULONG	EGDTCTL;				/**< 2841_(A380 - A383h) */
	ULONG	EGDTHWTH;				/**< 2841_(A384 - A387h) */
	ULONG	EGDTHWK;				/**< 2841_(A388 - A38Bh) */
	ULONG	EGDTMWTH;				/**< 2841_(A38C - A38Fh) */
	ULONG	EGDTMWK;				/**< 2841_(A390 - A393h) */
} T_IM_R2Y_CTRL_RDMA_EGDT_ADDR;

/** Edge emphasis blend control register address info (for RDMA)
*/
typedef struct {
	ULONG	EGCMPCTL;				/**< 2841_(A3C0 - A3C3h) */
	ULONG	EGCMPBD1;				/**< 2841_(A3C8 - A3CBh) */
	ULONG	EGCMPBD2;				/**< 2841_(A3CC - A3CFh) */
	ULONG	EGCMPALPOF;				/**< 2841_(A3D0 - A3D3h) */
	ULONG	EGCMPALPGA1;			/**< 2841_(A3D8 - A3DBh) */
	ULONG	EGCMPALPGA2;			/**< 2841_(A3DC - A3DFh) */
	ULONG	EGCMPBTAOF;				/**< 2841_(A3E0 - A3E3h) */
	ULONG	EGCMPBTAGA1;			/**< 2841_(A3E8 - A3EBh) */
	ULONG	EGCMPBTAGA2;			/**< 2841_(A3EC - A3EFh) */
	ULONG	EGCMPCLP;				/**< 2841_(A3F0 - A3F3h) */
} T_IM_R2Y_CTRL_RDMA_EGCMP_ADDR;

/** Chroma referenced edge and texture adjustment control register address info (for RDMA)
*/
typedef struct {
	ULONG	CRVAF;					/**< 2841_(A400 - A403h) */
	ULONG	CRVBF;					/**< 2841_(A404 - A407h) */
} T_IM_R2Y_CTRL_RDMA_CRV_ADDR;

/** Chroma referenced edge adjustment control register address info (for RDMA)
*/
typedef struct {
	ULONG	EGCRVCTL;				/**< 2841_(A440 - A443h) */
	ULONG	EGCRVASCLOF;			/**< 2841_(A444 - A447h) */
	ULONG	EGCRVASCLGA;			/**< 2841_(A448 - A44Bh) */
	ULONG	EGCRVASCLBD;			/**< 2841_(A44C - A44Fh) */
	ULONG	EGCRVASCLCP;			/**< 2841_(A450 - A453h) */
	ULONG	EGYASCLGA;				/**< 2841_(A454 - A457h) */
	ULONG	EGYASCLBD;				/**< 2841_(A458 - A45Bh) */
	ULONG	EGYASCLCLP;				/**< 2841_(A45C - A45Fh) */
	ULONG	EGCRVBSCLOF;			/**< 2841_(A460 - A463h) */
	ULONG	EGCRVBSCLGA;			/**< 2841_(A464 - A467h) */
	ULONG	EGCRVBSCLBD;			/**< 2841_(A468 - A46Bh) */
	ULONG	EGCRVBSCLCP;			/**< 2841_(A46C - A46Fh) */
	ULONG	EGYBSCLGA;				/**< 2841_(A470 - A473h) */
	ULONG	EGYBSCLBD;				/**< 2841_(A474 - A477h) */
	ULONG	EGYBSCLCLP;				/**< 2841_(A478 - A47Bh) */
	ULONG	EGCRVCLP;				/**< 2841_(A47C - A47Fh) */
} T_IM_R2Y_CTRL_RDMA_EGCRV_ADDR;

/** Chroma referenced luminance blend control register address info (for RDMA)
*/
typedef struct {
	ULONG	YBCRVCTL;				/**< 2841_(A480 - A483h) */
	ULONG	YBCRVAALPOF;			/**< 2841_(A484 - A487h) */
	ULONG	YBCRVAALPGA;			/**< 2841_(A488 - A48Bh) */
	ULONG	YBCRVAALPBD;			/**< 2841_(A48C - A48Fh) */
	ULONG	YBCRVAALPCP;			/**< 2841_(A490 - A493h) */
	ULONG	YBYAALPGA;				/**< 2841_(A494 - A497h) */
	ULONG	YBYAALPBD;				/**< 2841_(A498 - A49Bh) */
	ULONG	YBYAALPCLP;				/**< 2841_(A49C - A49Fh) */
	ULONG	YBCRVBALPOF;			/**< 2841_(A4A0 - A4A3h) */
	ULONG	YBCRVBALPGA;			/**< 2841_(A4A4 - A4A7h) */
	ULONG	YBCRVBALPBD;			/**< 2841_(A4A8 - A4ABh) */
	ULONG	YBCRVBALPCP;			/**< 2841_(A4AC - A4AFh) */
	ULONG	YBYBALPGA;				/**< 2841_(A4B0 - A4B3h) */
	ULONG	YBYBALPBD;				/**< 2841_(A4B4 - A4B7h) */
	ULONG	YBYBALPCLP;				/**< 2841_(A4B8 - A4BBh) */
} T_IM_R2Y_CTRL_RDMA_YBCRV_ADDR;

/** Color Noise Reduction(LPF) control register address info (for RDMA)
*/
typedef struct {
	ULONG	CLPFCTL;				/**< 2841_(A500 - A503h) */
	ULONG	CLPFK1;					/**< 2841_(A520 - A523h) */
	ULONG	CLPFK2;					/**< 2841_(A524 - A527h) */
	ULONG	CLPFK3;					/**< 2841_(A528 - A52Bh) */
	ULONG	CLPFK4;					/**< 2841_(A52C - A52Fh) */
	ULONG	CLPFK5;					/**< 2841_(A530 - A533h) */
	ULONG	CLPFOF;					/**< 2841_(A540 - A543h) */
	ULONG	CLPFGA1;				/**< 2841_(A548 - A54Bh) */
	ULONG	CLPFGA2;				/**< 2841_(A54C - A54Fh) */
	ULONG	CLPFBD1;				/**< 2841_(A550 - A553h) */
	ULONG	CLPFBD2;				/**< 2841_(A554 - A557h) */
} T_IM_R2Y_CTRL_RDMA_CLPF_ADDR;

/** Chroma suppress control register address info (for RDMA)
*/
typedef struct {
	ULONG	CSYCTL;					/**< 2841_(A580 - A583h) */
	ULONG	CSYOF1;					/**< 2841_(A588 - A58Bh) */
	ULONG	CSYOF2;					/**< 2841_(A58C - A58Fh) */
	ULONG	CSYGA1;					/**< 2841_(A590 - A593h) */
	ULONG	CSYGA2;					/**< 2841_(A594 - A597h) */
	ULONG	CSYBD1;					/**< 2841_(A598 - A59Bh) */
	ULONG	CSYBD2;					/**< 2841_(A59C - A59Fh) */
	ULONG	YCRVFX;					/**< 2841_(A5A0 - A5A3h) */
	ULONG	CFIX;					/**< 2841_(A5A4 - A5A7h) */
	ULONG	YCOF1;					/**< 2841_(A5A8 - A5ABh) */
	ULONG	YCOF2;					/**< 2841_(A5AC - A5AFh) */
} T_IM_R2Y_CTRL_RDMA_CSP_ADDR;

/** Tone control Table register address info (for RDMA)
*/
typedef struct {
	ULONG	TC[512];				/**< 294A_(0000 - 07FFh) */
} T_IM_R2Y_CTRL_RDMA_TONE_TBL_ADDR;

/** Gamma Table(Diff) register address info (for RDMA)
*/
typedef struct {
	ULONG	GMDF[512];				/**< 294A_(8000 - 87FFh) */
									/**< 294A_(8800 - 8FFFh) */
									/**< 294A_(9000 - 97FFh) */
									/**< 294A_(9800 - 9FFFh) */
									/**< 294A_(A000 - A7FFh) */
} T_IM_R2Y_CTRL_RDMA_GMDF_TBL_ADDR;

/** Gamma Table(Full) register address info (for RDMA)
*/
typedef struct {
	ULONG	GMFL[128];				/**< 294A_(A800 - A9FFh) */
									/**< 294A_(AA00 - ABFFh) */
									/**< 294A_(AC00 - ADFFh) */
									/**< 294A_(AE00 - AFFFh) */
									/**< 294A_(B000 - B1FFh) */
} T_IM_R2Y_CTRL_RDMA_GMFL_TBL_ADDR;

/** (High/Medium/Low) Edge emphasis scaling Table register address info (for RDMA)
*/
typedef struct {
	ULONG	EGWSCL[128];			/**< 294A_(C000 - C1FFh) */
									/**< 294A_(C400 - C5FFh) */
									/**< 294A_(C800 - C9FFh) */
} T_IM_R2Y_CTRL_RDMA_EGWSCL_TBL_ADDR;

/** (High/Medium/Low) Edge emphasis stepping Table register address info (for RDMA)
*/
typedef struct {
	ULONG	EGWTON[128];			/**< 294A_(C200 - C3FFh) */
									/**< 294A_(C600 - C7FFh) */
									/**< 294A_(CA00 - CBFFh) */
} T_IM_R2Y_CTRL_RDMA_EGWTON_TBL_ADDR;

/** Map Scale Table register address info (for RDMA)
*/
typedef struct {
	ULONG	EGMPSCL[111];			/**< 294A_(CC00 - CDBBh) */
} T_IM_R2Y_CTRL_RDMA_EGMPSCL_TBL_ADDR;



/** Post-resize edge enhancement0 control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_ee0ctl		EE0CTL;			/**< 2841_(C400 - C403h) */
	union	io_r2y_ee0hpfk		EE0HPFK;		/**< 2841_(C408 - C40Fh) */
	union	io_r2y_ee0nrlv		EE0NRLV;		/**< 2841_(C410 - C413h) */
	union	io_r2y_ee0corpof	EE0CORPOF;		/**< 2841_(C414 - C417h) */
	union	io_r2y_ee0corpga	EE0CORPGA;		/**< 2841_(C418 - C41Bh) */
	union	io_r2y_ee0corpbd	EE0CORPBD;		/**< 2841_(C41C - C41Fh) */
	union	io_r2y_ee0cormof	EE0CORMOF;		/**< 2841_(C420 - C423h) */
	union	io_r2y_ee0cormga	EE0CORMGA;		/**< 2841_(C424 - C427h) */
	union	io_r2y_ee0cormbd	EE0CORMBD;		/**< 2841_(C428 - C42Bh) */
	union	io_r2y_ee0sclsup	EE0SCLSUP;		/**< 2841_(C42C - C42Fh) */
	union	io_r2y_ee0sclpof	EE0SCLPOF;		/**< 2841_(C430 - C43Bh) */
	union	io_r2y_ee0sclpga	EE0SCLPGA;		/**< 2841_(C440 - C453h) */
	union	io_r2y_ee0sclpbd	EE0SCLPBD;		/**< 2841_(C460 - C46Bh) */
	union	io_r2y_ee0sclmof	EE0SCLMOF;		/**< 2841_(C470 - C47Bh) */
	union	io_r2y_ee0sclmga	EE0SCLMGA;		/**< 2841_(C480 - C493h) */
	union	io_r2y_ee0sclmbd	EE0SCLMBD;		/**< 2841_(C4A0 - C4ABh) */
	union	io_r2y_ee0tonpof	EE0TONPOF;		/**< 2841_(C4B0 - C4B7h) */
	union	io_r2y_ee0tonpga	EE0TONPGA;		/**< 2841_(C4C0 - C4CBh) */
	union	io_r2y_ee0tonpbd	EE0TONPBD;		/**< 2841_(C4D0 - C4D3h) */
	union	io_r2y_ee0tonmof	EE0TONMOF;		/**< 2841_(C4D8 - C4DFh) */
	union	io_r2y_ee0tonmga	EE0TONMGA;		/**< 2841_(C4E0 - C4EBh) */
	union	io_r2y_ee0tonmbd	EE0TONMBD;		/**< 2841_(C4F0 - C4F3h) */
	union	io_r2y_ee0clppof	EE0CLPPOF;		/**< 2841_(C4F4 - C4F7h) */
	union	io_r2y_ee0clppga	EE0CLPPGA;		/**< 2841_(C4F8 - C4FBh) */
	union	io_r2y_ee0clppbd	EE0CLPPBD;		/**< 2841_(C4FC - C4FFh) */
	union	io_r2y_ee0clpmof	EE0CLPMOF;		/**< 2841_(C500 - C503h) */
	union	io_r2y_ee0clpmga	EE0CLPMGA;		/**< 2841_(C504 - C507h) */
	union	io_r2y_ee0clpmbd	EE0CLPMBD;		/**< 2841_(C508 - C50Bh) */
} T_IM_R2Y_CTRL_RDMA_EE0_VAL;

/** Post-resize edge enhancement1 control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_ee1ctl		EE1CTL;			/**< 2841_(C600 - C603h) */
	union	io_r2y_ee1hpfk		EE1HPFK;		/**< 2841_(C608 - C60Fh) */
	union	io_r2y_ee1nrlv		EE1NRLV;		/**< 2841_(C610 - C613h) */
	union	io_r2y_ee1corpof	EE1CORPOF;		/**< 2841_(C618 - C61Fh) */
	union	io_r2y_ee1corpga	EE1CORPGA;		/**< 2841_(C620 - C623h) */
	union	io_r2y_ee1corpbd	EE1CORPBD;		/**< 2841_(C628 - C62Fh) */
	union	io_r2y_ee1cormof	EE1CORMOF;		/**< 2841_(C630 - C637h) */
	union	io_r2y_ee1cormga	EE1CORMGA;		/**< 2841_(C638 - C63Bh) */
	union	io_r2y_ee1cormbd	EE1CORMBD;		/**< 2841_(C640 - C647h) */
	union	io_r2y_ee1sclsup	EE1SCLSUP;		/**< 2841_(C648 - C64Bh) */
	union	io_r2y_ee1sclpof	EE1SCLPOF;		/**< 2841_(C650 - C65Bh) */
	union	io_r2y_ee1sclpga	EE1SCLPGA;		/**< 2841_(C660 - C673h) */
	union	io_r2y_ee1sclpbd	EE1SCLPBD;		/**< 2841_(C680 - C693h) */
	union	io_r2y_ee1sclmof	EE1SCLMOF;		/**< 2841_(C6A0 - C6ABh) */
	union	io_r2y_ee1sclmga	EE1SCLMGA;		/**< 2841_(C6C0 - C6D3h) */
	union	io_r2y_ee1sclmbd	EE1SCLMBD;		/**< 2841_(C6E0 - C6F3h) */
	union	io_r2y_ee1tonpof	EE1TONPOF;		/**< 2841_(C700 - C70Bh) */
	union	io_r2y_ee1tonpga	EE1TONPGA;		/**< 2841_(C710 - C71Bh) */
	union	io_r2y_ee1tonpbd	EE1TONPBD;		/**< 2841_(C720 - C727h) */
	union	io_r2y_ee1tonmof	EE1TONMOF;		/**< 2841_(C730 - C73Bh) */
	union	io_r2y_ee1tonmga	EE1TONMGA;		/**< 2841_(C740 - C74Bh) */
	union	io_r2y_ee1tonmbd	EE1TONMBD;		/**< 2841_(C750 - C757h) */
	union	io_r2y_ee1clppof	EE1CLPPOF;		/**< 2841_(C758 - C75Fh) */
	union	io_r2y_ee1clppga	EE1CLPPGA;		/**< 2841_(C760 - C763h) */
	union	io_r2y_ee1clppbd	EE1CLPPBD;		/**< 2841_(C764 - C767h) */
	union	io_r2y_ee1clpmof	EE1CLPMOF;		/**< 2841_(C768 - C76Fh) */
	union	io_r2y_ee1clpmga	EE1CLPMGA;		/**< 2841_(C770 - C773h) */
	union	io_r2y_ee1clpmbd	EE1CLPMBD;		/**< 2841_(C774 - C777h) */
} T_IM_R2Y_CTRL_RDMA_EE1_VAL;

/** Deknee Table(R/G/B) register value info (for RDMA)
*/
typedef union {
	union	io_r2y_sram_dkntbl	DKNTBLR;	/**< 294A_(E000 - E1FFh) */
	union	io_r2y_sram_dkntbl	DKNTBLG;	/**< 294A_(E200 - E3FFh) */
	union	io_r2y_sram_dkntbl	DKNTBLB;	/**< 294A_(E400 - E5FFh) */
} U_IM_R2Y_CTRL_RDMA_DEKNEE_TBL_VAL;

/** Offset control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_ofst			OFST;			/**< 2841_(8020 - 8027h) */
} T_IM_R2Y_CTRL_RDMA_OFST_VAL;

/** WB clip control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_wblv			WBLV;			/**< 2841_(8030 - 8037h) */
} T_IM_R2Y_CTRL_RDMA_WB_CLIP_VAL;

/** CC0 matrix control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_cc0ctl		CC0CTL;			/**< 2841_(8080 - 8083h) */
	union	io_r2y_cc0k			CC0K;			/**< 2841_(80A0 - 80B3h) */
	union	io_r2y_cc0ybof		CC0YBOF;		/**< 2841_(80C0 - 80C7h) */
	union	io_r2y_cc0ybga		CC0YBGA;		/**< 2841_(80C8 - 80CFh) */
	union	io_r2y_cc0ybbd		CC0YBBD;		/**< 2841_(80D0 - 80D3h) */
	union	io_r2y_ccyc			CCYC;			/**< 2841_(80D4 - 80D7h) */
} T_IM_R2Y_CTRL_RDMA_CC0_VAL;

/** CC0 matrix coefficient control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_cc0k			CC0K;			/**< 2841_(80A0 - 80B3h) */
} T_IM_R2Y_CTRL_RDMA_CC0_COEF_VAL;

/** Multi Axis control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_mcyc			MCYC;			/**< 2841_(9000 - 9013h) */
	union	io_r2y_mcb1ab		MCB1AB;			/**< 2841_(9020 - 9023h) */
	union	io_r2y_mcb1cd		MCB1CD;			/**< 2841_(9024 - 9027h) */
	union	io_r2y_mcb2ab		MCB2AB;			/**< 2841_(9028 - 902Bh) */
	union	io_r2y_mcb2cd		MCB2CD;			/**< 2841_(902C - 902Fh) */
	union	io_r2y_mcb3ab		MCB3AB;			/**< 2841_(9030 - 9033h) */
	union	io_r2y_mcb3cd		MCB3CD;			/**< 2841_(9034 - 9037h) */
	union	io_r2y_mcb4ab		MCB4AB;			/**< 2841_(9038 - 903Bh) */
	union	io_r2y_mcb4cd		MCB4CD;			/**< 2841_(903C - 903Fh) */
	union	io_r2y_mcid1		MCID1;			/**< 2841_(9040 - 9043h) */
	union	io_r2y_mcid2		MCID2;			/**< 2841_(9044 - 9047h) */
	union	io_r2y_mcid3		MCID3;			/**< 2841_(9048 - 904Bh) */
	union	io_r2y_mcid4		MCID4;			/**< 2841_(904C - 904Fh) */
	struct io_r2y_mck			MCKA;			/**< 2841_(9080 - 90E3h) */
	struct io_r2y_mck			MCKB;			/**< 2841_(9100 - 9163h) */
	struct io_r2y_mck			MCKC;			/**< 2841_(9180 - 91E3h) */
	struct io_r2y_mck			MCKD;			/**< 2841_(9200 - 9263h) */
	struct io_r2y_mck			MCKE;			/**< 2841_(9280 - 92E3h) */
	struct io_r2y_mck			MCKF;			/**< 2841_(9300 - 9363h) */
	struct io_r2y_mck			MCKG;			/**< 2841_(9380 - 93E3h) */
	struct io_r2y_mck			MCKH;			/**< 2841_(9400 - 9463h) */
	struct io_r2y_mck			MCKI;			/**< 2841_(9480 - 94E3h) */
	struct io_r2y_mck			MCKJ;			/**< 2841_(9500 - 9563h) */
	struct io_r2y_mck			MCKK;			/**< 2841_(9580 - 95E3h) */
	struct io_r2y_mck			MCKL;			/**< 2841_(9600 - 9663h) */
	struct io_r2y_mcl			MCLA;			/**< 2841_(9680 - 96BBh) */
	struct io_r2y_mcl			MCLB;			/**< 2841_(96C0 - 96FBh) */
	struct io_r2y_mcl			MCLC;			/**< 2841_(9700 - 973Bh) */
	struct io_r2y_mcl			MCLD;			/**< 2841_(9740 - 977Bh) */
	struct io_r2y_mcl			MCLE;			/**< 2841_(9780 - 97BBh) */
	struct io_r2y_mcl			MCLF;			/**< 2841_(97C0 - 97FBh) */
	struct io_r2y_mcl			MCLG;			/**< 2841_(9800 - 983Bh) */
	struct io_r2y_mcl			MCLH;			/**< 2841_(9840 - 987Bh) */
	struct io_r2y_mcl			MCLI;			/**< 2841_(9880 - 98BBh) */
	struct io_r2y_mcl			MCLJ;			/**< 2841_(98C0 - 98FBh) */
	struct io_r2y_mcl			MCLK;			/**< 2841_(9900 - 993Bh) */
	struct io_r2y_mcl			MCLL;			/**< 2841_(9940 - 997Bh) */
	union	io_r2y_mcycbalp		MCYCBALP;		/**< 2841_(9980 - 9983h) */
	union	io_r2y_mcycbga		MCYCBGA;		/**< 2841_(9988 - 998Fh) */
	union	io_r2y_mcycbbd		MCYCBBD;		/**< 2841_(9990 - 9997h) */
	union	io_r2y_mcbabalp		MCBABALP;		/**< 2841_(9998 - 999Bh) */
	union	io_r2y_mcbabof		MCBABOF;		/**< 2841_(99A0 - 99A7h) */
	union	io_r2y_mcbabga		MCBABGA;		/**< 2841_(99A8 - 99AFh) */
	union	io_r2y_mcbabbd		MCBABBD;		/**< 2841_(99B0 - 99B7h) */
} T_IM_R2Y_CTRL_RDMA_MCYC_VAL;

/** RGB Offset before TC control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_tcof			TCOF;			/**< 2841_(A000 - A007h) */
} T_IM_R2Y_CTRL_RDMA_BTC_TCOF_VAL;

/** Luminance Evaluation before TC control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_tctctl		TCTCTL;			/**< 2841_(A008 - A00Bh) */
	union	io_r2y_tctsta		TCTSTA;			/**< 2841_(A00C - A00Fh) */
	union	io_r2y_tctb			TCTB;			/**< 2841_(A010 - A013h) */
} T_IM_R2Y_CTRL_RDMA_BTC_TCT_VAL;

/** Histogram before TC control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_tchsctl		TCHSCTL;		/**< 2841_(A014 - A017h) */
	union	io_r2y_tchssta		TCHSSTA;		/**< 2841_(A018 - A01Bh) */
	union	io_r2y_tchssiz		TCHSSIZ;		/**< 2841_(A01C - A01Fh) */
	union	io_r2y_tchov		TCHOV;			/**< 2841_(A020 - A023h) */
} T_IM_R2Y_CTRL_RDMA_BTC_TCHS_VAL;

/** Tone control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_tcctl		TCCTL;			/**< 2841_(A040 - A043h) */
	union	io_r2y_tcyc			TCYC;			/**< 2841_(A044 - A047h) */
	union	io_r2y_tcep			TCEP;			/**< 2841_(A048 - A04Fh) */
	union	io_r2y_tcclpr		TCCLPR;			/**< 2841_(A050 - A053h) */
	union	io_r2y_tcclpg		TCCLPG;			/**< 2841_(A054 - A057h) */
	union	io_r2y_tcclpb		TCCLPB;			/**< 2841_(A058 - A05Bh) */
} T_IM_R2Y_CTRL_RDMA_TONE_VAL;

/** Gamma control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_gmctl		GMCTL;			/**< 2841_(A060 - A063h) */
} T_IM_R2Y_CTRL_RDMA_GAMMA_VAL;

/** CC1 matrix control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_cc1ctl		CC1CTL;			/**< 2841_(A080 - A083h) */
	union	io_r2y_cc1k			CC1K;			/**< 2841_(A0A0 - A0B3h) */
	union	io_r2y_cc1clpr		CC1CLPR;		/**< 2841_(A0C0 - A0C3h) */
	union	io_r2y_cc1clpg		CC1CLPG;		/**< 2841_(A0C4 - A0C7h) */
	union	io_r2y_cc1clpb		CC1CLPB;		/**< 2841_(A0C8 - A0CBh) */
} T_IM_R2Y_CTRL_RDMA_CC1_VAL;

/** CC1 matrix coefficient control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_cc1k			CC1K;			/**< 2841_(A0A0 - A0B3h) */
} T_IM_R2Y_CTRL_RDMA_CC1_COEF_VAL;

/** Yc convert control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_yc			YC;				/**< 2841_(A100 - A113h) */
	union	io_r2y_yblend		YBLEND;			/**< 2841_(A120 - A123h) */
} T_IM_R2Y_CTRL_RDMA_YCC_VAL;

/** YNR control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_nrctl		NRCTL;			/**< 2841_(A140 - A143h) */
	union	io_r2y_nrof			NROF;			/**< 2841_(A148 - A14Fh) */
	union	io_r2y_nrga			NRGA;			/**< 2841_(A150 - A157h) */
	union	io_r2y_nrbd			NRBD;			/**< 2841_(A158 - A15Fh) */
} T_IM_R2Y_CTRL_RDMA_YNR_VAL;

/** Edge enhancement noise reduction control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_egsmctl		EGSMCTL;		/**< 2841_(A180 - A183h) */
	union	io_r2y_egsmtt		EGSMTT;			/**< 2841_(A184 - A187h) */
} T_IM_R2Y_CTRL_RDMA_EENR_VAL;

/** High frequency edge enhancement control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_eghwctl		EGHWCTL;		/**< 2841_(A200 - A203h) */
	union	io_r2y_eghwhpfk		EGHWHPFK;		/**< 2841_(A208 - A20Fh) */
	union	io_r2y_eghwcorpof	EGHWCORPOF;		/**< 2841_(A210 - A217h) */
	union	io_r2y_eghwcorpga	EGHWCORPGA;		/**< 2841_(A218 - A21Bh) */
	union	io_r2y_eghwcorpbd	EGHWCORPBD;		/**< 2841_(A220 - A227h) */
	union	io_r2y_eghwcormof	EGHWCORMOF;		/**< 2841_(A228 - A22Fh) */
	union	io_r2y_eghwcormga	EGHWCORMGA;		/**< 2841_(A230 - A233h) */
	union	io_r2y_eghwcormbd	EGHWCORMBD;		/**< 2841_(A238 - A23Fh) */
	union	io_r2y_eghwsclsup	EGHWSCLSUP;		/**< 2841_(A240 - A243h) */
	union	io_r2y_eghwclppof	EGHWCLPPOF;		/**< 2841_(A248 - A24Fh) */
	union	io_r2y_eghwclppga	EGHWCLPPGA;		/**< 2841_(A250 - A253h) */
	union	io_r2y_eghwclppbd	EGHWCLPPBD;		/**< 2841_(A254 - A257h) */
	union	io_r2y_eghwclpmof	EGHWCLPMOF;		/**< 2841_(A258 - A25Fh) */
	union	io_r2y_eghwclpmga	EGHWCLPMGA;		/**< 2841_(A260 - A263h) */
	union	io_r2y_eghwclpmbd	EGHWCLPMBD;		/**< 2841_(A264 - A267h) */
} T_IM_R2Y_CTRL_RDMA_EGHW_VAL;

/** Medium frequency edge enhancement control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_egmwctl		EGMWCTL;		/**< 2841_(A280 - A283h) */
	union	io_r2y_egmwhpfk		EGMWHPFK;		/**< 2841_(A288 - A28Fh) */
	union	io_r2y_egmwcorpof	EGMWCORPOF;		/**< 2841_(A290 - A297h) */
	union	io_r2y_egmwcorpga	EGMWCORPGA;		/**< 2841_(A298 - A29Bh) */
	union	io_r2y_egmwcorpbd	EGMWCORPBD;		/**< 2841_(A2A0 - A2A7h) */
	union	io_r2y_egmwcormof	EGMWCORMOF;		/**< 2841_(A2A8 - A2AFh) */
	union	io_r2y_egmwcormga	EGMWCORMGA;		/**< 2841_(A2B0 - A2B3h) */
	union	io_r2y_egmwcormbd	EGMWCORMBD;		/**< 2841_(A2B8 - A2BFh) */
	union	io_r2y_egmwsclsup	EGMWSCLSUP;		/**< 2841_(A2C0 - A2C3h) */
	union	io_r2y_egmwclppof	EGMWCLPPOF;		/**< 2841_(A2C8 - A2CFh) */
	union	io_r2y_egmwclppga	EGMWCLPPGA;		/**< 2841_(A2D0 - A2D3h) */
	union	io_r2y_egmwclppbd	EGMWCLPPBD;		/**< 2841_(A2D4 - A2D7h) */
	union	io_r2y_egmwclpmof	EGMWCLPMOF;		/**< 2841_(A2D8 - A2DFh) */
	union	io_r2y_egmwclpmga	EGMWCLPMGA;		/**< 2841_(A2E0 - A2E3h) */
	union	io_r2y_egmwclpmbd	EGMWCLPMBD;		/**< 2841_(A2E4 - A2E7h) */
} T_IM_R2Y_CTRL_RDMA_EGMW_VAL;

/** Low frequency edge enhancement control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_eglwctl		EGLWCTL;		/**< 2841_(A300 - A303h) */
	union	io_r2y_eglwhpfk		EGLWHPFK;		/**< 2841_(A310 - A31Bh) */
	union	io_r2y_eglwcorpof	EGLWCORPOF;		/**< 2841_(A320 - A327h) */
	union	io_r2y_eglwcorpga	EGLWCORPGA;		/**< 2841_(A328 - A32Bh) */
	union	io_r2y_eglwcorpbd	EGLWCORPBD;		/**< 2841_(A330 - A337h) */
	union	io_r2y_eglwcormof	EGLWCORMOF;		/**< 2841_(A338 - A33Fh) */
	union	io_r2y_eglwcormga	EGLWCORMGA;		/**< 2841_(A340 - A343h) */
	union	io_r2y_eglwcormbd	EGLWCORMBD;		/**< 2841_(A348 - A34Fh) */
	union	io_r2y_eglwsclsup	EGLWSCLSUP;		/**< 2841_(A350 - A353h) */
	union	io_r2y_eglwclppof	EGLWCLPPOF;		/**< 2841_(A358 - A35Fh) */
	union	io_r2y_eglwclppga	EGLWCLPPGA;		/**< 2841_(A360 - A363h) */
	union	io_r2y_eglwclppbd	EGLWCLPPBD;		/**< 2841_(A364 - A367h) */
	union	io_r2y_eglwclpmof	EGLWCLPMOF;		/**< 2841_(A368 - A36Fh) */
	union	io_r2y_eglwclpmga	EGLWCLPMGA;		/**< 2841_(A370 - A373h) */
	union	io_r2y_eglwclpmbd	EGLWCLPMBD;		/**< 2841_(A374 - A377h) */
} T_IM_R2Y_CTRL_RDMA_EGLW_VAL;

/** Edge dot noise correction control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_egdtctl		EGDTCTL;		/**< 2841_(A380 - A383h) */
	union	io_r2y_egdthwth		EGDTHWTH;		/**< 2841_(A384 - A387h) */
	union	io_r2y_egdthwk		EGDTHWK;		/**< 2841_(A388 - A38Bh) */
	union	io_r2y_egdtmwth		EGDTMWTH;		/**< 2841_(A38C - A38Fh) */
	union	io_r2y_egdtmwk		EGDTMWK;		/**< 2841_(A390 - A393h) */
} T_IM_R2Y_CTRL_RDMA_EGDT_VAL;

/** Edge emphasis blend control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_egcmpctl		EGCMPCTL;		/**< 2841_(A3C0 - A3C3h) */
	union	io_r2y_egcmpbd		EGCMPBD;		/**< 2841_(A3C8 - A3CFh) */
	union	io_r2y_egcmpalpof	EGCMPALPOF;		/**< 2841_(A3D0 - A3D3h) */
	union	io_r2y_egcmpalpga	EGCMPALPGA;		/**< 2841_(A3D8 - A3DFh) */
	union	io_r2y_egcmpbtaof	EGCMPBTAOF;		/**< 2841_(A3E0 - A3E3h) */
	union	io_r2y_egcmpbtaga	EGCMPBTAGA;		/**< 2841_(A3E8 - A3EFh) */
	union	io_r2y_egcmpclp		EGCMPCLP;		/**< 2841_(A3F0 - A3F3h) */
} T_IM_R2Y_CTRL_RDMA_EGCMP_VAL;

/** Chroma referenced edge and texture adjustment control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_crvaf		CRVAF;			/**< 2841_(A400 - A403h) */
	union	io_r2y_crvbf		CRVBF;			/**< 2841_(A404 - A407h) */
} T_IM_R2Y_CTRL_RDMA_CRV_VAL;

/** Chroma referenced edge adjustment control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_egcrvctl		EGCRVCTL;		/**< 2841_(A440 - A443h) */
	union	io_r2y_egcrvasclof	EGCRVASCLOF;	/**< 2841_(A444 - A447h) */
	union	io_r2y_egcrvasclga	EGCRVASCLGA;	/**< 2841_(A448 - A44Bh) */
	union	io_r2y_egcrvasclbd	EGCRVASCLBD;	/**< 2841_(A44C - A44Fh) */
	union	io_r2y_egcrvasclcp	EGCRVASCLCP;	/**< 2841_(A450 - A453h) */
	union	io_r2y_egyasclga	EGYASCLGA;		/**< 2841_(A454 - A457h) */
	union	io_r2y_egyasclbd	EGYASCLBD;		/**< 2841_(A458 - A45Bh) */
	union	io_r2y_egyasclclp	EGYASCLCLP;		/**< 2841_(A45C - A45Fh) */
	union	io_r2y_egcrvbsclof	EGCRVBSCLOF;	/**< 2841_(A460 - A463h) */
	union	io_r2y_egcrvbsclga	EGCRVBSCLGA;	/**< 2841_(A464 - A467h) */
	union	io_r2y_egcrvbsclbd	EGCRVBSCLBD;	/**< 2841_(A468 - A46Bh) */
	union	io_r2y_egcrvbsclcp	EGCRVBSCLCP;	/**< 2841_(A46C - A46Fh) */
	union	io_r2y_egybsclga	EGYBSCLGA;		/**< 2841_(A470 - A473h) */
	union	io_r2y_egybsclbd	EGYBSCLBD;		/**< 2841_(A474 - A477h) */
	union	io_r2y_egybsclclp	EGYBSCLCLP;		/**< 2841_(A478 - A47Bh) */
	union	io_r2y_egcrvclp		EGCRVCLP;		/**< 2841_(A47C - A47Fh) */
} T_IM_R2Y_CTRL_RDMA_EGCRV_VAL;

/** Chroma referenced luminance blend control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_ybcrvctl		YBCRVCTL;		/**< 2841_(A480 - A483h) */
	union	io_r2y_ybcrvaalpof	YBCRVAALPOF;	/**< 2841_(A484 - A487h) */
	union	io_r2y_ybcrvaalpga	YBCRVAALPGA;	/**< 2841_(A488 - A48Bh) */
	union	io_r2y_ybcrvaalpbd	YBCRVAALPBD;	/**< 2841_(A48C - A48Fh) */
	union	io_r2y_ybcrvaalpcp	YBCRVAALPCP;	/**< 2841_(A490 - A493h) */
	union	io_r2y_ybyaalpga	YBYAALPGA;		/**< 2841_(A494 - A497h) */
	union	io_r2y_ybyaalpbd	YBYAALPBD;		/**< 2841_(A498 - A49Bh) */
	union	io_r2y_ybyaalpclp	YBYAALPCLP;		/**< 2841_(A49C - A49Fh) */
	union	io_r2y_ybcrvbalpof	YBCRVBALPOF;	/**< 2841_(A4A0 - A4A3h) */
	union	io_r2y_ybcrvbalpga	YBCRVBALPGA;	/**< 2841_(A4A4 - A4A7h) */
	union	io_r2y_ybcrvbalpbd	YBCRVBALPBD;	/**< 2841_(A4A8 - A4ABh) */
	union	io_r2y_ybcrvbalpcp	YBCRVBALPCP;	/**< 2841_(A4AC - A4AFh) */
	union	io_r2y_ybybalpga	YBYBALPGA;		/**< 2841_(A4B0 - A4B3h) */
	union	io_r2y_ybybalpbd	YBYBALPBD;		/**< 2841_(A4B4 - A4B7h) */
	union	io_r2y_ybybalpclp	YBYBALPCLP;		/**< 2841_(A4B8 - A4BBh) */
} T_IM_R2Y_CTRL_RDMA_YBCRV_VAL;

/** Color Noise Reduction(LPF) control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_clpfctl		CLPFCTL;		/**< 2841_(A500 - A503h) */
	union	io_r2y_clpfk		CLPFK;			/**< 2841_(A520 - A533h) */
	union	io_r2y_clpfof		CLPFOF;			/**< 2841_(A540 - A543h) */
	union	io_r2y_clpfga		CLPFGA;			/**< 2841_(A548 - A54Fh) */
	union	io_r2y_clpfbd		CLPFBD;			/**< 2841_(A550 - A557h) */
} T_IM_R2Y_CTRL_RDMA_CLPF_VAL;

/** Chroma suppress control register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_csyctl		CSYCTL;			/**< 2841_(A580 - A583h) */
	union	io_r2y_csyof		CSYOF;			/**< 2841_(A588 - A58Fh) */
	union	io_r2y_csyga		CSYGA;			/**< 2841_(A590 - A597h) */
	union	io_r2y_csybd		CSYBD;			/**< 2841_(A598 - A59Fh) */
	union	io_r2y_ycrvfx		YCRVFX;			/**< 2841_(A5A0 - A5A3h) */
	union	io_r2y_cfix			CFIX;			/**< 2841_(A5A4 - A5A7h) */
	union	io_r2y_ycof			YCOF;			/**< 2841_(A5A8 - A5AFh) */
} T_IM_R2Y_CTRL_RDMA_CSP_VAL;

/** Tone control Table register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_sram_tc		TC;				/**< 294A_(0000 - 07FFh) */
} T_IM_R2Y_CTRL_RDMA_TONE_TBL_VAL;

/** Gamma Table(Diff) Table register value info (for RDMA)
*/
typedef union {
	union	io_r2y_sram_gmrgbdf	GMRGBDF;		/**< 294A_(8000 - 87FFh) */
	union	io_r2y_sram_gmrdf	GMRDF;			/**< 294A_(8800 - 8FFFh) */
	union	io_r2y_sram_gmgdf	GMGDF;			/**< 294A_(9000 - 97FFh) */
	union	io_r2y_sram_gmbdf	GMBDF;			/**< 294A_(9800 - 9FFFh) */
	union	io_r2y_sram_gmybdf	GMYBDF;			/**< 294A_(A000 - A7FFh) */
} U_IM_R2Y_CTRL_RDMA_GMDF_TBL_VAL;

/** Gamma Table(Full) Table register value info (for RDMA)
*/
typedef union {
	union	io_r2y_sram_gmrgbfl	GMRGBFL;		/**< 294A_(A800 - A9FFh) */
	union	io_r2y_sram_gmrfl	GMRFL;			/**< 294A_(AA00 - ABFFh) */
	union	io_r2y_sram_gmgfl	GMGFL;			/**< 294A_(AC00 - ADFFh) */
	union	io_r2y_sram_gmbfl	GMBFL;			/**< 294A_(AE00 - AFFFh) */
	union	io_r2y_sram_gmybfl	GMYBFL;			/**< 294A_(B000 - B1FFh) */
} U_IM_R2Y_CTRL_RDMA_GMFL_TBL_VAL;

/** (High/Medium/Low) Edge emphasis scaling Table register value info (for RDMA)
*/
typedef union {
	union	io_r2y_sram_eghwscl	EGHWSCL;		/**< 294A_(C000 - C1FFh) */
	union	io_r2y_sram_egmwscl	EGMWSCL;		/**< 294A_(C400 - C5FFh) */
	union	io_r2y_sram_eglwscl	EGLWSCL;		/**< 294A_(C800 - C9FFh) */
} U_IM_R2Y_CTRL_RDMA_EGWSCL_TBL_VAL;

/** (High/Medium/Low) Edge emphasis stepping Table register value info (for RDMA)
*/
typedef union {
	union	io_r2y_sram_eghwton	EGHWTON;		/**< 294A_(C200 - C3FFh) */
	union	io_r2y_sram_egmwton	EGMWTON;		/**< 294A_(C600 - C7FFh) */
	union	io_r2y_sram_eglwton	EGLWTON;		/**< 294A_(CA00 - CBFFh) */
} U_IM_R2Y_CTRL_RDMA_EGWTON_TBL_VAL;

/** Map Scale Table register value info (for RDMA)
*/
typedef struct {
	union	io_r2y_sram_egmpscl	EGMPSCL;		/**< 294A_(CC00 - CDBBh) */
} T_IM_R2Y_CTRL_RDMA_EGMPSCL_TBL_VAL;
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Nothing Special
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/** @weakgroup im_r2y
@{
*/

/**
Enable PCLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_On_Pclk( UCHAR pipe_no );

/**
Disable PCLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_Off_Pclk( UCHAR pipe_no );

/**
Force disable PCLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_Force_Off_Pclk( UCHAR pipe_no );

/**
Enable HCLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_On_Hclk( UCHAR pipe_no );

/**
Disable HCLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_Off_Hclk( UCHAR pipe_no );

/**
Force disable HCLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_Force_Off_Hclk( UCHAR pipe_no );

/**
Enable ICLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_On_Iclk( UCHAR pipe_no );

/**
Disable ICLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_Off_Iclk( UCHAR pipe_no );

/**
Force disable ICLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_Force_Off_Iclk( UCHAR pipe_no );

/**
Enable R2YCLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_On_Clk( UCHAR pipe_no );

/**
Disable R2YCLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_Off_Clk( UCHAR pipe_no );

/**
Force disable R2YCLK for R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_R2Y_Force_Off_Clk( UCHAR pipe_no );

/**
R2Y Initialize
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Init( UCHAR pipe_no );

/**
set AXI bus I/F Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_axi			: Control information for AXI bus.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_Axi( UCHAR pipe_no, const T_IM_R2Y_CTRL_AXI* const r2y_ctrl_axi );

/**
get AXI bus read channel status
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		r2y_axi_read_stat		: Status of AXI bus read channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Get_AxiReadStat( UCHAR pipe_no, T_IM_R2Y_AXI_YYR_STAT* const r2y_axi_read_stat );

/**
get AXI bus write channel status
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		r2y_axi_write_stat		: Status of AXI bus write channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Get_AxiWriteStat( UCHAR pipe_no, T_IM_R2Y_AXI_YYW_STAT* const r2y_axi_write_stat );

/**
get AXI bus write channel Transfer mode
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[in]		yyw_no					: YYW number
@param[out]		r2y_axi_write_mode		: Transfer mode of AXI bus write channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Get_AxiWriteMode( UCHAR pipe_no, UCHAR yyw_no, T_IM_R2Y_AXI_YYW_MODE* const r2y_axi_write_mode );

/**
get AXI bus write channel Transfer mode
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		yyw_no					: YYW number
@param[in]		r2y_axi_write_mode		: Transfer mode of AXI bus write channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_AxiWriteMode( UCHAR pipe_no, UCHAR yyw_no, const T_IM_R2Y_AXI_YYW_MODE* const r2y_axi_write_mode );

/**
R2Y Control (each mode common control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl				: Control information for Common.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl( UCHAR pipe_no, const T_IM_R2Y_CTRL* const r2y_ctrl );

/**
R2Y Control (photo mode control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_sdram_input	: Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
*/
extern INT32 Im_R2Y_Ctrl_ModeSDRAMInput( UCHAR pipe_no, const T_IM_R2Y_CTRL_SDRAM_INPUT* const r2y_ctrl_sdram_input );

/**
Set Managed Input Address information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		in_addr					: Input address information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_InAddr_Info( UCHAR pipe_no, const T_IM_R2Y_INADDR_INFO* const in_addr );

/**
Set Managed bank information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[in]		yyw_no					: YYW number.
@param[in]		r2y_bank				: Output bank information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		Selecting normal format	: Set address in units of 8 bytes.<br>
				Selecting video format	: Set address in units of 16384 bytes.
*/
extern INT32 Im_R2Y_Set_OutBankInfo( UCHAR pipe_no, UCHAR yyw_no, const T_IM_R2Y_OUTBANK_INFO* const r2y_bank );

/**
Get bank index value
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[in]		yyw_no					: YYW number
@param[out]		bank_index				: Current bank index
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Get_OutBankIndex( UCHAR pipe_no, UCHAR yyw_no, UCHAR* const bank_index );

/**
Set bank index value
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		yyw_no					: YYW number
@param[in]		bank_index				: Output bank index.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_OutBankIndex( UCHAR pipe_no, UCHAR yyw_no, UCHAR bank_index );

/**
Increment bank index value
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		yyw_no					: YYW number
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Inc_OutBankIndex( UCHAR pipe_no, UCHAR yyw_no );

/**
Set YYW parameter hold
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		hold_enable				: Parameter hold enable/disable
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_YywParamHold( UCHAR pipe_no, UCHAR hold_enable );

/**
R2Y Control (B2R/LTM direct control mode control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_direct			: Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
*/
extern INT32 Im_R2Y_Ctrl_ModeDirect( UCHAR pipe_no, const T_IM_R2Y_CTRL_DIRECT* const r2y_ctrl_direct );

/**
R2Y pixel average reduction rectangle Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_resize_rect_param	: Resize Rectangle Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		Selecting normal format	: Set output_global_w in units of 8 bytes.<br>
				Selecting video format	: Set output_global_w in units of 128 bytes.
										  Setting this parameter to a negative value is prohibited.
*/
extern INT32 Im_R2Y_Set_Resize_Rect( UCHAR pipe_no, const T_IM_R2Y_RESIZE_RECT* const r2y_resize_rect_param );

/**
R2Y resize pitch Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_resize_pitch		: Resize Pitch Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@remarks		Selecting normal format	: Set output_global_w in units of 8 bytes.<br>
				Selecting video format	: Set output_global_w in units of 128 bytes.
										  Setting this parameter to a negative value is prohibited.
*/
extern INT32 Im_R2Y_Set_Resize_Pitch( UCHAR pipe_no, const T_IM_R2Y_RESIZE_PITCH* const r2y_resize_pitch );

/**
R2Y get resize rectangle Control information.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		r2y_resize_param		: Resize Rectangle Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Get_Resize_Param( UCHAR pipe_no, T_IM_R2Y_RESIZE_PARAM* const r2y_resize_param );

/**
Set YYW Right side offset(for video format).
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[in]		yyw_no					: YYW number.
@param[in]		offset_byte				: Offset byte information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_RightSide_Offset( UCHAR pipe_no, UCHAR yyw_no, T_IM_R2Y_RIGHTSIDE_OFFSET* offset_byte );

/**
Get Next Address information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[in]		yyw_no					: YYW number.
@param[out]		r2y_addr				: Pointer of Output address information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Get_Next_OutAddr( UCHAR pipe_no, const UINT32 yyw_no, T_IM_R2Y_OUTADDR_INFO* const r2y_addr );

/**
Get Latest Address information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[in]		yyw_no					: YYW number.
@param[out]		latest_bank_idx			: Pointer of Output address information.
@param[out]		latest_addr				: Pointer of Output address information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Get_Latest_OutAddr( UCHAR pipe_no, UINT32 yyw_no, UCHAR* const latest_bank_idx, T_IM_R2Y_OUTADDR_INFO* const latest_addr );

/**
Set YYW0 External I/F output.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		output_onoff_ipu		: 0:disable External I/F(B) output 1:enable External I/F(B) output
@param[in]		output_onoff_cnr		: 0:disable External I/F(C) output 1:enable External I/F(C) output
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_ExternalIfOutput( UCHAR pipe_no, UCHAR output_onoff_ipu, UCHAR output_onoff_cnr );

/**
Get YYW0 External I/F output status.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		output_onoff_ipu		: 0:disable External I/F(B) output 1:enable External I/F(B) output
@param[out]		output_onoff_cnr		: 0:disable External I/F(C) output 1:enable External I/F(C) output
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Get_ExternalIfOutput( UCHAR pipe_no, UCHAR* const output_onoff_ipu, UCHAR* const output_onoff_cnr );

/**
Set Frame stop
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		onoff					: 0:Frame stop disable 1:Frame stop enable
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_FrameStop( UCHAR pipe_no, UCHAR onoff );

/**
Get YYRch busy status.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		busy_status				: TRUE : YYR is running. (YYRTRG is on). / FALSE : YYR is not running. (YYRTRG is off).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Get_YyrBusy( UCHAR pipe_no, BOOL* const busy_status );

/**
Get Horizontal ring pixel padding number
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		ring_pixs				: Ring pixel padding number.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Get_HRingPixs( UCHAR pipe_no, USHORT* const ring_pixs );

/**
Restart R2Y process when yyw_continue_start_enable equal D_IM_R2Y_ENABLE_OFF.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_ContStart( UCHAR pipe_no );

/**
start R2Y process.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_SYSTEM_ERROR	: systemcall error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@remarks		This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_R2Y_Start( UCHAR pipe_no );

/**
Stop R2Y
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_SYSTEM_ERROR	: systemcall error.
*/
extern INT32 Im_R2Y_Stop( UCHAR pipe_no );

/**
wait end of R2Y macro.
@param[in]		waiptn					: waiting end cause. (bit field)
										<ul><li>@ref D_IM_R2Y1_INT_STATE_YYR_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYWALL_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYW0_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYW0A_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYW1_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYW2_END
											<li>@ref D_IM_R2Y1_INT_STATE_LINE0_END
											<li>@ref D_IM_R2Y1_INT_STATE_LINE1_END
											<li>@ref D_IM_R2Y1_INT_STATE_LINE2_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYR_ERR
											<li>@ref D_IM_R2Y1_INT_STATE_YYW_ERR
											<li>@ref D_IM_R2Y1_INT_STATE_AXR_ERR
											<li>@ref D_IM_R2Y1_INT_STATE_AXW_ERR
											<li>@ref D_IM_R2Y1_INT_STATE_YCFERR_ERR
											<li>@ref D_IM_R2Y2_INT_STATE_YYR_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYWALL_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYW0_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYW0A_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYW1_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYW2_END
											<li>@ref D_IM_R2Y2_INT_STATE_LINE0_END
											<li>@ref D_IM_R2Y2_INT_STATE_LINE1_END
											<li>@ref D_IM_R2Y2_INT_STATE_LINE2_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYR_ERR
											<li>@ref D_IM_R2Y2_INT_STATE_YYW_ERR
											<li>@ref D_IM_R2Y2_INT_STATE_AXR_ERR
											<li>@ref D_IM_R2Y2_INT_STATE_AXW_ERR
											<li>@ref D_IM_R2Y2_INT_STATE_YCFERR_ERR</ul>
@param[in]		tmout					: timeout.
@param[out]		p_flgptn				: end cause. (bit field)
										<ul><li>@ref D_IM_R2Y1_INT_STATE_YYR_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYWALL_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYW0_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYW0A_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYW1_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYW2_END
											<li>@ref D_IM_R2Y1_INT_STATE_LINE0_END
											<li>@ref D_IM_R2Y1_INT_STATE_LINE1_END
											<li>@ref D_IM_R2Y1_INT_STATE_LINE2_END
											<li>@ref D_IM_R2Y1_INT_STATE_YYR_ERR
											<li>@ref D_IM_R2Y1_INT_STATE_YYW_ERR
											<li>@ref D_IM_R2Y1_INT_STATE_AXR_ERR
											<li>@ref D_IM_R2Y1_INT_STATE_AXW_ERR
											<li>@ref D_IM_R2Y1_INT_STATE_YCFERR_ERR
											<li>@ref D_IM_R2Y2_INT_STATE_YYR_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYWALL_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYW0_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYW0A_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYW1_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYW2_END
											<li>@ref D_IM_R2Y2_INT_STATE_LINE0_END
											<li>@ref D_IM_R2Y2_INT_STATE_LINE1_END
											<li>@ref D_IM_R2Y2_INT_STATE_LINE2_END
											<li>@ref D_IM_R2Y2_INT_STATE_YYR_ERR
											<li>@ref D_IM_R2Y2_INT_STATE_YYW_ERR
											<li>@ref D_IM_R2Y2_INT_STATE_AXR_ERR
											<li>@ref D_IM_R2Y2_INT_STATE_AXW_ERR
											<li>@ref D_IM_R2Y2_INT_STATE_YCFERR_ERR</ul>
@retval			D_DDIM_OK				: R2Y normal end. and set end cause to p_flgptn.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_SYSTEM_ERROR	: system call error.
@retval			D_IM_R2Y_TIMEOUT		: wait timeout.
@remarks		This API uses DDIM_User_Clr_Flg().
@remarks		This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Im_R2Y_WaitEnd( DDIM_USER_FLGPTN* const p_flgptn, DDIM_USER_FLGPTN waiptn, DDIM_USER_TMO tmout );

/**
Interrupt handler.
@param[in]		pipe_no	: Image pipe no(pipe1/pipe2).
@remarks		This API uses DDIM_User_Set_Flg().
*/
extern VOID Im_R2Y_Int_Handler( UCHAR pipe_no );

/**
Trimming control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		yyw_no					: YYW number.
@param[in]		r2y_ctrl_trimming		: pointer of trimming control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_Trimming( UCHAR pipe_no, UCHAR yyw_no, const T_IM_R2Y_CTRL_TRIMMING* const r2y_ctrl_trimming );

/**
Trimming control(for external I/F)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_trimming		: pointer of trimming control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_Trimming_External( UCHAR pipe_no, const T_IM_R2Y_CTRL_TRIMMING_EXT* const r2y_ctrl_trimming );

/**
Histogram control register
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_hist			: Histogram control register value.
@retval			D_DDIM_OK				: Success
@retval			D_IM_R2Y_PARAM_ERROR	: Parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. only "r2y_ctrl_hist->enable" is reflected.
*/
extern INT32 Im_R2Y_Ctrl_Histogram( UCHAR pipe_no, const T_IM_R2Y_CTRL_HISTOGRAM* const r2y_ctrl_hist );

/**
Get Histogram value
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		status					: histogram status.
@param[out]		dst_addr				: Histogram value return buffer address. Array number is D_IM_R2Y_TABLE_MAX_HISTOGRAM.
@retval			D_DDIM_OK				: Success
@retval			D_IM_R2Y_PARAM_ERROR	: Parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. Do not update "dst_addr".
*/
extern INT32 Im_R2Y_Get_Histogram( UCHAR pipe_no, T_IM_R2Y_HISTOGRAM_STAT* const status, T_IM_R2Y_HISTOGRAM_ADDR* const dst_addr );

/**
Set Histogram ram access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		hist_enable				: D_IM_R2Y_ENABLE_OFF : Access disable. / D_IM_R2Y_ENABLE_ON : Access enable.
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: Parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
*/
extern INT32 Im_R2Y_Set_HistogramAccessEnable( UCHAR pipe_no, UCHAR hist_enable, UCHAR wait_enable );

/**
Set a partial color extraction
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_color_extract		: pointer of color extraction parameter.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error or R2Y is active.
*/
extern INT32 Im_R2Y_Set_Color_Extract( UCHAR pipe_no, const T_IM_R2Y_COLOR_EXTRACT* const r2y_color_extract );

/**
Set chroma scale value
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_chroma_scale		: pointer of chroma scale parameter.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_Chroma( UCHAR pipe_no, const T_IM_R2Y_CHROMA_SCALE* const r2y_chroma_scale );

/**
Set hue scale value
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_hue_scale			: pointer of hue scale parameter.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_Hue( UCHAR pipe_no, const T_IM_R2Y_HUE_SCALE* const r2y_hue_scale );

/**
Post-resize edge enhancement0 control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_post_resize	: pointer of Post resize edge enhancement control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_PostResizeEdge0( UCHAR pipe_no, const T_IM_R2Y_POST_RESIZE0_PARAM* const r2y_ctrl_post_resize );

/**
Post-resize edge enhancement1 control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_post_resize	: pointer of Post resize edge enhancement control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_PostResizeEdge1( UCHAR pipe_no, const T_IM_R2Y_POST_RESIZE1_PARAM* const r2y_ctrl_post_resize );

/** Set RGB Deknee table.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tbl_type				: Table type (R, G, B).
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_R2Y_TABLE_MAX_RGB_DEKNEE)
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
*/
extern INT32 Im_R2Y_Set_RGBDeknee_Table( UCHAR pipe_no, E_R2Y_DKN_RGBTBL tbl_type, const USHORT* const src_tbl );

/**
Set RGB Deknee access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		access_enable			: set enable/disable Deknee table access control parameters.
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
*/
extern INT32 Im_R2Y_Set_RGBDekneeAccessEnable( UCHAR pipe_no, UCHAR access_enable, UCHAR wait_enable );

/* @} */	// weakgroup im_r2y



/** @weakgroup im_r2y_iq
@{
*/

/**
Offset Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		ofs						: pointer of offset parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_Offset( UCHAR pipe_no, const T_IM_R2Y_OFS* const ofs );

/**
WB Gain control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		rgb_color				: pointer of WB gain parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_WB_Gain( UCHAR pipe_no, const T_IM_R2Y_RGB_COLOR* const rgb_color );

/**
WB Clip Level control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_rgb_color			: pointer of WB clip level parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_WB_Clip_Level( UCHAR pipe_no, const T_IM_R2Y_RGB_COLOR* const r2y_rgb_color );

/**
CC0 Matrix Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_cc				: pointer of CC0 Matrix parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_CC0_Matrix( UCHAR pipe_no, const T_IM_R2Y_CTRL_CC0* const r2y_ctrl_cc );

/**
Set CC0 Matrix coefficient
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		cc0k					: pointer of CC0 Matrix coefficient.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_CC0_Matrix_Coefficient( UCHAR pipe_no, const SHORT* const cc0k );

/**
Multi Axis Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_multi_axis		: pointer of Multi Axis control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_Multi_Axis( UCHAR pipe_no, const T_IM_R2Y_CTRL_MULTI_AXIS* const r2y_ctrl_multi_axis );

/**
RGB Offset before TC control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_btc_offset		: pointer of BTC rgb offset control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_BeforeTone_Offset( UCHAR pipe_no, const T_IM_R2Y_TCOF* const r2y_ctrl_btc_offset );

/**
Luminance Evaluation before TC control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_btc_tct		: pointer of BTC tct control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_BeforeTone_Tct( UCHAR pipe_no, const T_IM_R2Y_TCT* const r2y_ctrl_btc_tct );

/**
Histogram before TC control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_btc_hist		: pointer of BTC histogram control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_BeforeTone_Tchs( UCHAR pipe_no, const T_IM_R2Y_TCHS* const r2y_ctrl_btc_hist );

/**
Set Luminance evaluation table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tct_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_LuminanceEvaluationTblAccessEnable( UCHAR pipe_no, UCHAR tct_enable, UCHAR wait_enable );

/**
Set BTC histogram table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		hist_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_BTC_HistogramTblAccessEnable( UCHAR pipe_no, UCHAR hist_enable, UCHAR wait_enable );

/**
Tone Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_tone			: pointer of Tone control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_Tone( UCHAR pipe_no, const T_IM_R2Y_CTRL_TONE* const r2y_ctrl_tone );

/**
Set Tone control table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tc_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_ToneControlTblAccessEnable( UCHAR pipe_no, UCHAR tc_enable, UCHAR wait_enable );

/**
Is Tone Control active
@param[in]		pipe_no	: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		active_status			: Tone control status(active/inactive).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Is_Act_Tone( UCHAR pipe_no, UCHAR* const active_status );

/**
Gamma Correction control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_gamma			: pointer of Gamma Correction control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_Gamma( UCHAR pipe_no, const T_IM_R2Y_CTRL_GAMMA* const r2y_ctrl_gamma );

/**
Set Gamma table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		acc_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_GammaTblAccessEnable( UCHAR pipe_no, UCHAR acc_enable, UCHAR wait_enable );

/**
Set Gamma Yb table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		acc_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_GammaYbTblAccessEnable( UCHAR pipe_no, UCHAR acc_enable, UCHAR wait_enable );

/** Is Gamma Control active
@param[in]		pipe_no	: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		active_status			: Gamma control status(active/inactive).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
 */
extern INT32 Im_R2Y_Is_Act_Gamma( UCHAR pipe_no, UCHAR* const active_status );

/**
CC1 Matrix Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_cc				: pointer of CC1 Matrix parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_CC1_Matrix( UCHAR pipe_no, const T_IM_R2Y_CTRL_CC1* const r2y_ctrl_cc );

/**
Set CC1 Matrix coefficient
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		cc1k					: pointer of CC1 Matrix coefficient.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_CC1_Matrix_Coefficient( UCHAR pipe_no, const SHORT* const cc1k );

/**
YC Convert control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_ycc			: pointer of YC Convert control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_Yc_Convert( UCHAR pipe_no, const T_IM_R2Y_CTRL_YCC* const r2y_ctrl_ycc );

/**
Y(Luminance) Noise Reduction control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_ynr			: pointer of Y Noise Reduction control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_Ynr( UCHAR pipe_no, const T_IM_R2Y_CTRL_YNR* const r2y_ctrl_ynr );

/**
High/Medium frequency edge enhancement common control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_edge_cmn		: pointer of Edge noise reduction control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_Edge_NoiseReduction( UCHAR pipe_no, const T_IM_R2Y_CTRL_EDGE_CMN* const r2y_ctrl_edge_cmn );

/**
High frequency Edge enhancement control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_hedge			: pointer of Edge enhancement control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_HighEdge( UCHAR pipe_no, const T_IM_R2Y_CTRL_HEDGE* const r2y_ctrl_hedge );

/**
Set High Edge scale table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		scl_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_HighEdgeSclTblAccessEnable( UCHAR pipe_no, UCHAR scl_enable, UCHAR wait_enable );

/**
Set High Edge step table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		step_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_HighEdgeStepTblAccessEnable( UCHAR pipe_no, UCHAR step_enable, UCHAR wait_enable );

/**
Medium frequency Edge enhancement control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_medge			: pointer of Edge enhancement control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_MediumEdge( UCHAR pipe_no, const T_IM_R2Y_CTRL_MEDGE* const r2y_ctrl_medge );

/**
Set Medium Edge scale table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		scl_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_MediumEdgeSclTblAccessEnable( UCHAR pipe_no, UCHAR scl_enable, UCHAR wait_enable );

/**
Set Medium Edge step table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		step_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_MediumEdgeStepTblAccessEnable( UCHAR pipe_no, UCHAR step_enable, UCHAR wait_enable );

/**
Low frequency Edge enhancement control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_ledge			: pointer of Edge enhancement control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_LowEdge( UCHAR pipe_no, const T_IM_R2Y_CTRL_LEDGE* const r2y_ctrl_ledge );

/**
Set Low Edge scale table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		scl_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_LowEdgeSclTblAccessEnable( UCHAR pipe_no, UCHAR scl_enable, UCHAR wait_enable );

/**
Set Low Edge step table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		step_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_LowEdgeStepTblAccessEnable( UCHAR pipe_no, UCHAR step_enable, UCHAR wait_enable );

/** Is Post filter active
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		active_status			: Post filter control status(active/inactive).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
 */
extern INT32 Im_R2Y_Is_Act_PostFilter( UCHAR pipe_no, UCHAR* const active_status );

/**
Edge dot noise control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_edge_dot		: pointer of Edge dot noise control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_EdgeDotNoise( UCHAR pipe_no, const T_IM_R2Y_CTRL_EDGE_DOT_NOISE* const r2y_ctrl_edge_dot );

/**
Map scale control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_mapscl			: pointer of Map scale control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_MapScl( UCHAR pipe_no, const T_IM_R2Y_CTRL_MAPSCL* const r2y_ctrl_mapscl );

/**
Set Map scale table access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		acc_enable				: 0 : access disable / 1 : access enable
@param[in]		wait_enable				: D_IM_R2Y_WAIT_OFF : Turn off wait. / D_IM_R2Y_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_MapSclTblAccessEnable( UCHAR pipe_no, UCHAR acc_enable, UCHAR wait_enable );

/**
Edge enhancement blend control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_edge_blend		: pointer of Edge enhancement blend control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_EdgeBlend( UCHAR pipe_no, const T_IM_R2Y_CTRL_EDGE_BLEND* const r2y_ctrl_edge_blend );

/**
Chroma referenced edge and texture adjustment control
@param[in]		pipe_no									: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_c_ref_edge_texture_adj_common	: pointer of Chroma referenced edge and texture adjustment control parameters.
@retval			D_DDIM_OK								: success.
@retval			D_IM_R2Y_PARAM_ERROR					: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_CRefEdgeTextureAdjCommon( UCHAR pipe_no, const T_IM_R2Y_CTRL_C_REF_EDGE_TEXTURE_ADJ_COMMON* const r2y_ctrl_c_ref_edge_texture_adj_common );

/**
Chroma referenced edge adjustment control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_c_ref_edge_adj	: pointer of Chroma referenced edge adjustment control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_CRefEdgeAdj( UCHAR pipe_no, const T_IM_R2Y_CTRL_C_REF_EDGE_ADJ* const r2y_ctrl_c_ref_edge_adj );

/**
Chroma referenced luminance blend control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_c_ref_yb_blend	: pointer of Chroma referenced edge adjustment control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_CRefYbBlend( UCHAR pipe_no, const T_IM_R2Y_CTRL_C_REF_YB_BLEND* const r2y_ctrl_c_ref_yb_blend );

/**
Color Noise Reduction(LPF) control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_clpf			: pointer of Color Noise Reduction control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_Color_NR( UCHAR pipe_no, const T_IM_R2Y_CTRL_CLPF* const r2y_ctrl_clpf );

/**
Chroma suppress control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		r2y_ctrl_cs				: pointer of Chroma suppress control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Ctrl_Chroma_Suppress( UCHAR pipe_no, const T_IM_R2Y_CTRL_CS* const r2y_ctrl_cs );

/**
Set Tone control table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_R2Y_TABLE_MAX_TONE)
@param[in]		write_ofs_num			: write offset num of table constituent(USHORT data num, must set multiple of 2).
@param[in]		array_num				: num of table constituent(USHORT data num, must set multiple of 2).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Set_Tone_Control_Table( UCHAR pipe_no, const USHORT* const src_tbl, USHORT write_ofs_num, USHORT array_num );

/**
Get Tone control Table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		dst_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_R2Y_TABLE_MAX_TONE)
@param[in]		read_ofs_num			: read offset num of table constituent(USHORT data num, must set multiple of 2).
@param[in]		array_num				: table size(USHORT data num, must set multiple of 2)
@retval			D_DDIM_OK				: success
@retval			D_IM_R2Y_MACRO_BUSY		: Macro busy error.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_R2Y_Get_Tone_Control_Table( UCHAR pipe_no, USHORT* const dst_tbl, USHORT read_ofs_num, USHORT array_num );

/**
Get Luminance evaluation
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		dst_addr				: Luminance evaluation value return buffer address. Array number is D_IM_R2Y_TABLE_MAX_TCT.
@param[in]		array_num				: table size.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. Can not get Luminance evaluation value.
*/
extern INT32 Im_R2Y_Get_LuminanceEvaluation_BeforeTone( UCHAR pipe_no, UINT32* const dst_addr, USHORT array_num );

/**
Get BTC Histogram
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[in]		data_type				: Data type(R/G/B/Y).
@param[out]		overflow_flg			: Histogram overflow flag before TC data.
@param[out]		dst_addr				: Histogram value return buffer address. Array number is D_IM_R2Y_TABLE_MAX_BTC_HISTOGRAM_Y/D_IM_R2Y_TABLE_MAX_BTC_HISTOGRAM_RGB.
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. Can not get histogram.
*/
extern INT32 Im_R2Y_Get_Histogram_BeforeTone( UCHAR pipe_no, E_R2Y_BTC_HIST_RGBTBL data_type, T_IM_R2Y_TCHS_FLG* const overflow_flg, USHORT* const dst_addr );

/**
Set Gamma Table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tbl_index				: Selection of setting table<br>
										  0 : RGB common / 1 : R / 2 : G / 3 : B / 4 : Yb
@param[in]		src_full_tbl			: Pointer of diff Table data array (Type of USHORT, array number is D_IM_R2Y_TABLE_MAX_GAMMA)
@param[in]		src_diff_tbl			: Pointer of full Table data array (Type of ULLONG, array number is D_IM_R2Y_TABLE_MAX_GAMMA)
@retval			D_DDIM_OK				: success
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. Do not update Gamma table.
*/
extern INT32 Im_R2Y_Set_Gamma_Table( UCHAR pipe_no, UCHAR tbl_index, const USHORT* const src_full_tbl, const ULLONG* const src_diff_tbl );

/**
Set High Edge enhancement scaling Table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_R2Y_TABLE_MAX_EDGE_SCALE_HI)
@param[in]		write_ofs_num			: write offset num of table constituent(USHORT data num, must set multiple of 2).
@param[in]		array_num				: num of table constituent(USHORT data num, must set multiple of 2).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. Do not update High Edge enhancement table.
*/
extern INT32 Im_R2Y_Set_HighEdge_Scale_Table( UCHAR pipe_no, const UCHAR* const src_tbl, USHORT write_ofs_num, USHORT array_num );

/**
Set High Edge enhancement stepping Table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_R2Y_TABLE_MAX_EDGE_TC_HI)
@param[in]		write_ofs_num			: write offset num of table constituent(USHORT data num, must set multiple of 2).
@param[in]		array_num				: num of table constituent(USHORT data num, must set multiple of 2).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter errore.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. Do not update High Edge enhancement table.
*/
extern INT32 Im_R2Y_Set_HighEdge_Step_Table( UCHAR pipe_no, const USHORT* const src_tbl, USHORT write_ofs_num, USHORT array_num );

/**
Set Medium Edge enhancement scaling Table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_R2Y_TABLE_MAX_EDGE_SCALE_HI)
@param[in]		write_ofs_num			: write offset num of table constituent(USHORT data num, must set multiple of 2).
@param[in]		array_num				: num of table constituent(USHORT data num, must set multiple of 2).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. Do not update Medium Edge enhancement table.
*/
extern INT32 Im_R2Y_Set_MediumEdge_Scale_Table( UCHAR pipe_no, const UCHAR* const src_tbl, USHORT write_ofs_num, USHORT array_num );

/**
Set Medium Edge enhancement stepping Table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_R2Y_TABLE_MAX_EDGE_TC_HI)
@param[in]		write_ofs_num			: write offset num of table constituent(USHORT data num, must set multiple of 2).
@param[in]		array_num				: num of table constituent(USHORT data num, must set multiple of 2).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter errore.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. Do not update Medium Edge enhancement table.
*/
extern INT32 Im_R2Y_Set_MediumEdge_Step_Table( UCHAR pipe_no, const USHORT* const src_tbl, USHORT write_ofs_num, USHORT array_num );

/**
Set Low Edge enhancement scaling Table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_R2Y_TABLE_MAX_EDGE_SCALE_LO)
@param[in]		write_ofs_num			: write offset num of table constituent(USHORT data num, must set multiple of 2).
@param[in]		array_num				: num of table constituent(USHORT data num, must set multiple of 2).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter errore.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. Do not update Low Edge enhancement table.
*/
extern INT32 Im_R2Y_Set_LowEdge_Scale_Table( UCHAR pipe_no, const UCHAR* const src_tbl, USHORT write_ofs_num, USHORT array_num );

/**
Set Low Edge enhancement stepping Table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_R2Y_TABLE_MAX_EDGE_TC_LO)
@param[in]		write_ofs_num			: write offset num of table constituent(USHORT data num, must set multiple of 2).
@param[in]		array_num				: num of table constituent(USHORT data num, must set multiple of 2).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. Do not update Low Edge enhancement table.
*/
extern INT32 Im_R2Y_Set_LowEdge_Step_Table( UCHAR pipe_no, const USHORT* const src_tbl, USHORT write_ofs_num, USHORT array_num );

/**
Set Map Scale Table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_R2Y_TABLE_MAX_MAPSCL)
@param[in]		write_ofs_num			: write offset num of table constituent(USHORT data num, must set multiple of 2).
@param[in]		array_num				: num of table constituent(USHORT data num, must set multiple of 2).
@retval			D_DDIM_OK				: success.
@retval			D_IM_R2Y_PARAM_ERROR	: parameter error.
@retval			D_IM_R2Y_MACRO_BUSY		: Macro is running. Do not update Low Edge enhancement table.
*/
extern INT32 Im_R2Y_Set_MapScl_Table( UCHAR pipe_no, const USHORT* const src_tbl, USHORT write_ofs_num, USHORT array_num );

/* @} */	// weakgroup im_r2y_iq

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
/**
Get the top address of the address array of Post-resize edge enhancement0 control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Post-resize edge enhancement0 control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EE0_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_EE0_ADDR** addr );

/**
Get the top address of the address array of Post-resize edge enhancement1 control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Post-resize edge enhancement1 control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EE1_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_EE1_ADDR** addr );

/**
Get the top address of the address array of Deknee table.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tbl_type				: Table type (R, G, B).
@param[out]		addr					: Top address of the address array of Deknee table.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_Deknee_Table( UCHAR pipe_no, E_R2Y_DKN_RGBTBL tbl_type, const T_IM_R2Y_CTRL_RDMA_DEKNEE_TBL_ADDR** addr );

/**
Get the top address of the address array of Offset control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Offset control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_Offset_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_OFST_ADDR** addr );

/**
Get the top address of the address array of WB clip control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of WB clip control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_WB_Clip_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_WB_CLIP_ADDR** addr );

/**
Get the top address of the address array of CC0 matrix control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of CC0 matrix control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_CC0_Matrix_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_CC0_ADDR** addr );

/**
Get the top address of the address array of CC0 matrix coefficient control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of CC0 matrix coefficient control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_CC0_Matrix_Coefficient_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_CC0_COEF_ADDR** addr );

/**
Get the top address of the address array of Multi Axis control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Multi Axis control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_Multi_Axis_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_MCYC_ADDR** addr );

/**
Get the top address of the address array of RGB Offset before TC control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of RGB Offset before TC control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_BeforeTone_Offset_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_BTC_TCOF_ADDR** addr );

/**
Get the top address of the address array of Luminance Evaluation before TC control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Luminance Evaluation before TC control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_BeforeTone_Tct_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_BTC_TCT_ADDR** addr );

/**
Get the top address of the address array of Histogram before TC control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Histogram before TC control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_BeforeTone_Tchs_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_BTC_TCHS_ADDR** addr );

/**
Get the top address of the address array of Tone control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Tone control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_Tone_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_TONE_ADDR** addr );

/**
Get the top address of the address array of Gamma control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Gamma control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_Gamma_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_GAMMA_ADDR** addr );

/**
Get the top address of the address array of CC1 matrix control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of CC1 matrix control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_CC1_Matrix_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_CC1_ADDR** addr );

/**
Get the top address of the address array of CC1 matrix coefficient control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of CC1 matrix coefficient control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_CC1_Matrix_Coefficient_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_CC1_COEF_ADDR** addr );

/**
Get the top address of the address array of YCC control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of YCC control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_YCC_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_YCC_ADDR** addr );

/**
Get the top address of the address array of YNR control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of YNR control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_YNR_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_YNR_ADDR** addr );

/**
Get the top address of the address array of EENR control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of EENR control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EENR_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_EENR_ADDR** addr );

/**
Get the top address of the address array of High frequency edge enhancement control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of High frequency edge enhancement control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EGHW_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_EGHW_ADDR** addr );

/**
Get the top address of the address array of Medium frequency edge enhancement control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Medium frequency edge enhancement control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EGMW_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_EGMW_ADDR** addr );

/**
Get the top address of the address array of Low frequency edge enhancement control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Low frequency edge enhancement control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EGLW_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_EGLW_ADDR** addr );

/**
Get the top address of the address array of Edge dot noise correction control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Edge dot noise correction control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EGDT_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_EGDT_ADDR** addr );

/**
Get the top address of the address array of Edge emphasis blend control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Edge emphasis blend control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EGCMP_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_EGCMP_ADDR** addr );

/**
Get the top address of the address array of Chroma referenced edge and texture adjustment control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Chroma referenced edge and texture adjustment control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_CRV_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_CRV_ADDR** addr );

/**
Get the top address of the address array of Chroma referenced edge adjustment control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Chroma referenced edge adjustment control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EGCRV_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_EGCRV_ADDR** addr );

/**
Get the top address of the address array of Chroma referenced luminance blend control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Chroma referenced luminance blend control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_YBCRV_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_YBCRV_ADDR** addr );

/**
Get the top address of the address array of Color Noise Reduction(LPF) control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Color Noise Reduction(LPF) control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_CLPF_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_CLPF_ADDR** addr );

/**
Get the top address of the address array of Chroma suppress control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Chroma suppress control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_CSP_Cntl( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_CSP_ADDR** addr );

/**
Get the top address of the address array of Tone control table.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Tone control table.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_Tone_Table( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_TONE_TBL_ADDR** addr );

/**
Get the top address of the address array of Gamma Table(Diff).
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tbl_index				: Selection of setting table<br>
										  0 : RGB common / 1 : R / 2 : G / 3 : B / 4 : Yb
@param[out]		addr					: Top address of the address array of Gamma Table(Diff).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_GMDF_Table( UCHAR pipe_no, UCHAR tbl_index, const T_IM_R2Y_CTRL_RDMA_GMDF_TBL_ADDR** addr );

/**
Get the top address of the address array of Gamma Table(Full).
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tbl_index				: Selection of table<br>
										  0 : RGB common / 1 : R / 2 : G / 3 : B / 4 : Yb
@param[out]		addr					: Top address of the address array of Gamma Table(Full).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_GMFL_Table( UCHAR pipe_no, UCHAR tbl_index, const T_IM_R2Y_CTRL_RDMA_GMFL_TBL_ADDR** addr );

/**
Get the top address of the address array of (High/Medium/Low) edge emphasis scaling table.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tbl_index				: Selection of table<br>
										  0 : High / 1 : Medium / 2 : Low
@param[out]		addr					: Top address of the address array of High edge emphasis scaling table.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EGWSCL_Table( UCHAR pipe_no, UCHAR tbl_index, const T_IM_R2Y_CTRL_RDMA_EGWSCL_TBL_ADDR** addr );

/**
Get the top address of the address array of (High/Medium/Low) edge emphasis stepping table.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tbl_index				: Selection of table<br>
										  0 : High / 1 : Medium / 2 : Low
@param[out]		addr					: Top address of the address array of High edge emphasis stepping table.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EGWTON_Table( UCHAR pipe_no, UCHAR tbl_index, const T_IM_R2Y_CTRL_RDMA_EGWTON_TBL_ADDR** addr );

/**
Get the top address of the address array of Map Scale table.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Map Scale table.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_R2Y_Get_RdmaAddr_EGMPSCL_Table( UCHAR pipe_no, const T_IM_R2Y_CTRL_RDMA_EGMPSCL_TBL_ADDR** addr );

#endif	// CO_DDIM_UTILITY_USE


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IM_R2Y_H_

/**
@weakgroup im_r2y
@{

<hr>

@section im_r2y_sample Sample code

@section im_r2y_sample_section1	Sample code of R2Y Initialization.
	@code
	Im_R2Y_Init( D_IM_R2Y_PIPE12 );
	@endcode
	<br><br>

@section im_r2y_sample_section2	Sample code of data setting for Photo.
	@code
	#define D_USER_YCC_TOP_X_PIXS 16
	#define D_USER_YCC_TOP_Y_PIXS 12
	#define D_USER_YCC_WIDTH_PIXS 640
	#define D_USER_YCC_LINES_PIXS 480

	#define D_USER_RGB_LINEFEED_BYTES (D_USER_YCC_WIDTH_PIXS *3)	// 1pixel = 24bit
	#define D_USER_RGB_IN_ADDR_R   0x00000000
	#define D_USER_RGB_IN_ADDR_G   0x00000000
	#define D_USER_RGB_IN_ADDR_B   0x00000000
	#define D_USER_YCC_OUT_ADDR_Y  0x00000000
	#define D_USER_YCC_OUT_ADDR_CB 0x00000000
	#define D_USER_YCC_OUT_ADDR_CR 0x00000000

	VOID User_Photo_Handler( UINT32* status, UINT32 user_param );

	VOID User_Capture_R2Y_Process( VOID ) {
		T_IM_R2Y_CTRL r2y_ctrl;
		T_IM_R2Y_CTRL_SDRAM_INPUT r2y_ctrl_sdram_in;
		T_IM_R2Y_RESIZE_RECT r2y_resize_rect_param;
		T_IM_R2Y_INADDR_INFO r2y_in_addr;
		T_IM_R2Y_OUTBANK_INFO r2y_addr_0;
		T_IM_R2Y_ERR_FLAG t_r2y_err_flg;

		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.master_if_select = D_IM_R2Y_MASTER_IF0_PORT0 | D_IM_R2Y_MASTER_IF0_PORT1 | D_IM_R2Y_MASTER_IF0_PORT2;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.burst_length = D_IM_R2Y_BRST_512;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_Y] = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_CB] = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_CR] = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_Y] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_CB] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_CR] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].pixel_avg_reduction_mode = D_IM_R2Y_RDC_MODE_DIV_2;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].pixel_avg_reduction_enable = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].ycc_cc_thin_select = D_IM_R2Y_THIN_OUT_YCC422_CENTER;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].trim_out_enable = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].planar_interleaved_mode = D_IM_R2Y_PORT_PLANAR;

		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.master_if_select = D_IM_R2Y_MASTER_IF0_PORT0 | D_IM_R2Y_MASTER_IF0_PORT1 | D_IM_R2Y_MASTER_IF0_PORT2;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.burst_length = D_IM_R2Y_BRST_512;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_Y] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_CB] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_CR] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_Y] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_CB] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_CR] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].pixel_avg_reduction_mode = D_IM_R2Y_RDC_MODE_DIV_2;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].pixel_avg_reduction_enable = D_IM_R2Y_ENABLE_OFF;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].ycc_cc_thin_select = D_IM_R2Y_THIN_OUT_YCC422_CENTER;				// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].trim_out_enable = D_IM_R2Y_ENABLE_OFF;								// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].planar_interleaved_mode = D_IM_R2Y_PORT_PLANAR;						// dummy

		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].axi_write_mode.master_if_select = D_IM_R2Y_MASTER_IF0_PORT0;		// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].axi_write_mode.burst_length = D_IM_R2Y_BRST_512;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].axi_write_mode.out_enable[D_IM_R2Y_PORT_Y] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].axi_write_mode.out_enable[D_IM_R2Y_PORT_CB] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].axi_write_mode.out_enable[D_IM_R2Y_PORT_CR] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].write_request_mask[D_IM_R2Y_PORT_Y] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].write_request_mask[D_IM_R2Y_PORT_CB] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].write_request_mask[D_IM_R2Y_PORT_CR] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].pixel_avg_reduction_mode = D_IM_R2Y_RDC_MODE_DIV_2;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].pixel_avg_reduction_enable = D_IM_R2Y_ENABLE_OFF;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].ycc_cc_thin_select = D_IM_R2Y_THIN_OUT_YCC422_CENTER;				// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].trim_out_enable = D_IM_R2Y_ENABLE_OFF;								// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].planar_interleaved_mode = D_IM_R2Y_PORT_PLANAR;						// dummy

		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].axi_write_mode.master_if_select = D_IM_R2Y_MASTER_IF0_PORT0;		// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].axi_write_mode.burst_length = D_IM_R2Y_BRST_512;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].axi_write_mode.out_enable[D_IM_R2Y_PORT_Y] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].axi_write_mode.out_enable[D_IM_R2Y_PORT_CB] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].axi_write_mode.out_enable[D_IM_R2Y_PORT_CR] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].write_request_mask[D_IM_R2Y_PORT_Y] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].write_request_mask[D_IM_R2Y_PORT_CB] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].write_request_mask[D_IM_R2Y_PORT_CR] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].pixel_avg_reduction_mode = D_IM_R2Y_RDC_MODE_DIV_2;				// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].pixel_avg_reduction_enable = D_IM_R2Y_ENABLE_OFF;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].ycc_cc_thin_select = D_IM_R2Y_THIN_OUT_YCC422_CENTER;				// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].trim_out_enable = D_IM_R2Y_ENABLE_OFF;								// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].planar_interleaved_mode = D_IM_R2Y_PORT_PLANAR;					// dummy

		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_0].level = 0;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_1].level = 0;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_2].level = 0;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_0].yyw_select = D_IM_R2Y_YYW_CH_0;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_1].yyw_select = D_IM_R2Y_YYW_CH_0;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_2].yyw_select = D_IM_R2Y_YYW_CH_0;

		r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_0] = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_1] = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_2] = D_IM_R2Y_ENABLE_OFF;

		r2y_ctrl.line_transfer_cycle = 0;
		r2y_ctrl.yyw_continue_start_enable = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.yyw_horizontal_flip = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.video_format_out_select_0 = D_IM_R2Y_VFM_NORMAL;
		r2y_ctrl.ipu_macro_output_enable = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.ipu_macro_trimming_enable = D_IM_R2Y_ENABLE_OFF;	// dummy
		r2y_ctrl.cnr_macro_output_enable = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.cnr_macro_trimming_enable = D_IM_R2Y_ENABLE_OFF;	// dummy
		r2y_ctrl.output_mode_0a = D_IM_R2Y_YYW0A_OUTPUT_MODE_STOP;
		r2y_ctrl.video_format_out_select_0a = D_IM_R2Y_VFM_NORMAL;	// dummy
		r2y_ctrl.output_format_sel1 = D_IM_R2Y_DATA_FORMAT_8;		// dummy
		r2y_ctrl.output_type_sel1 = D_IM_R2Y_WRITE_DTYP_PACK8;		// dummy
		r2y_ctrl.ycf_bypass = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.ycf_padding = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.mcc_select = D_IM_R2Y_MCC_AFTER_CC0;
		r2y_ctrl.mcc_bit_shift = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.r2y_user_handler = User_Photo_Handler;
		r2y_ctrl.user_param = 0;

		r2y_ctrl_sdram_in.burst_length = D_IM_R2Y_BRST_512;
		r2y_ctrl_sdram_in.read_request_mask[D_IM_R2Y_PORT_0] = 0;
		r2y_ctrl_sdram_in.read_request_mask[D_IM_R2Y_PORT_1] = 0;
		r2y_ctrl_sdram_in.read_request_mask[D_IM_R2Y_PORT_2] = 0;
		r2y_ctrl_sdram_in.input_dtype = D_IM_R2Y_READ_DTYP_PACK12;
		r2y_ctrl_sdram_in.mono_ebable = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl_sdram_in.rgb_deknee_enable = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl_sdram_in.rgb_left_shift = D_IM_R2Y_STL_RGB_LEFT_SHIFT_NONE;
		r2y_ctrl_sdram_in.rgb_saturation_mode = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.top_offset[D_IM_R2Y_PORT_Y] = 0;
		r2y_ctrl.top_offset[D_IM_R2Y_PORT_CB] = 0;
		r2y_ctrl.top_offset[D_IM_R2Y_PORT_CR] = 0;
		r2y_ctrl_sdram_in.input_global = D_USER_RGB_LINEFEED_BYTES;

		r2y_resize_rect_param.input_size.img_top = D_USER_YCC_TOP_X_PIXS;
		r2y_resize_rect_param.input_size.img_left = D_USER_YCC_TOP_Y_PIXS;
		r2y_resize_rect_param.input_size.img_width = D_USER_YCC_WIDTH_PIXS;
		r2y_resize_rect_param.input_size.img_lines = D_USER_YCC_LINES_PIXS;
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].yyw_width = D_USER_YCC_WIDTH_PIXS;
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].yyw_lines = D_USER_YCC_LINES_PIXS;
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].output_global_w[D_IM_R2Y_PORT_Y] = D_USER_YCC_WIDTH_PIXS;
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].output_global_w[D_IM_R2Y_PORT_CB] = D_USER_YCC_WIDTH_PIXS / 2;
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_1].yyw_width = D_USER_YCC_WIDTH_PIXS;									// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_1].yyw_lines = D_USER_YCC_LINES_PIXS;									// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_1].output_global_w[D_IM_R2Y_PORT_Y] = D_USER_YCC_WIDTH_PIXS;			// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_1].output_global_w[D_IM_R2Y_PORT_CB] = D_USER_YCC_WIDTH_PIXS / 2;		// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_2].yyw_width = D_USER_YCC_WIDTH_PIXS;									// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_2].yyw_lines = D_USER_YCC_LINES_PIXS;									// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_2].output_global_w[D_IM_R2Y_PORT_Y] = D_USER_YCC_WIDTH_PIXS;			// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_2].output_global_w[D_IM_R2Y_PORT_CB] = D_USER_YCC_WIDTH_PIXS / 2;		// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0A].yyw_width = D_USER_YCC_WIDTH_PIXS;								// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0A].yyw_lines = D_USER_YCC_LINES_PIXS;								// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0A].output_global_w[D_IM_R2Y_PORT_Y] = D_USER_YCC_WIDTH_PIXS;			// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0A].output_global_w[D_IM_R2Y_PORT_CB] = D_USER_YCC_WIDTH_PIXS / 2;	// dummy

		r2y_in_addr.rgb.addr_R = (VOID*)D_USER_RAW_IN_ADDR_R;
		r2y_in_addr.rgb.addr_G = (VOID*)D_USER_RAW_IN_ADDR_G;
		r2y_in_addr.rgb.addr_B = (VOID*)D_USER_RAW_IN_ADDR_B;

		memset( &r2y_addr_0, '\x00', sizeof(r2y_addr_0) );
		r2y_addr_0.bank_initial_position = D_IM_R2Y_YYW_BANK_0;
		r2y_addr_0.use_bank_num = 3;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Y = (VOID*)D_USER_YCC_OUT_ADDR_Y;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Cb = (VOID*)D_USER_YCC_OUT_ADDR_CB;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Cr = (VOID*)D_USER_YCC_OUT_ADDR_CR;

		Im_R2Y_Ctrl( D_IM_R2Y_PIPE1, &r2y_ctrl );
		Im_R2Y_Ctrl_ModeSDRAMInput( D_IM_R2Y_PIPE1, &r2y_ctrl_sdram_in );
		Im_R2Y_Set_Resize_Rect( D_IM_R2Y_PIPE1, &r2y_resize_rect_param );
		Im_R2Y_Set_InAddr_Info( D_IM_R2Y_PIPE1, &r2y_in_addr );
		Im_R2Y_Set_OutBankInfo( D_IM_R2Y_PIPE1, D_IM_R2Y_YYW_0, &r2y_addr_0 );

		// Start R2Y
		Im_R2Y_Start(D_IM_R2Y_PIPE1);

		// Wait R2Y YYW0 is normal end.
		Im_R2Y_WaitEnd( NULL, D_IM_R2Y1_INT_FLG_YYW0_END, 60 );

		// Stop R2Y
		Im_R2Y_Stop(D_IM_R2Y_PIPE1);
	}

	T_IM_R2Y_HISTOGRAM_STAT photo_hist_overflow_status;
	T_IM_R2Y_HISTOGRAM_ADDR photo_hist_dst_buf;
	VOID User_Photo_Handler( UINT32* status, UINT32 user_param )
	{
		switch( *status ) {
			case D_IM_R2Y1_INT_FLG_YYW0_END:
				// Finished create YCC image.
				Im_R2Y_Get_Histogram( D_IM_R2Y_PIPE1, &photo_hist_overflow_status, &photo_hist_dst_buf );
				break;
			default:
				// Nothing to do.
				break;
		}
	}
	@endcode
	<br><br>


@section im_r2y_sample_section3	Sample code of data setting for Video.
	@code
	#define D_USER_YCC_TOP_X_PIXS 16
	#define D_USER_YCC_TOP_Y_PIXS 12
	#define D_USER_YCC_WIDTH_PIXS 640
	#define D_USER_YCC_LINES_PIXS 480

	#define D_USER_VIEW_HORIZONTAL_BLANKING 200
	#define D_USER_VIEW_YCC_OUT_ADDR_0_Y  0x00000000
	#define D_USER_VIEW_YCC_OUT_ADDR_0_CB 0x00000000
	#define D_USER_VIEW_YCC_OUT_ADDR_0_CR 0x00000000
	#define D_USER_VIEW_YCC_OUT_ADDR_1_Y  0x00000000
	#define D_USER_VIEW_YCC_OUT_ADDR_1_CB 0x00000000
	#define D_USER_VIEW_YCC_OUT_ADDR_1_CR 0x00000000
	#define D_USER_VIEW_YCC_OUT_ADDR_2_Y  0x00000000
	#define D_USER_VIEW_YCC_OUT_ADDR_2_CB 0x00000000
	#define D_USER_VIEW_YCC_OUT_ADDR_2_CR 0x00000000
	#define D_USER_VIEW_YCC_OUT_ADDR_3_Y  0x00000000
	#define D_USER_VIEW_YCC_OUT_ADDR_3_CB 0x00000000
	#define D_USER_VIEW_YCC_OUT_ADDR_3_CR 0x00000000

	VOID User_View_Handler( UINT32* status, UINT32 user_param );

	VOID User_View_R2Y_Start( VOID ) {
		T_IM_R2Y_CTRL r2y_ctrl;
		T_IM_R2Y_CTRL_DIRECT r2y_ctrl_direct;
		T_IM_R2Y_RESIZE_RECT r2y_resize_rect_param;
		T_IM_R2Y_OUTBANK_INFO r2y_addr_0;
		T_IM_R2Y_CTRL_HISTOGRAM r2y_hist_ctrl;

		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.master_if_select = D_IM_R2Y_MASTER_IF0_PORT0 | D_IM_R2Y_MASTER_IF0_PORT1 | D_IM_R2Y_MASTER_IF0_PORT2;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.burst_length = D_IM_R2Y_BRST_512;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_Y] = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_CB] = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_CR] = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_Y] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_CB] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_CR] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].pixel_avg_reduction_mode = D_IM_R2Y_RDC_MODE_DIV_2;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].pixel_avg_reduction_enable = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].ycc_cc_thin_select = D_IM_R2Y_THIN_OUT_YCC422_CENTER;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].trim_out_enable = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].planar_interleaved_mode = D_IM_R2Y_PORT_PLANAR;

		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.master_if_select = D_IM_R2Y_MASTER_IF0_PORT0 | D_IM_R2Y_MASTER_IF0_PORT1 | D_IM_R2Y_MASTER_IF0_PORT2;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.burst_length = D_IM_R2Y_BRST_512;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_Y] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_CB] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_CR] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_Y] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_CB] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_CR] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].pixel_avg_reduction_mode = D_IM_R2Y_RDC_MODE_DIV_2;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].pixel_avg_reduction_enable = D_IM_R2Y_ENABLE_OFF;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].ycc_cc_thin_select = D_IM_R2Y_THIN_OUT_YCC422_CENTER;				// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].trim_out_enable = D_IM_R2Y_ENABLE_OFF;								// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].planar_interleaved_mode = D_IM_R2Y_PORT_PLANAR;						// dummy

		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].axi_write_mode.master_if_select = D_IM_R2Y_MASTER_IF0_PORT0;		// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].axi_write_mode.burst_length = D_IM_R2Y_BRST_512;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].axi_write_mode.out_enable[D_IM_R2Y_PORT_Y] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].axi_write_mode.out_enable[D_IM_R2Y_PORT_CB] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].axi_write_mode.out_enable[D_IM_R2Y_PORT_CR] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].write_request_mask[D_IM_R2Y_PORT_Y] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].write_request_mask[D_IM_R2Y_PORT_CB] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].write_request_mask[D_IM_R2Y_PORT_CR] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].pixel_avg_reduction_mode = D_IM_R2Y_RDC_MODE_DIV_2;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].pixel_avg_reduction_enable = D_IM_R2Y_ENABLE_OFF;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].ycc_cc_thin_select = D_IM_R2Y_THIN_OUT_YCC422_CENTER;				// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].trim_out_enable = D_IM_R2Y_ENABLE_OFF;								// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2].planar_interleaved_mode = D_IM_R2Y_PORT_PLANAR;						// dummy

		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].axi_write_mode.master_if_select = D_IM_R2Y_MASTER_IF0_PORT0;		// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].axi_write_mode.burst_length = D_IM_R2Y_BRST_512;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].axi_write_mode.out_enable[D_IM_R2Y_PORT_Y] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].axi_write_mode.out_enable[D_IM_R2Y_PORT_CB] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].axi_write_mode.out_enable[D_IM_R2Y_PORT_CR] = D_IM_R2Y_ENABLE_ON;	// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].write_request_mask[D_IM_R2Y_PORT_Y] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].write_request_mask[D_IM_R2Y_PORT_CB] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].write_request_mask[D_IM_R2Y_PORT_CR] = 0;							// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].pixel_avg_reduction_mode = D_IM_R2Y_RDC_MODE_DIV_2;				// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].pixel_avg_reduction_enable = D_IM_R2Y_ENABLE_OFF;					// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].ycc_cc_thin_select = D_IM_R2Y_THIN_OUT_YCC422_CENTER;				// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].trim_out_enable = D_IM_R2Y_ENABLE_OFF;								// dummy
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A].planar_interleaved_mode = D_IM_R2Y_PORT_PLANAR;					// dummy

		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_0].level = 0;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_1].level = 0;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_2].level = 0;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_0].yyw_select = D_IM_R2Y_YYW_CH_0;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_1].yyw_select = D_IM_R2Y_YYW_CH_0;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_2].yyw_select = D_IM_R2Y_YYW_CH_0;

		r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_0] = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_1] = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_2] = D_IM_R2Y_ENABLE_OFF;

		r2y_ctrl.line_transfer_cycle = 0;
		r2y_ctrl.yyw_continue_start_enable = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.yyw_horizontal_flip = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.video_format_out_select_0 = D_IM_R2Y_VFM_NORMAL;
		r2y_ctrl.ipu_macro_output_enable = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.ipu_macro_trimming_enable = D_IM_R2Y_ENABLE_OFF;	// dummy
		r2y_ctrl.cnr_macro_output_enable = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.cnr_macro_trimming_enable = D_IM_R2Y_ENABLE_OFF;	// dummy
		r2y_ctrl.output_mode_0a = D_IM_R2Y_YYW0A_OUTPUT_MODE_STOP;
		r2y_ctrl.video_format_out_select_0a = D_IM_R2Y_VFM_NORMAL;	// dummy
		r2y_ctrl.output_format_sel1 = D_IM_R2Y_DATA_FORMAT_8;		// dummy
		r2y_ctrl.output_type_sel1 = D_IM_R2Y_WRITE_DTYP_PACK8;		// dummy
		r2y_ctrl.ycf_bypass = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.ycf_padding = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.mcc_select = D_IM_R2Y_MCC_AFTER_CC0;
		r2y_ctrl.mcc_bit_shift = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.r2y_user_handler = User_View_Handler;
		r2y_ctrl.user_param = 0;

		r2y_resize_rect_param.input_size.img_top = D_USER_YCC_TOP_X_PIXS;
		r2y_resize_rect_param.input_size.img_left = D_USER_YCC_TOP_Y_PIXS;
		r2y_resize_rect_param.input_size.img_width = D_USER_YCC_WIDTH_PIXS;
		r2y_resize_rect_param.input_size.img_lines = D_USER_YCC_LINES_PIXS;
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].yyw_width = D_USER_YCC_WIDTH_PIXS;
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].yyw_lines = D_USER_YCC_LINES_PIXS;
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].output_global_w[D_IM_R2Y_PORT_Y] = D_USER_YCC_WIDTH_PIXS;
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0].output_global_w[D_IM_R2Y_PORT_CB] = D_USER_YCC_WIDTH_PIXS / 2;
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_1].yyw_width = D_USER_YCC_WIDTH_PIXS;									// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_1].yyw_lines = D_USER_YCC_LINES_PIXS;									// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_1].output_global_w[D_IM_R2Y_PORT_Y] = D_USER_YCC_WIDTH_PIXS;			// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_1].output_global_w[D_IM_R2Y_PORT_CB] = D_USER_YCC_WIDTH_PIXS / 2;		// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_2].yyw_width = D_USER_YCC_WIDTH_PIXS;									// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_2].yyw_lines = D_USER_YCC_LINES_PIXS;									// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_2].output_global_w[D_IM_R2Y_PORT_Y] = D_USER_YCC_WIDTH_PIXS;			// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_2].output_global_w[D_IM_R2Y_PORT_CB] = D_USER_YCC_WIDTH_PIXS / 2;		// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0A].yyw_width = D_USER_YCC_WIDTH_PIXS;								// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0A].yyw_lines = D_USER_YCC_LINES_PIXS;								// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0A].output_global_w[D_IM_R2Y_PORT_Y] = D_USER_YCC_WIDTH_PIXS;			// dummy
		r2y_resize_rect_param.output_size[D_IM_R2Y_YYW_CH_0A].output_global_w[D_IM_R2Y_PORT_CB] = D_USER_YCC_WIDTH_PIXS / 2;	// dummy

		memset( &r2y_addr_0, '\x00', sizeof(r2y_addr_0) );
		r2y_addr_0.bank_initial_position = D_IM_R2Y_YYW_BANK_0;
		r2y_addr_0.use_bank_num = 3;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Y  = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_0_Y;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Cb = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_0_CB;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_0].ycc.addr_Cr = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_0_CR;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_1].ycc.addr_Y  = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_1_Y;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_1].ycc.addr_Cb = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_1_CB;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_1].ycc.addr_Cr = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_1_CR;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_2].ycc.addr_Y  = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_2_Y;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_2].ycc.addr_Cb = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_2_CB;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_2].ycc.addr_Cr = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_2_CR;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_3].ycc.addr_Y  = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_3_Y;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_3].ycc.addr_Cb = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_3_CB;
		r2y_addr_0.output_addr[D_IM_R2Y_YYW_BANK_3].ycc.addr_Cr = (VOID*)D_USER_VIEW_YCC_OUT_ADDR_3_CR;

		r2y_hist_ctrl.enable = D_IM_R2Y_ENABLE_ON;
		r2y_hist_ctrl.yyw_no = D_IM_R2Y_YYW_CH_0;
		r2y_hist_ctrl.sampling_pitch = D_IM_R2Y_HIST_PIT_16X16;
		r2y_hist_ctrl.histogram_area.img_top = 0;
		r2y_hist_ctrl.histogram_area.img_left = 0;
		r2y_hist_ctrl.histogram_area.img_width = D_USER_YCC_WIDTH_PIXS;
		r2y_hist_ctrl.histogram_area.img_lines = D_USER_YCC_LINES_PIXS;

		Im_R2Y_Ctrl( D_IM_R2Y_PIPE1, &r2y_ctrl );

		r2y_ctrl_direct.frame_stop = 0;

		Im_R2Y_Ctrl_ModeDirect( D_IM_R2Y_PIPE1, &r2y_ctrl_direct );
		Im_R2Y_Set_Resize_Rect( D_IM_R2Y_PIPE1, &r2y_resize_rect_param );
		Im_R2Y_Set_OutBankInfo( D_IM_R2Y_PIPE1, D_IM_R2Y_YYW_0, &r2y_addr_0 );

		Im_R2Y_Ctrl_Histogram( D_IM_R2Y_PIPE1, r2y_hist_ctrl );

		// Start R2Y
		Im_R2Y_Start(D_IM_R2Y_PIPE1);
	}

	T_IM_R2Y_HISTOGRAM_STAT view_hist_overflow_status;
	T_IM_R2Y_HISTOGRAM_ADDR view_hist_dst_buf;
	VOID User_View_Handler( UINT32* status, UINT32 user_param )
	{
		switch( *status ) {
			case D_IM_R2Y1_INT_FLG_YYW0_END:
				// Finished create YCC image.
				Im_R2Y_Get_Histogram( D_IM_R2Y_PIPE1, &view_hist_overflow_status, &view_hist_dst_buf );
				break;
			default:
				// Nothing to do.
				break;
		}
	}
	@endcode
	<br><br>

@}*/	// weakgroup im_r2y


/**
@weakgroup im_r2y_iq
@{

<hr>

@section im_r2y_iq_sample Sample code

@section im_r2y_iq_sample_section1	Sample code of data setting for White Balance Gain.
	The precondition of the this sample is as follows.<br>
	@code
	T_IM_R2Y_RGB_COLOR r2y_wb;
	FLGPTN flgptn;

	r2y_wb.R = 400;
	r2y_wb.G = 256;
	r2y_wb.B = 480;

	Im_R2Y_Set_WB_Gain( D_IM_R2Y_PIPE1, &r2y_wb );

	r2y_wb.R = 0xFFF;
	r2y_wb.G = 0xFFF;
	r2y_wb.B = 0xFFF;

	Im_R2Y_Set_WB_Clip_Level( D_IM_R2Y_PIPE1, &r2y_wb );

	Im_R2Y_Start( D_IM_R2Y_PIPE1 );
	Im_R2Y_WaitEnd( &flgptn, (D_IM_R2Y1_INT_STATE_YYW0_END | D_IM_R2Y1_INT_STATE_YYW_ERR), 1000 );
	@endcode
	<br><br>

@section im_r2y_iq_sample_section2	Sample code of data setting for imaging.
	@code

	T_IM_R2Y_OFS r2y_ctrl_ofs;
	T_IM_R2Y_CTRL_MULTI_AXIS r2y_ctrl_multi_axis;
	T_IM_R2Y_TCOF r2y_ctrl_btc_offset;
	T_IM_R2Y_TCT r2y_ctrl_btc_tct;
	T_IM_R2Y_TCHS r2y_ctrl_btc_tchs;
	T_IM_R2Y_CTRL_TONE r2y_ctrl_tone;
	T_IM_R2Y_CTRL_GAMMA r2y_ctrl_gamma;
	T_IM_R2Y_CTRL_YNR r2y_ctrl_ynr;
	T_IM_R2Y_CTRL_EDGE_CMN r2y_ctrl_edge_cmn;
	T_IM_R2Y_CTRL_HEDGE r2y_ctrl_high_edge;
	T_IM_R2Y_CTRL_MEDGE r2y_ctrl_medium_edge;
	T_IM_R2Y_CTRL_LEDGE r2y_ctrl_low_edge;
	T_IM_R2Y_CTRL_EDGE_DOT_NOISE r2y_ctrl_edge_dot_noise;
	T_IM_R2Y_CTRL_MAPSCL r2y_ctrl_mapscl;
	T_IM_R2Y_CTRL_EDGE_BLEND r2y_ctrl_edge_blend;
	T_IM_R2Y_CTRL_C_REF_EDGE_TEXTURE_ADJ_COMMON r2y_ctrl_edge_texture;
	T_IM_R2Y_CTRL_C_REF_EDGE_ADJ r2y_ctrl_edge_adj;
	T_IM_R2Y_CTRL_C_REF_YB_BLEND r2y_ctrl_yb_blend;
	T_IM_R2Y_CTRL_CLPF r2y_ctrl_clpf;
	T_IM_R2Y_CTRL_CS r2y_ctrl_cs;

	UINT32	loop_cnt, loop_cnt2;

	const USHORT gTone_Tbl[D_IM_R2Y_TABLE_MAX_TONE] = {
		 279,  292,  305,  318,  331,  344,  357,  370,  383,  396,  409,  423,  436,  450,  463,  477,
		 491,  506,  520,  534,  548,  563,  577,  591,  605,  619,  633,  646,  659,  672,  685,  697,
		 709,  720,  731,  742,  751,  761,  770,  778,  786,  794,  801,  808,  814,  820,  826,  832,
		 837,  842,  846,  851,  855,  859,  863,  866,  870,  873,  876,  880,  883,  886,  889,  892,
		 896,  899,  902,  905,  909,  912,  915,  919,  922,  925,  929,  932,  936,  939,  943,  946,
		 950,  953,  957,  960,  963,  967,  970,  973,  977,  980,  983,  986,  989,  992,  995,  998,
		1001, 1004, 1007, 1009, 1012, 1014, 1017, 1019, 1021, 1024, 1026, 1028, 1029, 1031, 1033, 1034,
		1036, 1037, 1039, 1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1047, 1048, 1049, 1049, 1050,
		1050, 1050, 1050, 1051, 1051, 1051, 1051, 1051, 1051, 1051, 1051, 1050, 1050, 1050, 1050, 1049,
		1049, 1048, 1048, 1048, 1047, 1047, 1046, 1045, 1045, 1044, 1044, 1043, 1042, 1042, 1041, 1040,
		1040, 1039, 1038, 1037, 1037, 1036, 1035, 1035, 1034, 1033, 1032, 1032, 1031, 1030, 1030, 1029,
		1028, 1028, 1027, 1026, 1025, 1025, 1024, 1023, 1023, 1022, 1022, 1021, 1020, 1020, 1019, 1018,
		1018, 1017, 1016, 1016, 1015, 1015, 1014, 1013, 1013, 1012, 1012, 1011, 1011, 1010, 1010, 1009,
		1008, 1008, 1007, 1007, 1006, 1006, 1005, 1005, 1005, 1004, 1004, 1003, 1003, 1002, 1002, 1002,
		1001, 1001, 1000, 1000, 1000,  999,  999,  999,  998,  998,  998,  997,  997,  997,  997,  996,
		 996,  996,  996,  995,  995,  995,  995,  995,  994,  994,  994,  994,  994,  994,  993,  993,
		 993,  993,  993,  993,  993,  993,  993,  992,  992,  992,  992,  992,  992,  992,  992,  992,
		 992,  992,  992,  991,  991,  991,  991,  991,  991,  991,  991,  991,  991,  991,  990,  990,
		 990,  990,  990,  990,  990,  990,  990,  989,  989,  989,  989,  989,  989,  989,  988,  988,
		 988,  988,  988,  987,  987,  987,  987,  987,  986,  986,  986,  986,  985,  985,  985,  984,
		 984,  984,  984,  983,  983,  983,  982,  982,  982,  981,  981,  981,  980,  980,  980,  979,
		 979,  979,  978,  978,  978,  977,  977,  976,  976,  976,  975,  975,  975,  974,  974,  973,
		 973,  973,  972,  972,  972,  971,  971,  970,  970,  970,  969,  969,  969,  968,  968,  967,
		 967,  967,  966,  966,  966,  965,  965,  965,  964,  964,  964,  963,  963,  963,  962,  962,
		 962,  961,  961,  961,  960,  960,  960,  959,  959,  959,  959,  958,  958,  958,  957,  957,
		 957,  957,  956,  956,  956,  956,  956,  955,  955,  955,  955,  955,  954,  954,  954,  954,
		 954,  954,  953,  953,  953,  953,  953,  953,  953,  953,  953,  952,  952,  952,  952,  952,
		 952,  952,  952,  952,  952,  952,  952,  952,  952,  952,  952,  952,  952,  952,  952,  953,
		 953,  953,  953,  953,  953,  953,  953,  954,  954,  954,  954,  954,  955,  955,  955,  955,
		 956,  956,  956,  956,  957,  957,  957,  957,  958,  958,  958,  959,  959,  959,  960,  960,
		 960,  961,  961,  961,  962,  962,  963,  963,  963,  964,  964,  965,  965,  965,  966,  966,
		 967,  967,  968,  968,  968,  969,  969,  970,  970,  971,  971,  972,  972,  972,  973,  973,
		 974,  974,  975,  975,  976,  976,  977,  977,  978,  978,  978,  979,  979,  980,  980,  981,
		 981,  982,  982,  983,  983,  984,  984,  984,  985,  985,  986,  986,  987,  987,  988,  988,
		 988,  989,  989,  990,  990,  991,  991,  991,  992,  992,  993,  993,  994,  994,  994,  995,
		 995,  996,  996,  996,  997,  997,  997,  998,  998,  999,  999,  999, 1000, 1000, 1000, 1001,
		1001, 1002, 1002, 1002, 1003, 1003, 1003, 1004, 1004, 1004, 1005, 1005, 1005, 1006, 1006, 1006,
		1007, 1007, 1007, 1008, 1008, 1008, 1009, 1009, 1009, 1010, 1010, 1010, 1011, 1011, 1011, 1011,
		1012, 1012, 1012, 1013, 1013, 1013, 1014, 1014, 1014, 1014, 1015, 1015, 1015, 1016, 1016, 1016,
		1016, 1017, 1017, 1017, 1017, 1018, 1018, 1018, 1018, 1019, 1019, 1019, 1019, 1020, 1020, 1020,
		1020, 1021, 1021, 1021, 1021, 1022, 1022, 1022, 1022, 1022, 1023, 1023, 1023, 1023, 1024, 1024,
		1024, 1024, 1024, 1025, 1025, 1025, 1025, 1025, 1026, 1026, 1026, 1026, 1026, 1027, 1027, 1027,
		1027, 1027, 1027, 1028, 1028, 1028, 1028, 1028, 1029, 1029, 1029, 1029, 1029, 1029, 1029, 1030,
		1030, 1030, 1030, 1030, 1030, 1031, 1031, 1031, 1031, 1031, 1031, 1031, 1032, 1032, 1032, 1032,
		1032, 1032, 1032, 1033, 1033, 1033, 1033, 1033, 1033, 1033, 1033, 1033, 1034, 1034, 1034, 1034,
		1034, 1034, 1034, 1034, 1034, 1035, 1035, 1035, 1035, 1035, 1035, 1035, 1035, 1035, 1035, 1036,
		1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1037, 1037, 1037, 1037, 1037,
		1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1038, 1038, 1038, 1038,
		1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038,
		1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039,
		1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039,
		1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039, 1038, 1038,
		1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038,
		1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1038, 1037, 1037, 1037,
		1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037, 1037,
		1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036, 1036,
		1035, 1035, 1035, 1035, 1035, 1035, 1035, 1035, 1035, 1035, 1035, 1035, 1035, 1035, 1034, 1034,
		1034, 1034, 1034, 1034, 1034, 1034, 1034, 1034, 1034, 1034, 1033, 1033, 1033, 1033, 1033, 1033,
		1033, 1033, 1033, 1033, 1033, 1033, 1032, 1032, 1032, 1032, 1032, 1032, 1032, 1032, 1032, 1032,
		1032, 1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031, 1030, 1030, 1030, 1030,
		1030, 1030, 1030, 1030, 1030, 1030, 1029, 1029, 1029, 1029, 1029, 1029, 1029, 1029, 1029, 1029,
		1028, 1028, 1028, 1028, 1028, 1028, 1028, 1028, 1028, 1028, 1027, 1027, 1027, 1027, 1027, 1027,
		1027, 1027, 1027, 1026, 1026, 1026, 1026, 1026, 1026, 1026, 1026, 1026, 1026, 1025, 1025, 1025,
		1025, 1025, 1025, 1025, 1025, 1025, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024,
	};

	const USHORT gGamma_Tbl[D_IM_R2Y_TABLE_MAX_GAMMA] = {
		  0,   1,   3,   4,   5,   7,   8,   9,  10,  12,  13,  14,  15,  16,  17,  18,
		 20,  21,  21,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,
		 36,  37,  38,  38,  39,  40,  41,  42,  43,  44,  45,  46,  46,  47,  48,  49,
		 50,  51,  51,  52,  53,  54,  54,  55,  56,  57,  57,  58,  59,  60,  61,  61,
		 62,  63,  63,  64,  65,  66,  66,  67,  68,  68,  69,  70,  70,  71,  72,  73,
		 73,  74,  74,  75,  76,  76,  77,  77,  78,  79,  79,  80,  81,  81,  82,  82,
		 83,  84,  84,  85,  85,  86,  87,  87,  88,  88,  89,  89,  90,  91,  91,  92,
		 92,  93,  93,  94,  94,  95,  95,  96,  97,  97,  98,  98,  99,  99, 100, 100,
		101, 101, 102, 102, 103, 103, 104, 104, 105, 105, 106, 106, 107, 107, 108, 108,
		109, 109, 110, 110, 111, 111, 111, 112, 112, 113, 113, 114, 114, 115, 115, 116,
		116, 116, 117, 117, 118, 118, 119, 119, 120, 120, 120, 121, 121, 122, 122, 122,
		123, 123, 124, 124, 125, 125, 125, 126, 126, 127, 127, 127, 128, 128, 129, 129,
		129, 130, 130, 131, 131, 131, 132, 132, 132, 133, 133, 134, 134, 134, 135, 135,
		135, 136, 136, 137, 137, 137, 138, 138, 138, 139, 139, 139, 140, 140, 141, 141,
		141, 142, 142, 142, 143, 143, 143, 144, 144, 144, 145, 145, 145, 146, 146, 146,
		147, 147, 147, 148, 148, 148, 149, 149, 149, 150, 150, 150, 151, 151, 151, 152,
		152, 152, 153, 153, 153, 154, 154, 154, 154, 155, 155, 155, 156, 156, 156, 157,
		157, 157, 157, 158, 158, 158, 159, 159, 159, 160, 160, 160, 160, 161, 161, 161,
		162, 162, 162, 162, 163, 163, 163, 164, 164, 164, 164, 165, 165, 165, 166, 166,
		166, 166, 167, 167, 167, 167, 168, 168, 168, 168, 169, 169, 169, 170, 170, 170,
		170, 171, 171, 171, 171, 172, 172, 172, 172, 173, 173, 173, 173, 174, 174, 174,
		174, 175, 175, 175, 175, 176, 176, 176, 176, 177, 177, 177, 177, 178, 178, 178,
		178, 179, 179, 179, 179, 180, 180, 180, 180, 180, 181, 181, 181, 181, 182, 182,
		182, 182, 183, 183, 183, 183, 184, 184, 184, 184, 184, 185, 185, 185, 185, 186,
		186, 186, 186, 186, 187, 187, 187, 187, 188, 188, 188, 188, 188, 189, 189, 189,
		189, 189, 190, 190, 190, 190, 190, 191, 191, 191, 191, 191, 192, 192, 192, 192,
		192, 193, 193, 193, 193, 194, 194, 194, 194, 194, 194, 195, 195, 195, 195, 195,
		196, 196, 196, 196, 196, 197, 197, 197, 197, 197, 198, 198, 198, 198, 198, 199,
		199, 199, 199, 199, 199, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201,
		202, 202, 202, 202, 202, 203, 203, 203, 203, 203, 203, 204, 204, 204, 204, 204,
		204, 205, 205, 205, 205, 205, 206, 206, 206, 206, 206, 206, 207, 207, 207, 207,
		207, 207, 208, 208, 208, 208, 208, 208, 209, 209, 209, 209, 209, 209, 209, 210,
		210, 210, 210, 210, 210, 211, 211, 211, 211, 211, 211, 211, 212, 212, 212, 212,
		212, 212, 213, 213, 213, 213, 213, 213, 213, 214, 214, 214, 214, 214, 214, 215,
		215, 215, 215, 215, 215, 215, 216, 216, 216, 216, 216, 216, 216, 217, 217, 217,
		217, 217, 217, 217, 218, 218, 218, 218, 218, 218, 218, 219, 219, 219, 219, 219,
		219, 219, 219, 220, 220, 220, 220, 220, 220, 220, 221, 221, 221, 221, 221, 221,
		221, 222, 222, 222, 222, 222, 222, 222, 222, 223, 223, 223, 223, 223, 223, 223,
		223, 224, 224, 224, 224, 224, 224, 224, 224, 225, 225, 225, 225, 225, 225, 225,
		225, 226, 226, 226, 226, 226, 226, 226, 226, 226, 227, 227, 227, 227, 227, 227,
		227, 227, 228, 228, 228, 228, 228, 228, 228, 228, 228, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 230, 230, 230, 230, 230, 230, 230, 230, 230, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232,
		233, 233, 233, 233, 233, 233, 233, 233, 233, 234, 234, 234, 234, 234, 234, 234,
		234, 234, 234, 235, 235, 235, 235, 235, 235, 235, 235, 235, 235, 236, 236, 236,
		236, 236, 236, 236, 236, 236, 236, 236, 237, 237, 237, 237, 237, 237, 237, 237,
		237, 237, 237, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 239, 239,
		239, 239, 239, 239, 239, 239, 239, 239, 239, 240, 240, 240, 240, 240, 240, 240,
		240, 240, 240, 240, 240, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241,
		241, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 243, 243,
		243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 244, 244, 244, 244, 244,
		244, 244, 244, 244, 244, 244, 244, 244, 245, 245, 245, 245, 245, 245, 245, 245,
		245, 245, 245, 245, 245, 245, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246,
		246, 246, 246, 246, 246, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247,
		247, 247, 247, 247, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248,
		248, 248, 248, 248, 249, 249, 249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
		249, 249, 249, 249, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250,
		250, 250, 250, 250, 250, 250, 251, 251, 251, 251, 251, 251, 251, 251, 251, 251,
		251, 251, 251, 251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252, 252,
		252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 253, 253, 253, 253,
		253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253,
		253, 253, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
		254, 254, 254, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	};

	const ULLONG gGamma_Diff_Tbl[D_IM_R2Y_TABLE_MAX_GAMMA] = {
		  0,   1,   3,   4,   5,   7,   8,   9,  10,  12,  13,  14,  15,  16,  17,  18,
		 20,  21,  21,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,
		 36,  37,  38,  38,  39,  40,  41,  42,  43,  44,  45,  46,  46,  47,  48,  49,
		 50,  51,  51,  52,  53,  54,  54,  55,  56,  57,  57,  58,  59,  60,  61,  61,
		 62,  63,  63,  64,  65,  66,  66,  67,  68,  68,  69,  70,  70,  71,  72,  73,
		 73,  74,  74,  75,  76,  76,  77,  77,  78,  79,  79,  80,  81,  81,  82,  82,
		 83,  84,  84,  85,  85,  86,  87,  87,  88,  88,  89,  89,  90,  91,  91,  92,
		 92,  93,  93,  94,  94,  95,  95,  96,  97,  97,  98,  98,  99,  99, 100, 100,
		101, 101, 102, 102, 103, 103, 104, 104, 105, 105, 106, 106, 107, 107, 108, 108,
		109, 109, 110, 110, 111, 111, 111, 112, 112, 113, 113, 114, 114, 115, 115, 116,
		116, 116, 117, 117, 118, 118, 119, 119, 120, 120, 120, 121, 121, 122, 122, 122,
		123, 123, 124, 124, 125, 125, 125, 126, 126, 127, 127, 127, 128, 128, 129, 129,
		129, 130, 130, 131, 131, 131, 132, 132, 132, 133, 133, 134, 134, 134, 135, 135,
		135, 136, 136, 137, 137, 137, 138, 138, 138, 139, 139, 139, 140, 140, 141, 141,
		141, 142, 142, 142, 143, 143, 143, 144, 144, 144, 145, 145, 145, 146, 146, 146,
		147, 147, 147, 148, 148, 148, 149, 149, 149, 150, 150, 150, 151, 151, 151, 152,
		152, 152, 153, 153, 153, 154, 154, 154, 154, 155, 155, 155, 156, 156, 156, 157,
		157, 157, 157, 158, 158, 158, 159, 159, 159, 160, 160, 160, 160, 161, 161, 161,
		162, 162, 162, 162, 163, 163, 163, 164, 164, 164, 164, 165, 165, 165, 166, 166,
		166, 166, 167, 167, 167, 167, 168, 168, 168, 168, 169, 169, 169, 170, 170, 170,
		170, 171, 171, 171, 171, 172, 172, 172, 172, 173, 173, 173, 173, 174, 174, 174,
		174, 175, 175, 175, 175, 176, 176, 176, 176, 177, 177, 177, 177, 178, 178, 178,
		178, 179, 179, 179, 179, 180, 180, 180, 180, 180, 181, 181, 181, 181, 182, 182,
		182, 182, 183, 183, 183, 183, 184, 184, 184, 184, 184, 185, 185, 185, 185, 186,
		186, 186, 186, 186, 187, 187, 187, 187, 188, 188, 188, 188, 188, 189, 189, 189,
		189, 189, 190, 190, 190, 190, 190, 191, 191, 191, 191, 191, 192, 192, 192, 192,
		192, 193, 193, 193, 193, 194, 194, 194, 194, 194, 194, 195, 195, 195, 195, 195,
		196, 196, 196, 196, 196, 197, 197, 197, 197, 197, 198, 198, 198, 198, 198, 199,
		199, 199, 199, 199, 199, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201,
		202, 202, 202, 202, 202, 203, 203, 203, 203, 203, 203, 204, 204, 204, 204, 204,
		204, 205, 205, 205, 205, 205, 206, 206, 206, 206, 206, 206, 207, 207, 207, 207,
		207, 207, 208, 208, 208, 208, 208, 208, 209, 209, 209, 209, 209, 209, 209, 210,
		210, 210, 210, 210, 210, 211, 211, 211, 211, 211, 211, 211, 212, 212, 212, 212,
		212, 212, 213, 213, 213, 213, 213, 213, 213, 214, 214, 214, 214, 214, 214, 215,
		215, 215, 215, 215, 215, 215, 216, 216, 216, 216, 216, 216, 216, 217, 217, 217,
		217, 217, 217, 217, 218, 218, 218, 218, 218, 218, 218, 219, 219, 219, 219, 219,
		219, 219, 219, 220, 220, 220, 220, 220, 220, 220, 221, 221, 221, 221, 221, 221,
		221, 222, 222, 222, 222, 222, 222, 222, 222, 223, 223, 223, 223, 223, 223, 223,
		223, 224, 224, 224, 224, 224, 224, 224, 224, 225, 225, 225, 225, 225, 225, 225,
		225, 226, 226, 226, 226, 226, 226, 226, 226, 226, 227, 227, 227, 227, 227, 227,
		227, 227, 228, 228, 228, 228, 228, 228, 228, 228, 228, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 230, 230, 230, 230, 230, 230, 230, 230, 230, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232,
		233, 233, 233, 233, 233, 233, 233, 233, 233, 234, 234, 234, 234, 234, 234, 234,
		234, 234, 234, 235, 235, 235, 235, 235, 235, 235, 235, 235, 235, 236, 236, 236,
		236, 236, 236, 236, 236, 236, 236, 236, 237, 237, 237, 237, 237, 237, 237, 237,
		237, 237, 237, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 239, 239,
		239, 239, 239, 239, 239, 239, 239, 239, 239, 240, 240, 240, 240, 240, 240, 240,
		240, 240, 240, 240, 240, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241,
		241, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 242, 243, 243,
		243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 243, 244, 244, 244, 244, 244,
		244, 244, 244, 244, 244, 244, 244, 244, 245, 245, 245, 245, 245, 245, 245, 245,
		245, 245, 245, 245, 245, 245, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246,
		246, 246, 246, 246, 246, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247,
		247, 247, 247, 247, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248,
		248, 248, 248, 248, 249, 249, 249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
		249, 249, 249, 249, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250,
		250, 250, 250, 250, 250, 250, 251, 251, 251, 251, 251, 251, 251, 251, 251, 251,
		251, 251, 251, 251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252, 252,
		252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 253, 253, 253, 253,
		253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253,
		253, 253, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
		254, 254, 254, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	}

	static const UCHAR gHE_SCL_Tbl[D_IM_R2Y_TABLE_MAX_EDGE_SCALE_HI] = {
	// Plus
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206,
		 206, 206, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 204, 204,
		 204, 204, 204, 204, 204, 204, 204, 204, 204, 203, 203, 203, 203, 203, 203, 203,
		 203, 202, 202, 202, 202, 202, 202, 202, 202, 201, 201, 201, 201, 201, 201, 201,
		 200, 200, 200, 200, 200, 200, 199, 199, 199, 199, 199, 198, 198, 198, 198, 198,
		 198, 197, 197, 197, 197, 197, 196, 196, 196, 196, 195, 195, 195, 195, 195, 194,
		 194, 194, 194, 193, 193, 193, 193, 192, 192, 192, 191, 191, 191, 191, 190, 190,
		 190, 189, 189, 189, 188, 188, 188, 188, 187, 187, 186, 186, 186, 185, 185, 185,
		 184, 184, 184, 183, 183, 182, 182, 182, 181, 181, 180, 180, 179, 179, 179, 178,
		 178, 177, 177, 176, 176, 175, 175, 174, 174, 173, 173, 172, 172, 171, 170, 170,
		 169, 169, 168, 167, 167, 166, 165, 165, 164, 163, 163, 162, 161, 160, 160, 159,
		 158, 157, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144,

	// Minus
		 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142,
		 142, 143, 144, 145, 145, 146, 147, 148, 148, 149, 150, 150, 151, 152, 152, 153,
		 153, 154, 155, 155, 156, 156, 157, 157, 158, 159, 159, 160, 160, 161, 161, 162,
		 162, 162, 163, 163, 164, 164, 165, 165, 166, 166, 166, 167, 167, 168, 168, 168,
		 169, 169, 170, 170, 170, 171, 171, 171, 172, 172, 172, 173, 173, 173, 174, 174,
		 174, 175, 175, 175, 175, 176, 176, 176, 177, 177, 177, 177, 178, 178, 178, 178,
		 179, 179, 179, 179, 180, 180, 180, 180, 181, 181, 181, 181, 181, 182, 182, 182,
		 182, 182, 183, 183, 183, 183, 183, 184, 184, 184, 184, 184, 184, 185, 185, 185,
		 185, 185, 185, 186, 186, 186, 186, 186, 186, 186, 187, 187, 187, 187, 187, 187,
		 187, 188, 188, 188, 188, 188, 188, 188, 188, 188, 189, 189, 189, 189, 189, 189,
		 189, 189, 189, 189, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190,
		 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191,
		 191, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
		 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
		 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
		 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
	};

	static const USHORT gHE_STEP_Tbl[D_IM_R2Y_TABLE_MAX_EDGE_TC_HI] = {
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206,
		 206, 206, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 204, 204,
		 204, 204, 204, 204, 204, 204, 204, 204, 204, 203, 203, 203, 203, 203, 203, 203,
		 203, 202, 202, 202, 202, 202, 202, 202, 202, 201, 201, 201, 201, 201, 201, 201,
		 200, 200, 200, 200, 200, 200, 199, 199, 199, 199, 199, 198, 198, 198, 198, 198,
		 198, 197, 197, 197, 197, 197, 196, 196, 196, 196, 195, 195, 195, 195, 195, 194,
		 194, 194, 194, 193, 193, 193, 193, 192, 192, 192, 191, 191, 191, 191, 190, 190,
		 190, 189, 189, 189, 188, 188, 188, 188, 187, 187, 186, 186, 186, 185, 185, 185,
		 184, 184, 184, 183, 183, 182, 182, 182, 181, 181, 180, 180, 179, 179, 179, 178,
		 178, 177, 177, 176, 176, 175, 175, 174, 174, 173, 173, 172, 172, 171, 170, 170,
		 169, 169, 168, 167, 167, 166, 165, 165, 164, 163, 163, 162, 161, 160, 160, 159,
		 158, 157, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144,
	};

	static const UCHAR gME_SCL_Tbl[D_IM_R2Y_TABLE_MAX_EDGE_SCALE_MEDIUM] = {
	// Plus
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206,
		 206, 206, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 204, 204,
		 204, 204, 204, 204, 204, 204, 204, 204, 204, 203, 203, 203, 203, 203, 203, 203,
		 203, 202, 202, 202, 202, 202, 202, 202, 202, 201, 201, 201, 201, 201, 201, 201,
		 200, 200, 200, 200, 200, 200, 199, 199, 199, 199, 199, 198, 198, 198, 198, 198,
		 198, 197, 197, 197, 197, 197, 196, 196, 196, 196, 195, 195, 195, 195, 195, 194,
		 194, 194, 194, 193, 193, 193, 193, 192, 192, 192, 191, 191, 191, 191, 190, 190,
		 190, 189, 189, 189, 188, 188, 188, 188, 187, 187, 186, 186, 186, 185, 185, 185,
		 184, 184, 184, 183, 183, 182, 182, 182, 181, 181, 180, 180, 179, 179, 179, 178,
		 178, 177, 177, 176, 176, 175, 175, 174, 174, 173, 173, 172, 172, 171, 170, 170,
		 169, 169, 168, 167, 167, 166, 165, 165, 164, 163, 163, 162, 161, 160, 160, 159,
		 158, 157, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144,

	// Minus
		 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142,
		 142, 143, 144, 145, 145, 146, 147, 148, 148, 149, 150, 150, 151, 152, 152, 153,
		 153, 154, 155, 155, 156, 156, 157, 157, 158, 159, 159, 160, 160, 161, 161, 162,
		 162, 162, 163, 163, 164, 164, 165, 165, 166, 166, 166, 167, 167, 168, 168, 168,
		 169, 169, 170, 170, 170, 171, 171, 171, 172, 172, 172, 173, 173, 173, 174, 174,
		 174, 175, 175, 175, 175, 176, 176, 176, 177, 177, 177, 177, 178, 178, 178, 178,
		 179, 179, 179, 179, 180, 180, 180, 180, 181, 181, 181, 181, 181, 182, 182, 182,
		 182, 182, 183, 183, 183, 183, 183, 184, 184, 184, 184, 184, 184, 185, 185, 185,
		 185, 185, 185, 186, 186, 186, 186, 186, 186, 186, 187, 187, 187, 187, 187, 187,
		 187, 188, 188, 188, 188, 188, 188, 188, 188, 188, 189, 189, 189, 189, 189, 189,
		 189, 189, 189, 189, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190,
		 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191,
		 191, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
		 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
		 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
		 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
	};

	static const USHORT gME_STEP_Tbl[D_IM_R2Y_TABLE_MAX_EDGE_TC_MEDIUM] = {
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206,
		 206, 206, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 204, 204,
		 204, 204, 204, 204, 204, 204, 204, 204, 204, 203, 203, 203, 203, 203, 203, 203,
		 203, 202, 202, 202, 202, 202, 202, 202, 202, 201, 201, 201, 201, 201, 201, 201,
		 200, 200, 200, 200, 200, 200, 199, 199, 199, 199, 199, 198, 198, 198, 198, 198,
		 198, 197, 197, 197, 197, 197, 196, 196, 196, 196, 195, 195, 195, 195, 195, 194,
		 194, 194, 194, 193, 193, 193, 193, 192, 192, 192, 191, 191, 191, 191, 190, 190,
		 190, 189, 189, 189, 188, 188, 188, 188, 187, 187, 186, 186, 186, 185, 185, 185,
		 184, 184, 184, 183, 183, 182, 182, 182, 181, 181, 180, 180, 179, 179, 179, 178,
		 178, 177, 177, 176, 176, 175, 175, 174, 174, 173, 173, 172, 172, 171, 170, 170,
		 169, 169, 168, 167, 167, 166, 165, 165, 164, 163, 163, 162, 161, 160, 160, 159,
		 158, 157, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144,
	};

	static const UCHAR gLE_SCL_Tbl[D_IM_R2Y_TABLE_MAX_EDGE_SCALE_LO] = {
	// Plus
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206,
		 206, 206, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 204, 204,
		 204, 204, 204, 204, 204, 204, 204, 204, 204, 203, 203, 203, 203, 203, 203, 203,
		 203, 202, 202, 202, 202, 202, 202, 202, 202, 201, 201, 201, 201, 201, 201, 201,
		 200, 200, 200, 200, 200, 200, 199, 199, 199, 199, 199, 198, 198, 198, 198, 198,
		 198, 197, 197, 197, 197, 197, 196, 196, 196, 196, 195, 195, 195, 195, 195, 194,
		 194, 194, 194, 193, 193, 193, 193, 192, 192, 192, 191, 191, 191, 191, 190, 190,
		 190, 189, 189, 189, 188, 188, 188, 188, 187, 187, 186, 186, 186, 185, 185, 185,
		 184, 184, 184, 183, 183, 182, 182, 182, 181, 181, 180, 180, 179, 179, 179, 178,
		 178, 177, 177, 176, 176, 175, 175, 174, 174, 173, 173, 172, 172, 171, 170, 170,
		 169, 169, 168, 167, 167, 166, 165, 165, 164, 163, 163, 162, 161, 160, 160, 159,
		 158, 157, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144,

	// Minus
		 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142,
		 142, 143, 144, 145, 145, 146, 147, 148, 148, 149, 150, 150, 151, 152, 152, 153,
		 153, 154, 155, 155, 156, 156, 157, 157, 158, 159, 159, 160, 160, 161, 161, 162,
		 162, 162, 163, 163, 164, 164, 165, 165, 166, 166, 166, 167, 167, 168, 168, 168,
		 169, 169, 170, 170, 170, 171, 171, 171, 172, 172, 172, 173, 173, 173, 174, 174,
		 174, 175, 175, 175, 175, 176, 176, 176, 177, 177, 177, 177, 178, 178, 178, 178,
		 179, 179, 179, 179, 180, 180, 180, 180, 181, 181, 181, 181, 181, 182, 182, 182,
		 182, 182, 183, 183, 183, 183, 183, 184, 184, 184, 184, 184, 184, 185, 185, 185,
		 185, 185, 185, 186, 186, 186, 186, 186, 186, 186, 187, 187, 187, 187, 187, 187,
		 187, 188, 188, 188, 188, 188, 188, 188, 188, 188, 189, 189, 189, 189, 189, 189,
		 189, 189, 189, 189, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190,
		 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191,
		 191, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
		 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
		 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
		 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
	};

	static const USHORT gLE_STEP_Tbl[D_IM_R2Y_TABLE_MAX_EDGE_TC_LO] = {
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
		 207, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206,
		 206, 206, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 204, 204,
		 204, 204, 204, 204, 204, 204, 204, 204, 204, 203, 203, 203, 203, 203, 203, 203,
		 203, 202, 202, 202, 202, 202, 202, 202, 202, 201, 201, 201, 201, 201, 201, 201,
		 200, 200, 200, 200, 200, 200, 199, 199, 199, 199, 199, 198, 198, 198, 198, 198,
		 198, 197, 197, 197, 197, 197, 196, 196, 196, 196, 195, 195, 195, 195, 195, 194,
		 194, 194, 194, 193, 193, 193, 193, 192, 192, 192, 191, 191, 191, 191, 190, 190,
		 190, 189, 189, 189, 188, 188, 188, 188, 187, 187, 186, 186, 186, 185, 185, 185,
		 184, 184, 184, 183, 183, 182, 182, 182, 181, 181, 180, 180, 179, 179, 179, 178,
		 178, 177, 177, 176, 176, 175, 175, 174, 174, 173, 173, 172, 172, 171, 170, 170,
		 169, 169, 168, 167, 167, 166, 165, 165, 164, 163, 163, 162, 161, 160, 160, 159,
		 158, 157, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144,
	};

	static const USHORT gMAP_SCL_Tbl[12][16] = {
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
	};

	r2y_ctrl_ofs.R = 0;
	r2y_ctrl_ofs.G = 0;
	r2y_ctrl_ofs.B = 0;

	Im_R2Y_Set_Offset( D_IM_R2Y_PIPE1, &r2y_ctrl_ofs );

	// MCYC00~MCYC22
	r2y_ctrl_multi_axis.cyc_coeff[0] = 0x4D;
	r2y_ctrl_multi_axis.cyc_coeff[1] = 0x96;
	r2y_ctrl_multi_axis.cyc_coeff[2] = 0x1D;
	r2y_ctrl_multi_axis.cyc_coeff[3] = 0x1D5;
	r2y_ctrl_multi_axis.cyc_coeff[4] = 0x1AB;
	r2y_ctrl_multi_axis.cyc_coeff[5] = 0x80;
	r2y_ctrl_multi_axis.cyc_coeff[6] = 0x80;
	r2y_ctrl_multi_axis.cyc_coeff[7] = 0x195;
	r2y_ctrl_multi_axis.cyc_coeff[8] = 0x1EB;

	// MCB1A~MCB4D
	memset( r2y_ctrl_multi_axis.boundary, 0, 32 );

	// MCID1A~MCID4E
	for (loop_cnt = 0; loop_cnt < 4; loop_cnt++) {
		for (loop_cnt2 = 0; loop_cnt2 < 5; loop_cnt2++) {
			r2y_ctrl_multi_axis.area_index[loop_cnt][loop_cnt2] = 0;
		}
	}

	// MCKA_0_00~MCKA_4_22
	r2y_ctrl_multi_axis.area_a_k[0][0][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[0][0][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[0][0][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[0][1][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[0][1][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[0][1][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[0][2][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[0][2][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[0][2][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[1][0][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[1][0][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[1][0][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[1][1][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[1][1][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[1][1][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[1][2][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[1][2][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[1][2][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[2][0][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[2][0][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[2][0][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[2][1][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[2][1][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[2][1][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[2][2][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[2][2][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[2][2][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[3][0][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[3][0][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[3][0][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[3][1][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[3][1][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[3][1][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[3][2][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[3][2][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[3][2][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[4][0][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[4][0][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[4][0][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[4][1][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[4][1][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[4][1][2] = 0;
	r2y_ctrl_multi_axis.area_a_k[4][2][0] = 1024;
	r2y_ctrl_multi_axis.area_a_k[4][2][1] = 0;
	r2y_ctrl_multi_axis.area_a_k[4][2][2] = 0;

	// MCLA_0_00~MCLA_4_21
	r2y_ctrl_multi_axis.area_a_l[0][0][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[0][0][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[0][1][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[0][1][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[0][2][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[0][2][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[1][0][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[1][0][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[1][1][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[1][1][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[1][2][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[1][2][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[2][0][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[2][0][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[2][1][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[2][1][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[2][2][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[2][2][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[3][0][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[3][0][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[3][1][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[3][1][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[3][2][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[3][2][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[4][0][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[4][0][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[4][1][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[4][1][1] = 1024;
	r2y_ctrl_multi_axis.area_a_l[4][2][0] = 1024;
	r2y_ctrl_multi_axis.area_a_l[4][2][1] = 1024;

	// MCKB_0_00~MCKB_4_22, MCLB_0_00~MCKB_4_21
	memcpy( r2y_ctrl_multi_axis.area_b_k, r2y_ctrl_multi_axis.area_a_k, sizeof(r2y_ctrl_multi_axis.area_a_k) );
	memcpy( r2y_ctrl_multi_axis.area_b_l, r2y_ctrl_multi_axis.area_a_l, sizeof(r2y_ctrl_multi_axis.area_a_l) );

	// MCKC_0_00~MCKC_4_22, MCLC_0_00~MCKC_4_21
	memcpy( r2y_ctrl_multi_axis.area_c_k, r2y_ctrl_multi_axis.area_a_k, sizeof(r2y_ctrl_multi_axis.area_a_k) );
	memcpy( r2y_ctrl_multi_axis.area_c_l, r2y_ctrl_multi_axis.area_a_l, sizeof(r2y_ctrl_multi_axis.area_a_l) );

	// MCKD_0_00~MCKD_4_22, MCLD_0_00~MCKD_4_21
	memcpy( r2y_ctrl_multi_axis.area_d_k, r2y_ctrl_multi_axis.area_a_k, sizeof(r2y_ctrl_multi_axis.area_a_k) );
	memcpy( r2y_ctrl_multi_axis.area_d_l, r2y_ctrl_multi_axis.area_a_l, sizeof(r2y_ctrl_multi_axis.area_a_l) );

	// MCKE_0_00~MCKE_4_22, MCLE_0_00~MCKE_4_21
	memcpy( r2y_ctrl_multi_axis.area_e_k, r2y_ctrl_multi_axis.area_a_k, sizeof(r2y_ctrl_multi_axis.area_a_k) );
	memcpy( r2y_ctrl_multi_axis.area_e_l, r2y_ctrl_multi_axis.area_a_l, sizeof(r2y_ctrl_multi_axis.area_a_l) );

	// MCKF_0_00~MCKF_4_22, MCLF_0_00~MCKF_4_21
	memcpy( r2y_ctrl_multi_axis.area_f_k, r2y_ctrl_multi_axis.area_a_k, sizeof(r2y_ctrl_multi_axis.area_a_k) );
	memcpy( r2y_ctrl_multi_axis.area_f_l, r2y_ctrl_multi_axis.area_a_l, sizeof(r2y_ctrl_multi_axis.area_a_l) );

	// MCKG_0_00~MCKG_4_22, MCLG_0_00~MCKG_4_21
	memcpy( r2y_ctrl_multi_axis.area_g_k, r2y_ctrl_multi_axis.area_a_k, sizeof(r2y_ctrl_multi_axis.area_a_k) );
	memcpy( r2y_ctrl_multi_axis.area_g_l, r2y_ctrl_multi_axis.area_a_l, sizeof(r2y_ctrl_multi_axis.area_a_l) );

	// MCKH_0_00~22, MCLH_0_00~21
	memcpy( r2y_ctrl_multi_axis.area_h_k, r2y_ctrl_multi_axis.area_a_k, sizeof(r2y_ctrl_multi_axis.area_a_k) );
	memcpy( r2y_ctrl_multi_axis.area_h_l, r2y_ctrl_multi_axis.area_a_l, sizeof(r2y_ctrl_multi_axis.area_a_l) );

	// MCKI_0_00~22, MCLI_0_00~21
	memcpy( r2y_ctrl_multi_axis.area_i_k, r2y_ctrl_multi_axis.area_a_k, sizeof(r2y_ctrl_multi_axis.area_a_k) );
	memcpy( r2y_ctrl_multi_axis.area_i_l, r2y_ctrl_multi_axis.area_a_l, sizeof(r2y_ctrl_multi_axis.area_a_l) );

	// MCKJ_0_00~22, MCLJ_0_00~21
	memcpy( r2y_ctrl_multi_axis.area_j_k, r2y_ctrl_multi_axis.area_a_k, sizeof(r2y_ctrl_multi_axis.area_a_k) );
	memcpy( r2y_ctrl_multi_axis.area_j_l, r2y_ctrl_multi_axis.area_a_l, sizeof(r2y_ctrl_multi_axis.area_a_l) );

	// MCKK_0_00~22, MCLK_0_00~21
	memcpy( r2y_ctrl_multi_axis.area_k_k, r2y_ctrl_multi_axis.area_a_k, sizeof(r2y_ctrl_multi_axis.area_a_k) );
	memcpy( r2y_ctrl_multi_axis.area_k_l, r2y_ctrl_multi_axis.area_a_l, sizeof(r2y_ctrl_multi_axis.area_a_l) );

	// MCKL_0_00~22, MCLL_0_00~21
	memcpy( r2y_ctrl_multi_axis.area_l_k, r2y_ctrl_multi_axis.area_a_k, sizeof(r2y_ctrl_multi_axis.area_a_k) );
	memcpy( r2y_ctrl_multi_axis.area_l_l, r2y_ctrl_multi_axis.area_a_l, sizeof(r2y_ctrl_multi_axis.area_a_l) );

	// MCYCBALP.MCYCBALP
	r2y_ctrl_multi_axis.cyc_alpha_blend = 0;

	// MCYCBOF
	r2y_ctrl_multi_axis.cyc_blend_offset[0] = 0;
	r2y_ctrl_multi_axis.cyc_blend_offset[1] = 0;
	r2y_ctrl_multi_axis.cyc_blend_offset[2] = 0;

	// MCYCBGA
	r2y_ctrl_multi_axis.cyc_blend_gain[0] = 0;
	r2y_ctrl_multi_axis.cyc_blend_gain[1] = 0;
	r2y_ctrl_multi_axis.cyc_blend_gain[2] = 0;

	// MCYCBGA
	r2y_ctrl_multi_axis.cyc_blend_border[0] = 0;
	r2y_ctrl_multi_axis.cyc_blend_border[1] = 0;

	Im_R2Y_Ctrl_Multi_Axis( D_IM_R2Y_PIPE1, &r2y_ctrl_multi_axis );

	// BTC
	r2y_ctrl_btc_offset.R = 0;
	r2y_ctrl_btc_offset.G = 0;
	r2y_ctrl_btc_offset.B = 0;
	r2y_ctrl_btc_offset.Yb = 0;
	r2y_ctrl_btc_tct.tct_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_btc_tct.start_x = 0;
	r2y_ctrl_btc_tct.start_y = 0;
	r2y_ctrl_btc_tct.block_hsiz = 8;
	r2y_ctrl_btc_tct.block_vsiz = 8;
	r2y_ctrl_btc_tct.block_hnum = 2;
	r2y_ctrl_btc_tct.block_vnum = 2;
	r2y_ctrl_btc_tchs.hist_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_btc_tchs.sampling_hcyc = D_IM_R2Y_HIST_HCYC_4;
	r2y_ctrl_btc_tchs.sampling_vcyc = D_IM_R2Y_HIST_VCYC_1;
	r2y_ctrl_btc_tchs.histogram_mode = D_IM_R2Y_HIST_BIT_WIDTH_7_0;
	r2y_ctrl_btc_tchs.hist_minus_mode = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_btc_tchs.tchs_window.img_top = 0;
	r2y_ctrl_btc_tchs.tchs_window.img_left = 0;
	r2y_ctrl_btc_tchs.tchs_window.img_width = 2;
	r2y_ctrl_btc_tchs.tchs_window.img_lines = 1;

	Im_R2Y_Ctrl_BeforeTone_Offset( D_IM_R2Y_PIPE1, &r2y_ctrl_btc_offset );
	Im_R2Y_Ctrl_BeforeTone_Tct( D_IM_R2Y_PIPE1, &r2y_ctrl_btc_tct );
	Im_R2Y_Ctrl_BeforeTone_Tchs( D_IM_R2Y_PIPE1, &r2y_ctrl_btc_tchs );

	r2y_ctrl_tone.tone_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_tone.tone_y_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_tone.table_blend_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_tone.table_resol = D_IM_R2Y_TC_RESOLUTION_1024;
	r2y_ctrl_tone.table_select = 0;
	r2y_ctrl_tone.ytc_out = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_tone.int_bit = 0;
	r2y_ctrl_tone.table_blend_ratio = 0;
	r2y_ctrl_tone.yc_matrix[0] = 77;
	r2y_ctrl_tone.yc_matrix[1] = 150;
	r2y_ctrl_tone.yc_matrix[2] = 29;
	r2y_ctrl_tone.table_endp[0] = 0;
	r2y_ctrl_tone.table_endp[1] = 0;
	r2y_ctrl_tone.table_endp[2] = 0;
	r2y_ctrl_tone.table_endp[3] = 0;
	r2y_ctrl_tone.clip_p_r = 0xFFF;
	r2y_ctrl_tone.clip_m_r = 0xFFF;
	r2y_ctrl_tone.clip_p_g = 0xFFF;
	r2y_ctrl_tone.clip_m_g = 0xFFF;
	r2y_ctrl_tone.clip_p_b = 0xFFF;
	r2y_ctrl_tone.clip_m_b = 0xFFF;

	Im_R2Y_Ctrl_Tone( D_IM_R2Y_PIPE1, &r2y_ctrl_tone );

	r2y_ctrl_gamma.gamma_enable = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl_gamma.gamma_mode = D_IM_R2Y_GAMMA_MODE_12;
	r2y_ctrl_gamma.gamma_yb_tbl_simul = D_IM_R2Y_ENABLE_OFF;

	Im_R2Y_Ctrl_Gamma( D_IM_R2Y_PIPE1, &r2y_ctrl_gamma );

	r2y_ctrl_ynr.nr_mode = 1;
	r2y_ctrl_ynr.blend_ratio = 0;
	r2y_ctrl_ynr.offset[0] = 0;
	r2y_ctrl_ynr.offset[1] = 0;
	r2y_ctrl_ynr.offset[2] = 0;
	r2y_ctrl_ynr.offset[3] = 0;
	r2y_ctrl_ynr.gain[0] = 0;
	r2y_ctrl_ynr.gain[1] = 0;
	r2y_ctrl_ynr.gain[2] = 0;
	r2y_ctrl_ynr.gain[3] = 0;
	r2y_ctrl_ynr.border[0] = 0;
	r2y_ctrl_ynr.border[1] = 0;
	r2y_ctrl_ynr.border[2] = 0;

	Im_R2Y_Ctrl_Ynr( D_IM_R2Y_PIPE1, &r2y_ctrl_ynr );

	r2y_ctrl_edge_cmn.reduction_mode = D_IM_R2Y_EGSMT_VERSION1;
	r2y_ctrl_edge_cmn.threshold = 0x10;
	r2y_ctrl_edge_cmn.texture_priority = 0x10;

	Im_R2Y_Ctrl_Edge_NoiseReduction( D_IM_R2Y_PIPE1, &r2y_ctrl_edge_cmn );

	r2y_ctrl_high_edge.edge_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_high_edge.scale_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_high_edge.table_clip_select = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_high_edge.scale_table_resol = 0;
	r2y_ctrl_high_edge.scale_table_select = 0;
	r2y_ctrl_high_edge.gradation_table_resol = 0;
	r2y_ctrl_high_edge.gradation_table_select = 0;
	r2y_ctrl_high_edge.nr_level = 0;
	r2y_ctrl_high_edge.hpf_k[0] = 0x100;
	r2y_ctrl_high_edge.hpf_k[1] = 0xC0;
	r2y_ctrl_high_edge.hpf_k[2] = 0;
	r2y_ctrl_high_edge.hpf_k[3] = 0;
	r2y_ctrl_high_edge.hpf_k[4] = 0;
	r2y_ctrl_high_edge.hpf_k[5] = 0;
	r2y_ctrl_high_edge.corp_offset[0] = 0;
	r2y_ctrl_high_edge.corp_offset[1] = 0;
	r2y_ctrl_high_edge.corp_offset[2] = 0;
	r2y_ctrl_high_edge.corp_offset[3] = 0;
	r2y_ctrl_high_edge.corp_gain[0] = 0;
	r2y_ctrl_high_edge.corp_gain[1] = 0;
	r2y_ctrl_high_edge.corp_gain[2] = 0;
	r2y_ctrl_high_edge.corp_gain[3] = 0;
	r2y_ctrl_high_edge.corp_border[0] = 0;
	r2y_ctrl_high_edge.corp_border[1] = 0;
	r2y_ctrl_high_edge.corp_border[2] = 0;
	r2y_ctrl_high_edge.corm_offset[0] = 0;
	r2y_ctrl_high_edge.corm_offset[1] = 0;
	r2y_ctrl_high_edge.corm_offset[2] = 0;
	r2y_ctrl_high_edge.corm_offset[3] = 0;
	r2y_ctrl_high_edge.corm_gain[0] = 0;
	r2y_ctrl_high_edge.corm_gain[1] = 0;
	r2y_ctrl_high_edge.corm_gain[2] = 0;
	r2y_ctrl_high_edge.corm_gain[3] = 0;
	r2y_ctrl_high_edge.corm_border[0] = 0;
	r2y_ctrl_high_edge.corm_border[1] = 0;
	r2y_ctrl_high_edge.corm_border[2] = 0;
	r2y_ctrl_high_edge.sup_scl = 0x10;
	r2y_ctrl_high_edge.level_clip_p_offset[0] = 0;
	r2y_ctrl_high_edge.level_clip_p_offset[1] = 0;
	r2y_ctrl_high_edge.level_clip_p_offset[2] = 0x1FF;
	r2y_ctrl_high_edge.level_clip_p_gain[0] = 0;
	r2y_ctrl_high_edge.level_clip_p_gain[1] = 0;
	r2y_ctrl_high_edge.level_clip_p_gain[2] = 0;
	r2y_ctrl_high_edge.level_clip_p_border[0] = 0;
	r2y_ctrl_high_edge.level_clip_p_border[1] = 0;
	r2y_ctrl_high_edge.level_clip_m_offset[0] = 0;
	r2y_ctrl_high_edge.level_clip_m_offset[1] = 0;
	r2y_ctrl_high_edge.level_clip_m_offset[2] = 0x1FF;
	r2y_ctrl_high_edge.level_clip_m_gain[0] = 0;
	r2y_ctrl_high_edge.level_clip_m_gain[1] = 0;
	r2y_ctrl_high_edge.level_clip_m_gain[2] = 0;
	r2y_ctrl_high_edge.level_clip_m_border[0] = 0;
	r2y_ctrl_high_edge.level_clip_m_border[1] = 0;

	Im_R2Y_Ctrl_HighEdge( D_IM_R2Y_PIPE1, &r2y_ctrl_high_edge );

	r2y_ctrl_medium_edge.edge_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_medium_edge.scale_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_medium_edge.table_clip_select = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_medium_edge.scale_table_resol = 0;
	r2y_ctrl_medium_edge.scale_table_select = 0;
	r2y_ctrl_medium_edge.gradation_table_resol = 0;
	r2y_ctrl_medium_edge.gradation_table_select = 0;
	r2y_ctrl_medium_edge.nr_level = 0;
	r2y_ctrl_medium_edge.hpf_k[0] = 0x100;
	r2y_ctrl_medium_edge.hpf_k[1] = 0xC0;
	r2y_ctrl_medium_edge.hpf_k[2] = 0;
	r2y_ctrl_medium_edge.hpf_k[3] = 0;
	r2y_ctrl_medium_edge.hpf_k[4] = 0;
	r2y_ctrl_medium_edge.hpf_k[5] = 0;
	r2y_ctrl_medium_edge.corp_offset[0] = 0;
	r2y_ctrl_medium_edge.corp_offset[1] = 0;
	r2y_ctrl_medium_edge.corp_offset[2] = 0;
	r2y_ctrl_medium_edge.corp_offset[3] = 0;
	r2y_ctrl_medium_edge.corp_gain[0] = 0;
	r2y_ctrl_medium_edge.corp_gain[1] = 0;
	r2y_ctrl_medium_edge.corp_gain[2] = 0;
	r2y_ctrl_medium_edge.corp_gain[3] = 0;
	r2y_ctrl_medium_edge.corp_border[0] = 0;
	r2y_ctrl_medium_edge.corp_border[1] = 0;
	r2y_ctrl_medium_edge.corp_border[2] = 0;
	r2y_ctrl_medium_edge.corm_offset[0] = 0;
	r2y_ctrl_medium_edge.corm_offset[1] = 0;
	r2y_ctrl_medium_edge.corm_offset[2] = 0;
	r2y_ctrl_medium_edge.corm_offset[3] = 0;
	r2y_ctrl_medium_edge.corm_gain[0] = 0;
	r2y_ctrl_medium_edge.corm_gain[1] = 0;
	r2y_ctrl_medium_edge.corm_gain[2] = 0;
	r2y_ctrl_medium_edge.corm_gain[3] = 0;
	r2y_ctrl_medium_edge.corm_border[0] = 0;
	r2y_ctrl_medium_edge.corm_border[1] = 0;
	r2y_ctrl_medium_edge.corm_border[2] = 0;
	r2y_ctrl_medium_edge.sup_scl = 0x10;
	r2y_ctrl_medium_edge.level_clip_p_offset[0] = 0;
	r2y_ctrl_medium_edge.level_clip_p_offset[1] = 0;
	r2y_ctrl_medium_edge.level_clip_p_offset[2] = 0x1FF;
	r2y_ctrl_medium_edge.level_clip_p_gain[0] = 0;
	r2y_ctrl_medium_edge.level_clip_p_gain[1] = 0;
	r2y_ctrl_medium_edge.level_clip_p_gain[2] = 0;
	r2y_ctrl_medium_edge.level_clip_p_border[0] = 0;
	r2y_ctrl_medium_edge.level_clip_p_border[1] = 0;
	r2y_ctrl_medium_edge.level_clip_m_offset[0] = 0;
	r2y_ctrl_medium_edge.level_clip_m_offset[1] = 0;
	r2y_ctrl_medium_edge.level_clip_m_offset[2] = 0x1FF;
	r2y_ctrl_medium_edge.level_clip_m_gain[0] = 0;
	r2y_ctrl_medium_edge.level_clip_m_gain[1] = 0;
	r2y_ctrl_medium_edge.level_clip_m_gain[2] = 0;
	r2y_ctrl_medium_edge.level_clip_m_border[0] = 0;
	r2y_ctrl_medium_edge.level_clip_m_border[1] = 0;

	Im_R2Y_Ctrl_MediumEdge( D_IM_R2Y_PIPE1, &r2y_ctrl_medium_edge );

	r2y_ctrl_low_edge.edge_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_low_edge.scale_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_low_edge.table_clip_select = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_low_edge.scale_table_resol = 0;
	r2y_ctrl_low_edge.scale_table_select = 0;
	r2y_ctrl_low_edge.gradation_table_resol = 0;
	r2y_ctrl_low_edge.gradation_table_select = 0;
	r2y_ctrl_low_edge.hpf_k[0] = 0x100;
	r2y_ctrl_low_edge.hpf_k[1] = 0xC0;
	r2y_ctrl_low_edge.hpf_k[2] = 0;
	r2y_ctrl_low_edge.hpf_k[3] = 0;
	r2y_ctrl_low_edge.hpf_k[4] = 0;
	r2y_ctrl_low_edge.hpf_k[5] = 0;
	r2y_ctrl_low_edge.hpf_k[6] = 0;
	r2y_ctrl_low_edge.hpf_k[7] = 0;
	r2y_ctrl_low_edge.hpf_k[8] = 0;
	r2y_ctrl_low_edge.hpf_k[9] = 0;
	r2y_ctrl_low_edge.corp_offset[0] = 0;
	r2y_ctrl_low_edge.corp_offset[1] = 0;
	r2y_ctrl_low_edge.corp_offset[2] = 0;
	r2y_ctrl_low_edge.corp_offset[3] = 0;
	r2y_ctrl_low_edge.corp_gain[0] = 0;
	r2y_ctrl_low_edge.corp_gain[1] = 0;
	r2y_ctrl_low_edge.corp_gain[2] = 0;
	r2y_ctrl_low_edge.corp_gain[3] = 0;
	r2y_ctrl_low_edge.corp_border[0] = 0;
	r2y_ctrl_low_edge.corp_border[1] = 0;
	r2y_ctrl_low_edge.corp_border[2] = 0;
	r2y_ctrl_low_edge.corm_offset[0] = 0;
	r2y_ctrl_low_edge.corm_offset[1] = 0;
	r2y_ctrl_low_edge.corm_offset[2] = 0;
	r2y_ctrl_low_edge.corm_offset[3] = 0;
	r2y_ctrl_low_edge.corm_gain[0] = 0;
	r2y_ctrl_low_edge.corm_gain[1] = 0;
	r2y_ctrl_low_edge.corm_gain[2] = 0;
	r2y_ctrl_low_edge.corm_gain[3] = 0;
	r2y_ctrl_low_edge.corm_border[0] = 0;
	r2y_ctrl_low_edge.corm_border[1] = 0;
	r2y_ctrl_low_edge.corm_border[2] = 0;
	r2y_ctrl_low_edge.sup_scl = 0x10;
	r2y_ctrl_low_edge.level_clip_p_offset[0] = 0;
	r2y_ctrl_low_edge.level_clip_p_offset[1] = 0;
	r2y_ctrl_low_edge.level_clip_p_offset[2] = 0x1FF;
	r2y_ctrl_low_edge.level_clip_p_gain[0] = 0;
	r2y_ctrl_low_edge.level_clip_p_gain[1] = 0;
	r2y_ctrl_low_edge.level_clip_p_gain[2] = 0;
	r2y_ctrl_low_edge.level_clip_p_border[0] = 0;
	r2y_ctrl_low_edge.level_clip_p_border[1] = 0;
	r2y_ctrl_low_edge.level_clip_m_offset[0] = 0;
	r2y_ctrl_low_edge.level_clip_m_offset[1] = 0;
	r2y_ctrl_low_edge.level_clip_m_offset[2] = 0x1FF;
	r2y_ctrl_low_edge.level_clip_m_gain[0] = 0;
	r2y_ctrl_low_edge.level_clip_m_gain[1] = 0;
	r2y_ctrl_low_edge.level_clip_m_gain[2] = 0;
	r2y_ctrl_low_edge.level_clip_m_border[0] = 0;
	r2y_ctrl_low_edge.level_clip_m_border[1] = 0;

	Im_R2Y_Ctrl_LowEdge( D_IM_R2Y_PIPE1, &r2y_ctrl_low_edge );

	r2y_ctrl_edge_dot_noise.hf_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_edge_dot_noise.mf_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_edge_dot_noise.hf_p_threshold = 0;
	r2y_ctrl_edge_dot_noise.hf_m_threshold = 0;
	r2y_ctrl_edge_dot_noise.hf_p_coef = 0;
	r2y_ctrl_edge_dot_noise.hf_m_coef = 0;
	r2y_ctrl_edge_dot_noise.mf_p_threshold = 0;
	r2y_ctrl_edge_dot_noise.mf_m_threshold = 0;
	r2y_ctrl_edge_dot_noise.mf_p_coef = 0;
	r2y_ctrl_edge_dot_noise.mf_m_coef = 0;

	Im_R2Y_Ctrl_EdgeDotNoise( D_IM_R2Y_PIPE1, &r2y_ctrl_edge_dot_noise );

	r2y_ctrl_mapscl.block_size_h = 0x10;
	r2y_ctrl_mapscl.block_size_v = 0x10;
	r2y_ctrl_mapscl.recip_multipli_h = 0;
	r2y_ctrl_mapscl.recip_multipli_shift_h = 0;
	r2y_ctrl_mapscl.recip_multipli_v = 0;
	r2y_ctrl_mapscl.recip_multipli_shift_v = 0;
	r2y_ctrl_mapscl.block_start_h = 0;
	r2y_ctrl_mapscl.block_start_coord_h = 0;
	r2y_ctrl_mapscl.block_start_v = 0;
	r2y_ctrl_mapscl.block_start_coord_v = 0;

	Im_R2Y_Ctrl_MapScl( D_IM_R2Y_PIPE1, &r2y_ctrl_mapscl );

	memset( &r2y_ctrl_edge_blend, 0x00, sizeof(T_IM_R2Y_CTRL_EDGE_BLEND) );
	r2y_ctrl_edge_blend.blend_type = 0;
	r2y_ctrl_edge_blend.border[0] = 0;
	r2y_ctrl_edge_blend.border[1] = 0;
	r2y_ctrl_edge_blend.border[2] = 0;
	r2y_ctrl_edge_blend.alpha_offset[0] = 0;
	r2y_ctrl_edge_blend.alpha_offset[1] = 0;
	r2y_ctrl_edge_blend.alpha_offset[2] = 0;
	r2y_ctrl_edge_blend.alpha_offset[3] = 0;
	r2y_ctrl_edge_blend.alpha_gain[0] = 0;
	r2y_ctrl_edge_blend.alpha_gain[1] = 0;
	r2y_ctrl_edge_blend.alpha_gain[2] = 0;
	r2y_ctrl_edge_blend.alpha_gain[3] = 0;
	r2y_ctrl_edge_blend.beta_offset[0] = 0;
	r2y_ctrl_edge_blend.beta_offset[1] = 0;
	r2y_ctrl_edge_blend.beta_offset[2] = 0;
	r2y_ctrl_edge_blend.beta_offset[3] = 0;
	r2y_ctrl_edge_blend.beta_gain[0] = 0;
	r2y_ctrl_edge_blend.beta_gain[1] = 0;
	r2y_ctrl_edge_blend.beta_gain[2] = 0;
	r2y_ctrl_edge_blend.beta_gain[3] = 0;
	r2y_ctrl_edge_blend.level_clip_p = 0x1FF;
	r2y_ctrl_edge_blend.level_clip_m = 0x1FF;

	Im_R2Y_Ctrl_EdgeBlend( D_IM_R2Y_PIPE1, &r2y_ctrl_edge_blend );

	r2y_ctrl_edge_texture.cb_a_focus1_pos = 0;
	r2y_ctrl_edge_texture.cr_a_focus1_pos = 0;
	r2y_ctrl_edge_texture.cb_a_focus2_pos = 0;
	r2y_ctrl_edge_texture.cr_a_focus2_pos = 0;
	r2y_ctrl_edge_texture.cb_b_focus1_pos = 0;
	r2y_ctrl_edge_texture.cr_b_focus1_pos = 0;
	r2y_ctrl_edge_texture.cb_b_focus2_pos = 0;
	r2y_ctrl_edge_texture.cr_b_focus2_pos = 0;

	Im_R2Y_Ctrl_CRefEdgeTextureAdjCommon( D_IM_R2Y_PIPE1, &r2y_ctrl_edge_texture );

	r2y_ctrl_edge_adj.area_a_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_edge_adj.area_b_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_edge_adj.merge_method = 0;
	r2y_ctrl_edge_adj.area_a_scale_offset = 0x80;
	r2y_ctrl_edge_adj.area_a_scale_gain[0] = 0;
	r2y_ctrl_edge_adj.area_a_scale_gain[1] = 0;
	r2y_ctrl_edge_adj.area_a_scale_border = 0;
	r2y_ctrl_edge_adj.area_a_scale_clip_lo = 0;
	r2y_ctrl_edge_adj.area_a_scale_clip_hi = 0xFF;
	r2y_ctrl_edge_adj.area_a_y_scale_gain_p = 0;
	r2y_ctrl_edge_adj.area_a_y_scale_gain_m = 0;
	r2y_ctrl_edge_adj.area_a_y_scale_border[0] = 0;
	r2y_ctrl_edge_adj.area_a_y_scale_border[1] = 0;
	r2y_ctrl_edge_adj.area_a_y_scale_clip_lo = 0;
	r2y_ctrl_edge_adj.area_a_y_scale_clip_hi = 0x100;
	r2y_ctrl_edge_adj.area_b_scale_offset = 0x80;
	r2y_ctrl_edge_adj.area_b_scale_gain[0] = 0;
	r2y_ctrl_edge_adj.area_b_scale_gain[1] = 0;
	r2y_ctrl_edge_adj.area_b_scale_border = 0;
	r2y_ctrl_edge_adj.area_b_scale_clip_lo = 0;
	r2y_ctrl_edge_adj.area_b_scale_clip_hi = 0xFF;
	r2y_ctrl_edge_adj.area_b_y_scale_gain_p = 0;
	r2y_ctrl_edge_adj.area_b_y_scale_gain_m = 0;
	r2y_ctrl_edge_adj.area_b_y_scale_border[0] = 0;
	r2y_ctrl_edge_adj.area_b_y_scale_border[1] = 0;
	r2y_ctrl_edge_adj.area_b_y_scale_clip_lo = 0;
	r2y_ctrl_edge_adj.area_b_y_scale_clip_hi = 0x100;
	r2y_ctrl_edge_adj.level_clip_p = 0x1FF;
	r2y_ctrl_edge_adj.level_clip_m = 0x1FF;

	Im_R2Y_Ctrl_CRefEdgeAdj( D_IM_R2Y_PIPE1, &r2y_ctrl_edge_adj );

	r2y_ctrl_yb_blend.area_a_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_yb_blend.area_b_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_yb_blend.area_a_offset = 0;
	r2y_ctrl_yb_blend.area_a_gain[0] = 0;
	r2y_ctrl_yb_blend.area_a_gain[1] = 0;
	r2y_ctrl_yb_blend.area_a_border = 0;
	r2y_ctrl_yb_blend.area_a_scale_clip_lo = 0;
	r2y_ctrl_yb_blend.area_a_scale_clip_hi = 0x80;
	r2y_ctrl_yb_blend.area_a_correct_gain_p = 0;
	r2y_ctrl_yb_blend.area_a_correct_gain_m = 0;
	r2y_ctrl_yb_blend.area_a_correct_border[0] = 0;
	r2y_ctrl_yb_blend.area_a_correct_border[1] = 0;
	r2y_ctrl_yb_blend.area_a_correct_clip_lo = 0;
	r2y_ctrl_yb_blend.area_a_correct_clip_hi = 0x80;
	r2y_ctrl_yb_blend.area_b_offset = 0;
	r2y_ctrl_yb_blend.area_b_gain[0] = 0;
	r2y_ctrl_yb_blend.area_b_gain[1] = 0;
	r2y_ctrl_yb_blend.area_b_border = 0;
	r2y_ctrl_yb_blend.area_b_scale_clip_lo = 0;
	r2y_ctrl_yb_blend.area_b_scale_clip_hi = 0x80;
	r2y_ctrl_yb_blend.area_b_correct_gain_p = 0;
	r2y_ctrl_yb_blend.area_b_correct_gain_m = 0;
	r2y_ctrl_yb_blend.area_b_correct_border[0] = 0;
	r2y_ctrl_yb_blend.area_b_correct_border[1] = 0;
	r2y_ctrl_yb_blend.area_b_correct_clip_lo = 0;
	r2y_ctrl_yb_blend.area_b_correct_clip_hi = 0x80;

	Im_R2Y_Ctrl_CRefYbBlend( D_IM_R2Y_PIPE1, &r2y_ctrl_yb_blend );

	r2y_ctrl_clpf.clpf_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_clpf.interlock_blend_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_clpf.lpf_k[0] = 0;
	r2y_ctrl_clpf.lpf_k[1] = 0;
	r2y_ctrl_clpf.lpf_k[2] = 0;
	r2y_ctrl_clpf.lpf_k[3] = 0;
	r2y_ctrl_clpf.lpf_k[4] = 0;
	r2y_ctrl_clpf.lpf_k[5] = 0;
	r2y_ctrl_clpf.lpf_k[6] = 0;
	r2y_ctrl_clpf.lpf_k[7] = 0;
	r2y_ctrl_clpf.lpf_k[8] = 0;
	r2y_ctrl_clpf.lpf_ib_offset[0] = 0;
	r2y_ctrl_clpf.lpf_ib_offset[1] = 0;
	r2y_ctrl_clpf.lpf_ib_offset[2] = 0;
	r2y_ctrl_clpf.lpf_ib_offset[3] = 0;
	r2y_ctrl_clpf.lpf_ib_gain[0] = 0;
	r2y_ctrl_clpf.lpf_ib_gain[1] = 0;
	r2y_ctrl_clpf.lpf_ib_gain[2] = 0;
	r2y_ctrl_clpf.lpf_ib_gain[3] = 0;
	r2y_ctrl_clpf.lpf_ib_border[0] = 0;
	r2y_ctrl_clpf.lpf_ib_border[1] = 0;
	r2y_ctrl_clpf.lpf_ib_border[2] = 0;

	Im_R2Y_Ctrl_Color_NR( D_IM_R2Y_PIPE1, &r2y_ctrl_clpf );

	r2y_ctrl_cs.csy_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_cs.csy_mix_ratio = 8;
	r2y_ctrl_cs.csy_select_table = 0;
	r2y_ctrl_cs.csy_offset[0] = 0;
	r2y_ctrl_cs.csy_offset[1] = 0;
	r2y_ctrl_cs.csy_offset[2] = 0;
	r2y_ctrl_cs.csy_offset[3] = 0;
	r2y_ctrl_cs.csy_gain[0] = 0;
	r2y_ctrl_cs.csy_gain[1] = 0;
	r2y_ctrl_cs.csy_gain[2] = 0;
	r2y_ctrl_cs.csy_gain[3] = 0;
	r2y_ctrl_cs.csy_border[0] = 0;
	r2y_ctrl_cs.csy_border[1] = 0;
	r2y_ctrl_cs.csy_border[2] = 0;
	r2y_ctrl_cs.y_rev_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_cs.c_rev_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_cs.c_fixed_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl_cs.cb_fixed = 0;
	r2y_ctrl_cs.cr_fixed = 0;
	r2y_ctrl_cs.y_offset = 0;
	r2y_ctrl_cs.cb_offset = 0;
	r2y_ctrl_cs.cr_offset = 0;

	Im_R2Y_Ctrl_Chroma_Suppress( D_IM_R2Y_PIPE1, &r2y_ctrl_cs );


	Im_R2Y_Set_Tone_Control_Table( D_IM_R2Y_PIPE1, &gTone_Tbl, 0, D_IM_R2Y_TABLE_MAX_TONE );

	Im_R2Y_Set_Gamma_Table( D_IM_R2Y_PIPE1, 0, &gGamma_Tbl, &gGamma_Diff_Tbl );
	Im_R2Y_Set_Gamma_Table( D_IM_R2Y_PIPE1, 1, &gGamma_Tbl, &gGamma_Diff_Tbl );
	Im_R2Y_Set_Gamma_Table( D_IM_R2Y_PIPE1, 2, &gGamma_Tbl, &gGamma_Diff_Tbl );
	Im_R2Y_Set_Gamma_Table( D_IM_R2Y_PIPE1, 3, &gGamma_Tbl, &gGamma_Diff_Tbl );
	Im_R2Y_Set_Gamma_Table( D_IM_R2Y_PIPE1, 4, &gGamma_Tbl, &gGamma_Diff_Tbl );

	Im_R2Y_Set_HighEdge_Scale_Table( D_IM_R2Y_PIPE1, &gHE_SCL_Tbl, 0, D_IM_R2Y_TABLE_MAX_EDGE_SCALE_HI );
	Im_R2Y_Set_HighEdge_Step_Table( D_IM_R2Y_PIPE1, &gHE_STEP_Tbl, 0, D_IM_R2Y_TABLE_MAX_EDGE_TC_HI );

	Im_R2Y_Set_MediumEdge_Scale_Table( D_IM_R2Y_PIPE1, &gME_SCL_Tbl, 0, D_IM_R2Y_TABLE_MAX_EDGE_SCALE_MEDIUM );
	Im_R2Y_Set_MediumEdge_Step_Table( D_IM_R2Y_PIPE1, &gME_STEP_Tbl, 0, D_IM_R2Y_TABLE_MAX_EDGE_TC_MEDIUM );

	Im_R2Y_Set_LowEdge_Scale_Table( D_IM_R2Y_PIPE1, &gLE_SCL_Tbl, 0, D_IM_R2Y_TABLE_MAX_EDGE_SCALE_LO );
	Im_R2Y_Set_LowEdge_Step_Table( D_IM_R2Y_PIPE1, &gLE_STEP_Tbl, 0, D_IM_R2Y_TABLE_MAX_EDGE_TC_LO );

	Im_R2Y_Set_MapScl_Table( D_IM_R2Y_PIPE1, &gMAP_SCL_Tbl, 0, D_IM_R2Y_TABLE_MAX_MAPSCL );

	@endcode
	<br><br>


@section im_r2y_iq_sample_section3	Sample code of data setting for YC Matrix.
	@code
	T_IM_R2Y_CTRL_YCC r2y_ctrl_ycc;

	r2y_ctrl_ycc.yc_coeff[0][0] = 77;
	r2y_ctrl_ycc.yc_coeff[0][1] = 150;
	r2y_ctrl_ycc.yc_coeff[0][2] = 29;
	r2y_ctrl_ycc.yc_coeff[1][0] = -43;
	r2y_ctrl_ycc.yc_coeff[1][1] = -85;
	r2y_ctrl_ycc.yc_coeff[1][2] = 128;
	r2y_ctrl_ycc.yc_coeff[2][0] = 128;
	r2y_ctrl_ycc.yc_coeff[2][1] = -107;
	r2y_ctrl_ycc.yc_coeff[2][2] = -21;
	r2y_ctrl_ycc.y_blend_ratio = 0;
	r2y_ctrl_ycc.yb_blend_ratio = 0;

	Im_R2Y_Ctrl_Yc_Convert( D_IM_R2Y_PIPE1, &r2y_ctrl_ycc );
	@endcode
	<br><br>

@section im_r2y_iq_sample_section4	Sample code of data setting for CC Matrix.
	@code
	T_IM_R2Y_CTRL_CC0 t_r2y_ctrl_cc0;
	T_IM_R2Y_CTRL_CC1 t_r2y_ctrl_cc1;

	t_r2y_ctrl_cc0.posi_dec = 0;
	t_r2y_ctrl_cc0.cc_matrix[0][0] = 512;
	t_r2y_ctrl_cc0.cc_matrix[0][1] = 0;
	t_r2y_ctrl_cc0.cc_matrix[0][2] = 0;
	t_r2y_ctrl_cc0.cc_matrix[1][0] = 0;
	t_r2y_ctrl_cc0.cc_matrix[1][1] = 512;
	t_r2y_ctrl_cc0.cc_matrix[1][2] = 0;
	t_r2y_ctrl_cc0.cc_matrix[2][0] = 0;
	t_r2y_ctrl_cc0.cc_matrix[2][1] = 0;
	t_r2y_ctrl_cc0.cc_matrix[2][2] = 512;
	t_r2y_ctrl_cc0.cybof[0] = 0;
	t_r2y_ctrl_cc0.cybof[1] = 0;
	t_r2y_ctrl_cc0.cybof[2] = 0x400;
	t_r2y_ctrl_cc0.cybga[0] = 0;
	t_r2y_ctrl_cc0.cybga[1] = 0;
	t_r2y_ctrl_cc0.cybga[2] = 0;
	t_r2y_ctrl_cc0.cybbd[0] = 0;
	t_r2y_ctrl_cc0.cybbd[1] = 0;
	t_r2y_ctrl_cc0.cyc[0] = 77;
	t_r2y_ctrl_cc0.cyc[1] = 150;
	t_r2y_ctrl_cc0.cyc[2] = 29;

	Im_R2Y_Ctrl_CC0_Matrix( D_IM_R2Y_PIPE1, &t_r2y_ctrl_cc0 );

	t_r2y_ctrl_cc1.posi_dec = 0;
	t_r2y_ctrl_cc1.cc_matrix[0][0] = 512;
	t_r2y_ctrl_cc1.cc_matrix[0][1] = 0;
	t_r2y_ctrl_cc1.cc_matrix[0][2] = 0;
	t_r2y_ctrl_cc1.cc_matrix[1][0] = 0;
	t_r2y_ctrl_cc1.cc_matrix[1][1] = 512;
	t_r2y_ctrl_cc1.cc_matrix[1][2] = 0;
	t_r2y_ctrl_cc1.cc_matrix[2][0] = 0;
	t_r2y_ctrl_cc1.cc_matrix[2][1] = 0;
	t_r2y_ctrl_cc1.cc_matrix[2][2] = 512;
	t_r2y_ctrl_cc1.clip_p_r = 0xFFF;
	t_r2y_ctrl_cc1.clip_m_r = 0xFFF;
	t_r2y_ctrl_cc1.clip_p_g = 0xFFF;
	t_r2y_ctrl_cc1.clip_m_g = 0xFFF;
	t_r2y_ctrl_cc1.clip_p_b = 0xFFF;
	t_r2y_ctrl_cc1.clip_m_b = 0xFFF;

	Im_R2Y_Ctrl_CC1_Matrix( D_IM_R2Y_PIPE1, &t_r2y_ctrl_cc1 );
	@endcode
	<br><br>

@} */	// weakgroup im_r2y_iq

