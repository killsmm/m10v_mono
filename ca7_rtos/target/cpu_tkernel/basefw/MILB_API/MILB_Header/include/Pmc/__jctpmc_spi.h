/**
 * @file		__jctpmc_spi.h
 * @brief		Definition JCTPMC QSPI Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_SPI_H_
#define _IO_PMC_SPI_H_

/* QSPI */

/* DIV */
union io_pmc_spi_div {
	unsigned long		word;
	struct {
		unsigned long	DIVISOR			:8;
		unsigned long					:24;
	}bit;
};

/* CTRL */
union io_pmc_spi_ctrl {
	unsigned long		word;
	struct {
		unsigned long	CONTXFER		:1;
		unsigned long	DIVENABLE		:1;
		unsigned long	MSB1ST			:1;
		unsigned long	CPHA			:1;
		unsigned long	CPOL			:1;
		unsigned long	MASTER			:1;
		unsigned long					:4;
		unsigned long	DMA				:1;
		unsigned long	MWAITEN			:1;
		unsigned long					:20;
	}bit;
};

/* AUX_CTRL */
union io_pmc_spi_aux_ctrl {
	unsigned long		word;
	struct {
		unsigned long	SPIMODE			:2;
		unsigned long					:1;
		unsigned long	INHIBITDIN		:1;
		unsigned long	XFERFORMAT		:2;
		unsigned long					:1;
		unsigned long	CONTXFEREXTEND	:1;
		unsigned long	BITSIZE			:5;
		unsigned long					:19;
	}bit;
};

/* ST */
union io_pmc_spi_st {
	unsigned long		word;
	struct {
		unsigned long	XFERIP			:1;
		unsigned long				 	:1;
		unsigned long	TXEMPTY			:1;
		unsigned long	TXWMARK			:1;
		unsigned long	TXFULL			:1;
		unsigned long	RXEMPTY			:1;
		unsigned long	RXWMARK			:1;
		unsigned long	RXFULL			:1;
		unsigned long	RXOVERFLOW		:1;
		unsigned long	RXTIMEOUT		:1;
		unsigned long					:22;
	}bit;
};

/* SLV_SEL */
union io_pmc_spi_slv_sel {
	unsigned long		word;
	struct {
		unsigned long	SSOUT			:1;
		unsigned long					:31;
	}bit;
};

/* SLV_POL */
union io_pmc_spi_slv_pol {
	unsigned long		word;
	struct {
		unsigned long	SSPOL			:1;
		unsigned long					:31;
	}bit;
};

/* INT_EN */
union io_pmc_spi_int_en {
	unsigned long		word;
	struct {
		unsigned long	TXEMPTYPULSE	:1;
		unsigned long	TXWMARKPULSE	:1;
		unsigned long	RXWMARKPULSE	:1;
		unsigned long	RXFULLPULSE		:1;
		unsigned long	XFERDONEPULSE	:1;
		unsigned long					:2;
		unsigned long	RXFIFOOVERFLOW	:1;
		unsigned long	RXTIMEOUT		:1;
		unsigned long					:23;
	}bit;
};

/* INT_ST */
union io_pmc_spi_int_st {
	unsigned long		word;
	struct {
		unsigned long	TXEMPTYPULSE	:1;
		unsigned long	TXWMARKPULSE	:1;
		unsigned long	RXWMARKPULSE	:1;
		unsigned long	RXFULLPULSE		:1;
		unsigned long	XFERDONEPULSE	:1;
		unsigned long					:2;
		unsigned long	RXFIFOOVERFLOW	:1;
		unsigned long	RXTIMEOUT		:1;
		unsigned long					:23;
	}bit;
};

/* INT_CLR */
union io_pmc_spi_int_clr {
	unsigned long		word;
	struct {
		unsigned long	__TXEMPTYPULSE		:1;
		unsigned long	__TXWMARKPULSE		:1;
		unsigned long	__RXWMARKPULSE		:1;
		unsigned long	__RXFULLPULSE		:1;
		unsigned long	__XFERDONEPULSE		:1;
		unsigned long						:2;
		unsigned long	__RXFIFOOVERFLOW	:1;
		unsigned long	__RXTIMEOUT			:1;
		unsigned long						:23;
	}bit;
};

/* TXFIFO */
union io_pmc_spi_txfifo {
	unsigned long		word;
	struct {
		unsigned long	TX_FIFO_LEVEL	:7;
		unsigned long					:25;
	}bit;
};

/* RXFIFO */
union io_pmc_spi_rxfifo {
	unsigned long		word;
	struct {
		unsigned long	RX_FIFO_LEVEL	:7;
		unsigned long					:25;
	}bit;
};

/* DMA_TO */
union io_pmc_spi_dma_to {
	unsigned long		word;
	struct {
		unsigned long	TIMEOUT			:24;
		unsigned long					:8;
	}bit;
};

/* MS_DLY */
union io_pmc_spi_ms_dly {
	unsigned long		word;
	struct {
		unsigned long	MWAIT			:8;
		unsigned long					:24;
	}bit;
};

/* EN */
union io_pmc_spi_en {
	unsigned long		word;
	struct {
		unsigned long	__ENABLEREQ		:1;
		unsigned long	__EXTENSEL		:3;
		unsigned long					:28;
	}bit;
};

/* FIFO_DPTH */
union io_pmc_spi_fifo_dpth {
	unsigned long		word;
	struct {
		unsigned long	FIFODEPTH		:9;
		unsigned long					:23;
	}bit;
};

/* FIFO_WMK */
union io_pmc_spi_fifo_wmk {
	unsigned long		word;
	struct {
		unsigned long	RXWMARKSET		:8;
		unsigned long	TXWMARKSET		:8;
		unsigned long					:16;
	}bit;
};

/* TX_DWR */
union io_pmc_spi_tx_dwr {
	unsigned long		word;
	struct {
		unsigned long	TXDUMMYWR		:8;
		unsigned long					:24;
	}bit;
};


/* QSPI */
struct io_pmc_spi {
	unsigned long					TXDATA;										/* 1DD0_(0000 - 0003h)	*/
	unsigned long					RXDATA;										/* 1DD0_(0004 - 0007h)	*/
	union	io_pmc_spi_div			DIV;										/* 1DD0_(0008 - 000Bh)	*/
	union	io_pmc_spi_ctrl			CTRL;										/* 1DD0_(000C - 000Fh)	*/
	union	io_pmc_spi_aux_ctrl		AUX_CTRL;									/* 1DD0_(0010 - 0013h)	*/
	union	io_pmc_spi_st			ST;											/* 1DD0_(0014 - 0017h)	*/
	union	io_pmc_spi_slv_sel		SLV_SEL;									/* 1DD0_(0018 - 001Bh)	*/
	union	io_pmc_spi_slv_pol		SLV_POL;									/* 1DD0_(001C - 001Fh)	*/
	union	io_pmc_spi_int_en		INT_EN;										/* 1DD0_(0020 - 0023h)	*/
	union	io_pmc_spi_int_st		INT_ST;										/* 1DD0_(0024 - 0027h)	*/
	union	io_pmc_spi_int_clr		INT_CLR;									/* 1DD0_(0028 - 002Bh)	*/
	union	io_pmc_spi_txfifo		TXFIFO;										/* 1DD0_(002C - 002Fh)	*/
	union	io_pmc_spi_rxfifo		RXFIFO;										/* 1DD0_(0030 - 0033h)	*/
	union	io_pmc_spi_dma_to		DMA_TO;										/* 1DD0_(0034 - 0037h)	*/
	union	io_pmc_spi_ms_dly		MS_DLY;										/* 1DD0_(0038 - 003Bh)	*/
	union	io_pmc_spi_en			EN;											/* 1DD0_(003C - 003Fh)	*/
	
	unsigned char					dmy_pmc_spi_040_047[0x048 - 0x040];			/* 1DD0_(0040 - 0047h)	*/
	
	union	io_pmc_spi_fifo_dpth	FIFO_DPTH;									/* 1DD0_(0048 - 004Bh)	*/
	union	io_pmc_spi_fifo_wmk		FIFO_WMK;									/* 1DD0_(004C - 004Fh)	*/
	union	io_pmc_spi_tx_dwr		TX_DWR;										/* 1DD0_(0050 - 0053h)	*/
	
	unsigned char					dmy_pmc_spi_00054_FFFFF[0x100000 - 0x054];	/* 1DD0_0054 - 1DDF_FFFFh)	*/
};

#endif	// _IO_PMC_QSPI_H_
