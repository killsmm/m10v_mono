/**
 * @file		_pcie.h
 * @brief		Definition JMLEXS_PCIE Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*----------------------------------------------*/
/*	Core Register								*/
/*----------------------------------------------*/
/* Device ID and Vendor ID (Offset 0000h) */
union io_dev_id_vend_id {
	unsigned long		word;
	struct {
		unsigned long	VID				:16;
		unsigned long	DID				:16;
	} bit;
};

/* Status Command (Offset 0004h) */
union io_status_command {
	unsigned long		word;
	struct {
		unsigned long	IOS				:1;
		unsigned long	MS				:1;
		unsigned long	BME				:1;
		unsigned long					:3;
		unsigned long	PER				:1;
		unsigned long					:1;
		unsigned long	SERRE			:1;
		unsigned long					:1;
		unsigned long	ID				:1;
		unsigned long					:8;
		unsigned long	IS				:1;
		unsigned long	CL				:1;
		unsigned long					:3;
		unsigned long	MDPE			:1;
		unsigned long					:2;
		unsigned long	STA				:1;
		unsigned long	RTA				:1;
		unsigned long	RMA				:1;
		unsigned long	SSE				:1;
		unsigned long	DPE				:1;
	} bit;
};

/* Class Code and Revision ID (Offset 0008h) */
union io_class_code_rev_id {
	unsigned long		word;
	struct {
		unsigned long	RID 			:8;
		unsigned long	PI				:8;
		unsigned long	SCC				:8;
		unsigned long	BCC				:8;
	} bit;
};

/* Header Type and Cache Line Size (Offset 000Ch) */
union io_hdr_type_cache_line_size {
	unsigned long		word;
	struct {
		unsigned long	CLS 			:8;
		unsigned long					:8;
		unsigned long	HT 				:7;
		unsigned long	MFS				:1;
		unsigned long					:8;
	} bit;
};

/* Primary, Secondary, Subordinate Bus Numbers and Latency Timer (Offset 0018h) */
union io_sec_lat_timer_sub_bus_sec_bus_pri_bus {
	unsigned long		word;
	struct {
		unsigned long	PBN 			:8;
		unsigned long	SUBBN			:8;
		unsigned long	SECBN			:8;
		unsigned long	SLT				:8;
	} bit;
};

/* Secondary Status and IO Limit and IO Base (Offset 001Ch) <RC> */
union io_sec_stat_io_limit_io_base {
	unsigned long		word;
	struct {
		unsigned long	IOBA_IOA 		:1;
		unsigned long		 			:3;
		unsigned long	IOBA			:4;
		unsigned long	IOL_IOA 		:1;
		unsigned long		 			:3;
		unsigned long	IOL				:4;
		unsigned long					:8;
		unsigned long	MDPE			:1;
		unsigned long					:2;
		unsigned long	STA				:1;
		unsigned long	RTA				:1;
		unsigned long	RMA				:1;
		unsigned long	RSE				:1;
		unsigned long	DPE				:1;
	} bit;
};

/* Memory Limit and Memory Base (Offset 0020h) <RC> */
union io_mem_limit_mem_base {
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	MBA				:12;
		unsigned long					:4;
		unsigned long	ML				:12;
	} bit;
};


/* Prefetchable Memory Limit and Prefetchable Memory Base Address (Offset 0024h) <RC> */
union io_pref_mem_limit_pref_mem_base {
	unsigned long		word;
	struct {
		unsigned long	PMD				:1;
		unsigned long					:3;
		unsigned long	PMBA			:12;
		unsigned long	PMLD			:1;
		unsigned long					:3;
		unsigned long	PML				:12;
	} bit;
};

/* Prefetchable Base Upper (Offset 0028h) <RC> */
union io_pref_base_upper {
	unsigned long		word;
	struct {
		unsigned long	PBAU32			:32;
	} bit;
};

/* Prefetchable Limit Upper (Offset 002Ch) <RC> */
union io_pref_limit_upper {
	unsigned long		word;
	struct {
		unsigned long	PMLU32			:32;
	} bit;
};

/* IO Limit Upper and IO Base Upper (Offset 0030h) <RC> */
union io_io_limit_upper_io_base_upper {
	unsigned long		word;
	struct {
		unsigned long	IOBAU16			:16;
		unsigned long	IOLU16			:16;
	} bit;
};

/* Capabilities Pointer (Offset 0034h) */
union io_cap_ptr {
	unsigned long		word;
	struct {
		unsigned long	NCP				:8;		/* New Capabilities Pointer */
		unsigned long					:24;
	} bit;
};

/* Expansion ROM Base Address (RC:Offset 0038h, EP: Offset 0030h) */
union io_exp_rom_bar {
	unsigned long		word;
	struct {
		unsigned long	EROME			:1;
		unsigned long					:10;
		unsigned long	BA				:21;
	} bit;
};

/* Bridge Control Interrupt Pin and Interrupt Line (Offset 003Ch) <RC> */
union io_bridge_ctrl_int_pin_int_line {
	unsigned long		word;
	struct {
		unsigned long	IL	 			:8;
		unsigned long	IP				:8;
		unsigned long	PER				:1;
		unsigned long	SERRE			:1;
		unsigned long	ISAE			:1;
		unsigned long	VGAE			:1;
		unsigned long	VGA16D			:1;
		unsigned long					:1;
		unsigned long	SBR				:1;
		unsigned long					:9;
	} bit;
};

/* Type1 Configuration Space Header */
struct io_t1csh {
	union io_dev_id_vend_id							DEV_ID_VEND_ID;							/* +0000 - 0003h */
	union io_status_command							STATUS_COMMAND;							/* +0004 - 0007h */
	union io_class_code_rev_id						CLASS_CODE_REV_ID;						/* +0008 - 000Bh */
	union io_hdr_type_cache_line_size				HDR_TYPE_CACHE_LINE_SIZE;				/* +000C - 000Fh */
	unsigned char									dmy_0010_0017[0x0018-0x0010];			/* +0010 - 0017h */
	union io_sec_lat_timer_sub_bus_sec_bus_pri_bus	SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS;	/* +0018 - 001Bh */
	union io_sec_stat_io_limit_io_base				SEC_STAT_IO_LIMIT_IO_BASE;				/* +001C - 001Fh */
	union io_mem_limit_mem_base						MEM_LIMIT_MEM_BASE;						/* +0020 - 0023h */
	union io_pref_mem_limit_pref_mem_base			PREF_MEM_LIMIT_PREF_MEM_BASE;			/* +0024 - 0027h */
	union io_pref_base_upper						PREF_BASE_UPPER;						/* +0028 - 002Bh */
	union io_pref_limit_upper						PREF_LIMIT_UPPER;						/* +002C - 002Fh */
	union io_io_limit_upper_io_base_upper			IO_LIMIT_UPPER_IO_BASE_UPPER;			/* +0030 - 0033h */
	union io_cap_ptr								CAP_PTR;								/* +0034 - 0037h */
	union io_exp_rom_bar							EXP_ROM_BAR;							/* +0038 - 003Bh */
	union io_bridge_ctrl_int_pin_int_line			BRIDGE_CTRL_INT_PIN_INT_LINE;			/* +003C - 003Fh */
};

/* Base Address Register 0 (Offset 010h) <EP> */
/* Base Address Register 1 (Offset 014h) <EP> */
/* Base Address Register 2 (Offset 018h) <EP> */
/* Base Address Register 3 (Offset 01Ch) <EP> */
/* Base Address Register 4 (Offset 020h) <EP> */
/* Base Address Register 5 (Offset 024h) <EP> */
union io_bar {
	unsigned long		word;
	struct		{
		unsigned long	MIOSI			:1;
		unsigned long	MT				:2;
		unsigned long	PF				:1;
		unsigned long	BA				:28;
	} bit;
};

/* CardBus CIS Pointer (Offset 0028h) <EP> */
union io_cardbus_cis_ptr {
	unsigned long		word;
	struct {
		unsigned long	CCP	 			:32;
	} bit;
};

/* Subsystem ID and Subsystem Vendor ID (Offset 002Ch) <EP> */
union io_subsys_id_subsys_v_id {
	unsigned long		word;
	struct {
		unsigned long	SVID	 		:16;
		unsigned long	SID	 			:16;
	} bit;
};

/* Interrupt Pin and Interrupt Line (Offset 003Ch) <EP> */
union io_int_pin_int_line {
	unsigned long		word;
	struct {
		unsigned long	IL	 			:8;
		unsigned long	IP				:8;
		unsigned long					:16;
	} bit;
};

/* Type0 Configuration Space Header */
struct io_t0csh {
	union io_dev_id_vend_id				DEV_ID_VEND_ID;					/* +0000 - 0003h */
	union io_status_command				STATUS_COMMAND;					/* +0004 - 0007h */
	union io_class_code_rev_id			CLASS_CODE_REV_ID;				/* +0008 - 000Bh */
	union io_hdr_type_cache_line_size	HDR_TYPE_CACHE_LINE_SIZE;		/* +000C - 000Fh */
	union io_bar						BAR[6];							/* +0010 - 0027h */
	union io_cardbus_cis_ptr			CARDBUS_CIS_PTR;				/* +0028 - 002Bh */
	union io_subsys_id_subsys_v_id		SUBSYS_ID_SUBSYS_V_ID;			/* +002C - 002Fh */
	union io_exp_rom_bar				EXP_ROM_BAR;					/* +0030 - 0033h */
	union io_cap_ptr					CAP_PTR;						/* +0034 - 0037h */
	unsigned char						dmy_0038_003B[0x003C-0x0038];	/* +0038 - 003Bh */
	union io_int_pin_int_line			INT_PIN_INT_LINE;				/* +003C - 003Fh */
};

/* Power Management Capabilities (Offset 0040h) */
union io_pmcr_pmc {
	unsigned long		word;
	struct {
		unsigned long	CID				: 8;
		unsigned long	NCP				: 8;
		unsigned long	VER				: 3;
		unsigned long					: 1;
		unsigned long					: 1;
		unsigned long	DSI				: 1;
		unsigned long	AC				: 3;
		unsigned long	D1S				: 1;
		unsigned long	D2S				: 1;
		unsigned long	PMES			: 5;
	} bit;
};

/* Power Management Control / Status (Offset 0044h) */
union io_pmcr_pmcs {
	unsigned long		word;
	struct {
		unsigned long	PS				:2;
		unsigned long					:1;
		unsigned long	NSR				:1;
		unsigned long					:4;
		unsigned long	PMEEN			:1;
		unsigned long	DSEL			:4;
		unsigned long	DS				:2;
		unsigned long	PMES			:1;
		unsigned long					:6;
		unsigned long	B2B3S			:1;
		unsigned long	BPCCE			:1;
		unsigned long	DATA			:8;
	} bit;
};

/* Power Management Capabilities Register */
struct io_pmcr {
	union io_pmcr_pmc		PMC;							/* +0040 - 0043h */
	union io_pmcr_pmcs		PMCS;							/* +0044 - 0047h */
	unsigned char			dmy_0048_004F[0x0050-0x0048];	/* +0048 - 004Fh */
};

/* MSI Cap ID / Next Item / Message Control (Offset 050h) */
union io_msicr_mcinc {
	unsigned long		word;
	struct		{
		unsigned long	CID				:8;
		unsigned long	NP				:8;
		unsigned long	MSIE			:1;
		unsigned long	MMC				:3;
		unsigned long	MME				:3;
		unsigned long	AC64			:1;
		unsigned long	PVMC			:1;
		unsigned long					:7;
	} bit;
};

/* Message Address (Offset 054h) */
union io_msicr_mla {
	unsigned long		word;
	struct		{
		unsigned long					:2;
		unsigned long	MLA				:30;
	} bit;
};

/* Message Upper Address (Offset 058h) */
union io_msicr_mua {
	unsigned long		word;
	struct		{
		unsigned long	MUA				:32;
	} bit;
};

/* Message Data (Offset 05Ch) */
union	io_msicr_md	{
	unsigned long		word;
	struct		{
		unsigned long	MD				:16;
		unsigned long					:16;
	} bit;
};

/* Mask Bits (Offset 060h) */
union io_msicr_mmb {
	unsigned long		word;
	struct		{
		unsigned long	MMB				:32;
	} bit;
};

/* Pending Bits (Offset 064h) */
union io_msicr_mpb {
	unsigned long		word;
	struct		{
		unsigned long	MPB				:32;
	} bit;
};

/* MSI Capability Register */
struct io_msicr {
	union io_msicr_mcinc	MSI_CAP_ID_NEXT_CTRL;		/* +0050 - 0053h */
	union io_msicr_mla		MSI_LOW_ADR;				/* +0054 - 0057h */
	union io_msicr_mua		MSI_UPP_ADR;				/* +0058 - 005Bh */
	union io_msicr_md		MSI_DATA;					/* +005C - 005Fh */
	union io_msicr_mmb		MSI_MASK_BITS;				/* +0060 - 0063h */
	union io_msicr_mpb		MSI_PENDING_BITS;			/* +0064 - 0067h */
};

/* Cap ID / Next Item (Offset 0070h) */
/* PCI Express Capability Register (Offset 0072h) */
union io_pciecr_cr {
	unsigned long		word;
	struct {
		unsigned long	CID				:8;
		unsigned long	NP				:8;

		unsigned long	CV				:4;
		unsigned long	DPT				:4;
		unsigned long	SI				:1;
		unsigned long	IMN				:5;
		unsigned long					:2;
	} bit;
};

/* Device Capability Register (Offset 0074h) */
union io_pciecr_dcr {
	unsigned long		word;
	struct {
		unsigned long	MPSS			:3;
		unsigned long	PFS				:2;
		unsigned long	ETFS			:1;
		unsigned long	EL0AL			:3; // EP only
		unsigned long	EL1AL			:3; // EP only
		unsigned long					:3;
		unsigned long	RBER			:1;
		unsigned long					:2;
		unsigned long	CSPLV			:8; // EP only
		unsigned long	CSPLS			:2; // EP only
		unsigned long	FLRC			:1; // EP only
		unsigned long					:3;
	} bit;
};

/* Device Control Register (Offset 0078h)  */
/* Device Status Register (Offset 007Ah)  */
union io_pciecr_dcsr {
	unsigned long		word;
	struct {
		unsigned long	CERE			:1;
		unsigned long	NFERE			:1;
		unsigned long	FERE			:1;
		unsigned long	URRE			:1;
		unsigned long	ERO				:1;
		unsigned long	MPS				:3;
		unsigned long	ETFE			:1;
		unsigned long	PFE				:1;
		unsigned long	APPME			:1;
		unsigned long	ENS				:1;
		unsigned long	MRRS			:3;
		unsigned long	IFLR			:1;
                                
		unsigned long	CED				:1;
		unsigned long	NFED			:1;
		unsigned long	FED				:1;
		unsigned long	URD				:1;
		unsigned long	AUXPD			:1;
		unsigned long	TP				:1;
		unsigned long					:10;
	} bit;
};

/* Link Capability Register (Offset 007Ch)  */
union io_pciecr_lcr {
	unsigned long		word;
	struct {
		unsigned long	MLS				:4;
		unsigned long	MLW				:6;
		unsigned long	ASPMS			:2;
		unsigned long	L0EL			:3;
		unsigned long	L1EL			:3;
		unsigned long	CPM				:1;
		unsigned long	SDERC			:1;
		unsigned long	DLLLARC			:1;
		unsigned long	LBNC			:1;
		unsigned long	ASPMOC			:1;
		unsigned long					:1;
		unsigned long	PN				:8;
	} bit;
};

/* Link Control Register (Offset 0080h) */
/* Link Status Register (Offset 0082h) */
union io_pciecr_lcsr {
	unsigned long		word;
	struct {
		unsigned long	ASPMC			:2;
		unsigned long					:1;
		unsigned long	RCB				:1;
		unsigned long	LD				:1;
		unsigned long	RL				:1;
		unsigned long	CCC				:1;
		unsigned long	ES				:1;
		unsigned long	ECPM			:1;
		unsigned long	HAWD			:1;
		unsigned long	LBMIE			:1;
		unsigned long	LABIE			:1;
		unsigned long					:2;
		unsigned long	DRSSC			:2;

		unsigned long	CLS				:4;
		unsigned long	NLW				:6;
		unsigned long					:1;
		unsigned long	LT				:1;
		unsigned long	SCC				:1;
		unsigned long	DLLLA			:1;
		unsigned long	LBMS			:1;
		unsigned long	LABS			:1;
	} bit;
};

/* Slot Capabilities Register (Offset 0084h) <RC> */
union io_pciecr_scr {
	unsigned long		word;
	struct {
		unsigned long	ABP				:1;
		unsigned long	PCP				:1;
		unsigned long	MRLSP			:1;
		unsigned long	AIP				:1;
		unsigned long	PIP				:1;
		unsigned long	HPS				:1;
		unsigned long	HPC				:1;
		unsigned long	SPLV			:8;
		unsigned long	SPLS			:2;
		unsigned long	EIP				:1;
		unsigned long	NCCS			:1;
		unsigned long	PSN				:13;
	} bit;
};

/* Slot Control Register (Offset 0088h) <RC> */
/* Slot Status Register (Offset 008Ah) <RC> */
union io_pciecr_scsr {
	unsigned long		word;
	struct {
		unsigned long	ABPE			:1;
		unsigned long	PFDE			:1;
		unsigned long	MRLSCE			:1;
		unsigned long	PDCE			:1;
		unsigned long	CCIE			:1;
		unsigned long	HPIE			:1;
		unsigned long	AIC				:2;
		unsigned long	PIC				:2;
		unsigned long	PCC				:1;
		unsigned long	EIC				:1;
		unsigned long	DLLSCE			:1;
		unsigned long					:3;

		unsigned long	ABP				:1;
		unsigned long	PFD				:1;
		unsigned long	MRLSC			:1;
		unsigned long	PDC				:1;
		unsigned long	CC				:1;
		unsigned long	MRLSS			:1;
		unsigned long	PDS				:1;
		unsigned long	EIS				:1;
		unsigned long	DLLSC			:1;
		unsigned long					:7;
	} bit;
};

/* Root Control Register (Offset 008Ch) <RC> */
/* Root Capability Register (Offset 008Eh) <RC> */
union io_pciecr_rccr {
	unsigned long		word;
	struct {
		unsigned long	SECEE			:1;
		unsigned long	SENFEE			:1;
		unsigned long	SEFEE			:1;
		unsigned long	PMEIE			:1;
		unsigned long	CRSSVE			:1;
		unsigned long					:11;

		unsigned long	CRSSV			:1;
		unsigned long					:15;
	} bit;
};

/* Root Status Register (Offset 0090h) <RC> */
union io_pciecr_rsr {
	unsigned long		word;
	struct {
		unsigned long	PMERID			:16;
		unsigned long	PMES			:1;
		unsigned long	PMEPEND			:1;
		unsigned long					:14;
	} bit;
};

/* Device Capabilities 2 Register (Offset 0094h) */
union io_pciecr_dc2r {
	unsigned long		word;
	struct {
		unsigned long	CTRS			:4;
		unsigned long	CTDS			:1;
		unsigned long	ARIFS			:1;
		unsigned long	AORS			:1;
		unsigned long	_32AOCS			:1;
		unsigned long	_64AOCS			:1;
		unsigned long	_128CASCS		:1;
		unsigned long	NROPRPRP		:1;
		unsigned long	LTRMS			:1;
		unsigned long	TPHCS			:2;
		unsigned long	LNSCLS			:2;
		unsigned long					:2;
		unsigned long	OBFFS			:2;
		unsigned long					:12;
	} bit;
};

/* Device Control2 Register (Offset 0098h) */
/* Device Status2 Register (Offset 009Ah) */
union io_pciecr_dc2sr {
	unsigned long		word;
	struct {
		unsigned long	CTV				:4;
		unsigned long	CTD				:1;
		unsigned long	ARIFE			:1;
		unsigned long					:4;
		unsigned long	LTRME			:1;
		unsigned long					:5;

		unsigned long					:16;
	} bit;
};



/* Link Capability2 Register (Offset 009Ch) */
union io_pciecr_lc2r {
	unsigned long		word;
	struct {
		unsigned long					:1;
		unsigned long	SLSV			:7;
		unsigned long	CLSP			:1;
		unsigned long					:22;
		unsigned long	DRSS			:1;
	} bit;
};

/* Link Control2 Register (Offset 00A0h) */
/* Link Status2 Register (Offset 00A2h) */
union io_pciecr_lc2sr {
	unsigned long		word;
	struct {
		unsigned long	TLS				:4;
		unsigned long	EC				:1;
		unsigned long	HASD			:1;
		unsigned long	SDE				:1;
		unsigned long	TM				:3;
		unsigned long	EMC				:1;
		unsigned long	CSOS			:1;
		unsigned long	CPDE			:4;

		unsigned long	CDEL			:1;
		unsigned long					:11;
		unsigned long	DCP				:3;
		unsigned long	DRSMR			:1;
	} bit;
};

/* PCI Express Capability Register */
struct io_pciecr {
	union io_pciecr_cr		CR;								/* +0070 - 0073h */
	union io_pciecr_dcr		DCR;							/* +0074 - 0077h */
	union io_pciecr_dcsr	DCSR;							/* +0078 - 007Bh */
	union io_pciecr_lcr		LCR;							/* +007C - 007Fh */
	union io_pciecr_lcsr	LCSR;							/* +0080 - 0083h */
	union io_pciecr_scr		SCR;							/* +0084 - 0087h */	// RC only
	union io_pciecr_scsr	SCSR;							/* +0088 - 008Bh */	// RC only
	union io_pciecr_rccr	RCCR;							/* +008C - 008Fh */	// RC only
	union io_pciecr_rsr		RSR;							/* +0090 - 0093h */	// RC only
	union io_pciecr_dc2r	DC2R;							/* +0094 - 0097h */
	union io_pciecr_dc2sr	DC2SR;							/* +0098 - 009Bh */
	union io_pciecr_lc2r	LC2R;							/* +009C - 009Fh */
	union io_pciecr_lc2sr	LC2SR;							/* +00A0 - 00A3h */
	unsigned char			dmy_00A4_00AF[0x00B0-0x00A4];	/* +00A4 - 00AFh */
};

/* Advanced Error Reporting Enhanced Capability Header (Offset 0100h) */
union io_aerc_aerech {
	unsigned long		word;
	struct {
		unsigned long	PCIEECID		:16;
		unsigned long	CAPV			:4;
		unsigned long	NCO				:12;
	} bit;
};

/* Uncorrectable Error Status Register (Offset 0104h) */
union io_aerc_uesr {
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	DLPES			:1;
		unsigned long	SDES			:1;
		unsigned long					:6;
		unsigned long	PTLPS			:1;
		unsigned long	FCPES			:1;
		unsigned long	CTS				:1;
		unsigned long	CAS				:1;
		unsigned long	UCS				:1;
		unsigned long	ROS				:1;
		unsigned long	MTLPS			:1;
		unsigned long	ECRCES			:1;
		unsigned long	URES			:1;
		unsigned long					:1;
		unsigned long	UIES			:1;
		unsigned long					:2;
		unsigned long	TLPPBES			:1;
		unsigned long					:6;
	} bit;
};

/* Uncorrectable Error Mask Register (Offset 0108h) */
union io_aerc_uemr {
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	DLPEM			:1;
		unsigned long	SDEM			:1;
		unsigned long					:6;
		unsigned long	PTLPM			:1;
		unsigned long	FCPEM			:1;
		unsigned long	CTM				:1;
		unsigned long	CAM				:1;
		unsigned long	UCM				:1;
		unsigned long	ROM				:1;
		unsigned long	MTLPM			:1;
		unsigned long	ECRCEM			:1;
		unsigned long	UREM			:1;
		unsigned long					:1;
		unsigned long	UIEM			:1;
		unsigned long					:1;
		unsigned long	AOEBM			:1;
		unsigned long	TLPPBEM			:1;
		unsigned long					:6;
	} bit;
};

/* Uncorrectable Error Severity Register (Offset 010Ch) */
union io_aerc_uesvr {
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	DLPES			:1;
		unsigned long	SDES			:1;
		unsigned long					:6;
		unsigned long	PTLPS			:1;
		unsigned long	FCPES			:1;
		unsigned long	CTES			:1;
		unsigned long	CAES			:1;
		unsigned long	UCES			:1;
		unsigned long	ROES			:1;
		unsigned long	MTLPS			:1;
		unsigned long	ECRCES			:1;
		unsigned long	URES			:1;
		unsigned long					:1;
		unsigned long	UIES			:1;
		unsigned long					:1;
		unsigned long	AOEBS			:1;
		unsigned long	TLPPBES			:1;
		unsigned long					:6;
	} bit;
};

/* Correctable Error Status Register (Offset 0110h) */
union io_aerc_cesr {
	unsigned long		word;
	struct {
		unsigned long	RES				:1;
		unsigned long					:5;
		unsigned long	BTLPS			:1;
		unsigned long	BDLLPS			:1;
		unsigned long	RNRS			:1;
		unsigned long					:3;
		unsigned long	RTTS			:1;
		unsigned long	ANFES			:1;
		unsigned long	CIES			:1;
		unsigned long	HLOS			:1;
		unsigned long					:16;
	} bit;
};

/* Correctable Error Mask Register (Offset 0114h) */
union io_aerc_cemr {
	unsigned long		word;
	struct {
		unsigned long	REM				:1;
		unsigned long					:5;
		unsigned long	BTLPM			:1;
		unsigned long	BDLLPM			:1;
		unsigned long	RNRM			:1;
		unsigned long					:3;
		unsigned long	RTTM			:1;
		unsigned long	ANFEM			:1;
		unsigned long	CIEM			:1;
		unsigned long	HLOM			:1;
		unsigned long					:16;
	} bit;
};

/* Advanced Error Capabilities and Control Register (Offset 0118h) */
union io_aerc_aeccr {
	unsigned long		word;
	struct {
		unsigned long	FEP				:5;
		unsigned long	ECRCGC			:1;
		unsigned long	ECRCGE			:1;
		unsigned long	ECRCCC			:1;
		unsigned long	ECRCCE			:1;
		unsigned long	MHRC			:1;
		unsigned long	MHRE			:1;
		unsigned long					:21;
	} bit;
};

/* Header Log Register (Offset 011Ch,0120h,0124h,0128h) */
union io_aerc_hlr {
	unsigned long		word;
	struct {
		unsigned long	HLR				:32;
	} bit;
};

/* Root Error Command Register (Offset 012Ch) <RC> */
union io_aerc_recr {
	unsigned long		word;
	struct {
		unsigned long	CERE			:1;
		unsigned long	NFERE			:1;
		unsigned long	FERE			:1;
		unsigned long					:29;
	} bit;
};

/* Root Error Status Register (Offset 0130h) <RC> */
union io_aerc_resr {
	unsigned long		word;
	struct {
		unsigned long	ERCORR			:1;
		unsigned long	MERCORR			:1;
		unsigned long	ERFNFR			:1;
		unsigned long	MERFNFR			:1;
		unsigned long	FUF				:1;
		unsigned long	NFEMR			:1;
		unsigned long	FEMR			:1;
		unsigned long					:20;
		unsigned long	AEIMN			:5;
	} bit;
};

/* Error Source Identification Register (Offset 0134h) <RC> */
union io_aerc_esir {
	unsigned long		word;
	struct {
		unsigned long	ERCORSI			:16;
		unsigned long	ERFNFSI			:16;
	} bit;
};

/* TLP Prefix Log Register (Offset 0138h,013Ch,0140h,0144h) */
union io_tlpplr {
	unsigned long		word;
	struct {
		unsigned long	TLPPLR			:32;
	} bit;
};

/* Advanced Error Reporting Capability */
struct io_aerc {
	union io_aerc_aerech	AERECH;							/* +0100 - 0103h */
	union io_aerc_uesr		UESR;							/* +0104 - 0107h */
	union io_aerc_uemr		UEMR;							/* +0108 - 010Bh */
	union io_aerc_uesvr		UESVR;							/* +010C - 010Fh */
	union io_aerc_cesr		CESR;							/* +0110 - 0113h */
	union io_aerc_cemr		CEMR;							/* +0114 - 0117h */
	union io_aerc_aeccr		AECCR;							/* +0118 - 011Bh */
	union io_aerc_hlr		HLR[4];							/* +011C - 012Bh */
	union io_aerc_recr		RECR;							/* +012C - 012Fh */	// RC Only
	union io_aerc_resr		RESR;							/* +0130 - 0133h */	// RC Only
	union io_aerc_esir		ESIR;							/* +0134 - 0137h */	// RC Only
	union io_tlpplr			TLPPLR[4];						/* +0138 - 0147h */
};

/* LTR Extended Capability Header (Offset 0148h) */
union io_ltrc_ltrech {
	unsigned long		word;
	struct {
		unsigned long	PCIEECID		:16;
		unsigned long	CAPV			:4;
		unsigned long	NCO				:12;
	} bit;
};

/* Max Snoop Latency Register    (Offset 014Ch) */
/* Max No-Snoop Latency Register (Offset 014Eh) */
union io_ltrc_msnslr {
	unsigned long		word;
	struct {
		unsigned long	MSLV			:10;
		unsigned long	MSLS			:3;
		unsigned long					:3;
		unsigned long	MNSLV			:10;
		unsigned long	MNSLS			:3;
		unsigned long					:3;
	} bit;
};

/* Latency Tolerance Reporting (LTR) Capability */
struct io_ltrc {
	union io_ltrc_ltrech	LTRECH;							/* +0148 - 014Bh */
	union io_ltrc_msnslr	MSNSLR;							/* +014C - 014Fh */
};

/* L1 PM Substates Extended Capability Header (Offset 0150h) */
union io_l1pmsec_l1pmsech {
	unsigned long		word;
	struct {
		unsigned long	PCIEECID		:16;
		unsigned long	CV				:4;
		unsigned long	NCO				:12;
	} bit;
};

/* L1 PM Substates Capabilities Register (Offset 0154h) */
union io_l1pmsec_l1pmscr {
	unsigned long		word;
	struct {
		unsigned long	PCIPML12S		:1;
		unsigned long	PCIPML11S		:1;
		unsigned long	ASPML12S		:1;
		unsigned long	ASPML11S		:1;
		unsigned long	L1PMSS			:1;
		unsigned long					:3;
		unsigned long	PCMRT			:8;
		unsigned long	PTPOS			:2;
		unsigned long					:1;
		unsigned long	PTPOV			:5;
		unsigned long					:8;
	} bit;
};

/* L1 PM Substates Control 1 Register (Offset 0158h) */
union io_l1pmsec_l1pmsc1r {
	unsigned long		word;
	struct {
		unsigned long	PCIPML12E		:1;
		unsigned long	PCIPML11E		:1;
		unsigned long	ASPML12E		:1;
		unsigned long	ASPML11E		:1;
		unsigned long					:4;
		unsigned long	CMRT			:8;
		unsigned long	LTRL12TV		:10;
		unsigned long					:3;
		unsigned long	LTRL12TS		:3;
	} bit;
};

/* L1 PM Substates Control 2 Register (Offset 015Ch) */
union io_l1pmsec_l1pmsc2r {
	unsigned long		word;
	struct {
		unsigned long	TPOS			:2;
		unsigned long					:1;
		unsigned long	TPOV			:5;
		unsigned long					:24;
	} bit;
};

/* L1 PM Substates Extended Capability */
struct io_l1pmsec {
	union io_l1pmsec_l1pmsech	L1PMSECH;					/* +0150 - 0153h */
	union io_l1pmsec_l1pmscr	L1PMSCR;					/* +0154 - 0157h */
	union io_l1pmsec_l1pmsc1r	L1PMSC1R;					/* +0158 - 015Bh */
	union io_l1pmsec_l1pmsc2r	L1PMSC2R;					/* +015C - 015Fh */
};

/* ACK_LATENCY_TIMER_OFF (Offset 0700h) */
union io_ppl_alt {
	unsigned long		word;
	struct {
		unsigned long	RTL			:16;
		unsigned long	RTLTL		:16;
	} bit;
};

/* VENDOR_SPEC_DLLP_OFF (Offset 0704h) */
union io_ppl_vsd {
	unsigned long		word;
	struct {
		unsigned long	VSD			:32;
	} bit;
};

/* PORT_FORCE_OFF (Offset 0708h) */
union io_ppl_pf {
	unsigned long		word;
	struct {
		unsigned long	LN			:8;
		unsigned long	FL			:4;
		unsigned long				:3;
		unsigned long	FE			:1;
		unsigned long	LS			:6;
		unsigned long				:2;
		unsigned long	CSC			:8;
	} bit;
};

/* ACK_F_ASPM_CTRL_OFF (Offset 070Ch) */
union io_ppl_afac {
	unsigned long		word;
	struct {
		unsigned long	AF			:8;
		unsigned long	ANF			:8;
		unsigned long	CCNF		:8;
		unsigned long	L0SEL		:3;
		unsigned long	L1EL		:3;
		unsigned long	EA			:1;
		unsigned long				:1;
	} bit;
};

/* PORT_LINK_CTRL_OFF (Offset 0710h) */
union io_ppl_plc {
	unsigned long		word;
	struct {
		unsigned long	VSDR		:1;
		unsigned long	SD			:1;
		unsigned long	LE			:1;
		unsigned long	RA			:1;
		unsigned long				:1;
		unsigned long	DLE			:1;
		unsigned long				:1;
		unsigned long	FLM			:1;
		unsigned long				:4;
		unsigned long				:4;
		unsigned long	LC			:6;
		unsigned long				:2;
		unsigned long				:1;
		unsigned long				:1;
		unsigned long				:1;
		unsigned long				:1;
		unsigned long				:4;
	} bit;
};

/* LANE_SKEW_OFF (Offset 0714h) */
union io_ppl_ls {
	unsigned long		word;
	struct {
		unsigned long	ILS			:24;
		unsigned long	FCD			:1;
		unsigned long	AND			:1;
		unsigned long				:5;
		unsigned long	DLTLD		:1;
	} bit;
};

/* TIMER_CTRL_MAX_FUNC_NUM_OFF (Offset 0718h) */
union io_ppl_tcmfn {
	unsigned long		word;
	struct {
		unsigned long	MFN			:8;
		unsigned long				:6;
		unsigned long	TMRT		:5;
		unsigned long	TMAN		:5;
		unsigned long				:5;
		unsigned long				:3;
	} bit;
};

/* SYMBOL_TIMER_FILTER_1_OFF (Offset 071Ch) */
union io_ppl_stf1 {
	unsigned long		word;
	struct {
		unsigned long	SIV			:11;
		unsigned long				:4;
		unsigned long	DFWT		:1;
		unsigned long	MR1			:16;
	} bit;
};

/* FILTER_MASK_2_OFF (Offset 0720h) */
union io_ppl_fm2 {
	unsigned long		word;
	struct {
		unsigned long	MR2			:32;
	} bit;
};

/* AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_OFF (Offset 0724h) */
union io_ppl_amodnsrc {
	unsigned long		word;
	struct {
		unsigned long	ORSBE		:1;
		unsigned long				:31;
	} bit;
};

/* PL_DEBUG0_OFF (Offset 0728h) */
union io_ppl_pd0 {
	unsigned long		word;
	struct {
		unsigned long	DR0			:32;
	} bit;
};

/* PL_DEBUG1_OFF (Offset 072Ch) */
union io_ppl_pd1 {
	unsigned long		word;
	struct {
		unsigned long	DR1			:32;
	} bit;
};

/* TX_P_FC_CREDIT_STATUS_OFF (Offset 0730h) */
union io_ppl_tpfcs {
	unsigned long		word;
	struct {
		unsigned long	TPDFC		:12;
		unsigned long	TPHFC		:8;
		unsigned long				:12;
	} bit;
};

/* TX_NP_FC_CREDIT_STATUS_OFF (Offset 0734h) */
union io_ppl_tnfcs {
	unsigned long		word;
	struct {
		unsigned long	TNDFC		:12;
		unsigned long	TNHFC		:8;
		unsigned long				:12;
	} bit;
};

/* TX_CPL_FC_CREDIT_STATUS_OFF (Offset 0738h) */
union io_ppl_tcfcs {
	unsigned long		word;
	struct {
		unsigned long	TCDFC		:12;
		unsigned long	TCHFC		:8;
		unsigned long				:12;
	} bit;
};

/* QUEUE_STATUS_OFF (Offset 073Ch) */
union io_ppl_qs {
	unsigned long		word;
	struct {
		unsigned long	RTFCNR		:1;
		unsigned long	TRBN		:1;
		unsigned long	RQNE		:1;
		unsigned long				:13;
		unsigned long	TMFC		:13;
		unsigned long				:2;
		unsigned long	TMFCE		:1;
	} bit;
};

/* VC_TX_ARBI_1_OFF (Offset 0740h) */
union io_ppl_vta1 {
	unsigned long		word;
	struct {
		unsigned long	WWV0		:8;
		unsigned long	WWV1		:8;
		unsigned long	WWV2		:8;
		unsigned long	WWV3		:8;
	} bit;
};

/* VC_TX_ARBI_2_OFF (Offset 0744h) */
union io_ppl_vta2 {
	unsigned long		word;
	struct {
		unsigned long	WWV4		:8;
		unsigned long	WWV5		:8;
		unsigned long	WWV6		:8;
		unsigned long	WWV7		:8;
	} bit;
};

/* VC0_P_RX_Q_CTRL_OFF (Offset 0748h) */
union io_ppl_vprqc {
	unsigned long		word;
	struct {
		unsigned long	VPDC		:12;
		unsigned long	VPHC		:8;
		unsigned long				:1;
		unsigned long	VPTQM		:3;
		unsigned long				:6;
		unsigned long	TTOV		:1;
		unsigned long	VORQ		:1;
	} bit;
};

/* VC0_NP_RX_Q_CTRL_OFF (Offset 074Ch) */
union io_ppl_vnrqc {
	unsigned long		word;
	struct {
		unsigned long	VNDC		:12;
		unsigned long	VNHC		:8;
		unsigned long				:1;
		unsigned long	VNTQM		:3;
		unsigned long				:8;
	} bit;
};

/* VC0_CPL_RX_Q_CTRL_OFF (Offset 0750h) */
union io_ppl_vcrqc {
	unsigned long		word;
	struct {
		unsigned long	VCDC		:12;
		unsigned long	VCHC		:8;
		unsigned long				:1;
		unsigned long	VCTQM		:3;
		unsigned long				:8;
	} bit;
};

/* GEN2_CTRL_OFF (Offset 080Ch) */
union io_ppl_gen2c {
	unsigned long		word;
	struct {
		unsigned long	FTS			:8;
		unsigned long	NOL			:5;
		unsigned long	PDL			:3;
		unsigned long	ALFCE		:1;
		unsigned long	DSC			:1;
		unsigned long	CPTC		:1;
		unsigned long	CTCR		:1;
		unsigned long	SD			:1;
		unsigned long	GEI			:1;
		unsigned long				:10;
	} bit;
};

/* PHY_STATUS_OFF (Offset 0810h) */
union io_ppl_phys {
	unsigned long		word;
	struct {
		unsigned long	PS			:32;
	} bit;
};

/* PHY_CONTROL_OFF (Offset 0814h) */
union io_ppl_phyc {
	unsigned long		word;
	struct {
		unsigned long	PC			:32;
	} bit;
};

/* MSI_CTRL_ADDR_OFF (Offset 0820h) */
union io_ppl_mca {
	unsigned long		word;
	struct {
		unsigned long	MCA			:32;
	} bit;
};

/* MSI_CTRL_UPPER_ADDR_OFF (Offset 0824h) */
union io_ppl_mcua {
	unsigned long		word;
	struct {
		unsigned long	MCUA		:32;
	} bit;
};

/* MSI_CTRL_INT_0_EN_OFF (Offset 0828h) */
union io_ppl_mci0e {
	unsigned long		word;
	struct {
		unsigned long	MCI0E		:32;
	} bit;
};

/* MSI_CTRL_INT_0_MASK_OFF (Offset 082Ch) */
union io_ppl_mci0m {
	unsigned long		word;
	struct {
		unsigned long	MCI0M		:32;
	} bit;
};

/* MSI_CTRL_INT_0_STATUS_OFF (Offset 0830h) */
union io_ppl_mci0s {
	unsigned long		word;
	struct {
		unsigned long	MCI0S		:32;
	} bit;
};

/* MSI_GPIO_IO_OFF (Offset 0888h) */
union io_ppl_mgi {
	unsigned long		word;
	struct {
		unsigned long	MGR			:32;
	} bit;
};

/* PIPE_LOOPBACK_CONTROL_OFF (Offset 08B8h) */
union io_ppl_pilc {
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long				:6;
		unsigned long				:2;
		unsigned long				:3;
		unsigned long				:4;
		unsigned long	PLE			:1;
	} bit;
};

/* MISC_CONTROL_1_OFF (Offset 08BCh) */
union io_ppl_mc1 {
	unsigned long		word;
	struct {
		unsigned long	DRWE		:1;
		unsigned long				:31;
	} bit;
};

/* MULTI_LANE_CONTROL_OFF (Offset 08C0h) */
union io_ppl_mlc {
	unsigned long		word;
	struct {
		unsigned long	TLW			:6;
		unsigned long	DLWC		:1;
		unsigned long	US			:1;
		unsigned long				:24;
	} bit;
};

/* PHY_INTEROP_CTRL_OFF (Offset 08C4h) */
union io_ppl_phyic {
	unsigned long		word;
	struct {
		unsigned long	RC			:7;
		unsigned long				:1;
		unsigned long	L1EM		:1;
		unsigned long	L1NP		:1;
		unsigned long				:22;
	} bit;
};

/* LINK_FLUSH_CONTROL_OFF (Offset 08CCh) */
union io_ppl_lfc {
	unsigned long		word;
	struct {
		unsigned long	AFE			:1;
		unsigned long				:31;
	} bit;
};

/* AMBA_ERROR_RESPONSE_DEFAULT_OFF (Offset 08D0h) */
union io_ppl_aerdr {
	unsigned long		word;
	struct {
		unsigned long	AERG		:1;
		unsigned long				:1;
		unsigned long	AERV		:1;
		unsigned long				:7;
		unsigned long	AERM		:6;
		unsigned long				:16;
	} bit;
};

/* AMBA_LINK_TIMEOUT_OFF (Offset 08D4h) */
union io_ppl_amlt {
	unsigned long		word;
	struct {
		unsigned long	LTPD		:8;
		unsigned long	LTED		:1;
		unsigned long				:23;
	} bit;
};

/* AMBA_ORDERING_CTRL_OFF (Offset 08D8h) */
union io_ppl_aoc {
	unsigned long		word;
	struct {
		unsigned long	AMNPP		:1;
		unsigned long	ASE			:1;
		unsigned long				:30;
	} bit;
};

/* COHERENCY_CONTROL_1_OFF (Offset 08E0h) */
union io_ppl_cc1 {
	unsigned long		word;
	struct {
		unsigned long	CMV			:1;
		unsigned long				:1;
		unsigned long	CMBLA		:30;
	} bit;
};

/* COHERENCY_CONTROL_2_OFF (Offset 08E4h) */
union io_ppl_cc2 {
	unsigned long		word;
	struct {
		unsigned long	CMBHA		:32;
	} bit;
};

/* COHERENCY_CONTROL_3_OFF (Offset 08E8h) */
union io_ppl_cc3 {
	unsigned long		word;
	struct {
		unsigned long				:3;
		unsigned long	CMARM		:4;
		unsigned long				:4;
		unsigned long	CMAWM		:4;
		unsigned long				:4;
		unsigned long	CMARV		:4;
		unsigned long				:4;
		unsigned long	CMAWV		:4;
		unsigned long				:1;
	} bit;
};

/* AXI_MSTR_MSG_ADDR_LOW_OFF (Offset 08F0h) */
union io_ppl_ammal {
	unsigned long		word;
	struct {
		unsigned long				:12;
		unsigned long	CAMAL		:20;
	} bit;
};

/* AXI_MSTR_MSG_ADDR_HIGH_OFF (Offset 08F4h) */
union io_ppl_ammah {
	unsigned long		word;
	struct {
		unsigned long	CAMAH		:32;
	} bit;
};

/* IATU_VIEWPORT_OFF (Offset 0900h) */
union io_ppl_iv {
	unsigned long		word;
	struct {
		unsigned long	RI			:1;
		unsigned long				:30;
		unsigned long	RD			:1;
	} bit;
};

/* IATU_REGION_CTRL_1_OFF_OUTBOUND_0 (Offset 0904h) */
/* IATU_REGION_CTRL_1_OFF_INBOUND_0  (Offset 0904h) */
union io_ppl_irc1 {
	unsigned long		word;
	struct {
		unsigned long	TYPE		:5;
		unsigned long	TC			:3;
		unsigned long	TD			:1;
		unsigned long	ATTR		:2;
		unsigned long				:5;
		unsigned long	AT			:2;
		unsigned long				:2;
		unsigned long	C1FN		:3;
		unsigned long				:9;
	} bit;
};

/* IATU_REGION_CTRL_2_OFF_OUTBOUND_0 (Offset 0908h) */
union io_ppl_irc2o {
	unsigned long		word;
	struct {
		unsigned long	MC			:8;
		unsigned long	TAG			:8;
		unsigned long	TSE			:1;
		unsigned long				:2;
		unsigned long	FB			:1;
		unsigned long	SNP			:1;
		unsigned long				:1;
		unsigned long	IP			:1;
		unsigned long	HSE			:1;
		unsigned long				:3;
		unsigned long	DB			:1;
		unsigned long	CSM			:1;
		unsigned long	IM			:1;
		unsigned long				:1;
		unsigned long	RE			:1;
	} bit;
};

/* IATU_REGION_CTRL_2_OFF_INBOUND_0 (Offset 0908h) */
union io_ppl_irc2i {
	unsigned long		word;
	struct {
		unsigned long	MC			:8;
		unsigned long	BN			:3;
		unsigned long				:3;
		unsigned long	TCME		:1;
		unsigned long	TDME		:1;
		unsigned long	ATTRME		:1;
		unsigned long				:1;
		unsigned long	ATME		:1;
		unsigned long	FNME		:1;
		unsigned long				:1;
		unsigned long	MCME		:1;
		unsigned long				:1;
		unsigned long	SALTE		:1;
		unsigned long	RC			:2;
		unsigned long				:1;
		unsigned long	FTMC		:1;
		unsigned long	CSM			:1;
		unsigned long	IM			:1;
		unsigned long	MM			:1;
		unsigned long	RE			:1;
	} bit;
};

/* IATU_REGION_CTRL_2_OFF */
union io_ppl_irc2 {
	unsigned long		word;
	union io_ppl_irc2o	OB;
	union io_ppl_irc2i	IB;
};

/* IATU_LWR_BASE_ADDR_OFF_OUTBOUND_0 (Offset 090Ch) */
/* IATU_LWR_BASE_ADDR_OFF_INBOUND_0  (Offset 090Ch) */
union io_ppl_ilba {
	unsigned long		word;
	struct {
		unsigned long	LBH			:16;
		unsigned long	LBR			:16;
	} bit;
};

/* IATU_UPPER_BASE_ADDR_OFF_OUTBOUND_0 (Offset 0910h) */
/* IATU_UPPER_BASE_ADDR_OFF_INBOUND_0  (Offset 0910h) */
union io_ppl_iuba {
	unsigned long		word;
	struct {
		unsigned long	UBR			:32;
	} bit;
};

/* IATU_LIMIT_ADDR_OFF_OUTBOUND_0 (Offset 0914h) */
/* IATU_LIMIT_ADDR_OFF_INBOUND_0  (Offset 0914h) */
union io_ppl_ila {
	unsigned long		word;
	struct {
		unsigned long	LAH			:16;
		unsigned long	LAR			:16;
	} bit;
};

/* IATU_LWR_TARGET_ADDR_OFF_OUTBOUND_0 (Offset 0918h) */
union io_ppl_iltao {
	unsigned long		word;
	struct {
		unsigned long	LTRO		:32;
	} bit;
};

/* IATU_LWR_TARGET_ADDR_OFF_INBOUND_0 (Offset 0918h) */
union io_ppl_iltai {
	unsigned long		word;
	struct {
		unsigned long	LTH			:16;
		unsigned long	LTR			:16;
	} bit;
};

/* IATU_LWR_TARGET_ADDR_OFF */
union io_ppl_ilta {
	unsigned long		word;
	union io_ppl_iltao	OB;
	union io_ppl_iltai	IB;
};

/* IATU_UPPER_TARGET_ADDR_OFF_OUTBOUND_0 (Offset 091Ch) */
/* IATU_UPPER_TARGET_ADDR_OFF_INBOUND_0 (Offset 091Ch) */
union io_ppl_iuta {
	unsigned long		word;
	struct {
		unsigned long	UTR			:32;
	} bit;
};

/* DMA_CTRL_DATA_ARB_PRIOR_OFF (Offset 0970h) */
union io_ppl_dcdap {
	unsigned long		word;
	struct {
		unsigned long	RW			:3;
		unsigned long	WCTW		:3;
		unsigned long	RCTW		:3;
		unsigned long	RTW			:3;
		unsigned long				:20;
	} bit;
};

/* DMA_CTRL_OFF (Offset 0978h) */
union io_ppl_dc {
	unsigned long		word;
	struct {
		unsigned long	NDWC		:4;
		unsigned long				:12;
		unsigned long	NDRC		:4;
		unsigned long				:12;
	} bit;
};

/* DMA_WRITE_ENGINE_EN_OFF (Offset 097Ch) */
union io_ppl_dwee {
	unsigned long		word;
	struct {
		unsigned long	DWE			:1;
		unsigned long				:31;
	} bit;
};

/* DMA_WRITE_DOORBELL_OFF (Offset 0980h) */
union io_ppl_dwd {
	unsigned long		word;
	struct {
		unsigned long	WDN			:3;
		unsigned long				:28;
		unsigned long	WS			:1;
	} bit;
};

/* DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_OFF (Offset 0988h) */
union io_ppl_dwcawl {
	unsigned long		word;
	struct {
		unsigned long	WC0W		:5;
		unsigned long	WC1W		:5;
		unsigned long	WC2W		:5;
		unsigned long	WC3W		:5;
		unsigned long				:12;
	} bit;
};

/* DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_OFF (Offset 098Ch) */
union io_ppl_dwcawh {
	unsigned long		word;
	struct {
		unsigned long	WC4W		:5;
		unsigned long	WC5W		:5;
		unsigned long	WC6W		:5;
		unsigned long	WC7W		:5;
		unsigned long				:12;
	} bit;
};

/* DMA_READ_ENGINE_EN_OFF (Offset 099Ch) */
union io_ppl_dree {
	unsigned long		word;
	struct {
		unsigned long	DRE			:1;
		unsigned long				:31;
	} bit;
};

/* DMA_READ_DOORBELL_OFF (Offset 09A0h) */
union io_ppl_drd {
	unsigned long		word;
	struct {
		unsigned long	RDN			:3;
		unsigned long				:28;
		unsigned long	RS			:1;
	} bit;
};

/* DMA_READ_CHANNEL_ARB_WEIGHT_LOW_OFF (Offset 09A8h) */
union io_ppl_drcawl {
	unsigned long		word;
	struct {
		unsigned long	RC0W		:5;
		unsigned long	RC1W		:5;
		unsigned long	RC2W		:5;
		unsigned long	RC3W		:5;
		unsigned long				:12;
	} bit;
};

/* DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_OFF (Offset 09ACh) */
union io_ppl_drcawh {
	unsigned long		word;
	struct {
		unsigned long	RC4W		:5;
		unsigned long	RC5W		:5;
		unsigned long	RC6W		:5;
		unsigned long	RC7W		:5;
		unsigned long				:12;
	} bit;
};

/* DMA_WRITE_INT_STATUS_OFF (Offset 09BCh) */
union io_ppl_dwis {
	unsigned long		word;
	struct {
		unsigned long	WDIS		:8;
		unsigned long				:8;
		unsigned long	WAIS		:8;
		unsigned long				:8;
	} bit;
};

/* DMA_WRITE_INT_MASK_OFF (Offset 09C4h) */
union io_ppl_dwim {
	unsigned long		word;
	struct {
		unsigned long	WDIM		:2;
		unsigned long				:14;
		unsigned long	WAIM		:2;
		unsigned long				:14;
	} bit;
};

/* DMA_WRITE_INT_CLEAR_OFF (Offset 09C8h) */
union io_ppl_dwic {
	unsigned long		word;
	struct {
		unsigned long	WDIC		:8;
		unsigned long				:8;
		unsigned long	WAIC		:8;
		unsigned long				:8;
	} bit;
};

/* DMA_WRITE_ERR_STATUS_OFF (Offset 09CCh) */
union io_ppl_dwes {
	unsigned long		word;
	struct {
		unsigned long	ARED		:8;
		unsigned long				:8;
		unsigned long	LEFED		:8;
		unsigned long				:8;
	} bit;
};

/* DMA_WRITE_DONE_IMWR_LOW_OFF (Offset 09D0h) */
union io_ppl_dwdil {
	unsigned long		word;
	struct {
		unsigned long	DWDLR		:32;
	} bit;
};

/* DMA_WRITE_DONE_IMWR_HIGH_OFF (Offset 09D4h) */
union io_ppl_dwdih {
	unsigned long		word;
	struct {
		unsigned long	DWDHR		:32;
	} bit;
};

/* DMA_WRITE_ABORT_IMWR_LOW_OFF (Offset 09D8h) */
union io_ppl_dwail {
	unsigned long		word;
	struct {
		unsigned long	DWALR		:32;
	} bit;
};

/* DMA_WRITE_ABORT_IMWR_HIGH_OFF (Offset 09DCh) */
union io_ppl_dwaih {
	unsigned long		word;
	struct {
		unsigned long	DWAHR		:32;
	} bit;
};

/* DMA_WRITE_CH01_IMWR_DATA_OFF (Offset 09E0h) */
union io_ppl_dwc01id {
	unsigned long		word;
	struct {
		unsigned long	WC0D			:16;
		unsigned long	WC1D			:16;
	} bit;
};

/* DMA_WRITE_CH23_IMWR_DATA_OFF (Offset 09E4h) */
union io_ppl_dwc23id {
	unsigned long		word;
	struct {
		unsigned long	WC2D			:16;
		unsigned long	WC3D			:16;
	} bit;
};

/* DMA_WRITE_CH45_IMWR_DATA_OFF (Offset 09E8h) */
union io_ppl_dwc45id {
	unsigned long		word;
	struct {
		unsigned long	WC4D			:16;
		unsigned long	WC5D			:16;
	} bit;
};

/* DMA_WRITE_CH67_IMWR_DATA_OFF (Offset 09ECh) */
union io_ppl_dwch67id {
	unsigned long		word;
	struct {
		unsigned long	WC6D			:16;
		unsigned long	WC7D			:16;
	} bit;
};

/* DMA_WRITE_LINKED_LIST_ERR_EN_OFF (Offset 0A00h) */
union io_ppl_dwllee {
	unsigned long		word;
	struct {
		unsigned long	WCLLLAIE	:2;
		unsigned long				:14;
		unsigned long	WCLLRAIE	:2;
		unsigned long				:14;
	} bit;
};

/* DMA_READ_INT_STATUS_OFF (Offset 0A10h) */
union io_ppl_dris {
	unsigned long		word;
	struct {
		unsigned long	RDIS		:8;
		unsigned long				:8;
		unsigned long	RAIS		:8;
		unsigned long				:8;
	} bit;
};

/* DMA_READ_INT_MASK_OFF (Offset 0A18h) */
union io_ppl_drim {
	unsigned long		word;
	struct {
		unsigned long	RDIM		:2;
		unsigned long				:14;
		unsigned long	RAIM		:2;
		unsigned long				:14;
	} bit;
};

/* DMA_READ_INT_CLEAR_OFF (Offset 0A1Ch) */
union io_ppl_dric {
	unsigned long		word;
	struct {
		unsigned long	RDIC		:8;
		unsigned long				:8;
		unsigned long	RAIC		:8;
		unsigned long				:8;
	} bit;
};

/* DMA_READ_ERR_STATUS_LOW_OFF (Offset 0A24h) */
union io_ppl_dresl {
	unsigned long		word;
	struct {
		unsigned long	AWED		:8;
		unsigned long				:8;
		unsigned long	LLEFED		:8;
		unsigned long				:8;
	} bit;
};

/* DMA_READ_ERR_STATUS_HIGH_OFF (Offset 0A28h) */
union io_ppl_dresh {
	unsigned long		word;
	struct {
		unsigned long	UR			:8;
		unsigned long	CA			:8;
		unsigned long	CT			:8;
		unsigned long	DP			:8;
	} bit;
};

/* DMA_READ_LINKED_LIST_ERR_EN_OFF (Offset 0A34h) */
union io_ppl_drllee {
	unsigned long		word;
	struct {
		unsigned long	RCLLLAIE	:2;
		unsigned long				:14;
		unsigned long	RCLLRAIE	:2;
		unsigned long				:14;
	} bit;
};

/* DMA_READ_DONE_IMWR_LOW_OFF (Offset 0A3Ch) */
union io_ppl_drdil {
	unsigned long		word;
	struct {
		unsigned long	DRDLR		:32;
	} bit;
};

/* DMA_READ_DONE_IMWR_HIGH_OFF (Offset 0A40h) */
union io_ppl_drdih {
	unsigned long		word;
	struct {
		unsigned long	DRDHR		:32;
	} bit;
};

/* DMA_READ_ABORT_IMWR_LOW_OFF (Offset 0A44h) */
union io_ppl_drail {
	unsigned long		word;
	struct {
		unsigned long	DRALR		:32;
	} bit;
};

/* DMA_READ_ABORT_IMWR_HIGH_OFF (Offset 0A48h) */
union io_ppl_draih {
	unsigned long		word;
	struct {
		unsigned long	DRAHR		:32;
	} bit;
};

/* DMA_READ_CH01_IMWR_DATA_OFF (Offset 0A4Ch) */
union io_ppl_drch01id {
	unsigned long		word;
	struct {
		unsigned long	RC0D		:16;
		unsigned long	RC1D		:16;
	} bit;
};

/* DMA_READ_CH23_IMWR_DATA_OFF (Offset 0A50h) */
union io_ppl_drch23id {
	unsigned long		word;
	struct {
		unsigned long	RC2D		:16;
		unsigned long	RC3D		:16;
	} bit;
};

/* DMA_READ_CH45_IMWR_DATA_OFF (Offset 0A54h) */
union io_ppl_drch45id {
	unsigned long		word;
	struct {
		unsigned long	RC4D		:16;
		unsigned long	RC5D		:16;
	} bit;
};

/* DMA_READ_CH67_IMWR_DATA_OFF (Offset 0A58h) */
union io_ppl_drch67id {
	unsigned long		word;
	struct {
		unsigned long	RC6D		:16;
		unsigned long	RC7D		:16;
	} bit;
};

/* DMA_VIEWPORT_SEL_OFF (Offset 0A6Ch) */
union io_ppl_dvs {
	unsigned long		word;
	struct {
		unsigned long	CN			:3;
		unsigned long				:28;
		unsigned long	CD			:1;
	} bit;
};

/* DMA_CH_CONTROL1_OFF_WRCH_0 (Offset 0A70h) */
/* DMA_CH_CONTROL1_OFF_RDCH_0 (Offset 0A70h) */
union io_ppl_dcc1 {
	unsigned long		word;
	struct {
		unsigned long	CB			:1;
		unsigned long	TCB			:1;
		unsigned long	LLP			:1;
		unsigned long	LIE			:1;
		unsigned long	RIE			:1;
		unsigned long	CS			:2;
		unsigned long				:1;
		unsigned long	CCS			:1;
		unsigned long	LLE			:1;
		unsigned long				:2;
		unsigned long	DFN			:5;
		unsigned long				:7;
		unsigned long	NS			:1;
		unsigned long	DRO			:1;
		unsigned long	DTD			:1;
		unsigned long	DTC			:3;
		unsigned long	DAT			:2;
	} bit;
};

/* DMA_TRANSFER_SIZE_OFF_WRCH_0 (Offset 0A78h) */
/* DMA_TRANSFER_SIZE_OFF_RDCH_0 (Offset 0A78h) */
union io_ppl_dts {
	unsigned long		word;
	struct {
		unsigned long	DTS			:32;
	} bit;
};

/* DMA_SAR_LOW_OFF_WRCH_0 (Offset 0A7Ch) */
/* DMA_SAR_LOW_OFF_RDCH_0 (Offset 0A7Ch) */
union io_ppl_dsl {
	unsigned long		word;
	struct {
		unsigned long	SARL		:32;
	} bit;
};

/* DMA_SAR_HIGH_OFF_WRCH_0 (Offset 0A80h) */
/* DMA_SAR_HIGH_OFF_RDCH_0 (Offset 0A80h) */
union io_ppl_dsh {
	unsigned long		word;
	struct {
		unsigned long	SARH		:32;
	} bit;
};

/* DMA_DAR_LOW_OFF_WRCH_0 (Offset 0A84h) */
/* DMA_DAR_LOW_OFF_RDCH_0 (Offset 0A84h) */
union io_ppl_ddl {
	unsigned long		word;
	struct {
		unsigned long	DARL		:32;
	} bit;
};

/* DMA_DAR_HIGH_OFF_WRCH_0 (Offset 0A88h) */
/* DMA_DAR_HIGH_OFF_RDCH_0 (Offset 0A88h) */
union io_ppl_ddh {
	unsigned long		word;
	struct {
		unsigned long	DARH		:32;
	} bit;
};

/* DMA_LLP_LOW_OFF_WRCH_0 (Offset 0A8Ch) */
/* DMA_LLP_LOW_OFF_RDCH_0 (Offset 0A8Ch) */
union io_ppl_dll {
	unsigned long		word;
	struct {
		unsigned long	LL			:32;
	} bit;
};

/* DMA_LLP_HIGH_OFF_WRCH_0 (Offset 0A90h) */
/* DMA_LLP_HIGH_OFF_RDCH_0 (Offset 0A90h) */
union io_ppl_dlh {
	unsigned long		word;
	struct {
		unsigned long	LH			:32;
	} bit;
};

/* PL_LTR_LATENCY_OFF (Offset 0B30h) */
union io_ppl_pll {
	unsigned long		word;
	struct {
		unsigned long	SLV			:10;
		unsigned long	SLS			:3;
		unsigned long				:2;
		unsigned long	SLR			:1;
		unsigned long	NSLV		:10;
		unsigned long	NSLS		:3;
		unsigned long				:2;
		unsigned long	NSLR		:1;
	} bit;
};

/* AUX_CLK_FREQ_OFF (Offset 0B40h) */
union io_ppl_acf {
	unsigned long		word;
	struct {
		unsigned long	ACF			:10;
		unsigned long				:22;
	} bit;
};

/* L1_SUBSTATES_OFF (Offset 0B44h) */
union io_ppl_l1s {
	unsigned long		word;
	struct {
		unsigned long	L1TPO		:2;
		unsigned long	L1TL12		:4;
		unsigned long	L1TPA		:2;
		unsigned long				:24;
	} bit;
};

/* PF0_PORT_LOGIC */
struct io_ppl {
	union io_ppl_alt		ALT;							/* +0700 - 0703h */
	union io_ppl_vsd		VSD;							/* +0704 - 0707h */
	union io_ppl_pf			PF;								/* +0708 - 070Bh */
	union io_ppl_afac		AFAC;							/* +070C - 070Fh */
	union io_ppl_plc		PLC;							/* +0710 - 0713h */
	union io_ppl_ls			LS;								/* +0714 - 0717h */
	union io_ppl_tcmfn		TCMFN;							/* +0718 - 071Bh */
	union io_ppl_stf1		STF1;							/* +071C - 071Fh */
	union io_ppl_fm2		FM2;							/* +0720 - 0723h */
	union io_ppl_amodnsrc	AMODNSRC;						/* +0724 - 0727h */
	union io_ppl_pd0		PD0;							/* +0728 - 072Bh */
	union io_ppl_pd1		PD1;							/* +072C - 072Fh */
	union io_ppl_tpfcs		TPFCS;							/* +0730 - 0733h */
	union io_ppl_tnfcs		TNFCS;							/* +0734 - 0737h */
	union io_ppl_tcfcs		TCFCS;							/* +0738 - 073Bh */
	union io_ppl_qs			QS;								/* +073C - 073Fh */
	union io_ppl_vta1		VTA1;							/* +0740 - 0743h */
	union io_ppl_vta2		VTA2;							/* +0744 - 0747h */
	union io_ppl_vprqc		VPRQC;							/* +0748 - 074Bh */
	union io_ppl_vnrqc		VNRQC;							/* +074C - 074Fh */
	union io_ppl_vcrqc		VCRQC;							/* +0750 - 0753h */
	unsigned char			dmy_0754_080B[0x080C-0x0754];	/* +0754 - 080Bh */
	union io_ppl_gen2c		GEN2C;							/* +080C - 080Fh */
	union io_ppl_phys		PHYS;							/* +0810 - 0813h */
	union io_ppl_phyc		PHYC;							/* +0814 - 0817h */
	unsigned char			dmy_0818_081F[0x0820-0x0818];	/* +0818 - 081Fh */
	union io_ppl_mca		MCA;							/* +0820 - 0823h */
	union io_ppl_mcua		MCUA;							/* +0824 - 0827h */
	union io_ppl_mci0e		MCI0E;							/* +0828 - 082Bh */
	union io_ppl_mci0m		MCI0M;							/* +082C - 082Fh */
	union io_ppl_mci0s		MCI0S;							/* +0830 - 0833h */
	unsigned char			dmy_0834_0887[0x0888-0x0834];	/* +0834 - 0887h */
	union io_ppl_mgi		MGI;							/* +0888 - 088Bh */
	unsigned char			dmy_088C_08B7[0x08B8-0x088C];	/* +088C - 08B7h */
	union io_ppl_pilc		PILC;							/* +08B8 - 08BBh */
	union io_ppl_mc1		MC1;							/* +08BC - 08BFh */
	union io_ppl_mlc		MLC;							/* +08C0 - 08C3h */
	union io_ppl_phyic		PHYIC;							/* +08C4 - 08C7h */
	unsigned char			dmy_08C8_08CB[0x08CC-0x08C8];	/* +08C8 - 08CBh */
	union io_ppl_lfc		LFC;							/* +08CC - 08CFh */
	union io_ppl_aerdr		AERDR;							/* +08D0 - 08D3h */
	union io_ppl_amlt		AMLT;							/* +08D4 - 08D7h */
	union io_ppl_aoc		AOC;							/* +08D8 - 08DBh */
	unsigned char			dmy_08DC_08DF[0x08E0-0x08DC];	/* +08DC - 08DFh */
	union io_ppl_cc1		CC1;							/* +08E0 - 08E3h */
	union io_ppl_cc2		CC2;							/* +08E4 - 08E7h */
	union io_ppl_cc3		CC3;							/* +08E8 - 08EBh */
	unsigned char			dmy_08EC_08EF[0x08F0-0x08EC];	/* +08EC - 08EFh */
	union io_ppl_ammal		AMMAL;							/* +08F0 - 08F3h */
	union io_ppl_ammah		AMMAH;							/* +08F4 - 08F7h */
	unsigned char			dmy_08F8_08FF[0x0900-0x08F8];	/* +08F8 - 08FFh */
	union io_ppl_iv			IV;								/* +0900 - 0903h */
	union io_ppl_irc1		IRC1;							/* +0904 - 0907h */
	union io_ppl_irc2		IRC2;							/* +0908 - 090Bh */
	union io_ppl_ilba		ILBA;							/* +090C - 090Fh */
	union io_ppl_iuba		IUBA;							/* +0910 - 0913h */
	union io_ppl_ila		ILA;							/* +0914 - 0917h */
	union io_ppl_ilta		ILTA;							/* +0918 - 091Bh */
	union io_ppl_iuta		IUTA;							/* +091C - 091Fh */
	unsigned char			dmy_0920_096F[0x0970-0x0920];	/* +0920 - 096Fh */
	union io_ppl_dcdap		DCDAP;							/* +0970 - 0973h */
	unsigned char			dmy_0974_0977[0x0978-0x0974];	/* +0974 - 0977h */
	union io_ppl_dc			DC;								/* +0978 - 097Bh */
	union io_ppl_dwee		DWEE;							/* +097C - 097Fh */
	union io_ppl_dwd		DWD;							/* +0980 - 0983h */
	unsigned char			dmy_0984_0987[0x0988-0x0984];	/* +0984 - 0987h */
	union io_ppl_dwcawl		DWCAWL;							/* +0988 - 098Bh */
	union io_ppl_dwcawh		DWCAWH;							/* +098C - 098Fh */
	unsigned char			dmy_0990_099B[0x099C-0x0990];	/* +0990 - 099Bh */
	union io_ppl_dree		DREE;							/* +099C - 099Fh */
	union io_ppl_drd		DRD;							/* +09A0 - 09A3h */
	unsigned char			dmy_09A4_09A7[0x09A8-0x09A4];	/* +09A4 - 09A7h */
	union io_ppl_drcawl		DRCAWL;							/* +09A8 - 09ABh */
	union io_ppl_drcawh		DRCAWH;							/* +09AC - 09AFh */
	unsigned char			dmy_09B0_09BB[0x09BC-0x09B0];	/* +09B0 - 09BBh */
	union io_ppl_dwis		DWIS;							/* +09BC - 09BFh */
	unsigned char			dmy_09C0_09C3[0x09C4-0x09C0];	/* +09C0 - 09C3h */
	union io_ppl_dwim		DWIM;							/* +09C4 - 09C7h */
	union io_ppl_dwic		DWIC;							/* +09C8 - 09CBh */
	union io_ppl_dwes		DWES;							/* +09CC - 09CFh */
	union io_ppl_dwdil		DWDIL;							/* +09D0 - 09D3h */
	union io_ppl_dwdih		DWDIH;							/* +09D4 - 09D7h */
	union io_ppl_dwail		DWAIL;							/* +09D8 - 09DBh */
	union io_ppl_dwaih		DWAIH;							/* +09DC - 09DFh */
	union io_ppl_dwc01id	DWC01ID;						/* +09E0 - 09E3h */
	union io_ppl_dwc23id	DWC23ID;						/* +09E4 - 09E7h */
	union io_ppl_dwc45id	DWC45ID;						/* +09E8 - 09EBh */
	union io_ppl_dwch67id	DWCH67ID;						/* +09EC - 09EFh */
	unsigned char			dmy_09F0_09FF[0x0A00-0x09F0];	/* +09F0 - 09FFh */
	union io_ppl_dwllee		DWLLEE;							/* +0A00 - 0A03h */
	unsigned char			dmy_0A04_0A0F[0x0A10-0x0A04];	/* +0A04 - 0A0Fh */
	union io_ppl_dris		DRIS;							/* +0A10 - 0A13h */
	unsigned char			dmy_0A14_0A17[0x0A18-0x0A14];	/* +0A14 - 0A17h */
	union io_ppl_drim		DRIM;							/* +0A18 - 0A1Bh */
	union io_ppl_dric		DRIC;							/* +0A1C - 0A1Fh */
	unsigned char			dmy_0A20_0A23[0x0A24-0x0A20];	/* +0A20 - 0A23h */
	union io_ppl_dresl		DRESL;							/* +0A24 - 0A27h */
	union io_ppl_dresh		DRESH;							/* +0A28 - 0A2Bh */
	unsigned char			dmy_0A2C_0A33[0x0A34-0x0A2C];	/* +0A2C - 0A33h */
	union io_ppl_drllee		DRLLEE;							/* +0A34 - 0A37h */
	unsigned char			dmy_0A38_0A3B[0x0A3C-0x0A38];	/* +0A38 - 0A3Bh */
	union io_ppl_drdil		DRDIL;							/* +0A3C - 0A3Fh */
	union io_ppl_drdih		DRDIH;							/* +0A40 - 0A43h */
	union io_ppl_drail		DRAIL;							/* +0A44 - 0A47h */
	union io_ppl_draih		DRAIH;							/* +0A48 - 0A4Bh */
	union io_ppl_drch01id	DRCH01ID;						/* +0A4C - 0A4Fh */
	union io_ppl_drch23id	DRCH23ID;						/* +0A50 - 0A53h */
	union io_ppl_drch45id	DRCH45ID;						/* +0A54 - 0A57h */
	union io_ppl_drch67id	DRCH67ID;						/* +0A58 - 0A5Bh */
	unsigned char			dmy_0A5C_0A6B[0x0A6C-0x0A5C];	/* +0A5C - 0A6Bh */
	union io_ppl_dvs		DVS;							/* +0A6C - 0A6Fh */
	union io_ppl_dcc1		DCC1;							/* +0A70 - 0A73h */
	unsigned char			dmy_0A74_0A77[0x0A78-0x0A74];	/* +0A74 - 0A77h */
	union io_ppl_dts		DTS;							/* +0A78 - 0A7Bh */
	union io_ppl_dsl		DSL;							/* +0A7C - 0A7Fh */
	union io_ppl_dsh		DSH;							/* +0A80 - 0A83h */
	union io_ppl_ddl		DDL;							/* +0A84 - 0A87h */
	union io_ppl_ddh		DDH;							/* +0A88 - 0A8Bh */
	union io_ppl_dll		DLL;							/* +0A8C - 0A8Fh */
	union io_ppl_dlh		DLH;							/* +0A90 - 0A93h */
	unsigned char			dmy_0A94_0B2F[0x0B30-0x0A94];	/* +0A94 - 0B2Fh */
	union io_ppl_pll		PLL;							/* +0B30 - 0B33h */
	unsigned char			dmy_0B34_0B3F[0x0B40-0x0B34];	/* +0B34 - 0B3Fh */
	union io_ppl_acf		ACF;							/* +0B40 - 0B43h */
	union io_ppl_l1s		L1S;							/* +0B44 - 0B47h */
};

/* core register of Root Complex */
struct io_pcie_core_rc {
	struct io_t1csh			T1CSH;							/* +0000 - 003Fh */
	struct io_pmcr			PMCR;							/* +0040 - 004Fh */
	struct io_msicr			MSICR;							/* +0050 - 0067h */
	unsigned char			dmy_0068_006F[0x0070-0x0068];	/* +0068 - 006Fh */
	struct io_pciecr		PCIECR;							/* +0070 - 00AFh */
	unsigned char			dmy_00B0_00AF[0x0100-0x00B0];	/* +00B0 - 00FFh */
	struct io_aerc			AERC;							/* +0100 - 0147h */
	struct io_ltrc			LTRC;							/* +0148 - 014Fh */
	struct io_l1pmsec		L1PMSEC;						/* +0150 - 015Fh */
	unsigned char			dmy_0160_06FF[0x0700-0x0160];	/* +0160 - 06FFh */
	struct io_ppl			PPL;							/* +0700 - 0B47 */
};

/* core register of Endpoint */
struct io_pcie_core_ep {
	struct io_t0csh			T0CSH;							/* +0000 - 003Fh */
	struct io_pmcr			PMCR;							/* +0040 - 004Fh */
	struct io_msicr			MSICR;							/* +0050 - 0067h */
	unsigned char			dmy_0068_006F[0x0070-0x0068];	/* +0068 - 006Fh */
	struct io_pciecr		PCIECR;							/* +0070 - 00AFh */
	unsigned char			dmy_00B0_00AF[0x0100-0x00B0];	/* +00B0 - 00FFh */
	struct io_aerc			AERC;							/* +0100 - 0147h */
	struct io_ltrc			LTRC;							/* +0148 - 014Fh */
	struct io_l1pmsec		L1PMSEC;						/* +0150 - 015Fh */
	unsigned char			dmy_0160_06FF[0x0700-0x0160];	/* +0160 - 06FFh */
	struct io_ppl			PPL;							/* +0700 - 0B47h */
};

/* CORE Register mapping */
union io_pcie_core	{
	struct io_pcie_core_rc	RC;		/* Root Complex */
	struct io_pcie_core_ep	EP;		/* Endpoint     */
};

/*----------------------------------------------*/
/*	Macro Register								*/
/*----------------------------------------------*/
/* CORE_CONTROL (Offset 000h) */
union io_pcie_core_control {
	unsigned long		word;
	struct {
		unsigned long	DEVICE_TYPE			:4;
		unsigned long	APP_LTSSM_ENABLE	:1;
		unsigned long						:3;
		unsigned long	APP_REQ_RETRY_EN	:1;
		unsigned long						:7;
		unsigned long	RX_LANE_FLIP_EN		:1;
		unsigned long						:3;
		unsigned long	TX_LANE_FLIP_EN		:1;
		unsigned long						:11;
	} bit;
};

/* AXI_CLK_STOP (Offset 004h) */
union io_pcie_axi_clk_stop {
	unsigned long		word;
	struct {
		unsigned long	MSTR_ACLK_STOP		:1;
		unsigned long	MSTR_CSYSREQ_REG	:1;
		unsigned long	MSTR_CSYSACK_REG	:1;
		unsigned long	MSTR_CACTIVE_REG	:1;
		unsigned long	SLV_ACLK_STOP		:1;
		unsigned long	SLV_CSYSREQ_REG		:1;
		unsigned long	SLV_CSYSACK_REG		:1;
		unsigned long	SLV_CACTIVE_REG		:1;
		unsigned long	DBI_ACLK_STOP		:1;
		unsigned long	DBI_CSYSREQ_REG		:1;
		unsigned long	DBI_CSYSACK_REG		:1;
		unsigned long	DBI_CACTIVE_REG		:1;
		unsigned long						:20;
	} bit;
};

/* CLOCK_CONTROL (Offset 006h) */
union io_pcie_clock_control {
	unsigned long		word;
	struct {
		unsigned long	APP_CLK_REQ_N		:1;
		unsigned long						:7;
		unsigned long	EXT_PCLK_REQ		:1;
		unsigned long						:3;
		unsigned long	CLK_SWITCH_EN		:1;
		unsigned long						:19;
	} bit;
};

/* RESET_CONTROL_1 (Offset 00Ch) */
union io_pcie_reset_control_1 {
	unsigned long		word;
	struct {
		unsigned long	PWUP_RST_N_REG		:1;
		unsigned long	BUTTON_RST_N_REG	:1;
		unsigned long						:2;
		unsigned long	PERST_N_I_REG		:1;
		unsigned long	PERST_N_O_REG		:1;
		unsigned long						:2;
		unsigned long	PIPE_RST_N_REG		:1;
		unsigned long						:3;
		unsigned long	CORE_RST_N_REG		:1;
		unsigned long						:3;
		unsigned long	N_STI_RST_N_REG		:1;
		unsigned long						:3;
		unsigned long	STI_RST_N_REG		:1;
		unsigned long						:3;
		unsigned long	PWR_RST_N_REG		:1;
		unsigned long						:3;
		unsigned long	PHY_RST_N_REG		:1;
		unsigned long	SQU_RST_N_REG		:1;
		unsigned long						:2;
	} bit;
};

/* RESET_CONTROL_2 (Offset 010h) */
union io_pcie_reset_control_2 {
	unsigned long		word;
	struct {
		unsigned long	MSTR_ARST_N_REG		:1;
		unsigned long						:3;
		unsigned long	SLV_ARST_N_REG		:1;
		unsigned long						:3;
		unsigned long	DBI_ARST_N_REG		:1;
		unsigned long						:7;
		unsigned long	APP_INIT_RST		:1;
		unsigned long						:15;
	} bit;
};

/* RESET_SELECT_1 (Offset 014h) */
union io_pcie_reset_select_1 {
	unsigned long		word;
	struct {
		unsigned long	PWUP_RST_SEL		:1;
		unsigned long	BUTTON_RST_SEL		:1;
		unsigned long						:2;
		unsigned long	PERST_SEL			:1;
		unsigned long						:3;
		unsigned long	PIPE_RST_SEL		:1;
		unsigned long						:3;
		unsigned long	CORE_RST_SEL		:1;
		unsigned long						:3;
		unsigned long	N_STI_RST_SEL		:1;
		unsigned long						:3;
		unsigned long	STI_RST_SEL			:1;
		unsigned long						:3;
		unsigned long	PWR_RST_SEL			:1;
		unsigned long						:3;
		unsigned long	PHY_RST_SEL			:1;
		unsigned long	SQU_RST_SEL			:1;
		unsigned long						:2;
	} bit;
};

/* RESET_SELECT_2 (Offset 018h) */
union io_pcie_reset_select_2 {
	unsigned long		word;
	struct {
		unsigned long	MSTR_ARST_SEL		:1;
		unsigned long						:3;
		unsigned long	SLV_ARST_SEL		:1;
		unsigned long						:3;
		unsigned long	DBI_ARST_SEL		:1;
		unsigned long						:23;
	} bit;
};

/* MSI_REQUEST_1 (Offset 01Ch) */
union io_pcie_msi_request_1 {
	unsigned long		word;
	struct {
		unsigned long	LOCAL_INTERRUPT		:32;
	} bit;
};

/* MSI_REQUEST_2 (Offset 020h) */
union io_pcie_msi_request_2 {
	unsigned long		word;
	struct {
		unsigned long	MSI_FUNC_NUM		:3;
		unsigned long						:1;
		unsigned long	MSI_TC				:3;
		unsigned long						:25;
	} bit;
};

/* INTERRUPT_CONTROL_1 (Offset 024h) */
union io_pcie_int_control_1 {
	unsigned long		word;
	struct {
		unsigned long	SYS_INT				:1;
		unsigned long						:15;
		unsigned long	INT_SYNCBYP			:1;
		unsigned long						:11;
		unsigned long	INTA_STATUS			:1;
		unsigned long	INTB_STATUS			:1;
		unsigned long	INTC_STATUS			:1;
		unsigned long	INTD_STATUS			:1;
	} bit;
};

/* INTERRUPT_CONTROL_2 (Offset 028h) */
union io_pcie_int_control_2 {
	unsigned long		word;
	struct {
		unsigned long	SEND_COR_ERR_EN		:1;
		unsigned long	SEND_COR_ERR_CEN	:1;
		unsigned long	SEND_COR_ERR_STT	:1;
		unsigned long	SEND_COR_ERR_M		:1;
		unsigned long	SEND_NF_ERR_EN		:1;
		unsigned long	SEND_NF_ERR_CEN		:1;
		unsigned long	SEND_NF_ERR_STT		:1;
		unsigned long	SEND_NF_ERR_M		:1;
		unsigned long	SEND_F_ERR_EN		:1;
		unsigned long	SEND_F_ERR_CEN		:1;
		unsigned long	SEND_F_ERR_STT		:1;
		unsigned long	SEND_F_ERR_M		:1;
		unsigned long	SYS_ERR_RC_EN		:1;
		unsigned long	SYS_ERR_RC_CEN		:1;
		unsigned long	SYS_ERR_RC_STT		:1;
		unsigned long	SYS_ERR_RC_M		:1;
		unsigned long	RDLH_LINK_UP_EN		:1;
		unsigned long	RDLH_LINK_UP_CEN	:1;
		unsigned long	RDLH_LINK_UP_STT	:1;
		unsigned long	RDLH_LINK_UP_M		:1;
		unsigned long	WAKE_EN				:1;
		unsigned long	WAKE_CLEAR_EN		:1;
		unsigned long	WAKE_STATUS			:1;
		unsigned long	WAKE_MASK			:1;
		unsigned long	VENDOR_MSG_EN		:1;
		unsigned long	VENDOR_MSG_CEN		:1;
		unsigned long	VENDOR_MSG_STT		:1;
		unsigned long	VENDOR_MSG_M		:1;
		unsigned long	SMLH_REQ_RST_EN		:1;
		unsigned long	SMLH_REQ_RST_CEN	:1;
		unsigned long	SMLH_REQ_RST_STT	:1;
		unsigned long	SMLH_REQ_RST_M		:1;
	} bit;
};

/* INTERRUPT_CONTROL_3 (Offset 02Ch) */
union io_pcie_int_control_3 {
	unsigned long		word;
	struct {
		unsigned long	RADM_PM_PME_EN		:1;
		unsigned long	RADM_PM_PME_CEN		:1;
		unsigned long	RADM_PM_PME_STT		:1;
		unsigned long	RADM_PM_PME_M		:1;
		unsigned long	RADM_MSG_UL_EN		:1;
		unsigned long	RADM_MSG_UL_CEN		:1;
		unsigned long	RADM_MSG_UL_STT		:1;
		unsigned long	RADM_MSG_UL_M		:1;
		unsigned long	RADM_COR_ERR_EN		:1;
		unsigned long	RADM_COR_ERR_CEN	:1;
		unsigned long	RADM_COR_ERR_STT	:1;
		unsigned long	RADM_COR_ERR_M		:1;
		unsigned long	RADM_NF_ERR_EN		:1;
		unsigned long	RADM_NF_ERR_CEN		:1;
		unsigned long	RADM_NF_ERR_STT		:1;
		unsigned long	RADM_NF_ERR_M		:1;
		unsigned long	RADM_F_ERR_EN		:1;
		unsigned long	RADM_F_ERR_CEN		:1;
		unsigned long	RADM_F_ERR_STT		:1;
		unsigned long	RADM_F_ERR_M		:1;
		unsigned long	RADM_MSG_LTR_EN		:1;
		unsigned long	RADM_MSG_LTR_CEN	:1;
		unsigned long	RADM_MSG_LTR_STT	:1;
		unsigned long	RADM_MSG_LTR_M		:1;
		unsigned long	RADM_PM2ACK_EN		:1;
		unsigned long	RADM_PM2ACK_CEN		:1;
		unsigned long	RADM_PM2ACK_STT		:1;
		unsigned long	RADM_PM2ACK_M		:1;
		unsigned long	RADM_PM_TOFF_EN		:1;
		unsigned long	RADM_PM_TOFF_CEN	:1;
		unsigned long	RADM_PM_TOFF_STT	:1;
		unsigned long	RADM_PM_TOFF_M		:1;
	} bit;
};

/* ELECTROMECHANICAL_CONTROL (Offset 030h) */
union io_pcie_em_control {
	unsigned long		word;
	struct {
		unsigned long	ATTEN_B_PRESSED		:1;
		unsigned long						:3;
		unsigned long	PRE_DET_STT_REG		:1;
		unsigned long						:3;
		unsigned long	MRL_SENSOR_STT		:1;
		unsigned long						:3;
		unsigned long	PWR_FAULT_DET		:1;
		unsigned long						:3;
		unsigned long	MRL_SENSOR_CHGED	:1;
		unsigned long						:3;
		unsigned long	PRE_DET_CHGED		:1;
		unsigned long						:3;
		unsigned long	CMD_CPLED_INT		:1;
		unsigned long						:3;
		unsigned long	EML_IL_ENGAGED		:1;
		unsigned long						:3;
	} bit;
};

/* ELECTROMECHANICAL_SELECT (Offset 034h) */
union io_pcie_em_select {
	unsigned long		word;
	struct {
		unsigned long						:4;
		unsigned long	PRE_DET_STT_SEL		:1;
		unsigned long						:27;
	} bit;
};

/* ELECRTOMECHANICAL_MONITOR (Offset 038h) */
union io_pcie_em_monitor {
	unsigned long		word;
	struct {
		unsigned long	CFG_EML_CONTROL		:1;
		unsigned long						:31;
	} bit;
};

/* PM_UNLOCK_MESSAGE_REQUEST (Offset 03Ch) */
union io_pcie_pm_unlock_req {
	unsigned long		word;
	struct {
		unsigned long	APP_UNLOCK_MSG		:1;
		unsigned long						:3;
		unsigned long	PM_XMT_TURNOFF		:1;
		unsigned long						:11;
		unsigned long	LTR_MSG_REQ			:1;
		unsigned long						:11;
		unsigned long	LTR_MSG_FUNC_NUM	:3;
		unsigned long						:1;
	} bit;
};

/* APP_LTR_MSG (Offset 040h) */
union io_pcie_ltr_msg {
	unsigned long		word;
	struct {
		unsigned long	LTR_MSG_LATENCY		:32;
	} bit;
};

/* LTR_MONITOR_1 (Offset 044h) */
union io_pcie_ltr_monitor_1 {
	unsigned long		word;
	struct {
		unsigned long	APP_LTR_MSG_GR		:1;
		unsigned long						:30;
		unsigned long	APP_LTR_MSG_RST		:1;
	} bit;
};

/* LTR_MONITOR_2 (Offset 048h) */
union io_pcie_ltr_monitor_2 {
	unsigned long		word;
	struct {
		unsigned long	APP_LTR_LATENCY		:32;
	} bit;
};

/* PM_CONTROL_1 (Offset 04Ch) */
union io_pcie_pm_control_1 {
	unsigned long		word;
	struct {
		unsigned long	WAKE_REG			:1;
		unsigned long						:15;
		unsigned long	CLKREQ_REG			:1;
		unsigned long						:15;
	} bit;
};

/* PM_CONTROL_2 (Offset 050h) */
union io_pcie_pm_control_2 {
	unsigned long		word;
	struct {
		unsigned long	OUTBAND_PWUP_CMD	:1;
		unsigned long						:3;
		unsigned long	APPS_PM_XMT_PME		:1;
		unsigned long						:3;
		unsigned long	SYS_AUX_PWR_DET		:1;
		unsigned long						:3;
		unsigned long	APP_REQ_ENTR_L1		:1;
		unsigned long						:3;
		unsigned long	PP_RDY_ENTR_L23		:1;
		unsigned long						:3;
		unsigned long	APP_REQ_EXIT_L1		:1;
		unsigned long						:3;
		unsigned long	PP_CLK_PM_EN		:1;
		unsigned long						:3;
		unsigned long	APP_XFER_PENDING	:1;
		unsigned long						:3;
	} bit;
};

/* PM_SELECT_1 (Offset 054h) */
union io_pcie_pm_select_1 {
	unsigned long		word;
	struct {
		unsigned long	WAKE_SEL			:1;
		unsigned long						:15;
		unsigned long	CLKREQ_SEL			:1;
		unsigned long						:15;
	} bit;
};

/* PM_MONITOR_1 (Offset 058h) */
union io_pcie_pm_monitor_1 {
	unsigned long		word;
	struct {
		unsigned long	PM_CURNT_STATE		:3;
		unsigned long						:1;
		unsigned long	PM_DSTATE			:3;
		unsigned long						:1;
		unsigned long	PM_STATUS			:1;
		unsigned long						:3;
		unsigned long	PM_PME_EN			:1;
		unsigned long						:3;
		unsigned long	SMLH_LTSSM_STATE	:6;
		unsigned long						:2;
		unsigned long	AUX_PM_EN			:1;
		unsigned long						:7;
	} bit;
};

/* PM_MONITOR_2 (Offset 05Ch) */
union io_pcie_pm_monitor_2 {
	unsigned long		word;
	struct {
		unsigned long	PM_LINKST_IN_L0S	:1;
		unsigned long						:3;
		unsigned long	PM_LINKST_IN_L1		:1;
		unsigned long						:3;
		unsigned long	PM_LINKST_IN_L2		:1;
		unsigned long						:3;
		unsigned long	PM_LINKST_L2_E		:1;
		unsigned long						:3;
		unsigned long	PM_LINKST_IN_L1S	:1;
		unsigned long						:3;
		unsigned long	CFG_L1SUB_EN		:1;
		unsigned long						:7;
		unsigned long	CLKREQ_MONITOR		:1;
		unsigned long						:3;
	} bit;
};

/* PHY_CONFIG_COMMON_1 (Offset 100h) */
union io_pcie_config_com_1 {
	unsigned long		word;
	struct {
		unsigned long	PIPE_TX2RX_LPBK		:1;
		unsigned long	PHY_RX2TX_LB_EN		:1;
		unsigned long						:2;
		unsigned long	PG_MODE_EN			:1;
		unsigned long	PMA_PW_FOR_EN		:1;
		unsigned long	PCS_PW_FOR_EN		:1;
		unsigned long						:1;
		unsigned long	PHY_MPLLA_FOR_EN	:1;
		unsigned long	PHY_MPLLA_SSC_EN	:1;
		unsigned long	PHY_MPLLB_FOR_EN	:1;
		unsigned long	PHY_MPLLB_SSC_EN	:1;
		unsigned long	PHY_EXT_CTRL_SEL	:1;
		unsigned long	PHY_RTUNE_REQ		:1;
		unsigned long						:2;
		unsigned long	PCS_PSO_S_CNT		:8;
		unsigned long	PCS_STABLE_CNT		:8;
	} bit;
};

/* PHY_CONFIG_COMMON_2 (Offset 104h) */
union io_pcie_config_com_2 {
	unsigned long		word;
	struct {
		unsigned long	PHY_MPLL_MUL		:7;
		unsigned long						:1;
		unsigned long	PHY_REF_CLKDIV2		:1;
		unsigned long						:11;
		unsigned long	RX0_SRIS_MODE_EN	:1;
		unsigned long						:11;
	} bit;
};

/* PHY_CONFIG_COMMON_3 (Offset 108h) */
union io_pcie_config_com_3 {
	unsigned long		word;
	struct {
		unsigned long	TX_DEEM_G1			:6;
		unsigned long						:2;
		unsigned long	TX_DEEM_G2_3P5DB	:6;
		unsigned long						:2;
		unsigned long	TX_DEEM_G2_6DB		:6;
		unsigned long						:10;
	} bit;
};

/* PHY_CONFIG_COMMON_4 (Offset 10Ch) */
union io_pcie_config_com_4 {
	unsigned long		word;
	struct {
		unsigned long	TX_SWING_FULL		:7;
		unsigned long						:1;
		unsigned long	TX_SWING_LOW		:7;
		unsigned long						:1;
		unsigned long	COMMON_CLOCKS		:1;
		unsigned long						:15;
	} bit;
};

/* PHY_CONFIG_COMMON_5 (Offset 110h) */
union io_pcie_config_com_5 {
	unsigned long		word;
	struct {
		unsigned long	PHY_TX0_TERM_OFS	:5;
		unsigned long						:3;
		unsigned long	PHY_TX1_TERM_OFS	:5;
		unsigned long						:3;
		unsigned long	PHY_RX0_EQ			:3;
		unsigned long						:5;
		unsigned long	PHY_RX1_EQ			:3;
		unsigned long						:5;
	} bit;
};

/* PHY_CONFIG_COMMON_6 (Offset 114h) */
union io_pcie_config_com_6 {
	unsigned long		word;
	struct {
		unsigned long	PIPE_PORT_SEL		:2;
		unsigned long						:2;
		unsigned long	PHY_TX_VBOOST_LV	:3;
		unsigned long						:9;
		unsigned long	PHY_LOS_LEVEL		:5;
		unsigned long						:3;
		unsigned long	PHY_LOS_BIAS		:3;
		unsigned long						:5;
	} bit;
};

/* CFG_MONITOR_1 (Offset 1D8h) */
union io_pcie_cfg_monitor_1 {
	unsigned long		word;
	struct {
		unsigned long	CFG_BUS_MSTR_EN		:1;
		unsigned long						:3;
		unsigned long	CFG_MEM_SPACE_EN	:1;
		unsigned long						:3;
		unsigned long	CFG_MAX_RD_SIZE		:3;
		unsigned long						:1;
		unsigned long	CFG_MAX_PL_SIZE		:3;
		unsigned long						:1;
		unsigned long	CFG_RCB				:1;
		unsigned long						:1;
		unsigned long	CFG_PWR_CTRLERC		:1;
		unsigned long						:1;
		unsigned long	CFG_PM_NO_SRST		:1;
		unsigned long						:3;
		unsigned long	CFG_PWR_IND			:2;
		unsigned long						:2;
		unsigned long	CFG_ATTEN_IND		:2;
		unsigned long						:2;
	} bit;
};

/* CFG_MONITOR_2 (Offset 1DCh) */
union io_pcie_cfg_monitor_2 {
	unsigned long		word;
	struct {
		unsigned long	CFG_PHY_CONTROL		:32;
	} bit;
};

/* CFG_MONITOR_3 (Offset 1E0h) */
union io_pcie_cfg_monitor_3 {
	unsigned long		word;
	struct {
		unsigned long	CFG_PBUS_NUM		:8;
		unsigned long	CFG_PBUS_DEV_NUM	:5;
		unsigned long						:3;
		unsigned long	CFG_2NDBUS_NUM		:8;
		unsigned long	CFG_SUBBUS_NUM		:8;
	} bit;
};

/* CFG_MONITOR_4 (Offset 1E4h) */
union io_pcie_cfg_monitor_4 {
	unsigned long		word;
	struct {
		unsigned long	CFG_LTR_MAX_LAT		:32;
	} bit;
};

/* CFG_MONITOR_5 (Offset 1E8h) */
union io_pcie_cfg_monitor_5 {
	unsigned long		word;
	struct {
		unsigned long	CFG_LTR_M_EN		:1;
		unsigned long						:3;
		unsigned long	CFG_NO_SNOOP_EN		:1;
		unsigned long						:3;
		unsigned long	CFG_RELAX_ORDER		:1;
		unsigned long						:7;
		unsigned long	CFG_2ND_RESET		:1;
		unsigned long						:15;
	} bit;
};

/* DEBUG_MONITOR_1 (Offset 1ECh) */
union io_pcie_debug_monitor_1 {
	unsigned long		word;
	struct {
		unsigned long						:4;
		unsigned long	RTLH_RFC_UPD		:1;
		unsigned long						:3;
		unsigned long	BRDG_SLV_XFER_PE	:1;
		unsigned long						:3;
		unsigned long	BRDG_DBI_XFER_PE	:1;
		unsigned long						:3;
		unsigned long	EDMA_XFER_PE		:1;
		unsigned long						:3;
		unsigned long	RADM_XFER_PE		:1;
		unsigned long						:3;
		unsigned long	RADM_Q_NOT_EMPTY	:1;
		unsigned long						:3;
		unsigned long	RADM_QOVERFLOW		:1;
		unsigned long						:3;
	} bit;
};

/* DEBUG_MONITOR_2 (Offset 1F0h) */
union io_pcie_debug_monitor_2 {
	unsigned long		word;
	struct {
		unsigned long	RTLH_RFC_DATA		:32;
	} bit;
};

/* DEBUG_MONITOR_3 (Offset 1F4h) */
union io_pcie_debug_monitor_3 {
	unsigned long		word;
	struct {
		unsigned long	CXPL_D_INFO_EI		:16;
		unsigned long						:16;
	} bit;
};

/* DEBUG_MONITOR_4 (Offset 1F8h) */
struct io_pcie_debug_monitor_4 {
	unsigned long		CXPL_D_INFO_L;				/* 1F8 - 1FBh */
	unsigned long		CXPL_D_INFO_U;				/* 1FC - 1FFh */
};

/* MSG_MONITOR_1 (Offset 200h) */
union io_pcie_msg_monitor_1 {
	unsigned long		word;
	struct {
		unsigned long						:16;
		unsigned long	RADM_MSG_REQ_ID		:16;
	} bit;
};

/* MSG_MONITOR_3 (Offset 208h) */
struct io_pcie_msg_monitor_3 {
	unsigned long		RADM_MSG_PAYLOAD_L;			/* 208 - 20Bh */
	unsigned long		RADM_MSG_PAYLOAD_U;			/* 20C - 20Fh */
};

/* LINK_MONITOR (Offset 210h) */
union io_pcie_link_monitor {
	unsigned long		word;
	struct {
		unsigned long	SMLH_LINK_UP		:1;
		unsigned long						:3;
		unsigned long	SMLH_REQ_RST_NOT	:1;
		unsigned long						:27;
	} bit;
};

/* TIMEOUT_MONITOR_1 (Offset 218h) */
union io_pcie_to_monitor_1 {
	unsigned long		word;
	struct {
		unsigned long	RADM_TO_FUNC_NU		:3;
		unsigned long						:1;
		unsigned long	RADM_TO_CPL_TC		:3;
		unsigned long						:1;
		unsigned long	RADM_TO_CPL_TAG		:8;
		unsigned long	RADM_TO_CPL_ATTR	:2;
		unsigned long						:2;
		unsigned long	RADM_TO_CPL_LEN		:12;
	} bit;
};

/* TIMEOUT_MONITOR_2 (Offset 21Ch) */
union io_pcie_to_monitor_2 {
	unsigned long		word;
	struct {
		unsigned long	RADM_CPL_TIMEOUT	:1;
		unsigned long						:31;
	} bit;
};

/* TIMEOUT_MONITOR_3 (Offset 220h) */
union io_pcie_to_monitor_3 {
	unsigned long		word;
	struct {
		unsigned long	TRGT_TO_CPL_FNUM	:3;
		unsigned long						:1;
		unsigned long	TRGT_TO_CPL_TC		:3;
		unsigned long						:1;
		unsigned long	TRGT_TO_LKUP_ID		:8;
		unsigned long	TRGT_TO_CPL_ATTR	:2;
		unsigned long						:2;
		unsigned long	TRGT_TO_CPL_LEN		:12;
	} bit;
};

/* TIMEOUT_MONITOR_4 (Offset 224h) */
union io_pcie_to_monitor_4 {
	unsigned long		word;
	struct {
		unsigned long	TRGT_CPL_TIMEOUT	:1;
		unsigned long						:3;
		unsigned long	TRGT_LKUP_EMPTY		:1;
		unsigned long						:3;
		unsigned long	TRGT_LKUP_ID		:8;
		unsigned long						:16;
	} bit;
};

/* TRANSMIT_MONITOR (Offset 228h) */
union io_pcie_transmit_monitor {
	unsigned long		word;
	struct {
		unsigned long	PM_XTLH_BLK_TLP		:1;
		unsigned long						:31;
	} bit;
};

/* PIPE_MONITOR (Offset 22Ch) */
union io_pcie_pipe_monitor {
	unsigned long		word;
	struct {
		unsigned long	CFG_HW_AUTO_SP_D	:1;
		unsigned long						:3;
		unsigned long	MAC_PHY_RATE		:1;
		unsigned long						:27;
	} bit;
};

/* PHY_MONITOR (Offset 230h) */
union io_pcie_phy_monitor {
	unsigned long		word;
	struct {
		unsigned long						:1;
		unsigned long	PHY0_MPLLA_STATE	:1;
		unsigned long	PHY0_MPLLB_STATE	:1;
		unsigned long						:7;
		unsigned long	PHY_RTUNE_ACK		:1;
		unsigned long						:5;
		unsigned long	PIPE_RX_EBUF_LOC	:9;
		unsigned long						:7;
	} bit;
};

/* CONFIG_MONITOR_BAR0_START (Offset 238h) */
struct io_pcie_monitor_bar0_s {
	unsigned long		CFG_BAR0_START_L;			/* 238 - 23Bh */
	unsigned long		CFG_BAR0_START_U;			/* 23C - 23Fh */
};

/* CONFIG_MONITOR_BAR0_LIMIT (Offset 240h) */
struct io_pcie_monitor_bar0_l {
	unsigned long		CFG_BAR0_LIMIT_L;			/* 240 - 243h */
	unsigned long		CFG_BAR0_LIMIT_U;			/* 244 - 247h */
};

/* CONFIG_MONITOR_BAR1_START (Offset 248h) */
union io_pcie_monitor_bar1_s {
	unsigned long		word;
	struct {
		unsigned long	CFG_BAR1_START		:32;
	} bit;
};

/* CONFIG_MONITOR_BAR1_LIMIT (Offset 24Ch) */
union io_pcie_monitor_bar1_l {
	unsigned long		word;
	struct {
		unsigned long	CFG_BAR1_LIMIT		:32;
	} bit;
};

/* CONFIG_MONITOR_BAR2_START (Offset 250h) */
struct io_pcie_monitor_bar2_s {
	unsigned long		CFG_BAR2_START_L;			/* 250 - 253h */
	unsigned long		CFG_BAR2_START_U;			/* 254 - 257h */
};

/* CONFIG_MONITOR_BAR2_LIMIT (Offset 258h) */
struct io_pcie_monitor_bar2_l {
	unsigned long		CFG_BAR2_LIMIT_L;			/* 258 - 25Bh */
	unsigned long		CFG_BAR2_LIMIT_U;			/* 25C - 25Fh */
};

/* CONFIG_MONITOR_BAR3_START (Offset 260h) */
union io_pcie_monitor_bar3_s {
	unsigned long		word;
	struct {
		unsigned long	CFG_BAR3_START		:32;
	} bit;
};

/* CONFIG_MONITOR_BAR3_LIMIT (Offset 264h) */
union io_pcie_monitor_bar3_l {
	unsigned long		word;
	struct {
		unsigned long	CFG_BAR3_LIMIT		:32;
	} bit;
};

/* CONFIG_MONITOR_BAR4_START (Offset 268h) */
struct io_pcie_monitor_bar4_s {
	unsigned long		CFG_BAR4_START_L;			/* 258 - 25Bh */
	unsigned long		CFG_BAR4_START_U;			/* 25C - 25Fh */
};

/* CONFIG_MONITOR_BAR4_LIMIT (Offset 270h) */
struct io_pcie_monitor_bar4_l {
	unsigned long		CFG_BAR4_LIMIT_L;			/* 250 - 253h */
	unsigned long		CFG_BAR4_LIMIT_U;			/* 254 - 257h */
};

/* CONFIG_MONITOR_BAR5_START (Offset 278h) */
union io_pcie_monitor_bar5_s {
	unsigned long		word;
	struct {
		unsigned long	CFG_BAR5_START		:32;
	} bit;
};

/* CONFIG_MONITOR_BAR5_LIMIT (Offset 27Ch) */
union io_pcie_monitor_bar5_l {
	unsigned long		word;
	struct {
		unsigned long	CFG_BAR5_LIMIT		:32;
	} bit;
};

/* CONFIG_MONITOR_EXP_ROM_START (Offset 280h) */
union io_pcie_monitor_erom_s {
	unsigned long		word;
	struct {
		unsigned long	CFG_EROM_START		:32;
	} bit;
};

/* CONFIG_MONITOR_EEXP_ROM_LIMIT (Offset 284h) */
union io_pcie_monitor_erom_l {
	unsigned long		word;
	struct {
		unsigned long	CFG_EROM_LIMIT		:32;
	} bit;
};

/* MACRO REGISTER (Offset 000h - FFFh) */
struct io_pcie {
	union io_pcie_core_control		CORE_CONTROL;				/* +000 - 003h */
	union io_pcie_axi_clk_stop		AXI_CLK_STOP;				/* +004 - 007h */
	union io_pcie_clock_control		CLOCK_CONTROL;				/* +008 - 00Bh */
	union io_pcie_reset_control_1	RESET_CONTROL_1;			/* +00C - 00Fh */
	union io_pcie_reset_control_2	RESET_CONTROL_2;			/* +010 - 013h */
	union io_pcie_reset_select_1	RESET_SELECT_1;				/* +014 - 017h */
	union io_pcie_reset_select_2	RESET_SELECT_2;				/* +018 - 01Bh */
	union io_pcie_msi_request_1		MSI_REQUEST_1;				/* +01C - 01Fh */
	union io_pcie_msi_request_2		MSI_REQUEST_2;				/* +020 - 023h */
	union io_pcie_int_control_1		INT_CONTROL_1;				/* +024 - 027h */
	union io_pcie_int_control_2		INT_CONTROL_2;				/* +028 - 02Bh */
	union io_pcie_int_control_3		INT_CONTROL_3;				/* +02C - 02Fh */
	union io_pcie_em_control		EM_CONTROL;					/* +030 - 033h */
	union io_pcie_em_select			EM_SELECT;					/* +034 - 037h */
	union io_pcie_em_monitor		EM_MONITOR;					/* +038 - 03Bh */
	union io_pcie_pm_unlock_req		PM_UNLOCK_REQ;				/* +03C - 03Fh */
	union io_pcie_ltr_msg			LTR_MSG;					/* +040 - 043h */
	union io_pcie_ltr_monitor_1		LTR_MONITOR_1;				/* +044 - 047h */
	union io_pcie_ltr_monitor_2		LTR_MONITOR_2;				/* +048 - 04Bh */
	union io_pcie_pm_control_1		PM_CONTROL_1;				/* +04C - 04Fh */
	union io_pcie_pm_control_2		PM_CONTROL_2;				/* +050 - 053h */
	union io_pcie_pm_select_1		PM_SELECT_1;				/* +054 - 057h */
	union io_pcie_pm_monitor_1		PM_MONITOR_1;				/* +058 - 05Bh */
	union io_pcie_pm_monitor_2		PM_MONITOR_2;				/* +05C - 05Fh */
	unsigned char					dmy_060_0FF[0x100-0x060];	/* +060 - 0FFh */
	union io_pcie_config_com_1		PHY_CONFIG_COM_1;			/* +100 - 103h */
	union io_pcie_config_com_2		PHY_CONFIG_COM_2;			/* +104 - 107h */
	union io_pcie_config_com_3		PHY_CONFIG_COM_3;			/* +108 - 10Bh */
	union io_pcie_config_com_4		PHY_CONFIG_COM_4;			/* +10C - 10Fh */
	union io_pcie_config_com_5		PHY_CONFIG_COM_5;			/* +110 - 113h */
	union io_pcie_config_com_6		PHY_CONFIG_COM_6;			/* +114 - 117h */
	unsigned char					dmy_118_1D7[0x1D8-0x118];	/* +118 - 1D7h */
	union io_pcie_cfg_monitor_1		CFG_MONITOR_1;				/* +1D8 - 1DBh */
	union io_pcie_cfg_monitor_2		CFG_MONITOR_2;				/* +1DC - 1DFh */
	union io_pcie_cfg_monitor_3		CFG_MONITOR_3;				/* +1E0 - 1E3h */
	union io_pcie_cfg_monitor_4		CFG_MONITOR_4;				/* +1E4 - 1E7h */
	union io_pcie_cfg_monitor_5		CFG_MONITOR_5;				/* +1E8 - 1EBh */
	union io_pcie_debug_monitor_1	DEBUG_MONITOR_1;			/* +1EC - 1EFh */
	union io_pcie_debug_monitor_2	DEBUG_MONITOR_2;			/* +1F0 - 1F3h */
	union io_pcie_debug_monitor_3	DEBUG_MONITOR_3;			/* +1F4 - 1F7h */
	struct io_pcie_debug_monitor_4	DEBUG_MONITOR_4;			/* +1F8 - 1FFh */
	union io_pcie_msg_monitor_1		MSG_MONITOR_1;				/* +200 - 203h */
	unsigned char					dmy_204_207[0x208-0x204];	/* +204 - 207h */
	struct io_pcie_msg_monitor_3	MSG_MONITOR_3;				/* +208 - 20Fh */
	union io_pcie_link_monitor		LINK_MONITOR;				/* +210 - 213h */
	unsigned char					dmy_214_217[0x218-0x214];	/* +214 - 217h */
	union io_pcie_to_monitor_1		TO_MONITOR_1;				/* +218 - 21Bh */
	union io_pcie_to_monitor_2		TO_MONITOR_2;				/* +21C - 21Fh */
	union io_pcie_to_monitor_3		TO_MONITOR_3;				/* +220 - 223h */
	union io_pcie_to_monitor_4		TO_MONITOR_4;				/* +224 - 227h */
	union io_pcie_transmit_monitor	TRANSMIT_MONITOR;			/* +228 - 22Bh */
	union io_pcie_pipe_monitor		PIPE_MONITOR;				/* +22C - 22Fh */
	union io_pcie_phy_monitor		PHY_MONITOR;				/* +230 - 233h */
	unsigned char					dmy_234_237[0x238-0x234];	/* +234 - 237h */
	struct io_pcie_monitor_bar0_s	MONITOR_BAR0_S;				/* +238 - 23Fh */
	struct io_pcie_monitor_bar0_l	MONITOR_BAR0_L;				/* +240 - 247h */
	union io_pcie_monitor_bar1_s	MONITOR_BAR1_S;				/* +248 - 24Bh */
	union io_pcie_monitor_bar1_l	MONITOR_BAR1_L;				/* +24C - 249h */
	struct io_pcie_monitor_bar2_s	MONITOR_BAR2_S;				/* +250 - 257h */
	struct io_pcie_monitor_bar2_l	MONITOR_BAR2_L;				/* +258 - 25Fh */
	union io_pcie_monitor_bar3_s	MONITOR_BAR3_S;				/* +260 - 263h */
	union io_pcie_monitor_bar3_l	MONITOR_BAR3_L;				/* +264 - 267h */
	struct io_pcie_monitor_bar4_s	MONITOR_BAR4_S;				/* +268 - 26Fh */
	struct io_pcie_monitor_bar4_l	MONITOR_BAR4_L;				/* +270 - 277h */
	union io_pcie_monitor_bar5_s	MONITOR_BAR5_S;				/* +278 - 27Bh */
	union io_pcie_monitor_bar5_l	MONITOR_BAR5_L;				/* +27C - 27Fh */
	union io_pcie_monitor_erom_s	MONITOR_EROM_S;				/* +280 - 283h */
	union io_pcie_monitor_erom_l	MONITOR_EROM_L;				/* +284 - 287h */
};

extern volatile union io_pcie_core	IO_PCIE0_CORE;				/* 1915_(0000 - FFFFh) */
extern volatile union io_pcie_core	IO_PCIE1_CORE;				/* 1916_(0000 - FFFFh) */

extern volatile struct io_pcie		IO_PCIE0;					/* 1B11_1(000 - FFFh) */
extern volatile struct io_pcie		IO_PCIE1;					/* 1B11_2(000 - FFFh) */

