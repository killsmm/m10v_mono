/**
 * @file netsec_api.h
 * @author 
 * @date 2014/Apr/25-2014/May/xx
 * @brief NETSEC Network Offload Macro device driver
 */
/* Copyright (C) 2015 SOCIONEXT INCORPORATED All Rights Reserved. */


#ifndef __NETSEC_API_H
#define __NETSEC_API_H

#include <stdint.h>

//open
#include <sys/stat.h>
#include <fcntl.h>

//close
#include <unistd.h>

//ioctl
#include <sys/ioctl.h>

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;

#define DEV_NAME "netsec"

#define NETSEC_IOC_MAGIC	0x67

#define NETSEC_IOCINIT      _IOWR( NETSEC_IOC_MAGIC, 0, struct netsec_init_arg )
#define NETSEC_IOCRESET     _IO  ( NETSEC_IOC_MAGIC, 1 )
#define NETSEC_IOCSMODE     _IO  ( NETSEC_IOC_MAGIC, 2 )
#define NETSEC_IOCGMODE     _IOR ( NETSEC_IOC_MAGIC, 3, u32 * )
#define NETSEC_IOCSSESSDB   _IOW ( NETSEC_IOC_MAGIC, 4, struct netsec_sessdb_arg )
#define NETSEC_IOCSCTX      _IOWR( NETSEC_IOC_MAGIC, 5, struct netsec_ctx_arg )
#define NETSEC_IOCGDRINFO   _IOWR( NETSEC_IOC_MAGIC, 6, struct netsec_drinfo_arg )
#define NETSEC_IOCWAITCRYPT _IOW ( NETSEC_IOC_MAGIC, 7, struct netsec_wcrypt_arg )
#define NETSEC_IOCRDCRYPT   _IOW ( NETSEC_IOC_MAGIC, 8, struct netsec_rdcrypt_arg )
#define NETSEC_IOCGTCNT90K  _IOR ( NETSEC_IOC_MAGIC, 9, struct netsec_tcnt32_arg )
#define NETSEC_IOCGTCNT8K   _IOR ( NETSEC_IOC_MAGIC, 10, struct netsec_tcnt32_arg )
#define NETSEC_IOCGTCNT27M  _IOR ( NETSEC_IOC_MAGIC, 11, struct netsec_tcnt64_arg )

#define NETSEC_IOC_MAXNR 12

#define	NUM_RXQ			(512)

/* RTPモード設定 */
#define	NETSEC_MODE_ETS					(0x80000000)	/* ES TS Select */
#define	NETSEC_MODE_VJS					(0x40000000)	/* Video JPEG select */
#define	NETSEC_MODE_EN_JUMBO			(0x08000000)	/* Enable Jumbo */
#define	NETSEC_MODE_EN_ID_REP			(0x04000000)	/* Enable ID replace */
#define	NETSEC_MODE_LOG_CKSM_ER			(0x00000008)	/* Log checksum error */
#define	NETSEC_MODE_LOG_HD_INCOMPLETE	(0x00000004)	/* Log header incomplete error */
#define	NETSEC_MODE_LOG_HD_ER			(0x00000002)	/* Log header error */

#define	NETSEC_MODE_TTS					(0x20000000)	/* Timestamped TS */
#define	NETSEC_MODE_RFM					(0x10000000)	/* RTP Fragment Mode */

#define	NETSEC_MODE_MASK_PKT_CTRL1		(0xc000000e)
#define	NETSEC_MODE_MASK_PKT_CTRL2		(0x000000c0)
#define	NETSEC_REG_MASK_PKT_CTRL2		(0x0c000000)
#define	NETSEC_MODE_SHIFT_PKT_CTRL2		(20)
#define	NETSEC_MODE_MASK_STRM_CTRL		(0x38000000)

#define	NETSEC_SESSDB_STRM_ID_MIN		(0)
#define	NETSEC_SESSDB_STRM_ID_MAX		(12)
#define	NETSEC_SESSDB_BASIC_ID_MIN		(14)
#define	NETSEC_SESSDB_BASIC_ID_MAX		(63)

#define	NETSEC_LPB_RX_BUFF_LEN	(1520)

typedef struct {
	u32		desc0;
	u32		length;
	u32		reserved[2];
	u8		rx_buff[NETSEC_LPB_RX_BUFF_LEN];
} NETSEC_RX_DATA;

typedef struct {
	u32		num;
	u32		wr_count;
	u32		reserved[2];

	NETSEC_RX_DATA	rxque [NUM_RXQ];
} NETSEC_RX_QUE;

struct netsec_init_arg {
	u32				*dmac_hm_me_code;
	u32				dmac_hm_me_size;
	u32				*dmac_mh_me_code;
	u32				dmac_mh_me_size;
	u32				*pktc_code;
	u32				pktc_size;
	u16				num_descriptors [ 4 + 10 ];
	u32				rx_buf_size[ 4 ];
	u32				size_ctx;
	NETSEC_RX_QUE	*lpbk_rx_que;
	u8				mac[6];
};

#define	NETSEC_SESS_DB_NUM_ITEM		(16)

struct netsec_sessdb_arg {
	u32				sess_db_id;
	u32				sess_db[NETSEC_SESS_DB_NUM_ITEM];
};


struct netsec_ctx_arg {
	u8				*ctx_data;
	u32				ctx_size;
	u32				ctx_offset;	
	u32				ctx_addr;
};

struct netsec_drinfo_arg {
	u16				desc_id;
	u16				free;
	u16				total;
};

enum netsec_dring_id {
	NETSEC_DESC_RING_ID_PKT_TX		= 0x00,
	NETSEC_DESC_RING_ID_PKT_RX		= 0x01,
	NETSEC_DESC_RING_ID_VDO_TX		= 0x02,
	NETSEC_DESC_RING_ID_ETH_LPB_RX	= 0x03,
	NETSEC_DESC_RING_ID_ADO_TX		= 0x04,
	NETSEC_DESC_RING_ID_RTP_LPB_RX	= 0x05,
	NETSEC_DESC_RING_ID_CDB_TX		= 0x06,
	
	NETSEC_DESC_RING_ID_CRY_TX		= 0x08,
	NETSEC_DESC_RING_ID_CRY_RX		= 0x09,
	NETSEC_DESC_RING_ID_JPG1_TX		= 0x0a,
	NETSEC_DESC_RING_ID_JPG2_TX		= 0x0b,
	NETSEC_DESC_RING_ID_VSUB_TX		= 0x0c,
	NETSEC_DESC_RING_ID_MDT_TX		= 0x0d,
	NETSEC_DESC_RING_ID_TS_TX		= 0x0e,
};

#define	NETSEC_WCRYPT_MAX_PKTS			(4)

struct netsec_wcrypt_arg {
	u32				timeout;
	u32				recv_pkts;
	u32				recv_sizes[NETSEC_WCRYPT_MAX_PKTS];
};

struct netsec_rdcrypt_arg {
	u8				*recv_buf;
	u32				recv_size;
	u32				recv_len;
};


struct netsec_tcnt32_arg {
	u32				tcount;
};

struct netsec_tcnt64_arg {
	u32				upper;
	u32				lower;
};



/*! uniqID */
typedef union {
	u32 word;
	struct {
		u32	ct:8;		/*!< 0:data 1:RTCP、2:Bitrate */
		u32	rs:8;		/*!< Stream id */
		u32	fn:8;		/*!< Frame Index */
		u32	dc:8;		/*!< RTCP'sIndex */
	}bit;
} U_NETSEC_DESC_ID;
// write: Descriptor
struct netsec_desc {
	U_NETSEC_DESC_ID				uniq_id;
	u32				desc[4];
};

// read: completion
struct netsec_comp {
	u32				uniq_id;
	u32				result;
};
// for session db
struct session_db_info{
	int fd;
	int src_port;
	int dst_port;
	unsigned long ssrc;
	unsigned long yuv_pix_num;
	unsigned long ntp;
	char* str_dst_ip;
	char* str_dst_mac;
	char sess_db_id;
	char es_vid;
	char rtp_payload_type;
	unsigned sessdb08;
	unsigned sessdb09;
	unsigned sessdb10;
	unsigned sessdb11;
};
struct ts_descriptor_info{
	int fd;
	int paddr;
	int len;
	int ts;
	int fmt;
	int es_vid;
	int nvcl;
};
struct jpeg_descriptor_info{
	int fd;
	int paddr;
	int len;
	int ts;
	int fmt;
	int es_vid;
	int nvcl;
	//for quantization table
	int payload_pa;
	int qtable_cnt;
	int qtable_ofs[4];
	int qtable_sz[4];
};
#define	NSEC_H264 0x00000000
#define	NSEC_HEVC 0x00080000
#define	NSEC_UYVY 0x00040000
#define NSEC_NVCL 0x00000010

#define NSEC_YUV_VID   0
#define NSEC_META_VID  29 //29, 30
#define NSEC_AUDIO_VID 31
void NSec_set_Videobps_Descriptor(struct ts_descriptor_info p, unsigned long bps);
void NSec_set_sessiondb(struct session_db_info p);
void NSec_set_Video_Tx_Descriptor(struct ts_descriptor_info p,U_NETSEC_DESC_ID uniq_id);
void NSec_set_rtcp_Tx_Descriptor(struct ts_descriptor_info p,U_NETSEC_DESC_ID uniq_id);
void NSec_set_meta_Tx_Descriptor(struct ts_descriptor_info p,U_NETSEC_DESC_ID uniq_id);
void NSec_set_audio_Tx_Descriptor(struct ts_descriptor_info p,U_NETSEC_DESC_ID uniq_id);
void NSec_set_jpeg2_Tx_Descriptor(struct jpeg_descriptor_info p,U_NETSEC_DESC_ID uniq_id);
unsigned long rx_data(char *dt, unsigned long r );
void NSec_set_rtp_lpb_buffer(int fd, NETSEC_RX_QUE *rxq );
void NSec_set_streamdb_rtp_lpb(struct session_db_info p);
void NSec_set_Video_rtp_lpb_Descriptor(struct ts_descriptor_info p);
void NSec_set_audio_rtp_lpb_Descriptor(struct ts_descriptor_info p);
void NSec_set_jpeg2_rtp_lpb_Descriptor(struct jpeg_descriptor_info p);
#endif	/* __NETSEC_API_H */
