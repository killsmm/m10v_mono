/**
 * @file		ct_dd_mcc.c
 * @brief		dd_mcc Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "driver_common.h"
#include "dd_mcc.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		Command main function for MCC test.
 * @param[in]	INT32 argc	:The number of parameters
 * @param[in]   CHAR** argv	:The value of parameters
 * @return 		None
 * @note		None
 * @attention   None
 *
 *			The meaning of parameters
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| P1          | P2 | P3     | P4	  | P5  |Meaning                             |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| display     | -  | -      | -       | -   |Display all registers in MCC.       |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 */
VOID Ct_Dd_MCC_Main(int argc, char** argv)
{
	INT32 dat;
	
	if( 0 == strcmp( argv[1], "display" ) ) {
		dat = Dd_MCC_Get_CA70_WFI();
		Ddim_Print(("CA70_WFI=%d\n", dat));
		dat = Dd_MCC_Get_CA71_WFI();
		Ddim_Print(("CA71_WFI=%d\n", dat));
		dat = Dd_MCC_Get_CA72_WFI();
		Ddim_Print(("CA72_WFI=%d\n", dat));
		dat = Dd_MCC_Get_CA73_WFI();
		Ddim_Print(("CA73_WFI=%d\n", dat));
		dat = Dd_MCC_Get_CA70_WFE();
		Ddim_Print(("CA70_WFE=%d\n", dat));
		dat = Dd_MCC_Get_CA71_WFE();
		Ddim_Print(("CA71_WFE=%d\n", dat));
		dat = Dd_MCC_Get_CA72_WFE();
		Ddim_Print(("CA72_WFE=%d\n", dat));
		dat = Dd_MCC_Get_CA73_WFE();
		Ddim_Print(("CA73_WFE=%d\n", dat));
	}
}
