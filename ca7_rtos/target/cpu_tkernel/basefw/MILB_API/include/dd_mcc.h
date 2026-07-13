/**
 * @file		dd_mcc.h
 * @brief		MCC (Multi-Core Condition) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_mcc
@{
	- @ref dd_mcc_sample_section1
*/

#ifndef _DD_MCC_H_
#define _DD_MCC_H_

#include "arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define Dd_MCC_Get_CA70_WFI()		(1 & IO_MCC.MCSWFI.word)			/**< Get CA70 SWFI status */
#define Dd_MCC_Get_CA71_WFI()		(1 & (IO_MCC.MCSWFI.word >> 1))		/**< Get CA71 SWFI status */
#define Dd_MCC_Get_CA72_WFI()		(1 & (IO_MCC.MCSWFI.word >> 2))		/**< Get CA72 SWFI status */
#define Dd_MCC_Get_CA73_WFI()		(1 & (IO_MCC.MCSWFI.word >> 3))		/**< Get CA73 SWFI status */
#define Dd_MCC_Get_CA70_WFE()		(1 & (IO_MCC.MCSWFI.word >> 4))		/**< Get CA70 SWFE status */
#define Dd_MCC_Get_CA71_WFE()		(1 & (IO_MCC.MCSWFI.word >> 5))		/**< Get CA71 SWFE status */
#define Dd_MCC_Get_CA72_WFE()		(1 & (IO_MCC.MCSWFI.word >> 6))		/**< Get CA72 SWFE status */
#define Dd_MCC_Get_CA73_WFE()		(1 & (IO_MCC.MCSWFI.word >> 7))		/**< Get CA73 SWFE status */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif	// _DD_MCC_H_

/*@}*/

/**
@weakgroup dd_mcc
@{
	@section dd_mcc_sample_section1		Sample of "WSFI status of CPU CORE is read" using Dd_MCC_Get_CA70_WFI function.
    @code
	if( 0 == Dd_MCC_Get_CA70_WFI() ) {
		// Processing which is not in a WFI state
		// TODO
	}
	else {
		// Processing of a WFI state
		// TODO
	}
    @endcode
@}*/
