/**
 * @file		im_ela.h
 * @brief		Header file of JDSELA2D driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/**
@addtogroup im_ela
@{

	- @ref Noise_reduction_data_generation_flow
	- @ref Sequence_of_ELA_sync 
	- @ref Sequence_of_ELA_sync2 
	- @ref im_ela_sample_section1
	- @ref im_ela_sample_section2

	<hr>
	@section Noise_reduction_data_generation_flow Noise reduction data generation flow
	@image html im_ela_noise_reduction_flow.png
	<br>
	<br>
	@section Sequence_of_ELA_sync Sequence of ELA ( synchronous processing )
	@image html im_ela_sync.png
	<br>
	<br>
	@section Sequence_of_ELA_sync2 Sequence of ELA using Im_ELA_Ctrl_Next function ( synchronous processing )
	@image html im_ela_sync2.png
	<br>
	<br>
*/

#ifndef _IM_ELA_H_
#define _IM_ELA_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// result
#define	D_IM_ELA_INPUT_PARAM_ERROR		(D_IM_ELA | D_DDIM_INPUT_PARAM_ERROR)	/**< Processing result code<br> Input parameter error			(0x0A000001) */
#define	D_IM_ELA_MACRO_BUSY				(D_IM_ELA | D_DDIM_MACRO_BUSY_NG)		/**< Processing result code<br> Macro busy 						(0x0A000005) */
#define D_IM_ELA_SEM_NG					(D_IM_ELA | D_DDIM_SEM_NG)				/**< Processing result code<br> Semaphore acquisition NG		(0x0A000010) */
#define	D_IM_ELA_SEM_TIMEOUT_ERR		(D_IM_ELA | D_DDIM_SEM_TIMEOUT)			/**< Processing result code<br> Semaphore acquisition Time Out	(0x0A000011) */
#define	D_IM_ELA_TIMEOUT				(D_IM_ELA | D_DDIM_TIMEOUT)				/**< Processing result code<br> Time Out 						(0x0A000013) */
#define	D_IM_ELA_NG						(D_IM_ELA | D_DDIM_SYSTEM_ERROR)		/**< Processing result code<br> Processing NG					(0x0A000099) */
#define	D_IM_ELA_PROCESS_END			(D_IM_ELA | 0x000000FF)					/**< Processing result code<br> Process End 					(0x0A0000FF) */

#define D_IM_ELA_STAGE_MAX				(5)				/**< ELA macro stage cnt max	*/
#define D_IM_ELA_STEP_MAX				(16)			/**< ELA macro step cnt max	*/

/** Function Enable
*/
#define	D_IM_ELA_DISABLE				(0)				/**< Disable						*/
#define	D_IM_ELA_ENABLE					(1)				/**< Enable							*/

// ELA Register - NSLMD (Noise data mode )
/** Noise data mode
*/
#define	D_IM_ELA_NSLMD_REDUCTION		0x00			/**< Noise data mode : Reduction mode 			*/
#define	D_IM_ELA_NSLMD_SAME				0x01			/**< Noise data mode : Same magnification mode	*/
																/* @NSLMD.NSLMD					*/

/** Noise data precision
*/
#define	D_IM_ELA_NSLRES_PRECISION_1		0x00			/**< Noise data precision : Precision 1	*/
#define	D_IM_ELA_NSLRES_PRECISION_2		0x01			/**< Noise data precision : Precision 2	*/
#define	D_IM_ELA_NSLRES_PRECISION_3		0x02			/**< Noise data precision : Precision 3	*/
#define	D_IM_ELA_NSLRES_PRECISION_4		0x03			/**< Noise data precision : Precision 4	*/
																/* @NSLMD.NSLMD					*/

/** Bayer data start-pixel type
*/
#define	D_IM_ELA_ORG_R					0x00			/**< Bayer data start-pixel type : R Pixel 		*/
#define	D_IM_ELA_ORG_B					0x01			/**< Bayer data start-pixel type : B Pixel 		*/
#define	D_IM_ELA_ORG_GR					0x02			/**< Bayer data start-pixel type : Gr Pixel 	*/
#define	D_IM_ELA_ORG_GB					0x03			/**< Bayer data start-pixel type : Gb Pixel 	*/
																/* @BYRTYP.ORG					*/

/**	Bayer input/output data type
*/
#define	D_IM_ELA_DATATYP_8BIT			0x00			/**< Bayer input/output data type : 8bits data	*/
#define	D_IM_ELA_DATATYP_12BIT			0x02			/**< Bayer input/output data type : 12bits pack data	*/
#define	D_IM_ELA_DATATYP_16BIT			0x03			/**< Bayer input/output data type : 16bits data	*/
																/* @ELABYR.EIRTYP, ELABYR.EIWTYP		*/

/**	Bayer input/output bit shift mode
*/
#define	D_IM_ELA_BIT_SHIFT_MODE_NONE	(0)			/**< Bayer input/output bit shift mode : No shift				*/
#define	D_IM_ELA_BIT_SHIFT_MODE_R2		(1)			/**< Bayer input/output bit shift mode : 2bit right shift		*/
#define	D_IM_ELA_BIT_SHIFT_MODE_R4		(2)			/**< Bayer input/output bit shift mode : 4bit right shift		*/
#define	D_IM_ELA_BIT_SHIFT_MODE_L2		(3)			/**< Bayer input/output bit shift mode : 2bit shift to the left	*/
#define	D_IM_ELA_BIT_SHIFT_MODE_L4		(4)			/**< Bayer input/output bit shift mode : 4bit shift to the left	*/

/**	Number of the output bayer data
*/
#define	D_IM_ELA_EIWOUT_NONE			0x00			/**< Number of the output bayer data : none		*/
#define	D_IM_ELA_EIWOUT_1				0x01			/**< Number of the output bayer data : 1		*/
#define	D_IM_ELA_EIWOUT_4				0x02			/**< Number of the output bayer data : 4		*/
																/* @ELABYR.EIWOUT		*/

/**	Defect pixel correction mode
*/
#define	D_IM_ELA_ELADCMD_OFF			0x00			/**< Defect pixel correction mode : Defect correction off						*/
#define	D_IM_ELA_ELADCMD_WEAK			0x01			/**< Defect pixel correction mode : Defect tolerance on(comparatively weak)		*/
#define	D_IM_ELA_ELADCMD_STRONG			0x03			/**< Defect pixel correction mode : Defect tolerance on(comparatively strong)	*/
																/* @BYRTYP.ELADCMD		*/

#define	D_IM_ELA_EIWCH_ADDR_MAX			(4)				/**< The maximum values of output bayer data top address			*/
#define	D_IM_ELA_ENRECH_ADDR_MAX		(16)			/**< The maximum values of input extraction noise data top address	*/
#define	D_IM_ELA_EACH_DATA_MAX			(3)				/**< The maximum values of EAch data								*/
#define	D_IM_ELA_DEKNEE_TBL_MAX			(256)			/**< The maximum values of Deknee table data						*/

// The kind of channel which the error generated
#define D_IM_ELA_NORMAL_END				(0x00)			/**< Normal end.<br> */
#define D_IM_ELA_AXI_ERR_CH_W			(0x01)			/**< The AXI error occurred by the write channel.<br> */
#define D_IM_ELA_AXI_ERR_CH_R			(0x02)			/**< The AXI error occurred by the read channel.<br> */
#define D_IM_ELA_AXI_ERR_CH_RW			(0x03)			/**< The AXI error occurred by the read channel and write channel.<br> */


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/**	ELF luminance threshold selection. */
typedef enum {
	E_IM_ELA_ELFL_T1 = 0,				/**< ELF luminance threshold 1	*/
	E_IM_ELA_ELFL_T2,					/**< ELF luminance threshold 2	*/
	E_IM_ELA_ELFL_T3,					/**< ELF luminance threshold 3	*/
	E_IM_ELA_ELFL_T4,					/**< ELF luminance threshold 4	*/
	E_IM_ELA_ELFL_MAX					/**< Enumeration stopper */
} E_IM_ELA_ELFL;

/**	Noise characteristics selection. */
typedef enum {
	E_IM_ELA_ANB_1 = 0,					/**< Noise characteristics number-1	*/
	E_IM_ELA_ANB_2,						/**< Noise characteristics number-2	*/
	E_IM_ELA_ANB_3,						/**< Noise characteristics number-3	*/
	E_IM_ELA_ANB_4,						/**< Noise characteristics number-4	*/
	E_IM_ELA_ANB_MAX					/**< Enumeration stopper */
} E_IM_ELA_ANB;

/**	Edge degree threshold selection. */
typedef enum {
	E_IM_ELA_ALNGE_1 = 0,				/**< Edge degree threshold number-1	*/
	E_IM_ELA_ALNGE_2,					/**< Edge degree threshold number-2	*/
	E_IM_ELA_ALNGE_3,					/**< Edge degree threshold number-3	*/
	E_IM_ELA_ALNGE_MAX					/**< Enumeration stopper */
} E_IM_ELA_ALNGE;

/**	NR intensity selection. */
typedef enum {
	E_IM_ELA_ALNG_L1E1N = 0,			/**< Luminance L1, Edge E1	*/
	E_IM_ELA_ALNG_L1E2N,				/**< Luminance L1, Edge E2	*/
	E_IM_ELA_ALNG_L1E3N,				/**< Luminance L1, Edge E3	*/
	E_IM_ELA_ALNG_L2E1N,				/**< Luminance L2, Edge E1	*/
	E_IM_ELA_ALNG_L2E2N,				/**< Luminance L2, Edge E2	*/
	E_IM_ELA_ALNG_L2E3N,				/**< Luminance L2, Edge E3	*/
	E_IM_ELA_ALNG_L3E1N,				/**< Luminance L3, Edge E1	*/
	E_IM_ELA_ALNG_L3E2N,				/**< Luminance L3, Edge E2	*/
	E_IM_ELA_ALNG_L3E3N,				/**< Luminance L3, Edge E3	*/
	E_IM_ELA_ALNG_L4E1N,				/**< Luminance L4, Edge E1	*/
	E_IM_ELA_ALNG_L4E2N,				/**< Luminance L4, Edge E2	*/
	E_IM_ELA_ALNG_L4E3N,				/**< Luminance L4, Edge E3	*/
	E_IM_ELA_ALNG_MAX					/**< Enumeration stopper */
} E_IM_ELA_ALNG;

/**	Edge applicable threshold contour L selection. */
typedef enum {
	E_IM_ELA_LCOME_1 = 0,				/**< Edge applicable threshold contour L number-1	*/
	E_IM_ELA_LCOME_2,					/**< Edge applicable threshold contour L number-2	*/
	E_IM_ELA_LCOME_MAX					/**< Enumeration stopper */
} E_IM_ELA_LCOME;

/**	Applied intensity L selection. */
typedef enum {
	E_IM_ELA_LCOMEN_1 = 0,				/**< Applied intensity L number-1	*/
	E_IM_ELA_LCOMEN_2,					/**< Applied intensity L number-2	*/
	E_IM_ELA_LCOMEN_MAX					/**< Enumeration stopper */
} E_IM_ELA_LCOMEN;

/**	Adjustment method parameter selection. */
typedef enum {
	E_IM_ELA_ELF_LEVEL_0 = 0,			/**< Level 0 : Suppress the noise components of up to 9 pixels wavelength (Not used)	*/
	E_IM_ELA_ELF_LEVEL_1,				/**< Level 1 : Suppress the noise components of up to 16 pixels wavelength	*/
	E_IM_ELA_ELF_LEVEL_2,				/**< Level 2 : Suppress the noise components of up to 32 pixels wavelength	*/
	E_IM_ELA_ELF_LEVEL_3,				/**< Level 3 : Suppress the noise components of up to 64 pixels wavelength	*/
	E_IM_ELA_ELF_LEVEL_4,				/**< Level 4 : Suppress the noise components of up to 128 pixels wavelength	*/
	E_IM_ELA_ELF_LEVEL_5,				/**< Level 5 : Suppress the noise components of up to 256 pixels wavelength	*/
	E_IM_ELA_ELF_LEVEL_MAX				/**< Enumeration stopper */
} E_IM_ELA_QELFLVL;

/**	ELA density mode selection. */
typedef enum {
	E_IM_ELA_DENSITY_MODE_0 = 0,		/**< ELA density mode 0<br>
											 Noise output data size : Reduction<br>
											 Number of noise data inputs : 1	*/
	E_IM_ELA_DENSITY_MODE_1,			/**< ELA density mode 1<br>
											 Noise output data size : Same magnification<br>
											 Number of noise data inputs : 1	*/
	E_IM_ELA_DENSITY_MODE_2,			/**< ELA density mode 2<br>
											 Noise output data size : Same magnification<br>
											 Number of noise data inputs : 4	*/
	E_IM_ELA_DENSITY_MODE_3,			/**< ELA density mode 3<br>
											 Noise output data size : Same magnification<br>
											 Number of noise data inputs : 16	*/
	E_IM_ELA_DENSITY_MODE_MAX			/**< Enumeration stopper */
} E_IM_ELA_QELAMD;


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

typedef VOID (*T_IM_ELA_CALLBACK)( ULONG interrupt_flag_mask );	/**< Callback function pointer	*/

/** ELA common control data */
typedef struct {
	UCHAR	noise_data_mode;							/**< NSLMD.NSLMD	: Noise data mode 
														     <ul><li>@ref D_IM_ELA_NSLMD_REDUCTION
																 <li>@ref D_IM_ELA_NSLMD_SAME</ul>	*/
	UCHAR	noise_data_precision;						/**< NSLMD.NSLRES	: Noise data precision 
														     <ul><li>@ref D_IM_ELA_NSLRES_PRECISION_1
																 <li>@ref D_IM_ELA_NSLRES_PRECISION_2
																 <li>@ref D_IM_ELA_NSLRES_PRECISION_3
																 <li>@ref D_IM_ELA_NSLRES_PRECISION_4</ul>
<pre>
	D_IM_ELA_NSLRES_PRECISION_1 : High, Maximum amplitude : Small
	D_IM_ELA_NSLRES_PRECISION_2 :  |  , Maximum amplitude :   |  	
	D_IM_ELA_NSLRES_PRECISION_3 :  |  , Maximum amplitude :   |  	
	D_IM_ELA_NSLRES_PRECISION_4 : Low , Maximum amplitude :  Wide
</pre>
																 	*/
	UCHAR	noise_data_compress_enable;					/**< NSLMD.NSLKNE	: Noise data compression enable 
														     <ul><li>@ref D_IM_ELA_DISABLE
																 <li>@ref D_IM_ELA_ENABLE</ul>	*/
	UCHAR	bayer_start_pixel_type;						/**< BYRTYP.ORG	: Bayer data start-pixel type
														     <ul><li>@ref D_IM_ELA_ORG_R
																 <li>@ref D_IM_ELA_ORG_B
																 <li>@ref D_IM_ELA_ORG_GR
																 <li>@ref D_IM_ELA_ORG_GB</ul>	*/
} T_IM_ELA_CTRL_COMMON;

/** ELA bayer data control data */
typedef struct {
	UCHAR	in_data_type;						/**< ELABYR.EIRTYP	: Input data type
												     <ul><li>@ref D_IM_ELA_DATATYP_8BIT
														 <li>@ref D_IM_ELA_DATATYP_12BIT
														 <li>@ref D_IM_ELA_DATATYP_16BIT</ul>	*/
	UCHAR	in_data_shift_mode;					/**< ELABYR.EIRSFT	: Input bit shift mode
												     <ul><li>@ref D_IM_ELA_BIT_SHIFT_MODE_R2
														 <li>@ref D_IM_ELA_BIT_SHIFT_MODE_NONE
														 <li>@ref D_IM_ELA_BIT_SHIFT_MODE_L2 (Some saturation compensation)
														 <li>@ref D_IM_ELA_BIT_SHIFT_MODE_L4 (Some saturation compensation)</ul>	*/
	UCHAR	out_data_type;						/**< ELABYR.EIWTYP	: Output data type
												     <ul><li>@ref D_IM_ELA_DATATYP_8BIT
														 <li>@ref D_IM_ELA_DATATYP_12BIT</ul>	*/
	UCHAR	out_data_shift_mode;				/**< ELABYR.EIWSFT	: Output bit shift mode
												     <ul><li>@ref D_IM_ELA_BIT_SHIFT_MODE_NONE
														 <li>@ref D_IM_ELA_BIT_SHIFT_MODE_R2
														 <li>@ref D_IM_ELA_BIT_SHIFT_MODE_R4</ul>	*/
	UCHAR	deknee_enable;						/**< ELABYR.DKNEN	: Enabling the Deknee processing for bayer data entry
												     <ul><li>@ref D_IM_ELA_DISABLE
														 <li>@ref D_IM_ELA_ENABLE</ul>	*/
	UCHAR	ob_zsf_enable;						/**< ELABYR.EOBAEN	: Enabling the "OB correction"/"Optical zero shift" processing for bayer data entry
												     <ul><li>@ref D_IM_ELA_DISABLE
														 <li>@ref D_IM_ELA_ENABLE</ul>	*/
} T_IM_ELA_CTRL_BYR;

/** Noise characteristics element */
typedef struct {
	USHORT	offset[E_IM_ELA_ANB_MAX];		/**< ANBOR, ANBOG, ANBOB	: Offset noise characteristics (12bits)	*/
	USHORT	gain[E_IM_ELA_ANB_MAX];			/**< ANBKR, ANBKG, ANBKB	: Gain noise characteristics (12bits)	*/
	USHORT	boundary[E_IM_ELA_ANB_MAX];		/**< ANBDR, ANBDG, ANBDB	: Region boundary noise characteristics (12bits)	*/
} T_IM_ELA_CTRL_CORE_ANB;

/** Noise extraction limit data control */
typedef struct {
	USHORT	noise_plus_limit;				/**< NSLNGLMT.NSLNGLMP	: Noise element plus side limit (10bits)		*/
	USHORT	noise_minus_limit;				/**< NSLNGLMT.NSLNGLMM	: Noise element minus side limit (10bits)		*/
} T_IM_ELA_CTRL_LIMIT;

/** ELA core control data */
typedef struct {
	SHORT	optical_zero_level_shift;			/**< EZSFTA1	: Optical zero level shift parameter (11bits signed)	*/
	T_IM_ELA_CTRL_LIMIT	limit;					/**< Noise extraction limit data control parameter				*/
	UCHAR	defect_correction_mode;				/**< BYRTYP.ELADCMD	: Defect pixel correction mode
											     <ul><li>@ref D_IM_ELA_ELADCMD_OFF
													 <li>@ref D_IM_ELA_ELADCMD_WEAK
													 <li>@ref D_IM_ELA_ELADCMD_STRONG</ul>									*/
	USHORT	zero_point_level;					/**< ELFZP	: Zero point level (12bits)											*/
	USHORT	elflt[E_IM_ELA_ELFL_MAX];			/**< ELFL	: ELF luminance threshold (12bits)<br>
													Please set up so that "elflt[E_IM_ELA_ELFL_T1] <= elflt[E_IM_ELA_ELFL_T2] <= elflt[E_IM_ELA_ELFL_T3] <= elflt[E_IM_ELA_ELFL_T4]" Always.	*/
	T_IM_ELA_CTRL_CORE_ANB	anb_r;				/**< Noise characteristics of the pixel R						 				*/
	T_IM_ELA_CTRL_CORE_ANB	anb_g;				/**< Noise characteristics of the pixel G						 				*/
	T_IM_ELA_CTRL_CORE_ANB	anb_b;				/**< Noise characteristics of the pixel B						 				*/
	USHORT	lumen_gain_r;						/**< LGTW.LGTWR	: R Gain for luminance (10bits)	<br>
													value range: [0 - 1023(=0/256 - 1023/256)]									*/
	USHORT	lumen_gain_g;						/**< LGTW.LGTWG	: G Gain for luminance (10bits)	<br>
													value range: [0 - 1023(=0/256 - 1023/256)]									*/
	USHORT	lumen_gain_b;						/**< LGTW.LGTWB	: B Gain for luminance (10bits)	<br>
													value range: [0 - 1023(=0/256 - 1023/256)]									*/
	USHORT	lumen_factor_r;						/**< LGT.LGTR	: R factor for the luminance (5bits)	<br>
													value range: [0 - 31(=0/16 - 31/16)]										*/
	USHORT	lumen_factor_g;						/**< LGT.LGTG	: G factor for the luminance (5bits)	<br>
													value range: [0 - 31(=0/16 - 31/16)]										*/
	USHORT	lumen_factor_b;						/**< LGT.LGTB	: B factor for the luminance (5bits)	<br>
													value range: [0 - 31(=0/16 - 31/16)]										*/
	USHORT	lumen_factor_c;						/**< LGT.LGTC	: Color difference parameter for the luminance (5bits)	<br>
													value range: [0 - 31(=0/16 - 31/16)]										*/
	USHORT	lumen_edge_r;						/**< LGTE.LGTER	: R factor for the degree of weight edge (5bits)	<br>
													value range: [0 - 31(=0/16 - 31/16)]										*/
	USHORT	lumen_edge_g;						/**< LGTE.LGTEG	: G factor for the degree of weight edge (5bits)	<br>
													value range: [0 - 31(=0/16 - 31/16)]										*/
	USHORT	lumen_edge_b;						/**< LGTE.LGTEB	: B factor for the degree of weight edge (5bits)	<br>
													value range: [0 - 31(=0/16 - 31/16)]										*/
	USHORT	edge_adjust_a1;						/**< EDG2ANBA.EDG2ANBA	: Set the degree adjustment factor-1 edge of A (8bits)	<br>
													value range: [0 - 255(=0/64 - 255/64)]										*/
	USHORT	edge_adjust_a2;						/**< EDGSCLA.EDGSCLA	: Set the degree adjustment factor-2 edge of A (10bits)	*/
	USHORT	edge_adjust_b1;						/**< EDG2ANBB.EDG2ANBB	: Set the degree adjustment factor-1 edge of B (10bits)	<br>
													value range: [0 - 1023(=0/64 - 1023/64)]										*/
	USHORT	edge_adjust_b2;						/**< EDGSCLB.EDGSCLB	: Set the degree adjustment factor-2 edge of B (10bits)	*/
	USHORT	edge_threshold[E_IM_ELA_ALNGE_MAX];	/**< ALNGE	: Edge degree threshold (12bits)<br>
													Please set up so that "edge_threshold[E_IM_ELA_ALNGE_1] <= edge_threshold[E_IM_ELA_ALNGE_2] <= edge_threshold[E_IM_ELA_ALNGE_3]" Always.	*/
	USHORT	nr[E_IM_ELA_ALNG_MAX];				/**< ALNG	: NR intensity (9bits)<br>
													Set each degree the NR intensity in luminance(Lm), edge(En). (m=1~4, n=1~3)<br>
													Please set in the range of 0.0 to 1.0.	*/
	USHORT	l_noise;							/**< LFTH	: L extraction threshold noise (12bits)								*/
	USHORT	l_edge[E_IM_ELA_LCOME_MAX];			/**< LCOME	: Edge applicable threshold contour L (12bits)<br>
													Please set up so that "l_edge[E_IM_ELA_LCOMEN_1] <= l_edge[E_IM_ELA_LCOME_2]" Always.	*/
	USHORT	l_intense[E_IM_ELA_LCOMEN_MAX];		/**< LCOMEN	: Applied intensity L (9bits)<br>
													Please set in the range of 0.0 to 1.0.	*/
} T_IM_ELA_CTRL_CORE;

/** OB Correction control */
typedef struct {
	SHORT	ob_rr;							/**< OBOF.EOBRR		: OB Correction R Pixel (11bits)					*/
	SHORT	ob_bb;							/**< OBOF.EOBBB		: OB Correction B Pixel (11bits)					*/
	SHORT	ob_gr;							/**< OBOF.EOBGR		: OB Correction Gr Pixel (11bits)					*/
	SHORT	ob_gb;							/**< OBOF.EOBGB		: OB Correction Gb Pixel (11bits)					*/
} T_IM_ELA_CTRL_OB_CORRECTION;

/** EIRch control */
typedef struct {
	ULONG	in_bayer_top_addr;				/**< EIRA		: Input bayer data top address (32bits)<br>
													Set the address that contains the LSB bits of the first pixel.<br>
													When 16-bit, please set to be an even address this.	*/
	UCHAR	in_bayer_top_offset;			/**< EIROFS		: Set LSB bit (bit number) of the beginning of the input pixel Bayer data. (3bits)<br>
													When the data type of 8/16 bit, please set the "000b" Always.<br>
													Only when the 12-bit pack data type, please select the settings for "000b" or "100b".	*/
	USHORT	in_bayer_total_hsize;			/**< EIRDEF		: Input bayer data total horizontal size (15bits, Unit 16 byte, 16~32752)		*/
	USHORT	in_bayer_hsize;					/**< EIRHSIZ	: Input bayer data horizontal size (14bits, Unit 2 pixels, 320~15360)			*/
	USHORT	in_bayer_vline;					/**< EIRVSIZ	: Input bayer data vertical line (14bits, Unit 2 pixels, 240~8960)				*/
} T_IM_ELA_CTRL_EIRCH;

/** EIWch control */
typedef struct {
	UINT32	out_bayer_top_addr[D_IM_ELA_EIWCH_ADDR_MAX];	/**< EIWA	: Output bayer data top address (32bits, Unit 64 byte)	*/
	USHORT	out_bayer_total_hsize;							/**< EIWDEF	: Output bayer data total horizontal size (14bits, Unit 64 byte, 64~16368)	*/
} T_IM_ELA_CTRL_EIWCH;

/** ENWch control */
typedef struct {
	UINT32	out_extract_noise_top_addr;		/**< ENWA	: Output extraction noise data top address (32bits, Unit 64 byte)	*/
	USHORT	out_extract_noise_total_hsize;	/**< ENWDEF	: Output extraction noise data total horizontal size(14bits, Unit 64 byte)	*/
} T_IM_ELA_CTRL_ENWCH;

/** ENREch control */
typedef struct {
	UINT32	in_extract_noise_top_addr[D_IM_ELA_ENRECH_ADDR_MAX];	/**< ENRA		: Input extraction noise data top address (32bits, Unit 64 byte)	*/
	USHORT	in_extract_noise_hsize;									/**< ENRHSIZ	: Input extraction noise data horizontal size(14bits, Unit 2 pixels)	*/
	USHORT	in_extract_noise_vline;									/**< ENRVSIZ	: Input extraction noise data vertical line(14bits, Unit 2 pixels)	*/
} T_IM_ELA_CTRL_ENRECH;

/** ENRAch/ENWMch control */
typedef struct {
	UINT32	synthetic_noise_top_addr;		/**< ENSA		: Input/Output synthetic noise data top address (32bits, Unit 64 byte)	*/
	USHORT	synthetic_noise_total_hsize;	/**< ENSDEF		: Input/Output synthetic noise data total horizontal size(14bits, Unit 64 byte)	*/
	USHORT	synthetic_noise_hsize;			/**< ENSHSIZ	: Input/Output synthetic noise data horizontal size(14bits, Unit 2 pixels)	*/
	USHORT	synthetic_noise_vline;			/**< ENSVSIZ	: Input/Output synthetic noise data vertical line(14bits, Unit 2 pixels)	*/
} T_IM_ELA_CTRL_ENRACH_ENWMCH;

/** ELA data control */
typedef struct {
	UCHAR	next_ctrl_flag;											/**< Please set "D_IM_ELA_ENABLE" when using Im_ELA_Ctrl_Next function.<br>
																	     <ul><li>@ref D_IM_ELA_DISABLE
																			 <li>@ref D_IM_ELA_ENABLE</ul>	*/
	E_IM_ELA_QELFLVL	ela_elf_level;								/**< ELF level
																     <ul><li>@ref E_IM_ELA_ELF_LEVEL_1
																		 <li>@ref E_IM_ELA_ELF_LEVEL_2
																		 <li>@ref E_IM_ELA_ELF_LEVEL_3
																		 <li>@ref E_IM_ELA_ELF_LEVEL_4
																		 <li>@ref E_IM_ELA_ELF_LEVEL_5</ul>				*/
	E_IM_ELA_QELAMD		ela_density_mode[E_IM_ELA_ELF_LEVEL_MAX];	/**< ELA density mode
																     <ul><li>@ref E_IM_ELA_DENSITY_MODE_0
																		 <li>@ref E_IM_ELA_DENSITY_MODE_1
																		 <li>@ref E_IM_ELA_DENSITY_MODE_2
																		 <li>@ref E_IM_ELA_DENSITY_MODE_3</ul>				*/
	T_IM_ELA_CTRL_COMMON		ctrl_common;				/**< ELA common control parameter								*/
	T_IM_ELA_CTRL_BYR			ctrl_byr;					/**< ELA bayer data control parameter							*/
	T_IM_ELA_CTRL_OB_CORRECTION	ob_correction;				/**< OB Correction control parameter							*/
	T_IM_ELA_CTRL_EIRCH			ctrl_eirch;					/**< EIRch control parameter									*/
	UINT32	out_bayer_addr;									/**< EIWch control<br>
																 tput bayer data top address (32bits, Unit 64 byte)			*/
	UINT32	extract_noise_addr;								/**< ENWch/ENREch control<br>
																 Input/Output extraction noise data top address (32bits, Unit 64 byte)	*/
	UINT32	noise_suppress_addr;							/**< ENRAch/ENWMch control<br>
																 Noise suppression data address (32bits, Unit 64 byte)	*/
	T_IM_ELA_CALLBACK	pCallBack;							/**< callback function	*/
} T_IM_ELA_CTRL;

/** ELA multiple times execution parameter */
typedef struct {
	T_IM_ELA_CTRL_CORE	*core[D_IM_ELA_STAGE_MAX];	/**< ELA core control parameter										*/
} T_IM_ELA_MULTIPLE_TIMES_INFO;

/** Noise data mode data */
typedef struct {
	UCHAR	noise_data_mode;							/**< NSLMD.NSLMD	: Noise data mode 
														     <ul><li>@ref D_IM_ELA_NSLMD_REDUCTION
																 <li>@ref D_IM_ELA_NSLMD_SAME</ul>	*/
	UCHAR	noise_data_precision;						/**< NSLMD.NSLRES	: Noise data precision 
														     <ul><li>@ref D_IM_ELA_NSLRES_PRECISION_1	Data precision 1 : High, Maximum amplitude : Small	
																 <li>@ref D_IM_ELA_NSLRES_PRECISION_2	Data precision 2 :  |  , Maximum amplitude :   |  	
																 <li>@ref D_IM_ELA_NSLRES_PRECISION_3	Data precision 3 :  |  , Maximum amplitude :   |  	
																 <li>@ref D_IM_ELA_NSLRES_PRECISION_4	Data precision 4 : Low , Maximum amplitude : Wide	</ul>	*/
	UCHAR	noise_data_compress_enable;					/**< NSLMD.NSLKNE	: Noise data compression enable 
														     <ul><li>@ref D_IM_ELA_DISABLE
																 <li>@ref D_IM_ELA_ENABLE</ul>	*/
} T_IM_ELA_CTRL_NSLMD;

/** EAch control */
typedef struct {
	UCHAR	cache_type_r[D_IM_ELA_EACH_DATA_MAX];		/**< EARCACHE	: Setting of read channel cache type (4bit)		*/
	UCHAR	cache_type_w[D_IM_ELA_EACH_DATA_MAX];		/**< EAWCACHE	: Setting of write channel cache type (4bit)	*/
	UCHAR	protect_type_r[D_IM_ELA_EACH_DATA_MAX];		/**< EARPROT	: Setting of read channel protect type (3bit)	*/
	UCHAR	protect_type_w[D_IM_ELA_EACH_DATA_MAX];		/**< EAWPROT	: Setting of write channel protect type (3bit)	*/
	USHORT	req_mask_r[D_IM_ELA_EACH_DATA_MAX];			/**< EARREQMSK	: Setting of read channel request mask (12bit)	*/
	USHORT	req_mask_w[D_IM_ELA_EACH_DATA_MAX];			/**< EAWREQMSK	: Setting of write channel request mask (12bit)	*/
} T_IM_ELA_CTRL_EACH;

/** EAch AXI status */
typedef struct {
	UCHAR	axi_reply_r[D_IM_ELA_EACH_DATA_MAX];		/**< EARAXSTS	: Setting of read channel cache type (2bit)		*/
	UCHAR	axi_reply_w[D_IM_ELA_EACH_DATA_MAX];		/**< EAWAXSTS	: Setting of write channel cache type (2bit)	*/
} T_IM_ELA_EACH_AXI;

/**  ELA curtail bayer data table */
typedef struct {
	UINT32	bayer_addr[D_IM_ELA_STEP_MAX];		/**< Output bayer data top address									*/
	USHORT	gl_hbyte;							/**< Number of curtail bayer data total horizontal size (byte unit)	*/
	USHORT	hsize;								/**< Number of curtail bayer data horizontal pixels					*/
	USHORT	vline;								/**< Number of curtail bayer data vertical pixels					*/
} T_IM_ELA_CURTAIL_BAYER_DATA;

/** ELA extraction noise data table */
typedef struct {
	UINT32	noise_data_addr[D_IM_ELA_STEP_MAX];	/**< Output noise data top address									*/
	USHORT	gl_hbyte;							/**< Number of curtail bayer data total horizontal size (byte unit)	*/
	USHORT	hsize;								/**< Number of curtail bayer data horizontal pixels					*/
	USHORT	vline;								/**< Number of curtail bayer data vertical pixels					*/
} T_IM_ELA_EXTRACT_NOISE_DATA;


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
Execute exclusive control for ELA macro.<br>
@param [in]	tmout	Time of timeout<br>
						<ul><li>Positive Value(Time of timeout)
							<li>@ref D_DDIM_USER_SEM_WAIT_POL
							<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval D_DDIM_OK						OK
@retval D_IM_ELA_INPUT_PARAM_ERROR		Input Parameter Error
@retval D_IM_ELA_SEM_NG					Semaphore acquisition NG
@retval D_IM_ELA_SEM_TIMEOUT_ERR		Semaphore acquisition Time Out
@remarks This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
		 This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32 Im_ELA_Open( INT32 tmout );

/**
Cancel exclusive control for ELA macro.<br>
@retval D_DDIM_OK						OK
@retval D_IM_ELA_SEM_NG					Semaphore release NG
@retval D_IM_ELA_INPUT_PARAM_ERROR		Input Parameter Error
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern	INT32 Im_ELA_Close( VOID );

/**
ELA initialize.<br>
@remarks Reset release is done by this function.<br>
*/
extern	VOID Im_ELA_Init( VOID );

/**
Get ELA executing count. <br>
@retval ELA executing count.(0~4)
@retval Parameter error(0xFF)
*/
extern	UCHAR Im_ELA_Get_Exe_Cnt( VOID );

/**
The register is set for ELA.<br>
@param [in] ctrl_ela	Setting of ELA parameters<br> See @ref T_IM_ELA_CTRL.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
extern	UINT32 Im_ELA_Ctrl( const T_IM_ELA_CTRL* const ctrl_ela );

/**
The register setting of ELA common is acquired. <br>
@param [out] ctrl_ela_common	Setting of ELA common parameters.<br> See @ref T_IM_ELA_CTRL_COMMON.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Get_Ctrl_Common( T_IM_ELA_CTRL_COMMON* const ctrl_ela_common );

/**
The register is set for the ELA macro processing since the second times. <br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
@remarks Please specify "1" for "ela_last_flg" at the last ELA macro start. <br>
*/
extern	UINT32 Im_ELA_Ctrl_Next( VOID );

/**
The register setting of EIRch is acquired. <br>
@param [out] ctrl_ela_eirch	Setting of EIRch control parameters.<br> See @ref T_IM_ELA_CTRL_EIRCH.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Get_Ctrl_Eirch( T_IM_ELA_CTRL_EIRCH* const ctrl_ela_eirch );

/**
The register setting of EIWch is acquired. <br>
@param [out] ctrl_ela_eiwch	Setting of EIWch control parameters.<br> See @ref T_IM_ELA_CTRL_EIWCH.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Get_Ctrl_Eiwch( T_IM_ELA_CTRL_EIWCH* const ctrl_ela_eiwch );

/**
The register setting of ENWch is acquired. <br>
@param [out] ctrl_ela_enwch	Setting of ENWch control parameters.<br> See @ref T_IM_ELA_CTRL_ENWCH.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Get_Ctrl_Enwch( T_IM_ELA_CTRL_ENWCH* const ctrl_ela_enwch );

/**
The register setting of ENREch is acquired. <br>
@param [out] ctrl_ela_enrech	Setting of ENREch control parameters.<br> See @ref T_IM_ELA_CTRL_ENRECH.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Get_Ctrl_Enrech( T_IM_ELA_CTRL_ENRECH* const ctrl_ela_enrech );

/**
The register setting of ENRAch/ENWMch is acquired. <br>
@param [out] ctrl_ela_enrach_enwmch	Setting of ENRAch/ENWMch control parameters.<br> See @ref T_IM_ELA_CTRL_ENRACH_ENWMCH.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Get_Ctrl_Enrach_Enwmch( T_IM_ELA_CTRL_ENRACH_ENWMCH* const ctrl_ela_enrach_enwmch );

/**
The register setting of core information is acquired. <br>
@param [out] ctrl_ela_core	Setting of cire information.<br> See @ref T_IM_ELA_CTRL_CORE.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Get_Ctrl_Core( T_IM_ELA_CTRL_CORE* const ctrl_ela_core );

/**
The register setting of OB Correction control is acquired. <br>
@param [out]	ctrl_ela_ob		pointer of OB Correction control parameters.<br> See @ref T_IM_ELA_CTRL_OB_CORRECTION.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Get_Ctrl_Ob_Correction( T_IM_ELA_CTRL_OB_CORRECTION* const ctrl_ela_ob ) ;

/**
Get Optical zero level shift parameter. <br>
@retval Optical zero level shift parameter.(Two's complement)
@retval Parameter error(0xFF)
*/
extern	USHORT Im_ELA_Get_Optical_Zero_Level_Shift( VOID );

/**
Set address of parameter for multiple times execution.<br>
@param [in]	multiple_exec_info		pointer of multiple times execution information data address.<br> See @ref T_IM_ELA_MULTIPLE_TIMES_INFO.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
@remarks	This API uses DDIM_User_Clr_Flg().
*/
extern	UINT32 Im_ELA_Set_Multiple_Exec_Info( const T_IM_ELA_MULTIPLE_TIMES_INFO* const multiple_exec_info );

/**
ELA is executed by synchronous processing.<br>
@retval	D_DDIM_OK			Success
@retval D_IM_ELA_TIMEOUT	Fail - Time-out
@retval	D_IM_ELA_MACRO_BUSY	Fail - Macro busy
@retval	D_IM_ELA_NG			Fail - Processing NG (system error)
@remarks	This API uses DDIM_User_Dly_Tsk().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern	INT32 Im_ELA_Start_Sync( VOID );

/**
ELA is executed by asynchronization processing.<br>
@retval	D_DDIM_OK				Success
@retval	D_IM_ELA_MACRO_BUSY		Fail - Macro busy
@remarks When the processing of ELA is completed, the callback function is started.<br>
         Please wait for the completion of processing with @ref Im_ELA_Wait_End() when you do not use the callback function. 
@remarks	This API uses DDIM_User_Dly_Tsk().
@remarks	This API uses DDIM_User_Clr_Flg().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern  INT32    Im_ELA_Start_Async( VOID );

/**
The processing of ELA stop. <br>
@remarks	This API uses DDIM_User_Clr_Flg().
*/
extern  VOID    Im_ELA_Stop( VOID );

/**
Wait the end of the asynchronization processing started with Im_ELA_Start_Async().
@retval	D_DDIM_OK			Success
@retval D_IM_ELA_TIMEOUT	Fail - Time-out
@retval	D_IM_ELA_NG			Fail - Processing NG (system error)
@remarks Please receive the end of the asynchronization processing by the callback function when you do not wait for the end of the asynchronization processing by this function.
@remarks	This API uses DDIM_User_Clr_Flg().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern  INT32   Im_ELA_Wait_End( VOID );

/**
Interruption handler of ELA that starts when the processing of ELA ends. <br>
@remarks This API uses DDIM_User_Set_Flg().
*/
extern	VOID Im_ELA_Int_Handler(VOID);

/**
Cancel the LUT sleep for Deknee.
@param [in]		deknee_enable	: Setting the LUT sleep release for Deknee
						<ul><li>@ref D_IM_ELA_DISABLE
							<li>@ref D_IM_ELA_ENABLE</ul>
@remarks		When setting the LUT for the Deknee from the CPU, and when the process is performed Deknee, please set to "D_IM_ELA_ENABLE".<br>
				If no processing is performed Deknee, you will be able to reduce power consumption by the LUT for Deknee in that you will be set to "D_IM_ELA_DISABLE".
*/
extern	VOID Im_ELA_Set_Deknee_Lut_Sleep( UCHAR deknee_enable );

/**
The register setting of NSLMD is acquired. <br>
@param [out]	nslmd		pointer of NSLMD parameters.<br> See @ref T_IM_ELA_CTRL_NSLMD.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Get_Nslmd( T_IM_ELA_CTRL_NSLMD* const nslmd ) ;

/**
The register is set for EAch.<br>
@param [in] ctrl_each	Setting of EAch parameters<br> See @ref T_IM_ELA_CTRL_EACH.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Set_EAch( const T_IM_ELA_CTRL_EACH* const ctrl_each );

/**
The register setting of EAch is acquired. <br>
@param [out]	ctrl_each	pointer of EAch parameters<br> See @ref T_IM_ELA_CTRL_EACH.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Get_EAch( T_IM_ELA_CTRL_EACH* const ctrl_each );

/**
The register setting of AXI status is acquired. <br>
@param [out]	each_axi	pointer of AXI status parameters<br> See @ref T_IM_ELA_EACH_AXI.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Get_AXI_Status( T_IM_ELA_EACH_AXI* const each_axi );


/**
Set Deknee table
@param [in]	src_tbl			Pointer of Table data (Maximum array number is D_IM_ELA_DEKNEE_TBL_MAX)
@param [in]	write_ofs_num	write offset number of table constituent(data number, must set multiple of 2).<br>
							value range: [write_ofs_num + array_num <= D_IM_ELA_DEKNEE_TBL_MAX ]
@param [in]	array_num		num of table constituent(data number, must set multiple of 2).<br>
							value range: [write_ofs_num + array_num <= D_IM_ELA_DEKNEE_TBL_MAX ]
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_MACRO_BUSY				Fail - Macro busy
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
*/
extern	UINT32 Im_ELA_Set_Deknee_Tbl( const USHORT* const src_tbl, USHORT write_ofs_num, USHORT array_num );



#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/**
Set curtail bayer data table
@param [in]	curtail_bayerl	Pointer of Table data<br> See @ref T_IM_ELA_CURTAIL_BAYER_DATA.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
@remarks 
sample code<br>
@code
	UINT32 i, j;
	T_IM_ELA_CURTAIL_BAYER_DATA curtail_bayer[D_IM_ELA_STAGE_MAX];

	for(i = 0; i < D_IM_ELA_STAGE_MAX; i++){
		for(j = 0; j < D_IM_ELA_STEP_MAX; j++){
			curtail_bayer[i].bayer_addr[j] = 0x10000000 + (0x100 * i) + j;
		}
		curtail_bayer[i].gl_hbyte = i + 100;
		curtail_bayer[i].hsize    = i + 200;
		curtail_bayer[i].vline    = i + 300;
	}

	Im_ELA_Set_Curtail_Bayer_Data_Tbl(0, curtail_bayer );
@endcode
*/
extern	UINT32 Im_ELA_Set_Curtail_Bayer_Data_Tbl( const T_IM_ELA_CURTAIL_BAYER_DATA curtail_bayerl[D_IM_ELA_STAGE_MAX] );

/**
Get curtail bayer data table
@param [out]	curtail_bayerl	Pointer of Table data<br> See @ref T_IM_ELA_CURTAIL_BAYER_DATA.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
@remarks 
sample code<br>
@code
	UINT32 i, j;
	T_IM_ELA_CURTAIL_BAYER_DATA curtail_bayer[D_IM_ELA_STAGE_MAX];

	Im_ELA_Get_Curtail_Bayer_Data_Tbl(0, curtail_bayer);

	for(i = 0; i < D_IM_ELA_STAGE_MAX; i++){
		printf("(((Stage%d)))\n", i + 1);
		printf("gl_hbyte = %d\n",	curtail_bayer[i].gl_hbyte);
		printf("hsize = %d\n",	curtail_bayer[i].hsize);
		printf("vline = %d\n",	curtail_bayer[i].vline);
		for(j = 0; j < D_IM_ELA_STEP_MAX; j++){
			printf("(Sterp%02d)bayer_addr = 0x%08X\n", j, curtail_bayer[i].bayer_addr[j]);
		}
	}
@endcode
*/
extern	UINT32 Im_ELA_Get_Curtail_Bayer_Data_Tbl( T_IM_ELA_CURTAIL_BAYER_DATA curtail_bayerl[D_IM_ELA_STAGE_MAX] );

/**
Set extraction noise data table
@param [in]	extract_noise	Pointer of Table data<br> See @ref T_IM_ELA_EXTRACT_NOISE_DATA.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
@remarks 
sample code<br>
@code
	UINT32 i, j;
	UINT32 result;
	T_IM_ELA_EXTRACT_NOISE_DATA extract_noise[D_IM_ELA_STAGE_MAX];

	for(i = 0; i < D_IM_ELA_STAGE_MAX; i++){
		for(j = 0; j < D_IM_ELA_STEP_MAX; j++){
			extract_noise[i].noise_data_addr[j] = 0x20000000 + (0x100 * i) + j;
		}
		extract_noise[i].gl_hbyte = i + 100;
		extract_noise[i].hsize    = i + 200;
		extract_noise[i].vline    = i + 300;
	}

	Im_ELA_Set_Extract_Noise_Data_Tbl(0, extract_noise );
@endcode
*/
extern	UINT32 Im_ELA_Set_Extract_Noise_Data_Tbl( const T_IM_ELA_EXTRACT_NOISE_DATA extract_noise[D_IM_ELA_STAGE_MAX] );

/**
Get extraction noise data table
@param [out]	extract_noise	Pointer of Table data<br> See @ref T_IM_ELA_EXTRACT_NOISE_DATA.<br>
@retval	D_DDIM_OK						Success
@retval	D_IM_ELA_INPUT_PARAM_ERROR		Fail - Parameter error
@remarks 
sample code<br>
@code
	UINT32 i, j;
	T_IM_ELA_EXTRACT_NOISE_DATA extract_noise[D_IM_ELA_STAGE_MAX];

	Im_ELA_Get_Extract_Noise_Data_Tbl(extract_noise);

	for(i = 0; i < D_IM_ELA_STAGE_MAX; i++){
		printf("(((Stage%d)))\n", i + 1);
		printf("gl_hbyte = %d\n",	extract_noise[i].gl_hbyte);
		printf("hsize = %d\n",	extract_noise[i].hsize);
		printf("vline = %d\n",	extract_noise[i].vline);
		for(j = 0; j < D_IM_ELA_STEP_MAX; j++){
			printf("(Sterp%02d)noise_data_addr = 0x%08X\n", j, extract_noise[i].noise_data_addr[j]);
		}
	}
@endcode
*/
extern	UINT32 Im_ELA_Get_Extract_Noise_Data_Tbl( T_IM_ELA_EXTRACT_NOISE_DATA extract_noise[D_IM_ELA_STAGE_MAX] );

//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IM_ELA_H_

/*@}*/
/**
@weakgroup im_ela
@{

@section im_ela_sample_section1	Sample of ELA
The example of executing synchronous processing is as follows.<br>
@code

    INT32 result;
    int cnt;
    T_IM_ELA_CTRL ctrl_ela;
    T_IM_ELA_MULTIPLE_TIMES_INFO ela_ctrl_multiple_exec;

    memset(&ela_ctrl_multiple_exec, 0, sizeof(T_IM_ELA_MULTIPLE_TIMES_INFO)); // Set the noise data obtained from IQ is in practice.

    ctrl_ela.next_ctrl_flag = D_IM_ELA_DISABLE;

    ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
    ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
    ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
    ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;
    ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;
    ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_3;

    ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
    ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
    ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;
    ctrl_ela.ctrl_common.bayer_start_pixel_type     = D_IM_ELA_ORG_R;

    ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
    ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
    ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
    ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;
    ctrl_ela.ctrl_byr.deknee_enable       = D_IM_ELA_ENABLE;
    ctrl_ela.ctrl_byr.ob_zsf_enable       = D_IM_ELA_ENABLE;

    //OB Correction control
    ctrl_ela.ob_correction.ob_rr = 0;
    ctrl_ela.ob_correction.ob_bb = 0;
    ctrl_ela.ob_correction.ob_gr = 0;
    ctrl_ela.ob_correction.ob_gb = 0;

    ctrl_ela.ctrl_eirch.in_bayer_top_addr = 0x2000000;
    ctrl_ela.ctrl_eirch.in_bayer_top_offset = 0;
    ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6912;
    ctrl_ela.ctrl_eirch.in_bayer_hsize = 4608;
    ctrl_ela.ctrl_eirch.in_bayer_vline = 3456;

    ctrl_ela.out_bayer_addr     = 0x3000000;
    ctrl_ela.extract_noise_addr = 0x4000000;

    ctrl_ela.pCallBack = NULL;

    // Setting of ELA control parameter
    result = Im_ELA_Ctrl(&ctrl_ela);
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }

    result = Im_ELA_Set_Multiple_Exec_Info(&ela_ctrl_multiple_exec);
    if( result != D_DDIM_OK ) {
        // Error processing
        return;
    }

    // Processing is started synchronously
    result = Im_ELA_Start_Sync();
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }

@endcode
<br><br>

@section im_ela_sample_section2	Sample of ELA using Im_ELA_Ctrl_Next function
The example of executing synchronous processing is as follows.<br>
@code

    INT32 result;
    int cnt;
    T_IM_ELA_CTRL ctrl_ela;
    T_IM_ELA_MULTIPLE_TIMES_INFO ela_ctrl_multiple_exec;

    memset(&ela_ctrl_multiple_exec, 0, sizeof(T_IM_ELA_MULTIPLE_TIMES_INFO)); // Set the noise data obtained from IQ is in practice.

    ctrl_ela.next_ctrl_flag = D_IM_ELA_ENABLE;

    ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
    ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
    ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
    ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;
    ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;
    ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_3;

    ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
    ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
    ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;
    ctrl_ela.ctrl_common.bayer_start_pixel_type     = D_IM_ELA_ORG_R;

    ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
    ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
    ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
    ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;
    ctrl_ela.ctrl_byr.deknee_enable       = D_IM_ELA_ENABLE;
    ctrl_ela.ctrl_byr.ob_zsf_enable       = D_IM_ELA_ENABLE;

    //OB Correction control
    ctrl_ela.ob_correction.ob_rr = 0;
    ctrl_ela.ob_correction.ob_bb = 0;
    ctrl_ela.ob_correction.ob_gr = 0;
    ctrl_ela.ob_correction.ob_gb = 0;

    ctrl_ela.ctrl_eirch.in_bayer_top_addr = 0x2000000;
    ctrl_ela.ctrl_eirch.in_bayer_top_offset = 0;
    ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6912;
    ctrl_ela.ctrl_eirch.in_bayer_hsize = 4608;
    ctrl_ela.ctrl_eirch.in_bayer_vline = 3456;

    ctrl_ela.out_bayer_addr     = 0x3000000;
    ctrl_ela.extract_noise_addr = 0x4000000;

	ctrl_ela.pCallBack = NULL;

    // Setting of ELA control parameter
    result = Im_ELA_Ctrl(&ctrl_ela);
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }

    result = Im_ELA_Set_Multiple_Exec_Info(&ela_ctrl_multiple_exec);
    if( result != D_DDIM_OK ) {
        // Error processing
        return;
    }

    // Processing is started synchronously
    result = Im_ELA_Start_Sync();
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }


    // Setting of ELA control parameter
    // ELA macro processing of the second times
    result = Im_ELA_Ctrl_Next();
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }

    // Processing is started synchronously
    result = Im_ELA_Start_Sync();
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }

    // Setting of ELA control parameter
    // The last ELA macro processing
    result = Im_ELA_Ctrl_Next();
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }

    // Processing is started synchronously
    result = Im_ELA_Start_Sync();
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }

    // Setting of ELA control parameter
    // The last ELA macro processing
    result = Im_ELA_Ctrl_Next();
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }

    // Processing is started synchronously
    result = Im_ELA_Start_Sync();
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }

    // Setting of ELA control parameter
    // The last ELA macro processing
    result = Im_ELA_Ctrl_Next();
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }

    // Processing is started synchronously
    result = Im_ELA_Start_Sync();
    if(result != D_DDIM_OK) {
        // Error processing
        return;
    }

@endcode

*/


/*@}*/

