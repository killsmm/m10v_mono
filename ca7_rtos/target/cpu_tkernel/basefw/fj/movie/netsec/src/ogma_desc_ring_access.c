/**
 * ogma_basic_access.c
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#include "ogma_internal.h"
#include "ogma_basic_access.h"
#include "ogma_desc_ring_access_internal.h"
#include "bf_arm.h"

#undef STATIC
#ifdef MODULE_TEST
#define STATIC
#else
#define STATIC static
#endif

static UINT32 netsec_cs __attribute__((section(".LOCK_SECTION"), aligned(64)));

STATIC void ogma_set_tx_desc_entry (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 idx,
    const ogma_tx_pkt_ctrl_t *tx_pkt_ctrl_p,
    ogma_bool first_flag,
    ogma_bool last_flag,
    const ogma_frag_info_t *frag_info_p,
    pfdep_pkt_handle_t pkt_handle);

STATIC void ogma_set_rx_desc_entry (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 idx,
    const ogma_frag_info_t *frag_info_p,
    pfdep_pkt_handle_t pkt_handle);

STATIC void ogma_get_rx_desc_entry (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 idx,
    ogma_rx_pkt_info_t *rx_pkt_info_p,
    ogma_frag_info_t *frag_info_p,
    ogma_uint16 *len_p,
    pfdep_pkt_handle_t *pkt_handle_p);

STATIC void ogma_set_strm_desc_entry (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 idx,
    const ogma_strm_ctrl_t *strm_ctrl_p,
    ogma_bool first_flag,
    ogma_bool last_flag,
    const ogma_frag_info_t *frag_info_p,
    pfdep_pkt_handle_t pkt_handle);


STATIC void ogma_set_rx_lpbr_desc_entry (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 idx,
    ogma_bool first_flag,
    ogma_bool last_flag,
    const ogma_frag_info_t *out_frag_info_p,
    const void *priv_data_p
    );

STATIC void ogma_inc_desc_head_idx (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 increment);

STATIC void ogma_inc_desc_tail_idx (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 increment);

STATIC ogma_uint16 ogma_get_tx_avail_num_sub (
    ogma_ctrl_t *ctrl_p,
    const ogma_desc_ring_t *desc_ring_p
    );

/**********************************************************************
 * Function definitions
 **********************************************************************/

ogma_err_t ogma_alloc_desc_ring (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_id_t ring_id
    )
{
    ogma_err_t ogma_err = OGMA_ERR_OK;
    ogma_desc_ring_t *desc_ring_p = &ctrl_p->desc_ring[ring_id];
    ogma_desc_ring_param_t *desc_ring_param_p = &ctrl_p->desc_ring[ring_id].param;

    ogma_check_clk_supply( ctrl_p, 0);

    if ( ( ctrl_p->param.desc_ring_param[ring_id].valid_flag) &&
         ( ( ctrl_p->param.desc_ring_param[ring_id].entry_num <
             OGMA_DESC_ENTRY_NUM_MIN) ||
           ( ctrl_p->param.desc_ring_param[ring_id].entry_num >
             OGMA_DESC_ENTRY_NUM_MAX) ) ) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_alloc_desc_ring.\n"
                     "Please set entry_num between %d and %d.\n",
                     OGMA_DESC_ENTRY_NUM_MIN, OGMA_DESC_ENTRY_NUM_MAX);
        return OGMA_ERR_PARAM;
    }

    desc_ring_p->ring_id = ring_id;

    pfdep_memcpy( desc_ring_param_p,
                  &ctrl_p->param.desc_ring_param[ring_id],
                  sizeof( ogma_desc_ring_param_t) );

    if ( !desc_ring_param_p->valid_flag) {
        desc_ring_p->desc_ring_phys_addr = ctrl_p->dummy_desc_entry_phys_addr;
        ogma_write_reg( ctrl_p,
                        ogma_desc_start_reg_addr[ring_id],
                        desc_ring_p->desc_ring_phys_addr);
        return OGMA_ERR_OK;
    }

    switch ( ring_id) {
	case OGMA_DESC_RING_ID_PKT_TX:
        desc_ring_p->pkt_desc_ring_flag = OGMA_TRUE;
        desc_ring_p->tx_desc_ring_flag = OGMA_TRUE;
        desc_ring_p->desc_entry_len = sizeof( ogma_tx_desc_entry_t);
        break;

	case OGMA_DESC_RING_ID_PKT_RX:
        desc_ring_p->pkt_desc_ring_flag = OGMA_TRUE;
        desc_ring_p->rx_desc_ring_flag = OGMA_TRUE;
        desc_ring_p->desc_entry_len = 16U;
        break;

	case OGMA_DESC_RING_ID_TS_TX:
	case OGMA_DESC_RING_ID_VSUB_TX:
        desc_ring_p->strm_desc_ring_flag = OGMA_TRUE;
        desc_ring_p->tx_desc_ring_flag = OGMA_TRUE;
        desc_ring_p->desc_entry_len = sizeof( ogma_tx_desc_entry_t);
        break;

	case OGMA_DESC_RING_ID_LPBR_RX:
	case OGMA_DESC_RING_ID_LPBE_RX:
        desc_ring_p->rx_desc_ring_flag = OGMA_TRUE;
		desc_ring_p->desc_entry_len = 16U;
        break;
		
    default:
        pfdep_assert(0);
    }

    if ( ( pfdep_init_soft_lock ( &desc_ring_p->soft_lock) )
         != PFDEP_ERR_OK) {
        pfdep_memset( desc_ring_param_p,
                      0,
                      sizeof( ogma_desc_ring_param_t) );
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_alloc_desc_ring.\n"
                     "Failed to soft_lock's initialization.\n");
        return OGMA_ERR_ALLOC;
    }

    if ( (pfdep_dma_malloc (
			  ( pfdep_dev_handle_t) ctrl_p->dev_handle,
			  ( pfdep_uint32) desc_ring_p->desc_entry_len * desc_ring_param_p->entry_num,
			  ( void **) &desc_ring_p->desc_ring_cpu_addr,
			  ( pfdep_phys_addr_t *) &desc_ring_p->desc_ring_phys_addr) )
         != PFDEP_ERR_OK) {
        ogma_err = OGMA_ERR_ALLOC;
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_alloc_desc_ring.\n"
                     "Failed to desc_ring entry memory allocation.\n");
        goto err;
    }

   pfdep_memset(
        desc_ring_p->desc_ring_cpu_addr,
        0,
        ( ogma_uint32)desc_ring_p->desc_entry_len * desc_ring_param_p->entry_num);

    ogma_write_reg( ctrl_p,
                    ogma_desc_start_reg_addr[ring_id],
                    desc_ring_p->desc_ring_phys_addr);

    if ( ( desc_ring_p->frag_info_p =
           pfdep_malloc( sizeof( ogma_frag_info_t) * desc_ring_param_p->entry_num) )
         == NULL) {
        ogma_err = OGMA_ERR_ALLOC;
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_alloc_desc_ring.\n"
                     "Failed to fragment infomation memory allocation.\n");
        goto err;
    }

    pfdep_memset(
        desc_ring_p->frag_info_p,
        0,
        sizeof( ogma_frag_info_t) * desc_ring_param_p->entry_num);
	
    if ( ( desc_ring_p->priv_data_p =
           pfdep_malloc( sizeof( ogma_desc_entry_priv_t) * desc_ring_param_p->entry_num) )
        == NULL) {
        ogma_err = OGMA_ERR_ALLOC;
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_alloc_desc_ring.\n"
                     "Failed to private data memory allocation.\n");
        goto err;
    }

    pfdep_memset(
        desc_ring_p->priv_data_p,
        0,
        sizeof( ogma_desc_entry_priv_t) * desc_ring_param_p->entry_num);

    return OGMA_ERR_OK;

err:
    ogma_free_desc_ring( ctrl_p, desc_ring_p);
    return ogma_err;
}

void ogma_free_desc_ring (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p
    )
{

    ogma_check_clk_supply( ctrl_p, 0);

    if ( !desc_ring_p->param.valid_flag) {
        return ;
    }
    if ( ogma_is_pkt_desc_ring ( desc_ring_p) ) {
        if ( ( desc_ring_p->desc_ring_cpu_addr != NULL) &&
             ( desc_ring_p->frag_info_p != NULL) &&
             ( desc_ring_p->priv_data_p != NULL) ) {
            ogma_uninit_pkt_desc_ring( ctrl_p, desc_ring_p);
        }
    }
    if ( desc_ring_p->desc_ring_cpu_addr != NULL) {
        pfdep_dma_free( ctrl_p->dev_handle,
                        ( ogma_uint32)desc_ring_p->desc_entry_len *
                        desc_ring_p->param.entry_num,
                        desc_ring_p->desc_ring_cpu_addr,
                        desc_ring_p->desc_ring_phys_addr);
    }

    if ( desc_ring_p->frag_info_p != NULL) {
        pfdep_free(desc_ring_p->frag_info_p);
    }

    if ( desc_ring_p->priv_data_p != NULL) {
        pfdep_free( desc_ring_p->priv_data_p);
    }

    pfdep_memset( desc_ring_p, 0, sizeof( ogma_desc_ring_t) );
}

ogma_err_t ogma_setup_rx_desc_ring (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
	ogma_uint32  pkt_buf_len
    )
{
    ogma_uint16 idx;
    ogma_frag_info_t frag_info= {0,0,0};

    pfdep_pkt_handle_t tmp_pkt_handle;

    ogma_check_clk_supply( ctrl_p, 0);

    for ( idx = 0, frag_info.len = pkt_buf_len;
          idx < desc_ring_p->param.entry_num; idx++) {
        if ( ( pfdep_alloc_pkt_buf (
                   ( pfdep_dev_handle_t)ctrl_p->dev_handle,
                   ( ogma_uint16)frag_info.len,
                   ( void **)&frag_info.addr,
                   ( pfdep_phys_addr_t *)&frag_info.phys_addr,
                   ( pfdep_pkt_handle_t *)&tmp_pkt_handle) )
             != PFDEP_ERR_OK) {
            ogma_uninit_pkt_desc_ring( ctrl_p, desc_ring_p);
            pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                         "An error occurred at ogma_setup_rx_desc_ring.\n"
                         "Failed to rx packet memory allocation.\n");
            return OGMA_ERR_ALLOC;
        }
		
		ogma_set_rx_desc_entry( ctrl_p,
                                desc_ring_p,
                                idx,
                                &frag_info,
                                tmp_pkt_handle);

		pfdep_invalidate_dcache_addr((UINT32)frag_info.addr, (UINT32)frag_info.len);
    }

	desc_ring_p->param.rx_buf_len = pkt_buf_len;
	
    return OGMA_ERR_OK;
}


void ogma_uninit_pkt_desc_ring (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p
    )
{
    ogma_uint16 idx;
    ogma_uint32 tmp;
    ogma_bool last_flag;

    ogma_check_clk_supply( ctrl_p, 0);

    for ( idx = 0; idx < desc_ring_p->param.entry_num; idx++) {
        if ( desc_ring_p->frag_info_p[idx].addr == NULL ) {
            continue;
        }
        tmp = *( ( ogma_uint32 *)
                 ( ( pfdep_cpu_addr_t)desc_ring_p->desc_ring_cpu_addr +
                   desc_ring_p->desc_entry_len * idx) );

        last_flag = ( ( ( tmp >> 8) & 0x1) != 0);

        pfdep_free_pkt_buf (
            ctrl_p->dev_handle,
            desc_ring_p->frag_info_p[idx].len,
            desc_ring_p->frag_info_p[idx].addr,
            desc_ring_p->frag_info_p[idx].phys_addr,
            last_flag,
            desc_ring_p->priv_data_p[idx].pkt_handle);
    }

    /* clear frag_info_p */
    pfdep_memset( desc_ring_p->frag_info_p,
                  0,
                  sizeof( ogma_frag_info_t) * desc_ring_p->param.entry_num);

    /* clear pkt_handle_p */
    pfdep_memset( desc_ring_p->priv_data_p,
                  0,
                  sizeof( ogma_desc_entry_priv_t) * desc_ring_p->param.entry_num);
    
    /* clear desc ring entry*/
    pfdep_memset ( desc_ring_p->desc_ring_cpu_addr,
                   0,
                   ( ogma_uint32)desc_ring_p->desc_entry_len *
                   desc_ring_p->param.entry_num);
}

STATIC void ogma_set_tx_desc_entry (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 idx,
    const ogma_tx_pkt_ctrl_t *tx_pkt_ctrl_p,
    ogma_bool first_flag,
    ogma_bool last_flag,
    const ogma_frag_info_t *frag_info_p,
    pfdep_pkt_handle_t pkt_handle
    )
{
    ogma_tx_desc_entry_t tx_desc_entry;
    ogma_uint32 attr;

    ogma_check_clk_supply( ctrl_p, 0);

    ogma_check_desc_own_sanity(ctrl_p, desc_ring_p, idx, 0);
 
    pfdep_memset( &tx_desc_entry, 0, sizeof( ogma_tx_desc_entry_t) );

    attr = ( 1UL << OGMA_TX_PKT_DESC_RING_OWN_FIELD) |
        ( ((idx == (desc_ring_p->param.entry_num - 1) )? 1 : 0) <<
          OGMA_TX_PKT_DESC_RING_LD_FIELD) |
        ( desc_ring_p->ring_id << OGMA_TX_PKT_DESC_RING_DRID_FIELD) |
        ( tx_pkt_ctrl_p->target_desc_ring_id <<
          OGMA_TX_PKT_DESC_RING_TDRID_FIELD) |
        ( tx_pkt_ctrl_p->lpbe_flag <<
          OGMA_TX_PKT_DESC_RING_LPBE_FIELD) |
        ( (tx_pkt_ctrl_p->auto_fragment_flag) <<
		  OGMA_TX_PKT_DESC_RING_AF_FIELD) |
        ( first_flag <<
		  OGMA_TX_PKT_DESC_RING_FS_FIELD) |
        ( last_flag <<
		  OGMA_TX_PKT_DESC_RING_LS_FIELD) |
        ( ( tx_pkt_ctrl_p->enc_flag ? tx_pkt_ctrl_p->cksum_offload_flag: 0) <<
          OGMA_TX_PKT_DESC_RING_CO_FIELD) |
        ( tx_pkt_ctrl_p->tcp_seg_offload_flag <<
          OGMA_TX_PKT_DESC_RING_SO_FIELD) |
		( 1UL <<
		  OGMA_TX_PKT_DESC_RING_TRS_FIELD);

    tx_desc_entry.attr = attr;

    tx_desc_entry.data_buf_addr = frag_info_p->phys_addr;

    tx_desc_entry.buf_len_info =
        ( tx_pkt_ctrl_p->seg_len << 16) | frag_info_p->len;

    pfdep_memcpy( ( (void *) ( ( pfdep_cpu_addr_t)desc_ring_p->desc_ring_cpu_addr +
                               desc_ring_p->desc_entry_len * idx) ),
                  ( void *)&tx_desc_entry,
                  desc_ring_p->desc_entry_len);

    desc_ring_p->frag_info_p[idx].phys_addr = frag_info_p->phys_addr;
    desc_ring_p->frag_info_p[idx].addr = frag_info_p->addr;
    desc_ring_p->frag_info_p[idx].len = frag_info_p->len;
    desc_ring_p->priv_data_p[idx].pkt_handle = pkt_handle;

}

STATIC void ogma_set_rx_desc_entry (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 idx,
    const ogma_frag_info_t *frag_info_p,
    pfdep_pkt_handle_t pkt_handle
    )
{
    ogma_rx_desc_entry_t rx_desc_entry;

    ogma_check_clk_supply( ctrl_p, 0);

    ogma_check_desc_own_sanity(ctrl_p, desc_ring_p, idx, 0);
 
    pfdep_memset( &rx_desc_entry, 0, sizeof ( ogma_rx_desc_entry_t) );

    rx_desc_entry.attr = ( 1UL << OGMA_RX_PKT_DESC_RING_OWN_FIELD) |
        ( 1UL << OGMA_RX_PKT_DESC_RING_FS_FIELD) |
		( 1UL << OGMA_RX_PKT_DESC_RING_LS_FIELD) ; /* OWN = FS = LS = 1 */

    rx_desc_entry.data_buf_addr = frag_info_p->phys_addr;

    rx_desc_entry.buf_len_info = frag_info_p->len;

    if ( idx == ( desc_ring_p->param.entry_num - 1) ) {
        rx_desc_entry.attr |= ( 0x1U << OGMA_RX_PKT_DESC_RING_LD_FIELD); /* LD = 1 */
    }

    pfdep_memcpy( ( ( void *) ( ( pfdep_cpu_addr_t)desc_ring_p->desc_ring_cpu_addr +
                                desc_ring_p->desc_entry_len * idx + 4) ),
                  ( void *) ( ( pfdep_cpu_addr_t)&rx_desc_entry + 4),
                  ( ogma_uint32)( desc_ring_p->desc_entry_len - 4U) );


    pfdep_memcpy( ( ( void *) ( ( pfdep_cpu_addr_t)desc_ring_p->desc_ring_cpu_addr +
                                desc_ring_p->desc_entry_len * idx) ),
                  ( void *)&rx_desc_entry,
                  4);

    desc_ring_p->frag_info_p[idx].phys_addr = frag_info_p->phys_addr;
    desc_ring_p->frag_info_p[idx].addr = frag_info_p->addr;
    desc_ring_p->frag_info_p[idx].len = frag_info_p->len;
    desc_ring_p->priv_data_p[idx].pkt_handle = pkt_handle;

}

/* OGMA_CONFIG_USE_PKT_DESC_RING*/


STATIC void ogma_get_rx_desc_entry (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 idx,
    ogma_rx_pkt_info_t *rx_pkt_info_p,
    ogma_frag_info_t *frag_info_p,
    ogma_uint16 *len_p,
    pfdep_pkt_handle_t *pkt_handle_p)
{
    ogma_rx_desc_entry_t rx_desc_entry;

    ogma_check_clk_supply( ctrl_p, 0);

    ogma_check_desc_own_sanity( ctrl_p, desc_ring_p, idx, 0);

    pfdep_memset( &rx_desc_entry, 0, sizeof( ogma_rx_desc_entry_t) );
    pfdep_memset( rx_pkt_info_p, 0, sizeof( ogma_rx_pkt_info_t) );

    pfdep_memcpy( &rx_desc_entry,
                  (void *) ( ( pfdep_cpu_addr_t)desc_ring_p->desc_ring_cpu_addr +
                               desc_ring_p->desc_entry_len * idx),
                  desc_ring_p->desc_entry_len);

	if(desc_ring_p->ring_id == OGMA_DESC_RING_ID_LPBR_RX) {
		*len_p = rx_desc_entry.buf_len_info;
	} else {
		*len_p = rx_desc_entry.buf_len_info >> 16;
	}

    rx_pkt_info_p->fragmented_flag =
        ( rx_desc_entry.attr >> OGMA_RX_PKT_DESC_RING_FR_FIELD) & 0x1; /* FR*/

    rx_pkt_info_p->err_flag =
        ( rx_desc_entry.attr >> OGMA_RX_PKT_DESC_RING_ER_FIELD) & 0x1; /* ER */

    rx_pkt_info_p->rx_cksum_result =
        ( rx_desc_entry.attr >> OGMA_RX_PKT_DESC_RING_CO_FIELD) & 0x3; /* CO */

    rx_pkt_info_p->err_code =
        ( rx_desc_entry.attr >> OGMA_RX_PKT_DESC_RING_ERROR_CODE_FIELD) & 0x3; /* Error Code */

    rx_pkt_info_p->stid =
        ( rx_desc_entry.attr >> OGMA_RX_PKT_DESC_RING_ID_FIELD ) & 0xf;  /* STID */

    pfdep_memcpy( frag_info_p,
                  &desc_ring_p->frag_info_p[idx],
                  sizeof(ogma_frag_info_t) );

    *pkt_handle_p = desc_ring_p->priv_data_p[idx].pkt_handle;
}

void ogma_set_rx_lpbr_desc_entry (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 idx,
    ogma_bool first_flag,
    ogma_bool last_flag,
    const ogma_frag_info_t *out_frag_info_p,
    const void *priv_data_p
    )
{
    ogma_lpb_desc_entry_t lpb_desc_entry;
    ogma_uint32 attr;
	/* ogma_uint32 *debug_lpb_desc_entry_p; */

    ogma_check_clk_supply( ctrl_p, 0);

    ogma_check_desc_own_sanity( ctrl_p, desc_ring_p, idx, 0);

    pfdep_memset( &lpb_desc_entry, 0, sizeof(ogma_lpb_desc_entry_t) );

    attr =
        ( 1UL << OGMA_LPB_RX_DESC_RING_OWN_FIELD) |
		( (( idx == ( desc_ring_p->param.entry_num - 1)) ? 1 : 0) <<
          OGMA_LPB_RX_DESC_RING_LD_FIELD) |
        ( first_flag << OGMA_LPB_RX_DESC_RING_FS_FIELD) |
        ( last_flag << OGMA_LPB_RX_DESC_RING_LS_FIELD);

    lpb_desc_entry.attr = attr;
    lpb_desc_entry.out_data_buf_addr = out_frag_info_p->phys_addr;
    lpb_desc_entry.data_len = out_frag_info_p->len;
	
    pfdep_memcpy( (void *) ( ( pfdep_cpu_addr_t)desc_ring_p->desc_ring_cpu_addr +
                             desc_ring_p->desc_entry_len * idx),
                  &lpb_desc_entry,
                  desc_ring_p->desc_entry_len);
	
    pfdep_print( PFDEP_DEBUG_LEVEL_DEBUG_MORE_DETAILED,
                 "ogma_set_rx_lpbr_desc_entry debug info, ring_id=%u, idx=%u.\n",
                 desc_ring_p->ring_id, idx);
\
    desc_ring_p->frag_info_p[idx].phys_addr = out_frag_info_p->phys_addr;
    desc_ring_p->frag_info_p[idx].addr = out_frag_info_p->addr;
    desc_ring_p->frag_info_p[idx].len = out_frag_info_p->len;
    desc_ring_p->priv_data_p[idx].bulk_priv_data_p = (void *)priv_data_p;

}

STATIC void ogma_set_strm_desc_entry (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 idx,
    const ogma_strm_ctrl_t *strm_ctrl_p,
    ogma_bool first_flag,
    ogma_bool last_flag,
    const ogma_frag_info_t *frag_info_p,
    pfdep_pkt_handle_t pkt_handle
    )
{
    ogma_tx_desc_entry_t tx_desc_entry;
    ogma_uint32 attr;

    ogma_check_clk_supply( ctrl_p, 0);

    ogma_check_desc_own_sanity(ctrl_p, desc_ring_p, idx, 0);
 
    pfdep_memset( &tx_desc_entry, 0, sizeof( ogma_tx_desc_entry_t) );

    attr = (1UL <<
			OGMA_TX_STRM_DESC_RING_OWN_FIELD) |
        ( (( idx == (desc_ring_p->param.entry_num - 1)) ? 1 : 0 ) <<
          OGMA_TX_STRM_DESC_RING_LD_FIELD) |
        ( desc_ring_p->ring_id << 
		  OGMA_TX_STRM_DESC_RING_DRID_FIELD) |
        ( strm_ctrl_p->mark_flag << 
          OGMA_TX_STRM_DESC_RING_MARK_FIELD) |
        ( strm_ctrl_p->lpbr_flag << 
          OGMA_TX_STRM_DESC_RING_LPBR_FIELD) |
		( ((strm_ctrl_p->stid & 0x10) >> 4) << 
		  OGMA_TX_STRM_DESC_RING_STID_HIGH_FIELD)|
        ( strm_ctrl_p->lpbe_flag << 
          OGMA_TX_STRM_DESC_RING_LPBE_FIELD) |
        ( strm_ctrl_p->ast_int_flag << 
          OGMA_TX_STRM_DESC_RING_AST_INT_FIELD) |
        ( (strm_ctrl_p->cr_flag & last_flag) << 
          OGMA_TX_STRM_DESC_RING_CR_FIELD) |
        ( (strm_ctrl_p->fn_flag & last_flag) << 
          OGMA_TX_STRM_DESC_RING_FN_FIELD) |
        ((strm_ctrl_p->si_flag & first_flag) << 
          OGMA_TX_STRM_DESC_RING_SI_FIELD) |
        ( first_flag << 
		  OGMA_TX_STRM_DESC_RING_FS_FIELD) |
        ( last_flag << 
		  OGMA_TX_STRM_DESC_RING_LS_FIELD) |
        ( strm_ctrl_p->enc_flag << 
          OGMA_TX_STRM_DESC_RING_ENC_FIELD) |
        ( strm_ctrl_p->dtid << 
		  OGMA_TX_STRM_DESC_RING_DTID_FIELD) |
        ( (strm_ctrl_p->stid & 0xf) << 
		  OGMA_TX_STRM_DESC_RING_STID_LOW_FIELD);

    tx_desc_entry.attr = attr;
	
    tx_desc_entry.data_buf_addr = frag_info_p->phys_addr;

    tx_desc_entry.buf_len_info = frag_info_p->len;

    tx_desc_entry.timestamp    = strm_ctrl_p->timestamp;

    pfdep_memcpy( ( (void *) ( ( pfdep_cpu_addr_t)desc_ring_p->desc_ring_cpu_addr +
                               desc_ring_p->desc_entry_len * idx) ),
                  ( void *)&tx_desc_entry,
                  desc_ring_p->desc_entry_len);

    desc_ring_p->frag_info_p[idx].phys_addr = frag_info_p->phys_addr;
    desc_ring_p->frag_info_p[idx].addr = frag_info_p->addr;
    desc_ring_p->frag_info_p[idx].len = frag_info_p->len;
    desc_ring_p->priv_data_p[idx].pkt_handle = pkt_handle;

}


STATIC __inline ogma_uint16 ogma_calc_busy_entry_num (
    ogma_uint16 head_idx,
    ogma_uint16 tail_idx,
    ogma_uint16 entry_num,
    ogma_bool full_flag
    )
{

    ogma_int16 busy_entry_num;

    if (full_flag) {
        busy_entry_num = entry_num;
    } else if (head_idx >= tail_idx) {
        busy_entry_num = head_idx - tail_idx;
    } else {
        busy_entry_num = entry_num + head_idx - tail_idx;
    }

    return busy_entry_num;

}

STATIC void ogma_inc_desc_head_idx (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 increment)
{
    ogma_uint32 sum;

    ogma_check_clk_supply( ctrl_p, 0);

    if ( ( desc_ring_p->tail_idx > desc_ring_p->head_idx) ||
         desc_ring_p->full_flag) {
        pfdep_assert( increment <=
                      ( desc_ring_p->tail_idx -
                       desc_ring_p->head_idx));
    } else {
        pfdep_assert( increment <=
                      ( desc_ring_p->param.entry_num +
                        desc_ring_p->tail_idx -
                        desc_ring_p->head_idx) );
    }

    sum = (ogma_uint32) desc_ring_p->head_idx + increment;

    if ( sum >= desc_ring_p->param.entry_num) {
        sum -= desc_ring_p->param.entry_num;
    }

    desc_ring_p->head_idx = ( ogma_uint16)sum;

    if ( desc_ring_p->head_idx == desc_ring_p->tail_idx) {
        desc_ring_p->full_flag = OGMA_TRUE;
    }

    {
        ogma_uint16 busy_entry_num =
            ogma_calc_busy_entry_num (
                desc_ring_p->head_idx,
                desc_ring_p->tail_idx,
                desc_ring_p->param.entry_num,
                desc_ring_p->full_flag
                );

        if (ctrl_p->stat_info.max_busy_entry_num[desc_ring_p->ring_id]
            < busy_entry_num) {
            ctrl_p->stat_info.max_busy_entry_num[desc_ring_p->ring_id]
                = busy_entry_num;
        }

        pfdep_print(PFDEP_DEBUG_LEVEL_DEBUG,
                    "[%s] busy_entry_num[%d] = %d\n",
                    (char *)__func__,
                    desc_ring_p->ring_id,
                    busy_entry_num);
    }
}

STATIC void ogma_inc_desc_tail_idx (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 increment)
{
    ogma_uint32 sum;

    ogma_check_clk_supply( ctrl_p, 0);

    if ( ( desc_ring_p->head_idx >= desc_ring_p->tail_idx) &&
         ( !desc_ring_p->full_flag) ) {
        pfdep_assert( increment <=
                      ( desc_ring_p->head_idx -
                        desc_ring_p->tail_idx) );
    } else {
        pfdep_assert( increment <=
                      ( desc_ring_p->param.entry_num +
                        desc_ring_p->head_idx -
                        desc_ring_p->tail_idx) );
    }

    sum = (ogma_uint32) desc_ring_p->tail_idx + increment;

    if ( sum >= desc_ring_p->param.entry_num) {
        sum -= desc_ring_p->param.entry_num;
    } 

    desc_ring_p->tail_idx = ( ogma_uint16)sum;

    desc_ring_p->full_flag = OGMA_FALSE;

    {
        ogma_uint16 busy_entry_num =
            ogma_calc_busy_entry_num (
                desc_ring_p->head_idx,
                desc_ring_p->tail_idx,
                desc_ring_p->param.entry_num,
                desc_ring_p->full_flag
                );

        pfdep_print(PFDEP_DEBUG_LEVEL_DEBUG,
                    "[%s] busy_entry_num[%d] = %d\n",
                    (char *)__func__,
                    desc_ring_p->ring_id,
                    busy_entry_num);
    }

}


STATIC ogma_uint16 ogma_get_tx_avail_num_sub (
    ogma_ctrl_t *ctrl_p,
    const ogma_desc_ring_t *desc_ring_p
    )
{
    ogma_uint16 tx_avail_num;

    ogma_check_clk_supply( ctrl_p, 0);

    if ( desc_ring_p->full_flag) {

        tx_avail_num = 0;

    } else if ( desc_ring_p->tail_idx > desc_ring_p->head_idx) {

        tx_avail_num = desc_ring_p->tail_idx - desc_ring_p->head_idx;

    } else {

        tx_avail_num = 
            desc_ring_p->param.entry_num +
            desc_ring_p->tail_idx -
            desc_ring_p->head_idx;
    }

    return tx_avail_num;
}


ogma_err_t ogma_start_desc_ring (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    )
{
    ogma_uint32 value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p;

    if ( ( ctrl_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ) {
        return OGMA_ERR_PARAM;
    }

    if ( !ctrl_p->desc_ring[ring_id].param.valid_flag) {
        return OGMA_ERR_NOTAVAIL;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    if ( desc_ring_p->running_flag) {
        return OGMA_ERR_BUSY;
    }

    if ( desc_ring_p->rx_desc_ring_flag || 
		 desc_ring_p->bulk_desc_ring_flag) {
 
        ogma_write_reg ( ctrl_p,
                         desc_ring_irq_inten_set_reg_addr[ring_id],
                         OGMA_CH_IRQ_REG_RCV);

        ogma_write_reg ( ctrl_p,
                         pkt_int_cnt_reg_addr[ring_id],
                         1);
    }

    if ( desc_ring_p->tx_desc_ring_flag) {
      
        value = OGMA_CH_IRQ_REG_EMPTY;

        ogma_write_reg ( ctrl_p,
                         desc_ring_irq_inten_set_reg_addr[ring_id],
                         value);

        ogma_write_reg ( ctrl_p,
                         pkt_int_cnt_reg_addr[ring_id],
                         desc_ring_p->param.irq_ast_cnt);
    }

    desc_ring_p->running_flag = OGMA_TRUE;

    return OGMA_ERR_OK;
}

STATIC ogma_uint16 ogma_get_tx_done_num_sub (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p
    )
{
    ogma_uint32 value;

    value = ogma_read_reg ( ctrl_p,
                            pkt_cnt_reg_addr[desc_ring_p->ring_id] );
	
    desc_ring_p->tx_done_num += value;

    return desc_ring_p->tx_done_num;
}

ogma_err_t ogma_stop_desc_ring (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    )
{
    ogma_uint32 value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p;
 
    if ( ( ctrl_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ) {
        return OGMA_ERR_PARAM;
    }
          
    if ( !ctrl_p->desc_ring[ring_id].param.valid_flag) {
        return OGMA_ERR_NOTAVAIL;
    }

    if ( !ogma_is_pkt_desc_ring( &ctrl_p->desc_ring[ring_id] ) ) {
        return OGMA_ERR_PARAM;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    if ( !desc_ring_p->running_flag) {
        return OGMA_ERR_INVALID;
    }

    value = (OGMA_CH_IRQ_REG_RCV | OGMA_CH_IRQ_REG_EMPTY);

    ogma_write_reg ( ctrl_p,
                     desc_ring_irq_inten_clr_reg_addr[ring_id],
                     value);

    desc_ring_p->running_flag = OGMA_FALSE;

    return OGMA_ERR_OK;
}

ogma_desc_ring_id_t ogma_get_drid (
	ogma_handle_t ogma_handle,
	ogma_uint16 stid
	)
{	
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;

	if(ctrl_p->param.pkt_ctrl_param.ets == OGMA_TRUE) {
		return ogma_desc_ts_drid[stid];
	} else {
		return ogma_desc_es_drid[stid];
	}
}


ogma_uint16 ogma_get_rx_num (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    )
{
 
    ogma_uint32 result;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p = NULL;
    
    if ( ( ctrl_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ||
         ( !ctrl_p->desc_ring[ring_id].param.valid_flag) ) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_rx_num.\n"
                     "Please set valid argument.\n");
        return 0;
    }

    if ( ( !ctrl_p->desc_ring[ring_id].rx_desc_ring_flag) && 
         ( !ogma_is_bulk_desc_ring( &ctrl_p->desc_ring[ring_id] ) ) ) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_rx_num.\n"
                     "Please select rx packet desc ring or bulk desc ring.\n");
        return 0;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    if ( !desc_ring_p->running_flag) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_rx_num.\n"
                     "Please select running desc ring.\n");
        return 0;
    }

    result = ogma_read_reg( ctrl_p, pkt_cnt_reg_addr[ring_id]);

    desc_ring_p->rx_num += result;

    if ( desc_ring_p->rx_desc_ring_flag && ( result != 0) ) {
        ogma_inc_desc_head_idx( ctrl_p, desc_ring_p, ( ogma_uint16)result);
    }

    return desc_ring_p->rx_num;
}

ogma_uint16 ogma_get_new_rx_num (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    )
{
 
    ogma_uint32 result;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p = NULL;
    
    if ( ( ctrl_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ||
         ( !ctrl_p->desc_ring[ring_id].param.valid_flag) ) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_rx_num.\n"
                     "Please set valid argument.\n");
        return 0;
    }

    if ( ( !ctrl_p->desc_ring[ring_id].rx_desc_ring_flag) && 
         ( !ogma_is_bulk_desc_ring( &ctrl_p->desc_ring[ring_id] ) ) ) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_rx_num.\n"
                     "Please select rx packet desc ring or bulk desc ring.\n");
        return 0;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    if ( !desc_ring_p->running_flag) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_rx_num.\n"
                     "Please select running desc ring.\n");
        return 0;
    }

    result = ogma_read_reg( ctrl_p, pkt_cnt_reg_addr[ring_id]);

    desc_ring_p->rx_num += result;

    if ( desc_ring_p->rx_desc_ring_flag && ( result != 0) ) {
        ogma_inc_desc_head_idx( ctrl_p, desc_ring_p, ( ogma_uint16)result);
    }

    return result;
}

ogma_uint16 ogma_get_tx_done_num (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    )
{
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p = NULL;

    if ( ( ctrl_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ||
         ( !ctrl_p->desc_ring[ring_id].param.valid_flag) ) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_tx_done_num.\n"
                     "Please set valid argument.\n");
        return 0;
    }

    if ( !ctrl_p->desc_ring[ring_id].tx_desc_ring_flag)  {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_tx_done_num.\n"
                     "Please select tx packet desc ring.\n");
        return 0;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    if ( !desc_ring_p->running_flag) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_tx_done_num.\n"
                     "Please select running desc ring.\n");
        return 0;
    }

    ogma_get_tx_done_num_sub( ctrl_p, desc_ring_p);

    return desc_ring_p->tx_done_num;
}

ogma_uint16 ogma_get_tx_avail_num (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    )
{

    ogma_uint16 result;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p = NULL;
    
    if ( ( ctrl_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ||
         ( !ctrl_p->desc_ring[ring_id].param.valid_flag) ) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_tx_avail_num.\n"
                     "Please select valid argument.\n");
        return 0;
    }

    if ( !ctrl_p->desc_ring[ring_id].tx_desc_ring_flag ) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_tx_avail_num.\n"
                     "Please select tx packet desc ring or bulk desc ring.\n");
        return 0;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    if ( !desc_ring_p->running_flag) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_tx_avail_num.\n"
                     "Please select running desc ring.\n");
        return 0;
    }

    result = ogma_get_tx_avail_num_sub( ctrl_p, desc_ring_p);

    return result;
}

ogma_err_t ogma_clean_tx_desc_ring (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    )
{
    ogma_uint32 tmp;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p;
    ogma_tx_desc_entry_t *tx_desc_entry_p;

    if ( ( ctrl_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ) {
        return OGMA_ERR_PARAM;
    }

    if ( !ctrl_p->desc_ring[ring_id].param.valid_flag) {
        return OGMA_ERR_NOTAVAIL;
    }

    if ( !ctrl_p->desc_ring[ring_id].tx_desc_ring_flag) {
        return OGMA_ERR_PARAM;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    ogma_get_tx_done_num_sub( ctrl_p, desc_ring_p);
	
    while( ( desc_ring_p->tail_idx != desc_ring_p->head_idx) ||
           desc_ring_p->full_flag) {

        tx_desc_entry_p =
            (ogma_tx_desc_entry_t *)
            ( ( pfdep_cpu_addr_t)desc_ring_p->desc_ring_cpu_addr +
              desc_ring_p->desc_entry_len * desc_ring_p->tail_idx);

        tmp = tx_desc_entry_p->attr;

        if ( ( tmp & ( 1UL << desc_ring_own_field[ring_id]) ) != 0) {
            break;
        }

        pfdep_free_pkt_buf (
            ctrl_p->dev_handle,
            desc_ring_p->frag_info_p[desc_ring_p->tail_idx].len,
            desc_ring_p->frag_info_p[desc_ring_p->tail_idx].addr,
            desc_ring_p->frag_info_p[desc_ring_p->tail_idx].phys_addr,
            ( ( ( tmp >> OGMA_TX_PKT_DESC_RING_LS_FIELD) & 0x1) != 0),
            desc_ring_p->priv_data_p[desc_ring_p->tail_idx].pkt_handle
            );

        pfdep_memset( &desc_ring_p->frag_info_p[desc_ring_p->tail_idx],
                      0,
                      sizeof( ogma_frag_info_t) );

        ogma_inc_desc_tail_idx( ctrl_p, desc_ring_p, 1);

        if ( ( tmp & ( 1UL << OGMA_TX_PKT_DESC_RING_LS_FIELD) ) != 0) {

            pfdep_assert( desc_ring_p->tx_done_num != 0);

            desc_ring_p->tx_done_num--;
        }
    }

    return OGMA_ERR_OK;
}

ogma_err_t ogma_clean_rx_desc_ring (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    )
{

    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p;
	UINT32 sr;
	
    if ( ( ctrl_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ) {
        return OGMA_ERR_PARAM;
    }

    if ( !ctrl_p->desc_ring[ring_id].param.valid_flag) {
        return OGMA_ERR_NOTAVAIL;
    }

    if ( !ctrl_p->desc_ring[ring_id].rx_desc_ring_flag) {
        return OGMA_ERR_PARAM;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    while( desc_ring_p->full_flag ||
           ( desc_ring_p->tail_idx != desc_ring_p->head_idx) ) {

        ogma_set_rx_desc_entry(
            ctrl_p,
            desc_ring_p,
            desc_ring_p->tail_idx,
            &desc_ring_p->frag_info_p[desc_ring_p->tail_idx],
            desc_ring_p->priv_data_p[desc_ring_p->tail_idx].pkt_handle);

		sr = BF_Arm_Lock( &netsec_cs );
        --desc_ring_p->rx_num;
		BF_Arm_Unlock( &netsec_cs, sr );
		
        ogma_inc_desc_tail_idx( ctrl_p, desc_ring_p, 1);
    }

    pfdep_assert( desc_ring_p->rx_num == 0); //error check

    return OGMA_ERR_OK;
}

ogma_err_t ogma_set_tx_pkt_data (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    const ogma_tx_pkt_ctrl_t *tx_pkt_ctrl_p,
    ogma_uint8 scat_num,
    const ogma_frag_info_t *scat_info_p,
    pfdep_pkt_handle_t pkt_handle
    )
{
    ogma_uint i;
    ogma_uint16 tx_avail_num;
    ogma_err_t ogma_err = OGMA_ERR_OK;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p;

    if ( ( ctrl_p == NULL) ||
         ( tx_pkt_ctrl_p == NULL) ||
         ( scat_info_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ) {
        return OGMA_ERR_PARAM;
    }

    if ( !ctrl_p->desc_ring[ring_id].param.valid_flag) {
        return OGMA_ERR_NOTAVAIL;
    }

    if ( !ctrl_p->desc_ring[ring_id].tx_desc_ring_flag) {
        return OGMA_ERR_PARAM;
    }

    if ( tx_pkt_ctrl_p->pass_through_flag &&
         tx_pkt_ctrl_p->direct_sa_flag ) {
        return OGMA_ERR_DATA;
    }

    if ( tx_pkt_ctrl_p->force_target_desc_ring_flag &&
         ( tx_pkt_ctrl_p->pass_through_flag
           || tx_pkt_ctrl_p->direct_sa_flag
             ) ) {
        return OGMA_ERR_DATA;
    }

	if( ring_id != OGMA_DESC_RING_ID_PKT_TX) {
		return OGMA_ERR_DATA;
	}
	
    if ( tx_pkt_ctrl_p->tcp_seg_offload_flag && 
         ( !tx_pkt_ctrl_p->enc_flag ||
           !tx_pkt_ctrl_p->cksum_offload_flag) ) {
        return OGMA_ERR_DATA;
    }

    if ( tx_pkt_ctrl_p->tcp_seg_offload_flag) {
        if( ( tx_pkt_ctrl_p->seg_len < OGMA_TCP_SEG_LEN_MIN) ||
            ( tx_pkt_ctrl_p->seg_len > OGMA_TCP_SEG_LEN_MAX) ) {
            return OGMA_ERR_DATA;
        }
    } else {
        if (!tx_pkt_ctrl_p->auto_fragment_flag &&
			tx_pkt_ctrl_p->seg_len != 0) {
            return OGMA_ERR_DATA;
        }
    }

    if ( tx_pkt_ctrl_p->direct_sa_flag) {
        if ( tx_pkt_ctrl_p->stid >= OGMA_SDB_ENTRY_NUM) {
            return OGMA_ERR_DATA;
        }
    } else {
        if ( tx_pkt_ctrl_p->stid != 0) {
            return OGMA_ERR_DATA;
        }
    }

    if ( scat_num == 0) {
        return OGMA_ERR_RANGE;
    }

    for ( i = 0; i < scat_num; i++) {
        if ( ( scat_info_p[i].len == 0) ||
             ( scat_info_p[i].len > 0xffffU) ) {
            pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                         "An error occurred at ogma_set_tx_pkt_data.\n"
                         "Pleas check scat_info_p[%u].len.\n",
                         i);
            return OGMA_ERR_DATA;
        }
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    if ( !desc_ring_p->running_flag) {
        ogma_err = OGMA_ERR_NOTAVAIL;
        goto end;
    }

    tx_avail_num = ogma_get_tx_avail_num_sub( ctrl_p, desc_ring_p);

    if ( scat_num > tx_avail_num ) {
        ogma_err = OGMA_ERR_BUSY;
        goto end;
    }

    for ( i = 0; i < scat_num; i++) {
        ogma_set_tx_desc_entry(
            ctrl_p,
            desc_ring_p,
            desc_ring_p->head_idx,
            tx_pkt_ctrl_p,
            ( i == 0),
            ( i == ( scat_num - 1U) ),
            &scat_info_p[i],
            pkt_handle);
        ogma_inc_desc_head_idx( ctrl_p, desc_ring_p, 1);
    }

    ogma_write_reg( ctrl_p,
					OGMA_REG_ADDR_PKT_TX_CNT,
                    (ogma_uint32)1);
end:

    return ogma_err;
}

ogma_err_t ogma_get_rx_pkt_data (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    ogma_rx_pkt_info_t *rx_pkt_info_p,
    ogma_frag_info_t *frag_info_p,
    ogma_uint16 *len_p,
    pfdep_pkt_handle_t *pkt_handle_p
    )
{

    ogma_err_t ogma_err = OGMA_ERR_OK;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p;
    ogma_frag_info_t tmp_frag_info;

    pfdep_pkt_handle_t tmp_pkt_handle;

    if ( ( ctrl_p == NULL) ||
         ( rx_pkt_info_p == NULL) ||
         ( frag_info_p == NULL) ||
         ( len_p == NULL) ||
         ( pkt_handle_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ) {
        return OGMA_ERR_PARAM;
    }

    if ( !ctrl_p->desc_ring[ring_id].param.valid_flag) {
        return OGMA_ERR_NOTAVAIL;
    }

    if ( !ctrl_p->desc_ring[ring_id].rx_desc_ring_flag) {
        return OGMA_ERR_PARAM;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    if ( !desc_ring_p->running_flag) {
        ogma_err = OGMA_ERR_NOTAVAIL;
        goto end;
    }


    if ( desc_ring_p->rx_num == 0 ) {
        ogma_err = OGMA_ERR_INVALID;
        goto end;
    }

    tmp_frag_info.len = OGMA_RX_PKT_BUF_LEN;

    pfdep_read_mem_barrier();

    if ( ( pfdep_alloc_pkt_buf (
			   ctrl_p->dev_handle,
			   tmp_frag_info.len,
			   &tmp_frag_info.addr,
			   &tmp_frag_info.phys_addr,
			   &tmp_pkt_handle) ) != PFDEP_ERR_OK) {
        ogma_set_rx_desc_entry( ctrl_p,
                                desc_ring_p,
                                desc_ring_p->tail_idx,
                                &desc_ring_p->frag_info_p[desc_ring_p->tail_idx],
                                desc_ring_p->priv_data_p[desc_ring_p->tail_idx].pkt_handle);
        ogma_err = OGMA_ERR_ALLOC;

    } else {

        ogma_get_rx_desc_entry( ctrl_p,
                                desc_ring_p,
                                desc_ring_p->tail_idx,
                                rx_pkt_info_p,
                                frag_info_p,
                                len_p,
                                pkt_handle_p);

        ogma_set_rx_desc_entry( ctrl_p,
                                desc_ring_p,
                                desc_ring_p->tail_idx,
                                &tmp_frag_info,
                                tmp_pkt_handle);
    }

    ogma_inc_desc_tail_idx( ctrl_p, desc_ring_p, 1);
	
    --desc_ring_p->rx_num;
end:

    return ogma_err;
}



void ogma_get_strm_data_lpbr_desc_entry (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
    ogma_uint16	idx,
    ogma_bool *first_flag_p,
    ogma_bool *last_flag_p,
    ogma_bool *err_p,
    ogma_uint8 *err_code_p,
    ogma_frag_info_t *out_frag_info_p,
    ogma_uint32 *len_p,
	ogma_uint32 *stid_p,
	ogma_uint32 *dtid_p,
	ogma_uint32 *lti_p,
    void **priv_data_pp
    )
{
    ogma_lpb_desc_entry_t lpb_desc_entry;

    pfdep_memcpy( &lpb_desc_entry,
                  (void *) ( ( pfdep_cpu_addr_t)desc_ring_p->desc_ring_cpu_addr +
                             desc_ring_p->desc_entry_len * idx),
				  desc_ring_p->desc_entry_len );
    
    *len_p = lpb_desc_entry.data_len;

	*lti_p  =
		( lpb_desc_entry.attr >> OGMA_LPB_RX_DESC_RING_LTI_FIELD) & 0x1;

    *first_flag_p =
        ( lpb_desc_entry.attr >> OGMA_LPB_RX_DESC_RING_FS_FIELD) & 0x1;

    *last_flag_p =
        ( lpb_desc_entry.attr >> OGMA_LPB_RX_DESC_RING_LS_FIELD) & 0x1;

	*stid_p = ((lpb_desc_entry.attr >> OGMA_LPB_RX_DESC_RING_STID_LOW_FIELD)& 0xf) |
		((lpb_desc_entry.attr >> OGMA_LPB_RX_DESC_RING_STID_HIGH_FIELD)& 0x10);
	*dtid_p = (lpb_desc_entry.attr >> OGMA_LPB_RX_DESC_RING_DTID_FIELD)& 0xf;

    out_frag_info_p->phys_addr = desc_ring_p->frag_info_p[idx].phys_addr;
    out_frag_info_p->addr = desc_ring_p->frag_info_p[idx].addr;
    out_frag_info_p->len = desc_ring_p->frag_info_p[idx].len;
	
    if ( priv_data_pp != NULL) {
        *priv_data_pp = desc_ring_p->priv_data_p[idx].bulk_priv_data_p;
    }

}

ogma_err_t ogma_get_strm_data_lpbr (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    ogma_bool  *err_p,
    ogma_uint8 *err_code_p,
    ogma_frag_info_t *out_frag_info_p,
    ogma_uint32 *out_scat_num_p,
    ogma_uint32 *len_p,
	ogma_uint32 *stid_p,
	ogma_uint32 *dtid_p,
	ogma_uint32 *lti_p,
    void **priv_data_pp
    )
{
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p;
    ogma_bool first_flag, last_flag;
	ogma_uint32 i;
    ogma_err_t ogma_err = OGMA_ERR_OK;
    ogma_frag_info_t tmp_frag_info;
	UINT32 sr;
	
    if ( ( ctrl_p == NULL) ||
         ( out_frag_info_p == NULL) ||
         ( len_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_bulk_data.\n"
                     "Please set valid argument.\n");
        return OGMA_ERR_PARAM;
    }

    if ( !ctrl_p->desc_ring[ring_id].param.valid_flag) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_bulk_data.\n"
                     "Please select valid desc ring.\n");
        return OGMA_ERR_NOTAVAIL;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    if ( !desc_ring_p->running_flag) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_bulk_data.\n"
                     "Please select running desc ring.\n");
        return OGMA_ERR_NOTAVAIL;
    }

    if ( desc_ring_p->rx_num == 0) {
        return OGMA_ERR_INVALID;
    }

	for(i=0; i<*out_scat_num_p;i++) {
        ogma_get_strm_data_lpbr_desc_entry(
            ctrl_p,
            desc_ring_p,
            desc_ring_p->tail_idx,
            &first_flag,
            &last_flag,
            err_p,
            err_code_p,
            &out_frag_info_p[i],
            len_p,
			stid_p,
			dtid_p,
			lti_p,
            priv_data_pp);

		tmp_frag_info.len       = desc_ring_p->param.rx_buf_len;
 		tmp_frag_info.addr      = out_frag_info_p[i].addr;
 		tmp_frag_info.phys_addr = out_frag_info_p[i].phys_addr;

		ogma_set_rx_desc_entry( ctrl_p,
                                desc_ring_p,
                                desc_ring_p->tail_idx,
                                &tmp_frag_info,
                                NULL);

        ogma_inc_desc_tail_idx( ctrl_p, desc_ring_p, 1);
		
        if ( last_flag) {
            break;
        }
    }
	
	sr = BF_Arm_Lock( &netsec_cs );
	--desc_ring_p->rx_num;
	BF_Arm_Unlock( &netsec_cs, sr );
	
    return ogma_err;

}


ogma_err_t ogma_set_strm_data (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    const ogma_strm_ctrl_t *strm_ctrl_p,
    ogma_uint8 scat_num,
    const ogma_frag_info_t *scat_info_p,
    pfdep_pkt_handle_t pkt_handle,
	ogma_bool       kick
    )
{
    ogma_uint i;
    ogma_uint16 tx_avail_num;
    ogma_err_t ogma_err = OGMA_ERR_OK;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p;
	
    if ( ( ctrl_p == NULL) ||
         ( strm_ctrl_p == NULL) ||
         ( scat_info_p == NULL) ||
         ( ring_id > OGMA_DESC_RING_ID_MAX) ) {
        return OGMA_ERR_PARAM;
    }

    if ( !ctrl_p->desc_ring[ring_id].param.valid_flag) {
        return OGMA_ERR_NOTAVAIL;
    }

    if ( !ctrl_p->desc_ring[ring_id].tx_desc_ring_flag) {
        return OGMA_ERR_PARAM;
    }

    if ( scat_num == 0) {
        return OGMA_ERR_RANGE;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    if ( !desc_ring_p->running_flag) {
        ogma_err = OGMA_ERR_NOTAVAIL;
        goto end;
    }

	tx_avail_num = ogma_get_tx_avail_num_sub( ctrl_p, desc_ring_p);

    if ( scat_num > tx_avail_num ) {
        ogma_err = OGMA_ERR_BUSY;
        goto end;
    }

    for ( i = 0; i < scat_num; i++) {
		ogma_set_strm_desc_entry(
			ctrl_p,
			desc_ring_p,
			desc_ring_p->head_idx,
			strm_ctrl_p,
			( i == 0),
			( i == ( scat_num - 1U) ),
			&scat_info_p[i],
			pkt_handle);
		ogma_inc_desc_head_idx( ctrl_p, desc_ring_p, 1);
    }

	if(kick == OGMA_TRUE) {
		ogma_write_reg( ctrl_p,	desc_ring_tx_cnt_reg_addr[ring_id], (ogma_uint32)1);
	}
	
end:

	return ogma_err;
}


ogma_err_t ogma_set_strm_data_lpbr (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t tx_ring_id,
    ogma_desc_ring_id_t rx_ring_id,
    const ogma_strm_ctrl_t *strm_ctrl_p,
    ogma_uint8 in_scat_num,
    const ogma_frag_info_t *in_scat_info_p,
    ogma_uint8 out_scat_num,
    const ogma_frag_info_t *out_scat_info_p,
    pfdep_pkt_handle_t pkt_handle,
	ogma_bool       kick
    )
{
    ogma_uint i;
    ogma_uint16 tx_avail_num;
    ogma_err_t ogma_err = OGMA_ERR_OK;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *tx_desc_ring_p;
    ogma_desc_ring_t *rx_desc_ring_p;

    if ( ( ctrl_p == NULL) ||
         ( strm_ctrl_p == NULL) ||
         ( in_scat_info_p == NULL) ||
         ( tx_ring_id > OGMA_DESC_RING_ID_MAX) ||
         ( rx_ring_id > OGMA_DESC_RING_ID_MAX) ) {
        return OGMA_ERR_PARAM;
    }

    if ( !ctrl_p->desc_ring[tx_ring_id].param.valid_flag || 
		 !ctrl_p->desc_ring[rx_ring_id].param.valid_flag) {
        return OGMA_ERR_NOTAVAIL;
    }

    if ( !ctrl_p->desc_ring[tx_ring_id].tx_desc_ring_flag ||
		 !ctrl_p->desc_ring[tx_ring_id].tx_desc_ring_flag) {
        return OGMA_ERR_PARAM;
    }

    if ( in_scat_num == 0 || out_scat_num == 0) {
        return OGMA_ERR_RANGE;
    }

    tx_desc_ring_p = &ctrl_p->desc_ring[tx_ring_id];
    rx_desc_ring_p = &ctrl_p->desc_ring[rx_ring_id];

    if ( !tx_desc_ring_p->running_flag) {
        ogma_err = OGMA_ERR_NOTAVAIL;
        goto end;
    }

    tx_avail_num = ogma_get_tx_avail_num_sub( ctrl_p, tx_desc_ring_p);

    if ( in_scat_num > tx_avail_num ) {
        ogma_err = OGMA_ERR_BUSY;
        goto end;
    }

    for ( i = 0; i < in_scat_num; i++) {
		ogma_set_strm_desc_entry(
			ctrl_p,
			tx_desc_ring_p,
			tx_desc_ring_p->head_idx,
			strm_ctrl_p,
			( i == 0),
			( i == ( in_scat_num - 1U) ),
			&in_scat_info_p[i],
			pkt_handle);
		ogma_inc_desc_head_idx( ctrl_p, tx_desc_ring_p, 1);
    }

    for ( i = 0; i < out_scat_num; i++) {
        ogma_set_rx_lpbr_desc_entry (
            ctrl_p,
            rx_desc_ring_p,
            rx_desc_ring_p->head_idx,
            ( i == 0),
            ( i == ( out_scat_num - 1U) ),
            &out_scat_info_p[i],
            NULL
            );
        ogma_inc_desc_head_idx( ctrl_p, rx_desc_ring_p, 1);
    }

	if(kick == OGMA_TRUE) {
		ogma_write_reg( ctrl_p,	desc_ring_tx_cnt_reg_addr[tx_ring_id], (ogma_uint32)1);
	}

end:

    return ogma_err;
}


ogma_err_t ogma_kick_tx_count_reg(
    ogma_handle_t       ogma_handle,
    ogma_desc_ring_id_t ring_id,
	ogma_uint32         kick_num
	)
{
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
	
	ogma_write_reg( ctrl_p,	desc_ring_tx_cnt_reg_addr[ring_id], kick_num);

	return OGMA_ERR_OK;
}
