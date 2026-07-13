/**
 * @file		boot_loader.c
 * @brief		boot_loader process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ddim_typedef.h"
#include "os_user_custom.h"
#include "fj_boot.h"
#include "mwnf.h"
#include "mwnf_core.h"
#include "mw_user_custom.h"
#include "debug.h"
#include "boot_loader.h"
#include "mba_if.h"
#include "em_if_ext.h"
#include "em_config.h"
#include "em_int_cmn.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_BOOT_LOADER_SEC_SIZE			(512)
#define D_BF_BOOT_LOADER_RTOS_PARTITION		(2)

#ifdef CO_TKERNEL_ONLY
#define PARTITION_TABLE_PARAM_NUM	3
#define PARTITION_TABLE_SELECTOR	0
#define PARTITION_TABLE_AREA_0		1
#define PARTITION_TABLE_AREA_1		2
#define PARTITION_TABLE_NO_ASSIGN	0xFFFF

/* Partiton data area */
#define PAR_RESOURCE			8
#define PAR_USERSETTING			9

/* user setting data */
#define USER_SET_DATA_NUM			10
#define USER_SET_INFO_SIZE			2048
#define USER_SET_SIZE_MAX			(2048*63)
#define LAST_COUNT_NO				0xFEEEEEEE
#define ROM_AREA_SIZE				0x00180000

typedef struct {
	unsigned int		count;
	unsigned int		size;
} USER_SET_DATA_INFO;

USER_SET_DATA_INFO gUser_set_data_info[USER_SET_DATA_NUM] __attribute__((aligned(8)));
#endif // CO_TKERNEL_ONLY

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
#ifdef CO_TKERNEL_ONLY
// boot parameter
typedef struct {
	unsigned long	header[16];
	unsigned long	ptable_sel[3];
	unsigned long	sdram_par[139];
	unsigned long 	nand_par[26];
} boot_parameter;
#endif // CO_TKERNEL_ONLY

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
#ifdef CO_FAST_BOOT_ON
UINT32 gBF_Boot_Loader_Area = 0; 			/* for area check that loading is completed */
#else
UINT32 gBF_Boot_Loader_Area = 0;		 	/* for area check that loading is completed */
#endif

#ifdef CO_FAST_BOOT_ON
static UCHAR gBF_Boot_Loader_AreaNo;		/* load area No */
static CHAR* gBF_Boot_Loader_DstStartAdr;	/* start distination on SDRAM address */
static ULONG gBF_Boot_Loader_LoadSize;		/* load code size (Byte) */
#endif

#ifdef CO_TKERNEL_ONLY
static UCHAR Read_data[2048];
#endif

extern volatile T_NF_PARTITION gNF_partition_table[D_NF_PARTITION_NUM];

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
#ifdef CO_TKERNEL_ONLY
static int latest_user_set_data_search(unsigned int target_count_val, unsigned int *block_no, unsigned int *count_val){
	int i;
	int last_count_flag;
	int	range;
	unsigned int max_count_val;
	unsigned int max_count_val_blcok_no;
	
	last_count_flag = 0;
	if(target_count_val == 0) {	// latest search
		range = 0;
		for(i=0; i < USER_SET_DATA_NUM; i++) {
			if(LAST_COUNT_NO == gUser_set_data_info[i].count) {
				last_count_flag = 1;
				break;
			}
			if((LAST_COUNT_NO < gUser_set_data_info[i].count) || (0 == gUser_set_data_info[i].count)) {
				range++;
			}
		}
		if(range == USER_SET_DATA_NUM) {
			return -2;
		}
		
		if(last_count_flag == 0) {  // There is no final count value
			max_count_val = 0;
			max_count_val_blcok_no = 0;
			for(i=0; i < USER_SET_DATA_NUM; i++) {
				if(LAST_COUNT_NO > gUser_set_data_info[i].count) {
					if(max_count_val <= gUser_set_data_info[i].count) {
						max_count_val = gUser_set_data_info[i].count;
						max_count_val_blcok_no = i;
					}
				}
			}
		}
		else {						// There is a final count value
			range = 0;
			max_count_val = 0;
			max_count_val_blcok_no = 0;
			for(i=0; i < USER_SET_DATA_NUM; i++) {
				if(USER_SET_DATA_NUM > gUser_set_data_info[i].count) {
					if(max_count_val <= gUser_set_data_info[i].count) {
						max_count_val = gUser_set_data_info[i].count;
						max_count_val_blcok_no = i;
					}
					range = 1;
				}
			}
			if(range == 0) {
				max_count_val = 0;
				max_count_val_blcok_no = 0;
				for(i=0; i < USER_SET_DATA_NUM; i++) {
					if(LAST_COUNT_NO == gUser_set_data_info[i].count) {
						max_count_val = gUser_set_data_info[i].count;
						max_count_val_blcok_no = i;
						break;
					}
				}
			}
		}
	}
	else {	// target_count_val
		max_count_val = 0;
		max_count_val_blcok_no = 0;
		for(i=0; i < USER_SET_DATA_NUM; i++) {
			if(target_count_val == gUser_set_data_info[i].count) {
				max_count_val = gUser_set_data_info[i].count;
				max_count_val_blcok_no = i;
				break;
			}
		}
		if(i == USER_SET_DATA_NUM) {
			return -1;
		}
	}
	
	*block_no = max_count_val_blcok_no;
	*count_val = max_count_val;
	return 0;
}

static int user_setting_data_loard(unsigned char device)
{
	unsigned int block_no;
	unsigned int count;
	unsigned int size;
	unsigned int max_count_block_no = 0;
	unsigned int max_count_val = 0;
	int i;
	int ret;
	unsigned int load_size;
	unsigned int load_pages;
	unsigned char *user_set_data_adr;
	unsigned long sector_no;
	unsigned long sector_num;
	unsigned long sector_offset;
	unsigned long sector_per_blk = 0;
	unsigned long user_set_area_phy_addr;
	unsigned short area_sel;
	T_NF_DEV_INFO	devinfo = {0};

	area_sel = gNF_partition_table[PAR_USERSETTING].selector;

	user_set_area_phy_addr = ROM_AREA_SIZE;
	for(i=0; i < 8; i++) {
		user_set_area_phy_addr += gNF_partition_table[i].area_size;
	}
	

	// sdram area clear.
	load_size = 0;
	user_set_data_adr = (unsigned char *)gNF_partition_table[PAR_USERSETTING].area[area_sel].load_addr;
	for(i=0; i < 16; i++) {
		*(user_set_data_adr+i) = 0;
	}

	// get user setting data.
	if(device == 0) {
		MWNF_Get_Dev_Info(&devinfo);
		sector_per_blk = (devinfo.page_per_blk * devinfo.sec_per_page);

		for(i=0; i< USER_SET_DATA_NUM; i++){

			block_no = gNF_partition_table[PAR_USERSETTING].area[area_sel].lut[i];

			if(block_no == 0xFFFF) {
					gUser_set_data_info[i].count = 0;
					gUser_set_data_info[i].size = 0;
			}
			else {
				sector_no  = (block_no * sector_per_blk);
				ret = MWNF_Direct_BRead(sector_no, 1, (char*)&Read_data[0]);
				if(ret == 0) {
					count = ((Read_data[0] << 24) | (Read_data[1] << 16) | (Read_data[2] << 8) | Read_data[3]);
					size = ((Read_data[4] << 24) | (Read_data[5] << 16) | (Read_data[6] << 8) | Read_data[7]);
					gUser_set_data_info[i].count = count;
					gUser_set_data_info[i].size = size;
				}
			}
		}
	}
	else {
		// eMMC
		sector_no  = gNF_partition_table[PAR_USERSETTING].area[area_sel].sector_num;
		sector_num = (USER_SET_INFO_SIZE / EM_CONFIG_DEFAULT_BLOCK_SIZE);
		sector_offset = ((USER_SET_INFO_SIZE + USER_SET_SIZE_MAX) / EM_CONFIG_DEFAULT_BLOCK_SIZE);

		for(i=0; i< USER_SET_DATA_NUM; i++){
			ret = MWEM_DirectRead(sector_no, sector_num, (char*)&Read_data[0]);
			if(ret == 0) {
				count = ((Read_data[0] << 24) | (Read_data[1] << 16) | (Read_data[2] << 8) | Read_data[3]);
				size = ((Read_data[4] << 24) | (Read_data[5] << 16) | (Read_data[6] << 8) | Read_data[7]);
				gUser_set_data_info[i].count = count;
				gUser_set_data_info[i].size = size;
			}
			sector_no += sector_offset;
		}
	}

	// latest user setting data search.
	latest_user_set_data_search(0, &max_count_block_no, &max_count_val);

	// latest user setting data read.
	for(i=0; i < USER_SET_DATA_NUM; i++) {
	
		load_size = gUser_set_data_info[max_count_block_no].size;

		if(load_size == 0) {	
			if(max_count_val == 1)
				max_count_val = LAST_COUNT_NO;
			else
				max_count_val--;
			latest_user_set_data_search(max_count_val, &max_count_block_no, &max_count_val);
			continue;
		}
		if(load_size > USER_SET_SIZE_MAX) {		// size max value check.
			load_size = USER_SET_SIZE_MAX;			// limit the maximum value.
		}

		if(device == 0) {
			if(load_size  < devinfo.page_size){
				load_pages = 1;
			}
			else {
				load_pages = load_size / devinfo.page_size;
		
				if ((load_size % devinfo.page_size) != 0)
					load_pages++;
			}
			

			block_no = gNF_partition_table[PAR_USERSETTING].area[area_sel].lut[max_count_block_no];
			sector_no  = (block_no * sector_per_blk) + devinfo.sec_per_page;
			ret = MWNF_Direct_BRead(sector_no, (load_pages * devinfo.sec_per_page), (char*)(user_set_data_adr+0x10));
		}
		else {
			// eMMC
			if(load_size  < EM_CONFIG_DEFAULT_BLOCK_SIZE){
				sector_num = 1;
			}
			else {
				sector_num = load_size / EM_CONFIG_DEFAULT_BLOCK_SIZE;
				if ((load_size % EM_CONFIG_DEFAULT_BLOCK_SIZE) != 0)
					sector_num++;
			}
			sector_no  = gNF_partition_table[PAR_USERSETTING].area[area_sel].sector_num;
			sector_no += ((max_count_block_no * (USER_SET_INFO_SIZE + USER_SET_SIZE_MAX)) / EM_CONFIG_DEFAULT_BLOCK_SIZE);
			sector_no += (USER_SET_INFO_SIZE / EM_CONFIG_DEFAULT_BLOCK_SIZE);
			ret = MWEM_DirectRead(sector_no, sector_num, (char*)(user_set_data_adr+0x10));
		}
		if(ret != 0) {
			if(max_count_val == LAST_COUNT_NO)
				max_count_val = 1;
			else
				max_count_val--;

			latest_user_set_data_search(max_count_val, &max_count_block_no, &max_count_val);
		}
		else{
			*(user_set_data_adr+0) = (unsigned char)(load_size & 0x000000ff);
			*(user_set_data_adr+1) = (unsigned char)((load_size & 0x0000ff00) >> 8);
			*(user_set_data_adr+2) = (unsigned char)((load_size & 0x00ff0000) >> 16);
			*(user_set_data_adr+3) = (unsigned char)((load_size & 0xff000000) >> 24);
			break;
		}
	}
	return 0;
}
#endif // CO_TKERNEL_ONLY

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
#ifdef CO_FAST_BOOT_ON
extern unsigned int __zero_sec_start;
extern unsigned int __zero_sec_end;
extern unsigned int __first_sec_start;
extern unsigned int __first_sec_end;
void BF_Boot_Loader_Load_First(char* dstStartAdr, ULONG loadSize)
{
	int ercd;
	unsigned long  sector_no;
	ULONG          frontloadSize;
	unsigned short lut_index;
	unsigned short read_sec_num;
	unsigned short first_sec_no;
	unsigned short first_read_num;
	unsigned short start_sector;
	char*          dst_addr = dstStartAdr;
	FW_UINT32      w_addr;
	T_NF_DEV_INFO  devinfo;
	USHORT         boot_loader_sec_per_blk;
	USHORT         area_sel;

	ercd = MWNF_Init();
	if (ercd != 0) {
		return;
	}
	w_addr = (FW_UINT32)gMW_Exs_IRAM_NF;
	ercd = MWNF_BasicInit(w_addr, 0);
	if (ercd != 0) {
		return;
	}

	MWNF_Get_Dev_Info(&devinfo);
	if (devinfo.device_kind == D_MWNF_LARGE_DEVICE)
	{
		boot_loader_sec_per_blk = 256;
	}

#ifdef CO_META_ON
	MWNF_Set_DMA_Mode(0);
#endif
	MWNF_Set_Ecc_Param((gNANDFlashParameter.ecc_config & 0x1), ((gNANDFlashParameter.ecc_config & 0x00000700) >> 8));

	// RTOS Partition = 2
	if ( gNF_partition_table[D_BF_BOOT_LOADER_RTOS_PARTITION].area_size != 0 ) {
		area_sel = gNF_partition_table[D_BF_BOOT_LOADER_RTOS_PARTITION].selector;

		frontloadSize = (UINT32)&__zero_sec_end - (UINT32)&__zero_sec_start;
		frontloadSize = (~511)&(frontloadSize+511);
		start_sector  = (frontloadSize / D_BF_BOOT_LOADER_SEC_SIZE);
		lut_index     = (start_sector / boot_loader_sec_per_blk);
		first_sec_no  = (start_sector % boot_loader_sec_per_blk);
		read_sec_num  = loadSize / D_BF_BOOT_LOADER_SEC_SIZE;

		// Code Loading
		if (first_sec_no != 0) {
			first_read_num = ((boot_loader_sec_per_blk - first_sec_no) < read_sec_num) ? (boot_loader_sec_per_blk - first_sec_no) : read_sec_num;
			sector_no = gNF_partition_table[D_BF_BOOT_LOADER_RTOS_PARTITION].area[area_sel].lut[lut_index] * boot_loader_sec_per_blk + first_sec_no;	/* pgr0039 */
			ercd = MWNF_Direct_BRead(sector_no, first_read_num, dst_addr);
			if (ercd != D_MWNF_OK) {
			}
			read_sec_num -= first_read_num;
			dst_addr += first_read_num * D_BF_BOOT_LOADER_SEC_SIZE;
			lut_index++;
		}

		while (1) {
			if (read_sec_num < boot_loader_sec_per_blk) {
				break;
			}

			sector_no = gNF_partition_table[D_BF_BOOT_LOADER_RTOS_PARTITION].area[area_sel].lut[lut_index] * boot_loader_sec_per_blk;	/* pgr0039 */
			ercd = MWNF_Direct_BRead(sector_no, boot_loader_sec_per_blk, dst_addr);
			if (ercd != D_MWNF_OK) {
			}
			dst_addr += (boot_loader_sec_per_blk * D_BF_BOOT_LOADER_SEC_SIZE);
			lut_index++;
			read_sec_num -= boot_loader_sec_per_blk;
		}

		if (read_sec_num != 0) {
			sector_no = gNF_partition_table[D_BF_BOOT_LOADER_RTOS_PARTITION].area[area_sel].lut[lut_index] * boot_loader_sec_per_blk;	/* pgr0039 */
			ercd = MWNF_Direct_BRead(sector_no, read_sec_num, dst_addr);
			if (ercd != D_MWNF_OK) {
			}
		}

	}
#ifdef CO_META_ON
	MWNF_Set_DMA_Mode(1);
#endif
	MWNF_Set_Ecc_Param(D_MWNF_ENABLE, D_MWNF_ECC_AUTO);
}

void BF_Boot_Loader_Load_First_emmc(char* dstStartAdr, ULONG loadSize)
{
	int		ercd;
	UINT32	area_sel;
	UINT32	front_sector;
	UINT32	frontloadSize;
	UINT32	start_sector;
	UINT32	read_sec_num;
	char*	dst_addr = dstStartAdr;

	BF_Fs_User_Custom_Set_EMMC_Port();
	// process start
	ercd = MWEM_Init();
	if (ercd != 0) {
		return;
	}
	ercd = MWEM_DeviceInit();
	if (ercd != 0) {
		return;
	}

	// RTOS Partition = 2
	if ( gNF_partition_table[D_BF_BOOT_LOADER_RTOS_PARTITION].area_size != 0 ) {
		area_sel = gNF_partition_table[D_BF_BOOT_LOADER_RTOS_PARTITION].selector;
		front_sector = gNF_partition_table[D_BF_BOOT_LOADER_RTOS_PARTITION].area[area_sel].sector_num;
		frontloadSize = (UINT32)&__zero_sec_end - (UINT32)&__zero_sec_start;
		frontloadSize = (~511)&(frontloadSize+511);
		start_sector = front_sector + (frontloadSize / D_BF_BOOT_LOADER_SEC_SIZE);
		read_sec_num = loadSize / D_BF_BOOT_LOADER_SEC_SIZE;

		ercd = MWEM_DirectRead(start_sector, read_sec_num, dst_addr);
		if (ercd != EM_OK) {
		}
	}
}
#endif

#ifdef CO_TKERNEL_ONLY
void BF_Boot_Loader_Load_PartitionData(void)
{
	INT32			ercd;
	T_NF_DEV_INFO	devinfo;
	boot_parameter*	boot_param;
	ULONG			sector_no;
	LONG			sector_num;
	LONG			sector_per_blk;
	UINT32			w_addr;
	UINT32			i, area;
	USHORT			table_sel[2];
	UINT32			read_buf;
	UINT32			area_sel;
	UINT32			block_size;
	UINT32			read_num;

	// initial value
	memset((VOID*)&gNANDFlashParameter, 0, sizeof(gNANDFlashParameter));

	// process start
	ercd = MWNF_Init();
	if (ercd != 0) {
		return;
	}
	w_addr = (FW_UINT32)gMW_Exs_IRAM_NF;
	ercd = MWNF_BasicInit(w_addr, 0);
	if (ercd != 0) {
		return;
	}

	MWNF_Get_Dev_Info(&devinfo);
	sector_per_blk = (devinfo.page_per_blk * devinfo.sec_per_page);

#ifdef CO_META_ON
	MWNF_Set_DMA_Mode(0);
#endif
	// boot header
	ercd = MWNF_Direct_BRead(0, 2, (char*)Read_data);
	if (ercd != D_MWNF_OK) {
		return;
	}
	boot_param = (boot_parameter*)Read_data;

	// nand parameter
	memcpy((VOID*)&gNANDFlashParameter, (const VOID*)boot_param->nand_par, sizeof(gNANDFlashParameter));

	MWNF_Set_Ecc_Param((gNANDFlashParameter.ecc_config & 0x1), ((gNANDFlashParameter.ecc_config & 0x00000700) >> 8));

	// partition table
	for( i = 0; i < PARTITION_TABLE_PARAM_NUM; i++ ){
		gPtableBlockTable[i] = boot_param->ptable_sel[i];
	}

	if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 0) {
		table_sel[0] = PARTITION_TABLE_AREA_0;
		table_sel[1] = PARTITION_TABLE_AREA_1;
	}
	else if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 1) {
		table_sel[0] = PARTITION_TABLE_AREA_1;
		table_sel[1] = PARTITION_TABLE_AREA_0;
	}
	else {
#ifdef CO_META_ON
		MWNF_Set_DMA_Mode(1);
#endif
		MWNF_Set_Ecc_Param(D_MWNF_ENABLE, D_MWNF_ECC_AUTO);
		return;
	}

	block_size = (devinfo.page_per_blk * devinfo.page_size);

	for (area=0; area<2; area++) {
		if (gPtableBlockTable[(table_sel[area])] == PARTITION_TABLE_NO_ASSIGN) {
			continue;
		}
		// partition table
		sector_no  = (gPtableBlockTable[(table_sel[area])] * sector_per_blk);
		sector_num = (sizeof(gNF_partition_table) / (devinfo.page_size / devinfo.sec_per_page) + 1);
		ercd = MWNF_Direct_BRead(sector_no, sector_num, (char*)gNF_partition_table);
		if (ercd != D_MWNF_OK) {
			continue;
		}

		// RESOURCE
		if (gNF_partition_table[PAR_RESOURCE].area_size != 0) {
			area_sel = gNF_partition_table[PAR_RESOURCE].selector;
			read_buf = gNF_partition_table[PAR_RESOURCE].area[area_sel].load_addr;

			if (gNF_partition_table[PAR_RESOURCE].area[area_sel].num_of_blocks == 0) {
				if( gNF_partition_table[PAR_RESOURCE].area[area_sel].data_size % block_size ){
					read_num = (gNF_partition_table[PAR_RESOURCE].area[area_sel].data_size / block_size + 1);
				}else{
					read_num = (gNF_partition_table[PAR_RESOURCE].area[area_sel].data_size / block_size);
				}
			}
			else {
				read_num = gNF_partition_table[PAR_RESOURCE].area[area_sel].num_of_blocks;
			}
			for (i=0; i<read_num; i++) {
				sector_no = (gNF_partition_table[PAR_RESOURCE].area[area_sel].lut[i] * sector_per_blk);
				ercd = MWNF_Direct_BRead(sector_no, sector_per_blk, (char*)read_buf);
				if (ercd != D_MWNF_OK) {
					break;
				}
				read_buf += block_size;
			}
			if (ercd != D_MWNF_OK) {
				continue;
			}
		}

		// USER SETTING DATA
		if (gNF_partition_table[PAR_USERSETTING].area_size != 0) {
			ercd = user_setting_data_loard(0);
			if (ercd != D_MWNF_OK) {
				continue;
			}
		}
		break;
	}

#ifdef CO_META_ON
	MWNF_Set_DMA_Mode(1);
#endif

	MWNF_Set_Ecc_Param(D_MWNF_ENABLE, D_MWNF_ECC_AUTO);
}

void BF_Boot_Loader_Load_PartitionData_emmc(void)
{
	INT32			ercd;
	boot_parameter*	boot_param;
	ULONG			sector_no;
	LONG			sector_num;
	UINT32			i, area;
	USHORT			table_sel[2];
	UINT32			read_buf;
	UINT32			area_sel;

	// initial value
	memset((VOID*)&gNANDFlashParameter, 0, sizeof(gNANDFlashParameter));

	BF_Fs_User_Custom_Set_EMMC_Port();
	// process start
	ercd = MWEM_Init();
	if (ercd != 0) {
		return;
	}
	ercd = MWEM_DeviceInit();
	if (ercd != 0) {
		return;
	}

	// Access partition area : boot partition1 area
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_1);
	if (ercd != 0) {
		return;
	}

	// boot header
	ercd = MWEM_DirectRead(0, 2, (char*)Read_data);
	if (ercd != 0) {
		(void)MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_NONE);
		return;
	}
	boot_param = (boot_parameter*)Read_data;

	// Access partition area : user data area
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_NONE);
	if( ercd != 0 ) {
		return;
	}

	// nand parameter
	memcpy((VOID*)&gNANDFlashParameter, (const VOID*)boot_param->nand_par, sizeof(gNANDFlashParameter));

	// partition table
	for( i = 0; i < PARTITION_TABLE_PARAM_NUM; i++ ){
		gPtableBlockTable[i] = boot_param->ptable_sel[i];
	}

	if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 0) {
		table_sel[0] = PARTITION_TABLE_AREA_0;
		table_sel[1] = PARTITION_TABLE_AREA_1;
	}
	else if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 1) {
		table_sel[0] = PARTITION_TABLE_AREA_1;
		table_sel[1] = PARTITION_TABLE_AREA_0;
	}
	else {
		return;
	}

	for (area=0; area<2; area++) {
		if (gPtableBlockTable[(table_sel[area])] == PARTITION_TABLE_NO_ASSIGN) {
			continue;
		}
		// partition table
		sector_no  = gPtableBlockTable[(table_sel[area])];
		sector_num = (sizeof(gNF_partition_table) / EM_CONFIG_DEFAULT_BLOCK_SIZE + 1);
		ercd = MWEM_DirectRead(sector_no, sector_num, (char*)gNF_partition_table);
		if (ercd != 0) {
			continue;
		}

		// RESOURCE
		if (gNF_partition_table[PAR_RESOURCE].area_size != 0) {
			area_sel   = gNF_partition_table[PAR_RESOURCE].selector;
			sector_no  = gNF_partition_table[PAR_RESOURCE].area[area_sel].sector_num;
			read_buf   = gNF_partition_table[PAR_RESOURCE].area[area_sel].load_addr;

			if (gNF_partition_table[PAR_RESOURCE].area[area_sel].num_of_blocks == 0) {
				if( gNF_partition_table[PAR_RESOURCE].area[area_sel].data_size % EM_CONFIG_DEFAULT_BLOCK_SIZE ){
					sector_num = ( gNF_partition_table[PAR_RESOURCE].area[area_sel].data_size / EM_CONFIG_DEFAULT_BLOCK_SIZE + 1 );
				}
				else {
					sector_num = ( gNF_partition_table[PAR_RESOURCE].area[area_sel].data_size / EM_CONFIG_DEFAULT_BLOCK_SIZE );
				}
			}
			else {
				sector_num = gNF_partition_table[PAR_RESOURCE].area[area_sel].num_of_blocks;
			}
			ercd = MWEM_DirectRead(sector_no, sector_num, (char*)read_buf);
			if (ercd != 0) {
				continue;
			}
		}

		// USER SETTING DATA
		if (gNF_partition_table[PAR_USERSETTING].area_size != 0) {
			ercd = user_setting_data_loard(1);
			if (ercd != 0) {
				continue;
			}
		}
		break;
	}
}
#endif // CO_TKERNEL_ONLY

/**
 * @brief 2nd Boot Code Loader Task
 * @param void
 * @return void
 * @note
 * @attention
*/
VOID boot_loader(INT32 stacd)
{
#ifdef CO_FAST_BOOT_ON
	INT32  ercd;
	USHORT lut[512];
	ULONG  sector_no;
	ULONG  frontloadSize;
	USHORT lut_index;
	USHORT read_sec_num;
	USHORT first_sec_no;
	USHORT first_read_num;
	USHORT start_sector;
	CHAR*  dst_addr = gBF_Boot_Loader_DstStartAdr;
	T_NF_DEV_INFO devinfo;
	USHORT boot_loader_sec_per_blk;

	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_LOAD, FJ_MBALOG_TYPE_START, "Boot Load" ));

	ercd = MWNF_Get_Dev_Info(&devinfo);
	if (ercd != D_MWNF_OK) {
		BF_Debug_Print_Error(("MWNF_Get_Dev_Info Error ercd(%d)\n", ercd));
		OS_User_Ext_Tsk();
	}
	if (devinfo.device_kind == D_MWNF_LARGE_DEVICE) {
		boot_loader_sec_per_blk = 256;
	}

//  RTOS version only
#ifdef CO_META_ON
	MWNF_Set_DMA_Mode(0);
#endif
	MWNF_Set_Ecc_Param(0x00000001, D_MWNF_ECC_AUTO);
//  RTOS version only
	// Read LUT
	ercd = MWNF_Direct_BRead(4, 1, (CHAR*)lut);
	if (ercd != D_MWNF_OK) {
		BF_Debug_Print_Error(("LUT Read Error ercd(%d)\n", ercd));
		OS_User_Ext_Tsk();
	}

//  RTOS version only
	frontloadSize = (UINT32)&__zero_sec_end - (UINT32)&__zero_sec_start;
	frontloadSize = (~511)&(frontloadSize+511);
	frontloadSize += (UINT32)&__first_sec_end - (UINT32)&__first_sec_start;
	frontloadSize = (~511)&(frontloadSize+511);
	start_sector  = (frontloadSize / D_BF_BOOT_LOADER_SEC_SIZE);
//  RTOS version only
	lut_index     = (start_sector / boot_loader_sec_per_blk);
	first_sec_no  = (start_sector % boot_loader_sec_per_blk);
	read_sec_num  = gBF_Boot_Loader_LoadSize / D_BF_BOOT_LOADER_SEC_SIZE;

	// Code Loading
	if (first_sec_no != 0) {
		first_read_num = ((boot_loader_sec_per_blk - first_sec_no) < read_sec_num) ? (boot_loader_sec_per_blk - first_sec_no) : read_sec_num;
		sector_no = lut[lut_index] * boot_loader_sec_per_blk + first_sec_no;
		ercd = MWNF_Direct_BRead(sector_no, first_read_num, dst_addr);
		if (ercd != D_MWNF_OK) {
			BF_Debug_Print_Error(("Code Read Error ercd(%d)\n", ercd));
			BF_Debug_Print_Error(("srctor 0x%08x logblk %d phyblk %d", sector_no, lut_index, lut[lut_index]));
		}
		read_sec_num -= first_read_num;
		dst_addr += first_read_num * D_BF_BOOT_LOADER_SEC_SIZE;
		lut_index++;
	}

	while (1) {
		if (read_sec_num < boot_loader_sec_per_blk) {
			break;
		}
		sector_no = lut[lut_index] * boot_loader_sec_per_blk;
		ercd = MWNF_Direct_BRead(sector_no, boot_loader_sec_per_blk, dst_addr);
		if (ercd != D_MWNF_OK) {
			BF_Debug_Print_Error(("Code Read Error ercd(%d)\n", ercd));
			BF_Debug_Print_Error(("srctor 0x%08x logblk %d phyblk %d", sector_no, lut_index, lut[lut_index]));
//			OS_User_Ext_Tsk();
		}
		dst_addr += (boot_loader_sec_per_blk * D_BF_BOOT_LOADER_SEC_SIZE);
		lut_index++;
		read_sec_num -= boot_loader_sec_per_blk;
	}

	if (read_sec_num != 0) {
		sector_no = lut[lut_index] * boot_loader_sec_per_blk;
		ercd = MWNF_Direct_BRead(sector_no, read_sec_num, dst_addr);
		if (ercd != D_MWNF_OK) {
			BF_Debug_Print_Error(("Code Read Error ercd(%d)\n", ercd));
			BF_Debug_Print_Error(("srctor 0x%08x logblk %d phyblk %d", sector_no, lut_index, lut[lut_index]));
		}
	}
//  RTOS version only
#ifdef CO_META_ON
	MWNF_Set_DMA_Mode(1);
#endif
	//  RTOS version only

	if (gBF_Boot_Loader_AreaNo != 0) {
		gBF_Boot_Loader_Area |= (0x80000000 >> (gBF_Boot_Loader_AreaNo - 1));
	}
	else {
		gBF_Boot_Loader_Area = 0x80000000;
	}

	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_LOAD, FJ_MBALOG_TYPE_END, "Boot Load" ));

	OS_User_Ext_Tsk();
#endif
}

/**
 * @brief Start Boot Code Loader
 * @param areaNo;			load area No
 								0 : All
 								1 : second region
 		  dstStartAdr;		start distination on SDRAM address
		  loadSize;			load size (byte)
 * @return void
 * @note
 * @attention
*/
FJ_ERR_CODE fj_bootload(UCHAR areaNo, CHAR* dstStartAdr, ULONG loadSize)
{
#ifdef CO_FAST_BOOT_ON
	T_NF_DEV_INFO	devinfo;

	// Parameter Check
	if (((ULONG)dstStartAdr % D_BF_BOOT_LOADER_SEC_SIZE) != 0) {
		return FJ_ERR_NG;
	}
	if ((loadSize % D_BF_BOOT_LOADER_SEC_SIZE) != 0) {
		return FJ_ERR_NG;
	}

	MWNF_Get_Dev_Info(&devinfo);
	if (devinfo.device_kind == D_MWNF_LARGE_DEVICE)
	{
		if (((ULONG)dstStartAdr % devinfo.page_size)!= 0)
		{
			// Warnning
		}
		if ((loadSize % devinfo.page_size) != 0)
		{
			// Warnning
		}
	}

	gBF_Boot_Loader_AreaNo        = areaNo;
	gBF_Boot_Loader_DstStartAdr   = dstStartAdr;
	gBF_Boot_Loader_LoadSize      = loadSize;

	OS_User_Sta_Tsk(TID_BOOTLOADER, 0);
#endif

	return FJ_ERR_OK;
}


/**
 * @brief Status of boot loader Task
 * @param VOID
 * @return 0 : DMT
 * @note
 * @attention
*/
INT32 fj_bootloadstate(VOID)
{
	OS_USER_ER ercd;
	T_OS_USER_RTSK k_rtsk;

	ercd = OS_User_Ref_Tsk( TID_BOOTLOADER, &k_rtsk );

	if(0 != ercd) {
		return -1;
	}
	if(D_OS_USER_TTS_DMT == k_rtsk.tskstat) {
		return 0;
	}
	return -1;
}

/**
 * @brief This function checks a state of loading of code/data.
 * @param chk_area	load check area.
			0x80000000 : Loading completion of second region.
			The bit stands in the area where loading was completed.
			Ex.) When a chk_area is 0x80000000,second region are loaded.
*/
UCHAR BF_Boot_Loader_Check(UINT32 chk_area)
{
	if ((gBF_Boot_Loader_Area & chk_area) == chk_area) {
		return 1;
	}
	else {
		return 0;
	}
}

