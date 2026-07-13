/**
 * @file		fj_common.c
 * @brief		Custom Definition
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2016</I></B>
 */

#include "fj_common.h"
#include "system_attr.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
VOID FJ_Common_Set_Base_DateTime( T_FJ_COMMON_DATE_TIME* date_time )
{
	fj_setbasedatetime(date_time);
}

VOID FJ_Common_Get_Base_DateTime( T_FJ_COMMON_DATE_TIME* date_time )
{
	fj_getbasedatetime(date_time);
}

//VOID FJ_Common_Get_Cur_DateTime( T_FJ_COMMON_DATE_TIME* date_time )
//{
//	fj_getcurdatetime(date_time);
//}
//
//VOID FJ_Common_Get_Cur_DateTime_String( CHAR date_time_string[20] )
//{
//	fj_getcurdatetimestring(date_time_string);
//}

