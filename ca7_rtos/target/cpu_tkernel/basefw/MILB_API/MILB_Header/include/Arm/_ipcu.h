/**
 * @file		_ipcu.h
 * @brief		Definition IPCU (Inter-Processor Communications Unit) I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* structure of ISR				(Base_0000h) */
union io_ipcu_isr {
	unsigned long		word;
	struct {
		unsigned long	MB0 		:1;
		unsigned long	MB1 		:1;
		unsigned long	MB2 		:1;
		unsigned long	MB3 		:1;
		unsigned long	MB4 		:1;
		unsigned long	MB5 		:1;
		unsigned long	MB6 		:1;
		unsigned long	MB7 		:1;
		unsigned long				:8;
		unsigned long				:8;
		unsigned long				:8;
	}bit;
};

/* structure of MBADR			(Base_0080h) */
union io_ipcu_mb_adr {
	unsigned long		word;
	struct {
		unsigned long	MBA 		:12;
		unsigned long	REQ 		:1;
		unsigned long	ACK 		:1;
		unsigned long				:2;
		unsigned long				:16;
	}bit;
};

/* structure of SRC				(Base_0100h) */
union io_ipcu_src {
	unsigned long		word;
	struct {
		unsigned long	UNIT00		:1;
		unsigned long	UNIT01		:1;
		unsigned long	UNIT02		:1;
		unsigned long	UNIT03		:1;
		unsigned long	UNIT04		:1;
		unsigned long	UNIT05		:1;
		unsigned long	UNIT06		:1;
		unsigned long	UNIT07		:1;
		unsigned long	UNIT08		:1;
		unsigned long	UNIT09		:1;
		unsigned long	UNIT10		:1;
		unsigned long	UNIT11		:1;
		unsigned long	UNIT12		:1;
		unsigned long	UNIT13		:1;
		unsigned long	UNIT14		:1;
		unsigned long	UNIT15		:1;
		unsigned long				:16;
	}bit;
};

/* structure of MODE			(Base_0104h) */
union io_ipcu_mode {
	unsigned long		word;
	struct {
		unsigned long	MODE		:3;
		unsigned long				:5;
		unsigned long				:24;
	}bit;
};

/* structure of SEND			(Base_0108h) */
union io_ipcu_send {
	unsigned long		word;
	struct {
		unsigned long	SEND		:1;
		unsigned long				:7;
		unsigned long				:24;
	}bit;
};

/* structure of DSTSET			(Base_0110h) */
union io_ipcu_dst_set {
	unsigned long		word;
	struct {
		unsigned long	UNIT00		:1;
		unsigned long	UNIT01		:1;
		unsigned long	UNIT02		:1;
		unsigned long	UNIT03		:1;
		unsigned long	UNIT04		:1;
		unsigned long	UNIT05		:1;
		unsigned long	UNIT06		:1;
		unsigned long	UNIT07		:1;
		unsigned long	UNIT08		:1;
		unsigned long	UNIT09		:1;
		unsigned long	UNIT10		:1;
		unsigned long	UNIT11		:1;
		unsigned long	UNIT12		:1;
		unsigned long	UNIT13		:1;
		unsigned long	UNIT14		:1;
		unsigned long	UNIT15		:1;
		unsigned long				:16;
	}bit;
};

/* structure of DSTCLR			(Base_0114h) */
union io_ipcu_dst_clr {
	unsigned long		word;
	struct {
		unsigned long	UNIT00		:1;
		unsigned long	UNIT01		:1;
		unsigned long	UNIT02		:1;
		unsigned long	UNIT03		:1;
		unsigned long	UNIT04		:1;
		unsigned long	UNIT05		:1;
		unsigned long	UNIT06		:1;
		unsigned long	UNIT07		:1;
		unsigned long	UNIT08		:1;
		unsigned long	UNIT09		:1;
		unsigned long	UNIT10		:1;
		unsigned long	UNIT11		:1;
		unsigned long	UNIT12		:1;
		unsigned long	UNIT13		:1;
		unsigned long	UNIT14		:1;
		unsigned long	UNIT15		:1;
		unsigned long				:16;
	}bit;
};

/* structure of DSTSTAT			(Base_0118h) */
union io_ipcu_dst_stat {
	unsigned long		word;
	struct {
		unsigned long	UNIT00		:1;
		unsigned long	UNIT01		:1;
		unsigned long	UNIT02		:1;
		unsigned long	UNIT03		:1;
		unsigned long	UNIT04		:1;
		unsigned long	UNIT05		:1;
		unsigned long	UNIT06		:1;
		unsigned long	UNIT07		:1;
		unsigned long	UNIT08		:1;
		unsigned long	UNIT09		:1;
		unsigned long	UNIT10		:1;
		unsigned long	UNIT11		:1;
		unsigned long	UNIT12		:1;
		unsigned long	UNIT13		:1;
		unsigned long	UNIT14		:1;
		unsigned long	UNIT15		:1;
		unsigned long				:16;
	}bit;
};

/* structure of MASKSET			(Base_0120h) */
union io_ipcu_mask_set {
	unsigned long		word;
	struct {
		unsigned long	UNIT00		:1;
		unsigned long	UNIT01		:1;
		unsigned long	UNIT02		:1;
		unsigned long	UNIT03		:1;
		unsigned long	UNIT04		:1;
		unsigned long	UNIT05		:1;
		unsigned long	UNIT06		:1;
		unsigned long	UNIT07		:1;
		unsigned long	UNIT08		:1;
		unsigned long	UNIT09		:1;
		unsigned long	UNIT10		:1;
		unsigned long	UNIT11		:1;
		unsigned long	UNIT12		:1;
		unsigned long	UNIT13		:1;
		unsigned long	UNIT14		:1;
		unsigned long	UNIT15		:1;
		unsigned long				:16;
	}bit;
};

/* structure of MASKCLR			(Base_0124h) */
union io_ipcu_mask_clr {
	unsigned long		word;
	struct {
		unsigned long	UNIT00		:1;
		unsigned long	UNIT01		:1;
		unsigned long	UNIT02		:1;
		unsigned long	UNIT03		:1;
		unsigned long	UNIT04		:1;
		unsigned long	UNIT05		:1;
		unsigned long	UNIT06		:1;
		unsigned long	UNIT07		:1;
		unsigned long	UNIT08		:1;
		unsigned long	UNIT09		:1;
		unsigned long	UNIT10		:1;
		unsigned long	UNIT11		:1;
		unsigned long	UNIT12		:1;
		unsigned long	UNIT13		:1;
		unsigned long	UNIT14		:1;
		unsigned long	UNIT15		:1;
		unsigned long				:16;
	}bit;
};

/* structure of MASKSTAT		(Base_0128h) */
union io_ipcu_mask_stat {
	unsigned long		word;
	struct {
		unsigned long	UNIT00		:1;
		unsigned long	UNIT01		:1;
		unsigned long	UNIT02		:1;
		unsigned long	UNIT03		:1;
		unsigned long	UNIT04		:1;
		unsigned long	UNIT05		:1;
		unsigned long	UNIT06		:1;
		unsigned long	UNIT07		:1;
		unsigned long	UNIT08		:1;
		unsigned long	UNIT09		:1;
		unsigned long	UNIT10		:1;
		unsigned long	UNIT11		:1;
		unsigned long	UNIT12		:1;
		unsigned long	UNIT13		:1;
		unsigned long	UNIT14		:1;
		unsigned long	UNIT15		:1;
		unsigned long				:16;
	}bit;
};

/* structure of ACKSET			(Base_0130h) */
union io_ipcu_ack_set {
	unsigned long		word;
	struct {
		unsigned long	UNIT00		:1;
		unsigned long	UNIT01		:1;
		unsigned long	UNIT02		:1;
		unsigned long	UNIT03		:1;
		unsigned long	UNIT04		:1;
		unsigned long	UNIT05		:1;
		unsigned long	UNIT06		:1;
		unsigned long	UNIT07		:1;
		unsigned long	UNIT08		:1;
		unsigned long	UNIT09		:1;
		unsigned long	UNIT10		:1;
		unsigned long	UNIT11		:1;
		unsigned long	UNIT12		:1;
		unsigned long	UNIT13		:1;
		unsigned long	UNIT14		:1;
		unsigned long	UNIT15		:1;
		unsigned long				:16;
	}bit;
};

/* structure of ACKCLR			(Base_0134h) */
union io_ipcu_ack_clr {
	unsigned long		word;
	struct {
		unsigned long	UNIT00		:1;
		unsigned long	UNIT01		:1;
		unsigned long	UNIT02		:1;
		unsigned long	UNIT03		:1;
		unsigned long	UNIT04		:1;
		unsigned long	UNIT05		:1;
		unsigned long	UNIT06		:1;
		unsigned long	UNIT07		:1;
		unsigned long	UNIT08		:1;
		unsigned long	UNIT09		:1;
		unsigned long	UNIT10		:1;
		unsigned long	UNIT11		:1;
		unsigned long	UNIT12		:1;
		unsigned long	UNIT13		:1;
		unsigned long	UNIT14		:1;
		unsigned long	UNIT15		:1;
		unsigned long				:16;
	}bit;
};

/* structure of ACKSTAT			(Base_0138h) */
union io_ipcu_ack_stat {
	unsigned long		word;
	struct {
		unsigned long	UNIT00		:1;
		unsigned long	UNIT01		:1;
		unsigned long	UNIT02		:1;
		unsigned long	UNIT03		:1;
		unsigned long	UNIT04		:1;
		unsigned long	UNIT05		:1;
		unsigned long	UNIT06		:1;
		unsigned long	UNIT07		:1;
		unsigned long	UNIT08		:1;
		unsigned long	UNIT09		:1;
		unsigned long	UNIT10		:1;
		unsigned long	UNIT11		:1;
		unsigned long	UNIT12		:1;
		unsigned long	UNIT13		:1;
		unsigned long	UNIT14		:1;
		unsigned long	UNIT15		:1;
		unsigned long				:16;
	}bit;
};

/* structure of ACKSRC			(Base_013Ch) */
union io_ipcu_ack_src {
	unsigned long		word;
	struct {
		unsigned long	UNIT00		:1;
		unsigned long	UNIT01		:1;
		unsigned long	UNIT02		:1;
		unsigned long	UNIT03		:1;
		unsigned long	UNIT04		:1;
		unsigned long	UNIT05		:1;
		unsigned long	UNIT06		:1;
		unsigned long	UNIT07		:1;
		unsigned long	UNIT08		:1;
		unsigned long	UNIT09		:1;
		unsigned long	UNIT10		:1;
		unsigned long	UNIT11		:1;
		unsigned long	UNIT12		:1;
		unsigned long	UNIT13		:1;
		unsigned long	UNIT14		:1;
		unsigned long	UNIT15		:1;
		unsigned long				:16;
	}bit;
};

/* structure of MBSTAT			(Base_0900h) */
union io_ipcu_mb_stat {
	unsigned long		word;
	struct {
		unsigned long	MB0 		:1;
		unsigned long	MB1 		:1;
		unsigned long	MB2 		:1;
		unsigned long	MB3 		:1;
		unsigned long	MB4 		:1;
		unsigned long	MB5 		:1;
		unsigned long	MB6 		:1;
		unsigned long	MB7 		:1;
		unsigned long				:24;
	}bit;
};

struct io_ipcu_mailbox {
	union io_ipcu_src				SRC;							/* Base_(0100 - 0103h) RW Source Register				*/
	union io_ipcu_mode				MODE;							/* Base_(0104 - 0107h) RW Mode Register					*/
	union io_ipcu_send				SEND;							/* Base_(0108 - 010Bh) RW Send Register					*/
	unsigned char					dmy_010C_010F[0x0110 - 0x010C];	/* Base_(010C - 010Fh) Reserved							*/
	union io_ipcu_dst_set			DSTSET;							/* Base_(0110 - 0113h) WO Destination Set Register		*/
	union io_ipcu_dst_clr			DSTCLR;							/* Base_(0114 - 0117h) WO Destination Clear Register	*/
	union io_ipcu_dst_stat			DSTSTAT;						/* Base_(0118 - 011Bh) RO Destination Status Register	*/
	unsigned char					dmy_011C_011F[0x0120 - 0x011C];	/* Base_(011C - 011Fh) Reserved							*/
	union io_ipcu_mask_set			MASKSET;						/* Base_(0120 - 0123h) WO Mask Set Register				*/
	union io_ipcu_mask_clr			MASKCLR;						/* Base_(0124 - 0127h) WO Mask Clear Register			*/
	union io_ipcu_mask_stat			MASKSTAT;						/* Base_(0128 - 012Bh) RO Mask Status Register			*/
	unsigned char					dmy_012C_0130[0x0130 - 0x012C];	/* Base_(012C - 012Fh) Reserved							*/
	union io_ipcu_ack_set			ACKSET;							/* Base_(0130 - 0133h) WO Acknowledge Set Register		*/
	union io_ipcu_ack_clr			ACKCLR;							/* Base_(0134 - 0137h) WO Acknowledge Clear Register	*/
	union io_ipcu_ack_stat			ACKSTAT;						/* Base_(0138 - 013Bh) RO Acknowledge Status Register	*/
	union io_ipcu_ack_src			ACKSRC;							/* Base_(013C - 013Fh) RO Acknowledge Source Register	*/
	unsigned long					DATA[9];						/* Base_(0140 - 0163h) RW Data Register					*/
	unsigned char					dmy_0164_017F[0x0180 - 0x0164];	
};

/*	IPCU register							*/
struct io_ipcu {
	union io_ipcu_isr				ISR[16];						/* Base_(0000 - 003Fh) RO Interrupt Status Register		*/
	unsigned char					dmy_0040_007F[0x0080 - 0x0040];	/* Base_(0040 - 007Fh) Reserved							*/
	union io_ipcu_mb_adr			MBADR[16];						/* Base_(0080 - 00BFh) RO Mailbox Address Register		*/
	unsigned char					dmy_00C0_00FF[0x0100 - 0x00C0];	/* Base_(00C0 - 00FFh) Reserved							*/
	struct io_ipcu_mailbox			MAILBOX[8];						/* Base_(0100 - 04FFh) Mailbox Register					*/
	unsigned char					dmy_0500_08FF[0x0900 - 0x0500];	/* Base_(0500 - 08FFh) Reserved							*/
	union  io_ipcu_mb_stat			MBSTAT;							/* Base_(0900 - 0903h) RO Mailbox Status Register		*/
};

extern volatile struct io_ipcu		IO_IPCU0;						/* Base(IPCU0) : 0x1C25_1000 */
extern volatile struct io_ipcu		IO_IPCU1;						/* Base(IPCU1) : 0x1C25_2000 */
