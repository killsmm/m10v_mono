/**
 * @file		ct_im_disp.h
 * @brief		This is ct header for im_disp.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef _CT_IM_DISP_H_
#define _CT_IM_DISP_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define CO_DEBUG_DISP
#define CO_DEBUG_PRINT_BEFORE_SETTING
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_CT_IM_DISP_SET_GAMMA_R_FULL =					0x001,	/**< Set r_full Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_G_FULL =					0x002,	/**< Set g_full Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_B_FULL =					0x004,	/**< Set b_full Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_ALL_FULL =				0x007,	/**< Set all full Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_R_DELTA_FULL =			0x008,	/**< Set r_delta Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_G_DELTA_FULL =			0x010,	/**< Set g_delta Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_B_DELTA_FULL =			0x020,	/**< Set b_delta Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_ALL_DELTA_FULL =			0x038,	/**< Set all delta Gamma*/ 

	E_CT_IM_DISP_SET_GAMMA_REVERSE_R_FULL =			0x040,	/**< Set reverse r_full Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_REVERSE_G_FULL =			0x080,	/**< Set reverse g_full Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_REVERSE_B_FULL =			0x100,	/**< Set reverse b_full Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_REVERSE_ALL_FULL =		0x1C0,	/**< Set reverse all full Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_REVERSE_R_DELTA_FULL =	0x200,	/**< Set reverse r_delta Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_REVERSE_G_DELTA_FULL =	0x400,	/**< Set reverse g_delta Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_REVERSE_B_DELTA_FULL =	0x800,	/**< Set reverse b_delta Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_REVERSE_ALL_DELTA_FULL =	0xE00,	/**< Set reverse all delta Gamma*/ 
	E_CT_IM_DISP_SET_GAMMA_ALL =					0xFFF,	/**< Set all */ 

}E_CT_IM_DISP_SET_GAMMA;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
void Ct_Im_DISP_Main(INT32 argc, CHAR** argv);

void Ct_Im_DISP_PcTest_Main(void);


#endif
