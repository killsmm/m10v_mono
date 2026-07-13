/**
 * @file		im_b2r.h
 * @brief		Bayer to YUV comversion
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@weakgroup im_b2r
@{
	@defgroup im_b2r_iq IQ
*/
/** @} */

/**
@weakgroup im_b2r
@{
	- @ref im_b2r_sequence
	  - @ref B2R_Init
	  - @ref B2R_Set_Photo_Sample
	  - @ref B2R_Set_Video_Sample
	  - @ref B2R_Ctrl
	  - @ref B2R_Int_Handler

	- @ref im_b2r_sample
	  - @ref im_b2r_sample_section1
	  - @ref im_b2r_sample_section2
	  - @ref im_b2r_sample_section3

	<hr>
	@section im_b2r_sequence Sequence

	@section B2R_Init						Sequence of B2R initialization
	@image html im_b2r_init.png

	@section B2R_Set_Photo_Sample			Sequence of B2R set photo sample sequence
	@image html im_b2r_set_photo_sample.png

	@section B2R_Set_Video_Sample			Sequence of B2R set video sample sequence
	@image html im_b2r_set_video_sample.png

	@section B2R_Ctrl						Sequence of B2R control (each mode common control)
	@image html im_b2r_ctrl.png

	@section B2R_Int_Handler				Sequence of B2R interrupt handler (VOID type)
	@image html im_b2r_int_handler.png
@}
*/

/**
@weakgroup im_b2r_iq
@{
	- @ref im_b2r_iq_sample
	  - @ref im_b2r_iq_sample_section1
	  - @ref im_b2r_iq_sample_section2

@}
*/

#ifndef _IM_B2R_H_
#define _IM_B2R_H_


#include "driver_common.h"
#include "ddim_user_custom.h"
#include "jdsb2r.h"

#include "ddim_typedef.h"

#if 1
#ifdef CO_DEBUG_PRINT_XX_YYY
#ifndef IM_B2R_STATUS_PRINT
#define CO_DEBUG_PRINT_IM_B2R	/**< DEBUG */
#endif
#endif
#endif
#ifdef CO_DEBUG_PRINT_IM_B2R
#ifndef IM_B2R_STATUS_PRINT
#define IM_B2R_STATUS_PRINT		/**< DEBUG */
#endif
#endif


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

/** @weakgroup im_b2r
@{
*/

/* Return value of processing result */
//	D_DDIM_OK is defined with driver_common.h
#define	D_IM_B2R_PARAM_ERROR				(D_IM_B2R | D_DDIM_INPUT_PARAM_ERROR)	/**< parameter error		*/
#define	D_IM_B2R_TIMEOUT					(D_IM_B2R | D_DDIM_TIMEOUT)				/**< timeout				*/
#define	D_IM_B2R_SYSTEM_ERROR				(D_IM_B2R | D_DDIM_SYSTEM_ERROR)		/**< System call error		*/
#define	D_IM_B2R_MACRO_BUSY					(D_IM_B2R | D_DDIM_MACRO_BUSY_NG)		/**< Macro busy error		*/

/** Function Enable
*/
#define	D_IM_B2R_ENABLE_OFF					(0)						/**< Disable						*/
#define	D_IM_B2R_ENABLE_ON					(1)						/**< Enable							*/

/** Wait Enable
*/
#define	D_IM_B2R_WAIT_OFF					(0)						/**< Wait disable.					*/
#define	D_IM_B2R_WAIT_ON					(1)						/**< Wait enable.					*/

/** Pipe No
*/
#define	D_IM_B2R_PIPE1						(0)						/**< Pipe1.							*/
#define	D_IM_B2R_PIPE2						(1)						/**< Pipe2.							*/
#define	D_IM_B2R_PIPE12						(2)						/**< Pipe1&2.						*/
#define	D_IM_B2R_PIPE_MAX					(3)						/**< Max value						*/

/** Interrupt status(bit field)
*/
#define	D_IM_B2R1_INT_STATE_YBR_END			(0x00000001)			/**< YBR: Normal end				*/
#define	D_IM_B2R1_INT_STATE_YBW_END			(0x00000002)			/**< YBW: Normal end				*/
#define	D_IM_B2R1_INT_STATE_FB2R_END		(0x00000004)			/**< F_B2R Frame transfer end		*/
#define	D_IM_B2R1_INT_STATE_LINE_END		(0x00000008)			/**< YBW Line transferd				*/
#define	D_IM_B2R1_INT_STATE_YBR_ERR			(0x00000010)			/**< YBR: Transfer error			*/
#define	D_IM_B2R1_INT_STATE_YBW_ERR			(0x00000020)			/**< YBW: Transfer error			*/
#define	D_IM_B2R1_INT_STATE_AXR_ERR			(0x00000040)			/**< AXI Read channel error			*/
#define	D_IM_B2R1_INT_STATE_AXW_ERR			(0x00000080)			/**< AXI Write channel error		*/
#define	D_IM_B2R1_INT_STATE_CPERR_ERR		(0x00000100)			/**< F_B2R CPERR					*/
#define	D_IM_B2R2_INT_STATE_YBR_END			(0x00010000)			/**< YBR: Normal end				*/
#define	D_IM_B2R2_INT_STATE_YBW_END			(0x00020000)			/**< YBW: Normal end				*/
#define	D_IM_B2R2_INT_STATE_FB2R_END		(0x00040000)			/**< F_B2R Frame transfer end		*/
#define	D_IM_B2R2_INT_STATE_LINE_END		(0x00080000)			/**< YBW Line transferd				*/
#define	D_IM_B2R2_INT_STATE_YBR_ERR			(0x00100000)			/**< YBR: Transfer error			*/
#define	D_IM_B2R2_INT_STATE_YBW_ERR			(0x00200000)			/**< YBW: Transfer error			*/
#define	D_IM_B2R2_INT_STATE_AXR_ERR			(0x00400000)			/**< AXI Read channel error			*/
#define	D_IM_B2R2_INT_STATE_AXW_ERR			(0x00800000)			/**< AXI Write channel error		*/
#define	D_IM_B2R2_INT_STATE_CPERR_ERR		(0x01000000)			/**< F_B2R CPERR					*/

/** Interrupt flag for Im_B2R_WaitEnd() (bit field)
*/
#define	D_IM_B2R1_INT_FLG_YBR_END			(0x00000001)			/**< YBR: Normal end				*/
#define	D_IM_B2R1_INT_FLG_YBW_END			(0x00000002)			/**< YBW: Normal end				*/
#define	D_IM_B2R1_INT_FLG_FB2R_END			(0x00000004)			/**< F_B2R Frame transfer end		*/
#define	D_IM_B2R1_INT_FLG_LINE_END			(0x00000008)			/**< YBW Line transferd				*/
#define	D_IM_B2R1_INT_FLG_YBR_ERR			(0x00000010)			/**< YBR: Transfer error			*/
#define	D_IM_B2R1_INT_FLG_YBW_ERR			(0x00000020)			/**< YBW: Transfer error			*/
#define	D_IM_B2R1_INT_FLG_AXR_ERR			(0x00000040)			/**< AXI Read channel error			*/
#define	D_IM_B2R1_INT_FLG_AXW_ERR			(0x00000080)			/**< AXI Write channel error		*/
#define	D_IM_B2R1_INT_FLG_CPERR_ERR			(0x00000100)			/**< F_B2R CPERR					*/
#define	D_IM_B2R1_INT_FLG_ALL				(0x000001FF)			/**< All flags(PIPE1)				*/
#define	D_IM_B2R2_INT_FLG_YBR_END			(0x00010000)			/**< YBR: Normal end				*/
#define	D_IM_B2R2_INT_FLG_YBW_END			(0x00020000)			/**< YBW: Normal end				*/
#define	D_IM_B2R2_INT_FLG_FB2R_END			(0x00040000)			/**< F_B2R Frame transfer end		*/
#define	D_IM_B2R2_INT_FLG_LINE_END			(0x00080000)			/**< YBW Line transferd				*/
#define	D_IM_B2R2_INT_FLG_YBR_ERR			(0x00100000)			/**< YBR: Transfer error			*/
#define	D_IM_B2R2_INT_FLG_YBW_ERR			(0x00200000)			/**< YBW: Transfer error			*/
#define	D_IM_B2R2_INT_FLG_AXR_ERR			(0x00400000)			/**< AXI Read channel error			*/
#define	D_IM_B2R2_INT_FLG_AXW_ERR			(0x00800000)			/**< AXI Write channel error		*/
#define	D_IM_B2R2_INT_FLG_CPERR_ERR			(0x01000000)			/**< F_B2R CPERR					*/
#define	D_IM_B2R2_INT_FLG_ALL				(0x01FF0000)			/**< All flags(PIPE2)				*/
#define	D_IM_B2R_INT_FLG_ALL				(0x01FF01FF)			/**< All flags						*/

/** Error factor for Im_B2R_Get_YBR_Error_Factor() (bit field)
*/
#define	D_IM_B2R_ERROR_FACOTOR_TRMER		(0x00000001)			/**< Trimming processing error				*/
#define	D_IM_B2R_ERROR_FACOTOR_HPDDER		(0x00000002)			/**< Horizontal padding processing error	*/
#define	D_IM_B2R_ERROR_FACOTOR_WAITER		(0x00000004)			/**< WAIT control error						*/

/** Color of top pixel of Bayer data
*/
#define	D_IM_B2R_TOP_PIXEL_R				(0)						/**< Bayer top pixel is R			*/
#define	D_IM_B2R_TOP_PIXEL_B				(1)						/**< Bayer top pixel is B			*/
#define	D_IM_B2R_TOP_PIXEL_GR				(2)						/**< Bayer top pixel is Gr			*/
#define	D_IM_B2R_TOP_PIXEL_GB				(3)						/**< Bayer top pixel is Gb			*/
																		/* @YBRCTL.PIXEL					*/

/** Reading/Writing data type
*/
#define	D_IM_B2R_STL_DTYP_PACK8				(0)						/**< 8bit							*/
#define	D_IM_B2R_STL_DTYP_PACK10			(1)						/**< 10bit PACK						*/
#define	D_IM_B2R_STL_DTYP_PACK12			(2)						/**< 12bit PACK						*/
#define	D_IM_B2R_STL_DTYP_PACK16			(3)						/**< 16bit							*/
																		/* @YBRCTL.YBRDTYP	*/

/** Transfer data output
*/
#define	D_IM_B2R_YBW_OUTPUT_RGB				(0)						/**< RGB data output				*/
#define	D_IM_B2R_YBW_OUTPUT_R_ONLY			(1)						/**< R data only output				*/

/** YBW burst length
*/
#define	D_IM_B2R_BRST_512					(0)						/**< 512bytes burst					*/
#define	D_IM_B2R_BRST_256					(1)						/**< 256bytes burst					*/
#define	D_IM_B2R_BRST_128					(2)						/**< 128bytes burst					*/
																		/* @YBRAXMD.YBRBL, YBWAXMD.YBWBL	*/

/** Bayer data left shift
*/
#define D_IM_B2R_BLSFT_THROUGH				(0)						/**< Through */
#define D_IM_B2R_BLSFT_1BIT					(1)						/**< Bayer data is shifted to the left by 1bit(11bits->12bits) */
#define D_IM_B2R_BLSFT_2BIT					(2)						/**< Bayer data is shifted to the left by 2bit(10bits->12bits) */
#define D_IM_B2R_BLSFT_4BIT					(3)						/**< Bayer data is shifted to the left by 4bit( 8bits->12bits) */
																		/* @BLSFT.BLSFT	*/

/** External output start alarm select
*/
#define	D_IM_B2R_EXT_OUT_ALARM_1LINE		(0)						/**< Turned on before about 1 line transfer time	*/
#define	D_IM_B2R_EXT_OUT_ALARM_2LINE		(1)						/**< Turned on before about 2 line transfer time	*/
#define	D_IM_B2R_EXT_OUT_ALARM_3LINE		(2)						/**< Turned on before about 3 line transfer time	*/
#define	D_IM_B2R_EXT_OUT_ALARM_VALID_DATA	(3)						/**< Timing of receiving the valid data(B2B direct)<br>
																		 Timing of YBRTRG start(SDRAM input)			*/

/** B2R start type
*/
#define D_IM_B2R_START_WITH_YBW				(0)						/**< To output RGB data to the external memory		*/
#define D_IM_B2R_START_EXTERNAL_IF_ONLY		(1)						/**< Not to output RGB data to the external memory	*/

/** Maximum of Table size
*/
// JDSB2R
#define D_IM_B2R_TABLE_MAX_BAYER_DEKNEE		(256)					/**< JDSB2R Bayer Deknee table array number	*/
#define D_IM_B2R_TABLE_MAX_RGB_KNEE			(257)					/**< JDSB2R RGB Knee table array number		*/

/** RGB component to be processed
*/
#define D_IM_B2R_PORT_0						(0)						/**< Port0						*/
#define D_IM_B2R_PORT_1						(1)						/**< Port1						*/
#define D_IM_B2R_PORT_2						(2)						/**< Port2						*/
#define D_IM_B2R_PORT_MAX					(3)						/**< The maximum values of RGB component to be processed */
// RGB
#define	D_IM_B2R_PORT_R						(D_IM_B2R_PORT_0)		/**< R data						*/
#define	D_IM_B2R_PORT_G						(D_IM_B2R_PORT_1)		/**< G data						*/
#define	D_IM_B2R_PORT_B						(D_IM_B2R_PORT_2)		/**< B data						*/

/** YBW output bank number
*/
#define D_IM_B2R_YBW_BANK_0					(0)						/**< YBW output bank index number 0 */
#define D_IM_B2R_YBW_BANK_1					(1)						/**< YBW output bank index number 1 */
#define D_IM_B2R_YBW_BANK_2					(2)						/**< YBW output bank index number 2 */
#define D_IM_B2R_YBW_BANK_3					(3)						/**< YBW output bank index number 3 */
#define D_IM_B2R_YBW_BANK_MAX				(4)						/**< Maximum output bank index number of YBW */

// Output Address array maximum number
#define D_IM_B2R_ADDR_INDEX_MAX				(D_IM_B2R_YBW_BANK_MAX)	/**< Maximum number of B2R output address information(bank0 ~ 3)	*/

/** Color difference reference high-frequency correction area A/B merge method
*/
#define D_IM_B2R_HFACRV_MRG_AVERAGE			(0)						/**< AB result average mode */
#define D_IM_B2R_HFACRV_MRG_SUPPRESSION		(1)						/**< AB suppression result priority mode */
#define D_IM_B2R_HFACRV_MRG_ENHANCEMENT		(2)						/**< AB enhancement result priority mode */

/* @} */	// im_b2r group

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

/** RGB table type.
 */
typedef enum {
	E_B2R_RGBTBL_R = 0,		/**< R color table. */
	E_B2R_RGBTBL_G,			/**< G color table. */
	E_B2R_RGBTBL_B,			/**< B color table. */
	E_B2R_RGBTBL_MAX,		/**< Maximum value. */
} E_B2R_RGBTBL;

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
/** @weakgroup im_b2r
@{
*/

/** AXI bus I/F control
*/
typedef struct {
	UCHAR					cache_type;						/**< cache type (4bits)		*/
	UCHAR					protect_type;					/**< protect type (3bits)	*/
} T_IM_B2R_CTRL_AXI_TYPE;

/** AXI bus I/F control
*/
typedef struct {
	T_IM_B2R_CTRL_AXI_TYPE	ybr;							/**< YBRAXCTL.YBRCACHE, YBRAXCTL.YBRPROT YBR Read channel types.		*/
	T_IM_B2R_CTRL_AXI_TYPE	ybw[D_IM_B2R_PORT_MAX];			/**< YBWAXCTL.YBWCACHE_x, YBWAXCTL.YBWPROT_x YBW Write channel types.	*/
} T_IM_B2R_CTRL_AXI;

/** AXI bus read channel status
*/
typedef struct {
	UCHAR					ybr_axi_stat;					/**< YBRAXSTS.YBRRESP : Read channel status	*/
} T_IM_B2R_AXI_YBR_STAT;

/** AXI bus write channel status
*/
typedef struct {
	UCHAR					ybw_axi_stat[D_IM_B2R_PORT_MAX];	/**< YBWAXSTS.YBBRESP[3] : Write channel status	*/
} T_IM_B2R_AXI_YBW_STAT;

/** AXI bus write channel control
*/
typedef struct {
	UCHAR					burst_length;					/**< YBWAXMD.YBWBL : Transfer burst length (2bit)
															<ul><li>@ref D_IM_B2R_BRST_512
																<li>@ref D_IM_B2R_BRST_256
																<li>@ref D_IM_B2R_BRST_128</ul>									*/
	UCHAR					out_sel;						/**< YBWAXMD.YBWDOS : Select the data to be output from YBW (1bit)
															<ul><li>@ref D_IM_B2R_YBW_OUTPUT_RGB : RGB data output
																<li>@ref D_IM_B2R_YBW_OUTPUT_R_ONLY : R data only output</ul>	*/
} T_IM_B2R_AXI_YBW_MODE;

/** Rectangle of image
*/
typedef struct {
	USHORT					img_top;						/**< Vertical start position	*/
	USHORT					img_left;						/**< Horizontal start position	*/
	USHORT					img_width;						/**< Horizontal size			*/
	USHORT					img_lines;						/**< Vertical size				*/
} T_IM_B2R_IN_RECT;								/* Rectangle control data	*/

/** B2R Window size
*/
typedef struct {
	INT32					output_global_w;				/**< YBWDEF : Output Horizontal global bytes (17 bits signed) */
} T_IM_B2R_WINDOW_SIZE;

/** YBW control
*/
typedef struct {
	T_IM_B2R_AXI_YBW_MODE	axi_write_mode;							/**< AXI bus write channel control parameters			*/
	UCHAR					output_dtype;							/**< YBWCTL.YBWDTYP : Output data type (2bits)
																	<ul><li>@ref D_IM_B2R_STL_DTYP_PACK8
																		<li>@ref D_IM_B2R_STL_DTYP_PACK12
																		<li>@ref D_IM_B2R_STL_DTYP_PACK16</ul>				*/
	USHORT					write_request_mask[D_IM_B2R_PORT_MAX];	/**< YBWREQMSK.YBWREQMSK[3] :
																		YBW transfer request mask (12bits)<br>
																		<p>Please set number of cycles, the time until the next access request.<br>
																		   If you set the "000h" you is disabled.			*/
	UCHAR					trim_out_enable;						/**< YBWMD.TRMEN : Trimming output enable
																	<ul><li>@ref D_IM_B2R_ENABLE_OFF : Do not do the trimming.
																		<li>@ref D_IM_B2R_ENABLE_ON : Trimming.</ul>		*/
	UCHAR					knee_enable;							/**< YBWMD.KNEEN : Knee enable (1bit)
																	<ul><li>@ref D_IM_B2R_ENABLE_OFF : Do not process Knee
																		<li>@ref D_IM_B2R_ENABLE_ON : Processing Knee</ul>	*/
} T_IM_B2R_YBW_CTRL;

/** YBW Line interrupt control
*/
typedef struct {
	USHORT					count;							/**< ELNLV : Monitor line count setting.
															<p>We generate the interrupt request flag when the set value of this register.
															<p>This interrupt request, can be disabled by setting the "0000" h.	*/
} T_IM_B2R_CTRL_LINE_INTR;

/** B2R control
*/
typedef struct {
	// YBR read channel control
	UCHAR					bayer_top_pixel;				/**< YBRCTL.PIXEL : Color of Bayer top pixel (2bits)
															<ul><li>@ref D_IM_B2R_TOP_PIXEL_R
																<li>@ref D_IM_B2R_TOP_PIXEL_B
																<li>@ref D_IM_B2R_TOP_PIXEL_GR
																<li>@ref D_IM_B2R_TOP_PIXEL_GB</ul>	*/
	UCHAR					deknee_enable;					/**< YBRCTL.DKNEN : Deknee enable for Bayer data. (1bit)
															<ul><li>@ref D_IM_B2R_ENABLE_OFF : Do not process Deknee
																<li>@ref D_IM_B2R_ENABLE_ON : Processing Deknee</ul>	*/

	// YBW write channel control
	T_IM_B2R_YBW_CTRL		ybw;							/**< YBW control parameters													*/
	T_IM_B2R_CTRL_LINE_INTR	line_intr;						/**< YBW Line interrupt control parameters									*/
	UCHAR					ybw_continue_start_enable;		/**< YBWCTL.YBWCSE : YBW continue start enable (1bit)
															<ul><li>@ref D_IM_B2R_ENABLE_OFF : Frame processing one-shot.
																	(YBWTRG will need to restart each process one frame.)
																<li>@ref D_IM_B2R_ENABLE_ON : Continuous frame processing.
																	(I continue to frame processing writing "00" to YBWTRG.)</ul>		*/

	/* B2R control */
	UCHAR					cia_bypass;						/**< B2RMODE.CIABYP : Color interpolation bypass mode of the F_B2R (1bit)
															<ul><li>@ref D_IM_B2R_ENABLE_OFF : F_B2R color interpolation unit is set to normal mode.
																<li>@ref D_IM_B2R_ENABLE_ON : F_B2R color interpolation unit is set to bypass mode.</ul>	*/
	UCHAR					cia_padding;					/**< B2RMODE.CIAPDD : Color interpolation padding mode of the F_B2R (1bit)
															<ul><li>@ref D_IM_B2R_ENABLE_OFF : Ring pixel padding is OFF.
																<li>@ref D_IM_B2R_ENABLE_ON : Ring pixel padding is ON.</ul>			*/
	UCHAR					ext_out_en;						/**< DINSEL.EXTOEN : RGB data external output enable (1bit)
															<ul><li>@ref D_IM_B2R_ENABLE_OFF : Output to the outside is disabled.
																<li>@ref D_IM_B2R_ENABLE_ON : Output to the outside is enabled.</ul>	*/
	UCHAR					ext_out_alarm;					/**< EXALMSL : External output start alarm select (2bits)
															<ul><li>@ref D_IM_B2R_EXT_OUT_ALARM_1LINE
																<li>@ref D_IM_B2R_EXT_OUT_ALARM_2LINE
																<li>@ref D_IM_B2R_EXT_OUT_ALARM_3LINE
																<li>@ref D_IM_B2R_EXT_OUT_ALARM_VALID_DATA</ul>							*/

	VOID					(*b2r_user_handler)(UINT32* int_status, UINT32 user_param);	/**< Interrupt Handler														*/
	UINT32					user_param;						/**< This set value will return to callback argument value when interrupt occurs.	*/
} T_IM_B2R_CTRL;								/* B2R control	*/

/** B2R SDRAM input mode control
*/
typedef struct {
	UCHAR					burst_length;					/**< YBRAXMD.YBRBL : Transfer burst length of the read channel. (2bit)
															<ul><li>@ref D_IM_B2R_BRST_512
																<li>@ref D_IM_B2R_BRST_256
																<li>@ref D_IM_B2R_BRST_128</ul>							*/
	USHORT					read_request_mask;				/**< YBRREQMSK.YBRREQMSK : Mask transfer request(12bits)<br>
																<p>Mask setting of the transfer request from the read to do YBR.<br>
																Please set number of cycles, the time until the next access request.<br>
																If you set the 000h you is disabled.					*/
	UCHAR					input_dtype;					/**< YBRCTL.YBRDTYP : Input data type (2bits)
															<ul><li>@ref D_IM_B2R_STL_DTYP_PACK8
																<li>@ref D_IM_B2R_STL_DTYP_PACK10
																<li>@ref D_IM_B2R_STL_DTYP_PACK12
																<li>@ref D_IM_B2R_STL_DTYP_PACK16</ul>					*/
	UCHAR					top_offset;						/**< YBROFS : Setting top bit input data area (3bits)<br>
															<p>Set the first bit of the data area is read YBR.
															   Set the position of the LSB bit in bit position at the time of each data format.<br>
															   However, when you select 8-bit / 16-bit in input_dtype(YBRDTYP) is always 000b.
															   Only when you select a 12-bit pack, please select the setting of 000b or 100b.
															   Only when you select a 10-bit pack, please select the setting of 000b or 010b or 100b or 110b.
															<p>This setting is valid only when the input mode SDRAM.
															<p>exp)<br>
															   I would like the following figure, data and each are stored in 12-bit pack.<br>
															   Assuming that the first pixel data3 at this time,
															   since the position of the LSB and bit 4 data3 address, I set 100b.
															   @image html im_b2r_ybrofs_exp.png
															*/
	USHORT					input_global;					/**< YBRDEF : Input horizontal Gobal area size				*/
	USHORT					line_space;						/**< YBRHBLNK : Transfer line spacing adjustment. (10bits)<br>
															<p>Sets the number of operating clock cycle of this macro,
															   the interval between the start of the next line YBR transfer is output from the
															   transfer end of each line (see figure below).<br>
															   When setting 0d, this function is disabled.
																@image html im_b2r_ybrhblnk_exp.png						*/
} T_IM_B2R_CTRL_SDRAM_INPUT;					/* B2R SDRAM input mode control	*/

/** B2R B2B direct connection mode control
*/
typedef struct {
	USHORT					even_line_start_timming;		/**< DINELS : Odd line transfer start timming (13bits)<br>
															<p>For the even-numbered data lines data to be transferred from the B2B macro,
															   it is set to adjust the timing to start to be transferred to F_B2R.					*/
	UCHAR					frame_stop;						/**< DINCTL.FRMSTP : B2R stop when next frame<br>
															<p>If you want to stop at the frame boundaries of YBR transfer process is set to "1".	*/
} T_IM_B2R_CTRL_B2B_DIRECT;						/* B2R B2B direct connection mode control	*/

/** Rectangle control
*/
typedef struct {
	T_IM_B2R_IN_RECT		input_size;						/**< input image size(and trimming)	*/
	T_IM_B2R_WINDOW_SIZE	output_size;					/**< output image size				*/
} T_IM_B2R_RECT;

/** Trimming control
*/
typedef struct {
	UCHAR					trimming_enable;				/**< YBWMD.TRMEN : Trimming enable (1bit)
															<ul><li>@ref D_IM_B2R_ENABLE_OFF : Do not do the trimming
																<li>@ref D_IM_B2R_ENABLE_ON : Trimming</ul>			*/
	USHORT					start_x;						/**< YBWTRMH.TRMHSTA : start X position (13bits)		*/
	USHORT					start_y;						/**< YBWTRMV.TRMVSTA : start Y position (14bits)		*/
	USHORT					width;							/**< YBWTRMH.TRMHSIZ : Width (13bits, multiple of 2)	*/
	USHORT					lines;							/**< YBWTRMV.TRMVSIZ : Lines (14bits, multiple of 2)	*/
} T_IM_B2R_CTRL_TRIMMING;

/** RAW address information
*/
typedef struct {
	VOID*					addr;								/**< RAW address	*/
} T_IM_B2R_ADDR_RAW_INFO;						/**< RAW address information	*/

/** RGB address information
*/
typedef struct {
	VOID*					addr_R;							/**< R address	*/
	VOID*					addr_G;							/**< G address	*/
	VOID*					addr_B;							/**< B address	*/
} T_IM_B2R_ADDR_RGB_INFO;

/** YBR input address information
*/
typedef union {
	T_IM_B2R_ADDR_RAW_INFO	raw;							/**< RAW address information	*/
} T_IM_B2R_INADDR_INFO;

/** YBW output address information
*/
typedef union {
	T_IM_B2R_ADDR_RGB_INFO	rgb;							/**< RGB address information	*/
} T_IM_B2R_OUTADDR_INFO;

/** YBW output bank control
*/
typedef struct {
	UCHAR					bank_initial_position;				/**< YBWBANK.BANKPS : Bank initial position
																<ul><li>@ref D_IM_B2R_YBW_BANK_0
																	<li>@ref D_IM_B2R_YBW_BANK_1
																	<li>@ref D_IM_B2R_YBW_BANK_2
																	<li>@ref D_IM_B2R_YBW_BANK_3</ul>					*/
	UCHAR					use_bank_num;						/**< YBWCTL.YBWUSEB : Use bank number<br>
																	Use only one bank in the setting of 0 (d),
																	and use of the four banks in the setting of 3 (d).	*/
	T_IM_B2R_OUTADDR_INFO	output_addr[D_IM_B2R_YBW_BANK_MAX];	/**< Output bank address information					*/
} T_IM_B2R_OUTBANK_INFO;

/* @} */	// im_b2r group


/** @weakgroup im_b2r_iq
@{
*/


/** Offset control of Bayer data
*/
typedef struct {
	SHORT					R;								/**< OFSTRR : Control of R pixel (10bits signed)	*/
	SHORT					B;								/**< OFSTBB : Control of B pixel (10bits signed)	*/
	SHORT					Gr;								/**< OFSTGR : Control of Gb pixel (10bits signed)	*/
	SHORT					Gb;								/**< OFSTGB : Control of Gb pixel (10bits signed)	*/
} T_IM_B2R_OFS;									/**< Offset control data	*/

/** Color control of Bayer data
*/
typedef struct {
	USHORT					R;								/**< WBGARR (11bits) / WBSLPRR (12bits) / WBLV (12bits) : Control of R pixel	*/
	USHORT					B;								/**< WBGABB (11bits) / WBSLPBB (12bits) / WBLV (12bits) : Control of B pixel	*/
	USHORT					Gr;								/**< WBGAGR (11bits) / WBSLPGR (12bits) / WBLV (12bits) : Control of Gb pixel	*/
	USHORT					Gb;								/**< WBGAGB (11bits) / WBSLPGB (12bits) / WBLV (12bits) : Control of Gb pixel	*/
} T_IM_B2R_BAYER_COLOR;							/**< Bayer array control data	*/

/** Sensitivity control
*/
typedef struct {
	UCHAR					sharp_level_a;					/**< SHARP.SHARPA/B/C : Resolving feeling parameter A (4bits 0 ~ 8)		*/
	UCHAR					sharp_level_b;					/**< SHARP.SHARPA/B/C : Resolving feeling parameter B (4bits 0 ~ 8)		*/
	UCHAR					sharp_level_c;					/**< SHARP.SHARPA/B/C : Resolving feeling parameter C (4bits 0 ~ 8)		*/
	UCHAR					adj_feel_a;						/**< AIRESY.AIRESYA : Total Resolving feeling (4bits 1 ~ 12)			*/
	UCHAR					adj_feel_b;						/**< AIRESY.AIRESYB : Total Resolving feeling (8bits) 					*/
	UCHAR					lf_strength;					/**< LF.LFSTR : L filter strength (5bits)								*/
	USHORT					lf_threshold;					/**< LF.LFTH : L filter threshold (10bits) 								*/
	UCHAR					cmc_level;						/**< CMC.CMCSCL : Strength of pretended color control (3bits 0 ~ 4)		*/
	USHORT					cmc_limit;						/**< CMC.CMCLMT : Limit of Pretended color control (10bits) 			*/
	USHORT					dir_reliab_threshold;			/**< DIRRLITH : Direction reliability threshold (12bits) 				*/
} T_IM_B2R_CTRL_ADIP;							/**< Adaptive Interpolation control	*/

/** Color difference reference high-frequency component correction control
*/
typedef struct {
	INT8					ellipse_area_focus1_x;			/**< HFACVAFX.HFACRVAFX1, HFACVBFX.HFACRVBFX1 : X(cb) coordinate of color difference ellipse area focus 1. (8bits signed)			*/
	INT8					ellipse_area_focus1_y;			/**< HFACVAFX.HFACRVAFY1, HFACVBFX.HFACRVBFY1 : Y(cr) coordinate of color difference ellipse area focus 1. (8bits signed)			*/
	INT8					ellipse_area_focus2_x;			/**< HFACVAFX.HFACRVAFX2, HFACVBFX.HFACRVBFX2 : X(cb) coordinate of color difference ellipse area focus 2. (8bits signed)			*/
	INT8					ellipse_area_focus2_y;			/**< HFACVAFX.HFACRVAFY2, HFACVBFX.HFACRVBFY2 : Y(cr) coordinate of color difference ellipse area focus 2. (8bits signed)			*/
	UCHAR					c_area_scale_offset;			/**< HFACRVASCLOF, HFACRVBSCLOF : Color difference reference high-frequency component correction area scale offset 1. (8bits)		*/
	SHORT					c_area_scale_gain[2];			/**< HFACRVASCLGA, HFACRVBSCLGA : Color difference reference high-frequency component correction area scale gain. (11bits signed)	*/
	UCHAR					c_area_scale_border;			/**< HFACRVASCLBD, HFACRVBSCLBD : Color difference reference high-frequency component correction area scale boudary. (7bits)		*/
	UCHAR					c_area_scale_clip_l;			/**< HFACRVASCLCP.HFACRVASCLCPL, HFACRVBSCLCP.HFACRVBSCLCPL : Color difference reference high-frequency component correction area scale lower limit clip. (8bits)	*/
	UCHAR					c_area_scale_clip_h;			/**< HFACRVASCLCP.HFACRVASCLCPH, HFACRVBSCLCP.HFACRVBSCLCPH : Color difference reference high-frequency component correction area scale upper limit clip. (8bits)	*/
	UCHAR					y_area_scale_gain[2];			/**< HFAYASCLGA, HFAYBSCLGA : Correction area luminance scale correction gain. (8bits)												*/
	USHORT					y_area_scale_border[2];			/**< HFAYASCLBD, HFAYBSCLBD : Correction area luminance scale correction boudary. (12bits)											*/
	USHORT					y_area_scale_clip_l;			/**< HFAYASCLCP.HFAYASCLCPL, HFAYBSCLCP.HFAYBSCLCPL : Correction area luminance scale correction lower limit clip. (9bits)			*/
	USHORT					y_area_scale_clip_h;			/**< HFAYASCLCP.HFAYASCLCPH, HFAYBSCLCP.HFAYBSCLCPH : Correction area luminance scale correction upper limit clip. (9bits)			*/
} T_IM_B2R_CTRL_CRV;							/**< Color difference reference high-frequency component correction control	*/

/** High pass filter control
*/
typedef struct {
	UCHAR					hpf_enable;						/**< HFACTL.HFEEN : High pass filter enable.										*/
	UCHAR					flat_ctrl_enable;				/**< HFACTL.HFAFEN : Flat control enable.											*/
	UCHAR					y_ctrl_enable;					/**< HFACTL.HFAYEN : Luminence control enable.										*/
	SHORT					hpf_k[10];						/**< HFK : High pass filter coefficient K0~K9.
															<ul><li>HFK0 : 10bits unsigned.
																<li>HFK1 : 10bits signed.
																<li>HFK2 : 12bits signed.
																<li>HFK3 :  8bits signed.
																<li>HFK4 : 10bits signed.
																<li>HFK5 : 10bits signed.
																<li>HFK6 :  8bits signed.
																<li>HFK7 :  8bits signed.
																<li>HFK8 :  6bits signed.
																<li>HFK9 :  6bits signed.</ul>													*/
	USHORT					hpf_level_clip_p;				/**< HFEFCLP.HFEFCLPP :  Level clip plus value. (9bits)								*/
	USHORT					hpf_level_clip_m;				/**< HFEFCLP.HFEFCLPM :  Level clip minus value. (9bits)							*/
	UCHAR					flat_scale;						/**< HFAFST.HFAFSL : Flat control scale. (4bits)									*/
	USHORT					flat_threshold;					/**< HFAFST.HFAFTH : Flat control threshold. (12bits)								*/
	USHORT					flat_gain_limit;				/**< HFAFGU.HFAFGU : Flat control gain upper limit. (12bits)						*/
	USHORT					flat_level_clip_p;				/**< HFAFCLP.HFAFCLPP :  Flat control level clip plus value. (9bits)				*/
	USHORT					flat_level_clip_m;				/**< HFAFCLP.HFAFCLPM :  Flat control level clip minus value. (9bits)				*/
	SHORT					y_convert_coef[9];				/**< HFAYC : Y convert coefficient.
															<ul><li>HFAYCC00 : 8bits unsigned.
																<li>HFAYCC01 : 8bits unsigned.
																<li>HFAYCC02 : 8bits unsigned.
																<li>HFAYCC10 : 9bits signed.
																<li>HFAYCC11 : 9bits signed.
																<li>HFAYCC12 : 9bits signed.
																<li>HFAYCC20 : 9bits signed.
																<li>HFAYCC21 : 9bits signed.
																<li>HFAYCC22 : 9bits signed.</ul>												*/
	USHORT					y_ctrl_offset[4];				/**< HFAYOF : Y control offset. (12bits)											*/
	INT8					y_ctrl_gain[4];					/**< HFAYGA : Y control gain. (8bits signed)										*/
	USHORT					y_ctrl_border[4];				/**< HFAYBD : Y control border. (12bits)											*/
	UCHAR					y_ctrl_gain_posi_dec1;			/**< HFAYDIV.HFAYDIV1 : Position of decimals for Y control gain. (3bits)			*/
	UCHAR					y_ctrl_gain_posi_dec2;			/**< HFAYDIV.HFAYDIV2 : Position of decimals for Y control output. (4bits 5 ~ 12)	*/
	USHORT					y_ctrl_level_clip_p;			/**< HFAYCLP.HFAYCLPP : Y control level clip plus value. (9bits)					*/
	USHORT					y_ctrl_level_clip_m;			/**< HFAYCLP.HFAYCLPM : Y control level clip minus value. (9bits)					*/
	UCHAR					y_ctrl_cor_p;					/**< HFACOR.HFACORP : Y control coring plus value. (6bits)							*/
	UCHAR					y_ctrl_cor_m;					/**< HFACOR.HFACORM : Y control coring minus value. (6bits)							*/
	USHORT					hpf_add_level_clip_r;			/**< HFACLP.HFACLPR : High pass filter output level clip R color value. (12bits)	*/
	USHORT					hpf_add_level_clip_g;			/**< HFACLP.HFACLPG : High pass filter output level clip G color value. (12bits)	*/
	USHORT					hpf_add_level_clip_b;			/**< HFACLP.HFACLPB : High pass filter output level clip B color value. (12bits)	*/

	UCHAR					area_a_enable;					/**< HFACRVCTL.HFACAE : Color difference reference high-frequency component correction area A enable. (1bits)	*/
	UCHAR					area_b_enable;					/**< HFACRVCTL.HFACBE : Color difference reference high-frequency component correction area B enable. (1bits)	*/
	UCHAR					area_marge_method;				/**< HFACRVCTL.HFACRVMRG : Color difference reference high-frequency component correction area a/B merge method. (2bits)
															<ul><li>@ref D_IM_B2R_HFACRV_MRG_AVERAGE
																<li>@ref D_IM_B2R_HFACRV_MRG_SUPPRESSION
																<li>@ref D_IM_B2R_HFACRV_MRG_ENHANCEMENT</ul>									*/
	T_IM_B2R_CTRL_CRV		area_a_ctrl;					/**< Color difference reference high-frequency component correction area A control	*/
	T_IM_B2R_CTRL_CRV		area_b_ctrl;					/**< Color difference reference high-frequency component correction area B control	*/
} T_IM_B2R_CTRL_HPF;							/**< High pass filter control	*/

/* @} */	// im_b2r_iq group

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
//------------ for RDMA ----------------
/** Deknee Table register address info (for RDMA)
*/
typedef struct {
	ULONG	DKNTBL[128];			/**< 2948_(2000 - 21FFh) */
} T_IM_B2R_CTRL_RDMA_DEKNEE_TBL_ADDR;

/** Knee Table(R/G/B) register address info (for RDMA)
*/
typedef struct {
	ULONG	KNETBL[65];				/**< 2948_(2800 - 2903h) */
									/**< 2948_(2A00 - 2B03h) */
									/**< 2948_(2C00 - 2D03h) */
} T_IM_B2R_CTRL_RDMA_KNEE_TBL_ADDR;

/** Offset control register address info (for RDMA)
*/
typedef struct {
	ULONG	OFSTRB;					/**< 2841_(2040 - 2043h) */
	ULONG	OFSTG;					/**< 2841_(2044 - 2047h) */
} T_IM_B2R_CTRL_RDMA_OFST_ADDR;

/** WB slope control register address info (for RDMA)
*/
typedef struct {
	ULONG	WBHCTL;					/**< 2841_(2054 - 2057h) */
	ULONG	WBSLP1;					/**< 2841_(2058 - 205Bh) */
	ULONG	WBSLP2;					/**< 2841_(205C - 205Fh) */
} T_IM_B2R_CTRL_RDMA_WB_SLOPE_ADDR;

/** WB clip control register address info (for RDMA)
*/
typedef struct {
	ULONG	WBLVRB;					/**< 2841_(2060 - 2063h) */
	ULONG	WBLVGR;					/**< 2841_(2064 - 2067h) */
} T_IM_B2R_CTRL_RDMA_WB_CLIP_ADDR;

/** Sensitivity control register address info (for RDMA)
*/
typedef struct {
	ULONG	SHARP;					/**< 2841_(2080 - 2083h) */
	ULONG	AIRESY;					/**< 2841_(2084 - 2087h) */
	ULONG	LF;						/**< 2841_(2088 - 208Bh) */
	ULONG	CMC;					/**< 2841_(208C - 208Fh) */
	ULONG	DIRRLITH;				/**< 2841_(2090 - 2093h) */
} T_IM_B2R_CTRL_RDMA_SENSITIVITY_ADDR;

/** High pass filter control register address info (for RDMA)
*/
typedef struct {
	ULONG	HFACTL;					/**< 2841_(2100 - 2103h) */
	ULONG	HFK1;					/**< 2841_(2110 - 2113h) */
	ULONG	HFK2;					/**< 2841_(2114 - 2117h) */
	ULONG	HFK3;					/**< 2841_(2118 - 211Bh) */
	ULONG	HFK4;					/**< 2841_(211C - 211Fh) */
	ULONG	HFEFCLP;				/**< 2841_(2120 - 2123h) */
	ULONG	HFAFST;					/**< 2841_(2124 - 2127h) */
	ULONG	HFAFGU;					/**< 2841_(2128 - 212Bh) */
	ULONG	HFAFCLP;				/**< 2841_(212C - 212Fh) */
	ULONG	HFAYC1;					/**< 2841_(2130 - 2133h) */
	ULONG	HFAYC2;					/**< 2841_(2134 - 2137h) */
	ULONG	HFAYC3;					/**< 2841_(2138 - 213Bh) */
	ULONG	HFAYC4;					/**< 2841_(213C - 213Fh) */
	ULONG	HFAYOF1;				/**< 2841_(2140 - 2143h) */
	ULONG	HFAYOF2;				/**< 2841_(2144 - 2147h) */
	ULONG	HFAYGA;					/**< 2841_(2148 - 214Bh) */
	ULONG	HFAYBD1;				/**< 2841_(2150 - 2153h) */
	ULONG	HFAYBD2;				/**< 2841_(2154 - 2157h) */
	ULONG	HFAYDIV;				/**< 2841_(2158 - 215Bh) */
	ULONG	HFAYCLP;				/**< 2841_(215C - 215Fh) */
	ULONG	HFACOR;					/**< 2841_(2160 - 2163h) */
	ULONG	HFACLP1;				/**< 2841_(2168 - 216Bh) */
	ULONG	HFACLP2;				/**< 2841_(216C - 216Fh) */
	ULONG	HFACRVCTL;				/**< 2841_(2170 - 2173h) */
	ULONG	HFACVAFX;				/**< 2841_(2174 - 2177h) */
	ULONG	HFACRVASCLOF;			/**< 2841_(2178 - 217Bh) */
	ULONG	HFACRVASCLGA;			/**< 2841_(217C - 217Fh) */
	ULONG	HFACRVASCLBD;			/**< 2841_(2180 - 2183h) */
	ULONG	HFACRVASCLCP;			/**< 2841_(2184 - 2187h) */
	ULONG	HFAYASCLGA;				/**< 2841_(2188 - 218Bh) */
	ULONG	HFAYASCLBD;				/**< 2841_(218C - 218Fh) */
	ULONG	HFAYASCLCLP;			/**< 2841_(2190 - 2193h) */
	ULONG	HFACVBFX;				/**< 2841_(2194 - 2197h) */
	ULONG	HFACRVBSCLOF;			/**< 2841_(2198 - 219Bh) */
	ULONG	HFACRVBSCLGA;			/**< 2841_(219C - 219Fh) */
	ULONG	HFACRVBSCLBD;			/**< 2841_(21A0 - 21A3h) */
	ULONG	HFACRVBSCLCP;			/**< 2841_(21A4 - 21A7h) */
	ULONG	HFAYBSCLGA;				/**< 2841_(21A8 - 21ABh) */
	ULONG	HFAYBSCLBD;				/**< 2841_(21AC - 21AFh) */
	ULONG	HFAYBSCLCLP;			/**< 2841_(21B0 - 21B3h) */
} T_IM_B2R_CTRL_RDMA_HPF_ADDR;



/** Deknee Table register value info (for RDMA)
*/
typedef struct {
	union	io_b2r_sram_dkntbl	DKNTBL;			/**< 2948_(2000 - 21FFh) */
} T_IM_B2R_CTRL_RDMA_DEKNEE_TBL_VAL;

/** Knee Table(R/G/B) register value info (for RDMA)
*/
typedef union {
	union	io_b2r_sram_knetblr	KNETBLR;		/**< 2948_(2800 - 2900h) */
	union	io_b2r_sram_knetblg	KNETBLG;		/**< 2948_(2A00 - 2B00h) */
	union	io_b2r_sram_knetblb	KNETBLB;		/**< 2948_(2C00 - 2D00h) */
} U_IM_B2R_CTRL_RDMA_KNEE_TBL_VAL;

/** Offset control register value info (for RDMA)
*/
typedef struct {
	union	io_b2r_ofstrb		OFSTRB;			/**< 2841_(2040 - 2043h) */
	union	io_b2r_ofstg		OFSTG;			/**< 2841_(2044 - 2047h) */
} T_IM_B2R_CTRL_RDMA_OFST_VAL;

/** WB slope control register value info (for RDMA)
*/
typedef struct {
	union	io_b2r_wbhctl		WBHCTL;			/**< 2841_(2054 - 2057h) */
	union	io_b2r_wbslp		WBSLP;			/**< 2841_(2058 - 205Fh) */
} T_IM_B2R_CTRL_RDMA_WB_SLOPE_VAL;

/** WB clip control register value info (for RDMA)
*/
typedef struct {
	union	io_b2r_wblvrb		WBLVRB;			/**< 2841_(2060 - 2063h) */
	union	io_b2r_wblvgr		WBLVGR;			/**< 2841_(2064 - 2067h) */
} T_IM_B2R_CTRL_RDMA_WB_CLIP_VAL;

/** Sensitivity control register value info (for RDMA)
*/
typedef struct {
	union	io_b2r_sharp		SHARP;			/**< 2841_(2080 - 2083h) */
	union	io_b2r_airesy		AIRESY;			/**< 2841_(2084 - 2087h) */
	union	io_b2r_lf			LF;				/**< 2841_(2088 - 208Bh) */
	union	io_b2r_cmc			CMC;			/**< 2841_(208C - 208Fh) */
	union	io_b2r_dirrlith		DIRRLITH;		/**< 2841_(2090 - 2093h) */
} T_IM_B2R_CTRL_RDMA_SENSITIVITY_VAL;

/** High pass filter control register value info (for RDMA)
*/
typedef struct {
	union	io_b2r_hfactl		HFACTL;			/**< 2841_(2100 - 2103h) */
	union	io_b2r_hfk			HFK;			/**< 2841_(2110 - 211Fh) */
	union	io_b2r_hfefclp		HFEFCLP;		/**< 2841_(2120 - 2123h) */
	union	io_b2r_hfafst		HFAFST;			/**< 2841_(2124 - 2127h) */
	union	io_b2r_hfafgu		HFAFGU;			/**< 2841_(2128 - 212Bh) */
	union	io_b2r_hfafclp		HFAFCLP;		/**< 2841_(212C - 212Fh) */
	union	io_b2r_hfayc		HFAYC;			/**< 2841_(2130 - 213Fh) */
	union	io_b2r_hfayof		HFAYOF;			/**< 2841_(2140 - 2147h) */
	union	io_b2r_hfayga		HFAYGA;			/**< 2841_(2148 - 214Bh) */
	union	io_b2r_hfaybd		HFAYBD;			/**< 2841_(2150 - 2157h) */
	union	io_b2r_hfaydiv		HFAYDIV;		/**< 2841_(2158 - 215Bh) */
	union	io_b2r_hfayclp		HFAYCLP;		/**< 2841_(215C - 215Fh) */
	union	io_b2r_hfacor		HFACOR;			/**< 2841_(2160 - 2163h) */
	union	io_b2r_hfaclp		HFACLP;			/**< 2841_(2168 - 216Fh) */
	union	io_b2r_hfacrvctl	HFACRVCTL;		/**< 2841_(2170 - 2173h) */
	union	io_b2r_hfacvafx		HFACVAFX;		/**< 2841_(2174 - 2177h) */
	union	io_b2r_hfacrvasclof	HFACRVASCLOF;	/**< 2841_(2178 - 217Bh) */
	union	io_b2r_hfacrvasclga	HFACRVASCLGA;	/**< 2841_(217C - 217Fh) */
	union	io_b2r_hfacrvasclbd	HFACRVASCLBD;	/**< 2841_(2180 - 2183h) */
	union	io_b2r_hfacrvasclcp	HFACRVASCLCP;	/**< 2841_(2184 - 2187h) */
	union	io_b2r_hfayasclga	HFAYASCLGA;		/**< 2841_(2188 - 218Bh) */
	union	io_b2r_hfayasclbd	HFAYASCLBD;		/**< 2841_(218C - 218Fh) */
	union	io_b2r_hfayasclclp	HFAYASCLCLP;	/**< 2841_(2190 - 2193h) */
	union	io_b2r_hfacvbfx		HFACVBFX;		/**< 2841_(2194 - 2197h) */
	union	io_b2r_hfacrvbsclof	HFACRVBSCLOF;	/**< 2841_(2198 - 219Bh) */
	union	io_b2r_hfacrvbsclga	HFACRVBSCLGA;	/**< 2841_(219C - 219Fh) */
	union	io_b2r_hfacrvbsclbd	HFACRVBSCLBD;	/**< 2841_(21A0 - 21A3h) */
	union	io_b2r_hfacrvbsclcp	HFACRVBSCLCP;	/**< 2841_(21A4 - 21A7h) */
	union	io_b2r_hfaybsclga	HFAYBSCLGA;		/**< 2841_(21A8 - 21ABh) */
	union	io_b2r_hfaybsclbd	HFAYBSCLBD;		/**< 2841_(21AC - 21AFh) */
	union	io_b2r_hfaybsclclp	HFAYBSCLCLP;	/**< 2841_(21B0 - 21B3h) */
} T_IM_B2R_CTRL_RDMA_HPF_VAL;
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

/** @weakgroup im_b2r
@{
*/

/**
Enable PCLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_On_Pclk( UCHAR pipe_no );

/**
Disable PCLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_Off_Pclk( UCHAR pipe_no );

/**
Force disable PCLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_Force_Off_Pclk( UCHAR pipe_no );

/**
Enable HCLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_On_Hclk( UCHAR pipe_no );

/**
Disable HCLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_Off_Hclk( UCHAR pipe_no );

/**
Force disable HCLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_Force_Off_Hclk( UCHAR pipe_no );

/**
Enable ICLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_On_Iclk( UCHAR pipe_no );

/**
Disable ICLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_Off_Iclk( UCHAR pipe_no );

/**
Force disable ICLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_Force_Off_Iclk( UCHAR pipe_no );

/**
Enable B2RCLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_On_Clk( UCHAR pipe_no );

/**
Disable B2RCLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_Off_Clk( UCHAR pipe_no );

/**
Force disable B2RCLK for B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_B2R_Force_Off_Clk( UCHAR pipe_no );

/**
B2R Initialize
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Init( UCHAR pipe_no );

/**
set AXI bus I/F Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		b2r_ctrl_axi			: Control information for AXI bus.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Ctrl_Axi( UCHAR pipe_no, const T_IM_B2R_CTRL_AXI* const b2r_ctrl_axi );

/**
get AXI bus read channel status
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		b2r_axi_read_stat		: Status of AXI bus read channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Get_AxiReadStat( UCHAR pipe_no, T_IM_B2R_AXI_YBR_STAT* const b2r_axi_read_stat );

/**
get AXI bus write channel status
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		b2r_axi_write_stat		: Status of AXI bus write channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Get_AxiWriteStat( UCHAR pipe_no, T_IM_B2R_AXI_YBW_STAT* const b2r_axi_write_stat );

/**
get AXI bus write channel Transfer mode
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		b2r_axi_write_mode		: Transfer mode of AXI bus write channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Get_AxiWriteMode( UCHAR pipe_no, T_IM_B2R_AXI_YBW_MODE* const b2r_axi_write_mode );

/**
get AXI bus write channel Transfer mode
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		b2r_axi_write_mode		: Transfer mode of AXI bus write channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_AxiWriteMode( UCHAR pipe_no, const T_IM_B2R_AXI_YBW_MODE* const b2r_axi_write_mode );

/**
B2R Control (each mode common control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		b2r_ctrl				: Control information for Common.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Ctrl( UCHAR pipe_no, const T_IM_B2R_CTRL* const b2r_ctrl );

/**
B2R Control (photo mode control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		b2r_ctrl_sdram_input	: Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@retval			D_IM_B2R_MACRO_BUSY		: Macro busy error.
*/
extern INT32 Im_B2R_Ctrl_ModeSDRAMInput( UCHAR pipe_no, const T_IM_B2R_CTRL_SDRAM_INPUT* const b2r_ctrl_sdram_input );

/**
Set Managed Input Address information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		in_addr					: Input address information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_InAddr_Info( UCHAR pipe_no, const T_IM_B2R_INADDR_INFO* const in_addr );

/**
Set Managed bank information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[in]		b2r_bank				: Output bank information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_OutBankInfo( UCHAR pipe_no, const T_IM_B2R_OUTBANK_INFO* const b2r_bank );

/**
Get bank index value
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		bank_index				: Current bank index
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Get_OutBankIndex( UCHAR pipe_no, UCHAR* const bank_index );

/**
Set bank index value
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		bank_index				: Output bank index.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_OutBankIndex( UCHAR pipe_no, UCHAR bank_index );

/**
Increment bank index value
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Inc_OutBankIndex( UCHAR pipe_no );

/**
B2R Control (B2B direct control mode control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		b2r_ctrl_b2b_direct		: Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@retval			D_IM_B2R_MACRO_BUSY		: Macro busy error.
*/
extern INT32 Im_B2R_Ctrl_ModeB2BDirect( UCHAR pipe_no, const T_IM_B2R_CTRL_B2B_DIRECT* const b2r_ctrl_b2b_direct );

/**
B2R pixel average reductionm rectangle Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		b2r_rect_param			: Rectangle Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_Rect( UCHAR pipe_no, const T_IM_B2R_RECT* const b2r_rect_param );

/**
B2R get rectangle Control information.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		b2r_rect_param			: Rectangle Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Get_Rect_Param( UCHAR pipe_no, T_IM_B2R_RECT* const b2r_rect_param );

/**
Get Latest Address information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		latest_bank_idx			: Pointer of Output address information.
@param[out]		latest_addr				: Pointer of Output address information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Get_Latest_OutAddr( UCHAR pipe_no, UCHAR* const latest_bank_idx, T_IM_B2R_OUTADDR_INFO* const latest_addr );

/**
Set YBW External I/F output.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[in]		ext_out_en				: 0:disable External I/F output  1:enable External I/F output
@param[in]		ext_out_alarm			: External output start alarm select
											<ul><li>@ref D_IM_B2R_EXT_OUT_ALARM_1LINE
												<li>@ref D_IM_B2R_EXT_OUT_ALARM_2LINE
												<li>@ref D_IM_B2R_EXT_OUT_ALARM_3LINE
												<li>@ref D_IM_B2R_EXT_OUT_ALARM_VALID_DATA</ul>
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_ExternalIfOutput( UCHAR pipe_no, const UCHAR ext_out_en, const UCHAR ext_out_alarm );

/**
Get YBW External I/F output.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		ext_out_en				: 0:disable External I/F output  1:enable External I/F output
@param[out]		ext_out_alarm			: External output start alarm select
											<ul><li>@ref D_IM_B2R_EXT_OUT_ALARM_1LINE
												<li>@ref D_IM_B2R_EXT_OUT_ALARM_2LINE
												<li>@ref D_IM_B2R_EXT_OUT_ALARM_3LINE
												<li>@ref D_IM_B2R_EXT_OUT_ALARM_VALID_DATA</ul>
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Get_ExternalIfOutput( UCHAR pipe_no, UCHAR* const ext_out_en, UCHAR* const ext_out_alarm );

/**
Set Frame stop
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		onoff					: 0 : Frame stop disable / 1 : Frame stop enable
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_FrameStop( UCHAR pipe_no, UCHAR onoff );

/**
Get YBRch busy status.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		busy_status				: TRUE : YBW is running. (YBWTRG is on). / FALSE : YBW is not running. (YBWTRG is off).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Get_YbrBusy( UCHAR pipe_no, BOOL* const busy_status );

/**
Get Horizontal ring pixel padding number
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		ring_pixs				: Ring pixel padding number
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Get_HRingPixs( UCHAR pipe_no, USHORT* const ring_pixs );

/**
Restart B2R process when ybw_continue_start_enable equal D_IM_B2R_ENABLE_OFF.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_ContStart( UCHAR pipe_no );

/**
start B2R process.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		start_type				: start type.
										<ul><li>@ref D_IM_B2R_START_WITH_YBW
											<li>@ref D_IM_B2R_START_EXTERNAL_IF_ONLY</ul>
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_SYSTEM_ERROR	: systemcall error.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@retval			D_IM_B2R_MACRO_BUSY		: Macro busy error.
@remarks		This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_B2R_Start( UCHAR pipe_no, UCHAR start_type );

/**
Stop B2R
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Stop( UCHAR pipe_no );

/**
wait end of B2R macro.
@param[in]		waiptn					: waiting end cause. (bit field)
										<ul><li>@ref D_IM_B2R1_INT_FLG_YBR_END
											<li>@ref D_IM_B2R1_INT_FLG_YBW_END
											<li>@ref D_IM_B2R1_INT_FLG_FB2R_END
											<li>@ref D_IM_B2R1_INT_FLG_LINE_END
											<li>@ref D_IM_B2R1_INT_FLG_YBR_ERR
											<li>@ref D_IM_B2R1_INT_FLG_YBW_ERR
											<li>@ref D_IM_B2R1_INT_FLG_AXR_ERR
											<li>@ref D_IM_B2R1_INT_FLG_AXW_ERR
											<li>@ref D_IM_B2R1_INT_FLG_CPERR_ERR
											<li>@ref D_IM_B2R2_INT_FLG_YBR_END
											<li>@ref D_IM_B2R2_INT_FLG_YBW_END
											<li>@ref D_IM_B2R2_INT_FLG_FB2R_END
											<li>@ref D_IM_B2R2_INT_FLG_LINE_END
											<li>@ref D_IM_B2R2_INT_FLG_YBR_ERR
											<li>@ref D_IM_B2R2_INT_FLG_YBW_ERR
											<li>@ref D_IM_B2R2_INT_FLG_AXR_ERR
											<li>@ref D_IM_B2R2_INT_FLG_AXW_ERR
											<li>@ref D_IM_B2R2_INT_FLG_CPERR_ERR</ul>
@param[in]		tmout					: timeout.
@param[out]		p_flgptn				: end cause. (bit field)
										<ul><li>@ref D_IM_B2R1_INT_FLG_YBR_END
											<li>@ref D_IM_B2R1_INT_FLG_YBW_END
											<li>@ref D_IM_B2R1_INT_FLG_FB2R_END
											<li>@ref D_IM_B2R1_INT_FLG_LINE_END
											<li>@ref D_IM_B2R1_INT_FLG_YBR_ERR
											<li>@ref D_IM_B2R1_INT_FLG_YBW_ERR
											<li>@ref D_IM_B2R1_INT_FLG_AXR_ERR
											<li>@ref D_IM_B2R1_INT_FLG_AXW_ERR
											<li>@ref D_IM_B2R1_INT_FLG_CPERR_ERR
											<li>@ref D_IM_B2R2_INT_FLG_YBR_END
											<li>@ref D_IM_B2R2_INT_FLG_YBW_END
											<li>@ref D_IM_B2R2_INT_FLG_FB2R_END
											<li>@ref D_IM_B2R2_INT_FLG_LINE_END
											<li>@ref D_IM_B2R2_INT_FLG_YBR_ERR
											<li>@ref D_IM_B2R2_INT_FLG_YBW_ERR
											<li>@ref D_IM_B2R2_INT_FLG_AXR_ERR
											<li>@ref D_IM_B2R2_INT_FLG_AXW_ERR
											<li>@ref D_IM_B2R2_INT_FLG_CPERR_ERR</ul>
@retval			D_DDIM_OK				: B2R normal end. and set end cause to p_flgptn.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
@retval			D_IM_B2R_SYSTEM_ERROR	: systemcall error.
@retval			D_IM_B2R_TIMEOUT		: wait timeout.
@remarks		This API uses DDIM_User_Clr_Flg().
@remarks		This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Im_B2R_WaitEnd( DDIM_USER_FLGPTN* const p_flgptn, DDIM_USER_FLGPTN waiptn, DDIM_USER_TMO tmout );

/**
Interrupt handler.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern VOID Im_B2R_Int_Handler( UCHAR pipe_no );

/** Get YBR error factor.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		err_factor				: error factor. (bit field)
										<ul><li>@ref D_IM_B2R_ERROR_FACOTOR_TRMER
											<li>@ref D_IM_B2R_ERROR_FACOTOR_HPDDER
											<li>@ref D_IM_B2R_ERROR_FACOTOR_WAITER</ul>
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Get_YBR_Error_Factor( UCHAR pipe_no, UINT32* const err_factor );

/**
Trimming control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		b2r_ctrl_trimming		: pointer of trimming control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Ctrl_Trimming( UCHAR pipe_no, const T_IM_B2R_CTRL_TRIMMING* const b2r_ctrl_trimming );

/** Set Deknee table.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_B2R_TABLE_MAX_BAYER_DEKNEE)
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_MACRO_BUSY		: Macro busy error.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_Deknee_Table( UCHAR pipe_no, const USHORT* const src_tbl );

/**
Set Deknee access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		access_enable			: set enable/disable Deknee table access control parameters.
@param[in]		wait_enable				: D_IM_B2R_WAIT_OFF : Turn off wait. / D_IM_B2R_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_MACRO_BUSY		: Macro busy error.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_DekneeAccessEnable( UCHAR pipe_no, UCHAR access_enable, UCHAR wait_enable );

/** Set RGB Knee table.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tbl_type				: Table type (R, G, B).
@param[in]		src_tbl					: Pointer of Table data (Type of UCHAR, Maximum array number is D_IM_B2R_TABLE_MAX_RGB_KNEE)
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_MACRO_BUSY		: Macro busy error.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_Knee_Table( UCHAR pipe_no, E_B2R_RGBTBL tbl_type, const UCHAR* const src_tbl );

/**
Set Knee access enable
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		access_enable			: set enable/disable Deknee table access control parameters.
@param[in]		wait_enable				: D_IM_B2R_WAIT_OFF : Turn off wait. / D_IM_B2R_WAIT_ON : Wait 1usec if needed.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_MACRO_BUSY		: Macro busy error.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_RGB_KneeAccessEnable( UCHAR pipe_no, UCHAR access_enable, UCHAR wait_enable );

/* @} */	// weakgroup im_b2r



/** @weakgroup im_b2r_iq
@{
*/

/**
Offset Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		ofs						: pointer of offset parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_Offset( UCHAR pipe_no, const T_IM_B2R_OFS* const ofs );

/**
Bayer left shift control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		bayer_left_shift		: Bayer left shift bit.
										<ul><li>@ref D_IM_B2R_BLSFT_THROUGH
											<li>@ref D_IM_B2R_BLSFT_1BIT
											<li>@ref D_IM_B2R_BLSFT_2BIT
											<li>@ref D_IM_B2R_BLSFT_4BIT</ul>
@param[in]		saturation_mode			: D_IM_B2R_ENABLE_OFF : saturation off. / D_IM_B2R_ENABLE_ON : saturation on.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Ctrl_Left_Shift( UCHAR pipe_no, UCHAR bayer_left_shift, UCHAR saturation_mode );

/**
WB Gain control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		bay_color				: pointer of WB gain parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_WB_Gain( UCHAR pipe_no, const T_IM_B2R_BAYER_COLOR* const bay_color );

/**
WB Gain Saturation compensation control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		slope_gain_enable		: WB gain saturation compensation enable.
@param[in]		b2r_bay_color			: pointer of WB gain parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_WB_Slope_Gain( UCHAR pipe_no, UCHAR slope_gain_enable, const T_IM_B2R_BAYER_COLOR* const b2r_bay_color );

/**
WB Clip Level control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		b2r_bay_color			: pointer of WB clip level parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Set_WB_Clip_Level( UCHAR pipe_no, const T_IM_B2R_BAYER_COLOR* const b2r_bay_color );

/**
Sensitivity(ADIP) Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		b2r_ctrl_adip			: pointer of Sensitivity(ADIP) control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Ctrl_Sensitivity( UCHAR pipe_no, const T_IM_B2R_CTRL_ADIP* const b2r_ctrl_adip );

/** High pass filter control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		b2r_ctrl_hpf			: pointer of High pass filter control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_B2R_Ctrl_HighPassFilter( UCHAR pipe_no, const T_IM_B2R_CTRL_HPF* const b2r_ctrl_hpf );

/* @} */	// weakgroup im_b2r_iq

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
/**
Get the top address of the address array of Deknee table.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Deknee table
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_B2R_Get_RdmaAddr_Deknee_Table( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_DEKNEE_TBL_ADDR** addr );

/**
Get the top address of the address array of Knee table.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tbl_type				: Table type (R, G, B).
@param[out]		addr					: Top address of the address array of Knee table(R)
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_B2R_Get_RdmaAddr_Knee_Table( UCHAR pipe_no, E_B2R_RGBTBL tbl_type, const T_IM_B2R_CTRL_RDMA_KNEE_TBL_ADDR** addr );

/**
Get the top address of the address array of Offset control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of offset control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_B2R_Get_RdmaAddr_Offset_Cntl( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_OFST_ADDR** addr );

/**
Get the top address of the address array of WB slope control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of WB slope control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_B2R_Get_RdmaAddr_WB_Slope_Cntl( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_WB_SLOPE_ADDR** addr );

/**
Get the top address of the address array of WB clip control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of WB clip control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_B2R_Get_RdmaAddr_WB_Clip_Cntl( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_WB_CLIP_ADDR** addr );

/**
Get the top address of the address array of Sensitivity control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of Sensitivity control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_B2R_Get_RdmaAddr_Sensitivity_Cntl( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_SENSITIVITY_ADDR** addr );

/**
Get the top address of the address array of High pass filter control.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		addr					: Top address of the address array of High pass filter control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_B2R_Get_RdmaAddr_HPF_Cntl( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_HPF_ADDR** addr );
#endif	// CO_DDIM_UTILITY_USE


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IM_B2R_H_

/**
@weakgroup im_b2r
@{

<hr>

@section im_b2r_sample Sample code

@section im_b2r_sample_section1	Sample code of B2R Initialization.
	@code
	Im_B2R_Init( D_IM_B2R_PIPE12 );
	@endcode
	<br><br>

@section im_b2r_sample_section2	Sample code of data setting for Photo.
	@code
	#define D_USER_RAW_TOP_X_PIXS 16
	#define D_USER_RAW_TOP_Y_PIXS 12
	#define D_USER_RAW_WIDTH_PIXS 1920
	#define D_USER_RAW_LINES_PIXS 1080

	#define D_USER_RAW_IN_LINEFEED_BYTES (D_USER_YCC_WIDTH_PIXS * 2)		// 1pixel = 16bit
	#define D_USER_RGB_OUT_LINEFEED_BYTES (D_USER_YCC_WIDTH_PIXS * 3 * 1.5)	// 1pixel = 36bit
	#define D_USER_RAW_IN_ADDR    0x00000000
	#define D_USER_RGB_OUT_ADDR_R 0x00000000
	#define D_USER_RGB_OUT_ADDR_G 0x00000000
	#define D_USER_RGB_OUT_ADDR_B 0x00000000

	VOID User_Photo_Handler( UINT32* status, UINT32 user_param );

	VOID User_Photo_B2R_Process( VOID ) {
		T_IM_B2R_CTRL b2r_ctrl;
		T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_in;
		T_IM_B2R_RECT b2r_rect_param;
		T_IM_B2R_INADDR_INFO b2r_in_addr;
		T_IM_B2R_OUTBANK_INFO b2r_out_addr;

		b2r_ctrl.bayer_top_pixel = D_IM_B2R_TOP_PIXEL_R;
		b2r_ctrl.deknee_enable = D_IM_B2R_ENABLE_OFF;

		b2r_ctrl.ybw.axi_write_mode.out_sel = D_IM_B2R_YBW_OUTPUT_RGB;
		b2r_ctrl.ybw.axi_write_mode.burst_length = D_IM_B2R_BRST_512;
		b2r_ctrl.ybw.output_dtype = D_IM_B2R_STL_DTYP_PACK12;
		b2r_ctrl.ybw.write_request_mask[D_IM_B2R_PORT_R] = 0;
		b2r_ctrl.ybw.write_request_mask[D_IM_B2R_PORT_G] = 0;
		b2r_ctrl.ybw.write_request_mask[D_IM_B2R_PORT_B] = 0;
		b2r_ctrl.ybw.trim_out_enable = D_IM_B2R_ENABLE_OFF;
		b2r_ctrl.ybw.knee_enable = D_IM_B2R_ENABLE_OFF;

		b2r_ctrl.line_intr.count = 0;
		b2r_ctrl.ybw_continue_start_enable = D_IM_B2R_ENABLE_OFF;
		b2r_ctrl.cia_bypass = D_IM_B2R_ENABLE_ON;
		b2r_ctrl.cia_padding = D_IM_B2R_ENABLE_ON;
		b2r_ctrl.ext_out_en = D_IM_B2R_ENABLE_OFF;
		b2r_ctrl.ext_out_alarm = D_IM_B2R_EXT_OUT_ALARM_1LINE;
		b2r_ctrl.b2r_user_handler = User_Photo_Handler;
		b2r_ctrl.user_param = 0;

		b2r_ctrl_sdram_in.burst_length = D_IM_B2R_BRST_512;
		b2r_ctrl_sdram_in.read_request_mask = 0;
		b2r_ctrl_sdram_in.input_dtype = D_IM_B2R_STL_DTYP_PACK16;
		b2r_ctrl_sdram_in.top_offset = 0;
		b2r_ctrl_sdram_in.input_global = D_USER_RAW_IN_LINEFEED_BYTES;
		b2r_ctrl_sdram_in.line_space = 0;

		b2r_rect_param.input_size.img_top = D_USER_RAW_TOP_X_PIXS;
		b2r_rect_param.input_size.img_left = D_USER_RAW_TOP_Y_PIXS;
		b2r_rect_param.input_size.img_width = D_USER_RAW_WIDTH_PIXS;
		b2r_rect_param.input_size.img_lines = D_USER_RAW_LINES_PIXS;
		b2r_rect_param.output_size.output_global_w = D_USER_RGB_OUT_LINEFEED_BYTES;

		b2r_in_addr.raw.addr = (VOID*)D_USER_RAW_IN_ADDR;

		memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
		b2r_out_addr.bank_initial_position = D_IM_B2R_YBW_BANK_0;
		b2r_out_addr.use_bank_num = 3;
		b2r_out_addr.output_addr.rgb.addr_R = (VOID*)D_USER_RGB_OUT_ADDR_R;
		b2r_out_addr.output_addr.rgb.addr_G = (VOID*)D_USER_RGB_OUT_ADDR_G;
		b2r_out_addr.output_addr.rgb.addr_B = (VOID*)D_USER_RGB_OUT_ADDR_B;

		Im_B2R_Ctrl( D_IM_B2R_PIPE1, &b2r_ctrl );
		Im_B2R_Ctrl_ModeSDRAMInput( D_IM_B2R_PIPE1, &b2r_ctrl_sdram_in );
		Im_B2R_Ctrl_Left_Shift( D_IM_B2R_PIPE1, 0, 1 );	// Left shift 0bit, Clip is on.
		Im_B2R_Set_Rect( D_IM_B2R_PIPE1, &b2r_rect_param );
		Im_B2R_Set_InAddr_Info( D_IM_B2R_PIPE1, &b2r_in_addr );
		Im_B2R_Set_OutBankInfo( D_IM_B2R_PIPE1, &b2r_out_addr );

		// Start B2R
		Im_B2R_Start( D_IM_B2R_PIPE1 );

		// Wait B2R YBW is normal end.
		Im_B2R_WaitEnd( NULL, D_IM_B2R_INT_FLG_YBW_END, 60 );

		Im_B2R_Stop( D_IM_B2R_PIPE1 );
	}

	VOID User_Photo_Handler( UINT32* status, UINT32 user_param )
	{
		UINT32 err_factor;
		switch( *status ) {
			case D_IM_B2R1_INT_FLG_YBR_ERR:
				Im_B2R_Get_YBR_Error_Factor( D_IM_B2R_PIPE1, &err_factor );
				printf( "B2R YBR1 Error. %lx, %lx\n", err_factor, user_param );
				break;
			case D_IM_B2R2_INT_FLG_YBR_ERR:
				Im_B2R_Get_YBR_Error_Factor( D_IM_B2R_PIPE2, &err_factor );
				printf( "B2R YBR2 Error. %lx, %lx\n", err_factor, user_param );
				break;
			case D_IM_B2R1_INT_FLG_YBW_ERR:
			case D_IM_B2R1_INT_FLG_AXR_ERR:
			case D_IM_B2R1_INT_FLG_AXW_ERR:
			case D_IM_B2R1_INT_FLG_CPERR_ERR:
			case D_IM_B2R2_INT_FLG_YBW_ERR:
			case D_IM_B2R2_INT_FLG_AXR_ERR:
			case D_IM_B2R2_INT_FLG_AXW_ERR:
			case D_IM_B2R2_INT_FLG_CPERR_ERR:
				printf( "B2R Error. %lx\n", user_param );
				break;
			default:
				// Nothing to do.
				break;
		}
	}
	@endcode
	<br><br>


@section im_b2r_sample_section3	Sample code of data setting for Video.
	@code
	#define D_USER_RAW_TOP_X_PIXS 16
	#define D_USER_RAW_TOP_Y_PIXS 12
	#define D_USER_RAW_WIDTH_PIXS 1920
	#define D_USER_RAW_LINES_PIXS 1080

	#define D_USER_RGB_OUT_LINEFEED_BYTES (D_USER_YCC_WIDTH_PIXS * 3 * 1.5)	// 1pixel = 36bit

	#define D_USER_VIDEO_HORIZONTAL_BLANKING 200
	#define D_USER_VIDEO_RGB_OUT_ADDR_R 0x00000000
	#define D_USER_VIDEO_RGB_OUT_ADDR_G 0x00000000
	#define D_USER_VIDEO_RGB_OUT_ADDR_B 0x00000000

	VOID User_Video_Handler( UINT32* status, UINT32 user_param );

	VOID User_Video_B2R_Start( VOID ) {
		T_IM_B2R_CTRL b2r_ctrl;
		T_IM_B2R_CTRL_B2B_DIRECT b2r_ctrl_b2b_direct;
		T_IM_B2R_RECT b2r_rect_param;
		T_IM_B2R_OUTBANK_INFO b2r_out_addr;

		b2r_ctrl.bayer_top_pixel = D_IM_B2R_TOP_PIXEL_R;

		b2r_ctrl.ybw.axi_write_mode.out_sel = D_IM_B2R_YBW_OUTPUT_RGB;
		b2r_ctrl.ybw.axi_write_mode.burst_length = D_IM_B2R_BRST_512;
		b2r_ctrl.ybw.output_dtype = D_IM_B2R_STL_DTYP_PACK12;			// dummy
		b2r_ctrl.ybw.write_request_mask[D_IM_B2R_PORT_R] = 0;
		b2r_ctrl.ybw.write_request_mask[D_IM_B2R_PORT_G] = 0;
		b2r_ctrl.ybw.write_request_mask[D_IM_B2R_PORT_B] = 0;
		b2r_ctrl.ybw.trim_out_enable = D_IM_B2R_ENABLE_OFF;
		b2r_ctrl.ybw.knee_enable = D_IM_B2R_ENABLE_OFF;

		b2r_ctrl.line_intr.count = 0;
		b2r_ctrl.ybw_continue_start_enable = D_IM_B2R_ENABLE_OFF;
		b2r_ctrl.cia_bypass = D_IM_B2R_ENABLE_ON;
		b2r_ctrl.cia_padding = D_IM_B2R_ENABLE_ON;
		b2r_ctrl.ext_out_en = D_IM_B2R_ENABLE_ON;
		b2r_ctrl.b2r_user_handler = User_Video_Handler;
		b2r_ctrl.user_param = 0;

		b2r_ctrl_b2b_direct.even_line_start_timming = D_USER_VIDEO_HORIZONTAL_BLANKING - Im_B2R_Get_HRingPixs();
		b2r_ctrl_b2b_direct.frame_stop = 0;

		b2r_rect_param.input_size.img_top = D_USER_RAW_TOP_X_PIXS;
		b2r_rect_param.input_size.img_left = D_USER_RAW_TOP_Y_PIXS;
		b2r_rect_param.input_size.img_width = D_USER_RAW_WIDTH_PIXS;
		b2r_rect_param.input_size.img_lines = D_USER_RAW_LINES_PIXS;
		b2r_rect_param.output_size.output_global_w = D_USER_RGB_OUT_LINEFEED_BYTES;	// dummy

		memset( &b2r_out_addr, '\x00', sizeof(b2r_out_addr) );
		b2r_addr_0.bank_initial_position = D_IM_B2R_YBW_BANK_0;								// dummy
		b2r_addr_0.use_bank_num = 3;														// dummy
		b2r_addr_0.output_addr.rgb.addr_R = (VOID*)D_USER_VIDEO_RGB_OUT_ADDR_R;				// dummy
		b2r_addr_0.output_addr.rgb.addr_G = (VOID*)D_USER_VIDEO_RGB_OUT_ADDR_G;				// dummy
		b2r_addr_0.output_addr.rgb.addr_B = (VOID*)D_USER_VIDEO_RGB_OUT_ADDR_B;				// dummy

		Im_B2R_Ctrl( D_IM_B2R_PIPE1, &b2r_ctrl );
		Im_B2R_Ctrl_ModeB2BDirect( D_IM_B2R_PIPE1, &b2r_ctrl_b2b_direct );
		Im_B2R_Ctrl_Left_Shift( D_IM_B2R_PIPE1, 0, 1 );	// Left shift 0bit, Clip is on.
		Im_B2R_Set_Rect( D_IM_B2R_PIPE1, &b2r_rect_param );
		Im_B2R_Set_OutBankInfo( D_IM_B2R_PIPE1, &b2r_addr_0 );

		// Start B2R
		Im_B2R_Start( D_IM_B2R_PIPE1 );

		// Wait B2R YBW0 is normal end.
		Im_B2R_WaitEnd( NULL, D_IM_B2R_INT_FLG_YBW_END, 60 );

		Im_B2R_Stop( D_IM_B2R_PIPE1 );
	}

	VOID User_Video_Handler( UINT32* status, UINT32 user_param )
	{
		switch( *status ) {
			case D_IM_B2R1_INT_FLG_YBR_ERR:
				Im_B2R_Get_YBR_Error_Factor( D_IM_B2R_PIPE1, &err_factor );
				printf( "B2R YBR1 Error. %lx, %lx\n", err_factor, user_param );
				break;
			case D_IM_B2R2_INT_FLG_YBR_ERR:
				Im_B2R_Get_YBR_Error_Factor( D_IM_B2R_PIPE2, &err_factor );
				printf( "B2R YBR2 Error. %lx, %lx\n", err_factor, user_param );
				break;
			case D_IM_B2R1_INT_FLG_YBW_ERR:
			case D_IM_B2R1_INT_FLG_AXR_ERR:
			case D_IM_B2R1_INT_FLG_AXW_ERR:
			case D_IM_B2R1_INT_FLG_CPERR_ERR:
			case D_IM_B2R2_INT_FLG_YBW_ERR:
			case D_IM_B2R2_INT_FLG_AXR_ERR:
			case D_IM_B2R2_INT_FLG_AXW_ERR:
			case D_IM_B2R2_INT_FLG_CPERR_ERR:
				printf( "B2R Error. %lx\n", user_param );
				break;
			default:
				// Nothing to do.
				break;
		}
	}
	@endcode
	<br><br>

@}*/	// weakgroup im_b2r


/**
@weakgroup im_b2r_iq
@{

<hr>

@section im_b2r_iq_sample Sample code

@section im_b2r_iq_sample_section1	Sample code of data setting for White Balance Gain.
	The precondition of the this sample is as follows.<br>
	@code
	T_IM_B2R_BAYER_COLOR b2r_wb;
	FLGPTN flgptn;

	b2r_wb.R  = 400;
	b2r_wb.Gr = 256;
	b2r_wb.Gb = 256;
	b2r_wb.B  = 480;

	Im_B2R_Set_WB_Gain( D_IM_B2R_PIPE1, &b2r_wb );

	b2r_wb.R  = 16;
	b2r_wb.Gr = 16;
	b2r_wb.Gb = 16;
	b2r_wb.B  = 16;

	Im_B2R_Set_WB_Slope_Gain( D_IM_B2R_PIPE1, 1, &b2r_wb );

	b2r_wb.R  = 0xFFF;
	b2r_wb.Gr = 0xFFF;
	b2r_wb.Gb = 0xFFF;
	b2r_wb.B  = 0xFFF;

	Im_B2R_Set_WB_Clip_Level( D_IM_B2R_PIPE1, 1, &b2r_wb );

	Im_B2R_Start( D_IM_B2R_PIPE1 );
	Im_B2R_WaitEnd( &flgptn, (D_IM_B2R_INT_FLG_YBW_END|D_IM_B2R_INT_FLG_YBW_ERR), 1000 );
	@endcode
	<br><br>

@section im_b2r_iq_sample_section2	Sample code of data setting for imaging.
	@code

	T_IM_B2R_OFS b2r_ctrl_ofs;
	T_IM_B2R_CTRL_ADIP b2r_ctrl_adip;
	T_IM_B2R_CTRL_HPF b2r_hpf;

	b2r_ctrl_ofs.R  = 0;
	b2r_ctrl_ofs.B  = 0;
	b2r_ctrl_ofs.Gr = 0;
	b2r_ctrl_ofs.Gb = 0;

	Im_B2R_Set_Offset( D_IM_B2R_PIPE1, &b2r_ctrl_ofs );

	Im_B2R_Ctrl_Left_Shift( D_IM_B2R_PIPE1, 0, 1 );

	b2r_ctrl_adip.sharp_level_a        = 8;
	b2r_ctrl_adip.sharp_level_b        = 0;
	b2r_ctrl_adip.sharp_level_c        = 0;
	b2r_ctrl_adip.adj_feel_a           = 9;
	b2r_ctrl_adip.adj_feel_b           = 4;
	b2r_ctrl_adip.lf_strength          = 0;
	b2r_ctrl_adip.lf_threshold         = 0;
	b2r_ctrl_adip.cmc_level            = 0;
	b2r_ctrl_adip.cmc_limit            = 1023;
	b2r_ctrl_adip.dir_reliab_threshold = 100;

	Im_B2R_Ctrl_Sensitivity( D_IM_B2R_PIPE1, &b2r_ctrl_adip );

	b2r_hpf.hpf_enable                         = 0;
	b2r_hpf.flat_ctrl_enable                   = 0;
	b2r_hpf.y_ctrl_enable                      = 0;
	b2r_hpf.hpf_k[0]                           = 0;
	b2r_hpf.hpf_k[1]                           = 0;
	b2r_hpf.hpf_k[2]                           = 0;
	b2r_hpf.hpf_k[3]                           = 0;
	b2r_hpf.hpf_k[4]                           = 0;
	b2r_hpf.hpf_k[5]                           = 0;
	b2r_hpf.hpf_k[6]                           = 0;
	b2r_hpf.hpf_k[7]                           = 0;
	b2r_hpf.hpf_k[8]                           = 0;
	b2r_hpf.hpf_k[9]                           = 0;
	b2r_hpf.hpf_level_clip_p                   = 0x007F;
	b2r_hpf.hpf_level_clip_m                   = 0x007F;
	b2r_hpf.flat_scale                         = 0;
	b2r_hpf.flat_threshold                     = 0;
	b2r_hpf.flat_gain_limit                    = 0xFFF;
	b2r_hpf.flat_level_clip_p                  = 0x7F;
	b2r_hpf.flat_level_clip_m                  = 0x7F;
	b2r_hpf.y_convert_coef[0]                  = 0x004D;
	b2r_hpf.y_convert_coef[1]                  = 0x0096;
	b2r_hpf.y_convert_coef[2]                  = 0x001D;
	b2r_hpf.y_convert_coef[3]                  = 0x01D5;
	b2r_hpf.y_convert_coef[4]                  = 0x01AB;
	b2r_hpf.y_convert_coef[5]                  = 0x0080;
	b2r_hpf.y_convert_coef[6]                  = 0x0080;
	b2r_hpf.y_convert_coef[7]                  = 0x01EB;
	b2r_hpf.y_convert_coef[8]                  = 0x0195;
	b2r_hpf.y_ctrl_offset[0]                   = 0;
	b2r_hpf.y_ctrl_offset[1]                   = 0;
	b2r_hpf.y_ctrl_offset[2]                   = 0;
	b2r_hpf.y_ctrl_offset[3]                   = 0;
	b2r_hpf.y_ctrl_gain[0]                     = 0;
	b2r_hpf.y_ctrl_gain[1]                     = 0;
	b2r_hpf.y_ctrl_gain[2]                     = 0;
	b2r_hpf.y_ctrl_gain[3]                     = 0;
	b2r_hpf.y_ctrl_border[0]                   = 0;
	b2r_hpf.y_ctrl_border[1]                   = 0;
	b2r_hpf.y_ctrl_border[2]                   = 0;
	b2r_hpf.y_ctrl_border[3]                   = 0;
	b2r_hpf.y_ctrl_gain_posi_dec1              = 0;
	b2r_hpf.y_ctrl_gain_posi_dec2              = 5;
	b2r_hpf.y_ctrl_level_clip_p                = 0x007F;
	b2r_hpf.y_ctrl_level_clip_m                = 0x007F;
	b2r_hpf.y_ctrl_cor_p                       = 0;
	b2r_hpf.y_ctrl_cor_m                       = 0;
	b2r_hpf.hpf_add_level_clip_r               = 0x0FFF;
	b2r_hpf.hpf_add_level_clip_g               = 0x0FFF;
	b2r_hpf.hpf_add_level_clip_b               = 0x0FFF;
	b2r_hpf.area_a_enable                      = 0;
	b2r_hpf.area_b_enable                      = 0;
	b2r_hpf.area_marge_method                  = D_IM_B2R_HFACRV_MRG_AVERAGE;	// dummy
	b2r_hpf.area_a_ctrl.ellipse_area_focus1_x  = 0;
	b2r_hpf.area_a_ctrl.ellipse_area_focus1_y  = 0;
	b2r_hpf.area_a_ctrl.ellipse_area_focus2_x  = 0;
	b2r_hpf.area_a_ctrl.ellipse_area_focus2_y  = 0;
	b2r_hpf.area_a_ctrl.c_area_scale_offset    = 0;
	b2r_hpf.area_a_ctrl.c_area_scale_gain[0]   = 0;
	b2r_hpf.area_a_ctrl.c_area_scale_gain[1]   = 0;
	b2r_hpf.area_a_ctrl.c_area_scale_border    = 0;
	b2r_hpf.area_a_ctrl.c_area_scale_clip_l    = 0;
	b2r_hpf.area_a_ctrl.c_area_scale_clip_h    = 0xFF;
	b2r_hpf.area_a_ctrl.y_area_scale_gain[0]   = 0;
	b2r_hpf.area_a_ctrl.y_area_scale_gain[1]   = 0;
	b2r_hpf.area_a_ctrl.y_area_scale_border[0] = 0;
	b2r_hpf.area_a_ctrl.y_area_scale_border[1] = 0;
	b2r_hpf.area_a_ctrl.y_area_scale_clip_l    = 0;
	b2r_hpf.area_a_ctrl.y_area_scale_clip_h    = 0;
	b2r_hpf.area_b_ctrl.ellipse_area_focus1_x  = 0;
	b2r_hpf.area_b_ctrl.ellipse_area_focus1_y  = 0;
	b2r_hpf.area_b_ctrl.ellipse_area_focus2_x  = 0;
	b2r_hpf.area_b_ctrl.ellipse_area_focus2_y  = 0;
	b2r_hpf.area_b_ctrl.c_area_scale_offset    = 0;
	b2r_hpf.area_b_ctrl.c_area_scale_gain[0]   = 0;
	b2r_hpf.area_b_ctrl.c_area_scale_gain[1]   = 0;
	b2r_hpf.area_b_ctrl.c_area_scale_border    = 0;
	b2r_hpf.area_b_ctrl.c_area_scale_clip_l    = 0;
	b2r_hpf.area_b_ctrl.c_area_scale_clip_h    = 0xFF;
	b2r_hpf.area_b_ctrl.y_area_scale_gain[0]   = 0;
	b2r_hpf.area_b_ctrl.y_area_scale_gain[1]   = 0;
	b2r_hpf.area_b_ctrl.y_area_scale_border[0] = 0;
	b2r_hpf.area_b_ctrl.y_area_scale_border[1] = 0;
	b2r_hpf.area_b_ctrl.y_area_scale_clip_l    = 0;
	b2r_hpf.area_b_ctrl.y_area_scale_clip_h    = 0;

	Im_B2R_Ctrl_HighPassFilter( D_IM_B2R_PIPE1, &b2r_hpf );

	@endcode
	<br><br>

@} */	// weakgroup im_b2r_iq

