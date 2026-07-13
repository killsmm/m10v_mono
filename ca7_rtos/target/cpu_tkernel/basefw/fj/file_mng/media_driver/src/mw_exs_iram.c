/**
 * @file		mw_exs_iram.c
 * @brief		Global variable which is assigned to External Storage Internal RAM.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include "sd_adma.h"
#include "em_config.h"

// CMD_DMA_Support
#include "mwnf_core.h"

// CMD_DMA_Support
#ifdef __GNUC__
volatile char sc_descriptor_area[ EM_CONFIG_MAX_DESCR_BUFF_SIZE * EM_CONFIG_SLOT_COUNT ] __attribute__((section(".EXS_IRAM"),aligned(32)));
volatile T_SD_ADMA_DESCRIPTOR gSD1_ADMA_Descriptor_Tbl[D_SD_ADMA_DESCRIPTOR_NUM] __attribute__((section(".EXS_IRAM"),aligned(32)));
volatile T_SD_ADMA_DESCRIPTOR gSD0_ADMA_Descriptor_Tbl[D_SD_ADMA_DESCRIPTOR_NUM] __attribute__((section(".EXS_IRAM"),aligned(32)));
volatile T_MWNF_CMD_DMA_DESCRIPTOR gNF_CMD_DMA_Descriptor_Tbl2[D_NF_CMD_DMA_DESCRIPTOR_NUM] __attribute__((section(".EXS_IRAM"),aligned(32)));
volatile T_MWNF_CMD_DMA_DESCRIPTOR gNF_CMD_DMA_Descriptor_Tbl1[D_NF_CMD_DMA_DESCRIPTOR_NUM] __attribute__((section(".EXS_IRAM"),aligned(32)));
volatile T_MWNF_CMD_DMA_SYNC_FLAG gNF_CMD_DMA_SyncFlag_Pointer_Tbl[D_NF_CMD_DMA_SYNC_FLAG_NUM] __attribute__((section(".EXS_IRAM"),aligned(32)));
volatile unsigned char gMW_Exs_IRAM_NF[18*1024] __attribute__((section(".EXS_IRAM"),aligned(32)));		// 20KB for now
#else
volatile __align(8) unsigned char gMW_Exs_IRAM_NF[18*1024];		// 20KB for now
volatile __align(32) T_MWNF_CMD_DMA_SYNC_FLAG gNF_CMD_DMA_SyncFlag_Pointer_Tbl[D_NF_CMD_DMA_SYNC_FLAG_NUM];
volatile __align(32) T_MWNF_CMD_DMA_DESCRIPTOR gNF_CMD_DMA_Descriptor_Tbl1[D_NF_CMD_DMA_DESCRIPTOR_NUM];
volatile __align(32) T_MWNF_CMD_DMA_DESCRIPTOR gNF_CMD_DMA_Descriptor_Tbl2[D_NF_CMD_DMA_DESCRIPTOR_NUM];
volatile __align(16) T_SD_ADMA_DESCRIPTOR gSD0_ADMA_Descriptor_Tbl[D_SD_ADMA_DESCRIPTOR_NUM];
volatile __align(16) T_SD_ADMA_DESCRIPTOR gSD1_ADMA_Descriptor_Tbl[D_SD_ADMA_DESCRIPTOR_NUM];
volatile __align(16) char sc_descriptor_area[ EM_CONFIG_MAX_DESCR_BUFF_SIZE * EM_CONFIG_SLOT_COUNT ];
#endif
