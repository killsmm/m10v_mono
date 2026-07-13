/**
 * @file		__jctpmc_uart.h
 * @brief		Definition JCTPMC UART Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_UART_H_
#define _IO_PMC_UART_H_

/* UART */
union io_pmc_uart_scr {
	unsigned char		byte;
	struct {
		unsigned char	TXE 			:1;
		unsigned char	RXE 			:1;
		unsigned char	TBIE			:1;
		unsigned char	TIE 			:1;
		unsigned char	RIE 			:1;
		unsigned char					:2;
		unsigned char	UPCL			:1;
	} bit;
};

union io_pmc_uart_smr {
	unsigned char		byte;
	struct {
		unsigned char	SOE 			:1;
		unsigned char					:1;
		unsigned char	BDS 			:1;
		unsigned char	SBL 			:1;
		unsigned char					:1;
		unsigned char	MD				:3;
	} bit;
};

union io_pmc_uart_ssr {
	unsigned char		byte;
	struct {
		unsigned char	__TBI 			:1;
		unsigned char	__TDRE			:1;
		unsigned char	__RDRF			:1;
		unsigned char	__ORE 			:1;
		unsigned char	__FRE 			:1;
		unsigned char	__PE			:1;
		unsigned char					:1;
		unsigned char	__REC 			:1;
	} bit;
};

union io_pmc_uart_escr {
	unsigned char		byte;
	struct {
		unsigned char	L				:3;
		unsigned char	P				:1;
		unsigned char	PEN 			:1;
		unsigned char	INV 			:1;
		unsigned char					:2;
	} bit;
};

union io_pmc_uart_dr {
	unsigned short		hword;
	unsigned char		byte[2];
	struct {
		unsigned short	__DATA			:9;
		unsigned short					:7;
	} bit;
};

union io_pmc_uart_bgr {
	unsigned short		hword;
	struct {
		unsigned short	BGR 			:15;
		unsigned short	EXT 			:1;
	} bit;
};

union io_pmc_uart_fcr {
	unsigned short		hword;
	struct {
		unsigned char	FE1 			:1;
		unsigned char	FE2 			:1;
		unsigned char	FCL1			:1;
		unsigned char	FCL2			:1;
		unsigned char	FSET			:1;
		unsigned char	FLD 			:1;
		unsigned char	FLST			:1;
		unsigned char					:1;
		
		unsigned char	FSEL			:1;
		unsigned char	FTIE			:1;
		unsigned char	FDRQ			:1;
		unsigned char	FRIIE			:1;
		unsigned char	FLSTE			:1;
		unsigned char					:1;
		unsigned char	FTST0			:1;
		unsigned char	FTST1			:1;
	} bit;
};

union io_pmc_uart_fbyte {
	unsigned short		hword;
	unsigned char		byte[2];
	struct {
		unsigned short	__FIFO1 		:8;
		unsigned short	__FIFO2 		:8;
	} bit;
};


/* UART */
struct io_pmc_uart {
	union	io_pmc_uart_smr 	SMR;									/* 1DE0_0000h	*/
	union	io_pmc_uart_scr 	SCR;									/* 1DE0_0001h	*/
	union	io_pmc_uart_escr	ESCR;									/* 1DE0_0002h	*/
	union	io_pmc_uart_ssr 	SSR;									/* 1DE0_0003h	*/
	union	io_pmc_uart_dr		DR; 									/* 1DE0_(0004 - 0005h)	*/
	union	io_pmc_uart_bgr 	BGR;									/* 1DE0_(0006 - 0007h)	*/
	
	unsigned char				dmy_pmc_uart_008_00B[0x00C - 0x008];	/* 1DE0_(0008 - 000Bh)	*/
	
	union	io_pmc_uart_fcr 	FCR;									/* 1DE0_(000C - 000Dh)	*/
	union	io_pmc_uart_fbyte	FBYTE;									/* 1DE0_(000E - 000Fh)	*/
	
	unsigned char				dmy_pmc_uart_00010_FFFFF[0x100000 - 0x0010];	/* 1DE0_0010 - 1DEF_FFFFh	*/
};

#endif	// _IO_PMC_UART_H_
