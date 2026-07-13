/**
 * @file		ct_im_raw.h
 * @brief		This is ct header for im_raw.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _CT_IM_RAW_H_
#define _CT_IM_RAW_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_CT_IM_RAW_ORG_ADDR	(0x60000000)
#define D_CT_IM_RAW_ENC_ADDR	(0x62000000)
#define D_CT_IM_RAW_DEC_ADDR	(0x64000000)

#define D_CT_IM_RAW_INPUT_BAYER_0_WIDTH		(4704)		// 4680(+24)
#define D_CT_IM_RAW_INPUT_BAYER_0_LINES		(3500)
#define D_CT_IM_RAW_INPUT_BAYER_1_WIDTH		(1536)		// 1536
#define D_CT_IM_RAW_INPUT_BAYER_1_LINES		(1152)
#define D_CT_IM_RAW_INPUT_BAYER_2_WIDTH		(2304)		// 2304
#define D_CT_IM_RAW_INPUT_BAYER_2_LINES		(1296)
#define D_CT_IM_RAW_INPUT_Y_0_WIDTH			(640)		// 640
#define D_CT_IM_RAW_INPUT_Y_0_LINES			(480)

#define D_CT_IM_RAW_INPUT_BAYER_0_SIZ		(D_CT_IM_RAW_INPUT_BAYER_0_WIDTH * D_CT_IM_RAW_INPUT_BAYER_0_LINES * 3 / 2)	// 12bit-Pack
#define D_CT_IM_RAW_INPUT_BAYER_1_SIZ		(D_CT_IM_RAW_INPUT_BAYER_1_WIDTH * D_CT_IM_RAW_INPUT_BAYER_1_LINES * 2)		// 16bit-NonPack
#define D_CT_IM_RAW_INPUT_BAYER_2_SIZ		(D_CT_IM_RAW_INPUT_BAYER_2_WIDTH * D_CT_IM_RAW_INPUT_BAYER_2_LINES * 2)		// 16bit-NonPack
#define D_CT_IM_RAW_INPUT_Y_0_SIZ			(D_CT_IM_RAW_INPUT_Y_0_WIDTH * D_CT_IM_RAW_INPUT_Y_0_LINES)					// Y


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
void Ct_Im_RAW_Main(int argc, char** argv);

#ifdef CO_DEBUG_ON_PC
extern	VOID	Ct_Im_RAW_Main_PC(int argc, char** argv);
#endif // CO_DEBUG_ON_PC
extern UINT32	Ct_Im_RAW_Get_Enc_Size( VOID );

#endif
