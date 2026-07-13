/**
 * @file		dcf_attr.h
 * @brief		SocioNext Dcf Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_DCF_ATTR_H
#define _FJ_DCF_ATTR_H

#include "ddim_typedef.h"

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
// DCF Attribute Data
typedef struct {
	UCHAR duplicate_operation;
	UCHAR not_add_date_db;
}T_BF_DCF_ATTR_DCFDB_ATTR;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
extern VOID BF_Dcf_Attr_Set_Attr_Dupli_Ope( USHORT wValue );
extern USHORT BF_Dcf_Attr_Get_Attr_Dupli_Ope( VOID );
extern VOID BF_Dcf_Attr_Set_Not_Add_Date_DB( UCHAR wValue );
extern UCHAR BF_Dcf_Attr_Get_Not_Add_Date_DB( VOID );

#endif
