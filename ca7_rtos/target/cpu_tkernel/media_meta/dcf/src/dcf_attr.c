/**
 * @file		dcf_attr.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dcf_attr.h"

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
static volatile T_BF_DCF_ATTR_DCFDB_ATTR g_bf_dcf_attr_dcfdb = {
	0,
	0
};

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

VOID BF_Dcf_Attr_Set_Attr_Dupli_Ope( USHORT wValue )
{
	g_bf_dcf_attr_dcfdb.duplicate_operation = wValue;
}

USHORT BF_Dcf_Attr_Get_Attr_Dupli_Ope( VOID )
{
	return g_bf_dcf_attr_dcfdb.duplicate_operation;
}

VOID BF_Dcf_Attr_Set_Not_Add_Date_DB( UCHAR wValue )
{
	g_bf_dcf_attr_dcfdb.not_add_date_db = wValue;
}

UCHAR BF_Dcf_Attr_Get_Not_Add_Date_DB( VOID )
{
	return g_bf_dcf_attr_dcfdb.not_add_date_db;
}

