/**
 * @file		iq_common.c
 * @brief		Common definition/macro/function for Image Quality functions.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// IQ Header
#include "iq_common.h"

// Driver Header
#include "ddim_typedef.h"



/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

USHORT BF_IQ_Dpc_Calc_Sta_y_Progr2Intr( const E_BF_IQ_FIELD odd_even, const ULONG progressive_sta_y )
{
	// Convert sta_y progressive to interlace.
	if( (progressive_sta_y %2) == 0 ) {
		return progressive_sta_y /2;			// Even value top and bottom
	}
	else {
		if( odd_even == E_BF_IQ_FIELD_EVEN ) {
			return (progressive_sta_y +1) /2;	// Roundup
		}
		else {
			return progressive_sta_y /2;		// Rounddown
		}
	}
}


USHORT BF_IQ_Dpc_Calc_Lines_Progr2Intr( const E_BF_IQ_FIELD odd_even, const ULONG progressive_sta_y, ULONG progressive_lines )
{
	// Convert height progressive to interlace
	if( (progressive_lines %2) == 0 ) {
		// Output lines is even.
		return progressive_lines /2;
	}
	else {
		// Output lines is odd.
		if( (progressive_sta_y %2) == 0 ) {
			// start line is even.
			if( odd_even == E_BF_IQ_FIELD_EVEN ) {
				return (progressive_lines +1) /2;	// Roundup
			}
			else {
				return progressive_lines /2;		// Rounddown
			}
		}
		else {
			// start line is odd.
			if( odd_even == E_BF_IQ_FIELD_EVEN ) {
				return progressive_lines /2;		// Rounddown
			}
			else {
				return (progressive_lines +1) /2;	// Roundup
			}
		}
	}
}

