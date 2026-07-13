/**
 * @file		iq_dpc_line_order.h
 * @brief		line order information of Frame readout mode
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_LINE_ORDER_H__
#define __IQ_DPC_LINE_ORDER_H__

#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

// IQ_DPC_LINE_ORDER_IN_FIELDS
typedef struct {
	UINT32 ui_table_num;						/**< array number of table				*/
	const USHORT* pus_line_order_in_field_tbl;	/**< line number table					*/
} T_FJ_IQ_DPC_LINE_ORDER_IN_FIELDS;

// T_FJ_IQ_DPC_LINE_ORDER
typedef struct {
	USHORT us_field_separate_row_cycle;										/**< block for separate lines		*/
	USHORT us_line_order_tbl_num;											/**< num of line separate order table array */
	const T_FJ_IQ_DPC_LINE_ORDER_IN_FIELDS* pt_line_order_in_fields_tbl;	/**< line separate order table addr	*/
} T_FJ_IQ_DPC_LINE_ORDER;


/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
// nothing special.


#endif /* __IQ_DPC_LINE_ORDER_H__ */

