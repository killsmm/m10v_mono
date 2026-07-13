/**
 * @file		sd_adma.h
 * @brief		SD Memory card ADMA function header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _SD_ADMA_H_
#define _SD_ADMA_H_

#if CO_SD_BIG_ENDIAN_DATA	// Big Endian select
typedef struct t_sd_adma_descriptor {
	unsigned short length;
	unsigned short attr;
	unsigned long  addr;
} T_SD_ADMA_DESCRIPTOR;
#else						// Little Endian select
typedef struct t_sd_adma_descriptor {
	unsigned short attr;
	unsigned short length;
	unsigned long  addr;
} T_SD_ADMA_DESCRIPTOR;
#endif


#define D_SD_ADMA_ACT_NOP		(0x0000)
#define D_SD_ADMA_ACT_TRAN		(0x0020)
#define D_SD_ADMA_ACT_LINK		(0x0030)

#define D_SD_ADMA_ATTR_INT		(0x0004)
#define D_SD_ADMA_ATTR_END		(0x0002)
#define D_SD_ADMA_ATTR_VALID	(0x0001)

#define D_SD_ADMA_MAX_LENGTH	(65536)

// CMD_DMA_Support
#define D_SD_ADMA_DESCRIPTOR_NUM	(512)

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern volatile T_SD_ADMA_DESCRIPTOR	gSD0_ADMA_Descriptor_Tbl[D_SD_ADMA_DESCRIPTOR_NUM];
extern volatile T_SD_ADMA_DESCRIPTOR	gSD1_ADMA_Descriptor_Tbl[D_SD_ADMA_DESCRIPTOR_NUM];
#endif	// _SD_ADMA_H_
