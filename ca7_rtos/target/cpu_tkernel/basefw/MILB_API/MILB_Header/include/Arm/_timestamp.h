/**
 * @file		_timestamp.h
 * @brief		Definition Timestamp Generator I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* structure of CNTCR					(1D01_0000h) */
union io_timestamp_cntcr {
	unsigned long		word;
	struct {
		unsigned long	EN				:1;
		unsigned long	HDBG			:1;
		unsigned long					:30;
	}bit;
};

/* structure of CNTSR					(1D01_0004h) */
union io_timestamp_cntsr {
	unsigned long		word;
	struct {
		unsigned long					:1;
		unsigned long	DBGH			:1;
		unsigned long					:30;
	}bit;
};

/* structure of PIDR4					(1D01_0FD0h) */
union io_timestamp_pidr4 {
	unsigned long		word;
	struct {
		unsigned long	DES_2			:4;
		unsigned long	SIZE			:4;
		unsigned long					:24;
	}bit;
};

/* structure of PIDR0					(1D01_0FE0h) */
union io_timestamp_pidr0 {
	unsigned long		word;
	struct {
		unsigned long	PART_0			:8;
		unsigned long					:24;
	}bit;
};

/* structure of PIDR1					(1D01_0FE4h) */
union io_timestamp_pidr1 {
	unsigned long		word;
	struct {
		unsigned long	PART_1			:4;
		unsigned long	DES_0			:4;
		unsigned long					:24;
	}bit;
};

/* structure of PIDR2					(1D01_0FE8h) */
union io_timestamp_pidr2 {
	unsigned long		word;
	struct {
		unsigned long	DES_1			:3;
		unsigned long	JEDEC			:1;
		unsigned long	REVISION		:4;
		unsigned long					:24;
	}bit;
};

/* structure of PIDR3					(1D01_0FECh) */
union io_timestamp_pidr3 {
	unsigned long		word;
	struct {
		unsigned long	CMOD			:4;
		unsigned long	REVAND			:4;
		unsigned long					:24;
	}bit;
};

/* structure of CIDR0					(1D01_0FF0h) */
union io_timestamp_cidr0 {
	unsigned long		word;
	struct {
		unsigned long	PRMBL_0			:8;
		unsigned long					:24;
	}bit;
};

/* structure of CIDR1					(1D01_0FF4h) */
union io_timestamp_cidr1 {
	unsigned long		word;
	struct {
		unsigned long	PRMBL_1			:4;
		unsigned long	CLASS			:4;
		unsigned long					:24;
	}bit;
};

/* structure of CIDR2					(1D01_0FF8h) */
union io_timestamp_cidr2 {
	unsigned long		word;
	struct {
		unsigned long	PRMBL_2			:8;
		unsigned long					:24;
	}bit;
};

/* structure of CIDR3					(1D01_0FFCh) */
union io_timestamp_cidr3 {
	unsigned long		word;
	struct {
		unsigned long	PRMBL_3			:8;
		unsigned long					:24;
	}bit;
};

/* Timestamp generator register						*/
struct io_timestamp {
	union io_timestamp_cntcr			CNTCR;							// 1D01_(0000 - 0003) Counter Control Register
	union io_timestamp_cntsr			CNTSR;							// 1D01_(0004 - 0007) Counter Status Register
	unsigned long						CNTCVL;							// 1D01_(0008 - 000B) Current Counter Value Lower register
	unsigned long						CNTCVU;							// 1D01_(000C - 000F) Current Counter Value Upper register
	unsigned char						dmy_0010_001F[0x0020 - 0x0010];	// 1D01_(0010 - 001F) Reserved
	unsigned long						CNTFID0;						// 1D01_(0020 - 0023) Base Frequency ID register
	unsigned char						dmy_0024_0FCF[0x0FD0 - 0x0024];	// 1D01_(0024 - 0FCF) Reserved
	union io_timestamp_pidr4			PIDR4;							// 1D01_(0FD0 - 0FD3) Peripheral ID4 Register
	unsigned char						dmy_0FD4_0FDF[0x0FE0 - 0x0FD4];	// 1D01_(0FD4 - 0FDF) Reserved
	union io_timestamp_pidr0			PIDR0;							// 1D01_(0FE0 - 0FE3) Peripheral ID0 Register
	union io_timestamp_pidr1			PIDR1;							// 1D01_(0FE4 - 0FE7) Peripheral ID1 Register
	union io_timestamp_pidr2			PIDR2;							// 1D01_(0FE8 - 0FEB) Peripheral ID2 Register
	union io_timestamp_pidr3			PIDR3;							// 1D01_(0FEC - 0FEF) Peripheral ID3 Register
	union io_timestamp_cidr0			CIDR0;							// 1D01_(0FF0 - 0FF3) Component ID0 Register
	union io_timestamp_cidr1			CIDR1;							// 1D01_(0FF4 - 0FF7) Component ID1 Register
	union io_timestamp_cidr2			CIDR2;							// 1D01_(0FF8 - 0FFB) Component ID2 Register
	union io_timestamp_cidr3			CIDR3;							// 1D01_(0FFC - 0FFF) Component ID3 Register
};

extern volatile struct io_timestamp		IO_TIMESTAMP;
