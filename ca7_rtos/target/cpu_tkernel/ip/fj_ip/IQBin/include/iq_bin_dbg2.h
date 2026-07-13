
#ifndef _IQ_BIN_DBG2_H_
#define _IQ_BIN_DBG2_H_

#include "ddim_typedef.h"
#include "share.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_IQ_DBG2_DATA_TYPE_NONE = 0,
	E_IQ_DBG2_DATA_TYPE_SEN,
	E_IQ_DBG2_DATA_TYPE_SEN_TBL,
	E_IQ_DBG2_DATA_TYPE_SRO,
	E_IQ_DBG2_DATA_TYPE_B2B,
	E_IQ_DBG2_DATA_TYPE_B2R,
	E_IQ_DBG2_DATA_TYPE_LTM,
	E_IQ_DBG2_DATA_TYPE_R2Y,
	E_IQ_DBG2_DATA_TYPE_CNR,
	E_IQ_DBG2_DATA_TYPE_SRO_TBL,
	E_IQ_DBG2_DATA_TYPE_B2B_TBL,
	E_IQ_DBG2_DATA_TYPE_B2R_TBL,
	E_IQ_DBG2_DATA_TYPE_LTM_TBL,
	E_IQ_DBG2_DATA_TYPE_R2Y_TBL,
	E_IQ_DBG2_DATA_TYPE_MAX = 32,
} E_IQ_DBG2_DATA_TYPE;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define IQ_DBG2_CONV_FLG( a )			( 1 << a )
#define IQ_DBG2_SET_FLG( a, b )		( a | b )
#define IQ_DBG2_CLR_FLG( a, b )		( a & (~b) )

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
extern INT32 IQ_Dbg2_Enable( BOOL enable );
extern INT32 IQ_Dbg2_Init( void );
extern INT32 IQ_Dbg2_Assign_Data( const char* name, E_IQ_DBG2_DATA_TYPE type, UCHAR pipe_no );
extern INT32 IQ_Dbg2_Set_Address( ULONG address, ULONG* next_address, BOOL r2y_tbl_dynamic );
extern INT32 IQ_Dbg2_Set_Data_for_Pipe_Direct( void );
extern INT32 IQ_Dbg2_Start( void );
extern INT32 IQ_Dbg2_Set_Data( const char* name, E_IQ_DBG2_DATA_TYPE type, UCHAR pipe_no );
extern INT32 IQ_Dbg2_Wait_End( INT32 timeout_ms );
extern INT32 IQ_Dgb2_Get_Head( ULONG* addr, ULONG* size, ULONG* total );
extern INT32 IQ_Dbg2_Save_File( void );

#endif
