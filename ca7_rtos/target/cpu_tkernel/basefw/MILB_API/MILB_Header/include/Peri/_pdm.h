/**
 * @file		_pdm.h
 * @brief		Definition PDM Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*	structure of PDM_CORE_CFG	*/
union io_pdm_corecfg{
	unsigned long		word;
	struct {
		unsigned long	PDMCORE_EN	:1;
		unsigned long	SOFT_MUTE	:1;
		unsigned long	S_CYCLES	:3;
		unsigned long	HPCUTOFF	:4;
		unsigned long	ADCHPD		:1;
		unsigned long	SINC_RATE	:7;
		unsigned long	MCLKDIV		:6;
		unsigned long	PGA_L		:4;
		unsigned long	PGA_R		:4;
		unsigned long	LRSWAP		:1;
	}bit;
};

/*	structure of PDM_DMA_CFG	*/
union io_pdm_dmacfg{
	unsigned long		word;
	struct {
		unsigned long	DMA_EN			:1;
		unsigned long	PCM_WDLEN		:2;
		unsigned long	PCM_CHSET		:2;
		unsigned long					:1;
		unsigned long	DMA_BURSTLEN	:2;
		unsigned long					:2;
		unsigned long	DMA_FLUSH		:1;
		unsigned long					:1;
		unsigned long	CLR_IRQ_DMA0	:1;
		unsigned long	CLR_IRQ_DMA1	:1;
		unsigned long					:1;
		unsigned long	CLR_IRQ_FFOVF	:1;
		unsigned long					:8;
		unsigned long	SINC_SHIFT		:4;
		unsigned long	DMICK_DLY		:2;
		unsigned long					:2;
	}bit;
};

/*	structure of PDM_DMA_LEN	*/
union io_pdm_dmalen{
	unsigned long		word;
	struct {
		unsigned long				:8;
		unsigned long	DMA_TSIZE	:8;
		unsigned long	DMA_TTSIZE	:15;
		unsigned long				:1;
	}bit;
};

/*	structure of PDM_DMA_DST_ADDR	*/
union io_pdm_dmadstaddr{
	unsigned long		word;
	struct {
		unsigned long				:2;
		unsigned long	DMA_ADDR	:30;
	}bit;
};

/*	structure of PDM_STATUS	*/
union io_pdm_status{
	unsigned long		word;
	struct {
		unsigned long	FFOVF_INT_REG	:1;
		unsigned long				 	:1;
		unsigned long	DMA0_INT_REG	:1;
		unsigned long	DMA1_INT_REG	:1;
		unsigned long					:28;
	}bit;
};

/* Define I/O Mapping PDM */
/* PDM ch0	(1E00_(4000 - 43FFh)*/
/* PDM ch1	(1E00_(4400 - 47FFh)*/
struct io_pdm {
	union io_pdm_corecfg		CORE_CFG;										/* 1E00_(4000 - 4003h)	*/
	union io_pdm_dmacfg			DMA_CFG;										/* 1E00_(4004 - 4007h)	*/
	union io_pdm_dmalen			DMA_LEN;										/* 1E00_(4008 - 400Bh)	*/
	union io_pdm_dmadstaddr		DMA0_DST_ADDR;									/* 1E00_(400C - 400Fh)	*/
	union io_pdm_dmadstaddr		DMA1_DST_ADDR;									/* 1E00_(4010 - 4013h)	*/
	union io_pdm_status 		STATUS; 										/* 1E00_(4014 - 4017h)	*/
	unsigned char				dmy_1E004018_1E0043FF[0x1E004400 - 0x1E004018]; /* 1E00_(4018 - 43FFh)	*/
};

extern volatile struct io_pdm 	IO_PDM[2];		/* addr 1E004000h-	*/

