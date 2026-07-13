/**
 * @file		ct_im_xch.h
 * @brief		This is ct header for im_xch.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _CT_IM_XCH_H_
#define _CT_IM_XCH_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_CT_XCH_SRC_TOP_ADDR			(0x60000000)	// Source address
#define D_CT_XCH_DST_TOP_ADDR			(0x70000000)	// Destination address

#define D_CT_XCH_SRC_TOP_ADDR2			(0x68000000)	// Source address 2
#define D_CT_XCH_DST_TOP_ADDR2			(0x78000000)	// Destination address 2

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern VOID Ct_Im_Xch_Main( INT32 argc, CHAR** argv );
#endif
