/**
 * @file		_jdsmxic.h
 * @brief		Definition JDSMXIC Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include	"__jdsmxic_tbl.h"

/*	structure of TMIRST		*/
union io_mxic_tmirst{
	unsigned long		word;
	struct {
		unsigned long	SR				:1;
		unsigned long					:31;
	}bit;
};

/*	structure of TCKEN		*/
union io_mxic_tcken{
	unsigned long		word;
	struct {
		unsigned long	TCKSEN			:1;
		unsigned long					:7;
		unsigned long	TCKMEN			:1;
		unsigned long					:23;
	}bit;
};

/*	structure of TCKENP		*/
union io_mxic_tckenp{
	unsigned long		word;
	struct {
		unsigned long	TCKENP_1		:4;
		unsigned long					:4;
		unsigned long	TCKENP_2		:4;
		unsigned long					:4;
		unsigned long	TCKENP_3		:4;
		unsigned long					:4;
		unsigned long	TCKENP_4		:4;
		unsigned long					:4;
	}bit;
};

/*	structure of TMIE		*/
union io_mxic_tmie{
	unsigned long		word;
	struct {
		unsigned long	TCE				:1;
		unsigned long	TME				:1;
		unsigned long	TDEWE			:1;
		unsigned long	TDERE			:1;
		unsigned long	TSAWE			:1;
		unsigned long					:27;
	}bit;
};

/*	structure of TMIF		*/
union io_mxic_tmif{
	unsigned long		word;
	struct {
		unsigned long	__TCF			:1;
		unsigned long	__TMF			:1;
		unsigned long	__TDEWF			:1;
		unsigned long	__TDERF			:1;
		unsigned long	__TSAWF			:1;
		unsigned long					:27;
	}bit;
};

/*	structure of TDESTW		*/
union io_mxic_tdestw{
	unsigned long		word[2];
	struct {
		unsigned long	TDEMNW			:7;
		unsigned long					:25;
		unsigned long	TDEADW			:32;
	}bit;
};

/*	structure of TDESTR		*/
union io_mxic_tdestr{
	unsigned long		word[2];
	struct {
		unsigned long	TDEMNR			:7;
		unsigned long					:25;
		unsigned long	TDEADR			:32;
	}bit;
};

/*	structure of TCFTRG		*/
union io_mxic_tcftrg{
	unsigned long		word;
	struct {
		unsigned long	TCFTRG			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of TCFMD		*/
union io_mxic_tcfmd{
	unsigned long		word[2];
	struct {
		unsigned long	TCFLG			:1;
		unsigned long					:7;
		unsigned long	TENW_1			:4;
		unsigned long	TENW_2			:4;
		unsigned long	TENW_3			:4;
		unsigned long	TENW_4			:4;
		unsigned long	TENR_1			:4;
		unsigned long	TENR_2			:4;
		unsigned long	TENR_3			:4;
		unsigned long	TENR_4			:4;
		unsigned long					:24;
	}bit;
};

/*	structure of TDECLR		*/
union io_mxic_tdeclr{
	unsigned long		word;
	struct {
		unsigned long	TDECLR			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of TRVSW		*/
union io_mxic_trvsw{
	unsigned long		word;
	struct {
		unsigned long	TRVSW			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of TAEN		*/
union io_mxic_taen{
	unsigned long		word;
	struct {
		unsigned long	TAEN			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of TACPTCW		*/
union io_mxic_tacptcw{
	unsigned long		word[4];
	struct {
		unsigned long	TACPTCW_1_0		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_1_1		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_1_2		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_1_3		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_2_0		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_2_1		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_2_2		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_2_3		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_3_0		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_3_1		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_3_2		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_3_3		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_4_0		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_4_1		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_4_2		:5;
		unsigned long					:3;
		unsigned long	TACPTCW_4_3		:5;
		unsigned long					:3;
	}bit;
};

/*	structure of TACPTCR		*/
union io_mxic_tacptcr{
	unsigned long		word[4];
	struct {
		unsigned long	TACPTCR_1_0		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_1_1		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_1_2		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_1_3		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_2_0		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_2_1		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_2_2		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_2_3		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_3_0		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_3_1		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_3_2		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_3_3		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_4_0		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_4_1		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_4_2		:5;
		unsigned long					:3;
		unsigned long	TACPTCR_4_3		:5;
		unsigned long					:3;
	}bit;
};

/*	structure of TSMTRG		*/
union io_mxic_tsmtrg{
	unsigned long		word;
	struct {
		unsigned long	TSMTRG			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of TREGION	*/
union io_mxic_tregion{
	unsigned long		word[2];
	struct {
		unsigned long	TSIZE			:32;
		unsigned long	TSTAD			:32;
	}bit;
};

/*	structure of TSASETW	*/
union io_mxic_tsasetw{
	unsigned long		word[10];
	struct {
		unsigned long	TSATRGW				:2;
		unsigned long						:30;
		unsigned long						:32;
		unsigned long	TSAMASW47_1_LO		:32;
		unsigned long	TSAMASW47_1_HI		:16;
		unsigned long						:16;
		unsigned long	TSAMASW127_64_LO	:32;
		unsigned long	TSAMASW127_64_HI	:32;
		unsigned long	TSASADW				:32;
		unsigned long	TSAEADW				:32;
		unsigned long	TSARW				:1;
		unsigned long						:7;
		unsigned long	TSAMNSTW			:7;
		unsigned long						:17;
		unsigned long	TSAADSTW			:32;
	}bit;
};

/*	structure of TMTRG		*/
union io_mxic_tmtrg{
	unsigned long		word;
	struct {
		unsigned long	TMTRG			:2;
		unsigned long					:30;
	}bit;
};

/*	structure of TMMD		*/
union io_mxic_tmmd{
	unsigned long		word;
	struct {
		unsigned long	TMON			:2;
		unsigned long					:2;
		unsigned long	TMOFF			:2;
		unsigned long					:2;
		unsigned long	TMUP			:1;
		unsigned long					:3;
		unsigned long	TMUPC			:3;
		unsigned long					:1;
		unsigned long	TMCLR			:1;
		unsigned long	TMCLRC			:1;
		unsigned long					:2;
		unsigned long	TMRSEL			:1;
		unsigned long					:3;
		unsigned long	THSEN			:1;
		unsigned long					:7;
	}bit;
};

/*	structure of TMONTRG	*/
union io_mxic_tmontrg{
	unsigned long		word;
	struct {
		unsigned long	TONTRG			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of TMUPTRG	*/
union io_mxic_tmuptrg{
	unsigned long		word;
	struct {
		unsigned long	TUPTRG			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of TMLIMSEL		*/
union io_mxic_tmlimsel{
	unsigned long		word;
	struct {
		unsigned long	TLIMSEL			:9;
		unsigned long					:3;
		unsigned long	TMALMS			:1;
		unsigned long					:19;
	}bit;
};

/*	structure of TMLIMIT		*/
union io_mxic_tmlimit{
	unsigned long		word;
	struct {
		unsigned long	TMLIMT			:32;
	}bit;
};

/*	structure of TMSEL			*/
union io_mxic_tmsel{
	unsigned long		word[2];
	struct {
		unsigned long	TMSEL_0			:9;
		unsigned long					:3;
		unsigned long	TMAS_0			:1;
		unsigned long					:3;
		unsigned long	TMSEL_1			:9;
		unsigned long					:3;
		unsigned long	TMAS_1			:1;
		unsigned long					:3;
		unsigned long	TMSEL_2			:9;
		unsigned long					:3;
		unsigned long	TMAS_2			:1;
		unsigned long					:3;
		unsigned long	TMSEL_3			:9;
		unsigned long					:3;
		unsigned long	TMAS_3			:1;
		unsigned long					:3;
	}bit;
};

/*	structure of TMCNT		*/
union io_mxic_tmcnt{
	unsigned long		word;
	struct {
		unsigned long	TMCNT			:32;
	}bit;
};

/*	structure of THSTW		*/
union io_mxic_thstw{
	unsigned long		word[4];
	struct {
		unsigned long	THSTW_0			:7;
		unsigned long					:1;
		unsigned long	THSTW_1			:7;
		unsigned long					:1;
		unsigned long	THSTW_2			:7;
		unsigned long					:1;
		unsigned long	THSTW_3			:7;
		unsigned long					:1;
		unsigned long	THSTW_4			:7;
		unsigned long					:1;
		unsigned long	THSTW_5			:7;
		unsigned long					:1;
		unsigned long	THSTW_6			:7;
		unsigned long					:1;
		unsigned long	THSTW_7			:7;
		unsigned long					:1;
		unsigned long	THSTW_8			:7;
		unsigned long					:1;
		unsigned long	THSTW_9			:7;
		unsigned long					:1;
		unsigned long	THSTW_10		:7;
		unsigned long					:1;
		unsigned long	THSTW_11		:7;
		unsigned long					:1;
		unsigned long	THSTW_12		:7;
		unsigned long					:1;
		unsigned long	THSTW_13		:7;
		unsigned long					:1;
		unsigned long	THSTW_14		:7;
		unsigned long					:1;
		unsigned long	THSTW_15		:7;
		unsigned long					:1;
	}bit;
};

/*	structure of THSTR		*/
union io_mxic_thstr{
	unsigned long		word[4];
	struct {
		unsigned long	THSTR_0			:7;
		unsigned long					:1;
		unsigned long	THSTR_1			:7;
		unsigned long					:1;
		unsigned long	THSTR_2			:7;
		unsigned long					:1;
		unsigned long	THSTR_3			:7;
		unsigned long					:1;
		unsigned long	THSTR_4			:7;
		unsigned long					:1;
		unsigned long	THSTR_5			:7;
		unsigned long					:1;
		unsigned long	THSTR_6			:7;
		unsigned long					:1;
		unsigned long	THSTR_7			:7;
		unsigned long					:1;
		unsigned long	THSTR_8			:7;
		unsigned long					:1;
		unsigned long	THSTR_9			:7;
		unsigned long					:1;
		unsigned long	THSTR_10		:7;
		unsigned long					:1;
		unsigned long	THSTR_11		:7;
		unsigned long					:1;
		unsigned long	THSTR_12		:7;
		unsigned long					:1;
		unsigned long	THSTR_13		:7;
		unsigned long					:1;
		unsigned long	THSTR_14		:7;
		unsigned long					:1;
		unsigned long	THSTR_15		:7;
		unsigned long					:1;
	}bit;
};

/*	structure of TMARLP_W		*/
union io_mxic_tmarlp_w{
	unsigned long		word;
	struct {
		unsigned long	MAS_PORT_W		:2;
		unsigned long					:2;
		unsigned long	MAS_LEVEL_W		:2;
		unsigned long					:26;
	}bit;
};

/*	structure of TMARLP_R		*/
union io_mxic_tmarlp_r{
	unsigned long		word;
	struct {
		unsigned long	MAS_PORT_R		:2;
		unsigned long					:2;
		unsigned long	MAS_LEVEL_R		:2;
		unsigned long					:26;
	}bit;
};

/*	structure of TRMCW		*/
union io_mxic_trmcw{
	unsigned long		word[2];
	struct {
		unsigned long	TRMW			:2;
		unsigned long					:14;
		unsigned long	TRMCVPW			:12;
		unsigned long					:4;
		unsigned long	TRMFTW			:5;
		unsigned long					:3;
		unsigned long	TRPW			:4;
		unsigned long					:4;
		unsigned long	TISULIMW		:6;
		unsigned long					:10;
	}bit;
};

/*	structure of TRMCR		*/
union io_mxic_trmcr{
	unsigned long		word[2];
	struct {
		unsigned long	TRMR			:2;
		unsigned long					:14;
		unsigned long	TRMCVPR			:12;
		unsigned long					:4;
		unsigned long	TRMFTR			:5;
		unsigned long					:3;
		unsigned long	TRPR			:4;
		unsigned long					:4;
		unsigned long	TISULIMR		:6;
		unsigned long					:10;
	}bit;
};

/*	structure of TLVLW		*/
union io_mxic_tlvlw{
	unsigned long		word[3];
	struct {
		unsigned long 	LMLW			:4;
		unsigned long 					:4;
		unsigned long 	LPL0W			:2;
		unsigned long 					:6;
		unsigned long 	LPCCL0W			:6;
		unsigned long 					:2;
		unsigned long 	LPCSL0W			:6;
		unsigned long 					:2;
		unsigned long 	LPL1W			:2;
		unsigned long 					:6;
		unsigned long 	LPCCL1W			:6;
		unsigned long 					:2;
		unsigned long 	LPCSL1W			:6;
		unsigned long 					:10;
		unsigned long	LPL2W			:2;
		unsigned long					:6;
		unsigned long	LPCCL2W			:6;
		unsigned long					:2;
		unsigned long	LPCSL2W			:6;
		unsigned long					:10;
	}bit;
};

/*	structure of TLVLR		*/
union io_mxic_tlvlr{
	unsigned long		word[3];
	struct {
		unsigned long	LMLR			:4;
		unsigned long					:4;
		unsigned long	LPL0R			:2;
		unsigned long					:6;
		unsigned long	LPCCL0R			:6;
		unsigned long					:2;
		unsigned long	LPCSL0R			:6;
		unsigned long					:2;
		unsigned long	LPL1R			:2;
		unsigned long					:6;
		unsigned long	LPCCL1R			:6;
		unsigned long					:2;
		unsigned long	LPCSL1R			:6;
		unsigned long					:10;
		unsigned long	LPL2R			:2;
		unsigned long					:6;
		unsigned long	LPCCL2R			:6;
		unsigned long					:2;
		unsigned long	LPCSL2R			:6;
		unsigned long					:10;
	}bit;
};

/*	structure of TMSTW		*/
union io_mxic_tmstw{
	unsigned long		word;
	struct {
		unsigned long	TMSTW_1			:4;
		unsigned long					:4;
		unsigned long	TMSTW_2			:4;
		unsigned long					:4;
		unsigned long	TMSTW_3			:4;
		unsigned long					:4;
		unsigned long	TMSTW_4			:4;
		unsigned long					:4;
	}bit;
};

/*	structure of TMSTR		*/
union io_mxic_tmstr{
	unsigned long		word;
	struct {
		unsigned long	TMSTR_1			:4;
		unsigned long					:4;
		unsigned long	TMSTR_2			:4;
		unsigned long					:4;
		unsigned long	TMSTR_3			:4;
		unsigned long					:4;
		unsigned long	TMSTR_4			:4;
		unsigned long					:4;
	}bit;
};

/*	structure of TSLSW		*/
union io_mxic_tslsw{
	unsigned long		word[4];
	struct {
		unsigned long	SLOTSW_0		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_1		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_2		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_3		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_4		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_5		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_6		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_7		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_8		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_9		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_10		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_11		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_12		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_13		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_14		:7;
		unsigned long					:1;
		unsigned long	SLOTSW_15		:7;
		unsigned long					:1;
	}bit;
};

/*	structure of TSLSR		*/
union io_mxic_tslsr{
	unsigned long		word[4];
	struct {
		unsigned long	SLOTSR_0		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_1		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_2		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_3		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_4		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_5		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_6		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_7		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_8		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_9		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_10		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_11		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_12		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_13		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_14		:7;
		unsigned long					:1;
		unsigned long	SLOTSR_15		:7;
		unsigned long					:1;
	}bit;
};

/*	structure of TGCKEN		*/
union io_mxic_tgcken{
	unsigned long		word;
	struct {
		unsigned long	TGCKSEN			:8;
		unsigned long					:24;
	}bit;
};

/*	structure of TGCFMD		*/
union io_mxic_tgcfmd{
	unsigned long		word;
	struct {
		unsigned long	TEGW_0			:1;
		unsigned long	TEGW_1			:1;
		unsigned long	TEGW_2			:1;
		unsigned long	TEGW_3			:1;
		unsigned long	TEGW_4			:1;
		unsigned long	TEGW_5			:1;
		unsigned long	TEGW_6			:1;
		unsigned long	TEGW_7			:1;
		unsigned long	TEGR_0			:1;
		unsigned long	TEGR_1			:1;
		unsigned long	TEGR_2			:1;
		unsigned long	TEGR_3			:1;
		unsigned long	TEGR_4			:1;
		unsigned long	TEGR_5			:1;
		unsigned long	TEGR_6			:1;
		unsigned long	TEGR_7			:1;
		unsigned long					:16;
	}bit;
};

/*	structure of TGACPTCW		*/
union io_mxic_tgacptcw{
	unsigned long		word[4];
	struct {
		unsigned long	TGACPTCW_0_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_0_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_1_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_1_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_2_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_2_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_3_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_3_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_4_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_4_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_5_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_5_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_6_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_6_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_7_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCW_7_1	:5;
		unsigned long					:3;
	}bit;
};

/*	structure of TGACPTCR		*/
union io_mxic_tgacptcr{
	unsigned long		word[4];
	struct {
		unsigned long	TGACPTCR_0_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_0_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_1_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_1_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_2_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_2_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_3_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_3_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_4_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_4_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_5_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_5_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_6_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_6_1	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_7_0	:5;
		unsigned long					:3;
		unsigned long	TGACPTCR_7_1	:5;
		unsigned long					:3;
	}bit;
};

/*	structure of TPORTMW	*/
union io_mxic_tportmw{
	unsigned long		word[2];
	struct {
		unsigned long	TPORTMW_127_64_LO	:32;
		unsigned long	TPORTMW_127_64_HI	:32;
	}bit;
};

/*	structure of TPORTMR	*/
union io_mxic_tportmr{
	unsigned long		word[2];
	struct {
		unsigned long	TPORTMR_127_64_LO	:32;
		unsigned long	TPORTMR_127_64_HI	:32;
	}bit;
};


/*	structure of TGSLSW		*/
union io_mxic_tgslsw{
	unsigned long		word[2];
	struct {
		unsigned long	GSLOTSW_0		:7;
		unsigned long					:1;
		unsigned long	GSLOTSW_1		:7;
		unsigned long					:1;
		unsigned long	GSLOTSW_2		:7;
		unsigned long					:1;
		unsigned long	GSLOTSW_3		:7;
		unsigned long					:1;
		unsigned long	GSLOTSW_4		:7;
		unsigned long					:1;
		unsigned long	GSLOTSW_5		:7;
		unsigned long					:1;
		unsigned long	GSLOTSW_6		:7;
		unsigned long					:1;
		unsigned long	GSLOTSW_7		:7;
		unsigned long					:1;
	}bit;
};

/*	structure of TGSLSR		*/
union io_mxic_tgslsr{
	unsigned long		word[2];
	struct {
		unsigned long	GSLOTSR_0		:7;
		unsigned long					:1;
		unsigned long	GSLOTSR_1		:7;
		unsigned long					:1;
		unsigned long	GSLOTSR_2		:7;
		unsigned long					:1;
		unsigned long	GSLOTSR_3		:7;
		unsigned long					:1;
		unsigned long	GSLOTSR_4		:7;
		unsigned long					:1;
		unsigned long	GSLOTSR_5		:7;
		unsigned long					:1;
		unsigned long	GSLOTSR_6		:7;
		unsigned long					:1;
		unsigned long	GSLOTSR_7		:7;
		unsigned long					:1;
	}bit;
};

/* Define i/o mapping */
struct io_jdsmxic{
	/* ALL 28xx_(0000 - 07FFh) */
	union  io_mxic_tmirst		TMIRST;							/* (0000 - 0003h) */
	union  io_mxic_tcken		TCKEN;							/* (0004 - 0007h) */
	union  io_mxic_tckenp		TCKENP;							/* (0008 - 000Bh) */
	union  io_mxic_tmie			TMIE;							/* (000C - 000Fh) */
	union  io_mxic_tmif			TMIF;							/* (0010 - 0013h) */

	unsigned char dmy_0014_0017[0x0018-0x0014];					/* (0014 - 0017h) */

	union  io_mxic_tdestw		TDESTW;							/* (0018 - 001Fh) */

	unsigned char dmy_0020_0027[0x0028-0x0020];					/* (0020 - 0027h) */

	union  io_mxic_tdestr		TDESTR;							/* (0028 - 002Fh) */

	unsigned char dmy_0030_0033[0x0034-0x0030];					/* (0030 - 0033h) */

	union  io_mxic_tcftrg		TCFTRG;							/* (0034 - 0037h) */
	union  io_mxic_tcfmd		TCFMD;							/* (0038 - 003Fh) */
	union  io_mxic_tdeclr		TDECLR;							/* (0040 - 0043h) */
	union  io_mxic_trvsw		TRVSW;							/* (0044 - 0047h) */

	unsigned char dmy_0048_005F[0x0060-0x0048];					/* (0048 - 005Fh) */

	union  io_mxic_taen			TAEN;							/* (0060 - 0063h) */

	unsigned char dmy_0064_00FF[0x0100-0x0064];					/* (0064 - 00FFh) */

	union  io_mxic_tacptcw		TACPTCW;						/* (0100 - 010Fh) */
	union  io_mxic_tacptcr		TACPTCR;						/* (0110 - 011Fh) */

	unsigned char dmy_0120_013F[0x0140-0x0120];					/* (0120 - 013Fh) */

	union  io_mxic_tsmtrg		TSMTRG;							/* (0140 - 0143h) */

	unsigned char dmy_0144_015F[0x0160-0x0144];					/* (0144 - 015Fh) */

	union  io_mxic_tregion		TREGION[4];						/* (0160 - 017Fh) */

	unsigned char dmy_0180_03FF[0x0400-0x0180];					/* (0180 - 03FFh) */

	union  io_mxic_tsasetw		TSASETW[4];						/* (0400 - 049Fh) */

	unsigned char dmy_04A0_07FF[0x0800-0x04A0];					/* (04A0 - 07FFh) */

	/* MONITOR 28xx_(0800 - 0BFFh) */
	union  io_mxic_tmtrg		TMTRG;							/* (0800 - 0803h) */
	union  io_mxic_tmmd			TMMD;							/* (0804 - 0807h) */
	union  io_mxic_tmontrg		TMONTRG;						/* (0808 - 080Bh) */
	union  io_mxic_tmuptrg		TMUPTRG;						/* (080C - 080Fh) */
	union  io_mxic_tmlimsel		TMLIMSEL;						/* (0810 - 0813h) */
	union  io_mxic_tmlimit		TMLIMIT;						/* (0814 - 0817h) */
	union  io_mxic_tmsel		TMSEL;							/* (0818 - 081Fh) */
	union  io_mxic_tmcnt		TMCNT[4];						/* (0820 - 082Fh) */

	unsigned char dmy_0830_08FF[0x0900-0x0830];					/* (0830 - 08FFh) */

	union  io_mxic_thstw		THSTW[4][4];					/* (0900 - 09FFh) */

	union  io_mxic_thstr		THSTR[4][4];					/* (0A00 - 0AFFh) */

	unsigned char dmy_0B00_0BFF[0x0C00-0x0B00];					/* (0B00 - 0BFFh) */

	unsigned char dmy_0C00_0FFF[0x1000-0x0C00];					/* (0C00 - 0FFFh) */

	/* MASTER 28xx_(1000 - 1FFFh) */
	union  io_mxic_tmarlp_w		TMARLP_W_1[63];					/* (1000 - 10FBh) */

	unsigned char dmy_10FC_10FF[0x1100-0x10FC];					/* (10FC - 10FFh) */

	union  io_mxic_tmarlp_w		TMARLP_W_2[63];					/* (1100 - 11FBh) */

	unsigned char dmy_11FC_11FF[0x1200-0x11FC];					/* (11FC - 11FFh) */

	union  io_mxic_tmarlp_w		TMARLP_W_3[63];					/* (1200 - 12FBh) */

	unsigned char dmy_12FC_12FF[0x1300-0x12FC];					/* (12FC - 12FFh) */

	union  io_mxic_tmarlp_w		TMARLP_W_4[63];					/* (1300 - 13FBh) */

	unsigned char dmy_13FC_13FF[0x1400-0x13FC];					/* (13FC - 13FFh) */

	union  io_mxic_tmarlp_r		TMARLP_R_1[63];					/* (1400 - 14FBh) */

	unsigned char dmy_14FC_14FF[0x1500-0x14FC];					/* (14FC - 14FFh) */

	union  io_mxic_tmarlp_r		TMARLP_R_2[63];					/* (1500 - 15FBh) */

	unsigned char dmy_15FC_15FF[0x1600-0x15FC];					/* (15FC - 15FFh) */

	union  io_mxic_tmarlp_r		TMARLP_R_3[63];					/* (1600 - 16FBh) */

	unsigned char dmy_16FC_16FF[0x1700-0x16FC];					/* (16FC - 16FFh) */

	union  io_mxic_tmarlp_r		TMARLP_R_4[63];					/* (1700 - 17FBh) */

	unsigned char dmy_17FC_1FFF[0x2000-0x17FC];					/* (17FC - 1FFFh) */

	/* TRMC 28xx_(2000 - 23FFh) */
	union  io_mxic_trmcw		TRMCW[63];						/* (2000 - 21F7h) */

	unsigned char dmy_21F8_21FF[0x2200-0x21F8];					/* (21F8 - 21FFh) */

	union  io_mxic_trmcr		TRMCR[63];						/* (2200 - 23F7h) */

	unsigned char dmy_23F8_23FF[0x2400-0x23F8];					/* (23F8 - 23FFh) */

	/* LEVEL 28xx_(2400 - 25FFh) */
	union  io_mxic_tlvlw		TLVLW1[4];						/* (2400 - 242Fh) */

	unsigned char dmy_2430_243F[0x2440-0x2430];					/* (2430 - 243Fh) */

	union  io_mxic_tlvlw		TLVLW2[4];						/* (2440 - 246Fh) */

	unsigned char dmy_2470_247F[0x2480-0x2470];					/* (2470 - 247Fh) */

	union  io_mxic_tlvlw		TLVLW3[4];						/* (2480 - 24AFh) */

	unsigned char dmy_24B0_24BF[0x24C0-0x24B0];					/* (24B0 - 24BFh) */

	union  io_mxic_tlvlw		TLVLW4[4];						/* (24C0 - 24EFh) */

	unsigned char dmy_24F0_24FF[0x2500-0x24F0];					/* (24F0 - 24FFh) */

	union  io_mxic_tlvlr		TLVLR1[4];						/* (2500 - 252Fh) */

	unsigned char dmy_2530_253F[0x2540-0x2530];					/* (2530 - 253Fh) */

	union  io_mxic_tlvlr		TLVLR2[4];						/* (2540 - 256Fh) */

	unsigned char dmy_2570_257F[0x2580-0x2570];					/* (2570 - 257Fh) */

	union  io_mxic_tlvlr		TLVLR3[4];						/* (2580 - 25AFh) */

	unsigned char dmy_25B0_25BF[0x25C0-0x25B0];					/* (25B0 - 25BFh) */

	union  io_mxic_tlvlr		TLVLR4[4];						/* (25C0 - 25EFh) */

	unsigned char dmy_25F0_25FF[0x2600-0x25F0];					/* (25F0 - 25FFh) */

	unsigned char dmy_2600_26FF[0x2700-0x2600];					/* (2600 - 26FFh) */

	/* MASTER_STATUS 28xx_(2700 - 277Fh) */
	union  io_mxic_tmstw		TMSTW[16];						/* (2700 - 273Fh) */
	union  io_mxic_tmstr		TMSTR[16];						/* (2740 - 277Fh) */

	unsigned char dmy_2780_27FF[0x2800-0x2780];					/* (2780 - 27FFh) */

	/* SLOT_STATUS 28xx_(2800 - 2FFFh) */
	union  io_mxic_tslsw		TSLSW_1[4][4];					/* (2800 - 28FFh) */
	union  io_mxic_tslsw		TSLSW_2[4][4];					/* (2900 - 29FFh) */
	union  io_mxic_tslsw		TSLSW_3[4][4];					/* (2A00 - 2AFFh) */
	union  io_mxic_tslsw		TSLSW_4[4][4];					/* (2B00 - 2BFFh) */
	union  io_mxic_tslsr		TSLSR_1[4][4];					/* (2C00 - 2CFFh) */
	union  io_mxic_tslsr		TSLSR_2[4][4];					/* (2D00 - 2DFFh) */
	union  io_mxic_tslsr		TSLSR_3[4][4];					/* (2E00 - 2EFFh) */
	union  io_mxic_tslsr		TSLSR_4[4][4];					/* (2F00 - 2FFFh) */

	/* GROUP_ARB 28xx_(3000 - 3FFFh) */
	union  io_mxic_tgcken		TGCKEN;							/* (3000 - 3003h) */
	union  io_mxic_tgcfmd		TGCFMD;							/* (3004 - 3007h) */

	unsigned char dmy_3008_301F[0x3020-0x3008];					/* (3008 - 301Fh) */

	union  io_mxic_tgacptcw		TGACPTCW;						/* (3020 - 302Fh) */
	union  io_mxic_tgacptcr		TGACPTCR;						/* (3030 - 303Fh) */
	union  io_mxic_tportmw		TPORTMW;						/* (3040 - 3047h) */
	union  io_mxic_tportmr		TPORTMR;						/* (3048 - 304Fh) */

	unsigned char dmy_3050_33FF[0x3400-0x3050];					/* (3050 - 33FFh) */

	union  io_mxic_trmcw		TRMCW_GR[64];					/* (3400 - 35FFh) */
	union  io_mxic_trmcr		TRMCR_GR[64];					/* (3600 - 37FFh) */
	union  io_mxic_tmstw		TMSTW_GR[16];					/* (3800 - 383Fh) */
	union  io_mxic_tmstr		TMSTR_GR[16];					/* (3840 - 387Fh) */
	union  io_mxic_tgslsw		TGSLSW[8];						/* (3880 - 38BFh) */
	union  io_mxic_tgslsr		TGSLSR[8];						/* (38C0 - 38FFh) */

	unsigned char dmy_3900_3FFF[0x4000-0x3900];					/* (3900 - 3FFFh) */
};

extern volatile struct io_jdsmxic	IO_MXIC0;
extern volatile struct io_jdsmxic	IO_MXIC1;
extern volatile struct io_jdsmxic	IO_MXIC2;
extern volatile struct io_jdsmxic	IO_MXIC3;
extern volatile struct io_jdsmxic	IO_MXIC4;
extern volatile struct io_jdsmxic	IO_MXIC5;
extern volatile struct io_jdsmxic	IO_MXIC6;

