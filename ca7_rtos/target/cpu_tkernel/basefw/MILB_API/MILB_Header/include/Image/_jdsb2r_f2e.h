/**
 * @file		_jdsb2r_f2e.h
 * @brief		Definition JDSB2R_F2E Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "__fb2r6a.h"
#include "__jdsb2r_f2e_sram.h"

/*	structure of YBRAXCTL	(2841_2200h)	*/
union io_b2r_ybraxctl{
	unsigned long		word;
	struct {
		unsigned long	YBRCACHE	:4;
		unsigned long	YBRPROT		:3;
		unsigned long				:25;
	}bit;
};

/*	structure of YBWAXCTL	(2841_2204h)	*/
union io_b2r_ybwaxctl{
	unsigned long		word;
	struct {
		unsigned long	YBWCACHE_0	:4;
		unsigned long	YBWPROT_0	:3;
		unsigned long				:1;
		unsigned long	YBWCACHE_1	:4;
		unsigned long	YBWPROT_1	:3;
		unsigned long				:1;
		unsigned long	YBWCACHE_2	:4;
		unsigned long	YBWPROT_2	:3;
		unsigned long				:9;
	}bit;
};

/*	structure of YBRAXSTS	(2841_2208h)	*/
union io_b2r_ybraxsts{
	unsigned long		word;
	struct {
		unsigned long	YBRRESP	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of YBWAXSTS	(2841_220Ch)	*/
union io_b2r_ybwaxsts{
	unsigned long		word;
	struct {
		unsigned long	YBBRESP_0	:2;
		unsigned long				:6;
		unsigned long	YBBRESP_1	:2;
		unsigned long				:6;
		unsigned long	YBBRESP_2	:2;
		unsigned long				:14;
	}bit;
};

/*	structure of YBRAXMD	(2841_2210h)	*/
union io_b2r_ybraxmd{
	unsigned long		word;
	struct {
		unsigned long	YBRBL	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of YBWAXMD	(2841_2214h)	*/
union io_b2r_ybwaxmd{
	unsigned long		word;
	struct {
		unsigned long	YBWBL	:2;
		unsigned long			:6;
		unsigned long	YBWDOS	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of YBRREQMSK	(2841_2218h)	*/
union io_b2r_ybrreqmsk{
	unsigned long		word;
	struct {
		unsigned long	YBRREQMSK	:12;
		unsigned long				:20;
	}bit;
};

/*	structure of YBWREQMSK	(2841_2220h)	*/
union io_b2r_ybwreqmsk{
	unsigned long		word[2];
	struct {
		unsigned long	YBWREQMSK_0	:12;
		unsigned long				:4;
		unsigned long	YBWREQMSK_1	:12;
		unsigned long				:4;
		unsigned long	YBWREQMSK_2	:12;
		unsigned long				:20;
	}bit;
};

/*	structure of YBINTE	(2841_2240h)	*/
union io_b2r_ybinte{
	unsigned long		word;
	struct {
		unsigned long	YBREE	:1;
		unsigned long	YBWEE	:1;
		unsigned long			:2;
		unsigned long	FB2REE	:1;
		unsigned long	ELNEE	:1;
		unsigned long			:2;
		unsigned long	YBRERE	:1;
		unsigned long	YBWERE	:1;
		unsigned long			:2;
		unsigned long	AXRERE	:1;
		unsigned long	AXWERE	:1;
		unsigned long			:18;
	}bit;
};

/*	structure of YBINTF	(2841_2244h)	*/
union io_b2r_ybintf{
	unsigned long		word;
	struct {
		unsigned long	__YBREF		:1;
		unsigned long	__YBWEF		:1;
		unsigned long				:2;
		unsigned long	__FB2REF	:1;
		unsigned long	__ELNEF		:1;
		unsigned long				:2;
		unsigned long	__YBRERF	:1;
		unsigned long	__YBWERF	:1;
		unsigned long				:2;
		unsigned long	__AXRERF	:1;
		unsigned long	__AXWERF	:1;
		unsigned long				:18;
	}bit;
};

/*	structure of DINSEL	(2841_2248h)	*/
union io_b2r_dinsel{
	unsigned long		word;
	struct {
		unsigned long	B2BDIN	:1;
		unsigned long			:3;
		unsigned long	EXTOEN	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of B2RMODE	(2841_224Ch)	*/
union io_b2r_b2rmode{
	unsigned long		word;
	struct {
		unsigned long	CIABYP	:1;
		unsigned long	CIAPDD	:1;
		unsigned long			:30;
	}bit;
};

/*	structure of ELNLV	(2841_2250h)	*/
union io_b2r_elnlv{
	unsigned long		word;
	struct {
		unsigned long	ELNLV	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of RAMAEN	(2841_2254h)	*/
union io_b2r_ramaen{
	unsigned long		word;
	struct {
		unsigned long	RAMAEN_BAY_DEKNEE	:1;
		unsigned long	RAMAEN_YBR			:1;
		unsigned long	RAMAEN_RGB_KNEE		:1;
		unsigned long	RAMAEN_YBW			:1;
		unsigned long						:28;
	}bit;
};

/*	structure of EXALMSL	(2841_2258h)	*/
union io_b2r_exalmsl{
	unsigned long		word;
	struct {
		unsigned long	EXALMSL				:2;
		unsigned long						:30;
	}bit;
};

/*	structure of YBRTRG	(2841_2280h)	*/
union io_b2r_ybrtrg{
	unsigned long		word;
	struct {
		unsigned long	YBRTRG	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of YBRCTL	(2841_2284h)	*/
union io_b2r_ybrctl{
	unsigned long		word;
	struct {
		unsigned long	YBRDTYP	:2;
		unsigned long			:2;
		unsigned long	PIXEL	:2;
		unsigned long			:2;
		unsigned long	DKNEN	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of YBRA	(2841_2288h)	*/
union io_b2r_ybra{
	unsigned long		word;
	struct {
		unsigned long	YBRA	:32;
	}bit;
};

/*	structure of YBROFS	(2841_228Ch)	*/
union io_b2r_ybrofs{
	unsigned long		word;
	struct {
		unsigned long	YBROFS	:3;
		unsigned long			:29;
	}bit;
};

/*	structure of YBRDEF	(2841_2290h)	*/
union io_b2r_ybrdef{
	unsigned long		word;
	struct {
		unsigned long	YBRDEF	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of YBRHSIZ	(2841_2294h)	*/
union io_b2r_ybrhsiz{
	unsigned long		word;
	struct {
		unsigned long	YBRHSIZ	:13;
		unsigned long			:19;
	}bit;
};

/*	structure of YBRVSIZ	(2841_2298h)	*/
union io_b2r_ybrvsiz{
	unsigned long		word;
	struct {
		unsigned long	YBRVSIZ	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of YBRHBLNK	(2841_229Ch)	*/
union io_b2r_ybrhblnk{
	unsigned long		word;
	struct {
		unsigned long	YBRHBLNK	:10;
		unsigned long				:22;
	}bit;
};

/*	structure of DINCTL	(2841_22A0h)	*/
union io_b2r_dinctl{
	unsigned long		word;
	struct {
		unsigned long	FRMSTP	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of DINHSTA	(2841_22A4h)	*/
union io_b2r_dinhsta{
	unsigned long		word;
	struct {
		unsigned long	DINHSTA	:13;
		unsigned long			:19;
	}bit;
};

/*	structure of DINVSTA	(2841_22A8h)	*/
union io_b2r_dinvsta{
	unsigned long		word;
	struct {
		unsigned long	DINVSTA	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of DINHSIZ	(2841_22ACh)	*/
union io_b2r_dinhsiz{
	unsigned long		word;
	struct {
		unsigned long	DINHSIZ	:13;
		unsigned long			:19;
	}bit;
};

/*	structure of DINVSIZ	(2841_22B0h)	*/
union io_b2r_dinvsiz{
	unsigned long		word;
	struct {
		unsigned long	DINVSIZ	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of DINELS	(2841_22B4h)	*/
union io_b2r_dinels{
	unsigned long		word;
	struct {
		unsigned long	DINELS	:13;
		unsigned long			:19;
	}bit;
};

/*	structure of DINERR	(2841_22BCh)	*/
union io_b2r_dinerr{
	unsigned long		word;
	struct {
		unsigned long	TRMER	:1;
		unsigned long	HPDDER	:1;
		unsigned long	WAITER	:1;
		unsigned long			:29;
	}bit;
};

/*	structure of YBWTRG	(2841_2300h)	*/
union io_b2r_ybwtrg{
	unsigned long		word;
	struct {
		unsigned long	YBWTRG	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of YBWCTL	(2841_2304h)	*/
union io_b2r_ybwctl{
	unsigned long		word;
	struct {
		unsigned long	YBWDTYP	:2;
		unsigned long			:2;
		unsigned long	YBWCSE	:1;
		unsigned long			:3;
		unsigned long	USEBANK	:2;
		unsigned long			:22;
	}bit;
};

/*	structure of YBWMD	(2841_2308h)	*/
union io_b2r_ybwmd{
	unsigned long		word;
	struct {
		unsigned long	TRMEN	:1;
		unsigned long			:3;
		unsigned long	KNEEN	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of YBWBANK	(2841_230Ch)	*/
union io_b2r_ybwbank{
	unsigned long		word;
	struct {
		unsigned long	BANKST	:1;
		unsigned long			:3;
		unsigned long	BANKPS	:2;
		unsigned long			:2;
		unsigned long	BANKSTS	:2;
		unsigned long			:22;
	}bit;
};

/*	structure of YBWA	(2841_2340h)	*/
union io_b2r_ybwa{
	unsigned long		word[3][4];
	struct {
		unsigned long	YBWA_0_0	:32;
		unsigned long	YBWA_0_1	:32;
		unsigned long	YBWA_0_2	:32;
		unsigned long	YBWA_0_3	:32;
		unsigned long	YBWA_1_0	:32;
		unsigned long	YBWA_1_1	:32;
		unsigned long	YBWA_1_2	:32;
		unsigned long	YBWA_1_3	:32;
		unsigned long	YBWA_2_0	:32;
		unsigned long	YBWA_2_1	:32;
		unsigned long	YBWA_2_2	:32;
		unsigned long	YBWA_2_3	:32;
	}bit;
};

/*	structure of YBWDEF	(2841_2380h)	*/
union io_b2r_ybwdef{
	unsigned long		word;
	struct {
		long			YBWDEF	:17;
		unsigned long			:15;
	}bit;
};

/*	structure of YBWTRMH	(2841_2384h)	*/
union io_b2r_ybwtrmh{
	unsigned long		word;
	struct {
		unsigned long	TRMHSTA	:13;
		unsigned long			:3;
		unsigned long	TRMHSIZ	:13;
		unsigned long			:3;
	}bit;
};

/*	structure of YBWTRMV	(2841_2388h)	*/
union io_b2r_ybwtrmv{
	unsigned long		word;
	struct {
		unsigned long	TRMVSTA	:14;
		unsigned long			:2;
		unsigned long	TRMVSIZ	:14;
		unsigned long			:2;
	}bit;
};

/*	structure of YBCH area	2841_(2200 - 223Fh) */
struct io_b2r_ybch {
	union	io_b2r_ybraxctl		YBRAXCTL;			/* 2841_(2200 - 2203h) */
	union	io_b2r_ybwaxctl		YBWAXCTL;			/* 2841_(2204 - 2207h) */
	union	io_b2r_ybraxsts		YBRAXSTS;			/* 2841_(2208 - 220Bh) */
	union	io_b2r_ybwaxsts		YBWAXSTS;			/* 2841_(220C - 220Fh) */
	union	io_b2r_ybraxmd		YBRAXMD;			/* 2841_(2210 - 2213h) */
	union	io_b2r_ybwaxmd		YBWAXMD;			/* 2841_(2214 - 2217h) */
	union	io_b2r_ybrreqmsk	YBRREQMSK;			/* 2841_(2218 - 221Bh) */

	unsigned char dmy_221C_221F[0x2220-0x221C];		/* 2841_(221C - 221Fh) */

	union	io_b2r_ybwreqmsk	YBWREQMSK;			/* 2841_(2220 - 2227h) */

	unsigned char dmy_2228_223F[0x2240-0x2228];		/* 2841_(2228 - 223Fh) */
};

/*	structure of B2R COMMON area	2841_(2240 - 227Fh) */
struct io_b2r_cmn {
	union	io_b2r_ybinte		YBINTE;				/* 2841_(2240 - 2243h) */
	union	io_b2r_ybintf		YBINTF;				/* 2841_(2244 - 2247h) */
	union	io_b2r_dinsel		DINSEL;				/* 2841_(2248 - 224Bh) */
	union	io_b2r_b2rmode		B2RMODE;			/* 2841_(224C - 224Fh) */
	union	io_b2r_elnlv		ELNLV;				/* 2841_(2250 - 2253h) */
	union	io_b2r_ramaen		RAMAEN;				/* 2841_(2254 - 2257h) */
	union	io_b2r_exalmsl		EXALMSL;			/* 2841_(2258 - 225Bh) */

	unsigned char dmy_225C_227F[0x2280-0x225C];		/* 2841_(225C - 227Fh) */
};

/*	structure of YBR area	2841_(2280 - 22FFh) */
struct io_b2r_ybr {
	union	io_b2r_ybrtrg		YBRTRG;				/* 2841_(2280 - 2283h) */
	union	io_b2r_ybrctl		YBRCTL;				/* 2841_(2284 - 2287h) */
	union	io_b2r_ybra			YBRA;				/* 2841_(2288 - 228Bh) */
	union	io_b2r_ybrofs		YBROFS;				/* 2841_(228C - 228Fh) */
	union	io_b2r_ybrdef		YBRDEF;				/* 2841_(2290 - 2293h) */
	union	io_b2r_ybrhsiz		YBRHSIZ;			/* 2841_(2294 - 2297h) */
	union	io_b2r_ybrvsiz		YBRVSIZ;			/* 2841_(2298 - 229Bh) */
	union	io_b2r_ybrhblnk		YBRHBLNK;			/* 2841_(229C - 229Fh) */
	union	io_b2r_dinctl		DINCTL;				/* 2841_(22A0 - 22A3h) */
	union	io_b2r_dinhsta		DINHSTA;			/* 2841_(22A4 - 22A7h) */
	union	io_b2r_dinvsta		DINVSTA;			/* 2841_(22A8 - 22ABh) */
	union	io_b2r_dinhsiz		DINHSIZ;			/* 2841_(22AC - 22AFh) */
	union	io_b2r_dinvsiz		DINVSIZ;			/* 2841_(22B0 - 22B3h) */
	union	io_b2r_dinels		DINELS;				/* 2841_(22B4 - 22B7h) */
	unsigned char dmy_22B8_22BB[0x22BC-0x22B8];		/* 2841_(22B8 - 22BBh) */
	union	io_b2r_dinerr		DINERR;				/* 2841_(22BC - 22BFh) */

	unsigned char dmy_22C0_22FF[0x2300-0x22C0];		/* 2841_(22C0 - 22FFh) */
};

/*	structure of YBW area	2841_(2300 - 23FFh) */
struct io_b2r_ybw {
	union	io_b2r_ybwtrg		YBWTRG;				/* 2841_(2300 - 2303h) */
	union	io_b2r_ybwctl		YBWCTL;				/* 2841_(2304 - 2307h) */
	union	io_b2r_ybwmd		YBWMD;				/* 2841_(2308 - 230Bh) */
	union	io_b2r_ybwbank		YBWBANK;			/* 2841_(230C - 230Fh) */

	unsigned char dmy_2310_233F[0x2340-0x2310];		/* 2841_(2310 - 233Fh) */

	union	io_b2r_ybwa			YBWA;				/* 2841_(2340 - 236Fh) */

	unsigned char dmy_2370_237F[0x2380-0x2370];		/* 2841_(2370 - 237Fh) */

	union	io_b2r_ybwdef		YBWDEF;				/* 2841_(2380 - 2383h) */
	union	io_b2r_ybwtrmh		YBWTRMH;			/* 2841_(2384 - 2387h) */
	union	io_b2r_ybwtrmv		YBWTRMV;			/* 2841_(2388 - 238Bh) */

	unsigned char dmy_238C_23FF[0x2400-0x238C];		/* 2841_(238C - 23FFh) */
};

/* Define i/o mapping */
struct io_b2r{
	/* JDSB2R */
	struct io_b2r_fb2r			F_B2R;				/* 2841_(2000 - 21FFh) */
	struct io_b2r_ybch			YBCH;				/* 2841_(2200 - 223Fh) */
	struct io_b2r_cmn			B2R_CMN;			/* 2841_(2240 - 227Fh) */
	struct io_b2r_ybr			YBR;				/* 2841_(2280 - 22FFh) */
	struct io_b2r_ybw			YBW;				/* 2841_(2300 - 23FFh) */

};

extern volatile struct io_b2r		IO_B2R_P1;
extern volatile struct io_b2r		IO_B2R_P2;
extern volatile struct io_b2r		IO_B2R_P3;

