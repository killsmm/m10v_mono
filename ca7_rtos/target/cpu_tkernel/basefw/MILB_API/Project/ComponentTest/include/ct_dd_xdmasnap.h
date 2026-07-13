/**
 * @file		ct_dd_xdmasnap.h
 * @brief		dd_xdmasnap Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _CT_DD_XDMASNAP_H_
#define _CT_DD_XDMASNAP_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/************************************/
/* Size definition                  */
/************************************/
#define D_XDMASNAP_AUTO_TEST_SIZE			(0x96000)

/************************************/
/* Address definition               */
/************************************/
#define D_XDMASNAP_SDRAM_BASE				(0x7B000000)

#define D_XDMASNAP_AUTO_TEST_ADR_SRC		(D_XDMASNAP_SDRAM_BASE)
#define D_XDMASNAP_AUTO_TEST_ADR_DST		(D_XDMASNAP_SDRAM_BASE + D_XDMASNAP_AUTO_TEST_SIZE)

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
extern	VOID Ct_Dd_XDMASNAP_Main(int argc, char** argv);

#endif
