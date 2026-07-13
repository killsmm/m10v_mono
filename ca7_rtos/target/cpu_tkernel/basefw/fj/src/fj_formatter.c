/**
 * @file		fj_formatter.c
 * @brief		NETSEC API.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_formatter.h"

#include "formatter.h"
/******************************************************************************
 Program
******************************************************************************/
VOID FJ_Formatter_Convert_Initialize( VOID ){
	fj_formatter_convert_initialize();
}

FJ_ERR_CODE	FJ_Formatter_Convert_Set_Stream( UINT32 stid, UINT32 max_out_len, T_FJ_FORMATTER_PID* pid ){
	return fj_formatter_convert_set_stream( stid, max_out_len, pid );
}

FJ_ERR_CODE	FJ_Formatter_Convert_Set_Program( UINT32 stid, UINT32 max_out_len, T_FJ_FORMATTER_PID* pid ){
	return fj_formatter_convert_set_program( stid, max_out_len, pid );
}

FJ_ERR_CODE FJ_Formatter_Convert_PES_Send0( UINT32 stid, UINT32 dtid, T_FJ_FORMATTER_DATA* in_data, ULONG wait_time ){
	return fj_formatter_convert_pes_send0( stid, dtid, in_data, wait_time );
}

FJ_ERR_CODE FJ_Formatter_Convert_PES_Send1( UINT32 stid, UINT32 dtid, T_FJ_FORMATTER_DATA* in_data, ULONG wait_time ){
	return fj_formatter_convert_pes_send1( stid, dtid, in_data, wait_time );
}

FJ_ERR_CODE FJ_Formatter_Convert_TS_Recv0( T_FJ_FORMATTER_DATA* out_data, ULONG* out_len, UINT32* stid, UINT32* dtid, UINT32* lti, ULONG wait_time ){
	return fj_formatter_convert_ts_recv0( out_data, out_len, stid, dtid, lti, wait_time );
}

FJ_ERR_CODE FJ_Formatter_Convert_TS_Recv1( T_FJ_FORMATTER_DATA* out_data, ULONG* out_len, UINT32* stid, UINT32* dtid, UINT32* lti, ULONG wait_time ){
	return fj_formatter_convert_ts_recv1( out_data, out_len, stid, dtid, lti, wait_time );
}

VOID FJ_Formatter_Convert_Finalize( VOID ){
	fj_formatter_convert_finalize();
}

