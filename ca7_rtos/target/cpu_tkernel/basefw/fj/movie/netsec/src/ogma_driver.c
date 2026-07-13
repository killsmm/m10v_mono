/**
 * ogma_driver.c
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#include "pfdep.h"
#include <stdio.h>
#include <string.h>
#include "dd_gic.h"
#include "memory.h"
#include "os_user_custom.h"
#include "debug.h"
#include "fj_peripheral.h"

#include "ogma_api.h"
#include "ogma_reg.h"
#include "ogma_internal.h"
#include "ogma_basic_access.h"
#include "ogma_driver_api.h"

#undef STATIC
#ifdef MODULE_TEST
#define STATIC
#else
#define STATIC static
#endif

STATIC ogma_handle_t netsec_ogma_handle;
STATIC ogma_uint32  netsec_desc_ring_ts0_num      = 64;
STATIC ogma_uint32  netsec_desc_ring_ts1_num      = 64;
STATIC ogma_uint32  netsec_desc_ring_lpb0_rx_num =  64;
STATIC ogma_uint32  netsec_desc_ring_lpb1_rx_num =  64;

STATIC void BF_Netsec_set_desc_ring_param(ogma_param_t *param);

STATIC ogma_err_t BF_Netsec_send_strm_data(ogma_handle_t ogma_handle,
										   T_BF_NETSEC_PROC_STRM_PARAM *strm_param_p); 

STATIC ogma_err_t BF_Netsec_send_strm_data_sub(ogma_handle_t ogma_handle,
											   ogma_strm_ctrl_t *strm_ctrl_p);

STATIC ogma_err_t BF_Netsec_send_strm_data_lpb0(ogma_handle_t ogma_handle,
												T_BF_NETSEC_PROC_STRM_PARAM *strm_param_p); 

STATIC ogma_err_t BF_Netsec_send_strm_data_lpb1(ogma_handle_t ogma_handle,
												T_BF_NETSEC_PROC_STRM_PARAM *strm_param_p); 

STATIC ogma_err_t BF_Netsec_get_strm_data(ogma_handle_t ogma_handle,
										  ogma_desc_ring_id_t drid,
										  T_BF_NETSEC_GET_STRM_PARAM *get_strm_param_p);

STATIC ogma_err_t BF_Netsec_set_session_basic_db(ogma_handle_t ogma_handle,
												 T_BF_NETSEC_SESSION_BASIC_DB_PARAM *sbdb_param_p);

STATIC ogma_err_t BF_Netsec_set_session_db(ogma_handle_t ogma_handle,
										   T_BF_NETSEC_SESSION_DB_PARAM *sdb_param_p);

STATIC ogma_err_t BF_Netsec_set_session_db_mpts_st(ogma_handle_t ogma_handle,
												   T_BF_NETSEC_SESSION_DB_MPTS_ST_PARAM *sdb_param_p);

STATIC ogma_err_t BF_Netsec_set_session_db_mpts_pg(ogma_handle_t ogma_handle,
												   T_BF_NETSEC_SESSION_DB_MPTS_PG_PARAM *sdb_param_p);

STATIC ogma_err_t BF_Netsec_set_lpb_rx_desc(ogma_handle_t ogma_handle,
											ogma_uint32 channel,
											ogma_uint32 payload_length);

STATIC ogma_err_t BF_Netsec_start_all_desc_ring_strm(ogma_handle_t ogma_handle);

STATIC ogma_err_t BF_Netsec_stop_all_desc_ring_strm(ogma_handle_t ogma_handle);

STATIC ogma_uint32 BF_Netsec_drid_to_wait_flag (ogma_uint32 drid);

STATIC ogma_uint32 BF_Netsec_drid_to_semaphoe_id (ogma_desc_ring_id_t drid);

void BF_Netsec_init (T_BF_NETSEC_INIT_PARAM *init_param_p)
{
	void *base_addr;
	ogma_param_t param;
	ogma_handle_t ogma_handle;	
	ogma_err_t ogma_err = OGMA_ERR_OK;

/*---------------*/
/* set base addr */
/*---------------*/
	base_addr = (void *)OGMA_CONFIG_BASE_ADDR;
	pfdep_memset(&param,0,sizeof(param));

	pfdep_reset_mem_pool(init_param_p->mem_start_addr,
						 init_param_p->dma_mem_cpu_start_addr,
						 init_param_p->dma_mem_phys_start_addr);
	
	netsec_desc_ring_ts0_num	 = init_param_p->ts0_tx_desc_ring_num;
	netsec_desc_ring_ts1_num	 = init_param_p->ts1_tx_desc_ring_num;
	netsec_desc_ring_lpb0_rx_num = init_param_p->lpb0_rx_desc_ring_num;
	netsec_desc_ring_lpb1_rx_num = init_param_p->lpb1_rx_desc_ring_num;
	
/*-----------------------------*/
/* set desc ring num and valid */
/*-----------------------------*/
	BF_Netsec_set_desc_ring_param(&param);

	param.pkt_ctrl_param.ets       = OGMA_TRUE;
	param.pkt_ctrl_param.vjs       = OGMA_FALSE;
	param.pkt_ctrl_param.en_jumbo  = OGMA_FALSE;
	param.pkt_ctrl_param.en_id_rep = OGMA_FALSE;
	param.pkt_ctrl_param.tts       = OGMA_FALSE;
	param.pkt_ctrl_param.rfm       = OGMA_FALSE;
	param.pkt_ctrl_param.jrm       = OGMA_FALSE;
	param.pkt_ctrl_param.ppm       = init_param_p->ppm;
	
/*-----------*/
/* omga_init */
/*-----------*/
	if((ogma_err = ogma_init(base_addr,
							 NULL,
							 &param,
							 &ogma_handle)) != OGMA_ERR_OK) {
		BF_Debug_Print_Error(("ogma_init failed with error code : %d\n", ogma_err));
	}
	
	netsec_ogma_handle = ogma_handle;


	ogma_err = BF_Netsec_set_lpb_rx_desc(ogma_handle,
										 OGMA_DESC_RING_ID_LPBR_RX,
										 init_param_p->lpb0_buf_size);
				
	ogma_err = BF_Netsec_set_lpb_rx_desc(ogma_handle,
										 OGMA_DESC_RING_ID_LPBE_RX,
										 init_param_p->lpb1_buf_size);

	ogma_enable_top_irq(ogma_handle,
						OGMA_TOP_IRQ_REG_PKT|OGMA_TOP_IRQ_REG_LPBR_RX|OGMA_TOP_IRQ_REG_LPBE_RX);

	ogma_enable_pkt_irq(ogma_handle,
						OGMA_PKT_IRQ_REG_TX_DONE_TS|OGMA_PKT_IRQ_REG_TX_DONE_Vsub);
 
	BF_Netsec_start_all_desc_ring_strm(ogma_handle);
	
}

void BF_Netsec_driver_init (void)
{
	void *base_addr;
	ogma_param_t param;
	ogma_handle_t ogma_handle;	
	ogma_err_t ogma_err = OGMA_ERR_OK;

/*---------------*/
/* set base addr */
/*---------------*/
	base_addr = (void *)OGMA_CONFIG_BASE_ADDR;
	pfdep_memset(&param,0,sizeof(param));
	
/*-----------------------------*/
/* set desc ring num and valid */
/*-----------------------------*/
	BF_Netsec_set_desc_ring_param(&param);

	param.pkt_ctrl_param.ets       = OGMA_FALSE;
	param.pkt_ctrl_param.vjs       = OGMA_FALSE;
	param.pkt_ctrl_param.en_jumbo  = OGMA_FALSE;
	param.pkt_ctrl_param.en_id_rep = OGMA_FALSE;
	param.pkt_ctrl_param.tts       = OGMA_FALSE;
	param.pkt_ctrl_param.rfm       = OGMA_FALSE;
	param.pkt_ctrl_param.jrm       = OGMA_FALSE;

/*-----------*/
/* omga_init */
/*-----------*/
	if((ogma_err = ogma_soft_init(base_addr,
								  NULL,
								  &param,
								  &ogma_handle)) != OGMA_ERR_OK) {
		BF_Debug_Print_Error(("ogma_soft_init failed with error code : %d\n", ogma_err));
	}
	
	netsec_ogma_handle = ogma_handle;

	ogma_enable_top_irq(ogma_handle,
						OGMA_TOP_IRQ_REG_PKT|OGMA_TOP_IRQ_REG_LPBR_RX);

	ogma_enable_top_irq(ogma_handle,
						OGMA_TOP_IRQ_REG_PKT|OGMA_TOP_IRQ_REG_LPBE_RX);

	ogma_enable_pkt_irq(ogma_handle,
						OGMA_PKT_IRQ_REG_TX_DONE_TS);

	ogma_enable_pkt_irq(ogma_handle,
						OGMA_PKT_IRQ_REG_TX_DONE_Vsub);

	BF_Netsec_start_all_desc_ring_strm(ogma_handle);
	
}

void BF_Netsec_terminate (void)
{
	BF_Netsec_stop_all_desc_ring_strm(netsec_ogma_handle);
	
	ogma_terminate(netsec_ogma_handle);
}

void BF_Netsec_driver_terminate (void)
{
	
}

ogma_err_t BF_Netsec_ctrl (
    unsigned int request,
    void        *argp
	)
{
	ogma_err_t err = OGMA_ERR_OK;

	if(netsec_ogma_handle == NULL) {
		return OGMA_ERR_INVALID;
	}
	
	switch (request) {
    case NETSEC_CTL_SET_SESSION_BASIC_DB:
		err = BF_Netsec_set_session_basic_db(netsec_ogma_handle, 
											 (T_BF_NETSEC_SESSION_BASIC_DB_PARAM *)argp);
        break;

    case NETSEC_CTL_SET_SESSION_DB:
        err = BF_Netsec_set_session_db(netsec_ogma_handle, 
									   (T_BF_NETSEC_SESSION_DB_PARAM *)argp);
        break;

	case NETSEC_CTL_SET_SESSION_DB_MPTS_ST:
        err = BF_Netsec_set_session_db_mpts_st(netsec_ogma_handle, 
											  (T_BF_NETSEC_SESSION_DB_MPTS_ST_PARAM *)argp);
        break;

	case NETSEC_CTL_SET_SESSION_DB_MPTS_PG:
        err = BF_Netsec_set_session_db_mpts_pg(netsec_ogma_handle, 
											  (T_BF_NETSEC_SESSION_DB_MPTS_PG_PARAM *)argp);
        break;

    case NETSEC_CTL_PROC_STRM_DATA_RAW:
        err = BF_Netsec_send_strm_data(netsec_ogma_handle,
									   (T_BF_NETSEC_PROC_STRM_PARAM *)argp);
        break;

	case NETSEC_CTL_PROC_STRM_DATA0:
		
        err = BF_Netsec_send_strm_data_lpb0(netsec_ogma_handle,
											(T_BF_NETSEC_PROC_STRM_PARAM *)argp);
        break;

	case NETSEC_CTL_PROC_STRM_DATA1:
		
        err = BF_Netsec_send_strm_data_lpb1(netsec_ogma_handle,
										   (T_BF_NETSEC_PROC_STRM_PARAM *)argp);
        break;

	case NETSEC_CTL_GET_STRM_DATA0:
        err = BF_Netsec_get_strm_data(netsec_ogma_handle,
									  OGMA_DESC_RING_ID_LPBR_RX,
									  (T_BF_NETSEC_GET_STRM_PARAM *)argp);
        break;

	case NETSEC_CTL_GET_STRM_DATA1:
        err = BF_Netsec_get_strm_data(netsec_ogma_handle,
									  OGMA_DESC_RING_ID_LPBE_RX,
									  (T_BF_NETSEC_GET_STRM_PARAM *)argp);
        break;

    default:
        err = OGMA_ERR_PARAM;
		break;
	}

	return err;
}

void BF_Netsec_set_desc_ring_param(ogma_param_t *param) 
{
    param->desc_ring_param[OGMA_DESC_RING_ID_TS_TX].valid_flag    = OGMA_TRUE;
    param->desc_ring_param[OGMA_DESC_RING_ID_TS_TX].entry_num     = netsec_desc_ring_ts0_num;
    param->desc_ring_param[OGMA_DESC_RING_ID_TS_TX].little_endian = OGMA_TRUE;
    param->desc_ring_param[OGMA_DESC_RING_ID_TS_TX].tmr_mode      = OGMA_FALSE;
    param->desc_ring_param[OGMA_DESC_RING_ID_TS_TX].irq_ast_cnt   = 1;
    
    param->desc_ring_param[OGMA_DESC_RING_ID_VSUB_TX].valid_flag    = OGMA_TRUE;
    param->desc_ring_param[OGMA_DESC_RING_ID_VSUB_TX].entry_num     = netsec_desc_ring_ts1_num;
    param->desc_ring_param[OGMA_DESC_RING_ID_VSUB_TX].little_endian = OGMA_TRUE;
    param->desc_ring_param[OGMA_DESC_RING_ID_VSUB_TX].tmr_mode      = OGMA_FALSE;
    param->desc_ring_param[OGMA_DESC_RING_ID_VSUB_TX].irq_ast_cnt   = 1;

    param->desc_ring_param[OGMA_DESC_RING_ID_LPBR_RX].valid_flag    = OGMA_TRUE;
    param->desc_ring_param[OGMA_DESC_RING_ID_LPBR_RX].entry_num     = netsec_desc_ring_lpb0_rx_num;
    param->desc_ring_param[OGMA_DESC_RING_ID_LPBR_RX].little_endian = OGMA_TRUE;
    param->desc_ring_param[OGMA_DESC_RING_ID_LPBR_RX].tmr_mode      = OGMA_FALSE;
    param->desc_ring_param[OGMA_DESC_RING_ID_LPBR_RX].irq_ast_cnt   = 0;

    param->desc_ring_param[OGMA_DESC_RING_ID_LPBE_RX].valid_flag    = OGMA_TRUE;
    param->desc_ring_param[OGMA_DESC_RING_ID_LPBE_RX].entry_num     = netsec_desc_ring_lpb1_rx_num;
    param->desc_ring_param[OGMA_DESC_RING_ID_LPBE_RX].little_endian = OGMA_TRUE;
    param->desc_ring_param[OGMA_DESC_RING_ID_LPBE_RX].tmr_mode      = OGMA_FALSE;
    param->desc_ring_param[OGMA_DESC_RING_ID_LPBE_RX].irq_ast_cnt   = 0;

}

ogma_err_t BF_Netsec_start_all_desc_ring_strm(ogma_handle_t ogma_handle) 
{
	ogma_err_t ogma_err;

	if((ogma_err = ogma_start_desc_ring(ogma_handle,
										OGMA_DESC_RING_ID_TS_TX))
	   != OGMA_ERR_OK) {
		BF_Debug_Print_Error(("ogma_start_desc_ring(enc_tx) failed with error code %d\n", ogma_err));
		return OGMA_ERR_PARAM;
	}

	if((ogma_err = ogma_start_desc_ring(ogma_handle,
										OGMA_DESC_RING_ID_VSUB_TX))
	   != OGMA_ERR_OK) {
		BF_Debug_Print_Error(("ogma_start_desc_ring(enc_tx) failed with error code %d\n", ogma_err));
		return OGMA_ERR_PARAM;
	}

	if((ogma_err = ogma_start_desc_ring(ogma_handle,
										OGMA_DESC_RING_ID_LPBR_RX))
	   != OGMA_ERR_OK) {
		BF_Debug_Print_Error(("ogma_start_desc_ring(enc_tx) failed with error code %d\n", ogma_err));
		return OGMA_ERR_PARAM;
	}

	ogma_enable_desc_ring_irq(ogma_handle,
							  OGMA_DESC_RING_ID_LPBR_RX,
							  OGMA_CH_IRQ_REG_PKT_CNT);

	if((ogma_err = ogma_start_desc_ring(ogma_handle,
										OGMA_DESC_RING_ID_LPBE_RX))
	   != OGMA_ERR_OK) {
		BF_Debug_Print_Error(("ogma_start_desc_ring(enc_tx) failed with error code %d\n", ogma_err));
		return OGMA_ERR_PARAM;
	}

	ogma_enable_desc_ring_irq(ogma_handle,
							  OGMA_DESC_RING_ID_LPBE_RX,
							  OGMA_CH_IRQ_REG_PKT_CNT);

	return OGMA_ERR_OK;
}

ogma_err_t BF_Netsec_stop_all_desc_ring_strm(ogma_handle_t ogma_handle)
{
	   
	ogma_stop_desc_ring(ogma_handle,
						OGMA_DESC_RING_ID_TS_TX);

	ogma_stop_desc_ring(ogma_handle,
						OGMA_DESC_RING_ID_VSUB_TX);

	ogma_stop_desc_ring(ogma_handle,
						OGMA_DESC_RING_ID_LPBR_RX);

	ogma_stop_desc_ring(ogma_handle,
						OGMA_DESC_RING_ID_LPBE_RX);

	return OGMA_ERR_OK;
}

ogma_err_t BF_Netsec_send_strm_data(ogma_handle_t ogma_handle,
									T_BF_NETSEC_PROC_STRM_PARAM *strm_param_p
	)
{
	ogma_err_t ogma_err;
	ogma_strm_ctrl_t strm_ctrl;

	/*-----------------*/
	/* set strm_ctrl */
	/*-----------------*/
	pfdep_memset(&strm_ctrl,0,sizeof(ogma_strm_ctrl_t));
	strm_ctrl.drid         = strm_param_p->drid;
	strm_ctrl.mark_flag    = strm_param_p->mark;
	strm_ctrl.lpbr_flag    = strm_param_p->lpbr;
	strm_ctrl.lpbe_flag    = strm_param_p->lpbe;
	strm_ctrl.ast_int_flag = strm_param_p->ast_int;
	strm_ctrl.cr_flag      = strm_param_p->cr;
	strm_ctrl.fn_flag      = strm_param_p->fn;
	strm_ctrl.si_flag      = strm_param_p->si;
	strm_ctrl.enc_flag     = strm_param_p->enc;
	strm_ctrl.dtid         = strm_param_p->dtid;
	strm_ctrl.stid         = strm_param_p->stid;
	strm_ctrl.in_len       = strm_param_p->in_len;
	strm_ctrl.in_addr      = strm_param_p->in_addr;
	strm_ctrl.timestamp    = strm_param_p->timestamp;
	strm_ctrl.wait_time    = strm_param_p->wait_time;

	ogma_err = BF_Netsec_send_strm_data_sub(ogma_handle,
											&strm_ctrl);
	
	return ogma_err;

}

ogma_err_t BF_Netsec_send_strm_data_lpb0(ogma_handle_t ogma_handle,
										 T_BF_NETSEC_PROC_STRM_PARAM *strm_param_p
	)
{
	ogma_err_t ogma_err;
	ogma_strm_ctrl_t strm_ctrl;
	
    /*-----------------*/
	/* set strm_ctrl   */
	/*-----------------*/
	pfdep_memset(&strm_ctrl,0,sizeof(ogma_strm_ctrl_t));
	strm_ctrl.drid         = OGMA_DESC_RING_ID_TS_TX;
	strm_ctrl.mark_flag    = strm_param_p->mark;	
	strm_ctrl.lpbr_flag    = 1;
	strm_ctrl.lpbe_flag    = 0;
	strm_ctrl.ast_int_flag = strm_param_p->ast_int;
	strm_ctrl.cr_flag      = strm_param_p->cr;
	strm_ctrl.fn_flag      = strm_param_p->fn;
	strm_ctrl.si_flag      = strm_param_p->si;
	strm_ctrl.enc_flag     = strm_param_p->enc;
	strm_ctrl.dtid         = strm_param_p->dtid;
	strm_ctrl.stid         = strm_param_p->stid;
	strm_ctrl.in_len       = strm_param_p->in_len;
	strm_ctrl.in_addr      = strm_param_p->in_addr;
	strm_ctrl.timestamp    = strm_param_p->timestamp;
	strm_ctrl.wait_time    = strm_param_p->wait_time;
	
	ogma_err = BF_Netsec_send_strm_data_sub(ogma_handle,
											&strm_ctrl);
	
	return ogma_err;
	
}

ogma_err_t BF_Netsec_send_strm_data_lpb1(ogma_handle_t ogma_handle,
										 T_BF_NETSEC_PROC_STRM_PARAM *strm_param_p
	)
{
	ogma_err_t ogma_err;
	ogma_strm_ctrl_t strm_ctrl;
	
    /*-----------------*/
	/* set strm_ctrl   */
	/*-----------------*/
	pfdep_memset(&strm_ctrl,0,sizeof(ogma_strm_ctrl_t));
	strm_ctrl.drid         = OGMA_DESC_RING_ID_VSUB_TX;
	strm_ctrl.mark_flag    = strm_param_p->mark;	
	strm_ctrl.lpbr_flag    = 1;
	strm_ctrl.lpbe_flag    = 0;
	strm_ctrl.ast_int_flag = strm_param_p->ast_int;
	strm_ctrl.cr_flag      = strm_param_p->cr;
	strm_ctrl.fn_flag      = strm_param_p->fn;
	strm_ctrl.si_flag      = strm_param_p->si;
	strm_ctrl.enc_flag     = strm_param_p->enc;
	strm_ctrl.dtid         = strm_param_p->dtid;
	strm_ctrl.stid         = strm_param_p->stid;
	strm_ctrl.in_len       = strm_param_p->in_len;
	strm_ctrl.in_addr      = strm_param_p->in_addr;
	strm_ctrl.timestamp    = strm_param_p->timestamp;
	strm_ctrl.wait_time    = strm_param_p->wait_time;
	
	ogma_err = BF_Netsec_send_strm_data_sub(ogma_handle,
											&strm_ctrl);
	
	return ogma_err;
	
}

ogma_err_t BF_Netsec_send_strm_data_sub(ogma_handle_t ogma_handle,
										ogma_strm_ctrl_t *strm_ctrl_p
	)
{
	ogma_frag_info_t tx_frag_info;
	ogma_err_t ogma_err;
	ogma_uint16 tx_avail_num;
	ogma_uint32   drid;
 	OS_USER_FLGPTN flgptn;
	ogma_uint32 wait_flag;
	OS_USER_ER ercd;

	drid = strm_ctrl_p->drid;
	
	if(drid == OGMA_DESC_RING_ID_NULL) {
		BF_Debug_Print_Error(("%s: Invalid STID (%d)\n", __func__, strm_ctrl_p->stid));
		return OGMA_ERR_DATA;
	}
	
	while (1) {
		/*-------------------------*/
		/* ogma_clean_tx_desc_ring */
		/*-------------------------*/
		if((ogma_err = ogma_clean_tx_desc_ring(ogma_handle, drid))
		   != OGMA_ERR_OK) {
			BF_Debug_Print_Error(("ogma_clean_tx_desc_ring failed with error code : %d\n", ogma_err));
		}

		tx_avail_num = ogma_get_tx_avail_num(ogma_handle, drid);
		if(tx_avail_num > 0) {
			break;
		}
		BF_Debug_Print_Error(("BF_Netsec_send_strm_data_sub drid = %2x, tx_avail_num = %d\n",
							  drid, tx_avail_num));
		pfdep_msleep(10); // Delay
	}

	/*---------------------*/
	/* set input frag_info */
	/*---------------------*/
	pfdep_memset(&tx_frag_info,0,sizeof(tx_frag_info));
	tx_frag_info.len = strm_ctrl_p->in_len;

	tx_frag_info.addr      = (void *) strm_ctrl_p->in_addr;
	tx_frag_info.phys_addr = (pfdep_phys_addr_t) tx_frag_info.addr;


	/*-----------------*/
	/* set strm_data   */
	/*-----------------*/
	OS_User_Wai_Sem(SID_NETSEC_TXDONE);

	pfdep_clean_invalidate_dcache_addr((UINT32)tx_frag_info.addr, (UINT32)tx_frag_info.len);

	if((ogma_err = ogma_set_strm_data(ogma_handle,
									  drid,
									  strm_ctrl_p,
									  1,
									  &tx_frag_info,
									  NULL,
									  OGMA_TRUE)) != OGMA_ERR_OK) {
		BF_Debug_Print_Error(("ogma_set_strm_data failed with error code : %d\n", ogma_err));
		return ogma_err;
	}
	
	if(strm_ctrl_p->ast_int_flag) {
		wait_flag = BF_Netsec_drid_to_wait_flag(drid);
		
		if((ercd = OS_User_Twai_Flg(FID_NETSEC_MAIN,
									wait_flag,
									D_OS_USER_TWF_ORW,
									&flgptn,
									strm_ctrl_p->wait_time)) != D_OS_USER_E_OK) {
			
			BF_Debug_Print_Error(("ogma_set_strm_data DMA timeout : %d\n", ercd));
			return OGMA_ERR_BUSY;
		}
		OS_User_Clr_Flg(FID_NETSEC_MAIN, ~flgptn);
	}

	OS_User_Sig_Sem(SID_NETSEC_TXDONE);

	return OGMA_ERR_OK;

}


ogma_err_t BF_Netsec_get_strm_data(ogma_handle_t ogma_handle,
								   ogma_desc_ring_id_t drid,
								   T_BF_NETSEC_GET_STRM_PARAM *get_strm_param_p
	)
{
	void *tmp_priv_data_p;
	ogma_bool  err;
	ogma_uint8 err_code;
	ogma_frag_info_t rx_frag_info;
	ogma_uint32 scat_num = 1;
	ogma_err_t ogma_err = OGMA_ERR_OK;
	OS_USER_ID semid;
	OS_USER_ER ercd;

	semid = BF_Netsec_drid_to_semaphoe_id(drid);

	if((ercd = OS_User_Twai_Sem(semid,
								get_strm_param_p->wait_time)) != D_OS_USER_E_OK) {
		BF_Debug_Print_Error(("BF_Netsec_get_strm_data Receive Waiting Timeout : %d\n",  ercd));
		return OGMA_ERR_DATA;
	}
	
	if((ogma_err = 
		ogma_get_strm_data_lpbr (
			ogma_handle,
			drid,
			&err,
			&err_code,
			&rx_frag_info,
			&scat_num,
			&get_strm_param_p->out_len,
			&get_strm_param_p->stid,
			&get_strm_param_p->dtid,
			&get_strm_param_p->lti,
			&tmp_priv_data_p
			)) != OGMA_ERR_OK) {

		BF_Debug_Print_Error(("ogma_get_strm_data failed with error code : %d\n", ogma_err));

		return ogma_err;
	}
	
	pfdep_memcpy(get_strm_param_p->out_addr,
				 rx_frag_info.addr,
				 rx_frag_info.len);

	pfdep_invalidate_dcache_addr((UINT32)rx_frag_info.addr, (UINT32)rx_frag_info.len);
	
	return OGMA_ERR_OK;

}


STATIC ogma_err_t BF_Netsec_set_session_basic_db(ogma_handle_t ogma_handle, T_BF_NETSEC_SESSION_BASIC_DB_PARAM *sbdb_param_p) 
{
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
		
	ogma_set_session_db_item(ctrl_p, sbdb_param_p->id, 0, sbdb_param_p->payload_length);
	ogma_set_session_db_item(ctrl_p, sbdb_param_p->id, 1, sbdb_param_p->dst_mac_addr_up);
	ogma_set_session_db_item(ctrl_p, sbdb_param_p->id, 2, sbdb_param_p->dst_mac_addr_low);
	ogma_set_session_db_item(ctrl_p, sbdb_param_p->id, 3, sbdb_param_p->src_mac_addr_up);
	ogma_set_session_db_item(ctrl_p, sbdb_param_p->id, 4, (sbdb_param_p->src_mac_addr_low << 16) | sbdb_param_p->eth_type);
	ogma_set_session_db_item(ctrl_p, sbdb_param_p->id, 5, (0x45<<24)|(sbdb_param_p->tos << 16));
	ogma_set_session_db_item(ctrl_p, sbdb_param_p->id, 6, sbdb_param_p->init_id << 16);
	ogma_set_session_db_item(ctrl_p, sbdb_param_p->id, 7, (sbdb_param_p->ttl << 24)| (0x11 << 16));
	ogma_set_session_db_item(ctrl_p, sbdb_param_p->id, 8, sbdb_param_p->src_ip_addr);
	ogma_set_session_db_item(ctrl_p, sbdb_param_p->id, 9, sbdb_param_p->dst_ip_addr);

	return OGMA_ERR_OK;
}

STATIC ogma_err_t BF_Netsec_set_session_db(ogma_handle_t ogma_handle, T_BF_NETSEC_SESSION_DB_PARAM *sdb_param_p) 
{
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
	ogma_err_t ogma_err = OGMA_ERR_OK;
	ogma_uint32 payload_length = ((sdb_param_p->src_port)<<16) |(sdb_param_p->dst_port);
	
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 0,  sdb_param_p->base_id);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 1,  payload_length);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 2,  sdb_param_p->payload_type);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 3,  sdb_param_p->ssrc);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 4,  sdb_param_p->random_offset);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 5,  sdb_param_p->init_seqnum);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 6,  0);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 7,  0);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 8,  sdb_param_p->pid_strm);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 9,  0);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 10, sdb_param_p->pid_pmt);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 11, 0);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 12, sdb_param_p->pid_pcr);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 13, 0);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 14, 0);
	ogma_set_session_db_item(ctrl_p, sdb_param_p->id, 15, 0);	
	
	return ogma_err;
}

STATIC ogma_err_t BF_Netsec_set_session_db_mpts_st(ogma_handle_t ogma_handle, T_BF_NETSEC_SESSION_DB_MPTS_ST_PARAM *sdbm_param_p) 
{
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
	ogma_err_t ogma_err = OGMA_ERR_OK;
	
    ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 0,  0);
    ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 1,  sdbm_param_p->payload_length);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 2,  0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 3,  0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 4,  sdbm_param_p->pid_psi0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 5,  sdbm_param_p->pid_psi1);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 6,  sdbm_param_p->pid_psi2);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 7,  sdbm_param_p->pid_psi3);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 8,  0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 9,  0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 10, 0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 11, 0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 12, 0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 13, 0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 14, 0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 15, 0);

	return ogma_err;
}

STATIC ogma_err_t BF_Netsec_set_session_db_mpts_pg(ogma_handle_t ogma_handle, T_BF_NETSEC_SESSION_DB_MPTS_PG_PARAM *sdbm_param_p) 
{
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
	ogma_err_t ogma_err = OGMA_ERR_OK;
	
    ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 0,  0);
    ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 1,  sdbm_param_p->payload_length);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 2,  sdbm_param_p->pid_pmt);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 3,  sdbm_param_p->pid_pcr);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 4,  sdbm_param_p->pid_video);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 5,  sdbm_param_p->pid_audio);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 6,  sdbm_param_p->pid_mtdat);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 7,  0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 8,  0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 9,  0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 10, 0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 11, 0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 12, 0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 13, 0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 14, 0);
	ogma_set_session_db_item(ctrl_p, sdbm_param_p->id, 15, 0);

	return ogma_err;
}

STATIC ogma_err_t BF_Netsec_set_lpb_rx_desc(ogma_handle_t ogma_handle,
											ogma_uint32 channel,
											ogma_uint32 payload_length)
{
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
	ogma_err_t ogma_err = OGMA_ERR_OK;	
	
	if( ( ogma_err = ogma_setup_rx_desc_ring( ctrl_p,
											  &ctrl_p->desc_ring[channel],
											  payload_length) )
		!= OGMA_ERR_OK) {
		BF_Debug_Print_Error(("An error occurred at ogma_init.\n"
							  "Failed to LPBR_RX packet memory allocation.\n"));
	}
	
	return ogma_err;
}

STATIC void BF_Netsec_lpbr_rx_irq_handler(void)
{
	ogma_uint16 pkt_cnt;
	UINT32 i;
		
	pkt_cnt =
		ogma_get_new_rx_num(netsec_ogma_handle,
							OGMA_DESC_RING_ID_LPBR_RX);
	
	for(i=0; i < pkt_cnt; i++) {
		OS_User_Sig_Sem(SID_NETSEC_TS0_RX);
	}
}

STATIC void BF_Netsec_lpbe_rx_irq_handler(void)
{
	ogma_uint16 pkt_cnt;
	UINT32 i;
		
	pkt_cnt =
		ogma_get_new_rx_num(netsec_ogma_handle,
							OGMA_DESC_RING_ID_LPBE_RX);
	
	for(i=0; i < pkt_cnt; i++) {
		OS_User_Sig_Sem(SID_NETSEC_TS1_RX);
	}
}

STATIC void BF_Netsec_ast_int_irq_handler () 
{	
	ogma_uint32 value;
	
	value = ogma_get_pkt_irq(netsec_ogma_handle);
	
	if(value & OGMA_PKT_IRQ_REG_TX_DONE_TS) {
		OS_User_Set_Flg(FID_NETSEC_MAIN,
						OGMA_FLG_TX_DONE_TS);
	}

	if(value & OGMA_PKT_IRQ_REG_TX_DONE_Vsub) {
		OS_User_Set_Flg(FID_NETSEC_MAIN,
						OGMA_FLG_TX_DONE_Vsub);
	}

}

STATIC ogma_uint32 BF_Netsec_drid_to_wait_flag (ogma_uint32 drid) 
{
	ogma_uint32 flag = 0;

	switch (drid) {
		
	case OGMA_DESC_RING_ID_TS_TX :
		flag = OGMA_FLG_TX_DONE_TS;
		break;
			
	case OGMA_DESC_RING_ID_VSUB_TX :
		flag = OGMA_FLG_TX_DONE_Vsub;
		break;

	default :
		break;
	}

	return flag;
}

STATIC ogma_uint32 BF_Netsec_drid_to_semaphoe_id (ogma_desc_ring_id_t drid) 
{
	ogma_uint32 id = 0;

	switch (drid) {
		
	case OGMA_DESC_RING_ID_LPBR_RX :
		id = SID_NETSEC_TS0_RX;
		break;
			
	case OGMA_DESC_RING_ID_LPBE_RX :
		id = SID_NETSEC_TS1_RX;
		break;

	default :
		break;
	}

	return id;
}

void BF_Netsec_Int_Handler(void)
{
	UINT32 top_irq_status;
	
	if(netsec_ogma_handle == NULL) {
		return;
	}
  
	/*-------------------------*/
	/* Handler for each Status */
	/*-------------------------*/
	top_irq_status = 
		ogma_get_top_irq_status(netsec_ogma_handle,OGMA_FALSE);

	if(top_irq_status & OGMA_TOP_IRQ_REG_PKT) {
		BF_Netsec_ast_int_irq_handler();
	}

	if(top_irq_status & OGMA_TOP_IRQ_REG_LPBR_RX) {
		BF_Netsec_lpbr_rx_irq_handler();
	} 

	if(top_irq_status & OGMA_TOP_IRQ_REG_LPBE_RX) {
		BF_Netsec_lpbe_rx_irq_handler();
	} 

	return ;
}
