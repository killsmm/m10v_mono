/**
 * @file		__jctpmc_crg11.h
 * @brief		Definition JCTPMC CRG11 Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_CRG_H_
#define _IO_PMC_CRG_H_

/* CRG11 */

/* CRPLC */
union io_pmc_crg11_crplc {
	unsigned long		word;
	struct {
		unsigned long					:24;
		unsigned long	PLLBYPASS		:1;
		unsigned long					:7;
	}bit;
};

/* CRSTP */
union io_pmc_crg11_crstp {
	unsigned long		word;
	struct {
		unsigned long	STOPEN			:1;
		unsigned long	STOPMNT			:1;
		unsigned long					:30;
	} bit;
};

/* CRRSC */
union io_pmc_crg11_crrsc {
	unsigned long		word;
	struct {
		unsigned long	ARSTMODE		:4;
		unsigned long					:4;
		unsigned long	SWRSTM			:1;
		unsigned long	WDRSTM			:1;
		unsigned long					:22;
	} bit;
};

/* CRSWR */
union io_pmc_crg11_crswr {
	unsigned long		word;
	struct {
		unsigned long	SWRSTREQ		:1;
		unsigned long					:31;
	} bit;
};

/* CRRSM */
union io_pmc_crg11_crrsm {
	unsigned long		word;
	struct {
		unsigned long	WDRST			:1;
		unsigned long	SWRST			:1;
		unsigned long	SRST			:1;
		unsigned long	PORESET			:1;
		unsigned long					:28;
	} bit;
};

/* CRCDC */
union io_pmc_crg11_crcdc {
	unsigned long		word;
	struct {
		unsigned long	DCHREQ			:1;
		unsigned long					:31;
	} bit;
};

/* CRDMx (x=0-5) */
union io_pmc_crg11_crdm {
	unsigned long		word;
	struct {
		unsigned long	DIVMODE			:8;
		unsigned long					:24;
	} bit;
};

/* CRLPx (x=0-5) */
union io_pmc_crg11_crlp {
	unsigned long		word;
	struct {
		unsigned long	CSYSREQ_R		:8;
		unsigned long	LPOWERHS		:8;
		unsigned long	CACTIVE_C		:8;
		unsigned long	CEN				:8;
	} bit;
};

/* PMC */
struct io_pmc_crg11 {
	union	io_pmc_crg11_crplc		CRPLC;										/* 1DFF_(E000 - E003h)	*/
	
	unsigned char					dmy_pmc_crg11_004_007[0x008 - 0x004];		/* 1DFF_(E004 - E007h)	*/
	
	union	io_pmc_crg11_crstp		CRSTP;										/* 1DFF_(E008 - E00Bh)	*/
	
	unsigned char					dmy_pmc_crg11_00C_01F[0x020 - 0x00C];		/* 1DFF_(E00C - E01Fh)	*/
	
	union	io_pmc_crg11_crrsc		CRRSC;										/* 1DFF_(E020 - E023h)	*/
	union	io_pmc_crg11_crswr		CRSWR;										/* 1DFF_(E024 - E027h)	*/
	
	unsigned char					dmy_pmc_crg11_028_02B[0x02C - 0x028];		/* 1DFF_(E028 - E02Bh)	*/
	
	union	io_pmc_crg11_crrsm		CRRSM;										/* 1DFF_(E02C - E02Fh)	*/
	union	io_pmc_crg11_crcdc		CRCDC;										/* 1DFF_(E030 - E033h)	*/
	
	unsigned char					dmy_pmc_crg11_034_0FF[0x100 - 0x034];		/* 1DFF_(E034 - E0FFh)	*/
	
	union	io_pmc_crg11_crdm		CRDM0;										/* 1DFF_(E100 - E103h)	*/
	union	io_pmc_crg11_crlp		CRLP0;										/* 1DFF_(E104 - E107h)	*/
	
	unsigned char					dmy_pmc_crg11_108_10F[0x110 - 0x108];		/* 1DFF_(E108 - E10Fh)	*/
	
	union	io_pmc_crg11_crdm		CRDM1;										/* 1DFF_(E110 - E113h)	*/
	union	io_pmc_crg11_crlp		CRLP1;										/* 1DFF_(E114 - E117h)	*/
	
	unsigned char					dmy_pmc_crg11_118_12F[0x130 - 0x118];		/* 1DFF_(E118 - E12Fh)	*/
	
	union	io_pmc_crg11_crdm		CRDM3;										/* 1DFF_(E130 - E133h)	*/
	union	io_pmc_crg11_crlp		CRLP3;										/* 1DFF_(E134 - E137h)	*/
	
	unsigned char					dmy_pmc_crg11_138_13F[0x140 - 0x138];		/* 1DFF_(E138 - E13Fh)	*/
	
	union	io_pmc_crg11_crdm		CRDM4;										/* 1DFF_(E140 - E143h)	*/
	union	io_pmc_crg11_crlp		CRLP4;										/* 1DFF_(E144 - E147h)	*/
	
	unsigned char					dmy_pmc_crg11_148_14F[0x150 - 0x148];		/* 1DFF_(E148 - E14Fh)	*/
	
	union	io_pmc_crg11_crdm		CRDM5;										/* 1DFF_(E150 - E153h)	*/
	union	io_pmc_crg11_crlp		CRLP5;										/* 1DFF_(E154 - E157h)	*/
	
	unsigned char					dmy_pmc_crg11_158_FFF[0x1000 - 0x158];		/* 1DFF_(E158 - EFFFh)	*/
};

#endif	// _IO_PMC_CRG_H_
