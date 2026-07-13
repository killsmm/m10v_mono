/**
 * @file		__fb2r6a.h
 * @brief		Definition FB2R6A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*	structure of CNTL	(2841_2000h)	*/
union io_b2r_cntl{
	unsigned long		word;
	struct {
		unsigned long		:8;
		unsigned long	TRG	:2;
		unsigned long		:22;
	}bit;
};

/*	structure of B2RINTE	(2841_2004h)	*/
union io_b2r_b2rinte{
	unsigned long		word;
	struct {
		unsigned long	CPERE	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of B2RINTF	(2841_2008h)	*/
union io_b2r_b2rintf{
	unsigned long		word;
	struct {
		unsigned long	__CPERF	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of FLTAEN	(2841_200Ch)	*/
union io_b2r_fltaen{
	unsigned long		word;
	struct {
		unsigned long	CIAEN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of OFSTRB	(2841_2040h)	*/
union io_b2r_ofstrb{
	unsigned long		word;
	struct {
		long			OFSTRR	:10;
		unsigned long			:6;
		long			OFSTBB	:10;
		unsigned long			:6;
	}bit;
};

/*	structure of OFSTG	(2841_2044h)	*/
union io_b2r_ofstg{
	unsigned long		word;
	struct {
		long			OFSTGR	:10;
		unsigned long			:6;
		long			OFSTGB	:10;
		unsigned long			:6;
	}bit;
};

/*	structure of BLSFT	(2841_2048h)	*/
union io_b2r_blsft{
	unsigned long		word;
	struct {
		unsigned long	BLSFT	:2;
		unsigned long			:2;
		unsigned long	BLSFMD	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of WBGARB	(2841_204Ch)	*/
union io_b2r_wbgarb{
	unsigned long		word;
	struct {
		unsigned long	WBGARR	:11;
		unsigned long			:5;
		unsigned long	WBGABB	:11;
		unsigned long			:5;
	}bit;
};

/*	structure of WBGAGR	(2841_2050h)	*/
union io_b2r_wbgagr{
	unsigned long		word;
	struct {
		unsigned long	WBGAGR	:11;
		unsigned long			:5;
		unsigned long	WBGAGB	:11;
		unsigned long			:5;
	}bit;
};

/*	structure of WBHCTL	(2841_2054h)	*/
union io_b2r_wbhctl{
	unsigned long		word;
	struct {
		unsigned long	WBHCEN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of WBSLP	(2841_2058h)	*/
union io_b2r_wbslp{
	unsigned long		word[2];
	struct {
		unsigned long	WBSLPRR	:12;
		unsigned long			:4;
		unsigned long	WBSLPBB	:12;
		unsigned long			:4;
		unsigned long	WBSLPGR	:12;
		unsigned long			:4;
		unsigned long	WBSLPGB	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of WBLVRB	(2841_2060h)	*/
union io_b2r_wblvrb{
	unsigned long		word;
	struct {
		unsigned long	WBLVRR	:12;
		unsigned long			:4;
		unsigned long	WBLVBB	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of WBLVGR	(2841_2064h)	*/
union io_b2r_wblvgr{
	unsigned long		word;
	struct {
		unsigned long	WBLVGR	:12;
		unsigned long			:4;
		unsigned long	WBLVGB	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of SHARP	(2841_2080h)	*/
union io_b2r_sharp{
	unsigned long		word;
	struct {
		unsigned long	SHARPA	:4;
		unsigned long	SHARPB	:4;
		unsigned long	SHARPC	:4;
		unsigned long			:20;
	}bit;
};

/*	structure of AIRESY	(2841_2084h)	*/
union io_b2r_airesy{
	unsigned long		word;
	struct {
		unsigned long	AIRESYA	:4;
		unsigned long			:4;
		unsigned long	AIRESYB	:8;
		unsigned long			:16;
	}bit;
};

/*	structure of LF	(2841_2088h)	*/
union io_b2r_lf{
	unsigned long		word;
	struct {
		unsigned long	LFSTR	:5;
		unsigned long			:11;
		unsigned long	LFTH	:10;
		unsigned long			:6;
	}bit;
};

/*	structure of CMC	(2841_208Ch)	*/
union io_b2r_cmc{
	unsigned long		word;
	struct {
		unsigned long	CMCSCL	:3;
		unsigned long			:13;
		unsigned long	CMCLMT	:10;
		unsigned long			:6;
	}bit;
};

/*	structure of DIRRLITH	(2841_2090h)	*/
union io_b2r_dirrlith{
	unsigned long		word;
	struct {
		unsigned long	DIRRLITH	:12;
		unsigned long				:20;
	}bit;
};

/*	structure of HFACTL	(2841_2100h)	*/
union io_b2r_hfactl{
	unsigned long		word;
	struct {
		unsigned long	HFEEN	:1;
		unsigned long			:3;
		unsigned long	HFAFEN	:1;
		unsigned long			:3;
		unsigned long	HFAYEN	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of HFK	(2841_2110h)	*/
union io_b2r_hfk{
	unsigned long		word[4];
	struct {
		unsigned long	HFK0	:10;
		unsigned long			:6;
		long			HFK1	:10;
		unsigned long			:6;
		long			HFK2	:12;
		unsigned long			:4;
		long			HFK3	:8;
		unsigned long			:8;
		long			HFK4	:10;
		unsigned long			:6;
		long			HFK5	:10;
		unsigned long			:6;
		long			HFK6	:8;
		long			HFK7	:8;
		long			HFK8	:6;
		unsigned long			:2;
		long			HFK9	:6;
		unsigned long			:2;
	}bit;
};

/*	structure of HFEFCLP	(2841_2120h)	*/
union io_b2r_hfefclp{
	unsigned long		word;
	struct {
		unsigned long	HFEFCLPP	:9;
		unsigned long				:7;
		unsigned long	HFEFCLPM	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of HFAFST	(2841_2124h)	*/
union io_b2r_hfafst{
	unsigned long		word;
	struct {
		unsigned long	HFAFSL	:4;
		unsigned long			:12;
		unsigned long	HFAFTH	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of HFAFGU	(2841_2128h)	*/
union io_b2r_hfafgu{
	unsigned long		word;
	struct {
		unsigned long	HFAFGU	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of HFAFCLP	(2841_212Ch)	*/
union io_b2r_hfafclp{
	unsigned long		word;
	struct {
		unsigned long	HFAFCLPP	:9;
		unsigned long				:7;
		unsigned long	HFAFCLPM	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of HFAYC	(2841_2130h)	*/
union io_b2r_hfayc{
	unsigned long		word[4];
	struct {
		unsigned long	HFAYC00	:8;
		unsigned long	HFAYC01	:8;
		unsigned long	HFAYC02	:8;
		unsigned long			:8;
		long			HFAYC10	:9;
		unsigned long			:7;
		long			HFAYC11	:9;
		unsigned long			:7;
		long			HFAYC12	:9;
		unsigned long			:7;
		long			HFAYC20	:9;
		unsigned long			:7;
		long			HFAYC21	:9;
		unsigned long			:7;
		long			HFAYC22	:9;
		unsigned long			:7;
	}bit;
};

/*	structure of HFAYOF	(2841_2140h)	*/
union io_b2r_hfayof{
	unsigned long		word[2];
	struct {
		unsigned long	HFAYOF_0	:12;
		unsigned long				:4;
		unsigned long	HFAYOF_1	:12;
		unsigned long				:4;
		unsigned long	HFAYOF_2	:12;
		unsigned long				:4;
		unsigned long	HFAYOF_3	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of HFAYGA	(2841_2148h)	*/
union io_b2r_hfayga{
	unsigned long		word;
	struct {
		long			HFAYGA_0	:8;
		long			HFAYGA_1	:8;
		long			HFAYGA_2	:8;
		long			HFAYGA_3	:8;
	}bit;
};

/*	structure of HFAYBD	(2841_2150h)	*/
union io_b2r_hfaybd{
	unsigned long		word[2];
	struct {
		unsigned long	HFAYBD_0	:12;
		unsigned long				:4;
		unsigned long	HFAYBD_1	:12;
		unsigned long				:4;
		unsigned long	HFAYBD_2	:12;
		unsigned long				:4;
		unsigned long	HFAYBD_3	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of HFAYDIV	(2841_2158h)	*/
union io_b2r_hfaydiv{
	unsigned long		word;
	struct {
		unsigned long	HFAYDIV1	:3;
		unsigned long				:1;
		unsigned long	HFAYDIV2	:4;
		unsigned long				:24;
	}bit;
};

/*	structure of HFAYCLP	(2841_215Ch)	*/
union io_b2r_hfayclp{
	unsigned long		word;
	struct {
		unsigned long	HFAYCLPP	:9;
		unsigned long				:7;
		unsigned long	HFAYCLPM	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of HFACOR	(2841_2160h)	*/
union io_b2r_hfacor{
	unsigned long		word;
	struct {
		unsigned long	HFACORP	:6;
		unsigned long			:2;
		unsigned long	HFACORM	:6;
		unsigned long			:18;
	}bit;
};

/*	structure of HFACLP	(2841_2168h)	*/
union io_b2r_hfaclp{
	unsigned long		word[2];
	struct {
		unsigned long	HFACLPR	:12;
		unsigned long			:4;
		unsigned long	HFACLPG	:12;
		unsigned long			:4;
		unsigned long	HFACLPB	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of HFACRVCTL	(2841_2170h)	*/
union io_b2r_hfacrvctl{
	unsigned long		word;
	struct {
		unsigned long	HFACAE		:1;
		unsigned long				:3;
		unsigned long	HFACBE		:1;
		unsigned long				:3;
		unsigned long	HFACRVMRG	:2;
		unsigned long				:22;
	}bit;
};

/*	structure of HFACVAFX	(2841_2174h)	*/
union io_b2r_hfacvafx{
	unsigned long		word;
	struct {
		long			HFACRVAFX1	:8;
		long			HFACRVAFY1	:8;
		long			HFACRVAFX2	:8;
		long			HFACRVAFY2	:8;
	}bit;
};

/*	structure of HFACRVASCLOF	(2841_2178h)	*/
union io_b2r_hfacrvasclof{
	unsigned long		word;
	struct {
		unsigned long	HFACRVASCLOF1	:8;
		unsigned long					:24;
	}bit;
};

/*	structure of HFACRVASCLGA	(2841_217Ch)	*/
union io_b2r_hfacrvasclga{
	unsigned long		word;
	struct {
		long			HFACRVASCLGA_0	:11;
		unsigned long					:5;
		long			HFACRVASCLGA_1	:11;
		unsigned long					:5;
	}bit;
};

/*	structure of HFACRVASCLBD	(2841_2180h)	*/
union io_b2r_hfacrvasclbd{
	unsigned long		word;
	struct {
		unsigned long	HFACRVASCLBD1	:7;
		unsigned long					:25;
	}bit;
};

/*	structure of HFACRVASCLCP	(2841_2184h)	*/
union io_b2r_hfacrvasclcp{
	unsigned long		word;
	struct {
		unsigned long	HFACRVASCLCPL	:8;
		unsigned long	HFACRVASCLCPH	:8;
		unsigned long					:16;
	}bit;
};

/*	structure of HFAYASCLGA	(2841_2188h)	*/
union io_b2r_hfayasclga{
	unsigned long		word;
	struct {
		unsigned long	HFAYASCLGA_0	:8;
		unsigned long	HFAYASCLGA_1	:8;
		unsigned long					:16;
	}bit;
};

/*	structure of HFAYASCLBD	(2841_218Ch)	*/
union io_b2r_hfayasclbd{
	unsigned long		word;
	struct {
		unsigned long	HFAYASCLBD_0	:12;
		unsigned long					:4;
		unsigned long	HFAYASCLBD_1	:12;
		unsigned long					:4;
	}bit;
};

/*	structure of HFAYASCLCLP	(2841_2190h)	*/
union io_b2r_hfayasclclp{
	unsigned long		word;
	struct {
		unsigned long	HFAYASCLCLPL	:9;
		unsigned long					:7;
		unsigned long	HFAYASCLCLPH	:9;
		unsigned long					:7;
	}bit;
};

/*	structure of HFACVBFX	(2841_2194h)	*/
union io_b2r_hfacvbfx{
	unsigned long		word;
	struct {
		long			HFACRVBFX1	:8;
		long			HFACRVBFY1	:8;
		long			HFACRVBFX2	:8;
		long			HFACRVBFY2	:8;
	}bit;
};

/*	structure of HFACRVBSCLOF	(2841_2198h)	*/
union io_b2r_hfacrvbsclof{
	unsigned long		word;
	struct {
		unsigned long	HFACRVBSCLOF1	:8;
		unsigned long					:24;
	}bit;
};

/*	structure of HFACRVBSCLGA	(2841_219Ch)	*/
union io_b2r_hfacrvbsclga{
	unsigned long		word;
	struct {
		long			HFACRVBSCLGA_0	:11;
		unsigned long					:5;
		long			HFACRVBSCLGA_1	:11;
		unsigned long					:5;
	}bit;
};

/*	structure of HFACRVBSCLBD	(2841_21A0h)	*/
union io_b2r_hfacrvbsclbd{
	unsigned long		word;
	struct {
		unsigned long	HFACRVBSCLBD1	:7;
		unsigned long					:25;
	}bit;
};

/*	structure of HFACRVBSCLCP	(2841_21A4h)	*/
union io_b2r_hfacrvbsclcp{
	unsigned long		word;
	struct {
		unsigned long	HFACRVBSCLCPL	:8;
		unsigned long	HFACRVBSCLCPH	:8;
		unsigned long					:16;
	}bit;
};

/*	structure of HFAYBSCLGA	(2841_21A8h)	*/
union io_b2r_hfaybsclga{
	unsigned long		word;
	struct {
		unsigned long	HFAYBSCLGA_0	:8;
		unsigned long	HFAYBSCLGA_1	:8;
		unsigned long					:16;
	}bit;
};

/*	structure of HFAYBSCLBD	(2841_21ACh)	*/
union io_b2r_hfaybsclbd{
	unsigned long		word;
	struct {
		unsigned long	HFAYBSCLBD_0	:12;
		unsigned long					:4;
		unsigned long	HFAYBSCLBD_1	:12;
		unsigned long					:4;
	}bit;
};

/*	structure of HFAYBSCLCLP	(2841_21B0h)	*/
union io_b2r_hfaybsclclp{
	unsigned long		word;
	struct {
		unsigned long	HFAYBSCLCLPL	:9;
		unsigned long					:7;
		unsigned long	HFAYBSCLCLPH	:9;
		unsigned long					:7;
	}bit;
};

/*	structure of FB2R control area	2841_(2000 - 203Fh) */
struct io_b2r_fb2r_cntl{
	union	io_b2r_cntl			CNTL;			/* 2841_(2000 - 2003h) */
	union	io_b2r_b2rinte		B2RINTE;		/* 2841_(2004 - 2007h) */
	union	io_b2r_b2rintf		B2RINTF;		/* 2841_(2008 - 200Bh) */
	union	io_b2r_fltaen		FLTAEN;			/* 2841_(200C - 200Fh) */

	unsigned char dmy_2010_203F[0x2040-0x2010];	/* 2841_(2010 - 203Fh) */
};

/*	structure of Offset & WBGain area	2841_(2040 - 207Fh) */
struct io_b2r_ofg{
	union	io_b2r_ofstrb		OFSTRB;			/* 2841_(2040 - 2043h) */
	union	io_b2r_ofstg		OFSTG;			/* 2841_(2044 - 2047h) */
	union	io_b2r_blsft		BLSFT;			/* 2841_(2048 - 204Bh) */
	union	io_b2r_wbgarb		WBGARB;			/* 2841_(204C - 204Fh) */
	union	io_b2r_wbgagr		WBGAGR;			/* 2841_(2050 - 2053h) */
	union	io_b2r_wbhctl		WBHCTL;			/* 2841_(2054 - 2057h) */
	union	io_b2r_wbslp		WBSLP;			/* 2841_(2058 - 205Fh) */
	union	io_b2r_wblvrb		WBLVRB;			/* 2841_(2060 - 2063h) */
	union	io_b2r_wblvgr		WBLVGR;			/* 2841_(2064 - 2067h) */

	unsigned char dmy_2068_207F[0x2080-0x2068];	/* 2841_(2068 - 207Fh) */
};

/*	structure of CIA area	2841_(2080 - 20FFh) */
struct io_b2r_cia{
	union	io_b2r_sharp		SHARP;			/* 2841_(2080 - 2083h) */
	union	io_b2r_airesy		AIRESY;			/* 2841_(2084 - 2087h) */
	union	io_b2r_lf			LF;				/* 2841_(2088 - 208Bh) */
	union	io_b2r_cmc			CMC;			/* 2841_(208C - 208Fh) */
	union	io_b2r_dirrlith		DIRRLITH;		/* 2841_(2090 - 2093h) */

	unsigned char dmy_2094_20FF[0x2100-0x2094];	/* 2841_(2094 - 20FFh) */
};

/*	structure of HFA area	2841_(2100 - 21FFh) */
struct io_b2r_hfa{
	union	io_b2r_hfactl		HFACTL;			/* 2841_(2100 - 2103h) */

	unsigned char dmy_2104_210F[0x2110-0x2104];	/* 2841_(2104 - 210Fh) */

	union	io_b2r_hfk			HFK;			/* 2841_(2110 - 211Fh) */
	union	io_b2r_hfefclp		HFEFCLP;		/* 2841_(2120 - 2123h) */
	union	io_b2r_hfafst		HFAFST;			/* 2841_(2124 - 2127h) */
	union	io_b2r_hfafgu		HFAFGU;			/* 2841_(2128 - 212Bh) */
	union	io_b2r_hfafclp		HFAFCLP;		/* 2841_(212C - 212Fh) */
	union	io_b2r_hfayc		HFAYC;			/* 2841_(2130 - 213Fh) */
	union	io_b2r_hfayof		HFAYOF;			/* 2841_(2140 - 2147h) */
	union	io_b2r_hfayga		HFAYGA;			/* 2841_(2148 - 214Bh) */

	unsigned char dmy_214C_214F[0x2150-0x214C];	/* 2841_(214C - 214Fh) */

	union	io_b2r_hfaybd		HFAYBD;			/* 2841_(2150 - 2157h) */
	union	io_b2r_hfaydiv		HFAYDIV;		/* 2841_(2158 - 215Bh) */
	union	io_b2r_hfayclp		HFAYCLP;		/* 2841_(215C - 215Fh) */
	union	io_b2r_hfacor		HFACOR;			/* 2841_(2160 - 2163h) */

	unsigned char dmy_2164_2167[0x2168-0x2164];	/* 2841_(2164 - 2167h) */

	union	io_b2r_hfaclp		HFACLP;			/* 2841_(2168 - 216Fh) */
	union	io_b2r_hfacrvctl	HFACRVCTL;		/* 2841_(2170 - 2173h) */
	union	io_b2r_hfacvafx		HFACVAFX;		/* 2841_(2174 - 2177h) */
	union	io_b2r_hfacrvasclof	HFACRVASCLOF;	/* 2841_(2178 - 217Bh) */
	union	io_b2r_hfacrvasclga	HFACRVASCLGA;	/* 2841_(217C - 217Fh) */
	union	io_b2r_hfacrvasclbd	HFACRVASCLBD;	/* 2841_(2180 - 2183h) */
	union	io_b2r_hfacrvasclcp	HFACRVASCLCP;	/* 2841_(2184 - 2187h) */
	union	io_b2r_hfayasclga	HFAYASCLGA;		/* 2841_(2188 - 218Bh) */
	union	io_b2r_hfayasclbd	HFAYASCLBD;		/* 2841_(218C - 218Fh) */
	union	io_b2r_hfayasclclp	HFAYASCLCLP;	/* 2841_(2190 - 2193h) */
	union	io_b2r_hfacvbfx		HFACVBFX;		/* 2841_(2194 - 2197h) */
	union	io_b2r_hfacrvbsclof	HFACRVBSCLOF;	/* 2841_(2198 - 219Bh) */
	union	io_b2r_hfacrvbsclga	HFACRVBSCLGA;	/* 2841_(219C - 219Fh) */
	union	io_b2r_hfacrvbsclbd	HFACRVBSCLBD;	/* 2841_(21A0 - 21A3h) */
	union	io_b2r_hfacrvbsclcp	HFACRVBSCLCP;	/* 2841_(21A4 - 21A7h) */
	union	io_b2r_hfaybsclga	HFAYBSCLGA;		/* 2841_(21A8 - 21ABh) */
	union	io_b2r_hfaybsclbd	HFAYBSCLBD;		/* 2841_(21AC - 21AFh) */
	union	io_b2r_hfaybsclclp	HFAYBSCLCLP;	/* 2841_(21B0 - 21B3h) */

	unsigned char dmy_21B4_21FF[0x2200-0x21B4];	/* 2841_(21B4 - 21FFh) */
};

/* Define i/o mapping */
struct io_b2r_fb2r{
	/* JDSB2R */
	struct io_b2r_fb2r_cntl		CNTL;			/* 2841_(2000 - 203Fh) */
	struct io_b2r_ofg			OFG;			/* 2841_(2040 - 207Fh) */
	struct io_b2r_cia			CIA;			/* 2841_(2080 - 20FFh) */
	struct io_b2r_hfa			HFA;			/* 2841_(2100 - 21FFh) */
};
