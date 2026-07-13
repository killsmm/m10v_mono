/**
 * @file		im_shdr.h
 * @brief		JDSSHDR macro wrapper.
 * @note		None.
 * @attention	None.
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
	@addtogroup im_shdr
	@{
*/

#ifndef __IM_SHDR_H__
#define __IM_SHDR_H__

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Result.
#define	D_IM_SHDR_INPUT_PARAM_ERROR			(D_IM_SHDR | D_DDIM_INPUT_PARAM_ERROR)	/**< Processing result code<br> Input parameter error			(0x0E000001) */
#define	D_IM_SHDR_MACRO_BUSY				(D_IM_SHDR | D_DDIM_MACRO_BUSY_NG)		/**< Processing result code<br> Macro busy 						(0x0E000005) */
#define D_IM_SHDR_SEM_NG					(D_IM_SHDR | D_DDIM_SEM_NG)				/**< Processing result code<br> Semaphore acquisition NG		(0x0E000010) */
#define	D_IM_SHDR_SEM_TIMEOUT_ERR			(D_IM_SHDR | D_DDIM_SEM_TIMEOUT)		/**< Processing result code<br> Semaphore acquisition Time Out	(0x0E000011) */
#define	D_IM_SHDR_TIMEOUT					(D_IM_SHDR | D_DDIM_TIMEOUT)			/**< Processing result code<br> Time Out 						(0x0E000013) */
#define	D_IM_SHDR_AXI_ERROR					(D_IM_SHDR | D_DDIM_AXI_ERROR)			/**< Processing result code<br> AXI BUS error					(0x0E00001A) */
#define	D_IM_SHDR_NG						(D_IM_SHDR | D_DDIM_SYSTEM_ERROR)		/**< Processing result code<br> Processing NG					(0x0E000099) */
#define	D_IM_SHDR_PROCESS_END				(D_IM_SHDR | 0x000000FF)				/**< Processing result code<br> Process End 					(0x0E0000FF) */

// AXI.
#define	D_IM_SHDR_AXI_READ_CH_DATA_MAX		(4)			/**< The maximum values of AXI read channel data.					*/
#define	D_IM_SHDR_AXI_WRITE_CH_DATA_MAX		(7)			/**< The maximum values of AXI write channel data.					*/

#define D_IM_SHDR_NORMAL_END				(0x00)		/**< Normal end.													*/
#define D_IM_SHDR_AXI_ERR_CH_W				(0x01)		/**< The AXI error occurred by the write channel.					*/
#define D_IM_SHDR_AXI_ERR_CH_R				(0x02)		/**< The AXI error occurred by the read channel.					*/
#define D_IM_SHDR_AXI_ERR_CH_RW				(0x03)		/**< The AXI error occurred by the read channel and write channel.	*/

// Degamma table.
#define D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE	(256)		/**< The maximum values of degamma table.	*/

// OFF/ON.
#define D_IM_SHDR_OFF						(0)			/**< OFF.									*/
#define D_IM_SHDR_ON						(1)			/**< ON.									*/

// DISABLE/ENABLE.
#define D_IM_SHDR_DISABLE					(0)			/**< DISABLE.								*/
#define D_IM_SHDR_ENABLE					(1)			/**< ENABLE.								*/

// Chroma up sampling mode.
#define D_IM_SHDR_CUS_YCC422_BILINEAR_A		(0)			/**< YCC422 bilinear mode A.				*/
#define D_IM_SHDR_CUS_YCC422_BILINEAR_B		(1)			/**< YCC422 bilinear mode B.				*/
#define D_IM_SHDR_CUS_YCC422_PADDING		(2)			/**< YCC422 padding mode.					*/
#define D_IM_SHDR_CUS_YCC420_BILINEAR_A		(3)			/**< YCC420 bilinear mode A.				*/
#define D_IM_SHDR_CUS_YCC420_BILINEAR_B		(4)			/**< YCC420 bilinear mode B.				*/
#define D_IM_SHDR_CUS_YCC420_PADDING		(5)			/**< YCC420 padding mode.					*/

// Chroma up sampling mode.
#define D_IM_SHDR_DEGAMMA_LUT				(0)			/**< Degamma(LUT).							*/
#define D_IM_SHDR_DEGAMMA_BITEXP			(1)			/**< Bit expansion.							*/

// Input image bit depth.
#define D_IM_SHDR_INBITDEPTH_8				(0)			/**< 8 bit.									*/
#define D_IM_SHDR_INBITDEPTH_16				(1)			/**< 16 bit.								*/

// Input CbCr image format.
#define D_IM_SHDR_INCFMT_420SP				(0)			/**< 420 semi-planar.						*/
#define D_IM_SHDR_INCFMT_422SP				(1)			/**< 422 semi-planar.						*/

// Output image control.
#define D_IM_SHDR_OUTCTRL_YCC				(0)			/**< YCC output path.						*/
#define D_IM_SHDR_OUTCTRL_RGB				(1)			/**< RGB output path.						*/

// Output image select.
#define D_IM_SHDR_OUTSEL_YCC_16				(0)			/**< YCC 16 bit image.						*/
#define D_IM_SHDR_OUTSEL_RGB_16				(1)			/**< RGB 16 bit image.						*/

// Filter margin.
#define D_IM_SHDR_FLITER_MARGIN_16			(16)		/**< 16.									*/
#define D_IM_SHDR_FLITER_MARGIN_32			(32)		/**< 32.									*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define D_IM_SHDR_FLITER_MARGIN_96			(96)		/**< 96.									*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// SMC input select.
#define D_IM_SHDR_SMCIN_STN_REF				(0)			/**< SMC input standard image  : Standard image<br>
															 SMC input reference image : Reference image<br>
														*/
#define D_IM_SHDR_SMCIN_EXT_STN				(1)			/**< SMC input standard image  : Extra image<br>
															 SMC input reference image : Standard image<br>
														*/
#define D_IM_SHDR_SMCIN_EXT_REF				(2)			/**< SMC input standard image  : Extra image<br>
															 SMC input reference image : Reference image<br>
														*/
// SHDR input select.
#define D_IM_SHDR_SHDRIN_STN_REF			(0)			/**< SHDR input standard image  : Standard image<br>
															 SHDR input reference image : Reference image<br>
														*/
#define D_IM_SHDR_SHDRIN_STN_SMCOUT			(1)			/**< SHDR input standard image  : Standard image<br>
															 SHDR input reference image : SMC output image<br>
														*/
#define D_IM_SHDR_SHDRIN_SMCOUT_REF			(2)			/**< SHDR input standard image  : SMC output image<br>
															 SHDR input reference image : Reference image<br>
														*/

// Tile size select.
#define D_IM_SHDR_TILESIZE_1024x1024		(0)			/**< Horizontal:1024 x Vertical:1024.		*/
#define D_IM_SHDR_TILESIZE_1024x512			(1)			/**< Horizontal:1024 x Vertical: 512.		*/

// Video format.
#define D_IM_SHDR_VFMT_RASTER				(0)			/**< Raster format.							*/
#define D_IM_SHDR_VFMT_VIDEO				(1)			/**< Video	format.							*/

// Video format(COMPIP WFID=A0/A1).
#define D_IM_SHDR_VFMT_COMPIP_VIDEO			(0)			/**< Video	format.							*/
#define D_IM_SHDR_VFMT_COMPIP_RASTER		(1)			/**< Raster format.							*/

// Weight base.
#define D_IM_SHDR_WGHTBASE_Y				(0)			/**< Y.										*/
#define D_IM_SHDR_WGHTBASE_RGB				(1)			/**< RGB.									*/

// Expansion process mode.
#define D_IM_SHDR_EXP77MD_COEFSUMDIV		(0)			/**< Coefficient sum division.				*/
#define D_IM_SHDR_EXP77MD_PERIMAXVAL		(1)			/**< Peripheral pixel maximum value.		*/

// FMD output image select.
#define D_IM_SHDR_FMDSEL_EXPANSION			(0)			/**< Expansion process image.				*/
#define D_IM_SHDR_FMDSEL_REDUCTION			(1)			/**< Reduction result merge image.			*/

// Pre mask mode.
#define D_IM_SHDR_PMSKMD_MAX				(0)			/**< Max value output.						*/
#define D_IM_SHDR_PMSKMD_AVE				(1)			/**< Average value output.					*/

// Mask merge mode.
#define D_IM_SHDR_MSKMD_SHORT				(0)			/**< Short exposure.						*/
#define D_IM_SHDR_MSKMD_LONG				(1)			/**< Long exposure.							*/
#define D_IM_SHDR_MSKMD_MSKONLY				(2)			/**< Motion mask only.						*/

// Blend select.
#define D_IM_SHDR_BLDSEL_SHORT_LONG			(0)			/**< Base   image : short exposure<br>
															 Target image : long  exposure<br>
														*/
#define D_IM_SHDR_BLDSEL_LONG_SHORT			(1)			/**< Base   image : long  exposure<br>
															 Target image : short exposure<br>
														*/

// Alpha value select.
#define D_IM_SHDR_ALPHASEL_DEVIATION		(0)			/**< Deviation + Margin condition.			*/
#define D_IM_SHDR_ALPHASEL_FIXED			(1)			/**< Fixed value (register setting).		*/
#define D_IM_SHDR_ALPHASEL_MERGEMASK		(2)			/**< Merge mask value.						*/

// EV compensation input data.
#define D_IM_SHDR_POSTSEL1_Y				(0)			/**< Y (BlendY12).							*/
#define D_IM_SHDR_POSTSEL1_RGB				(1)			/**< RGB max value (BlendRgbMax12).			*/

// EV compensation multiplication decimal point position.
#define D_IM_SHDR_POSTMULQPOS_Q5			(0)			/**< Q5.9  (Decimal part 9bit).				*/
#define D_IM_SHDR_POSTMULQPOS_Q4			(1)			/**< Q4.10 (Decimal part 10bit).			*/
#define D_IM_SHDR_POSTMULQPOS_Q3			(2)			/**< Q3.11 (Decimal part 11bit).			*/
#define D_IM_SHDR_POSTMULQPOS_Q2			(3)			/**< Q2.12 (Decimal part 12bit).			*/

// CSS input signal selection.
#define D_IM_SHDR_POSTSEL0_BLEND_YCC16		(0)			/**< BlendYcc16.							*/
#define D_IM_SHDR_POSTSEL0_BYPASS_YCC8		(1)			/**< BypassYcc8.							*/

// CSS sampling mode.
#define D_IM_SHDR_HDRCSSMD_YCC422_ODD		(0)			/**< YCC422 image output (odd sampling).	*/
#define D_IM_SHDR_HDRCSSMD_YCC422_CENTER	(1)			/**< YCC422 image output (center sampling).	*/

// Resize mode.
#define D_IM_RSZMD_ODD0						(0)			/**< Odd    -> Odd.							*/
#define D_IM_RSZMD_ODD1						(1)			/**< Odd    -> Center.						*/
#define D_IM_RSZMD_CENTER0					(2)			/**< Center -> Odd.							*/
#define D_IM_RSZMD_CENTER1					(3)			/**< Center -> Center.						*/

// Reduction mode 1.
#define D_IM_RDCMD1_1_4						(1)			/**< 1/4  reduction (output format : Y).		*/
#define D_IM_RDCMD1_1_8						(2)			/**< 1/8  reduction (output format : Y).		*/
#define D_IM_RDCMD1_1_32					(3)			/**< 1/32 reduction (output format : YCC422).	*/

// Reduction mode.
#define D_IM_RDCMD_1_2						(0)			/**< 1/2 reduction.							*/
#define D_IM_RDCMD_1_4						(1)			/**< 1/4 reduction.							*/
#define D_IM_RDCMD_1_8						(2)			/**< 1/8 reduction.							*/

// CSS mode.
#define D_IM_SHDR_CSSMD_YCC422				(0)			/**< YCC422.								*/
#define D_IM_SHDR_CSSMD_YCC420_ODD			(1)			/**< YCC420 odd.							*/
#define D_IM_SHDR_CSSMD_YCC420_CENTER		(2)			/**< YCC420 center.							*/

// Average MC mode.
#define D_IM_MCAMD_MAX						(0)			/**< Determining the maximum value of the error.									*/
#define D_IM_MCAMD_NR_EFFECT				(1)			/**< In the low confidence, output MC result of priority NR effect(Recommendation).	*/
#define D_IM_MCAMD_MAX_VECTOR				(2)			/**< Determining the maximum value of the error(overlapping vector).				*/
#define D_IM_MCAMD_MAX_MIN_VECTOR			(3)			/**< Determining the maximum - minimum value of the error(overlapping vector).		*/

// Low confidence MC mode.
#define D_IM_MCRMD_STANDARD					(0)			/**< The standard image output.				*/
#define D_IM_MCRMD_AVERAGE					(1)			/**< Average MC output(4 vector).			*/
#define D_IM_MCRMD_MC						(2)			/**< MC implementation.						*/

// Transfer burst length.
#define D_IM_SHDR_BURSTLEN_512				(0)			/**< 512byte.								*/
#define D_IM_SHDR_BURSTLEN_256				(1)			/**< 256byte.								*/

// Write with mask.
#define D_IM_SHDR_WWMASK_ON					(0)			/**< ON.									*/
#define D_IM_SHDR_WWMASK_OFF				(1)			/**< OFF.									*/

// Degamma table read selector.
#define D_IM_SHDR_DGTRSEL_PREFL				(2)			/**< PREF long exp 1/32 reduction image.			*/
#define D_IM_SHDR_DGTRSEL_PREFS				(3)			/**< PREF short exp 1/32 reduction image.			*/
#define D_IM_SHDR_DGTRSEL_PREMBL_EVEN		(4)			/**< PREMB long exp full size image (even pixel).	*/
#define D_IM_SHDR_DGTRSEL_PREMBL_ODD		(5)			/**< PREMB long exp full size image (odd pixel).	*/
#define D_IM_SHDR_DGTRSEL_PREMBS_EVEN		(6)			/**< PREMB short exp full size image (even pixel).	*/
#define D_IM_SHDR_DGTRSEL_PREMBS_ODD		(7)			/**< PREMB short exp full size image (odd pixel).	*/

// Compression/Decompression mode.
#define D_IM_SHDR_COMP_MODE_LOSSY			(0)			/**< Lossy mode.							*/
#define D_IM_SHDR_COMP_MODE_LOSSLESS		(1)			/**< Lossless mode.							*/

// MC mode.
#define D_IM_SHDR_COMP_MC_UNUSED			(0)			/**< MC mode unused.						*/
#define D_IM_SHDR_COMP_MC_USED				(1)			/**< MC mode used.							*/

// CbCr format at COMP side.
#define D_IM_SHDR_COMP_INCFMT_YUV420		(0)			/**< YUV420.								*/
#define D_IM_SHDR_COMP_INCFMT_YUV422		(1)			/**< YUV422.								*/

// Axi size unit.
#define D_IM_SHDR_COMP_AXI_512				(0)			/**< 512 byte.								*/
#define D_IM_SHDR_COMP_AXI_256				(1)			/**< 256 byte.								*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special.

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** User callback function pointer(Frame complete) */
typedef VOID (*T_IM_SHDR_CALLBACK)( ULONG interrupt_flag_mask );

/** User callback function pointer(Line complete) */
typedef VOID (*T_IM_SHDR_LINEINT_CALLBACK)( ULONG interrupt_flag_mask );

/** SHDR image setting parameter */
typedef struct {
	UCHAR							in_bit_depth;		/**< SHDRIFRM:Input image bit depth<br>
															<ul>
																<li>@ref D_IM_SHDR_INBITDEPTH_8
																<li>@ref D_IM_SHDR_INBITDEPTH_16
															</ul>
														*/
	UCHAR							in_c_fmt;			/**< SHDRIFRM:Input CbCr image format<br>
															 If you use 16bit image, you must set D_IM_SHDR_INCFMT_422SP.<br>
															<ul>
																<li>@ref D_IM_SHDR_INCFMT_420SP
																<li>@ref D_IM_SHDR_INCFMT_422SP
															</ul>
														*/
	UCHAR							in_edge_cpy;		/**< SHDRIFRM:Input image edge copy<br>
															 If you select D_IM_SHDR_SHDRIN_SMCOUT_REF on BRSEL(SHDR input select),<br>
															 you must set D_IM_SHDR_OFF.<br>
															<ul>
																<li>@ref D_IM_SHDR_OFF
																<li>@ref D_IM_SHDR_ON
															</ul>
														*/
	UCHAR							in_edge_cut;		/**< SHDRIFRM:Input image edge cut<br>
															 If you use a 1/32 reduct image, you must set D_IM_SHDR_OFF.<br>
															 Other than Bypass mode, you must set a value same as in_edge_cpy.<br>
															<ul>
																<li>@ref D_IM_SHDR_OFF
																<li>@ref D_IM_SHDR_ON
															</ul>
														*/
	UCHAR							out_ctrl;			/**< SHDROFRM:Output image control<br>
															<ul>
																<li>@ref D_IM_SHDR_OUTCTRL_YCC
																<li>@ref D_IM_SHDR_OUTCTRL_RGB
															</ul>
														*/
	UCHAR							out_sel;			/**< SHDROFRM:Output image select<br>
															 You must set a value same as SHDROFRM(Output image control).<br>
															<ul>
																<li>@ref D_IM_SHDR_OUTSEL_YCC_16
																<li>@ref D_IM_SHDR_OUTSEL_RGB_16
															</ul>
														*/
	UCHAR							out_sel_en;			/**< SHDROFRM:Output image select enable<br>
															<ul>
																<li>@ref D_IM_SHDR_OFF
																<li>@ref D_IM_SHDR_ON
															</ul>
														*/
} T_IM_SHDR_CTRL_IMAGE_SET;

/** SHDR image mode parameter.
*/
typedef struct {
	USHORT							exp_mag;			/**< EXMAG :Exposure magnification (14bits, Unit 1/256, 1~16383).	*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	UCHAR							fil_margin;			/**< MRGN  :Filter margin<br>
															If you use a 1/32 reduct image, you must set D_IM_SHDR_FLITER_MARGIN_96.<br>
															<ul>
																<li>@ref D_IM_SHDR_FLITER_MARGIN_16
																<li>@ref D_IM_SHDR_FLITER_MARGIN_32
																<li>@ref D_IM_SHDR_FLITER_MARGIN_96
															</ul>
														*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	UCHAR							fil_margin;			/**< MRGN  :Filter margin<br>
															<ul>
																<li>@ref D_IM_SHDR_FLITER_MARGIN_16
																<li>@ref D_IM_SHDR_FLITER_MARGIN_32
															</ul>
														*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	INT8							rexp_mag_shift;		/**< REMVAL.REMSFT:Reciprocal of the exposure magnification shift (4bits, -8~5).					*/
	ULONG							rexp_mag;			/**< REMVAL.REXMAG:Reciprocal of the exposure magnification(significant digits) (17bits 0~65536).	*/
	SHORT							shift_x;			/**< SHIFT.SHIFTX :Alignment shift X (13bits, Unit 2pixel, -4096~4094).<br>
															 If you select D_IM_SHDR_OFF on in_edge_cpy, you must set 0.
														*/
	SHORT							shift_y;			/**< SHIFT.SHIFTY :Alignment shift Y (13bits, Unit 2pixel, -4096~4094).<br>
															 If you select D_IM_SHDR_OFF on in_edge_cpy, you must set 0.
														*/
	UCHAR							fmd_en;				/**< FLWCNT.FMDEN :FMC enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							bypass_en;			/**< FLWCNT.BPEN  :Bypass enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							smc_in;				/**< FLWCNT.SMCIN :SMC input select<br>
															<ul>
																<li>@ref D_IM_SHDR_SMCIN_STN_REF
																<li>@ref D_IM_SHDR_SMCIN_EXT_STN
																<li>@ref D_IM_SHDR_SMCIN_EXT_REF
															</ul>
														*/
	UCHAR							shdr_in;			/**< FLWCNT.BRSEL :SHDR input select<br>
															<ul>
																<li>@ref D_IM_SHDR_SHDRIN_STN_REF
																<li>@ref D_IM_SHDR_SHDRIN_STN_SMCOUT
																<li>@ref D_IM_SHDR_SHDRIN_SMCOUT_REF
															</ul>
														*/
	UCHAR							tile_size;			/**< TMODE:Tile size select<br>
															<ul>
																<li>@ref D_IM_SHDR_TILESIZE_1024x1024
																<li>@ref D_IM_SHDR_TILESIZE_1024x512
															</ul>
														*/
} T_IM_SHDR_CTRL_IMAGE_MODE;

/** SHDR SDRAM parameter (SDRR_ADDR) */
typedef struct {
	ULONG							a_y;				/**< A:Reference image Y (32bits, unit byte).			*/
	ULONG							a_c;				/**< A:Reference image C (32bits, unit byte).			*/
	ULONG							b_y;				/**< B:SMC merge image Y (32bits, unit byte).			*/
	ULONG							b_c;				/**< B:SMC merge image C (32bits, unit byte).			*/
	ULONG							c_y;				/**< C:Long exp reduct image Y (32bits, unit byte).		*/
	ULONG							c_c;				/**< C:Long exp reduct image C (32bits, unit byte).		*/
	ULONG							d_y;				/**< D:Short exp reduct image Y (32bits, unit byte).	*/
	ULONG							d_c;				/**< D:Short exp reduct image C (32bits, unit byte).	*/
	ULONG							e_y;				/**< E:Reserve.											*/
	ULONG							e_c;				/**< E:Reserve.											*/
	ULONG							f;					/**< F:Reserve.											*/
} T_IM_SHDR_CTRL_SDRAM_ADDR_R;

/** SHDR SDRAM parameter (SDRR_GHSIZE) */
typedef struct {
	USHORT							a_y;				/**< A:Reference image Y (16bits unit 8byte, 8~FFF8h).			*/
	USHORT							a_c;				/**< A:Reference image C (16bits unit 8byte, 8~FFF8h).			*/
	USHORT							b_y;				/**< B:SMC merge image Y (16bits unit 8byte, 8~FFF8h).			*/
	USHORT							b_c;				/**< B:SMC merge image C (16bits unit 8byte, 8~FFF8h).			*/
	USHORT							c_y;				/**< C:Long exp reduct image Y (16bits unit 8byte, 8~FFF8h).	*/
	USHORT							c_c;				/**< C:Long exp reduct image C (16bits unit 8byte, 8~FFF8h).	*/
	USHORT							d_y;				/**< D:Short exp reduct image Y (16bits unit 8byte, 8~FFF8h).	*/
	USHORT							d_c;				/**< D:Short exp reduct image C (16bits unit 8byte, 8~FFF8h).	*/
	USHORT							e_y;				/**< E:Reserve.													*/
	USHORT							e_c;				/**< E:Reserve.													*/
	USHORT							f;					/**< F:Reserve.													*/
} T_IM_SHDR_CTRL_SDRAM_GHSIZE_R;

/** SHDR SDRAM parameter (SDRR_HSIZE) */
typedef struct {
	USHORT							a;					/**< A:Rference image (14bits unit 2pixel, Full image:128~11992, Reduct image:4~376).			*/
	USHORT							b;					/**< B:SMC merge image (14bits unit 2pixel, Full image:128~11992, Reduct image:4~376).			*/
	USHORT							c;					/**< C:Long exp reduct image (14bits unit 2pixel, Full image:128~11992, Reduct image:4~376).	*/
	USHORT							d;					/**< D:Short exp reduct image (14bits unit 2pixel, Full image:128~11992, Reduct image:4~376).	*/
	USHORT							e;					/**< E:same A.																					*/
} T_IM_SHDR_CTRL_SDRAM_HSIZE_R;

/** SHDR SDRAM parameter (SDRR_VSIZE) */
typedef struct {
	USHORT							a;					/**< A:Reference image (14bits unit 2pixel, Full image:72~8992, Reduct image:4~282).			*/
	USHORT							b;					/**< B:SMC merge image (14bits unit 2pixel, Full image:72~8992, Reduct image:4~282).			*/
	USHORT							c;					/**< C:Long exp reduct image (14bits unit 2pixel, Full image:72~8992, Reduct image:4~282).		*/
	USHORT							d;					/**< D:Short exp reduct image (14bits unit 2pixel, Full image:72~8992, Reduct image:4~282).		*/
	USHORT							e;					/**< E:same A.																					*/
} T_IM_SHDR_CTRL_SDRAM_VSIZE_R;

/** SHDR SDRAM parameter (SDRR_VFM) */
typedef struct {
	UCHAR							a;					/**< A:Reference image<br>
															<ul>
																<li>@ref D_IM_SHDR_VFMT_RASTER
																<li>@ref D_IM_SHDR_VFMT_VIDEO
															</ul>
														*/
} T_IM_SHDR_CTRL_SDRAM_VFM_R;

/** SHDR SDRAM parameter (SDWR_ADDR) */
typedef struct {
	ULONG							b_y;				/**< B:1/32 reduction image Y (32bits, unit 8byte).			*/
	ULONG							b_c;				/**< B:1/32 reduction image C (32bits, unit 8byte).			*/
	ULONG							e_y;				/**< E:Resize image #5 Y (32bits, unit 8byte).				*/
	ULONG							e_c;				/**< E:Resize image #5 C (32bits, unit 8byte).				*/
	ULONG							f_y;				/**< F:Resize image #6 Y (32bits, unit 8byte).				*/
	ULONG							f_c;				/**< F:Resize image #6 C (32bits, unit 8byte).				*/
	ULONG							g_r_y;				/**< G:SHDR output image R/Y (32bits, unit 8byte).			*/
	ULONG							g_g_c;				/**< G:SHDR output image G/C (32bits, unit 8byte).			*/
	ULONG							g_b;				/**< G:SHDR output image B (32bits, unit 8byte).			*/
	ULONG							h_x;				/**< H:Output motion vector image X (32bits, unit 8byte).	*/
	ULONG							h_y;				/**< H:Output motion vector image Y (32bits, unit 8byte).	*/
	ULONG							i;					/**< I:Reserve.												*/
} T_IM_SHDR_CTRL_SDRAM_ADDR_W;

/** SHDR SDRAM parameter (SDWR_GHSIZE) */
typedef struct {
	USHORT							b_y;				/**< B:1/32 reduction image Y (16bits, unit 8byte).			*/
	USHORT							b_c;				/**< B:1/32 reduction image C (16bits, unit 8byte).			*/
	USHORT							e_y;				/**< E:Resize image #5 Y (16bits, unit 8byte).				*/
	USHORT							e_c;				/**< E:Resize image #5 C (16bits, unit 8byte).				*/
	USHORT							f_y;				/**< F:Resize image #6 Y (16bits, unit 8byte).				*/
	USHORT							f_c;				/**< F:Resize image #6 C (16bits, unit 8byte).				*/
	USHORT							g_r_y;				/**< G:SHDR output image R/Y (16bits, unit 8byte).			*/
	USHORT							g_g_c;				/**< G:SHDR output image G/C (16bits, unit 8byte).			*/
	USHORT							g_b;				/**< G:SHDR output image B (16bits, unit 8byte).			*/
	USHORT							h_x;				/**< H:Output motion vector image X (16bits, unit 8byte).	*/
	USHORT							h_y;				/**< H:Output motion vector image Y (16bits, unit 8byte).	*/
	USHORT							i;					/**< I:Reserve.												*/
} T_IM_SHDR_CTRL_SDRAM_GHSIZE_W;

/** SHDR SDRAM parameter (SDWR_VFM) */
typedef struct {
	UCHAR							a;					/**< A:RBK0 output image<br>
															<ul>
																<li>@ref D_IM_SHDR_VFMT_COMPIP_VIDEO
																<li>@ref D_IM_SHDR_VFMT_COMPIP_RASTER
															</ul>
														*/
	UCHAR							b;					/**< B:RBK1 output image(1/32 reduction image).<br>
															<ul>
																<li>@ref D_IM_SHDR_VFMT_RASTER
																<li>@ref D_IM_SHDR_VFMT_VIDEO
															</ul>
														*/
} T_IM_SHDR_CTRL_SDRAM_VFM_W;

/** SHDR SDRAM parameter (SDWR_BA) */
typedef struct {
	ULONG							a_y;				/**< A:RBK0 output image Y (32bits, unit 16Kbyte).		*/
	ULONG							a_c;				/**< A:RBK0 output image C (32bits, unit 16Kbyte).		*/
	ULONG							c_y;				/**< C:RBK2 output image Y (32bits, unit 16Kbyte).		*/
	ULONG							c_c;				/**< C:RBK2 output image C (32bits, unit 16Kbyte).		*/
	ULONG							d_y;				/**< D:RBK3 output image Y (32bits, unit 16Kbyte).		*/
	ULONG							d_c;				/**< D:RBK3 output image C (32bits, unit 16Kbyte).		*/
} T_IM_SHDR_CTRL_SDRAM_BADDR_W;

/** SHDR SDRAM parameter (SDWR_FW) */
typedef struct {
	USHORT							a;					/**< A:RBK0 output image (16bits, unit 256byte).		*/
	USHORT							c;					/**< C:RBK2 output image (16bits, unit 256byte).		*/
	USHORT							d;					/**< D:RBK3 output image (16bits, unit 256byte).		*/
} T_IM_SHDR_CTRL_SDRAM_HSIZE_W;

/** SHDR SDRAM parameter */
typedef struct {
	T_IM_SHDR_CTRL_SDRAM_ADDR_R		r_addr;				/**< SDRR_ADDR  :Read address.<br>
															[Ycc image]16bit image : Multiple of 4. 8bit image : Multiple of 2.<br>
															[Vector image]Multiple of 2.<br>
														*/
	T_IM_SHDR_CTRL_SDRAM_GHSIZE_R	r_ghsize;			/**< SDRR_GHSIZE:Read global horizontal size.	*/
	T_IM_SHDR_CTRL_SDRAM_HSIZE_R	r_hsize;			/**< SDRR_HSIZE :Read horizontal size.			*/
	T_IM_SHDR_CTRL_SDRAM_VSIZE_R	r_vsize;			/**< SDRR_VSIZE :Read vertical size.			*/
	T_IM_SHDR_CTRL_SDRAM_VFM_R		r_vfm;				/**< SDRR_VFM   :Read video format.				*/
	T_IM_SHDR_CTRL_SDRAM_ADDR_W		w_addr;				/**< SDRW_ADDR  :Write address.					*/
	T_IM_SHDR_CTRL_SDRAM_GHSIZE_W	w_ghsize;			/**< SDRW_GHSIZE:Write global horizontal size.	*/
	T_IM_SHDR_CTRL_SDRAM_VFM_W		w_vfm;				/**< SDRW_VFM   :Write video format.			*/
	T_IM_SHDR_CTRL_SDRAM_BADDR_W	w_baddr;			/**< SDRW_BA    :Write base address.			*/
	T_IM_SHDR_CTRL_SDRAM_HSIZE_W	w_hsize;			/**< SDRW_FW    :Write horizontal size.			*/
} T_IM_SHDR_CTRL_SDRAM;

/** SHDR COMPIP parameter */
typedef struct {
	UCHAR							mode;				/**< Compression/Decompression mode.
															<ul>
																<li>@ref D_IM_SHDR_COMP_MODE_LOSSY
																<li>@ref D_IM_SHDR_COMP_MODE_LOSSLESS
															</ul>
														*/
	UCHAR							mc_mode;			/**< MC mode.
															<ul>
																<li>@ref D_IM_SHDR_COMP_MC_UNUSED
																<li>@ref D_IM_SHDR_COMP_MC_USED
															</ul>
														*/
	UCHAR							comp_incfmt;		/**< CbCr format at COMP side.
															If you use r/w_bypass_mode = D_IM_SHDR_DISABLE,<br>
															select D_IM_SHDR_COMP_INCFMT_YUV420.
															<ul>
																<li>@ref D_IM_SHDR_COMP_INCFMT_YUV420
																<li>@ref D_IM_SHDR_COMP_INCFMT_YUV422
															</ul>
														*/
	UCHAR							r_bypass_mode;		/**< Read bypass mode.
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							w_bypass_mode;		/**< Write bypass mode.
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							r_img_axi_size;		/**< axi size unit of image data transfer(read).
															<ul>
																<li>@ref D_IM_SHDR_COMP_AXI_512
																<li>@ref D_IM_SHDR_COMP_AXI_256
															</ul>
														*/
	UCHAR							r_comp_axi_size;	/**< axi size unit of compression information(read).
															<ul>
																<li>@ref D_IM_SHDR_COMP_AXI_512
																<li>@ref D_IM_SHDR_COMP_AXI_256
															</ul>
														*/
	UCHAR							w_img_axi_size;		/**< axi size unit of image data transfer(write).
															 axi size unit of compression information(write) is 256byte fixation.
															<ul>
																<li>@ref D_IM_SHDR_COMP_AXI_512
																<li>@ref D_IM_SHDR_COMP_AXI_256
															</ul>
														*/
	USHORT							lint_no;			/**< Tile vertical number that an line interrupt generates. */
	UCHAR							lint_mask;			/**< Line interrupt mask setting.
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							lint_sts;			/**< Line interrupt status.
															-- Case of read --
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul><br>
															-- Case of write --
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE (Line interrupt clear)
															</ul>
														*/
	ULONG							w_addr_y;			/**< Write address(Y) of compression information.<br>
															 You do not use it with the BYPASS mode.
														*/
	ULONG							w_addr_c;			/**< Write address(C) of compression information.<br>
															 You do not use it with the BYPASS mode.
														*/
	ULONG							r_addr_y;			/**< Read address(Y) of compression information.<br>
															 You do not use it with the BYPASS mode.
														*/
	ULONG							r_addr_c;			/**< Read address(C) of compression information.<br>
															 You do not use it with the BYPASS mode.
														*/
} T_IM_SHDR_CTRL_COMPIP;

/** SHDR control parameter */
typedef struct {
	T_IM_SHDR_CTRL_IMAGE_SET		image_set;			/**< Image setting parameter.				*/
	T_IM_SHDR_CTRL_IMAGE_MODE		image_mode;			/**< Image mode parameter.					*/
	T_IM_SHDR_CTRL_SDRAM			sdram;				/**< SDRAM parameter.						*/
	T_IM_SHDR_CTRL_COMPIP			compip;				/**< COMPIP parameter.						*/
	T_IM_SHDR_CALLBACK				pCallBack;			/**< User callback function(Frame comp).	*/
	T_IM_SHDR_LINEINT_CALLBACK		pLineInt_CallBack;	/**< User callback function(Line int).		*/
} T_IM_SHDR_CTRL;

/** SHDR PRE parameter (PREMD) */
typedef struct {
	UCHAR							cus_mode;			/**< Chroma up sampling mode<br>
															<ul>
																<li>@ref D_IM_SHDR_CUS_YCC422_BILINEAR_A
																<li>@ref D_IM_SHDR_CUS_YCC422_BILINEAR_B
																<li>@ref D_IM_SHDR_CUS_YCC422_PADDING
																<li>@ref D_IM_SHDR_CUS_YCC420_BILINEAR_A
																<li>@ref D_IM_SHDR_CUS_YCC420_BILINEAR_B
																<li>@ref D_IM_SHDR_CUS_YCC420_PADDING
															</ul>
														*/
	UCHAR							degamma_mode;		/**< Degamma mode<br>
															<ul>
																<li>@ref D_IM_SHDR_DEGAMMA_LUT
																<li>@ref D_IM_SHDR_DEGAMMA_BITEXP
															</ul>
														*/

} T_IM_SHDR_PRE_PREMD;

/** SHDR PRE parameter (PREY2RCLP) */
typedef struct {
	USHORT							upper_long0[ 3 ];	/**< Upper value R/G/B (long exposure - full image) (16bit).	*/
	USHORT							lower_long0[ 3 ];	/**< Lower value R/G/B (long exposure - full image) (16bit).	*/
	UCHAR							upper_short[ 3 ];	/**< Upper value R/G/B (shrt exposure) (8bit).					*/
	UCHAR							lower_short[ 3 ];	/**< Upper value R/G/B (shrt exposure) (8bit).					*/
} T_IM_SHDR_PRE_PREY2RCLP;

/** SHDR PRE parameter (PRER2YCLP) */
typedef struct {
	USHORT							upper_long_y;		/**< Upper value Y (long exposure) (16bit).			*/
	SHORT							upper_long_c[ 2 ];	/**< Upper value Cb/Cr (long exposure) (16bit).		*/
	USHORT							lower_long_y;		/**< Lower value Y (long exposure) (16bit).			*/
	SHORT							lower_long_c[ 2 ];	/**< Lower value Cb/Cr (long exposure) (16bit).		*/
	USHORT							upper_short_y;		/**< Upper value Y (short exposure) (12bit).		*/
	SHORT							upper_short_c[ 2 ];	/**< Upper value Cb/Cr (short exposure) (12bit).	*/
	USHORT							lower_short_y;		/**< Lower value Y (short exposure) (12bit).		*/
	SHORT							lower_short_c[ 2 ];	/**< Lower value Cb/Cr (short exposure) (12bit).	*/
} T_IM_SHDR_PRE_PRER2YCLP;

/** SHDR PRE parameter (SKN/LKN) */
typedef struct {
	UCHAR							offset[ 3 ];		/**< Offset (8bits).								*/
	UCHAR							slope[ 3 ];			/**< Slope (8bits).									*/
	USHORT							boundary[ 2 ];		/**< Boundary:boundary[0] =< boundary[1] (10bits).	*/
} T_IM_SHDR_PRE_KN;

/** SHDR PRE parameter */
typedef struct {
	T_IM_SHDR_PRE_PREMD				pre_mode;			/**< PREMD    :Chroma Up Sampling/Degamma mode.							*/
	SHORT							y2r_coef[ 9 ];		/**< Y2RCOEF  :Y2R color space conversion coefficient.					*/
	T_IM_SHDR_PRE_PREY2RCLP			y2r_clp;			/**< PREY2RCLP:Y2R conversion clip.										*/
	SHORT							r2y_coef[ 9 ];		/**< R2YCOEF  :R2Y color space conversion coefficient.					*/
	T_IM_SHDR_PRE_PRER2YCLP			r2y_clp;			/**< PRER2YCLP:R2Y conversion clip.										*/
	T_IM_SHDR_PRE_KN				short_knee;			/**< SKN      :Knee setting(Short exposure image).						*/
	T_IM_SHDR_PRE_KN				long_knee;			/**< LKN      :Knee setting(Long exposure image).						*/
} T_IM_SHDR_PRE;

/** SHDR FMD parameter (MVCNT) */
typedef struct {
	UCHAR							bounds;				/**< Bounds (8bit).					*/
	UCHAR							threshold;			/**< Threshold (8bit).				*/
	SHORT							gr;					/**< gr (13bit)						*/
} T_IM_SHDR_FMD_MVCNT;

/** SHDR FMD parameter (WGHTR2YCLP) */
typedef struct {
	UCHAR							upper;				/**< Upper value (8bit).			*/
	UCHAR							lower;				/**< Lower value (8bit).			*/
} T_IM_SHDR_FMD_WGHTR2YCLP;

/** SHDR FMD parameter (PRETH) */
typedef struct {
	UCHAR							md;					/**< Reduced image comparison judgment threshold (8bit).	*/
	UCHAR							y;					/**< Brightness reference threshold (8bit).					*/
} T_IM_SHDR_FMD_PRETH;

/** SHDR FMD parameter (SSMRGCNT) */
typedef struct {
	UCHAR							mode;				/**< Reduction image merge mode<br>
															<ul>
																<li>@ref D_IM_SHDR_OFF
																<li>@ref D_IM_SHDR_ON (use prohibition)
															</ul>
														*/
	UCHAR							threshold;			/**< Reduction image threshold (8bit).	*/
	UCHAR							value;				/**< Reduction image value (8bit).		*/
} T_IM_SHDR_FMD_SSMRGCNT;

/** SHDR FMD parameter */
typedef struct {
	UCHAR							ep_boundary[ 4 ];			/**< EPBD    :Epsilon boundary (8bit) [0]:D.C.				*/
	UCHAR							ep_offset[ 4 ];				/**< EPOF    :Epsilon offset (8bit).						*/
	UCHAR							ep_blend;					/**< EPBLD   :Epsilon blend ratio (3bit).					*/
	UCHAR							fd_weight;					/**< FDWEIGHT:Frame difference weight (5bit).				*/
	T_IM_SHDR_FMD_MVCNT				mvcnt;						/**< MVCNT   :.												*/
	UCHAR							bldr_offset[ 5 ];			/**< BLDROFS :Blend ratio offset (8bit).					*/
	USHORT							bldr_slope[ 5 ];			/**< BLDRSL  :Blend ratio slope (12bit).					*/
	UCHAR							bldr_boundary[ 5 ];			/**< BLDRB   :Blend ratio boundary (8bit) [0]:D.C.			*/
	SHORT							weight_r2y_coef[ 3 ];		/**< WGHTR2YCOEF:Weight R2Y coefficient (15bit).			*/
	T_IM_SHDR_FMD_WGHTR2YCLP		weight_r2y_clp;				/**< WGHTR2YCLP :Weight R2Y clip.							*/
	UCHAR							weight_base;				/**< WGHTBASE   :Weight base<br>
																	<ul>
																		<li>@ref D_IM_SHDR_WGHTBASE_Y
																		<li>@ref D_IM_SHDR_WGHTBASE_RGB
																	</ul>
																*/
	USHORT							weight_offset[ 3 ][ 5 ];	/**< WGHT R/G/B OFS:Weight offset RGB (12bit).				*/
	USHORT							weight_slope[ 3 ][ 5 ];		/**< WGHT R/G/B SL :Weight slope RGB (12bit).				*/
	UCHAR							weight_boundary[ 3 ][ 5 ];	/**< WGHT R/G/B B  :Weight boundary RGB (8bit) [0]:D.C.		*/
	USHORT							iso_threshold;				/**< ISOTH   :Isolated point threshold (9bit).				*/
	UCHAR							iso_coef[ 6 ];				/**< ISOCOEF :Isolated point coefficient (2/2/2/5/5/8 bit).	*/
	UCHAR							sat_threshold;				/**< SATTH   :Saturation threshold (8bit).					*/
	UCHAR							exp77_mode;					/**< EXP77MD :Expansion process mode<br>
																	<ul>
																		<li>@ref D_IM_SHDR_EXP77MD_COEFSUMDIV
																		<li>@ref D_IM_SHDR_EXP77MD_PERIMAXVAL
																	</ul>
																*/
	UCHAR							fmd_sel;					/**< FMDSEL  :FMD output image select<br>
																	<ul>
																		<li>@ref D_IM_SHDR_FMDSEL_EXPANSION
																		<li>@ref D_IM_SHDR_FMDSEL_REDUCTION
																	</ul>
																*/
} T_IM_SHDR_FMD;

/** SHDR PMSK parameter */
typedef struct {
	UCHAR							tlut[ 17 ];			/**< TLUT  :Reliability map conversion LUT (8bit).	*/
	UCHAR							pmsk_mode;			/**< PMSKMD:Pre mask mode<br>
															<ul>
																<li>@ref D_IM_SHDR_PMSKMD_MAX
																<li>@ref D_IM_SHDR_PMSKMD_AVE
															</ul>
														*/
} T_IM_SHDR_PMSK;

/** SHDR SPNR parameter */
typedef struct {
	UCHAR							spnr_en;			/**< SPNREN:SPNR process enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							spe_blend;			/**< SPEBLD:SPE blend ratio (3bit).			*/
	USHORT							spe_boundary[ 4 ];	/**< SPEBD :SPE boundary (12bit) [0]:D.C.	*/
	USHORT							spe_offset[ 4 ];	/**< SPEOF :SPE offset (12bit).				*/
} T_IM_SHDR_SPNR;

/** SHDR MSK parameter */
typedef struct {
	USHORT							y2r_clp_upper_long[ 3 ];	/**< MSKY2RCLPUL:Y2R conversion clip upper R/G/B (long exp) (12bit).	*/
	USHORT							y2r_clp_lower_long[ 3 ];	/**< MSKY2RCLPLL:Y2R conversion clip lower R/G/B (long exp) (12bit).	*/
	USHORT							y2r_clp_upper_short[ 3 ];	/**< MSKY2RCLPUS:Y2R conversion clip upper R/G/B (short exp) (12bit).	*/
	USHORT							y2r_clp_lower_short[ 3 ];	/**< MSKY2RCLPLS:Y2R conversion clip lower R/G/B (short exp) (12bit).	*/
	UCHAR							alpha;						/**< ALPHA      :Average processing weight (6bit) 63=100% short exp.	*/
	UCHAR							lpfc[ 3 ][ 3 ];				/**< LPFC       :Low-pass filter coefficient (8bit).					*/
	UCHAR							blend_offset[ 7 ];			/**< BLDOFS     :Blend offset (8bit).									*/
	USHORT							blend_slope[ 7 ];			/**< BLDSL      :Blend slope (16bit).									*/
	USHORT							blend_boundary[ 7 ];		/**< BLDB       :Blend boundary (12bit) [0]:D.C.<br>
																				 blend_boundary[1] =< [2] =< [3] =< [4] =< [5] =< [6]<br>
																*/
	UCHAR							norm_gain[ 3 ];				/**< NORMGAIN   :Normalization gain R/G/B (8bit).						*/
	UCHAR							norm_offset[ 3 ];			/**< NORMOFS    :Normalization offset R/G/B (8bit).						*/
	UCHAR							msk_mode;					/**< MSKMD      :Mask merge mode<br>
																	<ul>
																		<li>@ref D_IM_SHDR_MSKMD_SHORT
																		<li>@ref D_IM_SHDR_MSKMD_LONG
																		<li>@ref D_IM_SHDR_MSKMD_MSKONLY
																	</ul>
																*/
} T_IM_SHDR_MSK;

/** SHDR BLD parameter */
typedef struct {
	UCHAR							blend_sel;			/**< BLDSEL  :Blend select<br>
															<ul>
																<li>@ref D_IM_SHDR_BLDSEL_SHORT_LONG
																<li>@ref D_IM_SHDR_BLDSEL_LONG_SHORT
															</ul>
														*/
	UCHAR							alpha_sel;			/**< ALPHASEL:Alpha value select<br>
															<ul>
																<li>@ref D_IM_SHDR_ALPHASEL_MERGEMASK
																<li>@ref D_IM_SHDR_ALPHASEL_FIXED
															</ul>
														*/
	UCHAR							alpha_val[ 3 ];		/**< ALPHAVAL:Alpha value Y/U/V (8bit).				*/
} T_IM_SHDR_BLD;

/** SHDR POST parameter (PSOTSFT1RND) */
typedef struct {
	UCHAR							sft1rnd_en;			/**< POSTSFT1RNDEN:Random number addition enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
} T_IM_SHDR_POST_PSOTSFT1RND;

/** SHDR POST parameter */
typedef struct {
	T_IM_SHDR_POST_PSOTSFT1RND		sft1rnd;				/**< POSTSFT1RND  :Random number addition setting.								*/
	SHORT							y2r_coef[ 9 ];			/**< POSTY2RCOEF  :Y2R color space conversion coefficient (15bit).				*/
	USHORT							y2r_clp_th_upper[ 3 ];	/**< POSTY2RCLPTHU:Y2R clip upper value R/G/B (16bit).							*/
	USHORT							y2r_clp_th_lower[ 3 ];	/**< POSTY2RCLPTHL:Y2R clip lower value R/G/B (16bit).							*/
	UCHAR							ev_sel;					/**< POSTSEL1     :EV compensation input data<br>
																	<ul>
																		<li>@ref D_IM_SHDR_POSTSEL1_Y
																		<li>@ref D_IM_SHDR_POSTSEL1_RGB
																	</ul>
															*/
	UCHAR							mulqpos;				/**< POSTMULQPOS  :EV compensation multiplication decimal point position<br>
																	<ul>
																		<li>@ref D_IM_SHDR_POSTMULQPOS_Q5
																		<li>@ref D_IM_SHDR_POSTMULQPOS_Q4
																		<li>@ref D_IM_SHDR_POSTMULQPOS_Q3
																		<li>@ref D_IM_SHDR_POSTMULQPOS_Q2
																	</ul>
															*/
	USHORT							ev_kn_boundary[ 10 ];	/**< EVKNB        :EV compensation Knee input data boundary (12bit) [0]:D.C.	*/
	USHORT							ev_kn_offset[ 10 ];		/**< EVKNOFS      :EV compensation Knee offset (14bit).							*/
	SHORT							ev_kn_slope[ 10 ];		/**< EVKNSL       :EV compensation Knee slope (15bit).							*/
	UCHAR							css_sel;				/**< POSTSEL0     :CSS input signal selection<br>
																	<ul>
																		<li>@ref D_IM_SHDR_POSTSEL0_BLEND_YCC16
																		<li>@ref D_IM_SHDR_POSTSEL0_BYPASS_YCC8
																	</ul>
															*/
	UCHAR							hdrcss_mode;			/**< HDRCSSMD     :CSS sampling mode<br>
																	<ul>
																		<li>@ref D_IM_SHDR_HDRCSSMD_YCC422_ODD
																		<li>@ref D_IM_SHDR_HDRCSSMD_YCC422_CENTER
																	</ul>
															*/
} T_IM_SHDR_POST;

/** SHDR resize parameter (Channel mode) */
typedef struct {
	UCHAR							a_w_en;				/**< A:Write enable<br>
															 If you select D_IM_SHDR_OUTCTRL_RGB on SHDROFRM(Output image control),<br>
															 you must set D_IM_SHDR_DISABLE.<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							a_drc_en;			/**< A:Data range enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							a_css_md;			/**< A:CSS mode<br>
															<ul>
																<li>@ref D_IM_SHDR_CSSMD_YCC422
																<li>@ref D_IM_SHDR_CSSMD_YCC420_ODD
																<li>@ref D_IM_SHDR_CSSMD_YCC420_CENTER
															</ul>
														*/
	UCHAR							b_w_en;				/**< B:Write enable<br>
															 If you select D_IM_SHDR_OUTCTRL_RGB on SHDROFRM(Output image control),<br>
															 you must set D_IM_SHDR_DISABLE.<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							b_rdc_md;			/**< B:Reduction mode 1<br>
															<ul>
																<li>@ref D_IM_RDCMD1_1_4
																<li>@ref D_IM_RDCMD1_1_8
																<li>@ref D_IM_RDCMD1_1_32
															</ul>
														*/
	UCHAR							b_css_md;			/**< B:CSS mode<br>
															<ul>
																<li>@ref D_IM_SHDR_CSSMD_YCC422
																<li>@ref D_IM_SHDR_CSSMD_YCC420_ODD
																<li>@ref D_IM_SHDR_CSSMD_YCC420_CENTER
															</ul>
														*/
	UCHAR							c_w_en;				/**< C:Write enable<br>
															 If you select D_IM_SHDR_OUTCTRL_RGB on SHDROFRM(Output image control),<br>
															 you must set D_IM_SHDR_DISABLE.<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							c_rsz_md;			/**< C:Resize mode<br>
															<ul>
																<li>@ref D_IM_RSZMD_ODD0
																<li>@ref D_IM_RSZMD_ODD1
																<li>@ref D_IM_RSZMD_CENTER0
																<li>@ref D_IM_RSZMD_CENTER1
															</ul>
														*/
	UCHAR							c_rdc_en;			/**< C:Reduction enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							c_rdc_md;			/**< C:Reduction mode<br>
															<ul>
																<li>@ref D_IM_RDCMD_1_2
																<li>@ref D_IM_RDCMD_1_4
																<li>@ref D_IM_RDCMD_1_8
															</ul>
														*/
	UCHAR							c_trm_en;			/**< C:Triming enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							c_drc_en;			/**< C:DRC enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							c_css_md;			/**< C:CSS mode<br>
															 If c_rdc_en = D_IM_SHDR_ENABLE, abd c_rdc_md = D_IM_RDCMD_1_8, set other than D_IM_SHDR_CSSMD_YCC420_CENTER<br>
															<ul>
																<li>@ref D_IM_SHDR_CSSMD_YCC422
																<li>@ref D_IM_SHDR_CSSMD_YCC420_ODD
																<li>@ref D_IM_SHDR_CSSMD_YCC420_CENTER
															</ul>
														*/
	UCHAR							d_w_en;				/**< D:Write enable<br>
															 If you select D_IM_SHDR_OUTCTRL_RGB on SHDROFRM(Output image control),<br>
															 you must set D_IM_SHDR_DISABLE.<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							d_rsz_md;			/**< D:Resize mode<br>
															<ul>
																<li>@ref D_IM_RSZMD_ODD0
																<li>@ref D_IM_RSZMD_ODD1
																<li>@ref D_IM_RSZMD_CENTER0
																<li>@ref D_IM_RSZMD_CENTER1
															</ul>
														*/
	UCHAR							d_rdc_en;			/**< D:Reduction enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							d_rdc_md;			/**< D:Reduction mode<br>
															<ul>
																<li>@ref D_IM_RDCMD_1_2
																<li>@ref D_IM_RDCMD_1_4
																<li>@ref D_IM_RDCMD_1_8
															</ul>
														*/
	UCHAR							d_trm_en;			/**< D:Triming enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							d_drc_en;			/**< D:DRC enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							d_css_md;			/**< D:CSS mode<br>
															 If d_rdc_en = D_IM_SHDR_ENABLE, abd d_rdc_md = D_IM_RDCMD_1_8, set other than D_IM_SHDR_CSSMD_YCC420_CENTER.
															<ul>
																<li>@ref D_IM_SHDR_CSSMD_YCC422
																<li>@ref D_IM_SHDR_CSSMD_YCC420_ODD
																<li>@ref D_IM_SHDR_CSSMD_YCC420_CENTER
															</ul>
														*/
	UCHAR							e_w_en;				/**< E:Write enable<br>
															 If you select D_IM_SHDR_OUTCTRL_RGB on SHDROFRM(Output image control),<br>
															 you must set D_IM_SHDR_DISABLE.<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							e_rsz_md;			/**< E:Resize mode<br>
															<ul>
																<li>@ref D_IM_RSZMD_ODD0
																<li>@ref D_IM_RSZMD_ODD1
																<li>@ref D_IM_RSZMD_CENTER0
																<li>@ref D_IM_RSZMD_CENTER1
															</ul>
														*/
	UCHAR							e_rdc_en;			/**< E:Reduction enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							e_rdc_md;			/**< E:Reduction mode<br>
															<ul>
																<li>@ref D_IM_RDCMD_1_2
																<li>@ref D_IM_RDCMD_1_4
																<li>@ref D_IM_RDCMD_1_8
															</ul>
														*/
	UCHAR							e_trm_en;			/**< E:Triming enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							e_css_md;			/**< E:CSS mode<br>
															 If e_rdc_en = D_IM_SHDR_ENABLE, abd e_rdc_md = D_IM_RDCMD_1_8, set other than D_IM_SHDR_CSSMD_YCC420_CENTER<br>
															<ul>
																<li>@ref D_IM_SHDR_CSSMD_YCC422
																<li>@ref D_IM_SHDR_CSSMD_YCC420_ODD
																<li>@ref D_IM_SHDR_CSSMD_YCC420_CENTER
															</ul>
														*/
	UCHAR							f_w_en;				/**< F:Write enable<br>
															 If you select D_IM_SHDR_OUTCTRL_RGB on SHDROFRM(Output image control),<br>
															 you must set D_IM_SHDR_DISABLE.<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							f_rsz_md;			/**< F:Resize mode<br>
															<ul>
																<li>@ref D_IM_RSZMD_ODD0
																<li>@ref D_IM_RSZMD_ODD1
																<li>@ref D_IM_RSZMD_CENTER0
																<li>@ref D_IM_RSZMD_CENTER1
															</ul>
														*/
	UCHAR							f_rdc_en;			/**< F:Reduction enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							f_rdc_md;			/**< F:Reduction mode<br>
															<ul>
																<li>@ref D_IM_RDCMD_1_2
																<li>@ref D_IM_RDCMD_1_4
																<li>@ref D_IM_RDCMD_1_8
															</ul>
														*/
	UCHAR							f_trm_en;			/**< F:Triming enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
	UCHAR							f_css_md;			/**< F:CSS mode<br>
															 If f_rdc_en = D_IM_SHDR_ENABLE, abd f_rdc_md = D_IM_RDCMD_1_8, set other than D_IM_SHDR_CSSMD_YCC420_CENTER<br>
															<ul>
																<li>@ref D_IM_SHDR_CSSMD_YCC422
																<li>@ref D_IM_SHDR_CSSMD_YCC420_ODD
																<li>@ref D_IM_SHDR_CSSMD_YCC420_CENTER
															</ul>
														*/
} T_IM_SHDR_RESIZE_CHMODE;

/** SHDR resize parameter (WOUTSZ) */
typedef struct {
	USHORT							a_hsize;			/**< A:Horizontal size (14bits unit 2pixel, 512~11880).	*/
	USHORT							a_vsize;			/**< A:Vertical size   (14bits unit 8pixel, 384~8800).	*/
	USHORT							b_hsize;			/**< B:Horizontal size (14bits unit 2pixel, 16~2970).	*/
	USHORT							b_vsize;			/**< B:Vertical size   (14bits unit 2pixel, 12~2200).	*/
	USHORT							c_hsize;			/**< C:Horizontal size (14bits unit 2pixel, 32~11880).	*/
	USHORT							c_vsize;			/**< C:Vertical size   (14bits unit 2pixel, 24~8800).	*/
	USHORT							d_hsize;			/**< D:Horizontal size (14bits unit 2pixel, 32~11880).	*/
	USHORT							d_vsize;			/**< D:Vertical size   (14bits unit 2pixel, 24~8800).	*/
	USHORT							e_hsize;			/**< E:Horizontal size (14bits unit 2pixel, 32~11880).	*/
	USHORT							e_vsize;			/**< E:Vertical size   (14bits unit 2pixel, 24~8800).	*/
	USHORT							f_hsize;			/**< F:Horizontal size (14bits unit 2pixel, 32~11880).	*/
	USHORT							f_vsize;			/**< F:Vertical size   (14bits unit 2pixel, 24~8800).	*/
} T_IM_SHDR_RESIZE_WOUTSZ;

/** SHDR resize parameter (RSZHCNT) */
typedef struct {
	USHORT							c_hsta;				/**< C:Horizontal start position (14bits, 0~16383).						*/
	USHORT							c_hpit;				/**< C:Horizontal pitch size     (15bits, 16384/16384 ~ 32767/16384).	*/
	USHORT							d_hsta;				/**< D:Horizontal start position (14bits, 0~16383).						*/
	USHORT							d_hpit;				/**< D:Horizontal pitch size     (15bits, 16384/16384 ~ 32767/16384).	*/
	USHORT							e_hsta;				/**< E:Horizontal start position (14bits, 0~16383).						*/
	USHORT							e_hpit;				/**< E:Horizontal pitch size     (15bits, 16384/16384 ~ 32767/16384).	*/
	USHORT							f_hsta;				/**< F:Horizontal start position (14bits, 0~16383).						*/
	USHORT							f_hpit;				/**< F:Horizontal pitch size     (15bits, 16384/16384 ~ 32767/16384).	*/
} T_IM_SHDR_RESIZE_RSZHCNT;

/** SHDR resize parameter (RSZVCNT) */
typedef struct {
	USHORT							c_vsta;				/**< C:Vertical start position (14bits 0~16383).						*/
	USHORT							c_vpit;				/**< C:Vertical pitch size     (15bits, 16384/16384 ~ 32767/16384).		*/
	USHORT							d_vsta;				/**< D:Vertical start position (14bits 0~16383).						*/
	USHORT							d_vpit;				/**< D:Vertical pitch size     (15bits, 16384/16384 ~ 32767/16384).		*/
	USHORT							e_vsta;				/**< E:Vertical start position (14bits 0~16383).						*/
	USHORT							e_vpit;				/**< E:Vertical pitch size     (15bits, 16384/16384 ~ 32767/16384).		*/
	USHORT							f_vsta;				/**< F:Vertical start position (14bits  0~16383).						*/
	USHORT							f_vpit;				/**< F:Vertical pitch size     (15bits, 16384/16384 ~ 32767/16384).		*/
} T_IM_SHDR_RESIZE_RSZVCNT;

/** SHDR resize parameter (TRMSTA) */
typedef struct {
	USHORT							c_hsta;				/**< C:Horizontal start position (14bits unit 2pixel, 0~11846).	*/
	USHORT							c_vsta;				/**< C:Vertical start position   (15bits unit 2pixel, 0~8774).	*/
	USHORT							d_hsta;				/**< D:Horizontal start position (14bits unit 2pixel, 0~11846).	*/
	USHORT							d_vsta;				/**< D:Vertical start position   (15bits unit 2pixel, 0~8774).	*/
	USHORT							e_hsta;				/**< E:Horizontal start position (14bits unit 2pixel, 0~11846).	*/
	USHORT							e_vsta;				/**< E:Vertical start position   (15bits unit 2pixel, 0~8774).	*/
	USHORT							f_hsta;				/**< F:Horizontal start position (14bits unit 2pixel, 0~11846).	*/
	USHORT							f_vsta;				/**< F:Vertical start position   (15bits unit 2pixel, 0~8774).	*/
} T_IM_SHDR_RESIZE_TRMSTA;

/** SHDR resize parameter (DRCOFST) */
typedef struct {
	UCHAR							a_y;				/**< A:Data range compression offset Y  (5bits).	*/
	INT8							a_cb;				/**< A:Data range compression offset Cr (5bits).	*/
	INT8							a_cr;				/**< A:Data range compression offset Cb (5bits).	*/
	UCHAR							c_y;				/**< C:Data range compression offset Y  (5bits).	*/
	INT8							c_cb;				/**< C:Data range compression offset Cr (5bits).	*/
	INT8							c_cr;				/**< C:Data range compression offset Cb (5bits).	*/
	UCHAR							d_y;				/**< D:Data range compression offset Y  (5bits).	*/
	INT8							d_cb;				/**< D:Data range compression offset Cr (5bits).	*/
	INT8							d_cr;				/**< D:Data range compression offset Cb (5bits).	*/
} T_IM_SHDR_RESIZE_DRCOFST;

/** SHDR resize parameter (DRCGAIN) */
typedef struct {
	USHORT							a_y;				/**< A:Data range compression gain Y  (9bits).		*/
	UCHAR							a_cb;				/**< A:Data range compression gain Cr (8bits).		*/
	UCHAR							a_cr;				/**< A:Data range compression gain Cb (8bits).		*/
	USHORT							c_y;				/**< C:Data range compression gain Y  (9bits).		*/
	UCHAR							c_cb;				/**< C:Data range compression gain Cr (8bits).		*/
	UCHAR							c_cr;				/**< C:Data range compression gain Cb (8bits).		*/
	USHORT							d_y;				/**< D:Data range compression gain Y  (9bits).		*/
	UCHAR							d_cb;				/**< D:Data range compression gain Cr (8bits).		*/
	UCHAR							d_cr;				/**< D:Data range compression gain Cb (8bits).		*/
} T_IM_SHDR_RESIZE_DRCGAIN;

/** SHDR resize parameter (DRCCLPH) */
typedef struct {
	UCHAR							a_y;				/**< A:Data range compression clip high Y  (8bits).	*/
	INT8							a_cb;				/**< A:Data range compression clip high Cr (8bits).	*/
	INT8							a_cr;				/**< A:Data range compression clip high Cb (8bits).	*/
	UCHAR							c_y;				/**< C:Data range compression clip high Y. (8bits)	*/
	INT8							c_cb;				/**< C:Data range compression clip high Cr (8bits).	*/
	INT8							c_cr;				/**< C:Data range compression clip high Cb (8bits).	*/
	UCHAR							d_y;				/**< D:Data range compression clip high Y  (8bits).	*/
	INT8							d_cb;				/**< D:Data range compression clip high Cr (8bits).	*/
	INT8							d_cr;				/**< D:Data range compression clip high Cb (8bits).	*/
} T_IM_SHDR_RESIZE_DRCCLPH;

/** SHDR resize parameter (DRCCLPL) */
typedef struct {
	UCHAR							a_y;				/**< A:Data range compression clip low Y  (8bits).	*/
	INT8							a_cb;				/**< A:Data range compression clip low Cr (8bits).	*/
	INT8							a_cr;				/**< A:Data range compression clip low Cb (8bits).	*/
	UCHAR							c_y;				/**< C:Data range compression clip low Y  (8bits).	*/
	INT8							c_cb;				/**< C:Data range compression clip low Cr (8bits).	*/
	INT8							c_cr;				/**< C:Data range compression clip low Cb (8bits).	*/
	UCHAR							d_y;				/**< D:Data range compression clip low Y  (8bits).	*/
	INT8							d_cb;				/**< D:Data range compression clip low Cr (8bits).	*/
	INT8							d_cr;				/**< D:Data range compression clip low Cb (8bits).	*/
} T_IM_SHDR_RESIZE_DRCCLPL;

/** SHDR resize parameter */
typedef struct {
	T_IM_SHDR_RESIZE_CHMODE			chmode;				/**< CHMODE :Channel mode.							*/
	T_IM_SHDR_RESIZE_WOUTSZ			woutsz;				/**< WOUTSZ :Output image size.						*/
	T_IM_SHDR_RESIZE_RSZHCNT		rszhcnt;			/**< RSZHCNT:Bilinear interpolation (Horizontal).	*/
	T_IM_SHDR_RESIZE_RSZVCNT		rszvcnt;			/**< RSZVCNT:Bilinear interpolation (Vertical).		*/
	T_IM_SHDR_RESIZE_TRMSTA			trmsta;				/**< TRMSTA :Triming start position.				*/
	T_IM_SHDR_RESIZE_DRCOFST		drcofst;			/**< DRCOFST:Data range compression offset.			*/
	T_IM_SHDR_RESIZE_DRCGAIN		drcgain;			/**< DRCGAIN:Data range compression gain.			*/
	T_IM_SHDR_RESIZE_DRCCLPH		drcclph;			/**< DRCCLPH:Data range compression clip high.		*/
	T_IM_SHDR_RESIZE_DRCCLPL		drcclpl;			/**< DRCCLPL:Data range compression clip low.		*/
} T_IM_SHDR_RESIZE;

/** SHDR SMC parameter (MCARMD) */
typedef struct {
	UCHAR							mca_mode;			/**< Average MC mode<br>
															<ul>
																<li>@ref D_IM_MCAMD_MAX
																<li>@ref D_IM_MCAMD_NR_EFFECT
																<li>@ref D_IM_MCAMD_MAX_VECTOR
																<li>@ref D_IM_MCAMD_MAX_MIN_VECTOR
															</ul>
														*/
	UCHAR							mcr_mode;			/**< Low confidence MC mode<br>
															<ul>
																<li>@ref D_IM_MCRMD_STANDARD
																<li>@ref D_IM_MCRMD_AVERAGE
																<li>@ref D_IM_MCRMD_MC
															</ul>
														*/
} T_IM_SHDR_SMC_MCARMD;

/** SHDR SMC parameter (MCMPMD) */
typedef struct {
	UCHAR							mcmp_mode;			/**< Reliability MAP output mode<br>
															<ul>
																<li>@ref D_IM_SHDR_OFF
																<li>@ref D_IM_SHDR_ON
															</ul>
														*/
	UCHAR							mvw_en;				/**< MVW enable<br>
															<ul>
																<li>@ref D_IM_SHDR_DISABLE
																<li>@ref D_IM_SHDR_ENABLE
															</ul>
														*/
} T_IM_SHDR_SMC_MCMPMD;

/** SHDR SMC parameter (MMVVAL) */
typedef struct {
	UCHAR							mmv_x;				/**< Fast determination threshold(x) (7bits, 0~64).	*/
	UCHAR							mmv_y;				/**< Fast determination threshold(y) (7bits, 0~48).	*/
} T_IM_SHDR_SMC_MMVVAL;

/** SHDR SMC parameter (MMVRCNT) */
typedef struct {
	UCHAR							mmvro_x;			/**< Reliability reduction(x) (5bits, 0~16).		*/
	UCHAR							mmvro_y;			/**< Reliability reduction(y) (5bits, 0~16).		*/
	UCHAR							mmvrk_x;			/**< Smooth coefficient(x) (3bits, 0~6).			*/
	UCHAR							mmvrk_y;			/**< Smooth coefficient(y) (3bits, 0~6).			*/
} T_IM_SHDR_SMC_MMVRCNT;

/** SHDR SMC parameter (MDBCNT) */
typedef struct {
	UCHAR							mdb_x;				/**< Reference range(x) (3bits, 0~5).				*/
	UCHAR							mdb_y;				/**< Reference range(y) (3bits, 0~3).				*/
} T_IM_SHDR_SMC_MDBCNT;

/** SHDR SMC parameter (MDBROCNT) */
typedef struct {
	UCHAR							mdbro_x;			/**< Weight of the distant area reference(x) (5bits, 0~16).	*/
	UCHAR							mdbro_y;			/**< Weight of the distant area reference(y) (5bits, 0~16).	*/
	UCHAR							mdbrk_x;			/**< Smooth coefficient(x) (3bits, 0~4).					*/
	UCHAR							mdbrk_y;			/**< Smooth coefficient(y) (3bits, 0~3).					*/
} T_IM_SHDR_SMC_MDBROCNT;

/** SHDR SMC parameter (MOLRCNT) */
typedef struct {
	UCHAR							molr_sm;			/**< Occlusion determination threshold(main) (4bits).	*/
	UCHAR							molr_ss;			/**< Occlusion determination threshold(sub) (4bits).	*/
	UCHAR							molr_o;				/**< Reliability reduction (5bits, 0~16)				*/
	UCHAR							molr_k;				/**< Smooth cofficient (2bits, 0~2).					*/
} T_IM_SHDR_SMC_MOLRCNT;

/** SHDR SMC parameter (MCPCNT) */
typedef struct {
	UCHAR							mcpx_s;				/**< Decision threshold (8bits, 0~128).		*/
	UCHAR							mcpx_o;				/**< Reliability reduction (5bits, 0~16).	*/
	UCHAR							mcpx_k;				/**< Smooth coefficient (3bits, 0~6).		*/
} T_IM_SHDR_SMC_MCPCNT;

/** SHDR SMC parameter (MMCRCNT) */
typedef struct {
	USHORT							mmcr_s;				/**< Decision threshold (10bits, 0~765).	*/
	UCHAR							mmcr_k;				/**< Smooth coefficient (3bits, 0~6).		*/
} T_IM_SHDR_SMC_MMCRCNT;

/** SHDR SMC parameter (MOLECNT) */
typedef struct {
	USHORT							mole_s;				/**< Decision threshold (10bits, 0~765).	*/
	UCHAR							mole_o;				/**< Reliability reduction (5bits, 0~16).	*/
	UCHAR							mole_k;				/**< Smooth coefficient (3bits, 0~6).		*/
} T_IM_SHDR_SMC_MOLECNT;

/** SHDR SMC parameter (MMVECNT) */
typedef struct {
	USHORT							mmve_s;				/**< Decision threshold (10bits, 0~765).	*/
	UCHAR							mmve_o;				/**< Reliability reduction (5bits, 0~16).	*/
	UCHAR							mmve_k;				/**< Smooth coefficient (3bits, 0~6).		*/
} T_IM_SHDR_SMC_MMVECNT;

/** SHDR SMC parameter (MCPXECNT) */
typedef struct {
	USHORT							mcpxe_s;			/**< Decision threshold (10bits, 0~765).	*/
	UCHAR							mcpxe_o;			/**< Reliability reduction (5bits, 0~16).	*/
	UCHAR							mcpxe_k;			/**< Smooth coefficient (3bits, 0~6).		*/
} T_IM_SHDR_SMC_MCPXECNT;

/** SHDR SMC parameter (MVLCNT) */
typedef struct {
	UCHAR							mvl_en;					/**< Vector accuracy improvement process enable<br>
																<ul>
																	<li>@ref D_IM_SHDR_DISABLE
																	<li>@ref D_IM_SHDR_ENABLE
																</ul>
															*/
	UCHAR							mvl_threshold;			/**< Reference decision threshold (7bits, 0~64).	*/
	UCHAR							mvl_count_threshold;	/**< Count threshold (7bits, 0~127)					*/
} T_IM_SHDR_SMC_MVLCNT;

/** SHDR SMC parameter */
typedef struct {
	T_IM_SHDR_SMC_MCARMD			mcarmd;				/**< MCARMD  :MC mode.							*/
	T_IM_SHDR_SMC_MCMPMD			mcmpmd;				/**< MCMPMD  :MC map output mode.				*/
	T_IM_SHDR_SMC_MMVVAL			mmvval;				/**< MMVVAL  :Fast determination threshold.		*/
	T_IM_SHDR_SMC_MMVRCNT			mmvrcnt;			/**< MMVRCNT :Reliability reduction.			*/
	T_IM_SHDR_SMC_MDBCNT			mdbcnt;				/**< MDBCNT  :Vector MAX value determination.	*/
	T_IM_SHDR_SMC_MDBROCNT			mdbrocnt;			/**< MDBROCNT:Distant area reference.			*/
	T_IM_SHDR_SMC_MOLRCNT			molrcnt;			/**< MOLRCNT :Occlusion detection.				*/
	T_IM_SHDR_SMC_MCPCNT			mcpcnt;				/**< MCPCNT  :Complex movement control.			*/
	T_IM_SHDR_SMC_MMCRCNT			mmcrcnt;			/**< MMCRCNT :Local afterimage.					*/
	T_IM_SHDR_SMC_MOLECNT			molecnt;			/**< MOLECNT :Occlusion reliability.			*/
	T_IM_SHDR_SMC_MMVECNT			mmvecnt;			/**< MMVECNT :Fast decision result.				*/
	T_IM_SHDR_SMC_MCPXECNT			mcpxecnt;			/**< MCPXECNT:Complex movement result.			*/
	T_IM_SHDR_SMC_MVLCNT			mvlcnt;				/**< MVLCNT  :Vector smoothing.					*/
} T_IM_SHDR_SMC;

/** SHDR AXI parameter (AXMD) */
typedef struct {
	UCHAR							r_burst;			/**< Transfer burst length (read ch)<br>
															<ul>
																<li>@ref D_IM_SHDR_BURSTLEN_512
																<li>@ref D_IM_SHDR_BURSTLEN_256
															</ul>
														*/
	UCHAR							w_burst;			/**< Transfer burst length (write ch)<br>
															<ul>
																<li>@ref D_IM_SHDR_BURSTLEN_512
																<li>@ref D_IM_SHDR_BURSTLEN_256
															</ul>
														*/
	UCHAR							w_w_mask;			/**< Write with mask<br>
															<ul>
																<li>@ref D_IM_SHDR_WWMASK_ON
																<li>@ref D_IM_SHDR_WWMASK_OFF
															</ul>
														*/
} T_IM_SHDR_AXI_AXMD;

/** SHDR AXI parameter (AXICTLAR) */
typedef struct {
	UCHAR							a_cahche[ 2 ];		/**< A:Cache type   (4bits).					*/
	UCHAR							a_protect[ 2 ];		/**< A:Protect type (3bits).					*/
	UCHAR							b_cahche[ 2 ];		/**< B:Cache type   (4bits).					*/
	UCHAR							b_protect[ 2 ];		/**< B:Protect type (3bits).					*/
	UCHAR							c_cahche[ 4 ];		/**< C:Cache type   (4bits).					*/
	UCHAR							c_protect[ 4 ];		/**< C:Protect type (3bits).					*/
	UCHAR							d_cahche[ 2 ];		/**< D:Cache type   (4bits).					*/
	UCHAR							d_protect[ 2 ];		/**< D:Protect type (3bits).					*/
} T_IM_SHDR_AXI_CTLR;

/** SHDR AXI parameter (AXICTLAW) */
typedef struct {
	UCHAR							a_cahche[ 4 ];		/**< A:Cache type   (4bits).					*/
	UCHAR							a_protect[ 4 ];		/**< A:Protect type (3bits).					*/
	UCHAR							b_cahche[ 6 ];		/**< B:Cache type   (4bits).					*/
	UCHAR							b_protect[ 6 ];		/**< B:Protect type (3bits).					*/
	UCHAR							c_cahche[ 7 ];		/**< C:Cache type   (4bits).					*/
	UCHAR							c_protect[ 7 ];		/**< C:Protect type (3bits).					*/
} T_IM_SHDR_AXI_CTLW;

/** SHDR AXI parameter */
typedef struct {
	T_IM_SHDR_AXI_AXMD				ax_mode;			/**< AXMD    :AXI transfer mode.				*/
	T_IM_SHDR_AXI_CTLR				r_ctl;				/**< AXICTLAR:AXI read control.					*/
	T_IM_SHDR_AXI_CTLW				w_ctl;				/**< AXICTLAW:AXI write control.				*/
} T_IM_SHDR_AXI;

/** AXI response */
typedef struct {
	UCHAR							r_resp_a[ D_IM_SHDR_AXI_READ_CH_DATA_MAX ];		/**< RRESPA:Read channel response.	*/
	UCHAR							r_resp_b[ D_IM_SHDR_AXI_READ_CH_DATA_MAX ];		/**< RRESPB:Read channel response.	*/
	UCHAR							r_resp_c[ D_IM_SHDR_AXI_READ_CH_DATA_MAX ];		/**< RRESPC:Read channel response.	*/
	UCHAR							r_resp_d[ D_IM_SHDR_AXI_READ_CH_DATA_MAX ];		/**< RRESPD:Read channel response.	*/
	UCHAR							w_resp_a[ D_IM_SHDR_AXI_WRITE_CH_DATA_MAX ];	/**< BRESPA:Write channel response.	*/
	UCHAR							w_resp_b[ D_IM_SHDR_AXI_WRITE_CH_DATA_MAX ];	/**< BRESPB:Write channel response.	*/
	UCHAR							w_resp_c[ D_IM_SHDR_AXI_WRITE_CH_DATA_MAX ];	/**< BRESPC:Write channel response.	*/
} T_IM_SHDR_AXI_RESP;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special.

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special.

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
SHDR initialize.<br>
@param[in]		force_flg						Force init flg.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
extern INT32 Im_SHDR_Init( BOOL force_flg );

/**
The semaphore of SHDR is acquired.<br>
@param[in]		wait_time						Time-out period.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_SEM_NG				Semaphore acquisition NG.
@remarks		This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
				This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern INT32 Im_SHDR_Open( const INT32 wait_time );

/**
The semaphore of SHDR is returned.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_SEM_NG				Semaphore acquisition NG.
@remarks		This API uses DDIM_User_Sig_Sem().
*/
extern INT32 Im_SHDR_Close( VOID );

/**
SHDR common parameters set.<br>
@param[in]		shdr_ctrl						Setting of SHDR parameters. See @ref T_IM_SHDR_CTRL.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@remarks		This API uses DDIM_User_Dly_Tsk().
*/
extern INT32 Im_SHDR_Ctrl_Common( const T_IM_SHDR_CTRL* const shdr_ctrl );

/**
SHDR common parameters get.<br>
@param[out]		shdr_ctrl						Setting of SHDR parameters. See @ref T_IM_SHDR_CTRL.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_Ctrl_Common( T_IM_SHDR_CTRL* const shdr_ctrl );

/**
SHDR pre parameter set.<br>
@param[in]		shdr_pre						Setting of SHDR pre parameters. See @ref T_IM_SHDR_PRE.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Pre( const T_IM_SHDR_PRE* const shdr_pre );

/**
SHDR get pre parameter.<br>
@param[out]		shdr_pre						Setting of SHDR pre parameters. See @ref T_IM_SHDR_PRE.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_Pre( T_IM_SHDR_PRE* const shdr_pre );

/**
SHDR FMD parameter set.<br>
@param[in]		shdr_fmd						Setting of SHDR pre parameters. See @ref T_IM_SHDR_FMD.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Fmd( const T_IM_SHDR_FMD* const shdr_fmd );

/**
SHDR get FMD parameter.<br>
@param[out]		shdr_fmd						Setting of SHDR pre parameters. See @ref T_IM_SHDR_FMD.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_Fmd( T_IM_SHDR_FMD* const shdr_fmd );

/**
SHDR PMSK parameter set.<br>
@param[in]		shdr_pmsk						Setting of SHDR PMSK parameters. See @ref T_IM_SHDR_PMSK.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_PreMask( const T_IM_SHDR_PMSK* const shdr_pmsk );

/**
SHDR get PMSK parameter.<br>
@param[out]		shdr_pmsk						Setting of SHDR PMSK parameters. See @ref T_IM_SHDR_PMSK.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_PreMask( T_IM_SHDR_PMSK* const shdr_pmsk );

/**
SHDR SPNR parameter set.<br>
@param[in]		shdr_spnr						Setting of SHDR SPNR parameters. See @ref T_IM_SHDR_SPNR.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Spnr( const T_IM_SHDR_SPNR* const shdr_spnr );

/**
SHDR get SPNR parameter.<br>
@param[out]		shdr_spnr						Setting of SHDR SPNR parameters. See @ref T_IM_SHDR_SPNR.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_Spnr( T_IM_SHDR_SPNR* const shdr_spnr );

/**
SHDR MSK parameter set.<br>
@param[in]		shdr_msk						Setting of SHDR MSK parameters. See @ref T_IM_SHDR_MSK.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Mask( const T_IM_SHDR_MSK* const shdr_msk );

/**
SHDR get MSK parameter.<br>
@param[out]		shdr_msk						Setting of SHDR MSK parameters. See @ref T_IM_SHDR_MSK.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_Mask( T_IM_SHDR_MSK* const shdr_msk );

/**
SHDR BLD parameter set.<br>
@param[in]		shdr_bld						Setting of SHDR BLD parameters. See @ref T_IM_SHDR_BLD.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Blend( const T_IM_SHDR_BLD* const shdr_bld );

/**
SHDR get BLD parameter.<br>
@param[out]		shdr_bld						Setting of SHDR BLD parameters. See @ref T_IM_SHDR_BLD.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_Blend( T_IM_SHDR_BLD* const shdr_bld );

/**
SHDR POST parameter set.<br>
@param[in]		shdr_post						Setting of SHDR POST parameters. See @ref T_IM_SHDR_POST.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Post( const T_IM_SHDR_POST* const shdr_post );

/**
SHDR get POST parameter.<br>
@param[out]		shdr_post						Setting of SHDR POST parameters. See @ref T_IM_SHDR_POST.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_Post( T_IM_SHDR_POST* const shdr_post );

/**
SHDR resize parameter set.<br>
@param[in]		shdr_resize						Setting of SHDR resize parameters. See @ref T_IM_SHDR_RESIZE.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Resize( const T_IM_SHDR_RESIZE* const shdr_resize );

/**
SHDR get resize parameter.<br>
@param[out]		shdr_resize						Setting of SHDR resize parameters. See @ref T_IM_SHDR_RESIZE.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_Resize( T_IM_SHDR_RESIZE* const shdr_resize );

/**
SHDR SMC parameter set.<br>
@param[in]		shdr_smc						Setting of SHDR SMC parameters. See @ref T_IM_SHDR_SMC.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Smc( const T_IM_SHDR_SMC* const shdr_smc );

/**
SHDR get SMC parameter.<br>
@param[out]		shdr_smc						Setting of SHDR SMC parameters. See @ref T_IM_SHDR_SMC.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_Smc( T_IM_SHDR_SMC* const shdr_smc );

/**
SHDR degamma R table set.<br>
@param[in]		src_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		write_ofs_num					Write offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Degamma_R( const USHORT* const src_tbl, const USHORT write_ofs_num, const USHORT array_num );

/**
SHDR degamma G table set.<br>
@param[in]		src_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		write_ofs_num					Write offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Degamma_G( const USHORT* const src_tbl, const USHORT write_ofs_num, const USHORT array_num );

/**
SHDR degamma B table set.<br>
@param[in]		src_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		write_ofs_num					Write offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Degamma_B( const USHORT* const src_tbl, const USHORT write_ofs_num, const USHORT array_num );

/**
SHDR degamma table set.<br>
@param[in]		src_tbl_r						Pointer of Table data R(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		src_tbl_g						Pointer of Table data G(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		src_tbl_b						Pointer of Table data B(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		write_ofs_num					Write offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Fail - Parameter error.
*/
extern INT32 Im_SHDR_Set_Degamma( const USHORT* const src_tbl_r, const USHORT* const src_tbl_g, const USHORT* const src_tbl_b, const USHORT write_ofs_num, const USHORT array_num );

/**
SHDR get degamma R table.<br>
@param[out]		dst_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		read_ofs_num					Read offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		read_sel						Degamma table read selector.<br>
													<ul><li>@ref D_IM_SHDR_DGTRSEL_PREFL.
														<li>@ref D_IM_SHDR_DGTRSEL_PREFS.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_ODD.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_ODD.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
extern INT32 Im_SHDR_Get_Degamma_R( USHORT* const dst_tbl, const USHORT read_ofs_num, const USHORT array_num, const UCHAR read_sel );

/**
SHDR get degamma G table.<br>
@param[out]		dst_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		read_ofs_num					Read offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		read_sel						Degamma table read selector.<br>
													<ul><li>@ref D_IM_SHDR_DGTRSEL_PREFL.
														<li>@ref D_IM_SHDR_DGTRSEL_PREFS.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_ODD.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_ODD.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
extern INT32 Im_SHDR_Get_Degamma_G( USHORT* const dst_tbl, const USHORT read_ofs_num, const USHORT array_num, const UCHAR read_sel );

/**
SHDR get degamma B table.<br>
@param[out]		dst_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		read_ofs_num					Read offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		read_sel						Degamma table read selector.<br>
													<ul><li>@ref D_IM_SHDR_DGTRSEL_PREFL.
														<li>@ref D_IM_SHDR_DGTRSEL_PREFS.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_ODD.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_ODD.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
extern INT32 Im_SHDR_Get_Degamma_B( USHORT* const dst_tbl, const USHORT read_ofs_num, const USHORT array_num, const UCHAR read_sel );

/**
SHDR get degamma table.<br>
@param[out]		dst_tbl_r						Pointer of Table data R(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[out]		dst_tbl_g						Pointer of Table data G(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[out]		dst_tbl_b						Pointer of Table data B(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		read_ofs_num					Read offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		read_sel						Degamma table read selector.<br>
													<ul><li>@ref D_IM_SHDR_DGTRSEL_PREFL.
														<li>@ref D_IM_SHDR_DGTRSEL_PREFS.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_ODD.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_ODD.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
extern INT32 Im_SHDR_Get_Degamma( USHORT* const dst_tbl_r, USHORT* const dst_tbl_g, USHORT* const dst_tbl_b, const USHORT read_ofs_num, const USHORT array_num, const UCHAR read_sel );

/**
SHDR AXI parameter set.<br>
@param[in]		shdr_axi						Setting of SHDR AXI parameters. See @ref T_IM_SHDR_AXI.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Set_Axi( const T_IM_SHDR_AXI* const shdr_axi );

/**
SHDR get AXI parameter.<br>
@param[out]		shdr_axi						Setting of SHDR AXI parameters. See @ref T_IM_SHDR_AXI.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_Axi( T_IM_SHDR_AXI* const shdr_axi );

/**
SHDR get AXI response.<br>
@param[out]		axi_resp						AXI response. See @ref T_IM_SHDR_AXI_RESP.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
extern INT32 Im_SHDR_Get_Axi_Response( T_IM_SHDR_AXI_RESP* const axi_resp );

/**
SHDR start.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
@remarks		This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_SHDR_Start( VOID );

/**
SHDR wait end.<br>
@param[out]		p_wait_factor					Pointer of factor parameter which release wait process.
@param[in]		wait_time						Wait time [msec]. The valid range is -1, 0, ... .
												If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_TIMEOUT				Time out.
@retval			D_IM_SHDR_AXI_ERROR				AXI BUS error.
@retval			D_IM_SHDR_NG					System NG.
@remarks		This API uses DDIM_User_Clr_Flg().
@remarks		This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Im_SHDR_Wait_End( UINT32* const p_wait_factor, const INT32 wait_time );

/**
SHDR stop.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
@retval			D_IM_SHDR_NG					System NG.
@remarks		This API uses DDIM_User_Set_Flg().
*/
extern INT32 Im_SHDR_Stop( VOID );

/**
SHDR interupt handler.<br>
@retval			None.
@remarks		This API uses DDIM_User_Set_Flg().
*/
extern VOID Im_SHDR_Int_Handler( VOID );

/**
SHDR line interupt handler.<br>
@retval			None.
*/
extern VOID Im_SHDR_Line_Int_Handler( VOID );

/*@}*/	// end of im_shdr

#ifdef __cplusplus
}
#endif

#endif	// __IM_SHDR_H__

/**
@weakgroup im_shdr
@{
	@section im_shdr_code Sample code
	- @ref SHDR_Init
	- @ref SHDR_Start
	- @ref SHDR_Stop

	<hr>

	@section SHDR_Init			SHDR init
	@code
	Im_SHDR_Init();														// Release software-reset

	Dd_GIC_Ctrl( E_DD_GIC_INTID_SHDR_INT,      1, D_DD_GIC_PRI30, 1 );	// Enable interrupt
	Dd_GIC_Ctrl( E_DD_GIC_INTID_SHDR_LINE_INT, 1, D_DD_GIC_PRI30, 1 );	// Enable interrupt
	@endcode
	<br>
	<br>
	@section SHDR_Start			SHDR start
	@code
	T_IM_SHDR_CTRL	shdr_ctrl;
	UINT32			wait_factor;

	// Setup parameters
	shdr_ctrl.image_set.in_bit_depth = 0;

	Im_SHDR_Open( 100 );

	Im_SHDR_Ctrl_Common( &shdr_ctrl );

	Im_SHDR_Start();

	Im_SHDR_Wait_End( &wait_factor, 1000 );

	Im_SHDR_Close();
	@endcode
	<br>
	<br>
	@section SHDR_Stop			SHDR stop
	@code
	UINT32	wait_factor;
	INT32	ret;

	Im_SHDR_Start();								// SHDR start.

	Im_SHDR_Stop();									// Force stop.

	ret = Im_SHDR_Wait_End( &wait_factor, 1000 );	// Wait.

	@endcode
	<br>
	<br>
@}*/ /* end of im_shdr_code */
