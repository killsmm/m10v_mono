/**
 * formatter.c
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#include "pfdep.h"
#include <stdio.h>
#include <string.h>
//#include "dd_gic.h"
#include "memory.h"
#include "os_user_custom.h"

#include "ogma_internal.h"
#include "ogma_basic_access.h"
#include "ogma_api.h"
#include "ogma_reg.h"
#include "ogma_driver_api.h"
#include "pfdep.h"

#include "dd_exs.h"
#include "dd_gpv.h"

#include "fj_formatter.h"
#include "formatter.h"

#include "l1l2cache.h"

#include "sdram_map_movie_record.h"
#include "sdram_map_cache_define.h"

VOID	fj_formatter_convert_initialize( VOID ){
	T_BF_NETSEC_INIT_PARAM             init_param;
	T_BF_NETSEC_SESSION_BASIC_DB_PARAM session_basic_db_param;
	
#if 1
	Dd_Exs_Set_RAM_PD_Ctrl_NETSEC( 0 );
	Dd_Exs_Set_NETSEC_PTPCLK_Stop_Ctrl_NETSEC_PTPSTP( 0 );
	Dd_Exs_Set_NETSEC_Soft_Reset( 1 );
//	Dd_GPV_Set_ARC_Remap( D_DD_GPV_REMAP_NETSEC, 1 );
#else
	pfdep_iomem_write((void *)(0x1b110040), 0x7d7); // RAM PD Control Register
    pfdep_iomem_write((void *)(0x1b110070), 0x0);   // NETSEC PTP Clock Stop Control Register
    pfdep_iomem_write((void *)(0x1b11000c), 0x107); // Soft Reset Register
//  pfdep_iomem_write((void *)(0x19000000), 0xff);  // EXS-NIC Remap
//	Dd_GIC_Ctrl(E_DD_GIC_INTID_NETSEC_A_INT,1, D_DD_GIC_PRI30, 0x1);
//	Dd_GIC_Ctrl(E_DD_GIC_INTID_NETSEC_B_INT,1, D_DD_GIC_PRI30, 0x1);
#endif

	//----------------------------------------------
	// When SDRAM map has been determined
	// Please implement
	init_param.mem_start_addr          = Sdram_Map_Movie_Record_Get_TS_Work1();
	init_param.dma_mem_cpu_start_addr  = M_SDRAM_MAP_COMMON_ADR_PHY2LOG( Sdram_Map_Movie_Record_Get_TS_Work0() );
	init_param.dma_mem_phys_start_addr = Sdram_Map_Movie_Record_Get_TS_Work0();
	//----------------------------------------------
	init_param.ts0_tx_desc_ring_num    = 64;
	init_param.ts1_tx_desc_ring_num    = 64;
	init_param.lpb0_rx_desc_ring_num   = 64;
	init_param.lpb1_rx_desc_ring_num   = 64;
	init_param.lpb0_buf_size =	3948;
	init_param.lpb1_buf_size =	3948;
#ifdef CO_NETSEC_AF_ON
	init_param.ppm =  1;
#else
	init_param.ppm =  0;
#endif

	/* Initialize */
	BF_Netsec_init( &init_param );

	/* Set Session Basic DB */
    pfdep_memset(&session_basic_db_param, 0, sizeof(session_basic_db_param));
	session_basic_db_param.id = 14;
	BF_Netsec_ctrl(NETSEC_CTL_SET_SESSION_BASIC_DB, &session_basic_db_param);

}

FJ_ERR_CODE	fj_formatter_convert_set_stream ( UINT32 stid, UINT32 max_out_len, T_FJ_FORMATTER_PID* pid ){

	T_BF_NETSEC_SESSION_DB_MPTS_ST_PARAM  session_db_param;

	/* Set Session DB */
    memset(&session_db_param, 0, sizeof(session_db_param));
	session_db_param.id = stid;
	session_db_param.payload_length = max_out_len;
	session_db_param.pid_psi0 = 0;
	session_db_param.pid_psi1 = 0;
	session_db_param.pid_psi2 = 0;
	session_db_param.pid_psi3 = 0;
	BF_Netsec_ctrl(NETSEC_CTL_SET_SESSION_DB_MPTS_ST, &session_db_param);

	return FJ_ERR_OK;
}

FJ_ERR_CODE	fj_formatter_convert_set_program ( UINT32 stid, UINT32 max_out_len, T_FJ_FORMATTER_PID* pid ){

	T_BF_NETSEC_SESSION_DB_MPTS_PG_PARAM session_db_param;

	/* Set Session DB */
    memset(&session_db_param, 0, sizeof(session_db_param));
	session_db_param.id = stid;
	session_db_param.payload_length = max_out_len;
	session_db_param.pid_pmt   = pid->pid_pmt;
	session_db_param.pid_pcr   = pid->pid_pcr;
	session_db_param.pid_video = pid->pid_video;
	session_db_param.pid_audio = pid->pid_audio;
	session_db_param.pid_mtdat = pid->pid_mtdat;
	BF_Netsec_ctrl(NETSEC_CTL_SET_SESSION_DB_MPTS_PG, &session_db_param);

	return FJ_ERR_OK;
}

FJ_ERR_CODE	fj_formatter_convert_pes_send0( UINT32 stid, UINT32 dtid, T_FJ_FORMATTER_DATA* in_data, ULONG wait_time ){
	T_BF_NETSEC_PROC_STRM_PARAM        proc_strm_param;
	ogma_err_t                         result;
	FJ_ERR_CODE                        ER;
	
	proc_strm_param.in_addr    = (void*)in_data->addr;
	proc_strm_param.in_len	   = in_data->size;
	proc_strm_param.mark       = 0;
	proc_strm_param.lpbr       = 1;
	proc_strm_param.lpbe       = 0;
	proc_strm_param.ast_int    = 0;
	proc_strm_param.cr         = 1;
	proc_strm_param.fn		   = 0;
	proc_strm_param.si         = 1;
	proc_strm_param.enc        = 0;
	proc_strm_param.stid       = stid;
	proc_strm_param.dtid       = dtid;
	proc_strm_param.timestamp  = 0;
	proc_strm_param.wait_time  = wait_time;
	
	// Send PES data to NETSEC
	result = BF_Netsec_ctrl(NETSEC_CTL_PROC_STRM_DATA0, &proc_strm_param);
	if( result == OGMA_ERR_OK ){
		ER = FJ_ERR_OK;
	}
	else if( result == OGMA_ERR_BUSY ){
		ER = FJ_ERR_NG_TMOUT;
	}
	else{
		ER = FJ_ERR_NG;
	}
	
	return ER;
}

FJ_ERR_CODE	fj_formatter_convert_pes_send1( UINT32 stid, UINT32 dtid, T_FJ_FORMATTER_DATA* in_data, ULONG wait_time ){
	T_BF_NETSEC_PROC_STRM_PARAM        proc_strm_param;
	ogma_err_t                         result;
	FJ_ERR_CODE                        ER;
	
	proc_strm_param.in_addr    = (void*)in_data->addr;
	proc_strm_param.in_len	   = in_data->size;
	proc_strm_param.mark       = 0;
	proc_strm_param.lpbr       = 1;
	proc_strm_param.lpbe       = 0;
	proc_strm_param.ast_int    = 0;
	proc_strm_param.cr         = 1;
	proc_strm_param.fn		   = 0;
	proc_strm_param.si         = 1;
	proc_strm_param.enc        = 0;
	proc_strm_param.stid       = stid;
	proc_strm_param.dtid       = dtid;
	proc_strm_param.timestamp  = 0;
	proc_strm_param.wait_time  = wait_time;
	
	// Send PES data to NETSEC
	result = BF_Netsec_ctrl(NETSEC_CTL_PROC_STRM_DATA1, &proc_strm_param);
	if( result == OGMA_ERR_OK ){
		ER = FJ_ERR_OK;
	}
	else if( result == OGMA_ERR_BUSY ){
		ER = FJ_ERR_NG_TMOUT;
	}
	else{
		ER = FJ_ERR_NG;
	}
	
	return ER;
}

FJ_ERR_CODE	fj_formatter_convert_ts_recv0( T_FJ_FORMATTER_DATA* out_data, ULONG* out_len, UINT32* stid, UINT32* dtid, UINT32* lti, ULONG wait_time ){
	T_BF_NETSEC_GET_STRM_PARAM         get_strm_param;
	ogma_err_t                         result;
	FJ_ERR_CODE                        ER;
	
	get_strm_param.out_addr = (void*)out_data->addr;
	get_strm_param.wait_time = wait_time;
	
	// Receive TS data
	result = BF_Netsec_ctrl(NETSEC_CTL_GET_STRM_DATA0, &get_strm_param);
	if( result == OGMA_ERR_OK ){
		ER = FJ_ERR_OK;
	}
	else if( result == OGMA_ERR_DATA ){
		ER = FJ_ERR_NG_TMOUT;
	}
	else{
		ER = FJ_ERR_NG;
	}
	
	*out_len = get_strm_param.out_len;
	*stid    = get_strm_param.stid;
	*dtid    = get_strm_param.dtid;
	*lti     = get_strm_param.lti;

	return ER;
}

FJ_ERR_CODE	fj_formatter_convert_ts_recv1( T_FJ_FORMATTER_DATA* out_data, ULONG* out_len, UINT32* stid, UINT32* dtid, UINT32* lti, ULONG wait_time ){
	T_BF_NETSEC_GET_STRM_PARAM         get_strm_param;
	ogma_err_t                         result;
	FJ_ERR_CODE                        ER;
	
	get_strm_param.out_addr = (void*)out_data->addr;
	get_strm_param.wait_time = wait_time;
	
	// Receive TS data
	result = BF_Netsec_ctrl(NETSEC_CTL_GET_STRM_DATA1, &get_strm_param);
	if( result == OGMA_ERR_OK ){
		ER = FJ_ERR_OK;
	}
	else if( result == OGMA_ERR_DATA ){
		ER = FJ_ERR_NG_TMOUT;
	}
	else{
		ER = FJ_ERR_NG;
	}
	
	*out_len = get_strm_param.out_len;
	*stid    = get_strm_param.stid;
	*dtid    = get_strm_param.dtid;
	*lti     = get_strm_param.lti;

	return ER;
}

VOID	fj_formatter_convert_finalize( VOID ){
	// Terminates
	BF_Netsec_terminate();
	
	Dd_Exs_Set_NETSEC_Soft_Reset( 0 );
	Dd_Exs_Set_NETSEC_PTPCLK_Stop_Ctrl_NETSEC_PTPSTP( 1 );
	Dd_Exs_Set_RAM_PD_Ctrl_NETSEC( 1 );
//	Dd_GPV_Set_ARC_Remap( D_DD_GPV_REMAP_NETSEC, 0 );
}

