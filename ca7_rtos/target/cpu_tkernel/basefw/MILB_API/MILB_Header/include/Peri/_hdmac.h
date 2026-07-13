/**
 * @file  _hdmac.h
 * @brief  Definition HDMAC Macro I/O register
 * @note  None
 * @attention None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

union io_hdmac_dmacr {
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

union io_hdmac_dmaca {
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

union io_hdmac_dmacb {
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

struct io_hdmac_element {
	union io_hdmac_dmaca	DMACA;						/* 0-7ch 10,20,30,40,50,60,70,80 */
	union io_hdmac_dmacb	DMACB;						/* 0-7ch 14,24,34,44,54,64,74,84 */
	unsigned long			DMACSA;						/* 0-7ch 18,28,38,48,58,68,78,88 */
	unsigned long			DMACDA;						/* 0-7ch 1C,2C,3C,4C,5C,6C,7C,8C */
};

struct io_hdmac {
	union io_hdmac_dmacr	DMACR;						/* 1E10_(0000 - 0003h) */
	unsigned char			dmy_04_0F[0x10 - 0x04];		/* 1E10_(0004 - 000Fh) */
	struct io_hdmac_element	DMAC[8];					/* 1E10_(0010 - 008Fh) */
};

extern volatile struct io_hdmac		IO_HDMAC;			/* 1B04_0000 */
extern volatile struct io_hdmac		IO_HDMAC0;			/* 1E10_0000 */
extern volatile struct io_hdmac		IO_HDMAC1;			/* 1E11_0000 */

