/**
 * @file		_jmiluart.h
 * @brief		Definition JMILUART Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* UART */
union io_uart_scr {
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

union io_uart_smr {
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

union io_uart_ssr {
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

union io_uart_escr {
	unsigned char		byte;
	struct {
		unsigned char	L				:3;
		unsigned char	P				:1;
		unsigned char	PEN 			:1;
		unsigned char	INV 			:1;
		unsigned char					:1;
		unsigned char	FLWEN			:1;
	} bit;
};

union io_uart_dr {
	unsigned short		hword;
	unsigned char		byte[2];
	struct {
		unsigned short	__DATA			:9;
		unsigned short					:7;
	} bit;
};

union io_uart_bgr {
	unsigned short		hword;
	struct {
		unsigned short	BGR 			:15;
		unsigned short	EXT 			:1;
	} bit;
};


/* CSIO */
union io_csio_scr {
	unsigned char		byte;
	struct {
		unsigned char	TXE 			:1;
		unsigned char	RXE 			:1;
		unsigned char	TBIE			:1;
		unsigned char	TIE 			:1;
		unsigned char	RIE 			:1;
		unsigned char	SPI 			:1;
		unsigned char	MS				:1;
		unsigned char	UPCL			:1;
	} bit;
};

union io_csio_smr {
	unsigned char		byte;
	struct {
		unsigned char	SOE 			:1;
		unsigned char	SCKE			:1;
		unsigned char	BDS 			:1;
		unsigned char	SCINV			:1;
		unsigned char					:1;
		unsigned char	MD				:3;
	} bit;
};

union io_csio_ssr {
	unsigned char		byte;
	struct {
		unsigned char	__TBI 			:1;
		unsigned char	__TDRE			:1;
		unsigned char	__RDRF			:1;
		unsigned char	__ORE 			:1;
		unsigned char					:3;
		unsigned char	__REC 			:1;
	} bit;
};

union io_csio_escr {
	unsigned char		byte;
	struct {
		unsigned char	L				:3;
		unsigned char					:2;
		unsigned char					:2;
		unsigned char	SOP 			:1;
	} bit;
};

union io_csio_dr {
	unsigned short		hword;
	unsigned char		byte[2];
	struct {
		unsigned short	__DATA			:9;
		unsigned short					:7;
	} bit;
};

union io_csio_bgr {
	unsigned short		hword;
	struct {
		unsigned short	BGR 			:15;
		unsigned short					:1;
	} bit;
};


/* UART/CSIO common */
union io_usio_fcr {
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

union io_usio_fbyte {
	unsigned short		hword;
	unsigned char		byte[2];
	struct {
		unsigned short	__FIFO1 		:8;
		unsigned short	__FIFO2 		:8;
	} bit;
};


/* UART */
struct io_uart {
	union	io_uart_smr 	SMR;									/* 1E70_0000h	*/
	union	io_uart_scr 	SCR;									/* 1E70_0001h	*/
	union	io_uart_escr	ESCR;									/* 1E70_0002h	*/
	union	io_uart_ssr 	SSR;									/* 1E70_0003h	*/
	union	io_uart_dr		DR; 									/* 1E70_(0004 - 0005h)	*/
	union	io_uart_bgr 	BGR;									/* 1E70_(0006 - 0007h)	*/

	unsigned char			dmy_uart_0008_000B[0x000C - 0x0008];	/* 1E70_(0008 - 000Bh)	*/

	union	io_usio_fcr 	FCR;									/* 1E70_(000C - 000Dh)	*/
	union	io_usio_fbyte	FBYTE;									/* 1E70_(000E - 000Fh)	*/
};


/* CSIO */
struct io_csio {
	union	io_csio_smr 	SMR;									/* 1E70_0000h	*/
	union	io_csio_scr 	SCR;									/* 1E70_0001h	*/
	union	io_csio_escr	ESCR;									/* 1E70_0002h	*/
	union	io_csio_ssr 	SSR;									/* 1E70_0003h	*/
	union	io_csio_dr		DR; 									/* 1E70_(0004 - 0005h)	*/
	union	io_csio_bgr 	BGR;									/* 1E70_(0006 - 0007h)	*/

	unsigned char			dmy_csio_0008_000B[0x000C - 0x0008];	/* 1E70_(0008 - 000Bh)	*/

	union	io_usio_fcr 	FCR;									/* 1E70_(000C - 000Dh)	*/
	union	io_usio_fbyte	FBYTE;									/* 1E70_(000E - 000Fh)	*/
};


/* Define I/O mapping		*/
union io_usio {
	struct	io_uart 		UART[8];							/* 1E70_(0000 - 007Fh)	*/
	struct	io_csio 		CSIO[8];							/* 1E70_(0000 - 007Fh)	*/
};

extern volatile union io_usio		IO_USIO;					/* addr 1E70_0000h- */

