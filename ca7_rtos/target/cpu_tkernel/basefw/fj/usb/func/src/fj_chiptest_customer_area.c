/**
 * @file		fj_chiptest_customer_area.c
 * @brief		reserve memory for ChipTest
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

/*--------------------------------------------------------------------------
Include Files
--------------------------------------------------------------------------*/
#include "ddim_typedef.h"
#include "fj_usbfunc.h"


/*--------------------------------------------------------------------------
Types/Constants
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Variables
--------------------------------------------------------------------------*/
//#pragma arm section rwdata="CHIPTEST_CUSTOMER_AREA"

/* customer area */
volatile BYTE gCustomerArea[CHIP_TEST_RESERVED_SIZE];



/*--------------------------------------------------------------------------
Functions
--------------------------------------------------------------------------*/
