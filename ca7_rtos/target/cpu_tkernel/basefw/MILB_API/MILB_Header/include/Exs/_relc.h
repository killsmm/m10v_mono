/**
 * @file		_relc.h
 * @brief		Definition RELC(Decode) Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// structure of RELC RESET		(1B03_0000h)
union io_relc_reset {
    unsigned long word;
    struct {
		unsigned long	SRST	:1;
		unsigned long			:31;	// reserved
    }bit;
};

// structure of RELC MODE1		(1B03_0004h)
union io_relc_mode1 {
    unsigned long word;
    struct {
		unsigned long	PRUN	:1;
		unsigned long	CRMD	:1;
		unsigned long	DSMD	:1;
		unsigned long			:29;	// reserved
    }bit;
};

// structure of RELC MODE2		(1B03_0008h)
union io_relc_mode2 {
    unsigned long word;
    struct {
		unsigned long	IMAX	:1;
		unsigned long	OMAX	:1;
		unsigned long	IAAI	:1;
		unsigned long	OAAI	:1;
		unsigned long			:28;	// reserved
    }bit;
};

// structure of RELC INTEN		(1B03_000Ch)
union io_relc_inten {
    unsigned long word;
    struct {
		unsigned long	SINT	:1;
		unsigned long	FINT	:1;
		unsigned long	EINT	:1;
		unsigned long	CINT	:1;
		unsigned long			:28;	// reserved
    }bit;
};

// structure of RELC INTST		(1B03_0010h)
union io_relc_intst {
    unsigned long word;
    struct {
		unsigned long	__SINS	:1;
		unsigned long	__FINS	:1;
		unsigned long	__EINS	:1;
		unsigned long	__CINS	:1;
		unsigned long			:28;	// reserved
    }bit;
};

// structure of RELC STATUS		(1B03_0014h)
union io_relc_status {
    unsigned long word;
    struct {
		unsigned long	RUNS	:1;
		unsigned long	SLPS	:1;
		unsigned long	FINS	:1;
		unsigned long			:1;		// reserved
		unsigned long	SRES	:3;
		unsigned long			:1;		// reserved
		unsigned long	EFLG	:1;
		unsigned long			:23;	// reserved
    }bit;
};

// structure of RELC BUF SIZE	(1B03_0034h)
union io_relc_buf_size {
    unsigned long word;
    struct {
		unsigned long	IBFS	:3;
		unsigned long			:5;		// reserved
		unsigned long	RBFS	:1;
		unsigned long			:23;	// reserved
    }bit;
};

// structure of RELC ERROR NUM	(1B03_0038h)
union io_relc_error_num {
    unsigned long word;
    struct {
		unsigned long	ENUM	:16;
		unsigned long			:16;		// reserved
    }bit;
};

// structure of RELC READ BYTE	(1B03_003Ch)
union io_relc_read_byte {
    unsigned long word;
    struct {
		unsigned long	DRNM	:16;
		unsigned long			:16;		// reserved
    }bit;
};

// structure of RELC WRITE BYTE	(1B03_0040h)
union io_relc_write_byte {
    unsigned long word;
    struct {
		unsigned long	DWNM	:16;
		unsigned long			:16;		// reserved
    }bit;
};

// structure of RELC BLOCK		(1B03_0044h)
union io_relc_block {
    unsigned long word;
    struct {
		unsigned long	BLKN	:16;
		unsigned long			:16;		// reserved
    }bit;
};


struct io_relc {
	union io_relc_reset			RELC_RESET;					// 1B03_0000h
	union io_relc_mode1			RELC_MODE1;					// 1B03_0004h
	union io_relc_mode2			RELC_MODE2;					// 1B03_0008h
	union io_relc_inten			RELC_INT_EN;				// 1B03_000Ch
	union io_relc_intst			RELC_INT_ST;				// 1B03_0010h
	union io_relc_status		RELC_STATUS;				// 1B03_0014h
	unsigned long				RELC_IN_START_ADDR;			// 1B03_0018h
	unsigned long				RELC_IN_END_ADDR;			// 1B03_001Ch
	unsigned long				RELC_OUT_START_ADDR;		// 1B03_0020h
	unsigned long				RELC_OUT_END_ADDR;			// 1B03_0024h
	unsigned long				RELC_DESCRIPTOR_ADDR;		// 1B03_0028h
	unsigned long				RELC_SEQ_NUM;				// 1B03_002Ch
	unsigned long				RELC_SEQ_CNT;				// 1B03_0030h
	union io_relc_buf_size		RELC_BUF_SIZE;				// 1B03_0034h
	union io_relc_error_num		RELC_ERROR_NUM;				// 1B03_0038h
	union io_relc_read_byte		RELC_READ_BYTE;				// 1B03_003Ch
	union io_relc_write_byte	RELC_WRITE_BYTE;			// 1B03_0040h
	union io_relc_block			RELC_BLOCK;					// 1B03_0044h
	unsigned long				RELC_INPUT_BYTE;			// 1B03_0048h
	unsigned long				RELC_OUTPUT_BYTE;			// 1B03_004Ch
	unsigned char				dmy_00000050_00000FFF[0x00001000-0x00000050];
	unsigned long				RELC_INPUT_BUF_DATA_MIRROR;	// 1B03_1000h
	unsigned char				dmy_00001004_000017FF[0x00001800-0x00001004];
	unsigned long				RELC_INPUT_BUF_DATA;		// 1B03_1800h
	unsigned char				dmy_00001804_00001FFF[0x00002000-0x00001804];
	unsigned long				RELC_REF_BUF_DATA;			// 1B03_2000h
	unsigned char				dmy_00002004_00003FFF[0x00004000-0x00002004];
};

extern volatile struct io_relc	IO_RELC;					// addr 1B03_0000h-


