/**
 * @file		ct_dd_hdmac1.h
 * @brief		This is ct header for dd_hdmac1.c
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CT_DD_HDMAC1_H_
#define _CT_DD_HDMAC1_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/************************************/
/* Size definition                  */
/************************************/
#define D_HDMAC1_AUTO_TEST_SIZE			(0x96000)

/************************************/
/* Address definition               */
/************************************/
#define D_HDMAC1_SDRAM_BASE				(0x7A000000)

#define D_HDMAC1_AUTO_TEST_ADR_SRC		(D_HDMAC1_SDRAM_BASE)
#define D_HDMAC1_AUTO_TEST_ADR_DST		(D_HDMAC1_SDRAM_BASE + D_HDMAC1_AUTO_TEST_SIZE)

#define D_HDMAC1_CPU_TARGET				(2)	// CPU1

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
extern	VOID Ct_Dd_HDMAC1_Main(int argc, char** argv);

#endif
