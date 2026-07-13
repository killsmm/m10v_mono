/**
 * @file		updater.h
 * @brief		updater control header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _UPDATER_H_
#define _UPDATER_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define SDRAM_PARAM_NUM		(sizeof(SDRAM_PARAMETER) / sizeof(unsigned long))	// number of SDRAM_PARAMETER
#define BOOT_HEADER_NUM				(8)		// version - k_size
#define BOOT_PARAM_NUM				(12)	// version - tggle_time
#define NAND_FLASH_PARAMETER_CNT	(26)	// NAND_FLASH_PARAMETER
#define PARTITION_NUM				(16)	// PARTITION DATA num
#define PARTITION_PARAMETER_CNT		(PARTITION_NUM*4+1)	// number of PARDATA.par

#define PARTITION_TYPE_NAND_PARAM		0x0000
#define PARTITION_TYPE_SDRAM_PARAM		0x0001
#define PARTITION_TYPE_PARTITION_PARAM	0x0002
#define PARTITION_TYPE_BOOTSTRAP		0x0003	// boot image
#define PARTITION_TYPE_PARTITION_DATA	0x0004
#define PARTITION_TYPE_BOOT_PARAM		0x0005
// --- REMOVE_SECURE BEGIN ---
#define PARTITION_TYPE_SECURE_PARAM		0x0006
// --- REMOVE_SECURE END ---
#define PARTITION_TYPE_SELECTOR			0x0010

// --- REMOVE_SECURE BEGIN ---
/* Secure Parameter *************************************************/
#define DIGEST_NUM						(16)			// Message digest num 
#define PARTITION_PARAMETER_ALL			(0xFF)
#define PARTITION_PARAMETER_MDgst		(0xFE)
// --- REMOVE_SECURE END ---

#define FLASHDATA_SRAM_AREA				(SDRAM_ADR_CH0_RTOS_IMAGE_WORK)
#define RAMCHECK_AREA					(FLASHDATA_SRAM_AREA + 0x100000) 
#define READ_FILE_DATA_AREA				(RAMCHECK_AREA +       0x100000) 

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

// Boot Parameter structure definition
typedef struct {
	unsigned long	version;
	unsigned long	copy_addr;
	unsigned long	exec_addr;
	unsigned long	image_size;
	unsigned long	image_offset;
	unsigned long	dg_offset;
	unsigned long	sr_offset;
	unsigned long	k_size;
	unsigned long	retry_count;
	unsigned long	boot_notice;
	unsigned long	error_notice;
	unsigned long	toggle_time;
	unsigned long	rsvd[2];
	unsigned long	dvc_select;
	char        	LEDSEL1[4];
	char        	LEDSEL2[4];
	unsigned long	linux_enable;
} T_BOOT_PARAMETER;			// BootParameter   BOOT_PAR


// BOOT PARAMETER
typedef struct {
	unsigned long bmfRetryModeEna;		/**< enable control of retry mode */
	unsigned long bmfToggleTImeError;	/**< toggle for time error */
} T_BOOT_MODE_FIELDS;

typedef struct {
	unsigned long bhVersion;			/**< version number				*/
	unsigned long bhCopyAddr;			/**< copy address(SRAM)			*/
	unsigned long bhExcuteAddr;			/**< excute address				*/
	unsigned long bhImageSize;			/**< image size					*/
	unsigned long bhImageOffset;		/**< image offset				*/
	unsigned long bhDOffset;			/**< d offset					*/
	unsigned long bhSNOffset;			/**< sn offset					*/
	unsigned long bhKSize;				/**< k size						*/
	unsigned long bhReserved[6];		/**< reserved					*/
	T_BOOT_MODE_FIELDS bhBootMode;		/**< boot mode					*/
} boot_header;

typedef struct {
	unsigned long	DDR_SDRAM_TM_SPEC0;
	unsigned long	DDR_SDRAM_TM_SPEC1;
	unsigned long	DDR_SDRAM_TM_SPEC2;
	unsigned long	DDR_SDRAM_TM_SPEC3;
	unsigned long	DDR_SDRAM_TM_SPEC4;
	unsigned long	DDR_SDRAM_MRW0;
	unsigned long	DDR_SDRAM_MRW1;
	unsigned long	DDR_PHY_INIT2;
	unsigned long	DDR_IO_INIT1;
	unsigned long	DDR_IO_INIT3;
	unsigned long	DDR_PZQ_INIT;
	unsigned long	DDR_TR_MODE;
	unsigned long	DDR_TXCA_CH_DLY;
	unsigned long	DDR_TXDQS_BYTE_DLY0;
	unsigned long	DDR_TXDQS_BYTE_DLY1;
	unsigned long	DDR_TXDQS_BYTE_DLY2;
	unsigned long	DDR_TXDQS_BYTE_DLY3;
	unsigned long	DDR_RXRTT_BYTE_DLY0;
	unsigned long	DDR_RXRTT_BYTE_DLY1;
	unsigned long	DDR_RXRTT_BYTE_DLY2;
	unsigned long	DDR_RXRTT_BYTE_DLY3;
	unsigned long	DDR_RDDATA_LAT0;
	unsigned long	DDR_RDDATA_LAT1;
	unsigned long	DDR_RDDATA_LAT2;
	unsigned long	DDR_RDDATA_LAT3;
	unsigned long	DDR_TR_SEQ;
	unsigned long	DDR_TR_PTN_CA;
	unsigned long	DDR_TR_PTN_WRDE;
	unsigned long	DDR_TXCA_CH_RANGE;
	unsigned long	DDR_TXDQS_RANGE0;
	unsigned long	DDR_TXDQS_RANGE1;
	unsigned long	DDR_TXDQS_RANGE2;
	unsigned long	DDR_TXDQS_RANGE3;
	unsigned long	DDR_RXDQS_BYTE_RANGE0;
	unsigned long	DDR_RXDQS_BYTE_RANGE1;
	unsigned long	DDR_RXDQS_BYTE_RANGE2;
	unsigned long	DDR_RXDQS_BYTE_RANGE3;
	unsigned long	DDR_TXDQ_BYTE_RANGE0;
	unsigned long	DDR_TXDQ_BYTE_RANGE1;
	unsigned long	DDR_TXDQ_BYTE_RANGE2;
	unsigned long	DDR_TXDQ_BYTE_RANGE3;
	unsigned long	UMCINITCTLA;			/**< DRAM initialization command interval setting A.		*/
	unsigned long	UMCINITCTLB;			/**< DRAM initialization command interval setting B.		*/
	unsigned long	UMCINITCTLC;			/**< DRAM initialization command interval setting C.		*/
	unsigned long	UMCDRMMRA;				/**< DRAM initialization MR setting.						*/
	unsigned long	UMCMEMCONF0A;			/**< DRAM parameter	setting A-Rank0.						*/
	unsigned long	UMCMEMCONF0B;			/**< DRAM parameter	setting B-Rank0.						*/
	unsigned long	UMCMEMCONF1A;			/**< DRAM parameter	setting A-Rank1.						*/
	unsigned long	UMCMEMCONF1B;			/**< DRAM parameter	setting B-Rank1.						*/
	unsigned long	UMCMEMCONFCH;			/**< DRAM parameter	setting Channel.						*/
	unsigned long	UMCMEMMAPSET;			/**< DRAM mapping setting.									*/
	unsigned long	UMCCMDCTLA;				/**< DRAM command interval setting A.						*/
	unsigned long	UMCCMDCTLB;				/**< DRAM command interval setting B.						*/
	unsigned long	UMCCMDCTLC;				/**< DRAM command interval setting C.						*/
	unsigned long	UMCCMDCTLD;				/**< DRAM command interval setting D.						*/
	unsigned long	UMCCMDCTLE;				/**< DRAM command interval setting E.						*/
	unsigned long	UMCCMDCTLF;				/**< DRAM command interval setting F.						*/
	unsigned long	UMCCMDCTLG;				/**< DRAM command interval setting G.						*/
	unsigned long	UMCCMDCTLH;				/**< DRAM command interval setting H.						*/
	unsigned long	UMCCMDCTLI;				/**< DRAM command interval setting I.						*/
	unsigned long	UMCCMDCTLJ;				/**< DRAM command interval setting J.						*/
	unsigned long	UMCRDATACTL_D0;			/**< DRAM read data interval control 0.						*/
	unsigned long	UMCRDATACTL_D1;			/**< DRAM read data interval control 1.						*/
	unsigned long	UMCWDATACTL_D0;			/**< DRAM write data interval control 0.					*/
	unsigned long	UMCWDATACTL_D1;			/**< DRAM write data interval control 1.					*/
	unsigned long	UMCDATASET;				/**< DRAM command issue timing control.						*/
	unsigned long	UMCWDATASWP;			/**< DRAM write data swap.									*/
	unsigned long	REMAPADDR;				/**< Remap address setting.									*/
} sdram_parameter;

typedef struct {
    unsigned long    nf_clk_sel;
    unsigned long    common_settings;
    unsigned long    toggle_timings_0;
    unsigned long    toggle_timings_1;
    unsigned long    async_toggle_timings;
    unsigned long    sync_timings;
    unsigned long    timings0;
    unsigned long    timings1;
    unsigned long    timings2;
    unsigned long    dll_phy_ctrl;
    unsigned long    phy_ctrl_reg;
    unsigned long    phy_tsel_reg;
    unsigned long    phy_dq_timing_reg;
    unsigned long    phy_dqs_timing_reg;
    unsigned long    phy_gate_lpbk_ctrl_reg;
    unsigned long    phy_dll_master_ctrl_reg;
    unsigned long    phy_dll_slave_ctrl_reg;
    unsigned long    timing_mode;
    unsigned long    dbcnt;
    unsigned long    ecc_config;
    unsigned long    skip_bytes_conf;
    unsigned long    skip_bytes_offset;
    unsigned long    rsvd;
    unsigned long    cache_read_enable;
    unsigned long    FACTOR_CHECK;
    unsigned long    factor_check_search_page;
} nand_parameter;


typedef struct {
	boot_header			header;				/**< boot header				*/
	unsigned long		ptable_sel[3];		/**< partition table selector	*/
	unsigned long		PLLCNTL1_B;			/**< PLLCNTL1 Register (Before)	*/
	unsigned long		PLLCNTL1_A;			/**< PLLCNTL1 Register (After)	*/
	unsigned long		PLLCNTL7;			/**< PLLCNTL7 Register			*/
	sdram_parameter		sdram_parh[2];		/**< sdram parameter			*/
	nand_parameter		nand_par;			/**< nand parameter				*/
} boot_parameter;
// NAND BLOCK zero data


typedef struct _SDRAM_PARAMETER {
	unsigned long	PLLCNTL1_B;
	unsigned long	PLLCNTL1_A;
	unsigned long	PLLCNTL7;
	sdram_parameter	ch[2];		// SDI parameter
} SDRAM_PARAMETER;
// for recovery

// PARTITION Parameter structure definition
typedef struct {
	unsigned long	area_size;
	unsigned long	load_addr;
	unsigned long	load_size;
	unsigned long	jump_addr;
} PARTITION_PAR_DETAIL;

typedef struct {
	unsigned long			start_block;
	PARTITION_PAR_DETAIL	data[PARTITION_NUM];
} PARTITION_PARAMETER;


// PARTITION TABLE structure definition
typedef struct {
	unsigned char  signature[8];
	unsigned short type;
	unsigned short param;
	unsigned long  datalen;
} T_BF_UPDATER_DATA_HEAD;

// --- REMOVE_SECURE BEGIN ---
// Secure Parameter structure definition
typedef struct {
	unsigned long	data[8];
} MESSAGE_DIGEST;

typedef struct {
	unsigned long		serial_number;
	MESSAGE_DIGEST		image_digest;
	MESSAGE_DIGEST		ms_dg[DIGEST_NUM];
} SECURE_PARAMETER;
// --- REMOVE_SECURE END ---

/** Hash Data */
typedef struct {
	unsigned long		addr;		/**< data address */
	unsigned long		size;		/**< data size */
} T_HASH_DATA;



/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

FJ_ERR_CODE fj_updater(UCHAR* filename);
FJ_ERR_CODE fj_updater_memory(UCHAR area, UINT32 *data_addr, UINT32 size);
FJ_ERR_CODE fj_updater_selector(VOID);
FJ_ERR_CODE fj_updater_memory_all( UINT32 *data_addr, UINT32 size);
FJ_ERR_CODE fj_updater_dump_partition(UINT32 part_idx);
FJ_ERR_CODE fj_updater_dump_block(UINT32 blk_idx, UINT8 *block_buf);
FJ_ERR_CODE fj_updater_save_block(UINT32 blk_idx, UINT8 *block_buf);
FJ_ERR_CODE fj_updater_set_secure_enable(BOOL en);

#ifdef __cplusplus
}
#endif

#endif	// _UPDATER_H_
