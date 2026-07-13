/**
 * ogma_api.h
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#ifndef OGMA_API_H
#define OGMA_API_H

#include "ogma_config.h"
#include "ogma_basic_type.h"
#include "pfdep.h"
/*
 * Check configuration macro settings.
 */



#ifdef OGMA_CONFIG_CLK_HZ
#if ( (OGMA_CONFIG_CLK_HZ < 0x100000) || (OGMA_CONFIG_CLK_HZ > 0x10000000) )
#error "OGMA_CONFIG_CLK_HZ is not appropriate."
#endif /* ( (OGMA_CONFIG_CLK_HZ < 0x100000) || (OGMA_CONFIG_CLK_HZ > 0x10000000) ) */
#else /* ! OGMA_CONFIG_CLK_HZ */
#error "OGMA_CONFIG_CLK_HZ is not given."
#endif /* OGMA_CONFIG_CLK_HZ */

#define OGMA_SDB_ENTRY_NUM      32
#define OGMA_CONTEXTDB_ENTRY_NUM  32
#define OGMA_SDB_ENTRY_NUM_MAX OGMA_SDB_ENTRY_NUM
#define OGMA_CONTEXTDB_ENTRY_NUM_MAX OGMA_CONTEXTDB_ENTRY_NUM

/**
 * Number of Descriptor ring id
 */
#define OGMA_DESC_RING_ID_PKT_TX      0
#define OGMA_DESC_RING_ID_PKT_RX      1
#define OGMA_DESC_RING_ID_LPBE_RX     3
#define OGMA_DESC_RING_ID_LPBR_RX     5
#define OGMA_DESC_RING_ID_VSUB_TX     12
#define OGMA_DESC_RING_ID_TS_TX      14
#define OGMA_DESC_RING_ID_MAX        14
#define OGMA_DESC_RING_ID_NULL        15
/**
 * Numbre of TCP Segmentation length limits
 */
#define OGMA_TCP_SEG_LEN_MAX 1460
#define OGMA_TCP_SEG_LEN_MIN  536

/**
 * Number of checksum calculation result for received packet
 */
#define OGMA_RX_CKSUM_RESULT_OK       0x1
#define OGMA_RX_CKSUM_RESULT_NG       0x2
#define OGMA_RX_CKSUM_RESULT_NOTAVAIL 0x0

/**
 * Number of top interrupt enable register bit field
 */
#define OGMA_TOP_IRQ_REG_SUB         (1UL << 21)
#define OGMA_TOP_IRQ_REG_ME_START    (1UL << 20)
#define OGMA_TOP_IRQ_REG_MAC         (1UL << 19)
#define OGMA_TOP_IRQ_REG_PKT         (1UL << 18) 
#define OGMA_TOP_IRQ_REG_STRM        (1UL << 17)
#define OGMA_TOP_IRQ_REG_TS_TX       (1UL << 14)
#define OGMA_TOP_IRQ_REG_LPBR_RX     (1UL <<  5)
#define OGMA_TOP_IRQ_REG_LPBE_RX     (1UL <<  3)
#define OGMA_TOP_IRQ_REG_PKT_RX      (1UL <<  1)
#define OGMA_TOP_IRQ_REG_PKT_TX      (1UL <<  0)


/**
 *  Number of top channel enable register bit field
 */
#define OGMA_CH_IRQ_REG_EMPTY   (1UL << 17)
#define OGMA_CH_IRQ_REG_ERR     (1UL << 16)
#define OGMA_CH_IRQ_REG_PKT_CNT (1UL << 15)
#define OGMA_CH_IRQ_REG_TIMEUP  (1UL << 14)
#define OGMA_CH_IRQ_REG_RCV     (OGMA_CH_IRQ_REG_PKT_CNT | OGMA_CH_IRQ_REG_TIMEUP)

/**
 * Number of don't fragment control setting
 */ 
#define OGMA_DF_CONTROL_CLEAR 0x02
#define OGMA_DF_CONTROL_SET   0x01
#define OGMA_DF_CONTROL_COPY  0x00

/**
 * Number of ipsec tunnel setting
 */
#define OGMA_IPSEC_MODE_TUNNEL_IPV4_OVER_IPV4 0x00
#define OGMA_IPSEC_MODE_TUNNEL_IPV4_OVER_IPV6 0x04
#define OGMA_IPSEC_MODE_TUNNEL_IPV6_OVER_IPV6 0x05
#define OGMA_IPSEC_MODE_TUNNEL_IPV6_OVER_IPV4 0x01
#define OGMA_IPSEC_MODE_TRANSPORT_IPV4        0x02
#define OGMA_IPSEC_MODE_TRANSPORT_IPV6        0x07

/**
 * Number of auto fragment length limits
 */
#define OGMA_AUTOFRAG_MTU_LEN_MAX 1500
#define OGMA_AUTOFRAG_MTU_LEN_MIN 576

/**
 * Number of lifetime invalidation setting
 */
#define OGMA_LIFETIME_INFINITY 0xffff

/**
 * Number of various limits
 */
#define OGMA_DESC_ENTRY_NUM_MIN        2 
#define OGMA_DESC_ENTRY_NUM_MAX        2047
#define OGMA_REAS_TMR_MAX              32767


/**
 * Number of ogma phy interface setting
 */
#define OGMA_PHY_INTERFACE_GMII  0
#define OGMA_PHY_INTERFACE_RGMII 1
#define OGMA_PHY_INTERFACE_RMII  4

/**
 * Number of ogma link speed setting
 */
#define OGMA_PHY_LINK_SPEED_1G   0
#define OGMA_PHY_LINK_SPEED_100M 1
#define OGMA_PHY_LINK_SPEED_10M  2


/**
 * Number of flow control limits
 */
#define OGMA_FLOW_CTRL_START_THRESHOLD_MAX 7
#define OGMA_FLOW_CTRL_STOP_THRESHOLD_MAX  8
#define OGMA_FLOW_CTRL_PAUSE_TIME_MIN      5

enum ogma_err_e{
    OGMA_ERR_OK = 0,
    OGMA_ERR_PARAM,
    OGMA_ERR_ALLOC,
    OGMA_ERR_BUSY,
    OGMA_ERR_RANGE,
    OGMA_ERR_DATA,
    OGMA_ERR_NOTAVAIL,
    OGMA_ERR_INTERRUPT,
    OGMA_ERR_AGAIN,
    OGMA_ERR_INVALID
};

typedef void *ogma_handle_t;
typedef struct ogma_param_s ogma_param_t;
typedef struct ogma_pkt_ctrl_param_s ogma_pkt_ctrl_param_t;
typedef struct ogma_desc_ring_param_s ogma_desc_ring_param_t;
typedef struct ogma_sa_data_s ogma_sa_data_t;
typedef struct ogma_tnhd_data_s ogma_tnhd_data_t;
typedef struct ogma_mib_data_s ogma_mib_data_t;
typedef struct ogma_enc_mib_data_s ogma_enc_mib_data_t;
typedef struct ogma_dec_mib_data_s ogma_dec_mib_data_t;
typedef enum ogma_err_e ogma_err_t;
typedef ogma_uint8 ogma_desc_ring_id_t;
typedef ogma_uint8 ogma_stid_t;
typedef ogma_uint8 ogma_cdb_idx_t;
typedef struct ogma_tx_pkt_ctrl_s ogma_tx_pkt_ctrl_t;
typedef struct ogma_rx_pkt_info_s ogma_rx_pkt_info_t;
typedef struct ogma_tls_hdr_info_s ogma_tls_hdr_info_t;
typedef struct ogma_frag_info_s ogma_frag_info_t;
typedef struct ogma_strm_ctrl_s ogma_strm_ctrl_t;

struct ogma_pkt_ctrl_param_s{
    ogma_uint ets:1;
    ogma_uint vjs:1;
    ogma_uint en_jumbo:1;
    ogma_uint en_id_rep:1;
    ogma_uint log_chksum_er_flag:1;
    ogma_uint log_hd_imcomplete_flag:1;
    ogma_uint log_hd_er_flag:1;

	// Stream Ctrl Param
    ogma_uint tts:1;
    ogma_uint rfm:1;
    ogma_uint jrm:1;
    ogma_uint ppm:1;
};

struct ogma_desc_ring_param_s{
    ogma_uint valid_flag:1;
    ogma_uint16 entry_num;
    ogma_uint   little_endian:1;
    ogma_uint   tmr_mode:1;
    ogma_uint16 irq_ast_cnt;
    ogma_uint32 rx_buf_len;
};

struct ogma_param_s{

    ogma_uint reas_tmr_unit_ms_flag:1;
	
    ogma_uint lifetime_unit_10s_flag:1;

    ogma_uint8 mac_addr[6];

    ogma_uint16 reas_tmr;

    ogma_uint32 init_iv[4];

    ogma_pkt_ctrl_param_t pkt_ctrl_param;

    ogma_desc_ring_param_t desc_ring_param[OGMA_DESC_RING_ID_MAX+1];

};

struct ogma_tnhd_data_s{
    ogma_uint8 tos;
    ogma_uint8 ttl;
    ogma_uint16 identification;
    ogma_uint32 flow_label;
    ogma_uint8 src_ip_addr[16];
    ogma_uint8 dst_ip_addr[16];
};

struct ogma_dec_mib_data_s{
    ogma_uint32 dropped_by_alignment_err_pkt_num;
    ogma_uint32 dropped_by_auth_err_pkt_num;
    ogma_uint32 dropped_by_anti_replay_pkt_num;
    ogma_uint32 dropped_by_dec_err_pkt_num;
    ogma_uint32 dropped_by_inner_pkt_err_pkt_num;
};

struct ogma_enc_mib_data_s{
    ogma_uint32 dropped_by_seq_num_err_pkt_num;
    ogma_uint32 dropped_by_cannot_fragment_err_pkt_num;
    ogma_uint32 dropped_by_already_fragment_in_transport_mode_err_pkt_num;
    ogma_uint32 fragmented_pkt_num;
};

struct ogma_mib_data_s{
    ogma_uint32 received_pkt_num;
    ogma_uint32 dropped_by_hard_limit_pkt_num;
    union {
        ogma_dec_mib_data_t dec_mib;
        ogma_enc_mib_data_t enc_mib;
    }u;
};

struct ogma_tx_pkt_ctrl_s{
    ogma_uint pass_through_flag:1;
    ogma_uint force_target_desc_ring_flag:1;
    ogma_uint enc_flag:1;
    ogma_uint cksum_offload_flag:1;
    ogma_uint ts_latch_flag:1;
    ogma_uint tcp_seg_offload_flag:1;
    ogma_uint auto_fragment_flag:1;
    ogma_uint direct_sa_flag:1;
    ogma_desc_ring_id_t target_desc_ring_id;
    ogma_stid_t stid;
    ogma_uint16 seg_len;
    ogma_desc_ring_id_t desc_ring_id;
    ogma_uint lpbe_flag:1;
    ogma_uint mark_flag:1;
    ogma_uint lpbr_flag:1;
};


struct ogma_rx_pkt_info_s{
    ogma_uint matched_flag:1;
    ogma_uint enc_flag:1;
    ogma_uint fragmented_flag:1;
    ogma_uint err_flag:1;
    ogma_uint rx_cksum_result:2;
    ogma_uint8 err_code;
    ogma_stid_t stid;
    ogma_uint32 ts_high;
    ogma_uint32 ts_low;
};

struct ogma_strm_ctrl_s{
    ogma_desc_ring_id_t target_desc_ring_id;	
    ogma_uint drid;
    ogma_uint mark_flag:1;
    ogma_uint lpbr_flag:1;
    ogma_uint lpbe_flag:1;
    ogma_uint ast_int_flag:1;
    ogma_uint cr_flag:1;
    ogma_uint fn_flag:1;
    ogma_uint si_flag:1;
    ogma_uint enc_flag:1;
    ogma_uint dtid;
    ogma_stid_t stid;
	ogma_uint32 in_len;
	void       *in_addr;
    ogma_uint32 timestamp;
	ogma_uint32 wait_time;
};

struct ogma_frag_info_s{
    pfdep_phys_addr_t phys_addr;
    void *addr;
    ogma_uint32 len;
};

typedef struct ogma_stat_info_s {
    ogma_uint16 max_busy_entry_num[OGMA_DESC_RING_ID_MAX + 1];
} ogma_stat_info_t;


/**************************
***************************
***************************/

ogma_err_t ogma_init(
    void *base_addr,
    pfdep_dev_handle_t dev_handle,
    const ogma_param_t *param_p,
    ogma_handle_t *ogma_handle_p
    );

void ogma_terminate (
    ogma_handle_t ogma_handle
    );

ogma_err_t ogma_soft_init(
    void *base_addr,
    pfdep_dev_handle_t dev_handle,
    const ogma_param_t *param_p,
    ogma_handle_t *ogma_handle_p
    );

ogma_err_t ogma_set_tnhd_data (
    ogma_handle_t ogma_handle,
    ogma_stid_t idx,
    const ogma_tnhd_data_t *tnhd_data_p,
    ogma_bool over_ipv4_tunnel_flag,
    ogma_bool overwrtite_id_flag
    );

ogma_err_t ogma_get_mib_data (
    ogma_handle_t ogma_handle,
    ogma_bool enc_flag,
    ogma_stid_t idx,
    ogma_bool clear_flag,
    ogma_mib_data_t *mib_data_p
    );

void ogma_set_phy_reg (
    ogma_handle_t ogma_handle,
    ogma_uint8 phy_addr,
    ogma_uint8 reg_addr,
    ogma_uint16 value
    );
    
ogma_uint16 ogma_get_phy_reg (
    ogma_handle_t ogma_handle,
    ogma_uint8 phy_addr,
    ogma_uint8 reg_addr
    );


ogma_uint32 ogma_get_top_irq_status (
    ogma_handle_t ogma_handle,
    ogma_bool mask_flag
    );

ogma_uint32 ogma_get_top_irq_status_non_clear (
    ogma_handle_t ogma_handle,
    ogma_bool mask_flag
    );

ogma_uint32 ogma_get_desc_ring_irq_status (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    ogma_bool mask_flag
    );

ogma_uint32 ogma_get_pkt_irq (
    ogma_handle_t ogma_handle
    );

/* ogma_desc_ring_access.c */
ogma_err_t ogma_start_desc_ring (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    );

ogma_err_t ogma_stop_desc_ring (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    );

ogma_uint16 ogma_get_rx_num (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    );

ogma_uint16 ogma_get_new_rx_num (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    );

ogma_uint16 ogma_get_tx_done_num (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    );

ogma_uint16 ogma_get_tx_avail_num (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    );

ogma_desc_ring_id_t ogma_get_drid (
	ogma_handle_t ogma_handle,
	ogma_uint16 stid
	);

ogma_err_t ogma_clean_tx_desc_ring(
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    );

ogma_err_t ogma_clean_rx_desc_ring(
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id
    );

ogma_err_t ogma_set_tx_pkt_data (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    const ogma_tx_pkt_ctrl_t *tx_pkt_ctrl_p,
    ogma_uint8 scat_num,
    const ogma_frag_info_t *scat_info_p,
    pfdep_pkt_handle_t pkt_handle
    );

ogma_err_t ogma_get_rx_pkt_data (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    ogma_rx_pkt_info_t *rx_pkt_info_p,
    ogma_frag_info_t *frag_info_p,
    ogma_uint16 *len_p,
    pfdep_pkt_handle_t *pkt_handle_p
    );

ogma_err_t ogma_enable_top_irq (
    ogma_handle_t ogma_handle,
    ogma_uint32 irq_factor
    );

ogma_err_t ogma_disable_top_irq (
    ogma_handle_t ogma_handle,
    ogma_uint32 irq_factor
    );

ogma_err_t ogma_enable_desc_ring_irq (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    ogma_uint32 irq_factor
    );

ogma_err_t ogma_disable_desc_ring_irq (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    ogma_uint32 irq_factor
    );

ogma_err_t ogma_enable_pkt_irq (
    ogma_handle_t ogma_handle,
    ogma_uint32 irq_factor
    );

ogma_err_t ogma_disable_pkt_irq (
    ogma_handle_t ogma_handle,
    ogma_uint32 irq_factor
    );

/**************************
***************************
***************************/

/**
 * Get statistics information.
 */
ogma_err_t ogma_get_stat_info (
    ogma_handle_t ogma_handle,
    ogma_stat_info_t *stat_info_p,
    ogma_bool clear_flag
    );

ogma_err_t ogma_set_strm_data (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    const ogma_strm_ctrl_t *strm_ctrl_p,
    ogma_uint8 scat_num,
    const ogma_frag_info_t *scat_info_p,
    pfdep_pkt_handle_t pkt_handle,
	ogma_bool       kick
    );

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
    );

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
    );

ogma_err_t ogma_kick_tx_count_reg(
    ogma_handle_t       ogma_handle,
    ogma_desc_ring_id_t ring_id,
	ogma_uint32         kick_num
	);


#endif /* OGMA_API_H*/
