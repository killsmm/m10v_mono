/**
 * @file		mw_user_custom.h
 * @brief		common process for media drivers
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _MW_USER_CUSTOM_H
#define _MW_USER_CUSTOM_H

#include "../../include/media.h"


/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/

typedef signed short		FW_SHORT;
typedef unsigned short		FW_USHORT;
typedef signed long 		FW_LONG;
typedef unsigned long		FW_ULONG;
typedef signed long long	FW_LLONG;
typedef unsigned long long	FW_ULLONG;
typedef char				FW_CHAR;
typedef unsigned char		FW_UCHAR;
typedef int					FW_INT32;
typedef unsigned int 	 	FW_UINT32;

#define CO_MEM_BARI_ON
//#define CO_MEDIA_DRIVER_DEBUG_READ_WRITE_TIME

#define D_FLAG_WAIT_AND		0x00
#define D_FLAG_WAIT_OR		0x02

#ifndef NULL
#define NULL	(0)
#endif

#define D_SEM_TIMEOUT		(5000)

// sector size
#define D_MW_SECTOR_SIZE	(512)

// SDXC Card Support
#define	D_SD_UHS2_CLK			(51851)						// UHS-II Clock      (1400000/27)
#define	D_SD_UHS2_CLK_46		(46666)						// UHS-II Clock      (1400000/30)
#define	D_SD_UHS2_CLK_42		(42424)						// UHS-II Clock      (1400000/33)
#define	D_SD_UHS2_CLK_38		(38888)						// UHS-II Clock      (1400000/36)
#define	D_SD_UHS2_CLK_35		(35897)						// UHS-II Clock      (1400000/39)
#define	D_SD_UHS2_CLK_33		(33333)						// UHS-II Clock      (1400000/42)
#define	D_SD_UHS2_CLK_29		(29166)						// UHS-II Clock      (1400000/48)
#define	D_SD_UHS2_CLK_25		(25825)						// UHS-II Clock      (1400000/54)
#define	D_SD_SDR104_CLK			(175000)					// UHS-I SDR104 Clock ch0 (700000/4)
#define	D_SD_SDR104_CLK_1		(200000)					// UHS-I SDR104 Clock ch1 (200000*16/4)
#define	D_SD_SDR50_CLK			(100000)					// UHS-I SDR50  Clock ch0 (400000/4)
#define	D_SD_SDR50_CLK_1		(100000)					// UHS-I SDR50  Clock ch1 (200000*16/8)
#define	D_SD_BASE_CLK			(50000)						// SD Base Clock     (200000/4)
#define	D_SD_TRN_CLK			(25000)						// Transfer Clock    (100000/4)
#define	D_SD_ID_CLK				(400)						// ID Clock 400KHz   (min:=D_SD_BASE_CLK/256)

#define	D_SD_MAX_CLK_SPEED		(175000)					// SD Clock Speed Max ch0 175.0MHz
#define	D_SD_MAX_CLK_SPEED_1	(200000)					// SD Clock Speed Max ch1  51.851MHz

#define D_SD_NO_CARD		(0)				/**< No card */
#define D_SD_ANY_CARD		(1)				/**< Card is inserted */

#define D_SD_RAM_PD_WAIT_TIME	(1)

// NF attribute
#define D_NF_CE_1					(1)
#define D_NF_CE_2					(2)
#define D_NF_DEVICE_BLOCK_NUM_1024	(1024)
#define D_NF_DEVICE_BLOCK_NUM_2048	(2048)
#define D_NF_DEVICE_BLOCK_NUM_4096	(4096)
#define D_NF_DEVICE_BLOCK_NUM_8192	(8192)
#define D_NF_MAX_BAD_BLOCK_NUM_24	(24)
#define D_NF_MAX_BAD_BLOCK_NUM_48	(48)
#define D_NF_MAX_BAD_BLOCK_NUM_96	(96)
#define D_NF_MAX_BAD_BLOCK_NUM_192	(192)
#define D_NF_SEARCH_OTHER_PAGE_1	(1)
#define D_NF_SEARCH_OTHER_PAGE_31	(31)
#define D_NF_SEARCH_OTHER_PAGE_63	(63)
#define D_NF_SEARCH_OTHER_PAGE_127	(127)
#define D_NF_SEARCH_OTHER_PAGE_255	(255)
#define D_NF_SEARCH_OTHER_PAGE_DIS	(0xFFFF)

// NF Maker ID
#define D_MICRON_MID (0x2C)
#define D_HYNIX_MID (0xAD)
#define D_SUMSUNG_MID (0xEC)
#define D_NUMONYX_MID (0x20)
#define D_SPANSION_MID (0x01)
#define D_MARCRNIX_MID (0xC2)

// NF Device ID
#define D_MT29F2G08AAD (0xDA)
#define D_MT29F16G08CBACA (0x48)
#define D_MT29F64G08CBAAA (0x88)
//#define D_MT29F128G08CFAAA (0x88)		// same as D_MT29F64G08CBAAA
#define D_MT29F256G08CJAAA (0xA8)
#define D_K9F1G08U0A (0xF1) 
#define D_HY27US08561A (0x75) 
#define D_K9F5608U0D (0x75) 
#define D_NAND256W3A28N6 (0x75) 
#define D_S34ML04G1 (0xDC)
#define D_S34MS04G2 (0xAC)

#define D_MX30UF4G16 (0xAC)

// NF meta mode flag
#define D_FLAG_META_MODE_OFF	(0)
#define D_FLAG_META_MODE_ON		(1)

// NF DrivabilityControl
#define D_NF_DC_0	(0)
#define D_NF_DC_1	(1)
#define D_NF_DC_2	(2)
#define D_NF_DC_3	(3)

#define D_NF_RAM_PD_WAIT_TIME			(1)
#define D_NF_PARTITION_NUM				(16)
#define D_NF_PARTITION_TABLE_PARAM_NUM	(3)

// eMMC Support
#define	D_EM_CLK_187_5M			(187500)					// 1500000/8
#define	D_EM_CLK_166_6M			(166600)					// 1500000/9
#define	D_EM_CLK_150_0M			(150000)					// 1500000/10
#define	D_EM_CLK_100_0M			(100000)					// 1500000/15

// eMMC Signal
#define D_EM_PV_DTIMEC			(0)							// MMC_PV_DTIMEC_I (because always insert)
#define D_EM_PV_AMPBL			(0xF)						// MMC_PV_AMPBL_I  (16 beat)
#define D_EM_CR_SLOTTYPE		(1)							// CR_SLOTTYPE_I   (embeded slot for one drive)
#define D_EM_CR_BCLKFREQ		(188)						// CR_BCLKFREQ_I   (188MHz)
#define D_EM_CDDET				(0)							// MMC_CD_I        (card present)

#define D_EM_RAM_PD_WAIT_TIME	(1)

#define D_MW_FSFILL1			(480)
#define D_MW_FSFILL2			(12)

#define D_MW_BOOTFLG0			(0x55)
#define D_MW_BOOTFLG1			(0xAA)

#define D_MW_RESERVED32           12


#define D_MW_GETNotAL2(adr2)      D_MW_GETNOTAL2(adr2)
#define D_MW_GETNotAL4(adr4)      D_MW_GETNOTAL4(adr4)
#define D_MW_PUTNotAL2(adr2, num) D_MW_PUTNOTAL2(adr2, num)
#define D_MW_PUTNotAL4(adr4, num) D_MW_PUTNOTAL4(adr4, num)

/*
 * Data is acquired from two byte Little endian area where the alignment is not done
 * Argument : adr2    The first address in two byte area
 * Return   : Two byte numerical value
 */
#define D_MW_GETNOTAL2(adr2)      ((*(FW_UCHAR*)(adr2))                                    \
                                    | ((FW_USHORT)(*((FW_UCHAR*)(adr2) + 1)) << 8))

/*
 * It writes it in the Little endian area where two byte numerical value is not done in the alignment
 * Argument : adr2    The first address in two byte area
 * Return   : None
 */
#define D_MW_PUTNOTAL2(adr2, num) { *(FW_UCHAR*)(adr2) = (FW_UCHAR)(num);                   \
                                    *((FW_UCHAR*)(adr2) + 1) = (FW_UCHAR)((num) >> 8); }

/*
 * Data is acquired from four byte Little endian area where the alignment is not done
 * Argument : adr4    The first address in four byte area
 * Return   : Four byte numerical value
 */
#define D_MW_GETNOTAL4(adr4)      ((*(FW_UCHAR*)(adr4))                                    \
                                    | ((FW_ULONG)(*((FW_UCHAR*)(adr4) + 1)) << 8)        \
                                    | ((FW_ULONG)(*((FW_UCHAR*)(adr4) + 2)) << 16)       \
                                    | ((FW_ULONG)(*((FW_UCHAR*)(adr4) + 3)) << 24))

/*
 * It writes it in the Little endian area where four byte numerical value is not done in the alignment
 * Argument : adr4    The first address in four byte area
 * Return   : None
 */
#define D_MW_PUTNOTAL4(adr4, num) { *(FW_UCHAR*)(adr4) = (FW_UCHAR)(num);                   \
                                    *((FW_UCHAR*)(adr4) + 1) = (FW_UCHAR)((FW_ULONG)(num) >> 8);    \
                                    *((FW_UCHAR*)(adr4) + 2) = (FW_UCHAR)((FW_ULONG)(num) >> 16);   \
                                    *((FW_UCHAR*)(adr4) + 3) = (FW_UCHAR)((FW_ULONG)(num) >> 24); }


/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
typedef struct {
	unsigned long top;	//no. of sector at which the Drive starts.
	unsigned long size;	//total no. of sectors in the Drive.
} NF_Device_Info;

typedef struct {
	unsigned long top;	//no. of sector at which the Drive starts.
	unsigned long size;	//total no. of sectors in the Drive.
} EM_Device_Info;

typedef struct {
	int formatType;		// kind of Format.
	int clusterSize;	// cluster size should be multiple of 512.
	int entryNum;		// total no. of entry. FAT12,16:more than 512 and multiple of 16
} FS_Nformat_Info;

typedef struct t_system_time {
	short utime;
	unsigned long ltime;
} T_SYSTEM_TIME;

typedef struct {
	UINT32 ce_num;
	UINT32 max_bad_block_num;
	UINT32 device_block_num;
	UINT32 search_other_page;
	UINT32 meta_mode_flg;
	UINT32 dbcnt0_t0dc;
	UINT32 dbcnt0_t1dc;
	UINT32 dbcnt0_t2dc;
	UINT32 ecc_move_bits;
	UINT32 nf_clk_sel;
	UINT32 common_settings;
	UINT32 toggle_timings_0;
	UINT32 toggle_timings_1;
	UINT32 async_toggle_timings;
	UINT32 sync_timings;
	UINT32 timings0;
	UINT32 timings1;
	UINT32 timings2;
	UINT32 dll_phy_ctrl;
	UINT32 phy_ctrl_reg;
	UINT32 phy_tsel_reg;
	UINT32 phy_dq_timing_reg;
	UINT32 phy_dqs_timing_reg;
	UINT32 phy_gate_lpbk_ctrl_reg;
	UINT32 phy_dll_master_ctrl_reg;
	UINT32 phy_dll_slave_ctrl_reg;
	UINT32 timing_mode;
	UINT32 dbcnt;
	UINT32 ecc_config;
	UINT32 skip_bytes_conf;
	UINT32 skip_bytes_offset;
	UINT32 cache_read_enable;
} T_NF_ATTRIBUTE;

typedef struct {
	ULONG Code_Size;
	NF_Device_Info devInfo[NF_MAX_DRV_NUM];
	FS_Nformat_Info formatInfo[NF_MAX_DRV_NUM];
	ULONG Param_Blk_Top;
	ULONG Param_Blk_Size;
	ULONG Exception_Log_Top;
	
} T_NF_Partition_Info;

typedef struct {
	ULONG Code_Size;
	ULONG storage_num;
	ULONG storage_mbr;
	EM_Device_Info devInfo[EM_MAX_DRV_NUM];
	FS_Nformat_Info formatInfo[EM_MAX_DRV_NUM];
	ULONG Param_Blk_Top;
	ULONG Param_Blk_Size;
	ULONG Exception_Log_Top;
} T_EM_Partition_Info;

typedef struct {
	UINT32 load_addr;
	UINT32 jump_addr;
	UINT32 sector_num;
	UINT32 data_size;
	UINT32 num_of_blocks;

// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#ifdef CO_MEMORY_1GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_1GB BEGIN ---
	UINT16 lut[880];
// --- REMOVE_MEMORY_1GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_1GB
#ifdef CO_MEMORY_2GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_2GB BEGIN ---
	UINT16 lut[512];
// --- REMOVE_MEMORY_2GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_2GB
#ifdef CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_3GB BEGIN ---
	UINT16 lut[512];
// --- REMOVE_MEMORY_3GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---

	UINT32 message_digest[8];
} T_NF_PARTITION_AREA;

typedef struct {
	UINT32 area_size;
	UINT32 selector;
	T_NF_PARTITION_AREA area[2];
} T_NF_PARTITION;

typedef struct {
	USHORT block_data;
	USHORT reserved;
} T_NF_PARTITION_TABLE_BLOCK;

typedef struct {
	unsigned long	nf_clk_sel;
	unsigned long	common_settings;
	unsigned long	toggle_timings_0;
	unsigned long	toggle_timings_1;
	unsigned long	async_toggle_timings;
	unsigned long	sync_timings;
	unsigned long	timings0;
	unsigned long	timings1;
	unsigned long	timings2;
	unsigned long	dll_phy_ctrl;
	unsigned long	phy_ctrl_reg;
	unsigned long	phy_tsel_reg;
	unsigned long	phy_dq_timing_reg;
	unsigned long	phy_dqs_timing_reg;
	unsigned long	phy_gate_lpbk_ctrl_reg;
	unsigned long	phy_dll_master_ctrl_reg;
	unsigned long	phy_dll_slave_ctrl_reg;
	unsigned long	timing_mode;
	unsigned long	dbcnt;
	unsigned long	ecc_config;
	unsigned long	skip_bytes_conf;
	unsigned long	skip_bytes_offset;
	unsigned long	rsvd;
	unsigned long	cache_read_enable;
	unsigned long	FACTOR_CHECK;
	unsigned long	factor_check_search_page;
} T_NF_NAND_PARAMETER;

typedef struct {
    FW_UCHAR InfoFlg1[4];
    FW_UCHAR InfoFill1[D_MW_FSFILL1];
    FW_UCHAR InfoFlg2[4];
    FW_UCHAR InfoFree1[4];
    FW_UCHAR InfoFree2[4];
    FW_UCHAR InfoFill2[D_MW_FSFILL2];
    FW_UCHAR InfoFlg3[4];
} T_MW_FSINFO;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
extern volatile UINT32	gSD0_FID_SD;		// SD0 Event flag ID
extern volatile UINT32	gSD0_SID_SD;		// SD0 Semafore ID
extern volatile UINT32	gSD1_FID_SD;		// SD1 Event flag ID
extern volatile UINT32	gSD1_SID_SD;		// SD1 Semafore ID
extern volatile UINT32	gMWNF_FID_NF;		// NF Event flag ID
extern volatile UINT32	gMWNF_SID_NF;		// NF Semafore ID
extern volatile UINT32	gEMMC_FID;			// eMMC Event flag ID
extern volatile UINT32	gEMMC_SID;			// eMMC Semafore ID
extern volatile T_NF_PARTITION gNF_partition_table[D_NF_PARTITION_NUM];
extern volatile UINT32 gPtableBlockTable[D_NF_PARTITION_TABLE_PARAM_NUM];
extern volatile T_NF_NAND_PARAMETER gNANDFlashParameter;
extern volatile UCHAR	gMW_emp_data[D_MW_SECTOR_SIZE * 32];
extern volatile unsigned long formatSectors;
extern volatile unsigned long formatSectors_phy;
extern volatile unsigned long formatted;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
#define D_MW_DEBUG_PRINT_ON
#ifdef D_MW_DEBUG_PRINT_ON
#define Media_Custom_Printf(arg)		(printf arg)
#else
#define Media_Custom_Printf(arg)
#endif
/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

extern VOID Media_Sleep_Task( VOID );

extern INT32  Media_Custom_Set_Flag(INT32 flg_id,INT32 flg_ptn);
extern INT32  Media_Custom_Wait_Flag(INT32 flg_id, UINT32* flg_ptn, UINT32 wai_ptn, UINT32 wai_mode, INT32 time);
extern INT32  Media_Custom_Clear_Flag(UINT32 flg_id, UINT32 flg_ptn);
extern VOID Media_Custom_Sleep_Task(INT32 time);
extern INT32 Media_Custom_Sig_Sem(INT32 semid);
extern INT32 Media_Custom_Twai_Sem(INT32 semid,INT32 tmout);
extern INT32 Media_Custom_Dly_Tsk(UINT32 time);

extern VOID Media_Custom_Get_Time(T_SYSTEM_TIME* time);
extern VOID Media_Custom_Get_Time_us(ULONG* time);
extern VOID Media_Custom_Wait_ns(UINT32 time);
extern VOID Media_Custom_Set_DSBU(VOID);
extern VOID Media_Custom_Set_DSBC(VOID);

extern VOID (*Media_Custom_Err_Printer)(const CHAR*, ...);
extern VOID (*Media_Custom_Wrn_Printer)(const CHAR*, ...);
extern VOID (*Media_Custom_Info_Printer)(const CHAR*, ...);

extern VOID Media_Custom_memcpy(VOID* d_buf, VOID* s_buf, ULONG size);
extern VOID Media_Custom_memset(VOID* s_buf, BYTE data, ULONG size);
extern INT32  Media_Custom_memcmp(BYTE* s_data, BYTE* t_data, ULONG size);
extern INT32  Media_Custom_strncmp(const CHAR* s_buf, CHAR* t_buf, ULONG size);

extern VOID Media_Custom_Dcache_Clean_Flush( ULONG* addr, ULONG num_sec );
extern VOID Media_Custom_Clean_Dcache_All( ULONG* addr, ULONG num_sec );
extern INT32 Media_Custom_Get_Log2Phy_Addr( ULONG addr );


extern INT32 Media_Custom_NF_GIC_Ctrl(VOID);
extern INT32 Media_Custom_NF_GIC_Ctrl_Dis(VOID);

extern INT32 Media_Custom_XDMAC_EXS_CH0_GIC_Ctrl(VOID);

extern VOID Media_Custom_Change_NF_CLK(INT32 clk);
extern VOID Media_Custom_NF_SoftReset(VOID);
extern INT32 Media_Custom_NF_Start_Clock(USHORT timeout_NF, USHORT timeout_XDMAC);
extern INT32 Media_Custom_NF_Direct_Start_Clock(USHORT timeout);
extern VOID Media_Custom_NF_Stop_Clock(USHORT timeout_NF, USHORT timeout_XDMAC);
extern VOID Media_Custom_NF_Direct_Stop_Clock(USHORT timeout_NF, USHORT timeout_XDMAC);
//extern VOID Media_Custom_NF_Set_Attribute(UINT32 MID, UINT32 DID, T_NF_ATTRIBUTE* attr);
extern VOID Media_Custom_NF_Set_Attribute(T_NF_ATTRIBUTE* attr);
extern VOID Media_Custom_NF_Get_Attribute(UINT32 MID, UINT32 DID, T_NF_ATTRIBUTE* attr);
extern VOID Media_Custom_NF_Drive_Ctrl(T_NF_ATTRIBUTE* attr);
extern VOID Media_Custom_NF_Set_Partition_Info(UCHAR device_kind, UINT32 MID, UINT32 DID, T_NF_Partition_Info* info);
extern UINT32 Media_Custom_NF_Start_Abort_Timer(UCHAR exception_flg);
extern INT32 Media_Custom_NF_Check_Abort_Timer(UINT32 timout, UINT32 start_time, UCHAR exception_flg);
extern VOID Media_Custom_NF_Set_RAMPD(UINT32 pd_flg);
extern ULONG Media_Custom_NF_Get_INIT_FAIL();

extern VOID Media_Custom_SD0_Drive_Ctrl(UINT32 dbcnt);
extern VOID Media_Custom_SD1_Drive_Ctrl(UINT32 dbcnt);
extern VOID Media_Custom_SD0_Set_Clock(ULONG clock);
extern ULONG Media_Custom_SD0_Get_Clock(VOID);
extern INT32 Media_Custom_SD0_Start_Clock(VOID);
extern VOID Media_Custom_SD0_Stop_Clock(VOID);
extern INT32 Media_Custom_SD0_Get_SDCDX(VOID);
extern INT32 Media_Custom_SD0_Get_State(VOID);
extern VOID Media_Custom_SD0_Crear_State(VOID);
extern VOID Media_Custom_SD0_Set_RAMPD(UINT32 slp_flg);
extern VOID Media_Custom_SD0_Set_Power(UINT32 onoff);
extern BOOL Media_Custom_SD0_Change_Voltage_CB(UCHAR voltage);
extern VOID Media_Custom_SD1_Set_Clock(ULONG clock);
extern VOID Media_Custom_SD1_Set_Clock_Uhs2(ULONG clock);
extern ULONG Media_Custom_SD1_Get_Clock(VOID);
extern ULONG Media_Custom_SD1_Get_Clock_Uhs2(VOID);
extern INT32 Media_Custom_SD1_Start_Clock(VOID);
extern VOID Media_Custom_SD1_Stop_Clock(VOID);
extern INT32 Media_Custom_SD1_Get_SDCDX(VOID);
extern INT32 Media_Custom_SD1_Get_State(VOID);
extern VOID Media_Custom_SD1_Crear_State(VOID);
extern VOID Media_Custom_SD1_Set_RAMPD(UINT32 slp_flg);
extern VOID Media_Custom_SD1_Set_Power(UINT32 onoff);
extern BOOL Media_Custom_SD1_Change_Voltage_CB(UCHAR voltage);

extern INT32 Media_Custom_EMMC_GIC_Ctrl(VOID);
extern INT32 Media_Custom_EMMC_GIC_Ctrl_Dis(VOID);
extern VOID Media_Custom_EMMC_Drive_Ctrl(UINT32 dbcnt);
extern VOID Media_Custom_EMMC_Signal_Ctrl(VOID);
extern VOID Media_Custom_EMMC_Set_Clock(ULONG clock);
extern ULONG Media_Custom_EMMC_Get_Clock(VOID);
extern INT32 Media_Custom_EMMC_Start_Clock(VOID);
extern INT32 Media_Custom_EMMC_Start_Clock_Direct(VOID);
extern VOID Media_Custom_EMMC_Stop_Clock(VOID);
extern VOID Media_Custom_EMMC_Set_SoftReset(UINT32 val);
extern VOID Media_Custom_EMMC_Set_RAMPD(UINT32 slp_flg);
extern VOID Media_Custom_EMMC_Set_Partition_Info(T_EM_Partition_Info* info);

#endif	// _MW_USER_CUSTOM_H
