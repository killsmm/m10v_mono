/**
 * @file		updater.c
 * @brief		updater control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

/*----------------------------------------------------------------------*/
/* Include Files                                                        */
/*----------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>

#include "sdram_map_common.h"
#include "sdram_map_define.h"		//SDRAM_ADR_BOOT_LOADER
#include "fj_mode.h"
#include "fj_updater.h"
#include "updater.h"
#include "mode.h"
#include "fs_if.h"
#include "debug.h"
#include "NF_driver_wrapper.h"
#include "mwnf_core.h"
#include "nf.h"				// for "IO_NF_REG"
#include "_jmlexs.h"		// for "IO_EXSTOP"
#include "dd_top.h"
#include "mw_user_custom.h"
#include "fj_host.h"
#include "fj_arm.h"
#include "em_config.h"
#include "em_if_ext.h"
#include "em_int_cmn.h"
#include "l1l2cache.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/

#define SECTOR_SIZE					(512)

#define PARA_START_BLK_MAIN			0
#define PARA_START_BLK_SPARE		1
#define CODE_START_BLK_MAIN			2
#define PARTITION_TABLE_START_BLK0	4
#define PARTITION_TABLE_START_BLK1	7
#define PARTITION_TABLE_BLK_NUM		3
//#define CODE_BLK_NUM				512				// max case => 512 * 1block size(128KB) = 64MB

#define PARA_START_SEC_MAIN				0
#define PARTITION_TABLE_START_SECTOR0	0
#define PARTITION_TABLE_START_SECTOR1	0x300
#define PARTITION_TABLE_SECTOR_NUM		0x300

// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#ifdef CO_MEMORY_1GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_1GB BEGIN ---
#define CODE_BLK_NUM				880				// max case => 880 * 1block size(128KB) = 110MB
// --- REMOVE_MEMORY_1GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_1GB
#ifdef CO_MEMORY_2GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_2GB BEGIN ---
#define CODE_BLK_NUM				512				// max case => 512 * 1block size(128KB) = 64MB
// --- REMOVE_MEMORY_2GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_2GB
#ifdef CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_3GB BEGIN ---
#define CODE_BLK_NUM				512				// max case => 512 * 1block size(128KB) = 64MB
// --- REMOVE_MEMORY_3GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---

#define PARTITION_TABLE_PARAM_NUM	3
#define PARTITION_TABLE_SELECTOR	0
#define PARTITION_TABLE_AREA_0		1
#define PARTITION_TABLE_AREA_1		2
#define PARTITION_TABLE_SELECTOR0	0
#define PARTITION_TABLE_SELECTOR1	1

#define PARTITION_NUM_OF_HIBERNATION	7

// Error Code
// ---------------------------------
#define WRITE_SUCCESS			0x00
#define CARDOPEN_ERR			0x01
#define SDRAMPAR_ERR			0x02
#define FAT_ERR					0x03
#define ROOTDIR_ERR				0x04
#define DATABIN_ERR				0x05
#define LOADPAR_ERR				0x06
#define GETPAR_ERR				0x07

#define NAND_DATA_VERIFY_ERR	0x21
#define NAND_PARA_VERIFY_ERR	0x22
#define NAND_CODESIZE_OVER_ERR	0x23
#define NAND_DATA_WRITE_ERR		0x24
#define EMMC_DATA_WRITE_ERR		0x25

#define FAT_ERR_ONEFILE			0x31
#define ROOTDIR_ERR_ONEFILE		0x32
#define READDATA_ERR_ONEFILE	0x33
// ---------------------------------

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothiong Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
// Nothiong Special

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/

static volatile T_NF_DEV_INFO			NandInfo;
//static volatile NAND_FLASH_PARAMETER	NANDFlashParameter;		//nand.par
static volatile   T_NF_NAND_PARAMETER 	NANDFlashParameter;		//nand.par

static volatile SDRAM_PARAMETER 		SDRAM_Parameter;
static volatile boot_parameter			boot_param;
static volatile T_BOOT_PARAMETER		BootParameter; 		// boot.par
static volatile boot_header				BootHeader     __attribute__((aligned(4))); 
// --- REMOVE_SECURE BEGIN ---
static volatile unsigned long			DeviceKey[64]  __attribute__((aligned(4)));
static volatile SECURE_PARAMETER		SecureParameter={0};		// secure.par
// --- REMOVE_SECURE END ---
static volatile PARTITION_PARAMETER		PartitionParameter;

static volatile unsigned int gNumOfCodeAreaBlock;				//	max code area block
static volatile unsigned int gLimitOfCodeAreaBlock;				//	max code area block
static volatile unsigned long gCodeSize;

static volatile unsigned short gCodeBlockTable1[CODE_BLK_NUM];	//	Logical To Physical Block Table
static volatile unsigned short gCodeBlockTable2[CODE_BLK_NUM];	//	Logical To Physical Block Table

static volatile unsigned long PartitionTop[PARTITION_NUM+1]; 
static volatile T_NF_PARTITION NF_partition_table[PARTITION_NUM];

static volatile unsigned short PtableBlockTable[PARTITION_TABLE_PARAM_NUM];

static volatile unsigned char g_bf_updater_partition_flag = 0;
static volatile unsigned char g_bf_updater_partition_no;

#ifdef __GNUC__
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#ifdef CO_MEMORY_1GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_1GB BEGIN ---
static volatile unsigned char gPageData[58*1024] __attribute__((aligned(64)));		// Parameter Block & CodeBlockTable buffer
// --- REMOVE_MEMORY_1GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_1GB
#ifdef CO_MEMORY_2GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_2GB BEGIN ---
static volatile unsigned char gPageData[34*1024] __attribute__((aligned(64)));		// Parameter Block & CodeBlockTable buffer
// --- REMOVE_MEMORY_2GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_2GB
#ifdef CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_3GB BEGIN ---
static volatile unsigned char gPageData[34*1024] __attribute__((aligned(64)));		// Parameter Block & CodeBlockTable buffer
// --- REMOVE_MEMORY_3GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
static volatile unsigned short gRamCheckArea[2*1024] __attribute__((aligned(64)));	// for verify data (2048byte)
#else
static volatile __align(64) unsigned char gPageData[34*1024];		//	Parameter Block buffer (also use verification)
static volatile __align(64) unsigned short gRamCheckArea[2*1024];	// for verify data (2048byte)
#endif
static volatile unsigned long g_MDR_value;				// Boot Device (0:NAND,1:eMMC)

static BOOL g_secure_update_enable = FALSE;
static char g_secure_binary_filename[] = "I:\\SECURE.BIN";

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothiong Special

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
static int ul_memcmp(unsigned long *src, unsigned long *dst, int num_page);
static int read_parameter(unsigned long PhyBlk);
static int get_sdram_param(unsigned char *pBuffer, unsigned long size);
static int get_nand_param(unsigned char *src, unsigned long size);
static int get_partition_param(unsigned char *src, unsigned long size);
static unsigned int check_update_partition(unsigned short *BlockTable, unsigned long erase);
static void make_partition_table(void);
static void make_block0_page0(void);
static void make_block_PartitionTable(unsigned long table_size, unsigned long table_page);
static void make_FF(void);
static int write_data_and_make_LUT(unsigned short PhyBlk, unsigned long dwNumBlk, unsigned short *BlockTable, unsigned short *pWriteBuffer);
static int write_partition_table(unsigned long PhyBlk);

static unsigned char updater_partition_data(unsigned long dwParNum, unsigned long dwSize, unsigned long dwAddress);


static int ul_memcmp(unsigned long *src, unsigned long *dst, int num_page){
	int i,cnt;

	cnt = num_page * ( NandInfo.page_size/sizeof(unsigned long) );
	
	for( i = 0; i < cnt ;i++){
		if(*src != *dst){
			if( *src > *dst){
				return *src - *dst;
			}else{
				return -(*dst - *src);
			}
		}
		src++;
		dst++;
	}
	return 0;
}

// Recovey BootParameter,SDRAMParameter, NandParameter
static int read_parameter(unsigned long PhyBlk)
{
	int ret;
	int i;
	int read_page;
	int read_size;
	unsigned long  *p1,*p2;
	boot_parameter *p_table;

	// set ecc correction
	MWNF_Set_Ecc_Param(D_MWNF_ENABLE, D_MWNF_ECC_CAP16);
	MWNF_Set_Skip_Bytes(0);

	//read nand.par from NandFlash device bank0,block0,page0
	ret = MWNF_ReadPages(PhyBlk, 0, 1, (BYTE*)gPageData);
	if (ret != D_MWNF_OK) {
		BF_Debug_Print_Error(("_____Err MWNF_ReadPages ret=%d,  read_parameter in \n",ret));
		return -1;
	}
	p_table = (boot_parameter*)gPageData;

	// BootParameter Recovery(version - tggle_time)
	p1 = (unsigned long *)&BootParameter;
	p2 = (unsigned long *)p_table;
	for(i=0;i<BOOT_PARAM_NUM;i++){
		*p1++ = *p2++;
	}

	// boot.par recovery
	BootParameter.dvc_select = 1;		// NAND Boot
	BootParameter.LEDSEL1[0] = '\0';
	BootParameter.LEDSEL2[0] = '\0';
	BootParameter.linux_enable = 0x00000000;

	// get sdram parameter
	SDRAM_Parameter.PLLCNTL1_B = p_table->PLLCNTL1_B;
	SDRAM_Parameter.PLLCNTL1_A = p_table->PLLCNTL1_A;
	SDRAM_Parameter.PLLCNTL7 = p_table->PLLCNTL7;
	memcpy( (void*)&SDRAM_Parameter.ch[0], (void*)&p_table->sdram_parh[0], (sizeof(sdram_parameter)*2) );

	// boot image
	read_page = (BootParameter.image_offset / NandInfo.page_size);
	read_size = (BootParameter.image_size / NandInfo.page_size);
	ret = MWNF_ReadPages(PhyBlk, read_page, read_size, (BYTE*)FLASHDATA_SRAM_AREA);
	if (ret != 0) {
		BF_Debug_Print_Error(("_____Err MWNF_ReadPages ret=%d, in read_parameter(boot_image) [L:%d] \n",ret,__LINE__));
		return -1;
	}

// --- REMOVE_SECURE BEGIN ---
	// message digest
	if (BootParameter.dg_offset != 0) {
		read_page = (BootParameter.dg_offset / NandInfo.page_size);
		ret = MWNF_ReadPages(PhyBlk, read_page, 1, (BYTE*)&gPageData[4096]);
		if (ret != 0) {
			BF_Debug_Print_Error(("_____Err MWNF_ReadPages ret=%d, in read_parameter(dg_offset) [L:%d] \n",ret,__LINE__));
			return -1;
		}
		memcpy( (void*)SecureParameter.image_digest.data, (const void*)&gPageData[4096], 32);
	}

	// serial number
	if (BootParameter.sr_offset != 0) {
		read_page = (BootParameter.sr_offset / NandInfo.page_size);
		ret = MWNF_ReadPages(PhyBlk, read_page, 1, (BYTE*)&gPageData[4096]);
		if (ret != 0) {
			BF_Debug_Print_Error(("_____Err MWNF_ReadPages ret=%d, in read_parameter[sr_offset] [L:%d] \n",ret,__LINE__));
			return -1;
		}
		memcpy( (void*)&SecureParameter.serial_number, (const void *)&gPageData[4096], sizeof(SecureParameter.serial_number));
	}
// --- REMOVE_SECURE END ---

	MWNF_Set_Ecc_Param( (gNANDFlashParameter.ecc_config & 0x00000001),  ( (gNANDFlashParameter.ecc_config & 0x00000700) >> 8) );
	MWNF_Set_Skip_Bytes((gNANDFlashParameter.skip_bytes_conf & 0xFF));

	return 0;
}



// Recovey BootParameter,SDRAMParameter, NandParameter
static int read_parameter_em(void)
{
	int ret;
	int i;
	int read_page;
	int read_size;
	unsigned long  *p1,*p2;
	boot_parameter *p_table;

	//read nand.par from NandFlash  bank0,block0,page0
	ret = MWEM_DirectRead( 0, 2, (CHAR*)gPageData);
	if (ret != D_MWNF_OK) {
		BF_Debug_Print_Error(("_____Err MWEM_DirectRead ret=%d,  read_parameter_em in \n",ret));
		return -1;
	}
	p_table = (boot_parameter*)gPageData;

	// BootParameter(version - tggle_time) revovery
	p1 = (unsigned long *)&BootParameter;
	p2 = (unsigned long *)p_table;
	for(i=0;i<BOOT_PARAM_NUM;i++){
		*p1++ = *p2++;
	}

	// boot.par    BOOT_PARAMETER
	BootParameter.dvc_select = 2;
	BootParameter.LEDSEL1[0] = '\0';
	BootParameter.LEDSEL2[0] = '\0';
	BootParameter.linux_enable = 0x00000000;

	// get sdram parameter
	SDRAM_Parameter.PLLCNTL1_B = p_table->PLLCNTL1_B;
	SDRAM_Parameter.PLLCNTL1_A = p_table->PLLCNTL1_A;
	SDRAM_Parameter.PLLCNTL7 = p_table->PLLCNTL7;
	memcpy( (void*)&SDRAM_Parameter.ch[0], (void*)&p_table->sdram_parh[0], (sizeof(sdram_parameter)*2) );

	// boot image
	read_page = (BootParameter.image_offset / EM_CONFIG_DEFAULT_BLOCK_SIZE);
	read_size = (BootParameter.image_size / EM_CONFIG_DEFAULT_BLOCK_SIZE);
	ret = MWEM_DirectRead(read_page, read_size, (CHAR*)FLASHDATA_SRAM_AREA);
	if (ret != 0) {
		BF_Debug_Print_Error(("_____Err MWEM_DirectRead ret=%d, in read_parameter_em(boot_image) [L:%d] \n",ret,__LINE__));
		return -1;
	}

// --- REMOVE_SECURE BEGIN ---
	// message digest
	if (BootParameter.dg_offset != 0) {
		read_page = (BootParameter.dg_offset / EM_CONFIG_DEFAULT_BLOCK_SIZE);
		ret = MWEM_DirectRead( read_page, 1, (CHAR*)&gPageData[4096]);
		if (ret != 0) {
			BF_Debug_Print_Error(("_____Err MWEM_DirectRead ret=%d, in read_parameter_em[dg_offset] [L:%d] \n",ret,__LINE__));
			return -1;
		}
		memcpy( (void*)SecureParameter.image_digest.data, (void*)&gPageData[4096], 32);
	}

	// serial number
	if (BootParameter.sr_offset != 0) {
		read_page = (BootParameter.sr_offset / EM_CONFIG_DEFAULT_BLOCK_SIZE);
		ret = MWEM_DirectRead( read_page, 1, (CHAR*)&gPageData[4096]);
		if (ret != 0) {
			BF_Debug_Print_Error(("_____Err MWEM_DirectRead ret=%d, in read_parameter_em[sr_offset] [L:%d] \n",ret,__LINE__));
			return -1;
		}
		memcpy( (void*)&SecureParameter.serial_number, (const void *)&gPageData[4096], sizeof(SecureParameter.serial_number) );
	}
// --- REMOVE_SECURE END ---
	return 0;
}



/**
 * @brief		Set SDRAM parameter from parameter block
 * @param		i:pBuffer		SDRAM parameter data buffer
 * @return		(0)		OK
 * @note		None
 * @attention	None
 */
static int get_sdram_param(unsigned char *pBuffer, unsigned long size)
{
	//for reading parameter field of one file boot.
	int i;
	unsigned long *dst, *src;

	if (size < sizeof(SDRAM_PARAMETER)) {
		return -1;
	}

	// SDRAM_CAP & ch0 SDRAM parameter read
	src = (unsigned long*)pBuffer;
	dst = (unsigned long*)&SDRAM_Parameter.PLLCNTL1_B;

	for (i = 0; i < (SDRAM_PARAM_NUM+1); i++) {
		*dst++ = *src++;
	}
	return 0;
}


static int get_boot_param(unsigned char *pBuffer, unsigned long size)
{
	int i;
	unsigned long *dst, *src;
	unsigned long param_cnt;

	if (size < sizeof(T_BOOT_PARAMETER)) {
		return -1;
	}

	src = (unsigned long*)pBuffer;
	dst = (unsigned long*)&BootParameter.version;
	
	param_cnt = sizeof(T_BOOT_PARAMETER)/sizeof(long);
	
	for(i=0;i<param_cnt;i++){
		*dst++ = *src++;
	}

	BootParameter.LEDSEL1[3] = '\0';
	BootParameter.LEDSEL2[3] = '\0';
	return 0;
}


static int get_nand_param(unsigned char *pBuffer, unsigned long size)
{
	int i;
	unsigned long *dst, *src;
	unsigned long param_cnt;

	if (size < sizeof(NANDFlashParameter)) {
		return -1;
	}

	src = (unsigned long*)pBuffer;
	dst = (unsigned long*)&NANDFlashParameter.nf_clk_sel;
	param_cnt = sizeof(T_NF_NAND_PARAMETER)/sizeof(long);
	
	for(i=0;i<param_cnt;i++){
		*dst++ = *src++;
	}

	return 0;
}

static int get_partition_param(unsigned char *pBuffer, unsigned long size)
{
	int i;
	unsigned long *dst, *src;

	if (size < sizeof(PartitionParameter)) {
		return -1;
	}

	src = (unsigned long*)pBuffer;
	dst = (unsigned long*)&PartitionParameter.start_block;

	for (i = 0; i < (PARTITION_PARAMETER_CNT); i++) {
		*dst++ = *src++;
	}
	return 0;
}


static unsigned int check_update_partition(unsigned short *BlockTable, unsigned long erase)
{
	unsigned long LogicBlk;		// counter for Logical Block address
	unsigned long dwLastBlock;

	dwLastBlock = 0;

	for( LogicBlk = 0; LogicBlk < CODE_BLK_NUM; LogicBlk++ ) {
		if ((BlockTable[LogicBlk] != 0) && (BlockTable[LogicBlk] != 0xFFFF)) {
			if( erase == 1) {
				(void)MWNF_EraseBlk(BlockTable[LogicBlk]);
			}
		}
		else {
			break;
		}
	}
	if( LogicBlk != 0 ) {
		LogicBlk--;
		dwLastBlock = BlockTable[LogicBlk];
	}

	return dwLastBlock;
}

static void make_partition_table(void)
{
	int i, selector;

	// partition table
	memcpy((void*)NF_partition_table, (void*)gNF_partition_table, sizeof(NF_partition_table));

	// partition parameter and partition top
	for (i=0; i<PARTITION_NUM; i++) {
		PartitionTop[i] = (NF_partition_table[i].area[0].sector_num / (NandInfo.page_per_blk*(NandInfo.page_size/SECTOR_SIZE)));
		
		// pardata.par recovery PARTITION_TABLE
		selector = NF_partition_table[i].selector;
		PartitionParameter.data[i].area_size = NF_partition_table[i].area_size;
		PartitionParameter.data[i].load_addr = NF_partition_table[i].area[selector].load_addr;
		PartitionParameter.data[i].jump_addr = NF_partition_table[i].area[selector].jump_addr;
		PartitionParameter.data[i].load_size = NF_partition_table[i].area[selector].num_of_blocks;
// --- REMOVE_SECURE BEGIN ---
		//SECURE_PARAMETER  SecureParameter;
		memcpy( (void*)SecureParameter.ms_dg[i].data, (const void*)NF_partition_table[i].area[selector].message_digest, 32 );
// --- REMOVE_SECURE END ---
	}

	PartitionParameter.start_block = PartitionTop[0];

	if (NF_partition_table[PARTITION_NUM-1].area_size % (NandInfo.page_per_blk*NandInfo.page_size)){
		PartitionTop[PARTITION_NUM] = (PartitionTop[PARTITION_NUM-1] + (NF_partition_table[PARTITION_NUM-1].area_size / (NandInfo.page_per_blk*NandInfo.page_size) + 1));
	}
	else {
		PartitionTop[PARTITION_NUM] = (PartitionTop[PARTITION_NUM-1] + (NF_partition_table[PARTITION_NUM-1].area_size / (NandInfo.page_per_blk*NandInfo.page_size)));
	}
}



// for eMMC
static void make_partition_table_em(void)
{
	int i, selector;

	// partition table
	memcpy((void*)NF_partition_table, (void*)gNF_partition_table, sizeof(NF_partition_table));
	
	// partition parameter and partition top 
	for (i=0; i<PARTITION_NUM; i++) {
		PartitionTop[i] =  NF_partition_table[i].area[0].sector_num;					// eMMC
		
		// pardata.par recover
		selector = NF_partition_table[i].selector;
		PartitionParameter.data[i].area_size = NF_partition_table[i].area_size;
		PartitionParameter.data[i].load_addr = NF_partition_table[i].area[selector].load_addr;
		PartitionParameter.data[i].jump_addr = NF_partition_table[i].area[selector].jump_addr;
		PartitionParameter.data[i].load_size = NF_partition_table[i].area[selector].num_of_blocks;
// --- REMOVE_SECURE BEGIN ---
		//SECURE_PARAMETER  SecureParameter;
		memcpy( (void*)SecureParameter.ms_dg[i].data, (const void*)NF_partition_table[i].area[selector].message_digest, 32 );
// --- REMOVE_SECURE END ---
	}

	PartitionParameter.start_block = PartitionTop[0];

	if (NF_partition_table[PARTITION_NUM-1].area_size % EM_CONFIG_DEFAULT_BLOCK_SIZE ){
		PartitionTop[PARTITION_NUM] = (PartitionTop[PARTITION_NUM-1] + (NF_partition_table[PARTITION_NUM-1].area_size / EM_CONFIG_DEFAULT_BLOCK_SIZE + 1));
	}
	else {
		PartitionTop[PARTITION_NUM] = (PartitionTop[PARTITION_NUM-1] + (NF_partition_table[PARTITION_NUM-1].area_size / EM_CONFIG_DEFAULT_BLOCK_SIZE ));
	}
}



static void make_block0_page0(void)
{
	int	i;
	unsigned long  *p1,*p2;
	unsigned long  retry_mode;


	for (i = 0; i < NandInfo.page_size; i++) {
		gPageData[i] = 0x00;
	}
	
	p1 = (unsigned long *)&gPageData[0];
	// boot header
	p2 = (unsigned long*)&BootParameter.version;
	
	for(i=0;i<BOOT_HEADER_NUM;i++){
		*p1++ = *p2++;
	}

	for(i=0;i<6;i++){
		*p1++ = 0x00000000;	// reserved [32 - 55]
	}

	// retry mode
	if (BootParameter.retry_count != 0) {
		retry_mode = 0x1;
		retry_mode |= (BootParameter.boot_notice << 1);
		retry_mode |= (BootParameter.error_notice << 3);
		*p1++ = retry_mode;
		*p1++ = BootParameter.toggle_time;
	}
	else {
		*p1++ = 0x00000000;	// reserved [56 - 59]
		*p1++ = 0x00000000;	// reserved [60 - 63]
	}

	// partition table block
	*p1++ = PtableBlockTable[0];
	*p1++ = PtableBlockTable[1];
	*p1++ = PtableBlockTable[2];

	// sdram parameter
	p2 = (unsigned long*)&SDRAM_Parameter.PLLCNTL1_B;
	for(i=0;i<SDRAM_PARAM_NUM;i++){		//68
		*p1++ = *p2++;
	}

	// nand parameter
	p2 = (unsigned long*)&gNANDFlashParameter.nf_clk_sel;
	for(i=0;i<NAND_FLASH_PARAMETER_CNT;i++){
		*p1++ = *p2++;
	}
}


static void make_boot_header_em(void)
{
	int	i;
	unsigned long  *p1,*p2;
	unsigned long  retry_mode;


//	for (i = 0; i < NandInfo.page_size; i++) {		// NAND
	for (i = 0; i < 1024; i++) {					// eMMC
		gPageData[i] = 0x00;
	}
	
	p1 = (unsigned long *)&gPageData[0];
	// boot header
	p2 = (unsigned long*)&BootParameter.version;
	
	for(i=0;i<BOOT_HEADER_NUM;i++){
		*p1++ = *p2++;
	}

	for(i=0;i<6;i++){
		*p1++ = 0x00000000;	// reserved [32 - 55]
	}

	// retry mode
	if (BootParameter.retry_count != 0) {
		retry_mode = 0x1;
		retry_mode |= (BootParameter.boot_notice << 1);
		retry_mode |= (BootParameter.error_notice << 3);
		*p1++ = retry_mode;
		*p1++ = BootParameter.toggle_time;
	}
	else {
		*p1++ = 0x00000000;	// reserved [56 - 59]
		*p1++ = 0x00000000;	// reserved [60 - 63]
	}

	// partition table block
	*p1++ = PtableBlockTable[0];
	*p1++ = PtableBlockTable[1];
	*p1++ = PtableBlockTable[2];

	// sdram parameter
	p2 = (unsigned long*)&SDRAM_Parameter.PLLCNTL1_B;
	for(i=0;i<SDRAM_PARAM_NUM;i++){
		*p1++ = *p2++;
	}

	// nand parameter
	p2 = (unsigned long*)&gNANDFlashParameter.nf_clk_sel;
	for(i=0;i<NAND_FLASH_PARAMETER_CNT;i++){
		*p1++ = *p2++;
	}
}



// --- REMOVE_SECURE BEGIN ---
static void make_block0_param_1page(unsigned long size, unsigned long* data)
{
	unsigned long  *p;
	int i;

	for (i = 0; i < NandInfo.page_size; i++) {
		gPageData[i] = 0xFF;
	}
	
	p = (unsigned long*)&gPageData[0];
	for( i = 0; i < (size/4); i++ ){
		*p++ = *data++;
	}
}

static void make_param_1sector(unsigned long size, unsigned long* data)
{
	unsigned long  *p;
	int i;

	for (i = 0; i < EM_CONFIG_DEFAULT_BLOCK_SIZE; i++) {
		gPageData[i] = 0xFF;
	}
	
	p = (unsigned long*)&gPageData[0];
	for( i = 0; i < (size/4); i++ ){
		*p++ = *data++;
	}
}
// --- REMOVE_SECURE END ---


static void make_block_PartitionTable(unsigned long table_size, unsigned long table_page)
{
	unsigned char *p1, *p2;
	int i;

	for (i = 0; i < (table_page * NandInfo.page_size); i++) {
		gPageData[i] = 0xFF;
	}
	
	p1 = (unsigned char*)&gPageData[0];
	p2 = (unsigned char*)&NF_partition_table[0];
	for( i = 0; i < table_size; i++ ){
		*(p1++) = *(p2++);
	}
}

static void make_FF(void)
{
	int i;

	for (i = 0; i < NandInfo.page_size; i++){
		gPageData[i] = 0xFF;
	}
}


static void make_PartitionTable(unsigned long table_size, unsigned long table_sec)
{
	unsigned char *p1, *p2;
	int i;

	
	if(( table_size % EM_CONFIG_DEFAULT_BLOCK_SIZE ) != 0) {
		for (i = ((table_sec - 1) * EM_CONFIG_DEFAULT_BLOCK_SIZE);
			 i < (table_sec * EM_CONFIG_DEFAULT_BLOCK_SIZE);
			 i++) {
			gPageData[i] = 0xFF;
		}
	}
	p1 = (unsigned char*)&gPageData[0];
	p2 = (unsigned char*)&NF_partition_table[0];
	for( i = 0; i < table_size; i++ ){
		*(p1++) = *(p2++);
	}
}

static int write_data_and_make_LUT(unsigned short PhyBlk, unsigned long dwNumBlk, unsigned short *BlockTable, unsigned short *pWriteBuffer)
{
	unsigned short LogicBlk;		// counter for Logical Block address
	int ret;
	unsigned long ProgrammedPage = 0;
	int percent, percent_bak = 0, page, page_ok = 0;
	unsigned char bWrite = 0, Bad_blk;
//	char bsp_str[7]={0x08,0x08,0x08,0x08,0x08,0x08};

//	printf("[%03d%%]",0);

	// Extract Normal Block (Eject Bad Block)
	for(LogicBlk = 0; LogicBlk < CODE_BLK_NUM; LogicBlk++){
		BlockTable[LogicBlk] = 0xFFFF;	// exception value.
	}

	LogicBlk = 0;
	while (LogicBlk < dwNumBlk) {

		if (PhyBlk > gLimitOfCodeAreaBlock) {	// fail safe
//			printf("\n");
			BF_Debug_Print_Error(("_____Err data size is over the Limit code area [L:%d] \n",__LINE__));
			return -1;
		}

		if (gNANDFlashParameter.FACTOR_CHECK == 1) {
			ret = MWNF_Check_Initial_BadBlock(PhyBlk, &Bad_blk);
			if (ret != D_MWNF_OK) {
				return -1;
			}
			else if(Bad_blk != 1) {
				bWrite = 1;	// not bad block
			}
			else {
				printf("Found bad block = %d \n", PhyBlk);
				bWrite = 0;	// bad block
			}
		} else {
			bWrite = 1;		// no bad block check
		}
		if (bWrite == 1) {
			ret = MWNF_EraseBlk(PhyBlk);
			if (ret == D_MWNF_OK) {
				//Erase OK
				ProgrammedPage = NandInfo.page_per_blk;
				ret = MWNF_WritePages_NoRetry(PhyBlk, 0, ProgrammedPage, (BYTE*)pWriteBuffer);
				if (ret == D_MWNF_OK) {
					//Write OK
					for ( page=0; page < ProgrammedPage; page++ ) {
						ret = MWNF_ReadPages(PhyBlk, page, 1, (BYTE*)gRamCheckArea);
						if (ret == D_MWNF_OK) {
							ret = ul_memcmp((unsigned long *)gRamCheckArea,
											(unsigned long *)(pWriteBuffer + (page * (NandInfo.page_size/sizeof(short)))), 1);
							if (ret == 0) {
								page_ok++;	// verify OK(page)
							}
						} else {
							printf("Read block %d failed\n", PhyBlk);
						}
					}
					if ( page == page_ok ) {
						//verify OK(all pages of block)
						BlockTable[LogicBlk++] = PhyBlk;
						pWriteBuffer += (ProgrammedPage * (NandInfo.page_size/sizeof(short) ) );	// pointer increment
						
						percent = ((float)LogicBlk / (float)dwNumBlk)*100;
						if(percent != percent_bak) {
							percent_bak = percent;
//							printf("%s[%03d%%]", bsp_str, percent);

							if((g_bf_updater_partition_flag == 1) && ((percent % 10) == 0)) {
								FJ_Updater_Progress_CB((E_FJ_UPDATER_PARTITION)g_bf_updater_partition_no, percent);
							}
						}
					}
					page_ok = 0;
				}
			} else {
				printf("Erase block %d failed\n", PhyBlk);
			}
		}
		PhyBlk++;
	}
//	printf("\n");
	return 0;
}



static int write_partition_table(unsigned long PhyBlk)
{
	int ret;
	int table_size, table_page;
	int page_num = 0;

	// Erase NF parameter block
	ret = MWNF_EraseBlk(PhyBlk);
	if (ret != 0) {
		return -1;
	}

	table_size = sizeof(NF_partition_table);
	if( table_size % NandInfo.page_size ){
		table_page = (table_size / NandInfo.page_size + 1);
	}else{
		table_page = (table_size / NandInfo.page_size);
	}

	make_block_PartitionTable(table_size, table_page);
	ret = MWNF_WritePages_NoRetry( PhyBlk, page_num, table_page, (BYTE*)&gPageData);	//writes Partition table
	if (ret == 0) {
		ret = MWNF_ReadPages( PhyBlk, page_num, table_page, (BYTE*)RAMCHECK_AREA);
		if (ret == 0) {
			ret = ul_memcmp((unsigned long *)&gPageData, (unsigned long *)RAMCHECK_AREA, table_page);
		}
	}
	if (ret != 0) {
		return -1;
	}
	page_num = table_page;

	make_FF();
	for(; page_num < NandInfo.page_per_blk; page_num++) {
		ret = MWNF_WritePages_NoRetry( PhyBlk, page_num, 1, (BYTE*)gPageData);			//writes ALLF to other pages
		if (ret == 0) {
			ret = MWNF_ReadPages( PhyBlk, page_num, 1, (BYTE*)RAMCHECK_AREA);
			if (ret == 0) { 
				ret = ul_memcmp((unsigned long *)gPageData, (unsigned long *)RAMCHECK_AREA, 1 );
			}
		}
		if (ret != 0) {
			return -1;
		}
	}

	return 0;	// verification OK.
}


static int write_partition_table_em(unsigned long Sector)
{
	int ret;
	int table_size;
	int dwNumSec;

	table_size = sizeof(NF_partition_table);
	if( table_size % EM_CONFIG_DEFAULT_BLOCK_SIZE ){
		dwNumSec = (table_size / EM_CONFIG_DEFAULT_BLOCK_SIZE + 1);
	}else{
		dwNumSec = (table_size / EM_CONFIG_DEFAULT_BLOCK_SIZE);
	}

	make_PartitionTable(table_size, dwNumSec);

	ret = MWEM_DirectWrite( Sector, dwNumSec, (char*)&gPageData);	//writes Partition table
	if (ret != 0) {
		return -1;
	}

	return 0;	// verification OK.
}



static int write_boot_header(unsigned long PhyBlk)
{
	int ret;
	int page_num;
	int write_page;
	int write_size;

	// Erase NF parameter block
	ret = MWNF_EraseBlk(PhyBlk);
	if (ret != 0) {
		return -1;
	}

	// set ecc correction (page0)
	MWNF_Set_Ecc_Param(D_MWNF_ENABLE, D_MWNF_ECC_CAP16);
	// set skip byte (page0)
	MWNF_Set_Skip_Bytes(0);

	// boot header
	make_block0_page0();

	ret = MWNF_WritePages_NoRetry( PhyBlk, 0, 1, (BYTE*)gPageData); //writes parameter to block0 page 0
	if ( ret == 0 ) {
		ret = MWNF_ReadPages( PhyBlk, 0, 1, (BYTE*)RAMCHECK_AREA);

		if (ret == 0) {
			ret = ul_memcmp((unsigned long *)gPageData, (unsigned long *)RAMCHECK_AREA, 1);
		}
	}
	if (ret != 0) {
		return -1;
	}

	page_num = 1;

	// boot image
	write_page = (BootParameter.image_offset / NandInfo.page_size);
	write_size = (BootParameter.image_size / NandInfo.page_size);

	if (write_page > 1) {
		// ALLF
		make_FF();
		for(; page_num < write_page; page_num++) {
			ret = MWNF_WritePages_NoRetry( PhyBlk, page_num, 1, (BYTE*)gPageData);	//writes ALLF
			if ( ret == 0 ) {
				ret = MWNF_ReadPages( PhyBlk, page_num, 1, (BYTE*)RAMCHECK_AREA);
				if (ret == 0) { 
					ret = ul_memcmp((unsigned long *)gPageData, (unsigned long *)RAMCHECK_AREA, 1 );
				}
			}
			if (ret != 0) {
				return -1;
			}
		}
	}

	ret = MWNF_WritePages_NoRetry( PhyBlk, write_page, write_size, (BYTE*)FLASHDATA_SRAM_AREA);
	if ( ret == 0 ) {
		ret = MWNF_ReadPages( PhyBlk, write_page, write_size, (BYTE*)RAMCHECK_AREA);
		if (ret == 0) {
			ret = ul_memcmp((unsigned long *)FLASHDATA_SRAM_AREA, (unsigned long *)RAMCHECK_AREA, write_size);
		}
	}
	if (ret != 0) {
		return -1;
	}
	page_num +=write_size;
// --- REMOVE_SECURE BEGIN ---
	// message digest
	if (BootParameter.dg_offset != 0) {
		write_page = (BootParameter.dg_offset / NandInfo.page_size);

		if (write_page > page_num) {
			// ALLF
			make_FF();
			for(; page_num < write_page; page_num++) {
				ret = MWNF_WritePages_NoRetry( PhyBlk, page_num, 1, (BYTE*)gPageData);	//writes ALLF
				if ( ret == 0 ) {
					ret = MWNF_ReadPages( PhyBlk, page_num, 1, (BYTE*)RAMCHECK_AREA);
					if (ret == 0) { 
						ret = ul_memcmp((unsigned long *)gPageData, (unsigned long *)RAMCHECK_AREA, 1 );
					}
				}
				if (ret != 0) {
					return -1;
				}
			}
		}

		make_block0_param_1page(32, (unsigned long*)&SecureParameter.image_digest);
		ret = MWNF_WritePages_NoRetry( PhyBlk, write_page, 1, (BYTE*)gPageData);
		if ( ret == 0 ) {
			ret = MWNF_ReadPages( PhyBlk, write_page, 1, (BYTE*)RAMCHECK_AREA);
			if (ret == 0) {
				ret = ul_memcmp((unsigned long *)gPageData, (unsigned long *)RAMCHECK_AREA, 1);
			}
		}
		if (ret != 0) {
			return -1;
		}
		page_num++;
	}

	// serial number
	if (BootParameter.sr_offset != 0) {
		write_page = (BootParameter.sr_offset / NandInfo.page_size);

		if (write_page > page_num) {
			// ALLF
			make_FF();
			for(; page_num < write_page; page_num++) {
				ret = MWNF_WritePages_NoRetry( PhyBlk, page_num, 1, (BYTE*)gPageData);	//writes ALLF
				if ( ret == 0 ) {
					ret = MWNF_ReadPages( PhyBlk, page_num, 1, (BYTE*)RAMCHECK_AREA);
					if (ret == 0) { 
						ret = ul_memcmp((unsigned long *)gPageData, (unsigned long *)RAMCHECK_AREA, 1 );
					}
				}
				if (ret != 0) {
					return -1;
				}
			}
		}

		make_block0_param_1page(4, (unsigned long*)&SecureParameter.serial_number);
		ret = MWNF_WritePages_NoRetry( PhyBlk, write_page, 1, (BYTE*)gPageData);
		if ( ret == 0 ) {
			ret = MWNF_ReadPages( PhyBlk, write_page, 1, (BYTE*)RAMCHECK_AREA);
			if (ret == 0) {
				ret = ul_memcmp((unsigned long *)gPageData, (unsigned long *)RAMCHECK_AREA, 1);
			}
		}
		if (ret != 0) {
			return -1;
		}
		page_num++;
	}
// --- REMOVE_SECURE END ---

	// other pages
	make_FF();
	for(; page_num < NandInfo.page_per_blk; page_num++) {
		ret = MWNF_WritePages_NoRetry( PhyBlk, page_num, 1, (BYTE*)gPageData);	//writes ALLF to block0 other pages
		if ( ret == 0 ) {
			ret = MWNF_ReadPages( PhyBlk, page_num, 1, (BYTE*)RAMCHECK_AREA);
			if (ret == 0) { 
				ret = ul_memcmp((unsigned long *)gPageData, (unsigned long *)RAMCHECK_AREA, 1 );
			}
		}
		if (ret != 0) {
			return -1;
		}
	}

	return 0;	// verification OK.
}


static int write_boot_header_em(unsigned long emSector)
{
	int ret;
	int sector_no;
	int sector_num;

	///////////////////////
	// boot header
	///////////////////////
	make_boot_header_em();

	sector_no  = emSector;
	sector_num = 2;
	ret = MWEM_DirectWrite( sector_no, sector_num, (char*)gPageData);
	if (ret != 0) {
		BF_Debug_Print_Error(("_____Err MWEM_DirectWrite (eMMC:boot header) ret=%d [L:%d] \n", ret, __LINE__));
		return -1;
	}

	///////////////////////
	// boot image
	///////////////////////

	sector_no  = (BootParameter.image_offset / EM_CONFIG_DEFAULT_BLOCK_SIZE);
	sector_num = (BootParameter.image_size / EM_CONFIG_DEFAULT_BLOCK_SIZE);
	
	ret = MWEM_DirectWrite( sector_no, sector_num, (char*)FLASHDATA_SRAM_AREA);
	if (ret != 0) {
		BF_Debug_Print_Error(("_____Err MWEM_DirectWrite (eMMC:boot image) ret=%d [L:%d] \n", ret, __LINE__));
		return -1;
	}

// --- REMOVE_SECURE BEGIN ---
	///////////////////////
	// message digest
	///////////////////////
	if (BootParameter.dg_offset != 0) {

		sector_no = (BootParameter.dg_offset / EM_CONFIG_DEFAULT_BLOCK_SIZE);
		sector_num =1;
		
		make_param_1sector(32, (unsigned long*)&SecureParameter.image_digest);

		ret = MWEM_DirectWrite( sector_no, sector_num, (char*)gPageData);
		if (ret != 0) {
			BF_Debug_Print_Error(("_____Err MWEM_DirectWrite (eMMC:message_digest) ret=%d [L:%d] \n", ret, __LINE__));
			return -1;
		}
	}

	///////////////////////
	// serial number
	///////////////////////
	if (BootParameter.sr_offset != 0) {
		sector_no = (BootParameter.sr_offset / EM_CONFIG_DEFAULT_BLOCK_SIZE);
		sector_num =1;
		
		make_param_1sector(4, (unsigned long*)&SecureParameter.serial_number);

		ret = MWEM_DirectWrite( sector_no, sector_num, (char*)gPageData);
		if (ret != 0) {
			return -1;
		}
	}
// --- REMOVE_SECURE END ---
	return 0;	// verification OK.
}


static unsigned char updater_partition_data(unsigned long dwParNum, unsigned long dwSize, unsigned long dwAddress)
{
	int ret;
	unsigned long old_selector, new_selector;
	unsigned long dwNumBlk;
	unsigned long dwStartBlock, dwLastBlock;

	// clear the Linux hibernation area
	if (dwParNum == PARTITION_NUM_OF_HIBERNATION) {
		new_selector = 0;
		dwStartBlock = PartitionTop[dwParNum];
		gLimitOfCodeAreaBlock = (PartitionTop[dwParNum+1] - 1);
	}
	else {
		// start/end block 
		if (NF_partition_table[dwParNum].selector == 0) {
			old_selector = 0;
			new_selector = 1;

			dwStartBlock = (PartitionTop[dwParNum] + ((PartitionTop[dwParNum+1]-PartitionTop[dwParNum]) / 2));

			dwLastBlock = check_update_partition((unsigned short*)NF_partition_table[dwParNum].area[old_selector].lut, 0);
			if (dwLastBlock >= dwStartBlock) {
				dwStartBlock = (dwLastBlock + 1);
			}

			gLimitOfCodeAreaBlock = (PartitionTop[dwParNum+1] - 1);
		}
		else {
			old_selector = 1;
			new_selector = 0;

			dwStartBlock = PartitionTop[dwParNum];

			if ((NF_partition_table[dwParNum].area[old_selector].lut[0] == 0) ||
				(NF_partition_table[dwParNum].area[old_selector].lut[0] == 0xFFFF)) {
				gLimitOfCodeAreaBlock = (PartitionTop[dwParNum+1] - 1);
			}
			else {
				gLimitOfCodeAreaBlock = (NF_partition_table[dwParNum].area[old_selector].lut[0] - 1);
			}
		}
	}

	// set partition table
	NF_partition_table[dwParNum].area[new_selector].data_size     = dwSize;
	NF_partition_table[dwParNum].area[new_selector].sector_num    = (dwStartBlock*NandInfo.page_per_blk*(NandInfo.page_size/SECTOR_SIZE));
	NF_partition_table[dwParNum].area[new_selector].load_addr     = PartitionParameter.data[dwParNum].load_addr;
	NF_partition_table[dwParNum].area[new_selector].jump_addr     = PartitionParameter.data[dwParNum].jump_addr;
	NF_partition_table[dwParNum].area[new_selector].num_of_blocks = PartitionParameter.data[dwParNum].load_size;
// --- REMOVE_SECURE BEGIN ---
	// set message_digest
	memcpy( (void*)&NF_partition_table[dwParNum].area[new_selector].message_digest,  (void*)&SecureParameter.ms_dg[dwParNum].data, 32 );
// --- REMOVE_SECURE END ---

	if( dwSize % (NandInfo.page_per_blk*NandInfo.page_size) ){
		dwNumBlk = ( dwSize/(NandInfo.page_per_blk*NandInfo.page_size)+1 );
	}
	else {
		dwNumBlk = ( dwSize/(NandInfo.page_per_blk*NandInfo.page_size) );
	}

//	BF_Debug_Print_Error(("_____DEGUB (NAND) dwParNum=%ld, load_size=%ld [L:%d] \n", dwParNum, PartitionParameter.data[dwParNum].load_size, __LINE__));

	// old data erase
	(void)check_update_partition((unsigned short*)NF_partition_table[dwParNum].area[new_selector].lut, 1);

	ret = write_data_and_make_LUT( dwStartBlock, 
									dwNumBlk,
									(unsigned short*)NF_partition_table[dwParNum].area[new_selector].lut,
									(unsigned short*)dwAddress );
	if( ret != 0 ){
		BF_Debug_Print_Error(("_____Err WriteERR (NAND) ret=%d [L:%d] \n", ret, __LINE__));
		return NAND_DATA_WRITE_ERR;
	}

	// selector update
	NF_partition_table[dwParNum].selector = new_selector;

	return WRITE_SUCCESS;
}


static unsigned char updater_partition_data_em(unsigned long dwParNum, unsigned long dwSize, unsigned long dwAddress)
{
	int ret;
	unsigned long new_selector, old_selector;
	unsigned long dwNumSec;
	unsigned long dwStartBlock;		// StartSector of new_selector
	unsigned long dwLastBlock;		// LastSector of old_selector

	// start/end block 
	if (NF_partition_table[dwParNum].selector == 0) {
		old_selector = 0;
		new_selector = 1;
		
		dwStartBlock = (PartitionTop[dwParNum] + ((PartitionTop[dwParNum+1]-PartitionTop[dwParNum]) / 2));

		// data size check of the 0 side.
		if ( NF_partition_table[dwParNum].area[0].data_size % EM_CONFIG_DEFAULT_BLOCK_SIZE ){
			dwLastBlock = PartitionTop[dwParNum] + ( NF_partition_table[dwParNum].area[0].data_size / EM_CONFIG_DEFAULT_BLOCK_SIZE ) + 1;
		}
		else{
			dwLastBlock = PartitionTop[dwParNum] + ( NF_partition_table[dwParNum].area[0].data_size / EM_CONFIG_DEFAULT_BLOCK_SIZE );
		}
		
		if (dwLastBlock >= dwStartBlock) {
			dwStartBlock = (dwLastBlock + 1);
		}

		gLimitOfCodeAreaBlock = (PartitionTop[dwParNum+1] - 1);
	}
	else {
		old_selector = 1;
		new_selector = 0;

		dwStartBlock = PartitionTop[dwParNum];
		gLimitOfCodeAreaBlock = (NF_partition_table[dwParNum].area[old_selector].sector_num - 1);
	}

	// set partition table
	NF_partition_table[dwParNum].area[new_selector].data_size     = dwSize;
	NF_partition_table[dwParNum].area[new_selector].sector_num    = dwStartBlock;
	NF_partition_table[dwParNum].area[new_selector].load_addr     = PartitionParameter.data[dwParNum].load_addr;
	NF_partition_table[dwParNum].area[new_selector].jump_addr     = PartitionParameter.data[dwParNum].jump_addr;
	NF_partition_table[dwParNum].area[new_selector].num_of_blocks = PartitionParameter.data[dwParNum].load_size;
// --- REMOVE_SECURE BEGIN ---
	// set message_digest
	memcpy( (void*)&NF_partition_table[dwParNum].area[new_selector].message_digest,  (void*)&SecureParameter.ms_dg[dwParNum].data, 32 );
// --- REMOVE_SECURE END ---

	if( dwSize % EM_CONFIG_DEFAULT_BLOCK_SIZE ){
		dwNumSec = ( dwSize/EM_CONFIG_DEFAULT_BLOCK_SIZE + 1 );
	}
	else {
		dwNumSec = ( dwSize/EM_CONFIG_DEFAULT_BLOCK_SIZE );
	}

//	BF_Debug_Print_Error(("_____DEGUB (eMMC) dwParNum=%ld, load_size=%ld [L:%d] \n", dwParNum, PartitionParameter.data[dwParNum].load_size, __LINE__));

	// check
	if (( dwStartBlock + dwNumSec)  > gLimitOfCodeAreaBlock ){
		BF_Debug_Print_Error(("_____Err data size is over the Limit code area[start=%ld, size=%ld, Limit=%u] [L:%d] \n", dwStartBlock, dwNumSec, gLimitOfCodeAreaBlock, __LINE__));
		return -1;
	}
	
	
	// output progress(10%)
	FJ_Updater_Progress_CB((E_FJ_UPDATER_PARTITION)g_bf_updater_partition_no, 10);

	ret = MWEM_DirectWrite(dwStartBlock, dwNumSec, (char*)dwAddress);
	
	if( ret != 0 ){
		BF_Debug_Print_Error(("_____Err WriteERR (eMMC) ret=%d [L:%d] \n", ret, __LINE__));
		return -1;
	}

	// output progress(100%)
	FJ_Updater_Progress_CB((E_FJ_UPDATER_PARTITION)g_bf_updater_partition_no, 100);

	// selector update
	NF_partition_table[dwParNum].selector = new_selector;

	return WRITE_SUCCESS;
}


static int updater_main_nand(int* fileid, unsigned long size, unsigned char* buff)
{
	int ercd = 0;
	T_BF_UPDATER_DATA_HEAD *data_head;
	unsigned long length;
	unsigned long PhyBlk;
	unsigned long max_count;
// --- REMOVE_SECURE BEGIN ---
	T_HASH_DATA   hash_data[4];
	unsigned long hash_data_cnt=0;
	unsigned long result;
	unsigned char id;
// --- REMOVE_SECURE END ---

	MWNF_Get_Dev_Info((T_NF_DEV_INFO*)&NandInfo);

	// make partition table(PartitionParameter...)
	make_partition_table();

	// read parameter
	ercd = read_parameter(PARA_START_BLK_MAIN);
	if ( ercd !=0 ){
		return -1;
	}
// new spec start
	// Update file read
	ercd = BF_Fs_If_Read(*fileid, buff, size, &length);
	if (ercd != 0) {
		BF_Debug_Print_Error(("_____Err BF_Fs_If_Read ret=%d, in updater_main_nand(data_head)  [L:%d] \n",ercd,__LINE__));
		return -1;
	}
	data_head = (T_BF_UPDATER_DATA_HEAD*)buff;

// --- REMOVE_SECURE BEGIN ---
	// Secure Check
	if ((data_head->type == PARTITION_TYPE_SECURE_PARAM )&& (data_head->param == 0xFF) && (g_secure_update_enable)){
		{
			FW_INT32 file_id;
			unsigned long secure_bin_size = 32;
			unsigned char* secure_bin_buff = (unsigned char*)(RAMCHECK_AREA + 0xFFF00);
			unsigned char i;
			
			file_id = BF_Fs_If_Open((const FW_CHAR*)g_secure_binary_filename, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
			if (file_id == 0){
				BF_Debug_Print_Error(("[Updater]BF_Fs_If_Open:%s err=%d\n" ,g_secure_binary_filename, file_id));
				return -1;
			}
			ercd = BF_Fs_If_Read(file_id, secure_bin_buff, secure_bin_size, &length);
			if (ercd != 0) {
				BF_Debug_Print_Error(("_____Err BF_Fs_If_Read ret=%d, in updater_main_nand(secure binary)  [L:%d] \n",ercd,__LINE__));
				BF_Fs_If_Close(file_id);
				return -1;
			}
			BF_Fs_If_Close(file_id);

			for(i = 0; i < secure_bin_size; i+=4)
			{
//				BF_Debug_Print_Information(("Binary Compare[%d] 0x%08lx vs 0x%08lx\n"
//					, i
//					, *(unsigned long *)(buff + sizeof(T_BF_UPDATER_DATA_HEAD) + i)
//					, *(unsigned long *)(secure_bin_buff + i)));
				if(*(unsigned long *)(buff + sizeof(T_BF_UPDATER_DATA_HEAD) + i) != *(unsigned long *)(secure_bin_buff + i)){
					BF_Debug_Print_Error(("_____Err secure binary compare fail [%d] 0x%08lx vs 0x%08lx, in updater_main_nand  [L:%d] \n"
						, i
						, *(unsigned long *)(buff + sizeof(T_BF_UPDATER_DATA_HEAD) + i)
						, *(unsigned long *)(secure_bin_buff + i)
						, __LINE__));
					return -1;
				}
			}
		}
		
		if ( size > (sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen) ){
			size  -= ( sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen );
		}
		else{
			BF_Debug_Print_Error(("_____Err Size NG. file size=%ld, datalen=%ld, in updater_main_nand() [L:%d] \n",size, data_head->datalen,__LINE__));
			return -1;
		}
		// Set top address and size of DATA.bin 
		memset(hash_data, 0, sizeof(hash_data));
		hash_data[hash_data_cnt].addr = (unsigned long)(buff + sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen)+0x20000000;
		hash_data[hash_data_cnt].size = size;
		hash_data_cnt++;
		memcpy((void*)RAMCHECK_AREA, (const void*)hash_data, sizeof(hash_data));


		// Pre Procedure of IPCU cmd
		BF_Debug_Print_Information(("IPCU_OPEN\n" ));
		ercd = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_14, &id);
		// Check IPCU cmd
		if ( ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(("_____Err IPCU NG, IPCU_OPEN ercd=%d, in updater_main_nand [L:%d] \n",ercd,__LINE__));
			return -1;
		}
//		BF_Debug_Print_Information(("IPCU d_add=0x%08lx \n", (unsigned long)(buff + sizeof(T_BF_UPDATER_DATA_HEAD))+0x20000000 ));
//		BF_Debug_Print_Information(("IPCU add0 =0x%08lx \n", hash_data[0].addr   ));
//		BF_Debug_Print_Information(("IPCU hash =0x%08lx \n", (unsigned long)(void*)(RAMCHECK_AREA + 0x20000000)   ));
//		BF_Debug_Print_Information(("IPCU cnt  =0x%08lx \n", hash_data_cnt  ));

		BF_Debug_Print_Information(("Send IPCU message to CM0(forSecureCheck..) \n" ));
		//  +0x20000000:The difference of SDRAM address of the CA7 and CM0.
		ercd = FJ_IPCU_Check_Secure_Code_CM0( (ULONG)(buff + sizeof(T_BF_UPDATER_DATA_HEAD))+0x20000000, (ULONG)RAMCHECK_AREA+0x20000000, hash_data_cnt, &result );		/* pgr0539 */
		BF_Debug_Print_Information(("return IPCU message(forSecureCheck) \n" ));
		FJ_IPCU_Close(E_FJ_IPCU_MAILBOX_TYPE_14);
		BF_Debug_Print_Information(("IPCU_Close \n" ));
		// Check IPCU cmd
		if ( ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(("_____Err IPCU NG, IPCU_Close ercd=%d, in updater_main_nand [L:%d] \n",ercd,__LINE__));
			return -1;
		}


		// Check HashCheck result
		if ( result !=0 ) {
			BF_Debug_Print_Error(("_____Err HashCheck result NG (result=%ld), in updater_main_nand  [L:%d] \n", result, __LINE__));
			return -1;
		}
		BF_Debug_Print_Information(("HashCheck result PASS, in updater_main_nand	[L:%d] \n", __LINE__));
		// Secure OK. set DATA.bin top
		buff += sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen;
	}
	else{
		// normal
		if(g_secure_update_enable){
			BF_Debug_Print_Error(("_____Err SecureParameter read error, in updater_main_nand  [L:%d] \n", __LINE__));
			return -1;
		}
	}
// --- REMOVE_SECURE END ---
// new spec end

	while (size != 0) {
		if (sizeof(T_BF_UPDATER_DATA_HEAD) > size) {
			size = 0;
		}
		else {
			size -= sizeof(T_BF_UPDATER_DATA_HEAD);
		}
		// Add Header size [new_spec]
		data_head = (T_BF_UPDATER_DATA_HEAD*)buff;
		buff += sizeof(T_BF_UPDATER_DATA_HEAD);
		
		// signature check
		if (strncmp((const char*)data_head->signature, "MILBEAUT", 8) != 0) {
			BF_Debug_Print_Error(("_____Err signature   [L:%d] \n",__LINE__));
			return -1;
		}

		switch (data_head->type) {
			case PARTITION_TYPE_NAND_PARAM:
				ercd = get_nand_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_SDRAM_PARAM:
				ercd = get_sdram_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_PARTITION_PARAM:
				ercd = get_partition_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_BOOT_PARAM:
				ercd = get_boot_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_BOOTSTRAP :
				// boot image
				memcpy((void*)FLASHDATA_SRAM_AREA, (void*)buff, data_head->datalen);
				if(BootParameter.image_size != data_head->datalen) {
					BootParameter.image_size = data_head->datalen;
				}
				break;

// --- REMOVE_SECURE BEGIN ---
			case PARTITION_TYPE_SECURE_PARAM :

				if (data_head->param == PARTITION_PARAMETER_MDgst){
					// Check digest_offset of boot_image(at Writer)
					if ( BootParameter.dg_offset == 0){
						BF_Debug_Print_Error(("_____Err When written in the beginning, message_digest of boot_image was not. \n" ));
						BF_Debug_Print_Error(("_____Err boot_images that were written in the non-secure, can not be updated in a secure. [L:%d] \n", __LINE__));
						return -1;
					}
					// Update the corresponding data_head->param of the Secure_param
					memcpy((void*)SecureParameter.image_digest.data, (void*)buff,  data_head->datalen );
				}
				else if (data_head->param < PARTITION_NUM){
					// Update the corresponding data_head->param of the Secure_param
					memcpy((void*)SecureParameter.ms_dg[data_head->param].data, (void*)buff,  data_head->datalen );
				}
				else{
					BF_Debug_Print_Error(("_____Err data_head->param over =%d, in updater_main_nand(SECURE_PARAM)  [L:%d] \n",data_head->param,__LINE__));
					return GETPAR_ERR;
				}
				break;

// --- REMOVE_SECURE END ---

			case PARTITION_TYPE_PARTITION_DATA :
				if (data_head->param >= PARTITION_NUM) {
					BF_Debug_Print_Error(("_____Err data_head->param over =%d, in updater_main_nand(PARTITION_DATA)  [L:%d] \n",data_head->param,__LINE__));
					return GETPAR_ERR;
				}
				

				g_bf_updater_partition_flag = 1;
				g_bf_updater_partition_no   = data_head->param;
				ercd = updater_partition_data(data_head->param, data_head->datalen, (unsigned long)buff);
				g_bf_updater_partition_flag = 0;
				break;

			default:
				break;
		}

		if (ercd != 0) {
			switch (data_head->type) {
				case PARTITION_TYPE_NAND_PARAM:
				case PARTITION_TYPE_SDRAM_PARAM:
				case PARTITION_TYPE_PARTITION_PARAM:
					return GETPAR_ERR;
				default:
					return NAND_DATA_WRITE_ERR;
			}
		}

		if (data_head->datalen > size) {
			size = 0;
		}
		else {
			size -= data_head->datalen;
			buff += data_head->datalen;
		}
	}
	
	printf("**** START **** Partition table write. \n");
	// Partition table write
	if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 0) {
		for (PhyBlk = PARTITION_TABLE_START_BLK1; PhyBlk < (PARTITION_TABLE_START_BLK1 + PARTITION_TABLE_BLK_NUM); PhyBlk++) {
			ercd = write_partition_table(PhyBlk);
			if (ercd == 0) {
				break;
			}
		}
		if (ercd != 0) {
			return NAND_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR1;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = gPtableBlockTable[PARTITION_TABLE_AREA_0];
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = PhyBlk;
	}
	else {
		for (PhyBlk = PARTITION_TABLE_START_BLK0; PhyBlk < (PARTITION_TABLE_START_BLK0 + PARTITION_TABLE_BLK_NUM); PhyBlk++) {
			ercd = write_partition_table(PhyBlk);
			if (ercd == 0) {
				break;
			}
		}
		if (ercd != 0) {
			return NAND_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR0;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = PhyBlk;
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = gPtableBlockTable[PARTITION_TABLE_AREA_1];
	}

	// retry_count
	if(BootParameter.retry_count == 0x0) {
		max_count = (PARA_START_BLK_MAIN + 1);
	}
	else {
		max_count = (PARA_START_BLK_MAIN + BootParameter.retry_count);
	}

	printf("**** START **** write_boot_header. \n");
	// Prameter block write
	for(PhyBlk = PARA_START_BLK_MAIN; PhyBlk < max_count; PhyBlk++) {
		ercd = write_boot_header(PhyBlk);
		//Ecc,skip
		MWNF_Set_Ecc_Param( (gNANDFlashParameter.ecc_config & 0x00000001),  ( (gNANDFlashParameter.ecc_config & 0x00000700) >> 8) );
		MWNF_Set_Skip_Bytes((gNANDFlashParameter.skip_bytes_conf & 0xFF));
		if (ercd != 0) {
			return NAND_PARA_VERIFY_ERR;
		}
	}

	// partition table(memory)
	memcpy((void*)gNF_partition_table, (void*)NF_partition_table, sizeof(gNF_partition_table));
	// partition table block
	gPtableBlockTable[PARTITION_TABLE_SELECTOR] = PtableBlockTable[PARTITION_TABLE_SELECTOR];
	gPtableBlockTable[PARTITION_TABLE_AREA_0]   = PtableBlockTable[PARTITION_TABLE_AREA_0];
	gPtableBlockTable[PARTITION_TABLE_AREA_1]   = PtableBlockTable[PARTITION_TABLE_AREA_1];

	return WRITE_SUCCESS;
}


static int updater_main_emmc(int* fileid, unsigned long size, unsigned char* buff)
{
	int ercd = 0;
	T_BF_UPDATER_DATA_HEAD *data_head;
	unsigned long length;
// --- REMOVE_SECURE BEGIN ---
	T_HASH_DATA   hash_data[4];
	unsigned long hash_data_cnt=0;
	unsigned long result;				// result of reading the device
	unsigned char id;
// --- REMOVE_SECURE END ---

	// make partition table  PartitionParameter
	make_partition_table_em();

	// Change AccessPartition( boot_header, boot_image )
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_1);
	if( ercd != 0 ) {
		BF_Debug_Print_Error(("_____Err Set_AccessPartition(EM_ACCCESS_BOOT_1) ret=%d, in updater_main_emmc()  [L:%d] \n",ercd,__LINE__));
		return -1;
	}
	// read parameter
	ercd = read_parameter_em();
	if ( ercd !=0 ){
		return -1;
	}
	
	// Change AccessPartition( UserData(PARTITION_TABLE,PARTITION_DATA[x]) )
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_NONE);
	if( ercd != 0 ) {
		BF_Debug_Print_Error(("_____Err Set_AccessPartition(EM_ACCCESS_BOOT_NONE) ret=%d, in updater_main_emmc()  [L:%d] \n",ercd,__LINE__));
		return -1;
	}
// new spec start
	// Update file read
	ercd = BF_Fs_If_Read(*fileid, buff, size, &length);
	if (ercd != 0) {
		BF_Debug_Print_Error(("_____Err BF_Fs_If_Read ret=%d, in updater_main_emmc(data_head)  [L:%d] \n",ercd,__LINE__));
		return -1;
	}
	data_head = (T_BF_UPDATER_DATA_HEAD*)buff;

// --- REMOVE_SECURE BEGIN ---
	// Secure Check
	if ((data_head->type == PARTITION_TYPE_SECURE_PARAM )&& (data_head->param == 0xFF) && (g_secure_update_enable)){
		{
			FW_INT32 file_id;
			unsigned long secure_bin_size = 32;
			unsigned char* secure_bin_buff = (unsigned char*)(RAMCHECK_AREA + 0xFFF00);
			unsigned char i;
					
			file_id = BF_Fs_If_Open((const FW_CHAR*)g_secure_binary_filename, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
			if (file_id == 0){
				BF_Debug_Print_Error(("[Updater]BF_Fs_If_Open:%s err=%d\n" ,g_secure_binary_filename, file_id));
				return -1;
			}
			ercd = BF_Fs_If_Read(file_id, secure_bin_buff, secure_bin_size, &length);
			if (ercd != 0) {
				BF_Debug_Print_Error(("_____Err BF_Fs_If_Read ret=%d, in updater_main_emmc(secure binary)  [L:%d] \n",ercd,__LINE__));
				BF_Fs_If_Close(file_id);
				return -1;
			}
			BF_Fs_If_Close(file_id);
		
			for(i = 0; i < secure_bin_size; i+=4)
			{
//				BF_Debug_Print_Information(("Binary Compare[%d] 0x%08lx vs 0x%08lx\n"
//					, i
//					, *(unsigned long *)(buff + sizeof(T_BF_UPDATER_DATA_HEAD) + i)
//					, *(unsigned long *)(secure_bin_buff + i)));
				if(*(unsigned long *)(buff + sizeof(T_BF_UPDATER_DATA_HEAD) + i) != *(unsigned long *)(secure_bin_buff + i)){
					BF_Debug_Print_Error(("_____Err secure binary compare fail [%d] 0x%08lx vs 0x%08lx, in updater_main_emmc  [L:%d] \n"
						, i
						, *(unsigned long *)(buff + sizeof(T_BF_UPDATER_DATA_HEAD) + i)
						, *(unsigned long *)(secure_bin_buff + i)
						, __LINE__));
					return -1;
				}
			}
		}
				
		if ( size > (sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen) ){
			size  -= ( sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen );
		}
		else{
			BF_Debug_Print_Error(("_____Err Size NG. file size=%ld, datalen=%ld, in updater_main_emmc() [L:%d] \n",size, data_head->datalen,__LINE__));
			return -1;
		}
		// Set DATA.bin
		memset(hash_data, 0, sizeof(hash_data) );
		hash_data[hash_data_cnt].addr = (unsigned long)(buff + sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen)+0x20000000;;
		hash_data[hash_data_cnt].size = size;
		hash_data_cnt++;
		memcpy((void*)RAMCHECK_AREA, (const void*)hash_data, sizeof(hash_data) );


		// Pre Procedure of IPCU cmd
		BF_Debug_Print_Information(("IPCU_OPEN \n" ));
		ercd = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_14, &id);
		// Check IPCU cmd
		if ( ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(("_____Err IPCU NG, IPCU_OPEN ercd=%d, in updater_main_emmc [L:%d] \n",ercd,__LINE__));
			return -1;
		}
//		BF_Debug_Print_Information(("IPCU d_add=0x%08lx \n", (unsigned long)(buff + sizeof(T_BF_UPDATER_DATA_HEAD))+0x20000000 ));
//		BF_Debug_Print_Information(("IPCU add0 =0x%08lx \n", hash_data[0].addr   ));
//		BF_Debug_Print_Information(("IPCU hash =0x%08lx \n", (unsigned long)(void*)(RAMCHECK_AREA + 0x20000000)   ));
//		BF_Debug_Print_Information(("IPCU cnt  =0x%08lx \n", hash_data_cnt  ));

		BF_Debug_Print_Information(("Send IPCU message to CM0(forSecureCheck..) \n" ));
		//  +0x20000000:The difference of SDRAM address of the CA7 and CM0.
		ercd = FJ_IPCU_Check_Secure_Code_CM0( (ULONG)(buff + sizeof(T_BF_UPDATER_DATA_HEAD))+0x20000000, (ULONG)RAMCHECK_AREA+0x20000000, hash_data_cnt, &result );		/* pgr0539 */
		BF_Debug_Print_Information(("return IPCU message(forSecureCheck) \n" ));
		FJ_IPCU_Close(E_FJ_IPCU_MAILBOX_TYPE_14);
		BF_Debug_Print_Information(("IPCU_Close \n" ));
		// Check IPCU cmd
		if ( ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(("_____Err IPCU NG, IPCU_Close ercd=%d, in updater_main_emmc [L:%d] \n",ercd,__LINE__));
			return -1;
		}


		// Check HashCheck result
		if ( result !=0 ) {
			BF_Debug_Print_Error(("_____Err HashCheck result NG (result=%ld), in updater_main_emmc  [L:%d] \n",result,__LINE__));
			return -1;
		}
		BF_Debug_Print_Information(("HashCheck result PASS, in updater_main_emmc	[L:%d] \n", __LINE__));
		buff += sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen;
	}
	else{
		// normal
		if(g_secure_update_enable){
			BF_Debug_Print_Error(("_____Err SecureParameter read error, in updater_main_emmc  [L:%d] \n", __LINE__));
			return -1;
		}
	}
// --- REMOVE_SECURE END ---
// new spec end

	while (size != 0) {
		if (sizeof(T_BF_UPDATER_DATA_HEAD) > size) {
			size = 0;
		}
		else {
			size -= sizeof(T_BF_UPDATER_DATA_HEAD);
		}
		// Add Header size [new_spec]
		data_head = (T_BF_UPDATER_DATA_HEAD*)buff;
		buff += sizeof(T_BF_UPDATER_DATA_HEAD);

		// signature check
		if (strncmp((const char*)data_head->signature, "MILBEAUT", 8) != 0) {
			BF_Debug_Print_Error(("_____Err signature in updater_main_emmc  [L:%d] \n",__LINE__));
			return -1;
		}

		switch (data_head->type) {
			case PARTITION_TYPE_NAND_PARAM:
				ercd = get_nand_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_SDRAM_PARAM:
				ercd = get_sdram_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_PARTITION_PARAM:
				ercd = get_partition_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_BOOT_PARAM:
				ercd = get_boot_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_BOOTSTRAP :
				// boot image
				memcpy((void*)FLASHDATA_SRAM_AREA, (void*)buff, data_head->datalen);
				if(BootParameter.image_size != data_head->datalen) {
					BootParameter.image_size = data_head->datalen;
				}
				break;

// --- REMOVE_SECURE BEGIN ---
			case PARTITION_TYPE_SECURE_PARAM :
				if (data_head->param == PARTITION_PARAMETER_MDgst){
					// Check digest_offset of boot_image(at Writer)
					if ( BootParameter.dg_offset == 0){
						BF_Debug_Print_Error(("_____Err When written in the beginning, message_digest of boot_image was not. \n" ));
						BF_Debug_Print_Error(("_____Err boot_images that were written in the non-secure, can not be updated in a secure. [L:%d] \n", __LINE__));
						return -1;
					}
					// Update the corresponding data_head->param of the Secure_param
					memcpy((void*)SecureParameter.image_digest.data, (void*)buff,  data_head->datalen );
				}
				else if (data_head->param < PARTITION_NUM){
					// Update the corresponding data_head->param of the Secure_param
					memcpy((void*)SecureParameter.ms_dg[data_head->param].data, (void*)buff,  data_head->datalen );
				}
				else{
					BF_Debug_Print_Error(("_____Err data_head->param over =%d, in updater_main_emmc(SECURE_PARAM)  [L:%d] \n",data_head->param,__LINE__));
					return GETPAR_ERR;
				}
				break;
// --- REMOVE_SECURE END ---

			case PARTITION_TYPE_PARTITION_DATA :
				if (data_head->param >= PARTITION_NUM) {
					BF_Debug_Print_Error(("_____Err data_head->param over =%d, in updater_main_emmc(PARTITION_DATA)  [L:%d] \n",data_head->param,__LINE__));
					return GETPAR_ERR;
				}
				
				g_bf_updater_partition_flag = 1;
				g_bf_updater_partition_no   = data_head->param;
				ercd = updater_partition_data_em(data_head->param, data_head->datalen, (unsigned long)buff);
				g_bf_updater_partition_flag = 0;
				break;

			default:
				break;
		}

		if (ercd != 0) {
			switch (data_head->type) {
				case PARTITION_TYPE_NAND_PARAM:
				case PARTITION_TYPE_SDRAM_PARAM:
				case PARTITION_TYPE_PARTITION_PARAM:
					return GETPAR_ERR;
				default:
					return EMMC_DATA_WRITE_ERR;
			}
		}

		if (data_head->datalen > size) {
			size = 0;
		}
		else {
			size -= data_head->datalen;
			buff += data_head->datalen;
		}
	}

	printf("**** START **** Partition table write. \n");
	// Partition table write
	if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 0) {
		ercd = write_partition_table_em(PARTITION_TABLE_START_SECTOR1);
		if (ercd != 0) {
			BF_Debug_Print_Error(("_____Err write_partition_table_em(START_SECTOR1)  ret=%d  [L:%d] \n",ercd,__LINE__));
			return EMMC_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR1;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = gPtableBlockTable[PARTITION_TABLE_AREA_0];
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = PARTITION_TABLE_START_SECTOR1;
	}
	else {
		ercd = write_partition_table_em(PARTITION_TABLE_START_SECTOR0);
		if (ercd != 0) {
			BF_Debug_Print_Error(("_____Err write_partition_table_em(START_SECTOR1)  ret=%d  [L:%d] \n",ercd,__LINE__));
			return EMMC_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR0;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = PARTITION_TABLE_START_SECTOR0;
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = gPtableBlockTable[PARTITION_TABLE_AREA_1];
	}

	printf("**** START **** write_boot_header. \n");
	// Access partition area : boot partition1
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_1);
	if( ercd != 0 ) {
		BF_Debug_Print_Error(("_____Err Set_AccessPartition(EM_ACCCESS_BOOT_1) ret=%d, in updater_main_emmc()  [L:%d] \n",ercd,__LINE__));
		return -1;
	}
	// Prameter block write     0
	ercd = write_boot_header_em( PARA_START_SEC_MAIN );
	if (ercd != 0) {
		BF_Debug_Print_Error(("_____Err write_boot_header_em(write_boot_header_em)  ret=%d  [L:%d] \n",ercd,__LINE__));
		return NAND_PARA_VERIFY_ERR;
	}

	// partition table(memory)
	memcpy((void*)gNF_partition_table, (void*)NF_partition_table, sizeof(gNF_partition_table));
	// partition table block
	gPtableBlockTable[PARTITION_TABLE_SELECTOR] = PtableBlockTable[PARTITION_TABLE_SELECTOR];
	gPtableBlockTable[PARTITION_TABLE_AREA_0]   = PtableBlockTable[PARTITION_TABLE_AREA_0];
	gPtableBlockTable[PARTITION_TABLE_AREA_1]   = PtableBlockTable[PARTITION_TABLE_AREA_1];

	return WRITE_SUCCESS;
}


static int updater_memory_main_nand(unsigned char area, unsigned int *buff, unsigned int size)
{
	int ercd = 0;
	unsigned long PhyBlk;
	unsigned long max_count;

	// parameter check(memory)
	if (area >= PARTITION_NUM) {
		return GETPAR_ERR;
	}

	BF_L1l2cache_Clean_Flush_Addr((UINT32) buff, size);

	MWNF_Get_Dev_Info((T_NF_DEV_INFO*)&NandInfo);

	// make partition table(PartitionParameter...)
	make_partition_table();

	// read parameter
	ercd = read_parameter(PARA_START_BLK_MAIN);
	if ( ercd !=0 ){
		return -1;
	}


	// update partition data(memory)
	g_bf_updater_partition_flag = 1;
	g_bf_updater_partition_no   = area;
	ercd = updater_partition_data(area, size, (unsigned long)buff);
	g_bf_updater_partition_flag = 0;
	if (ercd != 0) {
		return NAND_DATA_WRITE_ERR;
	}

	printf("**** START **** Partition table write. \n");
	// Partition table write
	if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 0) {
		for (PhyBlk = PARTITION_TABLE_START_BLK1; PhyBlk < (PARTITION_TABLE_START_BLK1 + PARTITION_TABLE_BLK_NUM); PhyBlk++) {
			ercd = write_partition_table(PhyBlk);
			if (ercd == 0) {
				break;
			}
		}
		if (ercd != 0) {
			return NAND_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR1;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = gPtableBlockTable[PARTITION_TABLE_AREA_0];
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = PhyBlk;
	}
	else {
		for (PhyBlk = PARTITION_TABLE_START_BLK0; PhyBlk < (PARTITION_TABLE_START_BLK0 + PARTITION_TABLE_BLK_NUM); PhyBlk++) {
			ercd = write_partition_table(PhyBlk);
			if (ercd == 0) {
				break;
			}
		}
		if (ercd != 0) {
			return NAND_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR0;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = PhyBlk;
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = gPtableBlockTable[PARTITION_TABLE_AREA_1];
	}

	// retry_count
	if(BootParameter.retry_count == 0x0) {
		max_count = (PARA_START_BLK_MAIN + 1);
	}
	else {
		max_count = (PARA_START_BLK_MAIN + BootParameter.retry_count);
	}

	printf("**** START **** write_boot_header. \n");
	// Prameter block write     0
	for(PhyBlk = PARA_START_BLK_MAIN; PhyBlk < max_count; PhyBlk++) {
		ercd = write_boot_header(PhyBlk);
		//Ecc,skip
		MWNF_Set_Ecc_Param( (gNANDFlashParameter.ecc_config & 0x00000001),  ( (gNANDFlashParameter.ecc_config & 0x00000700) >> 8) );
		MWNF_Set_Skip_Bytes((gNANDFlashParameter.skip_bytes_conf & 0xFF));
		if (ercd != 0) {
			return NAND_PARA_VERIFY_ERR;
		}
	}


	// partition table(memory)
	memcpy((void*)gNF_partition_table, (void*)NF_partition_table, sizeof(gNF_partition_table));
	// partition table block
	gPtableBlockTable[PARTITION_TABLE_SELECTOR] = PtableBlockTable[PARTITION_TABLE_SELECTOR];
	gPtableBlockTable[PARTITION_TABLE_AREA_0]   = PtableBlockTable[PARTITION_TABLE_AREA_0];
	gPtableBlockTable[PARTITION_TABLE_AREA_1]   = PtableBlockTable[PARTITION_TABLE_AREA_1];


	return WRITE_SUCCESS;
}


static int updater_memory_main_emmc(unsigned char area, unsigned int *buff, unsigned int size)
{
	int ercd = 0;

	// parameter check
	if (area >= PARTITION_NUM) {
		return GETPAR_ERR;
	}

	BF_L1l2cache_Clean_Flush_Addr((UINT32) buff, size);

	// make partition table  PartitionParameter
	make_partition_table_em();

	// Change AccessPartition( boot_header, boot_image )
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_1);
	if( ercd != 0 ) {
		BF_Debug_Print_Error(("_____Err Set_AccessPartition(EM_ACCCESS_BOOT_1) ret=%d, in updater_main_emmc()  [L:%d] \n",ercd,__LINE__));
		return -1;
	}
	// read parameter
	ercd = read_parameter_em();
	if ( ercd !=0 ){
		return -1;
	}
	
	// Change AccessPartition( UserData(PARTITION_TABLE,PARTITION_DATA[x]) )
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_NONE);
	if( ercd != 0 ) {
		BF_Debug_Print_Error(("_____Err Set_AccessPartition(EM_ACCCESS_BOOT_NONE) ret=%d, in updater_main_emmc()  [L:%d] \n",ercd,__LINE__));
		return -1;
	}


	// update partition data(memory)
	g_bf_updater_partition_flag = 1;
	g_bf_updater_partition_no   = area;
	ercd = updater_partition_data_em(area, size, (unsigned long)buff);
	g_bf_updater_partition_flag = 0;
	if (ercd != 0) {
		return NAND_DATA_WRITE_ERR;
	}

	printf("**** START **** Partition table write. \n");
	// Partition table write
	if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 0) {
		ercd = write_partition_table_em(PARTITION_TABLE_START_SECTOR1);
		if (ercd != 0) {
			BF_Debug_Print_Error(("_____Err write_partition_table_em(START_SECTOR1)  ret=%d  [L:%d] \n",ercd,__LINE__));
			return EMMC_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR1;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = gPtableBlockTable[PARTITION_TABLE_AREA_0];
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = PARTITION_TABLE_START_SECTOR1;
	}
	else {
		ercd = write_partition_table_em(PARTITION_TABLE_START_SECTOR0);
		if (ercd != 0) {
			BF_Debug_Print_Error(("_____Err write_partition_table_em(START_SECTOR1)  ret=%d  [L:%d] \n",ercd,__LINE__));
			return EMMC_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR0;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = PARTITION_TABLE_START_SECTOR0;
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = gPtableBlockTable[PARTITION_TABLE_AREA_1];
	}


	printf("**** START **** write_boot_header_emmc. \n");
	// Access partition area : boot partition1
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_1);
	if( ercd != 0 ) {
		BF_Debug_Print_Error(("_____Err Set_AccessPartition(EM_ACCCESS_BOOT_1) ret=%d, in updater_main_emmc()  [L:%d] \n",ercd,__LINE__));
		return -1;
	}
	// Prameter block write     0
	ercd = write_boot_header_em( PARA_START_SEC_MAIN );
	if (ercd != 0) {
		BF_Debug_Print_Error(("_____Err write_boot_header_em(write_boot_header_em)  ret=%d  [L:%d] \n",ercd,__LINE__));
		return NAND_PARA_VERIFY_ERR;
	}

	// partition table(memory)
	memcpy((void*)gNF_partition_table, (void*)NF_partition_table, sizeof(gNF_partition_table));
	// partition table block
	gPtableBlockTable[PARTITION_TABLE_SELECTOR] = PtableBlockTable[PARTITION_TABLE_SELECTOR];
	gPtableBlockTable[PARTITION_TABLE_AREA_0]   = PtableBlockTable[PARTITION_TABLE_AREA_0];
	gPtableBlockTable[PARTITION_TABLE_AREA_1]   = PtableBlockTable[PARTITION_TABLE_AREA_1];


	return WRITE_SUCCESS;
}


static int updater_selector_main_nand(VOID)
{
	int ercd = 0;
	boot_parameter *p_table;

	MWNF_Get_Dev_Info((T_NF_DEV_INFO*)&NandInfo);

	// set ecc correction
	MWNF_Set_Ecc_Param(D_MWNF_ENABLE, D_MWNF_ECC_CAP16);
	MWNF_Set_Skip_Bytes(0);

	while(1) {
		//read nand.par from NandFlash device bank0,block0,page0
		ercd = MWNF_ReadPages(0, 0, NandInfo.page_per_blk, (BYTE*)FLASHDATA_SRAM_AREA);
		if (ercd != D_MWNF_OK) {
			BF_Debug_Print_Error(("_____Err MWNF_ReadPages ret=%d,  read_parameter in \n",ercd));
			break;
		}

		BF_Debug_Print_Information(("now selector = %d\n", gPtableBlockTable[PARTITION_TABLE_SELECTOR]));

		// check selector
		if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 0) {
			if ((gPtableBlockTable[PARTITION_TABLE_AREA_1] != 0) && (gPtableBlockTable[PARTITION_TABLE_AREA_1] != 0xFFFF)) {
				PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR1;
			}
			else {
				BF_Debug_Print_Information(("selector 1 is not flashed.\n"));
				ercd = -1;
				break;
			}
		}
		else {
			PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR0;
		}
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = gPtableBlockTable[PARTITION_TABLE_AREA_0];
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = gPtableBlockTable[PARTITION_TABLE_AREA_1];
		BF_Debug_Print_Information(("new selector = %d\n", PtableBlockTable[PARTITION_TABLE_SELECTOR]));

		p_table = (boot_parameter*)FLASHDATA_SRAM_AREA;

		p_table->ptable_sel[0] = PtableBlockTable[0];
		p_table->ptable_sel[1] = PtableBlockTable[1];
		p_table->ptable_sel[2] = PtableBlockTable[2];

		// update boot header
		ercd = MWNF_EraseBlk(0);
		if (ercd != 0) {
			BF_Debug_Print_Information(("erase error\n"));
			break;
		}
		ercd = MWNF_WritePages_NoRetry( 0, 0, NandInfo.page_per_blk, (BYTE*)FLASHDATA_SRAM_AREA); //writes parameter to block0 page 0
		if ( ercd == 0 ) {
			ercd = MWNF_ReadPages( 0, 0, NandInfo.page_per_blk, (BYTE*)RAMCHECK_AREA);
			if (ercd == 0) {
				ercd = ul_memcmp((unsigned long *)FLASHDATA_SRAM_AREA, (unsigned long *)RAMCHECK_AREA, NandInfo.page_per_blk);
			}
			else {
				BF_Debug_Print_Information(("read error\n"));
			}
		}
		else {
			BF_Debug_Print_Information(("write error\n"));
		}
		break;
	}
	//Ecc,skip
	MWNF_Set_Ecc_Param( (gNANDFlashParameter.ecc_config & 0x00000001),  ( (gNANDFlashParameter.ecc_config & 0x00000700) >> 8) );
	MWNF_Set_Skip_Bytes((gNANDFlashParameter.skip_bytes_conf & 0xFF));

	if (ercd != 0) {
		BF_Debug_Print_Information(("verify error\n"));
		return NAND_PARA_VERIFY_ERR;
	}

	// save selector
	gPtableBlockTable[PARTITION_TABLE_SELECTOR] = PtableBlockTable[PARTITION_TABLE_SELECTOR];

	return WRITE_SUCCESS;
}


static int updater_selector_main_emmc(VOID)
{
	int ercd = 0;
	boot_parameter *p_table;

	// Change AccessPartition( boot_header, boot_image )
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_1);
	if( ercd != 0 ) {
		BF_Debug_Print_Error(("_____Err Set_AccessPartition(EM_ACCCESS_BOOT_1) ret=%d, in updater_main_emmc()  [L:%d] \n",ercd,__LINE__));
		return -1;
	}

	while(1) {
		// read parameter
		ercd = MWEM_DirectRead( 0, 2, (CHAR*)FLASHDATA_SRAM_AREA);
		if (ercd != D_MWNF_OK) {
			BF_Debug_Print_Error(("_____Err MWEM_DirectRead ret=%d,  read_parameter_em in \n",ercd));
			break;
		}
		
		BF_Debug_Print_Information(("now selector = %d\n", gPtableBlockTable[PARTITION_TABLE_SELECTOR]));

		// check selector
		if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 0) {
			if ((gPtableBlockTable[PARTITION_TABLE_AREA_1] != 0) && (gPtableBlockTable[PARTITION_TABLE_AREA_1] != 0xFFFF)) {
				PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR1;
			}
			else {
				BF_Debug_Print_Information(("selector 1 is not flashed.\n"));
				ercd = -1;
				break;
			}
		}
		else {
			PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR0;
		}
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = gPtableBlockTable[PARTITION_TABLE_AREA_0];
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = gPtableBlockTable[PARTITION_TABLE_AREA_1];
		BF_Debug_Print_Information(("new selector = %d\n", PtableBlockTable[PARTITION_TABLE_SELECTOR]));

		p_table = (boot_parameter*)FLASHDATA_SRAM_AREA;

		p_table->ptable_sel[0] = PtableBlockTable[0];
		p_table->ptable_sel[1] = PtableBlockTable[1];
		p_table->ptable_sel[2] = PtableBlockTable[2];

		// update boot header
		ercd = MWEM_DirectWrite( 0, 2, (char*)FLASHDATA_SRAM_AREA);
		if (ercd != 0) {
			BF_Debug_Print_Error(("_____Err MWEM_DirectWrite (eMMC:boot header) ret=%d [L:%d] \n", ercd, __LINE__));
		}
		break;
	}

	// Change AccessPartition
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_NONE);
	if( ercd != 0 ) {
		BF_Debug_Print_Error(("_____Err Set_AccessPartition(EM_ACCCESS_BOOT_NONE) ret=%d, in updater_main_emmc()  [L:%d] \n",ercd,__LINE__));
		return -1;
	}

	// save selector
	gPtableBlockTable[PARTITION_TABLE_SELECTOR] = PtableBlockTable[PARTITION_TABLE_SELECTOR];

	return WRITE_SUCCESS;
}

static int updater_memory_all_main_nand(unsigned char* buff, unsigned long size)
{
	int ercd = 0;
	T_BF_UPDATER_DATA_HEAD *data_head;
	unsigned long PhyBlk;
	unsigned long max_count;
//// --- REMOVE_SECURE BEGIN ---
//	T_HASH_DATA   hash_data[4];
//	unsigned long hash_data_cnt=0;
//	unsigned long result;
//	unsigned char id;
//// --- REMOVE_SECURE END ---

	BF_L1l2cache_Clean_Flush_Addr((UINT32) buff, size);

	MWNF_Get_Dev_Info((T_NF_DEV_INFO*)&NandInfo);

	// make partition table(PartitionParameter...)
	make_partition_table();

	// read parameter
	ercd = read_parameter(PARA_START_BLK_MAIN);
	if ( ercd !=0 ){
		return -1;
	}

	data_head = (T_BF_UPDATER_DATA_HEAD*)buff;

// --- REMOVE_SECURE BEGIN ---
//	// Secure Check
//	if ((data_head->type == PARTITION_TYPE_SECURE_PARAM )&& (data_head->param == 0xFF) ){
//		if ( size > (sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen) ){
//			size  -= ( sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen );
//		}
//		else{
//			BF_Debug_Print_Error(("_____Err Size NG. file size=%ld, datalen=%ld, in updater_main_nand() [L:%d] \n",size, data_head->datalen,__LINE__));
//			return -1;
//		}
//		// Set top address and size of DATA.bin 
//		memset(hash_data, 0, sizeof(hash_data));
//		hash_data[hash_data_cnt].addr = (unsigned long)(buff + sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen)+0x20000000;
//		hash_data[hash_data_cnt].size = size;
//		hash_data_cnt++;
//		memcpy((void*)RAMCHECK_AREA, (const void*)hash_data, sizeof(hash_data));
//
//
//		// Pre Procedure of IPCU cmd
//		BF_Debug_Print_Information(("IPCU_OPEN\n" ));
//		ercd = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_14, &id);
//		// Check IPCU cmd
//		if ( ercd != FJ_ERR_OK ) {
//			BF_Debug_Print_Error(("_____Err IPCU NG, IPCU_OPEN ercd=%d, in updater_main_nand [L:%d] \n",ercd,__LINE__));
//			return -1;
//		}
//		ercd = FJ_IPCU_SetReceiverCB(id, NULL);
//		// Check IPCU cmd
//		if ( ercd != FJ_ERR_OK ) {
//			BF_Debug_Print_Error(("_____Err IPCU NG, SetReceiverCB ercd=%d, in updater_main_nand [L:%d] \n",ercd,__LINE__));
//			FJ_IPCU_Close( E_FJ_IPCU_MAILBOX_TYPE_14 );
//			return -1;
//		}
////		BF_Debug_Print_Information(("IPCU d_add=0x%08x \n", (buff + sizeof(T_BF_UPDATER_DATA_HEAD))+0x20000000 ));
////		BF_Debug_Print_Information(("IPCU add0 =0x%08x \n", hash_data[0].addr   ));
////		BF_Debug_Print_Information(("IPCU hash =0x%08x \n", (void*)(RAMCHECK_AREA + 0x20000000)   ));
////		BF_Debug_Print_Information(("IPCU cnt  =0x%08x \n", hash_data_cnt  ));
//
//		BF_Debug_Print_Information(("Send IPCU message to CM0(forSecureCheck..) \n" ));
//		//  +0x20000000:The difference of SDRAM address of the CA7 and CM0.
//		ercd = FJ_IPCU_Check_Secure_Code_CM0( (ULONG)(buff + sizeof(T_BF_UPDATER_DATA_HEAD))+0x20000000, (ULONG)RAMCHECK_AREA+0x20000000, hash_data_cnt, &result );		/* pgr0539 */
//		BF_Debug_Print_Information(("return IPCU message(forSecureCheck) \n" ));
//		FJ_IPCU_Close(E_FJ_IPCU_MAILBOX_TYPE_14);
//		BF_Debug_Print_Information(("IPCU_Close \n" ));
//		// Check IPCU cmd
//		if ( ercd != FJ_ERR_OK ) {
//			BF_Debug_Print_Error(("_____Err IPCU NG, IPCU_Close ercd=%d, in updater_main_nand [L:%d] \n",ercd,__LINE__));
//			return -1;
//		}
//
//
//		// Check HashCheck result
//		if ( result !=0 ) {
//			BF_Debug_Print_Error(("_____Err HashCheck result NG (result=%d), in updater_main_nand  [L:%d] \n", result, __LINE__));
//			return -1;
//		}
//		// Secure OK. set DATA.bin top
//		buff += sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen;
//	}
//	else{
//		// normal
//	}
// --- REMOVE_SECURE END ---

	while (size != 0) {
		if (sizeof(T_BF_UPDATER_DATA_HEAD) > size) {
			size = 0;
		}
		else {
			size -= sizeof(T_BF_UPDATER_DATA_HEAD);
		}
		// Add Header size [new_spec]
		data_head = (T_BF_UPDATER_DATA_HEAD*)buff;
		buff += sizeof(T_BF_UPDATER_DATA_HEAD);
		
		// signature check
		if (strncmp((const char*)data_head->signature, "MILBEAUT", 8) != 0) {
			BF_Debug_Print_Error(("_____Err signature. size=%ld.   [L:%d] \n", size, __LINE__));
			return -1;
		}

		switch (data_head->type) {
			case PARTITION_TYPE_NAND_PARAM:
				ercd = get_nand_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_SDRAM_PARAM:
				ercd = get_sdram_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_PARTITION_PARAM:
				ercd = get_partition_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_BOOT_PARAM:
				ercd = get_boot_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_BOOTSTRAP :
				// boot image
				memcpy((void*)FLASHDATA_SRAM_AREA, (void*)buff, data_head->datalen);
				if(BootParameter.image_size != data_head->datalen) {
					BootParameter.image_size = data_head->datalen;
				}
				break;

// --- REMOVE_SECURE BEGIN ---
//			case PARTITION_TYPE_SECURE_PARAM :
//
//				if (data_head->param == PARTITION_PARAMETER_MDgst){
//					// Check digest_offset of boot_image(at Writer)
//					if ( BootParameter.dg_offset == 0){
//						BF_Debug_Print_Error(("_____Err When written in the beginning, message_digest of boot_image was not. \n" ));
//						BF_Debug_Print_Error(("_____Err boot_images that were written in the non-secure, can not be updated in a secure. [L:%d] \n", __LINE__));
//						return -1;
//					}
//					// Update the corresponding data_head->param of the Secure_param
//					memcpy((void*)SecureParameter.image_digest.data, (void*)buff,  data_head->datalen );
//				}
//				else if (data_head->param < PARTITION_NUM){
//					// Update the corresponding data_head->param of the Secure_param
//					memcpy((void*)SecureParameter.ms_dg[data_head->param].data, (void*)buff,  data_head->datalen );
//				}
//				else{
//					BF_Debug_Print_Error(("_____Err data_head->param over =%d, in updater_main_nand(SECURE_PARAM)  [L:%d] \n",data_head->param,__LINE__));
//					return GETPAR_ERR;
//				}
//				break;
//
// --- REMOVE_SECURE END ---

			case PARTITION_TYPE_PARTITION_DATA :
				if (data_head->param >= PARTITION_NUM) {
					BF_Debug_Print_Error(("_____Err data_head->param over =%d, in updater_main_nand(PARTITION_DATA)  [L:%d] \n",data_head->param,__LINE__));
					return GETPAR_ERR;
				}
				

				g_bf_updater_partition_flag = 1;
				g_bf_updater_partition_no   = data_head->param;
				ercd = updater_partition_data(data_head->param, data_head->datalen, (unsigned long)buff);
				g_bf_updater_partition_flag = 0;
				break;

			default:
				break;
		}

		if (ercd != 0) {
			switch (data_head->type) {
				case PARTITION_TYPE_NAND_PARAM:
				case PARTITION_TYPE_SDRAM_PARAM:
				case PARTITION_TYPE_PARTITION_PARAM:
					return GETPAR_ERR;
				default:
					return NAND_DATA_WRITE_ERR;
			}
		}

		if (data_head->datalen > size) {
			size = 0;
		}
		else {
			size -= data_head->datalen;
			buff += data_head->datalen;
		}
	}
	
	printf("**** START **** Partition table write(MemoryData). \n");
	// Partition table write
	if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 0) {
		for (PhyBlk = PARTITION_TABLE_START_BLK1; PhyBlk < (PARTITION_TABLE_START_BLK1 + PARTITION_TABLE_BLK_NUM); PhyBlk++) {
			ercd = write_partition_table(PhyBlk);
			if (ercd == 0) {
				break;
			}
		}
		if (ercd != 0) {
			return NAND_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR1;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = gPtableBlockTable[PARTITION_TABLE_AREA_0];
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = PhyBlk;
	}
	else {
		for (PhyBlk = PARTITION_TABLE_START_BLK0; PhyBlk < (PARTITION_TABLE_START_BLK0 + PARTITION_TABLE_BLK_NUM); PhyBlk++) {
			ercd = write_partition_table(PhyBlk);
			if (ercd == 0) {
				break;
			}
		}
		if (ercd != 0) {
			return NAND_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR0;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = PhyBlk;
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = gPtableBlockTable[PARTITION_TABLE_AREA_1];
	}

	// retry_count
	if(BootParameter.retry_count == 0x0) {
		max_count = (PARA_START_BLK_MAIN + 1);
	}
	else {
		max_count = (PARA_START_BLK_MAIN + BootParameter.retry_count);
	}

	printf("**** START **** write_boot_header. \n");
	// Prameter block write
	for(PhyBlk = PARA_START_BLK_MAIN; PhyBlk < max_count; PhyBlk++) {
		ercd = write_boot_header(PhyBlk);
		//Ecc,skip
		MWNF_Set_Ecc_Param( (gNANDFlashParameter.ecc_config & 0x00000001),  ( (gNANDFlashParameter.ecc_config & 0x00000700) >> 8) );
		MWNF_Set_Skip_Bytes((gNANDFlashParameter.skip_bytes_conf & 0xFF));
		if (ercd != 0) {
			return NAND_PARA_VERIFY_ERR;
		}
	}

	// partition table(memory)
	memcpy((void*)gNF_partition_table, (void*)NF_partition_table, sizeof(gNF_partition_table));
	// partition table block
	gPtableBlockTable[PARTITION_TABLE_SELECTOR] = PtableBlockTable[PARTITION_TABLE_SELECTOR];
	gPtableBlockTable[PARTITION_TABLE_AREA_0]   = PtableBlockTable[PARTITION_TABLE_AREA_0];
	gPtableBlockTable[PARTITION_TABLE_AREA_1]   = PtableBlockTable[PARTITION_TABLE_AREA_1];

	return WRITE_SUCCESS;
}


static int updater_memory_all_main_emmc(unsigned char* buff, unsigned long size)
{
#if 0
	int ercd = 0;
	T_BF_UPDATER_DATA_HEAD *data_head;
	unsigned long length;
// --- REMOVE_SECURE BEGIN ---
	T_HASH_DATA   hash_data[4];
	unsigned long hash_data_cnt=0;
	unsigned long result;				// result of reading the device
	unsigned char id;
// --- REMOVE_SECURE END ---

	// make partition table  PartitionParameter
	make_partition_table_em();

	// Change AccessPartition( boot_header, boot_image )
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_1);
	if( ercd != 0 ) {
		BF_Debug_Print_Error(("_____Err Set_AccessPartition(EM_ACCCESS_BOOT_1) ret=%d, in updater_main_emmc()  [L:%d] \n",ercd,__LINE__));
		return -1;
	}
	// read parameter
	ercd = read_parameter_em();
	if ( ercd !=0 ){
		return -1;
	}
	
	// Change AccessPartition( UserData(PARTITION_TABLE,PARTITION_DATA[x]) )
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_NONE);
	if( ercd != 0 ) {
		BF_Debug_Print_Error(("_____Err Set_AccessPartition(EM_ACCCESS_BOOT_NONE) ret=%d, in updater_main_emmc()  [L:%d] \n",ercd,__LINE__));
		return -1;
	}
// new spec start
	// Update file read
	ercd = BF_Fs_If_Read(*fileid, buff, size, &length);
	if (ercd != 0) {
		BF_Debug_Print_Error(("_____Err BF_Fs_If_Read ret=%d, in updater_main_emmc(data_head)  [L:%d] \n",ercd,__LINE__));
		return -1;
	}
	data_head = (T_BF_UPDATER_DATA_HEAD*)buff;

// --- REMOVE_SECURE BEGIN ---
	// Secure Check
	if ((data_head->type == PARTITION_TYPE_SECURE_PARAM )&& (data_head->param == 0xFF) ){
	
		if ( size > (sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen) ){
			size  -= ( sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen );
		}
		else{
			BF_Debug_Print_Error(("_____Err Size NG. file size=%ld, datalen=%ld, in updater_main_emmc() [L:%d] \n",size, data_head->datalen,__LINE__));
			return -1;
		}
		// Set DATA.bin
		memset(hash_data, 0, sizeof(hash_data) );
		hash_data[hash_data_cnt].addr = (unsigned long)(buff + sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen)+0x20000000;;
		hash_data[hash_data_cnt].size = size;
		hash_data_cnt++;
		memcpy((void*)RAMCHECK_AREA, (const void*)hash_data, sizeof(hash_data) );


		// Pre Procedure of IPCU cmd
		BF_Debug_Print_Information(("IPCU_OPEN \n" ));
		ercd = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_14, &id);
		// Check IPCU cmd
		if ( ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(("_____Err IPCU NG, IPCU_OPEN ercd=%d, in updater_main_emmc [L:%d] \n",ercd,__LINE__));
			return -1;
		}
		ercd = FJ_IPCU_SetReceiverCB(id, NULL);
		// Check IPCU cmd
		if ( ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(("_____Err IPCU NG, SetReceiverCB ercd=%d, in updater_main_emmc [L:%d] \n",ercd,__LINE__));
			FJ_IPCU_Close( E_FJ_IPCU_MAILBOX_TYPE_14 );
			return -1;
		}
//		BF_Debug_Print_Information(("IPCU d_add=0x%08x \n", (buff + sizeof(T_BF_UPDATER_DATA_HEAD))+0x20000000 ));
//		BF_Debug_Print_Information(("IPCU add0 =0x%08x \n", hash_data[0].addr   ));
//		BF_Debug_Print_Information(("IPCU hash =0x%08x \n", (void*)(RAMCHECK_AREA + 0x20000000)   ));
//		BF_Debug_Print_Information(("IPCU cnt  =0x%08x \n", hash_data_cnt  ));

		BF_Debug_Print_Information(("Send IPCU message to CM0(forSecureCheck..) \n" ));
		//  +0x20000000:The difference of SDRAM address of the CA7 and CM0.
		ercd = FJ_IPCU_Check_Secure_Code_CM0( (ULONG)(buff + sizeof(T_BF_UPDATER_DATA_HEAD))+0x20000000, (ULONG)RAMCHECK_AREA+0x20000000, hash_data_cnt, &result );		/* pgr0539 */
		BF_Debug_Print_Information(("return IPCU message(forSecureCheck) \n" ));
		FJ_IPCU_Close(E_FJ_IPCU_MAILBOX_TYPE_14);
		BF_Debug_Print_Information(("IPCU_Close \n" ));
		// Check IPCU cmd
		if ( ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(("_____Err IPCU NG, IPCU_Close ercd=%d, in updater_main_emmc [L:%d] \n",ercd,__LINE__));
			return -1;
		}


		// Check HashCheck result
		if ( result !=0 ) {
			BF_Debug_Print_Error(("_____Err HashCheck result NG (result=%d), in updater_main_emmc  [L:%d] \n",result,__LINE__));
			return -1;
		}
		buff += sizeof(T_BF_UPDATER_DATA_HEAD) + data_head->datalen;
	}
	else{
		// normal
	}
// --- REMOVE_SECURE END ---
// new spec end

	while (size != 0) {
		if (sizeof(T_BF_UPDATER_DATA_HEAD) > size) {
			size = 0;
		}
		else {
			size -= sizeof(T_BF_UPDATER_DATA_HEAD);
		}
		// Add Header size [new_spec]
		data_head = (T_BF_UPDATER_DATA_HEAD*)buff;
		buff += sizeof(T_BF_UPDATER_DATA_HEAD);

		// signature check
		if (strncmp((const char*)data_head->signature, "MILBEAUT", 8) != 0) {
			BF_Debug_Print_Error(("_____Err signature in updater_main_emmc  [L:%d] \n",__LINE__));
			return -1;
		}

		switch (data_head->type) {
			case PARTITION_TYPE_NAND_PARAM:
				ercd = get_nand_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_SDRAM_PARAM:
				ercd = get_sdram_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_PARTITION_PARAM:
				ercd = get_partition_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_BOOT_PARAM:
				ercd = get_boot_param(buff, data_head->datalen);
				break;
			case PARTITION_TYPE_BOOTSTRAP :
				// boot image
				memcpy((void*)FLASHDATA_SRAM_AREA, (void*)buff, data_head->datalen);
				if(BootParameter.image_size != data_head->datalen) {
					BootParameter.image_size = data_head->datalen;
				}
				break;

// --- REMOVE_SECURE BEGIN ---
			case PARTITION_TYPE_SECURE_PARAM :
				if (data_head->param == PARTITION_PARAMETER_MDgst){
					// Check digest_offset of boot_image(at Writer)
					if ( BootParameter.dg_offset == 0){
						BF_Debug_Print_Error(("_____Err When written in the beginning, message_digest of boot_image was not. \n" ));
						BF_Debug_Print_Error(("_____Err boot_images that were written in the non-secure, can not be updated in a secure. [L:%d] \n", __LINE__));
						return -1;
					}
					// Update the corresponding data_head->param of the Secure_param
					memcpy((void*)SecureParameter.image_digest.data, (void*)buff,  data_head->datalen );
				}
				else if (data_head->param < PARTITION_NUM){
					// Update the corresponding data_head->param of the Secure_param
					memcpy((void*)SecureParameter.ms_dg[data_head->param].data, (void*)buff,  data_head->datalen );
				}
				else{
					BF_Debug_Print_Error(("_____Err data_head->param over =%d, in updater_main_emmc(SECURE_PARAM)  [L:%d] \n",data_head->param,__LINE__));
					return GETPAR_ERR;
				}
				break;
// --- REMOVE_SECURE END ---

			case PARTITION_TYPE_PARTITION_DATA :
				if (data_head->param >= PARTITION_NUM) {
					BF_Debug_Print_Error(("_____Err data_head->param over =%d, in updater_main_emmc(PARTITION_DATA)  [L:%d] \n",data_head->param,__LINE__));
					return GETPAR_ERR;
				}
				
				g_bf_updater_partition_flag = 1;
				g_bf_updater_partition_no   = data_head->param;
				ercd = updater_partition_data_em(data_head->param, data_head->datalen, (unsigned long)buff);
				g_bf_updater_partition_flag = 0;
				break;

			default:
				break;
		}

		if (ercd != 0) {
			switch (data_head->type) {
				case PARTITION_TYPE_NAND_PARAM:
				case PARTITION_TYPE_SDRAM_PARAM:
				case PARTITION_TYPE_PARTITION_PARAM:
					return GETPAR_ERR;
				default:
					return EMMC_DATA_WRITE_ERR;
			}
		}

		if (data_head->datalen > size) {
			size = 0;
		}
		else {
			size -= data_head->datalen;
			buff += data_head->datalen;
		}
	}

	printf("**** START **** Partition table write. \n");
	// Partition table write
	if (gPtableBlockTable[PARTITION_TABLE_SELECTOR] == 0) {
		ercd = write_partition_table_em(PARTITION_TABLE_START_SECTOR1);
		if (ercd != 0) {
			BF_Debug_Print_Error(("_____Err write_partition_table_em(START_SECTOR1)  ret=%d  [L:%d] \n",ercd,__LINE__));
			return EMMC_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR1;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = gPtableBlockTable[PARTITION_TABLE_AREA_0];
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = PARTITION_TABLE_START_SECTOR1;
	}
	else {
		ercd = write_partition_table_em(PARTITION_TABLE_START_SECTOR0);
		if (ercd != 0) {
			BF_Debug_Print_Error(("_____Err write_partition_table_em(START_SECTOR1)  ret=%d  [L:%d] \n",ercd,__LINE__));
			return EMMC_DATA_WRITE_ERR;
		}
		PtableBlockTable[PARTITION_TABLE_SELECTOR] = PARTITION_TABLE_SELECTOR0;
		PtableBlockTable[PARTITION_TABLE_AREA_0]   = PARTITION_TABLE_START_SECTOR0;
		PtableBlockTable[PARTITION_TABLE_AREA_1]   = gPtableBlockTable[PARTITION_TABLE_AREA_1];
	}

	printf("**** START **** write_boot_header. \n");
	// Access partition area : boot partition1
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_1);
	if( ercd != 0 ) {
		BF_Debug_Print_Error(("_____Err Set_AccessPartition(EM_ACCCESS_BOOT_1) ret=%d, in updater_main_emmc()  [L:%d] \n",ercd,__LINE__));
		return -1;
	}
	// Prameter block write     0
	ercd = write_boot_header_em( PARA_START_SEC_MAIN );
	if (ercd != 0) {
		BF_Debug_Print_Error(("_____Err write_boot_header_em(write_boot_header_em)  ret=%d  [L:%d] \n",ercd,__LINE__));
		return NAND_PARA_VERIFY_ERR;
	}
#endif
	return WRITE_SUCCESS;
}


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		user update function(file)
 * @param		UCHAR* data : update file data
 * @return 		FJ_ERR_OK : Success
 * 				FJ_ERR_NG : Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_updater(UCHAR* filename)
{
	FW_INT32 ercd, file_id;
	T_BF_FS_IF_STAT stat;
	unsigned long *mdrAddr = ( unsigned long * )0x1DFF4000;

	if(g_secure_update_enable)
		printf("**** START **** FJ_Updater_SecureUpdate\n");
	else
		printf("**** START **** FJ_Updater\n");

	file_id = BF_Fs_If_Open((const FW_CHAR*)filename, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
	if (file_id == 0){
		BF_Debug_Print_Error(("[Updater]BF_Fs_If_Open:%s err=%d\n" ,filename, file_id));
		return FJ_ERR_NG;
	}

	ercd = BF_Fs_If_Fstat(file_id, &stat);
	if (ercd != 0) {
		BF_Fs_If_Close(file_id);
		BF_Debug_Print_Error(("[Updater]BF_Fs_If_Fstat:%s err=%d\n" ,filename, ercd));
		return FJ_ERR_NG;
	}
	// Check Mode pin (NAND or eMMC)
	g_MDR_value = (*mdrAddr) & 0x7;
	BF_Debug_Print_Information(("[Updater]Boot check:[ Boot Mode pin = 0x%lx ]\n", g_MDR_value));
	if ( g_MDR_value == 0 ) {
		// NAND Boot
		BF_Debug_Print_Information(("[Updater]Boot check:[ NAND boot ] \n"));
		ercd = updater_main_nand(&file_id, stat.fileSize, (unsigned char*)READ_FILE_DATA_AREA);
		if (ercd != 0){
			BF_Fs_If_Close(file_id);
			BF_Debug_Print_Error(("[Updater]updater_main_nand:%d\n", ercd));
			return FJ_ERR_NG;
		}
	}
	else if ( g_MDR_value == 1 ) {
		// eMMC Boot
		BF_Debug_Print_Information(("[Updater]Boot check:[ eMMC boot ] \n"));
		ercd = updater_main_emmc(&file_id, stat.fileSize, (unsigned char*)READ_FILE_DATA_AREA);
		if (ercd != 0){
			BF_Fs_If_Close(file_id);
			BF_Debug_Print_Error(("[Updater]updater_main_emmc:%d\n", ercd));
			return FJ_ERR_NG;
		}
	}
	else{
		BF_Fs_If_Close(file_id);
		BF_Debug_Print_Error(("[Updater]Boot check Err:[ Boot Mode pin = 0x%lx ]\n", g_MDR_value));
		return FJ_ERR_NG;
	}

	BF_Fs_If_Close(file_id);
	printf("**** COMPLETE **** FJ_Updater\n");
	return FJ_ERR_OK;
}

/**
 * @brief		user update function(memory)
 * @param		UCHAR area  : update partition area
 * 				UINT32* data_addr : update data address
 * 				UINT32 size : update file size
 * @return 		FJ_ERR_OK : Success
 * 				FJ_ERR_NG : Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_updater_memory(UCHAR area, UINT32 *data_addr, UINT32 size)
{
	FW_INT32 ercd;
	unsigned long *mdrAddr = ( unsigned long * )0x1DFF4000;
	
	if (size == 0){
		BF_Debug_Print_Error(("fj_updater_memory input size error(size=%d).\n", size ));
		return FJ_ERR_NG;
	}

	if(g_secure_update_enable)
		printf("**** START **** Not support secure update in %s\n", __func__);
	else
		printf("**** START **** FJ_Updater_Memory\n");

	// Check Mode pin (NAND or eMMC)
	g_MDR_value = (*mdrAddr) & 0x7;
	BF_Debug_Print_Information(("[Updater]Boot check:[ Boot Mode pin = 0x%lx ]\n", g_MDR_value));
	if ( g_MDR_value == 0 ) {
		BF_Debug_Print_Information(("[Updater]Boot check:[ NAND boot ] \n"));
		ercd = updater_memory_main_nand(area, data_addr, size);
		if (ercd != 0){
			BF_Debug_Print_Error(("[Updater]fj_updater_memory:%d\n", ercd));
			return FJ_ERR_NG;
		}
	}
	else if ( g_MDR_value == 1 ) {
		// eMMC Boot
		BF_Debug_Print_Information(("[Updater]Boot check:[ eMMC boot ] \n"));
		ercd = updater_memory_main_emmc(area, data_addr, size);
		if (ercd != 0){
			BF_Debug_Print_Error(("[Updater]fj_updater_memory:%d\n", ercd));
			return FJ_ERR_NG;
		}
	}
	else{
		BF_Debug_Print_Error(("[Updater]Boot check Err:[ Boot Mode pin = 0x%lx ]\n", g_MDR_value));
		return FJ_ERR_NG;
	}
	
	printf("**** COMPLETE **** FJ_Updater_Memory\n");
	return FJ_ERR_OK;
}

/**
 * @brief		user update function(memory)
 * @param		UINT32* data_addr : update data address
 * 				UINT32 size : update file size
 * @return 		FJ_ERR_OK : Success
 * 				FJ_ERR_NG : Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_updater_memory_all( UINT32 *data_addr, UINT32 size)
{
	FW_INT32 ercd;
	unsigned long *mdrAddr = ( unsigned long * )0x1DFF4000;

	if (size == 0){
		BF_Debug_Print_Error(("fj_updater_memory_all input size error(size=%d).\n", size ));
		return FJ_ERR_NG;
	}

	if(g_secure_update_enable)
		printf("**** START **** Not support secure update in %s\n", __func__);
	else
		printf("**** START **** FJ_Updater_Memory_All\n");

	// Check Mode pin (NAND or eMMC)
	g_MDR_value = (*mdrAddr) & 0x7;
	BF_Debug_Print_Information(("[Updater]Boot check:[ Boot Mode pin = 0x%lx ]\n", g_MDR_value));
	if ( g_MDR_value == 0 ) {
		BF_Debug_Print_Information(("[Updater]Boot check:[ NAND boot ] \n"));
		ercd = updater_memory_all_main_nand((unsigned char*)data_addr, size);
		if (ercd != 0){
			BF_Debug_Print_Error(("[Updater]fj_updater_memory_all:%d\n", ercd));
			return FJ_ERR_NG;
		}
	}
	else if ( g_MDR_value == 1 ) {
		// eMMC Boot
		BF_Debug_Print_Information(("[Updater]Boot check:[ eMMC boot ] \n"));
		BF_Debug_Print_Information(("[Updater]Unimplemented for eMMC. \n"));
		ercd = updater_memory_all_main_emmc((unsigned char*)data_addr, size);
		if (ercd != 0){
			BF_Debug_Print_Error(("[Updater]fj_updater_memory_all:%d\n", ercd));
			return FJ_ERR_NG;
		}
	}
	else{
		BF_Debug_Print_Error(("[Updater]Boot check Err:[ Boot Mode pin = 0x%lx ]\n", g_MDR_value));
		return FJ_ERR_NG;
	}

	printf("**** COMPLETE **** FJ_Updater_Memory_All\n");
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_updater_selector(VOID)
{
	FW_INT32 ercd;
	unsigned long *mdrAddr = ( unsigned long * )0x1DFF4000;

	printf("**** START **** FJ_Updater_selector\n");

	// Check Mode pin (NAND or eMMC)
	g_MDR_value = (*mdrAddr) & 0x7;
	BF_Debug_Print_Information(("[Updater]Boot check:[ Boot Mode pin = 0x%lx ]\n", g_MDR_value));
	if ( g_MDR_value == 0 ) {
		// NAND Boot
		BF_Debug_Print_Information(("[Updater]Boot check:[ NAND boot ] \n"));
		ercd = updater_selector_main_nand();
		if (ercd != 0){
			BF_Debug_Print_Error(("[Updater]updater_main_nand:%d\n", ercd));
			return FJ_ERR_NG;
		}
	}
	else if ( g_MDR_value == 1 ) {
		// eMMC Boot
		BF_Debug_Print_Information(("[Updater]Boot check:[ eMMC boot ] \n"));
		ercd = updater_selector_main_emmc();
		if (ercd != 0){
			BF_Debug_Print_Error(("[Updater]updater_main_emmc:%d\n", ercd));
			return FJ_ERR_NG;
		}
	}
	else{
		BF_Debug_Print_Error(("[Updater]Boot check Err:[ Boot Mode pin = 0x%lx ]\n", g_MDR_value));
		return FJ_ERR_NG;
	}

	printf("**** COMPLETE **** FJ_Updater_selector\n");
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_updater_dump_partition(UINT32 part_idx)
{
	UINT32 i, j, num_sectors_per_block, block_size, total_blocks, selector;
	T_NF_DEV_INFO NandInfo;
	ULONG PhyBlk, start_sec_num, fid, ret_wr_size, write_size, total_write_size;
	int data_size;
	char filepath[16] = {0};
	FJ_ERR_CODE rval = FJ_ERR_OK;

	//gNF_partition_table[]
	selector = gNF_partition_table[part_idx].selector;
	//gNF_partition_table[part_idx].area[selector].lut

	start_sec_num = gNF_partition_table[part_idx].area[selector].sector_num;
	data_size     = gNF_partition_table[part_idx].area[selector].data_size;
	//total_size    = gNF_partition_table[part_idx].area_size;

	MWNF_Get_Dev_Info(&NandInfo);
	num_sectors_per_block = NandInfo.page_per_blk * NandInfo.sec_per_page;
	block_size = NandInfo.page_per_blk * NandInfo.page_size;
	PhyBlk = start_sec_num / num_sectors_per_block;

	if (data_size % block_size) {
		total_blocks = (data_size / block_size) + 1;
	} else {
		total_blocks = data_size / block_size;
	}

	total_write_size = 0;

	sprintf(filepath, "I:\\par%02d.bin", part_idx);
	fid = FJ_Fs_Open((CHAR *)filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC));
	if (fid) {
		printf("Dump partition %02d, filename = %s, start block %lu, num = %d, size = %d bytes\n", part_idx, filepath, PhyBlk, total_blocks, data_size);

		for (j = 0; j < total_blocks; j++) {
			PhyBlk = gNF_partition_table[part_idx].area[selector].lut[j];
			for (i = 0; i < NandInfo.page_per_blk; i++) {
				if (data_size <= 0) break;

				MWNF_ReadPages(PhyBlk, i, 1, (BYTE *) gPageData);

				write_size = (data_size > NandInfo.page_size) ? NandInfo.page_size : data_size;

				FJ_Fs_Write ( fid, (UINT32 *)gPageData, write_size, &ret_wr_size );
				if (ret_wr_size != write_size) {
					printf("Write block %lu, page %d failed, write size = %lu\n", PhyBlk, i, ret_wr_size);
					rval = FJ_ERR_NG;
					break;
				}
				total_write_size += write_size;
				data_size -= NandInfo.page_size;
			}
		}

		FJ_Fs_Close(fid);
		printf("Total write size = %lu\n", total_write_size);
		printf("done\n");
	}

	return rval;
}

FJ_ERR_CODE fj_updater_dump_block(UINT32 blk_idx, UINT8 *tmp_blk)
{
	UINT32 i, j, block_size, total_blocks;
	T_NF_DEV_INFO NandInfo;
	ULONG PhyBlk, fid, ret_wr_size, write_size, total_write_size;
	int data_size, ret;
	char filepath[16] = {0};
	FJ_ERR_CODE rval = FJ_ERR_OK;

	MWNF_Get_Dev_Info(&NandInfo);

	printf("dump %dth block from total %d blocks\n", blk_idx, NandInfo.phy_blk_num);
	if (blk_idx > NandInfo.phy_blk_num) {
		return FJ_ERR_NG;
	}

	if (blk_idx == 0) {
		MWNF_Set_Ecc_Param(D_MWNF_ENABLE, D_MWNF_ECC_CAP16);
		MWNF_Set_Skip_Bytes(0);
	}

	block_size = NandInfo.page_per_blk * NandInfo.page_size;

	total_blocks = 1;
	data_size = block_size * total_blocks;
	total_write_size = 0;

	sprintf(filepath, "I:\\blk%04d.bin", blk_idx);
	fid = FJ_Fs_Open((CHAR *)filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC));
	if (fid) {
		printf("Dump block %02d, filename = %s, num = %d, size = %d bytes\n", blk_idx, filepath, total_blocks, data_size);

		for (j = blk_idx; j < blk_idx + total_blocks; j++) {
			PhyBlk = j;
			for (i = 0; i < NandInfo.page_per_blk; i++) {
				if (data_size <= 0) break;

				ret = MWNF_ReadPages(PhyBlk, i, 1, (BYTE *) gPageData);
				if (ret != D_MWNF_OK) {
					printf("Read page error, PhyBlk = %lu, page = %dth\n", PhyBlk,i);
				}

				write_size = (data_size > NandInfo.page_size) ? NandInfo.page_size : data_size;
				// copy to temp buffer
				memcpy((void *)(tmp_blk + total_write_size), (UINT32 *)gPageData, write_size);
				FJ_Fs_Write ( fid, (UINT32 *)gPageData, write_size, &ret_wr_size );
				if (ret_wr_size != write_size) {
					printf("Write block %lu, page %d failed, write size = %lu\n", PhyBlk, i, ret_wr_size);
					rval = FJ_ERR_NG;
					break;
				}
				total_write_size += write_size;
				data_size -= NandInfo.page_size;
			}
		}

		FJ_Fs_Close(fid);
		printf("Total write size = %lu\n", total_write_size);
		printf("done\n");
	}

	if (blk_idx == 0) {
		MWNF_Set_Ecc_Param( (gNANDFlashParameter.ecc_config & 0x00000001),  ( (gNANDFlashParameter.ecc_config & 0x00000700) >> 8) );
		MWNF_Set_Skip_Bytes((gNANDFlashParameter.skip_bytes_conf & 0xFF));
	}

	return rval;
}


FJ_ERR_CODE fj_updater_save_block(UINT32 blk_idx, UINT8 *tmp_blk)
{
	UINT32 PhyBlk = 0, bWrite = 0, i;
	BYTE Bad_blk = 0;
	INT32 ret;
	FJ_ERR_CODE rval = FJ_ERR_OK;

	MWNF_Get_Dev_Info((T_NF_DEV_INFO*)&NandInfo);

	printf("save %dth block from total %d blocks\n", blk_idx, NandInfo.phy_blk_num);

	PhyBlk = blk_idx;

	ret = MWNF_Check_Initial_BadBlock(PhyBlk, &Bad_blk);
	if (ret != D_MWNF_OK) {
		return FJ_ERR_NG;
	}
	else if(Bad_blk != 1) {
		bWrite = 1;	// not bad block
	}
	else {
		printf("Found bad block = %d \n", PhyBlk);
		bWrite = 0;	// bad block
	}
	if (bWrite == 1) {
		ret = MWNF_EraseBlk(PhyBlk);
		if (ret == D_MWNF_OK) {
			//Erase OK
			MWNF_WritePages_NoRetry(PhyBlk, 0, NandInfo.page_per_blk, (BYTE*)tmp_blk);
			for (i = 0; i < NandInfo.page_per_blk; i++) {
				ret = MWNF_ReadPages(PhyBlk, i, 1, (BYTE*)gRamCheckArea);
				if (ret == D_MWNF_OK) {
					ret = ul_memcmp((unsigned long *)gRamCheckArea,
									(unsigned long *)(tmp_blk + (i * (NandInfo.page_size/sizeof(short)))), 1);
					if (ret != 0) {
						printf("Verify block %d page %d failed\n", PhyBlk, i);
						rval = FJ_ERR_NG;
						break;
					}
				} else {
					printf("Read block %d failed\n", PhyBlk);
					rval = FJ_ERR_NG;
					break;
				}
			}
		} else {
			printf("Erase block %d failed\n", PhyBlk);
			rval = FJ_ERR_NG;
		}
	} else {
		rval = FJ_ERR_NG;
	}

	printf("done\n");

	return rval;
}

FJ_ERR_CODE fj_updater_set_secure_enable(BOOL en)
{
	FJ_ERR_CODE rval = FJ_ERR_OK;

	g_secure_update_enable = en;

	return rval;
}

