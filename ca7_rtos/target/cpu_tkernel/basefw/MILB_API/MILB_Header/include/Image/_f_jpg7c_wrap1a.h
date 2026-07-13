/**
 * @file		_f_jpg7c_wrap1a.h
 * @brief		Definition F_JPG7C_WRAP1A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*	structure of JPMODE				(298C_0000h)	*/
union io_jpeg7c_jpmode{
	unsigned long		word;
	struct {
		unsigned long	MMODE		:3;
		unsigned long	DISPOSAL	:1;
		unsigned long	P_ENDIAN	:1;
		unsigned long	S_ENDIAN	:1;
		unsigned long				:26;
	}bit;
};

/*	structure of JPCMD				(298C_0004h)	*/
union io_jpeg7c_jpcmd{
	unsigned long		word;
	struct {
		unsigned long	JSTART		:1;
		unsigned long	JRSTART		:1;
		unsigned long				:29;
		unsigned long	JRESET		:1;
	}bit;
};

/*	structure of JPSTATUS			(298C_0008h)	*/
union io_jpeg7c_jpstatus{
	unsigned long		word;
	struct {
		unsigned long	JPSTATUS	:1;
		unsigned long				:31;
	}bit;
};

/*	structure of JPQTABLE			(298C_000Ch)	*/
union io_jpeg7c_jpqtable{
	unsigned long		word;
	struct {
		unsigned long	QT0L		:1;
		unsigned long	QT0U		:1;
		unsigned long	QT1L		:1;
		unsigned long	QT1U		:1;
		unsigned long	QT2L		:1;
		unsigned long	QT2U		:1;
		unsigned long				:26;
	}bit;
};

/*	structure of JPHTABLE			(298C_0010h)	*/
union io_jpeg7c_jphtable{
	unsigned long		word;
	struct {
		unsigned long	HT0DC		:1;
		unsigned long	HT0AC		:1;
		unsigned long	HT1DC		:1;
		unsigned long	HT1AC		:1;
		unsigned long	HT2DC		:1;
		unsigned long	HT2AC		:1;
		unsigned long				:26;
	}bit;
};

/*	structure of JPDRISET			(298C_0014h)	*/
union io_jpeg7c_jpdriset{
	unsigned long		word;
	struct {
		unsigned long	JPDRISET	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of JPHEIGHT			(298C_0018h)	*/
union io_jpeg7c_jpheight{
	unsigned long		word;
	struct {
		unsigned long	JPHEIGHT	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of JPWIDTH			(298C_001Ch)	*/
union io_jpeg7c_jpwidth{
	unsigned long		word;
	struct {
		unsigned long	JPWIDTH		:16;
		unsigned long				:16;
	}bit;
};

/*	structure of JPBYTCNT			(298C_0020h)	*/
union io_jpeg7c_jpbytcnt{
	unsigned long		word;
	struct {
		unsigned long	JPBYTCNT	:32;
	}bit;
};

/*	structure of JPFORMAT			(298C_0024h)	*/
union io_jpeg7c_jpformat{
	unsigned long		word;
	struct {
		unsigned long	CODECNT		:1;
		unsigned long	MKSKIP		:1;
		unsigned long	EXIFMOD		:1;
		unsigned long				:29;
	}bit;
};

/*	structure of JPINTSET			(298C_0028h)	*/
union io_jpeg7c_jpintset{
	unsigned long		word;
	struct {
		unsigned long	APPMKSET	:1;
		unsigned long	COMMKSET	:1;
		unsigned long	UMKSET		:1;
		unsigned long	REGRDSET	:1;
		unsigned long	QTWSET		:1;
		unsigned long				:3;
		unsigned long	RSTSET		:1;
		unsigned long	SIZSET		:1;
		unsigned long	CORSET		:1;
		unsigned long				:20;
		unsigned long	INTSET		:1;
	}bit;
};

/*	structure of JPINTSTA			(298C_002Ch)	*/
union io_jpeg7c_jpintsta{
	unsigned long		word;
	struct {
		unsigned long	APPMKINFO	:1;
		unsigned long	COMMKINFO	:1;
		unsigned long	UMKSINFO	:1;
		unsigned long	REGRDINT	:1;
		unsigned long	QTWINT		:1;
		unsigned long				:3;
		unsigned long	RSTERR		:1;
		unsigned long	SIZERR		:1;
		unsigned long	ERRFLG		:1;
		unsigned long				:19;
		unsigned long	COREND		:1;
		unsigned long	INTFLG		:1;
	}bit;
};

/*	structure of JPERRCOD			(298C_0030h)	*/
union io_jpeg7c_jperrcod{
	unsigned long		word;
	struct {
		unsigned long	JPERRCOD	:6;
		unsigned long				:26;
	}bit;
};

/*	structure of JPCORSTA			(298C_0034h)	*/
union io_jpeg7c_jpcorsta{
	unsigned long		word;
	struct {
		unsigned long	CORDRIINFO	:1;
		unsigned long	CORSIZINFO	:1;
		unsigned long				:30;
	}bit;
};

/*	structure of JPSUSPEND			(298C_0038h)	*/
union io_jpeg7c_jpsuspend{
	unsigned long		word;
	struct {
		unsigned long	JPSUSPEND	:1;
		unsigned long				:31;
	}bit;
};

/*	structure of JPDNLSET			(298C_003Ch)	*/
union io_jpeg7c_jpdnlset{
	unsigned long		word;
	struct {
		unsigned long	JPDNLSET	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of Quantization table		(298C_0100h) (298C_0140h) (298C_0180h) (298C_01C0h)	*/
union io_jpeg7c_jpqt{
	unsigned long		word[16];
	struct {
		unsigned long	QT3		:8;
		unsigned long	QT2		:8;
		unsigned long	QT1		:8;
		unsigned long	QT0		:8;
	}bit[16];
};

/*	structure of Huffman DC Num			(298C_0200h) (298C_0300h)	*/	// Not Used
union io_jpeg7c_jphtdn{
	unsigned long		word[4];
	struct {
		unsigned long	HTDN4	:8;
		unsigned long	HTDN3	:8;
		unsigned long	HTDN2	:8;
		unsigned long	HTDN1	:8;
	}bit[4];
};

/*	structure of Huffman DC Group No	(298C_0210h) (298C_0310h)	*/	// Not Used
union io_jpeg7c_jphtds{
	unsigned long		word[3];
	struct {
		unsigned long	HTDS3	:8;
		unsigned long	HTDS2	:8;
		unsigned long	HTDS1	:8;
		unsigned long	HTDS0	:8;
	}bit[3];
};

/*	structure of Huffman AC Num			(298C_0220h) (298C_0320h)	*/	// Not Used
union io_jpeg7c_jphtan{
	unsigned long		word[4];
	struct {
		unsigned long	HTAN4	:8;
		unsigned long	HTAN3	:8;
		unsigned long	HTAN2	:8;
		unsigned long	HTAN1	:8;
	}bit[4];
};

/*	structure of Huffman AC RS			(298C_0230h) (298C_0330h)	*/	// Not Used
union io_jpeg7c_jphtars{
	unsigned long		word[40];
	struct {
		unsigned long	HTARS3	:8;
		unsigned long	HTARS2	:8;
		unsigned long	HTARS1	:8;
		unsigned long	HTARS0	:8;
	}bit[40];
};

/*	structure of Huffman AC RS			(298C_02D0h) (298C_03D0h)	*/	// Not Used
union io_jpeg7c_jphtars28{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	HTARS162	:8;
		unsigned long	HTARS161	:8;
	}bit;
};

/*	structure of JICTL			(298C_0400h)	*/
union io_jpeg7c_jictl{
	unsigned long		word;
	struct {
		unsigned long				:1;
		unsigned long	B12M		:1;
		unsigned long	JIRP		:2;
		unsigned long	JIXP		:2;
		unsigned long				:8;
		unsigned long	JITRG		:2;
		unsigned long				:16;
	}bit;
};

/*	structure of JCCTL			(298C_0404h)	*/
union io_jpeg7c_jcctl{
	unsigned long		word;
	struct {
		unsigned long	JCPS		:1;
		unsigned long	JCC			:1;
		unsigned long	JCRP		:2;
		unsigned long				:10;
		unsigned long	JCTRG		:2;
		unsigned long				:16;
	}bit;
};

/*	structure of JMODE			(298C_0408h)	*/
union io_jpeg7c_jmode{
	unsigned long		word;
	struct {
		unsigned long	JBURST_AL_ON	:1;
		unsigned long					:9;
		unsigned long	JJBENDIAN		:1;
		unsigned long	JPBENDIAN		:1;
		unsigned long					:4;
		unsigned long	JMEMFORM		:2;
		unsigned long					:2;
		unsigned long	JPBDEXT			:1;
		unsigned long					:7;
		unsigned long	JCSCCCEN		:1;
		unsigned long					:3;
	}bit;
};

/*	structure of JIMGYGH		(298C_0410h)	*/
union io_jpeg7c_jimgygh{
	unsigned long		word;
	struct {
		unsigned long	JIMGYGH		:20;
		unsigned long				:12;
	}bit;
};

/*	structure of JIMGCGH		(298C_0414h)	*/
union io_jpeg7c_jimgcgh{
	unsigned long		word;
	struct {
		unsigned long	JIMGCGH		:20;
		unsigned long				:12;
	}bit;
};

/*	structure of JIMGSH			(298C_0418h)	*/
union io_jpeg7c_jimgsh{
	unsigned long		word;
	struct {
		unsigned long	JIMGSH		:16;
		unsigned long				:16;
	}bit;
};

/*	structure of JIMGSV			(298C_041Ch)	*/
union io_jpeg7c_jimgsv{
	unsigned long		word;
	struct {
		unsigned long	JIMGSV		:16;
		unsigned long				:16;
	}bit;
};

/*	structure of JIMGSHDG		(298C_0420h)	*/
union io_jpeg7c_jimgshdg{
	unsigned long		word;
	struct {
		unsigned long	JIMGSHDG	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of JHOFFSET		(298C_0424h)	*/
union io_jpeg7c_jhoffset{
	unsigned long		word;
	struct {
		unsigned long	JHOFFSET	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of JVOFFSET		(298C_0428h)	*/
union io_jpeg7c_jvoffset{
	unsigned long		word;
	struct {
		unsigned long	JVOFFSET	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of JIMGAY			(298C_0430h)	*/
union io_jpeg7c_jimgay{
	unsigned long		word;
	struct {
		unsigned long	JIMGAY		:32;
	}bit;
};

/*	structure of JIMGACB		(298C_0434h)	*/
union io_jpeg7c_jimgacb{
	unsigned long		word;
	struct {
		unsigned long	JIMGACB		:32;
	}bit;
};

/*	structure of JC0DA			(298C_0440h)	*/
union io_jpeg7c_jcoda{
	unsigned long		word;
	struct {
		unsigned long	JCODA		:32;
	}bit;
};

/*	structure of JLINEVAL		(298C_0450h)	*/
union io_jpeg7c_jlineval{
	unsigned long		word;
	struct {
		unsigned long	JLINEVAL	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of JSECTVAL		(298C_0454h)	*/
union io_jpeg7c_jsectval{
	unsigned long		word;
	struct {
		unsigned long	JSECTVAL	:32;
	}bit;
};

/*	structure of JC0DSV			(298C_0458h)	*/
union io_jpeg7c_jcodsv{
	unsigned long		word;
	struct {
		unsigned long	JCODSV		:32;
	}bit;
};

/*	structure of JISVMN			(298C_0470h)	*/
union io_jpeg7c_jisvmn{
	unsigned long		word;
	struct {
		unsigned long	JISVMN		:16;
		unsigned long				:16;
	}bit;
};

/*	structure of JSCTC			(298C_0474h)	*/
union io_jpeg7c_jsctc{
	unsigned long		word;
	struct {
		unsigned long	JSCTC		:32;
	}bit;
};

/*	structure of JINT			(298C_0480h)	*/
union io_jpeg7c_jint{
	unsigned long		word;
	struct {
		unsigned long				:12;
		unsigned long	__JCPF		:1;
		unsigned long				:1;
		unsigned long	__JCF		:1;
		unsigned long	__JIF		:1;
		unsigned long	__JSECTINT	:1;
		unsigned long	__JLINEINT	:1;
		unsigned long	__JCLRFMER	:1;
		unsigned long				:3;
		unsigned long	__JJBINT	:1;
		unsigned long	__JPBINT	:1;
		unsigned long	JJIRQST		:3;
		unsigned long	JJIRQ		:1;
		unsigned long				:1;
		unsigned long	JERRINT		:1;
		unsigned long				:1;
		unsigned long	JALLINT		:1;
	}bit;
};

/*	structure of JINTEN			(298C_0484h)	*/
union io_jpeg7c_jinten{
	unsigned long		word;
	struct {
		unsigned long				:12;
		unsigned long	JCPFEN		:1;
		unsigned long				:1;
		unsigned long	JCFEN		:1;
		unsigned long	JIFEN		:1;
		unsigned long	JSECTEN		:1;
		unsigned long	JLINEEN		:1;
		unsigned long	JCLRFMEREN	:1;
		unsigned long				:3;
		unsigned long	JJBEN		:1;
		unsigned long	JPBEN		:1;
		unsigned long				:8;
	}bit;
};

/*	structure of JSTATE			(298C_0488h)	*/
union io_jpeg7c_jstate{
	unsigned long		word;
	struct {
		unsigned long				:6;
		unsigned long	JJBACTP		:1;
		unsigned long	JPBACTP		:1;
		unsigned long				:7;
		unsigned long	JALLACTP	:1;
		unsigned long				:6;
		unsigned long	JJBRSTP		:1;
		unsigned long	JPBRSTP		:1;
		unsigned long				:7;
		unsigned long	JALLRSTP	:1;
	}bit;
};

/*	structure of JPBSTA			(298C_0490h)	*/
union io_jpeg7c_jpbsta{
	unsigned long		word;
	struct {
		unsigned long	JPAPROT		:3;
		unsigned long				:1;
		unsigned long	JPACACHE	:4;
		unsigned long				:12;
		unsigned long	JPISSUESET	:2;
		unsigned long				:2;
		unsigned long	JPRES		:2;
		unsigned long				:6;
	}bit;
};

/*	structure of JJBSTA			(298C_0494h)	*/
union io_jpeg7c_jjbsta{
	unsigned long		word;
	struct {
		unsigned long	JJAPROT		:3;
		unsigned long				:1;
		unsigned long	JJACACHE	:4;
		unsigned long				:12;
		unsigned long	JJISSUESET	:2;
		unsigned long				:2;
		unsigned long	JJRES		:2;
		unsigned long				:6;
	}bit;
};

/*	structure of JBANDCOLOR			(298C_04B0h)	*/
union io_jpeg7c_jbandcolor{
	unsigned long		word;
	struct {
		unsigned long	JBANDY		:8;
		unsigned long	JBANDCB		:8;
		unsigned long	JBANDCR		:8;
		unsigned long				:8;
	}bit;
};

/*	structure of JCSCCC0-1			(298C_04C0h)	*/
/*	structure of JCSCCC1-1			(298C_04C8h)	*/
/*	structure of JCSCCC2-1			(298C_04D0h)	*/
union io_jpeg7c_jcsccc1{
	unsigned long		word;
	struct {
		unsigned long	JCSCCC0		:10;
		unsigned long				:6;
		unsigned long	JCSCCC1		:10;
		unsigned long				:6;
	}bit;
};

/*	structure of JCSCCC0-2			(298C_04C4h)	*/
/*	structure of JCSCCC1-2			(298C_04CCh)	*/
/*	structure of JCSCCC2-2			(298C_04D4h)	*/
union io_jpeg7c_jcsccc2{
	unsigned long		word;
	struct {
		unsigned long	JCSCCC2		:10;
		unsigned long				:22;
	}bit;
};

/*	structure of IIPSET				(298C_04E0h)	*/
union io_jpeg7c_iipset{
	unsigned long		word;
	struct {
		unsigned long	PORT_MODE	:3;
		unsigned long	RING_ON		:1;
		unsigned long	BLK_SIZE	:2;
		unsigned long				:2;
		unsigned long	RING_SIZE	:8;
		unsigned long				:16;
	}bit;
};

/* Define i/o mapping */
struct io_f_jpg7c_wrap{
	union io_jpeg7c_jpmode		JPMODE;			/* 298C_(0000 - 0003h) */
	union io_jpeg7c_jpcmd		JPCMD;			/* 298C_(0004 - 0007h) */
	union io_jpeg7c_jpstatus	JPSTATUS;		/* 298C_(0008 - 000Bh) */
	union io_jpeg7c_jpqtable	JPQTABLE;		/* 298C_(000C - 000Fh) */
	union io_jpeg7c_jphtable	JPHTABLE;		/* 298C_(0010 - 0013h) */
	union io_jpeg7c_jpdriset	JPDRISET;		/* 298C_(0014 - 0017h) */
	union io_jpeg7c_jpheight	JPHEIGHT;		/* 298C_(0018 - 001Bh) */
	union io_jpeg7c_jpwidth		JPWIDTH;		/* 298C_(001C - 001Fh) */
	union io_jpeg7c_jpbytcnt	JPBYTCNT;		/* 298C_(0020 - 0023h) */
	union io_jpeg7c_jpformat	JPFORMAT;		/* 298C_(0024 - 0027h) */
	union io_jpeg7c_jpintset	JPINTSET;		/* 298C_(0028 - 002Bh) */
	union io_jpeg7c_jpintsta	JPINTSTA;		/* 298C_(002C - 002Fh) */
	union io_jpeg7c_jperrcod	JPERRCOD;		/* 298C_(0030 - 0033h) */
	union io_jpeg7c_jpcorsta	JPCORSTA;		/* 298C_(0034 - 0037h) */
	union io_jpeg7c_jpsuspend	JPSUSPEND;		/* 298C_(0038 - 003Bh) */
	union io_jpeg7c_jpdnlset	JPDNLSET;		/* 298C_(003C - 003Fh) */

	unsigned char dmy_0040_00FF[0x0100-0x0040];	/* 298C_(0040 - 00FFh) */

	union io_jpeg7c_jpqt		JPQT0;			/* 298C_(0100 - 013Fh) */
	union io_jpeg7c_jpqt		JPQT1;			/* 298C_(0140 - 017Fh) */
	union io_jpeg7c_jpqt		JPQT2;			/* 298C_(0180 - 01BFh) */
	union io_jpeg7c_jpqt		JPQT3;			/* 298C_(01C0 - 01FFh) */

	union io_jpeg7c_jphtdn		JPHTD0N;		/* 298C_(0200 - 020Fh) */
	union io_jpeg7c_jphtds		JPHTD0S;		/* 298C_(0210 - 021Bh) */

	unsigned char dmy_021C_021F[0x0220-0x021C];	/* 298C_(021C - 021Fh) */

	union io_jpeg7c_jphtan		JPHTA0N;		/* 298C_(0220 - 022Fh) */
	union io_jpeg7c_jphtars		JPHTA0RS;		/* 298C_(0230 - 02CFh) */
	union io_jpeg7c_jphtars28	JPHTA0RS28;		/* 298C_(02D0 - 02D3h) */

	unsigned char dmy_02D4_02FF[0x0300-0x02D4];	/* 298C_(02D4 - 02FFh) */

	union io_jpeg7c_jphtdn		JPHTD1N;		/* 298C_(0300 - 030Fh) */
	union io_jpeg7c_jphtds		JPHTD1S;		/* 298C_(0310 - 031Bh) */

	unsigned char dmy_031C_031F[0x0320-0x031C];	/* 298C_(031C - 031Fh) */

	union io_jpeg7c_jphtan		JPHTA1N;		/* 298C_(0320 - 032Fh) */
	union io_jpeg7c_jphtars		JPHTA1RS;		/* 298C_(0330 - 03CFh) */
	union io_jpeg7c_jphtars28	JPHTA1RS28;		/* 298C_(03D0 - 03D3h) */

	unsigned char dmy_03D4_03FF[0x0400-0x03D4];	/* 298C_(03D4 - 03FFh) */

	union io_jpeg7c_jictl		JICTL;			/* 298C_(0400 - 0403h) */
	union io_jpeg7c_jcctl		JCCTL;			/* 298C_(0404 - 0407h) */
	union io_jpeg7c_jmode		JMODE;			/* 298C_(0408 - 040Bh) */

	unsigned char dmy_040C_040F[0x0410-0x040C];	/* 298C_(040C - 040Fh) */

	union io_jpeg7c_jimgygh		JIMGYGH;		/* 298C_(0410 - 0413h) */
	union io_jpeg7c_jimgcgh		JIMGCGH;		/* 298C_(0414 - 0417h) */
	union io_jpeg7c_jimgsh		JIMGSH;			/* 298C_(0418 - 041Bh) */
	union io_jpeg7c_jimgsv		JIMGSV;			/* 298C_(041C - 041Fh) */
	union io_jpeg7c_jimgshdg	JIMGSHDG;		/* 298C_(0420 - 0423h) */
	union io_jpeg7c_jhoffset	JHOFFSET;		/* 298C_(0424 - 0427h) */
	union io_jpeg7c_jvoffset	JVOFFSET;		/* 298C_(0428 - 042Bh) */

	unsigned char dmy_042C_042F[0x0430-0x042C];	/* 298C_(042C - 042Fh) */

	union io_jpeg7c_jimgay		JIMGAY;			/* 298C_(0430 - 0433h) */
	union io_jpeg7c_jimgacb		JIMGACB;		/* 298C_(0434 - 0437h) */

	unsigned char dmy_0438_043F[0x0440-0x0438];	/* 298C_(0438 - 043Fh) */

	union io_jpeg7c_jcoda		JCODA;			/* 298C_(0440 - 0443h) */

	unsigned char dmy_0444_044F[0x0450-0x0444];	/* 298C_(0444 - 044Fh) */

	union io_jpeg7c_jlineval	JLINEVAL;		/* 298C_(0450 - 0453h) */
	union io_jpeg7c_jsectval	JSECTVAL;		/* 298C_(0454 - 0457h) */
	union io_jpeg7c_jcodsv		JCODSV;			/* 298C_(0458 - 045Bh) */

	unsigned char dmy_045C_046F[0x0470-0x045C];	/* 298C_(045C - 046Fh) */

	union io_jpeg7c_jisvmn		JISVMN;			/* 298C_(0470 - 0473h) */
	union io_jpeg7c_jsctc		JSCTC;			/* 298C_(0474 - 0477h) */

	unsigned char dmy_0478_047F[0x0480-0x0478];	/* 298C_(0478 - 047Fh) */

	union io_jpeg7c_jint		JINT;			/* 298C_(0480 - 0483h) */
	union io_jpeg7c_jinten		JINTEN;			/* 298C_(0484 - 0487h) */
	union io_jpeg7c_jstate		JSTATE;			/* 298C_(0488 - 048Bh) */

	unsigned char dmy_048C_048F[0x0490-0x048C];	/* 298C_(048C - 048Fh) */

	union io_jpeg7c_jpbsta		JPBSTA;			/* 298C_(0490 - 0493h) */
	union io_jpeg7c_jjbsta		JJBSTA;			/* 298C_(0494 - 0497h) */

	unsigned char dmy_0498_04AF[0x04B0-0x0498];	/* 298C_(0498 - 04AFh) */

	union io_jpeg7c_jbandcolor	JBANDCOLOR;		/* 298C_(04B0 - 04B3h) */

	unsigned char dmy_04B4_04BF[0x04C0-0x04B4];	/* 298C_(04B4 - 04BFh) */

	union io_jpeg7c_jcsccc1		JCSCCC0_1;		/* 298C_(04C0 - 04C3h) */
	union io_jpeg7c_jcsccc2		JCSCCC0_2;		/* 298C_(04C4 - 04C7h) */
	union io_jpeg7c_jcsccc1		JCSCCC1_1;		/* 298C_(04C8 - 04CBh) */
	union io_jpeg7c_jcsccc2		JCSCCC1_2;		/* 298C_(04CC - 04CFh) */
	union io_jpeg7c_jcsccc1		JCSCCC2_1;		/* 298C_(04D0 - 04D3h) */
	union io_jpeg7c_jcsccc2		JCSCCC2_2;		/* 298C_(04D4 - 04D7h) */

	unsigned char dmy_04D8_04DF[0x04E0-0x04D8];	/* 298C_(04D8 - 04DFh) */

	union io_jpeg7c_iipset		IIPSET;			/* 298C_(04E0 - 04E3h) */

	unsigned char dmy_298C04E4_298C0FFF[0x298C1000-0x298C04E4];	/* 298C_04E4 - 298C_0FFFh) */
};

extern volatile struct io_f_jpg7c_wrap	IO_JPG7;
