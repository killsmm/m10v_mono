/**
 * @file		_slimbus.h
 * @brief		Definition SLIMbus Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* SLIMbus register */

/* structure of CONFIG_MODE				(Base_0000h) */
union io_slimbus_mode {
	unsigned long		word;
	struct {
		unsigned long	ENABLE				:1;
		unsigned long	MANAGER_MODE		:1;
		unsigned long	SNIFFER_MODE		:1;
		unsigned long	FR_EN				:1;
		unsigned long	DEV_EN				:1;
		unsigned long	GO_ABSENT			:1;
		unsigned long						:2;
		unsigned long	RETRY_LMT			:4;
		unsigned long	REPORT_AT_EVENT		:1;
		unsigned long	CRC_CALC_DISABLE	:1;
		unsigned long	LMTD_REPORT			:1;
		unsigned long	RECONF_TX_DIS		:1;
		unsigned long						:16;
	} bit;
};

/* structure of CONFIG_EA				(Base_0004h) */
union io_slimbus_ea {
	unsigned long		word;
	struct {
		unsigned long	PRODUCT_ID			:16;
		unsigned long	INSTANCE_VAL		:8;
		unsigned long						:8;
	} bit;
};

/* structure of CONFIG_PR_TP			(Base_0008h) */
union io_slimbus_prtp {
	unsigned long		word;
	struct {
		unsigned long	PR_SUPP				:24;
		unsigned long	TP_SUPP				:3;
		unsigned long						:5;
	} bit;
};

/* structure of CONFIG_FR				(Base_000Ch) */
union io_slimbus_fr {
	unsigned long		word;
	struct {
		unsigned long	RF_SUPP				:16;
		unsigned long	QUALITY				:2;
		unsigned long	PAUSE_AT_RFCHNG		:1;
		unsigned long						:13;
	} bit;
};

/* structure of CONFIG_DPORT			(Base_0010h) */
union io_slimbus_dport {
	unsigned long		word;
	struct {
		unsigned long	SINK_START_LVL		:8;
		unsigned long	DPORT_CLK_PRESC		:4;
		unsigned long	REFCLK_SEL			:2;
		unsigned long						:18;
	} bit;
};

/* structure of CONFIG_EA2				(Base_0018h) */
union io_slimbus_ea2 {
	unsigned long		word;
	struct {
		unsigned long	DEVICE_ID_1			:8;
		unsigned long	DEVICE_ID_2			:8;
		unsigned long	DEVICE_ID_3			:8;
		unsigned long						:8;
	} bit;
};

/* structure of CONFIG_THR				(Base_001Ch) */
union io_slimbus_thr {
	unsigned long		word;
	struct {
		unsigned long	SRC_THR				:6;
		unsigned long						:10;
		unsigned long	SINK_THR			:6;
		unsigned long						:10;
	} bit;
};

/* structure of COMMAND					(Base_0020h) */
union io_slimbus_command {
	unsigned long		word;
	struct {
		unsigned long	TX_PUSH				:1;
		unsigned long	RX_PULL				:1;
		unsigned long	TX_CLR				:1;
		unsigned long	UNFREEZE			:1;
		unsigned long	CFG_STROBE			:1;
		unsigned long	CFG_STROBE_CLR		:1;
		unsigned long						:26;
	} bit;
};

/* structure of STATE					(Base_0024h) */
union io_slimbus_state {
	unsigned long		word;
	struct {
		unsigned long	TX_FULL				:1;
		unsigned long	TX_NOTEMPTY			:1;
		unsigned long	RX_NOTEMPTY			:1;
		unsigned long						:1;
		unsigned long	TX_PUSH				:1;
		unsigned long	RX_PULL				:1;
		unsigned long						:2;
		unsigned long	F_SYNC				:1;
		unsigned long	SF_SYNC				:1;
		unsigned long	M_SYNC				:1;
		unsigned long	SFB_SYNC			:1;
		unsigned long	PH_SYNC				:1;
		unsigned long						:2;
		unsigned long	DETACHED			:1;
		unsigned long	SUBFRAME_MODE		:5;
		unsigned long						:3;
		unsigned long	CLOCK_GEAR			:4;
		unsigned long	ROOT_FR				:4;
	} bit;
};

/* structure of IE_STATE				(Base_0028h) */
union io_slimbus_iestate {
	unsigned long		word;
	struct {
		unsigned long	EX_ERROR_IF			:1;
		unsigned long						:1;
		unsigned long	UNSPRTD_MSG_IF		:1;
		unsigned long	MC_TX_COL			:1;
		unsigned long	LOST_FS				:1;
		unsigned long	LOST_SFS			:1;
		unsigned long	LOST_MS				:1;
		unsigned long	DATA_SLOT_OVERLAP	:1;
		unsigned long	EX_ERROR_FR			:1;
		unsigned long	RCFG_OBJECTION		:1;
		unsigned long	UNSPRTD_MSG_FR		:1;
		unsigned long	ACTIVE_FRAMER		:1;
		unsigned long	FS_TX_COL			:1;
		unsigned long	FI_TX_COL			:1;
		unsigned long	GC_TX_COL			:1;
		unsigned long						:1;
		unsigned long	EX_ERROR_DEV		:1;
		unsigned long	DATA_TX_COL			:1;
		unsigned long	UNSPRTD_MSG_DEV		:1;
		unsigned long						:13;
	} bit;
};

/* structure of MCH_USAGE				(Base_002Ch) */
union io_slimbus_mchusage {
	unsigned long		word;
	struct {
		unsigned long	MCH_USAGE			:11;
		unsigned long						:5;
		unsigned long	MCH_CAPACITY		:11;
		unsigned long	MCH_LAPSE			:5;
	} bit;
};

/* structure of INT_EN					(Base_0038h) */
union io_slimbus_inten {
	unsigned long		word;
	struct {
		unsigned long	IN_EN				:1;
		unsigned long	RX_INT_EN			:1;
		unsigned long	TX_INT_EN			:1;
		unsigned long	TX_ERR_EN			:1;
		unsigned long	SYNC_LOST_EN		:1;
		unsigned long	RCFG_INT_EN			:1;
		unsigned long	MCH_INT_EN			:1;
		unsigned long						:25;
	} bit;
};

/* structure of INT						(Base_003Ch) */
union io_slimbus_int {
	unsigned long		word;
	struct {
		unsigned long						:1;
		unsigned long	__RX_INT			:1;
		unsigned long	__TX_INT			:1;
		unsigned long	__TX_ERR			:1;
		unsigned long	__SYNC_LOST			:1;
		unsigned long	__RCFG_INT			:1;
		unsigned long	__MCH_INT			:1;
		unsigned long	__PORT_INT			:1;
		unsigned long						:24;
	} bit;
};

/* structure of P0_INT_EN				(Base_0080h) */
union io_slimbus_p0inten {
	unsigned long		word;
	struct {
		unsigned long	ACT_INT_EN			:1;
		unsigned long	CON_INT_EN			:1;
		unsigned long	CHAN_INT_EN			:1;
		unsigned long	DMA_INT_EN			:1;
		unsigned long	OVF_INT_EN			:1;
		unsigned long	UND_INT_EN			:1;
		unsigned long	FIFO_CLR			:1;
		unsigned long	PR_GEN_EN			:1;
		unsigned long						:24;
	} bit;
};

/* structure of RX_FIFO_FLAG	*/
union io_slimbus_rxfifoflag {
	unsigned long		word;
	struct {
		unsigned long	MSG_SIZE			:6;
		unsigned long						:2;
		unsigned long	RX_OVERFLOW			:1;
		unsigned long	TX_ERR				:7;
		unsigned long	LOST_MSG_NUM		:16;
	} bit;
};

/* structure of P0_INT					(Base_00C0h) */
union io_slimbus_p0int {
	unsigned long		word;
	struct {
		unsigned long	__ACT_INT			:1;
		unsigned long	__CON_INT			:1;
		unsigned long	__CHAN_INT			:1;
		unsigned long	__DMA_INT			:1;
		unsigned long	__OVF_INT			:1;
		unsigned long	__UND_INT			:1;
		unsigned long						:26;
	} bit;
};

/* structure of P0_STATE_0				(Base_0100h) */
union io_slimbus_p0state0 {
	unsigned long		word;
	struct {
		unsigned long	ACTIVE				:1;
		unsigned long	CONTENT_DEFINED		:1;
		unsigned long	CHANNEL_DEFINED		:1;
		unsigned long	SINK				:1;
		unsigned long	OVF					:1;
		unsigned long	UND					:1;
		unsigned long	DPORT_READY			:1;
		unsigned long						:9;
		unsigned long	S_DISTR				:12;
		unsigned long	TR_PROTOCOL			:4;
	} bit;
};

/* structure of P0_STATE_1				(Base_0104h) */
union io_slimbus_p0state1 {
	unsigned long		word;
	struct {
		unsigned long	P_RATE				:7;
		unsigned long	FR_LOCK				:1;
		unsigned long	DATA_TYPE			:4;
		unsigned long						:4;
		unsigned long	DATA_LENGTH			:6;
		unsigned long						:2;
		unsigned long	PORT_LINKED			:6;
		unsigned long						:1;
		unsigned long	CH_LINK				:1;
	} bit;
};


/* Define I/O mapping		*/
/* Base(SLIMbus ch0) 1E30_(0000 - 103Fh) */
/* Base(SLIMbus ch1) 1E40_(0000 - 103Fh) */
/* Base(SLIMbus ch2) 1E90_(0000 - 103Fh) */
/* Base(SLIMbus ch3) 1EA0_(0000 - 103Fh) */
struct io_slimbus {
	union io_slimbus_mode		CONFIG_MODE;				/* Base_(0000 - 0003h) */
	union io_slimbus_ea			CONFIG_EA;					/* Base_(0004 - 0007h) */
	union io_slimbus_prtp		CONFIG_PR_TP;				/* Base_(0008 - 000Bh) */
	union io_slimbus_fr			CONFIG_FR;					/* Base_(000C - 000Fh) */
	union io_slimbus_dport		CONFIG_DPORT;				/* Base_(0010 - 0013h) */
	unsigned char			dmy_0014_0017[0x0018-0x0014];	/* Base_(0014 - 0017h) */
	union io_slimbus_ea2		CONFIG_EA2;					/* Base_(0018 - 001Bh) */
	union io_slimbus_thr		CONFIG_THR;					/* Base_(001C - 001Fh) */
	union io_slimbus_command	COMMAND;					/* Base_(0020 - 0023h) */
	union io_slimbus_state		STATE;						/* Base_(0024 - 0027h) */
	union io_slimbus_iestate	IE_STATE;					/* Base_(0028 - 002Bh) */
	union io_slimbus_mchusage	MCH_USAGE;					/* Base_(002C - 002Fh) */
	unsigned char			dmy_0030_0037[0x0038-0x0030];	/* Base_(0030 - 0037h) */
	union io_slimbus_inten		INT_EN;						/* Base_(0038 - 003Bh) */
	union io_slimbus_int		INT;						/* Base_(003C - 003Fh) */
	unsigned long				MC_FIFO[16];				/* Base_(0040 - 007Fh) */
	union io_slimbus_p0inten	P0_INT_EN;					/* Base_(0080 - 0083h) */
	unsigned char			dmy_0084_00BF[0x00C0-0x0084];	/* Base_(0084 - 00BFh) */
	union io_slimbus_p0int		P0_INT;						/* Base_(00C0 - 00C3h) */
	unsigned char			dmy_00C4_00FF[0x0100-0x00C4];	/* Base_(00C4 - 00FFh) */
	union io_slimbus_p0state0	P0_STATE_0;					/* Base_(0100 - 0103h) */
	union io_slimbus_p0state1	P0_STATE_1;					/* Base_(0104 - 0107h) */
	unsigned char			dmy_0108_0FFF[0x1000-0x0108];	/* Base_(0108 - 0FFFh) */
	unsigned long				P0_FIFO[64];				/* Base_(1000 - 10FFh) */
};


extern volatile struct io_slimbus	 IO_SLIMBUS0;
extern volatile struct io_slimbus	 IO_SLIMBUS1;
extern volatile struct io_slimbus	 IO_SLIMBUS2;
extern volatile struct io_slimbus	 IO_SLIMBUS3;

