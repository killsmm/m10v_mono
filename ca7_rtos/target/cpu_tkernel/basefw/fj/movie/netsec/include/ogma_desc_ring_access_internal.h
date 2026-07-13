/**
 * ogma_desc_ring_access_internal.h
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#ifndef OGMA_DESC_RING_ACCESS_INTERNAL_H
#define OGMA_DESC_RING_ACCESS_INTERNAL_H

#include "ogma_basic_type.h"
#include "ogma_internal.h"

#define OGMA_TX_PKT_DESC_RING_OWN_FIELD        (31)
#define OGMA_TX_PKT_DESC_RING_LD_FIELD         (30)
#define OGMA_TX_PKT_DESC_RING_DRID_FIELD       (24)
#define OGMA_TX_PKT_DESC_RING_TDRID_FIELD      (16)
#define OGMA_TX_PKT_DESC_RING_LPBE_FIELD       (15)
#define OGMA_TX_PKT_DESC_RING_AF_FIELD         (11)
#define OGMA_TX_PKT_DESC_RING_FS_FIELD         (9)
#define OGMA_TX_PKT_DESC_RING_LS_FIELD         (8)
#define OGMA_TX_PKT_DESC_RING_CO_FIELD         (7)
#define OGMA_TX_PKT_DESC_RING_SO_FIELD         (6)
#define OGMA_TX_PKT_DESC_RING_TRS_FIELD        (4)

#define OGMA_RX_PKT_DESC_RING_OWN_FIELD        (31)
#define OGMA_RX_PKT_DESC_RING_LD_FIELD         (30)
#define OGMA_RX_PKT_DESC_RING_SDRID_FIELD      (24)
#define OGMA_RX_PKT_DESC_RING_FR_FIELD         (23)
#define OGMA_RX_PKT_DESC_RING_ER_FIELD         (21)
#define OGMA_RX_PKT_DESC_RING_ERROR_CODE_FIELD (16)
#define OGMA_RX_PKT_DESC_RING_ID_HIGH_FIELD    (16)
#define OGMA_RX_PKT_DESC_RING_TDRID_FIELD      (12)
#define OGMA_RX_PKT_DESC_RING_FS_FIELD         (9)
#define OGMA_RX_PKT_DESC_RING_LS_FIELD         (8)
#define OGMA_RX_PKT_DESC_RING_CO_FIELD         (6)
#define OGMA_RX_PKT_DESC_RING_MATCH_FIELD      (5)
#define OGMA_RX_PKT_DESC_RING_EDS_FIELD        (4)
#define OGMA_RX_PKT_DESC_RING_ID_FIELD         (0)

#define OGMA_TX_STRM_DESC_RING_OWN_FIELD	   (31)
#define OGMA_TX_STRM_DESC_RING_LD_FIELD		   (30)
#define OGMA_TX_STRM_DESC_RING_DRID_FIELD	   (24)
#define OGMA_TX_STRM_DESC_RING_MARK_FIELD	   (21)
#define OGMA_TX_STRM_DESC_RING_LPBR_FIELD	   (20)
#define OGMA_TX_STRM_DESC_RING_STID_HIGH_FIELD (16)
#define OGMA_TX_STRM_DESC_RING_LPBE_FIELD	   (15)
#define OGMA_TX_STRM_DESC_RING_AST_INT_FIELD   (14)
#define OGMA_TX_STRM_DESC_RING_CR_FIELD		   (13)
#define OGMA_TX_STRM_DESC_RING_FN_FIELD		   (12)
#define OGMA_TX_STRM_DESC_RING_SI_FIELD		   (11)
#define OGMA_TX_STRM_DESC_RING_FS_FIELD		   (9)
#define OGMA_TX_STRM_DESC_RING_LS_FIELD		   (8)
#define OGMA_TX_STRM_DESC_RING_ENC_FIELD	   (7)
#define OGMA_TX_STRM_DESC_RING_DTID_FIELD	   (4)
#define OGMA_TX_STRM_DESC_RING_STID_FIELD	   (0)
#define OGMA_TX_STRM_DESC_RING_STID_LOW_FIELD  (0)

#define OGMA_LPB_RX_DESC_RING_OWN_FIELD		  (31)
#define OGMA_LPB_RX_DESC_RING_LD_FIELD		  (30)
#define OGMA_LPB_RX_DESC_RING_SDRID_FIELD	  (24)
#define OGMA_LPB_RX_DESC_RING_LTI_FIELD	      (18)
#define OGMA_LPB_RX_DESC_RING_STID_HIGH_FIELD (16)
#define OGMA_LPB_RX_DESC_RING_TDRID_FIELD     (12)
#define OGMA_LPB_RX_DESC_RING_FS_FIELD		   (9)
#define OGMA_LPB_RX_DESC_RING_LS_FIELD		   (8)
#define OGMA_LPB_RX_DESC_RING_DTID_FIELD	   (4)
#define OGMA_LPB_RX_DESC_RING_STID_LOW_FIELD   (0)

static const ogma_uint32 ogma_desc_start_reg_addr[OGMA_DESC_RING_ID_MAX+1] = {
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
  	(ogma_uint32)OGMA_REG_ADDR_LPBE_RX_DESC_START,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_LPBR_RX_DESC_START,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_VSUB_TX_DESC_START,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_TS_TX_DESC_START
};

static const ogma_uint32 pkt_cnt_reg_addr[OGMA_DESC_RING_ID_MAX+1] = {
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_LPBE_RX_CNT,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_LPBR_RX_CNT,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_VSUB_TX_DONE_CNT,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_TS_TX_DONE_CNT
};

static const ogma_uint32 pkt_int_cnt_reg_addr[OGMA_DESC_RING_ID_MAX+1] = {
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_LPBE_RX_RXINT_CNT,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_LPBR_RX_RXINT_CNT,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_VSUB_TX_DONE_TXINT_CNT,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_TS_TX_DONE_TXINT_CNT
};

const ogma_uint32 desc_ring_irq_inten_reg_addr[OGMA_DESC_RING_ID_MAX + 1] = {
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_LPBE_RX_INTEN,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_LPBR_RX_INTEN,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_VSUB_TX_INTEN,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_TS_TX_INTEN
};

const ogma_uint32 desc_ring_irq_inten_set_reg_addr[OGMA_DESC_RING_ID_MAX + 1] = {
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_LPBE_RX_INTEN_SET,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_LPBR_RX_INTEN_SET,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_VSUB_TX_INTEN_SET,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_TS_TX_INTEN_SET
};

const ogma_uint32 desc_ring_irq_inten_clr_reg_addr[OGMA_DESC_RING_ID_MAX + 1] = {
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_LPBE_RX_INTEN_CLR,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_LPBR_RX_INTEN_CLR,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_VSUB_TX_INTEN_CLR,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_TS_TX_INTEN_CLR
};

const ogma_uint32 desc_ring_own_field[OGMA_DESC_RING_ID_MAX + 1] = {
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_LPB_RX_DESC_RING_OWN_FIELD,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_LPB_RX_DESC_RING_OWN_FIELD,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_TX_STRM_DESC_RING_OWN_FIELD,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_TX_STRM_DESC_RING_OWN_FIELD
};


const ogma_uint32 desc_ring_tx_cnt_reg_addr[OGMA_DESC_RING_ID_MAX + 1] = {
	(ogma_uint32)NULL,
    (ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_VSUB_TX_CNT,
	(ogma_uint32)NULL,
	(ogma_uint32)OGMA_REG_ADDR_TS_TX_CNT
};

static const ogma_uint32 ogma_desc_es_drid[OGMA_SDB_ENTRY_NUM_MAX] = {
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
};

static const ogma_uint32 ogma_desc_ts_drid[OGMA_SDB_ENTRY_NUM_MAX] = {
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_VSUB_TX,
  	(ogma_uint32)OGMA_DESC_RING_ID_VSUB_TX,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL    ,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_TS_TX,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_TS_TX,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_TS_TX,
	(ogma_uint32)OGMA_DESC_RING_ID_TS_TX,
	(ogma_uint32)OGMA_DESC_RING_ID_TS_TX,
	(ogma_uint32)OGMA_DESC_RING_ID_VSUB_TX,
	(ogma_uint32)OGMA_DESC_RING_ID_VSUB_TX,
	(ogma_uint32)OGMA_DESC_RING_ID_VSUB_TX,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
	(ogma_uint32)OGMA_DESC_RING_ID_NULL,
};

typedef struct ogma_tx_desc_entry_s ogma_tx_desc_entry_t;
typedef struct ogma_rx_desc_entry_s ogma_rx_desc_entry_t;

struct ogma_tx_desc_entry_s{

    ogma_uint32 attr;

    ogma_uint32 data_buf_addr;

    ogma_uint32 buf_len_info;

    ogma_uint32 timestamp;
};

struct ogma_rx_desc_entry_s{

    ogma_uint32 attr;

    ogma_uint32 data_buf_addr;

    ogma_uint32 buf_len_info;

    ogma_uint32 reserved;


    ogma_uint32 ts_high;

    ogma_uint32 ts_low;

    ogma_uint32 reserved1;

    ogma_uint32 reserved2;

};

/* OGMA_CONFIG_USE_PKT_DESC_RING */


typedef struct ogma_bulk_desc_entry_s ogma_bulk_desc_entry_t;
typedef struct ogma_lpb_desc_entry_s ogma_lpb_desc_entry_t;
typedef struct ogma_tls_desc_info_s ogma_tls_desc_info_t;

struct ogma_lpb_desc_entry_s{
    ogma_uint32 attr;
    ogma_uint32 out_data_buf_addr;
    ogma_uint32 data_len;
    ogma_uint32 reserved;
};

struct ogma_tls_desc_info_s{
    const ogma_tls_hdr_info_t *tls_hdr_info_p;
    ogma_uint8 major_ver;
    ogma_uint8 minor_ver;
    ogma_uint32 len;
};


static __inline ogma_bool ogma_is_pkt_desc_ring (
    const ogma_desc_ring_t *desc_ring_p
    )
{
    return ( desc_ring_p->pkt_desc_ring_flag);
}

static __inline ogma_bool ogma_is_bulk_desc_ring (
    const ogma_desc_ring_t *desc_ring_p
    )
{
    return ( desc_ring_p->bulk_desc_ring_flag);
}

static __inline ogma_bool ogma_is_strm_desc_ring (
    const ogma_desc_ring_t *desc_ring_p
    )
{
    return ( desc_ring_p->strm_desc_ring_flag);
}

static __inline void ogma_check_desc_own_sanity (
    ogma_ctrl_t *ctrl_p,
    const ogma_desc_ring_t *desc_ring_p,
    ogma_uint16 idx,
    ogma_uint expected_own)
{

}

#endif /* OGMA_DESC_RING_ACCESS_INTERNAL_H */
