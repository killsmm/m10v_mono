/**
 * @file		__jctpmc_hdmac.h
 * @brief		Definition JCTPMC HDMAC Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_HDMAC_H_
#define _IO_PMC_HDMAC_H_

/* HDMAC */

/* DMACR */
union io_pmc_hdmac_dmacr {
	unsigned long		word;
	struct {
		unsigned long					:24;
		unsigned long	DH				:4;
		unsigned long	PR				:1;
		unsigned long					:1;
		unsigned long	DS				:1;
		unsigned long	DE				:1;
	}bit;
};

/* DMACA */
union io_pmc_hdmac_dmaca {
	unsigned long		word;
	struct {
		unsigned long	TC				:16;
		unsigned long	BC				:4;
		unsigned long	BT				:4;
		unsigned long	IS				:5;
		unsigned long	ST				:1;
		unsigned long	PB				:1;
		unsigned long	EB				:1;
	}bit;
};

/* DMACB */
union io_pmc_hdmac_dmacb {
	unsigned long		word;
	struct {
		unsigned long					:8;
		unsigned long	DP				:4;
		unsigned long	SP				:4;
		unsigned long	SS				:3;
		unsigned long	CI				:1;
		unsigned long	EI				:1;
		unsigned long	RD				:1;
		unsigned long	RS				:1;
		unsigned long	RC				:1;
		unsigned long	FD				:1;
		unsigned long	FS				:1;
		unsigned long	TW				:2;
		unsigned long	MS				:2;
		unsigned long	TT				:2;
	}bit;
};

/* DMACA/DMACB/DMACSA/DMACDA (0-7ch)  */
struct io_pmc_hdmac_element {
	union io_pmc_hdmac_dmaca	DMACA;
	union io_pmc_hdmac_dmacb	DMACB;
	unsigned long				DMACSA;
	unsigned long				DMACDA;
};


/* HDMAC */
struct io_pmc_hdmac {
	union	io_pmc_hdmac_dmacr		DMACR;										/* 1DC0_(0000 - 0003h) */
	
	unsigned char					dmy_pmc_hdmac_0004_000F[0x0010 - 0x0004];	/* 1DC0_(0004 - 000Fh) */
	
	struct	io_pmc_hdmac_element	DMAC[8];									/* 1DC0_(0010 - 008Fh) */
	
	unsigned char					dmy_pmc_hdmac_00090_FFFFF[0x1DD00000 - 0x1DC00090];		/* 1DC0_0090 - 1DCF_FFFFh */
};

#endif	// _IO_PMC_HDMAC_H_
