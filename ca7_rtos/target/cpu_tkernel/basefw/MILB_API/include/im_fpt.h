/**
 * @file		im_fpt.h
 * @brief		Header file of JDSFPT driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/**
@addtogroup im_fpt
@{

*/

#ifndef _IM_FPT_H_
#define _IM_FPT_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/**
@weakgroup fpt_drv FPT driver API
@{
*/
/**
@name Returned value
@{
*/
#define D_IM_FPT_RETVAL_OK						(0)				/**< Normal end															*/
#define D_IM_FPT_RETVAL_INVALID_ARG_ERR			(-1)			/**< Invalid argument error. The members of structure is not checked.	*/
#define D_IM_FPT_RETVAL_SYSTEM_CALL_ERR			(-2)			/**< System-call error													*/
#define D_IM_FPT_RETVAL_ERR						(-3)			/**< Error																*/
#define D_IM_FPT_RETVAL_BUSY_ERR				(-4)			/**< FPT Busy															*/
/** @} */	// name
/** @} */	// end of fpt_drv

#define D_IM_FPT_WINDOW_5x5						(0)				/**< Sum Window 5x5								*/
#define D_IM_FPT_WINDOW_7x7						(1)				/**< Sum Window 7x7								*/

#define D_IM_FPT_DS_MIN							(0x0100)		/**< Min down-sampling 1/1 [No down-sampling]	*/
#define D_IM_FPT_DS_MAX							(0x1400)		/**< Max down-sampling 1/20						*/

#define D_IM_FPT_OUT_COORD_ONLY					(0)				/**< Output coord only							*/
#define D_IM_FPT_OUT_COORD_AND_FEATURE_AMOUNT	(1)				/**< Output coord and feature amount			*/

#define D_IM_FPT_READ_FROM_RAM					(0)				/**< Read target's coord from RAM.				*/
#define D_IM_FPT_SAME_AS_BASE_COORD				(1)				/**< Target coord is same as base coord.		*/

#define D_IM_FPT_INTERRUPT_COMPLETE				(0x00000001)	/**< Interruption of FPT completion				*/
#define D_IM_FPT_INTERRUPT_AXI_ERR				(0x00000010)	/**< Interruption of AXI error					*/
#define D_IM_FPT_INTERRUPT_PARAM_ERR			(0x00000100)	/**< Interruption of Parameter error			*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** The configuration of FPTCTL2.IO_MODE. */
typedef enum {
	E_IM_FPT_IO_MODE_SDRAM,				/**< SDRAM mode : Read data from SDRAM and do the processing. Write the result to SDRAM.	*/
	E_IM_FPT_IO_MODE_REGISTER			/**< Register mode : Read data via AHB and do the processing. Read the result via AHB.		*/
} E_IM_FPT_IO_MODE;

/** The configuration of FPTCTL2.PIPE_EN. */
typedef enum {
	E_IM_FPT_PIPE_EN_DISABLE,			/**< Disable pipeline processing.				*/
	E_IM_FPT_PIPE_EN_ENABLE				/**< Enable pipeline processing (fast method).	*/
} E_IM_FPT_PIPE_EN;

/** The configuration of FPTCTL2.DEBUG_MODE. */
typedef enum {
	E_IM_FPT_DEBUG_MODE_NORMAL,			/**< Normal mode.	*/
	E_IM_FPT_DEBUG_MODE_DEBUG			/**< Debug mode.	*/
} E_IM_FPT_DEBUG_MODE;

/** The selection of LUTRAM. */
typedef enum {
	E_IM_FPT_LUTSEL_OFF,				/**< LUT  is not used at all.	*/
	E_IM_FPT_LUTSEL_0,					/**< LUT0 is used.				*/
	E_IM_FPT_LUTSEL_1					/**< LUT1 is used.				*/
} E_IM_FPT_LUTSEL;

/** The configuration of LUTRAM. */
typedef enum {
	E_IM_FPT_LUTCFG_OFF,				/**< LUT  is not used at all.										*/
	E_IM_FPT_LUTCFG_BASE_0,				/**< LUT0 is used for Base   image. LUT1 is not used.				*/
	E_IM_FPT_LUTCFG_TARGET_1,			/**< LUT0 is not used.              LUT1 is used for Target image.	*/
	E_IM_FPT_LUTCFG_BASE_0__TARGET_1,	/**< LUT0 is used for Base   image. LUT1 is used for Target image.	*/
	E_IM_FPT_LUTCFG_BASE_1__TARGET_0	/**< LUT0 is used for Target image. LUT1 is used for Base   image.	*/
} E_IM_FPT_LUTCFG;


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/** 
 Parameters of Im_FPT_Ctrl_Mode0().
*/
typedef struct {
	USHORT				global_width;				/**<
														The number of pixels of 1 line.<br>
														It means line feed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFQCTL0.GHSIZE[14:0] register.<br>
													*/
	USHORT				width;						/**<
														The number of horizontal pixels to be processed.<br>
														Should be less than or equal to T_IM_FPT_MODE0::global_width.<br>
														The value is set to FPTFQCTL1.HSIZE[14:0] register.<br>
														This setting range depends on T_IM_FPT_MODE0::down_sample_x and T_IM_FPT_MODE0::window_size.<br>
														The calculation formula is the following.<br>
														<table>
															<tr>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_5x5</th>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_7x7</th>
															</tr>
															<tr>
																<th>min</th>
																<th>max</th>
																<th>min</th>
																<th>max</th>
															</tr>
															<tr>
																<td>13 * T_IM_FPT_MODE0::down_sample_x</td>
																<td>224 * T_IM_FPT_MODE0::down_sample_x</td>
																<td>15 * T_IM_FPT_MODE0::down_sample_x</td>
																<td>224 * T_IM_FPT_MODE0::down_sample_x</td>
															</tr>
														</table>
													*/
	USHORT				lines;						/**<
														The number of vertical pixels to be processed.<br>
														The value is set to FPTFQCTL1.VSIZE[14:0] register.<br>
														This setting range depends on T_IM_FPT_MODE0::down_sample_y and T_IM_FPT_MODE0::window_size.<br>
														<table>
															<tr>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_5x5</th>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_7x7</th>
															</tr>
															<tr>
																<th>min</th>
																<th>max</th>
																<th>min</th>
																<th>max</th>
															</tr>
															<tr>
																<td>13 * T_IM_FPT_MODE0::down_sample_y</td>
																<td>150 * T_IM_FPT_MODE0::down_sample_y</td>
																<td>15 * T_IM_FPT_MODE0::down_sample_y</td>
																<td>150 * T_IM_FPT_MODE0::down_sample_y</td>
															</tr>
														</table>
													*/
	E_IM_FPT_LUTSEL		lutsel;						/**<
														LUT control for input iamge.<br>
														<ul>
														<li> @ref E_IM_FPT_LUTSEL_OFF	[FPTCTL3.BRC_EN=0b]
														<li> @ref E_IM_FPT_LUTSEL_0		[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=0b]
														<li> @ref E_IM_FPT_LUTSEL_1		[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=1b]
														</ul>
													*/
	UCHAR				window_size;				/**<
														The size of sum window.<br>
														<ul>
														<li> @ref D_IM_FPT_WINDOW_5x5	[FPTFQCTL2.SUMW=0b]
														<li> @ref D_IM_FPT_WINDOW_7x7	[FPTFQCTL2.SUMW=1b]
														</ul>
													*/
	USHORT				down_sample_x;				/**<
														Horizontal down-sampling.(down_sample_x/256)<br>
														value range  :[256 - 5120]<br>
														The value is set to FPTFQCTL9.SKIPX_FQ[12:0] register.<br>
													*/
	USHORT				down_sample_y;				/**<
														Vertical down-sampling.(down_sample_y/256)<br>
														value range  :[256 - 5120]<br>
														The value is set to FPTFQCTL9.SKIPY_FQ[12:0] register.<br>
													*/
	UINT32				img_addr;					/**<
														Beginning address of image to be processed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFQCTL3.SDAD_FQ_OF[31:0] register.<br>
													*/
	UINT32				feature_amount_addr;		/**<
														Beginning address of feature amount data.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFQCTL4.SDAD_FQ_MP[31:0] register.<br>
													*/
	USHORT				feature_amount_line_byte;	/**<
														The number of bytes per feature amount data's line.<br>
														feature_amount_line_byte = [8-byte rounding up (((width/down_sample_x)-(12+(window_size*2)))*2)]<br>
														Should be aligned on 8.<br>
														The value is set to FPTFQCTL8.MAPGHSIZE[14:0] register.<br>
													*/
	UINT32				histgram_addr;				/**<
														Beginning address of histgram data.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFQCTL5.SDAD_FQ_HI[31:0] register.<br>
													*/
} T_IM_FPT_MODE0;

/** 
   Parameters of Im_FPT_Ctrl_Mode1().
 */
typedef struct {
	UCHAR				num_feature_point;			/**<
														The number of feature point to be re-calculated.[1~127].<br>
														The value is set to FPTFQCTL2.FPMAX[6:0] register.<br>
													*/
	UCHAR				output;						/**<
														Control output data.
														<ul>
														<li>@ref D_IM_FPT_OUT_COORD_ONLY 				Output coord data only.
														<li>@ref D_IM_FPT_OUT_COORD_AND_FEATURE_AMOUNT	Output coord and feature amount data.
														</ul>
														The value is set to FPTFQCTL2.OUTEN register.<br>
													*/
	USHORT				global_width;				/**<
														The number of pixels of 1 line.<br>
														It means line feed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFQCTL0.GHSIZE[14:0] register.<br>
													*/
	USHORT				width;						/**<
														The number of horizontal pixels to be processed.<br>
														Should be less than or equal to T_IM_FPT_MODE1::global_width.<br>
														The value is set to FPTFQCTL1.HSIZE[14:0] register.<br>
														This setting range depends on T_IM_FPT_MODE1::down_sample_x and T_IM_FPT_MODE1::window_size.<br>
														The calculation formula is the following.<br>
														<table>
															<tr>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_5x5</th>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_7x7</th>
															</tr>
															<tr>
																<th>min</th>
																<th>max</th>
																<th>min</th>
																<th>max</th>
															</tr>
															<tr>
																<td>13 * T_IM_FPT_MODE1::down_sample_x</td>
																<td>32 * T_IM_FPT_MODE1::down_sample_x</td>
																<td>15 * T_IM_FPT_MODE1::down_sample_x</td>
																<td>32 * T_IM_FPT_MODE1::down_sample_x</td>
															</tr>
														</table>
														Please set the odd number in Mode1 (Re-calculate Feature Amount).<br>
														In case of binning, please set the odd number to the value after binning.<br>
														(When the value after binning is 15 and binning rate is 1/4, Please set 60 to the value.)<br>
													*/
	USHORT				lines;						/**<
														The number of vertical pixels to be processed.<br>
														The value is set to FPTFQCTL1.VSIZE[14:0] register.<br>
														This setting range depends on T_IM_FPT_MODE1::down_sample_y and T_IM_FPT_MODE1::window_size.<br>
														<table>
															<tr>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_5x5</th>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_7x7</th>
															</tr>
															<tr>
																<th>min</th>
																<th>max</th>
																<th>min</th>
																<th>max</th>
															</tr>
															<tr>
																<td>Mode1 (Re-calculate Feature Amount)</td>
																<td>13 * T_IM_FPT_MODE1::down_sample_y</td>
																<td>32 * T_IM_FPT_MODE1::down_sample_y</td>
																<td>15 * T_IM_FPT_MODE1::down_sample_y</td>
																<td>32 * T_IM_FPT_MODE1::down_sample_y</td>
															</tr>
														</table>
													*/
	E_IM_FPT_LUTSEL		lutsel;						/**<
														LUT control for input iamge.<br>
														<ul>
														<li> @ref E_IM_FPT_LUTSEL_OFF	[FPTCTL3.BRC_EN=0b]
														<li> @ref E_IM_FPT_LUTSEL_0		[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=0b]
														<li> @ref E_IM_FPT_LUTSEL_1		[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=1b]
														</ul>
													*/
	UCHAR				window_size;				/**<
														The size of sum window.<br>
														<ul>
														<li> @ref D_IM_FPT_WINDOW_5x5	[FPTFQCTL2.SUMW=0b]
														<li> @ref D_IM_FPT_WINDOW_7x7	[FPTFQCTL2.SUMW=1b]
														</ul>
													*/
	USHORT				down_sample_x;				/**<
														Horizontal down-sampling.(down_sample_x/256)<br>
														value range  :[256 - 5120]<br>
														The value is set to FPTFQCTL9.SKIPX_FQ[12:0] register.<br>
													*/
	USHORT				down_sample_y;				/**<
														Vertical down-sampling.(down_sample_y/256)<br>
														value range  :[256 - 5120]<br>
														The value is set to FPTFQCTL9.SKIPY_FQ[12:0] register.<br>
													*/
	UINT32				img_addr;					/**<
														Beginning address of image to be processed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFQCTL3.SDAD_FQ_OF[31:0] register.<br>
													*/
	UINT32				input_coord_addr;			/**<
														Beginning address of feature point coord data.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFQCTL6.SDAD_FQ_CXY[31:0] register.<br>
													*/
	UINT32				output_addr;				/**<
														Beginning address of output data.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFQCTL7.SDAD_FQ_RXY[31:0] register.<br>
													*/
} T_IM_FPT_MODE1;

/** 
Tracking Search Window.<br>
Need to adjust search window on image boundary.<br>
*/
typedef struct {
	USHORT		left;								/**<
														Left search range.<br>
														4.8 fixed point value. [0x000~0x7FF]<br>
														The value is set to FPTFTCTL2.LEFTLIMIT register.<br>
													*/
	USHORT		right;								/**<
														Right search range.<br>
														4.8 fixed point value. [0x000~0x7FF]<br>
														The value is set to FPTFTCTL2.RIGHTLIMIT register.<br>
													*/
	USHORT		top;								/**<
														Top search range.<br>
														4.8 fixed point value. [0x000~0x7FF]<br>
														The value is set to FPTFTCTL3.TOPLIMIT register.<br>
													*/
	USHORT		bottom;								/**<
														Bottom search range.<br>
														4.8 fixed point value. [0x000~0x7FF]<br>
														The value is set to FPTFTCTL3.BOTTOMLIMIT register.<br>
													*/
} T_IM_FPT_SEARCH_WINDOW;

/** 
 Parameters of Im_FPT_Ctrl_Mode2().
*/
typedef struct {
	USHORT					base_global_width;		/**<
														The number of pixels of base image's 1 line.<br>
														It means line feed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL0.GHBSIZE[14:0] register.<br>
													*/
	USHORT					target_global_width;	/**<
														The number of pixels of target image's 1 line.<br>
														It means line feed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL0.GHTSIZE[14:0] register.<br>
													*/
	UCHAR					num_feature_point;		/**<
														The number of feature point to be tracked.[1~127].<br>
														The value is set to FPTFTCTL1.FTMAX[6:0] register.<br>
													*/
	UCHAR					usage_target_coord;		/**<
														Control the usage of target image's coord
														<ul>
														<li> @ref D_IM_FPT_READ_FROM_RAM      [FPTFTCTL1.INIT_0 = 0b]
														<li> @ref D_IM_FPT_SAME_AS_BASE_COORD [FPTFTCTL1.INIT_0 = 1b]
														</ul>
														The value is set to FPTFTCTL1.INIT_0 register.<br>
													*/
	USHORT					down_sample_x;			/**<
														Horizontal down-sampling.(down_sample_x/256)<br>
														value range  :[256 - 5120]<br>
														The value is set to FPTFTCTL10.SKIPX_FT[12:0] register.<br>
													*/
	USHORT					down_sample_y;			/**<
														Vertical down-sampling.(down_sample_y/256)<br>
														value range  :[256 - 5120]<br>
														The value is set to FPTFTCTL10.SKIPY_FT[12:0] register.<br>
													*/
	E_IM_FPT_LUTCFG			lutcfg;					/**<
														LUT control for input iamge.<br>
														<ul>
														<li> @ref E_IM_FPT_LUTCFG_OFF				[FPTCTL3.BRC_EN=0b]
														<li> @ref E_IM_FPT_LUTCFG_BASE_0			[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=0b, FPTCTL3.BRC_MOD=00b]
														<li> @ref E_IM_FPT_LUTCFG_TARGET_1			[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=1b, FPTCTL3.BRC_MOD=01b]
														<li> @ref E_IM_FPT_LUTCFG_BASE_0__TARGET_1	[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=0b, FPTCTL3.BRC_MOD=10b]
														<li> @ref E_IM_FPT_LUTCFG_BASE_1__TARGET_0	[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=1b, FPTCTL3.BRC_MOD=10b]
														</ul>
													*/
	T_IM_FPT_SEARCH_WINDOW	search_window;			/**<
														Search window.<br>
													*/
	USHORT					tracking_threshold;		/**<
														Tracking convergence condition.<br>
														4.8 fixed point value. [0x000~0x8FF].<br>
														The value is set to FPTFTCTL4.MAXERR[11:0] register.<br>
													*/
	USHORT					num_iteration;			/**<
														The number of tracking iteration. [1~10]<br>
														The value is set to FPTFTCTL4.MAXITR[3:0] register.<br>
													*/
	UINT32					base_img_addr;			/**<
														Beginning address of base image to be processed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL5.SDAD_BP_OF[31:0] register.<br>
													*/
	UINT32					target_img_addr;		/**<
														Beginning address of target image to be processed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL6.SDAD_TP_OF[31:0] register.<br>
													*/
	UINT32					base_coord_addr;		/**<
														Beginning address of base image's feature point coord data.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL7.SDAD_FT_CXY[31:0] register.<br>
													*/
	UINT32					target_coord_addr;		/**<
														Beginning address of target image's feature point coord data.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL8.SDAD_FT_BXY[31:0] register.<br>
													*/
	UINT32					tracking_result_addr;	/**<
														Beginning address of tracking output data, motion vectors. <br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL9.SDAD_FT_TXY[31:0] register.<br>
													*/
} T_IM_FPT_MODE2;

/**
 Parameters of Im_FPT_Ctrl_Mode3().
*/
typedef struct {
	USHORT					width;					/**<
														The number of horizontal pixels to be processed.<br>
														Should be less than or equal to T_IM_FPT_MODE3::base_global_width.<br>
														The value is set to FPTFQCTL1.HSIZE[14:0] register.<br>
														This setting range depends on T_IM_FPT_MODE3::down_sample_x and T_IM_FPT_MODE3::window_size.<br>
														The calculation formula is the following.<br>
														<table>
															<tr>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_5x5</th>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_7x7</th>
															</tr>
															<tr>
																<th>min</th>
																<th>max</th>
																<th>min</th>
																<th>max</th>
															</tr>
															<tr>
																<td>13 * T_IM_FPT_MODE3::down_sample_x</td>
																<td>32 * T_IM_FPT_MODE3::down_sample_x</td>
																<td>15 * T_IM_FPT_MODE3::down_sample_x</td>
																<td>32 * T_IM_FPT_MODE3::down_sample_x</td>
															</tr>
														</table>
														Please set the odd number in Mode3 (Re-calculate Feature Amount & Feature point Tracking).<br>
														In case of binning, please set the odd number to the value after binning.<br>
														(When the value after binning is 15 and binning rate is 1/4, Please set 60 to the value.)<br>
													*/
	USHORT					lines;					/**<
														The number of vertical pixels to be processed.<br>
														The value is set to FPTFQCTL1.VSIZE[14:0] register.<br>
														This setting range depends on T_IM_FPT_MODE3::down_sample_y and T_IM_FPT_MODE3::window_size.<br>
														<table>
															<tr>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_5x5</th>
																<th colspan="2">window_size<br>D_IM_FPT_WINDOW_7x7</th>
															</tr>
															<tr>
																<th>min</th>
																<th>max</th>
																<th>min</th>
																<th>max</th>
															</tr>
															<tr>
																<td>13 * T_IM_FPT_MODE3::down_sample_y</td>
																<td>32 * T_IM_FPT_MODE3::down_sample_y</td>
																<td>15 * T_IM_FPT_MODE3::down_sample_y</td>
																<td>32 * T_IM_FPT_MODE3::down_sample_y</td>
															</tr>
														</table>
													*/
	UCHAR					window_size;			/**<
														The size of sum window.<br>
														<ul>
														<li> @ref D_IM_FPT_WINDOW_5x5	[FPTFQCTL2.SUMW=0b]
														<li> @ref D_IM_FPT_WINDOW_7x7	[FPTFQCTL2.SUMW=1b]
														</ul>
													*/
	UINT32					output_addr;			/**<
														Beginning address of output data.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFQCTL7.SDAD_FQ_RXY[31:0] register.<br>
													*/
	USHORT					base_global_width;		/**<
														The number of pixels of base image's 1 line.<br>
														It means line feed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL0.GHBSIZE[14:0] register.<br>
													*/
	USHORT					target_global_width;	/**<
														The number of pixels of target image's 1 line.<br>
														It means line feed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL0.GHTSIZE[14:0] register.<br>
													*/
	UCHAR					num_feature_point;		/**<
														The number of feature point to be tracked.[1~127].<br>
														The value is set to FPTFTCTL1.FTMAX[6:0] register.<br>
													*/
	UCHAR					usage_target_coord;		/**<
														Control the usage of target image's coord
														<ul>
														<li> @ref D_IM_FPT_READ_FROM_RAM      [FPTFTCTL1.INIT_0 = 0b]
														<li> @ref D_IM_FPT_SAME_AS_BASE_COORD [FPTFTCTL1.INIT_0 = 1b]
														</ul>
														The value is set to FPTFTCTL1.INIT_0 register.<br>
													*/
	USHORT					down_sample_x;			/**<
														Horizontal down-sampling for tracking.(down_sample_x/256)<br>
														value range  :[256 - 5120]<br>
														The value is set to FPTFTCTL10.SKIPX_FT[12:0] register.<br>
														
													*/
	USHORT					down_sample_y;			/**<
														Vertical down-sampling for tracking.(down_sample_y/256)<br>
														value range  :[256 - 5120]<br>
														The value is set to FPTFTCTL10.SKIPY_FT[12:0] register.<br>
													*/
	E_IM_FPT_LUTCFG			lutcfg;					/**<
														LUT control for input iamge.<br>
														<ul>
														<li> @ref E_IM_FPT_LUTCFG_OFF				[FPTCTL3.BRC_EN=0b]
														<li> @ref E_IM_FPT_LUTCFG_BASE_0			[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=0b, FPTCTL3.BRC_MOD=00b]
														<li> @ref E_IM_FPT_LUTCFG_TARGET_1			[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=1b, FPTCTL3.BRC_MOD=01b]
														<li> @ref E_IM_FPT_LUTCFG_BASE_0__TARGET_1	[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=0b, FPTCTL3.BRC_MOD=10b]
														<li> @ref E_IM_FPT_LUTCFG_BASE_1__TARGET_0	[FPTCTL3.BRC_EN=1b, FPTCTL3.LUT_SEL=1b, FPTCTL3.BRC_MOD=10b]
														</ul>
													*/
	T_IM_FPT_SEARCH_WINDOW	search_window;			/**<
														Search window.<br>
													*/
	USHORT					tracking_threshold;		/**<
														Tracking convergence condition.<br>
														4.8 fixed point value. [0x000~0x8FF].<br>
														The value is set to FPTFTCTL4.MAXERR[11:0] register.<br>
													*/
	USHORT					num_iteration;			/**<
														The number of tracking iteration. [1~10]<br>
														The value is set to FPTFTCTL4.MAXITR[3:0] register.<br>
													*/
	UINT32					base_img_addr;			/**<
														Beginning address of base image to be processed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL5.SDAD_BP_OF[31:0] register.<br>
													*/
	UINT32					target_img_addr;		/**<
														Beginning address of target image to be processed.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL6.SDAD_TP_OF[31:0] register.<br>
													*/
	UINT32					base_coord_addr;		/**<
														Beginning address of base image's feature point coord data.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL7.SDAD_FT_CXY[31:0] register.<br>
													*/
	UINT32					target_coord_addr;		/**<
														Beginning address of target image's feature point coord data.<br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL8.SDAD_FT_BXY[31:0] register.<br>
													*/
	UINT32					tracking_result_addr;	/**<
														Beginning address of tracking output data, motion vectors. <br>
														Should be aligned on 8.<br>
														The value is set to FPTFTCTL9.SDAD_FT_TXY[31:0] register.<br>
													*/
} T_IM_FPT_MODE3;

/** 
   Parameters of Im_FPT_Ctrl_Mode_Config().
 */
typedef struct {
	E_IM_FPT_IO_MODE	io_mode;					/**<
														The settings for the input and output method of feature point coordinates and the processing result.<br>
													*/
	E_IM_FPT_PIPE_EN	pipe_en;					/**<
														At the time of the feature point tracking, the settings of whether reading pixel data from the SDRAM and the internal processing executed with pipeline processing.<br>
														Please set "E_IM_FPT_PIPE_EN_DISABLE".
													*/
	E_IM_FPT_DEBUG_MODE	debug_mode;					/**<
														The setting of debug mode.<br>
													*/
} T_IM_FPT_MODE_CONFIG;

/** 
   Parameters of Im_FPT_Ctrl_BRC_Config_Base() and Im_FPT_Ctrl_BRC_Config_Target().
 */
typedef struct {
	USHORT		img_offset;							/**< Offset values for the brightness correction of the base and target image.<br>
														This value is signed.<br>
														The value is set to FPTCTL4.OFFSET_B[24:16], FPTCTL5.OFFSET_T[24:16] register.<br>
													*/
	UCHAR		img_clip_max;						/**< Upper limit clip value after the offset for the brightness correction of the base and target image.<br>
														The value is set to FPTCTL4.CLPMAX_B[15:8], FPTCTL5.CLPMAX_T[15:8] register.<br>
													*/
	UCHAR		img_clip_min;						/**< Lower limit clip value after the offset for the brightness correction of the base and target image.<br>
														The value is set to FPTCTL4.CLPMIN_B[7:0], FPTCTL5.CLPMIN_T[7:0] register.<br>
													*/
} T_IM_FPT_BRC_CONFIG;

/** 
   Parameters of Im_FPT_Ctrl_Interrupt().
 */
typedef struct {
	UINT32		flag;								/**<
														Control interrupt.<br>
														<ul>
														<li> @ref D_IM_FPT_INTERRUPT_COMPLETE  [FPTINTE0.FPTEN = 1b]
														<li> @ref D_IM_FPT_INTERRUPT_AXI_ERR   [FPTINTE0.AXEEN = 1b]
														<li> @ref D_IM_FPT_INTERRUPT_PARAM_ERR [FPTINTE0.PMEEN = 1b]
														</ul>
														If (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR) is set to it,
														FPTINTE0.FPTEN and FPTINTE0.AXEEN is set to 1, and FPTINTE0.PMEEN is set to 0.
													*/
	VOID		(*callback)(UINT32 int_factor, UCHAR ch);	/**<
														The pointer of callback function.<br>
														The callback function can be called in Im_FPT_Int_Handler().<br>
														If this parameter is set to NULL, Im_FPT_Int_Handler() does not call callback function. <br><br>
														Sample of callback function.<br>
@code
VOID Im_FPT_User_Callback( UINT32 interrupt_flag )
{
	if ( interrupt_flag & D_IM_FPT_INTERRUPT_COMPLETE ) {
		// process according to FPT completion interrupt
	}
	if ( interrupt_flag & D_IM_FPT_INTERRUPT_AXI_ERR ) {
		// process according to AXI Error interrupt
	}
	if ( interrupt_flag & D_IM_FPT_INTERRUPT_PARAM_ERR ) {
		// process according to Parameter Error interrupt
	}
}
@endcode
													*/
} T_IM_FPT_INTERRUPT;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif


/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

//---------------------------- driver section ----------------------------
/**
@weakgroup fpt_drv
@{
*/

/**
Initialize driver.<br>

@param[in]	ch :Channel number(0/1)
@retval		D_IM_FPT_RETVAL_OK				: @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
@retval		D_IM_FPT_RETVAL_ERR				: @ref D_IM_FPT_RETVAL_ERR (FPT is running.)
@remarks	It should be called at first.
@remarks	It should be called when FPT is not running.
*/
extern INT32 Im_FPT_Init( UCHAR ch );

/**
Open driver.<br>
Exclusive access control API.<br>

@param[in]	ch :Channel number(0/1)
@param[in]	wait_time : Wait time [msec]. The valid range is -1, 0, ... .
						If this parameter is set to -1,
						API waits permanently unless system-call gets FPT semaphore.
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
@retval		D_IM_FPT_RETVAL_SYSTEM_CALL_ERR : @ref D_IM_FPT_RETVAL_SYSTEM_CALL_ERR
@remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern INT32 Im_FPT_Open( UCHAR ch, INT32 wait_time );

/**
Close driver.<br>
Exclusive access control API.<br>

@param[in]	ch :Channel number(0/1)
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_SYSTEM_CALL_ERR : @ref D_IM_FPT_RETVAL_SYSTEM_CALL_ERR
@retval		D_IM_FPT_RETVAL_ERR : @ref D_IM_FPT_RETVAL_ERR (FPT is running.)
@remarks	It should be called when FPT is not running.
@remarks	This API uses DDIM_User_Sig_Sem().
*/
extern INT32 Im_FPT_Close( UCHAR ch );

/**
Configure the mode, "Calculate Feature Amount".<br>
Please refer to @ref fpt_mode0.

@param[in]	ch :Channel number(0/1)
@param[in]	param : parameter. Please refer to @ref T_IM_FPT_MODE0.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
*/
extern INT32 Im_FPT_Ctrl_Mode0( UCHAR ch, const T_IM_FPT_MODE0* const param );

/**
Configure the mode, "Re-calculate Feature Amount".<br>
Please refer to @ref fpt_mode1.

@param[in]	ch :Channel number(0/1)
@param[in]	param : parameter. Please refer to @ref T_IM_FPT_MODE1.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
*/
extern INT32 Im_FPT_Ctrl_Mode1( UCHAR ch, const T_IM_FPT_MODE1* const param );

/**
Configure the mode, "Feature point Tracking".<br>
Please refer to @ref fpt_mode2.

@param[in]	ch :Channel number(0/1)
@param[in]	param : parameter. Please refer to @ref T_IM_FPT_MODE2.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
*/
extern INT32 Im_FPT_Ctrl_Mode2( UCHAR ch, const T_IM_FPT_MODE2* const param );

/**
Configure the mode, "Re-calculate Feature Amount and Feature point Tracking".<br>
Please refer to @ref fpt_mode3.

@param[in]	ch :Channel number(0/1)
@param[in]	param : parameter. Please refer to @ref T_IM_FPT_MODE3.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
*/
extern INT32 Im_FPT_Ctrl_Mode3( UCHAR ch, const T_IM_FPT_MODE3* const param );

/**
Configure the mode.<br>

@param[in]	ch :Channel number(0/1)
@param[in]	param : parameter. Please refer to @ref T_IM_FPT_MODE_CONFIG.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
*/
extern INT32 Im_FPT_Ctrl_Mode_Config( UCHAR ch, const T_IM_FPT_MODE_CONFIG* const param );

/**
Configure BRC(BRightness Correction) of the base image.<br>

@param[in]	ch :Channel number(0/1)
@param[in]	param : parameter. Please refer to @ref T_IM_FPT_BRC_CONFIG.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
*/
extern INT32 Im_FPT_Ctrl_BRC_Config_Base_Img( UCHAR ch, const T_IM_FPT_BRC_CONFIG* const param );

/**
Configure BRC(BRightness Correction) of the target image.<br>

@param[in]	ch :Channel number(0/1)
@param[in]	param : parameter. Please refer to @ref T_IM_FPT_BRC_CONFIG.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
*/
extern INT32 Im_FPT_Ctrl_BRC_Config_Target_Img( UCHAR ch, const T_IM_FPT_BRC_CONFIG* const param );

/**
Configure Interrupt.<br>

@param[in]	ch :Channel number(0/1)
@param[in]	param : parameter. Please refer to @ref T_IM_FPT_INTERRUPT.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
*/
extern INT32 Im_FPT_Ctrl_Interrupt( UCHAR ch, const T_IM_FPT_INTERRUPT* const param );

/**
Configure LUTRAM.<br>
<br>
In LUT function, FPT process the value of input pixel as follows:<br>
lut[ the value of input pixel ]<br>

@param[in]	ch :Channel number(0/1)
@param[in]	lut_sel : FPTLUT0 or FPTLUT1<br>
					<ul>
						<li>0  : FPTLUT0 is used.
						<li>1  : FPTLUT1 is used.
					</ul>
@param[in]	lut : Beginning address of LUTRAM buffer [256Bytes].<br>
				  These value is copied on the LUTRAM.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
*/
extern INT32 Im_FPT_Ctrl_LUTRAM( UCHAR ch, INT32 lut_sel, const UCHAR* lut );

/**
Set REGRAM.<br>
This area is used to store the input coordinates and the operation result register.<br>

@param[in]	ch :Channel number(0/1)
@param[in]	reg_sel : FPTREG0 or FPTREG1<br>
					<ul>
						<li>0  : FPTREG0 is used.
						<li>1  : FPTREG1 is used.
					</ul>
@param[in]	reg : Beginning address of REGRAM buffer [512Bytes].<br>
				  These value is copied on the REGRAM.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
*/
extern INT32 Im_FPT_Set_REGRAM( UCHAR ch, INT32 reg_sel, const UCHAR* reg );

/**
Get REGRAM.<br>
This area is used to store the input coordinates and the operation result register.<br>

@param[in]	ch :Channel number(0/1)
@param[in]	reg_sel : FPTREG0 or FPTREG1<br>
					<ul>
						<li>0  : FPTREG0 is used.
						<li>1  : FPTREG1 is used.
					</ul>
@param[in]	buf_addr : Beginning address of the buffer [512Bytes].<br>
					   These value is copied from the REGRAM.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
@remarks	This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_FPT_Get_REGRAM( UCHAR ch, INT32 reg_sel, ULONG* buf_addr );

/**
Start FPT.<br>

@param[in]	ch :Channel number(0/1)
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_ERR : @ref D_IM_FPT_RETVAL_ERR (FPT is running.)
@remarks	It should be called when FPT is not running.
*/
extern INT32 Im_FPT_Start( UCHAR ch );

/**
Force stop FPT.<br>

@param[in]	ch :Channel number(0/1)
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_SYSTEM_CALL_ERR : @ref D_IM_FPT_RETVAL_SYSTEM_CALL_ERR
@retval		D_IM_FPT_RETVAL_ERR : @ref D_IM_FPT_RETVAL_ERR (FPT is running.)
@remarks	It should be called when FPT is running.
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern INT32 Im_FPT_Stop( UCHAR ch );

/**
Wait process until FPT interrupt occurs.<br>

@param[in]	ch :Channel number(0/1)
@param[out]	status : Status.<br>
					<ul>
						<li>bit0  : If bit0  is set to 1, FPT completion interrupt has occured.
						<li>bit4  : If bit4  is set to 1, AXI error interrupt has occured.
						<li>bit8  : If bit8  is set to 1, parameter error interrupt has occured.
						<li>bit12 : If bit12 is set to 1, force stop has occured. It means that Im_FPT_Stop() is called while Im_FPT_WaitEnd().
					</ul>

@param[in]	wait_time : Wait time [msec]. The valid range is -1, 0, ... .
						If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
@retval		D_IM_FPT_RETVAL_SYSTEM_CALL_ERR : @ref D_IM_FPT_RETVAL_SYSTEM_CALL_ERR
@remarks	This API uses DDIM_User_Clr_Flg().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Im_FPT_WaitEnd( UCHAR ch, UINT32* status, INT32 wait_time );

/**
Interrupt Handler.<br>

@param[in]	ch :Channel number(0/1)
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_SYSTEM_CALL_ERR : @ref D_IM_FPT_RETVAL_SYSTEM_CALL_ERR
@remarks	This API should be called when interrupt of INTID 104 has occured.<br>
@remarks	If the address of callback funtion is set to T_IM_FPT_INTERRUPT::callback, this api calls callback function.
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern INT32 Im_FPT_Int_Handler( UCHAR ch );

/**
Get AXI response.<br>

@param[in]	ch :Channel number(0/1)
@param[out]	read_response  : The value of FPTAXIERR0.RRESP register.<br>.
							 It means that read response signal, RRESP.<br>
@param[out]	write_response : The value of FPTAXIERR0.WRESP register.<br>.
							 It means that write response signal, BRESP.<br>
@retval		D_IM_FPT_RETVAL_OK : @ref D_IM_FPT_RETVAL_OK
@retval		D_IM_FPT_RETVAL_INVALID_ARG_ERR	: @ref D_IM_FPT_RETVAL_INVALID_ARG_ERR
@remarks	This API should be used only for the purpose of debug.<br>
@remarks	This API should be used right after AXI Error interrupt has occured.<br>
*/
extern INT32 Im_FPT_Get_Axi_Response( UCHAR ch, UCHAR* read_response, UCHAR* write_response );

/**
Dump the value of all FPT's register.<br>

@param[in]	ch :Channel number(0/1)
@param[in]	buf0_addr : Beginning address of buffer [256Bytes].<br>
						The value of APB registers is copied on the buffer.<br>
						Should be aligned on 4.<br>
@param[in]	buf1_addr : Beginning address of buffer [256Bytes x2 = 512Bytes].<br>
						The value of AHB registers is copied on the buffer.<br>
						Should be aligned on 4.<br>
@remarks	This API should be used only for the purpose of debug.<br>
*/
extern VOID Im_FPT_DebugDump_All_Registers( UCHAR ch, UINT32 buf0_addr, UINT32 buf1_addr );

/**
Get version info.<br>
@code
	CHAR *str;
	Im_FPT_Get_Version( &str );
	printf("%s\n", str);
@endcode

@param[out]	str : The pointer of version string.<br>
@remarks	This API should be used only for the purpose of debug.<br>
*/
extern VOID Im_FPT_Get_Version( CHAR** str );

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------

//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/** @} */	// end of fpt_drv

/**
@weakgroup fpt_usage Usage
@{

@section fpt_sample_seq Sequence Charts
- @ref FPT_INIT
- @ref FPT_EXE
- @ref FPT_STOP

<hr>

@section FPT_INIT	1. Initialization
@image html fpt_init.png

<hr>

@section FPT_EXE		2. Execution
@image html fpt_mode.png

@code

#include "fpt.h"
#define round_up( val, al )	( (val+(al-1)) & (~(al-1)) )	// "al" is power of 2.
  
VOID FPT_Sample( INT32 mode, const UCHAR* lut0, const UCHAR* lut1 )
{
	T_IM_FPT_MODE0		m0_param;
	T_IM_FPT_MODE1		m1_param;
	T_IM_FPT_MODE2		m2_param;
	T_IM_FPT_MODE3		m3_param;
	T_IM_FPT_INTERRUPT	int_param;
	UINT32				status;
	int					offset;
	
	Im_FPT_Open( 0, 0 );
	
	switch ( mode ) {
		case 0:
			m0_param.global_width = 1568;
			m0_param.width = 1568;
			m0_param.lines = 1050;
			m0_param.lutsel = ( lut0 != NULL ) ? E_IM_FPT_LUTSEL_0 : E_IM_FPT_LUTSEL_OFF:
			m0_param.window_size = D_IM_FPT_WINDOW_7x7;
			m0_param.down_sample_x = D_IM_FPT_DS_MAX;
			m0_param.down_sample_y = D_IM_FPT_DS_MAX;
			m0_param.img_addr = 0x43000000;
			m0_param.feature_amount_addr = 0x43800000;
			offset = ( m0_param.window_size == D_IM_FPT_WINDOW_5x5 ) ? 12 : 14;
			m0_param.feature_amount_line_byte = round_up( (((1568/7) - offset) * 2), 8 );
			m0_param.histgram_addr = 0x44000000;
			
			Im_FPT_Ctrl_Mode0( 0, &m0_param );
			break;
		case 1:
			m1_param.num_feature_point = 100;
			m1_param.output = D_IM_FPT_OUT_COORD_AND_FEATURE_AMOUNT;
			m1_param.global_width = 896;
			m1_param.width = 128;
			m1_param.lines = 128;
			m1_param.lutsel = ( lut0 != NULL ) ? E_IM_FPT_LUTSEL_0 : E_IM_FPT_LUTSEL_OFF:
			m1_param.window_size = D_IM_FPT_WINDOW_7x7;
			m1_param.down_sample_x = 0x0400;
			m1_param.down_sample_y = 0x0400;
			m1_param.img_addr = 0x43000000;
			m1_param.input_coord_addr = 0x43800000;
			m1_param.output_addr = 0x44000000;
			
			Im_FPT_Ctrl_Mode1( 0, &m1_param );
			break;
		case 2:
			m2_param.base_global_width = 896;
			m2_param.target_global_width = 896;
			m2_param.num_feature_point = 100;
			m2_param.usage_target_coord = D_IM_FPT_READ_FROM_RAM;
			m2_param.down_sample_x = 0x0400;
			m2_param.down_sample_y = 0x0400;
			m2_param.lutcfg = ( (lut0 != NULL) && ((lut1 != NULL)) ) ? E_IM_FPT_LUTCFG_BASE_0__TARGET_1 :
							  (  lut0 != NULL ) ? E_IM_FPT_LUTCFG_BASE_0 :
							  (  lut1 != NULL ) ? E_IM_FPT_LUTCFG_TARGET_1 :
												  E_IM_FPT_LUTCFG_OFF;
			m2_param.search_window.left = 0x700;	// 7.0
			m2_param.search_window.right = 0x700;	// 7.0
			m2_param.search_window.top = 0x700;		// 7.0
			m2_param.search_window.bottom = 0x700;	// 7.0
			m2_param.tracking_threshold = 0x33;		// 0.2
			m2_param.num_iteration = 10;
			m2_param.base_img_addr = 0x43000000;
			m2_param.target_img_addr = 0x43800000;
			m2_param.base_coord_addr = 0x44000000;
			m2_param.target_coord_addr = 0x44800000;
			m2_param.tracking_result_addr = 0x45000000;
			
			Im_FPT_Ctrl_Mode2( 0, &m2_param );
			break;
		case 3:
			m3_param.width = 128;
			m3_param.lines = 128;
			m3_param.window_size = D_IM_FPT_WINDOW_7x7;
			m3_param.output_addr = 0x46000000;
			m3_param.base_global_width = 896;
			m3_param.target_global_width = 896;
			m3_param.num_feature_point = 100;
			m3_param.usage_target_coord = D_IM_FPT_READ_FROM_RAM;
			m3_param.down_sample_x = 0x0400;
			m3_param.down_sample_y = 0x0400;
			m3_param.lutcfg = ( (lut0 != NULL) && ((lut1 != NULL)) ) ? E_IM_FPT_LUTCFG_BASE_0__TARGET_1 :
							  (  lut0 != NULL ) ? E_IM_FPT_LUTCFG_BASE_0 :
							  (  lut1 != NULL ) ? E_IM_FPT_LUTCFG_TARGET_1 :
												  E_IM_FPT_LUTCFG_OFF;
			m3_param.search_window.left = 0x700;	// 7.0
			m3_param.search_window.right = 0x700;	// 7.0
			m3_param.search_window.top = 0x700;		// 7.0
			m3_param.search_window.bottom = 0x700;	// 7.0
			m3_param.tracking_threshold = 0x33;		// 0.2
			m3_param.num_iteration = 10;
			m3_param.base_img_addr = 0x43000000;
			m3_param.target_img_addr = 0x43800000;
			m3_param.base_coord_addr = 0x44000000;
			m3_param.target_coord_addr = 0x44800000;
			m3_param.tracking_result_addr = 0x45000000;
			
			Im_FPT_Ctrl_Mode3( 0, &m3_param );
			break;
		default:
			break;
	}
	
	if ( lut0 != NULL ) {
		Im_FPT_Ctrl_LUTRAM( 0, 0, lut0 );
	}

	if ( lut1 != NULL ) {
		Im_FPT_Ctrl_LUTRAM( 0, 1, lut1 );
	}

	int_param.flag = ( D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR );
	int_param.callback = NULL;	// If needs callback, please set the address of callback function.
	Im_FPT_Ctrl_Interrupt( 0, &int_param );
	
	Im_FPT_Start( 0 );
	
	Im_FPT_WaitEnd( 0, &status, 5000 );
	
	Im_FPT_Close( 0 );
}
@endcode

<hr>

@section FPT_STOP	3. Force Stop
@image html fpt_stop.png

<hr>
*/
/** @} */	// end of fpt_usage

/**
@weakgroup fpt_overview Overview
@{

FPT has the following 3 function:

- @ref fpt_mode0
- @ref fpt_mode1
- @ref fpt_mode2
- @ref fpt_mode3

For more information, please refer to FPT HW specification.

<hr>

@section fpt_mode0	Mode0 (Calculate Feature Amount)
Following figure shows input/output data in mode0.<br>
In the figure, T_IM_FPT_MODE0::xxx is the parameter of Im_FPT_Ctrl_Mode0().<br>
FPT outputs "Feature Amount" and "Histgram" using "Input image".<br>
@image html fpt_func_mode0.png
The data format of 1 component is the followings:<br>
- [IN]Input image: U8 (unsigned, 8 bits)
<br>
- [OUT]Feature Amount: S16 (signed, 16 bits)
<br>
- [OUT]Histgram: U16 (unsigned, 16 bits)
<br>

<hr>

@section fpt_mode1	Mode1 (Re-calculate Feature Amount)
Following figure shows input/output data in mode1.<br>
In the figure, T_IM_FPT_MODE1::xxx is the parameter of Im_FPT_Ctrl_Mode1().<br>
FPT reads "Input image" and "Input coord data".<br>
And output data depends on T_IM_FPT_MODE1::output setting.<br>
When T_IM_FPT_MODE1::output is set to @ref D_IM_FPT_OUT_COORD_ONLY (0), it contains feature point coord data only.<br>
When T_IM_FPT_MODE1::output is set to @ref D_IM_FPT_OUT_COORD_AND_FEATURE_AMOUNT (1), it contains feature point coord also feature amount data.<br>
@image html fpt_func_mode1.png
The data format of 1 component is the followings:<br>
- [IN]Input image: U8 (unsigned, 8 bits)
<br>
- [IN]Input coord data: U15+U15, please refer to following table.
<table>
	<tr>
		<th>bit31</th>
		<th>bit[30:16]</th>
		<th>bit15</th>
		<th>bit[14:0]</th>
	</tr>
	<tr>
		<td>unused [0]</td>
		<td>vertical coord, U15</td>
		<td>unused [0]</td>
		<td>horizontal coord, U15</td>
	</tr>
</table>
<br>
- [OUT]Only Feature point coord data: U15+U15, please refer to following table.
<table>
	<tr>
		<th>bit31</th>
		<th>bit[30:16]</th>
		<th>bit15</th>
		<th>bit[14:0]</th>
	</tr>
	<tr>
		<td>unused [0]</td>
		<td>vertical coord, U15</td>
		<td>unused [0]</td>
		<td>horizontal coord, U15</td>
	</tr>
</table>
<br>
- [OUT]Feature Amount + Feature point coord data: S16+U15+U15, please refer to following table.
<table>
	<tr>
		<th>bit[63:48]</th>
		<th>bit[47:32]</th>
		<th>bit31</th>
		<th>bit[30:16]</th>
		<th>bit15</th>
		<th>bit[14:0]</th>
	</tr>
	<tr>
		<td>unused [0]</td>
		<td>feature amount, S16</td>
		<td>unused [0]</td>
		<td>vertical coord, U15</td>
		<td>unused [0]</td>
		<td>horizontal coord, U15</td>
	</tr>
</table>
<br>
Please note that horizontal/vertical coord data U15 is integer only.<br>

<hr>

@section fpt_mode2	Mode2 (Feature point Tracking)
Following figure shows input/output data in mode2.<br>
In the figure, T_IM_FPT_MODE2::xxx is the parameter of Im_FPT_Ctrl_Mode2().<br>
In this mode, FPT tracks feature point using base and target image.<br>
When T_IM_FPT_MODE2::usage_target_coord is set to @ref D_IM_FPT_READ_FROM_RAM (0), FPT reads target's feature point coord data.<br>
Output contains tracking result.<br>
@image html fpt_func_mode2.png
The data format of 1 component is the followings:<br>
- [IN]Target/Base image: U8 (unsigned, 8 bits)
<br>
- [IN]Target/Base coord data: U15+U15, please refer to following table.
<table>
	<tr>
		<th>bit31</th>
		<th>bit[30:16]</th>
		<th>bit15</th>
		<th>bit[14:0]</th>
	</tr>
	<tr>
		<td>unused [0]</td>
		<td>vertical coord, U15</td>
		<td>unused [0]</td>
		<td>horizontal coord, U15</td>
	</tr>
</table>
Please note that horizontal/vertical coord data U15 is integer only.<br>
<br>
- [OUT]Tracking result: S13+U2+S13, please refer to following table.
<table>
	<tr>
		<th>bit[31:29]</th>
		<th>bit[28:16]</th>
		<th>bit15</th>
		<th>bit[14:13]</th>
		<th>bit[12:0]</th>
	</tr>
	<tr>
		<td>unused [0]</td>
		<td>vertical vector, S13[*1]</td>
		<td>unused [0]</td>
		<td>tracking status, U2[*2]</td>
		<td>horizontal vector, S13 [*1]</td>
	</tr>
</table>
[*1]: signed 13bits. 5.8 fixed point value. MSB (bit12) means sign bit. <br>
[*2]: unsigned 2 bits. 
		<ul>
			<li>0: Tracking success.<br>
			<li>1: Outside the range of tracking.<br>
			<li>2: Exceed the maximum number of iteration.<br>
			<li>3: Outside the range of tracking & Exceed the maximum number of iteration<br>
		</ul>
<br>

@section fpt_mode3	Mode3 (Re-calculate Feature Amount & Feature point Tracking)
In this mode, Automatically and continuously and run the Mode1 and Mode2.<br>
<br>

<hr>
*/
/** @} */	// end of fpt_overview

#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// _IM_FPT_H_
/** @} */	// end of im_fpt


