/**
 * ogma_driver_api.h
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#ifndef OGMA_DRIVER_API_H
#define OGMA_DRIVER_API_H

#include "ogma_config.h"
#include "ogma_config.h"
#include "ogma_basic_type.h"
#include "pfdep.h"

#define NETSEC_CTL_SET_SESSION_BASIC_DB    0
#define NETSEC_CTL_SET_SESSION_DB          1
#define NETSEC_CTL_SET_SESSION_DB_MPTS_ST  2
#define NETSEC_CTL_SET_SESSION_DB_MPTS_PG  3
#define NETSEC_CTL_SET_TIME_INFO           4
#define NETSEC_CTL_PROC_STRM_DATA0         5
#define NETSEC_CTL_PROC_STRM_DATA1         6
#define NETSEC_CTL_PROC_STRM_DATA_RAW      7
#define NETSEC_CTL_GET_STRM_DATA0	       8
#define NETSEC_CTL_GET_STRM_DATA1          9

#define NETSEC_SESSION_ID_VIDEO_0 12
#define NETSEC_SESSION_ID_VIDEO_1  2
#define NETSEC_SESSION_ID_AUDIO_0  7
#define NETSEC_SESSION_ID_AUDIO_1  3

#define NETSEC_SESSION_ID_MPTS_STRM 16
#define NETSEC_SESSION_ID_MPTS_PG_0 17
#define NETSEC_SESSION_ID_MPTS_PG_1 18
#define NETSEC_SESSION_ID_MPTS_PG_2 19

typedef struct {
	unsigned int drid;
	unsigned long timing_rate;
} T_BF_NETSEC_TIME_INFO_PARAM;

typedef struct {
 	void         *in_addr;
	unsigned int  in_len;
	unsigned int  drid;
	unsigned int  dtid;
	unsigned int  mark;
	unsigned int  lpbr;
	unsigned int  lpbe;
	unsigned int  ast_int;
	unsigned int  cr;
	unsigned int  fn;
	unsigned int  si;
	unsigned int  enc;
	unsigned int  stid;
	unsigned long timestamp;
	unsigned long wait_time;
} T_BF_NETSEC_PROC_STRM_PARAM;

typedef struct {
 	void         *out_addr;
	unsigned int  out_len;
	unsigned int  stid;
	unsigned int  dtid;
	unsigned int  lti;
	unsigned long wait_time;
} T_BF_NETSEC_GET_STRM_PARAM;

typedef struct {
	unsigned int  id;
	unsigned long payload_length;
	unsigned long dst_mac_addr_up;
	unsigned long dst_mac_addr_low;
	unsigned long src_mac_addr_up;
	unsigned long src_mac_addr_low;
	unsigned long eth_type;
	unsigned long tos;
	unsigned long init_id;
	unsigned long ttl;
	unsigned long src_ip_addr;
	unsigned long dst_ip_addr;
} T_BF_NETSEC_SESSION_BASIC_DB_PARAM;

typedef struct {
	unsigned int  id;
	unsigned int  base_id;
	unsigned int  src_port;
	unsigned int  dst_port;
	unsigned long payload_type;
	unsigned long ssrc;
	unsigned long random_offset;
	unsigned long init_seqnum;
	unsigned long pid_strm;
	unsigned long pid_pmt;
	unsigned long pid_pcr;
} T_BF_NETSEC_SESSION_DB_PARAM;

typedef struct {
	unsigned int  id;
	unsigned long payload_length;
	unsigned long pid_psi0;
	unsigned long pid_psi1;
	unsigned long pid_psi2;
	unsigned long pid_psi3;
} T_BF_NETSEC_SESSION_DB_MPTS_ST_PARAM;

typedef struct {
	unsigned int  id;
	unsigned long payload_length;
	unsigned long pid_pmt;
	unsigned long pid_pcr;
	unsigned long pid_video;
	unsigned long pid_audio;
	unsigned long pid_mtdat;
} T_BF_NETSEC_SESSION_DB_MPTS_PG_PARAM;

typedef struct {
	unsigned long mem_start_addr;
	unsigned long dma_mem_cpu_start_addr;
	unsigned long dma_mem_phys_start_addr;
	unsigned long ts0_tx_desc_ring_num;
	unsigned long ts1_tx_desc_ring_num;
	unsigned long lpb0_rx_desc_ring_num;
	unsigned long lpb1_rx_desc_ring_num;
	unsigned long lpb0_buf_size;
	unsigned long lpb1_buf_size;
	unsigned int  ppm;
}T_BF_NETSEC_INIT_PARAM;


void BF_Netsec_init (T_BF_NETSEC_INIT_PARAM *init_param_p);

void BF_Netsec_terminate (void);

void BF_Netsec_clean(void);

ogma_err_t BF_Netsec_ctrl (
	unsigned int request,
    void        *argp
	);

#endif /* OGMA_DRIVER_API_H*/
