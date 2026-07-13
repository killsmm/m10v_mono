/**
 * @file		_jdsme.h
 * @brief		Definition JDSME Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

/*	structure of SRESET			*/
union io_me_sreset{
	unsigned long		word;
	struct {
		unsigned long	CMDR			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of INTMODE		*/
union io_me_intmode{
	unsigned long		word;
	struct {
		unsigned long	PPAVALID		:1;
		unsigned long	PPBVALID		:1;
		unsigned long	PPCVALID		:1;
		unsigned long					:1;
		unsigned long	INTMODE			:1;
		unsigned long					:27;
	}bit;
};

/*	structure of INTSTATE		*/
union io_me_intstate{
	unsigned long		word;
	struct {
		unsigned long	PPAINT			:1;
		unsigned long	PPBINT			:1;
		unsigned long	PPCINT			:1;
		unsigned long					:29;
	}bit;
};

/*	structure of INTCNTR		*/
union io_me_intcntr{
	unsigned long		word;
	struct {
		unsigned long	INTERVAL		:10;
		unsigned long					:22;
	}bit;
};

/*	structure of CR				*/
union io_me_cr{
	unsigned long		word;
	struct {
		unsigned long	CMDR			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of PTCC			*/
union io_me_ptcc{
	unsigned long		word;
	struct {
		unsigned long	CKA				:1;
		unsigned long					:31;
	}bit;
};

/*	structure of PACC			*/
union io_me_pacc{
	unsigned long		word;
	struct {
		unsigned long	PABT_CKA		:1;
		unsigned long	CKM				:1;
		unsigned long					:30;
	}bit;
};

/*	structure of IRQSC			*/
union io_me_irqsc{
	unsigned long		word;
	struct {
		unsigned long	__IRQ0			:1;
		unsigned long	__IRQ1			:1;
		unsigned long	__IRQ2			:1;
		unsigned long	__IRQ3			:1;
		unsigned long	__IRQ4			:1;
		unsigned long	__IRQ5			:1;
		unsigned long	__IRQ6			:1;
		unsigned long	__IRQ7			:1;
		unsigned long	__IRQ8			:1;
		unsigned long					:23;
	}bit;
};

/*	structure of IRQE			*/
union io_me_irqe{
	unsigned long		word;
	struct {
		unsigned long	IRQE0			:1;
		unsigned long	IRQE1			:1;
		unsigned long	IRQE2			:1;
		unsigned long	IRQE3			:1;
		unsigned long	IRQE4			:1;
		unsigned long	IRQE5			:1;
		unsigned long	IRQE6			:1;
		unsigned long	IRQE7			:1;
		unsigned long	IRQE8			:1;
		unsigned long					:23;
	}bit;
};

/*	structure of MIRQS			*/
union io_me_mirqs{
	unsigned long		word;
	struct {
		unsigned long	IRQMS0			:1;
		unsigned long	IRQMS1			:1;
		unsigned long	IRQMS2			:1;
		unsigned long	IRQMS3			:1;
		unsigned long	IRQMS4			:1;
		unsigned long	IRQMS5			:1;
		unsigned long	IRQMS6			:1;
		unsigned long	IRQMS7			:1;
		unsigned long	IRQMS8			:1;
		unsigned long					:23;
	}bit;
};

/*	structure of PABT_PARAM_MPR		*/
union io_me_pabt_param_mpr{
	unsigned long		word;
	struct {
		unsigned long	INI_MPR			:4;
		unsigned long	AP_MPR			:2;
		unsigned long					:26;
	}bit;
};

/*	structure of PABT_PARAM_MVR		*/
union io_me_pabt_param_mvr{
	unsigned long		word;
	struct {
		unsigned long	INI_MVR			:4;
		unsigned long	AP_MVR			:2;
		unsigned long					:26;
	}bit;
};

/*	structure of PABT_PARAM_MVW		*/
union io_me_pabt_param_mvw{
	unsigned long		word;
	struct {
		unsigned long	INI_MVW			:4;
		unsigned long	AP_MVW			:2;
		unsigned long					:26;
	}bit;
};

/*	structure of PABT_PARAM_PPW		*/
union io_me_pabt_param_ppw{
	unsigned long		word;
	struct {
		unsigned long	INI_PPW			:4;
		unsigned long	AP_PPW			:2;
		unsigned long					:26;
	}bit;
};

/*	structure of PABT_PARAM_PPR		*/
union io_me_pabt_param_ppr{
	unsigned long		word;
	struct {
		unsigned long	INI_PPR			:4;
		unsigned long	AP_PPR			:2;
		unsigned long					:26;
	}bit;
};

/*	structure of PABT_PARAM_PSW		*/
union io_me_pabt_param_psw{
	unsigned long		word;
	struct {
		unsigned long	INI_PSW			:4;
		unsigned long	AP_PSW			:2;
		unsigned long					:26;
	}bit;
};

/*	structure of SRST				*/
union io_me_srst{
	unsigned long		word;
	struct {
		unsigned long	SRST			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of ACTIVE				*/
union io_me_active{
	unsigned long		word;
	struct {
		unsigned long	CLKACT			:1;
		unsigned long	CKEN_MODE0		:1;
		unsigned long	CKEN_MODE1		:1;
		unsigned long					:29;
	}bit;
};

/*	structure of STATE				*/
union io_me_state{
	unsigned long		word;
	struct {
		unsigned long					:1;
		unsigned long	ME1_STATE		:3;
		unsigned long					:28;
	}bit;
};

/*	structure of MBNUMH/V				*/
union io_me_mbnum{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:9;
		unsigned long					:23;
		unsigned long	V				:8;
		unsigned long					:24;
	}bit;
};

/*	structure of INPUTMODE			*/
union io_me_inputmode{
	unsigned long		word;
	struct {
		unsigned long	INPUTMODE		:1;
		unsigned long					:6;
		unsigned long	REF_TXB			:1;
		unsigned long					:24;
	}bit;
};

/*	structure of OUTLIMIT			*/
union io_me_outlimit{
	unsigned long		word;
	struct {
		unsigned long	LIMIT			:2;
		unsigned long					:30;
	}bit;
};

/*	structure of QMBADRH/V			*/
union io_me_qmbadr{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:8;
		unsigned long					:24;
		unsigned long	V				:7;
		unsigned long					:25;
	}bit;
};

/*	structure of M1BANKNUM			*/
union io_me_m1banknum{
	unsigned long		word;
	struct {
		unsigned long	BANKNUM			:3;
		unsigned long					:29;
	}bit;
};

/*	structure of M1ORGxMODE			*/
union io_me_m1orgmode{
	unsigned long		word;
	struct {
		unsigned long	DIRECTION		:1;
		unsigned long	PARITY			:1;
		unsigned long	READMODE		:1;
		unsigned long	SAD0MODE		:3;
		unsigned long	SAD1MODE		:3;
		unsigned long	VECMODE			:1;
		unsigned long	VECSEL			:2;
		unsigned long					:2;
		unsigned long	PICTYPE			:2;
		unsigned long					:16;
	}bit;
};

/*	structure of M1START			*/
union io_me_m1start{
	unsigned long		word;
	struct {
		unsigned long					:1;
		unsigned long	M1PIC_STR		:1;
		unsigned long					:30;
	}bit;
};

/*	structure of M1CORRECT			*/
union io_me_m1corrct{
	unsigned long		word[ 5 ];
	struct {
		unsigned long	FKY				:3;
		unsigned long	BKY				:3;
		unsigned long					:2;
		unsigned long	FKX				:2;
		unsigned long	BKX				:2;
		unsigned long	MODE			:4;
		unsigned long					:16;
		unsigned long	FH				:6;
		unsigned long					:26;
		unsigned long	FV				:5;
		unsigned long					:27;
		unsigned long	BH				:6;
		unsigned long					:26;
		unsigned long	BV				:5;
		unsigned long					:27;
	}bit;
};

/*	structure of M1MCWGHTP/B	*/
union io_me_m1mc{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	P				:13;
		unsigned long					:19;
		unsigned long	B				:13;
		unsigned long					:19;
	}bit;
};

/*	structure of M1SPLT					*/
union io_me_m1splt{
	unsigned long		word[ 3 ];
	struct {
		unsigned long	MODE			:2;
		unsigned long					:30;
		unsigned long	TH				:8;
		unsigned long					:24;
		unsigned long	TV				:8;
		unsigned long					:24;
	}bit;
};

/*	structure of M1SKIP		*/
union io_me_m1skip{
	unsigned long		word;
	struct {
		unsigned long	SKIP			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of M1SADSUM_U/L		*/
union io_me_m1sadsum{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	U				:12;
		unsigned long					:20;
		unsigned long	L				:12;
		unsigned long					:20;
	}bit;
};

/*	structure of M1ADJUST		*/
union io_me_m1adjust{
	unsigned long		word;
	struct {
		unsigned long	CYCLE			:15;
		unsigned long	ON				:1;
		unsigned long					:16;
	}bit;
};

/*	structure of M1PAUSE		*/
union io_me_m1pause{
	unsigned long		word;
	struct {
		unsigned long	LINE			:7;
		unsigned long					:8;
		unsigned long	ON				:1;
		unsigned long					:16;
	}bit;
};

/*	structure of M1LIMITMVx		*/
union io_me_m1limitmv{
	unsigned long		word;
	struct {
		unsigned long	P				:4;
		unsigned long					:4;
		unsigned long	M				:4;
		unsigned long					:20;
	}bit;
};

/*	structure of M1LIMITMHx		*/
union io_me_m1limitmh{
	unsigned long		word;
	struct {
		unsigned long	P				:5;
		unsigned long					:3;
		unsigned long	M				:5;
		unsigned long					:19;
	}bit;
};

/*	structure of M1STORGxAVRSAD			*/
union io_me_m1storgavrsad{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:12;
		unsigned long					:20;
		unsigned long	L				:16;
		unsigned long					:16;
	}bit;
};

/*	structure of M1STMINxAVRSAD			*/
union io_me_m1stminavrsad{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:12;
		unsigned long					:20;
		unsigned long	L				:16;
		unsigned long					:16;
	}bit;
};

/*	structure of M1ST_ORGx_VxVx			*/
union io_me_m1storgv{
	unsigned long		word[ 4 ];
	struct {
		unsigned long	VXVX_H			:12;
		unsigned long					:20;
		unsigned long	VXVX_L			:16;
		unsigned long					:16;
		unsigned long	VYVY_H			:12;
		unsigned long					:20;
		unsigned long	VYVY_L			:16;
		unsigned long					:16;
	}bit;
};

/*	structure of PMBUF_CTL		*/
union io_me_pmbufctl{
	unsigned long		word;
	struct {
		unsigned long	CMDRST			:1;
		unsigned long					:7;
		unsigned long	THRU			:1;
		unsigned long					:23;
	}bit;
};

/*	structure of COREMODE		*/
union io_me_coremode{
	unsigned long		word;
	struct {
		unsigned long	LINEINTV		:2;
		unsigned long					:6;
		unsigned long	INITPOSY		:2;
		unsigned long					:22;
	}bit;
};

/*	structure of CNCL		*/
union io_me_cncl{
	unsigned long		word[ 3 ];
	struct {
		unsigned long	MODE			:2;
		unsigned long					:30;
		unsigned long	THH				:8;
		unsigned long					:24;
		unsigned long	THL				:8;
		unsigned long					:24;
	}bit;
};

/*	structure of VOUT_FMT		*/
union io_me_voutfmt{
	unsigned long		word;
	struct {
		unsigned long	HSIZE			:12;
		unsigned long					:20;
	}bit;
};

/*	structure of ORGAVRx		*/
union io_me_orgavr{
	unsigned long		word;
	struct {
		unsigned long	ORGAVR			:8;
		unsigned long					:24;
	}bit;
};

/*	structure of CLKENB1		*/
union io_me_clkenb1{
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	M6_PFM_EN		:1;
		unsigned long					:27;
	}bit;
};

/*	structure of CLKENB4		*/
union io_me_clkenb4{
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	M6_P4M_EN		:1;
		unsigned long					:27;
	}bit;
};

/*	structure of CLKACT1		*/
union io_me_clkact1{
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	M6_PFM_ACT		:1;
		unsigned long					:27;
	}bit;
};

/*	structure of CLKACT4		*/
union io_me_clkact4{
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	M6_P4M_ACT		:1;
		unsigned long					:27;
	}bit;
};

/*	structure of PF_SIZE		*/
union io_me_pfsize{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:13;
		unsigned long					:19;
		unsigned long	V				:12;
		unsigned long					:20;
	}bit;
};

/*	structure of WRITE_STATE		*/
union io_me_writestate{
	unsigned long		word;
	struct {
		unsigned long	PP_A			:1;
		unsigned long	PP_B			:1;
		unsigned long	PP_C			:1;
		unsigned long					:29;
	}bit;
};

/*	structure of ABT_TYPE		*/
union io_me_abttype{
	unsigned long		word;
	struct {
		unsigned long	ABT_TYPE		:1;
		unsigned long					:31;
	}bit;
};

/*	structure of BANK_TBL_PVEC		*/
union io_me_banktblpvec{
	unsigned long		word;
	struct {
		unsigned long	FWD_PVEC		:8;
		unsigned long					:24;
	}bit;
};

/*	structure of BANK_TBL_8PELREF	*/
union io_me_banktbl8pelref{
	unsigned long		word;
	struct {
		unsigned long	TOP				:8;
		unsigned long					:24;
	}bit;
};

/*	structure of BANK_TBL_8PEL	*/
union io_me_banktbl8pel{
	unsigned long		word;
	struct {
		unsigned long	FWD_8PEL		:8;
		unsigned long					:24;
	}bit;
};

/*	structure of PF1_ON	*/
union io_me_pf1on{
	unsigned long		word;
	struct {
		unsigned long	PF1_ON			:1;
		unsigned long	PFBUF_EN		:1;
		unsigned long					:30;
	}bit;
};

/*	structure of PF1_INISIZE	*/
union io_me_pf1inisize{
	unsigned long		word;
	struct {
		unsigned long	SIZE_X			:8;
		unsigned long	SIZE_Y			:6;
		unsigned long					:18;
	}bit;
};

/*	structure of PF1_TBSEL		*/
union io_me_pf1tbsel{
	unsigned long		word;
	struct {
		unsigned long	TBSEL			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of PF1_PMENUM		*/
union io_me_pf1pmenum{
	unsigned long		word;
	struct {
		unsigned long	PMENUM			:2;
		unsigned long					:30;
	}bit;
};

/*	structure of PF1_START		*/
union io_me_pfstart{
	unsigned long		word;
	struct {
		unsigned long	STR				:1;
		unsigned long					:31;
	}bit;
};

/*	structure of PF1_CLEAR		*/
union io_me_clear{
	unsigned long		word;
	struct {
		unsigned long	CLR				:1;
		unsigned long					:31;
	}bit;
};

/*	structure of PF1_PB0/2_STBKNO	*/
union io_me_pf1pbstbkno{
	unsigned long		word;
	struct {
		unsigned long	PF1_PB0_STBKNO		:14;
		unsigned long						:1;
		unsigned long	PF1_PB0_STBKNOEN	:1;
		unsigned long						:16;
	}bit;
};

/*	structure of PF1_PFB0/2		*/
union io_me_pf1pfb{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	POSX			:8;
		unsigned long	POSY			:8;
		unsigned long					:16;
		unsigned long	SIZEY			:4;
		unsigned long					:28;
	}bit;
};

/*	structure of PREVSZ_3DNRME	*/
union io_me_prevsz3dnrme{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:11;
		unsigned long					:21;
		unsigned long	V				:10;
		unsigned long					:22;
	}bit;
};

/*	structure of MRGN_3DNRME	*/
union io_me_mtgn3dnrme{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:4;
		unsigned long					:28;
		unsigned long	V				:4;
		unsigned long					:28;
	}bit;
};

/*	structure of SHIFTX/Y_3DNRME	*/
union io_me_shift3dnrme{
	unsigned long		word[ 2 ];
	struct {
		long			X				:7;
		unsigned long					:25;
		long			Y				:7;
		unsigned long					:25;
	}bit;
};

/*	structure of CMD_RST		*/
union io_me_cmdrst{
	unsigned long		word;
	struct {
		unsigned long					:15;
		unsigned long	CMD_RST			:1;
		unsigned long					:16;
	}bit;
};

/*	structure of M1_GBO_H/L		*/
union io_me_m1gbo{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:16;
		unsigned long					:16;
		unsigned long	L				:16;
		unsigned long					:16;
	}bit;
};

/*	structure of M1_GBS_H/L		*/
union io_me_m1gbs{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:6;
		unsigned long					:26;
		unsigned long	L				:16;
		unsigned long					:16;
	}bit;
};

/*	structure of M1_GLSS		*/
union io_me_m1glss{
	unsigned long		word;
	struct {
		unsigned long	GLSS			:11;
		unsigned long					:21;
	}bit;
};

/*	structure of PREV_GBO_H/L		*/
union io_me_prevgbo{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:16;
		unsigned long					:16;
		unsigned long	L				:16;
		unsigned long					:16;
	}bit;
};

/*	structure of PREV_GBS_H/L		*/
union io_me_prevgbs{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:6;
		unsigned long					:26;
		unsigned long	L				:16;
		unsigned long					:16;
	}bit;
};

/*	structure of PREV_GLSS		*/
union io_me_prevglss{
	unsigned long		word;
	struct {
		unsigned long	GLSS			:11;
		unsigned long					:21;
	}bit;
};

/*	structure of PVEC_GBO_H/L		*/
union io_me_pvecgbo{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:16;
		unsigned long					:16;
		unsigned long	L				:16;
		unsigned long					:16;
	}bit;
};

/*	structure of PVEC_GBS_H/L		*/
union io_me_pvecgbs{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	H				:14;
		unsigned long					:18;
		unsigned long	L				:16;
		unsigned long					:16;
	}bit;
};

/*	structure of WBUF0			*/
union io_me_wbuf{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	RADD			:7;
		unsigned long					:25;
		unsigned long	WADD			:7;
		unsigned long					:25;
	}bit;
};

/*	structure of RBUF0/1			*/
union io_me_rbuf{
	unsigned long		word[ 2 ];
	struct {
		unsigned long	RADD			:8;
		unsigned long					:24;
		unsigned long	WADD			:8;
		unsigned long					:24;
	}bit;
};

/*	structure of AXI_CNTL_SET		*/
union io_me_axicntlset{
	unsigned long		word;
	struct {
		unsigned long	ARCACHE			:4;
		unsigned long	ARPROT			:3;
		unsigned long					:1;
		unsigned long	AWCACHE			:4;
		unsigned long	AWPROT			:3;
		unsigned long					:1;
		unsigned long					:16;
	}bit;
};

/*	structure of AXI_ERR_CLR		*/
union io_me_axierrclr{
	unsigned long		word;
	struct {
		unsigned long	CUR_CLR			:1;
		unsigned long					:3;
		unsigned long	PRE_CLR			:1;
		unsigned long					:3;
		unsigned long	VEC_CLR			:1;
		unsigned long					:23;
	}bit;
};

/*	structure of AXI_ERR_STATUS		*/
union io_me_axierrstatus{
	unsigned long		word;
	struct {
		unsigned long	RESP_CIR		:2;
		unsigned long	CUR_STS			:1;
		unsigned long					:1;
		unsigned long	RESP_PRE		:2;
		unsigned long	PRE_STS			:1;
		unsigned long					:1;
		unsigned long	RESP_VEC		:2;
		unsigned long	VEC_STS			:1;
		unsigned long					:21;
	}bit;
};

/*	structure of CUR_RDMA_QU_STAT		*/
union io_me_currdmaqustat{
	unsigned long		word;
	struct {
		unsigned long	CUR_PRQBACC		:2;
		unsigned long					:2;
		unsigned long	CUR_RRQBACC		:2;
		unsigned long					:2;
		unsigned long	CUR_CRQBACC		:2;
		unsigned long					:22;
	}bit;
};

/*	structure of PRE_RDMA_QU_STAT		*/
union io_me_prerdmaqustat{
	unsigned long		word;
	struct {
		unsigned long	PRE_PRQBACC		:2;
		unsigned long					:2;
		unsigned long	PRE_RRQBACC		:2;
		unsigned long					:2;
		unsigned long	PRE_CRQBACC		:2;
		unsigned long					:22;
	}bit;
};

/*	structure of VEC_WDMA_QU_STAT		*/
union io_me_vecwdmaqustat{
	unsigned long		word;
	struct {
		unsigned long	PWQBACC			:2;
		unsigned long					:2;
		unsigned long	RWQBACC			:2;
		unsigned long					:2;
		unsigned long	CWQBACC			:2;
		unsigned long					:22;
	}bit;
};

struct io_me_pme{
	// PTCNT.
	union  io_me_cr						CR;					/* (2000 - 2003h) */
	union  io_me_ptcc					PTCC;				/* (2004 - 2007h) */
	union  io_me_pacc					PACC;				/* (2008 - 200Bh) */
	unsigned char dmy_200C_203F[0x2040-0x200C];				/* (200C - 203Fh) */
	union  io_me_irqsc					IRQSC;				/* (2040 - 2043h) */
	union  io_me_irqe					IRQE;				/* (2044 - 2047h) */
	union  io_me_mirqs					MIRQS;				/* (2048 - 204Bh) */
	unsigned char dmy_204C_207F[0x2080-0x204C];				/* (204C - 207Fh) */
	union  io_me_pabt_param_mpr			PABT_PARAM_MPR;		/* (2080 - 2083h) */
	union  io_me_pabt_param_mvr			PABT_PARAM_MVR;		/* (2084 - 2087h) */
	union  io_me_pabt_param_mvw			PABT_PARAM_MVW;		/* (2088 - 208Bh) */
	union  io_me_pabt_param_ppw			PABT_PARAM_PPW;		/* (208C - 208Fh) */
	union  io_me_pabt_param_ppr			PABT_PARAM_PPR;		/* (2090 - 2093h) */
	union  io_me_pabt_param_psw			PABT_PARAM_PSW;		/* (2094 - 2097h) */
	unsigned char dmy_2098_20FF[0x2100-0x2098];				/* (2098 - 20FFh) */
	unsigned char dmy_2100_27FF[0x2800-0x2100];				/* (2100 - 27FFh) */

	// PME.
	union  io_me_srst					SRST;				/* (2800 - 2803h) */
	union  io_me_active					ACTIVE;				/* (2804 - 2807h) */
	union  io_me_state					STATE;				/* (2808 - 280Bh) */
	union  io_me_mbnum					MBNUM;				/* (280C - 280Fh) */
															/* (2810 - 2813h) */
	union  io_me_inputmode				INPUTMODE;			/* (2814 - 2817h) */
	unsigned char dmy_2818_2837[0x2838-0x2818];				/* (2818 - 2837h) */
	union  io_me_outlimit				OUTLIMIT;			/* (2838 - 283Bh) */
	unsigned char dmy_283C_283F[0x2840-0x283C];				/* (283C - 283Fh) */
	union  io_me_qmbadr					QMBADR;				/* (2840 - 2843h) */
															/* (2844 - 2847h) */
	unsigned char dmy_2848_284F[0x2850-0x2848];				/* (2848 - 284Fh) */
	union  io_me_m1banknum				M1BANKNUM;			/* (2850 - 2853h) */
	union  io_me_m1orgmode				M1ORGMODE[ 4 ];		/* (2854 - 2857h) */
															/* (2858 - 285Bh) */
															/* (285C - 285Fh) */
															/* (2860 - 2863h) */
	unsigned char dmy_2864_28FF[0x2900-0x2864];				/* (2864 - 28FFh) */
	union  io_me_m1start				M1START;			/* (2900 - 2903h) */
	union  io_me_m1corrct				M1CORRECT;			/* (2904 - 2907h) */
															/* (2908 - 290Bh) */
															/* (290C - 290Fh) */
															/* (2910 - 2913h) */
															/* (2914 - 2917h) */
	union  io_me_m1mc					M1MC;				/* (2918 - 291Bh) */
															/* (291C - 291Fh) */
	union  io_me_m1splt					M1SPLT;				/* (2920 - 2923h) */
															/* (2924 - 2927h) */
															/* (2928 - 292Bh) */
	unsigned char dmy_292C_2937[0x2938-0x292C];				/* (292C - 2937h) */
	union  io_me_m1skip					M1SKIP;				/* (2938 - 293Bh) */
	unsigned char dmy_293C_295B[0x295C-0x293C];				/* (293C - 295Bh) */
	union  io_me_m1sadsum				M1SADSUM;			/* (295C - 295Fh) */
															/* (2960 - 2963h) */
	unsigned char dmy_2964_2977[0x2978-0x2964];				/* (2964 - 2977h) */
	union  io_me_m1adjust				M1ADJUST;			/* (2978 - 297Bh) */
	union  io_me_m1pause				M1PAUSE;			/* (297C - 297Fh) */
	union  io_me_m1limitmv				M1LIMITMV[ 4 ];		/* (2980 - 2983h) */
															/* (2984 - 2987h) */
															/* (2988 - 298Bh) */
															/* (298C - 298Fh) */
	union  io_me_m1limitmh				M1LIMITMH[ 4 ];		/* (2990 - 2993h) */
															/* (2994 - 2997h) */
															/* (2998 - 299Bh) */
															/* (299C - 299Fh) */
	unsigned char dmy_29A0_29BF[0x29C0-0x29A0];				/* (29A0 - 29BFh) */
	union  io_me_m1storgavrsad			M1STORGAVRSAD[ 4 ];	/* (29C0 - 29C3h) */
															/* (29C4 - 29C7h) */
															/* (29C8 - 29CBh) */
															/* (29CC - 29CFh) */
															/* (29D0 - 29D3h) */
															/* (29D4 - 29D7h) */
															/* (29D8 - 29DBh) */
															/* (29DC - 29DFh) */
	union  io_me_m1stminavrsad			M1STMINAVRSAD[ 4 ];	/* (29E0 - 29E3h) */
															/* (29E4 - 29E7h) */
															/* (29E8 - 29EBh) */
															/* (29EC - 29EFh) */
															/* (29F0 - 29F3h) */
															/* (29F4 - 29F7h) */
															/* (29F8 - 29FBh) */
															/* (29FC - 29FFh) */
	union  io_me_m1storgv				M1ST_ORG_VXVX[ 4 ];	/* (2A00 - 2A03h) */
															/* (2A04 - 2A07h) */
															/* (2A08 - 2A0Bh) */
															/* (2A0C - 2A0Fh) */
															/* (2A10 - 2A13h) */
															/* (2A14 - 2A17h) */
															/* (2A18 - 2A1Bh) */
															/* (2A1C - 2A1Fh) */
															/* (2A20 - 2A23h) */
															/* (2A24 - 2A27h) */
															/* (2A28 - 2A2Bh) */
															/* (2A2C - 2A2Fh) */
															/* (2A30 - 2A33h) */
															/* (2A34 - 2A37h) */
															/* (2A38 - 2A3Bh) */
															/* (2A3C - 2A3Fh) */
	union  io_me_pmbufctl				PMBUF_CTL;			/* (2A40 - 2A43h) */
	unsigned char dmy_2A44_2A4F[0x2A50-0x2A44];				/* (2A44 - 2A4Fh) */
	union  io_me_coremode				COREMODE;			/* (2A50 - 2A53h) */
	unsigned char dmy_2A54_2A5F[0x2A60-0x2A54];				/* (2A54 - 2A5Fh) */
	union  io_me_cncl					CNCL;				/* (2A60 - 2A63h) */
															/* (2A64 - 2A67h) */
															/* (2A68 - 2A6Bh) */
	unsigned char dmy_2A6C_2A6F[0x2A70-0x2A6C];				/* (2A6C - 2A6Fh) */
	union  io_me_voutfmt				VOUT_FMT;			/* (2A70 - 2A73h) */
	unsigned char dmy_2A74_2A7F[0x2A80-0x2A74];				/* (2A74 - 2A7Fh) */
	union  io_me_orgavr					ORGAVR[ 4 ];		/* (2A80 - 2A83h) */
															/* (2A84 - 2A87h) */
															/* (2A88 - 2A8Bh) */
															/* (2A8C - 2A8Fh) */
	unsigned char dmy_2A90_2AFF[0x2B00-0x2A90];				/* (2A90 - 2AFFh) */
	unsigned char dmy_2B00_3FFF[0x4000-0x2B00];				/* (2B00 - 3FFFh) */
};

/* Define i/o mapping */
struct io_jdsme{
	// APBBRG.
	union  io_me_sreset					SRESET;				/* (0000 - 0003h) */
	union  io_me_intmode				INTMODE;			/* (0004 - 0007h) */
	union  io_me_intstate				INTSTATE;			/* (0008 - 000Bh) */
	union  io_me_intcntr				INTCNTR;			/* (000C - 000Fh) */
	unsigned char dmy_0010_1FFF[0x2000-0x0010];				/* (0010 - 1FFFh) */

	// PPA PME 0/1/2.
	struct	io_me_pme					PME[ 3 ];			/* (2000 - 3FFFh) */
															/* (4000 - 5FFFh) */
															/* (6000 - 7FFFh) */
	// RMCIF.
	union  io_me_clkenb1				CLKENB1;			/* (8000 - 8003h) */
	union  io_me_clkenb4				CLKENB4;			/* (8004 - 8007h) */
	union  io_me_clkact1				CLKACT1;			/* (8008 - 800Bh) */
	union  io_me_clkact4				CLKACT4;			/* (800C - 800Fh) */
	unsigned char dmy_8010_801F[0x8020-0x8010];				/* (8010 - 801Fh) */
	union  io_me_pfsize					PF_SIZE;			/* (8020 - 8023h) */
															/* (8024 - 8027h) */
	unsigned char dmy_8028_802B[0x802C-0x8028];				/* (8028 - 802Bh) */
	union  io_me_writestate				WRITE_STATE;		/* (802C - 802Fh) */
	union  io_me_abttype				ABT_TYPE;			/* (8030 - 8033h) */
	unsigned char dmy_8034_80FF[0x8100-0x8034];				/* (8034 - 80FFh) */
	union  io_me_banktblpvec			BANK_TBL_PVEC;		/* (8100 - 8103h) */
	unsigned char dmy_8104_81FF[0x8200-0x8104];				/* (8104 - 81FFh) */
	union  io_me_banktbl8pelref			BANK_TBL_8PELREF;	/* (8200 - 8203h) */
	union  io_me_banktbl8pel			BANK_TBL_8PEL;		/* (8204 - 8207h) */
	unsigned char dmy_8208_82FF[0x8300-0x8208];				/* (8208 - 82FFh) */
	union  io_me_pf1on					PF1_ON;				/* (8300 - 8303h) */
	union  io_me_pf1inisize				PF1_INISIZE;		/* (8304 - 8307h) */
	union  io_me_pf1tbsel				PF1_TBSEL;			/* (8308 - 830Bh) */
	union  io_me_pf1pmenum				PF1_PMENUM;			/* (830C - 830Fh) */
	union  io_me_pfstart				PF1_START;			/* (8310 - 8313h) */
	union  io_me_clear					PF1_CLEAR;			/* (8314 - 8317h) */
	unsigned char dmy_8318_831B[0x831C-0x8318];				/* (8318 - 831Bh) */
	union  io_me_pf1pbstbkno			PF1_PB0_STBKNO;		/* (831C - 831Fh) */
	union  io_me_pf1pbstbkno			PF1_PB2_STBKNO;		/* (8320 - 8323h) */
	union  io_me_pf1pfb					PF1_PFB0;			/* (8324 - 8327h) */
															/* (8328 - 832Bh) */
	union  io_me_pf1pfb					PF1_PFB2;			/* (832C - 832Fh) */
															/* (8330 - 8333h) */
	unsigned char dmy_8334_8FFF[0x8400-0x8334];				/* (8334 - 83FFh) */
	union  io_me_prevsz3dnrme			PREVSZ_3DNRME;		/* (8400 - 8403h) */
															/* (8404 - 8407h) */
	union  io_me_mtgn3dnrme				MRGN_3DNRME;		/* (8408 - 840Bh) */
															/* (840C - 840Fh) */
	union  io_me_shift3dnrme			SHIFT_3DNRME;		/* (8410 - 8413h) */
															/* (8414 - 8417h) */
	unsigned char dmy_8418_84FF[0x8500-0x8418];				/* (8418 - 84FFh) */
	union  io_me_cmdrst					CMD_RST;			/* (8500 - 8503h) */
	unsigned char dmy_8504_85FF[0x8600-0x8504];				/* (8504 - 85FFh) */
	union  io_me_m1gbo					M1_GBO;				/* (8600 - 8603h) */
															/* (8604 - 8607h) */
	union  io_me_m1gbs					M1_GBS;				/* (8608 - 860Bh) */
															/* (860C - 860Fh) */
	union  io_me_m1glss					M1_GLSS;			/* (8610 - 8613h) */
	unsigned char dmy_8614_86FF[0x8700-0x8614];				/* (8614 - 86FFh) */
	union  io_me_prevgbo				PREV_GBO;			/* (8700 - 8703h) */
	;														/* (8704 - 8707h) */
	union  io_me_prevgbs				PREV_GBS;			/* (8708 - 870Bh) */
															/* (870C - 870Fh) */
	union  io_me_prevglss				PREV_GLSS;			/* (8710 - 8713h) */
	unsigned char dmy_8714_87FF[0x8800-0x8714];				/* (8714 - 87FFh) */
	union  io_me_pvecgbo				PVEC_GBO;			/* (8800 - 8803h) */
															/* (8804 - 8807h) */
	union  io_me_pvecgbs				PVEC_GBS;			/* (8808 - 880Bh) */
															/* (880C - 880Fh) */
	unsigned char dmy_8810_89FF[0x8A00-0x8810];				/* (8810 - 89FFh) */
	union  io_me_wbuf					WBUF;				/* (8A00 - 8A03h) */
															/* (8A04 - 8A07h) */
	union  io_me_rbuf					RBUF[ 2 ];			/* (8A08 - 8A0Bh) */
															/* (8A0C - 8A0Fh) */
															/* (8A10 - 8A13h) */
															/* (8A14 - 8A17h) */
	unsigned char dmy_8A18_8BFF[0x8C00-0x8A18];				/* (8A18 - 8BFFh) */
	union  io_me_axicntlset				AXI_CNTL_SET;		/* (8C00 - 8C03h) */
	unsigned char dmy_8C04_8C0F[0x8C10-0x8C04];				/* (8C04 - 8C0Fh) */
	union  io_me_axierrclr				AXI_ERR_CLR;		/* (8C10 - 8C13h) */
	union  io_me_axierrstatus			AXI_ERR_STATUS;		/* (8C14 - 8C17h) */
	unsigned char dmy_8C18_8CFF[0x8D00-0x8C18];				/* (8C18 - 8CFFh) */
	union  io_me_currdmaqustat			CUR_RDMA_QU_STAT;	/* (8D00 - 8D03h) */
	union  io_me_prerdmaqustat			PRE_RDMA_QU_STAT;	/* (8D04 - 8D07h) */
	union  io_me_vecwdmaqustat			VEC_WDMA_QU_STAT;	/* (8D08 - 8D0Bh) */
	unsigned char dmy_8D0C_8FFF[0x9000-0x8D0C];				/* (8D0C - 8FFFh) */
	unsigned char dmy_9000_9FFF[0xA000-0x9000];				/* (9000 - 9FFFh) */

	unsigned char dmy_A000_FFFF[0x10000-0xA000];			/* (A000 - FFFFh) */
};

extern volatile struct io_jdsme		IO_ME;

