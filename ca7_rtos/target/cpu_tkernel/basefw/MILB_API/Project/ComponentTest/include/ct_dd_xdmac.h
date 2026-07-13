/**
 * @file		ct_dd_xdmac.h
 * @brief		dd_xdmac Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _CT_DD_XDMAC_H_
#define _CT_DD_XDMAC_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/************************************/
/* Size definition                  */
/************************************/
#define D_XDMAC_AUTO_TEST_SIZE			(0x96000)

/************************************/
/* Address definition               */
/************************************/
#define D_XDMAC_SDRAM_BASE				(0x7B000000)

#define D_XDMAC_AUTO_TEST_ADR_SRC		(D_XDMAC_SDRAM_BASE)
#define D_XDMAC_AUTO_TEST_ADR_DST		(D_XDMAC_SDRAM_BASE + D_XDMAC_AUTO_TEST_SIZE)

#define D_XDMAC_AUTO_TEST_INTMEM		(0x18000000)

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
extern	VOID Ct_Dd_XDMAC_Main(int argc, char** argv);

#endif
