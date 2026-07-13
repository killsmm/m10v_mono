/**
 * @file		_spi.h
 * @brief		Definition SPI Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*	structure of DIV	*/
union io_spi_div{
	unsigned long		word;
	struct {
		unsigned long	DIVISOR		:8;
		unsigned long				:24;
	}bit;
};

/*	structure of CTRL	*/
union io_spi_ctrl{
	unsigned long		word;
	struct {
		unsigned long	CONTXFER	:1;
		unsigned long	DIVENABLE	:1;
		unsigned long	MSB1ST		:1;
		unsigned long	CPHA		:1;
		unsigned long	CPOL		:1;
		unsigned long	MASTER		:1;
		unsigned long				:4;
		unsigned long	DMA			:1;
		unsigned long	MWAITEN		:1;
		unsigned long				:20;
	}bit;
};

/*	structure of AUX_CTRL	*/
union io_spi_auxctrl{
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

/*	structure of ST	*/
union io_spi_st{
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

/*	structure of SLV_SEL	*/
union io_spi_slvsel{
	unsigned long		word;
	struct {
		unsigned long	SSOUT0			:1;
		unsigned long	SSOUT1			:1;
		unsigned long	SSOUT2			:1;
		unsigned long	SSOUT3			:1;
		unsigned long					:28;
	}bit;
};

/*	structure of SLV_POL	*/
union io_spi_slvpol{
	unsigned long		word;
	struct {
		unsigned long	SSPOL0			:1;
		unsigned long	SSPOL1			:1;
		unsigned long	SSPOL2			:1;
		unsigned long	SSPOL3			:1;
		unsigned long					:28;
	}bit;
};

/*	structure of INT_EN	*/
union io_spi_inten{
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

/*	structure of INT_ST	*/
union io_spi_intst{
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

/*	structure of INT_CLR	*/
union io_spi_intclr{
	unsigned long		word;
	struct {
		unsigned long	__TXEMPTYPULSE	:1;
		unsigned long	__TXWMARKPULSE	:1;
		unsigned long	__RXWMARKPULSE	:1;
		unsigned long	__RXFULLPULSE	:1;
		unsigned long	__XFERDONEPULSE	:1;
		unsigned long					:2;
		unsigned long	__RXFIFOOVERFLOW:1;
		unsigned long	__RXTIMEOUT		:1;
		unsigned long					:23;
	}bit;
};

/*	structure of TXFIFO	*/
union io_spi_txfifo{
	unsigned long		word;
	struct {
		unsigned long	TX_FIFO_LEVEL	:8;
		unsigned long					:24;
	}bit;
};

/*	structure of RXFIFO	*/
union io_spi_rxfifo{
	unsigned long		word;
	struct {
		unsigned long	RX_FIFO_LEVEL	:8;
		unsigned long					:24;
	}bit;
};

/*	structure of DMA_TO	*/
union io_spi_dmato{
	unsigned long		word;
	struct {
		unsigned long	TIMEOUT			:24;
		unsigned long					:8;
	}bit;
};

/*	structure of MS_DLY	*/
union io_spi_msdly{
	unsigned long		word;
	struct {
		unsigned long	MWAIT			:8;
		unsigned long					:24;
	}bit;
};

/*	structure of EN	*/
union io_spi_en{
	unsigned long		word;
	struct {
		unsigned long	__ENABLEREQ		:1;
		unsigned long	__EXTENSEL		:3;
		unsigned long					:28;
	}bit;
};

/*	structure of FIFO_DPTH	*/
union io_spi_fifodpth{
	unsigned long		word;
	struct {
		unsigned long	FIFODEPTH		:9;
		unsigned long					:23;
	}bit;
};

/*	structure of FIFO_WMK	*/
union io_spi_fifowmk{
	unsigned long		word;
	struct {
		unsigned long	RXWMARKSET		:8;
		unsigned long	TXWMARKSET		:8;
		unsigned long					:16;
	}bit;
};

/*	structure of TX_DWR	*/
union io_spi_txdwr{
	unsigned long		word;
	struct {
		unsigned long	TXDUMMYWR		:8;
		unsigned long					:24;
	}bit;
};


/* Define I/O Mapping SPI */
/* SPI ch0	(1E80_(0000 - 00FFh)*/
/* SPI ch1	(1E80_(0100 - 01FFh)*/
/* SPI ch2	(1E80_(0200 - 02FFh)*/
struct io_spi {
	unsigned long				TXDATA;											/* 1E80_(0000 - 0003h)	*/
	unsigned long				RXDATA;											/* 1E80_(0004 - 0007h)	*/
	union io_spi_div			DIV;											/* 1E80_(0008 - 000Bh)	*/
	union io_spi_ctrl			CTRL;											/* 1E80_(000C - 000Fh)	*/
	union io_spi_auxctrl		AUX_CTRL;										/* 1E80_(0010 - 0013h)	*/
	union io_spi_st				ST;												/* 1E80_(0014 - 0017h)	*/
	union io_spi_slvsel			SLV_SEL;										/* 1E80_(0018 - 001Bh)	*/
	union io_spi_slvpol			SLV_POL;										/* 1E80_(001C - 001Fh)	*/
	union io_spi_inten			INT_EN;											/* 1E80_(0020 - 0023h)	*/
	union io_spi_intst			INT_ST;											/* 1E80_(0024 - 0027h)	*/
	union io_spi_intclr			INT_CLR;										/* 1E80_(0028 - 002Bh)	*/
	union io_spi_txfifo			TXFIFO;											/* 1E80_(002C - 002Fh)	*/
	union io_spi_rxfifo			RXFIFO;											/* 1E80_(0030 - 0033h)	*/
	union io_spi_dmato			DMA_TO;											/* 1E80_(0034 - 0037h)	*/
	union io_spi_msdly			MS_DLY;											/* 1E80_(0038 - 003Bh)	*/
	union io_spi_en				EN;												/* 1E80_(003C - 003Fh)	*/
	unsigned char				dmy_1E800040_1E800047[0x1E800048 - 0x1E800040]; /* 1E80_(0040 - 0047h)	*/
	union io_spi_fifodpth		FIFO_DPTH;										/* 1E80_(0048 - 004Bh)	*/
	union io_spi_fifowmk		FIFO_WMK;										/* 1E80_(004C - 004Fh)	*/
	union io_spi_txdwr			TX_DWR;											/* 1E80_(0050 - 0053h)	*/
	unsigned char				dmy_1E800054_1E8000FF[0x1E800100 - 0x1E800054]; /* 1E80_(0054 - 00FFh)	*/
};

extern volatile struct io_spi	IO_SPI[3];		/* addr 1E800000h-	*/

