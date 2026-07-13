/**
 * @file		_sdramc.h
 * @brief		Definition SDRAMC Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*	structure of EMBNUM		*/
union io_sdramc_embnum{
	unsigned long		word;
	struct {
		unsigned long	RNUM			:4;
		unsigned long					:4;
		unsigned long	WNUM			:4;
		unsigned long					:4;
		unsigned long	RSIZE			:3;
		unsigned long					:5;
		unsigned long	WSIZE			:3;
		unsigned long					:5;
	}bit;
};

/*	structure of REMAPADDR		*/
union io_sdramc_remapaddr{
	unsigned long		word;
	struct {
		unsigned long					:26;
		long			REMAPADDR		:6;
	}bit;
};

/*	structure of CHSEL			*/
union io_sdramc_chsel{
	unsigned long		word;
	struct {
		unsigned long					:25;
		unsigned long	CH				:1;
		unsigned long					:6;
	}bit;
};

/*	structure of RATECONT		*/
union io_sdramc_ratecount{
	unsigned long		word;
	struct {
		unsigned long	CYCNUM			:12;
		unsigned long					:4;
		unsigned long	LATNUM			:12;
		unsigned long					:4;
	}bit;
};

/*	structure of MASTEROPTION		*/
union io_sdramc_masterop{
	unsigned long		word;
	struct {
		unsigned long	ENABLE_CAT_R	:1;
		unsigned long					:3;
		unsigned long	ENABLE_CAT_B	:1;
		unsigned long					:27;
	}bit;
};

/*	structure of PRIORIT_CAT		*/
union io_sdramc_priorit{
	unsigned long		word;
	struct {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long	PRIORIT_CAT_B	:5;
		unsigned long					:27;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	PRIORIT_CAT_B	:6;
		unsigned long					:26;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	}bit;
};

/*	structure of RATE_CHANGE		*/
union io_sdramc_ratechg{
	unsigned long		word;
	struct {
		unsigned long	RATE_CHG		:1;
		unsigned long					:31;
	}bit;
};

/*	structure of UMCCMDCTLA		*/
union io_sdramc_umccmda{
	unsigned long		word;
	struct {
		unsigned long	TRAS			:7;
		unsigned long					:1;
		unsigned long	TRP				:7;
		unsigned long					:1;
		unsigned long	TRCD			:7;
		unsigned long					:1;
		unsigned long	TRRD			:7;
		unsigned long					:1;
	}bit;
};

/*	structure of UMCCMDCTLB		*/
union io_sdramc_umccmdb{
	unsigned long		word;
	struct {
		unsigned long	TR2P			:7;
		unsigned long					:1;
		unsigned long	TW2P			:7;
		unsigned long					:1;
		unsigned long	TFAW			:7;
		unsigned long					:1;
		unsigned long	TRC				:7;
		unsigned long					:1;
	}bit;
};

/*	structure of UMCCMDCTLC		*/
union io_sdramc_umccmdc{
	unsigned long		word;
	struct {
		unsigned long	TCCD			:7;
		unsigned long					:1;
		unsigned long	TR2W			:7;
		unsigned long					:1;
		unsigned long	TW2R			:7;
		unsigned long					:1;
		unsigned long					:7;
		unsigned long					:1;
	}bit;
};

/*	structure of UMCCMDCTLD		*/
union io_sdramc_umccmdd{
	unsigned long		word;
	struct {
		unsigned long	TDRWR			:7;
		unsigned long					:1;
		unsigned long	TDRRW			:7;
		unsigned long					:1;
		unsigned long	TDRWW			:7;
		unsigned long					:1;
		unsigned long	TDRRR			:7;
		unsigned long					:1;
	}bit;
};

/*	structure of UMCINITCTLA		*/
union io_sdramc_umcinita{
	unsigned long		word;
	struct {
		unsigned long	TI2CKE			:12;
		unsigned long	TCKE2RST		:20;
	}bit;
};

/*	structure of UMCINITCTLB		*/
union io_sdramc_umcinitb{
	unsigned long		word;
	struct {
		unsigned long	TRST2ZQC		:12;
		unsigned long					:4;
		unsigned long	TRST2DAI		:16;
	}bit;
};

/*	structure of UMCINITCTLC		*/
union io_sdramc_umcinitc{
	unsigned long		word;
	struct {
		unsigned long	TZQINIT			:12;
		unsigned long					:20;
	}bit;
};

/*	structure of UMCDRMMRA		*/
union io_sdramc_umcdrm{
	unsigned long		word;
	struct {
		unsigned long	MR1				:8;
		unsigned long	MR2				:8;
		unsigned long	MR3				:8;
		unsigned long	MR10			:8;
	}bit;
};

/*	structure of UMCINITSET		*/
union io_sdramc_umcinitset{
	unsigned long		word;
	struct {
		unsigned long	SINITEN			:1;
		unsigned long	SINITMD			:1;
		unsigned long					:6;
		unsigned long	MINITEN			:1;
		unsigned long					:23;
	}bit;
};

/*	structure of UMCINITSTAT		*/
union io_sdramc_umcinitsta{
	unsigned long		word;
	struct {
		unsigned long	SINITST			:1;
		unsigned long					:7;
		unsigned long	MINITST			:1;
		unsigned long					:7;
		unsigned long					:16;
	}bit;
};

/*	structure of UMCCMDCTLE		*/
union io_sdramc_umccmde{
	unsigned long		word;
	struct {
		unsigned long	TREFI			:16;
		unsigned long	TRFC			:12;
		unsigned long					:4;
	}bit;
};

/*	structure of UMCCMDCTLF		*/
union io_sdramc_umccmdf{
	unsigned long		word;
	struct {
		unsigned long					:8;
		unsigned long	TXSR			:12;
		unsigned long					:4;
		unsigned long	TCKESR			:8;
	}bit;
};

/*	structure of UMCCMDCTLG		*/
union io_sdramc_umccmdg{
	unsigned long		word;
	struct {
		unsigned long	TCPDXD			:4;
		unsigned long					:4;
		unsigned long	TCPDED			:4;
		unsigned long					:4;
		unsigned long	TXP				:4;
		unsigned long					:4;
		unsigned long	TCKE			:4;
		unsigned long					:4;
	}bit;
};

/*	structure of UMCCMDCTLH		*/
union io_sdramc_umccmdh{
	unsigned long		word;
	struct {
		unsigned long	TZQINT			:32;
	}bit;
};

/*	structure of UMCCMDCTLI		*/
union io_sdramc_umccmdi{
	unsigned long		word;
	struct {
		unsigned long	TZQCS			:8;
		unsigned long					:8;
		unsigned long	TZQCL			:12;
		unsigned long					:4;
	}bit;
};

/*	structure of UMCCMDCTLJ		*/
union io_sdramc_umccmdj{
	unsigned long		word;
	struct {
		unsigned long	TMRR			:4;
		unsigned long	TMRW			:4;
		unsigned long	TMR2PD			:8;
		unsigned long	TPD2MR			:8;
		unsigned long					:8;
	}bit;
};

/*	structure of UMCSPCSETA		*/
union io_sdramc_umcspca{
	unsigned long		word;
	struct {
		unsigned long	AREFEN			:1;
		unsigned long					:7;
		unsigned long	SRINEN			:1;
		unsigned long	SROUTEN			:1;
		unsigned long					:14;
		unsigned long	ZQCEN			:1;
		unsigned long					:7;
	}bit;
};

/*	structure of UMCSPCSETB		*/
union io_sdramc_umcspcb{
	unsigned long		word;
	struct {
		unsigned long	AREFMD			:2;
		unsigned long					:22;
		unsigned long	ZQCMD			:2;
		unsigned long					:6;
	}bit;
};

/*	structure of UMCSPCSTAT		*/
union io_sdramc_umcspcsta{
	unsigned long		word;
	struct {
		unsigned long	SLFRST			:1;
		unsigned long					:7;
		unsigned long	PWDNST			:1;
		unsigned long					:7;
		unsigned long					:16;
	}bit;
};

/*	structure of UMCMEMCONF0A/UMCMEMCONF1A	*/
union io_sdramc_umcmema{
	unsigned long		word;
	struct {
		unsigned long	VALID			:1;
		unsigned long					:7;
		unsigned long	SIZE			:4;
		unsigned long					:14;
		unsigned long	BASEADDR		:6;
	}bit;
};

/*	structure of UMCMEMCONF0B/UMCMEMCONF1B	*/
union io_sdramc_umcmemb{
	unsigned long		word;
	struct {
		unsigned long	COL				:2;
		unsigned long					:2;
		unsigned long	ROW				:3;
		unsigned long					:1;
		unsigned long	BANK			:2;
		unsigned long					:2;
		unsigned long	DEV				:2;
		unsigned long					:2;
		unsigned long					:16;
	}bit;
};

/*	structure of UMCMEMCONFCH	*/
union io_sdramc_umcmemch{
	unsigned long		word;
	struct {
		unsigned long	TYPE			:4;
		unsigned long					:4;
		unsigned long	SIZE			:4;
		unsigned long	BL				:3;
		unsigned long					:1;
		unsigned long	WIDTH			:3;
		unsigned long					:7;
		unsigned long	BASEADDR		:6;
	}bit;
};

/*	structure of UMCMEMMAPSET	*/
union io_sdramc_umcmemset{
	unsigned long		word;
	struct {
		unsigned long	BNK_TYP			:4;
		unsigned long					:28;
	}bit;
};

/*	structure of UMCACCCHKA/UMCACCCHKB/UMCOODCHK	*/
union io_sdramc_umcacchk{
	unsigned long		word;
	struct {
		unsigned long	CHKEN			:1;
		unsigned long	PRCT			:1;
		unsigned long	BUSERR			:1;
		unsigned long	INT				:1;
		unsigned long					:28;
	}bit;
};

/*	structure of UMCACCARSAA/UMCACCARSAB	*/
union io_sdramc_umcaccrsa{
	unsigned long		word;
	struct {
		unsigned long	STADDR			:20;
		unsigned long					:12;
	}bit;
};

/*	structure of UMCACCAREAA/UMCACCAREAB	*/
union io_sdramc_umcaccrea{
	unsigned long		word;
	struct {
		unsigned long	ENADDR			:20;
		unsigned long					:12;
	}bit;
};

/*	structure of UMCWACCMSTA/UMCWACCMSTB	*/
union io_sdramc_umcwaccst{
	unsigned long		word;
	struct {
		unsigned long	WMST			:32;
	}bit;
};

/*	structure of UMCRACCMSTA/UMCRACCMSTB	*/
union io_sdramc_umcraccst{
	unsigned long		word;
	struct {
		unsigned long	RMST			:32;
	}bit;
};

/*	structure of UMCMRACS		*/
union io_sdramc_umcmracs{
	unsigned long		word;
	struct {
		unsigned long	MRMA			:8;
		unsigned long	RESERVED0		:16;
		unsigned long	MRMODE			:4;
		unsigned long	MRRANK			:2;
		unsigned long	RESERVED1		:2;
	}bit;
};

/*	structure of UMCMRACSST		*/
union io_sdramc_umcmracsst{
	unsigned long		word;
	struct {
		unsigned long	MRST			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of UMCMRWDT		*/
union io_sdramc_umcmrwdt{
	unsigned long		word;
	struct {
		unsigned long	WDATA			:32;
	}bit;
};

/*	structure of UMCMRRDT		*/
union io_sdramc_umcmrrdt{
	unsigned long		word;
	struct {
		unsigned long	RDATA			:32;
	}bit;
};

/*	structure of UMCRDATACTL_D0		*/
union io_sdramc_umcrdata0{
	unsigned long		word;
	struct {
		unsigned long	RDYLTY_D0		:6;
		unsigned long					:2;
		unsigned long	RENLTY_D0		:5;
		unsigned long					:19;
	}bit;
};

/*	structure of UMCRDATACTL_D1		*/
union io_sdramc_umcrdata1{
	unsigned long		word;
	struct {
		unsigned long	RDYLTY_D1		:6;
		unsigned long					:2;
		unsigned long	RENLTY_D1		:5;
		unsigned long					:19;
	}bit;
};

/*	structure of UMCWDATACTL_D0		*/
union io_sdramc_umcwdata0{
	unsigned long		word;
	struct {
		unsigned long	TENLTY_D0		:5;
		unsigned long					:3;
		unsigned long	TDTLTY_D0		:4;
		unsigned long					:20;
	}bit;
};

/*	structure of UMCWDATACTL_D1		*/
union io_sdramc_umcwdata1{
	unsigned long		word;
	struct {
		unsigned long	TENLTY_D1		:5;
		unsigned long					:3;
		unsigned long	TDTLTY_D1		:4;
		unsigned long					:20;
	}bit;
};

/*	structure of UMCDATASET		*/
union io_sdramc_umcdataset{
	unsigned long		word;
	struct {
		unsigned long	CENLTY			:4;
		unsigned long					:28;
	}bit;
};

/*	structure of UMCWDATASWP		*/
union io_sdramc_umcwdatswp{
	unsigned long		word;
	struct {
		unsigned long	WDATASWP		:1;
		unsigned long					:31;
	}bit;
};

/*	structure of UMCINTCAUSEM0		*/
union io_sdramc_umcintc{
	unsigned long		word;
	struct {
		unsigned long	INTCAUSE		:30;
		unsigned long	RESERVED		:2;
	}bit;
};

/*	structure of UMCINTMASKM0		*/
union io_sdramc_umcintmsk{
	unsigned long		word;
	struct {
		unsigned long	INTMASK			:30;
		unsigned long	RESERVED		:2;
	}bit;
};

/*	structure of ACCCHKFLG		*/
union io_sdramc_accchkflg{
	unsigned long		word;
	struct {
		unsigned long					:8;
		unsigned long	ACCCHKFLGA		:1;
		unsigned long	ACCCHKFLGB		:1;
		unsigned long					:22;
	}bit;
};

/*	structure of ACCCHKFLGCLR	*/
union io_sdramc_accchkflgclr{
	unsigned long		word;
	struct {
		unsigned long					:8;
		unsigned long	ACCCHKFLGCLRA	:1;
		unsigned long	ACCCHKFLGCLRB	:1;
		unsigned long					:22;
	}bit;
};

/*	structure of ACCCHKLOGA		*/
union io_sdramc_accchkloga{
	unsigned long		word;
	struct {
		unsigned long	PORTID			:6;
		unsigned long					:26;
	}bit;
};

/*	structure of ACCCHKLOGB		*/
union io_sdramc_accchklogb{
	unsigned long		word;
	struct {
		unsigned long	DRMADR			:29;
		unsigned long					:3;
	}bit;
};

/*	structure of UMCPHYCTLA		*/
union io_sdramc_umcphya{
	unsigned long		word;
	struct {
		unsigned long	RSTX			:1;
		unsigned long					:3;
		unsigned long	PZQRSTX			:1;
		unsigned long					:3;
		unsigned long	RSTX_CACK		:1;
		unsigned long	RSTX_PZQ		:1;
		unsigned long					:2;
		unsigned long	RSTX_DQDQS		:4;
		unsigned long	SOFTRSTX_CACK	:1;
		unsigned long					:3;
		unsigned long	SOFTRSTX_DQDQS	:4;
		unsigned long	DLLRSTX_CACK	:1;
		unsigned long					:3;
		unsigned long	DLLRSTX_DQDQS	:4;
	}bit;
};

/*	structure of UMCPHYCTLB		*/
union io_sdramc_umcphyb{
	unsigned long		word;
	struct {
		unsigned long	DLL_INIT_UPD_CACK	:1;
		unsigned long						:3;
		unsigned long	DLL_INIT_UPD_DQDQS	:4;
		unsigned long	PZQ_INIT_UPD		:1;
		unsigned long						:7;
		unsigned long	PZQ_UPD_WIDTH		:8;
		unsigned long						:8;
	}bit;
};

/*	structure of UMCPHYCTLC		*/
union io_sdramc_umcphyc{
	unsigned long		word;
	struct {
		unsigned long	ITR_REQ				:1;
		unsigned long						:3;
		unsigned long	ITR_MODE			:4;
		unsigned long	OTR_RESP			:1;
		unsigned long						:23;
	}bit;
};

/*	structure of UMCDFICTLA		*/
union io_sdramc_umcdfictla{
	unsigned long		word;
	struct {
		unsigned long	LPREQ_CACK			:1;
		unsigned long	LPREQ_DQDQS			:1;
		unsigned long						:29;
		unsigned long	LPCTLMD				:1;
	}bit;
};

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN --
/*	structure of UMCDFICTLB		*/
union io_sdramc_umcdfictlb{
	unsigned long		word;
	struct {
		unsigned long	DRAM_CLK_DISABLE	:1;
		unsigned long						:30;
		unsigned long	DRAMCLKMD			:1;
	}bit;
};

// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
/*	structure of UMCPFMCTL/UMCMIDPFMCTL		*/
union io_sdramc_umcpfmctl{
	unsigned long		word;
	struct {
		unsigned long	PFM_ON				:1;
		unsigned long	PFM_RST				:1;
		unsigned long	PFM_RST_AFTER_READ	:1;
		unsigned long	PFM_MON_R_OFF		:1;
		unsigned long	PFM_MON_W_OFF		:1;
		unsigned long						:3;
		unsigned long	PFM_MON_CMD			:1;
		unsigned long						:23;
	}bit;
};

/*	structure of UMCPFMST/UMCMIDPFMST		*/
union io_sdramc_umcpfmst{
	unsigned long		word;
	struct {
		unsigned long	VLDDATA				:32;
	}bit;
};

/*	structure of CLKENBIFA		*/
union io_sdramc_clkena{
	unsigned long		word;
	struct {
		unsigned long	CLKWMX10		:1;
		unsigned long	CLKWMX11		:1;
		unsigned long	CLKWMX12		:1;
		unsigned long	CLKWMX13		:1;
		unsigned long	CLKWMX20		:1;
		unsigned long	CLKWMX21		:1;
		unsigned long	CLKWMX22		:1;
		unsigned long	CLKWMX23		:1;
		unsigned long	CLKWMX30		:1;
		unsigned long	CLKWMX31		:1;
		unsigned long	CLKWMX32		:1;
		unsigned long	CLKWMX33		:1;
		unsigned long	CLKWMX40		:1;
		unsigned long	CLKWMX50		:1;
		unsigned long	CLKWMX51		:1;
		unsigned long	CLKWMX00		:1;
		unsigned long	CLKWMX01		:1;
		unsigned long	CLKWMX60		:1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long					:14;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long					:6;
		unsigned long	CLKWVCOREW		:1;
		unsigned long	CLKWVCORER0		:1;
		unsigned long	CLKWVCORER1		:1;
		unsigned long					:3;
		unsigned long	CLKWVCPU		:1;
		unsigned long					:1;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	}bit;
};

/*	structure of CLKENBIFB		*/
union io_sdramc_clkenb{
	unsigned long		word;
	struct {
		unsigned long	CLKRMX10		:1;
		unsigned long	CLKRMX11		:1;
		unsigned long	CLKRMX12		:1;
		unsigned long	CLKRMX13		:1;
		unsigned long	CLKRMX20		:1;
		unsigned long	CLKRMX21		:1;
		unsigned long	CLKRMX22		:1;
		unsigned long	CLKRMX23		:1;
		unsigned long	CLKRMX30		:1;
		unsigned long	CLKRMX31		:1;
		unsigned long	CLKRMX32		:1;
		unsigned long	CLKRMX33		:1;
		unsigned long	CLKRMX40		:1;
		unsigned long	CLKRMX50		:1;
		unsigned long	CLKRMX51		:1;
		unsigned long	CLKRMX00		:1;
		unsigned long	CLKRMX01		:1;
		unsigned long	CLKRMX60		:1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long					:14;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long					:6;
		unsigned long	CLKRVCOREW		:1;
		unsigned long	CLKRVCORER0		:1;
		unsigned long	CLKRVCORER1		:1;
		unsigned long					:3;
		unsigned long	CLKRVCPU		:1;
		unsigned long					:1;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	}bit;
};

/*	structure of CLKENBIFC		*/
union io_sdramc_clkenc{
	unsigned long		word;
	struct {
		unsigned long	CLKDSP			:1;
		unsigned long					:7;
		unsigned long	CLKBIF			:1;
		unsigned long					:9;
		unsigned long	CLKRB0			:1;
		unsigned long	CLKRB1			:1;
		unsigned long	CLKRB2			:1;
		unsigned long	CLKDS0			:1;
		unsigned long	CLKDS1			:1;
		unsigned long	CLKSDM			:1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long	CLKVE0			:1;
		unsigned long	CLKVE1			:1;
		unsigned long	CLKVE2			:1;
		unsigned long	CLKVD0			:1;
		unsigned long	CLKVD1			:1;
		unsigned long	CLKVD2			:1;
		unsigned long	RESERVED		:2;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	RESERVED0		:3;
		unsigned long	CLKVD0			:1;
		unsigned long	CLKVD1			:1;
		unsigned long	CLKVD2			:1;
		unsigned long	RESERVED1		:1;
		unsigned long	CLKVOREM		:1;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	}bit;
};

/*	structure of CLKENDA		*/
union io_sdramc_clkenda{
	unsigned long		word;
	struct {
		unsigned long	CLKB0DA			:1;
		unsigned long	CLKB1DA			:1;
		unsigned long	CLKB2DA			:1;
		unsigned long	CLKB3DA			:1;
		unsigned long	CLKB4DA			:1;
		unsigned long	CLKB5DA			:1;
		unsigned long	CLKB6DA			:1;
		unsigned long	CLKB7DA			:1;
		unsigned long	CLKB8DA			:1;
		unsigned long	CLKB9DA			:1;
		unsigned long	CLKBADA			:1;
		unsigned long	CLKBBDA			:1;
		unsigned long	CLKBCDA			:1;
		unsigned long	CLKBDDA			:1;
		unsigned long	CLKBEDA			:1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long	CLKBFDA			:1;
		unsigned long	CLKB10DA		:1;
		unsigned long					:14;
		unsigned long	CLKDRBDA		:1;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	RESERVED0		:1;
		unsigned long	CLKB10DA		:1;
		unsigned long	RESERVED1		:1;
		unsigned long	CLKB12DA		:1;
		unsigned long	CLKB13DA		:1;
		unsigned long	CLKB14DA		:1;
		unsigned long	CLKB15DA		:1;
		unsigned long	RESERVED2		:9;
		unsigned long	CLKDRBDA		:1;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	}bit;
};

/*	structure of CLKENMON		*/
union io_sdramc_clkenmon{
	unsigned long		word;
	struct {
		unsigned long	CLKMON			:1;
		unsigned long					:31;

	}bit;
};

/*	structure of IRQEN			*/
union io_sdramc_irqen{
	unsigned long		word;
	struct {
		unsigned long	IRQEN			:1;
		unsigned long					:31;

	}bit;
};

/*	structure of IRQCLR			*/
union io_sdramc_irqclr{
	unsigned long		word;
	struct {
		unsigned long	IRQCLR			:1;
		unsigned long					:31;

	}bit;
};

/* Define i/o mapping */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
struct io_sdramc{
	// Bus IF.
	union io_sdramc_embnum		EMBNUM[30];			/* (0000 - 0077h) */
	unsigned char dmy_0078_02FF[0x0300-0x0078];		/* (0078 - 02FFh) */
	union io_sdramc_remapaddr	REMAPADDR[30];		/* (0300 - 0377h) */

	unsigned char dmy_0378_04FF[0x0500-0x0378];		/* (0378 - 04FFh) */
	unsigned char dmy_0500_0FFF[0x1000-0x0500];		/* (0500 - 0FFFh) */
	unsigned char dmy_1000_25FF[0x2600-0x1000];		/* (1000 - 25FFh) */

	union io_sdramc_chsel		CHSEL[30];			/* (2600 - 2677h) */
	unsigned char dmy_2678_4FFF[0x5000-0x2678];		/* (2678 - 4FFFh) */

	// Command arbitration.
	union io_sdramc_ratecount	RATECONT[30];		/* (5000 - 5077h) */
	unsigned char dmy_5078_51FF[0x5200-0x5078];		/* (5078 - 51FFh) */

	union io_sdramc_masterop	MASTEROPTION[30];	/* (5200 - 5277h) */
	unsigned char dmy_5278_53FF[0x5400-0x5278];		/* (5278 - 53FFh) */

	union io_sdramc_priorit		PRIORIT_CAT[30];	/* (5400 - 5477h) */
	unsigned char dmy_5478_54FF[0x5500-0x5478];		/* (5478 - 54FFh) */

	unsigned char dmy_5500_59FF[0x5A00-0x5500];		/* (5500 - 59FFh) */

	union io_sdramc_ratechg		RATE_CHANGE;		/* (5A00 - 5A03h) */
	unsigned char dmy_5A04_5AFF[0x5B00-0x5A04];		/* (5A04 - 5AFFh) */

	unsigned char dmy_5B00_7FFF[0x8000-0x5B00];		/* (5B00 - 7FFFh) */

	// DRAM control.
	union io_sdramc_umccmda		UMCCMDCTLA;			/* (8000 - 8003h) */
	union io_sdramc_umccmdb		UMCCMDCTLB;			/* (8004 - 8007h) */
	union io_sdramc_umccmdc		UMCCMDCTLC;			/* (8008 - 800Bh) */
	union io_sdramc_umccmdd		UMCCMDCTLD;			/* (800C - 800Fh) */
	unsigned char dmy_8010_801F[0x8020-0x8010];		/* (8010 - 801Fh) */
	union io_sdramc_umcinita	UMCINITCTLA;		/* (8020 - 8023h) */
	union io_sdramc_umcinitb	UMCINITCTLB;		/* (8024 - 8027h) */
	union io_sdramc_umcinitc	UMCINITCTLC;		/* (8028 - 802Bh) */
	unsigned char dmy_802C_802F[0x8030-0x802C];		/* (802C - 802Fh) */
	union io_sdramc_umcdrm		UMCDRMMRA;			/* (8030 - 8033h) */
	unsigned char dmy_8034_803F[0x8040-0x8034];		/* (8034 - 803Fh) */
	union io_sdramc_umcinitset	UMCINITSET;			/* (8040 - 8043h) */
	union io_sdramc_umcinitsta	UMCINITSTAT;		/* (8044 - 8047h) */
	unsigned char dmy_8048_804F[0x8050-0x8048];		/* (8048 - 804Fh) */
	union io_sdramc_umccmde		UMCCMDCTLE;			/* (8050 - 8053h) */
	union io_sdramc_umccmdf		UMCCMDCTLF;			/* (8054 - 8057h) */
	union io_sdramc_umccmdg		UMCCMDCTLG;			/* (8058 - 805Bh) */
	union io_sdramc_umccmdh		UMCCMDCTLH;			/* (805C - 805Fh) */
	union io_sdramc_umccmdi		UMCCMDCTLI;			/* (8060 - 8063h) */
	union io_sdramc_umccmdj		UMCCMDCTLJ;			/* (8064 - 8067h) */
	unsigned char dmy_8068_807F[0x8080-0x8068];		/* (8068 - 807Fh) */
	union io_sdramc_umcspca		UMCSPCSETA;			/* (8080 - 8083h) */
	union io_sdramc_umcspcb		UMCSPCSETB;			/* (8084 - 8087h) */
	unsigned char dmy_8088_808F[0x8090-0x8088];		/* (8088 - 808Fh) */
	union io_sdramc_umcspcsta	UMCSPCSTAT;			/* (8090 - 8093h) */
	unsigned char dmy_8094_809F[0x80A0-0x8094];		/* (8094 - 809Fh) */

	unsigned char dmy_80A0_81FF[0x8200-0x80A0];		/* (80A0 - 81FFh) */

	union io_sdramc_umcmema		UMCMEMCONF0A;		/* (8200 - 8203h) */
	union io_sdramc_umcmemb		UMCMEMCONF0B;		/* (8204 - 8207h) */
	unsigned char dmy_8208_820F[0x8210-0x8208];		/* (8208 - 820Fh) */
	union io_sdramc_umcmema		UMCMEMCONF1A;		/* (8210 - 8213h) */
	union io_sdramc_umcmemb		UMCMEMCONF1B;		/* (8214 - 8217h) */
	unsigned char dmy_8218_823F[0x8240-0x8218];		/* (8218 - 823Fh) */
	union io_sdramc_umcmemch	UMCMEMCONFCH;		/* (8240 - 8243h) */
	unsigned char dmy_8244_824F[0x8250-0x8244];		/* (8244 - 824Fh) */
	union io_sdramc_umcmemset	UMCMEMMAPSET;		/* (8250 - 8253h) */
	unsigned char dmy_8254_827F[0x8280-0x8254];		/* (8254 - 827Fh) */

	unsigned char dmy_8280_82FF[0x8300-0x8280];		/* (8280 - 82FFh) */

	union io_sdramc_umcacchk	UMCACCCHKA;			/* (8300 - 8303h) */
	union io_sdramc_umcaccrsa	UMCACCARSAA;		/* (8304 - 8307h) */
	union io_sdramc_umcaccrea	UMCACCAREAA;		/* (8308 - 830Bh) */
	union io_sdramc_umcwaccst	UMCWACCMSTA;		/* (830C - 830Fh) */
	union io_sdramc_umcraccst	UMCRACCMSTA;		/* (8310 - 8313h) */
	unsigned char dmy_8314_831F[0x8320-0x8314];		/* (8314 - 831Fh) */
	union io_sdramc_umcacchk	UMCACCCHKB;			/* (8320 - 8323h) */
	union io_sdramc_umcaccrsa	UMCACCARSAB;		/* (8324 - 8327h) */
	union io_sdramc_umcaccrea	UMCACCAREAB;		/* (8328 - 832Bh) */
	union io_sdramc_umcwaccst	UMCWACCMSTB;		/* (832C - 832Fh) */
	union io_sdramc_umcraccst	UMCRACCMSTB;		/* (8330 - 8333h) */
	unsigned char dmy_8334_833F[0x8340-0x8334];		/* (8334 - 833Fh) */
	union io_sdramc_umcacchk	UMCOODCHK;			/* (8340 - 8343h) */
	unsigned char dmy_8344_835F[0x8360-0x8344];		/* (8344 - 835Fh) */
	union io_sdramc_umcmracs	UMCMRACS;			/* (8360 - 8363h) */
	union io_sdramc_umcmracsst	UMCMRACSST;			/* (8364 - 8367h) */
	unsigned char dmy_8368_836F[0x8370-0x8368];		/* (8368 - 836Fh) */
	union io_sdramc_umcmrwdt	UMCMRWDT;			/* (8370 - 8373h) */
	unsigned char dmy_8374_838F[0x8390-0x8374];		/* (8374 - 838Fh) */
	union io_sdramc_umcmrrdt	UMCMRRDT;			/* (8390 - 8393h) */
	unsigned char dmy_8394_83FF[0x8400-0x8394];		/* (8394 - 83FFh) */

	unsigned char dmy_8400_85FF[0x8600-0x8400];		/* (8400 - 85FFh) */

	union io_sdramc_umcrdata0	UMCRDATACTL_D0;		/* (8600 - 8603h) */
	union io_sdramc_umcwdata0	UMCWDATACTL_D0;		/* (8604 - 8607h) */
	union io_sdramc_umcrdata1	UMCRDATACTL_D1;		/* (8608 - 860Bh) */
	union io_sdramc_umcwdata1	UMCWDATACTL_D1;		/* (860C - 860Fh) */
	union io_sdramc_umcdataset	UMCDATASET;			/* (8610 - 8613h) */
	unsigned char dmy_8614_861F[0x8620-0x8614];		/* (8614 - 861Fh) */
	union io_sdramc_umcwdatswp	UMCWDATASWP;		/* (8620 - 8623h) */
	unsigned char dmy_8624_867F[0x8680-0x8624];		/* (8624 - 867Fh) */

	unsigned char dmy_8680_88FF[0x8900-0x8680];		/* (8680 - 88FFh) */

	unsigned char dmy_8900_894F[0x8950-0x8900];		/* (8900 - 894Fh) */
	union io_sdramc_umcintc		UMCINTCAUSEM0;		/* (8950 - 8953h) */
	unsigned char dmy_8954_8957[0x8958-0x8954];		/* (8954 - 8957h) */
	union io_sdramc_umcintmsk	UMCINTMASKM0;		/* (8958 - 895Bh) */
	unsigned char dmy_895C_8967[0x8968-0x895C];		/* (895C - 8967h) */
	union io_sdramc_accchkflg		ACCCHKFLG;		/* (8968 - 896Bh) */
	union io_sdramc_accchkflgclr	ACCCHKFLGCLR;	/* (896C - 896Fh) */
	union io_sdramc_accchkloga		ACCCHKLOGA;		/* (8970 - 8973h) */
	union io_sdramc_accchklogb		ACCCHKLOGB;		/* (8974 - 8977h) */
	unsigned char dmy_8978_89FF[0x8A00-0x8978];		/* (8978 - 89FFh) */

	unsigned char dmy_8A00_8BFF[0x8C00-0x8A00];		/* (8A00 - 8BFFh) */

	union io_sdramc_umcphya		UMCPHYCTLA;			/* (8C00 - 8C03h) */
	union io_sdramc_umcphyb		UMCPHYCTLB;			/* (8C04 - 8C07h) */
	union io_sdramc_umcphyc		UMCPHYCTLC;			/* (8C08 - 8C0Bh) */
	unsigned char dmy_8C0C_8C3F[0x8C40-0x8C0C];		/* (8C0C - 8C3Fh) */
	union io_sdramc_umcdfictla	UMCDFICTLA;			/* (8C40 - 8C43h) */
	unsigned char dmy_8C44_8CFF[0x8D00-0x8C44];		/* (8C44 - 8CFFh) */
	union io_sdramc_umcpfmctl	UMCPFMCTL;			/* (8D00 - 8D03h) */
	unsigned char dmy_8D04_8D1F[0x8D20-0x8D04];		/* (8D04 - 8D1Fh) */
	union io_sdramc_umcpfmst	UMCPFMST[30];		/* (8D20 - 8D97h) */
	unsigned char dmy_8D98_8E3F[0x8E40-0x8D98];		/* (8D98 - 8E3Fh) */
	union io_sdramc_umcpfmctl	UMCMIDPFMCTL;		/* (8E40 - 8E43h) */
	unsigned char dmy_8E44_8E5F[0x8E60-0x8E44];		/* (8E44 - 8E5Fh) */
	union io_sdramc_umcpfmst	UMCMIDPFMST[62];	/* (8E60 - 8F57h) */
	unsigned char dmy_8F58_8FFF[0x9000-0x8F58];		/* (8F58 - 8FFFh) */

	unsigned char dmy_9000_EFFF[0xF000-0x9000];		/* (9000 - EFFFh) */

	// Overall control.
	union io_sdramc_clkena		CLKENBIFA;			/* (F000 - F003h) */
	union io_sdramc_clkenb		CLKENBIFB;			/* (F004 - F007h) */
	union io_sdramc_clkenc		CLKENBIFC;			/* (F008 - F00Bh) */
	unsigned char dmy_F00C_F00F[0xF010-0xF00C];		/* (F00C - F00Fh) */
	union io_sdramc_clkenda		CLKENDA;			/* (F010 - F013h) */
	unsigned char dmy_F014_F01F[0xF020-0xF014];		/* (F014 - F01Fh) */
	union io_sdramc_clkenmon	CLKENMON;			/* (F020 - F023h) */
	unsigned char dmy_F024_F0FF[0xF100-0xF024];		/* (F024 - F0FFh) */

	unsigned char dmy_F100_F2FF[0xF300-0xF100];		/* (F100 - F2FFh) */

	union io_sdramc_irqen		IRQEN;				/* (F300 - F303h) */
	unsigned char dmy_F304_F307[0xF308-0xF304];		/* (F304 - F307h) */
	union io_sdramc_irqclr		IRQCLR;				/* (F308 - F30Bh) */
	unsigned char dmy_F30C_F3FF[0xF400-0xF30C];		/* (F30C - F3FFh) */

	unsigned char dmy_F400_1FFFF[0x20000-0xF400];	/* (F400 - 1FFFFh) */
};
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN --
struct io_sdramc{
	// Bus IF.
	union io_sdramc_embnum		EMBNUM[34];			/* (0000 - 0087h) */
	unsigned char dmy_0088_02FF[0x0300-0x0088];		/* (0088 - 02FFh) */
	union io_sdramc_remapaddr	REMAPADDR[34];		/* (0300 - 0387h) */

	unsigned char dmy_0388_04FF[0x0500-0x0388];		/* (0388 - 04FFh) */
	unsigned char dmy_0500_0FFF[0x1000-0x0500];		/* (0500 - 0FFFh) */
	unsigned char dmy_1000_25FF[0x2600-0x1000];		/* (1000 - 25FFh) */

	union io_sdramc_chsel		CHSEL[34];			/* (2600 - 2687h) */
	unsigned char dmy_2688_4FFF[0x5000-0x2688];		/* (2688 - 4FFFh) */

	// Command arbitration.
	union io_sdramc_ratecount	RATECONT[34];		/* (5000 - 5087h) */
	unsigned char dmy_5088_51FF[0x5200-0x5088];		/* (5088 - 51FFh) */

	union io_sdramc_masterop	MASTEROPTION[34];	/* (5200 - 5287h) */
	unsigned char dmy_5278_53FF[0x5400-0x5288];		/* (5288 - 53FFh) */

	union io_sdramc_priorit		PRIORIT_CAT[34];	/* (5400 - 5487h) */
	unsigned char dmy_5488_54FF[0x5500-0x5488];		/* (5488 - 54FFh) */

	unsigned char dmy_5500_59FF[0x5A00-0x5500];		/* (5500 - 59FFh) */

	union io_sdramc_ratechg		RATE_CHANGE;		/* (5A00 - 5A03h) */
	unsigned char dmy_5A04_5AFF[0x5B00-0x5A04];		/* (5A04 - 5AFFh) */

	unsigned char dmy_5B00_7FFF[0x8000-0x5B00];		/* (5B00 - 7FFFh) */

	// DRAM control.
	union io_sdramc_umccmda		UMCCMDCTLA;			/* (8000 - 8003h) */
	union io_sdramc_umccmdb		UMCCMDCTLB;			/* (8004 - 8007h) */
	union io_sdramc_umccmdc		UMCCMDCTLC;			/* (8008 - 800Bh) */
	union io_sdramc_umccmdd		UMCCMDCTLD;			/* (800C - 800Fh) */
	unsigned char dmy_8010_801F[0x8020-0x8010];		/* (8010 - 801Fh) */
	union io_sdramc_umcinita	UMCINITCTLA;		/* (8020 - 8023h) */
	union io_sdramc_umcinitb	UMCINITCTLB;		/* (8024 - 8027h) */
	union io_sdramc_umcinitc	UMCINITCTLC;		/* (8028 - 802Bh) */
	unsigned char dmy_802C_802F[0x8030-0x802C];		/* (802C - 802Fh) */
	union io_sdramc_umcdrm		UMCDRMMRA;			/* (8030 - 8033h) */
	unsigned char dmy_8034_803F[0x8040-0x8034];		/* (8034 - 803Fh) */
	union io_sdramc_umcinitset	UMCINITSET;			/* (8040 - 8043h) */
	union io_sdramc_umcinitsta	UMCINITSTAT;		/* (8044 - 8047h) */
	unsigned char dmy_8048_804F[0x8050-0x8048];		/* (8048 - 804Fh) */
	union io_sdramc_umccmde		UMCCMDCTLE;			/* (8050 - 8053h) */
	union io_sdramc_umccmdf		UMCCMDCTLF;			/* (8054 - 8057h) */
	union io_sdramc_umccmdg		UMCCMDCTLG;			/* (8058 - 805Bh) */
	union io_sdramc_umccmdh		UMCCMDCTLH;			/* (805C - 805Fh) */
	union io_sdramc_umccmdi		UMCCMDCTLI;			/* (8060 - 8063h) */
	union io_sdramc_umccmdj		UMCCMDCTLJ;			/* (8064 - 8067h) */
	unsigned char dmy_8068_807F[0x8080-0x8068];		/* (8068 - 807Fh) */
	union io_sdramc_umcspca		UMCSPCSETA;			/* (8080 - 8083h) */
	union io_sdramc_umcspcb		UMCSPCSETB;			/* (8084 - 8087h) */
	unsigned char dmy_8088_808F[0x8090-0x8088];		/* (8088 - 808Fh) */
	union io_sdramc_umcspcsta	UMCSPCSTAT;			/* (8090 - 8093h) */
	unsigned char dmy_8094_809F[0x80A0-0x8094];		/* (8094 - 809Fh) */

	unsigned char dmy_80A0_81FF[0x8200-0x80A0];		/* (80A0 - 81FFh) */

	union io_sdramc_umcmema		UMCMEMCONF0A;		/* (8200 - 8203h) */
	union io_sdramc_umcmemb		UMCMEMCONF0B;		/* (8204 - 8207h) */
	unsigned char dmy_8208_820F[0x8210-0x8208];		/* (8208 - 820Fh) */
	union io_sdramc_umcmema		UMCMEMCONF1A;		/* (8210 - 8213h) */
	union io_sdramc_umcmemb		UMCMEMCONF1B;		/* (8214 - 8217h) */
	unsigned char dmy_8218_823F[0x8240-0x8218];		/* (8218 - 823Fh) */
	union io_sdramc_umcmemch	UMCMEMCONFCH;		/* (8240 - 8243h) */
	unsigned char dmy_8244_824F[0x8250-0x8244];		/* (8244 - 824Fh) */
	union io_sdramc_umcmemset	UMCMEMMAPSET;		/* (8250 - 8253h) */
	unsigned char dmy_8254_827F[0x8280-0x8254];		/* (8254 - 827Fh) */

	unsigned char dmy_8280_82FF[0x8300-0x8280];		/* (8280 - 82FFh) */

	union io_sdramc_umcacchk	UMCACCCHKA;			/* (8300 - 8303h) */
	union io_sdramc_umcaccrsa	UMCACCARSAA;		/* (8304 - 8307h) */
	union io_sdramc_umcaccrea	UMCACCAREAA;		/* (8308 - 830Bh) */
	union io_sdramc_umcwaccst	UMCWACCMSTAL;		/* (830C - 830Fh) */
	union io_sdramc_umcraccst	UMCRACCMSTAL;		/* (8310 - 8313h) */
	union io_sdramc_umcwaccst	UMCWACCMSTAU;		/* (8314 - 8317h) */
	union io_sdramc_umcraccst	UMCRACCMSTAU;		/* (8318 - 831Bh) */
	unsigned char dmy_831C_831F[0x8320-0x831C];		/* (831C - 831Fh) */
	union io_sdramc_umcacchk	UMCACCCHKB;			/* (8320 - 8323h) */
	union io_sdramc_umcaccrsa	UMCACCARSAB;		/* (8324 - 8327h) */
	union io_sdramc_umcaccrea	UMCACCAREAB;		/* (8328 - 832Bh) */
	union io_sdramc_umcwaccst	UMCWACCMSTBL;		/* (832C - 832Fh) */
	union io_sdramc_umcraccst	UMCRACCMSTBL;		/* (8330 - 8333h) */
	union io_sdramc_umcwaccst	UMCWACCMSTBU;		/* (8334 - 8337h) */
	union io_sdramc_umcraccst	UMCRACCMSTBU;		/* (8338 - 833Bh) */
	unsigned char dmy_833C_833F[0x8340-0x833C];		/* (833C - 833Fh) */
	union io_sdramc_umcacchk	UMCOODCHK;			/* (8340 - 8343h) */
	unsigned char dmy_8344_835F[0x8360-0x8344];		/* (8344 - 835Fh) */
	union io_sdramc_umcmracs	UMCMRACS;			/* (8360 - 8363h) */
	union io_sdramc_umcmracsst	UMCMRACSST;			/* (8364 - 8367h) */
	unsigned char dmy_8368_836F[0x8370-0x8368];		/* (8368 - 836Fh) */
	union io_sdramc_umcmrwdt	UMCMRWDT;			/* (8370 - 8373h) */
	unsigned char dmy_8374_838F[0x8390-0x8374];		/* (8374 - 838Fh) */
	union io_sdramc_umcmrrdt	UMCMRRDT;			/* (8390 - 8393h) */
	unsigned char dmy_8394_83FF[0x8400-0x8394];		/* (8394 - 83FFh) */

	unsigned char dmy_8400_85FF[0x8600-0x8400];		/* (8400 - 85FFh) */

	union io_sdramc_umcrdata0	UMCRDATACTL_D0;		/* (8600 - 8603h) */
	union io_sdramc_umcwdata0	UMCWDATACTL_D0;		/* (8604 - 8607h) */
	union io_sdramc_umcrdata1	UMCRDATACTL_D1;		/* (8608 - 860Bh) */
	union io_sdramc_umcwdata1	UMCWDATACTL_D1;		/* (860C - 860Fh) */
	union io_sdramc_umcdataset	UMCDATASET;			/* (8610 - 8613h) */
	unsigned char dmy_8614_861F[0x8620-0x8614];		/* (8614 - 861Fh) */
	union io_sdramc_umcwdatswp	UMCWDATASWP;		/* (8620 - 8623h) */
	unsigned char dmy_8624_867F[0x8680-0x8624];		/* (8624 - 867Fh) */

	unsigned char dmy_8680_88FF[0x8900-0x8680];		/* (8680 - 88FFh) */

	unsigned char dmy_8900_894F[0x8950-0x8900];		/* (8900 - 894Fh) */
	union io_sdramc_umcintc		UMCINTCAUSEM0;		/* (8950 - 8953h) */
	union io_sdramc_umcintc		UMCINTCAUSEM1;		/* (8954 - 8957h) */
	union io_sdramc_umcintmsk	UMCINTMASKM0;		/* (8958 - 895Bh) */
	union io_sdramc_umcintmsk	UMCINTMASKM1;		/* (895C - 895Fh) */
	unsigned char dmy_8960_8967[0x8968-0x8960];		/* (8960 - 8967h) */
	union io_sdramc_accchkflg		ACCCHKFLG;		/* (8968 - 896Bh) */
	union io_sdramc_accchkflgclr	ACCCHKFLGCLR;	/* (896C - 896Fh) */
	union io_sdramc_accchkloga		ACCCHKLOGA;		/* (8970 - 8973h) */
	union io_sdramc_accchklogb		ACCCHKLOGB;		/* (8974 - 8977h) */
	unsigned char dmy_8978_89FF[0x8A00-0x8978];		/* (8978 - 89FFh) */

	unsigned char dmy_8A00_8BFF[0x8C00-0x8A00];		/* (8A00 - 8BFFh) */

	union io_sdramc_umcphya		UMCPHYCTLA;			/* (8C00 - 8C03h) */
	union io_sdramc_umcphyb		UMCPHYCTLB;			/* (8C04 - 8C07h) */
	union io_sdramc_umcphyc		UMCPHYCTLC;			/* (8C08 - 8C0Bh) */
	unsigned char dmy_8C0C_8C13[0x8C14-0x8C0C];		/* (8C0C - 8C13h) */
	union io_sdramc_umcdfictlb	UMCDFICTLB;			/* (8C14 - 8C17h) */
	unsigned char dmy_8C18_8C3F[0x8C40-0x8C18];		/* (8C18 - 8C3Fh) */
	union io_sdramc_umcdfictla	UMCDFICTLA;			/* (8C40 - 8C43h) */
	unsigned char dmy_8C44_8CFF[0x8D00-0x8C44];		/* (8C44 - 8CFFh) */
	union io_sdramc_umcpfmctl	UMCPFMCTL;			/* (8D00 - 8D03h) */
	unsigned char dmy_8D04_8D1F[0x8D20-0x8D04];		/* (8D04 - 8D1Fh) */
	union io_sdramc_umcpfmst	UMCPFMST[34];		/* (8D20 - 8DA7h) */
	unsigned char dmy_8DA8_8E3F[0x8E40-0x8DA8];		/* (8DA8 - 8E3Fh) */
	union io_sdramc_umcpfmctl	UMCMIDPFMCTL;		/* (8E40 - 8E43h) */
	unsigned char dmy_8E44_8E5F[0x8E60-0x8E44];		/* (8E44 - 8E5Fh) */
	union io_sdramc_umcpfmst	UMCMIDPFMST[62];	/* (8E60 - 8F57h) */
	unsigned char dmy_8F58_8FFF[0x9000-0x8F58];		/* (8F58 - 8FFFh) */

	unsigned char dmy_9000_EFFF[0xF000-0x9000];		/* (9000 - EFFFh) */

	// Overall control.
	union io_sdramc_clkena		CLKENBIFA;			/* (F000 - F003h) */
	union io_sdramc_clkenb		CLKENBIFB;			/* (F004 - F007h) */
	union io_sdramc_clkenc		CLKENBIFC;			/* (F008 - F00Bh) */
	unsigned char dmy_F00C_F00F[0xF010-0xF00C];		/* (F00C - F00Fh) */
	union io_sdramc_clkenda		CLKENDA;			/* (F010 - F013h) */
	unsigned char dmy_F014_F01F[0xF020-0xF014];		/* (F014 - F01Fh) */
	union io_sdramc_clkenmon	CLKENMON;			/* (F020 - F023h) */
	unsigned char dmy_F024_F0FF[0xF100-0xF024];		/* (F024 - F0FFh) */

	unsigned char dmy_F100_F2FF[0xF300-0xF100];		/* (F100 - F2FFh) */

	union io_sdramc_irqen		IRQEN;				/* (F300 - F303h) */
	unsigned char dmy_F304_F307[0xF308-0xF304];		/* (F304 - F307h) */
	union io_sdramc_irqclr		IRQCLR;				/* (F308 - F30Bh) */
	unsigned char dmy_F30C_F3FF[0xF400-0xF30C];		/* (F30C - F3FFh) */

	unsigned char dmy_F400_1FFFF[0x20000-0xF400];	/* (F400 - 1FFFFh) */
};
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

extern volatile struct io_sdramc	IO_SDRAMC[2];
