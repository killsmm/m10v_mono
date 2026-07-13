/**
 * @file		ct_im_ela.h
 * @brief		ct header for im_ela.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _CT_IM_ELA_H_
#define _CT_IM_ELA_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/************************************/
/* Size definition                  */
/************************************/
#define S_ELA_BAYER_16M_WIDTH		(4704)
#define S_ELA_BAYER_16M_LINES		(3488)
#define S_ELA_BAYER_12M_WIDTH		(4032)
#define S_ELA_BAYER_12M_LINES		(3024)
#define S_ELA_BAYER_FHD_WIDTH		(2796)
#define S_ELA_BAYER_FHD_LINES		(1560)

#define S_ELA_BAYER_4K2K_WIDTH		(4448)			// 4424(+24)
#define S_ELA_BAYER_4K2K_LINES		(2488)


#define S_SDRAM_ELA_SIZ_16M			(S_ELA_BAYER_16M_WIDTH * S_ELA_BAYER_16M_LINES * 3 / 2)
#define S_SDRAM_ELA_SIZ_12M			(S_ELA_BAYER_12M_WIDTH * S_ELA_BAYER_12M_LINES * 3 / 2)
#define S_SDRAM_ELA_SIZ_FHD			(S_ELA_BAYER_FHD_WIDTH * S_ELA_BAYER_FHD_LINES * 3 / 2)
#define S_SDRAM_ELA_SIZ_4K2K		(S_ELA_BAYER_4K2K_WIDTH * S_ELA_BAYER_4K2K_LINES * 3 / 2)
#define S_SDRAM_ELA_NOISE_SUPPRESS	(0x1000000)		// Noise Suppress Data : 16M
#define S_SDRAM_ELA_EXTRACT_NOISE	(0x2600000)		// Extract Noise Data : 38M
#define S_SDRAM_ELA_OUT_BAYER		(0x5400000)		// Bayer Data : 84M
/************************************/
/* Address definition               */
/************************************/
//#define S_SDRAM_ELA_BASE			(0x81000000)
//#define S_SDRAM_ELA_BASE			(0x21000000)
#define S_SDRAM_ELA_BASE			(0x48000000)

#define S_SDRAM_ELA_ADR_IN_BAYER		(S_SDRAM_ELA_BASE)
#define S_SDRAM_ELA_ADR_NOISE_SUPPRESS	(S_SDRAM_ELA_ADR_IN_BAYER + S_SDRAM_ELA_SIZ_16M)					// 0x79778A00
#define S_SDRAM_ELA_ADR_EXTRACT_NOISE	(S_SDRAM_ELA_ADR_NOISE_SUPPRESS + S_SDRAM_ELA_NOISE_SUPPRESS)		// 0x7A778A00
#define S_SDRAM_ELA_ADR_OUT_BAYER		(S_SDRAM_ELA_ADR_EXTRACT_NOISE  + S_SDRAM_ELA_EXTRACT_NOISE)		// 0x7CD78A00

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern	VOID CT_Im_Ela_Reg_Print( VOID );
extern	VOID CT_Im_Ela_1_01( VOID );
extern	VOID CT_Im_Ela_1_02( VOID );
extern	VOID CT_Im_Ela_1_03( VOID );
extern	VOID CT_Im_Ela_1_04( VOID );
extern	VOID CT_Im_Ela_1_05( VOID );
extern	VOID CT_Im_Ela_1_06( VOID );
extern	VOID CT_Im_Ela_1_07( VOID );
extern	VOID CT_Im_Ela_1_08( VOID );
extern	VOID CT_Im_Ela_1_09( VOID );
extern	VOID CT_Im_Ela_1_10( VOID );
extern	VOID CT_Im_Ela_1_11( VOID );
extern	VOID CT_Im_Ela_1_12( VOID );
extern	VOID CT_Im_Ela_1_13( VOID );
extern	VOID CT_Im_Ela_1_14( VOID );
extern	VOID CT_Im_Ela_1_15( VOID );
extern	VOID CT_Im_Ela_1_16( VOID );
extern	VOID CT_Im_Ela_1_17( VOID );
extern	VOID CT_Im_Ela_1_18( VOID );
extern	VOID CT_Im_Ela_1_19( VOID );
extern	VOID CT_Im_Ela_1_20( VOID );
extern	VOID CT_Im_Ela_1_21( VOID );
extern	VOID CT_Im_Ela_1_22( VOID );
extern	VOID CT_Im_Ela_1_23( VOID );
extern	VOID CT_Im_Ela_1_24( VOID );
extern	VOID CT_Im_Ela_1_25( VOID );
extern	VOID CT_Im_Ela_2_01( VOID );
extern	VOID CT_Im_Ela_2_02( VOID );
extern	VOID CT_Im_Ela_2_03( VOID );
extern	VOID CT_Im_Ela_2_04( VOID );
extern	VOID CT_Im_Ela_2_05( VOID );
extern	VOID CT_Im_Ela_2_06( VOID );
extern	VOID CT_Im_Ela_2_07( VOID );
extern	VOID CT_Im_Ela_2_08( VOID );
extern	VOID CT_Im_Ela_2_09( VOID );
extern	VOID CT_Im_Ela_2_10( VOID );
extern	VOID CT_Im_Ela_2_11( VOID );
extern	VOID CT_Im_Ela_2_12( VOID );
extern	VOID CT_Im_Ela_2_13( VOID );
extern	VOID CT_Im_Ela_2_14( VOID );
extern	VOID CT_Im_Ela_2_15( VOID );
extern	VOID CT_Im_Ela_2_16( VOID );
extern	VOID CT_Im_Ela_2_17( VOID );
extern	VOID CT_Im_Ela_2_18( VOID );
extern	VOID CT_Im_Ela_2_19( VOID );
extern	VOID CT_Im_Ela_2_20( VOID );
extern	VOID CT_Im_Ela_2_21( VOID );
extern	VOID CT_Im_Ela_2_22( VOID );
extern	VOID CT_Im_Ela_2_23( VOID );
extern	VOID CT_Im_Ela_2_24( VOID );
extern	VOID CT_Im_Ela_3_01( VOID );
extern	VOID CT_Im_Ela_3_02( VOID );
extern	VOID CT_Im_Ela_3_03( VOID );
extern	VOID CT_Im_Ela_3_04( VOID );
extern	VOID CT_Im_Ela_3_05( VOID );
extern	VOID CT_Im_Ela_3_06( VOID );
extern	VOID CT_Im_Ela_3_07( VOID );
extern	VOID CT_Im_Ela_3_08( VOID );
extern	VOID CT_Im_Ela_3_09( VOID );
extern	VOID CT_Im_Ela_3_10( VOID );
extern	VOID CT_Im_Ela_3_11( VOID );
extern	VOID CT_Im_Ela_3_12( VOID );
extern	VOID CT_Im_Ela_3_13( VOID );
extern	VOID CT_Im_Ela_3_14( VOID );
extern	VOID CT_Im_Ela_3_15( VOID );
extern	VOID CT_Im_Ela_3_16( VOID );
extern	VOID CT_Im_Ela_3_17( VOID );
extern	VOID CT_Im_Ela_3_18( VOID );
extern	VOID CT_Im_Ela_3_19( VOID );
extern	VOID CT_Im_Ela_3_20( VOID );
extern	VOID CT_Im_Ela_3_21( VOID );
extern	VOID CT_Im_Ela_3_22( VOID );
extern	VOID CT_Im_Ela_3_23( VOID );
extern	VOID CT_Im_Ela_3_24( VOID );
extern	VOID CT_Im_Ela_3_25( VOID );
extern	VOID CT_Im_Ela_3_26( VOID );
extern	VOID CT_Im_Ela_3_27( VOID );
extern	VOID CT_Im_Ela_3_28( VOID );
extern	VOID CT_Im_Ela_3_29( VOID );
extern	VOID CT_Im_Ela_3_30( VOID );
extern	VOID CT_Im_Ela_3_31( VOID );


extern	VOID	Ct_Im_Ela_Main(INT32 argc, CHAR** argv);

#endif
