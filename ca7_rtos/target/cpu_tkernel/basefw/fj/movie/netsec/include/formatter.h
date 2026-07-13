/**
 * formatter.h
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#ifndef FORMATTER_H
#define FORMATTER_H

//#define CO_FORMATTER_AF_ON

extern VOID	fj_formatter_convert_initialize( VOID );
extern FJ_ERR_CODE	fj_formatter_convert_set_stream( UINT32 stid, UINT32 max_out_len, T_FJ_FORMATTER_PID* pid );
extern FJ_ERR_CODE	fj_formatter_convert_set_program( UINT32 stid, UINT32 max_out_len, T_FJ_FORMATTER_PID* pid );
extern FJ_ERR_CODE	fj_formatter_convert_pes_send0( UINT32 stid, UINT32 dtid, T_FJ_FORMATTER_DATA* in_data, ULONG wait_time );
extern FJ_ERR_CODE	fj_formatter_convert_pes_send1( UINT32 stid, UINT32 dtid, T_FJ_FORMATTER_DATA* in_data, ULONG wait_time );
extern FJ_ERR_CODE	fj_formatter_convert_ts_recv0( T_FJ_FORMATTER_DATA* out_data, ULONG* out_len, UINT32* stid, UINT32* dtid, UINT32* lti, ULONG wait_time );
extern FJ_ERR_CODE	fj_formatter_convert_ts_recv1( T_FJ_FORMATTER_DATA* out_data, ULONG* out_len, UINT32* stid, UINT32* dtid, UINT32* lti, ULONG wait_time );
extern VOID	fj_formatter_convert_finalize( VOID );

#endif /* FORMATTER_H */
