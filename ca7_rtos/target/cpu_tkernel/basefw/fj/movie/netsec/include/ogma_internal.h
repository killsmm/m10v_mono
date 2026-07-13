/**
 * ogma_itnernal.h
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#ifndef OGMA_INTERNAL_H
#define OGMA_INTERNAL_H
#include "ogma_api.h"
#include "ogma_reg.h"
#include "pfdep.h"

#define OGMA_INSTANCE_NUM_MAX 8
#define OGMA_RX_PKT_BUF_LEN 1514
#define OGMA_DUMMY_DESC_ENTRY_LEN 48

#define OGMA_FLG_TX_DONE_Vsub  (1 << 10) 
#define OGMA_FLG_TX_DONE_TS    (1 << 11)

extern const ogma_uint32 desc_ring_irq_inten_reg_addr[OGMA_DESC_RING_ID_MAX + 1];
extern const ogma_uint32 desc_ring_irq_inten_set_reg_addr[OGMA_DESC_RING_ID_MAX + 1];
extern const ogma_uint32 desc_ring_irq_inten_clr_reg_addr[OGMA_DESC_RING_ID_MAX + 1];

typedef struct ogma_ctrl_s ogma_ctrl_t;
typedef struct ogma_desc_ring_s ogma_desc_ring_t;
typedef struct ogma_clk_ctrl_s ogma_clk_ctrl_t;
typedef struct ogma_contextdb_info_s ogma_contextdb_info_t;
typedef union ogma_desc_entry_priv_u ogma_desc_entry_priv_t;

struct ogma_clk_ctrl_s{
    ogma_uint8 dmac_req_num;
    ogma_uint32 core_req_num;
    ogma_uint8 ike_req_num;
    ogma_uint8 mcr_req_num;
    ogma_uint8 tmr_req_num;
    ogma_uint8 mac_req_num;
};

struct ogma_contextdb_info_s{
    ogma_uint valid_flag:1;
    ogma_uint strm_mode_flag:1;
    ogma_uint aes_xts_mode_flag:1;
    ogma_uint8 bulk_mode;
    ogma_uint8 icv_len;
    ogma_uint8 block_len;
    ogma_uint8 iv_len;
    ogma_uint16 ref_num;
};

struct ogma_desc_ring_s{

    ogma_desc_ring_id_t ring_id;

    ogma_desc_ring_param_t param;

    ogma_uint rx_desc_ring_flag:1;

    ogma_uint tx_desc_ring_flag:1;

    ogma_uint pkt_desc_ring_flag:1;

    ogma_uint bulk_desc_ring_flag:1;

    ogma_uint strm_desc_ring_flag:1;

    ogma_uint running_flag:1;

    ogma_uint full_flag:1;

    ogma_uint8 desc_entry_len;

    ogma_uint16 head_idx;

    ogma_uint16 tail_idx;

    ogma_uint16 tx_done_num;
	
    ogma_uint16 rx_num;

    pfdep_soft_lock_t soft_lock;

    void *desc_ring_cpu_addr;

    pfdep_phys_addr_t desc_ring_phys_addr;

    ogma_frag_info_t *frag_info_p;

    ogma_desc_entry_priv_t *priv_data_p;
};

struct ogma_ctrl_s{
    ogma_ctrl_t *next_p;

    ogma_uint core_enabled_flag:1;
	
    void *base_addr;

    pfdep_dev_handle_t dev_handle;

    ogma_param_t param;

    ogma_clk_ctrl_t clk_ctrl;

    ogma_uint8 db_valid_num;

    ogma_uint32 rx_pkt_buf_len;

    ogma_contextdb_info_t contextdb_info[OGMA_CONTEXTDB_ENTRY_NUM_MAX];

    ogma_desc_ring_t desc_ring[OGMA_DESC_RING_ID_MAX+1];


    void *dummy_desc_entry_addr;

    pfdep_phys_addr_t dummy_desc_entry_phys_addr;

    /**
     * Statistics information.
     *
     * Note: Since mutual access exclusion is omitted,
     *       these values might be inaccurate.
     */
    ogma_stat_info_t stat_info;

    void *dmac_mh_mcr_data_cpu_addr;

    pfdep_phys_addr_t dmac_mh_mcr_data_phys_addr;

    void *pktc_mcr_data_cpu_addr;

    pfdep_phys_addr_t pktc_mcr_data_phys_addr;

    pfdep_soft_lock_t contextdb_info_lock;
};

union ogma_desc_entry_priv_u{
    pfdep_pkt_handle_t pkt_handle;
    void *bulk_priv_data_p;
};

/*****************************************************************
 *****************************************************************
 *****************************************************************/

/**
 *
 */
ogma_err_t ogma_alloc_desc_ring (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_id_t ring_idx
    );
/**
 *
 */
void ogma_free_desc_ring (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p
    );

/**
 *
 */
ogma_err_t ogma_setup_rx_desc_ring (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p,
	ogma_uint32  pkt_buf_len
    );

/**
 *
 */
void ogma_uninit_pkt_desc_ring (
    ogma_ctrl_t *ctrl_p,
    ogma_desc_ring_t *desc_ring_p
    );

/**
 *
 */
void ogma_push_clk_req (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 domain
    );

void ogma_pop_clk_req(
    ogma_ctrl_t *ctrl_p,
    ogma_uint32	domain
    );

#ifdef OGMA_CONFIG_CHECK_CLK_SUPPLY
#include "ogma_basic_access.h"
/*include for ogma_read_reg*/
static __inline void ogma_check_clk_supply (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 domain
    )
{
    ogma_uint32 value;

    value = ogma_read_reg( ctrl_p, OGMA_REG_ADDR_CLK_EN);

    pfdep_assert( ( ( value & domain ) == domain) );
}

#else

#define ogma_check_clk_supply( ctrl_p, domain)

#endif /* OGMA_CONFIG_CHECK_CLK_SUPPLY */

#endif /* OGMA_INTERNAL_H */
