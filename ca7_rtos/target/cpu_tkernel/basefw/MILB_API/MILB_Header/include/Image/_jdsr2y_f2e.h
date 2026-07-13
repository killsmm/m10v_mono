/**
 * @file		_jdsr2y_f2e.h
 * @brief		Definition JDSR2Y_F2E Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "__fr2y6a.h"
#include "__jdsr2y_f2e_sram.h"

/*	structure of YYRAXCTL	(2841_C000h)	*/
union io_r2y_yyraxctl{
	unsigned long		word;
	struct {
		unsigned long	YYRCACHE_0	:4;
		unsigned long	YYRPROT_0	:3;
		unsigned long				:1;
		unsigned long	YYRCACHE_1	:4;
		unsigned long	YYRPROT_1	:3;
		unsigned long				:1;
		unsigned long	YYRCACHE_2	:4;
		unsigned long	YYRPROT_2	:3;
		unsigned long				:9;
	}bit;
};

/*	structure of YYW0AXCTL	(2841_C004h)	*/
union io_r2y_yyw0axctl{
	unsigned long		word;
	struct {
		unsigned long	YYW0CACHE_0	:4;
		unsigned long	YYW0PROT_0	:3;
		unsigned long				:1;
		unsigned long	YYW0CACHE_1	:4;
		unsigned long	YYW0PROT_1	:3;
		unsigned long				:1;
		unsigned long	YYW0CACHE_2	:4;
		unsigned long	YYW0PROT_2	:3;
		unsigned long				:9;
	}bit;
};

/*	structure of YYWAAXCTL	(2841_C008h)	*/
union io_r2y_yywaaxctl{
	unsigned long		word;
	struct {
		unsigned long	YYWACACHE_0	:4;
		unsigned long	YYWAPROT_0	:3;
		unsigned long				:1;
		unsigned long	YYWACACHE_1	:4;
		unsigned long	YYWAPROT_1	:3;
		unsigned long				:17;
	}bit;
};

/*	structure of YYW1AXCTL	(2841_C00Ch)	*/
union io_r2y_yyw1axctl{
	unsigned long		word;
	struct {
		unsigned long	YYW1CACHE_0	:4;
		unsigned long	YYW1PROT_0	:3;
		unsigned long				:1;
		unsigned long	YYW1CACHE_1	:4;
		unsigned long	YYW1PROT_1	:3;
		unsigned long				:1;
		unsigned long	YYW1CACHE_2	:4;
		unsigned long	YYW1PROT_2	:3;
		unsigned long				:9;
	}bit;
};

/*	structure of YYW2AXCTL	(2841_C010h)	*/
union io_r2y_yyw2axctl{
	unsigned long		word;
	struct {
		unsigned long	YYW2CACHE	:4;
		unsigned long	YYW2PROT	:3;
		unsigned long				:25;
	}bit;
};

/*	structure of YYRAXSTS	(2841_C014h)	*/
union io_r2y_yyraxsts{
	unsigned long		word;
	struct {
		unsigned long	YYRRESP_0	:2;
		unsigned long				:6;
		unsigned long	YYRRESP_1	:2;
		unsigned long				:6;
		unsigned long	YYRRESP_2	:2;
		unsigned long				:14;
	}bit;
};

/*	structure of YYW0AXSTS	(2841_C018h)	*/
union io_r2y_yyw0axsts{
	unsigned long		word;
	struct {
		unsigned long	YY0BRESP_0	:2;
		unsigned long				:6;
		unsigned long	YY0BRESP_1	:2;
		unsigned long				:6;
		unsigned long	YY0BRESP_2	:2;
		unsigned long				:14;
	}bit;
};

/*	structure of YYWAAXSTS	(2841_C01Ch)	*/
union io_r2y_yywaaxsts{
	unsigned long		word;
	struct {
		unsigned long	YYABRESP_0	:2;
		unsigned long				:6;
		unsigned long	YYABRESP_1	:2;
		unsigned long				:22;
	}bit;
};

/*	structure of YYW1AXSTS	(2841_C020h)	*/
union io_r2y_yyw1axsts{
	unsigned long		word;
	struct {
		unsigned long	YY1BRESP_0	:2;
		unsigned long				:6;
		unsigned long	YY1BRESP_1	:2;
		unsigned long				:6;
		unsigned long	YY1BRESP_2	:2;
		unsigned long				:14;
	}bit;
};

/*	structure of YYW2AXSTS	(2841_C024h)	*/
union io_r2y_yyw2axsts{
	unsigned long		word;
	struct {
		unsigned long	YY2BRESP	:2;
		unsigned long				:30;
	}bit;
};

/*	structure of YYRAXMD	(2841_C028h)	*/
union io_r2y_yyraxmd{
	unsigned long		word;
	struct {
		unsigned long	YYRBL	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of YYW0AXMD	(2841_C02Ch)	*/
union io_r2y_yyw0axmd{
	unsigned long		word;
	struct {
		unsigned long	YYW0MSL	:6;
		unsigned long			:2;
		unsigned long	YYW0BL	:2;
		unsigned long			:6;
		unsigned long	YYW0DOE	:3;
		unsigned long			:13;
	}bit;
};

/*	structure of YYWAAXMD	(2841_C030h)	*/
union io_r2y_yywaaxmd{
	unsigned long		word;
	struct {
		unsigned long	YYWAMSL	:4;
		unsigned long			:4;
		unsigned long	YYWABL	:2;
		unsigned long			:6;
		unsigned long	YYWADOE	:2;
		unsigned long			:14;
	}bit;
};

/*	structure of YYW1AXMD	(2841_C034h)	*/
union io_r2y_yyw1axmd{
	unsigned long		word;
	struct {
		unsigned long	YYW1MSL	:6;
		unsigned long			:2;
		unsigned long	YYW1BL	:2;
		unsigned long			:6;
		unsigned long	YYW1DOE	:3;
		unsigned long			:13;
	}bit;
};

/*	structure of YYW2AXMD	(2841_C038h)	*/
union io_r2y_yyw2axmd{
	unsigned long		word;
	struct {
		unsigned long	YYW2MSL	:2;
		unsigned long			:6;
		unsigned long	YYW2BL	:2;
		unsigned long			:22;
	}bit;
};

/*	structure of YYRREQMSK	(2841_C040h)	*/
union io_r2y_yyrreqmsk{
	unsigned long		word[2];
	struct {
		unsigned long	YYRREQMSK_0	:12;
		unsigned long				:4;
		unsigned long	YYRREQMSK_1	:12;
		unsigned long				:4;
		unsigned long	YYRREQMSK_2	:12;
		unsigned long				:20;
	}bit;
};

/*	structure of YYW0REQMSK	(2841_C048h)	*/
union io_r2y_yyw0reqmsk{
	unsigned long		word[2];
	struct {
		unsigned long	YYW0REQMSK_0	:12;
		unsigned long					:4;
		unsigned long	YYW0REQMSK_1	:12;
		unsigned long					:4;
		unsigned long	YYW0REQMSK_2	:12;
		unsigned long					:20;
	}bit;
};

/*	structure of YYWAREQMSK	(2841_C050h)	*/
union io_r2y_yywareqmsk{
	unsigned long		word;
	struct {
		unsigned long	YYWAREQMSK_0	:12;
		unsigned long					:4;
		unsigned long	YYWAREQMSK_1	:12;
		unsigned long					:4;
	}bit;
};

/*	structure of YYW1REQMSK	(2841_C058h)	*/
union io_r2y_yyw1reqmsk{
	unsigned long		word[2];
	struct {
		unsigned long	YYW1REQMSK_0	:12;
		unsigned long					:4;
		unsigned long	YYW1REQMSK_1	:12;
		unsigned long					:4;
		unsigned long	YYW1REQMSK_2	:12;
		unsigned long					:20;
	}bit;
};

/*	structure of YYW2REQMSK	(2841_C060h)	*/
union io_r2y_yyw2reqmsk{
	unsigned long		word;
	struct {
		unsigned long	YYW2REQMSK	:12;
		unsigned long				:20;
	}bit;
};

/*	structure of YYINTE	(2841_C080h)	*/
union io_r2y_yyinte{
	unsigned long		word;
	struct {
		unsigned long	YYREE	:1;
		unsigned long			:3;
		unsigned long	YYW0EE	:1;
		unsigned long	YYWAEE	:1;
		unsigned long	YYW1EE	:1;
		unsigned long	YYW2EE	:1;
		unsigned long	YYWEE	:1;
		unsigned long			:3;
		unsigned long	LINT0E	:1;
		unsigned long	LINT1E	:1;
		unsigned long	LINT2E	:1;
		unsigned long			:1;
		unsigned long	YYRERE	:1;
		unsigned long	YYWERE	:1;
		unsigned long			:2;
		unsigned long	AXRERE	:1;
		unsigned long	AXWERE	:1;
		unsigned long			:10;
	}bit;
};

/*	structure of YYINTF	(2841_C084h)	*/
union io_r2y_yyintf{
	unsigned long		word;
	struct {
		unsigned long	__YYREF		:1;
		unsigned long				:3;
		unsigned long	__YYW0EF	:1;
		unsigned long	__YYWAEF	:1;
		unsigned long	__YYW1EF	:1;
		unsigned long	__YYW2EF	:1;
		unsigned long	__YYWEF		:1;
		unsigned long				:3;
		unsigned long	__LINT0F	:1;
		unsigned long	__LINT1F	:1;
		unsigned long	__LINT2F	:1;
		unsigned long				:1;
		unsigned long	__YYRERF	:1;
		unsigned long	__YYWERF	:1;
		unsigned long				:2;
		unsigned long	__AXRERF	:1;
		unsigned long	__AXWERF	:1;
		unsigned long				:10;
	}bit;
};

/*	structure of LINTLV	(2841_C088h)	*/
union io_r2y_lintlv{
	unsigned long		word[2];
	struct {
		unsigned long	LINTLV_0	:14;
		unsigned long				:2;
		unsigned long	LINTLV_1	:14;
		unsigned long				:2;
		unsigned long	LINTLV_2	:14;
		unsigned long				:18;
	}bit;
};

/*	structure of LINTSEL	(2841_C090h)	*/
union io_r2y_lintsel{
	unsigned long		word;
	struct {
		unsigned long	LINTS_0	:1;
		unsigned long			:7;
		unsigned long	LINTS_1	:1;
		unsigned long			:7;
		unsigned long	LINTS_2	:1;
		unsigned long			:15;
	}bit;
};

/*	structure of R2YMODE	(2841_C094h)	*/
union io_r2y_r2ymode{
	unsigned long		word;
	struct {
		unsigned long	YCFBYP	:1;
		unsigned long	YCFPDD	:1;
		unsigned long			:2;
		unsigned long	MCCSL	:1;
		unsigned long	MCC1BM	:1;
		unsigned long			:26;
	}bit;
};

/*	structure of RAMAEN	(2841_C098h)	*/
union io_r2y_ramaen{
	unsigned long		word;
	struct {
		unsigned long	YYRAEN_RGB_DEKNEE		:1;
		unsigned long	YYRAEN_YYR				:1;
		unsigned long							:2;
		unsigned long	YW0AEN_YYW0				:1;
		unsigned long	YW0AEN_YYW0_BICUBIC		:1;
		unsigned long	YW0AEN_YYW0_EDGE		:1;
		unsigned long	YW0AEN_YYW0A			:1;
		unsigned long	YW1AEN_YYW1				:1;
		unsigned long	YW1AEN_YYW1_BICUBIC		:1;
		unsigned long	YW1AEN_YYW1_EDGE		:1;
		unsigned long	YW2AEN					:1;
		unsigned long	HSTAEN					:1;
		unsigned long							:19;
	}bit;
};

/*	structure of YYRTRG	(2841_C0C0h)	*/
union io_r2y_yyrtrg{
	unsigned long		word;
	struct {
		unsigned long	YYRTRG	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of YYRCTL	(2841_C0C4h)	*/
union io_r2y_yyrctl{
	unsigned long		word;
	struct {
		unsigned long	YYRDTYP	:2;
		unsigned long			:2;
		unsigned long	MONOEN	:1;
		unsigned long			:3;
		unsigned long	DKNEN	:1;
		unsigned long			:3;
		unsigned long	RGBLSFT	:2;
		unsigned long	RGBLSM	:1;
		unsigned long			:17;
	}bit;
};

/*	structure of DINCTL	(2841_C0C8h)	*/
union io_r2y_dinctl{
	unsigned long		word;
	struct {
		unsigned long	B2RDIN	:1;
		unsigned long			:7;
		unsigned long	FRMSTP	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of YYRA	(2841_C0D0h)	*/
union io_r2y_yyra{
	unsigned long		word[3];
	struct {
		unsigned long	YYRA_0	:32;
		unsigned long	YYRA_1	:32;
		unsigned long	YYRA_2	:32;
	}bit;
};

/*	structure of YYROFS	(2841_C0E0h)	*/
union io_r2y_yyrofs{
	unsigned long		word;
	struct {
		unsigned long	YYROFS_0	:3;
		unsigned long				:5;
		unsigned long	YYROFS_1	:3;
		unsigned long				:5;
		unsigned long	YYROFS_2	:3;
		unsigned long				:13;
	}bit;
};

/*	structure of YYRDEF	(2841_C0E4h)	*/
union io_r2y_yyrdef{
	unsigned long		word;
	struct {
		unsigned long	YYRDEF	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of YYRHSIZ	(2841_C0E8h)	*/
union io_r2y_yyrhsiz{
	unsigned long		word;
	struct {
		unsigned long	YYRHSIZ	:13;
		unsigned long			:19;
	}bit;
};

/*	structure of YYRVSIZ	(2841_C0ECh)	*/
union io_r2y_yyrvsiz{
	unsigned long		word;
	struct {
		unsigned long	YYRVSIZ	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of YYWTRG	(2841_C100h)	*/
union io_r2y_yywtrg{
	unsigned long		word;
	struct {
		unsigned long	YYWTRG	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of YYWCTL	(2841_C104h)	*/
union io_r2y_yywctl{
	unsigned long		word;
	struct {
		unsigned long	YYWMODE		:3;
		unsigned long				:1;
		unsigned long	YYWCSE		:1;
		unsigned long	YYWHRV		:1;
		unsigned long				:2;
		unsigned long	USEBANK0	:2;
		unsigned long	USEBANKA	:2;
		unsigned long	USEBANK1	:2;
		unsigned long	USEBANK2	:2;
		unsigned long				:16;
	}bit;
};

/*	structure of YYWBANK	(2841_C108h)	*/
union io_r2y_yywbank{
	unsigned long		word;
	struct {
		unsigned long	BANKST		:1;
		unsigned long				:3;
		unsigned long	BANK0PS		:2;
		unsigned long	BANKAPS		:2;
		unsigned long	BANK1PS		:2;
		unsigned long	BANK2PS		:2;
		unsigned long				:4;
		unsigned long	BANK0STS	:2;
		unsigned long	BANKASTS	:2;
		unsigned long	BANK1STS	:2;
		unsigned long	BANK2STS	:2;
		unsigned long				:8;
	}bit;
};

/*	structure of YYWKPP	(2841_C10Ch)	*/
union io_r2y_yywkpp{
	unsigned long		word;
	struct {
		unsigned long	YYWKPP	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of YYW0MD	(2841_C110h)	*/
union io_r2y_yyw0md{
	unsigned long		word;
	struct {
		unsigned long	RSZ0MD	:1;
		unsigned long			:3;
		unsigned long	RDC0MD	:2;
		unsigned long	RDC0EN	:1;
		unsigned long			:1;
		unsigned long	CSS0SL	:4;
		unsigned long	TRM0EN	:1;
		unsigned long			:3;
		unsigned long	SPL0EN	:1;
		unsigned long	CH0VFM	:1;
		unsigned long			:2;
		unsigned long	EXTBEN	:1;
		unsigned long	TRMBEN	:1;
		unsigned long			:2;
		unsigned long	EXTCEN	:1;
		unsigned long	TRMCEN	:1;
		unsigned long			:2;
		unsigned long	CH0AMD	:2;
		unsigned long	CHAVFM	:1;
		unsigned long			:1;
	}bit;
};

/*	structure of YYW1MD	(2841_C114h)	*/
union io_r2y_yyw1md{
	unsigned long		word;
	struct {
		unsigned long	RSZ1MD	:1;
		unsigned long			:3;
		unsigned long	RDC1MD	:2;
		unsigned long	RDC1EN	:1;
		unsigned long			:1;
		unsigned long	CSS1SL	:4;
		unsigned long	TRM1EN	:1;
		unsigned long	DFM1SL	:1;
		unsigned long			:2;
		unsigned long	SPL1EN	:1;
		unsigned long			:1;
		unsigned long	DTY1SL	:2;
		unsigned long			:12;
	}bit;
};

/*	structure of YYW2MD	(2841_C118h)	*/
union io_r2y_yyw2md{
	unsigned long		word;
	struct {
		unsigned long			:4;
		unsigned long	RDC2MD	:2;
		unsigned long	RDC2EN	:1;
		unsigned long			:5;
		unsigned long	TRM2EN	:1;
		unsigned long			:19;
	}bit;
};

/*	structure of HSTMD	(2841_C120h)	*/
union io_r2y_hstmd{
	unsigned long		word;
	struct {
		unsigned long	HSTEN	:1;
		unsigned long			:1;
		unsigned long	HSTSEL	:1;
		unsigned long			:1;
		unsigned long	PIT_M	:2;
		unsigned long			:2;
		unsigned long	YHSOVF	:1;
		unsigned long			:3;
		unsigned long	HSTSTS	:1;
		unsigned long			:19;
	}bit;
};

/*	structure of HSTH	(2841_C124h)	*/
union io_r2y_hsth{
	unsigned long		word;
	struct {
		unsigned long	HSTHSTA	:13;
		unsigned long			:3;
		unsigned long	HSTHSIZ	:13;
		unsigned long			:3;
	}bit;
};

/*	structure of HSTV	(2841_C128h)	*/
union io_r2y_hstv{
	unsigned long		word;
	struct {
		unsigned long	HSTVSTA	:14;
		unsigned long			:2;
		unsigned long	HSTVSIZ	:14;
		unsigned long			:2;
	}bit;
};

/*	structure of YYW0A	(2841_C200h)	*/
union io_r2y_yyw0a{
	unsigned long		word[3][4];
	struct {
		unsigned long	YYW0A_0_0	:32;
		unsigned long	YYW0A_0_1	:32;
		unsigned long	YYW0A_0_2	:32;
		unsigned long	YYW0A_0_3	:32;
		unsigned long	YYW0A_1_0	:32;
		unsigned long	YYW0A_1_1	:32;
		unsigned long	YYW0A_1_2	:32;
		unsigned long	YYW0A_1_3	:32;
		unsigned long	YYW0A_2_0	:32;
		unsigned long	YYW0A_2_1	:32;
		unsigned long	YYW0A_2_2	:32;
		unsigned long	YYW0A_2_3	:32;
	}bit;
};

/*	structure of YYW0DEF	(2841_C240h)	*/
union io_r2y_yyw0def{
	unsigned long		word[2];
	struct {
		long			YYW0DEF_0	:17;
		unsigned long				:15;
		long			YYW0DEF_1	:17;
		unsigned long				:15;
	}bit;
};

/*	structure of YYW0OFSX	(2841_C248h)	*/
union io_r2y_yyw0ofsx{
	unsigned long		word[2];
	struct {
		unsigned long	YYW0OFSX_0	:12;
		unsigned long				:4;
		unsigned long	YYW0OFSX_1	:12;
		unsigned long				:4;
		unsigned long	YYW0OFSX_2	:12;
		unsigned long				:4;
		unsigned long	YYW0OFSX_3	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of RSZ0HSTA	(2841_C258h)	*/
union io_r2y_rsz0hsta{
	unsigned long		word;
	struct {
		unsigned long	RSZ0HSTA	:20;
		unsigned long				:12;
	}bit;
};

/*	structure of RSZ0HPIT	(2841_C25Ch)	*/
union io_r2y_rsz0hpit{
	unsigned long		word;
	struct {
		unsigned long	RSZ0HPIT	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of RSZ0VSTA	(2841_C260h)	*/
union io_r2y_rsz0vsta{
	unsigned long		word;
	struct {
		unsigned long	RSZ0VSTA	:20;
		unsigned long				:12;
	}bit;
};

/*	structure of RSZ0VPIT	(2841_C264h)	*/
union io_r2y_rsz0vpit{
	unsigned long		word;
	struct {
		unsigned long	RSZ0VPIT	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of PCEMD	(2841_C268h)	*/
union io_r2y_pcemd{
	unsigned long		word;
	struct {
		unsigned long	PCEEN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of PCEXY	(2841_C26Ch)	*/
union io_r2y_pcexy{
	unsigned long		word;
	struct {
		long			PCEX	:9;
		unsigned long			:7;
		long			PCEY	:9;
		unsigned long			:7;
	}bit;
};

/*	structure of PCEC	(2841_C270h)	*/
union io_r2y_pcec{
	unsigned long		word;
	struct {
		unsigned long	PCECOF	:8;
		unsigned long			:8;
		unsigned long	PCECGA	:10;
		unsigned long			:6;
	}bit;
};

/*	structure of PCEH	(2841_C274h)	*/
union io_r2y_pceh{
	unsigned long		word;
	struct {
		unsigned long	PCEHOF	:8;
		unsigned long			:8;
		unsigned long	PCEHGA	:10;
		unsigned long			:6;
	}bit;
};

/*	structure of TRM0H	(2841_C278h)	*/
union io_r2y_trm0h{
	unsigned long		word;
	struct {
		unsigned long	TRM0HSTA	:13;
		unsigned long				:3;
		unsigned long	TRM0HSIZ	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of TRM0V	(2841_C27Ch)	*/
union io_r2y_trm0v{
	unsigned long		word;
	struct {
		unsigned long	TRM0VSTA	:14;
		unsigned long				:2;
		unsigned long	TRM0VSIZ	:14;
		unsigned long				:2;
	}bit;
};

/*	structure of TRMBH	(2841_C280h)	*/
union io_r2y_trmbh{
	unsigned long		word;
	struct {
		unsigned long	TRMBHSTA	:13;
		unsigned long				:3;
		unsigned long	TRMBHSIZ	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of TRMBV	(2841_C284h)	*/
union io_r2y_trmbv{
	unsigned long		word;
	struct {
		unsigned long	TRMBVSTA	:14;
		unsigned long				:2;
		unsigned long	TRMBVSIZ	:14;
		unsigned long				:2;
	}bit;
};

/*	structure of TRMCH	(2841_C288h)	*/
union io_r2y_trmch{
	unsigned long		word;
	struct {
		unsigned long	TRMCHSTA	:13;
		unsigned long				:3;
		unsigned long	TRMCHSIZ	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of TRMCV	(2841_C28Ch)	*/
union io_r2y_trmcv{
	unsigned long		word;
	struct {
		unsigned long	TRMCVSTA	:14;
		unsigned long				:2;
		unsigned long	TRMCVSIZ	:14;
		unsigned long				:2;
	}bit;
};

/*	structure of YYW0LTT	(2841_C290h)	*/
union io_r2y_yyw0ltt{
	unsigned long		word;
	struct {
		unsigned long	YYW0LTT	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of YYWAA	(2841_C2A0h)	*/
union io_r2y_yywaa{
	unsigned long		word[2][4];
	struct {
		unsigned long	YYWAA_0_0	:32;
		unsigned long	YYWAA_0_1	:32;
		unsigned long	YYWAA_0_2	:32;
		unsigned long	YYWAA_0_3	:32;
		unsigned long	YYWAA_1_0	:32;
		unsigned long	YYWAA_1_1	:32;
		unsigned long	YYWAA_1_2	:32;
		unsigned long	YYWAA_1_3	:32;
	}bit;
};

/*	structure of YYWADEF	(2841_C2C0h)	*/
union io_r2y_yywadef{
	unsigned long		word;
	struct {
		long			YYWADEF	:17;
		unsigned long			:15;
	}bit;
};

/*	structure of YYWAOFSX	(2841_C2C8h)	*/
union io_r2y_yywaofsx{
	unsigned long		word[2];
	struct {
		unsigned long	YYWAOFSX_0	:12;
		unsigned long				:4;
		unsigned long	YYWAOFSX_1	:12;
		unsigned long				:4;
		unsigned long	YYWAOFSX_2	:12;
		unsigned long				:4;
		unsigned long	YYWAOFSX_3	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of YYW1A	(2841_C300h)	*/
union io_r2y_yyw1a{
	unsigned long		word[3][4];
	struct {
		unsigned long	YYW1A_0_0	:32;
		unsigned long	YYW1A_0_1	:32;
		unsigned long	YYW1A_0_2	:32;
		unsigned long	YYW1A_0_3	:32;
		unsigned long	YYW1A_1_0	:32;
		unsigned long	YYW1A_1_1	:32;
		unsigned long	YYW1A_1_2	:32;
		unsigned long	YYW1A_1_3	:32;
		unsigned long	YYW1A_2_0	:32;
		unsigned long	YYW1A_2_1	:32;
		unsigned long	YYW1A_2_2	:32;
		unsigned long	YYW1A_2_3	:32;
	}bit;
};

/*	structure of YYW1DEF	(2841_C340h)	*/
union io_r2y_yyw1def{
	unsigned long		word[2];
	struct {
		long			YYW1DEF_0	:17;
		unsigned long				:15;
		long			YYW1DEF_1	:17;
		unsigned long				:15;
	}bit;
};

/*	structure of RSZ1HSTA	(2841_C348h)	*/
union io_r2y_rsz1hsta{
	unsigned long		word;
	struct {
		unsigned long	RSZ1HSTA	:20;
		unsigned long				:12;
	}bit;
};

/*	structure of RSZ1HPIT	(2841_C34Ch)	*/
union io_r2y_rsz1hpit{
	unsigned long		word;
	struct {
		unsigned long	RSZ1HPIT	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of RSZ1VSTA	(2841_C350h)	*/
union io_r2y_rsz1vsta{
	unsigned long		word;
	struct {
		unsigned long	RSZ1VSTA	:20;
		unsigned long				:12;
	}bit;
};

/*	structure of RSZ1VPIT	(2841_C354h)	*/
union io_r2y_rsz1vpit{
	unsigned long		word;
	struct {
		unsigned long	RSZ1VPIT	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of TRM1H	(2841_C358h)	*/
union io_r2y_trm1h{
	unsigned long		word;
	struct {
		unsigned long	TRM1HSTA	:13;
		unsigned long				:3;
		unsigned long	TRM1HSIZ	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of TRM1V	(2841_C35Ch)	*/
union io_r2y_trm1v{
	unsigned long		word;
	struct {
		unsigned long	TRM1VSTA	:14;
		unsigned long				:2;
		unsigned long	TRM1VSIZ	:14;
		unsigned long				:2;
	}bit;
};

/*	structure of YYW2A	(2841_C380h)	*/
union io_r2y_yyw2a{
	unsigned long		word[4];
	struct {
		unsigned long	YYW2A_0	:32;
		unsigned long	YYW2A_1	:32;
		unsigned long	YYW2A_2	:32;
		unsigned long	YYW2A_3	:32;
	}bit;
};

/*	structure of YYW2DEF	(2841_C390h)	*/
union io_r2y_yyw2def{
	unsigned long		word;
	struct {
		long			YYW2DEF	:17;
		unsigned long			:15;
	}bit;
};

/*	structure of RSZ2HSTA	(2841_C394h)	*/
union io_r2y_rsz2hsta{
	unsigned long		word;
	struct {
		unsigned long	RSZ2HSTA	:20;
		unsigned long				:12;
	}bit;
};

/*	structure of RSZ2HPIT	(2841_C398h)	*/
union io_r2y_rsz2hpit{
	unsigned long		word;
	struct {
		unsigned long	RSZ2HPIT	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of RSZ2VSTA	(2841_C39Ch)	*/
union io_r2y_rsz2vsta{
	unsigned long		word;
	struct {
		unsigned long	RSZ2VSTA	:20;
		unsigned long				:12;
	}bit;
};

/*	structure of RSZ2VPIT	(2841_C3A0h)	*/
union io_r2y_rsz2vpit{
	unsigned long		word;
	struct {
		unsigned long	RSZ2VPIT	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of TRM2H	(2841_C3A4h)	*/
union io_r2y_trm2h{
	unsigned long		word;
	struct {
		unsigned long	TRM2HSTA	:13;
		unsigned long				:3;
		unsigned long	TRM2HSIZ	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of TRM2V	(2841_C3A8h)	*/
union io_r2y_trm2v{
	unsigned long		word;
	struct {
		unsigned long	TRM2VSTA	:14;
		unsigned long				:2;
		unsigned long	TRM2VSIZ	:14;
		unsigned long				:2;
	}bit;
};

/*	structure of EE0CTL	(2841_C400h)	*/
union io_r2y_ee0ctl{
	unsigned long		word;
	struct {
		unsigned long	EE0EN	:1;
		unsigned long			:3;
		unsigned long	EE0TC	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of EE0HPFK	(2841_C408h)	*/
union io_r2y_ee0hpfk{
	unsigned long		word[2];
	struct {
		unsigned long	EE0HPFK0	:10;
		unsigned long				:6;
		long			EE0HPFK1	:8;
		long			EE0HPFK2	:8;
		long			EE0HPFK3	:8;
		long			EE0HPFK4	:8;
		long			EE0HPFK5	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of EE0NRLV	(2841_C410h)	*/
union io_r2y_ee0nrlv{
	unsigned long		word;
	struct {
		unsigned long	EE0NRLV	:4;
		unsigned long			:28;
	}bit;
};

/*	structure of EE0CORPOF	(2841_C414h)	*/
union io_r2y_ee0corpof{
	unsigned long		word;
	struct {
		unsigned long	EE0CORPOF_0	:8;
		unsigned long	EE0CORPOF_1	:8;
		unsigned long	EE0CORPOF_2	:8;
		unsigned long	EE0CORPOF_3	:8;
	}bit;
};

/*	structure of EE0CORPGA	(2841_C418h)	*/
union io_r2y_ee0corpga{
	unsigned long		word;
	struct {
		long			EE0CORPGA_0	:8;
		long			EE0CORPGA_1	:8;
		long			EE0CORPGA_2	:8;
		long			EE0CORPGA_3	:8;
	}bit;
};

/*	structure of EE0CORPBD	(2841_C41Ch)	*/
union io_r2y_ee0corpbd{
	unsigned long		word;
	struct {
		unsigned long	EE0CORPBD_1	:8;
		unsigned long	EE0CORPBD_2	:8;
		unsigned long	EE0CORPBD_3	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of EE0CORMOF	(2841_C420h)	*/
union io_r2y_ee0cormof{
	unsigned long		word;
	struct {
		unsigned long	EE0CORMOF_0	:8;
		unsigned long	EE0CORMOF_1	:8;
		unsigned long	EE0CORMOF_2	:8;
		unsigned long	EE0CORMOF_3	:8;
	}bit;
};

/*	structure of EE0CORMGA	(2841_C424h)	*/
union io_r2y_ee0cormga{
	unsigned long		word;
	struct {
		long			EE0CORMGA_0	:8;
		long			EE0CORMGA_1	:8;
		long			EE0CORMGA_2	:8;
		long			EE0CORMGA_3	:8;
	}bit;
};

/*	structure of EE0CORMBD	(2841_C428h)	*/
union io_r2y_ee0cormbd{
	unsigned long		word;
	struct {
		unsigned long	EE0CORMBD_1	:8;
		unsigned long	EE0CORMBD_2	:8;
		unsigned long	EE0CORMBD_3	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of EE0SCLSUP	(2841_C42Ch)	*/
union io_r2y_ee0sclsup{
	unsigned long		word;
	struct {
		unsigned long	EE0SCLSUP	:6;
		unsigned long				:26;
	}bit;
};

/*	structure of EE0SCLPOF	(2841_C430h)	*/
union io_r2y_ee0sclpof{
	unsigned long		word[3];
	struct {
		unsigned long	EE0SCLPOF_0	:8;
		unsigned long	EE0SCLPOF_1	:8;
		unsigned long	EE0SCLPOF_2	:8;
		unsigned long	EE0SCLPOF_3	:8;
		unsigned long	EE0SCLPOF_4	:8;
		unsigned long	EE0SCLPOF_5	:8;
		unsigned long	EE0SCLPOF_6	:8;
		unsigned long	EE0SCLPOF_7	:8;
		unsigned long	EE0SCLPOF_8	:8;
		unsigned long	EE0SCLPOF_9	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of EE0SCLPGA	(2841_C440h)	*/
union io_r2y_ee0sclpga{
	unsigned long		word[5];
	struct {
		long			EE0SCLPGA_0	:13;
		unsigned long				:3;
		long			EE0SCLPGA_1	:13;
		unsigned long				:3;
		long			EE0SCLPGA_2	:13;
		unsigned long				:3;
		long			EE0SCLPGA_3	:13;
		unsigned long				:3;
		long			EE0SCLPGA_4	:13;
		unsigned long				:3;
		long			EE0SCLPGA_5	:13;
		unsigned long				:3;
		long			EE0SCLPGA_6	:13;
		unsigned long				:3;
		long			EE0SCLPGA_7	:13;
		unsigned long				:3;
		long			EE0SCLPGA_8	:13;
		unsigned long				:3;
		long			EE0SCLPGA_9	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of EE0SCLPBD	(2841_C460h)	*/
union io_r2y_ee0sclpbd{
	unsigned long		word[3];
	struct {
		unsigned long	EE0SCLPBD_1	:8;
		unsigned long	EE0SCLPBD_2	:8;
		unsigned long	EE0SCLPBD_3	:8;
		unsigned long	EE0SCLPBD_4	:8;
		unsigned long	EE0SCLPBD_5	:8;
		unsigned long	EE0SCLPBD_6	:8;
		unsigned long	EE0SCLPBD_7	:8;
		unsigned long	EE0SCLPBD_8	:8;
		unsigned long	EE0SCLPBD_9	:8;
		unsigned long				:24;
	}bit;
};

/*	structure of EE0SCLMOF	(2841_C470h)	*/
union io_r2y_ee0sclmof{
	unsigned long		word[3];
	struct {
		unsigned long	EE0SCLMOF_0	:8;
		unsigned long	EE0SCLMOF_1	:8;
		unsigned long	EE0SCLMOF_2	:8;
		unsigned long	EE0SCLMOF_3	:8;
		unsigned long	EE0SCLMOF_4	:8;
		unsigned long	EE0SCLMOF_5	:8;
		unsigned long	EE0SCLMOF_6	:8;
		unsigned long	EE0SCLMOF_7	:8;
		unsigned long	EE0SCLMOF_8	:8;
		unsigned long	EE0SCLMOF_9	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of EE0SCLMGA	(2841_C480h)	*/
union io_r2y_ee0sclmga{
	unsigned long		word[5];
	struct {
		long			EE0SCLMGA_0	:13;
		unsigned long				:3;
		long			EE0SCLMGA_1	:13;
		unsigned long				:3;
		long			EE0SCLMGA_2	:13;
		unsigned long				:3;
		long			EE0SCLMGA_3	:13;
		unsigned long				:3;
		long			EE0SCLMGA_4	:13;
		unsigned long				:3;
		long			EE0SCLMGA_5	:13;
		unsigned long				:3;
		long			EE0SCLMGA_6	:13;
		unsigned long				:3;
		long			EE0SCLMGA_7	:13;
		unsigned long				:3;
		long			EE0SCLMGA_8	:13;
		unsigned long				:3;
		long			EE0SCLMGA_9	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of EE0SCLMBD	(2841_C4A0h)	*/
union io_r2y_ee0sclmbd{
	unsigned long		word[3];
	struct {
		unsigned long	EE0SCLMBD_1	:8;
		unsigned long	EE0SCLMBD_2	:8;
		unsigned long	EE0SCLMBD_3	:8;
		unsigned long	EE0SCLMBD_4	:8;
		unsigned long	EE0SCLMBD_5	:8;
		unsigned long	EE0SCLMBD_6	:8;
		unsigned long	EE0SCLMBD_7	:8;
		unsigned long	EE0SCLMBD_8	:8;
		unsigned long	EE0SCLMBD_9	:8;
		unsigned long				:24;
	}bit;
};

/*	structure of EE0TONPOF	(2841_C4B0h)	*/
union io_r2y_ee0tonpof{
	unsigned long		word[2];
	struct {
		unsigned long	EE0TONPOF_0	:7;
		unsigned long				:1;
		unsigned long	EE0TONPOF_1	:7;
		unsigned long				:1;
		unsigned long	EE0TONPOF_2	:7;
		unsigned long				:1;
		unsigned long	EE0TONPOF_3	:7;
		unsigned long				:1;
		unsigned long	EE0TONPOF_4	:7;
		unsigned long				:25;
	}bit;
};

/*	structure of EE0TONPGA	(2841_C4C0h)	*/
union io_r2y_ee0tonpga{
	unsigned long		word[3];
	struct {
		long			EE0TONPGA_0	:9;
		unsigned long				:7;
		long			EE0TONPGA_1	:9;
		unsigned long				:7;
		long			EE0TONPGA_2	:9;
		unsigned long				:7;
		long			EE0TONPGA_3	:9;
		unsigned long				:7;
		long			EE0TONPGA_4	:9;
		unsigned long				:23;
	}bit;
};

/*	structure of EE0TONPBD	(2841_C4D0h)	*/
union io_r2y_ee0tonpbd{
	unsigned long		word;
	struct {
		unsigned long	EE0TONPBD_1	:7;
		unsigned long				:1;
		unsigned long	EE0TONPBD_2	:7;
		unsigned long				:1;
		unsigned long	EE0TONPBD_3	:7;
		unsigned long				:1;
		unsigned long	EE0TONPBD_4	:7;
		unsigned long				:1;
	}bit;
};

/*	structure of EE0TONMOF	(2841_C4D8h)	*/
union io_r2y_ee0tonmof{
	unsigned long		word[2];
	struct {
		unsigned long	EE0TONMOF_0	:7;
		unsigned long				:1;
		unsigned long	EE0TONMOF_1	:7;
		unsigned long				:1;
		unsigned long	EE0TONMOF_2	:7;
		unsigned long				:1;
		unsigned long	EE0TONMOF_3	:7;
		unsigned long				:1;
		unsigned long	EE0TONMOF_4	:7;
		unsigned long				:25;
	}bit;
};

/*	structure of EE0TONMGA	(2841_C4E0h)	*/
union io_r2y_ee0tonmga{
	unsigned long		word[3];
	struct {
		long			EE0TONMGA_0	:9;
		unsigned long				:7;
		long			EE0TONMGA_1	:9;
		unsigned long				:7;
		long			EE0TONMGA_2	:9;
		unsigned long				:7;
		long			EE0TONMGA_3	:9;
		unsigned long				:7;
		long			EE0TONMGA_4	:9;
		unsigned long				:23;
	}bit;
};

/*	structure of EE0TONMBD	(2841_C4F0h)	*/
union io_r2y_ee0tonmbd{
	unsigned long		word;
	struct {
		unsigned long	EE0TONMBD_1	:7;
		unsigned long				:1;
		unsigned long	EE0TONMBD_2	:7;
		unsigned long				:1;
		unsigned long	EE0TONMBD_3	:7;
		unsigned long				:1;
		unsigned long	EE0TONMBD_4	:7;
		unsigned long				:1;
	}bit;
};

/*	structure of EE0CLPPOF	(2841_C4F4h)	*/
union io_r2y_ee0clppof{
	unsigned long		word;
	struct {
		unsigned long	EE0CLPPOF_0	:7;
		unsigned long				:1;
		unsigned long	EE0CLPPOF_1	:7;
		unsigned long				:1;
		unsigned long	EE0CLPPOF_2	:7;
		unsigned long				:9;
	}bit;
};

/*	structure of EE0CLPPGA	(2841_C4F8h)	*/
union io_r2y_ee0clppga{
	unsigned long		word;
	struct {
		long			EE0CLPPGA_0	:8;
		long			EE0CLPPGA_1	:8;
		long			EE0CLPPGA_2	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of EE0CLPPBD	(2841_C4FCh)	*/
union io_r2y_ee0clppbd{
	unsigned long		word;
	struct {
		unsigned long	EE0CLPPBD_1	:8;
		unsigned long	EE0CLPPBD_2	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of EE0CLPMOF	(2841_C500h)	*/
union io_r2y_ee0clpmof{
	unsigned long		word;
	struct {
		unsigned long	EE0CLPMOF_0	:7;
		unsigned long				:1;
		unsigned long	EE0CLPMOF_1	:7;
		unsigned long				:1;
		unsigned long	EE0CLPMOF_2	:7;
		unsigned long				:9;
	}bit;
};

/*	structure of EE0CLPMGA	(2841_C504h)	*/
union io_r2y_ee0clpmga{
	unsigned long		word;
	struct {
		long			EE0CLPMGA_0	:8;
		long			EE0CLPMGA_1	:8;
		long			EE0CLPMGA_2	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of EE0CLPMBD	(2841_C508h)	*/
union io_r2y_ee0clpmbd{
	unsigned long		word;
	struct {
		unsigned long	EE0CLPMBD_1	:8;
		unsigned long	EE0CLPMBD_2	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of EE1CTL	(2841_C600h)	*/
union io_r2y_ee1ctl{
	unsigned long		word;
	struct {
		unsigned long	EE1EN	:1;
		unsigned long			:3;
		unsigned long	EE1TC	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of EE1HPFK	(2841_C608h)	*/
union io_r2y_ee1hpfk{
	unsigned long		word[2];
	struct {
		unsigned long	EE1HPFK0	:10;
		unsigned long				:6;
		long			EE1HPFK1	:8;
		long			EE1HPFK2	:8;
		long			EE1HPFK3	:8;
		long			EE1HPFK4	:8;
		long			EE1HPFK5	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of EE1NRLV	(2841_C610h)	*/
union io_r2y_ee1nrlv{
	unsigned long		word;
	struct {
		unsigned long	EE1NRLV	:4;
		unsigned long			:28;
	}bit;
};

/*	structure of EE1CORPOF	(2841_C618h)	*/
union io_r2y_ee1corpof{
	unsigned long		word[2];
	struct {
		unsigned long	EE1CORPOF_0	:10;
		unsigned long				:6;
		unsigned long	EE1CORPOF_1	:10;
		unsigned long				:6;
		unsigned long	EE1CORPOF_2	:10;
		unsigned long				:6;
		unsigned long	EE1CORPOF_3	:10;
		unsigned long				:6;
	}bit;
};

/*	structure of EE1CORPGA	(2841_C620h)	*/
union io_r2y_ee1corpga{
	unsigned long		word;
	struct {
		long			EE1CORPGA_0	:8;
		long			EE1CORPGA_1	:8;
		long			EE1CORPGA_2	:8;
		long			EE1CORPGA_3	:8;
	}bit;
};

/*	structure of EE1CORPBD	(2841_C628h)	*/
union io_r2y_ee1corpbd{
	unsigned long		word[2];
	struct {
		unsigned long	EE1CORPBD_1	:10;
		unsigned long				:6;
		unsigned long	EE1CORPBD_2	:10;
		unsigned long				:6;
		unsigned long	EE1CORPBD_3	:10;
		unsigned long				:22;
	}bit;
};

/*	structure of EE1CORMOF	(2841_C630h)	*/
union io_r2y_ee1cormof{
	unsigned long		word[2];
	struct {
		unsigned long	EE1CORMOF_0	:10;
		unsigned long				:6;
		unsigned long	EE1CORMOF_1	:10;
		unsigned long				:6;
		unsigned long	EE1CORMOF_2	:10;
		unsigned long				:6;
		unsigned long	EE1CORMOF_3	:10;
		unsigned long				:6;
	}bit;
};

/*	structure of EE1CORMGA	(2841_C638h)	*/
union io_r2y_ee1cormga{
	unsigned long		word;
	struct {
		long			EE1CORMGA_0	:8;
		long			EE1CORMGA_1	:8;
		long			EE1CORMGA_2	:8;
		long			EE1CORMGA_3	:8;
	}bit;
};

/*	structure of EE1CORMBD	(2841_C640h)	*/
union io_r2y_ee1cormbd{
	unsigned long		word[2];
	struct {
		unsigned long	EE1CORMBD_1	:10;
		unsigned long				:6;
		unsigned long	EE1CORMBD_2	:10;
		unsigned long				:6;
		unsigned long	EE1CORMBD_3	:10;
		unsigned long				:22;
	}bit;
};

/*	structure of EE1SCLSUP	(2841_C648h)	*/
union io_r2y_ee1sclsup{
	unsigned long		word;
	struct {
		unsigned long	EE1SCLSUP	:6;
		unsigned long				:26;
	}bit;
};

/*	structure of EE1SCLPOF	(2841_C650h)	*/
union io_r2y_ee1sclpof{
	unsigned long		word[3];
	struct {
		unsigned long	EE1SCLPOF_0	:8;
		unsigned long	EE1SCLPOF_1	:8;
		unsigned long	EE1SCLPOF_2	:8;
		unsigned long	EE1SCLPOF_3	:8;
		unsigned long	EE1SCLPOF_4	:8;
		unsigned long	EE1SCLPOF_5	:8;
		unsigned long	EE1SCLPOF_6	:8;
		unsigned long	EE1SCLPOF_7	:8;
		unsigned long	EE1SCLPOF_8	:8;
		unsigned long	EE1SCLPOF_9	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of EE1SCLPGA	(2841_C660h)	*/
union io_r2y_ee1sclpga{
	unsigned long		word[5];
	struct {
		long			EE1SCLPGA_0	:13;
		unsigned long				:3;
		long			EE1SCLPGA_1	:13;
		unsigned long				:3;
		long			EE1SCLPGA_2	:13;
		unsigned long				:3;
		long			EE1SCLPGA_3	:13;
		unsigned long				:3;
		long			EE1SCLPGA_4	:13;
		unsigned long				:3;
		long			EE1SCLPGA_5	:13;
		unsigned long				:3;
		long			EE1SCLPGA_6	:13;
		unsigned long				:3;
		long			EE1SCLPGA_7	:13;
		unsigned long				:3;
		long			EE1SCLPGA_8	:13;
		unsigned long				:3;
		long			EE1SCLPGA_9	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of EE1SCLPBD	(2841_C680h)	*/
union io_r2y_ee1sclpbd{
	unsigned long		word[5];
	struct {
		unsigned long	EE1SCLPBD_1	:10;
		unsigned long				:6;
		unsigned long	EE1SCLPBD_2	:10;
		unsigned long				:6;
		unsigned long	EE1SCLPBD_3	:10;
		unsigned long				:6;
		unsigned long	EE1SCLPBD_4	:10;
		unsigned long				:6;
		unsigned long	EE1SCLPBD_5	:10;
		unsigned long				:6;
		unsigned long	EE1SCLPBD_6	:10;
		unsigned long				:6;
		unsigned long	EE1SCLPBD_7	:10;
		unsigned long				:6;
		unsigned long	EE1SCLPBD_8	:10;
		unsigned long				:6;
		unsigned long	EE1SCLPBD_9	:10;
		unsigned long				:22;
	}bit;
};

/*	structure of EE1SCLMOF	(2841_C6A0h)	*/
union io_r2y_ee1sclmof{
	unsigned long		word[3];
	struct {
		unsigned long	EE1SCLMOF_0	:8;
		unsigned long	EE1SCLMOF_1	:8;
		unsigned long	EE1SCLMOF_2	:8;
		unsigned long	EE1SCLMOF_3	:8;
		unsigned long	EE1SCLMOF_4	:8;
		unsigned long	EE1SCLMOF_5	:8;
		unsigned long	EE1SCLMOF_6	:8;
		unsigned long	EE1SCLMOF_7	:8;
		unsigned long	EE1SCLMOF_8	:8;
		unsigned long	EE1SCLMOF_9	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of EE1SCLMGA	(2841_C6C0h)	*/
union io_r2y_ee1sclmga{
	unsigned long		word[5];
	struct {
		long			EE1SCLMGA_0	:13;
		unsigned long				:3;
		long			EE1SCLMGA_1	:13;
		unsigned long				:3;
		long			EE1SCLMGA_2	:13;
		unsigned long				:3;
		long			EE1SCLMGA_3	:13;
		unsigned long				:3;
		long			EE1SCLMGA_4	:13;
		unsigned long				:3;
		long			EE1SCLMGA_5	:13;
		unsigned long				:3;
		long			EE1SCLMGA_6	:13;
		unsigned long				:3;
		long			EE1SCLMGA_7	:13;
		unsigned long				:3;
		long			EE1SCLMGA_8	:13;
		unsigned long				:3;
		long			EE1SCLMGA_9	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of EE1SCLMBD	(2841_C6E0h)	*/
union io_r2y_ee1sclmbd{
	unsigned long		word[5];
	struct {
		unsigned long	EE1SCLMBD_1	:10;
		unsigned long				:6;
		unsigned long	EE1SCLMBD_2	:10;
		unsigned long				:6;
		unsigned long	EE1SCLMBD_3	:10;
		unsigned long				:6;
		unsigned long	EE1SCLMBD_4	:10;
		unsigned long				:6;
		unsigned long	EE1SCLMBD_5	:10;
		unsigned long				:6;
		unsigned long	EE1SCLMBD_6	:10;
		unsigned long				:6;
		unsigned long	EE1SCLMBD_7	:10;
		unsigned long				:6;
		unsigned long	EE1SCLMBD_8	:10;
		unsigned long				:6;
		unsigned long	EE1SCLMBD_9	:10;
		unsigned long				:22;
	}bit;
};

/*	structure of EE1TONPOF	(2841_C700h)	*/
union io_r2y_ee1tonpof{
	unsigned long		word[3];
	struct {
		unsigned long	EE1TONPOF_0	:9;
		unsigned long				:7;
		unsigned long	EE1TONPOF_1	:9;
		unsigned long				:7;
		unsigned long	EE1TONPOF_2	:9;
		unsigned long				:7;
		unsigned long	EE1TONPOF_3	:9;
		unsigned long				:7;
		unsigned long	EE1TONPOF_4	:9;
		unsigned long				:23;
	}bit;
};

/*	structure of EE1TONPGA	(2841_C710h)	*/
union io_r2y_ee1tonpga{
	unsigned long		word[3];
	struct {
		long			EE1TONPGA_0	:9;
		unsigned long				:7;
		long			EE1TONPGA_1	:9;
		unsigned long				:7;
		long			EE1TONPGA_2	:9;
		unsigned long				:7;
		long			EE1TONPGA_3	:9;
		unsigned long				:7;
		long			EE1TONPGA_4	:9;
		unsigned long				:23;
	}bit;
};

/*	structure of EE1TONPBD	(2841_C720h)	*/
union io_r2y_ee1tonpbd{
	unsigned long		word[2];
	struct {
		unsigned long	EE1TONPBD_1	:9;
		unsigned long				:7;
		unsigned long	EE1TONPBD_2	:9;
		unsigned long				:7;
		unsigned long	EE1TONPBD_3	:9;
		unsigned long				:7;
		unsigned long	EE1TONPBD_4	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of EE1TONMOF	(2841_C730h)	*/
union io_r2y_ee1tonmof{
	unsigned long		word[3];
	struct {
		unsigned long	EE1TONMOF_0	:9;
		unsigned long				:7;
		unsigned long	EE1TONMOF_1	:9;
		unsigned long				:7;
		unsigned long	EE1TONMOF_2	:9;
		unsigned long				:7;
		unsigned long	EE1TONMOF_3	:9;
		unsigned long				:7;
		unsigned long	EE1TONMOF_4	:9;
		unsigned long				:23;
	}bit;
};

/*	structure of EE1TONMGA	(2841_C740h)	*/
union io_r2y_ee1tonmga{
	unsigned long		word[3];
	struct {
		long			EE1TONMGA_0	:9;
		unsigned long				:7;
		long			EE1TONMGA_1	:9;
		unsigned long				:7;
		long			EE1TONMGA_2	:9;
		unsigned long				:7;
		long			EE1TONMGA_3	:9;
		unsigned long				:7;
		long			EE1TONMGA_4	:9;
		unsigned long				:23;
	}bit;
};

/*	structure of EE1TONMBD	(2841_C750h)	*/
union io_r2y_ee1tonmbd{
	unsigned long		word[2];
	struct {
		unsigned long	EE1TONMBD_1	:9;
		unsigned long				:7;
		unsigned long	EE1TONMBD_2	:9;
		unsigned long				:7;
		unsigned long	EE1TONMBD_3	:9;
		unsigned long				:7;
		unsigned long	EE1TONMBD_4	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of EE1CLPPOF	(2841_C758h)	*/
union io_r2y_ee1clppof{
	unsigned long		word[2];
	struct {
		unsigned long	EE1CLPPOF_0	:9;
		unsigned long				:7;
		unsigned long	EE1CLPPOF_1	:9;
		unsigned long				:7;
		unsigned long	EE1CLPPOF_2	:9;
		unsigned long				:23;
	}bit;
};

/*	structure of EE1CLPPGA	(2841_C760h)	*/
union io_r2y_ee1clppga{
	unsigned long		word;
	struct {
		long			EE1CLPPGA_0	:8;
		long			EE1CLPPGA_1	:8;
		long			EE1CLPPGA_2	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of EE1CLPPBD	(2841_C764h)	*/
union io_r2y_ee1clppbd{
	unsigned long		word;
	struct {
		unsigned long	EE1CLPPBD_1	:10;
		unsigned long				:6;
		unsigned long	EE1CLPPBD_2	:10;
		unsigned long				:6;
	}bit;
};

/*	structure of EE1CLPMOF	(2841_C768h)	*/
union io_r2y_ee1clpmof{
	unsigned long		word[2];
	struct {
		unsigned long	EE1CLPMOF_0	:9;
		unsigned long				:7;
		unsigned long	EE1CLPMOF_1	:9;
		unsigned long				:7;
		unsigned long	EE1CLPMOF_2	:9;
		unsigned long				:23;
	}bit;
};

/*	structure of EE1CLPMGA	(2841_C770h)	*/
union io_r2y_ee1clpmga{
	unsigned long		word;
	struct {
		long			EE1CLPMGA_0	:8;
		long			EE1CLPMGA_1	:8;
		long			EE1CLPMGA_2	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of EE1CLPMBD	(2841_C774h)	*/
union io_r2y_ee1clpmbd{
	unsigned long		word;
	struct {
		unsigned long	EE1CLPMBD_1	:10;
		unsigned long				:6;
		unsigned long	EE1CLPMBD_2	:10;
		unsigned long				:6;
	}bit;
};

/*  structure of YYCH area	2841_(C000 - C07Fh) */
struct io_r2y_yych {
	union  io_r2y_yyraxctl		YYRAXCTL;		/* 2841_(C000 - C003h) */
	union  io_r2y_yyw0axctl		YYW0AXCTL;		/* 2841_(C004 - C007h) */
	union  io_r2y_yywaaxctl		YYWAAXCTL;		/* 2841_(C008 - C00Bh) */
	union  io_r2y_yyw1axctl		YYW1AXCTL;		/* 2841_(C00C - C00Fh) */
	union  io_r2y_yyw2axctl		YYW2AXCTL;		/* 2841_(C010 - C013h) */
	union  io_r2y_yyraxsts		YYRAXSTS;		/* 2841_(C014 - C017h) */
	union  io_r2y_yyw0axsts		YYW0AXSTS;		/* 2841_(C018 - C01Bh) */
	union  io_r2y_yywaaxsts		YYWAAXSTS;		/* 2841_(C01C - C01Fh) */
	union  io_r2y_yyw1axsts		YYW1AXSTS;		/* 2841_(C020 - C023h) */
	union  io_r2y_yyw2axsts		YYW2AXSTS;		/* 2841_(C024 - C027h) */
	union  io_r2y_yyraxmd		YYRAXMD;		/* 2841_(C028 - C02Bh) */
	union  io_r2y_yyw0axmd		YYW0AXMD;		/* 2841_(C02C - C02Fh) */
	union  io_r2y_yywaaxmd		YYWAAXMD;		/* 2841_(C030 - C033h) */
	union  io_r2y_yyw1axmd		YYW1AXMD;		/* 2841_(C034 - C037h) */
	union  io_r2y_yyw2axmd		YYW2AXMD;		/* 2841_(C038 - C03Bh) */

	unsigned char dmy_C03C_C03F[0xC040-0xC03C];	/* 2841_(C03C - C03Fh) */

	union  io_r2y_yyrreqmsk		YYRREQMSK;		/* 2841_(C040 - C047h) */
	union  io_r2y_yyw0reqmsk	YYW0REQMSK;		/* 2841_(C048 - C04Fh) */
	union  io_r2y_yywareqmsk	YYWAREQMSK;		/* 2841_(C050 - C053h) */

	unsigned char dmy_C054_C057[0xC058-0xC054];	/* 2841_(C054 - C057h) */

	union  io_r2y_yyw1reqmsk	YYW1REQMSK;		/* 2841_(C058 - C05Fh) */
	union  io_r2y_yyw2reqmsk	YYW2REQMSK;		/* 2841_(C060 - C063h) */

	unsigned char dmy_C064_C07F[0xC080-0xC064];	/* 2841_(C064 - C07Fh) */
};

/*  structure of R2Y COMMON area	2841_(C080 - C0BFh) */
struct io_r2y_cmn {
	union  io_r2y_yyinte		YYINTE;			/* 2841_(C080 - C083h) */
	union  io_r2y_yyintf		YYINTF;			/* 2841_(C084 - C087h) */
	union  io_r2y_lintlv		LINTLV;			/* 2841_(C088 - C08Fh) */
	union  io_r2y_lintsel		LINTSEL;		/* 2841_(C090 - C093h) */
	union  io_r2y_r2ymode		R2YMODE;		/* 2841_(C094 - C097h) */
	union  io_r2y_ramaen		RAMAEN;			/* 2841_(C098 - C09Bh) */

	unsigned char dmy_C09C_C0BF[0xC0C0-0xC09C];	/* 2841_(C09C - C0BFh) */
};

/*  structure of YYR area	2841_(C0C0 - C0FFh) */
struct io_r2y_yyr {
	union  io_r2y_yyrtrg		YYRTRG;			/* 2841_(C0C0 - C0C3h) */
	union  io_r2y_yyrctl		YYRCTL;			/* 2841_(C0C4 - C0C7h) */
	union  io_r2y_dinctl		DINCTL;			/* 2841_(C0C8 - C0CBh) */

	unsigned char dmy_C0CC_C0CF[0xC0D0-0xC0CC];	/* 2841_(C0CC - C0CFh) */

	union  io_r2y_yyra			YYRA;			/* 2841_(C0D0 - C0DBh) */

	unsigned char dmy_C0DC_C0DF[0xC0E0-0xC0DC];	/* 2841_(C0DC - C0DFh) */

	union  io_r2y_yyrofs		YYROFS;			/* 2841_(C0E0 - C0E3h) */
	union  io_r2y_yyrdef		YYRDEF;			/* 2841_(C0E4 - C0E7h) */
	union  io_r2y_yyrhsiz		YYRHSIZ;		/* 2841_(C0E8 - C0EBh) */
	union  io_r2y_yyrvsiz		YYRVSIZ;		/* 2841_(C0EC - C0EFh) */

	unsigned char dmy_C0F0_C0FF[0xC100-0xC0F0];	/* 2841_(C0F0 - C0FFh) */
};

/*  structure of YYW area	2841_(C100 - FFFFh) */
struct io_r2y_yyw {
	union  io_r2y_yywtrg		YYWTRG;			/* 2841_(C100 - C103h) */
	union  io_r2y_yywctl		YYWCTL;			/* 2841_(C104 - C107h) */
	union  io_r2y_yywbank		YYWBANK;		/* 2841_(C108 - C10Bh) */
	union  io_r2y_yywkpp		YYWKPP;			/* 2841_(C10C - C10Fh) */
	union  io_r2y_yyw0md		YYW0MD;			/* 2841_(C110 - C113h) */
	union  io_r2y_yyw1md		YYW1MD;			/* 2841_(C114 - C117h) */
	union  io_r2y_yyw2md		YYW2MD;			/* 2841_(C118 - C11Bh) */

	unsigned char dmy_C11C_C11F[0xC120-0xC11C];	/* 2841_(C11C - C11Fh) */

	union  io_r2y_hstmd			HSTMD;			/* 2841_(C120 - C123h) */
	union  io_r2y_hsth			HSTH;			/* 2841_(C124 - C127h) */
	union  io_r2y_hstv			HSTV;			/* 2841_(C128 - C12Bh) */

	unsigned char dmy_C12C_C1FF[0xC200-0xC12C];	/* 2841_(C12C - C1FFh) */

	union  io_r2y_yyw0a			YYW0A;			/* 2841_(C200 - C22Fh) */

	unsigned char dmy_C230_C23F[0xC240-0xC230];	/* 2841_(C230 - C23Fh) */

	union  io_r2y_yyw0def		YYW0DEF;		/* 2841_(C240 - C247h) */
	union  io_r2y_yyw0ofsx		YYW0OFSX;		/* 2841_(C248 - C24Fh) */

	unsigned char dmy_C250_C257[0xC258-0xC250];	/* 2841_(C250 - C257h) */

	union  io_r2y_rsz0hsta		RSZ0HSTA;		/* 2841_(C258 - C25Bh) */
	union  io_r2y_rsz0hpit		RSZ0HPIT;		/* 2841_(C25C - C25Fh) */
	union  io_r2y_rsz0vsta		RSZ0VSTA;		/* 2841_(C260 - C263h) */
	union  io_r2y_rsz0vpit		RSZ0VPIT;		/* 2841_(C264 - C267h) */
	union  io_r2y_pcemd			PCEMD;			/* 2841_(C268 - C26Bh) */
	union  io_r2y_pcexy			PCEXY;			/* 2841_(C26C - C26Fh) */
	union  io_r2y_pcec			PCEC;			/* 2841_(C270 - C273h) */
	union  io_r2y_pceh			PCEH;			/* 2841_(C274 - C277h) */
	union  io_r2y_trm0h			TRM0H;			/* 2841_(C278 - C27Bh) */
	union  io_r2y_trm0v			TRM0V;			/* 2841_(C27C - C27Fh) */
	union  io_r2y_trmbh			TRMBH;			/* 2841_(C280 - C283h) */
	union  io_r2y_trmbv			TRMBV;			/* 2841_(C284 - C287h) */
	union  io_r2y_trmch			TRMCH;			/* 2841_(C288 - C28Bh) */
	union  io_r2y_trmcv			TRMCV;			/* 2841_(C28C - C28Fh) */
	union  io_r2y_yyw0ltt		YYW0LTT;		/* 2841_(C290 - C293h) */

	unsigned char dmy_C294_C29F[0xC2A0-0xC294];	/* 2841_(C294 - C29Fh) */

	union  io_r2y_yywaa			YYWAA;			/* 2841_(C2A0 - C2BFh) */
	union  io_r2y_yywadef		YYWADEF;		/* 2841_(C2C0 - C2C3h) */

	unsigned char dmy_C2C4_C2C7[0xC2C8-0xC2C4];	/* 2841_(C2C4 - C2C7h) */

	union  io_r2y_yywaofsx		YYWAOFSX;		/* 2841_(C2C8 - C2CFh) */

	unsigned char dmy_C2D0_C2FF[0xC300-0xC2D0];	/* 2841_(C2D0 - C2FFh) */

	union  io_r2y_yyw1a			YYW1A;			/* 2841_(C300 - C32Fh) */

	unsigned char dmy_C330_C33F[0xC340-0xC330];	/* 2841_(C330 - C33Fh) */

	union  io_r2y_yyw1def		YYW1DEF;		/* 2841_(C340 - C347h) */
	union  io_r2y_rsz1hsta		RSZ1HSTA;		/* 2841_(C348 - C34Bh) */
	union  io_r2y_rsz1hpit		RSZ1HPIT;		/* 2841_(C34C - C34Fh) */
	union  io_r2y_rsz1vsta		RSZ1VSTA;		/* 2841_(C350 - C353h) */
	union  io_r2y_rsz1vpit		RSZ1VPIT;		/* 2841_(C354 - C357h) */
	union  io_r2y_trm1h			TRM1H;			/* 2841_(C358 - C35Bh) */
	union  io_r2y_trm1v			TRM1V;			/* 2841_(C35C - C35Fh) */

	unsigned char dmy_C360_C37F[0xC380-0xC360];	/* 2841_(C360 - C37Fh) */

	union  io_r2y_yyw2a			YYW2A;			/* 2841_(C380 - C38Fh) */
	union  io_r2y_yyw2def		YYW2DEF;		/* 2841_(C390 - C393h) */
	union  io_r2y_rsz2hsta		RSZ2HSTA;		/* 2841_(C394 - C397h) */
	union  io_r2y_rsz2hpit		RSZ2HPIT;		/* 2841_(C398 - C39Bh) */
	union  io_r2y_rsz2vsta		RSZ2VSTA;		/* 2841_(C39C - C39Fh) */
	union  io_r2y_rsz2vpit		RSZ2VPIT;		/* 2841_(C3A0 - C3A3h) */
	union  io_r2y_trm2h			TRM2H;			/* 2841_(C3A4 - C3A7h) */
	union  io_r2y_trm2v			TRM2V;			/* 2841_(C3A8 - C3ABh) */

	unsigned char dmy_C3AC_C3FF[0xC400-0xC3AC];	/* 2841_(C3AC - C3FFh) */

	union  io_r2y_ee0ctl		EE0CTL;			/* 2841_(C400 - C403h) */

	unsigned char dmy_C404_C407[0xC408-0xC404];	/* 2841_(C404 - C407h) */

	union  io_r2y_ee0hpfk		EE0HPFK;		/* 2841_(C408 - C40Fh) */
	union  io_r2y_ee0nrlv		EE0NRLV;		/* 2841_(C410 - C413h) */
	union  io_r2y_ee0corpof		EE0CORPOF;		/* 2841_(C414 - C417h) */
	union  io_r2y_ee0corpga		EE0CORPGA;		/* 2841_(C418 - C41Bh) */
	union  io_r2y_ee0corpbd		EE0CORPBD;		/* 2841_(C41C - C41Fh) */
	union  io_r2y_ee0cormof		EE0CORMOF;		/* 2841_(C420 - C423h) */
	union  io_r2y_ee0cormga		EE0CORMGA;		/* 2841_(C424 - C427h) */
	union  io_r2y_ee0cormbd		EE0CORMBD;		/* 2841_(C428 - C42Bh) */
	union  io_r2y_ee0sclsup		EE0SCLSUP;		/* 2841_(C42C - C42Fh) */
	union  io_r2y_ee0sclpof		EE0SCLPOF;		/* 2841_(C430 - C43Bh) */

	unsigned char dmy_C43C_C43F[0xC440-0xC43C];	/* 2841_(C43C - C43Fh) */

	union  io_r2y_ee0sclpga		EE0SCLPGA;		/* 2841_(C440 - C453h) */

	unsigned char dmy_C454_C45F[0xC460-0xC454];	/* 2841_(C454 - C45Fh) */

	union  io_r2y_ee0sclpbd		EE0SCLPBD;		/* 2841_(C460 - C46Bh) */

	unsigned char dmy_C46C_C46F[0xC470-0xC46C];	/* 2841_(C46C - C46Fh) */

	union  io_r2y_ee0sclmof		EE0SCLMOF;		/* 2841_(C470 - C47Bh) */

	unsigned char dmy_C47C_C47F[0xC480-0xC47C];	/* 2841_(C47C - C47Fh) */

	union  io_r2y_ee0sclmga		EE0SCLMGA;		/* 2841_(C480 - C493h) */

	unsigned char dmy_C494_C49F[0xC4A0-0xC494];	/* 2841_(C494 - C49Fh) */

	union  io_r2y_ee0sclmbd		EE0SCLMBD;		/* 2841_(C4A0 - C4ABh) */

	unsigned char dmy_C4AC_C4AF[0xC4B0-0xC4AC];	/* 2841_(C4AC - C4AFh) */

	union  io_r2y_ee0tonpof		EE0TONPOF;		/* 2841_(C4B0 - C4B7h) */

	unsigned char dmy_C4B8_C4BF[0xC4C0-0xC4B8];	/* 2841_(C4B8 - C4BFh) */

	union  io_r2y_ee0tonpga		EE0TONPGA;		/* 2841_(C4C0 - C4CBh) */

	unsigned char dmy_C4CC_C4CF[0xC4D0-0xC4CC];	/* 2841_(C4CC - C4CFh) */

	union  io_r2y_ee0tonpbd		EE0TONPBD;		/* 2841_(C4D0 - C4D3h) */

	unsigned char dmy_C4D4_C4D7[0xC4D8-0xC4D4];	/* 2841_(C4D4 - C4D7h) */

	union  io_r2y_ee0tonmof		EE0TONMOF;		/* 2841_(C4D8 - C4DFh) */
	union  io_r2y_ee0tonmga		EE0TONMGA;		/* 2841_(C4E0 - C4EBh) */

	unsigned char dmy_C4EC_C4EF[0xC4F0-0xC4EC];	/* 2841_(C4EC - C4EFh) */

	union  io_r2y_ee0tonmbd		EE0TONMBD;		/* 2841_(C4F0 - C4F3h) */
	union  io_r2y_ee0clppof		EE0CLPPOF;		/* 2841_(C4F4 - C4F7h) */
	union  io_r2y_ee0clppga		EE0CLPPGA;		/* 2841_(C4F8 - C4FBh) */
	union  io_r2y_ee0clppbd		EE0CLPPBD;		/* 2841_(C4FC - C4FFh) */
	union  io_r2y_ee0clpmof		EE0CLPMOF;		/* 2841_(C500 - C503h) */
	union  io_r2y_ee0clpmga		EE0CLPMGA;		/* 2841_(C504 - C507h) */
	union  io_r2y_ee0clpmbd		EE0CLPMBD;		/* 2841_(C508 - C50Bh) */

	unsigned char dmy_C50C_C5FF[0xC600-0xC50C];	/* 2841_(C50C - C5FFh) */

	union  io_r2y_ee1ctl		EE1CTL;			/* 2841_(C600 - C603h) */

	unsigned char dmy_C604_C607[0xC608-0xC604];	/* 2841_(C604 - C607h) */

	union  io_r2y_ee1hpfk		EE1HPFK;		/* 2841_(C608 - C60Fh) */
	union  io_r2y_ee1nrlv		EE1NRLV;		/* 2841_(C610 - C613h) */

	unsigned char dmy_C614_C617[0xC618-0xC614];	/* 2841_(C614 - C617h) */

	union  io_r2y_ee1corpof		EE1CORPOF;		/* 2841_(C618 - C61Fh) */
	union  io_r2y_ee1corpga		EE1CORPGA;		/* 2841_(C620 - C623h) */

	unsigned char dmy_C624_C627[0xC628-0xC624];	/* 2841_(C624 - C627h) */

	union  io_r2y_ee1corpbd		EE1CORPBD;		/* 2841_(C628 - C62Fh) */
	union  io_r2y_ee1cormof		EE1CORMOF;		/* 2841_(C630 - C637h) */
	union  io_r2y_ee1cormga		EE1CORMGA;		/* 2841_(C638 - C63Bh) */

	unsigned char dmy_C63C_C63F[0xC640-0xC63C];	/* 2841_(C63C - C63Fh) */

	union  io_r2y_ee1cormbd		EE1CORMBD;		/* 2841_(C640 - C647h) */
	union  io_r2y_ee1sclsup		EE1SCLSUP;		/* 2841_(C648 - C64Bh) */

	unsigned char dmy_C64C_C64F[0xC650-0xC64C];	/* 2841_(C64C - C64Fh) */

	union  io_r2y_ee1sclpof		EE1SCLPOF;		/* 2841_(C650 - C65Bh) */

	unsigned char dmy_C65C_C65F[0xC660-0xC65C];	/* 2841_(C65C - C65Fh) */

	union  io_r2y_ee1sclpga		EE1SCLPGA;		/* 2841_(C660 - C673h) */

	unsigned char dmy_C674_C67F[0xC680-0xC674];	/* 2841_(C674 - C67Fh) */

	union  io_r2y_ee1sclpbd		EE1SCLPBD;		/* 2841_(C680 - C693h) */

	unsigned char dmy_C694_C69F[0xC6A0-0xC694];	/* 2841_(C694 - C69Fh) */

	union  io_r2y_ee1sclmof		EE1SCLMOF;		/* 2841_(C6A0 - C6ABh) */

	unsigned char dmy_C6AC_C6BF[0xC6C0-0xC6AC];	/* 2841_(C6AC - C6BFh) */

	union  io_r2y_ee1sclmga		EE1SCLMGA;		/* 2841_(C6C0 - C6D3h) */

	unsigned char dmy_C6D4_C6DF[0xC6E0-0xC6D4];	/* 2841_(C6D4 - C6DFh) */

	union  io_r2y_ee1sclmbd		EE1SCLMBD;		/* 2841_(C6E0 - C6F3h) */

	unsigned char dmy_C6F4_C6FF[0xC700-0xC6F4];	/* 2841_(C6F4 - C6FFh) */

	union  io_r2y_ee1tonpof		EE1TONPOF;		/* 2841_(C700 - C70Bh) */

	unsigned char dmy_C70C_C70F[0xC710-0xC70C];	/* 2841_(C70C - C70Fh) */

	union  io_r2y_ee1tonpga		EE1TONPGA;		/* 2841_(C710 - C71Bh) */

	unsigned char dmy_C71C_C71F[0xC720-0xC71C];	/* 2841_(C71C - C71Fh) */

	union  io_r2y_ee1tonpbd		EE1TONPBD;		/* 2841_(C720 - C727h) */

	unsigned char dmy_C728_C72F[0xC730-0xC728];	/* 2841_(C728 - C72Fh) */

	union  io_r2y_ee1tonmof		EE1TONMOF;		/* 2841_(C730 - C73Bh) */

	unsigned char dmy_C73C_C73F[0xC740-0xC73C];	/* 2841_(C73C - C73Fh) */

	union  io_r2y_ee1tonmga		EE1TONMGA;		/* 2841_(C740 - C74Bh) */

	unsigned char dmy_C74C_C74F[0xC750-0xC74C];	/* 2841_(C74C - C74Fh) */

	union  io_r2y_ee1tonmbd		EE1TONMBD;		/* 2841_(C750 - C757h) */
	union  io_r2y_ee1clppof		EE1CLPPOF;		/* 2841_(C758 - C75Fh) */
	union  io_r2y_ee1clppga		EE1CLPPGA;		/* 2841_(C760 - C763h) */
	union  io_r2y_ee1clppbd		EE1CLPPBD;		/* 2841_(C764 - C767h) */
	union  io_r2y_ee1clpmof		EE1CLPMOF;		/* 2841_(C768 - C76Fh) */
	union  io_r2y_ee1clpmga		EE1CLPMGA;		/* 2841_(C770 - C773h) */
	union  io_r2y_ee1clpmbd		EE1CLPMBD;		/* 2841_(C774 - C777h) */

	unsigned char dmy_C778_FFFF[0x10000-0xC778];	/* 2841_(C778 - FFFFh) */
};

/* Define i/o mapping */
struct io_r2y{
	/* JDSR2Y */
	struct io_r2y_fr2y			F_R2Y;			/* 2841_(8000 - BFFFh) */
	struct io_r2y_yych			YYCH;			/* 2841_(C000 - C07Fh) */
	struct io_r2y_cmn			R2Y_CMN;		/* 2841_(C080 - C0BFh) */
	struct io_r2y_yyr			YYR;			/* 2841_(C0C0 - C0FFh) */
	struct io_r2y_yyw			YYW;			/* 2841_(C100 - FFFFh) */
};

extern volatile struct io_r2y		IO_R2Y_P1;
extern volatile struct io_r2y		IO_R2Y_P2;
extern volatile struct io_r2y		IO_R2Y_P3;

