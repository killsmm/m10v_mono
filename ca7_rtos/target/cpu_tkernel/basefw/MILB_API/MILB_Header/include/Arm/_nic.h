/**
 * @file		_nic.h
 * @brief		Definition Network InterConnect I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* structure of security1				(1D10_000Ch) */
union nic_security1 {
	unsigned long		word;
	struct {
		unsigned long	SECURITY1		:16;
		unsigned long					:16;
	}bit;
};

/* structure of wr_tidemark				(1D14_2040h, 3040h, 4040h, 5040h, 6040h, 7040h, 8040h) */
union nic_tidemark {
	unsigned long		word;
	struct {
		unsigned long	WT				:4;
		unsigned long					:28;
	}bit;
};

/* structure of read_qos				(1D14_2100h, 3100h, 4100h, 5100h, 6100h, 7100h, 8100h) */
union nic_rq {
	unsigned long		word;
	struct {
		unsigned long	RQ				:4;
		unsigned long					:28;
	}bit;
};

/* structure of write_qos				(1D14_2104h, 3104h, 4104h, 5104h, 6104h, 7104h, 8104h) */
union nic_wq {
	unsigned long		word;
	struct {
		unsigned long	WQ				:4;
		unsigned long					:28;
	}bit;
};

/* structure of fn_mod					(1D14_2108h, 3108h, 4108h, 5108h, 6108h, 7108h, 8108h) */
union nic_fm {
	unsigned long		word;
	struct {
		unsigned long	FM				:2;
		unsigned long					:30;
	}bit;
};

/* Address region control register					*/
struct io_addrctrl {
	unsigned char						dmy_0000_000B[0x000C - 0x0000];	// 1D10_(0000 - 000B) Reserved
	union nic_security1					SECURITY1;						// 1D10_(000C - 000F) Slave 1 security setting Register
	unsigned char						dmy_0010_0FFF[0x1000 - 0x0010];	// 1D10_(0010 - 0FFF) Reserved
};

/* Slave Interface register							*/
struct io_asib {
	unsigned char						dmy_2000_203F[0x2040 - 0x2000];	// 1D14_(2000 - 203F) Reserved
	union nic_tidemark					WR_TIDEMARK;					// 1D14_(2040 - 2043) Tidemark Register
	unsigned char						dmy_2044_20FF[0x2100 - 0x2044];	// 1D14_(2044 - 20FF) Reserved
	union nic_rq						READ_QOS;						// 1D14_(2100 - 2103) Read channel QoS Register
	union nic_wq						WRITE_QOS;						// 1D14_(2104 - 2107) Write channel QoS Register
	union nic_fm						FN_MOD;							// 1D14_(2108 - 210B) Functionality Modification Register
	unsigned char						dmy_210C_2FFF[0x3000 - 0x210C];	// 1D14_(210C - 2FFF) Reserved
};

extern volatile struct io_addrctrl	IO_NIC_ADDRCTRL;
extern volatile struct io_asib		IO_NIC_ASIB[7];
