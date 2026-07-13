/**
 * @file		fj_top.h
 * @brief		LSI top control api header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_TOP_H
#define _FJ_TOP_H

#include "ddim_typedef.h"
#include "fj_std.h"

/** @weakgroup fj_top_overview
@{
	The following LSI top controls can be achieved by using these APIs.<br>
	Refer to clock and reset control section on LSI Specification document.<br>
	- Function <br>
	. Clock frequency selection of hardware macro.<br>
	. Clock output control of AHIP macro and DSP macro.<br>
	. Individual reset generation and Reset factor monitor of DSP macro.<br>
	. ASV(Adaptive Supply Voltage) code acquisition.
@}*/	// fj_top_overview

/** @weakgroup fj_top_definition
@{*/
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Clock name */
typedef enum {
	FJ_TOP_CLOCK_NAME_DSPCK,				/**< DSP Clock */
	FJ_TOP_CLOCK_NAME_DSPAX,				/**< DSP AXI Bus Clock */
	FJ_TOP_CLOCK_NAME_MAX
} FJ_TOP_CLOCK_NAME;

/** EMMC Clock Select */
typedef enum {
	FJ_TOP_EMMC_CLKSEL_187MHZ = 0,			/**< 187.5MHz	(=1500MHz/8)  	*/
	FJ_TOP_EMMC_CLKSEL_166MHZ,				/**< 166.666MHz	(=1500MHz/9)	*/
	FJ_TOP_EMMC_CLKSEL_150MHZ,				/**< 150MHz		(=1500MHz/10)  	*/
	FJ_TOP_EMMC_CLKSEL_100MHZ,				/**< 100MHz		(=1500MHz/15) 	*/
	FJ_TOP_EMMC_CLKSEL_MAX
}	FJ_TOP_EMMC_CLKSEL;

/** Nand Flash I/F Clock Select */
typedef enum {
	FJ_TOP_NF_CLKSEL_100MHZ = 0,			/**< 100MHz		(=1600MHz/16) 	*/
	FJ_TOP_NF_CLKSEL_80MHZ,					/**< 80MHz		(=1600MHz/20) 	*/
	FJ_TOP_NF_CLKSEL_61MHZ,					/**< 61.538MHz	(=1600MHz/26)	*/
	FJ_TOP_NF_CLKSEL_50MHZ,					/**< 50MHz		(=1600MHz/32) 	*/
	FJ_TOP_NF_CLKSEL_20MHZ,					/**< 20MHz		(=1600MHz/80) 	*/
	FJ_TOP_NF_CLKSEL_32MHZ = 8,				/**< 32MHz		(=1600MHz/50) 	*/
	FJ_TOP_NF_CLKSEL_MAX
}	FJ_TOP_NF_CLKSEL;

/** SD Card I/F ch2 UHS-II Clock Select */
typedef enum {
	FJ_TOP_SD2U2_CLKSEL_51MHZ = 0,			/**< 51.851MHz (=1400MHz/27)	*/
	FJ_TOP_SD2U2_CLKSEL_46MHZ,				/**< 46.666MHz (=1400MHz/30)	*/
	FJ_TOP_SD2U2_CLKSEL_42MHZ,				/**< 42.424MHz (=1400MHz/33) 	*/
	FJ_TOP_SD2U2_CLKSEL_38MHZ,				/**< 38.888MHz (=1400MHz/36) 	*/
	FJ_TOP_SD2U2_CLKSEL_35MHZ,				/**< 35.897MHz (=1400MHz/39) 	*/
	FJ_TOP_SD2U2_CLKSEL_33MHZ,				/**< 33.333MHz (=1400MHz/42) 	*/
	FJ_TOP_SD2U2_CLKSEL_29MHZ,				/**< 29.166MHz (=1400MHz/48) 	*/
	FJ_TOP_SD2U2_CLKSEL_25MHZ,				/**< 25.825MHz (=1400MHz/54) 	*/
	FJ_TOP_SD2U2_CLKSEL_MAX
}	FJ_TOP_SD2U2_CLKSEL;

/** SD Card I/F ch2 UHS-I Clock Select */
typedef enum {
	FJ_TOP_SD2U1_CLKSEL_400MHZ = 2,			/**< 400MHz 	(=1600MHz/4) 	*/
	FJ_TOP_SD2U1_CLKSEL_200MHZ,				/**< 200MHz 	(=1600MHz/8) 	*/
	FJ_TOP_SD2U1_CLKSEL_100MHZ,				/**< 100MHz 	(=1600MHz/16) 	*/
	FJ_TOP_SD2U1_CLKSEL_700MHZ = 8,			/**< 700MHz 	(=1400MHz/2) 	*/
	FJ_TOP_SD2U1_CLKSEL_MAX
}	FJ_TOP_SD2U1_CLKSEL;

/** SD Card I/F ch3 UHS-I Clock Select */
typedef enum {
	FJ_TOP_SD3_CLKSEL_200MHZ = 3,			/**< 200MHz 	(=1600MHz/8) 	*/
	FJ_TOP_SD3_CLKSEL_100MHZ,				/**< 100MHz 	(=1600MHz/16) 	*/
	FJ_TOP_SD3_CLKSEL_700MHZ = 8,			/**< 700MHz 	(=1400MHz/2) 	*/
	FJ_TOP_SD3_CLKSEL_MAX
}	FJ_TOP_SD3_CLKSEL;

/** SD Card I/F ch1 SDIO Clock Select */
typedef enum {
	FJ_TOP_SD1_CLKSEL_800MHZ = 0,			/**< 800MHz 	(=1600MHz/2) 	*/
	FJ_TOP_SD1_CLKSEL_533MHZ,				/**< 533.333MHz (=1600MHz/3)	*/
	FJ_TOP_SD1_CLKSEL_400MHZ,				/**< 400MHz 	(=1600MHz/4) 	*/
	FJ_TOP_SD1_CLKSEL_200MHZ,				/**< 200MHz 	(=1600MHz/8) 	*/
	FJ_TOP_SD1_CLKSEL_100MHZ,				/**< 100MHz 	(=1600MHz/16) 	*/
	FJ_TOP_SD1_CLKSEL_700MHZ,				/**< 700MHz 	(=1400MHz/2) 	*/
	FJ_TOP_SD1_CLKSEL_MAX
}	FJ_TOP_SD1_CLKSEL;

/** RAW Clock Select */
typedef enum {
	FJ_TOP_RAW_CLKSEL_200MHZ = 0,			/**< 200MHz (=1600MHz/8) 	*/
	FJ_TOP_RAW_CLKSEL_160MHZ,				/**< 160MHz (=1600MHz/10)	*/
	FJ_TOP_RAW_CLKSEL_100MHZ,				/**< 100MHz (=1600MHz/16) 	*/
	FJ_TOP_RAW_CLKSEL_50MHZ,				/**< 50MHz 	(=1600MHz/32) 	*/
	FJ_TOP_RAW_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_RAW_CLKSEL_MAX
}	FJ_TOP_RAW_CLKSEL;

/** HIF Clock Select */
typedef enum {
	FJ_TOP_HIF_CLKSEL_594MHZ = 0,			/**< 594MHz 	(=1188MHz/2)	*/
	FJ_TOP_HIF_CLKSEL_297MHZ,				/**< 297MHz 	(=1188MHz/4)    */
	FJ_TOP_HIF_CLKSEL_148MHZ,				/**< 148.5MHz 	(=1188MHz/8)  	*/
	FJ_TOP_HIF_CLKSEL_74MHZ,				/**< 74.25MHz 	(=1188MHz/16) 	*/
	FJ_TOP_HIF_CLKSEL_27MHZ,				/**< 27MHz 		(=1188MHz/44)   */
	FJ_TOP_HIF_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_HIF_CLKSEL_MAX
} FJ_TOP_HIF_CLKSEL;

/** MIF Clock Select */
typedef enum {
	FJ_TOP_MIF_CLKSEL_169MHZ = 0,			/**< 169.714MHz (=1188MHz/7)	*/
	FJ_TOP_MIF_CLKSEL_148MHZ,				/**< 148.5MHz   (=1188MHz/8)  	*/
	FJ_TOP_MIF_CLKSEL_108MHZ,				/**< 108MHz 	(=1188MHz/11)  	*/
	FJ_TOP_MIF_CLKSEL_74MHZ,				/**< 74.25MHz 	(=1188MHz/16) 	*/
	FJ_TOP_MIF_CLKSEL_66MHZ,				/**< 66MHz 		(=1188MHz/18)   */
	FJ_TOP_MIF_CLKSEL_54MHZ,				/**< 54MHz 		(=1188MHz/22)   */
	FJ_TOP_MIF_CLKSEL_49MHZ,				/**< 49.5MHz 	(=1188MHz/24)  	*/
	FJ_TOP_MIF_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_MIF_CLKSEL_MAX
} FJ_TOP_MIF_CLKSEL;

/** IPU Clock Select */
typedef enum {
	FJ_TOP_IPU_CLKSEL_400MHZ = 0,			/**< 400MHz (=1600MHz/4/(8/8))	*/
	FJ_TOP_IPU_CLKSEL_50MHZ,				/**< 50MHz  (=1600MHz/4/(1/8))  */
	FJ_TOP_IPU_CLKSEL_100MHZ,				/**< 100MHz (=1600MHz/4/(2/8))  */
	FJ_TOP_IPU_CLKSEL_150MHZ,				/**< 150MHz (=1600MHz/4/(3/8))  */
	FJ_TOP_IPU_CLKSEL_200MHZ,				/**< 200MHz (=1600MHz/4/(4/8))  */
	FJ_TOP_IPU_CLKSEL_250MHZ,				/**< 250MHz (=1600MHz/4/(5/8))  */
	FJ_TOP_IPU_CLKSEL_300MHZ,				/**< 300MHz (=1600MHz/4/(6/8))  */
	FJ_TOP_IPU_CLKSEL_350MHZ,				/**< 350MHz (=1600MHz/4/(7/8))  */
	FJ_TOP_IPU_CLKSEL_OFF,					/**<  OFF */
	FJ_TOP_IPU_CLKSEL_MAX
} FJ_TOP_IPU_CLKSEL;

/** IPU TME Clock Select */
typedef enum {
	FJ_TOP_IPU_TME_CLKSEL_500MHZ = 0,		/**< 500MHz  (=1500MHz/3/(8/8))	*/
	FJ_TOP_IPU_TME_CLKSEL_62MHZ,			/**< 62.5MHz (=1500MHz/3/(1/8)) */
	FJ_TOP_IPU_TME_CLKSEL_125MHZ,			/**< 125MHz  (=1500MHz/3/(2/8)) */
	FJ_TOP_IPU_TME_CLKSEL_187MHZ,			/**< 187.5MHz(=1500MHz/3/(3/8)) */
	FJ_TOP_IPU_TME_CLKSEL_250MHZ,			/**< 250MHz  (=1500MHz/3/(4/8)) */
	FJ_TOP_IPU_TME_CLKSEL_312MHZ,			/**< 312.5MHz(=1500MHz/3/(5/8)) */
	FJ_TOP_IPU_TME_CLKSEL_375MHZ,			/**< 375MHz  (=1500MHz/3/(6/8)) */
	FJ_TOP_IPU_TME_CLKSEL_437MHZ,			/**< 437.5MHz(=1500MHz/3/(7/8)) */
	FJ_TOP_IPU_TME_CLKSEL_OFF,				/**< OFF */
	FJ_TOP_IPU_TME_CLKSEL_MAX
} FJ_TOP_IPU_TME_CLKSEL;

/** GPU Clock Select */
typedef enum {
	FJ_TOP_GPU_CLKSEL_466MHZ = 0,			/**< 466.666MHz (=1400MHz/3/(8/8))	*/
	FJ_TOP_GPU_CLKSEL_58MHZ,				/**< 58.333MHz  (=1400MHz/3/(1/8)) 	*/
	FJ_TOP_GPU_CLKSEL_116MHZ,				/**< 116.666MHz (=1400MHz/3/(2/8))  */
	FJ_TOP_GPU_CLKSEL_175MHZ,				/**< 175MHz     (=1400MHz/3/(3/8))	*/
	FJ_TOP_GPU_CLKSEL_233MHZ,				/**< 233.333MHz (=1400MHz/3/(4/8))  */
	FJ_TOP_GPU_CLKSEL_291MHZ,				/**< 291.666MHz (=1400MHz/3/(5/8))	*/
	FJ_TOP_GPU_CLKSEL_350MHZ,				/**< 350MHz     (=1400MHz/3/(6/8))  */
	FJ_TOP_GPU_CLKSEL_408MHZ,				/**< 408.333MHz (=1400MHz/3/(7/8))	*/
	FJ_TOP_GPU_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_GPU_CLKSEL_MAX
} FJ_TOP_GPU_CLKSEL;

/** JPEG Clock Select */
typedef enum {
	FJ_TOP_JPG_CLKSEL_200MHZ = 0,			/**< 200MHz (=1600MHz/8/(8/8)) */
	FJ_TOP_JPG_CLKSEL_25MHZ,				/**<  25MHz (=1600MHz/8/(1/8)) */
	FJ_TOP_JPG_CLKSEL_50MHZ,				/**<  50MHz (=1600MHz/8/(2/8)) */
	FJ_TOP_JPG_CLKSEL_75MHZ,				/**<  75MHz (=1600MHz/8/(3/8)) */
	FJ_TOP_JPG_CLKSEL_100MHZ,				/**< 100MHz (=1600MHz/8/(4/8)) */
	FJ_TOP_JPG_CLKSEL_125MHZ,				/**< 125MHz (=1600MHz/8/(5/8)) */
	FJ_TOP_JPG_CLKSEL_150MHZ,				/**< 150MHz (=1600MHz/8/(6/8)) */
	FJ_TOP_JPG_CLKSEL_175MHZ,				/**< 175MHz (=1600MHz/8/(7/8)) */
	FJ_TOP_JPG_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_JPG_CLKSEL_MAX
} FJ_TOP_JPG_CLKSEL;

/** ELA Clock Select */
typedef enum {
	FJ_TOP_ELA_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4)	*/
	FJ_TOP_ELA_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6)	*/
	FJ_TOP_ELA_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8)	*/
	FJ_TOP_ELA_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)	*/
	FJ_TOP_ELA_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16)	*/
	FJ_TOP_ELA_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20)	*/
	FJ_TOP_ELA_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24)	*/
	FJ_TOP_ELA_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_ELA_CLKSEL_MAX
} FJ_TOP_ELA_CLKSEL;

/** PIPE Select */
typedef enum {
	FJ_TOP_PIPE_CLKSEL_PLL02 = 0,			/**< Use PLL02 (300MHz`50MHz)			*/
	FJ_TOP_PIPE_CLKSEL_PLL01,				/**< Use PLL01 (266.666MHz`66.666MHz)	*/
	FJ_TOP_PIPE_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_PIPE_CLKSEL_MAX
} FJ_TOP_PIPE_CLKSEL;

/** SENSOR Clock Select */
typedef enum {
	FJ_TOP_SEN_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4)	*/
	FJ_TOP_SEN_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6)	*/
	FJ_TOP_SEN_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8)	*/
	FJ_TOP_SEN_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)	*/
	FJ_TOP_SEN_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16)	*/
	FJ_TOP_SEN_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20)	*/
	FJ_TOP_SEN_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24)	*/
	FJ_TOP_SEN_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_SEN_CLKSEL_MAX
} FJ_TOP_SEN_CLKSEL;

/** SENSOR Clock Select */
typedef enum {
	FJ_TOP_SENMSK_CLKSEL_100MHZ = 0,		/**< 100MHz (=1600MHz/16)	*/
	FJ_TOP_SENMSK_CLKSEL_80MHZ,				/**< 80MHz (=1600MHz/20)	*/
	FJ_TOP_SENMSK_CLKSEL_OFF,				/**< OFF */
	FJ_TOP_SENMSK_CLKSEL_MAX
} FJ_TOP_SENMSK_CLKSEL;

/** IIP Clock Select */
typedef enum {
	FJ_TOP_IIP_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 	*/
	FJ_TOP_IIP_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 	*/
	FJ_TOP_IIP_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 	*/
	FJ_TOP_IIP_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)	*/
	FJ_TOP_IIP_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 	*/
	FJ_TOP_IIP_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 	*/
	FJ_TOP_IIP_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 	*/
	FJ_TOP_IIP_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_IIP_CLKSEL_MAX
} FJ_TOP_IIP_CLKSEL;

/** STAT Clock Select */
typedef enum {
	FJ_TOP_PAS_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 	*/
	FJ_TOP_PAS_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 	*/
	FJ_TOP_PAS_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 	*/
	FJ_TOP_PAS_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)	*/
	FJ_TOP_PAS_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 	*/
	FJ_TOP_PAS_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 	*/
	FJ_TOP_PAS_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 	*/
	FJ_TOP_PAS_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_PAS_CLKSEL_MAX
} FJ_TOP_PAS_CLKSEL;

/** IPP Clock Select */
typedef enum {
	FJ_TOP_IPP_CLKSEL_800MHZ = 0,			/**< 800MHz 	(=1600MHz/2)	*/
	FJ_TOP_IPP_CLKSEL_533MHZ = 1,			/**< 533.333MHz (=1600MHz/3)	*/
	FJ_TOP_IPP_CLKSEL_400MHZ,				/**< 400MHz 	(=1600MHz/4) 	*/
	FJ_TOP_IPP_CLKSEL_200MHZ,				/**< 200MHz 	(=1600MHz/8) 	*/
	FJ_TOP_IPP_CLKSEL_700MHZ,				/**< 700MHz 	(=1400MHz/2)	*/
	FJ_TOP_IPP_CLKSEL_600MHZ = 8,			/**< 600MHz 	(=1200MHz/2) 	*/
	FJ_TOP_IPP_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_IPP_CLKSEL_MAX
} FJ_TOP_IPP_CLKSEL;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/** PXF Clock Select */
typedef enum {
	FJ_TOP_PXF_CLKSEL_700MHZ = 0,			/**< 700MHz 	(=1400MHz/2)	*/
	FJ_TOP_PXF_CLKSEL_280MHZ,				/**< 280MHz 	(=1400MHz/5) 	*/
	FJ_TOP_PXF_CLKSEL_155MHZ,				/**< 155.555MHz (=1400MHz/9)	*/
	FJ_TOP_PXF_CLKSEL_533MHZ = 4,			/**< 533.333MHz (=1600MHz/3)	*/
	FJ_TOP_PXF_CLKSEL_400MHZ,				/**< 400MHz 	(=1600MHz/4) 	*/
	FJ_TOP_PXF_CLKSEL_100MHZ,				/**< 100MHz 	(=1600MHz/16) 	*/
	FJ_TOP_PXF_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_PXF_CLKSEL_MAX
} FJ_TOP_PXF_CLKSEL;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/** HEVENC Clock Select */
typedef enum {
	FJ_TOP_HEVENC_CLKSEL_466MHZ = 0,		/**< 466.666MHz (=1400MHz/3)	*/
	FJ_TOP_HEVENC_CLKSEL_350MHZ,			/**< 350MHz 	(=1400MHz/4) 	*/
	FJ_TOP_HEVENC_CLKSEL_280MHZ,			/**< 280MHz 	(=1400MHz/5)	*/
	FJ_TOP_HEVENC_CLKSEL_200MHZ = 4,		/**< 200MHz 	(=1600MHz/7)	*/
	FJ_TOP_HEVENC_CLKSEL_100MHZ,			/**< 100MHz 	(=1600MHz/14) 	*/
	FJ_TOP_HEVENC_CLKSEL_OFF,				/**< OFF */
	FJ_TOP_HEVENC_CLKSEL_MAX
} FJ_TOP_HEVENC_CLKSEL;

/** HEVENCSEL Select */
typedef enum {
	FJ_TOP_HEVSEL_CLKSEL_500MHZ = 0,		/**< 500MHz (=1500MHz/3)				*/
	FJ_TOP_HEVSEL_CLKSEL_HEVENCCLK,			/**< Enable the setting of HEVENCCLK	*/
	FJ_TOP_HEVSEL_CLKSEL_OFF,				/**< OFF */
	FJ_TOP_HEVSEL_CLKSEL_MAX
} FJ_TOP_HEVSEL_CLKSEL;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/** VDF Clock Select */
typedef enum {
	FJ_TOP_VDF_CLKSEL_700MHZ = 0,			/**< 700MHz 	(=1400MHz/2)	*/
	FJ_TOP_VDF_CLKSEL_280MHZ,				/**< 280MHz 	(=1400MHz/5) 	*/
	FJ_TOP_VDF_CLKSEL_155MHZ,				/**< 155.555MHz (=1400MHz/9)	*/
	FJ_TOP_VDF_CLKSEL_533MHZ = 4,			/**< 533.333MHz (=1600MHz/3)	*/
	FJ_TOP_VDF_CLKSEL_400MHZ,				/**< 400MHz 	(=1600MHz/4) 	*/
	FJ_TOP_VDF_CLKSEL_100MHZ,				/**< 100MHz 	(=1600MHz/16) 	*/
	FJ_TOP_VDF_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_VDF_CLKSEL_MAX
} FJ_TOP_VDF_CLKSEL;

/** Sensor (SRO1/SR02/SRO1_2/SRO2_2) Clock Select */
typedef enum {
	FJ_TOP_SRO_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 	*/
	FJ_TOP_SRO_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 	*/
	FJ_TOP_SRO_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 	*/
	FJ_TOP_SRO_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)	*/
	FJ_TOP_SRO_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 	*/
	FJ_TOP_SRO_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 	*/
	FJ_TOP_SRO_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 	*/
	FJ_TOP_SRO_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_SRO_CLKSEL_MAX
} FJ_TOP_SRO_CLKSEL;

/** R2Y1/R2Y2 Clock Select */
typedef enum {
	FJ_TOP_R2Y_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 	*/
	FJ_TOP_R2Y_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 	*/
	FJ_TOP_R2Y_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 	*/
	FJ_TOP_R2Y_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)	*/
	FJ_TOP_R2Y_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 	*/
	FJ_TOP_R2Y_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 	*/
	FJ_TOP_R2Y_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 	*/
	FJ_TOP_R2Y_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_R2Y_CLKSEL_MAX
} FJ_TOP_R2Y_CLKSEL;

/** LTM1 Clock Select */
typedef enum {
	FJ_TOP_LTM1_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 			*/
	FJ_TOP_LTM1_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 			*/
	FJ_TOP_LTM1_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 			*/
	FJ_TOP_LTM1_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)			*/
	FJ_TOP_LTM1_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 			*/
	FJ_TOP_LTM1_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 			*/
	FJ_TOP_LTM1_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 			*/
	FJ_TOP_LTM1_CLKSEL_SRO1CLK = 8,			/**< the frequency set by SRO1CLK	*/
	FJ_TOP_LTM1_CLKSEL_SRO1CLK_2 = 16,		/**< the frequency set by SRO1CLK_2	*/
	FJ_TOP_LTM1_CLKSEL_B2B1CLK = 24,		/**< the frequency set by B2B1CLK	*/
	FJ_TOP_LTM1_CLKSEL_B2R1CLK = 32,		/**< the frequency set by B2R1CLK	*/
	FJ_TOP_LTM1_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_LTM1_CLKSEL_MAX
} FJ_TOP_LTM1_CLKSEL;

/** LTM2 Clock Select */
typedef enum {
	FJ_TOP_LTM2_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 			*/
	FJ_TOP_LTM2_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 			*/
	FJ_TOP_LTM2_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 			*/
	FJ_TOP_LTM2_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)			*/
	FJ_TOP_LTM2_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 			*/
	FJ_TOP_LTM2_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 			*/
	FJ_TOP_LTM2_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 			*/
	FJ_TOP_LTM2_CLKSEL_SRO2CLK = 8,			/**< the frequency set by SRO2CLK	*/
	FJ_TOP_LTM2_CLKSEL_SRO2CLK_2 = 16,		/**< the frequency set by SRO2CLK_2	*/
	FJ_TOP_LTM2_CLKSEL_B2B2CLK = 24,		/**< the frequency set by B2B2CLK	*/
	FJ_TOP_LTM2_CLKSEL_B2R2CLK = 32,		/**< the frequency set by B2R2CLK	*/
	FJ_TOP_LTM2_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_LTM2_CLKSEL_MAX
} FJ_TOP_LTM2_CLKSEL;

/** B2R1 Clock Select */
typedef enum {
	FJ_TOP_B2R1_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 			*/
	FJ_TOP_B2R1_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 			*/
	FJ_TOP_B2R1_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 			*/
	FJ_TOP_B2R1_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)			*/
	FJ_TOP_B2R1_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 			*/
	FJ_TOP_B2R1_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 			*/
	FJ_TOP_B2R1_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 			*/
	FJ_TOP_B2R1_CLKSEL_SRO1CLK = 8,			/**< the frequency set by SRO1CLK	*/
	FJ_TOP_B2R1_CLKSEL_SRO1CLK_2 = 16,		/**< the frequency set by SRO1CLK_2	*/
	FJ_TOP_B2R1_CLKSEL_B2B1CLK = 24,		/**< the frequency set by B2B1CLK	*/
	FJ_TOP_B2R1_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_B2R1_CLKSEL_MAX
} FJ_TOP_B2R1_CLKSEL;

/** B2R2 Clock Select */
typedef enum {
	FJ_TOP_B2R2_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 			*/
	FJ_TOP_B2R2_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 			*/
	FJ_TOP_B2R2_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 			*/
	FJ_TOP_B2R2_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)			*/
	FJ_TOP_B2R2_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 			*/
	FJ_TOP_B2R2_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 			*/
	FJ_TOP_B2R2_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 			*/
	FJ_TOP_B2R2_CLKSEL_SRO2CLK = 8,			/**< the frequency set by SRO2CLK	*/
	FJ_TOP_B2R2_CLKSEL_SRO2CLK_2 = 16,		/**< the frequency set by SRO2CLK_2	*/
	FJ_TOP_B2R2_CLKSEL_B2B2CLK = 24,		/**< the frequency set by B2B2CLK	*/
	FJ_TOP_B2R2_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_B2R2_CLKSEL_MAX
} FJ_TOP_B2R2_CLKSEL;

/** CNR1 Clock Select */
typedef enum {
	FJ_TOP_CNR1_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 			*/
	FJ_TOP_CNR1_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 			*/
	FJ_TOP_CNR1_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 			*/
	FJ_TOP_CNR1_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)			*/
	FJ_TOP_CNR1_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 			*/
	FJ_TOP_CNR1_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 			*/
	FJ_TOP_CNR1_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 			*/
	FJ_TOP_CNR1_CLKSEL_R2Y1CLK = 8,			/**< the frequency set by R2Y1CLK	*/
	FJ_TOP_CNR1_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_CNR1_CLKSEL_MAX
} FJ_TOP_CNR1_CLKSEL;

/** CNR2 Clock Select */
typedef enum {
	FJ_TOP_CNR2_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 			*/
	FJ_TOP_CNR2_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 			*/
	FJ_TOP_CNR2_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 			*/
	FJ_TOP_CNR2_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)			*/
	FJ_TOP_CNR2_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 			*/
	FJ_TOP_CNR2_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 			*/
	FJ_TOP_CNR2_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 			*/
	FJ_TOP_CNR2_CLKSEL_R2Y1CLK = 8,			/**< the frequency set by R2Y2CLK	*/
	FJ_TOP_CNR2_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_CNR2_CLKSEL_MAX
} FJ_TOP_CNR2_CLKSEL;

/** B2B1 Clock Select */
typedef enum {
	FJ_TOP_B2B1_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 			*/
	FJ_TOP_B2B1_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 			*/
	FJ_TOP_B2B1_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 			*/
	FJ_TOP_B2B1_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)			*/
	FJ_TOP_B2B1_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 			*/
	FJ_TOP_B2B1_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 			*/
	FJ_TOP_B2B1_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 			*/
	FJ_TOP_B2B1_CLKSEL_SRO1CLK = 8,			/**< the frequency set by SRO1CLK	*/
	FJ_TOP_B2B1_CLKSEL_SRO1CLK_2 = 16,		/**< the frequency set by SRO1CLK_2	*/
	FJ_TOP_B2B1_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_B2B1_CLKSEL_MAX
} FJ_TOP_B2B1_CLKSEL;

/** B2B2 Clock Select */
typedef enum {
	FJ_TOP_B2B2_CLKSEL_300MHZ = 0,			/**< 300MHz (=1200MHz/4) 			*/
	FJ_TOP_B2B2_CLKSEL_200MHZ,				/**< 200MHz (=1200MHz/6) 			*/
	FJ_TOP_B2B2_CLKSEL_150MHZ,				/**< 150MHz (=1200MHz/8) 			*/
	FJ_TOP_B2B2_CLKSEL_100MHZ,				/**< 100MHz (=1200MHz/12)			*/
	FJ_TOP_B2B2_CLKSEL_75MHZ,				/**<  75MHz (=1200MHz/16) 			*/
	FJ_TOP_B2B2_CLKSEL_60MHZ,				/**<  60MHz (=1200MHz/20) 			*/
	FJ_TOP_B2B2_CLKSEL_50MHZ,				/**<  50MHz (=1200MHz/24) 			*/
	FJ_TOP_B2B2_CLKSEL_SRO2CLK = 8,			/**< the frequency set by SRO2CLK	*/
	FJ_TOP_B2B2_CLKSEL_SRO2CLK_2 = 16,		/**< the frequency set by SRO2CLK_2	*/
	FJ_TOP_B2B2_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_B2B2_CLKSEL_MAX
} FJ_TOP_B2B2_CLKSEL;

/** LTMRBK1 Clock Select */
typedef enum {
	FJ_TOP_LTMRBK1_CLKSEL_300MHZ = 0,		/**< 300MHz (=1200MHz/4) 			*/
	FJ_TOP_LTMRBK1_CLKSEL_200MHZ,			/**< 200MHz (=1200MHz/6) 			*/
	FJ_TOP_LTMRBK1_CLKSEL_150MHZ,			/**< 150MHz (=1200MHz/8) 			*/
	FJ_TOP_LTMRBK1_CLKSEL_100MHZ,			/**< 100MHz (=1200MHz/12)			*/
	FJ_TOP_LTMRBK1_CLKSEL_75MHZ,			/**<  75MHz (=1200MHz/16) 			*/
	FJ_TOP_LTMRBK1_CLKSEL_60MHZ,			/**<  60MHz (=1200MHz/20) 			*/
	FJ_TOP_LTMRBK1_CLKSEL_50MHZ,			/**<  50MHz (=1200MHz/24) 			*/
	FJ_TOP_LTMRBK1_CLKSEL_SRO1CLK = 8,		/**< the frequency set by SRO1CLK	*/
	FJ_TOP_LTMRBK1_CLKSEL_SRO1CLK_2 = 16,	/**< the frequency set by SRO1CLK_2	*/
	FJ_TOP_LTMRBK1_CLKSEL_OFF,				/**< OFF */
	FJ_TOP_LTMRBK1_CLKSEL_MAX
} FJ_TOP_LTMRBK1_CLKSEL;

/** LTMRBK2 Clock Select */
typedef enum {
	FJ_TOP_LTMRBK2_CLKSEL_300MHZ = 0,		/**< 300MHz (=1200MHz/4) 			*/
	FJ_TOP_LTMRBK2_CLKSEL_200MHZ,			/**< 200MHz (=1200MHz/6) 			*/
	FJ_TOP_LTMRBK2_CLKSEL_150MHZ,			/**< 150MHz (=1200MHz/8) 			*/
	FJ_TOP_LTMRBK2_CLKSEL_100MHZ,			/**< 100MHz (=1200MHz/12)			*/
	FJ_TOP_LTMRBK2_CLKSEL_75MHZ,			/**<  75MHz (=1200MHz/16) 			*/
	FJ_TOP_LTMRBK2_CLKSEL_60MHZ,			/**<  60MHz (=1200MHz/20) 			*/
	FJ_TOP_LTMRBK2_CLKSEL_50MHZ,			/**<  50MHz (=1200MHz/24) 			*/
	FJ_TOP_LTMRBK2_CLKSEL_SRO2CLK = 8,		/**< the frequency set by SRO2CLK	*/
	FJ_TOP_LTMRBK2_CLKSEL_SRO2CLK_2 = 16,	/**< the frequency set by SRO2CLK_2	*/
	FJ_TOP_LTMRBK2_CLKSEL_OFF,				/**< OFF */
	FJ_TOP_LTMRBK2_CLKSEL_MAX
} FJ_TOP_LTMRBK2_CLKSEL;

/** SNAPCLKO0/SNAPCLKO1/SNAPCLKO2/SNAPCLKO3 Clock Select */
typedef enum {
	FJ_TOP_SNAPCLKO_CLKSEL_PLL0 = 0,		/**< the frequency set by PLL00 (fractional)	*/
	FJ_TOP_SNAPCLKO_CLKSEL_APCLK,			/**< the frequency set by APCLK					*/
	FJ_TOP_SNAPCLKO_CLKSEL_OFF,				/**< OFF */
	FJ_TOP_SNAPCLKO_CLKSEL_MAX
} FJ_TOP_SNAPCLKO_CLKSEL;

/** APCLK Clock Select */
typedef enum {
	FJ_TOP_APCLK_CLKSEL_74P25MHZ = 0,		/**< 74.250MHz(=1188MHz/16)	*/
	FJ_TOP_APCLK_CLKSEL_54MHZ,				/**< 54.000MHz(=1188MHz/22)	*/
	FJ_TOP_APCLK_CLKSEL_37P125MHZ,			/**< 37.125MHz(=1188MHz/32)	*/
	FJ_TOP_APCLK_CLKSEL_27MHZ,				/**< 27.000MHz(=1188MHz/44)	*/
	FJ_TOP_APCLK_CLKSEL_72MHZ,				/**< 72.000MHz(= 864MHz/12)	*/
	FJ_TOP_APCLK_CLKSEL_48MHZ,				/**< 48.000MHz(= 864MHz/18)	*/
	FJ_TOP_APCLK_CLKSEL_63P158MHZ = 8,		/**< 63.158MHz(=1600MHz/25)	*/
	FJ_TOP_APCLK_CLKSEL_45P714MHZ = 9,		/**< 45.714MHz(=1600MHz/35)	*/
	FJ_TOP_APCLK_CLKSEL_60MHZ = 12,			/**< 60.000MHz(=1500MHz/25)	*/
	FJ_TOP_APCLK_CLKSEL_MAX
} FJ_TOP_APCLK_CLKSEL;

/** NETSEC Clock Select */
typedef enum {
	FJ_TOP_AU_CLKSEL_APLCLK = 0,			/**< source set by APLCLK */
	FJ_TOP_AU_CLKSEL_EXTERNAL,				/**< source is AUMCLKI external pin */
	FJ_TOP_AU_CLKSEL_MAX
}	FJ_TOP_AU_CLKSEL;

/** SPI Clock Select */
typedef enum {
	FJ_TOP_SPI_CLKSEL_216MHZ = 0,			/**< 216MHz (=864MHz/4) 	*/
	FJ_TOP_SPI_CLKSEL_108MHZ,				/**< 108MHz (=864MHz/8) 	*/
	FJ_TOP_SPI_CLKSEL_100MHZ,				/**< 100MHz (=1600MHz/16) 	*/
	FJ_TOP_SPI_CLKSEL_MAX
}	FJ_TOP_SPI_CLKSEL;

/** DSP Clock Select */
typedef enum {
	FJ_TOP_DSP_CLKSEL_400MHZ = 0,			/**< 400MHz 	(=1600MHz/4) 	*/
	FJ_TOP_DSP_CLKSEL_266MHZ,				/**< 266.666MHz (=1600MHz/6)	*/
	FJ_TOP_DSP_CLKSEL_200MHZ,				/**< 200MHz 	(=1600MHz/8) 	*/
	FJ_TOP_DSP_CLKSEL_100MHZ,				/**< 100MHz 	(=1600MHz/16) 	*/
	FJ_TOP_DSP_CLKSEL_MAX
}	FJ_TOP_DSP_CLKSEL;

/** AXI Clock Select (except for EXS BLOCK) */
typedef enum {
	FJ_TOP_AXI_CLKSEL_200MHZ = 3,			/**< 200MHz(=1600MHz/8)     */
	FJ_TOP_AXI_CLKSEL_100MHZ = 7,			/**< 100MHz(=1600MHz/16)    */
	FJ_TOP_AXI_CLKSEL_MAX
}	FJ_TOP_AXI_CLKSEL;

/** EXS BLOCK AXI Clock Select */
typedef enum {
	FJ_TOP_EAXI_CLKSEL_200MHZ = 3,			/**< 200MHz(=1600MHz/8)     */
	FJ_TOP_EAXI_CLKSEL_160MHZ,				/**< 160MHz(=1600MHz/10)    */
	FJ_TOP_EAXI_CLKSEL_133MHZ,				/**< 133.333MHz(=1600MHz/12)*/
	FJ_TOP_EAXI_CLKSEL_100MHZ = 7,			/**< 100MHz(=1600MHz/16)    */
	FJ_TOP_EAXI_CLKSEL_MAX
}	FJ_TOP_EAXI_CLKSEL;

/** BMH BLOCK AHB Clock Select */
typedef enum {
	FJ_TOP_BAHB_CLKSEL_200MHZ = 3,			/**< 200MHz(=1600MHz/8)     */
	FJ_TOP_BAHB_CLKSEL_100MHZ = 7,			/**< 100MHz(=1600MHz/16)    */
	FJ_TOP_BAHB_CLKSEL_MAX
}	FJ_TOP_BAHB_CLKSEL;

/** AHB Clock Select */
typedef enum {
	FJ_TOP_AHB_CLKSEL_100MHZ = 7,			/**< 100MHz(=1600MHz/16)     */
	FJ_TOP_AHB_CLKSEL_50MHZ = 15,			/**< 50MHz(=1600MHz/32)      */
	FJ_TOP_AHB_CLKSEL_MAX
}	FJ_TOP_AHB_CLKSEL;

/** APB Clock Select */
typedef enum {
	FJ_TOP_APB_CLKSEL_50MHZ = 15,			/**< 50MHz(=1600MHz/32)      */
	FJ_TOP_APB_CLKSEL_25MHZ = 31,			/**< 25MHz(=1600MHz/64)      */
	FJ_TOP_APB_CLKSEL_MAX
}	FJ_TOP_APB_CLKSEL;

/** MXI400 Clock Select */
typedef enum {
	FJ_TOP_MXI400_CLKSEL_400MHZ = 1,		/**< 400MHz(=1600MHz/4)     */
	FJ_TOP_MXI400_CLKSEL_200MHZ = 3,		/**< 200MHz(=1600MHz/8)     */
	FJ_TOP_MXI400_CLKSEL_MAX
}	FJ_TOP_MXI400_CLKSEL;

/** MXI200 Clock Select */
typedef enum {
	FJ_TOP_MXI200_CLKSEL_200MHZ = 3,		/**< 200MHz(=1600MHz/8)     */
	FJ_TOP_MXI200_CLKSEL_100MHZ = 7,		/**< 100MHz(=1600MHz/16)    */
	FJ_TOP_MXI200_CLKSEL_MAX
}	FJ_TOP_MXI200_CLKSEL;

/** AXI400 Clock Select */
typedef enum {
	FJ_TOP_AXI400_CLKSEL_400MHZ = 1,		/**< 400MHz(=1600MHz/4)     */
	FJ_TOP_AXI400_CLKSEL_200MHZ = 3,		/**< 200MHz(=1600MHz/8)     */
	FJ_TOP_AXI400_CLKSEL_MAX
}	FJ_TOP_AXI400_CLKSEL;

/** RIBERY Core Select */
typedef enum {
	FJ_TOP_RIB_CORE_1 = 1,					/**<  1core Mode */
	FJ_TOP_RIB_CORE_2,						/**<  2core Mode */
	FJ_TOP_RIB_CORE_MAX
}	FJ_TOP_RIB_CORE;

/** RIBERY Clock Select */
typedef enum {
	FJ_TOP_RIB_CLKSEL_466MHZ = 0,			/**< 466.666MHz(=1400MHz/3) */
	FJ_TOP_RIB_CLKSEL_350MHZ,				/**< 350MHz(=1400MHz/4)     */
	FJ_TOP_RIB_CLKSEL_233MHZ,				/**< 233.333MHz(=1400MHz/6) */
	FJ_TOP_RIB_CLKSEL_116MHZ,				/**< 116.666MHz(=1400MHz/12)*/
	FJ_TOP_RIB_CLKSEL_58MHZ,				/**< 58MHz(=1400MHz/24)     */
	FJ_TOP_RIB_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_RIB_CLKSEL_MAX
}	FJ_TOP_RIB_CLKSEL;

/** NFBCH Clock Select */
typedef enum {
	FJ_TOP_NFBCH_CLKSEL_200MHZ = 0,			/**< 200MHz(=1600MHz/8)     */
	FJ_TOP_NFBCH_CLKSEL_100MHZ,				/**< 100MHz(=1600MHz/16)    */
	FJ_TOP_NFBCH_CLKSEL_MAX
}	FJ_TOP_NFBCH_CLKSEL;

/** ME Clock Select */
typedef enum {
	FJ_TOP_ME_CLKSEL_466MHZ = 0,			/**< 466.666MHz(=1400MHz/3) */
	FJ_TOP_ME_CLKSEL_233MHZ,				/**< 233.333MHz(=1400MHz/6) */
	FJ_TOP_ME_CLKSEL_116MHZ,				/**< 116.666MHz(=1400MHz/12)*/
	FJ_TOP_ME_CLKSEL_58MHZ,					/**<  58.333MHz(=1400MHz/24)*/
	FJ_TOP_ME_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_ME_CLKSEL_MAX
}	FJ_TOP_ME_CLKSEL;

/** FPT0/FPT1 Clock Select */
typedef enum {
	FJ_TOP_FPT_CLKSEL_300MHZ = 0,			/**< 300MHz(=1200MHz/4)     */
	FJ_TOP_FPT_CLKSEL_200MHZ,				/**< 200MHz(=1200MHz/6)     */
	FJ_TOP_FPT_CLKSEL_150MHZ,				/**< 150MHz(=1200MHz/8)     */
	FJ_TOP_FPT_CLKSEL_100MHZ,				/**< 100MHz(=1200MHz/12)    */
	FJ_TOP_FPT_CLKSEL_75MHZ,				/**< 75MHz(=1200MHz/16)     */
	FJ_TOP_FPT_CLKSEL_60MHZ,				/**< 60MHz(=1200MHz/20)     */
	FJ_TOP_FPT_CLKSEL_50MHZ,				/**< 50MHz(=1200MHz/24)     */
	FJ_TOP_FPT_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_FPT_CLKSEL_MAX
}	FJ_TOP_FPT_CLKSEL;

/** SHDR Clock Select */
typedef enum {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	FJ_TOP_SHDR_CLKSEL_400MHZ = 0,			/**< 400MHz(=1600MHz/4)     */
	FJ_TOP_SHDR_CLKSEL_266MHZ,				/**< 266.666MHz(=1600MHz/6) */
	FJ_TOP_SHDR_CLKSEL_200MHZ,				/**< 200MHz(=1600MHz/8)     */
	FJ_TOP_SHDR_CLKSEL_160MHZ,				/**< 160MHz(=1600MHz/10)    */
	FJ_TOP_SHDR_CLKSEL_133MHZ,				/**< 133.333MHz(=1600MHz/12)*/
	FJ_TOP_SHDR_CLKSEL_100MHZ,				/**< 100MHz(=1600MHz/16)    */
	FJ_TOP_SHDR_CLKSEL_80MHZ,				/**< 80MHz(=1600MHz/20)     */
	FJ_TOP_SHDR_CLKSEL_66MHZ,				/**< 66.666MHz(=1600MHz/24) */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	FJ_TOP_SHDR_CLKSEL_432MHZ = 0,			/**< 432MHz(=864MHz/2)      */
	FJ_TOP_SHDR_CLKSEL_400MHZ = 8,			/**< 400MHz(=1600MHz/4)     */
	FJ_TOP_SHDR_CLKSEL_266MHZ,				/**< 266.666MHz(=1600MHz/6) */
	FJ_TOP_SHDR_CLKSEL_200MHZ,				/**< 200MHz(=1600MHz/8)     */
	FJ_TOP_SHDR_CLKSEL_160MHZ,				/**< 160MHz(=1600MHz/10)    */
	FJ_TOP_SHDR_CLKSEL_133MHZ,				/**< 133.333MHz(=1600MHz/12)*/
	FJ_TOP_SHDR_CLKSEL_100MHZ,				/**< 100MHz(=1600MHz/16)    */
	FJ_TOP_SHDR_CLKSEL_80MHZ,				/**< 80MHz(=1600MHz/20)     */
	FJ_TOP_SHDR_CLKSEL_66MHZ,				/**< 66.666MHz(=1600MHz/24) */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	FJ_TOP_SHDR_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_SHDR_CLKSEL_MAX
}	FJ_TOP_SHDR_CLKSEL;

/** GYRO Clock Select */
typedef enum {
	FJ_TOP_GYRO_CLKSEL_100MHZ = 0,			/**< 100MHz(=1200MHz/16)    */
	FJ_TOP_GYRO_CLKSEL_32MHZ,				/**< 32MHz(=1200MHz/50)     */
	FJ_TOP_GYRO_CLKSEL_2MHZ,				/**< 2MHz(=1200MHz/800)     */
	FJ_TOP_GYRO_CLKSEL_OFF,					/**< OFF */
	FJ_TOP_GYRO_CLKSEL_MAX
}	FJ_TOP_GYRO_CLKSEL;

/** AXI300 Clock Select */
typedef enum {
	FJ_TOP_AXI300_CLKSEL_300MHZ = 0,		/**< 300MHz(=1200MHz/4)     */
	FJ_TOP_AXI300_CLKSEL_200MHZ,			/**< 200MHz(=1200MHz/6)     */
	FJ_TOP_AXI300_CLKSEL_MAX
}	FJ_TOP_AXI300_CLKSEL;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Image Macro clock.
*/
typedef struct {
	FJ_TOP_RAW_CLKSEL		raw;			/**< RAW			Clock	*/
	FJ_TOP_JPG_CLKSEL		jpg;			/**< JPEG			Clock	*/
	FJ_TOP_ELA_CLKSEL		ela;			/**< ELA			Clock	*/
	FJ_TOP_SEN_CLKSEL		sen;			/**< SEN			Clock	*/
	FJ_TOP_SENMSK_CLKSEL	senmsk;			/**< SENMSK			Clock	*/
	FJ_TOP_IIP_CLKSEL		iip;			/**< IIP			Clock	*/
	FJ_TOP_PAS_CLKSEL		pas;			/**< PAS			Clock	*/
	FJ_TOP_SRO_CLKSEL		sro1;			/**< SRO(PIPE1)		Clock	*/
	FJ_TOP_SRO_CLKSEL		sro1_2;			/**< SRO_2(PIPE1)	Clock	*/
	FJ_TOP_B2B1_CLKSEL		b2b1;			/**< B2B(PIPE1)		Clock	*/
	FJ_TOP_B2R1_CLKSEL		b2r1;			/**< B2R(PIPE1)		Clock	*/
	FJ_TOP_LTM1_CLKSEL		ltm1;			/**< LTM(PIPE1)		Clock	*/
	FJ_TOP_LTMRBK1_CLKSEL	ltmrbk1;		/**< LTMRBK(PIPE1)	Clock	*/
	FJ_TOP_R2Y_CLKSEL		r2y1;			/**< R2Y(PIPE1)		Clock	*/
	FJ_TOP_CNR1_CLKSEL		cnr1;			/**< CNR(PIPE1)		Clock	*/
	FJ_TOP_SRO_CLKSEL		sro2;			/**< SRO(PIPE2)		Clock	*/
	FJ_TOP_SRO_CLKSEL		sro2_2;			/**< SRO_2(PIPE2)	Clock	*/
	FJ_TOP_B2B2_CLKSEL		b2b2;			/**< B2B(PIPE2)		Clock	*/
	FJ_TOP_B2R2_CLKSEL		b2r2;			/**< B2R(PIPE2)		Clock	*/
	FJ_TOP_LTM2_CLKSEL		ltm2;			/**< LTM(PIPE2)		Clock	*/
	FJ_TOP_LTMRBK2_CLKSEL	ltmrbk2;		/**< LTMRBK(PIPE2)	Clock	*/
	FJ_TOP_R2Y_CLKSEL		r2y2;			/**< R2Y(PIPE2)		Clock	*/
	FJ_TOP_CNR2_CLKSEL		cnr2;			/**< CNR(PIPE2)		Clock	*/
	FJ_TOP_SNAPCLKO_CLKSEL	apclk0;			/**< SNAPCLKO0		Clock	*/
	FJ_TOP_SNAPCLKO_CLKSEL	apclk1;			/**< SNAPCLKO1		Clock	*/
	FJ_TOP_SNAPCLKO_CLKSEL	apclk2;			/**< SNAPCLKO2		Clock	*/
	FJ_TOP_SNAPCLKO_CLKSEL	apclk3;			/**< SNAPCLKO3		Clock	*/
	FJ_TOP_APCLK_CLKSEL		apclk;			/**< APCLK			Clock	*/
	FJ_TOP_IPP_CLKSEL		ipp;			/**< IPP(HEVC)		Clock	*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	FJ_TOP_PXF_CLKSEL		pxf;			/**< PXF(HEVC)		Clock	*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	FJ_TOP_HEVENC_CLKSEL	hevenc;			/**< HEVENC(HEVC)	Clock	*/
	FJ_TOP_HEVSEL_CLKSEL	hevsel;			/**< HEVENCSEL(HEVC)	Clock	*/
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	FJ_TOP_VDF_CLKSEL		vdf;			/**< VDF(HEVC)		Clock	*/
	FJ_TOP_RIB_CLKSEL		rib;			/**< RIBERY			Clock	*/
	FJ_TOP_RIB_CORE			rib_core;		/**< RIBERY Core Mode		*/
	FJ_TOP_FPT_CLKSEL		fpt0;			/**< FPT0			Clock	*/
	FJ_TOP_FPT_CLKSEL		fpt1;			/**< FPT1			Clock	*/
	FJ_TOP_ME_CLKSEL		me;				/**< ME				Clock	*/
	FJ_TOP_SHDR_CLKSEL		shdr;			/**< SHDR			Clock	*/
	FJ_TOP_IPU_CLKSEL		ipu;			/**< IPU			Clock	*/
	FJ_TOP_IPU_TME_CLKSEL	ipu_tme;		/**< IPU TME		Clock	*/
} FJ_TOP_IMAGE_CLOCK;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*@} --- end of fj_top_definition */
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/** @weakgroup fj_top_api
@{*/
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Start clock.
@param[in] clock_name	: Clock name. See @ref FJ_TOP_CLOCK_NAME.
@param[in,out] counter	: Counter for exclusive control.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks Counter is incremented inside the API, and started clock when a it is 1.<br>
		 Therefore, please share this counter between programs that control the same clock.
*/
extern FJ_ERR_CODE FJ_Top_Start_Clock( FJ_TOP_CLOCK_NAME clock_name, UCHAR* counter );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Stop clock.
@param[in] clock_name	: Clock name. See @ref FJ_TOP_CLOCK_NAME.
@param[in,out] counter	: Counter for exclusive control.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks Counter is decremented inside the API, and stoped clock when a it is 0.<br>
		 Therefore, please share this counter between programs that control the same clock.
*/
extern FJ_ERR_CODE FJ_Top_Stop_Clock( FJ_TOP_CLOCK_NAME clock_name, UCHAR* counter );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set EMMC clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_EMMC_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_EMMC( FJ_TOP_EMMC_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set Nand Flash I/F clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_NF_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_NF( FJ_TOP_NF_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SD Card I/F ch2 UHS-II clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SD2U2_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SD2U2( FJ_TOP_SD2U2_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SD Card I/F ch2 UHS-I clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SD2U1_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SD2U1( FJ_TOP_SD2U1_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SD Card I/F ch3 UHS-I clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SD3_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SD3U1( FJ_TOP_SD3_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SD Card I/F ch1 SDIO clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SD1_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SD1IO( FJ_TOP_SD1_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set RAW clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_RAW_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_RAW( FJ_TOP_RAW_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set HIF clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_HIF_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_HIF( FJ_TOP_HIF_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set MIF clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_MIF_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_MIF( FJ_TOP_MIF_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set IPU clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_IPU_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_IPU( FJ_TOP_IPU_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set IPU TME clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_IPU_TME_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_IPU_TME( FJ_TOP_IPU_TME_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set GPU clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_GPU_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_GPU( FJ_TOP_GPU_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set JPEG clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_JPG_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_JPEG( FJ_TOP_JPG_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set ELA clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_ELA_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_ELA( FJ_TOP_ELA_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set PIPE select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_PIPE_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_PIPE( FJ_TOP_PIPE_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SENSOR clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SEN_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SEN( FJ_TOP_SEN_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SENSOR clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SENMSK_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SENMSK( FJ_TOP_SENMSK_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set IIP clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_IIP_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_IIP( FJ_TOP_IIP_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set STAT clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_PAS_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_PAS( FJ_TOP_PAS_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set IPP clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_IPP_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_IPP( FJ_TOP_IPP_CLKSEL clksel );

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set PXF clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_PXF_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_PXF( FJ_TOP_PXF_CLKSEL clksel );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set HEVENC clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_HEVENC_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_HEVENC( FJ_TOP_HEVENC_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set HEVENCSEL select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_HEVSEL_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_HEVSEL( FJ_TOP_HEVSEL_CLKSEL clksel );
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set VDF clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_VDF_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_VDF( FJ_TOP_VDF_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SRO1 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SRO_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SRO1_2( FJ_TOP_SRO_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set R2Y1 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_R2Y_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_R2Y1( FJ_TOP_R2Y_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set LTM1 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_LTM1_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_LTM1( FJ_TOP_LTM1_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set B2R1 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_B2R1_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_B2R1( FJ_TOP_B2R1_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set CNR1 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_CNR1_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_CNR1( FJ_TOP_CNR1_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SRO1 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SRO_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SRO1( FJ_TOP_SRO_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set B2B1 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_B2B1_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_B2B1( FJ_TOP_B2B1_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set LTMRBK1 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_LTMRBK1_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_LTMRBK1( FJ_TOP_LTMRBK1_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SRO2 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SRO_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SRO2_2( FJ_TOP_SRO_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set R2Y2 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_R2Y_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_R2Y2( FJ_TOP_R2Y_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set LTM2 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_LTM2_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_LTM2( FJ_TOP_LTM2_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set B2R2 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_B2R2_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_B2R2( FJ_TOP_B2R2_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set CNR2 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_CNR2_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_CNR2( FJ_TOP_CNR2_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SRO2 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SRO_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SRO2( FJ_TOP_SRO_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set B2B2 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_B2B2_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_B2B2( FJ_TOP_B2B2_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SNAPCLKO3 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_LTMRBK2_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_LTMRBK2( FJ_TOP_LTMRBK2_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SNAPCLKO3 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SNAPCLKO_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_AP3( FJ_TOP_SNAPCLKO_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SNAPCLKO2 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SNAPCLKO_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_AP2( FJ_TOP_SNAPCLKO_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SNAPCLKO1 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SNAPCLKO_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_AP1( FJ_TOP_SNAPCLKO_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SNAPCLKO0 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SNAPCLKO_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_AP0( FJ_TOP_SNAPCLKO_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set APCLK clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_APCLK_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_APCLK( FJ_TOP_APCLK_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set NETSEC clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_AU_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_NETAU( FJ_TOP_AU_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SPI clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SPI_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SPI( FJ_TOP_SPI_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set DSP clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_DSP_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_DSP( FJ_TOP_DSP_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set AXI clock select (except for EXS BLOCK).
@param[in] clksel	: Clock select. See @ref FJ_TOP_AXI_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks When this setting is validated, FJ_Top_Request_Clock_Select_DCHREQ() should be called after this API.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_AXI( FJ_TOP_AXI_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set EXS BLOCK AXI clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_EAXI_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks When this setting is validated, FJ_Top_Request_Clock_Select_DCHREQ() should be called after this API.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_EAXI( FJ_TOP_EAXI_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set BMH BLOCK AHB clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_BAHB_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks When this setting is validated, FJ_Top_Request_Clock_Select_DCHREQ() should be called after this API.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_BAHB( FJ_TOP_BAHB_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set AHB clock select (except for EXS BLOCK).
@param[in] clksel	: Clock select. See @ref FJ_TOP_AHB_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks When this setting is validated, FJ_Top_Request_Clock_Select_DCHREQ() should be called after this API.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_AHB( FJ_TOP_AHB_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set APB clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_APB_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks When this setting is validated, FJ_Top_Request_Clock_Select_DCHREQ() should be called after this API.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_APB( FJ_TOP_APB_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set MXI400 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_MXI400_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_MXI400( FJ_TOP_MXI400_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set MXI200 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_MXI200_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_MXI200( FJ_TOP_MXI200_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set AXI400 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_AXI400_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_AXI400( FJ_TOP_AXI400_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Request setting of select for CPU, L2C, CPU Peripheral, EAXI, AXI, AHB, and APB clocks.
@param[in] req	: Clock Set Request (1 : request, 0 : none).
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Request_Clock_Select_DCHREQ( UINT8 req );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set RIBERY clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_RIB_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_RIB( FJ_TOP_RIB_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set NFBCH clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_NFBCH_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_NFBCH( FJ_TOP_NFBCH_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set ME clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_ME_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_ME( FJ_TOP_ME_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set FPT1 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_FPT_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_FPT1( FJ_TOP_FPT_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set FPT0 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_FPT_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_FPT0( FJ_TOP_FPT_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set SHDR clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_SHDR_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_SHDR( FJ_TOP_SHDR_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set GYRO clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_GYRO_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_GYRO( FJ_TOP_GYRO_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set AXI300 clock select.
@param[in] clksel	: Clock select. See @ref FJ_TOP_AXI300_CLKSEL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Select_AXI300( FJ_TOP_AXI300_CLKSEL clksel );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API is sets the Image clock.
@param[in]	image_clock		: Clock settings information. . See @ref FJ_TOP_IMAGE_CLOCK.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern	FJ_ERR_CODE FJ_Top_Set_Image_Clock( FJ_TOP_IMAGE_CLOCK image_clock );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Assert DSP reset.
*/
extern VOID FJ_Top_Assert_Reset_DSP( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Negate DSP reset.
*/
extern VOID FJ_Top_Negate_Reset_DSP( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Get DSP reset state.
@retval	 Reset state.<br>
			- 0 : The reset is negated
			- 1 : The reset is asserted
*/
extern ULONG FJ_Top_Get_Reset_State_DSP( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Get clock frequency of ARM Cortex-A7 MPCore Peripheral.
@retval	clock frequency (Hz)
*/
extern ULONG FJ_Top_Get_Clock_Core_Peripheral( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API sets the clock stop for MICAX0.
@param[in] val	: clock stop value (0:no-stop or 1:stop)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_Top_Set_Clock_Stop_MICAX0( UCHAR val );


/* @} */	// fj_top_api

#ifdef __cplusplus
}
#endif

#endif	// _FJ_TOP_H
